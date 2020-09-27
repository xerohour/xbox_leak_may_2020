/*
 * Copyright 1993-1998 NVIDIA, Corporation.  All rights reserved.
 *
 * THE INFORMATION CONTAINED HEREIN IS PROPRIETARY AND CONFIDENTIAL TO
 * NVIDIA, CORPORATION.  USE, REPRODUCTION OR DISCLOSURE TO ANY THIRD PARTY
 * IS SUBJECT TO WRITTEN PRE-APPROVAL BY NVIDIA, CORPORATION.
 */

/********************************* Direct 3D *******************************\
*                                                                           *
* Module: d3dMath.cpp                                                       *
*     matrix, vector, and other math routines                               *
*                                                                           *
*****************************************************************************
*                                                                           *
* History:                                                                  *
*       Craig Duttweiler    (bertrem)   09Sep99     created                 *
*                                                                           *
\***************************************************************************/

// PERF: Can we use some of ATG's work here?

#include "precomp.hpp"

#ifdef STARTUPANIMATION
namespace D3DK
#else
namespace D3D
#endif
{
 
// convert an FP representation into a DWORD or vice-versa
#define DWORD_FROM_FLOAT(fp) (*(DWORD *)&(fp))
#define FLOAT_FROM_DWORD(dw) (*(float *)&(dw))

#define FP_SIGN_BIT(fp) (DWORD_FROM_FLOAT(fp)&0x80000000)
#define FP_ABS_BITS(fp) (DWORD_FROM_FLOAT(fp)&0x7FFFFFFF)
#define FP_EXP_BITS(fp) (DWORD_FROM_FLOAT(fp)&0x7F800000)
#define FP_MAN_BITS(fp) (DWORD_FROM_FLOAT(fp)&0x007FFFFF)

#define FP_EXP_VAL(fp)  (FP_EXP_BITS(fp) >> 27)
#define FP_MAN_VAL(fp)  (FP_MAN_BITS(fp) >>  0)

//---------------------------------------------------------------------------

// returns  1 / sqrt(x)

static float _0_47 = 0.47f;
static float _1_47 = 1.47f;

float JBInvSqrt (const float x)
{
    DWORD y;
    float r;

    _asm
    {
        mov     eax, 07F000000h+03F800000h  // (ONE_AS_INTEGER<<1) + ONE_AS_INTEGER
        sub     eax, x
        sar     eax, 1

        mov     y, eax                      // y
        fld     _0_47                       // 0.47
        fmul    DWORD PTR x                 // x*0.47

        fld     DWORD PTR y
        fld     st(0)                       // y y x*0.47
        fmul    st(0), st(1)                // y*y y x*0.47

        fld     _1_47                       // 1.47 y*y y x*0.47
        fxch    st(3)                       // x*0.47 y*y y 1.47
        fmulp   st(1), st(0)                // x*0.47*y*y y 1.47
        fsubp   st(2), st(0)                // y 1.47-x*0.47*y*y
        fmulp   st(1), st(0)                // result
        fstp    y
    }

    y &= 0x7FFFFFFF;  // make it positive
    r = Floatify(y);

    // optional
    r = (3.0f - x * (r * r)) * r * 0.5f;    // remove for low accuracy

    return (r);
}

//---------------------------------------------------------------------------

//  sqrt(x) = x * (1/sqrt(x))

float nvSqrt (float x)
{
    return (x*JBInvSqrt(x));
}

//---------------------------------------------------------------------------

//  1/x = 1 / sqrt(x*x) plus sign bit

float nvInv (float x)
{
    DWORD dwSign = *(DWORD *)&x & 0x80000000;
    float invSqRt = JBInvSqrt (x*x);
    DWORD dwInv = dwSign | *(DWORD *)&invSqRt;
    return (Floatify(dwInv));
}

//---------------------------------------------------------------------------

// scale a 3-component vector

void ScaleVector3 (D3DVECTOR *out, CONST D3DVECTOR *v1, FLOAT scale)
{
    out->x = scale * v1->x;
    out->y = scale * v1->y;
    out->z = scale * v1->z;
}

//---------------------------------------------------------------------------

// add two 3-component vectors

void AddVectors3 (D3DVECTOR *out, CONST D3DVECTOR *v1, CONST D3DVECTOR *v2)
{
    out->x = v1->x + v2->x;
    out->y = v1->y + v2->y;
    out->z = v1->z + v2->z;
}

//---------------------------------------------------------------------------

// return the square of the magnitude of a 3-component vectors

FLOAT SquareMagnitude3 (CONST D3DVECTOR *v)
{
    return (v->x*v->x + v->y*v->y + v->z*v->z);
}

//---------------------------------------------------------------------------

// normalize a 3-component vector

void NormalizeVector3 (D3DVECTOR *v)
{
    FLOAT invmag = JBInvSqrt (SquareMagnitude3(v));
    ASSERT (invmag > 0);
    v->x *= invmag;
    v->y *= invmag;
    v->z *= invmag;
}

//---------------------------------------------------------------------------

// Multiplies row vector [v.x v.y v.z w] by 4x3 matrix m
// res and v should not be the same

void XformBy4x3 (D3DVECTOR *res, CONST D3DVECTOR *v, FLOAT w, CONST D3DMATRIX *m)
{
    ASSERT (res != v);

#ifdef _X86_
    __asm {
        mov     eax, v
        mov     ecx, m
        movss   xmm2, [eax]         // v->x
        shufps  xmm2, xmm2, 0       // fill xmm2 with v->x
        mulps   xmm2, [ecx]         // v->x*m->_11, v->x*m->_12, v->x*m->_13

        movss   xmm1, [eax+4]       // v->y
        shufps  xmm1, xmm1, 0       // fill xmm1 with v->y
        mulps   xmm1, [ecx+16]      // v->y*m->_21, v->y*m->_22, v->y*m->_23
        addps   xmm2, xmm1          // store intermediate result in xmm2

        movss   xmm1, [eax+8]       // v->z
        shufps  xmm1, xmm1, 0       // fill xmm1 with v->z
        mulps   xmm1, [ecx+32]      // v->z*m->_31, v->z*m->_32, v->z*m->_33
        addps   xmm2, xmm1          // store intermediate result in xmm2

        mov     eax, res
        movss   xmm1, [w]           // w
        shufps  xmm1, xmm1, 0       // fill xmm1 with w
        mulps   xmm1, [ecx+48]      // w*m->_41, w*m->_42, w*m->_43
        addps   xmm2, xmm1

        movlps  [eax], xmm2         // copy x and y
        shufps  xmm2, xmm2, 02h
        movss   [eax+8], xmm2       // copy z
    }
#else
    res->x = v->x*m->_11 + v->y*m->_21 + v->z*m->_31 + w*m->_41;
    res->y = v->x*m->_12 + v->y*m->_22 + v->z*m->_32 + w*m->_42;
    res->z = v->x*m->_13 + v->y*m->_23 + v->z*m->_33 + w*m->_43;
#endif
}

//---------------------------------------------------------------------------
// PERF: In terms of code size, it sure seems like a loop would be
//       better:

void MatrixProduct4x4 (D3DMATRIX *res, CONST D3DMATRIX *a, CONST D3DMATRIX *b)
{
#ifdef _X86_

    __asm {
        mov     eax, a
        mov     ecx, b

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

        mov     ecx, res
        addps   xmm5, xmm0

        addps   xmm5, xmm1              // 4 row of the result

        movaps  [ecx   ], xmm2
        movaps  [ecx+16], xmm3
        movaps  [ecx+32], xmm4
        movaps  [ecx+48], xmm5
    }
#else
    res->_11 = a->_11*b->_11 + a->_12*b->_21 + a->_13*b->_31 + a->_14*b->_41;
    res->_12 = a->_11*b->_12 + a->_12*b->_22 + a->_13*b->_32 + a->_14*b->_42;
    res->_13 = a->_11*b->_13 + a->_12*b->_23 + a->_13*b->_33 + a->_14*b->_43;
    res->_14 = a->_11*b->_14 + a->_12*b->_24 + a->_13*b->_34 + a->_14*b->_44;

    res->_21 = a->_21*b->_11 + a->_22*b->_21 + a->_23*b->_31 + a->_24*b->_41;
    res->_22 = a->_21*b->_12 + a->_22*b->_22 + a->_23*b->_32 + a->_24*b->_42;
    res->_23 = a->_21*b->_13 + a->_22*b->_23 + a->_23*b->_33 + a->_24*b->_43;
    res->_24 = a->_21*b->_14 + a->_22*b->_24 + a->_23*b->_34 + a->_24*b->_44;

    res->_31 = a->_31*b->_11 + a->_32*b->_21 + a->_33*b->_31 + a->_34*b->_41;
    res->_32 = a->_31*b->_12 + a->_32*b->_22 + a->_33*b->_32 + a->_34*b->_42;
    res->_33 = a->_31*b->_13 + a->_32*b->_23 + a->_33*b->_33 + a->_34*b->_43;
    res->_34 = a->_31*b->_14 + a->_32*b->_24 + a->_33*b->_34 + a->_34*b->_44;

    res->_41 = a->_41*b->_11 + a->_42*b->_21 + a->_43*b->_31 + a->_44*b->_41;
    res->_42 = a->_41*b->_12 + a->_42*b->_22 + a->_43*b->_32 + a->_44*b->_42;
    res->_43 = a->_41*b->_13 + a->_42*b->_23 + a->_43*b->_33 + a->_44*b->_43;
    res->_44 = a->_41*b->_14 + a->_42*b->_24 + a->_43*b->_34 + a->_44*b->_44;
#endif
}

//---------------------------------------------------------------------------

// This function uses Cramer's Rule to calculate the matrix inverse.
//
// Returns:
//    0 - if success
//   -1 - if input matrix is singular

int Inverse4x4 (D3DMATRIX *inverse, CONST D3DMATRIX *src, BOOL bNormalize)
{
    float x00, x01, x02;
    float x10, x11, x12;
    float x20, x21, x22;
    float x30, x31, x32;
    float rcp;
    float y01, y02, y03, y12, y13, y23;
    float z02, z03, z12, z13, z22, z23, z32, z33;
    DWORD  dwSign;

#define x03 x01
#define x13 x11
#define x23 x21
#define x33 x31
#define z00 x02
#define z10 x12
#define z20 x22
#define z30 x32
#define z01 x03
#define z11 x13
#define z21 x23
#define z31 x33

    /* read 1st two columns of matrix into registers */
    x00 = src->_11;
    x01 = src->_12;
    x10 = src->_21;
    x11 = src->_22;
    x20 = src->_31;
    x21 = src->_32;
    x30 = src->_41;
    x31 = src->_42;

    /* compute all six 2x2 determinants of 1st two columns */
    y01 = x00*x11 - x10*x01;
    y02 = x00*x21 - x20*x01;
    y03 = x00*x31 - x30*x01;
    y12 = x10*x21 - x20*x11;
    y13 = x10*x31 - x30*x11;
    y23 = x20*x31 - x30*x21;

    /* read 2nd two columns of matrix into registers */
    x02 = src->_13;
    x03 = src->_14;
    x12 = src->_23;
    x13 = src->_24;
    x22 = src->_33;
    x23 = src->_34;
    x32 = src->_43;
    x33 = src->_44;

    /* compute all 3x3 cofactors for 2nd two columns */
    z33 = x02*y12 - x12*y02 + x22*y01;
    z23 = x12*y03 - x32*y01 - x02*y13;
    z13 = x02*y23 - x22*y03 + x32*y02;
    z03 = x22*y13 - x32*y12 - x12*y23;
    z32 = x13*y02 - x23*y01 - x03*y12;
    z22 = x03*y13 - x13*y03 + x33*y01;
    z12 = x23*y03 - x33*y02 - x03*y23;
    z02 = x13*y23 - x23*y13 + x33*y12;

    /* compute all six 2x2 determinants of 2nd two columns */
    y01 = x02*x13 - x12*x03;
    y02 = x02*x23 - x22*x03;
    y03 = x02*x33 - x32*x03;
    y12 = x12*x23 - x22*x13;
    y13 = x12*x33 - x32*x13;
    y23 = x22*x33 - x32*x23;

    /* read 1st two columns of matrix into registers */
    x00 = src->_11;
    x01 = src->_12;
    x10 = src->_21;
    x11 = src->_22;
    x20 = src->_31;
    x21 = src->_32;
    x30 = src->_41;
    x31 = src->_42;

    /* compute all 3x3 cofactors for 1st column */
    z30 = x11*y02 - x21*y01 - x01*y12;
    z20 = x01*y13 - x11*y03 + x31*y01;
    z10 = x21*y03 - x31*y02 - x01*y23;
    z00 = x11*y23 - x21*y13 + x31*y12;

    /* compute all 3x3 cofactors for 2nd column */
    z31 = x00*y12 - x10*y02 + x20*y01;
    z21 = x10*y03 - x30*y01 - x00*y13;
    z11 = x00*y23 - x20*y03 + x30*y02;
    z01 = x20*y13 - x30*y12 - x10*y23;

    // compute 4x4 determinant
    rcp = x30*z30 + x20*z20 + x10*z10 + x00*z00;
    if (rcp == 0.f) return (-1);

    if (bNormalize) {
        // get the reciprocal of the determinant
        // and multiply all 3x3 cofactors by it
        rcp = nvInv (rcp);
        inverse->_11 = z00*rcp;
        inverse->_21 = z01*rcp;
        inverse->_12 = z10*rcp;
        inverse->_31 = z02*rcp;
        inverse->_13 = z20*rcp;
        inverse->_41 = z03*rcp;
        inverse->_14 = z30*rcp;
        inverse->_22 = z11*rcp;
        inverse->_32 = z12*rcp;
        inverse->_23 = z21*rcp;
        inverse->_42 = z13*rcp;
        inverse->_24 = z31*rcp;
        inverse->_33 = z22*rcp;
        inverse->_43 = z23*rcp;
        inverse->_34 = z32*rcp;
        inverse->_44 = z33*rcp;
    }

    else {
        // just get the sign of the determinant and multiply all
        // cofactors by that. since we're not normalizing, we don't
        // care about the magnitude, but we do care about the sense.
        dwSign = FP_SIGN_BIT(rcp);
        DWORD_FROM_FLOAT(z00) ^= dwSign;
        inverse->_11 = z00;
        DWORD_FROM_FLOAT(z01) ^= dwSign;
        inverse->_21 = z01;
        DWORD_FROM_FLOAT(z10) ^= dwSign;
        inverse->_12 = z10;
        DWORD_FROM_FLOAT(z02) ^= dwSign;
        inverse->_31 = z02;
        DWORD_FROM_FLOAT(z20) ^= dwSign;
        inverse->_13 = z20;
        DWORD_FROM_FLOAT(z03) ^= dwSign;
        inverse->_41 = z03;
        DWORD_FROM_FLOAT(z30) ^= dwSign;
        inverse->_14 = z30;
        DWORD_FROM_FLOAT(z11) ^= dwSign;
        inverse->_22 = z11;
        DWORD_FROM_FLOAT(z12) ^= dwSign;
        inverse->_32 = z12;
        DWORD_FROM_FLOAT(z21) ^= dwSign;
        inverse->_23 = z21;
        DWORD_FROM_FLOAT(z13) ^= dwSign;
        inverse->_42 = z13;
        DWORD_FROM_FLOAT(z31) ^= dwSign;
        inverse->_24 = z31;
        DWORD_FROM_FLOAT(z22) ^= dwSign;
        inverse->_33 = z22;
        DWORD_FROM_FLOAT(z23) ^= dwSign;
        inverse->_43 = z23;
        DWORD_FROM_FLOAT(z32) ^= dwSign;
        inverse->_34 = z32;
        DWORD_FROM_FLOAT(z33) ^= dwSign;
        inverse->_44 = z33;
    }

    return (0);

#undef x03
#undef x13
#undef x23
#undef x33
#undef z00
#undef z10
#undef z20
#undef z30
#undef z01
#undef z11
#undef z21
#undef z31

}

//---------------------------------------------------------------------------
// this function comes from erik.

D3DCONST float g_Lar[32] =
{
     0.000000f,  // error=0.687808
    -0.023353f,  // error=0.070106
    -0.095120f,  // error=0.010402
    -0.170208f,  // error=0.016597
    -0.251038f,  // error=0.021605
    -0.336208f,  // error=0.025186
    -0.421539f,  // error=0.027635
    -0.503634f,  // error=0.029262
    -0.579592f,  // error=0.030311
    -0.647660f,  // error=0.030994
    -0.708580f,  // error=0.031427
    -0.760208f,  // error=0.031702
    -0.803673f,  // error=0.031889
    -0.840165f,  // error=0.031995
    -0.871344f,  // error=0.032067
    -0.896105f,  // error=0.032105
    -0.916457f,  // error=0.032139
    -0.933262f,  // error=0.032165
    -0.946507f,  // error=0.032173
    -0.957755f,  // error=0.032285
    -0.966165f,  // error=0.032230
    -0.972848f,  // error=0.032189
    -0.978413f,  // error=0.032191
    -0.983217f,  // error=0.032718
    -0.986471f,  // error=0.032289
    -0.988778f,  // error=0.033091
    -0.991837f,  // error=0.035067
    -0.993452f,  // error=0.034156
    -0.994839f,  // error=0.034863
    -0.995434f,  // error=0.034785
    -0.996690f,  // error=0.033426
    -1.000000f
};

D3DCONST float g_Mar[32] =
{
    -0.494592f,  // error=0.687808
    -0.494592f,  // error=0.070106
    -0.570775f,  // error=0.010402
    -0.855843f,  // error=0.016597
    -1.152452f,  // error=0.021605
    -1.436778f,  // error=0.025186
    -1.705918f,  // error=0.027635
    -1.948316f,  // error=0.029262
    -2.167573f,  // error=0.030311
    -2.361987f,  // error=0.030994
    -2.512236f,  // error=0.031427
    -2.652873f,  // error=0.031702
    -2.781295f,  // error=0.031889
    -2.890906f,  // error=0.031995
    -2.938739f,  // error=0.032067
    -3.017491f,  // error=0.032105
    -3.077762f,  // error=0.032139
    -3.099087f,  // error=0.032165
    -3.144977f,  // error=0.032173
    -3.100986f,  // error=0.032285
    -3.151608f,  // error=0.032230
    -3.212636f,  // error=0.032189
    -3.219419f,  // error=0.032191
    -3.079402f,  // error=0.032718
    -3.174922f,  // error=0.032289
    -3.469706f,  // error=0.033091
    -2.895668f,  // error=0.035067
    -2.959919f,  // error=0.034156
    -2.917150f,  // error=0.034863
    -3.600301f,  // error=0.034785
    -3.024990f,  // error=0.033426
    -3.300000f
};

const float LOG_64F = 4.15888308336f;
const float INV_LOG_2F = 1.44269504089f;

void Explut (float n, float *l, float *m)
{
    float idx,f,a;
    long  i;

    if (n < 1.f) {
        a  = (n == 0.f) ? 0.f : (float)Exp(-LOG_64F/n);
        *l = -a;
        *m = 1.f - (1.f-a)*n;
    }
    else {
        idx = 3.f * (float)Log(n) * INV_LOG_2F;
        i = FloatToLong(idx);
        f = idx-i;

        *l = g_Lar[i]*(1.f-f) + g_Lar[i+1]*f;
        *m = g_Mar[i]*(1.f-f) + g_Mar[i+1]*f;
    }
}

} // end of namespace
