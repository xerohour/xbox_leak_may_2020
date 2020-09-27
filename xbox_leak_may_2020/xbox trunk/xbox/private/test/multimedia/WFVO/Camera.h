// Camera.h: interface for the CCamera class.
//
//////////////////////////////////////////////////////////////////////

#ifndef CAMERA_H
#define CAMERA_H

//------------------------------------------------------------------------------
//	Includes:
//------------------------------------------------------------------------------
#include "xtl.h"
#include "xgmath.h"

namespace WFVO {

//------------------------------------------------------------------------------
//	CCamera:
//------------------------------------------------------------------------------
class CCamera  
{
public:

	struct Vertex
	{
		FLOAT	x, y, z;
		DWORD	color;
	};

public:

	CCamera(void);
	virtual ~CCamera(void);
	void Render(IDirect3DDevice8*);
	void SetViewport(DWORD, DWORD, DWORD, DWORD, float, float);
	void SetViewport(const D3DVIEWPORT8&);
	const D3DVIEWPORT8& GetViewport(void);
	void LookAt(const XGVECTOR3&, const XGVECTOR3&, const XGVECTOR3&);
	void SetPerspectiveFov(float, float, float, float);
	void Zoom(float);
	void Rotate(float, const XGVECTOR3&);
	void RotateX(float);
	void RotateY(float);
	void RotateZ(float);
	void EnableLight(BOOL, BOOL fixedLight);
	void SetLight(DWORD, const D3DLIGHT8&);
	void RotateLight(float, const XGVECTOR3&);
	void RotateLightX(float);
	void RotateLightY(float);
	void SetDrawAxis(BOOL);
	void SetHome(void);
	void GoHome(void);
    void GetPosition(XGVECTOR3* pvPosition);

private:

	void UpdateLight(void);
	void MakeRotationMatrices(float, const XGVECTOR3&, XGMATRIX&, XGMATRIX&);

private:

	D3DVIEWPORT8	m_viewport;			// Viewport
	XGMATRIX		m_viewMatrix;		// View matrix
	XGMATRIX		m_projectionMatrix;	// Projection matrix
	D3DLIGHT8		m_light;			// Light parameters
	XGVECTOR3		m_focalPoint;		// Focal point of camera
	XGVECTOR3		m_position;			// Position of camera
	XGVECTOR3		m_viewUp;			// View up vector of camera
	XGVECTOR3		m_vpn;				// View plane normal of camera
	XGVECTOR3		m_lightPosition;	// Position of light
	DWORD			m_lightNum;			// Llight number
	BOOL			m_lightOn;			// TRUE to enable the light
	BOOL			m_fixedLight;		// TRUE to lock light to the camera position
	Vertex			m_axis[6];			// Axis for rendering
	BOOL			m_drawAxis;			// TRUE to draw axis
	Vertex			m_lightNormal[2];	// Light normal
	XGVECTOR3		m_homeFocalPoint;
	XGVECTOR3		m_homePosition;
	XGVECTOR3		m_homeViewUp;
	XGVECTOR3		m_homeVpn;
	XGVECTOR3		m_homeLightPosition;
	BOOL			m_homeLightOn;
	BOOL			m_homeFixedLight;
	float			m_aspect;
	float			m_fov;
	float			m_zNear;
	float			m_zFar;
	float			m_dz;

};
}
#endif
