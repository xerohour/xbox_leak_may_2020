#if XBOX
#define _USE_XGMATH
#include <xtl.h>
#else
#include <windows.h>
#include <d3d8-xbox.h>
#endif

#include <xgmath.h>
#include <debug.h>

namespace XGRAPHICS
{

#pragma warning(disable:4035)

#define EPSILON 0.00001f

//
// WithinEpsilon - Are two values within EPSILON of each other?
//

static inline BOOL
WithinEpsilon(float a, float b)
{
    float f = a - b;
    return -EPSILON <= f && f <= EPSILON;
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


//--------------------------
// 2D Vector
//--------------------------

extern "C"
XGVECTOR2* WINAPI XGVec2Normalize
    ( XGVECTOR2 *pOut, const XGVECTOR2 *pV )
{
#if DBG
    if(!pOut || !pV)
        return NULL;
#endif

    float f = XGVec2LengthSq(pV);

    if(WithinEpsilon(f, 1.0f))
    {
        if(pOut != pV)
            *pOut = *pV;
    }
    else if(f > EPSILON * EPSILON)
    {
        *pOut = *pV / sqrtf(f);
    }
    else
    {
        pOut->x = 0.0f;
        pOut->y = 0.0f;
    }

    return pOut;
}

extern "C"
XGVECTOR2* WINAPI XGVec2Hermite
    ( XGVECTOR2 *pOut, const XGVECTOR2 *pV1, const XGVECTOR2 *pT1,
      const XGVECTOR2 *pV2, const XGVECTOR2 *pT2, float s )
{
#if DBG
    if(!pOut || !pV1 || !pT1 || !pV2 || !pT2)
        return NULL;
#endif

    float s2 = s * s;
    float s3 = s * s2;

    float sV1 = 2.0f * s3 - 3.0f * s2 + 1.0f;
    float sT1 = s3 - 2.0f * s2 + s;
    float sV2 = -2.0f * s3 + 3.0f * s2;
    float sT2 = s3 - s2;

    pOut->x = sV1 * pV1->x + sT1 * pT1->x + sV2 * pV2->x + sT2 * pT2->x;
    pOut->y = sV1 * pV1->y + sT1 * pT1->y + sV2 * pV2->y + sT2 * pT2->y;
    return pOut;
}

extern "C"
XGVECTOR2* WINAPI XGVec2CatmullRom
    ( XGVECTOR2 *pOut, const XGVECTOR2 *pV0, const XGVECTOR2 *pV1,
      const XGVECTOR2 *pV2, const XGVECTOR2 *pV3, float s )
{
#if DBG
    if(!pOut || !pV0 || !pV1 || !pV2 || !pV3)
        return NULL;
#endif

    float s2 = s * s;
    float s3 = s * s2;

    float sV0 = -s3 + s2 + s2 - s;
    float sV1 = 3.0f * s3 - 5.0f * s2 + 2.0f;
    float sV2 = -3.0f * s3 + 4.0f * s2 + s;
    float sV3 = s3 - s2;

    pOut->x = 0.5f * (sV0 * pV0->x + sV1 * pV1->x + sV2 * pV2->x + sV3 * pV3->x);
    pOut->y = 0.5f * (sV0 * pV0->y + sV1 * pV1->y + sV2 * pV2->y + sV3 * pV3->y);
    return pOut;
}

extern "C"
XGVECTOR2* WINAPI XGVec2BaryCentric
    ( XGVECTOR2 *pOut, const XGVECTOR2 *pV1, const XGVECTOR2 *pV2,
      XGVECTOR2 *pV3, float f, float g)
{
#if DBG
    if(!pOut || !pV1 || !pV2 || !pV3)
        return NULL;
#endif

    pOut->x = pV1->x + f * (pV2->x - pV1->x) + g * (pV3->x - pV1->x);
    pOut->y = pV1->y + f * (pV2->y - pV1->y) + g * (pV3->y - pV1->y);
    return pOut;
}

extern "C"
XGVECTOR4* WINAPI XGVec2Transform
    ( XGVECTOR4 *pOut, const XGVECTOR2 *pV, const XGMATRIX *pM )
{
#if DBG
    if(!pOut || !pV || !pM)
        return NULL;
    if ((ULONG_PTR)pM & 0xF)
    {
        DXGRIP("Matrix must be 16-byte aligned");
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

        mov     eax,  pOut
        addps   xmm2, [ecx + 48]        // Add the last row of the matrix
        addps   xmm2, xmm1
        movups  [eax], xmm2             // Output result
    }
#else // !_X86_
    XGVECTOR4 v;

    v.x = pV->x * pM->_11 + pV->y * pM->_21 + pM->_41;
    v.y = pV->x * pM->_12 + pV->y * pM->_22 + pM->_42;
    v.z = pV->x * pM->_13 + pV->y * pM->_23 + pM->_43;
    v.w = pV->x * pM->_14 + pV->y * pM->_24 + pM->_44;

    *pOut = v;
    return pOut;
#endif // !_X86_
}

extern "C"
XGVECTOR2* WINAPI XGVec2TransformCoord
    ( XGVECTOR2 *pOut, const XGVECTOR2 *pV, const XGMATRIX *pM )
{
#if DBG
    if(!pOut || !pV || !pM)
        return NULL;
    if ((ULONG_PTR)pM & 0xF)
    {
        DXGRIP("Matrix must be 16-byte aligned");
        return NULL;
    }
#endif

    float w;

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

        mov     eax,  pOut
        addps   xmm2, [ecx + 48]        // Add the last row of the matrix
        addps   xmm2, xmm1

        movlps  [eax], xmm2
        shufps  xmm2, xmm2, 0FFh        // Fill xmm2 with w
        movss   [w], xmm2
    }

#else // !_X86_
    XGVECTOR4 v;

    v.x = pV->x * pM->_11 + pV->y * pM->_21 + pM->_41;
    v.y = pV->x * pM->_12 + pV->y * pM->_22 + pM->_42;
    w   = pV->x * pM->_14 + pV->y * pM->_24 + pM->_44;

    *pOut = *((XGVECTOR2 *) &v);
#endif // !_X86_

    if(!WithinEpsilon(w, 1.0f))
        *pOut /= w;

    return pOut;
}

extern "C"
XGVECTOR2* WINAPI XGVec2TransformNormal
    ( XGVECTOR2 *pOut, const XGVECTOR2 *pV, const XGMATRIX *pM )
{
#if DBG
    if(!pOut || !pV || !pM)
        return NULL;
    if ((ULONG_PTR)pM & 0xF)
    {
        DXGRIP("Matrix must be 16-byte aligned");
        return NULL;
    }
#endif

#ifdef _X86_
    __asm {
        mov     eax, pV
        movss   xmm2, [eax]             // pV->x
        mov     ecx, pM
        shufps  xmm2, xmm2, 0           // Fill xmm2 with x
        mulps   xmm2, [ecx]             // x*pM->_11, x*pM->_12, x*pM->_13, x*pM->_14

        movss   xmm1, [eax+4]           // pV->y
        shufps  xmm1, xmm1, 0           // Fill xmm1 with y
        mulps   xmm1, [ecx+16]          // y*pM->_21, y*pM->_22, y*pM->_23, y*pM->_24

        mov     eax,  pOut
        addps   xmm2, xmm1
        movlps  [eax], xmm2             // Output result
    }
#else // !_X86_

    XGVECTOR2 v;

    v.x = pV->x * pM->_11 + pV->y * pM->_21;
    v.y = pV->x * pM->_12 + pV->y * pM->_22;

    *pOut = v;
    return pOut;
#endif // !_X86_
}


//--------------------------
// 3D Vector
//--------------------------

extern "C"
XGVECTOR3* WINAPI XGVec3Normalize
    ( XGVECTOR3 *pOut, const XGVECTOR3 *pV )
{
#if DBG
    if(!pOut || !pV)
        return NULL;
#endif

    float f = XGVec3LengthSq(pV);

    if(WithinEpsilon(f, 1.0f))
    {
        if(pOut != pV)
            *pOut = *pV;
    }
    else if(f > EPSILON * EPSILON)
    {
        *pOut = *pV / sqrtf(f);
    }
    else
    {
        pOut->x = 0.0f;
        pOut->y = 0.0f;
        pOut->z = 0.0f;
    }

    return pOut;
}

extern "C"
XGVECTOR3* WINAPI XGVec3Hermite
    ( XGVECTOR3 *pOut, const XGVECTOR3 *pV1, const XGVECTOR3 *pT1,
      const XGVECTOR3 *pV2, const XGVECTOR3 *pT2, float s )
{
#if DBG
    if(!pOut || !pV1 || !pT1 || !pV2 || !pT2)
        return NULL;
#endif

    float s2 = s * s;
    float s3 = s * s2;

    float sV1 = 2.0f * s3 - 3.0f * s2 + 1.0f;
    float sT1 = s3 - 2.0f * s2 + s;
    float sV2 = -2.0f * s3 + 3.0f * s2;
    float sT2 = s3 - s2;

    pOut->x = sV1 * pV1->x + sT1 * pT1->x + sV2 * pV2->x + sT2 * pT2->x;
    pOut->y = sV1 * pV1->y + sT1 * pT1->y + sV2 * pV2->y + sT2 * pT2->y;
    pOut->z = sV1 * pV1->z + sT1 * pT1->z + sV2 * pV2->z + sT2 * pT2->z;
    return pOut;
}

extern "C"
XGVECTOR3* WINAPI XGVec3CatmullRom
    ( XGVECTOR3 *pOut, const XGVECTOR3 *pV0, const XGVECTOR3 *pV1,
      const XGVECTOR3 *pV2, const XGVECTOR3 *pV3, float s )
{
#if DBG
    if(!pOut || !pV0 || !pV1 || !pV2 || !pV3)
        return NULL;
#endif

    float s2 = s * s;
    float s3 = s * s2;

    float sV0 = -s3 + s2 + s2 - s;
    float sV1 = 3.0f * s3 - 5.0f * s2 + 2.0f;
    float sV2 = -3.0f * s3 + 4.0f * s2 + s;
    float sV3 = s3 - s2;

    pOut->x = 0.5f * (sV0 * pV0->x + sV1 * pV1->x + sV2 * pV2->x + sV3 * pV3->x);
    pOut->y = 0.5f * (sV0 * pV0->y + sV1 * pV1->y + sV2 * pV2->y + sV3 * pV3->y);
    pOut->z = 0.5f * (sV0 * pV0->z + sV1 * pV1->z + sV2 * pV2->z + sV3 * pV3->z);
    return pOut;
}

extern "C"
XGVECTOR3* WINAPI XGVec3BaryCentric
    ( XGVECTOR3 *pOut, const XGVECTOR3 *pV1, const XGVECTOR3 *pV2,
      const XGVECTOR3 *pV3, float f, float g)
{
#if DBG
    if(!pOut || !pV1 || !pV2 || !pV3)
        return NULL;
#endif

    pOut->x = pV1->x + f * (pV2->x - pV1->x) + g * (pV3->x - pV1->x);
    pOut->y = pV1->y + f * (pV2->y - pV1->y) + g * (pV3->y - pV1->y);
    pOut->z = pV1->z + f * (pV2->z - pV1->z) + g * (pV3->z - pV1->z);
    return pOut;
}

extern "C"
XGVECTOR4* WINAPI XGVec3Transform
    ( XGVECTOR4 *pOut, const XGVECTOR3 *pV, const XGMATRIX *pM )
{
#if DBG
    if(!pOut || !pV || !pM)
        return NULL;
    if ((ULONG_PTR)pM & 0xF)
    {
        DXGRIP("Matrix must be 16-byte aligned");
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

extern "C"
XGVECTOR3* WINAPI XGVec3TransformCoord
    ( XGVECTOR3 *pOut, const XGVECTOR3 *pV, const XGMATRIX *pM )
{
#if DBG
    if(!pOut || !pV || !pM)
        return NULL;
    if ((ULONG_PTR)pM & 0xF)
    {
        DXGRIP("Matrix must be 16-byte aligned");
        return NULL;
    }
#endif

    float w;

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

        movlps  [eax], xmm2             // Move pOut x and y
        shufps  xmm2, xmm2, 0FEh
        movss   [eax + 8], xmm2         // Move pOut z

        shufps  xmm2, xmm2, 0FFh
        movss   [w], xmm2               // Move w
    }
#else // !_X86_
    XGVECTOR3 v;

    v.x = pV->x * pM->_11 + pV->y * pM->_21 + pV->z * pM->_31 + pM->_41;
    v.y = pV->x * pM->_12 + pV->y * pM->_22 + pV->z * pM->_32 + pM->_42;
    v.z = pV->x * pM->_13 + pV->y * pM->_23 + pV->z * pM->_33 + pM->_43;
    w   = pV->x * pM->_14 + pV->y * pM->_24 + pV->z * pM->_34 + pM->_44;

    *pOut = v;
#endif // !_X86_

    if(!WithinEpsilon(w, 1.0f))
        *pOut /= w;

    return pOut;
}

extern "C"
XGVECTOR3* WINAPI XGVec3TransformNormal
    ( XGVECTOR3 *pOut, const XGVECTOR3 *pV, const XGMATRIX *pM )
{
#if DBG
    if(!pOut || !pV || !pM)
        return NULL;
    if ((ULONG_PTR)pM & 0xF)
    {
        DXGRIP("Matrix must be 16-byte aligned");
        return NULL;
    }
#endif

#ifdef _X86_
    __asm {
        mov     eax, pV
        movss   xmm2, [eax]             // pV->x
        mov     ecx, pM
        shufps  xmm2, xmm2, 0           // Fill xmm2 with x
        mulps   xmm2, [ecx]             // x*pM->_11, x*pM->_12, x*pM->_13, x*pM->_14

        movss   xmm1, [eax+4]           // pV->y
        shufps  xmm1, xmm1, 0           // Fill xmm1 with y
        mulps   xmm1, [ecx+16]          // y*pM->_21, y*pM->_22, y*pM->_23, y*pM->_24

        movss   xmm3, [eax+8]           // pV->z
        shufps  xmm3, xmm3, 0           // Fill xmm3 with z
        mulps   xmm3, [ecx+32]          // z*pM->_31, z*pM->_32, z*pM->_33, z*pM->_34

        addps   xmm2, xmm1
        mov     eax,  pOut
        addps   xmm2, xmm3
        movlps  [eax], xmm2             // Output x and y of the result
        movhlps xmm2, xmm2
        movss   [eax+8], xmm2           // Output z of the result
    }
#else // !_X86_
    XGVECTOR3 v;

    v.x = pV->x * pM->_11 + pV->y * pM->_21 + pV->z * pM->_31;
    v.y = pV->x * pM->_12 + pV->y * pM->_22 + pV->z * pM->_32;
    v.z = pV->x * pM->_13 + pV->y * pM->_23 + pV->z * pM->_33;

    *pOut = v;
    return pOut;
#endif // !_X86_
}


extern "C"
XGVECTOR3* WINAPI XGVec3Project
    ( XGVECTOR3 *pOut, const XGVECTOR3 *pV, const D3DVIEWPORT8 *pViewport,
      const XGMATRIX *pProjection, const XGMATRIX *pView, const XGMATRIX *pWorld)
{
#if DBG
    if(!pOut || !pV)
        return NULL;
#endif

    XGMATRIX mat;
    const XGMATRIX *pMat = &mat;

    switch(((NULL != pWorld) << 2) | ((NULL != pView) << 1) | (NULL != pProjection))
    {
    case 0: // ---
        XGMatrixIdentity(&mat);
        break;

    case 1: // --P
        pMat = pProjection;
        break;

    case 2: // -V-
        pMat = pView;
        break;

    case 3: // -VP
        XGMatrixMultiply(&mat, pView, pProjection);
        break;

    case 4: // W--
        pMat = pWorld;
        break;

    case 5: // W-P
        XGMatrixMultiply(&mat, pWorld, pProjection);
        break;

    case 6: // WV-
        XGMatrixMultiply(&mat, pWorld, pView);
        break;

    case 7: // WVP
        XGMatrixMultiply(&mat, pWorld, pView);
        XGMatrixMultiply(&mat, &mat, pProjection);
        break;
    }


    XGVec3TransformCoord(pOut, pV, pMat);

    if(pViewport)
    {
        pOut->x = ( pOut->x + 1.0f) * 0.5f * (float) pViewport->Width  + (float) pViewport->X;
        pOut->y = (-pOut->y + 1.0f) * 0.5f * (float) pViewport->Height + (float) pViewport->Y;
        pOut->z = pOut->z * (pViewport->MaxZ - pViewport->MinZ) +  pViewport->MinZ;
    }

    return pOut;
}


extern "C"
XGVECTOR3* WINAPI XGVec3Unproject
    ( XGVECTOR3 *pOut, const XGVECTOR3 *pV, const D3DVIEWPORT8 *pViewport,
      const XGMATRIX *pProjection, const XGMATRIX *pView, const XGMATRIX *pWorld)
{
#if DBG
    if(!pOut || !pV)
        return NULL;
#endif

    XGMATRIX mat;

    switch(((NULL != pWorld) << 2) | ((NULL != pView) << 1) | (NULL != pProjection))
    {
    case 0: // ---
        XGMatrixIdentity(&mat);
        break;

    case 1: // --P
        XGMatrixInverse(&mat, NULL, pProjection);
        break;

    case 2: // -V-
        XGMatrixInverse(&mat, NULL, pView);
        break;

    case 3: // -VP
        XGMatrixMultiply(&mat, pView, pProjection);
        XGMatrixInverse(&mat, NULL, &mat);
        break;

    case 4: // W--
        XGMatrixInverse(&mat, NULL, pWorld);
        break;

    case 5: // W-P
        XGMatrixMultiply(&mat, pWorld, pProjection);
        XGMatrixInverse(&mat, NULL, &mat);
        break;

    case 6: // WV-
        XGMatrixMultiply(&mat, pWorld, pView);
        XGMatrixInverse(&mat, NULL, &mat);
        break;

    case 7: // WVP
        XGMatrixMultiply(&mat, pWorld, pView);
        XGMatrixMultiply(&mat, &mat, pProjection);
        XGMatrixInverse(&mat, NULL, &mat);
        break;
    }


    if(pViewport)
    {
        pOut->x = (pV->x - (float) pViewport->X) / (float) pViewport->Width * 2.0f - 1.0f;
        pOut->y = -((pV->y - (float) pViewport->Y) / (float) pViewport->Height * 2.0f - 1.0f);
        pOut->z = (pV->z - pViewport->MinZ) / (pViewport->MaxZ - pViewport->MinZ);

        XGVec3TransformCoord(pOut, pOut, &mat);
    }
    else
    {
        XGVec3TransformCoord(pOut, pV, &mat);
    }

    return pOut;
}



//--------------------------
// 4D Vector
//--------------------------

extern "C"
XGVECTOR4* WINAPI XGVec4Cross
    ( XGVECTOR4 *pOut, const XGVECTOR4 *pV1, const XGVECTOR4 *pV2,
      const XGVECTOR4 *pV3)
{
#if DBG
    if(!pOut || !pV1 || !pV2 || !pV3)
        return NULL;
#endif

    XGVECTOR4 v;

    v.x = pV1->y * (pV2->z * pV3->w - pV3->z * pV2->w) -
          pV1->z * (pV2->y * pV3->w - pV3->y * pV2->w) +
          pV1->w * (pV2->y * pV3->z - pV3->y * pV2->z);

    v.y = pV1->x * (pV3->z * pV2->w - pV2->z * pV3->w) -
          pV1->z * (pV3->x * pV2->w - pV2->x * pV3->w) +
          pV1->w * (pV3->x * pV2->z - pV2->x * pV3->z);

    v.z = pV1->x * (pV2->y * pV3->w - pV3->y * pV2->w) -
          pV1->y * (pV2->x * pV3->w - pV3->x * pV2->w) +
          pV1->w * (pV2->x * pV3->y - pV3->x * pV2->y);

    v.w = pV1->x * (pV3->y * pV2->z - pV2->y * pV3->z) -
          pV1->y * (pV3->x * pV2->z - pV2->x * pV3->z) +
          pV1->z * (pV3->x * pV2->y - pV2->x * pV3->y);

    *pOut = v;
    return pOut;
}

extern "C"
XGVECTOR4* WINAPI XGVec4Normalize
    ( XGVECTOR4 *pOut, const XGVECTOR4 *pV )
{
#if DBG
    if(!pOut || !pV)
        return NULL;
#endif

    float f = XGVec4LengthSq(pV);

    if(WithinEpsilon(f, 1.0f))
    {
        if(pOut != pV)
            *pOut = *pV;
    }
    else if(f > EPSILON * EPSILON)
    {
        *pOut = *pV / sqrtf(f);
    }
    else
    {
        pOut->x = 0.0f;
        pOut->y = 0.0f;
        pOut->z = 0.0f;
        pOut->w = 0.0f;
    }

    return pOut;
}

extern "C"
XGVECTOR4* WINAPI XGVec4Hermite
    ( XGVECTOR4 *pOut, const XGVECTOR4 *pV1, const XGVECTOR4 *pT1,
      const XGVECTOR4 *pV2, const XGVECTOR4 *pT2, float s )
{
#if DBG
    if(!pOut || !pV1 || !pT1 || !pV2 || !pT2)
        return NULL;
#endif

    float s2 = s * s;
    float s3 = s * s2;

    float sV1 = 2.0f * s3 - 3.0f * s2 + 1.0f;
    float sT1 = s3 - 2.0f * s2 + s;
    float sV2 = -2.0f * s3 + 3.0f * s2;
    float sT2 = s3 - s2;

    pOut->x = sV1 * pV1->x + sT1 * pT1->x + sV2 * pV2->x + sT2 * pT2->x;
    pOut->y = sV1 * pV1->y + sT1 * pT1->y + sV2 * pV2->y + sT2 * pT2->y;
    pOut->z = sV1 * pV1->z + sT1 * pT1->z + sV2 * pV2->z + sT2 * pT2->z;
    pOut->w = sV1 * pV1->w + sT1 * pT1->w + sV2 * pV2->w + sT2 * pT2->w;
    return pOut;
}

extern "C"
XGVECTOR4* WINAPI XGVec4CatmullRom
    ( XGVECTOR4 *pOut, const XGVECTOR4 *pV0, const XGVECTOR4 *pV1,
      const XGVECTOR4 *pV2, const XGVECTOR4 *pV3, float s )
{
#if DBG
    if(!pOut || !pV0 || !pV1 || !pV2 || !pV3)
        return NULL;
#endif

    float s2 = s * s;
    float s3 = s * s2;

    float sV0 = -s3 + s2 + s2 - s;
    float sV1 = 3.0f * s3 - 5.0f * s2 + 2.0f;
    float sV2 = -3.0f * s3 + 4.0f * s2 + s;
    float sV3 = s3 - s2;

    pOut->x = 0.5f * (sV0 * pV0->x + sV1 * pV1->x + sV2 * pV2->x + sV3 * pV3->x);
    pOut->y = 0.5f * (sV0 * pV0->y + sV1 * pV1->y + sV2 * pV2->y + sV3 * pV3->y);
    pOut->z = 0.5f * (sV0 * pV0->z + sV1 * pV1->z + sV2 * pV2->z + sV3 * pV3->z);
    pOut->w = 0.5f * (sV0 * pV0->w + sV1 * pV1->w + sV2 * pV2->w + sV3 * pV3->w);
    return pOut;
}

extern "C"
XGVECTOR4* WINAPI XGVec4BaryCentric
    ( XGVECTOR4 *pOut, const XGVECTOR4 *pV1, const XGVECTOR4 *pV2,
      const XGVECTOR4 *pV3, float f, float g)
{
#if DBG
    if(!pOut || !pV1 || !pV2 || !pV3)
        return NULL;
#endif

    pOut->x = pV1->x + f * (pV2->x - pV1->x) + g * (pV3->x - pV1->x);
    pOut->y = pV1->y + f * (pV2->y - pV1->y) + g * (pV3->y - pV1->y);
    pOut->z = pV1->z + f * (pV2->z - pV1->z) + g * (pV3->z - pV1->z);
    pOut->w = pV1->w + f * (pV2->w - pV1->w) + g * (pV3->w - pV1->w);
    return pOut;
}

extern "C"
XGVECTOR4* WINAPI XGVec4Transform
    ( XGVECTOR4 *pOut, const XGVECTOR4 *pV, const XGMATRIX *pM )
{
#if DBG
    if(!pOut || !pV || !pM)
        return NULL;
    if ((ULONG_PTR)pM & 0xF)
    {
        DXGRIP("Matrix must be 16-byte aligned");
        return NULL;
    }
#endif

#ifdef _X86_
    __asm {
        mov     eax, pV
        movups  xmm2, [eax]             // pV->x
        mov     eax, pM
        movaps  xmm1, xmm2              // pV->x
        shufps  xmm2, xmm2, 0           // Fill xmm2 with x
        mulps   xmm2, [eax]             // x*pM->_11, x*pM->_12, x*pM->_13, x*pM->_14

        movaps  xmm3, xmm1
        shufps  xmm1, xmm1, 55h         // Fill xmm1 with y
        mulps   xmm1, [eax+16]          // y*pM->_21, y*pM->_22, y*pM->_23, y*pM->_24

        movaps  xmm4, xmm3
        shufps  xmm3, xmm3, 0AAh        // Fill xmm3 with z
        mulps   xmm3, [eax+32]          // z*pM->_31, z*pM->_32, z*pM->_33, z*pM->_34

        shufps  xmm4, xmm4, 0FFh        // Fill xmm3 with w
        mulps   xmm4, [eax+48]          // w*pM->_41, w*pM->_42, w*pM->_43, w*pM->_44

        mov     eax,  pOut
        addps   xmm2, xmm1
        addps   xmm2, xmm3
        addps   xmm2, xmm4
        movups  [eax], xmm2             // Output result
    }
#else // !_X86_
    XGVECTOR4 v;

    v.x = pV->x * pM->_11 + pV->y * pM->_21 + pV->z * pM->_31 + pV->w * pM->_41;
    v.y = pV->x * pM->_12 + pV->y * pM->_22 + pV->z * pM->_32 + pV->w * pM->_42;
    v.z = pV->x * pM->_13 + pV->y * pM->_23 + pV->z * pM->_33 + pV->w * pM->_43;
    v.w = pV->x * pM->_14 + pV->y * pM->_24 + pV->z * pM->_34 + pV->w * pM->_44;

    *pOut = v;
    return pOut;
#endif // !_X86_
}


//--------------------------
// 4D Matrix
//--------------------------

extern "C"
float WINAPI XGMatrixfDeterminant
    ( const XGMATRIX *pM )
{
#if DBG
    if(!pM)
        return 0.0f;
#endif

    return (pM->_11 * (pM->_22 * (pM->_33 * pM->_44 - pM->_43 * pM->_34) -
                       pM->_23 * (pM->_32 * pM->_44 - pM->_42 * pM->_34) +
                       pM->_24 * (pM->_32 * pM->_43 - pM->_42 * pM->_33)))

         - (pM->_12 * (pM->_21 * (pM->_33 * pM->_44 - pM->_43 * pM->_34) -
                       pM->_23 * (pM->_31 * pM->_44 - pM->_41 * pM->_34) +
                       pM->_24 * (pM->_31 * pM->_43 - pM->_41 * pM->_33)))

         + (pM->_13 * (pM->_21 * (pM->_32 * pM->_44 - pM->_42 * pM->_34) -
                       pM->_22 * (pM->_31 * pM->_44 - pM->_41 * pM->_34) +
                       pM->_24 * (pM->_31 * pM->_42 - pM->_41 * pM->_32)))

         - (pM->_14 * (pM->_21 * (pM->_32 * pM->_43 - pM->_42 * pM->_33) -
                       pM->_22 * (pM->_31 * pM->_43 - pM->_41 * pM->_33) +
                       pM->_23 * (pM->_31 * pM->_42 - pM->_41 * pM->_32)));
}


extern "C"
XGMATRIX* WINAPI XGMatrixMultiply
    ( XGMATRIX *pOut, const XGMATRIX *pM1, const XGMATRIX *pM2 )
{
#if DBG
    if(!pOut || !pM1 || !pM2)
        return NULL;
    
    if ((ULONG_PTR)pOut & 0xF ||
        (ULONG_PTR)pM1 & 0xF ||
        (ULONG_PTR)pM2 & 0xF)
    {
        DXGRIP("Matrix must be 16-byte aligned");
        return NULL;
    }
#endif // DBG
    __asm
    {
        mov     eax, pM1
        mov     ecx, pM2

        // Result will be in xmm2, xmm3, xmm4, xmm5

        movaps  xmm2, [eax]                 
        shufps  xmm2, xmm2, 0               
        mulps   xmm2, [ecx]             // a11*b11, a11*b12, a11*b13, a11*b14

        movaps  xmm1, [eax]
        shufps  xmm1, xmm1, 55h
        mulps   xmm1, [ecx+16]          // a12*b21, a12*b22, a12*b23, a12*b24

        movaps  xmm0, [eax]
        shufps  xmm0, xmm0, 0AAh
        mulps   xmm0, [ecx+32]          // a13*b31, a13*b32, a13*b33, a13*b34

        addps   xmm2, xmm1

        movaps  xmm1, [eax]
        shufps  xmm1, xmm1, 0FFh
        mulps   xmm1, [ecx+48]          // a14*b41, a14*b42, a14*b43, a14*b44

        addps   xmm2, xmm0              

        movaps  xmm3, [eax+16]          // Start computing 2 row of the result
        shufps  xmm3, xmm3, 0

        addps   xmm2, xmm1              // 1 row of the result

        mulps   xmm3, [ecx]

        movaps  xmm1, [eax+16]
        shufps  xmm1, xmm1, 55h
        mulps   xmm1, [ecx+16]

        movaps  xmm0, [eax+16]
        shufps  xmm0, xmm0, 0AAh
        mulps   xmm0, [ecx+32]

        addps   xmm3, xmm1

        movaps  xmm1, [eax+16]
        shufps  xmm1, xmm1, 0FFh
        mulps   xmm1, [ecx+48]

        addps   xmm3, xmm0              

        movaps  xmm4, [eax+32]          // Start computing 3 row of the result
        shufps  xmm4, xmm4, 0
        
        addps   xmm3, xmm1              // 2 row of the result

        mulps   xmm4, [ecx]

        movaps  xmm1, [eax+32]
        shufps  xmm1, xmm1, 55h
        mulps   xmm1, [ecx+16]

        movaps  xmm0, [eax+32]
        shufps  xmm0, xmm0, 0AAh
        mulps   xmm0, [ecx+32]

        addps   xmm4, xmm1

        movaps  xmm1, [eax+32]
        shufps  xmm1, xmm1, 0FFh
        mulps   xmm1, [ecx+48]

        addps   xmm4, xmm0

        movaps  xmm5, [eax+48]          // Start computing 4 row of the result
        shufps  xmm5, xmm5, 0
        
        addps   xmm4, xmm1              // 3 row of the result

        mulps   xmm5, [ecx]

        movaps  xmm1, [eax+48]
        shufps  xmm1, xmm1, 55h
        mulps   xmm1, [ecx+16]

        movaps  xmm0, [eax+48]
        shufps  xmm0, xmm0, 0AAh
        mulps   xmm0, [ecx+32]

        addps   xmm5, xmm1

        movaps  xmm1, [eax+48]
        shufps  xmm1, xmm1, 0FFh
        mulps   xmm1, [ecx+48]

        mov     ecx, pOut
        addps   xmm5, xmm0

        addps   xmm5, xmm1              // 4 row of the result

        movaps  [ecx   ], xmm2
        movaps  [ecx+16], xmm3
        movaps  [ecx+32], xmm4
        movaps  [ecx+48], xmm5
    }
    return pOut;
}

extern "C"
XGMATRIX* WINAPI XGMatrixTranspose
    ( XGMATRIX *pOut, const XGMATRIX *pM )
{
#if DBG
    if(!pOut || !pM)
        return NULL;
#endif

    __asm {
        mov       ecx, pM
        mov       eax, pOut

        movaps    xmm0, [ecx]
        movaps    xmm2, [ecx+16]
        movaps    xmm3, [ecx+32]
        movaps    xmm5, [ecx+48]

        movaps    xmm1, xmm0
        movaps    xmm4, xmm3

        unpcklps  xmm0, xmm2
        unpckhps  xmm1, xmm2
        unpcklps  xmm3, xmm5
        unpckhps  xmm4, xmm5

        movlps    [eax], xmm0
        movlps    [eax+8], xmm3
        movhps    [eax+16], xmm0
        movhps    [eax+24], xmm3
        movlps    [eax+32], xmm1
        movlps    [eax+40], xmm4
        movhps    [eax+48], xmm1
        movhps    [eax+56], xmm4
    }
}

extern "C"
XGMATRIX* WINAPI XGMatrixInverse
    ( XGMATRIX *pOut, float *pfDeterminant, const XGMATRIX *pM )
{
#if DBG
    if(!pOut || !pM)
        return NULL;
#endif

    // XXXlorenmcq - The code was designed to work on a processor with more
    //  than 4 general-purpose registers.  Is there a more optimal way of
    //  doing this on X86?

    float fX00, fX01, fX02;
    float fX10, fX11, fX12;
    float fX20, fX21, fX22;
    float fX30, fX31, fX32;
    float fY01, fY02, fY03, fY12, fY13, fY23;
    float fZ02, fZ03, fZ12, fZ13, fZ22, fZ23, fZ32, fZ33;

#define fX03 fX01
#define fX13 fX11
#define fX23 fX21
#define fX33 fX31
#define fZ00 fX02
#define fZ10 fX12
#define fZ20 fX22
#define fZ30 fX32
#define fZ01 fX03
#define fZ11 fX13
#define fZ21 fX23
#define fZ31 fX33
#define fDet fY01
#define fRcp fY02

    // read 1st two columns of matrix
    fX00 = pM->_11;
    fX01 = pM->_12;
    fX10 = pM->_21;
    fX11 = pM->_22;
    fX20 = pM->_31;
    fX21 = pM->_32;
    fX30 = pM->_41;
    fX31 = pM->_42;

    // compute all six 2x2 determinants of 1st two columns
    fY01 = fX00 * fX11 - fX10 * fX01;
    fY02 = fX00 * fX21 - fX20 * fX01;
    fY03 = fX00 * fX31 - fX30 * fX01;
    fY12 = fX10 * fX21 - fX20 * fX11;
    fY13 = fX10 * fX31 - fX30 * fX11;
    fY23 = fX20 * fX31 - fX30 * fX21;

    // read 2nd two columns of matrix
    fX02 = pM->_13;
    fX03 = pM->_14;
    fX12 = pM->_23;
    fX13 = pM->_24;
    fX22 = pM->_33;
    fX23 = pM->_34;
    fX32 = pM->_43;
    fX33 = pM->_44;

    // compute all 3x3 cofactors for 2nd two columns
    fZ33 = fX02 * fY12 - fX12 * fY02 + fX22 * fY01;
    fZ23 = fX12 * fY03 - fX32 * fY01 - fX02 * fY13;
    fZ13 = fX02 * fY23 - fX22 * fY03 + fX32 * fY02;
    fZ03 = fX22 * fY13 - fX32 * fY12 - fX12 * fY23;
    fZ32 = fX13 * fY02 - fX23 * fY01 - fX03 * fY12;
    fZ22 = fX03 * fY13 - fX13 * fY03 + fX33 * fY01;
    fZ12 = fX23 * fY03 - fX33 * fY02 - fX03 * fY23;
    fZ02 = fX13 * fY23 - fX23 * fY13 + fX33 * fY12;

    // compute all six 2x2 determinants of 2nd two columns
    fY01 = fX02 * fX13 - fX12 * fX03;
    fY02 = fX02 * fX23 - fX22 * fX03;
    fY03 = fX02 * fX33 - fX32 * fX03;
    fY12 = fX12 * fX23 - fX22 * fX13;
    fY13 = fX12 * fX33 - fX32 * fX13;
    fY23 = fX22 * fX33 - fX32 * fX23;

    // read 1st two columns of matrix
    fX00 = pM->_11;
    fX01 = pM->_12;
    fX10 = pM->_21;
    fX11 = pM->_22;
    fX20 = pM->_31;
    fX21 = pM->_32;
    fX30 = pM->_41;
    fX31 = pM->_42;

    // compute all 3x3 cofactors for 1st two columns
    fZ30 = fX11 * fY02 - fX21 * fY01 - fX01 * fY12;
    fZ20 = fX01 * fY13 - fX11 * fY03 + fX31 * fY01;
    fZ10 = fX21 * fY03 - fX31 * fY02 - fX01 * fY23;
    fZ00 = fX11 * fY23 - fX21 * fY13 + fX31 * fY12;
    fZ31 = fX00 * fY12 - fX10 * fY02 + fX20 * fY01;
    fZ21 = fX10 * fY03 - fX30 * fY01 - fX00 * fY13;
    fZ11 = fX00 * fY23 - fX20 * fY03 + fX30 * fY02;
    fZ01 = fX20 * fY13 - fX30 * fY12 - fX10 * fY23;

    // compute 4x4 determinant & its reciprocal
    fDet = fX30 * fZ30 + fX20 * fZ20 + fX10 * fZ10 + fX00 * fZ00;

    if(pfDeterminant)
        *pfDeterminant = fDet;

    fRcp = 1.0f / fDet;

    if(!_finite(fRcp))
        return NULL;


    // multiply all 3x3 cofactors by reciprocal & transpose
    pOut->_11 = fZ00 * fRcp;
    pOut->_12 = fZ10 * fRcp;
    pOut->_13 = fZ20 * fRcp;
    pOut->_14 = fZ30 * fRcp;
    pOut->_21 = fZ01 * fRcp;
    pOut->_22 = fZ11 * fRcp;
    pOut->_23 = fZ21 * fRcp;
    pOut->_24 = fZ31 * fRcp;
    pOut->_31 = fZ02 * fRcp;
    pOut->_32 = fZ12 * fRcp;
    pOut->_33 = fZ22 * fRcp;
    pOut->_34 = fZ32 * fRcp;
    pOut->_41 = fZ03 * fRcp;
    pOut->_42 = fZ13 * fRcp;
    pOut->_43 = fZ23 * fRcp;
    pOut->_44 = fZ33 * fRcp;

    return pOut;
}



extern "C"
XGMATRIX* WINAPI XGMatrixScaling
    ( XGMATRIX *pOut, float sx, float sy, float sz )
{
#if DBG
    if(!pOut)
        return NULL;
#endif

    pOut->_12 = pOut->_13 = pOut->_14 =
    pOut->_21 = pOut->_23 = pOut->_24 =
    pOut->_31 = pOut->_32 = pOut->_34 =
    pOut->_41 = pOut->_42 = pOut->_43 = 0.0f;

    pOut->_11 = sx;
    pOut->_22 = sy;
    pOut->_33 = sz;
    pOut->_44 = 1.0f;
    return pOut;
}

extern "C"
XGMATRIX* WINAPI XGMatrixTranslation
    ( XGMATRIX *pOut, float x, float y, float z )
{
#if DBG
    if(!pOut)
        return NULL;
#endif

    pOut->_12 = pOut->_13 = pOut->_14 =
    pOut->_21 = pOut->_23 = pOut->_24 =
    pOut->_31 = pOut->_32 = pOut->_34 = 0.0f;

    pOut->_11 = pOut->_22 = pOut->_33 = pOut->_44 = 1.0f;

    pOut->_41 = x;
    pOut->_42 = y;
    pOut->_43 = z;
    return pOut;
}


extern "C"
XGMATRIX* WINAPI XGMatrixRotationX
    ( XGMATRIX *pOut, float angle )
{
#if DBG
    if(!pOut)
        return NULL;
#endif

    float s, c;
    sincosf(angle, &s, &c);

    pOut->_11 = 1.0f; pOut->_12 = 0.0f; pOut->_13 = 0.0f; pOut->_14 = 0.0f;
    pOut->_21 = 0.0f; pOut->_22 =    c; pOut->_23 =    s; pOut->_24 = 0.0f;
    pOut->_31 = 0.0f; pOut->_32 =   -s; pOut->_33 =    c; pOut->_34 = 0.0f;
    pOut->_41 = 0.0f; pOut->_42 = 0.0f; pOut->_43 = 0.0f; pOut->_44 = 1.0f;

    return pOut;
}

extern "C"
XGMATRIX* WINAPI XGMatrixRotationY
    ( XGMATRIX *pOut, float angle )
{
#if DBG
    if(!pOut)
        return NULL;
#endif

    float s, c;
    sincosf(angle, &s, &c);

    pOut->_11 =    c; pOut->_12 = 0.0f; pOut->_13 =   -s; pOut->_14 = 0.0f;
    pOut->_21 = 0.0f; pOut->_22 = 1.0f; pOut->_23 = 0.0f; pOut->_24 = 0.0f;
    pOut->_31 =    s; pOut->_32 = 0.0f; pOut->_33 =    c; pOut->_34 = 0.0f;
    pOut->_41 = 0.0f; pOut->_42 = 0.0f; pOut->_43 = 0.0f; pOut->_44 = 1.0f;

    return pOut;
}

extern "C"
XGMATRIX* WINAPI XGMatrixRotationZ
    ( XGMATRIX *pOut, float angle )
{
#if DBG
    if(!pOut)
        return NULL;
#endif

    float s, c;
    sincosf(angle, &s, &c);

    pOut->_11 =    c; pOut->_12 =    s; pOut->_13 = 0.0f; pOut->_14 = 0.0f;
    pOut->_21 =   -s; pOut->_22 =    c; pOut->_23 = 0.0f; pOut->_24 = 0.0f;
    pOut->_31 = 0.0f; pOut->_32 = 0.0f; pOut->_33 = 1.0f; pOut->_34 = 0.0f;
    pOut->_41 = 0.0f; pOut->_42 = 0.0f; pOut->_43 = 0.0f; pOut->_44 = 1.0f;

    return pOut;
}

extern "C"
XGMATRIX* WINAPI XGMatrixRotationAxis
    ( XGMATRIX *pOut, const XGVECTOR3 *pV, float angle )
{
#if DBG
    if(!pOut || !pV)
        return NULL;
#endif

    float s, c;
    sincosf(angle, &s, &c);
    float c1 = 1 - c;

    XGVECTOR3 v = *pV;
    XGVec3Normalize(&v, &v);

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

extern "C"
XGMATRIX* WINAPI XGMatrixRotationQuaternion
    ( XGMATRIX *pOut, const XGQUATERNION *pQ)
{
#if DBG
    if(!pOut || !pQ)
        return NULL;
#endif

    float x2 = pQ->x + pQ->x;
    float y2 = pQ->y + pQ->y;
    float z2 = pQ->z + pQ->z;

    float wx2 = pQ->w * x2;
    float wy2 = pQ->w * y2;
    float wz2 = pQ->w * z2;
    float xx2 = pQ->x * x2;
    float xy2 = pQ->x * y2;
    float xz2 = pQ->x * z2;
    float yy2 = pQ->y * y2;
    float yz2 = pQ->y * z2;
    float zz2 = pQ->z * z2;

    pOut->_11 = 1.0f - yy2 - zz2;
    pOut->_12 = xy2 + wz2;
    pOut->_13 = xz2 - wy2;
    pOut->_14 = 0.0f;

    pOut->_21 = xy2 - wz2;
    pOut->_22 = 1.0f - xx2 - zz2;
    pOut->_23 = yz2 + wx2;
    pOut->_24 = 0.0f;

    pOut->_31 = xz2 + wy2;
    pOut->_32 = yz2 - wx2;
    pOut->_33 = 1.0f - xx2 - yy2;
    pOut->_34 = 0.0f;

    pOut->_41 = 0.0f;
    pOut->_42 = 0.0f;
    pOut->_43 = 0.0f;
    pOut->_44 = 1.0f;

    return pOut;
}

extern "C"
XGMATRIX* WINAPI XGMatrixRotationYawPitchRoll
    ( XGMATRIX *pOut, float yaw, float pitch, float roll )
{
#if DBG
    if(!pOut)
        return NULL;
#endif

    XGQUATERNION q;

    XGQuaternionRotationYawPitchRoll(&q, yaw, pitch, roll);
    XGMatrixRotationQuaternion(pOut, &q);

    return pOut;
}

extern "C"
XGMATRIX* WINAPI XGMatrixTransformation
    ( XGMATRIX *pOut, const XGVECTOR3 *pScalingCenter,
      const XGQUATERNION *pScalingRotation, const XGVECTOR3 *pScaling,
      const XGVECTOR3 *pRotationCenter, const XGQUATERNION *pRotation,
      const XGVECTOR3 *pTranslation)
{
#if DBG
    if(!pOut)
        return NULL;
#endif

    XGMATRIX matS, matR, matRI;

    if (pScaling)
    {
        if (pScalingRotation)
        {
            matS._12 = matS._13 = matS._14 =
            matS._21 = matS._23 = matS._24 =
            matS._31 = matS._32 = matS._34 =
            matS._41 = matS._42 = matS._43 = 0.0f;

            matS._11 = pScaling->x;
            matS._22 = pScaling->y;
            matS._33 = pScaling->z;
            matS._44 = 1.0f;

            XGMatrixRotationQuaternion(&matR, pScalingRotation);


            if (pScalingCenter)
            {
                // SC-1, SR-1, S, SR, SC
                XGMatrixTranspose(&matRI, &matR);
                XGMatrixIdentity(pOut);

                pOut->_41 -= pScalingCenter->x;
                pOut->_42 -= pScalingCenter->y;
                pOut->_43 -= pScalingCenter->z;

                XGMatrixMultiply(pOut, pOut, &matRI);
                XGMatrixMultiply(pOut, pOut, &matS);
                XGMatrixMultiply(pOut, pOut, &matR);

                pOut->_41 += pScalingCenter->x;
                pOut->_42 += pScalingCenter->y;
                pOut->_43 += pScalingCenter->z;
            }
            else
            {
                // SR-1, S, SR
                XGMatrixTranspose(pOut, &matR);
                XGMatrixMultiply(pOut, pOut, &matS);
                XGMatrixMultiply(pOut, pOut, &matR);
            }
        }
        else
        {
            // S
            pOut->_12 = pOut->_13 = pOut->_14 =
            pOut->_21 = pOut->_23 = pOut->_24 =
            pOut->_31 = pOut->_32 = pOut->_34 =
            pOut->_41 = pOut->_42 = pOut->_43 = 0.0f;

            pOut->_11 = pScaling->x;
            pOut->_22 = pScaling->y;
            pOut->_33 = pScaling->z;
            pOut->_44 = 1.0f;
        }

    }
    else
    {
        XGMatrixIdentity(pOut);
    }

    if (pRotation)
    {
        XGMatrixRotationQuaternion(&matR, pRotation);

        if (pRotationCenter)
        {
            // RC-1, R, RC
            pOut->_41 -= pRotationCenter->x;
            pOut->_42 -= pRotationCenter->y;
            pOut->_43 -= pRotationCenter->z;

            XGMatrixMultiply(pOut, pOut, &matR);

            pOut->_41 += pRotationCenter->x;
            pOut->_42 += pRotationCenter->y;
            pOut->_43 += pRotationCenter->z;
        }
        else
        {
            // R
            XGMatrixMultiply(pOut, pOut, &matR);
        }
    }

    if (pTranslation)
    {
        // T
        pOut->_41 += pTranslation->x;
        pOut->_42 += pTranslation->y;
        pOut->_43 += pTranslation->z;
    }

    return pOut;
}

extern "C"
XGMATRIX* WINAPI XGMatrixAffineTransformation
    ( XGMATRIX *pOut, float Scaling, const XGVECTOR3 *pRotationCenter,
      const XGQUATERNION *pRotation, const XGVECTOR3 *pTranslation)
{
#if DBG
    if(!pOut)
        return NULL;
#endif

    // S
    pOut->_12 = pOut->_13 = pOut->_14 =
    pOut->_21 = pOut->_23 = pOut->_24 =
    pOut->_31 = pOut->_32 = pOut->_34 =
    pOut->_41 = pOut->_42 = pOut->_43 = 0.0f;

    pOut->_11 = Scaling;
    pOut->_22 = Scaling;
    pOut->_33 = Scaling;
    pOut->_44 = 1.0f;


    if (pRotation)
    {
        XGMATRIX matR;
        XGMatrixRotationQuaternion(&matR, pRotation);

        if (pRotationCenter)
        {
            // RC-1, R, RC
            pOut->_41 -= pRotationCenter->x;
            pOut->_42 -= pRotationCenter->y;
            pOut->_43 -= pRotationCenter->z;

            XGMatrixMultiply(pOut, pOut, &matR);

            pOut->_41 += pRotationCenter->x;
            pOut->_42 += pRotationCenter->y;
            pOut->_43 += pRotationCenter->z;
        }
        else
        {
            // R
            XGMatrixMultiply(pOut, pOut, &matR);
        }
    }


    if (pTranslation)
    {
        // T
        pOut->_41 += pTranslation->x;
        pOut->_42 += pTranslation->y;
        pOut->_43 += pTranslation->z;
    }

    return pOut;
}

extern "C"
XGMATRIX* WINAPI XGMatrixLookAtRH
    ( XGMATRIX *pOut, const XGVECTOR3 *pEye, const XGVECTOR3 *pAt,
      const XGVECTOR3 *pUp )
{
#if DBG
    if(!pOut || !pEye || !pAt || !pUp)
        return NULL;
#endif

    XGVECTOR3 XAxis, YAxis, ZAxis;

    // Compute direction of gaze. (-Z)
    XGVec3Subtract(&ZAxis, pEye, pAt);
    XGVec3Normalize(&ZAxis, &ZAxis);

    // Compute orthogonal axes from cross product of gaze and pUp vector.
    XGVec3Cross(&XAxis, pUp, &ZAxis);
    XGVec3Normalize(&XAxis, &XAxis);
    XGVec3Cross(&YAxis, &ZAxis, &XAxis);

    // Set rotation and translate by pEye
    pOut->_11 = XAxis.x;
    pOut->_21 = XAxis.y;
    pOut->_31 = XAxis.z;
    pOut->_41 = -XGVec3Dot(&XAxis, pEye);

    pOut->_12 = YAxis.x;
    pOut->_22 = YAxis.y;
    pOut->_32 = YAxis.z;
    pOut->_42 = -XGVec3Dot(&YAxis, pEye);

    pOut->_13 = ZAxis.x;
    pOut->_23 = ZAxis.y;
    pOut->_33 = ZAxis.z;
    pOut->_43 = -XGVec3Dot(&ZAxis, pEye);

    pOut->_14 = 0.0f;
    pOut->_24 = 0.0f;
    pOut->_34 = 0.0f;
    pOut->_44 = 1.0f;

    return pOut;
}

extern "C"
XGMATRIX* WINAPI XGMatrixLookAtLH
    ( XGMATRIX *pOut, const XGVECTOR3 *pEye, const XGVECTOR3 *pAt,
      const XGVECTOR3 *pUp )
{
#if DBG
    if(!pOut || !pEye || !pAt || !pUp)
        return NULL;
#endif

    XGVECTOR3 XAxis, YAxis, ZAxis;

    // Compute direction of gaze. (+Z)
    XGVec3Subtract(&ZAxis, pAt, pEye);
    XGVec3Normalize(&ZAxis, &ZAxis);

    // Compute orthogonal axes from cross product of gaze and pUp vector.
    XGVec3Cross(&XAxis, pUp, &ZAxis);
    XGVec3Normalize(&XAxis, &XAxis);
    XGVec3Cross(&YAxis, &ZAxis, &XAxis);

    // Set rotation and translate by pEye
    pOut->_11 = XAxis.x;
    pOut->_21 = XAxis.y;
    pOut->_31 = XAxis.z;
    pOut->_41 = -XGVec3Dot(&XAxis, pEye);

    pOut->_12 = YAxis.x;
    pOut->_22 = YAxis.y;
    pOut->_32 = YAxis.z;
    pOut->_42 = -XGVec3Dot(&YAxis, pEye);

    pOut->_13 = ZAxis.x;
    pOut->_23 = ZAxis.y;
    pOut->_33 = ZAxis.z;
    pOut->_43 = -XGVec3Dot(&ZAxis, pEye);

    pOut->_14 = 0.0f;
    pOut->_24 = 0.0f;
    pOut->_34 = 0.0f;
    pOut->_44 = 1.0f;

    return pOut;
}

extern "C"
XGMATRIX* WINAPI XGMatrixPerspectiveRH
    ( XGMATRIX *pOut, float w, float h, float zn, float zf )
{
#if DBG
    if(!pOut)
        return NULL;
#endif

    pOut->_11 = 2.0f * zn / w;
    pOut->_12 = 0.0f;
    pOut->_13 = 0.0f;
    pOut->_14 = 0.0f;

    pOut->_21 = 0.0f;
    pOut->_22 = 2.0f * zn / h;
    pOut->_23 = 0.0f;
    pOut->_24 = 0.0f;

    pOut->_31 = 0.0f;
    pOut->_32 = 0.0f;
    pOut->_33 = zf / (zn - zf);
    pOut->_34 = -1.0f;

    pOut->_41 = 0.0f;
    pOut->_42 = 0.0f;
    pOut->_43 = pOut->_33 * zn;
    pOut->_44 = 0.0f;

    return pOut;
}

extern "C"
XGMATRIX* WINAPI XGMatrixPerspectiveLH
    ( XGMATRIX *pOut, float w, float h, float zn, float zf )
{
#if DBG
    if(!pOut)
        return NULL;
#endif

    pOut->_11 = 2.0f * zn / w;
    pOut->_12 = 0.0f;
    pOut->_13 = 0.0f;
    pOut->_14 = 0.0f;

    pOut->_21 = 0.0f;
    pOut->_22 = 2.0f * zn / h;
    pOut->_23 = 0.0f;
    pOut->_24 = 0.0f;

    pOut->_31 = 0.0f;
    pOut->_32 = 0.0f;
    pOut->_33 = zf / (zf - zn);
    pOut->_34 = 1.0f;

    pOut->_41 = 0.0f;
    pOut->_42 = 0.0f;
    pOut->_43 = -pOut->_33 * zn;
    pOut->_44 = 0.0f;

    return pOut;
}

extern "C"
XGMATRIX* WINAPI XGMatrixPerspectiveFovRH
    ( XGMATRIX *pOut, float fovy, float aspect, float zn, float zf )
{
#if DBG
    if(!pOut)
        return NULL;
#endif

    float s, c;
    sincosf(0.5f * fovy, &s, &c);

    float h = c / s;
    float w = h / aspect;

    pOut->_11 = w;
    pOut->_12 = 0.0f;
    pOut->_13 = 0.0f;
    pOut->_14 = 0.0f;

    pOut->_21 = 0.0f;
    pOut->_22 = h;
    pOut->_23 = 0.0f;
    pOut->_24 = 0.0f;

    pOut->_31 = 0.0f;
    pOut->_32 = 0.0f;
    pOut->_33 = zf / (zn - zf);
    pOut->_34 = -1.0f;

    pOut->_41 = 0.0f;
    pOut->_42 = 0.0f;
    pOut->_43 = pOut->_33 * zn;
    pOut->_44 = 0.0f;

    return pOut;
}

extern "C"
XGMATRIX* WINAPI XGMatrixPerspectiveFovLH
    ( XGMATRIX *pOut, float fovy, float aspect, float zn, float zf )
{
#if DBG
    if(!pOut)
        return NULL;
#endif

    float s, c;
    sincosf(0.5f * fovy, &s, &c);

    float h = c / s;
    float w = h / aspect;

    pOut->_11 = w;
    pOut->_12 = 0.0f;
    pOut->_13 = 0.0f;
    pOut->_14 = 0.0f;

    pOut->_21 = 0.0f;
    pOut->_22 = h;
    pOut->_23 = 0.0f;
    pOut->_24 = 0.0f;

    pOut->_31 = 0.0f;
    pOut->_32 = 0.0f;
    pOut->_33 = zf / (zf - zn);
    pOut->_34 = 1.0f;

    pOut->_41 = 0.0f;
    pOut->_42 = 0.0f;
    pOut->_43 = -pOut->_33 * zn;
    pOut->_44 = 0.0f;

    return pOut;
}

extern "C"
XGMATRIX* WINAPI XGMatrixPerspectiveOffCenterRH
    ( XGMATRIX *pOut, float l, float r, float b, float t, float zn,
      float zf )
{
#if DBG
    if(!pOut)
        return NULL;
#endif

    float wInv = 1.0f / (r - l);
    float hInv = 1.0f / (t - b);

    pOut->_11 = 2.0f * zn * wInv;
    pOut->_12 = 0.0f;
    pOut->_13 = 0.0f;
    pOut->_14 = 0.0f;

    pOut->_21 = 0.0f;
    pOut->_22 = 2.0f * zn * hInv;
    pOut->_23 = 0.0f;
    pOut->_24 = 0.0f;

    pOut->_31 = (l + r) * wInv;
    pOut->_32 = (t + b) * hInv;
    pOut->_33 = zf / (zn - zf);
    pOut->_34 = -1.0f;

    pOut->_41 = 0.0f;
    pOut->_42 = 0.0f;
    pOut->_43 = pOut->_33 * zn;
    pOut->_44 = 0.0f;

    return pOut;
}

extern "C"
XGMATRIX* WINAPI XGMatrixPerspectiveOffCenterLH
    ( XGMATRIX *pOut, float l, float r, float b, float t, float zn,
      float zf )
{
#if DBG
    if(!pOut)
        return NULL;
#endif

    float wInv = 1.0f / (r - l);
    float hInv = 1.0f / (t - b);

    pOut->_11 = 2.0f * zn * wInv;
    pOut->_12 = 0.0f;
    pOut->_13 = 0.0f;
    pOut->_14 = 0.0f;

    pOut->_21 = 0.0f;
    pOut->_22 = 2.0f * zn * hInv;
    pOut->_23 = 0.0f;
    pOut->_24 = 0.0f;

    pOut->_31 = -(l + r) * wInv;
    pOut->_32 = -(t + b) * hInv;
    pOut->_33 = zf / (zf - zn);
    pOut->_34 = 1.0f;

    pOut->_41 = 0.0f;
    pOut->_42 = 0.0f;
    pOut->_43 = -pOut->_33 * zn;
    pOut->_44 = 0.0f;

    return pOut;
}

extern "C"
XGMATRIX* WINAPI XGMatrixOrthoRH
    ( XGMATRIX *pOut, float w, float h, float zn, float zf )
{
#if DBG
    if(!pOut)
        return NULL;
#endif

    pOut->_11 = 2.0f / w;
    pOut->_12 = 0.0f;
    pOut->_13 = 0.0f;
    pOut->_14 = 0.0f;

    pOut->_21 = 0.0f;
    pOut->_22 = 2.0f / h;
    pOut->_23 = 0.0f;
    pOut->_24 = 0.0f;

    pOut->_31 = 0.0f;
    pOut->_32 = 0.0f;
    pOut->_33 = 1.0f / (zn - zf);
    pOut->_34 = 0.0f;

    pOut->_41 = 0.0f;
    pOut->_42 = 0.0f;
    pOut->_43 = pOut->_33 * zn;
    pOut->_44 = 1.0f;

    return pOut;
}

extern "C"
XGMATRIX* WINAPI XGMatrixOrthoLH
    ( XGMATRIX *pOut, float w, float h, float zn, float zf )
{
#if DBG
    if(!pOut)
        return NULL;
#endif

    pOut->_11 = 2.0f / w;
    pOut->_12 = 0.0f;
    pOut->_13 = 0.0f;
    pOut->_14 = 0.0f;

    pOut->_21 = 0.0f;
    pOut->_22 = 2.0f / h;
    pOut->_23 = 0.0f;
    pOut->_24 = 0.0f;

    pOut->_31 = 0.0f;
    pOut->_32 = 0.0f;
    pOut->_33 = 1.0f / (zf - zn);
    pOut->_34 = 0.0f;

    pOut->_41 = 0.0f;
    pOut->_42 = 0.0f;
    pOut->_43 = -pOut->_33 * zn;
    pOut->_44 = 1.0f;

    return pOut;
}

extern "C"
XGMATRIX* WINAPI XGMatrixOrthoOffCenterRH
    ( XGMATRIX *pOut, float l, float r, float b, float t, float zn,
      float zf )
{
#if DBG
    if(!pOut)
        return NULL;
#endif

    float wInv = 1.0f / (r - l);
    float hInv = 1.0f / (t - b);

    pOut->_11 = 2.0f * wInv;
    pOut->_12 = 0.0f;
    pOut->_13 = 0.0f;
    pOut->_14 = 0.0f;

    pOut->_21 = 0.0f;
    pOut->_22 = 2.0f * hInv;
    pOut->_23 = 0.0f;
    pOut->_24 = 0.0f;

    pOut->_31 = 0.0f;
    pOut->_32 = 0.0f;
    pOut->_33 = 1.0f / (zn - zf);
    pOut->_34 = 0.0f;

    pOut->_41 = -(l + r) * wInv;
    pOut->_42 = -(t + b) * hInv;
    pOut->_43 = pOut->_33 * zn;
    pOut->_44 = 1.0f;

    return pOut;
}

extern "C"
XGMATRIX* WINAPI XGMatrixOrthoOffCenterLH
    ( XGMATRIX *pOut, float l, float r, float b, float t, float zn,
      float zf )
{
#if DBG
    if(!pOut)
        return NULL;
#endif

    float wInv = 1.0f / (r - l);
    float hInv = 1.0f / (t - b);

    pOut->_11 = 2.0f * wInv;
    pOut->_12 = 0.0f;
    pOut->_13 = 0.0f;
    pOut->_14 = 0.0f;

    pOut->_21 = 0.0f;
    pOut->_22 = 2.0f * hInv;
    pOut->_23 = 0.0f;
    pOut->_24 = 0.0f;

    pOut->_31 = 0.0f;
    pOut->_32 = 0.0f;
    pOut->_33 = 1.0f / (zf - zn);
    pOut->_34 = 0.0f;

    pOut->_41 = -(l + r) * wInv;
    pOut->_42 = -(t + b) * hInv;
    pOut->_43 = -pOut->_33 * zn;
    pOut->_44 = 1.0f;

    return pOut;
}

extern "C"
XGMATRIX* WINAPI XGMatrixShadow
    ( XGMATRIX *pOut, const XGVECTOR4 *pLight,
      const XGPLANE *pPlane )
{
#if DBG
    if(!pOut || !pLight || !pPlane)
        return NULL;
#endif

    XGPLANE p;
    XGPlaneNormalize(&p, pPlane);
    float dot = XGPlaneDot(&p, pLight);
    p = -p;

    pOut->_11 = p.a * pLight->x + dot;
    pOut->_21 = p.b * pLight->x;
    pOut->_31 = p.c * pLight->x;
    pOut->_41 = p.d * pLight->x;

    pOut->_12 = p.a * pLight->y;
    pOut->_22 = p.b * pLight->y + dot;
    pOut->_32 = p.c * pLight->y;
    pOut->_42 = p.d * pLight->y;

    pOut->_13 = p.a * pLight->z;
    pOut->_23 = p.b * pLight->z;
    pOut->_33 = p.c * pLight->z + dot;
    pOut->_43 = p.d * pLight->z;

    pOut->_14 = p.a * pLight->w;
    pOut->_24 = p.b * pLight->w;
    pOut->_34 = p.c * pLight->w;
    pOut->_44 = p.d * pLight->w + dot;

    return pOut;
}


extern "C"
XGMATRIX* WINAPI XGMatrixReflect
    ( XGMATRIX *pOut, const XGPLANE *pPlane )
{
#if DBG
    if(!pOut || !pPlane)
        return NULL;
#endif

    XGPLANE p;
    XGPlaneNormalize(&p, pPlane);

    float fa = -2.0f * p.a;
    float fb = -2.0f * p.b;
    float fc = -2.0f * p.c;

    pOut->_11 = fa * p.a + 1.0f;
    pOut->_12 = fb * p.a;
    pOut->_13 = fc * p.a;
    pOut->_14 = 0.0f;

    pOut->_21 = fa * p.b;
    pOut->_22 = fb * p.b + 1.0f;
    pOut->_23 = fc * p.b;
    pOut->_24 = 0.0f;

    pOut->_31 = fa * p.c;
    pOut->_32 = fb * p.c;
    pOut->_33 = fc * p.c + 1.0f;
    pOut->_34 = 0.0f;

    pOut->_41 = fa * p.d;
    pOut->_42 = fb * p.d;
    pOut->_43 = fc * p.d;
    pOut->_44 = 1.0f;

    return pOut;
}


//--------------------------
// Quaternion
//--------------------------

extern "C"
void WINAPI XGQuaternionToAxisAngle
    ( const XGQUATERNION *pQ, XGVECTOR3 *pAxis, float *pAngle )
{
#if DBG
    if(!pQ)
        return;
#endif

    // expects unit quaternions!
	// q = cos(A/2), sin(A/2) * v

    float lsq = XGQuaternionLengthSq(pQ);

    if(lsq > EPSILON * EPSILON)
    {
        if(pAxis)
        {
            float scale = 1.0f / sqrtf(lsq);
            pAxis->x = pQ->x * scale;
            pAxis->y = pQ->y * scale;
            pAxis->z = pQ->z * scale;
        }

        if(pAngle)
            *pAngle = 2.0f * acosf(pQ->w);

    }
    else
    {
        if(pAxis)
        {
            pAxis->x = 1.0;
            pAxis->y = 0.0;
            pAxis->z = 0.0;
        }

        if(pAngle)
            *pAngle = 0.0f;
    }
}

extern "C"
XGQUATERNION* WINAPI XGQuaternionRotationMatrix
    ( XGQUATERNION *pOut, const XGMATRIX *pM)
{
#if DBG
    if(!pOut || !pM)
        return NULL;
#endif

    float w2 = pM->_11 + pM->_22 + pM->_33; // 4w2 - 1

    if(w2 > 0.0f)
    {
        pOut->w = 0.5f * sqrtf(w2 + 1.0f);
        float wInv = 0.25f / pOut->w;

        pOut->x = (pM->_23 - pM->_32) * wInv; // 4xw / 4w
        pOut->y = (pM->_31 - pM->_13) * wInv; // 4yw / 4w
        pOut->z = (pM->_12 - pM->_21) * wInv; // 4zw / 4w
    }
    else
    {
        const UINT next[3] = { 1, 2, 0 };
        float *pf = (float *) pOut;
        float n2[3];
        UINT i, j, k;

        n2[0] =   pM->_11 - pM->_22 - pM->_33; // 4x2 - 1
        n2[1] = - pM->_11 + pM->_22 - pM->_33; // 4y2 - 1
        n2[2] = - pM->_11 - pM->_22 + pM->_33; // 4z2 - 1

        i = (n2[0] >= n2[1]) ? 0 : 1;
        i = (n2[i] >= n2[2]) ? i : 2;

        j = next[i];
        k = next[j];

        pf[i] = 0.5f * sqrtf(n2[i] + 1.0f); // x
        float nInv = 0.25f / pf[i];

        pf[j] = (pM->m[i][j] + pM->m[j][i]) * nInv; // 4yx / 4x
        pf[k] = (pM->m[k][i] + pM->m[i][k]) * nInv; // 4zx / 4x
        pf[3] = (pM->m[j][k] - pM->m[k][j]) * nInv; // 4wz / 4x
    }

    return pOut;
}

extern "C"
XGQUATERNION* WINAPI XGQuaternionRotationAxis
    ( XGQUATERNION *pOut, const XGVECTOR3 *pV, float angle )
{
#if DBG
    if(!pOut || !pV)
        return NULL;
#endif

    XGVECTOR3 v;
    XGVec3Normalize(&v, pV);

    float s;
    sincosf(0.5f * angle, &s, &pOut->w);

    pOut->x = v.x * s;
    pOut->y = v.y * s;
    pOut->z = v.z * s;

    return pOut;
}

extern "C"
XGQUATERNION* WINAPI XGQuaternionRotationYawPitchRoll
    ( XGQUATERNION *pOut, float yaw, float pitch, float roll )
{
#if DBG
    if(!pOut)
        return NULL;
#endif

    //  Roll first, about axis the object is facing, then
    //  pitch upward, then yaw to face into the new heading

    float SR, CR, SP, CP, SY, CY;

    sincosf(0.5f * roll,  &SR, &CR);
    sincosf(0.5f * pitch, &SP, &CP);
    sincosf(0.5f * yaw,   &SY, &CY);

    pOut->x = CY*SP*CR + SY*CP*SR;
    pOut->y = SY*CP*CR - CY*SP*SR;
    pOut->z = CY*CP*SR - SY*SP*CR;
    pOut->w = CY*CP*CR + SY*SP*SR;

    return pOut;
}

extern "C"
XGQUATERNION* WINAPI XGQuaternionMultiply
    ( XGQUATERNION *pOut, const XGQUATERNION *pQ1,
      const XGQUATERNION *pQ2 )
{
    const UINT n = 0x80000000; 

#if DBG
    if(!pOut || !pQ1 || !pQ2)
        return NULL;
#endif

    __asm {
        movss   xmm0, [n]
        mov     ecx, pQ1
        movups  xmm1, [ecx]
        mov     edx, pQ2

        movaps  xmm3, xmm1
        movss   xmm2, [edx+12]
        shufps  xmm2, xmm2, 0
        mulps   xmm3, xmm2

        shufps  xmm0, xmm0, 0x11
        movaps  xmm4, xmm1
        shufps  xmm4, xmm4, 0x1B
        movss   xmm2, [edx]
        shufps  xmm2, xmm2, 0
        mulps   xmm4, xmm2
        xorps   xmm4, xmm0
        addps   xmm3, xmm4

        shufps  xmm0, xmm0, 0x50
        movaps  xmm4, xmm1
        shufps  xmm4, xmm4, 0x4E
        movss   xmm2, [edx+4]
        shufps  xmm2, xmm2, 0
        mulps   xmm4, xmm2
        xorps   xmm4, xmm0
        addps   xmm3, xmm4
        
        shufps  xmm0, xmm0, 0x82
        movaps  xmm4, xmm1
        shufps  xmm4, xmm4, 0xB1
        movss   xmm2, [edx+8]
        shufps  xmm2, xmm2, 0
        mulps   xmm4, xmm2
        xorps   xmm4, xmm0
        addps   xmm3, xmm4

        mov     eax, pOut
        movups  [eax], xmm3
    }
}

extern "C"
XGQUATERNION* WINAPI XGQuaternionNormalize
    ( XGQUATERNION *pOut, const XGQUATERNION *pQ )
{
#if DBG
    if(!pOut || !pQ)
        return NULL;
#endif

    float f = XGQuaternionLengthSq(pQ);

    if(WithinEpsilon(f, 1.0f))
    {
        if(pOut != pQ)
            *pOut = *pQ;
    }
    else if(f > EPSILON * EPSILON)
    {
        *pOut = *pQ / sqrtf(f);
    }
    else
    {
        pOut->x = 0.0f;
        pOut->y = 0.0f;
        pOut->z = 0.0f;
        pOut->w = 0.0f;
    }

    return pOut;
}

extern "C"
XGQUATERNION* WINAPI XGQuaternionInverse
    ( XGQUATERNION *pOut, const XGQUATERNION *pQ )
{
#if DBG
    if(!pOut || !pQ)
        return NULL;
#endif

    float f = XGQuaternionLengthSq(pQ);

    if(f > EPSILON*EPSILON)
    {
        XGQuaternionConjugate(pOut, pQ);

        if(!WithinEpsilon(f, 1.0f))
            *pOut /= f;
    }
    else
    {
        pOut->x = 0.0f;
        pOut->y = 0.0f;
        pOut->z = 0.0f;
        pOut->w = 0.0f;
    }

    return pOut;
}

extern "C"
XGQUATERNION* WINAPI XGQuaternionLn
    ( XGQUATERNION *pOut, const XGQUATERNION *pQ )
{
#if DBG
    if(!pOut || !pQ)
        return NULL;
#endif

    // expects unit quaternions!
    // q = (cos(theta), sin(theta) * v); ln(q) = (0, theta * v)

    float theta, s;

    if(pQ->w < 1.0f)
    {
        theta = acosf(pQ->w);
        s = sinf(theta);

        if(!WithinEpsilon(s, 0.0f))
        {
            float scale = theta / s;
            pOut->x = pQ->x * scale;
            pOut->y = pQ->y * scale;
            pOut->z = pQ->z * scale;
            pOut->w = 0.0f;
        }
        else
        {
            pOut->x = pQ->x;
            pOut->y = pQ->y;
            pOut->z = pQ->z;
            pOut->w = 0.0f;
        }
    }
    else
    {
        pOut->x = pQ->x;
        pOut->y = pQ->y;
        pOut->z = pQ->z;
        pOut->w = 0.0f;
    }

    return pOut;
}

extern "C"
XGQUATERNION* WINAPI XGQuaternionExp
    ( XGQUATERNION *pOut, const XGQUATERNION *pQ )
{
#if DBG
    if(!pOut || !pQ)
        return NULL;
#endif

    // expects pure quaternions! (w == 0)
    // q = (0, theta * v) ; exp(q) = (cos(theta), sin(theta) * v)

    float theta, s;

    theta = sqrtf(pQ->x * pQ->x + pQ->y * pQ->y + pQ->z * pQ->z);
    sincosf(theta, &s, &pOut->w);

    if(WithinEpsilon(s, 0.0f))
    {
        if(pOut != pQ)
        {
            pOut->x = pQ->x;
            pOut->y = pQ->y;
            pOut->z = pQ->z;
        }
    }
    else
    {
        s /= theta;

        pOut->x = pQ->x * s;
        pOut->y = pQ->y * s;
        pOut->z = pQ->z * s;
    }

    return pOut;
}

extern "C"
XGQUATERNION* WINAPI XGQuaternionSlerp
    ( XGQUATERNION *pOut, const XGQUATERNION *pQ1,
      const XGQUATERNION *pQ2, float b )
{
#if DBG
    if(!pOut || !pQ1 || !pQ2)
        return NULL;
#endif

    // expects unit quaternions!
    float a, c, flip, s, omega, sInv;

    a = 1.0f - b;
    c = XGQuaternionDot(pQ1, pQ2);
    flip = (c >= 0.0f) ? 1.0f : -1.0f;
    c *= flip;

	if(1.0f - c > EPSILON) {
        s = sqrtf(1.0f - c * c);
 		omega = atan2f(s, c);
 		sInv = 1.0f / s;

 		a = sinf(a * omega) * sInv;
 		b = sinf(b * omega) * sInv;
 	}

    b *= flip;

    __asm {
        movss   xmm0, a
        shufps  xmm0, xmm0, 0
        mov     edx, pQ1
        movups  xmm1, [edx]
        mulps   xmm1, xmm0

        movss   xmm0, b
        shufps  xmm0, xmm0, 0
        mov     edx, pQ2
        movups  xmm2, [edx]
        mulps   xmm2, xmm0

        addps   xmm1, xmm2

        mov     eax, pOut
        movups  [eax], xmm1
    }
}

extern "C"
XGQUATERNION* WINAPI XGQuaternionSquad
    ( XGQUATERNION *pOut, const XGQUATERNION *pQ1,
      const XGQUATERNION *pQ2, const XGQUATERNION *pQ3,
      const XGQUATERNION *pQ4, float t )
{
#if DBG
    if(!pOut || !pQ1 || !pQ2 || !pQ3 || !pQ4)
        return NULL;
#endif

    // expects unit quaternions!
    XGQUATERNION QA, QB;

    XGQuaternionSlerp(&QA, pQ1, pQ4, t);
    XGQuaternionSlerp(&QB, pQ2, pQ3, t);
    XGQuaternionSlerp(pOut, &QA, &QB, 2.0f * t * (1.0f - t));

    return pOut;
}

extern "C"
XGQUATERNION* WINAPI XGQuaternionBaryCentric
    ( XGQUATERNION *pOut, const XGQUATERNION *pQ1,
      const XGQUATERNION *pQ2, const XGQUATERNION *pQ3,
      float f, float g )
{
#if DBG
    if(!pOut || !pQ1 || !pQ2 || !pQ3)
        return NULL;
#endif

    // expects unit quaternions!
    XGQUATERNION QA, QB;
    float s = f + g;

    if(WithinEpsilon(s, 0.0f))
    {
        if(pOut != pQ1)
            *pOut = *pQ1;
    }
    else
    {
        XGQuaternionSlerp(&QA, pQ1, pQ2, s);
        XGQuaternionSlerp(&QB, pQ1, pQ3, s);
        XGQuaternionSlerp(pOut, &QA, &QB, g / s);
    }

    return pOut;
}

//--------------------------
// Plane
//--------------------------

extern "C"
XGPLANE* WINAPI XGPlaneNormalize
    ( XGPLANE *pOut, const XGPLANE *pP )
{
#if DBG
    if(!pOut || !pP)
        return NULL;
#endif

    float f = pP->a * pP->a + pP->b * pP->b + pP->c * pP->c;

    if(WithinEpsilon(f, 1.0f))
    {
        if(pOut != pP)
            *pOut = *pP;
    }
    else if(f > EPSILON * EPSILON)
    {
        float fInv = 1.0f / sqrtf(f);

        pOut->a = pP->a * fInv;
        pOut->b = pP->b * fInv;
        pOut->c = pP->c * fInv;
        pOut->d = pP->d * fInv;
    }
    else
    {
        pOut->a = 0.0f;
        pOut->b = 0.0f;
        pOut->c = 0.0f;
        pOut->d = 0.0f;
    }

    return pOut;
}

extern "C"
XGVECTOR3* WINAPI XGPlaneIntersectLine
    ( XGVECTOR3 *pOut, const XGPLANE *pP, const XGVECTOR3 *pV1,
      const XGVECTOR3 *pV2)
{
#if DBG
    if(!pOut || !pP || !pV1 || !pV2)
        return NULL;
#endif

    float d =  XGPlaneDotNormal(pP, pV1) - XGPlaneDotNormal(pP, pV2);

    if(d == 0.0f)
        return NULL;

    float f = XGPlaneDotCoord(pP, pV1) / d;

    if(!_finite(f))
        return NULL;

    XGVec3Lerp(pOut, pV1, pV2, f);
    return pOut;
}

extern "C"
XGPLANE* WINAPI XGPlaneFromPointNormal
    ( XGPLANE *pOut, const XGVECTOR3 *pPoint, const XGVECTOR3 *pNormal)
{
#if DBG
    if(!pOut || !pPoint || !pNormal)
        return NULL;
#endif

    pOut->a = pNormal->x;
    pOut->b = pNormal->y;
    pOut->c = pNormal->z;
    pOut->d = -XGVec3Dot(pPoint, pNormal);
    return pOut;
}

extern "C"
XGPLANE* WINAPI XGPlaneFromPoints
    ( XGPLANE *pOut, const XGVECTOR3 *pV1, const XGVECTOR3 *pV2,
      const XGVECTOR3 *pV3)
{
#if DBG
    if(!pOut || !pV1 || !pV2 || !pV3)
        return NULL;
#endif

    XGVECTOR3 V12 = *pV1 - *pV2;
    XGVECTOR3 V13 = *pV1 - *pV3;

    XGVec3Cross((XGVECTOR3 *) pOut, &V12, &V13);
    XGVec3Normalize((XGVECTOR3 *) pOut, (XGVECTOR3 *) pOut);

    pOut->d = -XGPlaneDotNormal(pOut, pV1);
    return pOut;
}

extern "C"
XGPLANE* WINAPI XGPlaneTransform
    ( XGPLANE *pOut, const XGPLANE *pP, const XGMATRIX *pM )
{
#if DBG
    if(!pOut || !pP || !pM)
        return NULL;
#endif

    XGPLANE P;
    XGPlaneNormalize(&P, pP);

    XGVECTOR3 V(-P.a * P.d, -P.b * P.d, -P.c * P.d);
    XGVec3TransformCoord(&V, &V, pM);

    XGVec3TransformNormal((XGVECTOR3 *) pOut, (const XGVECTOR3 *) &P, pM);
    XGVec3Normalize((XGVECTOR3 *) pOut, (const XGVECTOR3 *) pOut);

    pOut->d = -XGPlaneDotNormal(pOut, &V);
    return pOut;
}


//--------------------------
// Color
//--------------------------

extern "C"
XGCOLOR* WINAPI XGColorAdjustSaturation
    (XGCOLOR *pOut, const XGCOLOR *pC, float s)
{
#if DBG
    if(!pOut || !pC)
        return NULL;
#endif

    // Approximate values for each component's contribution to luminance.
    // (Based upon the NTSC standard described in the comp.graphics.algorithms
    // colorspace FAQ)
    float grey = pC->r * 0.2125f + pC->g * 0.7154f + pC->b * 0.0721f;

    pOut->r = grey + s * (pC->r - grey);
    pOut->g = grey + s * (pC->g - grey);
    pOut->b = grey + s * (pC->b - grey);
    pOut->a = pC->a;
    return pOut;
}

extern "C"
XGCOLOR* WINAPI XGColorAdjustContrast
    (XGCOLOR *pOut, const XGCOLOR *pC, float c)
{
#if DBG
    if(!pOut || !pC)
        return NULL;
#endif

    pOut->r = 0.5f + c * (pC->r - 0.5f);
    pOut->g = 0.5f + c * (pC->g - 0.5f);
    pOut->b = 0.5f + c * (pC->b - 0.5f);
    pOut->a = pC->a;
    return pOut;
}

#pragma warning(default:4035)

} // namespace XGRAPHICS

