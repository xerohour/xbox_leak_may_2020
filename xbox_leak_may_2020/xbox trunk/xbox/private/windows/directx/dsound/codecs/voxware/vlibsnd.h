/*******************************************************************************
*                        Voxware Proprietary Material                          *
*                        Copyright 1997, Voxware, Inc                          *
*                            All Rights Reserved                               *
*                                                                              *
*                       DISTRIBUTION PROHIBITED without                        *
*                      written authorization from Voxware                      *
*                                                                              *
*******************************************************************************/
 
/*******************************************************************************
*
* File:         vLibSnd.h
*
* Purpose:
*
* Functions:
*
* Author/Date:
*******************************************************************************
* Modifications:
*
* Comments:
*
* Concerns:
*
* $Header:   /export/phobos/pvcs/pvcsproj/voxlib.prj/vlibsnd.h_v   1.11   02 Mar 1998 17:41:12   weiwang  $
******************************************************************************/
 
#ifndef __VLIBSND
#define __VLIBSND

#ifdef __cplusplus
extern "C" {
#endif

/* SpCBM.h */
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
******************************************************************************/
unsigned short InitSpCBM(int iLength, void **pvCircMblk);


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
******************************************************************************/
void FreeSpCBM(void **pvCircMblk);

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
******************************************************************************/
void PutFrameSpCBM( void  *pvCircMblk,  int iOffset,
                    int   iInLength,    const float *pfInBuff );

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
******************************************************************************/
void GetFrameSpCBM( void *pvCircMblk, int iDelay, int  iOutLength,
                    float *pfOutBuff);

#ifdef __cplusplus
}
#endif

#endif /* __VLIBSND */
