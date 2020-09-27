// FILE:       library\hardware\audio\generic\auddevs.h
// AUTHOR:		S. Herr
// COPYRIGHT:	(c) 1996 Viona Development GmbH.  All Rights Reserved.
// CREATED:		24.01.1996
//
// PURPOSE: 	Base Classes for Windows Sound System Device Drivers
//
// HISTORY:
//
//

#ifndef AUDDEVS_H
#define AUDDEVS_H

#include "library\common\prelude.h"
#include "library\general\lists.h"

///////////////////////////////////////////////////////////////////////////////
// Audio Device Base Class
///////////////////////////////////////////////////////////////////////////////

class ClientList;

class AudioDevice {
	protected:
		ClientList	*	clients;
		
		DWORD				enableCount;
		BOOL				enabled;

		HINSTANCE		instance;		
	public:
		AudioDevice(HINSTANCE instance);
		virtual ~AudioDevice();
		
		virtual DWORD Enable(void);
		virtual DWORD Disable(void);
	};


///////////////////////////////////////////////////////////////////////////////
// Device Client Base Class
///////////////////////////////////////////////////////////////////////////////

class DeviceClient : public Node {
	public:
		AudioDevice	*		owner;					// Audio Device of which this node is client. 
															// Initialized with NULL when created.
	
		DWORD					openFlags;                                     
		DWORD					procCallback;			// function callback
#if WIN32
		DWORD					callbackType;
#else
		UINT					callbackType;			// callback
#endif
		DWORD					instance;				// app's private instance information
//	PHARDWAREINSTANCE	phwi;							// speicher später die Hardware-Instanz... PHARDWAREINSTANCE muss noch als Klasse
															// definiert werden.

		DeviceClient() {owner = NULL;}

#if WIN32	
		virtual void Notify(DWORD message, DWORD param) = 0;
#else
		virtual void Notify(WORD message, DWORD param) = 0;
#endif
	};

///////////////////////////////////////////////////////////////////////////////
// Device Client List Class
///////////////////////////////////////////////////////////////////////////////

class ClientList : public List {
	public:
		~ClientList();
		DeviceClient* FindClient(DeviceClient* client);	// Looks for the given client. Returns FALSE when not in list.
		
#if WIN32	
		virtual void NotifyClients(DWORD message, DWORD param);		
#else
		virtual void NotifyClients(WORD message, DWORD param);		
#endif
	};


#endif

