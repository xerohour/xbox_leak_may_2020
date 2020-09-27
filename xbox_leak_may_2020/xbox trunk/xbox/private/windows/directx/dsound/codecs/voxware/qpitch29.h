/*******************************************************************************
*                        Voxware Proprietary Material                          *
*                        Copyright 1996, Voxware, Inc                          *
*                            All Rights Reserved                               *
*                                                                              *
*                       DISTRIBUTION PROHIBITED without                        *
*                      written authorization from Voxware                      *
*******************************************************************************/

/*******************************************************************************
* Filename:          QPitch29.h                                                         
*                                                                              
* Purpose:           Pitch Quantization for RT29                             
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
#ifndef QPITCH29_H
#define QPITCH29_H 

#define Q_PITCH_LEVEL           (1<<F0_BITS_29)
#define M_T0    ((float)(QUANT_MIN_PITCH_29-QUANT_MAX_PITCH_29)/(float)(Q_PITCH_LEVEL-1))

short VoxQuanPitchRT29(float pitch);
float VoxLutPitchRT29(short qP0);

#endif /* QPITCHBC_H */
 
