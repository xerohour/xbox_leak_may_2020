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
    if(!pf)
        return;
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
    if(!pf)
        return;
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
    if(!pf)
        return;
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
    if(!pf)
        return;
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
    if(!pf)
        return;
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
    if(!pf)
        return;
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
    if(!pf)
        return;
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
    DWORD dwR = r >= 1.0f ? 0xff : r <= 0.0f ? 0x00 : (DWORD) (r * 255.0f + 0.5f);
    DWORD dwG = g >= 1.0f ? 0xff : g <= 0.0f ? 0x00 : (DWORD) (g * 255.0f + 0.5f);
    DWORD dwB = b >= 1.0f ? 0xff : b <= 0.0f ? 0x00 : (DWORD) (b * 255.0f + 0.5f);
    DWORD dwA = a >= 1.0f ? 0xff : a <= 0.0f ? 0x00 : (DWORD) (a * 255.0f + 0.5f);

    return (dwA << 24) | (dwR << 16) | (dwG << 8) | dwB;
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
#ifdef _DEBUG
    if(!pV)
        return 0.0f;
#endif

#ifdef __cplusplus
    return sqrtf(pV->x * pV->x + pV->y * pV->y);
#else
    return (FLOAT) sqrt(pV->x * pV->x + pV->y * pV->y);
#endif
}

XGINLINE FLOAT XGVec2LengthSq
    ( CONST XGVECTOR2 *pV )
{
#ifdef _DEBUG
    if(!pV)
        return 0.0f;
#endif

    return pV->x * pV->x + pV->y * pV->y;
}

XGINLINE FLOAT XGVec2Dot
    ( CONST XGVECTOR2 *pV1, CONST XGVECTOR2 *pV2 )
{
#ifdef _DEBUG
    if(!pV1 || !pV2)
        return 0.0f;
#endif

    return pV1->x * pV2->x + pV1->y * pV2->y;
}

XGINLINE FLOAT XGVec2CCW
    ( CONST XGVECTOR2 *pV1, CONST XGVECTOR2 *pV2 )
{
#ifdef _DEBUG
    if(!pV1 || !pV2)
        return 0.0f;
#endif

    return pV1->x * pV2->y - pV1->y * pV2->x;
}

XGINLINE XGVECTOR2* XGVec2Add
    ( XGVECTOR2 *pOut, CONST XGVECTOR2 *pV1, CONST XGVECTOR2 *pV2 )
{
#ifdef _DEBUG
    if(!pOut || !pV1 || !pV2)
        return NULL;
#endif

    pOut->x = pV1->x + pV2->x;
    pOut->y = pV1->y + pV2->y;
    return pOut;
}

XGINLINE XGVECTOR2* XGVec2Subtract
    ( XGVECTOR2 *pOut, CONST XGVECTOR2 *pV1, CONST XGVECTOR2 *pV2 )
{
#ifdef _DEBUG
    if(!pOut || !pV1 || !pV2)
        return NULL;
#endif

    pOut->x = pV1->x - pV2->x;
    pOut->y = pV1->y - pV2->y;
    return pOut;
}

XGINLINE XGVECTOR2* XGVec2Minimize
    ( XGVECTOR2 *pOut, CONST XGVECTOR2 *pV1, CONST XGVECTOR2 *pV2 )
{
#ifdef _DEBUG
    if(!pOut || !pV1 || !pV2)
        return NULL;
#endif

    pOut->x = pV1->x < pV2->x ? pV1->x : pV2->x;
    pOut->y = pV1->y < pV2->y ? pV1->y : pV2->y;
    return pOut;
}

XGINLINE XGVECTOR2* XGVec2Maximize
    ( XGVECTOR2 *pOut, CONST XGVECTOR2 *pV1, CONST XGVECTOR2 *pV2 )
{
#ifdef _DEBUG
    if(!pOut || !pV1 || !pV2)
        return NULL;
#endif

    pOut->x = pV1->x > pV2->x ? pV1->x : pV2->x;
    pOut->y = pV1->y > pV2->y ? pV1->y : pV2->y;
    return pOut;
}

XGINLINE XGVECTOR2* XGVec2Scale
    ( XGVECTOR2 *pOut, CONST XGVECTOR2 *pV, FLOAT s )
{
#ifdef _DEBUG
    if(!pOut || !pV)
        return NULL;
#endif

    pOut->x = pV->x * s;
    pOut->y = pV->y * s;
    return pOut;
}

XGINLINE XGVECTOR2* XGVec2Lerp
    ( XGVECTOR2 *pOut, CONST XGVECTOR2 *pV1, CONST XGVECTOR2 *pV2,
      FLOAT s )
{
#ifdef _DEBUG
    if(!pOut || !pV1 || !pV2)
        return NULL;
#endif

    pOut->x = pV1->x + s * (pV2->x - pV1->x);
    pOut->y = pV1->y + s * (pV2->y - pV1->y);
    return pOut;
}


//--------------------------
// 3D Vector
//--------------------------

XGINLINE FLOAT XGVec3Length
    ( CONST XGVECTOR3 *pV )
{
#ifdef _DEBUG
    if(!pV)
        return 0.0f;
#endif

#ifdef __cplusplus
    return sqrtf(pV->x * pV->x + pV->y * pV->y + pV->z * pV->z);
#else
    return (FLOAT) sqrt(pV->x * pV->x + pV->y * pV->y + pV->z * pV->z);
#endif
}

XGINLINE FLOAT XGVec3LengthSq
    ( CONST XGVECTOR3 *pV )
{
#ifdef _DEBUG
    if(!pV)
        return 0.0f;
#endif

    return pV->x * pV->x + pV->y * pV->y + pV->z * pV->z;
}

XGINLINE FLOAT XGVec3Dot
    ( CONST XGVECTOR3 *pV1, CONST XGVECTOR3 *pV2 )
{
#ifdef _DEBUG
    if(!pV1 || !pV2)
        return 0.0f;
#endif

    return pV1->x * pV2->x + pV1->y * pV2->y + pV1->z * pV2->z;
}

XGINLINE XGVECTOR3* XGVec3Cross
    ( XGVECTOR3 *pOut, CONST XGVECTOR3 *pV1, CONST XGVECTOR3 *pV2 )
{
    XGVECTOR3 v;

#ifdef _DEBUG
    if(!pOut || !pV1 || !pV2)
        return NULL;
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
    if(!pOut || !pV1 || !pV2)
        return NULL;
#endif

    pOut->x = pV1->x + pV2->x;
    pOut->y = pV1->y + pV2->y;
    pOut->z = pV1->z + pV2->z;
    return pOut;
}

XGINLINE XGVECTOR3* XGVec3Subtract
    ( XGVECTOR3 *pOut, CONST XGVECTOR3 *pV1, CONST XGVECTOR3 *pV2 )
{
#ifdef _DEBUG
    if(!pOut || !pV1 || !pV2)
        return NULL;
#endif

    pOut->x = pV1->x - pV2->x;
    pOut->y = pV1->y - pV2->y;
    pOut->z = pV1->z - pV2->z;
    return pOut;
}

XGINLINE XGVECTOR3* XGVec3Minimize
    ( XGVECTOR3 *pOut, CONST XGVECTOR3 *pV1, CONST XGVECTOR3 *pV2 )
{
#ifdef _DEBUG
    if(!pOut || !pV1 || !pV2)
        return NULL;
#endif

    pOut->x = pV1->x < pV2->x ? pV1->x : pV2->x;
    pOut->y = pV1->y < pV2->y ? pV1->y : pV2->y;
    pOut->z = pV1->z < pV2->z ? pV1->z : pV2->z;
    return pOut;
}

XGINLINE XGVECTOR3* XGVec3Maximize
    ( XGVECTOR3 *pOut, CONST XGVECTOR3 *pV1, CONST XGVECTOR3 *pV2 )
{
#ifdef _DEBUG
    if(!pOut || !pV1 || !pV2)
        return NULL;
#endif

    pOut->x = pV1->x > pV2->x ? pV1->x : pV2->x;
    pOut->y = pV1->y > pV2->y ? pV1->y : pV2->y;
    pOut->z = pV1->z > pV2->z ? pV1->z : pV2->z;
    return pOut;
}

XGINLINE XGVECTOR3* XGVec3Scale
    ( XGVECTOR3 *pOut, CONST XGVECTOR3 *pV, FLOAT s)
{
#ifdef _DEBUG
    if(!pOut || !pV)
        return NULL;
#endif

    pOut->x = pV->x * s;
    pOut->y = pV->y * s;
    pOut->z = pV->z * s;
    return pOut;
}

XGINLINE XGVECTOR3* XGVec3Lerp
    ( XGVECTOR3 *pOut, CONST XGVECTOR3 *pV1, CONST XGVECTOR3 *pV2,
      FLOAT s )
{
#ifdef _DEBUG
    if(!pOut || !pV1 || !pV2)
        return NULL;
#endif

    pOut->x = pV1->x + s * (pV2->x - pV1->x);
    pOut->y = pV1->y + s * (pV2->y - pV1->y);
    pOut->z = pV1->z + s * (pV2->z - pV1->z);
    return pOut;
}


//--------------------------
// 4D Vector
//--------------------------

XGINLINE FLOAT XGVec4Length
    ( CONST XGVECTOR4 *pV )
{
#ifdef _DEBUG
    if(!pV)
        return 0.0f;
#endif

#ifdef __cplusplus
    return sqrtf(pV->x * pV->x + pV->y * pV->y + pV->z * pV->z + pV->w * pV->w);
#else
    return (FLOAT) sqrt(pV->x * pV->x + pV->y * pV->y + pV->z * pV->z + pV->w * pV->w);
#endif
}

XGINLINE FLOAT XGVec4LengthSq
    ( CONST XGVECTOR4 *pV )
{
#ifdef _DEBUG
    if(!pV)
        return 0.0f;
#endif

    return pV->x * pV->x + pV->y * pV->y + pV->z * pV->z + pV->w * pV->w;
}

XGINLINE FLOAT XGVec4Dot
    ( CONST XGVECTOR4 *pV1, CONST XGVECTOR4 *pV2 )
{
#ifdef _DEBUG
    if(!pV1 || !pV2)
        return 0.0f;
#endif

    return pV1->x * pV2->x + pV1->y * pV2->y + pV1->z * pV2->z + pV1->w * pV2->w;
}

XGINLINE XGVECTOR4* XGVec4Add
    ( XGVECTOR4 *pOut, CONST XGVECTOR4 *pV1, CONST XGVECTOR4 *pV2)
{
#ifdef _DEBUG
    if(!pOut || !pV1 || !pV2)
        return NULL;
#endif

    pOut->x = pV1->x + pV2->x;
    pOut->y = pV1->y + pV2->y;
    pOut->z = pV1->z + pV2->z;
    pOut->w = pV1->w + pV2->w;
    return pOut;
}

XGINLINE XGVECTOR4* XGVec4Subtract
    ( XGVECTOR4 *pOut, CONST XGVECTOR4 *pV1, CONST XGVECTOR4 *pV2)
{
#ifdef _DEBUG
    if(!pOut || !pV1 || !pV2)
        return NULL;
#endif

    pOut->x = pV1->x - pV2->x;
    pOut->y = pV1->y - pV2->y;
    pOut->z = pV1->z - pV2->z;
    pOut->w = pV1->w - pV2->w;
    return pOut;
}

XGINLINE XGVECTOR4* XGVec4Minimize
    ( XGVECTOR4 *pOut, CONST XGVECTOR4 *pV1, CONST XGVECTOR4 *pV2)
{
#ifdef _DEBUG
    if(!pOut || !pV1 || !pV2)
        return NULL;
#endif

    pOut->x = pV1->x < pV2->x ? pV1->x : pV2->x;
    pOut->y = pV1->y < pV2->y ? pV1->y : pV2->y;
    pOut->z = pV1->z < pV2->z ? pV1->z : pV2->z;
    pOut->w = pV1->w < pV2->w ? pV1->w : pV2->w;
    return pOut;
}

XGINLINE XGVECTOR4* XGVec4Maximize
    ( XGVECTOR4 *pOut, CONST XGVECTOR4 *pV1, CONST XGVECTOR4 *pV2)
{
#ifdef _DEBUG
    if(!pOut || !pV1 || !pV2)
        return NULL;
#endif

    pOut->x = pV1->x > pV2->x ? pV1->x : pV2->x;
    pOut->y = pV1->y > pV2->y ? pV1->y : pV2->y;
    pOut->z = pV1->z > pV2->z ? pV1->z : pV2->z;
    pOut->w = pV1->w > pV2->w ? pV1->w : pV2->w;
    return pOut;
}

XGINLINE XGVECTOR4* XGVec4Scale
    ( XGVECTOR4 *pOut, CONST XGVECTOR4 *pV, FLOAT s)
{
#ifdef _DEBUG
    if(!pOut || !pV)
        return NULL;
#endif

    pOut->x = pV->x * s;
    pOut->y = pV->y * s;
    pOut->z = pV->z * s;
    pOut->w = pV->w * s;
    return pOut;
}

XGINLINE XGVECTOR4* XGVec4Lerp
    ( XGVECTOR4 *pOut, CONST XGVECTOR4 *pV1, CONST XGVECTOR4 *pV2,
      FLOAT s )
{
#ifdef _DEBUG
    if(!pOut || !pV1 || !pV2)
        return NULL;
#endif

    pOut->x = pV1->x + s * (pV2->x - pV1->x);
    pOut->y = pV1->y + s * (pV2->y - pV1->y);
    pOut->z = pV1->z + s * (pV2->z - pV1->z);
    pOut->w = pV1->w + s * (pV2->w - pV1->w);
    return pOut;
}


//--------------------------
// 4D Matrix
//--------------------------

XGINLINE XGMATRIX* XGMatrixIdentity
    ( XGMATRIX *pOut )
{
#ifdef _DEBUG
    if(!pOut)
        return NULL;
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
    if(!pM)
        return FALSE;
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
#ifdef _DEBUG
    if(!pQ)
        return 0.0f;
#endif

#ifdef __cplusplus
    return sqrtf(pQ->x * pQ->x + pQ->y * pQ->y + pQ->z * pQ->z + pQ->w * pQ->w);
#else
    return (FLOAT) sqrt(pQ->x * pQ->x + pQ->y * pQ->y + pQ->z * pQ->z + pQ->w * pQ->w);
#endif
}

XGINLINE FLOAT XGQuaternionLengthSq
    ( CONST XGQUATERNION *pQ )
{
#ifdef _DEBUG
    if(!pQ)
        return 0.0f;
#endif
    
    return pQ->x * pQ->x + pQ->y * pQ->y + pQ->z * pQ->z + pQ->w * pQ->w;
}

XGINLINE FLOAT XGQuaternionDot
    ( CONST XGQUATERNION *pQ1, CONST XGQUATERNION *pQ2 )
{
#ifdef _DEBUG
    if(!pQ1 || !pQ2)
        return 0.0f;
#endif

    return pQ1->x * pQ2->x + pQ1->y * pQ2->y + pQ1->z * pQ2->z + pQ1->w * pQ2->w;
}


XGINLINE XGQUATERNION* XGQuaternionIdentity
    ( XGQUATERNION *pOut )
{
#ifdef _DEBUG
    if(!pOut)
        return NULL;
#endif

    pOut->x = pOut->y = pOut->z = 0.0f;
    pOut->w = 1.0f;
    return pOut;
}

XGINLINE BOOL XGQuaternionIsIdentity
    ( CONST XGQUATERNION *pQ )
{
#ifdef _DEBUG
    if(!pQ)
        return FALSE;
#endif

    return pQ->x == 0.0f && pQ->y == 0.0f && pQ->z == 0.0f && pQ->w == 1.0f;
}


XGINLINE XGQUATERNION* XGQuaternionConjugate
    ( XGQUATERNION *pOut, CONST XGQUATERNION *pQ )
{
#ifdef _DEBUG
    if(!pOut || !pQ)
        return NULL;
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
#ifdef _DEBUG
    if(!pP || !pV)
        return 0.0f;
#endif

    return pP->a * pV->x + pP->b * pV->y + pP->c * pV->z + pP->d * pV->w;
}

XGINLINE FLOAT XGPlaneDotCoord
    ( CONST XGPLANE *pP, CONST XGVECTOR3 *pV)
{
#ifdef _DEBUG
    if(!pP || !pV)
        return 0.0f;
#endif

    return pP->a * pV->x + pP->b * pV->y + pP->c * pV->z + pP->d;
}

XGINLINE FLOAT XGPlaneDotNormal
    ( CONST XGPLANE *pP, CONST XGVECTOR3 *pV)
{
#ifdef _DEBUG
    if(!pP || !pV)
        return 0.0f;
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
    if(!pOut || !pC)
        return NULL;
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
    if(!pOut || !pC1 || !pC2)
        return NULL;
#endif

    pOut->r = pC1->r + pC2->r;
    pOut->g = pC1->g + pC2->g;
    pOut->b = pC1->b + pC2->b;
    pOut->a = pC1->a + pC2->a;
    return pOut;
}

XGINLINE XGCOLOR* XGColorSubtract
    (XGCOLOR *pOut, CONST XGCOLOR *pC1, CONST XGCOLOR *pC2)
{
#ifdef _DEBUG
    if(!pOut || !pC1 || !pC2)
        return NULL;
#endif

    pOut->r = pC1->r - pC2->r;
    pOut->g = pC1->g - pC2->g;
    pOut->b = pC1->b - pC2->b;
    pOut->a = pC1->a - pC2->a;
    return pOut;
}

XGINLINE XGCOLOR* XGColorScale
    (XGCOLOR *pOut, CONST XGCOLOR *pC, FLOAT s)
{
#ifdef _DEBUG
    if(!pOut || !pC)
        return NULL;
#endif

    pOut->r = pC->r * s;
    pOut->g = pC->g * s;
    pOut->b = pC->b * s;
    pOut->a = pC->a * s;
    return pOut;
}

XGINLINE XGCOLOR* XGColorModulate
    (XGCOLOR *pOut, CONST XGCOLOR *pC1, CONST XGCOLOR *pC2)
{
#ifdef _DEBUG
    if(!pOut || !pC1 || !pC2)
        return NULL;
#endif

    pOut->r = pC1->r * pC2->r;
    pOut->g = pC1->g * pC2->g;
    pOut->b = pC1->b * pC2->b;
    pOut->a = pC1->a * pC2->a;
    return pOut;
}

XGINLINE XGCOLOR* XGColorLerp
    (XGCOLOR *pOut, CONST XGCOLOR *pC1, CONST XGCOLOR *pC2, FLOAT s)
{
#ifdef _DEBUG
    if(!pOut || !pC1 || !pC2)
        return NULL;
#endif

    pOut->r = pC1->r + s * (pC2->r - pC1->r);
    pOut->g = pC1->g + s * (pC2->g - pC1->g);
    pOut->b = pC1->b + s * (pC2->b - pC1->b);
    pOut->a = pC1->a + s * (pC2->a - pC1->a);
    return pOut;
}

#endif // __XG8MATH_INL__
