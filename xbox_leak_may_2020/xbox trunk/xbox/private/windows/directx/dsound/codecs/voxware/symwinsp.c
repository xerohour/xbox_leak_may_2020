/*****************************************************************************
*                        Voxware Proprietary Material                       *
*                        Copyright 1996, Voxware Inc.                       *
*                        All Rights Reserved                                *
*                                                                           *
*                        DISTRIBUTION PROHIBITED without                    *
*                        written authorization from Voxware.                *
*****************************************************************************/
/*******************************************************************************
*
* File:  SymWinSp.c
*
* Purpose: Considering the symmetricity of window, the signal is windowed by 
*          only the half size of window coefficients.
*
* Author: Wei Wang
*
* Date:  Oct. 07, 1996
*
* $Header:   /export/phobos/pvcs/pvcsproj/voxlib.prj/SymWinSp.c_v   1.7   02 Mar 1998 17:41:02   weiwang  $
*
******************************************************************************
* Modifications:
*
* Comments: In-place operation is supported
*
* Concerns:
*
******************************************************************************/
#include <assert.h>
#include <stdlib.h>
#include "vLibSgPr.h"

#include "xvocver.h"

/*******************************************************************************
* 
* Function: SymWindowSignal()
*
* Action: considering the symmetricity of window, the signal is windowed by 
*         only the half size of window coefficients.
*
* Input:  pIn:    input signal.
*         pWin:   window data (only left half size == ((length+1)>>1)).
*         length: data length.
*
* Output: pOut:   output signal.
*
* Return: None.
*******************************************************************************
* Modifications:
*
* Comments: In-place operation is supported.
*
* Concerns/TBD:
******************************************************************************/
void SymWindowSignal(const float *pIn, const float *pWin, float *pOut,
                      int length)
{
   int half_length = length >> 1;
   float *pIn2, *pOut2;

   assert((pIn != NULL) && (pWin != NULL) && (pOut != NULL));
  
   /** window the signal in both sides **/
   pIn2 = (float *) &(pIn[length-1]);
   pOut2 = &(pOut[length-1]);
   while ( (half_length --) > 0 ) 
     {
        *pOut++ = *pIn++ * *pWin;
        *pOut2-- = *pIn2-- * *pWin++;
     }

   /* considing the length is odd number */
   if (length & 1)
      *pOut = *pIn * *pWin;
}


