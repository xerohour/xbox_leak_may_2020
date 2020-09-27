// AutoDbg.h : header file
//

#ifndef __AUTODBG_H
#define __AUTODBG_H


#include <utilauto.h> //Shell Automation Utilities.
#include <ObjModel\dbgguid.h>
#include "autoguid.h"
#include <ObjModel\dbgauto.h>
#include "privguid.h"
#include "autobp.h"

/////////////////////////////////////////////////////////////////////////////
// CAutoDebugger command target


class CAutoDebugger : public CAutoObj
{
	DECLARE_DYNCREATE(CAutoDebugger)

// Attributes
public:

// Operations
public:

	CAutoDebugger();  // normal ctor
	CAutoDebugger(CDebugPackage *pDebugPackage);  // actual ctor

	// Public version of Go that can be called by ExecDebuggee in the case
	//  of a deferred Go from a BreakpointHit event hander.  Don't want to
	//  make the actual Go() public, since ClassWizard likes its declaration
	//  right where it is.
	void PublicGo() { Go(); }

	void PublicStop() { Stop(); }

  DECLARE_DUAL_ERRORINFO()

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAutoDebugger)
	public:
	virtual void OnFinalRelease();
	//}}AFX_VIRTUAL

public:

	// events
	VOID FireBreakpointHit(CAutoBreakpoint *bp);

	// connection point for the event dispatch interface
	BEGIN_CONNECTION_PART(CAutoDebugger, DispDebuggerCP)
		CONNECTION_IID(IID_IDispDebuggerEvents)
	END_CONNECTION_PART(DispDebuggerCP)

	// connection point for the event dispatch interface
	BEGIN_CONNECTION_PART(CAutoDebugger, DualDebuggerCP)
		CONNECTION_IID(IID_IDebuggerEvents)
	END_CONNECTION_PART(DualDebuggerCP)

  DECLARE_CONNECTION_MAP()


// Implementation
protected:

	virtual ~CAutoDebugger();
	BOOL NoPkg(void); // TRUE if associated package released, and throws error
	BOOL NoRunCmds(void); // TRUE if we shouldn't execute a run-type command

	CDebugPackage *m_pDebugPackage;

  DS_BEGIN_DUAL_INTERFACE_PART(Debugger)
    // specific dual interface members go here

    STDMETHOD(get_Application)(THIS_ IDispatch * FAR* ppDispatch);
    STDMETHOD(get_Parent)(THIS_ IDispatch * FAR* ppDispatch);
    STDMETHOD(get_Breakpoints)(THIS_ IDispatch * FAR* ppDispatch);
    STDMETHOD(get_DefaultRadix)(THIS_ long FAR* pLong);
    STDMETHOD(put_DefaultRadix)(THIS_ long l);
    STDMETHOD(get_State)(THIS_ DsExecutionState FAR* pState);
    STDMETHOD(get_JustInTimeDebugging)(THIS_ VARIANT_BOOL FAR* pBoolean);
    STDMETHOD(put_JustInTimeDebugging)(THIS_ VARIANT_BOOL bool);
    STDMETHOD(get_RemoteProcedureCallDebugging)(THIS_ VARIANT_BOOL FAR* pBoolean);
    STDMETHOD(put_RemoteProcedureCallDebugging)(THIS_ VARIANT_BOOL bool);
    STDMETHOD(Go)(THIS);
    STDMETHOD(StepInto)(THIS);
    STDMETHOD(StepOver)(THIS);
    STDMETHOD(StepOut)(THIS);
    STDMETHOD(Restart)(THIS);
    STDMETHOD(Stop)(THIS);
    STDMETHOD(Break)(THIS);
    STDMETHOD(Evaluate)(THIS_ BSTR expr, BSTR FAR* pBSTR);
    STDMETHOD(ShowNextStatement)(THIS);
    STDMETHOD(RunToCursor)(THIS);
    STDMETHOD(SetNextStatement)(THIS_ VARIANT Selection);

  DS_END_DUAL_INTERFACE_PART(Debugger)

  DS_DECLARE_ENABLE_DUAL(CAutoDebugger, Debugger)

	// Generated message map functions
	//{{AFX_MSG(CAutoDebugger)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()

	// Generated OLE dispatch map functions
	//{{AFX_DISPATCH(CAutoDebugger)
	afx_msg LPDISPATCH GetApplication();
	afx_msg LPDISPATCH GetParent();
	afx_msg LPDISPATCH GetBreakpoints();
	afx_msg long GetDefaultRadix();
	afx_msg void SetDefaultRadix(long nNewValue);
	afx_msg DsExecutionState GetState();
	afx_msg BOOL GetJustInTimeDebugging();
	afx_msg void SetJustInTimeDebugging(BOOL bNewValue);
	afx_msg BOOL GetRemoteProcedureCallDebugging();
	afx_msg void SetRemoteProcedureCallDebugging(BOOL bNewValue);
	afx_msg void Go();
	afx_msg void StepInto();
	afx_msg void StepOver();
	afx_msg void StepOut();
	afx_msg void Restart();
	afx_msg void Stop();
	afx_msg void Break();
	afx_msg BSTR Evaluate(LPCTSTR Expression);
	afx_msg void ShowNextStatement();
	afx_msg void RunToCursor();
	afx_msg void SetNextStatement(const VARIANT FAR& Selection);
	//}}AFX_DISPATCH
	DECLARE_DISPATCH_MAP()
	DECLARE_INTERFACE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
extern CAutoDebugger *pAutoDebugger;

#endif // __AUTODBG_H

