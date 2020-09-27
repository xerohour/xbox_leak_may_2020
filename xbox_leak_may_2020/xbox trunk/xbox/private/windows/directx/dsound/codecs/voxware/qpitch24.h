/*******************************************************************************
*                        Voxware Proprietary Material                          *
*                        Copyright 1996, Voxware, Inc                          *
*                            All Rights Reserved                               *
*                                                                              *
*                       DISTRIBUTION PROHIBITED without                        *
*                      written authorization from Voxware                      *
*******************************************************************************/

/*******************************************************************************
* Filename:          QPitch24.h                                                         
*                                                                              
* Purpose:           Pitch Quantization for RT24                             
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
#ifndef QPITCH24_H
#define QPITCH24_H

short VoxQuanPitch24(float pitch);
float VoxLutPitch24(short qP0);

#endif /* QPITCH24_H */
 
