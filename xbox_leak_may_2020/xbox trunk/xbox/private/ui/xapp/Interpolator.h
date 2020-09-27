#pragma once

class CPositionInterpolator : public CNode
{
	DECLARE_NODE(CPositionInterpolator, CNode)
public:
	CPositionInterpolator();
	~CPositionInterpolator();

	CNumArray m_key;
	CVec3Array m_keyValue;

	D3DXVECTOR3 Interpolate(float key);

	DECLARE_NODE_PROPS()
};

class COrientationInterpolator : public CNode
{
	DECLARE_NODE(COrientationInterpolator, CNode)
public:
	COrientationInterpolator();
	~COrientationInterpolator();

	CNumArray m_key;
	CVec4Array m_keyValue;

	D3DXQUATERNION Interpolate(float key);
	void OnLoad();

	DECLARE_NODE_PROPS()
};

