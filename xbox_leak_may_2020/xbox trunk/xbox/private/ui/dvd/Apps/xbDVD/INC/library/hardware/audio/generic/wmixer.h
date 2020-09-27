// FILE:       library\hardware\audio\generic\wmixer.h
// AUTHOR:		S. Herr
// COPYRIGHT:	(c) 1995 Viona Development GmbH.  All Rights Reserved.
// CREATED:		22.01.1996
//
// PURPOSE: 	Base class for Windows Sound System compatible Mixers
//
// HISTORY:

#define WINVER 0x0400

#ifndef WMIXER_H
#define WMIXER_H

#include "windows.h"
#include "mmsystem.h"
#include "mmddk.h"

#include "..\common\prelude.h"
#include "..\general\lists.h"
#include "..\common\gnerrors.h"

class MixerClient	: public Node {
	public:
		DWORD					openFlags;                                     
		HMIXER				mixerHandle;			// handle that will be used
		DWORD					procCallback;			// function callback
		UINT					callbackType;			// callback
		DWORD					instance;				// app's private instance information
//	PHARDWAREINSTANCE	phwi;							// speicher später die Hardware-Instanz... PHARDWAREINSTANCE muss noch als Klasse
															// definiert werden.
	};

class MixerClientList : public List {
	public:
		~MixerClientList();

		MixerClient* FindClient(MixerClient* client);
		void NotifyClients(DWORD controlID);
	};

class WindowsMixer {
	private:
		MixerClientList	*	clients;
		
		UnitSet	busterUnits;
		
		DWORD		enableCount;
		BOOL		enabled;
	protected:
		DWORD ctrlValues[MAXCONTROLS][MAXCHANNELS];
	
		DWORD GetDestLineInfo(LPMIXERLINE mixerLine, DWORD destination);
		DWORD GetSourceLineInfo(LPMIXERLINE mixerLine, DWORD source);
		BOOL	RetrieveINIValues(void);
		BOOL	StoreINIValues(void);
	public:
		WindowsMixer();
		~WindowsMixer();

		DWORD Enable(void);
		DWORD Disable(void);
		DWORD GetNumDevs(void);
		DWORD Open(UINT devID, DWORD __far * user, LPMIXEROPENDESC openDesc, DWORD openFlags);
		DWORD GetDevCaps(MIXERCAPS __far * mc);	// für PnP: MDEVICECAPSEX-Struktur!
		DWORD Close(MixerClient* client);
		DWORD GetLineInfo(MixerClient* client, LPMIXERLINE mixerLine, DWORD flags);
		DWORD GetLineControls(MixerClient* client, LPMIXERLINECONTROLS mlCtrls, DWORD flags);
		DWORD GetControlDetails(LPMIXERCONTROLDETAILS mCtrlDetails, DWORD flags);
		DWORD SetControlDetails(LPMIXERCONTROLDETAILS mCtrlDetails, DWORD flags);
	};

#endif

