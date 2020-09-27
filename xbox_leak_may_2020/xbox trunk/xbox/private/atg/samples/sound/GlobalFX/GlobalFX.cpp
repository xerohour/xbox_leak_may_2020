//-----------------------------------------------------------------------------
// File: GlobalFX.cpp
//
// Desc: The GlobalFX sample demonstrates how to create audio effects using
//       the DSP.  It also demonstrates modifying effect parameters via the
//       SetEffectData calll
//
// Hist: 5.29.01 - New for July XDK
//      10.15.01 - Updated DSP image handling.
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------

#include <XBApp.h>
#include <XBFont.h>
#include <XBHelp.h>
#include <XBSound.h>
#include <dsound.h>
#include <xgraphics.h>
#include <stddef.h>
#include <assert.h>
#include "image.h"
#include "dsfxparm.h"

//-----------------------------------------------------------------------------
// Callouts for labelling the gamepad on the help screen
//-----------------------------------------------------------------------------
XBHELP_CALLOUT g_HelpCallouts[] = 
{
    { XBHELP_BACK_BUTTON,  XBHELP_PLACEMENT_2, L"Display help" },
    { XBHELP_A_BUTTON,     XBHELP_PLACEMENT_2, L"Toggle playback" },
    { XBHELP_B_BUTTON,     XBHELP_PLACEMENT_2, L"Change sound" },
    { XBHELP_X_BUTTON,     XBHELP_PLACEMENT_2, L"Next effect\nchain" },
    { XBHELP_BLACK_BUTTON, XBHELP_PLACEMENT_2, L"Increase volume" },
    { XBHELP_WHITE_BUTTON, XBHELP_PLACEMENT_2, L"Decrease volume" },
    { XBHELP_RIGHTSTICK,   XBHELP_PLACEMENT_2, L"Modify parameter value" },
    { XBHELP_LEFTSTICK,    XBHELP_PLACEMENT_2, L"Modify oscillator(DSP)\nfrequency" },
    { XBHELP_DPAD,         XBHELP_PLACEMENT_2, L"Select parameter" },
};

#define NUM_HELP_CALLOUTS 9

// The GP DSP uses different mixbin constant definitions than DirectSound does
// These are used for re-routing DSP effects on the fly
#define DSPMIXBIN_FRONT_LEFT    0x000C00
#define DSPMIXBIN_FRONT_RIGHT   0x000C20
#define DSPMIXBIN_FRONT_CENTER  0x000C40
#define DSPMIXBIN_LOW_FREQUENCY 0x000C60
#define DSPMIXBIN_BACK_LEFT     0x000C80
#define DSPMIXBIN_BACK_RIGHT    0x000CA0
#define DSPMIXBIN_FXSEND_0      0x000D60
#define DSPMIXBIN_FXSEND_1      0x000D80
#define DSPMIXBIN_FXSEND_2      0x000DA0
#define DSPMIXBIN_FXSEND_3      0x000DC0
#define DSPMIXBIN_FXSEND_4      0x000DE0
#define DSPMIXBIN_FXSEND_5      0x000E00
#define DSPMIXBIN_FXSEND_6      0x000E20
#define DSPMIXBIN_FXSEND_7      0x000E40
#define DSPMIXBIN_FXSEND_8      0x000E60
#define DSPMIXBIN_FXSEND_9      0x000E80
#define DSPMIXBIN_FXSEND_10     0x000EA0
#define DSPMIXBIN_FXSEND_11     0x000EC0
#define DSPMIXBIN_FXSEND_12     0x000EE0
#define DSPMIXBIN_FXSEND_13     0x000F00
#define DSPMIXBIN_FXSEND_14     0x000F20
#define DSPMIXBIN_FXSEND_15     0x000F40
#define DSPMIXBIN_FXSEND_16     0x000F60
#define DSPMIXBIN_FXSEND_17     0x000F80
#define DSPMIXBIN_FXSEND_18     0x000FA0
#define DSPMIXBIN_FXSEND_19     0x000FC0


// These constants define how we want our DSP oscillator to operate
#define OSCILLATOR_SCALE    100
#define MAX_OSCILLATOR_FREQUENCY 30
#define MIN_OSCILLATOR_FREQUENCY 0.01

// Utility function for updating parameters of the DSP oscillator effect
HRESULT SetFxOscillatorParameters(LPDIRECTSOUND pDirectSound,DWORD dwEffectIndex,DOUBLE Frequency);

// Parameter type enum
typedef enum {
    PARAM_DWORD,
    PARAM_FLOAT,
} PARAM_TYPE;

// Parameter struct definition
typedef struct {
    WCHAR * szDescription;
    PARAM_TYPE ef;
    DWORD   dwEffectIndex;
    void *  pvFXParam;
    DWORD   dwOffset;
    FLOAT   fValue;
    FLOAT   fMin;
    FLOAT   fMax;
} EFFECTPARAM;

// Output mixbin struct definition
typedef struct {
    DWORD dwEffectIndex;
    DWORD dwOutputOffset;
} EFFECTOUTPUT;

// Struct definition for describing effect chains
typedef struct {
    WCHAR*          szDescription;
    DWORD           dwMixBinCount;
    DWORD           dwMixBins[ DSMIXBIN_ASSIGNMENT_MAX ];
    DWORD           dwNumParams;
    EFFECTPARAM*    pEffectParams;
    DWORD           dwNumOutputs;
    EFFECTOUTPUT*   pOutputs;
} OPTION_STRUCT;


// Echo effect parameters
DSFX_ECHO_STEREO_PARAMS g_fxEcho;
EFFECTPARAM g_aEchoParams[] =
{
    { L"Gain",                                                                  // Description
      PARAM_FLOAT,                                                              // Value type
      ECHO_CHAIN_ECHO,                                                          // Effect index
      &g_fxEcho,                                                                // pvFXParam
      offsetof( DSFX_ECHO_STEREO_PARAMS, dwGain),                               // Offset
      0.0f,                                                                     // Value
      0.0f,                                                                     // Min
      1.0f }                                                                    // Max

};

// Echo output mixbins
EFFECTOUTPUT g_aEchoOutputs[] =
{
    { ECHO_CHAIN_ECHO, offsetof( DSFX_ECHO_STEREO_STATE, dwOutMixbinPtrs[0] ) },
    { ECHO_CHAIN_ECHO, offsetof( DSFX_ECHO_STEREO_STATE, dwOutMixbinPtrs[1] ) }
};


// Distortion effect parameters
DSFX_DISTORTION_PARAMS g_fxDistortionL;
DSFX_DISTORTION_PARAMS g_fxDistortionR;
EFFECTPARAM g_aDistortionParams[] =
{
    { L"Gain L",                                                                // Description
      PARAM_FLOAT,                                                              // Value type
      DISTORTION_CHAIN_DISTORTION0,                                             // Effect index
      &g_fxDistortionL,                                                         // pvFXParam
      offsetof( DSFX_DISTORTION_PARAMS, dwGain ),                               // Offset
      0.0f,                                                                     // Value
      0.0f,                                                                     // Min
      1.0f },                                                                   // Max

    { L"PreFilter B0 L",                                                        // Description
      PARAM_FLOAT,                                                              // Value type
      DISTORTION_CHAIN_DISTORTION0,                                             // Effect index
      &g_fxDistortionL,                                                         // pvFXParam
      offsetof( DSFX_DISTORTION_PARAMS, dwPreFilterB0 ),                        // Offset
      0.0f,                                                                     // Value
      -1.0f,                                                                    // Min
      1.0f },                                                                   // Max

    { L"PreFilter B1 L",                                                        // Description
      PARAM_FLOAT,                                                              // Value type
      DISTORTION_CHAIN_DISTORTION0,                                             // Effect index
      &g_fxDistortionL,                                                         // pvFXParam
      offsetof( DSFX_DISTORTION_PARAMS, dwPreFilterB1 ),                        // Offset
      0.0f,                                                                     // Value
      -1.0f,                                                                    // Min
      1.0f },                                                                   // Max

    { L"PreFilter B2 L",                                                        // Description
      PARAM_FLOAT,                                                              // Value type
      DISTORTION_CHAIN_DISTORTION0,                                             // Effect index
      &g_fxDistortionL,                                                         // pvFXParam
      offsetof( DSFX_DISTORTION_PARAMS, dwPreFilterB2 ),                        // Offset
      0.0f,                                                                     // Value
      -1.0f,                                                                    // Min
      1.0f },                                                                   // Max

    { L"PreFilter A1 L",                                                        // Description
      PARAM_FLOAT,                                                              // Value type
      DISTORTION_CHAIN_DISTORTION0,                                             // Effect index
      &g_fxDistortionL,                                                         // pvFXParam
      offsetof( DSFX_DISTORTION_PARAMS, dwPreFilterA1 ),                        // Offset
      0.0f,                                                                     // Value
      -1.0f,                                                                    // Min
      1.0f },                                                                   // Max

    { L"PreFilter A2 L",                                                        // Description
      PARAM_FLOAT,                                                              // Value type
      DISTORTION_CHAIN_DISTORTION0,                                             // Effect index
      &g_fxDistortionL,                                                         // pvFXParam
      offsetof( DSFX_DISTORTION_PARAMS, dwPreFilterA2 ),                        // Offset
      0.0f,                                                                     // Value
      -1.0f,                                                                    // Min
      1.0f },                                                                   // Max

    { L"PostFilter B0 L",                                                       // Description
      PARAM_FLOAT,                                                              // Value type
      DISTORTION_CHAIN_DISTORTION0,                                             // Effect index
      &g_fxDistortionL,                                                         // pvFXParam
      offsetof( DSFX_DISTORTION_PARAMS, dwPostFilterB0 ),                       // Offset
      0.0f,                                                                     // Value
      -1.0f,                                                                    // Min
      1.0f },                                                                   // Max

    { L"PostFilter B1 L",                                                       // Description
      PARAM_FLOAT,                                                              // Value type
      DISTORTION_CHAIN_DISTORTION0,                                             // Effect index
      &g_fxDistortionL,                                                         // pvFXParam
      offsetof( DSFX_DISTORTION_PARAMS, dwPostFilterB1 ),                       // Offset
      0.0f,                                                                     // Value
      -1.0f,                                                                    // Min
      1.0f },                                                                   // Max

    { L"PostFilter B2 L",                                                       // Description
      PARAM_FLOAT,                                                              // Value type
      DISTORTION_CHAIN_DISTORTION0,                                             // Effect index
      &g_fxDistortionL,                                                         // pvFXParam
      offsetof( DSFX_DISTORTION_PARAMS, dwPostFilterB2 ),                       // Offset
      0.0f,                                                                     // Value
      -1.0f,                                                                    // Min
      1.0f },                                                                   // Max

    { L"PostFilter A1 L",                                                       // Description
      PARAM_FLOAT,                                                              // Value type
      DISTORTION_CHAIN_DISTORTION0,                                             // Effect index
      &g_fxDistortionL,                                                         // pvFXParam
      offsetof( DSFX_DISTORTION_PARAMS, dwPostFilterA1 ),                       // Offset
      0.0f,                                                                     // Value
      -1.0f,                                                                    // Min
      1.0f },                                                                   // Max

    { L"PostFilter A2 L",                                                       // Description
      PARAM_FLOAT,                                                              // Value type
      DISTORTION_CHAIN_DISTORTION0,                                             // Effect index
      &g_fxDistortionL,                                                         // pvFXParam
      offsetof( DSFX_DISTORTION_PARAMS, dwPostFilterA2 ),                       // Offset
      0.0f,                                                                     // Value
      -1.0f,                                                                    // Min
      1.0f },                                                                   // Max

    { L"Gain R",                                                                // Description
      PARAM_FLOAT,                                                              // Value type
      DISTORTION_CHAIN_DISTORTION1,                                             // Effect index
      &g_fxDistortionR,                                                         // pvFXParam
      offsetof( DSFX_DISTORTION_PARAMS, dwGain ),                               // Offset
      0.0f,                                                                     // Value
      0.0f,                                                                     // Min
      1.0f },                                                                   // Max

    { L"PreFilter B0 R",                                                        // Description
      PARAM_FLOAT,                                                              // Value type
      DISTORTION_CHAIN_DISTORTION1,                                             // Effect index
      &g_fxDistortionR,                                                         // pvFXParam
      offsetof( DSFX_DISTORTION_PARAMS, dwPreFilterB0 ),                        // Offset
      0.0f,                                                                     // Value
      -1.0f,                                                                    // Min
      1.0f },                                                                   // Max

    { L"PreFilter B1 R",                                                        // Description
      PARAM_FLOAT,                                                              // Value type
      DISTORTION_CHAIN_DISTORTION1,                                             // Effect index
      &g_fxDistortionR,                                                         // pvFXParam
      offsetof( DSFX_DISTORTION_PARAMS, dwPreFilterB1 ),                        // Offset
      0.0f,                                                                     // Value
      -1.0f,                                                                    // Min
      1.0f },                                                                   // Max

    { L"PreFilter B2 R",                                                        // Description
      PARAM_FLOAT,                                                              // Value type
      DISTORTION_CHAIN_DISTORTION1,                                             // Effect index
      &g_fxDistortionR,                                                         // pvFXParam
      offsetof( DSFX_DISTORTION_PARAMS, dwPreFilterB2 ),                        // Offset
      0.0f,                                                                     // Value
      -1.0f,                                                                    // Min
      1.0f },                                                                   // Max

    { L"PreFilter A1 R",                                                        // Description
      PARAM_FLOAT,                                                              // Value type
      DISTORTION_CHAIN_DISTORTION1,                                             // Effect index
      &g_fxDistortionR,                                                         // pvFXParam
      offsetof( DSFX_DISTORTION_PARAMS, dwPreFilterA1 ),                        // Offset
      0.0f,                                                                     // Value
      -1.0f,                                                                    // Min
      1.0f },                                                                   // Max

    { L"PreFilter A2 R",                                                        // Description
      PARAM_FLOAT,                                                              // Value type
      DISTORTION_CHAIN_DISTORTION1,                                             // Effect index
      &g_fxDistortionR,                                                         // pvFXParam
      offsetof( DSFX_DISTORTION_PARAMS, dwPreFilterA2 ),                        // Offset
      0.0f,                                                                     // Value
      -1.0f,                                                                    // Min
      1.0f },                                                                   // Max

    { L"PostFilter B0 R",                                                       // Description
      PARAM_FLOAT,                                                              // Value type
      DISTORTION_CHAIN_DISTORTION1,                                             // Effect index
      &g_fxDistortionR,                                                         // pvFXParam
      offsetof( DSFX_DISTORTION_PARAMS, dwPostFilterB0 ),                       // Offset
      0.0f,                                                                     // Value
      -1.0f,                                                                    // Min
      1.0f },                                                                   // Max

    { L"PostFilter B1 R",                                                       // Description
      PARAM_FLOAT,                                                              // Value type
      DISTORTION_CHAIN_DISTORTION1,                                             // Effect index
      &g_fxDistortionR,                                                         // pvFXParam
      offsetof( DSFX_DISTORTION_PARAMS, dwPostFilterB1 ),                       // Offset
      0.0f,                                                                     // Value
      -1.0f,                                                                    // Min
      1.0f },                                                                   // Max

    { L"PostFilter B2 R",                                                       // Description
      PARAM_FLOAT,                                                              // Value type
      DISTORTION_CHAIN_DISTORTION1,                                             // Effect index
      &g_fxDistortionR,                                                         // pvFXParam
      offsetof( DSFX_DISTORTION_PARAMS, dwPostFilterB2 ),                       // Offset
      0.0f,                                                                     // Value
      -1.0f,                                                                    // Min
      1.0f },                                                                   // Max

    { L"PostFilter A1 R",                                                       // Description
      PARAM_FLOAT,                                                              // Value type
      DISTORTION_CHAIN_DISTORTION1,                                             // Effect index
      &g_fxDistortionR,                                                         // pvFXParam
      offsetof( DSFX_DISTORTION_PARAMS, dwPostFilterA1 ),                       // Offset
      0.0f,                                                                     // Value
      -1.0f,                                                                    // Min
      1.0f },                                                                   // Max

    { L"PostFilter A2 R",                                                       // Description
      PARAM_FLOAT,                                                              // Value type
      DISTORTION_CHAIN_DISTORTION1,                                             // Effect index
      &g_fxDistortionR,                                                         // pvFXParam
      offsetof( DSFX_DISTORTION_PARAMS, dwPostFilterA2 ),                       // Offset
      0.0f,                                                                     // Value
      -1.0f,                                                                    // Min
      1.0f },                                                                   // Max
};

// Distortion output mixbins
EFFECTOUTPUT g_aDistortionOutputs[] =
{
    { DISTORTION_CHAIN_DISTORTION0, offsetof( DSFX_DISTORTION_STATE, dwOutMixbinPtrs[0] ) },
    { DISTORTION_CHAIN_DISTORTION1, offsetof( DSFX_DISTORTION_STATE, dwOutMixbinPtrs[1] ) }
};


// Chorus effect parameters
DSFX_CHORUS_STEREO_PARAMS g_fxChorus;
EFFECTPARAM g_aChorusParams[] =
{
    { L"Gain",                                                                  // Description
      PARAM_FLOAT,                                                              // Value type
      CHORUS_CHAIN_CHORUS,                                                      // Effect index
      &g_fxChorus,                                                              // pvFXParam
      offsetof( DSFX_CHORUS_STEREO_PARAMS, dwGain),                             // Offset
      0.0f,                                                                     // Value
      0.0f,                                                                     // Min
      1.0f },                                                                   // Max

    { L"ModScale",                                                              // Description
      PARAM_DWORD,                                                              // Value type
      CHORUS_CHAIN_CHORUS,                                                      // Effect index
      &g_fxChorus,                                                              // pvFXParam
      offsetof( DSFX_CHORUS_STEREO_PARAMS, dwModScale),                         // Offset
      0.0f,                                                                     // Value
      0.0f,                                                                     // Min
      200.0f }                                                                    // Max
};

// Chorus output mixbins
EFFECTOUTPUT g_aChorusOutputs[] =
{
    { CHORUS_CHAIN_CHORUS, offsetof( DSFX_CHORUS_STEREO_STATE, dwOutMixbinPtrs[0] ) },
    { CHORUS_CHAIN_CHORUS, offsetof( DSFX_CHORUS_STEREO_STATE, dwOutMixbinPtrs[1] ) }
};


// AmpMod->Flange effect paramaters
DSFX_FLANGE_STEREO_PARAMS g_fxFlange;
EFFECTPARAM g_aAmpModFlangeParams[] =
{
    { L"Flange Feedback",                                                       // Description
      PARAM_FLOAT,                                                              // Value type
      AMPMOD_FLANGE_CHAIN_FLANGE,                                               // Effect index
      &g_fxFlange,                                                              // pvFXParam
      offsetof( DSFX_FLANGE_STEREO_PARAMS, dwFeedback ),                        // Offset
      0.0f,                                                                     // Value
      0.0f,                                                                     // Min
      1.0f },                                                                   // Max

    { L"Flange Scale",                                                          // Description
      PARAM_DWORD,                                                              // Value type
      AMPMOD_FLANGE_CHAIN_FLANGE,                                               // Effect index
      &g_fxFlange,                                                              // pvFXParam
      offsetof( DSFX_FLANGE_STEREO_PARAMS, dwModScale ),                        // Offset
      0.0f,                                                                     // Value
      0.0f,                                                                     // Min
      100.0f },                                                                 // Max
};

// AmpMod->Flange output mixbins
EFFECTOUTPUT g_aAmpModFlangeOutputs[] =
{
    { AMPMOD_FLANGE_CHAIN_FLANGE, offsetof( DSFX_FLANGE_STEREO_STATE, dwOutMixbinPtrs[0] ) },
    { AMPMOD_FLANGE_CHAIN_FLANGE, offsetof( DSFX_FLANGE_STEREO_STATE, dwOutMixbinPtrs[1] ) }
};


typedef enum {

    EFFECT_NONE = 0,
    EFFECT_ECHO,
    EFFECT_DISTORTION,
    EFFECT_CHORUS,
    EFFECT_AMPMOD_FLANGE,
    MAX_CHAINS

} EFFECT_CHAINS;


// List of all effect chains, mixbins, and parameters
OPTION_STRUCT g_aOptions[] =
{
//      Name, # of bins, mixbin assignments, # params, param structure, # outputs, output struct
    { L"None",              6, { DSMIXBIN_FRONT_LEFT, DSMIXBIN_FRONT_RIGHT, DSMIXBIN_FRONT_CENTER, DSMIXBIN_BACK_LEFT, DSMIXBIN_BACK_RIGHT, DSMIXBIN_LOW_FREQUENCY }, 0, NULL, 0, NULL },
    { L"Echo",              2, { DSMIXBIN_FXSEND_0, DSMIXBIN_FXSEND_1 }, 1, g_aEchoParams, 2, g_aEchoOutputs },
    { L"Distortion",        2, { DSMIXBIN_FXSEND_2, DSMIXBIN_FXSEND_3 }, 22, g_aDistortionParams, 2, g_aDistortionOutputs },
    { L"Chorus",            2, { DSMIXBIN_FXSEND_4, DSMIXBIN_FXSEND_5 }, 2, g_aChorusParams, 2, g_aChorusOutputs },
    { L"AmpMod->Flange",    2, { DSMIXBIN_FXSEND_6, DSMIXBIN_FXSEND_7 }, 2, g_aAmpModFlangeParams, 2, g_aAmpModFlangeOutputs },
};


// List of wav files to cycle through
char * g_aszFileNames[] = 
{
    "D:\\Media\\Sounds\\heli.wav",
    "D:\\Media\\Sounds\\DockingMono.wav",
    "D:\\Media\\Sounds\\EngineStartMono.wav",
    "D:\\Media\\Sounds\\MaleDialog1.wav",
    "D:\\Media\\Sounds\\MiningMono.wav",
    "D:\\Media\\Sounds\\MusicMono.wav",
    "D:\\Media\\Sounds\\Dolphin4.wav",
};

#define NUM_SOUNDS 7

#define PARAM_DISPLAY_LENGTH 7

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

    // Utility function for creating a sine wave buffer
    HRESULT CreateSineWaveBuffer( double dFrequency, LPDIRECTSOUNDBUFFER8 * ppBuffer );

    HRESULT InitializeEffectParameters();           // Loads effect parameters
    HRESULT NextFilterGraph();                      // Switch to next effect

    // Font and help
    CXBFont     m_Font;
    CXBHelp     m_Help;
    BOOL        m_bDrawHelp;


    LPDIRECTSOUND8          m_pDSound;              // DirectSound object
    CWaveFile               m_awfSounds[NUM_SOUNDS];// Wave file parsers
    DWORD                   m_dwCurrent;            // Current sound
    BOOL                    m_bPlaying;             // Are we playing?
    LONG                    m_lVolume;              // Current volume
    LPDIRECTSOUNDBUFFER8    m_pDSBuffer;            // DirectSoundBuffer
    BYTE *                  m_pbSampleData;         // Sample data from wav
    DWORD                   m_dwEffect;             // Current effect graph
    DWORD                   m_dwParam;              // Current effect param
    DWORD                   m_dwFirstDisplayed;     // First displayed param
    LPDIRECTSOUNDBUFFER8    m_pOscillator;          // Host buffer used as Oscillator

    DOUBLE                  m_fOscillatorFrequency; // frequency of DSP fx oscillator

    HRESULT SwitchToSound( DWORD dwIndex );         // Sets up a different sound
    HRESULT DownloadEffectsImage(PCHAR pszScratchFile);  // downloads a default DSP image to the GP
};



//-----------------------------------------------------------------------------
// Name: main()
// Desc: Entry point to the program.
//-----------------------------------------------------------------------------
VOID __cdecl main()
{
    CXBoxSample xbApp;
    if( FAILED( xbApp.Create() ) )
        return;
    xbApp.Run();
}



//-----------------------------------------------------------------------------
// Name: CXBoxSample (constructor)
// Desc: Constructor for CXBoxSample class
//-----------------------------------------------------------------------------
CXBoxSample::CXBoxSample() 
            :CXBApplication()
{
    m_bDrawHelp = FALSE;
    m_fOscillatorFrequency = 1;

    // Sounds
    m_lVolume = DSBVOLUME_MAX;
    m_pbSampleData = NULL;

}


//-----------------------------------------------------------------------------
// Name: DownloadEffectsImage
// Desc: Downloads an effects image to the DSP
//-----------------------------------------------------------------------------
HRESULT CXBoxSample::DownloadEffectsImage(PCHAR pszScratchFile)
{
    if( !XLoadSection( "DSPImage" ) )
        return E_FAIL;

    LPDSEFFECTIMAGEDESC pDesc;
    if( FAILED( XAudioDownloadEffectsImage( "DSPImage", NULL, XAUDIO_DOWNLOADFX_XBESECTION, &pDesc ) ) )
        return E_FAIL;

    XFreeSection( "DSPImage" );

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: InitializeEffectParameters()
// Desc: Loads the current effect parameters from the DSP
//-----------------------------------------------------------------------------
HRESULT CXBoxSample::InitializeEffectParameters()
{
    // For each effect chain...
    for( DWORD dwEffect = 0; dwEffect < MAX_CHAINS; dwEffect++ )
    {
        // Iterate over each parameter
        for( DWORD i = 0; i < g_aOptions[ dwEffect ].dwNumParams; i++ )
        {
            EFFECTPARAM * pParam = &g_aOptions[ dwEffect ].pEffectParams[ i ];

            // Get the raw value
            DWORD dwValue;
            m_pDSound->GetEffectData( pParam->dwEffectIndex, pParam->dwOffset, &dwValue, sizeof( DWORD ) );

            // And convert to something we can display
            switch( pParam->ef )
            {
            case PARAM_DWORD:
                // DWORDs can be type-cast 
                pParam->fValue = (FLOAT)dwValue;
                break;

            case PARAM_FLOAT:
                // Float format is s.23: 24 bit two's complement with the point fixed
                // at the left. 0x7FFFFF is just less than 1.0, 0x800000 is -1.0
                if( dwValue >= 0x800000 )
                {
                    pParam->fValue = -(( 0x1000000 - dwValue ) / (FLOAT)0x800000);
                }
                else
                {
                    pParam->fValue = dwValue / (FLOAT)0x7FFFFF;
                }
                break;
            }
        }
    }

    return S_OK;
}



//-----------------------------------------------------------------------------
// Name: Initialize
// Desc: Peforms initialization
//-----------------------------------------------------------------------------
HRESULT CXBoxSample::Initialize()
{
    m_dwCurrent        = 0;
    m_dwEffect         = 0;
    m_dwParam          = 0;
    m_dwFirstDisplayed = 0;

    // Create a font
    if( FAILED( m_Font.Create( m_pd3dDevice, "Font.xpr" ) ) )
        return XBAPPERR_MEDIANOTFOUND;

    // Create help
    if( FAILED( m_Help.Create( m_pd3dDevice, "Gamepad.xpr" ) ) )
        return XBAPPERR_MEDIANOTFOUND;

    // Create DirectSound
    if( FAILED( DirectSoundCreate( NULL, &m_pDSound, NULL ) ) )
        return E_FAIL;

    // Download a scratch image that contains our effects graphs
    if( FAILED( DownloadEffectsImage("d:\\media\\image.bin") ) )
        return E_FAIL;

    // Load all of the initial effect parameters from the DSP
    InitializeEffectParameters();

    // Set frequency on oscillator running in the DSP
    SetFxOscillatorParameters(m_pDSound, OSCILLATOR_CHAIN_OSCILLATOR,m_fOscillatorFrequency);

    // Create a sine wave for our host-based oscillator
    CreateSineWaveBuffer( 1, &m_pOscillator );

    //
    // For testing purposes:
    // Make sure that all our files have the same format,
    // since we're re-using the samd DirectSoundBuffer for
    // all the files.  First, we have to determine how
    // much space to allocate for the wave format, since
    // the XBOXADPCMWAVEFORMAT is 2 bytes larger than
    // WAVEFORMATEX.
    //
    DWORD          cbFirst;
    WAVEFORMATEX * pwfFirst = NULL;
    DWORD          cbWalk;
    WAVEFORMATEX * pwfWalk = NULL;
    for( int i = 0; i < NUM_SOUNDS; i++ )
    {
        if( FAILED( m_awfSounds[ i ].Open( g_aszFileNames[i] ) ) )
            return XBAPPERR_MEDIANOTFOUND;

        // Testing only:
        // Verify they're all the same format, since we're only going to have
        // one sound buffer.
        if( i == 0 )
        {
            // Allocate space for the wave format
            m_awfSounds[ i ].GetFormat( NULL, 0, &cbFirst );
            pwfFirst = (WAVEFORMATEX *)new BYTE[ cbFirst ];

            if( FAILED( m_awfSounds[ i ].GetFormat( pwfFirst, cbFirst ) ) )
                return E_FAIL;
        }
        else
        {
            // Allocate space for the wave format
            m_awfSounds[ i ].GetFormat( NULL, 0, &cbWalk );
            pwfWalk = (WAVEFORMATEX *)new BYTE[ cbWalk ];

            if( FAILED( m_awfSounds[ i ].GetFormat( pwfWalk, cbWalk ) ) )
                return E_FAIL;

            if( cbWalk != cbFirst ||
                memcmp( pwfFirst, pwfWalk, cbFirst ) )
            {
                OUTPUT_DEBUG_STRING( "All wav files should have the same format!\n" );
                return E_FAIL;
            }

            delete[] pwfWalk;
        }
    }

    //
    // Create a sound buffer of 0 size, since we're going to use
    // SetBufferData
    //
    DSBUFFERDESC dsbdesc;
    DSMIXBINS dsmixbins;
    DSMIXBINVOLUMEPAIR dsmbvp[DSMIXBIN_ASSIGNMENT_MAX];
    ZeroMemory( &dsbdesc, sizeof( DSBUFFERDESC ) );
    dsbdesc.dwSize = sizeof( DSBUFFERDESC );

    /* If fewer than 256 buffers are in existence at all points during 
       the game, it may be more efficient not to use LOCDEFER. */
    dsbdesc.dwFlags = DSBCAPS_LOCDEFER;
    dsbdesc.dwBufferBytes = 0;
    dsbdesc.lpwfxFormat = pwfFirst;
    dsbdesc.lpMixBins = &dsmixbins;

    dsmixbins.dwMixBinCount = g_aOptions[ m_dwEffect ].dwMixBinCount;
    dsmixbins.lpMixBinVolumePairs = dsmbvp;

    for (i = 0; i < (int)dsmixbins.dwMixBinCount; i++ )
    {
        dsmbvp[ i ].dwMixBin = g_aOptions[ m_dwEffect ].dwMixBins[ i ];
        dsmbvp[ i ].lVolume = DSBVOLUME_MAX;
    }

    if( FAILED( DirectSoundCreateBuffer( &dsbdesc, &m_pDSBuffer ) ) )
        return E_FAIL;

    // Set up and play our initial sound
    m_bPlaying = TRUE;
    SwitchToSound( m_dwCurrent );

    // Clean up our memory allocations
    delete[] pwfFirst;

    return S_OK;
}


//-----------------------------------------------------------------------------
// Name: SwitchToSound
// Desc: Switches to the given sound by:
//       1) Stop playback if we're playing
//       2) Reallocate the sample data buffer
//       3) Point the DirectSoundBuffer to the new data
//       4) Restart plyaback if needed
//-----------------------------------------------------------------------------
HRESULT CXBoxSample::SwitchToSound( DWORD dwIndex )
{
    DWORD dwNewSize;
    DWORD dwLoopStartSample, dwLoopLengthSamples;
    DWORD cbLoopStart, cbLoopLength;

    // If we're currently playing, stop, so that we don't crash
    // when we reallocate our buffer
    if( m_bPlaying )
    {
        m_pDSBuffer->Stop();
    }

    // Calling stop doesn't immediately shut down
    // the voice, so point it away from our buffer
    m_pDSBuffer->SetBufferData( NULL, 0 );

    // Find out how big the new sample is
    m_awfSounds[ dwIndex ].GetDuration( &dwNewSize );

    // Set our allocation to that size
    if( m_pbSampleData )
        delete[] m_pbSampleData;
    m_pbSampleData = new BYTE[ dwNewSize ];
    if( !m_pbSampleData )
        return E_OUTOFMEMORY;

    // Read sample data from the file
    m_awfSounds[ dwIndex ].ReadSample( 0, m_pbSampleData, dwNewSize, &dwNewSize );

    // Check for embedded loop points
    if( SUCCEEDED( m_awfSounds[ dwIndex ].GetLoopRegion( &dwLoopStartSample, &dwLoopLengthSamples ) ) )
    {
        WAVEFORMATEX wfx;

        // We're not interested in the whole wave format, which is
        // why we can get away with just loading the core WAVEFORMATEX
        m_awfSounds[ dwIndex ].GetFormat( &wfx, sizeof( WAVEFORMATEX ) );

        // We need to convert the loop points from sample counts to
        // byte offsets, but it's slightly different between PCM and ADPCM
        if( wfx.wFormatTag == WAVE_FORMAT_XBOX_ADPCM )
        {
            // For ADPCM, calculate # of blocks and multiply that
            // by bytes per block.  Xbox ADPCM is always 64 samples
            // per block.
            cbLoopStart = dwLoopStartSample / 64 * wfx.nBlockAlign;
            cbLoopLength = dwLoopLengthSamples / 64 * wfx.nBlockAlign;
        }
        else
        {
            // For PCM, multiply by bytes per sample
            DWORD cbBytesPerSample = wfx.nChannels * wfx.wBitsPerSample / 8;
            cbLoopStart = dwLoopStartSample * cbBytesPerSample;
            cbLoopLength = dwLoopLengthSamples * cbBytesPerSample;
        }
    }
    else
    {
        // Otherwise, just loop the whole file
        cbLoopStart = 0;
        cbLoopLength = dwNewSize;
    }


    // Set up values for the new buffer
    m_pDSBuffer->SetBufferData( m_pbSampleData, dwNewSize );
    m_pDSBuffer->SetLoopRegion( cbLoopStart, cbLoopLength );
    m_pDSBuffer->SetCurrentPosition( 0 );

    // If we were playing before, restart playback now
    if( m_bPlaying )
        m_pDSBuffer->Play( 0, 0, DSBPLAY_LOOPING );

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: NextFilterGraph
// Desc: Switches to the next filter graph.  This involves:
//       1) Routing the current effect to a dummy mixbin to silence it
//       2) Routing the next effect to the speaker mixbins
//       3) Routing the buffer to the new effect's input mixbins
//-----------------------------------------------------------------------------
HRESULT CXBoxSample::NextFilterGraph()
{
    // First, we need to silence the current effect by routing its output
    // to unused mixbins.  Otherwise, an effect that uses a delay line may
    // keep playing
    for( DWORD i = 0; i < g_aOptions[ m_dwEffect ].dwNumOutputs; i++ )
    {
        // Use unlucky number 13 for our dummy mixbin
        DWORD dwDummyOutput = DSPMIXBIN_FXSEND_13;

        m_pDSound->SetEffectData( g_aOptions[ m_dwEffect ].pOutputs[ i ].dwEffectIndex,
                                  g_aOptions[ m_dwEffect ].pOutputs[ i ].dwOutputOffset,
                                  &dwDummyOutput,
                                  sizeof( DWORD ),
                                  DSFX_DEFERRED );
    }

    m_dwEffect = ( m_dwEffect + 1 ) % MAX_CHAINS;

    // Now, route the new effect's output to the speakers
    DWORD adwSpeakerOutputs[6] = {
        DSPMIXBIN_FRONT_LEFT,
        DSPMIXBIN_FRONT_RIGHT,
        DSPMIXBIN_FRONT_CENTER,
        DSPMIXBIN_LOW_FREQUENCY,
        DSPMIXBIN_BACK_LEFT,
        DSPMIXBIN_BACK_RIGHT };
    assert( g_aOptions[ m_dwEffect ].dwNumOutputs < 6 );
    for( DWORD i = 0; i < g_aOptions[ m_dwEffect ].dwNumOutputs; i++ )
    {
        DWORD dwDummy;
        m_pDSound->GetEffectData( g_aOptions[ m_dwEffect ].pOutputs[ i ].dwEffectIndex,
                                  g_aOptions[ m_dwEffect ].pOutputs[ i ].dwOutputOffset,
                                  &dwDummy,
                                  sizeof( DWORD ) );
        m_pDSound->SetEffectData( g_aOptions[ m_dwEffect ].pOutputs[ i ].dwEffectIndex,
                                  g_aOptions[ m_dwEffect ].pOutputs[ i ].dwOutputOffset,
                                  &adwSpeakerOutputs[i],
                                  sizeof( DWORD ),
                                  DSFX_DEFERRED );
    }

    // Now commit our routing changes
    m_pDSound->CommitEffectData();

    // Have our buffer send its output to the inputs of the new
    // effect chain
    DSMIXBINS dsmixbins;
    DSMIXBINVOLUMEPAIR dsmbvp[ DSMIXBIN_ASSIGNMENT_MAX ];

    dsmixbins.dwMixBinCount = g_aOptions[ m_dwEffect ].dwMixBinCount;
    dsmixbins.lpMixBinVolumePairs = dsmbvp;
    
    for( int i = 0; i < (int)dsmixbins.dwMixBinCount; i++)
    {
        dsmbvp[ i ].dwMixBin = g_aOptions[ m_dwEffect ].dwMixBins[ i ];
        dsmbvp[ i ].lVolume = DSBVOLUME_MAX;
    }
    
    m_pDSBuffer->SetMixBins( &dsmixbins );
    m_dwParam = 0;
    m_dwFirstDisplayed = 0;

    return S_OK;
}
                       

#define VOLUME_SCALE 5.0f
//-----------------------------------------------------------------------------
// Name: FrameMove
// Desc: Performs per-frame updates
//-----------------------------------------------------------------------------
HRESULT CXBoxSample::FrameMove()
{
    // Toggle help
    if( m_DefaultGamepad.wPressedButtons & XINPUT_GAMEPAD_BACK ) 
    {
        m_bDrawHelp = !m_bDrawHelp;
    }

    // Increase/Decrease volume
    m_lVolume += LONG( ( m_DefaultGamepad.bAnalogButtons[ XINPUT_GAMEPAD_BLACK ] - 
                   m_DefaultGamepad.bAnalogButtons[ XINPUT_GAMEPAD_WHITE ] ) *
                   m_fElapsedTime * 
                   VOLUME_SCALE );

    // Make sure volume is in the appropriate range
    if( m_lVolume < DSBVOLUME_MIN )
        m_lVolume = DSBVOLUME_MIN;
    else if( m_lVolume > DSBVOLUME_MAX )
        m_lVolume = DSBVOLUME_MAX;

    // Toggle sound on and off
    if( m_DefaultGamepad.bPressedAnalogButtons[ XINPUT_GAMEPAD_A ] )
    {
        if( m_bPlaying )
            m_pDSBuffer->Stop( );
        else
            m_pDSBuffer->Play( 0, 0, DSBPLAY_LOOPING );

        m_bPlaying = !m_bPlaying;
    }

    // Cycle through sounds
    if( m_DefaultGamepad.bPressedAnalogButtons[ XINPUT_GAMEPAD_B ] )
    {
        m_dwCurrent = ( m_dwCurrent + 1 ) % NUM_SOUNDS;
        SwitchToSound( m_dwCurrent );
    }

    // change oscillator frequency of DSP FX that drives ampmod->flange chain
    // the oscillator that drives chorus is justa looping buffer with a single-cycle sine-wave
    // on the host
    if (m_dwEffect == EFFECT_AMPMOD_FLANGE) {

        DOUBLE fOldFrequency = m_fOscillatorFrequency;
        m_fOscillatorFrequency += m_DefaultGamepad.fX1 / OSCILLATOR_SCALE;
        if( m_fOscillatorFrequency > MAX_OSCILLATOR_FREQUENCY )
            m_fOscillatorFrequency = MAX_OSCILLATOR_FREQUENCY;
        if( m_fOscillatorFrequency <= MIN_OSCILLATOR_FREQUENCY )
            m_fOscillatorFrequency = MIN_OSCILLATOR_FREQUENCY;

        //
        // check if the frequency changed. We only want to touch the dsp oscillator FX params
        // if something changed, to minimize clicks caused by resetting the waveform
        //

        if (fOldFrequency != m_fOscillatorFrequency) {

            SetFxOscillatorParameters(m_pDSound, OSCILLATOR_CHAIN_OSCILLATOR,m_fOscillatorFrequency);

        }
    }

    // Cycle through effect graphs
    if( m_DefaultGamepad.bPressedAnalogButtons[ XINPUT_GAMEPAD_X ] )
        NextFilterGraph();

    // Deal with effect parameters
    if( g_aOptions[ m_dwEffect ].dwNumParams )
    {
        // Select active parameter with DPAD
        if( m_DefaultGamepad.wPressedButtons & XINPUT_GAMEPAD_DPAD_UP )
        {
            // Scroll up if we can and need to
            if( m_dwFirstDisplayed == m_dwParam && m_dwFirstDisplayed > 0)
            {
                m_dwFirstDisplayed -= 1;
            }

            // Select previous parameter
            if( m_dwParam > 0 )
            {
                m_dwParam -= 1;
            }
        }
        if( m_DefaultGamepad.wPressedButtons & XINPUT_GAMEPAD_DPAD_DOWN )
        {
            // Scroll down if we can and need to
            if( m_dwFirstDisplayed + PARAM_DISPLAY_LENGTH - 1 == m_dwParam &&
                m_dwFirstDisplayed + PARAM_DISPLAY_LENGTH < g_aOptions[ m_dwEffect ].dwNumParams )
            {
                m_dwFirstDisplayed += 1;
            }

            // Select next parameter
            if( m_dwParam < g_aOptions[ m_dwEffect ].dwNumParams - 1 )
            {
                m_dwParam += 1;
            }
        }

        // Grab a pointer to the current effect
        EFFECTPARAM * pParam = &g_aOptions[ m_dwEffect ].pEffectParams[ m_dwParam ];
        FLOAT fOld = pParam->fValue;
        DWORD dwScale = DWORD( 32768 / ( pParam->fMax - pParam->fMin ) / 60 );
        BOOL bParamChanged = FALSE;

        // Internally, we store all types of parameters as floats, and then
        // convert for display/SetEffectData
        pParam->fValue += m_DefaultGamepad.fX2 / dwScale;
        if( pParam->fValue > pParam->fMax )
            pParam->fValue = pParam->fMax;
        if( pParam->fValue < pParam->fMin )
            pParam->fValue = pParam->fMin;

        // See if the parameter changed
        bParamChanged = ( fOld != pParam->fValue );

        // If a parameter changed, we need to call SetEffectData
        // Note that we could use DSFX_DEFERRED and CommitEffectData() if we were
        // changing several parameters at once.
        if( bParamChanged )
        {
            DWORD dwValue;
            switch( pParam->ef )
            {
            case PARAM_DWORD:
                // DWORDs can just be copied over
                dwValue = DWORD(pParam->fValue);
                break;
            case PARAM_FLOAT:
                // Float format is s.23: 24 bit two's complement with the point fixed
                // at the left. 0x7FFFFF is just less than 1.0, 0x800000 is -1.0
                if( pParam->fValue >= 0 )
                    dwValue = DWORD( pParam->fValue * 0x7FFFFF );
                else
                    dwValue = 0x1000000 - DWORD( -pParam->fValue * 0x800000 );
                break;
            }
            m_pDSound->SetEffectData( pParam->dwEffectIndex, pParam->dwOffset, &dwValue, sizeof( DWORD ), DSFX_IMMEDIATE );
        }
    }

    m_pDSBuffer->SetVolume( m_lVolume );

    return S_OK;
}



//-----------------------------------------------------------------------------
// Name: Render
// Desc: Renders the scene
//-----------------------------------------------------------------------------
HRESULT CXBoxSample::Render()
{
    // Clear the zbuffer
    m_pd3dDevice->Clear( 0L, NULL, D3DCLEAR_ZBUFFER|D3DCLEAR_STENCIL,
                         0x00000000, 1.0f, 0L );

    DirectSoundDoWork();

    // Draw a gradient filled background
    RenderGradientBackground( 0xff404040, 0xff404080 );

    // Show title, frame rate, and help
    if( m_bDrawHelp )
        m_Help.Render( &m_Font, g_HelpCallouts, NUM_HELP_CALLOUTS );
    else
    {
        WCHAR szBuff[200];

        m_Font.Begin();
        m_Font.DrawText(  64, 50, 0xffffffff, L"GlobalFX" );
        m_Font.DrawText( 450, 50, 0xffffff00, m_strFrameRate );

        // Show status
        swprintf( szBuff, L"Current Sound: %S", g_aszFileNames[ m_dwCurrent ] );
        m_Font.DrawText( 64, 100, m_bPlaying ? 0xFFFFFFFF : 0xFF808080, szBuff );
        swprintf( szBuff, L"Volume: %ddB (%0.0f%%)", m_lVolume / 100, 100.0f * ( m_lVolume - DSBVOLUME_MIN ) / (DSBVOLUME_MAX - DSBVOLUME_MIN ) );
        m_Font.DrawText( 64, 130, 0xFFFFFF00, szBuff );
        swprintf( szBuff, L"Effect graph: %s", g_aOptions[ m_dwEffect ].szDescription );
        m_Font.DrawText( 64, 160, 0xffffffff, szBuff );

        if (m_dwEffect == EFFECT_AMPMOD_FLANGE) {

            //
            // display current oscillator frequency
            //

            swprintf( szBuff, L"Oscillator Frequency: %f",m_fOscillatorFrequency );
            m_Font.DrawText( 64, 400, 0xffffffff, szBuff );

        }

        // Now display our effect parameters, if any
        EFFECTPARAM * pParams = g_aOptions[ m_dwEffect ].pEffectParams;
        DWORD dwLastDisplayed = min( m_dwFirstDisplayed + PARAM_DISPLAY_LENGTH,
                                     g_aOptions[ m_dwEffect ].dwNumParams );
        FLOAT fY = 190.0f;
        for( DWORD i = m_dwFirstDisplayed; i < dwLastDisplayed; i++ )
        {
            switch( pParams[ i ].ef )
            {
            case PARAM_DWORD:
                swprintf( szBuff, L"%s: %ld", pParams[ i ].szDescription, (DWORD)pParams[ i ].fValue );
                break;
            case PARAM_FLOAT:
                swprintf( szBuff, L"%s: %f", pParams[ i ].szDescription, pParams[ i ].fValue );
                break;
            }
            m_Font.DrawText( 90, fY, m_dwParam == i ? 0xffffffff : 0xffffff00, szBuff );
            fY += 30.0f;
        }

        // Show scroll arrows if needed
        if( m_dwFirstDisplayed > 0 )
            m_Font.DrawText( 64, 190, 0xffffff00, L"^" );
        if( m_dwFirstDisplayed + PARAM_DISPLAY_LENGTH < g_aOptions[ m_dwEffect ].dwNumParams )
            m_Font.DrawText( 64, 190 + ( PARAM_DISPLAY_LENGTH - 1 ) * 30.0f, 0xffffff00, L"v" );

        m_Font.End();
    }

    // Present the scene
    m_pd3dDevice->Present( NULL, NULL, NULL, NULL );

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: CreateSineWaveBuffer
// Desc: Creates a DirectSound Buffer and fills it with a sine wave.  This
//       can be useful for DSP effects that are driven off an oscillator.
//       Note that non-integral frequencies will end up with a discontinuity
//       at the loop point.
//-----------------------------------------------------------------------------
HRESULT CXBoxSample::CreateSineWaveBuffer( double dFrequency, LPDIRECTSOUNDBUFFER8 * ppBuffer )
{
    HRESULT hr = S_OK;
    LPDIRECTSOUNDBUFFER8 pBuffer = NULL;
    BYTE * pData = NULL;
    DWORD dwBytes = 0;
    double dArg = 0.0;
    double dSinVal = 0.0;
    BYTE bVal = 0;

    // Check arguments
    if( !ppBuffer || dFrequency < 0 )
        return E_INVALIDARG;

    *ppBuffer = NULL;

    //
    // Initialize a wave format structure
    //
    WAVEFORMATEX wfx;
    ZeroMemory( &wfx, sizeof( WAVEFORMATEX ) );

    wfx.wFormatTag      = WAVE_FORMAT_PCM;      // PCM data
    wfx.nChannels       = 1;                    // Mono
    wfx.nSamplesPerSec  = 1000;                 // 1kHz
    wfx.nAvgBytesPerSec = 1000;                 // 1kHz * 1 bytes / sample
    wfx.nBlockAlign     = 1;                    // sample size in bytes
    wfx.wBitsPerSample  = 8;                    // 8 bit samples
    wfx.cbSize          = 0;                    // No extra data

    //
    // Intialize the buffer description
    DSBUFFERDESC dsbd;
    DSMIXBINS dsmixbins;
    DSMIXBINVOLUMEPAIR dsmbvp;
    ZeroMemory( &dsbd, sizeof( DSBUFFERDESC ) );

    dsbd.dwSize = sizeof( DSBUFFERDESC );
    dsbd.dwBufferBytes = wfx.nAvgBytesPerSec;
    dsbd.lpwfxFormat = &wfx;
    dsbd.lpMixBins = &dsmixbins;

    dsmixbins.dwMixBinCount = 1;
    dsmixbins.lpMixBinVolumePairs = &dsmbvp;

    dsmbvp.dwMixBin = DSMIXBIN_FXSEND_19;
    dsmbvp.lVolume = DSBVOLUME_MAX;

    // Create the buffer
    hr = DirectSoundCreateBuffer( &dsbd, &pBuffer );
    if( FAILED( hr ) )
        return hr;

    // Set the oscillator mixbin headroom to 0 so the effect gets a full-scale
    // sine wave
    m_pDSound->SetMixBinHeadroom( DSMIXBIN_FXSEND_19, 0 );

    // Get a pointer to buffer data to fill
    hr = pBuffer->Lock( 0, dsbd.dwBufferBytes, (VOID **)&pData, &dwBytes, NULL, NULL, 0 );
    if( FAILED( hr ) )
        return hr;

    // Now fill the buffer, 1 8-bit sample at a time
    for( DWORD i = 0; i < dwBytes; i++ )
    {
        // Convert sample offset to radians
        dArg = (double)i / wfx.nSamplesPerSec * D3DX_PI * 2;

        // Calculate the sin
        dSinVal = sin( dFrequency * dArg );

        // Scale to sample format
        bVal = BYTE( dSinVal * 127 );

        // Store the sample
        pData[i] = bVal;
    }

    // Start the sine wave looping
    hr = pBuffer->Play( 0, 0, DSBPLAY_LOOPING );
    if( FAILED( hr ) )
        return hr;

    // return the buffer
    *ppBuffer = pBuffer;

    return hr;
}

#define PI (DOUBLE)3.14159

//-----------------------------------------------------------------------------
// Name: SetFXOscillatorParameters
// Desc: The routine manipulates variables in DSP memory to change the output
//       of the low frequency oscillator DSP effect. It translates frequency
//       to coefficients required for a second order IIR to produce a pseudo
//       sine wave. The dsp effect image loaded must include the oscillator FX.
//       Using an in-dsp oscillator is presented here as an alternative to using
//       a buffer above (essentially burning a hw voice) for oscillators.
//-----------------------------------------------------------------------------

HRESULT SetFxOscillatorParameters(LPDIRECTSOUND pDirectSound,DWORD dwEffectIndex,DOUBLE Frequency)
{

    DWORD dwValue = 0;
    HRESULT hr = S_OK;

    //
    // convert frequency to 1/10s of Hz as an integer
    //

    dwValue = (DWORD)(Frequency*10.0);

    hr = pDirectSound->SetEffectData(dwEffectIndex,FIELD_OFFSET( DSFX_OSCILLATOR_PARAMS,adwFrequency), &dwValue, sizeof( DWORD ), DSFX_IMMEDIATE);

    return hr;

}



