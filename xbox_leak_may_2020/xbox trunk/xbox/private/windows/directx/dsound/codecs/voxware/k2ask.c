/******************************************************************************
*
*                        Voxware Proprietary Material
*
*                        Copyright 1997, Voxware, Inc
*
*                            All Rights Reserved
*
*
*                       DISTRIBUTION PROHIBITED without
*
*                      written authorization from Voxware
*
******************************************************************************/
 
/******************************************************************************
*
* Filename:    K2ASK.c
*
* Purpose:    transform the reflection coefficients to acsin reflection coefficients.
*
* Functions: void K2ASK(const int iLPCorder, const float *pfk, float *pfASK)
*            void ASK2K(const int iLPCorder, const float *pfASK, float *pfk)
*
* Author/Date: Xiaoqin Sun   08/97
*
*******************************************************************************
*
* Modifications:  Wei Wang, clean up the code 11/97.
*
* Comments:
*
* Concerns:
*
* $Header:   /export/phobos/pvcs/pvcsproj/voxlib.prj/K2ASK.c_v   1.4   02 Mar 1998 17:40:58   weiwang  $
******************************************************************************/
#include <stdlib.h>
#include <assert.h>
#include <math.h>
#include "vLibTran.h"

#include "xvocver.h"

/******************************************************************************
*
* Function:  K2ASK()
*
* Action:    Convert reflection coefficients to Arcsin reflection coefficients.
*
* Input:     iLPCorder -- the order or the length of the coefficients.
*            pfk       -- reflection coefficients.
*            pfASK     -- the pointer for the output Arcsin reflection coefficients.
*
* Output:    pfASK     -- Arcsin reflection coefficients 
*
* Globals:   none
*
* Return:    void
*******************************************************************************
*
* Implementation/Detailed Description:
*
*
* References:
*******************************************************************************
*
* Modifications:
*
* Comments:
*
* Concerns/TBD:
******************************************************************************/
void K2ASK (int iLPCorder, const float *pfk, float *pfASK)
{
  assert(pfk != NULL && pfASK != NULL);

  while ((iLPCorder --) > 0)
    *pfASK++ = (float)(asin(*pfk++));
}


/******************************************************************************
*
* Function:  ASK2K()
*
* Action:    Convert Arcsin reflection coefficients to reflection coefficients.
*
* Input:     iLPCorder -- the order or the length of the coefficients.
*            pfASK     -- Arcsin reflection coefficients.
*            pfk       -- the pointer for the output reflection coefficients.
*
* Output:    pfk       -- reflection coefficients 
*
* Globals:   none
*
* Return:    void
*******************************************************************************
*
* Implementation/Detailed Description:
*
*
* References:
*******************************************************************************
*
* Modifications:
*
* Comments:
*
* Concerns/TBD:
******************************************************************************/

void ASK2K (int iLPCorder, const float *pfASK, float *pfk)
{
  assert(pfASK != NULL && pfk != NULL);
 
  while ((iLPCorder--) > 0)
    *pfk++ = (float)sin(*pfASK++);
}

