//
// profdlg.h
//
// Defines CProfileDlg class, these are
// dialogs to obtain information that we don't have but need to complete
// a requested operation.
//
// Implementation is in : profdlg.cpp
//
// History:
// Date				Who			What
// 09/03/96			Winslowf	created
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _PROGDLG_H_
#define _PROGDLG_H_

#include "resource.h"
//#include "prjconfg.h"
#include <dlgbase.h>		// C3dDialog
#include <srcapi.h>



///////////////////////////////////////////////////////////////////////////////
// CProfileDlg class
// definition for profiler dialog
///////////////////////////////////////////////////////////////////////////////

class CProfileDlg: public C3dDialog
{
	// Construction
public:
	CProfileDlg(BOOL bExecute = FALSE, CWnd * pParent = NULL);

	// Dialog Data
	//{{AFX_DATA(CProfileDlg)
	enum { IDD = IDD_PROFILE };
	static int m_btnIndex;
	static CString m_strComboCustom;
	static CString m_strEditAdvanced;
	//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CJavaDebugInfoDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
	enum PROFOPTIONS{FNTIMING, FNCOVERAGE, LNCOVERAGE, MERGE, CUSTOM, FNSAMPLING};
	
	void SetupPathFileName();
	void GetDebuggeeCommandLine(PSTR CommandLine, UINT size);

	CString strArgPrep1();
	CString strArgPrep2();
	CString strArgProfile();
	CString strArgPlist();
	CString strArgCustom();

	UINT Spawn(CStringList *slSpawnCmds);
	void RunProfiler();

protected:
	//{{AFX_MSG(CAboutDlg)
	afx_msg void OnPROFnTiming();
	afx_msg void OnPROFMerge();
	afx_msg void OnPROFnCover();
	afx_msg void OnPROFLnCover();
	afx_msg void OnPROFCustom();
	afx_msg void OnPROFBrowse();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()

public:
	//{{AFX_MSG(CJavaDebugInfoDlg)
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	// profiler dialog state retrieval/store from an archive
	static BOOL LoadProfOptStore(CArchive & archive);
	static BOOL SaveProfOptStore(CArchive & archive);

// private data
private:
	BOOL m_bExecute;
	CString m_strExe;
	CString m_strDir;
	CString m_strCmdLine;

	CString m_strPrep;
	CString m_strPlist;
	CString m_strProfile;
	CString m_strExtPbi;
	CString m_strExtMap;
};


class CBrowseDlg: public C3dFileDialog
{
// Construction
public:
	CBrowseDlg();
	~CBrowseDlg();

// Implementation
	int DoModal();
	CString GetFileName();

	// private data
private:
	TCHAR m_szFileBuff[_MAX_PATH];
};

extern BOOL g_fProfiling;
extern CConsoleSpawner *g_pSpawner;
BOOL FProfiling();

#endif // _PROGDLG_H_
