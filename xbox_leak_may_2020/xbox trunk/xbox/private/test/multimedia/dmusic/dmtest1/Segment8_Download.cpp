/*++

Copyright (c) 2001 Microsoft Corporation

Module Name:

	Segment8_Download.cpp

Abstract:

	Tests for IDirectMusicSegment8::Download

Author:

	Dan Haffner (danhaff) 

Revision History:

	14-Mar-2001 robheit
		Added additional valid tests

--*/

//------------------------------------------------------------------------------
//	Includes:
//------------------------------------------------------------------------------
#include "globals.h"

//------------------------------------------------------------------------------
//	Function Declarations:
//------------------------------------------------------------------------------
HRESULT Segment8_DownloadUnload_BVT(CtIDirectMusicPerformance8* ptPerf8);
HRESULT Segment8_Download_PreCache_Multiple(CtIDirectMusicPerformance8* ptPerf8, LPSTR *szFileName);

//------------------------------------------------------------------------------
//	Segment8_Download_BVT 
//------------------------------------------------------------------------------
HRESULT 
Segment8_Download_BVT(
					  IN CtIDirectMusicPerformance8*	ptPerf8, 
					  DWORD								dwUnused1, 
					  DWORD								dwUnused2
					  )
/*++

Routine Description:

	Performs all BVT tests for IDirectMusicSegment8

Arguments:

	IN ptPerf8 -	Performance object
	IN dwUnused1 -	Unused
	IN dwUnused2 -	Unused

Return Value:

	S_OK on success, any other value on failure

--*/
{
    HRESULT hr = S_OK;
    Log(FYILOGLEVEL, "Calling main test function Segment8_Download_BVT())");
    DMTEST_EXECUTE(Segment8_DownloadUnload_BVT(ptPerf8));           //PASSES
    return hr;
};

//------------------------------------------------------------------------------
//	Segment8_Download_Test1_Valid
//------------------------------------------------------------------------------
HRESULT 
Segment8_Download_Test1_Valid(
							  IN CtIDirectMusicPerformance8*	ptPerf8, 
							  IN DWORD							dwUnused1, 
							  IN DWORD							dwUnused2
							  )
/*++

Routine Description:

	Verify that playing a wave segment without downloading results in silence.

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
	DWORD						dwSegment		= 0;
    HRESULT						hr				= S_OK;
	BOOL						failed			= FALSE;
	CHAR*						szFileName[2]	= {"T:\\DMTest1\\Segment8\\Download\\One.sgt",
												   "T:\\DMTest1\\Segment8\\Download\\Two.sgt"};

    Log(FYILOGLEVEL, "Calling main test function Segment8_Download_Test1_Valid())");

    CHECKRUN(ptPerf8->CreateStandardAudioPath(DMUS_APATH_SHARED_STEREOPLUSREVERB, 32, TRUE, &ptPath));
    
    //Try downloading to both the performance and the audiopath.
    for (downloadToPerf = 0; downloadToPerf < 2 && SUCCEEDED(hr); downloadToPerf++)
    {
        //Play both the segments.
        for (dwSegment=0; dwSegment<2 && SUCCEEDED(hr); dwSegment++)
        {
            //Load up the media file.
            hr = dmthLoadSegment(szFileName[dwSegment], &ptSegment8);
			if(FAILED(hr))
			{
				Log(ABORTLOGLEVEL, "!!!! BUG: dmthLoadSegment failed (%s == %08Xh)",tdmXlatHRESULT(hr), hr);
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
				Log(FYILOGLEVEL, "We did not download anything; you should hear silence.");
				Log(FYILOGLEVEL, "Playing segment; you should NOT hear the phrase ""%d"" being spoken", dwSegment+1);
				CHECKRUN(ExpectSegmentStart(ptPerf8, ptSegment8, NULL, 2000, NULL));
				CHECKRUN(ExpectSegmentStop(ptPerf8, ptSegment8, NULL, 15000, NULL));				
                dmthPromptUserTestResults(hr, EXPECT_NO, "Did you hear any audio?");
			}

	        Log(FYILOGLEVEL, "Unloading original segment from %s interface", downloadToPerf ? "Performance8" : "AudioPath");
			CHECKRUN(ptSegment8->Unload(downloadToPerf ? (CtIUnknown *)ptPerf8 : (CtIUnknown *)ptPath));
            SAFE_RELEASE(ptSegment8);            
        }
    };

    SAFE_RELEASE(ptPath);

    return failed ? E_FAIL : S_OK;
};

//------------------------------------------------------------------------------
//	Segment8_Download_Test2_Valid
//------------------------------------------------------------------------------
HRESULT 
Segment8_Download_Test2_Valid(
							  IN CtIDirectMusicPerformance8*	ptPerf8, 
							  IN DWORD							dwUnused1, 
							  IN DWORD							dwUnused2
							  )
/*++

Routine Description:

	Verify that playing a wave segment with 257 different waves works.

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
	CHAR*						szFileName		= "T:\\DMTest1\\Segment8\\257Waves\\257Waves.sgt";

    Log(FYILOGLEVEL, "Calling main test function Segment8_Download_Test2_Valid())");

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
		CHECKRUN(ptSegment8->Download(downloadToPerf ? (CtIUnknown *)ptPerf8 : (CtIUnknown *)ptPath);) 

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
			Log(FYILOGLEVEL, "Playing the segment, you should hear 257 notes");
			CHECKRUN(ExpectSegmentStart(ptPerf8, ptSegment8, NULL, 5000, NULL));
			CHECKRUN(ExpectSegmentStop(ptPerf8, ptSegment8, NULL, 0xffffffff, NULL));
			dmthPromptUserTestResults(hr, EXPECT_YES, "Did you hear 257 notes?");
		}

	    Log(FYILOGLEVEL, "Unloading original segment from %s interface", downloadToPerf ? "Performance8" : "AudioPath");
		CHECKRUN(ptSegment8->Unload(downloadToPerf ? (CtIUnknown *)ptPerf8 : (CtIUnknown *)ptPath));
		SAFE_RELEASE(ptSegment8);            
	}
	SAFE_RELEASE(ptPath);

    return failed ? E_FAIL : S_OK;
};

//------------------------------------------------------------------------------
//	Segment8_Download_Test3_Valid
//------------------------------------------------------------------------------
HRESULT 
Segment8_Download_Test3_Valid(
							  IN CtIDirectMusicPerformance8*	ptPerf8, 
							  IN DWORD							dwUnused1, 
							  IN DWORD							dwUnused2
							  )
/*++

Routine Description:

	Determines if downloading a band segment after another segment is loaded and
	playing replaces the band

Arguments:

	IN ptPerf8 -	Performance object
	IN dwUnused1 -	Unused
	IN dwUnused2 -	Unused

Return Value:

	S_OK on success, any other value on failure

--*/
{
	HRESULT						hr					= S_OK;
	CtIDirectMusicSegment8*		pOriginalSegment	= NULL;
	CtIDirectMusicSegment8*		pNewSegment			= NULL;
	CtIDirectMusicAudioPath*	ptPath				= NULL;
	UINT						downloadToPerf		= 0;
	BOOL						failed				= FALSE;
	CHAR*						original			= "T:\\DMTest1\\Segment8\\Download\\OriginalSegment.sgt";
	CHAR*						change				= "T:\\DMTest1\\Segment8\\Download\\NewBand.sgt";

    // Create a audiopath.
    CHECKRUN(ptPerf8->CreateStandardAudioPath(DMUS_APATH_SHARED_STEREOPLUSREVERB, 32, TRUE, &ptPath));
    
	// Try downloading to both the performance and the audiopath.
    for(downloadToPerf = 0; downloadToPerf < 2; downloadToPerf++)
    {
		// Load the original data
	    hr = dmthLoadSegment(original, &pOriginalSegment);
		if(FAILED(hr))
		{
			Log(ABORTLOGLEVEL, "!!!! BUG: dmthLoadSegment failed (%s == %08Xh)",tdmXlatHRESULT(hr), hr);
			failed = TRUE;
			continue;
		}

	    // Load the original data
	    hr = dmthLoadSegment(change, &pNewSegment);
		if(FAILED(hr))
		{
			Log(ABORTLOGLEVEL, "!!!! BUG: dmthLoadSegment failed (%s == %08Xh)",tdmXlatHRESULT(hr), hr);
			failed = TRUE;
			continue;
		}

		// Download everything.
		Log(FYILOGLEVEL, "Downloading original to %s interface", downloadToPerf ? "Performance8" : "AudioPath");
		CHECKRUN(pOriginalSegment->Download(downloadToPerf ? (CtIUnknown *)ptPerf8 : (CtIUnknown *)ptPath)); 

        // Play the segment.
	    hr = ptPerf8->PlaySegmentEx(pOriginalSegment, 0, NULL, 0, __int64(0), NULL, NULL, ptPath);
	    if(FAILED(hr))
	    {
		    Log(ABORTLOGLEVEL, "!!!! BUG: pwPerformance->PlaySegmentEx failed (%s == %08Xh)",tdmXlatHRESULT(hr), hr);
            CHECKRUN(pOriginalSegment->Unload(downloadToPerf ? (CtIUnknown *)ptPerf8 : (CtIUnknown *)ptPath));
			SAFE_RELEASE(pOriginalSegment);            
			SAFE_RELEASE(pNewSegment);            
			continue;
	    }
        Log(FYILOGLEVEL, "Playing segment with original band.");
		WaitForSegmentStart(ptPerf8, pOriginalSegment, NULL, 5000);

		// Download the new segment
		Log(FYILOGLEVEL, "Downloading new segment to %s interface", downloadToPerf ? "Performance8" : "AudioPath");
		CHECKRUN(pNewSegment->Download(downloadToPerf ? (CtIUnknown *)ptPerf8 : (CtIUnknown *)ptPath);) 

        Log(FYILOGLEVEL, "You should hear a different band playing the same segment.");
		Wait(3000);

		// Prompt for response
		dmthPromptUserTestResults(hr, EXPECT_YES, "Did the band change?");

        Log(FYILOGLEVEL, "Unloading original segment from %s interface", downloadToPerf ? "Performance8" : "AudioPath");
        CHECKRUN(pOriginalSegment->Unload(downloadToPerf ? (CtIUnknown *)ptPerf8 : (CtIUnknown *)ptPath));
        SAFE_RELEASE(pOriginalSegment);            

        Log(FYILOGLEVEL, "Unloading new segment from %s interface", downloadToPerf ? "Performance8" : "AudioPath");
        CHECKRUN(pNewSegment->Unload(downloadToPerf ? (CtIUnknown *)ptPerf8 : (CtIUnknown *)ptPath));
        SAFE_RELEASE(pNewSegment);            
    }

    SAFE_RELEASE(ptPath);
    return failed ? E_FAIL : S_OK;
}

//------------------------------------------------------------------------------
//	Segment8_Download_Test4_Valid
//------------------------------------------------------------------------------
HRESULT 
Segment8_Download_Test4_Valid(
							  IN CtIDirectMusicPerformance8*	ptPerf8, 
							  IN DWORD							dwUnused1, 
							  IN DWORD							dwUnused2
							  )
/*++

Routine Description:

	Verify that playing a MIDI+DLS segment with a wave track without downloading
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

    Log(FYILOGLEVEL, "Calling main test function Segment8_Download_Test4_Valid())");

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

		//Play the segment.
		hr = ptPerf8->PlaySegmentEx(ptSegment8, 0, NULL, 0, __int64(0), NULL, NULL, ptPath);
		if(FAILED(hr))
			failed = TRUE;

		// Prompt the user
		else
		{
			Log(FYILOGLEVEL, "Playing the segment, you should not hear anything");
			ExpectSegmentStart(ptPerf8, ptSegment8, NULL, 5000, NULL);
			ExpectSegmentStop(ptPerf8, ptSegment8, NULL, 0xffffffff, NULL);
			dmthPromptUserTestResults(hr, EXPECT_NO, "Did you hear anything?");
		}
		SAFE_RELEASE(ptSegment8);            
	}
	SAFE_RELEASE(ptPath);

    return failed ? E_FAIL : S_OK;
}

//------------------------------------------------------------------------------
//	Segment8_DownloadUnload_BVT
//------------------------------------------------------------------------------
HRESULT 
Segment8_DownloadUnload_BVT(
							IN CtIDirectMusicPerformance8* ptPerf8
							)
/*++

Routine Description:

	Performs a download and unload test for the BVT

Arguments:

	IN ptPerf8 -	Performance object

Return Value:

	S_OK on success, some other error code on failure

--*/
{
	BOOL						failed			= FALSE;
	HRESULT						hr				= S_OK;
	CtIDirectMusicSegment8*		ptSegment8		= NULL;
	CtIDirectMusicAudioPath*	ptPath			= NULL;
	UINT						downloadToPerf	= 0;
	DWORD						dwSegment		= 0;
	CHAR*						szFileName[2]	= {"T:\\DMTest1\\Segment8\\Download\\One.sgt",
												   "T:\\DMTest1\\Segment8\\Download\\Two.sgt"};

    //Create a audiopath.
    CHECKRUN(ptPerf8->CreateStandardAudioPath(DMUS_APATH_SHARED_STEREOPLUSREVERB, 32, TRUE, &ptPath));
    
    //Try downloading to both the performance and the audiopath.
    for (downloadToPerf = 0; downloadToPerf < 2; downloadToPerf++)
    {
        //Play both the segments.
        for (dwSegment=0; dwSegment<2 && SUCCEEDED(hr); dwSegment++)
        {
            //Load up the media file.
            hr = dmthLoadSegment(szFileName[dwSegment], &ptSegment8);
			if(FAILED(hr))
			{
				Log(ABORTLOGLEVEL, "!!!! BUG: dmthLoadSegment failed (%s == %08Xh)",tdmXlatHRESULT(hr), hr);
				failed = TRUE;
				continue;
			}

            //Download everything.
            Log(FYILOGLEVEL, "Downloading to %s interface", downloadToPerf ? "Performance8" : "AudioPath");
            CHECKRUN(ptSegment8->Download(downloadToPerf ? (CtIUnknown *)ptPerf8 : (CtIUnknown *)ptPath);) 
	        if(FAILED(hr))
				failed = TRUE;

            //Play the segment.
	        hr = ptPerf8->PlaySegmentEx(ptSegment8, 0, NULL, 0, __int64(0), NULL, NULL, ptPath);
	        if(FAILED(hr))
	        {
		        Log(ABORTLOGLEVEL, "!!!! BUG: pwPerformance->PlaySegmentEx failed (%s == %08Xh)",tdmXlatHRESULT(hr), hr);
                CHECKRUN(ptSegment8->Unload(downloadToPerf ? (CtIUnknown *)ptPerf8 : (CtIUnknown *)ptPath));
				failed = TRUE;
	        }
            Log(FYILOGLEVEL, "Playing segment for 5 seconds, you should hear the phrase ""%d"" being spoken", dwSegment+1);
            Wait(5000);

            Log(FYILOGLEVEL, "Unloading from %s interface", downloadToPerf ? "Performance8" : "AudioPath");
            CHECKRUN(ptSegment8->Unload(downloadToPerf ? (CtIUnknown *)ptPerf8 : (CtIUnknown *)ptPath));
	        if(FAILED(hr))
				failed = TRUE;

            SAFE_RELEASE(ptSegment8);            
        }
    };

    SAFE_RELEASE(ptPath);
    return failed ? E_FAIL : S_OK;
}


#define WAVES 4

HRESULT Segment8_Download_PreCache_Multiple01(CtIDirectMusicPerformance8* ptPerf8, DWORD dwUnused1, DWORD dwUnused2)
{
	CHAR*						szFileName[4]   = {
                                                    "T:\\DMTest1\\Segment8\\Download\\Streaming03\\StreamingWaves.sgt",
                                                    "T:\\DMTest1\\Segment8\\Download\\Streaming03\\NonStreamingWaves.sgt",
                                                    "T:\\DMTest1\\Segment8\\Download\\Streaming03\\StreamingWavesPlay.sgt",
                                                    "T:\\DMTest1\\Segment8\\Download\\Streaming03\\NonStreamingWavesPlay.sgt",
                                                    };


    return Segment8_Download_PreCache_Multiple(ptPerf8, szFileName);
}

/*
HRESULT Segment8_Download_PreCache_Multiple02(CtIDirectMusicPerformance8* ptPerf8, DWORD dwUnused1, DWORD dwUnused2)
{
	CHAR*						szFileName[4]   = {
                                                    "T:\\DMTest1\\Segment8\\Download\\StreamingNone\\y_shaman.sgt",
                                                    "T:\\DMTest1\\Segment8\\Download\\Streaming04\\y_shaman.sgt",
                                                    "T:\\DMTest1\\Segment8\\Download\\StreamingNone\\ssR0001.sgt",
                                                    "T:\\DMTest1\\Segment8\\Download\\Streaming04\\ssR0001.sgt"
                                                    };


    return Segment8_Download_PreCache_Multiple(ptPerf8, szFileName);
}
*/

/********************************************************************************
Verify that by downloading a segment, another segment can use its waves
********************************************************************************/
HRESULT Segment8_Download_PreCache_Multiple(CtIDirectMusicPerformance8* ptPerf8, LPSTR *szFileName)
{
	HRESULT						hr				= S_OK;
	CtIDirectMusicSegment8*		ptSegment[4]	= {NULL};
	CtIDirectMusicAudioPath*	ptPath			= NULL;
    CtIDirectMusicLoader *ptLoader = NULL;
	DWORD						dwSegment		= 0;
    CHAR szPath[MAX_PATH] = {NULL};
    CHAR szFile[MAX_PATH] = {NULL};


    
    DWORD i = 0;
    BOOL bDownloaded = FALSE;

    //Create an audiopath.
    CHECKRUN(ptPerf8->CreateStandardAudioPath(DMUS_APATH_SHARED_STEREOPLUSREVERB, 32, TRUE, &ptPath));

    //Create a loader.
    CHECKRUN(dmthCreateLoader(IID_IDirectMusicLoader, &ptLoader));


    //Load and download the first 2 segments.
    for (i=0; i<2 && SUCCEEDED(hr); i++)
    {

        CHECKRUN(ChopPath(szFileName[i], szPath, szFile));
        CHECKRUN(ptLoader->SetSearchDirectory(GUID_DirectMusicAllTypes, szPath, FALSE));
        CHECKRUN(ptLoader->LoadObjectFromFile(CLSID_DirectMusicSegment, CTIID_IDirectMusicSegment, szFile, (void **)&ptSegment[i]));
        CHECKRUN(ptSegment[i]->Download(ptPerf8));
    }

    //Download the first 2 segments.
    if (SUCCEEDED(hr))
        bDownloaded = TRUE;

    //Load the last 2 segments.
    for (i=2; i<4 && SUCCEEDED(hr); i++)
    {
        CHECKRUN(ChopPath(szFileName[i], szPath, szFile));
        CHECKRUN(ptLoader->SetSearchDirectory(GUID_DirectMusicAllTypes, szPath, FALSE));
        CHECKRUN(ptLoader->LoadObjectFromFile(CLSID_DirectMusicSegment, CTIID_IDirectMusicSegment, szFile, (void **)&ptSegment[i]));
    }

    //Don't download the last 2 segments, just play them.
    Log(FYILOGLEVEL, "You should hear both a oneshot and a streaming wave");
    for (i=2; i<4 && SUCCEEDED(hr); i++)
    {
        CHECKRUN(ptPerf8->PlaySegmentEx(ptSegment[i], NULL, NULL, DMUS_SEGF_SECONDARY, 0, NULL, NULL, ptPath));
        CHECKRUN(ExpectSegmentStart(ptPerf8, ptSegment[i], NULL, 2000, NULL));
    }

    //Stop the segments after 5 seconds.
    Wait(5000);
    CHECKRUN(ptPerf8->StopEx(0, 0, 0));


    //Unload
    if (bDownloaded)
    {
        for (i=0; i<2; i++)
        {
            ptSegment[i]->Unload(ptPerf8);
        }
    }

    //RELEASE everything.
    for (i=0; i<WAVES; i++)
    {
        RELEASE(ptSegment[i]);
    }

     RELEASE(ptPath);
     RELEASE(ptLoader);
     return hr;
}

#undef WAVES


/********************************************************************************
Verify that by downloading a segment, another segment can use its waves
********************************************************************************/
/*
HRESULT Segment8_Download_PreCache(CtIDirectMusicPerformance8* ptPerf8, DWORD dwFileName1, DWORD dwFileName2)
{
	HRESULT						hr				= S_OK;
	CtIDirectMusicSegment8*		ptSegment[2]	= {NULL};
	CtIDirectMusicAudioPath*	ptPath			= NULL;
	DWORD						dwSegment		= 0;
	CHAR*						szFileName[2]   = {NULL};


    
    DWORD i = 0;
    LPSTR szExpected = "You should hear a guy counting (streaming wave) and music (oneshot wave) simultaneously";
    BOOL bDownloaded = FALSE;
    szFileName[0] = (LPSTR)dwFileName1;
    szFileName[1] = (LPSTR)dwFileName2;

    //Create an audiopath.
    CHECKRUN(ptPerf8->CreateStandardAudioPath(DMUS_APATH_SHARED_STEREOPLUSREVERB, 32, TRUE, &ptPath));   

    //Load andthe segments.
    for (i=0; i<2 && SUCCEEDED(hr); i++)
    {
        CHECKRUN(dmthLoadSegment(szFileName[i], &ptSegment[i]));
    }

    //Download the first one and play it.
    CHECKRUN(ptSegment[0]->Download(ptPerf8));
    if (SUCCEEDED(hr))
        bDownloaded = TRUE;
    //CHECKRUN(ptPerf8->PlaySegmentEx(ptSegment[0], NULL, NULL, 0, 0, NULL, NULL, ptPath));
    //CHECKRUN(ExpectSegmentStart(ptPerf8, ptSegment[0], NULL, 2000, NULL));
    //CHECKRUN(Log(FYILOGLEVEL, "First wave (downloaded and played)"));
    CHECKRUN(Log(FYILOGLEVEL, szExpected));
    Wait(5000);
    
    //Stop the first segment
    CHECKRUN(ptPerf8->StopEx(0, 0, 0));
    CHECKRUN(ExpectSegmentStop(ptPerf8, ptSegment[0], NULL, 2000, NULL));

    //Simply play the second segment.
    CHECKRUN(ptPerf8->PlaySegmentEx(ptSegment[1], NULL, NULL, 0, 0, NULL, NULL, ptPath));
    CHECKRUN(ExpectSegmentStart(ptPerf8, ptSegment[1], NULL, 2000, NULL));
    CHECKRUN(Log(FYILOGLEVEL, "Second wave (merely played)"));
    CHECKRUN(Log(FYILOGLEVEL, szExpected));
    Wait(5000);

    //Stop the second segment
    CHECKRUN(ptPerf8->StopEx(0, 0, 0));
    CHECKRUN(ExpectSegmentStop(ptPerf8, ptSegment[1], NULL, 2000, NULL));

    //Unload
    if (bDownloaded)
        ptSegment[0]->Unload(ptPerf8);

    //RELEASE everything.
    for (i=0; i<2; i++)
    {
        RELEASE(ptSegment[i]);
    }

     RELEASE(ptPath);

     return hr;
}

*/