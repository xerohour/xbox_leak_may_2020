/******************************************************************************
*                        Voxware Proprietary Material
*                        Copyright 1997, Voxware, Inc
*                            All Rights Reserved
*
*                       DISTRIBUTION PROHIBITED without
*                      written authorization from Voxware
******************************************************************************/
 
/******************************************************************************
*
* Filename:     FindMax.c
*
* Purpose:      Find the maximum value of an array and return the index
*
* Functions:    FindMaxValueFlt()
*
* Author/Date:  Wei Wang
*
*******************************************************************************
*
* Modifications: 
*
* Comments:    
*
* Concerns:   
*
* $Header:   /export/phobos/pvcs/pvcsproj/voxlib.prj/FindMax.c_v   1.9   04 Mar 1998 11:48:30   weiwang  $
******************************************************************************/
#include <stdlib.h>
#include <assert.h>
#include "vLibVec.h"

#include "xvocver.h"


/*******************************************************************************
*
* Function:  FindMaxValueFlt()
*
* Action:    Finds the maximum value (and its location) of a floating 
*              point vector.
*
* Input:     *pfX  -> the vector to be scaled
*            iLength -> the length of the vector
*
* Output:    *piMaxIndex -> index of maximum value
*            *pfXmax     -> maximum value
*
* Globals:   none
*
* Return:    void
********************************************************************************
* Modifications: Version 2.0, Originated from Intel NSP Lib
*                Modified by Xiangdong Fu 12/11/96
*
*
* Concerns:    This version is an optimized version on Pentium or IEEE 
*              floating point format machine. It assumes interger
*              comparision is faster than floating point comparision. 
*              This version is 3 times faster than version  1.0 on Pentium.
*              It is proved to work on Sun as well, but not faster(or slower)
*
*              If choose pre-definition "IEEE", float and long of this platform
*              need to be 32 bits.
*
* Comments:    Input vector length >= 1
*
*******************************************************************************/
#if ((defined(WIN32) == 1) || (defined(IEEE) == 1))
/*-------------------------------------------------------------------
  VERSION 2.0
 
  This is pentium optimized code.  Although it works on some other 
    platforms such as the Sun, it makes assumptions about the 
    hardware representations of type float and type int, and we have 
    decided to use it only on the PC. It is therefore under the
    WIN32 compiler switch.
-------------------------------------------------------------------*/
void FindMaxValueFlt( const float *pfX, 
                      int         iLength, 
                      int         *piMaxIndex, 
                      float       *pfXmax
                    )
{
  int   i;
  long  *plElement;
  long   lMax;
  long   lMin;
  int   iIndexMax;
  int   iIndexMin;
  union dt
  {
    float f;
    long   l;
  } uMax;


  /* do error checking */
  assert(pfX != NULL && piMaxIndex != NULL && pfXmax != NULL);
  assert(iLength >= 1);

  /* do block comparison */
  plElement = (long *)pfX;
  lMin = ( lMax = plElement[0] );
  iIndexMax = iIndexMin = 0;
  for ( i = 1; ( ( lMax < 0 ) && ( ( i + 4 ) <= iLength ) ); i += 4 )
  {
    if ( plElement[i  ] > lMax ) {lMax = plElement[i  ];  iIndexMax = i;}
    if ( plElement[i  ] < lMin ) {lMin = plElement[i  ];  iIndexMin = i;}
    if ( plElement[i+1] > lMax ) {lMax = plElement[i+1];  iIndexMax = i+1;}
    if ( plElement[i+1] < lMin ) {lMin = plElement[i+1];  iIndexMin = i+1;}
    if ( plElement[i+2] > lMax ) {lMax = plElement[i+2];  iIndexMax = i+2;}
    if ( plElement[i+2] < lMin ) {lMin = plElement[i+2];  iIndexMin = i+2;}
    if ( plElement[i+3] > lMax ) {lMax = plElement[i+3];  iIndexMax = i+3;}
    if ( plElement[i+3] < lMin ) {lMin = plElement[i+3];  iIndexMin = i+3;}
  }
  for ( ; ( ( lMax < 0 ) && ( i < iLength ) ); i++ )
  {
    if ( plElement[i] > lMax ) {lMax = plElement[i]; iIndexMax = i;}
    if ( plElement[i] < lMin ) {lMin = plElement[i]; iIndexMin = i;}
  }
  for ( ; ( i + 4 ) <= iLength; i += 4 )
  {
    if ( plElement[i  ] > lMax ){ lMax = plElement[i  ];iIndexMax = i;}
    if ( plElement[i+1] > lMax ){ lMax = plElement[i+1];iIndexMax = i+1;}
    if ( plElement[i+2] > lMax ){ lMax = plElement[i+2];iIndexMax = i+2;}
    if ( plElement[i+3] > lMax ){ lMax = plElement[i+3];iIndexMax = i+3;}
  }
  for ( ; i < iLength; i++ ) 
    if ( plElement[i] > lMax ) {
       lMax = plElement[i];
       iIndexMax = i;
    }

  uMax.l = ( lMax >= 0 ) ? lMax : lMin;
  *piMaxIndex = ( lMax >= 0 ) ? iIndexMax : iIndexMin;
  *pfXmax = uMax.f;
}

#else /* end of VERSION 2.0 code */

/*-------------------------------------------------------------------
  VERSION 1.0

  This is the general purpose verion to be used.
-------------------------------------------------------------------*/
void FindMaxValueFlt( const float *pfX, 
                      int         iLength, 
                      int         *piMaxIndex,
                      float       *pfXmax
                    )
{
  int   i;
  int   iMaxIndex;
  float fXmax;
  float fX;

  /* error checking */
  assert(pfX != NULL && piMaxIndex != NULL && pfXmax != NULL);
  assert(iLength >= 1);

  /* find the max value */
  fXmax = *pfX++;
  iMaxIndex = 0;

  for (i = 1; i < iLength; i++) 
  {
    fX = *pfX++;
    if (fX > fXmax) 
    {
      fXmax = fX;
      iMaxIndex = i;
    }
  }
  
  *piMaxIndex = iMaxIndex;
  *pfXmax = fXmax;
}
#endif /* end of VERSION 1.0 code */

