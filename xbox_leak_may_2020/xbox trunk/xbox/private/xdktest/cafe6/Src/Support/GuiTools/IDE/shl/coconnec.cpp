//////////////////////////////////////////////////////////////////////////////
//  COCONNEC.CPP
//
//  Created by :            Date :
//      MichMa                  9/16/94
//
//  Description :
//      Implementation of the COConnection class
//

#include "stdafx.h"
#include "coconnec.h"
#include "testxcpt.h"
#include "..\..\testutil.h"
#include "guiv1.h"
#include "mstwrap.h"
#include "uwbframe.h"

#define new DEBUG_NEW

#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;

SHL_DATA COConnection *gpConnec;

int platform_id[] = {IDSS_PLATFORM_WIN32, IDSS_PLATFORM_WIN32,                                     
					 IDSS_PLATFORM_MAC68K, IDSS_PLATFORM_MACPPC,
					 IDSS_PLATFORM_WIN32, IDSS_PLATFORM_WIN32,
					 IDSS_PLATFORM_WIN32, IDSS_PLATFORM_XBOX};                                     

int connection_id[7][4] =          
	{IDSS_CONNECTION_LOCAL, IDSS_CONNECTION_SERIAL, 0, IDSS_CONNECTION_TCPIP,             
	 0, IDSS_CONNECTION_SERIAL, 0, 0,                                                  
	 0, IDSS_CONNECTION_SERIAL, IDSS_CONNECTION_APPLETALK, IDSS_CONNECTION_TCPIP,               
	 0, IDSS_CONNECTION_SERIAL, IDSS_CONNECTION_APPLETALK, IDSS_CONNECTION_TCPIP,
	 IDSS_CONNECTION_LOCAL, 0, 0, 0,   //review: do mips, alpha, or powerpc
	 IDSS_CONNECTION_LOCAL, 0, 0, 0,   //have non-local connections?
	 0, 0, 0, IDSS_CONNECTION_TCPIP};

int settings_title_id[7][4] =                                                                                                                                                                                        
	{0, IDSS_DLG_TITLE_WIN32_SERIAL, 0, IDSS_DLG_TITLE_WIN32_TCPIP,
	 0, IDSS_DLG_TITLE_WIN32_SERIAL, 0, 0,
	 0, IDSS_DLG_TITLE_MAC68K_SERIAL, IDSS_DLG_TITLE_MAC68K_APPLETALK, IDSS_DLG_TITLE_MAC68K_TCPIP,                         
	 0, IDSS_DLG_TITLE_MACPPC_SERIAL, IDSS_DLG_TITLE_MACPPC_APPLETALK, IDSS_DLG_TITLE_MACPPC_TCPIP, 
	 0, 0, 0, 0,        //review: do mips, alpha, or powerpc have non-local connection?  
	 0, 0, 0, 0,       //if not no settings dlg title needed
	 0, 0, 0, IDSS_DLG_TITLE_WIN32_TCPIP};

int baud_rate_id[] = {IDSS_BAUD_300, IDSS_BAUD_600, IDSS_BAUD_1200, IDSS_BAUD_1800, 
					  IDSS_BAUD_2400, IDSS_BAUD_4800, IDSS_BAUD_7200, IDSS_BAUD_9600,
					  IDSS_BAUD_14400, IDSS_BAUD_19200, IDSS_BAUD_38400, IDSS_BAUD_57600};

int connector_id[] = {IDSS_CONNECTOR_COM1, IDSS_CONNECTOR_COM2, 
					  IDSS_CONNECTOR_COM3, IDSS_CONNECTOR_COM4};
					   
int data_bits_id[]	  = {IDSS_DATA_BITS_5, IDSS_DATA_BITS_6, IDSS_DATA_BITS_7, IDSS_DATA_BITS_8};                              
int flow_control_id[] = {IDSS_FLOW_CONTROL_HARDWARE, IDSS_FLOW_CONTROL_XONXOFF};          
int stop_bits_id[]    = {IDSS_STOP_BITS_1, IDSS_STOP_BITS_1_PT_5, IDSS_STOP_BITS_2};                         
int parity_id[]       = {IDSS_PARITY_NONE, IDSS_PARITY_ODD, IDSS_PARITY_EVEN};                           

CString user_arg_base[] = {"target_platforms", "connections",                                           //todo: localize
						   "remote_zone", "remote_machine_name", "remote_password",
						   "connector", "baud_rate", "data_bits",
						   "parity", "stop_bits", "flow_control",
						   "remote_local_path", "remote_share_path",
						   "remote_ip_address"};


COConnection::COConnection(PLATFORM_TYPE platform, CSettings **psettings)

	{
	m_platform = platform;
	m_connection = (CONNECTION_TYPE)psettings[0]->GetIntValue(settingConnection);
	m_local_path = psettings[0]->GetTextValue(settingRemoteLocalPath);
	m_share_path = psettings[0]->GetTextValue(settingRemoteSharePath);

	switch(m_connection)
		{
		case CONNECTION_TCP_IP:
			m_pConnecSettings = psettings[1];
			break;
		case CONNECTION_SERIAL:
			m_pConnecSettings = psettings[2];
			break;
		case CONNECTION_APPLETALK:
			m_pConnecSettings = psettings[3];
			break;
		}			
	}

// BEGIN_HELP_COMMENT
// Function: int COConnection::OpenConnectionDlg(void)
// Description: The description for this function is not available. Please see the appropriate component owner for a description of this function.
// Return: The return value description for this function is not available.
// Param: Param The parameter description(s) for this function are not available.
// END_HELP_COMMENT
int COConnection::OpenConnectionDlg(void)
	
	{
	UIWB.DoCommand(IDM_CONNECTION, DC_MNEMONIC);
	m_uicon = MST.WFndWndWait(m_uicon.ExpectedTitle(), FW_DEFAULT, 10);
	
	if(!m_uicon.HWnd())
	
		{
		LOG->RecordInfo("ERROR in COConnection::OpenConnectionDlg(): "
					 "failed to open connection dlg");
		
		return ERROR_ERROR;
		}

	return ERROR_SUCCESS;
	}


// BEGIN_HELP_COMMENT
// Function: int COConnection::OpenSettingsDlg(PLATFORM_TYPE platform, 
// Description: The description for this function is not available. Please see the appropriate component owner for a description of this function.
// Return: The return value description for this function is not available.
// Param: Param The parameter description(s) for this function are not available.
// END_HELP_COMMENT
int COConnection::OpenSettingsDlg(PLATFORM_TYPE platform, 
								  CONNECTION_TYPE connection)

	{
	if(OpenConnectionDlg() != ERROR_SUCCESS)
	
		{
		LOG->RecordInfo("ERROR in COConnection::OpenSettingsDlg(): "
					 "failed to open connection dlg");
		
		return ERROR_ERROR;
		}

	if(platform != PLATFORM_CURRENT)
		if(SetPlatformCore(platform) != ERROR_SUCCESS)
	
			{
			LOG->RecordInfo("ERROR in COConnection::OpenSettingsDlg(): "
						 "failed to set platform");
		
			CloseAllDlgs();
			return ERROR_ERROR;
			}

	if(connection != CONNECTION_CURRENT)
		if(SetConnectionCore(connection, platform) != ERROR_SUCCESS) 

			{
			LOG->RecordInfo("ERROR in COConnection::OpenSettingsDlg(): "
						 "failed to set connection");
		
			CloseAllDlgs();
			return ERROR_ERROR;
			}

	int current_platform = GetPowerOfTwo(GetPlatformCore());
	int current_connection = GetPowerOfTwo(GetConnectionCore());

	if((m_uiconset =
	   m_uicon.Settings(GetLocString(settings_title_id[current_platform][current_connection])))
	   == NULL)

			{
			LOG->RecordInfo("ERROR in COConnection::OpenSettingsDlg(): "
						 "failed to open settings dlg");
		
			CloseAllDlgs();
			return ERROR_ERROR;
			}

	return ERROR_SUCCESS;
	}


// BEGIN_HELP_COMMENT
// Function: int COConnection::CloseAllDlgs(void)
// Description: The description for this function is not available. Please see the appropriate component owner for a description of this function.
// Return: The return value description for this function is not available.
// Param: Param The parameter description(s) for this function are not available.
// END_HELP_COMMENT
int COConnection::CloseAllDlgs(void)

	{
	if(m_uiconset.IsActive())
		if(m_uiconset.Close() != NULL)

			{
			LOG->RecordInfo("ERROR in COConnection::CloseAllDlgs(): "
						 "failed to close settings dlg");
		
			return ERROR_ERROR;
			}

	if(m_uicon.IsActive())
		if(m_uicon.OK() != NULL)
 
			{
			LOG->RecordInfo("ERROR in COConnection::CloseAllDlgs(): "
						 "failed to close connection dlg");
		
			return ERROR_ERROR;
			}

	return ERROR_SUCCESS;
	}


// BEGIN_HELP_COMMENT
// Function: int COConnection::SetPlatformCore(PLATFORM_TYPE platform)
// Description: The description for this function is not available. Please see the appropriate component owner for a description of this function.
// Return: The return value description for this function is not available.
// Param: Param The parameter description(s) for this function are not available.
// END_HELP_COMMENT
int COConnection::SetPlatformCore(PLATFORM_TYPE platform)
	
	{
    if(m_uicon.SetPlatform(GetLocString(platform_id[GetPowerOfTwo(GetTruePlatform(platform))])) 
       != ERROR_SUCCESS)
	
		{
		LOG->RecordInfo("ERROR in COConnection::SetPlatformCore(): "
					 "failed to set platform");
		
		return ERROR_ERROR;
		}

	return ERROR_SUCCESS;
	}


// BEGIN_HELP_COMMENT
// Function: int COConnection::SetConnectionCore(CONNECTION_TYPE connection,
// Description: The description for this function is not available. Please see the appropriate component owner for a description of this function.
// Return: The return value description for this function is not available.
// Param: Param The parameter description(s) for this function are not available.
// END_HELP_COMMENT
int COConnection::SetConnectionCore(CONNECTION_TYPE connection,
									PLATFORM_TYPE platform)

	{
	PLATFORM_TYPE true_platform = GetTruePlatform(platform);
	 
	CONNECTION_TYPE true_connection =
		(connection != CONNECTION_USER) ? connection : 
		(CONNECTION_TYPE)GetUserConnections(platform);

	if(m_uicon.SetConnection(GetLocString(connection_id[GetPowerOfTwo(true_platform)]
										 [GetPowerOfTwo(true_connection)]))
										 != ERROR_SUCCESS)
	
		{
		LOG->RecordInfo("ERROR in COConnection::SetConnectionCore(): "
					 "failed to set connection");
		
		return ERROR_ERROR;
		}

	return ERROR_SUCCESS;
	}


// BEGIN_HELP_COMMENT
// Function: int COConnection::SetZoneCore(LPCSTR zone, PLATFORM_TYPE platform)
// Description: The description for this function is not available. Please see the appropriate component owner for a description of this function.
// Return: The return value description for this function is not available.
// Param: Param The parameter description(s) for this function are not available.
// END_HELP_COMMENT
int COConnection::SetZoneCore(LPCSTR zone, PLATFORM_TYPE platform)
	
	{
	CString true_zone = zone ? zone : m_pConnecSettings->GetTextValue(settingRemoteZone);
	
	if(m_uiconset.SetZone(true_zone) != ERROR_SUCCESS)
	
		{
		LOG->RecordInfo("ERROR in COConnection::SetZoneCore(): "
					 "failed to set zone");
		
		return ERROR_ERROR;
		}

	return ERROR_SUCCESS;
	}


// BEGIN_HELP_COMMENT
// Function: int COConnection::SetMachineNameCore(LPCSTR machine_name, 
// Description: The description for this function is not available. Please see the appropriate component owner for a description of this function.
// Return: The return value description for this function is not available.
// Param: Param The parameter description(s) for this function are not available.
// END_HELP_COMMENT
int COConnection::SetMachineNameCore(LPCSTR machine_name, 
									 PLATFORM_TYPE platform)

	{
	CString true_machine_name = machine_name ? machine_name :
								m_pConnecSettings->GetTextValue(settingRemoteMachName);

	if(m_uiconset.SetMachineName(true_machine_name) != ERROR_SUCCESS)
	
		{
		LOG->RecordInfo("ERROR in COConnection::SetMachineNameCore(): "
					 "failed to set machine name");
		
		return ERROR_ERROR;
		}

	return ERROR_SUCCESS;
	}


// BEGIN_HELP_COMMENT
// Function: int COConnection::SetPasswordCore(LPCSTR password, 
// Description: The description for this function is not available. Please see the appropriate component owner for a description of this function.
// Return: The return value description for this function is not available.
// Param: Param The parameter description(s) for this function are not available.
// END_HELP_COMMENT
int COConnection::SetPasswordCore(LPCSTR password, CONNECTION_TYPE connection,
								  PLATFORM_TYPE platform)

	{
	// TODO(michma): v6 IDE no longer supports password. since it is disabled in the UI
	// we must ignore it here.
	return ERROR_SUCCESS;

	CString true_password;
	CONNECTION_TYPE true_connection = GetTrueConnection(connection, platform);

	switch(true_connection)
		{
		case CONNECTION_APPLETALK:
			true_password = password ? password :
				m_pConnecSettings->GetTextValue(settingRemoteATPassword);
			break;
		case CONNECTION_TCP_IP:
			true_password = password ? password :
				m_pConnecSettings->GetTextValue(settingRemoteIPPassword);
			break;
		default:
			return ERROR_ERROR;
		}

	if(m_uiconset.SetPassword(true_password, true_connection) != ERROR_SUCCESS)
	
		{
		LOG->RecordInfo("ERROR in COConnection::SetPasswordCore(): "
					 "failed to set password");
		
		return ERROR_ERROR;
		}

	return ERROR_SUCCESS;
	}


// BEGIN_HELP_COMMENT
// Function: int COConnection::SetConnectorCore(CONNECTOR_TYPE connector, 
// Description: The description for this function is not available. Please see the appropriate component owner for a description of this function.
// Return: The return value description for this function is not available.
// Param: Param The parameter description(s) for this function are not available.
// END_HELP_COMMENT
int COConnection::SetConnectorCore(CONNECTOR_TYPE connector, 
								   PLATFORM_TYPE platform)

	{        
	CONNECTOR_TYPE true_connector = (connector != CONNECTOR_USER) ? connector :
		(CONNECTOR_TYPE)m_pConnecSettings->GetIntValue(settingConnector);
	
	if(m_uiconset.SetConnector(GetLocString(connector_id[true_connector])) != ERROR_SUCCESS)
	
		{
		LOG->RecordInfo("ERROR in COConnection::SetConnectorCore(): "
					 "failed to set connector");
		
		return ERROR_ERROR;
		}

	return ERROR_SUCCESS;
	}


// BEGIN_HELP_COMMENT
// Function: int COConnection::SetBaudRateCore(BAUD_RATE_TYPE baud_rate, 
// Description: The description for this function is not available. Please see the appropriate component owner for a description of this function.
// Return: The return value description for this function is not available.
// Param: Param The parameter description(s) for this function are not available.
// END_HELP_COMMENT
int COConnection::SetBaudRateCore(BAUD_RATE_TYPE baud_rate, 
								  PLATFORM_TYPE platform)

	{
	BAUD_RATE_TYPE true_baud_rate = (baud_rate != BAUD_RATE_USER) ? baud_rate :
		(BAUD_RATE_TYPE)m_pConnecSettings->GetIntValue(settingBaudRate);

	if(m_uiconset.SetBaudRate(GetLocString(baud_rate_id[true_baud_rate])) != ERROR_SUCCESS)
	
		{
		LOG->RecordInfo("ERROR in COConnection::SetBaudRateCore(): "
					 "failed to set baud rate");
		
		return ERROR_ERROR;
		}

	return ERROR_SUCCESS;
	}


// BEGIN_HELP_COMMENT
// Function: int COConnection::SetDataBitsCore(DATA_BITS_TYPE data_bits, 
// Description: The description for this function is not available. Please see the appropriate component owner for a description of this function.
// Return: The return value description for this function is not available.
// Param: Param The parameter description(s) for this function are not available.
// END_HELP_COMMENT
int COConnection::SetDataBitsCore(DATA_BITS_TYPE data_bits, 
								  PLATFORM_TYPE platform)

	{
	DATA_BITS_TYPE true_data_bits = (data_bits != DATA_BITS_USER) ? data_bits :
		(DATA_BITS_TYPE)m_pConnecSettings->GetIntValue(settingDataBits);

	if(m_uiconset.SetDataBits(GetLocString(data_bits_id[true_data_bits])) != ERROR_SUCCESS)
	
		{
		LOG->RecordInfo("ERROR in COConnection::SetDataBitsCore(): "
					 "failed to set data bits");
		
		return ERROR_ERROR;
		}

	return ERROR_SUCCESS;
	}


// BEGIN_HELP_COMMENT
// Function: int COConnection::SetParityCore(PARITY_TYPE parity,        PLATFORM_TYPE platform)
// Description: The description for this function is not available. Please see the appropriate component owner for a description of this function.
// Return: The return value description for this function is not available.
// Param: Param The parameter description(s) for this function are not available.
// END_HELP_COMMENT
int COConnection::SetParityCore(PARITY_TYPE parity,     PLATFORM_TYPE platform)

	{
	PARITY_TYPE true_parity = (parity != PARITY_USER) ? parity :
		(PARITY_TYPE)m_pConnecSettings->GetIntValue(settingParity);

	if(m_uiconset.SetParity(GetLocString(parity_id[true_parity])) != ERROR_SUCCESS)
	
		{
		LOG->RecordInfo("ERROR in COConnection::SetParityCore(): "
			  "failed to set parity");
		
		return ERROR_ERROR;
		}

	return ERROR_SUCCESS;
	}


// BEGIN_HELP_COMMENT
// Function: int COConnection::SetStopBitsCore(STOP_BITS_TYPE stop_bits, 
// Description: The description for this function is not available. Please see the appropriate component owner for a description of this function.
// Return: The return value description for this function is not available.
// Param: Param The parameter description(s) for this function are not available.
// END_HELP_COMMENT
int COConnection::SetStopBitsCore(STOP_BITS_TYPE stop_bits, 
								  PLATFORM_TYPE platform)

	{
	STOP_BITS_TYPE true_stop_bits = (stop_bits != STOP_BITS_USER) ? stop_bits :
		(STOP_BITS_TYPE)m_pConnecSettings->GetIntValue(settingStopBits);

	if(m_uiconset.SetStopBits(GetLocString(stop_bits_id[true_stop_bits])) != ERROR_SUCCESS)
	
		{
		LOG->RecordInfo("ERROR in COConnection::SetStopBitsCore(): "
			  "failed to set stop bits");
		
		return ERROR_ERROR;
		}

	return ERROR_SUCCESS;
	}


// BEGIN_HELP_COMMENT
// Function: int COConnection::SetFlowControlCore(FLOW_CONTROL_TYPE flow_control, 
// Description: The description for this function is not available. Please see the appropriate component owner for a description of this function.
// Return: The return value description for this function is not available.
// Param: Param The parameter description(s) for this function are not available.
// END_HELP_COMMENT
int COConnection::SetFlowControlCore(FLOW_CONTROL_TYPE flow_control, 
									 PLATFORM_TYPE platform)

	{
	FLOW_CONTROL_TYPE true_flow_control = 
		(flow_control != FLOW_CONTROL_USER) ? flow_control : 
		(FLOW_CONTROL_TYPE)m_pConnecSettings->GetIntValue(settingFlowControl);

	if(m_uiconset.SetFlowControl(GetLocString(flow_control_id[true_flow_control]))
	   != ERROR_SUCCESS)
	
		{
		LOG->RecordInfo("ERROR in COConnection::SetFlowControlCore(): "
					 "failed to set flow control");
		
		return ERROR_ERROR;
		}

	return ERROR_SUCCESS;
	}


// BEGIN_HELP_COMMENT
// Function: int COConnection::SetIPAddressCore(LPCSTR ip_address,
// Description: The description for this function is not available. Please see the appropriate component owner for a description of this function.
// Return: The return value description for this function is not available.
// Param: Param The parameter description(s) for this function are not available.
// END_HELP_COMMENT
int COConnection::SetIPAddressCore(LPCSTR ip_address, PLATFORM_TYPE platform)
	
	{
	CString true_ip_address = ip_address ? ip_address : 
		m_pConnecSettings->GetTextValue(settingRemoteIPAddress);

	if(m_uiconset.SetIPAddress(true_ip_address) != ERROR_SUCCESS)
	
		{
		LOG->RecordInfo("ERROR in COConnection::SetIPAddressCore(): "
					 "failed to set ip address");
		
		return ERROR_ERROR;
		}

	return ERROR_SUCCESS;
	}


// BEGIN_HELP_COMMENT
// Function: int COConnection::SetPlatform(PLATFORM_TYPE platform /* PLATFORM_USER */)
// Description: The description for this function is not available. Please see the appropriate component owner for a description of this function.
// Return: The return value description for this function is not available.
// Param: Param The parameter description(s) for this function are not available.
// END_HELP_COMMENT
int COConnection::SetPlatform(PLATFORM_TYPE platform /* PLATFORM_USER */)

	{
	if(OpenConnectionDlg() != ERROR_SUCCESS)

		{
		LOG->RecordInfo("ERROR in COConnection::SetPlatform(): "
					 "failed to open connection dlg");

		return ERROR_ERROR;
		}
	
	if(SetPlatformCore(platform) != ERROR_SUCCESS)

		{
		LOG->RecordInfo("ERROR in COConnection::SetPlatform(): "
					 "failed to set platform");

		CloseAllDlgs();
		return ERROR_ERROR;
		}

	if(CloseAllDlgs() != ERROR_SUCCESS)

		{
		LOG->RecordInfo("ERROR in COConnection::SetPlatform(): "
					 "failed to close all dlgs");

		return ERROR_ERROR;
		}

	return ERROR_SUCCESS;
	}


// BEGIN_HELP_COMMENT
// Function: int COConnection::SetConnection(CONNECTION_TYPE connection /* CONNECTION_USER */,
// Description: The description for this function is not available. Please see the appropriate component owner for a description of this function.
// Return: The return value description for this function is not available.
// Param: Param The parameter description(s) for this function are not available.
// END_HELP_COMMENT
int COConnection::SetConnection(CONNECTION_TYPE connection /* CONNECTION_USER */,
								PLATFORM_TYPE platform /* PLATFORM_USER */)

	{
	if(OpenConnectionDlg() != ERROR_SUCCESS)

		{
		LOG->RecordInfo("ERROR in COConnection::SetConnection(): "
					 "failed to open connection dlg");

		return ERROR_ERROR;
		}
	
	if(platform != PLATFORM_CURRENT)
		if(SetPlatformCore(platform) != ERROR_SUCCESS)

			{
			LOG->RecordInfo("ERROR in COConnection::SetConnection(): "
						 "failed to set platform");

			CloseAllDlgs();
			return ERROR_ERROR;
			}

	if(SetConnectionCore(connection, platform) != ERROR_SUCCESS) 

		{
		LOG->RecordInfo("ERROR in COConnection::SetConnection(): "
					 "failed to set connection");
		
		CloseAllDlgs();
		return ERROR_ERROR;
		}

	if(CloseAllDlgs() != ERROR_SUCCESS)

		{
		LOG->RecordInfo("ERROR in COConnection::SetConnection(): "
					 "failed to close all dlgs");

		return ERROR_ERROR;
		}

	return ERROR_SUCCESS;
	}


// BEGIN_HELP_COMMENT
// Function: int COConnection::SetZone(LPCSTR zone /* ZONE_USER */,
// Description: The description for this function is not available. Please see the appropriate component owner for a description of this function.
// Return: The return value description for this function is not available.
// Param: Param The parameter description(s) for this function are not available.
// END_HELP_COMMENT
int COConnection::SetZone(LPCSTR zone /* ZONE_USER */,
						  PLATFORM_TYPE platform /* PLATFORM_USER */)

	{
	if(OpenSettingsDlg(platform, CONNECTION_APPLETALK) != ERROR_SUCCESS)

		{
		LOG->RecordInfo("ERROR in COConnection::SetZone(): "
					 "failed to open settings dlg");
	
		return ERROR_ERROR;
		}

	if(SetZoneCore(zone, platform) != ERROR_SUCCESS)

		{
		LOG->RecordInfo("ERROR in COConnection::SetZone(): "
					 "failed to set zone");
	
		CloseAllDlgs();
		return ERROR_ERROR;
		}

	if(CloseAllDlgs() != ERROR_SUCCESS)

		{
		LOG->RecordInfo("ERROR in COConnection::SetZone(): "
					 "failed to close all dlgs");

		return ERROR_ERROR;
		}

	return ERROR_SUCCESS;
	}


// BEGIN_HELP_COMMENT
// Function: int COConnection::SetMachineName(LPCSTR machine_name 
// Description: The description for this function is not available. Please see the appropriate component owner for a description of this function.
// Return: The return value description for this function is not available.
// Param: Param The parameter description(s) for this function are not available.
// END_HELP_COMMENT
int COConnection::SetMachineName(LPCSTR machine_name /* MACHINE_NAME_USER */,
								 PLATFORM_TYPE platform /* PLATFORM_USER */)

	{
	if(OpenSettingsDlg(platform, CONNECTION_APPLETALK) != ERROR_SUCCESS)

		{
		LOG->RecordInfo("ERROR in COConnection::SetMachineName(): "
					 "failed to open settings dlg");
	
		return ERROR_ERROR;
		}

	if(SetMachineNameCore(machine_name, platform) != ERROR_SUCCESS)

		{
		LOG->RecordInfo("ERROR in COConnection::SetMachineName(): "
					 "failed to set machine name");
	
		CloseAllDlgs();
		return ERROR_ERROR;
		}
	
	if(CloseAllDlgs() != ERROR_SUCCESS)

		{
		LOG->RecordInfo("ERROR in COConnection::SetMachineName(): "
					 "failed to close all dlgs");

		return ERROR_ERROR;
		}

	return ERROR_SUCCESS;
	}


// BEGIN_HELP_COMMENT
// Function: int COConnection::SetPassword(LPCSTR password /* PASSWORD_USER */,
// Description: The description for this function is not available. Please see the appropriate component owner for a description of this function.
// Return: The return value description for this function is not available.
// Param: Param The parameter description(s) for this function are not available.
// END_HELP_COMMENT
int COConnection::SetPassword(LPCSTR password /* PASSWORD_USER */,
							  CONNECTION_TYPE connection /* CONNECTION_USER */,
							  PLATFORM_TYPE platform /* PLATFORM_USER */)

	{
	if(OpenSettingsDlg(platform, connection) != ERROR_SUCCESS)

		{
		LOG->RecordInfo("ERROR in COConnection::SetPassword(): "
					 "failed to open settings dlg");
	
		return ERROR_ERROR;
		}

	if(SetPasswordCore(password, connection, platform) != ERROR_SUCCESS)

		{
		LOG->RecordInfo("ERROR in COConnection::SetPassword(): "
					 "failed to set password");
	
		CloseAllDlgs();
		return ERROR_ERROR;
		}

	if(CloseAllDlgs() != ERROR_SUCCESS)

		{
		LOG->RecordInfo("ERROR in COConnection::SetPassword(): "
					 "failed to close all dlgs");

		return ERROR_ERROR;
		}

	return ERROR_SUCCESS;
	}


// BEGIN_HELP_COMMENT
// Function: int COConnection::SetConnector(CONNECTOR_TYPE connector /* CONNECTOR_USER */,
// Description: The description for this function is not available. Please see the appropriate component owner for a description of this function.
// Return: The return value description for this function is not available.
// Param: Param The parameter description(s) for this function are not available.
// END_HELP_COMMENT
int COConnection::SetConnector(CONNECTOR_TYPE connector /* CONNECTOR_USER */,
							   PLATFORM_TYPE platform /* PLATFORM_USER */)

	{
	if(OpenSettingsDlg(platform, CONNECTION_SERIAL) != ERROR_SUCCESS)

		{
		LOG->RecordInfo("ERROR in COConnection::SetConnector(): "
					 "failed to open settings dlg");
	
		return ERROR_ERROR;
		}

	if(SetConnectorCore(connector, platform) != ERROR_SUCCESS) 

		{
		LOG->RecordInfo("ERROR in COConnection::SetConnector(): "
					 "failed to set connector");
	
		CloseAllDlgs();
		return ERROR_ERROR;
		}
	
	if(CloseAllDlgs() != ERROR_SUCCESS)

		{
		LOG->RecordInfo("ERROR in COConnection::SetConnector(): "
					 "failed to close all dlgs");

		return ERROR_ERROR;
		}

	return ERROR_SUCCESS;
	}


// BEGIN_HELP_COMMENT
// Function: int COConnection::SetBaudRate(BAUD_RATE_TYPE baud_rate /* BAUD_RATE_USER */,
// Description: The description for this function is not available. Please see the appropriate component owner for a description of this function.
// Return: The return value description for this function is not available.
// Param: Param The parameter description(s) for this function are not available.
// END_HELP_COMMENT
int COConnection::SetBaudRate(BAUD_RATE_TYPE baud_rate /* BAUD_RATE_USER */,
							  PLATFORM_TYPE platform /* PLATFORM_USER */)

	{
	if(OpenSettingsDlg(platform, CONNECTION_SERIAL) != ERROR_SUCCESS)

		{
		LOG->RecordInfo("ERROR in COConnection::SetBaudRate(): "
					 "failed to open settings dlg");
	
		return ERROR_ERROR;
		}

	if(SetBaudRateCore(baud_rate, platform) != ERROR_SUCCESS) 

		{
		LOG->RecordInfo("ERROR in COConnection::SetBaudRate(): "
					 "failed to set baud rate");
	
		CloseAllDlgs();
		return ERROR_ERROR;
		}
	
	if(CloseAllDlgs() != ERROR_SUCCESS)

		{
		LOG->RecordInfo("ERROR in COConnection::SetBaudRate(): "
					 "failed to close all dlgs");

		return ERROR_ERROR;
		}

	return ERROR_SUCCESS;
	}


// BEGIN_HELP_COMMENT
// Function: int COConnection::SetDataBits(DATA_BITS_TYPE data_bits /* DATA_BITS_USER */,
// Description: The description for this function is not available. Please see the appropriate component owner for a description of this function.
// Return: The return value description for this function is not available.
// Param: Param The parameter description(s) for this function are not available.
// END_HELP_COMMENT
int COConnection::SetDataBits(DATA_BITS_TYPE data_bits /* DATA_BITS_USER */,
							  PLATFORM_TYPE platform /* PLATFORM_USER */)

	{
	if(OpenSettingsDlg(platform, CONNECTION_SERIAL) != ERROR_SUCCESS)

		{
		LOG->RecordInfo("ERROR in COConnection::SetDataBits(): "
					 "failed to open settings dlg");
	
		return ERROR_ERROR;
		}

	if(SetDataBitsCore(data_bits, platform) != ERROR_SUCCESS) 

		{
		LOG->RecordInfo("ERROR in COConnection::SetDataBits(): "
					 "failed to set data bits");
	
		CloseAllDlgs();
		return ERROR_ERROR;
		}
	
	if(CloseAllDlgs() != ERROR_SUCCESS)

		{
		LOG->RecordInfo("ERROR in COConnection::SetDataBits(): "
					 "failed to close all dlgs");

		return ERROR_ERROR;
		}

	return ERROR_SUCCESS;
	}


// BEGIN_HELP_COMMENT
// Function: int COConnection::SetParity(PARITY_TYPE parity /* PARITY_USER */,
// Description: The description for this function is not available. Please see the appropriate component owner for a description of this function.
// Return: The return value description for this function is not available.
// Param: Param The parameter description(s) for this function are not available.
// END_HELP_COMMENT
int COConnection::SetParity(PARITY_TYPE parity /* PARITY_USER */,
						PLATFORM_TYPE platform /* PLATFORM_USER */)

	{
	if(OpenSettingsDlg(platform, CONNECTION_SERIAL) != ERROR_SUCCESS)

		{
		LOG->RecordInfo("ERROR in COConnection::SetParity(): "
					 "failed to open settings dlg");
	
		return ERROR_ERROR;
		}

	if(SetParityCore(parity, platform) != ERROR_SUCCESS)

		{
		LOG->RecordInfo("ERROR in COConnection::SetParity(): "
					 "failed to set parity");
	
		CloseAllDlgs();
		return ERROR_ERROR;
		}

	if(CloseAllDlgs() != ERROR_SUCCESS)

		{
		LOG->RecordInfo("ERROR in COConnection::SetParity(): "
					 "failed to close all dlgs");

		return ERROR_ERROR;
		}

	return ERROR_SUCCESS;
	}


// BEGIN_HELP_COMMENT
// Function: int COConnection::SetStopBits(STOP_BITS_TYPE stop_bits /* STOP_BITS_USER */,
// Description: The description for this function is not available. Please see the appropriate component owner for a description of this function.
// Return: The return value description for this function is not available.
// Param: Param The parameter description(s) for this function are not available.
// END_HELP_COMMENT
int COConnection::SetStopBits(STOP_BITS_TYPE stop_bits /* STOP_BITS_USER */,
							  PLATFORM_TYPE platform /* PLATFORM_USER */)

	{
	if(OpenSettingsDlg(platform, CONNECTION_SERIAL) != ERROR_SUCCESS)

		{
		LOG->RecordInfo("ERROR in COConnection::SetStopBits(): "
					 "failed to open settings dlg");
	
		return ERROR_ERROR;
		}

	if(SetStopBitsCore(stop_bits, platform) != ERROR_SUCCESS) 

		{
		LOG->RecordInfo("ERROR in COConnection::SetStopBits(): "
					 "failed to set stop bits");
	
		CloseAllDlgs();
		return ERROR_ERROR;
		}
	
	if(CloseAllDlgs() != ERROR_SUCCESS)

		{
		LOG->RecordInfo("ERROR in COConnection::SetStopBits(): "
					 "failed to close all dlgs");

		return ERROR_ERROR;
		}

	return ERROR_SUCCESS;
	}


// BEGIN_HELP_COMMENT
// Function: int COConnection::SetFlowControl(FLOW_CONTROL_TYPE flow_control /* FLOW_CONTROL_USER */,
// Description: The description for this function is not available. Please see the appropriate component owner for a description of this function.
// Return: The return value description for this function is not available.
// Param: Param The parameter description(s) for this function are not available.
// END_HELP_COMMENT
int COConnection::SetFlowControl(FLOW_CONTROL_TYPE flow_control /* FLOW_CONTROL_USER */,
								 PLATFORM_TYPE platform /* PLATFORM_USER */)

	{
	if(OpenSettingsDlg(platform, CONNECTION_SERIAL) != ERROR_SUCCESS)

		{
		LOG->RecordInfo("ERROR in COConnection::SetFlowControl(): "
					 "failed to open settings dlg");
	
		return ERROR_ERROR;
		}

	if(SetFlowControlCore(flow_control, platform) != ERROR_SUCCESS)

		{
		LOG->RecordInfo("ERROR in COConnection::SetFlowControl(): "
					 "failed to set flow control");
	
		CloseAllDlgs();
		return ERROR_ERROR;
		}
	
	if(CloseAllDlgs() != ERROR_SUCCESS)

		{
		LOG->RecordInfo("ERROR in COConnection::SetFlowControl(): "
					 "failed to close all dlgs");

		return ERROR_ERROR;
		}

	return ERROR_SUCCESS;
	}


// BEGIN_HELP_COMMENT
// Function: int COConnection::SetIPAddress(LPCSTR ip_address /* IP_ADDRESS_USER */,
// Description: The description for this function is not available. Please see the appropriate component owner for a description of this function.
// Return: The return value description for this function is not available.
// Param: Param The parameter description(s) for this function are not available.
// END_HELP_COMMENT
int COConnection::SetIPAddress(LPCSTR ip_address /* IP_ADDRESS_USER */,
							   PLATFORM_TYPE platform /* PLATFORM_USER */)

	{
	if(OpenSettingsDlg(platform, CONNECTION_TCP_IP) != ERROR_SUCCESS)

		{
		LOG->RecordInfo("ERROR in COConnection::SetIPAddress(): "
					 "failed to open settings dlg");
	
		return ERROR_ERROR;
		}

	if(SetIPAddressCore(ip_address, platform) != ERROR_SUCCESS)

		{
		LOG->RecordInfo("ERROR in COConnection::SetIPAddress(): "
					 "failed to set ip address");
	
		CloseAllDlgs();
		return ERROR_ERROR;
		}
	
	if(CloseAllDlgs() != ERROR_SUCCESS)

		{
		LOG->RecordInfo("ERROR in COConnection::SetIPAddress(): "
					 "failed to close all dlgs");

		return ERROR_ERROR;
		}

	return ERROR_SUCCESS;
	}


// BEGIN_HELP_COMMENT
// Function: int COConnection::SetAll(PLATFORM_TYPE platform /* PLATFORM_USER */,
// Description: The description for this function is not available. Please see the appropriate component owner for a description of this function.
// Return: The return value description for this function is not available.
// Param: Param The parameter description(s) for this function are not available.
// END_HELP_COMMENT
int COConnection::SetAll(PLATFORM_TYPE platform /* PLATFORM_USER */,
						 CONNECTION_TYPE connection /* CONNECTION_USER */)

	{
	PLATFORM_TYPE true_platform = GetTruePlatform(platform);
	CONNECTION_TYPE true_connection = GetTrueConnection(connection, platform);

	if(((true_platform == PLATFORM_WIN32_X86)||(true_platform == PLATFORM_WIN32_MIPS)) && 
	   (true_connection == CONNECTION_LOCAL))
			
		{
		if(OpenConnectionDlg() != ERROR_SUCCESS)

			{
			LOG->RecordInfo("ERROR in COConnection::SetAll(): "
						 "failed to open connection dlg");

			return ERROR_ERROR;
			}

#ifndef _M_ALPHA
		if(SetPlatformCore(PLATFORM_WIN32_X86) != ERROR_SUCCESS)

			{
			LOG->RecordInfo("ERROR in COConnection::SetAll(): "
						 "failed to set platform to win32 x86");
	
			CloseAllDlgs();
			return ERROR_ERROR;
			}

					
		if(SetConnectionCore(CONNECTION_LOCAL, PLATFORM_CURRENT) 
		   != ERROR_SUCCESS)

			{
			LOG->RecordInfo("ERROR in COConnection::SetAll(): "
						 "failed to set connection to local");
	
			CloseAllDlgs();
			return ERROR_ERROR;
			}

#else
#pragma (message, "COConnect::SetAll is bogus on Alpha.")
// The code above should probably be modified to work 100% correctly on
// ALPHA instead of being commented out, but the fact that it errors due
// to the call to SetPlatformCore which cannot succeed when on Alpha
// stopped it from working.
#endif
		if(CloseAllDlgs() != ERROR_SUCCESS)

			{
			LOG->RecordInfo("ERROR in COConnection::SetAll(): "
						 "failed to close all dlgs");
	
			return ERROR_ERROR;
			}
		
		return ERROR_SUCCESS;
		}
		
	else if(((true_platform == PLATFORM_WIN32_X86) ||
			 (true_platform == PLATFORM_WIN32S_X86)) &&
			 (true_connection == CONNECTION_SERIAL))  
	
		{               
		if(SetAll(platform, CONNECTION_SERIAL, CONNECTOR_USER,
				  BAUD_RATE_USER, FLOW_CONTROL_USER))

			{
			LOG->RecordInfo("ERROR in COConnection::SetAll(): "
						 "failed to set all win32(s) x86 serial settings");
	
			return ERROR_ERROR;
			}

		return ERROR_SUCCESS;
		}


	else if(((true_platform == PLATFORM_MAC_68K) || 
			 (true_platform == PLATFORM_MAC_PPC)) &&
			 (true_connection == CONNECTION_APPLETALK))  
	
		{
		if(SetAll(platform, connection, 
				  (LPCSTR)ZONE_USER, (LPCSTR)MACHINE_NAME_USER, (LPCSTR)PASSWORD_USER))

			{
			LOG->RecordInfo("ERROR in COConnection::SetAll(): "
						 "failed to set all mac appletalk settings");
	
			return ERROR_ERROR;
			}

		return ERROR_SUCCESS;
		}
				
	else if((true_platform == PLATFORM_MAC_68K) && 
			(true_connection == CONNECTION_SERIAL))
				
		{
		if(SetAll(platform, CONNECTION_SERIAL, CONNECTOR_USER, BAUD_RATE_USER, 
				  DATA_BITS_USER, PARITY_USER, STOP_BITS_USER))

			{
			LOG->RecordInfo("ERROR in COConnection::SetAll(): "
						 "failed to set all mac 68k serial settings");
	
			return ERROR_ERROR;
			}

		return ERROR_SUCCESS;
		}

	else if((true_platform == PLATFORM_MAC_PPC) && 
			(true_connection == CONNECTION_SERIAL))

		{
		if(SetAll(platform, CONNECTION_SERIAL, CONNECTOR_USER))

			{
			LOG->RecordInfo("ERROR in COConnection::SetAll(): "
						 "failed to set all mac ppc serial settings");
	
			return ERROR_ERROR;
			}

		return ERROR_SUCCESS;
		}

	else if(true_connection == CONNECTION_TCP_IP)

		{
		if(SetAll(platform, connection, IP_ADDRESS_USER, PASSWORD_USER))

			{
			LOG->RecordInfo("ERROR in COConnection::SetAll(): "
						 "failed to set all tcp/ip settings");
	
			return ERROR_ERROR;
			}

		return ERROR_SUCCESS;
		}
		
	else
	
		{
		LOG->RecordInfo("ERROR in COConnection::SetAll(): "
					 "invalid platform/connection combination");
					
		return ERROR_ERROR;
		}
	}


// BEGIN_HELP_COMMENT
// Function: int COConnection::SetAll(PLATFORM_TYPE platform, CONNECTION_TYPE connection,
// Description: The description for this function is not available. Please see the appropriate component owner for a description of this function.
// Return: The return value description for this function is not available.
// Param: Param The parameter description(s) for this function are not available.
// END_HELP_COMMENT
int COConnection::SetAll(PLATFORM_TYPE platform, CONNECTION_TYPE connection,
						 CONNECTOR_TYPE connector, BAUD_RATE_TYPE baud_rate,
						 FLOW_CONTROL_TYPE flow_control)

	{
	PLATFORM_TYPE true_platform = GetTruePlatform(platform);

	if(((true_platform != PLATFORM_WIN32_X86) &&
		(true_platform != PLATFORM_WIN32S_X86)) ||
		(connection != CONNECTION_SERIAL))

		{
		LOG->RecordInfo("ERROR in COConnection::SetAll(): "
					 "platform/connection must be win32(s)/serial");
		
		return ERROR_ERROR;
		}

	if(SetAllSerialCore(platform, connector) != ERROR_SUCCESS)
		{
		LOG->RecordInfo("ERROR in COConnection::SetAll(): "
					 "failed to do serial core for win32(s)");
		
		return ERROR_ERROR;
		}
				
	if(SetBaudRateCore(baud_rate, platform) != ERROR_SUCCESS) 

		{
		LOG->RecordInfo("ERROR in COConnection::SetAll(): "
					 "failed to set baud rate for win32(s)");
		
		CloseAllDlgs();
		return ERROR_ERROR;
		}

	if(SetFlowControlCore(flow_control, platform) != ERROR_SUCCESS) 

		{
		LOG->RecordInfo("ERROR in COConnection::SetAll(): "
					 "failed to set flow control for win32(s)");
		
		CloseAllDlgs();
		return ERROR_ERROR;
		}

	if(CloseAllDlgs() != ERROR_SUCCESS)

		{
		LOG->RecordInfo("ERROR in COConnection::SetAll(): "
					 "failed to close all dlgs for win32(s)/serial");
		
		return ERROR_ERROR;
		}

	return ERROR_SUCCESS;
	}


// BEGIN_HELP_COMMENT
// Function: int COConnection::SetAll(PLATFORM_TYPE platform, CONNECTION_TYPE connection,
// Description: The description for this function is not available. Please see the appropriate component owner for a description of this function.
// Return: The return value description for this function is not available.
// Param: Param The parameter description(s) for this function are not available.
// END_HELP_COMMENT
int COConnection::SetAll(PLATFORM_TYPE platform, CONNECTION_TYPE connection,
						 LPCSTR zone, LPCSTR machine_name, LPCSTR password)

	{
	PLATFORM_TYPE true_platform = GetTruePlatform(platform);
	CONNECTION_TYPE true_connection = GetTrueConnection(connection, platform);

	if(((true_platform != PLATFORM_MAC_68K) && 
		(true_platform != PLATFORM_MAC_PPC)) ||
	   (true_connection != CONNECTION_APPLETALK)) 

		{
		LOG->RecordInfo("ERROR in COConnection::SetAll(): "
					 "platform/connection must be mac/appletalk");
		
		return ERROR_ERROR;
		}

	if(OpenSettingsDlg(platform, connection) != ERROR_SUCCESS)

		{
		LOG->RecordInfo("ERROR in COConnection::SetAll(): "
					 "failed to open settings dlg for mac/appletalk");
		
		return ERROR_ERROR;
		}

	if(SetZoneCore(zone, platform) != ERROR_SUCCESS) 

		{
		LOG->RecordInfo("ERROR in COConnection::SetAll(): "
					 "failed to set zone for mac");
		
		CloseAllDlgs();
		return ERROR_ERROR;
		}

	if(SetMachineNameCore(machine_name, platform) != ERROR_SUCCESS) 

		{
		LOG->RecordInfo("ERROR in COConnection::SetAll(): "
					 "failed to set machine name for mac");
		
		CloseAllDlgs();
		return ERROR_ERROR;
		}

	if(SetPasswordCore(password, connection, platform) != ERROR_SUCCESS) 

		{
		LOG->RecordInfo("ERROR in COConnection::SetAll(): "
					 "failed to set password for mac/appletalk");
		
		CloseAllDlgs();
		return ERROR_ERROR;
		}

	if(CloseAllDlgs() != ERROR_SUCCESS)

		{
		LOG->RecordInfo("ERROR in COConnection::SetAll(): "
					 "failed to close all dlgs for mac/appletalk");
		
		return ERROR_ERROR;
		}
			
	return ERROR_SUCCESS;
	}


// BEGIN_HELP_COMMENT
// Function: int COConnection::SetAll(PLATFORM_TYPE platform, CONNECTION_TYPE connection,
// Description: The description for this function is not available. Please see the appropriate component owner for a description of this function.
// Return: The return value description for this function is not available.
// Param: Param The parameter description(s) for this function are not available.
// END_HELP_COMMENT
int COConnection::SetAll(PLATFORM_TYPE platform, CONNECTION_TYPE connection,
						 CONNECTOR_TYPE connector, BAUD_RATE_TYPE baud_rate, 
						 DATA_BITS_TYPE data_bits, PARITY_TYPE parity, 
						 STOP_BITS_TYPE stop_bits)

	{
	PLATFORM_TYPE true_platform = GetTruePlatform(platform);

	if((true_platform != PLATFORM_MAC_68K) || (connection != CONNECTION_SERIAL)) 

		{
		LOG->RecordInfo("ERROR in COConnection::SetAll(): "
					 "platform/connection must be mac 68k/serial");
		
		return ERROR_ERROR;
		}

	if(SetAllSerialCore(platform, connector) != ERROR_SUCCESS)
		{
		LOG->RecordInfo("ERROR in COConnection::SetAll(): "
					 "failed to do serial core for mac 68k");
		
		return ERROR_ERROR;
		}               

	if(SetBaudRateCore(baud_rate, platform) != ERROR_SUCCESS) 

		{
		LOG->RecordInfo("ERROR in COConnection::SetAll(): "
					 "failed to set baud rate for mac 68k");
		
		CloseAllDlgs();
		return ERROR_ERROR;
		}

	if(SetDataBitsCore(data_bits, platform) != ERROR_SUCCESS) 

		{
		LOG->RecordInfo("ERROR in COConnection::SetAll(): "
					 "failed to set data bits for mac 68k");
		
		CloseAllDlgs();
		return ERROR_ERROR;
		}

	if(SetParityCore(parity, platform) != ERROR_SUCCESS) 

		{
		LOG->RecordInfo("ERROR in COConnection::SetAll(): "
					 "failed to set parity for mac 68k");
		
		CloseAllDlgs();
		return ERROR_ERROR;
		}

	if(SetStopBitsCore(stop_bits, platform) != ERROR_SUCCESS) 

		{
		LOG->RecordInfo("ERROR in COConnection::SetAll(): "
					 "failed to set stop bits for mac 68k");
		
		CloseAllDlgs();
		return ERROR_ERROR;
		}

	if(CloseAllDlgs() != ERROR_SUCCESS)

		{
		LOG->RecordInfo("ERROR in COConnection::SetAll(): "
					 "failed to close all dlgs for mac 68k/serial");
		
		return ERROR_ERROR;
		}

	return ERROR_SUCCESS;
	}


// BEGIN_HELP_COMMENT
// Function: int COConnection::SetAll(PLATFORM_TYPE platform, CONNECTION_TYPE connection,
// Description: The description for this function is not available. Please see the appropriate component owner for a description of this function.
// Return: The return value description for this function is not available.
// Param: Param The parameter description(s) for this function are not available.
// END_HELP_COMMENT
int COConnection::SetAll(PLATFORM_TYPE platform, CONNECTION_TYPE connection,
						 CONNECTOR_TYPE connector)

	{
	PLATFORM_TYPE true_platform = GetTruePlatform(platform);

	if((true_platform != PLATFORM_MAC_PPC) || (connection != CONNECTION_SERIAL))

		{
		LOG->RecordInfo("ERROR in COConnection::SetAll(): "
					 "platform/connection must be mac ppc/serial");
		
		return ERROR_ERROR;
		}

	if(SetAllSerialCore(platform, connector) != ERROR_SUCCESS)
		{
		LOG->RecordInfo("ERROR in COConnection::SetAll(): "
					 "failed to do serial core for mac ppc");
		
		return ERROR_ERROR;
		}               

	if(CloseAllDlgs() != ERROR_SUCCESS)

		{
		LOG->RecordInfo("ERROR in COConnection::SetAll(): "
					 "failed to close all dlgs for mac ppc/serial");
		
		return ERROR_ERROR;
		}

	return ERROR_SUCCESS;
	}


// BEGIN_HELP_COMMENT
// Function: int COConnection::SetAll(PLATFORM_TYPE platform, CONNECTION_TYPE connection,
// Description: The description for this function is not available. Please see the appropriate component owner for a description of this function.
// Return: The return value description for this function is not available.
// Param: Param The parameter description(s) for this function are not available.
// END_HELP_COMMENT
int COConnection::SetAll(PLATFORM_TYPE platform, CONNECTION_TYPE connection,
						 LPCSTR ip_address, LPCSTR password)

	{
	CONNECTION_TYPE true_connection = GetTrueConnection(connection, platform);

	if(true_connection != CONNECTION_TCP_IP)

		{
		LOG->RecordInfo("ERROR in COConnection::SetAll(): "
					 "connection must be tcp/ip");
		
		return ERROR_ERROR;
		}

	if(OpenSettingsDlg(platform, connection) != ERROR_SUCCESS)

		{
		LOG->RecordInfo("ERROR in COConnection::SetAll(): "
					 "failed to open settings dlg for tcp/ip");
		
		return ERROR_ERROR;
		}

	if(SetIPAddressCore(ip_address, platform) != ERROR_SUCCESS)
		{
		LOG->RecordInfo("ERROR in COConnection::SetAll(): "
					 "failed to set ip address");
		
		return ERROR_ERROR;
		}               

	if(SetPasswordCore(password, connection, platform) != ERROR_SUCCESS)
		{
		LOG->RecordInfo("ERROR in COConnection::SetAll(): "
					 "failed to set ip password");
		
		return ERROR_ERROR;
		}               

	if(CloseAllDlgs() != ERROR_SUCCESS)

		{
		LOG->RecordInfo("ERROR in COConnection::SetAll(): "
					 "failed to close all dlgs for tcp/ip");
		
		return ERROR_ERROR;
		}
   
	return ERROR_SUCCESS;
	}


// BEGIN_HELP_COMMENT
// Function: int COConnection::SetAllSerialCore(PLATFORM_TYPE platform, 
// Description: The description for this function is not available. Please see the appropriate component owner for a description of this function.
// Return: The return value description for this function is not available.
// Param: Param The parameter description(s) for this function are not available.
// END_HELP_COMMENT
int COConnection::SetAllSerialCore(PLATFORM_TYPE platform, 
								   CONNECTOR_TYPE connector)

	{
	if(OpenSettingsDlg(platform, CONNECTION_SERIAL) != ERROR_SUCCESS)

		{
		LOG->RecordInfo("ERROR in COConnection::SetAllSerialCore(): "
					 "failed to open settings dlg for serial");
		
		return ERROR_ERROR;
		}

	if(SetConnectorCore(connector, platform) != ERROR_SUCCESS)

		{
		LOG->RecordInfo("ERROR in COConnection::SetAllSerialCore(): "
					 "failed to set connector for serial");
		
		CloseAllDlgs();
		return ERROR_ERROR;
		}

	return ERROR_SUCCESS;
	}


// BEGIN_HELP_COMMENT
// Function: PLATFORM_TYPE COConnection::GetTruePlatform(PLATFORM_TYPE platform)
// Description: The description for this function is not available. Please see the appropriate component owner for a description of this function.
// Return: The return value description for this function is not available.
// Param: Param The parameter description(s) for this function are not available.
// END_HELP_COMMENT
PLATFORM_TYPE COConnection::GetTruePlatform(PLATFORM_TYPE platform)
	{
	switch(platform)
		{
		case PLATFORM_USER:
			return (PLATFORM_TYPE)GetUserPlatforms();
		case PLATFORM_CURRENT:
			return GetPlatformCore();
		default:
			return platform;
		}
	}


// BEGIN_HELP_COMMENT
// Function: CONNECTION_TYPE COConnection::GetTrueConnection(CONNECTION_TYPE connection,
// Description: The description for this function is not available. Please see the appropriate component owner for a description of this function.
// Return: The return value description for this function is not available.
// Param: Param The parameter description(s) for this function are not available.
// END_HELP_COMMENT
CONNECTION_TYPE COConnection::GetTrueConnection(CONNECTION_TYPE connection,
											    PLATFORM_TYPE platform)
	{
	switch(connection)
		{
		case CONNECTION_USER:
			return (CONNECTION_TYPE)GetUserConnections(platform);
		case CONNECTION_CURRENT:
			return GetConnectionCore();
		default:
			return connection;
		}
	}


// BEGIN_HELP_COMMENT
// Function: PLATFORM_TYPE COConnection::GetPlatformCore(void)
// Description: The description for this function is not available. Please see the appropriate component owner for a description of this function.
// Return: The return value description for this function is not available.
// Param: Param The parameter description(s) for this function are not available.
// END_HELP_COMMENT
PLATFORM_TYPE COConnection::GetPlatformCore(void)

	{
	CString current_platform_str = m_uicon.GetPlatform();

	if(current_platform_str == GetLocString(platform_id[GetPowerOfTwo(PLATFORM_WIN32_X86)]))
		return PLATFORM_WIN32_X86;
	if(current_platform_str == GetLocString(platform_id[GetPowerOfTwo(PLATFORM_MAC_68K)]))
		return PLATFORM_MAC_68K;
	if(current_platform_str == GetLocString(platform_id[GetPowerOfTwo(PLATFORM_MAC_PPC)]))
		return PLATFORM_MAC_PPC;
	if(current_platform_str == GetLocString(platform_id[GetPowerOfTwo(PLATFORM_XBOX)]))
		return PLATFORM_XBOX;

	EXPECT(0);
	return (PLATFORM_TYPE)0;
	}


// BEGIN_HELP_COMMENT
// Function: CONNECTION_TYPE COConnection::GetConnectionCore(void)
// Description: The description for this function is not available. Please see the appropriate component owner for a description of this function.
// Return: The return value description for this function is not available.
// Param: Param The parameter description(s) for this function are not available.
// END_HELP_COMMENT
CONNECTION_TYPE COConnection::GetConnectionCore(void)

	{
	CString current_connection_str = m_uicon.GetConnection();

	if(current_connection_str == 
		GetLocString(connection_id[GetPowerOfTwo(PLATFORM_WIN32_X86)]
					  [GetPowerOfTwo(CONNECTION_LOCAL)]))
		return CONNECTION_LOCAL;

	if(current_connection_str == 
	   GetLocString(connection_id[GetPowerOfTwo(PLATFORM_WIN32_X86)]
					 [GetPowerOfTwo(CONNECTION_SERIAL)]))
		return CONNECTION_SERIAL;

	if(current_connection_str == 
	   GetLocString(connection_id[GetPowerOfTwo(PLATFORM_MAC_68K)]
					 [GetPowerOfTwo(CONNECTION_APPLETALK)]))
		return CONNECTION_APPLETALK;

	if(current_connection_str == 
	   GetLocString(connection_id[GetPowerOfTwo(PLATFORM_MAC_PPC)]
					 [GetPowerOfTwo(CONNECTION_APPLETALK)]))
		return CONNECTION_APPLETALK;
	
	if(current_connection_str == 
	   GetLocString(connection_id[GetPowerOfTwo(PLATFORM_WIN32_X86)]
					 [GetPowerOfTwo(CONNECTION_TCP_IP)]))
		return CONNECTION_TCP_IP;
	
	if(current_connection_str == 
	   GetLocString(connection_id[GetPowerOfTwo(PLATFORM_MAC_68K)]
					 [GetPowerOfTwo(CONNECTION_TCP_IP)]))
		return CONNECTION_TCP_IP;
	
	if(current_connection_str == 
	   GetLocString(connection_id[GetPowerOfTwo(PLATFORM_MAC_PPC)]
					 [GetPowerOfTwo(CONNECTION_TCP_IP)]))
		return CONNECTION_TCP_IP;
	
	if(current_connection_str == 
	   GetLocString(connection_id[GetPowerOfTwo(PLATFORM_XBOX)]
					 [GetPowerOfTwo(CONNECTION_TCP_IP)]))
		return CONNECTION_TCP_IP;
	
	EXPECT(0);
	return (CONNECTION_TYPE)0;
	}


// BEGIN_HELP_COMMENT
// Function: PLATFORM_TYPE COConnection::GetPlatform(void)
// Description: The description for this function is not available. Please see the appropriate component owner for a description of this function.
// Return: The return value description for this function is not available.
// Param: Param The parameter description(s) for this function are not available.
// END_HELP_COMMENT
PLATFORM_TYPE COConnection::GetPlatform(void)
	{
	EXPECT(OpenConnectionDlg() == ERROR_SUCCESS);
	PLATFORM_TYPE current_platform = GetPlatformCore();
	EXPECT(CloseAllDlgs() == ERROR_SUCCESS);
	return current_platform;
	}


// BEGIN_HELP_COMMENT
// Function: CONNECTION_TYPE COConnection::GetConnection(PLATFORM_TYPE platform /* PLATFORM_CURRENT */)
// Description: The description for this function is not available. Please see the appropriate component owner for a description of this function.
// Return: The return value description for this function is not available.
// Param: Param The parameter description(s) for this function are not available.
// END_HELP_COMMENT
CONNECTION_TYPE COConnection::GetConnection(PLATFORM_TYPE platform /* PLATFORM_CURRENT */)

	{
	EXPECT(OpenConnectionDlg() == ERROR_SUCCESS);

	if(platform != PLATFORM_CURRENT)
		EXPECT(SetPlatformCore(platform) == ERROR_SUCCESS);

	CONNECTION_TYPE current_connection = GetConnectionCore();
	EXPECT(CloseAllDlgs() == ERROR_SUCCESS);
	return current_connection;
	}


// BEGIN_HELP_COMMENT
// Function: int COConnection::GetUserPlatforms(void)
// Description: The description for this function is not available. Please see the appropriate component owner for a description of this function.
// Return: The return value description for this function is not available.
// Param: Param The parameter description(s) for this function are not available.
// END_HELP_COMMENT
int COConnection::GetUserPlatforms(void)

	{
	return m_platform;
	}


// BEGIN_HELP_COMMENT
// Function: int COConnection::GetUserConnections(PLATFORM_TYPE platform /* PLATFORM_USER */)
// Description: The description for this function is not available. Please see the appropriate component owner for a description of this function.
// Return: The return value description for this function is not available.
// Param: Param The parameter description(s) for this function are not available.
// END_HELP_COMMENT
int COConnection::GetUserConnections(PLATFORM_TYPE platform /* PLATFORM_USER */)
	{
	return m_connection;
	}


// BEGIN_HELP_COMMENT
// Function: CString COConnection::GetUserRemoteLocalPath(PLATFORM_TYPE platform 
// Description: The description for this function is not available. Please see the appropriate component owner for a description of this function.
// Return: The return value description for this function is not available.
// Param: Param The parameter description(s) for this function are not available.
// END_HELP_COMMENT
CString COConnection::GetUserRemoteLocalPath(PLATFORM_TYPE platform /* PLATFORM_USER */)
	{
	return m_local_path;
	}


// BEGIN_HELP_COMMENT
// Function: CString COConnection::GetUserRemoteSharePath(PLATFORM_TYPE platform
// Description: The description for this function is not available. Please see the appropriate component owner for a description of this function.
// Return: The return value description for this function is not available.
// Param: Param The parameter description(s) for this function are not available.
// END_HELP_COMMENT
CString COConnection::GetUserRemoteSharePath(PLATFORM_TYPE platform /* PLATFORM_USER */)
	{
	return m_share_path;
	}

