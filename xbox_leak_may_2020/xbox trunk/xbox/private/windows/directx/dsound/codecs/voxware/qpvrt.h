/*******************************************************************************
*                        Voxware Proprietary Material                          *
*                        Copyright 1996, Voxware, Inc                          *
*                            All Rights Reserved                               *
*                                                                              *
*                       DISTRIBUTION PROHIBITED without                        *
*                      written authorization from Voxware                      *
*******************************************************************************/

/*******************************************************************************
* Filename:          QPvRT.c                                                         
*                                                                              
* Purpose:           Pv Quantization for RT series codecs                            
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
#ifndef QPVRT_H
#define QPVRT_H

unsigned short VoxQuanPvRT(float pv, const float *cb, short size);
float VoxUnQuanPvRT(const float *cb, short size, unsigned short index);

#endif /* QPVRT_H */

