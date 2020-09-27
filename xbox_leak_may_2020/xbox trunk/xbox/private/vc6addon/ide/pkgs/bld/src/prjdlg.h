// prjdlg.h : header file
//

#ifndef _INCLUDE_PRJDLG_H
#define _INCLUDE_PRJDLG_H

#ifndef __UTILBLD__H__
#include <utilbld_.h>
#endif

// Values to switch() on after CPromptDlg's DoModal.  Stored in CPromptDlg::m_nCreator
enum
{
		CREATOR_APPWIZ,		// We should start up AppWizard steps
		CREATOR_IDE,		// The user is creating an empty IDE project (no appwiz)
		CREATOR_CANCEL		// The user hit cancel	
};

#define MAX_PLATFORMS	8

/////////////////////////////////////////////////////////////////////////////
// CNewProjectCopyDlg dialog

class CProjectTargetRec : public CObject
{
// Attributes:
public:
	CString m_strName;
	CString m_strType;
	CString m_strUIType;
	CString m_strPlatform;
	BOOL m_bDebug;
	BOOL m_bSupported;
};

/////////////////////////////////////////////////////////////////////////////

class CNewProjectCopyDlg : public C3dDialog
{
// Construction
public:
	CNewProjectCopyDlg(LPCTSTR pszProject, CWnd* pParent = NULL);   // standard constructor
	~CNewProjectCopyDlg();

// Dialog Data
	//{{AFX_DATA(CNewProjectCopyDlg)
	enum { IDD = IDD_INSERTPROJ_COPY };
	CComboBox m_TargetList;
	CComboBox m_Platforms;
	//}}AFX_DATA

// Implementation
protected:
	CString m_strProjName;
	CProject *m_pProject;
	CProjectTargetRec *m_pTarget;
 	CObList m_ProjectTargets;
	CString m_strProject;

	BOOL m_bTrackBasedOn;		// Track based on field for config name

	BOOL OnInitDialog();
	void OnSelchangeCopyFrom();
	void OnSelchangePlatform();
	void OnOK();
	void OnChangeProjName();
	BOOL IsDebug(ConfigurationRecord *pConfig);
	BOOL AddSelectedTarget();
	void DoDataExchange(CDataExchange* pDX);

	void UpdateTargetNameText ();
	CString GetPlatformName ();
	CString ConstructTargetName (const CString &strPlatform);

	DECLARE_MESSAGE_MAP()
};


#endif   // _INCLUDE_PRJDLG_H
