// BldPkg.cpp : Implementation of CBuildPackage
#include "stdafx.h"
#include "bldpkg.h"
#include "path2.h"

void TShellPrint( wchar_t *pStr )
{
	CComPtr<IVsTshell> pVsTshell;
	if ((SUCCEEDED(ExternalQueryService(SID_SVsTshell, IID_IVsTshell, (void **)&pVsTshell))) && (pVsTshell != NULL))
	{
		pVsTshell->DebOutputStringW(pStr);
	}
}

STDMETHODIMP PrjBuild(unsigned argc, char **rgsz)
{
    if (argc != 0) 
	{
		RETURN_INVALID();
	}
    else 
	{
		CComPtr<IVsUIShell> pVsUIShell;
		if (SUCCEEDED(GetBuildPkg()->GetVsUIShell(&pVsUIShell, TRUE /* in main thread */)) && pVsUIShell != NULL)
		{
			pVsUIShell->PostExecCommand(&CLSID_StandardCommandSet97, cmdidBuildSln, NULL, NULL);
		}
        return S_OK;
	}
}

STDMETHODIMP PrjClean(unsigned argc, char **rgsz)
{
    if (argc != 0) 
	{
		RETURN_INVALID();
	}
    else 
	{
		CComPtr<IVsUIShell> pVsUIShell;
		if (SUCCEEDED(GetBuildPkg()->GetVsUIShell(&pVsUIShell, TRUE /* in main thread */)) && pVsUIShell != NULL)
		{
			pVsUIShell->PostExecCommand(&CLSID_StandardCommandSet97, cmdidCleanSln, NULL, NULL);
		}
        return S_OK;
	}
}

STDMETHODIMP PrjShowOutput(unsigned argc, char **rgsz)
{
    if (argc != 0) 
	{
		RETURN_INVALID();
	}
    else 
	{
		CComPtr<IVsUIShell> pVsUIShell;
		if (SUCCEEDED(GetBuildPkg()->GetVsUIShell(&pVsUIShell, TRUE /* in main thread */)) && pVsUIShell != NULL)
		{
			pVsUIShell->PostExecCommand(&CLSID_StandardCommandSet97, cmdidOutputWindow, NULL, NULL);
		}
        return S_OK;
	}
}

STDMETHODIMP PrjWaitBuild(unsigned argc, char **rgsz)
{
    if (argc != 0) 
	{
		RETURN_INVALID();
	}
    else 
	{
		CComPtr<_DTE> pDTE;
		if( SUCCEEDED(ExternalQueryService(SID_SDTE, IID__DTE, (void **)&pDTE )) && pDTE != NULL )
		{
			HRESULT hr;
			CComPtr<_Solution> pSln = NULL;
			CComPtr<SolutionBuild> pSlnBld;

			hr = pDTE->get_Solution((Solution**)&pSln);
			hr = pSln->get_SolutionBuild( &pSlnBld );
			vsBuildState enumBldState = vsBuildStateInProgress;
			while( enumBldState == vsBuildStateInProgress )
			{
				// Query the SolutionBuild object every second for the build status
				hr = pSlnBld->get_BuildState( &enumBldState );
				Sleep( 1000 );
			}
		}
        return S_OK;
	}
}

STDMETHODIMP PrjFileExists(unsigned argc, char **rgsz)
{
    if (argc != 1)
	{
		RETURN_INVALID();
	}
    else
	{
		CStringW strFilename = rgsz[0];
		CPathW path;
		path.Create(strFilename);
	
		if(path.ExistsOnDisk())
			TShellPrint( L"YES\n" );
		else
			TShellPrint( L"NO\n" );
        return S_OK;
	}
}

const TSHELL_CMD g_rgcmdsDbg[] =
{
    {"VCPrjBuildSln", (PfnTshell)PrjBuild, "'VCPrjBuildSln' builds the solution"},
    {"VCPrjCleanSln", (PfnTshell)PrjClean, "'VCPrjCleanSln' Cleans the solution"},
    {"VCPrjShowOutput", (PfnTshell)PrjShowOutput, "'VCPrjShowOutput' SHows the Ouput Window"},
    {"VCPrjFileExists", (PfnTshell)PrjFileExists, "'VCPrjFileExists' Tests If file exists on disk"}
};

void InitTShell()
{
	CComPtr<IVsTshell> pVsTshell;
	if ((SUCCEEDED(ExternalQueryService(SID_SVsTshell, IID_IVsTshell, (void **)&pVsTshell))) && (pVsTshell != NULL))
	{
		pVsTshell->AddCmdTable(sizeof(g_rgcmdsDbg) / sizeof(TSHELL_CMD), g_rgcmdsDbg);
	}
}
