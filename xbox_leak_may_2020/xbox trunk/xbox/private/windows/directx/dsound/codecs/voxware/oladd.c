/*******************************************************************************
*                        Voxware Proprietary Material                          *
*                        Copyright 1996, Voxware, Inc                          *
*                            All Rights Reserved                               *
*                                                                              *
*                       DISTRIBUTION PROHIBITED without                        *
*                      written authorization from Voxware                      *
*******************************************************************************/
 
/*******************************************************************************
* Filename:       OlAdd.c
*
* Purpose:        a generalized trapezoidal overlap-add routine.
*
* Functions:      OlAdd()
*
* Author/Date:    Rob Zopf 11/7/96
********************************************************************************
* $Header:   /export/phobos/pvcs/pvcsproj/voxlib.prj/oladd.c_v   1.11   25 Mar 1998 14:31:20   weiwang  $
*******************************************************************************/
#include <stdlib.h>
#include <assert.h>
#include <math.h>
#include <string.h>

#include "vLibVec.h"

#include "xvocver.h"

/*******************************************************************************
*
* Function:  OlAdd()
*
* Action:    a generalized trapezoidal overlap-add routine.
*
* Input:     const float *pfFadeOutBuf -> fade-out input buffer
*            const float *pfFadeInBuf  -> fade-in input buffer
*            int          iOverlapLen  -> length of overlap
*            int          iFrameLen    -> length of frame
*
* Output:    float       *pfOutBuf     -> output buffer
*
* Globals:   none
*
* Return:    void
********************************************************************************
*
*           iFrameLen
* /-------------------------------------\
*               iOverlapLen
*           /-----------------\
*
* ----------\__             __/
*              \__       __/
*                 \__ __/
*                  __X__
*               __/     \__
*            __/           \__
*           /                 \----------
*
*   pfFadeOutBuf
* |---------------------------|
*
*             pfFadeInBuf
*           |---------------------------|
*
********************************************************************************
*
* Modifications: Removed some variables and changed type (short) in call
*                to type (int). Bod Dunn 7/24/97.
*
* Comments: IMPORTANT: this routine ACCUMULATES into the output buffer.  If
*                      needed, the output buffer should be set to zero before
*                      calling this routine.
*
* Concerns/TBD:
*******************************************************************************/

void OlAdd ( const float *pfFadeOutBuf,
             const float *pfFadeInBuf,
             int         iOverlapLen,
             int         iFrameLen,
             float       *pfOutBuf
           )
{
   int      i;
   int      iNonOverlap;
   float    fInStep;
   float    fOutStep;
   float    fStep;

   assert( pfFadeOutBuf != NULL);
   assert( pfFadeInBuf != NULL);
   assert( pfOutBuf != NULL);
   assert( iOverlapLen >= 0 );
   assert( iFrameLen >= iOverlapLen );

   iNonOverlap = iFrameLen-iOverlapLen;

   /*--------------------------------------------------------
     If the non-overlap region has an odd length,
       there is an extra sample to add in.
   --------------------------------------------------------*/
   if (iNonOverlap&1)
      *pfOutBuf++ += *pfFadeOutBuf++;

   iNonOverlap >>= 1; /* process half now, half later */

   /*-------------------------------------------------------
     Accumulate non-overlap section into the output buffer 
   -------------------------------------------------------*/
   for (i=iNonOverlap; i>0; i--)
      *pfOutBuf++ += *pfFadeOutBuf++;

   /*-------------------------------------------------------
     Now process the overlap section
   -------------------------------------------------------*/
   if (iOverlapLen>0)
      fStep = 1.0F/((float)(iOverlapLen));
   else
      fStep = 0.0F;

   fInStep  = 0.0F;
   fOutStep = 1.0F;


   for (i=iOverlapLen; i>0; i--, fInStep+=fStep, fOutStep-=fStep)
   {
      *pfOutBuf++ += (*pfFadeOutBuf++ * fOutStep) + 
                     (*pfFadeInBuf++  * fInStep );
   }

   /*--------------------------------------------------------
     Finally, accumulate non-overlap section into end of
     the output buffer ...
   --------------------------------------------------------*/
   for (i=iNonOverlap; i>0; i--)
      *pfOutBuf++ += *pfFadeInBuf++;

}


