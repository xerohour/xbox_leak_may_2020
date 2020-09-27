#include "stdafx.h"
#pragma hdrstop

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/****************************************************************************

	FUNCTION:   StopDebugConfirm

	PURPOSE:    if Debugger is running, asks if you really want to stop it

****************************************************************************/

BOOL
StopDebugConfirm(BOOL fFailSilently /*= FALSE*/)
{
	BOOL fRet = TRUE;

	if (lpprcCurr &&
		(lpprcCurr->stp == stpRunning || lpprcCurr->stp == stpStopped))
	{
		int	idMsgBox;

		ASSERT(pDebugCurr);
		if (!(pDebugCurr->MtrcAsync() & asyncKill) && DebuggeeRunning()) {
			idMsgBox = IDS_CmdWillStopDebuggerRemoteRestart;
		}
		else {
			idMsgBox = IDS_CmdWillStopDebugger;
		}
		if (!fFailSilently)
			fRet = (AfxMessageBox(idMsgBox, MB_OKCANCEL) == IDOK);
		else
			fRet = FALSE;

		if ( fRet ) {
			// After we put up the message box the debuggee might
			// have died a natural death. make sure it still exists
			// before trying to kill it.
			if (DebuggeeAlive() && !ClearDebuggee(TRUE)) {
				fRet = FALSE;
			}
		}
		else {
			fRet = FALSE;
		}
	}
	return fRet;
}

// [CAVIAR #5904 12/02/92 v-natjm]
/****************************************************************************

	FUNCTION:       ControlExecDebuggee()

	PURPOSE:                Control if a modeless dialog box is present before
					executing a debug command

****************************************************************************/
VOID PASCAL ControlExecDebuggee(WORD CommandId,WORD wExec)
{
	ExecDebuggee((EXECTYPE)wExec) ;
}

LRESULT OnAutosaveTick(HWND, WPARAM, LPARAM lParam)
{
	CAutosave *const pAutosaveServices = (CAutosave *const) lParam;

//	CAutosave *const pAutosaveServices = (CAutosave *const) &gVeryBigHackAutosaver;
//	pAutosaveServices->EnterTick();

	// Sanity check on the autosaver.

	ASSERT(pAutosaveServices->IsEnabled());
	ASSERT(pAutosaveServices->InTick());

	return 0;
}

/****************************************************************************

	FUNCTION:	OnQueryJit

	PURPOSE:	Determine whether or not we would like to be the just-in-time
				debugger for the specified process.  Another copy of MSVC,
				which was started by the system, is asking us this.

	RETURNS:	TRUE for yes, FALSE for no.

****************************************************************************/

BOOL OnQueryJit(PID pid)
{
	// If we're already debugging something then we can't begin debugging
	// another process
	if (DebuggeeAlive())
		return FALSE;

	// If a GO command isn't legal right now then we don't want to start
	// debugging something else
	BOOL bEnabled = FALSE;

	CommandIdEnabled(IDM_RUN_GO, bEnabled);

	if (!bEnabled)
		return FALSE;

	// If we created the specified process, then we'll debug it
	return FParentOfProcess((DWORD)pid, NULL);
}

/****************************************************************************

	FUNCTION:	OnBeginJit

	PURPOSE:	Begin just-in-time debugging.  Another copy of MSVC,
				which was started by the system, is sending us this
				message.

	RETURNS:	TRUE if we begin just-in-time debugging, FALSE if not.

****************************************************************************/

BOOL OnBeginJit(PID pid, HANDLE hEvent)
{
	const char *szPath;

	// There's a slim chance that even though we responded yes to an earlier
	// WU_QUERYJIT message, we are now don't want to be the just-in-time
	// debugger (perhaps because in the meantime we've started debugging
	// something else).
	if (!OnQueryJit(pid))
	{
		CloseHandle(hEvent);
		return FALSE;
	}

	// get path
	VERIFY(FParentOfProcess(pid, &szPath));

	// Set up just-in-time debugging
	theApp.m_jit.SetPid(pid);
	theApp.m_jit.SetEvent(hEvent);
	VERIFY(theApp.m_jit.FSetPath(szPath, TRUE));
	theApp.m_jit.SetActive(TRUE);

	// [dolphin#1685 9/30/93 mikemo]
	if (theApp.m_pMainWnd->IsIconic())
		theApp.m_pMainWnd->ShowWindow(SW_RESTORE);

	SetForegroundWindow(theApp.m_pMainWnd->GetSafeHwnd());
	theApp.m_pMainWnd->UpdateWindow();

	// Post ourselves a Go message so that we'll start debugging.
	theApp.m_pMainWnd->PostMessage(WM_COMMAND,
		GET_WM_COMMAND_MPS(IDM_RUN_GO, 0, 0));

	return TRUE;
}


// FUTURE: Move these to appropriate views or the package!
BOOL MainWnd_OnCommand(WPARAM wParam, LPARAM lParam)
{
	UINT CommandId;
	CommandId = GET_WM_COMMAND_ID(wParam, lParam);
	switch (CommandId)
	{
		case IDM_GOTO_CODE:
			ASSERT(GetCurView() == pViewCalls);
			// Hacky way of telling the calls view to go to source
			// for current cursor position.
			pViewCalls->SendMessage(WM_CHAR, VK_RETURN, 0L);
			return TRUE;

		case IDM_GOTO_SOURCE:
		case IDM_GOTO_DISASSY:
		case IDM_VIEW_MIXEDMODE:
        {
			extern void SwitchToMixedMode(void);
			SwitchToMixedMode();
			return TRUE;
		}
	}
	return FALSE;
}


/****************************************************************************

	FUNCTION:   MainWndProc

	PURPOSE:    Processes window messages.

****************************************************************************/
long PASCAL MainWndProc(HWND hwnd, UINT message, UINT wParam, LONG lParam)
{
	if (message == WU_QUERYJIT)
		return OnQueryJit((PID)wParam);
	else if (message == WU_BEGINJIT)
		return OnBeginJit((PID)wParam, (HANDLE)lParam);

	switch ( message )
    {
		case WM_TIMER:
			switch (wParam)
			{
				case DBGCALLBACKTIMERID:
					// Prevent showing multiple Find Local Module dlgs
					if (!fInFindLocalDlg) {
						DoCallBackAndQueueReturn();
					}
					break;
			}
			goto DefProcessing;

		case WM_COMMAND:
			return (LRESULT)MainWnd_OnCommand(wParam, lParam);

		case WM_QUERYENDSESSION:
			//Before session ends, check that it is ok to quit when debugging
			extern int BoxCount;
			if (BoxCount > 0)
			{
				// we have a dialog up
				ErrorBox(ERR_Cannot_Quit);
				return FALSE;
			}
			if (DebuggeeAlive() && (QuestionBox(ERR_Close_When_Debugging, MB_YESNO) != IDYES))
				return FALSE;

			return TRUE;

		case WU_FINDSYMBOLS:
			{
				PSEARCHDEBUGINFOANDRETURN psdir = (PSEARCHDEBUGINFOANDRETURN) lParam;

				ASSERT (psdir);
				
				psdir -> fSuccess = SYFindDebugInfoFile (psdir -> psdi);

				g_evFindSymbols.Unlock();
				return 0;
			}


		case WU_MESSAGEBOX:
			{
				MsgBoxTypes mboxType = (MsgBoxTypes)wParam;
				CString *pStr = (CString *)lParam ;

				ASSERT(mboxType < NUMBER_OF_MESSAGE_BOXES);
				LRESULT lResult = MsgBox(mboxType, (*pStr));

				delete pStr;
				return lResult;
			}

		case WU_REMOTEQUIT:
			ErrorBox(DBG_Remote_Quit);
			FakeAsyncKillDebuggee(TRUE);
			return 0;
					
		default:
		DefProcessing:
			return DefWindowProc(hwnd, message, wParam, lParam);
	}
	return 0;
}

// Stolen from the MDI child fram's WM_MDIACTIVATE handler
//
void OnViewActivate(CMultiEdit *pcme, BOOL bActivate)
{
	// Sushi: stolen from the MDI child frame's WM_MDIACTIVATE handler
	if ( bActivate )
	{
		if ( pcme->m_dt == DOC_WIN )
		{
			TraceInfo.StepMode = SRCSTEPPING;

			// Save pLastDocWin
			pLastDocWin = (CIDEDoc *)pcme->GetDocument();
		}
		else if ( pcme->m_dt == DISASSY_WIN) // && !fIsLaunchingDebuggee )
		{
			TraceInfo.StepMode = ASMSTEPPING;
		}
	}
}



// Helper for various Debug menu commands as well as
// Project.Execute...
// Returns whether active project (or JIT) along with whether the
// current target is unknown (ie. can't do anything with this target).

BOOL EnsureActiveProjectOrJIT(BOOL & fUnknownTarget)
{
	fUnknownTarget = FALSE;

	if (FIsActiveProjectDebuggable( &fUnknownTarget, NULL, NULL, NULL ))
		return TRUE;

	if (theApp.m_jit.GetActive())
		return TRUE;	// JIT debugging -> everything hunky-dorey

	if (MsgBox(Question, IDS_ACTIVE_PROJ_REQUIRED, MB_YESNO) == IDNO)
		return FALSE;	// User refused to create project

	LPSOURCEEDIT pISrcEdit = NULL;
	CString strActiveDocPath;
	
	ASSERT(NULL != gpISrcQuery); //we should have gotten this when the package loaded
	if (NULL == gpISrcQuery)
		return FALSE;

	if (SUCCEEDED(gpISrcQuery->CreateSourceEditForActiveDoc(&pISrcEdit, TRUE)))
	{
		pISrcEdit->GetPath(strActiveDocPath);
		// Force the source file to be saved if it's untitled:
		if (strActiveDocPath.IsEmpty())
		{
			if (!SUCCEEDED(pISrcEdit->CommitChanges()))
			{
				pISrcEdit->Release();
				return FALSE;	// Save failed or user cancelled Save As dlg
			}
			// Get new name
			pISrcEdit->GetPath(strActiveDocPath);
		}
		pISrcEdit->Release();
	}
	else
		return FALSE;

	if (strActiveDocPath.IsEmpty())
		return FALSE; // no file to base the new project upon

	if (gpIProjectWorkspace)
	{
		CDocument * pDocument;
		if (FAILED(gpIProjectWorkspace->CreateDefaultWorkspace(&pDocument, strActiveDocPath)))
		{
			return FALSE;	// Project creation failed for some reason
		}

		ASSERT(gpIBldSys != NULL && gpIBldSys->IsActiveBuilderValid() == S_OK);	// Better have one now!
	}
	else 
		return FALSE;
	return TRUE;
}

void PASCAL EnsureDebugWindow(int winType)
{
	HWND win = GetDebugWindowHandle(winType);

	if (win == NULL)
        CreateView(winType);
}

