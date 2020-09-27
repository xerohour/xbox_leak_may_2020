// AutoCfgs.h : header file
//
#include <utilauto.h>
#include <ObjModel\bldauto.h>
#include <ObjModel\bldguid.h>
#include "autoguid.h"
#include "bldiface.h"



/////////////////////////////////////////////////////////////////////////////
// CAutoConfigurations command target

class CAutoConfigurations : public CAutoObj
{
	DECLARE_DYNCREATE(CAutoConfigurations)

	CAutoConfigurations();           // protected constructor used by dynamic creation

// Attributes
public:
	static CAutoConfigurations* Create(HBUILDER hBld, LPCTSTR szProject);
	virtual void AssociatedObjectReleased();

// Operations
public:
	DECLARE_DUAL_ERRORINFO();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAutoConfigurations)
	public:
	virtual void OnFinalRelease();
	//}}AFX_VIRTUAL

// Implementation
protected:
	CDispatchList* m_pPtrlConfigs;
	HBUILDER m_hBld;
	CString m_strProject;
	virtual ~CAutoConfigurations();

	virtual void RefreshConfigsInList();
	void ReleaseConfigsInList();

	// Generated message map functions
	//{{AFX_MSG(CAutoConfigurations)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()

	// Generated OLE dispatch map functions
	//{{AFX_DISPATCH(CAutoConfigurations)
	afx_msg LPDISPATCH GetApplication();
	afx_msg long GetCount();
	afx_msg LPDISPATCH GetParent();
	afx_msg LPDISPATCH Item(const VARIANT FAR& Index);
	//}}AFX_DISPATCH
	afx_msg LPUNKNOWN GetNewEnum();
	DECLARE_DISPATCH_MAP()
	DECLARE_INTERFACE_MAP()
 	DS_BEGIN_DUAL_INTERFACE_PART(Configurations)
		STDMETHOD(get_Application)(THIS_ IDispatch * FAR* Application);
		STDMETHOD(get_Count)(THIS_ long FAR* Count);
		STDMETHOD(get_Parent)(THIS_ IBuildProject FAR* FAR* Parent);
		STDMETHOD(get__NewEnum)(THIS_ IUnknown * FAR* _NewEnum);
		STDMETHOD(Item)(THIS_ VARIANT Index, IConfiguration FAR* FAR* Item);
 	DS_END_DUAL_INTERFACE_PART(Configurations)
 
 	DS_DECLARE_ENABLE_DUAL(CAutoConfigurations, Configurations)

};

/////////////////////////////////////////////////////////////////////////////
class CAutoDepConfigurations : public CAutoObj
{
	DECLARE_DYNCREATE(CAutoDepConfigurations)

	CAutoDepConfigurations();           // protected constructor used by dynamic creation

// Attributes
public:
	static CAutoDepConfigurations* Create(HBUILDER hBld, LPCTSTR szProject);
	virtual void AssociatedObjectReleased();

// Operations
public:
	DECLARE_DUAL_ERRORINFO();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAutoDepConfigurations)
	public:
	virtual void OnFinalRelease();
	//}}AFX_VIRTUAL

// Implementation
protected:
	CDispatchList* m_pPtrlConfigs;
	HBUILDER m_hBld;
	CString m_strProject;
	virtual ~CAutoDepConfigurations();

	virtual void RefreshConfigsInList();
	void ReleaseConfigsInList();

	// Generated message map functions
	//{{AFX_MSG(CAutoDepConfigurations)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()

	// Generated OLE dispatch map functions
	//{{AFX_DISPATCH(CAutoDepConfigurations)
	afx_msg LPDISPATCH GetApplication();
	afx_msg long GetCount();
	afx_msg LPDISPATCH GetParent();
	afx_msg LPDISPATCH Item(const VARIANT FAR& Index);
	//}}AFX_DISPATCH
	afx_msg LPUNKNOWN GetNewEnum();
	DECLARE_DISPATCH_MAP()
	DECLARE_INTERFACE_MAP()
 	DS_BEGIN_DUAL_INTERFACE_PART(Configurations)
		STDMETHOD(get_Application)(THIS_ IDispatch * FAR* Application);
		STDMETHOD(get_Count)(THIS_ long FAR* Count);
		STDMETHOD(get_Parent)(THIS_ IBuildProject FAR* FAR* Parent);
		STDMETHOD(get__NewEnum)(THIS_ IUnknown * FAR* _NewEnum);
		STDMETHOD(Item)(THIS_ VARIANT Index, IConfiguration FAR* FAR* Item);
 	DS_END_DUAL_INTERFACE_PART(Configurations)
 
 	DS_DECLARE_ENABLE_DUAL(CAutoDepConfigurations, Configurations)
public:
protected:
	CString m_strConfig;
};

