/////////////////////////////////////////////////////////////////////////////
//      SHELL.H
//              Shell implementation header.

#ifndef __SHELL_H__
#define __SHELL_H__

#include "bwizmgr.h"
#include <shlapi_.h>

/////////////////////////////////////////////////////////////////////////////
//      CBRecentFileList
//
// Overloads UpdateMenu to support Command bar menus. Used directly by the
// project list and indirectly by CTypedRecentFileList
//              CRecentFileList
//                      CBRecentFileList
//                              CTypedRecentFileList
// 

class CBRecentFileList : public CRecentFileList
{
public:
	CBRecentFileList(UINT nStart, LPCTSTR lpszSection,
					LPCTSTR lpszEntryFormat, int nSize, 
					int nMaxDispLen = AFX_ABBREV_FILENAME_LEN, int nShortCount=4)
	:       CRecentFileList(nStart, lpszSection,lpszEntryFormat, nSize, nMaxDispLen)
	{ m_nLastSize=nSize; m_nShortCount=nShortCount; }


	~CBRecentFileList() {}

	virtual void SetSize(int nSize);
	// sets the start accelerator number
	virtual void SetStart(int nStart);
	// sets the number of accelerators to issue in this list
	virtual void SetShortCount(int nStart);
	// returns the number of items in the list
	virtual int GetSize(void) const { return m_nSize; };
	// returns the number of items in the list
	virtual int GetShortCount(void) const { return m_nShortCount; };

	// Need to use CmdBar code to update menu
	virtual void UpdateMenu(CCmdUI* pCmdUI);        

	// this is the number of items that were put in the menu last time around
	int m_nLastSize;
	// Number of items to be given shortcuts
	int m_nShortCount;
};

/////////////////////////////////////////////////////////////////////////////
//      CTypedRecentFileList
//              Recent file list that saves the CDocTemplate type used to open it.
class CTypedRecentFileList : public CBRecentFileList
{
public:
	CTypedRecentFileList(UINT nStart, LPCTSTR lpszSection, 
		LPCTSTR lpszEntryFormat, LPCTSTR lpszEntryTypeFormat, int nSize, 
		int nMaxDispLen = AFX_ABBREV_FILENAME_LEN, int nShortCount=4);
	~CTypedRecentFileList();

// Attributes
	CLSID* m_arrTypes;
	CString m_strEntryTypeFormat;

// Operations
	CDocTemplate* GetTypeAt(int nIndex);
	virtual void Add(LPCTSTR lpszPathName, CDocTemplate* pTemplate);
	virtual void ReadList();        // reads from registry file
	virtual void WriteList();       // writes to registry file
	// sets the number of entries in the list
	virtual void SetSize(int nSize);
};
/////////////////////////////////////////////////////////////////////////////
//      CInternalApp
//              defined in intrnlap.cpp
//

class CInternalApp : public CCmdTarget
{
public:
	CBuilderWizardManager *GetBuilderWizardManager();       

private:
	CBuilderWizardManager m_builderWizardManager;
};


/////////////////////////////////////////////////////////////////////////////
//      CVShellPackage

class CVshellPackage : public CPackage
{
public:
	CVshellPackage();
	~CVshellPackage();

	enum { PKS = PKS_COMMANDS | PKS_INIT | PKS_QUERYEXIT | 
			PKS_INTERFACES | PKS_PREEXIT | PKS_AUTOMATION};

	void OnUpdateCommandUI(CCmdUI* pCmdUI);
	void OnUpdateCommandUI_View(CCmdUI* pCmdUI);
	void OnUpdateCommandUI_Tools(CCmdUI* pCmdUI);
	void OnUpdateCommandUI_MSOnTheWeb(CCmdUI* pCmdUI);
	void OnCommand_Tools(UINT nID);

	void RegisterTypeLibsIfNecessary(BOOL bForceFullRegistration = FALSE);
	void GetAllApplicationExtensions();

	virtual BOOL OnInit();
	virtual BOOL CanExit();
	virtual void OnPreExit();
	virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra, 
		AFX_CMDHANDLERINFO* pHandlerInfo);

//      DWORD GetIdRange(RANGE_TYPE rt);
//      LPCTSTR CVshellPackage::GetHelpFileName(HELPFILE_TYPE helpfileType);

	// Dynamic Command key strings for tooltips
	virtual UINT CmdIDForToolTipKey(UINT nIDCmdOriginal);

// Dockable window interface
	virtual HGLOBAL GetDockInfo();
	virtual HWND GetDockableWindow(UINT nID, HGLOBAL hglob);
	virtual HGLOBAL GetToolbarData(UINT nID);
	virtual BOOL AskAvailable(UINT nID);

	virtual void GetOpenDialogInfo(SRankedStringID** ppFilters, 
		SRankedStringID** ppEditors, int nOpenDialog);
	virtual void AddOptDlgPages(class CTabbedDialog* pOptionsDialog, OPTDLG_TYPE type);

	virtual POPDESC *GetMenuDescriptor(UINT nId);
	virtual BOOL IsMenuVisible(UINT nId);

protected:
	// Lists used to collect information about type libraries and
	//  top-level named items (for VBS) from each package

	// Main list of top-level named items for each package, used to implement
	//  IAutomationItems.  This is a private interface used exclusively by
	//  devaut1 for adding named items to VBS.
	CTypedPtrList< CPtrList, CAutomationItem* > m_ptrlAutomationItems;
	void RegisterTypeLib(CPackage* pPackage, BOOL bForceFullRegistration,
									 BOOL bGetApplicationExtensions);
	void AddNewAutoItem(const GUID* pTlid, WORD wMajorVersion, WORD wMinorVersion,
									LPCTSTR szName, const CLSID* pClsid);

	// Called by the IRecordNotifyShell interface to set theApp's members
	void SetShouldRecordText(BOOL bShouldRecordText)
		{ theApp.m_bShouldRecordText = bShouldRecordText; }
	void SetRecordState(int nRecordState)
		{ theApp.m_nRecordState = nRecordState; }

	//{{AFX_MSG(CVshellPackage)
	afx_msg void OnFileNew();
	afx_msg void OnFileNewInProject();
	afx_msg void OnFileOpen();
	afx_msg void OnFileSaveAll();
	afx_msg void OnFileSaveAllExit();
	afx_msg void OnAppAbout();
	afx_msg void OnHelpPSS();
	afx_msg void OnHelpMSOnTheWeb(UINT nID);
	afx_msg void OnHelpExtHelp();
	afx_msg void OnHelpTip();
	afx_msg void OnToolbarOptions();
	afx_msg void OnCustomize();
	afx_msg void OnToolbarCustomize();
	afx_msg void OnOptions();
	afx_msg void OnInsertFiles();
	afx_msg void OnWindowHide();
	afx_msg void OnToggleMDI();
	afx_msg void OnCmdRepeat();
	afx_msg void OnCmdRepeat0();
	afx_msg void OnCmdRepeat1();
	afx_msg void OnCmdRepeat2();
	afx_msg void OnCmdRepeat3();
	afx_msg void OnCmdRepeat4();
	afx_msg void OnCmdRepeat5();
	afx_msg void OnCmdRepeat6();
	afx_msg void OnCmdRepeat7();
	afx_msg void OnCmdRepeat8();
	afx_msg void OnCmdRepeat9();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()

protected:
	BEGIN_INTERFACE_PART(AutomationItems, IAutomationItems)
		STDMETHOD(GetAutomationItems)(CAutomationItem* pAutomationItems, DWORD* pdwNumItems);
		STDMETHOD(GetIDispatch)(LPCTSTR szItemName, IDispatch** ppDispatch);
	END_INTERFACE_PART(AutomationItems)
	BEGIN_INTERFACE_PART(RecordNotifyShell, IRecordNotifyShell)
		STDMETHOD(OnStartRecord)(THIS);
		STDMETHOD(OnStopRecord)(THIS);
		STDMETHOD(OnPauseRecord)(THIS);
		STDMETHOD(OnResumeRecord)(THIS);
		STDMETHOD(SetShouldRecordText)(THIS_ BOOL bShouldRecordText);
	END_INTERFACE_PART(RecordNotifyShell)
	DECLARE_INTERFACE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
//      Global dimensions

typedef struct tagSCREEN_METRIC
{
    int cxBorder, cyBorder;
    int cxFrame, cyFrame;
    int cxToolBorder, cyToolBorder;
	int cxToolCaption, cyToolCaption;
	int cxToolCapBorder, cyToolCapBorder;
	int cxToolCapGap, cyToolCapGap;			// gap between tool caption buttons
	int cxToolCapButton, cyToolCapButton;
    int nSplitter;
} SCREEN_METRIC;

extern SCREEN_METRIC g_mt;

extern void InitMetrics();

/////////////////////////////////////////////////////////////////////////////
//      General utility functions.

HINSTANCE GetResourceHandle();

//  Set main product registration info from PID resource in the 
//  specified module (normally the EXE).

BOOL InitProductId(HMODULE hMod);

//////////////////////////////////////////////////////////////////////////////
// Help

BOOL GetPSSHelpFile(CString & strHelpFile);
void ReleaseHelpInterface();

// control help
LPCTSTR HelpFileFromID(DWORD dwId, UINT type);

// Ext Help
BOOL ShowExtHelp(); // expose UI?
BOOL UseExtHelp();  // use ext help?
BOOL GetExtHelpFile(CString & strHelpFile);
BOOL HelpIndexExt(LPCTSTR szKeyword = NULL);
BOOL HelpContentsExt();
BOOL HelpOnKeywordExt(LPCTSTR szKeyword = NULL);
BOOL SetExtHelp(BOOL b = TRUE);
BOOL ToggleExtHelp();

// initexit.cpp strings used by bwizmgr and bwizdlg do something similar to
// GetRegKeyName
// 
extern const TCHAR g_szKeyRoot[];
extern const TCHAR g_szCompanyName[];

//////////////////////////////////////////////////////////////////////////////
// Util

// Wrappers for the winmm functions

BOOL MyPlaySound(LPCSTR pszSound, HMODULE hmod, DWORD fdwSound);
BOOL ShsndPlaySound(LPCSTR lpszSound, UINT fuSound);

/////////////////////////////////////////////////////////////////////////////
//
// GetFirstChildWindowWithID
//
//	A breath first version of CWnd::GetDescendentWindow.
//
// This function is in util.h.
//
class CPartView ;
CPartView* GetFirstChildWindowWithID(HWND hWnd, int nID, BOOL bOnlyPerm = FALSE) ;

#endif  // __SHELL_H__
