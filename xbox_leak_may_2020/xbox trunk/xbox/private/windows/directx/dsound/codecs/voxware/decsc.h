/******************************************************************************
*                        Voxware Proprietary Material                         *
*                        Copyright 1996, Voxware, Inc                         *
*                            All Rights Reserved                              *
*                                                                             *
*                       DISTRIBUTION PROHIBITED without                       *
*                      written authorization from Voxware                     *
******************************************************************************/

/******************************************************************************
* Filename:       DecSC.h
*                                                                              
* Purpose:        Decode speech model parameters and synthesize speech
*                 frame by frame and store in output buffer
*                                                                              
* Functions:      VoxDecodeSC(), VoxInitDecodeSC(), VoxFreeDecodeSC()
*                                                                              
* Author/Date:    Original developed by Bob McAulay and Bob Dunn 1/97
*******************************************************************************
* Modifications: changed output buffer manager, Wei Wang, 2/18/98
*                remove paramMblk from decoder structure, 2/19/98, Wei Wang
*                                                                              
* Comments:                                                                    
* 
* Concerns:
*
* $Header:   /export/phobos/pvcs/r_and_d/archives/scaleable/decode/DecSC.h_v   1.9   10 Apr 1998 16:10:46   weiwang  $
******************************************************************************/

#ifndef DECSC_H
#define DECSC_H

/*******************************************************************************
*
* Function:  VoxDecodeSC()
*
* Action:    main decoder function
*
* Input:    pPCMBuffer   -- pointer to the output PCM buffer
*           hMemBlkD     -- the decoder memory block
*           hParamMblk   -- parameter memory block
*
* Output:   pPCMBuffer   -- output PCM buffer
*           pwPCMsamples -- number of samples in the output PCM buffer
*           hMemBlkD     -- updated decoder memory block
*
* Globals:   none
*
* Return:    none
*******************************************************************************/


void VoxDecodeSC( STACK_R 
                  void           *hMemBlkD,
                  void           *hParamMblk,
                  short          *pPCMBuffer, 
                  unsigned short *pwPCMsamples 
                );

/*******************************************************************************
*
* Function:  VoxInitDecodeSC3()
*
* Action:    initialize decoder structure for SC3
*
* Input:    ppMemBlkD -- pointer to the decoder memory block (NULL)
*
* Output:   ppMemBlkD -- initialized decoder memory block
*
* Globals:   none
*
* Return:    0: succeed, 1: failed
********************************************************************************/

unsigned short VoxInitDecodeSC3(void **hMemBlkD);


/*******************************************************************************
*
* Function:  VoxFreeDecodeSC3()
*
* Action:    free decoder structure for SC3
*
* Input:    hMemBlkD -- pointer to the decoder memory block
*
* Output:   hMemBlkD -- freed decoder memory block (NULL)
*
* Globals:   none
*
* Return:    0: succeed, 1: failed
********************************************************************************/

unsigned short VoxFreeDecodeSC3(void **hMemBlkD);

/*******************************************************************************
*
* Function:  VoxInitDecodeSC6()
*
* Action:    initialize decoder structure for SC6
*
* Input:    ppMemBlkD -- pointer to the decoder memory block
*
* Output:   ppMemBlkD -- initialized decoder memory block
*
* Globals:   none
*
* Return:    0: succeed, 1: failed
********************************************************************************/

unsigned short VoxInitDecodeSC6(void **hMemBlkD);


/*******************************************************************************
*
* Function:  VoxFreeDecodeSC6()
*
* Action:    free decoder structure for SC6
*
* Input:    hMemBlkD -- pointer to the decoder memory block
*
* Output:   hMemBlkD -- modified decoder memory block with SC6 related fields
*                       freed.
*
* Globals:   none
*
* Return:    0: succeed, 1: failed
********************************************************************************
* Modifications:
*
* Comments:    Need call this function before call VoxFreeDecodeSC3().
*
* Concerns/TBD:
********************************************************************************/

unsigned short VoxFreeDecodeSC6(void **hMemBlkD);

#endif /* DECSC_H */






