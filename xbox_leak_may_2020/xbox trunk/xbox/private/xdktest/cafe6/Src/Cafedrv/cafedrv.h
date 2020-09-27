///////////////////////////////////////////////////////////////////////////////
//  CAFEDrv.H
//
//  Created by :            Date :
//      DavidGa                 8/13/93
//
//  Description :
//      Declaration of the CCAFEDriver class
//

#ifndef __CAFEDRV_H__
#define __CAFEDRV_H__

#ifndef __AFXWIN_H__
    #error include 'stdafx.h' before including this file for PCH
#endif

#include "settings.h"
#include "target.h"
#include "toolset.h"
#include "mainfrm.h"
#include "portview.h"
#include "hotkey.h"

#include "caferes.h"

#define theApp (*((CCAFEDriver*) AfxGetApp()))

///////////////////////////////////////////////////////////////////////////////
// CCAFEDriver class

class CSuiteDoc;

class CCAFEDriver : public CWinApp
{
// data types
public:

  enum ClientRegion { Suite, Log, Console };

public:
    CCAFEDriver();
    ~CCAFEDriver();

// CWinApp Overrides
public:
    virtual BOOL InitInstance();
    virtual int Run();
    virtual int ExitInstance();
//  virtual BOOL OnIdle(LONG lCount);

// commands
public:
    CViewportDoc* NewViewport(void);
    void ErrorMsg(UINT nIDPrompt, ...);

	void ErrorMsg(LPCSTR szMsg, ...);
	void Message(LPCSTR szMsg, ...);

// operations
public:
//  CToolset* GetToolset(void)                      { ASSERT(m_pToolset); return m_pToolset; }
    CSettings* GetSettings(void)                    { ASSERT(m_pstngCAFEDrv); return m_pstngCAFEDrv; }
    CSettings* GetLogSettings(void)                 { ASSERT(m_pstngLog); return m_pstngLog; }
    CSettings* GetCmdLine(void)                     { ASSERT(m_pstngCmdLine); return m_pstngCmdLine; }
	//in case we don't have a StepWnd (ie -hotkey_thread:false)
	CWnd* GetStepWnd(void)						{ return ((m_bHotKey) ? m_pthreadHotKey->GetHKWnd() : NULL); }

    CToolset* NewToolset(HANDLE hOutput);
    BOOL DeleteToolset(CToolset* pToolset);
	HICON  hIfail, hIpass ;

	BOOL IsOnWin95(void)							{ return m_bOnWin95; }

    LPCSTR GetDriverDir(void)                       { ASSERT(!m_strDriverDir.IsEmpty()); return m_strDriverDir; }
  BOOL DocToRegion(CDocument *pDoc, ClientRegion eRegion);
  void CallRunSuite(CSuiteDoc *pSuiteDoc);

  CPlatform *CreateCurrentPlatform(void); // NOTE: you're responsible for freeing the resulting ptr
    CSettings* ReadCmdLineSwitches(CString strCmdLine);

protected:
    BOOL GetNextSwitch(CString &strCmdLine, CString &strSwitch, CString &strValue);
    CSettings* InitCAFEDrvSettings(void);
    CSettings* InitLogSettings(void);
    BOOL InitWndSettings(void);
    CSettings* InitSingleStepSettings(void);
  BOOL MoveToRegion(CWnd *pWnd, ClientRegion eRegion);

	BOOL DeleteKey(LPCSTR pszKeyName);
	void DeleteSubKeys(HKEY hKey);

private:
    void LoadProfileSettings(void);
    void SaveProfileSettings(void);
    
// data
private:    
    CSettings* m_pstngCmdLine;
    CSettings* m_pstngCAFEDrv;
    CSettings* m_pstngLog;
    CSettings* m_pstngSingleStep;

	BOOL	m_bHotKey;

    CString m_strDriverDir;
    CString m_strWorkingDir;

    CString m_strHelpFile;

    // key name
    CString m_strKeyName;

  CSuiteDoc *m_pExecutingSuite;
  HANDLE m_hSuiteReadyEvent, m_hTerminationEvent;

	BOOL m_bOnWin95;

    // REVIEW(briancr): in order to use the old single step window, I've added this code
    CHotKeyThread* m_pthreadHotKey;
    CWinThread* m_threadSuite;

	HANDLE m_hmutexStartup; //mutex to restrict the instances of DRV, CMD, MON

    UINT m_idLagTimer;      // Lag timer ID.

    void DoOnFileNew(void) { OnFileNew(); }

// Attributes
public:
    inline CMainFrame* GetMainFrame(void) { return (CMainFrame*)m_pMainWnd; }

protected:

  unsigned long SuiteExecutionLoop(void);
  static UINT WrapSuiteExecutionLoop(void *pThis) { return ((CCAFEDriver*)pThis)->SuiteExecutionLoop(); }

// Generated message map functions
protected:
    //{{AFX_MSG(CCAFEDriver)
    afx_msg void OnAppAbout();
    afx_msg void OnOptionsSingleStep();
    afx_msg void OnUpdateOptionsSingleStep(CCmdUI* pCmdUI);
    afx_msg void OnOptionsBreakStep();
    afx_msg void OnOptionsToolset();
    afx_msg void OnViewLog();
    afx_msg void OnOptionSave(); //REVIEW(chriskoz) TEMP: hooked up the machine name. only a proof of concept
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
};

///////////////////////////////////////////////////////////////////////////////
// GetTestDriver() macro

#define GetTestDriver()     ((CCAFEDriver*)AfxGetApp())

///////////////////////////////////////////////////////////////////////////////
// this class and function are used for slowing down the suite thread

// the address of a global object of this clas is passed to the timer function (pslow_info).
// this object is declared in cafedrv.cpp and initialized in CCAFEDriver::InitInstance() after
// the suite thread is created.

class SlowSuiteInfo
    {
    public:
		HANDLE m_hthread;
//        CWinThread *m_pSuiteThread;     // the testing thread
        int m_delay;                    // how long to delay it
    };

// this is the timer function which periodically suspends the suite thread for a user-supplied
// amount of time, and then resumes it.
void CALLBACK SlowSuiteTimeProc(UINT IDEvent, UINT uRsrv, DWORD pslow_info, DWORD dwRsrv1, DWORD dwRsrv2);

/////////////////////////////////////////////////////////////////////////////
// CStepBreak dialog
class CStepBreak : public CDialog
{
// Construction
public:
	CStepBreak(int CurrentStep,CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CStepBreak)
	enum { IDD = IDD_StepBreak };
	int		m_iStepNumber;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CStepBreak)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CStepBreak)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#endif //__CAFEDRV_H__
