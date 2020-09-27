/*******************************************************************************
*                        Voxware Proprietary Material
*                        Copyright 1997, Voxware, Inc
*                            All Rights Reserved
*
*                       DISTRIBUTION PROHIBITED without
*                      written authorization from Voxware
*******************************************************************************/
 
/*******************************************************************************
*
* Filename:    DeMltplx.c
*
* Purpose:     Demultiplex the real and imaginary part of the spectrum
*
* Functions:   VoxDeMultiplex()
*
* Author/Date: Bob Dunn  1/16/98
*
********************************************************************************
*
* Modifications:  
*
* Comments: 
*
* Concerns: 
*
* $Header:   /export/phobos/pvcs/pvcsproj/voxlib.prj/DeMltplx.c_v   1.3   02 Mar 1998 15:35:42   weiwang  $
*******************************************************************************/

#include "vLibTran.h"

#include "xvocver.h"

/*******************************************************************************
*
* Function:  VoxDeMultiplex()
*
* Action:    Demultiplex the real and imaginary part of the spectrum
*
* Input:     const float *pfSW    --> interleaved spectrum
*            int          iLength --> length of output arrays
*
* Output:    float *pfRE          --> real part of spectrum
*            float *pfIM          --> imaginary part of spectrum
*
* Globals:   none
*
* Return:    void
********************************************************************************
*
* Modifications:
*
* Comments:       The length of the output array is left to the user to 
*                   define, since they may or may not have room for the
*                   point at PI.
*
* Concerns/TBD:   WARNING:  In order to save memory it is possible to 
*                   demultiplex the real part "in place".  That is, it
*                   is possible that pfSW=pfRE!!!  It is critical 
*                   that the order of operations in this function 
*                   maintain compatability with "in place" operation
*                   on the real part.
*******************************************************************************/

void VoxDemultiplex( const float *pfSW,
                     float       *pfRE,
                     float       *pfIM,
                     int          iLength
                   )
{
   while ((iLength--) > 0)
   {
      *pfRE++ = *pfSW++;
      *pfIM++ = *pfSW++;
   }
}

