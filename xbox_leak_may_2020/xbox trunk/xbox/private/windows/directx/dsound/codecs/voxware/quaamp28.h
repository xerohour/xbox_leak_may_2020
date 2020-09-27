/*******************************************************************************
*                        Voxware Proprietary Material                          *
*                        Copyright 1996, Voxware, Inc                          *
*                            All Rights Reserved                               *
*                                                                              *
*                       DISTRIBUTION PROHIBITED without                        *
*                      written authorization from Voxware                      *
*******************************************************************************/

/*******************************************************************************
* Filename:          QuaAmp28.h                                                         
*                                                                              
* Purpose:           Quantize residual amplitudes for RT28                                                          
*                                                                              
* Functions:                                                                   
*                                                                              
* Author/Date:                                                                 
********************************************************************************
* Modifications:
*                   
* Comments:                                                                      
* 
* Concerns:
*
* 
*******************************************************************************/
#ifndef QUAAMP28_H
#define QUAAMP28_H

void QuaAmpRT28(STACK_R void *vpParam_mblk, void *QUAN_mblk);
void UnQuaAmpRT28(STACK_R void *vpParam_mblk, void *QUAN_mblk);

#endif /* QUAAMP28_H */
 
