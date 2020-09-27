
#define MAX_SOUNDBANKS  5
#define MAX_WAVEBANKS  5
#define MAX_SOUNDSOURCES  5
#define MAX_CUES_PER_BANK  5

typedef struct _SOUND_BANK_CONTEXT {

    DWORD dwNumSoundCues;
    PXACTSOUNDBANK pSoundBank;
    PXACTSOUNDCUE paSoundCues[MAX_CUES_PER_BANK];
    PVOID   pvSoundBankData;

} SOUND_BANK_CONTEXT, *PSOUND_BANK_CONTEXT;

typedef struct _WAVE_BANK_CONTEXT {

    PXACTWAVEBANK pWaveBank;
    PVOID   pvWaveBankData;
    DWORD   dwSize;

} WAVE_BANK_CONTEXT, *PWAVE_BANK_CONTEXT;


class CXactTest {
public:

    CXactTest();
    ~CXactTest();

    HRESULT Initialize(DWORD dwNumSoundBanks,
        DWORD dwNumWaveBanks,
        DWORD dwNumSoundSources
        );

    //
    // tests
    //

    HRESULT BasicPlayStop();
    HRESULT RuntimeEvents();
    HRESULT Notifications();

    VOID    ThreadProc();
    VOID    LinkAll();

private:

    PXACTENGINE             m_pEngine;
    PVOID                   m_pvDspImage;
    DWORD                   m_dwNumSoundSources;
    DWORD                   m_dwNumSoundBanks;
    DWORD                   m_dwNumWaveBanks;


    SOUND_BANK_CONTEXT      m_aSoundBanks[MAX_SOUNDBANKS];
    PXACTSOUNDSOURCE        m_paSoundSources[MAX_SOUNDSOURCES];
    PXACTSOUNDCUE           m_paSoundCues[MAX_SOUNDSOURCES];
    WAVE_BANK_CONTEXT       m_aWaveBanks[MAX_WAVEBANKS];    
   
    HANDLE                  m_hThread;
    DWORD                   m_dwThreadId;
public:
    BOOL                    m_bTestNotifications;
    BOOL                    m_bTestBasicPlayStop;
    BOOL                    m_bTestRuntimeEvents;
    BOOL                    m_bTestPrematureWavebankUnregister;
};


HRESULT LoadBinaryData(const CHAR *pszName, PVOID *ppvData, PDWORD pdwSize);
VOID CreateRandomEvent(PXACT_TRACK_EVENT pEvent);

DWORD WINAPI XactTestThreadProc(
    LPVOID pParameter
    );

