//+-------------------------------------------------------------------------
//
//  Microsoft Windows
//
//  Copyright (C) Microsoft Corporation, 1998 - 1999
//
//  File:       tdmperf.cpp
//
//--------------------------------------------------------------------------


#include "globals.h"


//--------------------------------------------------------------------------;
// @doc IDIRECTMUSICPERFORMANCE A-LLUCAR
//
// @topic IDirectMusicPerformance::SendPMsg()_Unit_Test |
//
// Description: <nl>
// Unit test function for IDirectMusicPerformance::SendPMsg().
//
// Test Function: <nl>
// tdmperfSendPMsgTest <nl>
//
// Parameters: <nl>
//  BOOL    fCoCreate   - unused <nl>
//  BOOL    fMaxDebug   - unused <nl>
//  DWORD   dwParam1    - unused <nl>
//	DWORD   dwParam2    - unused <nl>
//
// File : TDMPERF.CPP
//
// Steps: <nl>
// 1) Initialize COM. <nl>
// 2) Create CtIDirectMusicPerformance object <nl>
// 3) Init the Performance <nl>
// 4) Call AllocPMsg and check result <nl>
// 5) Set pNote with a bunch of particulars <nl>
// 6) Call SendPMsg and check result <nl>
// 7) Cleanup objects, uninitialize COM and return results <nl>
//
// History: <nl>
//  01/17/1998 - markburt - created <nl>
//  03/23/1998 - a-llucar - clean up, logging installed, autodoc <nl>
//	The same test is used for AllocPMsg() and SendPMsg() <nl>
//
// @index topic | IDIRECTMUSICPERFORMANCE_CASES
//--------------------------------------------------------------------------;
DWORD tdmperfSendPMsgTest(BOOL fCoCreate, BOOL fMaxDebug, 
						   LPARAM dwParam1, LPARAM dwParam2)
{
	DWORD							dwRes			= FNS_PASS;
	HRESULT							hr				= S_OK;
	DMUS_NOTE_PMSG					*pNote			= NULL;
	CtIDirectMusicPerformance8		*pwPerformance	= NULL;
    CtIDirectMusicLoader8             *ptLoader8      = NULL;
    CtIDirectMusicSegment8             *ptSegment8      = NULL;
	DMUS_OBJECTDESC DESC = {0};					// Descriptor to use to find it.
	
    // **********************************************************************
    // 1) Initialize COM. <nl>
    // **********************************************************************
	hr = dmthCoInitialize(NULL);
    if(FAILED(hr))
    {
        fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: COM initialization failed! (%s == %08Xh)"),
                tdmXlatHRESULT(hr), hr);
        return FNS_ABORTED;
    }

    // **********************************************************************
    // 2) Create CtIDirectMusicPerformance object 
    // **********************************************************************
    hr = dmthCreatePerformance(IID_IDirectMusicPerformance8, &pwPerformance);
	if(FAILED(hr))
	{
        fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: DirectMusicSegment object creation "),
                TEXT("failed (%s == %08Xh)"),
                tdmXlatHRESULT(hr), hr);
		dwRes = FNS_ABORTED;
		goto TEST_END;
	}


    hr = pwPerformance->InitAudio(NULL, NULL, NULL, DMUS_APATH_SHARED_STEREOPLUSREVERB, 128, 0, NULL);
    if (S_OK != hr)
    {
        fnsLog(ABORTLOGLEVEL,"**** ABORT: pwPerformance->InitAudio failed with %s (%08X)",tdmXlatHRESULT(hr), hr);
        goto TEST_END;
    }


    hr = dmthCreateLoader(IID_IDirectMusicLoader8, &ptLoader8);
    if (S_OK != hr)
    {
        fnsLog(ABORTLOGLEVEL,"**** ABORT: dmthCreateLoader failed with %s (%08X)",tdmXlatHRESULT(hr), hr);
        goto TEST_END;
    }


    hr = ptLoader8->SetSearchDirectory(GUID_DirectMusicAllTypes, gwszMediaPath, FALSE);
    if (FAILED(hr))
    {
        fnsLog(ABORTLOGLEVEL, "**** ERROR: IDirectMusicLoader8::SetSearchDirectory failed!!");
        goto TEST_END;
    }

    // Set the GM DLS Collection so we can load midi files.
    Log(FYILOGLEVEL, "Installing GM.DLS.\n");
	memset( &DESC, 0, sizeof(DMUS_OBJECTDESC) );
	DESC.dwSize = sizeof (DMUS_OBJECTDESC);
	DESC.guidClass = CLSID_DirectMusicCollection;  
	wcscpy(DESC.wszFileName,L"gm.dls");
	DESC.guidObject = GUID_DefaultGMCollection;
	DESC.dwValidData = DMUS_OBJ_CLASS | DMUS_OBJ_FILENAME | DMUS_OBJ_OBJECT;
	hr = ptLoader8->SetObject(&DESC);
	if (S_OK != hr)
	{
        fnsLog(ABORTLOGLEVEL, "**** ERROR: CICMusic: IDirectMusicLoader8::SetObject returned (%s == %08Xh)",
                tdmXlatHRESULT(hr), hr);
        goto TEST_END;
	}


    hr = ptLoader8->LoadObjectFromFile(CLSID_DirectMusicSegment, CTIID_IDirectMusicSegment8, g_wszDefaultMedia, (void **)&ptSegment8);
    if (S_OK != hr)
    {
        fnsLog(ABORTLOGLEVEL,"**** ABORT: pLoader8->LoadObjectFromFile failed with %s (%08X)",tdmXlatHRESULT(hr), hr);
        goto TEST_END;
    }

    hr = ptSegment8->Download(pwPerformance);
    if (S_OK != hr)
    {
        fnsLog(ABORTLOGLEVEL,"**** ABORT: ptSegment8->Download failed with %s (%08X)",tdmXlatHRESULT(hr), hr);
        goto TEST_END;
    }


	// **********************************************************************
    // 4) Call AllocPMsg and check result
    // **********************************************************************
	hr = pwPerformance->AllocPMsg( sizeof(DMUS_NOTE_PMSG), (DMUS_PMSG**)&pNote );
	if(FAILED(hr))
	{
        fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: IDirectMusicPerformance->AllocPMsg "),
                TEXT("failed (%s == %08Xh)"),
                tdmXlatHRESULT(hr), hr);
		dwRes = FNS_ABORTED;
		goto TEST_END;
	}

	// Check to make sure that pNote was properly populated
	if(!pNote)
	{
        fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: AllocPMsg did not properly populate pNote correctly, \n"),
                TEXT("pNote == %08Xh"),
                pNote);
		dwRes = FNS_ABORTED;
		goto TEST_END;
	}
    // **********************************************************************
    // 5) Set pNote with a bunch of particulars
    // **********************************************************************	
	ZeroMemory( pNote, sizeof(DMUS_NOTE_PMSG) );
	pNote->bFlags		= DMUS_NOTEF_NOTEON;
	pNote->bDurRange	= 100;
	pNote->wMusicValue	= 50;
	pNote->bVelocity	= 127;
	pNote->dwType		= DMUS_PMSGT_NOTE;
	pNote->dwFlags		= DMUS_PMSGF_REFTIME;

    // **********************************************************************
    // 6) Call SendPMsg and check results
    // **********************************************************************	
	hr = pwPerformance->SendPMsg( (DMUS_PMSG*)pNote );
	if(FAILED(hr))
	{
        fnsLog(BUGLOGLEVEL, TEXT("!!!! BUG: IDirectMusicPerformance->SendPMsg "),
                TEXT("failed (%s == %08Xh)"),
                tdmXlatHRESULT(hr), hr);
		dwRes = FNS_FAIL;
		goto TEST_END;
	}


//I verified this works.
/*
    hr = pwPerformance->PlaySegmentEx(ptSegment8, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
    if (S_OK != hr)
    {
        fnsLog(ABORTLOGLEVEL,"**** ABORT: pwPerformance->PlaySegmentEx failed with %s (%08X)",tdmXlatHRESULT(hr), hr);
        goto TEST_END;
    }

*/
    Log(FYILOGLEVEL, "Should hear a note play");
    Sleep(5000);

TEST_END:

    SAFE_RELEASE(ptLoader8);
    SAFE_RELEASE(ptSegment8);

    // **********************************************************************
    // 7) Cleanup objects and uninitialize COM and Return results
    // **********************************************************************
	if(pwPerformance)
	{
		pwPerformance->CloseDown();
		pwPerformance->Release();
		pwPerformance = NULL;
	}
	dmthCoUninitialize();	
	return dwRes;
}


//--------------------------------------------------------------------------;
// @doc IDIRECTMUSICPERFORMANCE A-LLUCAR
//
// @topic IDirectMusicPerformance::AllocPMsg()_Unit_Test |
//
// Description: <nl>
// Unit test function for IDirectMusicPerformance::AllocPMsg().
//
// Test Function: <nl>
// tdmperfSendPMsgTest <nl>
//
// Parameters: <nl>
//  BOOL    fCoCreate   - unused <nl>
//  BOOL    fMaxDebug   - unused <nl>
//  DWORD   dwParam1    - unused <nl>
//	DWORD   dwParam2    - unused <nl>
//
// File : TDMPERF.CPP
//
// Steps: <nl>
// 1) Initialize COM. <nl>
// 2) Create CtIDirectMusicPerformance object <nl>
// 3) Call Init on performance <nl>
// 4) Call AllocPMsg and check result <nl>
// 5) Set pNote with a bunch of particulars <nl>
// 6) Call SendPMsg and check result <nl>
// 7) Call FreePMsg and check results <nl>
// 8) Cleanup objects, uninitialize COM and return results <nl>
//
// History: <nl>
//  01/17/1998 - markburt - created <nl>
//  03/23/1998 - a-llucar - clean up, logging installed, autodoc <nl>
//	The same test is used for AllocPMsg() and SendPMsg() <nl>
//
// @index topic | IDIRECTMUSICPERFORMANCE_CASES
//--------------------------------------------------------------------------;
//  !!!!!!!!!! Test Case removed (uses tdmperfSendPMsgTest) !!!!!!!!!!!!!!!!


//--------------------------------------------------------------------------;
// @doc IDIRECTMUSICPERFORMANCE A-LLUCAR
//
// @topic IDirectMusicPerformance::MusicToReferenceTime()_Unit_Test |
//
// Description: <nl>
// Unit test function for IDirectMusicPerformance::MusicToReferenceTime(). <nl>
//
// Test Function: <nl>
// tdmperfMusicToReferenceTimeTest <nl>
//
// Parameters: <nl>
//  BOOL    fCoCreate   - unused <nl>
//  BOOL    fMaxDebug   - unused <nl>
//  DWORD   dwParam1    - unused <nl>
//	DWORD   dwParam2    - unused <nl>
//
// File : TDMPERF.CPP
//
// Steps: <nl>
// 1) Initialize COM. <nl>
// 2) Create CtIDirectMusicPerformance object <nl>
// 3) Init the performance <nl>
// 4) Call MusicToReferenceTime and check results<nl>
// 5) Cleanup objects, uninitialize COM and return results <nl>
//
// History: <nl>
//  01/17/1998 - markburt - created <nl>
//  03/23/1998 - a-llucar - clean up, logging installed, autodoc <nl>
//
// @index topic | IDIRECTMUSICPERFORMANCE_CASES
//--------------------------------------------------------------------------;
/*
DWORD tdmperfMusicToReferenceTimeTest(BOOL fCoCreate, BOOL fMaxDebug, 
						   LPARAM dwParam1, LPARAM dwParam2)
{
	DWORD						dwRes			= FNS_PASS;
	HRESULT						hr				= S_OK;
	CtIDirectMusicPerformance	*pwPerformance	= NULL;
	CtIDirectMusic				*pwMusic		= NULL;
	REFERENCE_TIME				rt				= NULL;

    // **********************************************************************
    // 1) Initialize COM. <nl>
    // **********************************************************************
	hr = dmthCoInitialize(NULL);
    if(FAILED(hr))
    {
        fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: COM initialization failed! (%s == %08Xh)"),
                tdmXlatHRESULT(hr), hr);
        return FNS_ABORTED;
    }

    // **********************************************************************
    // 2) Create CtIDirectMusicPerformance object 
    // **********************************************************************
    hr = dmthCreatePerformance(IID_IDirectMusicPerformance, &pwPerformance);
	if(FAILED(hr))
	{
        fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: DirectMusicSegment object creation "),
                TEXT("failed (%s == %08Xh)"),
                tdmXlatHRESULT(hr), hr);
		dwRes = FNS_ABORTED;
		goto TEST_END;
	}
	
    // **********************************************************************
    // 3) Init the Performance
    // **********************************************************************
	hr = pwPerformance->Init(&pwMusic,NULL,ghWnd);
	if(FAILED(hr))
	{
		fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: Performance->Init ")
				TEXT("failed (%s == %08Xh)"),
				tdmXlatHRESULT(hr), hr);
		dwRes = FNS_ABORTED;
		goto TEST_END;
	}

    // set autodownload on
    BOOL fAuto;
    fAuto = TRUE;
    hr = pwPerformance->SetGlobalParam( GUID_PerfAutoDownload, &fAuto, sizeof(BOOL) );
	if(FAILED(hr))
	{
		fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: Performance->SetGlobalParam(AutoDownload == TRUE) ")
				TEXT("failed (%s == %08Xh)"),
				tdmXlatHRESULT(hr), hr);
		dwRes = FNS_ABORTED;
		goto TEST_END;
	}

    // **********************************************************************
    // 4) Call MusicToReferenceTime and check results
    // **********************************************************************
	hr = pwPerformance->MusicToReferenceTime( 1000, &rt );

	// Test the function
	if(FAILED(hr))
	{
        fnsLog(BUGLOGLEVEL, TEXT("!!!! BUG: IDirectMusicPerformance->MusicToReference ")
                TEXT("failed (%s == %08Xh)"),
                tdmXlatHRESULT(hr), hr);
		dwRes = FNS_FAIL;
		goto TEST_END;
	}
		
	// Test to make sure that rt was populated
	if(!rt)
	{
        fnsLog(BUGLOGLEVEL, TEXT("!!!! BUG: ReferenceTime rt was not properly populated \n")
                TEXT("rt == %ul"), rt);
		dwRes = FNS_FAIL;
		goto TEST_END;
	}

TEST_END:
    // **********************************************************************
    // 5) Release objects, CoUninitialize, and return results
    // **********************************************************************
	if(pwPerformance)
	{
		pwPerformance->CloseDown();
		pwPerformance->Release();
        pwPerformance = NULL;
	}
	if(pwMusic)
	{
		pwMusic->Release();
        pwMusic = NULL;
	}

	dmthCoUninitialize();	

	return dwRes;
}
//===========================================================================


//--------------------------------------------------------------------------;
// @doc IDIRECTMUSICPERFORMANCE A-LLUCAR
//
// @topic IDirectMusicPerformance::ReferenceToMusicTime()_Unit_Test |
//
// Description: <nl>
// Unit test function for IDirectMusicPerformance::ReferenceToMusicTime(). <nl>
//
// Test Function: <nl>
// tdmperfReferenceToMusicTimeTest <nl>
//
// Parameters: <nl>
//  BOOL    fCoCreate   - unused <nl>
//  BOOL    fMaxDebug   - unused <nl>
//  DWORD   dwParam1    - unused <nl>
//	DWORD   dwParam2    - unused <nl>
//
// File : TDMPERF.CPP
//
// Steps: <nl>
// 1) Initialize COM. <nl>
// 2) Create CtIDirectMusicPerformance object <nl>
// 3) Init the performance <nl>
// 4) Call GetTime to try to get a time after that <nl>
// 5) Call ReferenceToMusicTime and check results<nl>
// 6) Cleanup objects, uninitialize COM and return results <nl>
//
// History: <nl>
//  01/17/1998 - markburt - created <nl>
//  03/23/1998 - a-llucar - clean up, logging installed, autodoc <nl>
//
// @index topic | IDIRECTMUSICPERFORMANCE_CASES
//--------------------------------------------------------------------------;
DWORD tdmperfReferenceToMusicTimeTest(BOOL fCoCreate, BOOL fMaxDebug, 
						   LPARAM dwParam1, LPARAM dwParam2)
{
	DWORD						dwRes			= FNS_PASS;
	HRESULT						hr				= S_OK;
	CtIDirectMusicPerformance	*pwPerformance	= NULL;
	CtIDirectMusic				*pwMusic		= NULL;
	MUSIC_TIME					mt				= NULL;
	REFERENCE_TIME				rt				= NULL;

    // **********************************************************************
    // 1) Initialize COM. <nl>
    // **********************************************************************
	hr = dmthCoInitialize(NULL);
    if(FAILED(hr))
    {
        fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: COM initialization failed! (%s == %08Xh)"),
                tdmXlatHRESULT(hr), hr);
        return FNS_ABORTED;
    }

    // **********************************************************************
    // 2) Create CtIDirectMusicPerformance object 
    // **********************************************************************
    hr = dmthCreatePerformance(IID_IDirectMusicPerformance, &pwPerformance);
	if(FAILED(hr))
	{
        fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: DirectMusicSegment object creation "),
                TEXT("failed (%s == %08Xh)"),
                tdmXlatHRESULT(hr), hr);
		dwRes = FNS_ABORTED;
		goto TEST_END;
	}
	
    // **********************************************************************
    // 3) Init the Performance
    // **********************************************************************
	hr = pwPerformance->Init(&pwMusic,NULL,ghWnd);
	if(FAILED(hr))
	{
		fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: Performance->Init ")
				TEXT("failed (%s == %08Xh)"),
				tdmXlatHRESULT(hr), hr);
		dwRes = FNS_ABORTED;
		goto TEST_END;
	}

    // set autodownload on
    BOOL fAuto;
    fAuto = TRUE;
    hr = pwPerformance->SetGlobalParam( GUID_PerfAutoDownload, &fAuto, sizeof(BOOL) );
	if(FAILED(hr))
	{
		fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: Performance->SetGlobalParam(AutoDownload == TRUE) ")
				TEXT("failed (%s == %08Xh)"),
				tdmXlatHRESULT(hr), hr);
		dwRes = FNS_ABORTED;
		goto TEST_END;
	}

    // **********************************************************************
    // 4) Call GetTime to try to get a time after that
    // **********************************************************************
	hr = pwPerformance->GetTime(&rt, NULL);
	if(FAILED(hr))
	{
		fnsLog(ABORTLOGLEVEL, TEXT("*** ABORT: Performance->GetTime ")
				TEXT("failed (%s == %08Xh)"),
				tdmXlatHRESULT(hr), hr);
		dwRes = FNS_ABORTED;
		goto TEST_END;
	}

	// **********************************************************************
    // 5) Call MusicToReferenceTime and check results
    // **********************************************************************
	hr = pwPerformance->ReferenceToMusicTime(rt + 300000, &mt);

	// Test the function
	if(FAILED(hr))
	{
        fnsLog(BUGLOGLEVEL, TEXT("!!!! BUG: IDirectMusicPerformance->MusicToReference ")
                TEXT("failed (%s == %08Xh)"),
                tdmXlatHRESULT(hr), hr);
		dwRes = FNS_FAIL;
		goto TEST_END;
	}

	// Test to make sure that rt was populated
	if(!mt)
	{
        fnsLog(BUGLOGLEVEL, TEXT("!!!! BUG: ReferenceTime rt was not properly populated \n")
                TEXT("mt == %ul"), mt);
		dwRes = FNS_FAIL;
		goto TEST_END;
	}

TEST_END:
    // **********************************************************************
    // 6) Release objects, CoUninitialize, and return results
    // **********************************************************************
	if(pwPerformance)
	{
		pwPerformance->CloseDown();
		pwPerformance->Release();
        pwPerformance = NULL;
	}
	if(pwMusic)
	{
		pwMusic->Release();
        pwMusic = NULL;
	}
	dmthCoUninitialize();	
	return dwRes;
}


//--------------------------------------------------------------------------;
// @doc IDIRECTMUSICPERFORMANCE A-LLUCAR
//
// @topic IDirectMusicPerformance::IsPlaying()_Unit_Test |
//
// Description: <nl>
// Unit test function for IDirectMusicPerformance::IsPlaying().
//
// Test Function: <nl>
// tdmperfPlaySegmentTest <nl>
//
// Parameters: <nl>
//  BOOL    fCoCreate   - unused <nl>
//  BOOL    fMaxDebug   - unused <nl>
//  DWORD   dwParam1    - unused <nl>
//	DWORD   dwParam2    - unused <nl>
//
// File : TDMPERF.CPP
//
// Steps: <nl>
// 1) Initialize COM. <nl>
// 2) Create CtIDirectMusicComposer object (wrapped IDMComposer object) <nl>
// 3) Create CtIDirectMusicLoader object (wrapped IDirectMusicLoader object) <nl>
// 4) Loader->EnableCache(GUID_DirectMusicAllTypes) <nl>
// 5) Load the Style Object <nl>
// 6) Load the ChordMap Object <nl>
// 7) Get two segments by calling ComposeSegmentFromShape <nl>
// 8) Create CtIDirectMusicPerformance object (wrapped IDirectMusicPerformance object) <nl>
// 9) Init the Performance <nl>
// 10) Add a port to the Performance (Synth port) <nl>
// 11) Activate the Performance <nl>
// 12) Play the Segment, check the results <nl>
// 13) Wait for IsPlaying to return favorably (or time out) <nl>
// 14) Stop the segment, check the results <nl>
// 15) Wait for IsPlaying to return favorably (or time out) <nl>
// 16) Cleanup objects and uninitialize COM and Return results <nl>
//
// History: <nl>
//  01/17/1998 - markburt - created <nl>
//  03/23/1998 - a-llucar - clean up, logging installed, autodoc <nl>
//	This same test is used for PlaySegment(), IsPlaying(), and Stop() <nl>
//
// @index topic | IDIRECTMUSICPERFORMANCE_CASES
//--------------------------------------------------------------------------;
// !!!!!  Test case removed (uses PlaySegment() test) !!!!!


//--------------------------------------------------------------------------;
// @doc IDIRECTMUSICPERFORMANCE A-LLUCAR
//
// @topic IDirectMusicPerformance::GetTime()_Unit_Test |
//
// Description: <nl>
// Unit test function for IDirectMusicPerformance::GetTime(). <nl>
//
// Test Function: <nl>
// tdmperfGetTimeTest <nl>
//
// Parameters: <nl>
//  BOOL    fCoCreate   - unused <nl>
//  BOOL    fMaxDebug   - unused <nl>
//  DWORD   dwParam1    - unused <nl>
//	DWORD   dwParam2    - unused <nl>
//
// File : TDMPERF.CPP
//
// Steps: <nl>
// 1) Initialize COM. <nl>
// 2) Create CtIDirectMusicComposer object (wrapped IDMComposer object) <nl>
// 3) Init the Performance <nl>
// 4) Call GetTime() and check Results<nl>
// 5) Release objects, CoUninitialize, and return results <nl>
//
// History: <nl>
//  01/14/1998 - markburt - created <nl>
//  03/23/1998 - a-llucar - clean up, logging installed, autodoc <nl>
//
// @index topic | IDIRECTMUSICPERFORMANCE_CASES
//--------------------------------------------------------------------------;
DWORD tdmperfGetTimeTest(BOOL fCoCreate, BOOL fMaxDebug, 
						   LPARAM dwParam1, LPARAM dwParam2)
{
	DWORD						dwRes			= FNS_PASS;
	HRESULT						hr				= S_OK;
	CtIDirectMusicPerformance	*pwPerformance	= NULL;
	CtIDirectMusic				*pwMusic		= NULL;
	MUSIC_TIME					mt				= NULL;

    // **********************************************************************
    // 1) Initialize COM. <nl>
    // **********************************************************************
	hr = dmthCoInitialize(NULL);
    if(FAILED(hr))
    {
        fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: COM initialization failed! (%s == %08Xh)"),
                tdmXlatHRESULT(hr), hr);
        return FNS_ABORTED;
    }

    // **********************************************************************
    // 2) Create CtIDirectMusicPerformance object 
    // **********************************************************************
    hr = dmthCreatePerformance(IID_IDirectMusicPerformance, &pwPerformance);
	if(FAILED(hr))
	{
        fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: DirectMusicSegment object creation "),
                TEXT("failed (%s == %08Xh)"),
                tdmXlatHRESULT(hr), hr);
		dwRes = FNS_ABORTED;
		goto TEST_END;
	}

    // **********************************************************************
    // 3) Init the Performance
    // **********************************************************************
	hr = pwPerformance->Init(&pwMusic,NULL,ghWnd);
	if(FAILED(hr))
	{
		fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: Performance->Init ")
				TEXT("failed (%s == %08Xh)"),
				tdmXlatHRESULT(hr), hr);
		dwRes = FNS_ABORTED;
		goto TEST_END;
	}

    // set autodownload on
    BOOL fAuto;
    fAuto = TRUE;
    hr = pwPerformance->SetGlobalParam( GUID_PerfAutoDownload, &fAuto, sizeof(BOOL) );
	if(FAILED(hr))
	{
		fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: Performance->SetGlobalParam(AutoDownload == TRUE) ")
				TEXT("failed (%s == %08Xh)"),
				tdmXlatHRESULT(hr), hr);
		dwRes = FNS_ABORTED;
		goto TEST_END;
	}

    // **********************************************************************
    // 3) Call GetTime() and check results
    // **********************************************************************
	hr = pwPerformance->GetTime( NULL, &mt );
	if(FAILED(hr))
	{
        fnsLog(BUGLOGLEVEL, TEXT("!!!! BUG: IDirectMusicPerformance->GetTime ")
                TEXT("failed (%s == %08Xh)"),
                tdmXlatHRESULT(hr), hr);
		dwRes = FNS_FAIL;
		goto TEST_END;
	}
		
TEST_END:
    // **********************************************************************
    // 5) Release objects, CoUninitialize, and return results
    // **********************************************************************
	if(pwPerformance)
	{
		pwPerformance->CloseDown();
		pwPerformance->Release();
        pwPerformance = NULL;
	}
	if(pwMusic)
	{
		pwMusic->Release();
        pwMusic = NULL;
	}
	dmthCoUninitialize();	
	return dwRes;
}
//===========================================================================




//--------------------------------------------------------------------------;
// @doc IDIRECTMUSICPERFORMANCE A-LLUCAR
//
// @topic IDirectMusicPerformance::FreePMsg()_Unit_Test |
//
// Description: <nl>
// Unit test function for IDirectMusicPerformance::FreePMsg(). <nl>
//
// Test Function: <nl>
// tdmperfGetNotificationPMsgTest <nl>
//
// Parameters: <nl>
//  BOOL    fCoCreate   - unused <nl>
//  BOOL    fMaxDebug   - unused <nl>
//  DWORD   dwParam1    - unused <nl>
//	DWORD   dwParam2    - unused <nl>
//
// File : TDMPERF.CPP
//
// Steps: <nl>
// 1) Initialize COM. <nl>
// 2) Create CtIDirectMusicComposer object (wrapped IDMComposer object) <nl>
// 3) Create an Event for hNotification <nl>
// 4) Create an Event for hNotification and check results <nl>
// 5) AddNotificationType and check results <nl>
// 6) Call PlayMidiFile() convienence function <nl>
// 7) Wait for a single object to know that things should be fine <nl>
// 8) Call GetNotificationPMsg and check results <nl>
// 9) Call FreePMsg and check results <nl>
// 10) Call Stop and check results <nl>
// 11) Release objects, CoUninitialize, and return results <nl>
//
// History: <nl>
//  01/14/1998 - markburt - created <nl>
//  03/23/1998 - a-llucar - clean up, logging installed, autodoc <nl>
//
//  NOTE: Same test used for GetNotificationPMsg() and FreePMsg()
//
// @index topic | IDIRECTMUSICPERFORMANCE_CASES
//--------------------------------------------------------------------------;
// !!!!!!!!!!! Test has been removed (uses GetNotificationPMsgTest) !!!!!!!!


//--------------------------------------------------------------------------;
// @doc IDIRECTMUSICPERFORMANCE A-LLUCAR
//
// @topic IDirectMusicPerformance::GetGraph()_Unit_Test |
//
// Description: <nl>
// Unit test function for IDirectMusicPerformance::GetGraph(). <nl>
//
// Test Function: <nl>
// tdmperfGetGraphTest <nl>
//
// Parameters: <nl>
//  BOOL    fCoCreate   - unused <nl>
//  BOOL    fMaxDebug   - unused <nl>
//  DWORD   dwParam1    - unused <nl>
//	DWORD   dwParam2    - unused <nl>
//
// File : TDMPERF.CPP
//
// Steps: <nl>
// 1) Initialize COM. <nl>
// 2) Create CtIDirectMusicComposer object (wrapped IDMComposer object) <nl>
// 3) Create CtIDirectMusicGraph object <nl>
// 4) Call SetGraph() and check the results <nl>
// 5) Call SetGraph() and check the results <nl>
// 6) Get real Graph objects to compare <nl> 
// 7) Compare the two Graphs to make sure they're the same <nl>
// 8) Release objects, CoUninitialize, and return results <nl>
//
// History: <nl>
//  01/14/1998 - markburt - created <nl>
//  03/23/1998 - a-llucar - clean up, logging installed, autodoc <nl>
//
//  Note: This is the same test for GetGraph() and SetGraph() <nl>
//
// @index topic | IDIRECTMUSICPERFORMANCE_CASES
//--------------------------------------------------------------------------;
DWORD tdmperfGetGraphTest(BOOL fCoCreate, BOOL fMaxDebug, 
						   LPARAM dwParam1, LPARAM dwParam2)
{
	DWORD							dwRes			= FNS_PASS;
	HRESULT							hr				= S_OK;
	CtIDirectMusicGraph				*pwGraph		= NULL;
	CtIDirectMusicGraph				*pwGraph2		= NULL;
	IDirectMusicGraph				*pGraph			= NULL;
	IDirectMusicGraph				*pGraph2		= NULL;
	CtIDirectMusicPerformance		*pwPerformance	= NULL;

    // **********************************************************************
    // 1) Initialize COM. <nl>
    // **********************************************************************
	hr = dmthCoInitialize(NULL);
    if(FAILED(hr))
    {
        fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: COM initialization failed! (%s == %08Xh)"),
                tdmXlatHRESULT(hr), hr);
        return FNS_ABORTED;
    }

    // **********************************************************************
    // 2) Create CtIDirectMusicPerformance object 
    // **********************************************************************
    hr = dmthCreatePerformance(IID_IDirectMusicPerformance, &pwPerformance);
	if(FAILED(hr))
	{
        fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: DirectMusicSegment object creation ")
                TEXT("failed (%s == %08Xh)"), tdmXlatHRESULT(hr), hr);
		dwRes = FNS_ABORTED;
		goto TEST_END;
	}

    // **********************************************************************
    // 3) Create CtIDirectMusicGraph object 
    // **********************************************************************
	hr = dmthCreateGraph(IID_IDirectMusicGraph, &pwGraph);
	if(FAILED(hr))
	{
        fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: DirectMusicSegment object creation ")
                TEXT("failed (%s == %08Xh)"), tdmXlatHRESULT(hr), hr);
		dwRes = FNS_ABORTED;
		goto TEST_END;
	}

    // **********************************************************************
    // 4) Call SetGraph() and check the results 
    // **********************************************************************
	hr = pwPerformance->SetGraph(pwGraph);
	if(FAILED(hr))
	{
        fnsLog(BUGLOGLEVEL, TEXT("!!!! BUG: IDirectMusicPerformance->SetGraph ")
                TEXT("failed (%s == %08Xh)"), tdmXlatHRESULT(hr), hr);
		dwRes = FNS_FAIL;
		goto TEST_END;
	}

    // **********************************************************************
    // 5) Call SetGraph() and check the results 
    // **********************************************************************
	hr = pwPerformance->GetGraph(&pwGraph2);
	if(FAILED(hr))
	{
        fnsLog(BUGLOGLEVEL, TEXT("!!!! BUG: IDirectMusicPerformance->GetGraph ")
                TEXT("failed (%s == %08Xh)"),
                tdmXlatHRESULT(hr), hr);
		dwRes = FNS_FAIL;
		goto TEST_END;
	}

    // **********************************************************************
    // 6) Get real Graph objects to compare 
    // **********************************************************************
	hr = pwGraph2->GetRealObjPtr(&pGraph2);
	if(FAILED(hr))
	{
        fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: GetRealObjPtr (Graph2) ")
                TEXT("failed (%s == %08Xh)"),
                tdmXlatHRESULT(hr), hr);
		dwRes = FNS_ABORTED;
		goto TEST_END;
	}

	hr = pwGraph->GetRealObjPtr(&pGraph);
	if(FAILED(hr))
	{
        fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: GetRealObjPtr (Graph) ")
                TEXT("failed (%s == %08Xh)"),
                tdmXlatHRESULT(hr), hr);
		dwRes = FNS_ABORTED;
		goto TEST_END;
	}

    // **********************************************************************
    // 7) Compare the two Graphs to make sure they're the same
    // **********************************************************************
	if(pGraph != pGraph2)
	{
        fnsLog(BUGLOGLEVEL, TEXT("!!!! BUG: pGraph != pGraph2 \n")
                TEXT("pGraph == %08Xh\n")
                TEXT("pGraph2 == %08Xh"),
				pGraph, pGraph2);
		dwRes = FNS_FAIL;
		goto TEST_END;
	}
		
TEST_END:
    // **********************************************************************
    // 8) Release objects, CoUninitialize, and return results
    // **********************************************************************
	if(pGraph)
	{
		pGraph->Release();
        pGraph = NULL;
	}
	if(pGraph2)
	{
		pGraph2->Release();
        pGraph2 = NULL;
	}
	if(pwGraph)
	{
		pwGraph->Release();
        pwGraph = NULL;
	}
	if(pwGraph2)
	{
		pwGraph2->Release();
        pwGraph2 = NULL;
	}
	if(pwPerformance)
	{
		pwPerformance->CloseDown();
		pwPerformance->Release();
        pwPerformance = NULL;
	}

	dmthCoUninitialize();	

	return dwRes;
}
//===========================================================================


//--------------------------------------------------------------------------;
// @doc IDIRECTMUSICPERFORMANCE A-LLUCAR
//
// @topic IDirectMusicPerformance::SetGraph()_Unit_Test |
//
// Description: <nl>
// Unit test function for IDirectMusicPerformance::SetGraph(). <nl>
//
// Test Function: <nl>
// tdmperfGetGraphTest <nl>
//
// Parameters: <nl>
//  BOOL    fCoCreate   - unused <nl>
//  BOOL    fMaxDebug   - unused <nl>
//  DWORD   dwParam1    - unused <nl>
//	DWORD   dwParam2    - unused <nl>
//
// File : TDMPERF.CPP
//
// Steps: <nl>
// 1) Initialize COM. <nl>
// 2) Create CtIDirectMusicComposer object (wrapped IDMComposer object) <nl>
// 3) Create CtIDirectMusicGraph object <nl>
// 4) Call SetGraph() and check the results <nl>
// 5) Call SetGraph() and check the results <nl>
// 6) Get real Graph objects to compare <nl> 
// 7) Compare the two Graphs to make sure they're the same <nl>
// 8) Release objects, CoUninitialize, and return results <nl>
//
// History: <nl>
//  01/14/1998 - markburt - created <nl>
//  03/23/1998 - a-llucar - clean up, logging installed, autodoc <nl>
//
//  Note: This is the same test for GetGraph() and SetGraph() <nl>
//
// @index topic | IDIRECTMUSICPERFORMANCE_CASES
//--------------------------------------------------------------------------;
// !!!!!  Test case removed (uses GetGraph() test) !!!!!


//--------------------------------------------------------------------------;
// @doc IDIRECTMUSICPERFORMANCE A-LLUCAR
//
// @topic IDirectMusicPerformance::SetNotificationHandle()_Unit_Test |
//
// Description: <nl>
// Unit test function for IDirectMusicPerformance::SetNotificationHandle(). <nl>
//
// Test Function: <nl>
// tdmperfSetNotificationHandleTest <nl>
//
// Parameters: <nl>
//  BOOL    fCoCreate   - unused <nl>
//  BOOL    fMaxDebug   - unused <nl>
//  DWORD   dwParam1    - unused <nl>
//	DWORD   dwParam2    - unused <nl>
//
// File : TDMPERF.CPP
//
// Steps: <nl>
// 1) Initialize COM. <nl>
// 2) Create CtIDirectMusicComposer object (wrapped IDMComposer object) <nl>
// 3) Create an hNotify Event <nl>
// 4) Call SetNotificationHandle and check results <nl>
// 5) Call SetNotificationHandle and check results (to clear it out) <nl>
// 6) Release objects, CoUninitialize, and return results <nl>
//
// History: <nl>
//  01/14/1998 - markburt - created <nl>
//  03/23/1998 - a-llucar - clean up, logging installed, autodoc <nl>
//
// @index topic | IDIRECTMUSICPERFORMANCE_CASES
//--------------------------------------------------------------------------;
DWORD tdmperfSetNotificationHandleTest(BOOL fCoCreate, BOOL fMaxDebug, 
						   LPARAM dwParam1, LPARAM dwParam2)
{
	DWORD							dwRes			= FNS_PASS;
	HRESULT							hr				= S_OK;
	CtIDirectMusicPerformance		*pwPerformance	= NULL;
	HANDLE							hNotify			= NULL;
	HANDLE							hNotifyClear	= NULL;

    // **********************************************************************
    // 1) Initialize COM. <nl>
    // **********************************************************************
	hr = dmthCoInitialize(NULL);
    if(FAILED(hr))
    {
        fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: COM initialization failed! (%s == %08Xh)"),
                tdmXlatHRESULT(hr), hr);
        return FNS_ABORTED;
    }

    // **********************************************************************
    // 2) Create CtIDirectMusicPerformance object 
    // **********************************************************************
    hr = dmthCreatePerformance(IID_IDirectMusicPerformance, &pwPerformance);
	if(FAILED(hr))
	{
        fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: DirectMusicSegment object creation ")
                TEXT("failed (%s == %08Xh)"),
                tdmXlatHRESULT(hr), hr);
		dwRes = FNS_ABORTED;
		goto TEST_END;
	}

    // **********************************************************************
    // 3) Create an hNotify Event
    // **********************************************************************
	hNotify = CreateEvent( NULL, FALSE, FALSE, NULL );
	if( !hNotify )
	{
        fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: hNotify Event ")
                TEXT("failed (hNotify == %08Xh)"),
                hNotify);
		dwRes = FNS_ABORTED;
		goto TEST_END;
	}

    // **********************************************************************
    // 4) Call SetNotificationHandle and check results
    // **********************************************************************
	hr = pwPerformance->SetNotificationHandle( hNotify, 0 );
	if(FAILED(hr))
	{
        fnsLog(BUGLOGLEVEL, TEXT("!!!! BUG: IDirectMusicPerformance->SetNotificationHandle (valid event)")
                TEXT("failed (%s == %08Xh)"),
                tdmXlatHRESULT(hr), hr);
		dwRes = FNS_FAIL;
		goto TEST_END;
	}

    // **********************************************************************
    // 5) Call SetNotificationHandle and check results (to clear it out)
    // **********************************************************************
	hr = pwPerformance->SetNotificationHandle( hNotifyClear, 0 );
	if(FAILED(hr))
	{
        fnsLog(BUGLOGLEVEL, TEXT("!!!! BUG: IDirectMusicPerformance->SetNotificationHandle (NULL event)")
                TEXT("failed (%s == %08Xh)"),
                tdmXlatHRESULT(hr), hr);
		dwRes = FNS_FAIL;
		goto TEST_END;
	}

TEST_END:
    // **********************************************************************
    // 6) Release objects, CoUninitialize, and return results
    // **********************************************************************
	if( hNotify )
	{
		CloseHandle(hNotify);
	}
	// Clean up the wrapper
	if(pwPerformance)
	{
		pwPerformance->CloseDown();
		pwPerformance->Release();
        pwPerformance = NULL;
	}

	dmthCoUninitialize();	
	return dwRes;
}
//===========================================================================


//--------------------------------------------------------------------------;
// @doc IDIRECTMUSICPERFORMANCE A-LLUCAR
//
// @topic IDirectMusicPerformance::GetNotificationPMsg()_Unit_Test |
//
// Description: <nl>
// Unit test function for IDirectMusicPerformance::GetNotificationPMsg(). <nl>
//
// Test Function: <nl>
// tdmperfGetNotificationPMsgTest <nl>
//
// Parameters: <nl>
//  BOOL    fCoCreate   - unused <nl>
//  BOOL    fMaxDebug   - unused <nl>
//  DWORD   dwParam1    - unused <nl>
//	DWORD   dwParam2    - unused <nl>
//
// File : TDMPERF.CPP
//
// Steps: <nl>
// 1) Initialize COM. <nl>
// 2) Create CtIDirectMusicComposer object (wrapped IDMComposer object) <nl>
// 3) Create an Event for hNotification <nl>
// 4) Create an Event for hNotification and check results <nl>
// 5) AddNotificationType and check results <nl>
// 6) Call PlayMidiFile() convienence function <nl>
// 7) Wait for a single object to know that things should be fine <nl>
// 8) Call GetNotificationPMsg and check results <nl>
// 9) Call FreePMsg and check results <nl>
// 10) Call Stop and check results <nl>
// 11) Release objects, CoUninitialize, and return results <nl>
//
// History: <nl>
//  01/14/1998 - markburt - created <nl>
//  03/23/1998 - a-llucar - clean up, logging installed, autodoc <nl>
//
//  NOTE: Same test used for GetNotificationPMsg() and FreePMsg()
//
// @index topic | IDIRECTMUSICPERFORMANCE_CASES
//--------------------------------------------------------------------------;
DWORD tdmperfGetNotificationPMsgTest(BOOL fCoCreate, BOOL fMaxDebug, 
						   LPARAM dwParam1, LPARAM dwParam2)
{
	DWORD						dwRes				= FNS_PASS;
	HRESULT						hr					= S_OK;
	DMUS_NOTIFICATION_PMSG		*pEvent				= NULL;
	CtIDirectMusicPerformance	*pwPerformance		= NULL;
	CtIDirectMusicSegment		*pwSegment			= NULL;
	HANDLE						hNotification       = NULL;
	GUID						guidNotification	= GUID_NOTIFICATION_SEGMENT;
	DWORD						dwResult            = 0;

    // **********************************************************************
    // 1) Initialize COM. <nl>
    // **********************************************************************
	hr = dmthCoInitialize(NULL);
    if(FAILED(hr))
    {
        fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: COM initialization failed! (%s == %08Xh)"),
                tdmXlatHRESULT(hr), hr);
        return FNS_ABORTED;
    }

    // **********************************************************************
    // 2) Create CtIDirectMusicPerformance object 
    // **********************************************************************
    hr = dmthCreatePerformance(IID_IDirectMusicPerformance, &pwPerformance);
	if(FAILED(hr))
	{
        fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: DirectMusicPerformance object creation ")
                TEXT("failed (%s == %08Xh)"),
                tdmXlatHRESULT(hr), hr);
		dwRes = FNS_ABORTED;
		goto TEST_END;
	}

    // **********************************************************************
    // 3) Create an Event for hNotification
    // **********************************************************************
	hNotification = CreateEvent( NULL, FALSE, FALSE, NULL );
	if( !hNotification )
	{
	    fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: Unable to Create an hNotificationEvent \n")
		    TEXT("(%s == %08Xh)"),
			tdmXlatHRESULT(hr), hr);
		dwRes = FNS_ABORTED;
		goto TEST_END;
	}

	// **********************************************************************
	// 4) Create an Event for hNotification and check results
	// **********************************************************************
    hr = pwPerformance->SetNotificationHandle( hNotification, 0 );
	if(FAILED(hr))
	{
	    fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: pwPerformance->SetNotificationHandle  ")
		    TEXT("failed (%s == %08Xh)"),
			tdmXlatHRESULT(hr), hr);
		dwRes = FNS_ABORTED;
		goto TEST_END;
	}
		
	// **********************************************************************
	// 5) AddNotificationType and check results 
	// **********************************************************************
    hr = pwPerformance->AddNotificationType( guidNotification );
	if(FAILED(hr))
	{
	    fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: pwPerformance->AddNotificationType ")
		    TEXT("failed (%s == %08Xh)"),
			tdmXlatHRESULT(hr), hr);
		dwRes = FNS_ABORTED;
		goto TEST_END;
	}

   	// **********************************************************************
    // 6) Call PlayMidiFile() convienence function
    // **********************************************************************
	hr = dmthPlayMidiFile(pwPerformance,CTIID_IDirectMusicSegment,(CtIDirectMusicSegment**)&pwSegment,(CtIDirectMusicSegmentState**)NULL, TRUE, NULL );
	if(FAILED(hr))
	{
        fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: PlayMidiFile convienence function "),
                TEXT("failed (%s == %08Xh)"),
                tdmXlatHRESULT(hr), hr);
		dwRes = FNS_ABORTED;
		goto TEST_END;
	}

   	// **********************************************************************
    // 7) Wait for a single object to know that things should be fine
    // **********************************************************************
	dwResult = WaitForSingleObject( hNotification, 1000 );
	if( dwResult != WAIT_OBJECT_0 )
	{
	    fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: WaitForSingleObject "));
		dwRes = FNS_ABORTED;
		goto TEST_END;
	}
	
	// **********************************************************************
    // 8) Call GetNotificationPMsg and check results
    // **********************************************************************
	hr = pwPerformance->GetNotificationPMsg(&pEvent);
	if(FAILED(hr))
	{
        fnsLog(BUGLOGLEVEL, TEXT("!!!! BUG: pwPerformance->GetNotificationPMsg ")
                TEXT("failed (%s == %08Xh)"),
                tdmXlatHRESULT(hr), hr);
		dwRes = FNS_FAIL;
		goto TEST_END;
	}
	
	// Check to make sure that pEvent was populated
	if(!pEvent)
	{
        fnsLog(BUGLOGLEVEL, TEXT("!!!! BUG: pwPerformance->GetNotificationPMsg ")
                TEXT("pEvent wasn't properly populated\n")
                TEXT("pEvent == %08Xh\n"),
                pEvent);
		dwRes = FNS_FAIL;
		goto TEST_END;
	}

    // **********************************************************************
    // 9) Call FreePMsg and check results
    // **********************************************************************
    hr = pwPerformance->FreePMsg( (DMUS_PMSG*)pEvent );
	if(FAILED(hr))
	{
        fnsLog(BUGLOGLEVEL, TEXT("!!!! BUG: pwPerformance->FreePMsg ")
                TEXT("failed (%s == %08Xh)"),
                tdmXlatHRESULT(hr), hr);
		dwRes = FNS_FAIL;
		goto TEST_END;
	}

		
    // **********************************************************************
    // 10) Call Stop and check results
    // **********************************************************************
	Sleep(3000);
	hr = pwPerformance->Stop(NULL, NULL, 0, 0);
	if(FAILED(hr))
	{
        fnsLog(BUGLOGLEVEL, TEXT("!!!! BUG: IDirectMusicPerformance->Stop ")
                TEXT("failed (%s == %08Xh)"),
                tdmXlatHRESULT(hr), hr);
        dwRes = FNS_FAIL;
	}

TEST_END:
    // **********************************************************************
    // 11) Release objects, CoUninitialize, and return results
    // **********************************************************************
	if(pwPerformance)
	{
		pwPerformance->CloseDown();
		pwPerformance->Release();
        pwPerformance = NULL;
	}
	if(hNotification)
	{
		CloseHandle(hNotification);
		hNotification = NULL;
	}
	if(pwSegment)
	{
		pwSegment->Release();
		pwSegment = NULL;
	}

	dmthCoUninitialize();	
	return dwRes;
}
//===========================================================================


//--------------------------------------------------------------------------;
// @doc IDIRECTMUSICPERFORMANCE A-LLUCAR
//
// @topic IDirectMusicPerformance::AddNotificationType()_Unit_Test |
//
// Description: <nl>
// Unit test function for IDirectMusicPerformance::AddNotificationType(). <nl>
//
// Test Function: <nl>
// tdmperfAddNotificationTypeTest <nl>
//
// Parameters: <nl>
//  BOOL    fCoCreate   - unused <nl>
//  BOOL    fMaxDebug   - unused <nl>
//  DWORD   dwParam1    - unused <nl>
//	DWORD   dwParam2    - unused <nl>
//
// File : TDMPERF.CPP
//
// Steps: <nl>
// 1) Initialize COM. <nl>
// 2) Create CtIDirectMusicComposer object (wrapped IDMComposer object) <nl>
// 3) Call AddNotificationType() and check results <nl>
// 4) Call RemoveNotificationType() and check results <nl>
// 5) Release objects, CoUninitialize, and return results <nl>
//
// History: <nl>
//  01/14/1998 - markburt - created <nl>
//  03/23/1998 - a-llucar - clean up, logging installed, autodoc <nl>
//
//	Note:  Same test for AddNotificationType() and RemoveNotificationType(). <nl>
//
// @index topic | IDIRECTMUSICPERFORMANCE_CASES
//--------------------------------------------------------------------------;
DWORD tdmperfAddNotificationTypeTest(BOOL fCoCreate, BOOL fMaxDebug, 
						         LPARAM dwParam1, LPARAM dwParam2)
{
	DWORD							dwRes			= FNS_PASS;
	HRESULT							hr				= S_OK;
	GUID							guid			= { 0, 0, 0, 0 };	
	CtIDirectMusicPerformance		*pwPerformance	= NULL;

    // **********************************************************************
    // 1) Initialize COM. <nl>
    // **********************************************************************
	hr = dmthCoInitialize(NULL);
    if(FAILED(hr))
    {
        fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: COM initialization failed! (%s == %08Xh)"),
                tdmXlatHRESULT(hr), hr);
        return FNS_ABORTED;
    }

    // **********************************************************************
    // 2) Create CtIDirectMusicPerformance object 
    // **********************************************************************
    hr = dmthCreatePerformance(IID_IDirectMusicPerformance, &pwPerformance);
	if(FAILED(hr))
	{
        fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: DirectMusicPerformance object creation ")
                TEXT("failed (%s == %08Xh)"),
                tdmXlatHRESULT(hr), hr);
		dwRes = FNS_ABORTED;
		goto TEST_END;
	}

    // **********************************************************************
    // 3) Call AddNotificationType and check results
    // **********************************************************************
	hr = pwPerformance->AddNotificationType(guid);
	if(FAILED(hr))
	{
        fnsLog(BUGLOGLEVEL, TEXT("!!!! BUG: IDirectMusicPerformance->AddNotificationType ")
                TEXT("failed (%s == %08Xh)"),
                tdmXlatHRESULT(hr), hr);
		dwRes = FNS_FAIL;
		goto TEST_END;
	}

	// **********************************************************************
    // 4) Call RemoveNotificationType and check results
    // **********************************************************************
	hr = pwPerformance->RemoveNotificationType(guid);
	if(FAILED(hr))
	{
        fnsLog(BUGLOGLEVEL, TEXT("!!!! BUG: IDirectMusicPerformance->RemoveNotificationType ")
                TEXT("failed (%s == %08Xh)"),
                tdmXlatHRESULT(hr), hr);
		dwRes = FNS_FAIL;
		goto TEST_END;
	}

TEST_END:
    // **********************************************************************
    // 5) Release objects, CoUninitialize, and return results
    // **********************************************************************
	// Clean up the wrapper
	if(pwPerformance)
	{
		pwPerformance->CloseDown();
		pwPerformance->Release();
        pwPerformance = NULL;
	}

	dmthCoUninitialize();	
	return dwRes;
}
//===========================================================================


//--------------------------------------------------------------------------;
// @doc IDIRECTMUSICPERFORMANCE A-LLUCAR
//
// @topic IDirectMusicPerformance::RemoveNotificationType()_Unit_Test |
//
// Description: <nl>
// Unit test function for IDirectMusicPerformance::RemoveNotificationType(). <nl>
//
// Test Function: <nl>
// tdmperfAddNotificationTypeTest <nl>
//
// Parameters: <nl>
//  BOOL    fCoCreate   - unused <nl>
//  BOOL    fMaxDebug   - unused <nl>
//  DWORD   dwParam1    - unused <nl>
//	DWORD   dwParam2    - unused <nl>
//
// File : TDMPERF.CPP
//
// Steps: <nl>
// 1) Initialize COM. <nl>
// 2) Create CtIDirectMusicComposer object (wrapped IDMComposer object) <nl>
// 3) Call AddNotificationType() and check results <nl>
// 4) Call RemoveNotificationType() and check results <nl>
// 5) Release objects, CoUninitialize, and return results <nl>
//
// History: <nl>
//  01/14/1998 - markburt - created <nl>
//  03/23/1998 - a-llucar - clean up, logging installed, autodoc <nl>
//
//	Note:  Same test for AddNotificationType() and RemoveNotificationType(). <nl>
//
// @index topic | IDIRECTMUSICPERFORMANCE_CASES
//--------------------------------------------------------------------------;
// !!!!!  Test case removed (uses AddNotificationType() test) !!!!!


//--------------------------------------------------------------------------;
// @doc IDIRECTMUSICPERFORMANCE A-LLUCAR
//
// @topic IDirectMusicPerformance::AddPort()_Unit_Test |
//
// Description: <nl>
// Unit test function for IDirectMusicPerformance::AddPort(). <nl>
//
// Test Function: <nl>
// tdmperfRemovePortTest <nl>
//
// Parameters: <nl>
//  BOOL    fCoCreate   - unused <nl>
//  BOOL    fMaxDebug   - unused <nl>
//  DWORD   dwParam1    - unused <nl>
//	DWORD   dwParam2    - unused <nl>
//
// File : TDMPERF.CPP
//
// Steps: <nl>
// 1) Initialize COM. <nl>
// 2) Create CtIDirectMusicComposer object (wrapped IDMComposer object) <nl>
// 3) Init a Performance <nl>
// 4) Enumerate a Port and check results <nl>
// 5) Create a Port and check results <nl>
// 6) Call AddPort and check results <nl>
// 7) Call RemovePort and check results <nl>
// 8) Release objects, CoUninitialize, and return results <nl>
//
// History: <nl>
//  01/14/1998 - markburt - created <nl>
//  03/23/1998 - a-llucar - clean up, logging installed, autodoc <nl>
//
//	Note:  Same test for AddPort() and RemovePort(). <nl>
//
// @index topic | IDIRECTMUSICPERFORMANCE_CASES
//--------------------------------------------------------------------------;
// !!!!!  Test case removed (uses RemovePort() test) !!!!!


//--------------------------------------------------------------------------;
// @doc IDIRECTMUSICPERFORMANCE A-LLUCAR
//
// @topic IDirectMusicPerformance::RemovePort()_Unit_Test |
//
// Description: <nl>
// Unit test function for IDirectMusicPerformance::RemovePort(). <nl>
//
// Test Function: <nl>
// tdmperfRemovePortTest <nl>
//
// Parameters: <nl>
//  BOOL    fCoCreate   - unused <nl>
//  BOOL    fMaxDebug   - unused <nl>
//  DWORD   dwParam1    - unused <nl>
//	DWORD   dwParam2    - unused <nl>
//
// File : TDMPERF.CPP
//
// Steps: <nl>
// 1) Initialize COM. <nl>
// 2) Create CtIDirectMusicComposer object (wrapped IDMComposer object) <nl>
// 3) Init a Performance <nl>
// 4) Enumerate a Port and check results <nl>
// 5) Create a Port and check results <nl>
// 6) Call AddPort and check results <nl>
// 7) Call RemovePort and check results <nl>
// 8) Release objects, CoUninitialize, and return results <nl>
//
// History: <nl>
//  01/14/1998 - markburt - created <nl>
//  03/23/1998 - a-llucar - clean up, logging installed, autodoc <nl>
//
//	Note:  Same test for AddPort() and RemovePort(). <nl>
//
// @index topic | IDIRECTMUSICPERFORMANCE_CASES
//--------------------------------------------------------------------------;
DWORD tdmperfRemovePortTest(BOOL fCoCreate, BOOL fMaxDebug, 
						   LPARAM dwParam1, LPARAM dwParam2)
{
	DWORD						dwRes			= FNS_PASS;
	HRESULT						hr				= S_OK;
	GUID						NullGUID		= {00000000-0000-0000-0000-000000000000};
	CtIDirectMusic				*pwMusic		= NULL;
	CtIDirectMusicPerformance	*pwPerformance	= NULL;
	CtIDirectMusicPort			*pwPort			= NULL;
	DMUS_PORTCAPS				dmpc;
	GUID				        guidPort		= GUID_NULL;

    // **********************************************************************
    // 1) Initialize COM. <nl>
    // **********************************************************************
	hr = dmthCoInitialize(NULL);
    if(FAILED(hr))
    {
        fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: COM initialization failed! (%s == %08Xh)"),
                tdmXlatHRESULT(hr), hr);
        return FNS_ABORTED;
    }

    // **********************************************************************
    // 2) Create CtIDirectMusicPerformance object 
    // **********************************************************************
    hr = dmthCreatePerformance(IID_IDirectMusicPerformance, &pwPerformance);
	if(FAILED(hr))
	{
        fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: DirectMusicSegment object creation ")
                TEXT("failed (%s == %08Xh)"),
                tdmXlatHRESULT(hr), hr);
		dwRes = FNS_ABORTED;
		goto TEST_END;
	}

    // **********************************************************************
    // 3) Init a Performance
    // **********************************************************************
	hr = pwPerformance->Init(&pwMusic,NULL,ghWnd);
	if(FAILED(hr))
	{
        fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: pwPerformance->Init ")
                TEXT("failed (%s == %08Xh)"),
                tdmXlatHRESULT(hr), hr);
		dwRes = FNS_ABORTED;
		goto TEST_END;
	}

	if(!pwMusic)
	{
		fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: pwPerformance not properly init'ed"));
		dwRes = FNS_ABORTED;
		goto TEST_END;
	}

    // set autodownload on
    BOOL fAuto;
    fAuto = TRUE;
    hr = pwPerformance->SetGlobalParam( GUID_PerfAutoDownload, &fAuto, sizeof(BOOL) );
	if(FAILED(hr))
	{
		fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: Performance->SetGlobalParam(AutoDownload == TRUE) ")
				TEXT("failed (%s == %08Xh)"),
				tdmXlatHRESULT(hr), hr);
		dwRes = FNS_ABORTED;
		goto TEST_END;
	}

    // **********************************************************************
    // 4) Enumerate a Port and check results
    // **********************************************************************
	ZeroMemory(&dmpc, sizeof(DMUS_PORTCAPS));
	dmpc.dwSize = sizeof(DMUS_PORTCAPS);
	hr = pwMusic->GetDefaultPort(&guidPort);
  	if(FAILED(hr))
	{
        fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: pwMusic->GetDefaultPort ")
                TEXT("failed (%s == %08Xh)"),
                tdmXlatHRESULT(hr), hr);
		dwRes = FNS_ABORTED;
		goto TEST_END;
	}

    // **********************************************************************
    // 5) Create a Port and check results
    // **********************************************************************
	DMUS_PORTPARAMS dmos;
	ZeroMemory(&dmos, sizeof(dmos));
	dmos.dwSize = sizeof(DMUS_PORTPARAMS);
	
	hr = pwMusic->CreatePort(
		guidPort,
		&dmos, 
		&pwPort, 
		NULL);
	if(FAILED(hr))
	{
        fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: pwMusic->CreatePort ")
                TEXT("failed (%s == %08Xh)"),
                tdmXlatHRESULT(hr), hr);
		dwRes = FNS_ABORTED;
		goto TEST_END;
	}

    // **********************************************************************
    // 6) Call AddPort and check results
    // **********************************************************************
	hr = pwPerformance->AddPort( pwPort );
	if(FAILED(hr))
	{
        fnsLog(BUGLOGLEVEL, TEXT("!!!! BUG: IDirectMusicPerformance->AddPort ")
                TEXT("failed (%s == %08Xh)"),
                tdmXlatHRESULT(hr), hr);
		dwRes = FNS_FAIL;
		goto TEST_END;
	}    
	
	// **********************************************************************
    // 7) Call RemovePort and check results
    // **********************************************************************
	hr = pwPerformance->RemovePort( pwPort );
	if(FAILED(hr))
	{
        fnsLog(BUGLOGLEVEL, TEXT("!!!! BUG: IDirectMusicPerformance->RemovePort ")
                TEXT("failed (%s == %08Xh)"),
                tdmXlatHRESULT(hr), hr);
		dwRes = FNS_FAIL;
		goto TEST_END;
	}

TEST_END:
    // **********************************************************************
    // 8) Release objects, CoUninitialize, and check results
    // **********************************************************************
	if(pwPerformance)
	{
		pwPerformance->CloseDown();
		pwPerformance->Release();
        pwPerformance = NULL;
	}
	if( pwMusic )
	{
		pwMusic->Release();
        pwMusic = NULL;
	}
	if( pwPort )
	{
		pwPort->Release();
        pwPort = NULL;
	}

	dmthCoUninitialize();	
	return dwRes;
}
//===========================================================================


//--------------------------------------------------------------------------;
// @doc IDIRECTMUSICPERFORMANCE A-LLUCAR
//
// @topic IDirectMusicPerformance::AssignPChannelBlock()_Unit_Test |
//
// Description: <nl>
// Unit test function for IDirectMusicPerformance::AssignPChannelBlock(). <nl>
//
// Test Function: <nl>
// tdmperfAssignPChannelBlockTest <nl>
//
// Parameters: <nl>
//  BOOL    fCoCreate   - unused <nl>
//  BOOL    fMaxDebug   - unused <nl>
//  DWORD   dwParam1    - unused <nl>
//	DWORD   dwParam2    - unused <nl>
//
// File : TDMPERF.CPP
//
// Steps: <nl>
// 1) Initialize COM. <nl>
// 2) Create CtIDirectMusicComposer object (wrapped IDMComposer object) <nl>
// 3) Init a Performance <nl>
// 4) Enumerate a Port and check results <nl>
// 5) Create a Port and check results <nl>
// 6) Call AddPort and check results <nl>
// 7) Call AssignPChannelBlock and check results <nl>
// 8) Call RemovePort and check results <nl>
// 9) Release objects, CoUninitialize, and return results <nl>
//
// History: <nl>
//  01/14/1998 - markburt - created <nl>
//  03/23/1998 - a-llucar - clean up, logging installed, autodoc <nl>
//
// @index topic | IDIRECTMUSICPERFORMANCE_CASES
//--------------------------------------------------------------------------;
DWORD tdmperfAssignPChannelBlockTest(BOOL fCoCreate, BOOL fMaxDebug, 
						   LPARAM dwParam1, LPARAM dwParam2)
{
	DWORD						dwRes			= FNS_PASS;
	HRESULT						hr				= S_OK;
	GUID						NullGUID		= {00000000-0000-0000-0000-000000000000};
	CtIDirectMusic				*pwMusic		= NULL;
	CtIDirectMusicPerformance	*pwPerformance	= NULL;
	CtIDirectMusicPort			*pwPort			= NULL;
	DMUS_PORTCAPS				dmpc;
	GUID				        guidPort			= GUID_NULL;

    // **********************************************************************
    // 1) Initialize COM. <nl>
    // **********************************************************************
	hr = dmthCoInitialize(NULL);
    if(FAILED(hr))
    {
        fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: COM initialization failed! (%s == %08Xh)"),
                tdmXlatHRESULT(hr), hr);
        return FNS_ABORTED;
    }

    // **********************************************************************
    // 2) Create CtIDirectMusicPerformance object 
    // **********************************************************************
    hr = dmthCreatePerformance(IID_IDirectMusicPerformance, &pwPerformance);
	if(FAILED(hr))
	{
        fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: DirectMusicSegment object creation ")
                TEXT("failed (%s == %08Xh)"),
                tdmXlatHRESULT(hr), hr);
		dwRes = FNS_ABORTED;
		goto TEST_END;
	}

    // **********************************************************************
    // 3) Init a Performance
    // **********************************************************************
	hr = pwPerformance->Init(&pwMusic,NULL,ghWnd);
	if(FAILED(hr))
	{
        fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: pwPerformance->Init ")
                TEXT("failed (%s == %08Xh)"),
                tdmXlatHRESULT(hr), hr);
		dwRes = FNS_ABORTED;
		goto TEST_END;
	}

	if(!pwMusic)
	{
		fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: pwPerformance not properly init'ed"));
		dwRes = FNS_ABORTED;
		goto TEST_END;
	}

    // set autodownload on
    BOOL fAuto;
    fAuto = TRUE;
    hr = pwPerformance->SetGlobalParam( GUID_PerfAutoDownload, &fAuto, sizeof(BOOL) );
	if(FAILED(hr))
	{
		fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: Performance->SetGlobalParam(AutoDownload == TRUE) ")
				TEXT("failed (%s == %08Xh)"),
				tdmXlatHRESULT(hr), hr);
		dwRes = FNS_ABORTED;
		goto TEST_END;
	}

    // **********************************************************************
    // 4) Enumerate a Port and check results
    // **********************************************************************
	ZeroMemory(&dmpc, sizeof(DMUS_PORTCAPS));
	dmpc.dwSize = sizeof(DMUS_PORTCAPS);

	hr = pwMusic->GetDefaultPort(&guidPort);
	if(FAILED(hr))
	{
        fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: pwMusic->GetDefaultPort ")
                TEXT("failed (%s == %08Xh)"),
                tdmXlatHRESULT(hr), hr);
		dwRes = FNS_ABORTED;
		goto TEST_END;
	}

    // **********************************************************************
    // 5) Create a Port and check results
    // **********************************************************************
	DMUS_PORTPARAMS dmos;
	ZeroMemory(&dmos, sizeof(dmos));
	dmos.dwSize = sizeof(DMUS_PORTPARAMS);
	
	hr = pwMusic->CreatePort(
		guidPort,
        &dmos, 
        &pwPort, 
        NULL);
	if(FAILED(hr))
	{
        fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: pwMusic->CreatePort ")
                TEXT("failed (%s == %08Xh)"),
                tdmXlatHRESULT(hr), hr);
		dwRes = FNS_ABORTED;
		goto TEST_END;
	}

    // **********************************************************************
    // 6) Call AddPort and check results
    // **********************************************************************
	hr = pwPerformance->AddPort( pwPort );
	if(FAILED(hr))
	{
        fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT:: IDirectMusicPerformance->AddPort ")
                TEXT("failed (%s == %08Xh)"),
                tdmXlatHRESULT(hr), hr);
		dwRes = FNS_ABORTED;
		goto TEST_END;
	}    
	
    // **********************************************************************
    // 7) Call AssignPChannelBlock and check results
    // **********************************************************************
	hr = pwPerformance->AssignPChannelBlock( 0, pwPort, 1 );
	if(FAILED(hr))
	{
        fnsLog(BUGLOGLEVEL, TEXT("!!!! BUG: IDirectMusicPerformance->AssignPChannelBlock ")
                TEXT("failed (%s == %08Xh)"),
                tdmXlatHRESULT(hr), hr);
		dwRes = FNS_FAIL;
		goto TEST_END;
	}

	// **********************************************************************
    // 8) Call RemovePort and check results
    // **********************************************************************
	hr = pwPerformance->RemovePort( pwPort );
	if(FAILED(hr))
	{
        fnsLog(BUGLOGLEVEL, TEXT("!!!! BUG: IDirectMusicPerformance->RemovePort ")
                TEXT("failed (%s == %08Xh)"),
                tdmXlatHRESULT(hr), hr);
        dwRes = FNS_FAIL;
	}

TEST_END:
	// **********************************************************************
    // 9) Release objects, CoUninitialize, and return results
    // **********************************************************************
	if(pwPerformance)
	{
		pwPerformance->CloseDown();
		pwPerformance->Release();
        pwPerformance = NULL;
	}
	if( pwMusic )
	{
		pwMusic->Release();
        pwMusic = NULL;
	}
	if( pwPort )
	{
		pwPort->Release();
        pwPort = NULL;
	}

	dmthCoUninitialize();	

	return dwRes;
}
//===========================================================================


//--------------------------------------------------------------------------;
// @doc IDIRECTMUSICPERFORMANCE A-LLUCAR
//
// @topic IDirectMusicPerformance::AssignPChannel()_Unit_Test |
//
// Description: <nl>
// Unit test function for IDirectMusicPerformance::AssignPChannel(). <nl>
//
// Test Function: <nl>
// tdmperfAssignPChannelTest <nl>
//
// Parameters: <nl>
//  BOOL    fCoCreate   - unused <nl>
//  BOOL    fMaxDebug   - unused <nl>
//  DWORD   dwParam1    - unused <nl>
//	DWORD   dwParam2    - unused <nl>
//
// File : TDMPERF.CPP
//
// Steps: <nl>
// 1) Initialize COM. <nl>
// 2) Create CtIDirectMusicComposer object (wrapped IDMComposer object) <nl>
// 3) Init a Performance <nl>
// 4) Enumerate a Port and check results <nl>
// 5) Create a Port and check results <nl>
// 6) Call AddPort and check results <nl>
// 7) Call AssignPChannel and check results <nl>
// 8) Call RemovePort and check results <nl>
// 9) Release objects, CoUninitialize, and return results <nl>
//
// History: <nl>
//  01/14/1998 - markburt - created <nl>
//  03/23/1998 - a-llucar - clean up, logging installed, autodoc <nl>
//
// @index topic | IDIRECTMUSICPERFORMANCE_CASES
//--------------------------------------------------------------------------;
DWORD tdmperfAssignPChannelTest(BOOL fCoCreate, BOOL fMaxDebug, 
						   LPARAM dwParam1, LPARAM dwParam2)
{
	DWORD						dwRes			= FNS_PASS;
	HRESULT						hr				= S_OK;
	CtIDirectMusic				*pwMusic		= NULL;
	CtIDirectMusicPerformance	*pwPerformance	= NULL;
	CtIDirectMusicPort			*pwPort			= NULL;
	DMUS_PORTCAPS				dmpc;
	GUID				        guidPort		= GUID_NULL;

    // **********************************************************************
    // 1) Initialize COM. <nl>
    // **********************************************************************
	hr = dmthCoInitialize(NULL);
    if(FAILED(hr))
    {
        fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: COM initialization failed! (%s == %08Xh)"),
                tdmXlatHRESULT(hr), hr);
        return FNS_ABORTED;
    }

    // **********************************************************************
    // 2) Create CtIDirectMusicPerformance object 
    // **********************************************************************
    hr = dmthCreatePerformance(IID_IDirectMusicPerformance, &pwPerformance);
	if(FAILED(hr))
	{
        fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: DirectMusicSegment object creation ")
                TEXT("failed (%s == %08Xh)"),
                tdmXlatHRESULT(hr), hr);
		dwRes = FNS_ABORTED;
		goto TEST_END;
	}

    // **********************************************************************
    // 3) Init a Performance
    // **********************************************************************
	hr = pwPerformance->Init(&pwMusic,NULL,ghWnd);
	if(FAILED(hr))
	{
        fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: pwPerformance->Init ")
                TEXT("failed (%s == %08Xh)"),
                tdmXlatHRESULT(hr), hr);
		dwRes = FNS_ABORTED;
		goto TEST_END;
	}
	
	if(!pwMusic)
	{
		fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: pwPerformance not properly init'ed"));
		dwRes = FNS_ABORTED;
		goto TEST_END;
	}

    // set autodownload on
    BOOL fAuto;
    fAuto = TRUE;
    hr = pwPerformance->SetGlobalParam( GUID_PerfAutoDownload, &fAuto, sizeof(BOOL) );
	if(FAILED(hr))
	{
		fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: Performance->SetGlobalParam(AutoDownload == TRUE) ")
				TEXT("failed (%s == %08Xh)"),
				tdmXlatHRESULT(hr), hr);
		dwRes = FNS_ABORTED;
		goto TEST_END;
	}

    // **********************************************************************
    // 4) Enumerate a Port and check results
    // **********************************************************************
	ZeroMemory(&dmpc, sizeof(DMUS_PORTCAPS));
	dmpc.dwSize = sizeof(DMUS_PORTCAPS);

	hr = pwMusic->GetDefaultPort(&guidPort);
	if(FAILED(hr))
	{
		fnsLog(BUGLOGLEVEL, "**** ABORT: GetDefaultPort failed! "
                "(%s == %08Xh)",
                dmthXlatHRESULT(hr), hr);
        dwRes = FNS_ABORTED;
        goto TEST_END;
	}

    // **********************************************************************
    // 5) Create a Port and check results
    // **********************************************************************
	DMUS_PORTPARAMS dmos;
	ZeroMemory(&dmos, sizeof(dmos));
	dmos.dwSize = sizeof(DMUS_PORTPARAMS);
	
	hr = pwMusic->CreatePort(
		guidPort,
        &dmos, 
        &pwPort, 
        NULL);
	if(FAILED(hr))
	{
        fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: pwMusic->CreatePort ")
                TEXT("failed (%s == %08Xh)"),
                tdmXlatHRESULT(hr), hr);
		dwRes = FNS_ABORTED;
		goto TEST_END;
	}

    // **********************************************************************
    // 6) Call AddPort and check results
    // **********************************************************************
	hr = pwPerformance->AddPort( pwPort );
	if(FAILED(hr))
	{
        fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT:: IDirectMusicPerformance->AddPort ")
                TEXT("failed (%s == %08Xh)"),
                tdmXlatHRESULT(hr), hr);
		dwRes = FNS_ABORTED;
		goto TEST_END;
	}    
	
    // **********************************************************************
    // 7) Call AssignPChannel and check results
    // **********************************************************************
	hr = pwPerformance->AssignPChannel( 0, pwPort, 1, 0 );
	if(FAILED(hr))
	{
        fnsLog(BUGLOGLEVEL, TEXT("!!!! BUG: IDirectMusicPerformance->AssignPChannel ")
                TEXT("failed (%s == %08Xh)"),
                tdmXlatHRESULT(hr), hr);
		dwRes = FNS_FAIL;
		goto TEST_END;
	}

	// **********************************************************************
    // 8) Call RemovePort and check results
    // **********************************************************************
	hr = pwPerformance->RemovePort( pwPort );
	if(FAILED(hr))
	{
        fnsLog(BUGLOGLEVEL, TEXT("!!!! BUG: IDirectMusicPerformance->RemovePort ")
                TEXT("failed (%s == %08Xh)"),
                tdmXlatHRESULT(hr), hr);
        dwRes = FNS_FAIL;
	}

TEST_END:
	// **********************************************************************
    // 9) Release objects, CoUninitialize, and return results
    // **********************************************************************
	if(pwPerformance)
	{
		pwPerformance->CloseDown();
		pwPerformance->Release();
        pwPerformance = NULL;
	}
	if( pwMusic )
	{
		pwMusic->Release();
        pwMusic = NULL;
	}
	if( pwPort )
	{
		pwPort->Release();
        pwPort = NULL;
	}

	dmthCoUninitialize();	
	return dwRes;
}
//===========================================================================


//--------------------------------------------------------------------------;
// @doc IDIRECTMUSICPERFORMANCE A-LLUCAR
//
// @topic IDirectMusicPerformance::PChannelInfo()_Unit_Test |
//
// Description: <nl>
// Unit test function for IDirectMusicPerformance::PChannelInfo(). <nl>
//
// Test Function: <nl>
// tdmperfPChannelInfoTest <nl>
//
// Parameters: <nl>
//  BOOL    fCoCreate   - unused <nl>
//  BOOL    fMaxDebug   - unused <nl>
//  DWORD   dwParam1    - unused <nl>
//	DWORD   dwParam2    - unused <nl>
//
// File : TDMPERF.CPP
//
// Steps: <nl>
// 1) Initialize COM. <nl>
// 2) Create CtIDirectMusicComposer object (wrapped IDMComposer object) <nl>
// 3) Call AddPort (Synth) and check results <nl>
// 4) Call PChannelInfo and check results <nl>
// 5) Check to make sure that PChannelInfo returned the right info <nl>
// 6) Release object, CoUninitialize, and return results <nl>
// 7) Call AssignPChannel and check results <nl>
// 8) Call RemovePort and check results <nl>
// 9) Release objects, CoUninitialize, and return results <nl>
//
// History: <nl>
//  01/14/1998 - markburt - created <nl>
//  03/23/1998 - a-llucar - clean up, logging installed, autodoc <nl>
//
// @index topic | IDIRECTMUSICPERFORMANCE_CASES
//--------------------------------------------------------------------------;
DWORD tdmperfPChannelInfoTest(BOOL fCoCreate, BOOL fMaxDebug, 
						   LPARAM dwParam1, LPARAM dwParam2)
{
	DWORD							dwRes			= FNS_PASS;
	HRESULT							hr				= S_OK;
	CtIDirectMusicPort				*pwPort			= NULL;
	CtIDirectMusicPerformance		*pwPerformance	= NULL;
	DWORD							dwGroup			= 0;
	DWORD							dwChannel		= 0;

    // **********************************************************************
    // 1) Initialize COM. <nl>
    // **********************************************************************
	hr = dmthCoInitialize(NULL);
    if(FAILED(hr))
    {
        fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: COM initialization failed! (%s == %08Xh)"),
                tdmXlatHRESULT(hr), hr);
        return FNS_ABORTED;
    }

    // **********************************************************************
    // 2) Create CtIDirectMusicPerformance object 
    // **********************************************************************
    hr = dmthCreatePerformance(IID_IDirectMusicPerformance, &pwPerformance);
	if(FAILED(hr))
	{
        fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: DirectMusicSegment object creation ")
                TEXT("failed (%s == %08Xh)"),
                tdmXlatHRESULT(hr), hr);
		dwRes = FNS_ABORTED;
		goto TEST_END;
	}

    // **********************************************************************
    // 3) Init(NULL) and AddPort(NULL)
    // **********************************************************************	
	hr = pwPerformance->Init(NULL,NULL,ghWnd); 
	if(FAILED(hr))
	{
        fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: pwPerformance->Init(NULL)")
                TEXT("failed (%s == %08Xh)"),
                tdmXlatHRESULT(hr), hr);
		dwRes = FNS_ABORTED;
		goto TEST_END;
	}

    // set autodownload on
    BOOL fAuto;
    fAuto = TRUE;
    hr = pwPerformance->SetGlobalParam( GUID_PerfAutoDownload, &fAuto, sizeof(BOOL) );
	if(FAILED(hr))
	{
		fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: Performance->SetGlobalParam(AutoDownload == TRUE) ")
				TEXT("failed (%s == %08Xh)"),
				tdmXlatHRESULT(hr), hr);
		dwRes = FNS_ABORTED;
		goto TEST_END;
	}

	hr = pwPerformance->AddPort(NULL); 
	if(FAILED(hr))
	{
        fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: pwPerformance->AddPort(NULL)")
                TEXT("failed (%s == %08Xh)"),
                tdmXlatHRESULT(hr), hr);
		dwRes = FNS_ABORTED;
		goto TEST_END;
	}

    // **********************************************************************
    // 4) Call PChannelInfo and check results
    // **********************************************************************
	hr = pwPerformance->PChannelInfo( 0, &pwPort, &dwGroup, &dwChannel );
	if(FAILED(hr))
	{
        fnsLog(BUGLOGLEVEL, TEXT("!!!! BUG: IDirectMusicPerformance->PChannelInfo ")
                TEXT("failed (%s == %08Xh)"),
                tdmXlatHRESULT(hr), hr);
		dwRes = FNS_FAIL;
		goto TEST_END;
	}
    
	// **********************************************************************
    // 5) Check to make sure that PChannelInfo returned the right info
    // **********************************************************************
	if((NULL == pwPort) || (1 != dwGroup) || (0 != dwChannel))
	{
        fnsLog(BUGLOGLEVEL, TEXT("!!!! BUG: IDirectMusicPerformance->PChannelInfo ")
                TEXT("Did not return the right information\n")
				TEXT("pwPort -- Expected NULL, Returned %08Xh\n")
				TEXT("dwGroup -- Expected 1, Returned %ld\n")
				TEXT("dwChannel -- Expected 0, Returned %ld\n"),
                pwPort, dwGroup, dwChannel);
		dwRes = FNS_FAIL;
		goto TEST_END;
	}

TEST_END:
    // **********************************************************************
    // 6) Release object, CoUninitialize, and return results
    // **********************************************************************
	if( pwPort )
	{
		pwPort->Release();
		pwPort = NULL;
	}
	if(pwPerformance)
	{
		pwPerformance->CloseDown();
		pwPerformance->Release();
		pwPerformance = NULL;
	}

	dmthCoUninitialize();	
	return dwRes;
}
//===========================================================================


//--------------------------------------------------------------------------;
// @doc IDIRECTMUSICPERFORMANCE A-LLUCAR
//
// @topic IDirectMusicPerformance::DownloadInstrumentTest()_Unit_Test |
//
// Description: <nl>
// Unit test function for IDirectMusicPerformance::DownloadInstrument(). <nl>
//
// Test Function: <nl>
// tdmperfDownloadInstrumentTest <nl>
//
// Parameters: <nl>
//  BOOL    fCoCreate   - unused <nl>
//  BOOL    fMaxDebug   - unused <nl>
//  DWORD   dwParam1    - unused <nl>
//	DWORD   dwParam2    - unused <nl>
//
// File : TDMPERF.CPP
//
// Steps: <nl>
// 1) Initialize COM. <nl>
// 2) Create CtIDirectMusicPerformance object <nl>
// 3) Create CtIDirectMusicLoader object<nl>
// 4) Get the Object from the loader<nl>
// 5) Do QI on the object for the collection <nl>
// 6) Create a wrapped Instrument Collection <nl>
// 7) Create a wrapper for IDirectMusicCollection Object <nl>
// 8) Get an instrument collection and release the collection <nl>
// 9) Call Perf->AddPort(NULL) to set everything up for downloading <nl>
// 10) Call Download and check results <nl>
// 11) Release object, CoUninitialize, and return results <nl>
//
// History: <nl>
//  01/14/1998 - markburt - created <nl>
//  04/08/1998 - jimmo    - clean up autodoc <nl>
//
// @index topic | IDIRECTMUSICPERFORMANCE_CASES
//--------------------------------------------------------------------------;
DWORD tdmperfDownloadInstrumentTest(BOOL fCoCreate, BOOL fMaxDebug, 
						   LPARAM dwParam1, LPARAM dwParam2)
{
	DWORD								dwRes			= FNS_PASS;
	HRESULT								hr				= S_OK;
	GUID								NullGUID		= {00000000-0000-0000-0000-000000000000};
	CtIDirectMusicLoader				*pIDMLoader		= NULL;
	CtIDirectMusicCollection			*pwCollection	= NULL;
	CtIDirectMusicInstrument			*pwInstrument	= NULL;
	CtIDirectMusicDownloadedInstrument  *pwDownloaded	= NULL;
	CtIDirectMusicPerformance			*pwPerformance	= NULL;
	CtIDirectMusicPort					*pwPort			= NULL;
	DMUS_OBJECTDESC						desc; 
	DMUS_NOTERANGE						pNoteRanges;
	ZeroMemory(&desc, sizeof(desc));
	desc.dwSize											= sizeof(DMUS_OBJECTDESC);
	DWORD								dwGroup			= 0;
	DWORD								dwMChannel		= 0;

	dmthSetPath(NULL);
	WCHAR								*wchPath		= gwszMediaPath;

    // **********************************************************************
    // 1) Initialize COM. <nl>
    // **********************************************************************
	hr = dmthCoInitialize(NULL);
    if(FAILED(hr))
    {
        fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: COM initialization failed! (%s == %08Xh)"),
                tdmXlatHRESULT(hr), hr);
        return FNS_ABORTED;
    }

    // **********************************************************************
    // 2) Create CtIDirectMusicPerformance object 
    // **********************************************************************
    hr = dmthCreatePerformance(IID_IDirectMusicPerformance, &pwPerformance);
	if(FAILED(hr))
	{
        fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: DirectMusicPerformance object creation ")
                TEXT("failed (%s == %08Xh)"),
                tdmXlatHRESULT(hr), hr);
		dwRes = FNS_ABORTED;
		goto TEST_END;
	}

    // **********************************************************************
    // 3) Create CtIDirectMusicLoader object 
    // **********************************************************************	
	hr = dmthCreateLoader(IID_IDirectMusicLoader,&pIDMLoader);
	if(FAILED(hr))
	{
        fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: DirectMusicLoader object creation ")
                TEXT("failed (%s == %08Xh)"),
                tdmXlatHRESULT(hr), hr);
		dwRes = FNS_ABORTED;
		goto TEST_END;
	}

    // **********************************************************************
    // 4) Get the Object from the loader
    // **********************************************************************	
	hr = pIDMLoader->SetSearchDirectory(GUID_DirectMusicAllTypes, wchPath, FALSE);
	if (FAILED(hr))
	{
		fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: Loader->SetSeachDirectory(media directory) ")
				TEXT("failed (%s === %08Xh)"),
				tdmXlatHRESULT(hr), hr);
		dwRes = FNS_ABORTED;
		goto TEST_END;
	}

	desc.guidClass = CLSID_DirectMusicCollection;
	desc.dwValidData |= (DMUS_OBJ_CLASS | DMUS_OBJ_FILENAME );
	MultiByteToWideChar(CP_ACP, NULL, (LPCSTR)"gmts.dls", -1, desc.wszFileName, DMUS_MAX_FILENAME);

	hr = pIDMLoader->GetObject(&desc,CTIID_IDirectMusicCollection,(void**)&pwCollection);
	if(FAILED(hr))
	{
        fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: Loader->GetObject ")
                TEXT("failed (%s == %08Xh)"),
                tdmXlatHRESULT(hr), hr);
		dwRes = FNS_ABORTED;
		goto TEST_END;
	}

    // **********************************************************************
    // 8) Get an instrument collection and release the collection
    // **********************************************************************	
	hr = pwCollection->GetInstrument( 0, &pwInstrument );
	if(FAILED(hr))
	{
        fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: Collection->GetInstrument ")
                TEXT("failed (%s == %08Xh)"),
                tdmXlatHRESULT(hr), hr);
		dwRes = FNS_ABORTED;
		goto TEST_END;
	}

    // **********************************************************************
    // 9) Init(NULL) and AddPort(NULL)
    // **********************************************************************	
	hr = pwPerformance->Init(NULL,NULL,ghWnd); 
	if(FAILED(hr))
	{
        fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: pwPerformance->Init(NULL)")
                TEXT("failed (%s == %08Xh)"),
                tdmXlatHRESULT(hr), hr);
		dwRes = FNS_ABORTED;
		goto TEST_END;
	}

    // set autodownload on
    BOOL fAuto;
    fAuto = TRUE;
    hr = pwPerformance->SetGlobalParam( GUID_PerfAutoDownload, &fAuto, sizeof(BOOL) );
	if(FAILED(hr))
	{
		fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: Performance->SetGlobalParam(AutoDownload == TRUE) ")
				TEXT("failed (%s == %08Xh)"),
				tdmXlatHRESULT(hr), hr);
		dwRes = FNS_ABORTED;
		goto TEST_END;
	}

	hr = pwPerformance->AddPort(NULL); //pCtDM is NULL at this point
	if(FAILED(hr))
	{
        fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: pwPerformance->AddPort(NULL)")
                TEXT("failed (%s == %08Xh)"),
                tdmXlatHRESULT(hr), hr);
		dwRes = FNS_ABORTED;
		goto TEST_END;
	}

    // **********************************************************************
    // 10) Call Download and check results
    // **********************************************************************	
	hr = pwPerformance->DownloadInstrument( pwInstrument, 
											0, 
											&pwDownloaded, 
											&pNoteRanges,
											5,
											&pwPort, 
											&dwGroup,
											&dwMChannel );
	if(FAILED(hr))
	{
        fnsLog(BUGLOGLEVEL, TEXT("!!!! BUG: pwPerformance->DownloadInstrument ")
                TEXT("failed (%s == %08Xh)"),
                tdmXlatHRESULT(hr), hr);
		dwRes = FNS_FAIL;
		goto TEST_END;
	}
	

TEST_END:
    // **********************************************************************
    // 11) Release object, CoUninitialize, and return results
    // **********************************************************************
	if(pwDownloaded)
	{
		if(pwPort)
		{
			hr = pwPort->UnloadInstrument(pwDownloaded);
			if(FAILED(hr))
			{
				fnsLog(BUGLOGLEVEL, 
					"!!!! BUG: pwPort->UnloadInstrument() failed! "
					"(%s == %08Xh)",tdmXlatHRESULT(hr), hr);
				dwRes = FNS_FAIL;
			}
		}
		pwDownloaded->Release();
		pwDownloaded = NULL;
	}
	if(pwPerformance)
	{
		pwPerformance->CloseDown();
		pwPerformance->Release();
        pwPerformance = NULL;
	}
	if( pIDMLoader )
	{
		pIDMLoader->Release();
        pIDMLoader = NULL;
	}
	if( pwCollection )
	{
		pwCollection->Release();
        pwCollection = NULL;
	}
	if( pwPort )
	{
		pwPort->Release();
        pwPort = NULL;
	}
	if( pwInstrument )
	{
		pwInstrument->Release();
        pwInstrument = NULL;
	}

	dmthCoUninitialize();	
	return dwRes;
}
//===========================================================================



//--------------------------------------------------------------------------;
// @doc IDIRECTMUSICPERFORMANCE A-LLUCAR
//
// @topic IDirectMusicPerformance::Invalidate()_Unit_Test |
//
// Description: <nl>
// Unit test function for IDirectMusicPerformance::Invalidate(). <nl>
//
// Test Function: <nl>
// tdmperfInvalidateTest <nl>
//
// Parameters: <nl>
//  BOOL    fCoCreate   - unused <nl>
//  BOOL    fMaxDebug   - unused <nl>
//  DWORD   dwParam1    - unused <nl>
//	DWORD   dwParam2    - unused <nl>
//
// File : TDMPERF.CPP
//
// Steps: <nl>
// 1) Initialize COM. <nl>
// 2) Create CtIDirectMusicPerformance object <nl>
// 3) Init the performance <nl>
// 4) Call Invalidate and check results <nl>
// 5) Release objects, CoUninitialize, and return results <nl>
//
// History: <nl>
//  01/14/1998 - markburt - created <nl>
//  03/23/1998 - a-llucar - clean up, logging installed, autodoc <nl>
//
// @index topic | IDIRECTMUSICPERFORMANCE_CASES
//--------------------------------------------------------------------------;
DWORD tdmperfInvalidateTest(BOOL fCoCreate, BOOL fMaxDebug, 
						   LPARAM dwParam1, LPARAM dwParam2)
{
	DWORD							dwRes			= FNS_PASS;
	HRESULT							hr				= S_OK;
	CtIDirectMusicPerformance		*pwPerformance	= NULL;

    // **********************************************************************
    // 1) Initialize COM. <nl>
    // **********************************************************************
	hr = dmthCoInitialize(NULL);
    if(FAILED(hr))
    {
        fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: COM initialization failed! (%s == %08Xh)"),
                tdmXlatHRESULT(hr), hr);
        return FNS_ABORTED;
    }

    // **********************************************************************
    // 2) Create CtIDirectMusicPerformance object 
    // **********************************************************************
    hr = dmthCreatePerformance(IID_IDirectMusicPerformance, &pwPerformance);
	if(FAILED(hr))
	{
        fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: DirectMusicPerformance object creation ")
                TEXT("failed (%s == %08Xh)"),
                tdmXlatHRESULT(hr), hr);
		dwRes = FNS_ABORTED;
		goto TEST_END;
	}

    // **********************************************************************
    // 3) Init the Performance
    // **********************************************************************
	hr = pwPerformance->Init(NULL,NULL,ghWnd);
	if(FAILED(hr))
	{
		fnsLog(ABORTLOGLEVEL, TEXT("*** ABORT: Performance->Init ")
				TEXT("failed (%s == %08Xh)"),
				tdmXlatHRESULT(hr), hr);
		dwRes = FNS_ABORTED;
		goto TEST_END;
	}

    // set autodownload on
    BOOL fAuto;
    fAuto = TRUE;
    hr = pwPerformance->SetGlobalParam( GUID_PerfAutoDownload, &fAuto, sizeof(BOOL) );
	if(FAILED(hr))
	{
		fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: Performance->SetGlobalParam(AutoDownload == TRUE) ")
				TEXT("failed (%s == %08Xh)"),
				tdmXlatHRESULT(hr), hr);
		dwRes = FNS_ABORTED;
		goto TEST_END;
	}

    // **********************************************************************
    // 3) Call Invalidate and check results
    // **********************************************************************
	hr = pwPerformance->Invalidate( 0, 0 );
	if(FAILED(hr))
	{
        fnsLog(BUGLOGLEVEL, TEXT("!!!! BUG: IDirectMusicPerformance->Invalidate ")
                TEXT("failed (%s == %08Xh)"),
                tdmXlatHRESULT(hr), hr);
		dwRes = FNS_FAIL;
		goto TEST_END;
	}

TEST_END:
    // **********************************************************************
    // 4) Release objects, CoUnInitialize, and return results
    // **********************************************************************
	if(pwPerformance)
	{
		pwPerformance->CloseDown();
		pwPerformance->Release();
        pwPerformance = NULL;
	}

	dmthCoUninitialize();	
	return dwRes;
}
//===========================================================================



//--------------------------------------------------------------------------;
// @doc IDIRECTMUSICPERFORMANCE A-LLUCAR
//
// @topic IDirectMusicPerformance::GetParam()_Unit_Test |
//
// Description: <nl>
// Unit test function for IDirectMusicPerformance::GetParam(). <nl>
//
// Test Function: <nl>
// tdmperfGetParamTest <nl>
//
// Parameters: <nl>
//  BOOL    fCoCreate   - unused <nl>
//  BOOL    fMaxDebug   - unused <nl>
//  DWORD   dwParam1    - unused <nl>
//	DWORD   dwParam2    - unused <nl>
//
// File : TDMPERF.CPP
//
// Steps: <nl>
// 1) Initialize COM. <nl>
// 2) Create CtIDirectMusicPerformance object <nl>
// 3) Get Midi file (call GetMidiFile())  <nl>
// 4) Call AddPort (synth port) and check results <nl>
// 5) Set up a wrapped Segment object <nl>
// 6) Call PlaySegment and check results <nl>
// 7) Get the start time of the segment <nl>
// 8) Call GetParam (get the tempo clock after the start of the segState) <nl>
// 9) Release objects, CoUninitialize, and return results <nl>
//
// History: <nl>
//  01/14/1998 - markburt - created <nl>
//  03/23/1998 - a-llucar - clean up, logging installed, autodoc <nl>
//
// @index topic | IDIRECTMUSICPERFORMANCE_CASES
//--------------------------------------------------------------------------;
DWORD tdmperfGetParamTest(BOOL fCoCreate, BOOL fMaxDebug, 
						   LPARAM dwParam1, LPARAM dwParam2)
{
	DWORD						dwRes			= FNS_PASS;
	HRESULT						hr				= S_OK;	
	CtIDirectMusicSegment*		pwSegment		= NULL;
	CtIDirectMusicPerformance*	pwPerformance	= NULL;
	MUSIC_TIME					pmtNext			= NULL;
	MUSIC_TIME					mt				= NULL;
	GUID						guid			= GUID_TempoParam;
	DMUS_TEMPO_PARAM			tempo;

	int							tempoexpected	= (int)dwParam1;
	WCHAR*						pszMidiFile		= (WCHAR*)dwParam2;


    // **********************************************************************
    // 1) Initialize COM
    // **********************************************************************
    hr = dmthCoInitialize(NULL);
    if(FAILED(hr))
    {
        fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT:  initialization failed! (%08Xh == %s)"),
					tdmXlatHRESULT(hr), hr);
        return FNS_ABORTED;
    }

    // **********************************************************************
    // 2) Create CtIDirectMusicPerformance object 
    // **********************************************************************
    hr = dmthCreatePerformance(IID_IDirectMusicPerformance, &pwPerformance);
	if(FAILED(hr))
	{
        fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: DirectMusicPerformance object creation ")
                TEXT("failed (%s == %08Xh)"),
                tdmXlatHRESULT(hr), hr);
		dwRes = FNS_ABORTED;
		goto TEST_END;
	}

    // **********************************************************************
    // 3) Call PlayMidiFile() convienence function
    // **********************************************************************
	hr = dmthPlayMidiFile(pwPerformance,CTIID_IDirectMusicSegment,(CtIDirectMusicSegment**)&pwSegment,(CtIDirectMusicSegmentState**)NULL, TRUE, pszMidiFile);
	if(FAILED(hr))
	{
        fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: PlayMidiFile convienence function "),
                TEXT("failed (%s == %08Xh)"),
                tdmXlatHRESULT(hr), hr);
		dwRes = FNS_ABORTED;
		goto TEST_END;
	}

    // **********************************************************************
    // 4) Call GetTime() to get the Start Time and check results
    // **********************************************************************
	hr = pwPerformance->GetTime( NULL, &mt );
	if(FAILED(hr))
	{
        fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: IDirectMusicPerformance->GetTime ")
                TEXT("failed (%s == %08Xh)"),
                tdmXlatHRESULT(hr), hr);
		dwRes = FNS_ABORTED;
		goto TEST_END;
	}
	
	// Check to make sure that mt is something other than 0
	if(!mt)
	{
        fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: IDirectMusicPerformance->GetTime ")
                TEXT("failed (MUSIC_TIME mt==%ul, expected Non-Zero.)"),
				mt);
		dwRes = FNS_ABORTED;
		goto TEST_END;
	}

	// add 3 measures of 4/4
	// this is necessary when using onestop2.mid
	// because there are tempo changes at the beginning of the file
	// The tempo near measure 4 will be found
	mt += (768 * 12);

    // **********************************************************************
    // 5) Call GetParam (get the tempo clock after the start of the segState)
    // **********************************************************************
	hr = pwPerformance->GetParam( guid, 0xffffffff, 0, mt, &pmtNext, (void*)&tempo ); 
	if( FAILED(hr))
	{
        fnsLog(BUGLOGLEVEL, TEXT("!!!! BUG: IDirectMusicPerformance->GetParam ")
                TEXT("failed (%s == %08Xh)"),
                tdmXlatHRESULT(hr), hr);
		dwRes = FNS_FAIL;
		goto TEST_END;
	}

	// Make sure that it get's the right tempo
	// tempoexpected is DEPENDENT ON THE MIDI FILE!!!!!
	if ((int)tempo.dblTempo != tempoexpected)
	{
        fnsLog(BUGLOGLEVEL, TEXT("!!!! BUG: IDirectMusicPerformance->GetParam ")
                TEXT("tempo.dblTempo did not return correctly\n")
                TEXT("Expected: tempo.dblTempo == %d\n")
                TEXT("Returned: tempo.dblTempo == %d\n"),
                tempoexpected,(int)tempo.dblTempo);
		dwRes = FNS_FAIL;
		goto TEST_END;
	}

	// **********************************************************************
    // 6) Stop the segment, check the results 
    // **********************************************************************

	// wait another half second - for absolutely no good reason
	Sleep(500);

	hr = pwPerformance->Stop(NULL, NULL, 0, 0);
	if(FAILED(hr))
	{
		fnsLog(FYILOGLEVEL, TEXT("**** FYI: Performance->Stop")
				TEXT("failed (%s == %08Xh)"),
				tdmXlatHRESULT(hr), hr);
	}

TEST_END:
    // **********************************************************************
    // 7) Release objects, CoUninitialize, and return results
    // **********************************************************************
	if(pwPerformance)
	{
		pwPerformance->CloseDown();
		pwPerformance->Release();
        pwPerformance = NULL;
	}
	if( pwSegment )
	{
		pwSegment->Release();
        pwSegment = NULL;
	}

	dmthCoUninitialize();	
	return dwRes;
}
//===========================================================================




//--------------------------------------------------------------------------;
// @doc IDIRECTMUSICPERFORMANCE A-LLUCAR
//
// @topic IDirectMusicPerformance::SetParam()_Unit_Test |
//
// Description: <nl>
// Unit test function for IDirectMusicPerformance::SetParam(). <nl>
//
// Test Function: <nl>
// tdmperfSetParamTest <nl>
//
// Parameters: <nl>
//  BOOL    fCoCreate   - unused <nl>
//  BOOL    fMaxDebug   - unused <nl>
//  DWORD   dwParam1    - unused <nl>
//	DWORD   dwParam2    - unused <nl>
//
// File : TDMPERF.CPP
//
// Steps: <nl>
// 1) Initialize COM. <nl>
// 2) Create CtIDirectMusicPerformance object <nl>
// 3) Create CtIDirectMusicLoader object (wrapped IDirectMusicLoader object)  <nl>
// 4) Loader->EnableCache(GUID_DirectMusicAllTypes) <nl>
// 5) Load the Style Object <nl>
// 6) Load the ChordMap Object <nl>
// 7) Get the segment by calling ComposeSegmentFromShape <nl>
// 8) Create CtIDirectMusicPerformance object (wrapped IDirectMusicPerformance object) <nl>
// 9) Init the Performance <nl>
// 10) Add a port to the Performance (Synth port) <nl>
// 11) Activate the Performance <nl>
// 12) Play the Segment, check the results <nl>
// 13) Wait for IsPlaying to return favorably (or time out) <nl>
// 14) Get the start time of the segment, check results <nl>
// 15) Call SetParam, check the results <nl>
// 16) Stop the segment, check the results  <nl>
// 17) Cleanup objects and uninitialize COM and Return results <nl>
//
// History: <nl>
//  01/14/1998 - markburt - created <nl>
//  03/23/1998 - a-llucar - clean up, logging installed, autodoc <nl>
//
// @index topic | IDIRECTMUSICPERFORMANCE_CASES
//--------------------------------------------------------------------------;
DWORD tdmperfSetParamTest(BOOL fCoCreate, BOOL fMaxDebug, 
						   LPARAM dwParam1, LPARAM dwParam2)
{
	DWORD						dwRes			= FNS_PASS;
	HRESULT						hr				= S_OK;
	CtIDirectMusicPerformance	*pwPerformance	= NULL;
	CtIDirectMusicSegment		*pwSegment		= NULL;
	MUSIC_TIME					mt				= NULL;
	GUID						guid			= GUID_TempoParam;
	MUSIC_TIME					pmtNext			= NULL;
	DMUS_TEMPO_PARAM			tempo;

    // **********************************************************************
    // 1) Initialize COM
    // **********************************************************************
    hr = dmthCoInitialize(NULL);
    if(FAILED(hr))
    {
        fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT:  initialization failed! (%08Xh == %s)"),
					tdmXlatHRESULT(hr), hr);
        return FNS_ABORTED;
    }

    // **********************************************************************
    // 2) Create CtIDirectMusicPerformance object 
    // **********************************************************************
    hr = dmthCreatePerformance(IID_IDirectMusicPerformance, &pwPerformance);
	if(FAILED(hr))
	{
        fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: DirectMusicPerformance object creation ")
                TEXT("failed (%s == %08Xh)"),
                tdmXlatHRESULT(hr), hr);
		dwRes = FNS_ABORTED;
		goto TEST_END;
	}

    // **********************************************************************
    // 3) Call PlayMidiFile() convienence function
    // **********************************************************************
	hr = dmthPlayMidiFile(pwPerformance,CTIID_IDirectMusicSegment,(CtIDirectMusicSegment**)&pwSegment,(CtIDirectMusicSegmentState**)NULL, TRUE, NULL );
	if(FAILED(hr))
	{
        fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: PlayMidiFile convienence function "),
                TEXT("failed (%s == %08Xh)"),
                tdmXlatHRESULT(hr), hr);
		dwRes = FNS_ABORTED;
		goto TEST_END;
	}

    // **********************************************************************
    // 4) Call GetTime() to get the Start Time and check results
    // **********************************************************************
	hr = pwPerformance->GetTime( NULL, &mt );
	if(FAILED(hr))
	{
        fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: IDirectMusicPerformance->GetTime ")
                TEXT("failed (%s == %08Xh)"),
                tdmXlatHRESULT(hr), hr);
		dwRes = FNS_ABORTED;
		goto TEST_END;
	}
	
	// Check to make sure that mt is something other than 0
	if(!mt)
	{
        fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: IDirectMusicPerformance->GetTime ")
                TEXT("failed (MUSIC_TIME mt==%ul, expected Non-Zero.)"),
				mt);
		dwRes = FNS_ABORTED;
		goto TEST_END;
	}

	// **********************************************************************
    // 5) Call SetParam, check the results 
    // **********************************************************************
	hr = pwPerformance->SetParam( guid, 0xffffffff, 0, mt, (void*)&tempo ); 
	if(FAILED(hr))
	{
		fnsLog(BUGLOGLEVEL, TEXT("!!!! BUG: IDirectMusicPerformance->SetParam ")
				TEXT("failed (%s == %08Xh)"),
				tdmXlatHRESULT(hr), hr);
		dwRes = FNS_FAIL;
		goto TEST_END;
	}   

	// **********************************************************************
    // 6) Stop the segment, check the results 
    // **********************************************************************
	Sleep(3000);
	hr = pwPerformance->Stop(NULL, NULL, 0, 0);
	if(FAILED(hr))
	{
		fnsLog(FYILOGLEVEL, TEXT("**** FYI: Performance->Stop")
				TEXT("failed (%s == %08Xh)"),
				tdmXlatHRESULT(hr), hr);
	}

TEST_END:
    // **********************************************************************
    // 7) Cleanup objects and uninitialize COM and Return results
    // **********************************************************************
	if (pwPerformance)
	{
		pwPerformance->CloseDown();
		pwPerformance->Release();
        pwPerformance = NULL;
	}
	if (pwSegment)
	{
		pwSegment->Release();
        pwSegment = NULL;
	}

	dmthCoUninitialize();	

	return dwRes;
}	// End SetParam() Test

//--------------------------------------------------------------------------;
// @doc IDIRECTMUSICPERFORMANCE A-LLUCAR
//
// @topic IDirectMusicPerformance::GetGlobalParam()_Unit_Test |
//
// Description: <nl>
// Unit test function for IDirectMusicPerformance::GetGlobalParam(). <nl>
//
// Test Function: <nl>
// tdmperfGetGlobalParamTest <nl>
//
// Parameters: <nl>
//  BOOL    fCoCreate   - unused <nl>
//  BOOL    fMaxDebug   - unused <nl>
//  DWORD   dwParam1    - unused <nl>
//	DWORD   dwParam2    - unused <nl>
//
// File : TDMPERF.CPP
//
// Steps: <nl>
// 1) Initialize COM. <nl>
// 2) Create CtIDirectMusicPerformance object <nl>
// 3) Call SetGlobalParam, check results<nl>
// 4) Call GetGlobalParam, check results (make sure dw and dw2 are equal) <nl>
// 5) Release objects, CoUninitialize, and return results <nl>
//
// History: <nl>
//  01/14/1998 - markburt - created <nl>
//  03/23/1998 - a-llucar - clean up, logging installed, autodoc <nl>
//
//	Note - GetGlobalParam() and SetGlobalParam() both use the same test.
//
// @index topic | IDIRECTMUSICPERFORMANCE_CASES
//--------------------------------------------------------------------------;
DWORD tdmperfGetGlobalParamTest(BOOL fCoCreate, BOOL fMaxDebug, 
						   LPARAM dwParam1, LPARAM dwParam2)
{
	DWORD						dwRes			= FNS_PASS;
	HRESULT						hr				= S_OK;
	GUID						guid			= CLSID_DirectMusicPerformance; // use this for a global data guid. arbitrary.;
	DWORD						dw				= 53;
	DWORD						dw2				= 0;
	CtIDirectMusicPerformance	*pwPerformance	= NULL;

    // **********************************************************************
    // 1) Initialize COM. <nl>
    // **********************************************************************
	hr = dmthCoInitialize(NULL);
    if(FAILED(hr))
    {
        fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: COM initialization failed! (%s == %08Xh)"),
                tdmXlatHRESULT(hr), hr);
        return FNS_ABORTED;
    }

    // **********************************************************************
    // 2) Create CtIDirectMusicPerformance object 
    // **********************************************************************
    hr = dmthCreatePerformance(IID_IDirectMusicPerformance, &pwPerformance);
	if(FAILED(hr))
	{
        fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: DirectMusicPerformance object creation ")
                TEXT("failed (%s == %08Xh)"),
                tdmXlatHRESULT(hr), hr);
		dwRes = FNS_ABORTED;
		goto TEST_END;
	}

    // **********************************************************************
    // 3) Call SetGlobalParam, check results
    // **********************************************************************
	hr = pwPerformance->SetGlobalParam( guid, (void*)&dw, sizeof(DWORD)); 
	if(FAILED(hr))
	{
        fnsLog(BUGLOGLEVEL, TEXT("!!!! BUG: IDirectMusicPerformance->SetGlobalParam ")
                TEXT("failed (%s == %08Xh)"),
                tdmXlatHRESULT(hr), hr);
		dwRes = FNS_FAIL;
		goto TEST_END;
	}

    // **********************************************************************
    // 4) Call GetGlobalParam, check results
    // **********************************************************************
	hr = pwPerformance->GetGlobalParam( guid, (void*)&dw2, sizeof(DWORD));
	if(FAILED(hr))
	{
        fnsLog(BUGLOGLEVEL, TEXT("!!!! BUG: IDirectMusicPerformance->GetGlobalParam ")
                TEXT("failed (%s == %08Xh)"),
                tdmXlatHRESULT(hr), hr);
		dwRes = FNS_FAIL;
		goto TEST_END;
	}
		
	// Check to make sure that the DWORD was properly populated
	if(dw != dw2)
	{
        fnsLog(BUGLOGLEVEL, TEXT("!!!! BUG: IDirectMusicPerformance->GetGlobalParam\n ")
                TEXT("pData was not properly populated\n")
                TEXT("Expected -- dw == %ld\n")
				TEXT("Received -- dw2 == %ld\n"),
				dw, dw2);
		dwRes = FNS_FAIL;
		goto TEST_END;
	}
	
TEST_END:
	// Clean up the wrapper
	if(pwPerformance)
	{
		pwPerformance->CloseDown();
		pwPerformance->Release();
        pwPerformance = NULL;
	}

	dmthCoUninitialize();	
	return dwRes;
}
//===========================================================================



//--------------------------------------------------------------------------;
// @doc IDIRECTMUSICPERFORMANCE A-LLUCAR
//
// @topic IDirectMusicPerformance::SetGlobalParam()_Unit_Test |
//
// Description: <nl>
// Unit test function for IDirectMusicPerformance::SetGlobalParam(). <nl>
//
// Test Function: <nl>
// tdmperfGetGlobalParamTest <nl>
//
// Parameters: <nl>
//  BOOL    fCoCreate   - unused <nl>
//  BOOL    fMaxDebug   - unused <nl>
//  DWORD   dwParam1    - unused <nl>
//	DWORD   dwParam2    - unused <nl>
//
// File : TDMPERF.CPP
//
// Steps: <nl>
// 1) Initialize COM. <nl>
// 2) Create CtIDirectMusicPerformance object <nl>
// 3) Call SetGlobalParam, check results<nl>
// 4) Call GetGlobalParam, check results (make sure dw and dw2 are equal) <nl>
// 5) Release objects, CoUninitialize, and return results <nl>
//
// History: <nl>
//  01/14/1998 - markburt - created <nl>
//  03/23/1998 - a-llucar - clean up, logging installed, autodoc <nl>
//
//	Note - GetGlobalParam() and SetGlobalParam() both use the same test.
//
// @index topic | IDIRECTMUSICPERFORMANCE_CASES
//--------------------------------------------------------------------------;
//!!!!!!  Test case removed (now uses GetGlobalParam() test) !!!!!!!!!!!!!!!


//--------------------------------------------------------------------------;
// @doc IDIRECTMUSICPERFORMANCE A-LLUCAR
//
// @topic IDirectMusicPerformance::GetLatencyTime()_Unit_Test |
//
// Description: <nl>
// Unit test function for IDirectMusicPerformance::GetLatencyTime(). <nl>
//
// Test Function: <nl>
// tdmperfGetLatencyTimeTest <nl>
//
// Parameters: <nl>
//  BOOL    fCoCreate   - unused <nl>
//  BOOL    fMaxDebug   - unused <nl>
//  DWORD   dwParam1    - unused <nl>
//	DWORD   dwParam2    - unused <nl>
//
// File : TDMPERF.CPP
//
// Steps: <nl>
// 1) Initialize COM. <nl>
// 2) Create CtIDirectMusicPerformance object <nl>
// 3) Init the Performance <nl>
// 4) Call GetLatencyTime, check results <nl>
// 5) Release objects, CoUninitialize, and return results <nl>
//
// History: <nl>
//  01/14/1998 - markburt - created <nl>
//  03/23/1998 - a-llucar - clean up, logging installed, autodoc <nl>
//
//	Note - GetGlobalParam() and SetGlobalParam() both use the same test.
//
// @index topic | IDIRECTMUSICPERFORMANCE_CASES
//--------------------------------------------------------------------------;
DWORD tdmperfGetLatencyTimeTest(BOOL fCoCreate, BOOL fMaxDebug, 
						   LPARAM dwParam1, LPARAM dwParam2)
{
	DWORD						dwRes			= FNS_PASS;
	HRESULT						hr				= S_OK;
	REFERENCE_TIME				rt				= 0;
	CtIDirectMusicPerformance	*pwPerformance	= NULL;

    // **********************************************************************
    // 1) Initialize COM. <nl>
    // **********************************************************************
	hr = dmthCoInitialize(NULL);
    if(FAILED(hr))
    {
        fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: COM initialization failed! (%s == %08Xh)"),
                tdmXlatHRESULT(hr), hr);
        return FNS_ABORTED;
    }

    // **********************************************************************
    // 2) Create CtIDirectMusicPerformance object 
    // **********************************************************************
    hr = dmthCreatePerformance(IID_IDirectMusicPerformance, &pwPerformance);
	if(FAILED(hr))
	{
        fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: DirectMusicPerformance object creation ")
                TEXT("failed (%s == %08Xh)"),
                tdmXlatHRESULT(hr), hr);
		dwRes = FNS_ABORTED;
		goto TEST_END;
	}

    // **********************************************************************
    // 3) Init the Performance
    // **********************************************************************
	hr = pwPerformance->Init(NULL,NULL,ghWnd);
	if(FAILED(hr))
	{
		fnsLog(ABORTLOGLEVEL, TEXT("*** ABORT: Performance->Init ")
				TEXT("failed (%s == %08Xh)"),
				tdmXlatHRESULT(hr), hr);
		dwRes = FNS_ABORTED;
		goto TEST_END;
	}

    // set autodownload on
    BOOL fAuto;
    fAuto = TRUE;
    hr = pwPerformance->SetGlobalParam( GUID_PerfAutoDownload, &fAuto, sizeof(BOOL) );
	if(FAILED(hr))
	{
		fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: Performance->SetGlobalParam(AutoDownload == TRUE) ")
				TEXT("failed (%s == %08Xh)"),
				tdmXlatHRESULT(hr), hr);
		dwRes = FNS_ABORTED;
		goto TEST_END;
	}

    // **********************************************************************
    // 4) Call GetLatencyTime, check results
    // **********************************************************************
	hr = pwPerformance->GetLatencyTime(&rt);
	if(FAILED(hr))
	{
        fnsLog(BUGLOGLEVEL, TEXT("!!!! BUG: IDirectMusicPerformance->GetLatencyTime ")
                TEXT("failed (%s == %08Xh)"),
                tdmXlatHRESULT(hr), hr);
		dwRes = FNS_FAIL;
		goto TEST_END;
	}
		
    //  Check to make sure that REFERENCE_TIME pointer was populated
	if(!rt)
	{
        fnsLog(BUGLOGLEVEL, TEXT("!!!! BUG: IDirectMusicPerformance->GetLatencyTime ")
                TEXT("rt was not set correctly (rt == %08Xh)"), rt);
		dwRes = FNS_FAIL;
		goto TEST_END;
	}

TEST_END:
    // **********************************************************************
    // 5) Release objects, CoUnitialize, Return results
    // **********************************************************************
	if(pwPerformance)
	{
		pwPerformance->CloseDown();
		pwPerformance->Release();
        pwPerformance = NULL;
	}

	dmthCoUninitialize();

	return dwRes;
}
//===========================================================================


//--------------------------------------------------------------------------;
// @doc IDIRECTMUSICPERFORMANCE A-LLUCAR
//
// @topic IDirectMusicPerformance::GetQueueTime()_Unit_Test |
//
// Description: <nl>
// Unit test function for IDirectMusicPerformance::GetQueueTime(). <nl>
//
// Test Function: <nl>
// tdmperfGetQueueTimeTest <nl>
//
// Parameters: <nl>
//  BOOL    fCoCreate   - unused <nl>
//  BOOL    fMaxDebug   - unused <nl>
//  DWORD   dwParam1    - unused <nl>
//	DWORD   dwParam2    - unused <nl>
//
// File : TDMPERF.CPP
//
// Steps: <nl>
// 1) Initialize COM. <nl>
// 2) Create CtIDirectMusicComposer object (wrapped IDMComposer object) <nl>
// 3) Initialize the performance <nl>
// 4) call GetQueueTime and check results <nl>
// 5) Release objects, CoUninitialize, and return results <nl>
//
// History: <nl>
//  01/14/1998 - markburt - created <nl>
//  03/23/1998 - a-llucar - clean up, logging installed, autodoc <nl>
//
// @index topic | IDIRECTMUSICPERFORMANCE_CASES
//--------------------------------------------------------------------------;
DWORD tdmperfGetQueueTimeTest(BOOL fCoCreate, BOOL fMaxDebug, 
						   LPARAM dwParam1, LPARAM dwParam2)
{
	DWORD							dwRes			= FNS_PASS;
	HRESULT							hr				= S_OK;
	REFERENCE_TIME					rt              = 0;
	CtIDirectMusicPerformance		*pwPerformance	= NULL;

    // **********************************************************************
    // 1) Initialize COM. <nl>
    // **********************************************************************
	hr = dmthCoInitialize(NULL);
    if(FAILED(hr))
    {
        fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: COM initialization failed! (%s == %08Xh)"),
                tdmXlatHRESULT(hr), hr);
        return FNS_ABORTED;
    }

    // **********************************************************************
    // 2) Create CtIDirectMusicPerformance object 
    // **********************************************************************
    hr = dmthCreatePerformance(IID_IDirectMusicPerformance, &pwPerformance);
	if(FAILED(hr))
	{
        fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: DirectMusicPerformance object creation ")
                TEXT("failed (%s == %08Xh)"),
                tdmXlatHRESULT(hr), hr);
		dwRes = FNS_ABORTED;
		goto TEST_END;
	}

    // **********************************************************************
    // 3) Init the Performance
    // **********************************************************************
	hr = pwPerformance->Init(NULL,NULL,ghWnd);
	if(FAILED(hr))
	{
		fnsLog(ABORTLOGLEVEL, TEXT("*** ABORT: Performance->Init ")
				TEXT("failed (%s == %08Xh)"),
				tdmXlatHRESULT(hr), hr);
		dwRes = FNS_ABORTED;
		goto TEST_END;
	}

    // set autodownload on
    BOOL fAuto;
    fAuto = TRUE;
    hr = pwPerformance->SetGlobalParam( GUID_PerfAutoDownload, &fAuto, sizeof(BOOL) );
	if(FAILED(hr))
	{
		fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: Performance->SetGlobalParam(AutoDownload == TRUE) ")
				TEXT("failed (%s == %08Xh)"),
				tdmXlatHRESULT(hr), hr);
		dwRes = FNS_ABORTED;
		goto TEST_END;
	}

    // **********************************************************************
    // 4) Call GetQueueTime and check results
    // **********************************************************************
	hr = pwPerformance->GetQueueTime(&rt);
	if(FAILED(hr))
	{
        fnsLog(BUGLOGLEVEL, TEXT("!!!! BUG: IDirectMusicPerformance->GetQueueTime ")
                TEXT("failed (%s == %08Xh)"),
                tdmXlatHRESULT(hr), hr);
		dwRes = FNS_FAIL;
		goto TEST_END;
	}

    //  Check to make sure that REFERENCE_TIME pointer was populated
	if(!rt)
	{
        fnsLog(BUGLOGLEVEL, TEXT("!!!! BUG: IDirectMusicPerformance->GetQueueTime ")
                TEXT("rt was not set correctly (rt == %08Xh)"), rt);
		dwRes = FNS_FAIL;
		goto TEST_END;
	}
		
TEST_END:
    // **********************************************************************
    // 5) Release objects, CoUninitialize, and return results
    // **********************************************************************
	if(pwPerformance)
	{
		pwPerformance->CloseDown();
		pwPerformance->Release();
        pwPerformance = NULL;
	}

	dmthCoUninitialize();	

	return dwRes;
}
//===========================================================================


//--------------------------------------------------------------------------;
// @doc IDIRECTMUSICPERFORMANCE A-LLUCAR
//
// @topic IDirectMusicPerformance::AdjustTime()_Unit_Test |
//
// Description: <nl>
// Unit test function for IDirectMusicPerformance::AdjustTime(). <nl>
//
// Test Function: <nl>
// tdmperfAdjustTimeTest <nl>
//
// Parameters: <nl>
//  BOOL    fCoCreate   - unused <nl>
//  BOOL    fMaxDebug   - unused <nl>
//  DWORD   dwParam1    - unused <nl>
//	DWORD   dwParam2    - unused <nl>
//
// File : TDMPERF.CPP
//
// Steps: <nl>
// 1) Initialize COM. <nl>
// 2) Create CtIDirectMusicPerformance object <nl>
// 3) Init the performance <nl>
// 4) Call Invalidate and check results <nl>
// 5) Release objects, CoUninitialize, and return results <nl>
//
// History: <nl>
//  03/23/1998 - a-llucar - Created <nl>
//
// @index topic | IDIRECTMUSICPERFORMANCE_CASES
//--------------------------------------------------------------------------;
DWORD tdmperfAdjustTimeTest(BOOL fCoCreate, BOOL fMaxDebug, 
						   LPARAM dwParam1, LPARAM dwParam2)
{
	DWORD							dwRes			= FNS_PASS;
	HRESULT							hr				= S_OK;
	CtIDirectMusicPerformance		*pwPerformance	= NULL;

    // **********************************************************************
    // 1) Initialize COM. <nl>
    // **********************************************************************
	hr = dmthCoInitialize(NULL);
    if(FAILED(hr))
    {
        fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: COM initialization failed! (%s == %08Xh)"),
                tdmXlatHRESULT(hr), hr);
        return FNS_ABORTED;
    }

    // **********************************************************************
    // 2) Create CtIDirectMusicPerformance object 
    // **********************************************************************
    hr = dmthCreatePerformance(IID_IDirectMusicPerformance, &pwPerformance);
	if(FAILED(hr))
	{
        fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: DirectMusicPerformance object creation ")
                TEXT("failed (%s == %08Xh)"),
                tdmXlatHRESULT(hr), hr);
		dwRes = FNS_ABORTED;
		goto TEST_END;
	}

    // **********************************************************************
    // 3) Call AdjustTime and check results
    // **********************************************************************
	hr = pwPerformance->AdjustTime( 1000 );
	if(FAILED(hr))
	{
        fnsLog(BUGLOGLEVEL, TEXT("!!!! BUG: IDirectMusicPerformance->AdjustTime ")
                TEXT("failed (%s == %08Xh)"),
                tdmXlatHRESULT(hr), hr);
		dwRes = FNS_FAIL;
		goto TEST_END;
	}

TEST_END:
    // **********************************************************************
    // 4) Release objects, dmthCoUnInitialize, and return results
    // **********************************************************************
	// Clean up the wrapper
	if(pwPerformance)
	{
		pwPerformance->Release();
        pwPerformance = NULL;
	}

	dmthCoUninitialize();

	return dwRes;
}
//===========================================================================


//--------------------------------------------------------------------------;
// @doc IDIRECTMUSICPERFORMANCE A-LLUCAR
//
// @topic IDirectMusicPerformance::CloseDown()_Unit_Test |
//
// Description: <nl>
// Unit test function for IDirectMusicPerformance::CloseDown().
//
// Test Function: <nl>
// tdmperfCloseDownTest <nl>
//
// Parameters: <nl>
//  BOOL    fCoCreate   - unused <nl>
//  BOOL    fMaxDebug   - unused <nl>
//  DWORD   dwParam1    - unused <nl>
//	DWORD   dwParam2    - unused <nl>
//
// File : TDMPERF.CPP
//
// Steps: <nl>
// 1) Initialize COM. <nl>
// 2) Create CtIDirectMusicPerformance object <nl>
// 3) Init the Performance <nl>
// 4) Call CloseDown() and check results <nl>
// 5) Release objects, CoUninitialize, and return results <nl>
//
// History: <nl>
//  01/17/1998 - markburt - created <nl>
//  03/23/1998 - a-llucar - clean up, logging installed, autodoc <nl>
// @index topic | IDIRECTMUSICPERFORMANCE_CASES
//--------------------------------------------------------------------------;
DWORD tdmperfCloseDownTest(BOOL fCoCreate, BOOL fMaxDebug, 
						   LPARAM dwParam1, LPARAM dwParam2)
{
	DWORD						dwRes			= FNS_PASS;	
	HRESULT						hr				= S_OK;
	CtIDirectMusicPerformance	*pwPerformance	= NULL;

    // **********************************************************************
    // 1) Initialize COM. <nl>
    // **********************************************************************
	hr = dmthCoInitialize(NULL);
    if(FAILED(hr))
    {
        fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: COM initialization failed! (%s == %08Xh)"),
                tdmXlatHRESULT(hr), hr);
        return FNS_ABORTED;
    }

    // **********************************************************************
    // 2) Create CtIDirectMusicPerformance object 
    // **********************************************************************
    hr = dmthCreatePerformance(IID_IDirectMusicPerformance, &pwPerformance);
	if(FAILED(hr))
	{
        fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: DirectMusicSegment object creation "),
                TEXT("failed (%s == %08Xh)"),
                tdmXlatHRESULT(hr), hr);
		dwRes = FNS_ABORTED;
		goto TEST_END;
	}

    // **********************************************************************
    // 3) Init the Performance
    // **********************************************************************
	hr = pwPerformance->Init(NULL,NULL,ghWnd);
	if(FAILED(hr))
	{
        fnsLog(ABORTLOGLEVEL, TEXT("!!!! BUG: IDirectMusicPerformance->Init ")
                TEXT("failed (%s == %08Xh)"),
                tdmXlatHRESULT(hr), hr);
		dwRes = FNS_ABORTED;
		goto TEST_END;
	}

    // set autodownload on
    BOOL fAuto;
    fAuto = TRUE;
    hr = pwPerformance->SetGlobalParam( GUID_PerfAutoDownload, &fAuto, sizeof(BOOL) );
	if(FAILED(hr))
	{
		fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: Performance->SetGlobalParam(AutoDownload == TRUE) ")
				TEXT("failed (%s == %08Xh)"),
				tdmXlatHRESULT(hr), hr);
		dwRes = FNS_ABORTED;
		goto TEST_END;
	}

    // **********************************************************************
    // 4) Call Closedown() and check results
    // **********************************************************************
	hr = pwPerformance->CloseDown();
	if(FAILED(hr))     
	{
        fnsLog(BUGLOGLEVEL, TEXT("!!!! BUG: IDirectMusicPerformance->CloseDown ")
                TEXT("failed (%s == %08Xh)"),
                tdmXlatHRESULT(hr), hr);
		dwRes = FNS_FAIL;
		goto TEST_END;
	}

TEST_END:
    // **********************************************************************
    // 5) Release objects, CoUninitialize, and return result
    // **********************************************************************
	// Clean up the wrapper
	if(pwPerformance)
	{
		pwPerformance->Release();
        pwPerformance = NULL;
	}
	
	dmthCoUninitialize();	

	return dwRes;
}	// End IDirectMusicPerformance->CloseDown() Test


//--------------------------------------------------------------------------;
// @doc IDIRECTMUSICPERFORMANCE A-LLUCAR
//
// @topic IDirectMusicComposer::QueryInterface()_Unit_Test |
//
// Description: <nl>
// Unit test function for IDirectMusicPerformance::QueryInterface().  QI's for
// IID_IDirectMusicPerformance.
//
// Test Function: <nl>
// tdmperfQueryInterfaceTest
//
// Parameters: <nl>
//  BOOL    fCoCreate   - unused <nl>
//  BOOL    fMaxDebug   - unused <nl>
//  DWORD   dwParam1    - unused <nl>
//	DWORD   dwParam2    - unused <nl>
//
// File : TDMCOMP.CPP
//
// Steps: <nl>
// 1) Initialize COM. <nl>
// 2) Create CtIDirectMusicComposer object (wrapped version of IDirectMusicComposer) <nl>
// 3) Call CtIDirectMusicComposer::QueryiInterface() to request an IID_IDirectMusicComposer 
// inteface object. <nl>
// 4) Cleanup objects, uninitialize COM, return test results <nl>
//
// History:
//  03/26/1998 - a-llucar - created <nl>
//
// @index topic | IDIRECTMUSICPERFORMANCE_CASES
//--------------------------------------------------------------------------;
DWORD tdmperfQueryInterfaceTest(BOOL fCoCreate, BOOL fMaxDebug, LPARAM dwParam1, 
						  LPARAM dwParam2)
{
	HRESULT						hRes				= E_NOTIMPL;
	DWORD						dwRes				= FNS_PASS;
	CtIDirectMusicPerformance	*pwPerformance      = NULL;
	IDirectMusicPerformance		*pPerformance		= NULL;

    // **********************************************************************
    // 1) Initialize COM.
    // **********************************************************************
    hRes = dmthCoInitialize(NULL);
    if(FAILED(hRes))
    {
        fnsLog(ABORTLOGLEVEL, TEXT("**** COM initialization failed! (%s == %08Xh)"),
                tdmXlatHRESULT(hRes), hRes);
        fnsLog(ABORTLOGLEVEL, TEXT("**** Test case aborting."));
        return FNS_ABORTED;
    }

    // **********************************************************************
    // 2) Create CtIDirectMusic object (wrapped version of IDirectMusic)
    // **********************************************************************
    hRes = dmthCreatePerformance(IID_IDirectMusicPerformance, &pwPerformance);
    if(FAILED(hRes))
    {
        fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: Base DirectMusicPerformance object creation ")
                TEXT("failed (%s == %08Xh)"),
                tdmXlatHRESULT(hRes), hRes);
        dwRes = FNS_ABORTED;
        goto TEST_END;
    }

    // **********************************************************************
    // 3) Call CtIDirectMusic::QueryiInterface() to request an IID_IDirectMusic 
    // inteface object.
    // **********************************************************************
    hRes = pwPerformance->QueryInterface(IID_IDirectMusicPerformance, (void**)&pPerformance);
    if(FAILED(hRes))
    {
        fnsLog(BUGLOGLEVEL, TEXT("!!!! BUG: QueryInterface(IID_IDirectMusicPerformance) failed! ")
                TEXT("(%s == %08Xh)"),
                tdmXlatHRESULT(hRes), hRes);
        dwRes = FNS_FAIL;
        goto TEST_END;
    }

TEST_END:
    // **********************************************************************
    // 4) Cleanup objects, uninitialize COM, Return test result
    // **********************************************************************
    if(pwPerformance)
    {
        pwPerformance->Release();
        pwPerformance = NULL;
    }
    if(pPerformance)
    {
        pPerformance->Release();
        pPerformance = NULL;
    }

    dmthCoUninitialize();

    return dwRes;

} 
 
//--------------------------------------------------------------------------;
// @doc IDIRECTMUSICPERFORMANCE A-LLUCAR
//
// @topic IDirectMusicPerformance::GetResolvedTime()_Unit_Test |
//
// Description: <nl>
// Unit test function for IDirectMusicPerformance::GetResolvedTime().
//
// Test Function: <nl>
// tdmperfGetResolvedTimeTest <nl>
//
// Parameters: <nl>
//  BOOL    fCoCreate   - unused <nl>
//  BOOL    fMaxDebug   - unused <nl>
//  DWORD   dwParam1    - unused <nl>
//	DWORD   dwParam2    - unused <nl>
//
// File : TDMPERF.CPP
//
// Steps: <nl>
// 1) Initialize COM. <nl>
// 2) Create CtIDirectMusicPerformance object <nl>
// 3) Load a segment <nl>
// 4) PrePlay <nl>
// 5) Call GetResolvedTime and check results <nl>
// 6) Cleanup objects and uninitialize COM and Return results <nl>
//
// History: <nl>
//  07/31/1998 - a-llucar - created <nl>
//
// @index topic | IDIRECTMUSICPERFORMANCE_CASES
//--------------------------------------------------------------------------;
DWORD tdmperfGetResolvedTimeTest(BOOL fCoCreate, BOOL fMaxDebug, 
						   LPARAM dwParam1, LPARAM dwParam2)
{
	DWORD						dwRes			= FNS_PASS;
	HRESULT						hr				= S_OK;
	CtIDirectMusicPerformance	*pwPerformance	= NULL;
	CtIDirectMusicSegment		*pwSegment		= NULL;
	REFERENCE_TIME				rtResolved		= 0;

    // **********************************************************************
    // 1) Initialize COM. <nl>
    // **********************************************************************
	hr = dmthCoInitialize(NULL);
    if(FAILED(hr))
    {
        fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: COM initialization failed! (%s == %08Xh)"),
                tdmXlatHRESULT(hr), hr);
        return FNS_ABORTED;
    }

    // **********************************************************************
    // 2) Create CtIDirectMusicPerformance object 
    // **********************************************************************
    hr = dmthCreatePerformance(IID_IDirectMusicPerformance, &pwPerformance);
	if(FAILED(hr))
	{
        fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: DirectMusicSegment object creation "),
                TEXT("failed (%s == %08Xh)"),
                tdmXlatHRESULT(hr), hr);
		dwRes = FNS_ABORTED;
		goto TEST_END;
	}

	// **********************************************************************
    // 3) Get the Segment object
    // **********************************************************************
   	hr = dmthLoadSegment(L"test.mid", &pwSegment);
	if (FAILED(hr))
	{
		fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: dmthLoadSegment ")
				TEXT("failed (%s == %08Xh)"),
				tdmXlatHRESULT(hr), hr);
		dwRes = FNS_ABORTED;
		goto TEST_END;
	}

	// **********************************************************************
    // 4) dmthPrePlay
    // **********************************************************************		
	hr = dmthPrePlay(&pwPerformance);
    if(FAILED(hr))
	{
        fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: dmthPrePlay ")
                TEXT("failed (%s == %08Xh)"), tdmXlatHRESULT(hr), hr);
		dwRes = FNS_ABORTED;
		goto TEST_END;
	}

    // **********************************************************************
    // 5) Call GetResolvedTime and check results
    // **********************************************************************
	hr = pwPerformance->GetResolvedTime(2000, &rtResolved, DMUS_SEGF_BEAT);
	if(FAILED(hr))
	{
        fnsLog(BUGLOGLEVEL, TEXT("!!!! BUG: IDirectMusicPerformance->GetResolvedTime() ")
                TEXT("failed (%s == %08Xh)"),
                tdmXlatHRESULT(hr), hr);
		dwRes = FNS_FAIL;
		goto TEST_END;
	}

	// Test to make sure that dwSet and dwGet are the same
	if(!rtResolved)
	{
        fnsLog(BUGLOGLEVEL, TEXT("!!!! BUG: GetResolvedTime not set correctly, ")
                TEXT("rtResolved == %u)"), 
                rtResolved);
		dwRes = FNS_FAIL;
		goto TEST_END;
	}
TEST_END:
    // **********************************************************************
    // 6) Cleanup objects and uninitialize COM and Return results
    // **********************************************************************
	// Clean up the wrapper
	dmthReleasePerformance(pwPerformance);
	dmthReleaseSegment(pwSegment);
	dmthCoUninitialize();	
	return dwRes;
}
//===========================================================================


//--------------------------------------------------------------------------;
// @doc IDIRECTMUSICPERFORMANCE A-LLUCAR
//
// @topic IDirectMusicPerformance::RhythmToTime()_Unit_Test |
//
// Description: <nl>
// Unit test function for IDirectMusicPerformance::RhythmToTime().
//		Compares RhythmToTime and TimeToRhythm using Style->GetTimeSig().
//
// Test Function: <nl>
// tdmperfRhythmToTimeTest <nl>
//
// Parameters: <nl>
//  BOOL    fCoCreate   - unused <nl>
//  BOOL    fMaxDebug   - unused <nl>
//  DWORD   dwParam1    - unused <nl>
//	DWORD   dwParam2    - unused <nl>
//
// File : TDMPERF.CPP
//
// Steps: <nl>
// 1) Initialize COM. <nl>
// 2) Create a Performance Object <nl>
// 3) Create a Composer Object <nl>
// 4) Load the media <nl>
// 5) Compose the segment <nl>
// 6) Set up the performance to play <nl>
// 7) Play the Segment <nl>
// 8) Get the current MUSIC_TIME <nl>
// 9) Get the current Time Signature <nl>
// 10)Call GetStartTime to get the start of the segment <nl>
// 11)Call TimeToRhythm <nl>
// 12)Call RhythmToTime and compare mtTime with mtTime2 <nl>
// 13)Clean up objects, uninitialize COM and return results <nl>
//
// History: <nl>
//  11/18/1998 - a-llucar - created <nl>
//
// @index topic | IDIRECTMUSICPERFORMANCE_CASES
//--------------------------------------------------------------------------;
DWORD tdmperfRhythmToTimeTest(BOOL fCoCreate, BOOL fMaxDebug, 
						   LPARAM dwParam1, LPARAM dwParam2)
{
	DWORD						dwRes			= FNS_PASS;
	HRESULT						hr				= S_OK;
	CtIDirectMusicPerformance	*pwPerformance	= NULL;
	CtIDirectMusicSegment		*pwSegment		= NULL;
	CtIDirectMusicStyle			*pwStyle		= NULL;
	CtIDirectMusicChordMap		*pwChordMap		= NULL;
	CtIDirectMusicComposer		*pwComposer		= NULL;
	CtIDirectMusicSegmentState	*pwSegState		= NULL;

	MUSIC_TIME					mtTime			= 0;
	MUSIC_TIME					mtTime2			= 0;
	MUSIC_TIME					mtStartTime		= 0;
	WORD						wMeasure		= 0;
	BYTE						bBeat			= 0;
	BYTE						bGrid			= 0;
	SHORT						nOffset			= 0;
	DMUS_TIMESIGNATURE			TimeSig;

    // **********************************************************************
    // 1) Initialize COM. <nl>
    // **********************************************************************
	hr = dmthCoInitialize(NULL);
    if(FAILED(hr))
    {
        fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: COM initialization failed! (%s == %08Xh)"),
                tdmXlatHRESULT(hr), hr);
        return FNS_ABORTED;
    }

    // **********************************************************************
    // 2) Create CtIDirectMusicPerformance object 
    // **********************************************************************
    hr = dmthCreatePerformance(IID_IDirectMusicPerformance, &pwPerformance);
	if(FAILED(hr))
	{
        fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: DirectMusicSegment object creation "),
                TEXT("failed (%s == %08Xh)"),
                tdmXlatHRESULT(hr), hr);
		dwRes = FNS_ABORTED;
		goto TEST_END;
	}

    // **********************************************************************
    // 3) Create CtIDirectMusicComposer object (wrapped IDMComposer object)
    // **********************************************************************
	hr = dmthCreateComposer(IID_IDirectMusicComposer,&pwComposer);
	if (FAILED(hr))
	{
		fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: CoCreate Composer Object ")
				TEXT("failed (%s == %08Xh)"),
				tdmXlatHRESULT(hr), hr);
		dwRes = FNS_ABORTED;
		goto TEST_END;
	}

    // **********************************************************************
    // 4) Load the media
    // **********************************************************************
	hr = dmthLoadStyle(L"Jazz.sty", &pwStyle);
	if (FAILED(hr))
	{
		fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: dmthLoadStyle ")
				TEXT("failed (%s == %08Xh)"),
				tdmXlatHRESULT(hr), hr);
		dwRes = FNS_ABORTED;
		goto TEST_END;
	}

	hr = dmthLoadChordMap(NULL, &pwChordMap);
	if (FAILED(hr))
	{
		fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: dmthLoadChordMap ")
				TEXT("failed (%s == %08Xh)"),
				tdmXlatHRESULT(hr), hr);
		dwRes = FNS_ABORTED;
		goto TEST_END;
	}

    // **********************************************************************
    // 5) Compose the segment
    // **********************************************************************
	hr = pwComposer->ComposeSegmentFromShape(pwStyle, 8, DMUS_SHAPET_FALLING, 0, FALSE, FALSE, pwChordMap, &pwSegment);
	if (FAILED(hr))
	{
		fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: ComposeSegmentFromShape ")
				TEXT("failed (%s == %08Xh)"),
				tdmXlatHRESULT(hr), hr);
		dwRes = FNS_ABORTED;
		goto TEST_END;
	}

	// **********************************************************************
    // 6) dmthPrePlay
    // **********************************************************************		
	hr = dmthPrePlay(&pwPerformance);
    if(FAILED(hr))
	{
        fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: dmthPrePlay ")
                TEXT("failed (%s == %08Xh)"), tdmXlatHRESULT(hr), hr);
		dwRes = FNS_ABORTED;
		goto TEST_END;
	}

    // **********************************************************************
    // 7) Call PlaySegment and check results
    // **********************************************************************
	hr = pwPerformance->PlaySegment(pwSegment, 0, 0, &pwSegState);
	if(FAILED(hr))
	{
        fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: IDirectMusicPerformance->PlaySegment() ")
                TEXT("failed (%s == %08Xh)"),
                tdmXlatHRESULT(hr), hr);
		dwRes = FNS_ABORTED;
		goto TEST_END;
	}
	Sleep(2000);

    // **********************************************************************
    // 8) Get the current MusicTime
    // **********************************************************************
	hr = pwPerformance->GetTime(NULL, &mtTime);
	if(FAILED(hr))
	{
        fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: IDirectMusicPerformance->GetTime() ")
                TEXT("failed (%s == %08Xh)"),
                tdmXlatHRESULT(hr), hr);
		dwRes = FNS_ABORTED;
		goto TEST_END;
	}

    // **********************************************************************
    // 9) Get the Time Signature
    // **********************************************************************
	hr = pwStyle->GetTimeSignature(&TimeSig);
	if(FAILED(hr))
	{
        fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: IDirectMusicStyle->GetTimeSignature() ")
                TEXT("failed (%s == %08Xh)"),
                tdmXlatHRESULT(hr), hr);
		dwRes = FNS_ABORTED;
		goto TEST_END;
	}
	
    // **********************************************************************
    // 10) Get the StartTime of the Segment
    // **********************************************************************
	hr = pwSegState->GetStartTime(&mtStartTime);
	if(FAILED(hr) || !mtStartTime)
	{
        fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: IDirectMusicSegmentState->GetStartTime() ")
                TEXT("failed (%s == %08Xh), mtStartTime = %d"),
                tdmXlatHRESULT(hr), hr, mtStartTime);
		dwRes = FNS_ABORTED;
		goto TEST_END;
	}
	TimeSig.mtTime = mtStartTime;

    // **********************************************************************
    // 11) Now we'll call TimeToRhythm to populate the TimeSignature
    // **********************************************************************
	hr = pwPerformance->TimeToRhythm(mtTime, &TimeSig, &wMeasure, &bBeat, &bGrid, &nOffset);
	if(FAILED(hr))
	{
		fnsLog(BUGLOGLEVEL, TEXT("!!!! BUG:  IDirectMusicPerformance->TimeToRhythm() ")
				TEXT("failed (%s == %08Xh)"),
				tdmXlatHRESULT(hr), hr);
		dwRes = FNS_FAIL;
		goto TEST_END;
	}
	fnsLog(FYILOGLEVEL, TEXT("FYI:  \nwMeasures==%d\nbBeat == %d\nbGrid == %d\nnOffset == %d"),
				wMeasure, bBeat, bGrid, nOffset);

	if(!wMeasure && !bBeat && !bGrid && !nOffset)
	{
		fnsLog(BUGLOGLEVEL, TEXT("!!!! BUG:  TimeSignature was not set correctly")
				TEXT("wMeasure == %d\nbBeat == %d\nbGrid == %d\nnOffset == %d"),
				wMeasure, bBeat, bGrid, nOffset);
		dwRes = FNS_FAIL;
		goto TEST_END;
	}

    // **********************************************************************
    // 12) Call RhythmToTime and check results
    // **********************************************************************
	hr = pwPerformance->RhythmToTime(wMeasure, bBeat, bGrid, nOffset, &TimeSig, &mtTime2);
	if(FAILED(hr))
	{
		fnsLog(BUGLOGLEVEL, TEXT("!!!! BUG:  IDirectMusicPerformance->RhythmToTime() ")
				TEXT("failed (%s == %08Xh)"),
				tdmXlatHRESULT(hr), hr);
		dwRes = FNS_FAIL;
		goto TEST_END;
	}
	if(mtTime != mtTime2)
	{
		fnsLog(BUGLOGLEVEL, TEXT("mtTime and mtTime2 are not equal\n ")
				TEXT("mtTime == %d\nmtTime2 == %d"),
				mtTime, mtTime2);
		dwRes = FNS_FAIL;
		goto TEST_END;
	}

TEST_END:
    // **********************************************************************
    // 13) Cleanup objects and uninitialize COM and Return results
    // **********************************************************************
	dmthReleasePerformance(pwPerformance);
	dmthReleaseSegment(pwSegment);
	dmthReleaseStyle(pwStyle);
	dmthReleaseChordMap(pwChordMap);
	dmthReleaseComposer(pwComposer);
	dmthReleaseSegmentState(pwSegState);
	dmthCoUninitialize();	
	return dwRes;
}
//===========================================================================



//--------------------------------------------------------------------------;
// @doc IDIRECTMUSICPERFORMANCE A-LLUCAR
//
// @topic IDirectMusicPerformance2::QueryInterface()_Unit_Test |
//
// Description: <nl>
// Unit test function for IDirectMusicPerformance::QueryInterface().  QI's for
// IID_IDirectMusicPerformance2.
//
// Test Function: <nl>
// tdmperf2QueryInterfaceTest
//
// Parameters: <nl>
//  BOOL    fCoCreate   - unused <nl>
//  BOOL    fMaxDebug   - unused <nl>
//  DWORD   dwParam1    - unused <nl>
//	DWORD   dwParam2    - unused <nl>
//
// File : TDMCOMP.CPP
//
// Steps: <nl>
// 1) Initialize COM. <nl>
// 2) Create CtIDirectMusicComposer object (wrapped version of IDirectMusicComposer) <nl>
// 3) Call CtIDirectMusicComposer::QueryiInterface() to request an IID_IDirectMusicComposer 
// inteface object. <nl>
// 4) Cleanup objects, uninitialize COM, return test results <nl>
//
// History:
//  03/26/1998 - a-llucar - created <nl>
//
// @index topic | IDIRECTMUSICPERFORMANCE_CASES
//--------------------------------------------------------------------------;
DWORD tdmperf2QueryInterfaceTest(BOOL fCoCreate, BOOL fMaxDebug, LPARAM dwParam1, 
						  LPARAM dwParam2)
{
	HRESULT						hRes				= E_NOTIMPL;
	DWORD						dwRes				= FNS_PASS;
	CtIDirectMusicPerformance	*pwPerformance      = NULL;
	IDirectMusicPerformance		*pPerformance		= NULL;

	IUnknown					*pInterface			= NULL;
	IUnknown					*pIUnknown1			= NULL;
	IUnknown					*pIUnknown2			= NULL;
	GUID						*pIID				= (GUID *)dwParam2;

    // **********************************************************************
    // 1) Initialize COM.
    // **********************************************************************
    hRes = dmthCoInitialize(NULL);
    if(FAILED(hRes))
    {
        fnsLog(ABORTLOGLEVEL, TEXT("**** COM initialization failed! (%s == %08Xh)"),
                tdmXlatHRESULT(hRes), hRes);
        fnsLog(ABORTLOGLEVEL, TEXT("**** Test case aborting."));
        return FNS_ABORTED;
    }

    // **********************************************************************
    // 2) Create CtIDirectMusic object (wrapped version of IDirectMusic)
    // **********************************************************************
    hRes = dmthCreatePerformance(IID_IDirectMusicPerformance, &pwPerformance);
    if(FAILED(hRes))
    {
        fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: Base DirectMusicPerformance object creation ")
                TEXT("failed (%s == %08Xh)"),
                tdmXlatHRESULT(hRes), hRes);
        dwRes = FNS_ABORTED;
        goto TEST_END;
    }

    // **********************************************************************
    // 3) Call CtIDirectMusic::QueryiInterface() to request an IID_IDirectMusic 
    // inteface object.
    // **********************************************************************
    hRes = pwPerformance->QueryInterface(IID_IDirectMusicPerformance2, (void**)&pPerformance);
    if(FAILED(hRes))
    {
        fnsLog(BUGLOGLEVEL, TEXT("!!!! PENDING: QueryInterface(IID_IDirectMusicPerformance2) failed! ")
                TEXT("Do you have the right headers???"),
				TEXT("(%s == %08Xh)"),
                tdmXlatHRESULT(hRes), hRes);
        dwRes = FNS_PENDING;
        goto TEST_END;
    }

	// **********************************************************************
	// 4) QI the interface for the IID given to us.
    // **********************************************************************
	hRes = pPerformance->QueryInterface( *pIID, (void **)(&pInterface) );
    if ( FAILED(hRes) )
    {
        fnsLog( ABORTLOGLEVEL, "**** ABORT: pPerformance2::QI failed! "
			"(%s == %08Xh)", dmthXlatHRESULT(hRes), hRes);
        dwRes = FNS_ABORTED;
		goto TEST_END;
    }

    // **********************************************************************
	// 5) QI the newly obtained interface for IUnknown.
    // **********************************************************************
    fnsLog( FYILOGLEVEL, "---- FYI: QI'ing obtained interface for IUnknown " );
	hRes = pInterface->QueryInterface( IID_IUnknown, 
								(void **)(&pIUnknown1) );
    if ( FAILED(hRes) )
    {
        fnsLog( BUGLOGLEVEL, "!!!! BUG: IDMPerformance::QI for IUnknown failed! "
			"(%s == %08Xh)", dmthXlatHRESULT(hRes), hRes );
        dwRes = FNS_FAIL;
		goto TEST_END;
    }

    // **********************************************************************
	// 5) QI the original IDMBand for IUnknown.
    // **********************************************************************
	hRes = pwPerformance->QueryInterface( IID_IUnknown, 
								(void **)(&pIUnknown2) );
    if ( FAILED(hRes) )
    {
        fnsLog( BUGLOGLEVEL, "!!!! BUG: pwPerformance::QI for IUnknown failed! "
			"(%s == %08Xh)", dmthXlatHRESULT(hRes), hRes );
        dwRes = FNS_FAIL;
		goto TEST_END;
    }

    // **********************************************************************
	// 6) Compare their pointers.  Report a bug if they differ.
    // **********************************************************************
	if ( pIUnknown1 != pIUnknown2 )
	{
        fnsLog( BUGLOGLEVEL, "!!!! BUG: IUnknown pointers are different! "
			"(%s == %08Xh)", dmthXlatHRESULT(hRes), hRes );
        dwRes = FNS_FAIL;
		goto TEST_END;
    }

TEST_END:
    // **********************************************************************
    // 4) Cleanup objects, uninitialize COM, Return test result
    // **********************************************************************
    if(pwPerformance)
    {
        pwPerformance->Release();
        pwPerformance = NULL;
    }
    if(pPerformance)
    {
        pPerformance->Release();
        pPerformance = NULL;
    }
	if ( pInterface )
	{
		pInterface->Release();
        pInterface = NULL;
	}

    dmthCoUninitialize();

    return dwRes;

} 
*/
