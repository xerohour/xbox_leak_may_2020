// msgdoc.h : header file
//

#define SRCHFLAG_MESSAGE_USEHANDLE		0x01
#define SRCHFLAG_MESSAGE_USEMSG			0x02
#define SRCHFLAG_MESSAGE_USEPOSTTYPE	0x04

/////////////////////////////////////////////////////////////////////////////
// CMsgDoc document

class CMsgView;

class CMsgDoc : public CDocument
{
	DECLARE_DYNCREATE(CMsgDoc)

protected:
	CMsgDoc();		  // protected constructor used by dynamic creation

// Attributes
public:

// Operations
public:
	void LogMsg(PMSGSTREAMDATA pmsd);
	void SetWindowToSpyOn(HWND hwnd);
	void SetThreadToSpyOn(DWORD tid);
	void SetProcessToSpyOn(DWORD pid);
	void SetNothingToSpyOn();
	int   GetObjectTypeSpyingOn()
	{
		return(m_nObjectType);
	}
	HWND  GetWindowSpyingOn()
	{
		if (GetObjectTypeSpyingOn() == OT_WINDOW)
			return(m_hwndToSpyOn);
		else
			return(NULL);
	}
	DWORD GetThreadSpyingOn()
	{
		if (GetObjectTypeSpyingOn() == OT_THREAD)
			return(m_tidToSpyOn);
		else
			return(0);
	}
	DWORD GetProcessSpyingOn()
	{
		if (GetObjectTypeSpyingOn() == OT_PROCESS)
			return(m_pidToSpyOn);
		else
			return(0);
	}
	BOOL IsSomethingSelected()
	{
		return (m_nObjectType != OT_NONE || m_fAllWindows);
	}
	BOOL IsLogging()
	{
		return m_fLogging;
	}
	void StartLogging()
	{
		if (!m_fLogging)
			OnMessagesStartStop();
	}
	void StopLogging()
	{
		if (m_fLogging)
			OnMessagesStartStop();
	}
	int ShowWindowFinder()
	{
		return ShowMessagesOptions(0);
	}
	BOOL IsLoggingToFile()
	{
		return m_fLogToFile;
	}
	BOOL IsAppendingToLogFile()
	{
		return m_fAppendFile;
	}
	CString GetLogFileName()
	{
		return m_strLogFileName;
	}

	BOOL DoFind();
	BOOL DoFindNext();
	BOOL DoFindPrev();
	BOOL HasValidFind();
	BOOL CompareLine(LINE *pLine);
	BOOL FindFirstMatchingLine();
	BOOL FindNextMatchingLine();
	BOOL FindPreviousMatchingLine();
	BOOL FindLastMatchingLine();
	


	BOOL MinBuildFriendFix(){return TRUE;}
// Implementation
protected:
	// data variables use in searching
	WORD	m_wSearchFlags;
	HWND 	m_hwndSelected;
	WORD 	m_wMsgSelected;
	UINT	m_uTypeSelected;
	BOOL	m_fSearchUp;	// TRUE if searching up, FALSE if searching down (from dialog)

	virtual ~CMsgDoc();
	virtual BOOL OnNewDocument();

	BOOL IsFilteredHwnd(HWND hwnd);
	void PrintMsg(MSGTYPE mt, PMSGSTREAMDATA pmsd);
	void UpdateTitle();
	int ShowMessagesOptions(UINT iInitialTab);

	inline void SetMaxLines(int cLinesMax);
	inline int GetMaxLines();
	inline BOOL IsSameThreadWindow(HWND hwnd);
	inline BOOL IsSameProcessWindow(HWND hwnd);

	int m_nObjectType;
	HWND m_hwndToSpyOn;
	DWORD m_tidToSpyOn;
	DWORD m_pidToSpyOn;

	CMsgView *m_pMsgView;
	BOOL m_fLogging;
	BOOL m_fMsgsUser;
	BOOL m_fMsgsRegistered;
	BOOL m_fMsgsUndocumented;
	BOOL m_fAllWindows;
	BOOL m_fIncludeParent;
	BOOL m_fIncludeChildren;
	BOOL m_fIncludeProcess;
	BOOL m_fIncludeThread;
	BOOL m_fShowNestingLevel;
	BOOL m_fShowRawParms;
	BOOL m_fShowDecodedParms;
	BOOL m_fShowRawReturn;
	BOOL m_fShowDecodedReturn;
	BOOL m_fShowOriginTime;
	BOOL m_fShowMousePosition;
	BYTE m_afSelected[MAX_MESSAGES];			// table of messages selected for logging
	BYTE m_afDlgSelected[MAX_DLG_MESSAGES];		// table of messages selected for logging (for dialogs)
#ifndef DISABLE_WIN95_MESSAGES
	BYTE m_afAniSelected[MAX_ANI_MESSAGES];		// table of messages selected for logging (for animate Chicago control)
	BYTE m_afHdrSelected[MAX_HDR_MESSAGES];		// table of messages selected for logging (for header Chicago control)
	BYTE m_afTBSelected[MAX_TB_MESSAGES];		// table of messages selected for logging (for toolbar Chicago control)
	BYTE m_afTTSelected[MAX_TT_MESSAGES];		// table of messages selected for logging (for tooltip Chicago control)
	BYTE m_afStatSelected[MAX_STAT_MESSAGES];	// table of messages selected for logging (for statusbar Chicago control)
	BYTE m_afTrkSelected[MAX_TRK_MESSAGES];		// table of messages selected for logging (for trackbar Chicago control)
	BYTE m_afUpDnSelected[MAX_UD_MESSAGES];		// table of messages selected for logging (for undown Chicago control)
	BYTE m_afProgSelected[MAX_PROG_MESSAGES];	// table of messages selected for logging (for progress Chicago control)
	BYTE m_afHKSelected[MAX_HK_MESSAGES];		// table of messages selected for logging (for hotkey Chicago control)
	BYTE m_afLVSelected[MAX_LV_MESSAGES];		// table of messages selected for logging (for listview Chicago control)
	BYTE m_afTVSelected[MAX_TV_MESSAGES];		// table of messages selected for logging (for treeview Chicago control)
	BYTE m_afTabSelected[MAX_TAB_MESSAGES];		// table of messages selected for logging (for tab Chicago control)
#endif	// DISABLE_WIN95_MESSAGES
	BOOL m_fLogToFile;
	CString m_strLogFileName;
	BOOL m_fAppendFile;
	HANDLE m_hLoggingFile;
	CMsgStream m_mout;

	friend class CMsgView;

	friend class CFiltersWindowDlgTab;
	friend class CFiltersMessageDlgTab;
	friend class CFiltersOutputDlgTab;

	friend class CSearchMessageDlg;

public:
	static void InitStaticData(HKEY hkey);
	static void SetTestMode(BOOL fTestMode) { m_fTestMode = fTestMode; }
	static void EnableHook(BOOL fEnable, BOOL fForce = FALSE);
	static void EnableSubclass(BOOL fEnable, BOOL fForce = FALSE);
	static void SaveSettings(HKEY hkey);

	static int m_iInitialTab;				// initial tab for Messages.Options tabbed dialog

	static PMSGDESC m_apmdLT[MAX_MESSAGES];	// Message lookup table
	static MSGDESC m_aMsgDesc[];			// Msg description table
	static int m_cMsgs;						// Msgs in msg desc table

	static PMSGDESC m_apmdDlgLT[MAX_DLG_MESSAGES];	// Message lookup table (for dialog box messages > WM_USER)
#ifndef DISABLE_WIN95_MESSAGES
	static PMSGDESC m_apmdAniLT[MAX_ANI_MESSAGES];	// Message lookup table (for animate Chicago control messages > WM_USER)
	static PMSGDESC m_apmdHdrLT[MAX_HDR_MESSAGES];	// Message lookup table (for header Chicago control messages > WM_USER)
	static PMSGDESC m_apmdTBLT[MAX_TB_MESSAGES];	// Message lookup table (for toolbar Chicago control messages > WM_USER)
	static PMSGDESC m_apmdTTLT[MAX_TT_MESSAGES];	// Message lookup table (for tooltip Chicago control messages > WM_USER)
	static PMSGDESC m_apmdStatLT[MAX_STAT_MESSAGES];// Message lookup table (for statusbar Chicago control messages > WM_USER)
	static PMSGDESC m_apmdTrkLT[MAX_TRK_MESSAGES];	// Message lookup table (for trackbar Chicago control messages > WM_USER)
	static PMSGDESC m_apmdUpDnLT[MAX_UD_MESSAGES];	// Message lookup table (for undown Chicago control messages > WM_USER)
	static PMSGDESC m_apmdProgLT[MAX_PROG_MESSAGES];// Message lookup table (for progress Chicago control messages > WM_USER)
	static PMSGDESC m_apmdHKLT[MAX_HK_MESSAGES];	// Message lookup table (for hotkey Chicago control messages > WM_USER)
	static PMSGDESC m_apmdLVLT[MAX_LV_MESSAGES];	// Message lookup table (for listview Chicago control messages > WM_USER)
	static PMSGDESC m_apmdTVLT[MAX_TV_MESSAGES];	// Message lookup table (for treeview Chicago control messages > WM_USER)
	static PMSGDESC m_apmdTabLT[MAX_TAB_MESSAGES];	// Message lookup table (for tab Chicago control messages > WM_USER)
#endif	// DISABLE_WIN95_MESSAGES

private:
	enum REGFLAG
	{
		REGFLAG_MSGFILTERS_USER				= 0x00000001,
		REGFLAG_MSGFILTERS_REGISTERED		= 0x00000002,
		REGFLAG_MSGFILTERS_UNDOCUMENTED		= 0x00000004,

		REGFLAG_WINFILTERS_INCPARENT		= 0x00000001,
		REGFLAG_WINFILTERS_INCCHILDREN		= 0x00000002,
		REGFLAG_WINFILTERS_INCPROCESS		= 0x00000004,
		REGFLAG_WINFILTERS_INCTHREAD		= 0x00000008,

		REGFLAG_OUTPUTFILTERS_NESTLEVEL		= 0x00000001,
		REGFLAG_OUTPUTFILTERS_RAWPARM		= 0x00000002,
		REGFLAG_OUTPUTFILTERS_DECODEPARM	= 0x00000004,
		REGFLAG_OUTPUTFILTERS_RAWRET		= 0x00000008,
		REGFLAG_OUTPUTFILTERS_DECODERET		= 0x00000010,
		REGFLAG_OUTPUTFILTERS_SHOWTIME 		= 0x00000020,
		REGFLAG_OUTPUTFILTERS_SHOWMOUSEPOS	= 0x00000040,
		REGFLAG_OUTPUTFILTERS_LOGTOFILE   	= 0x00000080,
		REGFLAG_OUTPUTFILTERS_APPENDFILE   	= 0x00000100
	};

	static int m_nLogging;					// Count of logging views
	static BOOL m_fTestMode;				// TRUE if in test mode
	static BYTE m_BitTable[];

	static BOOL m_fMsgsUserDef;
	static BOOL m_fMsgsRegisteredDef;
	static BOOL m_fMsgsUndocumentedDef;
	static BYTE m_afSelectedDef[];			// Default selected table
	static BYTE m_afDlgSelectedDef[];		// Default selected table (for dialog messages)
#ifndef DISABLE_WIN95_MESSAGES
	static BYTE m_afAniSelectedDef[];		// Default selected table (for animate Chicago control)
	static BYTE m_afBLSelectedDef[];		// Default selected table (for button-list Chicago control)
	static BYTE m_afHdrSelectedDef[];		// Default selected table (for header Chicago control)
	static BYTE m_afTBSelectedDef[];		// Default selected table (for toolbar Chicago control)
	static BYTE m_afTTSelectedDef[];		// Default selected table (for tooltip Chicago control)
	static BYTE m_afStatSelectedDef[];		// Default selected table (for statusbar Chicago control)
	static BYTE m_afTrkSelectedDef[];		// Default selected table (for trackbar Chicago control)
	static BYTE m_afUpDnSelectedDef[];		// Default selected table (for undown Chicago control)
	static BYTE m_afProgSelectedDef[];		// Default selected table (for progress Chicago control)
	static BYTE m_afHKSelectedDef[];		// Default selected table (for hotkey Chicago control)
	static BYTE m_afLVSelectedDef[];		// Default selected table (for listview Chicago control)
	static BYTE m_afTVSelectedDef[];		// Default selected table (for treeview Chicago control)
	static BYTE m_afTabSelectedDef[];		// Default selected table (for tab Chicago control)
#endif	// DISABLE_WIN95_MESSAGES
	static BOOL m_fIncludeParentDef;
	static BOOL m_fIncludeChildrenDef;
	static BOOL m_fIncludeProcessDef;
	static BOOL m_fIncludeThreadDef;
	static BOOL m_fShowNestingLevelDef;
	static BOOL m_fShowRawParmsDef;
	static BOOL m_fShowDecodedParmsDef;
	static BOOL m_fShowRawReturnDef;
	static BOOL m_fShowDecodedReturnDef;
	static BOOL m_fShowOriginTimeDef;
	static BOOL m_fShowMousePositionDef;
	static BOOL m_fLogToFileDef;
	static CString m_strLogFileNameDef;
	static BOOL m_fAppendFileDef;
	static int m_cLinesMaxDef;
	static int m_cShowReturnUseCount;

	static CHAR m_szSpyKey[];
	static CHAR m_szKeyMsgFilters[];
	static CHAR m_szKeyMsgFiltersMask[];
	static CHAR m_szKeyDlgMsgFiltersMask[];
#ifndef DISABLE_WIN95_MESSAGES
	static CHAR m_szKeyAniMsgFiltersMask[];
	static CHAR m_szKeyBLMsgFiltersMask[];
	static CHAR m_szKeyHdrMsgFiltersMask[];
	static CHAR m_szKeyTBMsgFiltersMask[];
	static CHAR m_szKeyTTMsgFiltersMask[];
	static CHAR m_szKeyStatMsgFiltersMask[];
	static CHAR m_szKeyTrkMsgFiltersMask[];
	static CHAR m_szKeyUpDnMsgFiltersMask[];
	static CHAR m_szKeyProgMsgFiltersMask[];
	static CHAR m_szKeyHKMsgFiltersMask[];
	static CHAR m_szKeyLVMsgFiltersMask[];
	static CHAR m_szKeyTVMsgFiltersMask[];
	static CHAR m_szKeyTabMsgFiltersMask[];
#endif	// DISABLE_WIN95_MESSAGES
	static CHAR m_szKeyWinFilters[];
	static CHAR m_szKeyOutputFilters[];
	static CHAR m_szKeyOutputLogFile[];
	static CHAR m_szKeyLines[];

	// Generated message map functions
protected:
	//{{AFX_MSG(CMsgDoc)
	afx_msg void OnMessagesStartStop();
	afx_msg void OnUpdateMessagesStartStop(CCmdUI* pCmdUI);
	afx_msg void OnMessagesOptions();
	afx_msg void OnUpdateMessagesOptions(CCmdUI* pCmdUI);
	afx_msg void OnMessagesClear();
	afx_msg void OnUpdateMessagesClear(CCmdUI* pCmdUI);
	afx_msg void OnSearchFind();
	afx_msg void OnUpdateSearchFind(CCmdUI* pCmdUI);
	afx_msg void OnSearchFindNext();
	afx_msg void OnUpdateSearchFindNext(CCmdUI* pCmdUI);
	afx_msg void OnSearchFindPrev();
	afx_msg void OnUpdateSearchFindPrev(CCmdUI* pCmdUI);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


//-----------------------------------------------------------------------------
// CMsgStream::IsSameThreadWindow
//
// Determines if a given hwnd belongs to the same thread as the hwnd
// that is being spied on.
//
// Arguments:
//
//  HWND hwnd - The window to check.
//
// Returns:
//  Non-zero if the window belongs to the same thread as the window that is
//  being spied on, zero otherwise.
//
//-----------------------------------------------------------------------------

inline BOOL CMsgDoc::IsSameThreadWindow(HWND hwnd)
{
	return (GetWindowThreadProcessId(hwnd, NULL) == m_tidToSpyOn);
}


//-----------------------------------------------------------------------------
// CMsgStream::IsSameProcessWindow
//
// Determines if a given hwnd belongs to the same process as the hwnd
// that is being spied on.
//
// Arguments:
//
//  HWND hwnd - The window to check.
//
// Returns:
//  Non-zero if the window belongs to the same process as the window that is
//  being spied on, zero otherwise.
//
//-----------------------------------------------------------------------------

inline BOOL CMsgDoc::IsSameProcessWindow(HWND hwnd)
{
	DWORD pid;

	GetWindowThreadProcessId(hwnd, &pid);

	return (pid == m_pidToSpyOn);
}


inline void CMsgDoc::SetMaxLines(int cLinesMax)
{
	m_pMsgView->SetMaxLines(cLinesMax);
}


inline int CMsgDoc::GetMaxLines()
{
	return m_pMsgView->GetMaxLines();
}

/////////////////////////////////////////////////////////////////////////////
