//+-------------------------------------------------------------------------
//
//  Microsoft Windows
//
//  Copyright (C) Microsoft Corporation, 1998 - 1999
//
//  File:       CICMusic.cpp
//
//--------------------------------------------------------------------------

/////////////////////////////////////////////////////////////////////////////
//
// CICMusic.cpp
// Provides initialization and cleanup behavior for basic DirectMusic functionality
// Specifically, an IDirectMusic, IDirectMusicLoader, and IDirectMusicPerformance.
//
//
// Functions:
//
//
//  History:
//      03/09/2000 - danhaff/ruswoods - created
//      03/27/2000 - danhaff - Made Init call UnInit first, fixed bugs
//      08/29/2000 - danhaff - Removed instances of DYNAMIC_APATH_ENV3D
//
/////////////////////////////////////////////////////////////////////////////

#include "globals.h"
#include "cicmusicx.h"


//--------------------------------------------------------------------------;
// Description: <nl>
// Constructor for CICMusic
//
//
// Parameters: <nl>
//   hWnd: hWnd of our window.
//
// Notes:
//
// History: <nl>
//      03/09/2000 - danhaff/ruswoods - created
//
//--------------------------------------------------------------------------;
CICMusic::CICMusic(void)
:
m_ptLoader8 (NULL),
m_ptPerformance8 (NULL),
m_ptUnkAudioPathConfig (NULL),
m_ptAudioPath (NULL),
m_ptAudioPathEnv (NULL),
m_ptSegment8(NULL),
m_ptSegmentState8(NULL)
{
    //What are you looking at?
};


//--------------------------------------------------------------------------;
// Description: <nl>
//
// History: <nl>
//      03/09/2000 - danhaff/ruswoods - created
//
//--------------------------------------------------------------------------;
HRESULT CICMusic::Init
(
CtIDirectMusicPerformance8 *ptPerf8,
LPCSTR szSegment,
DWORD dwDefaultPathType,
DWORD dwPChannelCount)
{
    HRESULT hr = E_FAIL;

    __try
    {
        //Copy our performance in.
        if (!ptPerf8)
            __leave;
        m_ptPerformance8 = ptPerf8;
            
        //First uninitialize anything we've initialized, so we don't leave dangling pointers.
        hr = PrivateUninitialize();
        if (S_OK != hr)
            __leave;
        
        //Now perform our initialization.
        hr = PrivateInitialize(szSegment, NULL, dwDefaultPathType, /*dwUseAPathEnv,*/ dwPChannelCount);
        if (S_OK != hr)
            __leave;
    }
    __finally
    {
    }

    return hr;
}



//--------------------------------------------------------------------------;
// Description: <nl>
//
//
// History: <nl>
//      03/09/2000 - danhaff/ruswoods - created
//
//--------------------------------------------------------------------------;
HRESULT CICMusic::Init(
CtIDirectMusicPerformance8 *ptPerf8,
LPCSTR szSegment,
LPCSTR szConfig)
{
    HRESULT hr;
    __try
    {
        //Copy our performance in.
        if (!ptPerf8)
            __leave;
        m_ptPerformance8 = ptPerf8;

        //First uninitialize anything we've initialized, so we don't leave dangling pointers.
        hr = PrivateUninitialize();
        if (S_OK != hr)
            __leave;

        //Now perform our initialization.
        hr = PrivateInitialize(szSegment,              //Specified segment.
                               szConfig,               //Specified AP config file.
                               NULL,                    //Don't use a standard path type.
                               0);                      //pChannels don't matter cuz we're using file.
        if (S_OK != hr)
            __leave;

    }
    __finally
    {
    }
    return hr;
}

//--------------------------------------------------------------------------;
// Description: <nl>
//   Shutdowns the class, just like the destructor.  I added this
//   because sometimes you want to shut it down early.
//
// Notes:
//
// History: <nl>
//      03/27/2000 - danhaff - created
//
//--------------------------------------------------------------------------;
HRESULT CICMusic::Shutdown(void)
{
    HRESULT hr;
    hr = PrivateUninitialize();
    return hr;
}


//--------------------------------------------------------------------------;
// Description: <nl>
//  Destructor for CICMusic
//
// History: <nl>
//      03/09/2000 - danhaff/ruswoods - created
//--------------------------------------------------------------------------;
CICMusic::~CICMusic ()
{
    PrivateUninitialize();
}



//--------------------------------------------------------------------------;
// Description: <nl>
// Initialization function for CICMusic, called by both constructors.
//
//
// Parameters: <nl>
//
// Notes:
//
// History: <nl>
//      03/09/2000 - danhaff/ruswoods - created
//
//--------------------------------------------------------------------------;
HRESULT CICMusic::PrivateInitialize(
LPCSTR szSegment,
LPCSTR szConfig,
DWORD dwDefaultPathType,
DWORD dwPChannelCount
)
{
HRESULT hr=S_OK;
IDirectMusicPerformance8 *pPerformance8 = NULL;
char szConfigPath [MAX_PATH]          = {0};
char szConfigFile [MAX_PATH]         = {0};
char szSegmentPath[MAX_PATH]         = {0};
char szSegmentFile[MAX_PATH]        = {0};


    __try
    {
        //Verify that one of the correct types of standardpaths was passed, if any.
        if (dwDefaultPathType != 0 &&
            !IsValidAudioPath(dwDefaultPathType)
            )
        {
            Log(ABORTLOGLEVEL, "**** TEST APP ERROR:  Someone passed an invalid value for dwDefaultPathType!!");
            hr = E_FAIL;
            __leave;
        }

        if (!m_ptPerformance8)
        {
            Log(ABORTLOGLEVEL, "**** TEST APP ERROR:  Calling CICMusic::PrivateInitialize without a valid m_ptPerformance8!!");
            hr = E_FAIL;
            __leave;
        }

        //Create a new Loader8.
        hr = dmthCreateLoader(IID_IDirectMusicLoader8, &m_ptLoader8);
        if (S_OK != hr)
        {
            Log(ABORTLOGLEVEL, "**** ERROR: CICMusic: dmthCreateLoader returned (%s == %08Xh)",
                    tdmXlatHRESULT(hr), hr);
            __leave;
        }

        //Parse out the path and the file.
        hr = ChopPath(szSegment, szSegmentPath, szSegmentFile);
        if (hr != S_OK)
        {
            Log(ABORTLOGLEVEL, "**** ERROR parsing full path to Segment file: %s", szSegment);
            __leave;
        }
        
        //Keep all info, we don't want GM.DLS to be unfound.
        hr = m_ptLoader8->SetSearchDirectory(GUID_DirectMusicAllTypes, (LPSTR)szSegmentPath, FALSE);
        if (FAILED(hr))
        {
            Log(ABORTLOGLEVEL, "**** ERROR: CICMusic: IDirectMusicLoader8::SetSearchDirectory(%s) failed!!", szSegmentPath);
            __leave;
        }
        if (hr != S_OK)
        {
            Log(ABORTLOGLEVEL, "**** WARNING: CICMusic: SetSearchDirectory(%s) returned %s (%08X)", szSegmentPath, tdmXlatHRESULT(hr), hr);
        }

        //If we pass in an audiopath config file, make that the default path, otherwise choose
        //  and standard default path.  This decision was made by which constructor to use.
        if (szConfig)
        {

            //Parse out the Path and file.
            hr = ChopPath(szConfig, szConfigPath, szConfigFile);
            if (hr != S_OK)
            {
                Log(ABORTLOGLEVEL, "**** ERROR parsing full path to config file: %s", szConfig);
                __leave;
            }

            //Stack this on top the other info.
            hr = m_ptLoader8->SetSearchDirectory(GUID_DirectMusicAllTypes, szConfigPath, FALSE);
            if (FAILED(hr))
            {
                Log(ABORTLOGLEVEL, "**** ERROR: CICMusic: IDirectMusicLoader8::SetSearchDirectory(%s) failed!!", szConfigPath);
                __leave;
            }
            if (hr != S_OK)
            {
                Log(ABORTLOGLEVEL, "**** WARNING: CICMusic: SetSearchDirectory(%s) returned %s (%08X)", szConfigPath, tdmXlatHRESULT(hr), hr);
            }


            //Load up the audiopath config.
            //There is no actual interface for this; we use the IUnknown interface
            //  for this object.
            hr = m_ptLoader8->LoadObjectFromFile(CLSID_DirectMusicAudioPathConfig,
                                                 CTIID_IUnknown,
                                                 (LPSTR)szConfigFile,
                                                 (void **)&m_ptUnkAudioPathConfig);
            if (S_OK != hr)
            {
                Log(ABORTLOGLEVEL,
                TEXT("**** ABORT:  CICMusic: IDirectMusicLoader8->LoadObjectFromFile failed to load audiopath config %s!! (%s == %08Xh)"),
                PathFindFileName((LPSTR)szConfigFile),
                tdmXlatHRESULT(hr), hr);
                __leave;
            }

            //Make that audiopath config into a real audiopath
            hr = m_ptPerformance8->CreateAudioPath(m_ptUnkAudioPathConfig, TRUE, &m_ptAudioPath);
            if (S_OK != hr)
            {
                Log(ABORTLOGLEVEL,
                TEXT("**** ABORT:  CICMusic: Perf8->CreateAudioPath failed from loaded config file %s!! (%s == %08Xh)"),
                szConfig, tdmXlatHRESULT(hr), hr);
                __leave;
            }

        }
        else
        {
            //Call CreateStandardAudioPath.
            hr = m_ptPerformance8->CreateStandardAudioPath(dwDefaultPathType, dwPChannelCount, TRUE, &m_ptAudioPath);
            if (S_OK != hr)
            {
                Log(ABORTLOGLEVEL,
                TEXT("**** ABORT:  CICMusic: Perf8->CreateStandardAudioPath returned (%s == %08Xh)"),
                tdmXlatHRESULT(hr), hr);
                __leave;
            }            
        }

/*
        //Set the audiopath we created as the default.
        hr = m_ptPerformance8->SetDefaultAudioPath(m_ptAudioPath);
        if (S_OK != hr)
        {
            Log(ABORTLOGLEVEL,
            TEXT("**** ABORT:  CICMusic: Perf8->SetDefaultAudioPath failed from file %s!! (%s == %08Xh)"),
            szConfig, tdmXlatHRESULT(hr), hr);
            __leave;
        }

*/

        // Load the segment.
        hr = m_ptLoader8->LoadObjectFromFile(CLSID_DirectMusicSegment,
                                        CTIID_IDirectMusicSegment8,
                                        (LPSTR)szSegmentFile,
                                        (void **)&m_ptSegment8);
        if (S_OK != hr)
        {
            Log(ABORTLOGLEVEL,
            TEXT("**** ABORT:  CICMusic: IDirectMusicLoader8->LoadObjectFromFile failed to load segment %s!! (%s == %08Xh)"),
            PathFindFileName((LPSTR)szSegmentFile),
            tdmXlatHRESULT(hr), hr);
                __leave;
        }

        //Tell the segment to repeat forever.
        hr = m_ptSegment8->SetRepeats(DMUS_SEG_REPEAT_INFINITE);
        if (S_OK != hr)
        {
            Log(ABORTLOGLEVEL,
            TEXT("**** ABORT:  CICMusic: IDirectMusicSegment8->SetRepeats(DMUS_SEG_REPEAT_INFINITE) returned (%s == %08Xh)"),
            tdmXlatHRESULT(hr), hr);
                __leave;
        }

        //Get the real Performance8 ptr from the performance, so we can pass it to SetParam.
        hr = m_ptPerformance8->GetRealObjPtr(&pPerformance8);
        if (S_OK != hr)
        {
            Log(ABORTLOGLEVEL,
            TEXT("**** ABORT:  CICMusic: CtIDirectMusicPerformance8->GetRealObjPtr returned (%s == %08Xh)"),
            tdmXlatHRESULT(hr), hr);
                __leave;
        }


        //Play the segment on the default audio path, and get the segment state.
        hr = m_ptPerformance8->PlaySegmentEx((CtIUnknown *)m_ptSegment8,
                                            NULL,                   //no segment id, this isn't a song
                                            NULL,                   //no transition to compose
                                            NULL,                   //no DMUS_SEGF flags
                                            (__int64)0,             //time to start playback (right away)
                                            &m_ptSegmentState8,     //returned segment state
                                            NULL,                   //no segstate to replace
                                            (CtIUnknown *)m_ptAudioPath);                  //play on default audiopath.
        if (S_OK != hr)
        {
            Log(ABORTLOGLEVEL,
            TEXT("**** ABORT:  CICMusic: IDirectMusicPerformance8->PlaySegmentEx returned (%s == %08Xh)"),
            tdmXlatHRESULT(hr), hr);
                __leave;
        }

        hr = ExpectSegmentStart(m_ptPerformance8, m_ptSegment8, NULL, 5000, NULL, FALSE);
        if (S_OK != hr)
        {
            Log(ABORTLOGLEVEL,
            TEXT("**** ABORT:  CICMusic: ExpectSegmentStart on %s returned (%s == %08Xh)"),
            PathFindFileName((LPSTR)szSegmentFile),
            tdmXlatHRESULT(hr), hr);
            hr = E_FAIL;
                __leave;
        }

    }
    __finally
    {
        //Release the only thing we allocated locally during this function.
        SAFE_RELEASE(pPerformance8);

        //If init failed, release all members.
        if (S_OK != hr)
            PrivateUninitialize();
    }

return hr;
};






//--------------------------------------------------------------------------;
// Description: <nl>
//  Frees up everything in the object.
//
// History: <nl>
//      03/09/2000 - danhaff/ruswoods - created
//--------------------------------------------------------------------------;
HRESULT CICMusic::PrivateUninitialize(void)
{
    //Might as well have the return value somewhere.
    HRESULT hr = S_OK;

    //Stop Everything.
    if (m_ptPerformance8)
    {
        hr = m_ptPerformance8->StopEx(NULL, __int64(0), NULL);  //Uncomment this to repro bug 2967
    }

    //Release all the interfaces we've stashed, except the performance which must be Closed down and
    //  released last.
    SAFE_RELEASE (m_ptSegmentState8);
    SAFE_RELEASE (m_ptSegment8);
    SAFE_RELEASE (m_ptAudioPath);
    SAFE_RELEASE (m_ptAudioPathEnv);
    SAFE_RELEASE (m_ptUnkAudioPathConfig);
    SAFE_RELEASE (m_ptLoader8);


    return hr;
};



//--------------------------------------------------------------------------;
// Description: <nl>
//      GetLoader8
//
// Parameters: <nl>
//      pptLoader8: contains address of pointer to contain wrapped IDirectMusicLoader8.
//
// History: <nl>
//      03/09/2000 - danhaff/ruswoods - created
//--------------------------------------------------------------------------;
HRESULT CICMusic::GetInterface(CtIDirectMusicLoader8** pptDirectMusicLoader8)
{
    if (pptDirectMusicLoader8 && m_ptLoader8)
    {
        *pptDirectMusicLoader8 = m_ptLoader8;
        m_ptLoader8->AddRef();
        return S_OK;
    }
    else if (!pptDirectMusicLoader8)
    {
        return E_POINTER;
    }

    return OLE_E_BLANK;
}


//--------------------------------------------------------------------------;
// Description: <nl>
//  Gets the performance object, which must be created upon initialization.
//
// Parameters: <nl>
//      pptPerformance8: contains address of pointer to contain wrapped IDirectMusicPerformance8.
//
// History: <nl>
//      03/09/2000 - danhaff/ruswoods - created
//--------------------------------------------------------------------------;
HRESULT CICMusic::GetInterface(CtIDirectMusicPerformance8 **pptPerformance8 )
{
    if (pptPerformance8 && m_ptPerformance8)
    {
        *pptPerformance8 = m_ptPerformance8;
        m_ptPerformance8->AddRef();
        return S_OK;
    }
    else if (!pptPerformance8)
    {
        return E_POINTER;
    }

    return OLE_E_BLANK;

};




//--------------------------------------------------------------------------;
// Description: <nl>
//      GetAudioPath
//
// Parameters: <nl>
//      pptAudioPath: contains address of pointer to contain wrapped IDirectMusicAudioPath.
//
// History: <nl>
//      03/09/2000 - danhaff/ruswoods - created
//--------------------------------------------------------------------------;
HRESULT CICMusic::GetInterface(CtIDirectMusicAudioPath** pptAudioPath)
{
    if (pptAudioPath && m_ptAudioPath)
    {
        *pptAudioPath= m_ptAudioPath;
        m_ptAudioPath->AddRef();
        return S_OK;
    }
    else if (!pptAudioPath)
    {
        return E_POINTER;
    }

    return OLE_E_BLANK;
}


//--------------------------------------------------------------------------;
// Description: <nl>
//      GetAudioPathConfig
//
// Parameters: <nl>
//      pptUnkAudioPathConfig: contains address of pointer to contain wrapped IDirectMusicAudioPathConfig.
//
// History: <nl>
//      03/09/2000 - danhaff/ruswoods - created
//--------------------------------------------------------------------------;
HRESULT CICMusic::GetInterface(CtIUnknown ** pptUnkAudioPathConfig)
{
    if (pptUnkAudioPathConfig && m_ptUnkAudioPathConfig)
    {
        *pptUnkAudioPathConfig= m_ptUnkAudioPathConfig;
        m_ptUnkAudioPathConfig->AddRef();
        return S_OK;
    }
    else if (!pptUnkAudioPathConfig)
    {
        return E_POINTER;
    }

    return OLE_E_BLANK;
}



//--------------------------------------------------------------------------;
// Description: <nl>
//      GetSegment8
//
// Parameters: <nl>
//      pptSegment8: contains address of pointer to contain wrapped IDirectMusicSegment8.
//
// History: <nl>
//      03/09/2000 - danhaff/ruswoods - created
//--------------------------------------------------------------------------;
HRESULT CICMusic::GetInterface(CtIDirectMusicSegment8** pptSegment8)
{
    if (pptSegment8 && m_ptSegment8)
    {
        *pptSegment8 = m_ptSegment8;
        m_ptSegment8->AddRef();
        return S_OK;
    }
    else if (!pptSegment8)
    {
        return E_POINTER;
    }

    return OLE_E_BLANK;
}

//--------------------------------------------------------------------------;
// Description: <nl>
//      GetSegmentState8
//
// Parameters: <nl>
//      pptSegmentState8: contains address of pointer to contain wrapped IDirectMusicSegmentState8.
//
// History: <nl>
//      03/09/2000 - danhaff/ruswoods - created
//--------------------------------------------------------------------------;
HRESULT CICMusic::GetInterface(CtIDirectMusicSegmentState8** pptSegmentState8)
{
    if (pptSegmentState8 && m_ptSegmentState8)
    {
        *pptSegmentState8 = m_ptSegmentState8;
        m_ptSegmentState8->AddRef();
        return S_OK;
    }
    else if (!pptSegmentState8)
    {
        return E_POINTER;
    }

    return OLE_E_BLANK;
}



//--------------------------------------------------------------------------;
// Description: <nl>
//      GetEnvironmentPath
//
// Parameters: <nl>
//      pptAudioPath: contains address of pointer to contain wrapped
//      IDirectMusicAudioPath.
//
// History: <nl>
//      05/06/2000 - danhaff - created
//--------------------------------------------------------------------------;
HRESULT CICMusic::GetAudioPathEnv(CtIDirectMusicAudioPath **pptAudioPathEnv)
{
    if (pptAudioPathEnv && m_ptAudioPathEnv)
    {
        *pptAudioPathEnv = m_ptAudioPathEnv;
        m_ptAudioPathEnv->AddRef();
        return S_OK;
    }
    else if (!pptAudioPathEnv)
    {
        return E_POINTER;
    }

    return OLE_E_BLANK;
}


