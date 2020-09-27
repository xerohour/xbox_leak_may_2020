///////////////////////////////////////////////////////////////////////////////
//	BINCASES.CPP
//
//	Created by :			Date :
//		ChrisSh					8/13/93
//
//	Description :
//		Implementation of the CBinTestCases class
//

#include "stdafx.h"
#include "bincases.h"

#define new DEBUG_NEW

#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;

IMPLEMENT_TEST(CBinTestCases, CTest, "Binary Editor Tests", -1, CSniffDriver)

void CBinTestCases::Run(void)
{
	// Delete previous files
	KillAllFiles(m_strCWD + "TESTOUT", FALSE);

	//Create new binary resource
	if ( CreateBinaryEditor() == FALSE )
 	{
		m_pLog->RecordFailure("Binary editor not enabled");
		UIWB.CloseAllWindows();
		return;
	}
	m_pLog->RecordInfo("Binary editor enabled");

	// Put some data in it
	if ( EnterBinaryData() == FALSE )
 	{
		m_pLog->RecordFailure("Binary data creation failed, possible timing issue");
		UIWB.CloseAllWindows();
		return;
	}
	m_pLog->RecordInfo("Binary data entry passed");

	// Save & Open
	if ( SaveOpen() == FALSE )
 	{
		m_pLog->RecordFailure("Failed binary IO test");
		UIWB.CloseAllWindows();
		return;
	}
	m_pLog->RecordInfo("Binary IO passed");

	UIWB.CloseAllWindows();
	XSAFETY;
}

///////////////////////////////////////////////////////////////////////////////
//	Test Cases

BOOL CBinTestCases::CreateBinaryEditor(void)
{
	XSAFETY;

	COResScript res;

	if (res.CreateResource(IDSS_RT_BINARY) == ERROR_ERROR)
	{
		m_pLog->RecordFailure("Could not create new binary resource");
		return FALSE;
	}
	return TRUE;
}

BOOL CBinTestCases::EnterBinaryData(void)
{
	XSAFETY;

	// Enter some data both in ASCII and HEX
	MST.DoKeys("616263{TAB}def");	
	MST.DoKeys("{HOME}+({END})");	// Highlight entire line
	MST.DoKeys("^({INSERT})");		// Copy text into Clipboard

	//Check to see if string was copied correctly
	CString str;
	GetClipText(str);
	if (str != "abcdef") 
	{
		m_pLog->RecordFailure("Binary Data not copied to clipboard correctly:", str);
		return FALSE;
	}
	
	//Add some international text
	MST.DoKeys("{END}");
	MST.DoKeys("ΐΖΚΦίαρÿ");

	// Search for ί character
	MST.DoKeys("%{F3}");
	MST.DoKeys("ί");
	MST.DoKeys("~");
	
	//Check to see if we found it
 	UIWB.DoCommand( ID_EDIT_COPY, DC_ACCEL );
	GetClipText(str);
	if (str != "ί") 
	{
		m_pLog->RecordFailure("ί character not found correctly:", str);
		return FALSE;
	}

	//Goto specific offset
	UIWB.DoCommand(IDM_GOTO_LINE, DC_MNEMONIC);
	MST.DoKeys("4");	//should be 'e'
	MST.DoKeys("~");
	MST.DoKeys("{ESC}");

	//Check to see if we found it
 	UIWB.DoCommand( ID_EDIT_COPY, DC_ACCEL );
	GetClipText(str);
	if (str != "e") 
	{
		m_pLog->RecordFailure("Goto failed:", str);
		return FALSE;
	}

	return TRUE;
}


BOOL CBinTestCases::SaveOpen(void)
{
	XSAFETY;

	// Save resource 
	if ( UIWB.SaveFileAs(m_strCWD + "testout\\Binary ίαρ.rc", TRUE) == FALSE )
	{
		m_pLog->RecordFailure("File save failed %s", m_strCWD + "testout\\Binary ίαρ.rc");
		return FALSE;
	}
	Sleep(500);

	//close file
	UIWB.DoCommand(ID_FILE_CLOSE, DC_MNEMONIC);

	//reopen file
	HWND hwnd = UIWB.OpenFile(m_strCWD + "testout\\Binary ίαρ.rc");
	if (hwnd == NULL)
	{
		m_pLog->RecordFailure("File-Open failed on %s", m_strCWD + "testout\\Binary ίαρ.rc");
		return FALSE;
	}

	//Reopen editor
	MST.DoKeys("{DOWN}~{DOWN}~");

	//Make sure it opened
	MST.DoKeys("+{RIGHT}");
 	UIWB.DoCommand( ID_EDIT_COPY, DC_ACCEL );
	CString str;
	GetClipText(str);
	if (str != "a") 
	{
		m_pLog->RecordFailure("File not opened, returned String: %s", str);
		return FALSE;
	}

	//Close it again
	UIWB.DoCommand(ID_FILE_CLOSE, DC_MNEMONIC);

	return TRUE;

}

