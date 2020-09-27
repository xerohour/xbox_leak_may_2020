#ifndef __Texform_h__
#define __Texform_h__


//USETESTFRAME


class Texform : public CD3DTest
{
public:
	Texform();
	~Texform();

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
//	CMaterial*      m_pMaterial;
//    CLight*         m_pLight;
    CShapes*        m_pShape;
    CTexture8*      m_pTexture;
    void*           m_pVertices;

    DWORD           m_dwCurrentTestNumber;
    LPSTR           m_pszTestName;

    DWORD           m_dwFVF;
    DWORD           m_dwInputSize;
    DWORD           m_dwOutputSize;

    bool            SetProperties(void);
    void            SetVertices(void);
};


#endif