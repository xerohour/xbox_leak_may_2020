/******************************************************************************
*                        Voxware Proprietary Material
*                        Copyright 1996, Voxware, Inc
*                            All Rights Reserved
*
*                       DISTRIBUTION PROHIBITED without
*                      written authorization from Voxware
******************************************************************************/
 
/******************************************************************************
*
* Filename:        VoxQR.c
*
* Purpose:         Approximate fourth root.
*
* Functions:
*
* Author/Date:     Bob Dunn 1/9/87
*
*******************************************************************************
*
* Modifications:
*
* Comments:
*
*     WARNING: This code assumes that the input is between 0 and 1.
*              The output is meaningless for inputs greater than 1.
*              If the input is negative an infinite loop results.
*
* Concerns:
*
* $Header:   /export/phobos/pvcs/pvcsproj/voxlib.prj/VoxQR.c_v   1.4   02 Mar 1998 16:25:38   weiwang  $
******************************************************************************/
#include <assert.h>
 
#include "vLibMath.h"

#include "xvocver.h"

/*----------------------------------------------------
  Constants to approximate 4th root function in
    the region (0.1 <= x <= 1.0):

    y = x^(1/4) = (approx) (QRA*x*x + QRB*x + QRC)
----------------------------------------------------*/
#define QRA      -0.48015624485737840033F
#define QRB       1.00009717769371908602F
#define QRC       0.46449154220907556967F

/*----------------------------------------------------
  A constant to use for scaling:

   INV_QR10 = 1.0/(10.0^(1/4))
----------------------------------------------------*/
#define SCALE_INIT    1.0F
#define CONST_SCALE   10.0F
#define INV_SCALE     0.1F
#define INV_QR_SCALE  0.56234132519034907283F

/*----------------------------------------------------
  The LIMITING_FACTOR is used to control the maximum
    number of iterations in the while loop that scales
    an input which is less than 0.1 (but greater than
    0.0).  If N is the maximum number of iterations
    then LIMITING_FACTOR should be set to:

    LIMITING_FACTOR = 1.0x10.0^(-(N+1))
  
    This effectively limits the dynamic range of the 
    input to N+1 orders of magnitude.
----------------------------------------------------*/
#define LIMITING_FACTOR  1.0e-7F    /* N=6 for 7 orders of magnitude */

/******************************************************************************
*
* function:  float fQuadRoot(float fXval)
*
* Action:    approximation fourth root.
*
* Input:     fXvale -- input number 
*
* Output:    same as return
*
* Globals:   none
*
* Return:    approximation to fourth root of fXval
*
*******************************************************************************
*
* Implementation/Detailed Description: 
*
* References:
*******************************************************************************
*
* Modifications:
*
* Comments:    
*
* Concerns/TBD: fXvale must be smaller than 1.
******************************************************************************/

float fQuadRoot(float fXval)
{
  float fScale;
  float fout;

  /*---------------------------------------------
    Assert for input between 0.0 and 1.0
  ---------------------------------------------*/
  assert( fXval >= 0.0F );
  assert( fXval <= 1.0F );

  /*---------------------------------------------
    Scale fXval so that it is between 0.1 and 1.0.
  ---------------------------------------------*/
  fScale = SCALE_INIT;
  fXval += LIMITING_FACTOR; /* limit number of iterations in the while loop */
  while (fXval < INV_SCALE)
    {
      fXval  *= CONST_SCALE;
      fScale *= INV_QR_SCALE;
    }

  /*---------------------------------------------
    Quadratic approximation to 4th root
  ---------------------------------------------*/
  fout  = QRA*fXval+QRB;
  fout  = fXval*fout + QRC;
  fout *= fScale;

  return fout;
}
