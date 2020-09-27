// oledlg.h : header file
//

#include "seqdlg.h"

/////////////////////////////////////////////////////////////////////////////
// COleDlg dialog

class COleDlg : public CSeqDlg
{
// Construction
public:
	COleDlg(CWnd* pParent = NULL);	// standard constructor
	virtual void WriteValues();
	virtual UINT ChooseBitmap();

// Dialog Data
	//{{AFX_DATA(COleDlg)
	enum { IDD = IDD_OLE_OPTIONS };
	CButton	m_btnActiveDocContainer;
	CButton	m_btnActiveDocServer;
	BOOL	m_bActiveDocServer;
	int		m_nOleType;
	int		m_nCompFile;
	BOOL	m_bAutomation;
	BOOL	m_bOcx;
	BOOL	m_bActiveDocContainer;
	//}}AFX_DATA

	virtual BOOL CanDestroy();

// Implementation
protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	void EnableOcx(BOOL bEnable);
	void ReadValues();

	int m_nLastCompFile;
	BOOL	m_bActiveDocServerSave;	// valid when the btn is disabled
	BOOL	m_bActiveDocContainerSave;	// valid when the btn is disabled
	void UpdateActiveDocServer(BOOL fInitDialog);
	void UpdateCompFile(BOOL bCalledFromInit = FALSE);

	// Generated message map functions
	//{{AFX_MSG(COleDlg)
	virtual BOOL OnInitDialog();
	afx_msg BOOL OnClickedRadio(UINT nID);	
	afx_msg void OnClickedDocServer();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
