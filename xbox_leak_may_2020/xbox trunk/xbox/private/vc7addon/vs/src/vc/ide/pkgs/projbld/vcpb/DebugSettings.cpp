// DebugSettings.cpp : Implementation of 

#include "stdafx.h"
#include "DebugSettings.h"
#include "msdbg.h"
#include "path2.h"
#include "register.h"
#include "regscan.h"
#include "vsappid.h"
#include "util2.h"

#define VCDSID_Version	0
GUID guidNull = { 0x00000000, 0x0000, 0x0000, { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 } };

// The following macro just makes it easy to check preconditions on functions.	It asserts
// the expression given, and returns with the supplied error code if the expression is false.
#define DEBUGSETTINGS_PRECONDITION(x, err, str) { if (str){VSASSERT((x), (str));} if (!(x)) { return (err); } }

BOOL		CDebugSettings::s_bTestContainerInit = FALSE;
CStringW	CDebugSettings::s_strTestContainer = L"";

int GetLengthDoubleNULLString(LPOLESTR pStr)
{
	LPOLESTR  pStart = pStr;
	LPOLESTR  pIter = pStr;

	INT_PTR n = wcslen(pIter);
	if( n==0 )
		return 2;

	while( n )
	{
		pIter = pIter + n + 1; // advance it past the NULL
		n = wcslen(pIter);
	}
	// pIter now points to the final NULL.
	return (int)(pIter - pStart); 
}


CDebugSettings::CDebugSettings()
{
	//
	// Initialize the defaults.
	//

	SetBoolProperty(VCDSID_Attach, VARIANT_FALSE);
	SetIntProperty(VCDSID_DebuggerType, DbgAuto);
	SetIntProperty(VCDSID_Remote, DbgLocal);

	m_bWorkingDirInit = FALSE;
}

/////////////////////////////////////////////////////////////////////////////
//


HRESULT CDebugSettings::CreateInstance(VCDebugSettings** ppDebugSettings, VCConfiguration* pConfig)
{
	CHECK_POINTER_NULL(ppDebugSettings);

	CDebugSettings *pVar;
	CComObject<CDebugSettings> *pObj;
	HRESULT hr = CComObject<CDebugSettings>::CreateInstance(&pObj);
	if (SUCCEEDED(hr))
	{
		pVar = pObj;
		pVar->AddRef();
		*ppDebugSettings = pVar;
		hr = pVar->Initialize(pConfig);
	}

	return hr;
}

HRESULT CDebugSettings::Initialize(VCConfiguration* pConfig)
{
	m_spProjCfg = pConfig;
	CComQIPtr<IVCPropertyContainer> spPropContainer = pConfig;
	if (spPropContainer)
		m_rgParents.AddHead(spPropContainer);
	return S_OK;
}

//
// Return a browser command w/ %s for the URL to open.	For example:
//
//	c:\W3C\Arena.exe -startpage "%s"
//
// NOTE: please update similar code in ..\ui_dll\src\exefordbg.h appropriately if you modify this function

/* static */ HRESULT CDebugSettings::GetBrowserCommand( CStringW* pstrCommand, CStringW* pstrArgs )
{
	CRegistryScanner regKey;
	CStringW strBrowser;
	CStringW strCommandLine;

	if (!regKey.Open (HKEY_CLASSES_ROOT, L"http\\shell\\open\\command"))
	{
		return E_FAIL;
	}

	if (!regKey.QueryStringValue (NULL, strBrowser))
	{
		return E_FAIL;
	}

	if (strBrowser.IsEmpty())
	{
		return E_FAIL;
	}

	{
		// From here there are two possible formats for the registry entry:
		// Netscape uses c:\progra~1\...\Netscape.exe -h "%1"
		// while IE uses "C:\Program Files\...\IEXPLORE.EXE" -nohome
		// The bstrExe that we return must be the executable alone without quotes, 
		// and we cannot pass args except through the bstrArg parameter.
		int iMarker = strBrowser.Find (L"%1");
		if (iMarker != -1)
		{
			// Netscape-style entry -- make a %s
			strCommandLine = strBrowser;
			((LPWSTR)(LPCWSTR)strCommandLine)[ iMarker + 1 ] = L's';
		}
		else
		{
			// IE-style entry
			strCommandLine = strBrowser + L" %s";
		}
	}

	//
	// Spit into EXE paths and args
	//

	int iSplit;
	if (strCommandLine[0] == L'\"')
	{
		// Quoted exe
		iSplit = strCommandLine.Find (L'\"', 1);
		VSASSERT (iSplit != -1, "Quote not found");
		VSASSERT (iSplit > 0, "Empty EXE Path");

		*pstrCommand = strCommandLine.Mid (1, iSplit - 1);
		if (pstrArgs)
		{
			*pstrArgs = strCommandLine.Mid (iSplit + 1);
		}
	}

	else
	{
		iSplit = strCommandLine.Find (L' ');
		VSASSERT (iSplit != -1, "Space not found");
		VSASSERT (iSplit > 0, "Empty EXE Path");

		*pstrCommand = strCommandLine.Left (iSplit);
		if (pstrArgs)
		{
			*pstrArgs = strCommandLine.Mid (iSplit);
		}
	}

	return S_OK;
}

// NOTE: please update similar code in ..\ui_dll\src\exefordbg.h appropriately if you modify this function
/* static */ HRESULT CDebugSettings::GetTestContainerCommand( CStringW& strCommand )
{
	GetTestContainerName(strCommand);
	return S_OK;
}	

// NOTE: please update similar code in ..\ui_dll\src\exefordbg.h appropriately if you modify this function
/* static */ BOOL CDebugSettings::GetTestContainerName(CStringW& strTestContainer)
{
	InitializeTestContainer();
	strTestContainer = s_strTestContainer;
	return !strTestContainer.IsEmpty();
}

// NOTE: please update similar code in ..\ui_dll\src\exefordbg.h appropriately if you modify this function
/* static */ void CDebugSettings::InitializeTestContainer()
{
	if (s_bTestContainerInit)
		return;

	s_bTestContainerInit = TRUE;
	//
	// Search order:
	//  1) Common7\Tools
	//	2) vc7 directory
	//	3) devenv.exe directory
	//	4) Use SearchPath()


	// Try Common7\Tools
	///////////////////////////////////
	UtilGetCommon7Folder(s_strTestContainer);
	s_strTestContainer += L"Tools\\tstcon32.exe";
	CPathW pathExe1;
	if (pathExe1.Create(s_strTestContainer) && pathExe1.ExistsOnDisk())
		return;

	// Try vc7\bin
	///////////////////////////////////

	UtilGetVCInstallFolder(s_strTestContainer);
	s_strTestContainer += L"bin\\tstcon32.exe";
	CPathW pathExe2;
	if (pathExe2.Create(s_strTestContainer) && pathExe2.ExistsOnDisk())
		return;

	//
	// Try devenv.exe directory
	//
	UtilGetDevenvFolder(s_strTestContainer);
	s_strTestContainer += L"tstcon32.exe";

	CPathW pathExe3;
	if (pathExe3.Create(s_strTestContainer) && pathExe3.ExistsOnDisk())
		return;

	//
	// Try the path
	//
	wchar_t *psz, szPath[_MAX_PATH + 1];
	DWORD dw = SearchPathW(NULL, L"tstcon32.exe", NULL, _MAX_PATH, szPath, &psz);
	if ( 0 < dw && _MAX_PATH > dw )
		s_strTestContainer = szPath;
	else
		s_strTestContainer.Empty();
}


// NOTE: please update similar code in ..\ui_dll\src\exefordbg.h appropriately if you modify this function
/* static */ HRESULT CDebugSettings::GetRegSvrCommand( CStringW &strCommand )
{
	wchar_t *pszFile = NULL;

	strCommand.ReleaseBuffer(SearchPathW(
		NULL,
		L"regsvr32.exe",
		NULL,
		_MAX_PATH + 1,
		strCommand.GetBuffer( _MAX_PATH + 1 ),
		&pszFile
		));


	return strCommand.IsEmpty() ? E_FAIL : S_OK;
}


HRESULT CDebugSettings::GetHttpDebugTargetInfo(VSDBGLAUNCHFLAGS grfLaunch, /*[in,out]*/ VsDebugTargetInfo *pDebugTargetInfo,
											   /*[in,out]*/ DWORD *pdwCount)
{
	// Test our preconditions...
	DEBUGSETTINGS_PRECONDITION(pDebugTargetInfo != NULL, E_INVALIDARG, 
							   "CDebugSettings::GetHttpDebugTargetInfo() - pDebugTargetInfo cannot be NULL.");
	DEBUGSETTINGS_PRECONDITION(pdwCount != NULL && (*pdwCount == 1 || *pdwCount == 2), E_INVALIDARG,
							   "CDebugSettings::GetHttpDebugTargetInfo() - CDebugSettings::GetHttpDebugTargetInfo() - pdwCount is invalid.");

	if (!(grfLaunch & DBGLAUNCH_NoDebug))
	{
		DEBUGSETTINGS_PRECONDITION(*pdwCount == 2, E_INVALIDARG, "CDebugSettings::GetHttpDebugTargetInfo() - If we're debug-launching a URL we need to fill in 2 VsDebugTargetInfo's.");
	}

	// If we got this far, we're ready to continue...
	HRESULT hr = S_OK;

	// Get the URL to launch the browser with
	CComBSTR sbstrHttpUrl;
	hr = get_HttpUrl(&sbstrHttpUrl);

	// Get the browser command and arguments.
	CStringW strBrowserCmd, strArgFmt;
	if (SUCCEEDED(hr))
	{
		hr = GetBrowserCommand(&strBrowserCmd, &strArgFmt);
	}

	if (SUCCEEDED(hr))
	{														
		// We'll have 2 targets if doing a debug launch, only 1 if this is a NoDebug launch.
		*pdwCount = (grfLaunch & DBGLAUNCH_NoDebug)? 1: 2;	

		// Create the browser launch target info.  For a 2 target launch, the second
		// target specifies the browser launch.
		VsDebugTargetInfo* pInfo = pDebugTargetInfo + (*pdwCount - 1);
		memset(pInfo, 0, sizeof(VsDebugTargetInfo));
		pInfo->cbSize = sizeof(VsDebugTargetInfo);

		// Format the actual arguments for the browser.
		CStringW strArgLine;
		strArgLine.Format(strArgFmt, sbstrHttpUrl);

		// Fill in the VsDebugTargetInfo struct for launching the browser
		pInfo->cbSize = sizeof(VsDebugTargetInfo);
		pInfo->bstrExe = strBrowserCmd.AllocSysString();
		pInfo->bstrArg = strArgLine.AllocSysString();
		pInfo->dlo = DLO_CreateProcess;
		pInfo->grfLaunch = grfLaunch | DBGLAUNCH_StopDebuggingOnEnd;
		pInfo->pClsidList = (CLSID*)CoTaskMemAlloc( sizeof(CLSID) );
		if (pInfo->pClsidList==NULL)
			return E_OUTOFMEMORY;
		pInfo->pClsidList[0] = guidScriptEng;
		pInfo->dwClsidCount = 1;

		hr = (pInfo->bstrExe == NULL || pInfo->bstrArg == NULL)? E_OUTOFMEMORY: S_OK;

		// If we need to fill in two launch targets, then fill in the
		// first target, which happens to be the LaunchByWebServer target.
		if (SUCCEEDED(hr) && *pdwCount == 2)
		{
			memset(pDebugTargetInfo, 0, sizeof(VsDebugTargetInfo));
			pDebugTargetInfo->cbSize = sizeof(VsDebugTargetInfo);

			// TODO: get the remote machine name from somewhere...but where?
			// Should we split the URL to get the server name out of it?
			pDebugTargetInfo->bstrRemoteMachine = NULL;
			pDebugTargetInfo->dlo = DLO_LaunchByWebServer;
			pDebugTargetInfo->grfLaunch = grfLaunch | DBGLAUNCH_WaitForAttachComplete | DBGLAUNCH_DetachOnStop;
			pDebugTargetInfo->bstrExe = SysAllocString(sbstrHttpUrl);

			TypeOfDebugger debuggerType = DbgAuto;
			GetIntProperty(VCDSID_DebuggerType, (long *)&debuggerType);
			CLSID clsid;
			GetEngineForDebuggerType(debuggerType, NULL, &clsid);

			VARIANT_BOOL bSQL = VARIANT_FALSE;
			get_SQLDebugging(&bSQL);
			if( bSQL == VARIANT_TRUE )
			{
				pDebugTargetInfo->pClsidList = (CLSID*)CoTaskMemAlloc( 2*sizeof(CLSID) );
				pDebugTargetInfo->pClsidList[1] = guidSQLEng;
				pDebugTargetInfo->dwClsidCount = 2;
			}
			else
			{
				pDebugTargetInfo->pClsidList = (CLSID*)CoTaskMemAlloc( sizeof(CLSID) );
				pDebugTargetInfo->dwClsidCount = 1;
			}
			pDebugTargetInfo->pClsidList[0] = clsid;
			hr = (pDebugTargetInfo->bstrExe == NULL)? E_OUTOFMEMORY: S_OK;
		}

	}
	return hr;
}

STDMETHODIMP CDebugSettings::CanGetDebugTargetInfo(VARIANT_BOOL* pbCanGet)
{
	if (pbCanGet != NULL)
		*pbCanGet = VARIANT_TRUE;

	CComBSTR bstrHttpUrl;
	GetEvaluatedStrProperty(VCDSID_HttpUrl, &bstrHttpUrl);
	if (bstrHttpUrl.Length() > 0)
		return S_OK;

	CComBSTR bstrDebuggee;
	DoGetCommand( bstrDebuggee, TRUE );
	if (bstrDebuggee.Length() > 0)
		return S_OK;

	if (pbCanGet != NULL)
		*pbCanGet = VARIANT_FALSE;
	return E_FAIL;
}

void CDebugSettings::DoEvaluate(IVCPropertyContainer* pPropContainer, CComBSTR& bstrProp)
{
	CComBSTR bstrPropIn = bstrProp.Detach();
	if (pPropContainer)
		pPropContainer->Evaluate(bstrPropIn, &bstrProp);
	else
		Evaluate(bstrPropIn, &bstrProp);
}

void CDebugSettings::DoGetEvaluatedStrProperty(IVCPropertyContainer* pPropContainer, long idProp, BSTR* pbstrProp)
{
	if (pPropContainer == NULL)	// exe project
	{
		GetEvaluatedStrProperty(idProp, pbstrProp);
		return;
	}

	HRESULT hr = pPropContainer->GetEvaluatedStrProperty(idProp, pbstrProp);

	if (hr == S_OK)	// we're done.  get outta here
		return;

	// hmmm.  Property must be in the debugger's property container, then
	CComBSTR bstrTmp;
	GetStrProperty(idProp, &bstrTmp);
	pPropContainer->Evaluate(bstrTmp, pbstrProp);
}

STDMETHODIMP CDebugSettings::GetDebugTargetInfo(/*[in]*/ VSDBGLAUNCHFLAGS grfLaunch, 
	/*[in,out]*/ VsDebugTargetInfo *pDebugTargetInfo, /*[in, out]*/ DWORD *pdwCount )
{
	CComBSTR bstrArgs;
	CComBSTR bstrDebuggee;
	CComBSTR bstrDir;
	CComBSTR bstrMachine;
	CComBSTR bstrOptionString;
	CComPtr<IDebugPort2> pPort;
	bool bDoSpawn = false;
	bool bRemote = false;
	USES_CONVERSION;
	
	CComQIPtr<IVCPropertyContainer> spPropContainer = m_spProjCfg;

	// Now, build up the DebugTargetInfo structure (defined in vsshell.idl)
	memset(pDebugTargetInfo, 0, sizeof(VsDebugTargetInfo));
	pDebugTargetInfo->cbSize = sizeof(VsDebugTargetInfo);

	CComBSTR bstrHttpUrl;
	GetStrProperty(VCDSID_HttpUrl, &bstrHttpUrl);
	DoEvaluate(spPropContainer, bstrHttpUrl);
	if (bstrHttpUrl.Length() > 0)
		return GetHttpDebugTargetInfo(grfLaunch, pDebugTargetInfo, pdwCount);

	DoGetCommand( bstrDebuggee, TRUE );
	if (bstrDebuggee.Length() == 0)
		return E_FAIL;


	// Only launch the one process by default
	*pdwCount = 1;

	// Assume local first so get usual things
	pDebugTargetInfo->bstrMdmRegisteredName = NULL;
	pDebugTargetInfo->dlo = DLO_Custom;

	// figure out the working dir
	CComBSTR bstrProjDir;
	if (spPropContainer != NULL)
		spPropContainer->GetStrProperty(VCPROJID_ProjectDirectory, &bstrProjDir );

	RemoteDebuggerType remoteType = DbgLocal;
	GetIntProperty(VCDSID_Remote, (long *)&remoteType);

	VARIANT_BOOL bAttach = VARIANT_FALSE;
	get_Attach(&bAttach);

	get_WorkingDirectory( &bstrDir );
	if( bstrDir.Length() == 0 )
	{
		if (remoteType == DbgLocal)		// VS7Bug: 156499 Only set the directory if we're debugging locally.
			bstrDir = bstrProjDir;
	}
	else	// need to see if it is a relative path and handle accordingly
	{
		DoEvaluate(spPropContainer, bstrDir);
		CDirW dirProj;
		if (dirProj.CreateFromKnown(bstrProjDir))
		{
			CPathW pathWorking;
			if (pathWorking.CreateFromDirAndFilename(dirProj, bstrDir))
			{
				CStringW strPathWorking;
				pathWorking.GetFullPath(strPathWorking);
				bstrDir = strPathWorking;
			}
		}
	}
	if (pDebugTargetInfo->bstrCurDir)
		::SysFreeString(pDebugTargetInfo->bstrCurDir);
	if (bstrDir.Length() > 0)
		bstrDir.CopyTo(&(pDebugTargetInfo->bstrCurDir));

	// figure out the fullpath to the local exe to debug
	// if( (remoteType == DbgLocal) && (bAttach == VARIANT_FALSE) )
	if( remoteType == DbgLocal )
	{
		CDirW dir;
		CPathW path;
		dir.CreateFromStringEx( bstrProjDir, FALSE );
		path.CreateFromDirAndFilename(dir, bstrDebuggee);
		if( path.ExistsOnDisk() )
		{
			// its in a location relative to the project.
			bstrDebuggee = path;
		}
		else
		{
			dir.CreateFromStringEx( bstrDir, FALSE );
			path.CreateFromDirAndFilename(dir, bstrDebuggee);
			if( path.ExistsOnDisk() )
			{
				// its in a location relative to the working dir.
				bstrDebuggee = path;
			}
			else
			{
				// We are running out of options here. Maybe its on the path ?
				wchar_t szPathToDebug[ _MAX_PATH+10 ];
				LPOLESTR pJunk = NULL;

				CComBSTR bstrPath;
				HRESULT hr = VCGetEnvironmentVariableW(L"PATH", &bstrPath);
				BOOL b = SearchPathW( bstrPath, bstrDebuggee, NULL, _MAX_PATH+10, szPathToDebug, &pJunk);
				if( b )
				{
					bstrDebuggee = szPathToDebug;
				}
				else
				{
					// still couldn't find it !! 
					// We are probably going to error out eventually then.
				}
			}
		}
	}


	// now get the rest of the stuff
	get_CommandArguments( &bstrArgs );
	DoEvaluate(spPropContainer, bstrArgs);
	pDebugTargetInfo->bstrArg = bstrArgs.Detach();
	pDebugTargetInfo->grfLaunch = grfLaunch;

	IMAGE_INFO ImageInfo;
	ImageInfo.Size = sizeof(ImageInfo);
	BOOL bGotImageInfo = FALSE;
	bGotImageInfo = bstrDebuggee && GetImageInfo( W2A(bstrDebuggee), &ImageInfo );

	// select Engine based on debugger type
	TypeOfDebugger debuggerType = DbgAuto;
	GetIntProperty(VCDSID_DebuggerType, (long *)&debuggerType);

	CLSID clsid;
	GetEngineForDebuggerType(debuggerType, &ImageInfo, &clsid);

	// select remote & options as required
	switch (remoteType)
	{
	case DbgLocal:
		{
			pDebugTargetInfo->bstrRemoteMachine = NULL;
			bDoSpawn = (grfLaunch & DBGLAUNCH_NoDebug) ? true : false;				// Use VCSpawn if local and not debugging
			if( bDoSpawn )
			{
				LPOLESTR pStr = GetEnvironmentStringsW();
				int len = GetLengthDoubleNULLString(pStr);
				pDebugTargetInfo->bstrEnv = SysAllocStringLen( pStr, len );
				FreeEnvironmentStringsW(pStr);
			}
		}
		break;
	case DbgRemote:
		{
			pDebugTargetInfo->clsidPortSupplier = guidLocalPortSupplier;
			DoGetEvaluatedStrProperty(spPropContainer, VCDSID_RemoteMachine, &pDebugTargetInfo->bstrRemoteMachine);
			DoGetEvaluatedStrProperty(spPropContainer, VCDSID_RemoteCommand, &bstrDebuggee);
			bRemote = true;
		}
		break;
	case DbgRemoteTCPIP:
		{
			// TCPIP only works for native-only
			if (clsid!=guidNativeOnlyEng)
				return E_INVALIDARG;

			// need to build a native port, on the local machine, whose name is the remote machine
			pDebugTargetInfo->bstrRemoteMachine = NULL;
			pDebugTargetInfo->clsidPortSupplier = guidNativePortSupplier;
			DoGetEvaluatedStrProperty(spPropContainer, VCDSID_RemoteMachine, &pDebugTargetInfo->bstrPortName);
			DoGetEvaluatedStrProperty(spPropContainer, VCDSID_RemoteCommand, &bstrDebuggee);
			bRemote = true;
		}
		break;
	default:
		return E_POINTER;
	}

	bstrDebuggee.CopyTo(&pDebugTargetInfo->bstrExe);
	pDebugTargetInfo->clsidCustom = clsid;

	if (bAttach != VARIANT_FALSE )
	{
		pDebugTargetInfo->dlo = DLO_AlreadyRunning;
	}

	if (bDoSpawn &&
		bGotImageInfo &&
		(ImageInfo.ImageType==IMAGE_TYPE_WIN32) &&
		(ImageInfo.u.Win32.Subsystem==IMAGE_SUBSYSTEM_WINDOWS_CUI)
	   )
	{
		// First, if it's a console app we run the app with 'vcspawn -p -t' so
		// the user gets a 'Press any key to continue' when it's done.
		//
		// BUG BUG We should not be doing this when user is redirecting
		// stdout
		//

		CComPtr< IDispatch > spDispPlatform;
		CComBSTR bstrWPath;
		CStringW strPath;
		if (m_spProjCfg && m_spProjCfg->get_Platform(&spDispPlatform)==S_OK)
		{
			CComQIPtr<VCPlatform> spPlatform = spDispPlatform;
			if (spPlatform)
			{
				spPlatform->get_ExecutableDirectories(&bstrWPath);
				if (bstrWPath)
				{
					CComBSTR bstrTmp = bstrWPath.Detach();
					spPlatform->Evaluate(bstrTmp, &bstrWPath);
					strPath = bstrWPath;
				}
			}
		}

		wchar_t szPathToSpawn[ _MAX_PATH+10 ];
		LPOLESTR pJunk = NULL;

		// Find the exe on the path if no setting set
		BOOL bFoundVCSpawn = (strPath.GetLength() && SearchPathW( strPath, L"vcspawn.exe", NULL, _MAX_PATH+10, szPathToSpawn, 
			&pJunk));
		if (!bFoundVCSpawn)
			bFoundVCSpawn = (SearchPathW( NULL, L"vcspawn.exe", NULL, _MAX_PATH+10, szPathToSpawn, &pJunk) > 0);
		if (!bFoundVCSpawn)
		{
			CStringW strMsvcPath;
			UtilGetCommon7Folder(strMsvcPath);
			strMsvcPath += L"Tools\\vcspawn.exe";
			CPathW pathSpawn;
			if (pathSpawn.Create(strMsvcPath) && pathSpawn.ExistsOnDisk())
			{
				bFoundVCSpawn = TRUE;
				wcscpy(szPathToSpawn, strMsvcPath);
			}
		}

		if (bFoundVCSpawn)
		{
			CComBSTR strNewExe;
			CComBSTR strNewArgs;

			// Build a new szArgs that looks like this:
			// "vcspawn -t -p d:\Path\MyDebuggee arg1 arg2..."
			strNewArgs = L"-t -p \"";
			strNewArgs += pDebugTargetInfo->bstrExe;
			strNewArgs += "\" ";
			strNewArgs += pDebugTargetInfo->bstrArg;

			SysFreeString(pDebugTargetInfo->bstrExe);
			strNewExe = szPathToSpawn;
			pDebugTargetInfo->bstrExe = strNewExe.Detach();

			SysFreeString(pDebugTargetInfo->bstrArg);
			pDebugTargetInfo->bstrArg = strNewArgs.Detach();
		}
	}

	if (bRemote)
	{
		// need to tell Engine where local path is
		CComBSTR bstrLocalPath;
		get_Command(&bstrLocalPath );
		DoEvaluate(spPropContainer, bstrLocalPath);

		INT_PTR nLen = bstrLocalPath.Length();
		bstrOptionString.Append(L";LOCALPATH=\"");
		if (nLen)
			bstrOptionString.Append(bstrLocalPath);
		bstrOptionString.Append(L"\"");

		// need to figure out the pdb search path which is directory of local exe
		// if it has a directory path, plus pdbpath if specified
		CComBSTR bstrPDBPath;
		get_PDBPath(&bstrPDBPath);
		DoEvaluate(spPropContainer, bstrPDBPath);

		const wchar_t *pStr = nLen ? wcsrchr(bstrLocalPath,L'\\') : NULL;
		long iSlash = -1;
		if( pStr )
		{
			iSlash = (int)(pStr - bstrLocalPath);
		}


		if (iSlash>=0 || bstrPDBPath.Length()>0 )
		{
			// we have a search path
			bstrOptionString += L";PDBSEARCHPATH=\"";

			if(iSlash>=0)
			{
				// append directory of local exe
				CStringW strPath = bstrLocalPath;
				strPath = strPath.Left(iSlash);
				bstrOptionString += strPath;
			}
			
			if (bstrPDBPath.Length() > 0)
			{
				// append pdb path
				if (iSlash>=0)
				{
					bstrOptionString.Append(";");
				}
				bstrOptionString.Append(bstrPDBPath);
			}
			// complete quote
			bstrOptionString +=	 L"\"";
		}
	} else {
		CComBSTR bstrPDBPath;
		get_PDBPath(&bstrPDBPath);
		DoEvaluate(spPropContainer, bstrPDBPath);
		if (bstrPDBPath.Length() > 0)
		{
			bstrOptionString += L";PDBSEARCHPATH=\"";
			bstrOptionString += bstrPDBPath;
			bstrOptionString +=	 L"\"";
		}
	}

	VARIANT_BOOL bSQL = VARIANT_FALSE;
	get_SQLDebugging(&bSQL);
	if( bSQL == VARIANT_TRUE )
	{
		pDebugTargetInfo->pClsidList = (CLSID*)CoTaskMemAlloc( 2*sizeof(CLSID) );
		RETURN_ON_NULL2(pDebugTargetInfo->pClsidList, E_OUTOFMEMORY);
		pDebugTargetInfo->pClsidList[1] = guidSQLEng;
		pDebugTargetInfo->dwClsidCount = 2;
	}
	else
	{
		pDebugTargetInfo->pClsidList = (CLSID*)CoTaskMemAlloc( sizeof(CLSID) );
		RETURN_ON_NULL2(pDebugTargetInfo->pClsidList, E_OUTOFMEMORY);
		pDebugTargetInfo->dwClsidCount = 1;
	}
	
	if (clsid == guidNativeOnlyEng )
		pDebugTargetInfo->pClsidList[0] = guidNativeOnlyEng;
	else if (clsid == guidCOMPlusNativeEng)
		pDebugTargetInfo->pClsidList[0] = guidCOMPlusNativeEng;
	else
		pDebugTargetInfo->pClsidList[0] = guidNull;

	if (bstrOptionString!=NULL)
	{
		pDebugTargetInfo->bstrOptions = bstrOptionString.Detach();
	}

	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
// CDebugSettings

STDMETHODIMP CDebugSettings::get_Command(BSTR *pVal)
{
	CHECK_POINTER_VALID(pVal);
	CComBSTR bstrCommand;
	DoGetCommand(bstrCommand, FALSE);
	*pVal = bstrCommand.Detach();
	return S_OK;
}

HRESULT CDebugSettings::DoGetCommand(CComBSTR& bstrCommand, BOOL bEvaluate)
{
	DoGetStrProperty(VCDSID_Command, FALSE, &bstrCommand);
	
	long remoteType = DbgLocal;
	GetIntProperty(VCDSID_Remote, (long *)&remoteType);

	// If we are going to remote debug, then the command is 
	// the remote command
	if (bstrCommand.Length() == 0 && remoteType != DbgLocal)
	{
		CComBSTR bstrRemoteCommand;
		GetStrProperty(VCDSID_RemoteCommand, &bstrRemoteCommand);
		bstrCommand = bstrRemoteCommand;
	}
	else if (bstrCommand.Length() == 0 && m_spProjCfg != NULL)
	{
		ConfigurationTypes cfgType;
		m_spProjCfg->get_ConfigurationType(&cfgType);
		if( cfgType == typeApplication || cfgType == typeUnknown )
			bstrCommand = L"$(TargetPath)";
	}

	if (bEvaluate)
	{
		CComQIPtr<IVCPropertyContainer> spPropContainer = m_spProjCfg;
		if (spPropContainer)
			return spPropContainer->Evaluate(bstrCommand, &bstrCommand);
		else
			return Evaluate(bstrCommand, &bstrCommand);
	}

	return S_OK;
}

STDMETHODIMP CDebugSettings::put_Command(BSTR newVal)
{
	SetStrProperty(VCDSID_Command, newVal);
	if (m_spProjCfg == NULL)	// need to kludge saving off the 'project' file info...
	{
		CComBSTR bstrJunk;
		if (GetStrProperty(VCDSID_BaseCommand, &bstrJunk) == S_FALSE)	// not yet set
			SetStrProperty(VCDSID_BaseCommand, newVal);
	}
	return S_OK;
}

STDMETHODIMP CDebugSettings::get_WorkingDirectory(BSTR *pVal)
{
	CHECK_POINTER_VALID( pVal );
	GetStrProperty(VCDSID_WorkingDirectory, pVal);
	return S_OK;
}

STDMETHODIMP CDebugSettings::put_WorkingDirectory(BSTR newVal)
{
	SetStrProperty(VCDSID_WorkingDirectory, newVal);
	m_bWorkingDirInit = TRUE;
	return S_OK;
}

STDMETHODIMP CDebugSettings::get_CommandArguments(BSTR *pVal)
{
	CHECK_POINTER_VALID( pVal );
	GetStrProperty(VCDSID_CommandArguments, pVal);
	return S_OK;
}

STDMETHODIMP CDebugSettings::put_CommandArguments(BSTR newVal)
{
	SetStrProperty(VCDSID_CommandArguments, newVal);
	return S_OK;
}

STDMETHODIMP CDebugSettings::get_Attach(VARIANT_BOOL *pVal)
{
	CHECK_POINTER_VALID(pVal);
	GetBoolProperty(VCDSID_Attach, pVal);
	return S_OK;
}

STDMETHODIMP CDebugSettings::put_Attach(VARIANT_BOOL newVal)
{
	SetBoolProperty(VCDSID_Attach, newVal);
	return S_OK;
}

STDMETHODIMP CDebugSettings::get_SQLDebugging(VARIANT_BOOL *pVal)
{
	CHECK_POINTER_VALID(pVal);
	GetBoolProperty(VCDSID_SQLDebugging, pVal);
	return S_OK;
}

STDMETHODIMP CDebugSettings::put_SQLDebugging(VARIANT_BOOL newVal)
{
	SetBoolProperty(VCDSID_SQLDebugging, newVal);
	return S_OK;
}

STDMETHODIMP CDebugSettings::get_Remote(RemoteDebuggerType *pVal)
{
	CHECK_POINTER_VALID( pVal );
	GetIntProperty(VCDSID_Remote, (long *)pVal);
	return S_OK;
}

STDMETHODIMP CDebugSettings::put_Remote(RemoteDebuggerType newVal)
{
	SetIntProperty(VCDSID_Remote, newVal);
	return S_OK;
}

STDMETHODIMP CDebugSettings::get_RemoteMachine(BSTR *pVal)
{
	CHECK_POINTER_VALID( pVal );
	GetStrProperty(VCDSID_RemoteMachine, pVal);
	return S_OK;
}

STDMETHODIMP CDebugSettings::put_RemoteMachine(BSTR newVal)
{
	SetStrProperty(VCDSID_RemoteMachine, newVal);
	return S_OK;
}

STDMETHODIMP CDebugSettings::get_RemoteCommand(BSTR *pVal)
{
	CHECK_POINTER_VALID( pVal );
	GetStrProperty(VCDSID_RemoteCommand, pVal);
	return S_OK;
}

STDMETHODIMP CDebugSettings::put_RemoteCommand(BSTR newVal)
{
	SetStrProperty(VCDSID_RemoteCommand, newVal);
	return S_OK;
}

STDMETHODIMP CDebugSettings::get_HttpUrl(BSTR *pVal)
{
	CHECK_POINTER_VALID( pVal );
	GetStrProperty(VCDSID_HttpUrl, pVal);
	return S_OK;
}

STDMETHODIMP CDebugSettings::put_HttpUrl(BSTR newVal)
{
	SetStrProperty(VCDSID_HttpUrl, newVal);
	return S_OK;
}

STDMETHODIMP CDebugSettings::get_PDBPath(BSTR *pbstrVal)
{
	CHECK_POINTER_VALID( pbstrVal );
	GetStrProperty(VCDSID_PDBPath, pbstrVal);
	return S_OK;
}

STDMETHODIMP CDebugSettings::put_PDBPath(BSTR bstrNewVal)
{
	SetStrProperty(VCDSID_PDBPath, bstrNewVal);
	return S_OK;
}

STDMETHODIMP CDebugSettings::get_DebuggerType(TypeOfDebugger *pVal)
{
	CHECK_POINTER_VALID( pVal );
	GetIntProperty(VCDSID_DebuggerType, (long *)pVal);
	return S_OK;
}

STDMETHODIMP CDebugSettings::put_DebuggerType(TypeOfDebugger newVal)
{
	SetIntProperty(VCDSID_DebuggerType, newVal);
	return S_OK;
}

STDMETHODIMP CDebugSettings::Close()
{
	m_spProjCfg.Release();
	FinishClose();
	return S_OK;
}

STDMETHODIMP CDebugSettings::WriteToStream(IStream *pStream)
{
	HRESULT		hr;
	// Write out version field first
	long lVersion = 0x00010000;
	hr = WritePropertyToStream(pStream, lVersion, VCDSID_Version );
	RETURN_ON_FAIL(hr);

	//Write out all properties
	CComBSTR bstrCommand;
	GetStrProperty(VCDSID_Command, &bstrCommand);
	hr = WritePropertyToStream(pStream, bstrCommand, VCDSID_Command);
	RETURN_ON_FAIL(hr);

	CComBSTR bstrWorkingDir;
	GetStrProperty(VCDSID_WorkingDirectory, &bstrWorkingDir);
	hr = WritePropertyToStream(pStream, bstrWorkingDir, VCDSID_WorkingDirectory); 
	RETURN_ON_FAIL(hr);

	CComBSTR bstrCommandArgs;
	GetStrProperty(VCDSID_CommandArguments, &bstrCommandArgs);
	hr = WritePropertyToStream(pStream, bstrCommandArgs, VCDSID_CommandArguments); 
	RETURN_ON_FAIL(hr);

	VARIANT_BOOL bAttach = VARIANT_FALSE;
	GetBoolProperty(VCDSID_Attach, &bAttach);
	hr = WritePropertyToStream(pStream, bAttach, VCDSID_Attach); 
	RETURN_ON_FAIL(hr);

	long debuggerType = DbgAuto;
	GetIntProperty(VCDSID_DebuggerType, &debuggerType);
	hr = WritePropertyToStream(pStream, debuggerType, VCDSID_DebuggerType); 
	RETURN_ON_FAIL(hr);

	long remoteType = DbgLocal;
	GetIntProperty(VCDSID_Remote, (long *)&remoteType);
	hr = WritePropertyToStream(pStream, remoteType, VCDSID_Remote);
	RETURN_ON_FAIL(hr);

	CComBSTR bstrRemoteCommand;
	GetStrProperty(VCDSID_RemoteCommand, &bstrRemoteCommand);
	hr = WritePropertyToStream(pStream, bstrRemoteCommand, VCDSID_RemoteCommand);
	RETURN_ON_FAIL(hr);

	CComBSTR bstrRemoteMachine;
	GetStrProperty(VCDSID_RemoteMachine, &bstrRemoteMachine);
	hr = WritePropertyToStream(pStream, bstrRemoteMachine, VCDSID_RemoteMachine);
	RETURN_ON_FAIL(hr);

	CComBSTR bstrHttpUrl;
	GetStrProperty(VCDSID_HttpUrl, &bstrHttpUrl);
	hr = WritePropertyToStream(pStream, bstrHttpUrl, VCDSID_HttpUrl);
	RETURN_ON_FAIL(hr);

	CComBSTR bstrPDBPath;
	GetStrProperty(VCDSID_PDBPath, &bstrPDBPath);
	hr = WritePropertyToStream(pStream, bstrPDBPath, VCDSID_PDBPath);
	RETURN_ON_FAIL(hr);
	
	VARIANT_BOOL bSQL = VARIANT_FALSE;
	GetBoolProperty(VCDSID_SQLDebugging, &bSQL);
	hr = WritePropertyToStream(pStream, bSQL, VCDSID_SQLDebugging);
	RETURN_ON_FAIL(hr);

	//Write "end of property list" marker
	VARTYPE type = VT_EMPTY;
	ULONG	iByteCount;
	hr = pStream->Write(&type, sizeof(VARTYPE), &iByteCount);
	RETURN_ON_FAIL(hr);

	return S_OK;
}

STDMETHODIMP CDebugSettings::ReadFromStream(IStream *pStream)
{
	VARTYPE		type;
	ULONG		iByteCount;
	DISPID		dispid;
	HRESULT		hr;
	long		lVersion;

	while (true)
	{
		//Read in type
		hr = pStream->Read(&type, sizeof(VARTYPE), &iByteCount);
		RETURN_ON_FAIL(hr);

		//End of list marker
		if (type == VT_EMPTY) break;

		//Read in dispid
		hr = pStream->Read(&dispid, sizeof(DISPID), &iByteCount);
		RETURN_ON_FAIL(hr);

		switch (type)	//Switch the property types
		{
			case VT_BOOL:
				switch (dispid)
				{
					case VCDSID_Attach:
					{
						VARIANT_BOOL bAttach = VARIANT_FALSE;
						hr = pStream->Read(&bAttach, sizeof(VARIANT_BOOL), &iByteCount);
						SetBoolProperty(VCDSID_Attach, bAttach);
					}
						break;
					case VCDSID_SQLDebugging:
					{
						VARIANT_BOOL bSQL = VARIANT_FALSE;
						hr = pStream->Read(&bSQL, sizeof(VARIANT_BOOL), &iByteCount);
						SetBoolProperty(VCDSID_SQLDebugging, bSQL);
					}
						break;

					default:
						return E_UNEXPECTED;
				}
				RETURN_ON_FAIL(hr);
				break;
	
			case VT_I4:
				switch (dispid)
				{
					case VCDSID_Version:
						hr = pStream->Read(&lVersion, sizeof(DWORD), &iByteCount);
						if (lVersion!=0x00010000)
						{
							OutputDebugStringW(L"VCDebugSettings:wrong version, settings discarded\n");
							return S_OK;						// if version not understood, abort rest of props entirely
						}
						break;

					case VCDSID_DebuggerType:
						{
							TypeOfDebugger debuggerType = DbgAuto;
							hr = pStream->Read(&debuggerType, sizeof(debuggerType), &iByteCount );
							SetIntProperty(VCDSID_DebuggerType, debuggerType);
						}
						break;

					case VCDSID_Remote:
						{
							RemoteDebuggerType remoteType = DbgLocal;
							hr = pStream->Read(&remoteType, sizeof(remoteType), &iByteCount );
							SetIntProperty(VCDSID_Remote, remoteType);
						}
						break;

					default:
						return E_UNEXPECTED;
				}
				RETURN_ON_FAIL(hr);
				break;
	
			case VT_BSTR:
				switch (dispid)
				{
					case VCDSID_Command:
						{
							CComBSTR bstrCommand;
							hr = bstrCommand.ReadFromStream(pStream);
							SetStrProperty(VCDSID_Command, bstrCommand);
						}
						break;

					case VCDSID_WorkingDirectory:
						{
							CComBSTR bstrWorkingDir;
							hr = bstrWorkingDir.ReadFromStream(pStream);
							SetStrProperty(VCDSID_WorkingDirectory, bstrWorkingDir);
						}
						break;

					case VCDSID_CommandArguments:
						{
							CComBSTR bstrCommandArgs;
							hr = bstrCommandArgs.ReadFromStream(pStream);
							SetStrProperty(VCDSID_CommandArguments, bstrCommandArgs);
						}
						break;

					case VCDSID_RemoteCommand:
						{
							CComBSTR bstrRemoteCommand;
							hr = bstrRemoteCommand.ReadFromStream(pStream);
							SetStrProperty(VCDSID_RemoteCommand, bstrRemoteCommand);
						}
						break;

					case VCDSID_RemoteMachine:
						{
							CComBSTR bstrRemoteMachine;
							hr = bstrRemoteMachine.ReadFromStream(pStream);
							SetStrProperty(VCDSID_RemoteMachine, bstrRemoteMachine);
						}
						break;

					case VCDSID_HttpUrl:
						{
							CComBSTR bstrHttpUrl;
							hr = bstrHttpUrl.ReadFromStream(pStream);
							SetStrProperty(VCDSID_HttpUrl, bstrHttpUrl);
						}
						break;

					case VCDSID_PDBPath:
						{
							CComBSTR bstrPDBPath;
							hr = bstrPDBPath.ReadFromStream(pStream);
							SetStrProperty(VCDSID_PDBPath, bstrPDBPath);
						}
						break;
					
					default:
						return E_UNEXPECTED;
				}
				RETURN_ON_FAIL(hr);
				break;

			default:
				return E_UNEXPECTED;
		}
	}
	return S_OK;
}

HRESULT CDebugSettings::DoGetProp(long id, BOOL bCheckSpecial, VARIANT* pVarValue)
{
	if (bCheckSpecial && id == VCDSID_Command)	// we can figure this one out if not there already
	{
		CHECK_POINTER_NULL(pVarValue);
		CComBSTR bstrCommand;
		DoGetCommand(bstrCommand, FALSE);

		CComVariant varVal;
		varVal = bstrCommand;
		varVal.Detach(pVarValue);
		return S_OK;
	}

	return CPropertyContainerImpl::DoGetProp(id, bCheckSpecial, pVarValue);
}

STDMETHODIMP CDebugSettings::GetMultiProp(long id, LPCOLESTR szSeparator, VARIANT_BOOL bSkipLocal, BSTR *pbstrValue)
{
	CHECK_POINTER_NULL(pbstrValue);

	// all properties are single props to the debugger
	return GetStrProperty(id, pbstrValue);
}

HRESULT CDebugSettings::DoGetStrProperty(long idProp, BOOL bCheckSpecial, BSTR* pbstrValue)
{
	if (idProp == VCDSID_Command && bCheckSpecial)	// we can figure this one out if not there already
	{
		CHECK_POINTER_NULL(pbstrValue);
		CComBSTR bstrCmd;
		DoGetCommand(bstrCmd, FALSE);
		*pbstrValue = bstrCmd.Detach();
		return S_OK;
	}

	return CPropertyContainerImpl::DoGetStrProperty(idProp, bCheckSpecial, pbstrValue);
}

STDMETHODIMP CDebugSettings::GetStagedPropertyContainer(VARIANT_BOOL bCreateIfNecessary, IVCStagedPropertyContainer** ppPropContainer)
{
	if (m_spProjCfg)
		return CPropertyContainerImpl::GetStagedPropertyContainer(bCreateIfNecessary, ppPropContainer);

	CHECK_POINTER_NULL(ppPropContainer);

	if (!m_spStagedContainer)
	{
		if (bCreateIfNecessary)
		{
			HRESULT hr = CVCStagedExeConfigPropertyContainer::CreateInstance(this, &m_spStagedContainer);
			RETURN_ON_FAIL(hr);
		}
		else
		{
			*ppPropContainer = NULL;
			return S_OK;
		}
	}

	return m_spStagedContainer.CopyTo(ppPropContainer);
}

// get the value of a macro
BOOL CDebugSettingsBase::GetMacroValue(UINT idMacro, CStringW& strMacroValue, IVCPropertyContainer* pPropContainer)
{
	if (CallBaseGetMacroValue(pPropContainer))
		return CProjectEvaluator::GetMacroValue(idMacro, strMacroValue, pPropContainer);

	CStringW strTemp;
	CPathW path;

	// empty macro-value
	strMacroValue.Empty();

	HRESULT hr = S_OK;

	switch (idMacro)
	{
		// output directory
		case IDMACRO_OUTDIR:
			strMacroValue = L'.';
			break;

		case IDMACRO_ENVDIR:
		case IDMACRO_SOLNDIR:
		case IDMACRO_SOLNPATH:
		case IDMACRO_SOLNBASE:
		case IDMACRO_SOLNFILE:
		case IDMACRO_SOLNEXT:
		case IDMACRO_INHERIT:
		case IDMACRO_NOINHERIT:
			return CProjectEvaluator::GetMacroValue(idMacro, strMacroValue, pPropContainer);
			break;

		case IDMACRO_PROJDIR:	// project directory
		case IDMACRO_TARGDIR:	// target directory
		case IDMACRO_INPTDIR:	// input can only be the project/target
		{
			if (GetExeProjectTargetPath(path, strMacroValue))
			{
				CDirW dir;
				if (dir.CreateFromPath(path))
					strMacroValue = (const wchar_t*)dir;
			}
			break;
		}

		case IDMACRO_PROJPATH:	// full path of project
		case IDMACRO_TARGPATH:	// full path of target
		case IDMACRO_INPTPATH:	// input can only be the project/target
		{
			CComBSTR bstrProjFile;
			if (SUCCEEDED(GetEvaluatedStrProperty(VCDSID_BaseCommand, &bstrProjFile)))
				strMacroValue = bstrProjFile;
			break;
		}

		case IDMACRO_PROJBASE:	// project file basename
		case IDMACRO_TARGBASE:	// target file basename
		case IDMACRO_INPTBASE:	// input can only be the project/target
		{
			if (GetExeProjectTargetPath(path, strMacroValue))
			{
				path.GetBaseNameString(strTemp);
				if (!strTemp.IsEmpty())
					strMacroValue = strTemp;
			}
			break;
		}

		case IDMACRO_PROJFILE:	// file name of project
		case IDMACRO_TARGFILE:	// file name of target
		case IDMACRO_INPTFILE:	// input can only be the project/target
		{
			if (GetExeProjectTargetPath(path, strMacroValue))
			{
				strTemp = path.GetFileName();
				if (!strTemp.IsEmpty())
					strMacroValue = strTemp;
			}
			break;
		}

		case IDMACRO_PROJEXT:	// file extension of project
		case IDMACRO_TARGEXT:	// file extension of target
		case IDMACRO_INPTEXT:	// input can only be the project/target
		{
			if (GetExeProjectTargetPath(path, strMacroValue))
			{
				strTemp = path.GetExtension();
				if (!strTemp.IsEmpty())
					strMacroValue = strTemp;
			}
			break;
		}

		// didn't deal with this
//		case IDMACRO_PLATNAME:
//		case IDMACRO_INTDIR:
//		case IDMACRO_CFGNAME:
		default:
			return FALSE;	// invalid
	}

	return TRUE;	// ok
}

BOOL CDebugSettingsBase::GetExeProjectTargetPath(CPathW& rProjPath, CStringW& rstrProjPath)
{
	CComBSTR bstrProjPath;
	HRESULT hr = GetEvaluatedStrProperty(VCDSID_BaseCommand, &bstrProjPath);
	VSASSERT(SUCCEEDED(hr), "Should always be able to get base command for an exe project!");
	rstrProjPath = bstrProjPath;
	if (rstrProjPath.IsEmpty())
		return FALSE;

	if (rProjPath.CreateFromKnown(rstrProjPath))
		return TRUE;

	VSASSERT(FALSE, "Failed to create project directory path");
	return FALSE;
}

///////////////////////////////////////////////////////////////////////////////
// CDebugSettings::GetEngineForDebuggerType
// 
// Description: Returns the CLSID for a debug engine that best matches the 
//		debugger type supplied.
//
void CDebugSettings::GetEngineForDebuggerType(TypeOfDebugger debuggerType, const IMAGE_INFO* pImageInfo, CLSID* pClsid)
{
	VSASSERT(pClsid != NULL, __FUNCTION__ "() - pClsid can't be null");
	if (pClsid == NULL)
		return;

	switch (debuggerType)
	{
		case DbgNativeOnly:
			*pClsid = guidNativeOnlyEng;
			break;

		case DbgManagedOnly:
			*pClsid = guidCOMPlusOnlyEng;
			break;

		case DbgMixed:
			*pClsid = guidCOMPlusNativeEng;
			break;

		case DbgAuto:
		{
			*pClsid = guidNativeOnlyEng;	// Assume we'll wind up with hard-mode native debugging.

			// DbgAuto means look at the project type and see if it is managed or not
			// If it contains managed, then use Mixed, else look at the .exe
			CComPtr<IDispatch> pDispTools;
			CComQIPtr<IVCCollection> pCollTools;
			CComPtr<IDispatch> pDispCompiler;

			if (m_spProjCfg && (m_spProjCfg->get_Tools(&pDispTools) == S_OK))
			{
				pCollTools = pDispTools;

				if (pCollTools)
				{
					HRESULT hr = pCollTools->Item(CComVariant("C/C++ Compiler Tool"), &pDispCompiler);
					if ((hr == S_OK) && pDispCompiler)
					{
						CComQIPtr<VCCLCompilerTool> pCompiler = pDispCompiler;
						compileAsManagedOptions managedExt;

						if (pCompiler)
						{
							hr = pCompiler->get_CompileAsManaged( &managedExt );		// S_FALSE is a common return
							if (SUCCEEDED(hr))
							{
								// REVIEW: how should this be handled when there is just managed meta-data (/clr or /clr:noAssembly)?
								if (managedExt == managedAssembly)
									*pClsid = guidCOMPlusNativeEng;
							}
						}
					}
				}
			}
			else // Project type doesn't specify managed, so check the exe's image.
			{
				if (pImageInfo)
				{
					switch(pImageInfo->ImageType)
					{
						case IMAGE_TYPE_WIN32:
							if (pImageInfo->u.Win32.ManagedCode)
							{
								*pClsid = guidCOMPlusOnlyEng;
							}
							break;
			
						case IMAGE_TYPE_WIN64:
						case IMAGE_TYPE_CRASHDUMP:
							break;
					}
				}
			}
		}
		break;
	}
}


void CDebugSettingsPage::ResetParentIfNeeded()
{
	CComQIPtr<IVCStagedPropertyContainer> spStagedContainer = m_pContainer;
	CComPtr<IVCPropertyContainer> spLiveContainer;
	if (spStagedContainer)
		spStagedContainer->get_Parent(&spLiveContainer);
	if (spLiveContainer == NULL)
		spLiveContainer = m_pContainer;
	CComQIPtr<VCConfiguration> spProjCfg = spLiveContainer;
	if (spProjCfg)
	{
		if (spStagedContainer)
			spStagedContainer->ReleaseStagedContainer();

		// this is not an exe project so find the debug settings object to go with this.
		CComPtr<IDispatch> spDispDbgSettings;
		spProjCfg->get_DebugSettings(&spDispDbgSettings);
		CComQIPtr<VCDebugSettings> spDbgSettings = spDispDbgSettings;
		VSASSERT(spDbgSettings != NULL, "Config probably not initialized correctly.");

		spStagedContainer.Release();
		spLiveContainer = spDbgSettings;
		if (spLiveContainer)
		{
			spLiveContainer->GetStagedPropertyContainer(VARIANT_TRUE, &spStagedContainer);
			CComQIPtr<IVCPropertyContainer> spContainer = spStagedContainer;
			if (spContainer)
			{
				spLiveContainer = spContainer;
				spStagedContainer = spContainer;
				if (spStagedContainer)
					spStagedContainer->HoldStagedContainer();
			}
		}

		m_pContainer = spLiveContainer;
	}
}

void CDebugSettingsPage::GetBaseDefault(long id, CComVariant& varValue)
{
	if (id == VCDSID_Command)
	{
		CComBSTR bstrVal;
		SGetCommand(&bstrVal);
		varValue = bstrVal;
	}
	else
		CBase::GetBaseDefault(id, varValue);
}

HRESULT CDebugSettingsPage::SGetCommand(BSTR* pVal)
{
	CComQIPtr<IVCStagedPropertyContainer> spStagedContainer = m_pContainer;
	CComQIPtr<IVCPropertyContainer> spParent;
	if (spStagedContainer)
		spStagedContainer->get_Parent(&spParent);
	if (spParent == NULL)
		spParent = m_pContainer;
	CComQIPtr<VCDebugSettings> spDbgSettings = spParent;
	if (spDbgSettings)
		return spDbgSettings->get_Command(pVal);

	return S_FALSE;
}

STDMETHODIMP CDebugSettingsPage::get_Command(BSTR *pVal)
{
	HRESULT hr = GetStrProperty(VCDSID_Command, pVal);
	if (hr != S_FALSE)
		return hr;

	CComBSTR bstrVal = *pVal;
	if (bstrVal.Length() == 0)
		hr = SGetCommand(pVal);

	return hr;
}

/*static */
HRESULT CDebugSettingsPage::BrowseForCommand( CStringW* pstrCommand, HWND hwndOwner /* = NULL */ )
{
	VSOPENFILENAMEW openFileName = {0};
	CStringW strFilter( MAKEINTRESOURCE( IDS_EXEFilter ));
	CStringW strTitle( MAKEINTRESOURCE( IDS_DebugProgram ));

	strFilter.Replace( L';', 0 );
	strFilter += L'\0';

	openFileName.lStructSize = sizeof (VSOPENFILENAMEW);
	openFileName.hwndOwner = hwndOwner;
	openFileName.pwzDlgTitle = strTitle;
	openFileName.pwzFileName = pstrCommand->GetBuffer( _MAX_PATH );
	openFileName.nMaxFileName = _MAX_PATH;
	openFileName.pwzInitialDir = NULL;
	openFileName.pwzFilter = strFilter;	 
	openFileName.nFilterIndex = 0;
	openFileName.nFileOffset = 0;
	openFileName.nFileExtension = 0;
	openFileName.dwHelpTopic = 0;
	openFileName.dwFlags = 0;

	return static_cast<CVCProjectEngine*>(g_pProjectEngine)->GetOpenFileNameViaDlg( &openFileName );
}


/*
	The UI shows Command as being a dropdrown.	When and item from the 
	dropdown is selected, the "predifined value" we provided is passed 
	in here, converted to a string.	 When the user types into the edit 
	box, what she types is passed
*/
STDMETHODIMP CDebugSettingsPage::put_Command(BSTR newVal)
{

	HRESULT hr = S_OK;

	int id = _wtoi(newVal);
	
	CStringW strCommand;

	{
		CComBSTR bstr;
		GetStrProperty (VCDSID_Command, &bstr);

		strCommand = bstr;
	}

	// NOTE: please update ..\ui_dll\src\exefordbg.h,OnSelectExeForDbg if you update this 
	// switch statement in any way
	switch (id)
	{
	case VCDSID_Command_IExplore:
		//
		// User selected default browser, go find it
		//
		if (FAILED(hr = CDebugSettings::GetBrowserCommand( &strCommand, NULL )))
		{
			VSFAIL ("Couldn't find a browser!");
		}
		break;

	case VCDSID_Command_ATLTest:
		hr = CDebugSettings::GetTestContainerCommand( strCommand );
		break;
		
	case VCDSID_Command_RegSvr:
		hr = CDebugSettings::GetRegSvrCommand( strCommand );
		break;
		
	case VCDSID_Command_Browse:
		hr = BrowseForCommand( &strCommand );
		if (hr != S_OK)
		{
			return hr;
		}
		break;
		
	default:
		//
		// User must have typed something in - use it as-is.
		//
		strCommand = newVal;
		hr = S_OK;
		break;
	}
	
	if (hr != S_OK)
	{
		return hr;
	}

	else
	{
		return SetStrProperty(VCDSID_Command, CComBSTR( strCommand ));
	}
}

STDMETHODIMP CDebugSettingsPage::get_WorkingDirectory(BSTR *pVal)
{
	return GetStrProperty(VCDSID_WorkingDirectory, pVal);
}

STDMETHODIMP CDebugSettingsPage::put_WorkingDirectory(BSTR newVal)
{
	CStringW strNewVal = newVal;
	strNewVal.TrimLeft();
	strNewVal.TrimRight();
	return SetStrProperty(VCDSID_WorkingDirectory, CComBSTR(newVal));
}

STDMETHODIMP CDebugSettingsPage::get_CommandArguments(BSTR *pVal)
{
	return GetStrProperty(VCDSID_CommandArguments, pVal);
}

STDMETHODIMP CDebugSettingsPage::put_CommandArguments(BSTR newVal)
{
	return SetStrProperty(VCDSID_CommandArguments, newVal);
}

STDMETHODIMP CDebugSettingsPage::get_Attach(VARIANT_BOOL *pVal)
{
	return GetBoolProperty(VCDSID_Attach, pVal);
}

STDMETHODIMP CDebugSettingsPage::put_Attach(VARIANT_BOOL newVal)
{
	CHECK_VARIANTBOOL(newVal);
	return SetBoolProperty(VCDSID_Attach, newVal);
}

STDMETHODIMP CDebugSettingsPage::get_SQLDebugging(VARIANT_BOOL *pVal)
{
	return GetBoolProperty(VCDSID_SQLDebugging, pVal);
}

STDMETHODIMP CDebugSettingsPage::put_SQLDebugging(VARIANT_BOOL newVal)
{
	CHECK_VARIANTBOOL(newVal);
	return SetBoolProperty(VCDSID_SQLDebugging, newVal);
}


STDMETHODIMP CDebugSettingsPage::get_Remote(RemoteDebuggerType *pVal)
{
	return GetIntProperty(VCDSID_Remote, (long *)pVal);
}

STDMETHODIMP CDebugSettingsPage::put_Remote(RemoteDebuggerType newVal)
{
	if (newVal != DbgLocal)
	{
		if (newVal == DbgRemoteTCPIP)
		{
			SetIntProperty(VCDSID_DebuggerType, DbgNativeOnly);
		}

		// Put the current machine name as the remote if none
		// is currently specified.
		CComBSTR bstrMachineName;
		GetStrProperty(VCDSID_RemoteMachine, &bstrMachineName);

		if (bstrMachineName == NULL || bstrMachineName[0] == L'\0')
		{
			DWORD dwBufLen = MAX_COMPUTERNAME_LENGTH + 1;
			wchar_t szComputerName[MAX_COMPUTERNAME_LENGTH + 1];
			GetComputerNameW(szComputerName, &dwBufLen);

			CComBSTR bstrComputerName = szComputerName;
			SetStrProperty(VCDSID_RemoteMachine, bstrComputerName);
		}
	}

	m_pPage->Refresh();
	return SetIntProperty(VCDSID_Remote, newVal);
}

STDMETHODIMP CDebugSettingsPage::get_RemoteMachine(BSTR *pVal)
{
	return GetStrProperty(VCDSID_RemoteMachine, pVal);
}

STDMETHODIMP CDebugSettingsPage::put_RemoteMachine(BSTR newVal)
{
	CStringW strNewVal = newVal;
	strNewVal.TrimLeft();
	strNewVal.TrimRight();
	return SetStrProperty(VCDSID_RemoteMachine, CComBSTR(newVal));
}

STDMETHODIMP CDebugSettingsPage::get_DebuggerType(TypeOfDebugger *pVal)
{
	return GetIntProperty(VCDSID_DebuggerType, (long *)pVal);
}

STDMETHODIMP CDebugSettingsPage::put_DebuggerType(TypeOfDebugger newVal)
{
	if (newVal != DbgNativeOnly)
	{
		//
		// Force remote type to not be TCP/IP
		//

		RemoteDebuggerType type;
		if (S_OK == GetIntProperty(VCDSID_Remote, (long *)&type) && type == DbgRemoteTCPIP)
		{
			SetIntProperty(VCDSID_Remote, DbgRemote);
		}
	}
	return SetIntProperty(VCDSID_DebuggerType, newVal);
}

STDMETHODIMP CDebugSettingsPage::get_RemoteCommand(BSTR *pVal)
{
	return GetStrProperty(VCDSID_RemoteCommand, pVal);
}

STDMETHODIMP CDebugSettingsPage::put_RemoteCommand(BSTR newVal)
{
	CStringW strNewVal = newVal;
	strNewVal.TrimLeft();
	strNewVal.TrimRight();
	return SetStrProperty(VCDSID_RemoteCommand, CComBSTR(newVal));
}

STDMETHODIMP CDebugSettingsPage::get_HttpUrl(BSTR *pVal)
{
	return GetStrProperty(VCDSID_HttpUrl, pVal);
}

STDMETHODIMP CDebugSettingsPage::put_HttpUrl(BSTR newVal)
{
	CStringW strNewVal = newVal;
	strNewVal.TrimLeft();
	strNewVal.TrimRight();
	return SetStrProperty(VCDSID_HttpUrl, CComBSTR(strNewVal));
}

STDMETHODIMP CDebugSettingsPage::get_PDBPath(BSTR* pbstrVal)
{
	return GetStrProperty(VCDSID_PDBPath, pbstrVal);
}

STDMETHODIMP CDebugSettingsPage::put_PDBPath(BSTR bstrNewVal)
{
	CStringW strNewVal = bstrNewVal;
	strNewVal.TrimLeft();
	strNewVal.TrimRight();
	return SetStrProperty(VCDSID_PDBPath, CComBSTR(strNewVal));
}

//
// A typesafe CoTaskMemAlloc wrapper
//
template <typename T>
T*
TTaskMemeAlloc(UINT_PTR cItems)
{
	return static_cast <T*> (CoTaskMemAlloc ( (ULONG) cItems * sizeof (T)));
}

#define ELEM_COUNT(x) (sizeof((x))/sizeof((*x)))

/*
	The "predefined strings" are what is shown to the user in a dropdown

	We supply the caller with a mapping of strings (to display) and cookies.  

	Cookie values can be anything unique, and we use the resource IDs of 
	the strings (which we get out of rgDebugCommands).
*/
STDMETHODIMP CDebugSettingsPage::GetPredefinedStrings( DISPID dispID, CALPOLESTR *pCaStringsOut, CADWORD *pCaCookiesOut )
{
	switch (dispID)
	{
	default:
		return CBase::GetPredefinedStrings( dispID, pCaStringsOut, pCaCookiesOut);
		
	case VCDSID_Command:
#ifdef DEBUG
		{
			HRESULT hr;
			CComVariant var;
			hr = CComDispatchDriver::GetProperty( this, dispID, &var );
			VSASSERT (hr == S_OK, "");
			VSASSERT (var.vt == VT_BSTR, "");
		}
#endif
		
		int iItem;
		const int cItems = ELEM_COUNT(rgDebugCommands);
		
		pCaStringsOut->cElems = cItems;
		pCaCookiesOut->cElems = cItems;

		pCaStringsOut->pElems = TTaskMemeAlloc<LPOLESTR> (cItems);
		RETURN_ON_NULL2(pCaStringsOut->pElems, E_OUTOFMEMORY);
			
		pCaCookiesOut->pElems = TTaskMemeAlloc<DWORD> (cItems);
		RETURN_ON_NULL2(pCaCookiesOut->pElems, E_OUTOFMEMORY);
		
		for (iItem = 0; iItem < cItems; iItem++)
		{
			pCaCookiesOut->pElems[iItem] = rgDebugCommands[iItem];
			
			CStringW str (MAKEINTRESOURCE (rgDebugCommands[iItem]));
			VSASSERT (str.GetLength(), "");
			
			pCaStringsOut->pElems[iItem] = TTaskMemeAlloc<OLECHAR>( str.GetLength() + 1 );
			wcscpy( pCaStringsOut->pElems[iItem], str );
		}
		break;
	}
	
	return S_OK;
}

/*
	Here we provide yet another mapping, this time between cookies (which 
	were supplied above) and some variants.	 Whatever you put in the 
	variant will be passed to your put_ function.  The two most obvious 
	options in any implementation are 1) the cookie and 2) the string you 
	passed in last time.

	We return the cookie in the variant.

	I'd say this is very silly.
*/
STDMETHODIMP CDebugSettingsPage::GetPredefinedValue( DISPID dispID, DWORD dwCookie, VARIANT* pVarOut )
{
	switch (dispID)
	{
	default:
		return CBase::GetPredefinedValue (dispID, dwCookie, pVarOut);
	case VCDSID_Command:
		CComVariant( dwCookie ).Detach( pVarOut );
		break;
	}
	return S_OK;
}

STDMETHODIMP CDebugSettingsPage::HideProperty( DISPID dispid, BOOL *pfHide)
{
	CHECK_POINTER_NULL(pfHide);
	if( dispid == VCDSID_SQLDebugging )
	{
		long SKU;
		static_cast<CVCProjectEngine*>(g_pProjectEngine)->GetSKU( &SKU );
		switch( SKU )
		{
			case VSASKUEdition_Book:
			case VSASKUEdition_Standard:
				*pfHide = TRUE;
				break;
			default:
				*pfHide = FALSE;
				break;
		}
	}
	else
		*pfHide = FALSE;
	return S_OK;
};


STDMETHODIMP CDebugSettingsPage::MapPropertyToCategory( DISPID dispid, PROPCAT* ppropcat)
{

	switch( dispid )
	{
		case VCDSID_Command:
		case VCDSID_CommandArguments:
		case VCDSID_WorkingDirectory:
		case VCDSID_Attach:
		case VCDSID_PDBPath:
			*ppropcat = IDS_ACTION;
			break;
		case VCDSID_Remote:
		case VCDSID_RemoteCommand:
		case VCDSID_RemoteMachine:
		case VCDSID_HttpUrl:
			*ppropcat = IDS_REMOTESETTINGS;
			break;
		case VCDSID_DebuggerType:
		case VCDSID_SQLDebugging:
		case VCDSID_BaseCommand:
		default:
			*ppropcat = IDS_DEBUGGERS;
			break;
	}
	return S_OK;
}

STDMETHODIMP CDebugSettingsPage::GetCategoryName( PROPCAT propcat, LCID lcid, BSTR* pbstrName)
{
	CComBSTR bstrCat;
	bstrCat.LoadString(propcat);
	return bstrCat.CopyTo(pbstrName);
}


HRESULT CVCStagedExeConfigPropertyContainer::CreateInstance(IVCPropertyContainer* pParentPropContainer, 
	IVCStagedPropertyContainer **ppStagedPropertyContainer)
{
	CVCStagedExeConfigPropertyContainer *pVar;
	CComObject<CVCStagedExeConfigPropertyContainer> *pObj;
	HRESULT hr = CComObject<CVCStagedExeConfigPropertyContainer>::CreateInstance(&pObj);
	if (SUCCEEDED(hr))
	{
		pVar = pObj;
		pVar->AddRef();
		*ppStagedPropertyContainer = pVar;
		hr = pVar->Initialize(pParentPropContainer);
	}
	return hr;
}

HRESULT CVCStagedExeConfigPropertyContainer::Initialize(IVCPropertyContainer* pParentPropContainer)
{
	m_spParentPropContainer = pParentPropContainer;
	return (m_spParentPropContainer ? S_OK : E_UNEXPECTED);
}

HRESULT CVCStagedExeConfigPropertyContainer::DoGetParentProp(long id, VARIANT_BOOL bAllowInherit, BOOL bCheckSpecialProps, VARIANT* pVarValue)
{
	CHECK_POINTER_NULL(pVarValue);
	RETURN_ON_NULL2(m_spParentPropContainer, S_FALSE);

	if (bAllowInherit)
	{
		if (m_spParentPropContainer->GetProp(id, pVarValue) == S_OK )
			return S_OK;
	}
	else
	{
		if (m_spParentPropContainer->GetLocalProp(id, pVarValue) == S_OK )
			return S_OK;
	}

	// We didn't find it
	return S_FALSE;  
}

STDMETHODIMP CVCStagedExeConfigPropertyContainer::GetMultiProp(long id, LPCOLESTR szSeparator, VARIANT_BOOL bSkipLocal, BSTR *pbstrValue)
{
	CHECK_POINTER_NULL(pbstrValue);
	*pbstrValue = NULL;

	CComBSTR bstrVal;
	
	if (!bSkipLocal)
	{
		// look in here.
		CComVariant var;
		BOOL b;
		b = m_PropMap.Lookup(id, var);
		if( b )
		{
			bstrVal = var.bstrVal;
			*pbstrValue = bstrVal.Detach();
			return S_OK;
		}
		else if (m_spParentPropContainer)
		{
			HRESULT hr = m_spParentPropContainer->GetLocalProp(id, &var);
			if (hr == S_OK)
			{
				bstrVal = var.bstrVal;
				*pbstrValue = bstrVal.Detach();
				return S_OK;
			}
		}
	}
	
	// And check your parents
	if (m_spParentPropContainer && 
		m_spParentPropContainer->GetStrProperty(id, &bstrVal) == S_OK)
	{
		*pbstrValue = bstrVal.Detach();
		return S_OK;
	}

	return S_FALSE;
}

STDMETHODIMP CVCStagedExeConfigPropertyContainer::GetMultiPropSeparator(long id, BSTR* pbstrPreferred, BSTR* pbstrAll)
{
	if (m_spParentPropContainer)
		return m_spParentPropContainer->GetMultiPropSeparator(id, pbstrPreferred, pbstrAll);

	// no parent?  make an educated guess
	CHECK_POINTER_NULL(pbstrPreferred);
	CHECK_POINTER_NULL(pbstrAll);
	CComBSTR bstrPreferred = L";";
	CComBSTR bstrAll = L";,";
	*pbstrPreferred = bstrPreferred.Detach();
	*pbstrAll = bstrAll.Detach();
	return S_OK;
}

STDMETHODIMP CVCStagedExeConfigPropertyContainer::ClearPropertyRange(long nMinID, long nMaxID)
{
	VCPOSITION pos = m_PropMap.GetStartPosition();
	while (pos)
	{
		long key;
		CComVariant varVal;
		m_PropMap.GetNextAssoc(pos, key, varVal);
		if (key >= nMinID && key <= nMaxID)
			m_PropMap.RemoveKey(key);
	}

	return S_OK;
}

STDMETHODIMP CVCStagedExeConfigPropertyContainer::get_Parent(IVCPropertyContainer** ppParent)
{
	CHECK_POINTER_NULL(ppParent);
	return m_spParentPropContainer.CopyTo(ppParent);
}

STDMETHODIMP CVCStagedExeConfigPropertyContainer::GetGrandParentProp(long id, VARIANT *pvarValue)		// skip to parent of our non-staged parent
{
	CHECK_ZOMBIE(m_spParentPropContainer, IDS_ERR_CFG_ZOMBIE);
	return m_spParentPropContainer->GetParentProp(id, VARIANT_TRUE /* allow inherit */, pvarValue);
}

STDMETHODIMP CVCStagedExeConfigPropertyContainer::Close()
{
	m_spParentPropContainer.Release();
	return S_OK;
}

STDMETHODIMP CVCStagedExeConfigPropertyContainer::CommitPropertyRange(long nMinID, long nMaxID)
{
	return DoCommitPropertyRange(nMinID, nMaxID, false);
}

STDMETHODIMP CVCStagedExeConfigPropertyContainer::Commit()
{
	return DoCommitPropertyRange(0, 0, true);
}

HRESULT CVCStagedExeConfigPropertyContainer::DoCommitPropertyRange(long nMinID, long nMaxID, bool bIgnoreRange)
{
	CHECK_ZOMBIE(m_spParentPropContainer, IDS_ERR_CFG_ZOMBIE);

	VCPOSITION iter = m_PropMap.GetStartPosition();
	// iterate over each item in the map
	HRESULT hr = S_OK;
	while( iter && SUCCEEDED(hr))
	{
		long key;
		CComVariant varVal;
		m_PropMap.GetNextAssoc(iter, key, varVal);

		if (!bIgnoreRange && (key < nMinID || key > nMaxID))
			continue;

		BOOL bSetProp = TRUE;
		if ((varVal.vt == VT_I4 && varVal.intVal == INHERIT_PROP_VALUE) || 
			(varVal.vt == VT_I2 && varVal.iVal == INHERIT_PROP_VALUE) || 
			(varVal.vt == VT_BSTR && wcscmp( L"-2", varVal.bstrVal ) == 0))
			bSetProp = FALSE;
		else if (varVal.vt == VT_BSTR && SysStringLen( varVal.bstrVal ) == 0 )
		{
			VARIANT_BOOL bIsMulti = VARIANT_FALSE;
			if (m_spParentPropContainer->IsMultiProp(key, &bIsMulti) != S_OK || bIsMulti == VARIANT_TRUE)
				bSetProp = FALSE;
		}
		if (bSetProp)
			hr = m_spParentPropContainer->SetProp(key, varVal);
		else
			hr = m_spParentPropContainer->Clear(key);
		RETURN_ON_FAIL(hr);
		m_PropMap.RemoveKey(key);
	}

	return S_OK;
}

STDMETHODIMP CVCStagedExeConfigPropertyContainer::NextElement(long* pKey, VARIANT* pVal)
{
	CHECK_POINTER_NULL(pKey);
	*pKey = 0;
	CHECK_POINTER_NULL(pVal);
	RETURN_ON_NULL2(pVal, E_POINTER);
	pVal->vt = VT_EMPTY;

	RETURN_ON_NULL2(m_pos, S_FALSE);

	long id;
	CComVariant var;
	m_PropMap.GetNextAssoc(m_pos, id, var);
	*pKey = id;
	var.Detach(pVal);

	return S_OK;
}

STDMETHODIMP CVCStagedExeConfigPropertyContainer::Reset(void)
{
	m_pos = m_PropMap.GetStartPosition();
	return S_OK;
}

STDMETHODIMP CVCStagedExeConfigPropertyContainer::HasDefaultValue(DISPID dispid, BOOL *pfDefault)
{
	CHECK_POINTER_NULL(pfDefault);
	*pfDefault = FALSE;
	
	// Check if its here and not clear
	CComVariant varVal;
	if( m_PropMap.Lookup(dispid, varVal) )
	{
		if( (varVal.vt == VT_I4 && varVal.intVal == INHERIT_PROP_VALUE) || (varVal.vt == VT_I2 && varVal.iVal == INHERIT_PROP_VALUE) || (varVal.vt == VT_BSTR && wcscmp( L"-2", varVal.bstrVal ) == 0) )
			*pfDefault = TRUE;
		return S_OK;
	}

	// OK so is it in the parent ?
	if (!m_spParentPropContainer || m_spParentPropContainer->GetLocalProp( dispid, NULL ) != S_OK)
		*pfDefault = TRUE;

	return S_OK;
}

STDMETHODIMP CVCStagedExeConfigPropertyContainer::HoldStagedContainer()			// number of calls to Hold/Release StagedContainer need to match
{
	m_cStaged++;
	return S_OK;
}

STDMETHODIMP CVCStagedExeConfigPropertyContainer::ReleaseStagedContainer()		// number of calls to Hold/Release StagedContainer need to match
{
	if (m_cStaged > 0)
		m_cStaged--;
	if (m_cStaged == 0 && m_spParentPropContainer)
		m_spParentPropContainer->ReleaseStagedPropertyContainer();

	return S_OK;
}
