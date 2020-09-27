/*******************************************************************************
*                        Voxware Proprietary Material                          *
*                        Copyright 1996, Voxware Inc.                          *
*                        All Rights Reserved                                   *
*                                                                              *
*                        DISTRIBUTION PROHIBITED without                       *
*                        written authorization from Voxware.                   *
*******************************************************************************/
/******************************************************************************
* File:         PeakPick.c
*
* Description:  Pick peaks from a segment of an array of floats
*
* Author:       
*
* $Header:   /export/phobos/pvcs/pvcsproj/voxlib.prj/PeakPick.c_v   1.8   25 Mar 1998 14:04:26   weiwang  $ 
******************************************************************************/
#include <stdlib.h>
#include <assert.h>

#include "vLibSpch.h"

#include "xvocver.h"

/*******************************************************************************
* FUNCTION    : PeakPick()
*
* PURPOSE     : Pick peaks from a segment of an array of floats
*
* INPUT
*      pfMag      - segment of an array of floating point magintudes
*      iStart     - starting index of the segment
*      iStop      - ending index of the segment
*      iMaxPeaks  - maximum number of peaks to be picked
*
* OUTPUT
*      piPeakFreq - indices of peaks
*      piNPeaks   - number of peaks
*
* RETURN          None    
*******************************************************************************/

void PeakPick( const float *pfMag, 
               int          iStart,
               int          iStop,
               int          iMaxPeaks,
               int         *piPeakFreq,
               int         *piNPeaks
             )             
{
   int    i;
   int    iNPeaks;
  
   assert( pfMag != NULL);
   assert( piPeakFreq != NULL);
   assert( piNPeaks != NULL);

   iNPeaks = 0;
   i=iStart+1;
   iStop--;    /* the ending test index is 1 less than the end of the array */

   while (i<iStop)
   {
      /*----------------------------------------------------------
        A peak is defined as:

           (pfMag[i]>=pfMag[i-1]) && (pfMag[i]>pfMag[i+1])

        (There are other valid ways to define a peak but this
         one has been chosen.)
      ----------------------------------------------------------*/
      if ( (pfMag[i]>=pfMag[i-1]) && (pfMag[i]>pfMag[i+1]) )
      {
         *piPeakFreq++ = i;
         i++;                     /* (adjacent peaks are not possible)   */
         iNPeaks++;
         if (iNPeaks==iMaxPeaks)  /* If the maximum number of peaks has   */
           break;                 /*   been found, then stop the search.  */
      }
      i++;
   }

   *piNPeaks = iNPeaks;
}


