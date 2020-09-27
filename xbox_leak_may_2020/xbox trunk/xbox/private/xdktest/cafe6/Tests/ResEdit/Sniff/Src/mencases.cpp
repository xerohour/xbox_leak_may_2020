///////////////////////////////////////////////////////////////////////////////
//	MENCASES.CPP
//
//	Created by :			Date :
//		EnriqueP				8/13/93
//
//	Modified by :			Date :			Action :
//		ScottSe					9/4/96			Major rewrite of entire test
//
//	Description :
//		Implementation of the CMenTestCases class
//

#include "stdafx.h"
#include "mencases.h"

#define new DEBUG_NEW

//Delay to resolve timing issues
#define WAIT 500

#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;

IMPLEMENT_TEST(CMenTestCases, CTest, "Menu Editor Tests", -1, CSniffDriver)

void CMenTestCases::Run(void)
{
	// Delete previous files
	KillAllFiles(m_strCWD + "TESTOUT", FALSE);

	//Create new resource
	if ( CreateMenuRes() == FALSE )
 	{
		m_pLog->RecordFailure("Menu creation failed, devres.pkg not loaded");
		UIWB.CloseAllWindows();
		return;
	}
	m_pLog->RecordInfo("Menu creation passed");

	//Enter a bunch of items
	if ( EnterMenuItems() == FALSE )
 	{
		m_pLog->RecordFailure("Menu items creation failed, possible timing issue");
		UIWB.CloseAllWindows();
		return;
	}
	m_pLog->RecordInfo("Menu items creation passed");

	//Do copy, paste etc.
	if ( CopyAndPaste() == FALSE )
 	{
		m_pLog->RecordFailure("Failed Menu copy/paste test");
		UIWB.CloseAllWindows();
		return;
	}
	m_pLog->RecordInfo("Menu copy/paste passed");

	// File IO
	if ( SaveOpen() == FALSE )
 	{
		m_pLog->RecordFailure("Failed Menu IO test");
		UIWB.CloseAllWindows();
		return;
	}
	m_pLog->RecordInfo("Menu IO passed");

	UIWB.CloseAllWindows();
	XSAFETY;
}
 
///////////////////////////////////////////////////////////////////////////////
//	Test Cases

BOOL CMenTestCases::CreateMenuRes(void)
{
	XSAFETY;

	//create a new menu
	COResScript res;
	res.CreateResource(IDSS_RT_MENU);

	//Make sure it is active
	Sleep(WAIT);
	UIMenEdit uMenu = UIWB.GetActiveEditor();
	if (!uMenu.IsValid())
	{
		m_pLog->RecordFailure("Unable to create Menu Resource.  Found %s active.", (LPCSTR)uMenu.GetText());
		return FALSE;
	}

	//Make sure it has the right title
	CString str = uMenu.GetTitle();
	if( str.Find("IDR_MENU") < 0 )
	{		// expect "Script1 - IDR_MENU1 (Menu)"
		m_pLog->RecordFailure("Unable to create Menu Resource.  Found %s active.", (LPCSTR)str);
		return FALSE;
	}
	return TRUE;
}

BOOL CMenTestCases::EnterMenuItems(void)
{
	XSAFETY;
	
	//Create first Menu Item
	MST.DoKeys("&Pop up1");
	MST.DoKeys("~");
	MST.DoKeys("{LEFT}{DOWN}");
	Sleep(WAIT);
	CString strCaption = UIWB.GetProperty(P_Caption);
	CString strPopUp = UIWB.GetProperty(P_Popup);	
	
	//Check to see if we created it correctly
	if ((strCaption != "&Pop up1") || (strPopUp != "1"))
	{
		m_pLog->Comment("Caption = %s, Pop up = %s", (LPCSTR)strCaption, (LPCSTR)strPopUp);
		m_pLog->RecordFailure("Failed to create pop up menu");
		return FALSE;
	}

	//Enter a checked item
	MST.DoKeys("{DOWN}");
	MST.DoKeys("&One");
	Sleep(WAIT);
	MST.WCheckCheck( GetLabel(VRES_IDC_CHECKED) );
	MST.DoKeys("~");
	
	//Enter a grayed item
	MST.DoKeys("&Two");
	Sleep(WAIT);
	MST.WCheckCheck( GetLabel(VRES_IDC_GRAYED) );
	MST.DoKeys("~");

	//Enter a seperator
	MST.DoKeys("Seperate");
	Sleep(WAIT);
	MST.WCheckCheck( GetLabel(VRES_IDC_SEPARATOR) );
	MST.DoKeys("~");

	//Enter an inactive item
	MST.DoKeys("T&hree");
	Sleep(WAIT);
	MST.WCheckCheck( GetLabel(VRES_IDC_INACTIVE) );
	MST.DoKeys("~");

	//Enter a help enabled item
	MST.DoKeys("&Four");
	Sleep(WAIT);
	MST.WCheckCheck( GetLabel(VRES_IDC_HELPMENU) );
	MST.DoKeys("~");

	//Enter an item with a prompt
	MST.DoKeys("F&ive");
	Sleep(WAIT);
	MST.WEditSetText( GetLabel(VRES_IDC_CMDMESSAGE), "Here is a prompt" );
	MST.DoKeys("~");

	//Enter a popup item
	MST.DoKeys("&Six");
	Sleep(WAIT);
	MST.WCheckCheck( GetLabel(VRES_IDC_POPUP) );
	MST.DoKeys("~");

	//Enter an item for the popup 
	MST.DoKeys("Si&x One");
	Sleep(WAIT);
	MST.WCheckCheck( GetLabel(VRES_IDC_GRAYED) );
	MST.DoKeys("~");

	//Enter an item back on main
	MST.DoKeys("{LEFT}");
	MST.DoKeys("{DOWN}");
	Sleep(WAIT);
	MST.DoKeys("ίαρ");
	Sleep(WAIT);
	MST.WCheckCheck( GetLabel(VRES_IDC_CHECKED) );
	MST.DoKeys("~");

	//Check to see if main menu was entered correctly
	MST.DoKeys("{LEFT}");
	Sleep(WAIT);
	if (!CheckCreation("IDR_MENU1",0) ) return FALSE;

	//Check top popup menu
	MST.DoKeys("{DOWN}");
	Sleep(WAIT);
	if (!CheckCreation("&Pop up1",3) ) return FALSE;

	//Check item 1
	MST.DoKeys("{DOWN}");
	Sleep(WAIT);
	if (!CheckCreation("&One",2) ) return FALSE;
	
	//Check item 2
	MST.DoKeys("{DOWN}");
	Sleep(WAIT);
	if (!CheckCreation("&Two",4) ) return FALSE;
	
	//Check seperator
	MST.DoKeys("{DOWN}");
	Sleep(WAIT);
	if (!CheckCreation("Seperator",1) ) return FALSE;

	//Check item 3
	MST.DoKeys("{DOWN}");
	Sleep(WAIT);
	if (!CheckCreation("T&hree",5) ) return FALSE;
	
	//Check item 4
	MST.DoKeys("{DOWN}");
	Sleep(WAIT);
	if (!CheckCreation("&Four",6) ) return FALSE;
	
	//Check item 5
	MST.DoKeys("{DOWN}");
	Sleep(WAIT);
	if (!CheckCreation("F&ive",7) ) return FALSE;
	
	//Check item 6
	MST.DoKeys("{DOWN}");
	Sleep(WAIT);
	if (!CheckCreation("&Six",3) ) return FALSE;
	
	//Check item 6.1
	MST.DoKeys("{RIGHT}");
	Sleep(WAIT);
	if (!CheckCreation("Si&x One",4) ) return FALSE;

	//Check item 7
	MST.DoKeys("{LEFT}");
	MST.DoKeys("{DOWN}");
	Sleep(WAIT);
	if (!CheckCreation("ίαρ",2) ) return FALSE;

	return TRUE;
}


BOOL CMenTestCases::CopyAndPaste(void)
{
	XSAFETY;
	
	//Find item "Four"
	UIWB.DoCommand(ID_EDIT_FIND_DLG, DC_MNEMONIC);
	MST.DoKeys("Four");
	MST.DoKeys("~");

	//See if we found it
	Sleep(WAIT);
	if (!CheckCreation("&Four",6) ) return FALSE;

	//Multiple select next 3 items and change caption to "FooBar"
	MST.DoKeys("+{DOWN 3}");
	Sleep(WAIT);
	MST.DoKeys("FooBar");
	MST.DoKeys("~");

	//See if we changed it on the last item
	MST.DoKeys("{DOWN}");
	MST.DoKeys("{UP}");
	Sleep(WAIT);
	if (!CheckCreation("FooBar",2) ) return FALSE;

	//Undo the change
	UIWB.DoCommand(ID_EDIT_UNDO, DC_ACCEL);

	//Check to see it got changed back
	MST.DoKeys("{DOWN}");
	MST.DoKeys("{UP}");
	Sleep(WAIT);
	if (!CheckCreation("ίαρ",2) ) return FALSE;

	return TRUE;
}
	
BOOL CMenTestCases::SaveOpen(void)
{
	XSAFETY;

	// Save resource 
	if ( UIWB.SaveFileAs(m_strCWD + "testout\\Menu ίαρ.rc", TRUE) == FALSE )
	{
		m_pLog->RecordFailure("File save failed %s", m_strCWD + "testout\\Menu ίαρ.rc");
		return FALSE;
	}
	Sleep(1000);

	//close file
	UIWB.DoCommand(ID_FILE_CLOSE, DC_MNEMONIC);

	//reopen file
	HWND hwnd = UIWB.OpenFile(m_strCWD + "testout\\Menu ίαρ.rc");
	if (hwnd == NULL)
	{
		m_pLog->RecordFailure("File-Open failed on %s", m_strCWD + "testout\\Menu ίαρ.rc");
		return FALSE;
	}

	//Reopen editor
	MST.DoKeys("{DOWN}~{DOWN}~");

	//Make sure it opened
	MST.DoKeys("{HOME}");
	Sleep(WAIT);
	if (!CheckCreation("IDR_MENU1",0) )
	{
		m_pLog->RecordFailure("File not opened correctly");
		return FALSE;
	}

	//Close the editor
	MST.DoKeys("^{F4}");

	//Open string table
	MST.DoKeys("{DOWN}~{DOWN}~");

	//Make sure it opened
	MST.DoKeys("{HOME}");
	CString strID = UIWB.GetProperty(136);	//136 == P_String
	if (strID != "Here is a prompt") 
	{
		m_pLog->RecordFailure("Prompt not saved correctly, returned string: %s", strID);
		return FALSE;
	}

	//Close it again
	UIWB.DoCommand(ID_FILE_CLOSE, DC_MNEMONIC);

	return TRUE;

}

BOOL CMenTestCases::CheckCreation(CString CtrlID, int nState)
{
	CString strID = "Null";
	if ( nState == 0 ) //Top menu has no caption
	{
		//Get ID of selected menu item
		strID = UIWB.GetProperty(P_ID);
		
		//Test with expected value
		if (strID != CtrlID)
		{
			m_pLog->RecordFailure("Incorrect ID given to new menu item: %s expected %s", strID, CtrlID);
			return FALSE;
		}
	}
	else if ( nState != 1 ) //Skip seperator
	{
		//Get caption of selected menu item
		strID = UIWB.GetProperty(P_Caption);
		
		//Test with expected value
		if (strID != CtrlID)
		{
			m_pLog->RecordFailure("Incorrect caption given to new menu item: %s expected %s", strID, CtrlID);
			return FALSE;
		}
	}


	//Check for proper checkmark
	CString strProp = "1";
	CString Prop = "Null";
	switch (nState)
	{
	case 1:		//Separator
		Prop = "Separator";
		strProp = UIWB.GetProperty(P_Separator);
		break;
	case 2:		//Checked
		Prop = "Checked";
		strProp = UIWB.GetProperty(P_Checked);
		break;
	case 3:		//Popup
		Prop = "Popup";
		strProp = UIWB.GetProperty(P_Popup);
		break;
	case 4:		//Grayed
		Prop = "Grayed";
		strProp = UIWB.GetProperty(P_Grayed);
		break;
	case 5:		//Inactive
		Prop = "Inactive";
		strProp = UIWB.GetProperty(P_Inactive);
		break;
	case 6:		//Help
		Prop = "Help";
		strProp = UIWB.GetProperty(P_Help);
		break;
	case 7:		//Prompt
		strProp = UIWB.GetProperty(P_CmdMessage);
		if (strProp != "Here is a prompt")
		{
			m_pLog->RecordFailure("Menu item %s did not create prompt correctly", strID);
			return FALSE;
		}
		strProp = "1";
		break;
	}
	
	if (strProp != "1")
	{
		m_pLog->RecordFailure("Menu item %s not %s", strID, Prop);
		return FALSE;
	}
	
	return TRUE;
}

