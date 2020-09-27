// spy.h : main header file for the SPY application
//

//#define WIN32_LEAN_AND_MEAN

#define OEM_JAPAN			932
#define OEM_PRC				936
#define OEM_TAIWAN			950
#define OEM_KOR_WANGSUNG	949
#define OEM_KOR_JOHAB		1361
#define OEM_US				437

//
// Chicago ToolHelp function typedefs
//
typedef HANDLE (WINAPI *SNAPSHOTPROC)(DWORD, DWORD);
typedef BOOL (WINAPI *HEAPLISTPROC)(HANDLE, LPHEAPLIST32);
typedef BOOL (WINAPI *HEAP32FIRSTPROC)(LPHEAPENTRY32, DWORD, DWORD);
typedef BOOL (WINAPI *HEAP32NEXTPROC)(LPHEAPENTRY32);
typedef BOOL (WINAPI *READPROCMEMPROC)(DWORD, LPCVOID, LPVOID, DWORD, LPDWORD);
typedef BOOL (WINAPI *PROCESSLISTPROC)(HANDLE, LPPROCESSENTRY32);
typedef BOOL (WINAPI *THREADLISTPROC)(HANDLE, LPTHREADENTRY32);
typedef BOOL (WINAPI *MODULELISTPROC)(HANDLE, LPMODULEENTRY32);

/////////////////////////////////////////////////////////////////////////////
// Standard font handling

enum FontType
{
	font_Normal,			// MS Sans Serif 8, MS ‚S‚V‚b‚N 10, System 10
	font_NormalBold,
	font_Small,				// Small Fonts (-9), Terminal (-9), Terminal (-9)
	font_SmallBold,
	font_Fixed,				// Courier (14), FixedSys (14), FixedSys (14)
	font_FixedBold,
	font_NormalUnderline,	// MS Sans Serif 8, MS ‚S‚V‚b‚N 10, System 10
	font_NormalUnderlineBold,
	font_count				// Not a font, just provides a count
};

CFont* GetStdFont(const int iType);

// NOTE: The LOGFONT returned by this is temporary
const LOGFONT* GetStdLogfont(const int iType, CDC *pDC = NULL);

/////////////////////////////////////////////////////////////////////////////
// CSpyApp:
// See spy.cpp for the implementation of this class
//

class CSpyApp : public CWinApp
{
public:
	CSpyApp();

	virtual void WinHelp(DWORD dwData, UINT nCmd = HELP_CONTEXT);
	virtual BOOL OnIdle(LONG lCount);

	void InvokeWinHelp(DWORD dwData, int helpType);

	BOOL IsChicago() {return(m_bIsChicago);}
	BOOL IsDaytona() {return(m_bIsDaytona);}

	int GetWinTabCur() {return(m_nWinTabCur);}
	int GetPrcTabCur() {return(m_nPrcTabCur);}
	int GetThdTabCur() {return(m_nThdTabCur);}
	int GetMsgTabCur() {return(m_nMsgTabCur);}
	void SetWinTabCur(int nTab) {m_nWinTabCur = nTab;}
	void SetPrcTabCur(int nTab) {m_nPrcTabCur = nTab;}
	void SetThdTabCur(int nTab) {m_nThdTabCur = nTab;}
	void SetMsgTabCur(int nTab) {m_nMsgTabCur = nTab;}

	HICON m_hiconApp;
	HCURSOR m_hcurFindTool;
	HICON m_hiconFindTool;
	HICON m_hiconFindTool2;
	CBrush m_BrushWindow;
	LOGFONT m_DefLogFont;
	BOOL m_fMainWndplValid;
	WINDOWPLACEMENT m_wndplMain;
	CMultiDocTemplate *m_pMsgDocTemplate;
	CMultiDocTemplate *m_pWndTreeDocTemplate;
	CMultiDocTemplate *m_pPrcTreeDocTemplate;
	CMultiDocTemplate *m_pThdTreeDocTemplate;

	BOOL m_bDidPerfQueries;
	CStringList m_strlistLogFiles;

	static BOOL m_bIsMDIMaximized;

// Chicago process/thread function pointers
	static HINSTANCE		hToolhelp32;

	static SNAPSHOTPROC		pfnCreateToolhelp32Snapshot;

	static HEAPLISTPROC		pfnHeap32ListFirst;
	static HEAPLISTPROC		pfnHeap32ListNext;
	static HEAP32FIRSTPROC	pfnHeap32First;
	static HEAP32NEXTPROC	pfnHeap32Next;
	static READPROCMEMPROC	pfnToolhelp32ReadProcessMemory;

	static PROCESSLISTPROC	pfnProcess32First;
	static PROCESSLISTPROC	pfnProcess32Next;

	static THREADLISTPROC	pfnThread32First;
	static THREADLISTPROC	pfnThread32Next;

	static MODULELISTPROC	pfnModule32First;
	static MODULELISTPROC	pfnModule32Next;

// Overrides
	virtual BOOL InitInstance();
	virtual int ExitInstance();

// Implementation
private:
	static CHAR m_szSpyKey[];
	static CHAR m_szKeyDefFont[];
	static CHAR m_szKeyMainPos[];
	static CHAR m_szKeyDecodeStructs[];
	static CHAR m_szMaximizeMDI[];
	static BOOL m_bIsChicago;
	static BOOL m_bIsDaytona;

	static int m_nWinTabCur;
	static int m_nPrcTabCur;
	static int m_nThdTabCur;
	static int m_nMsgTabCur;

	//{{AFX_MSG(CSpyApp)
	afx_msg void OnAppAbout();
	afx_msg void OnSpyMessages();
	afx_msg void OnSpyMessagesDirect();
	afx_msg void OnSpyHighlightWindow();
	afx_msg void OnSpyWindows();
	afx_msg void OnSpyProcesses();
	afx_msg void OnSpyThreads();
	afx_msg void OnSpyFindWindow();
	afx_msg void OnUpdateSpyProcesses(CCmdUI *pCmdUI);
	afx_msg void OnUpdateSpyThreads(CCmdUI *pCmdUI);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//
// Define this to enter a mode where Spy++ will only log one occurrence
// of each message.  It changes the settings to filter out each message
// after a single one of them is logged.  This allows us to more easily
// determine what messages are not getting logged and try to ensure
// that all of our message crackers are getting exercised.
//
//#define ONESHOTFILTER

#define MIN_REGISTEREDMSG			0xC000	// Minimum registered msg value.
#define MAXLINESDEFAULT				750		// Default max msg stream lines.
#define TIMEOUT_OTHERPROCESSDATA	10000	// OtherProcessData msg timeout

//
// Maximum number of lines to keep.  Note that if this value is changed,
// the text of the IDS_MSG_INVALIDMAXLINES message MUST be updated as well!
//
#define MAXLINESMAX					20000	// Maximum max msg stream lines.

//
// Message sent to the ProcessPacket window when a hook packet needs to be
// processed.  wParam is 0, lParam is a pointer to a MSGSTREAMDATA
// structure.
//
#define WM_USER_PROCESSPACKET		(WM_USER + 111)

//
// Message group types.  These are indices into the message group table
// array for the CMsgDoc object.  If these are changed, the table must
// be updated to match!
//
#define MT_MISC		0
#define MT_DDE		1
#define MT_CLIP		2
#define MT_MOUSE	3
#define MT_NC		4
#define MT_KEYBD	5
#define MT_BM		6
#define MT_CB		7
#define MT_EM		8
#define MT_LB		9
#define MT_STM		10
#define MT_MDI		11
#define MT_SBM		12
#define MT_IME		13
#define MT_DLG		14
#ifndef DISABLE_WIN95_MESSAGES
#define MT_ANI		15
#define MT_HDR		16
#define MT_HK		17
#define MT_LV		18
#define MT_PRG		19
#define MT_STB		20
#define MT_TB		21
#define MT_TLB		22
#define MT_TAB		23
#define MT_TT		24
#define MT_TV		25
#define MT_UD		26
#endif	// DISABLE_WIN95_MESSAGES

//
// Object type defines.
//
#define OT_NONE		(-1)
#define OT_WINDOW	0
#define OT_THREAD	1
#define OT_PROCESS	2
#define OT_MESSAGE	3

//
// These are the indices to the bitmaps used for the different
// object types in the tree controls.  The order shown MUST match
// the order that the images appear from left to right in the
// IDB_FOLDERS bitmap!
//
#define IDX_WNDBITMAP		0	// Windows
#define IDX_THDBITMAP		1	// Threads
#define IDX_PRCBITMAP		2	// Processes
#define MAX_OBJECTBITMAPS	3

//
// These are the indices to the Message/Start Logging and
// Message/Stop Logging graphics in the toolbar bitmap.
// If you modify the toolbar bitmap, you MUST update these
// indices!  They need to be hardcoded numbers because
// of the way toolbars were designed.  The Start/Stop Logging
// command is one that changes its image based on its state.
//
#define INDEX_START_IMAGE	4
#define INDEX_STOP_IMAGE	18

//
// Starting offset for string resources accessed using the ids() function.
//
#define STRINGID_BASE		10000

//
// Maximum number of string resources accessed using the ids() function.
// This number should be kept low to save memory, but it must be large
// enough to cover all the string resources that will be retrieved
// using the ids() function (which start at STRINGID_BASE).
//
#define MAX_STRINGIDS		140

//
// The Message Description Table structure that describes each message
// that Spy++ understands.
//
typedef struct
{
	UINT msg;
	LPTSTR pszMsg;
	INT iMsgType;
	PFNDECODE pfnDecode;
	PFNDECODERET pfnDecodeRet;
	PFNDECODEPARM pfnDecodeParm;
} MSGDESC, *PMSGDESC;

//
// The message group structure.  A table of these structures describes
// each different group of messages that can be selected/deselected
// for viewing using the Message Filters dialog.
//
typedef struct
{
	INT idCheckBox;
	INT cMsgs;
	INT cUseCount;
} MSGGROUP, *PMSGGROUP;

//
// spy.cpp
//
extern CSpyApp theApp;
int SpyMessageBox(UINT nIDPrompt, UINT nType = MB_OK, UINT nIcon = MB_ICONEXCLAMATION);
CHAR* ids(UINT idString);
LPSTR GetExpandedClassName(LPSTR);

//
// msghook.cpp
//
void LogMsg(PMSGSTREAMDATA pmsd);
BOOL CreateHookThread(void);
BOOL SetMsgHook(BOOL fSet);
BOOL GetOtherProcessData(HWND hwnd, POTHERPROCESSDATA popd);
BOOL IsSpyableWindow(HWND hwnd);

//
// help.cpp
//
// Help type constants.  These are passed to InvokeWinHelp() and
// determine what type of help is being requested.
//
#define HELPTYPE_INDEX			0	// Index help
#define HELPTYPE_HELPONHELP		1	// Help on using help
#define HELPTYPE_CONTEXT		2	// A Spy++ help context
#define HELPTYPE_MSGHELP		3	// Help on a msg (from Win32 help)

CString GetRegString(LPCSTR szSection, LPCSTR szKey, LPCSTR szDefault = NULL);
BOOL WriteRegString(LPCSTR szSection, LPCSTR szKey, LPCSTR szVal);

//
// propinsp.cpp
//
void SetLastSelectedObject(DWORD dwObject, int nObjectType);
DWORD GetLastSelectedObject();
int GetLastSelectedObjectType();
void ChangeToLastSelectedObject();
void SetSpyImmediate(BOOL bDirect);
BOOL GetSpyImmediate();

extern CPropertyInspector* g_pInspector;
