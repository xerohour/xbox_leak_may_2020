// CommonDoc.cpp : implementation file
//

#include "stdafx.h"
#include "JazzApp.h"
#include "SavePrompt.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCommonDoc

IMPLEMENT_DYNCREATE(CCommonDoc, COleDocument)


/////////////////////////////////////////////////////////////////////////////
// CCommonDoc::CCommonDoc

CCommonDoc::CCommonDoc()
{
	m_pIDocRootNode = NULL;

	AfxOleLockApp();
}


/////////////////////////////////////////////////////////////////////////////
// CCommonDoc::~CCommonDoc

CCommonDoc::~CCommonDoc()
{
	if( m_pIDocRootNode )
	{
		IDMUSProdNode* pINode = NULL;

		HTREEITEM hItem = theApp.m_pFramework->FindTreeItem( m_pIDocRootNode );
		if( hItem )
		{
			CTreeCtrl* pTreeCtrl = theApp.GetProjectTreeCtrl();
			if( pTreeCtrl )
			{
				// Remove DocRoot's FileNode
				hItem = pTreeCtrl->GetNextItem( hItem, TVGN_PARENT );
				if( hItem )
				{
					CJzNode* pJzNode = (CJzNode *)pTreeCtrl->GetItemData( hItem );
					if( pJzNode )
					{
						pINode = pJzNode->m_pINode;
					}
				}

				if( pINode == NULL )
				{
					pINode = m_pIDocRootNode;
				}

				theApp.m_pFramework->RemoveNodes( pINode );
			}
		}
	}

	if( m_pIDocRootNode )
	{
		m_pIDocRootNode->Release();
	}

	AfxOleUnlockApp();
}


/////////////////////////////////////////////////////////////////////////////
// CCommonDoc::SaveAndCloseDoc

BOOL CCommonDoc::SaveAndCloseDoc( void )
{
	if( SaveModified() )
	{
		OnCloseDocument();
		return TRUE;
	}

	return FALSE;
}


BEGIN_MESSAGE_MAP(CCommonDoc, COleDocument)
	//{{AFX_MSG_MAP(CCommonDoc)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CCommonDoc diagnostics

#ifdef _DEBUG
void CCommonDoc::AssertValid() const
{
	AFX_MANAGE_STATE( AfxGetAppModuleState() );
	COleDocument::AssertValid();
}

void CCommonDoc::Dump(CDumpContext& dc) const
{
	COleDocument::Dump(dc);
}
#endif //_DEBUG


/////////////////////////////////////////////////////////////////////////////
// CCommonDoc serialization

void CCommonDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: add storing code here
	}
	else
	{
		// TODO: add loading code here
	}
}


/////////////////////////////////////////////////////////////////////////////
// CCommonDoc commands
