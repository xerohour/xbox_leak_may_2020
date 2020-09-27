
#ifndef _DSPTEST_H_
#define _DSPTEST_H_

#define MAX_FREQUENCY_BINS  512
#define M_PI 3.14159265358979323846

typedef enum {
    DRAW_SPECTRUM = 0,
    DRAW_CYCLES,
    MAX_SCREENS
    } SCREENS;

//
// defines that control what the app does
//

//#define SRC_TEST 1
//#define BREAK_ON_START
#define DOWNLOAD_SCRATCH_IMAGE

#define USE_DSOUND

#define TRACK_TOTAL_GP_USAGE 1


#define DSP_CLOCK_133 1
//#define DSP_CLOCK_160 1
//#define DSP_CLOCK_200 1

//#define ENABLE_DOLBY_DOWNLOAD 1

//*************************************************************

extern HRESULT
DirectSoundLoadEncoder
(
    LPCVOID                 pvImageBuffer, 
    DWORD                   dwImageSize, 
    LPVOID *                ppvScratchData, 
    LPDIRECTSOUND *         ppDirectSound
);

extern HRESULT LoadDolbyCode();
extern HRESULT LoadReverbParameters();
extern HRESULT CreateSineWaveBuffer( double dFrequency, LPDIRECTSOUNDBUFFER8 * ppBuffer );

extern HRESULT SetFXOscillatorParameters(LPDIRECTSOUND pDirectSound,DWORD dwEffectIndex,FLOAT Frequency);

extern HRESULT LoadWaveFile(LPCSTR                  pszFileName,LPCWAVEFORMATEX *       ppwfxFormat,XFileMediaObject **     ppMediaObject);

extern HRESULT
PlayLoopingBuffer
(
    LPCSTR pszFile,
    LPDIRECTSOUNDBUFFER     *pBuffer,
    DWORD dwFlags
);



#ifdef DSP_CLOCK_160 
// 160Mhz
#define MAX_GP_IDLE_CYCLES  106720 // 32 samples at 160Mhz (or 667us*160Mhz)
#define MAX_EP_IDLE_CYCLES  853333 // 256 samples at 160Mhz (or 667us*160Mhz)
#endif

#ifdef DSP_CLOCK_200 
// 200Mhz
#define MAX_GP_IDLE_CYCLES   133333 // 32 samples at 200Mhz
#define MAX_EP_IDLE_CYCLES  1066666 // 256 samples at 200Mhz
#endif

#ifdef DSP_CLOCK_133 
// 133 Mhz
#define MAX_GP_IDLE_CYCLES  88666 // 32 samples at 133Mhz
#define MAX_EP_IDLE_CYCLES  709333 // 256 samples at 133Mhz
#endif

//#define MAX_EP_IDLE_CYCLES  10000

//-----------------------------------------------------------------------------
// Name: class CXBoxSample
// Desc: Main class to run this application. Most functionality is inherited
//       from the CXBApplication base class.
//-----------------------------------------------------------------------------
class CXBoxSample : public CXBApplication
{
public:
    CXBoxSample();

    virtual HRESULT Initialize();
    virtual HRESULT Render();
    virtual HRESULT FrameMove();

    HRESULT RenderSpectrum();
    HRESULT RenderDSPUsage();

    HRESULT FourierTransform();
    HRESULT DownloadScratch(PCHAR pszScratchFile);
    HRESULT UpdateReverb();

    HRESULT VerifySRCEffect(LPDSMIXBINS pDsMixBins);

    // Font and help
    CXBFont     m_Font;
    CXBHelp     m_Help;

    FLOAT       m_fEPCycles;
    FLOAT       m_fEPMinCycles;
    FLOAT       m_fEPMaxCycles;

    DWORD       m_dwDelta;

    FLOAT       m_fGPCycles;
    DWORD       m_dwGPMinCycles;
    DWORD       m_dwGPMaxCycles;
    DWORD       m_dwCount;

    DWORD       m_dwCurrentEnv;
    CHAR        m_szCurrentReverb[256];

    // Draw help?
    BOOL        m_bDrawHelp;
    BOOL        m_bDoDFT;

    DWORD       m_dwScreenSelected;

    HRESULT     m_hOpenResult;
    
    LPDIRECTSOUND m_pDirectSound;
    LPDIRECTSOUNDBUFFER m_pDSBuffer;
    LPDIRECTSOUNDBUFFER m_pOscillatorBuffer0;
    LPDIRECTSOUNDBUFFER m_pOscillatorBuffer1;
    LPDIRECTSOUNDBUFFER m_pOscillatorBuffer2;


    FLOAT               m_fMaxLevels[6];

    //
    // DFT stuff
    //

    LPDSEFFECTIMAGEDESC m_pEffectsImageDesc;
    PDWORD              m_pdwAudioData;
    DWORD               m_dwEffectIndex;

    DOUBLE              m_fMaxMagnitude;

    DOUBLE              m_aFrequencyBins[MAX_FREQUENCY_BINS];
    DOUBLE              m_aMagnitudeBins[MAX_FREQUENCY_BINS];
    DOUBLE              m_aPhaseBins[MAX_FREQUENCY_BINS];

};



#endif
