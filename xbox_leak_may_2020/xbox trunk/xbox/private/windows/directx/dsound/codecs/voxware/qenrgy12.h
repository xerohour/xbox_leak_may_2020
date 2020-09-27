/*******************************************************************************
*                        Voxware Proprietary Material                          *
*                        Copyright 1996, Voxware, Inc                          *
*                            All Rights Reserved                               *
*                                                                              *
*                       DISTRIBUTION PROHIBITED without                        *
*                      written authorization from Voxware                      *
*******************************************************************************/

/*******************************************************************************
* Filename:          QEnrgy12.h                                                         
*                                                                              
* Purpose:           Energy Quantization for VR12                            
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
#ifndef QENRGY12_H
#define QENRGY12_H

void VoxQuanEnergyVR12(float Energy, const float *cb, short size, unsigned short *index);
float VoxUnQuanEnergyVR12(const float *cb, short size, unsigned short index);

#endif /* QENRGY12_H */

