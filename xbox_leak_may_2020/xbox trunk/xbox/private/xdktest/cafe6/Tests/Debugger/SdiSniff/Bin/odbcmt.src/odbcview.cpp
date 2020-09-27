// ODBCMTView.cpp : implementation of the CODBCMTView class
//

#include "stdafx.h"
#include "ODBCMT.h"

#include "ODBCDoc.h"
#include "ODBCView.h"
#include "ODBCThrd.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CODBCMTView

IMPLEMENT_DYNCREATE(CODBCMTView, CScrollView)

BEGIN_MESSAGE_MAP(CODBCMTView, CScrollView)
	ON_MESSAGE(WM_USER_NOTIFY_STATUS, OnExecuteDone)
	//{{AFX_MSG_MAP(CODBCMTView)
	//}}AFX_MSG_MAP
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, CScrollView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, CScrollView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, CScrollView::OnFilePrintPreview)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CODBCMTView construction/destruction

CODBCMTView::CODBCMTView()
{
	// TODO: add construction code here

}

CODBCMTView::~CODBCMTView()
{
}

BOOL CODBCMTView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CScrollView::PreCreateWindow(cs);
}

void CODBCMTView::OnInitialUpdate()
{
	SetScrollSizes(MM_TEXT, GetDocument()->m_sizeDSN);
	CScrollView::OnInitialUpdate();
	int nb_runs=((CODBCMTApp*)AfxGetApp())->m_commandrun;
	if(nb_runs!=0)
	{	// the run was found on the command line
		GetDocument()->CreateAndExecute(TRUE,nb_runs);
		((CODBCMTApp*)AfxGetApp())->m_commandrun=0; // clear the cmnd line
	}
}

/////////////////////////////////////////////////////////////////////////////
// CODBCMTView drawing

void CODBCMTView::OnDraw(CDC* pDC)
{
	CODBCMTDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	int screenpos;
	int count;
   	for(count=0,screenpos=0; 
		count<pDoc->m_iCountConect; 
		count++,screenpos+=15)
	{
		int column=0;
		CString strproc;
		CODBCThread *pThread=(CODBCThread*)(pDoc->m_ThreadArray[count]);
		strproc = pThread->m_pSPName;
	//	while(1)
	//	{
	//		strcol= pDoc->m_resultList.GetNext( pos );
	//		if(strcol=="\n")
	//			break;
	//	}
		pDC->TextOut(column,screenpos,strproc);
		column+=20;screenpos+=15;
		pDC->TextOut(column,screenpos,pThread->m_Connect);
	}
}

/////////////////////////////////////////////////////////////////////////////
// CODBCMTView printing

BOOL CODBCMTView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default CScrollView preparation
	return CScrollView::OnPreparePrinting(pInfo);
}

void CODBCMTView::OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo)
{
	// Default CScrollView begin printing.
	CScrollView::OnBeginPrinting(pDC, pInfo);
}

void CODBCMTView::OnEndPrinting(CDC* pDC, CPrintInfo* pInfo)
{
	// Default CScrollView end printing
	CScrollView::OnEndPrinting(pDC, pInfo);
}

/////////////////////////////////////////////////////////////////////////////
// CODBCMTView diagnostics

#ifdef _DEBUG
void CODBCMTView::AssertValid() const
{
	CScrollView::AssertValid();
}

void CODBCMTView::Dump(CDumpContext& dc) const
{
	CScrollView::Dump(dc);
}

CODBCMTDoc* CODBCMTView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CODBCMTDoc)));
	return (CODBCMTDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CODBCMTView message handlers

 /*

LRESULT CRecalcView::OnNotifyStatus(WPARAM, LPARAM)
{
	GetDocument()->NotifyStatus();
	return 0;
}
*/


LRESULT CODBCMTView::OnExecuteDone(WPARAM wParam, LPARAM lParam)
{

	GetDocument()->ExecutionDone();
	return 0;
}

void CODBCMTView::OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint) 
{
	SetScrollSizes(MM_TEXT, GetDocument()->m_sizeDSN);
	CScrollView::OnUpdate(pSender,lHint,pHint);		
}

