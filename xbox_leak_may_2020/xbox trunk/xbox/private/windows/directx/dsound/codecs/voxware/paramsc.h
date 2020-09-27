/*******************************************************************************
*                        Voxware Proprietary Material                          *
*                        Copyright 1996, Voxware, Inc                          *
*                            All Rights Reserved                               *
*                                                                              *
*                       DISTRIBUTION PROHIBITED without                        *
*                      written authorization from Voxware                      *
******************************************************************************/

/******************************************************************************
* Filename:       param.h
*                                                                              
* Purpose:                                                                    
*                                                                              
* Author/Date:    Ilan Berci 09/96
********************************************************************************
* $Header:   /export/phobos/pvcs/r_and_d/archives/scaleable/utils/paramSC.h_v   1.5   13 Apr 1998 16:17:58   weiwang  $                                                                     
******************************************************************************/
#ifndef PARAM_H
#define PARAM_H

#include "codec.h"

typedef struct
{
  float Pitch;
  float Pv; 
  float pfVEVphase[SC6_PHASES];
} SUBFRAME_PARAM_SC;


typedef struct /** all members of this structure need to be used in encoder **/
{
  SUBFRAME_PARAM_SC SubParam[2];

  float fLog2Gain;      /** Spectral level                                  **/
  float pfASK[LPC_ORDER];         /** arc sinc area ratios                            **/
  float pfASKunquant[LPC_ORDER];  /** copy of unquantized arc sinc area ratios        **/
} PARAM_SC;


#if 0
/***** Parameter structure *****/
typedef struct /** all members of this structure need to be used in encoder **/
{
  float Pitch[2];      /** pitch value in time samples                     **/
  float Pv[2];         /** voice probability                               **/
  float fLog2Gain;      /** Spectral level                                  **/
  float pfASK[LPC_ORDER];         /** arc sinc area ratios                            **/
  float pfASKunquant[LPC_ORDER];  /** copy of unquantized arc sinc area ratios        **/
  float pfVEVphase[2][SC6_PHASES];  /** excitation phase -- NPEAKS                      **/
} PARAM_SC;
#endif

#endif /* PARAM_H */

