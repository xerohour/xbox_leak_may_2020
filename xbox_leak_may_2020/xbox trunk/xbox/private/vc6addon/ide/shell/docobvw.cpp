// docobvw.cpp : implementation of the CDocObjectView class
//

#include <stdafx.h>

#include <afxres.h>

#include "resource.h"
#include "docobtpl.h"
#include "docobctr.h"
#include "docobdoc.h"
#include "docobvw.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDocObjectView

IMPLEMENT_DYNCREATE(CDocObjectView, CPartView)

BEGIN_MESSAGE_MAP(CDocObjectView, CPartView)
	//{{AFX_MSG_MAP(CDocObjectView)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	ON_WM_SETFOCUS()
	ON_WM_SIZE()
	ON_COMMAND(ID_FILE_PRINT, OnFilePrint)
	ON_COMMAND(ID_FILE_PAGE_SETUP, OnPageSetup)
	ON_UPDATE_COMMAND_UI(ID_FILE_PRINT, OnUpdatePrint)
	ON_UPDATE_COMMAND_UI(ID_FILE_PAGE_SETUP, OnUpdatePrint)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDocObjectView construction/destruction

CDocObjectView::CDocObjectView()
{
	m_pSelection = NULL;
	m_pOleCmdTarget = NULL;
}

CDocObjectView::~CDocObjectView()
{
	if(m_pOleCmdTarget != NULL)
		m_pOleCmdTarget->Release();
}

/////////////////////////////////////////////////////////////////////////////
// CDocObjectView drawing

void CDocObjectView::OnDraw(CDC* pDC)
{
	CDocObjectDoc *pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	// we don't draw anything ... the entire view window is owned by the server.
}

BOOL CDocObjectView::s_bInActivateView = FALSE;

void CDocObjectView::OnActivateView(BOOL bActivate, CView* pActivateView, 
	CView* pDeactiveView)
{
	if(!s_bInActivateView)
	{
		s_bInActivateView = TRUE; // This is to avoid deadly recursions
		CPartView::OnActivateView(bActivate, pActivateView, pDeactiveView);
		s_bInActivateView = FALSE;
	}
}

void CDocObjectView::OnInitialUpdate()
{
	// The document should contain exactly 1 OLE container item (otherwise the
	// doc shouldn't have been created successfully).  Get a pointer to the item.
	CDocObjectDoc *pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	POSITION position = pDoc->GetStartPosition();
	COleClientItem *pItemT = pDoc->GetNextClientItem(position);
	ASSERT(pItemT != NULL && pItemT->IsKindOf(RUNTIME_CLASS(CDocObjectContainerItem)) &&
		   pDoc->GetNextClientItem(position) == NULL);	// should be 1 and only 1 client item
	CDocObjectContainerItem *pItem = (CDocObjectContainerItem *)pItemT;

	if (pItem->GetOleDocument())
	{
		// The client item is a document object -- immediately tell it to 
		// activate and take over our view.
		// (If it isn't a document object, we display its inactive representation
		// until the user explicitly activates it.  This may not be able to happen
		// anyway, since we may not create a CDocObjectView unless the thing is
		// identifiable as a doc object ... however, who knows, it might refuse to
		// activate as a doc object for some reason.)
		pItem->Activate(OLEIVERB_SHOW, this);
	}

	if(pItem->m_lpObject != NULL)
	{
		pItem->m_lpObject->QueryInterface(IID_IOleCommandTarget, (void **)&m_pOleCmdTarget);
	}
	CPartView::OnInitialUpdate();	// see if the base class wants to do anything
}

/////////////////////////////////////////////////////////////////////////////
// OLE Client support and commands

BOOL CDocObjectView::IsSelected(const CObject* pDocItem) const
{
	// The implementation below is adequate if your selection consists of
	//  only CDocObjectContainerItem objects.  To handle different selection
	//  mechanisms, the implementation here should be replaced.

	// TODO: implement this function that tests for a selected OLE client item
	return pDocItem == m_pSelection;
}

void CDocObjectView::OnInsertObject()
{
	// this is not supported since it doesn't make sense.
	ASSERT(FALSE);
}

BOOL CDocObjectView::CanClose()
{
	CDocument *pDoc = GetDocument();
	ASSERT(pDoc != NULL);
	// We need to pass in NULL for pFrame, since we only want to check whether we can close 
	// the container item, but we don't want to prompt the user.
	if (pDoc != NULL)
		return(pDoc->CanCloseFrame(NULL));
	return(TRUE);
}

void CDocObjectView::OnUpdateTRUE(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(TRUE);
}

// The following command handler provides the standard keyboard
//  user interface to cancel an in-place editing session.  Here,
//  the container (not the server) causes the deactivation.
void CDocObjectView::OnCancelEditCntr()
{
	// Close any in-place active item on this view.
	COleClientItem* pActiveItem = GetDocument()->GetInPlaceActiveItem(this);
	if (pActiveItem != NULL)
	{
		pActiveItem->Close();
	}
	ASSERT(GetDocument()->GetInPlaceActiveItem(this) == NULL);
}

// Special handling of OnSetFocus and OnSize are required for a container
//  when an object is being edited in-place.
void CDocObjectView::OnSetFocus(CWnd* pOldWnd)
{
	COleClientItem* pActiveItem = GetDocument()->GetInPlaceActiveItem(this);
	if (pActiveItem != NULL && pActiveItem->GetItemState() == COleClientItem::activeUIState)
	{
		// need to set focus to this item if it is in the same view
		CWnd* pWnd = pActiveItem->GetInPlaceWindow();
		if (pWnd != NULL)
		{
			pWnd->SetFocus();   // don't call the base class
			return;
		}
	}

	CPartView::OnSetFocus(pOldWnd);
}

void CDocObjectView::OnSize(UINT nType, int cx, int cy)
{
	CPartView::OnSize(nType, cx, cy);

	COleClientItem* pActiveItem = GetDocument()->GetInPlaceActiveItem(this);
	if (pActiveItem != NULL)
	{
		ASSERT(pActiveItem->IsKindOf(RUNTIME_CLASS(CDocObjectContainerItem)));

		// Notify the doc object view of the size change.
		((CDocObjectContainerItem *)pActiveItem)->OnSizeChange();
	}
}

CDocObjectDoc *CDocObjectView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(COleDocument)));
	return((CDocObjectDoc *)m_pDocument);
}

void CDocObjectView::OnFilePrint()
{
	if(m_pOleCmdTarget != NULL)
	{
		m_pOleCmdTarget->Exec(	NULL, 
								OLECMDID_PRINT,			// Command ID
								OLECMDEXECOPT_PROMPTUSER,	// Exec Options
								NULL, NULL);	// Exec args
	}
}

void CDocObjectView::OnPageSetup()
{
	if(m_pOleCmdTarget != NULL)
	{
		m_pOleCmdTarget->Exec(	NULL, 
								OLECMDID_PAGESETUP,			// Command ID
								OLECMDEXECOPT_PROMPTUSER,	// Exec Options
								NULL, NULL);	// Exec args
	}
}

void CDocObjectView::OnUpdatePrint(CCmdUI* pCmdUI)
{
	if(m_pOleCmdTarget != NULL)
	{
		OLECMD rgcmds[2];

		rgcmds[0].cmdID = OLECMDID_PRINT;
		rgcmds[0].cmdf = 0;

		rgcmds[1].cmdID = OLECMDID_PAGESETUP;
		rgcmds[1].cmdf = 0;

		m_pOleCmdTarget->QueryStatus(NULL, 2, rgcmds, NULL);
		if(pCmdUI->m_nID == ID_FILE_PRINT)
			pCmdUI->Enable(rgcmds[0].cmdf & OLECMDF_ENABLED);
		else if(pCmdUI->m_nID == ID_FILE_PAGE_SETUP)
			pCmdUI->Enable(rgcmds[1].cmdf & OLECMDF_ENABLED);
	}
	else
		pCmdUI->Enable(FALSE);
}
/////////////////////////////////////////////////////////////////////////////
// CDocObjectView diagnostics

#ifdef _DEBUG
void CDocObjectView::AssertValid() const
{
	CPartView::AssertValid();
}

void CDocObjectView::Dump(CDumpContext& dc) const
{
	CPartView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
