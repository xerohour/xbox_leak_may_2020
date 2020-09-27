
/*
    EXECMAN.CPP

    Copyright 1995, Microsoft Corp.

    This file contains the implementation of the CExecutable class and its
    descendants.

    Edit history:
      1] CFlaat / 1-1-95 / created

*/


#include "stdafx.h"
#include "bldtools.h"
#include "execpage.h"

#define new DEBUG_NEW

BOOL CX86Executable::Execute(void)
{
  if (!Launch())
    return FALSE;

  return !GetExitCode();
}

BOOL CX86Executable::Initialize(CToolset::BUILD_TYPE nBuildType, CString strKey)
{
	// instantiate the property page for this target
	m_ppageTarget = new CX86ExecutablePage(m_psettingsTarget);

  return CExecutable::Initialize(nBuildType, strKey);
}

BOOL CExecutable::Initialize(CToolset::BUILD_TYPE nBuildType, CString strKey)
{
	// instantiate the settings object for this target
	strKey += "\\";
  strKey += "ExecManager";
	m_psettingsTarget = new CSettings(strKey);

	// initialize settings
	return InitializeSettings();
}

BOOL CExecutable::ApplySettings(CSettings* pSettings)
{
	// call the base class
	CConsoleTarget::ApplySettings(pSettings);

	// TODO(CFlaat): apply command line overrides

	return TRUE;
}


BOOL CExecutable::InitializeSettings(void)
{
	CString strDrive;
	CString strDir;

	BOOL bResult = TRUE;

	// call the base class
	bResult &= CConsoleTarget::InitializeSettings();

	// initialize  default settings
	m_psettingsTarget->SetTextValue(settingWorkingDir, "e:\\cafe\\v3\\sd2\\testdir");

#if 0  //TODO(CFlaat): figure out why this doesn't work!
	// load settings from the registry
	bResult &= m_psettingsTarget->ReadRegistry();
	// write the settings back out to initialize registry
	bResult &= m_psettingsTarget->WriteRegistry();
#endif

	return bResult;
}


void CExecutable::SetFileName(LPCSTR szExeFile)
{
  CString strDir;
	m_psettingsTarget->GetTextValue(settingWorkingDir, strDir);
	m_psettingsTarget->SetTextValue(settingFilename, strDir + szExeFile);

  m_strFileName = "e:\\cafe\\v3\\sd2\\testdir\\";
  m_strFileName += szExeFile;
}

