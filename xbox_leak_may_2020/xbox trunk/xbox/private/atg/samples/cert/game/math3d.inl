//-----------------------------------------------------------------------------
// File: Math3d.inl
//
// Desc: Inline math functions
//
// Hist: 03.14.01 - New for May XDK release
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------




//-------------------------------------------------------------------------
// Name: Vector3()
// Desc: Empty vector. No initialization for speed. Use Zero() to clear 
//-------------------------------------------------------------------------
inline Vector3::Vector3()
:
    D3DXVECTOR3()
{
}




//-------------------------------------------------------------------------
// Name: Vector3()
// Desc: Construct given params
//-------------------------------------------------------------------------
inline Vector3::Vector3( FLOAT x, FLOAT y, FLOAT z )
:
    D3DXVECTOR3( x, y, z )
{
}




//-------------------------------------------------------------------------
// Name: Vector3()
// Desc: Construct from D3DXVECTOR3
//-------------------------------------------------------------------------
inline Vector3::Vector3( const D3DXVECTOR3& v )
:
    D3DXVECTOR3( v )
{
}




//-------------------------------------------------------------------------
// Name: operator=()
// Desc: Copy from D3DXVECTOR3
//-------------------------------------------------------------------------
inline Vector3& Vector3::operator=( const D3DXVECTOR3& v )
{
    x = v.x;
    y = v.y;
    z = v.z;
}




//-------------------------------------------------------------------------
// Name: operator-()
// Desc: Negate
//-------------------------------------------------------------------------
inline Vector3 Vector3::operator-() const
{
    return Vector3( -x, -y, -z );
}




//-------------------------------------------------------------------------
// Name: operator+()
// Desc: Add
//-------------------------------------------------------------------------
inline Vector3 Vector3::operator+( const Vector3& rhs ) const
{
    return Vector3( x + rhs.x, y + rhs.y, z + rhs.z );
}




//-------------------------------------------------------------------------
// Name: operator-()
// Desc: Subtract
//-------------------------------------------------------------------------
inline Vector3 Vector3::operator-( const Vector3& rhs ) const
{
    return Vector3( x - rhs.x, y - rhs.y, z - rhs.z );
}




//-------------------------------------------------------------------------
// Name: operator*()
// Desc: Multiply by float
//-------------------------------------------------------------------------
inline Vector3 Vector3::operator*( const FLOAT& f ) const
{
    return Vector3( x*f, y*f, z*f );
}




//-------------------------------------------------------------------------
// Name: operator*()
// Desc: Dot product
//-------------------------------------------------------------------------
inline FLOAT Vector3::operator*( const Vector3& rhs ) const
{
    return x*rhs.x + y*rhs.y + z*rhs.z;
}




//-------------------------------------------------------------------------
// Name: operator^()
// Desc: Cross product
//-------------------------------------------------------------------------
inline Vector3 Vector3::operator^( const Vector3& rhs ) const
{
    return Vector3( y*rhs.z - z*rhs.y, z*rhs.x - x*rhs.z, x*rhs.y - y*rhs.x );
}




//-------------------------------------------------------------------------
// Name: operator*()
// Desc: Multiply float by vector
//-------------------------------------------------------------------------
inline Vector3 operator*( const FLOAT& f, const Vector3& v )
{
    return Vector3( v.x*f, v.y*f, v.z*f );
}




//-------------------------------------------------------------------------
// Name: GetLength()
// Desc: Vector length
//-------------------------------------------------------------------------
inline FLOAT Vector3::GetLength() const
{
    return FLOAT( sqrt( Sqr(x) + Sqr(y) + Sqr(z) ) );
}




//-------------------------------------------------------------------------
// Name: Normalize()
// Desc: Normalize vector
//-------------------------------------------------------------------------
inline VOID Vector3::Normalize()
{
    FLOAT len = GetLength();

    if( len >= FLT_MIN )
    {
        FLOAT r = 1.0f / len;
        x *= r; 
        y *= r;
        z *= r; 
    }
}




//-------------------------------------------------------------------------
// Name: Zero()
// Desc: Clear vector
//-------------------------------------------------------------------------
inline VOID Vector3::Zero()
{
    x = 0.0f;
    y = 0.0f;
    z = 0.0f;
}




//-------------------------------------------------------------------------
// Name: Quaternion()
// Desc: Construct identity rotation
//-------------------------------------------------------------------------
inline Quaternion::Quaternion()
:
    D3DXQUATERNION( 0.0f, 0.0f, 0.0f, 1.0f )
{
}




//-------------------------------------------------------------------------
// Name: Quaternion()
// Desc: Construct from given values
//-------------------------------------------------------------------------
inline Quaternion::Quaternion( FLOAT x, FLOAT y, FLOAT z, FLOAT w )
:
    D3DXQUATERNION( x, y, z, w )
{
}




//-------------------------------------------------------------------------
// Name: Quaternion()
// Desc: Construct from D3DXQUATERNION
//-------------------------------------------------------------------------
inline Quaternion::Quaternion( const D3DXQUATERNION& rhs )
:
    D3DXQUATERNION( rhs.x, rhs.y, rhs.z, rhs.w )
{
}




//-------------------------------------------------------------------------
// Name: operator~()
// Desc: Inverse
//-------------------------------------------------------------------------
inline Quaternion Quaternion::operator~() const
{
    return Quaternion( -x, -y, -z, w );
}




//-------------------------------------------------------------------------
// Name: operator*()
// Desc: Multiply by float
//-------------------------------------------------------------------------
inline Quaternion Quaternion::operator*( const FLOAT& f ) const
{
    return Quaternion( x*f, y*f, z*f, w*f );
}




//-------------------------------------------------------------------------
// Name: operator*()
// Desc: Multiply
//-------------------------------------------------------------------------
inline Quaternion Quaternion::operator*( const Quaternion& rhs ) const
{
    return Quaternion
    (
        w * rhs.x + x * rhs.w + y * rhs.z - z * rhs.y,
        w * rhs.y + y * rhs.w + z * rhs.x - x * rhs.z,
        w * rhs.z + z * rhs.w + x * rhs.y - y * rhs.x,
        w * rhs.w - x * rhs.x - y * rhs.y - z * rhs.z
    );  
}




//-------------------------------------------------------------------------
// Name: operator*()
// Desc: Transform vector by quaternion
//-------------------------------------------------------------------------
inline Vector3 operator*( const Vector3& v, const Quaternion& q )
{
    Vector3 qV( q.x, q.y, q.z );
    return 2.0f * (q.w * (qV ^ v) + (v * qV) * qV) + (q.w * q.w - (qV * qV)) * v;
}




//-------------------------------------------------------------------------
// Name: Matrix3()
// Desc: Identity matrix
//-------------------------------------------------------------------------
inline Matrix3::Matrix3()
{
    m[0][0] = 1.0f;
    m[0][1] = 0.0f;
    m[0][2] = 0.0f;

    m[1][0] = 0.0f;
    m[1][1] = 1.0f;
    m[1][2] = 0.0f;

    m[2][0] = 0.0f;
    m[2][1] = 0.0f;
    m[2][2] = 1.0f;
}




//-------------------------------------------------------------------------
// Name: Matrix3()
// Desc: Construct given values
//-------------------------------------------------------------------------
inline Matrix3::Matrix3( FLOAT m00, FLOAT m01, FLOAT m02, 
                         FLOAT m10, FLOAT m11, FLOAT m12,
                         FLOAT m20, FLOAT m21, FLOAT m22 )
{
    m[0][0] = m00;
    m[0][1] = m01;
    m[0][2] = m02;

    m[1][0] = m10;
    m[1][1] = m11;
    m[1][2] = m12;

    m[2][0] = m20;
    m[2][1] = m21;
    m[2][2] = m22;
}




//-------------------------------------------------------------------------
// Name: operator*()
// Desc: Transform vector by matrix
//-------------------------------------------------------------------------
inline Vector3 operator*( const Vector3& v, const Matrix3& m )
{
    return Vector3
    (
        v.x * m.m[0][0] + v.y * m.m[1][0] + v.z * m.m[2][0],
        v.x * m.m[0][1] + v.y * m.m[1][1] + v.z * m.m[2][1],
        v.x * m.m[0][2] + v.y * m.m[1][2] + v.z * m.m[2][2]
    );
}




//-------------------------------------------------------------------------
// Name: GetTranspose()
// Desc: Transposed matrix
//-------------------------------------------------------------------------
inline Matrix3 Matrix3::GetTranspose() const
{
    return Matrix3( m[0][0], m[1][0], m[2][0],
                    m[0][1], m[1][1], m[2][1],
                    m[0][2], m[1][2], m[2][2] );
}




//-------------------------------------------------------------------------
// Name: Zero()
// Desc: Clears matrix
//-------------------------------------------------------------------------
inline VOID Matrix3::Zero()
{
    m[0][0] = 0.0f;
    m[0][1] = 0.0f;
    m[0][2] = 0.0f;

    m[1][0] = 0.0f;
    m[1][1] = 0.0f;
    m[1][2] = 0.0f;

    m[2][0] = 0.0f;
    m[2][1] = 0.0f;
    m[2][2] = 0.0f;
}




//-------------------------------------------------------------------------
// Name: Det2()
// Desc: Determinate of the 2x2 sub-matrix consisting of rows r1, r2 
//       and columns c1 and c2.
//-------------------------------------------------------------------------
inline FLOAT Matrix3::Det2( INT r1, INT r2, INT c1, INT c2) const // private
{
    return m[r1][c1] * m[r2][c2] - m[r1][c2] * m[r2][c1];
}




//-------------------------------------------------------------------------
// Name: RowDotCol()
// Desc: Return a row of this matrix dotted with a column of matrix m.
//-------------------------------------------------------------------------
inline FLOAT Matrix3::RowDotCol( INT row, const Matrix3& m3, INT col ) const // private
{
    return m[row][0] * m3.m[0][col] 
         + m[row][1] * m3.m[1][col] 
         + m[row][2] * m3.m[2][col];
}





//-------------------------------------------------------------------------
// Name: Plane3()
// Desc: Construct empty plane
//-------------------------------------------------------------------------
inline Plane3::Plane3()
:
    D3DXPLANE()
{
}




//-------------------------------------------------------------------------
// Name: Plane3()
// Desc: Construct plane from plane params
//-------------------------------------------------------------------------
inline Plane3::Plane3( FLOAT a, FLOAT b, FLOAT c, FLOAT d )
:
    D3DXPLANE( a, b, c, d )
{
}




//-------------------------------------------------------------------------
// Name: Plane3()
// Desc: Construct plane given a normal and a point
//-------------------------------------------------------------------------
inline Plane3::Plane3( const Vector3& v3Normal, const Vector3& v3Point )
:
    D3DXPLANE( v3Normal.x, v3Normal.y, v3Normal.z, -(v3Normal * v3Point) )
{
}




//-------------------------------------------------------------------------
// Name: Plane3()
// Desc: Construct from D3DXPLANE
//-------------------------------------------------------------------------
inline Plane3::Plane3( const D3DXPLANE& rhs )
:
    D3DXPLANE( rhs.a, rhs.b, rhs.c, rhs.d )
{
}




//-------------------------------------------------------------------------
// Name: GetNormal()
// Desc: Vector normal to the plane
//-------------------------------------------------------------------------
inline Vector3 Plane3::GetNormal() const
{
    return Vector3( a, b, c );
}




//-------------------------------------------------------------------------
// Name: GetDistance()
// Desc: Distance from plane to point
//-------------------------------------------------------------------------
inline FLOAT Plane3::GetDistance( const Vector3& v ) const
{
    return v.x * a + v.y * b + v.z * c + d;
}





//-------------------------------------------------------------------------
// Name: Transformation()
// Desc: Construct empty transformation
//-------------------------------------------------------------------------
inline Transformation::Transformation()
:
    m_qRotation(),
    m_v3Translation( 0.0f, 0.0f, 0.0f ),
    m_m3Rotation() // identity
{
}




//-------------------------------------------------------------------------
// Name: Transformation()
// Desc: Construct from quat and vector
//-------------------------------------------------------------------------
inline Transformation::Transformation( const Quaternion& q, const Vector3& v )
:
    m_qRotation( q ),
    m_v3Translation( v ),
    m_m3Rotation( q )
{
}




//-------------------------------------------------------------------------
// Name: operator~()
// Desc: Transformation inverse
//-------------------------------------------------------------------------
inline Transformation Transformation::operator~() const
{
    Quaternion qInverse = ~m_qRotation;
    return Transformation( qInverse, -m_v3Translation * qInverse );
}




//-------------------------------------------------------------------------
// Name: operator*()
// Desc: Transformation composition
//-------------------------------------------------------------------------
inline Transformation Transformation::operator*
                                        ( const Transformation& rhs ) const
{
    return Transformation( m_qRotation * rhs.m_qRotation, 
                           m_v3Translation * rhs.m_m3Rotation + 
                           rhs.m_v3Translation );
}




//-------------------------------------------------------------------------
// Name: operator*()
// Desc: Transform a vector by Transformation
//-------------------------------------------------------------------------
inline Vector3 operator*( const Vector3& v, const Transformation& tf )
{
    return v * tf.m_m3Rotation + tf.m_v3Translation;
}




//-------------------------------------------------------------------------
// Name: operator*()
// Desc: Transform a plane by Transformation
//-------------------------------------------------------------------------
inline Plane3 operator*( const Plane3& pl, const Transformation& tf )
{
    Vector3 vNormal = pl.GetNormal() * tf.m_m3Rotation;
    FLOAT d = pl.d - ( vNormal * tf.m_v3Translation );
    return Plane3( vNormal.x, vNormal.y, vNormal.z, d );
}
