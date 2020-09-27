/*******************************************************************************
*                        Voxware Proprietary Material                          *
*                        Copyright 1996, Voxware, Inc                          *
*                            All Rights Reserved                               *
*                                                                              *
*                       DISTRIBUTION PROHIBITED without                        *
*                      written authorization from Voxware                      *
*******************************************************************************/
/*******************************************************************************
* Filename:  SQ.c
*
* Purpose:   Scalar quantization
*
* Date:
* Author:
*
* $Header:   /export/phobos/pvcs/pvcsproj/voxlib.prj/sq.c_v   1.9   08 Apr 1998 17:47:36   weiwang  $
*******************************************************************************/
#include <stdlib.h>
#include <assert.h>

#include "vLibQuan.h"

#include "xvocver.h"

/*******************************************************************************
* Function:  ScalarQuant()
*
* Action:    Find index into codebook of the value closest to the
*              input variable.
*
* Input:     fValue:------- Input variable
*            pfCodeBook:--- Codebook valiables
*            iLength:------ Number of valiable in the codebook
*
* Output:    none
*
* Return:    index--------- Index to codebook
********************************************************************************
*
* Modifications:
*
* Comments:
*
* Concerns/TBD:
*******************************************************************************/
int ScalarQuant( float         fValue, 
                 const float  *pfCodeBook, 
                 int           iLength 
               )
{
   int   i;
   int   iIndex;
   float fMinErr;
   float fErr;

   assert( pfCodeBook != NULL);
   assert( iLength > 0);

   iIndex   = 0;
   fMinErr  = fValue-pfCodeBook[0];
   fMinErr *= fMinErr;

   for (i=1;i<iLength;i++)
   {
      fErr  = fValue-pfCodeBook[i];
      fErr *= fErr;
      if (fErr < fMinErr)
      {
         fMinErr = fErr;
         iIndex = i;
      }
   }

   return( iIndex );
}

/*******************************************************************************
* Function:  ScalarLUT
*
* Action:    Dequantisation.  Return the peoper valiable according to the 
*              index of the codebook
*
* Input:     pfCodeBook:------ Codebook valiables
*            iLength:--------- Number of valiable in the codebook   
*            index------------ Index to codebook
*
* Output:    none
*
* Return:    pfCodeBook[index]----chosen variable in the codebook
*******************************************************************************/
float ScalarLUT( const float *pfCodeBook, 
                 int          iLength, 
                 int          iIndex
               )
{
   assert( pfCodeBook != NULL);
   assert( iLength>0 );
   assert( iIndex<iLength );
   assert( iIndex>=0);

   if (iIndex >= iLength)
      iIndex = 0;
   return (pfCodeBook[iIndex]);
} 


