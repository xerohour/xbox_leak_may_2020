#ifndef __CONFIGS_H__
#define __CONFIGS_H__

#include <afxcmn.h>
#include <utilctrl.h>

/////////////////////////////////////////////////////////////////////////////
// The "Project Configurations..." dialog box.
class CProjectConfigurationsDlg : public C3dDialog
{
// Construction
public:
	CProjectConfigurationsDlg(CWnd* pParent = NULL);
	virtual ~CProjectConfigurationsDlg();

// Dialog Data
protected:
	CTreeCtrl	m_ConfigurationsTree;
	int			m_nFileset;

// Implementation
protected:
	virtual void DoDataExchange(CDataExchange* pDX);

	// Message map functions.
	virtual BOOL OnInitDialog();
	afx_msg void OnAddConfiguration();
	afx_msg void OnRemove();
	afx_msg void OnSelectConfiguration(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnOK ();
	DECLARE_MESSAGE_MAP()

	// Helpers
	void EnableButtons();
	void FillTree();
	void CalculateInvalidDependencies(HBLDTARGET hConfig, CStringList& rslConfigs);
	BOOL IsConfig(HTREEITEM hItem);
	HBLDTARGET GetConfig(HTREEITEM hItem);
	LPCTSTR GetConfigName(HTREEITEM hItem);
	HFILESET GetProject(HTREEITEM hItem);
	LPCTSTR GetProjectName(HTREEITEM hItem);
};

#endif
