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
#include "nvprecomp.h"

#if (NVARCH >= 0x04)

#include "x86.h"

//---------------------------------------------------------------------------

// returns  1 / sqrt(x)

static float _0_47 = 0.47f;
static float _1_47 = 1.47f;

float __fastcall JBInvSqrt (const float x)
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
    r = FLOAT_FROM_DWORD(y);

    // optional
    r = (3.0f - x * (r * r)) * r * 0.5f;    // remove for low accuracy

    return (r);
}

//---------------------------------------------------------------------------

//  sqrt(x) = x * (1/sqrt(x))

float __fastcall nvSqrt (float x)
{
    return (x*JBInvSqrt(x));
}

//---------------------------------------------------------------------------

//  1/x = 1 / sqrt(x*x) plus sign bit

float __fastcall nvInv (float x)
{
    DWORD dwSign = *(DWORD *)&x & 0x80000000;
    float invSqRt = JBInvSqrt (x*x);
    DWORD dwInv = dwSign | *(DWORD *)&invSqRt;
    return (FLOAT_FROM_DWORD(dwInv));
}

//---------------------------------------------------------------------------

// reverse a 3-component vector

void __fastcall ReverseVector3 (D3DVECTOR *out, D3DVECTOR *in)
{
    out->x = -(in->x);
    out->y = -(in->y);
    out->z = -(in->z);
}

//---------------------------------------------------------------------------

// scale a 3-component vector

void __fastcall ScaleVector3 (D3DVECTOR *out, D3DVECTOR *v1, D3DVALUE scale)
{
    out->x = scale * v1->x;
    out->y = scale * v1->y;
    out->z = scale * v1->z;
}

//---------------------------------------------------------------------------

// add two 3-component vectors

void __fastcall AddVectors3 (D3DVECTOR *out, D3DVECTOR *v1, D3DVECTOR *v2)
{
    out->x = v1->x + v2->x;
    out->y = v1->y + v2->y;
    out->z = v1->z + v2->z;
}

//---------------------------------------------------------------------------

// subtract two 3-component vectors

void __fastcall SubtractVectors3 (D3DVECTOR *out, D3DVECTOR *v1, D3DVECTOR *v2)
{
    out->x = v1->x - v2->x;
    out->y = v1->y - v2->y;
    out->z = v1->z - v2->z;
}

//---------------------------------------------------------------------------

// return the square of the magnitude of a 3-component vectors

D3DVALUE __fastcall SquareMagnitude3 (D3DVECTOR *v)
{
    return (v->x*v->x + v->y*v->y + v->z*v->z);
}

//---------------------------------------------------------------------------

// return the magnitude of a 3-component vector

D3DVALUE __fastcall Magnitude3 (D3DVECTOR *v)
{
    return (D3DVALUE) nvSqrt(SquareMagnitude3(v));
}

//---------------------------------------------------------------------------

// normalize a 3-component vector

void __fastcall NormalizeVector3 (D3DVECTOR *v)
{
    D3DVALUE invmag = JBInvSqrt (SquareMagnitude3(v));
    nvAssert (invmag > 0);
    v->x *= invmag;
    v->y *= invmag;
    v->z *= invmag;
}

//---------------------------------------------------------------------------

// dot product of a 3-component vector

D3DVALUE __fastcall DotProduct3 (D3DVECTOR *v1, D3DVECTOR *v2)
{
    return (v1->x*v2->x + v1->y*v2->y + v1->z*v2->z);
}

//---------------------------------------------------------------------------

// Multiplies row vector [v.x v.y v.z w] by 4x3 matrix m
// res and v should not be the same

void __fastcall XformBy4x3 (D3DVECTOR *res, D3DVECTOR *v, D3DVALUE w, D3DMATRIX *m)
{
    nvAssert (res != v);
    res->x = v->x*m->_11 + v->y*m->_21 + v->z*m->_31 + w*m->_41;
    res->y = v->x*m->_12 + v->y*m->_22 + v->z*m->_32 + w*m->_42;
    res->z = v->x*m->_13 + v->y*m->_23 + v->z*m->_33 + w*m->_43;
}

//---------------------------------------------------------------------------

// Multiplies vector (x,y,z) by 3x3 matrix
// res and v should not be the same

void __fastcall XformVector3 (D3DVECTOR *res, D3DVECTOR *v, D3DMATRIX *m)
{
    nvAssert (res != v);
    res->x = v->x*m->_11 + v->y*m->_21 + v->z*m->_31;
    res->y = v->x*m->_12 + v->y*m->_22 + v->z*m->_32;
    res->z = v->x*m->_13 + v->y*m->_23 + v->z*m->_33;
}

//---------------------------------------------------------------------------

// Multiplies vector (x,y,z,w) by 4x4 matrix
// res and v should not be the same

void __fastcall XformVector4 (D3DVALUE *res, D3DVALUE *v, D3DMATRIX *m)
{
    nvAssert (res != v);
    res[0] = v[0]*m->_11 + v[1]*m->_21 + v[2]*m->_31 + v[3]*m->_41;
    res[1] = v[0]*m->_12 + v[1]*m->_22 + v[2]*m->_32 + v[3]*m->_42;
    res[2] = v[0]*m->_13 + v[1]*m->_23 + v[2]*m->_33 + v[3]*m->_43;
    res[3] = v[0]*m->_14 + v[1]*m->_24 + v[2]*m->_34 + v[3]*m->_44;
}

//---------------------------------------------------------------------------

void __fastcall MatrixProduct3x3 (D3DMATRIX *res, D3DMATRIX *a, D3DMATRIX *b)
{
    res->_11 = a->_11*b->_11 + a->_12*b->_21 + a->_13*b->_31;
    res->_12 = a->_11*b->_12 + a->_12*b->_22 + a->_13*b->_32;
    res->_13 = a->_11*b->_13 + a->_12*b->_23 + a->_13*b->_33;

    res->_21 = a->_21*b->_11 + a->_22*b->_21 + a->_23*b->_31;
    res->_22 = a->_21*b->_12 + a->_22*b->_22 + a->_23*b->_32;
    res->_23 = a->_21*b->_13 + a->_22*b->_23 + a->_23*b->_33;

    res->_31 = a->_31*b->_11 + a->_32*b->_21 + a->_33*b->_31;
    res->_32 = a->_31*b->_12 + a->_32*b->_22 + a->_33*b->_32;
    res->_33 = a->_31*b->_13 + a->_32*b->_23 + a->_33*b->_33;
}

//---------------------------------------------------------------------------

void __fastcall MatrixProduct4x4 (D3DMATRIX *res, D3DMATRIX *a, D3DMATRIX *b)
{
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
}

//---------------------------------------------------------------------------

// transpose a 4x4 matrix. res and m should not be the same.

void __fastcall MatrixTranspose4x4 (D3DMATRIX *res, D3DMATRIX *m)
{
    assert (res != m);

    res->_11 = m->_11;
    res->_12 = m->_21;
    res->_13 = m->_31;
    res->_14 = m->_41;

    res->_21 = m->_12;
    res->_22 = m->_22;
    res->_23 = m->_32;
    res->_24 = m->_42;

    res->_31 = m->_13;
    res->_32 = m->_23;
    res->_33 = m->_33;
    res->_34 = m->_43;

    res->_41 = m->_14;
    res->_42 = m->_24;
    res->_43 = m->_34;
    res->_44 = m->_44;
}

//---------------------------------------------------------------------------

// This function uses Cramer's Rule to calculate the inverse of a 3x3 matrix
// (or the upper 3x3 portion of a larger matrix)
//
// Returns:
//    0 - if success
//   -1 - if input matrix is singular

#define DETERMINANT(m, r1, r2, c1, c2) (m->_##r1##c1 * m->_##r2##c2 - m->_##r1##c2 * m->_##r2##c1)

int __fastcall Inverse3x3 (D3DMATRIX *inverse, D3DMATRIX *src, BOOL bNormalize)
{
    float b11,b12,b13;
    float b21,b22,b23;
    float b31,b32,b33;
    float det, idet;
    DWORD dwSign;

    b11 =  DETERMINANT(src, 2,3,2,3);
    b12 = -DETERMINANT(src, 2,3,1,3);
    b13 =  DETERMINANT(src, 2,3,1,2);
    b21 = -DETERMINANT(src, 1,3,2,3);
    b22 =  DETERMINANT(src, 1,3,1,3);
    b23 = -DETERMINANT(src, 1,3,1,2);
    b31 =  DETERMINANT(src, 1,2,2,3);
    b32 = -DETERMINANT(src, 1,2,1,3);
    b33 =  DETERMINANT(src, 1,2,1,2);

    // compute 3x3 determinant
    det = (src->_11 * b11) + (src->_12 * b12) + (src->_13 * b13);
    if (det == 0.0) {
        return (-1);
    }

    if (bNormalize) {
        // get the reciprocal of the determinant
        // and multiply all 3x3 cofactors by it
        idet = nvInv (det);
        inverse->_11 = (D3DVALUE) (idet * b11);
        inverse->_12 = (D3DVALUE) (idet * b21);
        inverse->_13 = (D3DVALUE) (idet * b31);
        inverse->_21 = (D3DVALUE) (idet * b12);
        inverse->_22 = (D3DVALUE) (idet * b22);
        inverse->_23 = (D3DVALUE) (idet * b32);
        inverse->_31 = (D3DVALUE) (idet * b13);
        inverse->_32 = (D3DVALUE) (idet * b23);
        inverse->_33 = (D3DVALUE) (idet * b33);
    }

    else {
        // just get the sign of the determinant and multiply all
        // cofactors by that. since we're not normalizing, we don't
        // care about the magnitude, but we do care about the sense.
        dwSign = FP_SIGN_BIT(det);
        DWORD_FROM_FLOAT(b11) ^= dwSign;
        inverse->_11 = (D3DVALUE) (b11);
        DWORD_FROM_FLOAT(b21) ^= dwSign;
        inverse->_12 = (D3DVALUE) (b21);
        DWORD_FROM_FLOAT(b31) ^= dwSign;
        inverse->_13 = (D3DVALUE) (b31);
        DWORD_FROM_FLOAT(b12) ^= dwSign;
        inverse->_21 = (D3DVALUE) (b12);
        DWORD_FROM_FLOAT(b22) ^= dwSign;
        inverse->_22 = (D3DVALUE) (b22);
        DWORD_FROM_FLOAT(b32) ^= dwSign;
        inverse->_23 = (D3DVALUE) (b32);
        DWORD_FROM_FLOAT(b13) ^= dwSign;
        inverse->_31 = (D3DVALUE) (b13);
        DWORD_FROM_FLOAT(b23) ^= dwSign;
        inverse->_32 = (D3DVALUE) (b23);
        DWORD_FROM_FLOAT(b33) ^= dwSign;
        inverse->_33 = (D3DVALUE) (b33);
    }

    return (0);
}

//---------------------------------------------------------------------------

// This function uses Cramer's Rule to calculate the matrix inverse.
//
// Returns:
//    0 - if success
//   -1 - if input matrix is singular

int __fastcall Inverse4x4 (D3DMATRIX *inverse, D3DMATRIX *src, BOOL bNormalize)
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

// More accurate matrix inversion. The reciprocal of the determinant computed by Inverse4x4
// is often really, really bad.
int __fastcall STInverse4x4 (D3DMATRIX *inverse, D3DMATRIX *src)
{
    float x00, x01, x02;
    float x10, x11, x12;
    float x20, x21, x22;
    float x30, x31, x32;
    float rcp;
    float y01, y02, y03, y12, y13, y23;
    float z02, z03, z12, z13, z22, z23, z32, z33;

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

    // get the reciprocal of the determinant
    // and multiply all 3x3 cofactors by it
    rcp = 1.0f / rcp;
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

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
// helper routines ///////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

void nvComputeBoundingBox
(
    BYTE  *pVertices,
    DWORD  dwCount,
    DWORD  dwVertexStride,
    float *pafMinMaxInfo        // 6 floats, x,y,z min; x,y,z max
)
{
    if (!dwCount)
    {
        pafMinMaxInfo[0] = pafMinMaxInfo[1]
                         = pafMinMaxInfo[2]
                         = pafMinMaxInfo[3]
                         = pafMinMaxInfo[4]
                         = pafMinMaxInfo[5]
                         = 0.0f;
        return;
    }

    //
    // todo - use SIMD for AMD and KNI
    //
    pafMinMaxInfo[0] = +1e37f;
    pafMinMaxInfo[1] = +1e37f;
    pafMinMaxInfo[2] = +1e37f;
    pafMinMaxInfo[3] = -1e37f;
    pafMinMaxInfo[4] = -1e37f;
    pafMinMaxInfo[5] = -1e37f;
    while (dwCount)
    {
        float x = *(float*)(pVertices + 0);
        float y = *(float*)(pVertices + 4);
        float z = *(float*)(pVertices + 8);

        if (x < pafMinMaxInfo[0]) pafMinMaxInfo[0] = x;
        if (y < pafMinMaxInfo[1]) pafMinMaxInfo[1] = y;
        if (z < pafMinMaxInfo[2]) pafMinMaxInfo[2] = z;

        if (x > pafMinMaxInfo[3]) pafMinMaxInfo[3] = x;
        if (y > pafMinMaxInfo[4]) pafMinMaxInfo[4] = y;
        if (z > pafMinMaxInfo[5]) pafMinMaxInfo[5] = z;

        pVertices += dwVertexStride;
        dwCount--;
    }
}

//////////////////////////////////////////////////////////////////////////////

#define X pfVerticesOut[0]
#define Y pfVerticesOut[1]
#define Z pfVerticesOut[2]
#define W pfVerticesOut[3]

void nvComputeXfmAndOutCodes
(
    D3DMATRIX *pXfm,
    float     *pfVerticesIn,
    float     *pfVerticesOut,
    DWORD      dwCount,
    DWORD     *pdwOutCodes
)
{
    //
    // todo - use SIMD for AMD and KNI
    //
    while (dwCount)
    {
        // xfm
        XformVector4 (pfVerticesOut,pfVerticesIn,pXfm);

        // outcodes
        DWORD dwOutCode = 0;
        if (X < -W) dwOutCode |= 1;
        if (X >  W) dwOutCode |= 2;
        if (Y < -W) dwOutCode |= 4;
        if (Y >  W) dwOutCode |= 8;
        if (Z <  0) dwOutCode |= 16;
        if (Z >  W) dwOutCode |= 32;
        *pdwOutCodes = dwOutCode;

        // next
        pfVerticesIn  += 4;
        pfVerticesOut += 4;
        pdwOutCodes   ++;
        dwCount       --;
    }
}

#undef X
#undef Y
#undef Z
#undef W

//////////////////////////////////////////////////////////////////////////////


#endif  // (NVARCH >= 0x04)