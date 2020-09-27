#pragma once

class CViewpoint : public CNode
{
	DECLARE_NODE(CViewpoint, CNode)
public:
	CViewpoint();
	~CViewpoint();

	void OnLoad();
	void Bind();

	bool m_isBound;
	float m_fieldOfView;
	bool m_jump;
	D3DXQUATERNION m_orientation;
	D3DXVECTOR3 m_position;
	TCHAR* m_description;

	bool OnSetProperty(const PRD* pprd, const void* pvValue);

	DECLARE_NODE_PROPS()
};

extern void BindViewpoint(CNode* pViewpontNode);
