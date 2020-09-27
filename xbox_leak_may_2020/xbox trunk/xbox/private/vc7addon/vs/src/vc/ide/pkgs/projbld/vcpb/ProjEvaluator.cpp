#include "stdafx.h"
#include <register.h>

#include "ProjEvaluator.h"
#include "fileregistry.h"
#include "bldhelpers.h"
#include "BuildEngine.h"

CStringW CProjectEvaluator::s_strVCDir = L"";
CStringW CProjectEvaluator::s_strVSDir = L"";
CStringW CProjectEvaluator::s_strComDir = L"";
CStringW CProjectEvaluator::s_strComVer = L"";
CStringW CProjectEvaluator::s_strComSDKDir = L"";
bool CProjectEvaluator::s_bComDirsInit = false;

//
// primitive custom build 'tool macros'
//

typedef struct 
{
	UINT idMacro;
	const wchar_t * pchName;
} S_ToolMacroMapEl;

// we read this from the bottom, so put most frequently used stuff at the end
const S_ToolMacroMapEl mapToolMacros[] =
{
	{IDMACRO_SOLNEXT,	L"SolutionExt"},		// solution extension (ex: .sln)
	{IDMACRO_PROJEXT,	L"ProjectExt"},			// project extension (ex: .vcproj)
	{IDMACRO_PLATNAME,	L"PlatformName"},		// platform name
	{IDMACRO_SOLNBASE,	L"SolutionName"},		// solution file basename (ex: foo)
	{IDMACRO_SOLNDIR,	L"SolutionDir"},		// absolute path to solution directory (ex: c:\foo\)
	{IDMACRO_SOLNPATH,	L"SolutionPath"},		// absolute path to solution file (ex: c:\foo\foo.sln)
	{IDMACRO_SOLNFILE,	L"SolutionFileName"},	// solution file basename + extension (ex: foo.sln)
	{IDMACRO_REMOTEMACH,L"RemoteMachine"},		// remote machine name (ex: MyServer)
	{IDMACRO_PROJDIR,	L"ProjectDir"},			// absolute path to project directory (ex: c:\foo\bar\)
	{IDMACRO_TARGDIR,	L"TargetDir"},			// target (output) directory (ex: c:\foo\bar\debug\)
	{IDMACRO_INPTDIR,	L"InputDir"},			// input directory (ex: c:\foo\bar\)
	{IDMACRO_ENVDIR,	L"DevEnvDir"},			// devenv directory
	{IDMACRO_TARGFILE,	L"TargetFileName"},		// target (output) file basename + extension (ex: foobar.exe)
	{IDMACRO_INPTEXT,	L"InputExt"},			// input extension (ex: .cpp)
	{IDMACRO_TARGEXT,	L"TargetExt"},			// target (output) file extension (ex: .exe)
	{IDMACRO_PROJFILE,	L"ProjectFileName"},	// project file basename + extension (ex: bar.vcproj)
	{IDMACRO_PROJPATH,	L"ProjectPath"},		// absolute path to project file (ex: c:\foo\bar\bar.vcproj)
	{IDMACRO_INPTFILE,	L"InputFileName"},		// input file basename + extension (ex: myfile.cpp)
	{IDMACRO_INPTPATH,	L"InputPath"},			// full path of input (ex: c:\foo\bar\myfile.cpp)
	{IDMACRO_INPTBASE,	L"InputName"},			// input file basename (ex: myfile)
	{IDMACRO_TARGPATH,	L"TargetPath"},			// full path of target (output) (ex: c:\foo\bar\debug\foobar.exe)
	{IDMACRO_TARGBASE,	L"TargetName"},			// target (output) file basename (ex: foobar)
	{IDMACRO_PROJBASE,	L"ProjectName"},		// project file basename (ex: bar)
	{IDMACRO_CFGNAME,	L"ConfigurationName"},	// configuration name
	{IDMACRO_OUTDIR,	L"OutDir"},				// output directory
	{IDMACRO_INTDIR,	L"IntDir"},				// intermediate directory
	{IDMACRO_VCDIR,		L"VCInstallDir"},		// installation directory for Visual C++
	{IDMACRO_VSDIR,		L"VSInstallDir"},		// installation directory for Visual Studio
	{IDMACRO_COMDIR,	L"FrameworkDir"},		// root directory for COM+
	{IDMACRO_COMSDKDIR,	L"FrameworkSDKDir"},	// root directory for the COM+ framework SDK
	{IDMACRO_COMVER,	L"FrameworkVersion"},	// version of COM+ we're running
	{IDMACRO_INHERIT,	L"Inherit"},			// put multi-prop inheritance HERE (suppress when scanning here)
	{IDMACRO_NOINHERIT,	L"NoInherit"},			// explicitly don't inherit in a multi-prop (suppress when scanning here)
};


// map a macro name to a macro id (IDM_)
BOOL CProjectEvaluator::MapMacroNameToId(const wchar_t* pchName, int& cchName, UINT& idMacro)
{
	cchName = 0;
	WCHAR* chCloseParen = wcschr(pchName, L')');
	int nCloseParenLoc = (chCloseParen == NULL) ? -1 : (int)(chCloseParen - pchName);

	for (int i = sizeof(mapToolMacros) / sizeof(S_ToolMacroMapEl); i > 0; i--)
	{
		int cch = (int)wcslen(mapToolMacros[i-1].pchName);
		if ((cch > cchName) &&
			(_wcsnicmp(mapToolMacros[i-1].pchName, pchName, cch) == 0))
		{
			idMacro = mapToolMacros[i-1].idMacro;
			cchName = cch;	// found one match, possibly try for bigger match....
			if (nCloseParenLoc > 0 && cchName == nCloseParenLoc)	// exact match, let's go
				break;
		}
	}

	return cchName != 0;	// not mapped
}

// map a macro id (IDM_) to the macro name
BOOL CProjectEvaluator::MapMacroIdToName(UINT idMacro, CStringW& strName)
{
	for (int i = sizeof(mapToolMacros) / sizeof(S_ToolMacroMapEl); i > 0; i--)
	{
		if (mapToolMacros[i-1].idMacro == idMacro)
		{
			strName = mapToolMacros[i-1].pchName;
			return TRUE;	// mapped
		}
	}

	return FALSE;	// not mapped
}

BOOL CProjectEvaluator::GetSolutionPath(CPathW& rSolnPath)
{
	CStringW strSolnFile;
	RETURN_ON_NULL2(CVCProjectEngine::s_pBuildPackage, FALSE);
	CComBSTR bstrSolnFile;
	HRESULT hr = CVCProjectEngine::s_pBuildPackage->get_SolutionFile(&bstrSolnFile);
	strSolnFile = bstrSolnFile;
	if (strSolnFile.IsEmpty())
		return FALSE;

	BOOL bOK = rSolnPath.Create(strSolnFile);
	VSASSERT(bOK, "Solution path bad!");
	return TRUE;
}

BOOL CProjectEvaluator::GetSolutionDirectory(CStringW& rstrDir)
{
	CPathW solnPath;
	if (!GetSolutionPath(solnPath))
		return FALSE;

	CDirW solnDir;
	BOOL bOK = solnDir.CreateFromPath(solnPath);
	VSASSERT(bOK, "Solution directory bad!");

	rstrDir = (const wchar_t *)solnDir;
	NormalizeDirectoryString(rstrDir);

	return TRUE;
}

void CProjectEvaluator::NormalizeDirectoryString(CStringW& strDir)
{
	int nLen = strDir.GetLength();
	if (nLen <= 0)
		return;	// nothing to do

	if (strDir[nLen-1] == L'\\')
		return;		// already looks fine
	else if (strDir[nLen-1] == L'/')
		strDir.SetAt(nLen-1, L'\\');		// make it go the correct direction
	else
		strDir += L"\\";
}

BOOL CProjectEvaluator::GetProjectDirectoryPath(IVCPropertyContainer* pPropContainer, CPathW& rProjDirPath, 
	CStringW& rstrProjDirPath, BOOL bNormalize)
{
	RETURN_ON_NULL2(pPropContainer, FALSE);

	CComBSTR bstrProjPath;
	HRESULT hr = pPropContainer->GetStrProperty(VCPROJID_ProjectDirectory, &bstrProjPath);	// should never have macros in it
	VSASSERT(SUCCEEDED(hr), "Should always be able to get project directory!");
	rstrProjDirPath = bstrProjPath;
	if (rstrProjDirPath.IsEmpty())
		return FALSE;
	if (bNormalize)
		NormalizeDirectoryString(rstrProjDirPath);
	else
	{
		int nLast = rstrProjDirPath.GetLength()-1;
		if (rstrProjDirPath.GetAt(nLast) == L'\\' || rstrProjDirPath.GetAt(nLast) == L'/')
			rstrProjDirPath = rstrProjDirPath.Left(nLast);
	}

	if (rProjDirPath.CreateFromKnown(rstrProjDirPath))
		return TRUE;

	VSASSERT(FALSE, "Failed to create project directory path");
	return FALSE;
}

BOOL CProjectEvaluator::GetProjectDirectoryDir(IVCPropertyContainer* pPropContainer, CDirW& rProjDir)
{
	RETURN_ON_NULL2(pPropContainer, FALSE);

	CComBSTR bstrProjPath;
	HRESULT hr = pPropContainer->GetStrProperty(VCPROJID_ProjectDirectory, &bstrProjPath);	// should never have macros in it
	VSASSERT(SUCCEEDED(hr), "Should always be able to get project directory!");
	CStringW strProjPath = bstrProjPath;
	if (strProjPath.IsEmpty())
		return FALSE;

	if (rProjDir.CreateFromKnown(strProjPath))
		return TRUE;

	VSASSERT(FALSE, "Failed to create project directory path");
	return FALSE;
}

// get the value of a macro
BOOL CProjectEvaluator::GetMacroValue(UINT idMacro, CStringW& strMacroValue, IVCPropertyContainer* pPropContainer)
{
	VSASSERT(pPropContainer != NULL, "Cannot evaluate a macro without a property container to look things up in.");
	RETURN_ON_NULL2(pPropContainer, FALSE);

	CPathW path;
	CComBSTR bstrTemp;

	// empty macro-value
	strMacroValue.Empty();

	HRESULT hr = S_OK;

	switch (idMacro)
	{
		case IDMACRO_CFGNAME:	// configuration name
			hr = pPropContainer->GetEvaluatedStrProperty(VCCFGID_ConfigurationName, &bstrTemp);
			VSASSERT(SUCCEEDED(hr), "Should always be able to get configuration name!");
			strMacroValue = bstrTemp;
			break;

		case IDMACRO_INTDIR:	// intermediate directory
		case IDMACRO_OUTDIR:	// output directory
			EvalIntDirOutDir(pPropContainer, idMacro == IDMACRO_INTDIR, strMacroValue);
			break;

		case IDMACRO_ENVDIR:	// devenv directory
			UtilGetDevenvFolder(strMacroValue);
			break;

		case IDMACRO_SOLNDIR:	// solution directory
			GetSolutionDirectory(strMacroValue);
			break;

		case IDMACRO_SOLNBASE:	// solution file basename
			if (GetSolutionPath(path))
				path.GetBaseNameString(strMacroValue);
			break;

		case IDMACRO_SOLNFILE:	// solution file name
			if (GetSolutionPath(path))
				strMacroValue = path.GetFileName();
			break;

		case IDMACRO_SOLNEXT:	// solution file extension
			if (GetSolutionPath(path))
				strMacroValue = path.GetExtension();
			break;

		case IDMACRO_SOLNPATH:	// solution file path
			if (GetSolutionPath(path))
				path.GetFullPath(strMacroValue);
			break;

		case IDMACRO_PROJDIR:	// project directory
			if (!GetProjectDirectoryPath(pPropContainer, path, strMacroValue, TRUE))
				strMacroValue.Empty();
			break;

		case IDMACRO_PROJFILE:	// project file base name + extension
		case IDMACRO_PROJPATH:	// full path of project
		case IDMACRO_PROJEXT:	// project file extension
			EvalProjectMacroValue(pPropContainer, idMacro, strMacroValue);
			break;

		case IDMACRO_PROJBASE:	// project file basename
			hr = pPropContainer->GetEvaluatedStrProperty(VCPROJID_Name, &bstrTemp);
			VSASSERT(SUCCEEDED(hr), "Should always be able to get at least default prop from a property container!");
			strMacroValue = bstrTemp;
			break;

		case IDMACRO_TARGDIR:	// target directory
		case IDMACRO_TARGPATH:	// full path of target
		case IDMACRO_TARGFILE:	// base name + extension of target
		case IDMACRO_TARGEXT:	// extension of target
		case IDMACRO_TARGBASE:	// target file basename
			EvalTargetMacroValue(pPropContainer, idMacro, strMacroValue);
			break;

		case IDMACRO_INPTDIR:	// input directory
		case IDMACRO_INPTEXT:	// extension of input
		case IDMACRO_INPTPATH:	// full path of input
		case IDMACRO_INPTFILE:	// base name + extension of input
		case IDMACRO_INPTBASE:	// input file basename
			EvalInputMacroValue(pPropContainer, idMacro, strMacroValue);
			break;

		case IDMACRO_REMOTEMACH:
			GetDebuggerProperty(pPropContainer, VCDSID_RemoteMachine, strMacroValue);
			break;

		case IDMACRO_PLATNAME:
			hr = pPropContainer->GetEvaluatedStrProperty(VCPLATID_Name, &bstrTemp);
			VSASSERT(SUCCEEDED(hr), "Should always be able to get at least default prop from a property container!");
			strMacroValue = bstrTemp;
			break;

		case IDMACRO_VCDIR:
			if (s_strVCDir.IsEmpty())
				UtilGetVCInstallFolder(s_strVCDir);
			strMacroValue = s_strVCDir;
			break;

		case IDMACRO_VSDIR:
			if (s_strVSDir.IsEmpty())
				UtilGetVSInstallFolder(s_strVSDir);
			strMacroValue = s_strVSDir;
			break;

		case IDMACRO_COMDIR:
		case IDMACRO_COMSDKDIR:
		case IDMACRO_COMVER:
			EvalFrameworkMacro(idMacro, strMacroValue);
			break;

		case IDMACRO_INHERIT:
		case IDMACRO_NOINHERIT:
			strMacroValue.Empty();	// just suppress the macro
			break;

		// didn't deal with this
		default:
			return FALSE;	// invalid
	}

	return TRUE;	// ok
}

BOOL CProjectEvaluator::EvalIntDirOutDir(IVCPropertyContainer* pPropContainer, BOOL bIsIntDir, CStringW& strMacroValue)
{
	// directory prop?
	long idOutDirProp = bIsIntDir ? VCCFGID_IntermediateDirectory : VCCFGID_OutputDirectory;

	CComBSTR bstrDir;
	HRESULT hr = pPropContainer->GetEvaluatedStrProperty(idOutDirProp, &bstrDir);
	VSASSERT(SUCCEEDED(hr), "Should always be able to get at least default prop from a property container!");
	strMacroValue = bstrDir;
	if (strMacroValue.IsEmpty())
		strMacroValue = L'.';

	return TRUE;
}

BOOL CProjectEvaluator::EvalProjectMacroValue(IVCPropertyContainer* pPropContainer, UINT idMacro, CStringW& strMacroValue)
{
	CComBSTR bstrProjFile;
	HRESULT hr = pPropContainer->GetEvaluatedStrProperty(VCPROJID_ProjectFile, &bstrProjFile);

	strMacroValue = bstrProjFile;
	if (idMacro == IDMACRO_PROJPATH)
		return TRUE;	// we're done

	CPathW pathProj;
	if (!pathProj.Create(strMacroValue))
	{
		strMacroValue.Empty();
		return FALSE;
	}

	if (idMacro == IDMACRO_PROJFILE)
		strMacroValue = pathProj.GetFileName();
	else	// idMacro == IDMACRO_PROJEXT
		strMacroValue = pathProj.GetExtension();

	return TRUE;
}

BOOL CProjectEvaluator::EvalTargetMacroValue(IVCPropertyContainer* pPropContainer, UINT idMacro, CStringW& strMacroValue)
{
	CComBSTR bstrOutput;
	HRESULT hr = pPropContainer->GetEvaluatedStrProperty(VCCFGID_PrimaryOutput, &bstrOutput);
//	Actually, you CAN get a bad default prop for this, such as when you have a Custom AppWizard project
//	VSASSERT(SUCCEEDED(hr), "Should always be able to get at least default prop from a property container!");
	CStringW strTemp = bstrOutput;
	if (FAILED(hr))
	{
		CComQIPtr<IVCConfigurationImpl> spProjCfgImpl = pPropContainer;
		if (spProjCfgImpl)
		{
			CComPtr<IVCBuildErrorContext> spEC;
			if (SUCCEEDED(spProjCfgImpl->get_ErrorContext(&spEC)) && spEC)
			{
				long nErrPropID = -1;
				CComPtr<IVCToolImpl> spOutputTool;
				spProjCfgImpl->GetPrimaryOutputFileID(&spOutputTool, &nErrPropID);
				CBldAction::InformUserAboutBadFileName(spEC, pPropContainer, spOutputTool, nErrPropID, strTemp);
			}
		}
		return FALSE;
	}
	if (strTemp.IsEmpty())
		return FALSE;
	CPathW pathOutput;
	if (!pathOutput.CreateFromKnown(strTemp))
		return FALSE;

	if (idMacro == IDMACRO_TARGDIR)
	{
		CDirW dir;
		if (dir.CreateFromPath(pathOutput))
		{
			strMacroValue = (const wchar_t *)dir;
			NormalizeDirectoryString(strMacroValue);
		}
	}
	else if (idMacro == IDMACRO_TARGPATH)
	{	// full path of target
		pathOutput.GetFullPath(strMacroValue);
	}
	else if (idMacro == IDMACRO_TARGFILE)
	{	// base name + extension of target
		strMacroValue = pathOutput.GetFileName();
	}
	else if (idMacro == IDMACRO_TARGEXT)
	{	// extension of target
		strMacroValue = pathOutput.GetExtension();
	}
	else
	{
		VSASSERT(idMacro == IDMACRO_TARGBASE, "Bad macro value");
		pathOutput.GetBaseNameString(strMacroValue);
	}

	return TRUE;
}

BOOL CProjectEvaluator::EvalInputMacroValue(IVCPropertyContainer* pPropContainer, UINT idMacro, CStringW& strMacroValue)
{
	BldFileRegHandle frh = NULL;
	CComPtr<IVCPropertyContainer> spActualContainer;
	CVCProjectEngine::ResolvePropertyContainer(pPropContainer, &spActualContainer);
	CComQIPtr<IVCBuildableItem> spItem = spActualContainer;
	RETURN_ON_NULL2(spItem, FALSE);

	HRESULT hr = spItem->get_FileRegHandle((void **)&frh);
	VSASSERT(SUCCEEDED(hr), "All items have a file reg handle!");

	RETURN_ON_NULL2(frh, FALSE);

	CBldFileRegEntry* fre = g_FileRegistry.GetRegEntry(frh);
	const CPathW* pPath = NULL;
	if (fre)
		pPath = fre->GetFilePath();
	RETURN_ON_NULL2(pPath, FALSE);

	CStringW strTemp;
	if (idMacro == IDMACRO_INPTDIR)
	{
		CDirW dir;
		if (dir.CreateFromPath(*pPath))
		{
			strMacroValue = (const wchar_t *)dir;
			NormalizeDirectoryString(strMacroValue);
		}
	}
	else if (idMacro == IDMACRO_INPTPATH)
	{
		pPath->GetFullPath(strMacroValue);
	}
	else if (idMacro == IDMACRO_INPTFILE)
	{
		strMacroValue = pPath->GetFileName();
	}
	else if (idMacro == IDMACRO_INPTEXT)
	{
		strMacroValue = pPath->GetExtension();
	}
	else
	{
		VSASSERT(idMacro == IDMACRO_INPTBASE, "Bad macro value.");
		pPath->GetBaseNameString(strMacroValue);
	}

	return TRUE;
}

void CProjectEvaluator::GetDebuggerProperty(IVCPropertyContainer* pPropContainer, long idProp, CStringW& strMacroValue)
{
	strMacroValue.Empty();

	CComQIPtr<IVCPropertyContainer> spPropContainerDbg;
	CComQIPtr<VCDebugSettings> spDebugSetting;

	CComPtr<IVCPropertyContainer> spActualContainer;
	CVCProjectEngine::ResolvePropertyContainer(pPropContainer, &spActualContainer);
	CComQIPtr<VCConfiguration> spProjCfg = spActualContainer;
	if (spProjCfg == NULL)
	{
		CComQIPtr<IVCFileConfigurationImpl> spFileCfgImpl = spActualContainer;
		if (spFileCfgImpl == NULL)
		{
			CComQIPtr<VCDebugSettings> spDebugSetting = spActualContainer;
			if (spDebugSetting == NULL)
				return;
			spPropContainerDbg = spActualContainer;
		}
		else
		{
			CComPtr<IDispatch> spDispProjCfg;
			spFileCfgImpl->get_Configuration(&spDispProjCfg);
			spProjCfg = spDispProjCfg;
			if (spProjCfg == NULL)
				return;
		}
	}

	if( spPropContainerDbg == NULL )
	{
		CComPtr<IDispatch> spDispDebugSettings;
		spProjCfg->get_DebugSettings(&spDispDebugSettings);
		spPropContainerDbg = spDispDebugSettings;
	}

	if (spPropContainerDbg == NULL)
		return;

	CComPtr<IVCStagedPropertyContainer> spStagedContainer;
	spPropContainerDbg->GetStagedPropertyContainer(VARIANT_FALSE, &spStagedContainer);
	CComQIPtr<IVCPropertyContainer> spActualDbgContainer;
	if (spStagedContainer)
		spActualDbgContainer = spStagedContainer;
	if (spActualDbgContainer == NULL)
		spActualDbgContainer = spPropContainerDbg;

	CComBSTR bstrTemp;
	HRESULT hr = spActualDbgContainer->GetEvaluatedStrProperty(idProp, &bstrTemp);
	if (SUCCEEDED(hr))
		strMacroValue = bstrTemp;
}

static LONG GetRegEntry(HKEY hSectionKey, LPCOLESTR szValue, CStringW& strBuf)
{
	wchar_t* szBuf = strBuf.GetBuffer(MAX_PATH+1);
	DWORD nType = REG_SZ;
	DWORD nSize = MAX_PATH;
	LONG lRet = RegQueryValueExW( hSectionKey, szValue, NULL, &nType, (LPBYTE)szBuf, &nSize );
	strBuf.ReleaseBuffer(-1);
	return lRet;
}

typedef HRESULT (STDAPICALLTYPE * LPFNGETRUNTIMEVER)(LPWSTR,DWORD,DWORD *);

void CProjectEvaluator::EvalFrameworkMacro(UINT idMacro, CStringW& strMacroValue)
{
	if (!s_bComDirsInit)
	{
		s_bComDirsInit = true;
		CStringW strKey = L"SOFTWARE\\Microsoft\\.NETFramework";
		HKEY hSectionKey = NULL;
		LONG lRet = RegOpenKeyExW( HKEY_LOCAL_MACHINE, strKey, 0, KEY_READ, &hSectionKey );
		if( hSectionKey )
		{
			lRet = GetRegEntry(hSectionKey, L"InstallRoot", s_strComDir);
			lRet = GetRegEntry(hSectionKey, L"sdkInstallRoot", s_strComSDKDir);

			if (s_strComDir.IsEmpty())
			{}	// nothing we can do
			else if (s_strComDir[s_strComDir.GetLength()-1] == L'/')
				s_strComDir = s_strComDir.Left(s_strComDir.GetLength()-1) + L"\\";
			else if (s_strComDir[s_strComDir.GetLength()-1] != L'\\')
				s_strComDir += L"\\";
			if (s_strComSDKDir.IsEmpty())
				s_strComSDKDir = s_strComDir + L"complus\\sdk\\";
			else if (s_strComSDKDir[s_strComSDKDir.GetLength()-1] == L'/')
				s_strComSDKDir = s_strComSDKDir.Left(s_strComSDKDir.GetLength()-1) + L"\\";
			else if (s_strComSDKDir[s_strComSDKDir.GetLength()-1] != L'\\')
				s_strComSDKDir += L"\\";
			RegCloseKey( hSectionKey );
		}

		// now need to run through some fancy footwork to get the version of COM+ the IDE is running
		wchar_t szRuntimeVer[MAX_PATH * 2];
		wchar_t szSys[MAX_PATH];

		szRuntimeVer[0] = L'\0';
		szSys[0] = L'\0';
		if (GetSystemDirectoryW(szSys, MAX_PATH))
		{
			size_t iLen = wcslen(szSys);
			if (iLen > 0 && szSys[iLen - 1] != L'\\')
			wcscat(szSys, L"\\");
			wcscat(szSys, L"mscoree.dll");
			HMODULE hModEE = LoadLibraryExW(szSys, NULL, 0);
			if (hModEE)
			{
				LPFNGETRUNTIMEVER pfnGetRuntimeVer = (LPFNGETRUNTIMEVER)GetProcAddress(hModEE, "GetCORSystemDirectory");
				if (pfnGetRuntimeVer)
				{
					HRESULT hrT = E_FAIL;
					WCHAR wszCORSysDir[MAX_PATH];
					wszCORSysDir[0] = L'\0';
					DWORD dwChars;
					hrT = (pfnGetRuntimeVer)(wszCORSysDir, MAX_PATH, &dwChars);
					if (SUCCEEDED(hrT) && wszCORSysDir[0])
					{
						WCHAR *pszBackslash = wcsrchr(wszCORSysDir, L'\\');
						if (pszBackslash)
						{
							if (pszBackslash[1] == L'\0' && pszBackslash != wszCORSysDir) //trailing backslash, go back one more
							{
								*pszBackslash = L'\0';
								pszBackslash = wcsrchr(wszCORSysDir, L'\\');  
							}

							WCHAR *pszVerString = pszBackslash ? pszBackslash + 1 : NULL;
							s_strComVer = pszVerString;
						}
					}
				}
				// FreeLibrary(hModEE);	// no need to free this -- we want it around until the IDE shuts down...
			}
		}
	}

	switch (idMacro)
	{
		case IDMACRO_COMDIR:
			strMacroValue = s_strComDir;
			break;
		case IDMACRO_COMSDKDIR:
			strMacroValue = s_strComSDKDir;
			break;
		case IDMACRO_COMVER:
			strMacroValue = s_strComVer;
			break;
		default:
			VSASSERT(FALSE, "Hey, unknown location macro!");
			strMacroValue.Empty();
			break;
	}
}

void CProjectEvaluator::GetBuildEngine(IVCPropertyContainer* pPropContainer, CComPtr<IVCBuildEngine>& spBuildEngine)
{
	if (spBuildEngine != NULL)
		return;	// already handled

	CComPtr<IVCPropertyContainer> spActualContainer;
	CVCProjectEngine::ResolvePropertyContainer(pPropContainer, &spActualContainer);
	CComQIPtr<IVCBuildableItem> spBldableItem = spActualContainer;
	if (spBldableItem == NULL)
		return;

	spBldableItem->get_ExistingBuildEngine(&spBuildEngine);
}

#define CCH_ALLOC_EXPAND	2048
class CCounter
{
public:
	int *m_pn;
	long *m_pID;
	CCounter( int *pn, long* pID ){ m_pn = pn; (*m_pn) = (*m_pn)+1; m_pID = pID; }
	~CCounter(){ (*m_pn) = (*m_pn)-1; if ((*m_pn) == 0) (*m_pID) = 0; }
	void SetMacroID(long id) { if ((*m_pn) == 1) (*m_pID) = id; }
};
    	

HRESULT CProjectEvaluator::ExpandMacros(BSTR* pbstrOut, BSTR bstrIn, IVCPropertyContainer* pPropContainer, BOOL bNoEnvVars, 
	long idProp /* = 0 */)
{
    	// First Check expansion depth
	if( m_nDepth > 32 )
	{
		CComQIPtr<IVCBuildEngine> spBuildEngine;
		GetBuildEngine(pPropContainer, spBuildEngine);
		CComQIPtr<IVCBuildEngineImpl> spBuildEngineImpl = spBuildEngine;
		if (spBuildEngineImpl)
		{
			CComPtr<IVCBuildErrorContext> spEC;
			spBuildEngineImpl->get_ErrorContext(&spEC);
			if (spEC)
			{
				CStringW strErr, strMacroName;
				if (m_nLastMacroID)
					MapMacroIdToName(m_nLastMacroID, strMacroName);
				strErr.Format(IDS_ERR_MACRO_EXPANSION_PRJ0030, strMacroName);
				CVCProjectEngine::AddProjectError(spEC, strErr, L"PRJ0030", pPropContainer);
			}
		}
		return E_FAIL;
	}

	CCounter cnt(&m_nDepth, &m_nLastMacroID);
    
	// just test the usage
	BOOL fJustTestUsage = pbstrOut == NULL;

	if (bstrIn == NULL)
	{
		if (pbstrOut != NULL)
		{
			CComBSTR bstrBlank = L"";
			*pbstrOut = bstrBlank.Detach();
		}
		return S_OK;
	}

	const wchar_t * pchSpec = bstrIn;
	int cchOut = 0;
	int cchAlloc = 0;

	wchar_t pchOutBuffer[2048];
	wchar_t *pchOut = NULL;
	wchar_t *pszAllocatedPtr = NULL;

	if (!fJustTestUsage)
	{
		cchAlloc = cchOut = CCH_ALLOC_EXPAND;
		pchOut = pchOutBuffer;
		*pchOut = L'\0';
		cchOut -= sizeof(wchar_t);
	}


	CStringW strMacroValue;
	// copy everything verbatim unless we come across a macro
	while (*pchSpec != L'\0')
	{
		// macro?
		const wchar_t * pch = pchSpec;
		while (!(*pch == L'$' && *(pch + 1) == L'(') && *pch != L'\0')
			pch++;

		int cchReq = 0; 
		const wchar_t * pchBlk = (const wchar_t *)NULL;

		// do block append?
		if (pch != pchSpec)
		{
			// yes
			cchReq = (int)(pch - pchSpec);

			// get block start
			pchBlk = pchSpec;

			// advance
			pchSpec = pch;
		}
		else
		{

			UINT idMacro; int cchMacro;

			// macro-name exact match?
			// skip '$('
			pch += 2;

			if (MapMacroNameToId(pch, cchMacro, idMacro) &&
				*(pch + cchMacro) == L')')
			{
				cnt.SetMacroID(idMacro);
				// advance and skip macro
				pchSpec = pch + cchMacro;

				// skip trailing ')'
				pchSpec++;

				// verify that we're not trying to recurse into a property to evaluate itself...
				HRESULT hr = ValidateMacro(idMacro, idProp);
				RETURN_ON_FAIL(hr);

				// get the macro value
				if ( (!fJustTestUsage) && GetMacroValue(idMacro, strMacroValue, pPropContainer))
				{
					// get the char. block
					cchReq = strMacroValue.GetLength();
					pchBlk = (const wchar_t *)strMacroValue;
				}
			}
			else if( !bNoEnvVars )
			{
				CStringW strMacroBuf;
				wchar_t* pchMacro;
				const wchar_t *pchStart;

				// save the start
				pchStart = pch;

				// Find last char of Macro
				while (!(*pch == L')') && *pch != L'\0' )
					pch++;

				if( *pch == L'\0' ){
					// stand alone "$(" with no trailing ")"
					strMacroValue = "$(";
					pchBlk = (const wchar_t *)strMacroValue;
					cchReq = strMacroValue.GetLength();
					pchSpec = pchStart;
				} else {

					cchMacro = (int)(pch - pchStart);
					pchMacro = strMacroBuf.GetBuffer(cchMacro+1);

					wcsncpy( pchMacro, pchStart, cchMacro );
					pchMacro[cchMacro] = L'\0';
	
					int nSuccess = GetEnvironmentVariableW( pchMacro, strMacroValue.GetBuffer(2048), 2047 );
					if( nSuccess > 2047 )
					{
						GetEnvironmentVariableW( pchMacro, strMacroValue.GetBuffer(nSuccess+1), nSuccess );
					}
					strMacroValue.ReleaseBuffer();
					strMacroBuf.ReleaseBuffer();
					
					if (nSuccess)
					{
						pchBlk = (const wchar_t *)strMacroValue;
						cchReq = strMacroValue.GetLength();
					}
					else {
						CComQIPtr<IVCBuildEngine> spBuildEngine;
						GetBuildEngine(pPropContainer, spBuildEngine);
						if (spBuildEngine)
						{
							CStringW strCmd;
							strCmd.Format(IDS_WRN_ENVVAR, strMacroBuf);		
							CComBSTR bstrCmd = strCmd;
							spBuildEngine->LogTrace(eLogCommand, bstrCmd);
							CComBSTR bstrMacro = strMacroBuf;
							spBuildEngine->AddMissingEnvironmentVariable(bstrMacro);
						}
						strMacroValue.Empty();
					}
	
					if( *pch == L')' )
						pch++;

					// advance and skip macro
					pchSpec = pch;
				}
			}
			else {
				strMacroValue = L"$(";
				pchBlk = (const wchar_t *)strMacroValue;
				cchReq = strMacroValue.GetLength();
				pchSpec = pch;
			}
		} 

		// not just testing usage and chars. req'd and char blk to copy?
		if (!fJustTestUsage && cchReq && pchBlk)
		{
			if (cchReq > cchOut)
			{
				int cchIncAlloc = max(CCH_ALLOC_EXPAND, cchReq);

				cchAlloc += cchIncAlloc;
				cchOut += cchIncAlloc;

				wchar_t *pchToDelete = pszAllocatedPtr;
				pszAllocatedPtr = (wchar_t *)malloc( cchAlloc * sizeof(wchar_t));
				if( pszAllocatedPtr )
				{
					wcscpy(pszAllocatedPtr, pchOut);
				}
				pchOut = pszAllocatedPtr;
				if( pchToDelete )
					free(pchToDelete);
			}
						
			// copy block
			wcsncat(pchOut, pchBlk, cchReq);
			cchOut -= cchReq;
		}
	}

	// copy into BSTR
	if (!fJustTestUsage)
	{
		CComBSTR bstrOut = pchOut;
		*pbstrOut = bstrOut.Detach();

		// free up our local buffer
		if(pszAllocatedPtr)
			free(pszAllocatedPtr);
	}

	return S_OK;
}

HRESULT CProjectEvaluator::ValidateMacro(UINT idMacro, long idProp)
{
	if (idProp == 0)
		return S_OK;

	switch (idMacro)
	{
	case IDMACRO_INTDIR:
		if (idProp == VCCFGID_IntermediateDirectory)
		{
			VSASSERT(FALSE, "Trying to use $(IntDir) to resolve IntermediateDirectory!");
			return CVCProjectEngine::DoSetErrorInfo(E_INVALIDARG, IDS_ERR_RECURSE_INTDIR);	// recursion!
		}
		break;
	case IDMACRO_OUTDIR:
		if (idProp == VCCFGID_OutputDirectory)
		{
			VSASSERT(FALSE, "Trying to use $(OutDir) to resolve OutputDirectory!");
			return CVCProjectEngine::DoSetErrorInfo(E_INVALIDARG, IDS_ERR_RECURSE_OUTDIR);	// recursion!
		}
		break;
	case IDMACRO_TARGDIR:
	case IDMACRO_TARGFILE:
	case IDMACRO_TARGBASE:
		if (idProp == VCCFGID_PrimaryOutput)
		{
			VSASSERT(FALSE, "Trying to use a target macro to resolve PrimaryOutput!");
			return CVCProjectEngine::DoSetErrorInfo(E_INVALIDARG, IDS_ERR_RECURSE_TARG);	// recursion!
		}
		break;
	case IDMACRO_INPTDIR:
	case IDMACRO_INPTFILE:
	case IDMACRO_INPTBASE:
	case IDMACRO_INPTEXT:
		{
			switch (idProp)
			{
			case VCFILEID_Name:
			case VCFILEID_FullPath:
			case VCFILEID_RelativePath:
				VSASSERT(FALSE, "Trying to use an input macro to resolve something about a file!");
				return CVCProjectEngine::DoSetErrorInfo(E_INVALIDARG, IDS_ERR_RECURSE_INPUT);	// recursion!
				break;
			// default is everything is OK
			}
		}
		break;
	case IDMACRO_CFGNAME:
		if (idProp == VCFCFGID_Name)
		{
			VSASSERT(FALSE, "Trying to use the config name to resolve itself!");
				return CVCProjectEngine::DoSetErrorInfo(E_INVALIDARG, IDS_ERR_RECURSE_CFG);	// recursion!
		}
		break;
	case IDMACRO_PROJDIR:
	case IDMACRO_PROJFILE:
	case IDMACRO_PROJBASE:
	case IDMACRO_PROJEXT:
	case IDMACRO_PROJPATH:
		{
			switch(idProp)
			{
			case VCPROJID_Name:
			case VCPROJID_ProjectFile:
			case VCPROJID_ProjectDirectory:
				VSASSERT(FALSE, "Trying to use a project macro to resolve something about a project!");
				return CVCProjectEngine::DoSetErrorInfo(E_INVALIDARG, IDS_ERR_RECURSE_PROJ);	// recursion!
				break;
			// default is everything is OK
			}
		}
	case IDMACRO_REMOTEMACH:
		if (idProp == VCDSID_RemoteMachine)
		{
			VSASSERT(FALSE, "Trying to use the remote machine name to resolve itself!");
			return CVCProjectEngine::DoSetErrorInfo(E_INVALIDARG, IDS_ERR_RECURSE_REMOTE);	// recursion!
		}
		break;
	case IDMACRO_PLATNAME:
		if (idProp == VCPLATID_Name)
		{
			VSASSERT(FALSE, "Trying to use platform name to resolve itself!");
			return CVCProjectEngine::DoSetErrorInfo(E_INVALIDARG, IDS_ERR_RECURSE_PLATFORM);	// recursion!
		}
	// default is everything is OK
	}

	return S_OK;
}

BOOL CProjectEvaluator::NoInheritOnMultiProp(BSTR bstrVal, LPCOLESTR szSeparator, BSTR* pbstrValue)
{
	CStringW strVal = bstrVal;
	if (strVal.IsEmpty())
		return FALSE;

	// NoInherit takes precedence over Inherit
	int nFindElem = strVal.Find(L"$(");
	int nMaxLen = strVal.GetLength();
	const wchar_t* szVal = strVal;
	bool bNoInherit = false;
	while (nFindElem >= 0 && !bNoInherit)
	{
		if (_wcsnicmp(szVal+nFindElem, L"$(NoInherit)", 12) == 0)	// don't inherit
		{
			bNoInherit = true;
			break;
		}
		nFindElem++;
		if (nFindElem >= nMaxLen)	// definitely not found
			break;
		nFindElem = strVal.Find(L"$(", nFindElem);
	}
	if (!bNoInherit)
		return FALSE;	// we're going to do inheritance

	// Now let's remove all the $(Inherit) and $(NoInherit) tags
	CStringW strSeparator = szSeparator;
	nFindElem = strVal.Find(L"$(");
	int nLastStart = 0;
	CStringW strTmp;
	bool bNeedSeparator = false;
	while (nFindElem >= 0 && nLastStart < nMaxLen)
	{
		int nReplaceLen = 0;
		if (_wcsnicmp(szVal+nFindElem, L"$(Inherit)", 10) == 0)
			nReplaceLen = 10;
		else if (_wcsnicmp(szVal+nFindElem, L"$(NoInherit)", 12) == 0)
			nReplaceLen = 12;

		if (nReplaceLen > 0)
		{
			if (nLastStart == 0)
			{
				if (nFindElem > 0)
				{
					strTmp = strVal.Left(nFindElem-1);
					bNeedSeparator = true;
				}
			}
			else
			{
				if (bNeedSeparator && !strSeparator.IsEmpty())
					strTmp += strSeparator.GetAt(0);
				bNeedSeparator = true;
				if (nFindElem+nReplaceLen+1 == nMaxLen)
					strTmp += strVal.Right(nMaxLen-nFindElem-nReplaceLen);
				else
					// if another macro follows immediately, append nothing
					if( nLastStart != nFindElem )
						strTmp += strVal.Mid(nLastStart, nFindElem-1-nLastStart);	// pull out the separator, too
			}
			nLastStart = nFindElem + nReplaceLen + 1;
		}
		nFindElem++;
		if (nFindElem >= nMaxLen)	// definitely no more found
			break;
		nFindElem = strVal.Find(L"$(", nFindElem);
	}
	if (nLastStart < nMaxLen)
	{
		if (!strSeparator.IsEmpty())
			strTmp += strSeparator.GetAt(0);
		strTmp += strVal.Right(nMaxLen-nLastStart);
	}

	CComBSTR bstrValue = strTmp;
	*pbstrValue = bstrValue.Detach();
	return TRUE;
}

HRESULT CProjectEvaluator::MakeMultiPropString(CComBSTR& bstrLocal, CComBSTR& bstrParent, LPCOLESTR szSeparator, BSTR* pbstrValue)
{
	CStringW strParent = bstrParent;
	bool bDidInsertAtLeastOnce = false;
	CStringW strLocal = bstrLocal;
	int nFindElem = strLocal.Find(L"$(");
	int nMaxLen = strLocal.GetLength();
	const wchar_t* szLocal = strLocal;
	CStringW strFull;
	CStringW strSeparator = szSeparator;
	// See if we have a non-default location for Inherit
	int nLastStart = 0;
	int nReplaceLen = 10;
	bool bNeedSeparator1 = !strParent.IsEmpty();
	bool bNeedSeparator2 = false;
	while (nFindElem >= 0 && nLastStart < nMaxLen)
	{
		if (_wcsnicmp(szLocal+nFindElem, L"$(Inherit)", 10) == 0)
		{
			if (nLastStart == 0)
			{
				if (nFindElem > 0)
					strFull = strLocal.Left(nFindElem-1);
			}
			else
			{
				if (bNeedSeparator1 && !strSeparator.IsEmpty())
					strFull += strSeparator.GetAt(0);
				if (nFindElem+nReplaceLen+1 == nMaxLen)
					strFull += strLocal.Right(nMaxLen-nFindElem-nReplaceLen);
				else
					strFull += strLocal.Mid(nLastStart, nFindElem-1-nLastStart);	// pull out the separator, too
			}
			nLastStart = nFindElem + nReplaceLen + 1;
			if (bNeedSeparator2 && !strSeparator.IsEmpty())
				strFull +=  strSeparator.GetAt(0);
			bNeedSeparator2 = true;
			strFull += strParent;
		}
		nFindElem++;
		if (nFindElem >= nMaxLen)	// definitely not found
			break;
		nFindElem = strLocal.Find(L"$(", nFindElem);
	}

	if (strFull.IsEmpty())
	{
		strFull = strLocal;
		if (bNeedSeparator1 && !strSeparator.IsEmpty())
			strFull +=  strSeparator.GetAt(0);
		strFull += strParent;
	}
	else if (nLastStart < nMaxLen)
	{
		if (!strSeparator.IsEmpty())
			strFull += strSeparator.GetAt(0);
		strFull += strLocal.Right(nMaxLen-nLastStart);
	}
	CComBSTR bstrFull = strFull;
	*pbstrValue = bstrFull.Detach();

	return S_OK;
}

HRESULT CProjectEvaluator::CollapseMultiples(BSTR bstrIn, LPCOLESTR szSeparator, VARIANT_BOOL bCaseSensitive, BSTR* pbstrOut)
{
	CHECK_POINTER_NULL(pbstrOut);
	CStringW strIn = bstrIn;
	CStringW strOut, strOutNoCase;
	CStringW strItem, strItemNoCase;
	CStringW strSeparator = szSeparator;
	bool bFirstElem = true;

	int nNextIdx = 0;
	int nMaxIdx = strIn.GetLength();
	int nFoundIdx = 0;
	while (nNextIdx >= 0)
	{
		nNextIdx = g_StaticBuildEngine.GetNextItem(strIn, nNextIdx, nMaxIdx, strSeparator, strItem);
		if (!bCaseSensitive)
		{
			strItemNoCase = strItem;
			strItemNoCase.MakeLower();
			nFoundIdx = DoFindItem(strOutNoCase, strItemNoCase, strSeparator, 0);
		}
		else
			nFoundIdx = DoFindItem(strOut, strItem, strSeparator, 0);
		if (nFoundIdx >= 0)		// already there
			continue;
		if (!bFirstElem)
		{
			if (!strSeparator.IsEmpty())
			{
				strOut += strSeparator.GetAt(0);
				if (!bCaseSensitive)
					strOutNoCase += strSeparator.GetAt(0);
			}
		}
		else
			bFirstElem = false;
		strOut += strItem;
		if (!bCaseSensitive)
			strOutNoCase += strItemNoCase;
	}

	if (strOut.IsEmpty())
	{
		CComBSTR bstrBlank = L"";
		*pbstrOut = bstrBlank.Detach();
	}
	else
		*pbstrOut = strOut.AllocSysString();

	return S_OK;
}

int CProjectEvaluator::DoFindItem(CStringW& rstrItems, CStringW& rstrFindItem, CStringW& strSeparator, int nStart)
{
	if (rstrFindItem.IsEmpty())
		return -1;	// never find a NULL item

	nStart = rstrItems.Find(rstrFindItem, nStart);
	if (nStart < 0)
		return nStart;

	int nItemLen = rstrItems.GetLength();
	int nLen = rstrFindItem.GetLength();

	if (nStart > 0 && nStart + nLen <= nItemLen && strSeparator.Find(rstrItems[nStart-1]) < 0)
	{
		if (nStart + nLen == nItemLen)
			return -1;	// partial match to the end of the string
		else
			return DoFindItem(rstrItems, rstrFindItem, strSeparator, nStart+nLen);
	}
	else if (nItemLen > nStart + nLen && strSeparator.Find(rstrItems[nStart+nLen]) < 0)
		return DoFindItem(rstrItems, rstrFindItem, strSeparator, nStart+nLen);

	return nStart;
}

