/*******************************************************************************
*                        Voxware Proprietary Material                          *
*                        Copyright 1996, Voxware, Inc                          *
*                            All Rights Reserved                               *
*                                                                              *
*                       DISTRIBUTION PROHIBITED without                        *
*                      written authorization from Voxware                      *
*******************************************************************************/

/*******************************************************************************
* Filename:     Compress.c                        
*                                                                              
* Purpose:      Scale and compress an array using fourth root compression.
*
* Functions:    ScaleAndCompress()
*
* Author/Date:  Bob Dunn   3/30/98
********************************************************************************
* Modifications:
*                                                                              
* Comments:                                                                    
* 
* Concerns:
*
* $Header:   /export/phobos/pvcs/r_and_d/archives/scaleable/pea/Compress.c_v   1.4   11 Apr 1998 15:25:02   bobd  $
*******************************************************************************/

#include "vLibMath.h"
#include "vLibVec.h"

#include "Compress.h"

#include "xvocver.h"

/******************************************************************************
*
* Function:   ScaleAndCompress()
*
* Action:     Scales and compress an array using fourth root compression.
*
* Input:      const float *pfIn      --> input array
*             int          iLength   --> length of input array
*             float        fMaxVal   --> inverse of scale factor (maximum value)
*
* Output:     float       *pfOut     --> output array
*
* Globals:    none
*
* Return:     none
*******************************************************************************
*
* Implementation/Detailed Description:
*
* References:
*******************************************************************************
*
* Modifications:
*
* Comments:    In place operation is supported.
*
* Concerns/TBD:
******************************************************************************/
void ScaleAndCompress( const float *pfIn, 
                       int          iLength, 
                       float        fMaxVal, 
                       float       *pfOut
                     ) 
{
   int   i;
   float fScale;
   
   /*------------------------------------------------------
     Compute the scale factor: 1/fMaxVal
   ------------------------------------------------------*/
   if (fMaxVal == 0.0F)
     fScale = 1.0F;
   else
     fScale = 1.0F/fMaxVal;

   /*------------------------------------------------------
     Scale 
   ------------------------------------------------------*/
   ScaleVector( (float*) pfIn, iLength, fScale, pfOut );

   /*------------------------------------------------------
     Compress with an approximation to the fourth root
   ------------------------------------------------------*/
   for (i = 0; i < iLength; i++)
      pfOut[i] = fQuadRoot( pfOut[i] ); 
 
} /* ScaleAndCompress() */
 
