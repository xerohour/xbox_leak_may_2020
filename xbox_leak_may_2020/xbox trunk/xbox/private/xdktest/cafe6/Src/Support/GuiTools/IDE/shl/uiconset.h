///////////////////////////////////////////////////////////////////////////////
//  UICONSET.H
//
//  Created by :            Date :
//      MichMa                  8/25/94
//
//  Description :
//      Declaration of the UIConnectionSettings class
//

#ifndef __UICONSET_H__
#define __UICONSET_H__

#include "..\..\udialog.h"

#include "shlxprt.h"

#define ERROR_ERROR -1

///////////////////////////////////////////////////////////////////////////////
//  UIConnectionSettings class

typedef enum {CONNECTION_LOCAL	   = 0x00000001,
			  CONNECTION_SERIAL	   = 0x00000002,
			  CONNECTION_APPLETALK = 0x00000004,
			  CONNECTION_TCP_IP	   = 0x00000008,
			  // add any new connection types here
			  CONNECTION_CURRENT  = 0x20000000,
			  CONNECTION_USER	  = 0x40000000,
			  CONNECTION_NON_USER = 0x80000000} CONNECTION_TYPE;

// BEGIN_CLASS_HELP
// ClassName: UIConnectionSettings
// BaseClass: UIDialog
// Category: Shell
// END_CLASS_HELP
class SHL_CLASS UIConnectionSettings : public UIDialog

	{
	UIWND_COPY_CTOR(UIConnectionSettings, UIDialog);

	// Utilities
	public:
		
		int SetZone(LPCSTR zone);
		int SetMachineName(LPCSTR name);
   		int SetConnector(LPCSTR connector);
		int SetBaudRate(LPCSTR baud_rate);
		int SetDataBits(LPCSTR data_bits);
		int SetParity(LPCSTR parity);
		int SetStopBits(LPCSTR stop_bits);
		int SetFlowControl(LPCSTR flow_control);
  		int SetIPAddress(LPCSTR ip_address);
		int SetPassword(LPCSTR password, CONNECTION_TYPE connection);

		HWND Close(void);
	};

#endif // __UICONSET_H__
