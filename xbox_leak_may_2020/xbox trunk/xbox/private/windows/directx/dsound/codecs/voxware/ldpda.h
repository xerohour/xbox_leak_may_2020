/*******************************************************************************
*                        Voxware Proprietary Material                          *
*                        Copyright 1997, Voxware, Inc                          *
*                            All Rights Reserved                               *
*                                                                              *
*                       DISTRIBUTION PROHIBITED without                        *
*                      written authorization from Voxware                      *
******************************************************************************/

/*******************************************************************************
* Filename:       LDPDA.h
*                                                                              
* Purpose:        Estimate pitch using frequency domain cost function and back-
*                 ward tracking.
*                                                                              
* Functions:      VoxLDPDA(), GetPitchTable(),
*                 VoxInitLDPDA(), VoxFreeLDPDA()
*
* Author/Date:    Original developed by Gerard Aguilar   01/97
*                 Enhanced R&D by Wei Wang   04/97
*
********************************************************************************
* Modifications:
*                3/30/98, W.W. Rewrite whole file and use inter-leave format
*                spectrum. 
*  
*                05/97, W.W.
*                (1) Use longer table and change the boundary calculation. 
*                      (LDPDA.1)
*                (2) Change the boundary calculation using masker value and 
*                      use cosine table. (LDPDA.2).
*                (3) Use pitch table instead of fundamental frequency table.
*                (4) reduce table to 86 entries and expand the candidate from 
*                      8 to 160.
*                (5) add refinement for best candidates.
*                (6) when pitch is noise-like or (plusive), put pitch value 
*                      higher.
*                (7) put fix-point indeces for Calc_Cost_Func and VoicingLD.
*                                                                              
* Comments:                                                                    
* 
* Concerns:
*
* $Header:   /export/phobos/pvcs/r_and_d/archives/scaleable/pea/LDPDA.h_v   1.1   02 Apr 1998 14:46:56   weiwang  $
******************************************************************************/

#ifndef _LDPDA_H_
#define _LDPDA_H_

/******************************************************************************
* Function:  VoxLDPDA
*
* Action:    Estimate pitch value from frequency domain spectrum. This is 
*            the main function for coarse pitch calculation.
*
* Input:     pfSW -- input spectrum
*            hLD_PDA_mblk -- PDA structure
*
* Output:    none
*
* Globals:   none
*
* Return:    pitch value
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

float VoxLDPDA( STACK_R 
                const float *pfSW, 
                void        *hLD_PDA_mblk
              );

/******************************************************************************
* Function:  GetPitchTable()
*
* Action:    Export pitch table and table size.
*
* Input:     pfPitchTable -- pointer for output pitch table
*            piSize       -- pointer for output table size  
*
* Output:    pfPitchTable -- pitch table
*            piSize       -- table size
*
* Globals:   none
*
* Return:    none
*******************************************************************************
*
* Modifications:
*
* Comments:
*
* Concerns/TBD:
*
******************************************************************************/

void GetPitchTable( float  **pfPitchTable, 
                    int    *piSize
                  );

/******************************************************************************
* Function:  VoxInitLDPDA()
*
* Action:    initialize LDPDA memory block.
*
* Input:     hLD_PDA_mblk -- pointer to LDPDA memory block
*
* Output:    hLD_PDA_mblk -- initialized LDPDA memory block
*
* Globals:   none
*
* Return:    none
*******************************************************************************
*
* Modifications:
*
* Comments:
*
* Concerns/TBD:
*
******************************************************************************/

unsigned short VoxInitLDPDA(void **hLD_PDA_mblk);



/******************************************************************************
* Function:  VoxFreeLDPDA()
*
* Action:    free the memory which's allocated for LDPDA memory block.
*
* Input:     hLD_PDA_mblk -- pointer to LDPDA memory block
*
* Output:    hLD_PDA_mblk -- memory-freed LDPDA memory block
*
* Globals:   none
*
* Return:    none
*******************************************************************************
*
* Modifications:
*
* Comments:
*
* Concerns/TBD:
*
******************************************************************************/

unsigned short VoxFreeLDPDA(void **hLD_PDA_mblk);

#endif  /* _PITCH_H*/
