#include "std.h"
#include "xapp.h"
#include "Node.h"
#include "Camera.h"
#include "Shape.h"
#include "Interpolator.h"

extern D3DXMATRIX g_matView;
extern D3DXMATRIX g_matPosition;

IMPLEMENT_NODE("Camera", CCamera, CNode)

START_NODE_PROPS(CCamera, CNode)
	NODE_PROP(pt_string, CCamera, mode)
	NODE_PROP(pt_node, CCamera, lookat)
END_NODE_PROPS()

CCamera theCamera;

CCamera::CCamera()
{
	Reset();
	m_mode = NULL;

#ifdef COOL_XDASH
	m_bNoisy = true;
#else
	m_bNoisy = false;
#endif
}

CCamera::~CCamera()
{
	delete [] m_mode;
}

void CCamera::Reset()
{
	m_nDeltaTime = 0.0f;
	m_position.x = 0.0f;
	m_position.y = 0.0f;
	m_position.z = 20.0f;

	D3DXQuaternionIdentity(&m_orientation);
}

void CCamera::UpdateViewMatrix()
{
	D3DXVECTOR3 position = m_position;
	D3DXQUATERNION orientation = m_orientation;

	if (m_bNoisy)
	{
		float t = (float) (XAppGetNow() / 4.0f);
		float x = sinf(t * D3DX_PI / 2.0f);
		float y = sinf(t * D3DX_PI);

		// Plus or minus 1 degree...
		x *= D3DX_PI / 180.0f;
		y *= D3DX_PI / 180.0f;

		D3DXQUATERNION q;
		D3DXQuaternionRotationYawPitchRoll(&q, x, y, 0.0f);
		D3DXQuaternionMultiply(&orientation, &q, &orientation);
	}

	m_actualPosition = position;
	D3DXMatrixAffineTransformation(&g_matPosition, 1.0f, NULL, &orientation, &position);
	D3DXMatrixInverse(&g_matView, NULL, &g_matPosition);
	g_matView._31 = -g_matView._31;
	g_matView._32 = -g_matView._32;
	g_matView._33 = -g_matView._33;
	g_matView._34 = -g_matView._34;
	XAppSetTransform(D3DTS_VIEW, &g_matView);
}

void CCamera::Set(const D3DXVECTOR3* pPosition, const D3DXQUATERNION* pOrientation, float nSeconds/*=0.0f*/)
{
//	TRACE(_T("CCamera::Set(%f,%f,%f)\n"), pPosition->x, pPosition->y, pPosition->z);

	m_newPosition = *pPosition;
	m_newOrientation = *pOrientation;

	m_oldPosition = m_position;
	m_oldOrientation = m_orientation;

	m_nStartTime = theApp.m_now;
	m_nDeltaTime = nSeconds;

	if (m_nDeltaTime <= 0.0f)
	{
		m_position = m_newPosition;
		m_orientation = m_newOrientation;
	}
}

void CCamera::Advance(float nSeconds)
{
	if (m_nDeltaTime > 0.0f)
	{
		float f = (float) (theApp.m_now - m_nStartTime) / m_nDeltaTime;
		if (f >= 1.0f)
		{
			m_nDeltaTime = 0.0f;
			f = 1.0f;
		}

		D3DXVec3Lerp(&m_position, &m_oldPosition, &m_newPosition, f);
		D3DXQuaternionSlerp(&m_orientation, &m_oldOrientation, &m_newOrientation, f);

//		TRACE(_T("CCamera::Advance(%f,%f,%f)\n"), m_position.x, m_position.y, m_position.z);
#ifndef _XBOX
		if (theApp.m_pNavigator != NULL)
		{
			theApp.m_pNavigator->m_position = m_position;
			theApp.m_pNavigator->m_orientation = m_orientation;
		}
#endif
	}
#ifndef _XBOX
	else if (theApp.m_pNavigator != NULL)
	{
		if (m_lookat != NULL && m_lookat->GetNodeClass() == NODE_CLASS(CShape))
		{
			m_position = theApp.m_pNavigator->m_position;
			D3DXVECTOR3 lookat = ((CShape*)m_lookat)->m_position;
			AdvanceWatch(nSeconds, lookat);
		}
		else
		{
			int nMode = 0;

			if (m_mode != NULL)
			{
				switch (m_mode[0])
				{
				case 'w':
					nMode = 1;
					break;

				case 'c':
					nMode = 2;
					break;
				}
			}

			switch (nMode)
			{
			case 0: // "inside looking forward" mode
				m_position = theApp.m_pNavigator->m_position;
				m_orientation = theApp.m_pNavigator->m_orientation;
				break;

			case 1: // "stationary watch cam" mode
				{
					bool bSlerp = true;
					D3DXVECTOR3 newPosition(m_position.x, m_position.y, m_position.z);

					float a = newPosition.x - theApp.m_pNavigator->m_position.x;
					float b = newPosition.z - theApp.m_pNavigator->m_position.z;
					float dist = sqrtf(a * a + b * b);
					if (dist > 50.0f)
					{
						float yaw = D3DX_PI / 2 - theApp.m_pNavigator->m_yaw;
						newPosition = theApp.m_pNavigator->m_position;
						dist = 5.0f + rnd(10.0f);
						newPosition.x -= cosf(yaw) * dist;
						newPosition.z -= sinf(yaw) * dist;

						yaw = theApp.m_pNavigator->m_yaw;
						dist = rnd(20.0f) - 10.0f;
						newPosition.x += cosf(yaw) * dist;
						newPosition.z += sinf(yaw) * dist;

						float groundHeight = 0.0f;
						if (theApp.m_pGround != NULL)
							groundHeight = theApp.m_pGround->GetGroundHeight(newPosition.x, newPosition.z);

						float y = groundHeight;
						float height = 2.0f + rnd(20.0f); // TODO: make this adjustable... (height of camera above the ground)

						newPosition.y = y + height;
						bSlerp = false;
					}

					D3DXMATRIX mat;
					D3DXVECTOR3 up(0.0f, 1.0f, 0.0f);
					D3DXMatrixLookAtLH(&mat, &newPosition, &theApp.m_pNavigator->m_position, &up);
					D3DXMatrixInverse(&mat, NULL, &mat);
					D3DXQUATERNION newOrientation;
					D3DXQuaternionRotationMatrix(&newOrientation, &mat);

	//				D3DXVec3Lerp(&m_position, &m_position, &newPosition, 0.05f);
					m_position = newPosition;
					if (bSlerp)
						D3DXQuaternionSlerp(&m_orientation, &m_orientation, &newOrientation, 0.1f);
					else
						m_orientation = newOrientation;
				}
				break;

			case 2: // "chase cam" mode
				AdvanceChase(nSeconds, theApp.m_pNavigator->m_position, theApp.m_pNavigator->m_yaw);
				break;
			}
		}
	}
#endif

	UpdateViewMatrix();
}


#ifndef _XBOX
void CCamera::AdvanceChase(float nSeconds, const D3DXVECTOR3& chasePosition, float yaw)
{
	D3DXVECTOR3 newPosition;

	float x = chasePosition.x;
	float y = chasePosition.y;
	float z = chasePosition.z;
	
	yaw = D3DX_PI / 2 - yaw;

	float dist = 5.0f;

	newPosition.x = x + cosf(yaw) * dist;
	newPosition.z = z + sinf(yaw) * dist;

	float groundHeight = 0.0f;
	if (theApp.m_pGround != NULL)
		groundHeight = theApp.m_pGround->GetGroundHeight(newPosition.x, newPosition.z);

	float height = 1.0f;

	if (y < groundHeight)
		y = groundHeight;

	newPosition.y = y + height;

	float h = sqrtf((chasePosition.x - newPosition.x) * (chasePosition.x - newPosition.x) +
		(chasePosition.z - newPosition.z) * (chasePosition.z - newPosition.z));
	float pitch = atan2f(chasePosition.y - newPosition.y, h);

	D3DXQUATERNION newOrientation;
	D3DXQuaternionRotationYawPitchRoll(&newOrientation, yaw, pitch, 0.0f);

	D3DXVec3Lerp(&m_position, &m_position, &newPosition, 0.25f);
	D3DXQuaternionSlerp(&m_orientation, &m_orientation, &newOrientation, 0.25f);
}

void CCamera::AdvanceWatch(float nSeconds, const D3DXVECTOR3& watchPosition)
{
	D3DXMATRIX mat;
	D3DXVECTOR3 up(0.0f, -1.0f, 0.0f);
	D3DXMatrixLookAtLH(&mat, &m_position, &watchPosition, &up);
	D3DXMatrixInverse(&mat, NULL, &mat);
	D3DXQUATERNION newOrientation;
	D3DXQuaternionRotationMatrix(&newOrientation, &mat);

	D3DXQuaternionSlerp(&m_orientation, &m_orientation, &newOrientation, 0.1f);
}
#endif


////////////////////////////////////////////////////////////////////////////

IMPLEMENT_NODE("CameraPath", CCameraPath, CNode)

START_NODE_PROPS(CCameraPath, CNode)
	NODE_PROP(pt_boolean, CCameraPath, isActive)
	NODE_PROP(pt_boolean, CCameraPath, backward)
	NODE_PROP(pt_number, CCameraPath, interval)
	NODE_PROP(pt_node, CCameraPath, position)
	NODE_PROP(pt_node, CCameraPath, orientation)
END_NODE_PROPS()

CCameraPath::CCameraPath() :
	m_isActive(false),
	m_backward(false),
	m_interval(1.0f),
	m_position(NULL),
	m_orientation(NULL)
{
}

CCameraPath::~CCameraPath()
{
	if (m_position != NULL)
		m_position->Release();

	if (m_orientation != NULL)
		m_orientation = NULL;
}

void CCameraPath::Advance(float nSeconds)
{
	CNode::Advance(nSeconds);

	if (!m_isActive)
		return;

	float t = (float) (XAppGetNow() - m_startTime) / m_interval;
	if (t >= 1.0f)
	{
		t = 1.0f;
		m_isActive = false;

		TRACE(_T("End of camera path\n"));
	}

	if (m_backward)
		t = 1.0f - t;

	D3DXVECTOR3 position = theCamera.m_position;
	D3DXQUATERNION orientation = theCamera.m_orientation;

	if (m_position != NULL)
	{
		position = ((CPositionInterpolator*)m_position)->Interpolate(t);
		position.z = -position.z;
	}

	if (m_orientation != NULL)
		orientation = ((COrientationInterpolator*)m_orientation)->Interpolate(t);
	
	theCamera.Set(&position, &orientation);

#ifndef _XBOX
	if (theApp.m_pNavigator != NULL)
	{
		theApp.m_pNavigator->m_position = position;
		theApp.m_pNavigator->m_orientation = orientation;
	}
#endif
}

bool CCameraPath::OnSetProperty(const PRD* pprd, const void* pvValue)
{
	if ((int)pprd->pbOffset == offsetof(m_isActive))
		m_startTime = XAppGetNow();

	return true;
}

void CCameraPath::Activate(bool bBackwards)
{
	m_isActive = true;
	m_startTime = XAppGetNow();
	m_backward = bBackwards;
}
