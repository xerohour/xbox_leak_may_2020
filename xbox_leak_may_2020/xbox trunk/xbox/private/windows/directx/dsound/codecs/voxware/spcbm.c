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
* File:         SpCBM.c
*
* Purpose:      circular buffer manager routines
*
* Functions:
*
* Author/Date:  Wei Wang
*******************************************************************************
* Modifications:
*
* Comments:
*
* Concerns:
*
* $Header:   /export/phobos/pvcs/pvcsproj/voxlib.prj/SpCBM.c_v   1.10   06 Mar 1998 16:57:48   weiwang  $
******************************************************************************/
#include <stdlib.h>
#include <assert.h>
#include <math.h>
#include <string.h>

#include "vLibMacr.h"
#include "vLibSnd.h"

#include "xvocver.h"

/* Private function */
static int CalcCircBufPosition(int n, int j, int M);

/* typedef for circular buffer */
typedef struct tagSP_CIRC_BUF
{
   int    iLength;
   int    iIndex;
   float  *pfData;
} SP_CIRC_BUF;

/*******************************************************************************
* 
* Function: InitSpCBM()
*
* Action: initialize a structure containing circular buffer,its total 
*         length & index.
*
* Input:  pvCircMblk  - A structure containing circular buffer,its total 
*                       length & index.
*         iLength  - Length of the structur
*
* Output: pvCircMblk  - The initialised structure 
*
* return: 0: succeed, 1: failed
*
******************************************************************************
* Modifications:
*
* Comments:
*
* Concerns/TBD:
*
******************************************************************************/
unsigned short InitSpCBM(int iLength, void **pvCircMblk)
{
  SP_CIRC_BUF *pCirc_Mblk;
  unsigned short iRetFlag = 0;

  pCirc_Mblk = (SP_CIRC_BUF *)calloc(1,sizeof(SP_CIRC_BUF));
  *pvCircMblk = pCirc_Mblk;
  if(pCirc_Mblk == NULL)
     iRetFlag = 1;

  if (iRetFlag == 0)
    {
      pCirc_Mblk->iLength = iLength;
      pCirc_Mblk->iIndex  = 0;

      pCirc_Mblk->pfData = (float *)calloc(iLength,sizeof(float)); 
      if(pCirc_Mblk->pfData == NULL) 
        {
          SafeFree(pvCircMblk);
          iRetFlag = 1;
        }
    }

  return iRetFlag;
}

/*******************************************************************************
* 
* Function: FreeSpCBM()
*
* Action: free speech circular buffer structure
*
* Input:  pvCircMblk  --  circular buffer structure
*
* Output: pvCircMblk  -- NULL pointer
*
* return: none
*
******************************************************************************
* Modifications:
*
* Comments:
*
* Concerns/TBD:
*
******************************************************************************/

void FreeSpCBM(void **pvCircMblk)
{
  assert(pvCircMblk != NULL);

  if(*pvCircMblk)
    {
      SafeFree(((SP_CIRC_BUF *)*pvCircMblk)->pfData);
      SafeFree(*pvCircMblk);
   }
}


/*******************************************************************************
* 
* Function: PutFrameSpCBM()
*
* Action: put a frame of speech to a circular buffer 
*
* Input:  pvCircMblk  --  A structure containing circular buffer,its total 
*                         length & index.
*         iOffset     --  the relative distance from the start position for 
*                         putting speech to the circular buffer index.
*                         (in most case just set it to 0)
*         iInLength   -- Number of input data
*         pfInBuff    -- Input data
*
* Output: pvCircMblk  -- update structure pointer
*
* return: none
*
******************************************************************************
* Modifications:
*
* Comments:
*
* Concerns/TBD:
*
******************************************************************************/
void PutFrameSpCBM( void  *pvCircMblk,  int iOffset,
                    int   iInLength,    const float *pfInBuff )
{
   float        *pfSpCircBuf = ((SP_CIRC_BUF *)pvCircMblk)->pfData;
   int           iTotalLength = ((SP_CIRC_BUF *)pvCircMblk)->iLength;
   int           iIndex = ((SP_CIRC_BUF *)pvCircMblk)->iIndex;
   int           iStartPosition;
   int           iRemainLength;

   assert((pfInBuff != NULL) && (pvCircMblk != NULL));

   /*----------------------------------------------------
      Calculate the start position in circular
        buffer to copy data from inBuff
   ----------------------------------------------------*/
   iStartPosition = CalcCircBufPosition(iOffset, iIndex, iTotalLength);

   /*----------------------------------------------------
     remain length in circular buffer from the starting
       copying position to th end of the circular buffer
   ----------------------------------------------------*/
   iRemainLength = iTotalLength - iStartPosition;

   /*----------------------------------------------------
     Point to the start position in circular buffer
   ----------------------------------------------------*/
   if (iRemainLength >= iInLength)
   {
      /*----------------------------------------------------
        the remain length in circular is suffucient
          for copying data from InBuff
      ----------------------------------------------------*/
     memcpy(&(pfSpCircBuf[iStartPosition]), pfInBuff, iInLength*sizeof(float));
   }
  else {
      /*----------------------------------------------------
        the remain length in circular is shorter than
          the length to be copied from InBuff
      ----------------------------------------------------*/

      /*----------------------------------------------------
         fill up the remain length in circular buffer
      ----------------------------------------------------*/
     memcpy(&(pfSpCircBuf[iStartPosition]), pfInBuff,
            iRemainLength*sizeof(float));
    
      /*----------------------------------------------------
         fill up from the beginning of the circular buffer
      ----------------------------------------------------*/
     memcpy(pfSpCircBuf, &(pfInBuff[iRemainLength]), 
            (iInLength-iRemainLength)*sizeof(float));
   }

   /* update the Index -- independant with iOffset */
   iIndex = CalcCircBufPosition(iInLength, iIndex, iTotalLength);
   ((SP_CIRC_BUF *)pvCircMblk)->iIndex = iIndex;
}


/*******************************************************************************
* 
* Function: GetFrameSpCBM()
*
* Action: extract speech from a circular buffer 
*
* Input:  pvCircMblk  --  a structure containing circular buffer,its total 
*                         length & index.
*         iDelay      --  the relative distance from the middle position of the
*                         extracting speech to the current circular buffer 
*                         index.
*         iOutLength  --  number of output data
*         pfOutBuff   --  pointer to the output speech
*
* Output: pfOutBuff -- output speech
*
* return: none
*
******************************************************************************
* Modifications:
*
* Comments:
*
* Concerns/TBD:
*
******************************************************************************/

void GetFrameSpCBM( void *pvCircMblk, int iDelay, int  iOutLength,
                    float *pfOutBuff)
{
   float        *pfSpCircBuf = ((SP_CIRC_BUF *)pvCircMblk)->pfData;
   int          iTotalLength = ((SP_CIRC_BUF *)pvCircMblk)->iLength;
   int          iIndex = ((SP_CIRC_BUF *)pvCircMblk)->iIndex;
   int          iStartPosition;
   int          iRemainLength;

   assert(pvCircMblk != NULL && pfOutBuff != NULL);

   /*----------------------------------------------------
      get the starting pointer of the frame
      Calculate the starting position for extracting
      data, which should be half window 
      length + position of window center
   ----------------------------------------------------*/
   iDelay += ((iOutLength+1)>>1);

   /*----------------------------------------------------
     locate the starting position in curcular buffer
   ----------------------------------------------------*/
   iStartPosition = CalcCircBufPosition(-iDelay, iIndex, iTotalLength);
  
   /*----------------------------------------------------
     remain length in curcilar buffer
   ----------------------------------------------------*/
   iRemainLength = iTotalLength - iStartPosition;

   if (iRemainLength >= iOutLength)
   {
      /*----------------------------------------------------
        the remain length in circular is suffucient
          for copying data into OutBuff
      ----------------------------------------------------*/
      memcpy(pfOutBuff, &(pfSpCircBuf[iStartPosition]), 
             iOutLength*sizeof(float));

   } else {
      /*----------------------------------------------------
        the remain length in circular is shorter than
          the length to be copied into OutBuff

         fill up the remain length in circular buffer
      ----------------------------------------------------*/
      memcpy(pfOutBuff, &(pfSpCircBuf[iStartPosition]), 
             iRemainLength*sizeof(float));

      /*----------------------------------------------------
         fill up from the beginning of the circular buffer
      ----------------------------------------------------*/
      memcpy(&(pfOutBuff[iRemainLength]), pfSpCircBuf, 
             (iOutLength-iRemainLength)*sizeof(float));
   }

}



/*******************************************************************************
* 
* Function: CalcCircBufPosition()
*
* Action:   Calculate the position in circular buffer 
*
* Input:    iPosition -- start position relative to the circular buffer index
*           iIndex    -- circular buffer index
*           iLength   -- total length of circular buffer
*
* Output: none
*
* return: the position at the circular buffer
*
******************************************************************************
* Modifications:
*
* Comments:
*
* Concerns/TBD:
*
******************************************************************************/

static int CalcCircBufPosition(int iPosition, int iIndex, int iLength)
{
   return ((iLength+(iIndex+iPosition%iLength))%iLength);
}


