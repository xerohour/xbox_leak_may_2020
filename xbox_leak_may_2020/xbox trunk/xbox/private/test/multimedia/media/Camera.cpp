/*++

Copyright (c) 2001 Microsoft Corporation

Module Name:

	Camera.cpp

Abstract:

	Camera for D3D viewing

Author:

	Robert Heitkamp (robheit) 27-Apr-2001

Environment:

	Xbox only

Revision History:

	27-Apr-2001 robheit
		Initial Version

--*/

//------------------------------------------------------------------------------
//	Includes:
//------------------------------------------------------------------------------
#include "Camera.h"

using namespace Media;

namespace Media {

//------------------------------------------------------------------------------
//	CCamera::CCamera
//------------------------------------------------------------------------------
CCamera::CCamera(void)
/*++

Routine Description:

	Constructor

Arguments:

	None

Return Value:

	None

--*/
{
	D3DLIGHT8	light;

	// Default viewport
	m_viewport.X		= 0;
	m_viewport.Y		= 0;
	m_viewport.Width	= 640;
	m_viewport.Height	= 480;
	m_viewport.MinZ		= 0.0f;
	m_viewport.MaxZ		= 1.0f;

	// View matrix
	XGMatrixIdentity(&m_viewMatrix);

	// Projection matrix
	XGMatrixIdentity(&m_projectionMatrix);

	// light
	light.Type			= D3DLIGHT_DIRECTIONAL;
    light.Diffuse.r		= 1.0f;
    light.Diffuse.g		= 1.0f;
    light.Diffuse.b		= 1.0f;
    light.Diffuse.a		= 1.0f;
    light.Specular.r	= 1.0f;
    light.Specular.g	= 1.0f;
    light.Specular.b	= 1.0f;
    light.Specular.a	= 1.0f;
    light.Ambient.r		= 1.0f;
    light.Ambient.g		= 1.0f;
    light.Ambient.b		= 1.0f;
    light.Ambient.a		= 1.0f;
    light.Direction.x	= 0.0f;
    light.Direction.y	= 0.0f;
    light.Direction.z	= 1.0f;
    light.Range			= 1000.0f;
	SetLight(0, light);

	// Other parameters
	m_focalPoint		= XGVECTOR3(0.0f, 0.0f, 0.0f);
	m_position			= XGVECTOR3(0.0f, 0.0f, 1.0f);
	m_viewUp			= XGVECTOR3(0.0f, 1.0f, 0.0f);
	m_vpn				= XGVECTOR3(0.0f, 0.0f, 1.0f);
	m_lightPosition		= m_position;
	m_lightNum			= 5;
	m_lightOn			= TRUE;
	m_fixedLight		= TRUE;
	m_drawAxis			= FALSE;

	// Axis color
	m_axis[0].color	= 0xffff0000;
	m_axis[1].color	= 0xffff0000;
	m_axis[2].color	= 0xff00ff00;
	m_axis[3].color	= 0xff00ff00;
	m_axis[4].color	= 0xff0000ff;
	m_axis[5].color	= 0xff0000ff;

	m_lightNormal[0].color	= 0xffffffff;
	m_lightNormal[1].color	= 0xffffffff;
}

//------------------------------------------------------------------------------
//	CCamera::~CCamera
//------------------------------------------------------------------------------
CCamera::~CCamera(void)
/*++

Routine Description:

	Destructor

Arguments:

	None

Return Value:

	None

--*/
{
}

//------------------------------------------------------------------------------
//	CCamera::Render
//------------------------------------------------------------------------------
void 
CCamera::Render(
				IN IDirect3DDevice8* pD3DDevice
				)
/*++

Routine Description:

	Render the scene

Arguments:

	IN pD3DDevice -	Device to draw to

Return Value:

	None

--*/
{
	// View matrix
	pD3DDevice->SetTransform(D3DTS_VIEW, &m_viewMatrix);

	// Projection matrix
	pD3DDevice->SetTransform(D3DTS_PROJECTION, &m_projectionMatrix);

	pD3DDevice->SetVertexShader(D3DFVF_XYZ | D3DFVF_DIFFUSE);

	// Draw the axis?
	if(m_drawAxis)
	{
		DWORD oldState;
		
		pD3DDevice->GetRenderState(D3DRS_LIGHTING, &oldState);
		pD3DDevice->SetRenderState(D3DRS_LIGHTING, FALSE);

		// Draw the axis?
		pD3DDevice->SetVertexShader(D3DFVF_XYZ | D3DFVF_DIFFUSE);
		pD3DDevice->DrawPrimitiveUP(D3DPT_LINELIST, 3, m_axis, 
									sizeof(CCamera::Vertex));

		if(m_lightOn)
		{
			pD3DDevice->SetVertexShader(D3DFVF_XYZ | D3DFVF_DIFFUSE);
			pD3DDevice->DrawPrimitiveUP(D3DPT_LINELIST, 1, m_lightNormal, 
										sizeof(CCamera::Vertex));
		}

		if(oldState)
			pD3DDevice->SetRenderState(D3DRS_LIGHTING, TRUE);
	}

	// light
	pD3DDevice->SetLight(m_lightNum, &m_light);
	pD3DDevice->LightEnable(m_lightNum, m_lightOn);

}

//------------------------------------------------------------------------------
//	CCamera::SetViewport
//------------------------------------------------------------------------------
void 
CCamera::SetViewport(
					 IN DWORD	x, 
					 IN DWORD	y, 
					 IN DWORD	w, 
					 IN DWORD	h, 
					 IN float	minZ, 
					 IN float	maxZ
					 )
/*++

Routine Description:

	Sets the viewport

Arguments:

	IN x -			X coordinate
	IN y -			Y coordinate
	IN w -			Width
	IN h -			Height
	IN minZ -		Z coordinate of fron clipping plane (0.0 - 1.0)
	IN maxZ -		Z coordinate of back clipping plane (0.0 - 1.0)
	IN vieport -	Complete viewport structure

Return Value:

	None

--*/
{
	m_viewport.X		= x;
	m_viewport.Y		= y;
	m_viewport.Width	= w;
	m_viewport.Height	= h;
	m_viewport.MinZ		= minZ;
	m_viewport.MaxZ		= maxZ;
}

//------------------------------------------------------------------------------
//	CCamera::SetViewport
//------------------------------------------------------------------------------
void 
CCamera::SetViewport(
					 IN const D3DVIEWPORT8& viewport
					 )
/*++

Routine Description:

	Sets the viewport

Arguments:

	IN viewport -	Viewport

Return Value:

	None

--*/
{
	m_viewport.X		= viewport.X;
	m_viewport.Y		= viewport.Y;
	m_viewport.Width	= viewport.Width;
	m_viewport.Height	= viewport.Height;
	m_viewport.MinZ		= viewport.MinZ;
	m_viewport.MaxZ		= viewport.MaxZ;
}

//------------------------------------------------------------------------------
//	CCamera::GetViewport
//------------------------------------------------------------------------------
const D3DVIEWPORT8& 
CCamera::GetViewport(void)
/*++

Routine Description:

	Returns the viewport

Arguments:

	None

Return Value:

	The current viewport

--*/
{
	return m_viewport;
}

//------------------------------------------------------------------------------
//	CCamera::LookAt
//------------------------------------------------------------------------------
void 
CCamera::LookAt(
				IN const XGVECTOR3& position, 
				IN const XGVECTOR3& focalPoint, 
				IN const XGVECTOR3& viewUp
				)
/*++

Routine Description:

	Sets the (initial) orientation of the camera

Arguments:

	IN position -	Position of camera
	IN focalPoint -	Focal point of camera
	IN viewUp -		View up vector

Return Value:

	None

--*/
{
	float	length;

	m_focalPoint	= focalPoint;
	m_position		= position;
	m_viewUp		= viewUp;
	XGMatrixLookAtLH(&m_viewMatrix, &m_position, &m_focalPoint, &m_viewUp);
	XGVec3Subtract(&m_vpn, &m_focalPoint, &m_position);

	length	= XGVec3Length(&m_vpn);
	m_vpn	/= length;
	length	/= 4.0f;

	// Calculate the axis
	m_axis[0].x	= m_focalPoint.x;
	m_axis[0].y	= m_focalPoint.y;
	m_axis[0].z	= m_focalPoint.z;
	m_axis[1].x	= m_axis[0].x + length;
	m_axis[1].y	= m_axis[0].y;
	m_axis[1].z	= m_axis[0].z;
	m_axis[2].x	= m_axis[0].x;
	m_axis[2].y	= m_axis[0].y;
	m_axis[2].z	= m_axis[0].z;
	m_axis[3].x	= m_axis[0].x;
	m_axis[3].y	= m_axis[0].y + length;
	m_axis[3].z	= m_axis[0].z;
	m_axis[4].x	= m_axis[0].x;
	m_axis[4].y	= m_axis[0].y;
	m_axis[4].z	= m_axis[0].z;
	m_axis[5].x	= m_axis[0].x;
	m_axis[5].y	= m_axis[0].y;
	m_axis[5].z	= m_axis[0].z + length;

	UpdateLight();
}

//------------------------------------------------------------------------------
//	CCamera::SetFieldOfView
//------------------------------------------------------------------------------
void 
CCamera::SetPerspectiveFov(
						   IN float	fov,
						   IN float	aspect,
						   IN float	zNear,
						   IN float	zFar
						   )
/*++

Routine Description:

	Sets the field of view and the perspective projection

Arguments:

	IN fov -	Field of view in degrees.
	IN aspect -	Aspect ratio
	IN zNear -	Near clipping plane
	IN zFar -	Far clipping plane

Return Value:

	None

--*/
{
	m_aspect	= aspect;
	m_fov		= XGToRadian(fov);
	m_zNear		= zNear;
	m_zFar		= zFar;
	m_dz		= m_zFar - m_zNear;
	XGMatrixPerspectiveFovLH(&m_projectionMatrix, m_fov, m_aspect, m_zNear, 
							   m_zFar);
}

//------------------------------------------------------------------------------
//	CCamera::Zoom
//------------------------------------------------------------------------------
void 
CCamera::Zoom(
			  IN float amount
			  )
/*++

Routine Description:

	Zooms the camera in or out

Arguments:

	IN amount -	Amount to zoom in world coordinates

Return Value:

	None

--*/
{
	float		length;
	float		zFar;
	float		zNear;
	XGVECTOR3	oldPosition = m_position;

	// Make sure user doesn't zoom past focal point
	length = XGVec3Length(&XGVECTOR3(m_focalPoint - m_position));
	if((length <= 0.01) && (amount > 0.0f))
		return;
	if(amount >= length)
		amount = length - 0.01f;

	// Move the point
	m_position.x += m_vpn.x * amount;
	m_position.y += m_vpn.y * amount;
	m_position.z += m_vpn.z * amount;
	
	// Reproject
	XGMatrixLookAtLH(&m_viewMatrix, &m_position, &m_focalPoint, &m_viewUp);

	length = XGVec3Length(&XGVECTOR3(m_focalPoint - m_position));
	m_zNear = length - (m_dz / 2.0f) - 0.01f;
	m_zFar	= length + (m_dz / 2.0f) + 0.01f;

	if(m_zNear < 0.00001f)
		zNear = 0.00001f;
	else
		zNear = m_zNear;
	if(m_zFar < 0.00001f)
		zFar = 0.00001f;
	else
		zFar = m_zFar;

	XGMatrixPerspectiveFovLH(&m_projectionMatrix, m_fov, m_aspect, zNear, zFar);
}

//------------------------------------------------------------------------------
//	CCamera::Rotate
//------------------------------------------------------------------------------
void 
CCamera::Rotate(
				IN float				degrees, 
				IN const XGVECTOR3&	axis
				)
/*++

Routine Description:

	Rotates the scene around an arbitrary axis

Arguments:

	IN degrees -	Amount to rotate.
	IN axis -		Axis to rotate about

Return Value:

	None

--*/
{
	XGMATRIX	rotate;

	if(degrees == 0.0f)
		return;

	MakeRotationMatrices(degrees, axis, m_viewMatrix, rotate);

	// Transform the position
	XGVec3TransformCoord(&m_position, &m_position, &m_viewMatrix);

	// Recalculate the view plane normal
	XGVec3Normalize(&m_vpn, &XGVECTOR3(m_focalPoint - m_position));

	// Transform the view up vector
	XGVec3TransformCoord(&m_viewUp, &m_viewUp, &rotate);

	// Reset the view matrix
	XGMatrixLookAtLH(&m_viewMatrix, &m_position, &m_focalPoint, &m_viewUp);

	UpdateLight();
}

//------------------------------------------------------------------------------
//	CCamera::RotateX
//------------------------------------------------------------------------------
void 
CCamera::RotateX(
				 IN float degrees
				 )
/*++

Routine Description:

	Rotates the scene around the X axis of the screen

	Regardless of the current orientation of the objects in the scene,
	the rotation will be about axis relative to the screen (Z into, Y
	up and X to the right).

Arguments:

	IN degrees -	Amount to rotate in degrees

Return Value:

	None

--*/
{
	XGVECTOR3	axis;

	XGVec3Cross(&axis, &m_vpn, &m_viewUp);
	Rotate(degrees, axis);
}

//------------------------------------------------------------------------------
//	CCamera::RotateY
//------------------------------------------------------------------------------
void 
CCamera::RotateY(
				 IN float degrees
				 )
/*++

Routine Description:

	Rotates the scene around the Y axis of the screen

	Regardless of the current orientation of the objects in the scene,
	the rotation will be about axis relative to the screen (Z into, Y
	up and X to the right).

Arguments:

	IN degrees -	Amount to rotate in degrees

Return Value:

	None

--*/
{
	Rotate(degrees, m_viewUp);
}

//------------------------------------------------------------------------------
//	CCamera::RotateZ
//------------------------------------------------------------------------------
void 
CCamera::RotateZ(
				 IN float degrees
				 )
/*++

Routine Description:

	Rotates the scene around the Z axis of the screen

	Regardless of the current orientation of the objects in the scene,
	the rotation will be about axis relative to the screen (Z into, Y
	up and X to the right).

Arguments:

	IN degrees -	Amount to rotate in degrees

Return Value:

	None

--*/
{
	Rotate(degrees, m_vpn);
}

//------------------------------------------------------------------------------
//	CCamera::EnableLight
//------------------------------------------------------------------------------
void 
CCamera::EnableLight(
					 IN BOOL	turnOn,
					 IN BOOL	fixedLight
					 )
/*++

Routine Description:

	Enables the headlight

Arguments:

	IN turnOn -		TRUE to enable, FALSE to disable
	IN fixedLight -	TRUE to follow camera position, FALSE to float

Return Value:

	None

--*/
{
	m_lightOn		= turnOn;
	m_fixedLight	= fixedLight;
	UpdateLight();
}

//------------------------------------------------------------------------------
//	CCamera::SetLight
//------------------------------------------------------------------------------
void 
CCamera::SetLight(
				  IN DWORD				lightNum, 
				  IN const D3DLIGHT8&	lightParams
				  )
/*++

Routine Description:

	Sets the parameters of the light

Arguments:

	IN lightNum -		Light number to use.
	IN lightParams -	Parameters of the light source

Return Value:

	None

--*/
{
	m_lightNum			= lightNum;

    m_light.Type		= lightParams.Type;
	m_light.Diffuse.r	= lightParams.Diffuse.r;	
	m_light.Diffuse.g	= lightParams.Diffuse.g;	
	m_light.Diffuse.b	= lightParams.Diffuse.b;	
    m_light.Specular.r	= lightParams.Specular.r;
	m_light.Specular.g	= lightParams.Specular.g;
	m_light.Specular.b	= lightParams.Specular.b;
    m_light.Ambient.r	= lightParams.Ambient.r;
	m_light.Ambient.g	= lightParams.Ambient.g;
	m_light.Ambient.b	= lightParams.Ambient.b;
    m_light.Range		= lightParams.Range;
}

//------------------------------------------------------------------------------
//	CCamera::RotateLight
//------------------------------------------------------------------------------
void 
CCamera::RotateLight(
					 IN float				degrees, 
					 IN const XGVECTOR3&	axis
					 )
/*++

Routine Description:

	Rotates the Light around an arbitrary axis

Arguments:

	IN degrees -	Amount to rotate.
	IN axis -		Axis to rotate about.

Return Value:

	None

--*/
{
	XGMATRIX	matrix;
	XGMATRIX	temp;

	if(degrees == 0.0f)
		return;

	MakeRotationMatrices(degrees, axis, matrix, temp);

	// Transform the position
	XGVec3TransformCoord(&m_lightPosition, &m_lightPosition, &matrix);

	UpdateLight();
}

//------------------------------------------------------------------------------
//	CCamera::RotateLightX
//------------------------------------------------------------------------------
void 
CCamera::RotateLightX(
					  IN float degrees
					  )
/*++

Routine Description:

	Rotates the Light around the X axis of the screen

Arguments:

	IN degrees -	Amount to rotate in degrees

Return Value:

	None

--*/
{
	XGVECTOR3	cross;

	XGVec3Cross(&cross, &m_vpn, &m_viewUp);
	RotateLight(degrees, cross);
}

//------------------------------------------------------------------------------
//	CCamera::RotateLightY
//------------------------------------------------------------------------------
void 
CCamera::RotateLightY(
					  IN float degrees
					  )
/*++

Routine Description:

	Rotates the Light around the X axis of the screen

Arguments:

	IN degrees -	Amount to rotate in degrees

Return Value:

	None

--*/
{
	RotateLight(degrees, m_viewUp);
}

//------------------------------------------------------------------------------
//	CCamera::SetDrawAxis
//------------------------------------------------------------------------------
void 
CCamera::SetDrawAxis(
					 IN BOOL draw
					 )
/*++

Routine Description:

	Enables and disbles the drawing of an axis

	Red == +X, Blue == +Y and Green == +Z

Arguments:

	IN draw -	TRUE to draw the axis, FALSE to hide the axis

Return Value:

	None

--*/
{
	m_drawAxis = draw;
}

//------------------------------------------------------------------------------
//	CCamera::SetHome
//------------------------------------------------------------------------------
void
CCamera::SetHome(void)
/*++

Routine Description:

	Sets the home position of the camera to the current position

Arguments:

	None

Return Value:

	None

--*/
{
	m_homePosition		= m_position;
	m_homeViewUp		= m_viewUp;
	m_homeFocalPoint	= m_focalPoint;
	m_homeVpn			= m_vpn;
	m_homeLightPosition	= m_lightPosition;
	m_homeLightOn		= m_lightOn;
	m_homeFixedLight	= m_fixedLight;
	
	UpdateLight();
}

//------------------------------------------------------------------------------
//	CCamera::GoHome
//------------------------------------------------------------------------------
void
CCamera::GoHome(void)
/*++

Routine Description:

	Moves the camera to its home position

Arguments:

	None

Return Value:

	None

--*/
{
	m_position		= m_homePosition;
	m_viewUp		= m_homeViewUp;
	m_focalPoint	= m_homeFocalPoint;
	m_vpn			= m_homeVpn;
	m_lightPosition	= m_homeLightPosition;
	m_lightOn		= m_homeLightOn;
	m_fixedLight	= m_homeFixedLight;
	
	UpdateLight();
}

//------------------------------------------------------------------------------
//	CCamera::UpdateLight
//------------------------------------------------------------------------------
void
CCamera::UpdateLight(void)
/*++

Routine Description:

	Updates the light direction based upon the movement of the camera
	so the light always points towards the focal point

Arguments:

	None

Return Value:

	None

--*/
{
	XGVECTOR3	point;

	if(m_fixedLight)
	{
		m_lightPosition		= m_position;
		m_light.Direction	= m_vpn;
	}
	else
	{
		XGVECTOR3	direction;
		XGVec3Normalize(&direction, &XGVECTOR3(m_focalPoint - m_lightPosition));
		m_light.Direction = direction;
	}

	m_lightNormal[0].x = m_focalPoint.x;
	m_lightNormal[0].y = m_focalPoint.y;
	m_lightNormal[0].z = m_focalPoint.z;

	XGVec3Lerp(&point, &m_focalPoint, &m_lightPosition, 0.5);

	m_lightNormal[1].x = point.x;
	m_lightNormal[1].y = point.y;
	m_lightNormal[1].z = point.z;
}

//------------------------------------------------------------------------------
//	CCamera::MakeRotationMatrices
//------------------------------------------------------------------------------
void
CCamera::MakeRotationMatrices(
							  IN float				degrees, 
							  IN const XGVECTOR3&	axis,
							  OUT XGMATRIX&			transform,
							  OUT XGMATRIX&			rotation
							  )
/*++

Routine Description:

	Create a transformation and rotation (only) matrix from the
	current focal point and a rotation about an arbitrary axis

Arguments:

	IN degrees -	Degrees to rotate about
	IN axis -		Axis to rotate about
	OUT transform -	Complete transormation matrix
	OUT rotation -	Rotation matrix only

Return Value:

	None

--*/
{
	if(degrees == 0.0f)
	{
		XGMatrixIdentity(&transform);
		XGMatrixIdentity(&rotation);
		return;
	}

	XGMATRIX	temp;
	
	XGMatrixTranslation(&transform, -m_focalPoint.x, -m_focalPoint.y, 
						-m_focalPoint.z);
	XGMatrixMultiply(&transform, &transform, 
					 XGMatrixRotationAxis(&rotation, &axis, 
										  XGToRadian(degrees)));
	XGMatrixMultiply(&transform, &transform, 
					 XGMatrixTranslation(&temp, m_focalPoint.x, 
										 m_focalPoint.y, m_focalPoint.z));
}

//------------------------------------------------------------------------------
//	CCamera::GetPosition
//------------------------------------------------------------------------------
void 
CCamera::GetPosition(XGVECTOR3* pvPosition)
{
    if (pvPosition) {
        *pvPosition = m_position;
    }
}

}