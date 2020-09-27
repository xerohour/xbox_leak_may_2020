/***************************************************************************
 *
 *  Copyright (C) 2000 Microsoft Corporation.  All Rights Reserved.
 *
 *  File:       HRTF.h
 *  Content:    HRTF 3D objects.
 *  History:
 *   Date       By      Reason
 *   ====       ==      ======
 *  08/13/01    dereks  Created.
 *
 ****************************************************************************/

#ifndef __HRTF_H__
#define __HRTF_H__

//
// 3D calculation flags
//

#define MCPX_3DCALC_NORMPOS                 0x00000001
#define MCPX_3DCALC_LEFTRIGHT               0x00000002
#define MCPX_3DCALC_DISTANCE                0x00000004
#define MCPX_3DCALC_DIRECTION               0x00000008
#define MCPX_3DCALC_CONEVOLUME              0x00000010
#define MCPX_3DCALC_FRONTREAR               0x00000020
#define MCPX_3DCALC_DOPPLER                 0x00000040
#define MCPX_3DCALC_I3DL2SOURCE             0x00000080
#define MCPX_3DCALC_I3DL2LISTENER           0x00000100
#define MCPX_3DCALC_LISTENER_ORIENTATION    0x00000200

#define DS3DPARAM_LISTENER_DISTANCEFACTOR   (MCPX_3DCALC_FRONTREAR | MCPX_3DCALC_DOPPLER)
#define DS3DPARAM_LISTENER_DOPPLERFACTOR    (MCPX_3DCALC_DOPPLER)
#define DS3DPARAM_LISTENER_ROLLOFFFACTOR    (MCPX_3DCALC_DISTANCE)
#define DS3DPARAM_LISTENER_ORIENTATION      (MCPX_3DCALC_LEFTRIGHT | MCPX_3DCALC_DISTANCE | MCPX_3DCALC_FRONTREAR | MCPX_3DCALC_LISTENER_ORIENTATION)
#define DS3DPARAM_LISTENER_POSITION         (MCPX_3DCALC_NORMPOS | MCPX_3DCALC_LEFTRIGHT | MCPX_3DCALC_DISTANCE | MCPX_3DCALC_DIRECTION | MCPX_3DCALC_CONEVOLUME | MCPX_3DCALC_FRONTREAR | MCPX_3DCALC_DOPPLER | MCPX_3DCALC_I3DL2SOURCE)
#define DS3DPARAM_LISTENER_VELOCITY         (MCPX_3DCALC_DOPPLER)
#define DS3DPARAM_LISTENER_I3DL2            (MCPX_3DCALC_I3DL2SOURCE | MCPX_3DCALC_I3DL2LISTENER)
#define DS3DPARAM_LISTENER_MASK             (MCPX_3DCALC_NORMPOS | MCPX_3DCALC_LEFTRIGHT | MCPX_3DCALC_DISTANCE | MCPX_3DCALC_DIRECTION | MCPX_3DCALC_CONEVOLUME | MCPX_3DCALC_FRONTREAR | MCPX_3DCALC_DOPPLER | MCPX_3DCALC_I3DL2SOURCE | MCPX_3DCALC_LISTENER_ORIENTATION)

#define DS3DPARAM_BUFFER_CONEANGLES         (MCPX_3DCALC_CONEVOLUME)
#define DS3DPARAM_BUFFER_CONEORIENTATION    (MCPX_3DCALC_DIRECTION | MCPX_3DCALC_CONEVOLUME)
#define DS3DPARAM_BUFFER_CONEOUTSIDEVOLUME  (MCPX_3DCALC_CONEVOLUME)
#define DS3DPARAM_BUFFER_MAXDISTANCE        (MCPX_3DCALC_DISTANCE)
#define DS3DPARAM_BUFFER_MINDISTANCE        (MCPX_3DCALC_DISTANCE)
#define DS3DPARAM_BUFFER_POSITION           (MCPX_3DCALC_NORMPOS | MCPX_3DCALC_LEFTRIGHT | MCPX_3DCALC_DISTANCE | MCPX_3DCALC_DIRECTION | MCPX_3DCALC_CONEVOLUME | MCPX_3DCALC_FRONTREAR | MCPX_3DCALC_DOPPLER | MCPX_3DCALC_I3DL2SOURCE)
#define DS3DPARAM_BUFFER_VELOCITY           (MCPX_3DCALC_DOPPLER)
#define DS3DPARAM_BUFFER_FREQUENCY          (MCPX_3DCALC_DOPPLER)
#define DS3DPARAM_BUFFER_DISTANCEFACTOR     (MCPX_3DCALC_FRONTREAR | MCPX_3DCALC_DOPPLER)
#define DS3DPARAM_BUFFER_DOPPLERFACTOR      (MCPX_3DCALC_DOPPLER)
#define DS3DPARAM_BUFFER_ROLLOFFFACTOR      (MCPX_3DCALC_DISTANCE)
#define DS3DPARAM_BUFFER_I3DL2              (MCPX_3DCALC_I3DL2SOURCE)
#define DS3DPARAM_BUFFER_MASK               (MCPX_3DCALC_NORMPOS | MCPX_3DCALC_LEFTRIGHT | MCPX_3DCALC_DISTANCE | MCPX_3DCALC_DIRECTION | MCPX_3DCALC_CONEVOLUME | MCPX_3DCALC_FRONTREAR | MCPX_3DCALC_DOPPLER | MCPX_3DCALC_I3DL2SOURCE)

//
// Pan3d algorithm speaker locations
//

BEGIN_DEFINE_STRUCT()
    union 
    {
        D3DVECTOR   vSpeakerPos;    // Speaker position in 3D space
        VECTOR4     v4SpeakerPos;   // Speaker position in 4D space
    }; 

    DWORD       dwMixBin;           // Speaker mixbin identifier
END_DEFINE_STRUCT(PAN3DSPEAKER);

#define PAN3D_DEFAULT_SPEAKER_DATA \
    { { -0.7f, 0.0f,  0.7f }, DSMIXBIN_3D_FRONT_LEFT }, \
    { {  0.7f, 0.0f,  0.7f }, DSMIXBIN_3D_FRONT_RIGHT }, \
    { { -0.7f, 0.0f, -0.7f }, DSMIXBIN_3D_BACK_LEFT }, \
    { {  0.7f, 0.0f, -0.7f }, DSMIXBIN_3D_BACK_RIGHT },\
    { {  0.0f, 0.0f,  1.0f }, DSMIXBIN_FRONT_CENTER },

#define PAN3D_SPEAKER_COUNT 5

//
// 31-tap, 8-bit FIR filter coefficient data
//

BEGIN_DEFINE_STRUCT()
    BYTE    Coeff[31];
    BYTE    Delay;
END_DEFINE_STRUCT(FIRFILTER8);

//
// 3D filter pair
//

BEGIN_DEFINE_STRUCT()
    LPCFIRFILTER8   pLeftFilter;
    LPCFIRFILTER8   pRightFilter;
END_DEFINE_STRUCT(HRTFFILTERPAIR);

//
// 3D calculator types
//

BEGIN_DEFINE_ENUM()
    DS3DALG_FULL_HRTF = 0,
    DS3DALG_LIGHT_HRTF,
    DS3DALG_PAN,
    DS3DALG_COUNT,
END_DEFINE_ENUM_();

#define DS3DALG_FIRST DS3DALG_FULL_HRTF
#define DS3DALG_LAST DS3DALG_PAN
#define DS3DALG_INVALID DS3DALG_COUNT

//
// 3D listener parameters
//

BEGIN_DEFINE_STRUCT()
    D3DVECTOR       vNormOrient;                        // Full & light HRTF only
    PAN3DSPEAKER    aSpeakers[PAN3D_SPEAKER_COUNT];     // Pan3D only
END_DEFINE_STRUCT(HRTFLISTENER);

//
// 3D source parameters
//

BEGIN_DEFINE_STRUCT()
    D3DVECTOR       vNormPos;
    FLOAT           flMagPos;
    FLOAT           flAzimuth;
    FLOAT           flElevation;
    FLOAT           flThetaS;
END_DEFINE_STRUCT(HRTFSOURCE);

//
// 3D voice parameters
//

#define MCPX_3DAPPLY_DISTANCEVOLUME     0x00000001
#define MCPX_3DAPPLY_CONEVOLUME         0x00000002
#define MCPX_3DAPPLY_FRONTVOLUME        0x00000004
#define MCPX_3DAPPLY_REARVOLUME         0x00000008
#define MCPX_3DAPPLY_DOPPLERPITCH       0x00000010
#define MCPX_3DAPPLY_FILTERPAIR         0x00000020
#define MCPX_3DAPPLY_MASK               0x0000003F

BEGIN_DEFINE_STRUCT()
    DWORD               dwChangeMask;
    DWORD               dwMixBinValidMask;
    DWORD               dwMixBinChangeMask;
    LONG                lDistanceVolume;
    LONG                lConeVolume;
    LONG                lFrontVolume;
    LONG                lRearVolume;
    LONG                lDopplerPitch;
    HRTFFILTERPAIR      FilterPair;
    LONG                alMixBinVolumes[DSMIXBIN_COUNT];
END_DEFINE_STRUCT(HRTFVOICE);

//
// 3D API parameters
//

BEGIN_DEFINE_STRUCT()
    DS3DLISTENER    HrtfParams;             // 3D listener parameters
    DSI3DL2LISTENER I3dl2Params;            // I3DL2 listener parameters
    DWORD           dwParameterMask;        // 3D source properties that have changed
END_DEFINE_STRUCT(DS3DLISTENERPARAMS);

BEGIN_DEFINE_STRUCT()
    DS3DBUFFER      HrtfParams;             // 3D source parameters
    DSI3DL2BUFFER   I3dl2Params;            // I3DL2 source parameters
    const FLOAT *   paflRolloffPoints;      // Rolloff curve points
    DWORD           dwRolloffPointCount;    // Number of points in the rolloff curve
    DWORD           dwParameterMask;        // 3D source properties that have changed
END_DEFINE_STRUCT(DS3DSOURCEPARAMS);

#ifdef __cplusplus

//
// Forward declarations
//

namespace DirectSound
{
    class CHrtfSource;
}

//
// HRTF data calculation function
//

namespace DirectSound
{
    typedef void (*LPFNHRTFCALC)(DWORD dwOperation, CHrtfSource *pSource);
    typedef void (*LPFNGETHRTFFILTERPAIR)(CHrtfSource *pSource);
}

//
// 3D listener
//

namespace DirectSound
{
    class CHrtfListener
    {
        friend class CHrtfSource;
        friend class CFullHrtfSource;
        friend class CLightHrtfSource;
        friend class CPan3dSource;

    protected:
        static const D3DVECTOR      m_vDefaultNormOrient;                       // Default normalized orientation
        static const D3DVECTOR      m_vDefaultNormFrontOrient;                  // Default normalized front orientation

        static const PAN3DSPEAKER   m_aDefaultSpeakers[PAN3D_SPEAKER_COUNT];    // Default Pan3d speaker data

        D3DMATRIX *                 m_pTransformMatrix;

        REFDS3DLISTENER             m_3dParams;                                 // 3D parameters
        HRTFLISTENER                m_3dData;                                   // 3D calculation data
        BOOLEAN                     m_fSurround;                                // Surround or stereo?

    public:
        CHrtfListener(REFDS3DLISTENER ds3dl);
        ~CHrtfListener();

    public:
        void Calculate3d(DWORD dwOperation);
    };    
}

//
// Full 3D source
//

namespace DirectSound
{
    class CFullHrtfSource
    {
        friend class CLightHrtfSource;
        friend class CPan3dSource;

    public:
        enum
        {
            DS3DALG = DS3DALG_FULL_HRTF
        };

    public:
        // Calculation functions
        static void Calculate3d(DWORD dwOperation, CHrtfSource *pSource);
    
        // HRTF filter coefficients
        static void GetHrtfFilterPair(CHrtfSource *pSource);

    protected:
        // Calculation functions
        static void CalcNormPos(CHrtfSource *pSource);
        static void CalcLeftRightGains(CHrtfSource *pSource);
        static void CalcDistanceVolume(CHrtfSource *pSource);
        static void CalcDirection(CHrtfSource *pSource);
        static void CalcConeVolume(CHrtfSource *pSource);
        static void CalcFrontRearGains(CHrtfSource *pSource);
        static void CalcDoppler(CHrtfSource *pSource);
    };
}

//
// Light 3D source
//

namespace DirectSound
{
    class CLightHrtfSource
    {
        friend class CPan3dSource;

    public:
        enum
        {
            DS3DALG = DS3DALG_LIGHT_HRTF
        };

    public:
        // Calculation functions
        static void Calculate3d(DWORD dwOperation, CHrtfSource *pSource);
    
        // HRTF filter coefficients
        static void GetHrtfFilterPair(CHrtfSource *pSource);

    protected:
        // Calculation functions
        static void CalcNormPos(CHrtfSource *pSource);
        static void CalcLeftRightGains(CHrtfSource *pSource);
        static void CalcFrontRearGains(CHrtfSource *pSource);
    };
}

//
// MixBin pan 3D source
//

namespace DirectSound
{
    class CPan3dSource
    {
    public:
        enum
        {
            DS3DALG = DS3DALG_PAN
        };

    public:
        // Calculation functions
        static void Calculate3d(DWORD dwOperation, CHrtfSource *pSource);
    
        // HRTF filter coefficients
        static void GetHrtfFilterPair(CHrtfSource *pSource);

    protected:
        // Calculation functions
        static void CalcPan(CHrtfSource *pSource);
    };
}

//
// 3D source
//

namespace DirectSound
{
    class CHrtfSource
    {
        friend class CFullHrtfSource;
        friend class CLightHrtfSource;
        friend class CPan3dSource;

    public:
        HRTFSOURCE                      m_3dData;               // 3D calculation data
        HRTFVOICE                       m_3dVoiceData;          // 3D voice data
                                                            
    protected:                                                  
        const CHrtfListener &           m_Listener;             // 3D listener
        REFDS3DSOURCEPARAMS             m_3dParams;             // 3D parameters
        static UINT                     m_nAlgorithm;           // Algorithm identifier

    private:
        static const HRTFSOURCE         m_Default3dData;        // Default 3D calculation data
        static const HRTFVOICE          m_Default3dVoiceData;   // Default 3D voice data
        static LPFNHRTFCALC             m_pfnCalculate;         // 3D calculation function
        static LPFNGETHRTFFILTERPAIR    m_pfnGetFilterPair;     // Filter pair lookup function

    public:
        CHrtfSource(const CHrtfListener &Listener, REFDS3DSOURCEPARAMS Params);

    public:
        // HRTF quality
        static void SetAlgorithm_FullHrtf(void);
        static void SetAlgorithm_LightHrtf(void);
        static void SetAlgorithm_Pan(void);
        static BOOL IsValidAlgorithm(void);
        static UINT GetAlgorithm(void);

        // 3D parameters
        void Calculate3d(DWORD dwOperation);
        void GetHrtfFilterPair(void);
    };

    __inline void CHrtfSource::SetAlgorithm_FullHrtf(void)
    {
        m_nAlgorithm = CFullHrtfSource::DS3DALG;
        m_pfnCalculate = CFullHrtfSource::Calculate3d;
        m_pfnGetFilterPair = CFullHrtfSource::GetHrtfFilterPair;
    }

    __inline void CHrtfSource::SetAlgorithm_LightHrtf(void)
    {
        m_nAlgorithm = CLightHrtfSource::DS3DALG;
        m_pfnCalculate = CLightHrtfSource::Calculate3d;
        m_pfnGetFilterPair = CLightHrtfSource::GetHrtfFilterPair;
    }

    __inline void CHrtfSource::SetAlgorithm_Pan(void)
    {
        m_nAlgorithm = CPan3dSource::DS3DALG;
        m_pfnCalculate = CPan3dSource::Calculate3d;
        m_pfnGetFilterPair = CPan3dSource::GetHrtfFilterPair;
    }

    __inline BOOL CHrtfSource::IsValidAlgorithm(void)
    {
        return (m_nAlgorithm >= DS3DALG_FIRST) && (m_nAlgorithm <= DS3DALG_LAST);
    }

    __inline UINT CHrtfSource::GetAlgorithm(void)
    {
        return m_nAlgorithm;
    }

    __inline void CHrtfSource::GetHrtfFilterPair(void)
    {
        ASSERT(IsValidAlgorithm());
        ASSERT(m_pfnGetFilterPair);
        m_pfnGetFilterPair(this);
    }
}

#endif // __cplusplus

#endif // __HRTF_H__
