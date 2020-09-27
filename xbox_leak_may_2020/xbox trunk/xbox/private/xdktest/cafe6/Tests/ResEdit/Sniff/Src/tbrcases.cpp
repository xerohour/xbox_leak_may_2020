///////////////////////////////////////////////////////////////////////////////
//	TBRCASES.CPP
//
//	Created by :			Date :
//		MikePie				3/27/95
//
//	Description :
//		Implementation of the CTbrTestCases class
//
//	7/7/95		M.Pietraszak	Used to change focus (F6), now closes (F4)
//	8/13/96		ScottSe			Separated %(MT) & %(MI) 
//

#include "stdafx.h"
#include "Tbrcases.h"

#define new DEBUG_NEW

#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;

IMPLEMENT_TEST(CTbrTestCases, CTest, "Toolbar Editor Tests", -1, CSniffDriver)

void CTbrTestCases::Run(void)
{
	HWND hwndRC;

	if ((hwndRC = UIWB.CreateNewFile(GetLocString(IDSS_NEW_RC))) == NULL)
	{
		m_pLog->RecordFailure("Could not create RC script");
		UIWB.CloseAllWindows();
		return;
	}
	else
	{
		m_pLog->RecordInfo("Created RC script");
		NewToolbar();		// Create a bitmap resource	
		ModifyToolbar();	// Change the toolbar
		CheckWidth();		// Checks the toolbar button size
		CheckHeight();		// Checks the toolbar button size
 		CloseToolbar();			
	}
	UIWB.CloseAllWindows();
	return;
}

///////////////////////////////////////////////////////////////////////////////
//	Test Cases

BOOL CTbrTestCases::NewToolbar(void)
{
	COResScript res;

	Sleep(1000);
	if (res.CreateResource(IDSS_RT_BITMAP) == ERROR_ERROR) 
 	{
		m_pLog->RecordFailure("Could not create new Bitmap.");
		return FALSE;
	}
	else
	{
		m_pLog->RecordInfo("Created new Bitmap."); 
		return TRUE;
	}

}

BOOL CTbrTestCases::ModifyToolbar(void)
{
	m_pLog->RecordInfo("Converting Bitmap into Toolbar...");

	Sleep(6000);
 	UIWB.DoCommand( IDM_INVERTCOLORS, DC_MNEMONIC );     // I(m)age.(I)nvert colors
	Sleep(1000);

 	UIWB.DoCommand( IDM_TOOLBAR_WIZARD, DC_MNEMONIC );     // I(m)age.(T)oolbar editor
	Sleep(2000);
	MST.DoKeys("24");			// Width = 24
	MST.DoKeys("{TAB}");		// Go to height...
	Sleep(500);
	MST.DoKeys("23");			// Height = 23
	MST.DoKeys("{TAB}");		// Go to OK button
	Sleep(500);
	MST.DoKeys("{ENTER}");		// Click on OK
	Sleep(800);
	MST.DoKeys("{ENTER}");		// Image will be converted - OK
	Sleep(2000);
	MST.DoKeys("^({F4})");		// Ctrl+F4 will close the toolbar editor
	Sleep(1000);
	MST.DoKeys("{UP}");			// Move up to the toolbar folder
	MST.DoKeys("{ENTER}");		// Close the toolbar folder
	MST.DoKeys("{ENTER}");		// Open the toolbar folder
	Sleep(2000);
	MST.DoKeys("{DOWN}");		// Move from the toolbar folder to a specific resource
	Sleep(800);
	MST.DoKeys("{ENTER}");		// Will open the toolbar editor again
	Sleep(800);
	MST.DoKeys("{RIGHT}");		// Select the second toolbar button
	Sleep(400);
	MST.DoKeys("%({ENTER})");	// Check out the properties of the second button

	// This was bombing out on a faster machine.
	Sleep(2000);
	CString cstrButtonId = UIWB.GetProperty(P_ID);	

	if (cstrButtonId != "ID_BUTTON40002")
	{
		m_pLog->RecordFailure("Button ID not correct. Expected ID_BUTTON40002, but");
		m_pLog->RecordFailure("got id = %s instead.",cstrButtonId);
		m_pLog->RecordFailure("----------------------------------");
		m_pLog->RecordFailure("Failure due to Olympus 13073 bug. ");
		m_pLog->RecordFailure("----------------------------------");
		return FALSE;
	}
	else
	{
		m_pLog->RecordInfo("Got id = %s as expected.",cstrButtonId);
		return TRUE;
	}
	

}

BOOL CTbrTestCases::CheckWidth(void)
{
	m_pLog->RecordInfo("Checking Toolbar button width...");

	Sleep(2000);
	CString cstrButtonWidth = UIWB.GetProperty(P_Width);

	if (cstrButtonWidth != "24")
	{
		m_pLog->RecordFailure("Button width is not correct. Expected 24, but");
		m_pLog->RecordFailure("got width = %s instead.",cstrButtonWidth);
		m_pLog->RecordFailure("----------------------------------");
		m_pLog->RecordFailure("Failure due to Olympus 13073 bug. ");
		m_pLog->RecordFailure("----------------------------------");
		return FALSE;
	}
	else
	{
		m_pLog->RecordInfo("Got width = %s as expected.",cstrButtonWidth);
		return TRUE;
	}
	

}

BOOL CTbrTestCases::CheckHeight(void)
{
	m_pLog->RecordInfo("Checking Toolbar button height...");

	Sleep(2000);
	CString cstrButtonHeight = UIWB.GetProperty(P_Height);	

	if (cstrButtonHeight != "23")
	{
		m_pLog->RecordFailure("Button height is not correct. Expected 23, but");
		m_pLog->RecordFailure("got height = %s instead.",cstrButtonHeight);
		m_pLog->RecordFailure("----------------------------------");
		m_pLog->RecordFailure("Failure due to Olympus 13073 bug. ");
		m_pLog->RecordFailure("----------------------------------");
		return FALSE;
	}
	else
	{
		m_pLog->RecordInfo("Got height = %s as expected.",cstrButtonHeight);
		return TRUE;
	}
	

}




BOOL CTbrTestCases::CloseToolbar(void)
{
	MST.DoKeys("{ESC}");		// Close properties
	MST.DoKeys("^({F4})");		// Close editor	 

	Sleep(2000);
	UIEditor ed = UIWB.GetActiveEditor();
	EXPECT( ed.IsValid() );
	if( !ed.Close() )
	{
		m_pLog->RecordFailure("Could not close Toolbar editor.");
		m_pLog->RecordFailure("----------------------------------");
		m_pLog->RecordFailure("Failure due to Olympus 13073 bug. ");
		m_pLog->RecordFailure("----------------------------------");
		return FALSE;
	}
	else
	{
		m_pLog->RecordInfo("Closed Toolbar editor.");
		return TRUE;
	}

}
 