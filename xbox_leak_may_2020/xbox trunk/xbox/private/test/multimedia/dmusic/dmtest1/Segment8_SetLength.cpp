/*++

Copyright (c) 2001 Microsoft Corporation

Module Name:

	Segment8_SetLength.cpp

Abstract:

	Tests the SetLength methods of IDirectMusicSegment8

Author:

	Dan Haffner (danhaff) 

Revision History:

	20-Mar-2001 robheit
		Added all Valid tests
    01-Aug-2001 danhaff
        Combied 4 tests into one, calling now from external test struct

--*/

//------------------------------------------------------------------------------
//	Includes:
//------------------------------------------------------------------------------
#include "globals.h"
#include "CICMusicX.h"

//------------------------------------------------------------------------------
//	Function Prototypes:
//------------------------------------------------------------------------------
HRESULT Segment8_SetLength_Range(IN CtIDirectMusicPerformance8*	ptPerf8, DWORD dwMusicTimeLength,DWORD dwUnused2);

//------------------------------------------------------------------------------
//	Segment8_SetLength_BVT
//------------------------------------------------------------------------------
HRESULT 
Segment8_SetLength_BVT(
					   IN CtIDirectMusicPerformance8*	ptPerf8, 
					   IN DWORD							dwUnused1, 
					   IN DWORD							dwUnused2
					   )
/*++

Routine Description:

	BVT test for IDirectMusicSegment8::SetLength

Arguments:

	IN ptPerf8 -	Performance object
	IN dwUnused1 -	Unused 
	IN dwUnused2 -	Unused

Return Value:

	S_OK on success, any other value on failure

--*/
{
    HRESULT hr = S_OK;
    Log(FYILOGLEVEL, "Calling main test function Segment8_SetLength_BVT())");

    DMTEST_EXECUTE(Segment8_SetLength_Range(ptPerf8, 100, 0));

    return hr;
}

//------------------------------------------------------------------------------
//	Segment8_SetLength_Test1_Valid
//------------------------------------------------------------------------------
HRESULT 
Segment8_SetLength_Test1_Valid(
							   IN CtIDirectMusicPerformance8*	ptPerf8, 
							   IN DWORD							dwUnused1,
							   IN DWORD							dwUnused2
							   )
/*++

Routine Description:

	Valid test for IDirectMusicSegment8::SetLength

Arguments:

	IN ptPerf8 -	Performance object
	IN dwUnused1 -	Unused 
	IN dwUnused2 -	Unused

Return Value:

	S_OK on success, any other value on failure

--*/
{
    DWORD						i;
    CICMusic					Music;
    HRESULT						hr			= S_OK;
	BOOL						failed		= FALSE;
    CtIDirectMusicSegment8*		ptSegment8	= NULL;
    CtIDirectMusicAudioPath*	ptPath		= NULL;
    MUSIC_TIME					mtTime[]	= {DMUS_PPQ*4, DMUS_PPQ-1, DMUS_PPQ, DMUS_PPQ+1, MT_MIN, MT_MAX};
	DWORD						numValues	= sizeof(mtTime) / sizeof(MUSIC_TIME);

    CHECKRUN(MediaCopyFile("DMusic/DMTest1/Segment8/SetRep/Repeat5.sgt"));
    CHECKRUN(MediaCopyFile("DMusic/DMTest1/Segment8/SetRep/Demo.dls"));	
    CHECKRUN(Music.Init(ptPerf8, "T:\\DMTest1\\Segment8\\SetRep\\Repeat5.sgt",
						DMUS_APATH_SHARED_STEREOPLUSREVERB));
    CHECKRUN(Music.GetInterface(&ptSegment8));
    CHECKRUN(Music.GetInterface(&ptPath));
	if(FAILED(hr))
	{
		SAFE_RELEASE(ptSegment8);
		return E_FAIL;
	}
    CHECKRUN(ptSegment8->SetRepeats(0));
	
    //Try all these length values.
    for(i=0; i<numValues; ++i)
    {
		// Set Repeats
		Log(FYILOGLEVEL, "Calling SetLength(%u)", mtTime[i]);
		CHECKRUN(ptSegment8->SetLength(mtTime[i]));

		// Play
		CHECKRUN(ptPerf8->PlaySegmentEx(ptSegment8, 0, NULL, 0, __int64(0), NULL, NULL, ptPath));
		ExpectSegmentStart(ptPerf8, ptSegment8, NULL, 5000, NULL, TRUE);

        //Wait until done
		Log(FYILOGLEVEL, "Waiting until segment stops");
		if(FAILED(WaitForSegmentStop(ptPerf8, ptSegment8, NULL, 0xffffffff, TRUE)))
			CHECKRUN(ptPerf8->StopEx(0, __int64(0), 0));
		dmthPromptUserTestResults(hr, EXPECT_YES, "Did the audio play for the correct length?");
	}

    SAFE_RELEASE(ptSegment8);
    SAFE_RELEASE(ptPath);

    return failed ? E_FAIL : S_OK;
}



//------------------------------------------------------------------------------
//	Segment8_SetLength_Range
//------------------------------------------------------------------------------
HRESULT Segment8_SetLength_Range(IN CtIDirectMusicPerformance8*	ptPerf8, DWORD dwMusicTimeLength,DWORD dwUnused2)
/*++

Routine Description:

	Portion of the BVT test for IDirectMusicSegment8::SetLength

Arguments:

	IN ptPerf8 -	Performance object
	IN mtLength -	

Return Value:

	None

--*/
{
	CICMusic				Music;
	HRESULT					hr				= S_OK;
	CtIDirectMusicSegment8*	ptSegment8		= NULL;
	LPSTR					szSegmentName	= "T:\\DMTest1\\Segment8\\GetLength\\1Bar.sgt";
    MUSIC_TIME              mtLength        = (MUSIC_TIME)dwMusicTimeLength;
	MUSIC_TIME				mtLength2		= 0;

    CHECKRUN(MediaCopyFile("SGT/1Bar.sgt",               "T:\\DMTest1\\Segment8\\GetLength\\"));
    CHECKRUN(MediaCopyFile("DLS/Main1.DLS",              "T:\\DMTest1\\Segment8\\GetLength\\"));
	CHECKRUN(Music.Init(ptPerf8, szSegmentName, DMUS_APATH_SHARED_STEREOPLUSREVERB));

	Log(FYILOGLEVEL, "FYI: Playing for 1 second with default length");
	Wait(1000);

	CHECKRUN(Music.GetInterface(&ptSegment8));
	CHECKRUN(ptSegment8->SetLength(mtLength));
	CHECKRUN(ptSegment8->GetLength(&mtLength2));
	if (mtLength != mtLength2)
	{
		Log(ABORTLOGLEVEL, "%s: Set length to %d but returned length was %d.", szSegmentName, mtLength, mtLength2);
		hr = E_FAIL;
	}

	Log(FYILOGLEVEL, "FYI: Playing for 4 seconds with length = %d", mtLength2);
	Wait(4000);

	SAFE_RELEASE(ptSegment8);
	return hr;
}
