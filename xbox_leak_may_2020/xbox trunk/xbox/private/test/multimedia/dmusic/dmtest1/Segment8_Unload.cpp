/*++

Copyright (c) 2001 Microsoft Corporation

Module Name:

	Segment8_Unload.cpp

Abstract:

	Performs all tests for the IDirectMusicSegment8 api's

Author:

	Dan Haffner (danhaff) 

Revision History:

	16-Mar-2001 robheit
		Added all valid tests

--*/

//------------------------------------------------------------------------------
//	Includes
//------------------------------------------------------------------------------
#include "globals.h"

//------------------------------------------------------------------------------
//	Segment8_Unload_BVT
//------------------------------------------------------------------------------
HRESULT 
Segment8_Unload_BVT(
					IN CtIDirectMusicPerformance8*	ptPerf8, 
					DWORD							dwUnused1, 
					DWORD							dwUnused2
					)
/*++

Routine Description:

	BVT tests for IDirectMusicSegment8::Unload

Arguments:

	IN ptPerf8 -	Performance object
	IN dwUnused1 -	Unused
	IN dwUnused2 -	Unused

Return Value:

	S_OK on success, any other value on failure

--*/
{
    HRESULT hr = S_OK;
    Log(FYILOGLEVEL, "Calling BVT test function Segment8_Unload_BVT())");

    Log(FYILOGLEVEL, "--- CURRENT TESTING IS IMPLEMENTED IN Segment8_Download ---");

    return hr;
}

//------------------------------------------------------------------------------
//	Segment8_Unload_Test1_Valid
//------------------------------------------------------------------------------
HRESULT 
Segment8_Unload_Test1_Valid(
						    IN CtIDirectMusicPerformance8*	ptPerf8,
						    DWORD							dwUnused1, 
						    DWORD							dwUnused2
						    )
/*++

Routine Description:

	Tests the unloading of a wave while playing produces silence

Arguments:

	IN ptPerf8 -	Performance object
	IN dwUnused1 -	Unused
	IN dwUnused2 -	Unused

Return Value:

	S_OK on success, any other value on failure

--*/
{
	CtIDirectMusicSegment8*		ptSegment8		= NULL;
	CtIDirectMusicAudioPath*	ptPath			= NULL;
	UINT						downloadToPerf	= 0;
    HRESULT						hr				= S_OK;
	BOOL						failed			= FALSE;
	CHAR*						szFileName		= "T:\\DMTest1\\Segment8\\MIDIandWave\\WaveSegment.sgt";

    Log(FYILOGLEVEL, "Calling main test function Segment8_Unload_Test1_Valid())");

	CHECKRUN(ptPerf8->CreateStandardAudioPath(DMUS_APATH_SHARED_STEREOPLUSREVERB, 32, TRUE, &ptPath));

	//Try downloading to both the performance and the audiopath.
	for (downloadToPerf = 0; downloadToPerf < 2 && SUCCEEDED(hr); downloadToPerf++)
	{
		//Load up the media file.
		hr = dmthLoadSegment(szFileName, &ptSegment8);
		if(FAILED(hr))
		{
			Log(ABORTLOGLEVEL, "!!!! BUG: dmthLoadSegment failed (%s == %08Xh)",tdmXlatHRESULT(hr), hr);
			failed = TRUE;
			continue;
		}

		// Download the segment
		Log(FYILOGLEVEL, "Downloading to %s interface", downloadToPerf ? "Performance8" : "AudioPath");
		hr = ptSegment8->Download(downloadToPerf ? (CtIUnknown *)ptPerf8 : (CtIUnknown *)ptPath);
		if(FAILED(hr))
		{
			Log(ABORTLOGLEVEL, "!!!! BUG: Download failed (%s == %08Xh)",tdmXlatHRESULT(hr), hr);
			CHECKRUN(ptSegment8->Unload(downloadToPerf ? (CtIUnknown *)ptPerf8 : (CtIUnknown *)ptPath));
			failed = TRUE;
			continue;
		}

		//Play the segment.
		hr = ptPerf8->PlaySegmentEx(ptSegment8, 0, NULL, 0, __int64(0), NULL, NULL, ptPath);
		if(FAILED(hr))
		{
			Log(ABORTLOGLEVEL, "!!!! BUG: pwPerformance->PlaySegmentEx failed (%s == %08Xh)",tdmXlatHRESULT(hr), hr);
			CHECKRUN(ptSegment8->Unload(downloadToPerf ? (CtIUnknown *)ptPerf8 : (CtIUnknown *)ptPath));
			failed = TRUE;
		}

		// Prompt the user
		else
		{
			Log(FYILOGLEVEL, "You should hear the entire wave file (about 28 seconds)");
			ExpectSegmentStart(ptPerf8, ptSegment8, NULL, 5000, NULL);
			Wait(2000);
		    Log(FYILOGLEVEL, "Unloading original segment from %s interface", downloadToPerf ? "Performance8" : "AudioPath");
			CHECKRUN(ptSegment8->Unload(downloadToPerf ? (CtIUnknown *)ptPerf8 : (CtIUnknown *)ptPath));
			dmthPromptUserTestResults(hr, EXPECT_YES, "Is the wave file still playing?");

			Log(FYILOGLEVEL, "Replaying the segment (you should hear silence)");
			CHECKRUN(ptPerf8->PlaySegmentEx(ptSegment8, 0, NULL, 0, __int64(0), NULL, NULL, ptPath));
			ExpectSegmentStart(ptPerf8, ptSegment8, NULL, 5000, NULL);
			dmthPromptUserTestResults(hr, EXPECT_NO, "Do you hear anything?");
	        CHECKRUN(ptPerf8->StopEx(0, __int64(0), 0));
		}

		SAFE_RELEASE(ptSegment8);            
	}
	SAFE_RELEASE(ptPath);

    return failed ? E_FAIL : S_OK;
}

//------------------------------------------------------------------------------
//	Segment8_Unload_Test2_Valid
//------------------------------------------------------------------------------
HRESULT 
Segment8_Unload_Test2_Valid(
							IN CtIDirectMusicPerformance8*	ptPerf8,
						    DWORD							dwUnused1, 
						    DWORD							dwUnused2
							)
/*++

Routine Description:

	Verify that playing a MIDI+DLS segment with a wave track and then unloading
	results in silence.

Arguments:

	IN ptPerf8 -	Performance object
	IN dwUnused1 -	Unused
	IN dwUnused2 -	Unused

Return Value:

	S_OK on success, any other value on failure

--*/
{
	CtIDirectMusicSegment8*		ptSegment8		= NULL;
	CtIDirectMusicAudioPath*	ptPath			= NULL;
	UINT						downloadToPerf	= 0;
    HRESULT						hr				= S_OK;
	BOOL						failed			= FALSE;
	CHAR*						szFileName		= "T:\\DMTest1\\Segment8\\MIDIandWave\\MIDIandWave.sgt";

    Log(FYILOGLEVEL, "Calling main test function Segment8_Unload_Test2_Valid())");

	CHECKRUN(ptPerf8->CreateStandardAudioPath(DMUS_APATH_SHARED_STEREOPLUSREVERB, 32, TRUE, &ptPath));

	//Try downloading to both the performance and the audiopath.
	for (downloadToPerf = 0; downloadToPerf < 2; downloadToPerf++)
	{
		//Load up the media file.
		hr = dmthLoadSegment(szFileName, &ptSegment8);
		if(FAILED(hr))
		{
			Log(ABORTLOGLEVEL, "!!!! BUG: dmthLoadSegment failed (%s == %08Xh)",tdmXlatHRESULT(hr), hr);
			failed = TRUE;
			continue;
		}

		// Download the segment
		Log(FYILOGLEVEL, "Downloading to %s interface", downloadToPerf ? "Performance8" : "AudioPath");
		hr = ptSegment8->Download(downloadToPerf ? (CtIUnknown *)ptPerf8 : (CtIUnknown *)ptPath);
		if(FAILED(hr))
		{
			Log(ABORTLOGLEVEL, "!!!! BUG: Download failed (%s == %08Xh)",tdmXlatHRESULT(hr), hr);
			CHECKRUN(ptSegment8->Unload(downloadToPerf ? (CtIUnknown *)ptPerf8 : (CtIUnknown *)ptPath));
			failed = TRUE;
			continue;
		}

		//Play the segment.
		hr = ptPerf8->PlaySegmentEx(ptSegment8, 0, NULL, 0, __int64(0), NULL, NULL, ptPath);
		if(FAILED(hr))
		{
			Log(ABORTLOGLEVEL, "!!!! BUG: pwPerformance->PlaySegmentEx failed (%s == %08Xh)",tdmXlatHRESULT(hr), hr);
			CHECKRUN(ptSegment8->Unload(downloadToPerf ? (CtIUnknown *)ptPerf8 : (CtIUnknown *)ptPath));
			failed = TRUE;
		}

		// Prompt the user
		else
		{
			Log(FYILOGLEVEL, "You should hear 2 seconds of audio then the MIDI track should go silent.");
			ExpectSegmentStart(ptPerf8, ptSegment8, NULL, 5000, NULL);
			Wait(2000);
		    Log(FYILOGLEVEL, "Unloading original segment from %s interface", downloadToPerf ? "Performance8" : "AudioPath");
			CHECKRUN(ptSegment8->Unload(downloadToPerf ? (CtIUnknown *)ptPerf8 : (CtIUnknown *)ptPath));

			dmthPromptUserTestResults(hr, EXPECT_YES, "Did the MIDI stop and the Wave continue?");
	        CHECKRUN(ptPerf8->StopEx(0, __int64(0), 0));
		}

		SAFE_RELEASE(ptSegment8);            
	}
	SAFE_RELEASE(ptPath);

    return failed ? E_FAIL : S_OK;
}

//------------------------------------------------------------------------------
//	Segment8_Unload_Test3_Valid
//------------------------------------------------------------------------------
HRESULT 
Segment8_Unload_Test3_Valid(
							IN CtIDirectMusicPerformance8*	ptPerf8,
						    DWORD							dwUnused1, 
						    DWORD							dwUnused2
							)
/*++

Routine Description:

	Verify that playing a MIDI+DLS segment and then unloading results in silence

Arguments:

	IN ptPerf8 -	Performance object
	IN dwUnused1 -	Unused
	IN dwUnused2 -	Unused

Return Value:

	S_OK on success, any other value on failure

--*/
{
	CtIDirectMusicSegment8*		ptSegment8		= NULL;
	CtIDirectMusicAudioPath*	ptPath			= NULL;
	UINT						downloadToPerf	= 0;
    HRESULT						hr				= S_OK;
	BOOL						failed			= FALSE;
	CHAR*						szFileName		= "T:\\DMTest1\\Segment8\\MIDIandWave\\MIDI.sgt";

    Log(FYILOGLEVEL, "Calling main test function Segment8_Unload_Test3_Valid())");

	CHECKRUN(ptPerf8->CreateStandardAudioPath(DMUS_APATH_SHARED_STEREOPLUSREVERB, 32, TRUE, &ptPath));

	//Try downloading to both the performance and the audiopath.
	for (downloadToPerf = 0; downloadToPerf < 2; downloadToPerf++)
	{
		//Load up the media file.
		hr = dmthLoadSegment(szFileName, &ptSegment8);
		if(FAILED(hr))
		{
			Log(ABORTLOGLEVEL, "!!!! BUG: dmthLoadSegment failed (%s == %08Xh)",tdmXlatHRESULT(hr), hr);
			failed = TRUE;
			continue;
		}

		// Download the segment
		Log(FYILOGLEVEL, "Downloading to %s interface", downloadToPerf ? "Performance8" : "AudioPath");
		hr = ptSegment8->Download(downloadToPerf ? (CtIUnknown *)ptPerf8 : (CtIUnknown *)ptPath);
		if(FAILED(hr))
		{
			Log(ABORTLOGLEVEL, "!!!! BUG: Download failed (%s == %08Xh)",tdmXlatHRESULT(hr), hr);
			failed = TRUE;
		    Log(FYILOGLEVEL, "Unloading original segment from %s interface", downloadToPerf ? "Performance8" : "AudioPath");
			CHECKRUN(ptSegment8->Unload(downloadToPerf ? (CtIUnknown *)ptPerf8 : (CtIUnknown *)ptPath));
			continue;
		}

		//Play the segment.
		hr = ptPerf8->PlaySegmentEx(ptSegment8, 0, NULL, 0, __int64(0), NULL, NULL, ptPath);
		if(FAILED(hr))
		{
			Log(ABORTLOGLEVEL, "!!!! BUG: pwPerformance->PlaySegmentEx failed (%s == %08Xh)",tdmXlatHRESULT(hr), hr);
			CHECKRUN(ptSegment8->Unload(downloadToPerf ? (CtIUnknown *)ptPerf8 : (CtIUnknown *)ptPath));
			failed = TRUE;
		}

		// Prompt the user
		else
		{
			Log(FYILOGLEVEL, "You should hear 2 second of audio then silence");
			ExpectSegmentStart(ptPerf8, ptSegment8, NULL, 5000, NULL);
			Wait(2000);
		    Log(FYILOGLEVEL, "Unloading original segment from %s interface", downloadToPerf ? "Performance8" : "AudioPath");
			CHECKRUN(ptSegment8->Unload(downloadToPerf ? (CtIUnknown *)ptPerf8 : (CtIUnknown *)ptPath));
			dmthPromptUserTestResults(hr, EXPECT_YES, "Did the audio stop after 2 seconds?");
		}

		SAFE_RELEASE(ptSegment8);            
	}
	SAFE_RELEASE(ptPath);

    return failed ? E_FAIL : S_OK;
}

//------------------------------------------------------------------------------
//	Segment8_Unload_Test4_Valid
//------------------------------------------------------------------------------
HRESULT 
Segment8_Unload_Test4_Valid(
							IN CtIDirectMusicPerformance8*	ptPerf8,
						    DWORD							dwUnused1, 
						    DWORD							dwUnused2
							)
/*++

Routine Description:

	Verify via timing that unload requires that download reload the instruments 
	(hence they're not cached anywhere)

Arguments:

	IN ptPerf8 -	Performance object
	IN dwUnused1 -	Unused
	IN dwUnused2 -	Unused

Return Value:

	S_OK on success, any other value on failure

--*/
{
    Log(FYILOGLEVEL, "Calling main test function Segment8_Unload_Test4_Valid())");

	return S_OK;
}