/////////////////////////////////////////////////////////////////////////////
//      MAIN.H
//              Developer Studio main application header file.
//

#ifndef __MAIN_H__
#define __MAIN_H__

#ifndef __AFXWIN_H__
        #error include 'afxwin.h' before including this file for PCH
#endif

#define PRE_MFC_IDLE_PROCESSING_COUNT 3

typedef UINT TYPE_FILTER;

// The types of options dialogs...
enum OPTDLG_TYPE { options, customize };

// EXE strings available from CTheApp::GetExeString
enum EXESTRING_INDEX {
        DefaultAppHelpFile = 0,
        DefaultRefHelpFile,
        DefaultRegKeyName,
        DefaultExtensionDir,
        DefaultControlHelpFile
};

// UI elements that can be disabled from CTheApp::IsUIElementEnabled
enum UIELEMENT_INDEX {
        UieToolbarCustomize = 0,
        UieToolsCustomize,
        UieKeyCustomize,
        UieWorkspaceOptions,
        UieFormatOptions,
};

// display types for CTheApp::HelpOnKeyword
enum REF_DISPLAY_TYPE { topicWindow, popupWindow };

// find types for CTheApp::HelpFind
enum HELP_FIND_PAGE { HF_Default, HF_Index, HF_Search };

// ANSI code page identifier (values for mbcodepage in CTheApp)
#define OEM_JAPAN                       932
#define OEM_PRC                         936
#define OEM_TAIWAN                      950
#define OEM_KOR_WANGSUNG        949
#define OEM_KOR_JOHAB           1361
#define OEM_US                          437

// Define different level of file dependency notification
#define FILEDEP_DISABLENCB  0
#define FILEDEP_DISABLE_ALL 1
#define FILEDEP_DISABLE_ONOPEN 2
#define FILEDEP_DISABLE_IFUPTODATE 3

// Help defines
#define HELPTYPE_DIALOG             0x4010
#define HELPTYPE_WINDOW             0x4020
#define HELPTYPE_COMMAND            0x4030
#define HELPTYPE_CONTROL            0x4040

#include <afxtempl.h>
#ifndef __IDS_H__
#include "ids.h"
#endif

#ifndef __PATH_H__
#include "path.h"
#endif

#ifndef __SLOB_H__
#include "slob.h"
#endif

#ifndef __PACKAGE_H__
#include "package.h"
#endif

#ifdef _DEBUG
#define _EXPORT_LAYOUT
#endif

#ifdef _SHIP
#pragma message("Using _SHIP switch")
#endif



#ifndef _SHIP
// Enable/Disable this if you want to build with the MTTF Dialog.
#define _MTTF
#pragma message("MTTF is turned ON.")

#pragma message("NOT Using _SHIP switch")

#ifdef _DEBUG
// Enable/Disable this if you want to build with command logging.
#define _CMDLOG

// Enable/Disable this if you want to build with IDE startup logging.
#define _IDELOG

// Enable/Disable this if you want to build with Edit and Continue success/failure logging
#define _ENCLOG
#endif
#endif

#undef AFX_DATA
#define AFX_DATA AFX_EXT_DATA

class CApplication;                     // Defined in ide\include\autoapp.h
class CInternalApp;
class CIPCompHostApp;
class CPartFrame;
class CPartTemplate;
class CPartDoc;
class CIPCompDoc;
class CBRecentFileList;
class CTypedRecentFileList;

struct BARPOPDESC;

interface IMsoStdComponentMgr;

/////////////////////////////////////////////////////////////////////////////
// CAppSlob

class CAppSlob : public CSlob
{
public:
        CAppSlob();
        ~CAppSlob();

        virtual BOOL SetSlobProp(UINT nID, CSlob* val);

        // the application global properties
        CSlob * m_pCurrentSlob;
        BOOL m_bPoppingProperties;

        DECLARE_SLOBPROP_MAP()

protected:
        // over-ride the SetStrProp() so that we can
        // trap setting of the current slob
        virtual void OnInform(CSlob* pChangedSlob, UINT idChange, DWORD dwHint);
};

#ifdef  _CMDLOG
/////////////////////////////////////////////////////////////////////////////
// CCmdMap

class CCmdMap : public CMapWordToOb
{
        DECLARE_SERIAL(CCmdMap)
public:
        CCmdMap() {}
        virtual ~CCmdMap() {}

        void Update(WORD wKey, DWORD nCount = 1);
        void Serialize(CArchive& ar);
};
#endif  // _CMDLOG

/////////////////////////////////////////////////////////////////////////////
// Just in time debugging

class CJit      // Just in time debugging
{
public:
                        CJit() : m_fActive(FALSE), m_pid(0), m_hEvent(0),
                                m_fPathIsReal(FALSE)            { }
        VOID    SetActive(BOOL fActive)                 { m_fActive = fActive; }
        BOOL    GetActive()                                             { return m_fActive; }
        VOID    SetPid(DWORD pid)                               { m_pid = pid; }
        DWORD   GetPid()                                                { return m_pid; }
        VOID    SetEvent(HANDLE hEvent)                 { m_hEvent = hEvent; }
        HANDLE  GetEvent()                                              { return m_hEvent; }
        BOOL    FSetPath(const char *szPath, BOOL fPathIsReal)
                                                                                        { m_fPathIsReal = fPathIsReal;
                                                                                          return m_path.Create(szPath); }
        const CPath& GetPath()                                  { return m_path; }
        BOOL    FPathIsReal()                                   { return m_fPathIsReal; }

private:
        BOOL    m_fActive;
        DWORD   m_pid;
        HANDLE  m_hEvent;
        CPath   m_path;
        BOOL    m_fPathIsReal;  // whether m_path is the real mccoy or a dummy
};

///////////////////////////////////////////////////////////////////////////////
//      CCPLReg - Control panel stuff
//

enum FORMAT_TYPE {TIME_ALL, TIME_NO_SECONDS, DATE_ALL};

class CCPLReg
{
public:
        CCPLReg();
        ~CCPLReg();
        const CString Format(CTime& time, FORMAT_TYPE fType, DWORD dwFlags = DATE_SHORTDATE);
        BOOL IsMetric () const { return !m_bEnglishMeasure; }
        const CString & Decimal () const { return m_strDecimal; }
        void Update();

// Attributes
private:

        BOOL    m_bEnglishMeasure;
        CString m_strDecimal;

        // Kernel.dll handle, and pointers to date/time formatting functions
        // within it.
        HINSTANCE m_hKernel;
        int (FAR WINAPI *m_lpfnGetDateFormatW)(LCID, DWORD, CONST SYSTEMTIME *, LPCWSTR, LPWSTR, int);
        int (FAR WINAPI *m_lpfnGetDateFormatA)(LCID, DWORD, CONST SYSTEMTIME *, LPCSTR, LPSTR, int);

        int (FAR WINAPI *m_lpfnGetTimeFormatW)(LCID, DWORD, CONST SYSTEMTIME *, LPCWSTR, LPWSTR, int);
        int (FAR WINAPI *m_lpfnGetTimeFormatA)(LCID, DWORD, CONST SYSTEMTIME *, LPCSTR, LPSTR, int);

// Implementation
private:
        CString GetCPLString(LCTYPE lcType, LPCSTR szDefault);
        UINT GetCPLInt(LCTYPE lcType, int nDefault);
};

/////////////////////////////////////////////////////////////////////////////
// CTheApp

#define DHF_CENTER              0x0001
#define DHF_3DCONTROLS  0x0002

extern BOOL fVCPPIsActive;
class CVshellPackage;
class CHaltRecording;           // Defined in utilauto.h
class CDlgTab;

#ifndef __PRJAPI_H__
#include <prjapi.h>                             // for IPkgProject (used in BuildFilterList)
#endif

class CTheApp : public CWinApp
{
        friend class CVshellPackage;
        friend class CHaltRecording;
public:
        enum REPEAT_COUNT_STATE { FinishedAbsolute, FinishedMaybe, FinishedNot, DoneRepeating };
        enum RECORD_STATE { RecordOff, RecordOn, RecordPaused };

private:
        UINT                    m_cActiveFlashPumps;
        UINT                    m_nTimerID;
        INT                                             m_cCmdRepeat;                           // Repeat count to apply to next repeatable command
        REPEAT_COUNT_STATE      m_SettingRepeatCount;   // In the process of setting the count?
        CIPCompHostApp*                 m_pAppCmpHost;
        CInternalApp*                   m_pInternalApp;
        BOOL                                    m_bShouldRecordText;
        int                                             m_nRecordState;
                short                                   m_nAsyncCmdLevel;               // see inlines below


private:
        BOOL InitStdCompMgr();
        BOOL CmpMgrDoIdle();

public:

        CTheApp(const TCHAR** rgszPackages, int nPackages);
        ~CTheApp();
        BOOL ShouldRecordText() { return m_bShouldRecordText; }
        int GetRecordState() { return m_nRecordState; } // One of the RECORD_STATE enum values

        CPartTemplate* GetTemplate(REFCLSID clsid);
        CPartTemplate* GetTemplate(LPCTSTR szType);
        CPartTemplate* GetOpenTemplate(LPCTSTR lpszPathName, CDocument*& pDoc);
        CPartFrame* GetOpenFrame(LPCSTR szFileName,
                CDocTemplate*& pTemplate, CDocument*& pDoc);

        IMsoStdComponentMgr*    m_pMsoStdCompMgr;
        CMapWordToPtr   m_RepeatableCmds;               // Table of repeatable commands
        BOOL                    m_fReplaceRepeatCount;  // Should next arg replace the existing count?
        BOOL m_fVerbose;        // Set to FALSE to disable some message boxes

// Overrides
        virtual BOOL InitInstance();
        virtual int ExitInstance();
        virtual int Run();
        virtual BOOL PumpMessage();     // low level message pump
        virtual BOOL OnIdle(long lCount);
        virtual BOOL IsIdleMessage(MSG * pMsg);
        virtual CDocument* OpenDocumentFile(LPCSTR lpszFileName); // open named file
        virtual void AddToRecentFileList(const TCHAR* pszPathName);  // add to MRU
        void SetRecentFileListSize(int nSize);
        void SetRecentProjectListSize(int nSize);
        int GetRecentFileListSize();
        int GetRecentProjectListSize();
        void LockRecentFileList(BOOL bLock);    // Lock to disable MRU adding.
        void TermStdCompMgr();
                inline CIPCompHostApp *GetStdCompMgr(void) { return m_pAppCmpHost; };

// Like OpenDocumentFile, but has option to open file via ShellExecute. Function
// first tries registered templates to see if any can open the file in native mode.  If
// none can, the function calls ShellExecute to see if there is an association for the
// file.  If there's no association, the function lets the "best match" template open the file.
        CDocument* OpenDocumentFileEx(LPCSTR lpszFileName, BOOL bTryShellToo);

        inline const MSG* GetCurrentMsg() const { return &m_msgCur; }
        inline UINT GetCurrentMessage() const { return m_msgCur.message; }

        virtual BOOL PreTranslateMessage(MSG* pMsg);
        virtual BOOL OnDDECommand(char* pszCommand);
        virtual int DoMessageBox(LPCSTR lpszPrompt, UINT nType, UINT nIDPrompt);
        LPDISPATCH GetPackageExtension(LPCTSTR szExtensionName);

        HRESULT FindInterface(REFIID riid, LPVOID FAR* ppvObj);
        HRESULT GetService(REFGUID guidService, REFIID riid, void **ppvObj);
        HRESULT DoesIntrinsicBuilderExist(REFGUID rguidBuilder);
        HRESULT GetIntrinsicBuilder(REFGUID rguidBuilder, DWORD grfGetOpt,
                                                        HWND hwndPromptOwner, IDispatch **ppdispApp,
                                                        HWND *pwndBuilderOwner, REFIID riidBuilder,
                                                        IUnknown **ppunkBuilder);

        HRESULT GetIntrinsicBuilder(REFGUID guidService, REFIID riid, void **ppvObj);

        BOOL IdleAndPump(HANDLE hEventEarlyBreak =0);
        BOOL MainLoop(HANDLE hDummyEvent = 0, BOOL fPushed = FALSE, ULONG uReason = 0);

        void FlashStatePump();

// Implementation
        BOOL LoadPackages(int *pnPackagesLoaded);               // Returns FALSE on error, TRUE otherwise;
                                                                                                                // pnPackagesLoaded returns the count of loaded packages,
                                                                                                                // if return value is TRUE
        void UnloadPackages();
        BOOL RegisterPackage(CPackage* pPackage);
        BOOL RegisterPacket(CPacket* pPacket);
        HWND GetActiveView();
        void SetActiveView(HWND hwnd);
        void ReactivateView();

        void BuildFilterList(CString& rstrFilter, CPtrArray* ppaEditors,
                int nOpenDialog, IPkgProject *pContextProject = NULL);
        CDocTemplate* GetTemplateFromEditor(UINT idsEditor, const char* szFilename);

        BOOL DoPromptFileOpen(CStringArray& saFileNames, UINT nIDSTitle,
                        DWORD lFlags, CDocTemplate* pTemplate,
                        UINT* nOpenAsEditor = NULL, BOOL* pbOpenReadOnly = NULL);
        BOOL DoPromptFileSave(CString& fileName, UINT nIDSTitle,
                        DWORD lFlags, CPartDoc* pDocument, UINT* pnSaveType);

        BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra,
                AFX_CMDHANDLERINFO* pHandlerInfo);

        void OnDirChange();

        // The following function is used by OnFileOpen and the automation
        // Documents.Open to Openg a file.
        CDocument* OpenFile(CDocTemplate* pTemplate, // Pointer to the template which opens the file. Can be Null.
                                                CString strFilename,     // Filename to open.
                                                BOOL bReadOnly);                 // True if the file should be marked readonly.

        void OnFileNewProject(BOOL fAddToWks = FALSE);

        //{{AFX_MSG(CTheApp)
        afx_msg void OnAppAbout();
        afx_msg void OnFileNew();
        afx_msg void OnFileNewInProject();
        afx_msg void OnFileOpen();
        afx_msg void OnInsertFiles();
        afx_msg void OnUpdateRecentProjMenu(CCmdUI* pCmdUI);
        afx_msg BOOL OnOpenRecentFile(UINT nID);
        //}}AFX_MSG
        DECLARE_MESSAGE_MAP()


public:
        CPackage*       GetPackage(UINT idPackage);
        CPacket*        GetPacket(UINT idPackage, UINT idPacket, BOOL bActivate = FALSE);
        BOOL            ActivatePacket(UINT idPackage, UINT idPacket)
                                        { return GetPacket(idPackage, idPacket, TRUE) != NULL; }
        BOOL            ActivatePacket(CPacket* pPacket)
                                        { SetActivePacket(pPacket); return TRUE; }
        BOOL            DeactivatePacket();
        void            SetActivePacket(CPacket* pPacket);

                // This tells all packages to lose their loaded bitmaps. We use this after startup is complete to lose these
                // bitmaps, which we no longer need anyway, as they'll have been copied into the toolbars/menus
        void            UnloadPackageBitmaps();

        void            PrintCommandTables();

        // For a given command, this determines the package and the flags associated with the button
        UINT            GetCommandBtnInfo(UINT nCommand, CPackage** ppPackage);

        // This function is used to find the bitmap and glyph index for a given command (nID), and button size
        // It returns true if the command could be found. If pCTE is non-null, it should point to the
        // the CTE of nID; this is a simple optimisation.
        BOOL            GetCommandBitmap(UINT nID, HBITMAP *hbmWell, int *iGlyph, BOOL bLarge, CTE *pCTE=NULL);
        BOOL            HasCommandBitmap(UINT nID) { return GetCommandBitmap(nID, NULL, FALSE, NULL); };

        void            DestroyAll(UINT nFlags = 0);
        BOOL            CanCloseAll(UINT nFlags = 0);
        BOOL            SaveAll(BOOL bQuery = FALSE, UINT nFlags = 0);
        BOOL            HasOpenDocument(UINT nFlags = 0);

        void            DoOptionsDialog(UINT nIDPageCaption = 0);
        void            DoCustomizeDialog(UINT nIDPageCaption = 0, LPCTSTR szMacroName = NULL);

        INT                     GetCmdArg() { return m_cCmdRepeat; }    // to use it as a numerical arg, not repeat count
        INT                     GetRepCount() { return m_cCmdRepeat; }
        void            ClearRepCount()
                                {
                                        m_cCmdRepeat = 1;
                                        if (m_SettingRepeatCount != DoneRepeating)
                                        {
                                                m_SettingRepeatCount = FinishedAbsolute;
                                                UpdateRepeatStatus();
                                        }
                                }

        void            SetRepCount(INT c) { m_cCmdRepeat = c; }
        void            UpdateRepCount(INT i);
        void            InitRepeatableCommands(void);
        BOOL            FSettingRepeatCount(void)
                                {
                                        if (m_SettingRepeatCount == DoneRepeating)
                                                return FALSE;

                                        if (m_SettingRepeatCount != FinishedAbsolute)
                                        {
                                                m_SettingRepeatCount = FinishedNot;
                                                return TRUE;
                                        }
                                        return FALSE;
                                }

        BOOL            FSettingRepeatCount(MSG *pMsg)
                                {
                                        if (m_SettingRepeatCount == FinishedAbsolute)
                                                return FALSE;

                                        if (m_SettingRepeatCount == FinishedNot)
                                                return TRUE;

                                        if (m_SettingRepeatCount == FinishedMaybe && pMsg->message == WM_CHAR &&
                                                        (((TCHAR)(pMsg->wParam) == '-') || _istdigit((TCHAR)(pMsg->wParam))))
                                        {
                                                m_SettingRepeatCount = FinishedNot;
                                                return TRUE;
                                        }
                                        return FALSE;
                                }
        void            BeginSetRepeatCount(INT cRpt = -1, BOOL fReplacable = FALSE);
        void            EndSetRepeatCount(REPEAT_COUNT_STATE);
        BOOL            FCmdRepeatPretranslate(MSG *);
        void            UpdateRepeatStatus();
        // Packages which have UI elements following the command bars style should call this function during
        // WM_MOUSEMOVE processing to decide whether they need to extrude themselves at this time.
        BOOL            ShouldExtrude(void);
        BOOL            IsCustomizing(void);

        void LoadMRUList();
        void CalculateRecentMnemonics();
        void LoadPrivateProfileSettings();
        void SavePrivateProfileSettings();
        UINT GetProfileInt(LPCTSTR szSection, LPCTSTR szKey, int nDefault);
        CString GetProfileString( LPCTSTR szSection, LPCTSTR szKey, LPCTSTR szDefault = NULL);

        // Tooltip implementation helpers. These were previously silently exported by having their declarations
        // hidden in the using packages. We now export them properly.
        static BOOL HandleToolTipText(UINT, NMHDR* pNMHDR, LRESULT* pResult, LPCTSTR szKeyIn = NULL);
        static void AddKeyToToolTip(CString& rstrTextCmd, UINT nID, WORD flagsCmdCache, LPCTSTR szKeyIn);
        static void SetEmptyToolTip(NMHDR* pNMHDR);

        // Takes the name of the command and returns its command ID.
        //  This searches the entire list of commands so THIS IS VERY
        //  INEFFICIENT-- USE IT ONLY IF YOU HAVE TO
        // Returns BOOL indicating whether the ID was found.
        BOOL GetCommandID(LPCTSTR szCommandName, UINT* pnCmdID);

        // Packages may use this to check whether a command is currently bound to a key
        // This is used by devaut1 to remember to never change the cmdID of bound commands.
        BOOL IsCommandKeyBound(WORD id);

        // Packages may use this to add a command dynamically instead of using
        //  cmdcomp.  This is used by devaut1 to add commands for macros and DLL AddIns
        BOOL AddCommand(WORD id, WORD group, WORD flags, WORD glyph, CPack* pPack,
                                        LPCTSTR szCommand, int nCmdLength);

        // Packages can use this to replace a substring within the command string:
        //  either the menutext, prompt text, or tip text.  Devaut1 uses this to
        //  update the command's description when the user updates a macro's description
        //  in the macro file.  You may not use this to change the name of the command
        //  itself.  This will dirty the command cache.
        void ReplaceCommandString(UINT nID, UINT iString, LPCTSTR szNewSubString);

        // Packages may use this to create one-shot toolbars. The bar would not be owned by the package, and is
        // drawn from the users stock of custom bars. Bars should be created all at one time. If pPackage or dwCookie
        // are different from the last time this function was called, then a new bar will be created.
        //  nButtonType is either dsGlyph or dsText
        //  (see ide\pkgs\include\ObjModel\appdefs.h)
        BOOL AddCommandBarButton(CPackage *pPackage, DWORD dwCookie, UINT nId,
                long nButtonType);

        // Packages can use this to implement dragable areas in customization pages
        // call this in the page's Activate function
        void ActivateDraggableCustomizePage(CDlgTab *pTab);
        // Call this in LBUTTONDOWN. Note that this will eat LBUTTONUP, so you will need to fake one if
        // the drag source is something like a list box, which relies on the lbutton up.
        void DragCommandButton(CPoint ptSource, UINT nId);
        // call this in the page's Deactivate function
        void DeactivateDraggableCustomizePage(CWnd *pParentWnd);

        // Packages may use this to see if a command of this name already exists
        //  This is used by devaut1 to make sure macros/addin cmds don't clash
        //  with preexisting ones.
        BOOL DoesCommandNameExist(LPCTSTR szCmdName);
		LPCTSTR GetCommandName(WORD id);


        // Packages should use this to remove commands that are being permanently removed from the system.
        // UI elements related to the command will be irretrievably removed from the UI. If pPack is not
        // the owner of the command, then the command will not be removed. This stops most commands being removed
        // erroneously by confused packages.
        BOOL RemoveCommand(WORD id, CPack* pPack);

                // Asynchronous commands.  Automation calls to
                //  Application::ExecuteCommand must not return until the command is
                //  really finished.  For example, the ToolsCustomize command handler
                //  returns immediately, even though the tools customize dialog (actually
                //  a modeless window) is not dismissed yet.  Thus, that command must
                //  inform the shell of when it begins and ends, so that ExecuteCommand
                //  can know when it's truly safe to return.

                // Call this in your asynchronous command handler, before returning
                void BeginAsyncCmd() { m_nAsyncCmdLevel++; }

                // Call this when your command is fully completed.  (For example,
                //  the ToolsCustomize dialog will call this on destruction.)
                void EndAsyncCmd() { m_nAsyncCmdLevel--; ASSERT(m_nAsyncCmdLevel >= 0); }

                // This is used by Application::ExecuteCommand to detect when all
                //  asynchronous commands are finished.
                WORD GetCurrentAsyncCmdLevel() { return m_nAsyncCmdLevel; }

                // Start iteration thru filtered windows
                CPartFrame* MDITypedGetActive(TYPE_FILTER tf, BOOL bForwards=FALSE) { return MDITypedGetNext(NULL, tf, bForwards); };
                // iterate to next in filtered windows
                CPartFrame* MDITypedGetNext(CPartFrame* pFrame, TYPE_FILTER tf, BOOL bForwards=FALSE);



#ifdef _WIN32
        void LoadPrivateAutosaveSettings();
        void SavePrivateAutosaveSettings();
#endif


#ifdef _CMDLOG
        void InitCmdLog();
        void CmdLogOnExit();
        void UpdateCmdLog();
#endif

#ifdef _IDELOG
        void InitIDEstartupLog();
#endif

        void SetDialogBkColor()
                { CWinApp::SetDialogBkColor(::GetSysColor(COLOR_BTNFACE)); }

        void SetAppWizHwnd(HWND hWnd);

        //      Load and save vcp files and notify all interested parties:
        BOOL LoadLastProject();
        BOOL SaveLastProject();

        BOOL SerializeAllWorkspaceSettings(const CPath *pPath, DWORD dwFlags, BOOL bSave);
        BOOL LoadAllWorkspaceSettings(const CPath *pPath, DWORD dwFlags)
                { return SerializeAllWorkspaceSettings(pPath, dwFlags, FALSE); }
        BOOL SaveAllWorkspaceSettings(const CPath *pPath, DWORD dwFlags)
                { return SerializeAllWorkspaceSettings(pPath, dwFlags, TRUE); }

        BOOL NotifyPackages (UINT id, void *Hint = NULL, void *ReturnArea = NULL );
        void GetPackagesDefProvidedNodes(CPtrList &DefNodeList, BOOL bWorkspaceInit);
        UINT GetCP (void)
                { return m_codePage; }
        UINT GetCharSet (int * piFaceName = NULL) const;

        int IdsFromFerr(int ferr);

        // This must be overriden by the exe entry module to provide default menus
        virtual POPDESC *GetMenuDescriptor(UINT nId)=0;
        virtual LPCTSTR GetExeString(EXESTRING_INDEX i) = 0;
        virtual HGLOBAL GetStandardToolbarData() = 0;
        virtual BOOL IsUIElementEnabled(UIELEMENT_INDEX i) { return TRUE; }
        virtual void ShowSplashScreen(BOOL bShow);
                virtual UINT GetExecutableProperty(UINT nProperty);
	virtual BOOL WriteLog(LPCTSTR szOut, BOOL bAppendCRLF = FALSE);

        DWORD CurrentPromptContext()    { return m_dwPromptContext; }

        // MSIN Help support
        void HelpContents();
        void HelpFind(LPCTSTR szKeyword, HELP_FIND_PAGE hf = HF_Default);
        void HelpOnApplication(DWORD dwID, UINT type = HELPTYPE_DIALOG, LPCTSTR szFileName = NULL);
        void HelpOnKeyword(LPCTSTR szKeyword, BOOL bAlink = FALSE, REF_DISPLAY_TYPE dt = topicWindow);
        BOOL ScanCommandLine(const TCHAR* szSwitch, CString* pstrArg = NULL, BOOL bRemove = TRUE);

        const char* m_pszRegKeyName;

        int                     m_nLockRecentFileList;
        CPoint          m_ptPropPage;
#ifdef _WIN32
                // FUTURE: Perhaps this should be an enum for Chicago, NT/Intel, NT/Mips, etc.?
                BOOL                    m_bOnChicago;
                BOOL                    m_bWin4;    // Either Win95 or WinNT 4.0 or higher
				BOOL					m_bWin5;	// WinNT 5
                BOOL                    m_bWin95;   // Win95 (NOT WinNT 4.0)
#endif
                BOOL                    m_bMaximizeApp;
                BOOL                    m_bMaximizeDoc;
                BOOL                    m_bWorkspaceAuto;
                BOOL                    m_bToolTips;
                BOOL                    m_bToolTipKeys;
                BOOL                    m_bLargeToolBars;
                BOOL                    m_bCompressMak;
                BOOL                    m_bStatusbar;
                BOOL                    m_bMenuDirty;                           // TRUE if the menus need to be rebuilt on idle
                BOOL                    m_bProjectCommandsDirty;        // TRUE if the project commands need to be rebuilt on idle
                BOOL                    m_bMenuDead;
                BOOL                    m_bDeactByOtherApp;
                WORD                    m_wHookDialogs;
                BOOL                    m_fOEM_MBCS;
                BOOL                    m_bFastHelp;
                BOOL                    m_bOpenedFromMRU;
                BOOL                    m_bOpenedFromDrop;
                BOOL                    m_bOpenedFromLastLoad;
                BOOL                    m_fVCPPIsActive;
                BOOL                    m_bFirstInstance;
                BOOL                    m_bPinGoToDlg;  // if true, goto dialog stays up after goto
                BOOL                    m_bPkgChk;
                BOOL                    m_bWindowMenuSorted;  // true if the window menu is being sorted into MRU-first order.
                // m_bRestoreKey is a flag whether we use /RESTORE option
                // or not. If this flag is set, we will not prepend the default
                // environment (eg: path, include, and lib) for dolphin since
                // it should be already set in *.key file
                BOOL                    m_bRestoreKey;
                BOOL                    m_bEnableWorkspaceLoad; // load workspace during init?
                BOOL                    m_bDlgEdDblClickEditCode;  //compatibility option for double click in dialog editor
                BOOL                    m_bActivated;
                BOOL                    m_bRunInvisibly;        // RunAutomated or RunEmbedded (from automation controller, e.g.)
                BOOL                    m_bHideMicrosoftWebMenu;                // Hides the microsoft on the web menu
                BOOL                    m_bMRUOnFileMenu;
                BOOL                    m_fRunningMacro;
#ifndef NO_VBA
                BOOL                    m_fOleAutomation;       // expose programmable objects?
                                                                                                // (post-Olympus feature, currently disabled by default)
#endif  // NO_VBA

        UINT            m_codePage;
        CApplication*   m_pAutoApp;       // OLE Automation Applicationobject
        int             m_nWindowMenuItems; // total entries in the window menu
        HWND            m_hwndValidate;
        CAppSlob        m_theAppSlob;
        CObList         m_packages;
        CObList         m_packets;
        CStringList     m_argList;
        CString         m_strCurArg;
        CJit            m_jit;
        CCPLReg         m_CPLReg;
        CPacket*        m_pActivePacket;
        CPacket*        m_pMDIActivePacket;
#ifdef _CMDLOG
        CCmdMap         m_cmdTable;
        BOOL            m_bCmdLog;
        UINT            m_timerCmdLog;
#endif
#ifdef _IDELOG
        BOOL            m_bIDELog;
#endif
#ifdef _EXPORT_LAYOUT
        BOOL            m_bExportLayout;
        const char* m_pszLayoutFilePath;
#endif
        CBRecentFileList* m_pRecentProjectList;
        DWORD           m_dwMenuSelectTime;

/////////////////////////////////////////////////////////////////
// disabling NCB, FileDep Notification,
/////////////////////////////////////////////////////////////////
        UINT	        m_nDisableFileDep;

        BOOL            m_bInvokedCommandLine;
        BOOL            m_bQuitting;
		HANDLE		m_hLogFile;

        // Edit and Continue performance logging
        BOOL            m_bEncPerfLog;

#ifdef _ENCLOG
        // Edit and Continue success/failure logging
        BOOL            m_bEncLog;
#endif
        // WARNING!!!  Don't insert anything below this unless you're positive
        //  all the packages that use it are build with the same PERF_MEASUREMENTS
        //  setting as the shell is built with.  The shell *defaults* to building
        //  with PERF_MEASUREMENTS on, so your package better, too.  Or just
        //  don't insert anything below this.
#ifdef PERF_MEASUREMENTS
        ///////////////// Testing code for V4 perf release (fabriced)
        BOOL            m_bPerfQA_AutoRun;
        BOOL            m_bPerfQA_WorkSet;
        BOOL            m_bPerfQA_Workspace;  // Open and close Workspace
        char*           m_szPerfQA_Project;
        DWORD           dwWorkspaceTime;        // Times from project file part of open/close workspace
/////////////////////////////////////////////////////////////
#endif  // PERF_MEASUREMENTS

#ifdef _MTTF
        void InitMTTF();
        void MTTFOnExit();
        BOOL            m_bMTTF;
        UINT            m_timerMTTF;
#endif
};

#define theApp (*((CTheApp*) AfxGetApp()))

inline BOOL UseWin4Look()
        { return theApp.m_bWin4; }

inline BOOL NotOnNT()
        { return theApp.m_bWin95; }

void SetModeName(UINT nModeNameStringID);
void SetModeName(LPCTSTR szModeName);
void SetSubTitle(LPCTSTR szSubTitle);

////////////////////////////////////////////////////////////////////////////
// IME utility functions

void imeMoveConvertWin (HWND, int, int);
void imeChangeFont (HWND, HFONT);
BOOL imeEnableIME (HWND, BOOL);
void imeFlush( HWND hWnd );
BOOL imeIsOpen();
BOOL imeIsEnabled();
BOOL imeIsActive();

///////////////////////////////////////////////////////////////////////////
// DBCS utility functions

//      declarations of DBCS string functions in dbstr.cpp
//  These functions take the place of _str* functions that windows didn't
//  duplicate with lstr* functions. (DBCS enabled)

LPSTR dbstrchr( LPCSTR sz, int n );
LPSTR dbstrrchr( LPCSTR sz, int n );
LPCSTR dbstrstr( LPCSTR string1, LPCSTR string2 );
int dbstrnicmp( LPCSTR string1, LPCSTR string2, size_t count );

BOOL IsJapaneseSystem();
BOOL IsSBKatakana( unsigned char c );
BOOL IsDBCSTrailByte( LPCSTR string, LPCSTR pch );

////////////////////////////////////////////////////////////////////////////
//      Help

void GetHelpDir(CString & strHelpDir);
void DoHelpContextMenu(CWnd* pDialog, UINT idDlg, CPoint pt);
BOOL DoHelpInfo(CWnd* pDialog, UINT idDlg, HELPINFO* pInfo);
BOOL GetHID(HWND hwndDlg, HWND hwndCtrl, HWND * phwndHelp, DWORD* hid);

////////////////////////////////////////////////////////////////////////////
// Registry access helpers

const char chKeySep = '\\';

LPSTR   GetRegistryKeyName();
CString GetRegString(LPCSTR szSection, LPCSTR szKey, LPCSTR szDefault = NULL);
UINT    GetRegInt(LPCSTR szSection, LPCSTR szKey, int nDefault);
HGLOBAL GetRegData(LPCSTR szSection, LPCSTR szKey, HGLOBAL hglobDefault);
IStream *GetRegIStream(LPCTSTR szSection, LPCTSTR szKey);
BOOL    WriteRegString(LPCTSTR szSection, LPCTSTR szKey, LPCTSTR szVal);
BOOL    WriteRegInt(LPCSTR szSection, LPCSTR szKey, int nValue);
BOOL    WriteRegData(LPCSTR szSection, LPCSTR szKey, HGLOBAL hglobValue);
BOOL    WriteRegIStream(LPCTSTR szSection, LPCTSTR szKey, IStream *pIStream);
BOOL    DeleteRegData(LPCSTR szSection, LPCSTR szKey);

/////////////////////////////////////////////////////////////////////////////
//      Function to take an IUnknown, request an IPersistStream from it and
//      assuming it supports it, writes the stream to the registry.
extern HRESULT DevStudioPersistStreamToRegistry(IUnknown *pIUnknown, LPCTSTR szSection, LPCTSTR szKey);
extern HRESULT DevStudioLoadStreamFromRegistry(LPCTSTR szSection, LPCTSTR szKey, const CLSID &rclsid,
                                                                                           REFIID riid, LPVOID *ppvUnknown,
                                                                                           IUnknown *pIUnknownOuter = NULL, CLSCTX clsctx = CLSCTX_INPROC_SERVER);

/////////////////////////////////////////////////////////////////////////////
// CKeymap

class CKeymap : public CMapWordToPtr
{
public:
        CKeymap();
        virtual ~CKeymap();

        virtual BOOL LookupCommand(UINT nCmdID, WORD& kcOne, WORD& kcTwo, CKeymap* pOverride = NULL);
        virtual BOOL LookupKey(WORD kc, UINT& nCmdID, CKeymap*& pKeymap);

        void LoadAccelerators(HINSTANCE hInstance, LPCTSTR lpResourceID);
        void LoadAccelerators(HACCEL hAccelTable);
        virtual void LoadEditorAccelerators();

        void SetKey(WORD kc1, UINT nCmdID);
        void SetKey(WORD kc1, WORD kc2, UINT nCmdID);
        void SetKey(WORD kc, CKeymap* pKeymap);
        void RemoveKey(WORD kc1, WORD kc2 = 0);
        void RemoveAll();

        POSITION GetStartPosition()
                { return CMapWordToPtr::GetStartPosition(); }
        void GetNextKey(POSITION& pos, WORD& kc, UINT& nCmdID, CKeymap*& pKeymap);

        BOOL TranslateMessage(CWnd* pWnd, MSG* pMsg);

        void WriteReg(const TCHAR* szKey = NULL);
        BOOL GetReg(const TCHAR* szKey = NULL);

        CString m_strName;
        CPacket* m_pPacket;
        BOOL m_bDirty;

        static void GetKeyName(WORD kc1, WORD kc2, CString& str);
        static void CancelChord();

        static void SetEscIsAltPrefix(BOOL bSetEscIsAltPrefix);

        static CObList c_keymaps;
        static CKeymap* c_pChordKeymap;
        static WORD c_kcChordPrefix;
        static BOOL c_bDisableIME;
        static BOOL c_bEscIsAltPrefix;
        static BOOL c_bImplyAltForNextKey;
        static const TCHAR c_szCtrl [];
        static const TCHAR c_szAlt [];
        static const TCHAR c_szShift [];
};

class COverridingKeymap : public CKeymap
{
public:
        COverridingKeymap(CKeymap *pkeymapBase)
                {m_pkeymapBase = pkeymapBase; m_fOverrideActive = FALSE;}

        void Activate(void)
                {m_fOverrideActive = TRUE;}
        void Deactivate(void)
                {m_fOverrideActive = FALSE;}

        virtual BOOL LookupCommand(UINT nCmdID, WORD& kcOne, WORD& kcTwo, CKeymap* pOverride = NULL);
        virtual BOOL LookupKey(WORD kc, UINT& nCmdID, CKeymap*& pKeymap);

private:
        BOOL m_fOverrideActive;
        CKeymap *m_pkeymapBase;
};

#define KCF_CONTROL     0x0800
#define KCF_SHIFT       0x1000
#define KCF_ALT         0x2000
#define KCF_ASCII       0x4000
#define KCF_CHORD       0x8000

// from keymap.cpp
extern BOOL GetCmdKeyString(UINT nCmdID, CString& str); // Active binding
extern BOOL GetCmdKeyStringAll(UINT nCmdID, CString& str); // First binding in all maps -- NOT NECESSARILY ACTIVE

extern void AddGlobalAcceleratorTable(HACCEL hAccel, WORD kcPrefix = 0);
extern void AddGlobalAcceleratorTable(LPCTSTR lpAccelID, WORD kcPrefix = 0);

extern BOOL LookupGlobalCommand(UINT nCmdID, WORD& kcOne, WORD& kcTwo);

/////////////////////////////////////////////////////////////////////////////
//      CAddOn
//              One package from each product should sub-class this and create one
//              instance to provide information for the about box.

class CAddOn
{
public:
        CAddOn();
        virtual ~CAddOn();
        CAddOn(int nSplashImageIndex);
        virtual const TCHAR* GetAboutString();
        virtual HICON GetIcon();

public:
        static POSITION GetHeadPosition()
                { return c_addons.GetHeadPosition(); }

        static CAddOn* GetNext(POSITION& pos)
                { return (CAddOn*)c_addons.GetNext(pos); }

private:
        static CPtrList c_addons;
};

/////////////////////////////////////////////////////////////////////////////
//      CProductInfo
//              One of these per product (e.g. C++, Fortran, Mac add-on, Test, etc.)

// REVIEW(davidga): what's with the arbitrary lengths?
#define PI_CCHMAX_LICENSE_NAME          53
#define PI_CCHMAX_LICENSE_COMPANY       53
#define PI_CCHMAX_SERIAL_NUMBER         24

class CProductInfo
{
public:
        enum { PIDID = ID_PID, PIDTYPE = IDRT_PID };
        CProductInfo() : m_bValid(FALSE) {}
        BOOL Create(HMODULE hModule, UINT nId = PIDID, UINT nType = PIDTYPE);

        BOOL IsValid() { return m_bValid; }

        // copy info into your strings
        BOOL CopyInfo( LPTSTR szPID = NULL, LPTSTR szUserName = NULL, LPTSTR szOrganization = NULL ) const;

        LPCTSTR GetPID() const { return m_bValid ? m_szPID : _TEXT("xxxxxxxxxxxxxxxxxxxxxxx"); }
        LPCTSTR GetUserName() const { return m_bValid ? m_szUser : NULL; }
        LPCTSTR GetOrganizationName() const { return m_bValid ? m_szOrg : NULL; }

protected:
        BOOL m_bValid;
        _TCHAR m_szPID[PI_CCHMAX_SERIAL_NUMBER];
        _TCHAR m_szUser[PI_CCHMAX_LICENSE_NAME];
        _TCHAR m_szOrg[PI_CCHMAX_LICENSE_COMPANY];
};

// Main product info

struct PRODUCT_INFO
{
        TCHAR szLicenseName[PI_CCHMAX_LICENSE_NAME];
        TCHAR szLicenseCompany[PI_CCHMAX_LICENSE_COMPANY];
        TCHAR szSerialNumber[PI_CCHMAX_SERIAL_NUMBER];
};

extern PRODUCT_INFO AFX_EXT_DATA productInfo;

/////////////////////////////////////////////////////////////////////////////
//      Module loading help

enum SETUP_TYPE
{
        stStandard,
        stMacintosh,
        stPackageInit
};

// Use this to report a missing required file.  The setup type indicates which
// setup program the user will have to run in order to get the specific DLL.
//
void ReportMissingFile(const TCHAR* szFileName,
        SETUP_TYPE setupType = stStandard);


// Use this instead of LoadLibrary() to load DLL's.  It will prevent Windows
// from displaying its ugly DLL missing message.  If bRequired is TRUE, we
// will display our own message asking the user to re-install the development
// environment.  The setup type indicates which setup program the user will
// have to run in order to get the specific DLL.
//
HINSTANCE SushiLoadLibrary(const TCHAR* szName, BOOL bRequired,
        SETUP_TYPE setupType = stStandard);

/////////////////////////////////////////////////////////////////////////////
// Interface to CTL3D32.DLL

#define WM_DLGBORDER (WM_USER+3567)
/* WM_DLGBORDER *(int FAR *)lParam return codes */
#define CTL3D_NOBORDER          0
#define CTL3D_BORDER            1

#define WM_DLGSUBCLASS (WM_USER+3568)
/* WM_DLGSUBCLASS *(int FAR *)lParam return codes */
#define CTL3D_NOSUBCLASS        0
#define CTL3D_SUBCLASS          1

/////////////////////////////////////////////////////////////////////////////
// Random MFC helpers

#ifdef _DEBUG
extern UINT SushiRegisterWindowMessage(const char* szMsg);
#define RegisterMessage SushiRegisterWindowMessage
#else
#define RegisterMessage RegisterWindowMessage
#endif

inline void SetDlgCtrlID(HWND hWnd, int nID)
{
#ifdef _WIN32
        ::SetWindowLong(hWnd, GWL_ID, nID);
#else
        ::SetWindowWord(hWnd, GWW_ID, nID);
#endif
}

// This will search all packages and MFC for a cursor...
HCURSOR AfxLoadCursor(LPCTSTR lpResourceID);
inline HCURSOR AfxLoadCursor(UINT nResourceID)
        { return AfxLoadCursor(MAKEINTRESOURCE(nResourceID)); }

/////////////////////////////////////////////////////////////////////////////
// Random C++ helpers

// Macro to keep up with the compiler syntax for declaring template specializations.
// Example of using this macro:
//                      SPECIALIZE(UINT AFXAPI HashKey(GUID key));
#if _MSC_VER < 1100
        // what we used to use ... but this has no effect in ANSI C++
        #define SPECIALIZE(T) T
#elif _MSC_VER == 1100
        // a temporary hack in the M1 compiler
        #pragma warning(disable:4231)
        #define SPECIALIZE(T) extern template T
#elif _MSC_VER > 1100
        // The true ANSI C++ syntax, which our compiler will support post-M1.
        #define SPECIALIZE(T) template<> T
#else
        #error This Can't Happen
#endif

/////////////////////////////////////////////////////////////////////////////
// Platform alignment

#if defined ( _MIPS_ ) || defined ( _X86_ ) || defined (_ALPHA_) || defined (_PPC_)
#define PACKSIZE        8
#define ALIGNMASK       (PACKSIZE - 1)
#else
    /* Deal with others as we get them */
#error "Must define PACKSIZE for this platform"
#endif

#define ALIGN(x)  ( ( (x) + ALIGNMASK ) & ( ~ALIGNMASK ) )

/////////////////////////////////////////////////////////////////////////////

#undef AFX_DATA
#define AFX_DATA NEAR

#define AfxMessageBox DsMessageBox
extern int DsMessageBox(LPCTSTR lpszText, UINT nType = MB_OK, UINT nIDHelp = 0);
extern int DsMessageBox(UINT nIDPrompt, UINT nType = MB_OK);
extern int DsMessageBox(UINT nIDPrompt, UINT nType, UINT nIDHelp);

// We used to pick these warnings up via various places from the SC++L headers. MFC (or something) no longer includes the
// SC++L headers, so we need these ourselves. In the next version, we should go back and remove these and fix the issues they relate to.
// martynl 12/06/96
// Copied from yvals.h
#pragma warning(4: 4018 4114 4146 4244 4245)
#pragma warning(4: 4663 4664 4665)
#pragma warning(disable: 4237 4284 4290 4514 4242)

#endif // __MAIN_H__
