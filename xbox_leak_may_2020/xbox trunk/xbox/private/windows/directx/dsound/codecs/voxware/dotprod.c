/*******************************************************************************
*                        Voxware Proprietary Material                          *
*                        Copyright 1996, Voxware, Inc                          *
*                            All Rights Reserved                               *
*                                                                              *
*                       DISTRIBUTION PROHIBITED without                        *
*                      written authorization from Voxware                      *
*******************************************************************************/

/*******************************************************************************
* Filename:       DotProd.c
*
* Purpose:        Calculate the inner product of two vectors
*
* Functions:      float VoxDotProd(const float*, const float *, const int)
*
* Author/Date:    Xiangdong Fu   12/4/96
********************************************************************************
* $Header:   /export/phobos/pvcs/pvcsproj/voxlib.prj/dotprod.c_v   1.12   25 Mar 1998 14:04:04   weiwang  $
*******************************************************************************/

/******************************************************************************
*
* Function:  DotProd()
*
* Action:    Calculate the inner product of two vector
*
* Input:     const float *pfVecA  -> 1st input vector
*            const float *pfVecA  -> 2nd input vector
*            int iLength          -> the length of input vectors
*
* Globals:   none
*
* Return:    float fDotProduct    -> the inner product
********************************************************************************
*
* Comments:   Two versions are available for this function. 
*             1. General C level Version
*             2. Optimized Pentium specific version
*             Turn on the WIN32 switch in "codec.h" to use the P5 version.
* Concerns:
*
********************************************************************************
* Modifications:                                                               
*******************************************************************************/
#include <stdlib.h>
#include <assert.h>

#include "vLibVec.h"

#include "xvocver.h"

#ifdef _WIN64
#undef WIN32
#endif


#ifndef WIN32
/*------------------------------------------------------------------------
  This is the general version of DotProd()
------------------------------------------------------------------------*/
float DotProd(const float *pfVecA, const float *pfVecB, int iLength)
{
   float fDotProduct = 0.0F;

   assert( iLength >= 0 );
   assert( pfVecA != NULL);
   assert( pfVecB != NULL);

   while ((iLength--)>0)
      fDotProduct += (*pfVecA++) * (*pfVecB++);

   return (fDotProduct);
}

#else /* end of general version of dotProd() */

/*------------------------------------------------------------------------
  This is the Pentium specific version of dotProd() which is 
    written in assembly code.
------------------------------------------------------------------------*/
#pragma warning(disable:4035)
float DotProd(const float* pfVecA, const float* pfVecB,  int iLength)
{
   _asm{
     
      mov ebx, pfVecA;
      mov eax, iLength;

      mov edx, pfVecB;
      cmp eax, 6; 

      fldz;
      fld st(0);

      jnge loop12;

loop1:
      faddp st(1), st(0);         

      fld DWORD PTR[ebx];

      fmul DWORD PTR[edx];

      fld DWORD PTR[ebx+4];

      fmul DWORD PTR[edx+4];

      fld DWORD PTR[ebx+8];

      fmul DWORD PTR[edx+8];

      fld DWORD PTR[ebx+12];

      fmul DWORD PTR[edx+12];

      fld DWORD PTR[ebx+16];

      fmul DWORD PTR[edx+16];

      fld DWORD PTR[ebx+20];

      fmul DWORD PTR[edx+20];
      fxch st(5);

      faddp st(3),st(0);

      faddp st(3),st(0);

      faddp st(3),st(0);

      faddp st(3),st(0);

      sub eax, 6;
      add edx ,24;

      faddp st(1),st(0);

      add ebx, 24;
      cmp eax, 6;
      
      jge loop1;
loop12:
      test eax, eax;

      jz  exitdot;
      

loop2:         
      faddp st(1),st(0);

      fld DWORD PTR[ebx];
      fmul DWORD PTR[edx];

      add ebx, 4;
      add edx, 4;

      dec eax;
      jnz loop2;
exitdot:
      faddp st(1), st(0);


   }
}
#endif /* WIN32 */

#ifdef _WIN64
#define WIN32
#endif

