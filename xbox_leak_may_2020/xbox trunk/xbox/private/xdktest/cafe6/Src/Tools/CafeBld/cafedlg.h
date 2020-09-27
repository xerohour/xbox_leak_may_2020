// cafedlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CCAFEBuildDlg dialog

#define WM_UPDATEOUTPUT (WM_USER+1)
#define WM_BUILDCOMPLETE (WM_USER+2)

#define BASEREGISTRYKEY "Software\\Microsoft\\"
#define DEFAULTREGISTRYKEY "CAFE Build System"

class CCAFEBuildDlg : public CDialog
{
// Construction
public:
	CCAFEBuildDlg(CWnd* pParent = NULL);	// standard constructor
	~CCAFEBuildDlg();

// Dialog Data
	//{{AFX_DATA(CCAFEBuildDlg)
	enum { IDD = IDD_CAFEBLD_DIALOG };
	CEdit	m_editOutput;
	CListBox	m_listTargets;
	BOOL	m_bBrowse;
	BOOL	m_bClean;
	int 	m_iDebug;
	BOOL	m_bDepend;
	BOOL	m_bSLMIn;
	BOOL	m_bSLMOut;
	BOOL	m_bSsync;
	BOOL	m_bBuild;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCAFEBuildDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Data
protected:
	static char* m_szTargets[][2];

	// pointer to the read pipe thread
	CWinThread* m_pthreadReadPipe;

	// pipe handles
	HANDLE m_hpipeRead;
	HANDLE m_hpipeWrite;
	HANDLE m_hpipeWriteErr;

	// process information structure for spawning nmake
	PROCESS_INFORMATION m_procinfo;

	// event to signal vcspawn if we want to terminate the build
	HANDLE m_heventTerminate;

	// flag to indicate whether we're building (1), getting ready to build (2) or not (0)
	int m_nBuilding;

	// flag to indicate if build was cancelled or not
	BOOL m_bCancelled;

	// string that's filled in by the read pipe thread
	CString m_strOutput;

	// storage for settings data
	CString m_strCAFEDIR;
	CString m_strSNIFFDIR;
	CString m_strIDESRCDIR;
	CString m_strMakefile;
	CString m_strPATH;
	CString m_strINCLUDE;
	CString m_strLIB;

	// additional command line to pass to nmake
	CString m_strMoreCmdLine;

	// registry key name
	CString m_strRegistryKey;

// Internal Operations
protected:
	void ReadRegistry(void);
	void WriteRegistry(void);

	static UINT WrapReadPipe(LPVOID pParam);
	UINT ReadPipe(void);

	void UpdateButtons(void);

// Implementation
protected:
	HICON m_hIcon;

	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnBuild();
	afx_msg void OnCancelBuild();
	afx_msg void OnExit();
	afx_msg void OnSettings();
	//}}AFX_MSG
	//}}AFX_MSG
	afx_msg LONG OnUpdateOutput(UINT wParam, LONG lParam);
	afx_msg LONG OnBuildComplete(UINT wParam, LONG lParam);
	DECLARE_MESSAGE_MAP()
};
