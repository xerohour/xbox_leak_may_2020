/*----------------------------------------------------------------------
* NAME:		STFTPhase.c
* PURPOSE:	Subroutine of "EncSTC.c" to compute the STFT 
*               phases at the harmonics.
* CREATED: 8-Apr-97 rjm                         VOXWARE INC
* LATEST REVISION: 26-mar-97 rjm
*-----------------------------------------------------------------------
* Copyright 1997 VOXWARE INC
* This software is not for release and should neither be copied
* nor distributed without explicit permission of$VOXWARE INC
------------------------------------------------------------------------
* $Header:   /export/phobos/pvcs/r_and_d/archives/scaleable/encode/STFTfaze.c_v   1.3   07 Jul 1998 17:50:26   bobd  $
----------------------------------------------------------------------*/
#include <math.h>
#include <assert.h>

#include "vLibMath.h"

#include "codec.h"
#include "STFTfaze.h"

#include "xvocver.h"


/**********************************************************************/

unsigned short VoxSTFTPhase ( float *pfSWR,
                              float *pfSWI,
                              int   iVEVphases,
                              float *pfVEVphase,
                              float fPitchDFT,
                              short iWinShift
                            )
{
   int   i, iNpeaks, iBasePeaks, iIndex;
   float fFreq, fSTFTphase;
   float ftmp;

   float fPhaseShift, fDeltaPhase;
   int   iMhat;

 /*** Push down quantized STFT phases from previous 10ms frame ***/

   for (i = 0; i < iVEVphases; i++)
   {
      pfVEVphase[i]   = 0.0F;
   }



 /*** Compute the number of harmonics in [0,PI) ***/
 
   iNpeaks = (int)(NFFT_2/fPitchDFT);
   ftmp=iNpeaks*fPitchDFT;
   VoxROUND2pos(ftmp, iIndex);
   if (iIndex >= NFFT_2)
         iNpeaks --;
 
   if (iNpeaks > MAXHARM)
         iNpeaks = MAXHARM;
 
 

 /*** Establish the number of phases to be coded ***/

   iBasePeaks = iNpeaks;
   if (iBasePeaks > iVEVphases)
       iBasePeaks = iVEVphases;

   /**** compute the phase shift. If the center shift to the left, iWinShift < 0, 
     then the phase should have a negative compensation. If iWinShift > 0,
     phase have a positive compensation. ****/ 
   fPhaseShift = (float)iWinShift * fPitchDFT * TWOPIONFFT;
   fDeltaPhase = 0.0F;

 /*** Compute the sine-wave phases as harmonic samples of the STFT phase ***/

   fFreq = 0.0F;
   for (i = 0; i < iBasePeaks; i++)
   {
      fFreq += fPitchDFT;
      VoxROUND2pos(fFreq, iIndex);
      fSTFTphase = VoxATAN2(pfSWI[iIndex], pfSWR[iIndex]);

      fDeltaPhase += fPhaseShift;
      fSTFTphase += fDeltaPhase;

      /*----------------------------------------------------------
        This code computes fSTFTphase modulo 2 pi. 

          ##### The argorithm is theoretically correct, but
                it is not robust to finite precision arithmetic.
                We need a better way to do this.
      ----------------------------------------------------------*/
      ftmp = fSTFTphase*INVTWOPI;
      VoxROUND2(ftmp, iMhat);

      fSTFTphase -= (float)iMhat * TWOPI;
      if (fSTFTphase < 0.0F)
          fSTFTphase += TWOPI;

      assert((fSTFTphase>=0) && (fSTFTphase<=TWOPI));

      pfVEVphase[i] = fSTFTphase;
   }

 return 0;


} /*VoxSTFTPhase*/


/**********************************************************************/


unsigned short VoxCodePhase ( float fPhase,
                              int   iNlevels,
                              float *pfPhase_q
                            )
{
   int   iIndex;
   float fArg, fArg_q;


 /*** 0 <= Phase <= TWOPI ***/
   fArg = iNlevels*fPhase*INVTWOPI;

   iIndex = (int)fArg;  /** 0 <= iIndex <= (iNlevels-1) **/

   fArg_q = 0.5F + (float)iIndex;

   *pfPhase_q =  TWOPI*fArg_q/iNlevels;

   return 0;
}

/**********************************************************************/
