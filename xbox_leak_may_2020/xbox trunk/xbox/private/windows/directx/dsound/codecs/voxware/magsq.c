/*******************************************************************************
*                        Voxware Proprietary Material                          *
*                        Copyright 1996, Voxware, Inc                          *
*                            All Rights Reserved                               *
*                                                                              *
*                       DISTRIBUTION PROHIBITED without                        *
*                      written authorization from Voxware                      *
*******************************************************************************/

/*******************************************************************************
* Filename:          MagSq.c
*                                                                              
* Purpose:           Compute vector magnitude squared.
*                                                                              
* Functions:         MagSq()
*                                                                              
* Author/Date:       Bob Dunn   4/7/97
********************************************************************************
* $Header:   /export/phobos/pvcs/pvcsproj/voxlib.prj/magsq.c_v   1.9   07 Apr 1998 17:56:56   weiwang  $
*******************************************************************************/
#include <stdlib.h>
#include <assert.h>

#include "vLibVec.h"

#include "xvocver.h"

/******************************************************************************
*
* Function: MagSq()
*
* Action:   Compute magnitude squared of a complex vector
*
* Input:    const float *pcfRe    -> real part of complex vector
*           const float *pcfIm    -> imaginary part of complex vector
*           int    iLength        -> length of vector
*
* Output:   float *pfMagSq        -> magnitude squared of the vector
*
* Globals:  none
*
* Return:   void
*******************************************************************************
*
* Modifications:
*
* Comments:
*
* Concerns/TBD:
******************************************************************************/
void MagSq(const float *pcfRe, const float *pcfIm, int iLength, float *pfMagSq)
{

  assert( iLength > 0 );
  assert( pcfRe != NULL);
  assert( pcfIm != NULL);
  assert( pfMagSq != NULL);

  for (; iLength > 0; iLength--, pcfRe++, pcfIm++)
    *pfMagSq++ = (*pcfRe)*(*pcfRe) + (*pcfIm)*(*pcfIm);
}


/******************************************************************************
*
* Function: MagSqIL()
*
* Action:   Compute magnitude squared of a inter-leave formatted complex vector
*
* Input:    const float *pcfVec    -> inter-leave complex vector
*           int    iLength        -> length of vector
*
* Output:   float *pfMagSq        -> magnitude squared of the vector
*
* Globals:  none
*
* Return:   void
*******************************************************************************
*
* Modifications:
*
* Comments:
*
* Concerns/TBD:
******************************************************************************/
void MagSqIL(const float *pcfVec, int iLength, float *pfMagSq)
{

  assert( iLength > 0 );
  assert( pcfVec != NULL);
  assert( pfMagSq != NULL);

  for (; iLength > 0; iLength--, pcfVec+=2)
    *pfMagSq++ = pcfVec[0]*pcfVec[0] + pcfVec[1]*pcfVec[1];
}
