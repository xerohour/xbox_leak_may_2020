extern LPDIRECT3DTEXTURE8 LoadTexture(const TCHAR* szURL, UINT width, UINT height);
extern LPDIRECT3DTEXTURE8 ParseTexture(const TCHAR* szURL, const BYTE* pbContent, int cbContent, UINT width=0, UINT height=0);

class CTexture : public CNode
{
	DECLARE_NODE(CTexture, CNode);
public:
	CTexture();
	~CTexture();

	bool m_repeatS;
	bool m_repeatT;

	D3DFORMAT m_format;
	LPDIRECT3DTEXTURE8 m_pSurface;
	int m_nImageWidth;
	int m_nImageHeight;

	virtual bool Create(int nWidth, int nHeight);

	LPDIRECT3DTEXTURE8 GetTextureSurface();

	DECLARE_NODE_PROPS()
};


class CImageTexture : public CTexture
{
	DECLARE_NODE(CImageTexture, CTexture);
public:
	CImageTexture();
	~CImageTexture();

	TCHAR* m_url;
	bool m_alpha;

	LPDIRECT3DTEXTURE8 GetTextureSurface();
	bool OnSetProperty(const PRD* pprd, const void* pvValue);

	void Load(const TCHAR* szURL);
	bool m_bDirty;

	DECLARE_NODE_PROPS()
};
