///////////////////////////////////////////////////////////////////////////////
//	Filedep.cpp
//
//	Created by :			Date :
//		t-seanc					8/12/97
//
//	Description :
//		Implementation of the CProjectDependencies class

#include "stdafx.h"
#include "projdep.h"

#include <io.h>
#include <sys/utime.h>
#include <sys/stat.h> 

#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;

IMPLEMENT_TEST(CProjectDependencies, CProjectTestSet, "Dependencies of Projects", -1, CDependenciesSubSuite)

void CProjectDependencies::PreRun(void)
	{
	// call the base class
	CTest::PreRun();
	}

////////////////////////////////////////////////////////////////////
//	CDependenciesTest1

void CProjectDependencies::Run(void)
	{	 
	XSAFETY;
	EXPECT_TRUE(ProjectsDependantOnFiles());
	EXPECT_TRUE(ProjectsDependantOnProjects());
	EXPECT_TRUE(ProjectsDependantOnGenProjects());
	XSAFETY;
	}

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Function: ProjectsDependantOnFiles

	This test opens the workspace Useless, and sets the active project
	to Useless which contains several source files.  The test touches 
	each of these source files and rebuilds, checking to make sure the 
	build occurs in every case.

	Return Value: BOOL

	This function returns TRUE if the test succeeds and FALSE if the 
	test fails.

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
BOOL CProjectDependencies::ProjectsDependantOnFiles()
	{	
	LogTestHeader(_T("Projects dependant on files"));

	UIWorkspaceWindow uiws;

	// Create string containing the path to the Useless tree
	CString szProjectPath = m_strCWD + _T("src\\") + _T("useless\\");

	// Create a clean copy of the test workspace tree
	KillFile(szProjectPath, _T("*.*"));
	KillAllFiles(szProjectPath + _T("Debug\\"), TRUE);
	KillAllFiles(szProjectPath + _T("Release\\"), TRUE);
	CopyProjectSources(szProjectPath + _T("new\\"), szProjectPath);

	// Open the workspace
	prj.Open(szProjectPath + _T("Useless.dsw"));

	// Change the active project to Useless
	uiws.SetActiveProject(_T("Useless"));

	// Touch everything in the project Useless
	EXPECT_TRUE(TouchAndVerifyRebuild(szProjectPath + _T("Useless.cpp")));
	EXPECT_TRUE(VerifyBuildString(_T("Useless.cpp")));
	EXPECT_TRUE(TouchAndVerifyRebuild(szProjectPath + _T("Useless.h")));
	EXPECT_TRUE(VerifyBuildString(_T("Useless.cpp")));
	EXPECT_TRUE(TouchAndVerifyRebuild(szProjectPath + _T("Resource1.rc")));
	EXPECT_TRUE(VerifyBuildString(GetLocString(IDSS_OUTPUTWIN_RC_COMPILING)));
	EXPECT_TRUE(TouchAndVerifyRebuild(szProjectPath + _T("This file has a really long name and I like that fact.cpp")));
	EXPECT_TRUE(VerifyBuildString(_T("This file has a really long name and I like that fact.cpp")));
	
	// Close the project
	prj.Close(0);
	
	return TRUE;
	}

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Function: ProjectsDependantOnProjects

	This test opens the workspace Useless and sets the active project to
	Useless which is dependant on the project Nothing.  It then touches 
	every file in Nothing and rebuilds, making sure the build occurs in 
	every case.

	Return Value: BOOL

	This function returns TRUE if the test succeeds and FALSE if the 
	test fails.

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
BOOL CProjectDependencies::ProjectsDependantOnProjects()
	{
	LogTestHeader(_T("Projects dependant on projects"));
	
	UIWorkspaceWindow uiws;
	
	// Create string containing the path to the Useless tree
	CString szProjectPath = m_strCWD + _T("src\\") + _T("useless\\");

	// Open the workspace
	prj.Open(szProjectPath + _T("Useless.dsw"));

	// Change the active project to Useless
	uiws.SetActiveProject(_T("Useless"));

	// Touch everything the in the project Nothing
	EXPECT_TRUE(TouchAndVerifyRebuild(szProjectPath + _T("Nothing.cpp")));
	EXPECT_TRUE(VerifyBuildString(_T("Nothing.cpp")));
	EXPECT_TRUE(TouchAndVerifyRebuild(szProjectPath + _T("Nothing.h")));
	EXPECT_TRUE(VerifyBuildString(_T("Nothing.cpp")));
	EXPECT_TRUE(VerifyBuildString(_T("Nothing2.cpp")));
	EXPECT_TRUE(VerifyBuildString(_T("Nothing3.cpp"), FALSE));
	EXPECT_TRUE(TouchAndVerifyRebuild(szProjectPath + _T("Precomp.h")));
	EXPECT_TRUE(VerifyBuildString(_T("Nothing.cpp")));
	EXPECT_TRUE(VerifyBuildString(_T("Nothing2.cpp")));
	EXPECT_TRUE(VerifyBuildString(_T("Nothing3.cpp")));
	EXPECT_TRUE(TouchAndVerifyRebuild(szProjectPath + _T("Nothing2.cpp")));
	EXPECT_TRUE(VerifyBuildString(_T("Nothing2.cpp")));

	// Close the workspace
	prj.Close(0);
	
	return TRUE;
	}

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Function: ProjectsDependantOnGenProjects

	This test opens the workspace Useless and set the active project to 
	Pointless which is dependant on the generic project Another Generic 
	Project.  It then touches several of the files that Another Generic 
	Project is dependant on and rebuilds making sure the build occurs in 
	every case.

	Return Value: BOOL

	This function returns TRUE if the test succeeds and FALSE if the 
	test fails.

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
BOOL CProjectDependencies::ProjectsDependantOnGenProjects()
	{
	LogTestHeader(_T("Projects dependant on generic projects"));

	UIWorkspaceWindow uiws;

	// Create string containing the path to the Useless tree
	CString szProjectPath = m_strCWD + _T("src\\") + _T("useless\\");

	// Open the workspace
	prj.Open(szProjectPath + _T("Useless.dsw"));

	// Change the active project to Pointless
	uiws.SetActiveProject(_T("Pointless"));

	// Touch everything the in the project Nothing
	EXPECT_TRUE(TouchAndVerifyRebuild(szProjectPath + _T("Nothing.cpp")));
	EXPECT_TRUE(VerifyBuildString(_T("Nothing.cpp")));
	EXPECT_TRUE(TouchAndVerifyRebuild(szProjectPath + _T("Nothing.h")));
	EXPECT_TRUE(VerifyBuildString(_T("Nothing.cpp")));
	EXPECT_TRUE(VerifyBuildString(_T("Nothing2.cpp")));
	EXPECT_TRUE(VerifyBuildString(_T("Nothing3.cpp"), FALSE));
	EXPECT_TRUE(TouchAndVerifyRebuild(szProjectPath + _T("Precomp.h")));
	EXPECT_TRUE(VerifyBuildString(_T("Nothing.cpp")));
	EXPECT_TRUE(VerifyBuildString(_T("Nothing2.cpp")));
	EXPECT_TRUE(VerifyBuildString(_T("Nothing3.cpp")));
	EXPECT_TRUE(TouchAndVerifyRebuild(szProjectPath + _T("Nothing2.cpp")));
	EXPECT_TRUE(VerifyBuildString(_T("Nothing2.cpp")));

	// Touch everything the in the project Worthless
	EXPECT_TRUE(TouchAndVerifyRebuild(szProjectPath + _T("Worthless")));
	EXPECT_TRUE(VerifyBuildString(_T("Worthless.c")));
	EXPECT_TRUE(TouchAndVerifyRebuild(szProjectPath + _T("Worthless2.h")));
	EXPECT_TRUE(VerifyBuildString(_T("Worthless.c")));
	EXPECT_TRUE(TouchAndVerifyRebuild(szProjectPath + _T("Worthless3.h")));
	EXPECT_TRUE(VerifyBuildString(_T("Worthless.c")));
	EXPECT_TRUE(TouchAndVerifyRebuild(szProjectPath + _T("Worthless4.h")));
	EXPECT_TRUE(VerifyBuildString(_T("Worthless.c")));

	// Touch everything in the project Useless
	EXPECT_TRUE(TouchAndVerifyRebuild(szProjectPath + _T("Useless.cpp")));
	EXPECT_TRUE(VerifyBuildString(_T("Useless.cpp")));
	EXPECT_TRUE(TouchAndVerifyRebuild(szProjectPath + _T("Useless.h")));
	EXPECT_TRUE(VerifyBuildString(_T("Useless.cpp")));
	EXPECT_TRUE(TouchAndVerifyRebuild(szProjectPath + _T("Resource1.rc")));
	EXPECT_TRUE(VerifyBuildString(GetLocString(IDSS_OUTPUTWIN_RC_COMPILING)));
	EXPECT_TRUE(TouchAndVerifyRebuild(szProjectPath + _T("This file has a really long name and I like that fact.cpp")));
	EXPECT_TRUE(VerifyBuildString(_T("This file has a really long name and I like that fact.cpp")));

	// Close the workspace
	prj.Close(0);

	return TRUE;
	}
