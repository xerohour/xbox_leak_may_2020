#if DBG

#define MY_ASSERT(x) if( !(x) ) _asm int 3;

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

extern ULONG g_VdXmoDebugLevel;
#define DBG_SPAM(_exp_) {if (g_VdXmoDebugLevel >= DEBUG_LEVEL_SPAM) DebugPrint _exp_;}
#define DBG_INFO(_exp) {if (g_VdXmoDebugLevel >= DEBUG_LEVEL_INFO) DebugPrint _exp;}
#define DBG_ERROR(_exp) {if (g_VdXmoDebugLevel >= DEBUG_LEVEL_ERROR) DebugPrint _exp;}
#define DBG_WARN(_exp) {if (g_VdXmoDebugLevel >= DEBUG_LEVEL_WARNING) DebugPrint _exp;}

#else

#define MY_ASSERT(x)
#define DBG_SPAM(_exp_)
#define DBG_INFO(_exp_)
#define DBG_ERROR(_exp_)
#define DBG_WARN(_exp_)

#endif


class CVoiceDecoderXmo: public XMediaObject
{
protected:
    DWORD m_dwMinOutputSize;
    DWORD m_dwMinInputSize;
    DWORD m_dwCurrentSeqNum;

    LPXMEDIAOBJECT m_pDecoderXmo;

    DWORD m_cRef;

public:

	CVoiceDecoderXmo() 
        : m_dwMinInputSize(sizeof(VOICE_ENCODER_HEADER))
        , m_dwMinOutputSize(0)
        , m_cRef(0)
        , m_dwCurrentSeqNum(0)
		{};

	~CVoiceDecoderXmo();
	
    STDMETHODIMP_(ULONG) AddRef() {
       return InterlockedIncrement((long*)&m_cRef);
    }

    STDMETHODIMP_(ULONG) Release() {
       long l = InterlockedDecrement((long*)&m_cRef);
       if (l == 0)
          delete this;
       return l;
    }

    HRESULT STDMETHODCALLTYPE Process( 
        LPCXMEDIAPACKET pSrcBuffer,
        LPCXMEDIAPACKET pDstBuffer
        );
    
	HRESULT STDMETHODCALLTYPE GetInfo(
        LPXMEDIAINFO pInfo
		);

    HRESULT STDMETHODCALLTYPE Flush();
    HRESULT STDMETHODCALLTYPE Discontinuity(void);

    HRESULT STDMETHODCALLTYPE GetStatus( 
        LPDWORD pdwStatus 
		);


	HRESULT Init(
        DWORD dwCodecTag,
        PWAVEFORMATEX pwfx);

private:

};

