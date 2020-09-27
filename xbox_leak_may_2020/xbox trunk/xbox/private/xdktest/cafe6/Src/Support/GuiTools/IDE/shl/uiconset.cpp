///////////////////////////////////////////////////////////////////////////////
//  UICONSET.CPP
//
//  Created by :            Date :
//      MichMa                  8/25/94
//
//  Description :
//      Implementation of the UIConnectionSettings class
//

#include "stdafx.h"
#include "uiconset.h"
#include "mstwrap.h"
#include "..\..\testutil.h"
#include "guiv1.h"

#define new DEBUG_NEW

#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;

// BEGIN_HELP_COMMENT
// Function: int UIConnectionSettings::SetZone(LPCSTR zone)	
// Description: Set the Appletalk zone in the Connection Settings dialog.
// Return: An integer that indicates success. ERROR_SUCCESS if successful; ERROR_ERROR otherwise.
// Param: zone A pointer to a string that contains the name of the Appletalk zone.
// END_HELP_COMMENT
int UIConnectionSettings::SetZone(LPCSTR zone)	
	
	{
	if(!MST.WEditEnabled("@2"))		//todo: use GetLabel() 
		
		{
		LOG->RecordInfo("ERROR in UIConnectionSettings::SetZone(): "
			  "@2 edit box not enabled");
			   
		return ERROR_ERROR;
		}
									
	MST.WEditSetText("@2", zone);					
	return ERROR_SUCCESS;
	}


// BEGIN_HELP_COMMENT
// Function: int UIConnectionSettings::SetMachineName(LPCSTR machine_name)
// Description: Set the machine name in the Connection Settings dialog.
// Return: An integer that indicates success. ERROR_SUCCESS if successful; ERROR_ERROR otherwise.
// Param: machine_name A pointer to a string that contains the machine name.
// END_HELP_COMMENT
int UIConnectionSettings::SetMachineName(LPCSTR machine_name)
		
	{
	if(!MST.WEditEnabled("@1"))		//todo: use GetLabel() 
		
		{
		LOG->RecordInfo("ERROR in UIConnectionSettings::SetMachineName(): "
			  "@1 edit box not enabled");
			   
		return ERROR_ERROR;
		}
									
	MST.WEditSetText("@1", machine_name);					
	return ERROR_SUCCESS;
	}


// BEGIN_HELP_COMMENT
// Function: int UIConnectionSettings::SetConnector(LPCSTR connector)
// Description: Set the connector type in the Connection Settings dialog.
// Return: An integer that indicates success. ERROR_SUCCESS if successful; ERROR_ERROR otherwise.
// Param: connector A pointer to a string that contains the connector name.
// END_HELP_COMMENT
int UIConnectionSettings::SetConnector(LPCSTR connector)
	
	{
	if(MST.WComboItemExists("@1", connector) < 1) 		//todo: use GetLabel()
		
		{
		LOG->RecordInfo("ERROR in UIConnectionSettings::SetConnector(): "
			  "%s item of @1 combo box doesn't exist", connector);
			   
		return ERROR_ERROR;
		}
									
	MST.WComboItemClk("@1", connector);									
	return ERROR_SUCCESS;
	}


// BEGIN_HELP_COMMENT
// Function: int UIConnectionSettings::SetBaudRate(LPCSTR baud_rate)
// Description: Set the baud rate in the Connection Settings dialog.
// Return: An integer that indicates success. ERROR_SUCCESS if successful; ERROR_ERROR otherwise.
// Param: baud_rate A pointer to a string that contains the baud rate.
// END_HELP_COMMENT
int UIConnectionSettings::SetBaudRate(LPCSTR baud_rate)
	
	{
	if(MST.WComboItemExists("@2", baud_rate) < 1) 	   //todo: use GetLabel()
		
		{
		LOG->RecordInfo("ERROR in UIConnectionSettings::SetBaudRate): "
			  "%s item of @2 combo box doesn't exist", baud_rate);
			   
		return ERROR_ERROR;
		}
									
	MST.WComboItemClk("@2", baud_rate);							
	return ERROR_SUCCESS;
	}


// BEGIN_HELP_COMMENT
// Function: int UIConnectionSettings::SetDataBits(LPCSTR data_bits)
// Description: Set the number of data bits in the Connection Settings dialog.
// Return: An integer that indicates success. ERROR_SUCCESS if successful; ERROR_ERROR otherwise.
// Param: data_bits A pointer to a string that contains the number of data bits.
// END_HELP_COMMENT
int UIConnectionSettings::SetDataBits(LPCSTR data_bits)

	{
	if(MST.WComboItemExists("@3", data_bits) < 1) 		//todo: use GetLabel()
		
		{
		LOG->RecordInfo("ERROR in UIConnectionSettings::SetDataBits(): "
			  "%s item of @3 combo box doesn't exist", data_bits);
			   
		return ERROR_ERROR;
		}
									
	MST.WComboItemClk("@3", data_bits);							
	return ERROR_SUCCESS;
	}


// BEGIN_HELP_COMMENT
// Function: int UIConnectionSettings::SetParity(LPCSTR parity)
// Description: Set the parity in the Connection Settings dialog.
// Return: An integer that indicates success. ERROR_SUCCESS if successful; ERROR_ERROR otherwise.
// Param: parity A pointer to a string that contains the parity.
// END_HELP_COMMENT
int UIConnectionSettings::SetParity(LPCSTR parity)

	{
	if(MST.WComboItemExists("@4", parity) < 1) 	 //todo: use GetLabel()
		
		{
		LOG->RecordInfo("ERROR in UIConnectionSettings::SetParity(): "
			  "%s item of @4 combo box doesn't exist", parity);
			   
		return ERROR_ERROR;
		}
									
	MST.WComboItemClk("@4", parity);							
	return ERROR_SUCCESS;
	}


// BEGIN_HELP_COMMENT
// Function: int UIConnectionSettings::SetStopBits(LPCSTR stop_bits)
// Description: Set the number of stop bits in the Connection Settings dialog.
// Return: An integer that indicates success. ERROR_SUCCESS if successful; ERROR_ERROR otherwise.
// Param: stop_bits A pointer to a string that contains the stop bits.
// END_HELP_COMMENT
int UIConnectionSettings::SetStopBits(LPCSTR stop_bits)
	
	{
	if(MST.WComboItemExists("@5", stop_bits) < 1) 	   //todo: use GetLabel()
		
		{
		LOG->RecordInfo("ERROR in UIConnectionSettings::SetStopBits(): "
			  "%s item of @5 combo box doesn't exist", stop_bits);
			   
		return ERROR_ERROR;
		}
									
	MST.WComboItemClk("@5", stop_bits);							
	return ERROR_SUCCESS;
	}


// BEGIN_HELP_COMMENT
// Function: int UIConnectionSettings::SetFlowControl(LPCSTR flow_control)
// Description: Set the type of flow control in the Connection Settings dialog.
// Return: An integer that indicates success. ERROR_SUCCESS if successful; ERROR_ERROR otherwise.
// Param: flow_control A pointer to a string that contains the type of flow control.
// END_HELP_COMMENT
int UIConnectionSettings::SetFlowControl(LPCSTR flow_control)

	{
	if(!MST.WOptionEnabled(flow_control)) 	  //todo: use GetLabel()
		
		{
		LOG->RecordInfo("ERROR in UIConnectionSettings::SetFlowControl(): "
			  "%s option button not enabled", flow_control);
			   
		return ERROR_ERROR;
		}
									
	MST.WOptionSelect(flow_control);
	return ERROR_SUCCESS;
	}

	
// BEGIN_HELP_COMMENT
// Function: int UIConnectionSettings::SetIPAddress(LPCSTR ip_address)	
// Description: Set the IP address in the Connection Settings dialog.
// Return: An integer that indicates success. ERROR_SUCCESS if successful; ERROR_ERROR otherwise.
// Param: ip_address A pointer to a string that contains the IP address.
// END_HELP_COMMENT
int UIConnectionSettings::SetIPAddress(LPCSTR ip_address)	
	
	{
	if(!MST.WEditEnabled("@1"))		//todo: use GetLabel() 
		
		{
		LOG->RecordInfo("ERROR in UIConnectionSettings::SetIPAddress(): "
			  "@1 edit box not enabled");
			   
		return ERROR_ERROR;
		}
									
	MST.WEditSetText("@1", ip_address);					
	return ERROR_SUCCESS;
	}


// BEGIN_HELP_COMMENT
// Function: int UIConnectionSettings::SetPassword(LPCSTR password, CONNECTION_TYPE connection)
// Description: Set the password in the Connection Settings dialog.
// Return: An integer that indicates success. ERROR_SUCCESS if successful; ERROR_ERROR otherwise.
// Param: password A pointer to a string that contains the password.
// Param: connection A CONNECTION_TYPE value that specifies the type of connection: CONNECTION_APPLETALK, CONNECTION_TCP_IP.
// END_HELP_COMMENT
int UIConnectionSettings::SetPassword(LPCSTR password, 
									  CONNECTION_TYPE connection)

	{
	CString edit_ordinal;

	switch(connection)

		{
		case CONNECTION_APPLETALK:
			edit_ordinal = "@3";
			break;
		case CONNECTION_TCP_IP:
			edit_ordinal = "@2";
			break;
		
		default:
			
			LOG->RecordInfo("ERROR in UIConnectionSettings::SetPassword(): "
				  "invalid connection type");
			   
			return ERROR_ERROR;
		}
	
	
	if(!MST.WEditEnabled(edit_ordinal)) 	//todo: use GetLabel()
		
		{
		LOG->RecordInfo("ERROR in UIConnectionSettings::SetPassword(): "
			  "%s edit box not enabled", edit_ordinal);
			   
		return ERROR_ERROR;
		}
									
	MST.WEditSetText(edit_ordinal, password);				
	return ERROR_SUCCESS;
	}


// review: can't use UIDialog::Close() because it calls ExpectValid() which
// checks the ExpectedTitle(), which this window doesn't really have because
// it's dependent on the tl.  could override ExpectedTitle() to choose correct
// title, but since the dlg only knows it's title once it's up, that function
// would have limited use.  this works for now. michma

// BEGIN_HELP_COMMENT
// Function: HWND UIConnectionSettings::Close(void)								
// Description: Close the Connection Settings dialog.
// Return: NULL if successful; the HWND of any remaining message or dialog box otherwise.
// END_HELP_COMMENT
HWND UIConnectionSettings::Close(void)								
	
	{																
	if(!MST.WButtonEnabled(GetLabel(IDOK)))
			
		{
		LOG->RecordInfo("ERROR in UIConnectionSettings::Close(): "
			  "%s button not enabled", GetLabel(IDOK));
			   
		return MST.WGetActWnd(0);			
		}
									
	MST.WButtonClick(GetLabel(IDOK));							    

	if(WaitUntilGone(3000))		
		return NULL;				
	else
	
		{
		LOG->RecordInfo("ERROR in UIConnectionSettings::Close(): "
			  "settings dlg not gone after 3 seconds");
			   
		return MST.WGetActWnd(0);			
		}
	}
