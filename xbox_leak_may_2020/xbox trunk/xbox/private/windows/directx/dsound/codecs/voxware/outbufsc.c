/******************************************************************************
*                        Voxware Proprietary Material                         *
*                        Copyright 1996, Voxware, Inc                         *
*                            All Rights Reserved                              *
*                                                                             *
*                       DISTRIBUTION PROHIBITED without                       *
*                      written authorization from Voxware                     *
******************************************************************************/

/******************************************************************************
* Filename:   OutBufSC.c
*                                                                              
* Purpose:    Output speech to PCM buffer
*                                                                              
* Functions:  PreOutputManagerSC(), PostOutputManagerSC()
*
* Author/Date:   Wei Wang, 2/18/98
*******************************************************************************
* Modifications: 
*            
* Comments: Only for half frame synthesizer
* 
* Concerns:
*
* $Header:   /export/phobos/pvcs/r_and_d/archives/scaleable/decode/OutBufSC.c_v   1.7   14 Apr 1998 10:14:16   weiwang  $
******************************************************************************/

#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "codec.h"
#include "vLibMath.h"
#include "OutBufSC.h"

#include "xvocver.h"

/******************************************************************************
* Function:   void PreOutputManagerSC()
*
* Action: output pfSaveBuffer to psPCMBuffer. Then pvSaveBuffer
*         can be reused later.
*
* Input:      pfSaveBuffer -- samples from previous frame
*             iSaveSamples -- number of samples in pfSaveBuffer
*             psPCMBuffer --  pointer to output speech buffer
*
* Output:     psPCMBuffer -- output speech
*
* Globals:    none
*
* Return:     none
*******************************************************************************
* Implementation/Detailed Description:
*
* References:
*******************************************************************************
* Modifications:
*
* Comments:   iSaveSamples can only be 0 or HALF_FRAME_LENGTH_SC.
*
* Concerns/TBD:
******************************************************************************/

void PreOutputManagerSC(const float *pfSaveBuffer, int iSaveSamples, 
                        short *psPCMBuffer)
{
  int i;
  int iTemp; 
  float ftmp;

  assert(iSaveSamples <= HALF_FRAME_LENGTH_SC);
  assert((pfSaveBuffer != NULL) && (psPCMBuffer != NULL));

  /*--------------------------------------------------
    Copy pfSaveBuffer to psPCMBuffer.
    --------------------------------------------------*/
  for (i = 0; i < iSaveSamples; i++) {
    ftmp = pfSaveBuffer[i];
    VoxROUND2(ftmp, iTemp);
    psPCMBuffer[i] = (short)iTemp;
  }
}


/******************************************************************************
* Function:   void PostOutputManagerSC()
*
* Action:     Output speech to PCM buffer and update pfSaveBuffer.
*
* Input:      *pfSaveBuffer -- synthesized speech buffer
*             *piSaveSamples -- number of saved samples
*             iSynSamples -- number of synthesized speech
*             *psPCMBuffer -- pointer to output speech buffer
*
* Output:     *pfSaveBuffer -- saved samples for next frame
*             *piSaveSamples -- updated number of saved samples
*             *psPCMBuffer -- output PCM buffer
*             *pwPCMsamples -- number of samples in psPCMBuffer 
*                              (either 0 or FRAME_LENGTH_SC).
*
* Globals:    none
*
* Return:     none
*******************************************************************************
* Implementation/Detailed Description:
*
* References:
*******************************************************************************
* Modifications:
*
* Comments: iSaveSamples is 0 or HALF_FRAME_LENGTH_SC (80) and iSynSamples is
*           0, HALF_FRAME_LENGTH_SC (80) or FRAME_LENGTH_SC (160).
*
* Concerns/TBD:  
******************************************************************************/

void PostOutputManagerSC( float          *pfSaveBuffer, 
                          int            *piSaveSamples, 
                          int             iSynSamples,
                          short          *psPCMBuffer, 
                          unsigned short *pwPCMsamples
                        )

{
  int i;
  int iSaveSamples = *piSaveSamples;
  int iWriteSamples;
  short *psTemp;
  int iTemp;
  float ftmp;

  assert(iSaveSamples <= HALF_FRAME_LENGTH_SC);
  assert((pfSaveBuffer!=NULL)&&(psPCMBuffer!=NULL)&&(pwPCMsamples!=NULL));

  /*----------------------------------------------------------------------
    Case 1: When sum of iSaveSamples and iSynSamples is less than 
            FRAME_LENGTH_SC, write_flag is set to 0. 
            Then speech should not output to PCMBuffer. 
            Such as: iSaveSamples = 0,  iSynSamples = 0 or 80.
                     iSaveSamples = 80, iSynSamples = 0
   ----------------------------------------------------------------------*/
   if ((iSaveSamples+iSynSamples) < FRAME_LENGTH_SC) 
     {
       *pwPCMsamples = 0;

       /*------------------------------------------------------------
         Since pfSynBuffer shares with pfSaveBuffer, we need to make 
         sure that pfSaveBuffer won't share with other memory. 
         Otherwise, speech in PCMBuffer need be re-written
         to pfSaveBuffer if iSaveSamples is larger than 0.

         Such as iSaveSamples = 80, iSynSamples = 0;
        ------------------------------------------------------------*/
       for (i = 0; i < iSaveSamples; i++)
          pfSaveBuffer[i] = (float)psPCMBuffer[i];

       if (iSynSamples > iSaveSamples)
         iSaveSamples = iSynSamples;
     }
   else
     {
        /*----------------------------------------------------------------------
         Case 2: If sum of iSaveSamples and iSynSamples is larger or equal to 
             FRAME_LENGTH_SC, one frame of speech will be output to PCMBuffer.
             such as: iSaveSamples = 80, iSynSamples = 80 or 160,
                      iSaveSamples = 0, iSynSamples = 160.
         ---------------------------------------------------------------------*/
        *pwPCMsamples = FRAME_LENGTH_SC;

        psTemp = &(psPCMBuffer[iSaveSamples]);    
        iWriteSamples = FRAME_LENGTH_SC - iSaveSamples;
        for (i = 0; i < iWriteSamples; i++) {
          ftmp = pfSaveBuffer[i];
          VoxROUND2(ftmp, iTemp);
          psTemp[i] = (short)iTemp;
	}


        /*------------------------------------------------------------
          If sum of iSaveSamples and iSynSamples is larger than 
          FRAME_LENGTH_SC, such as iSaveSamples = 80, iSynSamples = 160, 
          the extra 80 samples in pfSaveBuffer need to be shuffled to 
          the beginning.
         ------------------------------------------------------------*/
        iSaveSamples = iSynSamples - iWriteSamples;
        memcpy(pfSaveBuffer, &(pfSaveBuffer[iWriteSamples]), 
               iSaveSamples*sizeof(float));
     }

    *piSaveSamples = iSaveSamples;
}

