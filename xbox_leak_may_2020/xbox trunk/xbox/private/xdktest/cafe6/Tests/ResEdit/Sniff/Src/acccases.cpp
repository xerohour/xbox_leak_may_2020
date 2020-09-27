///////////////////////////////////////////////////////////////////////////////
//	ACCCASES.CPP
//
//	Created by :			Date :
//		ChrisSh					8/13/93
//
//	Modified by :			Date :			Action :
//		ScottSe					8/8/96			Added several tests in NewAccel() section
//
//	Description :
//		Implementation of the CAccCases class
//

#include "stdafx.h"
#include "acccases.h"

#define new DEBUG_NEW

#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;

IMPLEMENT_TEST(CAccTestCases, CTest, "Accelerator Editor Tests", -1, CSniffDriver)

void CAccTestCases::Run(void)
{
	// Delete previous files
	KillAllFiles(m_strCWD + "TESTOUT", FALSE);

	if ( CreateAccelTable() == FALSE )
 	{
		m_pLog->RecordFailure("Accelerator table creation failed, devres.pkg not loaded");
		UIWB.CloseAllWindows();
		return;
	}
	m_pLog->RecordInfo("Accelerator table creation passed");

	if ( NewAccel() == FALSE )
 	{
		m_pLog->RecordFailure("Accelerator items creation failed, possible timing issue");
		UIWB.CloseAllWindows();
		return;
	}
	m_pLog->RecordInfo("Accelerator items creation passed");

	if ( SaveOpen() == FALSE )
 	{
		m_pLog->RecordFailure("Failed accelerator IO test");
		UIWB.CloseAllWindows();
		return;
	}
	m_pLog->RecordInfo("Accelerator IO passed");

	UIWB.CloseAllWindows();
	XSAFETY;
}

///////////////////////////////////////////////////////////////////////////////
//	Test Cases

BOOL CAccTestCases::CreateAccelTable(void)
{
	XSAFETY;
	
	COResScript res;

	if (res.CreateResource(IDSS_RT_ACCELERATOR) == ERROR_ERROR)
	{
		m_pLog->RecordFailure("Create new AccelTable");
		return FALSE;
	}
	return TRUE;
}

BOOL CAccTestCases::NewAccel(void)
{
	XSAFETY;

	//Maximize Window
	MST.DoKeys("%-");
	MST.DoKeys("{DOWN 4}~");


	//Add first accelerator
	MST.DoKeys("{INSERT}");
	MST.DoKeys("A");		//ID_ACCEL40001
	MST.DoKeys("~");
	
	//Check to see if it was named properly
	MST.DoKeys("{UP}");
	CString strID = UIWB.GetProperty(P_ID);
	if (strID.Find("ID_ACCEL40001") != 0) {
		m_pLog->RecordFailure("Incorrect ID given to new Accel: %s", strID);
		return FALSE;
	}
	
	//Add more accelerators
	MST.DoKeys("{INSERT}");
	MST.DoKeys("A");		//ID_ACCEL40002
	MST.DoKeys("~");
	MST.DoKeys("{INSERT}");
	MST.DoKeys("b");		//ID_ACCEL40003
	MST.DoKeys("~");
	MST.DoKeys("{INSERT}");
	MST.DoKeys("z");		//ID_ACCEL40004
	MST.DoKeys("~");
	MST.DoKeys("{INSERT}");
	MST.DoKeys("5");		//ID_ACCEL40005
	MST.DoKeys("~");
	
	//Find the Z accelerator
	UIWB.DoCommand(ID_EDIT_FIND_DLG, DC_MNEMONIC);
	MST.DoKeys("z");
	MST.DoKeys("~");

	//Check to see if we found the right one
	strID = UIWB.GetProperty(P_ID);
	if (strID.Find("ID_ACCEL40004") != 0) {
		m_pLog->RecordFailure("Z accelerator not found: %s", strID);
		return FALSE;
	}

	//Multiple selection of all strings
	MST.DoKeys("{HOME}");
	MST.DoKeys("+{END}");
	MST.DoKeys("+{UP}");

	
	//Change accelerator to X
	MST.DoKeys("%{ENTER}");
	MST.DoKeys("x");
	MST.DoKeys("~");

	//Check to see we changed the top one to X
	MST.DoKeys("{HOME}");
	strID = UIWB.GetProperty(P_Key);
	if (strID.Find("X") != 0) {
		m_pLog->RecordFailure("Top accelerator not changed to X: %s", strID);
		return FALSE;
	}

	//Undo the change
	UIWB.DoCommand(ID_EDIT_UNDO, DC_MNEMONIC);
	
	//Check to see we changed the top one back to 5
	MST.DoKeys("{HOME}");
	strID = UIWB.GetProperty(P_Key);
	if (strID.Find("5") != 0) {
		m_pLog->RecordFailure("Top accelerator not undo'd correctly to 5: %s", strID);
		return FALSE;
	}

	return TRUE;
}

BOOL CAccTestCases::SaveOpen(void)
{
	XSAFETY;

	// Save resource 
	if ( UIWB.SaveFileAs(m_strCWD + "testout\\Accel ίαρ.rc", TRUE) == FALSE )
	{
		m_pLog->RecordFailure("File save failed %s", m_strCWD + "testout\\Accel ίαρ.rc");
		return FALSE;
	}
	Sleep(1500);

	//close file
	UIWB.DoCommand(ID_FILE_CLOSE, DC_MNEMONIC);

	//reopen file
	HWND hwnd = UIWB.OpenFile(m_strCWD + "testout\\Accel ίαρ.rc");
	if (hwnd == NULL)
	{
		m_pLog->RecordFailure("File-Open failed on %s", m_strCWD + "testout\\Accel ίαρ.rc");
		return FALSE;
	}

	//Reopen editor
	MST.DoKeys("{DOWN}~{DOWN}~");

	//Make sure it opened
	MST.DoKeys("{HOME}");
	CString strID = UIWB.GetProperty(P_ID);
	if (strID.Find("ID_ACCEL40005") != 0) {
		m_pLog->RecordFailure("File not opened, returned ID: %s", strID);
		return FALSE;
	}

	//Close it again
	UIWB.DoCommand(ID_FILE_CLOSE, DC_MNEMONIC);

	return TRUE;

}

