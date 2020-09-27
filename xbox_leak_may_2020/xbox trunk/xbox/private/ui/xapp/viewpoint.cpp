#include "std.h"
#include "xapp.h"
#include "Node.h"
#include "Camera.h"
#include "Viewpoint.h"

IMPLEMENT_NODE("Viewpoint", CViewpoint, CNode)

START_NODE_PROPS(CViewpoint, CNode)
	NODE_PROP(pt_number, CViewpoint, fieldOfView)
	NODE_PROP(pt_boolean, CViewpoint, jump)
	NODE_PROP(pt_boolean, CViewpoint, isBound)
	NODE_PROP(pt_vec3, CViewpoint, position)
	NODE_PROP(pt_vec4, CViewpoint, orientation)
	NODE_PROP(pt_string, CViewpoint, description)
END_NODE_PROPS()

CViewpoint::CViewpoint() :
	m_isBound(false),
	m_fieldOfView(0.785398f),
	m_jump(true),
	m_orientation(0.0f, 0.0f, 1.0f, 0.0f),
	m_position(0.0f, 0.0f, 10.0f),
	m_description(NULL)
{
}

CViewpoint::~CViewpoint()
{
	delete [] m_description;

	if (theApp.m_pViewpoint == this)
		theApp.m_pViewpoint = NULL;
}

void CViewpoint::OnLoad()
{
	CNode::OnLoad();

	if (theApp.m_pViewpoint == NULL)
		Bind();
}

void BindViewpoint(CNode* pViewpointNode)
{
	if (pViewpointNode == NULL || !pViewpointNode->IsKindOf(NODE_CLASS(CViewpoint)))
		return;

	((CViewpoint*)pViewpointNode)->Bind();
}

void CViewpoint::Bind()
{
	if (theApp.m_pViewpoint == this)
		return;

	if (theApp.m_pViewpoint != NULL)
		theApp.m_pViewpoint->m_isBound = false;

	theApp.m_pViewpoint = this;

	D3DXQUATERNION q;
	D3DXQuaternionRotationAxis(&q, (D3DXVECTOR3*)&m_orientation, -m_orientation.w);

	D3DXVECTOR3 position = m_position;
	position.z = -position.z;

	theCamera.Set(&position, &q, m_jump ? 0.0f : 1.0f);

#ifndef _XBOX
	if (theApp.m_pNavigator != NULL)
	{
		theApp.m_pNavigator->m_position = position;
		theApp.m_pNavigator->m_orientation = q;
	}
#endif

	theApp.m_bProjectionDirty = true;
}

bool CViewpoint::OnSetProperty(const PRD* pprd, const void* pvValue)
{
	if ((int)pprd->pbOffset == offsetof(m_isBound))
	{
		if (*(bool*)pvValue)
			Bind();
	}
	else if ((int)pprd->pbOffset == offsetof(m_fieldOfView))
	{
		theApp.m_bProjectionDirty = true;
	}

	return true;
}
