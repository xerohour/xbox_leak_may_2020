///////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) 1999 Microsoft Corporation.  All Rights Reserved.
//
//  File:       CDeclaration.h
//  Content:    Declaration compiler
//
///////////////////////////////////////////////////////////////////////////



//----------------------------------------------------------------------------
// CD3DXDeclarationCompiler
//----------------------------------------------------------------------------

class CD3DXDeclarationCompiler
{
    CNode *m_pDeclaration;
    CNode *m_pErrors;

    BOOL  m_bFail;

    UINT  m_iLine;
    char  m_szFile[256];
    char *m_pchSource;
    char *m_pchLine;
    char *m_pch;
    char *m_pchLim;

    char  m_szTok[256];

public:
    static CD3DXDeclarationCompiler* s_pCompiler;

public:
    CD3DXDeclarationCompiler();
   ~CD3DXDeclarationCompiler();

    HRESULT Compile(LPCVOID pv, UINT cb, LPCSTR szFile, UINT iLine,
        LPD3DXBUFFER *ppCompiledDeclaration, LPD3DXBUFFER *ppCompilationErrors);

    int     Token();
    void    Production(UINT uProduction, UINT uArgs);
    void    Push(CNode *pNode);
    CNode*  Pop();
    UINT    Serialize(CNode *pNode, LPVOID pv);
    void    Error(char *psz, ...);
    void    AppendError(char *psz);


    HRESULT CreateNode(DWORD dwId, UINT cdwData, UINT cNodes, CNode **ppNode);
    HRESULT LookupDecl(DWORD dwType, char *szId, DWORD dwDim, DWORD *pdwDecl);
    HRESULT LookupFvf(char *szId, DWORD *pdwFvf);
};
