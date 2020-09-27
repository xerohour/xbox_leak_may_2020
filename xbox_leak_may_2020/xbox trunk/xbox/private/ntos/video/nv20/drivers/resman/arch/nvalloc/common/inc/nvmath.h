/***************************************************************************\
|*                                                                           *|
|*       Copyright 1993-1998 NVIDIA, Corporation.  All rights reserved.      *|
|*                                                                           *|
|*     NOTICE TO USER:   The source code  is copyrighted under  U.S. and     *|
|*     international laws.  NVIDIA, Corp. of Sunnyvale,  California owns     *|
|*     copyrights, patents, and has design patents pending on the design     *|
|*     and  interface  of the NV chips.   Users and  possessors  of this     *|
|*     source code are hereby granted a nonexclusive, royalty-free copy-     *|
|*     right  and design patent license  to use this code  in individual     *|
|*     and commercial software.                                              *|
|*                                                                           *|
|*     Any use of this source code must include,  in the user documenta-     *|
|*     tion and  internal comments to the code,  notices to the end user     *|
|*     as follows:                                                           *|
|*                                                                           *|
|*     Copyright  1993-1998  NVIDIA,  Corporation.   NVIDIA  has  design     *|
|*     patents and patents pending in the U.S. and foreign countries.        *|
|*                                                                           *|
|*     NVIDIA, CORPORATION MAKES NO REPRESENTATION ABOUT THE SUITABILITY     *|
|*     OF THIS SOURCE CODE FOR ANY PURPOSE. IT IS PROVIDED "AS IS" WITH-     *|
|*     OUT EXPRESS OR IMPLIED WARRANTY OF ANY KIND.  NVIDIA, CORPORATION     *|
|*     DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOURCE CODE, INCLUD-     *|
|*     ING ALL IMPLIED WARRANTIES  OF MERCHANTABILITY  AND FITNESS FOR A     *|
|*     PARTICULAR  PURPOSE.  IN NO EVENT  SHALL NVIDIA,  CORPORATION  BE     *|
|*     LIABLE FOR ANY SPECIAL,  INDIRECT,  INCIDENTAL,  OR CONSEQUENTIAL     *|
|*     DAMAGES, OR ANY DAMAGES  WHATSOEVER  RESULTING  FROM LOSS OF USE,     *|
|*     DATA OR PROFITS,  WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR     *|
|*     OTHER TORTIOUS ACTION,  ARISING OUT OF OR IN CONNECTION  WITH THE     *|
|*     USE OR PERFORMANCE OF THIS SOURCE CODE.                               *|
|*                                                                           *|
|*     RESTRICTED RIGHTS LEGEND:  Use, duplication, or disclosure by the     *|
|*     Government is subject  to restrictions  as set forth  in subpara-     *|
|*     graph (c) (1) (ii) of the Rights  in Technical Data  and Computer     *|
|*     Software  clause  at DFARS  52.227-7013 and in similar clauses in     *|
|*     the FAR and NASA FAR Supplement.                                      *|
|*                                                                           *|
\***************************************************************************/

/*
* nvmath.h
*
* OS-independent math utility functions implemented
*
*/

#if !defined _NVMATH_H_
#define _NVMATH_H_

#include <math.h>


// most functions (sqrt, sin.. etc) can be used by turning
// intrinsic functions on (option /Oi)
// see... http://msdn.microsoft.com/library/devprods/vs6/visualc/vccore/_core_.2f.oi.htm
// not-true intrinsic functions are implemented here...

// /Og messes up the .NMS
#pragma optimize ("g", on)
// not required if i use /Oi
// #pragma intrinsic (exp, log)

__inline double r0_pow (double a, double u)
{
    if (a == 0.0)
        return 0.0;
    else if (u == 0.0)
        return 1.0;
    else
        return (exp(u*log(a)));
}

__inline double r0_sinh(double a)
{
    return (0.5*(exp(a)-exp(-a)));
}

static __inline long FTOL(float ftmp1)
{
    long lTemp1;

    __asm
    {
        fld ftmp1
        fistp lTemp1
    }
    return lTemp1;
}

static __inline long DTOL(double ftmp1)
{
    long lTemp1;

    __asm
    {
        fld ftmp1
        fistp lTemp1
    }
    return lTemp1;
}


#pragma optimize ("g", off)

#define pow(x,y)    r0_pow ((x),(y))
#define sinh(x)     r0_sinh((x))

#endif