///////////////////////////////////////////////////////////////////////////////
//	BLDOPTS.CPP
//
//	Created by :			Date :
//		ThuyT					01/22/96
//
//	Description :
//		Implementation of the CPrjBuildSettingsTest class

#include "stdafx.h"
#include "prjcases.h"
#include "bldopts.h"
#include <io.h>
 

#define new DEBUG_NEW

#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;

IMPLEMENT_TEST(CPrjBuildSettingsTest, CTest, "Verify Build Setting Options", -1, CSniffDriver)

////////////////////////////////////////////////////////////////////
//	CPrjBuildSettingsTest

void CPrjBuildSettingsTest::Run(void)
{

	XSAFETY;

	//CPrjUtil	PrjUtil;
	szDummyLibrary		 = "DLL\\Release\\DLL.lib";

	szBuildOutputFile	 = m_strCWD + PROJNAME + "\\" + BUILDRES;

	m_pLog->RecordInfo("Verifying build setting options...");
	EXPECT( OpenProject() );
	m_pLog->RecordInfo("Set intermediate directory.");
	SetIntermediateDir();
	m_pLog->RecordInfo("Set output directory.");
	SetOutDir(); 
	m_pLog->RecordInfo("Add additional library.");
	AddLibrary();
	m_pLog->RecordInfo("Build the project...");
	BuildProject();
	m_pLog->RecordInfo("Verify the results...");
	VerifyIntermediateDirectory();
	VerifyOutDirectory();
	VerifyLinkerLibrary();
	EXPECT( CloseProject() );
	m_pLog->RecordInfo("Successfully verify build setting options.");
}

BOOL CPrjBuildSettingsTest::AddLibrary( void )
{
	if (!VERIFY_TEST_SUCCESS( prj.SetProjProperty(CT_LINKER, OPTNLINK_IDC_LIBS, szDummyLibrary))) {
		m_pLog->RecordFailure("Add a library to link");
		return FALSE;
	}
	return TRUE;
}

BOOL CPrjBuildSettingsTest::SetIntermediateDir( void )
{
	if (!VERIFY_TEST_SUCCESS( prj.SetProjProperty(CT_GENERAL,VPROJ_IDC_OUTDIRS_INT, NEWEXEINTDIR))) {
		m_pLog->RecordFailure("Change the build intermediate directory from the default");
		return FALSE;
	}
	return TRUE;
}

BOOL CPrjBuildSettingsTest::SetOutDir( void )
{
	if (!VERIFY_TEST_SUCCESS( prj.SetProjProperty(CT_GENERAL,VPROJ_IDC_OUTDIRS_TRG, NEWEXEOUTDIR))) {
		m_pLog->RecordFailure("Change the build target directory from the default");
		return FALSE;
	}
	return TRUE;
}


BOOL CPrjBuildSettingsTest::VerifyIntermediateDirectory(void)
{
	//	Check to see if the output directory was created
	CString strPath;

	strPath.Format("%s%s\\*.*", prj.GetDir(), NEWEXEINTDIR);

	WIN32_FIND_DATA fdTemp;
	HANDLE hdFile = FindFirstFile(strPath, &fdTemp);

	if (hdFile == INVALID_HANDLE_VALUE)
	{
		m_pLog->RecordFailure("Intermediate directory %s was not passed to the build tools", strPath);
		return FALSE;
	}
	FindClose(hdFile);
	return TRUE;

}

BOOL CPrjBuildSettingsTest::VerifyOutDirectory(void)
{
	CString strPath;

	strPath.Format("%s%s\\*.*", prj.GetDir(), NEWEXEOUTDIR);

	WIN32_FIND_DATA fdTemp;
	HANDLE hdFile = FindFirstFile(strPath, &fdTemp);

	if (hdFile == INVALID_HANDLE_VALUE)
	{
		m_pLog->RecordFailure("Target directory %s was not passed to the build tools", strPath);
		return FALSE;
	}
	FindClose(hdFile);
	return TRUE;

}

BOOL CPrjBuildSettingsTest::VerifyLinkerLibrary(void)
{
	CString strTemp = prj.GetProjProperty(CT_LINKER, OPTNLINK_IDC_LIBS);
	// Lets check to see if the data was saved during the whole process. 
	if (szDummyLibrary == strTemp)
	{
		return TRUE;
	}
	else
	{
		m_pLog->RecordFailure("Library options was not saved in Project Settings");
		return FALSE;
	}
}

BOOL CPrjBuildSettingsTest::OpenProject( void  )
{
	if (!VERIFY_TEST_SUCCESS( prj.Open(m_strCWD + PROJNAME + "\\" + PROJNAME)))
	{
		m_pLog->RecordFailure("Open the project");
		return FALSE;
	}
	// in some cases the damm Source Safe connect dialog will come up
	// lets get rid of it. 
	//if(MST.WButtonExists(GetLabel(IDCANCEL)))
	//	MST.WButtonClick(GetLabel(IDCANCEL));

	return TRUE;
}

BOOL CPrjBuildSettingsTest::BuildProject( void )
{
	int iWaitForBuild = 3;	// Minutes to wait for the build to complete
	
	if (!VERIFY_TEST_SUCCESS( prj.Build(iWaitForBuild))) {
		m_pLog->RecordFailure("Build Project");
		return FALSE;
	}
	return TRUE;
}

BOOL CPrjBuildSettingsTest::CloseProject( void )
{
	BOOL bSaveProjBeforeClose = TRUE; 
	
	if (!VERIFY_TEST_SUCCESS( prj.Close(bSaveProjBeforeClose))) {
		m_pLog->RecordFailure("Close the project");
		return FALSE;
	}
	return TRUE;
}	

