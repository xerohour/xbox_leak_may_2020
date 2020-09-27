//+-------------------------------------------------------------------------
//
//	Microsoft Windows
//
//	Copyright (C) Microsoft Corporation, 1998 - 1999
//
//	File:		tdmperf8.cpp
//
//--------------------------------------------------------------------------

//===========================================================================
// tdmperf8.cpp
//
// Test case functions for IDirectMusicPerformance8 methods (unit parameters)
//
// Functions:
//	 tdmperf8ClonePMsg()
//	 tdmperf8CreateAudioPath()
//	 tdmperf8CreateStandardAudioPath()
//	 tdmperf8GetDefaultAudioPath()
//	 tdmperf8InitAudio()
//	 tdmperf8PlaySegmentEx()
//	 tdmperf8SetDefaultAudioPath()
//	 tdmperf8SetParamHook()
//	 tdmperf8StopEx()
//
// History:
//	01/27/2000 - kcraven  - created
//===========================================================================

#include "tdmusic.h"
#include "tdmperf.h"

#include "RecTake.h"

//--------------------------------------------------------------------------;
// @doc IDIRECTMUSICPERFORMANCE8 KCRAVEN
//
// @topic IDirectMusicPerformance8::ClonePMsg()_Unit_Test |
//
// Description: <nl>
// Unit test function for IDirectMusicPerformance8::ClonePMsg()<nl>
//
// Test Function: <nl>
// tdmperf8ClonePMsg()<nl>
//
// Parameters: <nl>
//	BOOL	fCoCreate	- unused <nl>
//	BOOL	fMaxDebug	- unused <nl>
//	DWORD	dwParam1	- unused <nl>
//	DWORD	dwParam2	- unused <nl>
//
// File : tdmperf8.cpp<nl>
//
// Steps: <nl>
//  1) CoInitialize<nl>
//  2) CoCreatePerformance<nl>
//  3) CoCreateLoader<nl>
//  4) Loader->GetObject(segment)<nl>
//  5) Performance->InitAudio<nl>
//  6) Performance->CreateStandardAudioPath<nl>
//  7) Create Test Tool<nl>
//  8) CoCreateGraph<nl>
//  9) Performance->SetGraph<nl>
//  10) Segment->SetParam(GUID_Download)<nl>
//  11) Performance->PlaySegmentEx<nl>
//  12) Wait for IsPlaying to be true<nl>
//  13) SegState->QueryInterface(IID_IDirectMusicGraph)<nl>
//  14) Performance->AllocPMsg<nl>
//  15) Set psg values<nl>
//  16) Performance->ClonePMsg<nl>
//  17) Compare Two PMsgs<nl>
//  18) Change cloned pmsg<nl>
//  19) Graph->StampPMsg(both)<nl>
//  20) Performance->SendPMsg(both)<nl>
//  21) Wait for IsPlaying to be false <nl>
//  22) Cleanup objects and uninitialize and Return results<nl>
//
// History: <nl>
//	01/27/2000 - kcraven - created<nl>
//
// @index topic | IDIRECTMUSICPERFORMANCE8
//--------------------------------------------------------------------------;
DWORD tdmperf8ClonePMsg(
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

	DMUS_CURVE_PMSG* pCurve;
	DMUS_CURVE_PMSG* pClone;

	MUSIC_TIME mtTimeNow;
	REFERENCE_TIME rtTimeNow;

	DWORD						dwType			= DMUS_APATH_SHARED_STEREOPLUSREVERB;
	DWORD						dwPChannelCount = 16;

	WCHAR						pMidiFile[] 	= L"test.mid";
	dmthSetPath(pMidiFile);
	WCHAR*						wchPath 		= gwszBasePath;
	WCHAR*						wchFile 		= gwszMediaPath;

	DMUS_OBJECTDESC 			desc; 

	MUSIC_TIME					mtLength;
	REFERENCE_TIME				rtLength;
	REFERENCE_TIME				rtMeasure;

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
	// 4) Loader->GetObject(segment)
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
		fnsLog(ABORTLOGLEVEL, TEXT("**** Loader->GetObject(pwSegment) ")
				TEXT("failed (%s == %08Xh)"),
				tdmXlatHRESULT(hr), hr);
		fnsLog(ABORTLOGLEVEL, TEXT("Test case aborting"));
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
		fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: IDirectMusicPerformance8->CreateStandardAudioPath ")
				TEXT("failed (%s == %08Xh)"),
				tdmXlatHRESULT(hr), hr);
		goto TEST_END;
	}
	if(!pNewPath)
	{
		fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: pNewPath is NULL"));
		goto TEST_END;
	}
	// **********************************************************************
	// 7) Create Test Tool
	// **********************************************************************
	pTake = CRecTake::Create();
	if(!pTake)
	{
		fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: pTake is NULL"));
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
	// 11) Performance->PlaySegmentEx
	// **********************************************************************
	hr = pwPerformance->PlaySegmentEx( 
		pwSegment,		// Segment to play. Alternately, could be an IDirectMusicSong. 
		0,				// If song, which segment in the song. 
		NULL,			// Optional template segment to compose transition with. 
		DMUS_SEGF_BEAT, // DMUS_SEGF_ flags.
		0,				// Time to start playback. 
		&pwSegState,	// Returned Segment State. 
		NULL,			// Optional segmentstate or audiopath to replace.
		pNewPath);		// Optional audioPath to play on. 
	if(FAILED(hr))
	{
		fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: pwPerformance->PlaySegment ")
				TEXT("failed (%s == %08Xh)"),
				tdmXlatHRESULT(hr), hr);
		goto TEST_END;
	}
	// **********************************************************************
	// 12) Wait for IsPlaying to be true
	// **********************************************************************
	// Set counter back to zero
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

	////////////////////////////////////////////////////////////////////////////////
	//
	//
	////////////////////////////////////////////////////////////////////////////////
	// length of song in reference time
//	hr = pwSegment->GetLength(&mtLength);
	//BUGBUG error check
//	hr = pwPerformance->MusicToReferenceTime(mtLength,&rtLength);
	//BUGBUG error check

	// time of measure in 1/10 nanoseconds divided by the number of times to check IsPlaying
//	iCheckPlaying = 10;
//	rtMeasure = rtLength / iCheckPlaying;

	// time of measure in milleseconds
//	REFERENCE_TIME rtTmp;
//	rtTmp = (rtMeasure / 10000) / 10000;
//	SleepTime = (int)(rtTmp);
//	iCheckPlaying += 2; // two extra times because IsPlaying may not be false until just a little after the segment actually stops
	////////////////////////////////////////////////////////////////////////////////
	//
	//
	////////////////////////////////////////////////////////////////////////////////

	// **********************************************************************
	// 13) SegState->QueryInterface(IID_IDirectMusicGraph)
	// **********************************************************************
	hr = pwSegState->QueryInterface(IID_IDirectMusicGraph,(void **) &pGraph);
	if(FAILED(hr) || !pGraph)
	{
		fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: IDMSegmentState::QueryInterface(IID_IDirectMusicGraph) failed (%s == %08Xh)"),
			tdmXlatHRESULT(hr), hr);
		goto TEST_END;
	}
	// **********************************************************************
	// 14) Performance->AllocPMsg
	// **********************************************************************
	hr = pwPerformance->AllocPMsg(sizeof(DMUS_CURVE_PMSG),(DMUS_PMSG**)&pCurve );
	if(FAILED(hr) || !pCurve)
	{
		fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: IDMPerformance::AllocPMsg failed (%s == %08Xh)"),
			tdmXlatHRESULT(hr), hr);
		goto TEST_END;
	}

	// fail from here on out
	dwRes = FNS_FAIL;
	// **********************************************************************
	// 15) Set psg values
	// **********************************************************************
	pwPerformance->GetLatencyTime(&rtTimeNow);
	pwPerformance->ReferenceToMusicTime(rtTimeNow,&mtTimeNow);

	// Set the DX8 flag to indicate the wMergeIndex and wParamType fields are valid.
	pCurve->dwFlags = DMUS_PMSGF_MUSICTIME | DMUS_PMSGF_DX8;
	pCurve->mtTime = mtTimeNow; 	// about 4 beats from now.
	pCurve->dwPChannel = DMUS_PCHANNEL_BROADCAST_SEGMENT;
	pCurve->dwVirtualTrackID = 0;	// This isn't a track, so default to 0.
	pCurve->dwType = DMUS_PMSGT_CURVE;
	pCurve->mtDuration = 768 * 4;		// Curve lasts two beats.
	pCurve->mtResetDuration = 0;	// No reset.
	pCurve->nStartValue = 0;		// ignored
	pCurve->nEndValue = 100;		// fade to full volume.
	pCurve->nResetValue = 0;		// No reset.
	pCurve->bType = DMUS_CURVET_CCCURVE;
	pCurve->bCurveShape = DMUS_CURVES_LINEAR;
	pCurve->bCCData = 7;		// MIDI Volume controller.
	// Set bFlags to indicatge that this is a curve that starts from the current playing value, not nStartValue.
	pCurve->bFlags = DMUS_CURVE_START_FROM_CURRENT; 
	pCurve->wParamType = 0; 	// This is for NRPN and RPN only.
	pCurve->wMergeIndex = 1;		// This will be added to the regular volume, not override it.
	pCurve->dwGroupID = -1; 	// Since this isn't a track, just say all groups.
	// **********************************************************************
	// 16) Performance->ClonePMsg
	// **********************************************************************
	hr = pwPerformance->ClonePMsg((DMUS_PMSG*)pCurve,(DMUS_PMSG**)&pClone);
	if(FAILED(hr) || !pClone)
	{
		fnsLog(ABORTLOGLEVEL, TEXT("!!!! FAILED: IDMPerformance::ClonePMsg failed (%s == %08Xh)"),
			tdmXlatHRESULT(hr), hr);
		goto TEST_END;
	}
	// **********************************************************************
	// 17) Compare Two PMsgs
	// **********************************************************************
	hr = pTake->CompareTwoPMsgs((DMUS_PMSG*)pCurve,(DMUS_PMSG*)pClone);
	if(FAILED(hr) || (hr == S_FALSE))
	{
		fnsLog(ABORTLOGLEVEL, TEXT("!!!! FAILED: CRecTake::CompareTwoPMsgs failed (%s == %08Xh)"),
			tdmXlatHRESULT(hr), hr);
		goto TEST_END;
	}
	// **********************************************************************
	// 18) Change cloned pmsg
	// **********************************************************************
	//change cloned pmsg to fade out in stead of in
	pClone->mtTime += (768 * 8);	// 8 beats past the first pmsg.
	pClone->nEndValue = 0;			// fade out.
	// **********************************************************************
	// 19) Graph->StampPMsg(both)
	// **********************************************************************
	hr = pGraph->StampPMsg((DMUS_PMSG*)pCurve);
	if(FAILED(hr))
	{
		fnsLog(ABORTLOGLEVEL, TEXT("!!!! FAILED: IDMToolGraph::StampPMsg(Curve) failed (%s == %08Xh)"),
			tdmXlatHRESULT(hr), hr);
		goto TEST_END;
	}
	hr = pGraph->StampPMsg((DMUS_PMSG*)pClone);
	if(FAILED(hr))
	{
		fnsLog(ABORTLOGLEVEL, TEXT("!!!! FAILED: IDMToolGraph::StampPMsg(Clone) failed (%s == %08Xh)"),
			tdmXlatHRESULT(hr), hr);
		goto TEST_END;
	}
	// **********************************************************************
	// 20) Performance->SendPMsg(both)
	// **********************************************************************
	hr = pwPerformance->SendPMsg((DMUS_PMSG*)pCurve);
	if(FAILED(hr))
	{
		pwPerformance->FreePMsg((DMUS_PMSG*)pCurve);
		fnsLog(ABORTLOGLEVEL, TEXT("!!!! FAILED: IDMPerformance::SendPMsg(Curve) failed (%s == %08Xh)"),
			tdmXlatHRESULT(hr), hr);
		goto TEST_END;
	}
	hr = pwPerformance->SendPMsg((DMUS_PMSG*)pClone);
	if(FAILED(hr))
	{
		pwPerformance->FreePMsg((DMUS_PMSG*)pClone);
		fnsLog(ABORTLOGLEVEL, TEXT("!!!! FAILED: IDMPerformance::SendPMsg(Clone) failed (%s == %08Xh)"),
			tdmXlatHRESULT(hr), hr);
		goto TEST_END;
	}
	// **********************************************************************
	// 21) Wait for IsPlaying to be false 
	// **********************************************************************
	counter = 0;
	SleepTime = 1000; // once a second
	iCheckPlaying = 30; // check for 30 seconds max
	while ((hr = pwPerformance->IsPlaying(pwSegment, NULL)) == S_OK)	
	{
		Sleep(SleepTime);
		if(counter++ > iCheckPlaying)
		{
		fnsLog(ABORTLOGLEVEL, TEXT("!!!! FAILED: IDMPerformance::IsPlaying did not return false (%s == %08Xh)"),
				tdmXlatHRESULT(hr), hr);
			goto TEST_END;
		}
	}

	// we got this far we must have passed
	dwRes = FNS_PASS;

TEST_END:
	// **********************************************************************
	// 22) Cleanup objects and uninitialize and Return results
	// *********************************************************************

	delete pTake;

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
// @topic IDirectMusicPerformance8::CreateAudioPath()_Unit_Test |
//
// Description: <nl>
// Unit test function for IDirectMusicPerformance8::CreateAudioPath(). <nl>
//
// Test Function: <nl>
// tdmperf8CreateAudioPath() <nl>
//
// Parameters: <nl>
//	BOOL	fCoCreate	- unused <nl>
//	BOOL	fMaxDebug	- unused <nl>
//	DWORD	dwParam1	- unused <nl>
//	DWORD	dwParam2	- unused <nl>
//
// File : tdmperf8.cpp <nl>
//
// Steps: <nl>
//	 1)   Initialize<nl>
//	 2.0) Create CtIDirectMusicLoader8 object <nl>
//	 2.1) SetSearchDirectory<nl>
//	 2.3) ScanDirectory<nl>
//	 3)   Get the AudioPath and Segment8<nl>
//	 4.0) Create CtIDirectMusicPerformance8 object <nl>
//	 4.1) Init the performance<nl>
//	 5.0) CreateAudioPath - the test method<nl>
//	 5.1) Verify the results - the path is not NULL<nl>
//	 5.2) Verify the results - at least one object<nl>
//		  that should be in the path is in the path<nl>
//	 5.3) varify the path - play something on the audio path<nl>
//	 5.4) Wait for IsPlaying to return true (or time out)<nl>
//	 6)   Cleanup objects and uninitialize and Return results<nl>
//
// History: <nl>
//	01/27/2000 - kcraven - created<nl>
//	03/02/2000 - kcraven - completed<nl>
//
// @index topic | IDIRECTMUSICPERFORMANCE8
//--------------------------------------------------------------------------;
DWORD tdmperf8CreateAudioPath(
	BOOL fCoCreate,
	BOOL fMaxDebug,
	LPARAM dwParam1,
	LPARAM dwParam2)
{
	DWORD						dwRes				= FNS_PASS;
	HRESULT 					hr					= S_OK;

	CtIDirectMusicPerformance8* pwPerformance8		= NULL;
	CtIDirectMusicLoader8*		pwLoader8			= NULL;
	CtIDirectMusicSegment8* 	pwSegment8			= NULL;
//	CtIUnknown* 				pAudioPathConfig	= NULL;
	CtIDirectMusicObject*		pAudioPathConfig	= NULL;
	CtIDirectMusicAudioPath*	pAudioPath			= NULL;
	DMUS_OBJECTDESC 			desc;
	DMUS_PORTCAPS				caps;

	int 						counter 			= 0;
	int 						SleepTime			= 3000;
	int 						TimeOutValue		= 10;

	DWORD						dwIndex 			= 0;
	IDirectMusicPort*			pDMPort 			= NULL;

	WCHAR	wszAudioPath[]	=	L"Standard.aud";
	WCHAR	wszSegment[]	=	L"test.mid";
	dmthSetPath(NULL);
	WCHAR	*wchPath		= gwszBasePath;
	WCHAR	*wchFile		= gwszMediaPath;

	// **********************************************************************
	// 1) Initialize
	// **********************************************************************
	hr = dmthCoInitialize(NULL);
	if(FAILED(hr))
	{
		fnsLog(ABORTLOGLEVEL,
			TEXT("**** ABORT:  initialization failed! (%s == %08Xh)"),
			tdmXlatHRESULT(hr), hr);
		dwRes = FNS_ABORTED;
		goto TEST_END;
	}
	// **********************************************************************
	// 2.0) Create CtIDirectMusicLoader8 object
	// **********************************************************************
	hr = dmthCreateLoader(IID_IDirectMusicLoader8, &pwLoader8);
	if(FAILED(hr))
	{
		fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: IDirectMusicLoader object creation "),
				TEXT("failed (%s == %08Xh)"),
				tdmXlatHRESULT(hr), hr);
		dwRes = FNS_ABORTED;
		goto TEST_END;
	}
	// **********************************************************************
	// 2.1) SetSearchDirectory
	// **********************************************************************
	hr = pwLoader8->SetSearchDirectory(GUID_DirectMusicAllTypes, wchPath, FALSE);
	if (FAILED(hr))
	{
		fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: Loader->SetSeachDirectory(media directory) ")
				TEXT("failed (%s == %08Xh)"),
				tdmXlatHRESULT(hr), hr);
		dwRes = FNS_ABORTED;
		goto TEST_END;
	}
	// **********************************************************************
	// 2.3) ScanDirectory
	// **********************************************************************
	hr = pwLoader8->ScanDirectory(CLSID_DirectMusicCollection, L"*", NULL);
	if (FAILED(hr))
	{
		fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: Loader->ScanDirectory(dls) ")
				TEXT("failed (%s == %08Xh)"),
				tdmXlatHRESULT(hr), hr);
		dwRes = FNS_ABORTED;
		goto TEST_END;
	}
	hr = pwLoader8->ScanDirectory(CLSID_DirectMusicStyle, L"*", NULL);
	if (FAILED(hr))
	{
		fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: Loader->ScanDirectory(style) ")
				TEXT("failed (%s == %08Xh)"),
				tdmXlatHRESULT(hr), hr);
		dwRes = FNS_ABORTED;
		goto TEST_END;
	}
	// **********************************************************************
	// 3) Get the AudioPath and Segment8
	// **********************************************************************
	ZeroMemory(&desc, sizeof(desc));
	desc.dwSize = sizeof(DMUS_OBJECTDESC);
	desc.guidClass = CLSID_DirectMusicAudioPathConfig;
	desc.dwValidData |= (DMUS_OBJ_CLASS | DMUS_OBJ_FILENAME);
	wcscpy(desc.wszFileName,wszAudioPath);

	hr = pwLoader8->GetObject(&desc,CTIID_IDirectMusicObject,(void **)&pAudioPathConfig);
	if(FAILED(hr))
	{
		fnsLog(ABORTLOGLEVEL, TEXT("**** Loader8->GetObject(pAudioPathConfig) ")
				TEXT("failed (%s == %08Xh)"),
				tdmXlatHRESULT(hr), hr);
		dwRes = FNS_ABORTED;
		goto TEST_END;
	}
	ZeroMemory(&desc, sizeof(desc));
	desc.dwSize = sizeof(DMUS_OBJECTDESC);
	desc.guidClass = CLSID_DirectMusicSegment;
	desc.dwValidData |= (DMUS_OBJ_CLASS | DMUS_OBJ_FILENAME);
	wcscpy(desc.wszFileName,wszSegment);

	hr = pwLoader8->GetObject(&desc,CTIID_IDirectMusicSegment8,(void **)&pwSegment8);
	if(FAILED(hr))
	{
		fnsLog(ABORTLOGLEVEL, TEXT("**** Loader8->GetObject(pwSegment8) ")
				TEXT("failed (%s == %08Xh)"),
				tdmXlatHRESULT(hr), hr);
		dwRes = FNS_ABORTED;
		goto TEST_END;
	}
	// **********************************************************************
	// 4.0) Create CtIDirectMusicPerformance8 object
	// **********************************************************************
	hr = dmthCreatePerformance(IID_IDirectMusicPerformance8, &pwPerformance8);
	if(FAILED(hr))
	{
		fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: IDirectMusicPerformance8 object creation "),
				TEXT("failed (%s == %08Xh)"),
				tdmXlatHRESULT(hr), hr);
		dwRes = FNS_ABORTED;
		goto TEST_END;
	}
	// **********************************************************************
	// 4.1) Init the performance
	// **********************************************************************
	hr = pwPerformance8->InitAudio(
		NULL,	// Can be NULL, pointer to NULL, or valid
		NULL,	// Can be NULL, pointer to NULL, or valid
		ghWnd,	// HWND for initializing DSound.
		0,		// Requested standard audio path or 0.
		0,		// Number of PChannels, if default audio path to be created.
		0,		// ?????? Not Sample rate for all ports and DSound sink
		NULL);
	if(FAILED(hr))
	{
		fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: IDirectMusicPerformance8->InitAudio ")
				TEXT("failed (%s == %08Xh)"),
				tdmXlatHRESULT(hr), hr);
		dwRes = FNS_ABORTED;
		goto TEST_END;
	}
	// **********************************************************************
	// 5.0) CreateAudioPath - the test method
	// **********************************************************************
	hr = pwPerformance8->CreateAudioPath(pAudioPathConfig,TRUE,&pAudioPath);
	if(FAILED(hr))
	{
		fnsLog(BUGLOGLEVEL, TEXT("!!!! BUG: IDirectMusicPerformance8->CreateAudioPath ")
				TEXT("failed (%s == %08Xh)"),
				tdmXlatHRESULT(hr), hr);
		dwRes = FNS_FAIL;
		goto TEST_END;
	}
	// **********************************************************************
	// 5.1) Verify the results - the path is not NULL
	// **********************************************************************
	if(!pAudioPath)
	{
		fnsLog(BUGLOGLEVEL, TEXT("!!!! BUG: pAudioPath is NULL "));
		dwRes = FNS_FAIL;
		goto TEST_END;
	}
	// **********************************************************************
	// 5.2) Verify the results - at least one object that should be in the path is in the path
	// **********************************************************************
	dwIndex = 0;
	while(S_OK == (hr = pAudioPath->GetObjectInPath(DMUS_PCHANNEL_ALL,DMUS_PATH_PORT,
		0, //dwBuffer //BUGBUG - Index?
		GUID_All_Objects,dwIndex,IID_IDirectMusicPort,(void**)&pDMPort)))
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
	if(FAILED(hr) && (DMUS_E_NOT_FOUND != hr))
	{
		fnsLog(BUGLOGLEVEL, TEXT("!!!! BUG: IDirectMusicPerformance8->GetObjectInPath ")
				TEXT("failed (%s == %08Xh)"),
				tdmXlatHRESULT(hr), hr);
		dwRes = FNS_FAIL;
		goto TEST_END;
	}
	// **********************************************************************
	// 5.3) varify the path - play something on the audio path
	// **********************************************************************
	hr = pwSegment8->Download(pwPerformance8);
	if(FAILED(hr))
	{
			fnsLog(BUGLOGLEVEL, TEXT("!!!! BUG: pwSegment8->Download ")
				TEXT("failed (%s == %08Xh)"),
				tdmXlatHRESULT(hr), hr);
		dwRes = FNS_FAIL;
		goto TEST_END;
	}
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
		dwRes = FNS_FAIL;
		goto TEST_END;
	}
	// **********************************************************************
	// 5.4) Wait for IsPlaying to return true (or time out)
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
			dwRes = FNS_FAIL;
			goto TEST_END;
		}
	}
	fnsIncrementIndent();
	fnsLog(FYILOGLEVEL, TEXT("**** FYI: IsPlaying(true) Sleeping...\n"));
	fnsDecrementIndent();
	Sleep(2000);

TEST_END:
	// **********************************************************************
	// 6) Cleanup objects and uninitialize and Return results
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
// @topic IDirectMusicPerformance8::CreateStandardAudioPath()_Unit_Test |
//
// Description: <nl>
// Unit test function for IDirectMusicPerformance8::CreateStandardAudioPath().<nl>
//
// Test Function: <nl>
// tdmperf8CreateStandardAudioPath()<nl>
//
// Parameters: <nl>
//	BOOL	fCoCreate	- unused <nl>
//	BOOL	fMaxDebug	- unused <nl>
//	DWORD	dwParam1	- unused <nl>
//	DWORD	dwParam2	- unused <nl>
//
// File : tdmperf8.cpp<nl>
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
//	01/27/2000 - kcraven - created<nl>
//	02/09/2000 - kcraven - added InitAudio<nl>
//	03/02/2000 - kcraven - finished<nl>
//
// @index topic | IDIRECTMUSICPERFORMANCE8
//--------------------------------------------------------------------------;
DWORD tdmperf8CreateStandardAudioPath(
	BOOL fCoCreate,
	BOOL fMaxDebug,
	LPARAM dwParam1,
	LPARAM dwParam2)
{
	DWORD						dwRes			= FNS_PASS;
	HRESULT 					hr				= S_OK;
	CtIDirectMusicPerformance8* pwPerformance8	= NULL;
	IDirectMusicPerformance8*	pPerformance	= NULL;
	CtIDirectMusicAudioPath*	pNewPath		= NULL;
	CtIDirectMusicLoader		*pwLoader		= NULL;
	CtIDirectMusicSegment		*pwSegment		= NULL;

	IUnknown*					pUnknown		= NULL;
	IUnknown*					pUnkQI			= NULL;
	DWORD						dwStage;
	GUID						guidObject;
	GUID						guidInterface;
	int 						index			= 0;

	DWORD						dwType			= DMUS_APATH_SHARED_STEREOPLUSREVERB;
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
		dwRes = FNS_ABORTED;
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
		dwRes = FNS_ABORTED;
		goto TEST_END;
	}
	// **********************************************************************
	// 3.0) Create CtIDirectMusicLoader object
	// **********************************************************************
	hr = dmthCreateLoader(IID_IDirectMusicLoader, &pwLoader);
	if (FAILED(hr))
	{
		fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: CoCreate Loader object ")
				TEXT("failed (%s == %08Xh)"),
				tdmXlatHRESULT(hr), hr);
		dwRes = FNS_ABORTED;
		goto TEST_END;
	}
	// **********************************************************************
	// 3.3) Get a Segment
	// **********************************************************************
	ZeroMemory(&desc, sizeof(desc));
	desc.dwSize = sizeof(DMUS_OBJECTDESC);
	desc.guidClass = CLSID_DirectMusicSegment;
	desc.dwValidData |= (DMUS_OBJ_CLASS | DMUS_OBJ_FILENAME | DMUS_OBJ_FULLPATH);
	wcscpy(desc.wszFileName,wchFile);
   
	hr = pwLoader->GetObject(&desc,CTIID_IDirectMusicSegment,(void **)&pwSegment);
	fnsIncrementIndent();
	fnsLog(FYILOGLEVEL, TEXT("**** FYI: Loader->GetObject(pwSegment) ")
				TEXT("returned (%s == %08Xh)"),
				tdmXlatHRESULT(hr), hr);
	fnsDecrementIndent();
	if(FAILED(hr))
	{
		fnsLog(ABORTLOGLEVEL, TEXT("**** Loader->GetObject(pwSegment) ")
				TEXT("failed (%s == %08Xh)"),
				tdmXlatHRESULT(hr), hr);
		fnsLog(ABORTLOGLEVEL, TEXT("Test case aborting"));
		dwRes = FNS_ABORTED;
		goto TEST_END;
	}
	// **********************************************************************
	// 4) InitAudio the performance
	// **********************************************************************
	hr = pwPerformance8->InitAudio(NULL,NULL,ghWnd,0,0,0,0); // don't create and audiopath
	if(FAILED(hr))
	{
		fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: IDirectMusicPerformance8->InitAudio ")
				TEXT("failed (%s == %08Xh)"),
				tdmXlatHRESULT(hr), hr);
		dwRes = FNS_ABORTED;
		goto TEST_END;
	}
	// **********************************************************************
	// 5) Call CreateStandardAudioPath - the function to be tested
	// **********************************************************************
	hr = pwPerformance8->CreateStandardAudioPath(
		dwType, 			// Choice of standard audio path.
		dwPChannelCount,	// How many pchannels to create.
		TRUE,
		&pNewPath);
	if(FAILED(hr))
	{
		fnsLog(BUGLOGLEVEL, TEXT("!!!! BUG: IDirectMusicPerformance8->CreateStandardAudioPath ")
				TEXT("failed (%s == %08Xh)"),
				tdmXlatHRESULT(hr), hr);
		dwRes = FNS_FAIL;
		goto TEST_END;
	}
	// **********************************************************************
	// 6.0.0) Verify the results - the audio path is no NULL
	// **********************************************************************
	if(!pNewPath)
	{
		fnsLog(BUGLOGLEVEL, TEXT("!!!! BUG: pNewPath is NULL"));
		dwRes = FNS_FAIL;
		goto TEST_END;
	}
	// **********************************************************************
	// 6.1.0) Play the segment - Download the band
	// **********************************************************************
	BOOL fAuto;
	fAuto = TRUE; // use true to regress autodownloading bug
//	fAuto = FALSE;
	if(fAuto)
	{
		hr = pwPerformance8->SetGlobalParam( GUID_PerfAutoDownload, &fAuto, sizeof(BOOL) );
		if(FAILED(hr))
		{
			fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: Performance->SetGlobalParam(AutoDownload == TRUE) ")
					TEXT("failed (%s == %08Xh)"),
					tdmXlatHRESULT(hr), hr);
			dwRes = FNS_ABORTED;
			goto TEST_END;
		}
	}
	else
	{
		hr = pwPerformance8->GetRealObjPtr(&pPerformance);
		if (FAILED( hr ))
		{
			fnsLog(ABORTLOGLEVEL, "**** ABORT: Failed to get Real Object Pointer(performance) "
					"(HRESULT: %s == %08Xh)", tdmXlatHRESULT(hr), hr);
			dwRes = FNS_ABORTED;
			goto TEST_END;
		}
		hr = pwSegment->SetParam(GUID_Download, -1, 0, 0, (void*)pPerformance); // needs real object
		if(FAILED(hr))
		{
			fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: pwSegment->SetParam(Download) ")
					TEXT("failed (%s == %08Xh)"),
					tdmXlatHRESULT(hr), hr);
			dwRes = FNS_ABORTED;
			goto TEST_END;
		}
	}
	// **********************************************************************
	// 6.1.1) PlaySegmentEx
	// **********************************************************************
	hr = pwPerformance8->PlaySegmentEx(
		pwSegment,		// Segment to play. Alternately, could be an IDirectMusicSong.
		0,				// If song, which segment in the song.
		NULL,			// Optional template segment to compose transition with.
		DMUS_SEGF_BEAT, // DMUS_SEGF_ flags.
		0,				// Time to start playback.
		NULL,			// Returned Segment State.
		NULL,			// Optional segmentstate or audiopath to replace.
		pNewPath);		// Optional audioPath to play on.
	if(FAILED(hr))
	{
		fnsLog(BUGLOGLEVEL, TEXT("!!!! BUG: ptPerformance->PlaySegment ")
				TEXT("failed (%s == %08Xh)"),
				tdmXlatHRESULT(hr), hr);
		dwRes = FNS_FAIL;
		goto TEST_END;
	}
	// **********************************************************************
	// 6.1.2) Wait for IsPlaying to return true (or time out)
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
			dwRes = FNS_FAIL;
			goto TEST_END;
		}
	}
	fnsIncrementIndent();
	fnsLog(FYILOGLEVEL, TEXT("**** FYI: IsPlaying(true) Sleeping...\n"));
	fnsDecrementIndent();
	Sleep(2000);
	// **********************************************************************
	// 6.3) Verification - Get a known object (port) out of the audio path.
	// **********************************************************************
	dwStage 		= DMUS_PATH_PORT;
	guidObject		= CLSID_DirectMusicSynth; //GUID_NULL;
	guidInterface	= IID_IDirectMusicPort;
	index			= 0;
	hr = pNewPath->GetObjectInPath(
		DMUS_PCHANNEL_ALL,		//dwPChannel
		dwStage,				//dwStage
		0,
		guidObject, 			//guidObject
		index,					//dwIndex
		guidInterface,			//idInterface
		(void **)&pUnknown);	//pinterface
	if(FAILED(hr))
	{
		fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: AudioPath->GetObjectInPath ")
				TEXT("failed (%s == %08Xh)"),
				tdmXlatHRESULT(hr), hr);
		dwRes = FNS_ABORTED;
		goto TEST_END;
	}
	// **********************************************************************
	// 6.4) Verification - Call QI for the same type object to Verify it's what we think it is
	// **********************************************************************
	hr = pUnknown->QueryInterface(guidInterface, (void **)&pUnkQI);
	if(FAILED(hr))
	{
		fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: IUnknown->QueryInterface ")
				TEXT("failed (%s == %08Xh)"),
				tdmXlatHRESULT(hr), hr);
		dwRes = FNS_ABORTED;
		goto TEST_END;
	}
	Sleep(2000);

TEST_END:
	// **********************************************************************
	// 7) Cleanup objects and uninitialize and Return results
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
	if((!fAuto) && pPerformance && pwSegment)
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
	if(pNewPath)
	{
		pNewPath->Release();
		pNewPath = NULL;
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
// @topic IDirectMusicPerformance8::GetDefaultAudioPath()_Unit_Test |
//
// Description: <nl>
// Unit test function for IDirectMusicPerformance8::GetDefaultAudioPath().<nl>
//
// Test Function: <nl>
// tdmperf8GetDefaultAudioPath()
//
// Parameters: <nl>
//	BOOL	fCoCreate	- unused <nl>
//	BOOL	fMaxDebug	- unused <nl>
//	DWORD	dwParam1	- unused <nl>
//	DWORD	dwParam2	- unused <nl>
//
// File : tdmperf8.cpp<nl>
//
// Steps: <nl>
//	 1) Initialize<nl>
//	 2) Create CtIDirectMusicPerformance8 object <nl>
//	 3.0) Create CtIDirectMusicLoader8 object <nl>
//	 3.1) Get a segment <nl>
//	 4) InitAudio the performance for a STANDARD MUSIC PATH<nl>
//	 5.0) Call GetDefaultAudioPath - the function to be tested<nl>
//	 5.1) Verify the results - AudioPath is not NULL<nl>
//	 6.0) Verify the results - Play a segment - Download<nl>
//	 6.1) Verify the results - Play a segment - PlaySegmentEx<nl>
//	 6.2) Verify the results - Wait for IsPlaying to return true (or time out)<nl>
//	 7) Cleanup objects and uninitialize and Return results<nl>
//
// History: <nl>
//	01/27/2000 - kcraven - created<nl>
//	03/02/2000 - kcraven - finished<nl>
//
// @index topic | IDIRECTMUSICPERFORMANCE8
//--------------------------------------------------------------------------;
DWORD tdmperf8GetDefaultAudioPath(
	BOOL fCoCreate,
	BOOL fMaxDebug,
	LPARAM dwParam1,
	LPARAM dwParam2)
{
	DWORD						dwRes				= FNS_PASS;
	HRESULT 					hr					= S_OK;

	CtIDirectMusicPerformance8* pwPerformance8		= NULL;
	CtIDirectMusicAudioPath*	pAudioPath			= NULL;
	CtIDirectMusicLoader8*		pwLoader8			= NULL;
	CtIDirectMusicSegment8* 	pwSegment8			= NULL;
	DMUS_OBJECTDESC 			desc;
	DWORD						dwDefaultPathType	= DMUS_APATH_SHARED_STEREOPLUSREVERB;
	DWORD						dwPChannelCount 	= 8;
	DWORD						dwSampleRate		= 11025;
	int 						counter 			= 0;
	int 						SleepTime			= 3000;
	int 						TimeOutValue		= 10;

	dmthSetPath(L"test.mid");
	WCHAR	*wchFile	= gwszMediaPath;

	// **********************************************************************
	// 1) Initialize
	// **********************************************************************
	hr = dmthCoInitialize(NULL);
	if(FAILED(hr))
	{
		fnsLog(ABORTLOGLEVEL,
			TEXT("**** ABORT:  initialization failed! (%s == %08Xh)"),
			tdmXlatHRESULT(hr), hr);
		dwRes = FNS_ABORTED;
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
		dwRes = FNS_ABORTED;
		goto TEST_END;
	}
	// **********************************************************************
	// 3.0) Create CtIDirectMusicLoader8 object
	// **********************************************************************
	hr = dmthCreateLoader(IID_IDirectMusicLoader8, &pwLoader8);
	if(FAILED(hr))
	{
		fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: IDirectMusicLoader8 object creation "),
				TEXT("failed (%s == %08Xh)"),
				tdmXlatHRESULT(hr), hr);
		dwRes = FNS_ABORTED;
		goto TEST_END;
	}
	// **********************************************************************
	// 3.1) Get a segment
	// **********************************************************************
	ZeroMemory(&desc, sizeof(desc));
	desc.dwSize = sizeof(DMUS_OBJECTDESC);
	desc.guidClass = CLSID_DirectMusicSegment;
	desc.dwValidData |= (DMUS_OBJ_CLASS | DMUS_OBJ_FILENAME | DMUS_OBJ_FULLPATH);
	wcscpy(desc.wszFileName,wchFile);

	hr = pwLoader8->GetObject(&desc,CTIID_IDirectMusicSegment8,(void **)&pwSegment8);
	if(FAILED(hr))
	{
		fnsLog(ABORTLOGLEVEL, TEXT("**** Loader8->GetObject(pwSegment8) ")
				TEXT("failed (%s == %08Xh)"),
				tdmXlatHRESULT(hr), hr);
		dwRes = FNS_ABORTED;
		goto TEST_END;
	}
	// **********************************************************************
	// 4) InitAudio the performance for a STANDARD MUSIC PATH
	// **********************************************************************
	hr = pwPerformance8->InitAudio(
		NULL,NULL,ghWnd,
		dwDefaultPathType,	// Requested standard audio path or 0.
		dwPChannelCount,	// Number of PChannels, if default audio path to be created.
		0,		//BUGBUG // Sample rate for all ports and DSound sink
		NULL);
	if(FAILED(hr))
	{
		fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: IDirectMusicPerformance8->InitAudio ")
				TEXT("failed (%s == %08Xh)"),
				tdmXlatHRESULT(hr), hr);
		dwRes = FNS_ABORTED;
		goto TEST_END;
	}
	// **********************************************************************
	// 5.0) Call GetDefaultAudioPath - the function to be tested
	// **********************************************************************
	hr = pwPerformance8->GetDefaultAudioPath(&pAudioPath);
	if(FAILED(hr))
	{
		fnsLog(BUGLOGLEVEL, TEXT("!!!! BUG: IDirectMusicPerformance8->GetDefaultAudioPath ")
				TEXT("failed (%s == %08Xh)"),
				tdmXlatHRESULT(hr), hr);
		dwRes = FNS_FAIL;
		goto TEST_END;
	}
	// **********************************************************************
	// 5.1) Verify the results - AudioPath is not NULL
	// **********************************************************************
	if(!pAudioPath)
	{
		fnsLog(BUGLOGLEVEL, TEXT("!!!! BUG: pAudioPath is NULL "));
		dwRes = FNS_FAIL;
		goto TEST_END;
	}
	// **********************************************************************
	// 6.0) Verify the results - Play a segment - Download
	// **********************************************************************
	hr = pwSegment8->Download(pwPerformance8);
	if(FAILED(hr))
	{
			fnsLog(BUGLOGLEVEL, TEXT("!!!! BUG: pwSegment8->Download ")
				TEXT("failed (%s == %08Xh)"),
				tdmXlatHRESULT(hr), hr);
		dwRes = FNS_FAIL;
		goto TEST_END;
	}
	// **********************************************************************
	// 6.1) Verify the results - Play a segment - PlaySegmentEx
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
		dwRes = FNS_FAIL;
		goto TEST_END;
	}
	// **********************************************************************
	// 6.2) Verify the results - Wait for IsPlaying to return true (or time out)
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
			dwRes = FNS_FAIL;
			goto TEST_END;
		}
	}
	Sleep(2000);

TEST_END:
	// **********************************************************************
	// 7) Cleanup objects and uninitialize and Return results
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

	dmthCoUninitialize();
	return dwRes;
}





//--------------------------------------------------------------------------;
// @doc IDIRECTMUSICPERFORMANCE8 KCRAVEN
//
// @topic IDirectMusicPerformance8::GetParamEx()_Unit_Test |
//
// Description: <nl>
// Unit test function for IDirectMusicPerformance8::GetParamEx()<nl>
//
// Test Function: <nl>
// tdmperf8GetParamEx()<nl>
//
// Parameters: <nl>
//	BOOL	fCoCreate	- unused <nl>
//	BOOL	fMaxDebug	- unused <nl>
//	DWORD	dwParam1	- unused <nl>
//	DWORD	dwParam2	- unused <nl>
//
// File : tdmperf8.cpp<nl>
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
//	01/27/2000 - kcraven - created<nl>
//
// @index topic | IDIRECTMUSICPERFORMANCE8
//--------------------------------------------------------------------------;
DWORD tdmperf8GetParamEx(
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

	MUSIC_TIME mtTimeNow;
	REFERENCE_TIME rtTimeNow;

	DWORD						dwType			= DMUS_APATH_SHARED_STEREOPLUSREVERB;
	DWORD						dwPChannelCount = 16;

	WCHAR						pMidiFile[] 	= L"test.mid";
	dmthSetPath(pMidiFile);
	WCHAR*						wchPath 		= gwszBasePath;
	WCHAR*						wchFile 		= gwszMediaPath;

	DMUS_OBJECTDESC 			desc; 
	DMUS_TIMESIGNATURE			timesig;

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
	// 4) Loader->GetObject
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
	/*
	typedef struct _DMUS_TIMESIGNATURE {
		MUSIC_TIME mtTime;
		BYTE  bBeatsPerMeasure;
		BYTE  bBeat;
		WORD  wGridsPerBeat;
	} DMUS_TIMESIGNATURE;
	*/
	ZeroMemory((void*)&timesig,sizeof(DMUS_TIMESIGNATURE));
	hr = pwPerformance->GetParamEx( 
							GUID_TimeSignature, 
							0,
							0xFFFFFFFF,
							0, 
							0,
							NULL,
							&timesig
	);
	if(FAILED(hr))
	{
		fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: pwPerformance->GetParamEx(time sig) ")
				TEXT("failed (%s == %08Xh)"),
				tdmXlatHRESULT(hr), hr);
		goto TEST_END;
	}

	fnsLog(FYILOGLEVEL, TEXT("        DMUS_TIMESIGNATURE :"));
	fnsLog(FYILOGLEVEL, TEXT("        ntTime             : %016X"), timesig.mtTime);
	fnsLog(FYILOGLEVEL, TEXT("        bBeatsPerMeasure   : %d"), timesig.bBeatsPerMeasure);
	fnsLog(FYILOGLEVEL, TEXT("        bBeat              : %d"), timesig.bBeat);
	fnsLog(FYILOGLEVEL, TEXT("        wGridsPerBeat      : %d"), timesig.wGridsPerBeat);
	// **********************************************************************
	// 10) Wait for IsPlaying to be false
	// **********************************************************************
	counter = 0;
	SleepTime = 1000; // once a second
	iCheckPlaying = 30; // check for 30 seconds max
	while ((hr = pwPerformance->IsPlaying(pwSegment, NULL)) == S_OK)	
	{
		Sleep(SleepTime);
		if(counter++ > iCheckPlaying)
		{
		fnsLog(ABORTLOGLEVEL, TEXT("!!!! FAILED: IDMPerformance::IsPlaying did not return false (%s == %08Xh)"),
				tdmXlatHRESULT(hr), hr);
			goto TEST_END;
		}
	}

	// we got this far we must have passed
	dwRes = FNS_PASS;

TEST_END:
	// **********************************************************************
	// 11) Cleanup objects and uninitialize and Return results
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
// @topic IDirectMusicPerformance8::InitAudio()_Unit_Test |
//
// Description: <nl>
// Unit test function for IDirectMusicPerformance8::InitAudio()<nl>
//
// Test Function: <nl>
// tdmperf8InitAudio()<nl>
//
// Parameters: <nl>
//	BOOL	fCoCreate	- unused <nl>
//	BOOL	fMaxDebug	- unused <nl>
//	DWORD	dwParam1	- unused <nl>
//	DWORD	dwParam2	- unused <nl>
//
// File : tdmperf8.cpp<nl>
//
// Steps: <nl>
//  1) CoInitialize<nl>
//  2) CoCreatePerformance<nl>
//  3) Performance8->InitAudio<nl>
//  4) Performance8->GetDefaultAudioPath<nl>
//  5) AudioPath->GetObjectInPath(ports)<nl>
//  6) Cleanup objects and uninitialize and Return results<nl>
//
// History: <nl>
//	01/27/2000 - kcraven - created<nl>
//	02/02/2000 - kcraven - add test code<nl>
//
// @index topic | IDIRECTMUSICPERFORMANCE8
//--------------------------------------------------------------------------;
DWORD tdmperf8InitAudio(
	BOOL fCoCreate,
	BOOL fMaxDebug,
	LPARAM dwParam1,
	LPARAM dwParam2)
{
	DWORD						dwRes			= FNS_ABORTED;
	HRESULT 					hr				= S_OK;

	CtIDirectMusicPerformance8* pwPerformance8		= NULL;
	CtIDirectMusicAudioPath*	pAudioPath			= NULL;
	CtIDirectMusic* 			pwMusic 			= NULL;
	DWORD						dwDefaultPathType	= DMUS_APATH_SHARED_STEREOPLUSREVERB;
	DWORD						dwPChannelCount 	= 16;

	DWORD						dwIndex 			= 0;
	IDirectMusicPort*			pDMPort 			= NULL;
	DMUS_PORTCAPS				caps;

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
		&pwMusic,			// Can be NULL, pointer to NULL, or valid
		NULL,				// Can be NULL, pointer to NULL, or valid
		ghWnd,				// HWND for initializing DSound.
		dwDefaultPathType,	// Requested standard audio path or 0.
		dwPChannelCount,	// Number of PChannels, if default audio path to be created.
		0,					// Sample rate for all ports and DSound sink
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
	// **********************************************************************
	// 5) AudioPath->GetObjectInPath(ports)
	// **********************************************************************
	// get all the port there are - just for the heck of it - if the path is good this should not fail
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
	if(FAILED(hr) && (DMUS_E_NOT_FOUND != hr))
	{
		fnsLog(BUGLOGLEVEL, TEXT("!!!! BUG: IDirectMusicPerformance8->GetObjectInPath ")
				TEXT("failed (%s == %08Xh)"),
				tdmXlatHRESULT(hr), hr);
		goto TEST_END;
	}
	dwRes = FNS_PASS;

TEST_END:
	// **********************************************************************
	// 6) Cleanup objects and uninitialize and Return results
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

	dmthCoUninitialize();
	return dwRes;
}





//--------------------------------------------------------------------------;
// @doc IDIRECTMUSICPERFORMANCE8 KCRAVEN
//
// @topic IDirectMusicPerformance8::PlaySegmentEx()_Unit_Test |
//
// Description: <nl>
// Unit test function for IDirectMusicPerformance8::PlaySegmentEx()<nl>
//
// Test Function: <nl>
// tdmperf8PlaySegmentEx()<nl>
//
// Parameters: <nl>
//	BOOL	fCoCreate	- unused <nl>
//	BOOL	fMaxDebug	- unused <nl>
//	DWORD	dwParam1	- unused <nl>
//	DWORD	dwParam2	- unused <nl>
//
// File : tdmperf8.cpp<nl>
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
//	01/27/2000 - kcraven - created<nl>
//	02/02/2000 - kcraven - add test<nl>
//	03/02/2000 - kcraven - finished<nl>
//
// @index topic | IDIRECTMUSICPERFORMANCE8
//--------------------------------------------------------------------------;
DWORD tdmperf8PlaySegmentEx(
	BOOL fCoCreate,
	BOOL fMaxDebug,
	LPARAM dwParam1,
	LPARAM dwParam2)
{
	DWORD						dwRes			= FNS_PASS;
	HRESULT 					hr				= S_OK;

	WCHAR						pMidiFile[] 	= L"test.mid";
	dmthSetPath(NULL);
	WCHAR*						wchPath 		= gwszBasePath;

	CtIDirectMusicLoader		*pwLoader		= NULL;
	CtIDirectMusicPerformance8	*ptPerformance	= NULL;
	IDirectMusicPerformance8	*pPerformance	= NULL;
	CtIDirectMusicSegment		*pwSegment		= NULL;

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
		dwRes = FNS_ABORTED;
		goto TEST_END;
	}
	// **********************************************************************
	// 2) Create Performance8
	// **********************************************************************
	hr = dmthCreatePerformance(IID_IDirectMusicPerformance8, &ptPerformance);
	if(FAILED(hr))
	{
		fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: IDirectMusicPerformance8 object creation "),
			TEXT("failed (%s == %08Xh)"),
			tdmXlatHRESULT(hr), hr);
		dwRes = FNS_ABORTED;
		goto TEST_END;
	}
	// **********************************************************************
	// 3.0) Create Loader
	// **********************************************************************
	hr = dmthCreateLoader(IID_IDirectMusicLoader, &pwLoader);
	if (FAILED(hr))
	{
		fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: CoCreate Loader object ")
				TEXT("failed (%s == %08Xh)"),
				tdmXlatHRESULT(hr), hr);
		dwRes = FNS_ABORTED;
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
		dwRes = FNS_ABORTED;
		goto TEST_END;
	}
	// **********************************************************************
	// 3.2) Get the Segment
	// **********************************************************************
	ZeroMemory(&desc, sizeof(desc));
	desc.dwSize = sizeof(DMUS_OBJECTDESC);
	desc.guidClass = CLSID_DirectMusicSegment;
	desc.dwValidData |= (DMUS_OBJ_CLASS | DMUS_OBJ_FILENAME);
	wcscpy(desc.wszFileName,pMidiFile);
	hr = pwLoader->GetObject(&desc,CTIID_IDirectMusicSegment,(void **)&pwSegment);
	if(FAILED(hr))
	{
		fnsLog(ABORTLOGLEVEL, TEXT("**** Loader->GetObject(pwSegment) ")
				TEXT("failed (%s == %08Xh)"),
				tdmXlatHRESULT(hr), hr);
		fnsLog(ABORTLOGLEVEL, TEXT("Test case aborting"));
		dwRes = FNS_ABORTED;
		goto TEST_END;
	}
	// **********************************************************************
	// 4.0) Init the Performance
	// **********************************************************************
	hr = ptPerformance->Init(NULL,NULL,ghWnd);
	if(FAILED(hr))
	{
		fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: ptPerformance->Init(NULL) ")
					TEXT("failed (%s == %08Xh)"),
					tdmXlatHRESULT(hr), hr);
		dwRes = FNS_ABORTED;
		goto TEST_END;
	}
	// **********************************************************************
	// 4.1) Add the Port
	// **********************************************************************
	hr = ptPerformance->AddPort( NULL );
	if(FAILED(hr))
	{
		fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: ptPerformance->AddPort(NULL) ")
				TEXT("failed (%s == %08Xh)"),
				tdmXlatHRESULT(hr), hr);
		dwRes = FNS_ABORTED;
		goto TEST_END;
	}
	// **********************************************************************
	// 4.2) Download
	// **********************************************************************
	BOOL fAuto;
	fAuto = TRUE;  // autodl doesn't work with audiopaths -- should work here
//	  fAuto = FALSE;
	if(fAuto)
	{
		hr = ptPerformance->SetGlobalParam( GUID_PerfAutoDownload, &fAuto, sizeof(BOOL) );
		if(FAILED(hr))
		{
			fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: Performance->SetGlobalParam(AutoDownload == TRUE) ")
					TEXT("failed (%s == %08Xh)"),
					tdmXlatHRESULT(hr), hr);
			dwRes = FNS_ABORTED;
			goto TEST_END;
		}
	}
	else
	{
		hr = ptPerformance->GetRealObjPtr(&pPerformance);
		if (FAILED( hr ))
		{
			fnsLog(ABORTLOGLEVEL, "**** ABORT: Failed to get Real Object Pointer(performance) "
					"(HRESULT: %s == %08Xh)", tdmXlatHRESULT(hr), hr);
			dwRes = FNS_ABORTED;
			goto TEST_END;
		}
		hr = pwSegment->SetParam(GUID_Download, -1, 0, 0, (void*)pPerformance); // needs real object - no need to release - no addref done
		if(FAILED(hr))
		{
			fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: pwSegment->SetParam(Download) ")
					TEXT("failed (%s == %08Xh)"),
					tdmXlatHRESULT(hr), hr);
			dwRes = FNS_ABORTED;
			goto TEST_END;
		}
	}
	// **********************************************************************
	// 5) Call PlaySegmentEx - the method to be tested
	// **********************************************************************
	hr = ptPerformance->PlaySegmentEx(
		pwSegment,			// Segment to play
		0,NULL, 			// Optional stuff NULL.
		DMUS_SEGF_BEAT, 	// DMUS_SEGF_ flags.
		0,NULL,NULL,NULL);	// Optional stuff is NULL.
	if(FAILED(hr))
	{
		fnsLog(ABORTLOGLEVEL, TEXT("!!!! FAIL: ptPerformance->PlaySegment ")
				TEXT("failed (%s == %08Xh)"),
				tdmXlatHRESULT(hr), hr);
		dwRes = FNS_FAIL;
		goto TEST_END;
	}
	// **********************************************************************
	// 6) varification - segment should play
	// **********************************************************************
	counter = 0;
	SleepTime = 1000;
	while ((hr = ptPerformance->IsPlaying(pwSegment, NULL)) != S_OK)
	{
		Sleep(SleepTime);
		if(counter++ > TimeOutValue)
		{
			fnsLog(BUGLOGLEVEL, TEXT("!!!! FAIL: IsPlaying() timed out\n")
				TEXT("Segment did not start playing in the time specified (%s == %08Xh)"),
				tdmXlatHRESULT(hr), hr);
			dwRes = FNS_FAIL;
			goto TEST_END;
		}
	}
//	fnsIncrementIndent();
//	fnsLog(FYILOGLEVEL, TEXT("**** FYI: IsPlaying(true) Sleeping...\n"));
//	fnsDecrementIndent();
	Sleep(2000);

TEST_END:
	// *********************************************************************
	// 7) Cleanup objects and uninitialize and Return results
	// *********************************************************************
	if(ptPerformance)
	{
		ptPerformance->CloseDown();
		ptPerformance->Release();
		ptPerformance = NULL;
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
// @topic IDirectMusicPerformance8::SetDefaultAudioPath()_Unit_Test |
//
// Description: <nl>
// Unit test function for IDirectMusicPerformance8::SetDefaultAudioPath()<nl>
//
// Test Function: <nl>
// tdmperf8SetDefaultAudioPath()<nl>
//
// Parameters: <nl>
//	BOOL	fCoCreate	- unused <nl>
//	BOOL	fMaxDebug	- unused <nl>
//	DWORD	dwParam1	- unused <nl>
//	DWORD	dwParam2	- unused <nl>
//
// File : tdmperf8.cpp<nl>
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
//	01/27/2000 - kcraven - created<nl>
//	03/02/2000 - kcraven - finished<nl>
//
// @index topic | IDIRECTMUSICPERFORMANCE8
//--------------------------------------------------------------------------;
DWORD tdmperf8SetDefaultAudioPath(
	BOOL fCoCreate,
	BOOL fMaxDebug,
	LPARAM dwParam1,
	LPARAM dwParam2)
{
	DWORD						dwRes				= FNS_PASS;
	HRESULT 					hr					= S_OK;

	CtIDirectMusicPerformance8* pwPerformance8		= NULL;
	CtIDirectMusicLoader8*		pwLoader8			= NULL;
	CtIDirectMusicSegment8* 	pwSegment8			= NULL;
	CtIUnknown* 				pAudioPathConfig	= NULL;
//	CtIDirectMusicObject*		pAudioPathConfig	= NULL;
	CtIDirectMusicAudioPath*	pAudioPath			= NULL;
	DMUS_OBJECTDESC 			desc;

	int 						counter 			= 0;
	int 						SleepTime			= 3000;
	int 						TimeOutValue		= 10;

	WCHAR						pMidiFile[] 		= L"test.mid";
	WCHAR						pAudPathFile[]		= L"StandardReverbShared.aud";

	// **********************************************************************
	// 1) Initialize
	// **********************************************************************
	hr = dmthCoInitialize(NULL);
	if(FAILED(hr))
	{
		fnsLog(ABORTLOGLEVEL,
			TEXT("**** ABORT:  initialization failed! (%s == %08Xh)"),
			tdmXlatHRESULT(hr), hr);
		dwRes = FNS_ABORTED;
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
		dwRes = FNS_ABORTED;
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
		fnsLog(ABORTLOGLEVEL, TEXT("Test case aborting"));
		dwRes = FNS_ABORTED;
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
	// can be IID_IUnknown or IID_IDirectMusicObject
	hr = pwLoader8->GetObject(&desc,CTIID_IUnknown,(void **)&pAudioPathConfig);
//	hr = pwLoader8->GetObject(&desc,CTIID_IDirectMusicObject,(void **)&pAudioPathConfig);
	if(FAILED(hr))
	{
		fnsLog(ABORTLOGLEVEL, TEXT("**** Loader8->GetObject(pAudioPathConfig) ")
				TEXT("failed (%s == %08Xh)"),
				tdmXlatHRESULT(hr), hr);
		dwRes = FNS_ABORTED;
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
		dwRes = FNS_ABORTED;
		goto TEST_END;
	}
	// **********************************************************************
	// 3.1) InitAudio the performance witha standard audio path that plays silent
	// **********************************************************************
	hr = pwPerformance8->InitAudio(
		NULL,NULL,ghWnd,
		DMUS_APATH_DYNAMIC_MONO, // Requested standard audio path or 0.
		64, 							// Number of PChannels
		0,							// Sample rate
		NULL);
	if(FAILED(hr))
	{
		fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: IDirectMusicPerformance8->InitAudio ")
				TEXT("failed (%s == %08Xh)"),
				tdmXlatHRESULT(hr), hr);
		dwRes = FNS_ABORTED;
		goto TEST_END;
	}
	// **********************************************************************
	// 4.0) Create an AudioPath from the audiopathconfig file to be set later
	// **********************************************************************
	hr = pwPerformance8->CreateAudioPath(pAudioPathConfig,TRUE,&pAudioPath);
	if(FAILED(hr))
	{
		fnsLog(BUGLOGLEVEL, TEXT("!!!! BUG: IDirectMusicPerformance8->CreateAudioPath ")
				TEXT("failed (%s == %08Xh)"),
				tdmXlatHRESULT(hr), hr);
		dwRes = FNS_FAIL;
		goto TEST_END;
	}
	// **********************************************************************
	// 4.1) Verify the results
	// **********************************************************************
	if(!pAudioPath)
	{
		fnsLog(BUGLOGLEVEL, TEXT("!!!! BUG: pAudioPath is NULL "));
		dwRes = FNS_FAIL;
		goto TEST_END;
	}
	// **********************************************************************
	// 5.0) Play the segment - Download
	// **********************************************************************
	hr = pwSegment8->Download(pwPerformance8);
	if(FAILED(hr))
	{
			fnsLog(BUGLOGLEVEL, TEXT("!!!! BUG: pwSegment8->Download ")
				TEXT("failed (%s == %08Xh)"),
				tdmXlatHRESULT(hr), hr);
		dwRes = FNS_FAIL;
		goto TEST_END;
	}
	// **********************************************************************
	// 5.1) PlaySegmentEx
	// **********************************************************************
	hr = pwPerformance8->PlaySegmentEx(
		pwSegment8, 		// Segment to play
		0,NULL, 			// Optional stuff NULL
		DMUS_SEGF_BEAT, 	// DMUS_SEGF_ flags.
		0,NULL,NULL,NULL);	// Optional stuff NULL - use default audiopath
	if(FAILED(hr))
	{
		fnsLog(ABORTLOGLEVEL, TEXT("!!!! FAIL: ptPerformance->PlaySegment ")
				TEXT("failed (%s == %08Xh)"),
				tdmXlatHRESULT(hr), hr);
		dwRes = FNS_FAIL;
		goto TEST_END;
	}
	// **********************************************************************
	// 5.2) Verify silent playback - Wait for IsPlaying - NO SOUND EXPECTED
	// **********************************************************************
	counter = 0;
	SleepTime = 1000;
	while ((hr = pwPerformance8->IsPlaying(pwSegment8, NULL)) != S_OK)
	{
		Sleep(SleepTime);
		if(counter++ >TimeOutValue)
		{
			fnsLog(BUGLOGLEVEL, TEXT("!!!! FAIL: IsPlaying() timed out\n")
				TEXT("Segment did not start playing in the time specified (%s == %08Xh)"),
				tdmXlatHRESULT(hr), hr);
			dwRes = FNS_FAIL;
			goto TEST_END;
		}
	}
	counter = 0;
	SleepTime = 1000;
	while ((hr = pwPerformance8->IsPlaying(pwSegment8, NULL)) == S_OK)
	{
		Sleep(SleepTime);
		if(counter++ >TimeOutValue)
		{
			fnsLog(BUGLOGLEVEL, TEXT("!!!! FAIL: IsPlaying() timed out\n")
				TEXT("Segment did not start playing in the time specified (%s == %08Xh)"),
				tdmXlatHRESULT(hr), hr);
			dwRes = FNS_FAIL;
			goto TEST_END;
		}
	}
	// **********************************************************************
	// 5.3) Quickly unload - if there had been sound (bug) it should stop
	// **********************************************************************
	hr = pwSegment8->Unload(pwPerformance8);
	if(FAILED(hr))
	{
			fnsLog(BUGLOGLEVEL, TEXT("!!!! BUG: pwSegment8->Unload ")
				TEXT("failed (%s == %08Xh)"),
				tdmXlatHRESULT(hr), hr);
		dwRes = FNS_FAIL;
		goto TEST_END;
	}
	// **********************************************************************
	// 6) Call SetDefaultAudioPath - the function to be tested<nl>
	// **********************************************************************
	hr = pwPerformance8->SetDefaultAudioPath(pAudioPath);
	if(FAILED(hr))
	{
		fnsLog(BUGLOGLEVEL, TEXT("!!!! BUG: IDirectMusicPerformance8->SetDefaultAudioPath ")
				TEXT("failed (%s == %08Xh)"),
				tdmXlatHRESULT(hr), hr);
		dwRes = FNS_FAIL;
		goto TEST_END;
	}
	// **********************************************************************
	// 7.0) Verification - Play a segment - Download
	// **********************************************************************
	hr = pwSegment8->Download(pwPerformance8);
	if(FAILED(hr))
	{
			fnsLog(BUGLOGLEVEL, TEXT("!!!! BUG: pwSegment8->Download ")
				TEXT("failed (%s == %08Xh)"),
				tdmXlatHRESULT(hr), hr);
		dwRes = FNS_FAIL;
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
		fnsLog(ABORTLOGLEVEL, TEXT("!!!! FAIL: ptPerformance->PlaySegment ")
				TEXT("failed (%s == %08Xh)"),
				tdmXlatHRESULT(hr), hr);
		dwRes = FNS_FAIL;
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
		if(counter++ >TimeOutValue)
		{
			fnsLog(BUGLOGLEVEL, TEXT("!!!! FAIL: IsPlaying() timed out\n")
				TEXT("Segment did not start playing in the time specified (%s == %08Xh)"),
				tdmXlatHRESULT(hr), hr);
			dwRes = FNS_FAIL;
			goto TEST_END;
		}
	}
	counter = 0;
	SleepTime = 1000;
	while ((hr = pwPerformance8->IsPlaying(pwSegment8, NULL)) == S_OK)
	{
		Sleep(SleepTime);
		if(counter++ >TimeOutValue)
		{
			fnsLog(BUGLOGLEVEL, TEXT("!!!! FAIL: IsPlaying() timed out\n")
				TEXT("Segment did not start playing in the time specified (%s == %08Xh)"),
				tdmXlatHRESULT(hr), hr);
			dwRes = FNS_FAIL;
			goto TEST_END;
		}
	}

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

	dmthCoUninitialize();
	return dwRes;
}





//--------------------------------------------------------------------------;
// @doc IDIRECTMUSICPERFORMANCE8 KCRAVEN
//
// @topic IDirectMusicPerformance8::SetParamHook()_Unit_Test |
//
// Description: <nl>
// Unit test function for IDirectMusicPerformance8::SetParamHook()<nl>
//
// Test Function: <nl>
// tdmperf8SetParamHook()<nl>
//
// Parameters: <nl>
//	BOOL	fCoCreate	- unused <nl>
//	BOOL	fMaxDebug	- unused <nl>
//	DWORD	dwParam1	- unused <nl>
//	DWORD	dwParam2	- unused <nl>
//
// File : tdmperf8.cpp<nl>
//
// Steps: <nl>
// 1) Initialize <nl>
// 2) Clean up Objects, uninitialize, and return results <nl>
//
// History: <nl>
//	01/27/2000 - kcraven - created<nl>
//
// @index topic | IDIRECTMUSICPERFORMANCE8
//--------------------------------------------------------------------------;
/*
//PHOOPHOO
DWORD tdmperf8SetParamHook(
	BOOL fCoCreate,
	BOOL fMaxDebug,
	LPARAM dwParam1,
	LPARAM dwParam2)
{
	DWORD						dwRes			= FNS_PASS;
	HRESULT 					hr				= S_OK;

	dmthSetPath(NULL);
	WCHAR	*wchPath	= gwszBasePath;
	WCHAR	*wchFile	= gwszMediaPath;

	// **********************************************************************
	// 1) Initialize
	// **********************************************************************
	hr = dmthCoInitialize(NULL);
	if(FAILED(hr))
	{
		fnsLog(ABORTLOGLEVEL,
			TEXT("**** ABORT:  initialization failed! (%s == %08Xh)"),
			tdmXlatHRESULT(hr), hr);
		dwRes = FNS_ABORTED;
		goto TEST_END;
	}

	fnsLog(ABORTLOGLEVEL,
		TEXT("**** PENDING: IDirectMusicPerformance8::SetParamHook() Test is not implemented."));
	dwRes = FNS_PENDING;

TEST_END:
	// **********************************************************************
	// 18) Cleanup objects and uninitialize and Return results
	// *********************************************************************

	dmthCoUninitialize();
	return dwRes;
}
*/




//--------------------------------------------------------------------------;
// @doc IDIRECTMUSICPERFORMANCE8 KCRAVEN
//
// @topic IDirectMusicPerformance8::StopEx()_Unit_Test |
//
// Description: <nl>
// Unit test function for IDirectMusicPerformance8::StopEx()<nl>
//
// Test Function: <nl>
// tdmperf8StopEx()<nl>
//
// Parameters: <nl>
//	BOOL	fCoCreate	- unused <nl>
//	BOOL	fMaxDebug	- unused <nl>
//	DWORD	dwParam1	- unused <nl>
//	DWORD	dwParam2	- unused <nl>
//
// File : tdmperf8.cpp<nl>
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
//	01/27/2000 - kcraven - created<nl>
//	02/02/2000 - kcraven - add test<nl>
//	03/02/2000 - kcraven - finished<nl>
//
// @index topic | IDIRECTMUSICPERFORMANCE8
//--------------------------------------------------------------------------;
DWORD tdmperf8StopEx(
	BOOL fCoCreate,
	BOOL fMaxDebug,
	LPARAM dwParam1,
	LPARAM dwParam2)
{
	DWORD						dwRes			= FNS_PASS;
	HRESULT 					hr				= S_OK;

	WCHAR						pMidiFile[] 	= L"test.mid";
	dmthSetPath(pMidiFile);
//	WCHAR*						wchPath 		= gwszBasePath;
	WCHAR*						wchFile 		= gwszMediaPath;

	CtIDirectMusicLoader		*pwLoader		= NULL;
	CtIDirectMusicPerformance8	*pwPerformance8 = NULL;
	CtIDirectMusicSegment		*pwSegment		= NULL;

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
		dwRes = FNS_ABORTED;
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
		dwRes = FNS_ABORTED;
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
		dwRes = FNS_ABORTED;
		goto TEST_END;
	}
	// **********************************************************************
	// 3.1) Get the Segment object with fullpath
	// **********************************************************************
	ZeroMemory(&desc, sizeof(desc));
	desc.dwSize = sizeof(DMUS_OBJECTDESC);
	desc.guidClass = CLSID_DirectMusicSegment;
	desc.dwValidData |= (DMUS_OBJ_CLASS | DMUS_OBJ_FILENAME | DMUS_OBJ_FULLPATH);
	wcscpy(desc.wszFileName,wchFile);
	hr = pwLoader->GetObject(&desc,CTIID_IDirectMusicSegment,(void **)&pwSegment);
	if(FAILED(hr))
	{
		fnsLog(ABORTLOGLEVEL, TEXT("**** Loader->GetObject(pwSegment) ")
				TEXT("failed (%s == %08Xh)"),
				tdmXlatHRESULT(hr), hr);
		fnsLog(ABORTLOGLEVEL, TEXT("Test case aborting"));
		dwRes = FNS_ABORTED;
		goto TEST_END;
	}
	// **********************************************************************
	// 4.0) Init the Performance8 as level 1
	// **********************************************************************
	hr = pwPerformance8->Init(NULL,NULL,ghWnd);
	if(FAILED(hr))
	{
		fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: Performance8->Init(NULL) ")
					TEXT("failed (%s == %08Xh)"),
					tdmXlatHRESULT(hr), hr);
		dwRes = FNS_ABORTED;
		goto TEST_END;
	}
	// **********************************************************************
	// 4.1) Add the default Port
	// **********************************************************************
	hr = pwPerformance8->AddPort( NULL );
	if(FAILED(hr))
	{
		fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: Performance8->AddPort(NULL) ")
				TEXT("failed (%s == %08Xh)"),
				tdmXlatHRESULT(hr), hr);
		dwRes = FNS_ABORTED;
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
		dwRes = FNS_ABORTED;
		goto TEST_END;
	}
	// **********************************************************************
	// 5.0) Play the segment - PlaySegmentEx
	// **********************************************************************
	hr = pwPerformance8->PlaySegmentEx(pwSegment,0,NULL,DMUS_SEGF_BEAT,0,NULL,NULL,NULL);
	if(FAILED(hr))
	{
		fnsLog(ABORTLOGLEVEL, TEXT("!!!! BUG: Performance8->PlaySegment ")
				TEXT("failed (%s == %08Xh)"),
				tdmXlatHRESULT(hr), hr);
		dwRes = FNS_ABORTED;
		goto TEST_END;
	}
	// **********************************************************************
	// 5.1) Wait for IsPlaying to return true (or time out)
	// **********************************************************************
	counter = 0;
	SleepTime = 1000;
	while ((hr = pwPerformance8->IsPlaying(pwSegment, NULL)) != S_OK)
	{
		Sleep(SleepTime);
		if(counter++ >TimeOutValue)
		{
			fnsLog(ABORTLOGLEVEL, TEXT("!!!! BUG: PlaySegment() timed out\n")
				TEXT("Segment did not start playing in the time speciied (%s == %08Xh)"),
				tdmXlatHRESULT(hr), hr);
			dwRes = FNS_ABORTED;
			goto TEST_END;
		}
	}
	Sleep(1000);
	// **********************************************************************
	// 6) Call StopEx - the method we are testing
	// **********************************************************************
	hr = pwPerformance8->StopEx(
		pwSegment,			// Segstate, AudioPath, Segment, or Song to stop.
		0,					// Optional time.
		DMUS_SEGF_MEASURE); // Standard SEGF flags for setting time.
	if(FAILED(hr))
	{
		fnsLog(BUGLOGLEVEL, TEXT("**** BUG: Performance8->StopEx ")
				TEXT("failed (%s == %08Xh)"),
				tdmXlatHRESULT(hr), hr);
		dwRes = FNS_FAIL;
		goto TEST_END;
	}
	// **********************************************************************
	// 7) Verification - Wait for IsPlaying to return false (or time out)
	// **********************************************************************
	counter = 0;
	SleepTime = 1000;
	while ((hr = pwPerformance8->IsPlaying(pwSegment, NULL)) == S_OK)
	{
		Sleep(SleepTime);
		if(counter++ >TimeOutValue)
		{
			fnsLog(BUGLOGLEVEL, TEXT("**** BUG: Performance8->IsPlaying() is true\n")
				TEXT("Segment did not stop playing in the time specified (%s == %08Xh)"),
				tdmXlatHRESULT(hr), hr);
			dwRes = FNS_FAIL;
			goto TEST_END;
		}
	}

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

	dmthCoUninitialize();
	return dwRes;
}


