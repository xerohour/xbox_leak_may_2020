
#if !defined _GPDSPMANAGER_
#define _GPDSPMANAGER_


typedef struct
{
    union
    {
        struct
        {
            ULONG oper : 1;
            ULONG res  : 15;
            ULONG size : 16;
        } fields;

        ULONG uVal;
    } data1;

    ULONG offset;

} AC3_SET_BUFFER;



///////////////////////////////////////////////////////////////////////////////
//
// Dsp program base class
//
///////////////////////////////////////////////////////////////////////////////


class CMcpxDspImageTest
{
public:
    CMcpxDspImageTest()
    {
        m_pLoader = NULL;
        m_uLoaderSize = 0;
    }
    
    VOID Initialize();

    PVOID       GetLoader() { return m_pLoader; }
    ULONG       GetLoaderSize() { return m_uLoaderSize; }    

protected:
    
    PVOID        m_pLoader;
    ULONG        m_uLoaderSize;

};




class CMcpxGPDspManagerTest
{
public:

    CMcpxGPDspManagerTest();
    ~CMcpxGPDspManagerTest();

public:
    VOID Initialize();

    // debug only
    VOID *GetScratchPage(ULONG uPageNumber);
    VOID AC3SetOutputBuffer(PMCPX_ALLOC_CONTEXT pMem, ULONG uNumOfPages);
    VOID SetMultipassBuffer(PMCPX_ALLOC_CONTEXT pMem, ULONG uNumOfPages);

    HRESULT DownloadEffectsImage(LPCVOID pvImageBuffer, DWORD dwImageSize, LPDSEFFECTIMAGEDESC *ppImageDesc);
    HRESULT SetEffectData(DWORD dwEffectIndex, DWORD dwOffset, LPCVOID pvData, DWORD dwDataSize, DWORD dwFlags);
    HRESULT GetEffectData(DWORD dwEffectIndex, DWORD dwOffset, LPVOID pvData, DWORD dwDataSize);
    HRESULT GetEffectMap(DWORD dwEffectIndex, LPCDSEFFECTMAP *ppEffectMap);
    VOID CommitChanges(DWORD dwScratchOffset, DWORD dwDataSize);

    VOID RestoreCommandBlock();

protected:

    CMcpxDspScratchDmaTest *GetScratchDma() { return m_pScratchDma; }
    HRESULT     ParseEffectImageInfo(LPCVOID pScratchImage);

private:

    ULONG        m_uAC3BufferOffset;
    ULONG        m_uMultipassBufferOffset;

    CMcpxDspScratchDmaTest     *m_pScratchDma;
    CMcpxDspImageTest          *m_pDspImage; 

    ULONG        m_uPMemOffset;
    ULONG        m_uPMemMaxSize;

    DWORD m_dwCurrentLowestScratchOffset;
    DWORD m_dwStateSizeToCommit;
    LPDSEFFECTIMAGEDESC m_pFxDescriptor;
    HOST_TO_DSP_COMMANDBLOCK m_InitialCmdBlock;

};

#endif




