
/*
    COMPILER.CPP

    Copyright 1994-95, Microsoft Corp.

    This file contains the implementation of the CCompilerDriver class.

    Edit history:
      1] CFlaat / 1-1-95 / finished creation

*/


#include "stdafx.h"
#include "bldtools.h"
#include "comppage.h"

#define new DEBUG_NEW

BOOL CCompilerDriver::CompileOnly(LPCSTR szSrcFile, LPCSTR szCompileArgs)                         // CL -c [...]
{
  CString strCmdLine;

  // TODO: use permanent compiler switches

  strCmdLine.Format("-c %s %s", szSrcFile, szCompileArgs);

  m_psettingsTarget->SetTextValue(settingCmdLine, strCmdLine);

  if (!Launch())
    return FALSE;

  return !GetExitCode();
}

BOOL CCompilerDriver::CompileAndLink(LPCSTR szSrcFile, LPCSTR szCompileArgs, LPCSTR szLinkArgs) // CL [...] -link [...]
{
  CString strCmdLine;

  // TODO: use permanent compiler and linker switches

  strCmdLine.Format("%s %s -link %s", szSrcFile, szCompileArgs, szLinkArgs);

  m_psettingsTarget->SetTextValue(settingCmdLine, strCmdLine);

  if (!Launch())
    return FALSE;

  return !GetExitCode();
}

BOOL CCompilerDriver::LinkOnly(LPCSTR szObjFile, LPCSTR szLinkArgs)                               // CL -link [...]
{
  CString strCmdLine;

  // TODO: use permanent linker switches

  strCmdLine.Format("-link %s %s", szObjFile, szLinkArgs);

  m_psettingsTarget->SetTextValue(settingCmdLine, strCmdLine);

  if (!Launch())
    return FALSE;

  return !GetExitCode();
}


BOOL CCompilerDriver::Initialize(CToolset::BUILD_TYPE nBuildType, CString strKey)
{
	// instantiate the settings object for this target
	strKey += "\\";
  strKey += "CompilerDriver";
	m_psettingsTarget = new CSettings(strKey);

	// instantiate the property page for this target
	m_ppageTarget = new CCompilerDriverPage(m_psettingsTarget);

	// initialize settings
	return InitializeSettings();
}

BOOL CCompilerDriver::ApplySettings(CSettings* pSettings)
{
	// call the base class
	CConsoleTarget::ApplySettings(pSettings);

	// TODO(CFlaat): apply command line overrides

	return TRUE;
}


BOOL CCompilerDriver::InitializeSettings(void)
{
	CString strDrive;
	CString strDir;

	char cl_path[256],*pcl_exe=NULL;

	BOOL bResult = TRUE;

	// call the base class
	bResult &= CConsoleTarget::InitializeSettings();

	// initialize  default settings
	m_psettingsTarget->SetTextValue(settingFilename, "cl.exe");
	m_psettingsTarget->SetTextValue(settingWorkingDir, "e:\\cafe\\v3\\sd2\\testdir");

	m_strFileName = m_psettingsTarget->GetTextValue(settingFilename);
  //REVIEW(chriskoz): I've no idea about these bogus, hardcoded paths. The path shld be taken from ENV, I supose.
	BOOL bPath;
	bPath = m_mapEnv.Lookup( "PATH", /*CObject*&*/m_strPath); // = "d:\\v3\\bin\\";
	if(bPath)
		bPath = ::SearchPath(m_strPath, m_strFileName, NULL, 1024, cl_path, &pcl_exe);
	if(bPath && pcl_exe)
	{
		*pcl_exe=0; //cut the exe name which is not supposed to be in m_strPath
		m_strPath = cl_path;
	}
//	else //no path found, but leave it bogus (emptying it often means do not fail on bogus state)
//		m_strPath.Empty(); 
#if 0  //TODO(CFlaat): figure out why this doesn't work!
	// load settings from the registry
	bResult &= m_psettingsTarget->ReadRegistry();
	// write the settings back out to initialize registry
	bResult &= m_psettingsTarget->WriteRegistry();
#endif

	return bResult;
}


