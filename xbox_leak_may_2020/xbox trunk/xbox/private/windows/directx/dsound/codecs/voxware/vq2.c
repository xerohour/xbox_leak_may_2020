/*******************************************************************************
*                        Voxware Proprietary Material
*                        Copyright 1996, Voxware, Inc
*                            All Rights Reserved
*
*                       DISTRIBUTION PROHIBITED without
*                      written authorization from Voxware
*******************************************************************************/
/*******************************************************************************
* File:         VQ2.c
*
* Description:  Encode a 2-dimensional input vector using a 2-D vector 
*                 quantizer with the straightforward mean squared error 
*                 (MSE) criterion 
*
* Author/date:  Original VQ.c code written by:    Wei Wang, July 22, 1994
*               Special Optimized version for 2-dimensional VQ written by: 
*                                   Juin-Hwey (Raymond) Chen, May 21, 1997
* $Header:   /export/phobos/pvcs/pvcsproj/voxlib.prj/VQ2.c_v   1.9   25 Mar 1998 14:04:44   weiwang  $ 
*******************************************************************************/
#include <stdlib.h>
#include <assert.h>

#include "vLibDef.h"
#include "vLibQuan.h"

#include "xvocver.h"

/*******************************************************************************
* Function:  Dim2VectorQuantize()
*
* Action:    Encode a 2-dimensional input vector using a 2-D vector quantizer 
*            with the straightforward mean squared error (MSE) criterion 
*
* INPUT:     pfVector:   --- 2-dimensional input vector
*            pfCodeBook: --- one-dimensional codebook array containing
*                             (2 * iEntries) elements. Codebook vectors
*                             are stored in a format of (row * column)
*                             i.e. dimention of component * entries of
*                             the codebook
*            iEntries:   --- codebook size (number of 2-D codevectors)
*
* OUTPUT:    None
*    
* RETURN:    Chosen index of the codebook
********************************************************************************
*
* Implementation:  There are 2 versions of the code.  The WIN32/IEEE version
*                    uses a float/int union to speed up comparisons.
*
*******************************************************************************/

#if ( defined(WIN32) || defined(IEEE) )

/*---------------------------------------------------------------------------
  WIN32/IEEE VERSION: The float/int union speeds up comparisons.
---------------------------------------------------------------------------*/
int Dim2VectorQuantize( const float *pfVector, 
                        const float *pfCodeBook, 
                        int          iEntries
                      )
{
   int   i;
   int   iIndex;
   float fTmp;
   union dt {
      float f;
      long   l;
   } uMinError, uError;

   assert( pfVector != NULL);
   assert( pfCodeBook != NULL);
   iIndex = 0;
   uMinError.f = VERY_LARGE_FLOAT;
  
   for (i = 0; i < iEntries; i++) 
   {
      fTmp = *pfCodeBook++ - pfVector[0];
      uError.f = fTmp * fTmp;
      fTmp = *pfCodeBook++ - pfVector[1];
      uError.f += fTmp * fTmp;

      if (uError.l < uMinError.l) 
      {
         uMinError.l = uError.l;
         iIndex = i;
      }
   }

   return( iIndex );
}

#else /* else not WIN32 */

/*---------------------------------------------------------------------------
  GENERAL PURPOSE VERSION: No float/int union.
---------------------------------------------------------------------------*/
int Dim2VectorQuantize( const float *pfVector,
                        const float *pfCodeBook,
                        int          iEntries
                      )
{
   int   i;
   int   iEntry;
   float fTmp;
   float fError;
   float fMinError;

   assert( pfVector != NULL );
   assert( pfCodeBook != NULL);

   iEntry = 0;
   fMinError = VERY_LARGE_FLOAT;
 
   for (i = 0; i < iEntries; i++)
   {
      fTmp    = *pfCodeBook++ - pfVector[0];
      fError  = fTmp*fTmp;
      fTmp    = *pfCodeBook++ - pfVector[1];
      fError += fTmp*fTmp;

      if (fError < fMinError) 
      {
         fMinError = fError;
         iEntry = i;
      }
   }

   return( iEntry );
}

#endif

/*******************************************************************************
* Function:  Dim2VectorLUT()
*
* Action:    Decode the codebook index of a 2-dimensional vector quantizer
*
* Input:     iIndex:     --- the index of the selected codevector
*            pfCodeBook: --- one-dimensional codebook array containing
*                             (2 * iEntries) elements
*            iEntries:   --- number of entries in the codebook
*
* Output:    pfVector:   --- decoded 2-dimensional VQ output vector
*
* Author/Date:  Juin-Hwey (Raymond) Chen, June 9, 1997
*
* NOTE: the function VQ2dec() works only for the case of 2-dimensional VQ
*******************************************************************************/
 
void Dim2VectorLUT( unsigned int  iIndex, 
                    const float  *pfCodeBook, 
                    int           iEntries,
                    float        *pfVector
                  )
{
   assert( pfCodeBook != NULL);
   assert( pfVector != NULL);
   assert( iIndex < (unsigned int) iEntries );

   if (iIndex >= (unsigned int) iEntries)
      iIndex = 0;

   pfCodeBook += (iIndex<<1);    /* increment pointer by 2*iIndex */
   *pfVector++ = *pfCodeBook++;
   *pfVector   = *pfCodeBook;
}



