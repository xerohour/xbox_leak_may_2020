#ifndef __Clipping_h__
#define __Clipping_h__

#define     CP_GOURAUD  0
#define     CP_FLAT     1
#define     CP_SPECULAR 2
#define     CP_ALPHA    3
#define     CP_FOG      4
#define     CP_TEXTURE  5
#define     CP_MULTITEX 6



class WireClip : public CD3DTest
{
public:
    WireClip();
	~WireClip();

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

    virtual bool    ResolveTestNumber(void);
    virtual bool    SetWorldMatrix(void);
    virtual bool    SetProjectionMatrix(void);
    virtual bool    SetVertices(void);
    virtual bool    SetProperties(void);
    virtual bool    SetUserClipPlanes(void);
};

class WTriangle : public WireClip
{
public:
    WTriangle();
    ~WTriangle();

  	virtual UINT TestInitialize(void);
	virtual bool TestTerminate(void);

private:
    DWORD               m_dwCurrentTriangle;
    static D3DVERTEX    m_pTriangleVertices[24];
    DWORD               m_dwTotalAPIs;

    virtual bool    ResolveTestNumber(void);

};


class WTFan : public WireClip
{
public:
    WTFan();
    ~WTFan();

  	virtual UINT TestInitialize(void);
	virtual bool TestTerminate(void);

private:
    DWORD               m_dwTotalAPIs;

    virtual bool    ResolveTestNumber(void);
    virtual bool    SetWorldMatrix(void);
    bool            GenerateTriangleFan(void);

};

class WTStrip : public WireClip
{
public:
    WTStrip();
    ~WTStrip();

  	virtual UINT TestInitialize(void);
	virtual bool TestTerminate(void);

private:
    DWORD               m_dwTotalAPIs;

    virtual bool    ResolveTestNumber(void);
    virtual bool    SetWorldMatrix(void);
    bool            GenerateTriangleStrip(void);

};

class WLine : public WireClip
{
public:
    WLine();
    ~WLine();

  	virtual UINT TestInitialize(void);
	virtual bool TestTerminate(void);

private:
    DWORD               m_dwCurrentLine;
    static D3DVERTEX    m_pLineVertices[10];
    DWORD               m_dwTotalAPIs;

    virtual bool    ResolveTestNumber(void);
};

class WLStrip : public WireClip
{
public:
    WLStrip();
    ~WLStrip();

  	virtual UINT TestInitialize(void);
	virtual bool TestTerminate(void);

private:
    DWORD               m_dwTotalAPIs;

    virtual bool    ResolveTestNumber(void);
    virtual bool    SetWorldMatrix(void);
    bool            GenerateLineStrip(void);
};

class WPoint : public WireClip
{
public:
    WPoint();
    ~WPoint();

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
