//+-------------------------------------------------------------------------
//
//  Microsoft Windows
//
//  Copyright (C) Microsoft Corporation, 1998 - 1999
//
//  File:       tdmperfv.cpp
//
//--------------------------------------------------------------------------

//===========================================================================
// tdmperfv.cpp
//
// Valid Test case functions for IDirectMusicPerformance methods (valid parameters)
//
// Functions:
//  tdmperfAdjustTimeValidTest1()
//
// History:
//  03/27/1998 - a-llucar - created
//===========================================================================

#include "tdmusic.h"
#include "tdmperf.h"
#include "tdmatool.h"
#include "time.h"
#include <sys/timeb.h>
#define DWORD_MAX	0xffffffff;
#define ULONG_MIN	0

	const DWORD SEGF_FLAGS[]=
	{
		DMUS_SEGF_REFTIME,
		DMUS_SEGF_AFTERPREPARETIME,
		DMUS_SEGF_GRID,
		DMUS_SEGF_BEAT,
		DMUS_SEGF_MEASURE,
		DMUS_SEGF_DEFAULT,
		DMUS_SEGF_SECONDARY,
		DMUS_SEGF_QUEUE,
		DMUS_SEGF_CONTROL
	};

	const GUID *AllDMGuids[]=
	{
			&CLSID_DirectMusicPerformance,
			&CLSID_DirectMusicSegment,
			&CLSID_DirectMusicSegmentState,
			&CLSID_DirectMusicGraph,
			&CLSID_DirectMusicTempoTrack,
			&CLSID_DirectMusicSeqTrack,
			&CLSID_DirectMusicSysExTrack,
			&CLSID_DirectMusicTimeSigTrack,
			&CLSID_DirectMusicStyle,
			&CLSID_DirectMusicChordTrack,
			&CLSID_DirectMusicCommandTrack,
			&CLSID_DirectMusicStyleTrack,
			&CLSID_DirectMusicMotifTrack,
			&CLSID_DirectMusicChordMap,
			&CLSID_DirectMusicComposer,
			&CLSID_DirectMusicSignPostTrack,
			&CLSID_DirectMusicLoader,
			&CLSID_DirectMusicBandTrack,
			&CLSID_DirectMusicBand,
			&CLSID_DirectMusicChordMapTrack,
			&CLSID_DirectMusicMuteTrack,
			&GUID_DirectMusicAllTypes,
			&GUID_NOTIFICATION_SEGMENT,
			&GUID_NOTIFICATION_MEASUREANDBEAT,
			&GUID_NOTIFICATION_CHORD,
			&GUID_NOTIFICATION_COMMAND,
			&GUID_CommandParam,
			&GUID_ChordParam,
			&GUID_RhythmParam,
			&GUID_IDirectMusicStyle,
			&GUID_TempoParam,
			&GUID_Download,
			&GUID_Unload,
			&GUID_Enable_Auto_Download,
			&GUID_Disable_Auto_Download,
			&GUID_Clear_All_Bands,
			&GUID_IDirectMusicBand,
			&GUID_IgnoreBankSelectForGM,
			&GUID_ConnectToDLSCollection,
			&GUID_IDirectMusicChordMap,
			&GUID_MuteParam,
			&GUID_PerfMasterTempo,
			&GUID_PerfMasterVolume,
			&GUID_PerfMasterGrooveLevel,
			&GUID_DefaultGMCollection,
			&IID_IDirectMusicLoader,
			&IID_IDirectMusicGetLoader,
			&IID_IDirectMusicObject,
			&IID_IDirectMusicSegment,
			&IID_IDirectMusicSegmentState,
			&IID_IDirectMusicTrack,
			&IID_IDirectMusicPerformance,
			&IID_IDirectMusicTool,
			&IID_IDirectMusicGraph,
			&IID_IDirectMusicStyle,
			&IID_IDirectMusicChordMap,
			&IID_IDirectMusicComposer,
			&IID_IDirectMusicBand
	};

//--------------------------------------------------------------------------;
// @doc IDIRECTMUSICPERFORMANCE A-LLUCAR
//
// @topic IDirectMusicPerformance::AdjustTime()_Valid_Test1 |
//
// Description: <nl>
// Valid test function for IDirectMusicPerformance::AdjustTime(). <nl>
// Calls AdjustTime() back and forth while Segment is currently playing. <nl>
//
// Test Function: <nl>
// tdmperfvAdjustTimeValidTest1 <nl>
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
// 2) Create CtIDirectMusicPerformance object <nl>
// 3) Call PlayMidiFile convienence function <nl>
// 4) Call AdjustTime(large negative) and check results <nl>
// 5) Call AdjustTime(large positive) and check results <nl>
// 6) Release objects, CoUninitialize, and return results <nl>
//
// History: <nl>
//  03/29/1998 - a-llucar - Created <nl>
//
// @index topic | IDIRECTMUSICPERFORMANCE_CASES
//--------------------------------------------------------------------------;
DWORD tdmperfvAdjustTimeValidTest1(BOOL fCoCreate, BOOL fMaxDebug, 
						   LPARAM dwParam1, LPARAM dwParam2)
{
	DWORD							dwRes			= FNS_PASS;
	HRESULT							hr				= S_OK;
	CtIDirectMusicPerformance		*pwPerformance	= NULL;
	CtIDirectMusicSegment			*pwSegment		= NULL;
	WCHAR							*pMidiFile		= (WCHAR*)dwParam1;
	REFERENCE_TIME					rtNow			= 0;
   	
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
    // 3) Call PlayMidiFile() convienence function
    // **********************************************************************
	hr = dmthPlayMidiFile(pwPerformance,CTIID_IDirectMusicSegment,(CtIDirectMusicSegment**)&pwSegment,(CtIDirectMusicSegmentState**)NULL, TRUE, pMidiFile);
	if(S_OK != hr)
	{
        fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: PlayMidiFile convienence function "),
                TEXT("failed (%s == %08Xh)"),
                tdmXlatHRESULT(hr), hr);
		dwRes = FNS_ABORTED;
		goto TEST_END;
	}

    // **********************************************************************
    // 4) Call AdjustTime(large negative) and check results
    // **********************************************************************
	Sleep(3000);
	hr = pwPerformance->GetTime(&rtNow, NULL);
	if(FAILED(hr))
	{
		fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT:  Performance->Gettime")
				TEXT("failed (%s == %08Xh)"),
				tdmXlatHRESULT(hr), hr);
		dwRes = FNS_FAIL;
		goto TEST_END;
	}

	fnsLog(FYILOGLEVEL, TEXT("GetTime returned %08Xh"), rtNow);
	rtNow = 0;

	hr = pwPerformance->AdjustTime( -10000000);
	if(FAILED(hr))
	{
        fnsLog(BUGLOGLEVEL, TEXT("!!!! BUG: IDirectMusicPerformance->AdjustTime ")
                TEXT("failed (%s == %08Xh)"),
                tdmXlatHRESULT(hr), hr);
		dwRes = FNS_FAIL;
		goto TEST_END;
	}

	hr = pwPerformance->GetTime(&rtNow, NULL);
	if(FAILED(hr))
	{
		fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT:  Performance->Gettime")
				TEXT("failed (%s == %08Xh)"),
				tdmXlatHRESULT(hr), hr);
		dwRes = FNS_FAIL;
		goto TEST_END;
	}

	fnsLog(FYILOGLEVEL, TEXT("GetTime returned %08Xh"), rtNow);
	Sleep(7000);

TEST_END:
    // **********************************************************************
    // 6) Release objects, CoInitialize, and return results
    // **********************************************************************
	if(pwPerformance)
	{
		pwPerformance->Stop(NULL, NULL, 0, 0);
		pwPerformance->CloseDown();
		pwPerformance->Release();
		pwPerformance = NULL;
	}
	if(pwSegment)
	{
		pwSegment->Release();
		pwSegment = NULL;
	}
	dmthCoUninitialize();	
	return dwRes;
}	// End tdmperfAdjustTimeValidTest1()


//--------------------------------------------------------------------------;
// @doc IDIRECTMUSICPERFORMANCE A-LLUCAR
//
// @topic IDirectMusicPerformance::AddPort()_Valid_Test1 |
//
// Description: <nl>
// Valid test function for IDirectMusicPerformance::AddPort(). <nl>
// Calls AddPort passing NULL for pPort
//
// Test Function: <nl>
// tdmperfvAddPortValidTest1<nl>
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
// 2) Create CtIDirectMusicPerformance object <nl>
// 3) Load a Segment Object <nl>
// 4) Init the performance <nl>
// 5) Call Performance->AddPort passing NULL <nl>
// 6) Release objects, CoUninitialize, and check results <nl>
//
// History: <nl>
//  03/29/1998 - a-llucar - created <nl>
//  07/08/1998 - a-llucar - clean up <nl>
//
// @index topic | IDIRECTMUSICPERFORMANCE_CASES
//--------------------------------------------------------------------------;
DWORD tdmperfvAddPortValidTest1(BOOL fCoCreate, BOOL fMaxDebug, 
						   LPARAM dwParam1, LPARAM dwParam2)
{
	DWORD						dwRes			= FNS_PASS;
	HRESULT						hr				= S_OK;
	CtIDirectMusicPerformance	*pwPerformance	= NULL;
	CtIDirectMusicSegment		*pwSegment		= NULL;

	WCHAR						*pSegmentFile	= (WCHAR*)dwParam1;
	WCHAR						*wchPath		= gwszBasePath;
	char						*chPath			= gszMediaPath;
	dmthSetPath(NULL);
	
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
    // 3) Load a Segment Object
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
    // 4) Init the performance
	// **********************************************************************
	hr = pwPerformance->Init(NULL,NULL,ghWnd);
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
    // 5) Call Performance->AddPort passing NULL
	// **********************************************************************
	hr = pwPerformance->AddPort(NULL);
	if(FAILED(hr))
	{
        fnsLog(BUGLOGLEVEL, TEXT("!!!! BUG: pwPerformance->AddPort(NULL) ")
                TEXT("failed (%s == %08Xh)"),
                tdmXlatHRESULT(hr), hr);
		dwRes = FNS_FAIL;
		goto TEST_END;
	}

TEST_END:
    // **********************************************************************
    // 6) Release objects, CoUninitialize, and check results
    // **********************************************************************
	if(pwPerformance)
	{
		pwPerformance->CloseDown();
		pwPerformance->Release();
		pwPerformance = NULL;
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
// @topic IDirectMusicPerformance::AddPort()_Valid_Test2 |
//
// Description: <nl>
// Valid test function for IDirectMusicPerformance::AddPort(). <nl>
// Calls AddPort() while a Segment is currently playing.
//
// Test Function: <nl>
// tdmperfvAddPortValidTest2<nl>
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
// 2) Create CtIDirectMusicPerformance object <nl>
// 3) Call dmthPlayMidiFile convienence function<nl>
// 4) Create a Port and check results <nl>
// 5) Release objects, CoUninitialize, and return results <nl>
//
// History: <nl>
//  03/29/1998 - a-llucar - created <nl>
//
// @index topic | IDIRECTMUSICPERFORMANCE_CASES
//--------------------------------------------------------------------------;
DWORD tdmperfvAddPortValidTest2(BOOL fCoCreate, BOOL fMaxDebug, 
						   LPARAM dwParam1, LPARAM dwParam2)
{
	DWORD						dwRes			= FNS_PASS;
	HRESULT						hr				= S_OK;
	CtIDirectMusicPerformance	*pwPerformance	= NULL;
	CtIDirectMusicSegment		*pwSegment		= NULL;
	WCHAR						*pMidiFile		= (WCHAR*)dwParam1;

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
    // 3) Call PlayMidiFile() convienence function
    // **********************************************************************
	hr = dmthPlayMidiFile(pwPerformance,CTIID_IDirectMusicSegment,(CtIDirectMusicSegment**)&pwSegment,(CtIDirectMusicSegmentState**)NULL, TRUE, pMidiFile);
	if(S_OK != hr)
	{
        fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: PlayMidiFile convienence function "),
                TEXT("failed (%s == %08Xh)"),
                tdmXlatHRESULT(hr), hr);
		dwRes = FNS_ABORTED;
		goto TEST_END;
	}

    // **********************************************************************
    // 4) Call AddPort and check results
    // **********************************************************************
	hr = pwPerformance->AddPort(NULL);
	if(FAILED(hr))
	{
        fnsLog(BUGLOGLEVEL, TEXT("!!!! BUG: IDirectMusicPerformance->AddPort ")
                TEXT("failed (%s == %08Xh)"),
                tdmXlatHRESULT(hr), hr);
		dwRes = FNS_FAIL;
		goto TEST_END;
	}    
	Sleep(3000);

TEST_END:
    // **********************************************************************
    // 6) Release objects, CoUninitialize, and check results
    // **********************************************************************
	if(pwPerformance)
	{
		pwPerformance->Stop(NULL, NULL, 0, 0);
		pwPerformance->CloseDown();
		pwPerformance->Release();
		pwPerformance = NULL;
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
// @topic IDirectMusicPerformance::CloseDown()_Valid_Test1 |
//
// Description: <nl>
// Valid test function for IDirectMusicPerformance::CloseDown().
//		Calls CloseDown() without first Init'ing the performance.
//
// Test Function: <nl>
// tdmperfvCloseDownValidTest1 <nl>
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
// 2) Create CtIDirectMusicPerformance object <nl>
// 3) Call CloseDown() and check results <nl>
// 4) Release objects, CoUninitialize, and return results <nl>
//
// History: <nl>
//  03/23/1998 - a-llucar - created <nl>
//
// @index topic | IDIRECTMUSICPERFORMANCE_CASES
//--------------------------------------------------------------------------;
DWORD tdmperfvCloseDownValidTest1(BOOL fCoCreate, BOOL fMaxDebug, 
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
    // 3) Call Closedown() and check results
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
    // 4) Release objects, CoUninitialize, and return result
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
// @topic IDirectMusicPerformance::AssignPChannel()_Valid_Test1 |
//
// Description: <nl>
// Valid test function for IDirectMusicPerformance::AssignPChannel(). <nl>
//		Re-assigns the same PChannel twice.  Checks results. <nl>
//
// Test Function: <nl>
// tdmperfvAssignPChannelValidTest1 <nl>
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
// 2) Create CtIDirectMusicPerformance object <nl>
// 3) Init a Performance <nl>
// 4) Enumerate a Port and check results <nl>
// 5) Create a Port and check results <nl>
// 6) Call AddPort and check results <nl>
// 7) Call AssignPChannel twice to the same dwPChannel number and check results <nl>
// 8) Call RemovePort and check results <nl>
// 9) Release objects, CoUninitialize, and return results <nl>
//
// History: <nl>
//  01/14/1998 - markburt - created <nl>
//  03/23/1998 - a-llucar - clean up, logging installed, autodoc <nl>
//
// @index topic | IDIRECTMUSICPERFORMANCE_CASES
//--------------------------------------------------------------------------;
DWORD tdmperfvAssignPChannelValidTest1(BOOL fCoCreate, BOOL fMaxDebug, 
						   LPARAM dwParam1, LPARAM dwParam2)
{
	DWORD						dwRes			= FNS_PASS;
	HRESULT						hr				= S_OK;
	CtIDirectMusic				*pwMusic		= NULL;
	CtIDirectMusicPerformance	*pwPerformance	= NULL;
	CtIDirectMusicPort			*pwPort			= NULL;
	DMUS_PORTPARAMS				dmos;
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
	hr = pwPerformance->AddPort(pwPort);
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
	hr = pwPerformance->AssignPChannel(5, pwPort, 1, 0);
	if(FAILED(hr))
	{
        fnsLog(BUGLOGLEVEL, TEXT("!!!! BUG: IDirectMusicPerformance->AssignPChannel ")
                TEXT("failed (%s == %08Xh)"),
                tdmXlatHRESULT(hr), hr);
		dwRes = FNS_FAIL;
		goto TEST_END;
	}

	hr = pwPerformance->AssignPChannel(5, pwPort, 1, 0);
	if(FAILED(hr))
	{
        fnsLog(BUGLOGLEVEL, TEXT("!!!! BUG: IDirectMusicPerformance->AssignPChannel ")
                TEXT("failed (%s == %08Xh) \n (Attempted to assign the same PChannel number twice)."),
                tdmXlatHRESULT(hr), hr);
		dwRes = FNS_FAIL;
		goto TEST_END;
	}

	// **********************************************************************
    // 8) Call RemovePort and check results
    // **********************************************************************
	hr = pwPerformance->RemovePort(pwPort);
	if(FAILED(hr))
	{
        fnsLog(FYILOGLEVEL, TEXT("**** FYI: IDirectMusicPerformance->RemovePort ")
                TEXT("failed (%s == %08Xh)"),
                tdmXlatHRESULT(hr), hr);
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
// @topic IDirectMusicPerformance::AssignPChannel()_Valid_Test2 |
//
// Description: <nl>
// Valid test function for IDirectMusicPerformance::AssignPChannel(). <nl>
//		Calls AssignPChannel before calling AddPort(). <nl>
//		NOTE:  We Expect this test to Fail! <nl>
//
// Test Function: <nl>
// tdmperfvAssignPChannelValidTest2 <nl>
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
// 2) Create CtIDirectMusicPerformance object <nl>
// 3) Init a Performance <nl>
// 4) Enumerate a Port and check results <nl>
// 5) Create a Port and check results <nl>
// 6) Call AssignPChannel and check results <nl>
// 7) Release objects, CoUninitialize, and return results <nl>
//
// History: <nl>
//  01/14/1998 - markburt - created <nl>
//  03/23/1998 - a-llucar - clean up, logging installed, autodoc <nl>
//
// @index topic | IDIRECTMUSICPERFORMANCE_CASES
//--------------------------------------------------------------------------;
DWORD tdmperfvAssignPChannelValidTest2(BOOL fCoCreate, BOOL fMaxDebug, 
						   LPARAM dwParam1, LPARAM dwParam2)
{
	DWORD								dwRes					= FNS_PASS;
	HRESULT							hr						= S_OK;
	CtIDirectMusic					*pwMusic				= NULL;
	CtIDirectMusicPerformance	*pwPerformance		= NULL;
	CtIDirectMusicPort			*pwPort				= NULL;
	GUID								guidPort				= GUID_NULL;
	DMUS_PORTPARAMS				dmos;

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
    // 6) Call AssignPChannel and check results
    // **********************************************************************
	hr = pwPerformance->AssignPChannel( 5, pwPort, 1, 0 );
	fnsLog(FYILOGLEVEL, TEXT("**** FYI: WE EXPECT THIS TO RETURN E_INVALIDARG!!!!"));
	if(hr != E_INVALIDARG) //expecting a fail (E_INVALIDARG)
	{
        fnsLog(BUGLOGLEVEL, TEXT("!!!! BUG: IDirectMusicPerformance->AssignPChannel ")
                TEXT("did not return correctly(%s == %08Xh)"),
                tdmXlatHRESULT(hr), hr);
		dwRes = FNS_FAIL;
		goto TEST_END;
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
	if( pwPort )
	{
		pwPort->Release();
		pwPort = NULL;
	}
	if( pwMusic )
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
// @topic IDirectMusicPerformance::AssignPChannelBlock()_Valid_Test1 |
//
// Description: <nl>
// Valid test function for IDirectMusicPerformance::AssignPChannelBlock(). <nl>
//		Assigns the same PChannelBlock twice <nl>
//
// Test Function: <nl>
// tdmperfvAssignPChannelBlockValidTest1 <nl>
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
// 2) Create CtIDirectMusicPerformance object <nl>
// 3) Init a Performance <nl>
// 4) Enumerate a Port and check results <nl>
// 5) Create a Port and check results <nl>
// 6) Call AddPort and check results <nl>
// 7) Call AssignPChannelBlock (twice to same dwBlockNum) and check results <nl>
// 8) Call RemovePort and check results <nl>
// 9) Release objects, CoUninitialize, and return results <nl>
//
// History: <nl>
//  01/14/1998 - markburt - created <nl>
//  03/23/1998 - a-llucar - clean up, logging installed, autodoc <nl>
//
// @index topic | IDIRECTMUSICPERFORMANCE_CASES
//--------------------------------------------------------------------------;
DWORD tdmperfvAssignPChannelBlockValidTest1(BOOL fCoCreate, BOOL fMaxDebug, 
						   LPARAM dwParam1, LPARAM dwParam2)
{
	DWORD						dwRes			= FNS_PASS;
	HRESULT						hr				= S_OK;
	CtIDirectMusic				*pwMusic		= NULL;
	CtIDirectMusicPerformance	*pwPerformance	= NULL;
	CtIDirectMusicPort			*pwPort			= NULL;
	DMUS_PORTPARAMS				dmos;
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
	hr = pwPerformance->AssignPChannelBlock( 3, pwPort, 1 );
	if(FAILED(hr))
	{
        fnsLog(BUGLOGLEVEL, TEXT("!!!! BUG: IDirectMusicPerformance->AssignPChannelBlock ")
                TEXT("failed (%s == %08Xh)"),
                tdmXlatHRESULT(hr), hr);
		dwRes = FNS_FAIL;
		goto TEST_END;
	}

	hr = pwPerformance->AssignPChannelBlock( 3, pwPort, 1 );
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
        fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: IDirectMusicPerformance->RemovePort ")
                TEXT("failed (%s == %08Xh)"),
                tdmXlatHRESULT(hr), hr);
		dwRes = FNS_ABORTED;
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
	if( pwPort )
	{
		pwPort->Release();
		pwPort = NULL;
	}
	if( pwMusic )
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
// @topic IDirectMusicPerformance::AssignPChannelBlock()_Valid_Test2 |
//
// Description: <nl>
// Valid test function for IDirectMusicPerformance::AssignPChannelBlock(). <nl>
//		AssignPChannelBlock call before AddPort()<nl>
//
// Test Function: <nl>
// tdmperfvAssignPChannelBlockValidTest2 <nl>
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
// 2) Create CtIDirectMusicPerformance object <nl>
// 3) Init a Performance <nl>
// 4) Enumerate a Port and check results <nl>
// 5) Create a Port and check results <nl>
// 6) Call AssignPChannelBlock (without call to AddPort()) and check results <nl>
// 7) Release objects, CoUninitialize, and return results <nl>
//
// History: <nl>
// 03/30/98 - a-llucar - created. 
//
// @index topic | IDIRECTMUSICPERFORMANCE_CASES
//--------------------------------------------------------------------------;
DWORD tdmperfvAssignPChannelBlockValidTest2(BOOL fCoCreate, BOOL fMaxDebug, 
						   LPARAM dwParam1, LPARAM dwParam2)
{
	DWORD						dwRes			= FNS_PASS;
	HRESULT						hr				= S_OK;
	CtIDirectMusic				*pwMusic		= NULL;
	CtIDirectMusicPerformance	*pwPerformance	= NULL;
	CtIDirectMusicPort			*pwPort			= NULL;
	DMUS_PORTPARAMS				dmos;
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
        fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: DirectMusicPerformance object creation ")
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
    // 6) Call AssignPChannelBlock and check results
    // **********************************************************************
	hr = pwPerformance->AssignPChannelBlock( 3, pwPort, 1 );
	fnsLog(FYILOGLEVEL, TEXT("**** FYI: WE EXPECT THIS TO RETURN E_INVALIDARG!!!!"));
	if(hr != E_INVALIDARG)
	{
        fnsLog(BUGLOGLEVEL, TEXT("!!!! BUG: IDirectMusicPerformance->AssignPChannelBlock ")
                TEXT("failed (%s == %08Xh)"),
                tdmXlatHRESULT(hr), hr);
		dwRes = FNS_FAIL;
		goto TEST_END;
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
	if( pwPort )
	{
		pwPort->Release();
		pwPort = NULL;
	}
	if( pwMusic )
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
// @topic IDirectMusicPerformance::AddNotificationType()_Valid_Test1 |
//
// Description: <nl>
// Valid test function for IDirectMusicPerformance::AddNotificationType(). <nl>
//		Calls AddNotificationType(NULL) <nl>
//
// Test Function: <nl>
// tdmperfvAddNotificationTypeValidTest1 <nl>
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
// 2) Create CtIDirectMusicPerformance object <nl>
// 3) Call AddNotificationType(NULL) and check results <nl>
// 4) Call RemoveNotificationType(NULL) and check results <nl>
// 5) Release objects, CoUninitialize, and return results <nl>
//
// History: <nl>
//  04/01/1998 - a-llucar - created <nl>
//
//	Note:  Same test for AddNotificationTypeValidTest1() and RemoveNotificationTypeValidTest1(). <nl>
//
// @index topic | IDIRECTMUSICPERFORMANCE_CASES
//--------------------------------------------------------------------------;
DWORD tdmperfvAddNotificationTypeValidTest1(BOOL fCoCreate, BOOL fMaxDebug, 
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
    // 3) Call AddNotificationType and check results
    // **********************************************************************
	hr = pwPerformance->AddNotificationType(GUID_NULL);
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
	hr = pwPerformance->RemoveNotificationType(GUID_NULL);
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
// @topic IDirectMusicPerformance::RemoveNotificationType()_Valid_Test1 |
//
// Description: <nl>
// Valid test function for IDirectMusicPerformance::RemoveNotificationType(). <nl>
//		Calls RemoveNotificationType(NULL) <nl>
//
// Test Function: <nl>
// tdmperfvAddNotificationTypeValidTest1 <nl>
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
// 2) Create CtIDirectMusicPerformance object <nl>
// 3) Call AddNotificationType(NULL) and check results <nl>
// 4) Call RemoveNotificationType(NULL) and check results <nl>
// 5) Release objects, CoUninitialize, and return results <nl>
//
// History: <nl>
//  04/01/1998 - a-llucar - created <nl>
//
//	Note:  Same test for AddNotificationTypeValidTest1() and RemoveNotificationTypeValidTest1(). <nl>
//
// @index topic | IDIRECTMUSICPERFORMANCE_CASES
//--------------------------------------------------------------------------;
// !!!!!!!! Test used is tdmperfvAddNotificationTypeValidTest1 !!!!!!!!!!!!!!



//--------------------------------------------------------------------------;
// @doc IDIRECTMUSICPERFORMANCE A-LLUCAR
//
// @topic IDirectMusicPerformance::AddNotificationType()_Valid_Test2 |
//
// Description: <nl>
// Valid test function for IDirectMusicPerformance::AddNotificationType(). <nl>
//		Calls AddNotificationType with All types of guids <nl>
//
// Test Function: <nl>
// tdmperfvAddNotificationTypeValidTest2 <nl>
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
// 2) Create CtIDirectMusicPerformance object <nl>
// 3) Create an Event for hNotification <nl>
// 4) Create an Event for hNotification and check results <nl>
// 5) AddNotificationType(All Types) and check results <nl>
// 6) Call PlayMidiFile() convienence function <nl>
// 7) Wait for a single object to know that things should be fine <nl>
// 8) Call GetNotificationPMsg and check results <nl>
// 9) Call FreePMsg and check results <nl>
// 10) Release objects, CoUninitialize, and return results <nl>
//
// History: <nl>
//  03/23/1998 - a-llucar - created <nl>
//
//  NOTE: Same test used for AddNotificationType(), GetNotificationPMsg() and FreePMsg()
//
// @index topic | IDIRECTMUSICPERFORMANCE_CASES
//--------------------------------------------------------------------------;
DWORD tdmperfvAddNotificationTypeValidTest2(BOOL fCoCreate, 
											BOOL fMaxDebug, 
											LPARAM dwParam1, 
											LPARAM dwParam2)
{
	DWORD						dwRes				= FNS_PASS;
	HRESULT						hr					= S_OK;
	DMUS_NOTIFICATION_PMSG		*pEvent				= NULL;
	CtIDirectMusicPerformance	*pwPerformance		= NULL;
	CtIDirectMusicSegment		*pwSegment			= NULL;
	HANDLE						hNotification;
	DWORD						dwResult;
	GUID						*guidNotification	= (GUID*)dwParam1;
	WCHAR						*pMidiFile			= (WCHAR*)dwParam2;
    char                        szGuid[MAX_LOGSTRING];

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
	// *********************************************************************
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
	hr = pwPerformance->AddNotificationType(*guidNotification);
	if(FAILED(hr))
	{
	    fnsLog(BUGLOGLEVEL, TEXT("!!!! BUG: pwPerformance->AddNotificationType ")
		    TEXT("failed (%s == %08Xh)"),
			tdmXlatHRESULT(hr), hr);
		dwRes = FNS_FAIL;
		goto TEST_END;
	}
   	// **********************************************************************
    // 6) Call PlayMidiFile() convienence function
    // **********************************************************************
	hr = dmthPlayMidiFile(pwPerformance,CTIID_IDirectMusicSegment,(CtIDirectMusicSegment**)&pwSegment,(CtIDirectMusicSegmentState**)NULL, TRUE, pMidiFile);
	if(hr != S_OK)
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
	dwResult = WaitForSingleObject( hNotification, 10000 );
	if( dwResult != WAIT_OBJECT_0 )
	{
	    fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: WaitForSingleObject ")
		    TEXT("failed (%s == %08Xh)"),
			tdmXlatHRESULT(hr), hr);
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
                TEXT("pEvent was not set"));
		dwRes = FNS_FAIL;
		goto TEST_END;
	}
    //**********************************************************************
    // Validate the Members of the PMsg
    //**********************************************************************
    //dwSize
    fnsLog(FYILOGLEVEL,"!!!! FYI: pEvent->dwSize = %d",pEvent->dwSize);
    if (pEvent->dwSize != sizeof(DMUS_NOTIFICATION_PMSG))
    {
        fnsLog(BUGLOGLEVEL, "!!!! BUG: pEvent->dwSize incorrect.");
        fnsLog(BUGLOGLEVEL, "           Is: %d", pEvent->dwSize);
        fnsLog(BUGLOGLEVEL, "    Should be: %d", sizeof(DMUS_NOTIFICATION_PMSG));
		dwRes = FNS_FAIL;
		goto TEST_END;
    }
    //guidNotificationType
    dmthGUIDtoString(pEvent->guidNotificationType, szGuid);
    fnsLog(FYILOGLEVEL,"!!!! FYI: pEvent->guidNotificationType = %s (%s)",
        szGuid, dmthXlatGUID(pEvent->guidNotificationType));
    
    if (pEvent->guidNotificationType != (*guidNotification))
    {
        fnsLog(BUGLOGLEVEL, "!!!! BUG: pEvent->guidNotificationType incorrect.");
        fnsLog(BUGLOGLEVEL, "           Is: %s (%s)",
                pEvent->guidNotificationType, 
                szGuid);
        fnsLog(BUGLOGLEVEL, "    Should be: %s (%s)",
                pEvent->guidNotificationType,
                dmthXlatGUID(*guidNotification));
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
	Sleep(1000);
		
TEST_END:
    // **********************************************************************
    // 10) Release objects, CoUninitialize, and return results
    // **********************************************************************
	if(pwPerformance)
	{
        if (pEvent)
        {
        	pwPerformance->FreePMsg( (DMUS_PMSG*)pEvent );
        }

        pwPerformance->Stop(NULL, NULL, 0, 0);
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
// @topic IDirectMusicPerformance::GetNotificationPMsg()_Valid_Test2 |
//
// Description: <nl>
// Valid test function for IDirectMusicPerformance::GetNotificationPMsg(). <nl>
//		Calls GetNotificationPMsg with All types of guids <nl>
//
// Test Function: <nl>
// tdmperfvAddNotificationTypeValidTest2 <nl>
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
// 2) Create CtIDirectMusicPerformance object <nl>
// 3) Create an Event for hNotification <nl>
// 4) Create an Event for hNotification and check results <nl>
// 5) AddNotificationType(All Types) and check results <nl>
// 6) Call PlayMidiFile() convienence function <nl>
// 7) Wait for a single object to know that things should be fine <nl>
// 8) Call GetNotificationPMsg and check results <nl>
// 9) Call FreePMsg and check results <nl>
// 10) Release objects, CoUninitialize, and return results <nl>
//
// History: <nl>
//  03/23/1998 - a-llucar - created <nl>
//
//  NOTE: Same test used for AddNotificationType(), GetNotificationPMsg() and FreePMsg()
//
// @index topic | IDIRECTMUSICPERFORMANCE_CASES
//--------------------------------------------------------------------------;


//--------------------------------------------------------------------------;
// @doc IDIRECTMUSICPERFORMANCE A-LLUCAR
//
// @topic IDirectMusicPerformance::SetNotificationHandle()_Valid_Test1 |
//
// Description: <nl>
// Valid test function for IDirectMusicPerformance::SetNotificationHandle(). <nl>
//		Calls SetNotificationHandle (2 seconds wait), sleeps for 3 seconds, <nl>
//      and verifies that the Event was discarded. <nl>
//
// Test Function: <nl>
// tdmperfvSetNotificationHandleValidTest1 <nl>
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
// 2) Create CtIDirectMusicPerformance object <nl>
// 3) Create an Event for hNotification <nl>
// 4) Create an Event for hNotification and check results <nl>
// 5) AddNotificationType(All Types) and check results <nl>
// 6) Call PlayMidiFile() convienence function <nl>
// 7) Wait for a single object to know that things should be fine <nl>
// 8) Call GetNotificationPMsg and check results <nl>
// 9) Call FreePMsg and check results <nl>
// 10) Release objects, CoUninitialize, and return results <nl>
//
// History: <nl>
//  07/16/1998 - a-llucar - created <nl>
//
// @index topic | IDIRECTMUSICPERFORMANCE_CASES
//--------------------------------------------------------------------------;
DWORD tdmperfvSetNotificationHandleValidTest1(BOOL fCoCreate, 
											BOOL fMaxDebug, 
											LPARAM dwParam1, 
											LPARAM dwParam2)
{
	DWORD						dwRes				= FNS_PASS;
	HRESULT						hr					= S_OK;
	DMUS_NOTIFICATION_PMSG		*pEvent				= NULL;
	CtIDirectMusicPerformance	*pwPerformance		= NULL;
	CtIDirectMusicSegment		*pwSegment			= NULL;
	HANDLE						hNotification;
	DWORD						dwResult;
	WCHAR						*pMidiFile			= (WCHAR*)dwParam2;

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
	// *********************************************************************
	hr = pwPerformance->SetNotificationHandle( hNotification, 0 );
	if(FAILED(hr))
	{
	    fnsLog(BUGLOGLEVEL, TEXT("!!!! BUG: pwPerformance->SetNotificationHandle  ")
		    TEXT("failed (%s == %08Xh)"),
			tdmXlatHRESULT(hr), hr);
		dwRes = FNS_FAIL;
		goto TEST_END;
	}
		
	// **********************************************************************
	// 5) AddNotificationType and check results 
	// **********************************************************************
	hr = pwPerformance->AddNotificationType(GUID_NOTIFICATION_MEASUREANDBEAT);
	if(FAILED(hr))
	{
	    fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: pwPerformance->AddNotificationType ")
		    TEXT("failed (%s == %08Xh)"),
			tdmXlatHRESULT(hr), hr);
		dwRes = FNS_ABORTED;
		goto TEST_END;
	}

	Sleep(3000);
   	// **********************************************************************
    // 6) Call PlayMidiFile() convienence function
    // **********************************************************************
	hr = dmthPlayMidiFile(pwPerformance,CTIID_IDirectMusicSegment,(CtIDirectMusicSegment**)&pwSegment,(CtIDirectMusicSegmentState**)NULL, TRUE, pMidiFile);
	if(hr != S_OK)
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
	dwResult = WaitForSingleObject( hNotification, 10000 );
	if( dwResult != WAIT_OBJECT_0 ) // It found a notification, which it shouldn't have
	{
	    fnsLog(BUGLOGLEVEL, TEXT("!!!! BUG: WaitForSingleObject ")
		    TEXT("failed (%s == %08Xh)"),
			tdmXlatHRESULT(hr), hr);
		dwRes = FNS_FAIL;
		goto TEST_END;
	}
	
	// Check to make sure that pEvent was discarded
	if(pEvent)
	{
        fnsLog(BUGLOGLEVEL, TEXT("!!!! BUG: pwPerformance->GetNotificationPMsg ")
                TEXT("pEvent was set, but it should not have been."));
		dwRes = FNS_FAIL;
		goto TEST_END;
	}

	Sleep(1000);
		
TEST_END:
    // **********************************************************************
    // 10) Release objects, CoUninitialize, and return results
    // **********************************************************************
	if(pwPerformance)
	{
		pwPerformance->Stop(NULL, NULL, 0, 0);
		pwPerformance->CloseDown();
		pwPerformance->Release();
		pwPerformance = NULL;
	}
	if (hNotification)
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
// @topic IDirectMusicPerformance::GetTime()_Valid_Test |
//
// Description: <nl>
// Valid test function for IDirectMusicPerformance::GetTime(). <nl>
//		Calls GetTime passing NULL for both rt and mt. 
//
// Test Function: <nl>
// tdmperfvGetTimeValidTest1 <nl>
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
// 2) Create Performance object <nl>
// 3) Init the Performance <nl>
// 4) Call GetTime() and check results <nl>
// 5) Release objects, CoUninitialize, and return results <nl>
//
// History: <nl>
//  05/01/1998 - a-llucar - created <nl>
//
// @index topic | IDIRECTMUSICPERFORMANCE_CASES
//--------------------------------------------------------------------------;
DWORD tdmperfvGetTimeValidTest1(BOOL fCoCreate, BOOL fMaxDebug, 
						   LPARAM dwParam1, LPARAM dwParam2)
{
	DWORD						dwRes			= FNS_PASS;
	HRESULT						hr				= S_OK;
	CtIDirectMusicPerformance	*pwPerformance	= NULL;
	CtIDirectMusic					*pwMusic			= NULL;
	
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
    // 4) Call GetTime() and check results
    // **********************************************************************
	hr = pwPerformance->GetTime(NULL, NULL);
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
// @topic IDirectMusicPerformance::Init()_Valid_Test |
//
// Description: <nl>
// Valid test function for IDirectMusicPerformance::Init().
//	Call Init passing in an empty DirectMusic object
//
// Test Function: <nl>
// tdmperfvInitValidTest1 <nl>
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
// 2) Create CtIDirectMusicPerformance object <nl>
// 3) Call function to be tested, and check results<nl>
// 4) Release objects, dmthCoUninitialize, and return results <nl>
//
// History: <nl>
//  05/01/1998 - a-llucar - created <nl>
//
// @index topic | IDIRECTMUSICPERFORMANCE_CASES
//--------------------------------------------------------------------------;
DWORD tdmperfvInitValidTest1(BOOL fCoCreate, BOOL fMaxDebug, 
						   LPARAM dwParam1, LPARAM dwParam2)
{
	DWORD						dwRes			= FNS_PASS;	
	HRESULT						hr				= S_OK;
	CtIDirectMusicPerformance	*pwPerformance	= NULL;
	CtIDirectMusic				*pwMusic		= NULL;

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
    // 3) Call the function to be tested 
    // **********************************************************************
	hr = pwPerformance->Init(&pwMusic,NULL,ghWnd);
	if(FAILED(hr))
	{
        fnsLog(BUGLOGLEVEL, TEXT("!!!! BUG: IDirectMusicPerformance->Init ")
                TEXT("failed (%s == %08Xh)"),
                tdmXlatHRESULT(hr), hr);
		dwRes = FNS_FAIL;
		goto TEST_END;
	}
	//XXXXX		

TEST_END:
    // **********************************************************************
    // 4 Release objects, CoUninitialize, and return result
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
}	// End IDirectMusicPerformance->Init() Test


//--------------------------------------------------------------------------;
// @doc IDIRECTMUSICPERFORMANCE A-LLUCAR
//
// @topic IDirectMusicPerformance::MusicToReferenceTime()_Valid_Test |
//
// Description: <nl>
// Valid test function for IDirectMusicPerformance::MusicToReferenceTime(). <nl>
//		Call MusicToReferenceTime without first Initing the performance. <nl>
//		Expects to fail! <nl>
//
// Test Function: <nl>
// tdmperfvMusicToReferenceTimeValidTest1 <nl>
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
// 2) Create CtIDirectMusicPerformance object <nl>
// 3) Call MusicToReferenceTime and check results (EXPECT DMUS_E_NO_MASTER_CLOCK) <nl>
// 4) Cleanup objects, uninitialize COM and return results <nl>
//
// History: <nl>
//  05/01/1998 - a-llucar - created <nl>
//
// @index topic | IDIRECTMUSICPERFORMANCE_CASES
//--------------------------------------------------------------------------;
DWORD tdmperfvMusicToReferenceTimeValidTest1(BOOL fCoCreate, BOOL fMaxDebug, 
						   LPARAM dwParam1, LPARAM dwParam2)
{
	DWORD						dwRes			= FNS_PASS;
	HRESULT						hr				= S_OK;
	CtIDirectMusicPerformance	*pwPerformance	= NULL;
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
    // 3) Call MusicToReferenceTime and check results
    // **********************************************************************
	hr = pwPerformance->MusicToReferenceTime( 1000, &rt );

	// EXPECT FAIL
	fnsLog(FYILOGLEVEL, TEXT("NOTE:  WE EXPECT THIS TO FAIL!"));
	if(hr != DMUS_E_NO_MASTER_CLOCK)
	{
        fnsLog(BUGLOGLEVEL, TEXT("!!!! BUG: IDirectMusicPerformance->MusicToReference ")
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

	dmthCoUninitialize();	

	return dwRes;
}
//===========================================================================


//--------------------------------------------------------------------------;
// @doc IDIRECTMUSICPERFORMANCE A-LLUCAR
//
// @topic IDirectMusicPerformance::ReferenceToMusicTime()_Valid_Test |
//
// Description: <nl>
// Valid test function for IDirectMusicPerformance::ReferenceToMusicTime(). <nl>
//		Call ReferenceToMusicTime without first Initing the performance. <nl>
//		Expects to fail! <nl>
//
// Test Function: <nl>
// tdmperfvReferenceToMusicTimeTest <nl>
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
// 2) Create CtIDirectMusicPerformance object <nl>
// 3) Call ReferenceToMusicTime and check results (EXPECT DMUS_E_NO_MASTER_CLOCK) <nl>
// 4) Cleanup objects, uninitialize COM and return results <nl>
//
// History: <nl>
//  05/01/1998 - a-llucar - created <nl>
//
// @index topic | IDIRECTMUSICPERFORMANCE_CASES
//--------------------------------------------------------------------------;
DWORD tdmperfvReferenceToMusicTimeValidTest1(BOOL fCoCreate, BOOL fMaxDebug, 
						   LPARAM dwParam1, LPARAM dwParam2)
{
	DWORD						dwRes			= FNS_PASS;
	HRESULT						hr				= S_OK;
	CtIDirectMusicPerformance	*pwPerformance	= NULL;
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
    // 3) Call ReferenceToMusicTime and check results
    // **********************************************************************
	hr = pwPerformance->ReferenceToMusicTime( 1000, &mt );

	// EXPECT FAIL
	fnsLog(FYILOGLEVEL, TEXT("NOTE:  WE EXPECT THIS TO FAIL!"));
	if(hr != DMUS_E_NO_MASTER_CLOCK)
	{
        fnsLog(BUGLOGLEVEL, TEXT("!!!! BUG: IDirectMusicPerformance->ReferenceToMusicTime ")
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

	dmthCoUninitialize();	

	return dwRes;
}
//===========================================================================


//--------------------------------------------------------------------------;
// @doc IDIRECTMUSICPERFORMANCE A-LLUCAR
//
// @topic IDirectMusicPerformance::GetLatencyTime()_Valid_Test1 |
//
// Description: <nl>
// Unit test function for IDirectMusicPerformance::GetLatencyTime(). <nl>
//		Calls GetLatencyTime through SYNTH port, checks to see if it returns the same as GetTime()<nl>
//
// Test Function: <nl>
// tdmperfvGetLatencyTimeValidTest1 <nl>
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
// 2) Create CtIDirectMusicPerformance object <nl>
// 3) Call PlayMidiFile() convienence function - SYNTH port <nl>
// 4) Call GetTime and check results <nl>
// 5) Call GetLatencyTime and check results <nl>
// 6) Release objects, CoUninitialize, and return results <nl>
//
// History: <nl>
//  05/01/1998 - a-llucar - created <nl>
//
// @index topic | IDIRECTMUSICPERFORMANCE_CASES
//--------------------------------------------------------------------------;
DWORD tdmperfvGetLatencyTimeValidTest1(BOOL fCoCreate, BOOL fMaxDebug, 
						   LPARAM dwParam1, LPARAM dwParam2)
{
	DWORD						dwRes				= FNS_PASS;
	HRESULT						hr					= S_OK;
	CtIDirectMusicPerformance	*pwPerformance		= NULL;
	CtIDirectMusicSegment		*pwSegment			= NULL;
	REFERENCE_TIME				rtGetTime			= 0;
	REFERENCE_TIME				rtGetLatencyTime	= 0;
	int							GetDiff				= 0;
	WCHAR						*pMidiFile			= (WCHAR*)dwParam1;

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
    // 3) Call PlayMidiFile() convienence function - SYNTH port
    // **********************************************************************
	hr = dmthPlayMidiFile(pwPerformance,CTIID_IDirectMusicSegment,(CtIDirectMusicSegment**)&pwSegment,(CtIDirectMusicSegmentState**)NULL, TRUE, pMidiFile);
	if(hr != S_OK)
	{
        fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: PlayMidiFile convienence function "),
                TEXT("failed (%s == %08Xh)"),
                tdmXlatHRESULT(hr), hr);
		dwRes = FNS_ABORTED;
		goto TEST_END;
	}

    // **********************************************************************
    // 4) Call GetTime and check results
    // **********************************************************************
	hr = pwPerformance->GetTime(&rtGetTime, NULL);
	if(FAILED(hr))
	{
        fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: pwPerformance->GetTime ")
                TEXT("failed (%s == %08Xh)"),
                tdmXlatHRESULT(hr), hr);
		dwRes = FNS_ABORTED;
		goto TEST_END;
	}

    // **********************************************************************
    // 5) Call GetLatencyTime and check results
    // **********************************************************************
	hr = pwPerformance->GetLatencyTime(&rtGetLatencyTime);
	if(FAILED(hr))
	{
        fnsLog(BUGLOGLEVEL, TEXT("!!!! BUG: pwPerformance->GetLatencyTime ")
                TEXT("failed (%s == %08Xh)"),
                tdmXlatHRESULT(hr), hr);
		dwRes = FNS_FAIL;
		goto TEST_END;
	}
	 
	// Check to see if rtGetTime and rtGetLatencyTime are close to each other
	GetDiff = (((int)rtGetLatencyTime - (int)rtGetTime) /10000); //converts to milliseconds
	if (GetDiff < -1000 || GetDiff > 1000)
	{
        fnsLog(BUGLOGLEVEL, TEXT("!!!! BUG: Expected GetLatencyTime not close to GetTime \n ")
                TEXT("The difference between GetLatencyTime and GetTime is == %i\n"),
				GetDiff);
		dwRes = FNS_FAIL;
		goto TEST_END;
	}
	Sleep(1000);

TEST_END:
    // **********************************************************************
    // 6) Release objects, CoUninitialize, and check results
    // **********************************************************************
	if(pwPerformance)
	{
		pwPerformance->Stop(NULL, NULL, 0, 0);
		pwPerformance->CloseDown();
		pwPerformance->Release();
		pwPerformance = NULL;
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
// @topic IDirectMusicPerformance::PlaySegment()_Valid_Test |
//
// Description: <nl>
// Valid test function for IDirectMusicPerformance::PlaySegment(). <nl>
//		Plays a Segment through a MIDI port.
//
// Test Function: <nl>
// tdmperfvPlaySegmentValidTest1 <nl>
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
// 2) Create CtIDirectMusicPerformance object (wrapped IDMPerformance object) <nl>
// 3) Call dmthPlayMidiFile and check results (Through MIDI port) <nl>
// 4) Release objects, CoUninitialize, and return results <nl>
//
// History: <nl>
//  05/01/1998 - a-llucar - created <nl>
//
// @index topic | IDIRECTMUSICPERFORMANCE_CASES
//--------------------------------------------------------------------------;
DWORD tdmperfvPlaySegmentValidTest1(BOOL fCoCreate, BOOL fMaxDebug, 
						   LPARAM dwParam1, LPARAM dwParam2)
{
	DWORD						dwRes			= FNS_PASS;
	HRESULT						hr				= S_OK;
	CtIDirectMusicPerformance	*pwPerformance	= NULL;
	CtIDirectMusicSegment		*pwSegment		= NULL;

	WCHAR						*pMidiFile		= (WCHAR*)dwParam1;

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
        fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: DirectMusicPerformance object creation "),
                TEXT("failed (%s == %08Xh)"),
                tdmXlatHRESULT(hr), hr);
		dwRes = FNS_ABORTED;
		goto TEST_END;
	}

    // **********************************************************************
    // 3) Call PlayMidiFile() convienence function (Through MIDI port)
    // **********************************************************************
	hr = dmthPlayMidiFile(pwPerformance,CTIID_IDirectMusicSegment,(CtIDirectMusicSegment**)&pwSegment,(CtIDirectMusicSegmentState**)NULL, FALSE, pMidiFile);
	if(hr != S_OK)
	{
        fnsLog(BUGLOGLEVEL, TEXT("!!!! BUG: dmthPlayMidiFile convienence function failed "),
                TEXT("(%s == %08Xh)"),
                tdmXlatHRESULT(hr), hr);
		dwRes = FNS_FAIL;
		goto TEST_END;
	}
	Sleep(1000);

TEST_END:
    // **********************************************************************
    // 5) Cleanup objects and uninitialize COM and Return results
    // **********************************************************************
	if (pwPerformance)
	{
		pwPerformance->Stop(NULL, NULL, 0, 0);
		pwPerformance->CloseDown();
		pwPerformance->Release();
        pwPerformance = NULL;
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
// @topic IDirectMusicPerformance::Stop()_Valid_Test |
//
// Description: <nl>
// Valid test function for IDirectMusicPerformance::Stop(). <nl>
//		Calls Stop on a SegmentState instead of a Segment
//
// Test Function: <nl>
// tdmperfvStopValidTest1 <nl>
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
// 2) Create CtIDirectMusicPerformance object <nl>
// 3) Call PlayMidiFile() convienence function <nl>
// 4) Calls Segment->IsPlaying() on the SegmentState and checks results <nl>
// 4) Calls Segment->Stop)_ on the SegmentState and check results <nl>
// 5) Release objects, CoUninitialize, and return results <nl>
//
// History: <nl>
//  04/24/1998 - a-llucar - created <nl>
//
//	Note:  Same test is used for tdmperfvStopValidTest1 and tdmperfvIsPlayingValidTest1 <nl>
//
// @index topic | IDIRECTMUSICPERFORMANCE_CASES
//--------------------------------------------------------------------------;
DWORD tdmperfvStopValidTest1(BOOL fCoCreate, BOOL fMaxDebug, 
						   LPARAM dwParam1, LPARAM dwParam2)
{
	DWORD						dwRes				= FNS_PASS;
	HRESULT						hr					= S_OK;
	CtIDirectMusicSegment		*pwSegment			= NULL;
	CtIDirectMusicSegmentState	*pwSegmentState		= NULL;
	CtIDirectMusicPerformance 	*pwPerformance		= NULL;
	MUSIC_TIME					mt					= 1024;

	WCHAR						*pMidiFile			= (WCHAR*)dwParam1;

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
	hr = dmthPlayMidiFile(pwPerformance,CTIID_IDirectMusicSegment,(CtIDirectMusicSegment**)&pwSegment,(CtIDirectMusicSegmentState**)&pwSegmentState, TRUE, pMidiFile);
	if(hr != S_OK)
	{
        fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: PlayMidiFile convienence function ")
                TEXT("failed (%s == %08Xh)"),
                tdmXlatHRESULT(hr), hr);
		dwRes = FNS_ABORTED;
		goto TEST_END;
	}

    // **********************************************************************
    // 4) Call Segment->IsPlaying() on SegmentState and checks results
    // **********************************************************************
	hr = pwPerformance->IsPlaying(NULL, pwSegmentState);
	if(FAILED(hr))
	{
		fnsLog(BUGLOGLEVEL, TEXT("!!!! BUG: Segment->IsPlaying (on SegmentState) ")
				TEXT("failed (%s == %08Xh)"),
				tdmXlatHRESULT(hr), hr);
		dwRes = FNS_FAIL;
		goto TEST_END;
	}

    // **********************************************************************
    // 5) Call Stop and check results
    // **********************************************************************
	Sleep(1000);
	hr = pwPerformance->Stop(NULL, pwSegmentState, 0, 0);
    if(FAILED(hr))
	{
        fnsLog(BUGLOGLEVEL, TEXT("!!!! BUG: IDirectMusicPerformance->Stop (on SegmentState) ")
                TEXT("failed (%s == %08Xh)"),
                tdmXlatHRESULT(hr), hr);
		dwRes = FNS_FAIL;
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
	if(pwSegmentState)
	{
		pwSegmentState->Release();
		pwSegmentState = NULL;
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
// @topic IDirectMusicPerformance::IsPlaying()_Valid_Test |
//
// Description: <nl>
// Valid test function for IDirectMusicPerformance::IsPlaying(). <nl>
//		Calls IsPlaying on a SegmentState instead of a Segment
//
// Test Function: <nl>
// tdmperfvStopValidTest1 <nl>
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
// 2) Create CtIDirectMusicPerformance object <nl>
// 3) Call PlayMidiFile() convienence function <nl>
// 4) Calls Segment->IsPlaying() on the SegmentState and checks results <nl>
// 4) Calls Segment->Stop)_ on the SegmentState and check results <nl>
// 5) Release objects, CoUninitialize, and return results <nl>
//
// History: <nl>
//  04/24/1998 - a-llucar - created <nl>
//
//	Note:  Same test is used for tdmperfvStopValidTest1 and tdmperfvIsPlayingValidTest1 <nl>
//
// @index topic | IDIRECTMUSICPERFORMANCE_CASES
//--------------------------------------------------------------------------;
// *********************** Uses tdmperfvStopValidTest1 *********************

//--------------------------------------------------------------------------;
// @doc IDIRECTMUSICPERFORMANCE A-LLUCAR
//
// @topic IDirectMusicPerformance::Stop()_Valid_Test |
//
// Description: <nl>
// Valid test function for IDirectMusicPerformance::Stop().
//		Calls GetSegmentState After Stop to make sure that it is released correctly by the stop.
//
// Test Function: <nl>
// tdmperfStopValidTest2 <nl>
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
// 2) Create CtIDirectMusicPerformance object <nl>
// 3) Call PlayMidiFile() convienence function <nl>
// 4) Call GetTime() and check results <nl>
// 5) Call Stop and check results <nl>
// 6) Call GetSegmentState and check resutls <nl>
// 7) Release objects, CoUninitialize, and return results <nl>
//
// History: <nl>
//  04/24/1998 - a-llucar - created <nl>
//
// @index topic | IDIRECTMUSICPERFORMANCE_CASES
//--------------------------------------------------------------------------;
DWORD tdmperfvStopValidTest2(BOOL fCoCreate, BOOL fMaxDebug, 
						   LPARAM dwParam1, LPARAM dwParam2)
{
	DWORD						dwRes				= FNS_PASS;
	HRESULT						hr					= S_OK;
	CtIDirectMusicSegment		*pwSegment			= NULL;
	CtIDirectMusicSegmentState	*pwSegmentState		= NULL;
	CtIDirectMusicPerformance 	*pwPerformance		= NULL;
	CtIDirectMusicSegmentState	*pwSegmentState2	= NULL;
	MUSIC_TIME					mt					= 1024;

	WCHAR						*pMidiFile			= (WCHAR*)dwParam1;

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
	hr = dmthPlayMidiFile(pwPerformance,CTIID_IDirectMusicSegment,(CtIDirectMusicSegment**)&pwSegment,(CtIDirectMusicSegmentState**)&pwSegmentState, TRUE, pMidiFile);
	if(hr != S_OK)
	{
        fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: PlayMidiFile convienence function "),
                TEXT("failed (%s == %08Xh)"),
                tdmXlatHRESULT(hr), hr);
		dwRes = FNS_ABORTED;
		goto TEST_END;
	}

    // **********************************************************************
    // 4) Call GetTime() and check results
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

    // **********************************************************************
    // 5) Call Stop and check results
    // **********************************************************************
	Sleep(2000);
	hr = pwPerformance->Stop(NULL, NULL, 0, 0);
    if(FAILED(hr))
	{
        fnsLog(BUGLOGLEVEL, TEXT("!!!! BUG: IDirectMusicPerformance->Stop ")
                TEXT("failed (%s == %08Xh)"),
                tdmXlatHRESULT(hr), hr);
		dwRes = FNS_FAIL;
		goto TEST_END;
	}

	// **********************************************************************
    // 6) Call GetSegmentState and check resutls
    // **********************************************************************
	fnsLog(FYILOGLEVEL, TEXT("**** FYI:  WE EXPECT THIS TO RETURN DMUS_E_NOT_FOUND"));
	hr = pwPerformance->GetSegmentState( &pwSegmentState2, mt + 15000);
	if(DMUS_E_NOT_FOUND != hr)
	{
		fnsLog(BUGLOGLEVEL, TEXT("!!!! BUG: Performance->GetSegmentState ")
				TEXT("did not return correctly! (Expected DMUS_E_NOT_FOUND) -- (%s == %08Xh)"),
				tdmXlatHRESULT(hr), hr);
		dwRes = FNS_FAIL;
		goto TEST_END;
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
//===========================================================================


//--------------------------------------------------------------------------;
// @doc IDIRECTMUSICPERFORMANCE A-LLUCAR
//
// @topic IDirectMusicPerformance::CloseDown()_Valid_Test |
//
// Description: <nl>
// Unit test function for IDirectMusicPerformance::CloseDown().
//		Call CloseDown without call to Stop of a playing Segment.
//
// Test Function: <nl>
// tdmperfvCloseDownValidTest2 <nl>
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
// 3) Call PlayMidiFile() convienence function<nl>
// 4) Call CloseDown(), and make sure everything is ok <nl>
// 5) Call IsPlaying(Segment) and make sure it's not playing (returns error) <nl>
// 6) Call IsPlaying(SegmentState) and make sure it's not playing (returns error) <nl>
// 7) Release objects, CoUninitialize, and return results <nl>
//
// History: <nl>
//	04-24-1998	a-llucar - created <nl>
//
// @index topic | IDIRECTMUSICPERFORMANCE_CASES
//--------------------------------------------------------------------------;
DWORD tdmperfvCloseDownValidTest2(BOOL fCoCreate, BOOL fMaxDebug, 
						   LPARAM dwParam1, LPARAM dwParam2)
{
	DWORD						dwRes				= FNS_PASS;
	HRESULT						hr					= S_OK;
	CtIDirectMusicSegment		*pwSegment			= NULL;
	CtIDirectMusicSegmentState	*pwSegmentState		= NULL;
	CtIDirectMusicPerformance 	*pwPerformance		= NULL;
	CtIDirectMusicSegmentState	*pwSegmentState2	= NULL;
	MUSIC_TIME					mt					= 1024;

	WCHAR						*pMidiFile			= (WCHAR*)dwParam1;

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
	hr = dmthPlayMidiFile(pwPerformance,CTIID_IDirectMusicSegment,(CtIDirectMusicSegment**)&pwSegment,(CtIDirectMusicSegmentState**)&pwSegmentState, TRUE, pMidiFile);
	if(hr != S_OK)
	{
        fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: PlayMidiFile convienence function "),
                TEXT("failed (%s == %08Xh)"),
                tdmXlatHRESULT(hr), hr);
		dwRes = FNS_ABORTED;
		goto TEST_END;
	}

    // **********************************************************************
    // 4) Call CloseDown and check results
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

    // **********************************************************************
    // 5) Call IsPlaying(Segment) and check results - EXPECTS FAILURE
    // **********************************************************************
	hr = pwPerformance->IsPlaying(pwSegment, NULL);
	fnsLog(FYILOGLEVEL, TEXT("**** FYI: WE EXPECT THIS TO FAIL!!! We've already called CloseDown()!"));
	if(hr !=DMUS_E_NO_MASTER_CLOCK)
	{
        fnsLog(BUGLOGLEVEL, TEXT("!!!! BUG: IDirectMusicPerformance->IsPlaying ")
                TEXT("did not return correctly (expected DMUS_E_NO_MASTER_CLOCK) (%s == %08Xh)"),
                tdmXlatHRESULT(hr), hr);
		dwRes = FNS_FAIL;
		goto TEST_END;
	}

    // **********************************************************************
    // 6) Call IsPlaying(SegmentState) and check results - EXPECTS FAILURE
    // **********************************************************************
	hr = pwPerformance->IsPlaying(NULL, pwSegmentState);
	fnsLog(FYILOGLEVEL, TEXT("**** FYI: WE EXPECT THIS TO FAIL!!! We've already called CloseDown()!"));
	if(hr !=DMUS_E_NO_MASTER_CLOCK)
	{
        fnsLog(BUGLOGLEVEL, TEXT("!!!! BUG: IDirectMusicPerformance->IsPlaying ")
                TEXT("did not return correctly (expected DMUS_E_NO_MASTER_CLOCK) (%s == %08Xh)"),
                tdmXlatHRESULT(hr), hr);
		dwRes = FNS_FAIL;
		goto TEST_END;
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
//===========================================================================


//--------------------------------------------------------------------------;
// @doc IDIRECTMUSICPERFORMANCE A-LLUCAR
//
// @topic IDirectMusicPerformance::AllocPMsg()_Valid_Test |
//
// Description: <nl>
// Valid test function for IDirectMusicPerformance::AllocPMsg().
//		AllocPMSG for Note, Curve, Tempo, and MIDI, SendPMsg,  and check results.
//
// Test Function: <nl>
// tdmperfvAllocPMsgValidTest1 <nl>
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
// 2) Create CtIDirectMusicPerformance object <nl>
// 3) Init the Performance <nl>
// 4) Call AllocPMsg for Note, Curve, Tempo, and MIDI messages and check result <nl>
// 5) Set up pMSG's for Note, Curve, Tempo, and MIDI<nl>
// 6) Call SendPMsg for Note, Curve, Tempo, and MIDI, and check result <nl>
// 7) Cleanup objects, uninitialize COM and return results <nl>
//
// History: <nl>
//  05/04/1998 - a-llucar - created <nl>
//
//	The same test is used for tdmperfvAllocPMsgValidTest1() and tdmperfvSendPMsgValidTest1() <nl>
//
// @index topic | IDIRECTMUSICPERFORMANCE_CASES
//--------------------------------------------------------------------------;
DWORD tdmperfvAllocPMsgValidTest1(BOOL fCoCreate, BOOL fMaxDebug, 
						   LPARAM dwParam1, LPARAM dwParam2)
{
	DWORD							dwRes			= FNS_PASS;
	HRESULT							hr				= S_OK;
	DMUS_NOTE_PMSG					*pNote			= NULL;
	DMUS_CURVE_PMSG					*pCurve			= NULL;
	DMUS_TEMPO_PMSG					*pTempo			= NULL;
	DMUS_MIDI_PMSG					*pMidi			= NULL;
	CtIDirectMusic					*pwMusic		= NULL;
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
		fnsLog(ABORTLOGLEVEL, TEXT("*** ABORT: Performance->Init ")
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
    // 4.1) Call AllocPMsg on pNote and check result
    // **********************************************************************
	hr = pwPerformance->AllocPMsg( sizeof(DMUS_NOTE_PMSG), (DMUS_PMSG**)&pNote );
	if(FAILED(hr))
	{
        fnsLog(BUGLOGLEVEL, TEXT("!!!! BUG: IDirectMusicPerformance->AllocPMsg(pNote) "),
                TEXT("failed (%s == %08Xh)"),
                tdmXlatHRESULT(hr), hr);
		dwRes = FNS_FAIL;
		goto TEST_END;
	}

	// Check to make sure that pNote was properly populated
	if(!pNote)
	{
        fnsLog(BUGLOGLEVEL, TEXT("!!!! BUG: AllocPMsg did not properly set pNote, \n"),
                TEXT("pNote == NULL"));
		dwRes = FNS_FAIL;
		goto TEST_END;
	}

	// **********************************************************************
    // 4.2) Call AllocPMsg on pCurve and check result
    // **********************************************************************
	hr = pwPerformance->AllocPMsg( sizeof(DMUS_CURVE_PMSG), (DMUS_PMSG**)&pCurve );
	if(FAILED(hr))
	{
        fnsLog(BUGLOGLEVEL, TEXT("!!!! BUG: IDirectMusicPerformance->AllocPMsg(pCurve) "),
                TEXT("failed (%s == %08Xh)"),
                tdmXlatHRESULT(hr), hr);
		dwRes = FNS_FAIL;
		goto TEST_END;
	}

	// Check to make sure that pCurve was properly populated
	if(!pCurve)
	{
        fnsLog(BUGLOGLEVEL, TEXT("!!!! BUG: AllocPMsg did not properly set pCurve, \n"),
                TEXT("pCurve == NULL"));
		dwRes = FNS_FAIL;
		goto TEST_END;
	}

	// **********************************************************************
    // 4.3) Call AllocPMsg on pTempo and check result
    // **********************************************************************
	hr = pwPerformance->AllocPMsg( sizeof(DMUS_TEMPO_PMSG), (DMUS_PMSG**)&pTempo );
	if(FAILED(hr))
	{
        fnsLog(BUGLOGLEVEL, TEXT("!!!! BUG: IDirectMusicPerformance->AllocPMsg(pTempo) "),
                TEXT("failed (%s == %08Xh)"),
                tdmXlatHRESULT(hr), hr);
		dwRes = FNS_FAIL;
		goto TEST_END;
	}

	// Check to make sure that pTempo was properly populated
	if(!pTempo)
	{
        fnsLog(BUGLOGLEVEL, TEXT("!!!! BUG: AllocPMsg did not properly set pTempo, \n"),
                TEXT("pTempo == NULL"));
		dwRes = FNS_FAIL;
		goto TEST_END;
	}

	// **********************************************************************
    // 4.4) Call AllocPMsg on pMidi and check result
    // **********************************************************************
	hr = pwPerformance->AllocPMsg( sizeof(DMUS_MIDI_PMSG), (DMUS_PMSG**)&pMidi);
	if(FAILED(hr))
	{
        fnsLog(BUGLOGLEVEL, TEXT("!!!! BUG: IDirectMusicPerformance->AllocPMsg(pMidi) "),
                TEXT("failed (%s == %08Xh)"),
                tdmXlatHRESULT(hr), hr);
		dwRes = FNS_FAIL;
		goto TEST_END;
	}

	// Check to make sure that pMidi was properly populated
	if(!pMidi)
	{
        fnsLog(BUGLOGLEVEL, TEXT("!!!! BUG: AllocPMsg did not properly set pMidi, \n"),
                TEXT("pMidi == NULL"));
		dwRes = FNS_FAIL;
		goto TEST_END;
	}

    // **********************************************************************
    // 5.1) Set pNote with a bunch of particulars
    // **********************************************************************	
	ZeroMemory( pNote, sizeof(DMUS_NOTE_PMSG) );
	pNote->bFlags		= DMUS_NOTEF_NOTEON;
	pNote->bDurRange	= 100;
	pNote->wMusicValue	= 50;
	pNote->bVelocity	= 127;
	pNote->dwType		= DMUS_PMSGT_NOTE;
	pNote->dwFlags		= DMUS_PMSGF_REFTIME;

    // **********************************************************************
    // 5.2) Set pCurve with a bunch of particulars
    // **********************************************************************	
	ZeroMemory( pCurve, sizeof(DMUS_CURVE_PMSG) );
	pCurve->bFlags			= 1;
	pCurve->mtDuration		= 1000;
	pCurve->mtOriginalStart	= 0;
	pCurve->mtResetDuration	= 100;
	pCurve->nStartValue		= 0;
	pCurve->nEndValue		= 1000;
	pCurve->nResetValue		= 50;
	pCurve->wMeasure		= 8;
	pCurve->nOffset			= 0;
	pCurve->bBeat			= 2;
	pCurve->bGrid			= 0;
	pCurve->bType			= DMUS_CURVET_CCCURVE;
	pCurve->bCurveShape		= DMUS_CURVES_EXP;
	pCurve->dwFlags			= DMUS_PMSGF_REFTIME;
	
    // **********************************************************************
    // 5.3) Set pTempo with a bunch of particulars
    // **********************************************************************	
	ZeroMemory( pTempo, sizeof(DMUS_TEMPO_PMSG) );
	pTempo->dblTempo		= 100;
	pTempo->dwFlags			= DMUS_PMSGF_REFTIME;

    // **********************************************************************
    // 5.4) Set pMidi with a bunch of particulars
    // **********************************************************************	
	ZeroMemory( pMidi, sizeof(DMUS_MIDI_PMSG) );
	pMidi->dwFlags			= DMUS_PMSGF_REFTIME;

    // **********************************************************************
    // 6.1) Call SendPMsg on pNote and check results
    // **********************************************************************	
	hr = pwPerformance->SendPMsg( (DMUS_PMSG*)pNote );
	if(FAILED(hr))
	{
        fnsLog(BUGLOGLEVEL, TEXT("!!!! BUG: IDirectMusicPerformance->SendPMsg(pNote) "),
                TEXT("failed (%s == %08Xh)"),
                tdmXlatHRESULT(hr), hr);
		dwRes = FNS_FAIL;
		goto TEST_END;
	}

    // **********************************************************************
    // 6.2) Call SendPMsg on pCurve and check results
    // **********************************************************************	
	hr = pwPerformance->SendPMsg( (DMUS_PMSG*)pCurve );
	if(FAILED(hr))
	{
        fnsLog(BUGLOGLEVEL, TEXT("!!!! BUG: IDirectMusicPerformance->SendPMsg(pCurve) "),
                TEXT("failed (%s == %08Xh)"),
                tdmXlatHRESULT(hr), hr);
		dwRes = FNS_FAIL;
		goto TEST_END;
	}
	
    // **********************************************************************
    // 6.3) Call SendPMsg on pTempo and check results
    // **********************************************************************	
	hr = pwPerformance->SendPMsg( (DMUS_PMSG*)pTempo );
	if(FAILED(hr))
	{
        fnsLog(BUGLOGLEVEL, TEXT("!!!! BUG: IDirectMusicPerformance->SendPMsg(pTempo) "),
                TEXT("failed (%s == %08Xh)"),
                tdmXlatHRESULT(hr), hr);
		dwRes = FNS_FAIL;
		goto TEST_END;
	}

    // **********************************************************************
    // 6.4) Call SendPMsg on pMidi and check results
    // **********************************************************************	
	hr = pwPerformance->SendPMsg( (DMUS_PMSG*)pMidi );
	if(FAILED(hr))
	{
        fnsLog(BUGLOGLEVEL, TEXT("!!!! BUG: IDirectMusicPerformance->SendPMsg(pMidi) "),
                TEXT("failed (%s == %08Xh)"),
                tdmXlatHRESULT(hr), hr);
		dwRes = FNS_FAIL;
		goto TEST_END;
	}

TEST_END:
    // **********************************************************************
    // 7) Cleanup objects and uninitialize COM and Return results
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
// @topic IDirectMusicPerformance::SendPMsg()_Valid_Test |
//
// Description: <nl>
// Valid test function for IDirectMusicPerformance::SendPMsg().
//		AllocPMSG for Note, Curve, Tempo, and MIDI, SendPMsg,  and check results.
//
// Test Function: <nl>
// tdmperfvAllocPMsgValidTest1 <nl>
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
// 2) Create CtIDirectMusicPerformance object <nl>
// 3) Init the Performance <nl>
// 4) Call AllocPMsg for Note, Curve, Tempo, and MIDI messages and check result <nl>
// 5) Set up pMSG's for Note, Curve, Tempo, and MIDI<nl>
// 6) Call SendPMsg for Note, Curve, Tempo, and MIDI, and check result <nl>
// 7) Cleanup objects, uninitialize COM and return results <nl>
//
// History: <nl>
//  05/04/1998 - a-llucar - created <nl>
//
//	The same test is used for tdmperfvAllocPMsgValidTest1() and tdmperfvSendPMsgValidTest1() <nl>
//
// @index topic | IDIRECTMUSICPERFORMANCE_CASES
//--------------------------------------------------------------------------;
// !!!!!!!!!!!!!!!uses tdmperfvAllocPMsgValidTest1 !!!!!!!!!!!!!!!!!!!!!!!!!




//--------------------------------------------------------------------------;
// @doc IDIRECTMUSICPERFORMANCE JIMMO
//
// @topic IDirectMusicPerformance::SendPMsg()_Valid_Test |
//
// Description: <nl>
// This test creates an instance of the CTDMTestTool and attaches it to
// a PMsg.  That PMsg is then sent via a call to Perf->SendPMsg.
// After a short sleep, FreePMsg is called and the refcount of the test
// tool is verfied to have been decremented by one (from the call to
// FreePMsg).  In this case, the TestTool is not used in the same context
// as a DirectMusicTool.  It's used because it implements the IUnknown
// interface.<nl>
//
// Test Function: <nl>
// tdmperfvSendPMsgWithCOMToolTest<nl>
//
// Parameters: <nl>
//  BOOL    fCoCreate   - unused <nl>
//  BOOL    fMaxDebug   - unused <nl>
//  DWORD   dwParam1    - DMUS_PMSG_<type><nl>
//  DWORD   dwParam2    - unused<nl>
//
// File : TDMPERFV.CPP
//
// Steps:<nl>
// 1) Initialize COM.<nl>
// 2) Create CtIDirectMusicPerformance object
// 3) Call Perf->Init<nl>
// 4) Create Test Tool -- which will be attached to PMsg<nl>
// 5) Performance->AllocPMsg and check results<nl>
// 6) Setup the PMSG and Call StampPMsg() then check results<nl>
// 7) Call Perf->SendPMsg with Test Tool attached in punkUser of PMsg<nl>
// 8) Call Perf->FreePMsg<nl>
// 9) Verify that the Object was released by the Performance->FreePMsg()<nl>
// 10) Release objects, CoUninitialize, and return results<nl>
//
// History:
//  09/11/1998 - jimmo - Created
//
// @index topic | IDIRECTMUSICPERFORMANCE_CASES
//--------------------------------------------------------------------------;
DWORD tdmperfvSendPMsgWithCOMToolTest(BOOL fCoCreate, BOOL fMaxDebug, LPARAM dwParam1, LPARAM dwParam2)
{
    DWORD                       dwRes           = FNS_PASS;
    HRESULT                     hr              = S_OK;
    CtIDirectMusicPerformance   *pCtPerformance = NULL;
    CtIDirectMusicSegment       *pCtSegment     = NULL;
    CtIDirectMusicGraph         *pCtGraph       = NULL;
    IDirectMusicGraph           *pGraph         = NULL;
        
    //The pPMsgTool will just act as a COM object and be sent through the Performance by attaching
    //it to a PMsg and calling Perf::SendPMsg to pipe it along
    CTDMTestTool                *pPMsgTool      = NULL;

    //The pGraphTool will actually be added to the performance graph, and will be responsible for
    //Calling Perf::FreePMsg, from within it's ProcessPMsg function.
    CTDMTestTool                *pGraphTool     = NULL;
    
    DMUS_PMSG                   *pdmusPMSG      = NULL;

    int                         nRefCount       = 0;
    BOOL                        fFreePMsg       = TRUE;
    BOOL                        fAuto           = TRUE;;

    // **********************************************************************
    // 1) Initialize COM.
    // **********************************************************************
    hr = dmthCoInitialize(NULL);
    if(FAILED(hr))
    {
        fnsLog(ABORTLOGLEVEL, "**** ABORT: COM initialization failed! (%s == %08Xh)",
                tdmXlatHRESULT(hr), hr);
        return FNS_ABORTED;
    }

    // **********************************************************************
    // X) Create CtSegment object
    // **********************************************************************
    hr = dmthLoadSegment(L"test.mid", &pCtSegment);
    if(FAILED(hr))
    {
        fnsLog(ABORTLOGLEVEL, "**** ABORT: DirectMusicSegment object creation "
                "failed (%s == %08Xh)", tdmXlatHRESULT(hr), hr);
        dwRes = FNS_ABORTED;
        goto TEST_END;
    }

    // **********************************************************************
    // X) Create CtIDirectMusicPerformance object 
    // **********************************************************************
    hr = dmthCreatePerformance(IID_IDirectMusicPerformance, &pCtPerformance);
    if(FAILED(hr))
    {
        fnsLog(ABORTLOGLEVEL, "**** ABORT: DirectMusicPerformance object creation "
                "failed (%s == %08Xh)", tdmXlatHRESULT(hr), hr);
        dwRes = FNS_ABORTED;
        goto TEST_END;
    }

    // **********************************************************************
    // 3) Call Perf->Init
    // **********************************************************************
    hr = pCtPerformance->Init(NULL, NULL, ghWnd);
    if ( FAILED( hr ))
    {
        fnsLog(ABORTLOGLEVEL, "**** ABORT: Performance->Init(NULL, NULL, ghWnd) "
               "failed (%s == %08Xh)", tdmXlatHRESULT(hr), hr);
        dwRes = FNS_ABORTED;
        goto TEST_END;
    }
 

    fAuto = TRUE;
    hr = pCtPerformance->SetGlobalParam( GUID_PerfAutoDownload, &fAuto, sizeof(BOOL) );
	if(FAILED(hr))
	{
		fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: Performance->SetGlobalParam(AutoDownload == TRUE) ")
				TEXT("failed (%s == %08Xh)"),
				tdmXlatHRESULT(hr), hr);
		dwRes = FNS_ABORTED;
		goto TEST_END;
	}

	hr = pCtPerformance->AddPort(NULL);
	if(FAILED(hr))
	{
        fnsLog(ABORTLOGLEVEL, "**** ABORT: pwPerformance->AddPort(NULL) "
                "failed (%s == %08Xh)", dmthXlatHRESULT(hr), hr);
		dwRes = FNS_ABORTED;
		goto TEST_END;
	}


    // **********************************************************************
    // 4) Create Test Tool -- which will be attached to PMsg
    // **********************************************************************
    pPMsgTool = new CTDMTestTool();

    if (NULL == pPMsgTool)
    {
        fnsLog(ABORTLOGLEVEL, "**** ABORT: failed to create tool (out of memory?)");
        dwRes = FNS_ABORTED;
        goto TEST_END;
    }

    // ***********************************************************************
    // X) Create Test Tool -- which will be inserted into the Performance
    //    graph, and will call FreePMsg upon getting a DMUS_PMSGT_USER message
    // ***********************************************************************
    pGraphTool = new CTDMTestTool();

    if (NULL == pGraphTool)
    {
        fnsLog(ABORTLOGLEVEL, "**** ABORT: failed to create tool (out of memory?)");
        dwRes = FNS_ABORTED;
        goto TEST_END;
    }

    // **********************************************************************
    // X) Setup pGraphTool so it will do it's thing
    // **********************************************************************
    fFreePMsg = TRUE;
    pGraphTool->SetParam(GUID_TestToolFreePMsg, &fFreePMsg, sizeof(BOOL));
    pGraphTool->AddPMSGType(DMUS_PMSGT_USER);

    // **********************************************************************
    // 2) Create Graph Object
    // **********************************************************************
    hr = dmthCreateGraph(IID_IDirectMusicGraph, &pCtGraph);
    if(FAILED(hr))
    {
        fnsLog(ABORTLOGLEVEL, "**** ABORT: dmthCreateGraph "
                "failed (%s == %08Xh)", tdmXlatHRESULT(hr), hr);
        dwRes = FNS_ABORTED;
        goto TEST_END;
    }

    // **********************************************************************
    // X) Get Real pointe to graph
    // **********************************************************************
    hr = pCtGraph->GetRealObjPtr( &pGraph );
    if (FAILED( hr ))
    {
        fnsLog(ABORTLOGLEVEL, "**** ABORT: pCtGraph->GetRealObjPtr() "
                "failed (%s == %08Xh)", tdmXlatHRESULT(hr), hr);
        dwRes = FNS_ABORTED;
        goto TEST_END;
    }

    if (NULL == pGraph)
    {
        fnsLog(ABORTLOGLEVEL, "**** ABORT: pCtGraph->GetRealObjPtr() "
                "returned a NULL pGraph pointer");
        dwRes = FNS_ABORTED;
        goto TEST_END;
    }


    // **********************************************************************
    // X) Insert pGraphTool into the graph
    // **********************************************************************
    hr = pGraph->InsertTool( (IDirectMusicTool*)pGraphTool, NULL, 0, 0 );
    if(FAILED( hr ))
    {
        fnsLog(ABORTLOGLEVEL, "**** ABORT: Graph->InsertTool "
                "failed (%s == %08Xh)", tdmXlatHRESULT(hr), hr);
        dwRes = FNS_ABORTED;
        goto TEST_END;
    }


    // **********************************************************************
    // X) Set the Performance's Graph
    // **********************************************************************
    hr = pCtPerformance->SetGraph( pCtGraph );
    if(FAILED(hr))
    {
        fnsLog(ABORTLOGLEVEL, "**** ABORT: Performance->SetGraph "
                "failed (%s == %08Xh)", tdmXlatHRESULT(hr), hr);
        dwRes = FNS_ABORTED;
        goto TEST_END;
    }

    // **********************************************************************
    // 5) Performance->AllocPMsg and check results
    // **********************************************************************
    hr = pCtPerformance->AllocPMsg( sizeof(DMUS_PMSG), &pdmusPMSG );
    if(FAILED(hr))
    {
        fnsLog(ABORTLOGLEVEL, "**** ABORT: Performance->AllocPMsg "
                "failed (%s == %08Xh)", tdmXlatHRESULT(hr), hr);
        dwRes = FNS_ABORTED;
        goto TEST_END;
    }

    // **********************************************************************
    // 6) Setup the PMSG and Call StampPMsg() then check results
    // **********************************************************************
    pdmusPMSG->dwType   = DMUS_PMSGT_USER; //which is a message the Unique Tool handles

    pdmusPMSG->dwFlags  = 0;   //Clear out the flags, so we know what we're setting
    pdmusPMSG->dwFlags  = DMUS_PMSGF_MUSICTIME | DMUS_PMSGF_TOOL_ATTIME;

    pdmusPMSG->mtTime   = 1500;
    pdmusPMSG->punkUser = pPMsgTool;

    nRefCount = pPMsgTool->m_cRef;      //Store Ref Cound before Sending PMsg
    pPMsgTool->AddRef();                //Needed because it's going to be released by FreeMsg

    // **********************************************************************
    // X) Call Perf->PlaySegment
    // **********************************************************************
    hr = pCtPerformance->PlaySegment(pCtSegment, 0, 0, NULL);
    if(FAILED(hr))
    {
        fnsLog(ABORTLOGLEVEL, "**** ABORT: Perf->PlaySegment "
                "failed (%s == %08Xh)", tdmXlatHRESULT(hr), hr);
        dwRes = FNS_ABORTED;
        goto TEST_END;
    }

    Sleep(1000);    //Wait a second before sending message

    // **********************************************************************
    // X) Call Perf->SendPMsg with Test Tool attached in punkUser of PMsg
    // **********************************************************************
    hr = pCtPerformance->SendPMsg(pdmusPMSG);
    if(FAILED(hr))
    {
        fnsLog(ABORTLOGLEVEL, "**** ABORT: Perf->SendPMsg "
                "failed (%s == %08Xh)", tdmXlatHRESULT(hr), hr);
        dwRes = FNS_ABORTED;
        goto TEST_END;
    }


	// **********************************************************************
    // 7) Set Notification
    // **********************************************************************
	hr = dmthGetNotification(pCtPerformance, GUID_NOTIFICATION_SEGMENT, DMUS_NOTIFICATION_MUSICSTOPPED, 20000);	
	if (FAILED(hr))
	{
        fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: dmthGetNotification ")
                TEXT("failed (%s == %08Xh)"), tdmXlatHRESULT(hr), hr);
		dwRes = FNS_ABORTED;
		goto TEST_END;
	}
	pCtPerformance->Stop(NULL, NULL, 0, 0);

    // **********************************************************************
    // 9) Verify that the Object was released by the Performance->FreePMsg()
    //    that was called in the pGraphTool's ProcessPMsg function
    // **********************************************************************
    if (nRefCount != pPMsgTool->m_cRef)
    {
        fnsLog(BUGLOGLEVEL, "!!!! BUG: Release not called on pPMsgTool");
        fnsLog(BUGLOGLEVEL, "     Expected RefCount:%d", nRefCount);
        fnsLog(BUGLOGLEVEL, "              RefCount:%d", pPMsgTool->m_cRef);
        dwRes = FNS_FAIL;
        goto TEST_END;
    }


TEST_END:
    // **********************************************************************
    // 10) Release objects, CoUninitialize, and return results
    // **********************************************************************
    if(pCtPerformance)
    {
        pCtPerformance->Stop(NULL, NULL, 0, 0);
        pCtPerformance->CloseDown();
    }

    if (pPMsgTool)
    {
        pPMsgTool->Release();
        pPMsgTool = NULL;
    }

    if (pGraph)
    {
        pGraph->Release();
        pGraph = NULL;
    }

    if (pCtGraph)
    {
        pCtGraph->Release();
        pCtGraph = NULL;
    }
	if (pGraphTool)
    {
        pGraphTool->Release();
        pGraphTool = NULL;
    }

    if(pCtPerformance)
    {
        pCtPerformance->Release();
        pCtPerformance = NULL;
    }
	if(pCtSegment)
	{
		pCtSegment->Release();
		pCtSegment = NULL;
	}
    dmthCoUninitialize();   
    return dwRes;
}


//--------------------------------------------------------------------------;
// @doc IDIRECTMUSICPERFORMANCE A-LLUCAR
//
// @topic IDirectMusicPerformance::SetBumperLength()_Valid_Test |
//
// Description: <nl>
// Unit test function for IDirectMusicPerformance::SetBumperLength().
//		Calls SetBumperLength of length 0, while Segment is being played.
//
// Test Function: <nl>
// tdmperfvSetBumperLengthValidTest1 <nl>
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
// 2) Create CtIDirectMusicPerformance object <nl>
// 3) Call dmthPlayMidiFile convienence function <nl>
// 4) Call SetBumperLength and Test the function <nl>
// 5) Call GetBumperLength and Test the function <nl>
// 6) Cleanup objects and uninitialize COM and Return results <nl>
//
// History: <nl>
//  05/04/1998 - a-llucar - created <nl>
//	The same test is used for tdmperfvGetBumperLengthValidTest1() and tdmperfvSetBumperLengthValidTest1() <nl>
//
// @index topic | IDIRECTMUSICPERFORMANCE_CASES
//--------------------------------------------------------------------------;
DWORD tdmperfvSetBumperLengthValidTest1(BOOL fCoCreate, BOOL fMaxDebug, 
						   LPARAM dwParam1, LPARAM dwParam2)
{
	DWORD						dwRes			= FNS_PASS;
	HRESULT						hr				= S_OK;
	CtIDirectMusicPerformance	*pwPerformance	= NULL;
	CtIDirectMusicSegment		*pwSegment		= NULL;
	DWORD						dwSet			= 0;
	DWORD						dwGet           = 2000;
	
	WCHAR						*pMidiFile			= (WCHAR*)dwParam1;

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
    // 3) Call dmthPlayMidiFile convienence function
    // **********************************************************************
	hr = dmthPlayMidiFile(pwPerformance,CTIID_IDirectMusicSegment,(CtIDirectMusicSegment**)&pwSegment,(CtIDirectMusicSegmentState**)NULL, TRUE, pMidiFile);
	if(hr != S_OK)
	{
        fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: PlayMidiFile convienence function "),
                TEXT("failed (%s == %08Xh)"),
                tdmXlatHRESULT(hr), hr);
		dwRes = FNS_ABORTED;
		goto TEST_END;
	}

    // **********************************************************************
    // 4) Call SetBumperLength and Test the function
    // **********************************************************************
	hr = pwPerformance->SetBumperLength(dwSet);
	if(FAILED(hr))
	{
        fnsLog(BUGLOGLEVEL, TEXT("!!!! BUG: IDirectMusicPerformance->SetBumperLength() ")
                TEXT("failed (%s == %08Xh)"),
                tdmXlatHRESULT(hr), hr);
		dwRes = FNS_FAIL;
		goto TEST_END;
	}

    // **********************************************************************
    // 5) Call GetBumperLength and Test the function
    // **********************************************************************
	hr = pwPerformance->GetBumperLength(&dwGet);
	if(FAILED(hr))
	{
        fnsLog(BUGLOGLEVEL, TEXT("!!!! BUG: IDirectMusicPerformance->GetBumperLength() ")
                TEXT("failed (%s == %08Xh)"),
                tdmXlatHRESULT(hr), hr);
		dwRes = FNS_FAIL;
		goto TEST_END;
	}

	// Test to make sure that dwSet and dwGet are the same
	if(dwSet != dwGet)
	{
        fnsLog(BUGLOGLEVEL, TEXT("!!!! BUG: GetPrepareTime not set correctly, ")
                TEXT("dwSet == %08Xh)"),
				TEXT("dwGet == %08Xh)"),
                dwSet, dwGet);
		dwRes = FNS_FAIL;
		goto TEST_END;
	}
	Sleep(2000);

TEST_END:
    // **********************************************************************
    // 6) Cleanup objects and uninitialize COM and Return results
    // **********************************************************************
	if(pwPerformance)
	{
		pwPerformance->Stop(NULL, NULL, 0, 0);
		pwPerformance->CloseDown();
		pwPerformance->Release();
        pwPerformance = NULL;
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
// @topic IDirectMusicPerformance::GetBumperLength()_Valid_Test |
//
// Description: <nl>
// Unit test function for IDirectMusicPerformance::GetBumperLength().
//		Calls GetBumperLength of length 0, while segment is being played.
//
// Test Function: <nl>
// tdmperfvSetBumperLengthValidTest1 <nl>
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
// 2) Create CtIDirectMusicPerformance object <nl>
// 3) Call dmthPlayMidiFile convienence function <nl>
// 4) Call SetBumperLength and Test the function <nl>
// 5) Call GetBumperLength and Test the function <nl>
// 6) Call Performance->Stop <nl>
// 7) Cleanup objects and uninitialize COM and Return results <nl>
//
// History: <nl>
//  05/04/1998 - a-llucar - created <nl>
//	The same test is used for tdmperfvGetBumperLengthValidTest1() and tdmperfvSetBumperLengthValidTest1() <nl>
//
// @index topic | IDIRECTMUSICPERFORMANCE_CASES
//--------------------------------------------------------------------------;
// !!!!!!!!!!!! uses tdmperfvSetBumperLengthValidTest1 !!!!!!!!!!!!!!!!!!!!!


//--------------------------------------------------------------------------;
// @doc IDIRECTMUSICPERFORMANCE A-LLUCAR
//
// @topic IDirectMusicPerformance::SetBumperLength()_Valid_Test |
//
// Description: <nl>
// Valid test function for IDirectMusicPerformance::SetBumperLength().
//		Calls SetBumperLength of length 50000, makes sure the PlaySegment doe not time out.
//
// Test Function: <nl>
// tdmperfvSetBumperLengthValidTest2 <nl>
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
// 2) Create CtIDirectMusicPerformance object <nl>
// 3) Call SetBumperLength and Test the function <nl>
// 4) Call dmthPlayMidiFile convienence function <nl>
// 5) Cleanup objects and uninitialize COM and Return results <nl>
//
// History: <nl>
//  05/04/1998 - a-llucar - created <nl>
//
// @index topic | IDIRECTMUSICPERFORMANCE_CASES
//--------------------------------------------------------------------------;
DWORD tdmperfvSetBumperLengthValidTest2(BOOL fCoCreate, BOOL fMaxDebug, 
						   LPARAM dwParam1, LPARAM dwParam2)
{
	DWORD						dwRes			= FNS_PASS;
	HRESULT						hr				= S_OK;
	CtIDirectMusicPerformance	*pwPerformance	= NULL;
	CtIDirectMusicSegment		*pwSegment		= NULL;
	DWORD						dwSet			= 50000000;
	
	WCHAR						*pMidiFile			= (WCHAR*)dwParam1;

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
    // 3) Load a Segment Object
    // **********************************************************************
	hr = dmthLoadSegment(L"DSegTest3.sgt", &pwSegment);
	if (FAILED(hr))
	{
		fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: dmthLoadSegment ")
				TEXT("failed (%s == %08Xh)"),
				tdmXlatHRESULT(hr), hr);
		dwRes = FNS_ABORTED;
		goto TEST_END;
	}

	// **********************************************************************
    // 4) Init and AddPort
    // **********************************************************************		
	hr = dmthPrePlay(&pwPerformance);
    if(FAILED(hr))
	{
        fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: dmthPrePlay ")
                TEXT("failed (%s == %08Xh)"),
                tdmXlatHRESULT(hr), hr);
		dwRes = FNS_ABORTED;
		goto TEST_END;
	}
    
	// **********************************************************************
    // 5) Call SetBumperLength and Test the function
    // **********************************************************************
	hr = pwPerformance->SetBumperLength(dwSet);
	if(FAILED(hr))
	{
        fnsLog(BUGLOGLEVEL, TEXT("!!!! BUG: IDirectMusicPerformance->SetBumperLength() ")
                TEXT("failed (%s == %08Xh)"),
                tdmXlatHRESULT(hr), hr);
		dwRes = FNS_FAIL;
		goto TEST_END;
	}

    // **********************************************************************
    // 6) Play the segment
    // **********************************************************************
	fnsLog(FYILOGLEVEL, TEXT("**** FYI:  Expecting this to time out.  If not, SetBumperLength did not work correctly!!!!!"));
	hr = pwPerformance->PlaySegment(pwSegment, 0, 0, NULL);
	if(FAILED(hr))
	{
        fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: Performance->PlaySegment ")
                TEXT("failed (%s == %08Xh)"),
                tdmXlatHRESULT(hr), hr);
		dwRes = FNS_ABORTED;
		goto TEST_END;
	}

    // **********************************************************************
    // 7) Set a notification for a measure/beat.  If it times out, we pass
    // **********************************************************************
	Sleep(2000);
	hr = pwPerformance->IsPlaying(pwSegment, NULL);
	if(FAILED(hr)) // means that the segment is playing, which means that the bumper length did not 
		// delay the start of the segment
	{
        fnsLog(BUGLOGLEVEL, TEXT("!!!! BUG: Performance->IsPlaying ")
                TEXT("did not fail (%s == %08Xh)"), tdmXlatHRESULT(hr), hr);
		dwRes = FNS_ABORTED;
		goto TEST_END;
	}

TEST_END:
    // **********************************************************************
    // 8) Cleanup objects and uninitialize COM and Return results
    // **********************************************************************
	if(pwPerformance)
	{
		pwPerformance->Stop(NULL, NULL, 0, 0);
		pwPerformance->CloseDown();
		pwPerformance->Release();
        pwPerformance = NULL;
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
// @topic IDirectMusicPerformance::FreePMsg()_Valid_Test |
//
// Description: <nl>
// Valid test function for IDirectMusicPerformance::FreePMsg().
//		AllocPMSG for Note, Curve, Tempo, and MIDI, FreePMsg,  and check results.
//
// Test Function: <nl>
// tdmperfvAllocPMsgValidTest1 <nl>
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
// 2) Create CtIDirectMusicPerformance object <nl>
// 3) Init the Performance <nl>
// 4) Call AllocPMsg for Note, Curve, Tempo, and MIDI messages and check result <nl>
// 5) Set up pMSG's for Note, Curve, Tempo, and MIDI<nl>
// 6) Call FreePMsg for Note, Curve, Tempo, and MIDI, and check result <nl>
// 7) Cleanup objects, uninitialize COM and return results <nl>
//
// History: <nl>
//  05/04/1998 - a-llucar - created <nl>
//
// @index topic | IDIRECTMUSICPERFORMANCE_CASES
//--------------------------------------------------------------------------;
DWORD tdmperfvFreePMsgValidTest1(BOOL fCoCreate, BOOL fMaxDebug, 
						   LPARAM dwParam1, LPARAM dwParam2)
{
	DWORD							dwRes			= FNS_PASS;
	HRESULT							hr				= S_OK;
	DMUS_NOTE_PMSG					*pNote			= NULL;
	DMUS_CURVE_PMSG					*pCurve			= NULL;
	DMUS_TEMPO_PMSG					*pTempo			= NULL;
	DMUS_MIDI_PMSG					*pMidi			= NULL;
	CtIDirectMusic					*pwMusic		= NULL;
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
		fnsLog(ABORTLOGLEVEL, TEXT("*** ABORT: Performance->Init ")
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
    // 4.1) Call AllocPMsg on pNote and check result
    // **********************************************************************
	hr = pwPerformance->AllocPMsg( sizeof(DMUS_NOTE_PMSG), (DMUS_PMSG**)&pNote );
	if(FAILED(hr))
	{
        fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: IDirectMusicPerformance->AllocPMsg(pNote) "),
                TEXT("failed (%s == %08Xh)"),
                tdmXlatHRESULT(hr), hr);
		dwRes = FNS_ABORTED;
		goto TEST_END;
	}

	// Check to make sure that pNote was properly populated
	if(!pNote)
	{
        fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: AllocPMsg did not properly populate pNote correctly, \n"),
                TEXT("pNote == NULL"));
		dwRes = FNS_ABORTED;
		goto TEST_END;
	}

	// **********************************************************************
    // 4.2) Call AllocPMsg on pCurve and check result
    // **********************************************************************
	hr = pwPerformance->AllocPMsg( sizeof(DMUS_CURVE_PMSG), (DMUS_PMSG**)&pCurve );
	if(FAILED(hr))
	{
        fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: IDirectMusicPerformance->AllocPMsg(pCurve) "),
                TEXT("failed (%s == %08Xh)"),
                tdmXlatHRESULT(hr), hr);
		dwRes = FNS_ABORTED;
		goto TEST_END;
	}

	// Check to make sure that pCurve was properly populated
	if(!pCurve)
	{
        fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: AllocPMsg did not properly populate pCurve correctly, \n"),
                TEXT("pCurve == NULL"));
		dwRes = FNS_ABORTED;
		goto TEST_END;
	}

	// **********************************************************************
    // 4.3) Call AllocPMsg on pTempo and check result
    // **********************************************************************
	hr = pwPerformance->AllocPMsg( sizeof(DMUS_TEMPO_PMSG), (DMUS_PMSG**)&pTempo );
	if(FAILED(hr))
	{
        fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: IDirectMusicPerformance->AllocPMsg(pTempo) "),
                TEXT("failed (%s == %08Xh)"),
                tdmXlatHRESULT(hr), hr);
		dwRes = FNS_ABORTED;
		goto TEST_END;
	}

	// Check to make sure that pTempo was properly populated
	if(!pTempo)
	{
        fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: AllocPMsg did not properly populate pTempo correctly, \n"),
                TEXT("pTempo == NULL"));
		dwRes = FNS_ABORTED;
		goto TEST_END;
	}

	// **********************************************************************
    // 4.4) Call AllocPMsg on pCurve and check result
    // **********************************************************************
	hr = pwPerformance->AllocPMsg( sizeof(DMUS_MIDI_PMSG), (DMUS_PMSG**)&pMidi);
	if(FAILED(hr))
	{
        fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: IDirectMusicPerformance->AllocPMsg(pMidi) "),
                TEXT("failed (%s == %08Xh)"),
                tdmXlatHRESULT(hr), hr);
		dwRes = FNS_ABORTED;
		goto TEST_END;
	}

	// Check to make sure that pCurve was properly populated
	if(!pMidi)
	{
        fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: AllocPMsg did not properly populate pMidi correctly, \n"),
                TEXT("pMidi == NULL"));
		dwRes = FNS_ABORTED;
		goto TEST_END;
	}

    // **********************************************************************
    // 5.1) Set pNote with a bunch of particulars
    // **********************************************************************	
	ZeroMemory( pNote, sizeof(DMUS_NOTE_PMSG) );
	pNote->bFlags		= DMUS_NOTEF_NOTEON;
	pNote->bDurRange	= 100;
	pNote->wMusicValue	= 50;
	pNote->bVelocity	= 127;
	pNote->dwType		= DMUS_PMSGT_NOTE;
	pNote->dwFlags		= DMUS_PMSGF_REFTIME;

    // **********************************************************************
    // 5.2) Set pCurve with a bunch of particulars
    // **********************************************************************	
	ZeroMemory( pCurve, sizeof(DMUS_CURVE_PMSG) );
	pCurve->bFlags			= 1;
	pCurve->mtDuration		= 1000;
	pCurve->mtOriginalStart	= 0;
	pCurve->mtResetDuration	= 100;
	pCurve->nStartValue		= 0;
	pCurve->nEndValue		= 1000;
	pCurve->nResetValue		= 50;
	pCurve->wMeasure		= 8;
	pCurve->nOffset			= 0;
	pCurve->bBeat			= 2;
	pCurve->bGrid			= 0;
	pCurve->bType			= DMUS_CURVET_CCCURVE;
	pCurve->bCurveShape		= DMUS_CURVES_EXP;
	pCurve->dwFlags			= DMUS_PMSGF_REFTIME;
	
    // **********************************************************************
    // 5.3) Set pTempo with a bunch of particulars
    // **********************************************************************	
	ZeroMemory( pTempo, sizeof(DMUS_TEMPO_PMSG) );
	pTempo->dblTempo		= 100;
	pTempo->dwFlags			= DMUS_PMSGF_REFTIME;

    // **********************************************************************
    // 5.4) Set pMidi with a bunch of particulars
    // **********************************************************************	
	ZeroMemory( pMidi, sizeof(DMUS_MIDI_PMSG) );
	pMidi->dwFlags			= DMUS_PMSGF_REFTIME;

    // **********************************************************************
    // 6.1) Call FreePMsg on pNote and check results
    // **********************************************************************	
	hr = pwPerformance->FreePMsg( (DMUS_PMSG*)pNote );
	if(FAILED(hr))
	{
        fnsLog(BUGLOGLEVEL, TEXT("!!!! BUG: IDirectMusicPerformance->FreePMsg(pNote) "),
                TEXT("failed (%s == %08Xh)"),
                tdmXlatHRESULT(hr), hr);
		dwRes = FNS_FAIL;
		goto TEST_END;
	}

    // **********************************************************************
    // 6.2) Call FreePMsg on pCurve and check results
    // **********************************************************************	
	hr = pwPerformance->FreePMsg( (DMUS_PMSG*)pCurve );
	if(FAILED(hr))
	{
        fnsLog(BUGLOGLEVEL, TEXT("!!!! BUG: IDirectMusicPerformance->FreePMsg(pCurve) "),
                TEXT("failed (%s == %08Xh)"),
                tdmXlatHRESULT(hr), hr);
		dwRes = FNS_FAIL;
		goto TEST_END;
	}
	
    // **********************************************************************
    // 6.3) Call FreePMsg on pTempo and check results
    // **********************************************************************	
	hr = pwPerformance->FreePMsg( (DMUS_PMSG*)pTempo );
	if(FAILED(hr))
	{
        fnsLog(BUGLOGLEVEL, TEXT("!!!! BUG: IDirectMusicPerformance->FreePMsg(pTempo) "),
                TEXT("failed (%s == %08Xh)"),
                tdmXlatHRESULT(hr), hr);
		dwRes = FNS_FAIL;
		goto TEST_END;
	}

    // **********************************************************************
    // 6.4) Call FreePMsg on pMidi and check results
    // **********************************************************************	
	hr = pwPerformance->FreePMsg( (DMUS_PMSG*)pMidi );
	if(FAILED(hr))
	{
        fnsLog(BUGLOGLEVEL, TEXT("!!!! BUG: IDirectMusicPerformance->FreePMsg(pMidi) "),
                TEXT("failed (%s == %08Xh)"),
                tdmXlatHRESULT(hr), hr);
		dwRes = FNS_FAIL;
		goto TEST_END;
	}

TEST_END:
    // **********************************************************************
    // 7) Cleanup objects and uninitialize COM and Return results
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
// @topic IDirectMusicPerformance::RemovePort()_Valid_Test |
//
// Description: <nl>
// Valid test function for IDirectMusicPerformance::RemovePort().
//		Remove a port that is currently playing.
//
// Test Function: <nl>
// tdmperfvRemovePortValidTest1 <nl>
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
// 1) dmthCoInitialize <nl>
// 2) Create CtIDirectMusicPerformance object <nl>
// 3) Get the Segment object <nl>
// 4) Init the performance <nl>
// 5) Create a SynthPort <nl>
// 6) Add a port to our two performances <nl>
// 7) Call PlaySegment and check results <nl>
// 8) Make sure the segment is playing <nl>
// 9) RemovePort and check results <nl>
// 10) Release objects, CoUninitilalize, and return results <nl>
//
// History: <nl>
//  07/29/1998 - a-llucar - created <nl>
//
// @index topic | IDIRECTMUSICPERFORMANCE_CASES
//--------------------------------------------------------------------------;
DWORD tdmperfvRemovePortValidTest1(BOOL fCoCreate, BOOL fMaxDebug, 
						   LPARAM dwParam1, LPARAM dwParam2)
{
    DWORD                       dwRes			= FNS_PASS;
    HRESULT                     hr				= S_OK;
    CtIDirectMusicPerformance   *pwPerformance	= NULL;
    CtIDirectMusicSegment       *pwSegment		= NULL;
    CtIDirectMusicPort          *pwPort			= NULL;
    CtIDirectMusic              *pwDM			= NULL;
    DMUS_PORTPARAMS             dmusPortParams;
    DWORD                       dwCounter       = 0;
    DWORD                       dwTimeOutValue  = 20;
    GUID                        guidSink;

	WCHAR						*pSegmentFile	= (WCHAR*)dwParam1;

    // **********************************************************************
    // 1) Initialize COM.
    // **********************************************************************
    hr = dmthCoInitialize(NULL);
    if(FAILED(hr))
    {
        fnsLog(ABORTLOGLEVEL, "**** ABORT: COM initialization failed! (%s == %08Xh)",
                tdmXlatHRESULT(hr), hr);
        return FNS_ABORTED;
    }

    // **********************************************************************
    // 2) Create CtIDirectMusicPerformance object 
    // **********************************************************************
    hr = dmthCreatePerformance(IID_IDirectMusicPerformance, &pwPerformance);
    if(FAILED(hr))
    {
        fnsLog(ABORTLOGLEVEL, "**** ABORT: DirectMusicPerformance object creation "
                "failed (%s == %08Xh)", tdmXlatHRESULT(hr), hr);
        dwRes = FNS_ABORTED;
        goto TEST_END;
    }

    // **********************************************************************
    // 3) Load the segment
    // **********************************************************************
    hr = dmthLoadSegment(pSegmentFile, &pwSegment);
    if (FAILED( hr ))
    {
        fnsLog(ABORTLOGLEVEL, "**** ABORT: dmthLoadSegment "
                "failed (%s == %08Xh)", tdmXlatHRESULT(hr), hr);
        dwRes = FNS_ABORTED;
        goto TEST_END;
    }

    // **********************************************************************
    // 4) Init the performance
    // **********************************************************************
    hr = pwPerformance->Init(&pwDM,NULL,ghWnd);
    if(FAILED(hr))
    {
        fnsLog(ABORTLOGLEVEL, "**** ABORT: Performance->Init "
               "failed (%s == %08Xh)", tdmXlatHRESULT(hr), hr);
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
    // 5) Create a SynthPort
    // **********************************************************************
    ZeroMemory( &dmusPortParams, sizeof(DMUS_PORTPARAMS) );
    dmusPortParams.dwSize          = sizeof(DMUS_PORTPARAMS);  
    dmusPortParams.dwChannelGroups = 5; // create 5 channel groups on the port
    dmusPortParams.dwValidParams   = DMUS_PORTPARAMS_CHANNELGROUPS;

    ZeroMemory( &guidSink, sizeof(GUID) );

    hr = pwDM->CreatePort(
		CLSID_DirectMusicSynth, 
        &dmusPortParams, 
        &pwPort, 
        NULL );
    if ( FAILED(hr) )
    {
        fnsLog(ABORTLOGLEVEL, "**** ABORT: pwDM->CreatePort "
                "failed (%s == %08Xh)", tdmXlatHRESULT(hr), hr);
        dwRes = FNS_ABORTED;
        goto TEST_END;
    }

    // **********************************************************************
    // 6) Add a port to our two performances
    // **********************************************************************
    hr = pwPerformance->AddPort(pwPort);
    if(FAILED(hr))
    {
        fnsLog(ABORTLOGLEVEL, "**** ABORT: pwPerformance->AddPort(NULL) "
                "failed (%s == %08Xh)", tdmXlatHRESULT(hr), hr);
        dwRes = FNS_ABORTED;
        goto TEST_END;
    }
    pwPerformance->AssignPChannelBlock( 0, pwPort, 1 );

    // **********************************************************************
    // 7) Call PlaySegment and check results
    // **********************************************************************
    hr = pwPerformance->PlaySegment(pwSegment, 0, 0, NULL);
    if(FAILED(hr))
    {
        fnsLog(ABORTLOGLEVEL, "**** ABORT: pwPerformance->PlaySegment "
                "failed (%s == %08Xh)", tdmXlatHRESULT(hr), hr);
        dwRes = FNS_ABORTED;
        goto TEST_END;
    }

    // **********************************************************************
    // 8) Wait for IsPlaying to return or time out
    // **********************************************************************
    dwCounter = 0;
    while ((hr = pwPerformance->IsPlaying(pwSegment, NULL)) != S_OK)  
    {
        Sleep(1000);
        if(dwCounter++ > dwTimeOutValue)
        {
            fnsLog(ABORTLOGLEVEL, "**** ABORT: PlaySegment() timed out"
                "IsPlaying did not return the Segment (%s == %08Xh)", tdmXlatHRESULT(hr), hr);
            dwRes = FNS_ABORTED;
            goto TEST_END;
        }
    }

    // **********************************************************************
    // 9) Call RemovePort and check results
    // **********************************************************************
	hr = pwPerformance->RemovePort(pwPort);
    if(FAILED(hr))
    {
        fnsLog(BUGLOGLEVEL, "!!!! BUG: pwPerformance->RemovePort "
                "failed (%s == %08Xh)", tdmXlatHRESULT(hr), hr);
        dwRes = FNS_FAIL;
        goto TEST_END;
    }
	Sleep(1000);

TEST_END:
    // **********************************************************************
    // 10) Release objects, CoUninitialize, and return results
    // **********************************************************************
    if(pwPerformance)
    {
        pwPerformance->Stop(NULL, NULL, 0, 0);
        pwPerformance->CloseDown();
        pwPerformance->Release();
        pwPerformance = NULL;
    }
    if(pwSegment)
    {
        pwSegment->Release();
        pwSegment = NULL;
    }
    if(pwPort)
    {
        pwPort->Release();
        pwPort = NULL;
    }
    if(pwDM)
    {
        pwDM->Release();
        pwDM = NULL;
    }
	dmthCoUninitialize();   
    return dwRes;
}


//--------------------------------------------------------------------------;
// @doc IDIRECTMUSICPERFORMANCE A-LLUCAR
//
// @topic IDirectMusicPerformance::DownloadInstrumentTest()_Valid_Test |
//
// Description: <nl>
// Valid test function for IDirectMusicPerformance::DownloadInstrument(). <nl>
//			Checks NoteRange information when passing in NULL. <nl>
//
// Test Function: <nl>
// tdmperfvDownloadInstrumentValidTest1 <nl>
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
// 2) Create CtIDirectMusicPerformance object <nl>
// 3) Create CtIDirectMusicLoader object<nl>
// 4) Get the Object from the loader<nl>
// 5) Do QI on the object for the collection <nl>
// 6) Create a wrapped Instrument Collection <nl>
// 7) Create a wrapper for IDirectMusicCollection Object <nl>
// 8) Get an instrument collection and release the collection <nl>
// 9) Call Init(NULL) and AddPort(NULL) to set everything up for downloading <nl>
// 10) Set DMUS_NoteRanges array information <nl>
// 11) Call Download and check results <nl>
// 12) Release object, CoUninitialize, and return results <nl>
//
// History: <nl>
//  05/08/1998 - a-llucar - created <nl>
//
// @index topic | IDIRECTMUSICPERFORMANCE_CASES
//--------------------------------------------------------------------------;
DWORD tdmperfvDownloadInstrumentValidTest1(BOOL fCoCreate, BOOL fMaxDebug, 
						   LPARAM dwParam1, LPARAM dwParam2)
{
	DWORD								dwRes			= FNS_PASS;
	HRESULT								hr				= S_OK;
	CtIDirectMusicLoader				*pIDMLoader		= NULL;
	CtIDirectMusicCollection			*pwCollection	= NULL;
	CtIDirectMusicInstrument			*pwInstrument	= NULL;
	CtIDirectMusicDownloadedInstrument  *pwDownloaded	= NULL;
	CtIDirectMusicPerformance			*pwPerformance	= NULL;
	CtIDirectMusicPort					*pwPort			= NULL;
	DMUS_OBJECTDESC						desc; 
	DMUS_NOTERANGE						pNoteRanges[3];
	ZeroMemory(&desc, sizeof(desc));
	desc.dwSize											= sizeof(DMUS_OBJECTDESC);
	DWORD								dwGroup			= 0;
	DWORD								dwMChannel		= 0;
	WCHAR								*pDLS			= (WCHAR*)dwParam1;

	dmthSetPath(NULL);
	WCHAR								*wchPath		= gwszMediaPath;
	DWORD								dwPatch			= (DWORD)dwParam2;	

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
				TEXT("failed (%s == %08Xh)"),
				tdmXlatHRESULT(hr), hr);
		dwRes = FNS_ABORTED;
		goto TEST_END;
	}

	desc.guidClass = CLSID_DirectMusicCollection;
	desc.dwValidData |= (DMUS_OBJ_CLASS | DMUS_OBJ_FILENAME );
	wcscpy( desc.wszFileName, pDLS);

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
	hr = pwCollection->GetInstrument( dwPatch, &pwInstrument );
	if(FAILED(hr))
	{
        fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: Collection->GetInstrument ")
                TEXT("failed (%s == %08Xh)"),
                tdmXlatHRESULT(hr), hr);
		dwRes = FNS_ABORTED;
		goto TEST_END;
	}

    // **********************************************************************
    // 9) Call Init(NULL) and AddPort(NULL) 
    // **********************************************************************	
	hr = pwPerformance->Init(NULL,NULL,ghWnd);
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

	hr = pwPerformance->AddPort(NULL); 
	if(FAILED(hr))
	{
        fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: pwPerformance->Init ")
                TEXT("failed (%s == %08Xh)"),
                tdmXlatHRESULT(hr), hr);
		dwRes = FNS_ABORTED;
		goto TEST_END;
	}

	// **********************************************************************
    // 10) Set NoteRange information
    // **********************************************************************	
    ZeroMemory(&pNoteRanges, sizeof(DMUS_NOTERANGE));
	  	pNoteRanges[0].dwLowNote	= 0;
		pNoteRanges[0].dwHighNote	= 100;
		pNoteRanges[1].dwLowNote	= 110;
		pNoteRanges[1].dwHighNote	= 115;
		pNoteRanges[2].dwLowNote	= 116;
		pNoteRanges[2].dwHighNote	= 127;
    
	// **********************************************************************
    // 11) Call Download and check results
    // **********************************************************************	
	hr = pwPerformance->DownloadInstrument( pwInstrument, 
											0, 
											&pwDownloaded, 
											&pNoteRanges[1],
											1,
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
			pwPort->Release();
			pwPort = NULL;
		}
		pwDownloaded->Release();
		pwDownloaded = NULL;
	}

	// Pass NULL, set the dwNumNoteRanges != 0, expect FAIL
	hr = pwPerformance->DownloadInstrument( pwInstrument, 
											0, 
											&pwDownloaded, 
											NULL,
											3,
											&pwPort, 
											&dwGroup,
											&dwMChannel );
	if(SUCCEEDED(hr))
	{
        fnsLog(BUGLOGLEVEL, TEXT("!!!! BUG: pwPerformance->DownloadInstrument (Passing NULL) ")
                TEXT("did not failed (%s == %08Xh)"),
                tdmXlatHRESULT(hr), hr);
		dwRes = FNS_FAIL;
		goto TEST_END;
	}

	// Make sure it handles setting dwNumNoteRanges == 0
	hr = pwPerformance->DownloadInstrument( pwInstrument, 
											0, 
											&pwDownloaded, 
											&pNoteRanges[2],
											0,
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
    // 12) Release object, CoUninitialize, and return results
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
			pwPort->Release();
			pwPort = NULL;
		}
		pwDownloaded->Release();
		pwDownloaded = NULL;
	}
	if(pwCollection)
	{
		pwCollection->Release();
        pwCollection = NULL;
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
	if( pwPort )
	{
		pwPort->Release();
        pwPort = NULL;
	}
	if( pwDownloaded )
	{
		pwDownloaded->Release();
        pwDownloaded = NULL;
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
// @topic IDirectMusicPerformance::PlaySegment()_Valid_Test | <nl>
//
// Description: <nl>
// Valid test function for IDirectMusicPerformance::PlaySegment().
//		Verifies that SegStates are released when segment is done playing. <nl>
//
// Test Function: <nl>
// tdmperfPlaySegmentValidTest2 <nl>
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
// 2) AddRef and Release SegState to get a pre-play value <nl>
// 3) Create CtIDirectMusicPerformance object <nl>
// 4) Call PlayMidiFile() convienence function <nl>
// 5) Make sure that Segment is done playing with IsPlaying loop <nl>
// 6) AddRef and Release SegState to get a post-play value <nl>
// 7) Compare pre-play and post-play values <nl>
// 8) Release objects, CoUninitialize, and return results<nl>
//
// History: <nl>
//  05/11/1998 - a-llucar - clean up, logging installed, autodoc <nl>
//
// @index topic | IDIRECTMUSICPERFORMANCE_CASES
//--------------------------------------------------------------------------;
DWORD tdmperfvPlaySegmentValidTest2(BOOL fCoCreate, BOOL fMaxDebug, 
						   LPARAM dwParam1, LPARAM dwParam2)
{
	DWORD						dwRes				= FNS_PASS;
	HRESULT						hr					= S_OK;
	CtIDirectMusicSegment		*pwSegment			= NULL;
	CtIDirectMusicSegmentState	*pwSegmentState		= NULL;
	CtIDirectMusicPerformance 	*pwPerformance		= NULL;
	int							dwPlayingRef		= 0;
	int							dwAfterRef			= 0;
	int							counter				= 0;
	int							TimeOutValue		= 30;

	WCHAR						*pMidiFile			= (WCHAR*)dwParam1;

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
	hr = dmthPlayMidiFile(pwPerformance,CTIID_IDirectMusicSegment,(CtIDirectMusicSegment**)&pwSegment,(CtIDirectMusicSegmentState**)&pwSegmentState, TRUE, pMidiFile);
	if(FAILED(hr))
	{
        fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: PlayMidiFile convienence function ")
                TEXT("failed (%s == %08Xh)"),
                tdmXlatHRESULT(hr), hr);
		dwRes = FNS_ABORTED;
		goto TEST_END;
	}

	if(hr = pwPerformance->IsPlaying(pwSegment, NULL) == S_OK)
	{
	
		// **********************************************************************
		// 4) AddRef and Release SegState to get a pre-play value <nl>
		// **********************************************************************
		Sleep(1000);
		pwSegmentState->AddRef();
		dwPlayingRef = pwSegmentState->Release();
	}
	else
	{
		// It is already done playing, we'll abort
		fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT:  Unable to get a trace on the state of the segment during play"));
		dwRes = FNS_ABORTED;
		goto TEST_END;
	}

    // **********************************************************************
    // 5) Make sure that SegmentState is done by IsPlaying to return S_FALSE
    // **********************************************************************
	// Set counter back to zero
	counter = 0;
	while ((hr = pwPerformance->IsPlaying((pwSegment), NULL)) != S_FALSE)	
	{
		Sleep(1000);
		if(counter++ >TimeOutValue)
		{
			fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: PlaySegment() timed out waiting for segment to finish.\n")
				TEXT("IsPlaying did not return S_FALSE (%s == %08Xh)"),
				tdmXlatHRESULT(hr), hr);
			dwRes = FNS_ABORTED;
			goto TEST_END;
		}
	}

	// **********************************************************************
    // 6) AddRef and Release SegState to get a post-play value
    // **********************************************************************
	Sleep(10000);	//The performance "casually" releases SegmentStates after they are no longer needed.
	pwSegmentState->AddRef(); 
	dwAfterRef = pwSegmentState->Release();
	Sleep(2000);
	// **********************************************************************
    // 7) Compare dwInitialRef and dwAfterRef, make sure they are the same
    // **********************************************************************
	if (dwPlayingRef <= dwAfterRef)		//Checking to make sure that it loses at least 1
	{
		fnsLog(BUGLOGLEVEL, TEXT("!!!! BUG: SegmentState was not released when it finished playing! ")
			TEXT("dwAfterRef   == %lu\n")
			TEXT("dwPlayingRef == %lu\n"),
			dwAfterRef, dwPlayingRef);
		dwRes = FNS_FAIL;
		goto TEST_END;
	}

TEST_END:
    // **********************************************************************
    // 8) Release objects, CoUninitialize, and return results
    // **********************************************************************
	if(pwPerformance)
	{
		pwPerformance->CloseDown();
		pwPerformance->Release();
        pwPerformance = NULL;
	}
	if(pwSegmentState)
	{
		pwSegmentState->Release();
		pwSegmentState = NULL;
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
// @topic IDirectMusicPerformance::PlaySegment()_Valid_Test |
//
// Description: <nl>
// Valid test function for IDirectMusicPerformance::PlaySegment(). <nl>
//			Checks to make sure that files play properly from the middle . <nl>
//			NOTE:  THIS IS A LISTENING TEST!  YOU SHOULD HEAR A D! <nl><nl>
//
// Test Function: <nl>
// tdmperfvPlaySegmentValidTest3 <nl>
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
// 2) Create CtIDirectMusicPerformance object <nl>
// 3) Create a Loader object <nl>
// 4) Loader->EnableCache <nl>
// 5) Set the search directory <nl>
// 6) Load the segment object <nl>
// 7) QI for the Segment <nl>
// 8) Set up the Segment object <nl>
// 9) Call Init Test class on the object <nl> 
// 10) Add a Port <nl>
// 11) Call Segment->SetStartPoint so that it doesn't start at the beginning <nl>
// 12) Play the segment <nl>
// 13) Release objects, Co-Uninitialize, and return result <nl>
//
// History:
//  04/30/1998 - a-llucar - Created
//
//  Note - Same test is used for both GetLoopPointsValidTest1 and SetLoopPointsValidTest1
// @index topic | IDIRECTMUSICSEGMENT_CASES
//--------------------------------------------------------------------------;
DWORD tdmperfvPlaySegmentValidTest3(BOOL fCoCreate, BOOL fMaxDebug, LPARAM dwParam1, LPARAM dwParam2)
{
	DWORD						dwRes			= FNS_PASS;
	HRESULT						hr				= E_NOTIMPL;
	CtIDirectMusicSegment		*pwSegment		= NULL;
	CtIDirectMusicPerformance	*pwPerformance	= NULL;
	CtIDirectMusicLoader		*pwLoader		= NULL;
	IDirectMusicSegment			*pSegment		= NULL;
	CtIDirectMusicObject		*pwObject		= NULL;
    LPWSTR                      wszFilename     = L"PitchB.mid"; 
    LPWSTR                      wszExt          = L"mid";
	dmthSetPath(L"PitchB.mid");
	char						*chPath			= gszMediaPath;
	int							counter			= 0;
	int							SleepTime		= 1000;
	int							TimeOutValue	= 10;
	MUSIC_TIME					mtLength		= 0;
	MUSIC_TIME					mtStart			= 0;
	DMUS_OBJECTDESC				desc; 

//    dmthBeginMessage("You should hear a D");
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
    // 3) Create CtIDirectMusicLoader object 
    // **********************************************************************
	hr = dmthCreateLoader(IID_IDirectMusicLoader, &pwLoader);
	if(FAILED(hr))
	{
        fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: DirectMusicLoader object creation ")
                TEXT("failed (%s == %08Xh)"),
                tdmXlatHRESULT(hr), hr);
		dwRes = FNS_ABORTED;
		goto TEST_END;
	}
	
    // **********************************************************************
    // 4) Loader->EnableCache(GUID_DirectMusicAllTypes)
    // **********************************************************************
	hr = pwLoader->EnableCache(GUID_DirectMusicAllTypes, TRUE);
	if(FAILED(hr))
    {
        fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: Loader->EnableCache ")
                TEXT("failed (%s == %08Xh)"),
                tdmXlatHRESULT(hr), hr);
        dwRes = FNS_ABORTED;
        goto TEST_END;
    }

	// **********************************************************************
    // 5) SetSearchDirectory
    // **********************************************************************
	ZeroMemory(&desc, sizeof(desc));

	desc.dwSize = sizeof(DMUS_OBJECTDESC);
	desc.guidClass = CLSID_DirectMusicSegment;
	desc.dwValidData |= (DMUS_OBJ_CLASS | DMUS_OBJ_FILENAME | DMUS_OBJ_FULLPATH);

	MultiByteToWideChar(CP_ACP, NULL, (LPCSTR)chPath, -1, desc.wszFileName, DMUS_MAX_FILENAME);
	
	// **********************************************************************
    // 6) Get the Segment object
    // **********************************************************************
	hr = pwLoader->GetObject(&pwObject, &desc);
	if(FAILED(hr))
    {
        fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: Loader->GetObject(pSegment) ")
                TEXT("failed (%s == %08Xh)"),
                tdmXlatHRESULT(hr), hr);
        dwRes = FNS_ABORTED;
        goto TEST_END;
    }

    // **********************************************************************
    // 7) QueryInterface for the Segment object
    // **********************************************************************
	hr = pwObject->QueryInterface(IID_IDirectMusicSegment, (void **)&pSegment);
	if(FAILED(hr))
    {
        fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: QueryInterface for pSegment ")
                TEXT("failed (%s == %08Xh)"),
                tdmXlatHRESULT(hr), hr);
        dwRes = FNS_ABORTED;
        goto TEST_END;
    }

    // **********************************************************************
    // 8) Set up a wrapped Segment object
    // **********************************************************************
	pwSegment = new CtIDirectMusicSegment;
	if (!pwSegment)
	{
        fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: pwSegment did not get set(pwSegment == NULL) "));
		dwRes = FNS_ABORTED;
		goto TEST_END;
	}

    // **********************************************************************
    // 9) Call InitTestClass on the wrapped segment
    // **********************************************************************
	hr = pwSegment->InitTestClass(pSegment);
	if(FAILED(hr))
	{
        fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: pwSegment->InitTestClass ")
                TEXT("failed (%s == %08Xh)"),
                tdmXlatHRESULT(hr), hr);
		dwRes = FNS_ABORTED;
		goto TEST_END;
	}
    
	// **********************************************************************
    // 10) Init and AddPort
    // **********************************************************************
	hr = pwPerformance->Init(NULL,NULL,ghWnd);
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

	hr = pwPerformance->AddPort(NULL);
	if(FAILED(hr))
	{
        fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: pwPerformance->AddPort(NULL) ")
                TEXT("failed (%s == %08Xh)"),
                tdmXlatHRESULT(hr), hr);
		dwRes = FNS_ABORTED;
		goto TEST_END;
	}

	// **********************************************************************
    // 11) Set the start point to other than the beginning
    // **********************************************************************	
	hr = pwSegment->GetLength(&mtLength);
	if(FAILED(hr))
	{
		fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: pwSegment->GetLength ")
				TEXT("failed (%s == %08Xh)"),
				tdmXlatHRESULT(hr), hr);
		dwRes = FNS_ABORTED;
		goto TEST_END;
	}

	// make sure that mtLength is set to something other than 0
	if(!mtLength)
	{
		fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: mtLength was not set (mtLength == 0)"));
		dwRes = FNS_ABORTED;
		goto TEST_END;
	}
    
	mtStart = (mtLength /2); //Half the length of the segment

	hr = pwSegment->SetStartPoint(mtStart);
	if(FAILED(hr))
	{
		fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: pwSegment->SetStartPoint ")
				TEXT("failed (%s == %08Xh)"),
				tdmXlatHRESULT(hr), hr);
		dwRes = FNS_ABORTED;
		goto TEST_END;
	}
	    
    // **********************************************************************
    // 12) Call PlaySegment and check results
    // **********************************************************************
	hr = pwPerformance->PlaySegment(pwSegment, DMUS_SEGF_BEAT, 0, NULL);
	if(FAILED(hr))
	{
        fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: pwPerformance->PlaySegment ")
                TEXT("failed (%s == %08Xh)"),
                tdmXlatHRESULT(hr), hr);
		dwRes = FNS_ABORTED;
		goto TEST_END;
	}

    // **********************************************************************
    // 13) Wait for IsPlaying to return favorably (or time out)
    // **********************************************************************
	// Set counter back to zero
	counter = 0;
	while ((hr = pwPerformance->IsPlaying(pwSegment, NULL)) != S_OK)	
	{
		Sleep(SleepTime);
		if(counter++ >TimeOutValue)
		{
			fnsLog(BUGLOGLEVEL, TEXT("!!! BUG: PlaySegment() timed out")
				TEXT("IsPlaying did not return the Segment (%s == %08Xh)"),
				tdmXlatHRESULT(hr), hr);
			dwRes = FNS_FAIL;
			goto TEST_END;
		}
	}
Sleep(3000);
//	dwRes = dmthEndMessage("Did it sound correct?");	

TEST_END:
    // **********************************************************************
    // 16) Release objects, CoUninitialize, and return results
    // **********************************************************************
	if(pwPerformance)
	{
		pwPerformance->Stop(NULL, NULL, 0, 0);
		pwPerformance->CloseDown();
		pwPerformance->Release();
		pwPerformance = NULL;
	}
	if(pwLoader)
	{
		pwLoader->Release();
		pwLoader = NULL;
	}
	if(pwSegment)
	{
		pwSegment->Release();
		pwSegment = NULL;
	}
	if(pwObject)
	{
		pwObject->Release();
		pwObject = NULL;
	}
	if(pSegment)
	{
		pSegment->Release();
		pSegment = NULL;
	}
	dmthCoUninitialize();	
	return dwRes;
}
//===========================================================================


//--------------------------------------------------------------------------;
// @doc IDIRECTMUSICPERFORMANCE A-LLUCAR
//
// @topic IDirectMusicPerformance::FreePMsg()_Valid_Test |
//
// Description: <nl>
// Valid test function for IDirectMusicPerformance::FreePMsg().
//		Frees a PMSG that's already been Freed, Expects DMUS_E_CANNOT_FREE <nl>
//
// Test Function: <nl>
// tdmperfvFreePMsgValidTest2 <nl>
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
// 2) Create CtIDirectMusicPerformance object <nl>
// 3) Init the Performance <nl>
// 4) Call AllocPMsg for Note <nl>
// 5) Set up pMSG's for Note <nl>
// 6) Call FreePMsg for Note twice, make sure it returns E_FAIL the second time <nl>
// 7) Cleanup objects, uninitialize COM and return results <nl>
//
// History: <nl>
//  05/18/1998 - a-llucar - created <nl>
//
// @index topic | IDIRECTMUSICPERFORMANCE_CASES
//--------------------------------------------------------------------------;
DWORD tdmperfvFreePMsgValidTest2(BOOL fCoCreate, BOOL fMaxDebug, 
						   LPARAM dwParam1, LPARAM dwParam2)
{
	DWORD							dwRes			= FNS_PASS;
	HRESULT							hr				= S_OK;
	DMUS_NOTE_PMSG					*pNote			= NULL;
	CtIDirectMusic					*pwMusic		= NULL;
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
		fnsLog(ABORTLOGLEVEL, TEXT("*** ABORT: Performance->Init ")
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
    // 4) Call AllocPMsg on pNote and check result
    // **********************************************************************
	hr = pwPerformance->AllocPMsg( sizeof(DMUS_NOTE_PMSG), (DMUS_PMSG**)&pNote );
	if(FAILED(hr))
	{
        fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: IDirectMusicPerformance->AllocPMsg(pNote) "),
                TEXT("failed (%s == %08Xh)"),
                tdmXlatHRESULT(hr), hr);
		dwRes = FNS_ABORTED;
		goto TEST_END;
	}

	// Check to make sure that pNote was properly populated
	if(!pNote)
	{
        fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: AllocPMsg did not properly populate pNote correctly, \n"),
                TEXT("pNote == NULL"));
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
    // 6) Call FreePMsg on pNote and check results
    // **********************************************************************	
	hr = pwPerformance->FreePMsg( (DMUS_PMSG*)pNote );
	if(FAILED(hr))
	{
        fnsLog(BUGLOGLEVEL, TEXT("!!!! BUG: IDirectMusicPerformance->FreePMsg(pNote) ")
                TEXT("failed (%s == %08Xh)"),
                tdmXlatHRESULT(hr), hr);
		dwRes = FNS_FAIL;
		goto TEST_END;
	}

	// Call FreePMsg again, expect an E_FAIL
	hr = pwPerformance->FreePMsg( (DMUS_PMSG*)pNote );
	if(hr != DMUS_E_CANNOT_FREE)
	{
        fnsLog(BUGLOGLEVEL, TEXT("!!!! BUG: IDirectMusicPerformance->FreePMsg(pNote) ")
                TEXT("did not return DMUS_E_CANNOT_FREE second time called. (%s == %08Xh)"),
                tdmXlatHRESULT(hr), hr);
		dwRes = FNS_FAIL;
		goto TEST_END;
	}

TEST_END:
    // **********************************************************************
    // 7) Cleanup objects and uninitialize COM and Return results
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
// @topic IDirectMusicPerformance::FreePMsg()_Valid_Test |
//
// Description: <nl>
// Valid test function for IDirectMusicPerformance::FreePMsg().
//		Frees a PMSG that's already been sent, Expects DMUS_E_CANNOT_FREE <nl>
//
// Test Function: <nl>
// tdmperfvFreePMsgValidTest3 <nl>
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
// 2) Create CtIDirectMusicPerformance object <nl>
// 3) Init the Performance <nl>
// 4) Call AllocPMsg for Note <nl>
// 5) Set up pMSG's for Note <nl>
// 6) Call SendPMsg for the note <nl>
// 6) Call FreePMsg for Note, make sure it returns E_FAIL <nl>
// 7) Cleanup objects, uninitialize COM and return results <nl>
//
// History: <nl>
//  05/18/1998 - a-llucar - created <nl>
//
// @index topic | IDIRECTMUSICPERFORMANCE_CASES
//--------------------------------------------------------------------------;
DWORD tdmperfvFreePMsgValidTest3(BOOL fCoCreate, BOOL fMaxDebug, 
						   LPARAM dwParam1, LPARAM dwParam2)
{
	DWORD							dwRes			= FNS_PASS;
	HRESULT							hr				= S_OK;
	DMUS_NOTE_PMSG					*pNote			= NULL;
	CtIDirectMusic					*pwMusic		= NULL;
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
		fnsLog(ABORTLOGLEVEL, TEXT("*** ABORT: Performance->Init ")
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
    // 4) Call AllocPMsg on pNote and check result
    // **********************************************************************
	hr = pwPerformance->AllocPMsg( sizeof(DMUS_NOTE_PMSG), (DMUS_PMSG**)&pNote );
	if(FAILED(hr))
	{
        fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: IDirectMusicPerformance->AllocPMsg(pNote) ")
                TEXT("failed (%s == %08Xh)"),
                tdmXlatHRESULT(hr), hr);
		dwRes = FNS_ABORTED;
		goto TEST_END;
	}

	// Check to make sure that pNote was properly populated
	if(!pNote)
	{
        fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: AllocPMsg did not properly populate pNote correctly, \n"),
                TEXT("pNote == NULL"));
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
    // 6) Call SendPMsg on pNote and check results
    // **********************************************************************	
	hr = pwPerformance->SendPMsg( (DMUS_PMSG*)pNote );
	if(FAILED(hr))
	{
        fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: IDirectMusicPerformance->SendPMsg(pNote) "),
                TEXT("failed (%s == %08Xh)"),
                tdmXlatHRESULT(hr), hr);
		dwRes = FNS_ABORTED;
		goto TEST_END;
	}

    // **********************************************************************
    // 6) Call FreePMsg on pNote and check results
    // **********************************************************************	
	if( fnsGetMetric(FNSMETRIC_STRESS) )  //commented out by JJ
	{
		fnsLog(FYILOGLEVEL, TEXT("---- FYI: PMSG NOT FREED -- BY-DESIGN -- WHEN RUNNING IN STRESS"));
	}
	else
	{
		hr = pwPerformance->FreePMsg( (DMUS_PMSG*)pNote );
		if(hr != DMUS_E_CANNOT_FREE)
		{
			fnsLog(BUGLOGLEVEL, TEXT("!!!! BUG: IDirectMusicPerformance->FreePMsg(pNote) ")
					TEXT("EXPECTED DMUS_E_CANNOT_FREE (%s == %08Xh)"),
					tdmXlatHRESULT(hr), hr);
			dwRes = FNS_FAIL;
			goto TEST_END;
		}
	}

TEST_END:
    // **********************************************************************
    // 7) Cleanup objects and uninitialize COM and Return results
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
DWORD tdmperfvGetSegmentStateValidTest1(BOOL fCoCreate, BOOL fMaxDebug, 
						   LPARAM dwParam1, LPARAM dwParam2)
{
	HRESULT						hr				= E_NOTIMPL;
	DWORD						dwRes			= FNS_PASS;
	CtIDirectMusicPerformance	*pwPerformance	= NULL;
	CtIDirectMusicSegment		*pwSegment		= NULL;
	CtIDirectMusicSegmentState	*pwSegmentState	= NULL;
	REFERENCE_TIME				rt				= 0;
	MUSIC_TIME					mt				= 0;

	WCHAR						*pSegmentFile	= (WCHAR*)dwParam1;

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
    // 4) Init and AddPort
    // **********************************************************************		
	hr = dmthPrePlay(&pwPerformance);
    if(FAILED(hr))
	{
        fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: dmthPrePlay ")
                TEXT("failed (%s == %08Xh)"),
                tdmXlatHRESULT(hr), hr);
		dwRes = FNS_ABORTED;
		goto TEST_END;
	}

    // **********************************************************************
    // 5) Call GetTime to get the current time and check results
    // **********************************************************************
	hr = pwPerformance->GetTime(&rt, NULL);
	if(FAILED(hr))
	{
        fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: IDirectMusicPerformance->GetTime ")
                TEXT("failed (%s == %08Xh)"),
                tdmXlatHRESULT(hr), hr);
		dwRes = FNS_ABORTED;
		goto TEST_END;
	}
	rt = rt + 100000000;

    // **********************************************************************
    // 6) Call PlaySegment way into the future and check results
    // **********************************************************************
	// Set PlaySegment to play at time 1 billion after now
	hr = pwPerformance->PlaySegment(pwSegment, DMUS_SEGF_BEAT | DMUS_SEGF_REFTIME, rt, NULL);
	if(FAILED(hr))
	{
        fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: pwPerformance->PlaySegment ")
                TEXT("failed (%s == %08Xh)"),
                tdmXlatHRESULT(hr), hr);
		dwRes = FNS_ABORTED;
		goto TEST_END;
	}

    // **********************************************************************
    // 7) Call ReferenceToMusicTime and check results
    // **********************************************************************
	hr = pwPerformance->ReferenceToMusicTime(rt, &mt);
	if(FAILED(hr))
	{
        fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: pwPerformance->ReferenceToMusicTime ")
                TEXT("failed (%s == %08Xh)"),
                tdmXlatHRESULT(hr), hr);
		dwRes = FNS_ABORTED;
		goto TEST_END;
	}
	
    // **********************************************************************
    // 8) Call GetSegmentState way in the future
    // **********************************************************************
	hr = pwPerformance->GetSegmentState(&pwSegmentState, mt);
	if(FAILED(hr))
	{
        fnsLog(BUGLOGLEVEL, TEXT("!!!! BUG: pwPerformance->GetSegmentState(way in the future) ")
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
	dmthReleasePerformance(pwPerformance);
	dmthReleaseSegment(pwSegment);
	dmthReleaseSegmentState(pwSegmentState);
	dmthCoUninitialize();
	return dwRes; 
} 


//--------------------------------------------------------------------------;
// @doc IDIRECTMUSICPERFORMANCE A-LLUCAR
//
// @topic IDirectMusicPerformance::PlaySegment()_Valid_Test |
//
// Description: <nl>
// Valid test function for IDirectMusicPerformance::PlaySegment().
//			Play a segment that's set way into the future <nl>
//
// Test Function: <nl>
// tdmperfvPlaySegmentValidTest4 <nl>
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
// 3) Create CtIDirectMusicLoader object <nl>
// 4) Loader->EnableCache(GUID_DirectMusicAllTypes) <nl>
// 5) SetSearchDirectory <nl>
// 6) Get the Segment object <nl>
// 7) Add the port <nl>
// 8) Call GetTime to get the current time and check results <nl>
// 9) Call PlaySegment way into the future and check results <nl>
// 10) Release objects, CoUninitilalize, and return results <nl>
//
// History: <nl>
//  05/18/1998 - a-llucar - created <nl>
//
//		NOTE:  This is a listening test.  There should be a substantial pause before it starts playing! <nl>
//
// @index topic | IDIRECTMUSICPERFORMANCE_CASES
//--------------------------------------------------------------------------;
DWORD tdmperfvPlaySegmentValidTest4(BOOL fCoCreate, BOOL fMaxDebug, 
						   LPARAM dwParam1, LPARAM dwParam2)
{
	HRESULT						hr				= E_NOTIMPL;
	DWORD						dwRes			= FNS_PASS;
	CtIDirectMusicPerformance	*pwPerformance	= NULL;
	CtIDirectMusicSegment		*pwSegment		= NULL;
	CtIDirectMusic				*pwMusic		= NULL;
	REFERENCE_TIME				rt				= 0;
	MUSIC_TIME					mt				= 0;

	WCHAR						*pSegmentFile	= (WCHAR*)dwParam1;

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
    // 7) Init and AddPort
    // **********************************************************************		
	hr = pwPerformance->Init(NULL,NULL,ghWnd);
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

	hr = pwPerformance->AddPort(NULL); 
    if(FAILED(hr))
	{
        fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: pwPerformance->AddPort(NULL) ")
                TEXT("failed (%s == %08Xh)"),
                tdmXlatHRESULT(hr), hr);
		dwRes = FNS_ABORTED;
		goto TEST_END;
	}

    // **********************************************************************
    // 8) Call GetTime to get the current time and check results
    // **********************************************************************
	hr = pwPerformance->GetTime(&rt, NULL);
	if(FAILED(hr))
	{
        fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: IDirectMusicPerformance->GetTime ")
                TEXT("failed (%s == %08Xh)"),
                tdmXlatHRESULT(hr), hr);
		dwRes = FNS_ABORTED;
		goto TEST_END;
	}
	rt = rt + 50000000;

    // **********************************************************************
    // 9) Call PlaySegment way into the future and check results
    // **********************************************************************
	// Set PlaySegment to play at time 1 billion after now
	hr = pwPerformance->PlaySegment((pwSegment), DMUS_SEGF_BEAT | DMUS_SEGF_REFTIME, rt, NULL);
	if(FAILED(hr))
	{
        fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: pwPerformance->PlaySegment ")
                TEXT("failed (%s == %08Xh)"),
                tdmXlatHRESULT(hr), hr);
		dwRes = FNS_ABORTED;
		goto TEST_END;
	}

	Sleep(10000);	
TEST_END:

    // **********************************************************************
    // 10) Release objects, CoUninitilalize, and return results
    // **********************************************************************
	if(pwPerformance)
	{
		pwPerformance->Stop(NULL, NULL, 0, 0);
		pwPerformance->CloseDown();
		pwPerformance->Release();
		pwPerformance = NULL;
	}
	if(pwSegment)
	{
		pwSegment->Release();
		pwSegment = NULL;
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
// @topic IDirectMusicPerformance::Invalidate()_Valid_Test |
//
// Description: <nl>
// Valid test function for IDirectMusicPerformance::Invalidate(). <nl>
//		Tests dwFlags, able to pass any flag through app.cpp <nl>
//
// Test Function: <nl>
// tdmperfvInvalidateValidTest1 <nl>
//
// Parameters: <nl>
//  BOOL    fCoCreate   - unused <nl>
//  BOOL    fMaxDebug   - unused <nl>
//  DWORD   dwFlags     - DMUS_SEGF_FLAGS <nl>
//	DWORD   mtTime      - Time to flush at <nl>
//
// File : TDMPERFV.CPP
//
// Steps: <nl>
// 1) Initialize COM. <nl>
// 2) Create CtIDirectMusicPerformance object <nl>
// 3) Init the performance <nl>
// 4) Call Invalidate passing DMUS_SEGF_FLAGS from app.cpp <nl>
// 5) Release objects, CoUninitialize, and return results <nl>
//
// History: <nl>
//  01/14/1998 - markburt - created <nl>
//  03/23/1998 - a-llucar - clean up, logging installed, autodoc <nl>
//
// @index topic | IDIRECTMUSICPERFORMANCE_CASES
//--------------------------------------------------------------------------;
DWORD tdmperfvInvalidateValidTest1(BOOL fCoCreate, BOOL fMaxDebug, 
						   LPARAM dwFlags, LPARAM mtTime)
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
    // 4) Call Invalidate and check results
    // **********************************************************************
	hr = pwPerformance->Invalidate((MUSIC_TIME)mtTime, (DWORD)dwFlags);
	if(FAILED(hr))
	{
        fnsLog(BUGLOGLEVEL, TEXT("!!!! BUG: IDirectMusicPerformance->Invalidate "));
        fnsLog(BUGLOGLEVEL, TEXT("failed (%s == %08Xh)"), tdmXlatHRESULT(hr), hr);
		fnsLog(BUGLOGLEVEL,	TEXT("mtTime  == %lu\n"), mtTime);
		fnsLog(BUGLOGLEVEL,	TEXT("dwFlags == %08Xh\n"), dwFlags);
		dwRes = FNS_FAIL;
		goto TEST_END;
	}

TEST_END:
    // **********************************************************************
    // 5) Release objects, CoUnInitialize, and return results
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
// @topic IDirectMusicPerformance::GetSegmentState()_Valid_Test |
//
// Description: <nl>
// Valid test function for IDirectMusicPerformance::GetSegmentState().
//		Calls GetSegmentState while no segment is playing.  <nl>
//		Expects descriptive fail message.
//
// Test Function: <nl>
// tdmperfvGetSegmentStateValidTest2 <nl>
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
// 2) Create CtIDirectMusicPerformance object <nl>
// 3) Call PlayMidiFile() convienence function <nl>
// 4) Wait for IsPlaying to return S_FALSE (or time out) <nl>
// 5) Call GetSegmentState on Segment that's done playing <nl>
// 6) Release objects, dmthCoUninitialize, and return results <nl>
//
// History: <nl>
//  06/10/1998 - a-llucar - created <nl>
//
// @index topic | IDIRECTMUSICPERFORMANCE_CASES
//--------------------------------------------------------------------------;
DWORD tdmperfvGetSegmentStateValidTest2(BOOL fCoCreate, BOOL fMaxDebug, 
						   LPARAM dwParam1, LPARAM dwParam2)
{
	DWORD							dwRes			= FNS_PASS;
	HRESULT							hr				= S_OK;
	CtIDirectMusicPerformance		*pwPerformance	= NULL;
	CtIDirectMusicSegment			*pwSegment		= NULL;
	CtIDirectMusicSegmentState		*pwSegmentState	= NULL;
	int								counter			= 0;
	int								SleepTime		= 1000;
	int								TimeOutValue	= 15;

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
    // 3) Call PlayMidiFile() convienence function
    // **********************************************************************
	hr = dmthPlayMidiFile(pwPerformance,CTIID_IDirectMusicSegment,(CtIDirectMusicSegment**)&pwSegment,(CtIDirectMusicSegmentState**)&pwSegmentState, TRUE, NULL);
	if(hr != S_OK)
	{
        fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: PlayMidiFile convienence function "),
                TEXT("failed (%s == %08Xh)"),
                tdmXlatHRESULT(hr), hr);
		dwRes = FNS_ABORTED;
		goto TEST_END;
	}

    // **********************************************************************
    // 4) Wait for IsPlaying to return S_FALSE (or time out)
    // **********************************************************************
	// Set counter back to zero
	counter = 0;
	while ((hr = pwPerformance->IsPlaying((pwSegment), NULL)) != S_FALSE)	
	{
		Sleep(SleepTime);
		if(counter++ >TimeOutValue)
		{
			fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: PlaySegment() did not time out\n")
				TEXT("IsPlaying continued playing the Segment (%s == %08Xh)"),
				tdmXlatHRESULT(hr), hr);
			dwRes = FNS_ABORTED;
			goto TEST_END;
		}
	}

    // **********************************************************************
    // 5) GetSegmentState on Segment that's done playing
    // **********************************************************************
	if (pwSegmentState)
	{
		pwSegmentState->Release();
		pwSegmentState = NULL;
	}
	hr = pwPerformance->GetSegmentState(&pwSegmentState, 1000000);
	if(hr != DMUS_E_NOT_FOUND)
	{
        fnsLog(BUGLOGLEVEL, TEXT("!!!! BUG: IDirectMusicPerformance->GetSegmentState ")
                TEXT("did not return correctly (%s == %08Xh)"),
                tdmXlatHRESULT(hr), hr);
		dwRes = FNS_FAIL;
		goto TEST_END;
	}

TEST_END:
    // **********************************************************************
    // 6) Release objects, CoInitialize, and return results
    // **********************************************************************
	if(pwPerformance)
	{
		pwPerformance->Stop(NULL, NULL, 0, 0);
		pwPerformance->CloseDown();
		pwPerformance->Release();
		pwPerformance = NULL;
	}
	if(pwSegment)
	{
		pwSegment->Release();
		pwSegment = NULL;
	}
	if(pwSegmentState)
	{
		pwSegmentState->Release();
		pwSegmentState = NULL;
	}
	dmthCoUninitialize();	
	return dwRes;
}	// End tdmperfvGetSegmentStateValidTest2()


//--------------------------------------------------------------------------;
// @doc IDIRECTMUSICPERFORMANCE A-LLUCAR
//
// @topic IDirectMusicPerformance::Stop()_Valid_Test |
//
// Description: <nl>
// Valid test function for IDirectMusicPerformance::Stop().
//			Stop a segment that's cued to play in the future <nl>
//
// Test Function: <nl>
// tdmperfvStopValidTest3 <nl>
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
// 4) Add the port and Init <nl>
// 5) Call GetTime to get the current time and check results <nl>
// 6) Call PlaySegment way into the future and check results <nl>
// 7) Call Stop on the segment state that is going to play <nl>
// 8) Release objects, CoUninitilalize, and return results <nl>
//
// History: <nl>
//  06/10/1998 - a-llucar - created <nl>
//
//		NOTE:  This is a listening test.  YOU SHOULD HEAR NOTHING! <nl>
//
// @index topic | IDIRECTMUSICPERFORMANCE_CASES
//--------------------------------------------------------------------------;
DWORD tdmperfvStopValidTest3(BOOL fCoCreate, BOOL fMaxDebug, 
						   LPARAM dwParam1, LPARAM dwParam2)
{
	HRESULT						hr				= E_NOTIMPL;
	DWORD						dwRes			= FNS_PASS;
	IDirectMusicSegment			*pSegment		= NULL;
	CtIDirectMusicPerformance	*pwPerformance	= NULL;
	CtIDirectMusicSegment		*pwSegment		= NULL;
	CtIDirectMusicLoader		*pwLoader		= NULL;
	CtIDirectMusicObject		*pwObject		= NULL;
	CtIDirectMusic				*pwMusic		= NULL;
	REFERENCE_TIME				rt				= 0;
	MUSIC_TIME					mt				= 0;

	WCHAR						*pSegmentFile	= (WCHAR*)dwParam1;

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
    // 4) Init and AddPort
    // **********************************************************************		
	hr = pwPerformance->Init(NULL,NULL,ghWnd);
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

	hr = pwPerformance->AddPort(NULL); 
    if(FAILED(hr))
	{
        fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: pwPerformance->AddPort(NULL) ")
                TEXT("failed (%s == %08Xh)"),
                tdmXlatHRESULT(hr), hr);
		dwRes = FNS_ABORTED;
		goto TEST_END;
	}

    // **********************************************************************
    // 5) Call GetTime to get the current time and check results
    // **********************************************************************
	hr = pwPerformance->GetTime(&rt, NULL);
	if(FAILED(hr))
	{
        fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: IDirectMusicPerformance->GetTime ")
                TEXT("failed (%s == %08Xh)"),
                tdmXlatHRESULT(hr), hr);
		dwRes = FNS_ABORTED;
		goto TEST_END;
	}
	rt = rt + 80000000;

    // **********************************************************************
    // 6) Call PlaySegment way into the future and check results
    // **********************************************************************
	// Set PlaySegment to play at way into the future
	hr = pwPerformance->PlaySegment(pwSegment, DMUS_SEGF_BEAT | DMUS_SEGF_REFTIME, rt, NULL);
	if(FAILED(hr))
	{
        fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: pwPerformance->PlaySegment ")
                TEXT("failed (%s == %08Xh)"),
                tdmXlatHRESULT(hr), hr);
		dwRes = FNS_ABORTED;
		goto TEST_END;
	}

    // **********************************************************************
    // 7) Call Stop on the segment that hasn't started playing yet
    // **********************************************************************
	hr = pwPerformance->Stop(NULL, NULL, 0, 0);
	if(FAILED(hr))
	{
        fnsLog(BUGLOGLEVEL, TEXT("!!!! BUG: pwPerformance->Stop ")
                TEXT("failed (%s == %08Xh)"),
                tdmXlatHRESULT(hr), hr);
		dwRes = FNS_FAIL;
		goto TEST_END;
	}
	
	Sleep(10000);	
TEST_END:

    // **********************************************************************
    // 8) Release objects, CoUninitilalize, and return results
    // **********************************************************************
	if(pwPerformance)
	{
		pwPerformance->CloseDown();
		pwPerformance->Release();
		pwPerformance = NULL;
	}
	if(pwSegment)
	{
		pwSegment->Release();
		pwSegment = NULL;
	}
	if(pwLoader)
	{
		pwLoader->Release();
		pwLoader = NULL;
	}
	if(pSegment)
	{
		pSegment->Release();
		pSegment = NULL;
	}
	if(pwObject)
	{
		pwObject->Release();
		pwObject = NULL;
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
// @topic IDirectMusicPerformance::SetParam()_Valid_Test |
//
// Description: <nl>
// Valid test function for IDirectMusicPerformance::SetParam() <nl>
//			Change the tempo of a Performance that is playing <nl>
//
// Test Function: <nl>
// tdmperfvSetParamValidTest1 <nl>
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
// 2) Create CtIDirectMusicPerformance object <nl>
// 3) Create an CtIDirectMusicLoader object <nl>
// 4) Enable cache on the loader <nl>
// 5) Load a Segment object <nl>
// 6) Call GetTrack on a Tempo object <nl>
// 7) Call SetParam to set the Tempo object <nl>
// 8) Call GetParam, and make sure that the tempo changed <nl>
// 9) Cleanup objects, uninitialize COM, and return test result <nl>
//
// History:
//  05/15/1998 - a-llucar - created <nl>
//
//	NOTE:  The same test is used for SetParamValidTest1 and GetParamValidTest1 <nl>
//
// @index topic | IDIRECTMUSICPERFORMANCE_CASES
//--------------------------------------------------------------------------;
DWORD tdmperfvSetParamValidTest1(BOOL fCoCreate, BOOL fMaxDebug, LPARAM dwParam1, LPARAM dwParam2)
{
	DWORD						dwRes			= FNS_PASS;
	HRESULT						hr				= E_NOTIMPL;
	CtIDirectMusicSegment		*pwSegment		= NULL;
	CtIDirectMusicPerformance	*pwPerformance	= NULL;
	CtIDirectMusicTrack			*pwTrack		= NULL;

	int							counter			= 0;
	int							SleepTime		= 1000;
	int							TimeOutValue	= 10;
	GUID						guid			= GUID_TempoParam;
	DMUS_TEMPO_PARAM			dmtp;
	WCHAR						*pSegmentFile	= (WCHAR*)dwParam1;
	char						*chPath			= gszMediaPath;
	WCHAR						*wchPath		= gwszBasePath;
	dmthSetPath(NULL);
	MUSIC_TIME					mt				= 0;

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
    // 3) Create IDirectMusicTempoTrack object 
    // **********************************************************************	
	hr = dmthCreateTrack(CLSID_DirectMusicTempoTrack, IID_IDirectMusicTrack, &pwTrack);
	if(FAILED(hr))
	{
		fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: CoCreate TempoTrack failed! (%s == %08Xh)"),
			tdmXlatHRESULT(hr), hr);
		dwRes = FNS_ABORTED;
		goto TEST_END;
	}
	
	// **********************************************************************
    // 6) Get the Segment object
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
    // 7) Init(NULL) and AddPort(NULL)
    // **********************************************************************
	hr = pwPerformance->Init(NULL,NULL,ghWnd);
	if(FAILED(hr))
	{
        fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: pwPerformance->Init(NULL) ")
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
        fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: pwPerformance->AddPort(NULL) ")
                TEXT("failed (%s == %08Xh)"),
                tdmXlatHRESULT(hr), hr);
		dwRes = FNS_ABORTED;
		goto TEST_END;
	}

	pwSegment->SetRepeats(5);

    // **********************************************************************
	// 8) Set dmtp with a bunch of particulars
    // **********************************************************************	
	ZeroMemory( &dmtp, sizeof(dmtp) );
	dmtp.dblTempo = 300;

	// **********************************************************************
    // 10) Call PlaySegment and check results
    // **********************************************************************
	hr = pwPerformance->PlaySegment(pwSegment, 0, 0, NULL);
	if(FAILED(hr))
	{
        fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: pwPerformance->PlaySegment ")
                TEXT("failed (%s == %08Xh)"),
                tdmXlatHRESULT(hr), hr);
		dwRes = FNS_ABORTED;
		goto TEST_END;
	}
	
	pwPerformance->GetTime(NULL, &mt);
	fnsLog(FYILOGLEVEL, TEXT("Time Before = %ld"), mt);	

   	// **********************************************************************
    // 9) Call SetParam, check the results 
    // **********************************************************************
	hr = pwPerformance->SetParam(guid, 0xffffffff, 0, mt + 4000, (void*)&dmtp); 
	if(FAILED(hr))
	{
		fnsLog(BUGLOGLEVEL, TEXT("!!!! BUG: IDirectMusicPerformance->SetParam ")
				TEXT("failed (%s == %08Xh)"),
				tdmXlatHRESULT(hr), hr);
		dwRes = FNS_FAIL;
		goto TEST_END;
	}   

	hr = pwPerformance->Invalidate(0, 0);
	if(FAILED(hr))
	{
		fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT:  Performance->Invalidate ")
				TEXT("failed (%s == %08Xh)"),
				tdmXlatHRESULT(hr), hr);
		dwRes = FNS_ABORTED;
		goto TEST_END;
	}

	pwPerformance->GetTime(NULL, &mt);
	fnsLog(FYILOGLEVEL, TEXT("Time After = %ld"), mt);	

   	// **********************************************************************
    // 10) Call GetParam, check the results 
    // **********************************************************************
	hr = pwPerformance->GetParam(guid, 0xffffffff, 0, mt + 2000, 0, (void*)&dmtp); 
	if(FAILED(hr))
	{
		fnsLog(BUGLOGLEVEL, TEXT("!!!! BUG: IDirectMusicPerformance->SetParam ")
				TEXT("failed (%s == %08Xh)"),
				tdmXlatHRESULT(hr), hr);
		dwRes = FNS_FAIL;
		goto TEST_END;
	}   

    // **********************************************************************
    // 11) Wait for IsPlaying to return favorably (or time out)
    // **********************************************************************
	// Set counter back to zero
	counter = 0;
	while ((hr = pwPerformance->IsPlaying(pwSegment, NULL)) != S_OK)	
	{
		Sleep(SleepTime);
		if(counter++ >TimeOutValue)
		{
			fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: PlaySegment() timed out")
				TEXT("IsPlaying did not return the Segment (%s == %08Xh)"),
				tdmXlatHRESULT(hr), hr);
			dwRes = FNS_ABORTED;
			goto TEST_END;
		}
	}
Sleep(10000);
	pwPerformance->GetTime(NULL, &mt);
	fnsLog(FYILOGLEVEL, TEXT("Time After = %ld"), mt);	

TEST_END:
    // **********************************************************************
    // 12) Release objects, CoUninitialize, and return results
    // **********************************************************************
	if(pwPerformance)
	{
		pwPerformance->Stop(NULL, NULL, 0, 0);
		pwPerformance->CloseDown();
		pwPerformance->Release();
		pwPerformance = NULL;
	}
	if(pwTrack)
	{
		pwTrack->Release();
		pwTrack = NULL;
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
// @topic IDirectMusicPerformance::SetParam()_Valid_Test |
//
// Description: <nl>
// Valid test function for IDirectMusicPerformance::SetParam() <nl>
//			Change the tempo of a Performance that is playing <nl>
//
// Test Function: <nl>
// tdmsegvSetParamValidTest3 <nl>
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
// 2) Create CtIDirectMusicPerformance object <nl>
// 3) Load a Segment object <nl>
// 4) Call GetTrack on a Tempo object <nl>
// 5) Call SetParam to set the Tempo object <nl>
// 6) Call GetParam, and make sure that the tempo changed <nl>
// 7) Cleanup objects, uninitialize COM, and return test result <nl>
//
// History:
//  08/20/1998 - a-llucar - created <nl>
//
//	NOTE:  The same test is used for SetParamValidTest1 and GetParamValidTest1 <nl>
//
// @index topic | IDIRECTMUSICPERFORMANCE_CASES
//--------------------------------------------------------------------------;
DWORD tdmperfvSetParamValidTest3(BOOL fCoCreate, BOOL fMaxDebug, LPARAM dwParam1, LPARAM dwParam2)
{
	DWORD						dwRes			= FNS_PASS;
	HRESULT						hr				= E_NOTIMPL;
	CtIDirectMusicSegment		*pwSegment		= NULL;
	CtIDirectMusicPerformance	*pwPerformance	= NULL;
	WCHAR						*pSegmentFile	= (WCHAR*)dwParam1;
	
	MUSIC_TIME					mt				= 0;
	GUID						guid			= GUID_TimeSignature;
	DMUS_IO_TIMESIGNATURE_ITEM	pTempo;

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
    // 4) Set up the performance to play
    // **********************************************************************
	hr = dmthPrePlay(&pwPerformance);
	if(FAILED(hr))
	{
        fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: dmthPrePlay ")
                TEXT("failed (%s == %08Xh)"),
                tdmXlatHRESULT(hr), hr);
		dwRes = FNS_ABORTED;
		goto TEST_END;
	}
 
	// **********************************************************************
    // 5) Call PlaySegment and check results
    // **********************************************************************
	hr = pwPerformance->PlaySegment(pwSegment, 0, 0, NULL);
	if(FAILED(hr))
	{
        fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: pwPerformance->PlaySegment ")
                TEXT("failed (%s == %08Xh)"),
                tdmXlatHRESULT(hr), hr);
		dwRes = FNS_ABORTED;
		goto TEST_END;
	}
	
	pwPerformance->GetTime(NULL, &mt);
	fnsLog(FYILOGLEVEL, TEXT("Time Before = %ld"), mt);	

   	// **********************************************************************
    // 6) Call GetParam, check the results 
    // **********************************************************************
	hr = pwSegment->GetParam(guid, 0xffffffff, 0, mt + 2000, 0, (void*)&pTempo); 
	if(FAILED(hr))
	{
		fnsLog(BUGLOGLEVEL, TEXT("!!!! BUG: IDirectMusicPerformance->GetParam ")
				TEXT("failed (%s == %08Xh)"),
				tdmXlatHRESULT(hr), hr);
		dwRes = FNS_FAIL;
		goto TEST_END;
	}   
	if((!pTempo.bBeatsPerMeasure) || (!pTempo.bBeat))
	{
		fnsLog(BUGLOGLEVEL, TEXT("!!!! BUG: IDirectMusicPerformance->GetParam ")
				TEXT("failed (did not populate BPM's or Beats)"));
		dwRes = FNS_FAIL;
		goto TEST_END;
	}   

	fnsLog(FYILOGLEVEL, "TimeSignature == %i / %i", pTempo.bBeatsPerMeasure, pTempo.bBeat);
	
   	// **********************************************************************
    // 7) GetNotification that shows we're playing
    // **********************************************************************
	hr = dmthGetNotification(pwPerformance, GUID_NOTIFICATION_MEASUREANDBEAT, DMUS_NOTIFICATION_MEASUREBEAT, 2000);	
	if (FAILED(hr)) 
	{
        fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: dmthGetNotification ")
                TEXT("failed (%s == %08Xh)"), tdmXlatHRESULT(hr), hr);
		dwRes = FNS_ABORTED;
		goto TEST_END;
	}
		
	pwPerformance->GetTime(NULL, &mt);
	fnsLog(FYILOGLEVEL, TEXT("Time After = %ld"), mt);	

TEST_END:
    // **********************************************************************
    // 8) Release objects, CoUninitialize, and return results
    // **********************************************************************
	dmthReleasePerformance(pwPerformance);
	dmthReleaseSegment(pwSegment);
	dmthCoUninitialize();	
	return dwRes;
}
//===========================================================================



//--------------------------------------------------------------------------;
// @doc IDIRECTMUSICPERFORMANCE A-LLUCAR
//
// @topic IDirectMusicPerformance::GetGlobalParam()_Valid_Test |
//
// Description: <nl>
// Valid test function for IDirectMusicPerformance::GetGlobalParam(). <nl>
//		Passes all different types of guids.
//
// Test Function: <nl>
// tdmperfvGetGlobalParamValidTest1 <nl>
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
// 2) Create CtIDirectMusicPerformance object <nl>
// 3) Call SetGlobalParam, check results<nl>
// 4) Call GetGlobalParam, check results (make sure dw and dw2 are equal) <nl>
// 5) Release objects, CoUninitialize, and return results <nl>
//
// History: <nl>
//	06/25/1998 - a-llucar - created
//	07/21/1998 - a-llucar - changed it so that we use the Array instead of dwParam1
//
//	Note - GetGlobalParam() and SetGlobalParam() both use the same test.
//
// @index topic | IDIRECTMUSICPERFORMANCE_CASES
//--------------------------------------------------------------------------;
DWORD tdmperfvGetGlobalParamValidTest1(BOOL fCoCreate, BOOL fMaxDebug, 
						   LPARAM dwParam1, LPARAM dwParam2)
{
	DWORD						dwRes			= FNS_PASS;
	HRESULT						hr				= S_OK;
	DWORD						dw				= 53;
	DWORD						dw2				= 0;
	CtIDirectMusicPerformance	*pwPerformance	= NULL;
	int							i				= 0;

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
	for (i = 0; i < sizeof(AllDMGuids)/sizeof(GUID*); i++)
	{
		hr = pwPerformance->SetGlobalParam( *AllDMGuids[i], (void*)&dw, sizeof(AllDMGuids[i]));
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
		hr = pwPerformance->GetGlobalParam( *AllDMGuids[i], (void*)&dw2, sizeof(AllDMGuids[i]));
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
	        fnsLog(BUGLOGLEVEL, TEXT("!!!! BUG: IDirectMusicPerformance->GetGlobalParam\n "));
			fnsLog(BUGLOGLEVEL, TEXT("pData was not properly set"));
	        fnsLog(BUGLOGLEVEL, TEXT("Expected dw  == %ld"), dw);
			fnsLog(BUGLOGLEVEL,	TEXT("Received dw2 == %ld"), dw2);
			dwRes = FNS_FAIL;
			goto TEST_END;
		}	
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
// @topic IDirectMusicPerformance::PlaySegment()_Valid_Test |
//
// Description: <nl>
// Valid test function for IDirectMusicPerformance::PlaySegment().
//		Creates two ports, and calls primary and secondary segments
//
// Test Function: <nl>
// tdmperfvPlaySegmentValidTest5 <nl>
//
// Parameters: <nl>
//  BOOL    fUseNULL    - pass NULL or BOGUS <nl>
//  BOOL    fParamNum	- Which Parameter to test <nl>
//  DWORD   dwParam1    - unused <nl>
//	DWORD   dwParam2    - Expected HRESULT <nl>
//
// File : TDMPERFI.CPP
//
// Steps: <nl>
// 1) Initialize COM. <nl>
// 2) Create CtIDirectMusicPerformance object (wrapped IDMPerformance object) <nl>
// 3) Load a Segment Object <nl>
// 4) Set up Performance to play <nl>
// 5) Call PlaySegment <nl>
// 6) Call Stop passing NULL or BOGUS parameter, check results <nl> 
// 6) Release objects, CoUninitialize, and return results <nl>
//
// History: <nl>
//  06/21/1998 - a-llucar - created <nl>
// @index topic | IDIRECTMUSICPERFORMANCE_CASES
//--------------------------------------------------------------------------;
DWORD tdmperfvPlaySegmentValidTest5(BOOL fCoCreate, 
									BOOL fMaxDebug, 
									LPARAM dwParam1, 
									LPARAM dwParam2)
{
	DWORD						dwRes				= FNS_PASS;
	HRESULT						hr					= S_OK;
	CtIDirectMusicPerformance	*pwPerformance		= NULL;
	CtIDirectMusicSegment		*pwSegment			= NULL;
	CtIDirectMusicPort			*pwPort				= NULL;
	CtIDirectMusicPort			*pwPort2			= NULL;

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
        fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: DirectMusicSegment object creation "),
                TEXT("failed (%s == %08Xh)"),
                tdmXlatHRESULT(hr), hr);
		dwRes = FNS_ABORTED;
		goto TEST_END;
	}

    // **********************************************************************
    // 3) Load a Segment Object
    // **********************************************************************
	hr = dmthLoadSegment(L"DSegTest3.sgt", &pwSegment);
	if (FAILED(hr))
	{
		fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: dmthLoadSegment ")
				TEXT("failed (%s == %08Xh)"),
				tdmXlatHRESULT(hr), hr);
		dwRes = FNS_ABORTED;
		goto TEST_END;
	}

	// **********************************************************************
    // 4) Set up the performance to play
    // **********************************************************************
	hr = pwPerformance->Init(NULL,NULL,ghWnd);
	if(FAILED(hr))
	{
		fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: pwPerformance->Init(NULL) ")
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

	hr = pwPerformance->AddPort(pwPort);
	if(FAILED(hr))
	{
        fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: pwPerformance->AddPort(pwPort) ")
                TEXT("failed (%s == %08Xh)"),
                tdmXlatHRESULT(hr), hr);
		dwRes = FNS_ABORTED;
		goto TEST_END;
	}

	hr = pwPerformance->AddPort(pwPort2);
	if(FAILED(hr))
	{
        fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: pwPerformance->AddPort(pwPort2) ")
                TEXT("failed (%s == %08Xh)"),
                tdmXlatHRESULT(hr), hr);
		dwRes = FNS_ABORTED;
		goto TEST_END;
	}

	// **********************************************************************
    // 5) call PlaySegment
    // **********************************************************************
	hr = pwPerformance->PlaySegment(pwSegment, DMUS_SEGF_BEAT, 0, NULL);
	if(FAILED(hr))
	{
		fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: pwPerformance->PlaySegment (primary) ")
					TEXT("failed (%s == %08Xh)"),
					tdmXlatHRESULT(hr), hr);
		dwRes = FNS_ABORTED;
		goto TEST_END;
	}
	Sleep(1000);
	hr = pwPerformance->PlaySegment(pwSegment, DMUS_SEGF_BEAT | DMUS_SEGF_SECONDARY, 0, NULL);
	if(FAILED(hr))
	{
		fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: pwPerformance->PlaySegment (secondary) ")
					TEXT("failed (%s == %08Xh)"),
					tdmXlatHRESULT(hr), hr);
		dwRes = FNS_ABORTED;
		goto TEST_END;
	}
	Sleep(1000);

TEST_END:
    // **********************************************************************
    // 6) Cleanup objects and uninitialize COM and Return results
    // **********************************************************************
	if (pwPerformance)
	{
		pwPerformance->Stop(NULL, NULL, 0, 0);
		pwPerformance->CloseDown();
		pwPerformance->Release();
        pwPerformance = NULL;
	}
	if(pwSegment)
	{
		pwSegment->Release();
        pwSegment = NULL;
	}
	if(pwPort)
	{
		pwPort->Release();
		pwPort = NULL;
	}
	if(pwPort2)
	{
		pwPort2->Release();
		pwPort2 = NULL;
	} 
	dmthCoUninitialize();	
	return dwRes;
}	





//--------------------------------------------------------------------------;
// @doc IDIRECTMUSICPERFORMANCE A-LLUCAR
//
// @topic IDirectMusicPerformance::PlaySegment()_Valid_Test |
//
// Description: <nl>
// Valid test function for IDirectMusicPerformance::PlaySegment(). <nl>
//			Checks to make sure that files play properly from the middle . <nl>
//			This is identical to PlaySegmentValidTest3, except it's a producer generated segment.
//			NOTE:  THIS IS A LISTENING TEST!  YOU SHOULD HEAR A D! <nl><nl>
//
// Test Function: <nl>
// tdmperfvPlaySegmentValidTest6 <nl>
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
// 2) Create a wrapped Composer object <nl>
// 3) Create a wrapped Performance object <nl>
// 4) Load the media <nl>
// 5) Compose the two segments <nl>
// 6) Init and AddPort <nl>
// 7) Set the start point to other than the beginning <nl>
// 8) Call PlaySegment and check results <nl>
// 9) Wait for IsPlaying to return favorably (or time out) <nl>
// 10) Release objects, Co-Uninitialize, and return result <nl>
//
// History:
//  06/29/1998 - a-llucar - Created
//
//  Note - Same test is used for both GetLoopPointsValidTest1 and SetLoopPointsValidTest1
// @index topic | IDIRECTMUSICSEGMENT_CASES
//--------------------------------------------------------------------------;
DWORD tdmperfvPlaySegmentValidTest6(BOOL fCoCreate, BOOL fMaxDebug, LPARAM dwParam1, LPARAM dwParam2)
{
	DWORD						dwRes			= FNS_PASS;
	HRESULT						hr				= E_NOTIMPL;
	CtIDirectMusicSegment		*pwSegment		= NULL;
	CtIDirectMusicPerformance	*pwPerformance	= NULL;
	CtIDirectMusicComposer		*pwComposer		= NULL;
	CtIDirectMusicStyle			*pwStyle		= NULL;
	CtIDirectMusicChordMap		*pwChordMap	= NULL;
	int							counter			= 0;
	int							SleepTime		= 1000;
	int							TimeOutValue	= 10;
	MUSIC_TIME					mtLength		= 0;
	MUSIC_TIME					mtStart			= 0;

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
    // 2) Create CtIDirectMusicComposer object (wrapped IDMComposer object)
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
    // 3) Create CtIDirectMusicPerformance object (wrapped IDMPerformance object)
    // **********************************************************************
	hr = dmthCreatePerformance(IID_IDirectMusicPerformance, &pwPerformance);
	if (FAILED(hr))
	{
		fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: CoCreate Performance Object ")
				TEXT("failed (%s == %08Xh)"),
				tdmXlatHRESULT(hr), hr);
		dwRes = FNS_ABORTED;
		goto TEST_END;
	}

    // **********************************************************************
    // 4) Load the media
    // **********************************************************************
	hr = dmthLoadStyle(L"PitBen.sty", &pwStyle);
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
    // 5) Compose the two segments
    // **********************************************************************
	hr = pwComposer->ComposeSegmentFromShape(pwStyle, 4, DMUS_SHAPET_FALLING, 0, FALSE, FALSE, pwChordMap, &pwSegment);
	if (FAILED(hr))
	{
		fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: ComposeSegmentFromShape ")
				TEXT("failed (%s == %08Xh)"),
				tdmXlatHRESULT(hr), hr);
		dwRes = FNS_ABORTED;
		goto TEST_END;
	}
    
	// **********************************************************************
    // 6) Init and AddPort
    // **********************************************************************
	hr = pwPerformance->Init(NULL,NULL,ghWnd);
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

	hr = pwPerformance->AddPort(NULL);
	if(FAILED(hr))
	{
        fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: pwPerformance->AddPort(NULL) ")
                TEXT("failed (%s == %08Xh)"),
                tdmXlatHRESULT(hr), hr);
		dwRes = FNS_ABORTED;
		goto TEST_END;
	}

	// **********************************************************************
    // 7) Set the start point to other than the beginning
    // **********************************************************************	
	hr = pwSegment->GetLength(&mtLength);
	if(FAILED(hr))
	{
		fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: pwSegment->GetLength ")
				TEXT("failed (%s == %08Xh)"),
				tdmXlatHRESULT(hr), hr);
		dwRes = FNS_ABORTED;
		goto TEST_END;
	}

	// make sure that mtLength is set to something other than 0
	if(!mtLength)
	{
		fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: mtLength was not set (mtLength == 0)"));
		dwRes = FNS_ABORTED;
		goto TEST_END;
	}
    
	mtStart = (mtLength /2); //Half the length of the segment

	hr = pwSegment->SetStartPoint(mtStart);
	if(FAILED(hr))
	{
		fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: pwSegment->SetStartPoint ")
				TEXT("failed (%s == %08Xh)"),
				tdmXlatHRESULT(hr), hr);
		dwRes = FNS_ABORTED;
		goto TEST_END;
	}
	    
    // **********************************************************************
    // 8) Call PlaySegment and check results
    // **********************************************************************
	hr = pwPerformance->PlaySegment(pwSegment, 0, 0, NULL);
	if(FAILED(hr))
	{
        fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: pwPerformance->PlaySegment ")
                TEXT("failed (%s == %08Xh)"),
                tdmXlatHRESULT(hr), hr);
		dwRes = FNS_ABORTED;
		goto TEST_END;
	}

    // **********************************************************************
    // 9) Wait for IsPlaying to return favorably (or time out)
    // **********************************************************************
	// Set counter back to zero
	counter = 0;
	while ((hr = pwPerformance->IsPlaying(pwSegment, NULL)) != S_OK)	
	{
		Sleep(SleepTime);
		if(counter++ >TimeOutValue)
		{
			fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: PlaySegment() timed out")
				TEXT("IsPlaying did not return the Segment (%s == %08Xh)"),
				tdmXlatHRESULT(hr), hr);
			dwRes = FNS_ABORTED;
			goto TEST_END;
		}
	}
Sleep(5000);

TEST_END:
    // **********************************************************************
    // 10) Release objects, CoUninitialize, and return results
    // **********************************************************************
	if(pwPerformance)
	{
		pwPerformance->Stop(NULL, NULL, 0, 0);
		pwPerformance->CloseDown();
		pwPerformance->Release();
		pwPerformance = NULL;
	}
	if(pwSegment)
	{
		pwSegment->Release();
		pwSegment = NULL;
	}
	if(pwStyle)
	{
		pwStyle->Release();
		pwStyle = NULL;
	}
	if(pwComposer)
	{
		pwComposer->Release();
		pwComposer = NULL;
	}
	if(pwChordMap)
	{
		pwChordMap->Release();
		pwChordMap = NULL;
	}
	dmthCoUninitialize();	
	return dwRes;
}
//===========================================================================



//--------------------------------------------------------------------------;
// @doc IDIRECTMUSICPERFORMANCE A-LLUCAR
//
// @topic IDirectMusicPerformance::QueryInterface_Valid_Test |
//
// Description: <nl>
//	QI an IDMPerformance for the passed in interface ID(which should be 
//	valid!) and verify that the IUnknown obtained from QIing that interface
//	is the same as the IUnknown obtained from QIing the original performance.
//
// Test Function: <nl>
//	tdmperfvQIValidTest1 <nl>
//
// Parameters: <nl>
//  BOOL    fCoCreate   - unused <nl>
//  BOOL    fMaxDebug   - unused <nl>
//  DWORD   dwParam1    - unused <nl>
//	DWORD   dwParam2    - *IID to QI for <nl>
//
// File : TDMPERFV.CPP
//
// Steps: <nl>
// 1) Initialize COM. <nl>
// 2) Create a wrapped object <nl>
// 3) QI the interface for the IID given to us. <nl>
// 4) QI the newly obtained interface for IUnknown. <nl>
// 5) QI the original IDMPerformance for IUnknown. <nl>
// 6) Compare their pointers.  Report a bug if they differ. <nl>
// 7) Cleanup objects, uninitialize COM, and return test results. <nl>
//
// History:
//  07/16/1998 - a-llucar - created <nl>
//
// @index topic | IDIRECTMUSICPERFORMANCE_CASES
//--------------------------------------------------------------------------;
DWORD tdmperfvQIValidTest1( BOOL fCoCreate,
					  BOOL fMaxDebug, 
					  LPARAM dwParam1, 
					  LPARAM dwParam2 )
{
    HRESULT hr		= E_NOTIMPL;
	DWORD	dwRes	= FNS_PASS;

	IUnknown						*pInterface			= NULL;
	IUnknown						*pIUnknown1			= NULL;
	IUnknown						*pIUnknown2			= NULL;
	CtIDirectMusicPerformance		*pwPerformance		= NULL;
	GUID							*pIID				= (GUID *)dwParam2;

    // **********************************************************************
    // 1) Initialize COM.
    // **********************************************************************
    hr = dmthCoInitialize( NULL );
    if ( FAILED(hr) )
    {
        fnsLog( ABORTLOGLEVEL, "**** ABORT: COM initialization failed! "
			"(%s == %08Xh)", dmthXlatHRESULT(hr), hr );
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
	// 3) QI the interface for the IID given to us.
    // **********************************************************************
	hr = pwPerformance->QueryInterface( *pIID, (void **)(&pInterface) );
    if ( FAILED(hr) )
    {
        fnsLog( ABORTLOGLEVEL, "**** ABORT: pwPerformance::QI failed! "
			"(%s == %08Xh)", dmthXlatHRESULT(hr), hr );
        dwRes = FNS_ABORTED;
		goto TEST_END;
    }

    // **********************************************************************
	// 4) QI the newly obtained interface for IUnknown.
    // **********************************************************************
    fnsLog( FYILOGLEVEL, "---- FYI: QI'ing obtained interface for IUnknown " );
	hr = pInterface->QueryInterface( IID_IUnknown, 
								(void **)(&pIUnknown1) );
    if ( FAILED(hr) )
    {
        fnsLog( BUGLOGLEVEL, "!!!! BUG: IDMPerformance::QI for IUnknown failed! "
			"(%s == %08Xh)", dmthXlatHRESULT(hr), hr );
        dwRes = FNS_FAIL;
		goto TEST_END;
    }

    // **********************************************************************
	// 5) QI the original IDMBand for IUnknown.
    // **********************************************************************
	hr = pwPerformance->QueryInterface( IID_IUnknown, 
								(void **)(&pIUnknown2) );
    if ( FAILED(hr) )
    {
        fnsLog( BUGLOGLEVEL, "!!!! BUG: pwPerformance::QI for IUnknown failed! "
			"(%s == %08Xh)", dmthXlatHRESULT(hr), hr );
        dwRes = FNS_FAIL;
		goto TEST_END;
    }

    // **********************************************************************
	// 6) Compare their pointers.  Report a bug if they differ.
    // **********************************************************************
	if ( pIUnknown1 != pIUnknown2 )
	{
        fnsLog( BUGLOGLEVEL, "!!!! BUG: IUnknown pointers are different! "
			"(%s == %08Xh)", dmthXlatHRESULT(hr), hr );
        dwRes = FNS_FAIL;
		goto TEST_END;
    }


TEST_END:
    // **********************************************************************
    // 7) Cleanup objects, uninitialize COM, and return test results.
    // **********************************************************************
	if (pwPerformance)
	{
		pwPerformance->CloseDown();
		pwPerformance->Release();
        pwPerformance = NULL;
	}
	if ( pInterface )
	{
		pInterface->Release();
        pInterface = NULL;
	}

	if ( pIUnknown1 )
	{
		pIUnknown1->Release();
        pIUnknown1 = NULL;
	}
	
	if ( pIUnknown2 )
	{
		pIUnknown2->Release();
        pIUnknown2 = NULL;
	}

	dmthCoUninitialize();	

	return dwRes;
}
//===========================================================================


//--------------------------------------------------------------------------;
// @doc IDIRECTMUSICPERFORMANCE A-LLUCAR
//
// @topic IDirectMusicPerformance::SetPrepareTime()_Valid_Test |
//
// Description: <nl>
// Valid test function for IDirectMusicPerformance::SetPrepareTime().
//		Sets the Prepare Time way into the future, checks GetPrepareTime <nl>
//		after PlaySegment is called <nl>
//
// Test Function: <nl>
// tdmperfvSetPrepareTimeValidTest1 <nl>
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
// 2) Create CtIDirectMusicPerformance object <nl>
// 3) Call SetPrepareTime and Test the function <nl>
// 4) Call GetPrepareTime and Test the function <nl>
// 5) Cleanup objects and uninitialize COM and Return results <nl>
//
// History: <nl>
//  07/16/1998 - a-llucar - created <nl>
//
//	The same test is used for GetPrepareTimeValidTest1() and SetPrepareTimeValidTest1() <nl>
//
// @index topic | IDIRECTMUSICPERFORMANCE_CASES
//--------------------------------------------------------------------------;
DWORD tdmperfvSetPrepareTimeValidTest1(BOOL fCoCreate, BOOL fMaxDebug, 
						   LPARAM dwParam1, LPARAM dwParam2)
{
	DWORD						dwRes				= FNS_PASS;
	HRESULT						hr					= S_OK;
	CtIDirectMusicPerformance	*pwPerformance		= NULL;
	CtIDirectMusicSegment		*pwSegment			= NULL;
	DWORD						dwSet				= 10000;
	DWORD						dwGet				= 0;

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
        fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: DirectMusicSegment object creation "),
                TEXT("failed (%s == %08Xh)"),
                tdmXlatHRESULT(hr), hr);
		dwRes = FNS_ABORTED;
		goto TEST_END;
	}

    // **********************************************************************
    // 3) Load a Segment Object
    // **********************************************************************
	hr = dmthLoadSegment(L"DSegTest3.sgt", &pwSegment);
	if (FAILED(hr))
	{
		fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: dmthLoadSegment ")
				TEXT("failed (%s == %08Xh)"),
				tdmXlatHRESULT(hr), hr);
		dwRes = FNS_ABORTED;
		goto TEST_END;
	}

	// **********************************************************************
    // 4) Call SetPrepareTime and Test the function
    // **********************************************************************
	hr = pwPerformance->SetPrepareTime(dwSet);
	if(FAILED(hr))
	{
        fnsLog(BUGLOGLEVEL, TEXT("!!!! BUG: IDirectMusicPerformance->SetPrepareTime() ")
                TEXT("failed (%s == %08Xh)"),
                tdmXlatHRESULT(hr), hr);
		dwRes = FNS_FAIL;
		goto TEST_END;
	}

	// **********************************************************************
    // 5) Set up the performance to play
    // **********************************************************************
	hr = pwPerformance->Init(NULL,NULL,ghWnd);
	if(FAILED(hr))
	{
		fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: pwPerformance->Init(NULL) ")
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
        fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: pwPerformance->AddPort(NULL) ")
                TEXT("failed (%s == %08Xh)"),
                tdmXlatHRESULT(hr), hr);
		dwRes = FNS_ABORTED;
		goto TEST_END;
	}

	// **********************************************************************
    // 6) call PlaySegment
    // **********************************************************************
	hr = pwPerformance->PlaySegment(pwSegment, DMUS_SEGF_BEAT, 0, NULL);
	if(FAILED(hr))
	{
		fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: pwPerformance->PlaySegment ")
					TEXT("failed (%s == %08Xh)"),
					tdmXlatHRESULT(hr), hr);
		dwRes = FNS_ABORTED;
		goto TEST_END;
	}
	Sleep(5000);
						   
    // **********************************************************************
    // 7) Call GetPrepareTime and Test the function
    // **********************************************************************
	hr = pwPerformance->GetPrepareTime(&dwGet);
	if(FAILED(hr))
	{
        fnsLog(BUGLOGLEVEL, TEXT("!!!! BUG: IDirectMusicPerformance->GetPrepareTime() ")
                TEXT("failed (%s == %08Xh)"),
                tdmXlatHRESULT(hr), hr);
		dwRes = FNS_FAIL;
		goto TEST_END;
	}

	// Test to make sure that dwSet and dwGet are the same
	if(dwSet != dwGet)
	{
        fnsLog(BUGLOGLEVEL, TEXT("!!!! BUG: GetPrepareTime not set correctly, ")
                TEXT("(dwSet == %08Xh)")
				TEXT("(dwGet == %08Xh)"),
                dwSet, dwGet);
		dwRes = FNS_FAIL;
		goto TEST_END;
	}
TEST_END:
    // **********************************************************************
    // 5) Cleanup objects and uninitialize COM and Return results
    // **********************************************************************
	// Clean up the wrapper
	if(pwPerformance)
	{
		pwPerformance->Stop(NULL, NULL, 0, 0);
		pwPerformance->CloseDown();
		pwPerformance->Release();
        pwPerformance = NULL;
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
// @topic IDirectMusicPerformance::Stop()_Valid_Test |
//
// Description: <nl>
// Valid test function for IDirectMusicPerformance::PlaySegment().
//		Calls method passing all possible param types
//
// Test Function: <nl>
// tdmperfvStopCAPTest <nl>
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
// 2) Create CtIDirectMusicPerformance object (wrapped IDMPerformance object) <nl>
// 3) Load Segment
// 4) Pre-Play
// 5) Call PlaySegment and Stop for all params <nl>
// 6) Release objects, CoUninitialize, and return results <nl>
//
// History: <nl>
//  07/22/1998 - a-llucar - created <nl>
//
// @index topic | IDIRECTMUSICPERFORMANCE_CASES
//--------------------------------------------------------------------------;
DWORD tdmperfvStopCAPTest(BOOL fCoCreate, BOOL fMaxDebug, 
						   LPARAM dwParam1, LPARAM dwParam2)
{
	DWORD						dwRes			= FNS_PASS;
	HRESULT						hr				= S_OK;
	CtIDirectMusicPerformance*	pwPerformance	= NULL;
	CtIDirectMusicSegment		*pwSegment		= NULL;
	CtIDirectMusicSegmentState	*pwSegmentState	= NULL;
	int							i				= 0;

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
        fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: DirectMusicSegment object creation "),
                TEXT("failed (%s == %08Xh)"),
                tdmXlatHRESULT(hr), hr);
		dwRes = FNS_ABORTED;
		goto TEST_END;
	}

    // **********************************************************************
    // 3) Load a Segment Object
    // **********************************************************************
	hr = dmthLoadSegment(L"DSegTest3.sgt", &pwSegment);
	if (FAILED(hr))
	{
		fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: dmthLoadSegment ")
				TEXT("failed (%s == %08Xh)"),
				tdmXlatHRESULT(hr), hr);
		dwRes = FNS_ABORTED;
		goto TEST_END;
	}

	// **********************************************************************
    // 4) Set up the performance to play
    // **********************************************************************
	hr = pwPerformance->Init(NULL,NULL,ghWnd);
	if(FAILED(hr))
	{
		fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: pwPerformance->Init(NULL) ")
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
        fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: pwPerformance->AddPort(NULL) ")
                TEXT("failed (%s == %08Xh)"),
                tdmXlatHRESULT(hr), hr);
		dwRes = FNS_ABORTED;
		goto TEST_END;
	}

    // **********************************************************************
    // 5) Call PlayMidiFile() and stop passing all params
    // **********************************************************************
	for (i = 0; i < sizeof(SEGF_FLAGS)/sizeof(DWORD); i++)
	{
		fnsLog(FYILOGLEVEL, "i == %i", i);
		hr = pwPerformance->PlaySegment(pwSegment, SEGF_FLAGS[i], 0, &pwSegmentState);
		if(FAILED(hr))
		{
	        fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: pwPerformance->PlaySegment "),
	                TEXT("failed (%s == %08Xh)"),
	                tdmXlatHRESULT(hr), hr);
			dwRes = FNS_ABORTED;
			goto TEST_END;
		}
		
		hr = pwPerformance->Stop(pwSegment, NULL, 0, SEGF_FLAGS[i]);
		if(FAILED(hr))
		{
			fnsLog(BUGLOGLEVEL, TEXT("!!!! BUG: Performance->Stop")
					TEXT("failed (%s == %08Xh)"),
					tdmXlatHRESULT(hr), hr);
			dwRes = FNS_FAIL;
		}
		
		Sleep(500);
		if(pwSegmentState)
		{
			pwSegmentState->Release();
			pwSegmentState = NULL;
		}
	}
	// re-initialize i
	i = 0;
	// Do the same for SegmentState
	for (i = 0; i < sizeof(SEGF_FLAGS)/sizeof(DWORD); i++)
	{
		fnsLog(FYILOGLEVEL, "i == %i", i);
		hr = pwPerformance->PlaySegment(pwSegment, 32, 0, &pwSegmentState);
		if(FAILED(hr))
		{
	        fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: pwPerformance->PlaySegment "),
	                TEXT("failed (%s == %08Xh)"),
	                tdmXlatHRESULT(hr), hr);
			dwRes = FNS_ABORTED;
			goto TEST_END;
		}
		
		hr = pwPerformance->Stop(NULL, pwSegmentState, 0, SEGF_FLAGS[i]);
		if(FAILED(hr))
		{
			fnsLog(BUGLOGLEVEL, TEXT("!!!! BUG: Performance->Stop")
					TEXT("failed (%s == %08Xh)"),
					tdmXlatHRESULT(hr), hr);
			dwRes = FNS_FAIL;
		}
		
		Sleep(500);
		if(pwSegmentState)
		{
			pwSegmentState->Release();
			pwSegmentState = NULL;
		}
	}


TEST_END:
    // **********************************************************************
    // 6) Cleanup objects and uninitialize COM and Return results
    // **********************************************************************
	if (pwPerformance)
	{
		pwPerformance->CloseDown();
		pwPerformance->Release();
        pwPerformance = NULL;
	}
	if(pwSegment)
	{
		pwSegment->Release();
        pwSegment = NULL;
	}
	if(pwSegmentState)
	{
		pwSegmentState->Release();
		pwSegmentState = NULL;
	}

	dmthCoUninitialize();	
	return dwRes;
}


//--------------------------------------------------------------------------;
// @doc IDIRECTMUSICPERFORMANCE A-LLUCAR
//
// @topic IDirectMusicPerformance::SetGraph()_Valid_Test |
//
// Description: <nl>
// Valid test function for IDirectMusicPerformance::SetGraph().
//		Creates 2 performance objects, GetGraph from the first and <nl>
//		SetGraph on the second.
//
// Test Function: <nl>
// tdmperfvSetGraphValidTest1 <nl>
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
// 2) Create 2 CtIDirectMusicPerformance objects <nl>
// 3) Create Graph Object <nl>
// 4) dmthCreateEchoTool <nl>
// 5) Check to make sure Tool got set to something <nl>
// 6) Load the two segments <nl>
// 7) Insert Tool <nl>
// 8) Set the Performance Graph <nl>
// 9) Create DirectMusic Ojbect <nl>
// 10) Init the two performances <nl>
// 11) Create a SynthPort <nl>
// 12) Add a port to our two performances <nl>
// 13) Create the 5 channels for the echos <nl>
// 14) Performance->AllocPMsg and check results <nl>
// 15) Call StampPMsg() and check results <nl>
// 16) Call PlaySegment and check results <nl>
// 17) Call GetGraph, and pass it into pwGraph2 <nl>
// 18) Call SetGraph on the Second performance <nl>
// 19) Wait for IsPlaying to return or time out <nl>
// 20) Call PlaySegment on the second performance <nl>
// 21) Release objects, CoUninitialize, and return results <nl>
//
// History: <nl>
//  07/22/1998 - a-llucar - created <nl>
//
// @index topic | IDIRECTMUSICPERFORMANCE_CASES
//--------------------------------------------------------------------------;
DWORD tdmperfvSetGraphValidTest1(BOOL fCoCreate, BOOL fMaxDebug, 
						   LPARAM dwParam1, LPARAM dwParam2)
{
    DWORD                       dwRes			= FNS_PASS;
    HRESULT                     hr				= S_OK;
    CtIDirectMusicGraph         *pwGraph		= NULL;
    CtIDirectMusic              *pwDM			= NULL;
    CtIDirectMusicPerformance   *pwPerformance	= NULL;
    CtIDirectMusicSegment       *pwSegment		= NULL;
    CtIDirectMusicTool          *pwTool			= NULL;
    CtIDirectMusicPort          *pwPort			= NULL;
    DMUS_PMSG                   *pdmusPMSG		= NULL;
    DMUS_PORTPARAMS             dmusPortParams;
    DWORD                       dwCounter       = 0;
    DWORD                       dwTimeOutValue  = 20;
    GUID                        guidSink;
	CtIDirectMusicPerformance	*pwPerformance2	= NULL;
	CtIDirectMusicSegment		*pwSegment2		= NULL;
	CtIDirectMusicGraph			*pwGraph2		= NULL;

    // **********************************************************************
    // 1) Initialize COM.
    // **********************************************************************
    hr = dmthCoInitialize(NULL);
    if(FAILED(hr))
    {
        fnsLog(ABORTLOGLEVEL, "**** ABORT: COM initialization failed! (%s == %08Xh)",
                tdmXlatHRESULT(hr), hr);
        return FNS_ABORTED;
    }

    // **********************************************************************
    // 9) Create DirectMusic Object
    // **********************************************************************
	hr = dmthCreateDMBaseObj(IID_IDirectMusic, &pwDM);
	if (FAILED(hr))
    {
        fnsLog(ABORTLOGLEVEL, "**** ABORT: IDirectMusic object creation failed! (%s == %08Xh)", 
			tdmXlatHRESULT(hr), hr);
        dwRes = FNS_ABORTED;
        goto TEST_END;
    }

	pwDM->SetDirectSound(NULL, ghWnd);
	if (FAILED(hr))
    {
        fnsLog(ABORTLOGLEVEL, "**** ABORT: Music->SetDirectSound failed! (%s == %08Xh)", 
			tdmXlatHRESULT(hr), hr);
        dwRes = FNS_ABORTED;
        goto TEST_END;
    }

    // **********************************************************************
    // 2) Create 2 CtIDirectMusicPerformance objects 
    // **********************************************************************
    hr = dmthCreatePerformance(IID_IDirectMusicPerformance, &pwPerformance);
    if(FAILED(hr))
    {
        fnsLog(ABORTLOGLEVEL, "**** ABORT: DirectMusicPerformance object creation (pwPerformance1) "
                "failed (%s == %08Xh)", tdmXlatHRESULT(hr), hr);
        dwRes = FNS_ABORTED;
        goto TEST_END;
    }

    hr = dmthCreatePerformance(IID_IDirectMusicPerformance, &pwPerformance2);
    if(FAILED(hr))
    {
        fnsLog(ABORTLOGLEVEL, "**** ABORT: DirectMusicPerformance object creation (pwPerformance2) "
                "failed (%s == %08Xh)", tdmXlatHRESULT(hr), hr);
        dwRes = FNS_ABORTED;
        goto TEST_END;
    }

    // **********************************************************************
    // 3) Create Graph Object
    // **********************************************************************
    hr = dmthCreateGraph(IID_IDirectMusicGraph, &pwGraph);
    if(FAILED(hr))
    {
        fnsLog(ABORTLOGLEVEL, "**** ABORT: dmthCreateGraph "
                "failed (%s == %08Xh)", tdmXlatHRESULT(hr), hr);
        dwRes = FNS_ABORTED;
        goto TEST_END;
    }

    // **********************************************************************
    // 4) dmthCreateEchoTool
    // **********************************************************************
    hr = dmthCreateEchoTool(IID_IDirectMusicTool, &pwTool, 3, DMUS_PPQ);
    if (FAILED( hr ))
    {
        fnsLog(ABORTLOGLEVEL, "**** ABORT: dmthCreateEchoTool "
                "failed (%s == %08Xh)", tdmXlatHRESULT(hr), hr);
        dwRes = FNS_ABORTED;
        goto TEST_END;
    }

    // **********************************************************************
    // 5) Check to make sure Tool got set to something
    // **********************************************************************
    if ( NULL == pwTool )
    {   
        fnsLog(ABORTLOGLEVEL, "**** ABORT: Something wrong with the Tool.  Should not be NULL.");
        dwRes = FNS_ABORTED;
        goto TEST_END;
    }


    // **********************************************************************
    // 6) Load the two segments
    // **********************************************************************
    hr = dmthLoadSegment(L"DSegTest3.sgt", &pwSegment);
    if (FAILED( hr ))
    {
        fnsLog(ABORTLOGLEVEL, "**** ABORT: dmthLoadSegment "
                "failed (%s == %08Xh)", tdmXlatHRESULT(hr), hr);
        dwRes = FNS_ABORTED;
        goto TEST_END;
    }

    hr = dmthLoadSegment(L"onestop2.mid", &pwSegment2);
    if (FAILED( hr ))
    {
        fnsLog(ABORTLOGLEVEL, "**** ABORT: dmthLoadSegment "
                "failed (%s == %08Xh)", tdmXlatHRESULT(hr), hr);
        dwRes = FNS_ABORTED;
        goto TEST_END;
    }

    // **********************************************************************
    // 7) Insert Tool
    // **********************************************************************
    hr = pwGraph->InsertTool( pwTool, NULL, 0, 0 );
    if(FAILED(hr))
    {
        fnsLog(ABORTLOGLEVEL, "**** ABORT: Graph->InsertTool "
                "failed (%s == %08Xh)", tdmXlatHRESULT(hr), hr);
        dwRes = FNS_ABORTED;
        goto TEST_END;
    }

    // **********************************************************************
    // 8) Set the Performance Graph
    // **********************************************************************
    hr = pwPerformance->SetGraph( pwGraph );
    if (FAILED( hr ))
    {
        fnsLog(BUGLOGLEVEL, "!!!! BUG: Performance->SetGraph(pwGraph) "
                "failed (%s == %08Xh)", tdmXlatHRESULT(hr), hr);
        dwRes = FNS_FAIL;
        goto TEST_END;
    }


    // **********************************************************************
    // 10) Init the two performances
    // **********************************************************************
    hr = pwPerformance->Init(&pwDM,NULL,ghWnd);
    if(FAILED(hr))
    {
        fnsLog(ABORTLOGLEVEL, "**** ABORT: Performance->Init "
               "failed (%s == %08Xh)", tdmXlatHRESULT(hr), hr);
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
	hr = pwPerformance2->Init(&pwDM,NULL,ghWnd);
    if(FAILED(hr))
    {
        fnsLog(ABORTLOGLEVEL, "**** ABORT: Performance2->Init "
               "failed (%s == %08Xh)", tdmXlatHRESULT(hr), hr);
        dwRes = FNS_ABORTED;
        goto TEST_END;
    }
    // set autodownload on
    fAuto = TRUE;
    hr = pwPerformance2->SetGlobalParam( GUID_PerfAutoDownload, &fAuto, sizeof(BOOL) );
	if(FAILED(hr))
	{
		fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: Performance2->SetGlobalParam(AutoDownload == TRUE) ")
				TEXT("failed (%s == %08Xh)"),
				tdmXlatHRESULT(hr), hr);
		dwRes = FNS_ABORTED;
		goto TEST_END;
	}

    // **********************************************************************
    // 11) Create a SynthPort
    // **********************************************************************
    ZeroMemory( &dmusPortParams, sizeof(DMUS_PORTPARAMS) );
    dmusPortParams.dwSize          = sizeof(DMUS_PORTPARAMS);  
    dmusPortParams.dwChannelGroups = 5; // create 5 channel groups on the port
    dmusPortParams.dwValidParams   = DMUS_PORTPARAMS_CHANNELGROUPS;

    ZeroMemory( &guidSink, sizeof(GUID) );

    hr = pwDM->CreatePort(
		CLSID_DirectMusicSynth, 
        &dmusPortParams, 
        &pwPort, 
        NULL );
    if ( FAILED(hr) )
    {
        fnsLog(ABORTLOGLEVEL, "**** ABORT: pwDM->CreatePort "
                "failed (%s == %08Xh)", tdmXlatHRESULT(hr), hr);
        dwRes = FNS_ABORTED;
        goto TEST_END;
    }

    // **********************************************************************
    // 12) Add a port to our two performances
    // **********************************************************************
    hr = pwPerformance->AddPort(pwPort);
    if(FAILED(hr))
    {
        fnsLog(ABORTLOGLEVEL, "**** ABORT: pwPerformance->AddPort(NULL) "
                "failed (%s == %08Xh)", tdmXlatHRESULT(hr), hr);
        dwRes = FNS_ABORTED;
        goto TEST_END;
    }

    hr = pwPerformance2->AddPort(NULL);
    if(FAILED(hr))
    {
        fnsLog(ABORTLOGLEVEL, "**** ABORT: pwPerformance2->AddPort(NULL) "
                "failed (%s == %08Xh)", tdmXlatHRESULT(hr), hr);
        dwRes = FNS_ABORTED;
        goto TEST_END;
    }

    // **********************************************************************
    // 13) Create the 5 channels for the echos
    // **********************************************************************
    // Assign blocks of 16 PChannels to this port.
    // Block 0, port pPort, and group 1 means to assign
    // PChannels 0-15 to group 1 on port pPort.
    // PChannels 0-15 correspond to the standard 16
    // MIDI channels.
    pwPerformance->AssignPChannelBlock( 0, pwPort, 1 );
    pwPerformance->AssignPChannelBlock( 1, pwPort, 2 ); //16 - 31 Group 2
    pwPerformance->AssignPChannelBlock( 2, pwPort, 3 ); //32 - 47 Group 3
    pwPerformance->AssignPChannelBlock( 3, pwPort, 4 ); //48 - 63 Group 4
    pwPerformance->AssignPChannelBlock( 4, pwPort, 5 ); //64 - 80 Group 5

    // **********************************************************************
    // 14) Performance->AllocPMsg and check results
    // **********************************************************************
    hr = pwPerformance->AllocPMsg( sizeof(DMUS_PMSG), &pdmusPMSG );
    if(FAILED(hr))
    {
        fnsLog(ABORTLOGLEVEL, "**** ABORT: Performance->AllocPMsg "
                "failed (%s == %08Xh)", tdmXlatHRESULT(hr), hr);
        dwRes = FNS_ABORTED;
        goto TEST_END;
    }

    // **********************************************************************
    // 15) Call StampPMsg() and check results
    // **********************************************************************   
    hr = pwGraph->StampPMsg(pdmusPMSG);
    if(FAILED(hr))
    {
        fnsLog(ABORTLOGLEVEL, "**** ABORT: DirectMusicTool->StampPMsg "
                "failed (%s == %08Xh)", tdmXlatHRESULT(hr), hr);
        dwRes = FNS_ABORTED;
        goto TEST_END;
    }

    // **********************************************************************
    // 16) Call PlaySegment and check results
    // **********************************************************************
    hr = pwPerformance->PlaySegment(pwSegment, 0, 0, NULL);
    if(FAILED(hr))
    {
        fnsLog(ABORTLOGLEVEL, "**** ABORT: pwPerformance->PlaySegment "
                "failed (%s == %08Xh)", tdmXlatHRESULT(hr), hr);
        dwRes = FNS_ABORTED;
        goto TEST_END;
    }

    // **********************************************************************
    // 17) Call GetGraph, and pass it into pwGraph2
    // **********************************************************************
	hr = pwPerformance->GetGraph(&pwGraph2);
	if(FAILED(hr))
    {
        fnsLog(BUGLOGLEVEL, "!!!! BUG: pwPerformance->GetGraph(&pwGraph2) "
                "failed (%s == %08Xh)", tdmXlatHRESULT(hr), hr);
        dwRes = FNS_FAIL;
        goto TEST_END;
    }

    // **********************************************************************
    // 18) Call SetGraph on the Second performance
    // **********************************************************************
	hr = pwPerformance2->SetGraph(pwGraph2);
	if(FAILED(hr))
    {
        fnsLog(BUGLOGLEVEL, "!!!! BUG: pwPerformance2->SetGraph(pwGraph2) "
                "failed (%s == %08Xh)", tdmXlatHRESULT(hr), hr);
        dwRes = FNS_FAIL;
        goto TEST_END;
    }

    // **********************************************************************
    // 19) Wait for IsPlaying to return or time out
    // **********************************************************************
    dwCounter = 0;
    while ((hr = pwPerformance->IsPlaying(pwSegment, NULL)) != S_OK)  
    {
        Sleep(1000);
        if(dwCounter++ > dwTimeOutValue)
        {
            fnsLog(ABORTLOGLEVEL, "**** ABORT: PlaySegment() timed out"
                "IsPlaying did not return the Segment (%s == %08Xh)", tdmXlatHRESULT(hr), hr);
            dwRes = FNS_ABORTED;
            goto TEST_END;
        }
    }

    Sleep(1000);
	
    // **********************************************************************
    // 20) Call PlaySegment on the second performance
    // **********************************************************************
	hr = pwPerformance2->PlaySegment(pwSegment2, 0, 0, NULL);
    if(FAILED(hr))
    {
        fnsLog(ABORTLOGLEVEL, "**** ABORT: pwPerformance2->PlaySegment "
                "failed (%s == %08Xh)", tdmXlatHRESULT(hr), hr);
        dwRes = FNS_ABORTED;
        goto TEST_END;
    }
	Sleep(5000);

TEST_END:
    // **********************************************************************
    // 21) Release objects, CoUninitialize, and return results
    // **********************************************************************
    if(pwPerformance)
    {
        if (pdmusPMSG)
            pwPerformance->FreePMsg((DMUS_PMSG *) pdmusPMSG);

        pwPerformance->Stop(NULL, NULL, 0, 0);
        pwPerformance->CloseDown();
        pwPerformance->Release();
        pwPerformance = NULL;
    }
	if(pwPerformance2)
	{
        pwPerformance2->Stop(NULL, NULL, 0, 0);
		pwPerformance2->CloseDown();
		pwPerformance2->Release();
		pwPerformance2 = NULL;
	}
    if(pwTool)
    {
        pwTool->Release();
        pwTool = NULL;
    }
    if(pwGraph)
    {
        pwGraph->Release();
        pwGraph = NULL;
    }
    if(pwSegment)
    {
        pwSegment->Release();
        pwSegment = NULL;
    }
    if(pwPort)
    {
        pwPort->Release();
        pwPort = NULL;
    }
    if(pwDM)
    {
        pwDM->Release();
        pwDM = NULL;
    }
	if(pwSegment2)
	{
		pwSegment2->Release();
		pwSegment2 = NULL;
	}
	if(pwGraph2)
	{
		pwGraph2->Release();
		pwGraph2 = NULL;
	}
    dmthCoUninitialize();   
    return dwRes;
}


//--------------------------------------------------------------------------;
// @doc IDIRECTMUSICPERFORMANCE A-LLUCAR
//
// @topic IDirectMusicPerformance::SendPMsg()_Valid_Test |
//
// Description: <nl>
// Valid test function for IDirectMusicPerformance::SendPMsg().
//		Send a PMsg that's already been sent.
//
// Test Function: <nl>
// tdmperfvSendPMsgValidTest2 <nl>
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
// 2) Create CtIDirectMusicPerformance object <nl>
// 3) Init the Performance <nl>
// 4) Call AllocPMsg for Note <nl>
// 5) Set up pMSG's for Note <nl>
// 6) Call SendPMsg for Note and check result <nl>
// 7) Call SendPMsg again and look for return DMUS_E_ALREADY_SENT <nl>
// 8) Cleanup objects, uninitialize COM and return results <nl>
//
// History: <nl>
//  07/23/1998 - a-llucar - created <nl>
//
// @index topic | IDIRECTMUSICPERFORMANCE_CASES
//--------------------------------------------------------------------------;
DWORD tdmperfvSendPMsgValidTest2(BOOL fCoCreate, BOOL fMaxDebug, 
						   LPARAM dwParam1, LPARAM dwParam2)
{
	DWORD							dwRes			= FNS_PASS;
	HRESULT							hr				= S_OK;
	DMUS_NOTE_PMSG					*pNote			= NULL;
	CtIDirectMusic					*pwMusic		= NULL;
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
		fnsLog(ABORTLOGLEVEL, TEXT("*** ABORT: Performance->Init ")
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
    // 4) Call AllocPMsg on pNote and check result
    // **********************************************************************
	hr = pwPerformance->AllocPMsg( sizeof(DMUS_NOTE_PMSG), (DMUS_PMSG**)&pNote );
	if(FAILED(hr))
	{
        fnsLog(BUGLOGLEVEL, TEXT("!!!! BUG: IDirectMusicPerformance->AllocPMsg(pNote) ")
                TEXT("failed (%s == %08Xh)"),
                tdmXlatHRESULT(hr), hr);
		dwRes = FNS_FAIL;
		goto TEST_END;
	}

	// Check to make sure that pNote was properly populated
	if(!pNote)
	{
        fnsLog(BUGLOGLEVEL, TEXT("!!!! BUG: AllocPMsg did not properly set pNote, \n")
                TEXT("pNote == NULL"));
		dwRes = FNS_FAIL;
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
    // 6) Call SendPMsg on pNote and check results
    // **********************************************************************	
	fnsLog(FYILOGLEVEL, TEXT("We expect this to FAIL!"));
	hr = pwPerformance->SendPMsg( (DMUS_PMSG*)pNote );
	if(FAILED(hr))
	{
        fnsLog(BUGLOGLEVEL, TEXT("!!!! BUG: IDirectMusicPerformance->SendPMsg(pNote) ")
                TEXT("failed (%s == %08Xh)"),
                tdmXlatHRESULT(hr), hr);
		dwRes = FNS_FAIL;
		goto TEST_END;
	}

    // **********************************************************************
    // 7) Call SendPMsg on pNote again and check results
    // **********************************************************************	
	hr = pwPerformance->SendPMsg( (DMUS_PMSG*)pNote );
	//BUGBUG - This should really be DMUS_E_ALREADY_SENT, but we're still in the non-forked stuff!
	if(hr != DMUS_E_ALREADY_SENT)
//	if(SUCCEEDED(hr))
	{
        fnsLog(BUGLOGLEVEL, TEXT("!!!! BUG: IDirectMusicPerformance->SendPMsg (same message second time) ")
                TEXT("did not return DMUS_E_ALREADY_SENT (%s == %08Xh)"),
                tdmXlatHRESULT(hr), hr);
		dwRes = FNS_FAIL;
		goto TEST_END;
	}

TEST_END:
    // **********************************************************************
    // 8) Cleanup objects and uninitialize COM and Return results
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
// @topic IDirectMusicPerformance::GetQueueTime()_Valid_Test |
//
// Description: <nl>
// Valid test function for IDirectMusicPerformance::GetQueueTime().
//		Calls GetQueueTime without Init'ing the port.  Expects DMUS_E_NO_MASTER_CLOCK.
//
// Test Function: <nl>
// tdmperfvGetQueueTimeValidTest1 <nl>
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
// 2) Create CtIDirectMusicPerformance object <nl>
// 3) Call GetQueueTime (expect DMUS_E_NO_MASTER_CLOCK) <nl>
// 4) Cleanup objects, uninitialize COM and return results <nl>
//
// History: <nl>
//  07/23/1998 - a-llucar - created <nl>
//
// @index topic | IDIRECTMUSICPERFORMANCE_CASES
//--------------------------------------------------------------------------;
DWORD tdmperfvGetQueueTimeValidTest1(BOOL fCoCreate, BOOL fMaxDebug, 
						   LPARAM dwParam1, LPARAM dwParam2)
{
	DWORD							dwRes			= FNS_PASS;
	HRESULT							hr				= S_OK;
	CtIDirectMusicPerformance		*pwPerformance	= NULL;
	REFERENCE_TIME					rtTime			= 0;

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
    // 3) Call GetQueueTime (expect DMUS_E_NO_MASTER_CLOCK)
    // **********************************************************************
	fnsLog(FYILOGLEVEL, TEXT("We expect this to FAIL!"));
	hr = pwPerformance->GetQueueTime(&rtTime);
	if(hr != DMUS_E_NO_MASTER_CLOCK)
	{
        fnsLog(BUGLOGLEVEL, TEXT("!!!! BUG: GetQueueTime ")
                TEXT("Did not return DMUS_E_NO_MASTER_CLOCK (%s == %08Xh)"),
                tdmXlatHRESULT(hr), hr);
		dwRes = FNS_FAIL;
		goto TEST_END;
	}

TEST_END:
    // **********************************************************************
    // 4) Cleanup objects and uninitialize COM and Return results
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
// @topic IDirectMusicPerformance::GetQueueTime()_Valid_Test |
//
// Description: <nl>
// Valid test function for IDirectMusicPerformance::GetQueueTime().
//		Calls GetQueueTime before and after Playing segment. <nl>
//		Expects them to be relatively close to each other. <nl>
//
// Test Function: <nl>
// tdmperfvGetQueueTimeValidTest2 <nl>
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
// 2) Create CtIDirectMusicPerformance object <nl>
// 3) Set Bumper Length to 0 <nl>
// 4) Load Segment <nl>
// 5) PrePlay <nl>
// 6) Call GetQueueTime and store value <nl>
// 7) PlaySegment <nl>
// 8) Call GetQueueTime again, and compare values <nl>
// 9) Cleanup objects, uninitialize COM and return results <nl>
//
// History: <nl>
//  07/23/1998 - a-llucar - created <nl>
//
// @index topic | IDIRECTMUSICPERFORMANCE_CASES
//--------------------------------------------------------------------------;
DWORD tdmperfvGetQueueTimeValidTest2(BOOL fCoCreate, BOOL fMaxDebug, 
						   LPARAM dwParam1, LPARAM dwParam2)
{
	DWORD							dwRes			= FNS_PASS;
	HRESULT							hr				= S_OK;
	CtIDirectMusicPerformance		*pwPerformance	= NULL;
	CtIDirectMusicSegment			*pwSegment		= NULL;
	REFERENCE_TIME					rtTime			= 0;
	REFERENCE_TIME					rtTime2			= 0;
	MUSIC_TIME						mtTime			= 0;
	MUSIC_TIME						mtTime2			= 0;
	MUSIC_TIME						mtTimeNow		= 0;
	MUSIC_TIME						mtTimeNow2		= 0;
	MUSIC_TIME						mtTotalTime		= 0;
	MUSIC_TIME						mtTotalTime2	= 0;

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
    // 3) SetBumperLength to zero
    // **********************************************************************
	hr = pwPerformance->SetBumperLength(0);
	if(FAILED(hr))
	{
        fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: Performance->SetBumperLength ")
                TEXT("failed (%s == %08Xh)"),
                tdmXlatHRESULT(hr), hr);
		dwRes = FNS_ABORTED;
		goto TEST_END;
	}
	
	// **********************************************************************
    // 4) Load a Segment Object
    // **********************************************************************
   	hr = dmthLoadSegment(L"DSegTest3.sgt", &pwSegment);
	if (FAILED(hr))
	{
		fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: dmthLoadSegment ")
				TEXT("failed (%s == %08Xh)"),
				tdmXlatHRESULT(hr), hr);
		dwRes = FNS_ABORTED;
		goto TEST_END;
	}

    // **********************************************************************
    // 5) PrePlay Performance
    // **********************************************************************
	hr = pwPerformance->Init(NULL,NULL,ghWnd);
	if (FAILED(hr))
	{
		fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: Init(NULL) ")
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
	if (FAILED(hr))
	{
		fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: AddPort(NULL) ")
				TEXT("failed (%s == %08Xh)"),
				tdmXlatHRESULT(hr), hr);
		dwRes = FNS_ABORTED;
		goto TEST_END;
	}

    // **********************************************************************
    // 6) Call GetQueueTime 
    // **********************************************************************
	hr = pwPerformance->GetTime(NULL, &mtTimeNow);
	if (FAILED(hr))
	{
        fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: GetTime ")
                TEXT("failed (%s == %08Xh)"),
                tdmXlatHRESULT(hr), hr);
		dwRes = FNS_ABORTED;
		goto TEST_END;
	}

	hr = pwPerformance->GetQueueTime(&rtTime);
	if (FAILED(hr))
	{
        fnsLog(BUGLOGLEVEL, TEXT("!!!! BUG: GetQueueTime ")
                TEXT("failed (%s == %08Xh)"),
                tdmXlatHRESULT(hr), hr);
		dwRes = FNS_FAIL;
		goto TEST_END;
	}

	hr = pwPerformance->ReferenceToMusicTime(rtTime, &mtTime);
	if (FAILED(hr))	
	{
		fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: ReferenceToMusicTime ")
                TEXT("failed (%s == %08Xh)"),
                tdmXlatHRESULT(hr), hr);
		dwRes = FNS_ABORTED;
		goto TEST_END;
	}
	mtTotalTime = mtTime - mtTimeNow;

	// **********************************************************************
    // 7) Play the segment
    // **********************************************************************
	hr = pwPerformance->PlaySegment(pwSegment, 0, 0, NULL);
	if (FAILED(hr))
	{
		fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: PlaySegment ")
				TEXT("failed (%s == %08Xh)"),
				tdmXlatHRESULT(hr), hr);
		dwRes = FNS_ABORTED;
		goto TEST_END;
	}

    // **********************************************************************
    // 8) Call GetQueueTime and compare two QueueTimes
    // **********************************************************************
	hr = pwPerformance->GetTime(NULL, &mtTimeNow2);
	if (FAILED(hr))
	{
        fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: GetTime ")
                TEXT("failed (%s == %08Xh)"),
                tdmXlatHRESULT(hr), hr);
		dwRes = FNS_ABORTED;
		goto TEST_END;
	}

	hr = pwPerformance->GetQueueTime(&rtTime2);
	if (FAILED(hr))
	{
        fnsLog(BUGLOGLEVEL, TEXT("!!!! BUG: GetQueueTime ")
                TEXT("failed (%s == %08Xh)"),
                tdmXlatHRESULT(hr), hr);
		dwRes = FNS_FAIL;
		goto TEST_END;
	}

	hr = pwPerformance->ReferenceToMusicTime(rtTime2, &mtTime2);
	if (FAILED(hr))
	{
        fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: ReferenceToMusicTime ")
                TEXT("failed (%s == %08Xh)"),
                tdmXlatHRESULT(hr), hr);
		dwRes = FNS_ABORTED;
		goto TEST_END;
	}
	mtTotalTime2 = mtTime2 - mtTimeNow2;

	//BUGBUG this seems to be swayed HEAVILY by the logging in tdmusic.  If we log 10, the difference is
	// minimal, but logging 1 and there about 250 apart from each other.
	if((mtTotalTime2 - mtTotalTime) > 250)
	{
        fnsLog(BUGLOGLEVEL, TEXT("!!!! BUG: mtTotalTime and mtTotalTime2 are not close to each other."));
		fnsLog(BUGLOGLEVEL, TEXT("mtTotalTime  == %u"), mtTotalTime);
		fnsLog(BUGLOGLEVEL, TEXT("mtTotalTime2 == %u"), mtTotalTime2);
		dwRes = FNS_FAIL;
		goto TEST_END;
	}

TEST_END:
    // **********************************************************************
    // 9) Cleanup objects and uninitialize COM and Return results
    // **********************************************************************
	if(pwPerformance)
	{
		pwPerformance->CloseDown();
		pwPerformance->Release();
		pwPerformance = NULL;
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
// @topic IDirectMusicPerformance::GetTime()_Valid_Test |
//
// Description: <nl>
// Valid test function for IDirectMusicPerformance::GetTime().
//		Calls GetTime passing in both mtTime and rtTime. <nl>
//		Expects them both to be populated. <nl>
//
// Test Function: <nl>
// tdmperfvGetTimeValidTest2 <nl>
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
// 2) Create CtIDirectMusicPerformance object <nl>
// 3) Init the performance <nl>
// 4) Call GetTime and check results <nl>
// 5) Cleanup objects, uninitialize COM and return results <nl>
//
// History: <nl>
//  07/23/1998 - a-llucar - created <nl>
//
// @index topic | IDIRECTMUSICPERFORMANCE_CASES
//--------------------------------------------------------------------------;
DWORD tdmperfvGetTimeValidTest2(BOOL fCoCreate, BOOL fMaxDebug, 
						   LPARAM dwParam1, LPARAM dwParam2)
{
	DWORD							dwRes			= FNS_PASS;
	HRESULT							hr				= S_OK;
	CtIDirectMusicPerformance		*pwPerformance	= NULL;
	REFERENCE_TIME					rtTime			= 0;
	MUSIC_TIME						mtTime			= 0;

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
    // 3) Init the performance
    // **********************************************************************
    hr = pwPerformance->Init(NULL,NULL,ghWnd);
	if(FAILED(hr))
	{
        fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: Performance->Init(NULL) ")
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
    // 4) Call GetTime passing in both mtTime and rtTime
    // **********************************************************************
	Sleep(100);
	hr = pwPerformance->GetTime(&rtTime, &mtTime);
	if(FAILED(hr))
	{
		fnsLog(BUGLOGLEVEL, TEXT("!!!! BUG: Performance->GetTime ")
				TEXT("failed (%s == %08Xh)"),
				tdmXlatHRESULT(hr), hr);
		dwRes = FNS_FAIL;
		goto TEST_END;
	}
	if (!rtTime)	//rtTime was not properly set
	{
		fnsLog(BUGLOGLEVEL, TEXT("!!!! BUG: Performance->GetTime ")
				TEXT("rtTime was not properly set"));
		dwRes = FNS_FAIL;
		goto TEST_END;
	}
	if (!mtTime)	//mtTime was not properly set
	{
		fnsLog(BUGLOGLEVEL, TEXT("!!!! BUG: Performance->GetTime ")
				TEXT("mtTime was not properly set"));
		dwRes = FNS_FAIL;
		goto TEST_END;
	}
	
TEST_END:
    // **********************************************************************
    // 5) Cleanup objects and uninitialize COM and Return results
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
// @topic IDirectMusicPerformance::Init()_Valid_Test |
//
// Description: <nl>
// Valid test function for IDirectMusicPerformance::Init().
//		Calls Init on a DirectMusic object that was already created. <nl>
//		Expects return DMUS_E_ALREADY_INITED. <nl>
//
// Test Function: <nl>
// tdmperfvInitValidTest2 <nl>
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
// 2) Create CtIDirectMusicPerformance object <nl>
// 3) Create a CtIDirectMusic object <nl>
// 4) Init the performance using our Music object <nl>
// 5) Cleanup objects, uninitialize COM and return results <nl>
//
// History: <nl>
//  07/23/1998 - a-llucar - created <nl>
//
// @index topic | IDIRECTMUSICPERFORMANCE_CASES
//--------------------------------------------------------------------------;
DWORD tdmperfvInitValidTest2(BOOL fCoCreate, BOOL fMaxDebug, 
						   LPARAM dwParam1, LPARAM dwParam2)
{
	DWORD							dwRes			= FNS_PASS;
	HRESULT							hr				= S_OK;
	CtIDirectMusicPerformance		*pwPerformance	= NULL;
//	CtIDirectMusic					*pwMusic		= NULL;

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
    // 4) Init the performance
    // **********************************************************************
	hr = pwPerformance->Init(NULL,NULL,ghWnd);
	if(FAILED(hr))
	{
        fnsLog(BUGLOGLEVEL, TEXT("!!!! BUG: DirectMusicSegment object creation ")
                TEXT("failed (%s == %08Xh)"),
                tdmXlatHRESULT(hr), hr);
		dwRes = FNS_FAIL;
		goto TEST_END;
	}

    fnsLog(FYILOGLEVEL, TEXT("Note: We Expect this to FAIL."));
	hr = pwPerformance->Init(NULL,NULL,ghWnd);
	if(hr != DMUS_E_ALREADY_INITED)
	{
        fnsLog(BUGLOGLEVEL, TEXT("!!!! BUG: Performance->Init(Second call Init) ")
                TEXT("expected DMUS_E_ALREADY_INITED (%s == %08Xh)"),
                tdmXlatHRESULT(hr), hr);
		dwRes = FNS_FAIL;
		goto TEST_END;
	}
	
TEST_END:
    // **********************************************************************
    // 5) Cleanup objects and uninitialize COM and Return results
    // **********************************************************************
	if(pwPerformance)
	{
		pwPerformance->CloseDown();
		pwPerformance->Release();
		pwPerformance = NULL;
	}
//	if(pwMusic)
//	{
//		pwMusic->Release();
//		pwMusic = NULL;
//	}
	dmthCoUninitialize();	
	return dwRes;
}


//--------------------------------------------------------------------------;
// @doc IDIRECTMUSICPERFORMANCE A-LLUCAR
//
// @topic IDirectMusicPerformance::GetLatencyTime()_Valid_Test |
//
// Description: <nl>
// Valid test function for IDirectMusicPerformance::GetLatencyTime().
//		Calls GetLatencyTime after playing a segment and then after SetBumperLength to 0. <nl>
//		Expects latter to be smaller amount
//
// Test Function: <nl>
// tdmperfvGetLatencyTimeValidTest2 <nl>
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
// 2) Create CtIDirectMusicPerformance object <nl>
// 3) Load Segment <nl>
// 4) PrePlay <nl>
// 5) PlaySegment <nl>
// 6) Call GetLatencyTime and stores value <nl>
// 7) Set Bumper Length to 0 <nl>
// 8) Call GetLatencyTime again, and compare values <nl>
// 9) Cleanup objects, uninitialize COM and return results <nl>
//
// History: <nl>
//  07/23/1998 - a-llucar - created <nl>
//  07/23/1999 - kcraven - edited <nl>
//
// @index topic | IDIRECTMUSICPERFORMANCE_CASES
//--------------------------------------------------------------------------;
DWORD tdmperfvGetLatencyTimeValidTest2(BOOL fCoCreate, BOOL fMaxDebug, 
						   LPARAM dwParam1, LPARAM dwParam2)
{
	DWORD							dwRes			= FNS_PASS;
	HRESULT							hr				= S_OK;
	CtIDirectMusicPerformance		*pwPerformance	= NULL;
	CtIDirectMusicSegment			*pwSegment		= NULL;
	REFERENCE_TIME					rtTime			= 0;
	REFERENCE_TIME					rtTime2			= 0;

	DWORD							dwMilliseconds	= 0;

	REFERENCE_TIME					rtTimeNow		= 0;
	REFERENCE_TIME					rtTimeNow2		= 0;
	REFERENCE_TIME					rtTotalTime		= 0;
	REFERENCE_TIME					rtTotalTime2	= 0;

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
    // 3) Load a Segment Object
    // **********************************************************************
   	hr = dmthLoadSegment(L"DSegTest3.sgt", &pwSegment);
	if (FAILED(hr))
	{
		fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: dmthLoadSegment ")
				TEXT("failed (%s == %08Xh)"),
				tdmXlatHRESULT(hr), hr);
		dwRes = FNS_ABORTED;
		goto TEST_END;
	}

    // **********************************************************************
    // 4) PrePlay Performance
    // **********************************************************************
	hr = pwPerformance->Init(NULL,NULL,ghWnd);
	if (FAILED(hr))
	{
		fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: Init(NULL) ")
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
	if (FAILED(hr))
	{
		fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: AddPort(NULL) ")
				TEXT("failed (%s == %08Xh)"),
				tdmXlatHRESULT(hr), hr);
		dwRes = FNS_ABORTED;
		goto TEST_END;
	}

	// **********************************************************************
    // 5) Play the segment
    // **********************************************************************
	hr = pwPerformance->PlaySegment(pwSegment, 0, 0, NULL);
	if (FAILED(hr))
	{
		fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: PlaySegment ")
				TEXT("failed (%s == %08Xh)"),
				tdmXlatHRESULT(hr), hr);
		dwRes = FNS_ABORTED;
		goto TEST_END;
	}
	hr = pwPerformance->GetBumperLength(&dwMilliseconds);
	if (FAILED(hr))
	{
		fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: Performance->GetBumperLength ")
				TEXT("failed (%s == %08Xh)"),
				tdmXlatHRESULT(hr), hr);
		dwRes = FNS_ABORTED;
		goto TEST_END;
	}

	fnsLog(FYILOGLEVEL, TEXT("**** FYI: dwMilliseconds == %u"), dwMilliseconds);

    // **********************************************************************
    // 6) Call GetLatencyTime 
    // **********************************************************************

	hr = pwPerformance->GetTime(&rtTimeNow,NULL);
	if (FAILED(hr))
	{
        fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: GetTime ")
                TEXT("failed (%s == %08Xh)"),
                tdmXlatHRESULT(hr), hr);
		dwRes = FNS_ABORTED;
		goto TEST_END;
	}

	hr = pwPerformance->GetLatencyTime(&rtTime);
	if (FAILED(hr))
	{
        fnsLog(BUGLOGLEVEL, TEXT("!!!! BUG: GetLatencyTime ")
                TEXT("failed (%s == %08Xh)"),
                tdmXlatHRESULT(hr), hr);
		dwRes = FNS_FAIL;
		goto TEST_END;
	}

	rtTotalTime = rtTime - rtTimeNow;

    // **********************************************************************
    // 7) SetBumperLength to zero
    // **********************************************************************
	hr = pwPerformance->SetBumperLength(0);
	if(FAILED(hr))
	{
        fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: Performance->SetBumperLength ")
                TEXT("failed (%s == %08Xh)"),
                tdmXlatHRESULT(hr), hr);
		dwRes = FNS_ABORTED;
		goto TEST_END;
	}

	hr = pwPerformance->GetBumperLength(&dwMilliseconds);
	if (FAILED(hr))
	{
		fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: Performance->GetBumperLength ")
				TEXT("failed (%s == %08Xh)"),
				tdmXlatHRESULT(hr), hr);
		dwRes = FNS_ABORTED;
		goto TEST_END;
	}

	fnsLog(FYILOGLEVEL, TEXT("**** FYI: dwMilliseconds == %u"), dwMilliseconds);

    // **********************************************************************
    // 8) Call GetLatencyTime and compare two LatencyTimes
    // **********************************************************************

	hr = pwPerformance->GetTime(&rtTimeNow2,NULL);
	if (FAILED(hr))
	{
        fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: GetTime ")
                TEXT("failed (%s == %08Xh)"),
                tdmXlatHRESULT(hr), hr);
		dwRes = FNS_ABORTED;
		goto TEST_END;
	}

	hr = pwPerformance->GetLatencyTime(&rtTime2);
	if (FAILED(hr))
	{
        fnsLog(BUGLOGLEVEL, TEXT("!!!! BUG: GetLatencyTime ")
                TEXT("failed (%s == %08Xh)"),
                tdmXlatHRESULT(hr), hr);
		dwRes = FNS_FAIL;
		goto TEST_END;
	}

	rtTotalTime2 = rtTime2 - rtTimeNow2;

	//convert to millisecs
	rtTotalTime /= 10000;
	rtTotalTime2 /= 10000;

	fnsLog(FYILOGLEVEL, TEXT("rtTotalTime  == %d"), rtTotalTime);
	fnsLog(FYILOGLEVEL, TEXT("rtTotalTime2 == %d"), rtTotalTime2);

	if(abs((int)(rtTotalTime2 - rtTotalTime)) > 2000) // 2 second swing
	{
        fnsLog(BUGLOGLEVEL, TEXT("!!!! BUG: rtTotalTime2 is significantly different than rtTotalTime."));
		fnsLog(BUGLOGLEVEL, TEXT("Meaning that LatencyTime is not accurate"));
		dwRes = FNS_FAIL;
	}

TEST_END:
    // **********************************************************************
    // 9) Cleanup objects and uninitialize COM and Return results
    // **********************************************************************
	if(pwPerformance)
	{
		pwPerformance->CloseDown();
		pwPerformance->Release();
		pwPerformance = NULL;
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
// @topic IDirectMusicPerformance::GetGlobalParam()_Valid_Test |
//
// Description: <nl>
// Valid test function for IDirectMusicPerformance::GetGlobalParam(). <nl>
//		Passes all different types of guids.
//
// Test Function: <nl>
// tdmperfvGetGlobalParamValidTest1 <nl>
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
// 2) Create CtIDirectMusicPerformance object <nl>
// 3) Create CtIDirectMusicComposer object <nl>
// 4) Load the media <nl>
// 5) Compose the Segment <nl>
// 6) Pre-Play the performance <nl>
// 7) Play the segment <nl> 
// 8) Call SetGlobalParam, check results<nl>
// 9) Call GetGlobalParam, check results (make sure dw and dw2 are equal) <nl>
// 10) Release objects, CoUninitialize, and return results <nl>
//
// History: <nl>
//	06/25/1998 - a-llucar - created
//	07/21/1998 - a-llucar - changed it so that we use the Array instead of dwParam1
//
//	Note - GetGlobalParam() and SetGlobalParam() both use the same test.
//
// @index topic | IDIRECTMUSICPERFORMANCE_CASES
//--------------------------------------------------------------------------;
DWORD tdmperfvGetGlobalParamValidTest2(BOOL fCoCreate, BOOL fMaxDebug, 
						   LPARAM dwParam1, LPARAM dwParam2)
{
	DWORD						dwRes			= FNS_PASS;
	HRESULT						hr				= S_OK;
	GUID						guid			= GUID_PerfMasterGrooveLevel; 
	BYTE						bGroove			= 100;
	BYTE						bGroove2		= 5;
	CtIDirectMusicSegment		*pwSegment		= NULL;
	CtIDirectMusicPerformance	*pwPerformance	= NULL;
	CtIDirectMusicComposer		*pwComposer		= NULL;
	CtIDirectMusicStyle			*pwStyle		= NULL;
	CtIDirectMusicChordMap	*pwChordMap	= NULL;

	int							i				= 0;
	
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
	hr = dmthLoadStyle(NULL, &pwStyle);
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
    // 5) Compose a segment
    // **********************************************************************
	hr = pwComposer->ComposeSegmentFromShape(pwStyle, 16, DMUS_SHAPET_QUIET, 0, FALSE, FALSE, pwChordMap, &pwSegment);
	if (FAILED(hr))
	{
		fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: ComposeSegmentFromShape ")
				TEXT("failed (%s == %08Xh)"),
				tdmXlatHRESULT(hr), hr);
		dwRes = FNS_ABORTED;
		goto TEST_END;
	}
    
    // **********************************************************************
    // 6) PrePlay performance
    // **********************************************************************
	hr = pwPerformance->Init(NULL,NULL,ghWnd);
	if(FAILED(hr))
	{
		fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: Performance->Init(NULL) ")
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
		fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: Performance->AddPort(NULL) ")
				TEXT("failed (%s == %08Xh)"),
				tdmXlatHRESULT(hr), hr);
		dwRes = FNS_ABORTED;
		goto TEST_END;
	}
	
    // **********************************************************************
    // 7) Play the segment
    // **********************************************************************
	hr = pwPerformance->PlaySegment(pwSegment, 0, 0, NULL);
	if(FAILED(hr))
	{
		fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: Performance->PlaySegment ")
				TEXT("failed (%s == %08Xh)"),
				tdmXlatHRESULT(hr), hr);
		dwRes = FNS_ABORTED;
		goto TEST_END;
	}
	Sleep(2000);

	// **********************************************************************
    // 8) Call SetGlobalParam, check results
    // **********************************************************************
	hr = pwPerformance->SetGlobalParam(guid, &bGroove, 1);
	if(FAILED(hr))
	{
        fnsLog(BUGLOGLEVEL, TEXT("!!!! BUG: IDirectMusicPerformance->SetGlobalParam ")
                TEXT("failed (%s == %08Xh)"),
                tdmXlatHRESULT(hr), hr);
		dwRes = FNS_FAIL;
		goto TEST_END;
	}
	Sleep(6000);

	// **********************************************************************
    // 9) Call GetGlobalParam, check results
    // **********************************************************************
	hr = pwPerformance->GetGlobalParam( GUID_PerfMasterGrooveLevel, &bGroove2, 1);
	if(FAILED(hr))
	{
        fnsLog(BUGLOGLEVEL, TEXT("!!!! BUG: IDirectMusicPerformance->GetGlobalParam ")
                TEXT("failed (%s == %08Xh)"),
                tdmXlatHRESULT(hr), hr);
		dwRes = FNS_FAIL;
		goto TEST_END;
	}

	// Check to make sure that the DWORD was properly populated
	if(bGroove != bGroove2)
	{
        fnsLog(BUGLOGLEVEL, TEXT("!!!! BUG: IDirectMusicPerformance->GetGlobalParam\n ")
                TEXT("pData was not properly set\n")
                TEXT("Expected -- bGroove == %ld\n")
				TEXT("Received -- bGroove2 == %ld\n"),
				bGroove, bGroove2);
		dwRes = FNS_FAIL;
		goto TEST_END;
	}	

TEST_END:
	// **********************************************************************
    // 10) Release objects, CoUninitialize, and return results
    // **********************************************************************
	if(pwPerformance)
	{
		pwPerformance->Stop(0, 0, NULL, NULL);
		pwPerformance->CloseDown();
		pwPerformance->Release();
        pwPerformance = NULL;
	}
	if(pwSegment)
	{
		pwSegment->Release();
		pwSegment = NULL;
	}
	if(pwComposer)
	{
		pwComposer->Release();
		pwComposer = NULL;
	}
	if(pwStyle)
	{
		pwStyle->Release();
		pwStyle = NULL;
	}
	if(pwChordMap)
	{
		pwChordMap->Release();
		pwChordMap = NULL;
	}
	dmthCoUninitialize();	
	return dwRes;
}
//===========================================================================


//--------------------------------------------------------------------------;
// @doc IDIRECTMUSICPERFORMANCE A-LLUCAR
//
// @topic IDirectMusicPerformance::PChannelInfo()_Valid_Test |
//
// Description: <nl>
// Valid test function for IDirectMusicPerformance::PChannelInfo(). <nl>
//		Calls PChannelInfo without setting up a Port <nl>
//
// Test Function: <nl>
// tdmperfvPChannelInfoValidTest1 <nl>
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
// 2) Create CtIDirectMusicPerformance object <nl>
// 3) Call PChannelInfo and check results <nl>
// 4) Check to make sure that PChannelInfo returned the right info <nl>
// 5) Release object, CoUninitialize, and return results <nl>
//
// History: <nl>
//  07/24/1998 - a-llucar - created <nl>
//
// @index topic | IDIRECTMUSICPERFORMANCE_CASES
//--------------------------------------------------------------------------;
DWORD tdmperfvPChannelInfoValidTest1(BOOL fCoCreate, BOOL fMaxDebug, 
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

    // **********************************************************************
    // 4) Call PChannelInfo and check results
    // **********************************************************************
	fnsLog(FYILOGLEVEL, TEXT("We expect this to Fail!"));
	hr = pwPerformance->PChannelInfo( 0, &pwPort, &dwGroup, &dwChannel );
	if(SUCCEEDED(hr))
	{
        fnsLog(BUGLOGLEVEL, TEXT("!!!! BUG: IDirectMusicPerformance->PChannelInfo ")
                TEXT("failed (%s == %08Xh)"),
                tdmXlatHRESULT(hr), hr);
		dwRes = FNS_FAIL;
		goto TEST_END;
	}
    
TEST_END:
    // **********************************************************************
    // 5) Release object, CoUninitialize, and return results
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
// @topic IDirectMusicPerformance::PChannelInfo()_Valid_Test |
//
// Description: <nl>
// Valid test function for IDirectMusicPerformance::PChannelInfo(). <nl>
//		Verifies that all PChannelInfo information is constant throughout. <nl>
//
// Test Function: <nl>
// tdmperfvPChannelInfoValidTest2 <nl>
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
// 2) Create CtIDirectMusicComposer object (wrapped IDMComposer object) <nl>
// 3) Call AddPort (Synth) and check results <nl>
// 4) Call PChannelInfo and check results <nl>
// 5) Release object, CoUninitialize, and return results <nl>
//
// History: <nl>
//  07/24/1998 - a-llucar - created <nl>
//
// @index topic | IDIRECTMUSICPERFORMANCE_CASES
//--------------------------------------------------------------------------;
DWORD tdmperfvPChannelInfoValidTest2(BOOL fCoCreate, BOOL fMaxDebug, 
						   LPARAM dwParam1, LPARAM dwParam2)
{
	DWORD							dwRes			= FNS_PASS;
	HRESULT							hr				= S_OK;
	CtIDirectMusicPort				*pwPort			= NULL;
	CtIDirectMusicPerformance		*pwPerformance	= NULL;
	int								i				= 0;
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
	for (i = 0; i < 17; i++)
	{
		if (pwPort)
		{
			pwPort->Release();
			pwPort = NULL;
		}
		hr = pwPerformance->PChannelInfo( 0, &pwPort, &dwGroup, &dwChannel );
		if(FAILED(hr))
		{
		    fnsLog(BUGLOGLEVEL, TEXT("!!!! BUG: IDirectMusicPerformance->PChannelInfo ")
		            TEXT("failed (%s == %08Xh)"),
		            tdmXlatHRESULT(hr), hr);
			dwRes = FNS_FAIL;
			goto TEST_END;
		}

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
    }
TEST_END:
    // **********************************************************************
    // 5) Release object, CoUninitialize, and return results
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
// @topic IDirectMusicPerformance::SetPrepareTime()_Valid_Test |
//
// Description: <nl>
// Valid test function for IDirectMusicPerformance::SetPrepareTime().
//		Sets the Prepare Time to it's maximum, checks GetPrepareTime <nl>
//
// Test Function: <nl>
// tdmperfvSetPrepareTimeValidTest2 <nl>
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
// 2) Create CtIDirectMusicPerformance object <nl>
// 3) Call SetPrepareTime and Test the function <nl>
// 4) Call GetPrepareTime and Test the function <nl>
// 5) Cleanup objects and uninitialize COM and Return results <nl>
//
// History: <nl>
//  07/24/1998 - a-llucar - created <nl>
//
//	The same test is used for GetPrepareTimeValidTest2() and SetPrepareTimeValidTest2() <nl>
//
// @index topic | IDIRECTMUSICPERFORMANCE_CASES
//--------------------------------------------------------------------------;
DWORD tdmperfvSetPrepareTimeValidTest2(BOOL fCoCreate, BOOL fMaxDebug, 
						   LPARAM dwParam1, LPARAM dwParam2)
{
	DWORD						dwRes				= FNS_PASS;
	HRESULT						hr					= S_OK;
	CtIDirectMusicPerformance	*pwPerformance		= NULL;
	DWORD						dwSet				= DWORD_MAX;
	DWORD						dwGet				= 0;

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
        fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: DirectMusicSegment object creation "),
                TEXT("failed (%s == %08Xh)"),
                tdmXlatHRESULT(hr), hr);
		dwRes = FNS_ABORTED;
		goto TEST_END;
	}

	// **********************************************************************
    // 3) Call SetPrepareTime and Test the function
    // **********************************************************************
	hr = pwPerformance->SetPrepareTime(dwSet);
	if(FAILED(hr))
	{
        fnsLog(BUGLOGLEVEL, TEXT("!!!! BUG: IDirectMusicPerformance->SetPrepareTime() ")
                TEXT("failed (%s == %08Xh)"),
                tdmXlatHRESULT(hr), hr);
		dwRes = FNS_FAIL;
		goto TEST_END;
	}

    // **********************************************************************
    // 4) Call GetPrepareTime and Test the function
    // **********************************************************************
	hr = pwPerformance->GetPrepareTime(&dwGet);
	if(FAILED(hr))
	{
        fnsLog(BUGLOGLEVEL, TEXT("!!!! BUG: IDirectMusicPerformance->GetPrepareTime() ")
                TEXT("failed (%s == %08Xh)"),
                tdmXlatHRESULT(hr), hr);
		dwRes = FNS_FAIL;
		goto TEST_END;
	}

	// Test to make sure that dwSet and dwGet are the same
	if(dwSet != dwGet)
	{
        fnsLog(BUGLOGLEVEL, TEXT("!!!! BUG: GetPrepareTime not set correctly, ")
                TEXT("dwSet == %08Xh)"),
				TEXT("dwGet == %08Xh)"),
                dwSet, dwGet);
		dwRes = FNS_FAIL;
		goto TEST_END;
	}
TEST_END:
    // **********************************************************************
    // 5) Cleanup objects and uninitialize COM and Return results
    // **********************************************************************
	if(pwPerformance)
	{
		pwPerformance->Stop(NULL, NULL, 0, 0);
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
// @topic IDirectMusicPerformance::IsPlaying()_Valid_Test |
//
// Description: <nl>
// Valid test function for IDirectMusicPerformance::IsPlaying(). <nl>
//		Calls IsPlaying on a Secondary Segment <nl>
//
// Test Function: <nl>
// tdmperfvIsPlayingValidTest2 <nl>
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
// 2) Create CtIDirectMusicPerformance object <nl>
// 3) Create CtIDirectMusicComposer object <nl>
// 4) Load the media <nl>
// 5) Compose the segment <nl>
// 6) Pre-Play performance <nl>
// 7) Play a segment <nl>
// 8) GetMotif, SetMotif, and Play Secondary Segment <nl>
// 9) Call IsPlaying, wait 1 second, and call IsPlaying again <nl> 
// 10) Release objects, CoUninitialize, and return results <nl>
//
// History: <nl>
//  07/24/1998 - a-llucar - created <nl>
//
// @index topic | IDIRECTMUSICPERFORMANCE_CASES
//--------------------------------------------------------------------------;
DWORD tdmperfvIsPlayingValidTest2(BOOL fCoCreate, BOOL fMaxDebug, 
						   LPARAM dwParam1, LPARAM dwParam2)
{
	DWORD						dwRes				= FNS_PASS;
	HRESULT						hr					= S_OK;
	CtIDirectMusicSegment		*pwSegment			= NULL;
	CtIDirectMusicSegment		*pwSegment2			= NULL;
	CtIDirectMusicPerformance 	*pwPerformance		= NULL;
	CtIDirectMusicComposer		*pwComposer			= NULL;
	CtIDirectMusicStyle			*pwStyle			= NULL;
	CtIDirectMusicChordMap	*pwChordMap		= NULL;
	CtIDirectMusicSegmentState	*pwSegmentState		= NULL;

	WCHAR						wszName[100];
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
	hr = dmthLoadStyle(NULL, &pwStyle);
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
	hr = pwComposer->ComposeSegmentFromShape(pwStyle, 16, DMUS_SHAPET_QUIET, 0, FALSE, FALSE, pwChordMap, &pwSegment);
	if (FAILED(hr))
	{
		fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: ComposeSegmentFromShape ")
				TEXT("failed (%s == %08Xh)"),
				tdmXlatHRESULT(hr), hr);
		dwRes = FNS_ABORTED;
		goto TEST_END;
	}
		
    // **********************************************************************
    // 6) PrePlay performance
    // **********************************************************************
	hr = pwPerformance->Init(NULL,NULL,ghWnd);
	if(FAILED(hr))
	{
		fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: Performance->Init(NULL) ")
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
		fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: Performance->AddPort(NULL) ")
				TEXT("failed (%s == %08Xh)"),
				tdmXlatHRESULT(hr), hr);
		dwRes = FNS_ABORTED;
		goto TEST_END;
	}
	
    // **********************************************************************
    // 7) Play the segment
    // **********************************************************************
	hr = pwPerformance->PlaySegment(pwSegment, 0, 0, NULL);
	if(FAILED(hr))
	{
		fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: Performance->PlaySegment ")
				TEXT("failed (%s == %08Xh)"),
				tdmXlatHRESULT(hr), hr);
		dwRes = FNS_ABORTED;
		goto TEST_END;
	}
	Sleep(2000);

    // **********************************************************************
    // 8) GetMotif, SetMotif, and Play Secondary Segment
    // **********************************************************************
	hr = pwStyle->EnumMotif(1, wszName);
	if(FAILED(hr))
	{
		fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: Style->EnumMotif ")
				TEXT("failed (%s == %08Xh)"),
				tdmXlatHRESULT(hr), hr);
		dwRes = FNS_ABORTED;
		goto TEST_END;
	}

	hr = pwStyle->GetMotif(wszName, &pwSegment2);
	if(FAILED(hr))
	{
		fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: Style->GetMotif ")
				TEXT("failed (%s == %08Xh)"),
				tdmXlatHRESULT(hr), hr);
		dwRes = FNS_ABORTED;
		goto TEST_END;
	}

	hr = pwPerformance->PlaySegment(pwSegment2, DMUS_SEGF_SECONDARY, 0, &pwSegmentState);
	if(FAILED(hr))
	{
		fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: Performance->PlaySegment ")
				TEXT("failed (%s == %08Xh)"),
				tdmXlatHRESULT(hr), hr);
		dwRes = FNS_ABORTED;
		goto TEST_END;
	}

    // **********************************************************************
    // 9) Call IsPlaying on Sec Segment, wait 1 second, call IsPlaying again
    // **********************************************************************
	hr = pwPerformance->IsPlaying(NULL, pwSegmentState);
	if(FAILED(hr))
	{
		fnsLog(BUGLOGLEVEL, TEXT("!!!! BUG: Performance->IsPlaying ")
				TEXT("failed (%s == %08Xh)"),
				tdmXlatHRESULT(hr), hr);
		dwRes = FNS_FAIL;
		goto TEST_END;
	}

	Sleep(1000);
	hr = pwPerformance->IsPlaying(pwSegment, NULL);
	if(FAILED(hr))
	{
		fnsLog(BUGLOGLEVEL, TEXT("!!!! BUG: Performance->IsPlaying ")
				TEXT("failed (%s == %08Xh)"),
				tdmXlatHRESULT(hr), hr);
		dwRes = FNS_FAIL;
		goto TEST_END;
	}

TEST_END:
    // **********************************************************************
    // 10) Release objects, CoUninitialize, and return results 
    // **********************************************************************
	if(pwPerformance)
	{
		pwPerformance->Stop(NULL, NULL, 0, 0);
		pwPerformance->CloseDown();
		pwPerformance->Release();
        pwPerformance = NULL;
	}
	if(pwSegment)
	{
		pwSegment->Release();
		pwSegment = NULL;
	}
	if(pwSegment2)
	{
		pwSegment2->Release();
		pwSegment2 = NULL;
	}
	if(pwStyle)
	{
		pwStyle->Release();
		pwStyle = NULL;
	}
	if(pwChordMap)
	{
		pwChordMap->Release();
		pwChordMap = NULL;
	}
	if(pwComposer)
	{
		pwComposer->Release();
		pwComposer = NULL;
	}
	if(pwSegmentState)
	{
		pwSegmentState->Release();
		pwSegmentState = NULL;
	}
	dmthCoUninitialize();	
	return dwRes;
}



//--------------------------------------------------------------------------;
// @doc IDIRECTMUSICPERFORMANCE A-LLUCAR
//
// @topic IDirectMusicPerformance::SetParam()_Valid_Test |
//
// Description: <nl>
// Valid test function for IDirectMusicPerformance::SetParam() <nl>
//			Change the tempo of a Performance that is playing, <nl>
//			And already altered by SetGlobalParam <nl>
//
// Test Function: <nl>
// tdmperfvGetParamValidTest2 <nl>
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
// 2) Create CtIDirectMusicPerformance object <nl>
// 3) Create IDirectMusicTempoTrack object <nl>
// 4) Get the Segment object <nl>
// 5) Init(NULL) and AddPort(NULL) <nl>
// 6) Set pTempo with a bunch of particulars <nl>
// 7) Call PlaySegment and check results <nl>
// 8) Call SetGlobalParam, check results <nl>
// 9) Call SetParam, check the results <nl>
// 10) Call GetParam, check the results <nl>
// 11) Wait for IsPlaying to return favorably (or time out) <nl>
// 12) Release objects, CoUninitialize, and return results <nl>
//
// History:
//  07/27/1998 - a-llucar - created <nl>
//
//	NOTE:  The same test is used for SetParamValidTest2 and GetParamValidTest2 <nl>
//
// @index topic | IDIRECTMUSICPERFORMANCE_CASES
//--------------------------------------------------------------------------;
DWORD tdmperfvGetParamValidTest2(BOOL fCoCreate, BOOL fMaxDebug, LPARAM dwParam1, LPARAM dwParam2)
{
	DWORD						dwRes			= FNS_PASS;
	HRESULT						hr				= E_NOTIMPL;
	CtIDirectMusicSegment		*pwSegment		= NULL;
	CtIDirectMusicPerformance	*pwPerformance	= NULL;
	CtIDirectMusicTrack			*pwTrack		= NULL;
	float						fTempo			= .5; //half tempo
	int							counter			= 0;
	int							SleepTime		= 1000;
	int							TimeOutValue	= 10;
	GUID						guid			= GUID_TempoParam;
	DMUS_TEMPO_PARAM			dmtp;
	WCHAR						*pSegmentFile	= (WCHAR*)dwParam1;
	char						*chPath			= gszMediaPath;
	WCHAR						*wchPath		= gwszBasePath;
	dmthSetPath(NULL);
	MUSIC_TIME					mt				= 0;

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
    // 3) Create IDirectMusicTempoTrack object 
    // **********************************************************************	
	hr = dmthCreateTrack(CLSID_DirectMusicTempoTrack, IID_IDirectMusicTrack, &pwTrack);
	if(FAILED(hr))
	{
		fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: CoCreate TempoTrack failed! (%s == %08Xh)"),
			tdmXlatHRESULT(hr), hr);
		dwRes = FNS_ABORTED;
		goto TEST_END;
	}
	
	// **********************************************************************
    // 4) Get the Segment object
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
    // 5) Init(NULL) and AddPort(NULL)
    // **********************************************************************
	hr = pwPerformance->Init(NULL,NULL,ghWnd);
	if(FAILED(hr))
	{
        fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: pwPerformance->Init(NULL) ")
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
        fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: pwPerformance->AddPort(NULL) ")
                TEXT("failed (%s == %08Xh)"),
                tdmXlatHRESULT(hr), hr);
		dwRes = FNS_ABORTED;
		goto TEST_END;
	}

	pwSegment->SetRepeats(5);

    // **********************************************************************
	// 6) Set pTempo with a bunch of particulars
    // **********************************************************************	
	ZeroMemory( &dmtp, sizeof(dmtp) );
	dmtp.dblTempo = 300;

	// **********************************************************************
    // 7) Call PlaySegment and check results
    // **********************************************************************
	hr = pwPerformance->PlaySegment(pwSegment, 0, 0, NULL);
	if(FAILED(hr))
	{
        fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: pwPerformance->PlaySegment ")
                TEXT("failed (%s == %08Xh)"),
                tdmXlatHRESULT(hr), hr);
		dwRes = FNS_ABORTED;
		goto TEST_END;
	}
	
	pwPerformance->GetTime(NULL, &mt);
	fnsLog(FYILOGLEVEL, TEXT("Time Before = %ld"), mt);	

	// **********************************************************************
    // 8) Call SetGlobalParam, check results
    // **********************************************************************
	hr = pwPerformance->SetGlobalParam(GUID_PerfMasterTempo, &fTempo, sizeof(float));
	if(FAILED(hr))
	{
        fnsLog(BUGLOGLEVEL, TEXT("!!!! BUG: IDirectMusicPerformance->SetGlobalParam ")
                TEXT("failed (%s == %08Xh)"),
                tdmXlatHRESULT(hr), hr);
		dwRes = FNS_FAIL;
		goto TEST_END;
	}

   	// **********************************************************************
    // 9) Call SetParam, check the results 
    // **********************************************************************
	hr = pwPerformance->SetParam(guid, 0xffffffff, 0, mt + 2000, (void*)&dmtp); 
	if(FAILED(hr))
	{
		fnsLog(BUGLOGLEVEL, TEXT("!!!! BUG: IDirectMusicPerformance->SetParam ")
				TEXT("failed (%s == %08Xh)"),
				tdmXlatHRESULT(hr), hr);
		dwRes = FNS_FAIL;
		goto TEST_END;
	}   

	pwPerformance->GetTime(NULL, &mt);
	fnsLog(FYILOGLEVEL, TEXT("Time After = %ld"), mt);	

   	// **********************************************************************
    // 10) Call GetParam, check the results 
    // **********************************************************************
	hr = pwSegment->GetParam(guid, 0xffffffff, 0, mt + 2000, 0, (void*)&dmtp); 
	if(FAILED(hr))
	{
		fnsLog(BUGLOGLEVEL, TEXT("!!!! BUG: IDirectMusicPerformance->SetParam ")
				TEXT("failed (%s == %08Xh)"),
				tdmXlatHRESULT(hr), hr);
		dwRes = FNS_FAIL;
		goto TEST_END;
	}   

    // **********************************************************************
    // 11) Wait for IsPlaying to return favorably (or time out)
    // **********************************************************************
	// Set counter back to zero
	counter = 0;
	while ((hr = pwPerformance->IsPlaying(pwSegment, NULL)) != S_OK)	
	{
		Sleep(SleepTime);
		if(counter++ >TimeOutValue)
		{
			fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: PlaySegment() timed out")
				TEXT("IsPlaying did not return the Segment (%s == %08Xh)"),
				tdmXlatHRESULT(hr), hr);
			dwRes = FNS_ABORTED;
			goto TEST_END;
		}
	}
Sleep(6000);
	pwPerformance->GetTime(NULL, &mt);
	fnsLog(FYILOGLEVEL, TEXT("Time After = %ld"), mt);	

TEST_END:
    // **********************************************************************
    // 12) Release objects, CoUninitialize, and return results
    // **********************************************************************
	if(pwPerformance)
	{
		pwPerformance->Stop(NULL, NULL, 0, 0);
		pwPerformance->CloseDown();
		pwPerformance->Release();
		pwPerformance = NULL;
	}
	if(pwTrack)
	{
		pwTrack->Release();
		pwTrack = NULL;
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
// @topic IDirectMusicPerformance::AllocPMsg()_Valid_Test |
//
// Description: <nl>
// Valid test function for IDirectMusicPerformance::AllocPMsg().
//		AllocPMSG for Note using boundary for cb.
//
// Test Function: <nl>
// tdmperfvAllocPMsgValidTest2 <nl>
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
// 2) Create CtIDirectMusicPerformance object <nl>
// 3) Init the Performance <nl>
// 4) Call AllocPMsg for Note, Curve, Tempo, and MIDI messages and check result <nl>
// 5) Set up pMSG's for Note, Curve, Tempo, and MIDI<nl>
// 6) Call SendPMsg for Note, Curve, Tempo, and MIDI, and check result <nl>
// 7) Cleanup objects, uninitialize COM and return results <nl>
//
// History: <nl>
//  05/04/1998 - a-llucar - created <nl>
//
//	The same test is used for tdmperfvAllocPMsgValidTest1() and tdmperfvSendPMsgValidTest1() <nl>
//
// @index topic | IDIRECTMUSICPERFORMANCE_CASES
//--------------------------------------------------------------------------;
/*
DWORD tdmperfvAllocPMsgValidTest2(BOOL fCoCreate, BOOL fMaxDebug, 
						   LPARAM dwParam1, LPARAM dwParam2)
{
	DWORD							dwRes			= FNS_PASS;
	HRESULT							hr				= S_OK;
	DMUS_NOTE_PMSG					*pNote			= NULL;
	DMUS_CURVE_PMSG					*pCurve			= NULL;
	DMUS_TEMPO_PMSG					*pTempo			= NULL;
	DMUS_MIDI_PMSG					*pMidi			= NULL;
	CtIDirectMusic					*pwMusic		= NULL;
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
		fnsLog(ABORTLOGLEVEL, TEXT("*** ABORT: PerformanceNULL,NULL,ghWnd ")
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
    // 4.1) Call AllocPMsg on pNote and check result
    // **********************************************************************
	hr = pwPerformance->AllocPMsg( ULONG_MAX, (DMUS_PMSG**)&pNote );
	if(FAILED(hr))
	{
        fnsLog(BUGLOGLEVEL, TEXT("!!!! BUG: IDirectMusicPerformance->AllocPMsg(pNote) "),
                TEXT("failed (%s == %08Xh)"),
                tdmXlatHRESULT(hr), hr);
		dwRes = FNS_FAIL;
		goto TEST_END;
	}

	// Check to make sure that pNote was properly populated
	if(!pNote)
	{
        fnsLog(BUGLOGLEVEL, TEXT("!!!! BUG: AllocPMsg did not properly set pNote, \n"),
                TEXT("pNote == NULL"));
		dwRes = FNS_FAIL;
		goto TEST_END;
	}

	// **********************************************************************
    // 4.2) Call AllocPMsg on pCurve and check result
    // **********************************************************************
	hr = pwPerformance->AllocPMsg( ULONG_MAX, (DMUS_PMSG**)&pCurve );
	if(FAILED(hr))
	{
        fnsLog(BUGLOGLEVEL, TEXT("!!!! BUG: IDirectMusicPerformance->AllocPMsg(pCurve) "),
                TEXT("failed (%s == %08Xh)"),
                tdmXlatHRESULT(hr), hr);
		dwRes = FNS_FAIL;
		goto TEST_END;
	}

	// Check to make sure that pCurve was properly populated
	if(!pCurve)
	{
        fnsLog(BUGLOGLEVEL, TEXT("!!!! BUG: AllocPMsg did not properly set pCurve, \n"),
                TEXT("pCurve == NULL"));
		dwRes = FNS_FAIL;
		goto TEST_END;
	}

	// **********************************************************************
    // 4.3) Call AllocPMsg on pTempo and check result
    // **********************************************************************
	hr = pwPerformance->AllocPMsg( ULONG_MAX, (DMUS_PMSG**)&pTempo );
	if(FAILED(hr))
	{
        fnsLog(BUGLOGLEVEL, TEXT("!!!! BUG: IDirectMusicPerformance->AllocPMsg(pTempo) "),
                TEXT("failed (%s == %08Xh)"),
                tdmXlatHRESULT(hr), hr);
		dwRes = FNS_FAIL;
		goto TEST_END;
	}

	// Check to make sure that pTempo was properly populated
	if(!pTempo)
	{
        fnsLog(BUGLOGLEVEL, TEXT("!!!! BUG: AllocPMsg did not properly set pTempo, \n"),
                TEXT("pTempo == NULL"));
		dwRes = FNS_FAIL;
		goto TEST_END;
	}

	// **********************************************************************
    // 4.4) Call AllocPMsg on pMidi and check result
    // **********************************************************************
	hr = pwPerformance->AllocPMsg( ULONG_MAX, (DMUS_PMSG**)&pMidi);
	if(FAILED(hr))
	{
        fnsLog(BUGLOGLEVEL, TEXT("!!!! BUG: IDirectMusicPerformance->AllocPMsg(pMidi) "),
                TEXT("failed (%s == %08Xh)"),
                tdmXlatHRESULT(hr), hr);
		dwRes = FNS_FAIL;
		goto TEST_END;
	}

	// Check to make sure that pMidi was properly populated
	if(!pMidi)
	{
        fnsLog(BUGLOGLEVEL, TEXT("!!!! BUG: AllocPMsg did not properly set pMidi, \n"),
                TEXT("pMidi == NULL"));
		dwRes = FNS_FAIL;
		goto TEST_END;
	}

    // **********************************************************************
    // 5.1) Set pNote with a bunch of particulars
    // **********************************************************************	
	ZeroMemory( pNote, sizeof(DMUS_NOTE_PMSG) );
	pNote->bFlags		= DMUS_NOTEF_NOTEON;
	pNote->bDurRange	= 100;
	pNote->wMusicValue	= 50;
	pNote->bVelocity	= 127;
	pNote->dwType		= DMUS_PMSGT_NOTE;
	pNote->dwFlags		= DMUS_PMSGF_REFTIME;

    // **********************************************************************
    // 5.2) Set pCurve with a bunch of particulars
    // **********************************************************************	
	ZeroMemory( pCurve, sizeof(DMUS_CURVE_PMSG) );
	pCurve->bFlags			= 1;
	pCurve->mtDuration		= 1000;
	pCurve->mtOriginalStart	= 0;
	pCurve->mtResetDuration	= 100;
	pCurve->nStartValue		= 0;
	pCurve->nEndValue		= 1000;
	pCurve->nResetValue		= 50;
	pCurve->wMeasure		= 8;
	pCurve->nOffset			= 0;
	pCurve->bBeat			= 2;
	pCurve->bGrid			= 0;
	pCurve->bType			= DMUS_CURVET_CCCURVE;
	pCurve->bCurveShape		= DMUS_CURVES_EXP;
	pCurve->dwFlags			= DMUS_PMSGF_REFTIME;
	
    // **********************************************************************
    // 5.3) Set pTempo with a bunch of particulars
    // **********************************************************************	
	ZeroMemory( pTempo, sizeof(DMUS_TEMPO_PMSG) );
	pTempo->dblTempo		= 100;
	pTempo->dwFlags			= DMUS_PMSGF_REFTIME;

    // **********************************************************************
    // 5.4) Set pMidi with a bunch of particulars
    // **********************************************************************	
	ZeroMemory( pMidi, sizeof(DMUS_MIDI_PMSG) );
	pMidi->dwFlags			= DMUS_PMSGF_REFTIME;

    // **********************************************************************
    // 6.1) Call SendPMsg on pNote and check results
    // **********************************************************************	
	hr = pwPerformance->SendPMsg( (DMUS_PMSG*)pNote );
	if(FAILED(hr))
	{
        fnsLog(BUGLOGLEVEL, TEXT("!!!! BUG: IDirectMusicPerformance->SendPMsg(pNote) "),
                TEXT("failed (%s == %08Xh)"),
                tdmXlatHRESULT(hr), hr);
		dwRes = FNS_FAIL;
		goto TEST_END;
	}

    // **********************************************************************
    // 6.2) Call SendPMsg on pCurve and check results
    // **********************************************************************	
	hr = pwPerformance->SendPMsg( (DMUS_PMSG*)pCurve );
	if(FAILED(hr))
	{
        fnsLog(BUGLOGLEVEL, TEXT("!!!! BUG: IDirectMusicPerformance->SendPMsg(pCurve) "),
                TEXT("failed (%s == %08Xh)"),
                tdmXlatHRESULT(hr), hr);
		dwRes = FNS_FAIL;
		goto TEST_END;
	}
	
    // **********************************************************************
    // 6.3) Call SendPMsg on pTempo and check results
    // **********************************************************************	
	hr = pwPerformance->SendPMsg( (DMUS_PMSG*)pTempo );
	if(FAILED(hr))
	{
        fnsLog(BUGLOGLEVEL, TEXT("!!!! BUG: IDirectMusicPerformance->SendPMsg(pTempo) "),
                TEXT("failed (%s == %08Xh)"),
                tdmXlatHRESULT(hr), hr);
		dwRes = FNS_FAIL;
		goto TEST_END;
	}

    // **********************************************************************
    // 6.4) Call SendPMsg on pMidi and check results
    // **********************************************************************	
	hr = pwPerformance->SendPMsg( (DMUS_PMSG*)pMidi );
	if(FAILED(hr))
	{
        fnsLog(BUGLOGLEVEL, TEXT("!!!! BUG: IDirectMusicPerformance->SendPMsg(pMidi) "),
                TEXT("failed (%s == %08Xh)"),
                tdmXlatHRESULT(hr), hr);
		dwRes = FNS_FAIL;
		goto TEST_END;
	}

TEST_END:
    // **********************************************************************
    // 7) Cleanup objects and uninitialize COM and Return results
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
*/


//--------------------------------------------------------------------------;
// @doc IDIRECTMUSICPERFORMANCE A-LLUCAR
//
// @topic IDirectMusicPerformance::RemoveNotificationType()_Valid_Test2 |
//
// Description: <nl>
// Valid test function for IDirectMusicPerformance::RemoveNotificationType(). <nl>
//		Calls RemoveNotificationType with All types of guids <nl>
//
// Test Function: <nl>
// tdmperfvRemoveNotificationTypeValidTest2 <nl>
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
// 2) Create CtIDirectMusicPerformance object <nl>
// 3) Create an Event for hNotification <nl>
// 4) Create an Event for hNotification and check results <nl>
// 5) AddNotificationType(All Types) and check results <nl>
// 6) Call PlayMidiFile() convienence function <nl>
// 9) Call RemoveNotificationType and check results <nl>
// 10) Release objects, CoUninitialize, and return results <nl>
//
// History: <nl>
//  03/23/1998 - a-llucar - created <nl>
//
// @index topic | IDIRECTMUSICPERFORMANCE_CASES
//--------------------------------------------------------------------------;
DWORD tdmperfvRemoveNotificationTypeValidTest2(BOOL fCoCreate, BOOL fMaxDebug, 
											LPARAM dwParam1, LPARAM dwParam2)
{
	DWORD						dwRes				= FNS_PASS;
	HRESULT						hr					= S_OK;
	DMUS_NOTIFICATION_PMSG		*pEvent				= NULL;
	CtIDirectMusicPerformance	*pwPerformance		= NULL;
	CtIDirectMusicSegment		*pwSegment			= NULL;
	HANDLE						hNotification;
	GUID						*guidNotification	= (GUID*)dwParam1;
	WCHAR						*pMidiFile			= (WCHAR*)dwParam2;

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
	// *********************************************************************
	pwPerformance->SetNotificationHandle( hNotification, 0 );
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
	pwPerformance->AddNotificationType(*guidNotification);

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
	hr = dmthPlayMidiFile(pwPerformance,CTIID_IDirectMusicSegment,(CtIDirectMusicSegment**)&pwSegment,(CtIDirectMusicSegmentState**)NULL, TRUE, pMidiFile);
	if(hr != S_OK)
	{
        fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: PlayMidiFile convienence function "),
                TEXT("failed (%s == %08Xh)"),
                tdmXlatHRESULT(hr), hr);
		dwRes = FNS_ABORTED;
		goto TEST_END;
	}

    // **********************************************************************
    // 7) Call RemoveNotificationType and check results
    // **********************************************************************
	pwPerformance->RemoveNotificationType(*guidNotification);
	if(FAILED(hr))
	{
        fnsLog(BUGLOGLEVEL, TEXT("!!!! BUG: pwPerformance->RemoveNotificationType ")
                TEXT("failed (%s == %08Xh)"),
                tdmXlatHRESULT(hr), hr);
		dwRes = FNS_FAIL;
		goto TEST_END;
	}
	Sleep(1000);
		
TEST_END:
    // **********************************************************************
    // 8) Release objects, CoUninitialize, and return results
    // **********************************************************************
	if(pwPerformance)
	{
		pwPerformance->Stop(NULL, NULL, 0, 0);
		pwPerformance->CloseDown();
		pwPerformance->Release();
		pwPerformance = NULL;
	}
	if (hNotification)
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
// @topic IDirectMusicPerformance::ReferenceToMusicTime()_Valid_Test |
//
// Description: <nl>
// Valid test function for IDirectMusicPerformance::ReferenceToMusicTime(). <nl>
//		Call ReferenceToMusicTime at time rt == 0. <nl>
//		Expects to return mt == 0! <nl>
//
// Test Function: <nl>
// tdmperfvReferenceToMusicTimeTest <nl>
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
// 2) Create CtIDirectMusicPerformance object <nl>
// 3) Init the performance <nl>
// 4) Call ReferenceToMusicTime and check results <nl>
// 5) Cleanup objects, uninitialize COM and return results <nl>
//
// History: <nl>
//  07/27/1998 - a-llucar - created <nl>
//
// @index topic | IDIRECTMUSICPERFORMANCE_CASES
//--------------------------------------------------------------------------;
DWORD tdmperfvReferenceToMusicTimeValidTest2(BOOL fCoCreate, BOOL fMaxDebug, 
						   LPARAM dwParam1, LPARAM dwParam2)
{
	DWORD						dwRes			= FNS_PASS;
	HRESULT						hr				= S_OK;
	CtIDirectMusicPerformance	*pwPerformance	= NULL;
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
    // 3) Init the performance
    // **********************************************************************
	hr = pwPerformance->Init(NULL,NULL,ghWnd);
	if(FAILED(hr))
	{
        fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: Performance->Init(NULL) "),
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
    // 4) Call ReferenceToMusicTime and check results
    // **********************************************************************
	hr = pwPerformance->ReferenceToMusicTime( 0, &mt );

	if(FAILED(hr))
	{
        fnsLog(BUGLOGLEVEL, TEXT("!!!! BUG: IDirectMusicPerformance->ReferenceToMusicTime ")
                TEXT("failed (%s == %08Xh)"),
                tdmXlatHRESULT(hr), hr);
		dwRes = FNS_FAIL;
		goto TEST_END;
	}

	if (mt != 0)
	{
        fnsLog(BUGLOGLEVEL, TEXT("!!!! BUG: IDirectMusicPerformance->ReferenceToMusicTime "));
        fnsLog(BUGLOGLEVEL, TEXT("Music Time did not return expected results"));
        fnsLog(BUGLOGLEVEL, TEXT("Expected: mt == 0 "));
        fnsLog(BUGLOGLEVEL, TEXT("Returned: mt == %u "), mt);
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
// @topic IDirectMusicPerformance::MusicToReferenceTime()_Valid_Test |
//
// Description: <nl>
// Valid test function for IDirectMusicPerformance::MusicToReferenceTime(). <nl>
//		Call MusicToReferenceTime at time rt == 0. <nl>
//		Expects to return mt == 0! <nl>
//
// Test Function: <nl>
// tdmperfvMusicToReferenceTimeTest <nl>
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
// 2) Create CtIDirectMusicPerformance object <nl>
// 3) Init the performance <nl>
// 4) Call MusicToReferenceTime and check results <nl>
// 5) Cleanup objects, uninitialize COM and return results <nl>
//
// History: <nl>
//  07/27/1998 - a-llucar - created <nl>
//
// @index topic | IDIRECTMUSICPERFORMANCE_CASES
//--------------------------------------------------------------------------;
DWORD tdmperfvMusicToReferenceTimeValidTest2(BOOL fCoCreate, BOOL fMaxDebug, 
						   LPARAM dwParam1, LPARAM dwParam2)
{
	DWORD						dwRes			= FNS_PASS;
	HRESULT						hr				= S_OK;
	CtIDirectMusicPerformance	*pwPerformance	= NULL;
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
    // 3) Init the performance
    // **********************************************************************
	hr = pwPerformance->Init(NULL,NULL,ghWnd);
	if(FAILED(hr))
	{
        fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: Performance->Init(NULL) "),
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
	hr = pwPerformance->MusicToReferenceTime( 0, &rt );

	if(FAILED(hr))
	{
        fnsLog(BUGLOGLEVEL, TEXT("!!!! BUG: IDirectMusicPerformance->MusicToReferenceTime ")
                TEXT("failed (%s == %08Xh)"),
                tdmXlatHRESULT(hr), hr);
		dwRes = FNS_FAIL;
		goto TEST_END;
	}

	if (rt == 0)
	{
        fnsLog(BUGLOGLEVEL, TEXT("!!!! BUG: IDirectMusicPerformance->MusicToReferenceTime "));
        fnsLog(BUGLOGLEVEL, TEXT("Reference Time did not return expected results"));
        fnsLog(BUGLOGLEVEL, TEXT("Returned: rt == %u "), rt);
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
// @topic IDirectMusicPerformance::SetGraph()_Valid_Test |
//
// Description: <nl>
// Valid test function for IDirectMusicPerformance::SetGraph().
//		Creates 2 performance objects, GetGraph from the first and <nl>
//		SetGraph on the second.  Clears the Graph with SetGraph(NULL), <nl>
//		verifies with GetGraph(), and you should hear no Echo as it plays on. <nl>
//
// Test Function: <nl>
// tdmperfvSetGraphValidTest2 <nl>
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
// 2) Create 2 CtIDirectMusicPerformance objects <nl>
// 3) Create Graph Object <nl>
// 4) dmthCreateEchoTool <nl>
// 5) Check to make sure Tool got set to something <nl>
// 6) Load the two segments <nl>
// 7) Insert Tool <nl>
// 8) Set the Performance Graph <nl>
// 9) Create DirectMusic Object <nl>
// 10) Init the two performances <nl>
// 11) Create a SynthPort <nl>
// 12) Add a port to our two performances <nl>
// 13) Create the 5 channels for the echos <nl>
// 14) Performance->AllocPMsg and check results <nl>
// 15) Call StampPMsg() and check results <nl>
// 16) Call PlaySegment and check results <nl>
// 17) Call GetGraph, and pass it into pwGraph2 <nl>
// 18) Call SetGraph on the Second performance <nl>
// 19) Wait for IsPlaying to return or time out <nl>
// 20) Call PlaySegment on the second performance <nl>
// 21) Call SetGraph(NULL) to clear the graph from the performance <nl>
// 22) Call GetGraph(NULL) to clear the graph from the performance <nl>
// 23) Release objects, CoUninitialize, and return results <nl>
//
// History: <nl>
//  07/22/1998 - a-llucar - created <nl>
//
// @index topic | IDIRECTMUSICPERFORMANCE_CASES
//--------------------------------------------------------------------------;
DWORD tdmperfvSetGraphValidTest2(BOOL fCoCreate, BOOL fMaxDebug, 
						   LPARAM dwParam1, LPARAM dwParam2)
{
    DWORD                       dwRes			= FNS_PASS;
    HRESULT                     hr				= S_OK;
    CtIDirectMusicGraph         *pwGraph		= NULL;
    CtIDirectMusicPerformance   *pwPerformance	= NULL;
    CtIDirectMusicSegment       *pwSegment		= NULL;
    CtIDirectMusicTool          *pwTool			= NULL;
    CtIDirectMusicPort          *pwPort			= NULL;
    CtIDirectMusic              *pwDM			= NULL;
    DMUS_PMSG                   *pdmusPMSG		= NULL;
    DMUS_PORTPARAMS             dmusPortParams;
    DWORD                       dwCounter       = 0;
    DWORD                       dwTimeOutValue  = 20;
    GUID                        guidSink;

	CtIDirectMusicPerformance	*pwPerformance2	= NULL;
	CtIDirectMusicSegment		*pwSegment2		= NULL;
	CtIDirectMusicGraph			*pwGraph2		= NULL;
	CtIDirectMusicGraph			*pwGraph3		= NULL;
	CtIDirectMusic				*pwDM2			= NULL;

    // **********************************************************************
    // 1) Initialize COM.
    // **********************************************************************
    hr = dmthCoInitialize(NULL);
    if(FAILED(hr))
    {
        fnsLog(ABORTLOGLEVEL, "**** ABORT: COM initialization failed! (%s == %08Xh)",
                tdmXlatHRESULT(hr), hr);
        return FNS_ABORTED;
    }

    // **********************************************************************
    // 2) Create 2 CtIDirectMusicPerformance objects 
    // **********************************************************************
    hr = dmthCreatePerformance(IID_IDirectMusicPerformance, &pwPerformance);
    if(FAILED(hr))
    {
        fnsLog(ABORTLOGLEVEL, "**** ABORT: DirectMusicPerformance object creation (pwPerformance1) "
                "failed (%s == %08Xh)", tdmXlatHRESULT(hr), hr);
        dwRes = FNS_ABORTED;
        goto TEST_END;
    }

    hr = dmthCreatePerformance(IID_IDirectMusicPerformance, &pwPerformance2);
    if(FAILED(hr))
    {
        fnsLog(ABORTLOGLEVEL, "**** ABORT: DirectMusicPerformance object creation (pwPerformance2) "
                "failed (%s == %08Xh)", tdmXlatHRESULT(hr), hr);
        dwRes = FNS_ABORTED;
        goto TEST_END;
    }

    // **********************************************************************
    // 3) Create Graph Object
    // **********************************************************************
    hr = dmthCreateGraph(IID_IDirectMusicGraph, &pwGraph);
    if(FAILED(hr))
    {
        fnsLog(ABORTLOGLEVEL, "**** ABORT: dmthCreateGraph "
                "failed (%s == %08Xh)", tdmXlatHRESULT(hr), hr);
        dwRes = FNS_ABORTED;
        goto TEST_END;
    }

    // **********************************************************************
    // 4) dmthCreateEchoTool
    // **********************************************************************
    hr = dmthCreateEchoTool(IID_IDirectMusicTool, &pwTool, 3, DMUS_PPQ);
    if (FAILED( hr ))
    {
        fnsLog(ABORTLOGLEVEL, "**** ABORT: dmthCreateEchoTool "
                "failed (%s == %08Xh)", tdmXlatHRESULT(hr), hr);
        dwRes = FNS_ABORTED;
        goto TEST_END;
    }

    // **********************************************************************
    // 5) Check to make sure Tool got set to something
    // **********************************************************************
    if ( NULL == pwTool )
    {   
        fnsLog(ABORTLOGLEVEL, "**** ABORT: Something wrong with the Tool.  Should not be NULL.");
        dwRes = FNS_ABORTED;
        goto TEST_END;
    }


    // **********************************************************************
    // 6) Load the two segments
    // **********************************************************************
    hr = dmthLoadSegment(L"DSegTest3.sgt", &pwSegment);
    if (FAILED( hr ))
    {
        fnsLog(ABORTLOGLEVEL, "**** ABORT: dmthLoadSegment "
                "failed (%s == %08Xh)", tdmXlatHRESULT(hr), hr);
        dwRes = FNS_ABORTED;
        goto TEST_END;
    }

    hr = dmthLoadSegment(L"onestop2.mid", &pwSegment2);
    if (FAILED( hr ))
    {
        fnsLog(ABORTLOGLEVEL, "**** ABORT: dmthLoadSegment "
                "failed (%s == %08Xh)", tdmXlatHRESULT(hr), hr);
        dwRes = FNS_ABORTED;
        goto TEST_END;
    }

    // **********************************************************************
    // 7) Insert Tool
    // **********************************************************************
    hr = pwGraph->InsertTool( pwTool, NULL, 0, 0 );
    if(FAILED(hr))
    {
        fnsLog(ABORTLOGLEVEL, "**** ABORT: Graph->InsertTool "
                "failed (%s == %08Xh)", tdmXlatHRESULT(hr), hr);
        dwRes = FNS_ABORTED;
        goto TEST_END;
    }

    // **********************************************************************
    // 8) Set the Performance Graph
    // **********************************************************************
    hr = pwPerformance->SetGraph( pwGraph );
    if (FAILED( hr ))
    {
        fnsLog(BUGLOGLEVEL, "!!!! BUG: Performance->SetGraph(pwGraph) "
                "failed (%s == %08Xh)", tdmXlatHRESULT(hr), hr);
        dwRes = FNS_FAIL;
        goto TEST_END;
    }

    // **********************************************************************
    // 9) Create DirectMusic Object
    // **********************************************************************
	hr = dmthCreateDMBaseObj(IID_IDirectMusic, &pwDM);
	if (FAILED(hr))
    {
        fnsLog(ABORTLOGLEVEL, "**** ABORT: IDirectMusic object creation failed! (%s == %08Xh)", 
			tdmXlatHRESULT(hr), hr);
        dwRes = FNS_ABORTED;
        goto TEST_END;
    }

	pwDM->SetDirectSound(NULL, ghWnd);
	if (FAILED(hr))
    {
        fnsLog(ABORTLOGLEVEL, "**** ABORT: Music->SetDirectSound failed! (%s == %08Xh)", 
			tdmXlatHRESULT(hr), hr);
        dwRes = FNS_ABORTED;
        goto TEST_END;
    }

    // **********************************************************************
    // 10) Init the two performances
    // **********************************************************************
    hr = pwPerformance->Init(&pwDM,NULL,ghWnd);
    if(FAILED(hr))
    {
        fnsLog(ABORTLOGLEVEL, "**** ABORT: Performance->Init "
               "failed (%s == %08Xh)", tdmXlatHRESULT(hr), hr);
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

	hr = pwPerformance2->Init(&pwDM,NULL,ghWnd);
    if(FAILED(hr))
    {
        fnsLog(ABORTLOGLEVEL, "**** ABORT: Performance2->Init "
               "failed (%s == %08Xh)", tdmXlatHRESULT(hr), hr);
        dwRes = FNS_ABORTED;
        goto TEST_END;
    }
    // set autodownload on
    fAuto = TRUE;
    hr = pwPerformance2->SetGlobalParam( GUID_PerfAutoDownload, &fAuto, sizeof(BOOL) );
	if(FAILED(hr))
	{
		fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: Performance2->SetGlobalParam(AutoDownload == TRUE) ")
				TEXT("failed (%s == %08Xh)"),
				tdmXlatHRESULT(hr), hr);
		dwRes = FNS_ABORTED;
		goto TEST_END;
	}

    // **********************************************************************
    // 11) Create a SynthPort
    // **********************************************************************
    ZeroMemory( &dmusPortParams, sizeof(DMUS_PORTPARAMS) );
    dmusPortParams.dwSize          = sizeof(DMUS_PORTPARAMS);  
    dmusPortParams.dwChannelGroups = 5; // create 5 channel groups on the port
    dmusPortParams.dwValidParams   = DMUS_PORTPARAMS_CHANNELGROUPS;

    ZeroMemory( &guidSink, sizeof(GUID) );

    hr = pwDM->CreatePort(
		CLSID_DirectMusicSynth, 
        &dmusPortParams, 
        &pwPort, 
        NULL );
    if ( FAILED(hr) )
    {
        fnsLog(ABORTLOGLEVEL, "**** ABORT: pwDM->CreatePort "
                "failed (%s == %08Xh)", tdmXlatHRESULT(hr), hr);
        dwRes = FNS_ABORTED;
        goto TEST_END;
    }

    // **********************************************************************
    // 12) Add a port to our two performances
    // **********************************************************************
    hr = pwPerformance->AddPort(pwPort);
    if(FAILED(hr))
    {
        fnsLog(ABORTLOGLEVEL, "**** ABORT: pwPerformance->AddPort(NULL) "
                "failed (%s == %08Xh)", tdmXlatHRESULT(hr), hr);
        dwRes = FNS_ABORTED;
        goto TEST_END;
    }

    hr = pwPerformance2->AddPort(NULL);
    if(FAILED(hr))
    {
        fnsLog(ABORTLOGLEVEL, "**** ABORT: pwPerformance2->AddPort(NULL) "
                "failed (%s == %08Xh)", tdmXlatHRESULT(hr), hr);
        dwRes = FNS_ABORTED;
        goto TEST_END;
    }

    // **********************************************************************
    // 13) Create the 5 channels for the echos
    // **********************************************************************
    // Assign blocks of 16 PChannels to this port.
    // Block 0, port pPort, and group 1 means to assign
    // PChannels 0-15 to group 1 on port pPort.
    // PChannels 0-15 correspond to the standard 16
    // MIDI channels.
    pwPerformance->AssignPChannelBlock( 0, pwPort, 1 );
    pwPerformance->AssignPChannelBlock( 1, pwPort, 2 ); //16 - 31 Group 2
    pwPerformance->AssignPChannelBlock( 2, pwPort, 3 ); //32 - 47 Group 3
    pwPerformance->AssignPChannelBlock( 3, pwPort, 4 ); //48 - 63 Group 4
    pwPerformance->AssignPChannelBlock( 4, pwPort, 5 ); //64 - 80 Group 5

    // **********************************************************************
    // 14) Performance->AllocPMsg and check results
    // **********************************************************************
    hr = pwPerformance->AllocPMsg( sizeof(DMUS_PMSG), &pdmusPMSG );
    if(FAILED(hr))
    {
        fnsLog(ABORTLOGLEVEL, "**** ABORT: Performance->AllocPMsg "
                "failed (%s == %08Xh)", tdmXlatHRESULT(hr), hr);
        dwRes = FNS_ABORTED;
        goto TEST_END;
    }

    // **********************************************************************
    // 15) Call StampPMsg() and check results
    // **********************************************************************   
    hr = pwGraph->StampPMsg(pdmusPMSG);
    if(FAILED(hr))
    {
        fnsLog(ABORTLOGLEVEL, "**** ABORT: DirectMusicTool->StampPMsg "
                "failed (%s == %08Xh)", tdmXlatHRESULT(hr), hr);
        dwRes = FNS_ABORTED;
        goto TEST_END;
    }

    // **********************************************************************
    // 16) Call PlaySegment and check results
    // **********************************************************************
    hr = pwPerformance->PlaySegment(pwSegment, 0, 0, NULL);
    if(FAILED(hr))
    {
        fnsLog(ABORTLOGLEVEL, "**** ABORT: pwPerformance->PlaySegment "
                "failed (%s == %08Xh)", tdmXlatHRESULT(hr), hr);
        dwRes = FNS_ABORTED;
        goto TEST_END;
    }

    // **********************************************************************
    // 17) Call GetGraph, and pass it into pwGraph2
    // **********************************************************************
	hr = pwPerformance->GetGraph(&pwGraph2);
	if(FAILED(hr))
    {
        fnsLog(BUGLOGLEVEL, "!!!! BUG: pwPerformance->GetGraph(&pwGraph2) "
                "failed (%s == %08Xh)", tdmXlatHRESULT(hr), hr);
        dwRes = FNS_FAIL;
        goto TEST_END;
    }

    // **********************************************************************
    // 18) Call SetGraph on the Second performance
    // **********************************************************************
	hr = pwPerformance2->SetGraph(pwGraph2);
	if(FAILED(hr))
    {
        fnsLog(BUGLOGLEVEL, "!!!! BUG: pwPerformance2->SetGraph(pwGraph2) "
                "failed (%s == %08Xh)", tdmXlatHRESULT(hr), hr);
        dwRes = FNS_FAIL;
        goto TEST_END;
    }

    // **********************************************************************
    // 19) Wait for IsPlaying to return or time out
    // **********************************************************************
    dwCounter = 0;
    while ((hr = pwPerformance->IsPlaying(pwSegment, NULL)) != S_OK)  
    {
        Sleep(1000);
        if(dwCounter++ > dwTimeOutValue)
        {
            fnsLog(ABORTLOGLEVEL, "**** ABORT: PlaySegment() timed out"
                "IsPlaying did not return the Segment (%s == %08Xh)", tdmXlatHRESULT(hr), hr);
            dwRes = FNS_ABORTED;
            goto TEST_END;
        }
    }

    Sleep(1000);
	
    // **********************************************************************
    // 20) Call PlaySegment on the second performance
    // **********************************************************************
	hr = pwPerformance2->PlaySegment(pwSegment2, 0, 0, NULL);
    if(FAILED(hr))
    {
        fnsLog(ABORTLOGLEVEL, "**** ABORT: pwPerformance2->PlaySegment "
                "failed (%s == %08Xh)", tdmXlatHRESULT(hr), hr);
        dwRes = FNS_ABORTED;
        goto TEST_END;
    }
	Sleep(1000);

    // **********************************************************************
    // 21) Call SetGraph(NULL) to clear the graph from the performance
    // **********************************************************************
	hr = pwPerformance2->SetGraph(NULL);
	if(FAILED(hr))
    {
        fnsLog(BUGLOGLEVEL, "!!!! BUG: pwPerformance2->SetGraph(pwGraph2) "
                "failed (%s == %08Xh)", tdmXlatHRESULT(hr), hr);
        dwRes = FNS_FAIL;
        goto TEST_END;
    }

    // **********************************************************************
    // 22) Call GetGraph(NULL) to clear the graph from the performance
    // **********************************************************************
	hr = pwPerformance2->GetGraph(&pwGraph3);
	fnsLog(FYILOGLEVEL, "NOTE: We expect this to Fail");
	if(SUCCEEDED(hr))
    {
        fnsLog(BUGLOGLEVEL, "!!!! BUG: pwPerformance2->SetGraph(pwGraph3) "
                "failed (%s == %08Xh)", tdmXlatHRESULT(hr), hr);
        dwRes = FNS_FAIL;
        goto TEST_END;
    }
	if(pwGraph3)
	{
		fnsLog(BUGLOGLEVEL, "!!!! BUG: SetGraph(NULL) did not clear the graph from the performance");
        dwRes = FNS_FAIL;
        goto TEST_END;
    }
	Sleep(10000);
	
TEST_END:
    // **********************************************************************
    // 23) Release objects, CoUninitialize, and return results
    // **********************************************************************
    if(pwPerformance)
    {
        if (pdmusPMSG)
            pwPerformance->FreePMsg((DMUS_PMSG *) pdmusPMSG);

        pwPerformance->Stop(NULL, NULL, 0, 0);
        pwPerformance->CloseDown();

        pwPerformance->Release();
        pwPerformance = NULL;
    }
    if(pwTool)
    {
        pwTool->Release();
        pwTool = NULL;
    }
    if(pwGraph)
    {
        pwGraph->Release();
        pwGraph = NULL;
    }
    if(pwSegment)
    {
        pwSegment->Release();
        pwSegment = NULL;
    }
    if(pwPort)
    {
        pwPort->Release();
        pwPort = NULL;
    }
    if(pwDM)
    {
        pwDM->Release();
        pwDM = NULL;
    }
	if(pwPerformance2)
	{
        pwPerformance2->Stop(NULL, NULL, 0, 0);
		pwPerformance2->CloseDown();
		pwPerformance2->Release();
		pwPerformance2 = NULL;
	}
	if(pwSegment2)
	{
		pwSegment2->Release();
		pwSegment2 = NULL;
	}
	if(pwGraph2)
	{
		pwGraph2->Release();
		pwGraph2 = NULL;
	}
	if(pwGraph3)
	{
		pwGraph3->Release();
		pwGraph3 = NULL;
	}
    dmthCoUninitialize();   
    return dwRes;
}


//--------------------------------------------------------------------------;
// @doc IDIRECTMUSICPERFORMANCE A-LLUCAR
//
// @topic IDirectMusicPerformance::GetResolvedTime()_Valid test |
//
// Description: <nl>
// Valid test function for IDirectMusicPerformance::GetResolvedTime().
//		Calls GetResolvedTime while segment plays
//
// Test Function: <nl>
// tdmperfvGetResolvedTimeValidTest1 <nl>
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
// 2) Create CtIDirectMusicPerformance object <nl>
// 3) Load a segment <nl>
// 4) PrePlay <nl>
// 5) PlaySegment <nl>
// 5) Call GetResolvedTime <nl>
// 6) Cleanup objects and uninitialize COM and Return results <nl>
//
// History: <nl>
//  07/31/1998 - a-llucar - created <nl>
//
// @index topic | IDIRECTMUSICPERFORMANCE_CASES
//--------------------------------------------------------------------------;
DWORD tdmperfvGetResolvedTimeValidTest1(BOOL fCoCreate, BOOL fMaxDebug, 
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
    // 5) Play the segment
    // **********************************************************************
	hr = pwPerformance->PlaySegment(pwSegment, 0, 0, NULL);
    if(FAILED(hr))
	{
        fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: PlaySegment ")
                TEXT("failed (%s == %08Xh)"), tdmXlatHRESULT(hr), hr);
		dwRes = FNS_ABORTED;
		goto TEST_END;
	}

    // **********************************************************************
    // 6) Call GetResolvedTime and check results
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
    // 7) Cleanup objects and uninitialize COM and Return results
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
// @topic IDirectMusicPerformance::GetResolvedTime()_Valid test |
//
// Description: <nl>
// Valid test function for IDirectMusicPerformance::GetResolvedTime().
//		Calls GetResolvedTime passing in different Flags for dwFlags. <nl>
//
// Test Function: <nl>
// tdmperfvGetResolvedTimeValidTest2 <nl>
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
// 2) Create CtIDirectMusicPerformance object <nl>
// 3) Load a segment <nl>
// 4) PrePlay <nl>
// 5) Call GetResolvedTime <nl>
// 6) Cleanup objects and uninitialize COM and Return results <nl>
//
// History: <nl>
//  07/31/1998 - a-llucar - created <nl>
//
// @index topic | IDIRECTMUSICPERFORMANCE_CASES
//--------------------------------------------------------------------------;
DWORD tdmperfvGetResolvedTimeValidTest2(BOOL fCoCreate, BOOL fMaxDebug, 
						   LPARAM dwParam1, LPARAM dwParam2)
{
	DWORD						dwRes			= FNS_PASS;
	HRESULT						hr				= S_OK;
	CtIDirectMusicPerformance	*pwPerformance	= NULL;
	CtIDirectMusicSegment		*pwSegment		= NULL;
	REFERENCE_TIME				rtResolved		= 0;
	int							i				= 0;

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
	for (i = 0; i < sizeof(SEGF_FLAGS)/sizeof(DWORD); i++)
	{
		rtResolved = 0;	//Re-initialize rtResolved
		hr = pwPerformance->GetResolvedTime(2000, &rtResolved, SEGF_FLAGS[i]);
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
// @topic IDirectMusicPerformance::MusicToReferenceTime()_Valid_Test |
//
// Description: <nl>
// Valid test function for IDirectMusicPerformance::MusicToReferenceTime(). <nl>
//		Calls GetTime(mt, rt).  Then, does a MusictoReferenceTime(mt, &rt2) and 
//		ReferenceToMusicTime(rt, &mt2).  Compares mt with mt2 and rt with rt2 to
//		verify that they are the same <nl>
//
// Test Function: <nl>
// tdmperfvMusicToReferenceTimeTest3 <nl>
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
// 2) Create CtIDirectMusicPerformance object <nl>
// 3) Init the performance <nl>
// 4) Call GetTime and check results <nl>
// 5) Call ReferenceToMusic and MusicToReference <nl>
//
// History: <nl>
//  07/27/1998 - a-llucar - created <nl>
//
//	NOTE:  Same test is used for MusicToReferenceTime3 and ReferenceToMusicTime3
//
// @index topic | IDIRECTMUSICPERFORMANCE_CASES
//--------------------------------------------------------------------------;
DWORD tdmperfvMusicToReferenceTimeValidTest3(BOOL fCoCreate, BOOL fMaxDebug, 
						   LPARAM dwParam1, LPARAM dwParam2)
{
	DWORD						dwRes			= FNS_PASS;
	HRESULT						hr				= S_OK;
	CtIDirectMusicPerformance	*pwPerformance	= NULL;
	REFERENCE_TIME				rt				= 0;
	REFERENCE_TIME				rt2				= 0;
	MUSIC_TIME					mt				= 0;
	MUSIC_TIME					mt2				= 0;

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
    // 3) Init the performance
    // **********************************************************************
	hr = pwPerformance->Init(NULL,NULL,ghWnd);
	if(FAILED(hr))
	{
        fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: Performance->Init(NULL) "),
                TEXT("failed (%s == %08Xh)"),
                tdmXlatHRESULT(hr), hr);
		dwRes = FNS_ABORTED;
		goto TEST_END;
	}
    // **********************************************************************
    // 4) Call GetTime to get mt and rt
    // **********************************************************************
	Sleep(100); // insure some time has passed so NULL should never be returned
	hr = pwPerformance->GetTime( &rt, &mt );
	if(FAILED(hr))
	{
        fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: IDirectMusicPerformance->GetTime "),
                TEXT("failed (%s == %08Xh)"),
                tdmXlatHRESULT(hr), hr);
		dwRes = FNS_ABORTED;
		goto TEST_END;
	}

	if(!mt || !rt)
	{
		fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT:  mt or rt was not set correctly"),
				TEXT("rt == %u, mt == %u"), rt, mt);
		dwRes = FNS_ABORTED;
		goto TEST_END;
	}

    // **********************************************************************
    // 5) Call ReferenceToMusic (and vice-versa) to get mt2 and rt2; compare
    // **********************************************************************
	hr = pwPerformance->ReferenceToMusicTime(rt, &mt2);
	if(FAILED(hr))
	{
        fnsLog(BUGLOGLEVEL, TEXT("!!!! BUG: IDirectMusicPerformance->ReferenceToMusicTime ")
                TEXT("failed (%s == %08Xh)"),
                tdmXlatHRESULT(hr), hr);
		dwRes = FNS_FAIL;
		goto TEST_END;
	}

	hr = pwPerformance->MusicToReferenceTime(mt, &rt2);
	if(FAILED(hr))
	{
        fnsLog(BUGLOGLEVEL, TEXT("!!!! BUG: IDirectMusicPerformance->MusictoReferenceTime ")
                TEXT("failed (%s == %08Xh)"),
                tdmXlatHRESULT(hr), hr);
		dwRes = FNS_FAIL;
		goto TEST_END;
	}

	if(mt != mt2)
	{
        fnsLog(BUGLOGLEVEL, TEXT("!!!! BUG: mt != mt2 ")
                TEXT("mt == %u, mt2 == %u"), mt, mt2);
		dwRes = FNS_FAIL;
		goto TEST_END;
	}

	if(rt + 10000 < rt2 || rt - 10000 > rt2)
	{
        fnsLog(BUGLOGLEVEL, TEXT("!!!! BUG: rt != rt2 ")
                TEXT("rt == %u, rt2 == %u"), rt, rt2);
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

	dmthCoUninitialize();	

	return dwRes;
}
//===========================================================================



//--------------------------------------------------------------------------;
// @doc IDIRECTMUSICPERFORMANCE A-LLUCAR
//
// @topic IDirectMusicPerformance::LoadFiles()_Valid_Test2 |
//
// Description: <nl>
//		Verifies that loading UNFO subchunks should be UXXX rather than IXXX<nl>
//
// Test Function: <nl>
// tdmperfvLoadFilesValidTest1<nl>
//
// Parameters: <nl>
//  BOOL    fCoCreate   - unused <nl>
//  BOOL    fMaxDebug   - unused <nl>
//  DWORD   dwParam1    - unused <nl>
//	DWORD   dwParam2    - unused <nl>
//
// File : TDMPERFV.CPP
//
// History: <nl>
//  10/19/1998 - a-llucar - created <nl>
//
// @index topic | IDIRECTMUSICPERFORMANCE_CASES
//--------------------------------------------------------------------------;
/*
DWORD tdmperfvLoadFilesValidTest1(BOOL fCheck1, BOOL fCheck2, 
											LPARAM dwParam1, LPARAM dwParam2)
{
	DWORD						dwRes				= FNS_PASS;
	HRESULT						hr					= S_OK;
	CtIDirectMusicStyle			*pwStyle			= NULL;
	CtIDirectMusicBand			*pwBand				= NULL;

	BOOL						fLoadStyle			= fCheck1;
	BOOL						fLoadBand			= fCheck2;
	WCHAR						*pStyleFile			= (WCHAR*)dwParam1;
	WCHAR						*pBandFile			= (WCHAR*)dwParam2;

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
    // 2) Load Style File <nl>
    // **********************************************************************
	if(fCheck1)	// We want to look for a style file
	{
		hr = dmthLoadStyle(pStyleFile, &pwStyle);
		if(FAILED(hr))
		{
		    fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: DirectMusicStyle object creation "),
		            TEXT("failed (%s == %08Xh)"),
		            tdmXlatHRESULT(hr), hr);
			dwRes = FNS_ABORTED;
			goto TEST_END;
		}
	}

    // **********************************************************************
    // 3) Load Band File <nl>
    // **********************************************************************
	if(fCheck2)	// We want to look for a band file
	{
		hr = dmthLoadBand(pBandFile, &pwBand);
		if(FAILED(hr))
		{
		    fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: DirectMusicBand object creation "),
		            TEXT("failed (%s == %08Xh)"),
		            tdmXlatHRESULT(hr), hr);
			dwRes = FNS_ABORTED;
			goto TEST_END;
		}
	}

TEST_END:
    // **********************************************************************
    // 4) Release objects, CoUninitialize, and return results
    // **********************************************************************
	dmthReleaseStyle(pwStyle);	
	if(pwBand)
	{
		pwBand->Release();
		pwBand = NULL;
	}
	dmthCoUninitialize();	
	return dwRes;
}
*/
//===========================================================================



//--------------------------------------------------------------------------;
// @doc IDIRECTMUSICPERFORMANCE A-LLUCAR
//
// @topic IDirectMusicPerformance::MultipleTools()_Valid_Test |
//
// Description: <nl>
// Valid test function for IDirectMusicPerformance::SetGraph() <nl>
// This test inserts a tool at a given address, in an array of tools
// 
//
// Test Function: <nl>
// tdmPerfvMultipleToolsValidTest <nl>
//
// Parameters: <nl>
//  BOOL    fCoCreate   - unused <nl>
//  BOOL    fMaxDebug   - unused <nl>
//  DWORD   dwParam1    - Number of tools to create <nl>
//  DWORD   dwParam2    - Index at at which to stick Tool #(dwParam1 + 1) at<nl>
//
// File : TDMPERFV.CPP
//
// Steps: <nl>
// 1) Initialize COM. <nl>
// 2) Create Performance Object <nl>
// 3) Init the performance <nl>
// 4) AllocPMsg for the User PMsg <nl>
// 3) Create Graph Object<nl>
// 4) Create Array of Tools and insert them<nl>
//  4.1) Check Validity of the tool<nl>
//  4.2) Set the ID of the tool to the Index number<nl>
//  4.3) Set each tools m_fLogFlags to log USER PMsgs
//  4.4) Insert Tool at Index dwIndex. If last tool, insert at desired index<nl>
// 5) Get the tool at that index, to verify<nl>
// 6) Verify that the tool was the one we expected<nl>
// 7) Release objects, CoUninitialize, and return results<nl>
//
// History:
//	10/13/1998 - a-llucar - Created (based off of Graph test)
// @index topic | IDIRECTMUSICPERFORMANCE_CASES
//--------------------------------------------------------------------------;
DWORD tdmPerfvMultipleToolsValidTest(BOOL fIndexFromEnd, BOOL fMaxDebug, LPARAM dwParam1, LPARAM dwParam2)
{
    DWORD						dwRes			= FNS_PASS;
    HRESULT						hr				= S_OK;
	CtIDirectMusicPerformance	*pwPerformance	= NULL;
    CtIDirectMusicGraph			*pCtGraph		= NULL;
    CtIDirectMusicTool			*pCtTool		= NULL;
    LPVOID*						pCtTools		= NULL;

    DWORD						pdwPChannels[]	= {0,1,2,3,4,5}; //Array of PChannels just to make scenario richer.

    DWORD						dwIndex			= 0;
    DWORD						dwNumTools		= (DWORD)dwParam1; //Number of tools to create
    LONG						lToolIndex		= (LONG)(dwParam2 - 1); //Index at which to Insert Tool #(dwNumTools + 1) at
    DWORD						dwUniqueID		= 123456;
    CTDMTestTool*				pTestTool		= NULL;
    DMUS_PMSG                   *pdmusPMSG      = NULL;

    // **********************************************************************
    // 1) Initialize COM.
    // **********************************************************************
    hr = dmthCoInitialize(NULL);
    if(FAILED(hr))
    {
        fnsLog(ABORTLOGLEVEL, "**** ABORT: COM initialization failed! (%s == %08Xh)",
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
    // 3) Init(NULL) and AddPort(NULL)
    // **********************************************************************
	hr = pwPerformance->Init(NULL,NULL,ghWnd);
	if(FAILED(hr))
	{
        fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: pwPerformance->Init(NULL) ")
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
        fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: pwPerformance->AddPort(NULL) ")
                TEXT("failed (%s == %08Xh)"),
                tdmXlatHRESULT(hr), hr);
		dwRes = FNS_ABORTED;
		goto TEST_END;
	}

    // **********************************************************************
    // 3) Create Graph Object
    // **********************************************************************
    hr = dmthCreateGraph(IID_IDirectMusicGraph, &pCtGraph);
    if(FAILED(hr))
    {
        fnsLog(ABORTLOGLEVEL, "**** ABORT: dmthCreateGraph "
                "failed (%s == %08Xh)", tdmXlatHRESULT(hr), hr);
        dwRes = FNS_ABORTED;
        goto TEST_END;
    }

	// **********************************************************************
    // 4) Call AllocPMsg on pNote and check result
    // **********************************************************************
    hr = pwPerformance->AllocPMsg( sizeof(DMUS_PMSG), &pdmusPMSG );
    if(FAILED(hr))
    {
        fnsLog(ABORTLOGLEVEL, "**** ABORT: Performance->AllocPMsg "
                "failed (%s == %08Xh)", tdmXlatHRESULT(hr), hr);
        dwRes = FNS_ABORTED;
        goto TEST_END;
    }

    // **********************************************************************
    // 5.1) Set pUser with a bunch of particulars
    // **********************************************************************	
    pdmusPMSG->dwType   = DMUS_PMSGT_USER; //which is a message the Unique Tool handles
    pdmusPMSG->dwFlags  = DMUS_PMSGF_REFTIME | DMUS_PMSGF_TOOL_ATTIME;
//    pdmusPMSG->dwFlags  = DMUS_PMSGF_REFTIME | DMUS_PMSGF_TOOL_IMMEDIATE;
//    pdmusPMSG->dwFlags  = DMUS_PMSGF_MUSICTIME | DMUS_PMSGF_TOOL_ATTIME;
    pdmusPMSG->rtTime   = 0;
	pdmusPMSG->mtTime   = 0;
    pdmusPMSG->punkUser = pTestTool;

    // **********************************************************************
    // 4.1) Create Array of Tools and insert them
    // **********************************************************************
    pCtTools = new LPVOID[dwNumTools];  
    for (dwIndex = 0; dwIndex < ( dwNumTools ); dwIndex++)
    {
        //Create a test tool
        hr = dmthCreateTestTool(IID_IDirectMusicTool, ((CtIDirectMusicTool**)(&pCtTools[dwIndex])) );
        if (FAILED( hr ))
        {
            fnsLog(ABORTLOGLEVEL, "**** ABORT: dmthCreateTestTool (Tool #%d of %d)"
                    "failed (%s == %08Xh)", dwIndex, dwNumTools, tdmXlatHRESULT(hr), hr);
            dwRes = FNS_ABORTED;
            goto TEST_END;
        }
		
		// **********************************************************************
		// 4.2) Check Validity of the tool
		// **********************************************************************
        if (NULL == (&pCtTools[dwIndex]))
        {   
            fnsLog(ABORTLOGLEVEL, "**** ABORT: Something wrong with Tool[%d].  Should not be NULL.",
                                   dwIndex);
            dwRes = FNS_ABORTED;
            goto TEST_END;
        }

        hr = ((CtIDirectMusicTool*)(pCtTools[dwIndex]))->QueryInterface(IID_ITDMTESTTOOL, (void**)&pTestTool);
        if (FAILED( hr ))
        {
            fnsLog(ABORTLOGLEVEL, "**** ABORT: pCtTool->QI for TDMTestTool Failed"
                        "(%s == %08Xh)", tdmXlatHRESULT(hr), hr);
            goto TEST_END;
        }

        pTestTool->m_dwToolID = dwIndex;

	    // **********************************************************************
	    // 4.3) Set each tools m_fLogFlags to log USER PMsgs
	    // **********************************************************************   
	    pTestTool->SetLogFlags(TDMTESTTOOL_PMSGT_USER);

		//To Balance out the Ref
        pTestTool->Release();
        pTestTool = NULL;

        // *************************************************************************
        // 4.4) Insert Tool at Index dwIndex. If last tool, insert at desired index
        // *************************************************************************
        hr = pCtGraph->InsertTool( (CtIDirectMusicTool*)pCtTools[dwIndex], (DWORD*)&pdwPChannels, 6, 0);
        if(FAILED(hr))
        {
            fnsLog(ABORTLOGLEVEL, "**** ABORT: Graph->InsertTool "
                    "failed (%s == %08Xh)", tdmXlatHRESULT(hr), hr);
            dwRes = FNS_ABORTED;
            goto TEST_END;
        }
   
	}

   
    // *************************************************************************
    // 4.5) Add Graph to the performance
    // *************************************************************************
	hr = pwPerformance->SetGraph(pCtGraph);
    if(FAILED(hr))
    {
        fnsLog(ABORTLOGLEVEL, "**** ABORT: Performance->SetGraph "
                "failed (%s == %08Xh)", tdmXlatHRESULT(hr), hr);
        dwRes = FNS_ABORTED;
        goto TEST_END;
    }
		

    // *************************************************************************
    // 4.6) Stamp the pMsg
    // *************************************************************************
	hr = pCtGraph->StampPMsg(pdmusPMSG);
    if(FAILED(hr))
    {
        fnsLog(ABORTLOGLEVEL, "**** ABORT: Graph->StampPMsg "
                "failed (%s == %08Xh)", tdmXlatHRESULT(hr), hr);
        dwRes = FNS_ABORTED;
        goto TEST_END;
    }


//	Sleep(4000);

    // **********************************************************************
    // X) Call Perf->SendPMsg with Test Tool attached in punkUser of PMsg
    // **********************************************************************
    hr = pwPerformance->SendPMsg(pdmusPMSG);
    if(FAILED(hr))
    {
        fnsLog(ABORTLOGLEVEL, "**** ABORT: Perf->SendPMsg "
                "failed (%s == %08Xh)", tdmXlatHRESULT(hr), hr);
        dwRes = FNS_ABORTED;
        goto TEST_END;
    }

	Sleep(2000);

    // **********************************************************************
    // 6) Verify that the each tool was incremented in the USER pmsg
    // **********************************************************************   
    for (dwIndex = 0; dwIndex < ( dwNumTools ); dwIndex++)
    {
		fnsLog(FYILOGLEVEL, "Checking Tool #%d", dwIndex);
        hr = ((CtIDirectMusicTool*)(pCtTools[dwIndex]))->QueryInterface(IID_ITDMTESTTOOL, (void**)&pTestTool);
        if (FAILED( hr ))
        {
            fnsLog(ABORTLOGLEVEL, "**** ABORT: pCtTool->QI for TDMTestTool Failed"
                        "(%s == %08Xh)", tdmXlatHRESULT(hr), hr);
		    dwRes = FNS_FAIL;
            goto TEST_END;
        }
		if(pTestTool->m_dwUserPmsg != 1)
        {
            fnsLog(BUGLOGLEVEL, "!!!! BUG: UserPMsg was not incramented correctly"
                        "(m_dwUserPmsg == %d)(index == %d)", pTestTool->m_dwUserPmsg,dwIndex);
	        dwRes = FNS_FAIL;
            goto TEST_END;
        }
        pTestTool->Release();
        pTestTool = NULL;
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
    if(pCtGraph)
    {
        pCtGraph->Release();
        pCtGraph = NULL;
    }

    if (pTestTool)
    {
        pTestTool->Release();
        pTestTool = NULL;
    }

    if (pCtTool)
    {
        pCtTool->Release();
        pCtTool = NULL;
    }

    //Nuke the tools
	if ( pCtTools )
	{
		for (dwIndex = 0; dwIndex < (dwNumTools); dwIndex++)
		{
	        if(pCtTools[dwIndex])
			{
	            if (((CtIDirectMusicTool*)pCtTools[dwIndex]))
				{
	                ((CtIDirectMusicTool*)pCtTools[dwIndex])->Release();
					pCtTools[dwIndex] = NULL;
				}
			}
		} 
		delete [] pCtTools;
		pCtTools = NULL;
	}

    dmthCoUninitialize();   
    return dwRes;
}


//--------------------------------------------------------------------------;
// @doc IDIRECTMUSICPERFORMANCE A-LLUCAR
//
// @topic IDirectMusicPerformance::RhythmToTime()_Valid_Test |
//
// Description: <nl>
// Unit test function for IDirectMusicPerformance::RhythmToTime().
//		Compares RhythmToTime and TimeToRhythm using Midi File().
//
// Test Function: <nl>
// tdmperfvRhythmToTimeValidTest1 <nl>
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
// 3) Load the media <nl>
// 4) Set up the performance to play <nl>
// 5) Play the Segment <nl>
// 6) Get the current MUSIC_TIME <nl>
// 7) Get the current Time Signature using Performance->GetParam <nl>
// 8)Call GetStartTime to get the start of the segment <nl>
// 9)Call TimeToRhythm <nl>
// 10)Call RhythmToTime and compare mtTime with mtTime2 <nl>
// 11)Clean up objects, uninitialize COM and return results <nl>
//
// History: <nl>
//  11/18/1998 - a-llucar - created <nl>
//
// @index topic | IDIRECTMUSICPERFORMANCE_CASES
//--------------------------------------------------------------------------;
DWORD tdmperfvRhythmToTimeValidTest1(BOOL fCoCreate, BOOL fMaxDebug, 
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
    // 3) Load the media
    // **********************************************************************
	hr = dmthLoadSegment(L"onestop2.mid", &pwSegment);
	if (FAILED(hr))
	{
		fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: dmthLoadSegment")
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
    // 5) Call PlaySegment and check results
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
    // 6) Get the current MusicTime
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
    // 7) Get the Time Signature
    // **********************************************************************
	hr = pwPerformance->GetParam(GUID_TimeSignature, 0xffffffff, 0, mtTime, &mtStartTime, (void*)&TimeSig);
	if(FAILED(hr))
	{
        fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: IDirectMusicPerformance->GetParam() ")
                TEXT("failed (%s == %08Xh)"),
                tdmXlatHRESULT(hr), hr);
		dwRes = FNS_ABORTED;
		goto TEST_END;
	}

	TimeSig.mtTime += mtTime;
	fnsLog(FYILOGLEVEL, "mtTime == %d\nTimeSig.mtTime == %d", mtTime, TimeSig.mtTime);

    // **********************************************************************
    // 8) Now we'll call TimeToRhythm to populate the TimeSignature
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
    // 9) Call RhythmToTime and check results
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

		pwPerformance->TimeToRhythm(mtTime, &TimeSig, &wMeasure, &bBeat, &bGrid, &nOffset);
		pwPerformance->RhythmToTime(wMeasure, bBeat, bGrid, nOffset, &TimeSig, &mtTime2);


		fnsLog(BUGLOGLEVEL, TEXT("mtTime and mtTime2 are not equal\n ")
				TEXT("mtTime == %d\nmtTime2 == %d"),
				mtTime, mtTime2);
		dwRes = FNS_FAIL;
		goto TEST_END;
	}

TEST_END:
    // **********************************************************************
    // 10) Cleanup objects and uninitialize COM and Return results
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
// @topic IDirectMusicPerformance::MIDIToMusic()_Valid_Test |
//
// Description: <nl>
// Valid test function for IDirectMusicPerformance::MIDIToMusic().
//		Call MIDIToMusic with:
//			A chord containing a key of 0xC, scalepattern of 0xab5ab5, and
//			a subchord with chord pattern 0x81, scale pattern 0x81081, chord
//			root of 0xc, and scale root of 0.  MIDI value of 0x36.  
//		It should return a MusicValue of 0x410f.
//
// Test Function: <nl>
// tdmperfvMIDIToMusicValidTest1 <nl>
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
// 2) Create a Performance Object <nl>
// 3) dmthPrePlay <nl>
// 4) Set DMUS_CHORD_KEY with relevant info <nl>
// 5) Call MIDIToMusic <nl>
// 6) Call MusicToMIDI and make sure we get the right information back <nl>
// 7)Clean up objects, uninitialize COM and return results <nl>
//
// History: <nl>
//  11/18/1998 - a-llucar - created <nl>
//
// @index topic | IDIRECTMUSICPERFORMANCE_CASES
//--------------------------------------------------------------------------;
DWORD tdmperfvMIDIToMusicValidTest1(BOOL fCoCreate, BOOL fMaxDebug, 
						   LPARAM dwParam1, LPARAM dwParam2)
{
	DWORD						dwRes			= FNS_PASS;
	HRESULT						hr				= S_OK;
	CtIDirectMusicPerformance	*pwPerformance	= NULL;
	
	WORD						pwMusicValue;
	DMUS_CHORD_KEY				ChordKey;
	BYTE						pbMidiValue;

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
    // 3) dmthPrePlay
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
    // 4) Set DMUS_CHORD_KEY with relevant info
    // **********************************************************************		
	ChordKey.bKey = 0xC;
	ChordKey.dwScale = 0xab5ab5;
	ChordKey.bSubChordCount = 1;
	ChordKey.SubChordList[0].dwChordPattern = 0x81;
	ChordKey.SubChordList[0].dwScalePattern = 0x81081;
	ChordKey.SubChordList[0].bChordRoot = 0xC;
	ChordKey.SubChordList[0].bScaleRoot = 0;
	
	// **********************************************************************
    // 5) Call MIDIToMusic
    // **********************************************************************		
	hr = pwPerformance->MIDIToMusic(0x36, &ChordKey,
									DMUS_PLAYMODE_CHORD_ROOT | DMUS_PLAYMODE_SCALE_INTERVALS,
									0,
									&pwMusicValue);
	if(FAILED(hr))
	{
        fnsLog(BUGLOGLEVEL, TEXT("!!!! BUG: Performance->MIDIToMusic ")
                TEXT("failed (%s == %08Xh)"), tdmXlatHRESULT(hr), hr);
		dwRes = FNS_FAIL;
		goto TEST_END;
	}
	// Verify that we got the right information
	if(pwMusicValue != 0x420f)
	{
		fnsLog(BUGLOGLEVEL, "pwMusicValue did not return correctly (pwMusicValue == %08X)", pwMusicValue);
		dwRes = FNS_FAIL;
		goto TEST_END;
	}

	// **********************************************************************
    // 6) Call MusicToMIDI and make sure we get the right information back
    // **********************************************************************		
	hr = pwPerformance->MusicToMIDI(pwMusicValue, &ChordKey,
									DMUS_PLAYMODE_CHORD_ROOT | DMUS_PLAYMODE_SCALE_INTERVALS,
									0,
									&pbMidiValue);
	
	if(FAILED(hr))
	{
        fnsLog(BUGLOGLEVEL, TEXT("!!!! BUG: Performance->MusicToMIDI ")
                TEXT("failed (%s == %08Xh)"), tdmXlatHRESULT(hr), hr);
		dwRes = FNS_FAIL;
		goto TEST_END;
	}
	// Verify that we got the right information
	if(pbMidiValue != 0x36)
	{
		fnsLog(BUGLOGLEVEL, "pbMidiValue did not return correctly (pbMidiValue == %08X)", pbMidiValue);
		dwRes = FNS_FAIL;
		goto TEST_END;
	}

TEST_END:
    // **********************************************************************
    // 7) Cleanup objects and uninitialize COM and Return results
    // **********************************************************************
	dmthReleasePerformance(pwPerformance);
	dmthCoUninitialize();	
	return dwRes;
}
//===========================================================================



//--------------------------------------------------------------------------;
// @doc IDIRECTMUSICPERFORMANCE A-LLUCAR
//
// @topic IDirectMusicPerformance::MIDIToMusic()_Valid_Test |
//
// Description: <nl>
// Valid test function for IDirectMusicPerformance::MIDIToMusic().
//		Call MIDIToMusic with (truyly) random information for key, scale
//		pattern, subchord, and chord.  Get the Midi Value, and then call
//		MusicToMIDI and verify that we get the same inforamtion back.
//
// Test Function: <nl>
// tdmperfvMIDIToMusicValidTest2 <nl>
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
DWORD tdmperfvMIDIToMusicValidTest2(BOOL fCoCreate, BOOL fMaxDebug, 
						   LPARAM dwParam1, LPARAM dwParam2)
{
	DWORD						dwRes				= FNS_PASS;
	HRESULT						hr					= S_OK;
	CtIDirectMusicPerformance	*pwPerformance		= NULL;
	
	BYTE						bMIDIValue			= 0;
	BYTE						pbMIDIValueResult	= 0;
	WORD						wMusicValue			= 0;
	WORD						pwMusicValueResult	= 0;
	
	DMUS_CHORD_KEY				ChordKey;
	BYTE						bKey				= 0;
	DWORD						dwScale				= 0;
	DWORD						dwChordPattern		= 0;
	DWORD						dwScalePattern		= 0;
	BYTE						bChordRoot			= 0; // 0 - 23
	BYTE						bScaleRoot			= 0; // 0 - 23
	BYTE						bPlayMode			= 0; // 1 - 32
	long						CurrentNum			= 0; // PlaceHolder
	long						Seed				= 0;
	
	BYTE						bMIDIValueInc		= 0;
	BYTE						bPlayModeInc		= 0;
	struct _timeb tstruct;
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
    // 3) dmthPrePlay
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
    // 5) Call MIDIToMusic
    // **********************************************************************		
	for (bMIDIValueInc = 0; bMIDIValueInc < 127; bMIDIValueInc++)
	{
	    _ftime( &tstruct );
	    Seed = ((tstruct.millitm) * (tstruct.millitm));		
		fnsLog(CALLLOGLEVEL, "bMIDIValue == %d", bMIDIValueInc);
		Sleep(388);
		srand((unsigned)(Seed));
		fnsLog(CALLLOGLEVEL, "---FYI: Seed == %u", Seed);
		
		ChordKey.bKey = (BYTE)rand();
		ChordKey.dwScale = (DWORD)rand();
		ChordKey.bSubChordCount = 1;
		ChordKey.SubChordList[0].dwChordPattern = (DWORD)rand();//dwChordPattern;
		ChordKey.SubChordList[0].dwScalePattern = (DWORD)rand();//dwScalePattern;
		ChordKey.SubChordList[0].bChordRoot = (BYTE)rand();//bChordRoot;
		ChordKey.SubChordList[0].bScaleRoot = (BYTE)rand();//bScaleRoot;
		bChordRoot = 20;	//0-23
		bScaleRoot = 16;	//0-23

		hr = pwPerformance->MIDIToMusic(bMIDIValueInc,//bMIDIValue,
										&ChordKey,
										0,
										0,
										&pwMusicValueResult);
		if(FAILED(hr))
		{
		    fnsLog(BUGLOGLEVEL, TEXT("!!!! BUG: Performance->MIDIToMusic ")
		            TEXT("failed (%s == %08Xh)"), tdmXlatHRESULT(hr), hr);
			dwRes = FNS_FAIL;
			goto TEST_END;
		}

		// **********************************************************************
		// 6) Call MusicToMIDI and make sure we get the right information back
		// **********************************************************************		
		hr = pwPerformance->MusicToMIDI(pwMusicValueResult, 
										&ChordKey,
										0,
										0,
										&pbMIDIValueResult);
	
		if(FAILED(hr))
		{
		    fnsLog(BUGLOGLEVEL, TEXT("!!!! BUG: Performance->MusicToMIDI ")
		            TEXT("failed (%s == %08Xh)"), tdmXlatHRESULT(hr), hr);
			dwRes = FNS_FAIL;
			goto TEST_END;
		}

		// Verify that we got the right information
		if(pbMIDIValueResult != bMIDIValueInc)
		{
			fnsLog(BUGLOGLEVEL, "!!!! BUG: The value we entered didn't come back out\nbMIDIValue == %08Xh\npbMIDIValueResult == %08Xh", bMIDIValueInc, pbMIDIValueResult);
			dwRes = FNS_FAIL;
			goto TEST_END;
		}
	}	
TEST_END:
    // **********************************************************************
    // 7) Cleanup objects and uninitialize COM and Return results
    // **********************************************************************
	dmthReleasePerformance(pwPerformance);
	dmthCoUninitialize();	
	return dwRes;
}
//===========================================================================


//--------------------------------------------------------------------------;
// @doc IDIRECTMUSICPERFORMANCE A-LLUCAR
//
// @topic IDirectMusicPerformance::GetGlobalParam()_Valid_Test3 |
//
// Description: <nl>
// Valid test function for IDirectMusicPerformance::GetGlobalParam(). <nl>
//		Test integrity of setting custom global params.
//
// Test Function: <nl>
// tdmperfvGetGlobalParamValidTest3 <nl>
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
// 2) Create CtIDirectMusicPerformance object <nl>
// 3) Create a 256 byte buffer, initialized so that each byyte value is it's index into the buffer (0, 1, 2, ... FF)
// 4) Set up the performance
// 5) Call SetGlobalParam with this GUID and buffer
// 6) Call GetGlobalParam with this GUID and a new buffer to receive previous buffer
// 7) Verify data by using a quick For loop and check byte values.
// 8) Clean up
// 
// History: <nl>
//	03/02/1999 - a-llucar - created
//  07/22/1999 - kcraven - fixed test 
//
// @index topic | IDIRECTMUSICPERFORMANCE_CASES
//--------------------------------------------------------------------------;
DWORD tdmperfvGetGlobalParamValidTest3(BOOL fCoCreate, BOOL fMaxDebug, 
						   LPARAM dwParam1, LPARAM dwParam2)
{
	DWORD						dwRes			= FNS_PASS;
	HRESULT						hr				= S_OK;
	CtIDirectMusicPerformance	*pwPerformance	= NULL;

	GUID						customGUID		= {0x4b97c03, 0xd0cd, 0x11d2, 0xa5, 0x65, 0x0, 0x60, 0x8, 0x95, 0xd3, 0xa1};
	int							i				= 0;

	DWORD						dwSizeOfBuffer	= (DWORD)dwParam1;

	BYTE						*bBuffer;
	BYTE						*bReturn;
	bBuffer						= (BYTE*)malloc(sizeof(BYTE) * dwSizeOfBuffer);
	bReturn						= (BYTE*)malloc(sizeof(BYTE) * dwSizeOfBuffer);

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
    // 3) Initialize the byte buffer
    // **********************************************************************
	for (i = 0; i < (int)dwSizeOfBuffer; i++)
	{
		bBuffer[i] = (BYTE)(i % 256);
		bReturn[i] = 0;
	}
	// **********************************************************************
    // 4) Init and AddPort
    // **********************************************************************		
	hr = dmthPrePlay(&pwPerformance);
    if(FAILED(hr))
	{
        fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: dmthPrePlay ")
                TEXT("failed (%s == %08Xh)"),
                tdmXlatHRESULT(hr), hr);
		dwRes = FNS_ABORTED;
		goto TEST_END;
	}
	// **********************************************************************
	// 5) Call SetGlobalParam, check results
	// **********************************************************************
	hr = pwPerformance->SetGlobalParam(customGUID, bBuffer, dwSizeOfBuffer);
	if(FAILED(hr))
	{
	    fnsLog(BUGLOGLEVEL, TEXT("!!!! BUG: IDirectMusicPerformance->SetGlobalParam ")
	            TEXT("failed (%s == %08Xh)"),
	            tdmXlatHRESULT(hr), hr);
		dwRes = FNS_FAIL;
		goto TEST_END;
	}
	// **********************************************************************
    // 6) Call GetGlobalParam, check results
    // **********************************************************************
	hr = pwPerformance->GetGlobalParam(customGUID, bReturn, dwSizeOfBuffer);
	if(FAILED(hr))
	{
        fnsLog(BUGLOGLEVEL, TEXT("!!!! BUG: IDirectMusicPerformance->GetGlobalParam ")
                TEXT("failed (%s == %08Xh)"),
                tdmXlatHRESULT(hr), hr);
		dwRes = FNS_FAIL;
		goto TEST_END;
	}
	// **********************************************************************
    // 7) Verify that the bReturn[i] and bBuffer[i] are the same
    // **********************************************************************
	for (i = 0; i < (int)dwSizeOfBuffer; i++)
	{
		// Check to make sure that the DWORD was properly populated
		if(bBuffer[i] != bReturn[i])
		{
	        fnsLog(BUGLOGLEVEL,
				TEXT("!!!! BUG: IDirectMusicPerformance->GetGlobalParam data not equal! ")
				TEXT("(index == %ld)(%ld != %ld)"),i,bBuffer[i], bReturn[i]);
			dwRes = FNS_FAIL;
			goto TEST_END;
		}	
	}	

TEST_END:
	// **********************************************************************
    // 8) Release objects, CoUninitialize, and return results
    // **********************************************************************
	dmthReleasePerformance(pwPerformance);
	free (bBuffer);
	free (bReturn);
	dmthCoUninitialize();	
	return dwRes;
}
//===========================================================================


//--------------------------------------------------------------------------;
// @doc IDIRECTMUSICPERFORMANCE A-LLUCAR
//
// @topic IDirectMusicPerformance::GetGlobalParam()_Valid_Test4 |
//
// Description: <nl>
// Valid test function for IDirectMusicPerformance::GetGlobalParam(). <nl>
//		Test integrity of setting custom global params.
//
// Test Function: <nl>
// tdmperfvGetGlobalParamValidTest4 <nl>
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
// 2) Create 10 GUIDs
// 3) Create 10 buffers - Initialize all byte values to index of buffers (first buffer will contain all 0's,
//    second all 1's, ect.) - use memset for this
// 4) Call SetGlobalParam for each GUID and buffer combination in random order
// 5) Retrieve the GUID's and buffers in random order
// 6) Verify each buffer by it's GUID index
// 7) Clean up
// 
// History: <nl>
//	03/02/1999 - a-llucar - created
//
// @index topic | IDIRECTMUSICPERFORMANCE_CASES
//--------------------------------------------------------------------------;
DWORD tdmperfvGetGlobalParamValidTest4(BOOL fCoCreate, BOOL fMaxDebug, 
						   LPARAM dwParam1, LPARAM dwParam2)
{
	DWORD						dwRes			= FNS_PASS;
	HRESULT						hr				= S_OK;
	CtIDirectMusicPerformance	*pwPerformance	= NULL;

	GUID						customGUID[10] = 
	{
	{0xf29b9152, 0xd287, 0x11d2, 0xa5, 0x6f, 0x0, 0x60, 0x8, 0x95, 0xd3, 0xa1},
	{0x6dc5480, 0xd288, 0x11d2, 0xa5, 0x6f, 0x0, 0x60, 0x8, 0x95, 0xd3, 0xa1},
	{0x16337c60, 0xd288, 0x11d2, 0xa5, 0x6f, 0x0, 0x60, 0x8, 0x95, 0xd3, 0xa1},
	{0x3f3d0270, 0xd288, 0x11d2, 0xa5, 0x6f, 0x0, 0x60, 0x8, 0x95, 0xd3, 0xa1},
	{0x4d54cc20, 0xd288, 0x11d2, 0xa5, 0x6f, 0x0, 0x60, 0x8, 0x95, 0xd3, 0xa1},
	{0x5b5a3f90, 0xd288, 0x11d2, 0xa5, 0x6f, 0x0, 0x60, 0x8, 0x95, 0xd3, 0xa1},
	{0x693f9c10, 0xd288, 0x11d2, 0xa5, 0x6f, 0x0, 0x60, 0x8, 0x95, 0xd3, 0xa1},
	{0x751d6cd0, 0xd288, 0x11d2, 0xa5, 0x6f, 0x0, 0x60, 0x8, 0x95, 0xd3, 0xa1},
	{0x832d9290, 0xd288, 0x11d2, 0xa5, 0x6f, 0x0, 0x60, 0x8, 0x95, 0xd3, 0xa1},
	{0x8c61ddb0, 0xd288, 0x11d2, 0xa5, 0x6f, 0x0, 0x60, 0x8, 0x95, 0xd3, 0xa1}
	};

	BYTE						bBuffer[10];
	BYTE						bReturn[10];

	int							i				= 0;
	
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
    // 3) Initialize the two buffers (setting bReturn[i] to something else
    // **********************************************************************
	for (i = 0; i < 10; i++)
	{
		bBuffer[i] = ((i * 10) + i);
		bReturn[i] = 125;
	}

	// **********************************************************************
    // 4) Init and AddPort
    // **********************************************************************		
	hr = dmthPrePlay(&pwPerformance);
    if(FAILED(hr))
	{
        fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: dmthPrePlay ")
                TEXT("failed (%s == %08Xh)"),
                tdmXlatHRESULT(hr), hr);
		dwRes = FNS_ABORTED;
		goto TEST_END;
	}


	for (i = 0; i <10; i = i + 2)
	{
		// **********************************************************************
		// 5) Call SetGlobalParam, check results
		// **********************************************************************
		hr = pwPerformance->SetGlobalParam(customGUID[i], &bBuffer[i], sizeof(BYTE));
		if(FAILED(hr))
		{
		    fnsLog(BUGLOGLEVEL, TEXT("!!!! BUG: IDirectMusicPerformance->SetGlobalParam ")
		            TEXT("failed (%s == %08Xh)"),
		            tdmXlatHRESULT(hr), hr);
			dwRes = FNS_FAIL;
			goto TEST_END;
		}
	}
	for (i = 1; i < 10; i = i + 2)
	{
		// **********************************************************************
		// 5) Call SetGlobalParam, check results
		// **********************************************************************
		hr = pwPerformance->SetGlobalParam(customGUID[i], &bBuffer[i], sizeof(BYTE));
		if(FAILED(hr))
		{
		    fnsLog(BUGLOGLEVEL, TEXT("!!!! BUG: IDirectMusicPerformance->SetGlobalParam ")
		            TEXT("failed (%s == %08Xh)"),
		            tdmXlatHRESULT(hr), hr);
			dwRes = FNS_FAIL;
			goto TEST_END;
		}
	}
	for (i = 9; i >= 0; i--)
	{
		// **********************************************************************
	    // 6) Call GetGlobalParam, check results
	    // **********************************************************************
		hr = pwPerformance->GetGlobalParam(customGUID[i], &bReturn[i], sizeof(BYTE));
		if(FAILED(hr))
		{
	        fnsLog(BUGLOGLEVEL, TEXT("!!!! BUG: IDirectMusicPerformance->GetGlobalParam ")
	                TEXT("failed (%s == %08Xh)"),
	                tdmXlatHRESULT(hr), hr);
			dwRes = FNS_FAIL;
			goto TEST_END;
		}
	
		// Check to make sure that the DWORD was properly populated
		if(bReturn[i] != ((i * 10) + i))
		{
	        fnsLog(BUGLOGLEVEL, TEXT("!!!! BUG: IDirectMusicPerformance->GetGlobalParam\n ")
	                TEXT("pData was not properly set\n")
	                TEXT("Expected -- bBuffer == %ld\n")
					TEXT("Received -- bReturn == %ld\n"),
					((i * 10) + i), bReturn[i]);
			dwRes = FNS_FAIL;
			goto TEST_END;
		}	
	}	

TEST_END:
	// **********************************************************************
    // 7) Release objects, CoUninitialize, and return results
    // **********************************************************************
	dmthReleasePerformance(pwPerformance);
	
	dmthCoUninitialize();	
	return dwRes;
}
//===========================================================================
