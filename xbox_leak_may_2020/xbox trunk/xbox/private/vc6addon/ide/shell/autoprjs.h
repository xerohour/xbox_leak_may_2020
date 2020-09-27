// AutoPrjs.h : header file
//
#include <utilauto.h>
#include <ObjModel\appauto.h>
#include <ObjModel\appguid.h>


/////////////////////////////////////////////////////////////////////////////
// CAutoProjects command target
// This class represents the collection of all projects in the currently
//  open workspace.  An instance of this class is created by 
//  CBldPackage::GetPackageExtension in vproj.cpp (via CAutoProjects::Create).

class CAutoProjects : public CAutoObj
{
	DECLARE_DYNCREATE(CAutoProjects)

	CAutoProjects();           // protected constructor used by dynamic creation

// Attributes
public:
	static CAutoProjects* Create();

// Operations
public:
	DECLARE_DUAL_ERRORINFO();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAutoProjects)
	public:
	virtual void OnFinalRelease();
	//}}AFX_VIRTUAL

// Implementation
protected:
	CDispatchList* m_pPtrlProjects;
	virtual ~CAutoProjects();

	// Generated message map functions
	//{{AFX_MSG(CAutoProjects)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()

	// Generated OLE dispatch map functions
	//{{AFX_DISPATCH(CAutoProjects)
	afx_msg LPDISPATCH GetApplication();
	afx_msg long GetCount();
	afx_msg LPDISPATCH GetParent();
	afx_msg LPDISPATCH Item(const VARIANT FAR& Index);
	//}}AFX_DISPATCH
	afx_msg LPUNKNOWN GetNewEnum();
	DECLARE_DISPATCH_MAP()
	DECLARE_INTERFACE_MAP()
 	DS_BEGIN_DUAL_INTERFACE_PART(Projects)
		STDMETHOD(get_Application)(THIS_ IDispatch * FAR* Application);
		STDMETHOD(get_Count)(THIS_ long FAR* Count);
		STDMETHOD(get_Parent)(THIS_ IDispatch * FAR* Parent);
		STDMETHOD(get__NewEnum)(THIS_ IUnknown * FAR* _NewEnum);
		STDMETHOD(Item)(THIS_ VARIANT Index, IGenericProject FAR* FAR* Item);
 	DS_END_DUAL_INTERFACE_PART(Projects)
 
 	DS_DECLARE_ENABLE_DUAL(CAutoProjects, Projects)

private:
	void BuildNewList();
};
