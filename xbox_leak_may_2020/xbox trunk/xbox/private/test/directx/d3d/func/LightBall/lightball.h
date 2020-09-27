#ifndef __LightBall_h__

struct SPHEREVERTEX
{
	float   	x;
	float   	y;
	float   	z;
	D3DCOLOR	color;
	float   	tx;
	float   	ty;
	float   	tz;
};


#define LIGHTBALL_TEXTURESIZE	256



class LightBall : public CD3DTest
{
public:
	LightBall();
	~LightBall();

	virtual UINT	TestInitialize(void);
	virtual bool	ExecuteTest(UINT);
	virtual void	SceneRefresh(void);
	virtual bool	ProcessFrame(void);
	virtual bool	TestTerminate(void);

	virtual bool	SetDefaultMatrices(void);
	virtual bool	SetDefaultRenderStates(void);

private:
	CTexture8*			m_pImageTexture;
	CTexture8*			m_pBumpTexture;
	CTexture8*			m_pLuminanceTexture;
    CVertexBuffer8*     m_pSourceVertexBuffer;
    CVertexBuffer8*     m_pReferenceVertexBuffer;
    CVertexBuffer8*     m_pSrcDestVertexBuffer;
    CVertexBuffer8*     m_pRefDestVertexBuffer;
	D3DXMATRIX			m_mRotationMatrix;
	D3DXMATRIX			m_mModelMatrix;
	D3DXMATRIX			m_mViewMatrix;
	D3DXMATRIX			m_mProjectionMatrix;
	D3DXMATRIX			m_mProjectionTransform;
	D3DXMATRIX			m_mSphereTransform;

	static TLVERTEX	    m_pBackVertices[4];
	SPHEREVERTEX*		m_pSphereVertices;
	int					m_nSphereVertices;
	WORD*				m_pSphereIndices;
	int					m_nSphereIndices;		
	
	DWORD				m_dwCurrentTestNumber;
	DWORD				m_dwCurrentTransform;
	DWORD				m_dwCurrentScenario;
	
	DWORD				m_dwSameScenarioFrames;
	bool				m_bProjectedTexture;
	bool				m_bBumpMapping;
	bool				m_bLuminanceTexture;
    bool                m_bUseVertexBuffer;
	
	DWORD				m_dwNumFailed;
	DWORD				m_dwDriverStyle;

//	bool				FindRGBTextureFormat(void);
//	bool				FindBumpmapTextureFormat(void);
//	bool				FindLuminanceTextureFormat(void);
	bool				CheckProjectedTextures(void);

	bool				ResolveTestNumber(void);
	bool				SetCurrentTransform(void);
	bool				SetTextureTransform(void);

    bool                ProcessVertices(void);
    bool                ClearVertexBuffers(void);
};

#endif