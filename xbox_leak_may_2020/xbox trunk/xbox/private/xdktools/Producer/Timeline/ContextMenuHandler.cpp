// ContextMenuHandler.cpp : Implementation of temporary window to handle WM_COMMAND messages 
//							from the context menu.
//

#include "stdafx.h"
#include "TimelineCtl.h"
#include "ContextMenuHandler.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CContextMenuHandler

CContextMenuHandler::CContextMenuHandler()
{
	m_pStrip = NULL;
	m_hwndDeleteTracks = NULL;
	m_pTimeline = NULL;
	m_fCustomMenu = FALSE;
}

CContextMenuHandler::~CContextMenuHandler()
{
}


BEGIN_MESSAGE_MAP(CContextMenuHandler, CWnd)
	//{{AFX_MSG_MAP(CContextMenuHandler)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CContextMenuHandler message handlers

BOOL CContextMenuHandler::OnCommand( WPARAM wParam, LPARAM lParam ) 
{
	if( !m_fCustomMenu )
	{
		HRESULT hr = S_FALSE;

		if( ((LOWORD( wParam ) == ID_EDIT_DELETE_TRACK) || (LOWORD( wParam ) == ID_EDIT_ADD_TRACK)) && m_hwndDeleteTracks)
		{
			::SendMessage( m_hwndDeleteTracks, WM_COMMAND, wParam, lParam );
			hr = S_OK;
		}
		else if( m_pStrip != NULL )
		{
			switch( LOWORD( wParam ) )
			{
			case ID_EDIT_PASTE_MERGE:
				m_pTimeline->SetPasteType( TL_PASTE_MERGE );
				wParam = MAKELONG( ID_EDIT_PASTE, HIWORD(wParam) );
				break;
			case ID_EDIT_PASTE_OVERWRITE:
				m_pTimeline->SetPasteType( TL_PASTE_OVERWRITE );
				wParam = MAKELONG( ID_EDIT_PASTE, HIWORD(wParam) );
				break;
			}
			hr = m_pStrip->OnWMMessage( WM_COMMAND, wParam, lParam, 0, 0 );
		}
		else // m_pStrip == NULL
		{
			BOOL bReturn; // For OnZoomIn and OnZoomOut
			switch( LOWORD( wParam ) )
			{
			case ID_EDIT_PASTE_MERGE:
				m_pTimeline->SetPasteType( TL_PASTE_MERGE );
				hr = m_pTimeline->Paste( NULL );
				break;
			case ID_EDIT_PASTE_OVERWRITE:
				m_pTimeline->SetPasteType( TL_PASTE_OVERWRITE );
				hr = m_pTimeline->Paste( NULL );
				break;
			case ID_VIEW_PROPERTIES:
				m_pTimeline->DisplayPropertySheet();
				hr = m_pTimeline->OnShowProperties();
				break;
			case ID_EDIT_CUT:
				hr = m_pTimeline->Cut( NULL );
				break;
			case ID_EDIT_COPY:
				hr = m_pTimeline->Copy( NULL );
				break;
			case ID_EDIT_DELETE:
				hr = m_pTimeline->Delete();
				break;
			case ID_EDIT_PASTE:
				m_pTimeline->SetPasteType( TL_PASTE_MERGE );
				hr = m_pTimeline->Paste( NULL );
				break;
			case ID_EDIT_INSERT:
				hr = m_pTimeline->Insert();
				break;
			case ID_EDIT_SELECT_ALL:
				hr = m_pTimeline->SelectAll();
				break;
			case ID_EDIT_HORIZZOOMIN:
				m_pTimeline->OnZoomIn( 0, 0, 0, bReturn );
				hr = S_OK;
				break;
			case ID_EDIT_HORIZZOOMOUT:
				m_pTimeline->OnZoomOut( 0, 0, 0, bReturn );
				hr = S_OK;
				break;
			default:
				hr = E_FAIL;
				break;
			}
		}
		
		if( hr == S_OK )
		{
			return TRUE;
		}
	}
	else if( m_pStrip )
	{
		if( m_pStrip->OnWMMessage( WM_COMMAND, wParam, lParam, 0, 0 ) == S_OK )
		{
			return TRUE;
		}
	}

	return CWnd::OnCommand( wParam, lParam );
}
