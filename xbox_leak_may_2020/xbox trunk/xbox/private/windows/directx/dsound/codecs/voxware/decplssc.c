/******************************************************************************
*                        Voxware Proprietary Material                         *
*                        Copyright 1996, Voxware, Inc                         *
*                            All Rights Reserved                              *
*                                                                             *
*                       DISTRIBUTION PROHIBITED without                       *
*                      written authorization from Voxware                     *
******************************************************************************/

/******************************************************************************
* Filename:       DecPlsSC.c
*                                                                              
* Purpose:        Provide decoder plus functions as the interface for the private
*                 decoder structure.
*                                                                              
* Functions:     voxSetDecodeSC(), ... etc.
*                                                                              
* Author/Date:    Wei Wang, 3/98.
*******************************************************************************
* Modifications: 
*                                                                              
* Comments:                                                                    
* 
* Concerns:
*
* $Header:   /export/phobos/pvcs/r_and_d/archives/scaleable/decode/DecPlsSC.c_v   1.5   13 Apr 1998 16:16:54   weiwang  $
******************************************************************************/

#include <stdlib.h>
#include <assert.h>

#include "codec.h"
#include "rate.h"
#include "paramSC.h"
#include "DecPriSC.h"
#include "DecPlsSC.h"

#include "xvocver.h"

/*******************************************************************************
*
* Function:  voxSetDecodeSC()
*
* Action:   Set the compression rate for SC36 (either 3K or 6K)
*
* Input:    pMemBlkD -- decoder memory block
*           wScale -- codec rate 
*
* Output:   pMemBlkD -- updated decoder memory block
*
* Globals:   none
*
* Return:    0: succeed, 1: failed
*******************************************************************************/

unsigned short voxSetDecodeSC(void *pMemBlkD, unsigned short wScale)
{
   DEC_MEM_BLK *pDecode_mblk=(DEC_MEM_BLK*)pMemBlkD;
   unsigned short ret = 1;   /* default: fail */

   assert(pMemBlkD != NULL);

   switch(wScale) {
      case SC_RATE_3K:
         if(pDecode_mblk->iSCRateValid&SC_RATE_3K) {
            pDecode_mblk->iSCTargetRate=SC_RATE_3K;
            ret = 0;  /* ok */
         }
         break;
      case SC_RATE_6K:
         if(pDecode_mblk->iSCRateValid&SC_RATE_6K) {
            pDecode_mblk->iSCTargetRate=SC_RATE_6K;
            ret = 0;  /* ok */
         }
   }

   return ret;
}


/*******************************************************************************
*
* Function:  voxGetDecodeRateSC()
*
* Action:   Get the compression rate 
*
* Input:    pMemBlkD -- decoder memory block
*
* Output:   none
*
* Globals:   none
*
* Return:    the target rate for SC codec.
*******************************************************************************/

int voxGetDecodeRateSC(const void *pMemBlkD)
{
   assert(pMemBlkD != NULL);
   return(((DEC_MEM_BLK*)pMemBlkD)->iSCTargetRate);
}

/*******************************************************************************
*
* Function:  voxGetDecodeInitRateSC()
*
* Action:   Get the initialized compression rate 
*
* Input:    pMemBlkD -- decoder memory block
*
* Output:   none
*
* Globals:   none
*
* Return:    the initial rate for SC codec.
*******************************************************************************/

int voxGetDecodeInitRateSC(const void *pMemBlkD)
{
   assert(pMemBlkD != NULL);
   return(((DEC_MEM_BLK*)pMemBlkD)->iSCRateValid);
}  


/*******************************************************************************
*
* Function:  voxGetDecReqDataFlagSC()
*
* Action:    Get the RequestDataFLAG flag
*
* Input:     pMemBlkD -- decoder memory block
*
* Output:    none
*
* Globals:   none
*
* Return:    RequestDataFLAG
*******************************************************************************/

unsigned char voxGetDecReqDataFlagSC(const void *pMemBlkD)
{
   assert(pMemBlkD != NULL);

   assert((((DEC_MEM_BLK*)pMemBlkD)->iNewFrame==1)||
          (((DEC_MEM_BLK*)pMemBlkD)->iNewFrame==0));

   return (unsigned char)(((DEC_MEM_BLK*)pMemBlkD)->iNewFrame);
}  


/*******************************************************************************
*
* Function:  voxSetFrameSkippedSC()
*
* Action:   Set frame skipped flag
*
* Input:    pMemBlkD -- decoder memory block
*           iFrameSkipped -- frame skip flag
*
* Output:   none
*
* Globals:   none
*
* Return:    none
*******************************************************************************/
void voxSetFrameSkippedSC(void *pMemBlkD, int iFrameSkipped)
{
  assert(pMemBlkD != NULL);
  ((DEC_MEM_BLK*)pMemBlkD)->iFramesSkipped = iFrameSkipped;
}

/*******************************************************************************
*
* Function:  voxSetRepeatFrameSC()
*
* Action:   Set repeat frame flag
*
* Input:    pMemBlkD -- decoder memory block
*           iRepeatFrameFlag -- repeat-frame flag
*
* Output:   none
*
* Globals:   none
*
* Return:    nont
*******************************************************************************/
void voxSetRepeatFrameSC(void *pMemBlkD, int iRepeatFrameFlag)
{
  assert(pMemBlkD != NULL);
  ((DEC_MEM_BLK*)pMemBlkD)->iRepeatFrameFlag = iRepeatFrameFlag;
}


/*******************************************************************************
*
* Function:  voxSetWarpFactorSC()
*
* Action:   Set wWarpFactor
*
* Input:    pMemBlkD -- decoder memory block
*           wWarpFactor -- warp factor
*
* Output:   none
*
* Globals:   none
*
* Return:    none
*******************************************************************************/
void voxSetWarpFactorSC(void *pMemBlkD, float fWarpFactor)
{
  assert(pMemBlkD != NULL);
  ((DEC_MEM_BLK*)pMemBlkD)->fWarpedLengthFactor = fWarpFactor;
}


/*******************************************************************************
*
* Function:  voxGetWarpFactorSC()
*
* Action:   Get Warp Factor
*
* Input:    pMemBlkD -- decoder memory block
*
* Output:   none
*
* Globals:   none
*
* Return:    the warp factor
*******************************************************************************/
float voxGetWarpFactorSC(const void *pMemBlkD)
{
  assert(pMemBlkD != NULL);
  return ((DEC_MEM_BLK*)pMemBlkD)->fWarpedLengthFactor;
}


/*******************************************************************************
*
* Function:  VoxGetDecPrevPvSC()
*
* Action:   Get Pv_1
*
* Input:    pMemBlkD -- decoder memory block
*
* Output:   none
*
* Globals:   none
*
* Return:    Pv_1
*******************************************************************************/
float VoxGetDecPrevPvSC(const void *pMemBlkD)
{
  assert(pMemBlkD != NULL);
  return ((DEC_MEM_BLK*)pMemBlkD)->PrevSubParam.Pv;
}

/*******************************************************************************
*
* Function:  VoxGetDecPrevPitchSC()
*
* Action:   Get Pitch_1
*
* Input:    pMemBlkD -- decoder memory block
*
* Output:   none
*
* Globals:   none
*
* Return:    Pitch_1
*******************************************************************************/
float VoxGetDecPrevPitchSC(const void *pMemBlkD)
{
  assert(pMemBlkD != NULL);
  return ((DEC_MEM_BLK*)pMemBlkD)->PrevSubParam.Pitch;
}
