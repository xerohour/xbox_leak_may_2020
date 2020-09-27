///////////////////////////////////////////////////////////////////////////////
//  UIEEWND.CPP
//
//  Created by :            Date :
//      MichMa              	10/20/94
//
//  Description :
//      Implementation of the UIEEWindow class
//

#include "stdafx.h"
#include "uieewnd.h"

#define new DEBUG_NEW

#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;


// BEGIN_HELP_COMMENT
// Function: int UIEEWindow::GetState(int row /* ROW_CURRENT */)
// Description: Get the state of the expression at the specified row in the expression evaluator window.
// Return: A bit field that specifies the state of the expression: NOT_EXPANDABLE, EXPANDED, COLLAPSED.
// Param: row An integer that specifies the 1-based row of the EE window. Use ROW_CURRENT to specify the current row; ROW_BOTTOM to specify the last row; and ROW_ALL to specify all rows. (Default value is ROW_CURRENT.)
// END_HELP_COMMENT
int UIEEWindow::GetState(int row /* ROW_CURRENT */)
	{
	EXPR_INFO expr_info;
	GetAllFields(&expr_info, row, 1, METHOD_WND_SEL);
	return expr_info.state;
	}


// BEGIN_HELP_COMMENT
// Function: CString UIEEWindow::GetType(int row /* ROW_CURRENT */)
// Description: Get the type of the expression at the specified row in the expression evaluator window.
// Return: A CString that contains a description of the type of the expression.
// Param: row An integer that specifies the 1-based row of the EE window. Use ROW_CURRENT to specify the current row; ROW_BOTTOM to specify the last row; and ROW_ALL to specify all rows. (Default value is ROW_CURRENT.)
// END_HELP_COMMENT
CString UIEEWindow::GetType(int row /* ROW_CURRENT */)
	{
	EXPR_INFO expr_info;
	GetAllFields(&expr_info, row, 1, METHOD_PROP_PAGE);
	return expr_info.type;
	}

/*
// BEGIN_HELP_COMMENT
// Function: CString UIEEWindow::GetName(int row / ROW_CURRENT /)
// Description: Get the name of the expression at the specified row in the expression evaluator window.
// Return: A CString that contains the name of the expression.
// Param: row An integer that specifies the 1-based row of the EE window. Use ROW_CURRENT to specify the current row; ROW_BOTTOM to specify the last row; and ROW_ALL to specify all rows. (Default value is ROW_CURRENT.)
// END_HELP_COMMENT
CString UIEEWindow::GetName(int row / ROW_CURRENT /)
	{
	EXPR_INFO expr_info;
	GetAllFields(&expr_info, row, 1, METHOD_WND_SEL);
	return expr_info.name;
	}
*/
// BEGIN_HELP_COMMENT
// Function: CString UIEEWindow::GetName(int row /* ROW_CURRENT */, EE_METHOD_TYPE method /* METHOD_WND_SEL */)
// Description: Get the name of the expression at the specified row in the expression evaluator window.
// Return: A CString that contains the name of the expression.
// Param: row An integer that specifies the 1-based row of the EE window. Use ROW_CURRENT to specify the current row; ROW_BOTTOM to specify the last row; and ROW_ALL to specify all rows. (Default value is ROW_CURRENT.)
// Param: method An EE_METHOD_TYPE value that specifies the method to use when retrieving the data (METHOD_WND_SEL or METHOD_PROP_PAGE).
// END_HELP_COMMENT
CString UIEEWindow::GetName(int row /* ROW_CURRENT */, EE_METHOD_TYPE method /* METHOD_WND_SEL */)
	{
	EXPR_INFO expr_info;
	GetAllFields(&expr_info, row, 1, method);
	return expr_info.name;
	}


// BEGIN_HELP_COMMENT
// Function: CString UIEEWindow::GetValue(int row /* ROW_CURRENT */)
// Description: Get the value of the expression at the specified row in the expression evaluator window.
// Return: A CString that contains the value of the expression.
// Param: row An integer that specifies the 1-based row of the EE window. Use ROW_CURRENT to specify the current row; ROW_BOTTOM to specify the last row; and ROW_ALL to specify all rows. (Default value is ROW_CURRENT.)
// END_HELP_COMMENT
CString UIEEWindow::GetValue(int row /* ROW_CURRENT */)
	{
	EXPR_INFO expr_info;
	GetAllFields(&expr_info, row, 1, METHOD_WND_SEL);
	return expr_info.value;
	}


// BEGIN_HELP_COMMENT
// Function: int UIEEWindow::SelectRows(int start_row /* ROW_CURRENT */, int total_rows /*= 1*/)
// Description: Select the specified rows in the EE window.
// Return: An integer that indicates success. ERROR_SUCCESS if successful; ERROR_ERROR otherwise.
// Param: start_row An integer that specifies the 1-based row of the EE window. Use ROW_CURRENT to specify the current row; ROW_BOTTOM to specify the last row; and ROW_ALL to specify all rows. (Default value is ROW_CURRENT.)
// Param: total_rows An integer that specifies the number of rows to select. (Default value is 1.)
// END_HELP_COMMENT
int UIEEWindow::SelectRows(int start_row /* ROW_CURRENT */, 
						   int total_rows /* 1 */)
	
	{
	switch(start_row)

		{
		case ROW_CURRENT:
			break;
		case ROW_LAST:
			MST.DoKeys("{END}");
			break;
		case ROW_NEXT:
			MST.DoKeys("{DOWN}");
			break;
		case ROW_PREVIOUS:
			MST.DoKeys("{UP}");
			break;
		default:
			
			if(start_row < 1) 
				{					
				LOG->RecordInfo("ERROR in UIEEWindow::SelectRows(): start row < 1");
				return ERROR_ERROR;
				}

			MST.DoKeys("{HOME}");

			for(int i = 1; i < start_row; i++)
				MST.DoKeys("{DOWN}");
		}

	if(total_rows == ROW_ALL)
		 MST.DoKeys("+({END})");
	else
		
		{
		if(total_rows < 1) 
			{					
			LOG->RecordInfo("ERROR in UIEEWindow::SelectRows(): total rows < 1");
			return ERROR_ERROR;
			}
		
		for(int i = 1; i < total_rows; i++)
			MST.DoKeys("+({DOWN})");
		}	

	return ERROR_SUCCESS;
	}
			

// BEGIN_HELP_COMMENT
// Function: int UIEEWindow::Expand(int row /* ROW_CURRENT */)
// Description: Expand the specified row in the EE window.
// Return: An integer that indicates success. ERROR_SUCCESS if successful; ERROR_ERROR otherwise.
// Param: row An integer that specifies the 1-based row of the EE window. Use ROW_CURRENT to specify the current row; ROW_BOTTOM to specify the last row; and ROW_ALL to specify all rows. (Default value is ROW_CURRENT.)
// END_HELP_COMMENT
int UIEEWindow::Expand(int row /* ROW_CURRENT */)
	{
	SelectRows(row);
	MST.DoKeys("{+}");
	return ERROR_SUCCESS;
	}


// BEGIN_HELP_COMMENT
// Function: int UIEEWindow::Collapse(int row /* ROW_CURRENT */)
// Description: Collapse the specified row in the EE window.
// Return: An integer that indicates success. ERROR_SUCCESS if successful; ERROR_ERROR otherwise.
// Param: row An integer that specifies the 1-based row of the EE window. Use ROW_CURRENT to specify the current row; ROW_BOTTOM to specify the last row; and ROW_ALL to specify all rows. (Default value is ROW_CURRENT.)
// END_HELP_COMMENT
int UIEEWindow::Collapse(int row /* ROW_CURRENT */)
	{
	SelectRows(row);
	MST.DoKeys("{-}");
	return ERROR_SUCCESS;
	}


// BEGIN_HELP_COMMENT
// Function: int UIEEWindow::GetAllFields(EXPR_INFO* expr_info, int start_row /*= ROW_CURRENT*/, int total_rows /*= 1*/)
// Description: Get information about the expression(s) in the given range of expressions in the expression evaluator window.
// Return: An integer that indicates success. ERROR_SUCCESS if successful; ERROR_ERROR otherwise.
// Param: expr_info A pointer to an EXPR_INFO object that will contain the information about the expression(s).
// Param: start_row An integer that specifies the 1-based row of the EE window. Use ROW_CURRENT to specify the current row; ROW_BOTTOM to specify the last row; and ROW_ALL to specify all rows. (Default value is ROW_CURRENT.)
// Param: total_rows An integer that specifies the number of rows to retrieve. (Default value is 1.)
// END_HELP_COMMENT

int UIEEWindow::GetAllFields(EXPR_INFO* expr_info, 
							 int start_row /* ROW_CURRENT */, int total_rows /* 1 */,
							 EE_METHOD_TYPE method /* METHOD_PROP_PAGE */)
	
	{
	int i, cr, tab1, tab2;
	CString expr_str, static_text;		

	// need to select and extract rows directly FROM ee window so we can examine the 
	// expansion state of each row (not available in the properties page).
	if(SelectRows(start_row, total_rows) != ERROR_SUCCESS)
		{					
		LOG->RecordInfo("ERROR in UIEEWindow::GetAllFields(): failed to select rows");
		return ERROR_ERROR;
		}	
	
	// set clipboard to a known state.  this helps us avoid problems when the pane is empty.
	SetClipText("");
	MST.DoKeys("^c");
	GetClipText(expr_str);

	// check if the pane is empty.
	if(expr_str == "")
		{
		expr_info[0].state = NOT_EXPANDABLE;
		expr_info[0].type = "";
		expr_info[0].name = "";
		expr_info[0].value = "";
		return ERROR_SUCCESS;
		}

	// if the user wants to examine all rows, the number of carriage-returns in the extracted
	// string will tell us how many rows there are.
	if(total_rows == ROW_ALL)
		
		{
		CString temp_str = expr_str;	
		cr =  temp_str.Find("\x0D");
		i  = 0;

		while(cr != -1)
			{
			i++;
			temp_str = temp_str.Mid(cr + 2);
			cr = temp_str.Find("\x0D");
			}
		
		total_rows = i;
		}

	// start with the first row. only necessary to do this if we are using the properties
	// page to get the infor for each row.  if the method is METHOD_WND_SEL, we already have
	// the entire contents of the window in expr_info.
	if(method == METHOD_PROP_PAGE)
	{
		// we already selected the start row once to get the state information.
		// if the start row is supposed to be the previous or next row, then selecting it again
		// will actually put us on the wrong row. so in that case we actually want to stay where we are.
		if((start_row != ROW_PREVIOUS) && (start_row != ROW_NEXT))
		{
			if(SelectRows(start_row) != ERROR_SUCCESS)
			{					
				LOG->RecordInfo("ERROR in UIEEWindow::GetAllFields(): failed to select start row");
				return ERROR_ERROR;
			}
		}
	}

	for(i = 0; i < total_rows; i++)

		{
		// set expansion state.
		switch(expr_str[0])
			{
			case '+':
				expr_info[i].state = COLLAPSED;
				break;
			case '-':
				expr_info[i].state = EXPANDED;
				break;
			case '\t': 
				expr_info[i].state = NOT_EXPANDABLE;
				break;
			
			default:
				LOG->RecordInfo("ERROR in UIEEWindow::GetAllFields(): "
								"could not extract state from ee string");

				return ERROR_ERROR;
			}

		// find the end of the current row.
		cr = expr_str.Find("\x0D");

		// since the qw dialog doesn't support the properties page, we need to be able
		// to use the old window selection method in that case.
		switch(method)
			
			{
			// quickwatch functions should not use this method (the default).
			case METHOD_PROP_PAGE:
				
				// bring up properties page (wait for General Tab to appear).
				MST.DoKeys("%({ENTER})");
#ifdef _M_ALPHA
                Sleep(500);  // Slow down a bit so that we can detect the tab.
#endif

				if(!MST.WFndWndWaitC(GetLocString(IDSS_VAR_PROP_TYPE), "Static", FW_PART, 5)) 

					{					
					LOG->RecordInfo("ERROR in UIEEWindow::GetAllFields(): "
									"properties page did not appear within 5 seconds");

					return ERROR_ERROR;
					}
											
				// set type.
				MST.WStaticText("@2", static_text);
				expr_info[i].type = static_text;
				// prop page displays it with a space appended that we need to truncate.
				expr_info[i].type = expr_info[i].type.Left(expr_info[i].type.GetLength() - 1);
				
				// set name and value.
				MST.WStaticText("@4", static_text);
				expr_info[i].name = static_text;
				MST.WStaticText("@6", static_text);
				expr_info[i].value = static_text;

				// close properties page (wait for General Tab to disappear).
				MST.DoKeys("{ESC}");
				while(MST.WFndWndWaitC(GetLocString(IDSS_VAR_PROP_TYPE), "Static", FW_PART,1)); 

				// advance selection to next row if there is a next row.
				if((total_rows - i) > 1)
					MST.DoKeys("{DOWN}");

				break;
		
			case METHOD_WND_SEL:

				// tabs separate state, name, and value fields.
				// carriage-return/linefeed sequence terminates each row.
				tab1 = expr_str.Find("\t");
				tab2 = expr_str.Mid(tab1 + 1).Find("\t") + tab1 + 1;

				// set type (only available in properties page), name, and value.
				expr_info[i].type = "";
				expr_info[i].name  = expr_str.Mid(tab1 + 1, tab2 - tab1 - 1);
				expr_info[i].value = expr_str.Mid(tab2 + 1, cr - tab2 - 1);
				break;

			default:
				// programming error.
				LOG->RecordInfo("ERROR in UIEEWindow::GetAllFields(): invalid method");
				return ERROR_ERROR;
		}

		// advance expression string to next row.  even if method is METHOD_PROP_PAGE,
		// this string is still used to get the expansion state.
		expr_str = expr_str.Mid(cr + 2);
		} 

	return ERROR_SUCCESS;
	}
