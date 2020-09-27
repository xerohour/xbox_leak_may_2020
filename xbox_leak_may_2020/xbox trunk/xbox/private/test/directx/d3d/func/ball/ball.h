/*******************************************************************************

Copyright (c) 2000 Microsoft Corporation.  All rights reserved.

File Name:

    ball.h

*******************************************************************************/

#ifndef __Ball_h__
#define __Ball_h__

struct D3DVERTEXB1
{
    D3DVALUE    x, y, z;
    D3DVALUE    k1;
    D3DVALUE    nx, ny, nz;
    D3DVALUE    tu, tv;
};

struct D3DVERTEXB2
{
    D3DVALUE    x, y, z;
    D3DVALUE    k1, k2;
    D3DVALUE    nx, ny, nz;
    D3DVALUE    tu, tv;
};

struct D3DVERTEXB3
{
    D3DVALUE    x, y, z;
    D3DVALUE    k1, k2, k3;
    D3DVALUE    nx, ny, nz;
    D3DVALUE    tu, tv;
};

const int D3DFVF_VERTEXB1 = D3DFVF_XYZB1 | D3DFVF_NORMAL | D3DFVF_TEX1; 
const int D3DFVF_VERTEXB2 = D3DFVF_XYZB2 | D3DFVF_NORMAL | D3DFVF_TEX1; 
const int D3DFVF_VERTEXB3 = D3DFVF_XYZB3 | D3DFVF_NORMAL | D3DFVF_TEX1; 

class Ball : public CD3DTest
{
public:
    Ball();
    ~Ball();

	virtual UINT TestInitialize(void);
	virtual bool ExecuteTest(UINT);
	virtual void SceneRefresh(void);
	virtual bool ProcessFrame(void);
    virtual void TestCompleted(void);
    virtual bool TestTerminate(void);

	virtual bool SetDefaultMatrices(void);
	virtual bool SetDefaultMaterials(void);
	virtual bool SetDefaultLightStates(void);
	virtual bool SetDefaultRenderStates(void);
	virtual bool SetDefaultLights(void);

private:
//	CMaterial*		m_pMaterial;
//	CLight*			m_pLight;
	CTexture8*		m_pChainTexture;
	CTexture8*		m_pLogoTexture;
	CTexture8*		m_pWallTexture;

    D3DVERTEXB1*    m_pSphereVerticesB1;
    DWORD           m_dwSphereVertices;
    WORD*           m_pSphereIndices;
    DWORD           m_dwSphereIndices;

    D3DVERTEXB1*    m_pBoxVerticesB1;
    D3DVERTEXB2*    m_pBoxVerticesB2;
    D3DVERTEXB3*    m_pBoxVerticesB3;
    DWORD           m_dwBoxVertices;

    D3DVERTEXB1*    m_pVerticesB1;
    D3DVERTEXB2*    m_pVerticesB2;
    D3DVERTEXB3*    m_pVerticesB3;

    D3DTLVERTEX*    m_pWallVertices;

    DWORD           m_dwTranslations;
    DWORD           m_dwMatrixModes;
    DWORD           m_dwRasterModes;
    DWORD           m_dwTotalTests;
    
    DWORD           m_dwCurrentTestNumber;
    DWORD           m_dwCurrentTranslation;
    DWORD           m_dwCurrentMatrixMode;
    DWORD           m_dwCurrentRasterMode;
    DWORD           m_dwNumFailed;

    D3DMATRIX       GenerateSphereModelMatrix(DWORD dwFrame, DWORD nMatrix);
    D3DMATRIX       GenerateBoxModelMatrix(DWORD dwFrame, DWORD nMatrix);

};

#endif
