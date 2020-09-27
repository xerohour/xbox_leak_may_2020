/*******************************************************************************
*                        Voxware Proprietary Material                          *
*                        Copyright 1996, Voxware Inc.                          *
*                        All Rights Reserved                                   *
*                                                                              *
*                        DISTRIBUTION PROHIBITED without                       *
*                        written authorization from Voxware.                   *
*******************************************************************************/

/*******************************************************************************
*
* Filename: BandExpd.c
*
* Purpose:  expand the bandwidth of LPC poles.
*
* Author:   Ilan
*
* Date:     Oct. 07, 1996.
*
* Modified by Wei so that the input and output share the same array.
*
* $Header:   /export/phobos/pvcs/pvcsproj/voxlib.prj/BandExpd.c_v   1.8   02 Mar 1998 17:40:56   weiwang  $
*******************************************************************************/

#include "vLibSpch.h"

#include "xvocver.h"

/*******************************************************************************
*
* Function: BandExpand()
*
* Input:    pIn:            pointer of the input LPC coefficients.
*           iLPCOrder:      order of the LPC coefficients.
*           fExpandCoeff:   expandion coefficient.
*
* Output:   pOut:           pointer of the output LPC coefficients.
*
* Globals:  none
*
* Return:   none
*******************************************************************************
*
* Implementation/Detailed Description:
*
* References:
*******************************************************************************
*
* Modifications:
*
* Comments:
*           In place and out of place operation are supported.
*
*           We define A(z) as:
*
*               A(z) = 1 + SUM(k=1...p) a_k * z^(-k)
*
*           with the format:
*
*              pIn[0] = 1.0
*              pIn[1] = a_1
*              pIn[2] = a_2
*                :       :
*                :       :
*              pIn[p] = a_p
*
* Concerns/TBD:
******************************************************************************/
void BandExpand( const float *pIn, 
                 float       *pOut, 
                 int          iLPCOrder,
                 float       fExpandCoeff
               )
{
   int   i;
   float dMultiplier;
 
   dMultiplier = 1.0F;
   for(i=0;i<=iLPCOrder;i++) 
   {
     *pOut++ = (float)(*pIn++ * dMultiplier);
     dMultiplier *= (float)fExpandCoeff;
   }
}

