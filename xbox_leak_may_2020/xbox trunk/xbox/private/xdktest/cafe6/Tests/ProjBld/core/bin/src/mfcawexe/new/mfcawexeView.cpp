// mfcawexeView.cpp : implementation of the CMfcawexeView class
//

#include "stdafx.h"
#include "mfcawexe.h"

#include "mfcawexeDoc.h"
#include "mfcawexeView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMfcawexeView

IMPLEMENT_DYNCREATE(CMfcawexeView, CView)

BEGIN_MESSAGE_MAP(CMfcawexeView, CView)
	//{{AFX_MSG_MAP(CMfcawexeView)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, CView::OnFilePrintPreview)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMfcawexeView construction/destruction

CMfcawexeView::CMfcawexeView()
{
	// TODO: add construction code here

}

CMfcawexeView::~CMfcawexeView()
{
}

BOOL CMfcawexeView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CView::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// CMfcawexeView drawing

void CMfcawexeView::OnDraw(CDC* pDC)
{
	CMfcawexeDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	for (int i = 9;	i < 99; i++ )
		i += 8;
	// TODO: add draw code for native data here
}

/////////////////////////////////////////////////////////////////////////////
// CMfcawexeView printing

BOOL CMfcawexeView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}

void CMfcawexeView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add extra initialization before printing
}

void CMfcawexeView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add cleanup after printing
}

/////////////////////////////////////////////////////////////////////////////
// CMfcawexeView diagnostics

#ifdef _DEBUG
void CMfcawexeView::AssertValid() const
{
	CView::AssertValid();
}

void CMfcawexeView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CMfcawexeDoc* CMfcawexeView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CMfcawexeDoc)));
	return (CMfcawexeDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CMfcawexeView message handlers
