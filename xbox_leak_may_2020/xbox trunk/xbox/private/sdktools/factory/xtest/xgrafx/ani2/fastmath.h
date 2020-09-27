/*--
Copyright (c) 1999 - 2000 Microsoft Corporation - Xbox SDK Framework

Module Name:

    FastMath.h

Abstract:

    General math support including fast replacements for the standard math
    library. These fast replacements use SSE/SIMD instructions.

Revision History:

	7/1/2000	added fast_acos and fast_asin for SSE.
	7/3/2000	Fixed fast_log10 and fast_inversesqrt.
	7/4/2000	Fixed fast_atan2 bug.
	7/5/2000	improved quality of fast_acos and fast_asin.
	7/6/2000	improved algorithm of fast_log.
	8/1/2000	implemented hypot.
	8/8/2000	implemented fast_ceil and fast_floor.
	8/21/2000	implemented 3 fast hyperbolic functions.


TODO:
	Fix up arc-tangent quality.

--*/

#if !defined(SDK_HEADER_FASTMATH)
#define SDK_HEADER_FASTMATH


#if defined (_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#include "xmmintrin.h"
#include "math.h"

//------------------------------------------------------------------------------
#define PI              3.14159265358979323846f             // Pi
#define PI_MUL_2        6.28318530717958623200f             // 2 * Pi
#define PI_DIV_2        1.57079632679489655800f             // Pi / 2
#define PI_DIV_4        0.78539816339744827900f             // Pi / 4
#define INV_PI          0.31830988618379069122f             // 1 / Pi
#define DEGTORAD        0.01745329251994329547f             // Degrees to Radians
#define RADTODEG        57.29577951308232286465f            // Radians to Degrees
#define FLOAT_SMALL     1.0e-6f                             // Small number for floats
#define FLOAT_HUGE      1.0e+38f                            // Huge number for floats
#define FLOAT_EPSILON   1.0e-5f                             // Tolerance for floats
#define FLOAT_INFINITY  ((float)3.40282346638528860e+38)    // Infinity value for float


//----------------------------------------------------------------------------
// Flaoting point manipulation macros, used by the inline functions below.
#define FP_BITS(fp) (*(DWORD *)&(fp))
#define FP_ABS_BITS(fp) (FP_BITS(fp)&0x7FFFFFFF)
#define FP_SIGN_BIT(fp) (FP_BITS(fp)&0x80000000)
#define FP_ONE_BITS     0x3F800000
#define FP_HALFH        0x3f000000
#define FP_EXP_BITS     0x7f800000
#define FP_SMAN_BITS    0x807fffff

//----------------------------------------------------------------------------
// Define a C structure to hold a pair of floats.
struct SinCosPair
{
    float   fCos;
    float   fSin;
};


//----------------------------------------------------------------------------
extern float fast_atan(float x);
extern float fast_atan2(float x, float y);
extern float fast_acos(float x);
extern float fast_asin(float x);
extern float fast_log(float x);
extern float fast_log10(float x);
extern float fast_exp(float x);
extern float fast_sqrt(float x);
extern float fast_inversesqrt(float x);
extern float fast_fabs(float x);
extern void fast_sincos(float x, SinCosPair* v);
extern float fast_sin(float x);
extern float fast_cos(float x);
extern float fast_tan(float x);
extern float fast_pow(float x, float y);

extern float fast_hypot(float x, float y);
extern float fast_ceil(float x);
extern float fast_floor(float x);
extern float fast_tanh(float x);
extern float fast_cosh(float x);
extern float fast_sinh(float x);

#endif //SDK_HEADER_FASTMATH
