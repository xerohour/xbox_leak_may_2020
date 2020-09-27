#ifndef __ClipStatus_h__
#define __ClipStatus_h__

#define CLIPSTATUS_LEFT     D3DCS_LEFT
#define CLIPSTATUS_RIGHT    D3DCS_RIGHT
#define CLIPSTATUS_TOP      D3DCS_TOP
#define CLIPSTATUS_BOTTOM   D3DCS_BOTTOM
#define CLIPSTATUS_FRONT    D3DCS_FRONT
#define CLIPSTATUS_BACK     D3DCS_BACK
#define CLIPSTATUS_LEFT     D3DCS_LEFT
#define CLIPSTATUS_ALL      D3DCS_ALL

struct CSVERTEX
{
	D3DVALUE	x;
	D3DVALUE	y;
	D3DVALUE	z;
    D3DVALUE    nx;
    D3DVALUE    ny;
    D3DVALUE    nz;
	D3DCOLOR	c;

    CSVERTEX() {}
    CSVERTEX(float _x, float _y, float _z, char _c) 
    { 
        x = _x; y = _y; z = _z; 
        nx = 0.f; ny = 0.f; nz = -1.f;
        if (_c == 'r')          c = 0x00ff0000; 
        else if (_c == 'g')     c = 0x0000ff00;
        else if (_c == 'b')     c = 0x000000ff;
        else if (_c == 'w')     c = 0x00ffffff;
        else                    c = 0x00f0f0f0;
    }
};


struct CSTVERTEX
{
	float	    x;
	float	    y;
	float	    z;
    float       rhw;
	D3DCOLOR	c;
};


struct TEST
{
    CSVERTEX  v[3];
    DWORD     dwClipUnion;
    DWORD     dwClipIntersection;

    TEST() {}
    TEST(CSVERTEX &v0, CSVERTEX &v1, CSVERTEX &v2) 
    { 
        v[0] = v0; v[1] = v1; v[2] = v2; 
    }
};

class ClipStatus : public CD3DTest
{
public:
	ClipStatus();
	~ClipStatus();

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
    CVertexBuffer8*  m_pSrcSrcVB;
//    CVertexBuffer8*  m_pSrcRefVB;
//    CVertexBuffer8*  m_pDstSrcVB;
//    CMaterial*      m_pMaterial;
//    CLight*         m_pLight;

    DWORD           m_dwCurrentTestNumber;
    DWORD           m_dwTableTestNumber;
    bool            m_bOptimize;
    bool            m_bLighting;
    DWORD           m_dwTotalTableTests;
    DWORD           m_dwTotalTests;

    static TEST     Test[22];
};

#endif