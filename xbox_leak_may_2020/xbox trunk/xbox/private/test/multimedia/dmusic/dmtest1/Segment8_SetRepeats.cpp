/*++

Copyright (c) 2001 Microsoft Corporation

Module Name:

	Segment8_SetRepeates.cpp

Abstract:

	Tests the functionality of IDirectMusicSegment8::SetRepeats

Author:

	Dan Haffner (danhaff) 

Revision History:

	19-Mar-2001 robheit
		Added all Valid Tests

--*/

//------------------------------------------------------------------------------
//	Includes:
//------------------------------------------------------------------------------
#include "globals.h"
#include "CICMusicX.h"

//------------------------------------------------------------------------------
//	Static Function Declarations:
//------------------------------------------------------------------------------
HRESULT Segment8_SetRepeats_BVT1(CtIDirectMusicPerformance8* ptPerf8);

//------------------------------------------------------------------------------
//	Segment8_SetRepeats_BVT
//------------------------------------------------------------------------------
HRESULT 
Segment8_SetRepeats_BVT(
						IN CtIDirectMusicPerformance8*	ptPerf8, 
						IN DWORD						dwUnused1,
						IN DWORD						dwUnused2
						)
/*++

Routine Description:

	BVT tests or IDirectMusicSegment8::SetRepeats

Arguments:

	IN ptPerf8 -	Performance object 
	IN dwUnused1 -	Unused
	IN dwUnused2 -	Unused

Return Value:

	S_OK on success, any other value on failure

--*/
{
    HRESULT hr = S_OK;
    Log(FYILOGLEVEL, "Calling BVT test function Segment8_SetRepeats_BVT())");
    DMTEST_EXECUTE(Segment8_SetRepeats_BVT1(ptPerf8));
    return hr;
}

//------------------------------------------------------------------------------
//	Segment8_SetRepeats_Test1_Valid
//------------------------------------------------------------------------------
HRESULT 
Segment8_SetRepeats_Test1_Valid(
								IN CtIDirectMusicPerformance8*	ptPerf8, 
								IN DWORD						dwUnused1,
								IN DWORD						dwUnused2
								)
/*++

Routine Description:

	Valid test for IDirectMusicSegment8::SetRepeats

Arguments:

	IN ptPerf8 -	Performance object 
	IN dwUnused1 -	Unused
	IN dwUnused2 -	Unused

Return Value:

	S_OK on success, any other value on failure

--*/
{
    CICMusic					Music;
    HRESULT						hr					= S_OK;
	BOOL						failed				= FALSE;
    CtIDirectMusicSegment8*		ptSegment8			= NULL;
    CtIDirectMusicAudioPath*	ptPath				= NULL;
    DWORD						dwRepeatValues[]	= {0, 1, 2, 5, DMUS_SEG_REPEAT_INFINITE };
	DWORD						numValues			= sizeof(dwRepeatValues) / sizeof(DWORD);
    DWORD						i;

    Log(FYILOGLEVEL, "Calling Valid test function Segment8_SetRepeats_Test1_Valid())");

    MEDIAFILEDESC MediaFileDesc[] = {
                                {"DMusic/DMTest1/Segment8/SetRep/Repeat1.sgt",     "T:\\DMTest1\\Segment8\\SetRep\\",    COPY_IF_NEWER},
                                {"WAV/GuitarClip1.wav",            "T:\\DMTest1\\Segment8\\SetRep\\",    COPY_IF_NEWER},
                                {TERMINATE}
                                };
    CHECKRUN(LoadMedia(MediaFileDesc));

	
    CHECKRUN(Music.Init(ptPerf8, "T:\\DMTest1\\Segment8\\SetRep\\Repeat1.sgt", DMUS_APATH_SHARED_STEREOPLUSREVERB));
    CHECKRUN(Music.GetInterface(&ptSegment8));
    CHECKRUN(Music.GetInterface(&ptPath));
	if(FAILED(hr))
	{
		SAFE_RELEASE(ptSegment8);
		return E_FAIL;
	}
	
    //Try all these repeat values.
    for(i=0; i<numValues; ++i)
    {
		// Set Repeats
		Log(FYILOGLEVEL, "Calling SetRepeats(%u)",dwRepeatValues[i]);
		CHECKRUN(ptSegment8->SetRepeats(dwRepeatValues[i]));

		// Play
		CHECKRUN(ptPerf8->PlaySegmentEx(ptSegment8, 0, NULL, 0, __int64(0), NULL, NULL, ptPath));
		ExpectSegmentStart(ptPerf8, ptSegment8, NULL, 5000, NULL);

        //Wait until done
        if(dwRepeatValues[i] != DMUS_SEG_REPEAT_INFINITE)
        {
            Log(FYILOGLEVEL, "Waiting until segment stops");
			ExpectSegmentStop(ptPerf8, ptSegment8, NULL, 0xffffffff, NULL, TRUE);
			dmthPromptUserTestResults(hr, EXPECT_YES, "Did the audio loop %u time(s)?", dwRepeatValues[i] != 0 ? dwRepeatValues[i] : 1);
        }
        else
        {
			dmthPromptUserTestResults(hr, EXPECT_YES, "Is the audio looping forever?");
	        CHECKRUN(ptPerf8->StopEx(0, __int64(0), 0));
        }
	}

    SAFE_RELEASE(ptSegment8);
    SAFE_RELEASE(ptPath);

    return failed ? E_FAIL : S_OK;
}

//------------------------------------------------------------------------------
//	Segment8_SetRepeats_Test2_Valid
//------------------------------------------------------------------------------
HRESULT 
Segment8_SetRepeats_Test2_Valid(
								IN CtIDirectMusicPerformance8*	ptPerf8, 
								IN DWORD						dwUnused1,
								IN DWORD						dwUnused2
								)
/*++

Routine Description:

	Valid test for IDirectMusicSegment8::SetRepeats

Arguments:

	IN ptPerf8 -	Performance object 
	IN dwUnused1 -	Unused
	IN dwUnused2 -	Unused

Return Value:

	S_OK on success, any other value on failure

--*/
{
    CICMusic					Music;
    HRESULT						hr					= S_OK;
	BOOL						failed				= FALSE;
    CtIDirectMusicSegment8*		ptSegment8			= NULL;
    CtIDirectMusicAudioPath*	ptPath				= NULL;
    DWORD						dwRepeatValues[]	= {0, 1, 2, 5, DMUS_SEG_REPEAT_INFINITE };
	DWORD						numValues			= sizeof(dwRepeatValues) / sizeof(DWORD);
    DWORD						i;

    Log(FYILOGLEVEL, "Calling Valid test function Segment8_SetRepeats_Test2_Valid())");

    MEDIAFILEDESC MediaFileDesc[] = {
                                {"DMusic/DMTest1/Segment8/SetRep/Repeat2.sgt",       "T:\\DMTest1\\Segment8\\SetRep\\",    COPY_IF_NEWER},
                                {"WAV/GuitarStream1.wav",            "T:\\DMTest1\\Segment8\\SetRep\\",    COPY_IF_NEWER},
                                {TERMINATE}
                                };
    CHECKRUN(LoadMedia(MediaFileDesc));

	
    CHECKRUN(Music.Init(ptPerf8, "T:\\DMTest1\\Segment8\\SetRep\\Repeat2.sgt",
						DMUS_APATH_SHARED_STEREOPLUSREVERB));
    CHECKRUN(Music.GetInterface(&ptSegment8));
    CHECKRUN(Music.GetInterface(&ptPath));
	if(FAILED(hr))
	{
		SAFE_RELEASE(ptSegment8);
		return E_FAIL;
	}
	
    //Try all these repeat values.
    for(i=0; i<numValues; ++i)
    {
		// Set Repeats
		Log(FYILOGLEVEL, "Calling SetRepeats(%u)",dwRepeatValues[i]);
		CHECKRUN(ptSegment8->SetRepeats(dwRepeatValues[i]));

		// Play
		CHECKRUN(ptPerf8->PlaySegmentEx(ptSegment8, 0, NULL, 0, __int64(0), NULL, NULL, ptPath));
		ExpectSegmentStart(ptPerf8, ptSegment8, NULL, 5000, NULL);

        //Wait until done
        if(dwRepeatValues[i] != DMUS_SEG_REPEAT_INFINITE)
        {
            Log(FYILOGLEVEL, "Waiting until segment stops");
			ExpectSegmentStop(ptPerf8, ptSegment8, NULL, 0xffffffff, NULL, TRUE);
			dmthPromptUserTestResults(hr, EXPECT_YES, "Did the audio loop %u time(s)?", dwRepeatValues[i] != 0 ? dwRepeatValues[i] : 1);
        }
        else
        {
			dmthPromptUserTestResults(hr, EXPECT_YES, "Is the audio looping forever?");
	        CHECKRUN(ptPerf8->StopEx(0, __int64(0), 0));
        }
	}

    SAFE_RELEASE(ptSegment8);
    SAFE_RELEASE(ptPath);

    return failed ? E_FAIL : S_OK;
}

//------------------------------------------------------------------------------
//	Segment8_SetRepeats_Test3_Valid
//------------------------------------------------------------------------------
HRESULT 
Segment8_SetRepeats_Test3_Valid(
								IN CtIDirectMusicPerformance8*	ptPerf8, 
								IN DWORD						dwUnused1,
								IN DWORD						dwUnused2
								)
/*++

Routine Description:

	Valid test for IDirectMusicSegment8::SetRepeats

Arguments:

	IN ptPerf8 -	Performance object 
	IN dwUnused1 -	Unused
	IN dwUnused2 -	Unused

Return Value:

	S_OK on success, any other value on failure

--*/
{
    CICMusic					Music;
    HRESULT						hr					= S_OK;
	BOOL						failed				= FALSE;
    CtIDirectMusicSegment8*		ptSegment8			= NULL;
    CtIDirectMusicAudioPath*	ptPath				= NULL;
    DWORD						dwRepeatValues[]	= {0, 1, 2, 5, DMUS_SEG_REPEAT_INFINITE };
	DWORD						numValues			= sizeof(dwRepeatValues) / sizeof(DWORD);
    DWORD						i;

    Log(FYILOGLEVEL, "Calling Valid test function Segment8_SetRepeats_Test3_Valid())");

    MEDIAFILEDESC MediaFileDesc[] = {
                                {"DMusic/DMTest1/Segment8/SetRep/Repeat3.sgt",     "T:\\DMTest1\\Segment8\\SetRep\\",    COPY_IF_NEWER},
                                {"WAV/GuitarClip1.wav",            "T:\\DMTest1\\Segment8\\SetRep\\",    COPY_IF_NEWER},
                                {TERMINATE}
                                };
    CHECKRUN(LoadMedia(MediaFileDesc));

	
    CHECKRUN(Music.Init(ptPerf8, "T:\\DMTest1\\Segment8\\SetRep\\Repeat3.sgt",
						DMUS_APATH_SHARED_STEREOPLUSREVERB));
    CHECKRUN(Music.GetInterface(&ptSegment8));
    CHECKRUN(Music.GetInterface(&ptPath));
	if(FAILED(hr))
	{
		SAFE_RELEASE(ptSegment8);
		return E_FAIL;
	}
	
    //Try all these repeat values.
    for(i=0; i<numValues; ++i)
    {
		// Set Repeats
		Log(FYILOGLEVEL, "Calling SetRepeats(%u)",dwRepeatValues[i]);
		CHECKRUN(ptSegment8->SetRepeats(dwRepeatValues[i]));

		// Play
		CHECKRUN(ptPerf8->PlaySegmentEx(ptSegment8, 0, NULL, 0, __int64(0), NULL, NULL, ptPath));
		ExpectSegmentStart(ptPerf8, ptSegment8, NULL, 5000, NULL);

        //Wait until done
        if(dwRepeatValues[i] != DMUS_SEG_REPEAT_INFINITE)
        {
            Log(FYILOGLEVEL, "Waiting until segment stops");
			ExpectSegmentStop(ptPerf8, ptSegment8, NULL, 0xffffffff, NULL, TRUE);
			dmthPromptUserTestResults(hr, EXPECT_YES, "Did the audio loop %u time(s)?", dwRepeatValues[i] != 0 ? dwRepeatValues[i] : 1);
        }
        else
        {
			dmthPromptUserTestResults(hr, EXPECT_YES, "Is the audio looping forever?");
	        CHECKRUN(ptPerf8->StopEx(0, __int64(0), 0));
        }
	}

    SAFE_RELEASE(ptSegment8);
    SAFE_RELEASE(ptPath);

    return failed ? E_FAIL : S_OK;
}

//------------------------------------------------------------------------------
//	Segment8_SetRepeats_Test4_Valid
//------------------------------------------------------------------------------
HRESULT 
Segment8_SetRepeats_Test4_Valid(
								IN CtIDirectMusicPerformance8*	ptPerf8, 
								IN DWORD						dwUnused1,
								IN DWORD						dwUnused2
								)
/*++

Routine Description:

	Valid test for IDirectMusicSegment8::SetRepeats

Arguments:

	IN ptPerf8 -	Performance object 
	IN dwUnused1 -	Unused
	IN dwUnused2 -	Unused

Return Value:

	S_OK on success, any other value on failure

--*/
{
    CICMusic					Music;
    HRESULT						hr					= S_OK;
	BOOL						failed				= FALSE;
    CtIDirectMusicSegment8*		ptSegment8			= NULL;
    CtIDirectMusicAudioPath*	ptPath				= NULL;
    DWORD						dwRepeatValues[]	= {0, 1, 2, 5, DMUS_SEG_REPEAT_INFINITE };
	DWORD						numValues			= sizeof(dwRepeatValues) / sizeof(DWORD);
    DWORD						i;

    Log(FYILOGLEVEL, "Calling Valid test function Segment8_SetRepeats_Test4_Valid())");

    MEDIAFILEDESC MediaFileDesc[] = {
                                {"DMusic/DMTest1/Segment8/SetRep/Repeat4.sgt",     "T:\\DMTest1\\Segment8\\SetRep\\",    COPY_IF_NEWER},
                                {"WAV/GuitarClip1.wav",            "T:\\DMTest1\\Segment8\\SetRep\\",    COPY_IF_NEWER},
                                {TERMINATE}
                                };
    CHECKRUN(LoadMedia(MediaFileDesc));

	
    CHECKRUN(Music.Init(ptPerf8, "T:\\DMTest1\\Segment8\\SetRep\\Repeat4.sgt",
						DMUS_APATH_SHARED_STEREOPLUSREVERB));
    CHECKRUN(Music.GetInterface(&ptSegment8));
    CHECKRUN(Music.GetInterface(&ptPath));
	if(FAILED(hr))
	{
		SAFE_RELEASE(ptSegment8);
		return E_FAIL;
	}
	
    //Try all these repeat values.
    for(i=0; i<numValues; ++i)
    {
		// Set Repeats
		Log(FYILOGLEVEL, "Calling SetRepeats(%u)",dwRepeatValues[i]);
		CHECKRUN(ptSegment8->SetRepeats(dwRepeatValues[i]));

		// Play
		CHECKRUN(ptPerf8->PlaySegmentEx(ptSegment8, 0, NULL, 0, __int64(0), NULL, NULL, ptPath));
		ExpectSegmentStart(ptPerf8, ptSegment8, NULL, 5000, NULL);

        //Wait until done
        if(dwRepeatValues[i] != DMUS_SEG_REPEAT_INFINITE)
        {
            Log(FYILOGLEVEL, "Waiting until segment stops");
			ExpectSegmentStop(ptPerf8, ptSegment8, NULL, 0xffffffff, NULL, TRUE);
			dmthPromptUserTestResults(hr, EXPECT_YES, "Did the audio loop %u time(s)?", dwRepeatValues[i] != 0 ? dwRepeatValues[i] : 1);
        }
        else
        {
			dmthPromptUserTestResults(hr, EXPECT_YES, "Is the audio looping forever?");
	        CHECKRUN(ptPerf8->StopEx(0, __int64(0), 0));
        }
	}

    SAFE_RELEASE(ptSegment8);
    SAFE_RELEASE(ptPath);

    return failed ? E_FAIL : S_OK;
}

//------------------------------------------------------------------------------
//	Segment8_SetRepeats_Test5_Valid
//------------------------------------------------------------------------------
HRESULT 
Segment8_SetRepeats_Test5_Valid(
								IN CtIDirectMusicPerformance8*	ptPerf8, 
								IN DWORD						dwUnused1,
								IN DWORD						dwUnused2
								)
/*++

Routine Description:

	Valid test for IDirectMusicSegment8::SetRepeats

Arguments:

	IN ptPerf8 -	Performance object 
	IN dwUnused1 -	Unused
	IN dwUnused2 -	Unused

Return Value:

	S_OK on success, any other value on failure

--*/
{
    CICMusic					Music;
    HRESULT						hr					= S_OK;
	BOOL						failed				= FALSE;
    CtIDirectMusicSegment8*		ptSegment8			= NULL;
    CtIDirectMusicAudioPath*	ptPath				= NULL;
    DWORD						dwRepeatValues[]	= {0, 1, 2, 5, DMUS_SEG_REPEAT_INFINITE };
	DWORD						numValues			= sizeof(dwRepeatValues) / sizeof(DWORD);
    DWORD						i;

    Log(FYILOGLEVEL, "Calling Valid test function Segment8_SetRepeats_Test5_Valid())");

    MEDIAFILEDESC MediaFileDesc[] = {
                                {"DMusic/DMTest1/Segment8/SetRep/Repeat5.sgt",     "T:\\DMTest1\\Segment8\\SetRep\\",    COPY_IF_NEWER},
                                {"DMusic/DMTest1/Segment8/SetRep/Demo.DLS",        "T:\\DMTest1\\Segment8\\SetRep\\",    COPY_IF_NEWER},
                                {TERMINATE}
                                };
    CHECKRUN(LoadMedia(MediaFileDesc));
	
    CHECKRUN(Music.Init(ptPerf8, "T:\\DMTest1\\Segment8\\SetRep\\Repeat5.sgt",
						DMUS_APATH_SHARED_STEREOPLUSREVERB));
    CHECKRUN(Music.GetInterface(&ptSegment8));
    CHECKRUN(Music.GetInterface(&ptPath));
	if(FAILED(hr))
	{
		SAFE_RELEASE(ptSegment8);
		return E_FAIL;
	}
	
    //Try all these repeat values.
    for(i=0; i<numValues; ++i)
    {
		// Set Repeats
		Log(FYILOGLEVEL, "Calling SetRepeats(%u)",dwRepeatValues[i]);
		CHECKRUN(ptSegment8->SetRepeats(dwRepeatValues[i]));

		// Play
		CHECKRUN(ptPerf8->PlaySegmentEx(ptSegment8, 0, NULL, 0, __int64(0), NULL, NULL, ptPath));
		ExpectSegmentStart(ptPerf8, ptSegment8, NULL, 5000, NULL);

        //Wait until done
        if(dwRepeatValues[i] != DMUS_SEG_REPEAT_INFINITE)
        {
            Log(FYILOGLEVEL, "Waiting until segment stops");
			ExpectSegmentStop(ptPerf8, ptSegment8, NULL, 0xffffffff, NULL, TRUE);
			dmthPromptUserTestResults(hr, EXPECT_YES, "Did the audio loop %u time(s)?", dwRepeatValues[i] != 0 ? dwRepeatValues[i] : 1);
        }
        else
        {
			dmthPromptUserTestResults(hr, EXPECT_YES, "Is the audio looping forever?");
	        CHECKRUN(ptPerf8->StopEx(0, __int64(0), 0));
        }
	}

    SAFE_RELEASE(ptSegment8);
    SAFE_RELEASE(ptPath);

    return failed ? E_FAIL : S_OK;
}

//------------------------------------------------------------------------------
//	Segment8_SetRepeats_Test6_Valid
//------------------------------------------------------------------------------
HRESULT 
Segment8_SetRepeats_Test6_Valid(
								IN CtIDirectMusicPerformance8*	ptPerf8, 
								IN DWORD						dwUnused1,
								IN DWORD						dwUnused2
								)
/*++

Routine Description:

	Valid test for IDirectMusicSegment8::SetRepeats

Arguments:

	IN ptPerf8 -	Performance object 
	IN dwUnused1 -	Unused
	IN dwUnused2 -	Unused

Return Value:

	S_OK on success, any other value on failure

--*/
{
    CICMusic					Music;
    HRESULT						hr					= S_OK;
	BOOL						failed				= FALSE;
    CtIDirectMusicSegment8*		ptSegment8			= NULL;
    CtIDirectMusicAudioPath*	ptPath				= NULL;
    DWORD						dwRepeatValues[]	= {0, 1, 2, 5, DMUS_SEG_REPEAT_INFINITE };
	DWORD						numValues			= sizeof(dwRepeatValues) / sizeof(DWORD);
    DWORD						i;

    Log(FYILOGLEVEL, "Calling Valid test function Segment8_SetRepeats_Test6_Valid())");

    MEDIAFILEDESC MediaFileDesc[] = {
                                {"DMusic/DMTest1/Segment8/SetRep/ShortMIDIAndWave.sgt",     "T:\\DMTest1\\Segment8\\SetRep\\",    COPY_IF_NEWER},
                                {"DMusic/DMTest1/Segment8/SetRep/ShortMIDIAndWave.wav",     "T:\\DMTest1\\Segment8\\SetRep\\",    COPY_IF_NEWER},
                                {"DLS/SonicImplants Live Band.dls",         "T:\\DMTest1\\Segment8\\SetRep\\",    COPY_IF_NEWER},
                                {TERMINATE}
                                };

    CHECKRUN(LoadMedia(MediaFileDesc));	
    CHECKRUN(Music.Init(ptPerf8, "T:\\DMTest1\\Segment8\\SetRep\\ShortMIDIandWave.sgt",
						DMUS_APATH_SHARED_STEREOPLUSREVERB));
    CHECKRUN(Music.GetInterface(&ptSegment8));
    CHECKRUN(Music.GetInterface(&ptPath));
	if(FAILED(hr))
	{
		SAFE_RELEASE(ptSegment8);
		return E_FAIL;
	}

    //Try all these repeat values.
    for(i=0; i<numValues; ++i)
    {
		// Set Repeats
		Log(FYILOGLEVEL, "Calling SetRepeats(%u)",dwRepeatValues[i]);
		CHECKRUN(ptSegment8->SetRepeats(dwRepeatValues[i]));

		// Play
		CHECKRUN(ptPerf8->PlaySegmentEx(ptSegment8, 0, NULL, 0, __int64(0), NULL, NULL, ptPath));
		ExpectSegmentStart(ptPerf8, ptSegment8, NULL, 5000, NULL);

        //Wait until done
        if(dwRepeatValues[i] != DMUS_SEG_REPEAT_INFINITE)
        {
            Log(FYILOGLEVEL, "Waiting until segment stops");
			ExpectSegmentStop(ptPerf8, ptSegment8, NULL, 0xffffffff, NULL, TRUE);
			dmthPromptUserTestResults(hr, EXPECT_YES, "Did the audio loop %u time(s)?", dwRepeatValues[i] != 0 ? dwRepeatValues[i] : 1);
        }
        else
        {
			dmthPromptUserTestResults(hr, EXPECT_YES, "Is the audio looping forever?");
	        CHECKRUN(ptPerf8->StopEx(0, __int64(0), 0));
        }
	}

    SAFE_RELEASE(ptSegment8);
    SAFE_RELEASE(ptPath);

    return failed ? E_FAIL : S_OK;
}

//------------------------------------------------------------------------------
//	Segment8_SetRepeats_BVT1
//------------------------------------------------------------------------------
HRESULT 
Segment8_SetRepeats_BVT1(
						 IN CtIDirectMusicPerformance8* ptPerf8
						 )
/*++

Routine Description:

	BVT test for IDirectMusicSegment8::SetRepeats

Arguments:

	IN ptPerf8 -	Performance object

Return Value:

	S_OK on success, any other value on failure

--*/
{
    CICMusic					Music;	
    HRESULT						hr					= S_OK;
    CtIDirectMusicSegment8*		ptSegment8			= NULL;
    CtIDirectMusicAudioPath*	ptPath				= NULL;
    DWORD						dwRepeatValues[]	= {0, 1, 2, 5, DMUS_SEG_REPEAT_INFINITE };
    DWORD						i					= 0;
    LPSTR                       szSegmentServer     = "DMusic/DMTest1/Segment8/SetRep/ShortSeg.sgt";
    CHAR                        szDirectory[MAX_PATH] = {0};

    //Copy down the entire directory of files containing ShortSeg.sgt
    CHECKRUN(MediaCopyDirectory(szSegmentServer));
    
    //Get the this stuff was copied too.
    CHECKRUN(ChopPath(MediaServerToLocal(szSegmentServer), szDirectory, NULL));

    //Copy several other needed files into the directory as well.
    MEDIAFILEDESC MediaFileDesc[]=
    {
    {"DLS/Main1.DLS",                     szDirectory,    COPY_IF_NEWER},
    {"DLS/SonicImplants Live Band.DLS",   szDirectory,    COPY_IF_NEWER},
    {"WAV/GuitarClip1.wav",               szDirectory,    COPY_IF_NEWER},
    {"WAV/GuitarStream1.wav",             szDirectory,    COPY_IF_NEWER},
    {TERMINATE}
    };
    CHECKRUN(LoadMedia(MediaFileDesc));



    //Load up and play our segment.  It's really short so we can count the number of times it plays.
    CHECKRUN(Music.Init(ptPerf8, MediaServerToLocal(szSegmentServer), DMUS_APATH_SHARED_STEREOPLUSREVERB));
    CHECKRUN(Music.GetInterface(&ptSegment8));
    CHECKRUN(Music.GetInterface(&ptPath));

    //Try all these repeat values.
    for (i=0; i<AMOUNT(dwRepeatValues); i++)
    {

        //Stop everything and wait a sec.
        CHECKRUN(ptPerf8->StopEx(0, __int64(0), 0));
        Log(FYILOGLEVEL, "Playing and stopping immediately.  Should hear both WAV track and MIDI notes invalidated simultaneously");
        Wait(1000);


        //Set Repeats
        Log(FYILOGLEVEL, "Calling SetRepeats(%u)",dwRepeatValues[i]);
        CHECKRUN(ptSegment8->SetRepeats(dwRepeatValues[i]));
         //Play
         CHECKRUN(ptPerf8->PlaySegmentEx( 
		            ptSegment8, 
		            0,
		            NULL,
                    0, 
		            __int64(0),
		            NULL,
		            NULL,
		            ptPath));

        //Wait for segment to start.
        CHECKRUN(ExpectSegmentStart(ptPerf8, ptSegment8, NULL, 5000, NULL, TRUE));

        //Wait until done
        if (dwRepeatValues[i] != DMUS_SEG_REPEAT_INFINITE)
        {
            Log(FYILOGLEVEL, "Waiting until segment stops");
            CHECKRUN(ExpectSegmentStop(ptPerf8, ptSegment8, NULL, 20000, NULL, TRUE));
        }
        else
        {
            Log(FYILOGLEVEL, "Waiting 5 seconds.");
            Wait(5000);
        }
    }

    SAFE_RELEASE(ptSegment8);
    SAFE_RELEASE(ptPath);
    return hr;

}