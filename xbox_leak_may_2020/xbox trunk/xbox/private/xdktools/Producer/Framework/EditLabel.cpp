// EditLabel.cpp : implementation file
//

#include "stdafx.h"
#include "JazzApp.h"
#include "EditLabel.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// CEditLabel

CEditLabel::CEditLabel( IDMUSProdNode* pINode )
{
	ASSERT( pINode != NULL );

	m_pINode = pINode;
}

CEditLabel::~CEditLabel()
{
}


BEGIN_MESSAGE_MAP(CEditLabel, CEdit)
	//{{AFX_MSG_MAP(CEditLabel)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CEditLabel message handlers

/////////////////////////////////////////////////////////////////////////////
// CEditLabel::PreTranslateMessage

BOOL CEditLabel::PreTranslateMessage( MSG* pMsg ) 
{
	switch( pMsg->message )
	{
		case WM_KEYUP:
			{
				CTreeCtrl* pTreeCtrl = theApp.GetProjectTreeCtrl();

				if( pTreeCtrl )
				{
					switch( pMsg->wParam )
					{
						case VK_ESCAPE:
							pTreeCtrl->PostMessage( TVM_ENDEDITLABELNOW, TRUE, 0 );
							return TRUE;
					}
				}
			}
			break;

		case WM_KEYDOWN:
			if( !(pMsg->lParam & 0x40000000) )
			{
				CTreeCtrl* pTreeCtrl = theApp.GetProjectTreeCtrl();

				if( pTreeCtrl )
				{
					switch( pMsg->wParam )
					{
						case VK_RETURN:
							pTreeCtrl->PostMessage( TVM_ENDEDITLABELNOW, FALSE, 0 );
							return TRUE;

						case VK_ESCAPE:
							pTreeCtrl->PostMessage( TVM_ENDEDITLABELNOW, TRUE, 0 );
							return TRUE;

						case 0x43:  // VK_C
							if( GetAsyncKeyState(VK_CONTROL) & 0x8000 )
							{
								Copy();
							}
							return FALSE;

						case 0x56:	// VK_V
							if( GetAsyncKeyState(VK_CONTROL) & 0x8000 )
							{
								Paste();
							}
							return FALSE;

						case 0x58:	// VK_X
							if( GetAsyncKeyState(VK_CONTROL) & 0x8000 )
							{
								Cut();
							}
							return FALSE;

						case 0x5A:	// VK_Z
							if( GetAsyncKeyState(VK_CONTROL) & 0x8000 )
							{
								BSTR bstrNodeName;

								if( SUCCEEDED ( m_pINode->GetNodeName( &bstrNodeName ) ) )
								{
									SetSel( 0, -1 );
									ReplaceSel( CString(bstrNodeName) );
									::SysFreeString( bstrNodeName );
									SetSel( 0, -1 );
								}
							}
							return FALSE;
					}
				}
			}
			break;
	}
	
	return CEdit::PreTranslateMessage( pMsg );
}
