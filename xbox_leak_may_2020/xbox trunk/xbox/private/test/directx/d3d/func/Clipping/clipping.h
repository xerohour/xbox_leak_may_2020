#ifndef __Clipping_h__
#define __Clipping_h__

#define     CP_GOURAUD  0
#define     CP_FLAT     1
#define     CP_SPECULAR 2
#define     CP_ALPHA    3
#define     CP_FOG      4
#define     CP_TEXTURE  5
#define     CP_MULTITEX 6



class Clipping : public CD3DTest
{
public:
    Clipping();
	~Clipping();

	virtual UINT TestInitialize(void);
	virtual bool ClearFrame(void);
	virtual bool ExecuteTest(UINT);
	virtual void SceneRefresh(void);
	virtual bool ProcessFrame(void);
	virtual bool TestTerminate(void);

	virtual bool SetupViewport(void);
	virtual bool SetDefaultMatrices(void);
	virtual bool SetDefaultMaterials(void);
	virtual bool SetDefaultLightStates(void);
	virtual bool SetDefaultRenderStates(void);
	virtual bool SetDefaultLights(void);

protected:
//	CMaterial*		m_pMaterial;
//	CLight*			m_pLight;
	CTexture8*		m_pTextureArray[8];
    DWORD           m_dwMaxTextures;

    // Test numbers
    DWORD           m_dwTotalTests;
    DWORD           m_dwRotations;
    
    // Current settings
    DWORD           m_dwCurrentTestNumber;
    DWORD           m_dwCurrentRotation;
    DWORD           m_dwCurrentZClip;
    DWORD           m_dwCurrentAPI;
    DWORD           m_dwCurrentFVF;
    D3DPRIMITIVETYPE m_dwCurrentPrimitiveType;
    DWORD           m_dwCurrentProperties;

    LPSTR           m_pszCurrentProperties;

    LPD3DVERTEX     m_pInputVertices;
    BYTE*           m_pRenderVertices;
    DWORD           m_dwVertices;

    static RENDERPRIMITIVEAPI       m_pVBAPIList[2];
    static DWORD                    m_dwVBAPIListSize;

    virtual bool    ResolveTestNumber(void);
    virtual bool    SetWorldMatrix(void);
    virtual bool    SetProjectionMatrix(void);
    virtual bool    SetVertices(void);
    virtual bool    SetProperties(void);
    virtual bool    SetUserClipPlanes(void);
};

class Triangle : public Clipping
{
public:
    Triangle();
    ~Triangle();

  	virtual UINT TestInitialize(void);
	virtual bool TestTerminate(void);

private:
    DWORD               m_dwCurrentTriangle;
    static D3DVERTEX    m_pTriangleVertices[24];
    DWORD               m_dwTotalAPIs;

    virtual bool    ResolveTestNumber(void);

};


class TFan : public Clipping
{
public:
    TFan();
    ~TFan();

  	virtual UINT TestInitialize(void);
	virtual bool TestTerminate(void);

private:
    DWORD               m_dwTotalAPIs;

    virtual bool    ResolveTestNumber(void);
    virtual bool    SetWorldMatrix(void);
    bool            GenerateTriangleFan(void);

};

class TStrip : public Clipping
{
public:
    TStrip();
    ~TStrip();

  	virtual UINT TestInitialize(void);
	virtual bool TestTerminate(void);

private:
    DWORD               m_dwTotalAPIs;

    virtual bool    ResolveTestNumber(void);
    virtual bool    SetWorldMatrix(void);
    bool            GenerateTriangleStrip(void);

};

class Line : public Clipping
{
public:
    Line();
    ~Line();

  	virtual UINT TestInitialize(void);
	virtual bool TestTerminate(void);

private:
    DWORD               m_dwCurrentLine;
    static D3DVERTEX    m_pLineVertices[10];
    DWORD               m_dwTotalAPIs;

    virtual bool    ResolveTestNumber(void);
};

class LStrip : public Clipping
{
public:
    LStrip();
    ~LStrip();

  	virtual UINT TestInitialize(void);
	virtual bool TestTerminate(void);

private:
    DWORD               m_dwTotalAPIs;

    virtual bool    ResolveTestNumber(void);
    virtual bool    SetWorldMatrix(void);
    bool            GenerateLineStrip(void);
};

class Point : public Clipping
{
public:
    Point();
    ~Point();

  	virtual UINT TestInitialize(void);
	virtual bool TestTerminate(void);

private:
    DWORD                       m_dwTotalAPIs;

    static RENDERPRIMITIVEAPI   m_pDX6APIList[7];
    static RENDERPRIMITIVEAPI   m_pDX7APIList[7];
    static RENDERPRIMITIVEAPI   m_pDX8APIList[2];
    static DWORD                m_dwDX6APIListSize;
    static DWORD                m_dwDX7APIListSize;
    static DWORD                m_dwDX8APIListSize;

    virtual bool    ResolveTestNumber(void);
    bool            GeneratePointList(void);
};

#endif __Clipping_h__
