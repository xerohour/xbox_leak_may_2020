 /*****************************************************************************
 *                        Voxware Proprietary Material                       *
 *                        Copyright 1996, Voxware Inc.                       *
 *                        All Rights Reserved                                *
 *                                                                           *
 *                        DISTRIBUTION PROHIBITED without                    *
 *                        written authorization from Voxware.                *
 *****************************************************************************/

/*****************************************************************************
 * FILE      : HarmSynFloat.h
 *
 * CONTAINS  : Synthesis of input harmonics
 *
 * CREATION  : 12/7/96
 *
 * AUTHOR    : Bob Dunn (derived from Rob Zopf's code for fixed point FHT) 
 *
 * $Header:   /export/phobos/pvcs/r_and_d/archives/realtime/decode/HarmSynF.h_v   1.1   02 Mar 1998 10:30:58   weiwang  $
 *
 *****************************************************************************/

#ifndef HARMSYNFLOAT_H
#define HARMSYNFLOAT_H
void HarmSynFHTFloat ( STACK_R
                       float    *pfAmps,
                       int    *piPhases,
                       float     fPitch,
                       float     fVPitch,
                       float     fVPitch_1,
                       float    *fNormPhase,
                       float    *fVoicePhase,
                       float    *fResampRate,
                       short     nHarm,
                       float    *fPitchCycle,
                       float    *pfOutBuf,
                       short     frameLEN,
                       short     olLEN
                );


#endif /* HARMSYNFLOAT_H */

