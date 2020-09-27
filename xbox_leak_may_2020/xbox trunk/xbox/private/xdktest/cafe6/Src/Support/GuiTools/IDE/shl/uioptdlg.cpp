///////////////////////////////////////////////////////////////////////////////
//  UITABDLG.CPP
//
//  Created by :            Date :
//      EnriqueP              1/27/94
//				  
//  Description :
//      Implementation of the UIOptionTabDialog class
//

#include "stdafx.h"
//#include "..\sym\vcpp32.h"
#include "uioptdlg.h"
#include "mstwrap.h"
#include "guiv1.h"
#include "..\sym\cmdids.h"
#include "..\sym\vproj.h"
#include "uiwbmsg.h"
#include "..\sym\vshell.h"
#include "uwbframe.h"
#include "..\..\testutil.h"

#define new DEBUG_NEW

#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;

// BEGIN_HELP_COMMENT
// Function: HWND UIOptionsTabDlg::Display() 
// Description: Bring up the Options tabbed dialog.
// Return: The HWND of the Options tabbed dialog.
// END_HELP_COMMENT
HWND UIOptionsTabDlg::Display() 
{	
	AttachActive();		  // Check if is already up
	if (!IsValid())
	{
	 UIWB.DoCommand(IDM_OPTIONS, DC_MNEMONIC);
	 WaitAttachActive(10000);
	}
	return WGetActWnd(0); 
}

// BEGIN_HELP_COMMENT
// Function: HWND UIOptionsTabDlg::ChangeCompatility() 
// Description: Display the compatibility tab of the options dialog.
// Return: int
// END_HELP_COMMENT

int UIOptionsTabDlg::ChangeCompatility(int Scheme)
{
	// Add a 1 to the scheme because schemes are zero based, but MSTest demamds a 1 based.
	Scheme++ ;
	ActivateCompaTab() ;
	MST.WComboItemClk(GetLabel(VCPP32_IDC_EMULATION),Scheme) ;
	MST.DoKeys("{ENTER}") ;
	UIWBMessageBox mbAlert;
	while( mbAlert.WaitAttachActive(1000) )	// if any alerts come up
	{
		if (mbAlert.ButtonExists(mbAlert.m_idOKButton)) 
			MST.DoKeys("{ESC 2}");
		else
		MST.DoKeys("{ENTER}"); // click YES button
    }
	return TRUE ;
}

int UIOptionsTabDlg::ActivateCompaTab()
{

	Display() ;
	ShowPage(TAB_COMPATIBILITY, 8);
	return TRUE ;
}



// BEGIN_HELP_COMMENT
// Function: int UIOptionsTabDlg::AddDirectory( LPCSTR szNewDir, int iDirType, int iPlatform)
// Description: Add a directory to one of the environment variables editable on the Directories tab.
// Return: ERROR_SUCCESS if successful; ERROR_ERROR otherwise.
// Param: szNewDir A pointer to a string containing the directory to add.
// Param: iDirType The environment variable to set: DIR_DIRTYPE_EXE, DIR_DIRTYPE_INC, DIR_DIRTYPE_LIB.
// Param: iPlatform The platform to set the environment variable for: DIR_PLATFORM_WIN32, DIR_PLATFORM_DUMMY
// END_HELP_COMMENT
int UIOptionsTabDlg::AddDirectory( LPCSTR szNewDir, int iDirType, int iPlatform, int atTop)
{
	Display();
	 
	if (iPlatform != -1)
		SetOption( TAB_DIRECTORIES, DIR_PLATFORM, iPlatform );

	if (iDirType != -1)
		SetOption( TAB_DIRECTORIES, DIR_DIRTYPE, iDirType );

	ShowPage(TAB_DIRECTORIES, 8);

	if (atTop)
	{
		// give focus to the Directories list box (which really isn't a list box so we can't use
		// the MST.WList routines) and select the first item.
		//MST.WComboIteDoKeys("%d{HOME}");
		CString HK = "%" ;
		HK += ExtractHotKey(GetLocString(IDS_DIRECTORIES));
		MST.DoKeys(HK.GetBuffer(HK.GetLength()));
		MST.DoKeys("{HOME}") ;
	}
	else 
	{
		//set focus to empty directory list item & go to bottom.
		// give focus to the Directories list box (which really isn't a list box so we can't use
		// the MST.WList routines) and select the first item.
		//MST.WComboIteDoKeys("%d{END}");
		CString HK = "%" ;
		HK += ExtractHotKey(GetLocString(IDS_DIRECTORIES));
		MST.DoKeys(HK.GetBuffer(HK.GetLength()));
		MST.DoKeys("{END}") ;
	}
	MST.DoKeys(szNewDir); //enter new directory name
	MST.DoKeys("{ENTER}");
		//REVIEW (DorianG): kill the DoKeys crap and do this the right way

	UIWBMessageBox mbAlert;
	while( mbAlert.WaitAttachActive(1000) )	// if any alerts come up
	{
		if (mbAlert.ButtonExists(mbAlert.m_idOKButton)) 
			MST.DoKeys("{ESC 2}");
		else
		MST.DoKeys("{LEFT} "); // click YES button
	}
				   
	//UIWBMessageBox mbAlert;
	//while( mbAlert.WaitAttachActive(1000) )	// if any alerts come up
	//	mbAlert.ButtonClick();				// click default button
	
	// REVIEW(EnriqueP): Add Verification code and return values

	return ERROR_SUCCESS;

}

int UIOptionsTabDlg::ShowDirectoryTab()	 
{
	Display();
	ShowPage(TAB_DIRECTORIES, 8);
	return TRUE ;
}

// BEGIN_HELP_COMMENT
// Function: int UIOptionsTabDlg::RestoreWnd(BOOL state /* = TRUE */)
// Description: Set the state of the 'Restore Window Layout When Opening Projects' option in the Workspace tab.
// Return: ERROR_SUCCESS if successful; ERROR_ERROR otherwise.
// Param: state A Boolean value that specifies the state of the option. TRUE to set it to restore the layout; FALSE to unset it. (Default value is TRUE.)
// END_HELP_COMMENT
int UIOptionsTabDlg::RestoreWnd( BOOL state /* = TRUE */)
{
	Display();
	ShowPage(TAB_WORKSPACE, 8);
	if (state)
		MST.WCheckCheck(GetLabel(VSHELL_IDC_AUTO_WORKSPACE)) ;
	else
		MST.WCheckUnCheck(GetLabel(VSHELL_IDC_AUTO_WORKSPACE)) ;

	MST.WButtonClick(GetLabel(IDOK)) ;

	return ERROR_SUCCESS;
}

// BEGIN_HELP_COMMENT
// Function: int UIOptionsTabDlg::SetFont(int FontIndex, int Size /* = 11 */, CString Category /* = NULL*/)
// Description: Set the font in the Font tab by index. (See UIOptionsTabDlg::SetFont(LPCSTR, int, LPCSTR, LPCSTR) to set the font by name.)
// Return: ERROR_SUCCESS if successful; ERROR_ERROR otherwise.
// Param: FontIndex An integer that specifies the 1-based index into the list of fonts.
// Param: Size An integer that specifies the point size of the font. This parameter is NYI. (Default value is 11.)
// Param: Category A CString that specifies the category to set the font for. This parameter is NYI.
// END_HELP_COMMENT
int UIOptionsTabDlg::SetFont( int FontIndex, int Size /* = 11 */, CString Category /* = NULL*/)
{
	Display();
	ShowPage("Format");

	MST.WComboItemClk(GetLabel(VSHELL_IDC_COMBO_FONT_NAME),FontIndex) ;
	MST.WButtonClick(GetLabel(IDOK)) ;
	

	return ERROR_SUCCESS;
}

// BEGIN_HELP_COMMENT
// Function: int UIOptionsTabDlg::SetFont(LPCSTR pszFontName /*= NULL*/, int iFontSize /*= 0*/, LPCSTR pszFontStyle /*= NULL*/, LPCSTR pszCategory /*= NULL*/)
// Description: Set the font in the Font tab by name. (See UIOptionsTabDlg::SetFont(int, int, CString) to set the font by index.)
// Return: ERROR_SUCCESS if successful; ERROR_ERROR otherwise.
// Param: pszFontName A pointer to a string that contains the name of the font. NULL means use the current font. (Default value is NULL.)
// Param: iFontSize An integer that specifies the point size of the font. 0 means use the current point size. (Default value is 0.)
// Param: pszFontStyle A pointer to a string that contains the name of the font style. NULL means use the current style. (Default value is NULL.)
// Param: pszCategory A pointer to a string that contains the name of the category to set the font for. NULL means use the current category. This parameter is NYI. (Default value is NULL.)
// END_HELP_COMMENT
int UIOptionsTabDlg::SetFont(LPCSTR pszFontName /*= NULL*/, int iFontSize /*= 0*/, LPCSTR pszFontStyle /*= NULL*/, LPCSTR pszCategory /*= NULL*/)
{
	int nResult = ERROR_SUCCESS;
	CString strVerify;
	const int ccBuf = 20;
	char acBuf[ccBuf];

	// display the options tabbed dialog
	Display();

	// go to the font page
	ShowPage("Fonts");

	// set the font category, if specified
	if (pszCategory) {
		// select the category
		MST.WListItemClk(GetLabel(VSHELL_IDC_LIST_WINDOWS), pszCategory);
		// get the category name that was et
		MST.WListText(GetLabel(VSHELL_IDC_LIST_WINDOWS), strVerify);
		// verify the category name was set
		if (strVerify != pszCategory) {
			LOG->RecordInfo("UIOptionsTabDlg::SetFont: unable to set the category to '%s'.", pszCategory);
			nResult = ERROR_ERROR;
		}
	}

	// set the font name, if specified
	if (pszFontName) {
		// select the font name
		MST.WComboItemClk(GetLabel(VSHELL_IDC_COMBO_FONT_NAME), pszFontName);
		// get the font name that was set
		MST.WComboText(GetLabel(VSHELL_IDC_COMBO_FONT_NAME), strVerify);
		// verify the font name was set
		if (strVerify != pszFontName) {
			LOG->RecordInfo("UIOptionsTabDlg::SetFont: unable to set the font name to '%s'.", pszFontName);
			nResult = ERROR_ERROR;
		}
	}

	// set the font size, if specified
	if (iFontSize) {
		// convert the font size to a string
		_itoa(iFontSize, acBuf, 10);
		// select the font size
		MST.WComboItemClk(GetLabel(VSHELL_IDC_COMBO_FONT_SIZE), acBuf);
		// get the font size that was set
		MST.WComboText(GetLabel(VSHELL_IDC_COMBO_FONT_SIZE), strVerify);
		// verify the font size was set
		if (strVerify != acBuf) {
			LOG->RecordInfo("UIOptionsTabDlg::SetFont: unable to set the font size to '%s'.", acBuf);
			nResult = ERROR_ERROR;
		}
	}

	// set the font style, if specified
	// there is not font style currently supported

	// close the dialog
	MST.WButtonClick(GetLabel(IDOK));

	return nResult;
}

int UIOptionsTabDlg::SetAutoIndentMode(int Type /* NONE =0, DEFAULT =1, SMART =3 */)
{
	switch(Type)
	{
	case NONE:
		{
			MST.WOptionClick(GetLabel(VCPP32_IDC_NO_AUTOINDENT));
			break ;
		}
	case DEFAULT:
		{
		 	MST.WOptionClick(GetLabel(VCPP32_IDC_STD_AUTOINDENT));
			break ;
		}
	case SMART:
		{
			MST.WOptionClick(GetLabel(VCPP32_IDC_SMART_AUTOINDENT));
			break ;
		}
	default:
		return ERROR_ERROR ;
	}

	return ERROR_SUCCESS ;
}

int UIOptionsTabDlg::SetSmartIndentOption(int Option, int PrevLine /*= 0 */ )
{
	switch(Option)
	{
	case OPEN:
		{
			MST.WCheckCheck(GetLabel(VCPP32_IDC_INDENT_OPENBRACE));
			break ;
		}
	case CLOSING:
		{
		 	
			MST.WCheckCheck(GetLabel(VCPP32_IDC_INDENT_CLOSEBRACE));
			break ;
		}
		default:
			return ERROR_ERROR ;
	}
	if (PrevLine)
	{
		char acBuf[5] ;
		// convert the font size to a string
		_itoa(PrevLine, acBuf, 10);
		// select the font size

		MST.WEditSetText(GetLabel(VCPP32_IDC_LINES_SCAN_BACK),acBuf );
	}
	return ERROR_SUCCESS;
}			


// BEGIN_HELP_COMMENT
// Function: BOOL UIOptionsTabDlg::ToggleHexDisplay(BOOL bEnable)
// Description: Toggles Hexadecimal display mode
// Param: bEnable If the parameter is TRUE, the Hexadecimal display mode is on. If FALSE - off.
// Return: A Boolean value that indicates whether the toggle operatin was sucessfull (TRUE) or not.
// END_HELP_COMMENT

BOOL UIOptionsTabDlg::ToggleHexDisplay(BOOL bEnable)

{
	// Open the Tools.Options dlg and select the Debug tab.
	if(!ShowDebugTab())
	{
		LOG->RecordInfo("UIOptionsTabDlg::ToggleHexDisplay: unable to display Debug Tab");
		return FALSE;
	}

	// Check the status of the "Hexadecimal display" check box
	int stat = MST.WCheckState("@1");

	if(( stat  && bEnable) || (!stat  && !bEnable))
	{
		OK();
		return TRUE;
	}

	// Check/Uncheck
	if(bEnable)
		MST.WCheckCheck("@1");
	else
		MST. WCheckUnCheck("@1");

	// Check that the "Hexadecimal display" check box was checked\unchecked properly
	stat = MST.WCheckState("@1");

	if(( stat  && bEnable) || (!stat  && !bEnable))
	{	
		OK();
		return TRUE;
	}
	else 
	{	
		Close();
		LOG->RecordInfo("UIOptionsTabDlg::ToggleHexDisplay: Hexadecimal display mode was not set correctly");
		return FALSE;
	}
}


// BEGIN_HELP_COMMENT
// Function: BOOL UIOptionsTabDlg::ToggleSourceAnnotation(BOOL bEnable)
// Description: Toggles Source Annotation
// Param: bEnable If the parameter is TRUE, the Hexadecimal display mode is on. If FALSE - off.
// Return: A Boolean value that indicates whether the toggle operatin was sucessfull (TRUE) or not.
// END_HELP_COMMENT

BOOL UIOptionsTabDlg::ToggleSourceAnnotation(BOOL bEnable)

{
	// Open the Tools.Options dlg and select the Debug tab.
	if(!ShowDebugTab())
	{
		LOG->RecordInfo("UIOptionsTabDlg::ToggleHexDisplay: unable to display Debug Tab");
		return FALSE;
	}

	// Check the status of the "Hexadecimal display" check box
	int stat = MST.WCheckState("@2");

	if(( stat  && bEnable) || (!stat  && !bEnable))
	{
		OK();
		return TRUE;
	}

	// Check/Uncheck
	if(bEnable)
		MST.WCheckCheck("@2");
	else
		MST. WCheckUnCheck("@2");

	// Check that the "Hexadecimal display" check box was checked\unchecked properly
	stat = MST.WCheckState("@2");

	if(( stat  && bEnable) || (!stat  && !bEnable))
	{	
		OK();
		return TRUE;
	}
	else 
	{	
		Close();
		LOG->RecordInfo("UIOptionsTabDlg::ToggleHexDisplay: Hexadecimal display mode was not set correctly");
		return FALSE;
	}
}

