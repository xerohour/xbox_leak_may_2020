// filenew.h : header file for CFileNewTabbedDialog,
//							   CNewFileDialogTab,
//							   CNewTemplateDialogTab,
//							   CNewProjectDialogTab,
//							   CNewWorkspaceDialogTab,
//							   CNewOtherDocDialogTab
//

#ifndef __FILENEW_H__
#define __FILENEW_H__

#include "resource.h"
#include "utilbld_.h"
#include "utilctrl.h"

#include <bldapi.h>
#include <prjapi.h>
#include <prjguid.h>

class CFileNewTabbedDialog;
class CNewFileDialogTab;
class CNewTemplateDialogTab;
class CNewProjectDialogTab;
class CNewWorkspaceDialogTab;
class CNewOtherDocDialogTab;

#ifdef ENABLE_LISTVIEW_MODES
enum VIEW_MODE { largeicon, smallicon, list };
#endif	// ENABLE_LISTVIEW_MODES

/////////////////////////////////////////////////////////////////////////////
// CFileNewTabbedDialog

class CFileNewTabbedDialog : public CTabbedDialog
{
public:

	CFileNewTabbedDialog(BOOL bUseSavedData = FALSE, BOOL bForceAddToProject = FALSE, BOOL bForceShowProjectTab = FALSE);
	~CFileNewTabbedDialog();

// Operations
public:
	virtual void OnOK();
	virtual void OnCancel();

protected:
	BOOL IsCommitting() const { return(m_fCommitting); }

	void SetType(CPartTemplate::FileNewTabType nType) { m_Type = nType; }
	void SetTemplate(CDocTemplate * pTemplate) { m_pTemplate = pTemplate; }
	void SetFileName(const CString &strFileName) { m_strFileName = strFileName; }
	void SetPathName(const CString &strPathName) { m_strPathName = strPathName; }
	void SetAddToProject(BOOL fAddToProject) { m_fAddToProject = fAddToProject; }
	void SetProject(IPkgProject * pProject) { m_pProject = pProject; }
	void SetProjectProvider(IPkgProjectProvider *pProjectProvider) { m_pProjectProvider = pProjectProvider; }
	void SetProjectType(DWORD dwProjectType) { m_dwProjectType = dwProjectType; }
	void SetCreateNew(BOOL fCreateNew) { m_fCreateNewWorkspace = fCreateNew; }

public:
	CPartTemplate::FileNewTabType GetType() const { return(m_Type); }
	CDocTemplate *GetTemplate() const { return(m_pTemplate); }
	CString GetFileName() const { return(m_strFileName); }
	CString GetPathName() const { return(m_strPathName); }
	IPkgProject *GetProject() const { return(m_pProject); }
	IPkgProjectProvider *GetProjectProvider() const { return(m_pProjectProvider); }
	DWORD GetProjectType() const { return (m_dwProjectType); }
	BOOL GetCreateNew() const { return(m_fCreateNewWorkspace); }

	DECLARE_DYNAMIC(CFileNewTabbedDialog);

public:
	CStringList m_strlistProjects;
	CPtrList m_ptrlistBuilders;
	IPkgProject *m_pActiveProject;

private:
	CPartTemplate::FileNewTabType m_Type;
	CDocTemplate *m_pTemplate;
	CString m_strFileName;
	CString m_strPathName;
	BOOL m_fAddToProject;
	IPkgProject *m_pProject;

	IPkgProjectProvider *m_pProjectProvider;
	DWORD m_dwProjectType;
	BOOL m_fCreateNewWorkspace;

	BOOL m_fCommitting;

	static int c_nSelectTab;

	friend class CNewFileDialogTab;
	friend class CNewTemplateDialogTab;
	friend class CNewProjectDialogTab;
	friend class CNewWorkspaceDialogTab;
	friend class CNewOtherDocDialogTab;
};

/////////////////////////////////////////////////////////////////////////////
// CNewFileDialogTab

class CNewFileDialogTab : public CDlgTab
{
// Construction
public:
	
// Dialog data
	//{{AFX_DATA(CNewFileDialogTab)
	enum { IDD = IDDP_FILENEW_FILES };
	CListCtrl m_FileTypes;
	static int m_nSelected;
	static BOOL m_fAddToProject;
	static int m_nIndex;
	static CString m_strFileName;
	static CString m_strPathName;
	static int m_nFocusCtrlId;
	//}}AFX_DATA

	CNewFileDialogTab(BOOL bUseSavedData = FALSE, BOOL bForceAddToProject = FALSE);

	virtual BOOL Activate(CTabbedDialog* pParentWnd, CPoint position);
	virtual void Deactivate(CTabbedDialog* pParentWnd);

	virtual BOOL ValidateTab();
	virtual void CommitTab();
	virtual void CancelTab();

protected:
	//{{AFX_MSG(CNewFileDialogTab)
	virtual BOOL	OnInitDialog();
	afx_msg void	OnBrowse();
#ifdef ENABLE_LISTVIEW_MODES
	afx_msg void	OnClickLargeIconView();
	afx_msg void	OnClickSmallIconView();
	afx_msg void	OnClickListView();
#endif	// ENABLE_LISTVIEW_MODES
	afx_msg void	OnClickAddToProject();
	afx_msg void	OnDblClkType(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void	OnSelectType(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void	OnChangeName();
	afx_msg void	OnChangeLocation();
	afx_msg void	OnProjectChange();
	//}}AFX_MSG	

	// our message map functions
	DECLARE_MESSAGE_MAP()
	DECLARE_DYNCREATE(CNewFileDialogTab)

// Implementation
protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

private:
	void EnableButtons();

	BOOL m_fIsInitialized;
	BOOL m_fDoubleClicked;
	BOOL m_fForceAddToProj;
	BOOL m_fAddToProjectPrev;
//	CImageList m_imageListNormal;
	CImageList m_imageListSmall;

#ifdef ENABLE_LISTVIEW_MODES
	static VIEW_MODE c_ViewMode;
#endif	// ENABLE_LISTVIEW_MODES
};

/////////////////////////////////////////////////////////////////////////////
// CNewTemplateDialogTab

class CNewTemplateDialogTab : public CDlgTab
{
// Construction
public:
	
// Dialog data
	//{{AFX_DATA(CNewTemplateDialogTab)
	enum { IDD = IDDP_FILENEW_TEMPLATES };
	CListCtrl m_TemplateTypes;
	static int m_nSelected;
	static BOOL m_fAddToProject;
	static int m_nIndex;
	static CString m_strFileName;
	static CString m_strPathName;
	static int m_nFocusCtrlId;
	//}}AFX_DATA

	CNewTemplateDialogTab(BOOL bUseSavedData = FALSE, BOOL bForceAddToProject = FALSE);

	virtual BOOL Activate(CTabbedDialog* pParentWnd, CPoint position);
	virtual void Deactivate(CTabbedDialog* pParentWnd);

	virtual BOOL ValidateTab();
	virtual void CommitTab();
	virtual void CancelTab();

protected:
	//{{AFX_MSG(CNewTemplateDialogTab)
	virtual BOOL	OnInitDialog();
	afx_msg void	OnBrowse();
#ifdef ENABLE_LISTVIEW_MODES
	afx_msg void	OnClickLargeIconView();
	afx_msg void	OnClickSmallIconView();
	afx_msg void	OnClickListView();
#endif	// ENABLE_LISTVIEW_MODES
	afx_msg void	OnClickAddToProject();
	afx_msg void	OnDblClkType(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void	OnSelectType(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void	OnChangeName();
	afx_msg void	OnChangeLocation();
	afx_msg void	OnProjectChange();
	//}}AFX_MSG	

	// our message map functions
	DECLARE_MESSAGE_MAP()
	DECLARE_DYNCREATE(CNewTemplateDialogTab)

// Implementation
protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

private:
	void EnableButtons();

	BOOL m_fIsInitialized;
	BOOL m_fDoubleClicked;
	BOOL m_fForceAddToProj;
	BOOL m_fAddToProjectPrev;
//	CImageList m_imageListNormal;
	CImageList m_imageListSmall;

#ifdef ENABLE_LISTVIEW_MODES
	static VIEW_MODE c_ViewMode;
#endif	// ENABLE_LISTVIEW_MODES
};

/////////////////////////////////////////////////////////////////////////////
// CNewProjectDialogTab

class CNewProjectDialogTab : public CDlgTab
{
// Construction
public:
	
// Dialog data
	//{{AFX_DATA(CNewProjectDialogTab)
	enum { IDD = IDDP_FILENEW_PROJECTS };
	CListCtrl m_ProjectTypes;
	static int m_nSelected;
	static BOOL m_fCreateNew;
	static BOOL m_fDependency;
	static int m_nIndexProject;
	static int m_nIndexPlatform;
	static CString m_strFileName;
	static CString m_strPathName;
	static int m_nFocusCtrlId;
	//}}AFX_DATA

	CNewProjectDialogTab(BOOL bUseSavedData = FALSE, BOOL bForceAddToWorkspace = FALSE);

	virtual BOOL Activate(CTabbedDialog* pParentWnd, CPoint position);
	virtual void Deactivate(CTabbedDialog* pParentWnd);

	virtual BOOL ValidateTab();
	virtual void CommitTab();
	virtual void CancelTab();

	virtual void SetAppropriateLocation();

protected:
	//{{AFX_MSG(CNewProjectDialogTab)
	virtual BOOL	OnInitDialog();
	afx_msg void	OnBrowse();
#ifdef ENABLE_LISTVIEW_MODES
	afx_msg void	OnClickLargeIconView();
	afx_msg void	OnClickSmallIconView();
	afx_msg void	OnClickListView();
#endif	// ENABLE_LISTVIEW_MODES
	afx_msg void	OnClickCreateNew();
	afx_msg void	OnClickAddToCurrent();
	afx_msg void	OnClickDependencyOf();
	afx_msg void	OnDblClkType(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void	OnSelectType(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void	OnChangeName();
	afx_msg void	OnChangeLocation();
	//}}AFX_MSG	

	// our message map functions
	DECLARE_MESSAGE_MAP()
	DECLARE_DYNCREATE(CNewProjectDialogTab)

// Implementation
protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

private:
	void EnableButtons();

	BOOL m_fIsInitialized;
	BOOL m_fDoubleClicked;
	BOOL m_fUserChangedLocation;
	BOOL m_fForceAddToWks;
	BOOL m_fCreateNewPrev;
//	CImageList m_imageListNormal;
	CImageList m_imageListSmall;

	static CString m_strPrevName;

	CCheckList m_Platforms;
	static	int m_nIndexSave;

#ifdef ENABLE_LISTVIEW_MODES
	static VIEW_MODE c_ViewMode;
#endif	// ENABLE_LISTVIEW_MODES
};

/////////////////////////////////////////////////////////////////////////////
// CNewWorkspaceDialogTab

class CNewWorkspaceDialogTab : public CDlgTab
{
// Construction
public:
	
// Dialog data
	//{{AFX_DATA(CNewWorkspaceDialogTab)
	enum { IDD = IDDP_FILENEW_WORKSPACES };
	CListCtrl m_WorkspaceTypes;
	static int m_nSelected;
	static CString m_strFileName;
	static CString m_strPathName;
	static int m_nFocusCtrlId;
	//}}AFX_DATA

	CNewWorkspaceDialogTab(BOOL bUseSavedData = FALSE);

	virtual BOOL Activate(CTabbedDialog* pParentWnd, CPoint position);
	virtual void Deactivate(CTabbedDialog* pParentWnd);

	virtual BOOL ValidateTab();
	virtual void CommitTab();
	virtual void CancelTab();

protected:
	//{{AFX_MSG(CNewWorkspaceDialogTab)
	virtual BOOL	OnInitDialog();
	afx_msg void	OnBrowse();
#ifdef ENABLE_LISTVIEW_MODES
	afx_msg void	OnClickLargeIconView();
	afx_msg void	OnClickSmallIconView();
	afx_msg void	OnClickListView();
#endif	// ENABLE_LISTVIEW_MODES
	afx_msg void	OnDblClkType(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void	OnSelectType(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void	OnChangeName();
	afx_msg void	OnChangeLocation();
	//}}AFX_MSG	

	// our message map functions
	DECLARE_MESSAGE_MAP()
	DECLARE_DYNCREATE(CNewWorkspaceDialogTab)

// Implementation
protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

private:
	void EnableButtons();

	BOOL m_fIsInitialized;
	BOOL m_fDoubleClicked;
//	CImageList m_imageListNormal;
	CImageList m_imageListSmall;
	
	static CString m_strPrevName;

#ifdef ENABLE_LISTVIEW_MODES
	static VIEW_MODE c_ViewMode;
#endif	// ENABLE_LISTVIEW_MODES
};

/////////////////////////////////////////////////////////////////////////////
// CNewOtherDocDialogTab

class CNewOtherDocDialogTab : public CDlgTab
{
// Construction
public:
	
// Dialog data
	//{{AFX_DATA(CNewOtherDocDialogTab)
	enum { IDD = IDDP_FILENEW_OTHERDOCS };
	CListCtrl m_OtherDocTypes;
	static int m_nSelected;
	static BOOL m_fAddToProject;
	static int m_nIndex;
	static CString m_strFileName;
	static CString m_strPathName;
	static int m_nFocusCtrlId;
	//}}AFX_DATA

	CNewOtherDocDialogTab(BOOL bUseSavedData = FALSE, BOOL bForceAddToProject = FALSE);

	virtual BOOL Activate(CTabbedDialog* pParentWnd, CPoint position);
	virtual void Deactivate(CTabbedDialog* pParentWnd);

	virtual BOOL ValidateTab();
	virtual void CommitTab();
	virtual void CancelTab();

protected:
	//{{AFX_MSG(CNewOtherDocDialogTab)
	virtual BOOL	OnInitDialog();
	afx_msg void	OnBrowse();
#ifdef ENABLE_LISTVIEW_MODES
	afx_msg void	OnClickLargeIconView();
	afx_msg void	OnClickSmallIconView();
	afx_msg void	OnClickListView();
#endif	// ENABLE_LISTVIEW_MODES
	afx_msg void	OnClickAddToProject();
	afx_msg void	OnDblClkType(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void	OnSelectType(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void	OnChangeName();
	afx_msg void	OnChangeLocation();
	afx_msg void	OnProjectChange();
	//}}AFX_MSG	

	// our message map functions
	DECLARE_MESSAGE_MAP()
	DECLARE_DYNCREATE(CNewOtherDocDialogTab)

// Implementation
protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

private:
	void EnableButtons();

	BOOL m_fIsInitialized;
	BOOL m_fDoubleClicked;
	BOOL m_fForceAddToProj;
	BOOL m_fAddToProjectPrev;
//	CImageList m_imageListNormal;
	CImageList m_imageListSmall;

#ifdef ENABLE_LISTVIEW_MODES
	static VIEW_MODE c_ViewMode;
#endif	// ENABLE_LISTVIEW_MODES
};

/////////////////////////////////////////////////////////////////////////////

#endif	// __FILENEW_H__
