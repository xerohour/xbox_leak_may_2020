/*******************************************************************************
*                        Voxware Proprietary Material                          *
*                        Copyright 1996, Voxware, Inc                          *
*                            All Rights Reserved                               *
*                                                                              *
*                       DISTRIBUTION PROHIBITED without                        *
*                      written authorization from Voxware                      *
*******************************************************************************/

/*******************************************************************************
* Filename:       Setup.c
*                                                                              
* Purpose:        Prepare all frame parameters for the current frame or subframe
*                                                                              
* Functions:      VOXErr VoxSetup(void *pvDecMemBlk)
*                                                                              
* Author/Date:    Rob Zopf     03/01/96
********************************************************************************
* Modifications:Ilan Berci/ 10/96 Removed global dependencies
*                                 Modularized code segments (Encapsulation)
*                                 Removed scope changes
*                                 Changed error code procedure
*                                 Removed useless code segments
*                                 General optimization
*                                                                              
* Comments:                                                                    
* 
* Concerns:
*
* $Header:   /export/phobos/pvcs/r_and_d/archives/realtime/decode/Setup.c_v   1.2   02 Mar 1998 18:18:14   weiwang  $
*******************************************************************************/
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <assert.h>

#include "model.h"
#include "VoxMem.h"

#include "vLibSgPr.h"  /* SpecIntLn() */

#include "research.h"
#include "param.h"
#include "GetnHarm.h"
#include "Setup.h"

#include "xvocver.h"

static void
InterpolateLsfs(float *Lsf, float *prevLsf, float interpFactor, float *interpLsf);


/*******************************************************************************
* Function:       VOXErr VoxSetup(void *pvDecMemBlk)
*
* Action	: The basic idea involves the desire to output frame buffers of
*                180 samples, no matter what the warp factor is, and also to minimize
*                the amount of RAM necessary.
*
*                1) For warp=1, things are unchanged.
*                2) For warp<1, the frame is synthesized and may be buffered later until
*                   a full frame worth of data (180 samples) is obtained.
*                3) For warp>1, the frame is divided into equal length(L) subframes 
*                   such that L <=180 samples.
*                   
*                   - in each new subframe, all frame parameters (Amps, Lsf's, pitch, pv)
*                     are interpolated using the previous and current frames.
*                   - we have an unpack flag to set when we are on the last subframe 
*                     of the current frame.  This signals the need to unpack a new set of
*                     frame parameters before Decode is called again.
*                   - the new subframe lengths, overlaps, window lengths, etc. are calculated.
*                   - a minimum overlap length is used for small frames.
*
*                   - the interpolation factor (M/N) is calculated as follows
*
*				|                                      |
*				|              |           |           |
*				|              |           |           |
*			    prev. frame               subframe      cur. frame
*
*                              <--------   M  ------------>
*				<---------------------- N ------------->
*
* Input:          myPrevFrameParam_mblk 	- the previous frame parameters
*                 myParam_mblk		- the current frame parameters
*
* Output:         mySubFrameParam_mblk	- the new interpolated subframe parameters
*
* Globals:        none
*
* Return:		  VOXErr : Voxware return code
********************************************************************************
* Implementation/Detailed Description:
*
* References:
********************************************************************************
* Modifications:
*
* Comments:
*
* Concerns/TBD:
*******************************************************************************/
unsigned short VoxSetup(STACK_R void *pvParam_mblk, void *pvSubFrameParam_mblk, 
		void *pvPrevFrameParam_mblk, float Warp, unsigned char *unpackFLAG)
{
   PARAM         *Param_mblk=(PARAM *)pvParam_mblk;
   SubFrameParam *SubFrameParam_mblk=(SubFrameParam *)pvSubFrameParam_mblk;
   PrevFrameParam *PrevFrameParam_mblk=(PrevFrameParam *)pvPrevFrameParam_mblk;

   short          nHarm;

STACK_INIT
   float          Spec_sub_1[MAXHARM];
   float          Spec_sub[MAXHARM];
STACK_INIT_END

   int            i;
   float          interpFactor;

STACK_ATTACH (float *, Spec_sub_1)
STACK_ATTACH (float *, Spec_sub)

STACK_START

 /*---------------------------------------------
   Check if the codec is at a new frame ...
   ---------------------------------------------*/
   if (SubFrameParam_mblk->WarpFrame == 0)
   {
    /*----------------------------------------------
      Set the total number of Warp frames for this
      frame of encoded data ...
      ----------------------------------------------*/
      if (Warp < 1.0)
         SubFrameParam_mblk->totalWarpFrame = 1;
      else
         SubFrameParam_mblk->totalWarpFrame = (short) ceil(Warp);
      SubFrameParam_mblk->WarpFrame = SubFrameParam_mblk->totalWarpFrame;

    /*-----------------------------------------------
      - Set the synthesis frame length and number of
      subframes for this set of warp frames.
      - Make the number of subframes per time no more
      than during normal operation to prevent the
      complexity from increasing.
      - Make the frame length a multiple of the 
      eventual subframe length so that things work 
      out nicely in the decoder.
      -----------------------------------------------*/

      assert(Warp>0.0F);
      SubFrameParam_mblk->frameLEN = (short)(FRAME_LEN*Warp/SubFrameParam_mblk->totalWarpFrame);
      SubFrameParam_mblk->nSubs = (short) floor((SubFrameParam_mblk->frameLEN/ ((float)MAXSUBFRAMELEN)));
      SubFrameParam_mblk->frameLEN = (short) (SubFrameParam_mblk->nSubs*
                                     floor(SubFrameParam_mblk->frameLEN/((float)SubFrameParam_mblk->nSubs)));
   }

 /*------------------------------------------------
   Set the interpolation factor for this warp based
   on the Pv of the previous and current codec
   frames ...

   - if Pv is in transition from 0 to nonzero or
     visa-versa, don't interpolate the parameters.
   ------------------------------------------------*/
   if (((PrevFrameParam_mblk->Pv==0.0)&&(Param_mblk->Pv>0.0))||
       ((PrevFrameParam_mblk->Pv>0.0)&&(Param_mblk->Pv==0.0)))
   {
      if ((SubFrameParam_mblk->totalWarpFrame-SubFrameParam_mblk->WarpFrame)< (SubFrameParam_mblk->totalWarpFrame>>1))
      {
         interpFactor = 0.0F;
      }
      else
      {
         interpFactor = 1.0F;
      }
   }
   else
   {
      interpFactor = (SubFrameParam_mblk->totalWarpFrame-SubFrameParam_mblk->WarpFrame+1)
                     /((float)SubFrameParam_mblk->totalWarpFrame);
   } 


 /*-----------------------------------------------------
   Set the subframe memory structure to correct values.
   Interpolate from the original (param) values ...
   Do not interpolate Pv.
   -----------------------------------------------------*/
   SubFrameParam_mblk->Pitch = PrevFrameParam_mblk->Pitch + interpFactor*(Param_mblk->Pitch-PrevFrameParam_mblk->Pitch);

   nHarm = getnHarm(SubFrameParam_mblk->Pitch);
   if (nHarm > MAXHARM)
      nHarm = MAXHARM;

   SpecIntLn(PrevFrameParam_mblk->Pitch, SubFrameParam_mblk->Pitch, 
             (const int) nHarm, PrevFrameParam_mblk->Amp, Spec_sub_1, 
             (const unsigned int) MAXHARM);
   SpecIntLn(Param_mblk->Pitch, SubFrameParam_mblk->Pitch, (const int) nHarm, 
             Param_mblk->Amp, Spec_sub,  (const unsigned int) MAXHARM);

   for (i=0; i< nHarm; i++)
      SubFrameParam_mblk->Amp[i] = Spec_sub_1[i] + interpFactor*(Spec_sub[i]-Spec_sub_1[i]);
   for (i=nHarm; i< MAXHARM; i++)
      SubFrameParam_mblk->Amp[i] = 0.0F;

   InterpolateLsfs(Param_mblk->Lsf, PrevFrameParam_mblk->Lsf, interpFactor, SubFrameParam_mblk->Lsf);

   if (interpFactor < 0.5)
      SubFrameParam_mblk->Pv = PrevFrameParam_mblk->Pv;
   else
      SubFrameParam_mblk->Pv = Param_mblk->Pv;


 /*-------------------------------------------------------
   Decrement the warp frame, and check to see if unpacking
   is necessary next time around ...
   -------------------------------------------------------*/
   SubFrameParam_mblk->WarpFrame--;
   if (SubFrameParam_mblk->WarpFrame==0)
      *unpackFLAG = 1;
   else
      *unpackFLAG = 0;

 /*-------------------------------------------------------
   Update the previous frame parameters if we are finished
   with the current frame's data ...
   -------------------------------------------------------*/
   if (SubFrameParam_mblk->WarpFrame==0)
   {
      memcpy(PrevFrameParam_mblk->Amp, Param_mblk->Amp, sizeof(float)*MAXHARM);
      memcpy(PrevFrameParam_mblk->Lsf, Param_mblk->Lsf, sizeof(float)*LPC_ORDER);
      PrevFrameParam_mblk->Pv = Param_mblk->Pv;
      PrevFrameParam_mblk->Pitch = Param_mblk->Pitch;
      PrevFrameParam_mblk->Energy = Param_mblk->Energy;
   }

STACK_END

   return 0;
}

void VoxUpdateSubframe(void *pvSubFrameParam_mblk)
{

   SubFrameParam *SubFrameParam_mblk=(SubFrameParam *)pvSubFrameParam_mblk;

   memcpy(SubFrameParam_mblk->Amp_1, SubFrameParam_mblk->Amp, sizeof(float)*MAXHARM);
   memcpy(SubFrameParam_mblk->Lsf_1, SubFrameParam_mblk->Lsf, sizeof(float)*LPC_ORDER);
   SubFrameParam_mblk->Pv_1 = SubFrameParam_mblk->Pv;
   SubFrameParam_mblk->Pitch_1 = SubFrameParam_mblk->Pitch;
   SubFrameParam_mblk->Energy_1 = SubFrameParam_mblk->Energy;

}


/*******************************************************************************
* Function:       static void InterpolateLsfs(float *Lsf, float *prevLsf, float 
*                                             interpFactor, float *interpLsf)
*
* Action:
*
* Input:          Lsf		- pointer to the current lsf's
*		            prevLsf		- pointer to the previous lsf's
*		            interpFactor	- subframe interpolation factor (M/N)
*				|                                      |
*				|              |           |           |
*				|              |           |           |
*			    prev. frame               subframe      cur. frame
*
*                              <--------   M  ------------>
*				<---------------------- N ------------->
*
* Output:         interpLsf		- the subframe interpolated stable lsf's
*
* Globals:        none
*
* Return:		   VOXErr : Voxware return code
********************************************************************************
* Implementation/Detailed Description:
*
* References:
********************************************************************************
* Modifications:
*
* Comments:
*
* Concerns/TBD:
*******************************************************************************/
static void InterpolateLsfs(float *Lsf, float *prevLsf, float interpFactor, float *interpLsf)
{
   int i;
   
   for (i=0; i<LPC_ORDER; i++)
      interpLsf[i] = prevLsf[i] + (Lsf[i]-prevLsf[i])*interpFactor;
   for (i=1; i<LPC_ORDER; i++)
      if (interpLsf[i]<(interpLsf[i-1]+LSP_SEPERATION_HZ))
            interpLsf[i] = (float)(interpLsf[i-1]+LSP_SEPERATION_HZ);
   if (interpLsf[LPC_ORDER-1] > 3999.0F)
      interpLsf[LPC_ORDER-1] = 3999.0F;
}

