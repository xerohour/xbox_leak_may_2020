/*++

Copyright (c) 2001 Microsoft Corporation

Module Name:

    Helpers.cpp

Abstract:

    Helper functions for dmtest1

Author:

    Dan Haffner (danhaff)

Revision History:

    13-Mar-2001 robheit
        Added input functions for joystick polling
    19-Mar-2001 robheit
        Added enableBreak functionality in WaitForSegmentStart/Stop
        Added dmthIsAnyButtonDown

--*/

#define __HELPERS_CPP__ // Must be first


//------------------------------------------------------------------------------
//  Includes:
//------------------------------------------------------------------------------
#include "globals.h"
#include "DSSTDFX.h"
#include "dxconio.h"

LPCSTR g_szAppName = "DMTest1";
THREADPRI g_ThreadPris[] =  {
                            {"THREAD_PRIORITY_IDLE",            THREAD_PRIORITY_IDLE},
                            {"THREAD_PRIORITY_LOWEST",          THREAD_PRIORITY_LOWEST},
                            {"THREAD_PRIORITY_BELOW_NORMAL",    THREAD_PRIORITY_BELOW_NORMAL},
                            {"THREAD_PRIORITY_NORMAL",          THREAD_PRIORITY_NORMAL},
                            {"THREAD_PRIORITY_ABOVE_NORMAL",    THREAD_PRIORITY_ABOVE_NORMAL},
                            {"THREAD_PRIORITY_TIME_CRITICAL",   THREAD_PRIORITY_TIME_CRITICAL}
                            };

DWORD g_dwThreadPris = NUMELEMS(g_ThreadPris);


//------------------------------------------------------------------------------
//  Globals
//------------------------------------------------------------------------------
HANDLE              g_dmthInputHandles[4] = {NULL, NULL, NULL, NULL};
DMTHJoystick        g_dmthJoysticks[4];
static const float  minThreshold = 0.0001f;

//Prototype this, since it's not public anymore.
BOOL dmthPromptUserYesNo(IN LPSTR    yesNoQuestion,IN ...);


//------------------------------------------------------------------------------
//  ::UINTToPort
//------------------------------------------------------------------------------
DWORD
UINTToPort(
           IN UINT port
           )
/*++

Routine Description:

    Converts an unsigned integer in the range of 0-3 to a XDEVICE port

Arguments:

    IN port -   Value to convert

Return Value:

    A port XDEVICE_PORT0 - XDEVICE_PORT3

--*/
{
    switch(port)
    {
    case 0:
        return XDEVICE_PORT0;
    case 1:
        return XDEVICE_PORT1;
    case 2:
        return XDEVICE_PORT2;
    case 3:
        return XDEVICE_PORT3;
    default:
        __asm int 3;
        return 0xFFFFFFFF;
    }


}

//------------------------------------------------------------------------------
//  ::InitInput
//------------------------------------------------------------------------------
static void 
InitInput(
          IN UINT port
          )
/*++

Routine Description:

    Initializes the DirectInput device for joystick polling

Arguments:

    IN port -   Port to initialize

Return Value:

    None

--*/
{
    // If the port has already been initialized, just return
    if(g_dmthInputHandles[port] != NULL)
        return;
    
    // Create a device
    g_dmthInputHandles[port] = XInputOpen(XDEVICE_TYPE_GAMEPAD, UINTToPort(port),
                                          0, NULL);
}

//==========================================================================
// dmthCreatePerformance
//==========================================================================
HRESULT dmthCreatePerformance(REFIID riid, CtIDirectMusicPerformance8 **pwPerformance)
{
    HRESULT                     hr = S_OK;
    IDirectMusicPerformance8    *pdmPerformance = NULL;

    if(!pwPerformance)
    {
        return E_INVALIDARG;
    }

    // CoCreate Loader object
    if(SUCCEEDED(hr)){
        hr = dmthDirectMusicCreateInstance(CLSID_DirectMusicPerformance,
                          NULL,
                          riid,
                          (LPVOID*)&pdmPerformance);
    }
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
    SAFE_RELEASE(pdmPerformance);
    return hr;
}



//==========================================================================
// dmthLoadSegment
//
// Does all the leg work for creating a Segment for you.
//==========================================================================
HRESULT dmthLoadSegment(LPCSTR pSegmentFile,
                      CtIDirectMusicSegment8 **ppwSegment)
{
    if(pSegmentFile == NULL)
    {
        Log(ABORTLOGLEVEL, "TEST APP ERROR: Don't pass NULL!!!");
        ASSERT(FALSE);
    }

    HRESULT                     hr              = E_NOTIMPL;
    CtIDirectMusicLoader8        *pwLoader       = NULL;
    char szSegmentName[MAX_PATH];
    char szSegmentPath[MAX_PATH];


    //Take this path 
    hr = ChopPath(pSegmentFile, szSegmentPath, szSegmentName);
    if (FAILED(hr))
    {
        Log(ABORTLOGLEVEL, "**** ABORT: dmthLoadSegment's ChopPath(%s) failed with %s (%08Xh)", pSegmentFile, tdmXlatHRESULT(hr), hr);
        goto TEST_END;
    }

    // **********************************************************************
    // 1) Create CtIDirectMusicLoader object (wrapped IDirectMusicLoader object)
    // **********************************************************************
    hr = dmthCreateLoader(IID_IDirectMusicLoader8, &pwLoader);
    if (FAILED(hr))
    {
        Log(ABORTLOGLEVEL, TEXT("**** ABORT: CoCreate Loader object ")
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
        Log(ABORTLOGLEVEL, TEXT("**** ABORT: Loader->SetSeachDirectory(media directory) ")
                TEXT("failed (%s === %08Xh)"),
                tdmXlatHRESULT(hr), hr);
        goto TEST_END;
    }

    // Get the Segment object
    mbstowcs(desc.wszFileName, szSegmentName, strlen(szSegmentName) + 1);
//    strcpy(desc.wszFileName, szSegmentName);
    hr = pwLoader->GetObject(&desc, CTIID_IDirectMusicSegment8, (void **)ppwSegment);
    if(FAILED(hr))
    {
        Log(ABORTLOGLEVEL, TEXT("**** ABORT: Loader->GetObject(Segment) ")
                TEXT("failed (%s == %08Xh)"),
                tdmXlatHRESULT(hr), hr);
        goto TEST_END;
    }

TEST_END:
    SAFE_RELEASE(pwLoader);
    return hr; //end dmthLoadSegment
}





//==========================================================================
// dmthLoadSegment
//
// Does all the leg work for creating a Segment for you.
// UNWRAPPED VERSION!!!
//==========================================================================
HRESULT dmthLoadSegment(LPCSTR pSegmentFile,
                      IDirectMusicSegment8 **ppSegment)
{
    if(pSegmentFile == NULL)
    {
        Log(ABORTLOGLEVEL, "TEST APP ERROR: Don't pass NULL!!!");
        ASSERT(FALSE);
    }

    HRESULT                     hr              = E_NOTIMPL;
    IDirectMusicLoader8        *pwLoader       = NULL;
    char szSegmentName[MAX_PATH];
    char szSegmentPath[MAX_PATH];


    //Take this path 
    hr = ChopPath(pSegmentFile, szSegmentPath, szSegmentName);
    if (FAILED(hr))
    {
        Log(ABORTLOGLEVEL, "**** ABORT: dmthLoadSegment's ChopPath(%s) failed with %s (%08Xh)", pSegmentFile, tdmXlatHRESULT(hr), hr);
        goto TEST_END;
    }

    // **********************************************************************
    // 1) Create CtIDirectMusicLoader object (wrapped IDirectMusicLoader object)
    // **********************************************************************
    hr = dmthCreateLoader(IID_IDirectMusicLoader8, &pwLoader);
    if (FAILED(hr))
    {
        Log(ABORTLOGLEVEL, TEXT("**** ABORT: CoCreate Loader object ")
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
        Log(ABORTLOGLEVEL, TEXT("**** ABORT: Loader->SetSeachDirectory(media directory) ")
                TEXT("failed (%s === %08Xh)"),
                tdmXlatHRESULT(hr), hr);
        goto TEST_END;
    }

    // Get the Segment object
    mbstowcs(desc.wszFileName, szSegmentName, strlen(szSegmentName) + 1);
//    strcpy(desc.wszFileName, szSegmentName);
    hr = pwLoader->GetObject(&desc, IID_IDirectMusicSegment8, (void **)ppSegment);
    if(FAILED(hr))
    {
        Log(ABORTLOGLEVEL, TEXT("**** ABORT: Loader->GetObject(Segment) ")
                TEXT("failed (%s == %08Xh)"),
                tdmXlatHRESULT(hr), hr);
        goto TEST_END;
    }

TEST_END:
    SAFE_RELEASE(pwLoader);
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
            Log(ABORTLOGLEVEL, "FillBufferWithSinWave: Unsupported Format");
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
//    {DMUS_PATH_MIXIN_BUFFER     , FALSE},
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
    Log(ABORTLOGLEVEL, 
           TEXT("**** ABORT:  TEST_APP ERROR!!!  dwPChannelIgnore passed invalid stage %s (%08Xh)"),  dmthXlatDMUS_STAGE(dwStage), dwStage);
    
    //This should NEVER EVER EVER HAPPEN!!!
    return 0xFFFFFFFF;
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
            Log(ABORTLOGLEVEL,"**** FYI:  GetObjectInPath didn't zero out *ppUnkObject");
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


/**********************************************************************
**********************************************************************/
LPCSTR PathFindFileExtension(LPCSTR pPath)
{
    LPCSTR pT;

    for (pT = pPath; *pT; pT++) 
    {
        if (*pT == '.')
        {
            pT++;
            return pT;
        }
    }

    return NULL;
}





//==========================================================================
//==========================================================================
// dmthPlayMidiFile
// 
// Creates all the objects necessary to create a MIDI event, Loads the Midi
// Event, and plays the MIDI segment.  Standard helper for many tests.

// Parameters:
//  REFIID to Graph
//  pointer to wrapped Graph object
//
//  NOTE:  This returns a Segment Object containing a sequence track.
//
//         DOES NOT SUPPORT UNWRAPPED OBJECTS
//==========================================================================
HRESULT dmthPlayMidiFile(CtIDirectMusicPerformance8 *pwPerformance8,
                         REFIID riid,
                         CtIDirectMusicSegment8 **ppwSegment8,
                         CtIDirectMusicSegmentState **ppwSegmentState,
                         LPSTR pMidiFile)

{       
    if (pMidiFile == NULL)
        pMidiFile = g_TestParams.szDefaultMedia;

    HRESULT hr = S_OK;
    CtIDirectMusicLoader8 *ptLoader8 = NULL;
    CtIDirectMusicSegment8 *ptSegment8 = NULL;
    CtIDirectMusicSegmentState8 *ptSegmentState = NULL;
    CtIDirectMusicAudioPath  *ptPath = NULL;

    hr = pwPerformance8->CreateStandardAudioPath(DMUS_APATH_SHARED_STEREOPLUSREVERB, 64, TRUE, &ptPath);
    if (S_OK != hr)
    {
        Log(ABORTLOGLEVEL,"**** ABORT: CreateStandardAudioPath failed with %s (%08X)",tdmXlatHRESULT(hr), hr);
        goto TEST_END;
    }

    hr = dmthCreateLoader(IID_IDirectMusicLoader8, &ptLoader8);
    if (S_OK != hr)
    {
        Log(ABORTLOGLEVEL,"**** ABORT: dmthCreateLoader failed with %s (%08X)",tdmXlatHRESULT(hr), hr);
        goto TEST_END;
    }


    hr = ptLoader8->LoadObjectFromFile(CLSID_DirectMusicSegment, CTIID_IDirectMusicSegment8, pMidiFile, (void **)&ptSegment8);
    if (S_OK != hr)
    {
        Log(ABORTLOGLEVEL,"**** ABORT: pLoader8->LoadObjectFromFile failed with %s (%08X)",tdmXlatHRESULT(hr), hr);
        goto TEST_END;
    }

    hr = pwPerformance8->PlaySegmentEx(ptSegment8, NULL, NULL, NULL, (__int64)0, &ptSegmentState, NULL, ptPath);
    if (S_OK != hr)
    {
        Log(ABORTLOGLEVEL,"**** ABORT: pwPerformance->PlaySegmentEx failed with %s (%08X)",tdmXlatHRESULT(hr), hr);
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


hr = ptPerf8->InitAudioX(NULL, NULL, g_TestParams.dwDMVoiceCount, g_TestParams.bDoWorkLocalThread ? DMUS_INITAUDIO_NOTHREADS : 0);
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
HRESULT ChopPath(const char *p_szFullString, LPSTR p_szPathOut, LPSTR p_szFileOut)
{
HRESULT hr = S_OK;
LPCSTR szFile = NULL;
DWORD dwLength;
CHAR szPathOut[MAX_PATH] = {0};
CHAR szFileOut[MAX_PATH] = {0};

ASSERT(p_szPathOut || p_szFileOut);
ASSERT(p_szFullString);

if (SUCCEEDED(hr))
{
    if (!p_szFullString)
    {
        hr = E_POINTER;
    }
}

//Parse out the file name.
if (SUCCEEDED(hr))
{
    //If the name ends in a '/' or a '\\' then there is no file name.
    if ('/' == p_szFullString[strlen(p_szFullString) - 1] ||
        '\\' == p_szFullString[strlen(p_szFullString) - 1] )
    {
        szFile = &p_szFullString[strlen(p_szFullString)];
    }
    else
    {    
        szFile = (LPSTR)PathFindFileName(p_szFullString);
    }

    //Should never happen.
    ASSERT(szFile);

    //Store the file name.
    strcpy(szFileOut, szFile);
}

if (SUCCEEDED(hr))
{
    //Store the path name.
    dwLength = strlen(p_szFullString) - strlen(szFile);
    strncpy(szPathOut, p_szFullString, dwLength);
    szPathOut[dwLength] = NULL;
}

//Regardless of whether the test passed, set these out-parameters.
if (p_szPathOut)
    strcpy(p_szPathOut, szPathOut);
if (p_szFileOut)
    strcpy(p_szFileOut, szFileOut);

return S_OK;
}


/********************************************************************************
********************************************************************************/
HRESULT Wait(DWORD dwWait)
{
    if (g_TestParams.bWait)
        Sleep(dwWait);
    return S_OK;
}


/********************************************************************************
********************************************************************************/
HRESULT CountDown(DWORD dwWait, DWORD dwInterval)
{
DWORD dwNumIntervals; //How many intervals.
DWORD dwLastInterval; //Whatever's left over.
DWORD dwTotalTime = 0;
DWORD i = 0;

if (dwWait==0)
{
    Log(FYILOGLEVEL, "Waiting 0 milliseconds");
    return S_OK;
}

if (dwInterval==0)
    dwInterval = dwWait;

dwLastInterval = dwWait % dwInterval;
dwNumIntervals = dwWait / dwInterval;

Log(FYILOGLEVEL, "Counting up to %d...", dwWait);
Log(FYILOGLEVEL, "%d...", dwTotalTime);

for (i=0; i<dwNumIntervals; i++)
{    
    Wait(dwInterval);
    dwTotalTime += dwInterval;
    Log(FYILOGLEVEL, "%d...", dwTotalTime);
}

Wait(dwLastInterval);

return S_OK;
}



//===========================================================================
// dmthCreateEmptySegment
//
// Creates a wrapped version of IDirectMusicSegment which contains no tracks.
//
// Parameters:
//
// Returns: HRESULT
//
// History:
//  03/12/1998 - davidkl - created
//  04/28/1998 - davidkl - renamed to indicate that no tracks will be present
//  11/24/1999 - kcraven - moved from dmth to tdmusic to be consistant with other create helpers
//===========================================================================
HRESULT dmthCreateEmptySegment(CtIDirectMusicSegment **ppwSegment)
{
    HRESULT             hRes        = E_NOTIMPL;
    IDirectMusicSegment *pdmSegment = NULL;

    // get a real DirectMusicSegment object
    hRes = DirectMusicCreateInstance(CLSID_DirectMusicSegment,
                            NULL,               // don't support aggregation yet
                            IID_IDirectMusicSegment,
                            (void**)&pdmSegment);

    // if successs and object is not null
    if(SUCCEEDED(hRes) && pdmSegment)
    {
        (*ppwSegment) = new CtIDirectMusicSegment;
        if (!(*ppwSegment))
        {
            Log(ABORTLOGLEVEL, TEXT("new CtIDirectMusicSegment failed"));
            goto TEST_END;
        }

        hRes = (*ppwSegment)->InitTestClass(pdmSegment);
        if(FAILED(hRes))
        {
            Log(ABORTLOGLEVEL, TEXT("**** ABORT: pwSegment->InitTestClass ")
                    TEXT("failed (%s == %08Xh)"),
                    tdmXlatHRESULT(hRes), hRes);
            goto TEST_END;
        }
    }
TEST_END:
    // cleanup temp object
    if(pdmSegment)
    {
        pdmSegment->Release();
    }

    // done
    return hRes;

} // end dmthCreateEmptySegment()

//------------------------------------------------------------------------------
//  WaitForSegmentStop
//------------------------------------------------------------------------------
HRESULT 
WaitForSegmentStop(
                   IN CtIDirectMusicPerformance8*   ptPerf8, 
                   IN CtIDirectMusicSegment8*       ptSegment8, 
                   IN CtIDirectMusicSegmentState8*  ptSegmentState8, 
                   IN DWORD                         dwTimeout,
                   IN BOOL                          enableBreak
                   )
/*++

Routine Description:

    Returns S_OK if the segment stopped within the specified time; E_FAIL otherwise.

Arguments:

    IN ptPerf8 -            Performance
    IN ptSegment8 -         Segment to query 
    IN ptSegmentState8 -    Segment state 
    IN dwTimeout -          Milleseconds to timeout
    IN enableBreak -        TRUE to enable the user/tester to break out

Return Value:

    S_OK if the segment stopped within the specified time; E_FAIL otherwise.

--*/
{
    DWORD   dwStartTime     = 0;
    HRESULT hr = E_FAIL;

    // Is break enabled?
    if(enableBreak)
    {
        Log(FYILOGLEVEL, TEXT("Press any button to stop waiting"));
        dmthWaitForAllButtonsUp();
    }

    dwStartTime = timeGetTime();
    while (1)
    {
        // Has the segment stopped playing?
        hr = ptPerf8->IsPlaying(ptSegment8, ptSegmentState8);
        if (FAILED(hr))
        {
            Log(FYILOGLEVEL, "WaitForSegmentStop: Returned %s (%08X)", tdmXlatHRESULT(hr), hr);            
            break;
        }
        else if(S_FALSE == hr)
        {            
            return S_OK;
        }
 
        // Is break enabled?
        else if(enableBreak && dmthIsAnyButtonDown())
        {
            dmthWaitForAllButtonsUp();
            return E_FAIL;
        }

        if (timeGetTime() - dwStartTime >= dwTimeout)
        {
            Log(FYILOGLEVEL, "WaitForSegmentStop: Timed out after %dms", timeGetTime() - dwStartTime);
            hr = S_FALSE;
            break;
        }
    }

//    Log(FYILOGLEVEL, "WaitForSegmentStop: Exitted");


    return hr;
}




//------------------------------------------------------------------------------
//  WaitForSegmentStop
//------------------------------------------------------------------------------
HRESULT 
WaitForSegmentStart(
                   IN CtIDirectMusicPerformance8*   ptPerf8, 
                   IN CtIDirectMusicSegment8*       ptSegment8, 
                   IN CtIDirectMusicSegmentState8*  ptSegmentState8, 
                   IN DWORD                         dwTimeout,
                   IN BOOL                          enableBreak
                   )
/*++

Routine Description:

    Returns S_OK if the segment stopped within the specified time; E_FAIL otherwise.

Arguments:

    IN ptPerf8 -            Performance
    IN ptSegment8 -         Segment to query 
    IN ptSegmentState8 -    Segment state 
    IN dwTimeout -          Milleseconds to timeout
    IN enableBreak -        TRUE to enable the user/tester to break out

Return Value:

    S_OK if the segment stopped within the specified time; E_FAIL otherwise.

--*/
{
    DWORD   dwStartTime     = 0;
    HRESULT hr = E_FAIL;

    // Is break enabled?
    if(enableBreak)
    {
        Log(FYILOGLEVEL, TEXT("Press any button to stop waiting"));
        dmthWaitForAllButtonsUp();
    }

    dwStartTime = timeGetTime();
    while (1)
    {
        // Has the segment started playing?
        hr = ptPerf8->IsPlaying(ptSegment8, ptSegmentState8);
        if (FAILED(hr))
        {
            Log(FYILOGLEVEL, "WaitForSegmentStart: Returned %s (%08X)", tdmXlatHRESULT(hr), hr);            
            break;
        }
        else if(S_OK == hr)
        {            
            return S_OK;
        }
 
        // Is break enabled?
        else if(enableBreak && dmthIsAnyButtonDown())
        {
            dmthWaitForAllButtonsUp();
            return E_FAIL;
        }

        if (timeGetTime() - dwStartTime >= dwTimeout)
        {
            Log(FYILOGLEVEL, "WaitForSegmentStart: Timed out after %dms", timeGetTime() - dwStartTime);
            hr = S_FALSE;
            break;
        }
    }

    return hr;
}



/********************************************************************************
Returns S_OK if the segment started within the specified time; E_FAIL otherwise.
Prints a message on error.
********************************************************************************/
HRESULT ExpectSegmentStart(CtIDirectMusicPerformance8 *ptPerf8, CtIDirectMusicSegment8 *ptSegment8, CtIDirectMusicSegmentState8 *ptSegmentState8, DWORD dwTimeout, LPCSTR szSegname, BOOL bEnableBreak)
{
HRESULT hr = S_OK;
hr = WaitForSegmentStart(ptPerf8, ptSegment8, ptSegmentState8, dwTimeout, bEnableBreak);
if (hr == S_FALSE)
{
    Log(ABORTLOGLEVEL, "ERROR: ""%s"" didn't start playing after %d ms", szSegname, dwTimeout);
    hr = E_FAIL;
}

return hr;
};


/********************************************************************************
Returns S_OK if the segment stopped within the specified time; E_FAIL otherwise.
Prints a message on error.
********************************************************************************/
HRESULT ExpectSegmentStop(CtIDirectMusicPerformance8 *ptPerf8, CtIDirectMusicSegment8 *ptSegment8, CtIDirectMusicSegmentState8 *ptSegmentState8, DWORD dwTimeout, LPCSTR szSegname, BOOL bEnableBreak)
{
HRESULT hr = S_OK;
hr = WaitForSegmentStop(ptPerf8, ptSegment8, ptSegmentState8, dwTimeout, bEnableBreak);
if (hr == S_FALSE)
{
    Log(ABORTLOGLEVEL, "ERROR: ""%s"" didn't stop playing after %d ms", szSegname, dwTimeout);
    hr = E_FAIL;
}

return hr;
};


/********************************************************************************
Gets the current tempo.
********************************************************************************/
HRESULT GetTempo(CtIDirectMusicPerformance8 *ptPerf8, double *pdblTempo)
{
MUSIC_TIME mtTime = 0;
HRESULT hr = S_OK;
DMUS_TEMPO_PARAM Tempo = {NULL};
MUSIC_TIME mt = 0;

CHECKRUN(ptPerf8->GetTime(NULL, &mt));
CHECKRUN(ptPerf8->GetParamEx(GUID_TempoParam, 0, 0xFFFFFFFF, 0, mtTime, NULL, (void *)&Tempo));

if (SUCCEEDED(hr))
    *pdblTempo = Tempo.dblTempo;
else
    *pdblTempo = 0.f;

return hr;



}



/********************************************************************************
Caller must release the "real" tool.
********************************************************************************/
HRESULT WrapTool(IDirectMusicTool *pTool, CtIDirectMusicTool **pptTool)
{
    HRESULT             hr          = E_NOTIMPL;
    CtIDirectMusicTool *ptTool = NULL;

    if(!pTool)
    {
        return E_INVALIDARG;
    }

    ALLOCATEANDCHECK(ptTool, CtIDirectMusicTool);
    CHECKRUN(ptTool->InitTestClass(pTool));
    *pptTool = ptTool;
    return hr;

};


/********************************************************************************
********************************************************************************/
HRESULT dmthCreateToolGraph( CtIDirectMusicPerformance8* ptPerf8, IDirectMusicGraph** ppGraph )
{
    if ( NULL == ppGraph || NULL == ptPerf8 )
        return E_POINTER;

    HRESULT hr = S_OK;

    IDirectMusicPerformance* pPerf = NULL;
    IDirectMusicAudioPath* pPath = NULL;
    IDirectMusicGraph* pGraph = NULL;

    CHECKRUN( ptPerf8->GetRealObjPtr( &pPerf ) );
    CHECKALLOC( pPerf );

    CHECKRUN( pPerf->CreateStandardAudioPath( DMUS_APATH_SHARED_STEREOPLUSREVERB, 16, TRUE, &pPath ) );
    CHECKALLOC( pPath );

    CHECKRUN( pPerf->SetDefaultAudioPath( pPath ) );

    CHECKRUN( pPath->GetObjectInPath( 0, DMUS_PATH_PERFORMANCE_GRAPH, 0, CLSID_DirectMusicGraph, 0, IID_IDirectMusicGraph, (LPVOID*) &pGraph ) );
    CHECKALLOC( pGraph );

    if ( SUCCEEDED( hr ) )
    {
        *ppGraph = pGraph;
        pGraph->AddRef();
    }

    RELEASE( pGraph );
    RELEASE( pPath );
    RELEASE( pPerf );

    return hr;
}



//==========================================================================
// dmthCreateLoader8
//
// CoCreates a loader, wraps the loader object, and does an init test class
//
// Parameters:
//  REFIID to Loader
//  pointer to wrapped loader object
//
// History:
//  03/17/98    a-llucar    Created
//  04/16/1998 - davidkl - cleaned up a bit.  moved here from tdmusic
//  02/10/2000 - kcraven  create overloaded method
//==========================================================================
HRESULT dmthCreateLoader(REFIID riid, CtIDirectMusicLoader8 **pptLoader)
{
    HRESULT hr          = E_NOTIMPL;
    IDirectMusicLoader8 *prLoader   = NULL;
    CtIDirectMusicLoader8  *ptLoader  = NULL;

    // CoCreate Loader object
    hr = DirectMusicCreateInstance(
        CLSID_DirectMusicLoader,
        NULL,        
        IID_IDirectMusicLoader8,
        (LPVOID*)&prLoader);
    if(FAILED(hr))
    {
        Log(MINLOGLEVEL,
            "**** CoCreate IDirectMusicLoader8 object "
            "failed (%s == %08Xh)",tdmXlatHRESULT(hr), hr);
        goto END;
    }


    // Wrap the Loader object
    ptLoader = new CtIDirectMusicLoader8();
    if(NULL == ptLoader)
    {
        Log(MINLOGLEVEL,
            "**** Unable to create test wrapped object "
            "(%s == %08Xh)",tdmXlatHRESULT(hr), hr);
        goto END;
    }
    hr = ptLoader->InitTestClass(prLoader);
    if(FAILED(hr))
    {
        Log(MINLOGLEVEL,
            "**** Unable to initialize test wrapped object "
            "(%s == %08Xh)",tdmXlatHRESULT(hr), hr);
        goto END;
    }

    // Set the GM DLS Collection so we can load midi files.
    if (g_TestParams.bUseGM_DLS)
    {
        LPCSTR szGM = "DMusic/DMTest1/Random/GM.DLS";
	    
        //Copy this over.
        CHECKRUN(MediaCopyFile(szGM));
        if(FAILED(hr))
        {
            Log(MINLOGLEVEL, "**** Failed to copy GM.DLS (%s == %08Xh)",tdmXlatHRESULT(hr), hr);
            goto END;
        }
        
        DMUS_OBJECTDESC DESC;					
	    memset( &DESC, 0, sizeof(DMUS_OBJECTDESC) );
	    DESC.dwSize = sizeof (DMUS_OBJECTDESC);
	    DESC.guidClass = CLSID_DirectMusicCollection;  
	    wcscpy(DESC.wszFileName,L"t:\\DMTest1\\random\\gm.dls");
	    DESC.guidObject = GUID_DefaultGMCollection;
	    DESC.dwValidData = DMUS_OBJ_CLASS | DMUS_OBJ_FULLPATH | DMUS_OBJ_OBJECT;
	    hr = ptLoader->SetObject(&DESC);
        if(FAILED(hr))
        {
            fnsLog(MINLOGLEVEL,
                "**** Unable to SetObject on GM.DLS object "
                "(%s == %08Xh)",tdmXlatHRESULT(hr), hr);
            goto END;
        }
    }


    // clean up
END:
    if (prLoader)
    {
            prLoader->Release();
    }
    if (ptLoader)
    {
            *pptLoader = ptLoader;
    }
    return hr;
} // *** end dmthCreateLoader8






//==========================================================================
// dmthCreateLoader8
// UNWRAPPED VERSION!!!
//==========================================================================
HRESULT dmthCreateLoader(REFIID riid, IDirectMusicLoader8 **pptLoader)
{
    HRESULT hr          = E_NOTIMPL;
    IDirectMusicLoader8 *prLoader   = NULL;

    // CoCreate Loader object
    hr = DirectMusicCreateInstance(
        CLSID_DirectMusicLoader,
        NULL,        
        IID_IDirectMusicLoader8,
        (LPVOID*)&prLoader);
    if(FAILED(hr))
    {
        Log(MINLOGLEVEL,
            "**** CoCreate IDirectMusicLoader8 object "
            "failed (%s == %08Xh)",tdmXlatHRESULT(hr), hr);
        goto END;
    }

    // clean up
END:
    *pptLoader = prLoader;
    return hr;

} // *** end dmthCreateLoader



struct TRIO
{
LPSTR szExtension;
GUID *pCLSID;
GUID *pIID;
};

TRIO trio[] = 
{
{"aud", (GUID *)&CLSID_DirectMusicAudioPathConfig, (GUID *)&IID_IUnknown},
//{"bnd", (GUID *)&CLSID_DirectMusicBand,             (GUID *)&IID_IDirectMusicBand},
{"cdm", (GUID *)&CLSID_DirectMusicChordMap, (GUID *)&IID_IUnknown},
{"con", (GUID *)&CLSID_DirectMusicContainer, (GUID *)&IID_IUnknown},
{"dls", (GUID *)&CLSID_DirectMusicCollection, (GUID *)&IID_IUnknown},
{"sgt", (GUID *)&CLSID_DirectMusicSegment, (GUID *)&IID_IDirectMusicSegment},
{"sty", (GUID *)&CLSID_DirectMusicStyle, (GUID *)&IID_IUnknown},
{"spt", (GUID *)&CLSID_DirectMusicScript, (GUID *)&IID_IDirectMusicScript},
{"wav", (GUID *)&CLSID_DirectMusicSegment, (GUID *)&IID_IDirectMusicSegment}
};



HRESULT GUIDsFromFileName(LPSTR szFilename, GUID **ppCLSID, GUID **ppIID)
{
LPSTR szExt = NULL;
DWORD i = 0;
HRESULT hr = S_OK;

    //Find the extension.
    szExt = (LPSTR)PathFindFileExtension(szFilename);

    for (i=0; i<AMOUNT(trio); i++)
    {
        if (_strcmpi(trio[i].szExtension, szExt) == 0)
            break;
    }

    //Make sure we found it.
    if (i==AMOUNT(trio))
    {
        Log(ABORTLOGLEVEL, "Test App Error!  Couldn't find %s in array", szExt);
        hr = E_FAIL;
        goto END;
    }

    
    *ppCLSID  = trio[i].pCLSID;    
    *ppIID    = trio[i].pIID;    

END:
    return hr;
}



//------------------------------------------------------------------------------
//  ::dmthInitInput
//------------------------------------------------------------------------------
void 
dmthInitInput(void)
/*++

Routine Description:

    Initializes the DirectInput device for joystick polling

Arguments:

    None

Return Value:

    None

--*/
{
    DWORD   dwInputDevices;
    UINT    i;
    
    // Get the devices
    dwInputDevices = XGetDevices(XDEVICE_TYPE_GAMEPAD);

    // Save time and drop out now if no joysticks are plugged in
    if(!dwInputDevices)
    {
        dmthReleaseInput();
        return;
    }
    
    // Create a device for each port
    for(i=0; i<4; ++i) 
    {
        if(dwInputDevices & (1 << i)) 
            InitInput(i);
        else
            dmthReleaseInput(i);
    }
}

//------------------------------------------------------------------------------
//  ::dmthReleaseInput
//------------------------------------------------------------------------------
void 
dmthReleaseInput(
                 IN UINT port
                 )
/*++

Routine Description:

    Releases the DirectInput devices

Arguments:

    IN port -   Port to release

Return Value:

    None

--*/
{
    if(g_dmthInputHandles[port]) 
    {
        XInputClose(g_dmthInputHandles[port]);
        g_dmthInputHandles[port] = NULL;
    }
}

//------------------------------------------------------------------------------
//  ::dmthReleaseInput
//------------------------------------------------------------------------------
void 
dmthReleaseInput(void)
/*++

Routine Description:

    Releases the DirectInput devices

Arguments:

    None

Return Value:

    None

--*/
{
    UINT    i;

    for(i=0; i<4; ++i)
        dmthReleaseInput(i);
}

//------------------------------------------------------------------------------
//  ::dmthGetJoystickStates
//------------------------------------------------------------------------------
void 
dmthGetJoystickStates(void)
/*++

Routine Description:

    Polls the state of all attached joysticks, storing the results in 
    g_dmthJoysticks

Arguments:

    None

Return Value:

    None

--*/
{
    UINT i;

    // Get the state of all ports
    for(i=0; i<4; ++i)
        dmthGetJoystickState(i);
}

//------------------------------------------------------------------------------
//  ::dmthGetJoystickState
//------------------------------------------------------------------------------
BOOL
dmthGetJoystickState(
                     IN UINT    port
                     )
/*++

Routine Description:

    Polls the state of a single joystick, storing the result in 
    g_dmthJoysticks[port]

Arguments:

    IN port -   Port to poll

Return Value:

    TRUE for a valid poll, FLASE otherwise

--*/
{
    XINPUT_STATE    istate;

    // Simple error checking
    if(!dmthIsPortValid(port))
        return FALSE;

    // Query the input state
    if(XInputGetState(g_dmthInputHandles[port], &istate) != ERROR_SUCCESS) 
        return FALSE;

    // Joystick stick states
    g_dmthJoysticks[port].leftStickX    =  (float)istate.Gamepad.sThumbLX / 32768.0f;
    g_dmthJoysticks[port].leftStickY    = -(float)istate.Gamepad.sThumbLY / 32768.0f;
    g_dmthJoysticks[port].rightStickX   =  (float)istate.Gamepad.sThumbRX / 32768.0f;
    g_dmthJoysticks[port].rightStickY   = -(float)istate.Gamepad.sThumbRY / 32768.0f;

    // Analog buttons
    g_dmthJoysticks[port].x             = istate.Gamepad.bAnalogButtons[XINPUT_GAMEPAD_X] / 255.0f;
    g_dmthJoysticks[port].y             = istate.Gamepad.bAnalogButtons[XINPUT_GAMEPAD_Y] / 255.0f;
    g_dmthJoysticks[port].white         = istate.Gamepad.bAnalogButtons[XINPUT_GAMEPAD_WHITE] / 255.0f;
    g_dmthJoysticks[port].a             = istate.Gamepad.bAnalogButtons[XINPUT_GAMEPAD_A] / 255.0f;
    g_dmthJoysticks[port].b             = istate.Gamepad.bAnalogButtons[XINPUT_GAMEPAD_B] / 255.0f;
    g_dmthJoysticks[port].black         = istate.Gamepad.bAnalogButtons[XINPUT_GAMEPAD_BLACK] / 255.0f;
    g_dmthJoysticks[port].leftTrigger   = istate.Gamepad.bAnalogButtons[XINPUT_GAMEPAD_LEFT_TRIGGER] / 255.0f;
    g_dmthJoysticks[port].rightTrigger  = istate.Gamepad.bAnalogButtons[XINPUT_GAMEPAD_RIGHT_TRIGGER] / 255.0f;

    // Digital buttons
    if(istate.Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_THUMB)
        g_dmthJoysticks[port].leftStick = TRUE;
    else
        g_dmthJoysticks[port].leftStick = FALSE;
    
    if(istate.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_THUMB)
        g_dmthJoysticks[port].rightStick = TRUE;
    else
        g_dmthJoysticks[port].rightStick = FALSE;
    
    if(istate.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_UP) 
        g_dmthJoysticks[port].dPadUp = TRUE;
    else
        g_dmthJoysticks[port].dPadUp = FALSE;
    
    if(istate.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_DOWN) 
        g_dmthJoysticks[port].dPadDown = TRUE;
    else
        g_dmthJoysticks[port].dPadDown = FALSE;
    
    if(istate.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_LEFT) 
        g_dmthJoysticks[port].dPadLeft = TRUE;
    else
        g_dmthJoysticks[port].dPadLeft = FALSE;
    
    if(istate.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_RIGHT) 
        g_dmthJoysticks[port].dPadRight = TRUE;
    else
        g_dmthJoysticks[port].dPadRight = FALSE;
    
    if(istate.Gamepad.wButtons & XINPUT_GAMEPAD_BACK) 
        g_dmthJoysticks[port].back = TRUE;
    else
        g_dmthJoysticks[port].back = FALSE;
    
    if(istate.Gamepad.wButtons & XINPUT_GAMEPAD_START) 
        g_dmthJoysticks[port].start = TRUE;
    else
        g_dmthJoysticks[port].start = FALSE;

    return TRUE;
}

//------------------------------------------------------------------------------
//  ::dmthWaitForAllButtons
//------------------------------------------------------------------------------
void
dmthWaitForAllButtons(
                      IN UINT   port,
                      IN DWORD  buttonMask
                      )
/*++

Routine Description:

    Waits for all buttons in the mask to be pressed.

    Note: Analog buttons/sticks must be pressed at least halfway for this to return TRUE

Arguments:

    IN port -       Port where event must occur
    IN buttonMask - Mask of buttons that will trigger a return

Return Value:

    None

--*/
{

    BOOL    notDone = TRUE;
    if (g_TestParams.bSkipUserInput)
        return;


    while(notDone)
    {
        notDone = FALSE;

        // Query all the joysticks
        if(dmthGetJoystickState(port))
        {
            if(((buttonMask & DMTH_JOYBUTTON_X) && (g_dmthJoysticks[port].x < 0.5f)) ||
               ((buttonMask & DMTH_JOYBUTTON_Y) && (g_dmthJoysticks[port].y < 0.5f)) ||
               ((buttonMask & DMTH_JOYBUTTON_WHITE) && (g_dmthJoysticks[port].white < 0.5f)) ||
               ((buttonMask & DMTH_JOYBUTTON_A) && (g_dmthJoysticks[port].a < 0.5f)) ||
               ((buttonMask & DMTH_JOYBUTTON_B) && (g_dmthJoysticks[port].b < 0.5f)) ||
               ((buttonMask & DMTH_JOYBUTTON_BLACK) && (g_dmthJoysticks[port].black < 0.5f)) ||
               ((buttonMask & DMTH_JOYBUTTON_LTRIG) && (g_dmthJoysticks[port].leftTrigger < 0.5f)) ||
               ((buttonMask & DMTH_JOYBUTTON_RTRIG) && (g_dmthJoysticks[port].rightTrigger < 0.5f)) ||
               ((buttonMask & DMTH_JOYBUTTON_LSTICK) && (g_dmthJoysticks[port].leftStick < 0.5f)) ||
               ((buttonMask & DMTH_JOYBUTTON_RSTICK) && (g_dmthJoysticks[port].rightStick < 0.5f)) ||
               ((buttonMask & DMTH_JOYBUTTON_UP) && (g_dmthJoysticks[port].dPadUp == FALSE)) ||
               ((buttonMask & DMTH_JOYBUTTON_DOWN) && (g_dmthJoysticks[port].dPadDown == FALSE)) ||
               ((buttonMask & DMTH_JOYBUTTON_LEFT) && (g_dmthJoysticks[port].dPadLeft == FALSE)) ||
               ((buttonMask & DMTH_JOYBUTTON_RIGHT) && (g_dmthJoysticks[port].dPadRight == FALSE)) ||
               ((buttonMask & DMTH_JOYBUTTON_BACK) && (g_dmthJoysticks[port].back == FALSE)) ||
               ((buttonMask & DMTH_JOYBUTTON_START) && (g_dmthJoysticks[port].start == FALSE)) ||
               (!(buttonMask & DMTH_JOYBUTTON_X) && (g_dmthJoysticks[port].x > minThreshold)) ||
               (!(buttonMask & DMTH_JOYBUTTON_Y) && (g_dmthJoysticks[port].y > minThreshold)) ||
               (!(buttonMask & DMTH_JOYBUTTON_WHITE) && (g_dmthJoysticks[port].white > minThreshold)) ||
               (!(buttonMask & DMTH_JOYBUTTON_A) && (g_dmthJoysticks[port].a > minThreshold)) ||
               (!(buttonMask & DMTH_JOYBUTTON_B) && (g_dmthJoysticks[port].b > minThreshold)) ||
               (!(buttonMask & DMTH_JOYBUTTON_BLACK) && (g_dmthJoysticks[port].black > minThreshold)) ||
               (!(buttonMask & DMTH_JOYBUTTON_LTRIG) && (g_dmthJoysticks[port].leftTrigger > minThreshold)) ||
               (!(buttonMask & DMTH_JOYBUTTON_RTRIG) && (g_dmthJoysticks[port].rightTrigger > minThreshold)) ||
               (!(buttonMask & DMTH_JOYBUTTON_LSTICK) && (g_dmthJoysticks[port].leftStick > minThreshold)) ||
               (!(buttonMask & DMTH_JOYBUTTON_RSTICK) && (g_dmthJoysticks[port].rightStick > minThreshold)) ||
               (!(buttonMask & DMTH_JOYBUTTON_UP) && (g_dmthJoysticks[port].dPadUp == TRUE)) ||
               (!(buttonMask & DMTH_JOYBUTTON_DOWN) && (g_dmthJoysticks[port].dPadDown == TRUE)) ||
               (!(buttonMask & DMTH_JOYBUTTON_LEFT) && (g_dmthJoysticks[port].dPadLeft == TRUE)) ||
               (!(buttonMask & DMTH_JOYBUTTON_RIGHT) && (g_dmthJoysticks[port].dPadRight == TRUE)) ||
               (!(buttonMask & DMTH_JOYBUTTON_BACK) && (g_dmthJoysticks[port].back == TRUE)) ||
               (!(buttonMask & DMTH_JOYBUTTON_START) && (g_dmthJoysticks[port].start == TRUE)))
            {
                notDone = TRUE;
            }
        }
    }
}

//------------------------------------------------------------------------------
//  ::dmthWaitForAnyButton
//------------------------------------------------------------------------------
DWORD
dmthWaitForAnyButton(
                     IN DWORD buttonMask
                     )
/*++

Routine Description:

    Waits for any button in the mask to be pressed.

    Note: Analog buttons/sticks must be pressed at least halfway for this to return TRUE

Arguments:

    IN buttonMask - Mask of buttons that will trigger a return

Return Value:

    The mask of the button(s) pressed

--*/
{
    BOOL    notDone = TRUE;
    DWORD   mask    = 0;
    UINT    i;

    if (g_TestParams.bSkipUserInput)
        return S_OK;


    while(notDone)
    {
        // Query all the joysticks
        dmthGetJoystickStates();

        for(i=0; i<4; ++i)
        {
            // Only check valid joysticks
            if(g_dmthInputHandles[i])
            {
                if((buttonMask & DMTH_JOYBUTTON_X) && (g_dmthJoysticks[i].x >= 0.5f))
                {
                   notDone  = FALSE;
                   mask     |= DMTH_JOYBUTTON_X;
                }

                if((buttonMask & DMTH_JOYBUTTON_Y) && (g_dmthJoysticks[i].y >= 0.5f))
                {
                   notDone  = FALSE;
                   mask     |= DMTH_JOYBUTTON_Y;
                }

                if((buttonMask & DMTH_JOYBUTTON_WHITE) && (g_dmthJoysticks[i].white >= 0.5f))
                {
                   notDone  = FALSE;
                   mask     |= DMTH_JOYBUTTON_WHITE;
                }

                if((buttonMask & DMTH_JOYBUTTON_A) && (g_dmthJoysticks[i].a >= 0.5f))
                {
                   notDone  = FALSE;
                   mask     |= DMTH_JOYBUTTON_A;
                }

                if((buttonMask & DMTH_JOYBUTTON_B) && (g_dmthJoysticks[i].b >= 0.5f))
                {
                   notDone  = FALSE;
                   mask     |= DMTH_JOYBUTTON_B;
                }

                if((buttonMask & DMTH_JOYBUTTON_BLACK) && (g_dmthJoysticks[i].black >= 0.5f))
                {
                   notDone  = FALSE;
                   mask     |= DMTH_JOYBUTTON_BLACK;
                }

                if((buttonMask & DMTH_JOYBUTTON_LTRIG) && (g_dmthJoysticks[i].leftTrigger >= 0.5f))
                {
                   notDone  = FALSE;
                   mask     |= DMTH_JOYBUTTON_LTRIG;
                }

                if((buttonMask & DMTH_JOYBUTTON_RTRIG) && (g_dmthJoysticks[i].rightTrigger >= 0.5f))
                {
                   notDone  = FALSE;
                   mask     |= DMTH_JOYBUTTON_RTRIG;
                }

                if((buttonMask & DMTH_JOYBUTTON_LSTICK) && (g_dmthJoysticks[i].leftStick >= 0.5f))
                {
                   notDone  = FALSE;
                   mask     |= DMTH_JOYBUTTON_LSTICK;
                }

                if((buttonMask & DMTH_JOYBUTTON_RSTICK) && (g_dmthJoysticks[i].rightStick >= 0.5f))
                {
                   notDone  = FALSE;
                   mask     |= DMTH_JOYBUTTON_RSTICK;
                }

                if((buttonMask & DMTH_JOYBUTTON_UP) && (g_dmthJoysticks[i].dPadUp == TRUE))
                {
                   notDone  = FALSE;
                   mask     |= DMTH_JOYBUTTON_UP;
                }

                if((buttonMask & DMTH_JOYBUTTON_DOWN) && (g_dmthJoysticks[i].dPadDown == TRUE))
                {
                   notDone  = FALSE;
                   mask     |= DMTH_JOYBUTTON_DOWN;
                }

                if((buttonMask & DMTH_JOYBUTTON_LEFT) && (g_dmthJoysticks[i].dPadLeft == TRUE))
                {
                   notDone  = FALSE;
                   mask     |= DMTH_JOYBUTTON_LEFT;
                }

                if((buttonMask & DMTH_JOYBUTTON_RIGHT) && (g_dmthJoysticks[i].dPadRight == TRUE))
                {
                   notDone  = FALSE;
                   mask     |= DMTH_JOYBUTTON_RIGHT;
                }

                if((buttonMask & DMTH_JOYBUTTON_BACK) && (g_dmthJoysticks[i].back == TRUE))
                {
                   notDone  = FALSE;
                   mask     |= DMTH_JOYBUTTON_BACK;
                }

                if((buttonMask & DMTH_JOYBUTTON_START) && (g_dmthJoysticks[i].start == TRUE))
                {
                   notDone  = FALSE;
                   mask     |= DMTH_JOYBUTTON_START;
                }
            }
        }
    }
    return mask;
}

//------------------------------------------------------------------------------
//  ::dmthIsPortValid
//------------------------------------------------------------------------------
BOOL
dmthIsPortValid(
                IN UINT port
                )
/*++

Routine Description:

    Returns TRUE if the port number is valid

Arguments:

    IN port -   Port to query

Return Value:

    TRUE if the port is valid (has a controller attached) FALSE otherwise

--*/
{
    if(g_dmthInputHandles[port] == NULL)
    {
        InitInput(port);
        return (g_dmthInputHandles[port] != NULL) ? TRUE : FALSE;
    }
    return TRUE;
}




//------------------------------------------------------------------------------
//  ::dmthPromptUserTestResults
//------------------------------------------------------------------------------
void
dmthPromptUserTestResults(
                    HRESULT &hr,
                    IN ANSWER   yesNoExpected,
                    IN LPSTR    yesNoQuestion,
                    ...
                    )


/*++

Routine Description:

Arguments:

Return Value:

--*/
{

    
    
    BOOL bResult = FALSE;
    va_list va = {0};
    char szBuffer[1000] = {0};

    if (FAILED(hr))
        return;

    if (g_TestParams.bSkipUserInput)
        return;

    va_start(va, yesNoQuestion);
    vsprintf(szBuffer, yesNoQuestion, va);
    va_end(va);

    bResult = dmthPromptUserYesNo(szBuffer);
    if (bResult && (yesNoExpected == EXPECT_NO))
    {
        Log(ABORTLOGLEVEL, "Answer was yes, expected no.");
        hr = E_FAIL;
    }
    if (!bResult && (yesNoExpected == EXPECT_YES))
    {
        Log(ABORTLOGLEVEL, "Answer was no, expected yes.");
        hr = E_FAIL;
    }

};





//------------------------------------------------------------------------------
//  ::dmthPromptUserYesNo
//------------------------------------------------------------------------------
BOOL
dmthPromptUserYesNo(
                    IN LPSTR    yesNoQuestion,
                    IN          ...
                    )
/*++

Routine Description:

    Prompts the user to respond to a yes/no question by pressing the A or B 
    button: A = Yes, B = No

Arguments:

    IN yesNoQuestion -  Question to ask the user (varargs)
    IN ... -            The varargs

Return Value:

    TRUE if the user pressed A (Yes), FALSE if the user pressed B (No)

--*/
{
    DWORD   response = 0;
    va_list va = {0};
    char    szBuffer[1000] = {0};

    if (g_TestParams.bSkipUserInput)
        return TRUE;

    va_start(va, yesNoQuestion);
    vsprintf(szBuffer, yesNoQuestion, va);
    va_end(va);
    
    //
    while(1)
    {
        // Display the question:
        Log(FYILOGLEVEL, TEXT("QUESTION: %s (A = Yes, B = No)"), szBuffer);

        // Wait for a response
        response = dmthWaitForAnyButton(DMTH_JOYBUTTON_A | DMTH_JOYBUTTON_B);

        // Then wait for all buttons to be up
        dmthWaitForAllButtonsUp();

        // Yes? Log success
        if(response == DMTH_JOYBUTTON_A)
        {
            Log(FYILOGLEVEL, TEXT("ANSWER: Yes"));
            return TRUE;
        }

        // No? Log Failure
        else if(response == DMTH_JOYBUTTON_B)
        {
            Log(FYILOGLEVEL, TEXT("ANSWER: No"));
            return FALSE;
        }
    }
}

//------------------------------------------------------------------------------
//  ::dmthWaitForAllButtonsUp
//------------------------------------------------------------------------------
void
dmthWaitForAllButtonsUp(void)
/*++

Routine Description:

    Waits until all joystick buttons are up

Arguments:

    None

Return Value:

    None

--*/
{
    UINT    i;
    BOOL    notDone = TRUE;

    if (g_TestParams.bSkipUserInput)
        return;

    while(notDone)
    {
        notDone = FALSE;

        // Query all the joysticks
        dmthGetJoystickStates();

        for(i=0; i<4; ++i)
        {
            // Only check valid joysticks
            if(g_dmthInputHandles[i])
            {
                if((g_dmthJoysticks[i].x > minThreshold) ||
                   (g_dmthJoysticks[i].y > minThreshold) ||
                   (g_dmthJoysticks[i].white > minThreshold) ||
                   (g_dmthJoysticks[i].a > minThreshold) ||
                   (g_dmthJoysticks[i].b > minThreshold) ||
                   (g_dmthJoysticks[i].black > minThreshold) ||
                   (g_dmthJoysticks[i].leftTrigger > minThreshold) ||
                   (g_dmthJoysticks[i].rightTrigger > minThreshold) ||
                   (g_dmthJoysticks[i].leftStick > minThreshold) ||
                   (g_dmthJoysticks[i].rightStick > minThreshold) ||
                   (g_dmthJoysticks[i].dPadUp == TRUE) ||
                   (g_dmthJoysticks[i].dPadDown == TRUE) ||
                   (g_dmthJoysticks[i].dPadLeft == TRUE) ||
                   (g_dmthJoysticks[i].dPadRight == TRUE) ||
                   (g_dmthJoysticks[i].back == TRUE) ||
                   (g_dmthJoysticks[i].start == TRUE))
                {
                   notDone  = TRUE;
                }
            }
        }
    }
}

//------------------------------------------------------------------------------
//  dmthIsAnyButtonDown
//------------------------------------------------------------------------------
BOOL
dmthIsAnyButtonDown(void)
/*++

Routine Description:

    Returns TRUE if any controller button is down

Arguments:

    None

Return Value:

    TRUE if any controller button is down (at least 50%), FALSE otherwise

--*/
{
    UINT i;

    dmthGetJoystickStates();

    for(i=0; i<4; ++i)
    {
        // Only check valid joysticks
        if(g_dmthInputHandles[i])
        {
            if((g_dmthJoysticks[i].x > 0.5f) ||
               (g_dmthJoysticks[i].y > 0.5f) ||
               (g_dmthJoysticks[i].white > 0.5f) ||
               (g_dmthJoysticks[i].a > 0.5f) ||
               (g_dmthJoysticks[i].b > 0.5f) ||
               (g_dmthJoysticks[i].black > 0.5f) ||
               (g_dmthJoysticks[i].leftTrigger > 0.5f) ||
               (g_dmthJoysticks[i].rightTrigger > 0.5f) ||
               (g_dmthJoysticks[i].leftStick > 0.5f) ||
               (g_dmthJoysticks[i].rightStick > 0.5f) ||
               (g_dmthJoysticks[i].dPadUp == TRUE) ||
               (g_dmthJoysticks[i].dPadDown == TRUE) ||
               (g_dmthJoysticks[i].dPadLeft == TRUE) ||
               (g_dmthJoysticks[i].dPadRight == TRUE) ||
               (g_dmthJoysticks[i].back == TRUE) ||
               (g_dmthJoysticks[i].start == TRUE))
            {
               return TRUE;
            }
        }
    }
    return FALSE;
}


BOOL FloatsAreEqual(const float& f1, const float& f2)
{
  return ((fabs(f1 - f2) < 1.0e-6) ? TRUE : FALSE);
}


HRESULT Fail_On_Purpose(CtIDirectMusicPerformance8 *ptPerf8, DWORD dwUnused1, DWORD dwUnused2)
{
    return E_FAIL;
}


//-----------------------------------------------------------------------------
// Name: DownloadScratch
// Desc: Downloads a DSP scratch image to the DSP
//-----------------------------------------------------------------------------
HRESULT DownloadScratch(IDirectSound *pDSound, PCHAR pszScratchFile)
{
    HANDLE hFile = INVALID_HANDLE_VALUE;
    DWORD dwSize = 0;
    HRESULT hr = S_OK;
    LPDSEFFECTIMAGEDESC pDesc;
    DSEFFECTIMAGELOC EffectLoc = {0};
    EffectLoc.dwI3DL2ReverbIndex = I3DL2_CHAIN_I3DL2_REVERB;
    EffectLoc.dwCrosstalkIndex   = I3DL2_CHAIN_XTALK;
    
    if (_strcmpi("Default", g_TestParams.szDSScratchImage) == 0)
    {
        CHECKRUN(XAudioDownloadEffectsImage("dsstdfx", &EffectLoc, XAUDIO_DOWNLOADFX_XBESECTION, &pDesc));
    }
    else
    {
        CHECKRUN(XAudioDownloadEffectsImage(pszScratchFile, &EffectLoc, XAUDIO_DOWNLOADFX_EXTERNFILE, &pDesc));
    }

    return hr;
}

//------------------------------------------------------------------------------
//	InitializeDSound
//------------------------------------------------------------------------------
HRESULT InitializeDSound(IDirectSound **ppDSound)
/*++
Routine Description:
	Downloads the scratch image to the DSP
Return Value:
	S_OK on success, any other value on failure
*/
{
HRESULT hr = S_OK;

CHECKRUN(DirectSoundCreate( NULL, ppDSound, NULL ));
if (_strcmpi("none", g_TestParams.szDSScratchImage))
    CHECKRUN(DownloadScratch(*ppDSound, g_TestParams.szDSScratchImage));

if (_strcmpi("full", g_TestParams.szDSHRTF) == 0)
{CHECK(DirectSoundUseFullHRTF());}
else if (_strcmpi("light", g_TestParams.szDSHRTF) == 0)
{CHECK(DirectSoundUseLightHRTF());}
else ASSERT(FALSE);
return hr;
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
HRESULT ClearAllPMsgs(CtIDirectMusicPerformance8 *ptPerf8)
{
DMUS_NOTIFICATION_PMSG *pMsg = NULL;


    //Cleaning up any last notifications.
    while (S_OK == ptPerf8->GetNotificationPMsg(&pMsg))
    {
        if (pMsg)
        {
            ptPerf8->FreePMsg((DMUS_PMSG *)pMsg);
            pMsg = NULL;
        }
    }

return S_OK;

}




//------------------------------------------------------------------------------
// Caller must delete[] what it receives in ppvAudioData and pwfxFormat.
//------------------------------------------------------------------------------
HRESULT HelpLoadWaveFileData(LPCSTR pszFile, LPCWAVEFORMATEX *ppwfxFormat, LPVOID *ppvAudioData, LPDWORD pdwAudioDataSize)
{
    LPXFILEMEDIAOBJECT      pFile           = NULL;
    HRESULT                 hr              = DS_OK;
    LPCWAVEFORMATEX         pwfxFormat;
    LPWAVEFORMATEX          pwfxFormatOut   = NULL;
    XMEDIAPACKET            xmp;
    DWORD                   dwAudioDataSize = 0;
    LPVOID                  pvAudioData     = NULL;

    //Create the media object.
    hr = XWaveFileCreateMediaObject(pszFile, &pwfxFormat, &pFile);
    if(SUCCEEDED(hr))
    {
        
        //Allocate our pwfxFormat.
        if(!(pwfxFormatOut = (LPWAVEFORMATEX)new BYTE [sizeof(*pwfxFormat) + pwfxFormat->cbSize]))
        {
            hr = DSERR_OUTOFMEMORY;
        }
    }

    if(SUCCEEDED(hr))
    {
        CopyMemory(pwfxFormatOut, pwfxFormat, sizeof(*pwfxFormat) + pwfxFormat->cbSize);
    }

    if(SUCCEEDED(hr))
    {
        hr = pFile->GetLength(&dwAudioDataSize);
    }

    //Allocate our data.
    if(SUCCEEDED(hr))
    {
        if(!(pvAudioData = new BYTE [dwAudioDataSize]))
        {
            hr = DSERR_OUTOFMEMORY;
        }
    }

    if(SUCCEEDED(hr))
    {
        ZeroMemory(&xmp, sizeof(xmp));

        xmp.pvBuffer = pvAudioData;
        xmp.dwMaxSize = dwAudioDataSize;
        
        hr = pFile->Process(NULL, &xmp);
    }

    if(pFile)
    {
        pFile->Release();
    }

    if(SUCCEEDED(hr))
    {
        ASSERT(pwfxFormatOut);
        ASSERT(pvAudioData);
        ASSERT(dwAudioDataSize);
        

        *ppwfxFormat = pwfxFormatOut;
        *ppvAudioData = pvAudioData;
        *pdwAudioDataSize = dwAudioDataSize;
    }

    return hr;
}

//==================================================================================
// StringHexToDWord
//----------------------------------------------------------------------------------
//
// Description: Converts the passed in string representation of a hexadecimal DWORD
//				to the actual DWORD value.  It ignores any non-English number
//				characters.
//
// Arguments:
//	char* szString		Textual representation of value.
//
// Returns: The value of the string converted into a DWORD.
//==================================================================================
DWORD StringHexToDWord(char* szString)
{
	DWORD	dwValue = 0;
	DWORD	dwTemp;


	// If the string starts with that "0x" prefix, we should move past that.
	//if (StringStartsWith(szString, "0x", false))
    if (szString[0] == '0' && szString[0] == 'x')
		szString += 2;


	if ((strlen(szString) < 1) || (strlen(szString) > 8))
		return (0);


	dwValue = 0;
	for(dwTemp = 0; dwTemp < strlen(szString); dwTemp++)
	{
		dwValue *= 16; // shift the hexadecimal place
		switch (szString[dwTemp])
		{
			case '0':
			  break; // don't add anything
			case '1':
				dwValue += 1;
			  break;
			case '2':
				dwValue += 2;
			  break;
			case '3':
				dwValue += 3;
			  break;
			case '4':
				dwValue += 4;
			  break;
			case '5':
				dwValue += 5;
			  break;
			case '6':
				dwValue += 6;
			  break;
			case '7':
				dwValue += 7;
			  break;
			case '8':
				dwValue += 8;
			  break;
			case '9':
				dwValue += 9;
			  break;
			case 'a':
			case 'A':
				dwValue += 10;
			  break;
			case 'b':
			case 'B':
				dwValue += 11;
			  break;
			case 'c':
			case 'C':
				dwValue += 12;
			  break;
			case 'd':
			case 'D':
				dwValue += 13;
			  break;
			case 'e':
			case 'E':
				dwValue += 14;
			  break;
			case 'f':
			case 'F':
				dwValue += 15;
			  break;
			default:
				dwValue = dwValue / 16; // shift the hexadecimal place back
			  break; // we got a wacky character
		} // end switch (on the letter)
	} // end for (loop through each letter in the string)

	return (dwValue);
} // StringHexToDWord


// StringContainsChar
//----------------------------------------------------------------------------------
//
// Description: Returns -1 if the passed string doesn't contain the char, otherwise
//				it returns the zero based position the first instance of the char
//				was found.  This checking starts at the value passed for iPos, so
//				pass zero if you want it to start at the beginning of the string. 
//
// Arguments:
//	char* szString		String to search.
//	char cCharToFind	Character to search for.
//	BOOL fMatchCase		Whether the character's case is important or not.
//	int iPos			Offset in string to begin looking
//
// Returns: The index of the char, or -1 if not found.
//==================================================================================
int StringContainsChar(char* szString, char cCharToFind, BOOL fMatchCase, int iPos)
{
	if (szString == NULL)
		return (-1);

	// (int) is to get rid of compiler warning
	if (iPos >= (int) strlen(szString))
		return (-1);

	if (iPos < 0)
		iPos = 0;

	if (cCharToFind == '\0')
		return (-1);

	if (fMatchCase)
	{
		while((szString[iPos] != '\0') && (szString[iPos] != cCharToFind))
			iPos++;
	} // end if (we have to match the case)
	else
	{
		char*	pszLowerString;
		char	szTemp[2];


		pszLowerString = (char*) LocalAlloc(LPTR, strlen(szString) + 1);
		if (pszLowerString == NULL)
			return (-1);

		strcpy(pszLowerString, szString);
		_strlwr(pszLowerString);


		szTemp[0] = cCharToFind;
		szTemp[1] = '\0';
		_strlwr(szTemp);

		while((pszLowerString[iPos] != '\0') &&
				(pszLowerString[iPos] != szTemp[0]))
		{
			iPos++;
		} // end while (not at end of string and not case-insensitive character)

		LocalFree(pszLowerString);
		pszLowerString = NULL;
	} // end else (we don't have to match the case)

	if (szString[iPos] == '\0')	// if we didn't find it
		return (-1);	// failure
	
	return (iPos);
} // StringContainsChar





//==================================================================================
// StringCountNumChars
//----------------------------------------------------------------------------------
//
// Description: Returns the number of times the string contains the passed in
//				character, or 0 if none. 
//
// Arguments:
//	char* szString		String to search.
//	char cCharToFind	Character to search for.
//	BOOL fMatchCase		Whether the character's case is important or not.
//
// Returns: The index of the char, or -1 if not found.
//==================================================================================
int StringCountNumChars(char* szString, char cCharToFind, BOOL fMatchCase)
{
	int				iCount = 0;
	unsigned int	ui;


	if (szString == NULL)
		return (-1);

	if (fMatchCase)
	{
		for(ui = 0; ui < strlen(szString); ui++)
		{
			if (szString[ui] == cCharToFind)
				iCount++;
		} // end for (each character in the string)
	} // end if (we have to match the case)
	else
	{
		char*	pszLowerString;
		char	szTemp[2];


		pszLowerString = (char*) LocalAlloc(LPTR, strlen(szString) + 1);
		if (pszLowerString == NULL)
			return (-1);

		strcpy(pszLowerString, szString);
		_strlwr(pszLowerString);


		szTemp[0] = cCharToFind;
		szTemp[1] = '\0';
		_strlwr(szTemp);


		for(ui = 0; ui < strlen(szString); ui++)
		{
			if (pszLowerString[ui] == szTemp[0])
				iCount++;
		} // end for (each character in the string)
	} // end else (we don't have to match the case)
	
	return (iCount);
} // StringCountNumChars



// StringToGuid
//----------------------------------------------------------------------------------
//
// Description: Converts the passed in string representation of a GUID to the actual
//				GUID. 
//
// Arguments:
//	char* szString			Textual representation of guid.
//	LPGUID pguidResult		Pointer to guid to store result in.
//
// Returns: S_OK if successful, error code otherwise.
//==================================================================================
HRESULT StringToGuid(char* szString, LPGUID pguidResult)
{
	int		iOldPos = 0;
	int		iNewPos = 0;
	char	szSegment[9];
	DWORD	dwValue = 0;
	int		i;



	// Skip the beginning
	while ((szString[iOldPos] == ' ') ||
			(szString[iOldPos] == '\t') ||
			(szString[iOldPos] == '{'))
	{
		iOldPos++;
	} // end while (it starts with an ignore character)


	// Data1 = 1 DWORD

	iNewPos = StringContainsChar(szString, '-', false, iOldPos);

	// Make sure we've got 8 valid characters of data (assume high order zeroes if
	// it's fewer than 8 characters).
	if ((iNewPos < 0) || (iNewPos > (iOldPos + 9)))
		return (ERROR_INVALID_PARAMETER);


	ZeroMemory(szSegment, 9);
	CopyMemory(szSegment, szString + iOldPos, (iNewPos - iOldPos));

	pguidResult->Data1 = StringHexToDWord(szSegment);

	iOldPos = iNewPos + 1;



	// Data2 = 1 WORD

	iNewPos = StringContainsChar(szString, '-', false, iOldPos);

	// 4 characters.
	if ((iNewPos < 0) || (iNewPos > (iOldPos + 5)))
		return (ERROR_INVALID_PARAMETER);


	ZeroMemory(szSegment, 9);
	CopyMemory(szSegment, szString + iOldPos, (iNewPos - iOldPos));

	pguidResult->Data2 = (WORD) StringHexToDWord(szSegment);

	iOldPos = iNewPos + 1;



	// Data3 = 1 WORD

	iNewPos = StringContainsChar(szString, '-', false, iOldPos);

	// 4 characters.
	if ((iNewPos < 0) || (iNewPos > (iOldPos + 5)))
		return (ERROR_INVALID_PARAMETER);


	ZeroMemory(szSegment, 9);
	CopyMemory(szSegment, szString + iOldPos, (iNewPos - iOldPos));

	pguidResult->Data3 = (WORD) StringHexToDWord(szSegment);


	iNewPos = StringContainsChar(szString, '-', false, iOldPos);

	iOldPos = strlen(szString); // temp variable

	// Make sure there are enough single bytes at the end here
	if (iNewPos + 16 > iOldPos)
		return (ERROR_INVALID_PARAMETER);


	// Move past dash
	iNewPos++;

	// Data4 = 8 single bytes

	for(i = 0; i < 8; i++)
	{
		if (i == 2)
		{
			if (szString[iNewPos] != '-')
				return (ERROR_INVALID_PARAMETER);

			// If it is the dash, then we want to skip it
			iNewPos++;
		} // end if (we should be at a dash)

		ZeroMemory(szSegment, 9);
		CopyMemory(szSegment, szString + iNewPos, 2);

		pguidResult->Data4[i] = (BYTE) StringHexToDWord(szSegment);

		iNewPos += 2;
	} // end for (each of the 8 individual bytes)


	return (S_OK);
} // StringToGuid



//Memory checking stuff
/********************************************************************************
********************************************************************************/
void GetMemoryStats(LONG *pLongArray, DWORD dwMax)
{
    for (DWORD i=0; i<dwMax; i++)
    {
        pLongArray[i] = DirectMusicMemCheck(i, NULL);
    }
};

/********************************************************************************
********************************************************************************/
void CompareMemoryStats(LONG *pLast, LONG *pCurrent, DWORD dwMax, LPSTR szTestName)
{  
  LPSTR szObjectName = NULL;
  for (DWORD i=0; i<dwMax; i++)
  {
    //Don't check unsupported ones.
    if (pLast[i] != -1 && pCurrent[i] != -1)
    {
        if (pLast[i] < pCurrent[i])
            Log(ABORTLOGLEVEL, "POSSIBLE MEMORY LEAK in %-54s", szTestName);
        if (pLast[i] > pCurrent[i])
            Log(ABORTLOGLEVEL, "REDEMPTION in %-66s", szTestName);

        if (pLast[i] != pCurrent[i])
        {
            DirectMusicMemCheck(i, &szObjectName);
            Log(ABORTLOGLEVEL, "%s count went from %u to %u", szObjectName, pLast[i], pCurrent[i]);
        }
    }
  }
};




/********************************************************************************
Either waits or continues on, at end of test, depending on settings.
********************************************************************************/
void EndTestRun(void)
{
int  dwValue[4] = {0};
CHAR dwDelta[4] = {1, 2, 1, -1};

    if (g_TestParams.bWaitAtTestEnd)    
    {
        Log(FYILOGLEVEL, "Please have a super-happy day!");
        
        //we want to do an infinite loop here.
        //lint -e716 
        while (1)
        //lint +e716
        {

            for (int i=0; i<4; i++)
            {
                dwValue[i] += dwDelta[i];
                if (dwValue[i] > 255)
                {
                    dwValue[i] = 255;
                    dwDelta[i] = -(rand() % 3 + 1);
                }

                if (dwValue[i] < 64 )
                {
                    dwValue[i] = 64;
                    dwDelta[i] = (rand() % 3 + 1);
                }

            }



            xSetBackgroundColor(
                             BYTE(dwValue[0]), 
                             BYTE(dwValue[1]), 
                             BYTE(dwValue[2]), 
                             BYTE(dwValue[3])
                             );
            Sleep(20);
        }

    }
    else
    {
        Log(FYILOGLEVEL, "Displayin' test results for 1 second");
        Sleep(1000);
    }
}



/********************************************************************************
********************************************************************************/
BOOL IsValidAudioPath(DWORD dwAudioPath)
{
BOOL bFound = FALSE;
DWORD dwValidPaths[] = {
  DMUS_APATH_SHARED_STEREOPLUSREVERB,
  DMUS_APATH_SHARED_STEREO,
  DMUS_APATH_DYNAMIC_3D,
  DMUS_APATH_DYNAMIC_MONO,
  DMUS_APATH_MIXBIN_QUAD               ,
  DMUS_APATH_MIXBIN_QUAD_ENV,
  DMUS_APATH_MIXBIN_QUAD_ENV,
  DMUS_APATH_MIXBIN_QUAD_MUSIC,
  DMUS_APATH_MIXBIN_5DOT1,
  DMUS_APATH_MIXBIN_5DOT1_ENV,
  DMUS_APATH_MIXBIN_5DOT1_MUSIC,
  DMUS_APATH_MIXBIN_STEREO_EFFECTS
};

for (int i=0; i<NUMELEMS(dwValidPaths); i++)
{
    if (dwAudioPath == dwValidPaths[i])
    {
        bFound = TRUE;
        break;
    }
}

return bFound;
}


BOOL InitPerformanceEveryTest(void)
{
    return (_strcmpi(g_TestParams.szInitPerformance, "EveryTest") == 0);
};


BOOL InitPerformanceOnce(void)
{
    return (_strcmpi(g_TestParams.szInitPerformance, "Once") == 0);
};


BOOL InitPerformanceNever(void)
{
    return (_strcmpi(g_TestParams.szInitPerformance, "Never") == 0);

};









/********************************************************************************
Note: These conversion functions differ from DMusic's in that DMusic converts
      between the RT passed since performance initialization and the MT passed
      since performance initialization.  Since DMusic's MT "clock" ticks faster or
      slower depending on the tempo, the DMusic converstion functions don't provide
      an accurate conversion for time deltas.
********************************************************************************/
//BASIC FORMULAS FOR CONVERSION BETWEEN REFERENCE AND MUSIC TIME.

//mt = rt ticks   seconds   beats   minutes    mt ticks
//              * ------- * ----- * -------- * -------
//                rt tick   minute  second     beat


//
//mt = 1         fTempo beats  1    mt ticks
//     ------- * -----       * -- * --------
//     10000000  minute        60   beat

//mt = rtTicks * fTempo * DMUS_PPQ * 3
//     ---------------------------
//               600000000

//rt = mtTicks * 600000000 
//     ------------------
//     fTempo  * DMUS_PPQ * 3




//===========================================================================
//===========================================================================
HRESULT ReferenceToMusicTime(CtIDirectMusicPerformance8 *ptPerf8, REFERENCE_TIME rtTime, MUSIC_TIME* pmtTime)
{
    HRESULT hr = S_OK;
    double fTempo = 0;
    MUSIC_TIME mtTime = 0;

    CHECKPTR(ptPerf8);
    CHECKPTR(pmtTime);

    CHECKRUN(GetTempo(ptPerf8, &fTempo));
        
    //Safest to convert things to a double.
    mtTime = MUSIC_TIME(double(rtTime) * fTempo * double(DMUS_PPQ)  / double(600000000));

    *pmtTime = mtTime;

    return S_OK;
}



//===========================================================================
//===========================================================================
HRESULT MusicToReferenceTime(CtIDirectMusicPerformance8 *ptPerf8, MUSIC_TIME mtTime, REFERENCE_TIME *prtTime)
{
    HRESULT hr = S_OK;
    double fTempo = 0;
    REFERENCE_TIME rtTime = 0;

    CHECKPTR(ptPerf8);
    CHECKPTR(prtTime);

    CHECKRUN(GetTempo(ptPerf8, &fTempo));
        
    //Safest to convert things to a double.
    rtTime = REFERENCE_TIME(double(mtTime) * double(600000000) / fTempo / double(DMUS_PPQ));

    *prtTime = rtTime;

    return S_OK;
}

//===========================================================================
//===========================================================================
double CalculateErrorPercentage(double tExpected, double tActual)
{
    double fError = 0.f;

    if (tExpected != 0.f)
        fError = fabs(tExpected- tActual) / tExpected * 100.f;
    else
        fError = fabs(tExpected- tActual) / 1.f * 100.f;

    return fError;
        
}


//===========================================================================
//===========================================================================
LPCSTR MediaServerToLocal(LPCSTR szFrom)
{
ASSERT(szFrom);
CHAR szTemp[MAX_PATH] = {0};
LPSTR szChopped = NULL;
static CHAR szOut[MAX_PATH] = {0};
LPCSTR szStrings[] = {"dmusic/", "sgt/", "wav/", "dls/"};
DWORD i = 0;

//TODO: Add a critical section here.


//Begin the conversion of the from path to the 'to' path.
strcpy(szTemp, szFrom);
_strlwr(szTemp);

//Verify that the string starts with "DMusic\\"
BOOL bFound = FALSE;
for (i=0; i<NUMELEMS(szStrings); i++)
{
    if (strstr(szTemp, szStrings[i]))
    {
        bFound = TRUE;
    }
}
ASSERT(bFound);


ASSERT(szTemp[0] != '/');

//Chop off the directory name if it's "dmusic/"
if (strstr(szTemp, szStrings[0]))
{
    szChopped = strstr(szTemp, szStrings[0]);
    szChopped += strlen(szStrings[0]);
}
else
{
    szChopped = szTemp;
}

//Change all the '/' to '\\'
for (DWORD i=0; (szTemp[i] != NULL) && (i != MAX_PATH); i++)
{
    if (szTemp[i] == '/')
        szTemp[i] = '\\';
}

sprintf(szOut, "T:\\%s", szChopped);

return szOut;
}

//===========================================================================
//===========================================================================
HRESULT MediaServerToLocal(LPCSTR szServer, LPSTR szLocal)
{
    ASSERT(szServer);
    ASSERT(szLocal);
    strcpy(szLocal, MediaServerToLocal(szServer));
    return S_OK;
};


//===========================================================================
//===========================================================================
HRESULT MediaCopyFile(LPCSTR szFromPath, LPCSTR szToPath, DWORD dwFlags)
{
HRESULT hr = S_OK;
MEDIAFILEDESC MediaFileDesc[] = {NULL, NULL, NULL, TERMINATE};
LPCSTR szDestination = NULL;
CHAR szDestinationDir[MAX_PATH] = {0};

if (SUCCEEDED(hr))
{
    if (!szToPath)
        szDestination = MediaServerToLocal(szFromPath);
    else
        szDestination = szToPath;
}

CHECKRUN(ChopPath(szDestination, szDestinationDir, NULL));

if (SUCCEEDED(hr))
{
    MediaFileDesc[0].lpszFromPath= (LPSTR)szFromPath;
    MediaFileDesc[0].lpszToPath  = (LPSTR)szDestinationDir;
    MediaFileDesc[0].dwFlags = dwFlags;

    hr = LoadMedia(&MediaFileDesc[0]);
    if (FAILED(hr))
    {
        Log(ABORTLOGLEVEL, "MediaCopyFile: Failed to copy %s to %s (%08X = %s)", szFromPath, szDestination, hr, tdmXlatHRESULT(hr));
    }
}

return hr;
}

//===========================================================================
//===========================================================================
HRESULT MediaCopyFile(LPCSTR szFromPath, LPCSTR szToPath)
{
    return MediaCopyFile(szFromPath, szToPath, COPY_IF_NEWER);
};

//===========================================================================
//===========================================================================
HRESULT MediaCopyFile(LPCSTR szFromPath, DWORD dwFlags)
{
    return MediaCopyFile(szFromPath, NULL, dwFlags);
};

//===========================================================================
//===========================================================================
HRESULT MediaCopyFile(LPCSTR szFromPath)
{
    return MediaCopyFile(szFromPath, NULL, COPY_IF_NEWER);
};


//===========================================================================
//This can take a file name as well as a file directory.
//===========================================================================
HRESULT MediaCopyDirectory(LPCSTR szFromPath, LPCSTR szToPath, DWORD dwFlags)
{
MEDIAFILEDESC MediaFileDesc[] = {NULL, NULL, NULL, TERMINATE};

HRESULT hr = S_OK;
CHAR szTemp[MAX_PATH] = {0};
CHAR szSourcePath[MAX_PATH] = {0};
LPCSTR szDestination = NULL;

ASSERT(szFromPath);

if (SUCCEEDED(hr))
{
    
    //If there's no '.' and the name doesn't end in a slash, then we have a problem.
    if ('/' != szFromPath[strlen(szFromPath) - 1] && !strstr(szFromPath, "."))
    {
        Log(ABORTLOGLEVEL, "MediaCopyDirectory: Don't know if %s is a file name or directory name", szFromPath);
        hr = E_FAIL;
    }
}

//Pull any filename off the source path.
CHECKRUN(ChopPath(szFromPath, szTemp, NULL));

if (SUCCEEDED(hr))
{
    //Make a destination path from the source path, if necessary.
    if (!szToPath)
        szDestination = MediaServerToLocal(szTemp);
    else
        szDestination = szToPath;


    //Tack on a "*" to the source path.
    sprintf(szSourcePath, "%s(\\w*).(\\w*)", szTemp);

    //Set up for the copy.
    MediaFileDesc[0].lpszFromPath = (LPSTR)szSourcePath;
    MediaFileDesc[0].lpszToPath   = (LPSTR)szDestination;
    MediaFileDesc[0].dwFlags = dwFlags;

    //Makin' copies!  The copymeister...
    hr = LoadMedia(&MediaFileDesc[0]);
    if (FAILED(hr))
    {
        Log(ABORTLOGLEVEL, "MediaCopyDirectory: Failed to copy %s to %s (%08X = %s)", szSourcePath, szToPath, hr, tdmXlatHRESULT(hr));
    }
}

return hr;
}

//===========================================================================
//This can take a file name as well as a file directory.
//===========================================================================
HRESULT MediaCopyDirectory(LPCSTR szFromPath, LPCSTR szToPath)
{
    return MediaCopyDirectory(szFromPath, szToPath, COPY_IF_NEWER);
}

//===========================================================================
//This can take a file name as well as a file directory.
//===========================================================================
HRESULT MediaCopyDirectory(LPCSTR szFromPath, DWORD dwFlags)
{
    return MediaCopyDirectory(szFromPath, NULL, dwFlags);
}


//===========================================================================
//This can take a file name as well as a file directory.
//===========================================================================
HRESULT MediaCopyDirectory(LPCSTR szFromPath)
{
    return MediaCopyDirectory(szFromPath, NULL, COPY_IF_NEWER);
}


//===========================================================================
//===========================================================================
BOOL FilesInSameDirectory(LPSTR szFiles[], DWORD dwCount)
{
BOOL bDifferentDirectory = FALSE;
CHAR szPath0[MAX_PATH];
CHAR szPathCurrent[MAX_PATH];
    ChopPath(szFiles[0],szPath0, NULL);
    for (DWORD i=1; i<AMOUNT(szFiles); i++)
    {
        ChopPath(szFiles[i],szPathCurrent, NULL);
        if (_strcmpi(szPathCurrent, szPath0) != 0)
        {
            bDifferentDirectory = TRUE;
            Log(ABORTLOGLEVEL, "%s is in a different directory from %s", szPathCurrent, szPath0);
            break;
        }
    };

    return !bDifferentDirectory;
}


//===========================================================================
//===========================================================================
HRESULT ThreadPriFromString(LPCSTR szString, DWORD *pdwPri)
//Verify that an appropriate thread priority was selected.
{
    if (pdwPri)
        *pdwPri = 0xFFFFFFFF;

    BOOL bFound = FALSE;
    for (DWORD i=0; i<g_dwThreadPris; i++)
    {
        if (_strcmpi(szString, g_ThreadPris[i].sz) == 0)
        {
            bFound = TRUE;
            if (pdwPri)
                *pdwPri = g_ThreadPris[i].dw;
            break;
        }
    }

    if (!bFound)
        return E_FAIL;
    else
        return S_OK;
}


