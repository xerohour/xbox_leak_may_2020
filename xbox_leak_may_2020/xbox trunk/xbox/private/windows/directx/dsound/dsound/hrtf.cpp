/***************************************************************************
 *
 *  Copyright (C) 2000 Microsoft Corporation.  All Rights Reserved.
 *
 *  File:       HRTF.cpp
 *  Content:    HRTF 3D objects.
 *  History:
 *   Date       By      Reason
 *   ====       ==      ======
 *  08/13/01    dereks  Created.
 *
 ****************************************************************************/

#include "dsoundi.h"
#include "xgmath.h"

const D3DVECTOR CHrtfListener::m_vDefaultNormOrient =
{
    1.0f, 0.0f, 0.0f
};

const D3DVECTOR CHrtfListener::m_vDefaultNormFrontOrient =
{
    0.0f, 0.0f, 1.0f
};

const PAN3DSPEAKER CHrtfListener::m_aDefaultSpeakers[PAN3D_SPEAKER_COUNT] = 
{ 
    PAN3D_DEFAULT_SPEAKER_DATA 
};

const HRTFSOURCE CHrtfSource::m_Default3dData =
{
    { 0.0f, 0.0f, 0.0f },   // vNormPos
    0.0f,                   // flMagPos
    0.0f,                   // flAzimuth
    0.0f,                   // flElevation
    0.0f                    // flThetaS
};

const HRTFVOICE CHrtfSource::m_Default3dVoiceData =
{
    0,                      // dwChangeMask
    0,                      // dwMixBinValidMask
    0,                      // dwMixBinChangeMask
    0,                      // lDistanceVolume
    0,                      // lConeVolume
    0,                      // lFrontVolume
    0,                      // lRearVolume
    0,                      // fDoppler
    { NULL, NULL },         // FilterPair
    { 0 }                   // alMixBinVolumes
};

LPFNHRTFCALC CHrtfSource::m_pfnCalculate = NULL;
LPFNGETHRTFFILTERPAIR CHrtfSource::m_pfnGetFilterPair = NULL;
UINT CHrtfSource::m_nAlgorithm = DS3DALG_INVALID;


/****************************************************************************
 *
 *  CHrtfListener
 *
 *  Description:
 *      Object constructor.
 *
 *  Arguments:
 *      LPCDWORD [in]: pointer to the speaker configuration.
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CHrtfListener::CHrtfListener"

CHrtfListener::CHrtfListener
(
    REFDS3DLISTENER         ds3dl
)
:   m_3dParams(ds3dl)
{
    DPF_ENTER();

    CopyMemory(&m_3dData.vNormOrient, &m_vDefaultNormOrient, sizeof(m_vDefaultNormOrient));
    CopyMemory(&m_3dData.aSpeakers, &m_aDefaultSpeakers, sizeof(m_aDefaultSpeakers));

    m_pTransformMatrix = PHYSALLOC(D3DMATRIX, 1, 16, PAGE_READWRITE);

    DPF_LEAVE_VOID();
}

/****************************************************************************
 *
 *  CHrtfListener
 *
 *  Description:
 *      Object destructor.
 *
 *  Arguments:
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "~CHrtfListener::CHrtfListener"

CHrtfListener::~CHrtfListener
(
)
{
    DPF_ENTER();

    PHYSFREE(m_pTransformMatrix);

    DPF_LEAVE_VOID();
}


/***************************************************************************
 *
 *  Calculate3d
 *
 *  Description:
 *      Commits deferred settings.
 *
 *  Arguments:
 *      DWORD [in]: parameter mask.
 *
 *  Returns:
 *      (void)
 *
 ***************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CHrtfListener::Calculate3d"

void
CHrtfListener::Calculate3d
(
    DWORD                   dwOperation
)
{
    const LPPAN3DSPEAKER    aSpeakers           = m_3dData.aSpeakers;
    const LPCPAN3DSPEAKER   aDefaultSpeakers    = m_aDefaultSpeakers;
    const UINT              nAlgorithm          = CHrtfSource::GetAlgorithm();
    D3DVECTOR               vNormOrientFront;
    FLOAT                   flTheta;
    XGVECTOR4               v4SpeakerPos;
    DWORD                   i;
    
    DPF_ENTER();

    if(dwOperation & MCPX_3DCALC_LISTENER_ORIENTATION)
    {
        //
        // Recalculate the normalized listener orientation
        //

        if((DS3DALG_FULL_HRTF == nAlgorithm) || (DS3DALG_LIGHT_HRTF == nAlgorithm) || (nAlgorithm >= DS3DALG_COUNT))
        {
            m_3dData.vNormOrient.x = (m_3dParams.vOrientTop.y * m_3dParams.vOrientFront.z) - (m_3dParams.vOrientFront.y * m_3dParams.vOrientTop.z);
            m_3dData.vNormOrient.y = (m_3dParams.vOrientTop.z * m_3dParams.vOrientFront.x) - (m_3dParams.vOrientFront.z * m_3dParams.vOrientTop.x);
            m_3dData.vNormOrient.z = (m_3dParams.vOrientTop.x * m_3dParams.vOrientFront.y) - (m_3dParams.vOrientFront.x * m_3dParams.vOrientTop.y);
        }
        
        //
        // Recalculate the speaker locations
        //

        if((DS3DALG_PAN == nAlgorithm) || (nAlgorithm >= DS3DALG_COUNT))
        {            
            //
            // Normalize front orientation vector
            //

            NormalizeVector3(&vNormOrientFront, &m_3dParams.vOrientFront);

            //
            // Calculate dot product between a default front orientation 
            // vector and new orientation vector. The dot product is the same 
            // as the angle between the default orientation vector and the 
            // new orientation vector because both vectors are normalized.
            //

            flTheta = (FLOAT)acos((vNormOrientFront.x * m_vDefaultNormFrontOrient.x) + (vNormOrientFront.y * m_vDefaultNormFrontOrient.y) + (vNormOrientFront.z * m_vDefaultNormFrontOrient.z));

            //
            // Now use the top orientation vector and the angle to calculate 
            // a transform matrix we need to multiply our speaker matrix by
            //

            ASSERT(m_pTransformMatrix);
            MatrixRotationAxis(m_pTransformMatrix, &m_3dParams.vOrientTop, flTheta);

            //
            // Now transform the speaker positions
            //

            for(i = 0; i < PAN3D_SPEAKER_COUNT; i++) 
            {
                Vec3Transform(&aSpeakers[i].v4SpeakerPos, &aDefaultSpeakers[i].vSpeakerPos, m_pTransformMatrix);
            }            
        }
    }

    DPF_LEAVE_VOID();
}


/****************************************************************************
 *
 *  CHrtfSource
 *
 *  Description:
 *      Object constructor.
 *
 *  Arguments:
 *      (void)
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CHrtfSource::CHrtfSource"

CHrtfSource::CHrtfSource
(
    const CHrtfListener &   Listener,
    REFDS3DSOURCEPARAMS     Params
)
:   m_Listener(Listener),
    m_3dParams(Params)
{
    DPF_ENTER();

    m_3dData = m_Default3dData;
    m_3dVoiceData = m_Default3dVoiceData;

    DPF_LEAVE_VOID();
}


/***************************************************************************
 *
 *  Calculate3d
 *
 *  Description:
 *      Commits deferred data to the device.
 *
 *  Arguments:
 *      DWORD [in]: parameter mask.
 *
 *  Returns:  
 *      (void)
 *
 ***************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CHrtfSource::Calculate3d"

void
CHrtfSource::Calculate3d
(   
    DWORD                   dwOperation
)
{
    DPF_ENTER();

    ASSERT((DS3DMODE_NORMAL == m_3dParams.HrtfParams.dwMode) || (DS3DMODE_HEADRELATIVE == m_3dParams.HrtfParams.dwMode));
    ASSERT(IsValidAlgorithm());

    //
    // Reset the changed voice data masks
    //

    m_3dVoiceData.dwChangeMask = 0;
    m_3dVoiceData.dwMixBinChangeMask = 0;

    //
    // Recalculate
    //

    ASSERT(m_pfnCalculate);
    m_pfnCalculate(dwOperation, this);

    DPF_LEAVE_VOID();
}


/***************************************************************************
 *
 *  Calculate3d
 *
 *  Description:
 *      Commits deferred data to the device.
 *
 *  Arguments:
 *      DWORD [in]: parameter mask.
 *      CHrtfSource * [in]: source object.
 *
 *  Returns:  
 *      (void)
 *
 ***************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CFullHrtfSource::Calculate3d"

void
CFullHrtfSource::Calculate3d
(   
    DWORD                   dwOperation,
    CHrtfSource *           pSource
)
{
    DPF_ENTER();

    if(dwOperation & MCPX_3DCALC_NORMPOS)
    {
        CalcNormPos(pSource);
    }

    if(dwOperation & MCPX_3DCALC_LEFTRIGHT)
    {
        CalcLeftRightGains(pSource);
    }

    if(dwOperation & MCPX_3DCALC_DISTANCE)
    {
        CalcDistanceVolume(pSource);
    }

    if(dwOperation & MCPX_3DCALC_DIRECTION)
    {
        CalcDirection(pSource);
    }

    if(dwOperation & MCPX_3DCALC_CONEVOLUME)
    {
        CalcConeVolume(pSource);
    }

    if(dwOperation & MCPX_3DCALC_FRONTREAR)
    {
        CalcFrontRearGains(pSource);
    }

    if(dwOperation & MCPX_3DCALC_DOPPLER)
    {
        CalcDoppler(pSource);
    }

#ifdef DS3D_ALLOW_ALGORITHM_SWAP

    pSource->m_3dVoiceData.dwMixBinChangeMask = pSource->m_3dVoiceData.dwMixBinValidMask;
    pSource->m_3dVoiceData.dwMixBinValidMask = 0;

#else // DS3D_ALLOW_ALGORITHM_SWAP

    ASSERT(!pSource->m_3dVoiceData.dwMixBinChangeMask);
    ASSERT(!pSource->m_3dVoiceData.dwMixBinValidMask);

#endif // DS3D_ALLOW_ALGORITHM_SWAP

    DPF_LEAVE_VOID();
}


/***************************************************************************
 *
 *  CalcNormPos
 *
 *  Description:
 *      Calculates the normalized source position.
 *
 *  Arguments:
 *      CHrtfSource * [in]: source object.
 *
 *  Returns:  
 *      (void)
 *
 ***************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CFullHrtfSource::CalcNormPos"

void
CFullHrtfSource::CalcNormPos
(   
    CHrtfSource *           pSource
)
{
    const D3DVECTOR &       vListenerPos    = pSource->m_Listener.m_3dParams.vPosition;
    const D3DVECTOR &       vSourcePos      = pSource->m_3dParams.HrtfParams.vPosition;
    D3DVECTOR &             vNormPos        = pSource->m_3dData.vNormPos;
    float &                 flMagPos        = pSource->m_3dData.flMagPos;

    DPF_ENTER();

    vNormPos.x = vSourcePos.x;
    vNormPos.y = vSourcePos.y;
    vNormPos.z = vSourcePos.z;

    if(DS3DMODE_HEADRELATIVE != pSource->m_3dParams.HrtfParams.dwMode)
    {
        vNormPos.x -= vListenerPos.x;
        vNormPos.y -= vListenerPos.y;
        vNormPos.z -= vListenerPos.z;
    }
    
    flMagPos = (float)NormalizeVector3(&vNormPos);

    DPF_LEAVE_VOID();
}


/***************************************************************************
 *
 *  CalcLeftRightGains
 *
 *  Description:
 *      Calculates left/right pan.
 *
 *  Arguments:
 *      CHrtfSource * [in]: source object.
 *
 *  Returns:  
 *      (void)
 *
 ***************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CFullHrtfSource::CalcLeftRightGains"

void
CFullHrtfSource::CalcLeftRightGains
(   
    CHrtfSource *           pSource
)
{
    const DWORD &           dwMode                  = pSource->m_3dParams.HrtfParams.dwMode;
    const DS3DLISTENER &    ListenerParams          = (DS3DMODE_HEADRELATIVE == dwMode) ? DirectSoundDefault3DListener : pSource->m_Listener.m_3dParams;
    const D3DVECTOR &       vListenerNormOrient     = (DS3DMODE_HEADRELATIVE == dwMode) ? CHrtfListener::m_vDefaultNormOrient : pSource->m_Listener.m_3dData.vNormOrient;
    const D3DVECTOR &       vListenerOrientFront    = ListenerParams.vOrientFront;
    const D3DVECTOR &       vListenerOrientTop      = ListenerParams.vOrientTop;
    const float &           flMagPos                = pSource->m_3dData.flMagPos;
    D3DVECTOR &             vNormPos                = pSource->m_3dData.vNormPos;
    float &                 flAzimuth               = pSource->m_3dData.flAzimuth;
    float &                 flElevation             = pSource->m_3dData.flElevation;
    D3DVECTOR               vPDot;
    float                   PdotT;
    float                   absPdotT;
    float                   magnitude;
    float                   absPdotF;
    float                   absPdotR;

    DPF_ENTER();

    //
    // Step 3a:  vPDot.x = PdotR, vPDot.y = 0, vPDot.z = PDotF
    //

    vPDot.z = (vNormPos.x * vListenerOrientFront.x) + (vNormPos.y * vListenerOrientFront.y) + (vNormPos.z * vListenerOrientFront.z);
    vPDot.x = (vNormPos.x * vListenerNormOrient.x) + (vNormPos.y * vListenerNormOrient.y) + (vNormPos.z * vListenerNormOrient.z);

    //
    // Step 3b:  calculate elevation
    //

    if(flMagPos)
    {
        PdotT = (vNormPos.x * vListenerOrientTop.x) + (vNormPos.y * vListenerOrientTop.y) + (vNormPos.z * vListenerOrientTop.z);
        absPdotT = fabsf(PdotT);

        magnitude = MagnitudeVector2(vPDot.x, vPDot.z);

        if(absPdotT < magnitude)
        {
            ASSERT( magnitude != 0.0f );
            flElevation = absPdotT / magnitude * 45.0f;
        }
        else
        {
            ASSERT( absPdotT != 0.0f );
            flElevation = 90.0f - (magnitude / absPdotT * 45.0f);
        }

        if(PdotT < 0.0f)
        {
            flElevation = -flElevation;
        }
    }
    else
    {
        flElevation = 0.0f;
    }

    //
    // Step 3c:  calculate azimuth
    //

    if(flMagPos)
    {
        absPdotF = fabsf(vPDot.z);
        absPdotR = fabsf(vPDot.x);

        if(absPdotF > absPdotR)
        {
            ASSERT( absPdotF != 0.0f );
            flAzimuth = absPdotR / absPdotF * 45.0f;
        }
        else if(absPdotR == 0.0f)
        {
            flAzimuth = 0.0f;
        }
        else
        {
            ASSERT(absPdotR != 0.0f);
            flAzimuth = 90.0f - (absPdotF / absPdotR * 45.0f);
        }
    }
    else
    {
        flAzimuth = 0.0f;
    }
        
    if(vPDot.z < 0.0f)
    {
        flAzimuth = 180.0f - flAzimuth;
    }

    if(vPDot.x < 0.0f)
    {
        flAzimuth = - flAzimuth;
    }

    //
    // Reload coefficients
    //

    GetHrtfFilterPair(pSource);

    DPF_LEAVE_VOID();
}


/***************************************************************************
 *
 *  CalcDistanceVolume
 *
 *  Description:
 *      Calculates distance attenuation.
 *
 *  Arguments:
 *      CHrtfSource * [in]: source object.
 *
 *  Returns:  
 *      (void)
 *
 ***************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CFullHrtfSource::CalcDistanceVolume"

void
CFullHrtfSource::CalcDistanceVolume
(   
    CHrtfSource *           pSource
)
{
    const float &           flSourceRolloffFactor   = pSource->m_3dParams.HrtfParams.flRolloffFactor;
    const float &           flListenerRolloffFactor = pSource->m_Listener.m_3dParams.flRolloffFactor;
    const float &           flMinDistance           = pSource->m_3dParams.HrtfParams.flMinDistance;
    const float &           flMaxDistance           = pSource->m_3dParams.HrtfParams.flMaxDistance;
    const float *           aflRolloffPoints        = pSource->m_3dParams.paflRolloffPoints;
    const DWORD             dwRolloffPointCount     = pSource->m_3dParams.dwRolloffPointCount;
    float                   flMagPos                = pSource->m_3dData.flMagPos;
    float                   flRolloffFactor;
    long                    lDistanceVolume;
    float                   flPointWidth;
    DWORD                   dwNextPoint;
    float                   aflLocalPoints[2];
    float                   flDistanceFromPoint;
    
    DPF_ENTER();

    //
    // If the position is less-than-or-equal-to MinDistance, there is no
    // distance attenuation.  
    //
    // If the position is greater-than-or-equal-to MaxDistance, the distance 
    // is floored.
    //
    // If a rolloff curve has been specified, we'll use that rather than
    // the standard rolloff factor.
    //

    if(flMagPos > flMinDistance)
    {
        if(flMagPos > flMaxDistance)
        {
            flMagPos = flMaxDistance;
        }

        if(aflRolloffPoints && dwRolloffPointCount)
        {
            //
            // How far (in meters) is the space between each point in the 
            // curve?
            //
            
            flPointWidth = (flMaxDistance - flMinDistance) / (float)dwRolloffPointCount;

            //
            // Store the two points the source is between locally.  We do this
            // because f[0] of the curve really specifies the first point after
            // MinDistance and f[n-1] is at MaxDistance.  This allows us to have 
            // a linear rolloff with only one point specified.
            //

            if((dwNextPoint = (DWORD)((flMagPos - flMinDistance) / flPointWidth)) >= dwRolloffPointCount)
            {
                ASSERT(dwRolloffPointCount == dwNextPoint);
                dwNextPoint = dwRolloffPointCount - 1;
            }

            if(!dwNextPoint)
            {
                aflLocalPoints[0] = 1.0f;
                aflLocalPoints[1] = aflRolloffPoints[0];

                flDistanceFromPoint = flMagPos;
            }
            else
            {
                aflLocalPoints[0] = aflRolloffPoints[dwNextPoint - 1];
                aflLocalPoints[1] = aflRolloffPoints[dwNextPoint];

                flDistanceFromPoint = flMagPos - ((float)dwNextPoint * flPointWidth);
            }

            flRolloffFactor = aflLocalPoints[0] + ((flDistanceFromPoint / flPointWidth) * (aflLocalPoints[1] - aflLocalPoints[0]));

            lDistanceVolume = AmplitudeToVolume(flRolloffFactor);
        }
        else
        {
            flRolloffFactor = flSourceRolloffFactor * flListenerRolloffFactor;

            lDistanceVolume = MetersToVolume(flRolloffFactor * (flMagPos / flMinDistance - 1.0f));
        }
    }
    else
    {
        lDistanceVolume = DSBVOLUME_MAX;
    }

    if(lDistanceVolume != pSource->m_3dVoiceData.lDistanceVolume)
    {
        pSource->m_3dVoiceData.lDistanceVolume = lDistanceVolume;
        pSource->m_3dVoiceData.dwChangeMask |= MCPX_3DAPPLY_DISTANCEVOLUME;
    }

    DPF_LEAVE_VOID();
}


/***************************************************************************
 *
 *  CalcDirection
 *
 *  Description:
 *      Calculates sound direction based on cone orientation.
 *
 *  Arguments:
 *      CHrtfSource * [in]: source object.
 *
 *  Returns:  
 *      (void)
 *
 ***************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CFullHrtfSource::CalcDirection"

void
CFullHrtfSource::CalcDirection
(   
    CHrtfSource *           pSource
)
{
    const D3DVECTOR &       vConeOrientation    = pSource->m_3dParams.HrtfParams.vConeOrientation;
    const D3DVECTOR &       vNormPos            = pSource->m_3dData.vNormPos;
    float &                 flThetaS            = pSource->m_3dData.flThetaS;
    float                   absCplusP;
    float                   absCminusP;
    D3DXVECTOR3             CplusP;
    D3DXVECTOR3             CminusP;

    DPF_ENTER();

    ASSERT(vConeOrientation.x || vConeOrientation.y || vConeOrientation.z);

    CminusP.x = vConeOrientation.x - vNormPos.x;
    CminusP.y = vConeOrientation.y - vNormPos.y;
    CminusP.z = vConeOrientation.z - vNormPos.z;
    
    absCminusP = MagnitudeVector3(&CminusP);

    CplusP.x = vConeOrientation.x + vNormPos.x;
    CplusP.y = vConeOrientation.y + vNormPos.y;
    CplusP.z = vConeOrientation.z + vNormPos.z;
    
    absCplusP = MagnitudeVector3(&CplusP);

    if(absCplusP < absCminusP)
    {
        ASSERT(absCminusP);
        flThetaS = 4.0f * (absCplusP / absCminusP * 45.0f);
    }

    else
    {
        ASSERT(absCplusP);
        flThetaS = 4.0f * (90.0f - (absCminusP / absCplusP * 45.0f));
    }

    DPF_LEAVE_VOID();
}


/***************************************************************************
 *
 *  CalcConeVolume
 *
 *  Description:
 *      Calculates cone volume.
 *
 *  Arguments:
 *      CHrtfSource * [in]: source object.
 *
 *  Returns:  
 *      (void)
 *
 ***************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CFullHrtfSource::CalcConeVolume"

void
CFullHrtfSource::CalcConeVolume
(   
    CHrtfSource *           pSource
)
{
    const DWORD &           dwInsideConeAngle   = pSource->m_3dParams.HrtfParams.dwInsideConeAngle;
    const DWORD &           dwOutsideConeAngle  = pSource->m_3dParams.HrtfParams.dwOutsideConeAngle;
    const long &            lConeOutsideVolume  = pSource->m_3dParams.HrtfParams.lConeOutsideVolume;
    const float &           flThetaS            = pSource->m_3dData.flThetaS;
    long                    lConeVolume;
    
    DPF_ENTER();

    ASSERT(dwInsideConeAngle <= dwOutsideConeAngle);

    if(flThetaS <= (float)dwInsideConeAngle)
    {
        lConeVolume = 0;
    }
    else if((dwInsideConeAngle < dwOutsideConeAngle) && (flThetaS >= (float)dwOutsideConeAngle))
    {
        lConeVolume = lConeOutsideVolume;
    }
    else
    {
        lConeVolume = (long)ftoi((float)lConeOutsideVolume * (flThetaS - (float)dwInsideConeAngle ) / (float)(max(1, dwOutsideConeAngle - dwInsideConeAngle)));
    }

    if(lConeVolume != pSource->m_3dVoiceData.lConeVolume)
    {
        pSource->m_3dVoiceData.lConeVolume = lConeVolume;
        pSource->m_3dVoiceData.dwChangeMask |= MCPX_3DAPPLY_CONEVOLUME;
    }

    DPF_LEAVE_VOID();
}


/***************************************************************************
 *
 *  CalcFrontRearGains
 *
 *  Description:
 *      Calculates front/rear gains.
 *
 *  Arguments:
 *      CHrtfSource * [in]: source object.
 *
 *  Returns:  
 *      (void)
 *
 ***************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CFullHrtfSource::CalcFrontRearGains"

void
CFullHrtfSource::CalcFrontRearGains
(   
    CHrtfSource *           pSource
)
{
    const float &           flMagPos                    = pSource->m_3dData.flMagPos;
    const float &           flSourceDistanceFactor      = pSource->m_3dParams.HrtfParams.flDistanceFactor;
    const float &           flListenerDistanceFactor    = pSource->m_Listener.m_3dParams.flDistanceFactor;
    const float &           flAzimuth                   = pSource->m_3dData.flAzimuth;
    const float &           flElevation                 = pSource->m_3dData.flElevation;
    float                   flDistance;
    float                   flFactor;
    LONG                    lFrontVolume;
    LONG                    lRearVolume;    

    DPF_ENTER();

    //
    // In surround mode, we'll calculate the proper front and rear gains.
    // In mono/stereo mode, front gains are always full and rear gains
    // are always silent.
    //

    if(pSource->m_Listener.m_fSurround)
    {
        //
        // Calculate front and rear gains based on position and distance factor.
        // Scaling factor is 0 for full front, 1 for full rear.
        //

        flDistance = flMagPos * flSourceDistanceFactor * flListenerDistanceFactor;

        flFactor = 0.5f * (1.0f + (fabsf(flAzimuth) / 90.0f - 1.0f) * (1.0f - fabsf(flElevation) / 90.0f));

        //
        // Convert to a range of -0.5 to 1.5 so that no attenuation happens until
        // the sound passes 45 degrees.
        //
    
        flFactor *= 2.0f;
        flFactor -= 0.5f;

        if(flFactor > 1.0f)
        {
            flFactor = 1.0f;
        }
        else if(flFactor < 0.0f)
        {
            flFactor = 0.0f;
        }

        //
        // Inside 0.5 meters, we crossfade to the 0, 0, 0 point at which we have 
        // equal front and rear gains.
        //

        if(flDistance < 0.5f)
        {
            flFactor = 0.5f + (flFactor - 0.5f) * flDistance / 2.0f;
        }

        //
        // Convert to millibels
        //

        lFrontVolume = PowerToVolume(1.0f - flFactor);
        lRearVolume = PowerToVolume(flFactor);
    }
    else
    {
        lFrontVolume = DSBVOLUME_MAX;
        lRearVolume = DSBVOLUME_MIN;
    }        

    if(lFrontVolume != pSource->m_3dVoiceData.lFrontVolume)
    {
        pSource->m_3dVoiceData.lFrontVolume = lFrontVolume;
        pSource->m_3dVoiceData.dwChangeMask |= MCPX_3DAPPLY_FRONTVOLUME;
    }

    if(lRearVolume != pSource->m_3dVoiceData.lRearVolume)
    {
        pSource->m_3dVoiceData.lRearVolume = lRearVolume;
        pSource->m_3dVoiceData.dwChangeMask |= MCPX_3DAPPLY_REARVOLUME;
    }

    DPF_LEAVE_VOID();
}


/***************************************************************************
 *
 *  CalcDoppler
 *
 *  Description:
 *      Calculates Doppler shift.
 *
 *  Arguments:
 *      CHrtfSource * [in]: source object.
 *
 *  Returns:  
 *      (void)
 *
 ***************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CFullHrtfSource::CalcDoppler"

void
CFullHrtfSource::CalcDoppler
(   
    CHrtfSource *           pSource
)
{
    static const float      flSpeedOfSound              = 342.0f;
    const D3DVECTOR &       vSourceVelocity             = pSource->m_3dParams.HrtfParams.vVelocity;
    const D3DVECTOR &       vListenerVelocity           = pSource->m_Listener.m_3dParams.vVelocity;
    const D3DVECTOR &       vNormPos                    = pSource->m_3dData.vNormPos;
    const float &           flSourceDistanceFactor      = pSource->m_3dParams.HrtfParams.flDistanceFactor;
    const float &           flListenerDistanceFactor    = pSource->m_Listener.m_3dParams.flDistanceFactor;
    const float &           flSourceDopplerFactor       = pSource->m_3dParams.HrtfParams.flDopplerFactor;
    const float &           flListenerDopplerFactor     = pSource->m_Listener.m_3dParams.flDopplerFactor;
    float                   flVelocity;
    LONG                    lDopplerPitch;
    
    DPF_ENTER();

    if(DS3DMODE_HEADRELATIVE == pSource->m_3dParams.HrtfParams.dwMode)
    {
        flVelocity = vSourceVelocity.x * vNormPos.x +
                     vSourceVelocity.y * vNormPos.y +
                     vSourceVelocity.z * vNormPos.z;
    }
    else
    {
        flVelocity = (vSourceVelocity.x - vListenerVelocity.x) * vNormPos.x +
                     (vSourceVelocity.y - vListenerVelocity.y) * vNormPos.y +
                     (vSourceVelocity.z - vListenerVelocity.z) * vNormPos.z;
    }

    flVelocity *= flSourceDistanceFactor * flListenerDistanceFactor;
    flVelocity *= flSourceDopplerFactor * flListenerDopplerFactor;

    if(!flVelocity)
    {
        lDopplerPitch = 0;
    }
    else if(flVelocity >= flSpeedOfSound)
    {
        lDopplerPitch = DSBPITCH_MIN;
    }
    else if(flVelocity <= -flSpeedOfSound)
    {
        lDopplerPitch = 4096;
    }
    else
    {
        lDopplerPitch = RatioToPitch(1.0f - flVelocity / flSpeedOfSound);
    }

    if(lDopplerPitch != pSource->m_3dVoiceData.lDopplerPitch)
    {
        pSource->m_3dVoiceData.lDopplerPitch = lDopplerPitch;
        pSource->m_3dVoiceData.dwChangeMask |= MCPX_3DAPPLY_DOPPLERPITCH;
    }

    DPF_LEAVE_VOID();
}


/****************************************************************************
 *
 *  GetHrtfFilterPair
 *
 *  Description:
 *      Loads HRTF coefficients.
 *
 *  Arguments:
 *      CHrtfSource * [in]: source object.
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CFullHrtfSource::GetHrtfFilterPair"

void
CFullHrtfSource::GetHrtfFilterPair
(
    CHrtfSource *           pSource
)
{
    static const WORD awFilterIndex[180 / 3 + 1][180 / 6 + 1] = 
    { 
        #include "hrtffull.idx" 
    };

    static const FIRFILTER8 aFilters[] = 
    { 
        #include "hrtffull.flt" 
    };

    static const int        nElevationInnerLimit    = 30;
    static const int        nElevationOuterLimit    = 60;
    const float &           flAzimuth               = pSource->m_3dData.flAzimuth;
    const float &           flElevation             = pSource->m_3dData.flElevation;
    int                     nElevation;
    int                     nAbsElevation;
    float                   flAbsAzimuth;
    int                     nAzimuth;
    int                     nIndex;
    HRTFFILTERPAIR          FilterPair;

    DPF_ENTER();

    ASSERT((flAzimuth >= -180.f) && (flAzimuth <= 180.f));
    ASSERT((flElevation >= -90.f) && (flElevation <= 90.f));

    //
    // Convert elevation to a table index.  The full HRTF table has 6 degrees
    // of elevation granularity.
    //

    if(flElevation >= 0.f)
    {
        nElevation = ftoi(flElevation + 3.f) / 6 * 6;
    }
    else
    {
        nElevation = ftoi(flElevation - 3.f) / 6 * 6;
    }

    nAbsElevation = abs(nElevation);

    //
    // Convert azimuth to a table index.  The full HRTF table has 3 degrees of
    // azimuth granularity.
    //

    flAbsAzimuth = fabsf(flAzimuth);

    if(90 == nAbsElevation)
    {
        nAzimuth = 0;
    }
    else if(nAbsElevation > nElevationOuterLimit)
    {
        nAzimuth = ftoi(flAbsAzimuth + 6.f) / 12 * 12;
    }
    else if(nAbsElevation > nElevationInnerLimit)
    {
        nAzimuth = ftoi(flAbsAzimuth + 3.f) / 6 * 6;
    }
    else
    {
        nAzimuth = ftoi(flAbsAzimuth + 1.5f) / 3 * 3;
    }

    //
    // If we're in quad mode, we don't want to use any of the coefficients 
    // with virtual rear low-pass filter
    //

    if(pSource->m_Listener.m_fSurround)
    {
        if(nAzimuth > 90)
        {
            nAzimuth = 180 - nAzimuth;
        }
    }

    //
    // Look up the filter index
    //

    nAzimuth /= 3;
    
    nElevation += 90;
    nElevation /= 6;

    if(nAzimuth >= NUMELMS(awFilterIndex))
    {
        ASSERTMSG("Azimuth out of range");
        nAzimuth = 0;
    }

    if(nElevation >= NUMELMS(awFilterIndex[0]))
    {
        ASSERTMSG("Elevation out of range");
        nElevation = 0;
    }

    nIndex = awFilterIndex[nAzimuth][nElevation];

    //
    // Load filters
    //

    ASSERT(nIndex < NUMELMS(aFilters) - 1);

    if(flAzimuth >= 0)
    {
        FilterPair.pLeftFilter = &aFilters[nIndex];
        FilterPair.pRightFilter = &aFilters[nIndex + 1];
    }
    else
    {
        FilterPair.pRightFilter = &aFilters[nIndex];
        FilterPair.pLeftFilter = &aFilters[nIndex + 1];
    }

    if((FilterPair.pLeftFilter != pSource->m_3dVoiceData.FilterPair.pLeftFilter) || (FilterPair.pRightFilter != pSource->m_3dVoiceData.FilterPair.pRightFilter))
    {
        pSource->m_3dVoiceData.FilterPair = FilterPair;
        pSource->m_3dVoiceData.dwChangeMask |= MCPX_3DAPPLY_FILTERPAIR;
    }

    DPF_LEAVE_VOID();
}


/***************************************************************************
 *
 *  Calculate3d
 *
 *  Description:
 *      Commits deferred data to the device.
 *
 *  Arguments:
 *      DWORD [in]: parameter mask.
 *      CHrtfSource * [in]: source object.
 *
 *  Returns:  
 *      (void)
 *
 ***************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CLightHrtfSource::Calculate3d"

void
CLightHrtfSource::Calculate3d
(   
    DWORD                   dwOperation,
    CHrtfSource *           pSource
)
{
    DPF_ENTER();

    if(dwOperation & MCPX_3DCALC_NORMPOS)
    {
        CalcNormPos(pSource);
    }

    if(dwOperation & MCPX_3DCALC_LEFTRIGHT)
    {
        CalcLeftRightGains(pSource);
    }

    if(dwOperation & MCPX_3DCALC_DISTANCE)
    {
        CFullHrtfSource::CalcDistanceVolume(pSource);
    }

    if(dwOperation & MCPX_3DCALC_FRONTREAR)
    {
        CalcFrontRearGains(pSource);
    }

    if(dwOperation & MCPX_3DCALC_DOPPLER)
    {
        CFullHrtfSource::CalcDoppler(pSource);
    }

#ifdef DS3D_ALLOW_ALGORITHM_SWAP

    if(pSource->m_3dVoiceData.lConeVolume)
    {
        pSource->m_3dVoiceData.lConeVolume = 0;
        pSource->m_3dVoiceData.dwChangeMask |= MCPX_3DAPPLY_CONEVOLUME;
    }

    pSource->m_3dVoiceData.dwMixBinChangeMask = pSource->m_3dVoiceData.dwMixBinValidMask;
    pSource->m_3dVoiceData.dwMixBinValidMask = 0;

#else // DS3D_ALLOW_ALGORITHM_SWAP

    ASSERT(!pSource->m_3dVoiceData.lConeVolume);
    ASSERT(!pSource->m_3dVoiceData.dwMixBinValidMask);
    ASSERT(!pSource->m_3dVoiceData.dwMixBinChangeMask);

#endif // DS3D_ALLOW_ALGORITHM_SWAP

    DPF_LEAVE_VOID();
}


/***************************************************************************
 *
 *  CalcNormPos
 *
 *  Description:
 *      Calculates the normalized source position.
 *
 *  Arguments:
 *      CHrtfSource * [in]: source object.
 *
 *  Returns:  
 *      (void)
 *
 ***************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CLightHrtfSource::CalcNormPos"

void
CLightHrtfSource::CalcNormPos
(   
    CHrtfSource *           pSource
)
{
    const D3DVECTOR &       vSourcePos      = pSource->m_3dParams.HrtfParams.vPosition;
    const D3DVECTOR &       vListenerPos    = pSource->m_Listener.m_3dParams.vPosition;
    D3DVECTOR &             vNormPos        = pSource->m_3dData.vNormPos;
    float &                 flMagPos        = pSource->m_3dData.flMagPos;

    DPF_ENTER();

    vNormPos.x = vSourcePos.x;
    vNormPos.z = vSourcePos.z;

    if(DS3DMODE_HEADRELATIVE != pSource->m_3dParams.HrtfParams.dwMode)
    {
        vNormPos.x -= vListenerPos.x;
        vNormPos.z -= vListenerPos.z;
    }
    
    flMagPos = (float)NormalizeVector2(&vNormPos);

    DPF_LEAVE_VOID();
}


/***************************************************************************
 *
 *  CalcLeftRightGains
 *
 *  Description:
 *      Calculates left/right pan.
 *
 *  Arguments:
 *      CHrtfSource * [in]: source object.
 *
 *  Returns:  
 *      (void)
 *
 ***************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CLightHrtfSource::CalcLeftRightGains"

void
CLightHrtfSource::CalcLeftRightGains
(   
    CHrtfSource *           pSource
)
{
    const DWORD &           dwMode                  = pSource->m_3dParams.HrtfParams.dwMode;
    const DS3DLISTENER &    ListenerParams          = (DS3DMODE_HEADRELATIVE == dwMode) ? DirectSoundDefault3DListener : pSource->m_Listener.m_3dParams;
    const D3DVECTOR &       vListenerNormOrient     = (DS3DMODE_HEADRELATIVE == dwMode) ? CHrtfListener::m_vDefaultNormOrient : pSource->m_Listener.m_3dData.vNormOrient;
    const D3DVECTOR &       vListenerOrientFront    = ListenerParams.vOrientFront;
    const D3DVECTOR &       vListenerOrientTop      = ListenerParams.vOrientTop;
    const float &           flMagPos                = pSource->m_3dData.flMagPos;
    D3DVECTOR &             vNormPos                = pSource->m_3dData.vNormPos;
    float &                 flAzimuth               = pSource->m_3dData.flAzimuth;
    float &                 flElevation             = pSource->m_3dData.flElevation;
    D3DVECTOR               vPDot;
    float                   PdotT;
    float                   absPdotT;
    float                   magnitude;
    float                   absPdotF;
    float                   absPdotR;

    DPF_ENTER();

    //
    // Step 3a:  vPDot.x = PdotR, vPDot.y = 0, vPDot.z = PDotF
    //

    vPDot.z = (vNormPos.x * vListenerOrientFront.x) + (vNormPos.y * vListenerOrientFront.y) + (vNormPos.z * vListenerOrientFront.z);
    vPDot.x = (vNormPos.x * vListenerNormOrient.x) + (vNormPos.y * vListenerNormOrient.y) + (vNormPos.z * vListenerNormOrient.z);

    //
    // Step 3c:  calculate azimuth
    //

    if(flMagPos)
    {
        absPdotF = fabsf(vPDot.z);
        absPdotR = fabsf(vPDot.x);

        if(absPdotF > absPdotR)
        {
            ASSERT( absPdotF != 0.0f );
            flAzimuth = absPdotR / absPdotF * 45.0f;
        }
        else if(absPdotR == 0.0f)
        {
            flAzimuth = 0.0f;
        }
        else
        {
            ASSERT(absPdotR != 0.0f);
            flAzimuth = 90.0f - (absPdotF / absPdotR * 45.0f);
        }
    }
    else
    {
        flAzimuth = 0.0f;
    }
        
    if(vPDot.z < 0.0f)
    {
        flAzimuth = 180.0f - flAzimuth;
    }

    if(vPDot.x < 0.0f)
    {
        flAzimuth = - flAzimuth;
    }

    //
    // Reload coefficients
    //

    GetHrtfFilterPair(pSource);

    DPF_LEAVE_VOID();
}


/***************************************************************************
 *
 *  CalcFrontRearGains
 *
 *  Description:
 *      Calculates front/rear gains.
 *
 *  Arguments:
 *      CHrtfSource * [in]: source object.
 *
 *  Returns:  
 *      (void)
 *
 ***************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CLightHrtfSource::CalcFrontRearGains"

void
CLightHrtfSource::CalcFrontRearGains
(   
    CHrtfSource *           pSource
)
{
    const float &           flMagPos                    = pSource->m_3dData.flMagPos;
    const float &           flSourceDistanceFactor      = pSource->m_3dParams.HrtfParams.flDistanceFactor;
    const float &           flListenerDistanceFactor    = pSource->m_Listener.m_3dParams.flDistanceFactor;
    const float &           flAzimuth                   = pSource->m_3dData.flAzimuth;
    const float &           flElevation                 = pSource->m_3dData.flElevation;
    float                   flDistance;
    float                   flFactor;
    LONG                    lFrontVolume;
    LONG                    lRearVolume;    

    DPF_ENTER();

    //
    // In surround mode, we'll calculate the proper front and rear gains.
    // In mono/stereo mode, front gains are always full and rear gains
    // are always silent.
    //

    if(pSource->m_Listener.m_fSurround)
    {
        //
        // Calculate front and rear gains based on position and distance factor.
        // Scaling factor is 0 for full front, 1 for full rear.
        //

        flDistance = flMagPos * flSourceDistanceFactor * flListenerDistanceFactor;

        flFactor = 0.5f * (1.0f + (fabsf(flAzimuth) / 90.0f - 1.0f));

        //
        // Convert to a range of -0.5 to 1.5 so that no attenuation happens until
        // the sound passes 45 degrees.
        //
    
        flFactor *= 2.0f;
        flFactor -= 0.5f;

        if(flFactor > 1.0f)
        {
            flFactor = 1.0f;
        }
        else if(flFactor < 0.0f)
        {
            flFactor = 0.0f;
        }

        //
        // Inside 0.5 meters, we crossfade to the 0, 0, 0 point at which we have 
        // equal front and rear gains.
        //

        if(flDistance < 0.5f)
        {
            flFactor = 0.5f + (flFactor - 0.5f) * flDistance / 2.0f;
        }

        //
        // Convert to millibels
        //

        lFrontVolume = PowerToVolume(1.0f - flFactor);
        lRearVolume = PowerToVolume(flFactor);
    }
    else
    {
        lFrontVolume = DSBVOLUME_MAX;
        lRearVolume = DSBVOLUME_MIN;
    }        

    if(lFrontVolume != pSource->m_3dVoiceData.lFrontVolume)
    {
        pSource->m_3dVoiceData.lFrontVolume = lFrontVolume;
        pSource->m_3dVoiceData.dwChangeMask |= MCPX_3DAPPLY_FRONTVOLUME;
    }

    if(lRearVolume != pSource->m_3dVoiceData.lRearVolume)
    {
        pSource->m_3dVoiceData.lRearVolume = lRearVolume;
        pSource->m_3dVoiceData.dwChangeMask |= MCPX_3DAPPLY_REARVOLUME;
    }

    DPF_LEAVE_VOID();
}


/****************************************************************************
 *
 *  GetHrtfFilterPair
 *
 *  Description:
 *      Loads HRTF coefficients.
 *
 *  Arguments:
 *      CHrtfSource * [in]: source object.
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CLightHrtfSource::GetHrtfFilterPair"

void
CLightHrtfSource::GetHrtfFilterPair
(
    CHrtfSource *           pSource
)
{
    static const FIRFILTER8 aFilters[] = 
    { 
        #include "hrtflite.flt" 
    };

    const float &           flAzimuth       = pSource->m_3dData.flAzimuth;
    const float &           flElevation     = pSource->m_3dData.flElevation;
    float                   flAbsAzimuth;
    int                     nAzimuth;
    int                     nIndex;
    HRTFFILTERPAIR          FilterPair;

    DPF_ENTER();

    ASSERT((flAzimuth >= -180.f) && (flAzimuth <= 180.f));
    ASSERT(!flElevation);

    //
    // Convert azimuth to a table index.  The azimuth HRTF table has 3 degrees 
    // of granularity.
    //

    flAbsAzimuth = fabsf(flAzimuth);

    nAzimuth = ftoi(flAbsAzimuth + 1.5f) / 3 * 3;

    //
    // If we're in quad mode, we don't want to use any of the coefficients 
    // with virtual rear low-pass filter
    //

    if(pSource->m_Listener.m_fSurround)
    {
        if(nAzimuth > 90)
        {
            nAzimuth = 180 - nAzimuth;
        }
    }

    //
    // Calculate the filter index
    //

    if(nAzimuth > 180)
    {
        ASSERTMSG("Azimuth out of range");
        nAzimuth = 0;
    }
    
    nIndex = ((180 - nAzimuth) / 3) * 2;

    //
    // Load filters
    //

    ASSERT(nIndex >= 0);
    ASSERT(nIndex < NUMELMS(aFilters) - 1);

    if(flAzimuth >= 0)
    {
        FilterPair.pLeftFilter = &aFilters[nIndex];
        FilterPair.pRightFilter = &aFilters[nIndex + 1];
    }
    else
    {
        FilterPair.pRightFilter = &aFilters[nIndex];
        FilterPair.pLeftFilter = &aFilters[nIndex + 1];
    }

    if((FilterPair.pLeftFilter != pSource->m_3dVoiceData.FilterPair.pLeftFilter) || (FilterPair.pRightFilter != pSource->m_3dVoiceData.FilterPair.pRightFilter))
    {
        pSource->m_3dVoiceData.FilterPair = FilterPair;
        pSource->m_3dVoiceData.dwChangeMask |= MCPX_3DAPPLY_FILTERPAIR;
    }

    DPF_LEAVE_VOID();
}


/***************************************************************************
 *
 *  Calculate3d
 *
 *  Description:
 *      Commits deferred data to the device.
 *
 *  Arguments:
 *      DWORD [in]: parameter mask.
 *      CHrtfSource * [in]: source object.
 *
 *  Returns:  
 *      (void)
 *
 ***************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CPan3dSource::Calculate3d"

void
CPan3dSource::Calculate3d
(   
    DWORD                   dwOperation,
    CHrtfSource *           pSource
)
{
    DPF_ENTER();

    if(dwOperation & MCPX_3DCALC_NORMPOS)
    {
        CLightHrtfSource::CalcNormPos(pSource);
    }

    if(dwOperation & (MCPX_3DCALC_LEFTRIGHT | MCPX_3DCALC_FRONTREAR))
    {
        CalcPan(pSource);
    }

    if(dwOperation & MCPX_3DCALC_DISTANCE)
    {
        CFullHrtfSource::CalcDistanceVolume(pSource);
    }

    if(dwOperation & MCPX_3DCALC_DOPPLER)
    {
        CFullHrtfSource::CalcDoppler(pSource);
    }

#ifdef DS3D_ALLOW_ALGORITHM_SWAP

    if(pSource->m_3dVoiceData.lConeVolume)
    {
        pSource->m_3dVoiceData.lConeVolume = 0;
        pSource->m_3dVoiceData.dwChangeMask |= MCPX_3DAPPLY_CONEVOLUME;
    }

    if(pSource->m_3dVoiceData.lFrontVolume)
    {
        pSource->m_3dVoiceData.lFrontVolume = 0;
        pSource->m_3dVoiceData.dwChangeMask |= MCPX_3DAPPLY_FRONTVOLUME;
    }

    if(pSource->m_3dVoiceData.lRearVolume)
    {
        pSource->m_3dVoiceData.lRearVolume = 0;
        pSource->m_3dVoiceData.dwChangeMask |= MCPX_3DAPPLY_REARVOLUME;
    }

    if(pSource->m_3dVoiceData.FilterPair.pLeftFilter || pSource->m_3dVoiceData.FilterPair.pRightFilter)
    {
        pSource->m_3dVoiceData.FilterPair.pLeftFilter = NULL;
        pSource->m_3dVoiceData.FilterPair.pRightFilter = NULL;
        pSource->m_3dVoiceData.dwChangeMask |= MCPX_3DAPPLY_FILTERPAIR;
    }

#else // DS3D_ALLOW_ALGORITHM_SWAP

    ASSERT(!pSource->m_3dVoiceData.lConeVolume);
    ASSERT(!pSource->m_3dVoiceData.lFrontVolume);
    ASSERT(!pSource->m_3dVoiceData.lRearVolume);
    ASSERT(!pSource->m_3dVoiceData.FilterPair.pLeftFilter);
    ASSERT(!pSource->m_3dVoiceData.FilterPair.pRightFilter);

#endif // DS3D_ALLOW_ALGORITHM_SWAP

    DPF_LEAVE_VOID();
}


/***************************************************************************
 *
 *  CalcPan
 *
 *  Description:
 *      Calculates left/right pan.
 *
 *  Arguments:
 *      CHrtfSource * [in]: source object.
 *
 *  Returns:  
 *      (void)
 *
 ***************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CPan3dSource::CalcPan"

void
CPan3dSource::CalcPan
(   
    CHrtfSource *           pSource
)
{
    static const float      flAlpha             = 0.25f;
    static const float      flVolumeRange       = 6400.0f;
    static const float      flDCOffset          = 2400.0f;
    const DWORD &           dwMode              = pSource->m_3dParams.HrtfParams.dwMode;
    const LPCPAN3DSPEAKER   aSpeakers           = (DS3DMODE_HEADRELATIVE == dwMode) ? CHrtfListener::m_aDefaultSpeakers : pSource->m_Listener.m_3dData.aSpeakers;
    const float &           flMagPos            = pSource->m_3dData.flMagPos;
    const D3DVECTOR &       vNormPos            = pSource->m_3dData.vNormPos;
    DWORD &                 dwMixBinChangeMask  = pSource->m_3dVoiceData.dwMixBinChangeMask;
    DWORD &                 dwMixBinValidMask   = pSource->m_3dVoiceData.dwMixBinValidMask;
    LPLONG                  alMixBinVolumes     = pSource->m_3dVoiceData.alMixBinVolumes;
    D3DVECTOR               vSpeakerNorm;
    FLOAT                   flSpeakerMag;
    FLOAT                   flSpeakerFactor;
    LONG                    lVolume;
    DWORD                   dwSpeakerCount;
    DWORD                   i;
    
    DPF_ENTER();

    ASSERT(!dwMixBinChangeMask);
    dwMixBinValidMask = 0;

    //
    // If we're not running in surround mode, only calculate the front
    // speakers
    //

    if(!flMagPos)
    {
        dwSpeakerCount = 0;
    }
    else if(!pSource->m_Listener.m_fSurround)
    {
        ASSERT(PAN3D_SPEAKER_COUNT >= 2);
        
        ASSERT((DSMIXBIN_3D_FRONT_LEFT == aSpeakers[0].dwMixBin) || (DSMIXBIN_3D_FRONT_RIGHT == aSpeakers[0].dwMixBin));
        ASSERT((DSMIXBIN_3D_FRONT_LEFT == aSpeakers[1].dwMixBin) || (DSMIXBIN_3D_FRONT_RIGHT == aSpeakers[1].dwMixBin));

        dwSpeakerCount = 2;
    }
    else
    {
        dwSpeakerCount = PAN3D_SPEAKER_COUNT;
    }

    //
    // Calculate volume for each speaker.  We're calculating volume for 
    // each speaker, regardless of speaker config so that processing takes
    // the same amount of time.
    //
    // Distance and rolloff attenuation are taken care of in a different
    // function using different data members, so this is strictly pan.
    //

    for(i = 0; i < dwSpeakerCount; i++)
    {

        // alternate algorithm but more cpu intensive
#if 0
        vSpeakerNorm.x = (vNormPos.x - aSpeakers[i].vSpeakerPos.x);
        vSpeakerNorm.y = (vNormPos.y - aSpeakers[i].vSpeakerPos.y);
        vSpeakerNorm.z = (vNormPos.z - aSpeakers[i].vSpeakerPos.z);

        flSpeakerMag = NormalizeVector3(&vSpeakerNorm);
        flSpeakerFactor = 1.0f - (float)exp(-pow(2.0f, flSpeakerMag) * flAlpha);

        ASSERT((flSpeakerFactor >= 0.0f) && (flSpeakerFactor <= 1.0f));
        
        lVolume = (long)(flSpeakerFactor * -flVolumeRange + flDCOffset);

#else
        vSpeakerNorm.x = (vNormPos.x - aSpeakers[i].vSpeakerPos.x)/2;
        vSpeakerNorm.y = (vNormPos.y - aSpeakers[i].vSpeakerPos.y)/2;
        vSpeakerNorm.z = (vNormPos.z - aSpeakers[i].vSpeakerPos.z)/2;

        flSpeakerMag =(FLOAT)(vSpeakerNorm.x*vSpeakerNorm.x +\
            vSpeakerNorm.y*vSpeakerNorm.y +\
            vSpeakerNorm.z*vSpeakerNorm.z);
        
        flSpeakerFactor = flSpeakerMag;

        ASSERT((flSpeakerFactor >= 0.0f) && (flSpeakerFactor <= 1.0f));            
        lVolume = (long)(flSpeakerFactor * -flVolumeRange);

#endif

        
        CHECKRANGE(lVolume, DSBVOLUME_MIN, DSBVOLUME_MAX);
        
        if(lVolume != alMixBinVolumes[aSpeakers[i].dwMixBin])
        {
            alMixBinVolumes[aSpeakers[i].dwMixBin] = lVolume;
            dwMixBinChangeMask |= 1UL << aSpeakers[i].dwMixBin;
        }
        
        dwMixBinValidMask |= 1UL << aSpeakers[i].dwMixBin;
    }

    //
    // Fill in the remaining speakers we would ever recalc, but might not
    // have this time (for example, when position is <= 0, or the speaker
    // config changes).
    //
    
    for(; i < PAN3D_SPEAKER_COUNT; i++)
    {
        if(alMixBinVolumes[aSpeakers[i].dwMixBin])
        {
            alMixBinVolumes[aSpeakers[i].dwMixBin] = 0;
            dwMixBinChangeMask |= 1UL << aSpeakers[i].dwMixBin;
        }

        dwMixBinValidMask |= 1UL << aSpeakers[i].dwMixBin;
    }

    DPF_LEAVE_VOID();
}


/****************************************************************************
 *
 *  GetHrtfFilterPair
 *
 *  Description:
 *      Loads HRTF coefficients.
 *
 *  Arguments:
 *      FLOAT [in]: azimuth.
 *      FLOAT [in]: elevation.
 *      BOOL [in]: TRUE if the speaker configuration is a quad setup.
 *      LPHRTFFILTERPAIR [out]: filter pair.
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CPan3dSource::GetHrtfFilterPair"

void
CPan3dSource::GetHrtfFilterPair
(
    CHrtfSource *           pSource
)
{
    DPF_ENTER();

#ifdef DS3D_ALLOW_ALGORITHM_SWAP

    if(pSource->m_3dVoiceData.FilterPair.pLeftFilter || pSource->m_3dVoiceData.FilterPair.pRightFilter)
    {
        pSource->m_3dVoiceData.FilterPair.pLeftFilter = NULL;
        pSource->m_3dVoiceData.FilterPair.pRightFilter = NULL;
        pSource->m_3dVoiceData.dwChangeMask |= MCPX_3DAPPLY_FILTERPAIR;
    }

#else // DS3D_ALLOW_ALGORITHM_SWAP
    
    ASSERT(!pSource->m_3dVoiceData.FilterPair.pLeftFilter);
    ASSERT(!pSource->m_3dVoiceData.FilterPair.pRightFilter);

#endif // DS3D_ALLOW_ALGORITHM_SWAP
    
    DPF_LEAVE_VOID();
}


