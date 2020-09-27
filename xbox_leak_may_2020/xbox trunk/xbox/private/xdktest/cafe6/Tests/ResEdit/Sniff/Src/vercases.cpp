///////////////////////////////////////////////////////////////////////////////
//	VERCASES.CPP
//
//	Created by :			Date :
//		Enriquep					8/27/93
//
//	Modifies by :			Date :
//		ScottSe						9/13/96		Major rewrite of the entire test
//
//	Description :
//		Implementation of the CVerTestCases class
//

#include "stdafx.h"
#include "vercases.h"

#define new DEBUG_NEW

#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;

IMPLEMENT_TEST(CVerTestCases, CTest, "Version editor tests", -1, CSniffDriver)

void CVerTestCases::Run(void)
{
	HWND hwndRC;

	XSAFETY;

	//Create new resource script
	if ((hwndRC = UIWB.CreateNewFile(GetLocString(IDSS_NEW_RC))) == NULL)
	{
		m_pLog->RecordFailure("Could not create RC script");
		UIWB.CloseAllWindows();
	}
	else
	{
		if (!CreateVersionRes())
		{
			m_pLog->RecordFailure("Failed version creation test");
			UIWB.CloseAllWindows();
			return;
		}
		if (!EditCommentsField())
		{
			m_pLog->RecordFailure("Failed editing contents");
			UIWB.CloseAllWindows();
			return;
		}
		if (!CopyPaste())
		{
			m_pLog->RecordFailure("Failed copy paste test");
			UIWB.CloseAllWindows();
			return;
		}
		if (!NewStringBlock())
		{
			m_pLog->RecordFailure("Failed NewStringBlock test");
			UIWB.CloseAllWindows();
			return;
		}
		if (!DeleteStringBlock())
		{
			m_pLog->RecordFailure("Failed DeleteStringBlocktest");
			UIWB.CloseAllWindows();
			return;
		}
		if (!ChangeToStringID())
		{
			m_pLog->RecordFailure("Failed ChangeToStringID test");
			UIWB.CloseAllWindows();
			return;
		}
		if (!SaveOpen())
		{
			m_pLog->RecordFailure("Failed SaveOpen test");
			UIWB.CloseAllWindows();
			return;
		}
	}
	UIWB.CloseAllWindows();
}

///////////////////////////////////////////////////////////////////////////////
//	Test Cases


BOOL CVerTestCases::CreateVersionRes(void)
{
	XSAFETY;

	COResScript res;

	//Create new version
	if (res.CreateResource(IDSS_RT_VERSION) == ERROR_ERROR)
	{
		m_pLog->RecordFailure("Unable to create new version resource");
		return FALSE;
	}
	Sleep(1500);

	m_uVerEd = UIWB.GetActiveEditor();

	return TRUE;
}


BOOL CVerTestCases::EditCommentsField(void)
{
	XSAFETY;

	CString cstrEditText;

	//Add file version
	MST.DoKeys("{DOWN}");
	MST.DoKeys("1,2,3,4");
	MST.DoKeys("~");

	//Add product version
	MST.DoKeys("{DOWN}");
	MST.DoKeys("5,6,7,8");
	MST.DoKeys("~");

	//Add file flags
	MST.DoKeys("{DOWN 2}");
	UIControlProp prop = UIWB.ShowPropPage(TRUE);
	prop.IsValid();
	MST.WCheckCheck( GetLabel(VRES_IDC_VS_FF_PRERELEASE) );
	UIWB.ShowPropPage(FALSE);	

	//Add file OS
	MST.DoKeys("{DOWN}");
	MST.DoKeys("{ENTER}{HOME}");
	MST.DoKeys("~");

	//Add file type
	MST.DoKeys("{DOWN}");
	MST.DoKeys("{ENTER}{HOME}");
	MST.DoKeys("~");

	//Add comments
	MST.DoKeys("{DOWN 3}");
	MST.DoKeys("This is a comment");
	MST.DoKeys("~");

	//Add company name
	MST.DoKeys("{DOWN}");
	MST.DoKeys("This is my company");
	MST.DoKeys("~");

	//Add file description
	MST.DoKeys("{DOWN}");
	MST.DoKeys("This is the file description");
	MST.DoKeys("~");

	//Add file version
	MST.DoKeys("{DOWN}");
	MST.DoKeys("1.3.5");
	MST.DoKeys("~");

	//Add internal name
	MST.DoKeys("{DOWN}");
	MST.DoKeys("Sniff test '96");
	MST.DoKeys("~");

	//Add copyright
	MST.DoKeys("{DOWN}");
	MST.DoKeys("Copyright ίαρ");
	MST.DoKeys("~");

	//Add legal tm's
	MST.DoKeys("{DOWN}");
	MST.DoKeys("Trademark ίαρ");
	MST.DoKeys("~");

	//Add original file name
	MST.DoKeys("{DOWN}");
	MST.DoKeys("Vres sniff");
	MST.DoKeys("~");

	//Add private build
	MST.DoKeys("{DOWN}");
	MST.DoKeys("This is a private build");
	MST.DoKeys("~");

	//Add product name
	MST.DoKeys("{DOWN}");
	MST.DoKeys("Cafe sniff test");
	MST.DoKeys("~");

	//Add product version
	MST.DoKeys("{DOWN}");
	MST.DoKeys("3.5.6");
	MST.DoKeys("~");

	//Add special build
	MST.DoKeys("{DOWN}");
	MST.DoKeys("This is a special build");
	MST.DoKeys("~");

	// Check if everything got entered correctly
	MST.DoKeys("{HOME}");

	//File version
	MST.DoKeys("{TAB}");
	MST.WEditText("", cstrEditText);
	MST.DoKeys("{ENTER}");
	if (cstrEditText != "1, 2, 3, 4")
	{
		m_pLog->RecordFailure("Incorrect file version: %s", cstrEditText);
		return FALSE;
	}

	//Product version
	MST.DoKeys("{DOWN}");
	MST.DoKeys("{TAB}");
	MST.WEditText("", cstrEditText);
	MST.DoKeys("{ENTER}");
	if (cstrEditText != "5, 6, 7, 8")
	{
		m_pLog->RecordFailure("Incorrect product version: %s", cstrEditText);
		return FALSE;
	}

	//Comments
	MST.DoKeys("{DOWN 7}");
	MST.DoKeys("{TAB}");
	MST.WEditText("", cstrEditText);
	MST.DoKeys("{ENTER}");
	if (cstrEditText != "This is a comment")
	{
		m_pLog->RecordFailure("Incorrect comment: %s", cstrEditText);
		return FALSE;
	}

	//Company name
	MST.DoKeys("{DOWN}");
	MST.DoKeys("{TAB}");
	MST.WEditText("", cstrEditText);
	MST.DoKeys("{ENTER}");
	if (cstrEditText != "This is my company")
	{
		m_pLog->RecordFailure("Incorrect company name: %s", cstrEditText);
		return FALSE;
	}

	//File description
	MST.DoKeys("{DOWN}");
	MST.DoKeys("{TAB}");
	MST.WEditText("", cstrEditText);
	MST.DoKeys("{ENTER}");
	if (cstrEditText != "This is the file description")
	{
		m_pLog->RecordFailure("Incorrect file description: %s", cstrEditText);
		return FALSE;
	}

	//File version
	MST.DoKeys("{DOWN}");
	MST.DoKeys("{TAB}");
	MST.WEditText("", cstrEditText);
	MST.DoKeys("{ENTER}");
	if (cstrEditText != "1.3.5")
	{
		m_pLog->RecordFailure("Incorrect file version: %s", cstrEditText);
		return FALSE;
	}

	//Internal name
	MST.DoKeys("{DOWN}");
	MST.DoKeys("{TAB}");
	MST.WEditText("", cstrEditText);
	MST.DoKeys("{ENTER}");
	if (cstrEditText != "Sniff test '96")
	{
		m_pLog->RecordFailure("Incorrect internal name: %s", cstrEditText);
		return FALSE;
	}

	//Copyright
	MST.DoKeys("{DOWN}");
	MST.DoKeys("{TAB}");
	MST.WEditText("", cstrEditText);
	MST.DoKeys("{ENTER}");
	if (cstrEditText != "Copyright ίαρ")
	{
		m_pLog->RecordFailure("Incorrect copyright: %s", cstrEditText);
		return FALSE;
	}

	//Legal tm's
	MST.DoKeys("{DOWN}");
	MST.DoKeys("{TAB}");
	MST.WEditText("", cstrEditText);
	MST.DoKeys("{ENTER}");
	if (cstrEditText != "Trademark ίαρ")
	{
		m_pLog->RecordFailure("Incorrect legal tm's: %s", cstrEditText);
		return FALSE;
	}

	//Original file name
	MST.DoKeys("{DOWN}");
	MST.DoKeys("{TAB}");
	MST.WEditText("", cstrEditText);
	MST.DoKeys("{ENTER}");
	if (cstrEditText != "Vres sniff")
	{
		m_pLog->RecordFailure("Incorrect original file name: %s", cstrEditText);
		return FALSE;
	}

	//Private build
	MST.DoKeys("{DOWN}");
	MST.DoKeys("{TAB}");
	MST.WEditText("", cstrEditText);
	MST.DoKeys("{ENTER}");
	if (cstrEditText != "This is a private build")
	{
		m_pLog->RecordFailure("Incorrect private build: %s", cstrEditText);
		return FALSE;
	}

	//Product name
	MST.DoKeys("{DOWN}");
	MST.DoKeys("{TAB}");
	MST.WEditText("", cstrEditText);
	MST.DoKeys("{ENTER}");
	if (cstrEditText != "Cafe sniff test")
	{
		m_pLog->RecordFailure("Incorrect product name: %s", cstrEditText);
		return FALSE;
	}

	//Product version
	MST.DoKeys("{DOWN}");
	MST.DoKeys("{TAB}");
	MST.WEditText("", cstrEditText);
	MST.DoKeys("{ENTER}");
	if (cstrEditText != "3.5.6")
	{
		m_pLog->RecordFailure("Incorrect product version: %s", cstrEditText);
		return FALSE;
	}

	//Special build
	MST.DoKeys("{DOWN}");
	MST.DoKeys("{TAB}");
	MST.WEditText("", cstrEditText);
	MST.DoKeys("{ENTER}");
	if (cstrEditText != "This is a special build")
	{
		m_pLog->RecordFailure("Incorrect special build: %s", cstrEditText);
		return FALSE;
	}

	return TRUE;
}	

BOOL CVerTestCases::CopyPaste(void)
{
	XSAFETY;

	CString cstrEditText;

	//Copy the last command
	MST.DoKeys("{END}");
	MST.DoKeys("{TAB}");
	UIWB.DoCommand( ID_EDIT_COPY, DC_ACCEL );
	MST.DoKeys("{ENTER}");


	//Paste it two up
	MST.DoKeys("{UP 2}");
	MST.DoKeys("{TAB}");
	UIWB.DoCommand( ID_EDIT_PASTE, DC_ACCEL );
	MST.DoKeys("{ENTER}");

	//Check it
	MST.DoKeys("{END}");
	MST.DoKeys("{UP 2}");
	MST.DoKeys("{TAB}");
	MST.WEditText("", cstrEditText);
	MST.DoKeys("{ENTER}");
	if (cstrEditText != "This is a special build")
	{
		m_pLog->RecordFailure("Paste failed: %s", cstrEditText);
		return FALSE;
	}

	//Undo the change
	UIWB.DoCommand(ID_EDIT_UNDO, DC_ACCEL);

	//Check it again
	MST.DoKeys("{END}");
	MST.DoKeys("{UP 2}");
	MST.DoKeys("{TAB}");
	MST.WEditText("", cstrEditText);
	MST.DoKeys("{ENTER}");
	if (cstrEditText != "Cafe sniff test")
	{
		m_pLog->RecordFailure("Undo failed: %s", cstrEditText);
		return FALSE;
	}

	//Find comments
	UIWB.DoCommand(ID_EDIT_FIND_DLG, DC_MNEMONIC);
	MST.DoKeys("This is a comment");
	MST.DoKeys("~");

	//Check that we found it
	MST.DoKeys("{DOWN}");
	MST.DoKeys("{TAB}");
	MST.WEditText("", cstrEditText);
	MST.DoKeys("{ENTER}");
	if (cstrEditText != "This is my company")
	{
		m_pLog->RecordFailure("Find failed: %s", cstrEditText);
		return FALSE;
	}

	return TRUE;
}

BOOL CVerTestCases::NewStringBlock(void)
{
	XSAFETY;

	//Create a new string block
	UIWB.DoCommand(IDM_NEW_STRINGBLOCK, DC_MNEMONIC);
	
	//Test it
	UIWB.ShowPropPage(TRUE);								// check props for new block
	CString cstrLangID = UIWB.GetProperty(P_LangID);		
	CString cstrCodePg = UIWB.GetProperty(P_CharSet);	
	UIWB.ShowPropPage(FALSE);		


//  Default changed: DougT, 02/08/95
//	if (cstrLangID != "1025")	   // 0 means 1st item in Language ID dropdown	(Arabic)
//	Default is now Lang Neutral ('0') 

	if (cstrLangID != "0")
	{
		m_pLog->Comment("New String block's LangID should be Lang Neutral");
		m_pLog->Comment("cstrLangID = %s", cstrLangID);
		return FALSE;
	}

	if (cstrCodePg != "1200")	  // 5th item in Code Page dropdown	 (Unicode)
	{
		m_pLog->Comment("New String block's Code Page should be Unicode");
		m_pLog->Comment("cstrCodePg = %s", cstrCodePg);
		return FALSE;
	}

	return TRUE;
}


BOOL CVerTestCases::DeleteStringBlock(void)
{
	XSAFETY;

	//Delete the block
	UIWB.DoCommand(IDM_DELETE_STRINGBLOCK, DC_MNEMONIC);
	MST.DoKeys("{ENTER}");  	// Confirm Delete alert
	
	//Select the block
	MST.DoKeys("{END}");
	
	//Check it
	UIWB.ShowPropPage(TRUE);	
	CString cstrLangID = UIWB.GetProperty(P_LangID);		// check props of current block
	CString cstrCodePg = UIWB.GetProperty(P_CharSet);	
	UIWB.ShowPropPage(FALSE);			//Hide Prop page if it is up


	if (cstrLangID == "0")	   // 0 means 1st item in Language ID dropdown	(Arabic)
	{
		m_pLog->Comment("Current String block's LangID should not be Arabic: %s", cstrLangID );
		return FALSE;
	}

	if (cstrCodePg != "1200")	  // 5th item in Code Page dropdown	 (Unicode)
	{
		m_pLog->Comment("Current String block's Code Page should be Unicode: %s", cstrCodePg);
		return FALSE;
	}

	return TRUE;
}

BOOL CVerTestCases::ChangeToStringID(void)
{
	XSAFETY;

	// Close editor
	UIEditor ed = UIWB.GetActiveEditor();
	EXPECT( ed.IsValid() );
	if( !ed.Close() )
	{
		m_pLog->RecordFailure("Could not close editor");
		return FALSE;
	}

	// Change ID
	UIWB.ShowPropPage(TRUE);			
	MST.DoKeys("Cool_Version{ENTER}");				// Close Version editor
	
	//Check it
	CString cstrID = UIWB.GetProperty(P_ID);	
	if(cstrID != "Cool_Version")
	{
		m_pLog->RecordFailure("Change to String ID failed");
		return FALSE;
	}

	UIWB.ShowPropPage(FALSE);

	return TRUE;
}		

BOOL CVerTestCases::SaveOpen(void)
{
	XSAFETY;

	CString cstrEditText;

	// Save resource 
	if ( UIWB.SaveFileAs(m_strCWD + "Version ίαρ.rc", TRUE) == FALSE )
	{
		m_pLog->RecordFailure("File save failed %s", m_strCWD + "Version ίαρ.rc");
		return FALSE;
	}
	Sleep(1500);

	// Close editor
	UIEditor ed = UIWB.GetActiveEditor();
	EXPECT( ed.IsValid() );
	if( !ed.Close() )
	{
		m_pLog->RecordFailure("Could not close editor");
		return FALSE;
	}

	//reopen file
	HWND hwnd = UIWB.OpenFile(m_strCWD + "Version ίαρ.rc");
	if (hwnd == NULL)
	{
		m_pLog->RecordFailure("File-Open: hwnd == %x", hwnd);
		return FALSE;
	}

	//Reopen editor
	MST.DoKeys("{DOWN}~{DOWN}~");

	// Check if everything got entered correctly
	MST.DoKeys("{HOME}");

	//File version
	MST.DoKeys("{TAB}");
	MST.WEditText("", cstrEditText);
	MST.DoKeys("{ENTER}");
	if (cstrEditText != "1, 2, 3, 4")
	{
		m_pLog->RecordFailure("File not opened correctly: %s", cstrEditText);
		return FALSE;
	}

	//Close it again
	UIWB.DoCommand(ID_FILE_CLOSE, DC_MNEMONIC);

	return TRUE;
}




