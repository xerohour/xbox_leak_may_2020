/*******************************************************************************
*                        Voxware Proprietary Material                          *
*                        Copyright 1996, Voxware, Inc                          *
*                            All Rights Reserved                               *
*                                                                              *
*                       DISTRIBUTION PROHIBITED without                        *
*                      written authorization from Voxware                      *
*******************************************************************************/

/*******************************************************************************
* Filename:          QEnrgy.h                                                         
*                                                                              
* Purpose:           Energy Quantization for RT24                            
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
#ifndef QENRGY24_H
#define QENRGY24_H

void VoxQuanEnergy24(float Energy, const float *cb, short size, unsigned short *index);
float VoxUnQuanEnergy24(const float *cb, short size, unsigned short index);

#endif /* QENRGY24_H */

