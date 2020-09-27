//+-------------------------------------------------------------------------
//
//  Microsoft Windows
//
//  Copyright (C) Microsoft Corporation, 1997 - 1999
//
//  File:       Helpers.cpp
//
//--------------------------------------------------------------------------

#define INITGUID
#include "globals.h"

//==========================================================================
// dmthCreatePerformance
//==========================================================================
HRESULT dmthCreatePerformance(REFIID riid, CtIDirectMusicPerformance8 **pwPerformance)
{
    HRESULT                     hr = E_NOTIMPL;
    IDirectMusicPerformance8    *pdmPerformance = NULL;

    if(!pwPerformance)
    {
        return E_INVALIDARG;
    }
    // CoCreate Loader object
    hr = dmthCoCreateInstance(CLSID_DirectMusicPerformance,
                          NULL,
                          CLSCTX_INPROC_SERVER,
                          riid,
                          (LPVOID*)&pdmPerformance);
    if(SUCCEEDED(hr) && pdmPerformance)
    {
        // Wrap the Performance object
        *pwPerformance = new CtIDirectMusicPerformance8();
        if(NULL != *pwPerformance)
        {
            hr = (*pwPerformance)->InitTestClass(pdmPerformance);
            if(FAILED(hr))
            {
                (*pwPerformance)->Release();
                (*pwPerformance) = NULL;
            }
        }
        else
        {
            hr = E_OUTOFMEMORY;
        }
    }
    // clean up
    if (pdmPerformance)
    {
        pdmPerformance->Release();
        pdmPerformance = NULL;
    }

    return hr;
}



//==========================================================================
// dmthLoadSegment
//
// Does all the leg work for creating a Segment for you.
//==========================================================================
HRESULT dmthLoadSegment(CHAR *pSegmentFile,
                      CtIDirectMusicSegment8 **ppwSegment)
{
    if(pSegmentFile == NULL)
        pSegmentFile = "T:\\Media\\Perf8\\GetSegSt\\DSegTst3.SGT";

    HRESULT                     hr              = E_NOTIMPL;
    CtIDirectMusicLoader8        *pwLoader       = NULL;
    char szSegmentName[MAX_PATH];
    char szSegmentPath[MAX_PATH];


    //Take this path 
    hr = ChopPath(pSegmentFile, szSegmentPath, szSegmentName);
    if (FAILED(hr))
    {
        fnsLog(ABORTLOGLEVEL, "**** ABORT: dmthLoadSegment's ChopPath(%s) failed with %s (%08Xh)", pSegmentFile, tdmXlatHRESULT(hr), hr);
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
    // 4) Load the Segment Object
    // **********************************************************************
    DMUS_OBJECTDESC desc;
    ZeroMemory(&desc, sizeof(desc));
    desc.dwSize         = sizeof(DMUS_OBJECTDESC);
    desc.guidClass      = CLSID_DirectMusicSegment;
    desc.dwValidData    |= (DMUS_OBJ_CLASS | DMUS_OBJ_FILENAME);

    hr = pwLoader->SetSearchDirectory(GUID_DirectMusicAllTypes, szSegmentPath, FALSE);
    if (FAILED(hr))
    {
        fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: Loader->SetSeachDirectory(media directory) ")
                TEXT("failed (%s === %08Xh)"),
                tdmXlatHRESULT(hr), hr);
        goto TEST_END;
    }

    // Get the Segment object
    mbstowcs(desc.wszFileName, szSegmentName, strlen(szSegmentName) + 1);
    hr = pwLoader->GetObject(&desc, CTIID_IDirectMusicSegment8, (void **)ppwSegment);
    if(FAILED(hr))
    {
        fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: Loader->GetObject(Segment) ")
                TEXT("failed (%s == %08Xh)"),
                tdmXlatHRESULT(hr), hr);
        goto TEST_END;
    }

TEST_END:
    if(pwLoader)
    {
        pwLoader->Release();
        pwLoader = NULL;
    }
    return hr; //end dmthLoadSegment
}


//===========================================================================
// TDMGetBogusPointer
//
// Creates a bad pointer.
//
// Parameters: none
//
// Returns: LPVOID
//
// History:
//  12/22/1997 - a-llucar - taken from tdinput sources
//===========================================================================
LPVOID TDMGetBogusPointer(void)
{

    // allocate some memory and mark it not accessable
    return VirtualAlloc(NULL, 8, MEM_COMMIT, PAGE_NOACCESS);

} // *** end dihelpGetBogusPointer()


//===========================================================================
// TDMFreeBogusPointer
//
// Frees bad pointer created by helpGetBogusPointer.
//
// Parameters:
//  LPVOID lpv  - pointer to free
//
// Returns: nothing
//
// History:
//  12/22/1997 - a-llucar - taken from tdinput sources
//  1/22/1998 - a-kellyc - added VirtualProtect
//===========================================================================
void TDMFreeBogusPointer(LPVOID lpv)
{
    DWORD   dwOld;

	if(lpv)
	{
		// set the access rights back to read/write
		VirtualProtect(lpv, 8, PAGE_READWRITE, &dwOld);

		// release memory pointed to by lpv
		VirtualFree(lpv, 0, MEM_RELEASE);

		// set it to NULL
		lpv = NULL;
	}
} 


//===========================================================================
// tdmPackDWORD
//
// Packs two WORDs into a DWORD.
//
// Parameters:
//
// Returns: The packed DWORD
//
// History:
//  10/13/1998 - davidkl - created
//===========================================================================
DWORD tdmPackDWORD(WORD wHigh, WORD wLow)
{
    DWORD dw    = 0;

    // start with the high word
    dw = wHigh;
    dw = dw << 16;

    // or in the low word
    dw |= wLow;

    return dw;

} // *** end tdmPackDWORD()


//===========================================================================
// tdmEnableBreakOnDMAssert
//
// Turns on/off the DebugBreak in DirectMusic assertions by adding the
//  AssertBreak and setting the value to 1 (on) or 0 (off)
//
// Parameters:
//
// Returns: BOOL (return code from WriteProfileString)
//
// History:
//  10/22/1998 - davidkl - created
//===========================================================================
BOOL tdmEnableBreakOnDMAssert(BOOL fBreak)
{
    return WriteProfileStringA(TEXT("Debug"),
                    TEXT("AssertBreak"),
                    (fBreak) ? TEXT("1") : TEXT("0"));

} // *** end tdmEnableBreakOnDMAssert()

/*
//Types of data for loader to scan.
static GUID* g_ScanTypes[] = {
                    (GUID*)&CLSID_DirectMusicAudioPathConfig,
                    (GUID*)&CLSID_DirectMusicBand,
                    (GUID*)&CLSID_DirectMusicContainer,
                    (GUID*)&CLSID_DirectMusicCollection,
//                    (GUID*)&CLSID_DirectMusicChordMap,
                    (GUID*)&CLSID_DirectMusicScript,
//                    (GUID*)&CLSID_DirectMusicSong,
//                    (GUID*)&CLSID_DirectMusicStyle,
                    (GUID*)&CLSID_DirectMusicSegment,
//                    (GUID*)&CLSID_DirectMusicGraph,
//                    (GUID*)&CLSID_DirectSoundWave,
                    };
static DWORD g_dwScanTypes = AMOUNT(g_ScanTypes);
*/



/**********************************************************************
**********************************************************************/
BOOL FillBufferWithSineWave 
(
 LPVOID             pBuffer,
 DWORD              dwBufferBytes,
 DWORD              dwFrequency,
 LPWAVEFORMATEX     pWfx,
 DOUBLE             fFactor
 )
{
    int                 i       = 0;
    double              lfPhase = 0;
    const double        pi      = 3.14159265;
    ASSERT(pBuffer);
    if (dwBufferBytes != 0)
    {
        switch (pWfx->wBitsPerSample)
        {
        case 8:
            while (i < (LONG) dwBufferBytes)
            {
                int    nMag;
                int j;
                lfPhase = 2.0*pi*((double)dwFrequency)*i
                          /pWfx->nSamplesPerSec/pWfx->nBlockAlign;
                nMag = (int)(255*sin(lfPhase)+128);

                nMag = (int) (nMag * fFactor);
                for (j=0;j<pWfx->nChannels;j++)
                {
                    ((char*)pBuffer)[i++] = (char)nMag;
                }
            }
            break;
        case 16:
            while (i*2 < (LONG) dwBufferBytes)
            {
                int    nMag;
                int j;
                lfPhase = 2.0*pi*((double)dwFrequency)*i*2
                          /pWfx->nSamplesPerSec/pWfx->nBlockAlign;
                nMag = (int)(((double)0x7fff)*sin(lfPhase));

                nMag = (int) (nMag * fFactor);
                for (j=0;j<pWfx->nChannels;j++)
                {
                    ((short *)pBuffer)[i++] = (short)nMag;
                }
            }
            break;
        default:
            fnsLog(ABORTLOGLEVEL, "FillBufferWithSinWave: Unsupported Format");
            Log(4, "FillBufferWithSinWave: Unsupported Format, filling with silence.");
            memset(pBuffer, 0, dwBufferBytes);
            return TRUE;
        }
    }

    return TRUE;
};




/**********************************************************************
Added by danhaff 07-17-00

PURPOSE:
    For each stage, lists whether the dwPChannel parameter is used or not.
    If it's used, then one should pass DMUS_PCHANNEL_ALL.  Otherwise
    they should pass 0.
**********************************************************************/
struct STAGEDATA
{
    DWORD dwStage;
    BOOL  bPChannelUsed;
};

STAGEDATA g_StageData[] = 
{
    {DMUS_PATH_SEGMENT          , FALSE},
//    {DMUS_PATH_SEGMENT_TRACK    , FALSE},
//    {DMUS_PATH_SEGMENT_GRAPH    , FALSE},
//    {DMUS_PATH_SEGMENT_TOOL     , TRUE},
    {DMUS_PATH_AUDIOPATH        , FALSE},
//    {DMUS_PATH_AUDIOPATH_GRAPH  , FALSE},
//    {DMUS_PATH_AUDIOPATH_TOOL   , TRUE},
    {DMUS_PATH_PERFORMANCE      , FALSE},
//    {DMUS_PATH_PERFORMANCE_GRAPH, FALSE},
//    {DMUS_PATH_PERFORMANCE_TOOL , TRUE},
//    {DMUS_PATH_PORT             , TRUE},
    {DMUS_PATH_BUFFER           , TRUE},
//    {DMUS_PATH_BUFFER_DMO       , TRUE},
    {DMUS_PATH_MIXIN_BUFFER     , FALSE},
//    {DMUS_PATH_MIXIN_BUFFER_DMO , FALSE},
//    {DMUS_PATH_PRIMARY_BUFFER   , FALSE}
};



/**********************************************************************
Added by danhaff 07-17-00

PURPOSE:
    Based on the stage, returns a value that says to GetObjectInPath,
    "Ignore the PChannel".
**********************************************************************/
DWORD IgnorePChannel(DWORD dwStage)
{
    DWORD i=0;

    for (i=0; i<AMOUNT(g_StageData); i++)
    {
        if (dwStage == g_StageData[i].dwStage)
        {
            if (g_StageData[i].bPChannelUsed)
                return DMUS_PCHANNEL_ALL;
            else
                return 0;
        }
    }

    //If we made it through, we were passed an invalid value.
    fnsLog(ABORTLOGLEVEL, 
           TEXT("**** ABORT:  TEST_APP ERROR!!!  dwPChannelIgnore passed invalid stage %s (%08Xh)"),  dmthXlatDMUS_STAGE(dwStage), dwStage);
    
    //This should NEVER EVER EVER HAPPEN!!!
    return -1;
};




/**********************************************************************
Added by danhaff 08-02-00

PURPOSE:
    Releases an IUnknown if a call succeeded, otherwise just NULLs it
    out again.  Prints error if API didn't do it's job.
**********************************************************************/
void CleanUpIUnknown(HRESULT hr, LPUNKNOWN &pUnk)
{
    //Make sure we got rid of pUnk if this succeeded unexpectedly.
    if (SUCCEEDED(hr))
    {
        SAFE_RELEASE(pUnk);
    }

    if (FAILED(hr))
    {
        if (pUnk)
        {
            fnsLog(ABORTLOGLEVEL,"**** FYI:  GetObjectInPath didn't zero out *ppUnkObject");
            pUnk = NULL;
        }
    }
};



/**********************************************************************
Added by danhaff 08-08-00

PURPOSE:
    What it says.

NOTES:
    Copied out of the shell code on index 2 because this isn't on 
    @#$@% Windows 95.
**********************************************************************/
#define TEXTW(quote) L##quote      
LPCWSTR PathFindFileNameW(LPCWSTR pPath)
{
    LPCWSTR pT;

    for (pT = pPath; *pPath; pPath++) {
        if ((pPath[0] == TEXTW('\\') || pPath[0] == TEXTW(':') || pPath[0] == TEXTW('/'))
            && pPath[1] &&  pPath[1] != TEXTW('\\')  &&   pPath[1] != TEXTW('/'))
            pT = pPath + 1;
    }

    return pT;
}



/**********************************************************************
**********************************************************************/
LPWSTR PathFindFileNameW(LPWSTR pPath)
{
    LPWSTR pT;

    for (pT = pPath; *pPath; pPath++) {
        if ((pPath[0] == TEXTW('\\') || pPath[0] == TEXTW(':') || pPath[0] == TEXTW('/'))
            && pPath[1] &&  pPath[1] != TEXTW('\\')  &&   pPath[1] != TEXTW('/'))
            pT = pPath + 1;
    }

    return pT;
}


/**********************************************************************
**********************************************************************/
LPCSTR PathFindFileName(LPCSTR pPath)
{
    LPCSTR pT;

    for (pT = pPath; *pPath; pPath++) {
        if ((pPath[0] == '\\' || pPath[0] == ':' || pPath[0] == '/')
            && pPath[1] &&  pPath[1] != '\\'  &&   pPath[1] != '/')
            pT = pPath + 1;
    }

    return pT;
}






//==========================================================================
//==========================================================================
// dmthPlayMidiFile
// 
// Creates all the objects necessary to create a MIDI event, Loads the Midi
// Event, and plays the MIDI segment.  Standard helper for many tests.

// Parameters:
//	REFIID to Graph
//  pointer to wrapped Graph object
//
//	NOTE:  This returns a Segment Object containing a sequence track.
//
//         DOES NOT SUPPORT UNWRAPPED OBJECTS
//==========================================================================
HRESULT dmthPlayMidiFile(CtIDirectMusicPerformance8 *pwPerformance8,
						 REFIID riid,
						 CtIDirectMusicSegment8 **ppwSegment8,
						 CtIDirectMusicSegmentState **ppwSegmentState,
                         CHAR *pMidiFile)

{		
	if (pMidiFile == NULL)
		pMidiFile = g_szDefaultMedia;

    HRESULT hr = S_OK;
	DMUS_OBJECTDESC DESC = {0};					// Descriptor to use to find it.
    CtIDirectMusicLoader8 *ptLoader8 = NULL;
    CtIDirectMusicSegment8 *ptSegment8 = NULL;
    CtIDirectMusicSegmentState8 *ptSegmentState = NULL;
    CtIDirectMusicAudioPath  *ptPath = NULL;

/*BUGBUG: Performance already init'd.
    hr = pwPerformance8->InitAudio(NULL, NULL, NULL, DMUS_APATH_SHARED_STEREOPLUSREVERB, 128, NULL, NULL);
    if (S_OK != hr)
    {
        fnsLog(ABORTLOGLEVEL,"**** ABORT:  pwPerformance->InitAudio failed with %s (%08X)",tdmXlatHRESULT(hr), hr);
        goto TEST_END;
    }
*/

    hr = pwPerformance8->CreateStandardAudioPath(DMUS_APATH_SHARED_STEREOPLUSREVERB, 64, TRUE, &ptPath);
    if (S_OK != hr)
    {
        fnsLog(ABORTLOGLEVEL,"**** ABORT: CreateStandardAudioPath failed with %s (%08X)",tdmXlatHRESULT(hr), hr);
        goto TEST_END;
    }

    hr = dmthCreateLoader(IID_IDirectMusicLoader8, &ptLoader8);
    if (S_OK != hr)
    {
        fnsLog(ABORTLOGLEVEL,"**** ABORT: dmthCreateLoader failed with %s (%08X)",tdmXlatHRESULT(hr), hr);
        goto TEST_END;
    }


    hr = ptLoader8->LoadObjectFromFile(CLSID_DirectMusicSegment, CTIID_IDirectMusicSegment8, pMidiFile, (void **)&ptSegment8);
    if (S_OK != hr)
    {
        fnsLog(ABORTLOGLEVEL,"**** ABORT: pLoader8->LoadObjectFromFile failed with %s (%08X)",tdmXlatHRESULT(hr), hr);
        goto TEST_END;
    }

    hr = ptSegment8->Download(pwPerformance8);
    if (S_OK != hr)
    {
        fnsLog(ABORTLOGLEVEL,"**** ABORT: ptSegment8->Download failed with %s (%08X)",tdmXlatHRESULT(hr), hr);
        goto TEST_END;
    }

    hr = pwPerformance8->PlaySegmentEx(ptSegment8, NULL, NULL, NULL, 0, &ptSegmentState, NULL, ptPath);
    if (S_OK != hr)
    {
        fnsLog(ABORTLOGLEVEL,"**** ABORT: pwPerformance->PlaySegmentEx failed with %s (%08X)",tdmXlatHRESULT(hr), hr);
        goto TEST_END;
    }

    *ppwSegment8        = ptSegment8;
    if (ppwSegmentState)
    {
        *ppwSegmentState   = ptSegmentState;
    }
    else
    {
        ptSegmentState->Release();
        *ppwSegmentState = NULL;
    }
	
TEST_END:

    SAFE_RELEASE(ptLoader8);
    SAFE_RELEASE(ptPath);
    return hr;


} // end dmthPlayMidiFile()



/********************************************************************************
********************************************************************************/
HRESULT CreateAndInitPerformance(CtIDirectMusicPerformance8 **pptPerf8)
{
HRESULT hr = S_OK;
CtIDirectMusicPerformance8 *ptPerf8 = NULL;

hr = dmthCreatePerformance(IID_IDirectMusicPerformance8, &ptPerf8);
if (FAILED(hr))
    goto END;

hr = ptPerf8->InitAudioX(NULL, NULL);
if (FAILED(hr))
{
    SAFE_RELEASE(ptPerf8);
    goto END;
}

END:
    *pptPerf8 = ptPerf8;
    return hr;

};


/********************************************************************************
********************************************************************************/
HRESULT ChopPath(const char *szFullString, char *szPathOut, char *szFileOut)
{
char *szFile = NULL;
char *szPath = NULL;
DWORD dwLength;

if (!szFullString)
{
    *szFile = NULL;
    return E_POINTER;
}

//Parse out the file name.
szFile = (char *)PathFindFileName(szFullString);
if (!szFile)
    return E_FAIL;

strcpy(szFileOut, szFile);

//Get the length of the directory name.
dwLength = strlen(szFullString) - strlen(szFile);

//Copy only that length, and set the final character of the out-string to NULL.
strncpy(szPathOut, szFullString, dwLength);
szPathOut[dwLength] = NULL;

return S_OK;
}
