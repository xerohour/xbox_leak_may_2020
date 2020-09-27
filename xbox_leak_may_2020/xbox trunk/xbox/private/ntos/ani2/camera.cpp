//
//	camera.cpp
//
///////////////////////////////////////////////////////////////////////////////
//  Copyright (C) 2001, Pipeworks Software Inc.
//				All rights reserved
#include "precomp.h"
#include "camera.h"
#include "renderer.h"

///////////////////////////////////////////////////////////////////////////////
void Camera::Init()
{
	bClipPlanesSet = false;
	bWTPValid = false;
}
///////////////////////////////////////////////////////////////////////////////
void Camera::UnInit()
{
}
///////////////////////////////////////////////////////////////////////////////
void Camera::lookAt(const D3DVECTOR &cam_pos, const D3DVECTOR &look_pt, const D3DVECTOR &up)
{
	SetLookAt(cam_pos,look_pt,up,&matWTC);

	vPos = cam_pos;
	vLook = look_pt;
	vUp = up;
	SetInverse(matWTC,&matCTW);
	bClipPlanesSet = false;
	bWTPValid = false;
}
///////////////////////////////////////////////////////////////////////////////
void Camera::setProjection(float fov_in_y, float aspect, float near_plane, float far_plane)
{
	SetProjection(fov_in_y,aspect,near_plane,far_plane,&matProj);
	fNearPlane = near_plane;
	fFarPlane = far_plane;
	fAspectRatio = aspect;
	bClipPlanesSet = false;
	bWTPValid = false;
}
///////////////////////////////////////////////////////////////////////////////
void Camera::translate(const D3DVECTOR &rv)
{
	matCTW._41 += rv.x;
	matCTW._42 += rv.y;
	matCTW._43 += rv.z;

	SetInverse(matCTW,&matWTC);

	bClipPlanesSet = false;
	bWTPValid = false;
}
///////////////////////////////////////////////////////////////////////////////
void Camera::setCTW(const D3DMATRIX &ctw)
{
	matCTW = ctw;
	SetInverse(matCTW,&matWTC);

	bClipPlanesSet = false;
	bWTPValid = false;
}
///////////////////////////////////////////////////////////////////////////////
void Camera::setWTC(const D3DMATRIX &wtc)
{
	matWTC = wtc;
	SetInverse(matWTC,&matCTW);

	bClipPlanesSet = false;
	bWTPValid = false;
}
///////////////////////////////////////////////////////////////////////////////
bool Camera::sphereVisCheck(const D3DVECTOR &pos, float rad)
{
	D3DVECTOR dv;

	Set(&dv,pos.x-matCTW._41,pos.y-matCTW._42,pos.z-matCTW._43);
	float dot = Dot(dv,*((D3DVECTOR *)&matCTW._31));

	if(dot+rad < fNearPlane)
		return false;
	if(dot-rad > fFarPlane)
		return false;

	if(!bClipPlanesSet)
	{
		updateClipPlanes();
	}

	for(int i=0; i<4; i++)
	{
		dot = Dot(pos,clipPlanes[i]);
		if(dot - clipPlanes[i].w > rad)
			return false;
	}
	
	return true;		
}
///////////////////////////////////////////////////////////////////////////////
void Camera::updateClipPlanes()
{
	D3DVECTOR pts[4];
	Set(&pts[0], 1.f, 1.f, 1.f);
	Set(&pts[1], 1.f,-1.f, 1.f);
	Set(&pts[2],-1.f, 1.f, 1.f);
	Set(&pts[3],-1.f,-1.f, 1.f);

	int ids[] =
	{
		0,1, 
		3,2, 
		2,0, 
		1,3, 
	};

	for(int i=0; i<4; i++)
	{
		D3DVECTOR norm,v;
		Sub(pts[ids[i*2+1]],pts[ids[i*2]],&v);
		Cross(pts[ids[i*2]],v,&norm);
		Normalize(&norm);

		TransformVector(norm,matCTW,&clipPlanes[i]);
		clipPlanes[i].w = Dot(clipPlanes[i],*(D3DVECTOR *)&matCTW._41);
	}

	bClipPlanesSet = true;
}
///////////////////////////////////////////////////////////////////////////////
float Camera::getPixelScaleForZ(float z) const
{
	float v = z / (matProj._11 * 320.f);
	return v;
}
///////////////////////////////////////////////////////////////////////////////
D3DMATRIX & Camera::getWTP()
{
	if( !bWTPValid )
	{
		MulMats(matWTC,matProj,&matWTP);
		bWTPValid = true;
	}

	return matWTP;
}