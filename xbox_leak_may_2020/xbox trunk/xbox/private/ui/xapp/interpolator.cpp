#include "std.h"
#include "xapp.h"
#include "Node.h"
#include "Interpolator.h"

IMPLEMENT_NODE("PositionInterpolator", CPositionInterpolator, CNode)

START_NODE_PROPS(CPositionInterpolator, CNode)
	NODE_PROP(pt_numarray, CPositionInterpolator, key)
	NODE_PROP(pt_vec3array, CPositionInterpolator, keyValue)
END_NODE_PROPS()

CPositionInterpolator::CPositionInterpolator()
{
}

CPositionInterpolator::~CPositionInterpolator()
{
}

D3DXVECTOR3 CPositionInterpolator::Interpolate(float key)
{
	D3DXVECTOR3 v;

//	if (m_key.m_nSize == 0)
	{
		ASSERT(m_keyValue.m_nSize > 0);

		float k = key * (m_keyValue.m_nSize - 1);
		int i = (int)floorf(k);
		float f = k - floorf(k);

		if (i < 0)
			return m_keyValue.m_value[0];
		
		if (i >= m_keyValue.m_nSize - 1)
			return m_keyValue.m_value[m_keyValue.m_nSize - 1];

		if (i > 0 && i < m_keyValue.m_nSize - 2 && m_keyValue.m_nSize >= 4)
		{
			D3DXVec3CatmullRom(&v, &m_keyValue.m_value[i - 1], &m_keyValue.m_value[i], &m_keyValue.m_value[i + 1], &m_keyValue.m_value[i + 2], f);
			return v;
		}

		D3DXVec3Lerp(&v, &m_keyValue.m_value[i], &m_keyValue.m_value[i + 1], f);
		return v;
	}
//	else
//	{
//		D3DXVECTOR3 v;
//		ASSERT(FALSE); // NYI!
//		return v;
//	}
}

////////////////////////////////////////////////////////////////////////////

IMPLEMENT_NODE("OrientationInterpolator", COrientationInterpolator, CNode)

START_NODE_PROPS(COrientationInterpolator, CNode)
	NODE_PROP(pt_numarray, COrientationInterpolator, key)
	NODE_PROP(pt_vec4array, COrientationInterpolator, keyValue)
END_NODE_PROPS()

COrientationInterpolator::COrientationInterpolator()
{
}

COrientationInterpolator::~COrientationInterpolator()
{
}

void COrientationInterpolator::OnLoad()
{
	CNode::OnLoad();

	for (int i = 0; i < m_keyValue.m_nSize; i += 1)
	{
		D3DXQUATERNION q;
		D3DXQuaternionRotationAxis(&q, (D3DXVECTOR3*)&m_keyValue.m_value[i], -m_keyValue.m_value[i].w);
		*((D3DXQUATERNION*)&m_keyValue.m_value[i]) = q;
	}
}

D3DXQUATERNION COrientationInterpolator::Interpolate(float key)
{
	D3DXQUATERNION v;

//	if (m_key.m_nSize == 0)
	{
		ASSERT(m_keyValue.m_nSize > 0);

		float k = key * (m_keyValue.m_nSize - 1);
		int i = (int)floorf(k);
		float f = k - floorf(k);

		if (i < 0)
			return *((D3DXQUATERNION*)&m_keyValue.m_value[0]);

		if (i >= m_keyValue.m_nSize - 1)
			return *((D3DXQUATERNION*)&m_keyValue.m_value[m_keyValue.m_nSize - 1]);
/*
		if (i > 0 && i < m_keyValue.m_nSize - 2 && m_keyValue.m_nSize >= 4)
		{
			D3DXQuaternionSquad(&v, (D3DXQUATERNION*)&m_keyValue.m_value[i - 1], (D3DXQUATERNION*)&m_keyValue.m_value[i], (D3DXQUATERNION*)&m_keyValue.m_value[i + 1], (D3DXQUATERNION*)&m_keyValue.m_value[i + 2], f);
			return v;
		}
*/
		D3DXQuaternionSlerp(&v, (D3DXQUATERNION*)&m_keyValue.m_value[i], (D3DXQUATERNION*)&m_keyValue.m_value[i + 1], f);
		return v;
	}
//	else
//	{
//		D3DXVECTOR4 v;
//		ASSERT(FALSE); // NYI!
//		return v;
//	}
}

