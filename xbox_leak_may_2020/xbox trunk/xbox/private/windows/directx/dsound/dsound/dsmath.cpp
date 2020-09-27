/***************************************************************************
 *
 *  Copyright (C) 2000 Microsoft Corporation.  All Rights Reserved.
 *
 *  File:       dsmath.cpp
 *  Content:    Math helpers.
 *  History:
 *   Date       By      Reason
 *   ====       ==      ======
 *  03/24/01    dereks  Created.
 *
 ****************************************************************************/

#include "dsoundi.h"


double 
Math::NormalizeDouble
(
    double                  f
)
{
    int                     nClass  = _fpclass(f);
    
    if(nClass & (_FPCLASS_SNAN | _FPCLASS_QNAN))
    {
        DPF_ERROR("floating point error: NAN");
        f = 0.0f;
    }
    else if(nClass & _FPCLASS_NINF)
    {
        DPF_ERROR("floating point error: NINF");
        f = -DBL_MAX;
    }
    else if(nClass & _FPCLASS_PINF)
    {
        DPF_ERROR("floating point error: PINF");
        f = DBL_MAX;
    }

    return f;
}


float 
Math::NormalizeFloat
(
    float                   f
)
{
    int                     nClass  = _fpclass(f);
    
    if(nClass & (_FPCLASS_SNAN | _FPCLASS_QNAN))
    {
        DPF_ERROR("floating point error: NAN");
        f = 0.0f;
    }
    else if(nClass & _FPCLASS_NINF)
    {
        DPF_ERROR("floating point error: NINF");
        f = -FLT_MAX;
    }
    else if(nClass & _FPCLASS_PINF)
    {
        DPF_ERROR("floating point error: PINF");
        f = FLT_MAX;
    }

    return f;
}


double 
Math::NormalizeVector3
(
    D3DVECTOR *             pdst, 
    double                  x, 
    double                  y, 
    double                  z
)
{
    double                  magnitude;

    if(!x && !y && !z)
    {
        pdst->x = 0.0f;
        pdst->y = 0.0f;
        pdst->z = 0.0f;

        return 0.0f;
    }
    else if((FLT_MAX == x) || (FLT_MAX == y) || (FLT_MAX == z))
    {
        pdst->x = (float)x;
        pdst->y = (float)y;
        pdst->z = (float)z;

        return FLT_MAX;
    }

    magnitude = MagnitudeVector3(x, y, z);
    ASSERT(magnitude > 0.0f);

    pdst->x = (float)(x / magnitude);
    pdst->y = (float)(y / magnitude);
    pdst->z = (float)(z / magnitude);

    return magnitude;
}


float 
Math::NormalizeVector3
(
    D3DVECTOR *             pdst, 
    const D3DVECTOR *       psrc
)
{
    float                   magnitude;

    if(!psrc->x && !psrc->y && !psrc->z)
    {
        pdst->x = 0.0f;
        pdst->y = 0.0f;
        pdst->z = 0.0f;

        return 0.0f;
    }
    else if((FLT_MAX == psrc->x) || (FLT_MAX == psrc->y) || (FLT_MAX == psrc->z))
    {
        pdst->x = psrc->x;
        pdst->y = psrc->y;
        pdst->z = psrc->z;

        return FLT_MAX;
    }

    magnitude = MagnitudeVector3(psrc);
    ASSERT(magnitude > 0.0f);

    pdst->x = psrc->x / magnitude;
    pdst->y = psrc->y / magnitude;
    pdst->z = psrc->z / magnitude;

    return magnitude;
}


float 
Math::NormalizeVector3
(
    D3DVECTOR *             pdst
)
{
    float                   magnitude;

    if(!pdst->x && !pdst->y && !pdst->z)
    {
        return 0.0f;
    }
    else if((FLT_MAX == pdst->x) || (FLT_MAX == pdst->y) || (FLT_MAX == pdst->z))
    {
        return FLT_MAX;
    }

    magnitude = MagnitudeVector3(pdst);
    ASSERT(magnitude > 0.0f);

    pdst->x = pdst->x / magnitude;
    pdst->y = pdst->y / magnitude;
    pdst->z = pdst->z / magnitude;

    return magnitude;
}


double 
Math::NormalizeVector2
(
    D3DVECTOR *             pdst, 
    double                  x, 
    double                  z
)
{
    double                  magnitude;

    pdst->y = 0.0f;

    if(!x && !z)
    {
        pdst->x = 0.0f;
        pdst->z = 0.0f;

        return 0.0f;
    }
    else if((FLT_MAX == x) || (FLT_MAX == z))
    {
        pdst->x = (float)x;
        pdst->z = (float)z;

        return FLT_MAX;
    }

    magnitude = MagnitudeVector2(x, z);
    ASSERT(magnitude > 0.0f);

    pdst->x = (float)(x / magnitude);
    pdst->z = (float)(z / magnitude);

    return magnitude;
}


float 
Math::NormalizeVector2
(
    D3DVECTOR *             pdst, 
    const D3DVECTOR *       psrc
)
{
    float                   magnitude;

    pdst->y = 0.0f;
    
    if(!psrc->x && !psrc->z)
    {
        pdst->x = 0.0f;
        pdst->z = 0.0f;

        return 0.0f;
    }
    else if((FLT_MAX == psrc->x) || (FLT_MAX == psrc->z))
    {
        pdst->x = psrc->x;
        pdst->z = psrc->z;

        return FLT_MAX;
    }

    magnitude = MagnitudeVector2(psrc);
    ASSERT(magnitude > 0.0f);

    pdst->x = psrc->x / magnitude;
    pdst->z = psrc->z / magnitude;

    return magnitude;
}


float 
Math::NormalizeVector2
(
    D3DVECTOR *             pdst
)
{
    float                   magnitude;

    pdst->y = 0.0f;
    
    if(!pdst->x && !pdst->z)
    {
        return 0.0f;
    }
    else if((FLT_MAX == pdst->x) || (FLT_MAX == pdst->z))
    {
        return FLT_MAX;
    }

    magnitude = MagnitudeVector2(pdst);
    ASSERT(magnitude > 0.0f);

    pdst->x = pdst->x / magnitude;
    pdst->z = pdst->z / magnitude;

    return magnitude;
}


long 
Math::RatioToPitch
(
    float                   flRatio
)
{
    const float             fl4096  = 4096.0f;
    long                    lPitch;

    ASSERT(flRatio);

    __asm 
    {
        fld     fl4096
        fld     flRatio
        fyl2x
        fistp   lPitch
    }

    return lPitch;
}


//
// sincosf - Compute the sin and cos of an angle at the same time
//

static inline void
sincosf(float angle, float *psin, float *pcos)
{
#ifdef _X86_
#define fsincos __asm _emit 0xd9 __asm _emit 0xfb
    __asm {
        mov eax, psin
        mov edx, pcos
        fld angle
        fsincos
        fstp DWORD ptr [edx]
        fstp DWORD ptr [eax]
    }
#undef fsincos
#else //!_X86_
    *psin = sinf(angle);
    *pcos = cosf(angle);
#endif //!_X86_
}

D3DMATRIX* 
Math::MatrixRotationAxis
( 
    D3DMATRIX *             pOut, 
    const D3DVECTOR *       pV, 
    float                   angle 
)
{
#if DBG
    if(!pOut || !pV)
        return NULL;
#endif

    float s, c;
    sincosf(angle, &s, &c);
    float c1 = 1 - c;

    D3DVECTOR v = *pV;
    NormalizeVector3(&v, &v);

    float xyc1 = v.x * v.y * c1;
    float yzc1 = v.y * v.z * c1;
    float zxc1 = v.z * v.x * c1;

    pOut->_11 = v.x * v.x * c1 + c;
    pOut->_12 = xyc1 + v.z * s;
    pOut->_13 = zxc1 - v.y * s;
    pOut->_14 = 0.0f;

    pOut->_21 = xyc1 - v.z * s;
    pOut->_22 = v.y * v.y * c1 + c;
    pOut->_23 = yzc1 + v.x * s;
    pOut->_24 = 0.0f;

    pOut->_31 = zxc1 + v.y * s;
    pOut->_32 = yzc1 - v.x * s;
    pOut->_33 = v.z * v.z * c1 + c;
    pOut->_34 = 0.0f;

    pOut->_41 = 0.0f;
    pOut->_42 = 0.0f;
    pOut->_43 = 0.0f;
    pOut->_44 = 1.0f;

    return pOut;
}

VECTOR4* 
Math::Vec3Transform
( 
    VECTOR4 *               pOut, 
    const D3DVECTOR *       pV, 
    const D3DMATRIX *       pM 
)
{
#if DBG
    if(!pOut || !pV || !pM)
        return NULL;
    if ((ULONG_PTR)pM & 0xF)
    {
        DPF_ERROR("Matrix must be 16-byte aligned");
        return NULL;
    }
#endif

#ifdef _X86_
    __asm {
        mov     eax, pV
        mov     ecx, pM
        movss   xmm2, [eax]             // pV->x
        shufps  xmm2, xmm2, 0           // Fill xmm2 with x
        mulps   xmm2, [ecx]             // x*pM->_11, x*pM->_12, x*pM->_13, x*pM->_14

        movss   xmm1, [eax+4]           // pV->y
        shufps  xmm1, xmm1, 0           // Fill xmm1 with y
        mulps   xmm1, [ecx+16]          // y*pM->_21, y*pM->_22, y*pM->_23, y*pM->_24

        movss   xmm3, [eax+8]
        shufps  xmm3, xmm3, 0           // Fill xmm3 with z
        mulps   xmm3, [ecx+32]          // z*pM->_31, z*pM->_32, z*pM->_33, z*pM->_34

        addps   xmm2, [ecx + 48]        // Add the last row of the matrix
        mov     eax,  pOut
        addps   xmm2, xmm1
        addps   xmm2, xmm3
        movups  [eax], xmm2             // Output result
    }
#else // !_X86_
    XGVECTOR4 v;

    v.x = pV->x * pM->_11 + pV->y * pM->_21 + pV->z * pM->_31 + pM->_41;
    v.y = pV->x * pM->_12 + pV->y * pM->_22 + pV->z * pM->_32 + pM->_42;
    v.z = pV->x * pM->_13 + pV->y * pM->_23 + pV->z * pM->_33 + pM->_43;
    v.w = pV->x * pM->_14 + pV->y * pM->_24 + pV->z * pM->_34 + pM->_44;

    *pOut = v;
    return pOut;
#endif // !_X86_
}


