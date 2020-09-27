///////////////////////////////////////////////////////////////////////////////
//	CONVERT.CPP
//
//	Created by :			Date :
//              ThuyT                           07/05/96
//
//	Description :
//		Implementation of the CPrjConversionTest class
#include "stdafx.h"
#include "prjcases.h"
#include "convert.h"
#include <io.h>
 

#define new DEBUG_NEW
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;

IMPLEMENT_TEST(CPrjConversionTest, CTest, "V4.x Makefile Conversion", -1, CSniffDriver)
					   
////////////////////////////////////////////////////////////////////
//	CPrjConversionTest


void CPrjConversionTest::Run(void)
{
	CPrjUtil PrjUtil;
	CString szProjSRCLoc = m_strCWD + "src" + "\\Hello.V4";
	szProjEXELoc		 = m_strCWD + V4PROJDIR + "\\";

	m_pLog->RecordInfo("Verify V4.x Makefile Conversion...");

	//	added by dverma 6/20/2000
	m_pLog->RecordInfo("V4.x Makefile Conversion Test disabled due to postponed VS98 bug 49148.");
	return;

	PrjUtil.RemoveTestFiles( szProjEXELoc );
	if(!CreateDirectory(szProjEXELoc, NULL))
	{
		_TCHAR szMsg[256+_MAX_PATH];
		wsprintf(szMsg, "Can't create directory %s.", szProjEXELoc);
		m_pLog->RecordFailure(szMsg);
		return;
	}

	//Copy v4.x project to a working directory
	{
		_TCHAR szSourceDir[_MAX_PATH];
		lstrcpy(szSourceDir, szProjSRCLoc);
		int iLen = lstrlen(szSourceDir);
		ASSERT(iLen);
		_TCHAR* pszEnd = &szSourceDir[iLen];
		ASSERT(!*pszEnd);
		pszEnd = _tcsdec(szSourceDir, pszEnd);
		if(_tcsncmp(pszEnd, _T("\\"), 1))
			lstrcat(pszEnd, _T("\\"));
		lstrcat(szSourceDir, "*.*");

		WIN32_FIND_DATA ffdImgFile;
		HANDLE hFile = FindFirstFile(szSourceDir, &ffdImgFile);
		if( hFile == INVALID_HANDLE_VALUE )
		{
			_TCHAR szMsg[256+_MAX_PATH];
			wsprintf(szMsg, "Can't find files to copy from %s", szProjSRCLoc);
			m_pLog->RecordFailure(szMsg);
			return;
		}

	}
	if(!CopyTree ( szProjSRCLoc, szProjEXELoc))
	{
		_TCHAR szMsg[256 + 2*_MAX_PATH];
		wsprintf("Can't CopyTree() from %s to %s", szProjSRCLoc, szProjEXELoc);
		m_pLog->RecordFailure(szMsg);
		return;
	}

	// load v4.x makefile into Orion workspace
	m_pLog->RecordInfo("Open v4.x makefile...");
	EXPECT( OpenProject( PROJNAME) );
	//Bug #13391: conversion - set active project doesn't get restored correctly
	//Thus we need to set active configuration manually to workaround this problem
	//If this bug is fixed in the later version we can comment this line out.
    m_pLog->RecordInfo("Set active project/configuration to Hello Win32 Release");
	prj.SetTarget(EXERELEASETARGET);
	m_pLog->RecordInfo("Build Hello project...");
	BuildProject();
	m_pLog->RecordInfo("Verify build....");
	VerifyBuild( TRUE);
	m_pLog->RecordInfo("Close the project...");
	EXPECT ( CloseProject() );
	m_pLog->RecordInfo("Verify conversion of .dsp, .dsw, and .mak files");
	VerifyConversion();
	
	m_pLog->RecordInfo("Reopen the project to make sure it is loadable after the conversion...");
	EXPECT( OpenProject( PROJNAME) );
	m_pLog->RecordInfo("Build the project.  Don't expect any re-compiling or re-linking since nothing change...");
	BuildProject();
	VerifyBuild( FALSE);
	EXPECT ( CloseProject() );
	m_pLog->RecordInfo("Close the project");
	m_pLog->RecordInfo("Verify makefile conversion is complete.");
}


BOOL CPrjConversionTest::VerifyBuild( BOOL iBuild)
{
	CPrjUtil	PrjUtil;
	CString		szDLLProj = "dll";
	// Get the output window contents
	CString szBldLog = PrjUtil.GetBldLog();
	CString exe = "Hello - Win32 Release" ;
	CString dll = "Dll - Win32 Release" ;
	dll.MakeLower();
	exe.MakeLower();

	// For some reason the output window text is always lower case when extracted.
	int iFoundDLLLibrary = szBldLog.Find(dll);
	int iFoundEXEBinary = szBldLog.Find(exe);

	// Get the output window contents
//	CString szBldLog = PrjUtil.GetBldLog();
//	int iFoundDLLLibrary = szBldLog.Find("configuration: " + szDLLProj);
//	int iFoundEXEBinary = szBldLog.Find("configuration: hello");

//	int iFoundDLLLibrary = PrjUtil.Find(dll);
//	int iFoundEXEBinary = PrjUtil.Find(exe);



	if ( iBuild ) {
		if ( iFoundDLLLibrary < 0 ) {
			m_pLog->RecordFailure("DLL subproject didn't get built.");
			return FALSE;
		}
		if ( iFoundEXEBinary < 0 ) {
			m_pLog->RecordFailure("Hello EXE didn't get built.");
			return FALSE;
		}

		if ( iFoundDLLLibrary > iFoundEXEBinary ) {
			m_pLog->RecordFailure("DLL subproject should be built first.");
			return FALSE;
		}
	}
	else {
		int iFoundCPP = szBldLog.Find (".cpp");
		if ( iFoundCPP > 0 ) {
			m_pLog->RecordFailure("Nothing change, the project shouldn't be rebuilt.");
			return FALSE;
		}
	}

	return TRUE;
}

BOOL CPrjConversionTest::VerifyBLD( CString szFileName )
{
	//Check existant of .dsp files : 
	//Also make sure no dependency list included in the file.
	if (GetFileAttributes(szFileName) == -1 ) {
		m_pLog->RecordFailure("Can't find %s", szFileName);
		return FALSE;
	}
	else if (FindStrInFile(szFileName, "DEP_") > 0 ) { //found it
		m_pLog->RecordFailure("Found 'DEP_' list in %s file", szFileName);
		return FALSE;
	}
    return TRUE;
}
BOOL CPrjConversionTest::VerifyConversion( void )
{
	//Check .dsp files : expect to have Hello.dsp and Dll.dsp files
	VerifyBLD(szProjEXELoc + "Hello" + PRJEXTENSION);
	VerifyBLD(szProjEXELoc + "DLL\\DLL" + PRJEXTENSION);
	
	//Check DSW file
	if (FindStrInFile(szProjEXELoc + "Hello" + WSPEXTENSION, "Dll" ) == 0)  {
		m_pLog->RecordFailure("DLL project is not included in file %s", szProjEXELoc + "Hello" + WSPEXTENSION);
	}

	//Check .mak file: NYI
	return TRUE;
}


BOOL CPrjConversionTest::OpenProject( CString szProjectName  )
{
	if (!VERIFY_TEST_SUCCESS( prj.Open(szProjEXELoc + szProjectName))) {
		m_pLog->RecordFailure("Open project %s", szProjectName);
		return FALSE;
	}
	return TRUE;
}

BOOL CPrjConversionTest::BuildProject( void )
{
	int iWaitForBuild = 3;	// Minutes to wait for the build to complete
	
	if (!VERIFY_TEST_SUCCESS( prj.Build(iWaitForBuild))) {
		m_pLog->RecordFailure("Build Project");
		return FALSE;
	}
	return TRUE;
}

BOOL CPrjConversionTest::CloseProject( void )
{
	BOOL bSaveProjBeforeClose = TRUE; 
	
	if (!VERIFY_TEST_SUCCESS( prj.Close(bSaveProjBeforeClose))) {
		m_pLog->RecordFailure("Close the project");
		return FALSE;
	}
	return TRUE;
}	

