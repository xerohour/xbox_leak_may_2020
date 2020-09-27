/******************************************************************************
*                        Voxware Proprietary Material                         *
*                        Copyright 1996, Voxware, Inc                         *
*                            All Rights Reserved                              *
*                                                                             *
*                       DISTRIBUTION PROHIBITED without                       *
*                      written authorization from Voxware                     *
******************************************************************************/

/******************************************************************************
* Filename:       DecPlsSC.h
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
* $Header:   /export/phobos/pvcs/r_and_d/archives/scaleable/decode/DecPlsSC.h_v   1.5   13 Apr 1998 16:16:56   weiwang  $
******************************************************************************/

#ifndef DECPLSSC_H
#define DECPLSSC_H


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
unsigned short voxSetDecodeSC(void *pMemBlkD, unsigned short wScale);

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
int voxGetDecodeRateSC(const void *pMemBlkD);

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
int voxGetDecodeInitRateSC(const void *pMemBlkD);

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
 
unsigned char voxGetDecReqDataFlagSC(const void *pMemBlkD);

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
* Return:    write flag
*******************************************************************************/
void voxSetFrameSkippedSC(void *pMemBlkD, int iFrameSkipped);

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
void voxSetRepeatFrameSC(void *pMemBlkD, int iRepeatFrameFlag);

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
void voxSetWarpFactorSC(void *pMemBlkD, float wWarpFactor);

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
float voxGetWarpFactorSC(const void *pMemBlkD);

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
float VoxGetDecPrevPvSC(const void *pMemBlkD);

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
float VoxGetDecPrevPitchSC(const void *pMemBlkD);



#endif
