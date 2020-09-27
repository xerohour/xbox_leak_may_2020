#ifndef __VSLanguage_h__
#define __VSLanguage_h__


//USETESTFRAME

#define     MAXSHADERS  200

typedef struct _VERTEX0
{
    float   x, y, z;
    float   nx, ny, nz;
    float   tu, tv;
} VERTEX0, *LPVERTEX0;

typedef struct _VERTEX1
{
	float	coord[4];
	DWORD	normal;
} VERTEX1, LPVERTEX1;

typedef struct _VERTEX2
{
	short	coord[4];
	DWORD	normal;
	short	tex[2];
} VERTEX2, *LPVERTEX2;


typedef struct _SHADER
{
	DWORD	dwHandle;
	DWORD	dwVersion;
	bool	bSupported;
} SHADER;



#define FVF_VERTEX0 ( D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1 )
#define D3DVS_OP    0x80000000

class VShader: public CD3DTest
{
public:
    VShader();
    ~VShader();

    virtual UINT TestInitialize(void);
    virtual bool ExecuteTest(UINT);
    virtual void SceneRefresh(void);
    virtual bool ProcessFrame(void);
    virtual bool TestTerminate(void);

    virtual BOOL AbortedExit(void);

protected:
    
    SHADER			m_pShaders[MAXSHADERS];
	DWORD           m_dwShaderVersion;
    DWORD			m_dwShaders;
	CVertexBuffer8  *m_pSrcVB;
	CVertexBuffer8  *m_pRefVB;
	CVertexBuffer8  *m_pSrcVB1;
	CVertexBuffer8  *m_pRefVB1;
	CVertexBuffer8  *m_pSrcVB2;
	CVertexBuffer8  *m_pRefVB2;
	CVertexBuffer8  *m_pSrcVB3;
	CVertexBuffer8  *m_pRefVB3;
	CIndexBuffer8   *m_pSrcIB;
	CIndexBuffer8   *m_pRefIB;
	DWORD           m_dwVertices;
	DWORD           m_dwIndices;
	DWORD           m_dwMaxStreams;

	CTexture8    	*m_pTexture;

    BOOL            m_bExit;

    DWORD			m_dwTotalTests;
    DWORD			m_dwCurrentTestNumber;
    DWORD			m_dwCurrentShader;
    
    virtual bool    CreateShaders(void);
    virtual bool    SetCurrentShaderConstants(void);
    virtual void    ReleaseShaders(void);

    virtual void    ProcessInput();
    virtual BOOL    WndProc(LRESULT* plr, HWND hWnd, UINT uMsg, 
                        WPARAM wParam, LPARAM lParam);
};

#define     MAXSHADERSIZE   (128*4)
#define     MAXSHADERPARTS  100

struct VShaderParts
{
    DWORD   dwCode[MAXSHADERSIZE];
    DWORD   dwSize;
	DWORD	dwVersion;
};


class Fixed : public VShader
{
public:
    Fixed();
    ~Fixed();

    virtual UINT TestInitialize(void);

protected:
    VShaderParts    m_vspTransform[MAXSHADERPARTS];
    VShaderParts    m_vspPointSize[MAXSHADERPARTS];
    VShaderParts    m_vspFog[MAXSHADERPARTS];

    VShaderParts    m_vspLighting[MAXSHADERPARTS];
    VShaderParts    m_vspTextures[MAXSHADERPARTS];


    DWORD           m_dwTransformParts;
    DWORD           m_dwLightingParts;
    DWORD           m_dwTextureParts;
    
    virtual bool    CreateShaders(void);
    virtual bool    SetCurrentShaderConstants(void);
    
    bool            CreateShaderParts();
    bool            WriteDeclaration(DWORD *pdwDecl);

};

#endif
