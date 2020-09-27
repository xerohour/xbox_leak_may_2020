///////////////////////////////////////////////////////////////////////////////
//	dep1.CPP
//
//	Created by :			Date :
//		YefimS					5/2/97
//
//	Description :
//		Implementation of the CDependenciesTest1 class

#include "stdafx.h"
#include "dep1.h"

#include <io.h>
#include <sys/utime.h>
#include <sys/stat.h> 

#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;

IMPLEMENT_TEST(CDependenciesTest1, CProjectTestSet, "Basic Dependency Test", -1, CDependenciesSubSuite)

void CDependenciesTest1::PreRun(void)
{
	// call the base class
	CTest::PreRun();
}

////////////////////////////////////////////////////////////////////
//	CDependenciesTest1

void CDependenciesTest1::Run(void)
{	 
	XSAFETY;
	EXPECT_TRUE( BasicDependencyTest() );
	XSAFETY;

}


/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	BasicDependencyTest

	Makes sure all simple dependency relationships (File, Project, Generic
	Project) work.

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
BOOL CDependenciesTest1::BasicDependencyTest( void )
	{
	// Write out the test header
	LogTestHeader("Basic Dependency Test");

	// Copy the files from the clean directory
	CString szProjPath = m_strCWD + _T("src\\") + _T("Useless\\");
	CString szCleanProjPath = szProjPath + _T("new\\");
	m_pLog->RecordInfo(szProjPath);
	m_pLog->RecordInfo(szCleanProjPath);
	KillFile(szProjPath, _T("*.*"));
	KillAllFiles(szProjPath + _T("Release\\"), TRUE);
	EXPECT_TRUE(CopyProjectSources(szCleanProjPath, szProjPath));

	// Open the Useless workspace
	EXPECT_SUCCESS(prj.Open(szProjPath + "Useless.dsw", 0));

	// Set the active project to Useless and build
	MST.DoKeys("%(BO)");
	MST.DoKeys("U");
	MST.DoKeys("{ENTER}");
	EXPECT_SUCCESS(prj.Build(5));
	EXPECT_TRUE(VerifyBuildOccured(TRUE));	

	// Check to make sure dependencies are correctly understood
	TouchAndBuild(szProjPath + _T("Useless.h"));
	VerifyTextInBuildLog(_T("Useless"));
	TouchAndBuild(szProjPath + _T("Nothing.h"));

	// Close the project
	EXPECT_SUCCESS(prj.Close(0));

	return TRUE;
	}

BOOL CDependenciesTest1::ProjectDependencies()
	{
	// Touch some other projects and make sure the results are correct
	TouchAndBuild(szProjPath + _T("Nothing.cpp"));
	TouchAndBuild(szProjPath + _T("Resource1.rc"));
	TouchAndBuild(szProjPath + _T("Worthless.h"), FALSE);

	return TRUE;
	}

BOOL CDependenciesTest1::GenericProjectDependencies()
	{
	// Set the active project to Generic and build
	MST.DoKeys("%(BO)");
	MST.DoKeys("G");
	MST.DoKeys("{ENTER}");
	EXPECT_SUCCESS(prj.Build(5));

	// Check to make sure dependants are recorded
	TouchAndBuild(szProjPath + _T("Useless.h"));
	TouchAndBuild(szProjPath + _T("Nothing.h"));
	TouchAndBuild(szProjPath + _T("Worthless.h"));

	return TRUE;
	}

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	TouchAndBuild

	Description:
	
	Touches the specified file and the rebuilds the current active project.
	
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
void CDependenciesTest1::TouchAndBuild
	(
	CString szFileToTouch,
	BOOL	bExpectRebuild /* = true */
	)	

	{
	// Set the modification time of the file to the current time
	_utime(szFileToTouch, NULL);

	// Build the active project
	EXPECT_SUCCESS(prj.Build(5));
	EXPECT_TRUE(VerifyBuildOccured(bExpectRebuild));
	}

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	VerifyTextInBuildLog

	Description:
	
	Checks to see if the specified text is found in the build log.  If it
	is not, a failure is recorded.
  
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
BOOL CDependenciesTest1::VerifyTextInBuildLog
	(
	CString szTextToFind
	)

	{
	// Get the build log
	CString BuildLog = GetBldLog();

	// Search for the text
	szTextToFind.MakeLower();
	if(BuildLog.Find(szTextToFind) == -1)
		{
		m_pLog->RecordFailure(_T("Could not find ") + szTextToFind + _T(" in build log."));
		return FALSE;
		}
	else 
		return TRUE;
	}


