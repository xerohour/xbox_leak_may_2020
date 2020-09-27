//-----------------------------------------------------------------------------
// File: ToolEcho.cpp
//-----------------------------------------------------------------------------

#include <globals.h>

//-----------------------------------------------------------------------------
// Name: CreateToolTrace
// Desc: 
//-----------------------------------------------------------------------------
HRESULT CreateToolTrace( IDirectMusicTool** ppTool )
{

	if ( NULL == ppTool )
		return E_POINTER;

	HRESULT hr = S_OK;
	IDirectMusicTool* pTool = NULL;

	pTool = new CToolTrace();

	if ( !pTool )
	{
		hr = E_OUTOFMEMORY;
	}

	if ( SUCCEEDED( hr ) )
	{
		*ppTool = pTool;
		pTool->AddRef();
	}

	return hr;
}


static IDirectMusicTool *pToolTrace = NULL;

//-----------------------------------------------------------------------------
// Name: InsertToolTrace
// Desc: Causes all lyric pMsgs to be displayed.
//-----------------------------------------------------------------------------
HRESULT InsertToolTrace(CtIDirectMusicPerformance *ptPerf8)
{
HRESULT hr = S_OK;
CtIDirectMusicAudioPath *ptPath = NULL;

CHECKRUN(ptPerf8->GetDefaultAudioPath(&ptPath));
if (!ptPath)
    Log(ABORTLOGLEVEL, "InsertToolTrace error: ptPerf8 doesn't have a default audiopath!");
CHECKRUN(InsertToolTrace(ptPath));
SAFE_RELEASE(ptPath);
return hr;
}



//-----------------------------------------------------------------------------
// Name: InsertToolTrace
// Desc: Causes all lyric pMsgs to be displayed.
//-----------------------------------------------------------------------------
HRESULT InsertToolTrace(CtIDirectMusicAudioPath *ptPath)
{
IDirectMusicGraph *pGraph = NULL;
HRESULT hr = S_OK;

    if (pToolTrace)
    {
        Log(ABORTLOGLEVEL, "InsertToolTrace Error: Tool is already allocated!!");
        return E_FAIL;
    }

CHECKRUN(CreateToolTrace(&pToolTrace));
CHECKRUN( ptPath->GetObjectInPath( 0, DMUS_PATH_PERFORMANCE_GRAPH, 0, CLSID_DirectMusicGraph, 0, IID_IDirectMusicGraph, (LPVOID*) &pGraph ) );
CHECKALLOC( pGraph );
CHECKRUN(pGraph->InsertTool(pToolTrace, NULL, NULL, 0));
SAFE_RELEASE(pGraph);
return hr;
};

//-----------------------------------------------------------------------------
// Name: RemoveToolTrace
// Desc: Causes all lyric pMsgs to be displayed.
//-----------------------------------------------------------------------------
void RemoveToolTrace(CtIDirectMusicPerformance *ptPerf8)
{
HRESULT hr = S_OK;
CtIDirectMusicAudioPath *ptPath = NULL;

CHECKRUN(ptPerf8->GetDefaultAudioPath(&ptPath));
if (!ptPath)
    Log(ABORTLOGLEVEL, "RemoveToolTrace error: ptPerf8 doesn't have a default audiopath!");
CHECK(RemoveToolTrace(ptPath));
SAFE_RELEASE(ptPath);
if (FAILED(hr))
{
    Log(ABORTLOGLEVEL, "ERROR (POSSIBLE LEAK): RemoveToolTrace(CtIDirectMusicPerformance) failed!  HR = %s (%08X)", tdmXlatHRESULT(hr), hr);
}

}


//-----------------------------------------------------------------------------
// Name: RemoveToolTrace
// Desc: 
//-----------------------------------------------------------------------------
void RemoveToolTrace(CtIDirectMusicAudioPath *ptPath)
{
IDirectMusicGraph *pGraph = NULL;
HRESULT hr = S_OK;

    if (!pToolTrace)
    {
        Log(ABORTLOGLEVEL, "RemoveToolTrace Error: Tool has not been allocated!!");
        return;
    }

    CHECKRUN(ptPath->GetObjectInPath( 0, DMUS_PATH_PERFORMANCE_GRAPH, 0, CLSID_DirectMusicGraph, 0, IID_IDirectMusicGraph, (LPVOID*) &pGraph ));
    CHECKALLOC(pGraph);
    CHECKRUN(pGraph->RemoveTool(pToolTrace));
    SAFE_RELEASE(pGraph);
    SAFE_RELEASE(pToolTrace);
    if (FAILED(hr))
    {
        Log(ABORTLOGLEVEL, "ERROR (POSSIBLE LEAK): RemoveToolTrace(CtIDirectMusicAudioPath) failed!  HR = %s (%08X)", tdmXlatHRESULT(hr), hr);
    }

};


//-----------------------------------------------------------------------------
// Name: CToolTrace::CToolTrace()
// Desc: 
//-----------------------------------------------------------------------------
CToolTrace::CToolTrace()
{
    m_cRef = 1;                 // Set to 1 so one call to Release() will free this
//    m_dwEchoNum = 3;            // Default to 3 echoes per note
//    m_mtDelay = DMUS_PPQ / 2;   // Default to 8th note echoes
    InitializeCriticalSection(&m_CrSec);
}




//-----------------------------------------------------------------------------
// Name: CToolTrace::~CToolTrace()
// Desc: 
//-----------------------------------------------------------------------------
CToolTrace::~CToolTrace()
{
    DeleteCriticalSection(&m_CrSec);
}




//-----------------------------------------------------------------------------
// Name: CToolTrace::QueryInterface()
// Desc: 
//-----------------------------------------------------------------------------
STDMETHODIMP CToolTrace::QueryInterface(const IID &iid, void **ppv)
{
    if (iid == IID_IUnknown || iid == IID_IDirectMusicTool)
    {
        *ppv = static_cast<IDirectMusicTool*>(this);
    } 
    else
    {
        *ppv = NULL;
        return E_NOINTERFACE;
    }
    
    reinterpret_cast<IUnknown*>(this)->AddRef();
    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: CToolTrace::AddRef()
// Desc: 
//-----------------------------------------------------------------------------
STDMETHODIMP_(ULONG) CToolTrace::AddRef()
{
    return InterlockedIncrement(&m_cRef);
}




//-----------------------------------------------------------------------------
// Name: CToolTrace::Release()
// Desc: 
//-----------------------------------------------------------------------------
STDMETHODIMP_(ULONG) CToolTrace::Release()
{
    if( 0 == InterlockedDecrement(&m_cRef) )
    {
        delete this;
        return 0;
    }

    return m_cRef;
}




//-----------------------------------------------------------------------------
// Name: CToolTrace::Init()
// Desc: 
//-----------------------------------------------------------------------------
HRESULT STDMETHODCALLTYPE CToolTrace::Init( IDirectMusicGraph* pGraph )
{
    // This tool has no need to do any type of initialization.
    return E_NOTIMPL;
}




//-----------------------------------------------------------------------------
// Name: CToolTrace::GetMsgDeliveryType()
// Desc: 
//-----------------------------------------------------------------------------
HRESULT STDMETHODCALLTYPE CToolTrace::GetMsgDeliveryType( DWORD* pdwDeliveryType )
{
    // This tool wants messages immediately.
    // This is the default, so returning E_NOTIMPL
    // would work. The other method is to specifically
    // set *pdwDeliveryType to the delivery type, DMUS_PMSGF_TOOL_IMMEDIATE,
    // DMUS_PMSGF_TOOL_QUEUE, or DMUS_PMSGF_TOOL_ATTIME.
    
    *pdwDeliveryType = DMUS_PMSGF_TOOL_IMMEDIATE;
    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: CToolTrace::GetMediaTypeArraySize()
// Desc: 
//-----------------------------------------------------------------------------
HRESULT STDMETHODCALLTYPE CToolTrace::GetMediaTypeArraySize( DWORD* pdwNumElements )
{
    *pdwNumElements = 2;
    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: CToolTrace::GetMediaTypes()
// Desc: 
//-----------------------------------------------------------------------------
HRESULT STDMETHODCALLTYPE CToolTrace::GetMediaTypes( DWORD** padwMediaTypes, 
                                                    DWORD dwNumElements )
{
    if( dwNumElements == 2 )
    {
        (*padwMediaTypes)[0] = DMUS_PMSGT_SCRIPTLYRIC;
        (*padwMediaTypes)[1] = DMUS_PMSGT_LYRIC;
        return S_OK;
    }
    else
    {
        // This should never happen
        return E_FAIL;
    }
}




//-----------------------------------------------------------------------------
// Name: CToolTrace::ProcessPMsg()
// Desc: 
//-----------------------------------------------------------------------------
HRESULT STDMETHODCALLTYPE CToolTrace::ProcessPMsg( IDirectMusicPerformance* pPerf, 
                                                  DMUS_PMSG* pPMsg )
{
    DMUS_LYRIC_PMSG *pLyricPMsg = (DMUS_LYRIC_PMSG *)pPMsg;
    CHAR szString[1000] = {0};

  //  wcstombs(szString, pLyricPMsg->wszString, 999); 
    sprintf(szString, "%ls", pLyricPMsg->wszString); 
    Log(FYILOGLEVEL, szString);
    return DMUS_S_FREE;
}




//-----------------------------------------------------------------------------
// Name: CToolTrace::Flush()
// Desc: 
//-----------------------------------------------------------------------------
HRESULT STDMETHODCALLTYPE CToolTrace::Flush( IDirectMusicPerformance* pPerf, 
                                            DMUS_PMSG* pDMUS_PMSG,
                                            REFERENCE_TIME rt)
{
    // This tool does not need to flush.
    return E_NOTIMPL;
}


