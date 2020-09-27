///////////////////////////////////////////////////////////////////////////////
//  UCWZDLG.CPP
//
//  Created by :            Date :
//      Ivanl              3/07/94
//				  
//  Description :
//      Implementation of the UIClassWizard class
//

#include "stdafx.h"
#include "odbcdlgs.h"
#include "mstwrap.h"
#include "..\..\testutil.h"
#include "..\sym\appwz.h"

#define new DEBUG_NEW

#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;


// BEGIN_HELP_COMMENT
// Function: HWND UISqlDataSources::Display(void)
// Description: Display the SQL Data Sources dialog. This dialog is used during AppWizard. This function is NYI.
// Return: The HWND of the SQL Data Sources dialog, if successful; NULL otherwise.
// END_HELP_COMMENT
 HWND UISqlDataSources::Display(void)
 {
 	return NULL ;
 }

// BEGIN_HELP_COMMENT
// Function: int UISqlDataSources::SetODBCName(CString ds) 
// Description: Set the name of the SQL data source in the SQL Data Sources dialog.
// Return: An integer that indicates success. ERROR_SUCCESS if successful; ERROR_ERROR otherwise.
// Param: ds A CString that contains the name of the SQL data source.
// END_HELP_COMMENT
 int  UISqlDataSources::SetODBCName(CString ds) 
 {
 	DataSource(ODBC_SOURCE);
	MST.WComboItemDblClk(GetLabel(CLSWIZ_IDC_ODBC_SOURCES), ds) ;
	return ERROR_SUCCESS ;
 }

 // BEGIN_HELP_COMMENT
// Function: int UISqlDataSources::SetODBCName(int ds) 
// Description: Set the name of the SQL data source in the SQL Data Sources dialog.
// Return: An integer that indicates success. ERROR_SUCCESS if successful; ERROR_ERROR otherwise.
// Param: ds A CString that contains the name of the SQL data source.
// END_HELP_COMMENT
 int  UISqlDataSources::SetODBCName(int ds) 
 {
 	DataSource(ODBC_SOURCE);
	MST.DoKeys("{TAB}") ; // Tab to the edit control.
	MST.WComboItemDblClk(GetLabel(CLSWIZ_IDC_ODBC_SOURCES), ds) ;
	return ERROR_SUCCESS ;
 }

 // BEGIN_HELP_COMMENT
// Function: int UISqlDataSources::SetDAOName(CString ds) 
// Description: Set the name of the SQL data source in the SQL Data Sources dialog.
// Return: An integer that indicates success. ERROR_SUCCESS if successful; ERROR_ERROR otherwise.
// Param: ds A CString that contains the name of the SQL data source.
// END_HELP_COMMENT
 int  UISqlDataSources::SetDAOName(CString ds) 
 {
 	DataSource(DAO_SOURCE);
	MST.DoKeys("{TAB}") ; // Tab to the edit control.
	MST.WEditSetText(GetLabel(CLSWIZ_IDC_DAO_SOURCE), ds) ;
	return ERROR_SUCCESS ;
 }



 // BEGIN_HELP_COMMENT
// Function: int UISqlDataSources::DataSource(Int Type)
// Description: Selects the type of data source ODBC or DAO
// Return: An integer that indicates success. ERROR_SUCCESS if successful; ERROR_ERROR otherwise.
// Param: ds A CString that contains the name of the SQL data source.
// END_HELP_COMMENT
 int  UISqlDataSources::DataSource(DBSOURCES source) 
 {
	MST.WOptionClick(GetLabel(source));
	return ERROR_SUCCESS ;
 }

 // BEGIN_HELP_COMMENT
// Function: int UISqlDataSources::RecordSetType(REC_SET_TYPE Type)
// Description: Selects record set type Snap shot, dynaset, tables.
// Return: An integer that indicates success. ERROR_SUCCESS if successful; ERROR_ERROR otherwise.
// Param: ds A CString that contains the name of the SQL data source.
// END_HELP_COMMENT
 int  UISqlDataSources::RecordSetType(REC_SET_TYPE Type) 
 {
 	MST.WOptionClick(GetLabel(Type));
	return ERROR_SUCCESS ;
 }

 // BEGIN_HELP_COMMENT
// Function: int UISqlDataSources::DetectDirtyCol(int Yes = 1) 
// Description: Check the Detect Dirty col option
// Return: An integer that indicates success. ERROR_SUCCESS if successful; ERROR_ERROR otherwise.
// Param: ds A CString that contains the name of the SQL data source.
// END_HELP_COMMENT
 int  UISqlDataSources::DetectDirtyCol(int Yes /* = 1 */) 
 {
	if(Yes)
		MST.WCheckCheck(GetLabel(CLSWIZ_IDC_AUTODETECT));
	else
		MST.WCheckUnCheck(GetLabel(CLSWIZ_IDC_AUTODETECT));

	return ERROR_SUCCESS ;
 }

 // BEGIN_HELP_COMMENT
// Function: int UISqlDataSources::BindAllCol(int Yes = 1) 
// Description: Specify if you want to set the Bind All collumn option.
// Return: An integer that indicates success. ERROR_SUCCESS if successful; ERROR_ERROR otherwise.
// Param: ds A CString that contains the name of the SQL data source.
// END_HELP_COMMENT
 int  UISqlDataSources::BindAllCol(int Yes /* = 1*/) 
 {
	if(Yes)
		MST.WCheckCheck(GetLabel(CLSWIZ_IDC_BIND_ALL));
	else
		MST.WCheckUnCheck(GetLabel(CLSWIZ_IDC_BIND_ALL));
	return ERROR_SUCCESS ;
 }

// BEGIN_HELP_COMMENT
// Function: int UISqlDataSources::Close(int OK /* = 1 */)
// Description: Close the SQL Data Sources dialog by clicking on either OK or Cancel.
// Return: An integer that indicates success. ERROR_SUCCESS if successful; ERROR_ERROR otherwise.
// Param: OK An integer that specifies which button to click on to dismiss the dialog. 1 to click on OK, 0 to click on Cancel. (Default value is 1.)
// END_HELP_COMMENT
 int UISqlDataSources::Close(int OK /* = 1 */)
 {
 	if (OK)
	{
	  	MST.DoKeys("{ENTER}") ;
	}
	else 
	{
		MST.DoKeys("{ESC}") ;
	}
   return ERROR_SUCCESS ;
 }  

   // UISqlServerLogIn class functions 
// BEGIN_HELP_COMMENT
// Function: int UISqlServerLogIn::ID(CString ID)
// Description: Set the ID in the SQL Server Log In dialog.
// Return: An integer that indicates success. ERROR_SUCCESS if successful; ERROR_ERROR otherwise.
// Param: ID A CString that contains the ID.
// END_HELP_COMMENT
int UISqlServerLogIn::ID(CString ID)
{
	MST.WEditSetText(GetLabel(0x7533), ID) ;
	return ERROR_SUCCESS ;
}
// BEGIN_HELP_COMMENT
// Function: int UISqlServerLogIn::Pw(CString pw)
// Description: Set the password in the SQL Server Log In dialog.
// Return: An integer that indicates success. ERROR_SUCCESS if successful; ERROR_ERROR otherwise.
// Param: pw A CString that contains the password.
// END_HELP_COMMENT
int UISqlServerLogIn::Pw(CString pw)
{
	MST.WEditSetText(GetLabel(0x7534),pw) ;
	return ERROR_SUCCESS ;
}

// BEGIN_HELP_COMMENT
// Function: int UISqlServerLogIn::Close(int OK/* = 1 */)
// Description: Close the SQL Server Log In dialog by clicking either the OK button or the Cancel button.
// Return: An integer that indicates success. ERROR_SUCCESS if successful; ERROR_ERROR otherwise.
// Param: OK An integer that specifies which button to click on to dismiss the dialog. 1 to click on OK, 0 to click on Cancel. (Default value is 1.)
// END_HELP_COMMENT
int UISqlServerLogIn::Close(int OK/* = 1 */)
{
	if (OK)
	{
	 	MST.DoKeys("{ENTER}") ;
	}
	else 
	{
		MST.DoKeys("{ESC}") ;
	}
   return ERROR_SUCCESS ;
} 

	// UISelectTables functions ;
// BEGIN_HELP_COMMENT
// Function: int UISelectTables::Name(CString tbName)
// Description: Set the table name in the Select Tables dialog by name. (See Name(int) to set the name by index.)
// Return: An integer that indicates success. ERROR_SUCCESS if successful; ERROR_ERROR otherwise.
// Param: tbName A CString that contains the name of the table.
// END_HELP_COMMENT
int UISelectTables::Name(CString tbName)
{
	MST.DoKeys("{TAB 2}") ; // Tabs to the listbox.
	MST.WListItemDblClk(NULL,tbName) ;
	return ERROR_SUCCESS ;
}

// BEGIN_HELP_COMMENT
// Function: int UISelectTables::Name(int index /*= 1*/)
// Description: Set the table name in the Select Tables dialog by index. (See Name(CString) to set the name by name.)
// Return: An integer that indicates success. ERROR_SUCCESS if successful; ERROR_ERROR otherwise.
// Param: index An integer that contains the index into the list of tables.
// END_HELP_COMMENT
int UISelectTables::Name(int index /*= 1*/)
{
	MST.DoKeys("{TAB 2}") ; // Tabs to the listbox 
	MST.WListItemDblClk(NULL,index);
	return ERROR_SUCCESS ;
}

// BEGIN_HELP_COMMENT
// Function: int UISelectTables::Close(int OK /* = 1 */)
// Description: Close the Select Tables dialog by clicking either the OK button or the Cancel button.
// Return: An integer that indicates success. ERROR_SUCCESS if successful; ERROR_ERROR otherwise.
// Param: OK An integer that specifies which button to click on to dismiss the dialog. 1 to click on OK, 0 to click on Cancel. (Default value is 1.)
// END_HELP_COMMENT
int UISelectTables::Close(int OK /* = 1 */)
{
	if (OK)
	{
	 	MST.DoKeys("{ENTER}") ;
		WaitForWndWithCtrl(APPWZ_IDC_DATA_SOURCE,3000) ;
		MST.DoKeys("{TAB}") ; // Tab to the main dialog.

	}
	else 
	{
		MST.DoKeys("{ESC}") ;
	}
   return ERROR_SUCCESS ;
}             
 
// BEGIN_HELP_COMMENT
// Function: int UISelectRecordSet::Close(int OK /* = 1*/)
// Description: Close the Select Record Set dialog by clicking either the OK button or the Cancel button.
// Return: An integer that indicates success. ERROR_SUCCESS if successful; ERROR_ERROR otherwise.
// Param: OK An integer that specifies which button to click on to dismiss the dialog. 1 to click on OK, 0 to click on Cancel. (Default value is 1.)
// END_HELP_COMMENT
int UISelectRecordSet::Close(int OK /* = 1*/)
{
	if (OK)
	{
	 	MST.DoKeys("{ENTER}") ;
	}
	else 
	{
		MST.DoKeys("{ESC}") ;
	}
   return ERROR_SUCCESS ;
} 

// BEGIN_HELP_COMMENT
// Function: UIDataBaseDlgs *UISelectRecordSet::New() 
// Description: Bring up the Database dialog.
// Return: A pointer to a UIDataBaseDlgs object that represents the dialog opened.
// END_HELP_COMMENT
UIDataBaseDlgs *UISelectRecordSet::New() 
{
	static UIDataBaseDlgs dbdlgs ;
	dbdlgs.WaitForSources() ;
	return &dbdlgs ;
}

// BEGIN_HELP_COMMENT
// Function: int UIDataBaseDlgs::WaitForSources() 
// Description: Wait for the SQL Data Sources dialog to exist.
// Return: An integer that indicates success. ERROR_SUCCESS if successful; ERROR_ERROR otherwise.
// END_HELP_COMMENT
int UIDataBaseDlgs::WaitForSources() 
	 {
	 	if (WaitForWndWithCtrl(0x1F5,1000))
			return ERROR_SUCCESS ;
		return ERROR_ERROR ;
	 }
// BEGIN_HELP_COMMENT
// Function: int UIDataBaseDlgs::WaitForLogin() 
// Description: Wait for the Server Log In dialog to exist
// Return: An integer that indicates success. ERROR_SUCCESS if successful; ERROR_ERROR otherwise.
// END_HELP_COMMENT
int UIDataBaseDlgs::WaitForLogin() 
 	 {
	 if (WaitForWndWithCtrl(0x7539,1000))
			return ERROR_SUCCESS ;
		return ERROR_ERROR ;
	 }

// BEGIN_HELP_COMMENT
// Function: int UIDataBaseDlgs::WaitForTables()
// Description: Wait for the Select Tables dialog to exist.
// Return: An integer that indicates success. ERROR_SUCCESS if successful; ERROR_ERROR otherwise.
// END_HELP_COMMENT
int UIDataBaseDlgs::WaitForTables()
	 {
	 	if (WaitForWndWithCtrl(0x3F0,1000))
			return ERROR_SUCCESS ;
		return ERROR_ERROR ;
	 }

		 
