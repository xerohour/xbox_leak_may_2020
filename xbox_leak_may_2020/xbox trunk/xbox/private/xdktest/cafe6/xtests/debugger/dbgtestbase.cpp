///////////////////////////////////////////////////////////////////////////////
// DbgTestBase.cpp
//
//	Created by:	MichMa
//	Date:		3/30/98
//
//	Description: Definition of base class for all debugger tests.

#include "stdafx.h"
#include "DbgTestBase.h"
#include "guitarg.h"

// emmang@xbox utility functions
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
// GetHkeyFromStr
//
// utility function to map the prefix of a string (either "hklm"
// or "hkcu") to an actual HKEY value
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
HKEY GetHkeyFromStr(CString cstr)
{
   cstr.MakeLower();
   if (cstr.Left(4) == "hklm")
	   return HKEY_LOCAL_MACHINE;
   else if (cstr.Left(4) == "hkcr")
	   return HKEY_CLASSES_ROOT;
   else
	   return HKEY_CURRENT_USER;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
// GetStrFromRegistry
//
// utility function; gets a string value from the registry; if the key
// is not present, return the supplied default instead
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
CString GetStrFromRegistry(CString cstrSubKey, CString cstrDefault)
{
	UCHAR szBuf[2048];
	LONG retval;
	ULONG lBufSize = 4096;
	CString cstrTemp = cstrSubKey.Mid(5);

	HKEY hKey;
	cstrTemp = cstrSubKey.Left(cstrSubKey.ReverseFind('\\'));
	cstrTemp = cstrTemp.Mid(5);
	retval = RegOpenKey(GetHkeyFromStr(cstrSubKey), cstrTemp, &hKey);
	if (retval != ERROR_SUCCESS)
		return cstrDefault;

	cstrTemp = cstrSubKey.Mid(cstrSubKey.ReverseFind('\\')+1);
	retval = RegQueryValueEx(hKey, cstrTemp, NULL, NULL, szBuf, &lBufSize);
	if ((retval != ERROR_SUCCESS) || !lBufSize)
	{
		RegCloseKey(hKey);
		return cstrDefault;
	}
	return szBuf;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
// GetProgramFilesDir
//
// utility function, returns the Program Files directory
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
CString GetProgramFilesDir()
{
	return GetStrFromRegistry(
		"HKLM\\Software\\Microsoft\\Windows\\CurrentVersion\\ProgramFilesDir",
		"C:\\Program Files");
}


//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
// GetVisualCDir
//
// utility function, returns VC directory
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
CString GetVisualCDir()
{
	return GetStrFromRegistry(
		"HKLM\\Software\\Microsoft\\VisualStudio\\6.0\\Products\\Microsoft Visual C++\\ProductDir",
		GetProgramFilesDir() + "\\Microsoft Visual Studio\\VC98");
}


//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
// GetVisualStudioCommonDir
//
// utility function, returns Visual Studio's common directory
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
CString GetVisualStudioCommonDir()
{
	return GetStrFromRegistry(
		"HKLM\\Software\\Microsoft\\VisualStudio\\6.0\\Setup\\VsCommonDir",
		GetProgramFilesDir() + "\\Microsoft Visual Studio\\Common");
}


//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
// GetXboxSdkDir
//
// utility function, returns the Xbox SDK's base directory
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
CString GetXboxSdkDir()
{
	TCHAR szSDKDir[1024];
	szSDKDir[0] = 0;
	GetEnvironmentVariable("XDK", szSDKDir, 1024);
	return CString(szSDKDir);
}


IMPLEMENT_DYNAMIC( CSubTestException, CException );

IMPLEMENT_DYNAMIC(CDbgTestBase, CTest)

#define GetSubSuite() ((CIDESubSuite*)m_pSubSuite)

CDbgTestBase::CDbgTestBase(/*int metricID,*/ CIDESubSuite* pSubSuite, LPCSTR szName, int nExpectedCompares, LPCSTR szListFilename /*= NULL*/)
: CTest(/*metricID,*/ pSubSuite, szName, nExpectedCompares, szListFilename)
{
}

#define NOSPACES

BOOL CDbgTestBase::SetXboxProject(LPCSTR projName, PROJECT_FLAGS flags /* fDefault */, LPCSTR cfgName /*NULL*/)
{
	const char* const THIS_FUNCTION = "SetXboxProject()";
	CString projPathAndName = projName;
	int iName = projPathAndName.ReverseFind('\\');
	if (iName < 0)
	{
		m_strProjectDir = FullPath("src");
		m_strProjectName = projName;
	}
	else
	{
		m_strProjectDir = (CString)FullPath("src") + "\\" + projPathAndName.Left(iName);
		m_strProjectName = projPathAndName.Mid(iName + 1);
	}

#ifdef NOSPACES
	m_strProjectName.Replace(" ", "_");
#endif

	// copy stuff to new directory and do work there
	CreateDirectory(TMP_PROJ_DIR, NULL);
	CreateDirectory(TMP_PROJ_DIR "\\" + m_strProjectName, NULL);
	// just in case dir was already there, clean it up
	KillFiles(TMP_PROJ_DIR "\\" + m_strProjectName, "*.*");
	CopyProjectSources(m_strProjectDir + "\\", TMP_PROJ_DIR "\\" + m_strProjectName + "\\");
	m_strProjectDir = TMP_PROJ_DIR "\\" + m_strProjectName;

	CIDESubSuite* pSubSuite=GetSubSuite();
	ASSERT(pSubSuite->IsKindOf(RUNTIME_CLASS(CIDESubSuite))); //our tests are IDE tests only
	// build the path, lib, and include environment strings.

	CString cstrPathEnv, cstrLibEnv, cstrIncludeEnv;
	pSubSuite->GetIDE()->GetEnvironmentVar(settingPathEnv, cstrPathEnv);
	cstrPathEnv = settingPathEnv + CString("=") +
		GetXboxSdkDir() + "\\Xbox\\Bin\\VC7;" +
		GetXboxSdkDir() + "\\Xbox\\Bin;" +
		GetVisualStudioCommonDir() + "\\MsDev98\\Bin;" +
		GetVisualCDir() + "\\Bin;" +
		GetVisualStudioCommonDir() + "\\Tools;" +
		GetVisualStudioCommonDir() + "\\Tools\\WINNT;" +
		cstrPathEnv;
	cstrLibEnv = "LIB=" + GetXboxSdkDir() + "\\Xbox\\Lib";
	cstrIncludeEnv = "INCLUDE=" + GetXboxSdkDir() + "\\Xbox\\Include";

	// get windows and system dir and append to path environment string.
	CString cstrWindowsDir, cstrSystemDir;
	// each dir is prepended with a semi-colon as a separator. 
	char chDirBuf[MAX_PATH] = ";";
 	GetWindowsDirectory(&chDirBuf[1], MAX_PATH);	// &chDirBuf[0] = ';'
	cstrWindowsDir = chDirBuf;
	GetSystemDirectory(&chDirBuf[1], MAX_PATH);		// &chDirBuf[0] = ';'
	cstrSystemDir = chDirBuf;
	cstrPathEnv += cstrWindowsDir + cstrSystemDir;

	// nmake's environment block.
	char chNmakeEnv[1024];
	// pointer to navigate through block.
	char *pchNmakeEnv = chNmakeEnv;

	// put the path, lib, and include environment strings into the block.
	// each string is terminated by null.
	strcpy(pchNmakeEnv, cstrPathEnv);
	pchNmakeEnv += strlen(pchNmakeEnv) + 1;
	strcpy(pchNmakeEnv, cstrLibEnv);
	pchNmakeEnv += strlen(pchNmakeEnv) + 1;
	strcpy(pchNmakeEnv, cstrIncludeEnv);
	// block is terminated by additional null.
	pchNmakeEnv[strlen(pchNmakeEnv) + 1] = 0;
 
	// to find nmake.exe in the toolset path environment, 
	// we'll use cstrPathFrag to search	for and extract directories
	// from cstrPathEnv	(minus first 5 characters which are "PATH=").
	CString cstrPathFrag = cstrPathEnv.Mid(5);
	// stores dir extracted from cstrPathFrag.
	CString cstrDir;
	// stores index of next semi-colon (dir separator) in cstrPathEnv.
	int indexSemi;
	// handle to nmake.exe returned by FindFirstFile().
	HANDLE hNmake;
	// FindFirstFile param.
	WIN32_FIND_DATA fdFindData;

	while(1)
		
		{
		// find the next semi-colon which terminates the next dir to search.
		indexSemi = cstrPathFrag.Find(';');

		// if a semi-colon was found, extract the dir it terminates. otherwise
		// we are at the final dir in the path environment. 
		if(indexSemi != -1)
			cstrDir = cstrPathFrag.Left(indexSemi);
		else
			cstrDir = cstrPathFrag;
			
		// only want to search dirs at least 3 chars long. for example:
		// path  = C:\;
		// index = 0123
		if(cstrDir.GetLength() >= 3)
			hNmake = FindFirstFile(cstrDir + "\\nmake.exe", &fdFindData);
  		
		// stop searching if nmake was found in cstrPathFrag.
		if(hNmake != INVALID_HANDLE_VALUE)
			break;

		// if no semi-colon was found or it was the last char in the path, 
		// then we just searched the final dir in the environment string, 
		// but still haven't found nmake. 
	 	if((indexSemi == -1) || (cstrPathFrag.GetLength() == (indexSemi + 1)))
			{
			m_pLog->RecordInfo("ERROR in %s: could not find nmake.exe in path environment "
							   "specified by CAFE.", THIS_FUNCTION);
			return FALSE;
			}
		else
			// advance to the start of the next dir after the semi-colon.
			cstrPathFrag = cstrPathFrag.Mid(indexSemi + 1);
		}


	// determine the configuration switch that we need to pass to nmake.
	// also set platform member value for easy access by tests.
	if(cfgName==NULL)
		{
            cfgName = "xbox";
            m_platform = PLATFORM_XBOX;
		}

	// project name and configuration switch we will pass to nmake.
	// we enclose the project name in quotes to support names with spaces.
	CString cstrProjAndConfig = (CString)"\"" + m_strProjectName + ".mak\" CFG=" + cfgName;
	if( UIWB.GetPcodeSwitch() )
		cstrProjAndConfig += " PCODE=yes";
	// CreateProcess() params.
   	STARTUPINFO siStartInfo;
	PROCESS_INFORMATION piProcInfo;
	// this is the minium initialization of STARTUPINFO required for CreateProcess() to work.
	memset(&siStartInfo, 0, sizeof(STARTUPINFO));
	siStartInfo.cb = sizeof(STARTUPINFO);
	siStartInfo.wShowWindow = SW_SHOWDEFAULT;
	// fully qualified nmake.exe cmd line (cstrDir is directory we found nmake in earlier).
	char chNmakeCmdLine[MAX_PATH];
	strcpy(chNmakeCmdLine, cstrDir + "\\nmake.exe /a /f " + cstrProjAndConfig);

	// before spawning nmake make sure Debugger is not running, so vc50.pdb is released
	if(dbg.GetDebugState() != NotDebugging)
		dbg.StopDebugging();

	// Allocate console to be able to see how the project is built.  If we don't allocate it the window close
	// immediately after build process has finished. *dklem
	WaitStepInstructions("Allocating nmake console");
	AllocConsole();
	SetConsoleTitle(chNmakeCmdLine);

	// spawn nmake in the project dir.
	BOOL bRet = CreateProcess(NULL, chNmakeCmdLine, NULL, NULL, TRUE, NORMAL_PRIORITY_CLASS, chNmakeEnv,
							  m_strProjectDir, &siStartInfo, &piProcInfo);
	
	// verify that nmake was successfully spawned.						  
	if(!bRet)
		{
		m_pLog->RecordInfo("ERROR in %s: nmake.exe found but could not be spawned.", THIS_FUNCTION);
		return FALSE;
		}

	// nmake's exit code returned by GetExitCodeProcess().
	DWORD dwNmakeExitCode;

	// wait up to 60 minutes for nmake to terminate.
	for(int i = 0; i < 3600; i++)
		
		{
		Sleep(1000);
		// get nmake's exit code (if it has finished).
		GetExitCodeProcess(piProcInfo.hProcess, &dwNmakeExitCode);
		
		// keep sleeping if nmake is still running.
		if(dwNmakeExitCode != STILL_ACTIVE)
			break;
		}

	// Say good bye to the console
	WaitStepInstructions("Destroying nmake console");
	FreeConsole();

	// verify that nmake finished within the allotted time.
	if(i == 3600)
		{
		m_pLog->RecordInfo("ERROR in %s: after 60 minutes nmake is still running.", THIS_FUNCTION);
		return FALSE;
		}

	// verify that nmake was successful in building the executable.
	if(dwNmakeExitCode != 0)
		{
		m_pLog->RecordInfo("ERROR in %s: nmake.exe returned error code %d.", THIS_FUNCTION, dwNmakeExitCode);
		return FALSE;
		}

	// wait until nmake's window has gone away.
	if(!MST.WFndWndWait(chNmakeCmdLine, FW_NOEXIST, 5))
		{
		m_pLog->RecordInfo("ERROR in %s: after 5 seconds nmake's window still exists.", THIS_FUNCTION);
		return FALSE;
		}

	// stop here if we only want to build.
	if(flags & fBuildOnly)
		return TRUE;

	// project executable
	CString cstrProjectExe = m_strProjectName.Right(m_strProjectName.GetLength() -
							 m_strProjectName.ReverseFind('\\') - 1) + ".xbe";

	// verify that file exists
	FILE *f = fopen(m_strProjectDir + "\\" + cstrProjectExe, "r");
	if (!f)
	{
		m_pLog->RecordInfo("ERROR in %s: Target file not built: \"%s\".", THIS_FUNCTION, 
				m_strProjectDir + "\\" + cstrProjectExe);
		return FALSE;
	}
	fclose(f);

	// we need to make sure the debugger isn't running on a previous project
	// or COWorkSpace::CloseAllWindows will fail.
	if(dbg.GetDebugState() != NotDebugging)
		dbg.StopDebugging();

	// we need to close all windows before opening the exe or COProject::Open will fail.
 	COWorkSpace ws;
	ws.CloseAllWindows();

	// open the executable.	
	if(prj.Open(m_strProjectDir + "\\" + cstrProjectExe) != ERROR_SUCCESS)
	{
		m_pLog->RecordInfo("ERROR in %s: could not open \"%s\".", THIS_FUNCTION, 
				m_strProjectDir + "\\" + cstrProjectExe);
		return FALSE;
	}

	// set up connection
	if (!XboxSetTarget(REMOTE_XBOX_IP))
	{
		m_pLog->RecordInfo("ERROR in %s: could not set target machine to \"%s\".", THIS_FUNCTION, 
				REMOTE_XBOX_IP);
		return FALSE;
	}
	if (!(XboxCopyToTarget(m_strProjectDir + "\\" + cstrProjectExe)))
	{
		m_pLog->RecordInfo("ERROR in %s: could not copy xbe to target machine \"%s\".", THIS_FUNCTION, 
				REMOTE_XBOX_IP);
		return FALSE;
	}
	if (prj.SetRemotePath(REMOTE_XBOX_FILE) != ERROR_SUCCESS)
	{
		m_pLog->RecordInfo("ERROR in %s: could not set remote path to \"%s\".", THIS_FUNCTION, 
				REMOTE_XBOX_FILE);
		return FALSE;
	}
	prj.SetLocateOtherDLLs(TOGGLE_OFF);
		
	return TRUE;
}


BOOL CDbgTestBase::CreateXboxProjectFromSource(LPCSTR strProjBase, LPCSTR strSrcDir, LPCSTR strSrcFile, LPCSTR strSrcFile2)
{
	// the base name of the localized directories and files we will use.
	if(GetSystem() & SYSTEM_DBCS)
		m_strProjectName = "ƒeƒXƒg" + CString(strProjBase) + " xbe";
	else
		m_strProjectName = CString(strProjBase) + " xbe";

#ifdef NOSPACES
	m_strProjectName.Replace(" ", "_");
#endif

	// the location of the unlocalized sources, under which the projects we will use will be located.
	m_strSrcDir = GetCWD() + strSrcDir;
	// the location of the xbe project, and the localized source(s), that we will use for this test.
	m_strProjectDir = CString(TMP_PROJ_DIR) + "\\" + m_strProjectName;

	CreateDirectory(TMP_PROJ_DIR, NULL);

	// clean-up the exe project from the last run.
	KillFiles(m_strProjectDir + "\\Debug\\", "*.*");
	KillFiles(m_strProjectDir + "\\Release\\", "*.*");
	RemoveDirectory(m_strProjectDir + "\\Debug");
	RemoveDirectory(m_strProjectDir + "\\Release");
	KillFiles(m_strProjectDir + "\\", "*.*");
	RemoveDirectory(m_strProjectDir);

	// set the options for the exe project we are building for this test.
	CXboxGameOptions *pXboxGameOpt = new(CXboxGameOptions);
	pXboxGameOpt->m_strLocation = TMP_PROJ_DIR;
	pXboxGameOpt->m_strName = m_strProjectName;
	
	// create the exe project.
	EXPECT_SUCCESS(prj.New(pXboxGameOpt));
	// create a new localized exe source file in the project dir from the unlocalized exe source file.
	CopyFile(m_strSrcDir + "\\" + strSrcFile, m_strProjectDir + "\\" + m_strProjectName + ".cpp", FALSE);
	// make it writable so it can be cleaned up later.
	SetFileAttributes(m_strProjectDir + "\\" + m_strProjectName + ".cpp", FILE_ATTRIBUTE_NORMAL);
	// add the source to the project.

	if (strSrcFile2)
	{
		CopyFile(m_strSrcDir + "\\" + strSrcFile2, m_strProjectDir + "\\" + strSrcFile2, FALSE);
		SetFileAttributes(m_strProjectDir + "\\" + strSrcFile2, FILE_ATTRIBUTE_NORMAL);
	}

	EXPECT_SUCCESS(prj.AddFiles(m_strProjectName + ".cpp"));
	if (ReturnCode)
	{
		LOG->Comment("prj.AddFiles(\"%s\") failed", m_strProjectName + ".cpp");
		return FALSE;
	}
	
	if (strSrcFile2)
	{
		EXPECT_SUCCESS(prj.AddFiles(strSrcFile2));
		if (ReturnCode)
		{
			LOG->Comment("prj.AddFiles(\"%s\") failed", strSrcFile2);
			return FALSE;
		}
	}

	// set the remote path first, so autocopy won't pop up a query
	EXPECT_SUCCESS(prj.SetRemotePath(REMOTE_XBOX_FILE));
	{
		if (ReturnCode)
			return FALSE;
	}

	// build the project.
	EXPECT_SUCCESS(prj.Build());
	
	if (!XboxSetTarget(REMOTE_XBOX_IP))
		return FALSE;

	// no longer needed due to autocopy
	// if (!XboxCopyToTarget())
	//	return FALSE;

	prj.SetLocateOtherDLLs(TOGGLE_OFF);

	return TRUE;
}


BOOL CDbgTestBase::SetProject(LPCSTR projName, PROJECT_TYPE projType /* PROJECT_EXE */, PROJECT_FLAGS flags /* fDefault */, LPCSTR cfgName /*NULL*/)
{
	if (projType = PROJECT_XBE)
		return SetXboxProject(projName, flags, cfgName);

	const char* const THIS_FUNCTION = "SetProject()";
	CString projPathAndName = projName;
	int iName = projPathAndName.ReverseFind('\\');
	if( iName < 0 )
	{
		m_strProjectDir = FullPath("src");
		m_strProjectName = projName;
	}
	else
	{
		m_strProjectDir = (CString)FullPath("src") + "\\" + projPathAndName.Left(iName);
		m_strProjectName = projPathAndName.Mid(iName + 1);
	}

	KillFiles(m_strProjectDir, "*.opt");
	KillFiles(m_strProjectDir, "*.exe");
	KillFiles(m_strProjectDir, "*.obj");
	KillFiles(m_strProjectDir, "*.ilk");
	KillFiles(m_strProjectDir, "*.ncb");
	KillFiles(m_strProjectDir, "*.plg");

	CIDESubSuite* pSubSuite=GetSubSuite();
	ASSERT(pSubSuite->IsKindOf(RUNTIME_CLASS(CIDESubSuite))); //our tests are IDE tests only
	// build the path, lib, and include environment strings.

	CString cstrPathEnv, cstrLibEnv, cstrIncludeEnv;
	pSubSuite->GetIDE()->GetEnvironmentVar(settingPathEnv, cstrPathEnv);
	cstrPathEnv = settingPathEnv + CString("=") + cstrPathEnv;
	pSubSuite->GetIDE()->GetEnvironmentVar(settingLibEnv, cstrLibEnv);
	cstrLibEnv = settingLibEnv + CString("=") + cstrLibEnv;
	pSubSuite->GetIDE()->GetEnvironmentVar(settingIncludeEnv, cstrIncludeEnv);
	cstrIncludeEnv = settingIncludeEnv + CString("=") + cstrIncludeEnv;

	// get windows and system dir and append to path environment string.
	CString cstrWindowsDir, cstrSystemDir;
	// each dir is prepended with a semi-colon as a separator. 
	char chDirBuf[MAX_PATH] = ";";
 	GetWindowsDirectory(&chDirBuf[1], MAX_PATH);	// &chDirBuf[0] = ';'
	cstrWindowsDir = chDirBuf;
	GetSystemDirectory(&chDirBuf[1], MAX_PATH);		// &chDirBuf[0] = ';'
	cstrSystemDir = chDirBuf;
	cstrPathEnv += cstrWindowsDir + cstrSystemDir;

	// nmake's environment block.
	char chNmakeEnv[1024];
	// pointer to navigate through block.
	char *pchNmakeEnv = chNmakeEnv;

	// put the path, lib, and include environment strings into the block.
	// each string is terminated by null.
	strcpy(pchNmakeEnv, cstrPathEnv);
	pchNmakeEnv += strlen(pchNmakeEnv) + 1;
	strcpy(pchNmakeEnv, cstrLibEnv);
	pchNmakeEnv += strlen(pchNmakeEnv) + 1;
	strcpy(pchNmakeEnv, cstrIncludeEnv);
	// block is terminated by additional null.
	pchNmakeEnv[strlen(pchNmakeEnv) + 1] = 0;
 
	// to find nmake.exe in the toolset path environment, 
	// we'll use cstrPathFrag to search	for and extract directories
	// from cstrPathEnv	(minus first 5 characters which are "PATH=").
	CString cstrPathFrag = cstrPathEnv.Mid(5);
	// stores dir extracted from cstrPathFrag.
	CString cstrDir;
	// stores index of next semi-colon (dir separator) in cstrPathEnv.
	int indexSemi;
	// handle to nmake.exe returned by FindFirstFile().
	HANDLE hNmake;
	// FindFirstFile param.
	WIN32_FIND_DATA fdFindData;

	while(1)
		
		{
		// find the next semi-colon which terminates the next dir to search.
		indexSemi = cstrPathFrag.Find(';');

		// if a semi-colon was found, extract the dir it terminates. otherwise
		// we are at the final dir in the path environment. 
		if(indexSemi != -1)
			cstrDir = cstrPathFrag.Left(indexSemi);
		else
			cstrDir = cstrPathFrag;
			
		// only want to search dirs at least 3 chars long. for example:
		// path  = C:\;
		// index = 0123
		if(cstrDir.GetLength() >= 3)
			hNmake = FindFirstFile(cstrDir + "\\nmake.exe", &fdFindData);
  		
		// stop searching if nmake was found in cstrPathFrag.
		if(hNmake != INVALID_HANDLE_VALUE)
			break;

		// if no semi-colon was found or it was the last char in the path, 
		// then we just searched the final dir in the environment string, 
		// but still haven't found nmake. 
	 	if((indexSemi == -1) || (cstrPathFrag.GetLength() == (indexSemi + 1)))
			{
			m_pLog->RecordInfo("ERROR in %s: could not find nmake.exe in path environment "
							   "specified by CAFE.", THIS_FUNCTION);
			return FALSE;
			}
		else
			// advance to the start of the next dir after the semi-colon.
			cstrPathFrag = cstrPathFrag.Mid(indexSemi + 1);
		}


	// determine the configuration switch that we need to pass to nmake.
	// also set platform member value for easy access by tests.
	if(cfgName==NULL)
	switch(GetUserTargetPlatforms())
		{
        case PLATFORM_WIN32_ALPHA:
            cfgName = "alpha";
            m_platform = PLATFORM_WIN32_ALPHA;
            break;
		case PLATFORM_WIN32_X86:
			cfgName = "x86";
			m_platform = PLATFORM_WIN32_X86;
			break;
		}

	// project name and configuration switch we will pass to nmake.
	// we enclose the project name in quotes to support names with spaces.
	CString cstrProjAndConfig = (CString)"\"" + m_strProjectName + ".mak\" CFG=" + cfgName;
	if( UIWB.GetPcodeSwitch() )
		cstrProjAndConfig += " PCODE=yes";
	// CreateProcess() params.
   	STARTUPINFO siStartInfo;
	PROCESS_INFORMATION piProcInfo;
	// this is the minium initialization of STARTUPINFO required for CreateProcess() to work.
	memset(&siStartInfo, 0, sizeof(STARTUPINFO));
	siStartInfo.cb = sizeof(STARTUPINFO);
	siStartInfo.wShowWindow = SW_SHOWDEFAULT;
	// fully qualified nmake.exe cmd line (cstrDir is directory we found nmake in earlier).
	char chNmakeCmdLine[MAX_PATH];
	strcpy(chNmakeCmdLine, cstrDir + "\\nmake.exe /a /f " + cstrProjAndConfig);

	// before spawning nmake make sure Debugger is not running, so vc50.pdb is released
	if(dbg.GetDebugState() != NotDebugging)
		dbg.StopDebugging();

	// Allocate console to be able to see how the project is built.  If we don't allocate it the window close
	// immediately after build process has finished. *dklem
	WaitStepInstructions("Allocating nmake console");
	AllocConsole();
	SetConsoleTitle(chNmakeCmdLine);

	// spawn nmake in the project dir.
	BOOL bRet = CreateProcess(NULL, chNmakeCmdLine, NULL, NULL, TRUE, NORMAL_PRIORITY_CLASS, chNmakeEnv,
							  m_strProjectDir, &siStartInfo, &piProcInfo);
	
	// verify that nmake was successfully spawned.						  
	if(!bRet)
		{
		m_pLog->RecordInfo("ERROR in %s: nmake.exe found but could not be spawned.", THIS_FUNCTION);
		return FALSE;
		}

	// nmake's exit code returned by GetExitCodeProcess().
	DWORD dwNmakeExitCode;

	// wait up to 60 minutes for nmake to terminate.
	for(int i = 0; i < 3600; i++)
		
		{
		Sleep(1000);
		// get nmake's exit code (if it has finished).
		GetExitCodeProcess(piProcInfo.hProcess, &dwNmakeExitCode);
		
		// keep sleeping if nmake is still running.
		if(dwNmakeExitCode != STILL_ACTIVE)
			break;
		}

	// Say good bye to the console
	WaitStepInstructions("Destroying nmake console");
	FreeConsole();

	// verify that nmake finished within the allotted time.
	if(i == 3600)
		{
		m_pLog->RecordInfo("ERROR in %s: after 60 minutes nmake is still running.", THIS_FUNCTION);
		return FALSE;
		}

	// verify that nmake was successful in building the executable.
	if(dwNmakeExitCode != 0)
		{
		m_pLog->RecordInfo("ERROR in %s: nmake.exe returned error code %d.", THIS_FUNCTION, dwNmakeExitCode);
		return FALSE;
		}

	// wait until nmake's window has gone away.
	if(!MST.WFndWndWait(chNmakeCmdLine, FW_NOEXIST, 5))
		{
		m_pLog->RecordInfo("ERROR in %s: after 5 seconds nmake's window still exists.", THIS_FUNCTION);
		return FALSE;
		}

	// stop here if we only want to build.
	if(flags & fBuildOnly)
		return TRUE;

	// project exe has same base as makefile (i.e. xxx.mak/xxx.exe/xxx.dll)
	// we want to strip out any prepended path and just use the file name
	// because we use this name to set the remote path, and the host
	// path will usually not match the remote path.
	CString cstrProjectExe = m_strProjectName.Right(m_strProjectName.GetLength() -
							 m_strProjectName.ReverseFind('\\') - 1) + 
							 ((projType == PROJECT_EXE) ? ".exe" : ".dll");


	// we need to make sure the debugger isn't running on a previous project
	// or COWorkSpace::CloseAllWindows will fail.
	if(dbg.GetDebugState() != NotDebugging)
		dbg.StopDebugging();

	// we need to close all windows before opening the exe or COProject::Open will fail.
 	COWorkSpace ws;
	ws.CloseAllWindows();

	// open the executable.	
	if(prj.Open(m_strProjectDir + "\\" + cstrProjectExe) != ERROR_SUCCESS)
		{
		m_pLog->RecordInfo("ERROR in %s: could not open \"%s\".", THIS_FUNCTION, 
							m_strProjectDir + "\\" + cstrProjectExe);
		return FALSE;
		}

	// allows us to access ide's connection settings via pconnec.
	COConnection *pconnec = pSubSuite->GetIDE()->GetConnectionInfo();
	
/*	// TODO(michma - 8/12/98): connection stuff has been moved to project settings.
re-enable when support layer has been updated.

	// set the connection as specified by the user.
	if(pconnec->SetAll() != ERROR_SUCCESS)
		{
		m_pLog->RecordInfo("ERROR in %s: could not set connection settings.", THIS_FUNCTION);
		return FALSE;
		}
*/	
	// if indicated, set the remote path and copy the executable to the remote machine.
	if(pconnec->GetUserConnections() != CONNECTION_LOCAL)
		
		{
		if(prj.SetRemotePath(cstrProjectExe) != ERROR_SUCCESS)
			{
			m_pLog->RecordInfo("ERROR in %s: could not set remote path using \"%s\".", 
							   THIS_FUNCTION, cstrProjectExe);
			return FALSE;
			}
		
		if(prj.UpdateRemoteFile(m_strProjectDir + "\\" + cstrProjectExe) != ERROR_SUCCESS)
			{
			m_pLog->RecordInfo("ERROR in %s: could not update remote file using \"%s\".", 
							   THIS_FUNCTION, m_strProjectDir + "\\" + cstrProjectExe);
			return FALSE;
			}
		}
	
	return TRUE;
	}


/*
** start debugging and do some initialization.  
*/

BOOL CDbgTestBase::StartDebugging(void)
{
	BOOL bSuccess; 
	COSource src;	

	bps.ClearAllBreakpoints();		// Clear All Breakpoints Just in case.
	dbg.StepOver(1);				// Start debugging session with initial trace.
	src.AttachActiveEditor();		// WinslowF
	bSuccess = dbg.AtSymbol("WinMain");

	// attach to the editor window the debugger opened
	bSuccess &= src.AttachActiveEditor();

	mem.SetMemoryFormat(MEM_FORMAT_BYTE); 
	dbg.SetSteppingMode(SRC); 		
	return bSuccess;
}


BOOL CDbgTestBase::StopDbgCloseProject(void)
{
	if (dbg.GetDebugState() != NotDebugging)
		dbg.StopDebugging();
	UIWB.CloseAllWindows();
	prj.Close();						

	return TRUE;
}


BOOL CDbgTestBase::VerifyProjectRan(int exitcode /* = 0 */)
{

	// REVIEW: use CODebug::VerifyTermination when it is available.
	// OpenOutputWindow
	UIOutput uow = UIDebug::ShowDockWindow(IDW_OUTPUT_WIN, TRUE);
//	EXPECT( uow.IsActive() );
	// Select the last line, write it to clipboard and look for "exit code 0"
	MST.DoKeys( "^{end}" );
	MST.DoKeys( "{up}" );

	char buffer[10];
	CString	szexcode = "exited with code ";			
	szexcode += itoa(exitcode, buffer, 10 ); // WinslowF - for OEM compatibility now we can specify the exit code to be verified

	if ( UIWB.VerifySubstringAtLine( szexcode ))
	{
		return (TRUE);
	}
	else
	{
		return (FALSE);
	}		
	
	return TRUE;
}


BOOL CDbgTestBase::StepInUntilFrameAdded( CString addframe )
{
	BOOL bResult = TRUE;
	COStack stk;
	CODebug dbg;
	FrameList framelist0;

	// Establish base line
	if(framelist0.CreateFromText( stk.GetAllFunctions() ) == FALSE)
	{
		m_pLog->RecordInfo( "StepInUntilFrameAdded - error retrieving stack frames");
		return FALSE;
	}
	Frame topframe( framelist0[0] );

	while( bResult ) {
		dbg.StepInto();
		FrameList current;
		if(current.CreateFromText( stk.GetAllFunctions() ) == FALSE)
		{
			m_pLog->RecordInfo( "StepInUntilFrameAdded - error retrieving stack frames");
			return FALSE;
		}

		if( current.GetSize() == framelist0.GetSize() ) {
			// Only check function name or image name for top frame.
			CString str0;
			if( topframe.GetFunctionName( str0 ) ) {
				bResult = Frame( current[0] ).Test( str0 );
				if( !bResult ) {
					m_pLog->RecordInfo( "StepInUntilFrameAdded - Mismatch top frame '%s' not in '%s'",
						(LPCTSTR)str0, (LPCTSTR)current[0] );
				}
			}
			else if( topframe.GetImageName( str0 ) ){
				bResult = Frame( current[0] ).Test( str0 );
				if( !bResult ) {
					m_pLog->RecordInfo( "StepInUntilFrameAdded - Mismatch top frame '%s' not in '%s'",
						(LPCTSTR)str0, (LPCTSTR)current[0] );
				}
			}
			else {
				// Bad format
				m_pLog->RecordInfo( "StepInUntilFrameAdded - Bad format for frame '%s'",
					(LPCTSTR)topframe );
				bResult = FALSE;
			}

			// The remaining frames should be identical
			for( int iFrame = 1; bResult && iFrame < framelist0.GetSize(); iFrame++ ) {
				bResult = framelist0[iFrame] == current[iFrame];
				if( !bResult ) {
					m_pLog->RecordInfo( "StepInUntilFrameAdded - Mismatch in frame %d, '%s' != '%s'",
						(LPCTSTR)current[0], (LPCTSTR)framelist0[0] );
				}
			}
		}
		else if( current.GetSize() == framelist0.GetSize() + 1 ) {
			// Test new top frame
			bResult = Frame( current[0] ).Test( addframe );

			// Only check function name or image name for 2nd frame since the line number
			// may change.
			CString str0;
			if( topframe.GetFunctionName( str0 ) ) {
				bResult = Frame( current[1] ).Test( str0 );
				if( !bResult ) {
					m_pLog->RecordInfo( "StepInUntilFrameAdded - Mismatch 2nd frame '%s' not in '%s'",
						(LPCTSTR)str0, (LPCTSTR)current[1] );
				}
			}
			else if( topframe.GetImageName( str0 ) ) {
				bResult = Frame( current[1] ).Test( str0 );
				if( !bResult ) {
					m_pLog->RecordInfo( "StepInUntilFrameAdded - Mismatch 2nd frame '%s' not in '%s'",
						(LPCTSTR)str0, (LPCTSTR)current[1] );
				}
			}
			else {
				// Bad format
				m_pLog->RecordInfo( "StepInUntilFrameAdded - Bad format for frame '%s'",
					(LPCTSTR)topframe );
				bResult = FALSE;
			}

			// The remaining frames should be identical
			for( int iFrame = 1; bResult && iFrame < framelist0.GetSize(); iFrame++ ) {
				bResult = framelist0[iFrame] == current[iFrame+1];
				if( !bResult ) {
					m_pLog->RecordInfo( "StepInUntilFrameAdded - Mismatch in frame %d, '%s' != '%s'",
						(LPCTSTR)current[iFrame+1], (LPCTSTR)framelist0[iFrame] );
				}
			}
			break;	// Get out of loop

		}
		else {
			// Unexpected number of frames
			m_pLog->RecordInfo( "StepInUntilFrameAdded - Expected %d frames, got %d frames",
				framelist0.GetSize() + 1, current.GetSize() );
			bResult = FALSE;
		}
	}

	return bResult;
}

BOOL CDbgTestBase::StepInUntilFrameRemoved() {
	BOOL bResult = TRUE;
	COStack stk;
	CODebug dbg;
	FrameList framelist0;

	// Establish base line
	if(framelist0.CreateFromText( stk.GetAllFunctions() ) == FALSE)
	{
		m_pLog->RecordInfo( "StepInUntilFrameRemoved - error retrieving stack frames");
		return FALSE;
	}
	Frame topframe( framelist0[0] );

	while( bResult ) {
		dbg.StepInto();
		FrameList current;
		if(current.CreateFromText( stk.GetAllFunctions() ) == FALSE)
		{
			m_pLog->RecordInfo( "StepInUntilFrameRemoved - error retrieving stack frames");
			return FALSE;
		}

		//
		// REVIEW: This can be shared with StepInUntilAdded
		//
		if( current.GetSize() == framelist0.GetSize() ) {
			// Only check function name or image name for top frame.
			CString str0;
			if( topframe.GetFunctionName( str0 ) ) {
				CString str1;
				if( topframe.GetParameters( str1 ) ) {
					str0 += "(" + str1 + ")";
				}
				bResult = Frame( current[0] ).Test( str0 );
				if( !bResult ) {
					m_pLog->RecordInfo( "StepInUntilFrameRemoved - Mismatch top frame '%s' not in '%s'",
						(LPCTSTR)str0, (LPCTSTR)current[0] );
				}
			}
			else if( topframe.GetImageName( str0 ) ) {
				bResult = Frame( current[0] ).Test( str0 );
				if( !bResult ) {
					m_pLog->RecordInfo( "StepInUntilFrameRemoved - Mismatch top frame '%s' not in '%s'",
						(LPCTSTR)str0, (LPCTSTR)current[0] );
				}
			}
			else {
				// Bad format
				m_pLog->RecordInfo( "StepInUntilFrameRemoved - Bad format for frame '%s'",
					(LPCTSTR)topframe );
				bResult = FALSE;
			}

			// The remaining frames should be identical
			for( int iFrame = 1; bResult && iFrame < framelist0.GetSize(); iFrame++ ) {
				bResult = framelist0[iFrame] == current[iFrame];
				if( !bResult ) {
					m_pLog->RecordInfo( "StepInUntilFrameRemoved - Mismatch in frame %d, '%s' != '%s'",
						iFrame, (LPCTSTR)current[0], (LPCTSTR)framelist0[0] );
				}
			}
		}
		else if( current.GetSize() == framelist0.GetSize() - 1 ) {
			// The remaining frames should be identical
			for( int iFrame = 0; bResult && iFrame < current.GetSize(); iFrame++ ) {
				bResult = framelist0[iFrame+1] == current[iFrame];
				if( !bResult ) {
					m_pLog->RecordInfo( "StepInUntilFrameRemoved - Mismatch in frame %d, '%s' != '%s'",
						iFrame, (LPCTSTR)current[iFrame], (LPCTSTR)framelist0[iFrame+1] );
				}
			}
			break;	// Get out of loop
		}
		else {
			// Unexpected number of frames
			m_pLog->RecordInfo( "StepInUntilFrameRemoved - Expected %d frames, got %d frames",
				framelist0.GetSize() + 1, current.GetSize() );
			bResult = FALSE;
		}
	}

	return bResult;
}


int ReturnCode =0;
bp  * ReturnBP =0;

void CDbgTestBase::LogResult(int line, int result,LPCSTR szoperation, int code /* 0 */, LPCSTR szComment /* "" */)

	{
	CString szOpCom = (CString)szoperation + "  " + szComment;
	if(result == PASSED)
		m_pLog->RecordSuccess("%s", szOpCom);
	else
		{
		CString szextra;
		char chbuf[24];
		szextra = szextra + "Error Code = " + itoa(code, chbuf, 10);
		m_pLog->RecordFailure("LINE %d %s", line, szextra + szOpCom);
		}
	}


//
//	Returns the current directory of the first time this function was called.
//
LPCSTR CDbgTestBase::HomeDir()
{
	static CString szCurDir;

	if( szCurDir.IsEmpty() )
	{
		char* psz = szCurDir.GetBufferSetLength(_MAX_DIR);

		GetCurrentDirectory(_MAX_DIR - 1, psz);
		szCurDir.ReleaseBuffer(-1);
		if( szCurDir.Right(1) != '\\' )
			szCurDir += "\\";
	}
 	return szCurDir;
}

//
//	Returns the fully qualified path with szAddPath appended to the breadth dir.	
//
LPCSTR CDbgTestBase::FullPath( LPCSTR szAddPath )
{
	static CString szFilespec;

	szFilespec = HomeDir();
	szFilespec += szAddPath;

	return szFilespec;
}


void CDbgTestBase::FillExprInfo(EXPR_INFO &expr_info, 
								 int state, LPCSTR type, LPCSTR name, LPCSTR value)

	{
	expr_info.state = state;
	expr_info.type = type;
	expr_info.name = name;
	expr_info.value = value;
	}


BOOL CDbgTestBase::ExprInfoIs(EXPR_INFO * expr_info_actual, EXPR_INFO * expr_info_expected, 
							   int total_rows /* 1 */)
	
	{
	for(int i = 0; i < total_rows; i++)

		{
		if(expr_info_actual[i].state != expr_info_expected[i].state)
			
			{
			m_pLog->RecordFailure("ERROR in ExprInfoIs(): state is %d instead of %d", 
								  expr_info_actual[i].state, expr_info_expected[i].state);

			return FALSE;
			}

		if(expr_info_actual[i].type != expr_info_expected[i].type)
			
			{
			m_pLog->RecordFailure("ERROR in ExprInfoIs(): type is %s instead of %s", 
								  expr_info_actual[i].type, expr_info_expected[i].type);

			return FALSE;
			}

		if(expr_info_actual[i].name != expr_info_expected[i].name)
			
			{
			m_pLog->RecordFailure("ERROR in ExprInfoIs(): name is %s instead of %s", 
								  expr_info_actual[i].name, expr_info_expected[i].name);

			return FALSE;
			}

		if(expr_info_expected[i].value.Mid(0, 7) != "UNKNOWN") 

			{
			BOOL value_tests_passed;

			if(expr_info_expected[i].value.Mid(0, 7) == "ADDRESS")
				
				{
				value_tests_passed = ValueIsAddress(expr_info_actual[i].value);

				// is extra data besides address expected? (ex. peek at string)
				if(expr_info_expected[i].value != "ADDRESS")
				
					{
					// user must supply extra data to avoid CString assert.
					EXPECT(expr_info_expected[i].value.GetLength() > 8);

					// make sure actual string is long enough so we avoid CString assert.
					if(expr_info_actual[i].value.GetLength() > 10)
					{
						// verify extra data is correct.
						if(expr_info_actual[i].value.Mid(10) ==  (" " + expr_info_expected[i].value.Mid(8)))
							value_tests_passed=TRUE;	
					}
					else
						// actual string wasn't long enough to contain extra data.
						value_tests_passed = FALSE;
					}
				}	 
	
			else	
				value_tests_passed = expr_info_actual[i].value == expr_info_expected[i].value;

			if(!value_tests_passed)
				
				{
				m_pLog->RecordFailure("ERROR in ExprInfoIs(): value is %s instead of %s", 
									  expr_info_actual[i].value, expr_info_expected[i].value);

				return FALSE;
				}
			}
		}

	return TRUE;
	}


BOOL CDbgTestBase::ValueIsAddress(CString str)
	{
	return (str.Mid(0, 2) == "0x") && 
		   (str.Mid(2).SpanIncluding("0123456789abcdef").GetLength() == 8);
	}


void CDbgTestBase::VerifyLocalsWndString(LPCSTR strType, LPCSTR strName,  LPCSTR strValue, int lineNumber)
{
	uivar.Activate();
	EXPECT_TRUE(uivar.SetPane(PANE_LOCALS) == ERROR_SUCCESS);

	EXPR_INFO expr_info;
	uivar.GetAllFields(&expr_info, lineNumber, 1);
	CString csType = expr_info.type;
	CString csName = expr_info.name;
	CString csValue = expr_info.value;


	WriteLog( (csType == strType && csName == strName && 
		csValue.Find(strValue) >= 0) ? PASSED : FAILED,
		"Current Locals line: \"%s %s %s \" Expectation: \"%s %s %s \"", 
		(LPCSTR)csType, (LPCSTR)csName, (LPCSTR)csValue, 
		(LPCSTR)strType, (LPCSTR)strName, (LPCSTR)strValue );
}

BOOL CDbgTestBase::LocalsWndStringIs(LPCSTR strType, LPCSTR strName,  LPCSTR strValue, int lineNumber)
{
	uivar.Activate();
	EXPECT_TRUE(uivar.SetPane(PANE_LOCALS) == ERROR_SUCCESS);
	BOOL ret = FALSE;

	EXPR_INFO expr_info;
	if(uivar.GetAllFields(&expr_info, lineNumber, 1)!=ERROR_SUCCESS)
		return FALSE;
	if(strcmp(strType, expr_info.type)==0 &&
	   strcmp(strName, expr_info.name)==0 &&
	   strstr(strValue,expr_info.value)!=NULL)
		ret = TRUE;

	return ret;
}


BOOL CDbgTestBase::PrepareProject(LPCSTR projName)
{
	const char* const THIS_FUNCTION = "PrepareProject()";
	CString projPathAndName = projName;
	int iName = projPathAndName.ReverseFind('\\');
	if( iName < 0 )
	{
		m_strProjectDir = FullPath("apps");
		m_strProjectName = projName;
	}
	else
	{
		m_strProjectDir = (CString)FullPath("apps") + "\\" + projPathAndName.Left(iName);
		m_strProjectName = projPathAndName.Mid(iName + 1);
	}


	// project exe has same base as makefile (i.e. xxx.mak/xxx.exe/xxx.dll)
	// we want to strip out any prepended path and just use the file name
	// because we use this name to set the remote path, and the host
	// path will usually not match the remote path.
/*	CString cstrProjectExe = m_strProjectName.Right(m_strProjectName.GetLength() -
							 m_strProjectName.ReverseFind('\\') - 1) + 
							 ((projType == PROJECT_EXE) ? ".exe" : ".dll");
*/
	CString cstrProjectExe = m_strProjectName;

	// we need to make sure the debugger isn't running on a previous project
	// or COWorkSpace::CloseAllWindows will fail.
	if(dbg.GetDebugState() != NotDebugging)
		dbg.StopDebugging();

	// we need to close all windows before opening the exe or COProject::Open will fail.
 	COWorkSpace ws;
	ws.CloseAllWindows();

	// open the executable.	
	if(prj.Open(m_strProjectDir + "\\" + cstrProjectExe) != ERROR_SUCCESS)
		{
		m_pLog->RecordInfo("ERROR in %s: could not open \"%s\".", THIS_FUNCTION, 
							m_strProjectDir + "\\" + cstrProjectExe);
		return FALSE;
		}

	CIDESubSuite* pSubSuite=GetSubSuite();
	ASSERT(pSubSuite->IsKindOf(RUNTIME_CLASS(CIDESubSuite))); //our tests are IDE tests only

	// allows us to access ide's connection settings via pconnec.
	COConnection *pconnec = pSubSuite->GetIDE()->GetConnectionInfo();
//	COConnection *pconnec = GetSubSuite()->GetIDE()->GetConnectionInfo();

	// set the connection as specified by the user.
	if(pconnec->SetAll() != ERROR_SUCCESS)
		{
		m_pLog->RecordInfo("ERROR in %s: could not set connection settings.", THIS_FUNCTION);
		return FALSE;
		}
	
	// if indicated, set the remote path and copy the executable to the remote machine.
	if(pconnec->GetUserConnections() != CONNECTION_LOCAL)
		
		{
		if(prj.SetRemotePath(cstrProjectExe) != ERROR_SUCCESS)
			{
			m_pLog->RecordInfo("ERROR in %s: could not set remote path using \"%s\".", 
							   THIS_FUNCTION, cstrProjectExe);
			return FALSE;
			}
		
		if(prj.UpdateRemoteFile(m_strProjectDir + "\\" + cstrProjectExe) != ERROR_SUCCESS)
			{
			m_pLog->RecordInfo("ERROR in %s: could not update remote file using \"%s\".", 
							   THIS_FUNCTION, m_strProjectDir + "\\" + cstrProjectExe);
			return FALSE;
			}
		}
	
	return TRUE;
}


void CDbgTestBase::LogTestHeader(LPCSTR test_desc_str, TestType testtype /* TT_TEST */)
{
	if( testtype == TT_TEST )
		m_pLog->RecordInfo("");
	m_pLog->RecordInfo("");
	CString strTestType;
    switch (testtype)
    {
		case TT_TEST:
			strTestType = "TEST";
			break;

		case TT_SUBTEST:
			strTestType = "SubTest";
			break;

		case TT_TESTCASE:
			strTestType = "TestCase";
			break;
	}

	m_pLog->RecordInfo("%s: %s", strTestType, test_desc_str);
}

void CDbgTestBase::LogTestStartEnd(LPCSTR test_desc_str, StartEndTest startend /* START_TEST */)
{
	CString strTest = "END";

	if( startend == START_TEST)
	{
		strTest = "START";
		m_pLog->RecordInfo("");
	}
 
	strTest += "***TEST";

	m_pLog->RecordInfo("%s: %s", strTest, test_desc_str);
}

void CDbgTestBase::LogTestBegin(LPCSTR test_desc_str)
{
	m_pLog->RecordInfo("");
	m_pLog->RecordInfo("BEGIN***TEST: %s", test_desc_str);
}

void CDbgTestBase::LogTestEnd(LPCSTR test_desc_str)
{
	m_pLog->RecordInfo("END***TEST: %s", test_desc_str);
}

void CDbgTestBase::LogTestEnd(LPCSTR test_desc_str, CTimeSpan elapsed_time, bool timing /* FALSE */)
{
//	GetLog()->RecordInfo("***TEST elapsed time %s", elapsed_time.Format("%H:%M:%S"));
	if( timing )
	{
		m_pLog->RecordInfo("END***TEST: %s  elapsed time %s", test_desc_str, elapsed_time.Format("%H:%M:%S"));
	}
	else
	{
		m_pLog->RecordInfo("END***TEST: %s", test_desc_str);
	}
}

void CDbgTestBase::LogSubTestHeader(LPCSTR test_desc_str)
{
	m_pLog->RecordInfo("");
	m_pLog->RecordInfo("SubTest: %s", test_desc_str);
}


BOOL CDbgTestBase::CopyProjectSources( LPCSTR szSRCFileLoc, LPCSTR szProjLoc )
{
	CString szPattern		= (CString)szSRCFileLoc + "*.*";

	// Copy sources to the project directory
	WIN32_FIND_DATA ffdImgFile;
    HANDLE hFile = FindFirstFile(szPattern, &ffdImgFile);
    if( hFile != INVALID_HANDLE_VALUE )
    {
        do
        {
			CopyFile( (CString)szSRCFileLoc + ffdImgFile.cFileName, (CString)szProjLoc + ffdImgFile.cFileName, FALSE );
			SetFileAttributes((CString)szProjLoc + ffdImgFile.cFileName, FILE_ATTRIBUTE_NORMAL );
		}
        while( FindNextFile(hFile, &ffdImgFile) );
        EXPECT( GetLastError() == ERROR_NO_MORE_FILES );
		return TRUE;
    }
	else
	{
		m_pLog->RecordFailure("Could not copy sources from %s to %s", szSRCFileLoc, szProjLoc);
		return FALSE;
	}
}

void CDbgTestBase::RemoveTestFiles( CString szProjLoc )
{
	KillAllFiles( szProjLoc, TRUE );
}
	
BOOL CDbgTestBase::IsMsgBox(CString strMsg, CString strBtn)
{	
	if( WaitMsgBoxText( strMsg, 10 ) )
	{
		MST.WButtonClick( strBtn );
		return TRUE;
	}
	else
		return FALSE;
}

CString CDbgTestBase::GetBldLog()
{
	CString BuildRecord;
	CString str;
	
	// Get build data
	UIWB.DoCommand(IDM_WINDOW_ERRORS,DC_ACCEL);  // goto output window
	MST.DoKeys("^{END}");			// goto end
	MST.DoKeys("+^{HOME}");			// select whole build record
	MST.DoKeys("^c");				// copy it
	GetClipText(BuildRecord);       // get text from clipboard
	MST.DoKeys("{ESC}");            // return focus from output window
	BuildRecord.MakeLower();
	return (BuildRecord);
	
};


BOOL CDbgTestBase::VerifyBuildOccured( BOOL iBuild )
{
	// iBuild == TRUE  -  project should be rebuilt
	// iBuild == FALSE  -  project should not be rebuilt

	// Get the output window contents
	CString szBldLog = GetBldLog();
	if (szBldLog == "")
	{
		m_pLog->RecordFailure("Incorrect build: Output window is empty");
		return FALSE;
	}

	int iFoundCompiling = szBldLog.Find ("compiling");	// International ?
	int iFoundLinking = szBldLog.Find ("linking");		// International ?

	if( iBuild )
	{
		if( (iFoundCompiling < 0) && (iFoundLinking < 0) )
		{
			m_pLog->RecordFailure("Incorrect build: can't find 'Compiling' or 'Linking");
			return FALSE;
		}
	}
	else
	{
		if ( (iFoundCompiling > 0) || (iFoundLinking > 0) )
		{
			m_pLog->RecordFailure("Nothing change, the project shouldn't be rebuilt.");
			return FALSE;
		}
	}

	return TRUE;
}

int CDbgTestBase::VerifyBuildString( LPCSTR verifyString, BOOL fExist /* TRUE */ )
{
	int iFoundString;

	// Get the output window contents
	CString szBldLog = GetBldLog();
	CString testString = verifyString;

	// For some reason the output window text is always lower case when extracted.

	testString.MakeLower();
	iFoundString = szBldLog.Find(testString);

	if ( (iFoundString < 0) && fExist )
	{
		m_pLog->RecordFailure("Can't find %s", verifyString);
	}
	
	if ( (iFoundString >= 0) && !fExist )
	{
		m_pLog->RecordFailure("Found %s : it should not be there", verifyString);
	}

	return iFoundString;
}

int CDbgTestBase::VerifyDebugOutputString( LPCSTR verifyString, BOOL fExist /* TRUE */ )
{
	int iFoundString;
	CString csLog;
	CString testString = verifyString;

	// Get the output window contents
	// TODO(michma): mismatch with CODebug here. need to merge?
	dbg.SetDebugOutputPane(IDSS_PANE_DEBUG_OUTPUT);
	MST.DoKeys("^{END}");			// goto end
	MST.DoKeys("+^{HOME}");			// select whole build record
	MST.DoKeys("^c");				// copy it
	GetClipText(csLog);       // get text from clipboard
	MST.DoKeys("{ESC}");            // return focus from output window
	csLog.MakeLower();

	// For some reason the output window text is always lower case when extracted.
	testString.MakeLower();
	iFoundString = csLog.Find(testString);

	if ( (iFoundString < 0) && fExist )
	{
		m_pLog->RecordFailure("Can't find %s", verifyString);
	}
	
	if ( (iFoundString >= 0) && !fExist )
	{
		m_pLog->RecordFailure("Found %s : it should not be there", verifyString);
	}

	return iFoundString;
}

CStartEndTest::CStartEndTest(LPCSTR testName, CDbgTestBase *test, bool timing /*=FALSE*/)
{
	m_testName = testName;
	m_Test = test;
	if( timing )
	{
		m_startTime = CTime::GetCurrentTime();
		m_timing = TRUE;
		m_Test->LogTestBegin( m_testName );
	}
}

CStartEndTest::~CStartEndTest()
{
	if( m_timing )
	{
		m_endTime = CTime::GetCurrentTime();
		CTimeSpan elapsed_time(m_endTime - m_startTime);
		m_Test->LogTestEnd( m_testName, elapsed_time, m_timing );
	}
	else
		m_Test->LogTestEnd( m_testName );
}


/*
unused in Xbox
char szecEditTypes[6][256] =
{
	"EC_NO_STACK_FRAME",
	"EC_CURRENT_FRAME_AFTER_IP",
	"EC_CURRENT_FRAME_BEFORE_IP",
	"EC_CURRENT_FRAME_AT_IP", 
	"EC_PARENT_FRAME_AFTER_IP", 
	"EC_PARENT_FRAME_BEFORE_IP"
};

BOOL CDbgTestBase::IsProjectReady(ProjType projType)
{
	switch (projType)
	{
	case PT_EXE:
		m_strProjectName = "ecconsol";
		break;
	case PT_EXT:
		m_strProjectName = "ecwinapp";
		break;
	case PT_INT:
		m_strProjectName = "ecmfcapp";
		break;
	}
	
	m_strSrcDir = "ecapp";	//ToDo: We should get the name from file with all languages strings
	m_strProjectLoc  = m_strCWD + "apps\\" + m_strProjectName + "\\";
	CString szSRCFileLoc	= m_strCWD + "src\\" + m_strSrcDir + "\\";
	CString szProjPathName	= (CString)("..\\apps\\") + m_strProjectName + "\\" + m_strProjectName;

	if (projType == PT_INT)	  // MFC app
		RemoveTestFiles( m_strProjectLoc + "res\\");
	RemoveTestFiles( m_strProjectLoc );
	CreateDirectory(m_strProjectLoc, 0);          
	EXPECT_TRUE( CopyProjectSources( szSRCFileLoc, m_strProjectLoc ) );
	if (projType == PT_INT)	   // MFC app
	{
		CreateDirectory(m_strProjectLoc + "res\\", 0);          
		EXPECT_TRUE( CopyProjectSources( szSRCFileLoc + "res\\", m_strProjectLoc + "res\\" ) );
	}
	if (projType == PT_EXE)	   // EXE consol app
	{
		if( SetProject(szProjPathName) )
			return TRUE;
	}
	else
	{
		int iWaitForBuild = 3;	// Minutes to wait for the build to complete
		if( PrepareProject(szProjPathName) && VERIFY_TEST_SUCCESS( prj.Build(iWaitForBuild)))
		{
			prj.SetExeForDebugSession(m_strProjectName + ".exe");
			return TRUE;
		}
	}
	return FALSE;
}


LPCSTR CDbgTestBase::GetThreadIDStr(LPCSTR szThreadIDVar)

{
	static char szText[12] = "";
	DWORD dwThreadID;
	
	if(!cxx.GetExpressionValue(szThreadIDVar, &dwThreadID))
	{
		LOG->RecordInfo("CDbgTestBase::GetThreadIDStr - GetExpressionValue failed.");
		return (LPCSTR)szText;
	}

	sprintf(szText, "%.8x", dwThreadID);
	return (LPCSTR)szText;
}
*/


void CleanUp(LPCSTR szDirName)

{
	CString strPath;
	//strPath = (CString)"src\\" + szDirName + "\\";
	if (szDirName[0] != '\\')
		strPath = CString(TMP_PROJ_DIR) + "\\" + szDirName + "\\";
	else
		strPath = szDirName;

	KillFiles(strPath + "*.dsp");
	KillFiles(strPath + "*.dsw");
	KillFiles(strPath + "*.opt");
	KillFiles(strPath + "*.plg");
	KillFiles(strPath + "*.ncb");
	KillFiles(strPath + "*.mak");
	KillFiles(strPath + "*.lnk");
	KillFiles(strPath + "*.mdp");
	KillFiles(strPath + "*.pdb");
	KillFiles(strPath + "*.obj");
	KillFiles(strPath + "*.ilk");
	KillFiles(strPath + "*.dll");
	KillFiles(strPath + "*.exe");
	KillFiles(strPath + "*.xbe");
}


void KillFiles(LPCSTR szPathName, LPCSTR szFileName /* NULL */)

{
	WIN32_FIND_DATA ffdImgFile;
	HANDLE hFile;
	CString str, strPattern, strPath = szPathName;

	if(szFileName != NULL)
		
	{
		if((strPath.Right(1) != "\\") && (szFileName[0] != '\\'))
			strPath += "\\";

		strPattern = strPath + szFileName; 
	}

	else
		strPattern = strPath;

	hFile = FindFirstFile(strPattern, &ffdImgFile);
	
	if(hFile != INVALID_HANDLE_VALUE)
	{
		do
		{
			str = strPath + ffdImgFile.cFileName;
			DeleteFile(str);
		} while(FindNextFile(hFile, &ffdImgFile));
	}

	FindClose(hFile);
}


CString CDbgTestBase::FindInPath(LPCSTR szAppName)
{
	CIDESubSuite* pSubSuite=GetSubSuite();

	CString cstrPathEnv;
	pSubSuite->GetIDE()->GetEnvironmentVar(settingPathEnv, cstrPathEnv);
	cstrPathEnv = settingPathEnv + CString("=") + cstrPathEnv;

	// get windows and system dir and append to path environment string.
	CString cstrWindowsDir, cstrSystemDir;
	// each dir is prepended with a semi-colon as a separator. 
	char chDirBuf[MAX_PATH] = ";";
 	GetWindowsDirectory(&chDirBuf[1], MAX_PATH);	// &chDirBuf[0] = ';'
	cstrWindowsDir = chDirBuf;
	GetSystemDirectory(&chDirBuf[1], MAX_PATH);		// &chDirBuf[0] = ';'
	cstrSystemDir = chDirBuf;
	cstrPathEnv += cstrWindowsDir + cstrSystemDir;

	// environment block.
	char chEnvBlock[1024];
	// pointer to navigate through block.
	char *pch = chEnvBlock;

	// put the path into the block.
	// each string is terminated by null.
	strcpy(chEnvBlock, cstrPathEnv);
 
	// to find our app in the toolset path environment, 
	// we'll use cstrPathFrag to search	for and extract directories
	// from cstrPathEnv	(minus first 5 characters which are "PATH=").
	CString cstrPathFrag = cstrPathEnv.Mid(5);
	// stores dir extracted from cstrPathFrag.
	CString cstrDir;
	// stores index of next semi-colon (dir separator) in cstrPathEnv.
	int indexSemi;
	// handle to nmake.exe returned by FindFirstFile().
	HANDLE hNmake;
	// FindFirstFile param.
	WIN32_FIND_DATA fdFindData;

	while(1)
		
		{
		// find the next semi-colon which terminates the next dir to search.
		indexSemi = cstrPathFrag.Find(';');

		// if a semi-colon was found, extract the dir it terminates. otherwise
		// we are at the final dir in the path environment. 
		if(indexSemi != -1)
			cstrDir = cstrPathFrag.Left(indexSemi);
		else
			cstrDir = cstrPathFrag;
			
		// only want to search dirs at least 3 chars long. for example:
		// path  = C:\;
		// index = 0123
		if(cstrDir.GetLength() >= 3)
			hNmake = FindFirstFile(cstrDir + "\\" + szAppName, &fdFindData);
  		
		// stop searching if nmake was found in cstrPathFrag.
		if(hNmake != INVALID_HANDLE_VALUE)
			break;

		// if no semi-colon was found or it was the last char in the path, 
		// then we just searched the final dir in the environment string, 
		// but still haven't found nmake. 
	 	if((indexSemi == -1) || (cstrPathFrag.GetLength() == (indexSemi + 1)))
			{
			m_pLog->RecordInfo("ERROR in %s: could not find %s in path environment "
							   "specified by CAFE.", "CDbgTestBase::FindInPath", szAppName);
			return "";
			}
		else
			// advance to the start of the next dir after the semi-colon.
			cstrPathFrag = cstrPathFrag.Mid(indexSemi + 1);
		}

	return cstrDir;
}

BOOL CDbgTestBase::XboxDebuggerConnect()
{
	BOOL bRetVal = dbg.Go(0, 0, 0, WAIT_FOR_RUN);
	if (!bRetVal)
	{
		m_pLog->RecordInfo("ERROR in %s: unable to start debugger, forcing stop",
						   "CDbgTestBase::XboxDebuggerConnect()");
		dbg.StopDebugging(NOWAIT);
		return FALSE;
	}

	return bRetVal;
}

typedef HRESULT (CALLBACK* LPDMAPI_LPCSTR) (LPCSTR);
typedef HRESULT (CALLBACK* LPDMAPI_LPCSTR_LPCSTR) (LPCSTR, LPCSTR);

BOOL CDbgTestBase::XboxSetTarget(LPCSTR szXbox)
{
	WaitStepInstructions("Calling DmSetXboxName");
	HINSTANCE hXboxDbgDll = LoadLibrary("XboxDbg");
	if (hXboxDbgDll == NULL)
	{
		LOG->RecordInfo("LoadLibrary() failed");
		return FALSE;
	}

	LPDMAPI_LPCSTR pDmSetXboxName = (LPDMAPI_LPCSTR) GetProcAddress(hXboxDbgDll, "DmSetXboxName");
	if (!pDmSetXboxName)
	{
		LOG->RecordInfo("GetProcAddress(\"DmSetXboxName\") failed");
		FreeLibrary(hXboxDbgDll);
		return FALSE;
	}

	HRESULT hr = pDmSetXboxName(REMOTE_XBOX_IP);

	if (FAILED(hr))
	{
		LOG->RecordInfo("XboxSetTarget() failed");
		FreeLibrary(hXboxDbgDll);
		return FALSE;
	}

	FreeLibrary(hXboxDbgDll);
	return TRUE;
}


// copy xbe binary to target
BOOL CDbgTestBase::XboxCopyToTarget(LPCSTR szFileName /* = NULL */)
{
	//const char* const THIS_FUNCTION = "CopyToXboxTarget()";

	// TODO: fix COProject::GetOutputXbeFile

	CString xbeFile = ((szFileName && *szFileName) ? szFileName : 
		(CString(prj.GetDir()) + "\\Debug\\" + CString(prj.GetName()) + ".xbe"));

	CString xbeDir, xbeName;
	int ipos = xbeFile.ReverseFind('\\');
	if (ipos > -1)
	{
		xbeDir = xbeFile.Left(ipos);
		xbeName = xbeFile.Mid(ipos+1);
	}
	else
	{
		xbeName = xbeFile;
	}

	HINSTANCE hXboxDbgDll = LoadLibrary("XboxDbg");
	if (hXboxDbgDll == NULL)
	{
		LOG->RecordInfo("LoadLibrary() failed");
		return FALSE;
	}

	HRESULT hr;

	LPDMAPI_LPCSTR pDmMkdir = (LPDMAPI_LPCSTR) GetProcAddress(hXboxDbgDll, "DmMkdir");
	if (!pDmMkdir)
	{
		LOG->RecordInfo("GetProcAddress(\"DmMkdir\") failed");
		FreeLibrary(hXboxDbgDll);
		return FALSE;
	}

	hr = pDmMkdir(REMOTE_XBOX_DIR);
	/*
	if (FAILED(hr))
	{
		LOG->RecordInfo("XboxCopyToTarget() failed, DmMkdir(\"%s\") returned %08X",
			REMOTE_XBOX_DIR, hr);
		return FALSE;
	}
	*/

	LPDMAPI_LPCSTR_LPCSTR pDmSendFile = (LPDMAPI_LPCSTR_LPCSTR) GetProcAddress(hXboxDbgDll, "DmSendFileA");
	if (!pDmSendFile)
	{
		LOG->RecordInfo("GetProcAddress(\"DmSendFileA\") failed");
		FreeLibrary(hXboxDbgDll);
		return FALSE;
	}

	hr = pDmSendFile(xbeFile, REMOTE_XBOX_FILE);
	if (FAILED(hr))
	{
		LOG->RecordInfo("XboxCopyToTarget() failed, DmSendFileA(\"%s\", \"%s\") returned %08X",
			xbeFile, REMOTE_XBOX_FILE, hr);
		return FALSE;
	}

	return TRUE;
}


CString GetRemoteXboxIP()
{
	return CString(CMDLINE->GetTextValue("xbox",
		GetStrFromRegistry("HKCU\\Software\\Microsoft\\XboxSDK\\XboxName",
			"157.56.10.118")));
}
