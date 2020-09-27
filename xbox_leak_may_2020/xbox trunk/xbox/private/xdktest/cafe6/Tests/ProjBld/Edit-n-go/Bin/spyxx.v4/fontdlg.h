// fontdlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CFontDlg dialog

class CFontDlg : public CFontDialog
{
// Construction
public:
	CFontDlg(LPLOGFONT lplfInitial = NULL, DWORD dwFlags = CF_EFFECTS | CF_SCREENFONTS, CDC* pdcPrinter = NULL, CWnd* pParentWnd = NULL)
		: CFontDialog(lplfInitial, dwFlags, pdcPrinter, pParentWnd)
	{
		m_fSave = FALSE;
		m_nIDHelp = CFontDlg::IDD;
	}

// Dialog Data
	//{{AFX_DATA(CFontDlg)
	enum {IDD = IDD_FONT};
	BOOL	m_fSave;
	//}}AFX_DATA

// Implementation
protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

	// Generated message map functions
	//{{AFX_MSG(CFontDlg)
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


HFONT SpyFontDialog(HFONT hfontInitial);
