/*==========================================================================
 *
 *  Copyright (C) 1999 Microsoft Corporation.  All Rights Reserved.
 *
 *  File:		dvtransport.h
 *  Content:	Base class for dp/dnet abstraction
 *
 *  History:
 *   Date		By		Reason
 *   ====		==		======
 * 07/06/99		rodtoll	Created It
 *
 ***************************************************************************/

#ifndef __DVTRANSPORT_H
#define __DVTRANSPORT_H

class CDirectVoiceEngine;
class CVoicePlayer;
class CDirectVoiceTransport;

struct DIRECTVOICEOBJECT;

#include "dvshared.h"

// CDirectVoiceTransport
//
// Abstracts the transport system so that the sends and group management 
// features of DPlay/DirectNet are indpendent.
class CDirectVoiceTransport 
{
// Voice player table management
public:
	CDirectVoiceTransport(): m_lRefCount(0) {};
	virtual ~CDirectVoiceTransport() {};

	inline void Release() { InterlockedDecrement( &m_lRefCount ); };
	inline void AddRef() { InterlockedIncrement( &m_lRefCount ); };

	virtual HRESULT AddPlayerEntry( DVID dvidPlayer, LPVOID lpData ) = 0;
	virtual HRESULT DeletePlayerEntry( DVID dvidPlayer ) = 0;
	virtual HRESULT GetPlayerEntry( DVID dvidPlayer, CVoicePlayer **lplpPlayer ) = 0;
	virtual HRESULT Initialize() = 0;
	virtual HRESULT MigrateHost( DVID dvidNewHost ) = 0;

	virtual DVID GetLocalID() = 0;
	virtual DVID GetServerID() = 0;

public:
	virtual HRESULT SendToServer( PDVTRANSPORT_BUFFERDESC pBufferDesc, LPVOID pvContext, DWORD dwFlags ) = 0;
	virtual HRESULT SendToAll( PDVTRANSPORT_BUFFERDESC pBufferDesc, LPVOID pvContext, DWORD dwFlags ) = 0;
	virtual HRESULT SendToIDS( PDVID pdvidTargets, DWORD dwNumTargets, PDVTRANSPORT_BUFFERDESC pBufferDesc, LPVOID pvContext, DWORD dwFlags ) = 0;

	virtual DWORD GetMaxPlayers( )= 0;

public: // Remote Server Synchronization functions
	virtual HRESULT CreateGroup( LPDVID dvidGroup ) = 0;
	virtual HRESULT DeleteGroup( DVID dvidGroup ) = 0;
	virtual HRESULT AddPlayerToGroup( LPDVID dvidGroup, DVID dvidPlayer ) = 0; 
	virtual HRESULT RemovePlayerFromGroup( DVID dvidGroup, DVID dvidPlayer ) = 0;

public: // Hooks into the transport

    virtual VOID DoWork() = 0;
	virtual BOOL IsPlayerInGroup( DVID dvidGroup, DVID dvidPlayer ) = 0;
	virtual BOOL ConfirmValidEntity( DVID dvid ) = 0;
	virtual BOOL ConfirmValidGroup( DVID dvid ) = 0;
	virtual HRESULT EnableReceiveHook( DIRECTVOICEOBJECT *dvObject, DWORD dwObjectType ) = 0;
	virtual HRESULT DisableReceiveHook( ) = 0;
	virtual HRESULT WaitForDetachCompletion() = 0;
	virtual void DestroyTransport() = 0;
	virtual BOOL ConfirmLocalHost( ) = 0;
	virtual BOOL ConfirmSessionActive( ) = 0;
	virtual HRESULT GetTransportSettings( LPDWORD lpdwSessionType, LPDWORD lpdwFlags ) = 0;

public:

	LONG	m_lRefCount;
};
#endif
