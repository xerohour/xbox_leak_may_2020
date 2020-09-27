/*******************************************************************************
*                        Voxware Proprietary Material                          *
*                        Copyright 1996, Voxware, Inc                          *
*                            All Rights Reserved                               *
*                                                                              *
*                       DISTRIBUTION PROHIBITED without                        *
*                      written authorization from Voxware                      *
*******************************************************************************/

/*******************************************************************************
* Filename:          QPitch12.c                                                         
*                                                                              
* Purpose:           Pitch Quantization for VR12                             
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
#include <assert.h>

#include "quanvr12.h"
#include "QPitch12.h"

#include "xvocver.h"

/*******************************************************************************
* Function: VoxQuanPitchVR12
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
 
#define Q_PITCH_LEVEL           (1<<F0_BITS_VR12_VOICED)
#define M_T0      1.023868851    /* alpha = pow(10, log10(Pmax/Pmin)/(Level-1)) */

short VoxQuanPitchVR12(float pitch)
{
  short qP0;

  assert(pitch > 0);
  qP0 = (short)floor((log10((double)QUANT_MAX_PITCH_VR/(double)pitch)) / log10(M_T0)+0.5);
  if (qP0 >= Q_PITCH_LEVEL)   qP0 = Q_PITCH_LEVEL-1;
  if (qP0 < 0)  qP0 = 0;
  return qP0;
}


float VoxLutPitchVR12(short qP0)
{	
  float pitch;
  
  pitch = (float)((double)QUANT_MAX_PITCH_VR * pow(M_T0, (double)(-qP0)));
  
  if (pitch < (float)QUANT_MIN_PITCH_VR)  pitch = (float)QUANT_MIN_PITCH_VR;
  if (pitch > (float)QUANT_MAX_PITCH_VR)  pitch = (float)QUANT_MAX_PITCH_VR;

#if 1  /* WIN16 ?? */

/* if (486) */
  {
     if (pitch > 125.0)
        pitch*=0.5F;
  }
/* endif */

#endif
  
  return pitch;
}
