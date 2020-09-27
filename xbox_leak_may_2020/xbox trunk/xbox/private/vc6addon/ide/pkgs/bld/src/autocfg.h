// autocfg.h : header file
//
#include <utilauto.h>
#include <ObjModel\bldauto.h>
#include <ObjModel\bldguid.h>
#include "autoguid.h"
#include "bldiface.h"


class CAutoConfigurations;
/////////////////////////////////////////////////////////////////////////////
// CAutoConfiguration command target

class CAutoConfiguration : public CAutoObj
{
	friend class CAutoConfigurations;
	friend class CAutoDepConfigurations;
	DECLARE_DYNCREATE(CAutoConfiguration)

	CAutoConfiguration();           // protected constructor used by dynamic creation

// Attributes
public:
	static CAutoConfiguration* Create(HBUILDER hBuilder, LPCTSTR szName);
	virtual void AssociatedObjectReleased();
	HBUILDER GetHBuilder() { return m_hBuilder; }
	LPCTSTR GetConfigName() { return m_strName; }

	// This throws a dispatch exception if an invalid configuration
	//  is being used.
	void EnsureBuildable();

// Operations
public:
	DECLARE_DUAL_ERRORINFO();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAutoConfiguration)
	public:
	virtual void OnFinalRelease();
	//}}AFX_VIRTUAL

// Implementation
protected:
	CString m_strName;
	DWORD 	m_nDepth;
	LPDISPATCH m_pConfigs;
	HBUILDER m_hBuilder;		// HBUILDER to parent project
	virtual ~CAutoConfiguration();
	HBLDTARGET GetTarget();
	void ThrowInvalidError();

	// Generated message map functions
	//{{AFX_MSG(CAutoConfiguration)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()

	// Generated OLE dispatch map functions
	//{{AFX_DISPATCH(CAutoConfiguration)
	afx_msg LPDISPATCH GetApplication();
	afx_msg LPDISPATCH GetParent();
	afx_msg void AddToolSettings(LPCTSTR szTool, LPCTSTR szSettings, const VARIANT FAR& Reserved);
	afx_msg void RemoveToolSettings(LPCTSTR szTool, LPCTSTR szSettings, const VARIANT FAR& Reserved);
	afx_msg void AddCustomBuildStep(LPCTSTR szCommand, LPCTSTR szOutput, LPCTSTR szDescription, const VARIANT FAR& Reserved);
	afx_msg void Reserved11(void);
	afx_msg void MakeCurrentSettingsDefault(const VARIANT FAR& Reserved);
	afx_msg LPDISPATCH GetDependentConfigurations();
//	afx_msg void Build();
	afx_msg void AddFileSettings(LPCTSTR szTool, LPCTSTR szSettings, const VARIANT FAR& Reserved);
	afx_msg void RemoveFileSettings(LPCTSTR szTool, LPCTSTR szSettings, const VARIANT FAR& Reserved);
	afx_msg void AddCustomBuildStepToFile(LPCTSTR szFile, LPCTSTR szCommand, LPCTSTR szOutput, LPCTSTR szDescription, const VARIANT FAR& Reserved);
	//}}AFX_DISPATCH
	afx_msg BSTR GetName();
	DECLARE_DISPATCH_MAP()
	DECLARE_INTERFACE_MAP()

 	DS_BEGIN_DUAL_INTERFACE_PART(Configuration)
		STDMETHOD(get__Name)(THIS_ BSTR FAR* _Name);
		STDMETHOD(get_Application)(THIS_ IDispatch * FAR* Application);
		STDMETHOD(get_Parent)(THIS_ IDispatch * FAR* Parent);
		STDMETHOD(get_Name)(THIS_ BSTR FAR* Name);
	    STDMETHOD(AddToolSettings)(THIS_ BSTR szTool, BSTR szSettings, VARIANT Reserved);
	    STDMETHOD(RemoveToolSettings)(THIS_ BSTR szTool, BSTR szSettings, VARIANT Reserved);
	    STDMETHOD(AddCustomBuildStep)(THIS_ BSTR szCommand, BSTR szOutput, BSTR szDescription, VARIANT Reserved);
		STDMETHOD(get_Configurations)(THIS_ IConfigurations FAR* FAR* Configurations);
	    STDMETHOD(Reserved11)(THIS);
	    STDMETHOD(MakeCurrentSettingsDefault)(THIS_ VARIANT Reserved);
//	    STDMETHOD(Build)(THIS_ VARIANT Reserved);
	    STDMETHOD(AddFileSettings)(THIS_ BSTR szFile, BSTR szSettings, VARIANT Reserved);
	    STDMETHOD(RemoveFileSettings)(THIS_ BSTR szFile, BSTR szSettings, VARIANT Reserved);
	    STDMETHOD(AddCustomBuildStepToFile)(THIS_ BSTR szFile, BSTR szCommand, BSTR szOutput, BSTR szDescription, VARIANT Reserved);
 	DS_END_DUAL_INTERFACE_PART(Configuration)
 
 	DS_DECLARE_ENABLE_DUAL(CAutoConfiguration, Configuration)

};

/////////////////////////////////////////////////////////////////////////////
