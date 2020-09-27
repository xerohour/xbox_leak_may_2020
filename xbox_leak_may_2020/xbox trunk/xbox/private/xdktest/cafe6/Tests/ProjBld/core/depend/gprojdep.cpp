///////////////////////////////////////////////////////////////////////////////
//	Filedep.cpp
//
//	Created by :			Date :
//		t-seanc					8/12/97
//
//	Description :
//		Implementation of the CGenProjectDependencies class

#include "stdafx.h"
#include "gprojdep.h"

#include <io.h>
#include <sys/utime.h>
#include <sys/stat.h> 

#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;

IMPLEMENT_TEST(
	CGenProjectDependencies, 
	CProjectTestSet, 
	"Dependencies of Generic Projects", 
	-1, 
	CDependenciesSubSuite
	)

void CGenProjectDependencies::PreRun(void)
	{
	// call the base class
	CTest::PreRun();
	}

////////////////////////////////////////////////////////////////////
//	CDependenciesTest1

void CGenProjectDependencies::Run(void)
	{	 
	XSAFETY;
	EXPECT_TRUE(GenProjectsDependantOnProjects());
	EXPECT_TRUE(GenProjectsDependantOnGenProjects());
	XSAFETY;
	}

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Function: GenProjectsDependantOnProjects

	This test opens the workspace Useless and sets the active project to a 
	generic project named Generic.  Generic is dependant on several other 
	projects in the workspace.	This test touches many of the files that 
	Generic is dependant on	and then rebuilds, making sure that the build 
	occurs in every case.

	Return Value: BOOL

	This function returns TRUE if the test succeeds and FALSE if the 
	test fails.

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
BOOL CGenProjectDependencies::GenProjectsDependantOnProjects()
	{
	LogTestHeader(_T("Generic projects dependant on projects"));
	
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

	// Change the active project to Generic
	uiws.SetActiveProject(_T("Generic"));

	// Build the project
	EXPECT_SUCCESS(prj.Build(5));

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

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Function: GenProjectsDependantOnGenProjects

	This test opens a workspace Useless and sets the active project to a 
	generic project named Another Generic Project.  Another Generic Project 
	includes the generic project Generic.  This test then touches many of 
	the files that Another Generic Project is dependant on and rebuilds the 
	project, making sure the build occurs each time.

	Return Value: BOOL

	This function returns TRUE if the test succeeds and FALSE if the 
	test fails.

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
BOOL CGenProjectDependencies::GenProjectsDependantOnGenProjects()
	{	
	LogTestHeader(_T("Generic projects dependant on generic projects"));

	UIWorkspaceWindow uiws;

	// Create string containing the path to the Useless tree
	CString szProjectPath = m_strCWD + _T("src\\") + _T("useless\\");

	// Open the workspace
	prj.Open(szProjectPath + _T("Useless.dsw"));

	// Change the active project to Another Generic Project
	uiws.SetActiveProject(_T("Another Generic Project"));

	// Build the project
	EXPECT_SUCCESS(prj.Build(5));

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