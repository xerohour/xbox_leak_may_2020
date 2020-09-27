/*******************************************************************************
*                        Voxware Proprietary Material                          *
*                        Copyright 1996, Voxware, Inc                          *
*                            All Rights Reserved                               *
*                                                                              *
*                       DISTRIBUTION PROHIBITED without                        *
*                      written authorization from Voxware                      *
*******************************************************************************/

/*******************************************************************************
* Filename:       GetnHarm.c                                                             
*                                                                              
* Purpose:        Provide a consistent means of calculating the number of 
*                 harmonics.                                                             
*    
*                                                                              
* Author/Date:    Rob Zopf   02/01/97
********************************************************************************
* Modifications: Wei Wang 02/25/97, change all the cast through math library floor(). 
*                                                                              
* Comments: the way that it is currently calculated is bogus, and should be 
*           changed, but it is the way it is currently calculated, and I want
*           it to be same for comparison. 
* 
* Concerns: 
*******************************************************************************/

#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <limits.h>

#include "model.h"

#include "GetnHarm.h"

#include "xvocver.h"


short getnHarm(float Pitch)
{
   short nHarm;
   nHarm        = (short)floor(Pitch*0.5F*ARBITRARY_CUTOFF);   /* 255/(512/pitch) */
   return(nHarm);
}

short getvHarm(float Pitch, float Pv)
{
   short nHarm;
   nHarm        = (short)floor(Pitch*0.5F*ARBITRARY_CUTOFF);   /* 255/(512/pitch) */
   nHarm        = (short)floor((float)nHarm*Pv+0.5F);
   return(nHarm);
}

float getPv(short iVHarm, short nHarm)
{
  float Pv;

  if (nHarm <= 0) Pv = 0.0F;      /* should never happen */
  Pv=(float)(iVHarm)/(float)nHarm;

  if (Pv > 1.0F)   Pv = 1.0F;     /* should never happen too! */
  return Pv;
}
