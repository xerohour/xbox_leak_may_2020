#include "globals.h"
#include "cicmusicX.h"

DWORD tdmperv8PlaySegmentEx(CtIDirectMusicPerformance8 *ptPerf8, DWORD dwTestType);
DWORD tdmperv8PlaySegmentExFrom(CtIDirectMusicPerformance8 *ptPerf8, DWORD dwTestType);
DWORD Performance8_PlaySegment_NoStop(CtIDirectMusicPerformance8 *ptPerf8);


/********************************************************************************
Main test function.
********************************************************************************/
HRESULT Performance8_PlaySegmentEx (CtIDirectMusicPerformance8* ptPerf8)
{
    HRESULT hr = S_OK;
    DWORD dwRes = FNS_FAIL;
    fnsLog(FYILOGLEVEL, "Calling main test function Performance8_PlaySegmentEx())");

    CALLDMTEST(Performance8_PlaySegment_NoStop(ptPerf8));   //See what happens if we don't stop the music.

/*

    CALLDMTEST(tdmperv8PlaySegmentEx(ptPerf8, 1));  //PlaySegmentEx                                      //PASSING
    CALLDMTEST(tdmperv8PlaySegmentEx(ptPerf8, 2));  //Manual Download    (wave track - audiopath)        //FAILS DUE TO BUG 2684 (download)

    CALLDMTEST(tdmperv8PlaySegmentExFrom(ptPerf8,  1));  //To Self(BEAT)",                                //PASSING
    CALLDMTEST(tdmperv8PlaySegmentExFrom(ptPerf8,  2));  //To Self(MEASURE)",                             //PASSING
    CALLDMTEST(tdmperv8PlaySegmentExFrom(ptPerf8,  3));  //To Self(QUEUE)",                               //PASSING
    CALLDMTEST(tdmperv8PlaySegmentExFrom(ptPerf8,  4));  //AutoTransition(BEAT)",                         //FAILS DUE TO IsPlaying succeeding (see mail to Kelly)
    CALLDMTEST(tdmperv8PlaySegmentExFrom(ptPerf8,  5));  //AutoTransition(MEASURE)",                    //""
    CALLDMTEST(tdmperv8PlaySegmentExFrom(ptPerf8,  6));  //AutoTransition(QUEUE)",                      //""
    CALLDMTEST(tdmperv8PlaySegmentExFrom(ptPerf8,  7));  //Secondary(QUEUE)",                           //""

*/
    return hr;
};

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
DWORD tdmperv8PlaySegmentEx(CtIDirectMusicPerformance8 *ptPerf8, DWORD dwTestType)
{
	DWORD						dwRes			= FNS_ABORTED;
	HRESULT 					hr				= S_OK;

    CtIDirectMusicAudioPath     *ptPath     = NULL;

	CtIDirectMusicLoader8		*pwLoader		= NULL;
	CtIDirectMusicSegment8		*pwSegment		= NULL;

	DMUS_OBJECTDESC 			desc            = {0};    

	int 						counter 		= 0;
	int 						SleepTime		= 3000;
	int 						TimeOutValue	= 10;
    char *                      pszFile         = NULL;

	switch(dwTestType)
	{
		case 1:
			pszFile = "T:\\Media\\test.sgt";
			break;
		case 2:
			pszFile = "T:\\Media\\Perf8\\PlaySeg\\WaveEmb.sgt";
			break;
		default:
			return FNS_PENDING;
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
	// 3.2) Get the Segment
	// **********************************************************************
	ZeroMemory(&desc, sizeof(desc));
	desc.dwSize = sizeof(DMUS_OBJECTDESC);
	desc.guidClass = CLSID_DirectMusicSegment;
	desc.dwValidData |= (DMUS_OBJ_CLASS | DMUS_OBJ_FILENAME);
    mbstowcs(desc.wszFileName,pszFile, strlen(pszFile) + 1);
	hr = pwLoader->GetObject(&desc,CTIID_IDirectMusicSegment8,(void **)&pwSegment);
	if(FAILED(hr))
	{
		fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: Loader->GetObject(pwSegment) ")
				TEXT("failed (%s == %08Xh)"),
				tdmXlatHRESULT(hr), hr);
		goto TEST_END;
	}

    hr = ptPerf8->CreateStandardAudioPath(DMUS_APATH_SHARED_STEREOPLUSREVERB, 16, TRUE, &ptPath);
	if(FAILED(hr))
	{
		fnsLog(ABORTLOGLEVEL, "**** ABORT: ptPerf8->CreateStandardAudioPath(pwSegment) failed with %s (%08Xh)",tdmXlatHRESULT(hr), hr);
		goto TEST_END;
	}

	// **********************************************************************
	// 4.2) Download
	// **********************************************************************	
	hr = pwSegment->Download(ptPerf8); // needs real object - no need to release - no addref done 
	if(dwTestType == 3)
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
	dwRes = FNS_FAIL;

	// **********************************************************************
	// 5) Call PlaySegmentEx - the method to be tested
	// **********************************************************************
	hr = ptPerf8->PlaySegmentEx( 
		pwSegment,			// Segment to play 
		0,NULL, 			// Optional stuff NULL. 
		DMUS_SEGF_BEAT, 	// DMUS_SEGF_ flags.
		0,NULL,NULL,ptPath);	// Optional stuff is NULL. 
	if(FAILED(hr))
	{
		fnsLog(ABORTLOGLEVEL, TEXT("!!!! BUG: ptPerf8->PlaySegmentEx ")
				TEXT("failed (%s == %08Xh)"),
				tdmXlatHRESULT(hr), hr);
		goto TEST_END;
	}
	// **********************************************************************
	// 6) varification - segment should play
	// **********************************************************************
	counter = 0;
	SleepTime = 1000;
	while ((hr = ptPerf8->IsPlaying(pwSegment, NULL)) != S_OK)	
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


	fnsLog(FYILOGLEVEL, "Should hear %s playing for 4 seconds", pszFile);
	Sleep(4000);
	dwRes = FNS_PASS;

TEST_END:
	if(pwSegment)
	{
		if(ptPerf8)
		{            
    		pwSegment->Unload(ptPerf8); //don't case about the return code at this point
		}
		pwSegment->Release();
		pwSegment = NULL;
	}
	if(pwLoader)
	{
		pwLoader->Release();
		pwLoader = NULL;
	}

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
DWORD tdmperv8PlaySegmentExFrom(CtIDirectMusicPerformance8 *ptPerf8, DWORD dwTestType)
{
	DWORD						dwRes			= FNS_ABORTED;
	HRESULT 					hr				= S_OK;

	CtIDirectMusicLoader8*		pwLoader		= NULL;
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

	CHAR*						pPMidiFile 		= "T:\\Media\\test.sgt";
	CHAR*						pSMidiFile 		= "T:\\Media\\test.sgt";
	CHAR*						pWMidiFile 		= "T:\\Media\\Perf8\\PlaySeg\\rdrum.sgt";
	CHAR*						pTemplateFile 	= "T:\\Media\\Perf8\\PlaySeg\\Seg8STC.sgt";
	DWORD						dwPathType		= DMUS_APATH_SHARED_STEREOPLUSREVERB;
	DWORD						dwPChannelCount	= 16;
	BOOL						fActivate		= TRUE;
	DWORD						dwFlags			= NULL;

	// which type of test to do
	switch(dwTestType)
	{
	case 1: // self beat
	case 2: // self measure
	case 3: // self queue
	case 4: // autotransition beat
	case 5: // autotransition measure
	case 6: // autotransition queue
		break;
	case 7: // autotransition to second segment
		pPMidiFile 		= "T:\\Media\\Perf8\\PlaySeg\\synctest.sgt";
		break;
	default: // not null - not supported
		return FNS_PENDING;
		break;
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
/*
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
*/
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
	switch(dwTestType)
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

	hr = ptPerf8->CreateStandardAudioPath(
		dwPathType, 			// Choice of standard audio path.
		dwPChannelCount,	// How many pchannels to create.
		fActivate,
		&pwPath);
	if(FAILED(hr))
	{
		fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: ptPerf8->CreateStandardAudioPath() ")
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
	hr = ptPerf8->PlaySegmentEx( 
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
		fnsLog(ABORTLOGLEVEL, TEXT("!!!! BUG: ptPerf8->PlaySegmentEx ")
				TEXT("failed (%s == %08Xh)"),
				tdmXlatHRESULT(hr), hr);
		goto TEST_END;
	}
	// **********************************************************************
	// 6) varification - segment should play
	// **********************************************************************
	counter = 0;
	SleepTime = 300;
	while ((hr = ptPerf8->IsPlaying(pwPSegment,NULL)) != S_OK)	
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
	switch(dwTestType)
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
	switch(dwTestType)
	{
	default: // from self
		hr = ptPerf8->PlaySegmentEx( 
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
		hr = ptPerf8->PlaySegmentEx( 
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
		fnsLog(ABORTLOGLEVEL, TEXT("!!!! BUG: ptPerf8->PlaySegmentEx ")
				TEXT("failed (%s == %08Xh)"),
				tdmXlatHRESULT(hr), hr);
		goto TEST_END;
	}
	switch(dwTestType)
	{
	case 4:
	case 5:
	case 6:
		hr = ptPerf8->IsPlaying(pwSSegment,NULL);
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
		hr = ptPerf8->PlaySegmentEx( 
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
			fnsLog(ABORTLOGLEVEL, TEXT("!!!! BUG: ptPerf8->PlaySegmentEx ")
					TEXT("failed (%s == %08Xh)"),
					tdmXlatHRESULT(hr), hr);
			goto TEST_END;
		}
		break;
	}
	counter = 0;
	SleepTime = 3000;
	while ((hr = ptPerf8->IsPlaying(NULL,pwSegmentState2)) != S_OK)	
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
	while ((hr = ptPerf8->IsPlaying(NULL,pwSegmentState2)) == S_OK)	
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
		while ((hr = ptPerf8->IsPlaying(pwWSegment,NULL)) == S_OK)	
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

	return dwRes;
}	





/********************************************************************************
Need to uncomment a line in CICMusic::PrivateUninitialize to repro bug 2967.
********************************************************************************/
DWORD Performance8_PlaySegment_NoStop(CtIDirectMusicPerformance8 *ptPerf8)
{
DWORD dwRes = FNS_FAIL;
HRESULT hr = S_OK;
CICMusic Music;

    //Play some music whose tempo we're gonna change.
    hr = Music.Init(ptPerf8, g_szDefaultMedia, DMUS_APATH_SHARED_STEREOPLUSREVERB);
    if (S_OK != hr)
    {
        fnsLog(ABORTLOGLEVEL, "Music.Init %s failed with %s (%08Xh)", g_szDefaultMedia, tdmXlatHRESULT(hr), hr);
	    goto TEST_END;
    }


    dwRes = FNS_PASS;

TEST_END:
    return dwRes;
};


 