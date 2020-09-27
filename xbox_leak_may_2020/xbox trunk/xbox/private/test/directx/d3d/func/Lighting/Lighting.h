#ifndef __Lighting_h__
#define __Lighting_h__

const int	LT_LIGHTS = 2;


struct NDSVERTEX
{
	float	    x;
	float	    y;
	float	    z;
	float	    nx;
	float	    ny;
	float	    nz;
	D3DCOLOR	diffuse;
	D3DCOLOR	specular;
};

const int D3DFVF_NDSVERTEX = D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_DIFFUSE | D3DFVF_SPECULAR;


struct LightProperties
{
	D3DLIGHTTYPE	dwLightType;
	D3DCOLORVALUE   dcvDiffuse; 
	D3DCOLORVALUE   dcvSpecular;
	D3DCOLORVALUE   dcvAmbient; 
	D3DVECTOR		dvLightPosition;
	D3DVECTOR		dvLightDirection;
	float		    dvRange;
	float		    dvFalloff;
	float		    dvAttenuation0;
	float		    dvAttenuation1;
	float		    dvAttenuation2;
	float		    dvTheta;
	float		    dvPhi;

	bool			bLightActive;
	bool			bUseLight2;						// DX6 specific testing 
};

struct ModelProperties
{
	D3DMATERIALCOLORSOURCE		AmbientSource;		// DX7 specific testing
	D3DMATERIALCOLORSOURCE		DiffuseSource;		// DX7 specific testing
	D3DMATERIALCOLORSOURCE		SpecularSource;		// DX7 specific testing
	D3DMATERIALCOLORSOURCE		EmissiveSource;		// DX7 specific testing
	D3DMATERIALCOLORSOURCE		AlphaSource;		// DX7 specific testing
	D3DMATERIALCOLORSOURCE		FogFactorSource;	// DX7 specific testing
	D3DCOLORVALUE				dcvAmbientColor;
    D3DMATERIAL8				Material;

	bool						bLocalViewer;		// DX7 specific testing
	bool						bColorVertex;
	bool						bNormalizeNormals;
	
	bool						bAmbient;
	bool						bDiffuse;
	bool						bSpecular;
	bool						bEmissive;

	bool						bRed;
	bool						bGreen;
	bool						bBlue;
	bool						bAlpha;
};

struct SceneProperties
{
	D3DMATRIX		WorldMatrix;
	D3DMATRIX		ViewMatrix;
	D3DMATRIX		ProjectionMatrix;
};

class Lighting : public CD3DTest
{
public:
	Lighting();
	~Lighting();

	virtual UINT	TestInitialize(void);
	virtual bool	ClearFrame(void);
	virtual bool	ExecuteTest(UINT);
	virtual void	SceneRefresh(void);
	virtual bool	ProcessFrame(void);
	virtual bool	TestTerminate(void);

	virtual bool	SetDefaultMatrices(void);
	virtual bool	SetDefaultMaterials(void);
	virtual bool	SetDefaultLights(void);
	virtual bool	SetDefaultLightStates(void);
	virtual bool	SetDefaultRenderStates(void);

protected:
	NDSVERTEX*			m_pSourceVertices;
	D3DTLVERTEX*		m_pReferenceVertices;
	DWORD				m_nVertices;

#ifdef TESTTYPE_LIGHTING
	D3DCOLOR*			m_pResultVertices;
    NDSVERTEX*          m_pPointVertices;
    NDSVERTEX*          m_pTriangleVertices;
    DWORD               m_nPointVertices;
    DWORD               m_nTriangleVertices;
#else
	D3DTLVERTEX*		m_pResultVertices;
#endif

//	CMaterial*			m_pMaterial;
    D3DMATERIAL8        m_Material;
//	CLight*				m_pLight[LT_LIGHTS];
    D3DLIGHT8           m_Light[LT_LIGHTS];
	CVertexBuffer8*		m_pSourceVertexBuffer;
//	CVertexBuffer8*		m_pOptimizedSourceVertexBuffer;
//	CVertexBuffer8*		m_pDestinationVertexBuffer;
//    D3DDRAWPRIMITIVESTRIDEDDATA  m_sStridedVertexData;

	// Test properties
	DWORD				m_dwRows;
	DWORD				m_dwColumns;
	int					m_iStep;
	bool				m_bRasterize;
	int					m_iColorTollerance;
	DWORD				m_dwWidth;
	DWORD				m_dwHeight;
	int					m_iVertexNumber;
	DWORD				m_dwNumFailed;

	// Current test peoperties
	LightProperties		m_LightProperties[LT_LIGHTS];
	ModelProperties		m_ModelProperties;
	SceneProperties		m_SceneProperties;
	DWORD				m_dwCurrentTestNumber;

    // Static data
#ifdef TESTTYPE_LIGHTING
    static RENDERPRIMITIVEAPI   m_pDX6APIList[10];
    static RENDERPRIMITIVEAPI   m_pDX7APIList[8];
    static RENDERPRIMITIVEAPI   m_pDX8APIList[6];
#else
    static RENDERPRIMITIVEAPI   m_pDX6APIList[4];
    static RENDERPRIMITIVEAPI   m_pDX7APIList[6];
    static RENDERPRIMITIVEAPI   m_pDX8APIList[4];
#endif
    static DWORD                m_dwDX6APIListSize;
    static DWORD                m_dwDX7APIListSize;
    static DWORD                m_dwDX8APIListSize;

	// Helper functions
	virtual bool	GenerateVertices(void);
	virtual bool	InitializeVertexProcessing(void);
	virtual bool	ProcessSourceVertices(void);
	virtual bool	ProcessReferenceVertices7(void);
	virtual bool	ProcessReferenceVertices6(void);
	virtual bool	GetResultVertices(void);
//	virtual bool	CompareImages(CDirectDrawSurface *, CDirectDrawSurface *, CDirectDrawSurface *);
    virtual bool    CompareImages(void);
	virtual bool	GetCompareResult(float fColorVariation, float fColorRange=0.78f, int iMinPixels=1);

	// Properties functions
	virtual bool	ResolveTest7(UINT);
    virtual bool    ValidateTest7(int);
//	virtual bool	ResolveTest6(UINT);
	virtual bool	SetInitialProperties(void);
	virtual bool	SetSceneProperties(void);
	virtual bool	SetModelProperties(void);
	virtual bool	SetLightProperties(void);

private:

};


#endif // __Lighting_h__