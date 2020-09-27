#ifndef VOICE_H_INCLUDED
#define VOICE_H_INCLUDED

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <XBSocket.h>
#include <XBSockAddr.h>
#include <cassert>
#include <stdio.h>
#include <vector>
#include "xvoice.h"
#include "Player.h"

const WORD  VOICE_PORT        = 10985;  // any port other than the first two

const int VOICE_SAMPLING_RATE = 8000;
const int CODEC_SAMPLING_RATE = 8000;

const int XVOICE_BYTES_PER_MS = (((ULONG)( VOICE_SAMPLING_RATE / 1000.0 )) * 2 );

const int XVOICE_BUFFER_REGION_TIME = 40; // initially 80, but changed in order to fit a network packet 
const int XVOICE_BUFFER_REGION_SIZE = ( XVOICE_BYTES_PER_MS * XVOICE_BUFFER_REGION_TIME );
const int XVOICE_PREBUFFER_REGIONS = 20;
const int XVOICE_TOTAL_REGIONS = ( XVOICE_PREBUFFER_REGIONS + 2 );
const int XVOICE_BUFFER_SIZE = ( XVOICE_BUFFER_REGION_SIZE * XVOICE_TOTAL_REGIONS );

const int HAWKS_COUNT = 4;
const int VOICE_DEVICES_COUNT = 2; //Microphone & Headphone
const int MICROPHONE = 0;
const int HEADPHONE = 1;


const int MAX_VOICE_TRANSFER_SIZE = XVOICE_BUFFER_REGION_SIZE + sizeof(VOICE_ENCODER_HEADER);
const int XVOICE_CODEC_BUFFER_SIZE = XVOICE_BUFFER_REGION_SIZE / 4 + sizeof(VOICE_ENCODER_HEADER);

const DWORD SUBMIX_THRESHOLD_TIME = 100;


//////////////////////////////////////////////////////////////////////////////
// Hack! DbgPrint is much nicer than OutputDebugString so JonT exported it from xapi.lib
// (as DebugPrint) but the prototype isn't in the public headers...
extern "C"
{
	ULONG
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
//////////////////////////////////////////////////////////////////////////////

struct MEDIA_PACKET_LIST
{
	MEDIA_PACKET_LIST	*pNext;
	XMEDIAPACKET		MediaPacket;
	DWORD				dwStatus;
	DWORD				dwCompletedSize;

	MEDIA_PACKET_LIST(DWORD dwStatusIn) : pNext(NULL) , dwStatus(dwStatusIn) , dwCompletedSize(0)
	{
		MediaPacket.dwMaxSize = XVOICE_BUFFER_REGION_SIZE;
		MediaPacket.pvBuffer = new BYTE[XVOICE_BUFFER_REGION_SIZE];
		memset(MediaPacket.pvBuffer , 0 , XVOICE_BUFFER_REGION_SIZE);
		MediaPacket.pdwCompletedSize = NULL;
		MediaPacket.pdwStatus = NULL;
		MediaPacket.prtTimestamp = NULL;
		MediaPacket.hCompletionEvent = NULL;
	}
	~MEDIA_PACKET_LIST()
	{
		if( NULL !=  MediaPacket.pvBuffer)
        {
			delete [] MediaPacket.pvBuffer;
        }
	}
};

//
// CVoiceUnit
//
//////////////////////////////////////////////////////////////////////////////
class CVoiceUnit
{
public:
	virtual HRESULT PlayToHeadphone();
	virtual HRESULT OnMicrophoneReadComplete();
	
	virtual void AddSockAddr(const IN_ADDR &addrPlayer);

	virtual HRESULT Inserted();
	virtual void Removed();
	virtual void Process();
	virtual void Init(DWORD dwPortNumber);

	virtual void SendPacketToRenderer();
	virtual BOOL IsPacketPreparedForRender();

    CVoiceUnit(); 
    virtual ~CVoiceUnit();


protected:
	void CreateQueue(MEDIA_PACKET_LIST **pQueue , WORD wSize , DWORD dwStatus);
	void DeleteQueue(MEDIA_PACKET_LIST *pQueue);
/*#ifdef _DEBUG
	void WriteToLogFile(HANDLE hFile , BYTE *pbBuffer , DWORD dwSize , LPOVERLAPPED lpOverlapped);
	void CloseFile(HANDLE hFile);
	void OpenFile(const char* szFileName , HANDLE *phFile);
#endif*/
	virtual void QueueMicrophoneBuffer(MEDIA_PACKET_LIST *pMediaPacketListEntry);
	virtual void QueueHeadphoneBuffer(MEDIA_PACKET_LIST *pMediaPacketListEntry);

    //
    // XMO objects created on call to Inserted()
    //
    XMediaObject *m_pMicrophone;
    XMediaObject *m_pHeadphone;

	//
    // Stuff given to us at init
    //
    DWORD m_dwPortNumber; // Port number for headset 


	//
    // The 2 queues for the buffers that are sent to the microphone/headphone
    //
	MEDIA_PACKET_LIST *m_pBeginMicrophoneQueuedBuffers;
	MEDIA_PACKET_LIST *m_pBeginHeadphoneQueuedBuffers;


 	//
    // Events for completion on microphone/headphone work
    //
	HANDLE m_hMicrophoneWorkEvent; 
	HANDLE m_hHeadphoneWorkEvent; 

	virtual void Cleanup();

/*#ifdef _DEBUG
	//
    // Diagnosis stuff
    //
	HANDLE m_hFileRawPCMData;
	HANDLE m_hFileCompressed;
	HANDLE m_hFileDecompressed;
#endif*/

	//
    //  Status
    //
    BOOL   m_fPreBuffering;
    DWORD  m_dwPreBufferRegions;

};

//
// CCompressedVoiceUnit
//
//////////////////////////////////////////////////////////////////////////////
class CCompressedVoiceUnit : public CVoiceUnit
{
public:
	HRESULT PlayToHeadphone();
	HRESULT OnMicrophoneReadComplete();
	
	HRESULT Inserted();
	void Removed();

    CCompressedVoiceUnit(); 
    ~CCompressedVoiceUnit();

protected:
	//
    // Specific codec stuff functions
    //
	HRESULT CompressMediaBuffer(XMEDIAPACKET *pSrc, XMEDIAPACKET *pDst , BOOL *pvbVoiceDetected );
	HRESULT DecompressMediaBuffer(XMEDIAPACKET *pSrc, XMEDIAPACKET *pDst);

    void SendPacketToRenderer();
	BOOL IsPacketPreparedForRender();

protected:
    //
    // XMO objects created on call to Inserted()
    //
    XMediaObject *m_pEncoder;
    XMediaObject *m_pDecoder;
	XMEDIAPACKET m_CodecMediaBuffer[VOICE_DEVICES_COUNT];

 	//
    // Events for completion on encode/decode work
    //
	HANDLE m_hEncodeWorkEvent; 
	HANDLE m_hDecodeWorkEvent; 

	//
	// Flag for correct use of the codec
	//
	BOOL   m_bInPlaceCodec;     


	void Cleanup();
};

//
// CNetVoiceUnit
//
//////////////////////////////////////////////////////////////////////////////
class CNetVoiceUnit : public CCompressedVoiceUnit
{
public:
    CNetVoiceUnit(); 
    ~CNetVoiceUnit();

    HRESULT Inserted();
    void Removed();

    //
    // Uses the voice queuing system
    //
    HRESULT PlayToHeadphone();

    void AddSockAddr(const IN_ADDR &addrPlayer);
    void SendVoicePacket( XMEDIAPACKET *pMediaPacket );
    BOOL RecvVoicePacket( XMEDIAPACKET *pMediaPacket );

    void SendPacketToRenderer();
    BOOL IsPacketPreparedForRender();

    void QueueHeadphoneBuffer(MEDIA_PACKET_LIST *pMediaPacketListEntry);
    void SubmixMediaPackets(PVOID pSrc, PVOID pDest, DWORD dwBytes, DWORD dwAmpFactor);

protected:
	void Cleanup();

	//
    // Network data
    //
	SOCKET		m_VoiceSock;

  typedef std::vector< SOCKADDR_IN > SockAddrList;
  SockAddrList  m_vSockAddrDests;

	//
    // Voice queuing XMO
    //
	XMediaObject *m_pVoiceQueuingXMO;

	//
    // Interval for output
    //
	DWORD       m_IntervalForOutput;
	DWORD       m_LastOutputTimeStamp;
  DWORD       m_LastNetReceiveTimeStamp;
  SOCKADDR_IN m_saLastNetReceiveSockAddr;
  BOOL        m_fSubmixPacket;
};

//
// CVoiceUnits
//
//////////////////////////////////////////////////////////////////////////////
class CVoiceUnits
{
    public:
        CVoiceUnits();
        ~CVoiceUnits();
        void ThreadProc();
	    friend DWORD WINAPI VoiceThreadProc( LPVOID lpParameter );
		HRESULT Initialize(PlayerList& playerlist);


    private:
	    HRESULT CheckForHotPlugs();
	    HRESULT CheckInitialState();
        CNetVoiceUnit m_VoiceUnits[HAWKS_COUNT];

        HANDLE m_hDeleteEvent; 
		HANDLE m_hThread;
		DWORD m_dwThreadId;
		HANDLE m_hMutex;

};


#endif  // VOICE_H_INCLUDED
