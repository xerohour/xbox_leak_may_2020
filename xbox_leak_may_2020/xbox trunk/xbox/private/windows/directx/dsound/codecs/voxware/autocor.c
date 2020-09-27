/*******************************************************************************
*                        Voxware Proprietary Material                          *
*                        Copyright 1996, Voxware, Inc                          *
*                            All Rights Reserved                               *
*                                                                              *
*                       DISTRIBUTION PROHIBITED without                        *
*                      written authorization from Voxware                      *
*******************************************************************************/
 
/*******************************************************************************
* Filename:       AutoCor.c
*
* Purpose:        Compute autocorrelation sequence
*
* Functions:      AutoCor()
*
* Author/Date:
********************************************************************************
* $Header:   /export/phobos/pvcs/pvcsproj/voxlib.prj/autocor.c_v   1.10   02 Mar 1998 17:41:02   weiwang  $
*******************************************************************************/

/*******************************************************************************
*
* Function:  AutoCor()
*
* Action:    Compute autocorrelation sequence.
*
* Input:     int          iOrder   -> number of autocorrelation samples to 
*                                       compute
*            const float *pcfVecIn -> input sequence
*            int          iLength  -> length of input sequence
*
* Output:    float *pfVecOut       -> the scaled vector
*
* Globals:   none
*
* Return:    void
********************************************************************************
*
* Modifications:
*
* Comments:
*
* Concerns/TBD:
*******************************************************************************/
#include <assert.h>

#include "vLibVec.h"

#include "xvocver.h"

void AutoCor( int iOrder, const float *pcfVecIn, int iLength, float *pfVecOut)
{
    const float *pcfData;

    assert( iOrder > 0 );
    assert( pcfVecIn );
    assert( pfVecOut );

    for(pcfData=pcfVecIn;iOrder > 0;pcfData++, iLength--, iOrder--) 
       *pfVecOut++= DotProd(pcfVecIn, pcfData, iLength);
}

