#ifndef __PPP_H__
#define __PPP_H__

#include "lcp.h"
#include "pap.h"
#include "ncp.h"

typedef DWORD IPADDR;

/*
 * DEFINES
 */
#define PPP_FLAG_BYTE           0x7E
#define PPP_ESC_BYTE            0x7D

#define READ_TIMEOUT (3 * 60 * 1000)

// Maximum length used for the phone number
#define PNLEN                   56

// these constants are used as indecies into the PPP_CP_INFO array
#define CP_LCP                  0
#define CP_PAP                  1
#define CP_IPCP                 2
#define NUM_CP                  3

#define PPP_MAX_FAILURE         5

// Add 4 bytes for protocol (1), control (1), and CRC (2)
#define PPP_FRAME_MAX_LEN (LCP_DEFAULT_MRU + 4)

/*
 * Framing options
 */
#define PPP_MULTILINK_FRAMING           0x00000010
#define PPP_SHORT_SEQUENCE_HDR_FORMAT   0x00000020
#define PPP_MC_MULTILINK_FRAMING        0x00000040
#define PPP_FRAMING                     0x00000100
#define PPP_COMPRESS_ADDRESS_CONTROL    0x00000200
#define PPP_COMPRESS_PROTOCOL_FIELD     0x00000400
#define PPP_ACCM_SUPPORTED              0x00000800

// Various control protocol IDs
#define PPP_LCP_PROTOCOL        0xC021  // Link Control Protocol 
#define PPP_PAP_PROTOCOL        0xC023  // Password Authentication Protocol 
#define PPP_CBCP_PROTOCOL       0xC029  // Callback Control Protocol
#define PPP_BACP_PROTOCOL       0xC02B  // Bandwidth Allocation Control Protocol
#define PPP_BAP_PROTOCOL        0xC02D  // Bandwidth Allocation Protocol
#define PPP_CHAP_PROTOCOL       0xC223  // Challenge Handshake Auth. Protocol
#define PPP_IPCP_PROTOCOL       0x8021  // Internet Protocol Control Protocol 
#define PPP_ATCP_PROTOCOL       0x8029  // Appletalk Control Protocol 
#define PPP_IPXCP_PROTOCOL      0x802B  // Novel IPX Control Procotol 
#define PPP_NBFCP_PROTOCOL      0x803F  // NetBIOS Framing Control Protocol 
#define PPP_CCP_PROTOCOL        0x80FD  // Compression Control Protocol
#define PPP_SPAP_NEW_PROTOCOL   0xC027  // Shiva PAP new protocol
#define PPP_EAP_PROTOCOL        0xC227  // Extensible Authentication Protocol

// CHAP Digest codes
#define PPP_CHAP_DIGEST_MD5        0x05 // PPP standard MD5
#define PPP_CHAP_DIGEST_MSEXT      0x80 // Microsoft extended CHAP (nonstandard)
#define PPP_CHAP_DIGEST_MSEXT_NEW  0x81 // Microsoft extended CHAP (nonstandard)

// Config Codes
#define CONFIG_REQ              1
#define CONFIG_ACK              2
#define CONFIG_NAK              3
#define CONFIG_REJ              4
#define TERM_REQ                5
#define TERM_ACK                6
#define CODE_REJ                7
#define PROT_REJ                8
#define ECHO_REQ                9
#define ECHO_REPLY              10
#define DISCARD_REQ             11
#define IDENTIFICATION          12
#define TIME_REMAINING          13

typedef enum _PPP_PHASE {
    PPP_LCP,
    PPP_AP,
    PPP_NCP
} PPP_PHASE;

typedef enum _FSM_STATE {
    FSM_INITIAL,
    FSM_STARTING,
    FSM_CLOSED,
    FSM_STOPPED,
    FSM_CLOSING,
    FSM_STOPPING,
    FSM_REQ_SENT,
    FSM_ACK_RCVD,
    FSM_ACK_SENT,
    FSM_OPENED
} FSM_STATE;

typedef enum _AP_ACTION {

    AP_NoAction,        // Be passive, i.e. listen without timeout (default)
    AP_Done,            // End authentication session, dwError gives result
    AP_SendAndDone,     // As above but send message without timeout first
    AP_Send,            // Send message, don't timeout waiting for reply
    AP_SendWithTimeout, // Send message, timeout if reply not received
    AP_SendWithTimeout2,// As above, but don't increment retry count
    AP_Authenticate     // Authenticate using specified credentials.

} AP_ACTION;

/*
 * STRUCTURES
 */
typedef struct _PPP_FRAME {
    BYTE Address;
    BYTE Control;
    BYTE Data[1];
} PPP_FRAME;

#define PPP_FRAME_HDR_LEN (sizeof(PPP_FRAME) - 1)

typedef struct _PPP_PACKET {
    BYTE Protocol[2];   // Protocol Number
    BYTE Data[1];       // Data
} PPP_PACKET;

#define PPP_PACKET_HDR_LEN (sizeof(PPP_PACKET) - 1)

typedef struct _PPP_CONFIG {
    BYTE Code;          // Config code
    BYTE Id;            // ID of this config packet
    BYTE Length[2];     // Length of this packet
    BYTE Data[1];       // Data
} PPP_CONFIG;

#define PPP_CONFIG_HDR_LEN (sizeof(PPP_CONFIG) - 1)

typedef struct _PPP_OPTION {
    BYTE Type;          // Option Code
    BYTE Length;		// Length of this option packet
    BYTE Data[1];       // Data
} PPP_OPTION;

#define PPP_OPTION_HDR_LEN (sizeof(PPP_OPTION) - 1)

typedef struct _PPP_RECEIVED PPP_RECEIVED;
typedef struct _PPP_RECEIVED {

    PPP_RECEIVED *pNext;
    PPP_RECEIVED *pPrev;

    DWORD iCrc;
    DWORD cb;

    Packet *pkt;

} PPP_RECEIVED;

typedef struct _PPP_CP_INFO {
    
    // Protocol number for this CP
    DWORD Protocol;

    // Recognize options up to this number.
    DWORD Recognize;

    // Initialize all work values.
    DWORD (*CP_Begin)(VOID);

    // Free any allocated data.
    DWORD (*CP_End)(VOID);

    // When leaving Initial or Stopped states. May be NULL.
    DWORD (*CP_ThisLayerStarted)(VOID);

    // When entering Closed or Stopped states. May be NULL
    DWORD (*CP_ThisLayerFinished)(VOID);

    // When entering the Opened state. May be NULL. 
    DWORD (*CP_ThisLayerUp)(VOID);

    // When leaving the Opened state. May be NULL. 
    DWORD (*CP_ThisLayerDown)(VOID);
 
    // Called to make a configure request.
    DWORD (*CP_MakeConfigRequest)(PPP_CONFIG *pRequestBuf, DWORD cbRequestBuf);

    // Called when configure request is received and a result packet 
    // Ack/Nak/Reject needs to be sent
    DWORD (*CP_MakeConfigResult)(PPP_CONFIG *pReceiveBuf, PPP_CONFIG *pResultBuf,
            DWORD cbResultBuf, BOOL fRejectNaks);

    // Called to process an Ack that was received.
    DWORD (*CP_ConfigAckReceived)(PPP_CONFIG *pReceiveBuf);

    // Called to process a Nak that was received.
    DWORD (*CP_ConfigNakReceived)(PPP_CONFIG *pReceiveBuf);

    // Called to process a Rej that was received.
    DWORD (*CP_ConfigRejReceived)(PPP_CONFIG *pReceiveBuf);

    DWORD (*CP_MakeMessage)(PPP_CONFIG *pReceiveBuf);

} PPP_CP_INFO;

typedef struct _AP_INFO {
    AP_ACTION Action;
    PAP_STATE State;
} AP_INFO;

typedef struct _PPP_INFO {
    CHAR szUsername[UNLEN + 1];
    CHAR szPassword[PWLEN + 1];
    CHAR szDomain[DNLEN + 1];
    CHAR szNumber[PNLEN + 1];
    
    PPP_PHASE Phase;
    FSM_STATE State;
    DWORD dwError;

    BYTE Id;
    BYTE LastId;

    DWORD NakRetryCount;
    DWORD RejRetryCount;

    DWORD ConfigRetryCount;
    DWORD TermRetryCount;

    PPP_FRAME *pFrame;

} PPP_INFO;

typedef struct _READ_QUEUE {

    HANDLE hEvent;

    DWORD nHdr;
    DWORD nCrc;
    DWORD nOk;

    PPP_RECEIVED *pReceived;
    PPP_RECEIVED *pReceivedFirst;
    PPP_RECEIVED *pReceivedLast;

} READ_QUEUE;

typedef struct _PPP_TIMER {
    DWORD Expire;
    DWORD Timeout;
} PPP_TIMER;

typedef struct _FRAMING_INFO {
    DWORD SendAccm;
    DWORD RecvAccm;

    UINT fSendPfc : 1;
    UINT fSendAcfc : 1;
    UINT fRecvPfc : 1;
    UINT fRecvAcfc : 1;
} FRAMING_INFO;

typedef struct _IP_INFO {
    IfInfo *ifp;
    IPADDR IpAddrRemote;
    IPADDR IpAddrLocal;
    IPADDR IpAddrDns;
} IP_INFO;

// auth.cpp
BOOL ApStart(DWORD CpIndex);
VOID ApReset(VOID);
VOID ApWork(DWORD CpIndex, PPP_CONFIG *pRecvConfig);

// pap.cpp
VOID GetPapInfo(PPP_CP_INFO *pInfo);

// lcp.cpp
VOID GetLcpInfo(PPP_CP_INFO *pInfo);

// worker.cpp
DWORD WorkerThread(LPVOID pThreadParameter);
BOOL SendFrame(DWORD dwLength);
VOID SetTimeout(VOID);
VOID ClearTimeout(VOID);

// hostwire.cpp
VOID HostToWireFormat16(WORD wHostFormat, PBYTE pWireFormat);
VOID HostToWireFormat32(DWORD dwHostFormat, PBYTE pWireFormat);
WORD WireToHostFormat16(PBYTE pWireFormat);
DWORD WireToHostFormat32(PBYTE pWireFormat);

// string.cpp
char* str_search(const char *str1, const char *str2);
char* str_add(char *s1, const char *s2);

// modem.cpp
BOOL InitModem(VOID);
BOOL DialModem(VOID);
BOOL HangupModem(VOID);

// misc.cpp
VOID InitRestartCounters(VOID);
DWORD GetCpIndexFromProtocol(DWORD dwProtocol);
VOID EncodePw(CHAR* pszPassword);
VOID DecodePw(CHAR* pszPassword);

// smaction.cpp
BOOL FsmSendConfigReq(DWORD CpIndex);
BOOL FsmSendTermReq(DWORD CpIndex);
BOOL FsmSendTermAck(DWORD CpIndex, PPP_CONFIG *pRecvConfig);
BOOL FsmSendConfigResult(DWORD CpIndex, PPP_CONFIG *pRecvConfig, BOOL *pfAcked);
BOOL FsmSendEchoReply(DWORD CpIndex, PPP_CONFIG *pRecvConfig);
BOOL FsmSendCodeReject(DWORD CpIndex, PPP_CONFIG *pRecvConfig);
BOOL FsmSendProtocolRej(PPP_PACKET *pPacket, DWORD dwPacketLength);
BOOL FsmInit(DWORD CpIndex);
BOOL FsmReset(DWORD CpIndex);
BOOL FsmThisLayerUp(DWORD CpIndex);
BOOL FsmThisLayerDown(DWORD CpIndex);
BOOL FsmThisLayerStarted(DWORD CpIndex);
BOOL FsmThisLayerFinished(DWORD CpIndex);

/// smevents.cpp
VOID FsmUp(DWORD CpIndex);
VOID FsmOpen(DWORD CpIndex);
VOID FsmDown(DWORD CpIndex);
VOID FsmClose(DWORD CpIndex);

// ncp.cpp
VOID GetIpcpInfo(PPP_CP_INFO *pInfo);
BOOL NcpStart(DWORD CpIndex);
VOID NcpReset(VOID);

// net.cpp
BOOL InitNet(VOID);

// receive.cpp
VOID ReceiveConfigReq(DWORD CpIndex, PPP_CONFIG *pRecvConfig);
VOID ReceiveConfigAck(DWORD CpIndex, PPP_CONFIG *pRecvConfig);
VOID ReceiveConfigNakRej(DWORD CpIndex, PPP_CONFIG *pRecvConfig);
VOID ReceiveTermReq(DWORD CpIndex, PPP_CONFIG *pConfig);
VOID ReceiveTermAck(DWORD CpIndex, PPP_CONFIG *pRecvConfig);
VOID ReceiveUnknownCode(DWORD CpIndex, PPP_CONFIG *pConfig);
VOID ReceiveDiscardReq(DWORD CpIndex, PPP_CONFIG *pConfig);
VOID ReceiveEchoReq(DWORD CpIndex, PPP_CONFIG *pConfig);
VOID ReceiveEchoReply(DWORD CpIndex, PPP_CONFIG *pConfig);
VOID ReceiveCodeRej(DWORD CpIndex, PPP_CONFIG *pConfig);
VOID ReceiveProtocolRej(PPP_PACKET *pPacket);
VOID FsmReceive(PPP_PACKET *pPacket, DWORD dwPacketLength);
BOOL FsmConfigResultReceived(DWORD CpIndex, PPP_CONFIG *pRecvConfig);

// frame.cpp
VOID DecodeFrame(PBYTE pIn, DWORD *pcb);
VOID EncodeFrame(PBYTE pIn, PBYTE pOut, DWORD *pcb);
BOOL CheckCrc(PBYTE pb, DWORD cb);

/*
 * GLOBALS
 */
extern PPP_INFO gInfo;
extern PPP_CP_INFO gCpTable[NUM_CP];
extern LCP_INFO gLcp;
extern READ_QUEUE gQueue;
extern PPP_TIMER gTimer;
extern FRAMING_INFO gFraming;
extern AP_INFO gAp;
extern IP_INFO gIp;

/*
 * INLINES
 */

// Allocate from the process heap
inline PVOID Alloc(SIZE_T size)
{
    return (PVOID)LocalAlloc(LPTR, size);
}

// Free to the process heap
inline VOID Free(PVOID p)
{
    LocalFree((HLOCAL)p);
}

inline BYTE GetId(VOID)
{
    return ++gInfo.Id;
}

inline BOOL IsLcpOpened(VOID)
{
    return (gInfo.Phase == PPP_LCP);
}

inline PPP_PACKET* GetSendPacket(VOID)
{
    return (PPP_PACKET*)(gInfo.pFrame->Data);
}

inline DWORD GetMagicNumber(VOID)
{
    DWORD dwMagic = XnetRand();
    if (dwMagic == 0) {
        dwMagic = 29;
    }
    return dwMagic;
}

#endif // __PPP_H__

