///////////////////////////////////////////////////////////////////////////////
//	DLGCASES.CPP
//
//	Created by :			Date :
//		EnriqueP				8/13/93
//
//	Edited By :				Date :
//		ScottSe					8/29/96
//
//	Description :
//		Implementation of the CDlgTestCases class
//

#include "stdafx.h"
#include "dlgcases.h"

#define new DEBUG_NEW

#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;

IMPLEMENT_TEST(CDlgTestCases, CTest, "Dialog Editor Tests", -1, CSniffDriver)

void CDlgTestCases::Run(void)
{
	// Delete previous files
	KillAllFiles(m_strCWD + "TESTOUT", FALSE);

	//Create new dialog
	if ( CreateDlgRes() == FALSE )
 	{
		m_pLog->RecordFailure("Dialog creation failed, devres.pkg not loaded");
		UIWB.CloseAllWindows();
		return;
	}
	m_pLog->RecordInfo("Dialog creation passed");

	// Put some controls on it
	if ( DropControls() == FALSE )
 	{
		m_pLog->RecordFailure("Dialog items creation failed, possible timing issue");
		UIWB.CloseAllWindows();
		return;
	}
	m_pLog->RecordInfo("Dialog items creation passed");

	//Find, copy, paste, etc.
	if ( FindControls() == FALSE )
 	{
		m_pLog->RecordFailure("Dialog edit operations failed, possible timing issue");
		UIWB.CloseAllWindows();
		return;
	}
	m_pLog->RecordInfo("Dialog edit operations passed");

	// Save & Open
	if ( SaveOpen() == FALSE )
 	{
		m_pLog->RecordFailure("Failed Dialog IO test");
		UIWB.CloseAllWindows();
		return;
	}
	m_pLog->RecordInfo("Dialog IO passed");

	// Test Dialog
	if ( TestDialog() == FALSE )
 	{
		m_pLog->RecordFailure("Failed Dialog test mode");
		UIWB.CloseAllWindows();
		return;
	}
	m_pLog->RecordInfo("Dialog test mode passed");

	UIWB.CloseAllWindows();
	XSAFETY;
}

///////////////////////////////////////////////////////////////////////////////
//	Test Cases

BOOL CDlgTestCases::CreateDlgRes(void)	  
{
	COResScript res;

	XSAFETY;

	res.CreateResource(IDSS_RT_DIALOG);

	m_uDlgEd = UIWB.GetActiveEditor();
	if (!m_uDlgEd.IsValid()) {
		m_pLog->RecordFailure("Unable to create Dialog Resource");
		return FALSE;
	}

	//Turn off guides
	Sleep(500);
	UIWB.DoCommand(IDM_GRIDSETTINGS,  DC_MNEMONIC);
	Sleep(500);
	MST.WOptionClick( GetLabel(VRES_IDC_GUIDES0) );
	MST.DoKeys("~");

	//Resize dialog height
	MST.DoKeys("+{DOWN 60}");

	//Resize dialog height
	//MST.DoKeys("+{DOWN 165}");

	//Resize dialog width
	//MST.DoKeys("+{RIGHT 40}");

	// Close and reopen dialog
	MST.DoKeys("^{F4}");	
	MST.DoKeys("~");
	m_uDlgEd = UIWB.GetActiveEditor();

	return TRUE;
}

BOOL CDlgTestCases::DropControls(void)
{
	XSAFETY;
	
	//Drop picture control
	m_uDlgEd.DropControl(CT_PICTURE, 10, 10 );
	
	//Drop static text control
	m_uDlgEd.DropControl(CT_TEXT, 10, 55 );
	
	//Drop edit control
	m_uDlgEd.DropControl(CT_EDIT, 10, 80 );
	
	//Drop group box control
	m_uDlgEd.DropControl(CT_GROUP, 120, 10 );
	
	//Drop button control
	m_uDlgEd.DropControl(CT_BUTTON, 230, 80 );
	
	//Drop check box control
	m_uDlgEd.DropControl(CT_CHECK, 125, 30 );

	//Drop radio button control
	m_uDlgEd.DropControl(CT_RADIO, 125, 50 );

	//Drop combo box control
	m_uDlgEd.DropControl(CT_DROPDOWN, 120, 100 );

	//Drop list box control
	m_uDlgEd.DropControl(CT_LIST, 229, 125 );

	//Drop hscroll control
	m_uDlgEd.DropControl(CT_HSCROLL, 10, 105 );
	
	//Drop vscroll control
	m_uDlgEd.DropControl(CT_VSCROLL, 175, 10 );

	//Drop spin control
	m_uDlgEd.DropControl(CT_SPIN, 60, 10 );

	//Drop progress control
	m_uDlgEd.DropControl(CT_PROGRESS, 10, 130 );

	//Drop slider control
	m_uDlgEd.DropControl(CT_SLIDER, 10, 160 );

	//Drop hot key control
	m_uDlgEd.DropControl(CT_HOTKEY, 10, 185 );

	//Drop animate control
	m_uDlgEd.DropControl(CT_ANIMATE, 175, 100 );

	//Drop rich edit control
	m_uDlgEd.DropControl(CT_RICHEDIT, 155, 135 );

	//Drop tab control
	m_uDlgEd.DropControl(CT_TAB, 125, 225 );

	//Drop listctrl control
	m_uDlgEd.DropControl(CT_LISTCTL, 225, 205 );

	//Drop date/time picker control
	//m_uDlgEd.DropControl(CT_DATETIMEPICKER, 10, 275 );

	//Drop calendar control
	//m_uDlgEd.DropControl(CT_MONTHCALENDAR, 240, 340 );

	//Drop IP address control
	//m_uDlgEd.DropControl(CT_IPADDRESS, 10, 305 );

	//Drop custom control
	m_uDlgEd.DropControl(CT_USER, 10, 215 );

	//Check to see that the dialog was created with proper ID
	if ( CheckCreation( "IDD_DIALOG1" ) == FALSE )
		return FALSE;

	//Check to see that the next control is an IDOK
	if ( CheckCreation( "IDOK" ) == FALSE )
		return FALSE;

	//Check to see that the next control is an IDCANCEL
	if ( CheckCreation( "IDCANCEL" ) == FALSE )
		return FALSE;

	//Check to see that the picture control was created with the proper ID
	if ( CheckCreation( "IDC_STATIC" ) == FALSE )
		return FALSE;

	//Check to see that the static text control was created with the proper ID
	if ( CheckCreation( "IDC_STATIC" ) == FALSE )
		return FALSE;

	//Check to see that the edit control was created with the proper ID
	if ( CheckCreation( "IDC_EDIT1" ) == FALSE )
		return FALSE;

	//Check to see that the group box control was created with the proper ID
	if ( CheckCreation( "IDC_STATIC" ) == FALSE )
		return FALSE;

	//Check to see that the button control was created with the proper ID
	if ( CheckCreation( "IDC_BUTTON1" ) == FALSE )
		return FALSE;

	//Check to see that the check box control was created with the proper ID
	if ( CheckCreation( "IDC_CHECK1" ) == FALSE )
		return FALSE;

	//Check to see that the radio control was created with the proper ID
	if ( CheckCreation( "IDC_RADIO1" ) == FALSE )
		return FALSE;

	//Check to see that the combo control was created with the proper ID
	if ( CheckCreation( "IDC_COMBO1" ) == FALSE )
		return FALSE;

	//Check to see that the list control was created with the proper ID
	if ( CheckCreation( "IDC_LIST1" ) == FALSE )
		return FALSE;

	//Check to see that the horizontal scrollbar control was created with the proper ID
	if ( CheckCreation( "IDC_SCROLLBAR1" ) == FALSE )
		return FALSE;

	//Check to see that the vertical scrollbar control was created with the proper ID
	if ( CheckCreation( "IDC_SCROLLBAR2" ) == FALSE )
		return FALSE;

	//Check to see that the spin control was created with the proper ID
	if ( CheckCreation( "IDC_SPIN1" ) == FALSE )
		return FALSE;

	//Check to see that the progress control was created with the proper ID
	if ( CheckCreation( "IDC_PROGRESS1" ) == FALSE )
		return FALSE;

	//Check to see that the slider control was created with the proper ID
	if ( CheckCreation( "IDC_SLIDER1" ) == FALSE )
		return FALSE;

	//Check to see that the hot key control was created with the proper ID
	if ( CheckCreation( "IDC_HOTKEY1" ) == FALSE )
		return FALSE;

	//Check to see that the animate control was created with the proper ID
	if ( CheckCreation( "IDC_ANIMATE1" ) == FALSE )
		return FALSE;

	//Check to see that the rich edit control was created with the proper ID
	if ( CheckCreation( "IDC_RICHEDIT1" ) == FALSE )
		return FALSE;

	//Check to see that the tab control was created with the proper ID
	if ( CheckCreation( "IDC_TAB1" ) == FALSE )
		return FALSE;

	//Check to see that the listctrl control was created with the proper ID
	if ( CheckCreation( "IDC_LIST2" ) == FALSE )
		return FALSE;

	//Check to see that the date/time picker control was created with the proper ID
	//if ( CheckCreation( "IDC_DATETIMEPICKER1" ) == FALSE )
	//	return FALSE;

	//Check to see that the calendar control was created with the proper ID
	//if ( CheckCreation( "IDC_MONTHCALENDAR1" ) == FALSE )
	//	return FALSE;

	//Check to see that the IP address control was created with the proper ID
	//if ( CheckCreation( "IDC_IPADDRESS1" ) == FALSE )
	//	return FALSE;

	//Check to see that the custom control was created with the proper ID
	if ( CheckCreation( "IDC_CUSTOM1" ) == FALSE )
		return FALSE;
	
	return TRUE;
}

BOOL CDlgTestCases::FindControls(void)
{
	XSAFETY;

	//Search for progress control
	UIWB.DoCommand(ID_EDIT_FIND_DLG, DC_MNEMONIC);
	MST.DoKeys("prog");
	MST.DoKeys("~");

	//Check to see that we found it
	MST.DoKeys("+{TAB}");
	if ( CheckCreation( "IDC_PROGRESS1" ) == FALSE )
		return FALSE;

	//Search for button
	UIWB.DoCommand(ID_EDIT_FIND_DLG, DC_MNEMONIC);
	MST.DoKeys("button");
	MST.DoKeys("~");

	//Check to see that we found it
	MST.DoKeys("+{TAB}");
	if ( CheckCreation( "IDC_BUTTON1" ) == FALSE )
		return FALSE;

	//Change the caption
	MST.DoKeys("Scott's Button");
	MST.DoKeys("~");

	//Check to see we changed it correctly
	CString strCap = UIWB.GetProperty(P_Caption);
	if ( strCap != "Scott's Button") 
	{
		m_pLog->RecordFailure("Button caption not changed to Scott's Button: %s", strCap);
		return FALSE;
	}

	//Undo the change
	UIWB.DoCommand(ID_EDIT_UNDO, DC_MNEMONIC);
	
	//Check to see we changed it back to Button1
	strCap = UIWB.GetProperty(P_Caption);
	if ( strCap != "Button1") 
	{
		m_pLog->RecordFailure("Button caption not undo'd correctly to Button1: %s", strCap);
		return FALSE;
	}
	
	return TRUE;
}

BOOL CDlgTestCases::TestDialog(void)
{
	XSAFETY;
	
	// Initiate test mode
	UIWB.DoCommand(IDMY_TEST, DC_MNEMONIC);
	UIDialog uTestDlg;
	uTestDlg.WaitAttachActive(3000);
	char acBuf[256];
	uTestDlg.GetText(acBuf, 255); 

	//Test controls
	MST.DoKeys("{TAB 2}");
	MST.DoKeys("Hi there");		//Editbox
	MST.DoKeys("{TAB 2}");
	MST.DoKeys("{SPACE}");		//Check box
	MST.DoKeys("{TAB}");
	MST.DoKeys("Hi again");		//Combo box
	MST.DoKeys("{TAB}");
	MST.DoKeys("s");			//List box
	MST.DoKeys("{TAB}");
	MST.DoKeys("{RIGHT 3}");	//Slider
	MST.DoKeys("{TAB}");
	MST.DoKeys("F");			//Hotkey
	MST.DoKeys("{TAB 2}");
	MST.DoKeys("Rich");			//Rich Edit box
	MST.DoKeys("{TAB}");
	MST.DoKeys("m");			//Listctrl
	//MST.DoKeys("{TAB}");
	//MST.DoKeys("{DOWN 3}");		//Date/Time Picker
	//MST.DoKeys("{TAB}");
	//MST.DoKeys("{PGDN 3}");		//Calendar
	//MST.DoKeys("{TAB}");
	//MST.DoKeys("123123123123");	//IP Address edit control
	MST.DoKeys("~");	// Hit return to exit from test mode

	// see if we got the right window
	//HWND hwnd = MST.WFndWnd("Dialog ίαρ.rc - IDD_DIALOG1 (Dialog)",  FW_DEFAULT);
	//m_pLog->Comment("Dialog ίαρ.rc wnd handle %d", hwnd);
	bool result = !uTestDlg.IsValid();
	m_pLog->Comment("uTestDlg not valid %d", result);
	result = m_uDlgEd.IsValid();
	m_pLog->Comment("m_uDlgEd is valid %d", result);
	if(!m_uDlgEd.IsValid() || uTestDlg.IsValid())
		m_pLog->Comment("Test mode should pass");

	//hwnd = m_uDlgEd.GetDialog();
	//m_pLog->Comment("m_uDlgEd wnd handle %d", result);

	//test we got back ok
	if ((strcmp(acBuf, GetLocString(IDSS_DIALOG)) != 0) ||
		!m_uDlgEd.IsValid() || uTestDlg.IsValid())
	{
		m_pLog->RecordFailure("Dialog Test mode failed");
		m_pLog->Comment("Active Wnd %s = %x",acBuf, uTestDlg);
		return FALSE;
	}
	
	//close file
	UIWB.DoCommand(ID_FILE_CLOSE, DC_MNEMONIC);

	return TRUE;
}

BOOL CDlgTestCases::CheckCreation(CString CtrlID)
{
	//Test the ID of the control next in tab order

	//Send tab & wait
	MST.DoKeys("{TAB}");
	Sleep(500);

	//Get ID of selected control
	CString strID = UIWB.GetProperty(P_ID);
	
	//Test with expected value
	if (strID != CtrlID)
	{
		m_pLog->RecordFailure("Incorrect ID given to new control: %s expected %s", strID, CtrlID);
		return FALSE;
	}

	return TRUE;

}

BOOL CDlgTestCases::SaveOpen(void)
{
	XSAFETY;

	//save file
	if ( UIWB.SaveFileAs(m_strCWD + "testout\\Dialog ίαρ.rc", TRUE) == FALSE )
	{
		m_pLog->RecordFailure("File save failed %s", m_strCWD + "testout\\Dialog ίαρ.rc");
		return FALSE;
	}

	//close file
	UIWB.DoCommand(ID_FILE_CLOSE, DC_MNEMONIC);

	//reopen file
	HWND hwnd = UIWB.OpenFile(m_strCWD + "testout\\Dialog ίαρ.rc");
	if (hwnd == NULL)
	{
		m_pLog->RecordFailure("File-Open: hwnd == %x", hwnd);
		return FALSE;
	}

	//Reopen editor
	MST.DoKeys("{DOWN}~{DOWN}~");

	// Get the active dialog editor
	m_uDlgEd = UIWB.GetActiveEditor();

	//Make sure it opened
	if ( CheckCreation( "IDOK" ) == FALSE )
	{
		m_pLog->RecordFailure("File not opened");
		return FALSE;
	}

	return TRUE;
}