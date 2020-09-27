/*******************************************************************************
*                        Voxware Proprietary Material                          *
*                        Copyright 1996, Voxware, Inc                          *
*                            All Rights Reserved                               *
*                                                                              *
*                       DISTRIBUTION PROHIBITED without                        *
*                      written authorization from Voxware                      *
*******************************************************************************/

/*******************************************************************************
* Filename:          QEnrgy29.h                                                         
*                                                                              
* Purpose:           Energy Quantization for RT29                            
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
#ifndef QENRGYRT29_H
#define QENRGYRT29_H

void VoxQuanEnergyRT29(float Energy, const float *cb, short size, unsigned short *index);
float VoxUnQuanEnergyRT29(const float *cb, short size, unsigned short index);

#endif /* QENRGYRT29_H */

