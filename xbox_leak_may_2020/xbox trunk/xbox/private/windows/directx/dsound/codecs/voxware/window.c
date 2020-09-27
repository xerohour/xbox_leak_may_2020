/*******************************************************************************
*                        Voxware Proprietary Material                          *
*                        Copyright 1996, Voxware, Inc                          *
*                            All Rights Reserved                               *
*                                                                              *
*                       DISTRIBUTION PROHIBITED without                        *
*                      written authorization from Voxware                      *
*******************************************************************************/
 
/******************************************************************************
*
* Filename:      Window.c
*
* Purpose:       Use the current pitch estimate to select the pitch-adaptive
*                window.	
*
* Functions:
*
* Author/Date:   Bob McAulay/1/25/97
*
*******************************************************************************
*
* Modifications:
*                
*
* Comments:
*
* Concerns:
*
*******************************************************************************
* $Header:   /export/phobos/pvcs/r_and_d/archives/scaleable/encode/Window.c_v   1.1   10 Feb 1998 13:39:22   weiwang  $
*******************************************************************************
*
*   This software is owned by Voxware, Inc. or its suppliers.
*   Title, ownership rights, and intellectual property rights in
*   and to the software shall remain in Voxware, Inc. and/or its
*   suppliers.  The software in source code form remains a
*   confidential trade secret of Voxware, Inc. and/or its suppliers.
*
******************************************************************************/
#include <stdlib.h>
#include <assert.h>
#include <math.h>

#include "vLibMath.h"
#include "Window.h"
#include "codec.h"

#include "xvocver.h"

int VoxAdaptiveWindow ( float CoarsePitchPeriod)
{

  int iWindowLength;


  if (CoarsePitchPeriod > 96)       /* 261 (32.5ms) window */
  {
    iWindowLength = PDA_FRAME_SIZE1;
  }
  else if (CoarsePitchPeriod > 88)       /* 241 (30.0ms) window */
  {
    iWindowLength = PDA_FRAME_SIZE2;
  }
  else if (CoarsePitchPeriod > 80)       /* 221 (27.5ms) window */
  {
    iWindowLength = PDA_FRAME_SIZE3;
  }
  else if (CoarsePitchPeriod > 72)       /* 201 (25.0ms) window */
  {
    iWindowLength = PDA_FRAME_SIZE4;
  }
  else if (CoarsePitchPeriod > 64)       /* 181 (22.5ms) window */
  {
    iWindowLength = PDA_FRAME_SIZE5;
  }
  else if (CoarsePitchPeriod > 56)       /* 161 (20.0ms) window */
  {
    iWindowLength = PDA_FRAME_SIZE6;
  }
  else if (CoarsePitchPeriod > 48)       /* 141 (17.5ms) window */
  {
    iWindowLength = PDA_FRAME_SIZE7;
  }
  else if (CoarsePitchPeriod > 40)       /* 121 (15.0ms) window */
  {
    iWindowLength = PDA_FRAME_SIZE8;
  }
  else if (CoarsePitchPeriod > 32)       /* 101 (12.5ms) window */
  {
    iWindowLength = PDA_FRAME_SIZE9;
  }
  else                                   /* 81 (10.0ms) window */
  {
    iWindowLength = PDA_FRAME_SIZE10;
  }

/*:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::*/
 
 
return iWindowLength;
 
} /*VoxAdaptiveWindow()*/
 

/********************************************************************/

