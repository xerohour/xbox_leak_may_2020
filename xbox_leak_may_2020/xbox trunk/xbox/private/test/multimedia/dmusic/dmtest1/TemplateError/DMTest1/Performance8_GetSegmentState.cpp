/********************************************************************************
FILE:

PURPOSE:

BY:
    DANHAFF
********************************************************************************/
#include "globals.h"
#include "cicmusicx.h"

DWORD Performance8_GetSegmentState_BVT(CtIDirectMusicPerformance8* ptPerf8);
DWORD Performance8_GetSegmentState_1(CtIDirectMusicPerformance8* ptPerf8);


/********************************************************************************
Main test function.
********************************************************************************/
HRESULT Performance8_GetSegmentState (CtIDirectMusicPerformance8* ptPerf8)
{
    HRESULT hr = S_OK;
    DWORD dwRes = FNS_FAIL;
    fnsLog(FYILOGLEVEL, "Calling main test function Performance8_GetSegmentState())");

    CALLDMTEST(Performance8_GetSegmentState_BVT(ptPerf8));                                            
    CALLDMTEST(Performance8_GetSegmentState_1(ptPerf8));                                              

    return hr;
};






/********************************************************************************
Main test function.
********************************************************************************/
DWORD Performance8_GetSegmentState_BVT(CtIDirectMusicPerformance8* ptPerf8)
{
	DWORD						dwRes				= FNS_PASS;
	HRESULT						hr					= S_OK;
	CtIDirectMusicSegment8		*pwSegment			= NULL;
	CtIDirectMusicSegmentState	*pwSegmentState		= NULL;
	CtIDirectMusicSegmentState	*pwSegmentState2	= NULL;
	MUSIC_TIME					mt					= 1024;

    // **********************************************************************
    // 3) Call PlayMidiFile() convienence function
    // **********************************************************************
	hr = dmthPlayMidiFile(ptPerf8,CTIID_IDirectMusicSegment,&pwSegment, &pwSegmentState, NULL);
	if(FAILED(hr))
	{
        fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: PlayMidiFile convienence function "),
                TEXT("failed (%s == %08Xh)"),
                tdmXlatHRESULT(hr), hr);
		dwRes = FNS_ABORTED;
		goto TEST_END;
	}

    // **********************************************************************
    // 3) Call GetTime() and check results
    // **********************************************************************
	hr = ptPerf8->GetTime( NULL, &mt );
	if(FAILED(hr))
	{
        fnsLog(BUGLOGLEVEL, TEXT("!!!! BUG: IDirectMusicPerformance->GetTime ")
                TEXT("failed (%s == %08Xh)"),
                tdmXlatHRESULT(hr), hr);
		dwRes = FNS_FAIL;
		goto TEST_END;
	}

	// **********************************************************************
    // 4) Call GetSegmentState and check resutls
    // **********************************************************************
	hr = ptPerf8->GetSegmentState( &pwSegmentState2, mt);
	if(FAILED(hr))
	{
		fnsLog(BUGLOGLEVEL, TEXT("!!!! BUG: Performance->GetSegmentState")
				TEXT("failed (%s == %08Xh)"),
				tdmXlatHRESULT(hr), hr);
		dwRes = FNS_FAIL;
		goto TEST_END;
	}

    // **********************************************************************
    // 5) Call Stop and check results
    // **********************************************************************
	Sleep(3000);
	hr = ptPerf8->StopEx(NULL, NULL, 0);
    if(FAILED(hr))
	{
        fnsLog(FYILOGLEVEL, TEXT("**** FYI: IDirectMusicPerformance->Stop ")
                TEXT("failed (%s == %08Xh)"),
                tdmXlatHRESULT(hr), hr);
	}

TEST_END:
    if (pwSegment && ptPerf8)
    {
        hr = pwSegment->Unload(ptPerf8);
        if(FAILED(hr))
            Log(ABORTLOGLEVEL, "ERROR!!  Unload returned %s (%08Xh)",tdmXlatHRESULT(hr), hr);
    }

	if(pwSegmentState)
	{
		pwSegmentState->Release();
		pwSegmentState = NULL;
	}
	if(pwSegmentState2)
	{
		pwSegmentState2->Release();
		pwSegmentState2 = NULL;
	}
	if(pwSegment)
	{
		pwSegment->Release();
        pwSegment = NULL;
	}

	dmthCoUninitialize();	
	return dwRes;

}




//--------------------------------------------------------------------------;
// @doc IDIRECTMUSICPERFORMANCE A-LLUCAR
//
// @topic IDirectMusicPerformance::GetSegmentState()_Valid_Test |
//
// Description: <nl>
// Valid test function for IDirectMusicPerformance::GetSegmentState().
//			Get a SegmentState that's set way into the future <nl>
//
// Test Function: <nl>
// tdmperfvGetSegmentStateValidTest1 <nl>
//
// Parameters: <nl>
//  BOOL    fCoCreate   - unused <nl>
//  BOOL    fMaxDebug   - unused <nl>
//  DWORD   dwParam1    - unused <nl>
//	DWORD   dwParam2    - unused <nl>
//
// File : TDMPERFV.CPP
//
// Steps: <nl>
// 1) Initialize COM. <nl>
// 2) Create CtIDirectMusicPerformance object<nl>
// 3) Get the Segment object <nl>
// 4) Add the port <nl>
// 5) Call GetTime to get the current time and check results <nl>
// 6) Call PlaySegment way into the future and check results <nl>
// 7) Call ReferenceToMusicTime and check results <nl>
// 8) Call GetSegmentState way in the future <nl>
// 9) Release objects, CoUninitilalize, and return results <nl>
//
// History: <nl>
//  05/18/1998 - a-llucar - created<nl>
//
// @index topic | IDIRECTMUSICPERFORMANCE_CASES
//--------------------------------------------------------------------------;
DWORD Performance8_GetSegmentState_1(CtIDirectMusicPerformance8* ptPerf8)
{
	HRESULT						hr				= E_NOTIMPL;
	DWORD						dwRes			= FNS_PASS;
	CtIDirectMusicSegment8		*pwSegment		= NULL;
	CtIDirectMusicSegmentState	*pwSegmentState	= NULL;
    CtIDirectMusicAudioPath     *pwAudioPath    = NULL;
	MUSIC_TIME					mt				= 0;

	CHAR						*pSegmentFile	= "T:\\Media\\Perf8\\GetSegSt\\DSegTst3.sgt";
    BOOL                        bDownloaded     = FALSE;

	// **********************************************************************
    // 3) Get the Segment object
    // **********************************************************************
   	hr = dmthLoadSegment(pSegmentFile, &pwSegment);
	if (FAILED(hr))
	{
		fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: dmthLoadSegment ")
				TEXT("failed (%s == %08Xh)"),
				tdmXlatHRESULT(hr), hr);
		dwRes = FNS_ABORTED;
		goto TEST_END;
	}

	// **********************************************************************
    // 3) Create an audiopath to play on.
    // **********************************************************************
    hr = ptPerf8->CreateStandardAudioPath(DMUS_APATH_SHARED_STEREOPLUSREVERB, 64, TRUE, &pwAudioPath);
	if (FAILED(hr))
	{
		fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: pwAudioPath")
				TEXT("failed (%s == %08Xh)"),
				tdmXlatHRESULT(hr), hr);
		dwRes = FNS_ABORTED;
		goto TEST_END;
	}

    //Download stuff to the path.
    hr = pwSegment->Download(pwAudioPath); 
	if(FAILED(hr))
	{
        fnsLog(ABORTLOGLEVEL, "**** ABORT: Download() returned %s (%08Xh)",tdmXlatHRESULT(hr), hr);
		goto TEST_END;
	}
    bDownloaded = TRUE;



    // **********************************************************************
    // 5) Call GetTime to get the current time and check results
    // **********************************************************************
	hr = ptPerf8->GetTime(NULL, &mt);
	if(FAILED(hr))
	{
        fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: IDirectMusicPerformance->GetTime ")
                TEXT("failed (%s == %08Xh)"),
                tdmXlatHRESULT(hr), hr);
		dwRes = FNS_ABORTED;
		goto TEST_END;
	}

    fnsLog(FYILOGLEVEL, "MUSIC_TIME is         %d.", mt);
    mt = mt + 1000000;
    fnsLog(FYILOGLEVEL, "Queuing segment for   %d.", mt);


    // **********************************************************************
    // 6) Call PlaySegment way into the future and check results
    // **********************************************************************
	// Set PlaySegment to play at time 1 billion after now
	hr = ptPerf8->PlaySegmentEx(pwSegment, NULL, NULL, DMUS_SEGF_BEAT, mt, NULL, NULL, pwAudioPath);
	if(FAILED(hr))
	{
        fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: ptPerf8->PlaySegment ")
                TEXT("failed (%s == %08Xh)"),
                tdmXlatHRESULT(hr), hr);
		dwRes = FNS_ABORTED;
		goto TEST_END;
	}

 	
    // **********************************************************************
    // 8) Call GetSegmentState way in the future
    // **********************************************************************
    fnsLog(FYILOGLEVEL, "Getting segment state at %d.", mt);
	hr = ptPerf8->GetSegmentState(&pwSegmentState, mt);
	if(FAILED(hr))
	{
        fnsLog(BUGLOGLEVEL, TEXT("!!!! BUG: ptPerf8->GetSegmentState(way in the future) ")
                TEXT("failed (%s == %08Xh)"),
                tdmXlatHRESULT(hr), hr);
		dwRes = FNS_FAIL;
		goto TEST_END;
	}
	Sleep(5000);
		
TEST_END:
    // **********************************************************************
    // 9) Release objects, CoUninitilalize, and return results
    // **********************************************************************

    if (bDownloaded)
    {
        hr = pwSegment->Unload(pwAudioPath); 
	    if(FAILED(hr))
	    {
            fnsLog(ABORTLOGLEVEL, "**** ABORT: Unload() returned %s (%08Xh). TEST APP LEAK!!",tdmXlatHRESULT(hr), hr);
		    goto TEST_END;
	    }
    }


    SAFE_RELEASE(pwSegment);
	SAFE_RELEASE(pwSegmentState);
    SAFE_RELEASE(pwAudioPath);
	return dwRes; 
} 
