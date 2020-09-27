/*==========================================================================
 *
 *  Copyright (C) 1999 Microsoft Corporation.  All Rights Reserved.
 *
 *  File:		dvdptransport.h
 *  Content:	Definition of base class for Transport --> DirectXVoice
 *
 *  History:
 *   Date		By		Reason
 *   ====		==		======
 * 07/06/99		rodtoll	Created It
 * 09/01/2000  georgioc started rewrite/port to xbox 
 *
 ***************************************************************************/

#ifndef __DVENGINE_H
#define __DVENGINE_H

#include "dvntos.h"
#include "dvoicep.h"
#include "dvtran.h"

// CDirectVoiceEngine
//
// This class is the base interface for DirectVoiceClientEngine and 
// DirectVoiceServerEngine.  This interface is used by DirectPlay/
// DirectNet to inform the DirectXVoice engine of new events.  
//
// Hooks are placed into DirectPlay that call these functions.
//
class CDirectVoiceEngine
{
public:
public: // Incoming messages
	virtual BOOL ReceiveSpeechMessage( DVID dvidSource, LPVOID lpMessage, DWORD dwSize ) = 0;

public: // Session Management
    virtual HRESULT StartTransportSession() = 0;
	virtual HRESULT StopTransportSession() = 0;

public: // Player information
	virtual HRESULT AddPlayer( DVID dvID ) = 0;
	virtual HRESULT RemovePlayer( DVID dvID ) = 0;

public: // Used by local voice server to hook player messages to send
	    // to the remote voice server
	virtual HRESULT Initialize( CDirectVoiceTransport *lpTransport, LPDVMESSAGEHANDLER lpdvHandler, LPVOID lpUserContext, LPDWORD lpdwMessages, DWORD dwNumElements ) = 0;
	virtual HRESULT CreateGroup( DVID dvID ) = 0;
	virtual HRESULT DeleteGroup( DVID dvID ) = 0;
	virtual HRESULT AddPlayerToGroup( DVID dvidGroup, DVID dvidPlayer ) = 0;
	virtual HRESULT RemovePlayerFromGroup( DVID dvidGroup, DVID dvidPlayer ) = 0;
	virtual HRESULT MigrateHost( DVID dvidNewHost, LPDIRECTPLAYVOICESERVER lpdvServer ) = 0;
	virtual HRESULT SendComplete( PDVEVENTMSG_SENDCOMPLETE pSendComplete ) = 0;

public: // Compression Information Storage
	static HRESULT Startup();
    static DWORD s_dwCompressedFrameSize;           // default compressor values
    static DWORD s_dwUnCompressedFrameSize;         // should be the same for both server and client
	static LPWAVEFORMATEX s_lpwfxPrimaryFormat;		// Primary buffer format
	
protected:

	static DWORD s_dwDefaultSensitivity;			// Default system sensitivity

};

#endif
