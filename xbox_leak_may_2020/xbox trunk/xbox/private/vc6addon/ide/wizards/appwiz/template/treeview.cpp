// $$treeview_ifile$$.cpp : implementation of the $$TREEVIEW_CLASS$$ class
//

#include "stdafx.h"
#include "$$root$$.h"

#include "$$doc_hfile$$.h"
#include "$$treeview_hfile$$.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// $$TREEVIEW_CLASS$$

IMPLEMENT_DYNCREATE($$TREEVIEW_CLASS$$, $$TREEVIEW_BASE_CLASS$$)

BEGIN_MESSAGE_MAP($$TREEVIEW_CLASS$$, $$TREEVIEW_BASE_CLASS$$)
	//{{AFX_MSG_MAP($$TREEVIEW_CLASS$$)
$$IF(VERBOSE)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
$$ENDIF
	//}}AFX_MSG_MAP
$$IF(PRINT)
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, $$TREEVIEW_BASE_CLASS$$::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, $$TREEVIEW_BASE_CLASS$$::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, $$TREEVIEW_BASE_CLASS$$::OnFilePrintPreview)
$$ENDIF //PRINT
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// $$TREEVIEW_CLASS$$ construction/destruction

$$TREEVIEW_CLASS$$::$$TREEVIEW_CLASS$$()
{
$$IF(VERBOSE)
	// TODO: add construction code here

$$ENDIF
}

$$TREEVIEW_CLASS$$::~$$TREEVIEW_CLASS$$()
{
}

BOOL $$TREEVIEW_CLASS$$::PreCreateWindow(CREATESTRUCT& cs)
{
$$IF(VERBOSE)
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

$$ENDIF //VERBOSE
	return $$TREEVIEW_BASE_CLASS$$::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// $$TREEVIEW_CLASS$$ drawing

void $$TREEVIEW_CLASS$$::OnDraw(CDC* pDC)
{
	$$DOC_CLASS$$* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
$$IF(VERBOSE)

	// TODO: add draw code for native data here
$$ENDIF //VERBOSE
}

$$IF(PRINT)

/////////////////////////////////////////////////////////////////////////////
// $$TREEVIEW_CLASS$$ printing

BOOL $$TREEVIEW_CLASS$$::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}

void $$TREEVIEW_CLASS$$::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
$$IF(VERBOSE)
	// TODO: add extra initialization before printing
$$ENDIF //VERBOSE
}

void $$TREEVIEW_CLASS$$::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
$$IF(VERBOSE)
	// TODO: add cleanup after printing
$$ENDIF //VERBOSE
}
$$ENDIF //PRINT

void $$TREEVIEW_CLASS$$::OnInitialUpdate()
{
	$$TREEVIEW_BASE_CLASS$$::OnInitialUpdate();
$$IF(VERBOSE)

	// TODO: You may populate your TreeView with items by directly accessing
	//  its tree control through a call to GetTreeCtrl().
$$ENDIF //VERBOSE
}

/////////////////////////////////////////////////////////////////////////////
// $$TREEVIEW_CLASS$$ diagnostics

#ifdef _DEBUG
void $$TREEVIEW_CLASS$$::AssertValid() const
{
	$$TREEVIEW_BASE_CLASS$$::AssertValid();
}

void $$TREEVIEW_CLASS$$::Dump(CDumpContext& dc) const
{
	$$TREEVIEW_BASE_CLASS$$::Dump(dc);
}

$$DOC_CLASS$$* $$TREEVIEW_CLASS$$::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS($$DOC_CLASS$$)));
	return ($$DOC_CLASS$$*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// $$TREEVIEW_CLASS$$ message handlers
