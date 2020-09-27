

#include "plist.h"

///////////////////////////////////////////////////////////////////////////////
// CMapRemoteDllDlg class
// definition for remote-to-local name mapping dialog
///////////////////////////////////////////////////////////////////////////////

class CMapRemoteDllDlg : public C3dDialog
{
// Construction
public:
	CMapRemoteDllDlg (LPCTSTR szRemote, CWnd * pParent = NULL);

// Dialog Data
	//{{AFX_DATA(CCallingExeDlg)
	enum { IDD = IDD_REMOTE_FILE_MAP };
	//}}AFX_DATA

// Implementation
protected:
	DECLARE_MESSAGE_MAP()

public:
	//{{AFX_MSG(CCallingExeDlg)
	virtual void OnOK();
	virtual void OnCancel();
	virtual BOOL OnInitDialog();
	void OnNameChange();
	void OnBrowse();
	void OnPromptDLLs();
	//}}AFX_MSG

	CString m_strLocal;
	BOOL	m_fPrompt;

private:
	CString m_strRemote;
	void UpdateOKState();
};


/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
// CAssertFailedDlg dialog

class CAssertFailedDlg : public C3dDialog
{
// Construction
public:
	CAssertFailedDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CAssertFailedDlg)
	enum { IDD = IDD_ASSERT_FAILED };
	CString	m_strAssertText;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAssertFailedDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CAssertFailedDlg)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};



//
// Attach To Active Dialog

class CAttachToActive : public C3dDialog
{

  private:

	TASK_LIST*	m_TaskList;
	int			m_nTasks;
	

  public:

	enum { IDD = IDD_ATTACH_TO_ACTIVE };

	ULONG	m_ProcessId;
	CString	m_ProcessName;
	CString	m_ImageName;
	

  public:	// operations

	CAttachToActive(
		CWnd*	pParent = NULL
		);
	
	~CAttachToActive(
		);
		

  protected:
  
	virtual void
	DoDataExchange(
		CDataExchange*	pDX
		);
		
	virtual BOOL
	OnInitDialog(
		);
		
	virtual void
	OnOK(
		);

	virtual BOOL
	OnNotify(
		WPARAM		wParam,
		LPARAM		lParam,
		LRESULT*	lResult
		);

	BOOL
	OnLvnColumnClick(
		NM_LISTVIEW*	ListData,
		LRESULT*		lResult
		);

	BOOL
	OnNmDblClk(
		NMHDR*	Data,
		LRESULT*	lResult
		);

	void
	EmptyProcessList(
		);

	void
	FillProcessList(
		BOOL	fListSystemProcesses
		);

	void
	OnChangeSystemProcess(
		);
	
	DECLARE_MESSAGE_MAP()
};

class CModuleDialog : public C3dDialog
{
  private:
  	LPMODULE_LIST m_pModuleList;

  public:

	enum { IDD = IDD_MODULELIST };


  public:	// operations

	CModuleDialog (
		CWnd*	pParent = NULL
		);
	
	~CModuleDialog (
		);
		

  protected:
  
	virtual void
	DoDataExchange(
		CDataExchange*	pDX
		);
		
	virtual BOOL
	OnInitDialog(
		);
		
	virtual void
	OnOK(
		);

	virtual BOOL
	OnNotify(
		WPARAM		wParam,
		LPARAM		lParam,
		LRESULT*	lResult
		);

	BOOL
	OnLvnColumnClick(
		NM_LISTVIEW*	ListData,
		LRESULT*		lResult
		);

	BOOL
	OnNmDblClk(
		NMHDR*	Data,
		LRESULT*	lResult
		);

	void
	EmptyModuleList(
		);

	void
	FillModuleList(
		);

	void
	OnChangeModule(
		);

	DECLARE_MESSAGE_MAP()
};


class CNoSymbolInfoDlg	: public C3dDialog
{

  public:

	BOOL	m_fNoPrompt;
	CString	m_strCaption;
	CString	m_strPrompt;

  public:

    enum { IDD = IDD_NO_SYMBOL_INFO };

	CNoSymbolInfoDlg(
		CWnd*	pParent = NULL
		);


  protected:
  
	virtual BOOL
	OnInitDialog(
		);

	virtual void
	DoDataExchange(
		CDataExchange*	pDX
		);

	DECLARE_MESSAGE_MAP ()
};

//
//  Fiber dialog
//

typedef struct _FBRLST {
	LPVOID	FbrCntx;
	CString	strFbr;
} FBRLST, *PFBRLST;

class CFibers : public C3dDialog
{

  private:

	FBRLST *m_FbrLst;
	DWORD  iFbrCnt;

  public:

	enum { IDD = IDD_FIBERS };	

	LPVOID m_FbrCntx;

  public:	// operations

	CFibers(
		CWnd*	pParent = NULL
		);
	
	~CFibers(
		);
		

  protected:
  
	virtual void
	DoDataExchange(
		CDataExchange*	pDX
		);
		
	virtual BOOL
	OnInitDialog(
		);
		
	virtual void
	OnOK(
		);

	virtual BOOL
	OnNotify(
		WPARAM		wParam,
		LPARAM		lParam,
		LRESULT*	lResult
		);

	BOOL
	OnLvnColumnClick(
		NM_LISTVIEW*	ListData,
		LRESULT*		lResult
		);
	
	DECLARE_MESSAGE_MAP()
};
