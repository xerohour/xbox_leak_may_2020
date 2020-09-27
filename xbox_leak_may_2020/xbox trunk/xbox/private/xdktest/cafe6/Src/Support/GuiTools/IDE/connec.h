#ifndef CONNEC_H
#define CONNEC_H

/*******************************************************************************

Include files

********************************************************************************/

#include <afxwin.h>
#include "settings.h"
#include "shl\coconnec.h"
#include "shl\wbutil.h"
#include "shl\uiconset.h"


/*******************************************************************************

Declaration of CConnections class

********************************************************************************/

class CConnections
	
	{		
	public:
		
		// entry point into connection settings feature
		int Connections(HWND hwndOwner, CSettings** pConnecSettings, PLATFORM_TYPE platform);

		// text-to-int conversion routines (called by ide targets during init)
		CONNECTION_TYPE ConnectionTextToInt(CString connection_txt);
		CONNECTOR_TYPE ConnectorTextToInt(CString connector_txt);
		BAUD_RATE_TYPE BaudRateTextToInt(CString baud_rate_txt);
		FLOW_CONTROL_TYPE FlowControlTextToInt(CString flow_control_txt);
		DATA_BITS_TYPE DataBitsTextToInt(CString data_bits_txt);
		STOP_BITS_TYPE StopBitsTextToInt(CString stop_bits_txt);
		PARITY_TYPE ParityTextToInt(CString parity_txt);
	};

// window procedures for settings dlgs
BOOL CALLBACK ConnectionDlgProc(HWND hwndDlg, UINT msg,	WPARAM wParam, LPARAM lParam);
BOOL CALLBACK Win32SerialDlgProc(HWND hwndDlg, UINT msg, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK PMacSerialDlgProc(HWND hwndDlg, UINT msg, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK Mac68KSerialDlgProc(HWND hwndDlg, UINT msg, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK IPDlgProc(HWND hwndDlg, UINT msg,	WPARAM wParam, LPARAM lParam);
BOOL CALLBACK ATDlgProc(HWND hwndDlg, UINT msg,	WPARAM wParam, LPARAM lParam);
		
#endif