//-----------------------------------------------------------------------------
// File: QueueXMOp.h
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#ifndef QUEUEXMOP_H_INCLUDED
#define QUEUEXMOP_H_INCLUDED

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef VOICEPEERAFX_H
#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

#include <nt.h>
#include <ntrtl.h>
#include <nturtl.h>
#include <stddef.h>
#include <ntos.h>

#ifdef __cplusplus
}
#endif // __cplusplus
#endif
#define  NODSOUND
#include <xtl.h>
#include <dsound.h>
#include "xvoice.h"
#include <assert.h>


//
// Typedefs
//
//////////////////////////////////////////////////////////////////////////////
typedef DWORD TimeStamp;
typedef DWORD TimeInterval;

//
// Constants
//
//////////////////////////////////////////////////////////////////////////////
const WORD MAX_MILISECONDS_OF_ATTENUATED_VOICE = 80;

//
// Debug Tools
//
//////////////////////////////////////////////////////////////////////////////
extern "C"
{
	ULONG __cdecl
	DebugPrint(
		PCHAR Format,
		...
		);
}

#define VERIFY_SUCCESS(xprsn) \
{\
    HRESULT hrTemp = (xprsn);\
    if(!SUCCEEDED(hrTemp))\
    {\
        DebugPrint("Call failed: %s(%d): 0x%0.8x\n", __FILE__, __LINE__, hrTemp);\
    }\
}

//
// Structures
//
//////////////////////////////////////////////////////////////////////////////
struct BufferList
{
	TimeStamp		m_timestamp;
	BYTE			*m_pbBuffer;
	BufferList      *m_pPrev;
	BufferList      *m_pNext;
	
	BufferList() : m_timestamp(0), m_pbBuffer(NULL) , m_pPrev(NULL), m_pNext(NULL){}
	~BufferList(){}
};

//
// Class CQueueXMO
//
//////////////////////////////////////////////////////////////////////////////
class CQueueXMO:public XVoiceQueueMediaObject 
{
public:
	//
    // Construction/Destruction
    //
	CQueueXMO() : m_cRef(1),
	              m_pBufferPool(NULL),
				  m_pHeapForBuffers(NULL),
		          m_pFreeEntriesList(NULL),
				  m_pInUseEntriesListHead(NULL),
				  m_pInUseEntriesListTail(NULL),
				  m_LogicalOutputTime(0),
				  m_RealStartOutputTime(0),
				  m_wCurrentSeqNo(0),
				  m_pDecoder(NULL) ,
      			  m_wInUseEntriesCount(0),
				  m_bStartTiming(FALSE),
				  m_pdHighWaterMarkQuality(NULL),
				  m_pSavedBuffer(NULL),
				  m_bVoiceReturned(TRUE)
	{
		m_LastOutputInfo.cbSize = sizeof(PCM_INFO);
		m_LastOutputInfo.nMaxPower = 0; // no voice
	}

	virtual ~CQueueXMO()
	{
		Cleanup();
	}


	//
    // Initialization
    //
	HRESULT STDMETHODCALLTYPE Initialize( LPQUEUE_XMO_CONFIG pConfig );

	//
    // IUnknown Methods
    //
    virtual STDMETHODIMP_(ULONG) AddRef( void ) 
	{
       return InterlockedIncrement( (long*) &m_cRef );
    }

    virtual STDMETHODIMP_(ULONG) Release( void ) 
	{
       long l = InterlockedDecrement( (long*) &m_cRef );

       if ( l == 0 )
	   {
          delete this;
	   }

       return l;
    }
    
	//
    // XMediaObject methods
    //
    virtual HRESULT STDMETHODCALLTYPE Flush( void )
    {
		m_wHighWaterMark = 0;
        return QUEUE_S_OK;
    }

    virtual HRESULT STDMETHODCALLTYPE Discontinuity( void )
    {
        return QUEUE_S_OK;
    }

    virtual HRESULT STDMETHODCALLTYPE GetStatus( LPDWORD pdwStatus )
    {
        return QUEUE_S_OK;
    }

	virtual HRESULT STDMETHODCALLTYPE GetInfo( LPXMEDIAINFO pXMediaInfo )
	{
		return QUEUE_S_OK;
	}
    virtual HRESULT STDMETHODCALLTYPE Process( LPCXMEDIAPACKET pSrcBuffer, LPCXMEDIAPACKET pDstBuffer );

	
	//
    // XVoiceQueueMediaObject methods
    //
    virtual HRESULT STDMETHODCALLTYPE GetLastOutputInfo(LPPCM_INFO pPCMInfo);

protected:
	void Cleanup();
	void SetConfigDefaults(LPQUEUE_XMO_CONFIG pConfig);
	ULONG m_cRef;

	//
    // Specific queuing members
    //
protected:
	BufferList       *m_pBufferPool;         // The array of BufferList from which the Free and InUse lists are composed
	BYTE             *m_pHeapForBuffers;     // Amount of contigous memory for the actual codec buffers
	WORD              m_wBufferPoolSize ;    // Maximum number of free buffers
	WORD              m_wMsOfDataPerBuffer;  // Miliseconds of compressed data in a codec buffer
	WORD              m_wXVoiceBufferSize;   // Size of decompressed data in bytes
	WORD              m_wCodecBufferSize;    // Size of codec buffer in bytes(contains header + compressed voice)

	BufferList       *m_pFreeEntriesList;       // Double linked list of free buffers to be used to get the input from process
	BufferList       *m_pInUseEntriesListHead;  // Double linked list of buffers to be used to process input data
	BufferList       *m_pInUseEntriesListTail;  // End of in use list

	TimeStamp         m_LogicalOutputTime;   // Time for an input packet to be introduced into the output stream
	TimeStamp         m_PreviousTime;        // Most recent time a packet was introduced into the output stream
	TimeStamp         m_RealStartOutputTime; // The real time that the output process began with; reference time
	WORD              m_wCurrentSeqNo;       // Sequence number for the current received packet
	WORD              m_wSafeDeltaSeq;       // Safe difference between input sequence numbers

	XMediaObject     *m_pDecoder;            // Decoder XMO
	WORD			  m_wHighWaterMark;      // Current high water mark(in miliseconds)
	WORD			  m_wInUseEntriesCount;  // Number of buffers waiting to be decompressed 

	BOOL              m_bStartTiming;        // Start measuring real time of the queue

	DOUBLE            *m_pdHighWaterMarkQuality;    // Array that keeps the history of performances of the queue
	WORD	          m_wMaxHighWaterMark;			// Maximum delay for the queue in miliseconds
	WORD	          m_wMinHighWaterMark;			// Minimum delay for the queue in miliseconds
	WORD              m_wHighWaterMarkQualitySize;  // Size of the array

	DOUBLE            m_dIdealQuality;              // Ideal quality for high water mark 
	DOUBLE            m_dFrameStrength;             // Current frame strength
	DOUBLE            m_dCurrentTalkspurtWeighting; // Computed current talkspurt weighting
	WORD              m_wNumberOfFramesInTalkspurt; // Counter for each talkspurt
	BYTE              m_bCurrentTalkspurtID;        // The message ID of the current talkspurt
	WORD              m_wErrorCount;                // Number of errors in current talkspurt

	BYTE             *m_pSavedBuffer;          // Buffer saved between outputs in case of lost packets
	BOOL              m_bVoiceReturned;        // True if previous output buffer contained voice
	WORD              m_wAttenuatedBackupMs;   // Number of miliseconds of attenuated voice provided for lost voice

	PCM_INFO          m_LastOutputInfo;        // PCM info on the last returned buffer         

	//
    // Specific queuing methods
    //
protected:
	void RecomputeQueueParameters();
	void IncrementErrorCount(){ m_wErrorCount++; }
	void AddFreeBuffer(BufferList *pBuffer);
	HRESULT AddInUseBuffer(BufferList *pBuffer);

	BufferList* RemoveFirstBuffer(BufferList **pHead , BufferList **pTail = NULL);
	BufferList* GetInputWorkingBuffer();
	BufferList* GetOutputWorkingBuffer(BOOL *pbReturnSilence , BOOL *pbIsNextPacketLost);

	HRESULT OutputProcess(LPCXMEDIAPACKET pOutMediaPacket);
	HRESULT InputProcess(LPCXMEDIAPACKET pInMediaPacket);
	HRESULT DecompressMediaBuffer(BufferList *pSrc, LPCXMEDIAPACKET pDst);

	void CopyAttenuatedBuffer(LPCXMEDIAPACKET pOutMediaPacket , DOUBLE dAttenuationFactor);

};


#endif  // QUEUEXMOP_H_INCLUDED
