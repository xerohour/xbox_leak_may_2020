// AutoPrj.h : header file
//
#include <utilauto.h>
#include <ObjModel\bldauto.h>
#include <ObjModel\bldguid.h>
#include "autoguid.h"
#include "bldiface.h"


class CAutoProjects;

/////////////////////////////////////////////////////////////////////////////
// CAutoProject command target

class CAutoProject : public CAutoObj
{
	friend class CAutoProjects;
	DECLARE_DYNCREATE(CAutoProject)

	CAutoProject();           // protected constructor used by dynamic creation

// Attributes
public:
	static CAutoProject* Create(HBUILDER hBld, LPCTSTR szName);

// Operations
public:
	DECLARE_DUAL_ERRORINFO();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAutoProject)
	public:
	virtual void OnFinalRelease();
	//}}AFX_VIRTUAL

// Implementation
protected:
	HBUILDER m_hBld;
	CString m_strName;
	CString m_strFullName;
	LPDISPATCH m_pConfigs;
	virtual ~CAutoProject();

	// Generated message map functions
	//{{AFX_MSG(CAutoProject)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()

	// Generated OLE dispatch map functions
	//{{AFX_DISPATCH(CAutoProject)
	afx_msg BSTR GetName();
	afx_msg BSTR GetFullName();
	afx_msg BSTR GetType();
	afx_msg LPDISPATCH GetApplication();
	afx_msg LPDISPATCH GetParent();
	afx_msg LPDISPATCH GetConfigurations();
	afx_msg void AddFile(LPCTSTR szFile, const VARIANT FAR& Reserved);
	afx_msg void AddConfiguration(LPCTSTR szConfiguration, const VARIANT FAR& Reserved);
	//}}AFX_DISPATCH
	DECLARE_DISPATCH_MAP()
	DECLARE_INTERFACE_MAP()

 	DS_BEGIN_DUAL_INTERFACE_PART(BuildProject)
		STDMETHOD(get_Name)(THIS_ BSTR FAR* Name);
		STDMETHOD(get_FullName)(THIS_ BSTR FAR* FullName);
		STDMETHOD(get_Application)(THIS_ IDispatch * FAR* Application);
		STDMETHOD(get_Parent)(THIS_ IDispatch * FAR* Parent);
	    STDMETHOD(get_Type)(THIS_ BSTR FAR* pType);
		STDMETHOD(get_Configurations)(THIS_ IConfigurations FAR* FAR* Configurations);
	    STDMETHOD(AddFile)(THIS_ BSTR szFile, VARIANT Reserved);
	    STDMETHOD(AddConfiguration)(THIS_ BSTR szConfiguration, VARIANT Reserved);
		DS_DECLARE_VTBL_PAD_10()
 	DS_END_DUAL_INTERFACE_PART(BuildProject)
 
 	DS_DECLARE_ENABLE_DUAL(CAutoProject, BuildProject)
};

/////////////////////////////////////////////////////////////////////////////
// CLinkAutoObjToProject

// This object serves as an intermediary between CProject and
//  the automation objects that depend on it.  When one goes away
//  the other is notified by this object.

class CLinkAutoObjToProject
{
public:
	void OnProjectDestroyed(CProject* pProject);
	void OnAutoObjDestroyed(CAutoObj* pAutoObj);
	void OnAutoObjCreated(CAutoObj* pAutoObj, HBUILDER hBld);

protected:
	// If both automation objects and CProject's are around, it's
	//  more frequent that the automation objects will be
	//  created/destroyed than CProject's being created and destroyed.
	//  So access is optimized for having a CAutoObj, and needing to
	//  find the linked CProject.
	CTypedPtrMap< CMapPtrToPtr, CAutoObj*, CProject* > m_PtrMap;
};

extern CLinkAutoObjToProject g_LinkAutoObjToProject;

/////////////////////////////////////////////////////////////////////////////
