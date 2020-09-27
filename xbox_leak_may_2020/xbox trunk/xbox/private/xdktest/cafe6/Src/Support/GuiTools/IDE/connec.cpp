/*******************************************************************************

Include files

********************************************************************************/

#include "stdafx.h"
#include "connec.h"
#include "resource.h"
#include "..\testutil.h"

#define new DEBUG_NEW

/*******************************************************************************

Global data

********************************************************************************/

CSettings **gConnecSettings;	// connections settings objects for current platform
PLATFORM_TYPE gplatform;		// current platform
HINSTANCE ghinst;				// handle to instance of cafe driver

CString	platform_str[4] = {"Win32", "Win32s", "68k Macintosh", "Power Macintosh"};

CString	connection_str[6][4] = {"Local", "Serial", "", "Network (TCP/IP)",            
								"", "Serial", "", "",                                                  
								"", "Serial", "Network (AppleTalk)", "Network (TCP/IP)",               
								"", "Serial", "Network (AppleTalk)", "Network (TCP/IP)"};
	 
CString baud_rate_str[] = {"300", "600", "1200", "1800", "2400", "4800", "7200",
						   "9600", "14400", "19200", "38400", "57600"};

CString connector_str[] = {"COM1", "COM2", "COM3", "COM4"};  //todo: localize
CString data_bits_str[] = {"5", "6", "7", "8"};                              
CString stop_bits_str[] = {"1", "1.5", "2"};                         
CString parity_str[]    = {"NONE", "ODD", "EVEN"};                           

int connection_to_index[6][4] = {0, 1, -1, 2,
								 -1, 0, -1, -1,
								 -1, 0, 1, 2,
								 -1, 0, 1, 2};

int	index_to_connection[6][4] = {CONNECTION_LOCAL, CONNECTION_SERIAL, CONNECTION_TCP_IP, -1,
								 CONNECTION_SERIAL, -1, -1, -1,
								 CONNECTION_SERIAL, CONNECTION_APPLETALK, CONNECTION_TCP_IP, -1,
								 CONNECTION_SERIAL, CONNECTION_APPLETALK, CONNECTION_TCP_IP, -1}; 


/*******************************************************************************

Entry point to connection settings feature

********************************************************************************/

int CConnections::Connections(HWND hwndOwner, CSettings **pSettings, PLATFORM_TYPE platform)
	{
	gConnecSettings = pSettings;
	gplatform = platform;
	ghinst = (HINSTANCE)GetWindowLong(hwndOwner, GWL_HINSTANCE); 
	return DialogBox(ghinst, MAKEINTRESOURCE(IDD_DIALOG_CONNECTION), hwndOwner, ConnectionDlgProc);
	}


/*******************************************************************************

Text to int conversion routines

********************************************************************************/

CONNECTION_TYPE CConnections::ConnectionTextToInt(CString connection_txt)
	
	{
	connection_txt.MakeLower();

	if(connection_txt == "local")             
		return CONNECTION_LOCAL;
	if(connection_txt == "serial")             
		return CONNECTION_SERIAL;
	if(connection_txt == "tcp_ip")             
		return CONNECTION_TCP_IP;
	if(connection_txt == "appletalk")             
		return CONNECTION_APPLETALK;

	// todo: print error
	return CONNECTION_LOCAL;
	}


CONNECTOR_TYPE CConnections::ConnectorTextToInt(CString connector_txt)
	
	{
	connector_txt.MakeLower();

	if(connector_txt == "com1")             
		return CONNECTOR_COM1;
	if(connector_txt == "com2")             
		return CONNECTOR_COM2;
	if(connector_txt == "com3")             
		return CONNECTOR_COM3;
	if(connector_txt == "com4")             
		return CONNECTOR_COM4;

	// todo: print error
	return CONNECTOR_COM1;
	}


BAUD_RATE_TYPE CConnections::BaudRateTextToInt(CString baud_rate_txt)
	
	{
	if(baud_rate_txt == "300")
		return BAUD_RATE_1200;
	if(baud_rate_txt == "600")
		return BAUD_RATE_1200;
	if(baud_rate_txt == "1200")
		return BAUD_RATE_1200;
	if(baud_rate_txt == "1800")
		return BAUD_RATE_1200;
	if(baud_rate_txt == "2400")
		return BAUD_RATE_2400;
	if(baud_rate_txt == "4800")
		return BAUD_RATE_4800;
	if(baud_rate_txt == "7200")
		return BAUD_RATE_4800;
	if(baud_rate_txt == "9600")
		return BAUD_RATE_9600;
	if(baud_rate_txt == "14400")
		return BAUD_RATE_14400;
	if(baud_rate_txt == "19200")
		return BAUD_RATE_19200;
	if(baud_rate_txt == "38400")
		return BAUD_RATE_38400;
	if(baud_rate_txt == "57600")
		return BAUD_RATE_57600;

	// todo: print error
	return BAUD_RATE_57600;
	}


FLOW_CONTROL_TYPE CConnections::FlowControlTextToInt(CString flow_control_txt)

	{
	flow_control_txt.MakeLower();

	if(flow_control_txt == "hardware")      
		return FLOW_CONTROL_HARDWARE;
	if(flow_control_txt == "xonxoff")       
		return FLOW_CONTROL_XONXOFF;

	// todo: print error
	return FLOW_CONTROL_HARDWARE;
	}


DATA_BITS_TYPE CConnections::DataBitsTextToInt(CString data_bits_txt)

	{
	data_bits_txt.MakeLower();

	if(data_bits_txt == "5")
		return DATA_BITS_5;
	if(data_bits_txt == "6")
		return DATA_BITS_6;
	if(data_bits_txt == "7")
		return DATA_BITS_7;
	if(data_bits_txt == "8")
		return DATA_BITS_8;

	// todo: print error
	return DATA_BITS_8;
	}


STOP_BITS_TYPE CConnections::StopBitsTextToInt(CString stop_bits_txt)

	{
	stop_bits_txt.MakeLower();

	if(stop_bits_txt == "1")
		return STOP_BITS_1;
	if(stop_bits_txt == "1.5")
		return STOP_BITS_1_PT_5;
	if(stop_bits_txt == "2")
		return STOP_BITS_2;

	// todo: print error
	return STOP_BITS_1;
	}


PARITY_TYPE CConnections::ParityTextToInt(CString parity_txt)

	{
	parity_txt.MakeLower();

	if(parity_txt == "none")
		return PARITY_NONE;
	if(parity_txt == "even")
		return PARITY_EVEN;
	if(parity_txt == "odd")
		return PARITY_ODD;

	// todo: print error
	return PARITY_NONE;
	}

/*******************************************************************************

Connection dialog window procedure

********************************************************************************/

BOOL CALLBACK ConnectionDlgProc(HWND hwndDlg, UINT msg, WPARAM wParam, LPARAM lParam)

	{
	static HWND hwndListConnection, hwndButtonSettings, hwndEditLocalPath, hwndEditSharePath, hwndStaticSharePath;
	static int current_connection; 

	switch(msg)
		
		{
		case WM_INITDIALOG:
			
			{
			hwndListConnection = GetDlgItem(hwndDlg, IDC_LIST_CONNECTION);
			hwndButtonSettings = GetDlgItem(hwndDlg, IDC_BUTTON_SETTINGS);
			hwndEditLocalPath = GetDlgItem(hwndDlg, IDC_EDIT_LOCAL_PATH);
			hwndEditSharePath = GetDlgItem(hwndDlg, IDC_EDIT_SHARE_PATH);
			hwndStaticSharePath = GetDlgItem(hwndDlg, IDC_STATIC_SHARE_PATH);
			current_connection = gConnecSettings[0]->GetIntValue(settingConnection);

			for(int i = 0; i < 4; i++)
				if(connection_str[GetPowerOfTwo(gplatform)][i] != "")
					SendMessage(hwndListConnection, LB_ADDSTRING, 0, 
								(LPARAM)(LPCTSTR)connection_str[GetPowerOfTwo(gplatform)][i]);
				
			if(current_connection == CONNECTION_LOCAL)
				EnableWindow(hwndButtonSettings, FALSE);
				
			if(gplatform == PLATFORM_WIN32S_X86)
				EnableWindow(hwndListConnection, FALSE);						
					
			SendMessage(hwndListConnection, LB_SETCURSEL, 
						connection_to_index[GetPowerOfTwo(gplatform)][GetPowerOfTwo(current_connection)], 0);

			SetWindowText(hwndEditLocalPath, gConnecSettings[0]->GetTextValue(settingRemoteLocalPath));

			if((gplatform != PLATFORM_MAC_68K) && (gplatform != PLATFORM_MAC_PPC))
				{
				ShowWindow(hwndEditSharePath, SW_SHOW);
				ShowWindow(hwndStaticSharePath, SW_SHOW);
				SetWindowText(hwndEditSharePath, gConnecSettings[0]->GetTextValue(settingRemoteSharePath));
				}
			
			CString title = platform_str[GetPowerOfTwo(gplatform)] + " Connections";
			SetWindowText(hwndDlg, title);
			return TRUE;
			}

		case WM_COMMAND:
			
			switch(LOWORD(wParam))
				
				{
				case IDOK:
					
					char ch_array[256];
					GetWindowText(hwndEditLocalPath, ch_array, 256);
					gConnecSettings[0]->SetTextValue(settingRemoteLocalPath, ch_array);

					if((gplatform != PLATFORM_MAC_68K) && (gplatform != PLATFORM_MAC_PPC))
						{
						GetWindowText(hwndEditSharePath, ch_array, 256);
						gConnecSettings[0]->SetTextValue(settingRemoteSharePath, ch_array);
						}

					gConnecSettings[0]->SetIntValue(settingConnection, current_connection); 
					gConnecSettings[0]->WriteRegistry();
					return SendMessage(hwndDlg, WM_CLOSE, 0, 0);
				
				case IDCANCEL:
					return SendMessage(hwndDlg, WM_CLOSE, 0, 0);
				
				case IDC_BUTTON_SETTINGS:

					switch(current_connection)

						{
						case CONNECTION_TCP_IP:
							return DialogBox(ghinst, MAKEINTRESOURCE(IDD_DIALOG_IP), hwndDlg, IPDlgProc);

						case CONNECTION_APPLETALK:
							return DialogBox(ghinst, MAKEINTRESOURCE(IDD_DIALOG_AT), hwndDlg, ATDlgProc);

						case CONNECTION_SERIAL:

							switch(gplatform)

								{
								case PLATFORM_WIN32_X86:
								case PLATFORM_WIN32S_X86:

									return DialogBox(ghinst, MAKEINTRESOURCE(IDD_DIALOG_WIN32_SERIAL),
													 hwndDlg, Win32SerialDlgProc);

								case PLATFORM_MAC_68K:

									return DialogBox(ghinst, MAKEINTRESOURCE(IDD_DIALOG_68K_SERIAL),
													 hwndDlg, Mac68KSerialDlgProc);
								case PLATFORM_MAC_PPC:

									return DialogBox(ghinst, MAKEINTRESOURCE(IDD_DIALOG_PMAC_SERIAL),
													 hwndDlg, PMacSerialDlgProc);

								default:
									return 0;
								}

						default:
							return 0;
						}

				case IDC_LIST_CONNECTION:
					
					switch(HIWORD(wParam))	
						
						{
						case LBN_SELCHANGE:
																
							current_connection = (CONNECTION_TYPE)index_to_connection[GetPowerOfTwo(gplatform)][SendMessage((HWND)lParam, LB_GETCURSEL, 0, 0)];

							if(current_connection == CONNECTION_LOCAL)
								EnableWindow(hwndButtonSettings, FALSE);						
							else
								EnableWindow(hwndButtonSettings, TRUE);

							return -1;

						default:
							return 0;
						}									
						
				default:
					return 0;
				}

		case WM_CLOSE:
			return EndDialog(hwndDlg, 0);

		default:
			return 0;
		}
	}


/*******************************************************************************

Win32 serial settings dialog window procedure

********************************************************************************/

BOOL CALLBACK Win32SerialDlgProc(HWND hwndDlg, UINT msg, WPARAM wParam, LPARAM lParam)

	{
	static HWND hwndComboConnector, hwndComboBaudRate;

	switch(msg)
		
		{
		case WM_INITDIALOG:
			
			{
			hwndComboConnector = GetDlgItem(hwndDlg, IDC_COMBO_WIN32_CONNECTOR);
			hwndComboBaudRate = GetDlgItem(hwndDlg, IDC_COMBO_BAUD_RATE);

			for(int i = 0; i < 4; i++)
				SendMessage(hwndComboConnector, CB_ADDSTRING, 0, (LPARAM)(LPCTSTR)connector_str[i]);

			for(i = 0; i < 12; i++)
				SendMessage(hwndComboBaudRate, CB_ADDSTRING, 0, (LPARAM)(LPCTSTR)baud_rate_str[i]);

			SendMessage(hwndComboConnector, CB_SETCURSEL, gConnecSettings[2]->GetIntValue(settingConnector), 0);
			SendMessage(hwndComboBaudRate, CB_SETCURSEL, gConnecSettings[2]->GetIntValue(settingBaudRate), 0);

			if(gConnecSettings[2]->GetIntValue(settingFlowControl) == FLOW_CONTROL_HARDWARE)
				CheckRadioButton(hwndDlg, IDC_RADIO_HARDWARE, IDC_RADIO_XONXOFF, IDC_RADIO_HARDWARE);
			else
				CheckRadioButton(hwndDlg, IDC_RADIO_HARDWARE, IDC_RADIO_XONXOFF, IDC_RADIO_XONXOFF);
	
			CString title = platform_str[GetPowerOfTwo(gplatform)] + " Serial Settings";
			SetWindowText(hwndDlg, title);
			return TRUE;
			}

		case WM_COMMAND:
			
			switch(LOWORD(wParam))
				
				{
				case IDOK:

					gConnecSettings[2]->SetIntValue(settingConnector, 
													(CONNECTOR_TYPE)SendMessage(hwndComboConnector, CB_GETCURSEL, 0, 0));

					gConnecSettings[2]->SetIntValue(settingBaudRate, 
													(BAUD_RATE_TYPE)SendMessage(hwndComboBaudRate, CB_GETCURSEL, 0, 0));

					if(SendMessage(GetDlgItem(hwndDlg, IDC_RADIO_HARDWARE), BM_GETCHECK, 0, 0) == 1)
						gConnecSettings[2]->SetIntValue(settingFlowControl, FLOW_CONTROL_HARDWARE);
					else
						gConnecSettings[2]->SetIntValue(settingFlowControl, FLOW_CONTROL_XONXOFF);

					gConnecSettings[2]->WriteRegistry();
					return SendMessage(hwndDlg, WM_CLOSE, 0, 0);
				
				case IDCANCEL:
					return SendMessage(hwndDlg, WM_CLOSE, 0, 0);
				
				default:
					return 0;
				}

		case WM_CLOSE:
			return EndDialog(hwndDlg, 0);

		default:
			return 0;
		}
	}


/*******************************************************************************

PMac serial settings dialog window procedure

********************************************************************************/

BOOL CALLBACK PMacSerialDlgProc(HWND hwndDlg, UINT msg, WPARAM wParam, LPARAM lParam)

	{
	static HWND hwndComboConnector;

	switch(msg)
		
		{
		case WM_INITDIALOG:
			
			{
			hwndComboConnector = GetDlgItem(hwndDlg, IDC_COMBO_PMAC_CONNECTOR);

			for(int i = 0; i < 4; i++)
				SendMessage(hwndComboConnector, CB_ADDSTRING, 0, (LPARAM)(LPCTSTR)connector_str[i]);

			SendMessage(hwndComboConnector, CB_SETCURSEL, gConnecSettings[2]->GetIntValue(settingConnector), 0);
			return TRUE;
			}

		case WM_COMMAND:
			
			switch(LOWORD(wParam))
				
				{
				case IDOK:
					
					gConnecSettings[2]->SetIntValue(settingConnector, 
													(CONNECTOR_TYPE)SendMessage(hwndComboConnector, CB_GETCURSEL, 0, 0));

					gConnecSettings[2]->WriteRegistry();
					return SendMessage(hwndDlg, WM_CLOSE, 0, 0);
				
				case IDCANCEL:
					return SendMessage(hwndDlg, WM_CLOSE, 0, 0);
				default:
					return 0;
				}

		case WM_CLOSE:
			return EndDialog(hwndDlg, 0);

		default:
			return 0;
		}
	}


/*******************************************************************************

68K serial settings dialog window procedure

********************************************************************************/

BOOL CALLBACK Mac68KSerialDlgProc(HWND hwndDlg, UINT msg, WPARAM wParam, LPARAM lParam)

	{
	static HWND hwndComboConnector, hwndComboBaudRate, hwndComboDataBits, hwndComboParity, hwndComboStopBits;

	switch(msg)
		
		{
		case WM_INITDIALOG:
			
			{
			hwndComboConnector = GetDlgItem(hwndDlg, IDC_COMBO_68K_CONNECTOR);
			hwndComboBaudRate = GetDlgItem(hwndDlg, IDC_COMBO_68K_BAUD_RATE);
			hwndComboDataBits = GetDlgItem(hwndDlg, IDC_COMBO_DATA_BITS);
			hwndComboParity = GetDlgItem(hwndDlg, IDC_COMBO_PARITY);
			hwndComboStopBits = GetDlgItem(hwndDlg, IDC_COMBO_STOP_BITS);

			int i;

			for(i = 0; i < 4; i++)
				SendMessage(hwndComboConnector, CB_ADDSTRING, 0, (LPARAM)(LPCTSTR)connector_str[i]);
			for(i = 0; i < 12; i++)
				SendMessage(hwndComboBaudRate, CB_ADDSTRING, 0, (LPARAM)(LPCTSTR)baud_rate_str[i]);
			for(i = 0; i < 4; i++)
				SendMessage(hwndComboDataBits, CB_ADDSTRING, 0, (LPARAM)(LPCTSTR)data_bits_str[i]);
			for(i = 0; i < 3; i++)
				SendMessage(hwndComboParity, CB_ADDSTRING, 0, (LPARAM)(LPCTSTR)parity_str[i]);
			for(i = 0; i < 3; i++)
				SendMessage(hwndComboStopBits, CB_ADDSTRING, 0, (LPARAM)(LPCTSTR)stop_bits_str[i]);

			SendMessage(hwndComboConnector, CB_SETCURSEL, gConnecSettings[2]->GetIntValue(settingConnector), 0);
			SendMessage(hwndComboBaudRate, CB_SETCURSEL, gConnecSettings[2]->GetIntValue(settingBaudRate), 0);
			SendMessage(hwndComboDataBits, CB_SETCURSEL, gConnecSettings[2]->GetIntValue(settingDataBits), 0);
			SendMessage(hwndComboParity, CB_SETCURSEL, gConnecSettings[2]->GetIntValue(settingParity), 0);
			SendMessage(hwndComboStopBits, CB_SETCURSEL, gConnecSettings[2]->GetIntValue(settingStopBits), 0);
			return TRUE;
			}

		case WM_COMMAND:
			
			switch(LOWORD(wParam))
				
				{
				case IDOK:
					
					gConnecSettings[2]->SetIntValue(settingConnector, 
													(CONNECTOR_TYPE)SendMessage(hwndComboConnector, CB_GETCURSEL, 0, 0));
					gConnecSettings[2]->SetIntValue(settingBaudRate, 
													(BAUD_RATE_TYPE)SendMessage(hwndComboBaudRate, CB_GETCURSEL, 0, 0));
					gConnecSettings[2]->SetIntValue(settingDataBits, 
													(DATA_BITS_TYPE)SendMessage(hwndComboDataBits, CB_GETCURSEL, 0, 0));
					gConnecSettings[2]->SetIntValue(settingParity, 
													(PARITY_TYPE)SendMessage(hwndComboParity, CB_GETCURSEL, 0, 0));
					gConnecSettings[2]->SetIntValue(settingStopBits, 
													(STOP_BITS_TYPE)SendMessage(hwndComboStopBits, CB_GETCURSEL, 0, 0));

					gConnecSettings[2]->WriteRegistry();
					return SendMessage(hwndDlg, WM_CLOSE, 0, 0);
				
				case IDCANCEL:
					return SendMessage(hwndDlg, WM_CLOSE, 0, 0);
				default:
					return 0;
				}

		case WM_CLOSE:
			return EndDialog(hwndDlg, 0);

		default:
			return 0;
		}
	}


/*******************************************************************************

IP settings dialog window procedure

********************************************************************************/

BOOL CALLBACK IPDlgProc(HWND hwndDlg, UINT msg, WPARAM wParam, LPARAM lParam)

	{
	static HWND hwndEditName, hwndEditPassword;

	switch(msg)
		{
		case WM_INITDIALOG:
			{
			hwndEditName = GetDlgItem(hwndDlg, IDC_EDIT_IP_NAME);
			hwndEditPassword = GetDlgItem(hwndDlg, IDC_EDIT_IP_PASSWORD);
			SetWindowText(hwndEditName, gConnecSettings[1]->GetTextValue(settingRemoteIPAddress));
			SetWindowText(hwndEditPassword, gConnecSettings[1]->GetTextValue(settingRemoteIPPassword));
			CString title = platform_str[GetPowerOfTwo(gplatform)] + " TCP/IP Settings";
			SetWindowText(hwndDlg, title);
			return TRUE;
			}
		case WM_COMMAND:
			switch(LOWORD(wParam))
				{
				case IDOK:
					char ch_array[256];
					GetWindowText(hwndEditName, ch_array, 256);
					gConnecSettings[1]->SetTextValue(settingRemoteIPAddress, ch_array);
					GetWindowText(hwndEditPassword, ch_array, 256);
					gConnecSettings[1]->SetTextValue(settingRemoteIPPassword, ch_array);
					gConnecSettings[1]->WriteRegistry();
					return SendMessage(hwndDlg, WM_CLOSE, 0, 0);
				case IDCANCEL:
					return SendMessage(hwndDlg, WM_CLOSE, 0, 0);
				default:
					return 0;
				}
		case WM_CLOSE:
			return EndDialog(hwndDlg, 0);
		default:
			return 0;
		}
	}


/*******************************************************************************

AppleTalk settings dialog window procedure

********************************************************************************/

BOOL CALLBACK ATDlgProc(HWND hwndDlg, UINT msg, WPARAM wParam, LPARAM lParam)
	
	{
	static HWND hwndEditName, hwndEditZone, hwndEditPassword;

	switch(msg)
		{
		case WM_INITDIALOG:
			{
			hwndEditName = GetDlgItem(hwndDlg, IDC_EDIT_AT_NAME);
			hwndEditZone = GetDlgItem(hwndDlg, IDC_EDIT_ZONE);
			hwndEditPassword = GetDlgItem(hwndDlg, IDC_EDIT_AT_PASSWORD);
			SetWindowText(hwndEditName, gConnecSettings[3]->GetTextValue(settingRemoteMachName));
			SetWindowText(hwndEditZone, gConnecSettings[3]->GetTextValue(settingRemoteZone));
			SetWindowText(hwndEditPassword, gConnecSettings[3]->GetTextValue(settingRemoteATPassword));
			CString title = platform_str[GetPowerOfTwo(gplatform)] + " AppleTalk Settings";
			SetWindowText(hwndDlg, title);
			return TRUE;
			}
		case WM_COMMAND:
			switch(LOWORD(wParam))
				{
				case IDOK:
					char ch_array[256];
					GetWindowText(hwndEditName, ch_array, 256);
					gConnecSettings[3]->SetTextValue(settingRemoteMachName, ch_array);
					GetWindowText(hwndEditZone, ch_array, 256);
					gConnecSettings[3]->SetTextValue(settingRemoteZone, ch_array);
					GetWindowText(hwndEditPassword, ch_array, 256);
					gConnecSettings[3]->SetTextValue(settingRemoteATPassword, ch_array);
					gConnecSettings[3]->WriteRegistry();
					return SendMessage(hwndDlg, WM_CLOSE, 0, 0);
				case IDCANCEL:
					return SendMessage(hwndDlg, WM_CLOSE, 0, 0);
				default:
					return 0;
				}
		case WM_CLOSE:
			return EndDialog(hwndDlg, 0);
		default:
			return 0;
		}
	}



