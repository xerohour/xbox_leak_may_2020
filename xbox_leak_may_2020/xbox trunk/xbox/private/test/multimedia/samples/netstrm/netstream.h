
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

#define  NODSOUND
#include <xtl.h>
#include <xdbg.h>
#include <dsoundp.h>

#include <winsockx.h>
#include "netxmo.h"
#include <xdbg.h>

#define NETSTRM_FAILED(Status) ((HRESULT)(Status)<0 && ((HRESULT)(Status)!=E_PENDING))

#define NETSTRM_VERIFY_PACKET_SIZE(xMediaInfo,dwPacketSize) ASSERT(xMediaInfo.dwOutputSize <= dwPacketSize);\
    if (xMediaInfo.dwOutputSize) ASSERT(dwPacketSize%xMediaInfo.dwOutputSize == 0);\
    ASSERT(dwPacketSize >= xMediaInfo.dwMaxLookahead);\

#if DBG


#define DEBUG_LEVEL_ERROR 0
#define DEBUG_LEVEL_WARNING 1
#define DEBUG_LEVEL_INFO 2
#define DEBUG_LEVEL_SPAM 5

extern ULONG g_NetStrmDebugLevel;
#define DBG_SPAM(_exp_) {if (g_NetStrmDebugLevel >= DEBUG_LEVEL_SPAM) DbgPrint _exp_;}
#define DBG_INFO(_exp) {if (g_NetStrmDebugLevel >= DEBUG_LEVEL_INFO) DbgPrint _exp;}
#define DBG_ERROR(_exp) {if (g_NetStrmDebugLevel >= DEBUG_LEVEL_ERROR) DbgPrint _exp;}
#define DBG_WARN(_exp) {if (g_NetStrmDebugLevel >= DEBUG_LEVEL_WARNING) DbgPrint _exp;}

#else
#define DBG_SPAM(_exp_)
#define DBG_INFO(_exp_)
#define DBG_ERROR(_exp_)
#define DBG_WARN(_exp_)
#endif


#undef max
#undef min
#define max(a,b) (((a) > (b)) ? (a) : (b))
#define min(a,b) (((a) < (b)) ? (a) : (b))

//
//  VOID
//  InitializeListHead(
//      PLIST_ENTRY ListHead
//      );
//

#define InitializeListHead(ListHead) (\
    (ListHead)->Flink = (ListHead)->Blink = (ListHead))

//
//  BOOLEAN
//  IsListEmpty(
//      PLIST_ENTRY ListHead
//      );
//

#define IsListEmpty(ListHead) \
    ((ListHead)->Flink == (ListHead))

//
//  PLIST_ENTRY
//  RemoveHeadList(
//      PLIST_ENTRY ListHead
//      );
//

#define RemoveHeadList(ListHead) \
    (ListHead)->Flink;\
    {RemoveEntryList((ListHead)->Flink)}

//
//  PLIST_ENTRY
//  RemoveTailList(
//      PLIST_ENTRY ListHead
//      );
//

#define RemoveTailList(ListHead) \
    (ListHead)->Blink;\
    {RemoveEntryList((ListHead)->Blink)}

//
//  VOID
//  RemoveEntryList(
//      PLIST_ENTRY Entry
//      );
//

#define RemoveEntryList(Entry) {\
    PLIST_ENTRY _EX_Blink;\
    PLIST_ENTRY _EX_Flink;\
    _EX_Flink = (Entry)->Flink;\
    _EX_Blink = (Entry)->Blink;\
    _EX_Blink->Flink = _EX_Flink;\
    _EX_Flink->Blink = _EX_Blink;\
    }

//
//  VOID
//  InsertTailList(
//      PLIST_ENTRY ListHead,
//      PLIST_ENTRY Entry
//      );
//

#define InsertTailList(ListHead,Entry) {\
    PLIST_ENTRY _EX_Blink;\
    PLIST_ENTRY _EX_ListHead;\
    _EX_ListHead = (ListHead);\
    _EX_Blink = _EX_ListHead->Blink;\
    (Entry)->Flink = _EX_ListHead;\
    (Entry)->Blink = _EX_Blink;\
    _EX_Blink->Flink = (Entry);\
    _EX_ListHead->Blink = (Entry);\
    }

//
//  VOID
//  InsertHeadList(
//      PLIST_ENTRY ListHead,
//      PLIST_ENTRY Entry
//      );
//

#define InsertHeadList(ListHead,Entry) {\
    PLIST_ENTRY _EX_Flink;\
    PLIST_ENTRY _EX_ListHead;\
    _EX_ListHead = (ListHead);\
    _EX_Flink = _EX_ListHead->Flink;\
    (Entry)->Flink = _EX_Flink;\
    (Entry)->Blink = _EX_ListHead;\
    _EX_Flink->Blink = (Entry);\
    _EX_ListHead->Flink = (Entry);\
    }

#define PACKET_CNT      10
#define PACKET_SIZE		320

#define MAXBUFSIZE      (PACKET_SIZE*PACKET_CNT)

#define TESTPORT        600
#define TESTADDR_LEFT   "157.56.13.36" // left test box
#define TESTADDR_RIGHT   "157.56.13.40" // right test box
#define LOOPADDRESS       "127.0.0.1" // left test box
#define ANYADDRESS        "0.0.0.0" 


// #define TESTADDR        "157.56.10.165" // davidx3


#define BREAK_INTO_DEBUGGER __asm int 3
#define CALLFAILED(_apiname, err) \
        DbgPrint(#_apiname " failed: %d %d\n", err)

typedef struct _mediabuf {

    LIST_ENTRY ListEntry;
	DWORD dwStatus;
	DWORD dwCompletedSize;
    HANDLE hCompletionEvent;
    PUCHAR pBuffer;

} MEDIA_BUFFER_CTX, *PMEDIA_BUFFER_CTX;

#define TESTF_NET_SEND  	0x00000001
#define TESTF_NET_RECV  	0x00000002
#define TESTF_USE_HAWK_OUT	0x00000004
#define TESTF_USE_HAWK_IN	0x00000008
#define TESTF_USE_CODEC 	0x00000010

#define NET_RECV_CONTEXT      0x00000000
#define NET_SEND_CONTEXT      0x00000001

#define INVALID_PORT        0xFFFFFFFF

typedef struct _graphcontext {

    DWORD dwFlags;
	DWORD dwPlayerPort;
	XMediaObject *pSourceXmo;
	XMediaObject *pIntermediateXmo;
	XMediaObject *pSRCXmo;
	XMediaObject *pTargetXmo;

	DWORD	dwMinPacketSize;
	DWORD	dwPacketSize;
    DWORD   dwMaxBufferCount;

	WAVEFORMATEX wfxAudioFormat;

    LIST_ENTRY  AvailableList;
    LIST_ENTRY  SrcPendingList;
    LIST_ENTRY  DstPendingList;

	XMEDIAPACKET DstMediaBuffer;
	PUCHAR TransferBuffer;
    PUCHAR PacketContextPool;

} GRAPH_CONTEXT, *PGRAPH_CONTEXT;


typedef struct _globaldata {

	DWORD dwFlags;
    GRAPH_CONTEXT ContextArray[2];

} NETSTREAM_GLOBAL_DATA, *PNETSTREAM_GLOBAL_DATA;

//
// netstream.cpp
//
VOID
Cleanup();

HRESULT
TransferData(DWORD dwIndex,DWORD dwTimeout);

HRESULT
Init(DWORD dwIndex);


