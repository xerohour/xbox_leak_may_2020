/*******************************************************************************
*                        Voxware Proprietary Material                          *
*                        Copyright 1997, Voxware, Inc                          *
*                            All Rights Reserved                               *
*                                                                              *
*                       DISTRIBUTION PROHIBITED without                        *
*                      written authorization from Voxware                      *
******************************************************************************/

/*******************************************************************************
* Filename:       VoicLD.c
*                                                                              
* Purpose:        voicing calculation for LDPDA.
*                                                                              
* Functions:     VoicingLD(), FindBand4VoicLD()
*
* Author/Date:    Wei Wang, 12/97
*
********************************************************************************
* Modifications: Reduce the length of refining window spectrum from 2048 to 112.
*                Use fix point indices to speech up code.
*                                                                              
* Comments:                                                                    
* 
* Concerns:
*
* $Header:   /export/phobos/pvcs/r_and_d/archives/scaleable/pea/VoicLD.c_v   1.6   14 Apr 1998 10:14:20   weiwang  $
******************************************************************************/

/**** system include files ****/
#include <stdlib.h>
#include <assert.h>
#include <math.h>

/**** codec related include files ****/
#include "codec.h"
#include "vLibMath.h"
#include "VoicLD.h"

#include "xvocver.h"

/**** table for real part of Kaiser window spectrum:
  fft( Kaiser(291,6),2^14 ). ****/
const static float tfWinSpec[] = {
#include "FD291SC.h"          
};

/**** local definitions ****/
#define REFINE_SPEC_SIZE  112 /* ((sizeof(tfWinSpec)/sizeof(float))>>1) */

#define REFINE_SPEC_BITS   5
#define REFINE_SPEC_SCALE  (1<<REFINE_SPEC_BITS)
#define F0_SHIFT_BIT  10
#define F0_SHIFT_SCALE  (float)(1<<F0_SHIFT_BIT)  
#define F0_SCALED_05  (1<<(F0_SHIFT_BIT-1))   

#define REF_F0_SHIFT_BITS  (F0_SHIFT_BIT+REFINE_SPEC_BITS) 
#define REF_F0_SHIFT_SCALE  (float)((long)1<<REF_F0_SHIFT_BITS)
#define REF_SCALED_05  ((long)1<<(REF_F0_SHIFT_BITS-1))

#define CONST_EXTEND_BAND  3.9F


/******************************************************************************
* Function:  VoicingLD()
*
* Action:    Estimate voicing probability and use it for final pitch seletion
*
* Input:     fPitch     -- pitch period
*            pfSW       -- complex spectrum
*            iNBand     -- length of spectrum for voicing probability 
*                          calculation
*
* Output:    none
*
* Globals:   none
*
* Return:    estimated voicing probability
*******************************************************************************
*
* Implementation/Detailed Description:
*
*******************************************************************************
*
* Modifications:
*
* Comments:  
*
* Concerns/TBD:
*
******************************************************************************/
float VoicingLD( float        fPitch,
                 const float  *pfSW, 
                 int          iNBand
               )
{
   const float *pfFWinCt = &(tfWinSpec[REFINE_SPEC_SIZE]); /* shift to center */
   float fF0;
   int iNHarm;
   long lF0, lBandWdth;
   int iLeftBound, iRightBound;
   long lHarmF0;
   int i, j;
   int iHF0;
   float ftmp;
   float fHarmMagReal, fHarmMagImag;
   float fErr1, fErr2;
   float fTotalError, fTotalPower;
   float *ptmpSW;
   int iWinIndex;
   float fVoicing = 0.0F;

   assert(pfSW != NULL);

   /*** initializing fF0 -- F0 in DFT domain ***/
   fF0 = (float)NFFT/fPitch;

   /*** initializing iNHarm -- number of harmonics within [0 iNBand] ***/
   iNHarm = (int)VoxFLOORpos((float)iNBand / fF0);

   /*** shift fF0 by REF_F0_SHIFT_BITS to get fix-point lF0 ***/
   ftmp = REF_F0_SHIFT_SCALE * fF0;
   VoxROUND2pos(ftmp, lF0);

   /*** initialize fix point bandwidth ***/
   lBandWdth = lF0 >> 1;

   /*** right boundary for each harmonics: round((i+0.5)*F0): i refers to 
     i'th harmonic ***/
   iRightBound = (int)((lBandWdth+REF_SCALED_05)>>REF_F0_SHIFT_BITS);

   /** compute reconstructed spectrum and compare it with original spectrum. **/
   lHarmF0 = 0L;
   for (j=1; j<=iNHarm; j++) {

     /*** update the left boundary and right boundary ***/
     lHarmF0 += lF0;
     iLeftBound = iRightBound;
     iRightBound = (int)((lBandWdth+lHarmF0+REF_SCALED_05)
                                >>REF_F0_SHIFT_BITS);

     /*** Get the magnitude of j'th harmonic ***/
     iHF0 = (int)((lHarmF0+REF_SCALED_05)>>REF_F0_SHIFT_BITS);

     ptmpSW = (float *)(&(pfSW[iHF0 << 1]));
     fHarmMagReal = ptmpSW[0];
     fHarmMagImag = ptmpSW[1];

     /**** reconstruct spectrum ****/
     fTotalPower = EPS;
     
     /*** initialize window index from j'th harmonic ***/
     iWinIndex = (int)(-((lHarmF0+F0_SCALED_05)>>F0_SHIFT_BIT)
                       +((long)iLeftBound<<REFINE_SPEC_BITS));

     /*** Since the indices for pfFWinCt[] is between [-REFINE_SPEC_SIZE
          REFINE_SPEC_SIZE]. Outside that range assume window spectra are 0 */
     /*** Deal with left side outliers: band error equals to band power ***/
     i = iLeftBound;
     ptmpSW = (float *)(&(pfSW[i<<1]));
     while (iWinIndex <= (-REFINE_SPEC_SIZE))
       {
         fErr1 = ptmpSW[0]*ptmpSW[0]+ptmpSW[1]*ptmpSW[1];

	 fTotalPower += fErr1;
	 iWinIndex += REFINE_SPEC_SCALE;
	 i++;
	 ptmpSW += 2;
       }
     fTotalError = fTotalPower;

     /*** Compute band errors and band power for the center part ***/
     while ((iWinIndex < REFINE_SPEC_SIZE) && (i < iRightBound))
       {
	 fTotalPower += ptmpSW[0]*ptmpSW[0] + ptmpSW[1]*ptmpSW[1];

	 fErr1 = ptmpSW[0] - fHarmMagReal * pfFWinCt[iWinIndex];
	 fErr2 = ptmpSW[1] - fHarmMagImag * pfFWinCt[iWinIndex];

	 fTotalError += fErr1 * fErr1 + fErr2 * fErr2;
	 iWinIndex += REFINE_SPEC_SCALE;
	 i++;
	 ptmpSW += 2;
       }
     
     /*** Deal with right side outliers: band errors equals to band power ***/
     while(i < iRightBound) 
       {
	 fErr1 = ptmpSW[0]*ptmpSW[0]+ptmpSW[1]*ptmpSW[1]; 
	 fTotalPower += fErr1;
	 fTotalError += fErr1;
	 i++;
	 ptmpSW += 2;
       }

     /**** voicing estimation of each band ****/
     fVoicing += fTotalError / fTotalPower; 
   }

   return  (1.0F - fVoicing/(float)iNHarm);
} /* VoicingLD() */


/******************************************************************************
* Function:  FindBand4VoicLD()
*
* Action:    Find out spectrum band for voicing calculation
*
* Input:     iBaseBand   -- band for pitch estimation
*            fMinPitch   -- minimum pitch period for voicing calculation
*
* Output:    none
*
* Globals:   none
*
* Return:    band for voicing calculation
*******************************************************************************
*
* Implementation/Detailed Description:
*
*******************************************************************************
*
* Modifications:
*
* Comments:  
*
* Concerns/TBD:
*
******************************************************************************/
int FindBand4VoicLD ( int   iBaseBand, 
                      float fMinPitch
                    )
{
  int iNewBand;
  float fBand;

  fBand = (float)NFFT/(float)fMinPitch;
  fBand *= CONST_EXTEND_BAND;

  VoxROUND2pos(fBand, iNewBand);

  if (iNewBand > iBaseBand)
    iNewBand = iBaseBand;

  return iNewBand;
}


