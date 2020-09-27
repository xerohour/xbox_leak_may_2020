//+-------------------------------------------------------------------------
//
//	Microsoft Windows
//
//	Copyright (C) Microsoft Corporation, 1998 - 1999
//
//	File:		tdmperv8.cpp
//
//--------------------------------------------------------------------------

//===========================================================================
// tdmperv8.cpp 
//
// Test case functions for IDirectMusicPerformance8 methods (valid parameters)
//
// Functions:
//	 tdmperv8ClonePMsg()
//	 tdmperv8CreateAudioPath()
//	 tdmperv8CreateStandardAudioPath()
//	 tdmperv8GetDefaultAudioPath()
//	 tdmperv8InitAudio()
//	 tdmperv8PlaySegmentEx()
//	 tdmperv8SetDefaultAudioPath()
//	 tdmperv8SetParamHook()
//	 tdmperv8StopEx()
//
// History:
//	01/27/2000 - kcraven  - created
//  08/29/2000 - danhaff - Removed instances of DYNAMIC_APATH_ENV3D
//===========================================================================

#include "tdmusic.h"
#include "tdmperf.h"
//PHOOPHOO
//#include "tdmhook.h"

#include <RecTool.h>
#include <factory.h>
#include <limits.h>
#include "RecTake.h"

#include "tdmapthHelpDefs.h"

BOOL HasDSOne(CtIDirectMusic* pMusic);
BOOL HasDSOne(CtIDirectMusic* pMusic)
{
	HRESULT hr = S_OK;
	DMUS_PORTCAPS portcaps;
	int index;

	if (pMusic)
	{
		for(index = 0;;index++)
		{
			ZeroMemory(&portcaps, sizeof(DMUS_PORTCAPS));
			portcaps.dwSize = sizeof(DMUS_PORTCAPS);
			hr = pMusic->EnumPort(index,&portcaps);
			if(SUCCEEDED(hr) && hr != S_FALSE)
			{
				if(DMUS_PC_OUTPUTCLASS == portcaps.dwClass)
				{
					if(portcaps.dwFlags & DMUS_PC_DLS)
					{
						if((
							!(portcaps.dwFlags & DMUS_PC_SOFTWARESYNTH)) &&
							(DMUS_PORT_KERNEL_MODE == portcaps.dwType) &&
							!wcscmp(portcaps.wszDescription,L"Yamaha DMusic"))
						{
							return TRUE;
						}
					}
				}
			}
			else
			{
				break;
			}
		}
	}
	return FALSE;
}

//--------------------------------------------------------------------------;
// @doc IDIRECTMUSICPERFORMANCE8 KCRAVEN
//
// @topic IDirectMusicPerformance8::AddNotificationType()_Valid_Test2 |
//
// Description: <nl>
// Valid test function for IDirectMusicPerformance8::AddNotificationType(). <nl>
//
// Test Function: <nl>
// tdmperv8AddNotificationTypeValidTest <nl>
//
// Parameters: <nl>
//  BOOL    fCoCreate   - unused <nl>
//  BOOL    fMaxDebug   - unused <nl>
//  DWORD   dwParam1    - unused <nl>
//	DWORD   dwParam2    - unused <nl>
//
// File : tdmperv8.CPP
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
//  06/08/2000 - kcraven - convrted to dx8 <nl>
//
//
// @index topic | IDIRECTMUSICPERFORMANCE8_CASES
//--------------------------------------------------------------------------;
DWORD tdmperv8AddNotificationTypeValidTest(BOOL fCoCreate, 
											BOOL fMaxDebug, 
											LPARAM dwParam1, 
											LPARAM dwParam2)
{
	DWORD						dwRes				= FNS_PASS;
	HRESULT						hr					= S_OK;
	DMUS_NOTIFICATION_PMSG		*pEvent				= NULL;
	CtIDirectMusicPerformance8	*pwPerformance		= NULL;
	CtIDirectMusicLoader8		*pwLoader			= NULL;
	CtIDirectMusicSegment8		*pwSegment			= NULL;

	HANDLE						hNotification;
	DWORD						dwResult;

	GUID						*guidNotification	= (GUID*)dwParam1;
	WCHAR						*pMidiFile			= (WCHAR*)dwParam2;
    char                        szGuid[MAX_LOGSTRING];

	if (pMidiFile == NULL)
	{
		pMidiFile = L"test.mid";
	}	
	dmthSetPath(pMidiFile);
	char						*chPath			= gszMediaPath;
	WCHAR						*wchPath		= gwszBasePath;
	LPWSTR                      wszFilename     = pMidiFile; 

	int							counter			= 0;
	int							SleepTime		= 1000;
	int							TimeOutValue	= 10;
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
    // 1) Create CtIDirectMusicLoader object (wrapped IDirectMusicLoader object)
    // **********************************************************************
	hr = dmthCreateLoader(IID_IDirectMusicLoader8, &pwLoader);
	if (FAILED(hr))
	{
        fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: CoCreate Loader object ")
                TEXT("failed (%s == %08Xh)"),
                tdmXlatHRESULT(hr), hr);
        goto TEST_END;
	}
	// **********************************************************************
    // 3.1) SetSearchDirectory
    // **********************************************************************
	hr = pwLoader->SetSearchDirectory(GUID_DirectMusicAllTypes, wchPath, FALSE);
	if (FAILED(hr))
	{
		fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: Loader->SetSeachDirectory(media directory) ")
				TEXT("failed (%s == %08Xh)"),
				tdmXlatHRESULT(hr), hr);
		goto TEST_END;
	}
	// **********************************************************************
    // 4) Get the Segment object
    // **********************************************************************
	DMUS_OBJECTDESC desc; 
	ZeroMemory(&desc, sizeof(desc));
	desc.dwSize = sizeof(DMUS_OBJECTDESC);
	desc.guidClass = CLSID_DirectMusicSegment;
	desc.dwValidData |= (DMUS_OBJ_CLASS | DMUS_OBJ_FILENAME | DMUS_OBJ_FULLPATH);
	MultiByteToWideChar(CP_ACP, NULL, (LPCSTR)chPath, -1, desc.wszFileName, DMUS_MAX_FILENAME);
	hr = pwLoader->GetObject(&desc,CTIID_IDirectMusicSegment8,(void **)&pwSegment);
	if(FAILED(hr))
    {
        fnsLog(ABORTLOGLEVEL, TEXT("**** Loader->GetObject(pwSegment) ")
                TEXT("failed (%s == %08Xh)"),
                tdmXlatHRESULT(hr), hr);
        fnsLog(ABORTLOGLEVEL, TEXT("Test case aborting"));
        goto TEST_END;
    }

    // **********************************************************************
    // 6) Add the Port
    // **********************************************************************	
	hr = pwPerformance->InitAudio(NULL,NULL,ghWnd,DMUS_APATH_SHARED_STEREOPLUSREVERB,64,NULL,NULL);
	if(FAILED(hr))
	{
	    fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: CtIDirectMusicBase creation ")
	            TEXT("failed (%s == %08Xh)"),
	            tdmXlatHRESULT(hr), hr);
		goto TEST_END;
	}
	// **********************************************************************
    // set autodownload on
	// **********************************************************************
    BOOL fAuto;
    fAuto = TRUE;
    hr = pwPerformance->SetGlobalParam( GUID_PerfAutoDownload, &fAuto, sizeof(BOOL) );
	if(FAILED(hr))
	{
		fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: Performance->SetGlobalParam(AutoDownload == TRUE) ")
				TEXT("failed (%s == %08Xh)"),
				tdmXlatHRESULT(hr), hr);
		goto TEST_END;
	}
	// **********************************************************************
    // set autodownload on
	// **********************************************************************
//	hr = pwPerformance->PlaySegment(pwSegment, DMUS_SEGF_BEAT, 0,NULL);
	hr = pwPerformance->PlaySegmentEx(pwSegment,0,NULL,DMUS_SEGF_QUEUE,0,NULL,NULL,NULL); 
	if(FAILED(hr))
	{
        fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: pwPerformance->PlaySegment ")
                TEXT("failed (%s == %08Xh)"),
                tdmXlatHRESULT(hr), hr);
		goto TEST_END;
	}
    // **********************************************************************
    // 10) Wait for IsPlaying to return favorably (or time out)
    // **********************************************************************
	// Set counter back to zero
	counter = 0;
	while ((hr = pwPerformance->IsPlaying(pwSegment, NULL)) != S_OK)	
	{
		Sleep(SleepTime);
		if(counter++ >TimeOutValue)
		{
			fnsLog(ABORTLOGLEVEL, TEXT("**** FYI: PlaySegment() timed out\n")
				TEXT("IsPlaying did not return the Segment (%s == %08Xh)"),
				tdmXlatHRESULT(hr), hr);
			goto TEST_END;
		}
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
	if(pwLoader)
	{
		pwLoader->Release();
		pwLoader = NULL;
	}
	dmthCoUninitialize();	
	return dwRes;
}




//--------------------------------------------------------------------------;
// @doc IDIRECTMUSICPERFORMANCE8 KCRAVEN
//
// @topic IDirectMusicPerformance8::ClonePMsg()_Valid_Test |
//
// Description: <nl>
// Valid test function for IDirectMusicPerformance8::ClonePMsg()<nl>
//
// Test Function: <nl>
// tdmperv8ClonePMsg()<nl>
//
// Parameters: <nl>
//	BOOL	fCoCreate	- unused <nl>
//	BOOL	fMaxDebug	- unused <nl>
//	DWORD	dwParam1	- unused <nl>
//	DWORD	dwParam2	- unused <nl>
//
// File : tdmperv8.cpp<nl>
//
// Steps: <nl>
//  1) CoInitialize<nl>
//  2) CoCreatePerformance <nl>
//  3) CoCreateLoader<nl>
//  4) Loader->GetObject(Segment)<nl>
//  5) Performance->InitAudio<nl>
//  6) Performance->CreateStandardAudioPath<nl>
//  7) Create Test Tool<nl>
//  8) CoCreateGraph<nl>
//  9) Performance->SetGraph<nl>
//  10) Segment->SetParam(GUID_Download)<nl>
//  11) Performance->AllocPMsg<nl>
//  12) Set PMsg values<nl>
//  13) Performance->ClonePMsg<nl>
//  14) Compare PMsgs<nl>
//  15) Change cloned pmsg<nl>
//  16) Performance->PlaySegmentEx<nl>
//  17) Performance->GetLatencyTime<nl>
//  18) Performance->ReferenceToMusicTime<nl>
//  19) SegState->QueryInterface(IID_IDirectMusicGraph<nl>
//  20) Graph->StampPMsgs<nl>
//  21) Wait for IsPlaying to be true<nl>
//  22) Performance->SendPMsg(both)<nl>
//  23) Wait for IsPlaying to be false<nl>
//  24) Cleanup objects and uninitialize and Return results<nl>
//
// History: <nl>
//	03/15/2000 - kcraven - created<nl>
//	04/18/2000 - kcraven - finished<nl>
//
// @index topic | IDIRECTMUSICPERFORMANCE8
//--------------------------------------------------------------------------;
DWORD tdmperv8ClonePMsg(
	BOOL fCoCreate,
	BOOL fMaxDebug,
	LPARAM dwParam1,
	LPARAM dwParam2)
{
	DWORD						dwRes			= FNS_ABORTED;
	HRESULT 					hr				= S_OK;
	CtIDirectMusicPerformance8* pwPerformance	= NULL;
	IDirectMusicPerformance8*	pPerformance	= NULL;
	CtIDirectMusicAudioPath*	pNewPath		= NULL;
	CtIDirectMusicLoader*		pwLoader		= NULL;
	CtIDirectMusicSegment*		pwSegment		= NULL;
	CtIDirectMusicSegmentState* pwSegState		= NULL;
	CtIDirectMusicGraph*		pwGraph 		= NULL;
	IDirectMusicGraph*			pGraph			= NULL;

	DWORD						dwPMsgType		= dwParam1;
	DWORD						dwPMsgFlags		= dwParam2;			// additional flags to set	
	ULONG 						ulPMsgSize;

	// generic pointer
	DMUS_PMSG**					ppPMsg;
	DMUS_PMSG**					ppPMsgClone;

//	DMUS_PMSG*					pBase;
//	DMUS_PMSG*					pBaseClone;
//	DMUS_CHANNEL_PRIORITY_PMSG*	pChanPri;
//	DMUS_CHANNEL_PRIORITY_PMSG*	pChanPriClone;
	DMUS_CURVE_PMSG*			pCurve;
	DMUS_CURVE_PMSG*			pCurveClone;
	DMUS_MIDI_PMSG*				pMidi;
	DMUS_MIDI_PMSG*				pMidiClone;
	DMUS_NOTE_PMSG*				pNote;
	DMUS_NOTE_PMSG*				pNoteClone;
//	DMUS_NOTIFICATION_PMSG*		pNotify;
//	DMUS_NOTIFICATION_PMSG*		pNotifyClone;
	DMUS_PATCH_PMSG*			pPatch;
	DMUS_PATCH_PMSG*			pPatchClone;
//	DMUS_SYSEX_PMSG*			pSysex;
//	DMUS_SYSEX_PMSG*			pSysexClone;
	DMUS_TEMPO_PMSG*			pTempo;
	DMUS_TEMPO_PMSG*			pTempoClone;
//	DMUS_TIMESIG_PMSG*			pTimeSig;
//	DMUS_TIMESIG_PMSG*			pTimeSigClone;
	DMUS_TRANSPOSE_PMSG*		pTrans;
	DMUS_TRANSPOSE_PMSG*		pTransClone;

	/*
	struct
	{
		WORD octave : 4;
		WORD chord : 4;
		WORD scale : 4;
		WORD accident : 4;
	} wMusicValue;

	wMusicValue.octave = 0;
	wMusicValue.chord = 0;
	wMusicValue.scale = 0;
	wMusicValue.accident = 0;
	*/

	MUSIC_TIME					mtTimeNow;
	REFERENCE_TIME				rtTimeNow;

	DWORD						dwType			= DMUS_APATH_SHARED_STEREOPLUSREVERB;
	DWORD						dwPChannelCount = 16;

	WCHAR						pMidiFile[] 	= L"test.mid";
	dmthSetPath(pMidiFile);
	WCHAR*						wchPath 		= gwszBasePath;
	WCHAR*						wchFile 		= gwszMediaPath;

	DMUS_OBJECTDESC 			desc; 

//	MUSIC_TIME					mtLength;
//	REFERENCE_TIME				rtLength;
//	REFERENCE_TIME				rtMeasure;
//	REFERENCE_TIME				rtTmp;

	// gets intialized below
	int 						counter;
	int 						SleepTime;
	int 						iCheckPlaying; 

    CRecTake*					pTake			= NULL;

	// **********************************************************************
	// 1) CoInitialize
	// **********************************************************************
	hr = dmthCoInitialize(NULL);
	if(FAILED(hr))
	{
		fnsLog(ABORTLOGLEVEL,
			TEXT("**** ABORT:  initialization failed! (%s == %08Xh)"),
			tdmXlatHRESULT(hr), hr);
		goto TEST_END;
	}
	// **********************************************************************
	// 2) CoCreatePerformance 
	// **********************************************************************
	hr = dmthCreatePerformance(IID_IDirectMusicPerformance8, &pwPerformance);
	if(FAILED(hr))
	{
		fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: IDirectMusicPerformance8 object creation "),
			TEXT("failed (%s == %08Xh)"),
			tdmXlatHRESULT(hr), hr);
		goto TEST_END;
	}
	// **********************************************************************
	// 3) CoCreateLoader
	// **********************************************************************
	hr = dmthCreateLoader(IID_IDirectMusicLoader, &pwLoader);
	if (FAILED(hr))
	{
		fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: CoCreate Loader object ")
				TEXT("failed (%s == %08Xh)"),
				tdmXlatHRESULT(hr), hr);
		goto TEST_END;
	}
	// **********************************************************************
	// 4) Loader->GetObject(Segment)
	// **********************************************************************
	ZeroMemory(&desc, sizeof(desc));
	desc.dwSize = sizeof(DMUS_OBJECTDESC);
	desc.guidClass = CLSID_DirectMusicSegment;
	desc.dwValidData |= (DMUS_OBJ_CLASS | DMUS_OBJ_FILENAME | DMUS_OBJ_FULLPATH);
	wcscpy(desc.wszFileName,wchFile);
	hr = pwLoader->GetObject(&desc,CTIID_IDirectMusicSegment,(void **)&pwSegment);
//	fnsIncrementIndent();
//	fnsLog(FYILOGLEVEL, TEXT("**** FYI: Loader->GetObject(pwSegment) ")
//				TEXT("returned (%s == %08Xh)"),
//				tdmXlatHRESULT(hr), hr);
//	fnsDecrementIndent();
	if(FAILED(hr))
	{
		fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: Loader->GetObject(pwSegment) ")
				TEXT("failed (%s == %08Xh)"),
				tdmXlatHRESULT(hr), hr);
		goto TEST_END;
	}
	// **********************************************************************
	// 5) Performance->InitAudio
	// **********************************************************************
	hr = pwPerformance->InitAudio(NULL,NULL,ghWnd,0,0,0,NULL); // don't create and audiopath 
	if(FAILED(hr))
	{
		fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: IDirectMusicPerformance8->InitAudio ")
				TEXT("failed (%s == %08Xh)"),
				tdmXlatHRESULT(hr), hr);
		goto TEST_END;
	}
	// **********************************************************************
	// 6) Performance->CreateStandardAudioPath
	// **********************************************************************
	hr = pwPerformance->CreateStandardAudioPath(
		dwType, 			// Choice of standard audio path.
		dwPChannelCount,	// How many pchannels to create.
		TRUE,
		&pNewPath);
	if(FAILED(hr))
	{
		fnsLog(BUGLOGLEVEL, TEXT("**** ABORT: IDirectMusicPerformance8->CreateStandardAudioPath ")
				TEXT("failed (%s == %08Xh)"),
				tdmXlatHRESULT(hr), hr);
		goto TEST_END;
	}
	if(!pNewPath)
	{
		fnsLog(BUGLOGLEVEL, TEXT("**** ABORT: pNewPath is NULL"));
		goto TEST_END;
	}
	// **********************************************************************
	// 7) Create Test Tool
	// **********************************************************************
	pTake = CRecTake::Create();
	if(!pTake)
	{
		fnsLog(BUGLOGLEVEL, TEXT("**** ABORT: pTake is NULL"));
		goto TEST_END;
	}
	// **********************************************************************
	// 8) CoCreateGraph
	// **********************************************************************
	hr = dmthCreateGraph(IID_IDirectMusicGraph, &pwGraph);
	if(FAILED(hr))
	{
		fnsLog(ABORTLOGLEVEL, "**** ABORT: dmthCreateGraph "
				"failed (%s == %08Xh)", tdmXlatHRESULT(hr), hr);
		goto TEST_END;
	}
	// **********************************************************************
	// 9) Performance->SetGraph
	// **********************************************************************
	hr = pwPerformance->SetGraph(pwGraph);
	if(FAILED(hr))
	{
		fnsLog(ABORTLOGLEVEL, "**** ABORT: Performance->SetGraph "
				"failed (%s == %08Xh)", tdmXlatHRESULT(hr), hr);
		goto TEST_END;
	}
	// **********************************************************************
	// 10) Segment->SetParam(GUID_Download)
	// **********************************************************************	
	hr = pwPerformance->GetRealObjPtr(&pPerformance);
	if (FAILED( hr ))
	{
		fnsLog(ABORTLOGLEVEL, "**** ABORT: Failed to get Real Object Pointer(performance) "
				"(HRESULT: %s == %08Xh)", tdmXlatHRESULT(hr), hr);
		goto TEST_END;
	}
	hr = pwSegment->SetParam(GUID_Download, -1, 0, 0, (void*)pPerformance); // needs real object
	if(FAILED(hr))
	{
		fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: pwSegment->SetParam(Download) ")
				TEXT("failed (%s == %08Xh)"),
				tdmXlatHRESULT(hr), hr);
		goto TEST_END;
	}
	// **********************************************************************
	// 11) Performance->AllocPMsg
	// **********************************************************************
	switch(dwPMsgType)
	{
	case DMUS_PMSGT_CURVE:
		ulPMsgSize = sizeof(DMUS_CURVE_PMSG);
		ppPMsg = (DMUS_PMSG**)&pCurve;
		ppPMsgClone = (DMUS_PMSG**)&pCurveClone;
		break;
	case DMUS_PMSGT_MIDI:
		ulPMsgSize = sizeof(DMUS_MIDI_PMSG);
		ppPMsg = (DMUS_PMSG**)&pMidi;
		ppPMsgClone = (DMUS_PMSG**)&pMidiClone;
		break;
	case DMUS_PMSGT_NOTE:
		ulPMsgSize = sizeof(DMUS_NOTE_PMSG);
		ppPMsg = (DMUS_PMSG**)&pNote;
		ppPMsgClone = (DMUS_PMSG**)&pNoteClone;
		break;
//	case DMUS_PMSGT_NOTIFICATION:
//		ulPMsgSize = sizeof(DMUS_NOTIFICATION_PMSG);
//		ppPMsg = (DMUS_PMSG**)&pNotify;
//		ppPMsgClone = (DMUS_PMSG**)&pNotifyClone;
//		break;
	case DMUS_PMSGT_PATCH:
		ulPMsgSize = sizeof(DMUS_PATCH_PMSG);
		ppPMsg = (DMUS_PMSG**)&pPatch;
		ppPMsgClone = (DMUS_PMSG**)&pPatchClone;
		break;
//	case DMUS_PMSGT_SYSEX:
//		ulPMsgSize = sizeof(DMUS_SYSEX_PMSG);
//		ppPMsg = (DMUS_PMSG**)&pSysex;
//		ppPMsgClone = (DMUS_PMSG**)&pSysexClone;
//		break;
	case DMUS_PMSGT_TEMPO:
		ulPMsgSize = sizeof(DMUS_TEMPO_PMSG);
		ppPMsg = (DMUS_PMSG**)&pTempo;
		ppPMsgClone = (DMUS_PMSG**)&pTempoClone;
		break;
//	case DMUS_PMSGT_TIMESIG:
//		ulPMsgSize = sizeof(DMUS_TIMESIG_PMSG);
//		ppPMsg = (DMUS_PMSG**)&pTimeSig;
//		ppPMsgClone = (DMUS_PMSG**)&pTimeSigClone;
//		break;
	case DMUS_PMSGT_TRANSPOSE:
		ulPMsgSize = sizeof(DMUS_TRANSPOSE_PMSG);
		ppPMsg = (DMUS_PMSG**)&pTrans;
		ppPMsgClone = (DMUS_PMSG**)&pTransClone;
		break;
//	case DMUS_PMSGT_CHANNEL_PRIORITY:
//		ulPMsgSize = sizeof(DMUS_CHANNEL_PRIORITY_PMSG);
//		ppPMsg = (DMUS_PMSG**)&pChanPri;
//		ppPMsgClone = (DMUS_PMSG**)&pChanPriClone;
//		break;
	default:
		//not tested at the moment
			//DMUS_PMSGT_STOP
			//DMUS_PMSGT_DIRTY
			//DMUS_PMSGT_WAVE
			//DMUS_PMSGT_LYRIC
			//DMUS_PMSGT_SCRIPTLYRIC
			//DMUS_PMSGT_USER
		fnsLog(ABORTLOGLEVEL, TEXT("**** PENDING: PMsg type unsupported in this test case"));
		dwRes = FNS_PENDING;
		goto TEST_END;
	}
	hr = pwPerformance->AllocPMsg(ulPMsgSize,(DMUS_PMSG**)ppPMsg);
	if(FAILED(hr) || !(*ppPMsg))
	{
		fnsLog(BUGLOGLEVEL,TEXT("**** ABORT: IDMPerformance::AllocPMsg failed (%s == %08Xh)"),
			tdmXlatHRESULT(hr), hr);
		goto TEST_END;
	}
	// **********************************************************************
	// 12) Set PMsg values
	// **********************************************************************
	/*
		// common to all pmsgs

			#define DMUS_PMSG_PART                                                                              
			DWORD               dwSize;
			REFERENCE_TIME      rtTime;             //  real time (in 100 nanosecond increments)
			MUSIC_TIME          mtTime;             //  music time
			DWORD               dwFlags;            //  various bits (see DMUS_PMSGF_FLAGS enumeration)
			DWORD               dwPChannel;         //  Performance Channel. The Performance can
													//  use this to determine the port/channel.
			DWORD               dwVirtualTrackID;   //  virtual track ID
			IDirectMusicTool*   pTool;              //  tool interface pointer
			IDirectMusicGraph*  pGraph;             //  tool graph interface pointer
			DWORD               dwType;             //  PMSG type (see DMUS_PMSGT_TYPES defines)
			DWORD               dwVoiceID;          //  unique voice id which allows synthesizers to
													//  identify a specific event. For DirectX 6.0,
													//  this field should always be 0.
			DWORD               dwGroupID;          //  Track group id
			IUnknown*           punkUser;           //  user com pointer, auto released upon PMSG free
	*/
	(*ppPMsg)->dwFlags = DMUS_PMSGF_MUSICTIME | DMUS_PMSGF_DX8 | dwPMsgFlags;
	(*ppPMsg)->dwPChannel = 0; //DMUS_PCHANNEL_BROADCAST_SEGMENT;
	(*ppPMsg)->dwVirtualTrackID = 0;
	(*ppPMsg)->dwGroupID = -1;

	(*ppPMsg)->dwType = dwPMsgType;

	switch(dwPMsgType)
	{
	case DMUS_PMSGT_CURVE:
		//DMUS_CURVE_PMSG
		((DMUS_CURVE_PMSG*)*ppPMsg)->mtDuration			= 768 * 4;		// Curve lasts four beats.
		((DMUS_CURVE_PMSG*)*ppPMsg)->mtResetDuration	= 0;			// No reset.
		((DMUS_CURVE_PMSG*)*ppPMsg)->nStartValue		= 0;			// ignored
		((DMUS_CURVE_PMSG*)*ppPMsg)->nEndValue			= 127;			// fade to full volume.
		((DMUS_CURVE_PMSG*)*ppPMsg)->nResetValue		= 0;			// No reset.
		((DMUS_CURVE_PMSG*)*ppPMsg)->bType				= DMUS_CURVET_CCCURVE;
		((DMUS_CURVE_PMSG*)*ppPMsg)->bCurveShape		= DMUS_CURVES_LINEAR;
		((DMUS_CURVE_PMSG*)*ppPMsg)->bCCData			= 7;			// MIDI Volume controller.
		((DMUS_CURVE_PMSG*)*ppPMsg)->bFlags				= DMUS_CURVE_START_FROM_CURRENT; 
		((DMUS_CURVE_PMSG*)*ppPMsg)->wParamType			= 0; 			// This is for NRPN and RPN only.
		((DMUS_CURVE_PMSG*)*ppPMsg)->wMergeIndex		= 1;			// This will be added to the regular volume, not override it.
		break;
	case DMUS_PMSGT_MIDI:
		//DMUS_MIDI_PMSG
		((DMUS_MIDI_PMSG*)*ppPMsg)->bStatus				= MIDI_CCHANGE;
		((DMUS_MIDI_PMSG*)*ppPMsg)->bByte1				= 10;			// pan
		((DMUS_MIDI_PMSG*)*ppPMsg)->bByte2				= 0;			// left
		((DMUS_MIDI_PMSG*)*ppPMsg)->bPad[0]				= 0;
		break;
	case DMUS_PMSGT_NOTE:
		//DMUS_NOTE_PMSG
		((DMUS_NOTE_PMSG*)*ppPMsg)->mtDuration			= 768 * 8;		// duration
		((DMUS_NOTE_PMSG*)*ppPMsg)->wMusicValue			= 0;			// Description of note in chord and key
		((DMUS_NOTE_PMSG*)*ppPMsg)->wMeasure			= 0;			// Measure in which this note occurs
		((DMUS_NOTE_PMSG*)*ppPMsg)->nOffset				= 0;			// Offset from grid at which this note occurs
		((DMUS_NOTE_PMSG*)*ppPMsg)->bBeat				= 0;			// Beat (in measure) at which this note occurs
		((DMUS_NOTE_PMSG*)*ppPMsg)->bGrid				= 0;			// Grid offset from beat at which this note occurs
		((DMUS_NOTE_PMSG*)*ppPMsg)->bVelocity			= 127;			// Note velocity
		((DMUS_NOTE_PMSG*)*ppPMsg)->bFlags				= DMUS_NOTEF_NOTEON;	// see DMUS_NOTEF_FLAGS
		((DMUS_NOTE_PMSG*)*ppPMsg)->bTimeRange			= 0;			// Range to randomize time
		((DMUS_NOTE_PMSG*)*ppPMsg)->bDurRange			= 0;			// Range to randomize duration
		((DMUS_NOTE_PMSG*)*ppPMsg)->bVelRange			= 0;			// Range to randomize velocity
		((DMUS_NOTE_PMSG*)*ppPMsg)->bPlayModeFlags		= DMUS_PLAYMODE_FIXED;	// Play mode
		((DMUS_NOTE_PMSG*)*ppPMsg)->bSubChordLevel		= 0;			// Which subchord level this note uses
		((DMUS_NOTE_PMSG*)*ppPMsg)->bMidiValue			= 36;			// The MIDI note value, converted from wMusicValue
		((DMUS_NOTE_PMSG*)*ppPMsg)->cTranspose			= 0;			// Transposition to add to midi note value after converted from wMusicValue
		break;
	case DMUS_PMSGT_NOTIFICATION:
		//DMUS_NOTIFICATION_PMSG
		//GUID    guidNotificationType;
		//DWORD   dwNotificationOption;
		//DWORD   dwField1;
		//DWORD   dwField2;
		break;
	case DMUS_PMSGT_PATCH:
		//DMUS_PATCH_PMSG
		((DMUS_PATCH_PMSG*)*ppPMsg)->byInstrument		= 1;
		((DMUS_PATCH_PMSG*)*ppPMsg)->byMSB				= 0;
		((DMUS_PATCH_PMSG*)*ppPMsg)->byLSB				= 0;
		((DMUS_PATCH_PMSG*)*ppPMsg)->byPad[0]			= 0;
		break;
	case DMUS_PMSGT_SYSEX:
		//DMUS_SYSEX_PMSG
		//DWORD   dwLen;			// length of the data
		//BYTE    abData[1];		// array of data, length equal to dwLen
		break;
	case DMUS_PMSGT_TEMPO:
		//DMUS_TEMPO_PMSG
		((DMUS_TEMPO_PMSG*)*ppPMsg)->dblTempo			= 60;			// the tempo
		break;
	case DMUS_PMSGT_TIMESIG:
		//DMUS_TIMESIG_PMSG
	    //BYTE    bBeatsPerMeasure;	// beats per measure (top of time sig)
	    //BYTE    bBeat;			// what note receives the beat (bottom of time sig)
			                        // we can assume that 0 means 256th note
		//WORD    wGridsPerBeat;    // grids per beat
		break;
	case DMUS_PMSGT_TRANSPOSE:
		//DMUS_TRANSPOSE_PMSG 
		((DMUS_TRANSPOSE_PMSG*)*ppPMsg)->nTranspose		= -1;			// one half step down
	    //((DMUS_TRANSPOSE_PMSG*)*ppPMsg)->wMergeIndex;					// Allows multiple parameters to be merged (pitchbend, volume, and expression.)
		break;
	case DMUS_PMSGT_CHANNEL_PRIORITY:
		//DMUS_CHANNEL_PRIORITY_PMSG 
	    //DWORD   dwChannelPriority;
		break;
	}

	// fail from here on out
	dwRes = FNS_FAIL;
	// **********************************************************************
	// 13) Performance->ClonePMsg
	// **********************************************************************
	hr = pwPerformance->ClonePMsg((*ppPMsg),ppPMsgClone);
	if(FAILED(hr) || !(*ppPMsgClone))
	{
		fnsLog(BUGLOGLEVEL,TEXT("!!!! BUG: IDMPerformance::ClonePMsg failed (%s == %08Xh)"),
			tdmXlatHRESULT(hr), hr);
		goto TEST_END;
	}
	// **********************************************************************
	// 14) Compare PMsgs
	// **********************************************************************
	hr = pTake->CompareTwoPMsgs((*ppPMsg),(*ppPMsgClone));
	if(FAILED(hr) || (hr == S_FALSE))
	{
		fnsLog(BUGLOGLEVEL,TEXT("!!!! BUG: CRecTake::CompareTwoPMsgs failed (%s == %08Xh)"),
			tdmXlatHRESULT(hr), hr);
		goto TEST_END;
	}
	// **********************************************************************
	// 15) Change cloned pmsg
	// **********************************************************************
	switch(dwPMsgType)
	{
	case DMUS_PMSGT_CURVE:
//		((DMUS_CURVE_PMSG*)*ppPMsgClone)->nStartValue		= 127;			// ignored
		((DMUS_CURVE_PMSG*)*ppPMsgClone)->nEndValue			= 0;		// fade out.
		break;
	case DMUS_PMSGT_MIDI:
		((DMUS_MIDI_PMSG*)*ppPMsgClone)->bByte2				= 127;		// higher
		break;
	case DMUS_PMSGT_NOTE:
		((DMUS_NOTE_PMSG*)*ppPMsgClone)->mtDuration			/= 8;		// shorter
		((DMUS_NOTE_PMSG*)*ppPMsgClone)->bMidiValue			+= 1;		// upa half step
		break;
	case DMUS_PMSGT_NOTIFICATION:
		break;
	case DMUS_PMSGT_PATCH:
		((DMUS_PATCH_PMSG*)*ppPMsgClone)->byInstrument		= 0;
		break;
	case DMUS_PMSGT_SYSEX:
		break;
	case DMUS_PMSGT_TEMPO:
		((DMUS_TEMPO_PMSG*)*ppPMsgClone)->dblTempo			= 200;		// the tempo
		break;
	case DMUS_PMSGT_TIMESIG:
		break;
	case DMUS_PMSGT_TRANSPOSE:
		((DMUS_TRANSPOSE_PMSG*)*ppPMsgClone)->nTranspose	= 2;		// two steps up
		break;
	case DMUS_PMSGT_CHANNEL_PRIORITY:
		break;
	}
	// **********************************************************************
	// 16) Performance->PlaySegmentEx
	// **********************************************************************
	hr = pwPerformance->PlaySegmentEx(pwSegment,0,NULL,DMUS_SEGF_BEAT,0,&pwSegState,NULL,pNewPath); 
	if(FAILED(hr))
	{
		fnsLog(BUGLOGLEVEL, TEXT("!!!! BUG: pwPerformance->PlaySegment ")
				TEXT("failed (%s == %08Xh)"),
				tdmXlatHRESULT(hr), hr);
		goto TEST_END;
	}
	// **********************************************************************
	// 17) Performance->GetLatencyTime
	// **********************************************************************
	hr = pwPerformance->GetLatencyTime(&rtTimeNow);
	if(FAILED(hr))
	{
		fnsLog(BUGLOGLEVEL,TEXT("!!!! BUG: Performance->GetLatencyTime failed (%s == %08Xh)"),
			tdmXlatHRESULT(hr), hr);
		goto TEST_END;
	}
	// **********************************************************************
	// 18) Performance->ReferenceToMusicTime
	// **********************************************************************
	hr = pwPerformance->ReferenceToMusicTime(rtTimeNow,&mtTimeNow);
	if(FAILED(hr))
	{
		fnsLog(BUGLOGLEVEL,TEXT("!!!! BUG: Performance->ReferenceToMusicTime failed (%s == %08Xh)"),
			tdmXlatHRESULT(hr), hr);
		goto TEST_END;
	}
	(*ppPMsg)->mtTime		= mtTimeNow; 				// at next measure boundary
	(*ppPMsgClone)->mtTime	= mtTimeNow + (768 * 4);	// 4 beats past the first pmsg.
	// **********************************************************************
	// 19) SegState->QueryInterface(IID_IDirectMusicGraph
	// **********************************************************************
	hr = pwSegState->QueryInterface(IID_IDirectMusicGraph,(void **) &pGraph);
	if(FAILED(hr) || !pGraph)
	{
		fnsLog(BUGLOGLEVEL,TEXT("!!!! BUG: IDMSegmentState::QueryInterface(IID_IDirectMusicGraph) failed (%s == %08Xh)"),
			tdmXlatHRESULT(hr), hr);
		goto TEST_END;
	}
	// **********************************************************************
	// 20) Graph->StampPMsgs
	// **********************************************************************
	hr = pGraph->StampPMsg(*ppPMsg);
	if(FAILED(hr))
	{
		fnsLog(BUGLOGLEVEL,TEXT("!!!! BUG: IDMToolGraph::StampPMsg(pPMsg) failed (%s == %08Xh)"),
			tdmXlatHRESULT(hr), hr);
		goto TEST_END;
	}
	hr = pGraph->StampPMsg(*ppPMsgClone);
	if(FAILED(hr))
	{
		fnsLog(BUGLOGLEVEL,TEXT("!!!! BUG: IDMToolGraph::StampPMsg(pPMsgClone) failed (%s == %08Xh)"),
			tdmXlatHRESULT(hr), hr);
		goto TEST_END;
	}
	// **********************************************************************
	// 21) Wait for IsPlaying to be true
	// **********************************************************************
	counter = 0;
	SleepTime = 100; // once every 100 mseconds
	iCheckPlaying = 300; // check for 30 seconds max
	while ((hr = pwPerformance->IsPlaying(pwSegment, NULL)) != S_OK)	
	{
		Sleep(SleepTime);
		if(counter++ > iCheckPlaying)
		{
			fnsLog(ABORTLOGLEVEL, TEXT("!!!! BUG: PlaySegment (first Segment) timed out")
				TEXT("IsPlaying did not return the Segment (%s == %08Xh)"),
				tdmXlatHRESULT(hr), hr);
			goto TEST_END;
		}
	}
	// **********************************************************************
	// 22) Performance->SendPMsg(both)
	// **********************************************************************
	hr = pwPerformance->SendPMsg(*ppPMsg);
	if(FAILED(hr))
	{
		pwPerformance->FreePMsg(*ppPMsg);
		fnsLog(BUGLOGLEVEL,TEXT("!!!! BUG: IDMPerformance::SendPMsg(Curve) failed (%s == %08Xh)"),
			tdmXlatHRESULT(hr), hr);
		goto TEST_END;
	}
	hr = pwPerformance->SendPMsg(*ppPMsgClone);
	if(FAILED(hr))
	{
		pwPerformance->FreePMsg(*ppPMsgClone);
		fnsLog(BUGLOGLEVEL,TEXT("!!!! BUG: IDMPerformance::SendPMsg(Clone) failed (%s == %08Xh)"),
			tdmXlatHRESULT(hr), hr);
		goto TEST_END;
	}
	// **********************************************************************
	// 23) Wait for IsPlaying to be false
	// **********************************************************************
	counter = 0;
	SleepTime = 1000; // once a second
	iCheckPlaying = 30; // check for 30 seconds max
	while ((hr = pwPerformance->IsPlaying(pwSegment, NULL)) == S_OK)	
	{
		Sleep(SleepTime);
		if(counter++ > iCheckPlaying)
		{
			fnsLog(BUGLOGLEVEL,TEXT("!!!! BUG: IDMPerformance::IsPlaying did not return false (%s == %08Xh)"),
				tdmXlatHRESULT(hr), hr);
			goto TEST_END;
		}
	}
	// we got this far we must have passed
	dwRes = FNS_PASS;

TEST_END:
	// **********************************************************************
	// 24) Cleanup objects and uninitialize and Return results
	// *********************************************************************
	if(pPerformance && pwSegment)
	{
		pwSegment->SetParam(GUID_Unload, -1, 0, 0, (void*)pPerformance); //don't care about the return code at this point
	}
	if(pwPerformance)
	{
		pwPerformance->CloseDown();
		pwPerformance->Release();
		pwPerformance = NULL;
	}

	delete pTake;

	if(pGraph)
	{
		pGraph->Release();
		pGraph = NULL;
	}
	if(pwGraph)
	{
		pwGraph->Release();
		pwGraph = NULL;
	}
	if(pNewPath)
	{
		pNewPath->Release();
		pNewPath = NULL;
	}
	if(pwSegState)
	{
		pwSegState->Release();
		pwSegState = NULL;
	}
	if(pwSegment)
	{
		pwSegment->Release();
		pwSegment = NULL;
	}
	if(pPerformance)
	{
		pPerformance->Release();
		pPerformance = NULL;
	}
	if(pwLoader)
	{
		pwLoader->Release();
		pwLoader = NULL;
	}

	dmthCoUninitialize();	
	return dwRes;
}




//--------------------------------------------------------------------------;
// @doc IDIRECTMUSICPERFORMANCE8 KCRAVEN
//
// @topic IDirectMusicPerformance8::ClonePMsg()_Valid_Test |
//
// Description: <nl>
// Valid test function for IDirectMusicPerformance8::ClonePMsg()<nl>
//
// Test Function: <nl>
// tdmperv8AllocPMsgChannelGroup()<nl>
//
// Parameters: <nl>
//	BOOL	fCoCreate	- unused <nl>
//	BOOL	fMaxDebug	- unused <nl>
//	DWORD	dwParam1	- unused <nl>
//	DWORD	dwParam2	- unused <nl>
//
// File : tdmperv8.cpp<nl>
//
// Steps: <nl>
//  1) CoInitialize<nl>
//  2) CoCreatePerformance <nl>
//  3) CoCreateLoader<nl>
//  4) Loader->GetObject(Segment)<nl>
//  5) Performance->InitAudio<nl>
//  6) Performance->CreateStandardAudioPath<nl>
//  7) Create Test Tool<nl>
//  8) CoCreateGraph<nl>
//  9) Performance->SetGraph<nl>
//  10) Segment->SetParam(GUID_Download)<nl>
//  11) Performance->AllocPMsg<nl>
//  12) Set PMsg values<nl>
//  13) Performance->ClonePMsg<nl>
//  14) Compare PMsgs<nl>
//  15) Change cloned pmsg<nl>
//  16) Performance->PlaySegmentEx<nl>
//  17) Performance->GetLatencyTime<nl>
//  18) Performance->ReferenceToMusicTime<nl>
//  19) SegState->QueryInterface(IID_IDirectMusicGraph<nl>
//  20) Graph->StampPMsgs<nl>
//  21) Wait for IsPlaying to be true<nl>
//  22) Performance->SendPMsg(both)<nl>
//  23) Wait for IsPlaying to be false<nl>
//  24) Cleanup objects and uninitialize and Return results<nl>
//
// History: <nl>
//	03/15/2000 - kcraven - created<nl>
//	04/18/2000 - kcraven - finished<nl>
//
// @index topic | IDIRECTMUSICPERFORMANCE8
//--------------------------------------------------------------------------;
DWORD tdmperv8AllocPMsgChannelGroup(
	BOOL fCoCreate,
	BOOL fMaxDebug,
	LPARAM dwParam1,
	LPARAM dwParam2)
{
	DWORD						dwRes			= FNS_ABORTED;
	HRESULT 					hr				= S_OK;
	CtIDirectMusicPerformance8* pwPerformance	= NULL;
	IDirectMusicPerformance8*	pPerformance	= NULL;
	CtIDirectMusicAudioPath*	pNewPath		= NULL;
	CtIDirectMusicLoader*		pwLoader		= NULL;
	CtIDirectMusicSegment*		pwSegment		= NULL;
	CtIDirectMusicSegmentState* pwSegState		= NULL;
	CtIDirectMusicGraph*		pwGraph 		= NULL;
	IDirectMusicGraph*			pGraph			= NULL;

	DWORD						dwPMsgFlags		= NULL;			// additional flags to set	
	DWORD						dwPChannel		= (DWORD)dwParam1; 
	DWORD						dwPMsgType		= (DWORD)dwParam2; //DMUS_PMSGT_NOTE;
	ULONG 						ulPMsgSize;

	// generic pointer
	DMUS_PMSG**					ppPMsg;

//	DMUS_PMSG*					pBase;
//	DMUS_CHANNEL_PRIORITY_PMSG*	pChanPri;
	DMUS_CURVE_PMSG*			pCurve;
	DMUS_MIDI_PMSG*				pMidi;
	DMUS_NOTE_PMSG*				pNote;
//	DMUS_NOTIFICATION_PMSG*		pNotify;
	DMUS_PATCH_PMSG*			pPatch;
//	DMUS_SYSEX_PMSG*			pSysex;
	DMUS_TEMPO_PMSG*			pTempo;
//	DMUS_TIMESIG_PMSG*			pTimeSig;
	DMUS_TRANSPOSE_PMSG*		pTrans;

	MUSIC_TIME					mtTimeNow;
	REFERENCE_TIME				rtTimeNow;

	DWORD						dwType			= DMUS_APATH_SHARED_STEREOPLUSREVERB;
	DWORD						dwPChannelCount = 16;

	WCHAR						pMidiFile[] 	= L"testdrum.mid";
	dmthSetPath(pMidiFile);
	WCHAR*						wchPath 		= gwszBasePath;
	WCHAR*						wchFile 		= gwszMediaPath;

	DMUS_OBJECTDESC 			desc; 

//	MUSIC_TIME					mtLength;
//	REFERENCE_TIME				rtLength;
//	REFERENCE_TIME				rtMeasure;
//	REFERENCE_TIME				rtTmp;

	// gets intialized below
	int 						counter;
	int 						SleepTime;
	int 						iCheckPlaying; 

	// **********************************************************************
	// 1) CoInitialize
	// **********************************************************************
	hr = dmthCoInitialize(NULL);
	if(FAILED(hr))
	{
		fnsLog(ABORTLOGLEVEL,
			TEXT("**** ABORT:  initialization failed! (%s == %08Xh)"),
			tdmXlatHRESULT(hr), hr);
		goto TEST_END;
	}
	// **********************************************************************
	// 2) CoCreatePerformance 
	// **********************************************************************
	hr = dmthCreatePerformance(IID_IDirectMusicPerformance8, &pwPerformance);
	if(FAILED(hr))
	{
		fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: IDirectMusicPerformance8 object creation "),
			TEXT("failed (%s == %08Xh)"),
			tdmXlatHRESULT(hr), hr);
		goto TEST_END;
	}
	// **********************************************************************
	// 3) CoCreateLoader
	// **********************************************************************
	hr = dmthCreateLoader(IID_IDirectMusicLoader, &pwLoader);
	if (FAILED(hr))
	{
		fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: CoCreate Loader object ")
				TEXT("failed (%s == %08Xh)"),
				tdmXlatHRESULT(hr), hr);
		goto TEST_END;
	}
	// **********************************************************************
	// 4) Loader->GetObject(Segment)
	// **********************************************************************
	ZeroMemory(&desc, sizeof(desc));
	desc.dwSize = sizeof(DMUS_OBJECTDESC);
	desc.guidClass = CLSID_DirectMusicSegment;
	desc.dwValidData |= (DMUS_OBJ_CLASS | DMUS_OBJ_FILENAME | DMUS_OBJ_FULLPATH);
	wcscpy(desc.wszFileName,wchFile);
	hr = pwLoader->GetObject(&desc,CTIID_IDirectMusicSegment,(void **)&pwSegment);
	if(FAILED(hr))
	{
		fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: Loader->GetObject(pwSegment) ")
				TEXT("failed (%s == %08Xh)"),
				tdmXlatHRESULT(hr), hr);
		goto TEST_END;
	}
	// **********************************************************************
	// 5) Performance->InitAudio
	// **********************************************************************
	hr = pwPerformance->InitAudio(NULL,NULL,ghWnd,0,0,0,NULL); // don't create and audiopath 
	if(FAILED(hr))
	{
		fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: IDirectMusicPerformance8->InitAudio ")
				TEXT("failed (%s == %08Xh)"),
				tdmXlatHRESULT(hr), hr);
		goto TEST_END;
	}
	// **********************************************************************
	// 6) Performance->CreateStandardAudioPath
	// **********************************************************************
	hr = pwPerformance->CreateStandardAudioPath(
		dwType, 			// Choice of standard audio path.
		dwPChannelCount,	// How many pchannels to create.
		TRUE,
		&pNewPath);
	if(FAILED(hr))
	{
		fnsLog(BUGLOGLEVEL, TEXT("**** ABORT: IDirectMusicPerformance8->CreateStandardAudioPath ")
				TEXT("failed (%s == %08Xh)"),
				tdmXlatHRESULT(hr), hr);
		goto TEST_END;
	}
	if(!pNewPath)
	{
		fnsLog(BUGLOGLEVEL, TEXT("**** ABORT: pNewPath is NULL"));
		goto TEST_END;
	}
	// **********************************************************************
	// 8) CoCreateGraph
	// **********************************************************************
	hr = dmthCreateGraph(IID_IDirectMusicGraph, &pwGraph);
	if(FAILED(hr))
	{
		fnsLog(ABORTLOGLEVEL, "**** ABORT: dmthCreateGraph "
				"failed (%s == %08Xh)", tdmXlatHRESULT(hr), hr);
		goto TEST_END;
	}
	// **********************************************************************
	// 9) Performance->SetGraph
	// **********************************************************************
	hr = pwPerformance->SetGraph(pwGraph);
	if(FAILED(hr))
	{
		fnsLog(ABORTLOGLEVEL, "**** ABORT: Performance->SetGraph "
				"failed (%s == %08Xh)", tdmXlatHRESULT(hr), hr);
		goto TEST_END;
	}
	// **********************************************************************
	// 10) Segment->SetParam(GUID_Download)
	// **********************************************************************	
	hr = pwPerformance->GetRealObjPtr(&pPerformance);
	if (FAILED( hr ))
	{
		fnsLog(ABORTLOGLEVEL, "**** ABORT: Failed to get Real Object Pointer(performance) "
				"(HRESULT: %s == %08Xh)", tdmXlatHRESULT(hr), hr);
		goto TEST_END;
	}
	hr = pwSegment->SetParam(GUID_Download, -1, 0, 0, (void*)pPerformance); // needs real object
	if(FAILED(hr))
	{
		fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: pwSegment->SetParam(Download) ")
				TEXT("failed (%s == %08Xh)"),
				tdmXlatHRESULT(hr), hr);
		goto TEST_END;
	}
	// **********************************************************************
	// 11) Performance->AllocPMsg
	// **********************************************************************
	if(dwPMsgType == NULL)
	{
		dwPMsgType = DMUS_PMSGT_NOTE;
	}
	switch(dwPMsgType)
	{
	case DMUS_PMSGT_CURVE:
		ulPMsgSize = sizeof(DMUS_CURVE_PMSG);
		ppPMsg = (DMUS_PMSG**)&pCurve;
		break;
	case DMUS_PMSGT_MIDI:
		ulPMsgSize = sizeof(DMUS_MIDI_PMSG);
		ppPMsg = (DMUS_PMSG**)&pMidi;
		break;
	case DMUS_PMSGT_NOTE:
		ulPMsgSize = sizeof(DMUS_NOTE_PMSG);
		ppPMsg = (DMUS_PMSG**)&pNote;
		break;
	case DMUS_PMSGT_PATCH:
		ulPMsgSize = sizeof(DMUS_PATCH_PMSG);
		ppPMsg = (DMUS_PMSG**)&pPatch;
		break;
	case DMUS_PMSGT_TEMPO:
		ulPMsgSize = sizeof(DMUS_TEMPO_PMSG);
		ppPMsg = (DMUS_PMSG**)&pTempo;
		break;
	case DMUS_PMSGT_TRANSPOSE:
		ulPMsgSize = sizeof(DMUS_TRANSPOSE_PMSG);
		ppPMsg = (DMUS_PMSG**)&pTrans;
		break;
	default:
		//not tested at the moment
		fnsLog(ABORTLOGLEVEL, TEXT("**** PENDING: PMsg type unsupported in this test case"));
		dwRes = FNS_PENDING;
		goto TEST_END;
	}

	// fail from here on out
	dwRes = FNS_FAIL;

	hr = pwPerformance->AllocPMsg(ulPMsgSize,(DMUS_PMSG**)ppPMsg);
	if(FAILED(hr) || !(*ppPMsg))
	{
		fnsLog(BUGLOGLEVEL,TEXT("**** ABORT: IDMPerformance::AllocPMsg failed (%s == %08Xh)"),
			tdmXlatHRESULT(hr), hr);
		goto TEST_END;
	}
	// **********************************************************************
	// 12) Set PMsg values
	// **********************************************************************
	/*
		// common to all pmsgs

			#define DMUS_PMSG_PART                                                                              
			DWORD               dwSize;
			REFERENCE_TIME      rtTime;             //  real time (in 100 nanosecond increments)
			MUSIC_TIME          mtTime;             //  music time
			DWORD               dwFlags;            //  various bits (see DMUS_PMSGF_FLAGS enumeration)
			DWORD               dwPChannel;         //  Performance Channel. The Performance can
													//  use this to determine the port/channel.
			DWORD               dwVirtualTrackID;   //  virtual track ID
			IDirectMusicTool*   pTool;              //  tool interface pointer
			IDirectMusicGraph*  pGraph;             //  tool graph interface pointer
			DWORD               dwType;             //  PMSG type (see DMUS_PMSGT_TYPES defines)
			DWORD               dwVoiceID;          //  unique voice id which allows synthesizers to
													//  identify a specific event. For DirectX 6.0,
													//  this field should always be 0.
			DWORD               dwGroupID;          //  Track group id
			IUnknown*           punkUser;           //  user com pointer, auto released upon PMSG free
	*/

	// these are the choices for dwPChannel
    // dwPChannel == DMUS_PCHANNEL_BROADCAST_AUDIOPATH,
    // dwPChannel == DMUS_PCHANNEL_BROADCAST_GROUPS,
    // dwPChannel == DMUS_PCHANNEL_BROADCAST_PERFORMANCE,
    // dwPChannel == DMUS_PCHANNEL_BROADCAST_SEGMENT,
	(*ppPMsg)->dwPChannel = dwPChannel;
	(*ppPMsg)->dwFlags = DMUS_PMSGF_MUSICTIME | DMUS_PMSGF_DX8 | dwPMsgFlags;
	(*ppPMsg)->dwVirtualTrackID = 0;
	(*ppPMsg)->dwGroupID = -1;
	(*ppPMsg)->dwType = dwPMsgType;

	switch(dwPMsgType)
	{
	case DMUS_PMSGT_CURVE:
		//DMUS_CURVE_PMSG
		((DMUS_CURVE_PMSG*)*ppPMsg)->mtDuration			= 768 * 4;		// Curve lasts four beats.
		((DMUS_CURVE_PMSG*)*ppPMsg)->mtResetDuration	= 0;			// No reset.
		((DMUS_CURVE_PMSG*)*ppPMsg)->nStartValue		= 0;			// ignored
		((DMUS_CURVE_PMSG*)*ppPMsg)->nEndValue			= 127;			// fade to full volume.
		((DMUS_CURVE_PMSG*)*ppPMsg)->nResetValue		= 0;			// No reset.
		((DMUS_CURVE_PMSG*)*ppPMsg)->bType				= DMUS_CURVET_CCCURVE;
		((DMUS_CURVE_PMSG*)*ppPMsg)->bCurveShape		= DMUS_CURVES_LINEAR;
		((DMUS_CURVE_PMSG*)*ppPMsg)->bCCData			= 7;			// MIDI Volume controller.
		((DMUS_CURVE_PMSG*)*ppPMsg)->bFlags				= DMUS_CURVE_START_FROM_CURRENT; 
		((DMUS_CURVE_PMSG*)*ppPMsg)->wParamType			= 0; 			// This is for NRPN and RPN only.
		((DMUS_CURVE_PMSG*)*ppPMsg)->wMergeIndex		= 1;			// This will be added to the regular volume, not override it.
		break;
	case DMUS_PMSGT_MIDI:
		//DMUS_MIDI_PMSG
		((DMUS_MIDI_PMSG*)*ppPMsg)->bStatus				= MIDI_CCHANGE;
		((DMUS_MIDI_PMSG*)*ppPMsg)->bByte1				= 10;			// pan
		((DMUS_MIDI_PMSG*)*ppPMsg)->bByte2				= 0;			// left
		((DMUS_MIDI_PMSG*)*ppPMsg)->bPad[0]				= 0;
		break;
	case DMUS_PMSGT_NOTE:
		//DMUS_NOTE_PMSG
		((DMUS_NOTE_PMSG*)*ppPMsg)->mtDuration			= 768 * 8;		// duration
		((DMUS_NOTE_PMSG*)*ppPMsg)->wMusicValue			= 0;			// Description of note in chord and key
		((DMUS_NOTE_PMSG*)*ppPMsg)->wMeasure			= 0;			// Measure in which this note occurs
		((DMUS_NOTE_PMSG*)*ppPMsg)->nOffset				= 0;			// Offset from grid at which this note occurs
		((DMUS_NOTE_PMSG*)*ppPMsg)->bBeat				= 0;			// Beat (in measure) at which this note occurs
		((DMUS_NOTE_PMSG*)*ppPMsg)->bGrid				= 0;			// Grid offset from beat at which this note occurs
		((DMUS_NOTE_PMSG*)*ppPMsg)->bVelocity			= 90;			// Note velocity
		((DMUS_NOTE_PMSG*)*ppPMsg)->bFlags				= DMUS_NOTEF_NOTEON;	// see DMUS_NOTEF_FLAGS
		((DMUS_NOTE_PMSG*)*ppPMsg)->bTimeRange			= 0;			// Range to randomize time
		((DMUS_NOTE_PMSG*)*ppPMsg)->bDurRange			= 0;			// Range to randomize duration
		((DMUS_NOTE_PMSG*)*ppPMsg)->bVelRange			= 0;			// Range to randomize velocity
		((DMUS_NOTE_PMSG*)*ppPMsg)->bPlayModeFlags		= DMUS_PLAYMODE_FIXED;	// Play mode
		((DMUS_NOTE_PMSG*)*ppPMsg)->bSubChordLevel		= 0;			// Which subchord level this note uses
		((DMUS_NOTE_PMSG*)*ppPMsg)->bMidiValue			= 36;			// The MIDI note value, converted from wMusicValue
		((DMUS_NOTE_PMSG*)*ppPMsg)->cTranspose			= 0;			// Transposition to add to midi note value after converted from wMusicValue
		break;
	case DMUS_PMSGT_PATCH:
		//DMUS_PATCH_PMSG
		((DMUS_PATCH_PMSG*)*ppPMsg)->byInstrument		= 1;
		((DMUS_PATCH_PMSG*)*ppPMsg)->byMSB				= 0;
		((DMUS_PATCH_PMSG*)*ppPMsg)->byLSB				= 0;
		((DMUS_PATCH_PMSG*)*ppPMsg)->byPad[0]			= 0;
		break;
	case DMUS_PMSGT_TEMPO:
		//DMUS_TEMPO_PMSG
		((DMUS_TEMPO_PMSG*)*ppPMsg)->dblTempo			= 60;			// the tempo
		break;
	case DMUS_PMSGT_TRANSPOSE:
		//DMUS_TRANSPOSE_PMSG 
		((DMUS_TRANSPOSE_PMSG*)*ppPMsg)->nTranspose		= -1;			// one half step down
		break;
	}
	// **********************************************************************
	// 16) Performance->PlaySegmentEx
	// **********************************************************************
	hr = pwPerformance->PlaySegmentEx(pwSegment,0,NULL,DMUS_SEGF_BEAT,0,&pwSegState,NULL,pNewPath); 
	if(FAILED(hr))
	{
		fnsLog(BUGLOGLEVEL, TEXT("!!!! BUG: pwPerformance->PlaySegment ")
				TEXT("failed (%s == %08Xh)"),
				tdmXlatHRESULT(hr), hr);
		goto TEST_END;
	}
	// **********************************************************************
	// 17) Performance->GetLatencyTime
	// **********************************************************************
	hr = pwPerformance->GetLatencyTime(&rtTimeNow);
	if(FAILED(hr))
	{
		fnsLog(BUGLOGLEVEL,TEXT("!!!! BUG: Performance->GetLatencyTime failed (%s == %08Xh)"),
			tdmXlatHRESULT(hr), hr);
		goto TEST_END;
	}
	// **********************************************************************
	// 18) Performance->ReferenceToMusicTime
	// **********************************************************************
	hr = pwPerformance->ReferenceToMusicTime(rtTimeNow,&mtTimeNow);
	if(FAILED(hr))
	{
		fnsLog(BUGLOGLEVEL,TEXT("!!!! BUG: Performance->ReferenceToMusicTime failed (%s == %08Xh)"),
			tdmXlatHRESULT(hr), hr);
		goto TEST_END;
	}
	(*ppPMsg)->mtTime		= mtTimeNow + (768 * 4); 				// at next measure boundary

	// **********************************************************************
	// 19) SegState->QueryInterface(IID_IDirectMusicGraph
	// **********************************************************************
	hr = pwSegState->QueryInterface(IID_IDirectMusicGraph,(void **) &pGraph);
	if(FAILED(hr) || !pGraph)
	{
		fnsLog(BUGLOGLEVEL,TEXT("!!!! BUG: IDMSegmentState::QueryInterface(IID_IDirectMusicGraph) failed (%s == %08Xh)"),
			tdmXlatHRESULT(hr), hr);
		goto TEST_END;
	}
	// **********************************************************************
	// 20) Graph->StampPMsgs
	// **********************************************************************
	hr = pGraph->StampPMsg(*ppPMsg);
	if(FAILED(hr))
	{
		fnsLog(BUGLOGLEVEL,TEXT("!!!! BUG: IDMToolGraph::StampPMsg(pPMsg) failed (%s == %08Xh)"),
			tdmXlatHRESULT(hr), hr);
		goto TEST_END;
	}
	// **********************************************************************
	// 21) Wait for IsPlaying to be true
	// **********************************************************************
	counter = 0;
	SleepTime = 100; // once every 100 mseconds
	iCheckPlaying = 300; // check for 30 seconds max
	while ((hr = pwPerformance->IsPlaying(pwSegment, NULL)) != S_OK)	
	{
		Sleep(SleepTime);
		if(counter++ > iCheckPlaying)
		{
			fnsLog(ABORTLOGLEVEL, TEXT("!!!! BUG: PlaySegment (first Segment) timed out")
				TEXT("IsPlaying did not return the Segment (%s == %08Xh)"),
				tdmXlatHRESULT(hr), hr);
			goto TEST_END;
		}
	}
	// **********************************************************************
	// 22) Performance->SendPMsg(both)
	// **********************************************************************
	hr = pwPerformance->SendPMsg(*ppPMsg);
	if(FAILED(hr))
	{
		pwPerformance->FreePMsg(*ppPMsg);
		fnsLog(BUGLOGLEVEL,TEXT("!!!! BUG: IDMPerformance::SendPMsg(Curve) failed (%s == %08Xh)"),
			tdmXlatHRESULT(hr), hr);
		goto TEST_END;
	}
	// **********************************************************************
	// 23) Wait for IsPlaying to be false
	// **********************************************************************
	counter = 0;
	SleepTime = 1000; // once a second
	iCheckPlaying = 30; // check for 30 seconds max
	while ((hr = pwPerformance->IsPlaying(pwSegment, NULL)) == S_OK)	
	{
		Sleep(SleepTime);
		if(counter++ > iCheckPlaying)
		{
			fnsLog(BUGLOGLEVEL,TEXT("!!!! BUG: IDMPerformance::IsPlaying did not return false (%s == %08Xh)"),
				tdmXlatHRESULT(hr), hr);
			goto TEST_END;
		}
	}
	// we got this far we must have passed
	dwRes = FNS_PASS;

TEST_END:
	// **********************************************************************
	// 24) Cleanup objects and uninitialize and Return results
	// *********************************************************************
	if(pPerformance && pwSegment)
	{
		pwSegment->SetParam(GUID_Unload, -1, 0, 0, (void*)pPerformance); //don't care about the return code at this point
	}
	if(pwPerformance)
	{
		pwPerformance->CloseDown();
		pwPerformance->Release();
		pwPerformance = NULL;
	}
	if(pGraph)
	{
		pGraph->Release();
		pGraph = NULL;
	}
	if(pwGraph)
	{
		pwGraph->Release();
		pwGraph = NULL;
	}
	if(pNewPath)
	{
		pNewPath->Release();
		pNewPath = NULL;
	}
	if(pwSegState)
	{
		pwSegState->Release();
		pwSegState = NULL;
	}
	if(pwSegment)
	{
		pwSegment->Release();
		pwSegment = NULL;
	}
	if(pPerformance)
	{
		pPerformance->Release();
		pPerformance = NULL;
	}
	if(pwLoader)
	{
		pwLoader->Release();
		pwLoader = NULL;
	}

	dmthCoUninitialize();	
	return dwRes;
}
//--------------------------------------------------------------------------;
// @doc IDIRECTMUSICPERFORMANCE8 KCRAVEN
//
// @topic IDirectMusicPerformance8::PlaySegmentEx()_Valid_Test |
//
// Description: <nl>
// Valid test function for IDirectMusicPerformance8::CloseDown()<nl>
//
// Test Function: <nl>
// tdmperv8CloseDownTwiceTest()<nl>
//
// Parameters: <nl>
//	BOOL	fCoCreate	- unused <nl>
//	BOOL	fMaxDebug	- unused <nl>
//	DWORD	dwParam1	- unused <nl>
//	DWORD	dwParam2	- unused <nl>
//
// File : tdmperv8.cpp<nl>
//
// Steps: <nl>
//	 1) Initialize<nl>
//	 2) Create Performance8<nl>
//	 3.0) Create Loader<nl>
//	 3.1) SetSearchDirectory<nl>
//	 3.2) Get the Segment<nl>
//	 4.0) Init the Performance<nl>
//	 4.1) Add the default Port<nl>
//	 4.2) Download<nl>
//	 5) Call PlaySegmentEx - the method to be tested<nl>
//	 6) varification - segment should play<nl>
//	 7) Cleanup objects and uninitialize and Return results<nl>
//
// History: <nl>
//	07/11/2000 - kcraven - created<nl>
//
// @index topic | IDIRECTMUSICPERFORMANCE8
//--------------------------------------------------------------------------;
DWORD tdmperv8CloseDownTwiceTest(
	BOOL fCoCreate,
	BOOL fMaxDebug,
	LPARAM dwParam1,
	LPARAM dwParam2)
{
	DWORD						dwRes			= FNS_ABORTED;
	HRESULT 					hr				= S_OK;

	WCHAR*						pszFile			= L"test.mid";
	dmthSetPath(NULL);
	WCHAR*						wchPath 		= gwszBasePath;
//	WCHAR*						wchFile 		= gwszMediaPath;

	CtIDirectMusicLoader8		*pwLoader		= NULL;
	CtIDirectMusicPerformance8	*pwPerformance	= NULL;
	IDirectMusicPerformance8	*pPerformance	= NULL;
	CtIDirectMusicSegment8		*pwSegment		= NULL;

	DMUS_OBJECTDESC 			desc; 

	int 						counter 		= 0;
	int 						SleepTime		= 3000;
	int 						TimeOutValue	= 10;

	DWORD						testtype		= (DWORD)dwParam2;

	switch(testtype)
	{
		case 1:
		case 2:
			break;
		default:
			return FNS_PENDING;
	}
	// **********************************************************************
	// 1) Initialize
	// **********************************************************************
	hr = dmthCoInitialize(NULL);
	if(FAILED(hr))
	{
		fnsLog(ABORTLOGLEVEL,
			TEXT("**** ABORT:  initialization failed! (%s == %08Xh)"),
			tdmXlatHRESULT(hr), hr);
		goto TEST_END;
	}
	// **********************************************************************
	// 2) Create Performance8
	// **********************************************************************
	hr = dmthCreatePerformance(IID_IDirectMusicPerformance8, &pwPerformance);
	if(FAILED(hr))
	{
		fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: IDirectMusicPerformance8 object creation "),
			TEXT("failed (%s == %08Xh)"),
			tdmXlatHRESULT(hr), hr);
		goto TEST_END;
	}
	// **********************************************************************
	// 3.0) Create Loader
	// **********************************************************************
	hr = dmthCreateLoader(IID_IDirectMusicLoader8, &pwLoader);
	if (FAILED(hr))
	{
		fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: CoCreate Loader object ")
				TEXT("failed (%s == %08Xh)"),
				tdmXlatHRESULT(hr), hr);
		goto TEST_END;
	}
	// **********************************************************************
	// 3.1) SetSearchDirectory
	// **********************************************************************
	hr = pwLoader->SetSearchDirectory(GUID_DirectMusicAllTypes, wchPath, FALSE);
	if (FAILED(hr))
	{
		fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: Loader->SetSeachDirectory(media directory) ")
				TEXT("failed (%s == %08Xh)"),
				tdmXlatHRESULT(hr), hr);
		goto TEST_END;
	}
	// **********************************************************************
	// 3.2) Get the Segment
	// **********************************************************************
	ZeroMemory(&desc, sizeof(desc));
	desc.dwSize = sizeof(DMUS_OBJECTDESC);
	desc.guidClass = CLSID_DirectMusicSegment;
	desc.dwValidData |= (DMUS_OBJ_CLASS | DMUS_OBJ_FILENAME);
	wcscpy(desc.wszFileName,pszFile);
	hr = pwLoader->GetObject(&desc,CTIID_IDirectMusicSegment8,(void **)&pwSegment);
	if(FAILED(hr))
	{
		fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: Loader->GetObject(pwSegment) ")
				TEXT("failed (%s == %08Xh)"),
				tdmXlatHRESULT(hr), hr);
		goto TEST_END;
	}
	// **********************************************************************
	// 4.0) Init the Performance
	// **********************************************************************
	switch(testtype)
	{
	case 1:
		hr = pwPerformance->Init(NULL,NULL,ghWnd);
		if(FAILED(hr))
		{
			fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: pwPerformance->Init(NULL) ")
						TEXT("failed (%s == %08Xh)"),
						tdmXlatHRESULT(hr), hr);
			goto TEST_END;
		}
		// **********************************************************************
		// 4.1) Add the Port
		// **********************************************************************	
		hr = pwPerformance->AddPort( NULL );
		if(FAILED(hr))
		{
			fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: pwPerformance->AddPort(NULL) ")
					TEXT("failed (%s == %08Xh)"),
					tdmXlatHRESULT(hr), hr);
			goto TEST_END;
		}
		break;
	case 2:
		hr = pwPerformance->InitAudio(NULL,NULL,ghWnd,DMUS_APATH_SHARED_STEREOPLUSREVERB,64,NULL,NULL);
		if(FAILED(hr))
		{
			fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: pwPerformance->InitAudio(DMUS_APATH_SHARED_STEREOPLUSREVERB) ")
						TEXT("failed (%s == %08Xh)"),
						tdmXlatHRESULT(hr), hr);
			goto TEST_END;
		}
		break;
	}
	// **********************************************************************
	// 4.2) Download
	// **********************************************************************	
	hr = pwSegment->Download(pwPerformance); 
	if(FAILED(hr))
	{
		fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: pwSegment->Download(pwPerformance) ")
				TEXT("failed (%s == %08Xh)"),
				tdmXlatHRESULT(hr), hr);
		goto TEST_END;
	}
	dwRes = FNS_FAIL;
	// **********************************************************************
	// 5) Call PlaySegmentEx - the method to be tested
	// **********************************************************************
	hr = pwPerformance->PlaySegmentEx( 
		pwSegment,			// Segment to play 
		0,NULL, 			// Optional stuff NULL. 
		DMUS_SEGF_BEAT, 	// DMUS_SEGF_ flags.
		0,NULL,NULL,NULL);	// Optional stuff is NULL. 
	if(FAILED(hr))
	{
		fnsLog(ABORTLOGLEVEL, TEXT("!!!! BUG: pwPerformance->PlaySegmentEx ")
				TEXT("failed (%s == %08Xh)"),
				tdmXlatHRESULT(hr), hr);
		goto TEST_END;
	}
	// **********************************************************************
	// 6) varification - segment should play
	// **********************************************************************
	counter = 0;
	SleepTime = 1000;
	while ((hr = pwPerformance->IsPlaying(pwSegment, NULL)) != S_OK)	
	{
		Sleep(SleepTime);
		if(counter++ > TimeOutValue)
		{
			fnsLog(BUGLOGLEVEL, TEXT("!!!! BUG: IsPlaying() timed out\n")
				TEXT("Segment did not start playing in the time specified (%s == %08Xh)"),
				tdmXlatHRESULT(hr), hr);
			goto TEST_END;
		}
	}
	Sleep(1000);
	// **********************************************************************
	// 4.2) CloseDown
	// **********************************************************************	
	hr = pwPerformance->CloseDown(); 
	if(FAILED(hr))
	{
		fnsLog(ABORTLOGLEVEL, TEXT("!!!! BUG: First pwPerformance->CloseDown() ")
				TEXT("failed (%s == %08Xh)"),
				tdmXlatHRESULT(hr), hr);
		goto TEST_END;
	}
	// **********************************************************************
	// 4.2) CloseDown
	// **********************************************************************	
	hr = pwPerformance->CloseDown(); 
	if(FAILED(hr))
	{
		fnsLog(ABORTLOGLEVEL, TEXT("!!!! BUG: Second pwPerformance->CloseDown() ")
				TEXT("failed (%s == %08Xh)"),
				tdmXlatHRESULT(hr), hr);
		goto TEST_END;
	}
	dwRes = FNS_PASS;

TEST_END:
	// *********************************************************************
	// 7) Cleanup objects and uninitialize and Return results
	// *********************************************************************
	if(pwSegment && pwPerformance)
	{
		pwSegment->Unload(pwPerformance);
	}
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

	dmthCoUninitialize();	
	return dwRes;
}	





//--------------------------------------------------------------------------;
// @doc IDIRECTMUSICPERFORMANCE8 KCRAVEN
//
// @topic IDirectMusicPerformance8::CreateAudioPath()_Valid_Test |
//
// Description: <nl>
// Valid test function for IDirectMusicPerformance8::CreateAudioPath(). <nl>
//
// Test Function: <nl>
// tdmperv8CreateAudioPath() <nl>
//
// Parameters: <nl>
//	BOOL	fCoCreate	- unused <nl>
//	BOOL	fMaxDebug	- unused <nl>
//	DWORD	dwParam1	- unused <nl>
//	DWORD	dwParam2	- unused <nl>
//
// File : tdmperv8.cpp <nl>
//
// Steps: <nl>
//  1) CoInitialize<nl>
//  2) CoCreateLoader<nl>
//  3) Loader8->SetSearchDirectory<nl>
//  4) Loader8->ScanDirectory<nl>
//  5) Loader8->GetObject(segment)<nl>
//  6) Segment8->GetAudioPathConfig<nl>
//  7) CoCreatePerformance<nl>
//  8) Performance8->InitAudio<nl>
//  9) Performance8->CreateAudioPath<nl>
//  10) AudioPath->GetObjectInPath(port)<nl>
//  11) Segment8->Download<nl>
//  12) Performance8->PlaySegmentEx<nl>
//  13) Wait for IsPlaying to be true<nl>
//  14) Cleanup objects and uninitialize and Return results<nl>
//
// History: <nl>
//	03/15/2000 - kcraven - created<nl>
//
// @index topic | IDIRECTMUSICPERFORMANCE8
//--------------------------------------------------------------------------;
DWORD tdmperv8CreateAudioPath(
	BOOL fCoCreate,
	BOOL fMaxDebug,
	LPARAM dwParam1,
	LPARAM dwParam2)
{
	DWORD						dwRes				= FNS_ABORTED;
	HRESULT 					hr					= S_OK;

	CtIDirectMusicPerformance8* pwPerformance8		= NULL;
	CtIDirectMusicLoader8*		pwLoader8			= NULL;
	CtIDirectMusicSegment8* 	pwSegment8			= NULL;
	CtIUnknown* 				pAudioPathConfig	= NULL;
	CtIDirectMusicAudioPath*	pAudioPath			= NULL;
	DMUS_OBJECTDESC 			desc; 
	DMUS_PORTCAPS				caps;

	int 						counter 		= 0;
	int 						SleepTime		= 3000;
	int 						TimeOutValue	= 10;

	DWORD						dwIndex 			= 0;
	IDirectMusicPort*			pDMPort 			= NULL;

	WCHAR	wszAudioPath[DMUS_MAX_FILENAME];
	WCHAR	wszSegment[DMUS_MAX_FILENAME];
	dmthSetPath(NULL);
	WCHAR	*wchPath	= gwszBasePath;
	WCHAR	*wchFile	= gwszMediaPath;
	BOOL	bGetAudPath	= FALSE;

	dwRes = FNS_ABORTED;

	if(dwParam1)
	{
		wcscpy(wszAudioPath,(WCHAR*)dwParam1); // special file
	}
	else
	{
		wcscpy(wszAudioPath,L"Standard.aud");	// default file
		bGetAudPath	= TRUE;	// attempt to get audiopathconfig from segment 
	}
	if(dwParam2)
	{
		wcscpy(wszSegment,(WCHAR*)dwParam2); // special file
	}
	else
	{
		wcscpy(wszSegment,L"test.mid"); // default file
	}

	// **********************************************************************
	// 1) CoInitialize
	// **********************************************************************
	hr = dmthCoInitialize(NULL);
	if(FAILED(hr))
	{
		fnsLog(ABORTLOGLEVEL,
			TEXT("**** ABORT:  initialization failed! (%s == %08Xh)"),
			tdmXlatHRESULT(hr), hr);
		goto TEST_END;
	}
	// **********************************************************************
	// 2) CoCreateLoader
	// **********************************************************************
	hr = dmthCreateLoader(IID_IDirectMusicLoader8, &pwLoader8);
	if(FAILED(hr))
	{
		fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: IDirectMusicLoader object creation "),
				TEXT("failed (%s == %08Xh)"),
				tdmXlatHRESULT(hr), hr);
		goto TEST_END;
	}
	// **********************************************************************
	// 3) Loader8->SetSearchDirectory
	// **********************************************************************
	hr = pwLoader8->SetSearchDirectory(GUID_DirectMusicAllTypes, wchPath, FALSE);
	if (FAILED(hr))
	{
		fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: Loader->SetSeachDirectory(media directory) ")
				TEXT("failed (%s == %08Xh)"),
				tdmXlatHRESULT(hr), hr);
		goto TEST_END;
	}
	// **********************************************************************
	// 4) Loader8->ScanDirectory
	// **********************************************************************
	hr = pwLoader8->ScanDirectory(CLSID_DirectMusicCollection, L"*", NULL);
	if (FAILED(hr))
	{
		fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: Loader->ScanDirectory(dls) ")
				TEXT("failed (%s == %08Xh)"),
				tdmXlatHRESULT(hr), hr);
		goto TEST_END;
	}
	hr = pwLoader8->ScanDirectory(CLSID_DirectMusicStyle, L"*", NULL);
	if (FAILED(hr))
	{
		fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: Loader->ScanDirectory(style) ")
				TEXT("failed (%s == %08Xh)"),
				tdmXlatHRESULT(hr), hr);
		goto TEST_END;
	}
	// **********************************************************************
	// 5) Loader8->GetObject(segment)
	// **********************************************************************
	ZeroMemory(&desc, sizeof(desc));
	desc.dwSize = sizeof(DMUS_OBJECTDESC);
	desc.guidClass = CLSID_DirectMusicSegment;
	desc.dwValidData |= (DMUS_OBJ_CLASS | DMUS_OBJ_FILENAME);
	wcscpy(desc.wszFileName,wszSegment);
	hr = pwLoader8->GetObject(&desc,CTIID_IDirectMusicSegment8,(void **)&pwSegment8);
	if(FAILED(hr))
	{
		fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: Loader8->GetObject(pwSegment8) ")
				TEXT("failed (%s == %08Xh)"),
				tdmXlatHRESULT(hr), hr);
		goto TEST_END;
	}
	// **********************************************************************
	// 6) Segment8->GetAudioPathConfig
	// **********************************************************************
	// try to get the audiopathconfig if we are using the default
	if(bGetAudPath)
	{
		// expected to fail if the segment has no audiopath config 
		hr = pwSegment8->GetAudioPathConfig(&pAudioPathConfig);
		 
		if(FAILED(hr) && (hr != DMUS_E_NO_AUDIOPATH_CONFIG))
		{
			fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: Segment8->GetAudioPathConfig(pAudioPathConfig) ")
					TEXT("failed (%s == %08Xh)"),
					tdmXlatHRESULT(hr), hr);
			goto TEST_END;
		}
	}
	// if there is no audiopathconfig in the segment use the default
	if((!bGetAudPath) || ((bGetAudPath) && FAILED(hr)))
	{
		ZeroMemory(&desc, sizeof(desc));
		desc.dwSize = sizeof(DMUS_OBJECTDESC);
		desc.guidClass = CLSID_DirectMusicAudioPathConfig;
		desc.dwValidData |= (DMUS_OBJ_CLASS | DMUS_OBJ_FILENAME);
		wcscpy(desc.wszFileName,wszAudioPath);

		hr = pwLoader8->GetObject(&desc,CTIID_IUnknown,(void **)&pAudioPathConfig);
		if(FAILED(hr))
		{
			fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: Loader8->GetObject(pAudioPathConfig) ")
					TEXT("failed (%s == %08Xh)"),
					tdmXlatHRESULT(hr), hr);
			goto TEST_END;
		}
	}
	// **********************************************************************
	// 7) CoCreatePerformance
	// **********************************************************************
	hr = dmthCreatePerformance(IID_IDirectMusicPerformance8, &pwPerformance8);
	if(FAILED(hr))
	{
		fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: IDirectMusicPerformance8 object creation "),
				TEXT("failed (%s == %08Xh)"),
				tdmXlatHRESULT(hr), hr);
		goto TEST_END;
	}
	// **********************************************************************
	// 8) Performance8->InitAudio
	// **********************************************************************
	hr = pwPerformance8->InitAudio(
		NULL,	// Can be NULL, pointer to NULL, or valid
		NULL,	// Can be NULL, pointer to NULL, or valid 
		ghWnd,	// HWND for initializing DSound.
		0,		// Requested standard audio path or 0.
		0,		// Number of PChannels, if default audio path to be created.
		0,		// Sample rate for all ports and DSound sink
		NULL);
	if(FAILED(hr))
	{
		fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: IDirectMusicPerformance8->InitAudio ")
				TEXT("failed (%s == %08Xh)"),
				tdmXlatHRESULT(hr), hr);
		goto TEST_END;
	}
	// fail from here on out
	dwRes = FNS_FAIL;
	// **********************************************************************
	// 9) Performance8->CreateAudioPath
	// **********************************************************************
	hr = pwPerformance8->CreateAudioPath(pAudioPathConfig,TRUE,&pAudioPath);
	if(FAILED(hr))
	{
		fnsLog(BUGLOGLEVEL, TEXT("!!!! BUG: IDirectMusicPerformance8->CreateAudioPath ")
				TEXT("failed (%s == %08Xh)"),
				tdmXlatHRESULT(hr), hr);
		goto TEST_END;
	}
	if(!pAudioPath)
	{
		fnsLog(BUGLOGLEVEL, TEXT("!!!! BUG: pAudioPath is NULL "));
		goto TEST_END;
	}
	// **********************************************************************
	// 10) AudioPath->GetObjectInPath(port)
	// **********************************************************************
	dwIndex = 0;
	while(S_OK == (hr = pAudioPath->GetObjectInPath(DMUS_PCHANNEL_ALL,DMUS_PATH_PORT,0,GUID_All_Objects,dwIndex,IID_IDirectMusicPort,(void**)&pDMPort)))
	{
		// Go on to the next port in the audio path
		dwIndex++;

		if(pDMPort)
		{
			fnsLog(BUGLOGLEVEL, TEXT("*** PORT FOUND: AudioPath::GetObjectInPath(%ld) succeeded (%s)"),dwIndex,tdmXlatHRESULT(hr));
			ZeroMemory(&caps, sizeof(DMUS_PORTCAPS));
			caps.dwSize = sizeof(DMUS_PORTCAPS);
			hr = pDMPort->GetCaps(&caps);
			if(FAILED(hr))
			{
				fnsLog(BUGLOGLEVEL, TEXT("!!!! BUG: IDirectMusicPort->GetCaps ")
					TEXT("failed (%s == %08Xh)"),
					tdmXlatHRESULT(hr), hr);
			}
			else
			{
				fnsIncrementIndent();
				tdmLogDMUS_PORTCAPS(STRUCTLOGLEVEL,&caps);
				fnsDecrementIndent();
			}
			// Release the port
			pDMPort->Release();
			pDMPort = NULL;
		}
		else
		{
			fnsLog(BUGLOGLEVEL, TEXT("*** PORT NOT FOUND: AudioPath::GetObjectInPath(%ld) succeeded (%s) but pPort is NULL"),dwIndex,tdmXlatHRESULT(hr));
		}

	}
	if(FAILED(hr) && (DMUS_E_NOT_FOUND != hr) || (dwIndex < 1))
	{
		fnsLog(BUGLOGLEVEL, TEXT("!!!! BUG: IDirectMusicPerformance8->GetObjectInPath ")
				TEXT("failed (%s == %08Xh)"),
				tdmXlatHRESULT(hr), hr);
		goto TEST_END;
	}
	// **********************************************************************
	// 11) Segment8->Download
	// **********************************************************************	
	hr = pwSegment8->Download(pwPerformance8);
	if(FAILED(hr))
	{
			fnsLog(BUGLOGLEVEL, TEXT("!!!! BUG: pwSegment8->Download ")
				TEXT("failed (%s == %08Xh)"),
				tdmXlatHRESULT(hr), hr);
		goto TEST_END;
	}
	// **********************************************************************
	// 12) Performance8->PlaySegmentEx
	// **********************************************************************	
	hr = pwPerformance8->PlaySegmentEx( 
		pwSegment8, 	// Segment to play. Alternately, could be an IDirectMusicSong. 
		0,				// If song, which segment in the song. 
		NULL,			// Optional template segment to compose transition with. 
		NULL,			// DMUS_SEGF_ flags.
		0,				// Time to start playback. 
		NULL,			// Returned Segment State. 
		NULL,			// Optional segmentstate or audiopath to replace.
		pAudioPath);	// Optional audioPath to play on. 
	if(FAILED(hr))
	{
		fnsLog(BUGLOGLEVEL, TEXT("!!!! BUG: pwPerformance8->PlaySegment ")
				TEXT("failed (%s == %08Xh)"),
				tdmXlatHRESULT(hr), hr);
		goto TEST_END;
	}
	// **********************************************************************
	// 13) Wait for IsPlaying to be true
	// **********************************************************************
	counter = 0;
	SleepTime = 1000;
	while ((hr = pwPerformance8->IsPlaying(pwSegment8, NULL)) != S_OK)	
	{
		Sleep(SleepTime);
		if(counter++ >TimeOutValue)
		{
			fnsLog(BUGLOGLEVEL, TEXT("!!!! BUG: IsPlaying() timed out\n")
				TEXT("Segment did not start playing in the time specified (%s == %08Xh)"),
				tdmXlatHRESULT(hr), hr);
			goto TEST_END;
		}
	}
	// we go to here - we passed
	dwRes = FNS_PASS;

TEST_END:
	// **********************************************************************
	// 14) Cleanup objects and uninitialize and Return results
	// *********************************************************************
	if(pwPerformance8 && pwSegment8)
	{
		pwSegment8->Unload(pwPerformance8); //we don't care if this fails at the moment
	}
	if(pwPerformance8)
	{
		pwPerformance8->CloseDown();
		pwPerformance8->Release();
		pwPerformance8 = NULL;
	}
	if(pwSegment8)
	{
		pwSegment8->Release();
		pwSegment8 = NULL;
	}
	if(pDMPort)
	{
		pDMPort->Release();
		pDMPort = NULL;
	}
	if(pwLoader8)
	{
		pwLoader8->Release();
		pwLoader8 = NULL;
	}
	if(pAudioPathConfig)
	{
		pAudioPathConfig->Release();
		pAudioPathConfig = NULL;
	}
	if(pAudioPath)
	{
		pAudioPath->Release();
		pAudioPath = NULL;
	}

	dmthCoUninitialize();	
	return dwRes;
}	





//--------------------------------------------------------------------------;
// @doc IDIRECTMUSICPERFORMANCE8 KCRAVEN
//
// @topic IDirectMusicPerformance8::CreateStandardAudioPath()_Valid_Test |
//
// Description: <nl>
// Valid test function for IDirectMusicPerformance8::CreateStandardAudioPath().<nl>
//
// Test Function: <nl>
// tdmperv8CreateStandardAudioPath()<nl>
//
// Parameters: <nl>
//	BOOL	fCoCreate	- unused <nl>
//	BOOL	fMaxDebug	- unused <nl>
//	DWORD	dwParam1	- unused <nl>
//	DWORD	dwParam2	- unused <nl>
//
// File : tdmperv8.cpp<nl>
//
// Steps: <nl>
//	 1) Initialize<nl>
//	 2) Create CtIDirectMusicPerformance8 object<nl> 
//	 3.0) Create CtIDirectMusicLoader object <nl>
//	 3.3) Get a Segment<nl>
//	 4) InitAudio the performance<nl>
//	 5) Call CreateStandardAudioPath - the function to be tested<nl>
//	 6.0.0) Verify the results - the audio path is no NULL<nl>
//	 6.1.0) Play the segment - Download the band<nl>
//	 6.1.1) PlaySegmentEx<nl>
//	 6.1.2) Wait for IsPlaying to return true (or time out)<nl>
//	 6.3) Verification - Get a known object (port) out of the audio path.<nl>
//	 6.4) Verification - Call QI for the same type object to Verify it's what we think it is<nl>
//	 7) Cleanup objects and uninitialize and Return results<nl>
//
// History: <nl>
//	03/15/2000 - kcraven - created<nl>
//
// @index topic | IDIRECTMUSICPERFORMANCE8
//--------------------------------------------------------------------------;
DWORD tdmperv8CreateStandardAudioPath(
	BOOL fCoCreate,
	BOOL fMaxDebug,
	LPARAM dwParam1,
	LPARAM dwParam2)
{
	DWORD						dwRes			= FNS_ABORTED;
	HRESULT 					hr				= S_OK;
	CtIDirectMusicPerformance8* pwPerformance8	= NULL;
	IDirectMusicPerformance8*	pPerformance	= NULL;
	CtIDirectMusicAudioPath*	pNewPath1		= NULL;
	CtIDirectMusicAudioPath*	pNewPath2		= NULL;
//	CtIDirectMusicAudioPath*	pNewPath3		= NULL;
	CtIDirectMusicAudioPath**	ppNewPath		= NULL;
	CtIDirectMusicLoader*		pwLoader		= NULL;
	CtIDirectMusicSegment*		pwSegment		= NULL;

	IUnknown*					pUnknown		= NULL;
	IUnknown*					pUnkQI			= NULL;
	DWORD						dwStage;
	DWORD						dwBuffer;
	GUID						guidObject;
	GUID						guidInterface;
//	int 						index			= 0;
	int 						getobj			= 1; // how may times to getobjectinpath	

	BOOL						doenv			= (BOOL)fCoCreate;
	DWORD						dwType;
	DWORD						dwType1			= (DWORD)dwParam1;
	DWORD						dwType2			= (DWORD)dwParam2; // if not NULL this path gets created first. It may share a port with the next path. 
	DWORD						dwPChannelCount = 16;

	WCHAR						pMidiFile[] 	= L"test.mid";
	dmthSetPath(pMidiFile);
	WCHAR*						wchPath 		= gwszBasePath;
	WCHAR*						wchFile 		= gwszMediaPath;

	DMUS_OBJECTDESC 			desc; 

	int 						counter 		= 0;
	int 						SleepTime		= 3000;
	int 						TimeOutValue	= 10;

	// **********************************************************************
	// 1) Initialize
	// **********************************************************************
	hr = dmthCoInitialize(NULL);
	if(FAILED(hr))
	{
		fnsLog(ABORTLOGLEVEL,
			TEXT("**** ABORT:  initialization failed! (%s == %08Xh)"),
			tdmXlatHRESULT(hr), hr);
		goto TEST_END;
	}
	// **********************************************************************
	// 2) Create CtIDirectMusicPerformance8 object 
	// **********************************************************************
	hr = dmthCreatePerformance(IID_IDirectMusicPerformance8, &pwPerformance8);
	if(FAILED(hr))
	{
		fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: IDirectMusicPerformance8 object creation "),
			TEXT("failed (%s == %08Xh)"),
			tdmXlatHRESULT(hr), hr);
		goto TEST_END;
	}
	// **********************************************************************
	// 3) Create CtIDirectMusicLoader object 
	// **********************************************************************
	hr = dmthCreateLoader(IID_IDirectMusicLoader, &pwLoader);
	if (FAILED(hr))
	{
		fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: CoCreate Loader object ")
				TEXT("failed (%s == %08Xh)"),
				tdmXlatHRESULT(hr), hr);
		goto TEST_END;
	}
	// **********************************************************************
	// 4) Get a Segment
	// **********************************************************************
	ZeroMemory(&desc, sizeof(desc));
	desc.dwSize = sizeof(DMUS_OBJECTDESC);
	desc.guidClass = CLSID_DirectMusicSegment;
	desc.dwValidData |= (DMUS_OBJ_CLASS | DMUS_OBJ_FILENAME | DMUS_OBJ_FULLPATH);
	wcscpy(desc.wszFileName,wchFile);
	hr = pwLoader->GetObject(&desc,CTIID_IDirectMusicSegment,(void **)&pwSegment);
//	fnsIncrementIndent();
//	fnsLog(FYILOGLEVEL, TEXT("**** FYI: Loader->GetObject(pwSegment) ")
//				TEXT("returned (%s == %08Xh)"),
//				tdmXlatHRESULT(hr), hr);
//	fnsDecrementIndent();
	if(FAILED(hr))
	{
		fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: Loader->GetObject(pwSegment) ")
				TEXT("failed (%s == %08Xh)"),
				tdmXlatHRESULT(hr), hr);
		goto TEST_END;
	}
	// **********************************************************************
	// 5) InitAudio the performance
	// **********************************************************************
	hr = pwPerformance8->InitAudio(NULL,NULL,ghWnd,0,0,0,NULL); // don't create an audiopath 
	if(FAILED(hr))
	{
		fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: IDirectMusicPerformance8->InitAudio ")
				TEXT("failed (%s == %08Xh)"),
				tdmXlatHRESULT(hr), hr);
		goto TEST_END;
	}

	// fail from no on
	dwRes = FNS_FAIL;

	// **********************************************************************
	// 8) Call CreateStandardAudioPath - the function to be tested
	// **********************************************************************
	hr = pwPerformance8->CreateStandardAudioPath(
		dwType1, 			// Choice of standard audio path.
		dwPChannelCount,	// How many pchannels to create.
		TRUE,
		&pNewPath1);
	if(FAILED(hr))

	{
		fnsLog(BUGLOGLEVEL, TEXT("!!!! BUG: IDirectMusicPerformance8->CreateStandardAudioPath ")
				TEXT("failed (%s == %08Xh)"),
				tdmXlatHRESULT(hr), hr);
		goto TEST_END;
	}
	// **********************************************************************
	// 9) Verify the results - the audio path is no NULL
	// **********************************************************************
	if(!pNewPath1)
	{
		fnsLog(BUGLOGLEVEL, TEXT("!!!! BUG: pNewPath1 is NULL"));
		goto TEST_END;
	}
	// **********************************************************************
	// 6) Call CreateStandardAudioPath - the function to be tested
	// **********************************************************************
	if(dwType2)
	{
		hr = pwPerformance8->CreateStandardAudioPath(
			dwType2, 			// Choice of standard audio path.
			dwPChannelCount,	// How many pchannels to create.
			TRUE,
			&pNewPath2);
		if(FAILED(hr))
		{
			fnsLog(BUGLOGLEVEL, TEXT("!!!! BUG: IDirectMusicPerformance8->CreateStandardAudioPath ")
					TEXT("failed (%s == %08Xh)"),
					tdmXlatHRESULT(hr), hr);
			goto TEST_END;
		}
	// **********************************************************************
	// 7) Verify the results - the audio path is no NULL
	// **********************************************************************
		if(!pNewPath2)
		{
			fnsLog(BUGLOGLEVEL, TEXT("!!!! BUG: pNewPath2 is NULL"));
			goto TEST_END;
		}
	}
	// **********************************************************************
	// 10) Call CreateStandardAudioPath - the function to be tested
	// **********************************************************************
	if(pNewPath2)
	{
		ppNewPath = &pNewPath2;
		dwType = dwType2;
		getobj++; // do getobjectinpath one more time
	}
	else
	{
		ppNewPath = &pNewPath1;
		dwType = dwType1;
	}
	// **********************************************************************
	// 11) Play the segment - Download the band
	// **********************************************************************	
	hr = pwPerformance8->GetRealObjPtr(&pPerformance);
	if (FAILED( hr ))
	{
		fnsLog(ABORTLOGLEVEL, "!!!! BUG: Failed to get Real Object Pointer(performance) "
				"(HRESULT: %s == %08Xh)", tdmXlatHRESULT(hr), hr);
		goto TEST_END;
	}
	hr = pwSegment->SetParam(GUID_Download, -1, 0, 0, (void*)pPerformance); // needs real object
	if(FAILED(hr))
	{
		fnsLog(ABORTLOGLEVEL, TEXT("!!!! BUG: pwSegment->SetParam(Download) ")
				TEXT("failed (%s == %08Xh)"),
				tdmXlatHRESULT(hr), hr);
		goto TEST_END;
	}
	// **********************************************************************
	// 12) PlaySegmentEx
	// **********************************************************************
	hr = pwPerformance8->PlaySegmentEx( 
		pwSegment,		// Segment to play. Alternately, could be an IDirectMusicSong. 
		0,				// If song, which segment in the song. 
		NULL,			// Optional template segment to compose transition with. 
		DMUS_SEGF_BEAT, // DMUS_SEGF_ flags.
		0,				// Time to start playback. 
		NULL,			// Returned Segment State. 
		NULL,			// Optional segmentstate or audiopath to replace.
		(*ppNewPath));		// Optional audioPath to play on. 

	// playback is expectedto fail on an env only path
/*DANDAN
	if(DMUS_APATH_GLOBALFX_ENV == dwType)
	{
		if(SUCCEEDED(hr))
		{
			fnsLog(BUGLOGLEVEL, TEXT("!!!! BUG: IDirectMusicPerformance8->PlaySegmentEx ")
					TEXT("should have but did not fail (%s == %08Xh)"),
					tdmXlatHRESULT(hr), hr);
			goto TEST_END;
		}
	}
	else
	{
*/
		if(FAILED(hr))
		{
			fnsLog(BUGLOGLEVEL, TEXT("!!!! BUG: pwPerformance->PlaySegment ")
					TEXT("failed (%s == %08Xh)"),
					tdmXlatHRESULT(hr), hr);
			goto TEST_END;
		}
	// **********************************************************************
	// 13) Wait for IsPlaying to return true (or time out)
	// **********************************************************************
		counter = 0;
		SleepTime = 1000;
		while ((hr = pwPerformance8->IsPlaying(pwSegment, NULL)) != S_OK)	
		{
			Sleep(SleepTime);
			if(counter++ >TimeOutValue)
			{
				fnsLog(BUGLOGLEVEL, TEXT("!!!! BUG: IsPlaying() timed out\n")
					TEXT("Segment did not start playing in the time specified (%s == %08Xh)"),
					tdmXlatHRESULT(hr), hr);
				goto TEST_END;
			}
		}
//	}
	// **********************************************************************
	// 14) Verification - Get a known object (port) out of the audio path.
	// **********************************************************************
	while((getobj--) > 0)
	{
		switch(dwType)
		{
		case DMUS_APATH_SHARED_STEREOPLUSREVERB:
			dwStage 		= DMUS_PATH_BUFFER;
			guidObject		= GUID_All_Objects; //GUID_Buffer_Reverb;
			guidInterface	= IID_IDirectSoundBuffer;
			dwBuffer		= 1;
			break;
		case DMUS_APATH_DYNAMIC_3D:
			dwStage 		= DMUS_PATH_BUFFER;
			guidObject		= GUID_All_Objects; //GUID_Buffer_3D;
			guidInterface	= IID_IDirectSound3DBuffer8;
			dwBuffer		= 0;
			break;
		case DMUS_APATH_DYNAMIC_MONO:
			dwStage 		= DMUS_PATH_BUFFER;
			guidObject		= GUID_All_Objects; //GUID_Buffer_Mono;
			guidInterface	= IID_IDirectSoundBuffer;
			dwBuffer		= 0;
			break;
		case DMUS_APATH_DYNAMIC_STEREO:
			dwStage 		= DMUS_PATH_BUFFER;
			guidObject		= GUID_All_Objects; //GUID_Buffer_Stereo;
			guidInterface	= IID_IDirectSoundBuffer;
			dwBuffer		= 0;
			break;
        default:
			fnsLog(ABORTLOGLEVEL, "TEST APP ERROR!!!  Kelly's the wave of the future, but he's using an invalid type");
			goto TEST_END;
            
		}
		hr = (*ppNewPath)->GetObjectInPath(
			DMUS_PCHANNEL_ALL,		//dwPChannel
			dwStage,				//dwStage
			dwBuffer,				//dwBuffer
			guidObject, 			//guidObject
			0, //index,				//dwIndex
			guidInterface,			//idInterface
			(void **)&pUnknown);	//pinterface
		if(FAILED(hr))
		{
			fnsLog(ABORTLOGLEVEL, TEXT("!!!! BUG: AudioPath->GetObjectInPath ")
					TEXT("failed (%s == %08Xh)"),
					tdmXlatHRESULT(hr), hr);
			goto TEST_END;
		}
		// **********************************************************************
		// 15) Verify the results - the audio path is no NULL
		// **********************************************************************
		if(!pUnknown)
		{
			fnsLog(BUGLOGLEVEL, TEXT("!!!! BUG: pUnknown is NULL"));
			goto TEST_END;
		}
		// **********************************************************************
		// 16) Verification - Call QI for the same type object to Verify it's what we think it is
		// **********************************************************************
		hr = pUnknown->QueryInterface(guidInterface, (void **)&pUnkQI);
		if(FAILED(hr))
		{
			fnsLog(ABORTLOGLEVEL, TEXT("!!!! BUG: IUnknown->QueryInterface ")
					TEXT("failed (%s == %08Xh)"),
					tdmXlatHRESULT(hr), hr);
			goto TEST_END;
		}

		// if we have path2 we have two paths
		// and we were using type2 the first time
		// so we set it back to type1 for the next time
		if(pNewPath2)
		{
			dwType = dwType1;
			ppNewPath = &pNewPath1;
		}

		// cleanup for the next time
		if(pUnknown)
		{
			pUnknown->Release();
			pUnknown = NULL;
		}
		if(pUnkQI)
		{
			pUnkQI->Release();
			pUnkQI = NULL;
		}
	}

	counter = 0;
	SleepTime = 1000;
	while ((hr = pwPerformance8->IsPlaying(pwSegment, NULL)) == S_OK)	
	{
		Sleep(SleepTime);
		if(counter++ >TimeOutValue)
		{
			fnsLog(BUGLOGLEVEL, TEXT("!!!! BUG: IsPlaying() timed out\n")
				TEXT("Segment did not stop playing in the time specified (%s == %08Xh)"),
				tdmXlatHRESULT(hr), hr);
			goto TEST_END;
		}
	}
	// we passed
	dwRes = FNS_PASS;

TEST_END:
	// **********************************************************************
	// 17) Cleanup objects and uninitialize and Return results
	// *********************************************************************
	if(pUnknown)
	{
		pUnknown->Release();
		pUnknown = NULL;
	}
	if(pUnkQI)
	{
		pUnkQI->Release();
		pUnkQI = NULL;
	}
	if(pPerformance && pwSegment)
	{
		pwSegment->SetParam(GUID_Unload, -1, 0, 0, (void*)pPerformance); //don't care about the return code at this point
	}
	if(pPerformance)
	{
		pPerformance->Release();
		pPerformance = NULL;
	}
	if(pwPerformance8)
	{
		pwPerformance8->CloseDown();
		pwPerformance8->Release();
		pwPerformance8 = NULL;
	}
	if(pNewPath1)
	{
		pNewPath1->Release();
		pNewPath1 = NULL;
	}
	if(pNewPath2)
	{
		pNewPath2->Release();
		pNewPath2 = NULL;
	}
/*DANDAN
	if(pNewPath3)
	{
		pNewPath3->Release();
		pNewPath3 = NULL;
	}
*/
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

	dmthCoUninitialize();	
	return dwRes;
}	





//--------------------------------------------------------------------------;
// @doc IDIRECTMUSICPERFORMANCE8 KCRAVEN
//
// @topic IDirectMusicPerformance8::GetDefaultAudioPath()_Valid_Test |
//
// Description: <nl>
// Valid test function for IDirectMusicPerformance8::GetDefaultAudioPath().<nl>
//
// Test Function: <nl>
// tdmperv8GetDefaultAudioPath()
//
// Parameters: <nl>
//	BOOL	fCoCreate	- unused <nl>
//	BOOL	fMaxDebug	- unused <nl>
//	DWORD	dwParam1	- unused <nl>
//	DWORD	dwParam2	- unused <nl>
//
// File : tdmperv8.cpp<nl>
//
// Steps: <nl>
//  1) CoInitialize<nl>
//  2) CoCreatePerformance <nl>
//  3) CoCreateLoader<nl>
//  4) Loader8->GetObject <nl>
//  5) Performance8->InitAudio<nl>
//  6) Performance8->CreateStandardAudioPath<nl>
//  7) Performance8->GetDefaultAudioPath<nl>
//  8) Segment8->Download<nl>
//  9) Performance8->PlaySegmentEx<nl>
//  10) Wait for IsPlaying to be true<nl>
//  11) Verify the audiopath<nl>
//  12) Wait for IsPlaying to be false<nl>
//  13) Cleanup objects and uninitialize and Return results<nl>
//
// History: <nl>
//	03/15/2000 - kcraven - created<nl>
//
// @index topic | IDIRECTMUSICPERFORMANCE8
//--------------------------------------------------------------------------;
DWORD tdmperv8GetDefaultAudioPath(
	BOOL fCoCreate,
	BOOL fMaxDebug,
	LPARAM dwParam1,
	LPARAM dwParam2)
{
	DWORD						dwRes				= FNS_ABORTED;
	HRESULT 					hr					= S_OK;

	CtIDirectMusicPerformance8* pwPerformance8		= NULL;
	CtIDirectMusicAudioPath*	pAudioPath			= NULL;
	CtIDirectMusicAudioPath*	pAudioPath2			= NULL;
	CtIDirectMusicLoader8*		pwLoader8			= NULL;
	CtIDirectMusicSegment8* 	pwSegment8			= NULL;
	DMUS_OBJECTDESC 			desc; 
	DWORD						dwDefaultPathType	= dwParam1;
	DWORD						dwType2				= dwParam2;
	DWORD						dwPChannelCount 	= 64;

	int 						counter 		= 0;
	int 						SleepTime		= 3000;
	int 						TimeOutValue	= 10;

	dmthSetPath(L"test.mid");
//	WCHAR	*wchPath	= gwszBasePath;
	WCHAR	*wchFile	= gwszMediaPath;

	PATH_DEFINITION*		ppPathDef = NULL;		

	switch(dwParam1)
	{
	case DMUS_APATH_SHARED_STEREOPLUSREVERB:
		ppPathDef = &g_DefaultPathStereoPlusReverb;
		break;
	case DMUS_APATH_DYNAMIC_3D:
		break;
	case DMUS_APATH_DYNAMIC_MONO:
		break;
	case DMUS_APATH_DYNAMIC_STEREO:
		break;
	default:
		//Note: 
        fnsLog(ABORTLOGLEVEL, "TEST APP ERROR: tdmperv8GetDefaultAudioPath can't use this type of path!!");
		goto TEST_END;
		break;
	}

	// **********************************************************************
	// 1) CoInitialize
	// **********************************************************************
	hr = dmthCoInitialize(NULL);
	if(FAILED(hr))
	{
		fnsLog(ABORTLOGLEVEL,
			TEXT("**** ABORT:  initialization failed! (%s == %08Xh)"),
			tdmXlatHRESULT(hr), hr);
		goto TEST_END;
	}
	// **********************************************************************
	// 2) CoCreatePerformance 
	// **********************************************************************
	hr = dmthCreatePerformance(IID_IDirectMusicPerformance8, &pwPerformance8);
	if(FAILED(hr))
	{
		fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: IDirectMusicPerformance8 object creation "),
				TEXT("failed (%s == %08Xh)"),
				tdmXlatHRESULT(hr), hr);
		goto TEST_END;
	}
	// **********************************************************************
	// 3) CoCreateLoader
	// **********************************************************************
	hr = dmthCreateLoader(IID_IDirectMusicLoader8, &pwLoader8);
	if(FAILED(hr))
	{
		fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: IDirectMusicLoader8 object creation "),
				TEXT("failed (%s == %08Xh)"),
				tdmXlatHRESULT(hr), hr);
		goto TEST_END;
	}
	// **********************************************************************
	// 4) Loader8->GetObject 
	// **********************************************************************
	ZeroMemory(&desc, sizeof(desc));
	desc.dwSize = sizeof(DMUS_OBJECTDESC);
	desc.guidClass = CLSID_DirectMusicSegment;
	desc.dwValidData |= (DMUS_OBJ_CLASS | DMUS_OBJ_FILENAME | DMUS_OBJ_FULLPATH);
	wcscpy(desc.wszFileName,wchFile);
	hr = pwLoader8->GetObject(&desc,CTIID_IDirectMusicSegment8,(void **)&pwSegment8);
	if(FAILED(hr))
	{
		fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: Loader8->GetObject(pwSegment8) ")
				TEXT("failed (%s == %08Xh)"),
				tdmXlatHRESULT(hr), hr);
		goto TEST_END;
	}
	// **********************************************************************
	// 5) Performance8->InitAudio
	// **********************************************************************
	hr = pwPerformance8->InitAudio(
		NULL,NULL,ghWnd,
		dwDefaultPathType,	// Requested standard audio path or 0.
		dwPChannelCount,	// Number of PChannels, if default audio path to be created.
		0,					// flags is no params
		NULL); //&audparams);		// audparams structure
	if(FAILED(hr))
	{
		fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: IDirectMusicPerformance8->InitAudio ")
				TEXT("failed (%s == %08Xh)"),
				tdmXlatHRESULT(hr), hr);
		goto TEST_END;
	}
	// **********************************************************************
	// 6) Performance8->CreateStandardAudioPath
	// **********************************************************************
	if(dwType2)
	{
		//this trick doesn't work to get sound when using ENV because it MUST be created before the ENV path
		hr = pwPerformance8->CreateStandardAudioPath(
			dwType2, 			// Choice of standard audio path.
			dwPChannelCount,	// How many pchannels to create.
			TRUE,
			&pAudioPath2);
		if(FAILED(hr))
		{
			fnsLog(BUGLOGLEVEL, TEXT("**** ABORT: IDirectMusicPerformance8->CreateStandardAudioPath ")
					TEXT("failed (%s == %08Xh)"),
					tdmXlatHRESULT(hr), hr);
			goto TEST_END;
		}
		if(!pAudioPath2)
		{
			fnsLog(BUGLOGLEVEL, TEXT("**** ABORT: pAudioPath2 is NULL"));
			goto TEST_END;
		}
	}

	// fail from here on our
	dwRes = FNS_FAIL;
	// **********************************************************************
	// 7) Performance8->GetDefaultAudioPath
	// **********************************************************************
	hr = pwPerformance8->GetDefaultAudioPath(&pAudioPath);
	if(FAILED(hr))
	{
		fnsLog(BUGLOGLEVEL, TEXT("!!!! BUG: IDirectMusicPerformance8->GetDefaultAudioPath ")
				TEXT("failed (%s == %08Xh)"),
				tdmXlatHRESULT(hr), hr);
		goto TEST_END;
	}
	if(!pAudioPath)
	{
		fnsLog(BUGLOGLEVEL, TEXT("!!!! BUG: pAudioPath is NULL "));
		goto TEST_END;
	}
	// **********************************************************************
	// 8) Segment8->Download
	// **********************************************************************	
	hr = pwSegment8->Download(pwPerformance8);
	if(FAILED(hr))
	{
		fnsLog(BUGLOGLEVEL, TEXT("!!!! BUG: pwSegment8->Download ")
			TEXT("failed (%s == %08Xh)"),
			tdmXlatHRESULT(hr), hr);
		goto TEST_END;
	}
	// **********************************************************************
	// 9) Performance8->PlaySegmentEx
	// **********************************************************************	
	hr = pwPerformance8->PlaySegmentEx( 
		pwSegment8, 				// Segment to play. Alternately, could be an IDirectMusicSong. 
		0,NULL,NULL,0,NULL,NULL,	// Optional stuff all NULL
		pAudioPath);				// Optional audioPath to play on. 
	if(FAILED(hr))
	{
		fnsLog(BUGLOGLEVEL, TEXT("!!!! BUG: pwPerformance8->PlaySegment ")
			TEXT("failed (%s == %08Xh)"),
			tdmXlatHRESULT(hr), hr);
		goto TEST_END;
	}
	// **********************************************************************
	// 10) Wait for IsPlaying to be true
	// **********************************************************************
	counter = 0;
	SleepTime = 1000;
	while ((hr = pwPerformance8->IsPlaying(pwSegment8, NULL)) != S_OK)	
	{
		Sleep(SleepTime);
		if(counter++ >TimeOutValue)
		{
			fnsLog(BUGLOGLEVEL, TEXT("!!!! BUG: IsPlaying() timed out\n")
				TEXT("Segment did not start playing in the time specified (%s == %08Xh)"),
				tdmXlatHRESULT(hr), hr);
			goto TEST_END;
		}
	}
	// **********************************************************************
	// 11) Verify the audiopath
	// **********************************************************************
	if(ppPathDef)
	{
		fnsIncrementIndent();
		fnsLog(ABORTLOGLEVEL,    TEXT(""));
		fnsLog(ABORTLOGLEVEL,    TEXT("----AudioPath Verification------------------------------------"));
		fnsIncrementIndent();
		hr = VerifyPathData(ppPathDef,pAudioPath);
		fnsDecrementIndent();
		fnsLog(ABORTLOGLEVEL,    TEXT("--------------------------------------------------------------"));
		fnsLog(ABORTLOGLEVEL,    TEXT(""));
		fnsDecrementIndent();
		if(FAILED(hr))
		{
			fnsLog(ABORTLOGLEVEL, TEXT("!!!! BUG:---Verification of DMUS_XXX_PATH_MUSIC defaults failed."),tdmXlatHRESULT(hr), hr);
			goto TEST_END;
		}
	}
	else
	{
			fnsLog(ABORTLOGLEVEL, TEXT("         ----Verification of DMUS_XXX_PATH_MUSIC not done."));
	}
	// **********************************************************************
	// 12) Wait for IsPlaying to be false
	// **********************************************************************
	counter = 0;
	SleepTime = 1000;
	while ((hr = pwPerformance8->IsPlaying(pwSegment8, NULL)) == S_OK)	
	{
		Sleep(SleepTime);
		if(counter++ >TimeOutValue)
		{
			fnsLog(BUGLOGLEVEL, TEXT("!!!! BUG: IsPlaying() timed out\n")
				TEXT("Segment did not stop playing in the time specified (%s == %08Xh)"),
				tdmXlatHRESULT(hr), hr);
			goto TEST_END;
		}
	}

	// pass if we got this far
	dwRes = FNS_PASS;

TEST_END:
	// **********************************************************************
	// 13) Cleanup objects and uninitialize and Return results
	// *********************************************************************
	if(pwPerformance8 && pwSegment8)
	{
		pwSegment8->Unload(pwPerformance8); //we don't care if this fails at the moment
	}
	if(pwPerformance8)
	{
		pwPerformance8->CloseDown();
		pwPerformance8->Release();
		pwPerformance8 = NULL;
	}
	if(pwSegment8)
	{
		pwSegment8->Release();
		pwSegment8 = NULL;
	}
	if(pwLoader8)
	{
		pwLoader8->Release();
		pwLoader8 = NULL;
	}
	if(pAudioPath)
	{
		pAudioPath->Release();
		pAudioPath = NULL;
	}
	if(pAudioPath2)
	{
		pAudioPath2->Release();
		pAudioPath2 = NULL;
	}

	dmthCoUninitialize();	
	return dwRes;
}




//--------------------------------------------------------------------------;
// @doc IDIRECTMUSICPERFORMANCE8 KCRAVEN
//
// @topic IDirectMusicPerformance8::GetParamEx()_Valid_Test |
//
// Description: <nl>
// Valid test function for IDirectMusicPerformance8::GetParamEx()<nl>
//
// Test Function: <nl>
// tdmperv8GetParamEx()<nl>
//
// Parameters: <nl>
//	BOOL	fCoCreate	- unused <nl>
//	BOOL	fMaxDebug	- unused <nl>
//	DWORD	dwParam1	- unused <nl>
//	DWORD	dwParam2	- unused <nl>
//
// File : tdmperv8.cpp<nl>
//
// Steps: <nl>
//  1) CoInitialize<nl>
//  2) CoCreatePerformance<nl>
//  3) CoCreateLoader<nl>
//  4) Loader->GetObject<nl>
//  5) Performance->InitAudio<nl>
//  6) Segment->SetParam(GUID_Download)<nl>
//  7) Performance->PlaySegmentEx<nl>
//  8) Wait for IsPlaying to be true<nl>
//  9) Performance->GetParamEx(GUID_TimeSignature)<nl>
//  10) Wait for IsPlaying to be false<nl>
//  11) Cleanup objects and uninitialize and Return results<nl>
//
// History: <nl>
//	07/06/2000 - kcraven - created<nl>
//
// @index topic | IDIRECTMUSICPERFORMANCE8
//--------------------------------------------------------------------------;
DWORD tdmperv8GetParamEx(
	BOOL boolParam1,
	BOOL boolParam2,
	LPARAM lparamParam3,
	LPARAM lparamParam4)
{
	DWORD						dwRes			= FNS_ABORTED;
	HRESULT 					hr				= S_OK;
	CtIDirectMusicPerformance8* pwPerformance	= NULL;
	CtIDirectMusicLoader8*		pwLoader		= NULL;
	CtIDirectMusicSegment8*		pwSegment		= NULL;
	CtIDirectMusicSegmentState* pwSegState		= NULL;

//	MUSIC_TIME mtTimeNow;
//	REFERENCE_TIME rtTimeNow;

	DWORD						dwType			= DMUS_APATH_SHARED_STEREOPLUSREVERB;
	DWORD						dwPChannelCount = 16;

	WCHAR*						wchPath 		= gwszBasePath;
	WCHAR*						wchFile 		= gwszMediaPath;

	DMUS_OBJECTDESC 			desc;
	MUSIC_TIME					mtTime;

	void*						pstruct;
	DMUS_PLAY_MARKER_PARAM		playmarker;
	DMUS_RHYTHM_PARAM			rhythm;
	DMUS_TIMESIGNATURE			timesig;
	DMUS_VALID_START_PARAM		starttime;
	DMUS_VARIATIONS_PARAM		variation;

	// gets intialized below
	int 						counter;
	int 						SleepTime;
	int 						iCheckPlaying; 

	GUID*						guid;
	DWORD						testtype		= (DWORD)lparamParam4;

	switch(testtype)
	{
	case 1: // GUID_Play_Marker
		dmthSetPath(L"markers.sgt");
		guid = (GUID*)&GUID_Play_Marker;
		ZeroMemory(&playmarker,sizeof(DMUS_PLAY_MARKER_PARAM));
		pstruct = (void*)&playmarker;
		break;
	case 2: // GUID_RhythmParam
		dmthSetPath(L"scalpatt.sgt");
		guid = (GUID*)&GUID_RhythmParam;
		ZeroMemory(&rhythm,sizeof(DMUS_RHYTHM_PARAM));
		ZeroMemory(&timesig,sizeof(DMUS_TIMESIGNATURE));
		pstruct = (void*)&rhythm;
		break;
	case 3: // GUID_TimeSignature
		dmthSetPath(L"test.mid");
		guid = (GUID*)&GUID_TimeSignature;
		ZeroMemory(&timesig,sizeof(DMUS_TIMESIGNATURE));
		pstruct = (void*)&timesig;
		break;
	case 4: // GUID_Valid_Start_Time
		dmthSetPath(L"markers.sgt");
		guid = (GUID*)&GUID_Valid_Start_Time;
		ZeroMemory(&starttime,sizeof(DMUS_VALID_START_PARAM));
		pstruct = (void*)&starttime;
		break;
	case 5: // GUID_Variations
		dmthSetPath(L"scalpatt.sgt");
		guid = (GUID*)&GUID_Variations;
		ZeroMemory(&variation,sizeof(DMUS_VARIATIONS_PARAM));
		pstruct = (void*)&variation;
		break;
	default:
		return FNS_PENDING;
	}

	// **********************************************************************
	// 1) CoInitialize
	// **********************************************************************
	hr = dmthCoInitialize(NULL);
	if(FAILED(hr))
	{
		fnsLog(ABORTLOGLEVEL,
			TEXT("**** ABORT:  initialization failed! (%s == %08Xh)"),
			tdmXlatHRESULT(hr), hr);
		goto TEST_END;
	}
	// **********************************************************************
	// 2) CoCreatePerformance
	// **********************************************************************
	hr = dmthCreatePerformance(IID_IDirectMusicPerformance8, &pwPerformance);
	if(FAILED(hr))
	{
		fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: IDirectMusicPerformance8 object creation "),
			TEXT("failed (%s == %08Xh)"),
			tdmXlatHRESULT(hr), hr);
		goto TEST_END;
	}
	// **********************************************************************
	// 3) CoCreateLoader
	// **********************************************************************
	hr = dmthCreateLoader(IID_IDirectMusicLoader8, &pwLoader);
	if (FAILED(hr))
	{
		fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: CoCreate Loader8 object ")
				TEXT("failed (%s == %08Xh)"),
				tdmXlatHRESULT(hr), hr);
		goto TEST_END;
	}
	// **********************************************************************
	// 4) Loader->GetObject
	// **********************************************************************
	ZeroMemory(&desc, sizeof(desc));
	desc.dwSize = sizeof(DMUS_OBJECTDESC);
	desc.guidClass = CLSID_DirectMusicSegment;
	desc.dwValidData |= (DMUS_OBJ_CLASS | DMUS_OBJ_FILENAME | DMUS_OBJ_FULLPATH);
	wcscpy(desc.wszFileName,wchFile);
	hr = pwLoader->GetObject(&desc,CTIID_IDirectMusicSegment8,(void **)&pwSegment);
	if(FAILED(hr))
	{
		fnsLog(ABORTLOGLEVEL, TEXT("**** Loader->GetObject(pwSegment) ")
				TEXT("failed (%s == %08Xh)"),
				tdmXlatHRESULT(hr), hr);
		fnsLog(ABORTLOGLEVEL, TEXT("Test case aborting"));
		goto TEST_END;
	}
	// **********************************************************************
	// 5) Performance->InitAudio
	// **********************************************************************
	hr = pwPerformance->InitAudio(NULL,NULL,ghWnd,dwType,dwPChannelCount,0,NULL); // create standard audiopath w/defaults 
	if(FAILED(hr))
	{
		fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: IDirectMusicPerformance8->InitAudio ")
				TEXT("failed (%s == %08Xh)"),
				tdmXlatHRESULT(hr), hr);
		goto TEST_END;
	}
	// **********************************************************************
	// 6) Segment->SetParam(GUID_Download)
	// **********************************************************************	
	hr = pwSegment->Download(pwPerformance); // needs real object
	if(FAILED(hr))
	{
		fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: pwSegment->Download(pwPerformance) ")
				TEXT("failed (%s == %08Xh)"),
				tdmXlatHRESULT(hr), hr);
		goto TEST_END;
	}
	// **********************************************************************
	// 7) Performance->PlaySegmentEx
	// **********************************************************************
	hr = pwPerformance->PlaySegmentEx( 
		pwSegment,		// Segment to play. Alternately, could be an IDirectMusicSong. 
		0,				// If song, which segment in the song. 
		NULL,			// Optional template segment to compose transition with. 
		DMUS_SEGF_BEAT, // DMUS_SEGF_ flags.
		0,				// Time to start playback. 
		&pwSegState,	// Returned Segment State. 
		NULL,			// Optional segmentstate or audiopath to replace.
		NULL);		// Optional audioPath to play on. 
	if(FAILED(hr))
	{
		fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: pwPerformance->PlaySegment ")
				TEXT("failed (%s == %08Xh)"),
				tdmXlatHRESULT(hr), hr);
		goto TEST_END;
	}
	// **********************************************************************
	// 8) Wait for IsPlaying to be true
	// **********************************************************************
	counter = 0;
	SleepTime = 20; // 20 msecs - very fast
	iCheckPlaying = 100; // check faster because we expect this be true almost immediately - time out after 2 seconds
	while ((hr = pwPerformance->IsPlaying(pwSegment, NULL)) != S_OK)	
	{
		Sleep(SleepTime);
		if(counter++ > iCheckPlaying)
		{
			fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: PlaySegment (first Segment) timed out")
				TEXT("IsPlaying did not return the Segment (%s == %08Xh)"),
				tdmXlatHRESULT(hr), hr);
			goto TEST_END;
		}
	}

	// fail from here on out
	dwRes = FNS_FAIL;
	// **********************************************************************
	// 9) Performance->GetParamEx(GUID_TimeSignature)
	// **********************************************************************
	hr = pwPerformance->GetTime(NULL,&mtTime);
	if(FAILED(hr))
	{
		fnsLog(ABORTLOGLEVEL, TEXT("!!!! BUG: pwPerformance->GetTime() ")
				TEXT("failed (%s == %08Xh)"),
				tdmXlatHRESULT(hr), hr);
		goto TEST_END;
	}
	if(testtype == 2) //we need to get the time signature
	{
		hr = pwPerformance->GetParamEx(GUID_TimeSignature,0,0xFFFFFFFF,DMUS_SEG_ANYTRACK,mtTime,NULL,&timesig);
		if(FAILED(hr))
		{
			fnsLog(ABORTLOGLEVEL, TEXT("!!!! BUG: pwPerformance->GetParamEx() ")
					TEXT("failed (%s == %08Xh)"),
					tdmXlatHRESULT(hr), hr);
			goto TEST_END;
		}
		rhythm.TimeSig.mtTime			= timesig.mtTime;
		rhythm.TimeSig.bBeatsPerMeasure	= timesig.bBeatsPerMeasure;
		rhythm.TimeSig.bBeat			= timesig.bBeat;
		rhythm.TimeSig.wGridsPerBeat	= timesig.wGridsPerBeat;
	}
	hr = pwPerformance->GetParamEx((*guid),0,0xFFFFFFFF,0,mtTime,NULL,pstruct);
//	hr = pwPerformance->GetParamEx((*guid),0,0xFFFFFFFF,DMUS_SEG_ANYTRACK,0,NULL,pstruct);
	if(FAILED(hr))
	{
		fnsLog(ABORTLOGLEVEL, TEXT("!!!! BUG: pwPerformance->GetParamEx() ")
				TEXT("failed (%s == %08Xh)"),
				tdmXlatHRESULT(hr), hr);
		goto TEST_END;
	}

	switch(testtype)
	{
	case 1: // GUID_Play_Marker
		fnsLog(FYILOGLEVEL, TEXT("        DMUS_PLAY_MARKER_PARAM   :"));
		fnsLog(FYILOGLEVEL, TEXT("        mtTime                   : %016X"), playmarker.mtTime);
		break;
	case 2: // GUID_RhythmParam
		pstruct = (void*)&rhythm;
		fnsLog(FYILOGLEVEL, TEXT("        DMUS_RHYTHM_PARAM        :"));
		fnsLog(FYILOGLEVEL, TEXT("        TimeSig.mtTime           : %016X"), rhythm.TimeSig.mtTime);
		fnsLog(FYILOGLEVEL, TEXT("        TimeSig.bBeatsPerMeasure : %d"), rhythm.TimeSig.bBeatsPerMeasure);
		fnsLog(FYILOGLEVEL, TEXT("        TimeSig.bBeat            : %d"), rhythm.TimeSig.bBeat);
		fnsLog(FYILOGLEVEL, TEXT("        TimeSig.wGridsPerBeat    : %d"), rhythm.TimeSig.wGridsPerBeat);
		fnsLog(FYILOGLEVEL, TEXT("        dwRhythmPattern          : %08X"), rhythm.dwRhythmPattern);
		break;
	case 3: // GUID_TimeSignature
		fnsLog(FYILOGLEVEL, TEXT("        DMUS_TIMESIGNATURE       :"));
		fnsLog(FYILOGLEVEL, TEXT("        mtTime                   : %016X"), timesig.mtTime);
		fnsLog(FYILOGLEVEL, TEXT("        bBeatsPerMeasure         : %d"), timesig.bBeatsPerMeasure);
		fnsLog(FYILOGLEVEL, TEXT("        bBeat                    : %d"), timesig.bBeat);
		fnsLog(FYILOGLEVEL, TEXT("        wGridsPerBeat            : %d"), timesig.wGridsPerBeat);
		break;
	case 4: // GUID_Valid_Start_Time
		fnsLog(FYILOGLEVEL, TEXT("        DMUS_VALID_START_PARAM   :"));
		fnsLog(FYILOGLEVEL, TEXT("        mtTime                   : %016X"), starttime.mtTime);
		break;
	case 5: // GUID_Variations
		fnsLog(FYILOGLEVEL, TEXT("        DMUS_VARIATIONS_PARAM    :"));
		fnsLog(FYILOGLEVEL, TEXT("        dwPChannelsUsed          : %08X"), variation.dwPChannelsUsed);
		fnsLog(FYILOGLEVEL, TEXT("        padwPChannels            : %ph"), variation.padwPChannels);
		fnsLog(FYILOGLEVEL, TEXT("        padwVariations           : %ph"), variation.padwVariations);
		pstruct = (void*)&variation;
		break;
	}
	// **********************************************************************
	// 10) Wait for IsPlaying to be false
	// **********************************************************************
	hr = pwPerformance->Stop(NULL,NULL,0,0);
	if(FAILED(hr))
	{
		fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: pwPerformance->Stop() ")
				TEXT("failed (%s == %08Xh)"),
				tdmXlatHRESULT(hr), hr);
		goto TEST_END;
	}
//	counter = 0;
//	SleepTime = 1000; // once a second
//	iCheckPlaying = 30; // check for 30 seconds max
//	while ((hr = pwPerformance->IsPlaying(pwSegment, NULL)) == S_OK)	
//	{
//		Sleep(SleepTime);
//		if(counter++ > iCheckPlaying)
//		{
//		fnsLog(ABORTLOGLEVEL, TEXT("!!!! FAILED: IDMPerformance::IsPlaying did not return false (%s == %08Xh)"),
//				tdmXlatHRESULT(hr), hr);
//			goto TEST_END;
//		}
//	}

	// we got this far we must have passed
	dwRes = FNS_PASS;

TEST_END:
	// **********************************************************************
	// 11) Cleanup objects and uninitialize and Return results
	// *********************************************************************
	if(pwPerformance && pwSegment)
//	if(pPerformance && pwSegment)
	{
		pwSegment->Unload(pwPerformance); //don't care about the return code at this point
//		pwSegment->SetParam(GUID_Unload, -1, 0, 0, (void*)pPerformance); //don't care about the return code at this point
	}
	if(pwPerformance)
	{
		pwPerformance->CloseDown();
		pwPerformance->Release();
		pwPerformance = NULL;
	}
	if(pwSegState)
	{
		pwSegState->Release();
		pwSegState = NULL;
	}
	if(pwSegment)
	{
		pwSegment->Release();
		pwSegment = NULL;
	}
//	if(pPerformance)
//	{
//		pPerformance->Release();
//		pPerformance = NULL;
//	}
	if(pwLoader)
	{
		pwLoader->Release();
		pwLoader = NULL;
	}

	dmthCoUninitialize();	
	return dwRes;
}




//--------------------------------------------------------------------------;
// @doc IDIRECTMUSICPERFORMANCE8 KCRAVEN
//
// @topic IDirectMusicPerformance8::InitAudio()_Valid_Test |
//
// Description: <nl>
// Valid test function for IDirectMusicPerformance8::InitAudio()<nl>
//
// Test Function: <nl>
// tdmperv8InitAudio()<nl>
//
// Parameters: <nl>
//	BOOL	fCoCreate	- unused <nl>
//	BOOL	fMaxDebug	- unused <nl>
//	DWORD	dwParam1	- unused <nl>
//	DWORD	dwParam2	- unused <nl>
//
// File : tdmperv8.cpp<nl>
//
// Steps: <nl>
//  1) CoInitialize<nl>
//  2) CoCreatePerformance <nl>
//  3) Performance8->InitAudio<nl>
//  4) Performance8->GetDefaultAudioPath<nl>
//  5) AudioPath->GetObjectInPath(buffer)<nl>
//  6) AudioPath->GetObjectInPath(port)<nl>
//  7) Port->GetFormat<nl>
//  8) Cleanup objects and uninitialize and Return results<nl>
//
// History: <nl>
//	03/15/2000 - kcraven - created<nl>
//
// @index topic | IDIRECTMUSICPERFORMANCE8
//--------------------------------------------------------------------------;
DWORD tdmperv8InitAudio(
	BOOL fCoCreate,
	BOOL fMaxDebug,
	LPARAM dwParam1,
	LPARAM dwParam2)
{
	DWORD						dwRes				= FNS_ABORTED;
	HRESULT 					hr					= S_OK;

	CtIDirectMusicPerformance8* pwPerformance8		= NULL;
	CtIDirectMusicAudioPath*	pAudioPath			= NULL;
	CtIDirectMusic* 			pwMusic 			= NULL;

	IUnknown*					pUnknown			= NULL;
	IUnknown*					pUnkQI				= NULL;

	BOOL						bCheckFormat		= (DWORD)fCoCreate;
	DWORD						dwDefaultPathType	= (DWORD)dwParam1;
	DWORD						dwPChannelCount 	= 16;
	DWORD						dwSampleRate		= (DWORD)dwParam2;
	DMUS_AUDIOPARAMS			audioparams;
	DMUS_AUDIOPARAMS*			paudioparams		= NULL;

	DWORD						dwStage;
	DWORD						dwBuffer;
	GUID						guidObject;
	GUID						guidInterface;
	DWORD						dwIndex 			= 0;
	IDirectMusicPort*			pDMPort 			= NULL;

	DMUS_PORTCAPS				caps;
    LPWAVEFORMATEX				pWfx				= NULL;
    DWORD						dwWfxSize			= 0;
    DWORD						dwBufferSize		= 0;


	// **********************************************************************
	// 0) Use AudioParams only if we pass the sample rate as a parameter
	// **********************************************************************
	if(dwSampleRate)
	{
		paudioparams = &audioparams;
		ZeroMemory(paudioparams,sizeof(DMUS_AUDIOPARAMS));
		paudioparams->dwSize = sizeof(DMUS_AUDIOPARAMS);
		paudioparams->dwValidData = DMUS_AUDIOPARAMS_SAMPLERATE;
		paudioparams->dwSampleRate = dwSampleRate;
	}
	// **********************************************************************
	// 1) CoInitialize
	// **********************************************************************
	hr = dmthCoInitialize(NULL);
	if(FAILED(hr))
	{
		fnsLog(ABORTLOGLEVEL,
			TEXT("**** ABORT:  initialization failed! (%s == %08Xh)"),
			tdmXlatHRESULT(hr), hr);
		goto TEST_END;
	}
	// **********************************************************************
	// 2) CoCreatePerformance 
	// **********************************************************************
	hr = dmthCreatePerformance(IID_IDirectMusicPerformance8, &pwPerformance8);
	if(FAILED(hr))
	{
		fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: IDirectMusicPerformance8 object creation "),
				TEXT("failed (%s == %08Xh)"),
				tdmXlatHRESULT(hr), hr);
		goto TEST_END;
	}
	dwRes = FNS_FAIL;
	// **********************************************************************
	// 3) Performance8->InitAudio
	// **********************************************************************
	hr = pwPerformance8->InitAudio(
		&pwMusic,NULL,ghWnd,
		dwDefaultPathType,	
		dwPChannelCount,	
		NULL,		
		paudioparams);
	if(FAILED(hr))
	{
		fnsLog(BUGLOGLEVEL, TEXT("!!!! BUG: IDirectMusicPerformance8->InitAudio ")
				TEXT("failed (%s == %08Xh)"),
				tdmXlatHRESULT(hr), hr);
		goto TEST_END;
	}
	if(!pwMusic)
	{
		fnsLog(BUGLOGLEVEL, TEXT("!!!! BUG: pwMusic is NULL "));
		goto TEST_END;
	}
	// **********************************************************************
	// 4) Performance8->GetDefaultAudioPath
	// **********************************************************************
	hr = pwPerformance8->GetDefaultAudioPath(&pAudioPath);
	if(FAILED(hr))
	{
		fnsLog(BUGLOGLEVEL, TEXT("!!!! BUG: IDirectMusicPerformance8->GetDefaultAudioPath ")
				TEXT("failed (%s == %08Xh)"),
				tdmXlatHRESULT(hr), hr);
		goto TEST_END;
	}
	if(!pAudioPath)
	{
		fnsLog(BUGLOGLEVEL, TEXT("!!!! BUG: pAudioPath is NULL "));
		goto TEST_END;
	}
	// get all the port there are - just for the heck of it - if the path is good this should not fail
	switch(dwDefaultPathType)
	{
	case DMUS_APATH_SHARED_STEREOPLUSREVERB:
		dwStage 		= DMUS_PATH_BUFFER;
		guidObject		= GUID_All_Objects; //GUID_Buffer_Reverb;
		guidInterface	= IID_IDirectSoundBuffer;
		dwBuffer		= 1;
		break;
	case DMUS_APATH_DYNAMIC_3D:
		dwStage 		= DMUS_PATH_BUFFER;
		guidObject		= GUID_All_Objects; //GUID_Buffer_3D;
		guidInterface	= IID_IDirectSoundBuffer;
		dwBuffer		= 0;
		break;
	case DMUS_APATH_DYNAMIC_MONO:
		dwStage 		= DMUS_PATH_BUFFER;
		guidObject		= GUID_All_Objects; //GUID_Buffer_Mono;
		guidInterface	= IID_IDirectSoundBuffer;
		dwBuffer		= 0;
		break;
	case DMUS_APATH_DYNAMIC_STEREO:
		dwStage 		= DMUS_PATH_BUFFER;
		guidObject		= GUID_All_Objects; //GUID_Buffer_Stereo;
		guidInterface	= IID_IDirectSoundBuffer;
		dwBuffer		= 0;
		break;
    default:
        fnsLog(ABORTLOGLEVEL, TEXT("Test error!!  Kelly, you're calling this with wrong path type!"));
        goto TEST_END;
	}
	// **********************************************************************
	// 5) AudioPath->GetObjectInPath(buffer)
	// **********************************************************************
	dwIndex = 0;
	while(S_OK == (hr = pAudioPath->GetObjectInPath(DMUS_PCHANNEL_ALL,dwStage,dwBuffer,guidObject,dwIndex++,guidInterface,(void**)&pUnknown)))
	{
		hr = pUnknown->QueryInterface(guidInterface, (void **)&pUnkQI);
		if(FAILED(hr))
		{
			fnsLog(ABORTLOGLEVEL, TEXT("!!!! BUG: IUnknown->QueryInterface ")
					TEXT("failed (%s == %08Xh)"),
					tdmXlatHRESULT(hr), hr);
			goto TEST_END;
		}
		if(pUnknown)
		{
			pUnknown->Release();
			pUnknown = NULL;
		}
		if(pUnkQI)
		{
			pUnkQI->Release();
			pUnkQI = NULL;
		}
		fnsLog(BUGLOGLEVEL, TEXT("*** PORT FOUND: IDirectMusicAudioPath::GetObjectInPath(%ld) succeeded (%s,%s)"),dwIndex,tdmXlatGUID(guidObject),tdmXlatGUID(guidInterface));
	}
	// **********************************************************************
	// 6) AudioPath->GetObjectInPath(port)
	// **********************************************************************
	dwIndex = 0;
	guidInterface = IID_IDirectMusicPort;
	while(S_OK == (hr = pAudioPath->GetObjectInPath(DMUS_PCHANNEL_ALL,DMUS_PATH_PORT,0,GUID_All_Objects,dwIndex++,IID_IDirectMusicPort,(void**)&pDMPort)))
	{
		if(pDMPort)
		{
			fnsIncrementIndent();
			fnsLog(BUGLOGLEVEL, TEXT("*** PORT FOUND: IDirectMusicAudioPath::GetObjectInPath(%ld) succeeded (%s)"),dwIndex,tdmXlatGUID(guidInterface));
			fnsDecrementIndent();
			ZeroMemory(&caps, sizeof(DMUS_PORTCAPS));
			caps.dwSize = sizeof(DMUS_PORTCAPS);
			hr = pDMPort->GetCaps(&caps);
			if(FAILED(hr))
			{
				fnsLog(BUGLOGLEVEL, TEXT("!!!! BUG: IDirectMusicPort->GetCaps ")
					TEXT("failed (%s == %08Xh)"),
					tdmXlatHRESULT(hr), hr);
				goto TEST_END;
			}
			else
			{
				fnsIncrementIndent();
				tdmLogDMUS_PORTCAPS(STRUCTLOGLEVEL,&caps);
				fnsDecrementIndent();
			}
			if(bCheckFormat)
			{
				dwWfxSize = 0;
				dwBufferSize = 0;
// **********************************************************************
// 7) Port->GetFormat
// **********************************************************************
				hr = pDMPort->GetFormat(NULL, &dwWfxSize, &dwBufferSize);
				if(FAILED(hr))
				{
					fnsLog(ABORTLOGLEVEL, "**** ABORT: Port->GetFormat failed");
					goto TEST_END;
				}
				pWfx = (WAVEFORMATEX *)LocalAlloc(LPTR, dwWfxSize);
				if(NULL == pWfx)
				{
					fnsLog(ABORTLOGLEVEL, "**** ABORT: LocalAlloc failed");
					goto TEST_END;
				}
				hr = pDMPort->GetFormat(pWfx, &dwWfxSize, &dwBufferSize);
				if(FAILED(hr))
				{
					fnsLog(ABORTLOGLEVEL, "**** ABORT: Port->GetFormat failed");
					goto TEST_END;
				}
				fnsLog(FYILOGLEVEL, TEXT("--- Sample Rate: (%d)"),pWfx->nSamplesPerSec);
				LocalFree(pWfx);
				pWfx = NULL;
			}

			// Release the port
			pDMPort->Release();
			pDMPort = NULL;
		}
		else
		{
			fnsLog(BUGLOGLEVEL, TEXT("!!!! BUG: PORT NOT FOUND: AudioPath::GetObjectInPath(%ld) succeeded (%s) but pPort is NULL"),dwIndex,tdmXlatHRESULT(hr));
			goto TEST_END;
		}
	}
	if(FAILED(hr) && (DMUS_E_NOT_FOUND != hr))
	{
		fnsLog(BUGLOGLEVEL, TEXT("!!!! BUG: IDirectMusicAudioPath->GetObjectInPath ")
				TEXT("failed (%s == %08Xh)"),
				tdmXlatHRESULT(hr), hr);
		goto TEST_END;
	}
	dwRes = FNS_PASS;

TEST_END:
	// **********************************************************************
	// 8) Cleanup objects and uninitialize and Return results
	// *********************************************************************
	if(pWfx)
	{
		LocalFree(pWfx);
	}
	if(pwPerformance8)
	{
		pwPerformance8->CloseDown();
		pwPerformance8->Release();
		pwPerformance8 = NULL;
	}
	if(pwMusic)
	{
		pwMusic->Release();
		pwMusic = NULL;
	}
	if(pDMPort)
	{
		pDMPort->Release();
		pDMPort = NULL;
	}
	if(pAudioPath)
	{
		pAudioPath->Release();
		pAudioPath = NULL;
	}
	if(pUnknown)
	{
		pUnknown->Release();
		pUnknown = NULL;
	}
	if(pUnkQI)
	{
		pUnkQI->Release();
		pUnkQI = NULL;
	}

	dmthCoUninitialize();	
	return dwRes;
}	





//--------------------------------------------------------------------------;
// @doc IDIRECTMUSICPERFORMANCE8 KCRAVEN
//
// @topic IDirectMusicPerformance8::PlaySegmentEx()_Valid_Test |
//
// Description: <nl>
// Valid test function for IDirectMusicPerformance8::PlaySegmentEx()<nl>
//
// Test Function: <nl>
// tdmperv8PlaySegmentEx()<nl>
//
// Parameters: <nl>
//	BOOL	fCoCreate	- unused <nl>
//	BOOL	fMaxDebug	- unused <nl>
//	DWORD	dwParam1	- unused <nl>
//	DWORD	dwParam2	- unused <nl>
//
// File : tdmperv8.cpp<nl>
//
// Steps: <nl>
//	 1) Initialize<nl>
//	 2) Create Performance8<nl>
//	 3.0) Create Loader<nl>
//	 3.1) SetSearchDirectory<nl>
//	 3.2) Get the Segment<nl>
//	 4.0) Init the Performance<nl>
//	 4.1) Add the default Port<nl>
//	 4.2) Download<nl>
//	 5) Call PlaySegmentEx - the method to be tested<nl>
//	 6) varification - segment should play<nl>
//	 7) Cleanup objects and uninitialize and Return results<nl>
//
// History: <nl>
//	03/15/2000 - kcraven - created<nl>
//
// @index topic | IDIRECTMUSICPERFORMANCE8
//--------------------------------------------------------------------------;
DWORD tdmperv8PlaySegmentEx(
	BOOL fCoCreate,
	BOOL fMaxDebug,
	LPARAM dwParam1,
	LPARAM dwParam2)
{
	DWORD						dwRes			= FNS_ABORTED;
	HRESULT 					hr				= S_OK;

	WCHAR*						pszFile 		= NULL;
	dmthSetPath(NULL);
	WCHAR*						wchPath 		= gwszBasePath;
//	WCHAR*						wchFile 		= gwszMediaPath;

	CtIDirectMusicLoader8		*pwLoader		= NULL;
	CtIDirectMusicPerformance8	*pwPerformance	= NULL;
	IDirectMusicPerformance8	*pPerformance	= NULL;
	CtIDirectMusicSegment8		*pwSegment		= NULL;

	DMUS_OBJECTDESC 			desc; 

	int 						counter 		= 0;
	int 						SleepTime		= 3000;
	int 						TimeOutValue	= 10;

	BOOL						fAuto;
	DWORD						testtype		= (DWORD)dwParam2;

	switch(testtype)
	{
		case 1:
			fAuto = FALSE;
			pszFile = L"test.mid";
			break;
		case 2:
		case 4:
			fAuto = TRUE;
			pszFile = L"WaveEmbed.sgt";
			break;
		case 3:
		case 5:
			fAuto = FALSE;
			pszFile = L"WaveEmbed.sgt";
			break;
		default:
			return FNS_PENDING;
	}
	// **********************************************************************
	// 1) Initialize
	// **********************************************************************
	hr = dmthCoInitialize(NULL);
	if(FAILED(hr))
	{
		fnsLog(ABORTLOGLEVEL,
			TEXT("**** ABORT:  initialization failed! (%s == %08Xh)"),
			tdmXlatHRESULT(hr), hr);
		goto TEST_END;
	}
	// **********************************************************************
	// 2) Create Performance8
	// **********************************************************************
	hr = dmthCreatePerformance(IID_IDirectMusicPerformance8, &pwPerformance);
	if(FAILED(hr))
	{
		fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: IDirectMusicPerformance8 object creation "),
			TEXT("failed (%s == %08Xh)"),
			tdmXlatHRESULT(hr), hr);
		goto TEST_END;
	}
	// **********************************************************************
	// 3.0) Create Loader
	// **********************************************************************
	hr = dmthCreateLoader(IID_IDirectMusicLoader8, &pwLoader);
	if (FAILED(hr))
	{
		fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: CoCreate Loader object ")
				TEXT("failed (%s == %08Xh)"),
				tdmXlatHRESULT(hr), hr);
		goto TEST_END;
	}
	// **********************************************************************
	// 3.1) SetSearchDirectory
	// **********************************************************************
	hr = pwLoader->SetSearchDirectory(GUID_DirectMusicAllTypes, wchPath, FALSE);
	if (FAILED(hr))
	{
		fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: Loader->SetSeachDirectory(media directory) ")
				TEXT("failed (%s == %08Xh)"),
				tdmXlatHRESULT(hr), hr);
		goto TEST_END;
	}
	// **********************************************************************
	// 3.2) Get the Segment
	// **********************************************************************
	ZeroMemory(&desc, sizeof(desc));
	desc.dwSize = sizeof(DMUS_OBJECTDESC);
	desc.guidClass = CLSID_DirectMusicSegment;
	desc.dwValidData |= (DMUS_OBJ_CLASS | DMUS_OBJ_FILENAME);
	wcscpy(desc.wszFileName,pszFile);
	hr = pwLoader->GetObject(&desc,CTIID_IDirectMusicSegment8,(void **)&pwSegment);
	if(FAILED(hr))
	{
		fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: Loader->GetObject(pwSegment) ")
				TEXT("failed (%s == %08Xh)"),
				tdmXlatHRESULT(hr), hr);
		goto TEST_END;
	}
	// **********************************************************************
	// 4.0) Init the Performance
	// **********************************************************************
	switch(testtype)
	{
	case 1:
	case 2:
	case 3:
		hr = pwPerformance->Init(NULL,NULL,ghWnd);
		if(FAILED(hr))
		{
			fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: pwPerformance->Init(NULL) ")
						TEXT("failed (%s == %08Xh)"),
						tdmXlatHRESULT(hr), hr);
			goto TEST_END;
		}
		// **********************************************************************
		// 4.1) Add the Port
		// **********************************************************************	
		hr = pwPerformance->AddPort( NULL );
		if(FAILED(hr))
		{
			fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: pwPerformance->AddPort(NULL) ")
					TEXT("failed (%s == %08Xh)"),
					tdmXlatHRESULT(hr), hr);
			goto TEST_END;
		}
		break;
	case 4:
	case 5:
		hr = pwPerformance->InitAudio(NULL,NULL,ghWnd,DMUS_APATH_SHARED_STEREOPLUSREVERB,64,NULL,NULL);
		if(FAILED(hr))
		{
			fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: pwPerformance->Init(NULL) ")
						TEXT("failed (%s == %08Xh)"),
						tdmXlatHRESULT(hr), hr);
			goto TEST_END;
		}
		break;
	}
	// **********************************************************************
	// 4.2) Download
	// **********************************************************************	
	if(fAuto)
	{
		hr = pwPerformance->SetGlobalParam( GUID_PerfAutoDownload, &fAuto, sizeof(BOOL) );
		if(FAILED(hr))
		{
			fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: Performance->SetGlobalParam(AutoDownload == TRUE) ")
					TEXT("failed (%s == %08Xh)"),
					tdmXlatHRESULT(hr), hr);
			goto TEST_END;
		}
	}
	else
	{
		hr = pwPerformance->GetRealObjPtr(&pPerformance);
		if (FAILED( hr ))
		{
			fnsLog(ABORTLOGLEVEL, "**** ABORT: Failed to get Real Object Pointer(performance) "
					"(HRESULT: %s == %08Xh)", tdmXlatHRESULT(hr), hr);
			goto TEST_END;
		}
		hr = pwSegment->SetParam(GUID_Download, -1, 0, 0, (void*)pPerformance); // needs real object - no need to release - no addref done 
		if(testtype == 3)
		{
			if(hr != E_NOTIMPL)
			{
				fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: pwSegment->SetParam(Download) ")
						TEXT("did not return E_NOTIMPL (%s == %08Xh)"),
						tdmXlatHRESULT(hr), hr);
				goto TEST_END;
			}
		}
		else if(FAILED(hr)) // 3 == downloading was
		{
			fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: pwSegment->SetParam(Download) ")
					TEXT("failed (%s == %08Xh)"),
					tdmXlatHRESULT(hr), hr);
			goto TEST_END;
		}
	}
	dwRes = FNS_FAIL;
	// **********************************************************************
	// 5) Call PlaySegmentEx - the method to be tested
	// **********************************************************************
	hr = pwPerformance->PlaySegmentEx( 
		pwSegment,			// Segment to play 
		0,NULL, 			// Optional stuff NULL. 
		DMUS_SEGF_BEAT, 	// DMUS_SEGF_ flags.
		0,NULL,NULL,NULL);	// Optional stuff is NULL. 
	if(FAILED(hr))
	{
		fnsLog(ABORTLOGLEVEL, TEXT("!!!! BUG: pwPerformance->PlaySegmentEx ")
				TEXT("failed (%s == %08Xh)"),
				tdmXlatHRESULT(hr), hr);
		goto TEST_END;
	}
	// **********************************************************************
	// 6) varification - segment should play
	// **********************************************************************
	counter = 0;
	SleepTime = 1000;
	while ((hr = pwPerformance->IsPlaying(pwSegment, NULL)) != S_OK)	
	{
		Sleep(SleepTime);
		if(counter++ > TimeOutValue)
		{
			fnsLog(BUGLOGLEVEL, TEXT("!!!! BUG: IsPlaying() timed out\n")
				TEXT("Segment did not start playing in the time specified (%s == %08Xh)"),
				tdmXlatHRESULT(hr), hr);
			goto TEST_END;
		}
	}
	Sleep(4000);
	dwRes = FNS_PASS;

TEST_END:
	// *********************************************************************
	// 7) Cleanup objects and uninitialize and Return results
	// *********************************************************************
	if(pwPerformance)
	{
		pwPerformance->CloseDown();
		pwPerformance->Release();
		pwPerformance = NULL;
	}
	if(pwSegment)
	{
		if((!fAuto) && pPerformance)
		{
			pwSegment->SetParam(GUID_Unload, -1, 0, 0, (void*)pPerformance); //don't case about the return code at this point
		}
		pwSegment->Release();
		pwSegment = NULL;
	}
	if(pPerformance)
	{
		pPerformance->Release();
		pPerformance = NULL;
	}
	if(pwLoader)
	{
		pwLoader->Release();
		pwLoader = NULL;
	}

	dmthCoUninitialize();	
	return dwRes;
}	




//--------------------------------------------------------------------------;
// @doc IDIRECTMUSICPERFORMANCE8 KCRAVEN
//
// @topic IDirectMusicPerformance8::PlaySegmentEx(types)_Valid_Test |
//
// Description: <nl>
// Valid test function for IDirectMusicPerformance8::PlaySegmentEx(types)<nl>
//
// Test Function: <nl>
// tdmperv8PlaySegmentTypeEx()<nl>
//
// Parameters: <nl>
//	BOOL	fCoCreate	- unused <nl>
//	BOOL	fMaxDebug	- unused <nl>
//	DWORD	dwParam1	- unused <nl>
//	DWORD	dwParam2	- unused <nl>
//
// File : tdmperv8.cpp<nl>
//
// Steps: <nl>
//	 1) Initialize<nl>
//	 2) Create Performance8<nl>
//	 3.0) Create Loader<nl>
//	 3.1) SetSearchDirectory<nl>
//	 3.2) Get the Segment<nl>
//	 4.0) Init the Performance<nl>
//	 4.1) Add the default Port<nl>
//	 4.2) Download<nl>
//	 5) Call PlaySegmentEx - the method to be tested<nl>
//	 6) varification - segment should play<nl>
//	 7) Cleanup objects and uninitialize and Return results<nl>
//
// History: <nl>
//	05/22/2000 - kcraven - created<nl>
//
// @index topic | IDIRECTMUSICPERFORMANCE8
//--------------------------------------------------------------------------;
DWORD tdmperv8PlaySegmentTypeEx(
	BOOL fCoCreate,
	BOOL fMaxDebug,
	LPARAM dwParam1,
	LPARAM dwParam2)
{
	DWORD						dwRes			= FNS_ABORTED;
	HRESULT 					hr				= S_OK;

	dmthSetPath(NULL);
	WCHAR*						wchPath 		= gwszBasePath;
//	WCHAR*						wchFile 		= gwszMediaPath;

	CtIDirectMusic*				pwMusic			= NULL;
	CtIDirectMusicLoader8*		pwLoader		= NULL;
	CtIDirectMusicPerformance8*	pwPerformance	= NULL;
	CtIDirectMusicSegmentState*	pwSegmentState	= NULL;
	CtIDirectMusicSegment8*		pwSegment		= NULL;
	CtIDirectMusicSegment8*		pwTransSeg		= NULL;
	CtIDirectMusicSegment8*		pwOneSeg		= NULL;
//	CtIDirectMusicSong8*		pwSong			= NULL;
	CtIDirectMusicAudioPath*	pwPath			= NULL;
	CtIDirectMusicAudioPath*	pwPath2			= NULL;
	CtIDirectMusicObject*		pwPathCfg		= NULL;

	int 						counter 		= 0;
	int 						SleepTime		= 3000;
	int 						TimeOutValue	= 20;

	DWORD						testtype		= (DWORD)dwParam2;

	WCHAR*						pMidiFile 		= L"test.mid";
	WCHAR*						pPathFile 		= NULL;
	WCHAR*						pOneFile 		= NULL;
//	WCHAR*						pSngFile 		= NULL;
	WCHAR*						pTransFile 		= NULL;
	DWORD						dwPathType		= DMUS_APATH_SHARED_STEREOPLUSREVERB;
	DWORD						dwPChannelCount	= 16;
	DWORD						dwPathType2		= DMUS_APATH_DYNAMIC_STEREO;
	DWORD						dwPChannelCount2	= 1;
	BOOL						fActivate		= TRUE;
	BOOL						fAuto			= FALSE;
	DWORD						dwFlags			= DMUS_SEGF_BEAT;
	BOOL						hardware		= FALSE;

	// which type of test to do
	switch(testtype)
	{
	case 0: //download
	case 2: 
	case 4:
		break;
	case 5:
		pPathFile 		= L"both_1.aud";
		pMidiFile 		= L"test2.sgt";
		hardware		= TRUE;
		break;
	case 6:
		pPathFile 		= L"both_1.aud";
		pMidiFile 		= L"test2.sgt";
		fAuto			= TRUE;
		hardware		= TRUE;
		break;
	case 7:
		pPathFile 		= L"both_2.aud";
		pMidiFile 		= L"test2.sgt";
		hardware		= TRUE;
		break;
	case 8:
		pPathFile 		= L"both_2.aud";
		pMidiFile 		= L"test2.sgt";
		fAuto			= TRUE;
		hardware		= TRUE;
		break;
//	case 9: // play song with no transition
//		pMidiFile		= NULL; // no second file
//		pSngFile 		= L"blues.sng";
//		break;
//	case 10: // play song w/ autotransition to default seg
//		pSngFile 		= L"blues.sng";
//		dwFlags			= DMUS_SEGF_MEASURE|DMUS_SEGF_AUTOTRANSITION;
//		break;
//	case 11: //play song with autotransition to composable seg 
//		pSngFile 		= L"blues.sng";
//		pMidiFile		= L"Seg8STC.sgt";
//		dwFlags			= DMUS_SEGF_MEASURE|DMUS_SEGF_AUTOTRANSITION;
//		break;
	case 12: // play seg w/ autotransition using template
		pOneFile 		= L"Seg8STC.sgt";
		pMidiFile		= L"test.sgt";
		pTransFile		= L"AutoTrans.sgt";
		dwFlags			= DMUS_SEGF_MEASURE|DMUS_SEGF_AUTOTRANSITION;
		fAuto			= TRUE;
		break;
//	case 13: // play song w/ autotransition using template
//		pSngFile 		= L"blues.sng";
//		pMidiFile		= L"test.sgt";
//		pTransFile		= L"AutoTrans.sgt";
//		dwFlags			= DMUS_SEGF_MEASURE|DMUS_SEGF_AUTOTRANSITION;
//		fAuto			= TRUE;
//		break;
//	case 14: // play song w/ autotransition using template but no autotransition flag
//		pSngFile 		= L"blues.sng";
//		pMidiFile		= L"test.sgt";
//		pTransFile		= L"AutoTrans.sgt";
//		dwFlags			= DMUS_SEGF_MEASURE; //|DMUS_SEGF_AUTOTRANSITION;
//		fAuto			= TRUE;
//		break;
	case 15: // play song w/ autotransition using template but no autotransition flag
//		pSngFile 		= L"blues.sng";
		pMidiFile		= L"portbvt1.wav";
//		pTransFile		= L"AutoTrans.sgt";
//		dwFlags			= DMUS_SEGF_MEASURE; //|DMUS_SEGF_AUTOTRANSITION;
//		fAuto			= TRUE;
		break;
	case 1: //autodownload
	case 3:
		fAuto			= TRUE;
		break;
	default: // not null - not supported
		return FNS_PENDING;
		break;
	}
	// **********************************************************************
	// 1) Initialize
	// **********************************************************************
	hr = dmthCoInitialize(NULL);
	if(FAILED(hr))
	{
		fnsLog(ABORTLOGLEVEL,
			TEXT("**** ABORT:  initialization failed! (%s == %08Xh)"),
			tdmXlatHRESULT(hr), hr);
		goto TEST_END;
	}
	// **********************************************************************
	// 2) Create Performance8
	// **********************************************************************
	hr = dmthCreatePerformance(IID_IDirectMusicPerformance8, &pwPerformance);
	if(FAILED(hr))
	{
		fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: IDirectMusicPerformance8 object creation "),
			TEXT("failed (%s == %08Xh)"),
			tdmXlatHRESULT(hr), hr);
		goto TEST_END;
	}
	// **********************************************************************
	// 3.0) Create Loader
	// **********************************************************************
	hr = dmthCreateLoader(IID_IDirectMusicLoader8, &pwLoader);
	if (FAILED(hr))
	{
		fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: CoCreate Loader object ")
				TEXT("failed (%s == %08Xh)"),
				tdmXlatHRESULT(hr), hr);
		goto TEST_END;
	}
	// **********************************************************************
	// 3.1) SetSearchDirectory
	// **********************************************************************
	hr = pwLoader->SetSearchDirectory(GUID_DirectMusicAllTypes, wchPath, FALSE);
	if (FAILED(hr))
	{
		fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: Loader->SetSeachDirectory(media directory) ")
				TEXT("failed (%s == %08Xh)"),
				tdmXlatHRESULT(hr), hr);
		goto TEST_END;
	}
	// **********************************************************************
	// 3.2) Get the Segment
	// **********************************************************************
	if(pTransFile)
	{
		hr = pwLoader->LoadObjectFromFile(CLSID_DirectMusicSegment,CTIID_IDirectMusicSegment8,pTransFile,(void **)&pwTransSeg);
		if(FAILED(hr))
		{
			fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: Loader->GetObject(pwSegment) ")
					TEXT("failed (%s == %08Xh)"),
					tdmXlatHRESULT(hr), hr);
			goto TEST_END;
		}
	}
	if(pOneFile)
	{
		hr = pwLoader->LoadObjectFromFile(CLSID_DirectMusicSegment,CTIID_IDirectMusicSegment8,pOneFile,(void **)&pwOneSeg);
		if(FAILED(hr))
		{
			fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: Loader->GetObject(pwSegment) ")
					TEXT("failed (%s == %08Xh)"),
					tdmXlatHRESULT(hr), hr);
			goto TEST_END;
		}
	}
//	if(pSngFile)
//	{
//		hr = pwLoader->LoadObjectFromFile(CLSID_DirectMusicSong,CTIID_IDirectMusicSong8,pSngFile,(void **)&pwSong);
//		if(FAILED(hr))
//		{
//			fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: Loader->GetObject(pwSegment) ")
//					TEXT("failed (%s == %08Xh)"),
//					tdmXlatHRESULT(hr), hr);
//			goto TEST_END;
//		}
//	}
	if(pMidiFile)
	{
		hr = pwLoader->LoadObjectFromFile(CLSID_DirectMusicSegment,CTIID_IDirectMusicSegment8,pMidiFile,(void **)&pwSegment);
		if(FAILED(hr))
		{
			fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: Loader->GetObject(pwSegment) ")
					TEXT("failed (%s == %08Xh)"),
					tdmXlatHRESULT(hr), hr);
			goto TEST_END;
		}
	}
	if(pPathFile)
	{
		hr = pwLoader->LoadObjectFromFile(CLSID_DirectMusicAudioPathConfig,CTIID_IDirectMusicObject,pPathFile,(void **)&pwPathCfg);
		if(FAILED(hr))
		{
			fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: Loader->GetObject(pwPathCfg) ")
					TEXT("failed (%s == %08Xh)"),
					tdmXlatHRESULT(hr), hr);
			goto TEST_END;
		}
	}
	// **********************************************************************
	// 4.0) Init the Performance
	// **********************************************************************	
	switch(testtype)
	{
	default:
		hr = pwPerformance->InitAudio(&pwMusic,NULL,ghWnd,NULL,NULL,NULL,NULL);
		if(FAILED(hr))
		{
			fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: pwPerformance->Init or InitAudio(NULLs) ")
						TEXT("failed (%s == %08Xh)"),
						tdmXlatHRESULT(hr), hr);
			goto TEST_END;
		}
		break;
	case 2:
	case 3:
		hr = pwPerformance->Init(&pwMusic,NULL,ghWnd);
		if(FAILED(hr))
		{
			fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: pwPerformance->Init or InitAudio(NULLs) ")
						TEXT("failed (%s == %08Xh)"),
						tdmXlatHRESULT(hr), hr);
			goto TEST_END;
		}
		break;
	}

	// **********************************************************************
	// 4.1) Add the Port
	// **********************************************************************	
	switch(testtype)
	{
	default:
		if(!pwPathCfg)
		{
			hr = pwPerformance->CreateStandardAudioPath(
				dwPathType, 			// Choice of standard audio path.
				dwPChannelCount,	// How many pchannels to create.
				fActivate,
				&pwPath);
			if(FAILED(hr))
			{
				fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: pwPerformance->CreateStandardAudioPath() ")
						TEXT("failed (%s == %08Xh)"),
						tdmXlatHRESULT(hr), hr);
				goto TEST_END;
			}
			if(!pwPath)
			{
				fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: AudioPath is NULL "));
				goto TEST_END;
			}
			if(testtype == 4)
			{
				hr = pwPerformance->CreateStandardAudioPath(
					dwPathType2, 			// Choice of standard audio path.
					dwPChannelCount2,	// How many pchannels to create.
					fActivate,
					&pwPath2);
				if(FAILED(hr))
				{
					fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: pwPerformance->CreateStandardAudioPath(2) ")
							TEXT("failed (%s == %08Xh)"),
							tdmXlatHRESULT(hr), hr);
					goto TEST_END;
				}
				if(!pwPath2)
				{
					fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: AudioPath2 is NULL "));
					goto TEST_END;
				}
			}
		}
		else
		{
			hr = pwPerformance->CreateAudioPath(
				pwPathCfg, 			// audio path confif object.
				fActivate,
				&pwPath);
			if(!HasDSOne(pwMusic) && hardware)
			{
				// CreateAudioPath is expected to fail
				if(SUCCEEDED(hr))
				{
					//failed
					goto TEST_END;
				}
				else
				{
					//passed
					if(hr == E_NOINTERFACE)
					{
						dwRes = FNS_PASS;
					}
					goto TEST_END;
				}
			}
			if(FAILED(hr))
			{
				fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: pwPerformance->CreateAudioPath() ")
						TEXT("failed (%s == %08Xh)"),
						tdmXlatHRESULT(hr), hr);
				goto TEST_END;
			}
			if(!pwPath)
			{
				fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: AudioPath is NULL "));
				goto TEST_END;
			}
		}
		if(fAuto && pwPath)
		{
			hr = pwPerformance->SetDefaultAudioPath(pwPath);
			if(FAILED(hr))
			{
				fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: pwPerformance->CreateAudioPath() ")
						TEXT("failed (%s == %08Xh)"),
						tdmXlatHRESULT(hr), hr);
				goto TEST_END;
			}
			pwPath->Release();
			pwPath = NULL;
		}
		break;
	case 2:
	case 3:
		hr = pwPerformance->AddPort(NULL);
		if(FAILED(hr))
		{
			fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: pwPerformance->CreateStandardAudioPath or AddPort() ")
					TEXT("failed (%s == %08Xh)"),
					tdmXlatHRESULT(hr), hr);
			goto TEST_END;
		}
		break;
	}
	// **********************************************************************
	// 4.2) Download
	// **********************************************************************	
	if(fAuto)
	{
		hr = pwPerformance->SetGlobalParam(GUID_PerfAutoDownload, &fAuto, sizeof(BOOL));
		if(FAILED(hr))
		{
			fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: Performance->SetGlobalParam(AutoDownload == TRUE) ")
					TEXT("failed (%s == %08Xh)"),
					tdmXlatHRESULT(hr), hr);
			goto TEST_END;
		}
	}
	else
	{
		switch(testtype)
		{
		case 4:
			hr = pwSegment->Download(pwPath2); //we would have aborted earlier if pwPath2 was NULL
			break;
		case 5:
		case 6:
		case 7:
		case 8:
			hr = pwSegment->Download(pwPath);
			break;
		case 9:
		case 10:
		case 11:
			if(pwSegment)
			{
				hr = pwSegment->Download(pwPath);
				if(FAILED(hr))
				{
					fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: pwSegment->Download() ")
							TEXT("failed (%s == %08Xh)"),
							tdmXlatHRESULT(hr), hr);
					goto TEST_END;
				}
				if(S_FALSE == hr)
				{
					fnsLog(FYILOGLEVEL, TEXT("---- FYI: pwSegment->Download() ")
							TEXT("returned (%s == %08Xh)"),
							tdmXlatHRESULT(hr), hr);
				}
			}
//			hr = pwSong->Download(pwPath);
			break;
		default:
			hr = pwSegment->Download(pwPerformance);
			break;
		}
		if(FAILED(hr))
		{
			fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: pwSegment->Download() ")
					TEXT("failed (%s == %08Xh)"),
					tdmXlatHRESULT(hr), hr);
			goto TEST_END;
		}
		if(S_FALSE == hr)
		{
			fnsLog(FYILOGLEVEL, TEXT("---- FYI: pwSegment->Download() ")
					TEXT("returned (%s == %08Xh)"),
					tdmXlatHRESULT(hr), hr);
		}
	}
	dwRes = FNS_FAIL;
	// **********************************************************************
	// 5) Call PlaySegmentEx - the method to be tested
	// **********************************************************************
	if(pwOneSeg)
	{
		hr = pwOneSeg->SetRepeats(0);
		if(FAILED(hr))
		{
			fnsLog(ABORTLOGLEVEL, TEXT("!!!! BUG: pwOneSeg->SetRepeats ")
					TEXT("failed (%s == %08Xh)"),
					tdmXlatHRESULT(hr), hr);
			goto TEST_END;
		}

		hr = pwPerformance->PlaySegmentEx( 
			pwOneSeg, 
			0,
			NULL,
			DMUS_SEGF_BEAT,
			0,
			&pwSegmentState,
			NULL,
			pwPath);
		if(FAILED(hr))
		{
			fnsLog(ABORTLOGLEVEL, TEXT("!!!! BUG: pwPerformance->PlaySegmentEx ")
					TEXT("failed (%s == %08Xh)"),
					tdmXlatHRESULT(hr), hr);
			goto TEST_END;
		}
		counter = 0;
		SleepTime = 1000;
		while ((hr = pwPerformance->IsPlaying(NULL,pwSegmentState)) != S_OK)	
		{
			Sleep(SleepTime);
			if(counter++ > TimeOutValue)
			{
				fnsLog(BUGLOGLEVEL, TEXT("!!!! BUG: IsPlaying() timed out\n")
					TEXT("Segment did not start playing in the time specified (%s == %08Xh)"),
					tdmXlatHRESULT(hr), hr);
				goto TEST_END;
			}
		}
//		Sleep(4000);
	}
	if(pwSegmentState)
	{
		pwSegmentState->Release();
		pwSegmentState = NULL;
	}
//	if(pwSong)
//	{
//		hr = pwPerformance->PlaySegmentEx( 
//		pwSong, 
//		0,
//		pwTransSeg,
//		DMUS_SEGF_BEAT,
//		0,
//		&pwSegmentState,
//		NULL,
//		pwPath);
//		if(FAILED(hr))
//		{
//			fnsLog(ABORTLOGLEVEL, TEXT("!!!! BUG: pwPerformance->PlaySegmentEx ")
//					TEXT("failed (%s == %08Xh)"),
//					tdmXlatHRESULT(hr), hr);
//			goto TEST_END;
//		}
//		counter = 0;
//		SleepTime = 1000;
//		while ((hr = pwPerformance->IsPlaying(NULL,pwSegmentState)) != S_OK)	
//		{
//			Sleep(SleepTime);
//			if(counter++ > TimeOutValue)
//			{
//				fnsLog(BUGLOGLEVEL, TEXT("!!!! BUG: IsPlaying() timed out\n")
//					TEXT("Segment did not start playing in the time specified (%s == %08Xh)"),
//					tdmXlatHRESULT(hr), hr);
//				goto TEST_END;
//			}
//		}
////		Sleep(4000);
//	}
	if(pwSegmentState)
	{
		pwSegmentState->Release();
		pwSegmentState = NULL;
	}
	if(pwSegment)
	{
		hr = pwSegment->SetRepeats(0);
		if(FAILED(hr))
		{
			fnsLog(ABORTLOGLEVEL, TEXT("!!!! BUG: pwSegment->SetRepeats ")
					TEXT("failed (%s == %08Xh)"),
					tdmXlatHRESULT(hr), hr);
			goto TEST_END;
		}
		hr = pwPerformance->PlaySegmentEx( 
		pwSegment, 
		0,
		pwTransSeg,
		dwFlags,
		0,
		&pwSegmentState,
		NULL,
		pwPath);
		if(FAILED(hr))
		{
			fnsLog(ABORTLOGLEVEL, TEXT("!!!! BUG: pwPerformance->PlaySegmentEx ")
					TEXT("failed (%s == %08Xh)"),
					tdmXlatHRESULT(hr), hr);
			goto TEST_END;
		}
		counter = 0;
		SleepTime = 1000;
		while ((hr = pwPerformance->IsPlaying(NULL,pwSegmentState)) != S_OK)	
		{
			Sleep(SleepTime);
			if(counter++ > TimeOutValue)
			{
				fnsLog(BUGLOGLEVEL, TEXT("!!!! BUG: IsPlaying() timed out\n")
					TEXT("Segment did not start playing in the time specified (%s == %08Xh)"),
					tdmXlatHRESULT(hr), hr);
				goto TEST_END;
			}
		}
//		Sleep(4000);
	}
	// **********************************************************************
	// 6) varification - segment should play
	// **********************************************************************
	counter = 0;
	SleepTime = 1000;
	while ((hr = pwPerformance->IsPlaying(NULL,pwSegmentState)) == S_OK)	
	{
		Sleep(SleepTime);
		if(counter++ > TimeOutValue)
		{
			fnsLog(BUGLOGLEVEL, TEXT("!!!! BUG: IsPlaying() timed out\n")
				TEXT("Segment did not stop playing in the time specified (%s == %08Xh)"),
				tdmXlatHRESULT(hr), hr);
			goto TEST_END;
		}
	}
//	Sleep(8000);
	dwRes = FNS_PASS;

TEST_END:
	// *********************************************************************
	// 7) Cleanup objects and uninitialize and Return results
	// *********************************************************************
	if(pwPerformance)
	{
		pwPerformance->CloseDown();
	}
	if(pwSegment)
	{
		switch(testtype)
		{
		case 0: //download
			if(pwPerformance)
			{
				pwSegment->Unload(pwPerformance); //don't care about the return code at this point
			}
			break;
		case 2: 
			if(pwPath)
			{
				pwSegment->Unload(pwPath); //don't care about the return code at this point
			}
			break;
		case 4:
			if(pwPath2)
			{
				pwSegment->Unload(pwPath2); //don't care about the return code at this point
			}
			break;
		}
		pwSegment->Release();
		pwSegment = NULL;
	}
//	if(pwSong)
//	{
//		if(pwPath)
//		{
//			pwSong->Unload(pwPath); //don't care about the return code at this point
//		}
//		pwSong->Release();
//		pwSong = NULL;
//	}
	if(pwTransSeg)
	{
		pwTransSeg->Release();
		pwTransSeg = NULL;
	}
	if(pwSegmentState)
	{
		pwSegmentState->Release();
		pwSegmentState = NULL;
	}
	if(pwPerformance)
	{
		pwPerformance->Release();
		pwPerformance = NULL;
	}
	if(pwLoader)
	{
		pwLoader->Release();
		pwLoader = NULL;
	}
	if(pwPath)
	{
		pwPath->Release();
		pwPath = NULL;
	}
	if(pwPath2)
	{
		pwPath2->Release();
		pwPath2 = NULL;
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
// @doc IDIRECTMUSICPERFORMANCE8 KCRAVEN
//
// @topic IDirectMusicPerformance8::PlaySegmentEx(from)_Valid_Test |
//
// Description: <nl>
// Valid test function for IDirectMusicPerformance8::PlaySegmentEx(from)<nl>
//
// Test Function: <nl>
// tdmperv8PlaySegmentExFrom()<nl>
//
// Parameters: <nl>
//	BOOL	fCoCreate	- unused <nl>
//	BOOL	fMaxDebug	- unused <nl>
//	DWORD	dwParam1	- unused <nl>
//	DWORD	dwParam2	- unused <nl>
//
// File : tdmperv8.cpp<nl>
//
// Steps: <nl>
//	 1) Initialize<nl>
//	 2) Create Performance8<nl>
//	 3.0) Create Loader<nl>
//	 3.1) SetSearchDirectory<nl>
//	 3.2) Get the Segment<nl>
//	 4.0) Init the Performance<nl>
//	 4.1) Add the default Port<nl>
//	 4.2) Download<nl>
//	 5) Call PlaySegmentEx - the method to be tested<nl>
//	 6) varification - segment should play<nl>
//	 7) Cleanup objects and uninitialize and Return results<nl>
//
// History: <nl>
//	05/22/2000 - kcraven - created<nl>
//
// @index topic | IDIRECTMUSICPERFORMANCE8
//--------------------------------------------------------------------------;
DWORD tdmperv8PlaySegmentExFrom(
	BOOL fCoCreate,
	BOOL fMaxDebug,
	LPARAM dwParam1,
	LPARAM dwParam2)
{
	DWORD						dwRes			= FNS_ABORTED;
	HRESULT 					hr				= S_OK;

	dmthSetPath(NULL);
	WCHAR*						wchPath 		= gwszBasePath;
//	WCHAR*						wchFile 		= gwszMediaPath;

	CtIDirectMusic*				pwMusic			= NULL;
	CtIDirectMusicLoader8*		pwLoader		= NULL;
	CtIDirectMusicPerformance8*	pwPerformance	= NULL;
	CtIDirectMusicSegmentState*	pwSegmentState1	= NULL;
	CtIDirectMusicSegmentState*	pwSegmentState2	= NULL;
	CtIDirectMusicSegment8*		pwPSegment		= NULL;
	CtIDirectMusicSegment8*		pwTSegment		= NULL;
	CtIDirectMusicSegment8*		pwSSegment		= NULL;
	CtIDirectMusicSegment8*		pwWSegment		= NULL;
	CtIDirectMusicAudioPath*	pwPath			= NULL;
	CtIDirectMusicAudioPath*	pwPath2			= NULL;

	int 						counter 		= 0;
	int 						SleepTime		= 3000;
	int 						TimeOutValue	= 10;

	DWORD						testtype		= (DWORD)dwParam2;

	WCHAR*						pPMidiFile 		= L"test.mid";
	WCHAR*						pSMidiFile 		= L"test.sgt";
	WCHAR*						pWMidiFile 		= L"rdrum.sgt";
	WCHAR*						pTemplateFile 	= L"Seg8STC.sgt";
	DWORD						dwPathType		= DMUS_APATH_SHARED_STEREOPLUSREVERB;
	DWORD						dwPChannelCount	= 16;
	BOOL						fActivate		= TRUE;
	DWORD						dwFlags			= NULL;

	// which type of test to do
	switch(testtype)
	{
	case 1: // self beat
	case 2: // self measure
	case 3: // self queue
	case 4: // autotransition beat
	case 5: // autotransition measure
	case 6: // autotransition queue
		break;
	case 7: // autotransition to second segment
		pPMidiFile 		= L"synctest.mid";
		break;
	default: // not null - not supported
		return FNS_PENDING;
		break;
	}
	// **********************************************************************
	// 1) Initialize
	// **********************************************************************
	hr = dmthCoInitialize(NULL);
	if(FAILED(hr))
	{
		fnsLog(ABORTLOGLEVEL,
			TEXT("**** ABORT:  initialization failed! (%s == %08Xh)"),
			tdmXlatHRESULT(hr), hr);
		goto TEST_END;
	}
	// **********************************************************************
	// 2) Create Performance8
	// **********************************************************************
	hr = dmthCreatePerformance(IID_IDirectMusicPerformance8, &pwPerformance);
	if(FAILED(hr))
	{
		fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: IDirectMusicPerformance8 object creation "),
			TEXT("failed (%s == %08Xh)"),
			tdmXlatHRESULT(hr), hr);
		goto TEST_END;
	}
	// **********************************************************************
	// 3.0) Create Loader
	// **********************************************************************
	hr = dmthCreateLoader(IID_IDirectMusicLoader8, &pwLoader);
	if (FAILED(hr))
	{
		fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: CoCreate Loader object ")
				TEXT("failed (%s == %08Xh)"),
				tdmXlatHRESULT(hr), hr);
		goto TEST_END;
	}
	// **********************************************************************
	// 3.1) SetSearchDirectory
	// **********************************************************************
	hr = pwLoader->SetSearchDirectory(GUID_DirectMusicAllTypes, wchPath, FALSE);
	if (FAILED(hr))
	{
		fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: Loader->SetSeachDirectory(media directory) ")
				TEXT("failed (%s == %08Xh)"),
				tdmXlatHRESULT(hr), hr);
		goto TEST_END;
	}
	// **********************************************************************
	// 3.1) SetSearchDirectory
	// **********************************************************************
	hr = pwLoader->ScanDirectory(CLSID_DirectMusicCollection, L"*", NULL);
	if (FAILED(hr))
	{
		fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: Loader->ScanDirectory(dls) ")
				TEXT("failed (%s == %08Xh)"),
				tdmXlatHRESULT(hr), hr);
		goto TEST_END;
	}
	hr = pwLoader->ScanDirectory(CLSID_DirectMusicChordMap, L"*", NULL);
	if (FAILED(hr))
	{
		fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: Loader->ScanDirectory(chordmap) ")
				TEXT("failed (%s == %08Xh)"),
				tdmXlatHRESULT(hr), hr);
		goto TEST_END;
	}
	hr = pwLoader->ScanDirectory(CLSID_DirectMusicStyle, L"*", NULL);
	if (FAILED(hr))
	{
		fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: Loader->ScanDirectory(style) ")
				TEXT("failed (%s == %08Xh)"),
				tdmXlatHRESULT(hr), hr);
		goto TEST_END;
	}
	// **********************************************************************
	// 3.2) Get the Segment
	// **********************************************************************
	hr = pwLoader->LoadObjectFromFile(CLSID_DirectMusicSegment,CTIID_IDirectMusicSegment8,pPMidiFile,(void **)&pwPSegment);
	if(FAILED(hr))
	{
		fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: Loader->GetObject(pwSegment) ")
				TEXT("failed (%s == %08Xh)"),
				tdmXlatHRESULT(hr), hr);
		goto TEST_END;
	}
	// **********************************************************************
	// 3.2) Get the Segment
	// **********************************************************************
	switch(testtype)
	{
	case 4: // autotransition
	case 5: // autotransition
	case 6: // autotransition
		hr = pwLoader->LoadObjectFromFile(CLSID_DirectMusicSegment,CTIID_IDirectMusicSegment8,pTemplateFile,(void **)&pwTSegment);
		if(FAILED(hr))
		{
			fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: Loader->GetObject(pwSegment) ")
					TEXT("failed (%s == %08Xh)"),
					tdmXlatHRESULT(hr), hr);
			goto TEST_END;
		}
		hr = pwTSegment->SetRepeats(0);
		if(FAILED(hr))
		{
			fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: Segment->SetRepeats() ")
					TEXT("failed (%s == %08Xh)"),
					tdmXlatHRESULT(hr), hr);
			goto TEST_END;
		}
		break;
	case 7: // secondary segment
		hr = pwLoader->LoadObjectFromFile(CLSID_DirectMusicSegment,CTIID_IDirectMusicSegment8,pWMidiFile,(void **)&pwWSegment);
		if(FAILED(hr))
		{
			fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: Loader->GetObject(pwSegment) ")
					TEXT("failed (%s == %08Xh)"),
					tdmXlatHRESULT(hr), hr);
			goto TEST_END;
		}
		hr = pwWSegment->SetRepeats(0);
		if(FAILED(hr))
		{
			fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: Segment->SetRepeats() ")
					TEXT("failed (%s == %08Xh)"),
					tdmXlatHRESULT(hr), hr);
			goto TEST_END;
		}
		break;
	}
	// **********************************************************************
	// 3.2) Get the Segment
	// **********************************************************************
	hr = pwLoader->LoadObjectFromFile(CLSID_DirectMusicSegment,CTIID_IDirectMusicSegment8,pSMidiFile,(void **)&pwSSegment);
	if(FAILED(hr))
	{
		fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: Loader->GetObject(pwSegment) ")
				TEXT("failed (%s == %08Xh)"),
				tdmXlatHRESULT(hr), hr);
		goto TEST_END;
	}
	// **********************************************************************
	// 4.0) Init the Performance
	// **********************************************************************	
	// **********************************************************************
	// 4.1) Add the Port
	// **********************************************************************	
	hr = pwPerformance->InitAudio(&pwMusic,NULL,ghWnd,NULL,NULL,NULL,NULL);
	if(FAILED(hr))
	{
		fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: pwPerformance->Init or InitAudio(NULLs) ")
					TEXT("failed (%s == %08Xh)"),
					tdmXlatHRESULT(hr), hr);
		goto TEST_END;
	}
	hr = pwPerformance->CreateStandardAudioPath(
		dwPathType, 			// Choice of standard audio path.
		dwPChannelCount,	// How many pchannels to create.
		fActivate,
		&pwPath);
	if(FAILED(hr))
	{
		fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: pwPerformance->CreateStandardAudioPath() ")
				TEXT("failed (%s == %08Xh)"),
				tdmXlatHRESULT(hr), hr);
		goto TEST_END;
	}
	if(!pwPath)
	{
		fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: AudioPath is NULL "));
		goto TEST_END;
	}
	// **********************************************************************
	// 4.2) Download
	// **********************************************************************	
	if(pwPSegment)
	{
		hr = pwPSegment->Download(pwPath);
		if(FAILED(hr))
		{
			fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: pwSegment->Download(pwPath) ")
					TEXT("failed (%s == %08Xh)"),
					tdmXlatHRESULT(hr), hr);
			goto TEST_END;
		}
		if(S_FALSE == hr)
		{
			fnsLog(FYILOGLEVEL, TEXT("---- FYI: pwSegment->Download(pwPath) ")
					TEXT("returned (%s == %08Xh)"),
					tdmXlatHRESULT(hr), hr);
		}
	}
	if(pwSSegment)
	{
		hr = pwSSegment->Download(pwPath);
		if(FAILED(hr))
		{
			fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: pwSegment->Download(pwPath) ")
					TEXT("failed (%s == %08Xh)"),
					tdmXlatHRESULT(hr), hr);
			goto TEST_END;
		}
		if(S_FALSE == hr)
		{
			fnsLog(FYILOGLEVEL, TEXT("---- FYI: pwSegment->Download(pwPath) ")
					TEXT("returned (%s == %08Xh)"),
					tdmXlatHRESULT(hr), hr);
		}
	}
	if(pwWSegment)
	{
		hr = pwWSegment->Download(pwPath);
		if(FAILED(hr))
		{
			fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: pwSegment->Download(pwPath) ")
					TEXT("failed (%s == %08Xh)"),
					tdmXlatHRESULT(hr), hr);
			goto TEST_END;
		}
		if(S_FALSE == hr)
		{
			fnsLog(FYILOGLEVEL, TEXT("---- FYI: pwSegment->Download(pwPath) ")
					TEXT("returned (%s == %08Xh)"),
					tdmXlatHRESULT(hr), hr);
		}
	}
	dwRes = FNS_FAIL;
	// **********************************************************************
	// 5) Call PlaySegmentEx - the method to be tested
	// **********************************************************************
	// **********************************************************************
	hr = pwPerformance->PlaySegmentEx( 
		pwPSegment, 
		0,
		NULL,
		DMUS_SEGF_QUEUE,
		0,
		&pwSegmentState1,
		NULL,
		pwPath);
	if(FAILED(hr))
	{
		fnsLog(ABORTLOGLEVEL, TEXT("!!!! BUG: pwPerformance->PlaySegmentEx ")
				TEXT("failed (%s == %08Xh)"),
				tdmXlatHRESULT(hr), hr);
		goto TEST_END;
	}
	// **********************************************************************
	// 6) varification - segment should play
	// **********************************************************************
	counter = 0;
	SleepTime = 300;
	while ((hr = pwPerformance->IsPlaying(pwPSegment,NULL)) != S_OK)	
	{
		Sleep(SleepTime);
		if(counter++ > TimeOutValue)
		{
			fnsLog(BUGLOGLEVEL, TEXT("!!!! BUG: IsPlaying(segment 1) timed out\n")
				TEXT("Segment did not start playing in the time specified (%s == %08Xh)"),
				tdmXlatHRESULT(hr), hr);
			goto TEST_END;
		}
	}
	// **********************************************************************
	// 5) Call PlaySegmentEx - the method to be tested
	// **********************************************************************
	switch(testtype)
	{
	case 1: // from self
		dwFlags = DMUS_SEGF_BEAT;
		break;
	case 2: // from self
		dwFlags = DMUS_SEGF_MEASURE;
		break;
	case 3: // from self
		dwFlags = DMUS_SEGF_QUEUE;
		break;
	case 4: // autotransition
		dwFlags = DMUS_SEGF_BEAT | DMUS_SEGF_AUTOTRANSITION;
		break;
	case 5: // autotransition
		dwFlags = DMUS_SEGF_MEASURE | DMUS_SEGF_AUTOTRANSITION;
		break;
	case 6: // autotransition
		dwFlags = DMUS_SEGF_QUEUE | DMUS_SEGF_AUTOTRANSITION;
		break;
	case 7: // autotransition second segment
		dwFlags = DMUS_SEGF_MEASURE | DMUS_SEGF_SECONDARY;
		break;
	}
	switch(testtype)
	{
	default: // from self
		hr = pwPerformance->PlaySegmentEx( 
			pwSSegment, 
			NULL,
			pwTSegment,
			dwFlags,
			NULL,
			&pwSegmentState2,
			pwSegmentState1,
			pwPath);
		break;
	case 7: // from self
		hr = pwPerformance->PlaySegmentEx( 
			pwSSegment, 
			NULL,
			NULL,
			dwFlags,
			NULL,
			&pwSegmentState2,
			NULL,
			pwPath);
		break;
	}
	if(FAILED(hr))
	{
		fnsLog(ABORTLOGLEVEL, TEXT("!!!! BUG: pwPerformance->PlaySegmentEx ")
				TEXT("failed (%s == %08Xh)"),
				tdmXlatHRESULT(hr), hr);
		goto TEST_END;
	}
	switch(testtype)
	{
	case 4:
	case 5:
	case 6:
		hr = pwPerformance->IsPlaying(pwSSegment,NULL);
		if(SUCCEEDED(hr) && (S_FALSE != hr))
		{
			fnsLog(ABORTLOGLEVEL, TEXT("!!!! BUG: Performance->IsPlaying ")
					TEXT("succeeded and should not have (%s == %08Xh)"),
					tdmXlatHRESULT(hr), hr);
			goto TEST_END;
		}
		break;
	case 7:
		dwFlags = DMUS_SEGF_QUEUE | DMUS_SEGF_SECONDARY;
		hr = pwPerformance->PlaySegmentEx( 
			pwWSegment, 
			NULL,
			NULL,
			dwFlags,
			NULL,
			NULL,
			pwSegmentState2,
			pwPath);
		if(FAILED(hr))
		{
			fnsLog(ABORTLOGLEVEL, TEXT("!!!! BUG: pwPerformance->PlaySegmentEx ")
					TEXT("failed (%s == %08Xh)"),
					tdmXlatHRESULT(hr), hr);
			goto TEST_END;
		}
		break;
	}
	counter = 0;
	SleepTime = 3000;
	while ((hr = pwPerformance->IsPlaying(NULL,pwSegmentState2)) != S_OK)	
	{
		Sleep(SleepTime);
		if(counter++ > TimeOutValue)
		{
			fnsLog(BUGLOGLEVEL, TEXT("!!!! BUG: IsPlaying(segstate) timed out\n")
				TEXT("Segment did not start playing in the time specified (%s == %08Xh)"),
				tdmXlatHRESULT(hr), hr);
			goto TEST_END;
		}
	}
	counter = 0;
	SleepTime = 3000;
	while ((hr = pwPerformance->IsPlaying(NULL,pwSegmentState2)) == S_OK)	
	{
		Sleep(SleepTime);
		if(counter++ > TimeOutValue)
		{
			fnsLog(BUGLOGLEVEL, TEXT("!!!! BUG: IsPlaying(segment 2) timed out\n")
				TEXT("Segment did not stop playing in the time specified (%s == %08Xh)"),
				tdmXlatHRESULT(hr), hr);
			goto TEST_END;
		}
	}
	if(pwWSegment)
	{
		counter = 0;
		SleepTime = 3000;
		while ((hr = pwPerformance->IsPlaying(pwWSegment,NULL)) == S_OK)	
		{
			Sleep(SleepTime);
			if(counter++ > TimeOutValue)
			{
				fnsLog(BUGLOGLEVEL, TEXT("!!!! BUG: IsPlaying(segment 3) timed out\n")
					TEXT("Segment did not stop playing in the time specified (%s == %08Xh)"),
					tdmXlatHRESULT(hr), hr);
				goto TEST_END;
			}
		}
	}
	dwRes = FNS_PASS;

TEST_END:
	// *********************************************************************
	// 7) Cleanup objects and uninitialize and Return results
	// *********************************************************************
	if(pwPerformance)
	{
		pwPerformance->CloseDown();
	}
	if(pwPSegment)
	{
		if(pwPath)
		{
			pwPSegment->Unload(pwPath); //don't care about the return code at this point
		}
		pwPSegment->Release();
		pwPSegment = NULL;
	}
	if(pwSegmentState1)
	{
		pwSegmentState1->Release();
		pwSegmentState1 = NULL;
	}
	if(pwSegmentState2)
	{
		pwSegmentState2->Release();
		pwSegmentState2 = NULL;
	}
	if(pwTSegment)
	{
		pwTSegment->Release();
		pwTSegment = NULL;
	}
	if(pwSSegment)
	{
		if(pwPath)
		{
			pwSSegment->Unload(pwPath); //don't care about the return code at this point
		}
		pwSSegment->Release();
		pwSSegment = NULL;
	}
	if(pwWSegment)
	{
		if(pwPath)
		{
			pwWSegment->Unload(pwPath); //don't care about the return code at this point
		}
		pwWSegment->Release();
		pwWSegment = NULL;
	}
	if(pwPerformance)
	{
		pwPerformance->Release();
		pwPerformance = NULL;
	}
	if(pwLoader)
	{
		pwLoader->Release();
		pwLoader = NULL;
	}
	if(pwPath)
	{
		pwPath->Release();
		pwPath = NULL;
	}
	if(pwPath2)
	{
		pwPath2->Release();
		pwPath2 = NULL;
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
// @doc IDIRECTMUSICPERFORMANCE8 KCRAVEN
//
// @topic IDirectMusicPerformance8::SetDefaultAudioPath()_Valid_Test |
//
// Description: <nl>
// Valid test function for IDirectMusicPerformance8::SetDefaultAudioPath()<nl>
//
// Test Function: <nl>
// tdmperv8SetDefaultAudioPath()<nl>
//
// Parameters: <nl>
//	BOOL	fCoCreate	- unused <nl>
//	BOOL	fMaxDebug	- unused <nl>
//	DWORD	dwParam1	- unused <nl>
//	DWORD	dwParam2	- unused <nl>
//
// File : tdmperv8.cpp<nl>
//
// Steps: <nl>
//	 1)   Initialize<nl>
//	 2.0) Create Loader<nl>
//	 2.1) Get the Segment<nl>
//	 2.2) Get the AudioPath<nl>
//	 3.0) Create CtIDirectMusicPerformance8 object <nl>
//	 3.1) InitAudio the performance witha standard audio path that plays silent<nl>
//	 4.0) Create an AudioPath from the audiopathconfig file to be set later<nl>
//	 4.1) Verify the results<nl>
//	 5.0) Play the segment - Download<nl>
//	 5.1) PlaySegmentEx<nl>
//	 5.2) Verify silent playback - Wait for IsPlaying - NO SOUND EXPECTED<nl>
//	 5.3) Quickly unload - if there had been sound (bug) it should stop <nl>
//	 6)   Call SetDefaultAudioPath - the function to be tested<nl>
//	 7.0) Verification - Play a segment - Download<nl>
//	 7.1) Verification - PlaySegmentEx<nl>
//	 7.2) Verification - Wait for IsPlaying - should make sound this time<nl>
//	 8)   Cleanup objects and uninitialize and Return results<nl>
//
// History: <nl>
//	03/15/2000 - kcraven - created<nl>
//
// @index topic | IDIRECTMUSICPERFORMANCE8
//--------------------------------------------------------------------------;
DWORD tdmperv8SetDefaultAudioPath(
	BOOL fCoCreate,
	BOOL fMaxDebug,
	LPARAM dwParam1,
	LPARAM dwParam2)
{
	DWORD						dwRes				= FNS_ABORTED;
	HRESULT 					hr					= S_OK;

	CtIDirectMusic*				pwMusic				= NULL;
	CtIDirectMusicPerformance8* pwPerformance8		= NULL;
	CtIDirectMusicLoader8*		pwLoader8			= NULL;
	CtIDirectMusicSegment8* 	pwSegment8			= NULL;
	CtIUnknown* 				pAudioPathConfig	= NULL;
	CtIDirectMusicAudioPath*	pAudioPath			= NULL;
	DMUS_OBJECTDESC 			desc; 

	int 						counter 			= 0;
	int 						SleepTime			= 3000;
	int 						TimeOutValue		= 10;

	WCHAR*						pMidiFile			= NULL;
	WCHAR*						pAudPathFile		= NULL;
	DWORD						testtype			= (DWORD)dwParam2;
	BOOL						hardware			= FALSE;

	switch(testtype)
	{
	case 1:
		pAudPathFile		= L"Standard.aud";
		pMidiFile			= L"test.mid";
		break;
	case 2:
		pAudPathFile		= L"Standard3DShared.aud";
		pMidiFile			= L"test.mid";
		break;
	case 3:
		pAudPathFile		= L"StandardReverbShared.aud";
		pMidiFile			= L"test.mid";
		break;
	case 4:
		pAudPathFile		= L"both_1.aud";
		pMidiFile			= L"test.sgt";
		hardware			= TRUE;
		break;
	case 5:
		pAudPathFile		= L"both_2.aud";
		pMidiFile			= L"test2.sgt";
		hardware			= TRUE;
		break;
	default:
		return FNS_PENDING;
	}
	// **********************************************************************
	// 1) Initialize
	// **********************************************************************
	hr = dmthCoInitialize(NULL);
	if(FAILED(hr))
	{
		fnsLog(ABORTLOGLEVEL,
			TEXT("**** ABORT:  initialization failed! (%s == %08Xh)"),
			tdmXlatHRESULT(hr), hr);
		goto TEST_END;
	}
	// **********************************************************************
	// 2.0) Create Loader
	// **********************************************************************
	hr = dmthCreateLoader(IID_IDirectMusicLoader8, &pwLoader8);
	if (FAILED(hr))
	{
		fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: CoCreate Loader object ")
				TEXT("failed (%s == %08Xh)"),
				tdmXlatHRESULT(hr), hr);
		goto TEST_END;
	}
	// **********************************************************************
	// 2.1) Get the Segment with fullpath
	// **********************************************************************
	dmthSetPath(pMidiFile);
	ZeroMemory(&desc, sizeof(desc));
	desc.dwSize = sizeof(DMUS_OBJECTDESC);
	desc.guidClass = CLSID_DirectMusicSegment;
	desc.dwValidData |= (DMUS_OBJ_CLASS | DMUS_OBJ_FILENAME | DMUS_OBJ_FULLPATH);
	wcscpy(desc.wszFileName,gwszMediaPath);
	hr = pwLoader8->GetObject(&desc,CTIID_IDirectMusicSegment8,(void **)&pwSegment8);
	if(FAILED(hr))
	{
		fnsLog(ABORTLOGLEVEL, TEXT("**** Loader8->GetObject(pwSegment8) ")
				TEXT("failed (%s == %08Xh)"),
				tdmXlatHRESULT(hr), hr);
		goto TEST_END;
	}
	// **********************************************************************
	// 2.2) Get the AudioPath wih fullpath
	// **********************************************************************
	dmthSetPath(pAudPathFile);
	ZeroMemory(&desc, sizeof(desc));
	desc.dwSize = sizeof(DMUS_OBJECTDESC);
	desc.guidClass = CLSID_DirectMusicAudioPathConfig;
	desc.dwValidData |= (DMUS_OBJ_CLASS | DMUS_OBJ_FILENAME | DMUS_OBJ_FULLPATH);
	wcscpy(desc.wszFileName,gwszMediaPath);
	hr = pwLoader8->GetObject(&desc,CTIID_IUnknown,(void **)&pAudioPathConfig);
	if(FAILED(hr))
	{
		fnsLog(ABORTLOGLEVEL, TEXT("**** Loader8->GetObject(pAudioPathConfig) ")
				TEXT("failed (%s == %08Xh)"),
				tdmXlatHRESULT(hr), hr);
		goto TEST_END;
	}
	// **********************************************************************
	// 3.0) Create CtIDirectMusicPerformance8 object 
	// **********************************************************************
	hr = dmthCreatePerformance(IID_IDirectMusicPerformance8, &pwPerformance8);
	if(FAILED(hr))
	{
		fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: IDirectMusicPerformance8 object creation "),
				TEXT("failed (%s == %08Xh)"),
				tdmXlatHRESULT(hr), hr);
		goto TEST_END;
	}
	// **********************************************************************
	// 3.1) InitAudio the performance witha standard audio path that plays silent
	// **********************************************************************
	hr = pwPerformance8->InitAudio(&pwMusic,NULL,ghWnd,NULL,NULL,NULL,NULL);
	if(FAILED(hr))
	{
		fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: IDirectMusicPerformance8->InitAudio ")
				TEXT("failed (%s == %08Xh)"),
				tdmXlatHRESULT(hr), hr);
		goto TEST_END;
	}
	// **********************************************************************
	// 4.0) Create an AudioPath from the audiopathconfig file to be set later
	// **********************************************************************
	hr = pwPerformance8->CreateAudioPath(pAudioPathConfig,TRUE,&pAudioPath);
	if(!HasDSOne(pwMusic) && hardware)
	{
		// CreateAudioPath is expected to fail
		if(SUCCEEDED(hr))
		{
			//failed
			goto TEST_END;
		}
		else
		{
			//passed
			if(hr == E_NOINTERFACE)
			{
				dwRes = FNS_PASS;
			}
			goto TEST_END;
		}
	}
	if(FAILED(hr))
	{
		fnsLog(BUGLOGLEVEL, TEXT("**** ABORT: IDirectMusicPerformance8->CreateAudioPath ")
				TEXT("failed (%s == %08Xh)"),
				tdmXlatHRESULT(hr), hr);
		goto TEST_END;
	}
	// **********************************************************************
	// 4.1) Verify the results
	// **********************************************************************
	if(!pAudioPath)
	{
		fnsLog(BUGLOGLEVEL, TEXT("**** ABORT: pAudioPath is NULL "));
		goto TEST_END;
	}
	// **********************************************************************
	// 5.0) Play the segment - Download
	// **********************************************************************	
	hr = pwSegment8->Download(pwPerformance8);
	if(FAILED(hr))
	{
			fnsLog(BUGLOGLEVEL, TEXT("**** ABORT: pwSegment8->Download ")
				TEXT("failed (%s == %08Xh)"),
				tdmXlatHRESULT(hr), hr);
		goto TEST_END;
	}
	dwRes = FNS_FAIL;
	// **********************************************************************
	// 6) Call SetDefaultAudioPath - the function to be tested<nl>
	// **********************************************************************
	hr = pwPerformance8->SetDefaultAudioPath(pAudioPath);
	if(FAILED(hr))
	{
		fnsLog(BUGLOGLEVEL, TEXT("!!!! BUG: IDirectMusicPerformance8->SetDefaultAudioPath ")
				TEXT("failed (%s == %08Xh)"),
				tdmXlatHRESULT(hr), hr);
		goto TEST_END;
	}
	// **********************************************************************
	// 7.1) Verification - PlaySegmentEx
	// **********************************************************************
	hr = pwPerformance8->PlaySegmentEx( 
		pwSegment8, 		// Segment to play 
		0,NULL, 			// Optional stuff NULL 
		DMUS_SEGF_BEAT, 	// DMUS_SEGF_ flags.
		0,NULL,NULL,NULL);	// Optional stuff NULL - use default audio path 
	if(FAILED(hr))
	{
		fnsLog(ABORTLOGLEVEL, TEXT("!!!! BUG: pwPerformance->PlaySegmentEx ")
			TEXT("failed (%s == %08Xh)"),
			tdmXlatHRESULT(hr), hr);
		goto TEST_END;
	}
	// **********************************************************************
	// 7.2) Verification - Wait for IsPlaying - should make sound this time
	// **********************************************************************
	counter = 0;
	SleepTime = 1000;
	while ((hr = pwPerformance8->IsPlaying(pwSegment8, NULL)) != S_OK)	
	{
		Sleep(SleepTime);
		if(counter++ > TimeOutValue)
		{
			fnsLog(BUGLOGLEVEL, TEXT("!!!! BUG: IsPlaying() timed out\n")
				TEXT("Segment did not start playing in the time specified (%s == %08Xh)"),
				tdmXlatHRESULT(hr), hr);
			goto TEST_END;
		}
	}
	dwRes = FNS_PASS;

TEST_END:
	// **********************************************************************
	// 8) Cleanup objects and uninitialize and Return results
	// *********************************************************************
	if(pwSegment8 && pwPerformance8)
	{
		pwSegment8->Unload(pwPerformance8); // don't care about return code
	}
	if(pwPerformance8)
	{
		pwPerformance8->CloseDown();
		pwPerformance8->Release();
		pwPerformance8 = NULL;
	}
	if(pwSegment8)
	{
		pwSegment8->Release();
		pwSegment8 = NULL;
	}
	if(pwLoader8)
	{
		pwLoader8->Release();
		pwLoader8 = NULL;
	}
	if(pAudioPathConfig)
	{
		pAudioPathConfig->Release();
		pAudioPathConfig = NULL;
	}
	if(pAudioPath)
	{
		pAudioPath->Release();
		pAudioPath = NULL;
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
// @doc IDIRECTMUSICPERFORMANCE8 KCRAVEN
//
// @topic IDirectMusicPerformance8::SetParamHook()_Valid_Test |
//
// Description: <nl>
// Valid test function for IDirectMusicPerformance8::SetParamHook()<nl>
//
// Test Function: <nl>
// tdmperv8SetParamHook()<nl>
//
// Parameters: <nl>
//	BOOL	fCoCreate	- unused <nl>
//	BOOL	fMaxDebug	- unused <nl>
//	DWORD	dwParam1	- unused <nl>
//	DWORD	dwParam2	- unused <nl>
//
// File : tdmperv8.cpp<nl>
//
// Steps: <nl>
// 1) Initialize <nl>
// 2) Clean up Objects, uninitialize, and return results <nl>
//
// History: <nl>
//	03/15/2000 - kcraven - created<nl>
//
// @index topic | IDIRECTMUSICPERFORMANCE8
//--------------------------------------------------------------------------;
/*
cdDWORD tdmperv8SetParamHook(
	BOOL fCoCreate,
	BOOL fMaxDebug,
	LPARAM dwParam1,
	LPARAM dwParam2)
{
	return tdmhookGetParam (fCoCreate, fMaxDebug, dwParam1, dwParam2);
}	
*/




//--------------------------------------------------------------------------;
// @doc IDIRECTMUSICPERFORMANCE8 KCRAVEN
//
// @topic IDirectMusicPerformance8::StopEx()_Valid_Test |
//
// Description: <nl>
// Valid test function for IDirectMusicPerformance8::StopEx()<nl>
//
// Test Function: <nl>
// tdmperv8StopEx()<nl>
//
// Parameters: <nl>
//	BOOL	fCoCreate	- unused <nl>
//	BOOL	fMaxDebug	- unused <nl>
//	DWORD	dwParam1	- unused <nl>
//	DWORD	dwParam2	- unused <nl>
//
// File : tdmperv8.cpp<nl>
//
// Steps: <nl>
//	 1) Initialize<nl>
//	 2) Create a Performance8<nl>
//	 3.0) Create a Loader<nl>
//	 3.1) Get the Segment object with fullpath<nl>
//	 4.0) Init the Performance8 as level 1<nl>
//	 4.1) Add the default Port<nl>
//	 4.2) Turn on AutoDownload<nl>
//	 5.0) Play the segment - PlaySegmentEx<nl>
//	 5.1) Wait for IsPlaying to return true (or time out)<nl>
//	 6) Call StopEx - the method we are testing<nl>
//	 7) Verification - Wait for IsPlaying to return false (or time out)<nl>
//	 8) Cleanup objects and uninitialize and Return results<nl>
//
// History: <nl>
//	03/15/2000 - kcraven - created<nl>
//
// @index topic | IDIRECTMUSICPERFORMANCE8
//--------------------------------------------------------------------------;
DWORD tdmperv8StopEx(
	BOOL fCoCreate,
	BOOL fMaxDebug,
	LPARAM dwParam1,
	LPARAM dwParam2)
{
	DWORD						dwRes			= FNS_ABORTED;
	HRESULT 					hr				= S_OK;

	WCHAR*						pMidiFile	 	= NULL;
//	WCHAR*						pSongFile	 	= NULL;

//	WCHAR*						wchPath 		= gwszBasePath;
	WCHAR*						wchFile 		= gwszMediaPath;

	CtIDirectMusicLoader8		*pwLoader		= NULL;
	CtIDirectMusicPerformance8	*pwPerformance8 = NULL;
	CtIDirectMusicSegmentState	*pwSegmentState	= NULL;
	CtIDirectMusicSegment		*pwSegment		= NULL;
//	CtIDirectMusicSong			*pwSong			= NULL;
	CtIDirectMusicAudioPath		*pwPath			= NULL;

	DMUS_OBJECTDESC 			desc; 

	int 						counter 		= 0;
	int 						SleepTime		= 3000;
	int 						TimeOutValue	= 10;

	DWORD						dwPathType		= DMUS_APATH_SHARED_STEREOPLUSREVERB;
	DWORD						dwPChannelCount	= 16;
	BOOL						fActivate		= TRUE;
	DWORD						dwFlags			= DMUS_SEGF_MEASURE;

	if(fCoCreate) // use autotransition flag
	{
		dwFlags |= DMUS_SEGF_AUTOTRANSITION;
	}
	DWORD stopthis = (DWORD)dwParam1;
	DWORD testtype = (DWORD)dwParam2;

	switch(testtype)
	{
	default:
		pMidiFile	 	= L"test.mid";
		break;
//	case 1:
//		pSongFile	 	= L"blues.sng";
//		break;
	case 2:
		pMidiFile	 	= L"amaaud1.sgt";
		break;
	}


	// **********************************************************************
	// 1) Initialize
	// **********************************************************************
	hr = dmthCoInitialize(NULL);
	if(FAILED(hr))
	{
		fnsLog(ABORTLOGLEVEL,
			TEXT("**** ABORT:  initialization failed! (%s == %08Xh)"),
			tdmXlatHRESULT(hr), hr);
		goto TEST_END;
	}
	// **********************************************************************
	// 2) Create a Performance8
	// **********************************************************************
	hr = dmthCreatePerformance(IID_IDirectMusicPerformance8, &pwPerformance8);
	if(FAILED(hr))
	{
		fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: IDirectMusicPerformance8 object creation "),
			TEXT("failed (%s == %08Xh)"),
			tdmXlatHRESULT(hr), hr);
		goto TEST_END;
	}
	// **********************************************************************
	// 3.0) Create a Loader
	// **********************************************************************
	hr = dmthCreateLoader(IID_IDirectMusicLoader, &pwLoader);
	if (FAILED(hr))
	{
		fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: CoCreate Loader object ")
				TEXT("failed (%s == %08Xh)"),
				tdmXlatHRESULT(hr), hr);
		goto TEST_END;
	}
	// **********************************************************************
	// 3.1) Get the Segment object with fullpath
	// **********************************************************************
	if(pMidiFile)
	{
		dmthSetPath(pMidiFile);
		ZeroMemory(&desc, sizeof(desc));
		desc.dwSize = sizeof(DMUS_OBJECTDESC);
		desc.guidClass = CLSID_DirectMusicSegment;
		desc.dwValidData |= (DMUS_OBJ_CLASS | DMUS_OBJ_FILENAME | DMUS_OBJ_FULLPATH);
		wcscpy(desc.wszFileName,wchFile);
		hr = pwLoader->GetObject(&desc,CTIID_IDirectMusicSegment,(void **)&pwSegment);
	}
//	else if(pSongFile)
//	{
//		dmthSetPath(pSongFile);
//		ZeroMemory(&desc, sizeof(desc));
//		desc.dwSize = sizeof(DMUS_OBJECTDESC);
//		desc.guidClass = CLSID_DirectMusicSong;
//		desc.dwValidData |= (DMUS_OBJ_CLASS | DMUS_OBJ_FILENAME | DMUS_OBJ_FULLPATH);
//		wcscpy(desc.wszFileName,wchFile);
//		hr = pwLoader->GetObject(&desc,CTIID_IDirectMusicSong8,(void **)&pwSong);
//	}
	if(FAILED(hr))
	{
		fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: Loader->GetObject(pwSegment) ")
				TEXT("failed (%s == %08Xh)"),
				tdmXlatHRESULT(hr), hr);
		goto TEST_END;
	}
	// **********************************************************************
	// 4.0) Init the Performance8 as level 1
	// **********************************************************************	
	hr = pwPerformance8->InitAudio(NULL,NULL,ghWnd,NULL,NULL,NULL,NULL);
	if(FAILED(hr))
	{
		fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: IDirectMusicPerformance8->InitAudio ")
				TEXT("failed (%s == %08Xh)"),
				tdmXlatHRESULT(hr), hr);
		goto TEST_END;
	}
	// **********************************************************************
	// 4.1) Add the default Port
	// **********************************************************************	
	hr = pwPerformance8->CreateStandardAudioPath(
		dwPathType, 			// Choice of standard audio path.
		dwPChannelCount,	// How many pchannels to create.
		fActivate,
		&pwPath);
	if(FAILED(hr))
	{
		fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: pwPerformance->CreateStandardAudioPath() ")
				TEXT("failed (%s == %08Xh)"),
				tdmXlatHRESULT(hr), hr);
		goto TEST_END;
	}
	if(!pwPath)
	{
		fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: AudioPath is NULL "));
		goto TEST_END;
	}
	// **********************************************************************
	// 4.2) Turn on AutoDownload
	// **********************************************************************	
	BOOL fAuto;
	fAuto = TRUE;
	hr = pwPerformance8->SetGlobalParam( GUID_PerfAutoDownload, &fAuto, sizeof(BOOL) );
	if(FAILED(hr))
	{
		fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: Performance8->SetGlobalParam(AutoDownload == TRUE) ")
				TEXT("failed (%s == %08Xh)"),
				tdmXlatHRESULT(hr), hr);
		goto TEST_END;
	}
	// **********************************************************************
	// 5.0) Play the segment - PlaySegmentEx
	// **********************************************************************
	if(pwSegment)
	{
		hr = pwPerformance8->PlaySegmentEx(pwSegment,0,NULL,DMUS_SEGF_BEAT,0,&pwSegmentState,NULL,pwPath); 
	}
//	else if(pwSong)
//	{
//		hr = pwPerformance8->PlaySegmentEx(pwSong,0,NULL,DMUS_SEGF_BEAT,0,&pwSegmentState,NULL,pwPath); 
//	}
	if(FAILED(hr))
	{
		fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: Performance8->PlaySegment ")
				TEXT("failed (%s == %08Xh)"),
				tdmXlatHRESULT(hr), hr);
		goto TEST_END;
	}
	// **********************************************************************
	// 5.1) Wait for IsPlaying to return true (or time out)
	// **********************************************************************
	counter = 0;
	SleepTime = 1000;
	while ((hr = pwPerformance8->IsPlaying(NULL,pwSegmentState)) != S_OK)	
	{
		Sleep(SleepTime);
		if(counter++ > TimeOutValue)
		{
			fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: PlaySegment() timed out\n")
				TEXT("Segment did not start playing in the time speciied (%s == %08Xh)"),
				tdmXlatHRESULT(hr), hr);
			goto TEST_END;
		}
	}
	Sleep(1000);
	dwRes = FNS_FAIL;
	// **********************************************************************
	// 6) Call StopEx - the method we are testing
	// **********************************************************************
	if(stopthis == 1) // audiopath
	{
		hr = pwPerformance8->StopEx(
			pwPath,			// Segstate, AudioPath, Segment, or Song to stop. 
			0,					// Optional time.
			dwFlags); // Standard SEGF flags for setting time.
	}
	else if(stopthis == 2) // segmentstate
	{
		hr = pwPerformance8->StopEx(
			pwSegmentState,			// Segstate, AudioPath, Segment, or Song to stop. 
			0,					// Optional time.
			dwFlags); // Standard SEGF flags for setting time.
	}
	else if(pwSegment)
	{
		hr = pwPerformance8->StopEx(
			pwSegment,			// Segstate, AudioPath, Segment, or Song to stop. 
			0,					// Optional time.
			dwFlags); // Standard SEGF flags for setting time.
	}
//	else if(pwSong)
//	{
//		hr = pwPerformance8->StopEx(
//			pwSong,				// Segstate, AudioPath, Segment, or Song to stop. 
//			0,					// Optional time.
//			dwFlags); // Standard SEGF flags for setting time.
//	}
	if(FAILED(hr))
	{
		fnsLog(BUGLOGLEVEL, TEXT("!!!! BUG: Performance8->StopEx ")
				TEXT("failed (%s == %08Xh)"),
				tdmXlatHRESULT(hr), hr);
		goto TEST_END;
	}
	// **********************************************************************
	// 7) Verification - Wait for IsPlaying to return false (or time out)
	// **********************************************************************
	counter = 0;
	SleepTime = 1000;
	while ((hr = pwPerformance8->IsPlaying(NULL,pwSegmentState)) == S_OK)	
	{
		Sleep(SleepTime);
		if(counter++ >TimeOutValue)
		{
			fnsLog(BUGLOGLEVEL, TEXT("!!!! BUG: Performance8->IsPlaying() is true\n")
				TEXT("Segment did not stop playing in the time specified (%s == %08Xh)"),
				tdmXlatHRESULT(hr), hr);
			goto TEST_END;
		}
	}
	dwRes = FNS_PASS;

TEST_END:
	// *********************************************************************
	// 8) Cleanup objects and uninitialize and Return results
	// *********************************************************************
	if(pwPerformance8)
	{
		pwPerformance8->CloseDown();
		pwPerformance8->Release();
		pwPerformance8 = NULL;
	}
	if(pwPath)
	{
		pwPath->Release();
		pwPath = NULL;
	}
	if(pwLoader)
	{
		pwLoader->Release();
		pwLoader = NULL;
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
//	if(pwSong)
//	{
//		pwSong->Release();
//		pwSong = NULL;
//	}

	dmthCoUninitialize();	
	return dwRes;
}	




//--------------------------------------------------------------------------;
// @doc IDIRECTMUSICPERFORMANCE8 KCRAVEN
//
// @topic IDirectMusicPerformance8::PlaySegmentEx()_Valid_Test |
//
// Description: <nl>
// Valid test function for IDirectMusicPerformance8::PlaySegmentEx()<nl>
//
// Test Function: <nl>
// tdmperv8PlaySegmentNoPiano()<nl>
//
// Parameters: <nl>
//  BOOL    fCoCreate   - unused <nl>
//  BOOL    fMaxDebug   - unused <nl>
//  DWORD   dwParam1    - unused <nl>
//	DWORD   dwParam2    - unused <nl>
//
// File : tdmperv8.cpp<nl>
//
// Steps: <nl>
//  1) CoInitialize<nl>
//  2) CoCreatePerformance<nl>
//  3) Create a Notification Event<nl>
//  4) Performance->Init<nl>
//  5) Performance->AddPort<nl>
//  6) CoCreateLoader<nl>
//  7) Set path and scan for everything<nl>
//  8) Loader->LoadObjectFromFile(segment)<nl>
//  9) Segment->Download<nl>
//  10) Performance->SetNotificationHandle<nl>
//  11) Performance->AddNotificationType<nl>
//  12) Performance->PlaySegment<nl>
//  13) Wait for the segment to finish playing<nl>
//  14) Cleanup objects, uninitialize COM, Return test result<nl>
//
// History: <nl>
//  05/16/2000 - kcraven - created<nl>
//
// @index topic | IDIRECTMUSICPERFORMANCE8
//--------------------------------------------------------------------------;
DWORD tdmperv8PlaySegmentNoPiano
(
	BOOL fCoCreate, 
	BOOL fMaxDebug,						
	LPARAM dwParam1, 
	LPARAM dwParam2
)
{
	HRESULT 					hr						= E_NOTIMPL;
	DWORD						dwRes					= FNS_ABORTED;

	CtIDirectMusicPerformance*	pwPerformance			= NULL;
	CtIDirectMusicSegment8*		pwSegment				= NULL;
	CtIDirectMusicLoader8*		pwLoader	 			= NULL;

	DMUS_NOTIFICATION_PMSG		*pEvent 				= NULL;
	HANDLE						hNotify 				= INVALID_HANDLE_VALUE;
	BOOL						endwhile				= FALSE;
	DWORD						dwResult				= 0;

	WCHAR szFilename[] = L"DPrimary2.SGT";

	//Set the path
	dmthSetPath(NULL);
	// **********************************************************************
	// 1) CoInitialize
	// **********************************************************************
	hr = dmthCoInitialize(NULL);
	if(FAILED(hr))
	{
		fnsLog(ABORTLOGLEVEL, TEXT("**** COM initialization failed! (%s == %08Xh)"),
		tdmXlatHRESULT(hr), hr);
		fnsLog(ABORTLOGLEVEL, TEXT("**** Test case aborting."));
		return FNS_ABORTED;
	}
	// **********************************************************************
	// 2)  CoCreatePerformance
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
	// 3)  Create a Notification Event
	// **********************************************************************
	hNotify = CreateEvent( NULL, FALSE, FALSE, NULL );
	if( !hNotify || INVALID_HANDLE_VALUE == hNotify)
	{
		fnsLog(ABORTLOGLEVEL,
			"**** ABORT: hNotify Event failed (hNotify == %08Xh)",
			hNotify);
		goto TEST_END;
	}
	// **********************************************************************
	// 4) Performance->Init
	// **********************************************************************
	hr = pwPerformance->Init(NULL,NULL,ghWnd);
	if (FAILED (hr))
	{
		fnsLog (ABORTLOGLEVEL,
			"**** ABORT: Performance->Init failed (%s == %08Xh)",
			tdmXlatHRESULT(hr), hr);
		goto TEST_END;
	}
	// **********************************************************************
	// 5) Performance->AddPort
	// **********************************************************************
	hr =  pwPerformance->AddPort(NULL);
	if (FAILED (hr))
	{
		fnsLog (ABORTLOGLEVEL,
			"**** ABORT: Performance->AddPort failed (%s == %08Xh)",
			tdmXlatHRESULT(hr), hr);
		goto TEST_END;
	}
	// **********************************************************************
	// 6) CoCreateLoader
	// **********************************************************************
	hr = dmthCreateLoader(IID_IDirectMusicLoader8, &pwLoader);
	if(FAILED(hr))
	{
		fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: DirectMusicLoader object creation ")
				TEXT("failed (%s == %08Xh)"),
				tdmXlatHRESULT(hr), hr);
		dwRes = FNS_ABORTED;
		goto TEST_END;
	}
	// **********************************************************************
	// 7) Set path and scan for everything
	// **********************************************************************
	hr = SearchAndScanAllMedia(pwLoader);
	if (FAILED(hr))
	{
		fnsLog(ABORTLOGLEVEL, TEXT("**** ERROR: SearchAndScanAllMedia failed!!"));
		goto TEST_END;
	}
	// **********************************************************************
	// 8) Loader->LoadObjectFromFile(segment)
	// **********************************************************************
	hr = pwLoader->LoadObjectFromFile(CLSID_DirectMusicSegment,
									CTIID_IDirectMusicSegment8,
									szFilename,
									(void **)&pwSegment);
	if(FAILED(hr))
	{
		fnsLog(ABORTLOGLEVEL, "**** ABORT: dmlLoadSegment "
				"failed (%s == %08Xh)", tdmXlatHRESULT(hr), hr);
		goto TEST_END;
	}
	// **********************************************************************
	// 9) Segment->Download
	// **********************************************************************
	hr = pwSegment->Download(pwPerformance);
	if(FAILED(hr))
	{
		fnsLog(ABORTLOGLEVEL, "**** ABORT: pwSegment->Download(performance) "
				"failed (%s == %08Xh)", tdmXlatHRESULT(hr), hr);
		goto TEST_END;
	}
	// **********************************************************************
	// 10) Performance->SetNotificationHandle
	// **********************************************************************
	hr = pwPerformance->SetNotificationHandle(hNotify,0);
	if(FAILED(hr))
	{
		fnsLog(BUGLOGLEVEL,
			"!!!! BUG: pwPerformance->SetNotificationHandle (valid event)"
				"failed (%s == %08Xh)",
				tdmXlatHRESULT(hr), hr);
		goto TEST_END;
	}
	// **********************************************************************
	// 11) Performance->AddNotificationType
	// **********************************************************************
	hr = pwPerformance->AddNotificationType(GUID_NOTIFICATION_SEGMENT);
	if(FAILED(hr))
	{
		fnsLog(ABORTLOGLEVEL,
			"!!!! BUG: pwPerformance->AddNotificationType(GUID_NOTIFICATION_SEGMENT) "
			"failed (%s == %08Xh)",
			tdmXlatHRESULT(hr), hr);
		goto TEST_END;
	}
	//BUGBUG
	// hack to insure download has finished
	Sleep(1000);

	dwRes = FNS_FAIL;
	// **********************************************************************
	// 12) Performance->PlaySegment
	// **********************************************************************
	hr = pwPerformance->PlaySegment(pwSegment, DMUS_SEGF_BEAT, 0, NULL);
	if(FAILED(hr))
	{
		fnsLog(ABORTLOGLEVEL, "**** ABORT: PlaySegment "
				"failed (%s == %08Xh)", tdmXlatHRESULT(hr), hr);
		goto TEST_END;
	}
	// **********************************************************************
	// 13) Wait for the segment to finish playing
	// **********************************************************************
	endwhile = FALSE;
	while(FALSE == endwhile)
	{
		dwResult = WaitForSingleObject(hNotify,20000); //20 seconds
		if(WAIT_OBJECT_0 != dwResult)
		{
			fnsLog(ABORTLOGLEVEL,
				"**** ABORT: WaitForSingleObject != "
				"WAIT_OBJECT_0 (%08Xh)",dwResult);
			endwhile = TRUE;
		}
		while(SUCCEEDED(hr = pwPerformance->GetNotificationPMsg(&pEvent)) && (hr != S_FALSE))
		{
			if(!pEvent)
			{
				continue;
			}

			if(pEvent->guidNotificationType == GUID_NOTIFICATION_SEGMENT)
			{
				switch(pEvent->dwNotificationOption)
				{
				case DMUS_NOTIFICATION_SEGSTART:
					fnsLog(FYILOGLEVEL,"--- FYI: Performance PMsg: DMUS_NOTIFICATION_SEGSTART");
					break;
				case DMUS_NOTIFICATION_SEGEND:
					fnsLog(FYILOGLEVEL,"--- FYI: Performance PMsg: DMUS_NOTIFICATION_SEGEND");
					endwhile = TRUE;
					break;
				case DMUS_NOTIFICATION_SEGALMOSTEND:
					fnsLog(FYILOGLEVEL,"--- FYI: Performance PMsg: DMUS_NOTIFICATION_SEGALMOSTEND");
					break;
				case DMUS_NOTIFICATION_SEGLOOP:
					fnsLog(FYILOGLEVEL,"--- FYI: Performance PMsg: DMUS_NOTIFICATION_SEGLOOP");
					break;
				default:
					fnsLog(FYILOGLEVEL,"--- FYI: Performance PMsg: UNKNOWN");
					endwhile = TRUE;
					break;
				}
			}
			else
			{
				fnsLog(BUGLOGLEVEL,"!!!! BUG: Unexpected Performance PMsg: NotificationType: UNKNOWN");
				goto TEST_END;
			}
			hr = pwPerformance->FreePMsg( (DMUS_PMSG*)pEvent );
			pEvent = NULL;
			if(FAILED(hr))
			{
				fnsLog(BUGLOGLEVEL, TEXT("!!!! BUG: pwPerformance->FreePMsg ")
						TEXT("failed (%s == %08Xh)"),
						tdmXlatHRESULT(hr), hr);
				goto TEST_END;
			}
		}
		if(FAILED(hr))
		{
		  fnsLog(BUGLOGLEVEL, TEXT("!!!! BUG: pwPerformance->GetNotificationPMsg ")
					TEXT("failed (%s == %08Xh)"),
					tdmXlatHRESULT(hr), hr);
			goto TEST_END;
		}
	}
	dwRes = FNS_PASS;

TEST_END:
	// **********************************************************************
	// 14) Cleanup objects, uninitialize COM, Return test result
	// **********************************************************************
	if (pwPerformance)
	{
		if(pwSegment)
		{
			pwSegment->Unload(pwPerformance);
		}
		pwPerformance->SetNotificationHandle(0,0);
		if(pEvent)
		{
			pwPerformance->FreePMsg( (DMUS_PMSG*)pEvent );
		}
		pwPerformance->Stop(NULL, NULL, 0, 0);
		pwPerformance->CloseDown();
	}
	if(INVALID_HANDLE_VALUE != hNotify)
	{
		CloseHandle(hNotify);
	}

	pwPerformance	= SafeRelease (pwPerformance);
	pwSegment		= SafeRelease (pwSegment);
	pwLoader		= SafeRelease (pwLoader);

	dmthCoUninitialize();
	return dwRes;
}





//--------------------------------------------------------------------------;
// @doc IDIRECTMUSICSONG DANHAFF
//
// File : tdmperfv.cpp
//
// History: <nl>
//  07/18/2000 - danhaff - created <nl>
//
// @index topic | IDIRECTMUSICPERFORMANCE


//  Regression for ManBug 39879

// ReproSteps:
// 1. Create a DirectSound.
// 2. Create a DirectMusic performance.
// 3. Pass the DirectSound object into InitAudio.
// 4. Play a WAV file through DirectMusic.
// 5. Stop and release the WAV segment.
// 6. Release the performance. (Access violation occurs here)

//--------------------------------------------------------------------------;
DWORD tdmperfvInitAudioPlayWave(
    BOOL fCoCreate,
    BOOL fMaxDebug,
    LPARAM dwParam1,
    LPARAM dwParam2)
{
    DWORD                       dwRes           = FNS_FAIL;
    HRESULT                     hr              = S_OK;

    dmthSetPath(NULL);
    WCHAR   *wchPath                            = gwszMediaPath;
    char    *chPath                             = gszMediaPath;
    CtIDirectMusicLoader8          *ptLoader8   = NULL;
    CtIDirectMusicPerformance8     *ptPerf8     = NULL;
    CtIDirectMusicSegment8         *ptSeg8Wave  = NULL;
    CtIDirectMusicAudioPath        *ptAudioPath = NULL;
    CtIDirectMusic                 *ptDM        = NULL;
    IDirectSound8                  *pDS8         = NULL;
    IDirectSound                   *pDS          = NULL;
    DMUS_OBJECTDESC                desc         = {0};

    // **********************************************************************
    // 1) Initialize
    // **********************************************************************
    hr = dmthCoInitialize(NULL);
    if(FAILED(hr))
    {
        fnsLog(ABORTLOGLEVEL,
            TEXT("**** ABORT:  initialization failed! (%s == %08Xh)"),
            tdmXlatHRESULT(hr), hr);
        goto TEST_END;
    }

// 1. Create a DirectSound8 object
    hr = DirectSoundCreate8(NULL, &pDS8, NULL);
    if (FAILED (hr))
    {
        fnsLog(ABORTLOGLEVEL, TEXT("**** DirectSoundCreate failed. %s (%08Xh)"),
            tdmXlatHRESULT(hr), hr);
        goto TEST_END;
    }

  //Get the DS1 interface from it.
    hr = pDS8->QueryInterface(IID_IDirectSound, (void **)&pDS);
    if (FAILED (hr))
    {
        fnsLog(ABORTLOGLEVEL, TEXT("**** QI for IDirectSound1 failed. %s (%08Xh)"),
            tdmXlatHRESULT(hr), hr);
        goto TEST_END;
    }
    
    //Release the DS8 interface.
    SAFE_RELEASE(pDS8);


    //Set Cooperative level on it.
    pDS->SetCooperativeLevel(ghWnd, DSSCL_PRIORITY);
    if (FAILED (hr))
    {
        fnsLog(ABORTLOGLEVEL, TEXT("**** SetCooperativeLevel failed with %s (%08Xh)"),
            tdmXlatHRESULT(hr), hr);
        goto TEST_END;
    }

// 2. Create a DirectMusic performance.
    hr = dmthCreatePerformance (IID_IDirectMusicPerformance8, &ptPerf8);
    if (FAILED (hr))
    {
        fnsLog(ABORTLOGLEVEL, TEXT("**** dmthCreatePerformance (IID_IDirectMusicPerformance8failed! (%s == %08Xh)"),
            tdmXlatHRESULT(hr), hr);
        goto TEST_END;
    }

// 2.3 Create a DirectMusic object.
    hr = dmthCreateDMBaseObj(IID_IDirectMusic, &ptDM);
    if (FAILED (hr))
    {
        fnsLog(ABORTLOGLEVEL, TEXT("**** dmthCreatePerformance (IID_IDirectMusicPerformance8failed! (%s == %08Xh)"),
            tdmXlatHRESULT(hr), hr);
        goto TEST_END;
    }



//3. Pass the DirectSound object into InitAudio. (don't know what path he's using)
//    hr = ptPerf8->InitAudio (NULL, &pDS, ghWnd, DMUS_APATH_DYNAMIC_3D, 16, DMUS_AUDIOF_ALL, NULL);
    hr = ptPerf8->InitAudio (NULL, &pDS, ghWnd, DMUS_APATH_SHARED_STEREOPLUSREVERB, 16, DMUS_AUDIOF_ALL, NULL);
    if (FAILED (hr))
    {
        fnsLog(ABORTLOGLEVEL, TEXT("**** ptPerformance->InitAudio failed! (%s == %08Xh)"),
            tdmXlatHRESULT(hr), hr);
        goto TEST_END;
    }

    // Create a loader
    hr = dmthCreateLoader (IID_IDirectMusicLoader8, &ptLoader8);
    if (FAILED (hr))
    {
        fnsLog(ABORTLOGLEVEL, TEXT("**** dmthCreateLoader failed! (%s == %08Xh)"),
            tdmXlatHRESULT(hr), hr);
        goto TEST_END;
    }

    //This call should always succeed.
    hr = ptLoader8->SetSearchDirectory(GUID_DirectMusicAllTypes, gwszMediaPath, FALSE);
    if (FAILED(hr))
    {
        fnsLog(ABORTLOGLEVEL, "**** ERROR: IDirectMusicLoader8::SetSearchDirectory failed!!");
        goto TEST_END;
    }


    // Do a stupid scan of the directory since the loader can't figure this out for itself
    hr = ptLoader8->ScanDirectory(CLSID_DirectMusicSegment, L"*", NULL);
    if (FAILED (hr))
    {
        fnsLog(ABORTLOGLEVEL, TEXT("**** ptLoader->ScanDirectory failed! (%s == %08Xh)"),
            tdmXlatHRESULT(hr), hr);
        goto TEST_END;
    }



	// **********************************************************************
	// 3) Get the AudioPath and Segment8
	// **********************************************************************
	ZeroMemory(&desc, sizeof(desc));
	desc.dwSize = sizeof(DMUS_OBJECTDESC);
	desc.guidClass = CLSID_DirectMusicSegment;
	desc.dwValidData |= (DMUS_OBJ_CLASS | DMUS_OBJ_FILENAME);
	wcscpy(desc.wszFileName,L"PortBVT1.wav");

	hr = ptLoader8->GetObject(&desc, CTIID_IDirectMusicSegment8,(void **)&ptSeg8Wave);
	if(FAILED(hr))
	{
		fnsLog(ABORTLOGLEVEL, TEXT("**** Loader8->GetObject(ptSeg8Wave) ")
				TEXT("failed (%s == %08Xh)"),
				tdmXlatHRESULT(hr), hr);
		goto TEST_END;
	}

  
    hr = ptPerf8->GetDefaultAudioPath(&ptAudioPath);
	if(FAILED(hr))
	{
		fnsLog(ABORTLOGLEVEL, TEXT("**** Loader8->GetDefaultAudioPath")
				TEXT("failed (%s == %08Xh)"),
				tdmXlatHRESULT(hr), hr);
		goto TEST_END;
	}

    //Download this so we can hear it.
    hr = ptSeg8Wave->Download(ptAudioPath);
    if (hr != S_OK)
    {
        fnsLog(ABORTLOGLEVEL, TEXT("**** Download returned %s (%08Xh) instead of S_OK"),
            tdmXlatHRESULT(hr), hr);
        goto TEST_END;
    }

// 4. Play a WAV file through DirectMusic.
    hr = ptPerf8->PlaySegmentEx(ptSeg8Wave, NULL, NULL, DMUS_SEGF_DEFAULT, 0, NULL, NULL, ptAudioPath);
    if (hr != S_OK)
    {
        fnsLog(ABORTLOGLEVEL, TEXT("**** PlaySegmentEx returned %s (%08Xh) instead of S_OK"),
            tdmXlatHRESULT(hr), hr);
        goto TEST_END;
    }

//Wait a second.
    Sleep(1500);

// 5. Stop and release the WAV segment.
    hr = ptPerf8->Stop(ptSeg8Wave, NULL, 0, 0);
    if (hr != S_OK)
    {
        fnsLog(ABORTLOGLEVEL, TEXT("**** StopEx returned %s (%08Xh) instead of S_OK"),
            tdmXlatHRESULT(hr), hr);
        goto TEST_END;
    }

// 6. Release the performance. (Access violation occurs here)
    if (ptPerf8)
    {
        ptPerf8->CloseDown();
        SAFE_RELEASE(ptPerf8);
    }

    dwRes = FNS_PASS;

TEST_END:

    //Unload this so we don't leak.
    if (ptSeg8Wave && ptAudioPath)
    {
        hr = ptSeg8Wave->Unload(ptAudioPath);
        if (hr != S_OK)
        {
            fnsLog(ABORTLOGLEVEL, TEXT("**** LEAK!!!returned %s (%08Xh) instead of S_OK"),tdmXlatHRESULT(hr), hr);
        }
    }

    // **********************************************************************
    // 18) Cleanup objects and uninitialize and Return results
    // *********************************************************************
    if (ptPerf8)
    {
        ptPerf8->CloseDown();
        SAFE_RELEASE(ptPerf8);
    }

    SAFE_RELEASE(ptLoader8);
    SAFE_RELEASE(ptSeg8Wave);
    SAFE_RELEASE(ptAudioPath);
    SAFE_RELEASE(ptDM);
    SAFE_RELEASE(pDS8);
    SAFE_RELEASE(pDS);
    dmthCoUninitialize();
    return dwRes;
}
//--------------------------------------------------------------------------;
// @doc IDIRECTMUSICSONG DANHAFF
//
// File : tdmperfv.cpp
//
// History: <nl>
//  07/18/2000 - danhaff - created <nl>
//
// @index topic | IDIRECTMUSICPERFORMANCE


//  Regression for ManBug 39879

// ReproSteps:
// 1. Create a DirectSound.
// 2. Create a DirectMusic performance.
// 3. Pass the DirectSound object into InitAudio.
// 4. Play a WAV file through DirectMusic.
// 5. Stop and release the WAV segment.
// 6. Release the performance. (Access violation occurs here)

//--------------------------------------------------------------------------;
DWORD tdmperfvInitAudioPlayWaveLoop(
    BOOL fCoCreate,
    BOOL fMaxDebug,
    LPARAM dwParam1,
    LPARAM dwParam2)
{
    DWORD                       dwRes           = FNS_FAIL;
    HRESULT                     hr              = S_OK;

    dmthSetPath(NULL);
    WCHAR   *wchPath                            = gwszMediaPath;
    char    *chPath                             = gszMediaPath;
    CtIDirectMusicLoader8          *ptLoader8   = NULL;
    CtIDirectMusicPerformance8     *ptPerf8     = NULL;
    CtIDirectMusicSegment8         *ptSeg8Wave  = NULL;
    CtIDirectMusicAudioPath        *ptAudioPath = NULL;

    DMUS_OBJECTDESC                desc         = {0};

	DMUS_CURVE_PMSG*			pCurve;

    // **********************************************************************
    // 1) Initialize
    // **********************************************************************
    hr = dmthCoInitialize(NULL);
    if(FAILED(hr))
    {
        fnsLog(ABORTLOGLEVEL,
            TEXT("**** ABORT:  initialization failed! (%s == %08Xh)"),
            tdmXlatHRESULT(hr), hr);
        goto TEST_END;
    }

    hr = dmthCreatePerformance (IID_IDirectMusicPerformance8, &ptPerf8);
    if (FAILED (hr))
    {
        fnsLog(ABORTLOGLEVEL, TEXT("**** dmthCreatePerformance (IID_IDirectMusicPerformance8failed! (%s == %08Xh)"),
            tdmXlatHRESULT(hr), hr);
        goto TEST_END;
    }

    hr = ptPerf8->InitAudio (NULL, NULL, ghWnd, DMUS_APATH_SHARED_STEREOPLUSREVERB, 16, DMUS_AUDIOF_ALL, NULL);
    if (FAILED (hr))
    {
        fnsLog(ABORTLOGLEVEL, TEXT("**** ptPerformance->InitAudio failed! (%s == %08Xh)"),
            tdmXlatHRESULT(hr), hr);
        goto TEST_END;
    }

    hr = dmthCreateLoader (IID_IDirectMusicLoader8, &ptLoader8);
    if (FAILED (hr))
    {
        fnsLog(ABORTLOGLEVEL, TEXT("**** dmthCreateLoader failed! (%s == %08Xh)"),
            tdmXlatHRESULT(hr), hr);
        goto TEST_END;
    }

    hr = ptLoader8->SetSearchDirectory(GUID_DirectMusicAllTypes, gwszMediaPath, FALSE);
    if (FAILED(hr))
    {
        fnsLog(ABORTLOGLEVEL, "**** ERROR: IDirectMusicLoader8::SetSearchDirectory failed!!");
        goto TEST_END;
    }

    hr = ptLoader8->ScanDirectory(CLSID_DirectMusicSegment, L"*", NULL);
    if (FAILED (hr))
    {
        fnsLog(ABORTLOGLEVEL, TEXT("**** ptLoader->ScanDirectory failed! (%s == %08Xh)"),
            tdmXlatHRESULT(hr), hr);
        goto TEST_END;
    }

	// **********************************************************************
	// 3) Get the AudioPath and Segment8
	// **********************************************************************
	ZeroMemory(&desc, sizeof(desc));
	desc.dwSize = sizeof(DMUS_OBJECTDESC);
	desc.guidClass = CLSID_DirectMusicSegment;
	desc.dwValidData |= (DMUS_OBJ_CLASS | DMUS_OBJ_FILENAME);
	wcscpy(desc.wszFileName,L"PortBVT1.wav");
	hr = ptLoader8->GetObject(&desc, CTIID_IDirectMusicSegment8,(void **)&ptSeg8Wave);
	if(FAILED(hr))
	{
		fnsLog(ABORTLOGLEVEL, TEXT("**** Loader8->GetObject(ptSeg8Wave) ")
				TEXT("failed (%s == %08Xh)"),
				tdmXlatHRESULT(hr), hr);
		goto TEST_END;
	}

    hr = ptSeg8Wave->Download(ptPerf8);
	if(FAILED(hr))
	{
		fnsLog(ABORTLOGLEVEL, TEXT("**** ptSeg8Wave->Download")
				TEXT("failed (%s == %08Xh)"),
				tdmXlatHRESULT(hr), hr);
		goto TEST_END;
	}

    hr = ptSeg8Wave->SetRepeats(3); //play 4 time
    if(FAILED(hr))
    {
		fnsLog(ABORTLOGLEVEL, TEXT("**** ptSeg8Wave->SetRepeats")
				TEXT("failed (%s == %08Xh)"),
				tdmXlatHRESULT(hr), hr);
		goto TEST_END;
    }

	//allocpmsg
	hr = ptPerf8->AllocPMsg(sizeof(DMUS_CURVE_PMSG), (DMUS_PMSG**) &pCurve);
	if(FAILED(hr) || !pCurve)
	{
		fnsLog(BUGLOGLEVEL,TEXT("**** ABORT: IDMPerformance::AllocPMsg failed (%s == %08Xh)"),
			tdmXlatHRESULT(hr), hr);
		goto TEST_END;
	}

	ZeroMemory(pCurve, sizeof(DMUS_CURVE_PMSG));

	pCurve->dwSize = sizeof(DMUS_CURVE_PMSG);
	pCurve->rtTime = 0;
	pCurve->dwFlags = DMUS_PMSGF_DX8 | DMUS_PMSGF_REFTIME | DMUS_PMSGF_LOCKTOREFTIME;  
	pCurve->dwPChannel = DMUS_PCHANNEL_BROADCAST_PERFORMANCE;
	pCurve->dwType = DMUS_PMSGT_CURVE;            
	pCurve->dwGroupID = 0xFFFFFFF;

//	pCurve->mtOriginalStart = 0;
	pCurve->mtDuration = 200;
	pCurve->nStartValue = 0;				
	pCurve->nEndValue = 0;
	pCurve->bCurveShape = DMUS_CURVES_LINEAR;
	pCurve->bCCData = 7;
	pCurve->bFlags = DMUS_CURVE_START_FROM_CURRENT;  
	pCurve->bType = DMUS_CURVET_CCCURVE ;
	pCurve->wMergeIndex = 0;
	pCurve->mtResetDuration = 0;

    hr = ptPerf8->PlaySegmentEx(ptSeg8Wave, NULL, NULL, DMUS_SEGF_DEFAULT, 0, NULL, NULL, NULL);
    if(FAILED(hr))
    {
		ptPerf8->FreePMsg((DMUS_PMSG*) pCurve);
        fnsLog(ABORTLOGLEVEL, TEXT("**** PlaySegmentEx returned %s (%08Xh) instead of S_OK"),
            tdmXlatHRESULT(hr), hr);
        goto TEST_END;
    }
	Sleep(200);
	hr = ptPerf8->SendPMsg((DMUS_PMSG*) pCurve);
    if(FAILED(hr))
    {
		ptPerf8->FreePMsg((DMUS_PMSG*) pCurve);
        fnsLog(ABORTLOGLEVEL, TEXT("**** SendPMsg returned %s (%08Xh) instead of S_OK"),
            tdmXlatHRESULT(hr), hr);
        goto TEST_END;
    }
//Wait a second.
    Sleep(5000);

    dwRes = FNS_PASS;

TEST_END:
    // **********************************************************************
    // 18) Cleanup objects and uninitialize and Return results
    // *********************************************************************
    if (ptPerf8 && ptSeg8Wave)
    {
		ptPerf8->Stop(NULL, NULL, 0, 0);
	    ptSeg8Wave->Unload(ptPerf8);
	}
    if (ptPerf8)
    {
        ptPerf8->CloseDown();
        SAFE_RELEASE(ptPerf8);
    }

    SAFE_RELEASE(ptLoader8);
    SAFE_RELEASE(ptSeg8Wave);
    SAFE_RELEASE(ptAudioPath);

    dmthCoUninitialize();
    return dwRes;
}




//--------------------------------------------------------------------------;
// @doc IDIRECTMUSICSONG KCRAVEN
//
// File : tdmperfv.cpp
//
// History: <nl>
//  07/18/2000 - kcraven - created <nl>
//
// @index topic | IDIRECTMUSICPERFORMANCE
// ReproSteps:
// 1. Create a DirectSound.
// 2. Create a DirectMusic performance.
// 3. Pass the DirectSound object into InitAudio. (fails!!)

//--------------------------------------------------------------------------;
DWORD tdmperfvInitAudioWithDSound(
    BOOL fCoCreate,
    BOOL fMaxDebug,
    LPARAM dwParam1,
    LPARAM dwParam2)
{
    DWORD                       dwRes           = FNS_FAIL;
    HRESULT                     hr              = S_OK;

    dmthSetPath(NULL);
    WCHAR   *wchPath                            = gwszMediaPath;
    char    *chPath                             = gszMediaPath;
    CtIDirectMusicPerformance8     *ptPerf8     = NULL;
    IDirectSound8                   *pDS8         = NULL;
    IDirectSound                   *pDS         = NULL;

    // **********************************************************************
    // 1) Initialize
    // **********************************************************************
    hr = dmthCoInitialize(NULL);
    if(FAILED(hr))
    {
        fnsLog(ABORTLOGLEVEL,
            TEXT("**** ABORT:  initialization failed! (%s == %08Xh)"),
            tdmXlatHRESULT(hr), hr);
        goto TEST_END;
    }



// 1. Create a DirectSound8
    if (fCoCreate)
    {
        hr = DirectSoundCreate8(NULL, &pDS8, NULL);
        if (FAILED (hr))
        {
            fnsLog(ABORTLOGLEVEL, TEXT("**** DirectSoundCreate failed. %s (%08Xh)"),
                tdmXlatHRESULT(hr), hr);
            goto TEST_END;
        }

        hr = pDS8->QueryInterface(IID_IDirectSound, (void **)&pDS);
        if (FAILED (hr))
        {
            fnsLog(ABORTLOGLEVEL, TEXT("**** QI for DirectSound failed. %s (%08Xh)"),
                tdmXlatHRESULT(hr), hr);
            goto TEST_END;
        }
    }
    else
    {
        hr = CoCreateInstance(CLSID_DirectSound8,
                              NULL,               // don't support aggregation yet
                              CLSCTX_INPROC_SERVER,
                              IID_IDirectSound,
                              (void**)&pDS);
        if (FAILED (hr))
        {
            fnsLog(ABORTLOGLEVEL, TEXT("**** CoCreate(DSound8) failed with (%08Xh)"),
                tdmXlatHRESULT(hr), hr);
            goto TEST_END;
        }

        hr = pDS->Initialize(NULL);
        if (FAILED (hr))
        {
            fnsLog(ABORTLOGLEVEL, TEXT("**** CDSound8->Initialize failed with %s (%08Xh)"),
                tdmXlatHRESULT(hr), hr);
            goto TEST_END;
        }


    }

//
    hr = pDS->SetCooperativeLevel(ghWnd, DSSCL_PRIORITY);
    if (FAILED (hr))
    {
        fnsLog(ABORTLOGLEVEL, TEXT("**** CDSound8->SetCooperativeLevel failed with %s (%08Xh)"),
            tdmXlatHRESULT(hr), hr);
        goto TEST_END;
    }

// 2. Create a DirectMusic performance.
    hr = dmthCreatePerformance (IID_IDirectMusicPerformance8, &ptPerf8);
    if (FAILED (hr))
    {
        fnsLog(ABORTLOGLEVEL, TEXT("**** dmthCreatePerformance (IID_IDirectMusicPerformance8failed! (%s == %08Xh)"),
            tdmXlatHRESULT(hr), hr);
        goto TEST_END;
    }

//3. Pass the DirectSound object into InitAudio. (don't know what path he's using)
    hr = ptPerf8->InitAudio (NULL, &pDS, ghWnd, DMUS_APATH_SHARED_STEREOPLUSREVERB, 16, 0, NULL);
    if (FAILED (hr))
    {
        fnsLog(ABORTLOGLEVEL, TEXT("**** ptPerformance->InitAudio failed! (%s == %08Xh)"),
            tdmXlatHRESULT(hr), hr);
        goto TEST_END;
    }

    dwRes = FNS_PASS;

TEST_END:
    // **********************************************************************
    // 18) Cleanup objects and uninitialize and Return results
    // *********************************************************************



    if (ptPerf8)
    {
        ptPerf8->CloseDown();
        SAFE_RELEASE(ptPerf8);
    }
    SAFE_RELEASE(pDS8);
    if (pDS)
    {
        pDS->SetCooperativeLevel(ghWnd, DSSCL_NORMAL);
        SAFE_RELEASE(pDS);
    }

    dmthCoUninitialize();
    return dwRes;
}




//--------------------------------------------------------------------------;
// @doc IDIRECTMUSICPERFORMANCE8 KCRAVEN
//
// @topic IDirectMusicPerformance8::InitAudio()_Valid_Test |
//
// Description: <nl>
// Valid test function for IDirectMusicPerformance8::InitAudio()<nl>
//
// Test Function: <nl>
// tdmperv8InitAudio()<nl>
//
// Parameters: <nl>
//	BOOL	fCoCreate	- unused <nl>
//	BOOL	fMaxDebug	- unused <nl>
//	DWORD	dwParam1	- unused <nl>
//	DWORD	dwParam2	- unused <nl>
//
// File : tdmperv8.cpp<nl>
//
// Steps: <nl>
//  1) CoInitialize<nl>
//  2) CoCreatePerformance <nl>
//  3) Performance8->InitAudio<nl>
//  4) Performance8->GetDefaultAudioPath<nl>
//  5) AudioPath->GetObjectInPath(buffer)<nl>
//  6) AudioPath->GetObjectInPath(port)<nl>
//  7) Port->GetFormat<nl>
//  8) Cleanup objects and uninitialize and Return results<nl>
//
// History: <nl>
//	03/15/2000 - kcraven - created<nl>
//
// @index topic | IDIRECTMUSICPERFORMANCE8
//--------------------------------------------------------------------------;
DWORD tdmperv8PChannelInfo(
	BOOL fCoCreate,
	BOOL fMaxDebug,
	LPARAM dwParam1,
	LPARAM dwParam2)
{
	DWORD						dwRes				= FNS_ABORTED;
	HRESULT 					hr					= S_OK;

	CtIDirectMusicPerformance8* pwPerformance8		= NULL;
	CtIDirectMusicAudioPath*	pAudioPath			= NULL;
	CtIDirectMusic* 			pwMusic 			= NULL;

	BOOL						bCheckFormat		= (DWORD)fCoCreate;
	DWORD						dwDefaultPathType	= (DWORD)dwParam1;
	DWORD						dwPChannelCount 	= 64;

	DWORD						dwStage;
	DWORD						dwBuffer;
	GUID						guidObject;
	GUID						guidInterface;
	DWORD						dwIndex 			= 0;
	IDirectMusicPort*			pDMPortPath 		= NULL;
	IDirectMusicPort*			pDMPortInfo	 		= NULL;
	CtIDirectMusicPort*			pwPortInfo	 		= NULL;

	DWORD						dwPChannel			= 0;
	DWORD						dwGroup				= 0;
	DWORD						dwMChannel			= 0;

	// **********************************************************************
	// 1) CoInitialize
	// **********************************************************************
	hr = dmthCoInitialize(NULL);
	if(FAILED(hr))
	{
		fnsLog(ABORTLOGLEVEL,
			TEXT("**** ABORT:  initialization failed! (%s == %08Xh)"),
			tdmXlatHRESULT(hr), hr);
		goto TEST_END;
	}
	// **********************************************************************
	// 2) CoCreatePerformance 
	// **********************************************************************
	hr = dmthCreatePerformance(IID_IDirectMusicPerformance8, &pwPerformance8);
	if(FAILED(hr))
	{
		fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: IDirectMusicPerformance8 object creation "),
				TEXT("failed (%s == %08Xh)"),
				tdmXlatHRESULT(hr), hr);
		goto TEST_END;
	}
	dwRes = FNS_FAIL;
	// **********************************************************************
	// 3) Performance8->InitAudio
	// **********************************************************************
	hr = pwPerformance8->InitAudio(
		&pwMusic,NULL,ghWnd,
		dwDefaultPathType,	
		dwPChannelCount,	
		NULL,		
		NULL);
	if(FAILED(hr))
	{
		fnsLog(BUGLOGLEVEL, TEXT("!!!! BUG: IDirectMusicPerformance8->InitAudio ")
				TEXT("failed (%s == %08Xh)"),
				tdmXlatHRESULT(hr), hr);
		goto TEST_END;
	}
	if(!pwMusic)
	{
		fnsLog(BUGLOGLEVEL, TEXT("!!!! BUG: pwMusic is NULL "));
		goto TEST_END;
	}
	// **********************************************************************
	// 4) Performance8->GetDefaultAudioPath
	// **********************************************************************
	hr = pwPerformance8->GetDefaultAudioPath(&pAudioPath);
	if(FAILED(hr))
	{
		fnsLog(BUGLOGLEVEL, TEXT("!!!! BUG: IDirectMusicPerformance8->GetDefaultAudioPath ")
				TEXT("failed (%s == %08Xh)"),
				tdmXlatHRESULT(hr), hr);
		goto TEST_END;
	}
	if(!pAudioPath)
	{
		fnsLog(BUGLOGLEVEL, TEXT("!!!! BUG: pAudioPath is NULL "));
		goto TEST_END;
	}
	// **********************************************************************
	// 6) AudioPath->GetObjectInPath(port)
	// **********************************************************************
	dwIndex = 0;
	guidInterface = IID_IDirectMusicPort;
	while(S_OK == (hr = pAudioPath->GetObjectInPath(DMUS_PCHANNEL_ALL,DMUS_PATH_PORT,0,GUID_All_Objects,dwIndex++,IID_IDirectMusicPort,(void**)&pDMPortPath)))
	{
		if(pDMPortPath)
		{
			dwPChannel = 0;
			fnsLog(FYILOGLEVEL, TEXT("    ----    pPort->PChannelInfo(%s,%s,%s)"),"dwPChannel","   dwGroup","dwMChannel");
			while(dwPChannel < (dwPChannelCount + 1)) // +1 should be invalid
			{
				hr = pwPerformance8->PChannelInfo(dwPChannel,&pwPortInfo,&dwGroup,&dwMChannel);
				if(FAILED(hr))
				{
					if(dwPChannel < dwPChannelCount) // valid pchannel range for this port 0 - 63
					{
						fnsLog(BUGLOGLEVEL, TEXT("!!!! BUG: pwPerformance8->PChannelInfo ")
							TEXT("failed (%s == %08Xh)"),
							tdmXlatHRESULT(hr), hr);
						goto TEST_END;
					}
					else if(hr != E_INVALIDARG) // result if requested pchannel is out of valid range
					{
						fnsLog(BUGLOGLEVEL, TEXT("!!!! BUG: pwPerformance8->PChannelInfo ")
							TEXT("failed (%s == %08Xh)"),
							tdmXlatHRESULT(hr), hr);
						goto TEST_END;
					}
					else
					{
						hr = S_OK; //test passed with correct HRESULT
					}
				}
				else
				{
					//get real port
					hr = pwPortInfo->GetRealObjPtr(&pDMPortInfo);
					if(FAILED(hr))
					{
						fnsLog(BUGLOGLEVEL, TEXT("!!!! BUG: pwPortInfo->GetRealObj ")
							TEXT("failed (%s == %08Xh)"),
							tdmXlatHRESULT(hr), hr);
						goto TEST_END;
					}
					//log data and compare port pointers
					fnsLog(FYILOGLEVEL, TEXT("    ----    pPort->PChannelInfo(%10d,%10d,%10d)"),dwPChannel,dwGroup,dwMChannel);
					if(pDMPortInfo != pDMPortPath)
					{
						fnsLog(BUGLOGLEVEL, TEXT("!!!! BUG: GetObjectInPath and PChannelInfo did not return the same port!"));
						fnsLog(BUGLOGLEVEL, TEXT("          portpDMPortInfo != pDMPortPath (%p != %p)"),pDMPortInfo,pDMPortPath);
						goto TEST_END;
					}
				}
				if(pwPortInfo) 
				{
					pwPortInfo->Release();
					pwPortInfo = NULL;
				}
				if(pDMPortInfo) 
				{
					pDMPortInfo->Release();
					pDMPortInfo = NULL;
				}
				++dwPChannel;
			}
			// Release the ports
			pDMPortPath->Release();
			pDMPortPath = NULL;
		}
		else
		{
			fnsLog(BUGLOGLEVEL, TEXT("!!!! BUG: PORT NOT FOUND: AudioPath::GetObjectInPath(%ld) succeeded (%s) but pPort is NULL"),dwIndex,tdmXlatHRESULT(hr));
			goto TEST_END;
		}
	}
	if(FAILED(hr) && (DMUS_E_NOT_FOUND != hr))
	{
		fnsLog(BUGLOGLEVEL, TEXT("!!!! BUG: IDirectMusicAudioPath->GetObjectInPath ")
				TEXT("failed (%s == %08Xh)"),
				tdmXlatHRESULT(hr), hr);
		goto TEST_END;
	}
	dwRes = FNS_PASS;

TEST_END:
	// **********************************************************************
	// 8) Cleanup objects and uninitialize and Return results
	// *********************************************************************
	if(pwPerformance8)
	{
		pwPerformance8->CloseDown();
		pwPerformance8->Release();
		pwPerformance8 = NULL;
	}
	if(pwMusic)
	{
		pwMusic->Release();
		pwMusic = NULL;
	}
	if(pDMPortPath)
	{
		pDMPortPath->Release();
		pDMPortPath = NULL;
	}
	if(pDMPortInfo)
	{
		pDMPortInfo->Release();
		pDMPortInfo = NULL;
	}
	if(pwPortInfo)
	{
		pwPortInfo->Release();
		pwPortInfo = NULL;
	}
	if(pAudioPath)
	{
		pAudioPath->Release();
		pAudioPath = NULL;
	}

	dmthCoUninitialize();	
	return dwRes;
}	


//--------------------------------------------------------------------------;
// @doc IDIRECTMUSICPERFORMANCE8 KCRAVEN
//
// @topic IDirectMusicPerformance8::PlaySegmentEx()_Valid_Test |
//
// Description: <nl>
// Valid test function for IDirectMusicPerformance8::PlaySegmentEx().
//
// Test Function: <nl>
// tdmperv8PlayWaveAsSegment <nl>
//
// Parameters: <nl>
//  BOOL    fCoCreate   - unused <nl>
//  BOOL    fMaxDebug   - unused <nl>
//  DWORD   dwParam1    - unused <nl>
//	DWORD   dwParam2    - unused <nl>
//
// File : TDMPERV8.CPP
//
// Steps: <nl>
// 1) Initialize COM. <nl>
// 2) Create CtIDirectMusicLoader object <nl>
// 4) Call Loader->LoadObjectFromFile() to Segment object <nl>
// 5) Call Segment->GetLength() and check results <nl>
// 6) Release objects, CoUninitialize, and return results <nl>
//
// History:
//  09/19/2000 - kcraven - Created
//
// @index topic | IDIRECTMUSICPERFORMANCE8_CASES
//--------------------------------------------------------------------------;
DWORD tdmperv8PlayWaveAsSegment(
	BOOL fCoCreate,
	BOOL fMaxDebug,
	LPARAM dwParam1,
	LPARAM dwParam2)
{
	DWORD						dwRes			= FNS_ABORTED;
	HRESULT						hr				= E_NOTIMPL;
	CtIDirectMusicPerformance8*	pwPerformance	= NULL;
	CtIDirectMusicLoader8*		pwLoader		= NULL;
	CtIDirectMusicSegment8*		pwSegment		= NULL;
	CtIDirectMusicSegmentState*	pwSegmentState	= NULL;
	WCHAR*						pFile			= (WCHAR*)dwParam1;

	int 						counter 		= 0;
	int 						SleepTime		= 1000;
	int 						TimeOutValue	= 10;

	dmthSetPath(NULL);

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
    // 2) Create CtIDirectMusicPerformance8 object 
    // **********************************************************************
    hr = dmthCreatePerformance(IID_IDirectMusicPerformance8, &pwPerformance);
	if(FAILED(hr))
	{
        fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: IDirectMusicPerformance8 object creation "),
                TEXT("failed (%s == %08Xh)"),
                tdmXlatHRESULT(hr), hr);
		goto TEST_END;
	}
    // **********************************************************************
    // 3) Call the function to be tested and Test the function
    // **********************************************************************
	hr = pwPerformance->InitAudio(NULL,NULL,ghWnd,DMUS_APATH_SHARED_STEREOPLUSREVERB,64,0,NULL);
	if(FAILED(hr))
	{
        fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: IDirectMusicPerformance8->InitAudio ")
                TEXT("failed (%s == %08Xh)"),
                tdmXlatHRESULT(hr), hr);
		goto TEST_END;
	}
	// **********************************************************************
    // 3.1) Create Loader
    // **********************************************************************
	hr = dmthCreateLoader(IID_IDirectMusicLoader8, &pwLoader);
	if (FAILED(hr))
	{
        fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: CoCreate Loader object ")
                TEXT("failed (%s == %08Xh)"),
                tdmXlatHRESULT(hr), hr);
		dwRes = FNS_ABORTED;
        goto TEST_END;
	}
	// **********************************************************************
    // 3.2) SetSearchDirectory
    // **********************************************************************
	hr = pwLoader->SetSearchDirectory(GUID_DirectMusicAllTypes,gwszBasePath,FALSE);
	if (FAILED(hr))
	{
		fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: Loader->SetSeachDirectory(media directory) ")
				TEXT("failed (%s == %08Xh)"),
				tdmXlatHRESULT(hr), hr);
		goto TEST_END;
	}
	// **********************************************************************
    // 3.1) Get File
    // **********************************************************************
	hr = pwLoader->LoadObjectFromFile(CLSID_DirectMusicSegment,CTIID_IDirectMusicSegment8,pFile,(void **)&pwSegment);
	if(FAILED(hr))
	{
		fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: Loader->LoadObjectFromFile(pwSegment) ")
				TEXT("failed (%s == %08Xh)"),
				tdmXlatHRESULT(hr), hr);
		goto TEST_END;
	}
    // **********************************************************************
    // 5) Call Segment->Download and check results
    // **********************************************************************
	hr = pwSegment->Download(pwPerformance);
	if(FAILED(hr))
	{
        fnsLog(ABORTLOGLEVEL, TEXT("!!!! BUG: Segment->GetLength ")
                TEXT("failed (%s == %08Xh)"),
                tdmXlatHRESULT(hr), hr);
		dwRes = FNS_FAIL;
		goto TEST_END;
	}
	dwRes = FNS_FAIL;
    // **********************************************************************
    // 5) Call pwPerformance->PlaySegmentEx and check results
    // **********************************************************************
	hr = pwPerformance->PlaySegmentEx( 
		pwSegment,  	// Segment to play. Alternately, could be an IDirectMusicSong. 
		0, 				// If song, which segment in the song. 
		NULL,			// Optional template segment to compose transition with. 
		NULL,			// DMUS_SEGF_ flags.
		0, 				// Time to start playback. 
		&pwSegmentState,			// Returned Segment State. 
		NULL,			// Optional segmentstate or audiopath to replace.
		NULL);			// Optional audioPath to play on. 
	if(FAILED(hr))
	{
        fnsLog(ABORTLOGLEVEL, TEXT("!!!! FAIL: pwPerformance->PlaySegmentEx ")
                TEXT("failed (%s == %08Xh)"),
                tdmXlatHRESULT(hr), hr);
		goto TEST_END;
	}
	counter = 0;
	SleepTime = 1000;
	while ((hr = pwPerformance->IsPlaying(NULL,pwSegmentState)) != S_OK)	
	{
		Sleep(SleepTime);
		if(counter++ > TimeOutValue)
		{
			fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: PlaySegment() timed out\n")
				TEXT("Segment did not start playing in the time speciied (%s == %08Xh)"),
				tdmXlatHRESULT(hr), hr);
			goto TEST_END;
		}
	}
	counter = 0;
	SleepTime = 3000;
	while ((hr = pwPerformance->IsPlaying(NULL,pwSegmentState)) == S_OK)	
	{
		Sleep(SleepTime);
		if(counter++ > TimeOutValue)
		{
			fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: PlaySegment() timed out\n")
				TEXT("Segment did not stop playing in the time speciied (%s == %08Xh)"),
				tdmXlatHRESULT(hr), hr);
			goto TEST_END;
		}
	}
	dwRes = FNS_PASS;

TEST_END:
    // **********************************************************************
    // 6) Release objects, CoUninitialize, and return results
    // **********************************************************************
	if(pwSegment && pwPerformance)
	{
		pwSegment->Unload(pwPerformance);
	}
	if(pwPerformance)
	{
		pwPerformance->CloseDown();
		pwPerformance->Release();
		pwPerformance = NULL;
	}
	if(pwLoader)
	{
		pwLoader->Release();
		pwLoader = NULL;
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








//--------------------------------------------------------------------------;
// @doc IDIRECTMUSICPERFORMANCE8 KCRAVEN
//
// @topic IDirectMusicPerformance8::PlaySegmentEx(RPN)_Valid_Test |
//
// Description: <nl>
// Valid test function for IDirectMusicPerformance8::PlaySegmentEx(RPN)<nl>
//
// Test Function: <nl>
// tdmperv8PlaySegmentExRPN()<nl>
//
// Parameters: <nl>
//	BOOL	fCoCreate	- unused <nl>
//	BOOL	fMaxDebug	- unused <nl>
//	DWORD	dwParam1	- unused <nl>
//	DWORD	dwParam2	- unused <nl>
//
// File : tdmperv8.cpp<nl>
//
// Steps: <nl>
//	 1) Initialize<nl>
//	 2) Create Performance8<nl>
//	 3.0) Create Loader<nl>
//	 3.1) SetSearchDirectory<nl>
//	 3.2) Get the Segment<nl>
//	 4.0) Init the Performance<nl>
//	 4.1) Add the default Port<nl>
//	 4.2) Download<nl>
//	 5) Call PlaySegmentEx - the method to be tested<nl>
//	 6) varification - segment should play<nl>
//	 7) Cleanup objects and uninitialize and Return results<nl>
//
// History: <nl>
//	10/6/2000 - kcraven - created<nl>
//
// @index topic | IDIRECTMUSICPERFORMANCE8
//--------------------------------------------------------------------------;
DWORD tdmperv8PlaySegmentExRPN(
	BOOL fCoCreate,
	BOOL fMaxDebug,
	LPARAM dwParam1,
	LPARAM dwParam2)
{
	DWORD						dwRes			= FNS_ABORTED;
	HRESULT 					hr				= S_OK;

	dmthSetPath(NULL);
	WCHAR*						wchPath 		= gwszBasePath;
//	WCHAR*						wchFile 		= gwszMediaPath;

	CtIDirectMusic*				pwMusic			= NULL;
	CtIDirectMusicLoader8*		pwLoader		= NULL;
	CtIDirectMusicPerformance8*	pwPerformance	= NULL;
	CtIDirectMusicSegmentState*	pwSegmentState1	= NULL;
	CtIDirectMusicSegmentState*	pwSegmentState2	= NULL;
	CtIDirectMusicSegment8*		pwSegment1		= NULL;
	CtIDirectMusicSegment8*		pwSegment2		= NULL;
	CtIDirectMusicAudioPath*	pwPath1			= NULL;
	CtIDirectMusicAudioPath*	pwPath2			= NULL;

	int 						counter 		= 0;
	int 						SleepTime		= 3000;
	int 						TimeOutValue	= 10;

	WCHAR*						pMidiFile1 		= L"rpn0.mid";
	WCHAR*						pMidiFile2 		= L"rpn1.mid";
	DWORD						dwPathType		= DMUS_APATH_DYNAMIC_STEREO;
//	DWORD						dwPathType		= DMUS_APATH_SHARED_STEREOPLUSREVERB;
	DWORD						dwPChannelCount	= 16;
	BOOL						fActivate		= TRUE;
	DWORD						dwFlags			= NULL;
	MUSIC_TIME					mtNow1			= 0;
	MUSIC_TIME					mtNow2			= 0;
	long						lVolume			= 0;

	// **********************************************************************
	// 1) Initialize
	// **********************************************************************
	hr = dmthCoInitialize(NULL);
	if(FAILED(hr))
	{
		fnsLog(ABORTLOGLEVEL,
			TEXT("**** ABORT:  initialization failed! (%s == %08Xh)"),
			tdmXlatHRESULT(hr), hr);
		goto TEST_END;
	}
	// **********************************************************************
	// 2) Create Performance8
	// **********************************************************************
	hr = dmthCreatePerformance(IID_IDirectMusicPerformance8, &pwPerformance);
	if(FAILED(hr))
	{
		fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: IDirectMusicPerformance8 creation "),
			TEXT("failed (%s == %08Xh)"),
			tdmXlatHRESULT(hr), hr);
		goto TEST_END;
	}
	// **********************************************************************
	// 3.0) Create Loader
	// **********************************************************************
	hr = dmthCreateLoader(IID_IDirectMusicLoader8, &pwLoader);
	if (FAILED(hr))
	{
		fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: CoCreate Loader object ")
				TEXT("failed (%s == %08Xh)"),
				tdmXlatHRESULT(hr), hr);
		goto TEST_END;
	}
	// **********************************************************************
	// 3.1) SetSearchDirectory
	// **********************************************************************
	hr = pwLoader->SetSearchDirectory(GUID_DirectMusicAllTypes, wchPath, FALSE);
	if (FAILED(hr))
	{
		fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: Loader->SetSeachDirectory(media directory) ")
				TEXT("failed (%s == %08Xh)"),
				tdmXlatHRESULT(hr), hr);
		goto TEST_END;
	}
	// **********************************************************************
	// 3.2) Get the Segment
	// **********************************************************************
	hr = pwLoader->LoadObjectFromFile(CLSID_DirectMusicSegment,CTIID_IDirectMusicSegment8,pMidiFile1,(void **)&pwSegment1);
	if(FAILED(hr))
	{
		fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: Loader->GetObject(pwSegment1) ")
				TEXT("failed (%s == %08Xh)"),
				tdmXlatHRESULT(hr), hr);
		goto TEST_END;
	}
	hr = pwLoader->LoadObjectFromFile(CLSID_DirectMusicSegment,CTIID_IDirectMusicSegment8,pMidiFile2,(void **)&pwSegment2);
	if(FAILED(hr))
	{
		fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: Loader->GetObject(pwSegment2) ")
				TEXT("failed (%s == %08Xh)"),
				tdmXlatHRESULT(hr), hr);
		goto TEST_END;
	}
	// **********************************************************************
	// 4.0) Init the Performance
	// **********************************************************************	
	hr = pwPerformance->InitAudio(&pwMusic,NULL,ghWnd,NULL,NULL,NULL,NULL);
	if(FAILED(hr))
	{
		fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: pwPerformance->InitAudio(NULLs) ")
					TEXT("failed (%s == %08Xh)"),
					tdmXlatHRESULT(hr), hr);
		goto TEST_END;
	}
	hr = pwPerformance->CreateStandardAudioPath(
		dwPathType, 			// Choice of standard audio path.
		dwPChannelCount,	// How many pchannels to create.
		fActivate,
		&pwPath1);
	if(FAILED(hr))
	{
		fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: pwPerformance->CreateStandardAudioPath() ")
				TEXT("failed (%s == %08Xh)"),
				tdmXlatHRESULT(hr), hr);
		goto TEST_END;
	}
	if(!pwPath1)
	{
		fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: AudioPath is NULL "));
		goto TEST_END;
	}
	hr = pwPerformance->CreateStandardAudioPath(
		dwPathType, 			// Choice of standard audio path.
		dwPChannelCount,	// How many pchannels to create.
		fActivate,
		&pwPath2);
	if(FAILED(hr))
	{
		fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: pwPerformance->CreateStandardAudioPath() ")
				TEXT("failed (%s == %08Xh)"),
				tdmXlatHRESULT(hr), hr);
		goto TEST_END;
	}
	if(!pwPath2)
	{
		fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: AudioPath is NULL "));
		goto TEST_END;
	}
	// **********************************************************************
	// 4.2) Download
	// **********************************************************************	
	if(pwSegment1)
	{
		hr = pwSegment1->Download(pwPath1);
		if(FAILED(hr))
		{
			fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: pwSegment1->Download(pwPath1) ")
					TEXT("failed (%s == %08Xh)"),
					tdmXlatHRESULT(hr), hr);
			goto TEST_END;
		}
		if(S_FALSE == hr)
		{
			fnsLog(FYILOGLEVEL, TEXT("---- FYI: pwSegment1->Download(pwPath1) ")
					TEXT("returned (%s == %08Xh)"),
					tdmXlatHRESULT(hr), hr);
		}
	}
	if(pwSegment2)
	{
		hr = pwSegment2->Download(pwPath2);
		if(FAILED(hr))
		{
			fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: pwSegment2->Download(pwPath2) ")
					TEXT("failed (%s == %08Xh)"),
					tdmXlatHRESULT(hr), hr);
			goto TEST_END;
		}
		if(S_FALSE == hr)
		{
			fnsLog(FYILOGLEVEL, TEXT("---- FYI: pwSegment2->Download(pwPath2) ")
					TEXT("returned (%s == %08Xh)"),
					tdmXlatHRESULT(hr), hr);
		}
	}
	// **********************************************************************
	// 5) Call PlaySegmentEx - the method to be tested
	// **********************************************************************
	lVolume = -1000;
	hr = pwPerformance->SetGlobalParam(GUID_PerfMasterVolume,&lVolume,sizeof(long));
	if(FAILED(hr))
	{
		fnsLog(ABORTLOGLEVEL, TEXT("!!!! BUG: pwPerformance->SetGlobalParam(volume) ")
				TEXT("failed (%s == %08Xh)"),
				tdmXlatHRESULT(hr), hr);
		goto TEST_END;
	}
	hr = pwPerformance->GetTime(NULL,&mtNow1);
	if(FAILED(hr))
	{
		fnsLog(ABORTLOGLEVEL, TEXT("!!!! BUG: pwPerformance->GetTime(&mtNow1) ")
				TEXT("failed (%s == %08Xh)"),
				tdmXlatHRESULT(hr), hr);
		goto TEST_END;
	}

	mtNow1 += (768 * 4); // one 4/4 measure
	mtNow2 = mtNow1;

	fnsLog(FYILOGLEVEL, TEXT("---- FYI: pwPerformance1->GetTime(%d) "),(long)mtNow1);

	dwRes = FNS_FAIL;
	hr = pwPerformance->PlaySegmentEx( 
		pwSegment1, 
		0,
		NULL,
		DMUS_SEGF_SECONDARY,
		mtNow1,
		&pwSegmentState1,
		NULL,
		pwPath1);
	if(FAILED(hr))
	{
		fnsLog(ABORTLOGLEVEL, TEXT("!!!! BUG: pwPerformance->PlaySegmentEx(pSegment1) ")
				TEXT("failed (%s == %08Xh)"),
				tdmXlatHRESULT(hr), hr);
		goto TEST_END;
	}
	hr = pwPerformance->PlaySegmentEx( 
		pwSegment2, 
		0,
		NULL,
		DMUS_SEGF_BEAT,
		mtNow2,
		&pwSegmentState2,
		NULL,
		pwPath2);
	if(FAILED(hr))
	{
		fnsLog(ABORTLOGLEVEL, TEXT("!!!! BUG: pwPerformance->PlaySegmentEx(pSegment2) ")
				TEXT("failed (%s == %08Xh)"),
				tdmXlatHRESULT(hr), hr);
		goto TEST_END;
	}
	// **********************************************************************
	// 6) varification - segment should play
	// **********************************************************************
	counter = 0;
	SleepTime = 300;
	while ((hr = pwPerformance->IsPlaying(pwSegment1,NULL)) != S_OK)	
	{
		Sleep(SleepTime);
		if(counter++ > TimeOutValue)
		{
			fnsLog(BUGLOGLEVEL, TEXT("!!!! BUG: IsPlaying(segment 1) timed out\n")
				TEXT("Segment did not start playing in the time specified (%s == %08Xh)"),
				tdmXlatHRESULT(hr), hr);
			goto TEST_END;
		}
	}
	counter = 0;
	SleepTime = 300;
	while ((hr = pwPerformance->IsPlaying(pwSegment2,NULL)) != S_OK)	
	{
		Sleep(SleepTime);
		if(counter++ > TimeOutValue)
		{
			fnsLog(BUGLOGLEVEL, TEXT("!!!! BUG: IsPlaying(segment 2) timed out\n")
				TEXT("Segment did not start playing in the time specified (%s == %08Xh)"),
				tdmXlatHRESULT(hr), hr);
			goto TEST_END;
		}
	}
	counter = 0;
	SleepTime = 3000;
	while ((hr = pwPerformance->IsPlaying(pwSegment1,NULL)) == S_OK)	
	{
		Sleep(SleepTime);
		if(counter++ > TimeOutValue)
		{
			fnsLog(BUGLOGLEVEL, TEXT("!!!! BUG: IsPlaying(segment 1) timed out\n")
				TEXT("Segment did not stop playing in the time specified (%s == %08Xh)"),
				tdmXlatHRESULT(hr), hr);
			goto TEST_END;
		}
	}
	counter = 0;
	SleepTime = 3000;
	while ((hr = pwPerformance->IsPlaying(pwSegment2,NULL)) == S_OK)	
	{
		Sleep(SleepTime);
		if(counter++ > TimeOutValue)
		{
			fnsLog(BUGLOGLEVEL, TEXT("!!!! BUG: IsPlaying(segment 2) timed out\n")
				TEXT("Segment did not stop playing in the time specified (%s == %08Xh)"),
				tdmXlatHRESULT(hr), hr);
			goto TEST_END;
		}
	}
	dwRes = FNS_PASS;

TEST_END:
	// *********************************************************************
	// 7) Cleanup objects and uninitialize and Return results
	// *********************************************************************
	if(pwPerformance)
	{
		pwPerformance->CloseDown();
	}
	if(pwSegment1)
	{
		if(pwPath1)
		{
			pwSegment1->Unload(pwPath1); //don't care about the return code at this point
		}
		pwSegment1->Release();
		pwSegment1 = NULL;
	}
	if(pwSegment2)
	{
		if(pwPath2)
		{
			pwSegment2->Unload(pwPath2); //don't care about the return code at this point
		}
		pwSegment2->Release();
		pwSegment2 = NULL;
	}
	if(pwSegmentState1)
	{
		pwSegmentState1->Release();
		pwSegmentState1 = NULL;
	}
	if(pwSegmentState2)
	{
		pwSegmentState2->Release();
		pwSegmentState2 = NULL;
	}
	if(pwPerformance)
	{
		pwPerformance->Release();
		pwPerformance = NULL;
	}
	if(pwLoader)
	{
		pwLoader->Release();
		pwLoader = NULL;
	}
	if(pwPath1)
	{
		pwPath1->Release();
		pwPath1 = NULL;
	}
	if(pwPath2)
	{
		pwPath2->Release();
		pwPath2 = NULL;
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
// @doc IDIRECTMUSICPERFORMANCE8 KCRAVEN
//
// @topic IDirectMusicPerformance8::PlaySegmentEx(synthleak)_Valid_Test |
//
// Description: <nl>
// Valid test function for IDirectMusicPerformance8::PlaySegmentEx(synthleak)<nl>
//
// Test Function: <nl>
// tdmperv8PlaySegmentExSynthLeak()<nl>
//
// Parameters: <nl>
//	BOOL	fCoCreate	- unused <nl>
//	BOOL	fMaxDebug	- unused <nl>
//	DWORD	dwParam1	- unused <nl>
//	DWORD	dwParam2	- unused <nl>
//
// File : tdmperv8.cpp<nl>
//
// Steps: <nl>
//	 1) Initialize<nl>
//	 2) Create Performance8<nl>
//	 3.0) Create Loader<nl>
//	 3.1) SetSearchDirectory<nl>
//	 3.2) Get the Segment<nl>
//	 4.0) Init the Performance<nl>
//	 4.1) Add the default Port<nl>
//	 4.2) Download<nl>
//	 5) Call PlaySegmentEx - the method to be tested<nl>
//	 6) varification - segment should play<nl>
//	 7) Cleanup objects and uninitialize and Return results<nl>
//
// History: <nl>
//	10/09/2000 - kcraven - created<nl>
//
// @index topic | IDIRECTMUSICPERFORMANCE8
//--------------------------------------------------------------------------;
DWORD tdmperv8PlaySegmentExSynthLeak(
	BOOL fCoCreate,
	BOOL fMaxDebug,
	LPARAM dwParam1,
	LPARAM dwParam2)
{
	DWORD						dwRes			= FNS_ABORTED;
	HRESULT 					hr				= S_OK;

	dmthSetPath(NULL);
	WCHAR*						wchPath 		= gwszBasePath;
//	WCHAR*						wchFile 		= gwszMediaPath;

	CtIDirectMusic*				pwMusic			= NULL;
	CtIDirectMusicLoader8*		pwLoader		= NULL;
	CtIDirectMusicPerformance8*	pwPerformance	= NULL;
	CtIDirectMusicSegment8*		pwSegment1		= NULL;
	CtIDirectMusicAudioPath*	pwPath			= NULL;

	int 						counter 		= 0;
	int 						SleepTime		= 3000;
	int 						TimeOutValue	= 10;

	WCHAR*						pMidiFile1 		= L"synthleak.sgt";
	DWORD						dwPathType		= DMUS_APATH_DYNAMIC_STEREO;
//	DWORD						dwPathType		= DMUS_APATH_SHARED_STEREOPLUSREVERB;
	DWORD						dwPChannelCount	= 16;
	BOOL						fActivate		= TRUE;
	DWORD						dwFlags			= NULL;
	int							again			= 0;

	// **********************************************************************
	// 1) Initialize
	// **********************************************************************
	hr = dmthCoInitialize(NULL);
	if(FAILED(hr))
	{
		fnsLog(ABORTLOGLEVEL,
			TEXT("**** ABORT:  initialization failed! (%s == %08Xh)"),
			tdmXlatHRESULT(hr), hr);
		goto TEST_END;
	}
	// **********************************************************************
	// 2) Create Performance8
	// **********************************************************************
	hr = dmthCreatePerformance(IID_IDirectMusicPerformance8, &pwPerformance);
	if(FAILED(hr))
	{
		fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: IDirectMusicPerformance8 creation "),
			TEXT("failed (%s == %08Xh)"),
			tdmXlatHRESULT(hr), hr);
		goto TEST_END;
	}
	// **********************************************************************
	// 3.0) Create Loader
	// **********************************************************************
	hr = dmthCreateLoader(IID_IDirectMusicLoader8, &pwLoader);
	if (FAILED(hr))
	{
		fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: CoCreate Loader object ")
				TEXT("failed (%s == %08Xh)"),
				tdmXlatHRESULT(hr), hr);
		goto TEST_END;
	}
	// **********************************************************************
	// 3.1) SetSearchDirectory
	// **********************************************************************
	hr = pwLoader->SetSearchDirectory(GUID_DirectMusicAllTypes, wchPath, FALSE);
	if (FAILED(hr))
	{
		fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: Loader->SetSeachDirectory(media directory) ")
				TEXT("failed (%s == %08Xh)"),
				tdmXlatHRESULT(hr), hr);
		goto TEST_END;
	}
	// **********************************************************************
	// 3.2) Get the Segment
	// **********************************************************************
	hr = pwLoader->LoadObjectFromFile(CLSID_DirectMusicSegment,CTIID_IDirectMusicSegment8,pMidiFile1,(void **)&pwSegment1);
	if(FAILED(hr))
	{
		fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: Loader->GetObject(pwSegment1) ")
				TEXT("failed (%s == %08Xh)"),
				tdmXlatHRESULT(hr), hr);
		goto TEST_END;
	}
	// **********************************************************************
	// 4.0) Init the Performance
	// **********************************************************************	
	hr = pwPerformance->InitAudio(&pwMusic,NULL,ghWnd,NULL,NULL,NULL,NULL);
	if(FAILED(hr))
	{
		fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: pwPerformance->InitAudio(NULLs) ")
					TEXT("failed (%s == %08Xh)"),
					tdmXlatHRESULT(hr), hr);
		goto TEST_END;
	}
	hr = pwPerformance->CreateStandardAudioPath(
		dwPathType, 			// Choice of standard audio path.
		dwPChannelCount,	// How many pchannels to create.
		fActivate,
		&pwPath);
	if(FAILED(hr))
	{
		fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: pwPerformance->CreateStandardAudioPath() ")
				TEXT("failed (%s == %08Xh)"),
				tdmXlatHRESULT(hr), hr);
		goto TEST_END;
	}
	if(!pwPath)
	{
		fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: AudioPath is NULL "));
		goto TEST_END;
	}
	// **********************************************************************
	// 4.2) Download
	// **********************************************************************	
	if(pwSegment1)
	{
		hr = pwSegment1->Download(pwPath);
		if(FAILED(hr))
		{
			fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: pwSegment1->Download(pwPath) ")
					TEXT("failed (%s == %08Xh)"),
					tdmXlatHRESULT(hr), hr);
			goto TEST_END;
		}
		if(S_FALSE == hr)
		{
			fnsLog(FYILOGLEVEL, TEXT("---- FYI: pwSegment1->Download(pwPath) ")
					TEXT("returned (%s == %08Xh)"),
					tdmXlatHRESULT(hr), hr);
		}
	}

	dwRes = FNS_FAIL;

	// **********************************************************************
	// 4.3) Play
	// **********************************************************************	
	for(again = 0;again < 10;again++)
	{
		hr = pwSegment1->Download(pwPath);
		if(FAILED(hr))
		{
			fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: pwSegment1->Download(pwPath) ")
					TEXT("failed (%s == %08Xh)"),
					tdmXlatHRESULT(hr), hr);
			goto TEST_END;
		}
		hr = pwPerformance->PlaySegmentEx( 
			pwSegment1, 
			0,
			NULL,
			0,
			0,
			NULL,
			NULL,
			pwPath);
		if(FAILED(hr))
		{
			fnsLog(ABORTLOGLEVEL, TEXT("!!!! BUG: pwPerformance->PlaySegmentEx(pSegment1) ")
					TEXT("failed (%s == %08Xh)"),
					tdmXlatHRESULT(hr), hr);
			goto TEST_END;
		}
		Sleep(400);
		hr = pwSegment1->Unload(pwPath);
		if(FAILED(hr))
		{
			fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: pwSegment1->Download(pwPath) ")
					TEXT("failed (%s == %08Xh)"),
					tdmXlatHRESULT(hr), hr);
			goto TEST_END;
		}
	}
	dwRes = FNS_PASS;

TEST_END:
	// *********************************************************************
	// 7) Cleanup objects and uninitialize and Return results
	// *********************************************************************
	if(pwPerformance)
	{
		pwPerformance->CloseDown();
	}
	if(pwSegment1)
	{
		if(pwPath)
		{
			pwSegment1->Unload(pwPath); //don't care about the return code at this point
		}
		pwSegment1->Release();
		pwSegment1 = NULL;
	}
	if(pwPerformance)
	{
		pwPerformance->Release();
		pwPerformance = NULL;
	}
	if(pwLoader)
	{
		pwLoader->Release();
		pwLoader = NULL;
	}
	if(pwPath)
	{
		pwPath->Release();
		pwPath = NULL;
	}
	if(pwMusic)
	{
		pwMusic->Release();
		pwMusic = NULL;
	}

	dmthCoUninitialize();	
	return dwRes;
}	
















