///////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) 1999 Microsoft Corporation.  All Rights Reserved.
//
//  File:       compiler.h
//  Content:    Vertex Shader compiler
//
///////////////////////////////////////////////////////////////////////////



//
// Binary file nodes
//

#define N_EFFECT     MAKEFOURCC('F', 'X', '1', '0')
#define N_PARAMETERS MAKEFOURCC('P', 'A', 'R', 'S')
#define N_TECHNIQUES MAKEFOURCC('T', 'E', 'C', 'S')
#define N_PARAMETER  MAKEFOURCC('P', 'A', 'R', 'M')
#define N_TECHNIQUE  MAKEFOURCC('T', 'E', 'C', 'H')
#define N_PASS       MAKEFOURCC('P', 'A', 'S', 'S')
#define N_ASSIGNMENT MAKEFOURCC('A', 'S', 'G', 'N')
#define N_VALUE      MAKEFOURCC('V', 'A', 'L', 'U')
#define N_REFERENCE  MAKEFOURCC('R', 'E', 'F', 'E')
#define N_INTERNAL   MAKEFOURCC('X', 'X', 'X', 'X')



//
// Effect States
//

#define D3DXES_RS(rs)               (0x01000000 | (rs  & 0x00ffffff))
#define D3DXES_TSS(tss)             (0x02000000 | (tss & 0x00ffffff))

#define D3DXES_LIGHTTYPE            (0x03000000)
#define D3DXES_LIGHTDIFFUSE         (0x03000001)
#define D3DXES_LIGHTSPECULAR        (0x03000002)
#define D3DXES_LIGHTAMBIENT         (0x03000003)
#define D3DXES_LIGHTPOSITION        (0x03000004)
#define D3DXES_LIGHTDIRECTION       (0x03000005)
#define D3DXES_LIGHTRANGE           (0x03000006)
#define D3DXES_LIGHTFALLOFF         (0x03000007)
#define D3DXES_LIGHTATTENUATION0    (0x03000008)
#define D3DXES_LIGHTATTENUATION1    (0x03000009)
#define D3DXES_LIGHTATTENUATION2    (0x0300000a)
#define D3DXES_LIGHTTHETA           (0x0300000b)
#define D3DXES_LIGHTPHI             (0x0300000c)
#define D3DXES_LIGHTENABLE          (0x0300000d)

#define D3DXES_MATERIALDIFFUSE      (0x04000000)
#define D3DXES_MATERIALAMBIENT      (0x04000001)
#define D3DXES_MATERIALSPECULAR     (0x04000002)
#define D3DXES_MATERIALEMISSIVE     (0x04000003)
#define D3DXES_MATERIALPOWER        (0x04000004)

#define D3DXES_TEXTURE              (0x05000000)

#define D3DXES_VERTEXSHADER         (0x06000000)
#define D3DXES_PIXELSHADER          (0x06000001)
#define D3DXES_VERTEXSHADERCONSTANT (0x06000002)
#define D3DXES_PIXELSHADERCONSTANT  (0x06000003)

#define D3DXES_PROJECTIONTRANSFORM  (0x07000000)
#define D3DXES_VIEWTRANSFORM        (0x07000001)
#define D3DXES_WORLDTRANSFORM       (0x07000002)
#define D3DXES_TEXTURETRANSFORM     (0x07000003)



//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

//
// Node
//

class CNode
{
public:
    DWORD   m_dwType;
    UINT    m_cdwData;
    DWORD*  m_pdwData;
    UINT    m_cNodes;
    CNode** m_ppNodes;
    CNode*  m_pStack;

public:
    CNode();
   ~CNode();

    HRESULT Initialize(DWORD dwType, UINT cdwData, UINT cNodes);
};




//
// RValue
//

struct RValue
{
    char*    szName;
    DWORD    dwValue;
};


//
// LValue
//

struct LValue
{
    char*    szName;
    DWORD    dwType;
    DWORD    dwSize;
    DWORD    dwState;
    RValue*  pRValue;
};




//----------------------------------------------------------------------------
// CD3DXEffectCompiler
//----------------------------------------------------------------------------

class CD3DXEffectCompiler
{
    CNode *m_pEffect;
    CNode *m_pErrors;
    CNode *m_pParameters;

    BOOL  m_bFail;

    UINT  m_iLine;
    char  m_szFile[256];
    char *m_pchSource;
    char *m_pchLine;
    char *m_pch;
    char *m_pchLim;

    char  m_szTok[256];
    char *m_pchAsm;
    UINT  m_cchAsm;
    char  m_szFileAsm[256];
    UINT  m_iLineAsm;

public:
    static CD3DXEffectCompiler* s_pCompiler;

public:
    CD3DXEffectCompiler();
   ~CD3DXEffectCompiler();

    HRESULT Compile(LPCVOID pv, UINT cb, LPCSTR szFile, 
        LPD3DXBUFFER *ppCompiledEffect, LPD3DXBUFFER *ppCompilationErrors);

    int     Token();
    void    Production(UINT uProduction, UINT uArgs);
    void    Push(CNode *pNode);
    CNode*  Pop();
    UINT    Serialize(CNode *pNode, LPVOID pv);
    void    Error(char *psz, ...);
    void    AppendError(char *psz);

    HRESULT CreateNode(DWORD dwId, UINT cdwData, UINT cNodes, CNode **ppNode);
    HRESULT EvalExpression(DWORD dwState, CNode **ppNode);
    HRESULT ValidAssignment(DWORD dwDstType, DWORD dwSrcType);
    HRESULT LookupType(char *psz, DWORD *pdwType);
    HRESULT LookupState(char *psz, DWORD dwIndex, DWORD *pdwType, DWORD *pdwState);
    HRESULT LookupParameter(DWORD dwName, DWORD *pdwType);
    HRESULT UnusedParameter(DWORD dwName);
    HRESULT MakeFourCC(char *psz, DWORD *pdwFourCC);
    HRESULT AssembleShader(CNode *pDecl, CNode *pFunc, CNode **ppNode);
};
