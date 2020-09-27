/*==========================================================================
 *
 *  Copyright (C) 1999 - 1999 Microsoft Corporation.  All Rights Reserved.
 *
 *  File:       vplayer.h
 *  Content:	Voice Player Entry
 *				
 *  History:
 *   Date		By		Reason
 *   ====		==		======
 *  03/26/00	rodtoll Created
 *  09/01/2000  georgioc started rewrite/port to xbox
 ***************************************************************************/

#ifndef __VPLAYER_H
#define __VPLAYER_H

#define NODSOUND
#include "dvntos.h"
#include <dsoundp.h>
#include <dvoicep.h>

#include "dvshared.h"

#include "inqueue2.h"
#include "dvprot.h"
#include "dvsndt.h"

#define VOICEPLAYER_FLAGS_DISCONNECTED          0x00000001  // Player has disconnected
#define VOICEPLAYER_FLAGS_INITIALIZED           0x00000002  // Player is initialized
#define VOICEPLAYER_FLAGS_ISRECEIVING           0x00000004  // Player is currently receiving audio
#define VOICEPLAYER_FLAGS_ISSERVERPLAYER        0x00000008  // Player is the server player
#define VOICEPLAYER_FLAGS_ISAVAILABLE			0x00000020	// Is player available

#define VSIG_VOICEPLAYER		'YLPV'
#define VSIG_VOICEPLAYER_FREE	'YLP_'

VOID DV_VoicePlayerPlaybackPacketCompletion(PVOID pThis, PVOID pContext2, DWORD dwStatus);

class CVoicePlayer
{
public: // Init / destruct

    CVoicePlayer();
    virtual ~CVoicePlayer();

    HRESULT Initialize(const DVID dvidPlayer,
                       const DWORD dwHostOrder, 
                       const DWORD dwPortNumber,
                       DWORD dwFlags, 
                       PVOID pvContext,
                       LIST_ENTRY *pOwner );

    HRESULT CreateQueue( PQUEUE_PARAMS pQueueParams );

    HRESULT CreateMediaObjects(
        const GUID &guidCT,
        PWAVEFORMATEX pwfxTargetFormat,
        DWORD dwUnCompressedFrameSize,
        XMediaObject *pConverter,
        BOOLEAN fUseMcpx);

    virtual HRESULT DeInitialize();
	void FreeResources();
	HRESULT SetPlayerTargets( PDVID pdvidTargets, DWORD dwNumTargets );

    HRESULT SwitchInternalAudioTarget(
        PWAVEFORMATEX pwfxTargetFormat,
        BOOL fUseMcpx,
        BOOL fMute
        );

    inline void AddRef()
    {
        InterlockedIncrement( &m_lRefCount );
    }

    inline void Release()
    {
        if( InterlockedDecrement( &m_lRefCount ) == 0 )
        {
            DeInitialize();
        }
    }

public: // Speech Handling 

    __inline void *__cdecl operator new(size_t size)
    {
        return ExAllocatePoolWithTag(size, 'lcvd');
    }

    __inline void __cdecl operator delete(void *pv)
    {
        ExFreePool(pv);
    }

    HRESULT HandleReceive( PDVPROTOCOLMSG_SPEECHHEADER pdvSpeechHeader, PBYTE pbData, DWORD dwSize );
	HRESULT GetNextFrameAndDecompress(DWORD *pdwReturnFlags, DWORD *pdwSeqNum, DWORD *pdwMsgNum );
	HRESULT DeCompressInBound( PDVMEDIAPACKET pBuffer );
	CFrame *Dequeue();

    inline DVID GetPlayerID()
    {
        return m_dvidPlayer;
    }

    inline DWORD GetFlags()
    {
        return m_dwFlags;
    }
    
    inline BOOL IsReceiving()
    {
        return (m_dwFlags & VOICEPLAYER_FLAGS_ISRECEIVING);
    }

    inline void SetReceiving( const BOOL fReceiving )
    {
        KIRQL irql;
        RIRQL(irql);
        if( fReceiving )
            m_dwFlags |= VOICEPLAYER_FLAGS_ISRECEIVING;
        else
            m_dwFlags &= ~VOICEPLAYER_FLAGS_ISRECEIVING;
        LIRQL(irql);
    }

    inline void SetAvailable( const BOOL fAvailable )
    {
        KIRQL irql;
        RIRQL(irql);
		if( fAvailable )
			m_dwFlags |= VOICEPLAYER_FLAGS_ISAVAILABLE;
		else 
			m_dwFlags &= ~VOICEPLAYER_FLAGS_ISAVAILABLE;
    	LIRQL(irql);
    }

    inline BOOL IsAvailable()
    {
    	return (m_dwFlags & VOICEPLAYER_FLAGS_ISAVAILABLE);
   	}

    inline BOOL IsInitialized()
    {
        return (m_dwFlags & VOICEPLAYER_FLAGS_INITIALIZED);
    }

    inline BOOL IsServerPlayer()
    {
        return (m_dwFlags & VOICEPLAYER_FLAGS_ISSERVERPLAYER);
    }

    inline void SetServerPlayer()
    {
        KIRQL irql;
        RIRQL(irql);
        m_dwFlags |= VOICEPLAYER_FLAGS_ISSERVERPLAYER;
        LIRQL(irql);
    }

    inline BOOL IsDisconnected()
    {
        return (m_dwFlags & VOICEPLAYER_FLAGS_DISCONNECTED);
    }

    inline void SetDisconnected()
    {
        KIRQL irql;
        RIRQL(irql);
        m_dwFlags |= VOICEPLAYER_FLAGS_DISCONNECTED;
        LIRQL(irql);
    }

    inline void SetHostOrder( const DWORD dwHostOrder )
    {
        KIRQL irql;
        RIRQL(irql);
        m_dwHostOrderID = dwHostOrder;
        LIRQL(irql);
    }

    inline DWORD GetHostOrder() 
    {
        return m_dwHostOrderID;
    }

    inline void *GetContext()
    {
        return m_pvPlayerContext;
    }

    inline void SetContext( void *pvContext )
    {
        KIRQL irql;
        RIRQL(irql);
        m_pvPlayerContext = pvContext;
        LIRQL(irql);
    }

    inline BYTE GetLastPeak()
    {
        return m_bLastPeak;
    }

    inline DWORD GetTransportFlags()
    {
        return m_dwTransportFlags;
    }

    inline void AddToPlayList( PLIST_ENTRY ListHead )
    {
        InsertTailList(ListHead,&m_PlayListEntry);
    }

	inline void AddToNotifyList( PLIST_ENTRY ListHead )
	{
        InsertTailList( ListHead, &m_NotifyListEntry);

	}

    inline void RemoveFromNotifyList()
    {
        RemoveEntryList(&m_NotifyListEntry );
    }

	inline void RemoveFromPlayList()
	{
		RemoveEntryList(&m_PlayListEntry);
	}

	inline DWORD_PTR GetLastPlayback()
	{
		return m_dwLastPlayback;
	}

	inline DWORD GetNumTargets()
	{
		return m_dwNumTargets;
	}

	inline PDVID GetTargetList()
	{
		return m_pdvidTargets;
	}

    HRESULT GetSoundTarget(CSoundTarget **ppTarget) {*ppTarget = m_pSoundTarget;return NOERROR;}

    HRESULT CreateSoundTarget(CSoundTarget **ppSoundTarget);
    void DeleteSoundTarget();

	LIST_ENTRY			m_NotifyListEntry;
	LIST_ENTRY			m_PlayListEntry;
    LIST_ENTRY          m_PoolEntry;

    LIST_ENTRY          m_AvailableList;
    LIST_ENTRY          m_PendingList;

protected:

    virtual void Reset();

    CRITICAL_SECTION    m_CS;
    LONG		        m_lRefCount;		// Reference count on the player
	PDVID				m_pdvidTargets;		// The player's current target
	DWORD				m_dwNumTargets;

    DWORD               m_dwUnCompressedFrameSize;
    DWORD               m_dwTransportFlags;
    DWORD               m_dwFlags;
    DWORD               m_dwNumSilentFrames;
    DWORD               m_dwNumSpeechFrames;
    DWORD               m_dwNumReceivedFrames;
    DWORD               m_dwNumLostFrames;
	DVID		        m_dvidPlayer;		// Player's ID
	DWORD				m_dwHostOrderID;	// Host ORDER ID	

	XMediaObject        *m_pInBoundAudioConverter; // Converter for this player's audio    
    XMediaObject	    *m_pPlaybackMediaObject;   // Dsound playback XMO    

    PWAVEFORMATEX       m_pwfxEncodedFormat;

    CSoundTarget        *m_pSoundTarget;    // XMO we provide to calle rin responce to DVC_CreateSoundTarget

	CInputQueue2		*m_lpInputQueue;	// Input queue for this player's audio
    PVOID               m_pvPlayerContext;
    PLIST_ENTRY         m_pOwner;          // owner linked list where this player goes after we DeInitialize

    PVOID               m_pMediaPacketBuffer;

	DWORD_PTR			m_dwLastData;		// GetTickCount() value when last data received
    DWORD_PTR			m_dwLastPlayback;	// GetTickCount() when last non-silence from this player

    BYTE				m_bLastPeak;		// Last peak value for this player.
    UCHAR               m_bPortNumber;      // hawk port number..

};



#endif
