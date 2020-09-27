/*******************************************************************************
*                        Voxware Proprietary Material                          *
*                        Copyright 1996, Voxware, Inc                          *
*                            All Rights Reserved                               *
*                                                                              *
*                       DISTRIBUTION PROHIBITED without                        *
*                      written authorization from Voxware                      *
*******************************************************************************/

/*******************************************************************************
* Filename:       GetnHarm.h                                                            
*                                                                              
* Purpose:        Provide a consistent means of calculating the number of 
*                 harmonics.                                                             
*    
*                                                                              
* Author/Date:    Rob Zopf   02/01/97                                                           
********************************************************************************
* Modifications:                                                                 
*                                                                              
* Comments: the way that it is currently calculated is bogus, and should be 
*           changed, but it is the way it is currently calculated, and I want
*           it to be same for comparison. 
* 
* Concerns: 
*******************************************************************************/
#ifndef GETNHARM_H
#define GETNHARM_H

short getnHarm(float Pitch);
short getvHarm(float Pitch, float Pv);
float getPv(short iVHarm, short nHarm);
#endif
