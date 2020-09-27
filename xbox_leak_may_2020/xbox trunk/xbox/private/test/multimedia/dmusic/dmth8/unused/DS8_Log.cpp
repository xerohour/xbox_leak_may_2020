//==========================================================================;
//
//  THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
//  KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
//  IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
//  PURPOSE.
//
//  Copyright (c) 1992, 1994  Microsoft Corporation.  All Rights Reserved.
//
//--------------------------------------------------------------------------;
//
//  DS8_Log.cpp
//
//  Description:
//      Utilities used by DSound 8 interfaces
//
//  History:
//      04/10/00    danhaff         Copied from DSAPI8
//      04/12/00    danhaff         Made everything _stdcall
//                                  moved protos into dmth.h
//      04/14/00    danhaff         Added comparision funx for Send,
//                                  WavesReverb, and DS3DBUFFERs.
//==========================================================================;
#include "dmth.h"
//#include "dmthcom.h"
#include "dmthhelp.h"
//#include <mmreg.h>
//#include <ks.h>
//#include <ksmedia.h>
#include <dsound.h>
#include <stdio.h>
#include <tchar.h>
#include <math.h>
#include <assert.h>

#define ASSERT assert

bool IsfEqual(float f1, float f2)
{
    if (fabs(f1 - f2) < FLT_EPSILON)
        return true;
    else
        return false;

};


bool operator == (D3DVECTOR &v1, D3DVECTOR &v2)
{
    if (
        ( IsfEqual(v1.x, v2.x)) &&
        ( IsfEqual(v1.y, v2.y)) &&
        ( IsfEqual(v1.z, v2.z))
       )
        return true;
    else
        return false;

};


void _stdcall Log_Chorus_Params( int nLogLvl, LPCDSFXChorus lpDSFXC )
{
    fnsIncrementIndent();
    tstLof( nLogLvl, "DSFXChorus.fWetDryMix  = %g", lpDSFXC->fWetDryMix );
    tstLof( nLogLvl, "DSFXChorus.fDepth      = %g", lpDSFXC->fDepth );
    tstLof( nLogLvl, "DSFXChorus.fFeedback   = %g", lpDSFXC->fFeedback );
    tstLof( nLogLvl, "DSFXChorus.fFrequency  = %g", lpDSFXC->fFrequency );
    fnsLog( nLogLvl, "DSFXChorus.lWaveform   = %ld", lpDSFXC->lWaveform );
    tstLof( nLogLvl, "DSFXChorus.fDelay      = %g", lpDSFXC->fDelay );
    fnsLog( nLogLvl, "DSFXChorus.lPhase      = %ld", lpDSFXC->lPhase );
    fnsDecrementIndent();
}

void _stdcall Log_Flanger_Params( int nLogLvl, LPCDSFXFlanger lpDSFXF )
{
    fnsIncrementIndent();
    tstLof( nLogLvl, "DSFXFlanger.fWetDryMix = %g", lpDSFXF->fWetDryMix );
    tstLof( nLogLvl, "DSFXFlanger.fDepth     = %g", lpDSFXF->fDepth );
    tstLof( nLogLvl, "DSFXFlanger.fFeedback  = %g", lpDSFXF->fFeedback );
    tstLof( nLogLvl, "DSFXFlanger.fFrequency = %g", lpDSFXF->fFrequency );
    fnsLog( nLogLvl, "DSFXFlanger.lWaveform  = %ld",lpDSFXF->lWaveform );
    tstLof( nLogLvl, "DSFXFlanger.fDelay     = %g", lpDSFXF->fDelay );
    fnsLog( nLogLvl, "DSFXFlanger.lPhase     = %ld",lpDSFXF->lPhase );
    fnsDecrementIndent();
}

/*
void _stdcall Log_Send_Params( int nLogLvl, LPCDSFXSend lpDSFXS )
{
    fnsIncrementIndent();
    fnsLog( nLogLvl, "DSFXSend.lSendLevel     = %ld", lpDSFXS->lSendLevel );
    fnsDecrementIndent();
}
*/

void _stdcall Log_Echo_Params( int nLogLvl, LPCDSFXEcho lpDSFXE )
{
    fnsIncrementIndent();
    tstLof( nLogLvl, "DSFXEcho.fWetDryMix        = %g", lpDSFXE->fWetDryMix );
    tstLof( nLogLvl, "DSFXEcho.fLeftDelay        = %g", lpDSFXE->fLeftDelay );
    tstLof( nLogLvl, "DSFXEcho.fRightDelay       = %g", lpDSFXE->fRightDelay );
    tstLof( nLogLvl, "DSFXEcho.fFeedback         = %g", lpDSFXE->fFeedback );
    tstLof( nLogLvl, "DSFXEcho.lPanDelay         = %g", lpDSFXE->lPanDelay );
    fnsDecrementIndent();
}

void _stdcall Log_Distortion_Params( int nLogLvl, LPCDSFXDistortion lpDSFXD )
{
    fnsIncrementIndent();
    tstLof( nLogLvl, "DSFXDistortion.fGain                   = %g", lpDSFXD->fGain );
    tstLof( nLogLvl, "DSFXDistortion.fEdge                   = %g", lpDSFXD->fEdge );
    tstLof( nLogLvl, "DSFXDistortion.fPostEQCenterFrequency  = %g", lpDSFXD->fPostEQCenterFrequency );
    tstLof( nLogLvl, "DSFXDistortion.fPostEQBandwidth        = %g", lpDSFXD->fPostEQBandwidth );
    tstLof( nLogLvl, "DSFXDistortion.fPreLowpassCutoff       = %g", lpDSFXD->fPreLowpassCutoff );
    fnsDecrementIndent();
}

void _stdcall Log_Compressor_Params( int nLogLvl, LPCDSFXCompressor lpDSFXCP )
{
    fnsIncrementIndent();
    tstLof( nLogLvl, "DSFXCompressor.fGain               = %g", lpDSFXCP->fGain );
    tstLof( nLogLvl, "DSFXCompressor.fAttack             = %g", lpDSFXCP->fAttack );
    tstLof( nLogLvl, "DSFXCompressor.fRelease            = %g", lpDSFXCP->fRelease );
    tstLof( nLogLvl, "DSFXCompressor.fThreshold          = %g", lpDSFXCP->fThreshold );
    tstLof( nLogLvl, "DSFXCompressor.fRatio              = %g", lpDSFXCP->fRatio );
    tstLof( nLogLvl, "DSFXCompressor.fPredelay           = %g", lpDSFXCP->fPredelay );
    fnsDecrementIndent();
}

void _stdcall Log_I3DL2Reverb_Params( int nLogLvl, LPCDSFXI3DL2Reverb lpDSFX3R )
{
    fnsIncrementIndent();
    fnsLog( nLogLvl, "DSFX_I3DL2Reverb.lRoom                  = %ld",    lpDSFX3R->lRoom );
    fnsLog( nLogLvl, "DSFX_I3DL2Reverb.lRoomHF                = %ld",    lpDSFX3R->lRoomHF );
    tstLof( nLogLvl, "DSFX_I3DL2Reverb.flRoomRolloffFactor    = %g",     lpDSFX3R->flRoomRolloffFactor );
    tstLof( nLogLvl, "DSFX_I3DL2Reverb.flDecayTime            = %g",     lpDSFX3R->flDecayTime );
    tstLof( nLogLvl, "DSFX_I3DL2Reverb.flDecayHFRatio         = %g",     lpDSFX3R->flDecayHFRatio );
    fnsLog( nLogLvl, "DSFX_I3DL2Reverb.lReflections           = %ld",    lpDSFX3R->lReflections );
    tstLof( nLogLvl, "DSFX_I3DL2Reverb.flReflectionsDelay     = %g",     lpDSFX3R->flReflectionsDelay );
    fnsLog( nLogLvl, "DSFX_I3DL2Reverb.lReverb                = %ld",    lpDSFX3R->lReverb );
    tstLof( nLogLvl, "DSFX_I3DL2Reverb.flReverbDelay          = %g",     lpDSFX3R->flReverbDelay );
    tstLof( nLogLvl, "DSFX_I3DL2Reverb.flDiffusion            = %g",     lpDSFX3R->flDiffusion );
    tstLof( nLogLvl, "DSFX_I3DL2Reverb.flDensity              = %g",     lpDSFX3R->flDensity );
    tstLof( nLogLvl, "DSFX_I3DL2Reverb.flHFReference          = %g",     lpDSFX3R->flHFReference );
    fnsDecrementIndent();
}

void _stdcall Log_WavesReverb_Params( int nLogLvl, LPCDSFXWavesReverb lpDSFXWR )
{
    fnsIncrementIndent();
    tstLof( nLogLvl, "DSFX_WavesReverb.fInGain                = %g",     lpDSFXWR->fInGain);
    tstLof( nLogLvl, "DSFX_WavesReverb.fReverbMix             = %g",     lpDSFXWR->fReverbMix);
    tstLof( nLogLvl, "DSFX_WavesReverb.fReverbTime            = %g",     lpDSFXWR->fReverbTime);
    tstLof( nLogLvl, "DSFX_WavesReverb.fHighFreqRTRatio       = %g",     lpDSFXWR->fHighFreqRTRatio);
    fnsDecrementIndent();
};


void _stdcall Log_Gargle_Params( int nLogLvl, LPCDSFXGargle lpDSFXG )
{
    fnsIncrementIndent();
    tstLof( nLogLvl, "DSFXGargle.dwRateHz        = %g", lpDSFXG->dwRateHz );
    tstLof( nLogLvl, "DSFXGargle.dwWaveShape     = %g", lpDSFXG->dwWaveShape );
    fnsDecrementIndent();
}

void _stdcall Log_ParamEq_Params( int nLogLvl, LPCDSFXParamEq lpDSFXP )
{
    fnsIncrementIndent();
    tstLof( nLogLvl, "DSFXParamEq.fCenter        = %g", lpDSFXP->fCenter );
    tstLof( nLogLvl, "DSFXParamEq.fBandwidth     = %g", lpDSFXP->fBandwidth );
    tstLof( nLogLvl, "DSFXParamEq.fGain          = %g", lpDSFXP->fGain );
    fnsDecrementIndent();
}

void _stdcall String_Chorus_Params( char *szMessage, char* szPrompt, LPCDSFXChorus lpDSFXC )
{
    sprintf( szMessage,
        "%s\n    \
DSFXChorus.fWetDryMix   = %g\n    \
DSFXChorus.fDepth       = %g\n    \
DSFXChorus.fFeedback    = %g\n    \
DSFXChorus.fFrequency   = %g\n    \
DSFXChorus.lWaveform    = %ld\n    \
DSFXChorus.fDelay       = %g\n    \
DSFXChorus.lPhase       = %ld\n",
        szPrompt,
        lpDSFXC->fWetDryMix,
        lpDSFXC->fDepth,
        lpDSFXC->fFeedback,
        lpDSFXC->fFrequency,
        lpDSFXC->lWaveform,
        lpDSFXC->fDelay,
        lpDSFXC->lPhase );
}

void _stdcall String_Flanger_Params( char *szMessage, char* szPrompt, LPCDSFXFlanger lpDSFXF )
{
    sprintf( szMessage,
        "%s\n    DSFXFlanger.fDepth      = %g\n    \
DSFXFlanger.fWetDryMix  = %g\n    \
DSFXFlanger.fFeedback   = %g\n    \
DSFXFlanger.fFrequency  = %g\n    \
DSFXFlanger.lWaveform   = %ld\n    \
DSFXFlanger.fDelay      = %g\n    \
DSFXFlanger.lPhase      = %ld\n",
        szPrompt,
        lpDSFXF->fWetDryMix,
        lpDSFXF->fDepth,
        lpDSFXF->fFeedback,
        lpDSFXF->fFrequency,
        lpDSFXF->lWaveform,
        lpDSFXF->fDelay,
        lpDSFXF->lPhase );
}

/*
void _stdcall String_Send_Params( char *szMessage, char* szPrompt, LPCDSFXSend lpDSFXS )
{
    sprintf( szMessage,
        "%s\n    DSFXSend.lSendLevel     = %ld\n",
        szPrompt,
        lpDSFXS->lSendLevel );
}
*/

void _stdcall String_Echo_Params( char *szMessage, char* szPrompt, LPCDSFXEcho lpDSFXE )
{
    sprintf( szMessage,
        "%s\n    \
DSFXEcho.fWetDryMix       = %g\n    \
DSFXEcho.fLeftDelay       = %g\n    \
DSFXEcho.fRightDelay      = %g\n    \
DSFXEcho.fFeedback        = %g\n    \
DSFXEcho.lPanDelay        = %ld\n",
        szPrompt,
        lpDSFXE->fWetDryMix,
        lpDSFXE->fLeftDelay,
        lpDSFXE->fRightDelay,
        lpDSFXE->fFeedback,
        lpDSFXE->lPanDelay );
}

void _stdcall String_Distortion_Params( char *szMessage, char* szPrompt, LPCDSFXDistortion lpDSFXD )
{
    sprintf( szMessage,
        "%s\n    \
DSFXDistortion.fGain                  = %g\n    \
DSFXDistortion.fEdge                  = %g\n    \
DSFXDistortion.fPostEQCenterFrequency = %g\n    \
DSFXDistortion.fPostEQBandwidth       = %g\n    \
DSFXDistortion.fPreLowpassCutoff      = %g\n",
        szPrompt,
        lpDSFXD->fGain,
        lpDSFXD->fEdge,
        lpDSFXD->fPostEQCenterFrequency,
        lpDSFXD->fPostEQBandwidth,
        lpDSFXD->fPreLowpassCutoff );
}

void _stdcall String_Compressor_Params( char *szMessage, char* szPrompt, LPCDSFXCompressor lpDSFXCP )
{
    sprintf( szMessage,
        "%s\n    \
DSFXCompressor.fGain            = %g\n    \
DSFXCompressor.fAttack          = %g\n    \
DSFXCompressor.fRelease         = %g\n    \
DSFXCompressor.fThreshold       = %g\n    \
DSFXCompressor.fRatio           = %g\n    \
DSFXCompressor.fPredelay        = %g\n",
        szPrompt,
        lpDSFXCP->fGain,
        lpDSFXCP->fAttack,
        lpDSFXCP->fRelease,
        lpDSFXCP->fThreshold,
        lpDSFXCP->fRatio,
        lpDSFXCP->fPredelay);
}

void _stdcall String_I3DL2Reverberation_Params( char *szMessage, char* szPrompt, LPCDSFXI3DL2Reverb lpDSFX3R )
{
    sprintf( szMessage,
        "%s\n    DSFX_I3DL2Reverberation.lRoom                  = %g\n\
    DSFX_I3DL2Reverberation.lRoomHF                = %g\n\
    DSFX_I3DL2Reverberation.flRoomRolloffFactor    = %g\n\
    DSFX_I3DL2Reverberation.flDecayTime            = %g\n\
    DSFX_I3DL2Reverberation.flDecayHFRatio         = %g\n\
    DSFX_I3DL2Reverberation.lReflections           = %g\n\
    DSFX_I3DL2Reverberation.flReflectionsDelay     = %g\n\
    DSFX_I3DL2Reverberation.lReverb                = %g\n\
    DSFX_I3DL2Reverberation.flReverbDelay          = %g\n\
    DSFX_I3DL2Reverberation.flDiffusion            = %g\n\
    DSFX_I3DL2Reverberation.flDensity              = %g\n\
    DSFX_I3DL2Reverberation.flHFReference          = %g\n",
        szPrompt,
        lpDSFX3R->lRoom,
        lpDSFX3R->lRoomHF,
        lpDSFX3R->flRoomRolloffFactor,
        lpDSFX3R->flDecayTime,
        lpDSFX3R->flDecayHFRatio,
        lpDSFX3R->lReflections,
        lpDSFX3R->flReflectionsDelay,
        lpDSFX3R->lReverb,
        lpDSFX3R->flReverbDelay,
        lpDSFX3R->flDiffusion,
        lpDSFX3R->flDensity,
        lpDSFX3R->flHFReference );
}

void _stdcall String_Gargle_Params( char *szMessage, char* szPrompt, LPCDSFXGargle lpDSFXG )
{
    sprintf( szMessage,
        "%s\n    DSFXGargle.dwRateHz        = %g\n    DSFXGargle.dwWaveShape     = %g\n",
        szPrompt,
        lpDSFXG->dwRateHz,
        lpDSFXG->dwWaveShape );
}

void _stdcall String_ParamEq_Params( char *szMessage, char* szPrompt, LPCDSFXParamEq lpDSFXP )
{
    sprintf( szMessage,
        "%s\n    DSFXParamEq.fCenter        = %g\n    DSFXParamEq.fBandwidth     = %g\n    DSFXParamEq.fGain          = %g\n",
        szPrompt,
        lpDSFXP->fCenter,
        lpDSFXP->fBandwidth,
        lpDSFXP->fGain );
}

char *_stdcall String_Source_Preset( DWORD dwPreset )
{
    switch (dwPreset)
    {
    case DSFX_I3DL2_MATERIAL_PRESET_SINGLEWINDOW:
        return "DSFX_I3DL2_MATERIAL_PRESET_SINGLEWINDOW";
    case DSFX_I3DL2_MATERIAL_PRESET_DOUBLEWINDOW:
        return "DSFX_I3DL2_MATERIAL_PRESET_DOUBLEWINDOW";
    case DSFX_I3DL2_MATERIAL_PRESET_THINDOOR:
        return "DSFX_I3DL2_MATERIAL_PRESET_THINDOOR";
    case DSFX_I3DL2_MATERIAL_PRESET_THICKDOOR:
        return "DSFX_I3DL2_MATERIAL_PRESET_THICKDOOR";
    case DSFX_I3DL2_MATERIAL_PRESET_WOODWALL:
        return "DSFX_I3DL2_MATERIAL_PRESET_WOODWALL";
    case DSFX_I3DL2_MATERIAL_PRESET_BRICKWALL:
        return "DSFX_I3DL2_MATERIAL_PRESET_BRICKWALL";
    case DSFX_I3DL2_MATERIAL_PRESET_STONEWALL:
        return "DSFX_I3DL2_MATERIAL_PRESET_STONEWALL";
    case DSFX_I3DL2_MATERIAL_PRESET_CURTAIN:
        return "DSFX_I3DL2_MATERIAL_PRESET_CURTAIN";
    default:
        return "unknown";
    }
}

char * _stdcall String_Room_Preset( DWORD dwPreset )
{
    switch (dwPreset)
    {
    case DSFX_I3DL2_ENVIRONMENT_PRESET_DEFAULT:
        return "DSFX_I3DL2_ENVIRONMENT_PRESET_DEFAULT";
    case DSFX_I3DL2_ENVIRONMENT_PRESET_GENERIC:
        return "DSFX_I3DL2_ENVIRONMENT_PRESET_GENERIC";
    case DSFX_I3DL2_ENVIRONMENT_PRESET_PADDEDCELL:
        return "DSFX_I3DL2_ENVIRONMENT_PRESET_PADDEDCELL";
    case DSFX_I3DL2_ENVIRONMENT_PRESET_ROOM:
        return "DSFX_I3DL2_ENVIRONMENT_PRESET_ROOM";
    case DSFX_I3DL2_ENVIRONMENT_PRESET_BATHROOM:
        return "DSFX_I3DL2_ENVIRONMENT_PRESET_BATHROOM";
    case DSFX_I3DL2_ENVIRONMENT_PRESET_LIVINGROOM:
        return "DSFX_I3DL2_ENVIRONMENT_PRESET_LIVINGROOM";
    case DSFX_I3DL2_ENVIRONMENT_PRESET_STONEROOM:
        return "DSFX_I3DL2_ENVIRONMENT_PRESET_STONEROOM";
    case DSFX_I3DL2_ENVIRONMENT_PRESET_AUDITORIUM:
        return "DSFX_I3DL2_ENVIRONMENT_PRESET_AUDITORIUM";
    case DSFX_I3DL2_ENVIRONMENT_PRESET_CONCERTHALL:
        return "DSFX_I3DL2_ENVIRONMENT_PRESET_CONCERTHALL";
    case DSFX_I3DL2_ENVIRONMENT_PRESET_CAVE:
        return "DSFX_I3DL2_ENVIRONMENT_PRESET_CAVE";
    case DSFX_I3DL2_ENVIRONMENT_PRESET_ARENA:
        return "DSFX_I3DL2_ENVIRONMENT_PRESET_ARENA";
    case DSFX_I3DL2_ENVIRONMENT_PRESET_HANGAR:
        return "DSFX_I3DL2_ENVIRONMENT_PRESET_HANGAR";
    case DSFX_I3DL2_ENVIRONMENT_PRESET_CARPETEDHALLWAY:
        return "DSFX_I3DL2_ENVIRONMENT_PRESET_CARPETEDHALLWAY";
    case DSFX_I3DL2_ENVIRONMENT_PRESET_HALLWAY:
        return "DSFX_I3DL2_ENVIRONMENT_PRESET_HALLWAY";
    case DSFX_I3DL2_ENVIRONMENT_PRESET_STONECORRIDOR:
        return "DSFX_I3DL2_ENVIRONMENT_PRESET_STONECORRIDOR";
    case DSFX_I3DL2_ENVIRONMENT_PRESET_ALLEY:
        return "DSFX_I3DL2_ENVIRONMENT_PRESET_ALLEY";
    case DSFX_I3DL2_ENVIRONMENT_PRESET_FOREST:
        return "DSFX_I3DL2_ENVIRONMENT_PRESET_FOREST";
    case DSFX_I3DL2_ENVIRONMENT_PRESET_CITY:
        return "DSFX_I3DL2_ENVIRONMENT_PRESET_CITY";
    case DSFX_I3DL2_ENVIRONMENT_PRESET_MOUNTAINS:
        return "DSFX_I3DL2_ENVIRONMENT_PRESET_MOUNTAINS";
    case DSFX_I3DL2_ENVIRONMENT_PRESET_QUARRY:
        return "DSFX_I3DL2_ENVIRONMENT_PRESET_QUARRY";
    case DSFX_I3DL2_ENVIRONMENT_PRESET_PLAIN:
        return "DSFX_I3DL2_ENVIRONMENT_PRESET_PLAIN";
    case DSFX_I3DL2_ENVIRONMENT_PRESET_PARKINGLOT:
        return "DSFX_I3DL2_ENVIRONMENT_PRESET_PARKINGLOT";
    case DSFX_I3DL2_ENVIRONMENT_PRESET_SEWERPIPE:
        return "DSFX_I3DL2_ENVIRONMENT_PRESET_SEWERPIPE";
    case DSFX_I3DL2_ENVIRONMENT_PRESET_UNDERWATER:
        return "DSFX_I3DL2_ENVIRONMENT_PRESET_UNDERWATER";
    case DSFX_I3DL2_ENVIRONMENT_PRESET_SMALLROOM:
        return "DSFX_I3DL2_ENVIRONMENT_PRESET_SMALLROOM";
    case DSFX_I3DL2_ENVIRONMENT_PRESET_MEDIUMROOM:
        return "DSFX_I3DL2_ENVIRONMENT_PRESET_MEDIUMROOM";
    case DSFX_I3DL2_ENVIRONMENT_PRESET_LARGEROOM:
        return "DSFX_I3DL2_ENVIRONMENT_PRESET_LARGEROOM";
    case DSFX_I3DL2_ENVIRONMENT_PRESET_MEDIUMHALL:
        return "DSFX_I3DL2_ENVIRONMENT_PRESET_MEDIUMHALL";
    case DSFX_I3DL2_ENVIRONMENT_PRESET_LARGEHALL:
        return "DSFX_I3DL2_ENVIRONMENT_PRESET_LARGEHALL";
    case DSFX_I3DL2_ENVIRONMENT_PRESET_PLATE:
        return "DSFX_I3DL2_ENVIRONMENT_PRESET_PLATE";
    default:
        return "unknown";
    }
}


/*
BOOL _stdcall Equal_Send_Params( LPCDSFXSend lpDSFXS1, LPCDSFXSend lpDSFXS2 )
{
    ASSERT(lpDSFXS1);
    ASSERT(lpDSFXS2);
    return (lpDSFXS1->lSendLevel == lpDSFXS2->lSendLevel);
}
*/


BOOL _stdcall Equal_Chorus_Params( LPCDSFXChorus lpDSFXC1, LPCDSFXChorus lpDSFXC2 )
{
    ASSERT(lpDSFXC1);
    ASSERT(lpDSFXC2);

    return ( (fabs(lpDSFXC1->fWetDryMix - lpDSFXC2->fWetDryMix) < FLT_EPSILON) &&
             (fabs(lpDSFXC1->fDepth - lpDSFXC2->fDepth) < FLT_EPSILON) &&
             (fabs(lpDSFXC1->fFeedback - lpDSFXC2->fFeedback) < FLT_EPSILON) &&
             (fabs(lpDSFXC1->fFrequency -lpDSFXC2->fFrequency) < FLT_EPSILON) &&
             (lpDSFXC1->lWaveform == lpDSFXC2->lWaveform) &&
             (fabs(lpDSFXC1->fDelay - lpDSFXC2->fDelay) < FLT_EPSILON) &&
             (lpDSFXC1->lPhase == lpDSFXC2->lPhase) );
}

BOOL _stdcall Equal_Flanger_Params( LPCDSFXFlanger lpDSFXF1, LPCDSFXFlanger lpDSFXF2 )
{
    ASSERT(lpDSFXF1);
    ASSERT(lpDSFXF2);

    return ( (fabs(lpDSFXF1->fWetDryMix - lpDSFXF2->fWetDryMix) < FLT_EPSILON) &&
             (fabs(lpDSFXF1->fDepth - lpDSFXF2->fDepth) < FLT_EPSILON) &&
             (fabs(lpDSFXF1->fFeedback - lpDSFXF2->fFeedback) < FLT_EPSILON) &&
             (fabs(lpDSFXF1->fFrequency - lpDSFXF2->fFrequency) < FLT_EPSILON) &&
             (lpDSFXF1->lWaveform == lpDSFXF2->lWaveform) &&
             (fabs(lpDSFXF1->fDelay - lpDSFXF2->fDelay) < FLT_EPSILON) &&
             (lpDSFXF1->lPhase == lpDSFXF2->lPhase) );
}

BOOL _stdcall Equal_Echo_Params( LPCDSFXEcho lpDSFXE1, LPCDSFXEcho lpDSFXE2 )
{
    ASSERT(lpDSFXE1);
    ASSERT(lpDSFXE2);

    return ( (fabs(lpDSFXE1->fWetDryMix - lpDSFXE2->fWetDryMix) < FLT_EPSILON) &&
             (fabs(lpDSFXE1->fLeftDelay - lpDSFXE2->fLeftDelay) < FLT_EPSILON) &&
             (fabs(lpDSFXE1->fRightDelay - lpDSFXE2->fRightDelay) < FLT_EPSILON) &&
             (fabs(lpDSFXE1->fFeedback - lpDSFXE2->fFeedback) < FLT_EPSILON) &&
             (lpDSFXE1->lPanDelay == lpDSFXE2->lPanDelay) );
}

BOOL _stdcall Equal_Distortion_Params( LPCDSFXDistortion lpDSFXD1, LPCDSFXDistortion lpDSFXD2 )
{
    ASSERT(lpDSFXD1);
    ASSERT(lpDSFXD2);

    return ( (fabs(lpDSFXD1->fGain - lpDSFXD2->fGain) < FLT_EPSILON) &&
             (fabs(lpDSFXD1->fEdge - lpDSFXD2->fEdge) < FLT_EPSILON) &&
             (fabs(lpDSFXD1->fPostEQCenterFrequency - lpDSFXD2->fPostEQCenterFrequency) < FLT_EPSILON) &&
             (fabs(lpDSFXD1->fPostEQBandwidth - lpDSFXD2->fPostEQBandwidth) < FLT_EPSILON) &&
             (fabs(lpDSFXD1->fPreLowpassCutoff - lpDSFXD2->fPreLowpassCutoff) < FLT_EPSILON) );
}

BOOL _stdcall Equal_Compressor_Params( LPCDSFXCompressor lpDSFXCP1, LPCDSFXCompressor lpDSFXCP2 )
{
    ASSERT(lpDSFXCP1);
    ASSERT(lpDSFXCP2);

    return ( (fabs(lpDSFXCP1->fRatio - lpDSFXCP2->fRatio) < FLT_EPSILON) &&
             (fabs(lpDSFXCP1->fGain - lpDSFXCP2->fGain) < FLT_EPSILON) &&
             (fabs(lpDSFXCP1->fAttack - lpDSFXCP2->fAttack) < FLT_EPSILON) &&
             (fabs(lpDSFXCP1->fRelease - lpDSFXCP2->fRelease) < FLT_EPSILON) &&
             (fabs(lpDSFXCP1->fThreshold - lpDSFXCP2->fThreshold) < FLT_EPSILON) &&
             (fabs(lpDSFXCP1->fPredelay - lpDSFXCP2->fPredelay) < FLT_EPSILON));
}

BOOL _stdcall Equal_Gargle_Params( LPCDSFXGargle lpDSFXG1, LPCDSFXGargle lpDSFXG2 )
{
    ASSERT(lpDSFXG1);
    ASSERT(lpDSFXG2);

    return ( (lpDSFXG1->dwRateHz        == lpDSFXG2->dwRateHz) &&
             (lpDSFXG1->dwWaveShape     == lpDSFXG2->dwWaveShape) );
}

BOOL _stdcall Equal_ParamEq_Params( LPCDSFXParamEq lpDSFXP1, LPCDSFXParamEq lpDSFXP2 )
{
    ASSERT(lpDSFXP1);
    ASSERT(lpDSFXP2);

    return ( (fabs(lpDSFXP1->fCenter - lpDSFXP2->fCenter) < FLT_EPSILON) &&
             (fabs(lpDSFXP1->fBandwidth - lpDSFXP2->fBandwidth) < FLT_EPSILON) &&
             (fabs(lpDSFXP1->fGain - lpDSFXP2->fGain) < FLT_EPSILON) );
}


BOOL _stdcall Equal_I3DL2Reverb_Params( LPCDSFXI3DL2Reverb lpDSFX3R1, LPCDSFXI3DL2Reverb lpDSFX3R2 )
{
    ASSERT(lpDSFX3R1);
    ASSERT(lpDSFX3R2);

    return ( (lpDSFX3R1->lRoom                == lpDSFX3R2->lRoom) &&
             (lpDSFX3R1->lRoomHF              == lpDSFX3R2->lRoomHF) &&
             (fabs(lpDSFX3R1->flRoomRolloffFactor - lpDSFX3R2->flRoomRolloffFactor) < FLT_EPSILON) &&
             (fabs(lpDSFX3R1->flDecayTime - lpDSFX3R2->flDecayTime) < FLT_EPSILON) &&
             (fabs(lpDSFX3R1->flDecayHFRatio - lpDSFX3R2->flDecayHFRatio) < FLT_EPSILON) &&
             (lpDSFX3R1->lReflections         == lpDSFX3R2->lReflections) &&
             (fabs(lpDSFX3R1->flReflectionsDelay - lpDSFX3R2->flReflectionsDelay) < FLT_EPSILON) &&
             (lpDSFX3R1->lReverb              == lpDSFX3R2->lReverb) &&
             (fabs(lpDSFX3R1->flReverbDelay - lpDSFX3R2->flReverbDelay) < FLT_EPSILON) &&
             (fabs(lpDSFX3R1->flDiffusion - lpDSFX3R2->flDiffusion) < FLT_EPSILON) &&
             (fabs(lpDSFX3R1->flDensity - lpDSFX3R2->flDensity) < FLT_EPSILON) &&
             (fabs(lpDSFX3R1->flHFReference - lpDSFX3R2->flHFReference) < FLT_EPSILON) );
}

BOOL _stdcall Equal_WavesReverb_Params( LPCDSFXWavesReverb lpDSFX3W1, LPCDSFXWavesReverb lpDSFX3W2 )
{
    ASSERT(lpDSFX3W1);
    ASSERT(lpDSFX3W2);
    return ( (fabs(lpDSFX3W1->fInGain - lpDSFX3W2->fInGain ) < FLT_EPSILON) &&
             (fabs(lpDSFX3W1->fReverbMix - lpDSFX3W2->fReverbMix) < FLT_EPSILON) &&
             (fabs(lpDSFX3W1->fReverbTime - lpDSFX3W2->fReverbTime) < FLT_EPSILON) &&
             (fabs(lpDSFX3W1->fHighFreqRTRatio - lpDSFX3W2->fHighFreqRTRatio) < FLT_EPSILON));
}


static char g_szErrorEqual[]     = "unexpectedly equal";
static char g_szErrorUnequal []  = "unexpectedly unequal";

/********************************************************************************
HELPER FUNCTION

HISTORY:
    Created 04/14/00      danhaff

DESCRIPTION:
    Prints any unexpectency between two different DS3DBUFFER structs.

    #define EXPECT_EQUALITY   0x2345
    #define EXPECT_INEQUALITY 0x3456

    We need weird numbers to prevent people from passing TRUE and/or FALSE here.
    those values detract from readability.

    The equality of each field of both structs must match the dwExpected value
    passed in.
********************************************************************************/
HRESULT _stdcall Compare_DS3DBUFFER(int iLogLevel, DS3DBUFFER *p1, DS3DBUFFER *p2, DWORD dwExpected)
{
    HRESULT hr = S_OK;
    char *szError;
    bool bExpectedEquality;

    //Depending on what we expect, we'll print different output.
    if (EXPECT_EQUALITY == dwExpected)
    {
        szError = g_szErrorUnequal;
        bExpectedEquality = true;
    }
    else if (EXPECT_INEQUALITY == dwExpected)
    {
        szError = g_szErrorEqual;
        bExpectedEquality = false;
    }
    else
    {
        fnsLog(iLogLevel, "TEST_APP error: someone passed Compare_DS3DBUFFER dwExpected = %d", dwExpected);
        return E_FAIL;
    }

    //check vPosition
    if ( bool(p1->vPosition == p2->vPosition) != bExpectedEquality)
    {
        fnsLog(iLogLevel, "The 3D Buffers' positions are %s:", szError);
        Log_D3DVector(iLogLevel, &p1->vPosition);
        Log_D3DVector(iLogLevel, &p2->vPosition);
        hr = E_FAIL;
    }

    //check vVelocity
    if (bool( p1->vVelocity == p2->vVelocity) != bExpectedEquality)
    {
        fnsLog(iLogLevel, "The 3D Buffers' velocities are %s:", szError);
        Log_D3DVector(iLogLevel, &p1->vVelocity);
        Log_D3DVector(iLogLevel, &p2->vVelocity);
        hr = E_FAIL;
    }

    //check dwInsideConeAngle
    if (bool(p1->dwInsideConeAngle == p2->dwInsideConeAngle) != bExpectedEquality)
    {
        fnsLog(iLogLevel, "The 3D Buffer's dwInsideConeAngles are %s: %d and %d", szError, p1->dwInsideConeAngle, p2->dwInsideConeAngle);
        hr = E_FAIL;
    }

    //check dwOutsideConeAngle
    if (bool(p1->dwOutsideConeAngle == p2->dwOutsideConeAngle) != bExpectedEquality)
    {
        fnsLog(iLogLevel, "The 3D Buffer's dwOutsideConeAngles are %s: %d and %d", szError, p1->dwOutsideConeAngle, p2->dwOutsideConeAngle);
        hr = E_FAIL;
    }

    //check vConeOrientation
    if (bool( p1->vConeOrientation == p2->vConeOrientation) != bExpectedEquality)
    {
        fnsLog(iLogLevel, "The 3D Buffers' vConeOrientations are %s:", szError);
        Log_D3DVector(iLogLevel, &p1->vConeOrientation);
        Log_D3DVector(iLogLevel, &p2->vConeOrientation);
        hr = E_FAIL;
    }

    //check lConeOutsideVolume
    if (bool(p1->lConeOutsideVolume == p2->lConeOutsideVolume) != bExpectedEquality)
    {
        fnsLog(iLogLevel, "The 3D Buffer's lConeOutsideVolumes are %s: %d and %d", szError, p1->lConeOutsideVolume, p2->lConeOutsideVolume);
        hr = E_FAIL;
    }

    //check flMinDistance
    if ( IsfEqual(p1->flMinDistance, p2->flMinDistance) != bExpectedEquality)
    {
        fnsLog(iLogLevel, "The 3D Buffer's flMinDistances are %s:", szError);
        tstLof(iLogLevel, "  %g" , p1->flMinDistance);
        tstLof(iLogLevel, "  %g" , p2->flMinDistance);
        hr = E_FAIL;
    }

    //check flMaxDistance
    if (IsfEqual(p1->flMaxDistance, p2->flMaxDistance) != bExpectedEquality)
    {
        fnsLog(iLogLevel, "The 3D Buffer's flMaxDistances are %s:", szError);
        tstLof(iLogLevel, "  %g" , p1->flMaxDistance);
        tstLof(iLogLevel, "  %g" , p2->flMaxDistance);
        hr = E_FAIL;
    }

    //check dwMode
    if (bool(p1->dwMode == p2->dwMode) != bExpectedEquality)
    {
        fnsLog(iLogLevel, "The 3D Buffers' dwModes are %s:", szError);
        Log_DS3DBMode(iLogLevel, p1->dwMode);
        Log_DS3DBMode(iLogLevel, p2->dwMode);
        hr = E_FAIL;
    }

    return hr;
};




/********************************************************************************
HELPER FUNCTION

HISTORY:
    Created 04/14/00      danhaff

DESCRIPTION:
    Prints any unexpectency between two different DS3DLISTENER structs.

    #define EXPECT_EQUALITY   0x2345
    #define EXPECT_INEQUALITY 0x3456
    We need weird numbers to prevent people from passing TRUE and/or FALSE here.
    those values detract from readability.

    The equality of each field of both structs must match the dwExpected value
    passed in.
********************************************************************************/
HRESULT _stdcall Compare_DS3DLISTENER(int iLogLevel, DS3DLISTENER *p1, DS3DLISTENER *p2, DWORD dwExpected)
{
    HRESULT hr = S_OK;
    char *szError;
    bool bExpectedEquality;

    //Depending on what we expect, we'll print different output.
    if (EXPECT_EQUALITY == dwExpected)
    {
        szError = g_szErrorUnequal;
        bExpectedEquality = true;
    }
    else if (EXPECT_INEQUALITY == dwExpected)
    {
        szError = g_szErrorEqual;
        bExpectedEquality = false;
    }
    else
    {
        fnsLog(iLogLevel, "TEST_APP error: someone passed Compare_DS3DLISTENER dwExpected = %d", dwExpected);
        return E_FAIL;
    }

    //check vPosition
    if ( bool(p1->vPosition == p2->vPosition) != bExpectedEquality)
    {
        fnsLog(iLogLevel, "The 3D Listeners' positions are %s:", szError);
        Log_D3DVector(iLogLevel, &p1->vPosition);
        Log_D3DVector(iLogLevel, &p2->vPosition);
        hr = E_FAIL;
    }

    //check vVelocity
    if (bool( p1->vVelocity == p2->vVelocity) != bExpectedEquality)
    {
        fnsLog(iLogLevel, "The 3D Listeners' velocities are %s:", szError);
        Log_D3DVector(iLogLevel, &p1->vVelocity);
        Log_D3DVector(iLogLevel, &p2->vVelocity);
        hr = E_FAIL;
    }


    //check vOrientFront
    if (bool( p1->vOrientFront == p2->vOrientFront) != bExpectedEquality)
    {
        fnsLog(iLogLevel, "The 3D Listeners' vOrientFront are %s:", szError);
        Log_D3DVector(iLogLevel, &p1->vOrientFront);
        Log_D3DVector(iLogLevel, &p2->vOrientFront);
        hr = E_FAIL;
    }

    //check vOrientTop
    if (bool( p1->vOrientTop == p2->vOrientTop) != bExpectedEquality)
    {
        fnsLog(iLogLevel, "The 3D Listeners' vOrientTop are %s:", szError);
        Log_D3DVector(iLogLevel, &p1->vOrientTop);
        Log_D3DVector(iLogLevel, &p2->vOrientTop);
        hr = E_FAIL;
    }

    //check flDistanceFactor
    if ( IsfEqual(p1->flDistanceFactor, p2->flDistanceFactor) != bExpectedEquality)
    {
        fnsLog(iLogLevel, "The 3D Listener's flDistanceFactors are %s:", szError);
        tstLof(iLogLevel, "  %g" , p1->flDistanceFactor);
        tstLof(iLogLevel, "  %g" , p2->flDistanceFactor);
        hr = E_FAIL;
    }

    //check flRolloffFactor
    if ( IsfEqual(p1->flRolloffFactor, p2->flRolloffFactor) != bExpectedEquality)
    {
        fnsLog(iLogLevel, "The 3D Listener's flRolloffFactors are %s:", szError);
        tstLof(iLogLevel, "  %g" , p1->flRolloffFactor);
        tstLof(iLogLevel, "  %g" , p2->flRolloffFactor);
        hr = E_FAIL;
    }

    //check flDopplerFactor
    if ( IsfEqual(p1->flDopplerFactor, p2->flDopplerFactor) != bExpectedEquality)
    {
        fnsLog(iLogLevel, "The 3D Listener's flDopplerFactors are %s:", szError);
        tstLof(iLogLevel, "  %g" , p1->flDopplerFactor);
        tstLof(iLogLevel, "  %g" , p2->flDopplerFactor);
        hr = E_FAIL;
    }


return hr;
};


void _stdcall Copy_Chorus_Params( LPDSFXChorus lpDSFXC1, LPCDSFXChorus lpDSFXC2 )
{
    ASSERT(lpDSFXC1);
    ASSERT(lpDSFXC2);

    lpDSFXC1->fWetDryMix    = lpDSFXC2->fWetDryMix;
    lpDSFXC1->fDepth        = lpDSFXC2->fDepth;
    lpDSFXC1->fFeedback     = lpDSFXC2->fFeedback;
    lpDSFXC1->fFrequency    = lpDSFXC2->fFrequency;
    lpDSFXC1->lWaveform     = lpDSFXC2->lWaveform;
    lpDSFXC1->fDelay        = lpDSFXC2->fDelay;
    lpDSFXC1->lPhase        = lpDSFXC2->lPhase;
}

void _stdcall Copy_Flanger_Params( LPDSFXFlanger lpDSFXF1, LPCDSFXFlanger lpDSFXF2 )
{
    ASSERT(lpDSFXF1);
    ASSERT(lpDSFXF2);

    lpDSFXF1->fWetDryMix    = lpDSFXF2->fWetDryMix;
    lpDSFXF1->fDepth        = lpDSFXF2->fDepth;
    lpDSFXF1->fFeedback     = lpDSFXF2->fFeedback;
    lpDSFXF1->fFrequency    = lpDSFXF2->fFrequency;
    lpDSFXF1->lWaveform     = lpDSFXF2->lWaveform;
    lpDSFXF1->fDelay        = lpDSFXF2->fDelay;
    lpDSFXF1->lPhase        = lpDSFXF2->lPhase;
}

void _stdcall Copy_Echo_Params( LPDSFXEcho lpDSFXE1, LPCDSFXEcho lpDSFXE2 )
{
    ASSERT(lpDSFXE1);
    ASSERT(lpDSFXE2);

    lpDSFXE1->fWetDryMix    = lpDSFXE2->fWetDryMix;
    lpDSFXE1->fLeftDelay    = lpDSFXE2->fLeftDelay;
    lpDSFXE1->fRightDelay   = lpDSFXE2->fRightDelay;
    lpDSFXE1->fFeedback     = lpDSFXE2->fFeedback;
    lpDSFXE1->lPanDelay     = lpDSFXE2->lPanDelay;
}

void _stdcall Copy_Distortion_Params( LPDSFXDistortion lpDSFXD1, LPCDSFXDistortion lpDSFXD2 )
{
    ASSERT(lpDSFXD1);
    ASSERT(lpDSFXD2);

    lpDSFXD1->fGain                  = lpDSFXD2->fGain;
    lpDSFXD1->fEdge                  = lpDSFXD2->fEdge;
    lpDSFXD1->fPostEQCenterFrequency = lpDSFXD2->fPostEQCenterFrequency;
    lpDSFXD1->fPostEQBandwidth       = lpDSFXD2->fPostEQBandwidth;
    lpDSFXD1->fPreLowpassCutoff      = lpDSFXD2->fPreLowpassCutoff;
}

void _stdcall Copy_Compressor_Params( LPDSFXCompressor lpDSFXCP1, LPCDSFXCompressor lpDSFXCP2 )
{
    ASSERT(lpDSFXCP1);
    ASSERT(lpDSFXCP2);

    lpDSFXCP1->fRatio           = lpDSFXCP2->fRatio;
    lpDSFXCP1->fGain            = lpDSFXCP2->fGain;
    lpDSFXCP1->fAttack          = lpDSFXCP2->fAttack;
    lpDSFXCP1->fRelease         = lpDSFXCP2->fRelease;
    lpDSFXCP1->fThreshold       = lpDSFXCP2->fThreshold;
    lpDSFXCP1->fPredelay        = lpDSFXCP2->fPredelay;
}

void _stdcall Copy_Gargle_Params( LPDSFXGargle lpDSFXG1, LPCDSFXGargle lpDSFXG2 )
{
    ASSERT(lpDSFXG1);
    ASSERT(lpDSFXG2);

    lpDSFXG1->dwRateHz        = lpDSFXG2->dwRateHz;
    lpDSFXG1->dwWaveShape     = lpDSFXG2->dwWaveShape;
}

void _stdcall Copy_ParamEq_Params( LPDSFXParamEq lpDSFXP1, LPCDSFXParamEq lpDSFXP2 )
{
    ASSERT(lpDSFXP1);
    ASSERT(lpDSFXP2);

    lpDSFXP1->fCenter           = lpDSFXP2->fCenter;
    lpDSFXP1->fBandwidth        = lpDSFXP2->fBandwidth;
    lpDSFXP1->fGain             = lpDSFXP2->fGain;
}

void _stdcall Copy_I3DL2Reverb_Params( LPDSFXI3DL2Reverb lpDSFX3R1, LPCDSFXI3DL2Reverb lpDSFX3R2 )
{
    ASSERT(lpDSFX3R1);
    ASSERT(lpDSFX3R2);

    lpDSFX3R1->lRoom                    = lpDSFX3R2->lRoom;
    lpDSFX3R1->lRoomHF                  = lpDSFX3R2->lRoomHF;
    lpDSFX3R1->flRoomRolloffFactor      = lpDSFX3R2->flRoomRolloffFactor;
    lpDSFX3R1->flDecayTime              = lpDSFX3R2->flDecayTime;
    lpDSFX3R1->flDecayHFRatio           = lpDSFX3R2->flDecayHFRatio;
    lpDSFX3R1->lReflections             = lpDSFX3R2->lReflections;
    lpDSFX3R1->flReflectionsDelay       = lpDSFX3R2->flReflectionsDelay;
    lpDSFX3R1->lReverb                  = lpDSFX3R2->lReverb;
    lpDSFX3R1->flReverbDelay            = lpDSFX3R2->flReverbDelay;
    lpDSFX3R1->flDiffusion              = lpDSFX3R2->flDiffusion;
    lpDSFX3R1->flDensity                = lpDSFX3R2->flDensity;
    lpDSFX3R1->flHFReference            = lpDSFX3R2->flHFReference;
}

float _stdcall GetRandomFloatVal(float fltMax, float fltMin)
{
    return ( ((float)GetRandomDWORD((DWORD)((fltMax-fltMin)*10000)) + fltMin*10000) / 10000.0f );
}

DWORD _stdcall GetRandomDwordVal(DWORD dwMax, DWORD dwMin)
{
    return GetRandomDWORD(dwMax-dwMin+1) + dwMin;
}

void _stdcall GetRandomEchoParams( LPDSFXEcho lpDSFXE )
{
    ASSERT(lpDSFXE);

    lpDSFXE->fWetDryMix     = GetRandomFloatVal(DSFX_WETDRYMIX_MAX, DSFX_WETDRYMIX_MIN);
    lpDSFXE->fLeftDelay     = GetRandomFloatVal(DSFX_ECHO_LEFTDELAY_MAX, DSFX_ECHO_LEFTDELAY_MIN);
    lpDSFXE->fRightDelay    = GetRandomFloatVal(DSFX_ECHO_RIGHTDELAY_MAX, DSFX_ECHO_RIGHTDELAY_MIN);
    lpDSFXE->fFeedback      = GetRandomFloatVal(DSFX_ECHO_FEEDBACK_MAX, DSFX_ECHO_FEEDBACK_MIN);
    lpDSFXE->lPanDelay      = GetRandomDWORD(  2 );
}

void _stdcall GetRandomChorusParams( LPDSFXChorus lpDSFXC )
{
    ASSERT(lpDSFXC);

    lpDSFXC->fWetDryMix     = GetRandomFloatVal(DSFX_WETDRYMIX_MAX, DSFX_WETDRYMIX_MIN);
    lpDSFXC->fDepth         = GetRandomFloatVal(DSFX_CHORUS_DEPTH_MAX, DSFX_CHORUS_DEPTH_MIN);
    lpDSFXC->fFeedback      = GetRandomFloatVal(DSFX_CHORUS_FEEDBACK_MAX, DSFX_CHORUS_FEEDBACK_MIN);
    lpDSFXC->fFrequency     = GetRandomFloatVal(DSFX_CHORUS_FREQUENCY_MAX, DSFX_CHORUS_FREQUENCY_MIN);
    lpDSFXC->lWaveform      = GetRandomDWORD(  2 );
    lpDSFXC->fDelay         = GetRandomFloatVal(DSFX_CHORUS_DELAY_MAX, DSFX_CHORUS_DELAY_MIN);
    lpDSFXC->lPhase         = ( (int)GetRandomDWORD(  5 ) - 2 ) * 90;
}

void _stdcall GetRandomFlangerParams( LPDSFXFlanger lpDSFXF )
{
    ASSERT(lpDSFXF);

    lpDSFXF->fWetDryMix     = GetRandomFloatVal(DSFX_WETDRYMIX_MAX, DSFX_WETDRYMIX_MIN);
    lpDSFXF->fDepth         = GetRandomFloatVal(DSFX_FLANGER_DEPTH_MAX, DSFX_FLANGER_DEPTH_MIN);
    lpDSFXF->fFeedback      = GetRandomFloatVal(DSFX_FLANGER_FEEDBACK_MAX, DSFX_FLANGER_FEEDBACK_MIN);
    lpDSFXF->fFrequency     = GetRandomFloatVal(DSFX_FLANGER_FREQUENCY_MAX, DSFX_FLANGER_FREQUENCY_MIN);
    lpDSFXF->lWaveform      = GetRandomDWORD(  2 );
    lpDSFXF->fDelay         = GetRandomFloatVal(DSFX_FLANGER_DELAY_MAX, DSFX_FLANGER_DELAY_MIN);
    lpDSFXF->lPhase         = ( (int)GetRandomDWORD(  5 ) - 2 ) * 90;
}

void _stdcall GetRandomDistortionParams( LPDSFXDistortion lpDSFXD )
{
    ASSERT(lpDSFXD);

    lpDSFXD->fGain                     = GetRandomFloatVal(DSFX_DISTORTION_GAIN_MAX, DSFX_DISTORTION_GAIN_MIN);
    lpDSFXD->fEdge                     = GetRandomFloatVal(DSFX_DISTORTION_EDGE_MAX, DSFX_DISTORTION_EDGE_MIN);
    lpDSFXD->fPostEQCenterFrequency    = GetRandomFloatVal(DSFX_DISTORTION_POSTEQCENTERFREQUENCY_MAX, DSFX_DISTORTION_POSTEQCENTERFREQUENCY_MIN);
    lpDSFXD->fPostEQBandwidth          = GetRandomFloatVal(DSFX_DISTORTION_POSTEQBANDWIDTH_MAX, DSFX_DISTORTION_POSTEQBANDWIDTH_MIN);
    lpDSFXD->fPreLowpassCutoff         = GetRandomFloatVal(DSFX_DISTORTION_PRELOWPASSCUTOFF_MAX, DSFX_DISTORTION_PRELOWPASSCUTOFF_MIN);
}

void _stdcall GetRandomCompressorParams( LPDSFXCompressor lpDSFXCP )
{
    ASSERT(lpDSFXCP);

    lpDSFXCP->fRatio            = GetRandomFloatVal(DSFX_COMPRESSOR_RATIO_MAX, DSFX_COMPRESSOR_RATIO_MIN);
    lpDSFXCP->fGain             = GetRandomFloatVal(DSFX_COMPRESSOR_GAIN_MAX, DSFX_COMPRESSOR_GAIN_MIN);
    lpDSFXCP->fAttack           = GetRandomFloatVal(DSFX_COMPRESSOR_ATTACK_MAX, DSFX_COMPRESSOR_ATTACK_MIN);
    lpDSFXCP->fRelease          = GetRandomFloatVal(DSFX_COMPRESSOR_RELEASE_MAX, DSFX_COMPRESSOR_RELEASE_MIN);
    lpDSFXCP->fThreshold        = GetRandomFloatVal(DSFX_COMPRESSOR_THRESHOLD_MAX, DSFX_COMPRESSOR_THRESHOLD_MIN);
    lpDSFXCP->fPredelay         = GetRandomFloatVal(DSFX_COMPRESSOR_PREDELAY_MAX, DSFX_COMPRESSOR_PREDELAY_MIN);
}

void _stdcall GetRandomGargleParams( LPDSFXGargle lpDSFXG )
{
    ASSERT(lpDSFXG);

    lpDSFXG->dwRateHz       =
        GetRandomDWORD(  DSFX_GARGLE_RATEHZ_MAX-DSFX_GARGLE_RATEHZ_MIN+1 ) + DSFX_COMPRESSOR_RATIO_MIN;
    lpDSFXG->dwWaveShape    =
        GetRandomDWORD(  2 ) ? DSFXGARGLE_WAVE_TRIANGLE : DSFXGARGLE_WAVE_SQUARE;
}

void _stdcall GetRandomParamEqParams( LPDSFXParamEq lpDSFXP )
{
    ASSERT(lpDSFXP);

    lpDSFXP->fCenter        =
        (float)(GetRandomDWORD(  (DWORD) (DSFX_PARAMEQ_CENTER_MAX-DSFX_PARAMEQ_CENTER_MIN) ) + DSFX_PARAMEQ_CENTER_MIN);
    lpDSFXP->fBandwidth     =
        (float)(GetRandomDWORD(  (DWORD) (DSFX_PARAMEQ_BANDWIDTH_MAX-DSFX_PARAMEQ_BANDWIDTH_MIN) ) + DSFX_PARAMEQ_BANDWIDTH_MIN);
    lpDSFXP->fGain          =
        (float)(GetRandomDWORD(  (DWORD) (DSFX_PARAMEQ_GAIN_MAX-DSFX_PARAMEQ_GAIN_MIN) ) + DSFX_PARAMEQ_GAIN_MIN);
}

void _stdcall GetRandomI3DL2ReverbParams( LPDSFXI3DL2Reverb lpDSFX3R )
{
    ASSERT(lpDSFX3R);

    lpDSFX3R->lRoom                     =
        GetRandomDWORD(  DSFX_I3DL2REVERB_ROOM_MAX-DSFX_I3DL2REVERB_ROOM_MIN+1 ) + DSFX_I3DL2REVERB_ROOM_MIN;
    lpDSFX3R->lRoomHF                   =
        GetRandomDWORD(  DSFX_I3DL2REVERB_ROOMHF_MAX-DSFX_I3DL2REVERB_ROOMHF_MIN+1 ) + DSFX_I3DL2REVERB_ROOMHF_MIN;
    lpDSFX3R->flRoomRolloffFactor       =
        (float)(GetRandomDWORD(  (DWORD)((DSFX_I3DL2REVERB_ROOMROLLOFFFACTOR_MAX-DSFX_I3DL2REVERB_ROOMROLLOFFFACTOR_MIN)*10000) ) + DSFX_I3DL2REVERB_ROOMROLLOFFFACTOR_MIN*10000) / 10000.0f;
    lpDSFX3R->flDecayTime               =
        (float)(GetRandomDWORD(  (DWORD)((DSFX_I3DL2REVERB_DECAYTIME_MAX-DSFX_I3DL2REVERB_DECAYTIME_MIN)*10000) ) + DSFX_I3DL2REVERB_DECAYTIME_MIN*10000) / 10000.0f;
    lpDSFX3R->flDecayHFRatio            =
        (float)(GetRandomDWORD(  (DWORD)((DSFX_I3DL2REVERB_DECAYHFRATIO_MAX-DSFX_I3DL2REVERB_DECAYHFRATIO_MIN)*10000) ) + DSFX_I3DL2REVERB_DECAYHFRATIO_MIN*10000) / 10000.0f;
    lpDSFX3R->lReflections              =
        GetRandomDWORD(  DSFX_I3DL2REVERB_REFLECTIONS_MAX-DSFX_I3DL2REVERB_REFLECTIONS_MIN+1 ) + DSFX_I3DL2REVERB_REFLECTIONS_MIN;
    lpDSFX3R->flReflectionsDelay        =
        (float)(GetRandomDWORD(  (DWORD)((DSFX_I3DL2REVERB_REFLECTIONSDELAY_MAX-DSFX_I3DL2REVERB_REFLECTIONSDELAY_MIN)*10000) ) + DSFX_I3DL2REVERB_REFLECTIONSDELAY_MIN*10000) / 10000.0f;
    lpDSFX3R->lReverb                   =
        GetRandomDWORD(  DSFX_I3DL2REVERB_REVERB_MAX-DSFX_I3DL2REVERB_REVERB_MIN+1 ) + DSFX_I3DL2REVERB_REVERB_MIN;
    lpDSFX3R->flReverbDelay             =
        (float)(GetRandomDWORD(  (DWORD)((DSFX_I3DL2REVERB_REVERBDELAY_MAX-DSFX_I3DL2REVERB_REVERBDELAY_MIN)*10000) ) + DSFX_I3DL2REVERB_REVERBDELAY_MIN*10000) / 10000.0f;
    lpDSFX3R->flDiffusion               =
        (float)(GetRandomDWORD(  (DWORD)((DSFX_I3DL2REVERB_DIFFUSION_MAX-DSFX_I3DL2REVERB_DIFFUSION_MIN)*10000) ) + DSFX_I3DL2REVERB_DIFFUSION_MIN*10000) / 10000.0f;
    lpDSFX3R->flDensity                 =
        (float)(GetRandomDWORD(  (DWORD)((DSFX_I3DL2REVERB_DENSITY_MAX-DSFX_I3DL2REVERB_DENSITY_MIN)*10000) ) + DSFX_I3DL2REVERB_DENSITY_MIN*10000) / 10000.0f;
    lpDSFX3R->flHFReference             =
        (float)(GetRandomDWORD(  (DWORD)((DSFX_I3DL2REVERB_HFREFERENCE_MAX-DSFX_I3DL2REVERB_HFREFERENCE_MIN)*10000) ) + DSFX_I3DL2REVERB_HFREFERENCE_MIN*10000) / 10000.0f;
}