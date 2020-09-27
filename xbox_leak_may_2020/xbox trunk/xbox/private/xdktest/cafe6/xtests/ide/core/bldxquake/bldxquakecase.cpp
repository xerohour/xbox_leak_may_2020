///////////////////////////////////////////////////////////////////////////////
//	BldXQuakeCase.CPP
//											 
//	Created by :			
//		dklem
//
//	Description :								 
//		TestWizard Generated script		 

#include "stdafx.h"
#include "BldXQuakeCase.h"
//#include "coprject.h"	
//#include "process.h"
#include "xboxdbg.h"

#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
				 	 
											   
IMPLEMENT_TEST(/*999,*/ CBldXQuakeTest, CIdeTestBase, "General", -1, CBldXQuakeSubSuite)

// each function that runs a test has this prototype.
typedef void (CBldXQuakeTest::*PTR_TEST_FUNC)(void); 

// Information about a test to be put in the test header
#define LOGTESTHEADER(strTestName) 	m_strLogInfo.Format("# - %d.  %s. Total # - %d, Passed - %d %%", ++m_TestNumber, strTestName, m_TotalNumberOfTests, (m_TestNumber*100)/m_TotalNumberOfTests);  \
									LogTestHeader(m_strLogInfo);

void CBldXQuakeTest::PreRun(void)
{
	// call the base class
	CTest::PreRun();

}

void CBldXQuakeTest::Run()
{
	CreateDirectory(TMP_PROJ_DIR, NULL);
	BuildXQuake();
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

void CBldXQuakeTest::SafeCloseProject()
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

#define QUAKE_PROJ_NAME		"XQuake"
#define QUAKE_ROOT			REMOTE_XBOX_DIR "\\" QUAKE_PROJ_NAME
#define QUAKE_MEDIA			QUAKE_ROOT "\\media"
#define QUAKE_DBG_XBE		QUAKE_ROOT "\\" QUAKE_PROJ_NAME "D.xbe"
#define QUAKE_RLS_XBE		QUAKE_ROOT "\\" QUAKE_PROJ_NAME "R.xbe"

void CBldXQuakeTest::BuildXQuake(void)
{
	LOGTESTHEADER("BuildXQuake");

	// set the options for the exe project we are building for this test.
	CXboxGameOptions *pOptions = new(CXboxGameOptions);
	pOptions->m_strLocation = TMP_PROJ_DIR;
	pOptions->m_strName = QUAKE_PROJ_NAME;

	// create the exe project.
	RemovePreExistingProject(pOptions);
	EXPECT_SUCCESS(prj.New(pOptions));

	// where are the files?
	CString cstrNTDrive;
	GetEnvironmentVariable("_NTDRIVE", cstrNTDrive.GetBufferSetLength(1024), 1024);
	cstrNTDrive.ReleaseBuffer();

	CString cstrNTRoot;
	GetEnvironmentVariable("_NTROOT", cstrNTRoot.GetBufferSetLength(1024), 1024);
	cstrNTRoot.ReleaseBuffer();

	CString cstrSources;
	cstrSources = cstrNTDrive + cstrNTRoot + "\\private\\test\\games\\xquake";

	CFileFind finder;
	BOOL bWorking = finder.FindFile(cstrSources + "\\*.cpp");
	while (bWorking)
	{
		bWorking = finder.FindNextFile();
		EXPECT_SUCCESS(prj.AddFiles(finder.GetFilePath()));
	}

	bWorking = finder.FindFile(cstrSources + "\\*.c");
	while (bWorking)
	{
		bWorking = finder.FindNextFile();
		if (finder.GetFileName() != CString("net_ipx.c"))
		EXPECT_SUCCESS(prj.AddFiles(finder.GetFilePath()));
	}

	bWorking = finder.FindFile(cstrSources + "\\*.h");
	while (bWorking)
	{
		bWorking = finder.FindNextFile();
		EXPECT_SUCCESS(prj.AddFiles(finder.GetFilePath()));
	}

	////////////////////////////////////////////////////////////
	// build debug
	//
	EXPECT_SUCCESS(prj.SetTarget(QUAKE_PROJ_NAME " - Xbox Debug"));

	// set defines
	CString strDefines = prj.GetProjProperty(CT_COMPILER, CL_IDC_PREPROC_DEF);
	prj.SetProjProperty(CT_COMPILER, CL_IDC_PREPROC_DEF,
		strDefines + ",XBOX,NO_ASSEMBLY,NO_MGRAPH,GLQUAKE,D3DQUAKE");	

	// add include path
	prj.SetProjProperty(CT_COMPILER, CL_IDC_ADD_INCLUDE, cstrSources);

	// add library
	CString strDefaultLibs = prj.GetProjProperty(CT_LINKER, LNK_IDC_MODULES);
	prj.SetProjProperty(CT_LINKER, LNK_IDC_MODULES,
		strDefaultLibs + "xbdm.lib");	

	// turn off autocopy
	EXPECT_SUCCESS(prj.SetProjProperty(CT_IMAGEBLD, IMAGEBLD_IDC_AUTOCOPY_OFF, 1));
	EXPECT_SUCCESS(prj.Build());
	EXPECT_SUCCESS(prj.VerifyBuild());

	DmMkdir(REMOTE_XBOX_DIR);
	DmMkdir(QUAKE_ROOT);
	DmMkdir(QUAKE_MEDIA);

	CString strXBE = CString(prj.GetDir()) + "Debug\\" QUAKE_PROJ_NAME ".xbe";
	HRESULT hr = DmSendFile(strXBE, QUAKE_DBG_XBE);
	if (hr != XBDM_NOERR)
	{
		LOG->RecordFailure("BuildXQuake() failed, DmSendFile(\"%s\", \"%s\") returned %08X",
			strXBE, QUAKE_DBG_XBE, hr);
		return;
	}


	////////////////////////////////////////////////////////////
	// build release
	//
	EXPECT_SUCCESS(prj.SetTarget("XQuake - Xbox Release"));

	// set defines
	strDefines = prj.GetProjProperty(CT_COMPILER, CL_IDC_PREPROC_DEF);
	prj.SetProjProperty(CT_COMPILER, CL_IDC_PREPROC_DEF,
		strDefines + ",XBOX,NO_ASSEMBLY,NO_MGRAPH,GLQUAKE,D3DQUAKE");	

	// add include path
	prj.SetProjProperty(CT_COMPILER, CL_IDC_ADD_INCLUDE, cstrSources);

	// add library
	strDefaultLibs = prj.GetProjProperty(CT_LINKER, LNK_IDC_MODULES);
	prj.SetProjProperty(CT_LINKER, LNK_IDC_MODULES,
		strDefaultLibs + "xbdm.lib");	

	// turn off autocopy
	EXPECT_SUCCESS(prj.SetProjProperty(CT_IMAGEBLD, IMAGEBLD_IDC_AUTOCOPY_OFF, 1));
	EXPECT_SUCCESS(prj.Build());
	EXPECT_SUCCESS(prj.VerifyBuild());

	strXBE = CString(prj.GetDir()) + "Release\\" QUAKE_PROJ_NAME ".xbe";
	hr = DmSendFile(strXBE, QUAKE_RLS_XBE);
	if (hr != XBDM_NOERR)
	{
		LOG->RecordFailure("BuildXQuake() failed, DmSendFile(\"%s\", \"%s\") returned %08X",
			strXBE, QUAKE_RLS_XBE, hr);
		return;
	}

	CString strXBCP, strXDK;
	GetEnvironmentVariable("XDK", strXDK.GetBufferSetLength(1024), 1024);
	strXDK.ReleaseBuffer();

	strXBCP = "xbcp.exe /r /s /h /y /t /f " + cstrSources + "\\media\\*.* x" QUAKE_MEDIA;
	// CreateProcess() params.
   	STARTUPINFO siStartInfo;
	PROCESS_INFORMATION piProcInfo;
	// this is the minium initialization of STARTUPINFO required for CreateProcess() to work.
	memset(&siStartInfo, 0, sizeof(STARTUPINFO));
	siStartInfo.cb = sizeof(STARTUPINFO);
	siStartInfo.wShowWindow = SW_SHOWDEFAULT;

	// copy any media files needed
	// spawn nmake in the project dir.
	BOOL bRet = CreateProcess(NULL,
		(char *) ((LPCTSTR) strXBCP),
		NULL,
		NULL,
		TRUE,
		NORMAL_PRIORITY_CLASS,
		NULL,
		strXDK,
		&siStartInfo,
		&piProcInfo);
	
	// verify that nmake was successfully spawned.						  
	if(!bRet)
	{
		LPVOID lpMsgBuf;
		FormatMessage( 
			FORMAT_MESSAGE_ALLOCATE_BUFFER | 
			FORMAT_MESSAGE_FROM_SYSTEM | 
			FORMAT_MESSAGE_IGNORE_INSERTS,
			NULL,
			GetLastError(),
			MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
			(LPTSTR) &lpMsgBuf,
			0,
			NULL 
		);

		m_pLog->RecordFailure("XBCP spawn error (%s).\nxbcp = [%s]", lpMsgBuf, strXBCP);
		// Free the buffer.
		LocalFree( lpMsgBuf );
		return;
	}

	SafeCloseProject();
}
