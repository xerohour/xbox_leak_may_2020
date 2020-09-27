#include "stdafx.h"
#include "ScriptDesignerDLL.h"
#include "LogTool.h"
#include <dmusicp.h>

CLogTool::CLogTool( IDMUSProdDebugScript* pIDebugScript, bool fLogToStdOut)
  : m_cRef(1),
	m_pIDebugScript(pIDebugScript),
	m_fLogToStdOut(fLogToStdOut)
{
}

CLogTool::~CLogTool()
{
}

//////////////////////////////////////////////////////////////////////
// IUnknown

STDMETHODIMP
CLogTool::QueryInterface( REFIID riid, LPVOID FAR *ppv )
{
	if( riid == IID_IUnknown
	||  riid == IID_IDirectMusicTool )
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

STDMETHODIMP_(ULONG)
CLogTool::AddRef()
{
	return InterlockedIncrement(&m_cRef);
}

STDMETHODIMP_(ULONG)
CLogTool::Release()
{
    if( !InterlockedDecrement(&m_cRef) )
    {
        delete this;
        return 0;
    }

    return m_cRef;
}

//////////////////////////////////////////////////////////////////////
//  IDirectMusicTool

STDMETHODIMP
CLogTool::Init( IDirectMusicGraph* pGraph )
{
	return S_OK;
}

STDMETHODIMP
CLogTool::GetMsgDeliveryType( DWORD* pdwDeliveryType )
{
	*pdwDeliveryType = DMUS_PMSGF_TOOL_QUEUE;
	return S_OK;
}


#define NBR_ELEMENTS	3
DWORD aMediaTypes[NBR_ELEMENTS] = { DMUS_PMSGT_SCRIPTLYRIC,
									DMUS_PMSGT_SCRIPTTRACKERROR,
									DMUS_PMSGT_LYRIC };

STDMETHODIMP
CLogTool::GetMediaTypeArraySize( DWORD* pdwNumElements )
{
	*pdwNumElements = NBR_ELEMENTS;
	return S_OK;
}

STDMETHODIMP
CLogTool::GetMediaTypes( DWORD** padwMediaTypes, DWORD dwNumElements )
{
	for( DWORD i = 0;  i < dwNumElements;  ++i )
	{
		if( i < NBR_ELEMENTS )
		{
			(*padwMediaTypes)[i] = aMediaTypes[i];
		}
		else
		{
			// Array is too large
			(*padwMediaTypes)[i] = 0;
		}
	}

	if( dwNumElements < NBR_ELEMENTS )
	{
		// Array is too small
		return S_FALSE;
	}

	return S_OK;
}

STDMETHODIMP
CLogTool::ProcessPMsg( IDirectMusicPerformance* pPerf, DMUS_PMSG* pPMSG )
{
	switch( pPMSG->dwType )
	{
		case DMUS_PMSGT_SCRIPTLYRIC:
		case DMUS_PMSGT_LYRIC:
		{
			DMUS_LYRIC_PMSG *pLyric = reinterpret_cast<DMUS_LYRIC_PMSG*>(pPMSG);
			
			m_pIDebugScript->DisplayText( pLyric->wszString );

			if( pPMSG->pGraph == NULL
			||  FAILED( pPMSG->pGraph->StampPMsg(pPMSG) ) )
			{
				return DMUS_S_FREE;
			}
			return DMUS_S_REQUEUE;
		}

		case DMUS_PMSGT_SCRIPTTRACKERROR:
		{
			DMUS_SCRIPT_TRACK_ERROR_PMSG *pScriptError = reinterpret_cast<DMUS_SCRIPT_TRACK_ERROR_PMSG*>(pPMSG);
			
			m_pIDebugScript->DisplayScriptError( &pScriptError->ErrorInfo );

			if( pPMSG->pGraph == NULL
			||  FAILED( pPMSG->pGraph->StampPMsg(pPMSG) ) )
			{
				return DMUS_S_FREE;
			}
			return DMUS_S_REQUEUE;
		}
	}

	return E_UNEXPECTED;
}

STDMETHODIMP
CLogTool::Flush( IDirectMusicPerformance* pPerf, DMUS_PMSG* pPMSG, REFERENCE_TIME rtTime )
{
	return S_OK;
}
