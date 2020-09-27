#ifndef __LineTex_h__
#define __LineTex_h__

//#include "CD3DTest.h"

//USETESTFRAME

class PointTex : public CD3DTest
{
public:
    PointTex();
    ~PointTex();

    virtual BOOL Create(CDisplay* pDisplay);

	virtual UINT TestInitialize(void);
	virtual bool ExecuteTest(UINT);
	virtual void SceneRefresh(void);
	virtual bool ProcessFrame(void);
    virtual bool TestTerminate(void);

	virtual bool SetDefaultMatrices(void);
	virtual bool SetDefaultMaterials(void);
	virtual bool SetDefaultLightStates(void);
	virtual bool SetDefaultRenderStates(void);
	virtual bool SetDefaultLights(void);

private:
//	CMaterial*		m_pMaterial;
//	CLight*			m_pLight;
	CTexture8*		m_pTextureArray[8];

    CVertexBuffer8* m_pd3dr;
    D3DVERTEX*      m_pVertices;
    DWORD           m_dwVertices;

    DWORD           m_dwCurrentTestNumber;
    DWORD           m_dwNumFailed;

    bool            GeneratePointList(void);

};

#endif // __LineTex_h__