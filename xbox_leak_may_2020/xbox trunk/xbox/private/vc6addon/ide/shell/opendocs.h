// opendocs.h : header file
//

#include "resource.h"

/////////////////////////////////////////////////////////////////////////////
// COpenDocsDlg dialog

class COpenDocsDlg : public C3dDialog
{
// Construction
public:
	COpenDocsDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(COpenDocsDlg)
	enum { IDD = IDD_OPEN_DOCUMENTS };
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(COpenDocsDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(COpenDocsDlg)
	afx_msg void OnOpenDocumentsCloseDoc();
	afx_msg void OnOpenDocumentsActivate();
#ifdef PRINT_DOC_LIST
	afx_msg void OnOpenDocumentsPrint();
#endif
	afx_msg void OnOpenDocumentsSave();
	afx_msg void OnOpenDocumentsTileHorz();
	afx_msg void OnOpenDocumentsTileVert();
	afx_msg void OnSelChangeDocList();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	void FillListBox(void);
	void UpdateUI(void);
	BOOL IsViewPrintable(CPartView* pView);
	BOOL IsFrameSaveable(CPartFrame* pFrame);
	void SelectActiveChild(void);
	// minimizes all windows that are not selected. Used before tiling. If true is returned, the tile should
	// happen. If false, there were too few selections
	BOOL MinimizeUnselected(void);

private:
	BOOL m_bPrintable;
	BOOL m_bSaveable;
};
