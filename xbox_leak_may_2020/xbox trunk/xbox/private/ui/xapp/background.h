class CBackground : public CNode
{
	DECLARE_NODE(CBackground, CNode)
public:
	CBackground();
	~CBackground();

	void Advance(float nSeconds);
	bool OnSetProperty(const PRD* pprd, const void* pvValue);

	void Bind();
	void RenderBackdrop();

	CNode* m_backdrop;
	D3DXVECTOR3 m_skyColor;
	bool m_isBound;

	LPDIRECT3DVERTEXBUFFER8 m_pVB;

	DECLARE_NODE_PROPS()
};

