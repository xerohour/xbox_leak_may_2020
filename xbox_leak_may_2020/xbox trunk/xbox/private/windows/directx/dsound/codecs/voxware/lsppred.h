/*******************************************************************************
*                        Voxware Proprietary Material                          *
*                        Copyright 1996, Voxware, Inc                          *
*                            All Rights Reserved                               *
*                                                                              *
*                       DISTRIBUTION PROHIBITED without                        *
*                      written authorization from Voxware                      *
*******************************************************************************/

/*******************************************************************************
* Filename:          LspPred.h                                                         
*                                                                              
* Purpose:           Quantize lsfs using scalar quantization                                                          
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
#ifndef LSPPRED_H
#define LSPPRED_H

void LspPredQuan(float *lsf, unsigned short *index, const float **cb, float qPitch);
void LspPredUnQuan(float *lsf, unsigned short *index, const float **cb, float qPitch);

#endif /* LSPPRED_H */
 
