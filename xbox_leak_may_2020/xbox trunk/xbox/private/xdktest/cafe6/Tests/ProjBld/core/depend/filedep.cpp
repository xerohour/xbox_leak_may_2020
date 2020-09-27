///////////////////////////////////////////////////////////////////////////////
//	Filedep.cpp
//
//	Created by :			Date :
//		t-seanc					8/12/97
//
//	Description :
//		Implementation of the CFileDependencies class

#include "stdafx.h"
#include "filedep.h"

#include <io.h>
#include <sys/utime.h>
#include <sys/stat.h> 

#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;

IMPLEMENT_TEST(CFileDependencies, CProjectTestSet, "Dependencies of Files", -1, CDependenciesSubSuite)

void CFileDependencies::PreRun(void)
	{
	// call the base class
	CTest::PreRun();
	}

////////////////////////////////////////////////////////////////////
//	CDependenciesTest1

void CFileDependencies::Run(void)
	{	 
	XSAFETY;
	EXPECT_TRUE(FilesDependantOnHeaderFiles());
	XSAFETY;
	}

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Function: FilesDependantOnHeaderFiles

	This test makes sure that header file dependencies are correctly 
	recognized by the dependency scanner.  It opens the workspace Useless 
	and changes the active project to Worthless which contains a source 
	file that has several levels of nested include files, each of the 
	include files having many strange constructs within them.  It touches 
	each of those include files and rebuilds making sure that the build 
	occurs each time.

	After that, it removes dependencies from the project by removing
	#include directives, builds the project, touches the header files
	that were removed and makes sure a second rebuild does not occur.

	Return Value: BOOL

	This function returns TRUE if the test succeeds and FALSE if the 
	test fails.

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
BOOL CFileDependencies::FilesDependantOnHeaderFiles()
	{
	LogTestHeader(_T("Files dependant on header files"));
	
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

	// Change the active project to Worthless
	uiws.SetActiveProject(_T("Worthless"));

	// Build the project
	EXPECT_SUCCESS(prj.Build(5));
	EXPECT_TRUE(VerifyBuildOccured(TRUE));	

	// Touch every header and rebuild
	EXPECT_TRUE(TouchAndVerifyRebuild(szProjectPath + _T("Worthless")));
	EXPECT_TRUE(VerifyBuildString(_T("Worthless.c")));
	EXPECT_TRUE(TouchAndVerifyRebuild(szProjectPath + _T("Worthless2.h")));
	EXPECT_TRUE(VerifyBuildString(_T("Worthless.c")));
	EXPECT_TRUE(TouchAndVerifyRebuild(szProjectPath + _T("Worthless3.h")));
	EXPECT_TRUE(VerifyBuildString(_T("Worthless.c")));
	EXPECT_TRUE(TouchAndVerifyRebuild(szProjectPath + _T("Worthless4.h")));
	EXPECT_TRUE(VerifyBuildString(_T("Worthless.c")));
	EXPECT_TRUE(TouchAndVerifyRebuild(
		szProjectPath + 
		_T("This is another header file.Do you like it.I think it is neat")
		));
	EXPECT_TRUE(VerifyBuildString(_T("Worthless.c")));

	// Remove the dependency on "This is a header file", touch, and rebuild
	src.Open(szProjectPath + _T("Worthless4.h"));
	src.Replace(
		_T("#include.\\:q"), // search for the quoted string
		_T(" "), 
		TRUE, 
		TRUE, 
		TRUE, 
		COSource::RS_WHOLEFILE,
		TRUE
		);
	src.Close(TRUE);
	prj.Build(5);
	TouchAndVerifyRebuild(szProjectPath + _T("This is a header file"), FALSE);

	// *******
	// If you get a failuer here, take a look at BUG VS29120
	// *******

	// Remove the dependency on "Worthless4.h", touch, and rebuild
	src.Open(szProjectPath + _T("Worthless3.h"));
	src.Replace(
		_T("#include.\\:q"), 
		_T(" "), 
		TRUE,
		TRUE, 
		TRUE, 
		COSource::RS_WHOLEFILE,
		TRUE
		);
	src.Close(TRUE);
	prj.Build(5);
	TouchAndVerifyRebuild(szProjectPath + _T("Worthless4.h"), FALSE);
	
	// Remove the dependency on "Worthless3.h", touch, and rebuild
	src.Open(szProjectPath + _T("Worthless2.h"));
	src.Replace(
		_T("#include.\\:q"), 
		_T(" "), 
		TRUE, 
		TRUE, 
		TRUE, 
		COSource::RS_WHOLEFILE,
		TRUE
		);
	src.Close(TRUE);
	prj.Build(5);
	TouchAndVerifyRebuild(szProjectPath + _T("Worthless3.h"), FALSE);

	// Remove the dependency on "Worthless2.h", touch, and rebuild
	src.Open(szProjectPath + _T("Worthless"));
	src.Replace(
		_T("#include.\\:q"), 
		_T(" "), 
		TRUE, 
		TRUE, 
		TRUE, 
		COSource::RS_WHOLEFILE,
		TRUE
		);
	src.Close(TRUE);
	prj.Build(5);
	TouchAndVerifyRebuild(szProjectPath + _T("Worthless2.h"), FALSE);

	// Close the workspace
	prj.Close(0);
	
	return TRUE;
	}