/*******************************************************************************
*                        Voxware Proprietary Material                          *
*                        Copyright 1996, Voxware, Inc                          *
*                            All Rights Reserved                               *
*                                                                              *
*                       DISTRIBUTION PROHIBITED without                        *
*                      written authorization from Voxware                      *
*******************************************************************************/

/*******************************************************************************
*
* Filename: MidPitch.c
*
* Purpose:  centerize the pitch by calculating the autocorrelation value.
*
* Author:   Wei Wang
*
* Date:     Jan. 22, 1996
*
* Modified : Xiaoqin Sun for middle frame pitch estimation
*
* Date:    May   30  1997
*
* $Header:   /export/phobos/pvcs/r_and_d/archives/scaleable/encode/MidPitch.h_v   1.0   06 Feb 1998 10:06:48   johnp  $
*******************************************************************************/
#ifndef _MIDPITCH_H_
#define _MIDPITCH_H_

#define   MAXFLT           3.402823466E+38F  
#define   MINCON           1.0e-6F    /* minimum constant */

float MidEstPitch(float Pitchm1, float Pitchp1, float Pvm1, float Pvp1);

float TDPitchRefine(float *Spbuf, short length, float pitch, float *maxCost, 
		    float deltaP0);

#endif /* end of #ifndef _MIDPITCH_H_  */

