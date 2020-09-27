#pragma once

#ifndef _FMATH_H
#define _FMATH_H

/*///////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) 1998 Microsoft Corporation.  All Rights Reserved.
//
//  File:       fmath.h
//  Content:    C++ header to define standard float-valued math entry points
//              with the same names. This leverages function overloading in
//              C++ to 1) get more efficient implementations of the math
//              runtime if floats are being used, and 2) avoid thorny build
//              problems caused by downcasting the double-valued returns from
//              the standard math lib.
//
///////////////////////////////////////////////////////////////////////////*/

_inline void
sincos( float fRadians, float *pfSin, float *pfCos )
{
#ifdef _X86_
    _asm
    {
        mov eax,pfSin
        mov edx,pfCos
        fld fRadians
        // fsincos
        _emit 0xd9
        _emit 0xfb
        fstp dword ptr [edx]
        fstp dword ptr [eax]
    }
#else
    *pfSin = sinf( fRadians );
    *pfCos = cosf( fRadians );
#endif
}


// These overloaded functions are available only for c++
_inline float
hypot( float x, float y )
{
    return (float) _hypot(x, y);
}

_inline float
acos(float f)
{
    return acosf(f);
}

_inline float
asin(float f)
{
    return asinf(f);
}

_inline float
atan(float f)
{
    return atanf(f);
}

_inline float
atan2(float f1, float f2)
{
    return atan2f(f1, f2);
}

_inline float
cos(float f)
{
    return cosf(f);
}

_inline float
cosh(float f)
{
    return coshf(f);
}

_inline float
exp(float f)
{
    return expf(f);
}


_inline float
fabs(float f)
{
    return fabsf(f);
}

_inline float
fmod(float f1, float f2)
{
    return fmod(f1, f2);
}

_inline float
log(float f)
{
    return logf(f);
}

_inline float
log10(float f)
{
    return log10f(f);
}


_inline float
pow(float f1, float f2)
{
    return powf(f1, f2);
}

_inline float
sin(float f)
{
    return sinf(f);
}

_inline float
sinh(float f)
{
    return sinhf(f);
}

_inline float
tan(float f)
{
    return tanf(f);
}

_inline float
tanh(float f)
{
    return tanhf(f);
}

_inline float
sqrt(float f)
{
    return sqrtf(f);
}

_inline float
ceil(float f)
{
    return ceilf(f);
}

_inline float
floor(float f)
{
    return floorf(f);
}

_inline float
modf(float f, float *pf)
{
    return modff(f, pf);
}

//
//----------------------------------------------------------------------------
// Function: InvSqrt
//     Compute inverse square root - full precision
//----------------------------------------------------------------------------
//
_inline float
InvSqrt(float f)
{
    return 1.f / (float)sqrt( (double)f );
}

//
//----------------------------------------------------------------------------
//   The IEEE single precision floating point format is:
// 
//   SEEEEEEEEMMMMMMM MMMMMMMMMMMMMMMM
// 
//   S = Sign bit for whole number
//   E = Exponent bit (exponent in excess 127 form)
//   M = Mantissa bit
//----------------------------------------------------------------------------
//
#define _nEXPBIAS   127
#define _nEXPSHIFTS 23
#define _nEXPLSB    (1 << _nEXPSHIFTS)

//
//----------------------------------------------------------------------------
// Macro:       FloatToFixed
// Purpose:     Compute the 16.16 fixed point from the float
// Notes:       Relies on IEEE bit representation of floating point numbers
//----------------------------------------------------------------------------
//
#define FloatToFixed(__nDst, __fSrc) do { \
    float fTmp = __fSrc; \
    UINT nRaw = *((UINT *) &(fTmp)); \
    if (nRaw < ((_nEXPBIAS + 15 - 31) << _nEXPSHIFTS)) \
        __nDst = 0; \
    else \
    __nDst = (((nRaw | _nEXPLSB) << 8) >> ((_nEXPBIAS + 15) - (nRaw >> _nEXPSHIFTS))); \
} while (0)

//
//----------------------------------------------------------------------------
// Macro:       FloatToFixedNoScale
// Purpose:     Compute the 16.16 fixed point from the float, divides by 2^16
// Notes:       Relies on IEEE bit representation of floating point numbers
//----------------------------------------------------------------------------
//
#define FloatToFixedNoScale(nDst, fSrc) do { \
    float fTmp = fSrc; \
    UINT nRaw = *((UINT *) &(fTmp)); \
    if (nRaw < (_nEXPBIAS << _nEXPSHIFTS)) \
        nDst = 0; \
    else \
        nDst = (((nRaw | _nEXPLSB) << 8) >> ((_nEXPBIAS + 31) - (nRaw >> _nEXPSHIFTS))); \
} while (0)

//
//----------------------------------------------------------------------------
// Function:    FloatEquals
// Purpose:     Perform a "fuzzy" compare of two floating point numbers
// Notes:       Relies on IEEE bit representation of floating point numbers
//----------------------------------------------------------------------------
//
_inline BOOL __cdecl
FloatEquals(float x1, float x2)
{
#define maskEXP         0x7f800000
#define nBITSOFPREC     19
#define nMANTBITS       23
#define nEXPOFFSET      (nBITSOFPREC << nMANTBITS)
#define fZEROEPS        3.8E-6F
#define fTINYEPS        1.E-35F

    float fEps;
    unsigned int uExp;
    
    // compute epsilon
    if ((x1 == 0.0f) || (x2 == 0.0f)) {
        fEps = fZEROEPS;
    } else {
        float fMaxX;
        
        if (x1 > x2) 
            fMaxX = x1;
        else 
            fMaxX = x2;
        
        // grab the exponent of the larger number
        uExp = (*((unsigned int *) &fMaxX) & maskEXP);

        if (uExp < nEXPOFFSET) {
            // near zero
            fEps = fTINYEPS;
        } else {
            uExp -= nEXPOFFSET;
            fEps = *((float *) &uExp);
        }
    }
    return (((x1 + fEps) >= x2) && ((x1 - fEps) <= x2));
#undef maskEXP
#undef nBITSOFPREC
#undef nMANTBITS
#undef nEXPOFFSET
#undef fZEROEPS
#undef fTINYEPS
}

#endif // _FMATH_H
