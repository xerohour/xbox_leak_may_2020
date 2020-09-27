/*******************************************************************************
*                        Voxware Proprietary Material                          *
*                        Copyright 1996, Voxware, Inc                          *
*                            All Rights Reserved                               *
*                                                                              *
*                       DISTRIBUTION PROHIBITED without                        *
*                      written authorization from Voxware                      *
*******************************************************************************/

/*******************************************************************************
* Filename:          QPvVR.c                                                         
*                                                                              
* Purpose:           Pv Quantization for VR series codecs                            
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
#include <stdlib.h>
#include <math.h>
#include "quanvr12.h"
#include "VoxRound.h"
#include "QPvVR12.h"

#include "xvocver.h"

/*******************************************************************************
* Function: VoxQuanPvVR
*
* Action:
*
* Input:
*
* Output:
*
* Globals:        none
*
* Return:
********************************************************************************
* Implementation/Detailed Description:
*
* References:
********************************************************************************
* Modifications:
*
* Comments:
*
* Concerns/TBD:
*******************************************************************************/

#define PV_STEP  (( PV_V_BOUND-PV_RESET )/ ((1<<PV_BITS_VR12_MIXED)-1))
#define PV_BIAS  0.25

unsigned short VoxQuanPvVR12(float pv)
{
  unsigned short index;

  index = (unsigned short) VoxPosRound((float)((pv-PV_RESET)*(1.0/PV_STEP)+PV_BIAS));
  
  return(index);
}

float VoxUnQuanPvVR12(unsigned short index)
{
   float Pv;

   Pv = (float)((PV_STEP*index)+PV_RESET);
   return(Pv);

}
