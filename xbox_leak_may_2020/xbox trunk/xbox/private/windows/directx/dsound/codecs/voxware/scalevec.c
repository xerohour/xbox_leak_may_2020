/*******************************************************************************
*                        Voxware Proprietary Material                          *
*                        Copyright 1996, Voxware, Inc                          *
*                            All Rights Reserved                               *
*                                                                              *
*                       DISTRIBUTION PROHIBITED without                        *
*                      written authorization from Voxware                      *
*******************************************************************************/

/*******************************************************************************
* Filename:       ScaleVec.c
*                                                                              
* Purpose:        Multiply a vector by a scalar.
*                                                                              
* Functions:      ScaleVector() 
*                        
* Author/Date:    Bob Dunn 2/24/98
********************************************************************************
* $Header:   /export/phobos/pvcs/pvcsproj/voxlib.prj/ScaleVec.c_v   1.2   25 Mar 1998 14:31:18   weiwang  $
*******************************************************************************/
#include <stdlib.h>
#include <assert.h>

#include "vLibVec.h"

#include "xvocver.h"

/******************************************************************************
*
* Function:  ScaleVector()
*
* Action:    Multiply a vector by a scalar.
*
* Input:     float *pfIn      -> the vector to be scaled
*            int    iLength   -> the length of the vector
*            float  fScalar   -> the desired scale factor
*
* Output:    float *pfOut     -> the scaled vector
*
* Globals:   none
*
* Return:    void
*******************************************************************************
*
* Modifications:
*
* Comments:  In place and out of place operation are supported.
*
* Concerns/TBD:
******************************************************************************/
void ScaleVector( float *pfIn, 
                  int    iLength, 
                  float  fScalar, 
                  float *pfOut 
                )
{
  /*------------------------------------------------------
    Test for bad input.
  ------------------------------------------------------*/
  assert( pfIn != NULL );
  assert( pfOut != NULL );
  assert( iLength >= 0 );

  /*------------------------------------------------------
    Scale the vector.
  ------------------------------------------------------*/
  while ((iLength--)>0)
    *pfOut++ = *pfIn++ * fScalar;
}

