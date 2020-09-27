
#define _NTDRIVER_

#include <xtl.h>
#include <winsockx.h>
#include <xdbg.h>

#if DBG

#define DEBUG_LEVEL_ERROR 0
#define DEBUG_LEVEL_WARNING 1
#define DEBUG_LEVEL_INFO 2
#define DEBUG_LEVEL_SPAM 5

extern ULONG g_NetXmoDebugLevel;
#define NX_DBG_SPAM(_exp_) {if (g_NetXmoDebugLevel >= DEBUG_LEVEL_SPAM) DbgPrint _exp_;}
#define NX_DBG_INFO(_exp) {if (g_NetXmoDebugLevel >= DEBUG_LEVEL_INFO) DbgPrint _exp;}
#define NX_DBG_ERROR(_exp) {if (g_NetXmoDebugLevel >= DEBUG_LEVEL_ERROR) DbgPrint _exp;}
#define NX_DBG_WARN(_exp) {if (g_NetXmoDebugLevel >= DEBUG_LEVEL_WARNING) DbgPrint _exp;}

#define NETXMO_FAILED(Status) ((HRESULT)(Status)<0 && ((HRESULT)(Status)!=E_PENDING))

#else

#define NX_DBG_SPAM(_exp_)
#define NX_DBG_INFO(_exp_)
#define NX_DBG_ERROR(_exp_)
#define NX_DBG_WARN(_exp_)

#endif


#undef max
#undef min
#define max(a,b) (((a) > (b)) ? (a) : (b))
#define min(a,b) (((a) < (b)) ? (a) : (b))

#undef  BREAK_INTO_DEBUGGER
#define BREAK_INTO_DEBUGGER __asm int 3
#define CALLFAILED(_apiname, err) \
        DbgPrint(#_apiname " failed: %d %d\n", err)

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

#define NETXMO_PACKET_TYPE_AUDIO    0x01

typedef struct _netXMOPacketHeader {

    BYTE bType;
    BYTE bSeqNum;
    WORD wSize;

} NETXMOPACKETHEADER, *PNETXMOPACKETHEADER;


typedef struct _netxmbcontext {

    LIST_ENTRY ListEntry;
    XMEDIAPACKET Xmb;

} XMB_CONTEXT, *PXMB_CONTEXT;


class CNetXMO:public XMediaObject {

public:
    CNetXMO()
    {
        m_cRef = 0;
        m_pRecvBuffer = NULL;
        m_pSendBuffer = NULL;
        m_bSeqNum  = 0;
        m_hThread = NULL;
        m_hDeleteThreadEvent = NULL;
        m_hRecvPacketEvent = NULL;
        m_hMutex = NULL;
        m_dwThreadId = 0;
        m_dwFlags = 0;
        m_Socket = NULL;
        memset(&m_SocketName,0,sizeof(m_SocketName));
        memset(&m_RemoteSocketName,0,sizeof(m_RemoteSocketName));
        InitializeListHead(&m_AvailableList);
        InitializeListHead(&m_PendingList);
        memset(&m_overlapped,0,sizeof(m_overlapped));
    }

    ~CNetXMO();

    HRESULT Initialize(
        SOCKET Socket,
        PUCHAR pszAddress,
        DWORD dwPort,
        DWORD dwMaxPacketSize,
        DWORD dwFlags);

    STDMETHODIMP_(ULONG) AddRef() {
       return InterlockedIncrement((long*)&m_cRef);
    }

    STDMETHODIMP_(ULONG) Release(); 

    HRESULT STDMETHODCALLTYPE GetInfo( 
        PXMEDIAINFO pInfo
        );
    
    HRESULT STDMETHODCALLTYPE Process( 
        const XMEDIAPACKET __RPC_FAR *pSrcBuffer,
        const XMEDIAPACKET __RPC_FAR *pDstBuffer
        );
    
    HRESULT STDMETHODCALLTYPE Flush()
    {

        //
        // we could flush all pending receive packets here
        // but it involves closing the socket and reopening
        // which isoverhead and creates race conditions
        // Netstream.xbe does not use flush so this was not implemented
        //

        m_bSeqNum = 0;
        return S_OK;
    }

    HRESULT STDMETHODCALLTYPE Discontinuity(void)
    {
        return S_OK;
    }

    HRESULT STDMETHODCALLTYPE GetStatus( 
        DWORD *dwFlags
        );
    
private:

    friend DWORD WINAPI NetXmoThreadProc(
      LPVOID lpParameter   // thread data
    );

    VOID ThreadProc();

    HRESULT
    SubmitRecvRequest(
        const XMEDIAPACKET *pDstBuffer,
        PXMB_CONTEXT pCtx
        );

    VOID
    HandleRecvCompletion(
        const XMEDIAPACKET *pDstBuffer
        );



    HRESULT
    ReturnContext(
        PXMB_CONTEXT pCtx
        );

protected:

    HANDLE m_hThread;
    HANDLE m_hDeleteThreadEvent;
    HANDLE m_hRecvPacketEvent;
    DWORD m_dwThreadId;

    HANDLE m_hMutex;

    ULONG m_cRef;
    DWORD m_dwFlags;

    SOCKET m_Socket;
    struct sockaddr_in m_SocketName;
    struct sockaddr_in m_RemoteSocketName;

    LIST_ENTRY m_AvailableList;
    LIST_ENTRY m_PendingList;

    PUCHAR m_pRecvBuffer;
    PUCHAR m_pSendBuffer;

    WSAOVERLAPPED m_overlapped;
    UCHAR m_bSeqNum;


};

