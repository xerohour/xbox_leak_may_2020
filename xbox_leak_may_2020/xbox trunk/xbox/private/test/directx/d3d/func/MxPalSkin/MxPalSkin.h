// MxPalSkin.h
// written by Trevor Schrock

#ifndef __CMatrixPaletteSkinningTest_H__
#define __CMatrixPaletteSkinningTest_H__


#define TESTDXVER 0x0800

//#define STRICT

//USETESTFRAME

#define VEDGE               (16)
#define VEDGE_1             ( (VEDGE) - 1)
#define NUM_VERTICES        ( (VEDGE) * (VEDGE) )
#define NUM_INDICES         ( (VEDGE) * 2 * ( (VEDGE) - 1) )

#define IDX(x)              (x)

#define IMAGE_FILE          _T("mxpalskin.bmp")
#define SHADER2             _T("mxpalskin2.vsh")
#define SHADER3             _T("mxpalskin3.vsh")
#define SHADER4             _T("mxpalskin4.vsh")

#define COUNTOF(a) (sizeof(a)/sizeof((a)[0]))
#define DecodeHResult(hr)  TEXT("")//(GetHResultString(hr).c_str())



typedef struct _TestVertexNP1
{
  D3DXVECTOR3     m_vPos;
  float           m_fWeights[1];
  D3DXVECTOR3     m_vNormal;
  float           m_fTex0[2];
  
} TestVertexNP1, *LPTestVertexNP1;

typedef struct _TestVertexNP2
{
  D3DXVECTOR3     m_vPos;
  float           m_fWeights[2];
  D3DXVECTOR3     m_vNormal;
  float           m_fTex0[2];
  
} TestVertexNP2, *LPTestVertexNP2;

typedef struct _TestVertexNP3
{
  D3DXVECTOR3     m_vPos;
  float           m_fWeights[3];
  D3DXVECTOR3     m_vNormal;
  float           m_fTex0[2];
  
} TestVertexNP3, *LPTestVertexNP3;

typedef struct _TestVertexP1
{
  D3DXVECTOR3     m_vPos;
  float           m_fWeights[1];
  UCHAR           m_ucIndices[4];
  D3DXVECTOR3     m_vNormal;
  float           m_fTex0[2];
  
} TestVertexP1, *LPTestVertexP1;

typedef struct _TestVertexP2
{
  D3DXVECTOR3     m_vPos;
  float           m_fWeights[2];
  UCHAR           m_ucIndices[4];
  D3DXVECTOR3     m_vNormal;
  float           m_fTex0[2];
  
} TestVertexP2, *LPTestVertexP2;

typedef struct _TestVertexP3
{
  D3DXVECTOR3     m_vPos;
  float           m_fWeights[3];
  UCHAR           m_ucIndices[4];
  D3DXVECTOR3     m_vNormal;
  float           m_fTex0[2];
  
} TestVertexP3, *LPTestVertexP3;

// used for ProcessVertices call
typedef struct _CapturedVertex
{
  D3DXVECTOR3     m_vPos;
  float           m_fRHW;
  DWORD           m_dwDiffuse;
  DWORD           m_dwSpecular;
  float           m_fTex0[4];
} CapturedVertex, *LPCapturedVertex;


#define D3DFVF_NP1 ( (D3DFORMAT)(D3DFVF_XYZB1 | D3DFVF_NORMAL | D3DFVF_TEX1) )
#define D3DFVF_NP2 ( (D3DFORMAT)(D3DFVF_XYZB2 | D3DFVF_NORMAL | D3DFVF_TEX1) )
#define D3DFVF_NP3 ( (D3DFORMAT)(D3DFVF_XYZB3 | D3DFVF_NORMAL | D3DFVF_TEX1) )
#define D3DFVF_P1  ( (D3DFORMAT)(D3DFVF_XYZB2 | D3DFVF_NORMAL | D3DFVF_TEX1 | D3DFVF_LASTBETA_UBYTE4) )
#define D3DFVF_P2  ( (D3DFORMAT)(D3DFVF_XYZB3 | D3DFVF_NORMAL | D3DFVF_TEX1 | D3DFVF_LASTBETA_UBYTE4) )
#define D3DFVF_P3  ( (D3DFORMAT)(D3DFVF_XYZB4 | D3DFVF_NORMAL | D3DFVF_TEX1 | D3DFVF_LASTBETA_UBYTE4) )
#define D3DFVF_CAP ( (D3DFORMAT)(D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_SPECULAR | D3DFVF_TEX1 | D3DFVF_TEXCOORDSIZE4(0) ) )


class CVertexBlender : public CD3DTest
{
  
protected:
  
//  DEVICEDESC             *m_pDesc;
  DWORD                   m_dwMaxBlends;
  DWORD                   m_dwMaxIndex;
  
//  CMaterial              *m_pMaterial;
  D3DMATERIAL8            m_Material;
//  CLight                 *m_pLight;
  D3DLIGHT8               m_Light;
//  CImageData             *m_pImage;
  CTexture8              *m_pTexture;
  CShapes                 m_Mesh;

  D3DXMATRIX              m_mx[4];
  D3DXMATRIX              m_mxProj, m_mxView;

  int                     m_nMatrices;
  bool                    m_bPalette;
  bool                    m_bVS;
  DWORD                   m_dwShader;
  VOID                   *m_pvx;

  TCHAR                   m_szTitle[128];

  BOOL                    m_bExit;
  
public:
  
  CVertexBlender(int nMatrices, bool bPalette, bool bVS);
  ~CVertexBlender();
  
  bool SetDefaultMatrices(void);
  bool SetDefaultMaterials(void);
  bool SetDefaultLights(void);
  bool SetDefaultRenderStates(void);
  UINT TestInitialize(void);
  bool ProcessFrame(void);
  bool ExecuteTest(UINT);
  void SceneRefresh(void);
  bool TestTerminate(void);
//  void CommandLineHelp(void);

  bool SetupGeometry();
  HRESULT RenderBlender();

  virtual BOOL    AbortedExit(void);

protected:

  virtual void    ProcessInput();
  virtual BOOL    WndProc(LRESULT* plr, HWND hWnd, UINT uMsg, 
                      WPARAM wParam, LPARAM lParam);
};


class CBlend_NoPalette_FVF : public CVertexBlender
{
  
public:
  
  CBlend_NoPalette_FVF(int nMatrices);
  ~CBlend_NoPalette_FVF();
  
  UINT TestInitialize(void);
  bool ExecuteTest(UINT);

  bool ApplyWeights(UINT nTest);
};

class CBlend_Palette_FVF   : public CVertexBlender
{

public:
  
  CBlend_Palette_FVF(int nMatrices);
  ~CBlend_Palette_FVF();
  
  UINT TestInitialize(void);
  bool ExecuteTest(UINT);

  bool ApplyWeights(UINT nTest);
};

class CBlend_VS    : public CVertexBlender
{
  
public:
  
  CBlend_VS(int nMatrices);
  ~CBlend_VS();
  
  UINT TestInitialize(void);
  bool ExecuteTest(UINT);
  bool TestTerminate(void);

  bool ApplyWeights(UINT nTest);
};


#endif
