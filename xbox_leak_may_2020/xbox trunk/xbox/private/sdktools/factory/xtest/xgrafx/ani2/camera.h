//
//	camera.h
//
///////////////////////////////////////////////////////////////////////////////
//  Copyright (C) 2001, Pipeworks Software Inc.
//				All rights reserved
#ifndef __CAMERA_H__
#define __CAMERA_H__

#include "xbs_math.h"

///////////////////////////////////////////////////////////////////////////////
class Camera
{
protected:

	

	D3DVECTOR4   clipPlanes[4];
	bool         bClipPlanesSet;
	float        fNearPlane;
	float        fFarPlane;
	float		 fAspectRatio;

	bool         bWTPValid;

	float        fTime;

	D3DVECTOR    vPos;
	D3DVECTOR    vLook;
	D3DVECTOR    vUp;

	D3DMATRIX    matWTP;

public:

	D3DMATRIX    matCTW;
	D3DMATRIX    matWTC;
	D3DMATRIX    matProj;


	void Init();
	void UnInit();

	void lookAt(const D3DVECTOR &cam_pos, const D3DVECTOR &look_pt, const D3DVECTOR &up);
	void setProjection(float fov_in_y, float aspect, float near_plane, float far_plane);
	void setCTW(const D3DMATRIX &ctw);
	void setWTC(const D3DMATRIX &wtc);
	void translate(const D3DVECTOR &relative_vector);

	bool sphereVisCheck(const D3DVECTOR &pos, float r);
	void updateClipPlanes();

	float getPixelScaleForZ(float z) const ;
	D3DMATRIX & getWTP();
	void		getCameraPos (D3DVECTOR* p_pos)		const	{ *p_pos = vPos; }
	void		getCameraLook(D3DVECTOR* p_look)	const	{ *p_look= vLook; }
	float		getFarPlane()						const	{ return fFarPlane; }
	float		getAspectRatio()					const	{ return fAspectRatio; }
};
#endif // __BS_CAMERA_H__