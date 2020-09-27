///////////////////////////////////////////////////////////////////////////////
//  CODEBUG.H
//
//  Created by :            Date :
//      MichMa                  1/13/94
//
//  Description :
//      Declaration of the CODebug class
//
   
#ifndef __CODEBUG_H__
#define __CODEBUG_H__

#include "costack.h"
#include "..\shl\uwbframe.h"
#include "..\..\testutil.h"

#include "dbgxprt.h"

enum StepMode { SRC, ASM };
enum DebugState { NotDebugging, Debug_Run, Debug_Break };

// msg types user can pass to CancelMsg() and HandleMsg().  these also act as indexes into
// an array of localized string id's (see msg_array in codebug.cpp)

typedef enum {
	MSG_REMOTE_LOCAL_EXE_MISMATCH,
	MSG_REMOTE_EXE_NAME_REQUIRED,
	MSG_EC_DELAYED_DUE_TO_CTOR_DTOR,
	MSG_EC_ONLY_ASM_AVAILABLE, 
	MSG_EXCEPTION_DIVIDE_BY_ZERO,
	MSG_EC_POINT_OF_EXECUTION_MOVED,
	MSG_EC_LOCAL_VAR_REMOVED
} MSG_TYPE;

#define TOTAL_OUTPUT_PANES 4

///////////////////////////////////////////////////////////////////////////////
//  CODebug class

// BEGIN_CLASS_HELP
// ClassName: CODebug
// BaseClass: none
// Category: Debug
// END_CLASS_HELP
class DBG_CLASS CODebug

    {
    // Data
    private:
        COStack cos;
        StepMode mMode;
		COStack* m_pOwnedStack;

    // internal functions
    private:
        BOOL Exec(BOOL (UIWBFrame::*stepfcn)(int), int count = 1, LPCSTR symbol = NULL, LPCSTR line = NULL, LPCSTR function = NULL, int WAIT=WAIT_FOR_BREAK, int ModeVerify=TRUE);
        BOOL ExecUntilLine(BOOL   (UIWBFrame::*stepfcn)(int), LPCSTR line, LPCSTR function = NULL, int WAIT=WAIT_FOR_BREAK);
        BOOL ExecUntilFunc(BOOL   (UIWBFrame::*stepfcn)(int), LPCSTR function = NULL, int WAIT=WAIT_FOR_BREAK);
        BOOL ExecUntilSymbol(BOOL (UIWBFrame::*stepfcn)(int), LPCSTR symbol, int WAIT=WAIT_FOR_BREAK);
        BOOL MyIsActive();
		// REVIEW (michma): these should probably be in shl
		BOOL WaitMsgDisplayed(MSG_TYPE msg, int wait_time);
		BOOL WaitMsgDismissed(MSG_TYPE msg);


    // Utilities
    public:
        CODebug();
        BOOL Go(LPCSTR symbol = NULL, LPCSTR line = NULL, LPCSTR function = NULL, int WAIT=WAIT_FOR_BREAK);

        BOOL Restart(LPCSTR path = NULL);
        BOOL StopDebugging(int WAIT=ASSUME_NORMAL_TERMINATION);
        BOOL Break(void);

        BOOL SetSteppingMode(StepMode mode);
        StepMode GetSteppingMode(void);
        BOOL ValidateSteppingMode(StepMode mode);

        BOOL StepInto(int count = 1, LPCSTR symbol = NULL, LPCSTR line = NULL, LPCSTR function = NULL, int WAIT=WAIT_FOR_BREAK);
        BOOL StepIntoUntilLine(LPCSTR line, LPCSTR function = NULL, int WAIT=WAIT_FOR_BREAK);
        BOOL StepIntoUntilFunc(LPCSTR function = NULL, int WAIT=WAIT_FOR_BREAK);
        BOOL StepIntoUntilSymbol(LPCSTR symbol, int WAIT=WAIT_FOR_BREAK);

		BOOL StepIntoSpecific(int iWait = WAIT_FOR_BREAK);

        BOOL StepOver(int count = 1, LPCSTR symbol = NULL, LPCSTR line = NULL, LPCSTR function = NULL, int WAIT=WAIT_FOR_BREAK);
        BOOL StepOverUntilLine(LPCSTR line, LPCSTR function = NULL, int WAIT=WAIT_FOR_BREAK);
        BOOL StepOverUntilFunc(LPCSTR function = NULL, int WAIT=WAIT_FOR_BREAK);
        BOOL StepOverUntilSymbol(LPCSTR symbol, int WAIT=WAIT_FOR_BREAK);

        BOOL StepOut(int count = 1, LPCSTR symbol = NULL, LPCSTR line = NULL, LPCSTR function = NULL, int WAIT=WAIT_FOR_BREAK);
        BOOL StepOutUntilLine(LPCSTR line, LPCSTR function = NULL, int WAIT=WAIT_FOR_BREAK);
        BOOL StepOutUntilFunc(LPCSTR function = NULL, int WAIT=WAIT_FOR_BREAK);
        BOOL StepOutUntilSymbol(LPCSTR symbol, int WAIT=WAIT_FOR_BREAK);

        BOOL StepToCursor(int linepos = 0, LPCSTR symbol = NULL, LPCSTR line = NULL, LPCSTR function = NULL, int WAIT=WAIT_FOR_BREAK);
		BOOL AttachToProcess(LPCSTR szProcessName, int WAIT = WAIT_FOR_RUN, BOOL bSysProcess = FALSE);

        BOOL SetNextStatement(int line = 0);
        BOOL SetNextStatement(LPCSTR line);
        BOOL ShowNextStatement(void);

        BOOL Wait(int option, BOOL bLeaveFindSource = FALSE);
        DebugState GetDebugState(void);
		BOOL ToggleHexDisplay(BOOL bEnable);
		BOOL ToggleSourceAnnotation(BOOL bEnable);

        // check but no logging
        BOOL CurrentLineIs(LPCSTR line);
        BOOL AtSymbol(LPCSTR symbol);

        // check, log standard warning if fails
        BOOL VerifyCurrentLine(LPCSTR line);
        BOOL VerifyAtSymbol(LPCSTR symbol);
        BOOL VerifyTermination(int exitcode);  // log warning if code is wrong.
		BOOL VerifyTermination(void);          // any exitcode
		
		BOOL VerifyNotification(LPCSTR notification, int nOccurence = 1, int numberOfLines = 1, BOOL nCase = FALSE);      
		int SetDebugOutputPane(int pane);

		BOOL SetOwnedStack( COStack* );
		void DirtyStack() { if( m_pOwnedStack != NULL ) m_pOwnedStack->SetDirty( TRUE ); }

		// REVIEW (michma): these should probably be in shl
		BOOL HandleMsg(MSG_TYPE msg, LPCSTR pdata = NULL, int wait_time = 60);
		BOOL CancelMsg(MSG_TYPE msg, int wait_time = 60);
	
        // NOT PORTABLE AVOID USING THESE
        BOOL DoDebuggeeKeys(CString str);
        BOOL DoDebuggeeCmd(CString str);

		BOOL ToggleReturnValue(TOGGLE_TYPE t);
		BOOL ToggleEditAndContinue(TOGGLE_TYPE t);
        BOOL ResolveSourceLineAmbiguity(int iEntry);
		BOOL ResolveAmbiguity(int iEntry);
		BOOL ResolveAmbiguity(LPCSTR szEntry);
		BOOL ResolveSourceLineAmbiguity(LPCSTR szEntry);
		BOOL SupplySourceLocation(LPCSTR path);

};

#endif // __CDEBUG_H__
