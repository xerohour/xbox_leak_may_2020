class CNavigationInfo : public CNode
{
	DECLARE_NODE(CNavigationInfo, CNode)
public:
	CNavigationInfo();
	~CNavigationInfo();

	bool OnSetProperty(const PRD* pprd, const void* pvValue);
	void OnLoad();

	bool m_isBound;
	D3DXVECTOR3 m_avatarSize;
	bool m_headlight;
	float m_speed;
	TCHAR* m_type;
	float m_visibilityLimit;
	CNode* m_shape;

	void Bind();

	DECLARE_NODE_PROPS()
};
