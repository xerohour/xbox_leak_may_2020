/*******************************************************************************
*                        Voxware Proprietary Material                          *
*                        Copyright 1996, Voxware, Inc                          *
*                            All Rights Reserved                               *
*                                                                              *
*                       DISTRIBUTION PROHIBITED without                        *
*                      written authorization from Voxware                      *
*******************************************************************************/

/*******************************************************************************
* Filename:          QEnrgy15.h                                                         
*                                                                              
* Purpose:           Energy Quantization for VR15                            
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
#ifndef QENRGY15_H
#define QENRGY15_H

void VoxQuanEnergyVR15(float Energy, const float *cb, short size, unsigned short *index);
float VoxUnQuanEnergyVR15(const float *cb, short size, unsigned short index);

#endif /* QENRGY15_H */

