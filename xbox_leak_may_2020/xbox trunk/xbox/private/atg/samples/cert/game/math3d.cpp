//-----------------------------------------------------------------------------
// File: Math3d.cpp
//
// Desc: Math functions that are too large or infrequently used to by inline
//
// Hist: 04.10.01 - New for May XDK release 
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#include "Math3d.h"




//-------------------------------------------------------------------------
// Name: Quaternion()
// Desc: Construct from an axis and an angle (in radians)
//-------------------------------------------------------------------------
Quaternion::Quaternion( const Vector3& v3Axis, FLOAT fAngle )
{
    D3DXVECTOR3 v3AxisT( v3Axis );
    D3DXQuaternionRotationAxis( this, &v3AxisT, fAngle );
}




//-------------------------------------------------------------------------
// Name: Normalize()
// Desc: Normalize quat
//-------------------------------------------------------------------------
VOID Quaternion::Normalize()
{
    FLOAT len = FLOAT(sqrt( x*x + y*y + z*z + w*w ));

    if (len >= FLT_MIN) 
    {
        FLOAT r = 1.0f / len;
        x *= r; 
        y *= r;
        z *= r; 
        w *= r; 
    }
    else 
    {
        x = y = z = 0.0f;
        w = 1.0f;
    }
}




//-------------------------------------------------------------------------
// Name: Matrix3()
// Desc: Construct from quat
//-------------------------------------------------------------------------
Matrix3::Matrix3( const Quaternion& q )
{
    FLOAT x2 = q.x + q.x;
    FLOAT y2 = q.y + q.y;
    FLOAT z2 = q.z + q.z;

    FLOAT xx = q.x * x2;
    FLOAT xy = q.x * y2;
    FLOAT xz = q.x * z2;

    FLOAT yy = q.y * y2;
    FLOAT yz = q.y * z2;
    FLOAT zz = q.z * z2;

    FLOAT wx = q.w * x2;
    FLOAT wy = q.w * y2;
    FLOAT wz = q.w * z2;

    m[0][0] = 1.0f - ( yy + zz );
    m[0][1] = xy + wz;
    m[0][2] = xz - wy;

    m[1][0] = xy - wz;
    m[1][1] = 1.0f - ( xx + zz );
    m[1][2] = yz + wx;

    m[2][0] = xz + wy;
    m[2][1] = yz - wx;
    m[2][2] = 1.0f - ( xx + yy );
}




//-------------------------------------------------------------------------
// Name: operator~()
// Desc: Matrix inverse
//-------------------------------------------------------------------------
Matrix3 Matrix3::operator~() const
{
    // Determine the co-factors of the matrix.
    Matrix3 m3Cof
    (
        // Determinate of 2x2 sub-matrix formed by deleting Row 1, Column 1
        Det2(1, 2, 1, 2),

        // Determinate of 2x2 sub-matrix formed by deleting Row 1, Column 2
        -Det2(1, 2, 0, 2),

        // Determinate of 2x2 sub-matrix formed by deleting Row 1, Column 3
        Det2(1, 2, 0, 1),

        // Determinate of 2x2 sub-matrix formed by deleting Row 2, Column 1
        -Det2(0, 2, 1, 2),

        // Determinate of 2x2 sub-matrix formed by deleting Row 2, Column 2
        Det2(0, 2, 0, 2),

        // Determinate of 2x2 sub-matrix formed by deleting Row 2, Column 3
        -Det2(0, 2, 0, 1),

        // Determinate of 2x2 sub-matrix formed by deleting Row 3, Column 1
        Det2(0, 1, 1, 2),

        // Determinate of 2x2 sub-matrix formed by deleting Row 3, Column 2
        -Det2(0, 1, 0, 2),

        // Determinate of 2x2 sub-matrix formed by deleting Row 3, Column 3
        Det2(0, 1, 0, 1)
    );

    // Compute the determinate
    FLOAT fDet = m[0][0] * m3Cof.m[0][0] + 
                 m[0][1] * m3Cof.m[0][1] + 
                 m[0][2] * m3Cof.m[0][2];
    
    // Check for singular matrix
    assert( fDet != 0.0f );

    FLOAT fInvDet = 1.0f / fDet;

    // Inverse is the inverse of the determinate times the transpose of the co-factor matrix.
    return Matrix3
    (
        fInvDet * m3Cof.m[0][0], fInvDet * m3Cof.m[1][0], fInvDet * m3Cof.m[2][0],
        fInvDet * m3Cof.m[0][1], fInvDet * m3Cof.m[1][1], fInvDet * m3Cof.m[2][1],
        fInvDet * m3Cof.m[0][2], fInvDet * m3Cof.m[1][2], fInvDet * m3Cof.m[2][2]
    );
}




//-------------------------------------------------------------------------
// Name: operator*()
// Desc: Matrix multiply
//-------------------------------------------------------------------------
Matrix3 Matrix3::operator*( const Matrix3& rhs ) const
{
    return Matrix3
    (
        RowDotCol(0, rhs, 0), RowDotCol(0, rhs, 1), RowDotCol(0, rhs, 2),
        RowDotCol(1, rhs, 0), RowDotCol(1, rhs, 1), RowDotCol(1, rhs, 2),
        RowDotCol(2, rhs, 0), RowDotCol(2, rhs, 1), RowDotCol(2, rhs, 2)
    );
}




//-------------------------------------------------------------------------
// Name: Plane3()
// Desc: Construct from vectors
//-------------------------------------------------------------------------
Plane3::Plane3( const Vector3& v1, const Vector3& v2, const Vector3& v3 )
:
    D3DXPLANE()
{
    Vector3 vNorm = (v1 - v2) ^ (v3- v2);

    vNorm.Normalize();

    a = vNorm.x;
    b = vNorm.y;
    c = vNorm.z;
    d = -(vNorm * v1);
}
