///////////////////////////////////////////////////////////////////////////////
//	EXTMAK.CPP
//
//	Created by :			Date :
//		ThuyT				02/06/96
//
//	Description :
//		Implementation of the CPrjExternalMAKTest class
#include "stdafx.h"
#include "prjcases.h"
#include "extmak.h" 

#define new DEBUG_NEW

#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;

IMPLEMENT_TEST(CPrjExternalMAKTest, CTest, "Loading External MAK", -1, CSniffDriver)
////////////////////////////////////////////////////////////////////
//	CPrjExternalMAKTest

void CPrjExternalMAKTest::Run(void)
{
	XSAFETY;

	CPrjUtil	PrjUtil;
	szExternalProject	 = m_strCWD + PROJNAME + "\\extern.mak";
	szNewExternalProject = m_strCWD + PROJNAME + "\\newextrn.mak";

	m_pLog->RecordInfo("Loading an external makefile...");
	EXPECT( PrepareExtProject() );
	EXPECT( OpenExtProject() );
	m_pLog->RecordInfo("Changing external debug option.");
	SetExtDebugOption();
	m_pLog->RecordInfo("Changing external build command line.");
	SetExtBuildCmdLine();
	PrjUtil.SetToolsDir(m_strCWD + TOOLSDIR);
	m_pLog->RecordInfo("Build the project...");
	BuildProject();
	PrjUtil.RemoveToolsDir(m_strCWD + TOOLSDIR);
	EXPECT( CloseProject() );
	m_pLog->RecordInfo("Successfully loading an external makefile.");
}

BOOL CPrjExternalMAKTest::OpenExtProject( void )
{
	int ret = TRUE;

	if (!VERIFY_TEST_SUCCESS( prj.Open(szNewExternalProject))) {
		m_pLog->RecordFailure("Open external makefile");
		ret = FALSE;
	}
	return ret;
}	

BOOL CPrjExternalMAKTest::CloseProject( void )
{
	BOOL bSaveProjBeforeClose = FALSE; 
	
	if (!VERIFY_TEST_SUCCESS( prj.Close(bSaveProjBeforeClose))) {
		m_pLog->RecordFailure("Close the project");
		return FALSE;
	}
	return TRUE;
}

BOOL CPrjExternalMAKTest::BuildProject( void )
{
	int iWaitForBuild = 3;	// Minutes to wait for the build to complete
	
	if (!VERIFY_TEST_SUCCESS( prj.Build(iWaitForBuild))) {
		m_pLog->RecordFailure("Build Project");
		return FALSE;
	}
	return TRUE;
}
	

BOOL CPrjExternalMAKTest::SetExtDebugOption( void )
{
	CString szNewDebugTarget	 = "testing.exe";

	if (!VERIFY_TEST_SUCCESS( prj.SetProjProperty(CT_EXT_DBGOPTS, VPROJ_IDC_EXTOPTS_TARGET, szNewDebugTarget))) {
		m_pLog->RecordFailure("Change the debug target name");
		return FALSE;
	}
	return TRUE;
}

BOOL CPrjExternalMAKTest::SetExtBuildCmdLine( void )
{
	CString	szDummyBuildTool	= m_strCWD + TOOLSDIR + "\\cl.exe";

	if (!VERIFY_TEST_SUCCESS(prj.SetBuildCommand( szDummyBuildTool ))) {
		m_pLog->RecordFailure("Change the build command line to use dummy tools");
		return FALSE;
	}
	return TRUE;
}			

BOOL CPrjExternalMAKTest::PrepareExtProject(void)
{
//	Copy sample makefile to a new name so that we don't use the one 
//	enlisted in the project. 
	if (!CopyFile(szExternalProject, szNewExternalProject, FALSE) )
	{
		m_pLog->RecordFailure("The external makefile copy did not execute correctly.");
		return FALSE;
	}                
	else 
	{	//	Make sure that new file is not marked read-only. If so, make it 
		//	read/write so that it can be modified
		SetFileAttributes(szNewExternalProject, FILE_ATTRIBUTE_NORMAL);
		return TRUE;
	}
}
