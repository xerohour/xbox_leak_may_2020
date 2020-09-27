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
//	History:
//		03/09/2000 - danhaff/ruswoods - created
//      03/27/2000 - danhaff - Made Init call UnInit first, fixed bugs
//      08/29/2000 - danhaff - Removed instances of DYNAMIC_APATH_ENV3D
//      01/05/2001 - danhaff - Ported to XBox.
//
/////////////////////////////////////////////////////////////////////////////

#include "globals.h"
#include "cicmusic.h"


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
//		03/09/2000 - danhaff/ruswoods - created
//
//--------------------------------------------------------------------------;
CICMusic::CICMusic(void)
:
m_pDirectSound (NULL),
m_pDirectMusic (NULL),
m_pLoader8 (NULL),
m_pPerformance8 (NULL),
m_pAudioPath (NULL),
m_pAudioPathEnv (NULL),
m_pUnkAudioPathConfig (NULL),
m_pSegment8(NULL),
m_pSegmentState8(NULL),
m_pSegmentState(NULL),
//m_hWnd (ghWnd),
m_bCOMInitialized(FALSE)
{
    //What are you looking at?
};


//--------------------------------------------------------------------------;
// Description: <nl>
//
// History: <nl>
//		03/09/2000 - danhaff/ruswoods - created
//
//--------------------------------------------------------------------------;
HRESULT CICMusic::Init
(
LPCWSTR wszSegment,
DWORD dwDefaultPathType,
//DWORD dwUseAPathEnv,
DWORD dwPChannelCount)
{
    HRESULT hr;

    __try
    {
        //First uninitialize anything we've initialized, so we don't leave dangling pointers.
        hr = PrivateUninitialize();
        if (S_OK != hr)
            __leave;
        
        //Now perform our initialization.
        hr = PrivateInitialize(wszSegment, NULL, dwDefaultPathType, /*dwUseAPathEnv,*/ dwPChannelCount);
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
//		03/09/2000 - danhaff/ruswoods - created
//
//--------------------------------------------------------------------------;
HRESULT CICMusic::Init(
LPCWSTR wszSegment,
LPCWSTR wszConfig)//,
//DWORD dwUseAPathEnv)
{
    HRESULT hr;
    __try
    {
        //First uninitialize anything we've initialized, so we don't leave dangling pointers.
        hr = PrivateUninitialize();
        if (S_OK != hr)
            __leave;

        //Now perform our initialization.
        hr = PrivateInitialize(wszSegment,              //Specified segment.
                               wszConfig,               //Specified AP config file.
                               NULL,                    //Don't use a standard path type.
                               //dwUseAPathEnv,    //Whether to create teh APATH_ENV
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
//		03/27/2000 - danhaff - created
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
//		03/09/2000 - danhaff/ruswoods - created
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
//		03/09/2000 - danhaff/ruswoods - created
//
//--------------------------------------------------------------------------;
HRESULT CICMusic::PrivateInitialize(
LPCWSTR wszSegment,
LPCWSTR wszConfig,
DWORD dwDefaultPathType,
//DWORD dwUseAPathEnv,
DWORD dwPChannelCount
)
{
HRESULT hr;
IDirectMusicPerformance8 *pPerformance8 = NULL;

    __try
    {
        //Verify that one of the correct types of standardpaths was passed, if any.
        if (dwDefaultPathType != 0                                  &&
            dwDefaultPathType != DMUS_APATH_SHARED_STEREOPLUSREVERB &&
            dwDefaultPathType != DMUS_APATH_DYNAMIC_3D              &&
            dwDefaultPathType != DMUS_APATH_DYNAMIC_STEREO          &&
            dwDefaultPathType != DMUS_APATH_DYNAMIC_MONO            
            )
        {
            fnsLog(ABORTLOGLEVEL, ALL, "**** TEST APP ERROR:  Someone passed an invalid value for dwDefaultPathType!!");
            __leave;
        }

/*
        //Call this pointless function.
        if (!m_bCOMInitialized)
        {
            dmthCoInitialize(NULL);
            m_bCOMInitialized = TRUE;
        }
*/

        //Create performance
        DirectMusicCreateInstance( CLSID_DirectMusicPerformance, NULL, IID_IDirectMusicPerformance8,(void**)&m_pPerformance8 );
	    if (S_OK != hr)
        {
            fnsLog(ABORTLOGLEVEL,
			TEXT("**** ABORT:  CICMusic: CreatePerformance returned (%s == %08Xh)"),
			tdmXlatHRESULT(hr), hr);
            __leave;

        }

	    //Create a new Loader8.
        DirectMusicCreateInstance(CLSID_DirectMusicLoader, NULL, IID_IDirectMusicLoader8,(void**)&m_pLoader8 );
	    if (S_OK != hr)
	    {
            fnsLog(ABORTLOGLEVEL, ALL, "**** ERROR: CICMusic: CreateLoader returned (%s == %08Xh)",
                    tdmXlatHRESULT(hr), hr);
            __leave;
	    }

        //Make loader aware of everything in media directory.
        //  Note: This may return S_FALSE because it didn't find files of the specified type - that is okay,

/*
        hr = SearchAndScanAllMedia((IDirectMusicLoader *)m_pLoader8);
        if (FAILED(hr))
        {
            fnsLog(ABORTLOGLEVEL, ALL, "**** ERROR: SearchAndScanAllMedia returned (%s == %08Xh)",
                    tdmXlatHRESULT(hr), hr);
            __leave;
        }
*/

        //This call should always succeed.
        hr = m_pLoader8->SetSearchDirectory(GUID_DirectMusicAllTypes, gwszMediaPath, FALSE);
        if (FAILED(hr))
        {
            fnsLog(ABORTLOGLEVEL, ALL, "**** ERROR: CICMusic: IDirectMusicLoader8::SetSearchDirectory failed!!");
            __leave;
        }


        //Initaudio, but w/o setting a default path.
        //hr = m_pPerformance8->InitAudio (&m_pDirectMusic, &m_pDirectSound, m_hWnd, NULL, NULL, NULL, NULL);
        hr = m_pPerformance8->InitAudio (&m_pDirectMusic, &m_pDirectSound, NULL, NULL, NULL, NULL, NULL);
        if (S_OK != hr)
        {
            // Log failure
            fnsLog(ABORTLOGLEVEL,
			TEXT("**** ABORT:  CICMusic: Perf8->InitAudio returned (%s == %08Xh)"),
			tdmXlatHRESULT(hr), hr);
            __leave;
        }

        //If we pass in an audiopath config file, make that the default path, otherwise choose
        //  and standard default path.  This decision was made by which constructor to use.
        if (wszConfig)
        {

            //Load up the audiopath config.
            //There is no actual interface for this; we use the IUnknown interface
            //  for this object.
            hr = m_pLoader8->LoadObjectFromFile(CLSID_DirectMusicAudioPathConfig,
                                                 CTIID_IUnknown,
                                                 (WCHAR *)wszConfig,
                                                 (void **)&m_pUnkAudioPathConfig);
            if (S_OK != hr)
            {
                fnsLog(ABORTLOGLEVEL,
    			TEXT("**** ABORT:  CICMusic: IDirectMusicLoader8->LoadObjectFromFile failed to load audiopath config %ls!! (%s == %08Xh)"),
			    PathFindFileNameW(wszConfig),
                tdmXlatHRESULT(hr), hr);
                __leave;
            }

            //Make that audiopath config into a real audiopath
            hr = m_pPerformance8->CreateAudioPath(m_pUnkAudioPathConfig, TRUE, &m_pAudioPath);
    	    if (S_OK != hr)
            {
                fnsLog(ABORTLOGLEVEL,
			    TEXT("**** ABORT:  CICMusic: Perf8->CreateAudioPath failed from loaded config file %ls!! (%s == %08Xh)"),
			    wszConfig, tdmXlatHRESULT(hr), hr);
                __leave;
            }

        }
        else
        {
            //Call CreateStandardAudioPath.
            hr = m_pPerformance8->CreateStandardAudioPath(dwDefaultPathType, dwPChannelCount, TRUE, &m_pAudioPath);
            if (S_OK != hr)
            {
                fnsLog(ABORTLOGLEVEL,
			    TEXT("**** ABORT:  CICMusic: Perf8->CreateStandardAudioPath returned (%s == %08Xh)"),
			    tdmXlatHRESULT(hr), hr);
                __leave;
            }            
        }


        //Set the audiopath we created as the default.
        hr = m_pPerformance8->SetDefaultAudioPath(m_pAudioPath);
    	if (S_OK != hr)
        {
            fnsLog(ABORTLOGLEVEL,
			TEXT("**** ABORT:  CICMusic: Perf8->SetDefaultAudioPath failed from file %ls!! (%s == %08Xh)"),
			wszConfig, tdmXlatHRESULT(hr), hr);
            __leave;
        }

        // Load the segment.
        hr = m_pLoader8->LoadObjectFromFile(CLSID_DirectMusicSegment,
                                        CTIID_IDirectMusicSegment8,
                                        (WCHAR *)wszSegment,
                                        (void **)&m_pSegment8);
        if (S_OK != hr)
        {
            fnsLog(ABORTLOGLEVEL,
			TEXT("**** ABORT:  CICMusic: IDirectMusicLoader8->LoadObjectFromFile failed to load segment %ls!! (%s == %08Xh)"),
            PathFindFileNameW(wszSegment),
			tdmXlatHRESULT(hr), hr);
                __leave;
        }

        //Tell the segment to repeat forever.
        hr = m_pSegment8->SetRepeats(DMUS_SEG_REPEAT_INFINITE);
        if (S_OK != hr)
        {
            fnsLog(ABORTLOGLEVEL,
			TEXT("**** ABORT:  CICMusic: IDirectMusicSegment8->SetRepeats(DMUS_SEG_REPEAT_INFINITE) returned (%s == %08Xh)"),
			tdmXlatHRESULT(hr), hr);
                __leave;
        }

        //Get the real Performance8 ptr from the performance, so we can pass it to SetParam.
        hr = m_pPerformance8->GetRealObjPtr(&pPerformance8);
        if (S_OK != hr)
        {
            fnsLog(ABORTLOGLEVEL,
			TEXT("**** ABORT:  CICMusic: IDirectMusicPerformance8->GetRealObjPtr returned (%s == %08Xh)"),
			tdmXlatHRESULT(hr), hr);
                __leave;
        }

        //Cause the correct instruments to be downloaded.
        hr = m_pSegment8->Download((IUnknown *)m_pAudioPath);
        if (FAILED(hr))
        {
            fnsLog(ABORTLOGLEVEL,
			TEXT("**** ABORT:  CICMusic: IDirectMusicSegment8->Download returned (%s == %08Xh)"),
			tdmXlatHRESULT(hr), hr);
                __leave;
        }


        //Play the segment on the default audio path, and get the segment state.
        hr = m_pPerformance8->PlaySegmentEx((IUnknown *)m_pSegment8,
                                            NULL,                   //no segment id, this isn't a song
                                            NULL,                   //no transition to compose
                                            NULL,                   //no DMUS_SEGF flags
                                            0,                      //time to start playback (right away)
                                            &m_pSegmentState,     //returned segment state
                                            NULL,                   //no segstate to replace
                                            NULL);                  //play on default audiopath.
        if (S_OK != hr)
        {
            fnsLog(ABORTLOGLEVEL,
			TEXT("**** ABORT:  CICMusic: IDirectMusicPerformance8->PlaySegmentEx returned (%s == %08Xh)"),
			tdmXlatHRESULT(hr), hr);
                __leave;
        }


        //Get a SegmentState8 also, since it's more useful and what we'll be testing.
        hr = dmthQueryInterface8(&m_pSegmentState, &m_pSegmentState8);
        if (S_OK != hr)
        {
            fnsLog(ABORTLOGLEVEL,
			TEXT("**** ABORT:  CICMusic: QI'ing SegmentState8 from SegmentState returned. (%s == %08Xh)"),
			tdmXlatHRESULT(hr), hr);
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
//		03/09/2000 - danhaff/ruswoods - created
//--------------------------------------------------------------------------;
HRESULT CICMusic::PrivateUninitialize(void)
{
    //Might as well have the return value somewhere.
    HRESULT hr = S_OK;

    //Unload what we downloaded.
    if (m_pAudioPath && m_pSegment8)
    {
        hr = m_pSegment8->Unload((IUnknown *)m_pAudioPath);
        if (FAILED(hr))
        {
            fnsLog(ABORTLOGLEVEL,
		    TEXT("**** WARNING!!! POSSIBLE MEMORY LEAK, CONTACT KCRAVEN OR DANHAFF:  CICMusic: IDirectMusicSegment8->Unload returned (%s == %08Xh)"),
		    tdmXlatHRESULT(hr), hr);
        }
    }

    //Release all the interfaces we've stashed, except the performance which must be Closed down and
    //  released last.
	SAFE_RELEASE (m_pSegmentState);
	SAFE_RELEASE (m_pSegmentState8);
    SAFE_RELEASE (m_pSegment8);
    SAFE_RELEASE (m_pAudioPath);
    SAFE_RELEASE (m_pAudioPathEnv);
    SAFE_RELEASE (m_pUnkAudioPathConfig);
	SAFE_RELEASE (m_pLoader8);
	SAFE_RELEASE (m_pDirectMusic);
    SAFE_RELEASE (m_pDirectSound);

    //Performance must ALWAYS be closed down last.
	if (m_pPerformance8)
    {
		hr = m_pPerformance8->CloseDown();
        if (FAILED(hr))
        {
            fnsLog(ABORTLOGLEVEL,
		    TEXT("**** WARNING!!! CONTACT KCRAVEN OR DANHAFF:  CICMusic: IDirectMusicPerformance8->CloseDown() returned (%s == %08Xh)"),
		    tdmXlatHRESULT(hr), hr);
        }
    }

    //Finally release the performance.
	SAFE_RELEASE (m_pPerformance8);


    //Undo whatever happened in dmthCoInitialize()
    if (m_bCOMInitialized)
    {
        dmthCoUninitialize();
        m_bCOMInitialized = FALSE;
    }

    return hr;
};



//--------------------------------------------------------------------------;
// Description: <nl>
//      GetDirectSound
//
// Parameters: <nl>
//      ppDirectSound: contains address of pointer to contain unwrapped IDirectSound.
//
// History: <nl>
//		03/09/2000 - danhaff/ruswoods - created
//--------------------------------------------------------------------------;
HRESULT CICMusic::GetInterface(IDirectSound ** ppDirectSound)
{
    if (ppDirectSound && m_pDirectSound)
    {
        *ppDirectSound = m_pDirectSound;
        m_pDirectSound->AddRef();
        return S_OK;
    }
    else if (!ppDirectSound)
    {
        return E_POINTER;
    }

    return OLE_E_BLANK;
}


//--------------------------------------------------------------------------;
// Description: <nl>
//      GetDirectMusic
//
// Parameters: <nl>
//      pptDirectMusic: contains address of pointer to contain wrapped IDirectMusic.
//
// History: <nl>
//		03/09/2000 - danhaff/ruswoods - created
//--------------------------------------------------------------------------;
HRESULT CICMusic::GetInterface(IDirectMusic** pptDirectMusic)
{
    if (pptDirectMusic && m_pDirectMusic)
    {
        *pptDirectMusic = m_pDirectMusic;
        m_pDirectMusic->AddRef();
        return S_OK;
    }
    else if (!pptDirectMusic)
    {
        return E_POINTER;
    }

    return OLE_E_BLANK;
}



//--------------------------------------------------------------------------;
// Description: <nl>
//      GetLoader8
//
// Parameters: <nl>
//      pptLoader8: contains address of pointer to contain wrapped IDirectMusicLoader8.
//
// History: <nl>
//		03/09/2000 - danhaff/ruswoods - created
//--------------------------------------------------------------------------;
HRESULT CICMusic::GetInterface(IDirectMusicLoader8** pptDirectMusicLoader8)
{
    if (pptDirectMusicLoader8 && m_pLoader8)
    {
        *pptDirectMusicLoader8 = m_pLoader8;
        m_pLoader8->AddRef();
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
//		03/09/2000 - danhaff/ruswoods - created
//--------------------------------------------------------------------------;
HRESULT CICMusic::GetInterface(IDirectMusicPerformance8 **pptPerformance8 )
{
    if (pptPerformance8 && m_pPerformance8)
    {
        *pptPerformance8 = m_pPerformance8;
        m_pPerformance8->AddRef();
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
//		03/09/2000 - danhaff/ruswoods - created
//--------------------------------------------------------------------------;
HRESULT CICMusic::GetInterface(IDirectMusicAudioPath** pptAudioPath)
{
    if (pptAudioPath && m_pAudioPath)
    {
        *pptAudioPath= m_pAudioPath;
        m_pAudioPath->AddRef();
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
//		03/09/2000 - danhaff/ruswoods - created
//--------------------------------------------------------------------------;
HRESULT CICMusic::GetInterface(IUnknown ** pptUnkAudioPathConfig)
{
    if (pptUnkAudioPathConfig && m_pUnkAudioPathConfig)
    {
        *pptUnkAudioPathConfig= m_pUnkAudioPathConfig;
        m_pUnkAudioPathConfig->AddRef();
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
//		03/09/2000 - danhaff/ruswoods - created
//--------------------------------------------------------------------------;
HRESULT CICMusic::GetInterface(IDirectMusicSegment8** pptSegment8)
{
    if (pptSegment8 && m_pSegment8)
    {
        *pptSegment8 = m_pSegment8;
        m_pSegment8->AddRef();
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
//		03/09/2000 - danhaff/ruswoods - created
//--------------------------------------------------------------------------;
HRESULT CICMusic::GetInterface(IDirectMusicSegmentState8** pptSegmentState8)
{
    if (pptSegmentState8 && m_pSegmentState8)
    {
        *pptSegmentState8 = m_pSegmentState8;
        m_pSegmentState8->AddRef();
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
//      GetSegmentState
//
// Parameters: <nl>
//      pptSegmentState: contains address of pointer to contain wrapped IDirectMusicSegmentState.
//
// History: <nl>
//		03/09/2000 - danhaff/ruswoods - created
//--------------------------------------------------------------------------;
HRESULT CICMusic::GetInterface(IDirectMusicSegmentState** pptSegmentState)
{
    if (pptSegmentState && m_pSegmentState)
    {
        *pptSegmentState = m_pSegmentState;
        m_pSegmentState->AddRef();
        return S_OK;
    }
    else if (!pptSegmentState)
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
//		05/06/2000 - danhaff - created
//--------------------------------------------------------------------------;
HRESULT CICMusic::GetAudioPathEnv(IDirectMusicAudioPath **pptAudioPathEnv)
{
    if (pptAudioPathEnv && m_pAudioPathEnv)
    {
        *pptAudioPathEnv = m_pAudioPathEnv;
        m_pAudioPathEnv->AddRef();
        return S_OK;
    }
    else if (!pptAudioPathEnv)
    {
        return E_POINTER;
    }

    return OLE_E_BLANK;
}