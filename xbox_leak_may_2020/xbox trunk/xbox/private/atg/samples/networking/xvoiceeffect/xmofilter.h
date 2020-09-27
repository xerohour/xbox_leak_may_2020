#ifndef XMOFILTER_H_
#define XMOFILTER_H_

// Simple macro to verify that the packet size is OK for a particular XMO
inline HRESULT XMOFILTER_VERIFY_PACKET_SIZE( XMEDIAINFO& xMediaInfo, DWORD dwPacketSize )
{
    if( xMediaInfo.dwOutputSize > dwPacketSize )
        return E_FAIL;
    if( xMediaInfo.dwOutputSize ) 
        if( dwPacketSize%xMediaInfo.dwOutputSize != 0 )
            return E_FAIL;
    if( dwPacketSize < xMediaInfo.dwMaxLookahead )
        return E_FAIL;

    return S_OK;
}




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




// We arrived at this value based on the requirements of the XMOs.
// The idea is to pick an arbitrary packet size, go through the Init function
// once and then if any asserts fires while verifying the packet sizes, change
// the packet to satisfy all requirements
#define PACKET_CNT      10
#define PACKET_SIZE     0x140
#define MAXBUFSIZE      (PACKET_SIZE*PACKET_CNT)


struct MEDIA_PACKET_CTX
{
    LINKED_LIST ListEntry;
    DWORD       dwStatus;
    DWORD       dwCompletedSize;
    BYTE*       pBuffer;
};

#define INVALID_PORT        0xFFFFFFFF

class CFilterGraph
{
public:
    XMediaObject* m_pSourceXMO;
    XMediaObject* m_pIntermediateXMO;
    XMediaObject* m_pTargetXMO;

    DWORD         m_dwMinPacketSize;
    DWORD         m_dwPacketSize;
    DWORD         m_dwMaxBufferCount;

    WAVEFORMATEX  m_wfxAudioFormat;

    LINKED_LIST   m_SourcePendingList;
    LINKED_LIST   m_TargetPendingList;

    BYTE*         m_pTransferBuffer;
    BYTE*         m_pPacketContextPool;

    ULONGLONG     m_dwReserved;

    HRESULT Init(PWAVEFORMATEX pwfx,
                               XMediaObject *pSourceXmo,
                               XMediaObject *pIntermediateXmo,
                               XMediaObject *pTargetXmo);

    VOID    AttachPacketToSourceXMO( MEDIA_PACKET_CTX* pCtx );
    VOID    AttachPacketToTargetXMO( MEDIA_PACKET_CTX* pCtx );
    HRESULT TransferData();
};



#endif
