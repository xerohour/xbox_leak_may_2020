/*******************************************************************************
*                        Voxware Proprietary Material                          *
*                        Copyright 1996, Voxware, Inc                          *
*                            All Rights Reserved                               *
*                                                                              *
*                       DISTRIBUTION PROHIBITED without                        *
*                      written authorization from Voxware                      *
*******************************************************************************/

/*******************************************************************************
* Filename: FadeSC.h
*                                                                              
* Purpose: Fade-In/ Fade-Out during frame loss conditions 
*
* Functions: VoxFadeSC,  VoxinitFadeIO,  VoxfreeFadeIO,  
*         SetCurrentFrameState, GetCurrentFrameState, GetLastFrameState
*                                                                              
* Author/Date: RZ/ 02-26-97
********************************************************************************
* Modifications: RZ/ 07/30/97 Modified for SC
*                   
* Comments:                                                                    
* 
* Concerns:
*
* $Header:   /export/phobos/pvcs/r_and_d/archives/scaleable/utils/FadeSC.h_v   1.3   24 Mar 1998 10:14:42   bobd  $
*******************************************************************************/
#ifndef FADESC_H
#define FADESC_H

/*--------------------------------------------------
  global definitions
  --------------------------------------------------*/
#ifndef SILENCE
#define SILENCE                    0
#endif

#ifndef NORMAL
#define NORMAL                     1
#endif

/*******************************************************************************
* Function:    void VoxFadeSC(void *pvFadeIOMemBlk, void *pvParamMemBlk)
*
* Action:      Perform FadeIn/ FadeOut during frame loss
*
* Input:  pvFadeSCMemBlk -- pointer to FadeSC memory block
*         pvParamMemBlk --  pointer to parameter memory block
* 
* Output: pvFadeSCMemBlk -- updated FadeSC memory block
*         pvParamMemBlk --  updated parameter memory block
*
* Globals:  none
*
* Return:   none
*******************************************************************************/
void VoxFadeSC(void *pvFadeSCMemBlk, void *pvParamMemBlk);

/*******************************************************************************
* Function:  unsigned short VoxinitFadeIO(void **hFadeSCMemBlk)
*
* Action:    Initializes the LPCSyn struct
*
* Input:     hFadeSCMemBlk -- pointer to FadeSC memory block
*
* Output:    hFadeSCMemBlk -- memory allocated and initialized FadeSC memory 
*                               block
*
* Globals:   none
*
* Return:    0: succeed, 1: failed
*******************************************************************************/
unsigned short VoxinitFadeSC(void **hFadeSCMemBlk);

/*******************************************************************************
* Function:  unsigned short VoxfreeFadeIO(void **hFadeSCMemBlk)
*
* Action:   free up fade SC memory block
*
* Input:   hFadeSCMemBlk -- pointer to FadeSC memory block
*
* Output:  hFadeSCMemBlk -- memory freed FadeSC structure
*
* Globals:  none
*
* Return:  0 -- succeed
*******************************************************************************/
unsigned short VoxfreeFadeSC(void **hFadeSCMemBlk);

/*******************************************************************************
* Function:  SetCurrentFrameStateSC()
*
* Action:    Set the current frame state inside FadeSC structure
*
* Input:     pvFadeSCMemBlk -- FadeSC structure
*            iFrameFlag -- the frame state (either 0 or 1)
*
* Output:    pvFadeSCMemBlk -- the updated FadeSC structure
*
* Globals:   none
*
* Return:    none
*******************************************************************************/
void SetCurrentFrameStateSC(void *pvFadeSCMemBlk, const int iFrameFlag);


/*******************************************************************************
* Function:  GetCurrentFrameStateSC()
*
* Action:    Get the current frame state from FadeSC structure
*
* Input:     pvFadeSCMemBlk -- FadeSC structure
*            
* Output:    same as return
*
* Globals:   none
*
* Return:    the current frame state from FadeSC structure
*******************************************************************************/

int GetCurrentFrameStateSC(const void *pvFadeSCMemBlk);

/*******************************************************************************
* Function:  GetLastFrameStateSC()
*
* Action:    Get the last frame state from FadeSC structure
*
* Input:     pvFadeSCMemBlk -- FadeSC structure
*            
* Output:    same as return
*
* Globals:   none
*
* Return:    the last frame state from FadeSC structure
*******************************************************************************/

int GetLastFrameStateSC(const void *pvFadeSCMemBlk);

/*******************************************************************************
* Function:  UpdateFadeStateSC()
*
* Action:    Update the state of the Fade routine for the case where the
*              the decoder requires multiple calls to decode one frame of
*              parameters.
*
* Input:     pvFadeSCMemBlk  -- fade memory block
*            DecReqDataFlag  -- flag from decoder if decoder needs new data.
*
* Output:    none
*
* Globals:   none
*
* Return:    none
********************************************************************************
* Implementation/Detailed Description:
*              If the DecReqDataFlag is NOT true, then the decoder is still
*          working on the last frame.  When this happens CurrentFrameState
*          must be set to LastFrameState if LastFrameState is SILENCE.
*
* References:
********************************************************************************
* Modifications:
*
* Comments:
*
* Concerns/TBD:
*******************************************************************************/
 
void UpdateFadeStateSC(const void *pvFadeSCMemBlk, 
                       unsigned char DecReqDataFlag);

#endif /* FADESC_H */
