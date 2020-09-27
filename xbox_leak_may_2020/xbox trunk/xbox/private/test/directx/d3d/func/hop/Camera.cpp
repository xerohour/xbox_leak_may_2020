// Camera.cpp: implementation of the CCamera class.
//
//////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------------
//	Includes:
//------------------------------------------------------------------------------
#include "Camera.h"

//------------------------------------------------------------------------------
//	CCamera::CCamera
//------------------------------------------------------------------------------
CCamera::CCamera(void)
{
	// Default viewport
	m_viewport.X		= 0;
	m_viewport.Y		= 0;
	m_viewport.Width	= 640;
	m_viewport.Height	= 480;
	m_viewport.MinZ		= 0.0f;
	m_viewport.MaxZ		= 1.0f;

	// View matrix
	D3DXMatrixIdentity(&m_viewMatrix);

	// Projection matrix
	D3DXMatrixIdentity(&m_projectionMatrix);

	// light
	m_light.Type		= D3DLIGHT_DIRECTIONAL;
    m_light.Diffuse.r	= 1.0f;
    m_light.Diffuse.g	= 1.0f;
    m_light.Diffuse.b	= 1.0f;
    m_light.Diffuse.a	= 1.0f;
    m_light.Specular.r	= 1.0f;
    m_light.Specular.g	= 1.0f;
    m_light.Specular.b	= 1.0f;
    m_light.Specular.a	= 1.0f;
    m_light.Ambient.r	= 1.0f;
    m_light.Ambient.g	= 1.0f;
    m_light.Ambient.b	= 1.0f;
    m_light.Ambient.a	= 1.0f;
    m_light.Direction.x	= 0.0f;
    m_light.Direction.y	= 0.0f;
    m_light.Direction.z	= 1.0f;
    m_light.Range		= 1000.0f;

	// Other parameters
	m_focalPoint		= D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	m_position			= D3DXVECTOR3(0.0f, 0.0f, 1.0f);
	m_viewUp			= D3DXVECTOR3(0.0f, 1.0f, 0.0f);
	m_vpn				= D3DXVECTOR3(0.0f, 0.0f, 1.0f);
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
{
}

//------------------------------------------------------------------------------
//	CCamera::Render
//------------------------------------------------------------------------------
void 
CCamera::Render(CDevice8* pD3DDevice)
{
	// View matrix
	pD3DDevice->SetTransform(D3DTS_VIEW, &m_viewMatrix);

	// Projection matrix
	pD3DDevice->SetTransform(D3DTS_PROJECTION, &m_projectionMatrix);

	pD3DDevice->SetVertexShader(D3DFVF_XYZ | D3DFVF_DIFFUSE);

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
CCamera::SetViewport(DWORD	x, 
					 DWORD	y, 
					 DWORD	w, 
					 DWORD	h, 
					 float	minZ, 
					 float	maxZ)
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
CCamera::SetViewport(const D3DVIEWPORT8& viewport)
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
{
	return m_viewport;
}

//------------------------------------------------------------------------------
//	CCamera::LookAt
//------------------------------------------------------------------------------
void 
CCamera::LookAt(const D3DXVECTOR3& position, 
				const D3DXVECTOR3& focalPoint, 
				const D3DXVECTOR3& viewUp)
{
	float	length;

	m_focalPoint	= focalPoint;
	m_position		= position;
	m_viewUp		= viewUp;
	D3DXMatrixLookAtLH(&m_viewMatrix, &m_position, &m_focalPoint, &m_viewUp);
	D3DXVec3Subtract(&m_vpn, &m_focalPoint, &m_position);

	length	= D3DXVec3Length(&m_vpn);
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
CCamera::SetPerspectiveFov(float	fov,
						   float	aspect,
						   float	zNear,
						   float	zFar)
{
	D3DXMatrixPerspectiveFovLH(&m_projectionMatrix, D3DXToRadian(fov), aspect,
							   zNear, zFar);
}

//------------------------------------------------------------------------------
//	CCamera::Zoom
//------------------------------------------------------------------------------
void 
CCamera::Zoom(float amount)
{
	float	length;

	// Make sure user doesn't zoom past focal point
	length = D3DXVec3Length(&D3DXVECTOR3(m_focalPoint - m_position));
	if((length <= 0.01) && (amount > 0.0f))
		return;

	// Move the point
	m_position.x += m_vpn.x * amount;
	m_position.y += m_vpn.y * amount;
	m_position.z += m_vpn.z * amount;
	
	// Reproject
	D3DXMatrixLookAtLH(&m_viewMatrix, &m_position, &m_focalPoint, &m_viewUp);
}

//------------------------------------------------------------------------------
//	CCamera::Rotate
//------------------------------------------------------------------------------
void 
CCamera::Rotate(float				degrees, 
				const D3DXVECTOR3&	axis)
{
	D3DXMATRIX	rotate;

	if(degrees == 0.0f)
		return;

	MakeRotationMatrices(degrees, axis, m_viewMatrix, rotate);

	// Transform the position
	D3DXVec3TransformCoord(&m_position, &m_position, &m_viewMatrix);

	// Recalculate the view plane normal
	D3DXVec3Normalize(&m_vpn, &D3DXVECTOR3(m_focalPoint - m_position));

	// Transform the view up vector
	D3DXVec3TransformCoord(&m_viewUp, &m_viewUp, &rotate);

	// Reset the view matrix
	D3DXMatrixLookAtLH(&m_viewMatrix, &m_position, &m_focalPoint, &m_viewUp);

	UpdateLight();
}

//------------------------------------------------------------------------------
//	CCamera::RotateX
//------------------------------------------------------------------------------
void 
CCamera::RotateX(float degrees)
{
	D3DXVECTOR3	axis;

	D3DXVec3Cross(&axis, &m_vpn, &m_viewUp);
	Rotate(degrees, axis);
}

//------------------------------------------------------------------------------
//	CCamera::RotateY
//------------------------------------------------------------------------------
void 
CCamera::RotateY(float degrees)
{
	Rotate(degrees, m_viewUp);
}

//------------------------------------------------------------------------------
//	CCamera::RotateZ
//------------------------------------------------------------------------------
void 
CCamera::RotateZ(float degrees)
{
	Rotate(degrees, m_vpn);
}

//------------------------------------------------------------------------------
//	CCamera::EnableLight
//------------------------------------------------------------------------------
void 
CCamera::EnableLight(BOOL	turnOn,
					 BOOL	fixedLight)
{
	m_lightOn		= turnOn;
	m_fixedLight	= fixedLight;
	UpdateLight();
}

//------------------------------------------------------------------------------
//	CCamera::SetLight
//------------------------------------------------------------------------------
void 
CCamera::SetLight(DWORD				lightNum, 
				  const D3DLIGHT8&	lightParams)
{
	m_lightNum			= lightNum;

    m_light.Type		= D3DLIGHT_DIRECTIONAL;
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
CCamera::RotateLight(float				degrees, 
					 const D3DXVECTOR3&	axis)
{
	D3DXMATRIX	matrix;
	D3DXMATRIX	temp;

	if(degrees == 0.0f)
		return;

	MakeRotationMatrices(degrees, axis, matrix, temp);

	// Transform the position
	D3DXVec3TransformCoord(&m_lightPosition, &m_lightPosition, &matrix);

	UpdateLight();
}

//------------------------------------------------------------------------------
//	CCamera::RotateLightX
//------------------------------------------------------------------------------
void 
CCamera::RotateLightX(float degrees)
{
	D3DXVECTOR3	cross;

	D3DXVec3Cross(&cross, &m_vpn, &m_viewUp);
	RotateLight(degrees, cross);
}

//------------------------------------------------------------------------------
//	CCamera::RotateLightY
//------------------------------------------------------------------------------
void 
CCamera::RotateLightY(float degrees)
{
	RotateLight(degrees, m_viewUp);
}

//------------------------------------------------------------------------------
//	CCamera::SetDrawAxis
//------------------------------------------------------------------------------
void 
CCamera::SetDrawAxis(BOOL draw)
{
	m_drawAxis = draw;
}

//------------------------------------------------------------------------------
//	CCamera::SetHome
//------------------------------------------------------------------------------
void
CCamera::SetHome(void)
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
{
	D3DXVECTOR3	point;

	if(m_fixedLight)
	{
		m_lightPosition		= m_position;
		m_light.Direction	= m_vpn;
	}
	else
	{
		D3DXVECTOR3	direction;
		D3DXVec3Normalize(&direction, &D3DXVECTOR3(m_focalPoint - m_lightPosition));
		m_light.Direction = direction;
	}

	m_lightNormal[0].x = m_focalPoint.x;
	m_lightNormal[0].y = m_focalPoint.y;
	m_lightNormal[0].z = m_focalPoint.z;

	D3DXVec3Lerp(&point, &m_focalPoint, &m_lightPosition, 0.5);

	m_lightNormal[1].x = point.x;
	m_lightNormal[1].y = point.y;
	m_lightNormal[1].z = point.z;
}

//------------------------------------------------------------------------------
//	CCamera::MakeRotationMatrices
//------------------------------------------------------------------------------
void
CCamera::MakeRotationMatrices(float					degrees, 
							  const D3DXVECTOR3&	axis,
							  D3DXMATRIX&			transform,
							  D3DXMATRIX&			rotation)
{
	if(degrees == 0.0f)
	{
		D3DXMatrixIdentity(&transform);
		D3DXMatrixIdentity(&rotation);
		return;
	}

	D3DXMATRIX	temp;
	
	D3DXMatrixTranslation(&transform, -m_focalPoint.x, -m_focalPoint.y, 
						  -m_focalPoint.z);
	D3DXMatrixMultiply(&transform, &transform, 
					   D3DXMatrixRotationAxis(&rotation, &axis, 
											  D3DXToRadian(degrees)));
	D3DXMatrixMultiply(&transform, &transform, 
					  D3DXMatrixTranslation(&temp, m_focalPoint.x, 
											m_focalPoint.y, m_focalPoint.z));
}
