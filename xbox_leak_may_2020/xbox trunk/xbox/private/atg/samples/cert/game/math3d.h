//-----------------------------------------------------------------------------
// File: Math3d.h
//
// Desc: Math objects
//
// Hist: 04.10.01 - New for May XDK release 
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#ifndef TECH_CERT_GAME_MATH_H
#define TECH_CERT_GAME_MATH_H

#include "Common.h"
#pragma inline_depth(255)
#define inline __forceinline
#include <cfloat>




//-----------------------------------------------------------------------------
// Constants
//-----------------------------------------------------------------------------
const FLOAT fPI = 3.14159f;




//-----------------------------------------------------------------------------
// Name: Clamp()
// Desc: Clamp value to the given range
//-----------------------------------------------------------------------------
template <class T>
T Clamp( T Minimum, T Value, T Maximum )
{
    if( Value < Minimum )
        return Minimum;

    if( Value > Maximum )
        return Maximum;

    return Value;
}




//-----------------------------------------------------------------------------
// Name: Sqr()
// Desc: Returns x * x
//-----------------------------------------------------------------------------
template <class T>
T Sqr( T x )
{
    return x * x;
}




//-----------------------------------------------------------------------------
// Name: Cube()
// Desc: Returns x * x * x
//-----------------------------------------------------------------------------
template <class T>
T Cube( T x )
{
    return x * x * x;
}




//-----------------------------------------------------------------------------
// Name: struct Vector3
// Desc: Simple wrapper around D3DXVECTOR3
//-----------------------------------------------------------------------------
struct Vector3 : public D3DXVECTOR3
{

public:

    inline Vector3();
    inline Vector3( FLOAT x, FLOAT y, FLOAT z );

    inline Vector3( const D3DXVECTOR3& );
    inline Vector3& operator=( const D3DXVECTOR3& );

    // Operators
    inline Vector3 operator-() const;
    inline Vector3 operator+( const Vector3& ) const;
    inline Vector3 operator-( const Vector3& ) const;
    inline Vector3 operator*( const FLOAT& ) const;
    inline FLOAT   operator*( const Vector3& ) const; // dot product
    inline Vector3 operator^( const Vector3& ) const; // cross product

    inline friend Vector3 operator*( const FLOAT&, const Vector3& );

    inline FLOAT GetLength() const;
    inline VOID Normalize();
    inline VOID Zero();
};




//-----------------------------------------------------------------------------
// Name: class Quaternion
// Desc: Simple wrapper around D3DXQUATERNION
//-----------------------------------------------------------------------------
class Quaternion : public D3DXQUATERNION
{

public:

    inline Quaternion(); // Identity rotation
    inline Quaternion( FLOAT x, FLOAT y, FLOAT z, FLOAT w );
    inline Quaternion( const D3DXQUATERNION& );
           Quaternion( const Vector3& v3Axis, FLOAT fAngle );

    inline Quaternion operator~() const;
    inline Quaternion operator*( const FLOAT& ) const;
    inline Quaternion operator*( const Quaternion& ) const;

    inline friend Vector3 operator*(const Vector3& v, const Quaternion& q);

    VOID Normalize();
};




//-----------------------------------------------------------------------------
// Name: class Matrix3
// Desc: Simple 3x3 matrix class
//-----------------------------------------------------------------------------
class Matrix3
{

public:

    FLOAT m[3][3];

public:

    inline Matrix3();  // Identity matrix
    inline Matrix3( FLOAT m00, FLOAT m01, FLOAT m02, 
                    FLOAT m10, FLOAT m11, FLOAT m12,
                    FLOAT m20, FLOAT m21, FLOAT m22);
           Matrix3( const Quaternion& );

           Matrix3 operator~() const;
           Matrix3 operator*( const Matrix3& ) const;

    inline friend Vector3 operator* (const Vector3& v, const Matrix3& m);

    inline Matrix3 GetTranspose() const;
    inline VOID Zero();

private:

    FLOAT Det2( INT r1, INT r2, INT c1, INT c2 ) const;
    FLOAT RowDotCol( INT row, const Matrix3& m3, INT col ) const;

};




//-----------------------------------------------------------------------------
// Name: class Plane3
// Desc: Simple wrapper around D3DXPLANE
//-----------------------------------------------------------------------------
class Plane3 : public D3DXPLANE
{

public:

    inline Plane3();
    inline Plane3( FLOAT a, FLOAT b, FLOAT c, FLOAT d );
    inline Plane3( const Vector3& v3Normal, const Vector3& v3Point );
    inline Plane3( const D3DXPLANE& );
           Plane3( const Vector3& v1, const Vector3& v2, const Vector3& v3 );

    inline Vector3 GetNormal() const;
    inline FLOAT   GetDistance( const Vector3& ) const;

};




//-----------------------------------------------------------------------------
// Name: class Transformation
// Desc: Simple transform class
//-----------------------------------------------------------------------------
class Transformation
{

    Quaternion m_qRotation;       // Quaterion for fast inversion/composition
    Vector3    m_v3Translation;   // Translation
    Matrix3    m_m3Rotation;      // Matrix for fast vector rotation

public:

    Transformation();
    Transformation( const Quaternion&, const Vector3& );

    Transformation operator~() const;
    Transformation operator*( const Transformation& ) const;

    friend Vector3 operator*( const Vector3&, const Transformation& );
    friend Plane3  operator*( const Plane3&, const Transformation& );

};




//-----------------------------------------------------------------------------
// Include inline functions
//-----------------------------------------------------------------------------
#include "Math3d.inl"




#endif // TECH_CERT_GAME_MATH_H
