////////////////////////////////////////////////////////////////////////////
//
// A super-simple sample package that shows off most of the shell interface.
//

#include "stdafx.h"             // Windows, MFC, and Shell headers (pre-compiled)
#pragma hdrstop

#include "idbgproj.h"
#include "prjguid.h"
#include "cmdtable.h"
#include "fbrdbg.h"
#include "autodbg.h"
#include <initguid.h>
#include "msodcid.h"
#include "oleref.h"
#include "dbgenc.h"

#if defined (_X86_)
#define native_platform	win32x86
#elif defined (_ALPHA_)
#define native_platform win32alpha
#elif defined (_MIPS_)
#define	native_platform win32mips
#elif defined (_POWERPC_)
#define native_platform win32ppc
#endif

////////////////////////////////////////////////////////////////////////////
// Global command handlers should be added here.  This is just standard MFC.
//
BEGIN_MESSAGE_MAP(CDebugPackage, CPackage)
    ON_COMMAND_EX       (IDM_RUN_GO,                OnDebugGoPostMsg)
    ON_COMMAND          (ID_RUN_GO,                 OnDebugGo)
    ON_COMMAND          (IDM_RUN_JIT_GO,            OnDebugJITGo)
    ON_COMMAND          (ID_RUN_RESTART,            OnDebugRestart)
    ON_COMMAND          (ID_RUN_TOCURSOR,           OnDebugToCursor)
    ON_COMMAND          (ID_RUN_TRACEINTO,          OnDebugTraceInto)
    ON_COMMAND          (ID_RUN_STEPOVER,           OnDebugStepOver)
    ON_COMMAND          (ID_RUN_STOPAFTERRETURN,    OnDebugStopAfterReturn)
    ON_COMMAND          (ID_RUN_SRC_TRACEINTO,      OnDebugSrcTraceInto)
    ON_COMMAND          (ID_RUN_SRC_STEPOVER,       OnDebugSrcStepOver)
    ON_COMMAND          (ID_RUN_STOPDEBUGGING,      OnDebugStop)
    ON_COMMAND          (ID_RUN_TRACEFUNCTION,      OnDebugTraceFunction)
    ON_COMMAND_EX       (IDM_RUN_TOGGLEBREAK,       OnToggleBreakpoint)
    ON_COMMAND          (IDM_RUN_TOCURSOR,          OnDebugToCursorPostMsg)
    ON_COMMAND          (IDM_RUN_SETBREAK,          OnBreakpoints)
    ON_COMMAND          (IDM_RUN_QUICKWATCH,        OnQuickWatch)
    ON_COMMAND          (IDM_DEBUG_RADIX,           OnDebugRadix)
	ON_COMMAND          (IDM_MEMORY_NEXTFORMAT,     OnNextMemWinFmt)
	ON_COMMAND          (IDM_MEMORY_PREVFORMAT,     OnPrevMemWinFmt)
    ON_COMMAND          (IDM_DBGSHOW_FLOAT,         OnDebugRegFloat)
    ON_COMMAND_EX       (IDM_DBGSHOW_STACKTYPES,    OnDebugStackSet)
    ON_COMMAND_EX       (IDM_DBGSHOW_STACKVALUES,   OnDebugStackSet)
    ON_COMMAND          (IDM_DBGSHOW_SOURCE,        OnDebugSourceAnnotate)
    ON_COMMAND          (IDM_DBGSHOW_CODEBYTES,     OnDebugShowCodeBytes)
    ON_COMMAND_EX       (IDM_DBGSHOW_MEMBYTE,       OnDebugMemorySet)
    ON_COMMAND_EX       (IDM_DBGSHOW_MEMSHORT,      OnDebugMemorySet)
    ON_COMMAND_EX       (IDM_DBGSHOW_MEMLONG,       OnDebugMemorySet)
    ON_COMMAND          (IDM_DBGSHOW_MEMORY_BAR,    OnToggleMemoryBar)
    ON_COMMAND          (IDM_RUN_EXCEPTIONS,        OnExceptions)
    ON_COMMAND          (IDM_RUN_THREADS,           OnThreads)
	ON_COMMAND			(IDM_RUN_FIBERS,			OnFibers)
	ON_COMMAND			(IDM_RUN_FLIPHACK,			OnModuleView)
    ON_COMMAND          (IDM_PROJECT_EXECUTE,       OnExecuteTarget)
    ON_COMMAND          (IDM_RUN_RESTART,           OnDebugRestartPostMsg)
    ON_COMMAND_EX       (IDM_RUN_STOPDEBUGGING,     OnDebugStopPostMsg)
    ON_COMMAND_EX       (IDM_RUN_BREAK,             OnDebugBreak)
    ON_COMMAND          (IDM_RUN_CLEARALLBREAK,     OnClearBreakpoints)
    ON_COMMAND          (IDM_RUN_ENABLEBREAK,       OnEnableDisableBreakpoint)
    ON_COMMAND          (IDM_RUN_DISABLEALLBREAK,   OnDisableAllBreakpoints)
    ON_COMMAND_EX       (IDM_RUN_TRACEINTO,         OnDebugTraceIntoPostMsg)
    ON_COMMAND_EX       (IDM_RUN_STEPOVER,          OnDebugStepOverPostMsg)
    ON_COMMAND          (IDM_RUN_SRC_TRACEINTO,     OnDebugSrcTraceIntoPostMsg)
    ON_COMMAND          (IDM_RUN_TRACEFUNCTION,     OnDebugTraceFunctionPostMsg)
    ON_COMMAND          (IDM_RUN_SRC_STEPOVER,      OnDebugSrcStepOverPostMsg)
    ON_COMMAND_EX       (IDM_RUN_STOPAFTERRETURN,   OnDebugStopAfterReturnPostMsg)
    ON_COMMAND          (IDM_RUN_SETNEXTSTMT,       OnDebugSetIP)
    ON_COMMAND          (IDM_DEBUG_ACTIVE,          OnDebugActive)
    ON_COMMAND          (IDM_RUN_REMOTEDEBUGGER,    OnRemoteDebugger)
    ON_COMMAND          (IDM_DEBUG_SHOWIP,           OnGotoCurrentInstruction)
#ifdef CODECOVERAGE
	ON_COMMAND          (IDM_RUN_COVER_TOGGLE,      OnCodeCoverageToggle)
	ON_COMMAND          (IDM_RUN_COVER_CLEAR,       OnCodeCoverageClear)
#endif
	ON_COMMAND			(IDM_RUN_ATTACH_TO_ACTIVE, 	OnAttachToActive)
	ON_COMMAND			(IDM_DEBUG_UPDATEIMAGE, 	OnDebugUpdateImage)
	ON_COMMAND			(IDM_DEBUG_UPDATEIMAGE_TOGGLE, OnDebugUpdateImageToggle)

    ON_UPDATE_COMMAND_UI(IDM_RUN_TRACEFUNCTION,     OnUpdateTraceFunction)
    ON_UPDATE_COMMAND_UI(IDM_DBGSHOW_FLOAT,         OnUpdateDebugOptions)
    ON_UPDATE_COMMAND_UI(IDM_DBGSHOW_STACKTYPES,    OnUpdateDebugOptions)
    ON_UPDATE_COMMAND_UI(IDM_DBGSHOW_STACKVALUES,   OnUpdateDebugOptions)
    ON_UPDATE_COMMAND_UI(IDM_DBGSHOW_SOURCE,        OnUpdateDebugOptions)
    ON_UPDATE_COMMAND_UI(IDM_DBGSHOW_CODEBYTES,     OnUpdateDebugOptions)
    ON_UPDATE_COMMAND_UI(IDM_DBGSHOW_MEMBYTE,       OnUpdateDebugOptions)
    ON_UPDATE_COMMAND_UI(IDM_DBGSHOW_MEMSHORT,      OnUpdateDebugOptions)
    ON_UPDATE_COMMAND_UI(IDM_DBGSHOW_MEMLONG,       OnUpdateDebugOptions)
    ON_UPDATE_COMMAND_UI(IDM_DBGSHOW_MEMORY_BAR,    OnUpdateDebugOptions)
    ON_UPDATE_COMMAND_UI(IDM_PROJECT_EXECUTE,       OnUpdateProjectExecute)
    ON_UPDATE_COMMAND_UI(IDM_RUN_TOGGLEBREAK,       OnUpdateToggleBreakpoint)
    ON_UPDATE_COMMAND_UI(IDM_RUN_ENABLEBREAK,       OnUpdateEnableDisableBreakpoint)
    ON_UPDATE_COMMAND_UI(IDM_RUN_DISABLEALLBREAK,   OnUpdateDisableAllBreakpoints)
    ON_UPDATE_COMMAND_UI(IDM_DEBUG_ACTIVE,          OnUpdateDebugActive)
    ON_UPDATE_COMMAND_UI(IDM_RUN_CLEARALLBREAK,     OnUpdateClearBreakpoints)
    ON_UPDATE_COMMAND_UI(IDM_DEBUG_RADIX,           OnUpdateDebugRadix)
    ON_UPDATE_COMMAND_UI(IDM_DEBUG_SHOWIP,           OnUpdateGotoCurrentInstruction)
    ON_UPDATE_COMMAND_UI(IDM_DEBUG_UPDATEIMAGE,      OnUpdateDebugUpdateImage)
    ON_UPDATE_COMMAND_UI(IDM_DEBUG_UPDATEIMAGE_TOGGLE, OnUpdateDebugUpdateImageToggle)
    ON_UPDATE_COMMAND_UI_RANGE(IDM_ACTIVATE_SRCFIRST + 1, IDM_ACTIVATE_SRCLAST, OnUpdateActivateSrcWnd)
    ON_COMMAND_RANGE          (IDM_ACTIVATE_SRCFIRST + 1, IDM_ACTIVATE_SRCLAST, OnActivateSrcWnd)
    ON_UPDATE_COMMAND_UI_RANGE(IDM_TOGGLE_SRCFIRST,   IDM_TOGGLE_SRCLAST,   OnUpdateToggleSrcWnd)
    ON_COMMAND_RANGE          (IDM_TOGGLE_SRCFIRST,   IDM_TOGGLE_SRCLAST,   OnToggleSrcWnd)
END_MESSAGE_MAP()


// Maybe the current Project wants to enable menus
// Returns TRUE if it was interested, FALSE if not

BOOL ProjectCommandUpdate( UINT nCmd, BOOL *pEnabled )
{
	if (gpIBldSys == NULL)
		return FALSE;
	
	if (gpIBldSys->IsActiveBuilderValid() == S_OK)
		return FALSE;

	HRESULT hr = E_FAIL;

	if (gpIProjectWorkspace)
	{
		// check if the current project supports the new Debug interface
		if(GetActiveIDBGProj())
		{
			// if the interface exists on the active project, go use it
			hr = GetActiveIDBGProj()->CommandEnabled( nCmd, pEnabled );
		}
		else
		{
			// Remote Connection should be enabled if no workspace is open
			if((nCmd == IDM_DEBUG_ACTIVE)
				&& gpIProjectWorkspace->IsWorkspaceInitialised() == S_FALSE)
			{
				hr = S_OK;
				*pEnabled = TRUE;
			}
		}
	}

	return SUCCEEDED(hr) ? TRUE : FALSE;
}

// if no current builder, try running the command via the Project system
// returns TRUE if someone else has executed the command, FALSE for normal handling

BOOL ProjectCommandExecute( UINT nCmd )
{
	if (gpIBldSys == NULL)
		return FALSE;
	
	if (gpIBldSys->IsActiveBuilderValid() == S_OK)
		return FALSE;

	BOOL bHooked = FALSE;

	// check if the current project supports the new Debug interface
	if(GetActiveIDBGProj())
	{
		if (SUCCEEDED(GetActiveIDBGProj()->CommandExecute( nCmd )))
			bHooked = TRUE;
	}
	return bHooked;
}

void CDebugPackage::OnDebugGo(void)
{
	BOOL fUnknown;  // returned from EnsureActiveProjectOrJIT()

	if (!EnsureActiveProjectOrJIT(fUnknown) || fUnknown)
		return;

	ControlExecDebuggee (IDM_RUN_GO, EXEC_GO );
}

void CDebugPackage::OnDebugJITGo(void)
{
	if (pDebugCurr)
		pDebugCurr->Init(FALSE);

	OnDebugGo();
}

void CDebugPackage::OnExecuteTarget()
{
	if (ProjectCommandExecute( IDM_PROJECT_EXECUTE ))
		return;

	BOOL fUnknown;  // returned from EnsureActiveProjectOrJIT()

	if (!EnsureActiveProjectOrJIT(fUnknown) || fUnknown)
		return;

	ExecDebuggee(EXEC_TARGET);
}

void CDebugPackage::OnModuleView(void)
{
	CModuleDialog dlg2;
	dlg2.DoModal();
}

void CDebugPackage::OnExceptions(void)
{
    StartDialog(IDD_EXCEPTIONS, (DLGPROC)DlgException);
}

void CDebugPackage::OnThreads(void)
{
	StartDialog(IDD_THREADS, (DLGPROC)DlgThread);
}

void CDebugPackage::OnFibers(void)
{
	
	OFBRS	ofbrs;
	if(runDebugParams.fFiberDebugging){
		CFibers dlg;
		if(dlg.DoModal () == IDOK){
			ofbrs.op = OFBR_SET_FBRCNTX;
			ofbrs.FbrCntx = dlg.m_FbrCntx;
			OSDSystemService (hpidCurr,
						htidCurr,
						ssvcFiberDebug,
						(LPVOID) &ofbrs,
						sizeof(OFBRS),
						NULL
						);
			HTID htid = htidCurr;
			OSDGetFrame(hpidCurr,htid,1,&htid);
			CLFreeWalkbackStack(TRUE);
			UpdateDebuggerState(UPDATE_DEBUGGEE|UPDATE_ALLDBGWIN|UPDATE_SOURCE);
		}
	}
}

void CDebugPackage::OnDebugRegFloat()
{
	runDebugParams.fCPUFloat = !runDebugParams.fCPUFloat;

	// Update the display window if it's open.
	if (pViewCpu)
		pViewCpu->SendMessage(WM_COMMAND, 0, 0L);
}

BOOL CDebugPackage::OnDebugStackSet(UINT nIDCmd)
{               
	switch (nIDCmd)
	{
	case IDM_DBGSHOW_STACKTYPES:
		runDebugParams.fCallTypes = !runDebugParams.fCallTypes;
		break;
		
	case IDM_DBGSHOW_STACKVALUES:
		runDebugParams.fCallParams = !runDebugParams.fCallParams;
		break;
	}
	// Update the display window if it's open.
	if (pViewCalls)
		pViewCalls->SendMessage(WM_COMMAND, 0, 0L);

	return TRUE;
}

void CDebugPackage::OnDebugSourceAnnotate()
{
	runDebugParams.fDAMAnnotate = !runDebugParams.fDAMAnnotate;

	// Update the display window if it's open.
	if (pViewDisassy)
		pViewDisassy->SendMessage(WM_COMMAND, 0, 0L);
}

void CDebugPackage::OnDebugShowCodeBytes()
{
	runDebugParams.fDAMCodeBytes = !runDebugParams.fDAMCodeBytes;

	// Update the display window if it's open.
	if (pViewDisassy)
		pViewDisassy->SendMessage(WM_COMMAND, 0, 0L);

}

void CDebugPackage::OnToggleMemoryBar()
{
	BOOL fShow = runDebugParams.fMemoryToolbar = !runDebugParams.fMemoryToolbar;

	if (pViewMemory)
	{
		CMemoryParent *pParent = (CMemoryParent *)pViewMemory->GetParent();
		ASSERT(pParent->IsKindOf(RUNTIME_CLASS(CMemoryParent)));
		pParent->m_toolbar.ShowWindow(fShow ? SW_SHOWNA : SW_HIDE);
		pParent->RecalcLayout();
		DkRecalcBorders(pParent);

		if (pParent->m_toolbar.IsChild(CWnd::GetFocus()))
			pParent->SetFocus();
	}
}

BOOL CDebugPackage::OnDebugMemorySet(UINT nIDCmd)
{
	switch ( nIDCmd )
	{
	case IDM_DBGSHOW_MEMBYTE:
		runDebugParams.iMWFormat = MW_BYTE;
		break;

	case IDM_DBGSHOW_MEMSHORT:
		runDebugParams.iMWFormat = MW_INT_HEX;
		break;

	case IDM_DBGSHOW_MEMLONG:
		runDebugParams.iMWFormat = MW_LONG_HEX;
		break;
	};

	if ( pViewMemory )
		pViewMemory->SendMessage(WM_COMMAND, 0, 0L);

	return( TRUE );
}

void CDebugPackage::OnDebugRestart()
{
	BOOL fUnknown;  // returned from EnsureActiveProjectOrJIT()

	if (!EnsureActiveProjectOrJIT(fUnknown) || fUnknown)
		return;

	ControlExecDebuggee (IDM_RUN_RESTART,EXEC_RESTART );
}

void CDebugPackage::OnDebugStop()
{
	if (!(pDebugCurr->MtrcAsync() & asyncKill) && DebuggeeRunning())
	{
		StopDebugConfirm();
	}
	else
	{
		(void) ClearDebuggee(FALSE);
	}
}

void CDebugPackage::OnDebugToCursor()
{
	BOOL fUnknown;  // returned from EnsureActiveProjectOrJIT()

	if (!EnsureActiveProjectOrJIT(fUnknown) || fUnknown)
		return;

	ControlExecDebuggee (
		IDM_RUN_TOCURSOR,
		EXEC_TOCURSOR );
}

void CDebugPackage::OnDebugTraceInto()
{
	BOOL fUnknown;  // returned from EnsureActiveProjectOrJIT()

	if (!EnsureActiveProjectOrJIT(fUnknown) || fUnknown)
		return;

	ControlExecDebuggee (
		IDM_RUN_TRACEINTO,
		EXEC_TRACEINTO );
}

void CDebugPackage::OnUpdateTraceFunction(CCmdUI* pCmdUI)
{
	BOOL bEnable = FALSE;
	BOOL bGotFuncName = FALSE;
	static CString strDefault(GetCommandString(IDM_RUN_TRACEFUNCTION, STRING_MENUTEXT));
	
	if (!CommandIdEnabled(IDM_RUN_TRACEFUNCTION, bEnable))
		bEnable = FALSE;

	// If we are enabled and are on a menu figure out the 
	// function name we will step into. Don't do this for 
	// toolbar items. 
	if (bEnable && IsCmdFromPopupMenu(pCmdUI))
	{
		CTraceFunction trace;

		if (trace.SetupStep ())
		{
			CString strFormatItem, strMenuItem;
			CString strValue;
			
			if (trace.GetString (strValue))
			{
				strFormatItem.LoadString(IDS_CPOP_TRACEFUNCTION);
				wsprintf(strMenuItem.GetBuffer(strFormatItem.GetLength() +
					strValue.GetLength()), strFormatItem, (LPCTSTR) strValue);
				strMenuItem.ReleaseBuffer();
			
				pCmdUI->SetText(strMenuItem);
				bGotFuncName = TRUE;
			}
				
		}

		// If we didn't get a function name just disable the command.
		if (!bGotFuncName)
			bEnable = FALSE;
	}

	if (!bEnable || !bGotFuncName)
		pCmdUI->SetText(strDefault);

	pCmdUI->Enable(bEnable);

}

void CDebugPackage::OnDebugTraceFunction()
{
	BOOL fUnknown;  // returned from EnsureActiveProjectOrJIT()

	if (!EnsureActiveProjectOrJIT(fUnknown) || fUnknown)
		return;

//	was: if (pDebugCurr && !fIsLaunchingDebuggee & DebuggeeAlive () && !DebuggeeRunning())

	if (pDebugCurr && DebuggeeAlive () && !DebuggeeRunning())
	{
		ASSERT (g_pTraceFunction == NULL);
		g_pTraceFunction = new CTraceFunction;

		if (g_pTraceFunction->SetupStep ())
		{
			ControlExecDebuggee (
				IDM_RUN_TRACEFUNCTION,
				EXEC_TRACEFUNCTION );
		}

		ASSERT (g_pTraceFunction != NULL);

		delete g_pTraceFunction;
		g_pTraceFunction = NULL;
	}
}

void CDebugPackage::OnDebugStepOver()
{
	BOOL fUnknown;  // returned from EnsureActiveProjectOrJIT()

	if (!EnsureActiveProjectOrJIT(fUnknown) || fUnknown)
		return;

	ControlExecDebuggee (IDM_RUN_STEPOVER, EXEC_STEPOVER);
}

void CDebugPackage::OnDebugSrcTraceInto()
{
	BOOL fUnknown;  // returned from EnsureActiveProjectOrJIT()

	if (!EnsureActiveProjectOrJIT(fUnknown) || fUnknown)
		return;

	ControlExecDebuggee (
		IDM_RUN_SRC_TRACEINTO,
		EXEC_SRC_TRACEINTO );
}

void CDebugPackage::OnDebugSrcStepOver()
{
	BOOL fUnknown;  // returned from EnsureActiveProjectOrJIT()

	if (!EnsureActiveProjectOrJIT(fUnknown) || fUnknown)
		return;

	ControlExecDebuggee (
		IDM_RUN_SRC_STEPOVER,
		EXEC_SRC_STEPOVER );
}

void CDebugPackage::OnDebugStopAfterReturn()
{
	BOOL fUnknown;  // returned from EnsureActiveProjectOrJIT()

	if (!EnsureActiveProjectOrJIT(fUnknown) || fUnknown)
		return;

	ControlExecDebuggee (
		IDM_RUN_STOPAFTERRETURN,
		EXEC_STEPTORETURN );
}

void CDebugPackage::OnBreakpoints()
{
	void DoBPDlgEx();

	DoBPDlgEx();
}

void CDebugPackage::OnUpdateClearBreakpoints(CCmdUI* pCmdUI)
{
	BOOL bEnabled = FALSE;

	if (ProjectCommandUpdate( pCmdUI->m_nID, &bEnabled ))
		return;

    // Debugable?
    if ( gpIBldSys != NULL )
	{
		if ( gpIBldSys->IsActiveBuilderValid() == S_OK )
		{
			int iAttrs;

			gpIBldSys->GetTargetAttributes(ACTIVE_BUILDER, &iAttrs);

			if ( (iAttrs & TargetIsDebugable) != 0 )
				bEnabled = TRUE;
		}
	}

	pCmdUI->Enable(bEnabled);
}

void CDebugPackage::OnClearBreakpoints()
{
	ClearCV400Breakpoints();
	ClearBreakpointNodeList();
	UpdateBPInViews();
}

void CDebugPackage::OnQuickWatch()
{
	CQuickW quickW;
	quickW.DoModal();
}


BOOL CDebugPackage::OnDebugBreak(UINT nCmdId)
{
	BOOL bEnable;

	if (CommandIdEnabled(nCmdId, bEnable))
	{
	AsyncStopDebuggee();

		return( TRUE );
	}
	else
		return( FALSE );
}

void CDebugPackage::OnDebugRadix()
{
	runDebugParams.decimalRadix = !runDebugParams.decimalRadix;
	UpdateRadix(runDebugParams.decimalRadix);
	SetPrompt(runDebugParams.decimalRadix ? IDS_DBG_RADIX_DECIMAL : IDS_DBG_RADIX_HEX);
}

void CDebugPackage::OnDisableAllBreakpoints()
{
    DisableAllBreakpoints();
	UpdateBPInViews();
}

void CDebugPackage::OnUpdateDisableAllBreakpoints(CCmdUI* pCmdUI)
{
	// martynl 28May96 - you can do this on the same occasions when you can 
	// clean breakpoints, so we defer to that.
	OnUpdateClearBreakpoints(pCmdUI);	
}

#ifdef CODECOVERAGE

void CDebugPackage::OnCodeCoverageToggle()
{
	runDebugParams.fCodeCoverage = !runDebugParams.fCodeCoverage;
}

void CDebugPackage::OnCodeCoverageClear()
{
	gpISrc->ClearAllDocStatus(COVERAGE_LINE);
}

#endif

void CDebugPackage::OnDebugSetIP()
{
	//Move instruction pointer to the line the cursor is on
	if (!MoveIPToCursor())
		MessageBeep(0);
}

void CDebugPackage::OnDebugActive()
{
	CTLOpt dlg;

	if ( dlg.DoModal() == IDOK )
	{
		// commit changes to TL selections for all edited platforms...
		ASSERT(dlg.m_rgPlatforms);      // if we got OK these must have been set...
		ASSERT(dlg.m_rgiTL);
		ASSERT(dlg.m_cPlatforms);

		for (int i=0; i<dlg.m_cPlatforms; i++)
		{
			uniq_platform   upPlatform = dlg.m_rgPlatforms[i];
			UINT            itl = dlg.m_rgiTL[i];
			UINT            iTL;

			gpIBldPlatforms->GetPlatformCurrentTLIndex(upPlatform, &iTL);

			if ( iTL != itl )
				gpIBldPlatforms->SetPlatformCurrentTLIndex(upPlatform, itl);
		}

		// Make sure we are intialized already. 
		pDebugCurr->Init(FALSE);

		// update current debug DLLs for this project (NULL OK)
		pDebugCurr->OnConfigChange();

	if ( dlg.m_fConnectNow )
	{
	    // Blindly close any open project.
			BOOL    fSuccess = SUCCEEDED(gpIProjectWorkspace->CloseWorkspace());

			// FUTURE - need to put up some UI saying we couldn't close
			// the current project, so we can't JIT.
			if ( !fSuccess )
				return;

			ASSERT(pDebugCurr);

			if (pDebugCurr)
			{
				// Update the platform. NOTE: OnConfigChange above may not have done this.
				pDebugCurr->ConfigChange(dlg.m_rgPlatforms[dlg.m_iPlatform]);
			}

	    // commit changes to TL selections for all edited platforms...
	    gpISrc->StatusText(DBG_Connecting,STATUS_INFOTEXT,FALSE);

	    theApp.m_jit.SetPid(0);
	    theApp.m_jit.SetEvent(0);

	    theApp.m_jit.FSetPath("", FALSE);
	    theApp.m_jit.SetActive(TRUE);

	    ::SetForegroundWindow(theApp.m_pMainWnd->GetSafeHwnd());
	    theApp.m_pMainWnd->UpdateWindow();

	    // start debugging; we'll figure out that it's a jit and
	    //  act accordingly
	    gpISrc->StatusText(SYS_StatusClear, STATUS_INFOTEXT, FALSE);
	    ControlExecDebuggee (IDM_RUN_GO, EXEC_GO );
	}
    }
}

void CDebugPackage::OnUpdateDebugRadix(CCmdUI* pCmdUI)
{
	BOOL bEnabled = FALSE;

	// Because this is available from an accelerator as well as from
	// the locals and watch context popups, enabling is a bit more
	// complex than it would be for the popups alone.
	if ( gpIBldSys )
	{
		if ( gpIBldSys->IsActiveBuilderValid() == S_OK )
		{
			int     iAttrs;

			gpIBldSys->GetTargetAttributes(ACTIVE_BUILDER, &iAttrs);

			if ( iAttrs & TargetIsDebugable )
			{
				if ( !DebuggeeAlive() || !DebuggeeRunning() )
					bEnabled = TRUE;
			}
		}
		else
			ProjectCommandUpdate( pCmdUI->m_nID, &bEnabled );
	}

	pCmdUI->Enable(bEnabled);
	pCmdUI->SetCheck(!runDebugParams.decimalRadix);
}

void CDebugPackage::OnUpdateDebugOptions(CCmdUI* pCmdUI)
{
	BOOL bEnable;
	if (ProjectCommandUpdate( pCmdUI->m_nID, &bEnable ))
	{
		pCmdUI->Enable( bEnable );
		return;
	}

	pCmdUI->Enable(TRUE);

	switch (pCmdUI->m_nID)
	{
	case IDM_DBGSHOW_FLOAT:
		pCmdUI->SetCheck(runDebugParams.fCPUFloat);
		break;
		
	case IDM_DBGSHOW_STACKTYPES:
		pCmdUI->SetCheck(runDebugParams.fCallTypes);
		break;
		
	case IDM_DBGSHOW_STACKVALUES:
		pCmdUI->SetCheck(runDebugParams.fCallParams);
		break;
		
	case IDM_DBGSHOW_SOURCE:
		pCmdUI->SetCheck(runDebugParams.fDAMAnnotate);
		break;

	case IDM_DBGSHOW_CODEBYTES:
		pCmdUI->SetCheck(runDebugParams.fDAMCodeBytes);
		break;

	case IDM_DBGSHOW_MEMBYTE:
		pCmdUI->SetCheck(runDebugParams.iMWFormat == MW_BYTE);
		break;

	case IDM_DBGSHOW_MEMSHORT:
		pCmdUI->SetCheck(runDebugParams.iMWFormat == MW_INT_HEX);
		break;

	case IDM_DBGSHOW_MEMLONG:
		pCmdUI->SetCheck(runDebugParams.iMWFormat == MW_LONG_HEX);
		break;

	case IDM_DBGSHOW_MEMORY_BAR:
		pCmdUI->SetCheck(runDebugParams.fMemoryToolbar);
		break;

	default:
		ASSERT(FALSE);
	}
}

BOOL IsCurrentVisualTestDoc()
{
	if (!fVisTestPresent)
		return FALSE;
	
	LPSOURCEEDIT pISourceEdit = NULL;
	ASSERT(gpISrcQuery);
	if (SUCCEEDED(gpISrcQuery->CreateSourceEditForActiveDoc(&pISourceEdit, FALSE)) && NULL != pISourceEdit)
	{
		CString strLanguage;
		pISourceEdit->GetLanguage(strLanguage);
		pISourceEdit->Release();
		if (strLanguage == _T("Visual Test"))
			return TRUE;
	}
	return FALSE;
}

BOOL IsCurrentVCOrFortranDoc()
{
	COleRef<ISourceEdit> pISourceEdit;
	return gpISrcQuery 
				&& SUCCEEDED(gpISrcQuery->CreateSourceEditForActiveDoc(&pISourceEdit, FALSE))
				&& pISourceEdit;
		// Let's simply accept any text document.  This allows .ASM files
		// for Set Next Statement and Run to Cursor.
}

BOOL CDebugPackage::OnToggleBreakpoint(UINT)
{
	if ( !gpIBldSys && !fVisTestPresent )
    {
		MessageBeep(0);
		return TRUE;
    }

	// If current document is a Visual Test file pass on the command.
	if (IsCurrentVisualTestDoc())
		return FALSE;

	if (!ToggleLocBP())
		MessageBeep(0);

	return TRUE;
}

void CDebugPackage::OnEnableDisableBreakpoint()
{
	BOOL fBrkEnabled = FALSE;

	if (!gpIBldSys)
		return;

	UINT uiState = GetBPLineState();
	if ( uiState & HAS_BRKPOINT )
	{
		if ( uiState & (BRKPOINT_LINE | MIXED_BRKPOINT_LINE) )
		{
			// there is at least one enabled breakpoint
			fBrkEnabled = TRUE;
		}
		EnableDisableLocBP( fBrkEnabled );
	}
	else
		MessageBeep(0);
}

void CDebugPackage::OnGotoCurrentInstruction()
{
	if (DebuggeeAlive() && !DebuggeeRunning())
	{
		// get current view
		CView *pViewCur = GetCurView();

		// lock the frame while we're mucking with windows
//		CPartFrame::LockFrame(TRUE);

		// If the DAM window is MDI and the current view, we want to 
		// prevent a source window from popping up on top of it.
		if (pViewCur != NULL && 
			pViewCur == pViewDisassy && 
			DkWGetDock(MAKEDOCKID(PACKAGE_DEBUG, IDDW_VCPP_DISASSY_WIN)) == dpMDI
			)
		{
			CPartFrame::LockFrame(pViewDisassy->GetParentFrame()->GetSafeHwnd());
		}
		else
		{
			// we don't need to do anything special
			pViewCur = NULL;
		}

		// set back to stack frame 0
		// This takes care of navigating to the source line 
		// and updating the locals and watch windows.
		CLDispCallInfo(0);

//DS96 #10079 [CFlaat]: we avoid setting the active view after the frame is unlocked to avoid randomizing the min/max/normal state

		if (pViewCur) // we only locked the frame if it was disasm in MDI state
			CPartFrame::LockFrame(FALSE);

//			theApp.SetActiveView(pViewCur->GetSafeHwnd()); // was causing bug #10079, due to superfluous ShowWindow calls

	}
	else
		::MessageBeep(0);
}

void CDebugPackage::OnUpdateGotoCurrentInstruction(CCmdUI* pCmdUI)
{
	BOOL fEnable;
	if (!ProjectCommandUpdate( pCmdUI->m_nID, &fEnable ))
		fEnable = DebuggeeAlive() && !DebuggeeRunning();

	pCmdUI->Enable( fEnable );
}

void CDebugPackage::OnUpdateDebugUpdateImageToggle(CCmdUI* pCmdUI)
{
	if(DebuggeeAlive() && gpIBldSys && gpIBldSys->IsBuildInProgress() == S_OK)
	{
		// Replace "Update Image" with "Stop Build"
		CString str;
		str.LoadString(IDS_Enc_StopBuildMenu);

		GetCmdKeyString(IDM_PROJECT_STOP_BUILD, str);

		pCmdUI->SetText(str);
		pCmdUI->Enable(TRUE);
	}
	else
	{
		// Use "Update Image"
		OnUpdateDebugUpdateImage(pCmdUI);
	}
}

void CDebugPackage::OnUpdateDebugUpdateImage(CCmdUI* pCmdUI)
{
	BOOL fMenu = ::IsMenu(pCmdUI);
	if (fMenu && !IsContextMenu(pCmdUI)) {
		CString str;
		str.LoadString(IDS_Enc_UpdateImageMenu);

		GetCmdKeyString(IDM_DEBUG_UPDATEIMAGE, str);

		pCmdUI->SetText(str);
	}

	BOOL fEnable;
	if (!CommandIdEnabled(IDM_DEBUG_UPDATEIMAGE, fEnable))
		fEnable = FALSE;
	pCmdUI->Enable(fEnable);
}

void CDebugPackage::OnNextMemWinFmt()
{
	MWScanType(1);
}

void CDebugPackage::OnPrevMemWinFmt()
{
	MWScanType(-1);
}

void CDebugPackage::OnUpdateDebugActive(CCmdUI* pCmdUI)
{
    BOOL fEnable = FALSE;

	if (!ProjectCommandUpdate( pCmdUI->m_nID, &fEnable ))
	{
		if( !DebuggeeAlive() )
			fEnable = TRUE;

		// 10-14-96 jimsc  If Java is the active project, then disable the remote debugging menu.	
		uniq_platform upCurrent = unknown_platform;
		if (SUCCEEDED(gpIBldPlatforms->GetCurrentPlatform(ACTIVE_BUILDER, &upCurrent)))
		{
			if (upCurrent == java)
				fEnable = FALSE;
		}
		else
			fEnable = FALSE;   // this should catch the IS projects.
	}

    pCmdUI->Enable( fEnable );
}

// REVIEW: PIERSH
void CDebugPackage::OnRemoteDebugger()
{
	ActivateRemoteDebugger();
}

void CDebugPackage::OnUpdateActivateSrcWnd(CCmdUI* pCmdUI)
{
	DkWHandleDockingCmd(pCmdUI->m_nID, FALSE, pCmdUI);
}

void CDebugPackage::OnActivateSrcWnd(UINT nCmd)
{
	DkWHandleDockingCmd(nCmd, FALSE);
}

void CDebugPackage::OnUpdateToggleSrcWnd(CCmdUI* pCmdUI)
{
	DkWHandleDockingCmd(pCmdUI->m_nID, TRUE, pCmdUI);
}

void CDebugPackage::OnToggleSrcWnd(UINT nCmd)
{
	DkWHandleDockingCmd(nCmd, TRUE);
}

void CDebugPackage::OnUpdateProjectExecute(CCmdUI * pCmdUI)
{
	BOOL bEnabled = FALSE;


	BOOL fIsAProject = ((gpIBldSys != NULL) && (gpIBldSys->IsActiveBuilderValid() == S_OK));

	// if we have a project and the current target is unknown then
	// we can't do this
	if (fIsAProject)
	{
		int iAttrs;
		gpIBldSys->GetTargetAttributes(ACTIVE_BUILDER, &iAttrs);
        
		if (iAttrs & TargetUnknown || !(iAttrs & TargetIsDebugable))
		{
			pCmdUI->Enable(FALSE);
			return;
		}
	}
	else if (ProjectCommandUpdate( pCmdUI->m_nID, &bEnabled ))
	{
		pCmdUI->Enable( bEnabled );
		return;
	}

	BOOL fIsExecutable = FALSE ;			// Project target is an EXE
	BOOL fInThread = FALSE; 						// A second thread has started
	BOOL fJustInTime = theApp.m_jit.GetActive();	// Just-in-time debugging
	CString strTarget;										// our target name

	BOOL fAllowExecute = FALSE;

	TCHAR szExecutable[_MAX_PATH]; *szExecutable = _T('\0');

	EXEFROM exefrom = GetExecutableFilename(szExecutable, _MAX_PATH);

    ASSERT(gpIBldSys != NULL);
	fInThread = (gpIBldSys->IsBuildInProgress() == S_OK);

	// If remote debugging, get the remote target name
	if (pDebugCurr && pDebugCurr->MtrcRemoteTL() && fIsAProject)
	{
		CString strRemoteExe;

		gpIBldSys->GetRemoteTargetFileName(ACTIVE_BUILDER, strRemoteExe);

		// Save just the filename portion
		strTarget = LtszRemoteFileFromPath(strRemoteExe);
	}
	else
	{
		// Save just the filename portion
		strTarget = LtszRemoteFileFromPath(szExecutable);
	}

#if 0
	// We used to allow you to execute an exe if the focus was
	// currently in a resource window. However we need to be able to
	// open a project for this and the build system started refusing to 
	// open projects for 16-bit exes. This change doesn't allow you to execute
	// exes on the basis of the fact that the focus is currently within
	// a resource editor window ( which could be 16 or 32 bit) . Olympus:12410  
	BOOL fExeHasFocus = (exefrom == exefromExe);
#endif

	fJustInTime = (exefrom == exefromPid);
	if (fIsAProject)
	{
		CString strExe;
		gpIBldSys->GetCallingProgramName(ACTIVE_BUILDER, strExe);
		fIsExecutable = !strExe.IsEmpty();
	}

	// first set the 'Execute' target text
	// Set our text to be of the form:
	// 'Execute <Target>\tCtrl+F8'
	CString strExecuteMenu; strExecuteMenu.LoadString(IDS_PROJMENU_EXECUTE);

#define MENU_LENGTH_CCH 200 	// REVIEW: string buffer constant size
	TCHAR szMenuText[MENU_LENGTH_CCH];

	strTarget = strTarget.Left(MENU_LENGTH_CCH - strExecuteMenu.GetLength());
	MenuFileText(strTarget);

	wsprintf(szMenuText, strExecuteMenu, (LPCTSTR) strTarget);
	pCmdUI->SetText(szMenuText);

	// next enable/disable the 'Execute' target menu
	// [BUG #6047 12/05/92 v-natjm]
	fAllowExecute = ((fIsAProject||fJustInTime) && !fInThread);

	// [dolphin #8022 3/21/94 mikemo]
	if (DebuggeeAlive())
	{
		if (pDebugCurr && !pDebugCurr->MtrcMultipleInstances())
		{
			fAllowExecute = FALSE;
		}
		else if (DebuggeeRunning() && pDebugCurr &&
			!(pDebugCurr->MtrcAsync() & asyncSpawn))
		{
			fAllowExecute = FALSE;
		}
	}
	pCmdUI->Enable(fAllowExecute);

}

void CDebugPackage::OnUpdateEnableDisableBreakpoint(CCmdUI* pCmdUI)
{
    BOOL fEnabled = FALSE;

    if (BHFIsValidWindowForBP())
	{

	// only for a popup menu check if there is a breakpoint on the line
	if ( IsCmdFromPopupMenu(pCmdUI) )
	{
	    UINT uiState = GetBPLineState ();

	    if ( uiState & HAS_BRKPOINT )
	    {
		fEnabled = TRUE;

		if ( uiState & (BRKPOINT_LINE|MIXED_BRKPOINT_LINE) )
		{
		    CString  strMenuItem;

		    strMenuItem.LoadString (IDS_DISABLE_BREAKPOINT);
		    pCmdUI->SetText(strMenuItem);
		}
	    }
	}
	else
	{
	    fEnabled = TRUE;
	}
    }
	pCmdUI->Enable( fEnabled );
}

void CDebugPackage::OnUpdateToggleBreakpoint(CCmdUI* pCmdUI)
{
    BOOL fEnabled = FALSE;

	if (IsCurrentVisualTestDoc())
	{
		pCmdUI->m_bContinueRouting = TRUE;
		return;
	}

    if (BHFIsValidWindowForBP())
    {
		// it is valid window for a breakpoint
		fEnabled = TRUE;
		// only for a popup menu check if there is a breakpoint on the line
		if ( IsCmdFromPopupMenu(pCmdUI) )
		{
			UINT uiState =  GetBPLineState();
			if ( uiState & (BRKPOINT_LINE | MIXED_BRKPOINT_LINE) )
			{
				// there is at least one enabled breakpoint
				CString  strMenuItem;
				VERIFY(strMenuItem.LoadString(IDS_REMOVE_BREAKPOINT));
				pCmdUI->SetText(strMenuItem);
			}
		}
    }

	pCmdUI->Enable( fEnabled );
}

BOOL CDebugPackage::OnDebugGoPostMsg(UINT nCmdId)
{
	BOOL bEnable;
	if (CommandIdEnabled(nCmdId, bEnable))
	{
		if (!ProjectCommandExecute(nCmdId))
		{
			// If this was called from an automation event handler via 
			//  ExecuteCommand, then DEFER the DebugGo until after
			//  the event handler returns
			if (lpprcCurr && lpprcCurr->m_bInsideEvent)
				lpprcCurr->m_bGoDeferred = TRUE;
			else
				AfxGetMainWnd()->PostMessage (WM_COMMAND, ID_RUN_GO);
		}
		return TRUE;
	}
	else
		return FALSE;
}

void CDebugPackage::OnDebugRestartPostMsg()
{
	AfxGetMainWnd()->PostMessage (WM_COMMAND, ID_RUN_RESTART);
}

BOOL CDebugPackage::OnDebugStopPostMsg(UINT nCmdId)
{
	BOOL bEnable;
	if (CommandIdEnabled(nCmdId, bEnable))
	{
		if (!ProjectCommandExecute(nCmdId))
		{
			// If this was called from an automation event handler via 
			//  ExecuteCommand, then DEFER the DebugStop until after
			//  the event handler returns
			if (lpprcCurr && lpprcCurr->m_bInsideEvent)
				lpprcCurr->m_bStopDeferred = TRUE;
			else
				AfxGetMainWnd()->PostMessage (WM_COMMAND, ID_RUN_STOPDEBUGGING);
		}
		return TRUE;
	}
	else
		return FALSE;
}

void CDebugPackage::OnDebugToCursorPostMsg()
{
	if (!ProjectCommandExecute( IDM_RUN_TOCURSOR ))
		AfxGetMainWnd()->PostMessage (WM_COMMAND, ID_RUN_TOCURSOR);
}

BOOL CDebugPackage::OnDebugTraceIntoPostMsg(UINT nCmdId)
{
	BOOL bEnable;
	if (CommandIdEnabled(nCmdId, bEnable))
	{
		if (!ProjectCommandExecute( IDM_RUN_TRACEINTO ))
			AfxGetMainWnd()->PostMessage (WM_COMMAND, ID_RUN_TRACEINTO);
		return TRUE;
	}
	else
		return FALSE;
}

void CDebugPackage::OnDebugTraceFunctionPostMsg()
{
	AfxGetMainWnd()->PostMessage (WM_COMMAND, ID_RUN_TRACEFUNCTION);
}

BOOL CDebugPackage::OnDebugStepOverPostMsg(UINT nCmdId)
{
	BOOL bEnable;
	if (CommandIdEnabled(nCmdId, bEnable))
	{
		if (!ProjectCommandExecute( nCmdId ))
			AfxGetMainWnd()->PostMessage (WM_COMMAND, ID_RUN_STEPOVER);
		return TRUE;
	}
	else
		return FALSE;
}

void CDebugPackage::OnDebugSrcTraceIntoPostMsg()
{
	AfxGetMainWnd()->PostMessage (WM_COMMAND, ID_RUN_SRC_TRACEINTO);
}

void CDebugPackage::OnDebugSrcStepOverPostMsg()
{
	AfxGetMainWnd()->PostMessage (WM_COMMAND, ID_RUN_SRC_STEPOVER);
}

BOOL CDebugPackage::OnDebugStopAfterReturnPostMsg(UINT nCmdId)
{
	BOOL bEnable;
	if (CommandIdEnabled(nCmdId, bEnable))
	{
		AfxGetMainWnd()->PostMessage (WM_COMMAND, ID_RUN_STOPAFTERRETURN);
		return TRUE;
	}
	else
		return FALSE;
}

//
// Bring memory image up-to-date with source changes
//
void CDebugPackage::OnDebugUpdateImage()
{
	if (DebuggeeAlive() && DebuggeeRunning()) {
		// The debuggee is running. Do the following:
		//		Break
		//		Apply code changes
		//		Run
		AsyncStopDebuggee();
		if (lpprcCurr) {
			lpprcCurr->m_bEncGoPending = TRUE;
		}
	}
	else {
		if (lpprcCurr && lpprcCurr->m_bEncGoPending) {
			lpprcCurr->m_bEncGoPending = FALSE;
			ExecDebuggee(EXEC_ENC_FORCEGO);
		}
		else {
			ExecDebuggee(EXEC_EDITANDCONTINUE);
		}
	}
}

void CDebugPackage::OnDebugUpdateImageToggle()
{
	if(DebuggeeAlive() && gpIBldSys && gpIBldSys->IsBuildInProgress() == S_OK)
	{
		AfxGetMainWnd()->PostMessage (WM_COMMAND, IDM_PROJECT_STOP_BUILD);
	}
	else {
		OnDebugUpdateImage();
	}
}

// return true if no Language packages are loaded (e.g. Visual Test stand-alone)
// or false if any are loaded

bool NoLanguagesLoaded()
{
	static bool bLangCheck = false;			// have we checked it
	static bool bNoLangLoaded = false;		// and result of the check
	if (!bLangCheck)
	{
		bNoLangLoaded = !(IsPackageLoaded( PACKAGE_LANGCPP ) || IsPackageLoaded( PACKAGE_LANGFOR ) );
		bLangCheck = true;
	}
	return bNoLangLoaded;
}

UINT CommandIdEnabled(UINT CommandId, BOOL& Enabled)
{
	BOOL Alive;                                                     // Debuggee is alive
	BOOL Running;                                           // Debuggee is running
	BOOL IsAProject;                                        // Project opened
	BOOL ExeHasFocus;                                       // An EXE window has focus
	BOOL IsExecutable = FALSE ;                     // Project target is an EXE
	BOOL IsToDebug = FALSE ;                        // Project type debuggable
	BOOL IsRunDebugParam = TRUE ;           // Ext Proj without RunDeb params
	BOOL InBuild = FALSE ;                          // Depends on Wintee and Make engine
	BOOL fInThread = FALSE;                         // A second thread has started
	BOOL JustInTime = theApp.m_jit.GetActive();     // Just-in-time debugging
	EXEFROM exefrom;
	BOOL fVisTestDoc = FALSE;       // currently active doc is a Visual Test Document.
	BOOL fENC = ENCIsRebuilding();	// a recompile/relink is taking place for edit & continue

    // Debugger status flags
	Alive = DebuggeeAlive();
	Running = (Alive && DebuggeeRunning());
	fVisTestDoc = IsCurrentVisualTestDoc();
	IsAProject = ((gpIBldSys != NULL) && (gpIBldSys->IsActiveBuilderValid() == S_OK)) ? TRUE : FALSE;

	if (gpIBldSys == NULL)
	{
		Enabled = FALSE;
		return FALSE;
	}

	if (IsAProject)
	{
		int     ProjFlags;

		gpIBldSys->GetTargetAttributes(ACTIVE_BUILDER, &ProjFlags);

		IsExecutable    = ProjFlags & ImageExe;
		IsToDebug       = ProjFlags & TargetIsDebugable;
		IsRunDebugParam = TRUE;
	}
	else
	{
		// Active Project is not a builder, but maybe it wants to
		// run its own command enabling
		if (ProjectCommandUpdate( CommandId, &Enabled ))
			return TRUE;

		IsExecutable    = FALSE;
		IsToDebug       = FALSE;
		IsRunDebugParam = FALSE;
	}

	InBuild  = (gpIBldSys->IsBuildInProgress() == S_OK);

	switch ( CommandId )
	{
		case IDM_RUN_REMOTEDEBUGGER:
			if( pDebugCurr && pDebugCurr->MtrcNativeDebugger() )
				Enabled = Alive && !Running && !InBuild && !fENC;
			else
				Enabled = FALSE;
			break;

		case IDM_GOTO_CODE:
			Enabled = Alive && !Running;
			break;

		case IDM_GOTO_SOURCE:
		case IDM_GOTO_DISASSY:
		case IDM_VIEW_MIXEDMODE:
			Enabled = Alive;
			break;

		// Always enabled.  Should only come from the shell!
		case IDM_RUN_JIT_GO:
			Enabled = TRUE;
			break;

		case IDM_RUN_GO:
		case IDM_RUN_TRACEINTO:
		case IDM_RUN_STEPOVER:
		case IDM_RUN_STOPAFTERRETURN:
			// If we are currently in an Visual Test debug session or in a
			// Visual Test document let the command routing continue.
			// Also if VC is not installed pass the command on to
			// Test and let it do the appropriate thing.    
			if ( !Alive && (NoLanguagesLoaded() || fVisTestDoc || fVisTestDebugActive) )
				return FALSE;
			// Fall through

		case IDM_RUN_TRACEFUNCTION:
		case IDM_RUN_RESTART:
		case IDM_RUN_TOCURSOR:
		case IDM_RUN_SRC_TRACEINTO:
		case IDM_RUN_SRC_STEPOVER:
		case IDM_RUN_SETNEXTSTMT:
			{
				BOOL IsASource = FALSE;                                // known source file type
				exefrom     = GetExecutableFilename(NULL, 0);
				ExeHasFocus = (exefrom == exefromExe);
				
				// Restart, Go, ToCursor, TraceInto, StepOver:
				// We can execute the debuggee if he is already there OR
				// he can be/is built and is of the right type.
				
				// allow debugging command if we can build without project
				// Cannot debug while a build is running

				// File flags
				IsASource = !fVisTestDoc;
				if (IsASource) // maybe
				{
					IsASource = IsCurrentVCOrFortranDoc(); 
					if (!IsAProject && IsASource)
						IsToDebug = TRUE;
				}

				// was:  ... !fIsLaunchingDebuggee && ...
				
				Enabled = (!Running && !InBuild && !fInThread && !fENC &&
					(Alive || ((IsAProject || IsASource || ExeHasFocus || JustInTime) && IsToDebug)));
				
				// In addition, for RUN_TOCURSOR or SETNEXTSTMT, caret must be in a
				// document window or in the ribbon edit box
				if ( CommandId == IDM_RUN_TOCURSOR || CommandId == IDM_RUN_SETNEXTSTMT)
				{
					CView    *pViewActive   = GetCurView();
//					BOOL    fBrowseActive  = pWndActive && pWndActive->IsKindOf(RUNTIME_CLASS(CFloatingFrameWnd));
					
					Enabled = Enabled &&
						(IsFindComboActive() || IsASource ||
							(pViewActive && (
								pViewActive->IsKindOf(RUNTIME_CLASS(CDAMView)) ||
								pViewActive->IsKindOf(RUNTIME_CLASS(CCallsView))
							))
						);
				}
				
				// In addition, for RUN_STOPAFTERRETURN, there is no reason
				// to be enabled if the debuggee is not alive because
				// we have no caller !...
				// also SETNEXTSTMT requires that we already *have* an IP to move...
				if ( CommandId == IDM_RUN_STOPAFTERRETURN || CommandId == IDM_RUN_SETNEXTSTMT )
					Enabled = Enabled && Alive;
				
				// If doing just-in-time debugging, Restart can't work until
				// we've actually loaded a project.
				if ( CommandId == IDM_RUN_RESTART && JustInTime )
					Enabled = Enabled && IsAProject;

				// Step Info Specific Function (IDM_RUN_TRACEFUNCTION) is not currently
				// supported for Java.
				if (CommandId == IDM_RUN_TRACEFUNCTION && pDebugCurr && pDebugCurr->IsJava())
				{
					Enabled = FALSE;
				}
				break;
			}

		case IDM_DEBUG_UPDATEIMAGE:
			Enabled =  Alive && !InBuild && !fInThread && !fENC && 
				gpISrc && gpISrc->ENCStateModified();
			break;

		case IDM_RUN_STOPDEBUGGING:
			if ( !Alive && fVisTestDebugActive )
			{
				ASSERT(!Alive); // We couldn't be debugging at the same time.
				return FALSE;
			}

			Enabled = (Alive && !InBuild);
			break ;

		case IDM_RUN_BREAK:
			if ( fVisTestDebugActive )
			{
				ASSERT(!Alive);
				return FALSE;
			}

			if (  pDebugCurr && ((pDebugCurr->MtrcAsync() & asyncStop)) )
				Enabled = (Alive && Running && !TryingToAsyncStop() && !fENC);
			else
				Enabled = FALSE;

			break;

		case IDM_RUN_THREADS:
			Enabled = (pDebugCurr && pDebugCurr->MtrcHasThreads() && Alive && !Running && !InBuild && !fENC);
			break;

		case IDM_RUN_EXCEPTIONS:
		case IDM_RUN_FLIPHACK:
			Enabled = pDebugCurr && pDebugCurr->MtrcFirstChanceExc() && Alive && !InBuild && !fENC;
			break;

		case IDM_RUN_QUICKWATCH:
			// let quickwatch be accessible anytime since we can handle the "no expr" case
			// was: ... && !fIsLaunchingDebuggee ...
			Enabled = pDebugCurr && IsToDebug && !(Alive && Running) && !fENC;
			break;

		case IDM_RUN_SETBREAK:

			// if the interface exists on the active project, go use it
			if (GetActiveIPkgProject())
			{
				GUID guidProj;

				GetActiveIPkgProject()->GetClassID( &guidProj );
				if (guidProj != IID_IBSProject)
					Enabled = TRUE;
			}

			Enabled = Enabled && IsAProject;

			if( pDebugCurr && (pDebugCurr->MtrcAsync() & asyncBP) )
				Enabled = Enabled && (IsToDebug);
			else
				Enabled = Enabled && (!Running && IsToDebug);

			break;

		case IDM_RUN_ATTACH_TO_ACTIVE:

			//
			// Attaching to an active process is not currently supported
			// for Java.

			if (InBuild || (IsAProject && pDebugCurr && pDebugCurr->IsJava ()) || fENC)
				Enabled = FALSE;
			else
				Enabled = TRUE;

			//
			// If the CPP package is not available, we cannot attach to active.
			//
			
			if (!(IsPackageLoaded (PACKAGE_LANGCPP)||IsPackageLoaded(PACKAGE_LANGFOR))) {
				Enabled = FALSE;
			}
				
			break;

		default:
			return FALSE;
	}

	return TRUE;
}


BOOL CDebugPackage::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo)
{

	if (CPackage::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo))
		return( TRUE );

	switch (nCode)
	{
	case CN_UPDATE_COMMAND_UI:
	{
		if(	nID>=MIN_SOURCE_COMMAND_ID &&
			nID<=MAX_SOURCE_COMMAND_ID)
		{
			ASSERT(pExtra != NULL);

			BOOL bEnable = FALSE;

				if (CommandIdEnabled(nID, bEnable))
				{
					((CCmdUI*)pExtra)->Enable(bEnable);
					return( TRUE );
				}
		}
	    break;
	}

	case CN_COMMAND:
		if ((pExtra == NULL) && (pHandlerInfo == NULL))
		{
			extern BOOL MainWnd_OnCommand(WPARAM, LPARAM);

			return( MainWnd_OnCommand(nID, 0) );
		}
		break;
	}

	return( FALSE );
}

extern CDlgTab* CreateDebugPage();

typedef struct CDTC {
	CDlgTab *       (*lpfnCreate)(void);    // Pointer to page creation function
	int nOrder;
} CDTC; // CDlgTab Create

const CDTC      rgcdtc[] =
{
	{ CreateDebugPage, 15 }
};
#define ccdtc   (sizeof(rgcdtc)/sizeof(CDTC))

void CDebugPackage::AddOptDlgPages(class CTabbedDialog* pOptionsDialog, OPTDLG_TYPE type)
{

	switch (type)
	{
	case options:
	{
	    int icdtc;

	    for (icdtc = 0; icdtc < ccdtc; icdtc += 1)
	    {
		CDlgTab* pTab = (*rgcdtc[icdtc].lpfnCreate)();
		if ( pTab ) {
		    pTab->m_nOrder = rgcdtc[icdtc].nOrder;
		    pOptionsDialog->AddTab(pTab);
		}
	    }
	}
	break;

	case customize:
		// add customize pages here...
		break;
	}
}



void
CDebugPackage::OnAttachToActive(
	)
{
	BOOL				fSucc;
	HRESULT				status;
	CAttachToActive		dlg;
	CDocument*			unused = NULL;
	uniq_platform		platform;
	UINT				iTL;
	

	ASSERT (gpIBldSys);

	// this should probablly never happen, but just in case.
	
	if (gpIBldSys->IsBuildInProgress () == S_OK)
		return;
	
	if (dlg.DoModal () == IDOK)
	{
		// First make sure we have initialized everything properly.
		if (pDebugCurr)
			pDebugCurr->Init(FALSE);
		//
		// when attaching to active, turn off remoting

		gpIBldPlatforms->GetCurrentPlatform (ACTIVE_BUILDER, &platform);

		if (platform == unknown_platform)
			platform = native_platform;
			
		gpIBldPlatforms->GetPlatformLocalTLIndex (platform, &iTL);
		gpIBldPlatforms->SetPlatformCurrentTLIndex (platform, iTL);

		if (pDebugCurr)
			pDebugCurr->ConfigChange (platform);
		
		theApp.m_jit.SetEvent (NULL);
		theApp.m_jit.SetPid (dlg.m_ProcessId);
		theApp.m_jit.SetActive (TRUE);
		theApp.m_jit.FSetPath (dlg.m_ImageName, TRUE);

		//
		//	load the new workspace

//		status = gpIProjectWorkspace->CloseWorkspace ();

		//
		// OpenWorkspace automatically closes the opened workspace.
		//
		
		AfxGetApp()->BeginWaitCursor ();

		status = gpIProjectWorkspace->OpenWorkspace (&unused,
												  theApp.m_jit.GetPath (),
												  TRUE
												  );

		AfxGetApp()->EndWaitCursor ();

		if (SUCCEEDED (status))
		{
			ExecDebuggee (EXEC_GO);
		}
		else
		{
			theApp.m_jit.SetActive (FALSE);
		}
	}
}

#ifdef _DEBUG
#define ENT_NAME "DEVENTD.PKG"
#else
#define ENT_NAME "DEVENT.PKG"
#endif

BOOL CDebugPackage::IsVisibleProjectCommand(UINT nID, GUID *pGuids, UINT cGuidCount)
{
	// Breakpoints dlg box pertain to C++ or Java only at the moment...
	// also Data projects, but only when ENT package is present
	// this guid method sucks - how about passing interface pointers? [apennell]
	BOOL bShowIt = FALSE;
	// TRUE if the presence of a data project is enough to show this command
	BOOL bAllowData=TRUE;
	if (cGuidCount > 0)
	{
		UINT idx;
		switch (nID)
		{
		case IDM_RUN_SETBREAK:
		case IDM_RUN_TRACEINTO:
		case IDM_RUN_TOCURSOR:
			for (idx = 0; idx < cGuidCount && !bShowIt; idx++)
				bShowIt = (pGuids[idx] == IID_IBSCxxProject) ||
							(pGuids[idx] == IID_IBSJavaProject) ||
							(pGuids[idx]==IID_IBSExeProject) ||
							(pGuids[idx]==IID_ITestProject) ||
							(bAllowData && (pGuids[idx]==IID_DataPkgProject) && GetModuleHandle( ENT_NAME ));
			break;

		case IDM_ACTIVATE_MEMORY:
		case IDM_ACTIVATE_REGISTERS:
		case IDM_ACTIVATE_DISASSEMBLY:
			bAllowData=FALSE;
			// deliberate drop thru
		case IDM_ACTIVATE_WATCH:
		case IDM_ACTIVATE_CALLSTACK:
		case IDM_ACTIVATE_VARIABLES:
			for (idx = 0; idx < cGuidCount && !bShowIt; idx++)
				bShowIt = (pGuids[idx] == IID_IBSCxxProject) ||
							(pGuids[idx] == IID_IBSJavaProject) ||
							(pGuids[idx]==IID_IBSExeProject) ||
							(bAllowData && (pGuids[idx]==IID_DataPkgProject) && GetModuleHandle( ENT_NAME ));
			break;

		default:
			ASSERT(FALSE);
			break;
		}
	}
	return (bShowIt);
}

