// AutoBp.h : header file
//

#ifndef __AUTOBP_H
#define __AUTOBP_H

#include <utilauto.h> //Shell Automation Utilities.
#include <ObjModel\dbgguid.h>
#include <ObjModel\dbgauto.h>
#include <privguid.h>

/////////////////////////////////////////////////////////////////////////////
// CAutoBreakpoint command target

class CAutoBreakpoint : public CAutoObj
{
	DECLARE_DYNCREATE(CAutoBreakpoint)
	friend class CAutoBreakpoints;

private:
	CAutoBreakpoint();           // private constructor used by dynamic creation

// Attributes
public:

// Operations
public:
	CAutoBreakpoint(PBREAKPOINTNODE pbpn);  // normal ctor
	void BPBeGone();
#ifdef _DEBUG	
	int GetRef() { return m_dwRef; } // BUGBUG
#endif

  DECLARE_DUAL_ERRORINFO()

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAutoBreakpoint)
	public:
	virtual void OnFinalRelease();
	//}}AFX_VIRTUAL

// Implementation
protected:
	BOOL NoBP(void); // TRUE if associated BP released, and throws error
	virtual ~CAutoBreakpoint();

	PBREAKPOINTNODE m_pbpn;

  DS_BEGIN_DUAL_INTERFACE_PART(Breakpoint)
    // specific dual interface members go here

    /* IBreakpoint methods */
    STDMETHOD(get_Application)(THIS_ IDispatch * FAR* ppDispatch);
    STDMETHOD(get_Parent)(THIS_ IDispatch * FAR* ppDispatch);
    STDMETHOD(get_Enabled)(THIS_ VARIANT_BOOL FAR* pBool);
    STDMETHOD(put_Enabled)(THIS_ VARIANT_BOOL bool);
    STDMETHOD(get_Location)(THIS_ BSTR FAR* pBSTR);
    STDMETHOD(get_File)(THIS_ BSTR FAR* pBSTR);
    STDMETHOD(get_Function)(THIS_ BSTR FAR* pBSTR);
    STDMETHOD(get_Executable)(THIS_ BSTR FAR* pBSTR);
    STDMETHOD(get_Condition)(THIS_ BSTR FAR* pBSTR);
    STDMETHOD(put_Condition)(THIS_ BSTR bstr);
    STDMETHOD(get_Elements)(THIS_ long FAR* pLong);
    STDMETHOD(get_PassCount)(THIS_ long FAR* pLong);
    STDMETHOD(get_Message)(THIS_ long FAR* pLong);
    STDMETHOD(get_WindowProcedure)(THIS_ BSTR FAR* pBSTR);
    STDMETHOD(get_Type)(THIS_ long FAR* pLong);
    STDMETHOD(Remove)(THIS);

  DS_END_DUAL_INTERFACE_PART(Breakpoint)

  DS_DECLARE_ENABLE_DUAL(CAutoBreakpoint, Breakpoint)

	// Generated message map functions
	//{{AFX_MSG(CAutoBreakpoint)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()

	// Generated OLE dispatch map functions
	//{{AFX_DISPATCH(CAutoBreakpoint)
	afx_msg LPDISPATCH GetApplication();
	afx_msg LPDISPATCH GetParent();
	afx_msg BOOL GetEnabled();
	afx_msg void SetEnabled(BOOL bNewValue);
	afx_msg BSTR GetLocation();
	afx_msg BSTR GetFile();
	afx_msg BSTR GetFunction();
	afx_msg void SetFunction(LPCTSTR lpszNewValue);
	afx_msg BSTR GetExecutable();
	afx_msg void SetExecutable(LPCTSTR lpszNewValue);
	afx_msg BSTR GetCondition();
	afx_msg void SetCondition(LPCTSTR lpszNewValue);
	afx_msg long GetElements();
	afx_msg long GetPassCount();
	afx_msg long GetMessage();
	afx_msg BSTR GetWindowProcedure();
	afx_msg long GetType();
	afx_msg void Remove();
	//}}AFX_DISPATCH
//	afx_msg BOOL GetValid();

	DECLARE_DISPATCH_MAP()
	DECLARE_INTERFACE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

#endif // __AUTOBP_H

