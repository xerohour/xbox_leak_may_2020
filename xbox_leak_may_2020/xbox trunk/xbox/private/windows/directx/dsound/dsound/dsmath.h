/***************************************************************************
 *
 *  Copyright (C) 2000 Microsoft Corporation.  All Rights Reserved.
 *
 *  File:       dsmath.h
 *  Content:    Math helpers.
 *  History:
 *   Date       By      Reason
 *   ====       ==      ======
 *  03/24/01    dereks  Created.
 *
 ****************************************************************************/

#ifndef __DSMATH_H__
#define __DSMATH_H__

#include <float.h>
#include <math.h>

typedef struct _VECTOR4 {
    float x;
    float y;
    float z;
    float w;
} VECTOR4;

#ifndef FLT_MAX
#define FLT_MAX         3.402823466e+38F        /* max value */
#endif // FLT_MAX

#ifndef FLT_MIN
#define FLT_MIN         1.175494351e-38F        /* min positive value */
#endif // FLT_MIN

#ifdef __cplusplus

#define ftoi(x) \
    DirectSound::Math::FloatToLong(x)

#define FTOI(x) \
    DirectSound::Math::FloatToLong(x)

#define ftol(x) \
    DirectSound::Math::FloatToLong(x)

#define FTOL(x) \
    DirectSound::Math::FloatToLong(x)

namespace DirectSound
{
    namespace Math
    {
        double NormalizeDouble(double f);
        float NormalizeFloat(float f);
        long RatioToPitch(float flRatio);

        VECTOR4* Vec3Transform( VECTOR4 *pOut, const D3DVECTOR *pV, const D3DMATRIX *pM );
        D3DMATRIX* MatrixRotationAxis( D3DMATRIX *pOut, const D3DVECTOR *pV, float angle );

        double NormalizeVector3(D3DVECTOR *pdst, double x, double y, double z);
        float NormalizeVector3(D3DVECTOR *pdst, const D3DVECTOR *psrc);
        float NormalizeVector3(D3DVECTOR *pdst);

        double NormalizeVector2(D3DVECTOR *pdst, double x, double z);
        float NormalizeVector2(D3DVECTOR *pdst, const D3DVECTOR *psrc);
        float NormalizeVector2(D3DVECTOR *pdst);

        static _declspec(naked) long FloatToLong(float x)
        {
            __asm
            {
                cvttss2si eax, [esp+4]
                ret 4
            }
        }

        __inline double MagnitudeVector3(double x, double y, double z)
        {
            return sqrt((x * x) + (y * y) + (z * z));
        }

        __inline float MagnitudeVector3(float x, float y, float z)
        {
            return (float)sqrt((double)((x * x) + (y * y) + (z * z)));
        }

        __inline float MagnitudeVector3(const D3DVECTOR *p)
        {
            return (float)sqrt((double)((p->x * p->x) + (p->y * p->y) + (p->z * p->z)));
        }

        __inline double MagnitudeVector2(double x, double z)
        {
            return sqrt((x * x) + (z * z));
        }

        __inline float MagnitudeVector2(float x, float z)
        {
            return (float)sqrt((float)((x * x) + (z * z)));
        }

        __inline float MagnitudeVector2(const D3DVECTOR *p)
        {
            return (float)sqrt((p->x * p->x) + (p->z * p->z));
        }

        __inline long MetersToVolume(float x)
        {
            if(x >= 0.0f)
            {
                return ftoi(-2000.0f * log10f(x + 1.0f));
            }
            else
            {
                return DSBVOLUME_MAX;
            }
        }

        __inline long PowerToVolume(float x)
        {
            if(x <= 0.0f)
            {
                return DSBVOLUME_MIN;
            }
            else if(x >= 1.0f)
            {
                return DSBVOLUME_MAX;
            }
            else
            {
                return ftoi(1000.0f * log10f(x));
            }
        }

        __inline long AmplitudeToVolume(float x)
        {
            if(x <= 0.0f)
            {
                return DSBVOLUME_MIN;
            }
            else if(x >= 1.0f)
            {
                return DSBVOLUME_MAX;
            }
            else
            {
                return ftoi(2000.0f * log10f(x));
            }
        }

#ifdef DEBUG

        static unsigned int __fastcall lsb(unsigned int x)
        {
            ASSERT(x);
    
            __asm
            {
                mov     ecx, x
                bsf     eax, ecx
            }
        }

#else // DEBUG

        static unsigned int __fastcall lsb(unsigned int x)
        {
            __asm
            {
                bsf     eax, ecx
            }
        }

#endif // DEBUG

        __inline unsigned int log2(unsigned int x)
        {
            ASSERT(!(x & (x - 1)));
            return lsb(x);
        }
    }
}

using namespace DirectSound::Math;

#endif // __cplusplus

#endif // __DSMATH_H__
