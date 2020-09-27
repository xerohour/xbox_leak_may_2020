//////////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) 1998 - 2001 Microsoft Corporation.  All Rights Reserved.
//
//  File:       xgmath.inl
//  Content:    XG math inline functions
//
//////////////////////////////////////////////////////////////////////////////

#ifndef __XGMATH_INL__
#define __XGMATH_INL__

#pragma warning(disable:4035)

//===========================================================================
//
// Inline Class Methods
//
//===========================================================================

#ifdef __cplusplus

#include <malloc.h>

//--------------------------
// 2D Vector
//--------------------------

XGINLINE
XGVECTOR2::XGVECTOR2( CONST FLOAT *pf )
{
#ifdef _DEBUG

#ifdef ASSERT
    ASSERT(pf != NULL);
#endif

#endif

    x = pf[0];
    y = pf[1];
}

XGINLINE
XGVECTOR2::XGVECTOR2( FLOAT fx, FLOAT fy )
{
    x = fx;
    y = fy;
}

// casting
XGINLINE
XGVECTOR2::operator FLOAT* ()
{
    return (FLOAT *) &x;
}

XGINLINE
XGVECTOR2::operator CONST FLOAT* () const
{
    return (CONST FLOAT *) &x;
}

// assignment operators
XGINLINE XGVECTOR2&
XGVECTOR2::operator += ( CONST XGVECTOR2& v )
{
    x += v.x;
    y += v.y;
    return *this;
}

XGINLINE XGVECTOR2&
XGVECTOR2::operator -= ( CONST XGVECTOR2& v )
{
    x -= v.x;
    y -= v.y;
    return *this;
}

XGINLINE XGVECTOR2&
XGVECTOR2::operator *= ( FLOAT f )
{
    x *= f;
    y *= f;
    return *this;
}

XGINLINE XGVECTOR2&
XGVECTOR2::operator /= ( FLOAT f )
{
    FLOAT fInv = 1.0f / f;
    x *= fInv;
    y *= fInv;
    return *this;
}

// unary operators
XGINLINE XGVECTOR2
XGVECTOR2::operator + () const
{
    return *this;
}

XGINLINE XGVECTOR2
XGVECTOR2::operator - () const
{
    return XGVECTOR2(-x, -y);
}

// binary operators
XGINLINE XGVECTOR2
XGVECTOR2::operator + ( CONST XGVECTOR2& v ) const
{
    return XGVECTOR2(x + v.x, y + v.y);
}

XGINLINE XGVECTOR2
XGVECTOR2::operator - ( CONST XGVECTOR2& v ) const
{
    return XGVECTOR2(x - v.x, y - v.y);
}

XGINLINE XGVECTOR2
XGVECTOR2::operator * ( FLOAT f ) const
{
    return XGVECTOR2(x * f, y * f);
}

XGINLINE XGVECTOR2
XGVECTOR2::operator / ( FLOAT f ) const
{
    FLOAT fInv = 1.0f / f;
    return XGVECTOR2(x * fInv, y * fInv);
}


XGINLINE XGVECTOR2
operator * ( FLOAT f, CONST XGVECTOR2& v )
{
    return XGVECTOR2(f * v.x, f * v.y);
}

XGINLINE BOOL
XGVECTOR2::operator == ( CONST XGVECTOR2& v ) const
{
    return x == v.x && y == v.y;
}

XGINLINE BOOL
XGVECTOR2::operator != ( CONST XGVECTOR2& v ) const
{
    return x != v.x || y != v.y;
}




//--------------------------
// 3D Vector
//--------------------------
XGINLINE
XGVECTOR3::XGVECTOR3( CONST FLOAT *pf )
{
#ifdef _DEBUG

#ifdef ASSERT
    ASSERT(pf != NULL);
#endif

#endif

    x = pf[0];
    y = pf[1];
    z = pf[2];
}

XGINLINE
XGVECTOR3::XGVECTOR3( CONST D3DVECTOR& v )
{
    x = v.x;
    y = v.y;
    z = v.z;
}

XGINLINE
XGVECTOR3::XGVECTOR3( FLOAT fx, FLOAT fy, FLOAT fz )
{
    x = fx;
    y = fy;
    z = fz;
}


// casting
XGINLINE
XGVECTOR3::operator FLOAT* ()
{
    return (FLOAT *) &x;
}

XGINLINE
XGVECTOR3::operator CONST FLOAT* () const
{
    return (CONST FLOAT *) &x;
}


// assignment operators
XGINLINE XGVECTOR3&
XGVECTOR3::operator += ( CONST XGVECTOR3& v )
{
    x += v.x;
    y += v.y;
    z += v.z;
    return *this;
}

XGINLINE XGVECTOR3&
XGVECTOR3::operator -= ( CONST XGVECTOR3& v )
{
    x -= v.x;
    y -= v.y;
    z -= v.z;
    return *this;
}

XGINLINE XGVECTOR3&
XGVECTOR3::operator *= ( FLOAT f )
{
    x *= f;
    y *= f;
    z *= f;
    return *this;
}

XGINLINE XGVECTOR3&
XGVECTOR3::operator /= ( FLOAT f )
{
    FLOAT fInv = 1.0f / f;
    x *= fInv;
    y *= fInv;
    z *= fInv;
    return *this;
}


// unary operators
XGINLINE XGVECTOR3
XGVECTOR3::operator + () const
{
    return *this;
}

XGINLINE XGVECTOR3
XGVECTOR3::operator - () const
{
    return XGVECTOR3(-x, -y, -z);
}


// binary operators
XGINLINE XGVECTOR3
XGVECTOR3::operator + ( CONST XGVECTOR3& v ) const
{
    return XGVECTOR3(x + v.x, y + v.y, z + v.z);
}

XGINLINE XGVECTOR3
XGVECTOR3::operator - ( CONST XGVECTOR3& v ) const
{
    return XGVECTOR3(x - v.x, y - v.y, z - v.z);
}

XGINLINE XGVECTOR3
XGVECTOR3::operator * ( FLOAT f ) const
{
    return XGVECTOR3(x * f, y * f, z * f);
}

XGINLINE XGVECTOR3
XGVECTOR3::operator / ( FLOAT f ) const
{
    FLOAT fInv = 1.0f / f;
    return XGVECTOR3(x * fInv, y * fInv, z * fInv);
}


XGINLINE XGVECTOR3
operator * ( FLOAT f, CONST struct XGVECTOR3& v )
{
    return XGVECTOR3(f * v.x, f * v.y, f * v.z);
}


XGINLINE BOOL
XGVECTOR3::operator == ( CONST XGVECTOR3& v ) const
{
    return x == v.x && y == v.y && z == v.z;
}

XGINLINE BOOL
XGVECTOR3::operator != ( CONST XGVECTOR3& v ) const
{
    return x != v.x || y != v.y || z != v.z;
}



//--------------------------
// 4D Vector
//--------------------------
XGINLINE
XGVECTOR4::XGVECTOR4( CONST FLOAT *pf )
{
#ifdef _DEBUG

#ifdef ASSERT
    ASSERT(pf != NULL);
#endif

#endif

    x = pf[0];
    y = pf[1];
    z = pf[2];
    w = pf[3];
}

XGINLINE
XGVECTOR4::XGVECTOR4( FLOAT fx, FLOAT fy, FLOAT fz, FLOAT fw )
{
    x = fx;
    y = fy;
    z = fz;
    w = fw;
}


// casting
XGINLINE
XGVECTOR4::operator FLOAT* ()
{
    return (FLOAT *) &x;
}

XGINLINE
XGVECTOR4::operator CONST FLOAT* () const
{
    return (CONST FLOAT *) &x;
}


// assignment operators
XGINLINE XGVECTOR4&
XGVECTOR4::operator += ( CONST XGVECTOR4& v )
{
    x += v.x;
    y += v.y;
    z += v.z;
    w += v.w;
    return *this;
}

XGINLINE XGVECTOR4&
XGVECTOR4::operator -= ( CONST XGVECTOR4& v )
{
    x -= v.x;
    y -= v.y;
    z -= v.z;
    w -= v.w;
    return *this;
}

XGINLINE XGVECTOR4&
XGVECTOR4::operator *= ( FLOAT f )
{
    x *= f;
    y *= f;
    z *= f;
    w *= f;
    return *this;
}

XGINLINE XGVECTOR4&
XGVECTOR4::operator /= ( FLOAT f )
{
    FLOAT fInv = 1.0f / f;
    x *= fInv;
    y *= fInv;
    z *= fInv;
    w *= fInv;
    return *this;
}


// unary operators
XGINLINE XGVECTOR4
XGVECTOR4::operator + () const
{
    return *this;
}

XGINLINE XGVECTOR4
XGVECTOR4::operator - () const
{
    return XGVECTOR4(-x, -y, -z, -w);
}


// binary operators
XGINLINE XGVECTOR4
XGVECTOR4::operator + ( CONST XGVECTOR4& v ) const
{
    return XGVECTOR4(x + v.x, y + v.y, z + v.z, w + v.w);
}

XGINLINE XGVECTOR4
XGVECTOR4::operator - ( CONST XGVECTOR4& v ) const
{
    return XGVECTOR4(x - v.x, y - v.y, z - v.z, w - v.w);
}

XGINLINE XGVECTOR4
XGVECTOR4::operator * ( FLOAT f ) const
{
    return XGVECTOR4(x * f, y * f, z * f, w * f);
}

XGINLINE XGVECTOR4
XGVECTOR4::operator / ( FLOAT f ) const
{
    FLOAT fInv = 1.0f / f;
    return XGVECTOR4(x * fInv, y * fInv, z * fInv, w * fInv);
}


XGINLINE XGVECTOR4
operator * ( FLOAT f, CONST XGVECTOR4& v )
{
    return XGVECTOR4(f * v.x, f * v.y, f * v.z, f * v.w);
}


XGINLINE BOOL
XGVECTOR4::operator == ( CONST XGVECTOR4& v ) const
{
    return x == v.x && y == v.y && z == v.z && w == v.w;
}

XGINLINE BOOL
XGVECTOR4::operator != ( CONST XGVECTOR4& v ) const
{
    return x != v.x || y != v.y || z != v.z || w != v.w;
}


//--------------------------
// Matrix
//--------------------------
XGINLINE
XGMATRIX::XGMATRIX( CONST FLOAT* pf )
{
#ifdef _DEBUG

#ifdef ASSERT
    ASSERT(pf != NULL);
#endif

#endif

    memcpy(&_11, pf, sizeof(XGMATRIX));
}

XGINLINE
XGMATRIX::XGMATRIX( CONST D3DMATRIX& mat )
{
    memcpy(&_11, &mat, sizeof(XGMATRIX));
}

XGINLINE
XGMATRIX::XGMATRIX( FLOAT f11, FLOAT f12, FLOAT f13, FLOAT f14,
                        FLOAT f21, FLOAT f22, FLOAT f23, FLOAT f24,
                        FLOAT f31, FLOAT f32, FLOAT f33, FLOAT f34,
                        FLOAT f41, FLOAT f42, FLOAT f43, FLOAT f44 )
{
    _11 = f11; _12 = f12; _13 = f13; _14 = f14;
    _21 = f21; _22 = f22; _23 = f23; _24 = f24;
    _31 = f31; _32 = f32; _33 = f33; _34 = f34;
    _41 = f41; _42 = f42; _43 = f43; _44 = f44;
}


// access grants
XGINLINE FLOAT&
XGMATRIX::operator () ( UINT iRow, UINT iCol )
{
    return m[iRow][iCol];
}

XGINLINE FLOAT
XGMATRIX::operator () ( UINT iRow, UINT iCol ) const
{
    return m[iRow][iCol];
}


// casting operators
XGINLINE
XGMATRIX::operator FLOAT* ()
{
    return (FLOAT *) &_11;
}

XGINLINE
XGMATRIX::operator CONST FLOAT* () const
{
    return (CONST FLOAT *) &_11;
}


// assignment operators
XGINLINE XGMATRIX&
XGMATRIX::operator *= ( CONST XGMATRIX& mat )
{
    XGMatrixMultiply(this, this, &mat);
    return *this;
}

XGINLINE XGMATRIX&
XGMATRIX::operator += ( CONST XGMATRIX& mat )
{
    _11 += mat._11; _12 += mat._12; _13 += mat._13; _14 += mat._14;
    _21 += mat._21; _22 += mat._22; _23 += mat._23; _24 += mat._24;
    _31 += mat._31; _32 += mat._32; _33 += mat._33; _34 += mat._34;
    _41 += mat._41; _42 += mat._42; _43 += mat._43; _44 += mat._44;
    return *this;
}

XGINLINE XGMATRIX&
XGMATRIX::operator -= ( CONST XGMATRIX& mat )
{
    _11 -= mat._11; _12 -= mat._12; _13 -= mat._13; _14 -= mat._14;
    _21 -= mat._21; _22 -= mat._22; _23 -= mat._23; _24 -= mat._24;
    _31 -= mat._31; _32 -= mat._32; _33 -= mat._33; _34 -= mat._34;
    _41 -= mat._41; _42 -= mat._42; _43 -= mat._43; _44 -= mat._44;
    return *this;
}

XGINLINE XGMATRIX&
XGMATRIX::operator *= ( FLOAT f )
{
    _11 *= f; _12 *= f; _13 *= f; _14 *= f;
    _21 *= f; _22 *= f; _23 *= f; _24 *= f;
    _31 *= f; _32 *= f; _33 *= f; _34 *= f;
    _41 *= f; _42 *= f; _43 *= f; _44 *= f;
    return *this;
}

XGINLINE XGMATRIX&
XGMATRIX::operator /= ( FLOAT f )
{
    FLOAT fInv = 1.0f / f;
    _11 *= fInv; _12 *= fInv; _13 *= fInv; _14 *= fInv;
    _21 *= fInv; _22 *= fInv; _23 *= fInv; _24 *= fInv;
    _31 *= fInv; _32 *= fInv; _33 *= fInv; _34 *= fInv;
    _41 *= fInv; _42 *= fInv; _43 *= fInv; _44 *= fInv;
    return *this;
}


// unary operators
XGINLINE XGMATRIX
XGMATRIX::operator + () const
{
    return *this;
}

XGINLINE XGMATRIX
XGMATRIX::operator - () const
{
    return XGMATRIX(-_11, -_12, -_13, -_14,
                      -_21, -_22, -_23, -_24,
                      -_31, -_32, -_33, -_34,
                      -_41, -_42, -_43, -_44);
}


// binary operators
XGINLINE XGMATRIX
XGMATRIX::operator * ( CONST XGMATRIX& mat ) const
{
    XGMATRIX matT;
    XGMatrixMultiply(&matT, this, &mat);
    return matT;
}

XGINLINE XGMATRIX
XGMATRIX::operator + ( CONST XGMATRIX& mat ) const
{
    return XGMATRIX(_11 + mat._11, _12 + mat._12, _13 + mat._13, _14 + mat._14,
                      _21 + mat._21, _22 + mat._22, _23 + mat._23, _24 + mat._24,
                      _31 + mat._31, _32 + mat._32, _33 + mat._33, _34 + mat._34,
                      _41 + mat._41, _42 + mat._42, _43 + mat._43, _44 + mat._44);
}

XGINLINE XGMATRIX
XGMATRIX::operator - ( CONST XGMATRIX& mat ) const
{
    return XGMATRIX(_11 - mat._11, _12 - mat._12, _13 - mat._13, _14 - mat._14,
                      _21 - mat._21, _22 - mat._22, _23 - mat._23, _24 - mat._24,
                      _31 - mat._31, _32 - mat._32, _33 - mat._33, _34 - mat._34,
                      _41 - mat._41, _42 - mat._42, _43 - mat._43, _44 - mat._44);
}

XGINLINE XGMATRIX
XGMATRIX::operator * ( FLOAT f ) const
{
    return XGMATRIX(_11 * f, _12 * f, _13 * f, _14 * f,
                      _21 * f, _22 * f, _23 * f, _24 * f,
                      _31 * f, _32 * f, _33 * f, _34 * f,
                      _41 * f, _42 * f, _43 * f, _44 * f);
}

XGINLINE XGMATRIX
XGMATRIX::operator / ( FLOAT f ) const
{
    FLOAT fInv = 1.0f / f;
    return XGMATRIX(_11 * fInv, _12 * fInv, _13 * fInv, _14 * fInv,
                      _21 * fInv, _22 * fInv, _23 * fInv, _24 * fInv,
                      _31 * fInv, _32 * fInv, _33 * fInv, _34 * fInv,
                      _41 * fInv, _42 * fInv, _43 * fInv, _44 * fInv);
}


XGINLINE XGMATRIX
operator * ( FLOAT f, CONST XGMATRIX& mat )
{
    return XGMATRIX(f * mat._11, f * mat._12, f * mat._13, f * mat._14,
                      f * mat._21, f * mat._22, f * mat._23, f * mat._24,
                      f * mat._31, f * mat._32, f * mat._33, f * mat._34,
                      f * mat._41, f * mat._42, f * mat._43, f * mat._44);
}


XGINLINE BOOL
XGMATRIX::operator == ( CONST XGMATRIX& mat ) const
{
    return 0 == memcmp(this, &mat, sizeof(XGMATRIX));
}

XGINLINE BOOL
XGMATRIX::operator != ( CONST XGMATRIX& mat ) const
{
    return 0 != memcmp(this, &mat, sizeof(XGMATRIX));
}



//--------------------------
// Quaternion
//--------------------------

XGINLINE
XGQUATERNION::XGQUATERNION( CONST FLOAT* pf )
{
#ifdef _DEBUG

#ifdef ASSERT
    ASSERT(pf != NULL);
#endif

#endif

    x = pf[0];
    y = pf[1];
    z = pf[2];
    w = pf[3];
}

XGINLINE
XGQUATERNION::XGQUATERNION( FLOAT fx, FLOAT fy, FLOAT fz, FLOAT fw )
{
    x = fx;
    y = fy;
    z = fz;
    w = fw;
}


// casting
XGINLINE
XGQUATERNION::operator FLOAT* ()
{
    return (FLOAT *) &x;
}

XGINLINE
XGQUATERNION::operator CONST FLOAT* () const
{
    return (CONST FLOAT *) &x;
}


// assignment operators
XGINLINE XGQUATERNION&
XGQUATERNION::operator += ( CONST XGQUATERNION& q )
{
    x += q.x;
    y += q.y;
    z += q.z;
    w += q.w;
    return *this;
}

XGINLINE XGQUATERNION&
XGQUATERNION::operator -= ( CONST XGQUATERNION& q )
{
    x -= q.x;
    y -= q.y;
    z -= q.z;
    w -= q.w;
    return *this;
}

XGINLINE XGQUATERNION&
XGQUATERNION::operator *= ( CONST XGQUATERNION& q )
{
    XGQuaternionMultiply(this, this, &q);
    return *this;
}

XGINLINE XGQUATERNION&
XGQUATERNION::operator *= ( FLOAT f )
{
    x *= f;
    y *= f;
    z *= f;
    w *= f;
    return *this;
}

XGINLINE XGQUATERNION&
XGQUATERNION::operator /= ( FLOAT f )
{
    FLOAT fInv = 1.0f / f;
    x *= fInv;
    y *= fInv;
    z *= fInv;
    w *= fInv;
    return *this;
}


// unary operators
XGINLINE XGQUATERNION
XGQUATERNION::operator + () const
{
    return *this;
}

XGINLINE XGQUATERNION
XGQUATERNION::operator - () const
{
    return XGQUATERNION(-x, -y, -z, -w);
}


// binary operators
XGINLINE XGQUATERNION
XGQUATERNION::operator + ( CONST XGQUATERNION& q ) const
{
    return XGQUATERNION(x + q.x, y + q.y, z + q.z, w + q.w);
}

XGINLINE XGQUATERNION
XGQUATERNION::operator - ( CONST XGQUATERNION& q ) const
{
    return XGQUATERNION(x - q.x, y - q.y, z - q.z, w - q.w);
}

XGINLINE XGQUATERNION
XGQUATERNION::operator * ( CONST XGQUATERNION& q ) const
{
    XGQUATERNION qT;
    XGQuaternionMultiply(&qT, this, &q);
    return qT;
}

XGINLINE XGQUATERNION
XGQUATERNION::operator * ( FLOAT f ) const
{
    return XGQUATERNION(x * f, y * f, z * f, w * f);
}

XGINLINE XGQUATERNION
XGQUATERNION::operator / ( FLOAT f ) const
{
    FLOAT fInv = 1.0f / f;
    return XGQUATERNION(x * fInv, y * fInv, z * fInv, w * fInv);
}


XGINLINE XGQUATERNION
operator * (FLOAT f, CONST XGQUATERNION& q )
{
    return XGQUATERNION(f * q.x, f * q.y, f * q.z, f * q.w);
}


XGINLINE BOOL
XGQUATERNION::operator == ( CONST XGQUATERNION& q ) const
{
    return x == q.x && y == q.y && z == q.z && w == q.w;
}

XGINLINE BOOL
XGQUATERNION::operator != ( CONST XGQUATERNION& q ) const
{
    return x != q.x || y != q.y || z != q.z || w != q.w;
}




//--------------------------
// Plane
//--------------------------

XGINLINE
XGPLANE::XGPLANE( CONST FLOAT* pf )
{
#ifdef _DEBUG

#ifdef ASSERT
    ASSERT(pf != NULL);
#endif

#endif

    a = pf[0];
    b = pf[1];
    c = pf[2];
    d = pf[3];
}

XGINLINE
XGPLANE::XGPLANE( FLOAT fa, FLOAT fb, FLOAT fc, FLOAT fd )
{
    a = fa;
    b = fb;
    c = fc;
    d = fd;
}


// casting
XGINLINE
XGPLANE::operator FLOAT* ()
{
    return (FLOAT *) &a;
}

XGINLINE
XGPLANE::operator CONST FLOAT* () const
{
    return (CONST FLOAT *) &a;
}


// unary operators
XGINLINE XGPLANE
XGPLANE::operator + () const
{
    return *this;
}

XGINLINE XGPLANE
XGPLANE::operator - () const
{
    return XGPLANE(-a, -b, -c, -d);
}


// binary operators
XGINLINE BOOL
XGPLANE::operator == ( CONST XGPLANE& p ) const
{
    return a == p.a && b == p.b && c == p.c && d == p.d;
}

XGINLINE BOOL
XGPLANE::operator != ( CONST XGPLANE& p ) const
{
    return a != p.a || b != p.b || c != p.c || d != p.d;
}




//--------------------------
// Color
//--------------------------

XGINLINE
XGCOLOR::XGCOLOR( DWORD dw )
{
    CONST FLOAT f = 1.0f / 255.0f;
    r = f * (FLOAT) (unsigned char) (dw >> 16);
    g = f * (FLOAT) (unsigned char) (dw >>  8);
    b = f * (FLOAT) (unsigned char) (dw >>  0);
    a = f * (FLOAT) (unsigned char) (dw >> 24);
}

XGINLINE
XGCOLOR::XGCOLOR( CONST FLOAT* pf )
{
#ifdef _DEBUG

#ifdef ASSERT
    ASSERT(pf != NULL);
#endif

#endif

    r = pf[0];
    g = pf[1];
    b = pf[2];
    a = pf[3];
}

XGINLINE
XGCOLOR::XGCOLOR( CONST D3DCOLORVALUE& c )
{
    r = c.r;
    g = c.g;
    b = c.b;
    a = c.a;
}

XGINLINE
XGCOLOR::XGCOLOR( FLOAT fr, FLOAT fg, FLOAT fb, FLOAT fa )
{
    r = fr;
    g = fg;
    b = fb;
    a = fa;
}


// casting
XGINLINE
XGCOLOR::operator DWORD () const
{
    FLOAT R = r >= 1.0f ? 0xff : r <= 0.0f ? 0x00 : (r * 255.0f + 0.5f);
    FLOAT G = g >= 1.0f ? 0xff : g <= 0.0f ? 0x00 : (g * 255.0f + 0.5f);
    FLOAT B = b >= 1.0f ? 0xff : b <= 0.0f ? 0x00 : (b * 255.0f + 0.5f);
    FLOAT A = a >= 1.0f ? 0xff : a <= 0.0f ? 0x00 : (a * 255.0f + 0.5f);

    __asm {
        cvtss2si eax, B

        cvtss2si edx, A
        shl edx, 24
        or eax, edx

        cvtss2si edx, R
        shl edx, 16
        or eax, edx

        cvtss2si edx, G
        shl edx, 8
        or eax, edx
    }
}


XGINLINE
XGCOLOR::operator FLOAT * ()
{
    return (FLOAT *) &r;
}

XGINLINE
XGCOLOR::operator CONST FLOAT * () const
{
    return (CONST FLOAT *) &r;
}


XGINLINE
XGCOLOR::operator D3DCOLORVALUE * ()
{
    return (D3DCOLORVALUE *) &r;
}

XGINLINE
XGCOLOR::operator CONST D3DCOLORVALUE * () const
{
    return (CONST D3DCOLORVALUE *) &r;
}


XGINLINE
XGCOLOR::operator D3DCOLORVALUE& ()
{
    return *((D3DCOLORVALUE *) &r);
}

XGINLINE
XGCOLOR::operator CONST D3DCOLORVALUE& () const
{
    return *((CONST D3DCOLORVALUE *) &r);
}


// assignment operators
XGINLINE XGCOLOR&
XGCOLOR::operator += ( CONST XGCOLOR& c )
{
    r += c.r;
    g += c.g;
    b += c.b;
    a += c.a;
    return *this;
}

XGINLINE XGCOLOR&
XGCOLOR::operator -= ( CONST XGCOLOR& c )
{
    r -= c.r;
    g -= c.g;
    b -= c.b;
    a -= c.a;
    return *this;
}

XGINLINE XGCOLOR&
XGCOLOR::operator *= ( FLOAT f )
{
    r *= f;
    g *= f;
    b *= f;
    a *= f;
    return *this;
}

XGINLINE XGCOLOR&
XGCOLOR::operator /= ( FLOAT f )
{
    FLOAT fInv = 1.0f / f;
    r *= fInv;
    g *= fInv;
    b *= fInv;
    a *= fInv;
    return *this;
}


// unary operators
XGINLINE XGCOLOR
XGCOLOR::operator + () const
{
    return *this;
}

XGINLINE XGCOLOR
XGCOLOR::operator - () const
{
    return XGCOLOR(-r, -g, -b, -a);
}


// binary operators
XGINLINE XGCOLOR
XGCOLOR::operator + ( CONST XGCOLOR& c ) const
{
    return XGCOLOR(r + c.r, g + c.g, b + c.b, a + c.a);
}

XGINLINE XGCOLOR
XGCOLOR::operator - ( CONST XGCOLOR& c ) const
{
    return XGCOLOR(r - c.r, g - c.g, b - c.b, a - c.a);
}

XGINLINE XGCOLOR
XGCOLOR::operator * ( FLOAT f ) const
{
    return XGCOLOR(r * f, g * f, b * f, a * f);
}

XGINLINE XGCOLOR
XGCOLOR::operator / ( FLOAT f ) const
{
    FLOAT fInv = 1.0f / f;
    return XGCOLOR(r * fInv, g * fInv, b * fInv, a * fInv);
}


XGINLINE XGCOLOR
operator * (FLOAT f, CONST XGCOLOR& c )
{
    return XGCOLOR(f * c.r, f * c.g, f * c.b, f * c.a);
}


XGINLINE BOOL
XGCOLOR::operator == ( CONST XGCOLOR& c ) const
{
    return r == c.r && g == c.g && b == c.b && a == c.a;
}

XGINLINE BOOL
XGCOLOR::operator != ( CONST XGCOLOR& c ) const
{
    return r != c.r || g != c.g || b != c.b || a != c.a;
}

#endif //__cplusplus

//===========================================================================
//
// Inline functions
//
//===========================================================================


//--------------------------
// 2D Vector
//--------------------------

XGINLINE FLOAT XGVec2Length
    ( CONST XGVECTOR2 *pV )
{
    FLOAT res;

#ifdef _DEBUG

#ifdef ASSERT
    ASSERT(pV != NULL);
#endif

#endif

    __asm {
        mov     edx, pV
        movups  xmm1, [edx]
        movaps  xmm2, xmm1

        mulps   xmm1, xmm2

        movaps  xmm0, xmm1

        shufps  xmm0, xmm0, 1h
        addps   xmm1, xmm0

        sqrtss  xmm1, xmm1
        movss  [res], xmm1
    }

    return res;
}

XGINLINE FLOAT XGVec2LengthSq
    ( CONST XGVECTOR2 *pV )
{
    FLOAT res;

#ifdef _DEBUG

#ifdef ASSERT
    ASSERT(pV != NULL);
#endif

#endif

    __asm {
        mov     edx, pV
        movups  xmm1, [edx]
        movaps  xmm2, xmm1

        mulps   xmm1, xmm2

        movaps  xmm0, xmm1

        shufps  xmm0, xmm0, 1h
        addps   xmm1, xmm0

        movss  [res], xmm1
    }

    return res;
}

XGINLINE FLOAT XGVec2Dot
    ( CONST XGVECTOR2 *pV1, CONST XGVECTOR2 *pV2 )
{
    FLOAT res;

#ifdef _DEBUG

#ifdef ASSERT
    ASSERT(pV1 != NULL && pV2 != NULL);
#endif

#endif

    __asm {
        mov     edx, pV1
        movlps  xmm1, [edx]

        mov     ecx, pV2
        movlps  xmm2, [ecx]

        mulps   xmm1, xmm2

        movaps  xmm0, xmm1

        shufps  xmm0, xmm0, 1h
        addps   xmm1, xmm0

        movss  [res], xmm1
    }

    return res;
}

XGINLINE FLOAT XGVec2CCW
    ( CONST XGVECTOR2 *pV1, CONST XGVECTOR2 *pV2 )
{
#ifdef _DEBUG

#ifdef ASSERT
    ASSERT(pV1 != NULL && pV2 != NULL);
#endif

#endif

    return pV1->x * pV2->y - pV1->y * pV2->x;
}

XGINLINE XGVECTOR2* XGVec2Add
    ( XGVECTOR2 *pOut, CONST XGVECTOR2 *pV1, CONST XGVECTOR2 *pV2 )
{
#ifdef _DEBUG

#ifdef ASSERT
    ASSERT(pOut != NULL && pV1 != NULL && pV2 != NULL);
#endif

#endif

    __asm {
        mov     edx, pV1
        movlps  xmm1, [edx]

        mov     ecx, pV2
        movlps  xmm2, [ecx]

        addps   xmm1, xmm2

        mov     eax, pOut
        movlps  [eax], xmm1
    }
}

XGINLINE XGVECTOR2* XGVec2Subtract
    ( XGVECTOR2 *pOut, CONST XGVECTOR2 *pV1, CONST XGVECTOR2 *pV2 )
{
#ifdef _DEBUG

#ifdef ASSERT
    ASSERT(pOut != NULL && pV1 != NULL && pV2 != NULL);
#endif

#endif

    __asm {
        mov     edx, pV1
        movlps  xmm1, [edx]

        mov     ecx, pV2
        movlps  xmm2, [ecx]

        subps   xmm1, xmm2

        mov     eax, pOut
        movlps  [eax], xmm1
    }
}

XGINLINE XGVECTOR2* XGVec2Minimize
    ( XGVECTOR2 *pOut, CONST XGVECTOR2 *pV1, CONST XGVECTOR2 *pV2 )
{
#ifdef _DEBUG

#ifdef ASSERT
    ASSERT(pOut != NULL && pV1 != NULL && pV2 != NULL);
#endif

#endif

    __asm {
        mov      ecx, pV1
        mov      edx, pV2
        movlps   xmm0, [ecx]
        movlps   xmm1, [edx]
        minps    xmm0, xmm1
        mov      eax, pOut
        movlps   [eax], xmm0
    }
}

XGINLINE XGVECTOR2* XGVec2Maximize
    ( XGVECTOR2 *pOut, CONST XGVECTOR2 *pV1, CONST XGVECTOR2 *pV2 )
{
#ifdef _DEBUG

#ifdef ASSERT
    ASSERT(pOut != NULL && pV1 != NULL && pV2 != NULL);
#endif

#endif

    __asm {
        mov      ecx, pV1
        mov      edx, pV2
        movlps   xmm0, [ecx]
        movlps   xmm1, [edx]
        maxps    xmm0, xmm1
        mov      eax, pOut
        movlps   [eax], xmm0
    }
}

XGINLINE XGVECTOR2* XGVec2Scale
    ( XGVECTOR2 *pOut, CONST XGVECTOR2 *pV, FLOAT s )
{
#ifdef _DEBUG

#ifdef ASSERT
    ASSERT(pOut != NULL && pV != NULL);
#endif

#endif

    __asm {
        movss   xmm0, s
        shufps  xmm0, xmm0, 0h

        mov     ecx, pV
        movlps  xmm1, [ecx]

        mulps   xmm0, xmm1

        mov     eax, pOut
        movlps  [eax], xmm0
    }
}

XGINLINE XGVECTOR2* XGVec2Lerp
    ( XGVECTOR2 *pOut, CONST XGVECTOR2 *pV1, CONST XGVECTOR2 *pV2,
      FLOAT s )
{
#ifdef _DEBUG

#ifdef ASSERT
    ASSERT(pOut != NULL && pV1 != NULL && pV2 != NULL);
#endif

#endif

    __asm {
        movss   xmm0, s
        shufps  xmm0, xmm0, 0h

        mov     ecx, pV1
        movlps  xmm1, [ecx]

        mov     edx, pV2
        movlps  xmm2, [edx]

        subps   xmm2, xmm1
        mulps   xmm0, xmm2

        addps   xmm0, xmm1

        mov     eax, pOut
        movlps  [eax], xmm0
    }
}


//--------------------------
// 3D Vector
//--------------------------

XGINLINE FLOAT XGVec3Length
    ( CONST XGVECTOR3 *pV )
{
    FLOAT res;

#ifdef _DEBUG

#ifdef ASSERT
    ASSERT(pV != NULL);
#endif

#endif

    __asm {
        mov     edx, pV
        movss   xmm1, [edx]
        movhps  xmm1, [edx+4]

        movaps  xmm2, xmm1

        mulps   xmm1, xmm2

        movaps  xmm0, xmm1

        shufps  xmm0, xmm0, 32h
        addps   xmm1, xmm0

        shufps  xmm0, xmm0, 32h
        addps   xmm1, xmm0

        sqrtss  xmm1, xmm1
        movss   [res], xmm1
    }

    return res;
}

XGINLINE FLOAT XGVec3LengthSq
    ( CONST XGVECTOR3 *pV )
{
    FLOAT res;

#ifdef _DEBUG

#ifdef ASSERT
    ASSERT(pV != NULL);
#endif

#endif

    __asm {
        mov     edx, pV
        movss   xmm1, [edx]
        movhps  xmm1, [edx+4]

        movaps  xmm2, xmm1

        mulps   xmm1, xmm2

        movaps  xmm0, xmm1

        shufps  xmm0, xmm0, 32h
        addps   xmm1, xmm0

        shufps  xmm0, xmm0, 32h
        addps   xmm1, xmm0

        movss   [res], xmm1
    }

    return res;
}

XGINLINE FLOAT XGVec3Dot
    ( CONST XGVECTOR3 *pV1, CONST XGVECTOR3 *pV2 )
{
    FLOAT res;

#ifdef _DEBUG

#ifdef ASSERT
    ASSERT(pV1 != NULL && pV2 != NULL);
#endif

#endif

    __asm {
        mov     edx, pV1
        movss   xmm1, [edx]
        movhps  xmm1, [edx+4]

        mov     edx, pV2
        movss   xmm2, [edx]
        movhps  xmm2, [edx+4]

        mulps   xmm1, xmm2

        movaps  xmm0, xmm1

        shufps  xmm0, xmm0, 32h
        addps   xmm1, xmm0

        shufps  xmm0, xmm0, 32h
        addps   xmm1, xmm0

        movss   [res], xmm1
    }

    return res;
}

XGINLINE XGVECTOR3* XGVec3Cross
    ( XGVECTOR3 *pOut, CONST XGVECTOR3 *pV1, CONST XGVECTOR3 *pV2 )
{
    XGVECTOR3 v;

#ifdef _DEBUG

#ifdef ASSERT
    ASSERT(pOut != NULL && pV1 != NULL && pV2 != NULL);
#endif

#endif

    v.x = pV1->y * pV2->z - pV1->z * pV2->y;
    v.y = pV1->z * pV2->x - pV1->x * pV2->z;
    v.z = pV1->x * pV2->y - pV1->y * pV2->x;

    *pOut = v;
    return pOut;
}

XGINLINE XGVECTOR3* XGVec3Add
    ( XGVECTOR3 *pOut, CONST XGVECTOR3 *pV1, CONST XGVECTOR3 *pV2 )
{
#ifdef _DEBUG

#ifdef ASSERT
    ASSERT(pOut != NULL && pV1 != NULL && pV2 != NULL);
#endif

#endif

    __asm {
        mov      ecx, pV1
        movss    xmm0, [ecx]
        movhps   xmm0, [ecx+4]

        mov      edx, pV2
        movss    xmm1, [edx]
        movhps   xmm1, [edx+4]

        addps    xmm0, xmm1

        mov      eax, pOut
        movss    [eax], xmm0
        movhps   [eax+4], xmm0
    }
}

XGINLINE XGVECTOR3* XGVec3Subtract
    ( XGVECTOR3 *pOut, CONST XGVECTOR3 *pV1, CONST XGVECTOR3 *pV2 )
{
#ifdef _DEBUG

#ifdef ASSERT
    ASSERT(pOut != NULL && pV1 != NULL && pV2 != NULL);
#endif

#endif

    __asm {
        mov      ecx, pV1
        movss    xmm0, [ecx]
        movhps   xmm0, [ecx+4]

        mov      edx, pV2
        movss    xmm1, [edx]
        movhps   xmm1, [edx+4]

        subps    xmm0, xmm1

        mov      eax, pOut
        movss    [eax], xmm0
        movhps   [eax+4], xmm0
    }
}

XGINLINE XGVECTOR3* XGVec3Minimize
    ( XGVECTOR3 *pOut, CONST XGVECTOR3 *pV1, CONST XGVECTOR3 *pV2 )
{
#ifdef _DEBUG

#ifdef ASSERT
    ASSERT(pOut != NULL && pV1 != NULL && pV2 != NULL);
#endif

#endif

    __asm {
        mov      ecx, pV1
        movss    xmm0, [ecx]
        movhps   xmm0, [ecx+4]

        mov      edx, pV2
        movss    xmm1, [edx]
        movhps   xmm1, [edx+4]

        minps    xmm0, xmm1

        mov      eax, pOut
        movss    [eax], xmm0
        movhps   [eax+4], xmm0
    }
}

XGINLINE XGVECTOR3* XGVec3Maximize
    ( XGVECTOR3 *pOut, CONST XGVECTOR3 *pV1, CONST XGVECTOR3 *pV2 )
{
#ifdef _DEBUG

#ifdef ASSERT
    ASSERT(pOut != NULL && pV1 != NULL && pV2 != NULL);
#endif

#endif

    __asm {
        mov      ecx, pV1
        movss    xmm0, [ecx]
        movhps   xmm0, [ecx+4]

        mov      edx, pV2
        movss    xmm1, [edx]
        movhps   xmm1, [edx+4]

        maxps    xmm0, xmm1

        mov      eax, pOut
        movss    [eax], xmm0
        movhps   [eax+4], xmm0
    }
}

XGINLINE XGVECTOR3* XGVec3Scale
    ( XGVECTOR3 *pOut, CONST XGVECTOR3 *pV, FLOAT s)
{
#ifdef _DEBUG

#ifdef ASSERT
    ASSERT(pOut != NULL && pV != NULL);
#endif

#endif

    __asm {
        movss    xmm0, s
        shufps   xmm0, xmm0, 0h

        mov      edx, pV
        movss    xmm1, [edx]
        movhps   xmm1, [edx+4]

        mulps    xmm0, xmm1

        mov      eax, pOut
        movss    [eax], xmm0
        movhps   [eax+4], xmm0
    }
}

XGINLINE XGVECTOR3* XGVec3Lerp
    ( XGVECTOR3 *pOut, CONST XGVECTOR3 *pV1, CONST XGVECTOR3 *pV2,
      FLOAT s )
{
#ifdef _DEBUG

#ifdef ASSERT
    ASSERT(pOut != NULL && pV1 != NULL && pV2 != NULL);
#endif

#endif

    __asm {
        mov      ecx, pV1
        movss    xmm1, [ecx]
        movhps   xmm1, [ecx+4]

        mov      edx, pV2
        movss    xmm2, [edx]
        movhps   xmm2, [edx+4]

        subps    xmm2, xmm1

        movss    xmm0, s
        shufps   xmm0, xmm0, 0h

        mulps    xmm0, xmm2
        
        addps    xmm0, xmm1

        mov      eax, pOut
        movss    [eax], xmm0
        movhps   [eax+4], xmm0
    }
}


//--------------------------
// 4D Vector
//--------------------------

XGINLINE FLOAT XGVec4Length
    ( CONST XGVECTOR4 *pV )
{
    FLOAT res;

#ifdef _DEBUG

#ifdef ASSERT
    ASSERT(pV != NULL);
#endif

#endif

    __asm {
        mov     edx, pV
        movups  xmm1, [edx]
        movaps  xmm2, xmm1

        mulps   xmm1, xmm2

        movaps  xmm0, xmm1

        shufps  xmm0, xmm0, 93h
        addps   xmm1, xmm0

        shufps  xmm0, xmm0, 93h
        addps   xmm1, xmm0

        shufps  xmm0, xmm0, 93h
        addps   xmm1, xmm0

        sqrtss  xmm1, xmm1
        movss  [res], xmm1
    }

    return res;
}

XGINLINE FLOAT XGVec4LengthSq
    ( CONST XGVECTOR4 *pV )
{
    FLOAT res;

#ifdef _DEBUG

#ifdef ASSERT
    ASSERT(pV != NULL);
#endif

#endif

    __asm {
        mov     edx, pV
        movups  xmm1, [edx]
        movaps  xmm2, xmm1

        mulps   xmm1, xmm2

        movaps  xmm0, xmm1

        shufps  xmm0, xmm0, 93h
        addps   xmm1, xmm0

        shufps  xmm0, xmm0, 93h
        addps   xmm1, xmm0

        shufps  xmm0, xmm0, 93h
        addps   xmm1, xmm0

        movss  [res], xmm1
    }

    return res;
}

XGINLINE FLOAT XGVec4Dot
    ( CONST XGVECTOR4 *pV1, CONST XGVECTOR4 *pV2 )
{
    FLOAT res;

#ifdef _DEBUG

#ifdef ASSERT
    ASSERT(pV1 != NULL && pV2 != NULL);
#endif

#endif

    __asm {
        mov     edx, pV1
        movups  xmm1, [edx]

        mov     ecx, pV2
        movups  xmm2, [ecx]

        mulps   xmm1, xmm2

        movaps  xmm0, xmm1

        shufps  xmm0, xmm0, 93h
        addps   xmm1, xmm0

        shufps  xmm0, xmm0, 93h
        addps   xmm1, xmm0

        shufps  xmm0, xmm0, 93h
        addps   xmm1, xmm0

        movss  [res], xmm1
    }

    return res;
}

XGINLINE XGVECTOR4* XGVec4Add
    ( XGVECTOR4 *pOut, CONST XGVECTOR4 *pV1, CONST XGVECTOR4 *pV2)
{
#ifdef _DEBUG

#ifdef ASSERT
    ASSERT(pOut != NULL && pV1 != NULL && pV2 != NULL);
#endif

#endif

    __asm {
        mov     edx, pV1
        movups  xmm1, [edx]

        mov     ecx, pV2
        movups  xmm2, [ecx]

        addps   xmm1, xmm2

        mov     eax, pOut
        movups  [eax], xmm1
    }
}

XGINLINE XGVECTOR4* XGVec4Subtract
    ( XGVECTOR4 *pOut, CONST XGVECTOR4 *pV1, CONST XGVECTOR4 *pV2)
{
#ifdef _DEBUG

#ifdef ASSERT
    ASSERT(pOut != NULL && pV1 != NULL && pV2 != NULL);
#endif

#endif

    __asm {
        mov     edx, pV1
        movups  xmm1, [edx]

        mov     ecx, pV2
        movups  xmm2, [ecx]

        subps   xmm1, xmm2

        mov     eax, pOut
        movups  [eax], xmm1
    }
}

XGINLINE XGVECTOR4* XGVec4Minimize
    ( XGVECTOR4 *pOut, CONST XGVECTOR4 *pV1, CONST XGVECTOR4 *pV2)
{
#ifdef _DEBUG

#ifdef ASSERT
    ASSERT(pOut != NULL && pV1 != NULL && pV2 != NULL);
#endif

#endif

    __asm {
        mov      ecx, pV1
        mov      edx, pV2
        movups   xmm0, [ecx]
        movups   xmm1, [edx]
        minps    xmm0, xmm1
        mov      eax, pOut
        movups   [eax], xmm0
    }
}

XGINLINE XGVECTOR4* XGVec4Maximize
    ( XGVECTOR4 *pOut, CONST XGVECTOR4 *pV1, CONST XGVECTOR4 *pV2)
{
#ifdef _DEBUG

#ifdef ASSERT
    ASSERT(pOut != NULL && pV1 != NULL && pV2 != NULL);
#endif

#endif

    __asm {
        mov      ecx, pV1
        mov      edx, pV2
        movups   xmm0, [ecx]
        movups   xmm1, [edx]
        maxps    xmm0, xmm1
        mov      eax, pOut
        movups   [eax], xmm0
    }
}

XGINLINE XGVECTOR4* XGVec4Scale
    ( XGVECTOR4 *pOut, CONST XGVECTOR4 *pV, FLOAT s)
{
#ifdef _DEBUG

#ifdef ASSERT
    ASSERT(pOut != NULL && pV);
#endif

#endif

    __asm {
        movss   xmm0, s
        shufps  xmm0, xmm0, 0h

        mov     ecx, pV
        movups  xmm1, [ecx]

        mulps   xmm0, xmm1

        mov     eax, pOut
        movups  [eax], xmm0
    }
}

XGINLINE XGVECTOR4* XGVec4Lerp
    ( XGVECTOR4 *pOut, CONST XGVECTOR4 *pV1, CONST XGVECTOR4 *pV2,
      FLOAT s )
{
#ifdef _DEBUG

#ifdef ASSERT
    ASSERT(pOut != NULL && pV1 != NULL && pV2 != NULL);
#endif

#endif

    __asm {
        movss   xmm0, s
        shufps  xmm0, xmm0, 0h

        mov     ecx, pV1
        movups  xmm1, [ecx]

        mov     edx, pV2
        movups  xmm2, [edx]

        subps   xmm2, xmm1
        mulps   xmm0, xmm2

        addps   xmm0, xmm1

        mov     eax, pOut
        movups  [eax], xmm0
    }
}


//--------------------------
// 4D Matrix
//--------------------------

XGINLINE XGMATRIX* XGMatrixIdentity
    ( XGMATRIX *pOut )
{
#ifdef _DEBUG

#ifdef ASSERT
    ASSERT(pOut != NULL);
#endif

#endif

    pOut->m[0][1] = pOut->m[0][2] = pOut->m[0][3] =
    pOut->m[1][0] = pOut->m[1][2] = pOut->m[1][3] =
    pOut->m[2][0] = pOut->m[2][1] = pOut->m[2][3] =
    pOut->m[3][0] = pOut->m[3][1] = pOut->m[3][2] = 0.0f;

    pOut->m[0][0] = pOut->m[1][1] = pOut->m[2][2] = pOut->m[3][3] = 1.0f;
    return pOut;
}


XGINLINE BOOL XGMatrixIsIdentity
    ( CONST XGMATRIX *pM )
{
#ifdef _DEBUG

#ifdef ASSERT
    ASSERT(pM != NULL);
#endif

#endif

    return pM->m[0][0] == 1.0f && pM->m[0][1] == 0.0f && pM->m[0][2] == 0.0f && pM->m[0][3] == 0.0f &&
           pM->m[1][0] == 0.0f && pM->m[1][1] == 1.0f && pM->m[1][2] == 0.0f && pM->m[1][3] == 0.0f &&
           pM->m[2][0] == 0.0f && pM->m[2][1] == 0.0f && pM->m[2][2] == 1.0f && pM->m[2][3] == 0.0f &&
           pM->m[3][0] == 0.0f && pM->m[3][1] == 0.0f && pM->m[3][2] == 0.0f && pM->m[3][3] == 1.0f;
}


//--------------------------
// Quaternion
//--------------------------

XGINLINE FLOAT XGQuaternionLength
    ( CONST XGQUATERNION *pQ )
{
    FLOAT res;

#ifdef _DEBUG

#ifdef ASSERT
    ASSERT(pQ != NULL);
#endif

#endif

    __asm {
        mov     edx, pQ
        movups  xmm1, [edx]
        movaps  xmm2, xmm1

        mulps   xmm1, xmm2

        movaps  xmm0, xmm1

        shufps  xmm0, xmm0, 93h
        addps   xmm1, xmm0

        shufps  xmm0, xmm0, 93h
        addps   xmm1, xmm0

        shufps  xmm0, xmm0, 93h
        addps   xmm1, xmm0

        sqrtss  xmm1, xmm1
        movss  [res], xmm1
    }

    return res;
}

XGINLINE FLOAT XGQuaternionLengthSq
    ( CONST XGQUATERNION *pQ )
{
    FLOAT res;

#ifdef _DEBUG

#ifdef ASSERT
    ASSERT(pQ != NULL);
#endif

#endif
    
    __asm {
        mov     edx, pQ
        movups  xmm1, [edx]
        movaps  xmm2, xmm1

        mulps   xmm1, xmm2

        movaps  xmm0, xmm1

        shufps  xmm0, xmm0, 93h
        addps   xmm1, xmm0

        shufps  xmm0, xmm0, 93h
        addps   xmm1, xmm0

        shufps  xmm0, xmm0, 93h
        addps   xmm1, xmm0

        movss  [res], xmm1
    }

    return res;
}

XGINLINE FLOAT XGQuaternionDot
    ( CONST XGQUATERNION *pQ1, CONST XGQUATERNION *pQ2 )
{
    FLOAT res;

#ifdef _DEBUG

#ifdef ASSERT
    ASSERT(pQ1 != NULL && pQ2 != NULL);
#endif

#endif

    __asm {
        mov     edx, pQ1
        movups  xmm1, [edx]

        mov     ecx, pQ2
        movups  xmm2, [ecx]

        mulps   xmm1, xmm2

        movaps  xmm0, xmm1

        shufps  xmm0, xmm0, 93h
        addps   xmm1, xmm0

        shufps  xmm0, xmm0, 93h
        addps   xmm1, xmm0

        shufps  xmm0, xmm0, 93h
        addps   xmm1, xmm0

        movss  [res], xmm1
    }

    return res;
}

XGINLINE XGQUATERNION* XGQuaternionIdentity
    ( XGQUATERNION *pOut )
{
#ifdef _DEBUG

#ifdef ASSERT
    ASSERT(pOut != NULL);
#endif

#endif

    pOut->x = pOut->y = pOut->z = 0.0f;
    pOut->w = 1.0f;
    return pOut;
}

XGINLINE BOOL XGQuaternionIsIdentity
    ( CONST XGQUATERNION *pQ )
{
#ifdef _DEBUG

#ifdef ASSERT
    ASSERT(pQ != NULL);
#endif

#endif

    return pQ->x == 0.0f && pQ->y == 0.0f && pQ->z == 0.0f && pQ->w == 1.0f;
}


XGINLINE XGQUATERNION* XGQuaternionConjugate
    ( XGQUATERNION *pOut, CONST XGQUATERNION *pQ )
{
#ifdef _DEBUG

#ifdef ASSERT
    ASSERT(pOut != NULL && pQ != NULL);
#endif

#endif

    pOut->x = -pQ->x;
    pOut->y = -pQ->y;
    pOut->z = -pQ->z;
    pOut->w =  pQ->w;
    return pOut;
}

//--------------------------
// Plane
//--------------------------

XGINLINE FLOAT XGPlaneDot
    ( CONST XGPLANE *pP, CONST XGVECTOR4 *pV)
{
    FLOAT res;

#ifdef _DEBUG

#ifdef ASSERT
    ASSERT(pP != NULL && pV != NULL);
#endif

#endif

    __asm {
        mov     edx, pP
        movups  xmm1, [edx]

        mov     ecx, pV
        movups  xmm2, [ecx]

        mulps   xmm1, xmm2

        movaps  xmm0, xmm1

        shufps  xmm0, xmm0, 93h
        addps   xmm1, xmm0

        shufps  xmm0, xmm0, 93h
        addps   xmm1, xmm0

        shufps  xmm0, xmm0, 93h
        addps   xmm1, xmm0

        movss  [res], xmm1
    }

    return res;
}

XGINLINE FLOAT XGPlaneDotCoord
    ( CONST XGPLANE *pP, CONST XGVECTOR3 *pV)
{
#ifdef _DEBUG

#ifdef ASSERT
    ASSERT(pP != NULL && pV != NULL);
#endif

#endif

    return pP->a * pV->x + pP->b * pV->y + pP->c * pV->z + pP->d;
}

XGINLINE FLOAT XGPlaneDotNormal
    ( CONST XGPLANE *pP, CONST XGVECTOR3 *pV)
{
#ifdef _DEBUG

#ifdef ASSERT
    ASSERT(pP != NULL && pV != NULL);
#endif

#endif

    return pP->a * pV->x + pP->b * pV->y + pP->c * pV->z;
}


//--------------------------
// Color
//--------------------------

XGINLINE XGCOLOR* XGColorNegative
    (XGCOLOR *pOut, CONST XGCOLOR *pC)
{
#ifdef _DEBUG

#ifdef ASSERT
    ASSERT(pOut != NULL && pC != NULL);
#endif

#endif

    pOut->r = 1.0f - pC->r;
    pOut->g = 1.0f - pC->g;
    pOut->b = 1.0f - pC->b;
    pOut->a = pC->a;
    return pOut;
}

XGINLINE XGCOLOR* XGColorAdd
    (XGCOLOR *pOut, CONST XGCOLOR *pC1, CONST XGCOLOR *pC2)
{
#ifdef _DEBUG

#ifdef ASSERT
    ASSERT(pOut != NULL && pC1 != NULL && pC2 != NULL);
#endif

#endif

    __asm {
        mov     edx, pC1
        movups  xmm1, [edx]

        mov     ecx, pC2
        movups  xmm2, [ecx]

        addps   xmm1, xmm2

        mov     eax, pOut
        movups  [eax], xmm1
    }
}

XGINLINE XGCOLOR* XGColorSubtract
    (XGCOLOR *pOut, CONST XGCOLOR *pC1, CONST XGCOLOR *pC2)
{
#ifdef _DEBUG

#ifdef ASSERT
    ASSERT(pOut != NULL && pC1 != NULL && pC2 != NULL);
#endif

#endif

    __asm {
        mov     edx, pC1
        movups  xmm1, [edx]

        mov     ecx, pC2
        movups  xmm2, [ecx]

        subps   xmm1, xmm2

        mov     eax, pOut
        movups  [eax], xmm1
    }
}

XGINLINE XGCOLOR* XGColorScale
    (XGCOLOR *pOut, CONST XGCOLOR *pC, FLOAT s)
{
#ifdef _DEBUG

#ifdef ASSERT
    ASSERT(pOut != NULL && pC != NULL);
#endif

#endif

    __asm {
        movss   xmm0, s
        shufps  xmm0, xmm0, 0h

        mov     ecx, pC
        movups  xmm1, [ecx]

        mulps   xmm0, xmm1

        mov     eax, pOut
        movups  [eax], xmm0
    }
}

XGINLINE XGCOLOR* XGColorModulate
    (XGCOLOR *pOut, CONST XGCOLOR *pC1, CONST XGCOLOR *pC2)
{
#ifdef _DEBUG

#ifdef ASSERT
    ASSERT(pOut != NULL && pC1 != NULL && pC2 != NULL);
#endif

#endif

    __asm {
        mov     edx, pC1
        movups  xmm1, [edx]

        mov     ecx, pC2
        movups  xmm2, [ecx]

        mulps   xmm1, xmm2

        mov     eax, pOut
        movups  [eax], xmm1
    }
}

XGINLINE XGCOLOR* XGColorLerp
    (XGCOLOR *pOut, CONST XGCOLOR *pC1, CONST XGCOLOR *pC2, FLOAT s)
{
#ifdef _DEBUG

#ifdef ASSERT
    ASSERT(pOut != NULL && pC1 != NULL && pC2 != NULL);
#endif

#endif

    __asm {
        movss   xmm0, s
        shufps  xmm0, xmm0, 0h

        mov     ecx, pC1
        movups  xmm1, [ecx]

        mov     edx, pC2
        movups  xmm2, [edx]

        subps   xmm2, xmm1
        mulps   xmm0, xmm2

        addps   xmm0, xmm1

        mov     eax, pOut
        movups  [eax], xmm0
    }
}

#pragma warning(default:4035)

#endif // __XG8MATH_INL__
