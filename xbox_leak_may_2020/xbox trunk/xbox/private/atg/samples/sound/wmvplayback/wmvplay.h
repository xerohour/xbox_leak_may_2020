#ifndef _WMVPLAY_H__
#define _WMVPLAY_H__

#include "wmfdecode.h"

#if DBG

extern "C"
{
	ULONG
	DebugPrint(
		PCHAR Format,
		...
		);
}

#define DEBUG_LEVEL_ERROR 0
#define DEBUG_LEVEL_WARNING 1
#define DEBUG_LEVEL_INFO 2
#define DEBUG_LEVEL_SPAM 5

extern ULONG g_WmvDebugLevel;
#define DBG_SPAM(_exp_) {if (g_WmvDebugLevel >= DEBUG_LEVEL_SPAM) DebugPrint _exp_;}
#define DBG_INFO(_exp) {if (g_WmvDebugLevel >= DEBUG_LEVEL_INFO) DebugPrint _exp;}
#define DBG_ERROR(_exp) {if (g_WmvDebugLevel >= DEBUG_LEVEL_ERROR) DebugPrint _exp;}
#define DBG_WARN(_exp) {if (g_WmvDebugLevel >= DEBUG_LEVEL_WARNING) DebugPrint _exp;}

    #define ASSERT(Expression)  \
    {                                                          \
        if(!(Expression))                                      \
        {                                                      \
            DebugPrint("XBOX RIP:\n");               \
            _asm { int 3 };                                    \
        }                                                      \
    }                                   

#else

#define ASSERT(_exp_)
#define DBG_SPAM(_exp_)
#define DBG_INFO(_exp_)
#define DBG_ERROR(_exp_)
#define DBG_WARN(_exp_)

#endif

//
// graphics defines
//

#define NUM_HELP_CALLOUTS 5

//-----------------------------------------------------------------------------
// Globals variables and definitions
//-----------------------------------------------------------------------------

struct PROJTEXVERTEX
{
    D3DXVECTOR3 p;
    D3DCOLOR    color;
};

#define D3DFVF_PROJTEXVERTEX (D3DFVF_XYZ|D3DFVF_DIFFUSE)




//----------------------------------------------------------------------------
// projection frustrum
struct LINEVERTEX
{
    FLOAT x, y, z;
    DWORD color;
};






//
// audio defines
//


//
// Simple macro to verify that the packet size is OK for a particular XMO
//
inline HRESULT XMOFILTER_VERIFY_PACKET_SIZE( XMEDIAINFO& xMediaInfo, DWORD dwPacketSize )
{
    if( xMediaInfo.dwOutputSize > dwPacketSize )
        return E_FAIL;
    if( xMediaInfo.dwOutputSize ) 
        if( dwPacketSize % xMediaInfo.dwOutputSize != 0 )
            return E_FAIL;
    if( dwPacketSize < xMediaInfo.dwMaxLookahead )
        return E_FAIL;

    return S_OK;
}


//
// Linked list structure for tracking our media packet contexts
//
struct LINKED_LIST 
{
    LINKED_LIST* pNext;
    LINKED_LIST* pPrev;

    inline VOID Initialize()         { pNext = pPrev = this; }
    inline BOOL IsListEmpty()        { return pNext == this; }
    inline LINKED_LIST* RemoveHead() { pNext->Remove(); }
    inline LINKED_LIST* RemoveTail() { pPrev->Remove(); }

    inline VOID Remove() 
    {
        LINKED_LIST* pOldNext = pNext;
        LINKED_LIST* pOldPrev = pPrev;
        pOldPrev->pNext = pOldNext;
        pOldNext->pPrev = pOldPrev;
    }

    inline VOID Add( LINKED_LIST* pEntry ) 
    {
        LINKED_LIST* pOldHead = this;
        LINKED_LIST* pOldPrev = this->pPrev;
        pEntry->pNext   = pOldHead;
        pEntry->pPrev   = pOldPrev;
        pOldPrev->pNext = pEntry;
        pOldHead->pPrev = pEntry;
    }
};


//
// We arrived at this packet size based on the requirements of the XMOs.
// The idea is to pick an arbitrary packet size, go through the Init function
// once and then if any asserts fires while verifying the packet sizes, change
// the packet to satisfy all requirements
//
#define PACKET_CNT      0x50
#define PACKET_SIZE     0x1000*4
#define MAXBUFSIZE      (PACKET_SIZE*PACKET_CNT)

// This structure keeps track of our packet status, buffer data, etc.
struct MEDIA_PACKET_CTX
{
    LINKED_LIST ListEntry;
    DWORD       dwStatus;
    DWORD       dwCompletedSize;
    BYTE*       pBuffer;
};


//-----------------------------------------------------------------------------
// Name: class XFilterGraph
//
// Desc: Main class to run this application. Most functionality is inherited
//       from the CXBApplication base class.
//-----------------------------------------------------------------------------
class CFilterGraph : public CXBApplication
{
public:
    XFileMediaObject* m_pSourceXMO;         // Source XMO - the WMA file
    XMediaObject* m_pTargetXMO;             // Target XMO - DSound

    DWORD         m_dwSourceLength;         // Size of source
    DWORD         m_dwBytesRead;            // Bytes of stream processed

    DWORD         m_dwMinPacketSize;        // For verifying our packet size
    DWORD         m_dwPacketSize;           // Packet size
    DWORD         m_dwMaxBufferCount;       // # of buffers

    WAVEFORMATEX  m_wfxAudioFormat;         // Audio format

    LINKED_LIST   m_SourcePendingList;      // Media Contexts in source list
    LINKED_LIST   m_TargetPendingList;      // Media Contexts in target list

    BYTE*         m_pTransferBuffer;        // Buffer data
    BYTE*         m_pPacketContextPool;     // Packet contexts

    CXBFont       m_Font;                   // Font renderer
    DWORD         m_dwFrameCount;           //

    HANDLE        m_hThread;

    // Virtual calls from CXBApplication
    virtual HRESULT Initialize();
    virtual HRESULT FrameMove();
    virtual HRESULT Render();
    virtual HRESULT Cleanup();

    //
    // init calls
    //

    HRESULT InitWmv();
    HRESULT InitGraphics();

    VOID ThreadProc();

    // These functions attach a media context packet to an XMO by 
    // setting up the XMEDIAPACKET struct, calling Process(), and
    // then adding our media packet context to the appropriate list
    HRESULT AttachPacketToSourceXMO( MEDIA_PACKET_CTX* pCtx );
    HRESULT AttachPacketToTargetXMO( MEDIA_PACKET_CTX* pCtx );

    // Handles transferring packets through our filter graph
    HRESULT TransferData();

    //
    // wmv stuff
    //

    AudioStreamInfo m_AudStrm;
    VideoStreamInfo m_VidStrm;
    HWMFDECODER   m_hWmfDecoder;            // WMV decoder
    HANDLE        m_hWmvFile;
    PUCHAR        m_pVidBuf;
    PUCHAR        m_pVidTempBuf;


    //
    // graphics stuff...
    //

    CXBHelp                m_Help;
    BOOL                   m_bDrawHelp;

    LPDIRECT3DTEXTURE8     m_pSpotLightTexture;   // procedurally generated spotlight texture

    D3DXMATRIX             m_matTexProj;   // texture projection matrix

    D3DXVECTOR3            m_vTexEyePt;    // texture eye pt.
    D3DXVECTOR3            m_vTexLookatPt; // texture lookat pt.

    HRESULT CreateSpotLightTexture();
    VOID    ShowTexture( LPDIRECT3DTEXTURE8 pTexture );

    HRESULT GraphicsFrameMove();


};


#endif
