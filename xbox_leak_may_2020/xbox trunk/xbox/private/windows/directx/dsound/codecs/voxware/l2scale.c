/*******************************************************************************
*                        Voxware Proprietary Material                          *
*                        Copyright 1996, Voxware, Inc                          *
*                            All Rights Reserved                               *
*                                                                              *
*                       DISTRIBUTION PROHIBITED without                        *
*                      written authorization from Voxware                      *
*******************************************************************************/

/*******************************************************************************
* Filename:       L2Scale.c
*                                                                              
* Purpose:        Perform an L2 energy scaling
*                                                                              
* Functions:      L2Scale() 
*                        
* Author/Date:    Bob Dunn 3/97
********************************************************************************
* $Header:   /export/phobos/pvcs/pvcsproj/voxlib.prj/l2scale.c_v   1.7   02 Mar 1998 17:41:04   weiwang  $
*******************************************************************************/

/******************************************************************************
*
* Function:  L2Scale()
*
* Action:    Perform an L2 scaling of a vector.
*
* Input:     float *pfX       -> the vector to be scaled
*            int iLength      -> the length of the vector
*            float fEnergySqr -> the desired output energy of the vector
*
* Output:    float *pfX       -> the scaled vector
*
* Globals:   none
*
* Return:    void
*******************************************************************************
*
* Modifications:
*
* Comments:
*
* Concerns/TBD:
******************************************************************************/
#include <assert.h>
#include <math.h>

#include "vLibVec.h"

#include "xvocver.h"

void L2Scale(float *pfX, int iLength, float fEnergySqr)
{
  float fTemp;

  assert( pfX );
  assert( iLength >= 0 );

  /*------------------------------------------------------
    Compute the energy in the vector
  ------------------------------------------------------*/
  fTemp = DotProd(pfX, pfX, iLength);

  /*------------------------------------------------------
    If the energy (fTemp) is non-zero then compute the 
      scale factor and scale the vector, Otherwise,
      avoid a divide by zero and don't scale.
  ------------------------------------------------------*/
  if (fTemp>0.0)
  {
    fTemp=(float)(sqrt(fEnergySqr/fTemp));
    ScaleVector( pfX, iLength, fTemp, pfX );
  }
}

