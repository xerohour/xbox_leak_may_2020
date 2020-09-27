///////////////////////////////////////////////////////////////////////////////
//  COCONNEC.H
//
//  Created by :            Date :
//      MichMa                  9/16/94
//
//  Description :
//      Declaration of the COConnection class
//

#ifndef __COCONNEC_H__
#define __COCONNEC_H__

#include "uioptdlg.h"
#include "uiconnec.h"
#include "uiconset.h"
#include <settings.h>
#include "shlxprt.h"

///////////////////////////////////////////////////////////////////////////////
//  COConnection class

#define ZONE_USER 		  NULL
#define MACHINE_NAME_USER NULL
#define PASSWORD_USER  	  NULL
#define IP_ADDRESS_USER	  NULL

typedef enum {CONNECTOR_COM1, CONNECTOR_COM2, CONNECTOR_COM3, 
			  CONNECTOR_COM4, CONNECTOR_USER, CONNECTOR_NON_USER} 
			  CONNECTOR_TYPE;

typedef enum {BAUD_RATE_300,   BAUD_RATE_600,   BAUD_RATE_1200, BAUD_RATE_1800,
			  BAUD_RATE_2400,  BAUD_RATE_4800,  BAUD_RATE_7200, BAUD_RATE_9600,
			  BAUD_RATE_14400, BAUD_RATE_19200, BAUD_RATE_38400, 
			  BAUD_RATE_57600, BAUD_RATE_USER,  BAUD_RATE_NON_USER} 
			  BAUD_RATE_TYPE;

typedef enum {DATA_BITS_5, DATA_BITS_6, DATA_BITS_7, DATA_BITS_8,
			  DATA_BITS_USER, DATA_BITS_NON_USER} DATA_BITS_TYPE;

#undef PARITY_NONE		// review: where the hell are these
#undef PARITY_ODD		// being previously defined?
#undef PARITY_EVEN 
#undef PARITY_USER

typedef enum {PARITY_NONE, PARITY_ODD, PARITY_EVEN, 
			  PARITY_USER, PARITY_NON_USER} PARITY_TYPE;

typedef enum {STOP_BITS_1, STOP_BITS_1_PT_5, STOP_BITS_2,
			  STOP_BITS_USER, STOP_BITS_NON_USER} STOP_BITS_TYPE;

typedef enum {FLOW_CONTROL_HARDWARE, FLOW_CONTROL_XONXOFF, 
			  FLOW_CONTROL_USER, FLOW_CONTROL_NON_USER} FLOW_CONTROL_TYPE;

typedef enum {CONNEC_TARGET_PLATFORMS, CONNEC_CONNECTIONS,
			  CONNEC_ZONE, CONNEC_MACHINE_NAME, CONNEC_PASSWORD,
			  CONNEC_CONNECTOR, CONNEC_BAUD_RATE, CONNEC_DATA_BITS,
			  CONNEC_PARITY, CONNEC_STOP_BITS, CONNEC_FLOW_CONTROL,
			  CONNEC_REMOTE_LOCAL_PATH, CONNEC_REMOTE_SHARE_PATH,
			  CONNEC_IP_ADDRESS} 
			  CONNEC_INFO_TYPE;

typedef enum {RETRIEVAL_METHOD_REGISTRY, RETRIEVAL_METHOD_CMD_LINE,
			  RETRIEVAL_METHOD_DEFAULT} RETRIEVAL_METHOD_TYPE;

// BEGIN_CLASS_HELP
// ClassName: COConnection
// BaseClass: none
// Category: Shell
// END_CLASS_HELP
class SHL_CLASS COConnection

	{
	public:
		COConnection(PLATFORM_TYPE platform, CSettings **psettings);
	// Data
	private:

		UIConnection m_uicon;
		UIConnectionSettings m_uiconset;
		CSettings *m_pConnecSettings;
		PLATFORM_TYPE m_platform;
		CONNECTION_TYPE m_connection;
		CString m_local_path, m_share_path;

	// Internal Opertaions:
	private:

		int OpenSettingsDlg(PLATFORM_TYPE platform,	
							CONNECTION_TYPE connection);

		int OpenConnectionDlg(void);
		int CloseAllDlgs(void);

		int SetAllSerialCore(PLATFORM_TYPE platform, CONNECTOR_TYPE connector);
		int SetPlatformCore(PLATFORM_TYPE platform);

		int SetConnectionCore(CONNECTION_TYPE connection, 
							  PLATFORM_TYPE platform);

		int SetZoneCore(LPCSTR zone, PLATFORM_TYPE platform);
		int SetMachineNameCore(LPCSTR machine_name, PLATFORM_TYPE platform);

		int SetPasswordCore(LPCSTR password, CONNECTION_TYPE connection,
						    PLATFORM_TYPE platform);

		int SetConnectorCore(CONNECTOR_TYPE connector, PLATFORM_TYPE platform);
		int SetBaudRateCore(BAUD_RATE_TYPE baud_rate, PLATFORM_TYPE platform);
		int SetDataBitsCore(DATA_BITS_TYPE data_bits, PLATFORM_TYPE platform);
		int SetParityCore(PARITY_TYPE parity, PLATFORM_TYPE platform);
		int SetStopBitsCore(STOP_BITS_TYPE stop_bits, PLATFORM_TYPE platform);
		
		int SetFlowControlCore(FLOW_CONTROL_TYPE flow_control, 
							   PLATFORM_TYPE platform);

		int SetIPAddressCore(LPCSTR ip_address, PLATFORM_TYPE platform);

		PLATFORM_TYPE GetTruePlatform(PLATFORM_TYPE platform);

		CONNECTION_TYPE GetTrueConnection(CONNECTION_TYPE connection, 
										  PLATFORM_TYPE platform);
		
		PLATFORM_TYPE GetPlatformCore(void);
		CONNECTION_TYPE GetConnectionCore(void);

	// Utilities
	public:

		int SetPlatform(PLATFORM_TYPE platform = PLATFORM_USER);

		int SetConnection(CONNECTION_TYPE connection = CONNECTION_USER,
						  PLATFORM_TYPE platform = PLATFORM_USER);

		int SetZone(LPCSTR zone = ZONE_USER,
					PLATFORM_TYPE platform = PLATFORM_USER);

		int SetMachineName(LPCSTR machine_name = MACHINE_NAME_USER,
						   PLATFORM_TYPE platform = PLATFORM_USER);

		int SetPassword(LPCSTR password = PASSWORD_USER,
						CONNECTION_TYPE connection = CONNECTION_USER,
						PLATFORM_TYPE platform = PLATFORM_USER);

		int SetConnector(CONNECTOR_TYPE connector = CONNECTOR_USER,
						 PLATFORM_TYPE platform = PLATFORM_USER);

		int SetBaudRate(BAUD_RATE_TYPE baud_rate = BAUD_RATE_USER,
						PLATFORM_TYPE platform = PLATFORM_USER);

		int SetDataBits(DATA_BITS_TYPE data_bits = DATA_BITS_USER,
						PLATFORM_TYPE platform = PLATFORM_USER);

		int SetParity(PARITY_TYPE parity = PARITY_USER,
					  PLATFORM_TYPE platform = PLATFORM_USER);

		int SetStopBits(STOP_BITS_TYPE stop_bits = STOP_BITS_USER,
						PLATFORM_TYPE platform = PLATFORM_USER);

		int SetFlowControl(FLOW_CONTROL_TYPE flow_control = FLOW_CONTROL_USER,
						   PLATFORM_TYPE platform = PLATFORM_USER);

		int SetIPAddress(LPCSTR	ip_address = IP_ADDRESS_USER,
						 PLATFORM_TYPE platform = PLATFORM_USER);

		int SetAll(PLATFORM_TYPE platform = PLATFORM_USER,
				  CONNECTION_TYPE connection = CONNECTION_USER);

		int SetAll(PLATFORM_TYPE platform, CONNECTION_TYPE connection,
				   LPCSTR zone, LPCSTR machine_name, LPCSTR password);

		int SetAll(PLATFORM_TYPE platform, CONNECTION_TYPE connection,
				   CONNECTOR_TYPE connector, BAUD_RATE_TYPE baud_rate, 
				   DATA_BITS_TYPE data_bits, PARITY_TYPE parity,
				   STOP_BITS_TYPE stop_bits);

		int SetAll(PLATFORM_TYPE platform, CONNECTION_TYPE connection,
				   CONNECTOR_TYPE connector);

		int SetAll(PLATFORM_TYPE platform, CONNECTION_TYPE connection,
				   CONNECTOR_TYPE connector, BAUD_RATE_TYPE baud_rate,
				   FLOW_CONTROL_TYPE flow_control);

		int SetAll(PLATFORM_TYPE platform, CONNECTION_TYPE connection,
				   LPCSTR ip_address, LPCSTR password);

		PLATFORM_TYPE GetPlatform(void);

		CONNECTION_TYPE GetConnection(PLATFORM_TYPE platform = 
									  PLATFORM_CURRENT);

		int GetUserPlatforms(void);
		int GetUserConnections(PLATFORM_TYPE platform = PLATFORM_USER);
		CString GetUserRemoteLocalPath(PLATFORM_TYPE platform = PLATFORM_USER);
		CString GetUserRemoteSharePath(PLATFORM_TYPE platform = PLATFORM_USER);
	};

extern SHL_DATA COConnection *gpConnec;

#endif // __COCONNEC_H__
