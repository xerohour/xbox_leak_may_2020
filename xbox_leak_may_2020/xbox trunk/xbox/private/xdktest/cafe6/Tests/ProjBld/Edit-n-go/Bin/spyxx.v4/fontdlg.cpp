// fontdlg.cpp : implementation file
//

#include "stdafx.h"
#pragma hdrstop

#include "fontdlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CFontDlg dialog

void CFontDlg::DoDataExchange(CDataExchange* pDX)
{
	CFontDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CFontDlg)
	DDX_Check(pDX, IDC_FONT_SAVE, m_fSave);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CFontDlg, CFontDialog)
	//{{AFX_MSG_MAP(CFontDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
// CFontDlg dialog message handlers

void CFontDlg::OnOK()
{
	UpdateData();
	CFontDialog::OnOK();
}

/////////////////////////////////////////////////////////////////////////////

HFONT SpyFontDialog(HFONT hfontInitial)
{
	ASSERT(hfontInitial);

	LOGFONT lf;
	::GetObject(hfontInitial, sizeof(LOGFONT), &lf);
	CFontDlg dlg(&lf, CF_FORCEFONTEXIST | CF_SCREENFONTS | CF_ENABLETEMPLATE);

	dlg.m_cf.lpTemplateName = MAKEINTRESOURCE(CFontDlg::IDD);

	if (dlg.DoModal() == IDOK)
	{
		HFONT hfont = ::CreateFontIndirect(&lf);
		ASSERT(hfont);

		if (dlg.m_fSave)
		{
			::GetObject(hfont, sizeof(LOGFONT), &theApp.m_DefLogFont);
		}

		return hfont;
	}

	return NULL;
}
