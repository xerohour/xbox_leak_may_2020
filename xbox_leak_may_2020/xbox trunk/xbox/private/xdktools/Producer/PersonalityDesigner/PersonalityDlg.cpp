// PersonalityDlg.cpp : implementation file
//

#include "stdafx.h"
#pragma warning(disable:4201)
#include "PersonalityDesigner.h"
#include "PersonalityDlg.h"
#pragma warning(default:4201)

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPersonalityDlg

IMPLEMENT_DYNCREATE(CPersonalityDlg, CFormView)

CPersonalityDlg::CPersonalityDlg()
	: CFormView(CPersonalityDlg::IDD)
{
	//{{AFX_DATA_INIT(CPersonalityDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

CPersonalityDlg::~CPersonalityDlg()
{
}

void CPersonalityDlg::DoDataExchange(CDataExchange* pDX)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	CFormView::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPersonalityDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPersonalityDlg, CFormView)
	//{{AFX_MSG_MAP(CPersonalityDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPersonalityDlg diagnostics

#ifdef _DEBUG
void CPersonalityDlg::AssertValid() const
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	CFormView::AssertValid();
}

void CPersonalityDlg::Dump(CDumpContext& dc) const
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	CFormView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CPersonalityDlg message handlers

BOOL CPersonalityDlg::Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext) 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	// TODO: Add your specialized code here and/or call the base class
	
	return CFormView::Create(lpszClassName, lpszWindowName, dwStyle, rect, pParentWnd, nID, pContext);
}

