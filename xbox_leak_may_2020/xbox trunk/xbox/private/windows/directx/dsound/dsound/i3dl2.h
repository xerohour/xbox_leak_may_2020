/***************************************************************************
 *
 *  Copyright (C) 2000 Microsoft Corporation.  All Rights Reserved.
 *
 *  File:       i3dl2.h
 *  Content:    I3dl2 reverb and filter calculations.
 *  History:
 *   Date       By      Reason
 *   ====       ==      ======
 *  05/30/01    dereks  Created based on Sensaura tables.
 *
 ****************************************************************************/

#ifndef __I3DL2_H__
#define __I3DL2_H__

#define I3DL2_DSP_INIT

#define RVB_HW_REFLECTIONS	    4
#define RVB_HW_SHORTREVERBS	    4
#define RVB_HW_LONGREVERBS	    2

#define RVB_HW_DEFSIZE			1.0f	// Default size parameter = minimum supported size
#define RVB_MAXSMALLSIZE		2.5f	// Largest size that causes feedback delays to be shortened
#define RVB_BASESIZE			0.8f	// Minimum size for reverb feedback delay calculations 

//
// I3dl2 listener properties
//

DEFINETYPE(I3DL2LISTENER, DSFX_I3DL2REVERB_PARAMS);

//
// I3dl2 source properties
//

#define MCPX_I3DL2APPLY_DIRECT      0x00000001
#define MCPX_I3DL2APPLY_SOURCE      0x00000002
#define MCPX_I3DL2APPLY_DIRECTIIR   0x00000004
#define MCPX_I3DL2APPLY_REVERBIIR   0x00000008
#define MCPX_I3DL2APPLY_MASK        0x0000000F

BEGIN_DEFINE_STRUCT()
    DWORD       dwChangeMask;
    LONG        lDirect;
    LONG        lSource;
    INT         nDirectIir;
    INT         nReverbIir;
END_DEFINE_STRUCT(I3DL2SOURCE);

#ifdef __cplusplus

//
// I3DL2 listener object
//

namespace DirectSound
{
    class CI3dl2Listener
    {
        friend class CI3dl2Source;

    protected:
        REFDSI3DL2LISTENER      m_I3dl2Params;      // I3dl2 parameters
        I3DL2LISTENER           m_I3dl2Data;        // Calculated I3dl2 data

    private:
        static const float      m_flScale23;
        static const float      m_flScale16;
        static const DWORD      m_dwSamplesPerSec;
        static const float      m_aflReflectionData[RVB_HW_REFLECTIONS][5];
        static const float      m_aflShortReverbInputFactor[RVB_HW_SHORTREVERBS][2];
        static const float      m_aflLongReverbInputDelay[RVB_HW_LONGREVERBS][4];
        static const float      m_aflShortReverbFeedbackDelay[RVB_HW_SHORTREVERBS];

        enum 
        { 
            MainDelayLineID = 0, 
            ReflectionDelayLineID = 4, 
            ShortReverbDelayLineID = 8, 
            LongReverbDelayLineID = 12 
        };

        enum 
        { 
            InputIIR = 0, 
            MainDelayLineLongReverbIIR = 1, 
            ShortReverbIIR = 4, 
            LongReverbIIR = 8 
        };

    public:
        CI3dl2Listener(REFDSI3DL2LISTENER ds3dl);

    public:
        void Initialize(void);
        void CalculateI3dl2(void);

    private:
        void SetSize(float fSize);
        void SetInputFilter(long lGainHF, float fHFReference);
        void SetReflectionsGain(float fGain);
        void SetReflectionsDelay(float fDelay);
        void SetReverbGain(float fGain);
        void SetReverbDelay(float fDelay);
        void SetDecayTimes(float fDecayTime, float fDecayHFRatio, float fHFReference);
        void SetDiffusion(float fDiffusion);
        void SetDecayFilter(DSFX_I3DL2REVERB_IIR *pIir, DWORD dwDelay, float fDecayTime, float fDecayHFRatio, float fHFReference);
        void Get1PoleLoPass(long lGain, long lGainHF, float fHFReference, float fSamplesPerSec, int *pia, int *pib);
        void Get1PoleLoPass(long lGain, long lGainHF, float fHFReference, float fSamplesPerSec, float *pfa, float *pfb);
    };
}

//
// I3dl2 source object
//

namespace DirectSound
{
    class CI3dl2Source
    {
    public:
        I3DL2SOURCE                 m_I3dl2Data;

    private:
        static const I3DL2SOURCE    m_DefaultI3dl2Data;

    protected:
        const CI3dl2Listener &      m_Listener;
        REFDSI3DL2BUFFER            m_I3dl2Params;

    public:
        CI3dl2Source(const CI3dl2Listener &Listener, REFDSI3DL2BUFFER ds3db);

    public:
        void CalculateI3dl2(FLOAT flDistance);

    private:
        int Get1PoleLoPass(long lGain, long lGainHF, float fHFReference, float fSamplesPerSec);
    };
}

#endif // __cplusplus

#endif // __I3DL2_H__
