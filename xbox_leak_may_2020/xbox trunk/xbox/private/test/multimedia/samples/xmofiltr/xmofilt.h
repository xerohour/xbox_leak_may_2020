

#include <xtl.h>
#include <xdbg.h>


#if DBG

//
// simple macro to verify that the packet size is OK for a particular XMO
//


#define XMOFILTER_VERIFY_PACKET_SIZE(xMediaInfo,dwPacketSize) ASSERT(xMediaInfo.dwOutputSize <= dwPacketSize);\
    if (xMediaInfo.dwOutputSize) ASSERT(dwPacketSize%xMediaInfo.dwOutputSize == 0);\
    ASSERT(dwPacketSize >= xMediaInfo.dwMaxLookahead);\

#else 

    #define XMOFILTER_VERIFY_PACKET_SIZE(xMediaInfo,dwPacketSize)

#endif


#if DBG

#define DEBUG_LEVEL_ERROR 0
#define DEBUG_LEVEL_WARNING 1
#define DEBUG_LEVEL_INFO 2
#define DEBUG_LEVEL_SPAM 5

extern ULONG g_XmoFilterDebugLevel;
#define DBG_SPAM(_exp_) {if (g_XmoFilterDebugLevel >= DEBUG_LEVEL_SPAM) DbgPrint _exp_;}
#define DBG_INFO(_exp) {if (g_XmoFilterDebugLevel >= DEBUG_LEVEL_INFO) DbgPrint _exp;}
#define DBG_ERROR(_exp) {if (g_XmoFilterDebugLevel >= DEBUG_LEVEL_ERROR) DbgPrint _exp;}
#define DBG_WARN(_exp) {if (g_XmoFilterDebugLevel >= DEBUG_LEVEL_WARNING) DbgPrint _exp;}

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


#define PACKET_CNT      3

//
// we arrived at this value based on the requirements of the XMOs
// The idea is to pick an arbitrary packet size, go through the Init function once
// and then if any asserts fires while verifying the packet sizes, change the packet
// to satisfy all requirements
//

#define PACKET_SIZE		0x140*0x40

#define MAXBUFSIZE      (PACKET_SIZE*PACKET_CNT)



#define BREAK_INTO_DEBUGGER __asm int 3

typedef struct _testmediabuf {

    LIST_ENTRY ListEntry;
	DWORD dwStatus;
	DWORD dwCompletedSize;
    PUCHAR pBuffer;

} MEDIA_PACKET_CTX, *PMEDIA_PACKET_CTX;

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

    LIST_ENTRY  SrcPendingList;
    LIST_ENTRY  DstPendingList;

	PUCHAR TransferBuffer;
    PUCHAR PacketContextPool;

    ULONGLONG dwReserved;


} GRAPH_CONTEXT, *PGRAPH_CONTEXT;


typedef struct _testdata {

	DWORD dwFlags;
    GRAPH_CONTEXT ContextArray[2];

} TESTDATA, *PTESTDATA;

//
// xmofilt.cpp
//
VOID
Cleanup();

HRESULT
TransferData(DWORD dwIndex);

HRESULT
Init(DWORD dwIndex);

VOID
InsertContext(
    PGRAPH_CONTEXT pContext,
    PMEDIA_PACKET_CTX pCtx,
    XMediaObject *pXmo,
    BOOL fSource
    );


