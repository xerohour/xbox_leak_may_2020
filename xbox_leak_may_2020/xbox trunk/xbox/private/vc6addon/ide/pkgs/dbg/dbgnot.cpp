#include "stdafx.h"
#pragma hdrstop
#include "idbgproj.h"

BOOL CDebugPackage::OnNotify(UINT id, void *Hint, void *ReturnArea)
{
	switch (id)
	{
		case PN_QUERY_OPEN_PROJECT:
		case PN_QUERY_CLOSE_PROJECT:
		case PN_QUERY_CLOSE_WORKSPACE:
		case PN_QUERY_NEW_PROJECT:
		case PN_QUERY_NEW_WORKSPACE:
		case PN_QUERY_BEGIN_BUILD:
			return StopDebugConfirm();

		case PN_QUERY_CHANGE_CONFIG:
			return StopDebugConfirm(Hint != NULL ? (BOOL)(*((BOOL *)Hint)) : FALSE);

		case PN_WORKSPACE_INIT:
		case PN_CHANGE_PLATFORM:
			if (pDebugCurr)
			{
				pDebugCurr->Init(FALSE);
				pDebugCurr->OnConfigChange();
			}
			break;

		case PN_CHANGE_CONFIG:
			if ( pDebugCurr )
			{
				pDebugCurr->Init(FALSE);
				pDebugCurr->OnConfigChange();
			}

			// lose cached interfaces
			if(gpActiveIDBGProj)
			{
				gpActiveIDBGProj->Release();
				gpActiveIDBGProj=NULL;
			}

			if(gpActiveIPkgProject)
			{
				gpActiveIPkgProject->Release();
				gpActiveIPkgProject=NULL;
			}

            break;

		case PN_FULL_SCREEN:

			FullScreenData.rectFullScreen = ((FULLSCREENDATA *)Hint)->rectFullScreen;
			FullScreenData.bVert = gpISrc->GetSrcEnvironParam(ENV_VSCROLL);
			FullScreenData.bHorz = gpISrc->GetSrcEnvironParam(ENV_HSCROLL);
			FullScreenData.bFullScreen = TRUE;
			goto RefreshBars;
			break;

		case PN_FULL_SCREEN_END:
			((FULLSCREENDATA *)ReturnArea)->bVert = gpISrc->GetSrcEnvironParam(ENV_VSCROLL);
			((FULLSCREENDATA *)ReturnArea)->bHorz = gpISrc->GetSrcEnvironParam(ENV_HSCROLL);
			FullScreenData.bFullScreen = FALSE;
			//Refresh all opened views to display
			//text with new scrollbars
RefreshBars:
extern void UpdateAllMultiEditScroll();
			UpdateAllMultiEditScroll();
			break;

		case PN_WORKSPACE_CLOSE:
			runDebugParams.strExpr.Empty();
			// clear all breakpoints at CV level and IDE level
			ClearCV400Breakpoints();
			ClearBreakpointNodeList();

			// CAVIAR 4650: Update doc [mikeho]
			gpISrc->ClearAllDocStatus(BRKPOINT_LINE);

			// delete all DLL mapping info
			ClearDLLInfo();

			// We don't want to be the JIT debugger for any apps which the
			// user previously started with Project.Execute()
			ClearPidExecList();

			gpISrc->ResetProjectState();

			// Set the watch info back to its default state.
            g_persistWatch.InitDefault();

			// Destroy the debugger file alias list so we don't add dupes!
			ClearPathMappings();

			// Reset this flag.
			g_fPromptNoSymbolInfo = TRUE;

			break;

		case PN_BLD_INIT_COMPLETE:
			// should be called after build package and all platforms
			// have been loaded and initialized
#if 0 // Moved to workspace init
            if ( gpIBldSys )
            {
				// bld package has initialized
				if (!pDebugCurr->Init(FALSE))
					return FALSE;
			}
#endif

			break;
	}

	return TRUE;
}

// Use this to get the value of the global; the global will be nulled when the project changes
IPkgProject *GetActiveIPkgProject(void)
{
	if(gpActiveIPkgProject==NULL)
	{
		if(gpIProjectWorkspace)
		{
			if(!SUCCEEDED(gpIProjectWorkspace->GetActiveProject( &gpActiveIPkgProject )))
			{
				gpActiveIPkgProject=NULL;
			}
		}
	}

	return gpActiveIPkgProject;
}

// Use this to get the value of the global; the global will be nulled when the project changes
IDBGProj *GetActiveIDBGProj(void)
{
	if (gpActiveIDBGProj == NULL)
	{
		if(GetActiveIPkgProject())
		{
			if(!SUCCEEDED(GetActiveIPkgProject()->QueryInterface(IID_IDBGProject, (LPVOID*)&gpActiveIDBGProj)))
			{
				gpActiveIDBGProj=NULL;
			}
		}
	}
	return gpActiveIDBGProj;
}
