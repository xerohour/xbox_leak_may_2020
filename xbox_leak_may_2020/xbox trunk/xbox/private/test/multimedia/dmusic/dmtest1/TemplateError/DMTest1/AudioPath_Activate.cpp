#include "globals.h"
#include "cicmusicx.h"


DWORD AudioPath_Activate_BVT(TESTPARAMS);


/********************************************************************************
Main test function.
********************************************************************************/
HRESULT AudioPath_Activate (CtIDirectMusicPerformance8* ptPerf8)
{
    HRESULT hr = S_OK;
    DWORD dwRes = FNS_FAIL;
    fnsLog(FYILOGLEVEL, "Calling main test function AudioPath_Activate())");
    CALLDMTEST(AudioPath_Activate_BVT(ptPerf8, 0, 0, 0, 0));                                                    
    return hr;
};

/********************************************************************************
//Later we'll go through the entire audiopath and make sure we can get everything
//  out of it.  For now though we'll just play a bunch of stuff on them.

//TESTPARAMS are unused except for ptPerf8
********************************************************************************/
DWORD AudioPath_Activate_BVT(TESTPARAMS)
{
DWORD dwRes = FNS_FAIL;
HRESULT hr = S_OK;
CtIDirectMusicSegment8 *ptSegment8 = NULL;
CtIDirectMusicAudioPath *ptPath     = NULL;
BOOL bDownloaded = FALSE;


    //Load default segment.
    hr = dmthLoadSegment(g_szDefaultMedia, &ptSegment8);
	if(FAILED(hr))
	{
        fnsLog(ABORTLOGLEVEL, "**** ABORT: dmthLoadSegment function returned %s (%08Xh)",tdmXlatHRESULT(hr), hr);
		goto TEST_END;
	}

    //Create an inactive audiopath.
    hr = ptPerf8->CreateStandardAudioPath(DMUS_APATH_SHARED_STEREOPLUSREVERB,
                                          16,
                                          FALSE,
                                          &ptPath);
    if (S_OK != hr)
	{
        fnsLog(ABORTLOGLEVEL, "**** ABORT: CreateStandardAudioPath(%s, %d, %d) returned %s (%08Xh) instead of S_OK",
                              "DMUS_APATH_SHARED_STEREOPLUSREVERB",
                              16,
                              FALSE,
                              tdmXlatHRESULT(hr),
                              hr);
		goto TEST_END;
	}


    //Activate(FALSE) on inactive path.
    hr = ptPath->Activate(FALSE);
	if(FAILED(hr))
	{
        fnsLog(ABORTLOGLEVEL, "**** Activate (FALSE) on already-inactive audiopath returned",tdmXlatHRESULT(hr), hr);
		goto TEST_END;
	}
    fnsLog(FYILOGLEVEL, "Calling Activate(FALSE) on inactive path, should hear no sound for 3 seconds.");
    Sleep(100);

    //Activate(TRUE) on inactive path.
    hr = ptPath->Activate(TRUE);
	if(FAILED(hr))
	{
        fnsLog(ABORTLOGLEVEL, "**** Activate (TRUE) on inactive audiopath returned",tdmXlatHRESULT(hr), hr);
		goto TEST_END;
	}
    fnsLog(FYILOGLEVEL, "Calling Activate(TRUE) on inactive path, should hear sound for 3 seconds.");
    Sleep(100);

    //Download to the active path.
    hr = ptSegment8->Download(ptPath);
	if(FAILED(hr))
	{
        fnsLog(ABORTLOGLEVEL, "**** Download on inactive audiopath returned",tdmXlatHRESULT(hr), hr);
		goto TEST_END;
	}
    Sleep(100);
    bDownloaded = TRUE;


    //Try to play a segment on it; verify you don't hear any sound.
	hr = ptPerf8->PlaySegmentEx( 
		ptSegment8, 
		0,
		NULL,
        0, 
		0,
		NULL,
		NULL,
		ptPath);
	if(FAILED(hr))
	{
        fnsLog(ABORTLOGLEVEL, "**** ABORT: PlaySegmentEx function returned %s (%08Xh)",tdmXlatHRESULT(hr), hr);
		goto TEST_END;
	}
    fnsLog(FYILOGLEVEL, "Playing segment on active path, should hear sound for 3 seconds.");
    Sleep(3000);


    //Activate(TRUE) on active path.
    hr = ptPath->Activate(TRUE);
	if(FAILED(hr))
	{
        fnsLog(ABORTLOGLEVEL, "**** Activate (TRUE) on active audiopath returned",tdmXlatHRESULT(hr), hr);
		goto TEST_END;
	}
    fnsLog(FYILOGLEVEL, "Calling Activate(TRUE) on active path, should hear sound for 3 seconds.");
    Sleep(3000);

    //Activate(FALSE) on active path.
    hr = ptPath->Activate(FALSE);
	if(FAILED(hr))
	{
        fnsLog(ABORTLOGLEVEL, "**** Activate (FALSE) on active audiopath returned",tdmXlatHRESULT(hr), hr);
		goto TEST_END;
	}
    fnsLog(FYILOGLEVEL, "Calling Activate(FALSE) on active path, should hear no sound for 3 seconds.");
    Sleep(3000);

    //Activate(TRUE) on deactivated path.
    hr = ptPath->Activate(TRUE);
	if(FAILED(hr))
	{
        fnsLog(ABORTLOGLEVEL, "**** Activate (TRUE) on deactivated audiopath returned",tdmXlatHRESULT(hr), hr);
		goto TEST_END;
	}
    fnsLog(FYILOGLEVEL, "Calling Activate(TRUE) on deactivated path, no sound expected.");
    Sleep(3000);


    dwRes = FNS_PASS;


TEST_END:
    if (bDownloaded && ptSegment8 && ptPath)
    {
        hr = ptSegment8->Unload(ptPath);
	    if(FAILED(hr))
	    {
            fnsLog(ABORTLOGLEVEL, "**** Unload failed!!!  May incur memory leaks",tdmXlatHRESULT(hr), hr);
	    }
    }

    SAFE_RELEASE(ptSegment8);
    SAFE_RELEASE(ptPath);

    return dwRes;
};