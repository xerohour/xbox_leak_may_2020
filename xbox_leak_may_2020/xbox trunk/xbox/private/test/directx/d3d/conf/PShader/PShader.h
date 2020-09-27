// FILE:        pshader.h
// DESC:        pixel shader class header for pixel shader conformance tests
// AUTHOR:      Cliff Hudson

#ifndef _PSHADER_H_
#define _PSHADER_H_

//#include <d3d8.h>
//#include <d3dx8.h>
//#include <ddraw.h>
//#include "TestFrameBase.h"
//#include "CD3DTest.h"
//#include "CShapes.h"
//#include <vector>

//USETESTFRAME

#define DEFAULT_PSVERSION (DWORD)D3DPS_VERSION(1,0)
#define PSVERSION(a, b) ((DWORD)D3DPS_VERSION((a), (b)))
#define VERSIONMASK(_version) (_version & 0x0000FFFF)
/*
typedef struct _MYTLVERTEX 
{
    float x, y, z, w;
    DWORD a, b;
    float u, v;
} MYTLVERTEX, *PMYTLVERTEX;
*/
struct MYTLVERTEX 
{
    float x, y, z, w;
    DWORD a, b;
    float u, v;
};

#define MYTLVERTEX_FVF (D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_SPECULAR | D3DFVF_TEX1)

typedef struct _MYCUBETLVERTEX2
{
    float x, y, z, w;
    DWORD a, b;
    float u0, v0;
    float u1, v1, w1;
    float u2, v2, w2; 
} MYCUBETLVERTEX2, * PMYCUBETLVERTEX2;

#define MYCUBETLVERTEX2_FVF (D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_SPECULAR | D3DFVF_TEX3 | D3DFVF_TEXCOORDSIZE2(0) | D3DFVF_TEXCOORDSIZE3(1) | D3DFVF_TEXCOORDSIZE3(2))

typedef struct _MYCUBETLVERTEX3
{
    float x, y, z, w;
    DWORD a, b;
    float u0, v0;
    float u1, v1, w1; 
    float u2, v2, w2; 
    float u3, v3, w3; 
} MYCUBETLVERTEX3, * PMYCUBETLVERTEX3;

#define MYCUBETLVERTEX3_FVF (D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_SPECULAR | D3DFVF_TEX4 | D3DFVF_TEXCOORDSIZE2(0) | D3DFVF_TEXCOORDSIZE3(1) | D3DFVF_TEXCOORDSIZE3(2) | D3DFVF_TEXCOORDSIZE3(3))

//typedef std::vector<CTexture *> TEXTUREVECTOR;

typedef struct
{
    char *pName;                   // pixel shader name
    char *pCode;                   // pixel shader code (ASCII)
    DWORD dwHandle;                // pixel shader handle
//    std::vector<float> vConstants; // constant vector
    float fConstants[256];
    UINT uNumConstants;
    DWORD dwConstantsStart;        // constant start
    DWORD dwRequired;              // base pixel shader version
    CVertexBuffer8 *pSrcVB;         // src vertex buffer
    CVertexBuffer8 *pRefVB;         // ref vertex buffer
    DWORD dwVertexFormat;          // vertex format
    DWORD dwVertexFormatSize;      // vertex format size
//    TEXTUREVECTOR vTextures;       // texture vector
    CTexture8* pd3dt[8];
    UINT uNumTextures;
    bool bAlpha;                   // alpha blend flag
    bool bSetup;                   // setup flag
} MYSHADER, * PMYSHADER;

//typedef std::vector<MYSHADER> SHADERVECTOR;

class CPShader : public TESTFRAME(CD3DTest)
{
    public:
        CPShader();
        virtual UINT TestInitialize(void);
        virtual bool TestTerminate(void);
        virtual void CommandLineHelp(void);
        virtual bool ProcessFrame(void);
        virtual bool ExecuteTest(UINT);
        virtual void SceneRefresh(void);

        virtual BOOL    AbortedExit(void);

    protected:

        virtual void    ProcessInput();
        virtual BOOL    WndProc(LRESULT* plr, HWND hWnd, UINT uMsg, 
                            WPARAM wParam, LPARAM lParam);

    protected:
        bool DoCapsCheck();
        virtual bool LoadTextures() = 0;
        virtual void PopulateShaderInfo() = 0;

        void AddShaderDef(char *pName, char *pCode, DWORD dwFVF, void *pVertices);
        void AddShaderDef(char *pName, char *pCode, DWORD dwFVF, void *pVertices, bool bAlpha);
        void AddShaderDef(char *pName, char *pCode, DWORD dwFVF, void *pVertices, DWORD dwRequired);
        void AddShaderDef(char *pName, char *pCode, DWORD dwFVF, void *pVertices, bool bAlpha, DWORD dwRequired);

        void AddTextureToLastShaderDef(CTexture8 * pTexture);
        void SetConstantsStart(DWORD dwStart);
        void AddConstantsToLastShaderDef(float a, float b, float c, float d);
        void AddConstantsToLastShaderDef(float f[4]);
        void AddConstantsToLastShaderDef(float *f, DWORD dwCount);

        bool UpdateCubicTexture8(CTexture8 * pSrcTexture, CTexture8 * pTargetTexture);
		CTexture8*		GenerateBrickTexture(void);

        void VerifyTexture(CTexture8* pd3dt);

    private:
        bool SetupBasicScene(UINT);
        bool CreateLoadedVertexBuffers(DWORD dwFVFType, void *pData, CVertexBuffer8 **ppSource, CVertexBuffer8 **ppRef);

    protected:
//        SHADERVECTOR m_vShaders;
        MYSHADER m_Shaders[64];
        UINT m_uNumShaders;
        DWORD m_dwSupported;
        int m_nPass;
        int m_nFail;
        bool m_bSkipAllForCaps;
		DWORD			m_dwCurrentTestNumber;

        bool m_bStretch;

        BOOL            m_bExit;
};

class CPSOps : public CPShader
{
    public:
        CPSOps();

        UINT TestInitialize(void);
        bool ExecuteTest(UINT);
        bool TestTerminate(void);
        void CommandLineHelp(void);

    protected:
        bool LoadTextures();
        void PopulateShaderInfo();

//        CImageData		*m_pCheckerData;
        CTexture8		*m_pCheckerTex;
//        CImageData		*m_pChecker2Data;
        CTexture8		*m_pChecker2Tex;
//        CImageData		*m_pDP3Data;
        CTexture8		*m_pDP3Tex;
//        CImageData		*m_pBrickData;
        CTexture8		*m_pBrickTex;
//        CImageData		*m_pGradientData;
        CTexture8		*m_pGradientTex;
//        CImageData		*m_pDXLogoData;
        CTexture8		*m_pDXLogoTex;
};

class CPSRegister : public CPShader
{
    public:
        CPSRegister();

        UINT TestInitialize(void);
        bool ExecuteTest(UINT);
        bool TestTerminate(void);
        void CommandLineHelp(void);

    protected:
        bool LoadTextures();
        void PopulateShaderInfo();

//        CImageData *m_pCheckerData;
        CTexture8 *m_pCheckerTex;
//        CImageData *m_pDP3Data;
        CTexture8 *m_pDP3Tex;
//        CImageData *m_pColorCheckerData;
        CTexture8 *m_pColorCheckerTex;
//        CImageData *m_pDXLogoData;
        CTexture8 *m_pDXLogoTex;
};

class CPSSrcMod : public CPShader
{
    public:
        CPSSrcMod();

        UINT TestInitialize(void);
        bool ExecuteTest(UINT);
        bool TestTerminate(void);
        void CommandLineHelp(void);

    protected:
        bool LoadTextures();
        void PopulateShaderInfo();

//        CImageData *m_pCheckerData;
        CTexture8 *m_pCheckerTex;
//        CImageData *m_pDP3Data;
        CTexture8 *m_pDP3Tex;
//        CImageData *m_pColorCheckerData;
        CTexture8 *m_pColorCheckerTex;
//        CImageData *m_pDXLogoData;
        CTexture8 *m_pDXLogoTex;
};

class CPSDestMod : public CPShader
{
    public:
        CPSDestMod();

        UINT TestInitialize(void);
        bool ExecuteTest(UINT);
        bool TestTerminate(void);
        void CommandLineHelp(void);

    protected:
        bool LoadTextures();
        void PopulateShaderInfo();

//        CImageData *m_pCheckerData;
        CTexture8 *m_pCheckerTex;
//        CImageData *m_pDP3Data;
        CTexture8 *m_pDP3Tex;
//        CImageData *m_pColorCheckerData;
        CTexture8 *m_pColorCheckerTex;
//        CImageData *m_pDXLogoData;
        CTexture8 *m_pDXLogoTex;
};

class CPSInstMod : public CPShader
{
    public:
        CPSInstMod();

        UINT TestInitialize(void);
        bool ExecuteTest(UINT);
        bool TestTerminate(void);
        void CommandLineHelp(void);

    protected:
        bool LoadTextures();
        void PopulateShaderInfo();

//        CImageData *m_pDXLogoData;
        CTexture8 *m_pDXLogoTex;
};

class CPSAddress : public CPShader
{
    public:
        CPSAddress();

        UINT TestInitialize(void);
        bool ExecuteTest(UINT);
        bool TestTerminate(void);
        void CommandLineHelp(void);

    protected:
        bool LoadTextures();
        void PopulateShaderInfo();

//        CImageData *m_pCheckerData;
        CTexture8 *m_pCheckerTex;
//        CImageData *m_pColorCheckerData;
        CTexture8 *m_pColorCheckerTex;
//        CImageData *m_pBEM1Data;
        CTexture8 *m_pBEM1Tex;
//        CImageData *m_pBEM3Data;
        CTexture8 *m_pBEM3Tex;
        CTexture8 *m_pBrickTex;
        CTexture8 *m_pCubeTex;
        CTexture8 *m_pSysCubeTex;
//        CImageData *m_pCubeData[6];
};

#endif // _PSHADER_H_
