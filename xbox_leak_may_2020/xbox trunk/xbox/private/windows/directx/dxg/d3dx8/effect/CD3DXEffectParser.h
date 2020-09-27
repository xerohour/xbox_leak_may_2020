///////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) 1999 Microsoft Corporation.  All Rights Reserved.
//
//  File:       compiler.h
//  Content:    Vertex Shader compiler
//
///////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

enum
{
    NODE_EFFECT = 0,
    NODE_DECLARATION,
    NODE_TECHNIQUE,
    NODE_PASS,
    NODE_STAGE,
    NODE_VSHADER,
    NODE_PSHADER,
    NODE_STATE,
    NODE_ID,
    NODE_DWORD,
    NODE_FLOAT,
    NODE_VECTOR,
    NODE_ASM,
};



class Node
{
public:
    DWORD m_dwType;
    Node *m_pNext;
    Node *m_pStack;

public:
    Node(DWORD dwType);
    virtual ~Node();
};

class EffectNode : public Node
{
public:
    Node *m_pDeclarations;
    Node *m_pTechniques;

public:
    EffectNode();
    virtual ~EffectNode();
};


class DeclarationNode : public Node
{
public:
    Node *m_pType;
    Node *m_pName;

public:
    DeclarationNode();
    virtual ~DeclarationNode();
};


class TechniqueNode : public Node
{
public:
    Node *m_pLOD;
    Node *m_pStates;
    Node *m_pPasses;
    Node *m_pShaders;

public:
    TechniqueNode();
    virtual ~TechniqueNode();
};


class PassNode : public Node
{
public:
    Node *m_pSrcBlend;
    Node *m_pDstBlend;
    Node *m_pStates;
    Node *m_pStages;
    Node *m_pShaders;

public:
    PassNode();
    virtual ~PassNode();
};


class StageNode : public Node
{
public:
    Node *m_pTexture;
    Node *m_pMatrix;
    Node *m_pStates;

public:
    StageNode();
    virtual ~StageNode();
};


class VShaderNode : public Node
{
public:
    Node *m_pConst;
    Node *m_pAsm;

public:
    VShaderNode();
    virtual ~VShaderNode();
};


class PShaderNode : public Node
{
public:
    Node *m_pConst;
    Node *m_pAsm;

public:
    PShaderNode();
    virtual ~PShaderNode();
};


class StateNode : public Node
{
public:
    Node *m_pLValue;
    Node *m_pRValue;

public:
    StateNode();
    virtual ~StateNode();
};


class IdNode : public Node
{
public:
    char *m_szId;

public:
    IdNode(char *sz);
    virtual ~IdNode();
};


class DwordNode : public Node
{
public:
    DWORD m_dwValue;

public:
    DwordNode(DWORD dwValue);
};


class FloatNode : public Node
{
public:
    float m_fValue;

public:
    FloatNode(float fValue);
};


class VectorNode : public Node
{
public:
    D3DXVECTOR4 m_vec;

public:
    VectorNode(float x, float y, float z, float w);
};


class AsmNode : public Node
{
public:
    char *m_pchAsm;
    UINT  m_cchAsm;

public:
    AsmNode(char *pchAsm, UINT cchAsm);
};


struct Binding
{
    char *m_szName;
    DWORD m_dwValue;
    Binding *m_pBind;
};




//----------------------------------------------------------------------------
// CD3DXEffectParser
//----------------------------------------------------------------------------

class CD3DXEffectParser
{
    Node *m_pNode;

    BOOL  m_bFail;
    UINT  m_cErrors;
    UINT  m_cbErrors;

    char *m_pchSource;
    char *m_pchLine;
    char *m_pch;
    char *m_pchLim;

    char  m_szTok[256];
    char *m_pchAsm;
    UINT  m_cchAsm;

    UINT  m_iLine;
    char  m_szFile[256];

    EffectNode *m_pEffectNode;
    EffectData *m_pEffectData;

public:
    static CD3DXEffectParser* s_pParser;

public:
    CD3DXEffectParser();
   ~CD3DXEffectParser();


    HRESULT Parse(LPCVOID pv, UINT cb, LPCSTR szFile, LPDIRECT3DDEVICE8 pDevice, 
        LPD3DXEFFECT *ppEffect, LPD3DXBUFFER *ppErrors);


    int  Token();
    void Production(UINT nProduction);
    void Error(char *psz, ...);


protected:
    void  Push(Node *pNode);
    Node *Pop();
    void  Combine();

    HRESULT Compile();
    BOOL Eval(Node *pNode, Binding *pBind, DWORD *pdw, Binding **ppBind);
};
