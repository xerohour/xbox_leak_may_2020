// ecmfcappView.cpp : implementation of the CEcmfcappView class
//

#include "stdafx.h"
#include "ecmfcapp.h"

#include "ecmfcappDoc.h"
#include "ecmfcappView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CEcmfcappView

IMPLEMENT_DYNCREATE(CEcmfcappView, CView)

BEGIN_MESSAGE_MAP(CEcmfcappView, CView)
	//{{AFX_MSG_MAP(CEcmfcappView)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, CView::OnFilePrintPreview)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CEcmfcappView construction/destruction

CEcmfcappView::CEcmfcappView()
{
	// TODO: add construction code here

}

CEcmfcappView::~CEcmfcappView()
{
}

BOOL CEcmfcappView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CView::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// CEcmfcappView drawing

void CEcmfcappView::OnDraw(CDC* pDC)
{
	CEcmfcappDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	// TODO: add draw code for native data here
}

/////////////////////////////////////////////////////////////////////////////
// CEcmfcappView printing

BOOL CEcmfcappView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}

void CEcmfcappView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add extra initialization before printing
}

void CEcmfcappView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add cleanup after printing
}

/////////////////////////////////////////////////////////////////////////////
// CEcmfcappView diagnostics

#ifdef _DEBUG
void CEcmfcappView::AssertValid() const
{
	CView::AssertValid();
}

void CEcmfcappView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CEcmfcappDoc* CEcmfcappView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CEcmfcappDoc)));
	return (CEcmfcappDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CEcmfcappView message handlers
