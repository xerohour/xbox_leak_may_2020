///////////////////////////////////////////////////////////////////////////////
//	AppWizCase.CPP
//											 
//	Created by :			
//		dklem
//
//	Description :								 
//		TestWizard Generated script		 

#include "stdafx.h"
#include "Appwizcase.h"
//#include "coprject.h"	
//#include "process.h"

#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
				 	 
											   
IMPLEMENT_TEST(/*999,*/ CAppWizTest, CIdeTestBase, "General", -1, CAppWizSubSuite)

// each function that runs a test has this prototype.
typedef void (CAppWizTest::*PTR_TEST_FUNC)(void); 

// Information about a test to be put in the test header
#define LOGTESTHEADER(strTestName) 	m_strLogInfo.Format("# - %d.  %s. Total # - %d, Passed - %d %%", ++m_TestNumber, strTestName, m_TotalNumberOfTests, (m_TestNumber*100)/m_TotalNumberOfTests);  \
									LogTestHeader(m_strLogInfo);

void CAppWizTest::PreRun(void)
{
	// call the base class
	CTest::PreRun();

}

void CAppWizTest::Run()
{
	CreateDirectory(TMP_PROJ_DIR, NULL);

	CreateWin32App();
	CreateWin32ConsoleApp();
	CreateWin32Dll();
	CreateWin32StaticLib();
	CreateXboxGame();
	CreateXboxStaticLib();
	CreateAndBuildXboxStaticLib();
	UseStaticLibDebug();
	UseStaticLibRelease();
}	

void RemovePreExistingProject(CProjWizOptions *pOptions)
{
	CString strDir = pOptions->m_strLocation + "\\" + pOptions->m_strName;

	KillFiles(strDir + "\\Debug\\", "*.*");
	KillFiles(strDir + "\\Release\\", "*.*");
	RemoveDirectory(strDir + "\\Debug");
	RemoveDirectory(strDir + "\\Release");
	KillFiles(strDir + "\\", "*.*");
	RemoveDirectory(strDir);
}

void CAppWizTest::SafeCloseProject()
{
	if (prj.IsOpen())
		prj.Close();
	else
	{
		LOG->RecordFailure("Possible failure to create project %s", prj.GetName());
		MST.DoKeys("+{B}+{B}+{B}{ESC}");
	}
}

///////////////////////////////////////////////////////////////////////////////
//	Test Cases
///////////////////////////////////////////////////////////////////////////////

void CAppWizTest::CreateWin32App(void)
{
	LOGTESTHEADER("CreateWin32App");
	// set the options for the exe project we are building for this test.
	CWin32AppWizOptions *pOptions = new(CWin32AppWizOptions);
	pOptions->m_strLocation = TMP_PROJ_DIR;
	pOptions->m_strName = "CreateWin32App Empty";
	pOptions->m_AppType = (CWin32AppWizOptions::APP_TYPE) 1;
	
	// create the exe project.
	RemovePreExistingProject(pOptions);
	EXPECT_SUCCESS(prj.New(pOptions));
	prj.Close();

	pOptions->m_strLocation = TMP_PROJ_DIR;
	pOptions->m_strName = "CreateWin32App Simple App";
	pOptions->m_AppType = (CWin32AppWizOptions::APP_TYPE) 2;
	
	// create the exe project.
	RemovePreExistingProject(pOptions);
	EXPECT_SUCCESS(prj.New(pOptions));
	EXPECT_SUCCESS(prj.Build());
	prj.Close();

	pOptions->m_strLocation = TMP_PROJ_DIR;
	pOptions->m_strName = "CreateWin32App Hello World";
	pOptions->m_AppType = (CWin32AppWizOptions::APP_TYPE) 3;
	
	// create the exe project.
	RemovePreExistingProject(pOptions);
	EXPECT_SUCCESS(prj.New(pOptions));
	EXPECT_SUCCESS(prj.Build());
	prj.Close();
}

void CAppWizTest::CreateWin32ConsoleApp(void)
{

	LOGTESTHEADER("CreateWin32ConsoleApp");
}

void CAppWizTest::CreateWin32Dll(void)
{
	LOGTESTHEADER("CreateWin32Dll");

	// set the options for the dll project we are building for this test.
	CWin32DllWizOptions *pOptions = new(CWin32DllWizOptions);
	pOptions->m_strLocation = TMP_PROJ_DIR;
	pOptions->m_strName = "CreateWin32Dll Empty";
	pOptions->m_DllType = (CWin32DllWizOptions::DLL_TYPE) 1;
	
	// create the dll project.
	RemovePreExistingProject(pOptions);
	EXPECT_SUCCESS(prj.New(pOptions));
	SafeCloseProject();

	pOptions->m_strLocation = TMP_PROJ_DIR;
	pOptions->m_strName = "CreateWin32Dll Simple Dll";
	pOptions->m_DllType = (CWin32DllWizOptions::DLL_TYPE) 2;
	
	// create the dll project.
	RemovePreExistingProject(pOptions);
	EXPECT_SUCCESS(prj.New(pOptions));
	EXPECT_SUCCESS(prj.Build());
	SafeCloseProject();

	pOptions->m_strLocation = TMP_PROJ_DIR;
	pOptions->m_strName = "CreateWin32Dll Dll Exports";
	pOptions->m_DllType = (CWin32DllWizOptions::DLL_TYPE) 3;
	
	// create the dll project.
	RemovePreExistingProject(pOptions);
	EXPECT_SUCCESS(prj.New(pOptions));
	EXPECT_SUCCESS(prj.Build());
	SafeCloseProject();
}

void CAppWizTest::CreateWin32StaticLib(void)
{

	LOGTESTHEADER("CreateWin32StaticLib");
}

void CAppWizTest::CreateXboxGame(void)
{
	LOGTESTHEADER("CreateXboxGame");
	// set the options for the exe project we are building for this test.
	CXboxGameOptions *pOptions = new(CXboxGameOptions);
	pOptions->m_strLocation = TMP_PROJ_DIR;
	pOptions->m_strName = "CreateXboxGame";
	
	// create the exe project.
	RemovePreExistingProject(pOptions);
	EXPECT_SUCCESS(prj.New(pOptions));
	SafeCloseProject();
}

void CAppWizTest::CreateXboxStaticLib(void)
{
	LOGTESTHEADER("CreateXboxStaticLib");
	// set the options for the exe project we are building for this test.
	CXboxStaticLibOptions *pOptions = new(CXboxStaticLibOptions);
	pOptions->m_strLocation = TMP_PROJ_DIR;
	pOptions->m_strName = "CreateXboxStaticLib";
	
	// create the exe project.
	RemovePreExistingProject(pOptions);
	EXPECT_SUCCESS(prj.New(pOptions));
	SafeCloseProject();
}

void CAppWizTest::CreateAndBuildXboxStaticLib(void)
{
	LOGTESTHEADER("CreateAndBuildXboxStaticLib");
	// set the options for the exe project we are building for this test.
	CXboxStaticLibOptions *pOptions = new(CXboxStaticLibOptions);
	pOptions->m_strLocation = TMP_PROJ_DIR;
	pOptions->m_strName = "CreateAndBuildXboxStaticLib";

	// create the exe project.
	RemovePreExistingProject(pOptions);
	EXPECT_SUCCESS(prj.New(pOptions));

	// where is the common stuff?
	CString cstrXDK;
	GetEnvironmentVariable("XDK", cstrXDK.GetBufferSetLength(1024), 1024);
	cstrXDK.ReleaseBuffer();

	EXPECT_SUCCESS(prj.AddFiles(cstrXDK + "\\Samples\\Xbox\\Common\\Src\\xbapp.cpp"));
	EXPECT_SUCCESS(prj.AddFiles(cstrXDK + "\\Samples\\Xbox\\Common\\Src\\xbfont.cpp"));
	EXPECT_SUCCESS(prj.AddFiles(cstrXDK + "\\Samples\\Xbox\\Common\\Src\\xbhelp.cpp"));
	EXPECT_SUCCESS(prj.AddFiles(cstrXDK + "\\Samples\\Xbox\\Common\\Src\\xbutil.cpp"));
	EXPECT_SUCCESS(prj.AddFiles(cstrXDK + "\\Samples\\Xbox\\Common\\Src\\xbinput.cpp"));
	EXPECT_SUCCESS(prj.AddFiles(cstrXDK + "\\Samples\\Xbox\\Common\\Src\\xbmesh.cpp"));
	EXPECT_SUCCESS(prj.AddFiles(cstrXDK + "\\Samples\\Xbox\\Common\\Src\\xbresource.cpp"));

	prj.SetProjProperty(CT_COMPILER, CL_IDC_ADD_INCLUDE, cstrXDK + "\\Samples\\Xbox\\Common\\Include");
	EXPECT_SUCCESS(prj.Build());
	EXPECT_SUCCESS(prj.VerifyBuild());

	// try the release configuration
	EXPECT_SUCCESS(prj.SetTarget(pOptions->m_strName + " - Xbox Release"));

	prj.SetProjProperty(CT_COMPILER, CL_IDC_ADD_INCLUDE, cstrXDK + "\\Samples\\Xbox\\Common\\Include");
	EXPECT_SUCCESS(prj.Build());
	EXPECT_SUCCESS(prj.VerifyBuild());

	SafeCloseProject();
}

#define STATIC_LIB			"CreateAndBuildXboxStaticLib"
#define STATIC_LIB_DBG_DIR	TMP_PROJ_DIR "\\" STATIC_LIB "\\Debug"
#define STATIC_LIB_DBG		STATIC_LIB_DBG_DIR "\\" STATIC_LIB ".lib"

void CAppWizTest::UseStaticLibDebug(void)
{
	// make sure we have a lib to link to
	FILE *f = fopen(STATIC_LIB_DBG, "r");
	if (!f)
	{
		CreateAndBuildXboxStaticLib();
		f = fopen(STATIC_LIB_DBG, "r");
		if (!f)
			return;
	}
	fclose(f);

	LOGTESTHEADER("UseStaticLibDebug");
	// set the options for the exe project we are building for this test.
	CXboxGameOptions *pOptions = new(CXboxGameOptions);
	pOptions->m_strLocation = TMP_PROJ_DIR;
	pOptions->m_strName = "UseStaticLibDebug";

	// create the exe project.
	RemovePreExistingProject(pOptions);
	EXPECT_SUCCESS(prj.New(pOptions));

	// where is the common stuff?
	CString cstrXDK;
	GetEnvironmentVariable("XDK", cstrXDK.GetBufferSetLength(1024), 1024);
	cstrXDK.ReleaseBuffer();

	EXPECT_SUCCESS(prj.AddFiles(cstrXDK + "\\Samples\\Xbox\\Graphics\\Cartoon\\cartoon.cpp"));

	// add include path
	prj.SetProjProperty(CT_COMPILER, CL_IDC_ADD_INCLUDE, cstrXDK + "\\Samples\\Xbox\\Common\\Include");

	// add library
	CString strDefaultLibs = prj.GetProjProperty(CT_LINKER, LNK_IDC_MODULES);
	prj.SetProjProperty(CT_LINKER, LNK_IDC_MODULES, strDefaultLibs + STATIC_LIB + ".lib");	
	prj.SetProjProperty(CT_LINKER, LNK_IDC_ADDL_LIB_PATH, STATIC_LIB_DBG_DIR);

	// turn off autocopy
	EXPECT_SUCCESS(prj.SetProjProperty(CT_IMAGEBLD, IMAGEBLD_IDC_AUTOCOPY_OFF, 1));
	EXPECT_SUCCESS(prj.Build());
	EXPECT_SUCCESS(prj.VerifyBuild());
	SafeCloseProject();
}

#define STATIC_LIB_REL_DIR	TMP_PROJ_DIR "\\" STATIC_LIB "\\Release"
#define STATIC_LIB_REL		STATIC_LIB_REL_DIR "\\" STATIC_LIB ".lib"

void CAppWizTest::UseStaticLibRelease(void)
{
	// make sure we have a lib to link to
	FILE *f = fopen(STATIC_LIB_REL, "r");
	if (!f)
	{
		CreateAndBuildXboxStaticLib();
		f = fopen(STATIC_LIB_REL, "r");
		if (!f)
			return;
	}
	fclose(f);

	LOGTESTHEADER("UseStaticLibRelease");
	// set the options for the exe project we are building for this test.
	CXboxGameOptions *pOptions = new(CXboxGameOptions);
	pOptions->m_strLocation = TMP_PROJ_DIR;
	pOptions->m_strName = "UseStaticLibRelease";

	// create the exe project.
	RemovePreExistingProject(pOptions);
	EXPECT_SUCCESS(prj.New(pOptions));

	// set target to release
	prj.SetTarget(pOptions->m_strName + " - Xbox Release");

	// where is the common stuff?
	CString cstrXDK;
	GetEnvironmentVariable("XDK", cstrXDK.GetBufferSetLength(1024), 1024);
	cstrXDK.ReleaseBuffer();

	EXPECT_SUCCESS(prj.AddFiles(cstrXDK + "\\Samples\\Xbox\\Graphics\\Cartoon\\cartoon.cpp"));

	// add include path
	prj.SetProjProperty(CT_COMPILER, CL_IDC_ADD_INCLUDE, cstrXDK + "\\Samples\\Xbox\\Common\\Include");

	// add library
	CString strDefaultLibs = prj.GetProjProperty(CT_LINKER, LNK_IDC_MODULES);
	prj.SetProjProperty(CT_LINKER, LNK_IDC_MODULES, strDefaultLibs + STATIC_LIB + ".lib");	
	prj.SetProjProperty(CT_LINKER, LNK_IDC_ADDL_LIB_PATH, STATIC_LIB_REL_DIR);

	// turn off autocopy
	EXPECT_SUCCESS(prj.SetProjProperty(CT_IMAGEBLD, IMAGEBLD_IDC_AUTOCOPY_OFF, 1));
	EXPECT_SUCCESS(prj.Build());
	EXPECT_SUCCESS(prj.VerifyBuild());
	SafeCloseProject();
}
