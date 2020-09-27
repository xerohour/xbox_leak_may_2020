///////////////////////////////////////////////////////////////////////////////
//	CMDLINEBLD.CPP
//
//	Created by :			Date :
//		BrickB					12/29/97
//
//	Description :
//		Implementation of the CPrjCommandLineBldTest class
//		This test will take a close look at msdev.com - the command line build
//		When launched it should build a project file the same as the full IDE would
//		Without bringing up the UI. 
//
//		TestCleanOption:				Do a /CLEAN option only

#include "stdafx.h"
#include "cmdlinebld.h"
#include "prjcases.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

IMPLEMENT_TEST(CPrjCommandLineBldTest, CTest, "Verify Command Line Build", -1, CSniffDriver)

CPrjCommandLineBldTest::CPrjCommandLineBldTest()
{

}

CPrjCommandLineBldTest::~CPrjCommandLineBldTest()
{

}

void CPrjCommandLineBldTest::Run(void)
{
	//COProject prj;
	LPTSTR lpszJunk;

	// TODO: This need to know if to use the debug instead. Need more hooks into CAFE.
	// 
	CString strMSDEV = "msdev.com";

	// need to hunt down where the BIN directory is using the CAFE "Executable Path"
	CIDESubSuite* pSubSuite=(CIDESubSuite*)GetSubSuite();
	ASSERT(pSubSuite->IsKindOf(RUNTIME_CLASS(CIDESubSuite))); //our tests are IDE tests only
	// get the toolset environment settings.
	CSettings *settingsToolsetEnv = pSubSuite->GetToolset()->GetEnvSettings();
	// get the PATH string
	CString cstrPathEnv = settingsToolsetEnv->GetTextValue(settingPathEnv);

	m_pLog->RecordInfo("Search Path = %s", cstrPathEnv);

	CString strTemp;
		
	SearchPath(cstrPathEnv, strMSDEV, NULL, MAX_PATH, strTemp.GetBuffer(MAX_PATH), &lpszJunk);
	
	if (strTemp == "")
	{
		m_pLog->RecordFailure("%s - file not found in search path.", strMSDEV);
		return;
	}
	// lets work with short paths.
	::GetShortPathName(strTemp, m_strMSDEVPath.GetBuffer(MAX_PATH), MAX_PATH), 

	m_pLog->RecordInfo("Tool: MSDEV.com");
	m_pLog->RecordInfo("  MSDEV.com path: %s", m_strMSDEVPath);

	TestCleanOption();

}

// TEST /CLEAN
// this will build the Hello project and make note of the files in the output folder
// it will then attempt to delete these files using the command line build tool 
// with the /CLEAN option
void CPrjCommandLineBldTest::TestCleanOption(void)
{
	
	m_pLog->RecordInfo("Command Line Build Test with /CLEAN Option...");

	// Open a standard project
	if (!VERIFY_TEST_SUCCESS( m_prj.Open(m_strCWD + PROJNAME + "\\" + PROJNAME)))
	{
		m_pLog->RecordFailure("CPrjCommandLineBldTest::TestCleanOption - Open the project");
		return;
	}
	CString strTarget = PROJNAME;
	strTarget += " - Win32 Debug";
	// set the Debug build
	if (!VERIFY_TEST_SUCCESS( m_prj.SetTarget(strTarget)))
	{
		m_pLog->RecordFailure("CPrjCommandLineBldTest::TestCleanOption - Set Target");
		return;
	}

	// lets build the Debug version
	if (!VERIFY_TEST_SUCCESS( m_prj.Build(4))) 
	{
		m_pLog->RecordFailure("CPrjCommandLineBldTest::TestCleanOption - Build Project");
		return;
	}

	// Check to see if there are output files like Obj's and an EXE
	CString strOutFile = m_prj.GetOutputFile();

	// lets make sure the output file is there
	WIN32_FIND_DATA fdTemp;
	HANDLE hdFile = FindFirstFile(strOutFile, &fdTemp);
	m_pLog->RecordInfo("Check to see if %s was built", strOutFile);
	if (hdFile == INVALID_HANDLE_VALUE)
	{
		m_pLog->RecordFailure("CPrjCommandLineBldTest::TestCleanOption - Output file %s not found", strOutFile);
		return;
	}
	FindClose(hdFile);

	// Now lets run the Command Line Build deal with the /CLEAN Option

	char drive[_MAX_DRIVE];   char dir[_MAX_DIR];
	char fname[_MAX_FNAME];   char ext[_MAX_EXT];

	_splitpath( strOutFile, drive, dir, fname, ext );

	CString strCommandLine;
	
	strCommandLine.Format("\"%s\" /MAKE \"%s\" /CLEAN", m_prj.GetFullPath(), strTarget);

	m_pLog->RecordInfo("Command Line for msdev.com: %s %s", m_strMSDEVPath, strCommandLine);

	if (!SpawnConsoleApp(m_strMSDEVPath, strCommandLine)) 
	{
		m_pLog->RecordFailure("CPrjCommandLineBldTest::TestCleanOption - Could not start msdev.com");
		return;
	}

	// The Output files should be gone now
	// lets look in the output directory to see what is there. 
	// guess what? The Itermidate Dir can be different than the OutputFile.
	// first look for the Output File
	m_pLog->RecordInfo("Checking for EXE files in %s", strOutFile);
	hdFile = FindFirstFile(strOutFile, &fdTemp);
	if (hdFile != INVALID_HANDLE_VALUE)
	{
		m_pLog->RecordFailure("CPrjCommandLineBldTest::TestCleanOption - Output file %s was not CLEANed", strOutFile);
		FindClose(hdFile);
		return;
	}
	
	// now there should be no OBJ files in this folder
	strOutFile.Format("%s\\%s\\*.obj", m_prj.GetDir(), m_prj.GetIntermediateDir());
	m_pLog->RecordInfo("Checking for OBJ files in %s", strOutFile);

	hdFile = FindFirstFile(strOutFile, &fdTemp);
	if (hdFile != INVALID_HANDLE_VALUE)
	{
		m_pLog->RecordFailure("CPrjCommandLineBldTest::TestCleanOption - Output file %s was not CLEANed - Still contains OBJ Files", strOutFile);
		FindClose(hdFile);
		return;
	}

	// if we made it here, all is good for this test.

	m_pLog->RecordSuccess("Command Line Build CPrjCommandLineBldTest::TestCleanOption -- Passed");

	return;
}


// a Standard way to spawn the msdev.com file since it as a console app. 
BOOL CPrjCommandLineBldTest::SpawnConsoleApp(CString strEXE, CString strCmdLine)
{

	BOOL bSuccess = FALSE;

	STARTUPINFO si;  /* for CreateProcess call */
	PROCESS_INFORMATION pi;  /* for CreateProcess call */

	/* Set up the STARTUPINFO structure for the CreateProcess() call */
	memset(&si, 0, sizeof(si));
	si.cb = sizeof(si);

	si.dwFlags = STARTF_USESHOWWINDOW;
	si.wShowWindow = SW_NORMAL;

	/* Now create the child process */
	char cstrTempEXE[MAX_PATH];
	char cstrTempCMDLine[MAX_PATH*2];

	strcpy(cstrTempEXE, strEXE);
	// DOS applications need to have program and command line tack together
	strcpy(cstrTempCMDLine, cstrTempEXE);
	strcat(cstrTempCMDLine, " ");
	strcat(cstrTempCMDLine, strCmdLine);

	bSuccess = CreateProcess(cstrTempEXE,  /* filename */
			cstrTempCMDLine,  /* full command line for child */
			NULL,  /* process security descriptor */
			NULL,  /* thread security descriptor */
			FALSE,  /* inherit handles? Also use if STARTF_USESTDHANDLES */
			NORMAL_PRIORITY_CLASS,  /* creation flags */
			NULL,  /* inherited environment address */
			NULL,  /* startup dir; NULL = start in current */
			&si,  /* pointer to startup info (input) */
			&pi);  /* pointer to process info (output) */

	if (bSuccess == 0)
		DoGetLastMessageDlg(__FILE__, __LINE__);

	DWORD dw = WaitForSingleObject(pi.hProcess, 60000);

	if (WAIT_OBJECT_0 != dw)
	{
		DoGetLastMessageDlg(__FILE__, __LINE__);
		return FALSE;
	}
	
	DWORD dwExitCode;

	if (!GetExitCodeProcess(pi.hProcess, &dwExitCode))
	{
		return FALSE;
	}

	bSuccess = CloseHandle(pi.hThread);
	bSuccess &= CloseHandle(pi.hProcess);

	return bSuccess;
}

// just a way to get at the system error to help track down why we have a failure
void CPrjCommandLineBldTest::DoGetLastMessageDlg(char *strFile, int iLineNum)
{
	LPVOID lpMsgBuf;
	FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL,
		GetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
		(LPTSTR) &lpMsgBuf,
		0,
		NULL );// Process any inserts in lpMsgBuf.
	// ...// Display the string.

	char strTemp[_MAX_PATH];

	sprintf(strTemp, "%s, %i, %s", strFile, iLineNum, (char *)lpMsgBuf);

#ifdef _DEBUG
	::MessageBox( NULL, strTemp, "Error", MB_OK | MB_ICONINFORMATION );
#endif
	
	m_pLog->RecordFailure(strTemp);

	// Free the buffer.
	LocalFree( lpMsgBuf );
}
