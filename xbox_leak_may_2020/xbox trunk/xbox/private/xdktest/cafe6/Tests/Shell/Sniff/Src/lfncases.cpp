///////////////////////////////////////////////////////////////////////////////
//      LFNCASES.CPP
//
//      Created by :                    Date :
//              MikePie                                 3/29/95
//
//      Description :
//              Implementation of the CTBTestCases class
///////////////////////////////////////////////////////////////////////////////
//
#include "stdafx.h"
#include "lfncases.h"


#define new DEBUG_NEW
#undef THIS_FILE


static char BASED_CODE THIS_FILE[] = __FILE__;
CString testfile;
CString testfileMST;
CString delfile;


IMPLEMENT_TEST(CLfnTestCases, CTest, "Long Filename Tests", -1, CSniffDriver)


void CLfnTestCases::Run(void)
{
	XSAFETY;
	DetectLanguage();
	DeleteOldFile();
	CreateLongFilename();
	OpenLongFilename();  
	DeleteNewFile();
}


///////////////////////////////////////////////////////////////////////////////
//      Test Cases
///////////////////////////////////////////////////////////////////////////////
//
BOOL CLfnTestCases::DetectLanguage(void)
{
	DWORD sysinfo = (DWORD) GetSystem();

	testfile = "LfnTest!&().EXT";
	testfileMST = "LfnTest!&{(}{)}.EXT";
	delfile = testfile;
/*
	if (sysinfo & SYSTEM_NT)
	{
		m_pLog->RecordInfo("According to GetSystem, this is NT.");
		testfile = "LfnTest!&().EXT";
		testfileMST = "LfnTest!&{(}{)}.EXT";
		delfile = testfile;
	}
	if (sysinfo & SYSTEM_CHICAGO)
	{
		m_pLog->RecordInfo("According to GetSystem, this is Windows95");
		testfile = "Lfn Test!&().EXT";
		testfileMST = "Lfn Test!&{(}{)}.EXT";
		delfile=testfile;
	}
	if (sysinfo & SYSTEM_DBCS)
	{
		m_pLog->RecordInfo("According to GetSystem, this is multibyte-enabled.");
		testfile = "Lfn Test!&ƒA‚.ext•Z";
		testfileMST = "Lfn Test!&(ƒA)(‚).ext(•Z)";
		delfile = testfile;
	}
	if (sysinfo & SYSTEM_JAPAN)
	{
		m_pLog->RecordInfo("According to GetSystem, this is Japanese.");
		testfile = "Lfn Test!&ƒA‚.ext•Z";
		testfileMST = "Lfn Test!&(ƒA)(‚).ext(•Z)";
		delfile = testfile;
	}
*/		 
		return TRUE;
}


BOOL CLfnTestCases::DeleteOldFile(void)
{
	m_pLog->RecordInfo("Clearing out existing instance of file, if any...");
	m_pLog->RecordInfo("   Trying to use path %s...",m_strCWD);
	m_pLog->RecordInfo("   Trying to remove file %s...",testfile);
	KillFile(m_strCWD,delfile);

	return TRUE;
}


BOOL CLfnTestCases::CreateLongFilename(void)
{
	COSource sourc;
	m_pLog->RecordInfo("Creating long filename...");
    
	sourc.Create(); // New source file.
	MST.DoKeys("{ENTER}");          
	MST.DoKeys("This is a test file.");
//	MST.DoKeys("%(FA)");
	UIWB.DoCommand(ID_FILE_SAVE_AS, DC_MNEMONIC);
	MST.DoKeys(m_strCWD + testfileMST);
	MST.DoKeys("{ENTER}");
//	MST.DoKeys("%(FC)");
	UIWB.DoCommand(ID_FILE_CLOSE, DC_MNEMONIC);

	return TRUE;
}


BOOL CLfnTestCases::OpenLongFilename(void)
{
	COSource sourc;
	m_pLog->RecordInfo("Opening long filename...");

	if (sourc.Open(m_strCWD + testfile))
	{
//		MST.DoKeys("%(FC)");
		UIWB.DoCommand(ID_FILE_CLOSE, DC_MNEMONIC);
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}


BOOL CLfnTestCases::DeleteNewFile(void)
{
	m_pLog->RecordInfo("Cleaning up...");
	m_pLog->RecordInfo("   Trying to use path %s...",m_strCWD);
	m_pLog->RecordInfo("   Trying to remove file %s...",testfile);
	KillFile(m_strCWD,delfile);

	return TRUE;
}
					 
