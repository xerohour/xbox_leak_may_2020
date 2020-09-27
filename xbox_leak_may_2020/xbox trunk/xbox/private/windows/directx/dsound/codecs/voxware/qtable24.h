/*******************************************************************************
*                        Voxware Proprietary Material                          *
*                        Copyright 1996, Voxware, Inc                          *
*                            All Rights Reserved                               *
*                                                                              *
*                       DISTRIBUTION PROHIBITED without                        *
*                      written authorization from Voxware                      *
*******************************************************************************/

/*******************************************************************************
* Filename:          QTable24.h                                                         
*                                                                              
* Purpose:           Quantization table setup for RT24                             
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
#ifndef QTABLE24_H
#define QTABLE24_H

const int* voxQuanGetLevels24(void);
const float* voxQuanGetPvTable24(void);
const float* voxQuanGetEnergyTable24(void);
unsigned short voxQuanFillLsfTable24(const float ***phLsfTable);

#endif /* QTABLE24_H */
 
