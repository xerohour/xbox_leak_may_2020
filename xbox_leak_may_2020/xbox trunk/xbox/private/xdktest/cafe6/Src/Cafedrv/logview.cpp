// logview.cpp : implementation file
//

#include "stdafx.h"
#include "afxcview.h"
#include "cafedrv.h"
#include <afxtempl.h>
#include "portdoc.h"
#include "logview.h"							
#include "caferes.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CLogView * gLogView ;
/////////////////////////////////////////////////////////////////////////////
// CLogView
CViewportView * vConsole, *vTextLog ;

IMPLEMENT_DYNCREATE(CLogView, CTreeView)

CLogView::CLogView()
{
  // m_stNode = m_substNode = m_tstCaseNode = m_detailNode = m_sumNode = m_otherNode ;

   tv_Item.mask = TVIF_TEXT|TVIF_CHILDREN|TVIF_SELECTEDIMAGE|TVIF_PARAM|TVIF_STATE|TVIF_IMAGE ; //Generic item 
   //tv_Item.state = TVIS_EXPANDED ;
   tv_Item.lParam = 1 ;
   // The main nodes for the CAFE log tree.
   tv_Suite   =	new TV_INSERTSTRUCT ;
   tv_SubSuite=	new TV_INSERTSTRUCT ;
   tv_Case	  =	new TV_INSERTSTRUCT ;
   tv_Details = new TV_INSERTSTRUCT ;
   tv_Summary = new TV_INSERTSTRUCT ;
   tv_Other   =	new TV_INSERTSTRUCT ;
   isFirstSuite = 1, isFirstSubSuite=1,isFirstTestCase=1, isFirstDetails=1,isFirstSummary=1 ;
   //hSubSuite= 0 , hTestCase= 0,  hDetails = 0, hSummary = 0;
    hParentSubSuite = 0, hParentSuite =0 , hParentTest = 0 ;

}

CLogView::~CLogView()
{
	 delete tv_Suite ;
	 delete tv_SubSuite ;
	 delete tv_Case ;
	 delete tv_Details ;
	 delete tv_Summary ;
	 delete tv_Other ;
}


BEGIN_MESSAGE_MAP(CLogView, CTreeView)
	//{{AFX_MSG_MAP(CLogView)
	ON_NOTIFY_REFLECT(TVN_SELCHANGED, OnSelchanged)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CLogView drawing

void CLogView::OnDraw(CDC* pDC)
{
	CDocument* pDoc = GetDocument();
//	CViewportDoc* pDoc = (CViewportDoc*) GetDocument();
	ASSERT_VALID(pDoc);

	CTreeCtrl & bsCtrl = GetTreeCtrl() ;

//	bsCtrl.InsertItem(&tv_inStruct) ;
//	bsCtrl.SetItem(&tvItem) ; 
 
	// TODO: add draw code here
}

/////////////////////////////////////////////////////////////////////////////
// CLogView diagnostics

#ifdef _DEBUG
void CLogView::AssertValid() const
{
	CTreeView::AssertValid();
}

void CLogView::Dump(CDumpContext& dc) const
{
	CTreeView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CLogView message handlers

BOOL CLogView::Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext) 
{
	// TODO: Add your specialized code here and/or call the base class
	CCAFEDriver* TheApp = (CCAFEDriver*) AfxGetApp() ;
	pImageList.Create( 16, 16, FALSE, 2, 1 );
	pImageList.Add(TheApp->hIpass) ;
	pImageList.Add(TheApp->hIfail) ;
	return CWnd::Create(lpszClassName, lpszWindowName, dwStyle, rect, pParentWnd, nID, pContext);
}

void CLogView::ReportFailToTest()
{
	GetParentTest()	;
}
void CLogView::ReportFailToSubSuite() 
{
	GetParentTest()	;
}

void CLogView::ReportFailToSuite() 
{
	GetParentTest()	;
}


void CLogView::AddNewNode(NodeTypes iType, int nLine, CString szText)
{  
   CTreeCtrl & THE_TV_CTRL= GetTreeCtrl() ;

   if(!THE_TV_CTRL)
	   return ;
	tv_Item.lParam = nLine ; // The line number of this item in the log file.
	switch(iType)
	{
	case SUITE:
		
		{
		 tv_Item.mask = TVIF_TEXT|TVIF_SELECTEDIMAGE|TVIF_PARAM|TVIF_IMAGE ; //Generic item 

			TV_INSERTSTRUCT tv_suiteCpy;
			tv_Item.pszText = szText.GetBuffer(80);
			tv_suiteCpy.hInsertAfter=TVI_LAST ;
			tv_suiteCpy.item =  tv_Item ;
			tv_suiteCpy.hParent = TVI_ROOT;
			hParentSuite = THE_TV_CTRL.InsertItem(&tv_suiteCpy) ;
			THE_TV_CTRL.Expand(hSuite,TVE_EXPAND) ;

		}
		break ;
	case SUBSUITE:
		{
			TV_INSERTSTRUCT tv_suiteCpy;
			tv_Item.pszText = szText.GetBuffer(80);
			tv_suiteCpy.hInsertAfter=TVI_LAST ;
			tv_suiteCpy.item =  tv_Item ;
			tv_suiteCpy.hParent = GetParentSuite();
			hParentSubSuite =THE_TV_CTRL.InsertItem(&tv_suiteCpy) ;
			THE_TV_CTRL.Expand(THE_TV_CTRL.GetParentItem(hParentSubSuite),TVE_EXPAND) ;

		}
		break;
	case TESTCASE:
		{
			TV_INSERTSTRUCT tv_suiteCpy;
			tv_Item.pszText = szText.GetBuffer(80);
			tv_suiteCpy.hInsertAfter=TVI_LAST ;
			tv_suiteCpy.item =  tv_Item ;
			tv_suiteCpy.hParent = GetParentSubSuite() ;
			hParentTest = THE_TV_CTRL.InsertItem(&tv_suiteCpy) ;
			THE_TV_CTRL.Expand(THE_TV_CTRL.GetParentItem(hParentTest),TVE_EXPAND) ;
		}
		break;
	case FAILURE:
		{
			
			TV_INSERTSTRUCT tv_suiteCpy;
			tv_Item.pszText = szText.GetBuffer(80);
			tv_Item.iImage =1 ;
			tv_Item.iSelectedImage =1 ;
			tv_suiteCpy.hInsertAfter=TVI_LAST ;
			tv_suiteCpy.item =  tv_Item ;
			if (GetParentTest())
				tv_suiteCpy.hParent =  GetParentTest() ;
				else
					if (GetParentSubSuite())
						tv_suiteCpy.hParent = GetParentSubSuite() ;
					else
						if (GetParentSuite())
							tv_suiteCpy.hParent = GetParentSuite() ;
						else
							tv_suiteCpy.hParent = hSuite ;
			HTREEITEM hItem = THE_TV_CTRL.InsertItem(&tv_suiteCpy) ;
			THE_TV_CTRL.Expand(THE_TV_CTRL.GetParentItem(hItem),TVE_EXPAND) ;
			// Reset the bitmaps.
			tv_Item.iImage =0 ;
			tv_Item.iSelectedImage =0 ;
		}
		break;

	case DETAILS:
		{
		}
		break;

	case SUMMARY:
		{  
			TV_INSERTSTRUCT tv_suiteCpy;
			tv_Item.pszText = szText.GetBuffer(80);
			tv_suiteCpy.hInsertAfter=TVI_LAST ;
			tv_suiteCpy.item =  tv_Item ;
			tv_suiteCpy.hParent = GetParentSuite() ;
			THE_TV_CTRL.InsertItem(&tv_suiteCpy) ;
		}
		break;

	default:
	;	
	}
	THE_TV_CTRL.Expand(GetParentSuite(),TVE_EXPAND) ;
//	tv_Suite   	tv_SubSuite 	tv_Case	  	tv_Details 	tv_Summary 	tv_Other   
}

void CLogView::OnInitialUpdate() 
{
   gLogView = this ;
   CTreeCtrl & THE_TV_CTRL= GetTreeCtrl() ;

   long ctrlStyle = GetWindowLong(THE_TV_CTRL.m_hWnd,GWL_STYLE) ;
   ctrlStyle = ctrlStyle| TVS_HASLINES|TVS_LINESATROOT ;
   SetWindowLong(THE_TV_CTRL.m_hWnd,GWL_STYLE,ctrlStyle) ; 

    THE_TV_CTRL.SetImageList( &pImageList,TVSIL_NORMAL );

   	// Add the first Suite node.
	tv_Item.pszText = "Test suite summary:" ;
	tv_Item.iImage = 0 ;		   
	tv_Item.iSelectedImage = 0 ;
	tv_Suite->item =  tv_Item ;
	tv_Suite->hParent = TVI_ROOT ;
	hSuite =THE_TV_CTRL.InsertItem(tv_Suite) ;
		
   
    CTreeView::OnInitialUpdate();
	// TODO: Add your specialized code here and/or call the base class
	
}

void CLogView::OnSelchanged(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;
	// TODO: Add your control notification handler code here
    CTreeCtrl & THE_TV_CTRL= GetTreeCtrl() ;
	HTREEITEM hItem = THE_TV_CTRL.GetSelectedItem( );
	DWORD dLine = THE_TV_CTRL.GetItemData(hItem) ;
	vConsole->SelectLine(dLine) ;
	vTextLog->SelectLine(dLine) ; 

	*pResult = 0;
}
