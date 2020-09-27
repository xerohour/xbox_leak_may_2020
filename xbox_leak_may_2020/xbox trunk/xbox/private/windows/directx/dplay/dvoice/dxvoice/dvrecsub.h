/*==========================================================================
 *
 *  Copyright (C) 1999 Microsoft Corporation.  All Rights Reserved.
 *
 *  File:		ClientRecordSubSystem.h
 *  Content:	Recording sub-system.
 *
 *  History:
 *   Date		By		Reason
 *   ====		==		======
 * 07/19/99		rodtoll	Modified from original
 * 09/01/2000  georgioc started rewrite for xbox
 ***************************************************************************/
#ifndef __CLIENTRECORDSUBSYSTEM_H
#define __CLIENTRECORDSUBSYSTEM_H
#include "dvcleng.h"
#include "va.h"

// CClientRecordSubSystem
//
// This class implements the recording subsystem for the BattleCom client.
// It works closely with the control CShadowClientControl object to 
// provide the recording / compression and transmissions portions of the
// client.  This includes addition of microphone clicks to outgoing
// audio streams when appropriate.
//
// The core of the recording system is a finite state machine which 
// is used to provide a way of managing the recording system's
// state and to provide smooth transitions between various 
// states.  
//
// It looks to the CShadowClientControl object to detect when keys
// are pressed and to provide neccessary parameters.
//
#define VSIG_CLIENTRECORDSYSTEM			'SRCV'
#define VSIG_CLIENTRECORDSYSTEM_FREE	'SRC_'
//


VOID DV_RecordSubSystemPacketCompletion(
    PVOID pThis,
    PVOID pContext,
    DWORD dwStatus);

class CClientRecordSubSystem
{
protected: // State Machine States
    typedef enum {
        RECORDSTATE_NOTSTARTED = 0, // we have not attached packets to hawk yet
        RECORDSTATE_IDLE,   	// Recording is idle, no transmissions required
        RECORDSTATE_VA,			// Voice activated mode
        RECORDSTATE_PTT			// Push to talk mode
    } RecordState;

public:
    __inline void *__cdecl operator new(size_t size)
    {
        return ExAllocatePoolWithTag(size, 'srvd');
    }

    __inline void __cdecl operator delete(void *pv)
    {
        ExFreePool(pv);
    }

    CClientRecordSubSystem( CDirectVoiceClientEngine *clientEngine );
    ~CClientRecordSubSystem();

    LIST_ENTRY                 m_PendingList;
    LIST_ENTRY                 m_CompletedList;

protected:

	friend class CDirectVoiceClientEngine;

	HRESULT Initialize();

	BOOL IsMuted();
	BOOL IsValidTarget();
    inline BOOL IsPTT() { return !IsVA(); };
    inline BOOL IsVA() { return (m_clientEngine->m_dvClientConfig.dwFlags & DVCLIENTCONFIG_MANUALVOICEACTIVATED || m_clientEngine->m_dvClientConfig.dwFlags & DVCLIENTCONFIG_AUTOVOICEACTIVATED); };
    
    BOOL CheckVA();
    void EndMessage();
    void StartMessage();

    HRESULT TransmitFrame(PDVMEDIAPACKET pBuffer);

    void static PacketCompletion(PVOID Context);    // callback from hawk driver


protected: // FSM
	HRESULT BuildAndTransmitSpeechHeader(PDVMEDIAPACKET pBuffer);
	HRESULT BuildAndTransmitSpeechWithTarget(PDVMEDIAPACKET pBuffer);
    HRESULT RecordFSM();

protected: 

	DWORD					m_dwSilentTime;			// # of ms that the input has been silent
    DWORD					m_dwSilenceTimeout;  	// # of ms that the input has been silent
	DWORD					m_dwFrameTime;			// Amount of ms per frame
	CVoiceActivation1*		m_pVa;				    // Auto Gain control and Voice Activation algorithm

	void					DoFrameCheck();
	
protected:

    RecordState             m_recordState;          // Current state of the FSM
    XMediaObject            *m_pConverter;          // AudioConverter for outgoing data
    PWAVEFORMATEX           m_pwfxConverter;
    WAVEFORMATEX            m_wfxCapture;

    XMediaObject            *m_pCaptureMediaObject; // hawk xmo  

    CDirectVoiceClientEngine *m_clientEngine;       // The client engine this subsystem is for
    BOOL                    m_transmitFrame;        // Transmit Current frame?
	BOOL					m_lastFrameTransmitted; 
                                                    // Was the last frame transmitted
	unsigned char			m_msgNum;               // Current message number 
	unsigned char			m_seqNum;               // Current sequence #    	

    PVOID                   m_pMediaPacketBuffer;
	DWORD					m_dwCurrentPower;		// Power level of the last packet
	DWORD					m_dwLastTargetVersion;	// Version of target info on last frame (to check for changes)
	DWORD					m_dwResetCount;
	DWORD					m_dwNumSinceRestart;
	DWORD					m_dwLastFrameTime;		// GetTickCount() at last frame
	DWORD                   m_dwFrameCount;
    DWORD                   m_dwPort;


};

#endif
