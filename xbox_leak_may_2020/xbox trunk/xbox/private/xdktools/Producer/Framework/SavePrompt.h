#ifndef __SAVEPROMPT_H__
#define __SAVEPROMPT_H__

// SavePrompt.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CSavePromptDlg class used by save prompt dialog

class CSavePromptDlg : public CDialog
{
public:
	CSavePromptDlg( CString* pstrPrompt );

// Dialog Data
	//{{AFX_DATA(CSavePromptDlg)
	enum { IDD = IDD_SAVEPROMPTBOX };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSavePromptDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	CString m_strPrompt;

// Implementation
protected:
	//{{AFX_MSG(CSavePromptDlg)
	afx_msg void OnNo();
	afx_msg void OnYes();
	virtual BOOL OnInitDialog();
	afx_msg void OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct);
	afx_msg void OnYesAll();
	afx_msg void OnNoAll();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


#endif // __SAVEPROMPT_H__
