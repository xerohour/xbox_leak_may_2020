///////////////////////////////////////////////////////////////////////////////
//  UICONNEC.CPP
//
//  Created by :            Date :
//      MichMa                  8/24/94
//
//  Description :
//      Implementation of the UIConnection class
//

#include "stdafx.h"
#include "uiconnec.h"
#include "..\..\testutil.h"
#include "guiv1.h"
#include "mstwrap.h"
#include "..\sym\vcpp32.h"

#define new DEBUG_NEW

#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;


// BEGIN_HELP_COMMENT
// Function: HWND UIConnection::Settings(CString title) 
// Description: Open the Settings dialog from the Connection dialog by clicking the Settings button.
// Return: An HWND that specifies the Settings dialog.
// Param: title A CString that contains the expected title of the Settings dialog (this title changes based on the platform and connection type specified in the Connection dialog).
// END_HELP_COMMENT
HWND UIConnection::Settings(CString title) 
	
	{
	if(!MST.WButtonExists(GetLabel(VCPP32_IDC_SETUP))) 

		{
		LOG->RecordInfo("ERROR in UIConnection::Settings(): "
			  "%s button not enabled", GetLabel(VCPP32_IDC_SETUP));
			   
		return NULL;			
		}

	MST.WButtonClick(GetLabel(VCPP32_IDC_SETUP));
	return MST.WFndWndWait(title, FW_DEFAULT, 10);
	}


// BEGIN_HELP_COMMENT
// Function: int UIConnection::SetPlatform(LPCSTR platform)
// Description: Set the platform in the Connection dialog.
// Return: An integer that indicates success. ERROR_SUCCESS if successful; ERROR_ERROR otherwise.
// Param: platform A pointer to a string that contains the platform to select in the Connection dialog.
// END_HELP_COMMENT
int UIConnection::SetPlatform(LPCSTR platform)
	
	{
	if(MST.WComboItemExists(GetLabel(VCPP32_IDC_PLATFORM_LIST), platform) < 1)
		
		{
		LOG->RecordInfo("ERROR in UIConnection::SetPlatform(): "
			  "%s item of %s combo box doesn't exist", 
			  platform, GetLabel(VCPP32_IDC_PLATFORM_LIST));
			   
		return ERROR_ERROR;
		}							

	MST.WComboItemClk(GetLabel(VCPP32_IDC_PLATFORM_LIST), platform);
	return ERROR_SUCCESS;
	}


// BEGIN_HELP_COMMENT
// Function: int UIConnection::SetConnection(LPCSTR connection)
// Description: Set the connection type in the Connection dialog.
// Return: An integer that indicates success. ERROR_SUCCESS if successful; ERROR_ERROR otherwise.
// Param: connection A pointer to a string that contains the connection to select in the Connection dialog.
// END_HELP_COMMENT
int UIConnection::SetConnection(LPCSTR connection)
	
	{
	if(MST.WListItemExists(GetLabel(VCPP32_IDC_TRANSPORT_LIST), connection) < 1)

		{
		LOG->RecordInfo("ERROR in UIConnection::SetConnection(): "
			  "%s item of %s combo box doesn't exist", 
			  connection, GetLabel(VCPP32_IDC_TRANSPORT_LIST));
			   
		return ERROR_ERROR;
		}							

	MST.WListItemClk(GetLabel(VCPP32_IDC_TRANSPORT_LIST), connection);
	return ERROR_SUCCESS;
	}


// BEGIN_HELP_COMMENT
// Function: CString UIConnection::GetPlatform(void)
// Description: Get the selected platform in the Connection dialog.
// Return: A CString that contains the selected platform in the Connection dialog.
// END_HELP_COMMENT
CString UIConnection::GetPlatform(void)
	{
	CString platform;
	MST.WComboText(GetLabel(VCPP32_IDC_PLATFORM_LIST), platform);
	return platform;
	}


// BEGIN_HELP_COMMENT
// Function: CString UIConnection::GetConnection(void)
// Description: Get the selected connection in the Connection dialog.
// Return: A CString that contains the selected connection in the Connection dialog.
// END_HELP_COMMENT
CString UIConnection::GetConnection(void)
	{
	CString connection;
	MST.WListText(GetLabel(VCPP32_IDC_TRANSPORT_LIST), connection);
	return connection;
	}

