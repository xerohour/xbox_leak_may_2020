/*******************************************************************************
*                        Voxware Proprietary Material                          *
*                        Copyright 1996, Voxware, Inc                          *
*                            All Rights Reserved                               *
*                                                                              *
*                       DISTRIBUTION PROHIBITED without                        *
*                      written authorization from Voxware                      *
*******************************************************************************/

/******************************************************************************
*                        Voxware Proprietary Material
*                        Copyright 1997, Voxware, Inc
*                            All Rights Reserved
*
*                       DISTRIBUTION PROHIBITED without
*                      written authorization from Voxware
******************************************************************************/

/******************************************************************************
*
* Filename:    SpecInt.c
*
* Purpose:    Interpolate the harmonics from one pitch to another.
*
* Functions: SpecIntLn()
*
* Author/Date: Rob Zopf, 12/04/96
*
*******************************************************************************
*
* Modifications:
*
* Comments: 
*
* Concerns: 
*
* $Header:   /export/phobos/pvcs/pvcsproj/voxlib.prj/specint.c_v   1.12   07 Apr 1998 18:12:18   weiwang  $
******************************************************************************/
#include <string.h> 
#include <assert.h>
#include "vLibMath.h"
#include "vLibSgPr.h"

#include "xvocver.h"

#define H_QFACTOR     24                      /* Q24 */
#define HMASK         ((((long)1)<<H_QFACTOR)-1)
#define H_Q2P         (((unsigned long)1)<<H_QFACTOR)
#define INVHQ2P       (1.0F/H_Q2P)

/*******************************************************************************
* Function: SpecIntLn()
*
* Action: Interpolate the harmonic spectrum from one pitch to another using
*         linear interpolation.
*
* Input:  fPitch          - the original pitch lag (time-domain)
*         fPitchInt       - the pitch lag to interpolate the spectrum to.
*         iIntHarms       - the number of harmonics corresponding to fPitchInt
*         pfAmp           - pointer to the original harmonics
*         pfAmpInt        - pointer to a vector to put the interpolated spectrum
*         iMaxHarm        - maximum number of harmonics in fpAmpInt
*
* Output: pfAmpInt        - the interpolated harmonics
*
* Return:  None.
*
* Modifications:
*
* Comments: The pfAmp vector is zero padded up to iMaxHarm for unused 
*             harmonics.
*
*******************************************************************************/
void SpecIntLn( float           fPitch, 
                float           fPitchInt,  
                int             iIntHarms, 
                const float     *pfAmp,  
                float           *pfAmpInt,  
                unsigned int    iMaxHarm
              )
{
   int           i, harmsDone;
   float         interp;
   int harmNum;
   unsigned long harmFactor, totalFactor;
   
   /* error checking */
   assert(iIntHarms <= (int)iMaxHarm);
   assert(pfAmp != NULL);
   assert(pfAmpInt != NULL);
   assert(fPitchInt > 0.0F && fPitch > 0.0F);

   /* if iPitch==iPitchint Harmfactor==1 & interp==0.0  harmNum-1 == i 
      there is no need to do interpolation. "for" loops are doing nothing. 
      maximum value of harmNum could go up to MAXHARM which may cause 
      pfAmpInt index out of boundary
   */
   if(fPitch != fPitchInt) {
      harmFactor=(unsigned long)VoxFLOORposLong((float)H_Q2P *(fPitch/fPitchInt));
      totalFactor   = harmFactor;
      harmsDone     = 0;
      while (totalFactor <= H_Q2P)          
      {
         pfAmpInt[harmsDone] = totalFactor*INVHQ2P*pfAmp[0];
         harmsDone          ++; 
         totalFactor        += harmFactor;  
      }
      for (i=harmsDone; i<iIntHarms; i++)
      {
         harmNum      = (int)(totalFactor>>H_QFACTOR);
         interp       = (totalFactor&HMASK)*INVHQ2P;
         pfAmpInt[i]  = pfAmp[harmNum-1] +
                        (pfAmp[harmNum]-pfAmp[harmNum-1])*interp;
         totalFactor += harmFactor;
      }
   }
   else 
     memcpy(pfAmpInt, pfAmp, sizeof(float)*(iIntHarms));

   memset(pfAmpInt+iIntHarms, 0, sizeof(float)*(iMaxHarm-iIntHarms));
}
