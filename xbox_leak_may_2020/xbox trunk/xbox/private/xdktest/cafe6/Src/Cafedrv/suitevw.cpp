/////////////////////////////////////////////////////////////////////////////
// suitevw.cpp
//
// email	date		change
// briancr	10/25/94	created
//
// copyright 1994 Microsoft

// Implementation of the CSuiteView class

#include "stdafx.h"
#include "suitedoc.h"
#include "suitevw.h"
#include "cafedrv.h"
#include "caferes.h"
#include "test.h"
#include "logpage.h"
#include "suitepg.h"
#include "testspg.h"
#include "statepg.h"

#define new DEBUG_NEW

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSuiteView

IMPLEMENT_DYNCREATE(CSuiteView, CView)

CSuiteView::CSuiteView()
: m_ptreeSuite(NULL)
{
}

CSuiteView::~CSuiteView()
{
	// delete the tree control
	if (m_ptreeSuite) {
		delete m_ptreeSuite;
	}
}


BEGIN_MESSAGE_MAP(CSuiteView, CView)
	//{{AFX_MSG_MAP(CSuiteView)
	ON_WM_CREATE()
	ON_COMMAND(IDM_SubSuiteRun, OnSubSuiteRun)
	ON_COMMAND(IDM_SubSuiteAdd, OnSubSuiteAdd)
	ON_COMMAND(IDM_SubSuiteDelete, OnSubSuiteDelete)
	ON_COMMAND(IDM_SubSuiteSelectAll, OnSubSuiteSelectAll)
	ON_COMMAND(IDM_SubSuiteDeselectAll, OnSubSuiteDeselectAll)
	ON_COMMAND(IDM_SubSuiteToggleAll, OnSubSuiteToggleAll)
	ON_COMMAND(IDM_SubSuiteProperties, OnSubSuiteProperties)
	ON_COMMAND(IDM_OptionsSuite, OnOptionsSuite)
	ON_WM_SIZE()
	ON_WM_SETFOCUS()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CSuiteView drawing

void CSuiteView::OnDraw(CDC* pDC)
{
	CDocument* pDoc = GetDocument();
	// TODO: add draw code here
	
	UNREFERENCED_PARAMETER(pDC);
}

/////////////////////////////////////////////////////////////////////////////
// CSuiteView diagnostics

#ifdef _DEBUG
void CSuiteView::AssertValid() const
{
	CView::AssertValid();
}

void CSuiteView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CSuiteDoc* CSuiteView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CSuiteDoc)));
	return (CSuiteDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// operations (internal)

void CSuiteView::FillTree(void)
{
	CSuiteDoc::SubSuiteList* plistSubSuites;
	CSuiteDoc::SubSuiteInfo* pSubSuiteInfo;
	CString strSubSuite;
	CSubSuite::TestList* plistTests;
	CTest* pTest;
	POSITION posSubSuite;
	POSITION posTest;
	CNode* pSubSuiteNode;

	// get a pointer to the document
	CSuiteDoc *pDoc = GetDocument();

	// get the list of subsuites
	plistSubSuites = pDoc->GetSubSuiteList();

	// for each subsuite in the list,
	for (posSubSuite = plistSubSuites->GetHeadPosition(); posSubSuite != NULL; ) {
		pSubSuiteInfo = plistSubSuites->GetNext(posSubSuite);

		// build the subsuite name (name + owner + dll name)
		strSubSuite = pSubSuiteInfo->m_pSubSuite->GetName(); /* + " - Owner: " + pSubSuiteInfo->m_pSubSuite->GetOwner() + " - Filename: " + pSubSuiteInfo->m_strFilename; */

		// enter the subsuite at the end of the list
		pSubSuiteNode = m_ptreeSuite->InsertNode(NULL, strSubSuite, pSubSuiteInfo->m_dwId, -1, IDB_GlyphFolderClose, IDB_GlyphFolderOpen, pSubSuiteInfo->m_bRun);

		// get the list of tests
		plistTests = pSubSuiteInfo->m_pSubSuite->GetTestList();

		// for each test in the subsuite,
		for (posTest = plistTests->GetHeadPosition(); posTest != NULL; ) {
			pTest = plistTests->GetNext(posTest);

			// enter the test in this subsuite at the end of the list
			m_ptreeSuite->InsertNode(pSubSuiteNode, pTest->GetName(), 0, -1, IDB_GlyphTest, IDB_GlyphTest, pTest->GetRunState() == CTest::RS_Run);
		}

		// collapse this node
		m_ptreeSuite->Collapse(pSubSuiteNode);
	//	UpdateSuite();
	}
}

BOOL CSuiteView::UpdateSuite(void)
{
	CSuiteDoc::SubSuiteList* plistSubSuites;
	CSuiteDoc::SubSuiteInfo* pSubSuiteInfo;
	CNode* pSubSuiteNode;
	CSubSuite::TestList* plistTests;
	CNode::CNodeList* plistChildren;
	CTest* pTest;
	CNode* pNode;
	POSITION posSubSuite;
	POSITION posChild;
	POSITION posTest;

	// get a pointer to the document
	CSuiteDoc *pDoc = GetDocument();

  CCAFEDriver *pApp = ((CCAFEDriver*)AfxGetApp());

  CPlatform *pPlatform = pApp -> CreateCurrentPlatform();


	// get the list of subsuites
	plistSubSuites = pDoc->GetSubSuiteList();

	// for each subsuite in the list,
	for (posSubSuite = plistSubSuites->GetHeadPosition(); posSubSuite != NULL; ) {
		pSubSuiteInfo = plistSubSuites->GetNext(posSubSuite);

		// get the node for this subsuite
		pSubSuiteNode = m_ptreeSuite->FindNode(pSubSuiteInfo->m_dwId);

		// record its selection status
		pSubSuiteInfo->m_bRun = pSubSuiteNode->GetFlags() & TF_SELECTED;

		// get the list of children for this node
		plistChildren = pSubSuiteNode->GetChildren();

		// get the list of tests
		plistTests = pSubSuiteInfo->m_pSubSuite->GetTestList();

		// these lists must be same length
		ASSERT(plistChildren->GetCount() == plistTests->GetCount());

		// for each test in the subsuite,
		for (posTest = plistTests->GetHeadPosition(), posChild = plistChildren->GetHeadPosition(); posTest != NULL; ) {
			pTest = plistTests->GetNext(posTest);
			pNode = plistChildren->GetNext(posChild);

			// record its selection status


#if 0
      // use this code to use the funky "crossed-out" glyphs in the tree control

      pNode->SetGlyphIds(
        (pTest->CanTestPlatform(pPlatform)) ?
        IDB_GlyphTest :
        IDB_GlyphBadTest);
#endif

      pTest->SetRunState(
        ((pNode->GetFlags() & TF_SELECTED) && (pTest->CanTestPlatform(pPlatform))) ?
        CTest::RS_Run :
        CTest::RS_NoRun);


		}
	}

	delete pPlatform;
	
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CSuiteView message handlers

int CSuiteView::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	CRect rectWnd;
	CRect rectView;

	// call the base class
	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	// get the size of the view
	GetParent()->GetClientRect(&rectWnd);
/*
	GetParent()->GetWindowRect(&rectWnd);
	ScreenToClient(&rectWnd);
*/
	SetWindowPos(NULL, 0, 0, rectWnd.right, rectWnd.bottom, SWP_NOMOVE | SWP_NOZORDER);
	GetClientRect(&rectView);
/*
	GetWindowRect(&rectWnd);
	ScreenToClient(&rectWnd);
*/

	// create the suite tree control
//	m_ptreeSuite = new CTreeCtl(TRUE, TRUE);
	m_ptreeSuite = new CTreeCtl(TRUE,TRUE,TRUE) ;
	if (!m_ptreeSuite->Create(NULL, NULL, WS_CHILD | WS_VISIBLE, rectView, this, 0)) {
		return -1;
	}					  

	return 0;
}

void CSuiteView::OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint)
{
	// TODO: Add your specialized code here and/or call the base class

  UpdateSuite();
  m_ptreeSuite->UpdateWindow();
	

	UNREFERENCED_PARAMETER(pSender);
	UNREFERENCED_PARAMETER(lHint);
	UNREFERENCED_PARAMETER(pHint);
}

void CSuiteView::OnSubSuiteRun() 
{
	// get a pointer to the document
	CSuiteDoc *pDoc = GetDocument();

	// update the data from the UI (selected, nonselected)
	UpdateSuite();

	// the document runs the suite
	pDoc->Run();
}

void CSuiteView::OnSubSuiteAdd() 
{
	
}

void CSuiteView::OnSubSuiteDelete() 
{
	
}

void CSuiteView::OnSubSuiteProperties() 
{
	
}

void CSuiteView::OnSubSuiteSelectAll() 
{
	m_ptreeSuite->SelectAll();
}

void CSuiteView::OnSubSuiteDeselectAll() 
{
	m_ptreeSuite->DeselectAll();
}

void CSuiteView::OnSubSuiteToggleAll() 
{
	m_ptreeSuite->ToggleAll();
}

void CSuiteView::OnSize(UINT nType, int cx, int cy) 
{
	// call the base class
	CView::OnSize(nType, cx, cy);

	// resize the tree control
	if (m_ptreeSuite) {
		m_ptreeSuite->MoveWindow(0, 0, cx, cy);
	}

}

void CSuiteView::OnSetFocus(CWnd* pOldWnd) 
{
	CView::OnSetFocus(pOldWnd);
	
	// set focus to the tree
	m_ptreeSuite->SetFocus();
}


void CSuiteView::OnInitialUpdate() 
{
	// fill the tree control with the subsuites and tests
	FillTree();
	m_ptreeSuite->UpdateWindow();

	CView::OnInitialUpdate();
}

void CSuiteView::OnOptionsSuite()
{
	// get a pointer to the document
	CSuiteDoc *pDoc = GetDocument();

    // create the tabbed options dialog
    CPropertySheet dlgSuiteOptions(IDS_OptionsSuite);

    // create the pages
	CSuitePage pageSuite(pDoc);
    CLogPage pageLog;
	CTestsRunPage pageTestsRun(pDoc);
	CStatePage pageCurrentState(pDoc);

	// initialize the log page with this suite's data
	pageLog.m_strResultsFile = pDoc->GetResultsFile();
	pageLog.m_strSummaryFile = pDoc->GetSummaryFile();
	pageLog.m_strWorkDir = pDoc->GetWorkingDir();
	pageLog.m_nMaxFail = pDoc->GetMaxFail();
	pageLog.m_bPost = pDoc->GetPostResults();
	pageLog.m_bComments = pDoc->GetRecordComments();
	pageLog.m_bDebugOutput = pDoc->GetLogDebugOutput();

    // add the pages to the sheet
	dlgSuiteOptions.AddPage(&pageSuite);
    dlgSuiteOptions.AddPage(&pageLog);
	dlgSuiteOptions.AddPage(&pageTestsRun);
	dlgSuiteOptions.AddPage(&pageCurrentState);

    // bring up the tabbed dialog
    if (dlgSuiteOptions.DoModal() == IDOK) {
		// store the data from the log page
		pDoc->SetResultsFile(pageLog.m_strResultsFile);
		pDoc->SetSummaryFile(pageLog.m_strSummaryFile);
		pDoc->SetWorkingDir(pageLog.m_strWorkDir);
		pDoc->SetMaxFail(pageLog.m_nMaxFail);
		pDoc->SetPostResults(pageLog.m_bPost);
		pDoc->SetRecordComments(pageLog.m_bComments);
		pDoc->SetLogDebugOutput(pageLog.m_bDebugOutput);
    }
}
