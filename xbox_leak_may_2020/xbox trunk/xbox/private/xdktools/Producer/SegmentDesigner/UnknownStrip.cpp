// UnknownStrip.cpp : Implementation of CUnknownStrip
#include "stdafx.h"
#include "UnknownStripMgr.h"
#pragma warning( push )
#pragma warning( disable : 4201 )
#include <RiffStrm.h>
#pragma warning( pop )
#include <dmusici.h>
#include <dmusicf.h>

#define CF_UNKNOWNSTRIP "Microsoft DirectMusic Producer v.1 Unknown Strip"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// CUnknownStrip constructor/destructor

CUnknownStrip::CUnknownStrip( CUnknownStripMgr* pUnknownStripMgr )
{
	ASSERT( pUnknownStripMgr );
	if ( pUnknownStripMgr == NULL )
	{
		return;
	}

	m_pUnknownStripMgr = pUnknownStripMgr;
	m_pStripMgr = (IDMUSProdStripMgr*)pUnknownStripMgr;
	//m_pStripMgr->AddRef();

	// initialize our reference count
	m_cRef = 0;
	AddRef();

	m_bGutterSelected = FALSE;
	UpdateName();
}

CUnknownStrip::~CUnknownStrip()
{
	ASSERT( m_pStripMgr );
	if ( m_pStripMgr )
	{
		//m_pStripMgr->Release();
		m_pStripMgr = NULL;
		m_pUnknownStripMgr = NULL;
	}
}


/////////////////////////////////////////////////////////////////////////////
// CUnknownStrip IUnknown implementation

/////////////////////////////////////////////////////////////////////////////
// CUnknownStrip::QueryInterface

STDMETHODIMP CUnknownStrip::QueryInterface( REFIID riid, LPVOID *ppv )
{
	ASSERT( ppv );
	if ( ppv == NULL )
	{
		return E_INVALIDARG;
	}

    *ppv = NULL;

    if (IsEqualIID(riid, IID_IUnknown))
	{
        *ppv = (IUnknown *) (IDMUSProdStrip*) this;
	}
	else if (IsEqualIID(riid, IID_IDMUSProdStrip))
	{
        *ppv = (IUnknown *) (IDMUSProdStrip *) this;
	}
	else if (IsEqualIID(riid, IID_IDMUSProdStripFunctionBar))
	{
        *ppv = (IUnknown *) (IDMUSProdStripFunctionBar *) this;
	}
	else
	{
		return E_NOTIMPL;
	}

    ((IUnknown *) *ppv)->AddRef();
	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CUnknownStrip::AddRef

STDMETHODIMP_(ULONG) CUnknownStrip::AddRef(void)
{
	return ++m_cRef;
}


/////////////////////////////////////////////////////////////////////////////
// CUnknownStrip::Release

STDMETHODIMP_(ULONG) CUnknownStrip::Release(void)
{
	if( 0L == --m_cRef )
	{
		TRACE( "SEGMENT: CUnknownStrip destroyed!\n" );
		delete this;
		return 0;
	}
	else
	{
		return m_cRef;
	}
}


/////////////////////////////////////////////////////////////////////////////
// CUnknownStrip IDMUSProdStrip implementation

/////////////////////////////////////////////////////////////////////////////
// CUnknownStrip::Draw

HRESULT	STDMETHODCALLTYPE CUnknownStrip::Draw( HDC hDC, STRIPVIEW sv, LONG lXOffset )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	UNREFERENCED_PARAMETER(sv);

	// Get a pointer to the Timeline
	if( m_pUnknownStripMgr->m_pTimeline )
	{
		// Draw Measure and Beat lines in our strip
		m_pUnknownStripMgr->m_pTimeline->DrawMusicLines( hDC, ML_DRAW_MEASURE_BEAT, m_pUnknownStripMgr->m_dwGroupBits, 0, lXOffset );

		STATSTG statstg;
		if( SUCCEEDED( m_pUnknownStripMgr->m_pIStreamCopy->Stat( &statstg, STATFLAG_NONAME) ) )
		{
			CString strText, strFormat;
			if( statstg.cbSize.QuadPart != 0)
			{
				strFormat.LoadString( IDS_UNKNOWN_SOME_DATA );
				strText.Format( strFormat, statstg.cbSize.QuadPart );
			}
			else
			{
				strText.LoadString( IDS_UNKNOWN_NO_DATA );
			}

			RECT rect;
			rect.left = 0;
			rect.top = 0;
			VARIANT var;
			if( SUCCEEDED( m_pUnknownStripMgr->m_pTimeline->StripGetTimelineProperty( this, STP_HEIGHT, &var ) ) )
			{
				rect.bottom = V_I4(&var);
				if( SUCCEEDED( m_pUnknownStripMgr->m_pTimeline->GetTimelineProperty( TP_CLOCKLENGTH, &var ) ) )
				{
					if( SUCCEEDED( m_pUnknownStripMgr->m_pTimeline->ClocksToPosition( V_I4(&var), &rect.right ) ) )
					{
						::DrawText( hDC, strText, -1, &rect, DT_LEFT | DT_NOPREFIX | DT_SINGLELINE | DT_TOP );
					}
				}
			}
		}
	}
	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CUnknownStrip::GetStripProperty

HRESULT STDMETHODCALLTYPE CUnknownStrip::GetStripProperty( STRIPPROPERTY sp, VARIANT *pvar)
{
	if( NULL == pvar )
	{
		return E_POINTER;
	}

	switch( sp )
	{
	case SP_RESIZEABLE:
		// We are not resizable
		pvar->vt = VT_BOOL;
		V_BOOL(pvar) = FALSE;
		break;
	case SP_GUTTERSELECTABLE:
		// We don't support gutter selection
		pvar->vt = VT_BOOL;
		V_BOOL(pvar) = FALSE;
		break;
	case SP_MINMAXABLE:
		// We don't support Minimize/maximize
		pvar->vt = VT_BOOL;
		V_BOOL(pvar) = FALSE;
		break;
	case SP_DEFAULTHEIGHT:
	case SP_MAXHEIGHT:
	case SP_MINHEIGHT:
		// Our height is 20 pixels
		pvar->vt = VT_INT;
		V_INT(pvar) = 20;
		break;
	case SP_NAME:
		{
			BSTR bstr;

			pvar->vt = VT_BSTR; 
			try
			{
				bstr = m_strName.AllocSysString();
			}
			catch(CMemoryException*)
			{
				return E_OUTOFMEMORY;
			}
			V_BSTR(pvar) = bstr;
		}
		break;

	case SP_STRIPMGR:
		pvar->vt = VT_UNKNOWN;
		if( m_pUnknownStripMgr )
		{
			m_pUnknownStripMgr->QueryInterface( IID_IUnknown, (void **) &V_UNKNOWN(pvar) );
		}
		else
		{
			V_UNKNOWN(pvar) = NULL;
		}
		break;

	default:
		return E_FAIL;
	}
	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CUnknownStrip::SetStripProperty

HRESULT STDMETHODCALLTYPE CUnknownStrip::SetStripProperty( STRIPPROPERTY sp, VARIANT var)
{
	UNREFERENCED_PARAMETER(sp);
	UNREFERENCED_PARAMETER(var);
	return E_NOTIMPL;
}
/////////////////////////////////////////////////////////////////////////////
// CUnknownStrip::OnWMMessage

HRESULT STDMETHODCALLTYPE CUnknownStrip::OnWMMessage( UINT nMsg, WPARAM wParam, LPARAM lParam, LONG lXPos, LONG lYPos )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	UNREFERENCED_PARAMETER(lParam);
	UNREFERENCED_PARAMETER(lXPos);
	UNREFERENCED_PARAMETER(lYPos);

	// Process the window message
	HRESULT hr = S_OK;
	if( m_pUnknownStripMgr->m_pTimeline == NULL )
	{
		return E_FAIL;
	}

	switch( nMsg )
	{
	case WM_RBUTTONDOWN:
	case WM_LBUTTONDOWN:
		m_pUnknownStripMgr->OnShowProperties();
		break;

	case WM_RBUTTONUP:
		// Display a right-click context menu.
		POINT pt;
		BOOL	bResult;
		// Get the cursor position (To put the menu there)
		bResult = GetCursorPos( &pt );
		ASSERT( bResult );
		if( !bResult )
		{
			hr = E_UNEXPECTED;
			break;
		}

		m_pUnknownStripMgr->m_pTimeline->TrackPopupMenu(NULL, pt.x, pt.y, (IDMUSProdStrip *)this, TRUE);

		hr = S_OK;
		break;

	case WM_COMMAND:
		// We should only get this message in response to a selection in the right-click context menu.
		WORD wNotifyCode;
		WORD wID;

		wNotifyCode	= HIWORD( wParam );	// notification code 
		wID			= LOWORD( wParam );	// item, control, or accelerator identifier 
		switch( wID )
		{
		case ID_VIEW_PROPERTIES:
			// Change to the track property page
			ShowPropertySheet( m_pUnknownStripMgr->m_pTimeline );
			m_pUnknownStripMgr->OnShowProperties();
			break;
		default:
			break;
		}
		break;

	case WM_CREATE:
		UpdateName();
		break;

	default:
		break;
	}

	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CUnknownStrip IDMUSProdStripFunctionBar

/////////////////////////////////////////////////////////////////////////////
// CUnknownStrip::FBDraw

HRESULT CUnknownStrip::FBDraw( HDC hDC, STRIPVIEW sv )
{
	UNREFERENCED_PARAMETER(hDC);
	UNREFERENCED_PARAMETER(sv);
	return E_NOTIMPL;
}


/////////////////////////////////////////////////////////////////////////////
// CUnknownStrip::FBOnWMMessage

HRESULT CUnknownStrip::FBOnWMMessage( UINT nMsg, WPARAM wParam, LPARAM lParam, LONG lXPos, LONG lYPos )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	UNREFERENCED_PARAMETER(wParam);
	UNREFERENCED_PARAMETER(lParam);
	UNREFERENCED_PARAMETER(lXPos);
	UNREFERENCED_PARAMETER(lYPos);

	// Process the window message
	HRESULT hr = S_OK;
	switch( nMsg )
	{
	case WM_LBUTTONDOWN:
		m_pUnknownStripMgr->OnShowProperties();
		break;
	case WM_RBUTTONUP:
		m_pUnknownStripMgr->OnShowProperties();

		// Display a right-click context menu.
		POINT pt;
		// Get the cursor position (To put the menu there)
		if( !GetCursorPos( &pt ) )
		{
			hr = E_UNEXPECTED;
			break;
		}

		if( m_pUnknownStripMgr->m_pTimeline )
		{
			m_pUnknownStripMgr->m_pTimeline->TrackPopupMenu(NULL, pt.x, pt.y, (IDMUSProdStrip *)this, TRUE);
		}
		break;

	default:
		break;
	}
	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CUnknownStrip::ShowPropertySheet

HRESULT CUnknownStrip::ShowPropertySheet(IDMUSProdTimeline* pTimeline)
{
	HRESULT hr = S_OK;

	// Get a pointer to the property sheet and show it
	VARIANT			var;
	LPUNKNOWN		punk;
	IDMUSProdPropSheet*	pIPropSheet;
	pTimeline->GetTimelineProperty( TP_DMUSPRODFRAMEWORK, &var );
	if( var.vt == VT_UNKNOWN )
	{
		punk = V_UNKNOWN( &var );
		if( punk )
		{
			hr = punk->QueryInterface( IID_IDMUSProdPropSheet, (void**)&pIPropSheet );
			ASSERT( SUCCEEDED( hr ));
			if( FAILED( hr ))
			{
				hr = E_UNEXPECTED;
			}
			else
			{
				pIPropSheet->Show( TRUE );
				pIPropSheet->Release();
			}
			punk->Release();
		}
	}
	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CUnknownStrip::UpdateName

void CUnknownStrip::UpdateName()
{
	CString strText, strTmp;
	BOOL fFoundGroup = FALSE;
	BOOL fLastSet = FALSE;
	int nStartGroup = -1;

	for( int i = 0 ;  i < 32 ;  i++ )
	{
		if( m_pUnknownStripMgr->m_dwGroupBits & (1 << i) )
		{
			if( !fLastSet )
			{
				fLastSet = TRUE;
				nStartGroup = i;
			}
		}
		else
		{
			if( fLastSet )
			{
				fLastSet = FALSE;
				if( nStartGroup == i - 1 )
				{
					if( fFoundGroup )
					{
						strTmp.Format(", %d", i);
					}
					else
					{
						strTmp.Format("%d", i);
						fFoundGroup = TRUE;
					}
				}
				else
				{
					if( fFoundGroup )
					{
						strTmp.Format(", %d-%d", nStartGroup + 1, i);
					}
					else
					{
						strTmp.Format("%d-%d", nStartGroup + 1, i);
						fFoundGroup = TRUE;
					}
				}
				strText += strTmp;
			}
		}
	}

	if( fLastSet )
	{
		fLastSet = FALSE;
		if( nStartGroup == i - 1 )
		{
			if( fFoundGroup )
			{
				strTmp.Format(", %d", i);
			}
			else
			{
				strTmp.Format("%d", i);
				fFoundGroup = TRUE;
			}
		}
		else
		{
			if( fFoundGroup )
			{
				strTmp.Format(", %d-%d", nStartGroup + 1, i);
			}
			else
			{
				strTmp.Format("%d-%d", nStartGroup + 1, i);
				fFoundGroup = TRUE;
			}
		}
		strText += strTmp;
	}

	switch( m_pUnknownStripMgr->m_ckid )
	{
	case DMUS_FOURCC_MUTE_CHUNK:
		strTmp.LoadString( IDS_MUTE_STRIP );
		break;
	case DMUS_FOURCC_SEQ_TRACK:
		strTmp.LoadString( IDS_SEQ_STRIP );
		break;
	case DMUS_FOURCC_SYSEX_TRACK:
		strTmp.LoadString( IDS_SYSEX_STRIP );
		break;
	case DMUS_FOURCC_TIMESIGNATURE_TRACK:
		strTmp.LoadString( IDS_TIMESIG_STRIP );
		break;
	case 0:
	default:
		switch( m_pUnknownStripMgr->m_fccType )
		{
		case 0:
		default:
			strTmp.LoadString( IDS_UNKNOWN_STRIP );
			break;
		}
		break;
	}

	m_strName = strText + CString(": ") + strTmp;
}
