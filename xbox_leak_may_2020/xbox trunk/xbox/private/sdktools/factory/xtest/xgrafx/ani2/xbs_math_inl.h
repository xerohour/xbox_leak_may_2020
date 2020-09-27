//
//	xbs_math_inl.h
//
///////////////////////////////////////////////////////////////////////////////
//  Copyright (C) 2001, Pipeworks Software Inc.
//				All rights reserved
#ifndef __XBS_MATH_INL_H__
#define __XBS_MATH_INL_H__

#include "fastmath.h"

///////////////////////////////////////////////////////////////////////////////
inline void SetLookAt(const D3DVECTOR &cam, const D3DVECTOR &look, const D3DVECTOR &up, D3DMATRIX *pres)
{	
	D3DVECTOR zAxis = look;
	zAxis.x -= cam.x;
	zAxis.y -= cam.y;
	zAxis.z -= cam.z;
	
	Normalize(&zAxis);

	D3DVECTOR xAxis;
	Cross(zAxis,up,&xAxis);

	Normalize(&xAxis);

	D3DVECTOR yAxis;
	Cross(xAxis,zAxis,&yAxis);

	pres->_11 = xAxis.x;
	pres->_12 = yAxis.x;
	pres->_13 = zAxis.x;
	pres->_14 = 0.f;
	pres->_21 = xAxis.y;
	pres->_22 = yAxis.y;
	pres->_23 = zAxis.y;
	pres->_24 = 0.f;
	pres->_31 = xAxis.z;
	pres->_32 = yAxis.z;
	pres->_33 = zAxis.z;
	pres->_34 = 0.f;
	pres->_41 = pres->_42 = pres->_43 = 0.f;
	pres->_44 = 1.f;

	D3DVECTOR inv_cam;
	inv_cam.x = -cam.x;
	inv_cam.y = -cam.y;
	inv_cam.z = -cam.z;

	D3DVECTOR r_inv_cam;
	TransformVector(inv_cam,*pres,&r_inv_cam);

	pres->_41 = r_inv_cam.x;
	pres->_42 = r_inv_cam.y;
	pres->_43 = r_inv_cam.z;
}
///////////////////////////////////////////////////////////////////////////////
void SetProjection(float fov,float aspect,float near_plane,float far_plane,D3DMATRIX *pres)
{
	float fov2 = fov * .5f;
	float c,s,ct;
	SinCos(fov2, &s, &c);
	ct = c / s;

    float w = aspect * ct;
    float h = ct;
    float Q = far_plane / (far_plane - near_plane);

	pres->_11 = w;
	pres->_12 = 0.f;
	pres->_13 = 0.f;
	pres->_14 = 0.f;
	pres->_21 = 0.f;
	pres->_22 = h;
	pres->_23 = 0.f;
	pres->_24 = 0.f;
	pres->_31 = 0.f;
	pres->_32 = 0.f;
	pres->_33 = Q;
	pres->_34 = 1.f;
	pres->_41 = 0.f;
	pres->_42 = 0.f;
	pres->_43 = -Q * near_plane;
	pres->_44 = 0.f;
}
///////////////////////////////////////////////////////////////////////////////
inline void SetInverse(const D3DMATRIX &mat, D3DMATRIX *pres)
{
	pres->_11 = mat._11;
	pres->_12 = mat._21;
	pres->_13 = mat._31;
	pres->_14 = 0.f;
	
	pres->_21 = mat._12;
	pres->_22 = mat._22;
	pres->_23 = mat._32;
	pres->_24 = 0.f;

	pres->_31 = mat._13;
	pres->_32 = mat._23;
	pres->_33 = mat._33;
	pres->_34 = 0.f;

	D3DVECTOR inv_trans;
	inv_trans.x = -mat._41;
	inv_trans.y = -mat._42;
	inv_trans.z = -mat._43;

	D3DVECTOR r_inv_trans;
	TransformVector(inv_trans,*pres,&r_inv_trans);

	pres->_41 = r_inv_trans.x;
	pres->_42 = r_inv_trans.y;
	pres->_43 = r_inv_trans.z;
	pres->_44 = 1.f;
}
///////////////////////////////////////////////////////////////////////////////
inline void SetTranspose(const D3DMATRIX &mat, D3DMATRIX *pres)
{
	pres->_11 = mat._11;
	pres->_12 = mat._21;
	pres->_13 = mat._31;
	pres->_14 = mat._41;

	pres->_21 = mat._12;
	pres->_22 = mat._22;
	pres->_23 = mat._32;
	pres->_24 = mat._42;

	pres->_31 = mat._13;
	pres->_32 = mat._23;
	pres->_33 = mat._33;
	pres->_34 = mat._43;

	pres->_41 = mat._14;
	pres->_42 = mat._24;
	pres->_43 = mat._34;
	pres->_44 = mat._44;
}
///////////////////////////////////////////////////////////////////////////////
inline void SetIdentity(D3DMATRIX *pres)
{
	pres->_11 = pres->_22 = pres->_33 = pres->_44 = 1.f;
	pres->_12 = pres->_13 = pres->_14 = 
	pres->_21 = pres->_23 = pres->_24 = 
	pres->_31 = pres->_32 = pres->_34 = 
	pres->_41 = pres->_42 = pres->_43 = 0.f;
}
///////////////////////////////////////////////////////////////////////////////
inline void SetXRotation(float r,D3DMATRIX *pres)
{
	float s,c;
	SinCos(r,&s,&c);

	pres->_11 = 1.f;
	pres->_12 = 0.f;
	pres->_13 = 0.f;
	pres->_14 = 0.f;
	pres->_21 = 0.f;
	pres->_22 = c;
	pres->_23 = s;
	pres->_24 = 0.f;
	pres->_31 = 0.f;
	pres->_32 = -s;
	pres->_33 = c;
	pres->_34 = 0.f;
	pres->_41 = pres->_42 = pres->_43 = 0.f;
	pres->_44 = 1.f; 
}
///////////////////////////////////////////////////////////////////////////////
inline void SetYRotation(float r,D3DMATRIX *pres)
{
	float s,c;
	SinCos(r,&s,&c);

	pres->_11 = c;
	pres->_12 = 0.f;
	pres->_13 = s;
	pres->_14 = 0.f;
	pres->_21 = 0.f;
	pres->_22 = 1.f;
	pres->_23 = 0.f;
	pres->_24 = 0.f;
	pres->_31 = -s;
	pres->_32 = 0.f;
	pres->_33 = c;
	pres->_34 = 0.f;
	pres->_41 = pres->_42 = pres->_43 = 0.f;
	pres->_44 = 1.f; 
}
///////////////////////////////////////////////////////////////////////////////
inline void SetZRotation(float r,D3DMATRIX *pres)
{
	float s,c;
	SinCos(r,&s,&c);

	pres->_11 = c;
	pres->_12 = s;
	pres->_13 = 0.f;
	pres->_14 = 0.f;
	pres->_21 = -s;
	pres->_22 = c;
	pres->_23 = 0;
	pres->_24 = 0.f;
	pres->_31 = 0.f;
	pres->_32 = 0.f;
	pres->_33 = 1;
	pres->_34 = 0.f;
	pres->_41 = pres->_42 = pres->_43 = 0.f;
	pres->_44 = 1.f; 
}
///////////////////////////////////////////////////////////////////////////////
inline void SetXYZRotation(float x,float y,float z, D3DMATRIX *pres)
{
	D3DMATRIX matx;
	SetXRotation(x,&matx);

	D3DMATRIX maty;
	SetYRotation(y,&maty);

	D3DMATRIX matxy;
	MulMats(matx,maty,&matxy);

	D3DMATRIX matz;
	SetZRotation(z,&matz);

	MulMats(matz,matxy,pres);
}
///////////////////////////////////////////////////////////////////////////////
inline void SetRotationFromRHQuat(const D3DVECTOR4 &q,D3DMATRIX *pres)
{
	pres->_11 = q.w*q.w + q.x*q.x - q.y*q.y - q.z*q.z;
	pres->_12 = 2.f*q.x*q.y + 2.f*q.w*q.z;
	pres->_13 = 2.f*q.x*q.z - 2.f*q.w*q.y;
	pres->_14 = 0.f;

	pres->_21 = 2.f*q.x*q.y - 2.f*q.w*q.z;
	pres->_22 = q.w*q.w - q.x*q.x + q.y*q.y - q.z*q.z;
	pres->_23 = 2.f*q.y*q.z + 2.f*q.w*q.x;
	pres->_24 = 0.f;

	pres->_31 = 2.f*q.x*q.z + 2.f*q.w*q.y;
	pres->_32 = 2.f*q.y*q.z - 2.f*q.w*q.x;
	pres->_33 = q.w*q.w - q.x*q.x - q.y*q.y + q.z*q.z;
	pres->_34 = 0.f;
	
	pres->_41 = pres->_42 = pres->_43 = 0.f;
	pres->_44 = 1.f;
}
///////////////////////////////////////////////////////////////////////////////
inline void SetRotationFromLHQuat(const D3DVECTOR4 &q,D3DMATRIX *pres)
{
	pres->_11 = q.w*q.w + q.x*q.x - q.y*q.y - q.z*q.z;
	pres->_21 = 2.f*q.x*q.y + 2.f*q.w*q.z;
	pres->_31 = 2.f*q.x*q.z - 2.f*q.w*q.y;
	pres->_41 = 0.f;

	pres->_12 = 2.f*q.x*q.y - 2.f*q.w*q.z;
	pres->_22 = q.w*q.w - q.x*q.x + q.y*q.y - q.z*q.z;
	pres->_32 = 2.f*q.y*q.z + 2.f*q.w*q.x;
	pres->_42 = 0.f;

	pres->_13 = 2.f*q.x*q.z + 2.f*q.w*q.y;
	pres->_23 = 2.f*q.y*q.z - 2.f*q.w*q.x;
	pres->_33 = q.w*q.w - q.x*q.x - q.y*q.y + q.z*q.z;
	pres->_43 = 0.f;
	
	pres->_14 = pres->_24 = pres->_34 = 0.f;
	pres->_44 = 1.f;
}
///////////////////////////////////////////////////////////////////////////////
inline void TransformPoint(const D3DVECTOR &pt,const D3DMATRIX &mat, D3DVECTOR *pres)
{
	pres->x = pt.x * mat._11 + pt.y * mat._21 + pt.z * mat._31 + mat._41;
	pres->y = pt.x * mat._12 + pt.y * mat._22 + pt.z * mat._32 + mat._42;
	pres->z = pt.x * mat._13 + pt.y * mat._23 + pt.z * mat._33 + mat._43; 
}
///////////////////////////////////////////////////////////////////////////////
inline void TransformVector(const D3DVECTOR &v,const D3DMATRIX &mat, D3DVECTOR *pres)
{
	pres->x = v.x * mat._11 + v.y * mat._21 + v.z * mat._31;
	pres->y = v.x * mat._12 + v.y * mat._22 + v.z * mat._32;
	pres->z = v.x * mat._13 + v.y * mat._23 + v.z * mat._33; 
}
///////////////////////////////////////////////////////////////////////////////
inline void TransformPoint(const D3DVECTOR4 &pt,const D3DMATRIX &mat, D3DVECTOR4 *pres)
{
	pres->x = pt.x * mat._11 + pt.y * mat._21 + pt.z * mat._31 + pt.w * mat._41;
	pres->y = pt.x * mat._12 + pt.y * mat._22 + pt.z * mat._32 + pt.w * mat._42;
	pres->z = pt.x * mat._13 + pt.y * mat._23 + pt.z * mat._33 + pt.w * mat._43; 
	pres->w = pt.x * mat._14 + pt.y * mat._24 + pt.z * mat._34 + pt.w * mat._44; 
}
///////////////////////////////////////////////////////////////////////////////
inline void MulMats(const D3DMATRIX &a, const D3DMATRIX &b,D3DMATRIX *pres)
{
	pres->_11 = a._11*b._11 + a._12*b._21 + a._13*b._31 + a._14*b._41;
	pres->_12 = a._11*b._12 + a._12*b._22 + a._13*b._32 + a._14*b._42;
	pres->_13 = a._11*b._13 + a._12*b._23 + a._13*b._33 + a._14*b._43;
	pres->_14 = a._11*b._14 + a._12*b._24 + a._13*b._34 + a._14*b._44;

	pres->_21 = a._21*b._11 + a._22*b._21 + a._23*b._31 + a._24*b._41;
	pres->_22 = a._21*b._12 + a._22*b._22 + a._23*b._32 + a._24*b._42;
	pres->_23 = a._21*b._13 + a._22*b._23 + a._23*b._33 + a._24*b._43;
	pres->_24 = a._21*b._14 + a._22*b._24 + a._23*b._34 + a._24*b._44;

	pres->_31 = a._31*b._11 + a._32*b._21 + a._33*b._31 + a._34*b._41;
	pres->_32 = a._31*b._12 + a._32*b._22 + a._33*b._32 + a._34*b._42;
	pres->_33 = a._31*b._13 + a._32*b._23 + a._33*b._33 + a._34*b._43;
	pres->_34 = a._31*b._14 + a._32*b._24 + a._33*b._34 + a._34*b._44;

	pres->_41 = a._41*b._11 + a._42*b._21 + a._43*b._31 + a._44*b._41;
	pres->_42 = a._41*b._12 + a._42*b._22 + a._43*b._32 + a._44*b._42;
	pres->_43 = a._41*b._13 + a._42*b._23 + a._43*b._33 + a._44*b._43;
	pres->_44 = a._41*b._14 + a._42*b._24 + a._43*b._34 + a._44*b._44;
}
///////////////////////////////////////////////////////////////////////////////
inline void SetScale(float x,float y,float z,D3DMATRIX *pres)
{
	pres->_11 = x;
	pres->_22 = y;
	pres->_33 = z;
	
	pres->_12 = pres->_13 = pres->_14 = 
	pres->_21 = pres->_23 = pres->_24 = 
	pres->_31 = pres->_32 = pres->_34 = 
	pres->_41 = pres->_42 = pres->_43 = 0.f;

	pres->_44 = 1.f;
}

///////////////////////////////////////////////////////////////////////////////
inline void SetCubeMapView(DWORD dwFace, const D3DVECTOR &pos,D3DMATRIX *pres)
{
    D3DVECTOR look,up;
	
    switch( dwFace )
    {
        case D3DCUBEMAP_FACE_POSITIVE_X:
            Set(&look,1.f,0.f,0.f);
			Set(&up,0.f,1.f,0.f);
            break;

        case D3DCUBEMAP_FACE_NEGATIVE_X:
            Set(&look,-1.f,0.f,0.f);
			Set(&up,0.f,1.f,0.f);
            break;

        case D3DCUBEMAP_FACE_POSITIVE_Y:
			Set(&look,0.f,1.f,0.f);
			Set(&up,0.f,0.f,-1.f);
            break;

        case D3DCUBEMAP_FACE_NEGATIVE_Y:
            Set(&look,0.f,-1.f,0.f);
			Set(&up,0.f,0.f,1.f);
            break;

        case D3DCUBEMAP_FACE_POSITIVE_Z:
            Set(&look,0.f,0.f,1.f);
			Set(&up,0.f,1.f,0.f);
            break;

        case D3DCUBEMAP_FACE_NEGATIVE_Z:
            Set(&look,0.f,0.f,-1.f);
			Set(&up,0.f,1.f,0.f);
            break;
    }

	D3DVECTOR lookat;
	Add(pos,look,&lookat);

    SetLookAt( pos, lookat, up, pres );
}
///////////////////////////////////////////////////////////////////////////////
inline void ConcatMats(const D3DMATRIX &a, const D3DMATRIX &b, D3DMATRIX *pres)
{
	MulMats(a,b,pres);

	TransformVector(*((const D3DVECTOR *)(&a._41)),b,(D3DVECTOR *)&pres->_41);

	pres->_41 += b._41; pres->_42 += b._42; pres->_43 += b._43;
	
	pres->_14 = pres->_24 = pres->_34 = 0.f;
	pres->_44 = 1.f;
}
///////////////////////////////////////////////////////////////////////////////
inline float Dot(const D3DVECTOR &a, const D3DVECTOR &b)
{
	return a.x*b.x + a.y*b.y + a.z*b.z;
}
///////////////////////////////////////////////////////////////////////////////
inline float Length2(const D3DVECTOR &v)
{
	return v.x*v.x + v.y*v.y + v.z*v.z;
}
///////////////////////////////////////////////////////////////////////////////
inline float Length(const D3DVECTOR &v)
{
	return fast_sqrt(Length2(v));
}
///////////////////////////////////////////////////////////////////////////////
inline float Distance2(const D3DVECTOR &lhs, const D3DVECTOR &rhs)
{
	float dx = lhs.x - rhs.x;
	float dy = lhs.y - rhs.y;
	float dz = lhs.z - rhs.z;
	return dx*dx + dy*dy + dz*dz;
}
///////////////////////////////////////////////////////////////////////////////
inline float Distance(const D3DVECTOR &lhs, const D3DVECTOR &rhs)
{
	return fast_sqrt(Distance2(lhs, rhs));
}
///////////////////////////////////////////////////////////////////////////////
inline void Normalize(D3DVECTOR *pres)
{
	float fooLen = 1.f/Length(*pres);
	pres->x *= fooLen;
	pres->y *= fooLen;
	pres->z *= fooLen;
}
///////////////////////////////////////////////////////////////////////////////
inline void Cross(const D3DVECTOR &a, const D3DVECTOR &b, D3DVECTOR *pres)
{
	pres->x = a.y*b.z - a.z*b.y;
	pres->y = a.z*b.x - a.x*b.z;
	pres->z = a.x*b.y - a.y*b.x;
}
///////////////////////////////////////////////////////////////////////////////
inline void  Scale(D3DVECTOR *pres,float s)
{
	pres->x *= s; pres->y *= s; pres->z *= s;
}
///////////////////////////////////////////////////////////////////////////////
inline void  Set(D3DVECTOR *pres,float x,float y,float z)
{
	pres->x = x; pres->y = y; pres->z = z;
}
///////////////////////////////////////////////////////////////////////////////
inline void  Set(D3DVECTOR4 *pres,float x,float y,float z,float w)
{
	pres->x = x; pres->y = y; pres->z = z; pres->w = w;
}
///////////////////////////////////////////////////////////////////////////////
inline void  Sub(const D3DVECTOR &lhs, const D3DVECTOR &rhs, D3DVECTOR *pres)
{
	pres->x = lhs.x - rhs.x;
	pres->y = lhs.y - rhs.y;
	pres->z = lhs.z - rhs.z;
}
///////////////////////////////////////////////////////////////////////////////
inline void  SetQuatFromAxis(const D3DVECTOR &axis, float angle,D3DVECTOR4 *pres)
{
	float s,c;
	SinCos(angle*0.5f,&s,&c);
	
	pres->x = axis.x * s;
	pres->y = axis.y * s;
	pres->z = axis.z * s;
	pres->w = c;
}
///////////////////////////////////////////////////////////////////////////////
inline void  NormalizeQuat(D3DVECTOR4 *pres)
{
	float len2 = DotQuats(*pres,*pres);
	if(len2 != 0.f)
	{
		float oo_len = 1.f/fast_sqrt(len2);
		pres->x *= oo_len;
		pres->y *= oo_len;
		pres->z *= oo_len;
		pres->w *= oo_len;
	}
}
///////////////////////////////////////////////////////////////////////////////
inline float DotQuats(const D3DVECTOR4 &q0,const D3DVECTOR4 &q1)
{
	return q0.x * q1.x + q0.y * q1.y + q0.z * q1.z + q0.w * q1.w;
}
///////////////////////////////////////////////////////////////////////////////
inline void  SlerpQuats(const D3DVECTOR4 &q0,const D3DVECTOR4 &q1,float t,D3DVECTOR4 *pres)
{
	float dp = DotQuats(q0,q1);

	D3DVECTOR4 _q1 = q1;
	if(dp < 0.f)
	{
		_q1.x = -q1.x;
		_q1.y = -q1.y;
		_q1.z = -q1.z;
		_q1.w = -q1.w;
		dp = -dp;
	}
	
	if(IsEq(dp,1.f))
	{
		pres->x = q0.x * (1.f - t) + _q1.x * t;
		pres->y = q0.y * (1.f - t) + _q1.y * t;
		pres->z = q0.z * (1.f - t) + _q1.z * t;
		pres->w = q0.w * (1.f - t) + _q1.w * t;
	}
	else
	{
		float angle = fast_acos(dp);
		float t0 = fast_sin(angle*(1.f-t));
		float t1 = fast_sin(angle*t);
		
		pres->x = q0.x * t0 + _q1.x * t1;
		pres->y = q0.y * t0 + _q1.y * t1;
		pres->z = q0.z * t0 + _q1.z * t1;
		pres->w = q0.w * t0 + _q1.w * t1;

		float oo_sin_theta = 1.f/fast_sin(angle);

		pres->x *= oo_sin_theta;
		pres->y *= oo_sin_theta;
		pres->z *= oo_sin_theta;
		pres->w *= oo_sin_theta;
	}
}
///////////////////////////////////////////////////////////////////////////////
inline bool  IsEq(float a,float b)
{
	return fast_fabs(a-b) <= 0.00001f;
}
///////////////////////////////////////////////////////////////////////////////
inline bool  PtsEq(const D3DVECTOR &a,const D3DVECTOR &b)
{
	return IsEq(a.x,b.x) && IsEq(a.y,b.y) && IsEq(a.z,b.z);
}
///////////////////////////////////////////////////////////////////////////////
inline void  AddScaled(D3DVECTOR* ptarget, const D3DVECTOR& src, float scale)
{
	ptarget->x += src.x * scale;
	ptarget->y += src.y * scale;
	ptarget->z += src.z * scale;
}
///////////////////////////////////////////////////////////////////////////////
inline float  QuickLength(const D3DVECTOR& vec)
{
	float h = (float) fabs(vec.x);
	float m = (float) fabs(vec.y);
	float l = (float) fabs(vec.z);
	float t;
	if (m>h) { t = m; m = h; h = t; }
	if (l>m) { t = l; l = m; m = t; }
	if (m>h) { t = m; m = h; h = t; }
	return (1.043388475f * (h + 0.34375f * m + 0.25f * l));
}
///////////////////////////////////////////////////////////////////////////////
inline void  QuickNormalize(D3DVECTOR* p_vec)
{
	float qlen = QuickLength(*p_vec);
	if (qlen < 0.000001f) return;
	float oo_qlen = 1.0f / qlen;
	Scale(p_vec, oo_qlen);
}
///////////////////////////////////////////////////////////////////////////////
inline void  Add(const D3DVECTOR &a, const D3DVECTOR &b, D3DVECTOR *pres)
{
	pres->x = a.x + b.x;
	pres->y = a.y + b.y;
	pres->z = a.z + b.z;
}
///////////////////////////////////////////////////////////////////////////////
inline void SinCos(const float &a, float *ps, float *pc)
{
	_asm
	{
		mov eax,[a]
		fld dword ptr[eax]
		fsincos
		mov ebx,[pc]
		mov ecx,[ps]
		fstp dword ptr[ebx]
		fstp dword ptr[ecx]
	}
}

const float F_0 = 0.0f;

///////////////////////////////////////////////////////////////////////////////
inline float AngleFromSinCos(const float &s, const float &c)
{
	// Calculates the angle that produces a given sin and cosine.
	// This is a special case of atan2.
	//
	// Returns a value in (-Pi, Pi/2]

	if (s <= -1) return -Pi / 2;
	if (s >= 1)  return Pi / 2;
	if (c <= -1) return Pi;
	if (c >= 1)  return F_0;

	// Use either asin or acos, depending on what we think will have the
	// best numerical performance.
	float a;
	if (s < .1f && s > -.1f)
	{
		a = (float) fast_asin(s);
		if (c < 0) 
		{
			a = (float) Pi - a;
			if (a > Pi) a -= 2 * Pi;
		}
	}
	else
	{
		a = (float) fast_acos(c);
		if (s < 0) a = (float) - a;
	}

	return a;
}
///////////////////////////////////////////////////////////////////////////////
inline void bs_swap(float& a, float& b)
{
	float tmp = a;
	a = b;
	b = tmp;
}
///////////////////////////////////////////////////////////////////////////////
inline void bs_swap(int& a, int& b)
{
	int tmp = a;
	a = b;
	b = tmp;
}
///////////////////////////////////////////////////////////////////////////////
#endif // __XBS_MATH_INL_H__