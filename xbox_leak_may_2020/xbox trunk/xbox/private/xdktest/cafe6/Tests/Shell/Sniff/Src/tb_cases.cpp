///////////////////////////////////////////////////////////////////////////////
//	TB_CASES.CPP
//
//	Created by :			Date :
//		Enriquep					9/3/93
//
//	Description :
//		Implementation of the CTBTestCases class
//

#include "stdafx.h"
#include "tb_cases.h"

#define new DEBUG_NEW

#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;

IMPLEMENT_TEST(CTBTestCases, CTest, "Toolbar Tests", -1, CSniffDriver)

void CTBTestCases::Run(void)
{
	XSAFETY;
	ViewToolbars();
	HideToolbar();
	ShowToolbar();
	NewToolbar();
	AddButton();
	NewTB();  

	// Close the toolbars and any open text windows
	WS.ShowToolbar(IDTB_VCPP_DEBUG, FALSE);

	// Select the last item in the list
	int nCount = MST.WListCount(GetLabel(0x5018));
	MST.WListItemClk(GetLabel(0x5018), nCount);
	Sleep(1000);
	
	if (MST.WButtonEnabled(GetLabel(0x5019))) {
		// Click the Delete button twice to remove the custom toolbars we created
		MST.WButtonClick(GetLabel(0x5019));
		MST.WButtonClick(GetLabel(0x5019));
	}
	else {
		m_pLog->RecordFailure("Delete button not enabled on Toolbar dialog.");
	}

	// Click the Close button on the Customize dialog
	MST.WButtonClick(GetLabel(1));

	WS.CloseAllWindows();
}

///////////////////////////////////////////////////////////////////////////////
//	Test Cases

BOOL CTBTestCases::ViewToolbars(void)
{
	UIToolbar uTB;

	if (!uTB.DisplayTBDlg())
	{
		m_pLog->RecordFailure("(1) View toolbars");
		return FALSE;
	} 
	return TRUE;
}


BOOL CTBTestCases::HideToolbar(void)
{
	if (!WS.ShowToolbar(IDTB_SUSHI_STANDARD, FALSE) )
	{
		m_pLog->RecordFailure("(2) Hide Toolbar");
		return FALSE;
	} 
	return TRUE;
}


BOOL CTBTestCases::ShowToolbar(void)
{
	if (!WS.ShowToolbar(IDTB_VCPP_DEBUG, TRUE) )
	{
		m_pLog->RecordFailure("(3) Show Toolbar");
		return FALSE;
	}
	return TRUE;
}

BOOL CTBTestCases::NewToolbar(void)
{
	UIToolbar uTB;

	uTB.DisplayTBDlg();
		
	MST.WButtonClick(GetLabel(VSHELL_IDC_NEW));

#ifdef _M_ALPHA
    Sleep(1000); // Slow it down so the dlg has a chance to appear
#endif

	EXPECT(MST.WFndWndWait(GetLocString(IDSS_NEW_TB_TITLE), FW_PART, 1) != 0);	// Wait for New Toolbar Dialog
   
	MST.DoKeys("Cool TB");						// Name new toolbar
	MST.WButtonClick(GetLabel(IDOK));
	
	EXPECT(MST.WFndWndWait(GetLocString(IDSS_CUSTOMIZE_TITLE), FW_PART, 1) != 0);	// Wait for Customize Dialog
		
	HWND hTB = UIWB.GetToolbar(IDTB_CUSTOM_BASE + 1,PACKAGE_SUSHI);
	EXPECT(hTB != NULL);
   
	if (!IsWindowVisible(hTB))
	{
		m_pLog->RecordFailure("(4) New Toolbar");
		return FALSE;
	}

	return TRUE;
}

BOOL CTBTestCases::AddButton(void)
{
	UICustomizeTabDlg uiCustDlg;
	uiCustDlg.Display();
	HWND hTB = UIWB.GetToolbar(IDTB_CUSTOM_BASE + 1,PACKAGE_SUSHI);
	EXPECT( hTB != NULL );
	
	uiCustDlg.AddTBButton(1, hTB);	 // REVIEW: first parameter is ignored
	MST.WButtonClick(GetLabel(IDOK)); 		// Close dialog
	ClickMouse(VK_LBUTTON, hTB, 10, 10);		// Click on toolbar button
	
	if (MST.WFndWndWait(GetLocString(IDSS_NEW_TITLE), FW_PART, 10) == 0)
	{
		m_pLog->RecordFailure("(5) Add Button");
		return FALSE;
	}
	MST.WButtonClick(GetLabel(IDCANCEL));

	return TRUE;					 
}


BOOL CTBTestCases::NewTB(void)
{
	UICustomizeTabDlg uiCustDlg;
	uiCustDlg.Display();
	uiCustDlg.AddTBButton(1);	 // REVIEW: first parameter is ignored

	HWND hTB = UIWB.GetToolbar(IDTB_CUSTOM_BASE + 2,PACKAGE_SUSHI);
	MST.WButtonClick(GetLabel(IDOK)); 		// Close Toolbar dialog
	EXPECT( hTB != NULL );

	if (!IsWindowVisible(hTB))
	{
		m_pLog->RecordFailure("(6) New Toolbar via Customize Dlg");
		return FALSE;
	}

	return TRUE;
}		
