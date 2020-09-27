// miscdlgs.h : header file
//
#include "datbdlg.h"  // <-- declaration of CMyButton
#include "seqdlg.h"

/////////////////////////////////////////////////////////////////////////////
// CDocAppOptionsDlg dialog

class CDocAppOptionsDlg : public CSeqDlg
{
// Construction
public:
	CDocAppOptionsDlg(CWnd* pParent = NULL);	// standard constructor
	virtual void WriteValues();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual UINT ChooseBitmap();
	virtual UINT ChooseBitmap2();
	virtual BOOL ShowTwoBitmaps() { return TRUE; }
	virtual BOOL CanDestroy();
	
// Dialog Data
	//{{AFX_DATA(CDocAppOptionsDlg)
	enum { IDD = IDD_DOCAPP_OPTIONS };
	UINT	m_nSizeMRU;
	int		m_nMenus;
	//}}AFX_DATA

// Implementation
protected:
	CMyButton m_btnAdvanced;
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	void ReadValues();         // Reads from projectOptions
	void EnablePrint(BOOL bEnable);
	
	// Generated message map functions
	//{{AFX_MSG(CDocAppOptionsDlg)
	virtual BOOL OnInitDialog();
//	afx_msg void OnDocStrings();
//	afx_msg void OnFrameStyles();
	afx_msg void OnAdvanced();
	afx_msg BOOL OnClickedCheckbox(UINT nID);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
/////////////////////////////////////////////////////////////////////////////
// CDlgAppOptionsDlg dialog

class CDlgAppOptionsDlg : public CSeqDlg
{
// Construction
public:
	CDlgAppOptionsDlg(CWnd* pParent = NULL);	// standard constructor
	virtual void WriteValues();
    virtual BOOL CanDestroy();
	virtual UINT ChooseBitmap();
	virtual UINT ChooseBitmap2();
	virtual BOOL ShowTwoBitmaps() { return TRUE; }

// Dialog Data
	//{{AFX_DATA(CDlgAppOptionsDlg)
	enum { IDD = IDD_DLGAPP_OPTIONS };
	BOOL	m_bOleAutomation;
	BOOL	m_bOcx;
	//}}AFX_DATA

// Implementation
protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	void ReadValues();         // Reads from projectOptions
	void GetTitle(CString& strTitle);
	afx_msg BOOL OnClickedCheckbox(UINT nID);
	void EnableVarious(BOOL bEnableWOSA, BOOL bEnableOCX);
	
	// Generated message map functions
	//{{AFX_MSG(CDlgAppOptionsDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnDestroy();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
/////////////////////////////////////////////////////////////////////////////
// CProjOptionsDlg dialog

class CProjOptionsDlg : public CSeqDlg
{
// Construction
public:
	CProjOptionsDlg(UINT nID, CWnd* pParent);
	CProjOptionsDlg(CWnd* pParent = NULL);	// standard constructor
	virtual void WriteValues();
	virtual UINT ChooseBitmap();
	virtual BOOL CanDestroy();
	
// Dialog Data
	//{{AFX_DATA(CProjOptionsDlg)
	enum { IDD = IDD_PROJ_OPTIONS };
	int		m_nAtlSupport;
	int		m_nProjStyle;
	//}}AFX_DATA

// Implementation
protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	virtual void ReadValues();         // Reads from projectOptions

	// Generated message map functions
	//{{AFX_MSG(CProjOptionsDlg)
	virtual BOOL OnInitDialog();
	afx_msg BOOL OnClickedRadio(UINT nID);		
	afx_msg void OnDestroy();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
// CDllProjOptionsDlg dialog

// This enum order must match tab order of DLL Type buttons
enum {DT_REGSTATIC, DT_REGSHARED, DT_EXT};

class CDllProjOptionsDlg : public CProjOptionsDlg
{
// Construction
public:
	CDllProjOptionsDlg(CWnd* pParent = NULL);   // standard constructor
	virtual void WriteValues();

// Dialog Data
	//{{AFX_DATA(CDllProjOptionsDlg)
	enum { IDD = IDD_DLLPROJ_OPTIONS };
	int		m_nDllType;
	int		m_nVerbose;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDllProjOptionsDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//void EnableUsrdllOptions(BOOL bEnable);
	virtual void ReadValues();         // Reads from projectOptions
	void EnableSockets(BOOL bEnable);
	
	// Generated message map functions
	//{{AFX_MSG(CDllProjOptionsDlg)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
