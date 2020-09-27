
#define _NTDRIVER_

#include <xtl.h>

#undef max
#undef min
#define max(a,b) (((a) > (b)) ? (a) : (b))
#define min(a,b) (((a) < (b)) ? (a) : (b))

#undef  BREAK_INTO_DEBUGGER
#define BREAK_INTO_DEBUGGER __asm int 3


class CEchoXMO:public XMediaObject {

public:
    CEchoXMO()
    {
        m_cRef = 0;
        m_dwDelay = 0;
        m_dwSampleSize = 0;
        m_pPreviousPacket = NULL;
    }

    ~CEchoXMO()
    {
    }

    HRESULT Initialize(
        DWORD dwDelay,
        DWORD dwSampleSize
        );

    STDMETHODIMP_(ULONG) AddRef() {
       return InterlockedIncrement((long*)&m_cRef);
    }

    STDMETHODIMP_(ULONG) Release() {
        long l = InterlockedDecrement((long*)&m_cRef);
     
        if (l == 0)
           delete this;
        return l;
    }

    HRESULT STDMETHODCALLTYPE GetInfo( 
        PXMEDIAINFO pInfo
        );
    
    HRESULT STDMETHODCALLTYPE Process( 
        const XMEDIAPACKET __RPC_FAR *pSrcBuffer,
        const XMEDIAPACKET __RPC_FAR *pDstBuffer
        );
    
    HRESULT STDMETHODCALLTYPE Flush()
    {
        return S_OK;
    }

    HRESULT STDMETHODCALLTYPE Discontinuity(void)
    {
        return S_OK;
    }

    HRESULT STDMETHODCALLTYPE GetStatus( 
        DWORD *dwFlags
        );
    

protected:

    PSHORT m_pPreviousPacket;
    ULONG m_cRef;
    DWORD m_dwDelay;
    DWORD m_dwSampleSize;


};

