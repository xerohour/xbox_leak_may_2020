// AutoBps.h : header file
//

#ifndef __AUTOBPS_H__
#define __AUTOBPS_H__

#include <utilauto.h>
#include <ObjModel\dbgdefs.h>
#include <ObjModel\dbgauto.h>

#ifdef _DEBUG
#define dbprintf(args) DBprintf args
extern void DBprintf(const char *format, ...);
#else
#define dbprintf(args)
#endif

////////////////////////////////////////////////////////////////////////////////////
// class CAutoDebugger;

extern class CAutoBreakpoints *pAutoBPS;

class CAutoBreakpoints : public CAutoObj
{
	friend class CEnumVariantObjs;

	//	DECLARE_DYNACREATE(CAutoBreakpoints)
	DECLARE_DYNAMIC(CAutoBreakpoints)


// Attributes
public:
	// The collection will clean up the list. Not the caller.
	CAutoBreakpoints();

// Operations
public:
    DECLARE_DUAL_ERRORINFO()

	void AddBP(CAutoBreakpoint *pBP);
	void RemoveBP(CAutoBreakpoint *pBP);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAutoBreakpoints)
	public:
	virtual void OnFinalRelease();
	//}}AFX_VIRTUAL

// Implementation
protected:
	CDispatchList* m_pPtrlData;
	virtual ~CAutoBreakpoints();

	DS_BEGIN_DUAL_INTERFACE_PART(Breakpoints)
    STDMETHOD(get_Count)(THIS_ long FAR* Count);
    STDMETHOD(get_Application)(THIS_ IDispatch * FAR* ppDispatch);
    STDMETHOD(get_Parent)(THIS_ IDispatch * FAR* ppDispatch);
    STDMETHOD(get__NewEnum)(THIS_ IUnknown * FAR* _NewEnum);
    STDMETHOD(Item)(THIS_ VARIANT index, IDispatch * FAR* Item);
	STDMETHOD(RemoveAllBreakpoints)(THIS);	// This is the suspect
    STDMETHOD(RemoveBreakpointAtLine)(THIS_ VARIANT sel, VARIANT_BOOL FAR* bool);
    STDMETHOD(AddBreakpointAtLine)(THIS_ VARIANT sel, IDispatch * FAR* Item);

	DS_END_DUAL_INTERFACE_PART(Breakpoints)

	DS_DECLARE_ENABLE_DUAL(CAutoBreakpoints, Breakpoints)


	// Generated message map functions
	//{{AFX_MSG(CAutoBreakpoints)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()

	// Generated OLE dispatch map functions
	//{{AFX_DISPATCH(CAutoBreakpoints)
	afx_msg long GetCount();
	afx_msg LPDISPATCH GetApplication();
	afx_msg LPDISPATCH GetParent();
	afx_msg LPDISPATCH Item(const VARIANT FAR& index);
	afx_msg void RemoveAllBreakpoints();
	afx_msg BOOL RemoveBreakpointAtLine(const VARIANT FAR& Selection);
	afx_msg LPDISPATCH AddBreakpointAtLine(const VARIANT FAR& Selection);
	//}}AFX_DISPATCH
	afx_msg LPUNKNOWN Get_NewEnum();

	DECLARE_DISPATCH_MAP()
	DECLARE_INTERFACE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
int VarEditorLine(const VARIANT FAR &Selection);

#endif //__AUTOBPS_H__
