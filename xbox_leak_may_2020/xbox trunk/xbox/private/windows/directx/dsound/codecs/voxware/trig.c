/*******************************************************************************
*                        Voxware Proprietary Material                          *
*                        Copyright 1996, Voxware, Inc                          *
*                            All Rights Reserved                               *
*                                                                              *
*                       DISTRIBUTION PROHIBITED without                        *
*                      written authorization from Voxware                      *
*******************************************************************************/

/*******************************************************************************
* Filename:     trig.c                                                                
*                                                                              
* Purpose:      Provide a global pointer to a sin and a cos table. 
*                                                                              
*                                                                
*                                                                              
* Author/Date:  Rob Zopf                                                               
********************************************************************************
* Modifications:
*                                                
* Comments:                                                                    
* 
* Concerns:
*
* 
*******************************************************************************/
#include "trig.h"

#include "xvocver.h"

const float trigTBL[] = 
{
#include "trigtbl.h"
};
 
const float *sinTBL = trigTBL;
const float *cosTBL = trigTBL + TRIG_SIZE/4;
