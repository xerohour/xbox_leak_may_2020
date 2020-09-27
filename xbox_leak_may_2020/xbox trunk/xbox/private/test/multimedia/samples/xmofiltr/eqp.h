
#define _NTDRIVER_

#include <xtl.h>
#include <xdbg.h>

#undef max
#undef min
#define max(a,b) (((a) > (b)) ? (a) : (b))
#define min(a,b) (((a) < (b)) ? (a) : (b))

#undef  BREAK_INTO_DEBUGGER
#define BREAK_INTO_DEBUGGER __asm int 3


#define EQ_XMO_REQUIRED_SAMPLES 4 //(2 samples stereo)

class CEQXMO:public XMediaObject {

public:
    CEQXMO()
    {
        m_cRef = 0;
        memset(&m_wfx,0,sizeof(WAVEFORMATEX));
        memset(m_PreviousSamples, 0, EQ_XMO_REQUIRED_SAMPLES);
    }

    ~CEQXMO()
    {
    }

    HRESULT Initialize(
        PWAVEFORMATEX pwfx,
        float fCutoff,
        float fResonance
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
    
    //
    // these methods are specific to this filter
    //

    HRESULT STDMETHODCALLTYPE SetParameters( 
        float fCutoff,
        float fResonance
        );

    VOID ChamberlinFilter( 
        PSHORT srcBuffer,
        PSHORT dstBuffer,
        ULONG nStep,
        ULONG nLength,
        ULONG nOffset
        );

protected:

    SHORT m_PreviousSamples[EQ_XMO_REQUIRED_SAMPLES];
    ULONG m_cRef;

    WAVEFORMATEX m_wfx;

    //
    // chamberlin filter coefficients
    //

    unsigned int m_F;
    unsigned int m_Q;


};

