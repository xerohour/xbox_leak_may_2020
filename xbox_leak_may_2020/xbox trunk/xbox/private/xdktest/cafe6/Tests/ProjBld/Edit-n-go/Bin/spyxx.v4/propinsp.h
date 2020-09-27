// propinsp.h : header file
//


/////////////////////////////////////////////////////////////////////////////
// CPropertyInspector window

class CPropertyInspector : public CTabbedDialog
{
// Construction
public:
	CPropertyInspector(UINT nIDCaption, CWnd* pParentWnd = NULL, UINT iSelectTab = -1, COMMIT_MODEL commitModel = commitOnTheFly);
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

// Implementation
public:
	virtual ~CPropertyInspector();

	void SetValidObjectFlag(BOOL fValidObject)
	{
		m_fValidObject = fValidObject;
	}
	BOOL GetValidObjectFlag()
	{
		return m_fValidObject;
	}

	BOOL SetObject(DWORD dwObject, int nObjectType);
	void ChangeObject(DWORD dwObject, int nObjectType);

	static void InitializeTables();

protected:
	//
	// Modeless support.
	//
	virtual void PostNcDestroy()
	{
		delete this;
	}

	virtual void CreateButtons();
	virtual CButton* GetButtonFromIndex(int index);

	void FillStrListFromTable(CStringList* pStrList, DWORD *flStyle, VALUETABLE *pvt, BOOL fRemoveAll = TRUE,
					BOOL fOneEntry = FALSE, BOOL fPrintExtras = FALSE);
	void AppendStrListFromButtonTable(CStringList* pStrList, DWORD flStyle, VALUETABLE *pvt);
	void AppendStrListFromScrollTable(CStringList* pStrList, DWORD flStyle, VALUETABLE *pvt);
	void AppendStrListFromEditTable(CStringList* pStrList, DWORD flStyle, VALUETABLE *pvt);
	void AppendStrListFromTrackBarTable(CStringList* pStrList, DWORD flStyle, VALUETABLE *pvt);
	void AppendStrListFromListViewTable(CStringList* pStrList, DWORD flStyle, VALUETABLE *pvt);
	void AppendStrListFromTabControlTable(CStringList* pStrList, DWORD flStyle, VALUETABLE *pvt);

	BOOL SetObjectType(int nObjectType);
	BOOL SetWindowObject(HWND hwnd);
	BOOL SetProcessObject(DWORD tid);
	BOOL SetThreadObject(DWORD tid);
	BOOL SetMessageObject(PMSGSTREAMDATA2 pmsd2);
	BOOL SetNoObject();

	void OnRefresh();
	BOOL RefreshWindowObjectData();
	BOOL RefreshProcessObjectData();
	void GetTimesForProcess();
	BOOL RefreshThreadObjectData();
	void GetTimesForThread();
	BOOL RefreshMessageObjectData();

public:
	static void ShowObjectProperties(DWORD dwObject, int nObjectType);

protected:
	static VALUETABLE m_astClass[];
	static VALUETABLE m_astColor[];
	static HANDLESTABLE m_ahtSystemCursors[];
	static HANDLESTABLE m_ahtSystemIcons[];

protected:
	CButton m_btnRefresh;
	CStatic m_StaticMessage;

	int m_nObjectType;
	DWORD m_dwObject;
	BOOL m_fValidObject;
	BOOL m_fObjectTypeChanged;

	//
	// Data variables for when a window is selected:
	//
	HWND m_hwnd;
	CString m_strWindowCaption;
	CString m_strRectangle;
	CString m_strRestoredRect;
	CString m_strClientRect;
	DWORD m_dwID;
	DWORD m_dwUserData;
	CStringList m_strListExtraBytes;

	DWORD m_flStyles;
	DWORD m_flExtStyles;
	CStringList m_strListStyles;
	CStringList m_strListExtStyles;

	HWND m_hwndNext;
	HWND m_hwndPrevious;
	HWND m_hwndParent;
	HWND m_hwndChild;
	HWND m_hwndOwner;

	CString m_strClassName;
	CString m_strClassAtom;
	CString m_strClassStyles;
	CStringList m_strListClassStyles;
	CString m_strClassWndExtraBytes;
	CString m_strClassExtraBytes;
	CStringList m_strListClassExtraBytes;
	CString m_strClassInstance;
	CString m_strClassWndProc;
	CString m_strClassMenuName;
	CString m_strClassIcon;
	CString m_strClassCursor;
	CString m_strClassBrush;

	DWORD m_ProcessID;
	DWORD m_ThreadID;
	BOOL m_fValidProcessThreadID;
	HINSTANCE m_hInstance;
	CString m_strWndProc;

	CString m_strExecutable;
	//
	// Data variables for when a process is selected:
	//
	DWORD m_pid;
	// Win32 process data
	CPROCDB::PRFCNTRPROC m_ProcessInfo;
	CPROCDB::PRFCNTRPROCAS m_ProcessAddrSpcInfo;
	// Chicago process data
	PROCESSENTRY32	m_ProcessEntry;
	FILETIME		m_ftPrcCreationTime;
	FILETIME		m_ftPrcExitTime;
	FILETIME		m_ftPrcKernelTime;
	FILETIME		m_ftPrcUserTime;
	SYSTEMTIME		m_stPrcSnapshotTime;

	//
	// Data variables for when a thread is selected:
	//
	DWORD m_tid;
	// Win32 process data
	CPROCDB::PRFCNTRTHRD m_ThreadInfo;
	CPROCDB::PRFCNTRTHRDDET m_ThreadDetInfo;
	// Chicago thread data
	THREADENTRY32	m_ThreadEntry;
	FILETIME		m_ftThdCreationTime;
	FILETIME		m_ftThdExitTime;
	FILETIME		m_ftThdKernelTime;
	FILETIME		m_ftThdUserTime;
	SYSTEMTIME		m_stThdSnapshotTime;

	//
	// Data variables for when a message is selected:
	//
	MSGSTREAMDATA2 m_msd2;
	CString m_strMessage1;
	CString m_strMessage2;

	// Generated message map functions
protected:
	//{{AFX_MSG(CPropertyInspector)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	friend class CPropertyTab;

	friend class CWindowGeneralPropTab;
	friend class CWindowStylesPropTab;
	friend class CWindowWindowsPropTab;
	friend class CWindowClassPropTab;
	friend class CWindowProcessPropTab;

	friend class CProcessGeneralPropTab;
	friend class CProcessMemoryPropTab;
	friend class CProcessPageFilePropTab;
	friend class CProcessFileIOPropTab;
	friend class CProcessSpacePropTab;

	friend class CThreadGeneralPropTab;

	friend class CMessageGeneralPropTab;
};
