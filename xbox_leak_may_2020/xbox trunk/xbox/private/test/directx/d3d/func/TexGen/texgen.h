#ifndef __texgen_h__
#define __texgen_h__

//USETESTFRAME


struct SPHEREVERTEX
{
	D3DVALUE	x;
	D3DVALUE	y;
	D3DVALUE	z;
    D3DVALUE    k;
    D3DVALUE    nx;
    D3DVALUE    ny;
    D3DVALUE    nz;
	D3DCOLOR	color;
    D3DVALUE    tu0;
    D3DVALUE    tv0;
    D3DVALUE    tu1;
    D3DVALUE    tv1;
};


class TexGen : public CD3DTest
{
public:
	TexGen();
	~TexGen();

	virtual UINT	TestInitialize(void);
	virtual bool	ExecuteTest(UINT);
	virtual void	SceneRefresh(void);
	virtual bool	ProcessFrame(void);
	virtual bool	TestTerminate(void);

    virtual bool	SetDefaultMatrices(void);
	virtual bool	SetDefaultRenderStates(void);
	virtual bool    SetDefaultMaterials(void);
	virtual bool    SetDefaultLights(void);
	virtual bool    SetDefaultLightStates(void);

private:
	CTexture8*			m_pImageTexture;
//    CLight*             m_pLight;
//    CMaterial*          m_pMaterial;

	static D3DTLVERTEX	m_pBackVertices[4];
	SPHEREVERTEX*		m_pSphereVertices;
	SPHEREVERTEX*		m_pLocalVertices;
	SPHEREVERTEX*		m_pNonLocalVertices;
	int					m_nSphereVertices;
	WORD*				m_pSphereIndices;
	int					m_nSphereIndices;		

    static RENDERPRIMITIVEAPI   m_pAPIList[6];
    static DWORD                m_dwAPIListSize;

	DWORD				m_dwCurrentTestNumber;
    DWORD               m_dwSameTestNumber;
    DWORD               m_dwTestType;
    DWORD               m_dwTexgenType;
    LPSTR               m_pszTestName;

    bool                SetProperties(void);
    
	bool				CheckProjectedTextures(void);
};


#endif