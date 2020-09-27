/////////////////////////////////////////////////////////////////////////////
//	fnwiztpl.h
//		File New Wizard template

#ifndef __FNWIZTPL_H__
#define __FNWIZTPL_H__

class CFileNewWizardTemplate : public CPartTemplate
{
	DECLARE_DYNAMIC(CFileNewWizardTemplate)
private:
	HICON m_hIcon;
	
public:
	// constructor
	CFileNewWizardTemplate(CPackage* pPackage, REFCLSID clsid, CString strDesc, HICON hIcon=NULL);

	// Dont call this.  You should call NewDocumentFile
	virtual CDocument*	OpenDocumentFile(LPCTSTR pszPathName, BOOL bMakeVis = TRUE);
	// Override this one.
	virtual CPartDoc	  *NewDocumentFile(LPCTSTR szFileName, IPkgProject *pProject, LPCTSTR szFolder);

	// override to get alternative icon
	virtual HICON GetIcon() { return m_hIcon; };
	virtual FileNewTabType GetTabType() { return newtemplate; };
	//
	//	BOOL CFileNewWizardTemplate::CanBeAddedToProject
	//
	//	Description:
	//		This FileNewWizard can only be added to IWebPkgProjects.
	//
	//	Arguments:
	//		IPkgProject *pProject:
	//
	//	Return (BOOL): TRUE if Project CLSID == IID_IWebPkgProject(s), FALSE otw
	//
	virtual BOOL CanBeAddedToProject(IPkgProject *pProject);
};

#endif		// __FNWIZTPL_H__

