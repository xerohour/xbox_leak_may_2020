///////////////////////////////////////////////////////////////////////////////
//	STRCASES.CPP
//
//	Created by :			Date :
//		ChrisSh					8/13/93
//
//	Modified by:			Date:			Changes:
//		ScottSe					8/15/96			Added several tests to NewString()
//
//	Description :
//		Implementation of the CStrTestCases class
//

#include "stdafx.h"
#include "strcases.h"

#define new DEBUG_NEW

#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;

IMPLEMENT_TEST(CStrTestCases, CTest, "String Editor Tests", -1, CSniffDriver)

void CStrTestCases::Run(void)
{
	// Delete previous files
	KillAllFiles(m_strCWD + "TESTOUT", FALSE);

	if ( CreateStringTable() == FALSE )
 	{
		m_pLog->RecordFailure("String table creation failed, devres.pkg not loaded");
		UIWB.CloseAllWindows();
		return;
	}
	m_pLog->RecordInfo("String table creation passed");

	if ( NewString() == FALSE )
 	{
		m_pLog->RecordFailure("String items creation failed, possible timing issue");
		UIWB.CloseAllWindows();
		return;
	}
	m_pLog->RecordInfo("String items creation passed");

	if ( SaveOpen() == FALSE )
 	{
		m_pLog->RecordFailure("Failed String IO test");
		UIWB.CloseAllWindows();
		return;
	}
	m_pLog->RecordInfo("String IO passed");

	UIWB.CloseAllWindows();
	XSAFETY;
}

///////////////////////////////////////////////////////////////////////////////
//	Test Cases

BOOL CStrTestCases::CreateStringTable(void)
{
	XSAFETY;

	COResScript res;

	if (res.CreateResource(IDSS_RT_STRING_TABLE) == ERROR_ERROR)
	{
		m_pLog->RecordFailure("Could not create new StringTable");
		return FALSE;
	}
	return TRUE;
}

BOOL CStrTestCases::NewString(void)
{
	XSAFETY;

	// Add first string
	MST.DoKeys("This is the first string.");	// Type a new string
	MST.DoKeys("~");							// IDS_STRING1
	MST.DoKeys("{UP}");							// Highlight the new string again
	
	// Check for proper ID
	CString strID = UIWB.GetProperty(P_ID);
	if (strID != "IDS_STRING1")
	{
		m_pLog->RecordFailure("(3) Incorrect ID given to new string: %s", strID);
		return FALSE;
	}
	
	//Add more strings
	MST.DoKeys("{INSERT}");
	MST.DoKeys("This is the second string.");		//IDS_STRING2
	MST.DoKeys("~");
	MST.DoKeys("{INSERT}");
	MST.DoKeys("This is the third string.");		//IDS_STRING3
	MST.DoKeys("~");
	MST.DoKeys("{INSERT}");
	MST.DoKeys("This is the fourth string.");		//IDS_STRING4
	MST.DoKeys("~");
	MST.DoKeys("{INSERT}");
	MST.DoKeys("This is the fifth string.");		//IDS_STRING5
	MST.DoKeys("~");

	//Find the third string
	MST.DoKeys("%{F3}");
	MST.DoKeys("third");
	MST.DoKeys("~");

	//Check to see if we found the right one
	strID = UIWB.GetProperty(P_ID);
	if (strID != "IDS_STRING3") 
	{
		m_pLog->RecordFailure("Third string not found: %s", strID);
		return FALSE;
	}

	//Multiple selection of all strings
	MST.DoKeys("{HOME}");
	MST.DoKeys("+{END}");
	MST.DoKeys("+{UP}");

	
	//Change strings to FooBar
	MST.DoKeys("%{ENTER}");
	MST.DoKeys("FooBar");
	MST.DoKeys("~");

	//Check to see we changed the top one to FooBar
	MST.DoKeys("{HOME}");
	strID = UIWB.GetProperty(136);	//136 == P_String
	if (strID != "FooBar") 
	{
		m_pLog->RecordFailure("Top string not changed to FooBar: %s", strID);
		return FALSE;
	}

	//Undo the change
	MST.DoKeys("^z");

	//Check to see we changed the top one back to normal
	MST.DoKeys("{HOME}");
	strID = UIWB.GetProperty(136);	//136 == P_String
	if (strID != "This is the first string.") 
	{
		m_pLog->RecordFailure("Top string not undo'd correctly: %s", strID);
		return FALSE;
	}

	// Add international string
	MST.DoKeys("{INSERT}");
	MST.DoKeys("ίαρ");	
	MST.DoKeys("~");				// IDS_STRING6

	//Check to see it was added correctly
	MST.DoKeys("{UP}");
	strID = UIWB.GetProperty(136);	//136 == P_String
	if (strID != "ίαρ") 
	{
		m_pLog->RecordFailure("ίαρ String not added correctly: %s", strID);
		return FALSE;
	}

	return TRUE;
}

BOOL CStrTestCases::SaveOpen(void)
{
	XSAFETY;

	// Save resource 
	if ( UIWB.SaveFileAs(m_strCWD + "testout\\String ίαρ.rc", TRUE) == FALSE )
	{
		m_pLog->RecordFailure("File save failed %s", m_strCWD + "testout\\String ίαρ.rc");
		return FALSE;
	}
	Sleep(1500);

	//close file
	UIWB.DoCommand(ID_FILE_CLOSE, DC_MNEMONIC);

	//reopen file
	HWND hwnd = UIWB.OpenFile(m_strCWD + "testout\\String ίαρ.rc");
	if (hwnd == NULL)
	{
		m_pLog->RecordFailure("File-Open failed on %s", m_strCWD + "testout\\String ίαρ.rc");
		return FALSE;
	}

	//Reopen editor
	MST.DoKeys("{DOWN}~{DOWN}~");

	//Make sure it opened
	MST.DoKeys("{HOME}");
	CString strID = UIWB.GetProperty(136);	//136 == P_String
	if (strID != "This is the first string.") 
	{
		m_pLog->RecordFailure("File not opened, returned string: %s", strID);
		return FALSE;
	}

	//Close it again
	UIWB.DoCommand(ID_FILE_CLOSE, DC_MNEMONIC);

	return TRUE;
   
}


