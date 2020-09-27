//
//	xbs_math.h
//
///////////////////////////////////////////////////////////////////////////////
//  Copyright (C) 2001, Pipeworks Software Inc.
//				All rights reserved
#ifndef __XBS_MATH_H__
#define __XBS_MATH_H__

struct D3DVECTOR4 : public _D3DVECTOR
{
	float w;
};

const float Pi = 3.1415926535897932384626433832795028841971693993751058209f;
const float DegreeToRad = 0.017453292519943295769236907684883f; // (Pi / 180)
const float RadToDegree = 57.295779513082320876798154814114f; // (180 / Pi)

inline void SetLookAt(const D3DVECTOR &cam, const D3DVECTOR &look, const D3DVECTOR &up, D3DMATRIX *pres);
inline void SetProjection(float fov,float aspect, float near_plane, float far_plane, D3DMATRIX *pres);
inline void SetInverse(const D3DMATRIX &mat, D3DMATRIX *pres);
inline void SetTranspose(const D3DMATRIX &mat, D3DMATRIX *pres);
inline void SetIdentity(D3DMATRIX *pres);
inline void SetXRotation(float r,D3DMATRIX *pres);
inline void SetYRotation(float r,D3DMATRIX *pres);
inline void SetZRotation(float r,D3DMATRIX *pres);
inline void SetXYZRotation(float x,float y,float z, D3DMATRIX *pres);
inline void SetRotationFromLHQuat(const D3DVECTOR4 &quat,D3DMATRIX *pres); 
inline void SetRotationFromRHQuat(const D3DVECTOR4 &quat,D3DMATRIX *pres); 
inline void SetScale(float x,float y,float z,D3DMATRIX *pres);
inline void SetCubeMapView(DWORD dwFace, const D3DVECTOR &pos,D3DMATRIX *pres);

inline void TransformPoint(const D3DVECTOR &pt,const D3DMATRIX &mat, D3DVECTOR *pres);
inline void TransformVector(const D3DVECTOR &v,const D3DMATRIX &mat, D3DVECTOR *pres);
inline void TransformPoint(const D3DVECTOR4 &pt,const D3DMATRIX &mat, D3DVECTOR4 *pres);


inline void MulMats(const D3DMATRIX &a, const D3DMATRIX &b,D3DMATRIX *pres);
inline void ConcatMats(const D3DMATRIX &a, const D3DMATRIX &b, D3DMATRIX *pres);

inline float Dot(const D3DVECTOR &a, const D3DVECTOR &b);
inline float Length2(const D3DVECTOR &v);
inline float Length(const D3DVECTOR &v);
inline float Distance2(const D3DVECTOR &lhs, const D3DVECTOR &rhs);
inline float Distance(const D3DVECTOR &lhs, const D3DVECTOR &rhs);
inline void  Normalize(D3DVECTOR *pres);
inline void  Cross(const D3DVECTOR &a, const D3DVECTOR &b, D3DVECTOR *pres);
inline void  Scale(D3DVECTOR *pres,float s);
inline void  Set(D3DVECTOR *pres,float x,float y,float z);
inline void  Set(D3DVECTOR4 *pres,float x,float y,float z,float w);
inline void  Sub(const D3DVECTOR &lhs, const D3DVECTOR &rhs, D3DVECTOR *pres);
inline void  Add(const D3DVECTOR &a, const D3DVECTOR &b, D3DVECTOR *pres);
inline void  SetQuatFromAxis(const D3DVECTOR &axis, float angle,D3DVECTOR4 *pres);

inline float DotQuats(const D3DVECTOR4 &q0,const D3DVECTOR4 &q1);
inline void  SlerpQuats(const D3DVECTOR4 &q0,const D3DVECTOR4 &q1,float t,D3DVECTOR4 *pres);
inline void  NormalizeQuat(D3DVECTOR4 *pres);

inline bool  IsEq(float a,float b);
inline bool  PtsEq(const D3DVECTOR &a,const D3DVECTOR &b);

inline void  AddScaled(D3DVECTOR* ptarget, const D3DVECTOR& src, float scale);
inline float QuickLength(const D3DVECTOR& vec);
inline void  QuickNormalize(D3DVECTOR* p_vec);	// left unchanged if the length is already small


inline void SinCos(const float &a, float *ps, float *pc);
inline float AngleFromSinCos(const float &s, const float &c);


inline void bs_swap(float& a, float& b);
inline void bs_swap(int& a, int& b);

inline float Square(float a) { return a*a; }


#include "xbs_math_inl.h"


#endif // __XBS_MATH_H__