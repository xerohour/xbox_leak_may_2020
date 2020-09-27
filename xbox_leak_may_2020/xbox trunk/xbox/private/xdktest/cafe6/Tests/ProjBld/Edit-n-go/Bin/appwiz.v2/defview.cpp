// defview.cpp : implementation of the CDefView class
//

#include "stdafx.h"
#include "def.h"

#include "defdoc.h"
#include "defview.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDefView

IMPLEMENT_DYNCREATE(CDefView, CView)

BEGIN_MESSAGE_MAP(CDefView, CView)
	//{{AFX_MSG_MAP(CDefView)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, CView::OnFilePrintPreview)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDefView construction/destruction

CDefView::CDefView()
{
	// TODO: add construction code here

}

CDefView::~CDefView()
{
}

/////////////////////////////////////////////////////////////////////////////
// CDefView drawing

void CDefView::OnDraw(CDC* pDC)
{
	CDefDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	// TODO: add draw code for native data here
}

/////////////////////////////////////////////////////////////////////////////
// CDefView printing

BOOL CDefView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}

void CDefView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add extra initialization before printing
}

void CDefView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add cleanup after printing
}

/////////////////////////////////////////////////////////////////////////////
// CDefView diagnostics

#ifdef _DEBUG
void CDefView::AssertValid() const
{
	CView::AssertValid();
}

void CDefView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CDefDoc* CDefView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CDefDoc)));
	return (CDefDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CDefView message handlers
