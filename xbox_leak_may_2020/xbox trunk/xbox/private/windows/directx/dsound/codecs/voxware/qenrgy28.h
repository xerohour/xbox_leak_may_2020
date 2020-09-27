/*******************************************************************************
*                        Voxware Proprietary Material                          *
*                        Copyright 1996, Voxware, Inc                          *
*                            All Rights Reserved                               *
*                                                                              *
*                       DISTRIBUTION PROHIBITED without                        *
*                      written authorization from Voxware                      *
*******************************************************************************/

/*******************************************************************************
* Filename:          QEnrgy28.h                                                         
*                                                                              
* Purpose:           Energy Quantization for RT28                            
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
#ifndef QENRGY28_H
#define QENRGY28_H

void VoxQuanEnergyRT28(float Energy, const float *cb, short size, unsigned short *index);
float VoxUnQuanEnergyRT28(const float *cb, short size, unsigned short index);

#endif /* QENRGY28_H */

