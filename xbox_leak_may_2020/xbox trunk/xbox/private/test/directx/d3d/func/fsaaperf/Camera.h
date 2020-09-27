// Camera.h: interface for the CCamera class.
//
//////////////////////////////////////////////////////////////////////

#ifndef CAMERA_H
#define CAMERA_H

//------------------------------------------------------------------------------
//	Includes:
//------------------------------------------------------------------------------
#include "d3dlocus.h"

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
	//--------------------------------------------------------------------------
	//	Purpose:
	//		Constructor
	//
	//	Parameters:
	//		Nothing
	//
	//	Returns:
	//		Nothing
	//--------------------------------------------------------------------------

	virtual ~CCamera(void);
	//--------------------------------------------------------------------------
	//	Purpose:
	//		Does nothing
	//
	//	Parameters:
	//		Nothing
	//
	//	Returns:
	//		Nothing
	//--------------------------------------------------------------------------

	
	void Render(CDevice8* pD3DDevice);
	//--------------------------------------------------------------------------
	//	Purpose:
	//		Render the scene
	//
	//	Parameters:
	//		pD3DDevice:	Device to draw to
	//
	//	Returns:
	//		Nothing
	//--------------------------------------------------------------------------

	void SetViewport(DWORD x, DWORD y, DWORD w, DWORD h, float minZ, float maxZ);
	void SetViewport(const D3DVIEWPORT8& viewport);
	//--------------------------------------------------------------------------
	//	Purpose:
	//		Sets the viewport
	//
	//	Parameters:
	//		x:			X coordinate
	//		y:			Y coordinate
	//		w:			Width
	//		h:			Height
	//		minZ:		Z coordinate of fron clipping plane (0.0 - 1.0)
	//		maxZ:		Z coordinate of back clipping plane (0.0 - 1.0)
	//		vieport:	Complete viewport structure
	//
	//	Returns:
	//		Nothing
	//--------------------------------------------------------------------------

	const D3DVIEWPORT8& GetViewport(void);
	//--------------------------------------------------------------------------
	//	Purpose:
	//		Returns the viewport
	//
	//	Parameters:
	//		Nothing
	//
	//	Returns:
	//		The current viewport
	//--------------------------------------------------------------------------

	void LookAt(const D3DXVECTOR3& position, 
				const D3DXVECTOR3& focalPoint, 
				const D3DXVECTOR3& viewUp);
	//--------------------------------------------------------------------------
	//	Purpose:
	//		Sets the (initial) orientation of the camera
	//
	//	Parameters:
	//		position:	Position of camera
	//		focalPoint:	Focal point of camera
	//		viewUp:		View up vector
	//
	//	Returns:
	//		Nothing
	//
	//	Note:
	//		This function assume the values are in a LH coordinate system.
	//--------------------------------------------------------------------------

	void SetPerspectiveFov(float	fov,
						   float	aspect,
						   float	zNear,
						   float	zFar);
	//--------------------------------------------------------------------------
	//	Purpose:
	//		Sets the field of view and the perspective projection
	//
	//	Parameters:
	//		fov:	field of view in degrees.
	//		aspect:	Aspect ratio
	//		zNear:	Near clipping plane
	//		zFar:	Far clipping plane
	//
	//	Returns:
	//		Nothing
	//--------------------------------------------------------------------------

	void Zoom(float amount);
	//--------------------------------------------------------------------------
	//	Purpose:
	//		Zooms the camera in or out.
	//
	//	Parameters:
	//		amount:	Amount to zoom in world coordinates.
	//
	//	Returns:
	//		Nothing
	//
	//	Notes:
	//		The camera will never zoom past/to the focal point
	//--------------------------------------------------------------------------

	void Rotate(float degrees, const D3DXVECTOR3& axis);
	//--------------------------------------------------------------------------
	//	Purpose:
	//		Rotates the scene around an arbitrary axis.
	//
	//	Parameters:
	//		degrees:	Amount to rotate.
	//		axis:		Axis to rotate about.
	//
	//	Returns:
	//		Nothing
	//
	//	Note:
	//		Regardless of the current orientation of the objects in the scene,
	//		the rotation will be about axis relative to the screen (X into, Y
	//		up and X to the right).
	//--------------------------------------------------------------------------

	void RotateX(float degrees);
	//--------------------------------------------------------------------------
	//	Purpose:
	//		Rotates the scene around the X axis.
	//
	//	Parameters:
	//		degrees:	Amount to rotate in degrees.
	//
	//	Returns:
	//		Nothing
	//
	//	Note:
	//		Regardless of the current orientation of the objects in the scene,
	//		the rotation will be about axis relative to the screen (X into, Y
	//		up and X to the right).
	//--------------------------------------------------------------------------

	void RotateY(float degrees);
	//--------------------------------------------------------------------------
	//	Purpose:
	//		Rotates the scene around the Y axis.
	//
	//	Parameters:
	//		degrees:	Amount to rotate in degrees
	//
	//	Returns:
	//		Nothing
	//
	//	Note:
	//		Regardless of the current orientation of the objects in the scene,
	//		the rotation will be about axis relative to the screen (X into, Y
	//		up and X to the right).
	//--------------------------------------------------------------------------

	void RotateZ(float degrees);
	//--------------------------------------------------------------------------
	//	Purpose:
	//		Rotates the scene around the Z axis.
	//
	//	Parameters:
	//		degrees:	Amount to rotate in degrees
	//
	//	Returns:
	//		Nothing
	//
	//	Note:
	//		Regardless of the current orientation of the objects in the scene,
	//		the rotation will be about axis relative to the screen (X into, Y
	//		up and X to the right).
	//--------------------------------------------------------------------------

	void EnableLight(BOOL turnOn, BOOL fixedLight=TRUE);
	//--------------------------------------------------------------------------
	//	Purpose:
	//		Enables the headlight.
	//
	//	Parameters:
	//		turnOn:		TRUE to enable, FALSE to disable
	//		fixedLight:	TRUE to follow camera position, FALSE to float
	//
	//	Returns:
	//		Nothing
	//
	//	Notes:
	//		Regardless of the state of this call, 
	//		SetRenderState(D3DRS_LIGHTING, TRUE/FALSE) will not be called.
	//		This call will basically just call LightEnable(lightNum, TRUE/FALSE)
	//--------------------------------------------------------------------------

	void SetLight(DWORD lightNum, const D3DLIGHT8& lightParams);
	//--------------------------------------------------------------------------
	//	Purpose:
	//		Sets the parameters of the light
	//
	//	Parameters:
	//		lightNum:		Light number to use.
	//		lightParams:	Parameters of the light source
	//
	//	Returns:
	//		Nothing
	//
	//	Notes:
	//		The only parameters used are: Diffuse, Ambient, Specular, and Range
	//--------------------------------------------------------------------------

	void RotateLight(float degrees, const D3DXVECTOR3& axis);
	//--------------------------------------------------------------------------
	//	Purpose:
	//		Rotates the Light around an arbitrary axis.
	//
	//	Parameters:
	//		degrees:	Amount to rotate.
	//		axis:		Axis to rotate about.
	//
	//	Returns:
	//		Nothing
	//
	//	Note:
	//		Regardless of the current orientation of the objects in the scene,
	//		the rotation will be about axis relative to the screen (X into, Y
	//		up and X to the right).
	//--------------------------------------------------------------------------

	void RotateLightX(float degrees);
	//--------------------------------------------------------------------------
	//	Purpose:
	//		Rotates the Light around the X axis.
	//
	//	Parameters:
	//		degrees:	Amount to rotate in degrees.
	//
	//	Returns:
	//		Nothing
	//
	//	Note:
	//		Regardless of the current orientation of the objects in the scene,
	//		the rotation will be about axis relative to the screen (X into, Y
	//		up and X to the right).
	//--------------------------------------------------------------------------

	void RotateLightY(float degrees);
	//--------------------------------------------------------------------------
	//	Purpose:
	//		Rotates the Light around the Y axis.
	//
	//	Parameters:
	//		degrees:	Amount to rotate in degrees
	//
	//	Returns:
	//		Nothing
	//
	//	Note:
	//		Regardless of the current orientation of the objects in the scene,
	//		the rotation will be about axis relative to the screen (X into, Y
	//		up and X to the right).
	//--------------------------------------------------------------------------

	void SetDrawAxis(BOOL draw);
	//--------------------------------------------------------------------------
	//	Purpose:
	//		Enables and disbles the drawing of an axis
	//
	//	Parameters:
	//		draw:	TRUE to draw the axis, FALSE to hide the axis
	//
	//	Returns:
	//		Nothing
	//
	//	Note:
	//		Red == +X, Blue == +Y and Green == +Z
	//--------------------------------------------------------------------------

	void SetHome(void);
	//--------------------------------------------------------------------------
	//	Purpose:
	//		Sets the home position of the camera to the current position
	//
	//	Parameters:
	//		Nothing
	//
	//	Returns:
	//		Nothing
	//--------------------------------------------------------------------------

	void GoHome(void);
	//--------------------------------------------------------------------------
	//	Purpose:
	//		Moves the camera to its home position
	//
	//	Parameters:
	//		Nothing
	//
	//	Returns:
	//		Nothing
	//--------------------------------------------------------------------------

private:

	void UpdateLight(void);
	//--------------------------------------------------------------------------
	//	Purpose:
	//		Updates the light direction based upon the movement of the camera
	//		so the light always points towards the focal point
	//
	//	Parameters:
	//		Nothing
	//
	//	Returns:
	//		Nothing
	//--------------------------------------------------------------------------

	void MakeRotationMatrices(float					degrees, 
							  const D3DXVECTOR3&	axis,
							  D3DXMATRIX&			transform,
							  D3DXMATRIX&			rotation);
	//--------------------------------------------------------------------------
	//	Purpose:
	//		Create a transformation and rotation (only) matrix from the
	//		current focal point and a rotation about an arbitrary axis
	//
	//	Parameters:
	//		degrees:	Degrees to rotate about
	//		axis:		Axis to rotate about
	//		transform:	Complete transormation matrix
	//		rotation:	Rotation matrix only
	//
	//	Returns:
	//		Nothing
	//--------------------------------------------------------------------------

private:

	D3DVIEWPORT8	m_viewport;			// Viewport
	D3DXMATRIX		m_viewMatrix;		// View matrix
	D3DXMATRIX		m_projectionMatrix;	// Projection matrix
	D3DLIGHT8		m_light;			// Light parameters
	D3DXVECTOR3		m_focalPoint;		// Focal point of camera
	D3DXVECTOR3		m_position;			// Position of camera
	D3DXVECTOR3		m_viewUp;			// View up vector of camera
	D3DXVECTOR3		m_vpn;				// View plane normal of camera
	D3DXVECTOR3		m_lightPosition;	// Position of light
	DWORD			m_lightNum;			// Llight number
	BOOL			m_lightOn;			// TRUE to enable the light
	BOOL			m_fixedLight;		// TRUE to lock light to the camera position
	Vertex			m_axis[6];			// Axis for rendering
	BOOL			m_drawAxis;			// TRUE to draw axis
	Vertex			m_lightNormal[2];	// Light normal
	D3DXVECTOR3		m_homeFocalPoint;
	D3DXVECTOR3		m_homePosition;
	D3DXVECTOR3		m_homeViewUp;
	D3DXVECTOR3		m_homeVpn;
	D3DXVECTOR3		m_homeLightPosition;
	BOOL			m_homeLightOn;
	BOOL			m_homeFixedLight;
	float			m_aspect;
	float			m_fov;
	float			m_zNear;
	float			m_zFar;

};

#endif
