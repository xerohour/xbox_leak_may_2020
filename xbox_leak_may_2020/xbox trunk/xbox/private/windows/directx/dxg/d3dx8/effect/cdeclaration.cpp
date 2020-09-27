///////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) 1999 Microsoft Corporation.  All Rights Reserved.
//
//  File:       CDeclaration.cpp
//  Content:    Parse declarations
//
///////////////////////////////////////////////////////////////////////////

#include "pcheffect.h"


//
// Productions
//

enum
{
    P_DECLARATION_0,
    P_DECLARATION_1,
    P_STATEMENTS_0,
    P_STATEMENTS_1,
    P_STATEMENT_0,
    P_STATEMENT_1,
    P_STATEMENT_2,
    P_STATEMENT_3,
    P_STATEMENT_4,
    P_TYPE_UINT8,
    P_TYPE_INT16,
    P_TYPE_FLOAT,
    P_TYPE_D3DCOLOR,
    P_FVF_0,
    P_FVF_1,
    P_NUM_0,
    P_NUM_1,
    P_ID_0
};

#define Prod(n, m) \
    CD3DXDeclarationCompiler::s_pCompiler->Production(n, m)

#define d3dxdecl_lex \
    CD3DXDeclarationCompiler::s_pCompiler->Token

#define d3dxdecl_error \
    CD3DXDeclarationCompiler::s_pCompiler->Error

#define lint
#pragma warning(disable : 4242)
#include "decl_y.h"
#pragma warning(default : 4242)




//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

enum
{
    TYPE_UINT8,
    TYPE_INT16,
    TYPE_FLOAT,
    TYPE_D3DCOLOR
};

#define RVALUE_END() \
    { NULL, 0 }

static RValue g_rvFvf[] =
{
    { "XYZ",             D3DFVF_XYZ              },
    { "XYZRHW",          D3DFVF_XYZRHW           },
    { "XYZB1",           D3DFVF_XYZB1            },
    { "XYZB2",           D3DFVF_XYZB2            },
    { "XYZB3",           D3DFVF_XYZB3            },
    { "XYZB4",           D3DFVF_XYZB4            },

    { "NORMAL",          D3DFVF_NORMAL           },
    { "DIFFUSE",         D3DFVF_DIFFUSE          },
    { "SPECULAR",        D3DFVF_SPECULAR         },

    { "TEX0",            D3DFVF_TEX0             },
    { "TEX1",            D3DFVF_TEX1             },
    { "TEX2",            D3DFVF_TEX2             },
    { "TEX3",            D3DFVF_TEX3             },

    { "TEXCOORDSIZE1_0", D3DFVF_TEXCOORDSIZE1(0) },
    { "TEXCOORDSIZE1_1", D3DFVF_TEXCOORDSIZE1(1) },
    { "TEXCOORDSIZE1_2", D3DFVF_TEXCOORDSIZE1(2) },
    { "TEXCOORDSIZE1_3", D3DFVF_TEXCOORDSIZE1(3) },
    { "TEXCOORDSIZE1_4", D3DFVF_TEXCOORDSIZE1(4) },
    { "TEXCOORDSIZE1_5", D3DFVF_TEXCOORDSIZE1(5) },
    { "TEXCOORDSIZE1_6", D3DFVF_TEXCOORDSIZE1(6) },
    { "TEXCOORDSIZE1_7", D3DFVF_TEXCOORDSIZE1(7) },

    { "TEXCOORDSIZE2_0", D3DFVF_TEXCOORDSIZE2(0) },
    { "TEXCOORDSIZE2_1", D3DFVF_TEXCOORDSIZE2(1) },
    { "TEXCOORDSIZE2_2", D3DFVF_TEXCOORDSIZE2(2) },
    { "TEXCOORDSIZE2_3", D3DFVF_TEXCOORDSIZE2(3) },
    { "TEXCOORDSIZE2_4", D3DFVF_TEXCOORDSIZE2(4) },
    { "TEXCOORDSIZE2_5", D3DFVF_TEXCOORDSIZE2(5) },
    { "TEXCOORDSIZE2_6", D3DFVF_TEXCOORDSIZE2(6) },
    { "TEXCOORDSIZE2_7", D3DFVF_TEXCOORDSIZE2(7) },

    { "TEXCOORDSIZE3_0", D3DFVF_TEXCOORDSIZE3(0) },
    { "TEXCOORDSIZE3_1", D3DFVF_TEXCOORDSIZE3(1) },
    { "TEXCOORDSIZE3_2", D3DFVF_TEXCOORDSIZE3(2) },
    { "TEXCOORDSIZE3_3", D3DFVF_TEXCOORDSIZE3(3) },
    { "TEXCOORDSIZE3_4", D3DFVF_TEXCOORDSIZE3(4) },
    { "TEXCOORDSIZE3_5", D3DFVF_TEXCOORDSIZE3(5) },
    { "TEXCOORDSIZE3_6", D3DFVF_TEXCOORDSIZE3(6) },
    { "TEXCOORDSIZE3_7", D3DFVF_TEXCOORDSIZE3(7) },

    { "TEXCOORDSIZE4_0", D3DFVF_TEXCOORDSIZE4(0) },
    { "TEXCOORDSIZE4_1", D3DFVF_TEXCOORDSIZE4(1) },
    { "TEXCOORDSIZE4_2", D3DFVF_TEXCOORDSIZE4(2) },
    { "TEXCOORDSIZE4_3", D3DFVF_TEXCOORDSIZE4(3) },
    { "TEXCOORDSIZE4_4", D3DFVF_TEXCOORDSIZE4(4) },
    { "TEXCOORDSIZE4_5", D3DFVF_TEXCOORDSIZE4(5) },
    { "TEXCOORDSIZE4_6", D3DFVF_TEXCOORDSIZE4(6) },
    { "TEXCOORDSIZE4_7", D3DFVF_TEXCOORDSIZE4(7) },

    RVALUE_END()
};


//////////////////////////////////////////////////////////////////////////////
// CEffectCompiler ///////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

CD3DXDeclarationCompiler* CD3DXDeclarationCompiler::s_pCompiler = NULL;

CD3DXDeclarationCompiler::CD3DXDeclarationCompiler()
{
    m_pDeclaration = NULL;
    m_pErrors = NULL;

    m_bFail = FALSE;

    m_iLine     = 1;
    m_szFile[0] = '\0';
    m_pchSource = NULL;
    m_pchLine   = NULL;
    m_pch       = NULL;
    m_pchLim    = NULL;

    m_szTok[0]  = '\0';
    s_pCompiler = this;
}


CD3DXDeclarationCompiler::~CD3DXDeclarationCompiler()
{
    if(m_pDeclaration)
        delete m_pDeclaration;

    if(m_pErrors)
        delete m_pErrors;
}



//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////



HRESULT
CD3DXDeclarationCompiler::Compile(LPCVOID pv, UINT cb, LPCSTR szFile, UINT iLine,
        LPD3DXBUFFER *ppCompiledDeclaration, LPD3DXBUFFER *ppCompilationErrors)
{
    HRESULT hr = D3DERR_INVALIDCALL;

    if(!pv)
        goto LDone;

    m_bFail     = FALSE;
    m_iLine     = iLine;
    m_pchSource = (char *) pv;
    m_pch       = m_pchSource;
    m_pchLine   = m_pchSource;
    m_pchLim    = m_pchSource + cb;

    _snprintf(m_szFile, 256, "%s", szFile ? szFile : "");



    // Parse

    if(yyparse() || m_bFail)
    {
        delete m_pDeclaration;
        m_pDeclaration = NULL;
        hr = E_FAIL;
        goto LDone;
    }

    hr = S_OK;
    goto LDone;


LDone:
    // Create code buffer
    if(ppCompiledDeclaration)
    {
        UINT cdwSize = Serialize(m_pDeclaration, NULL);
    
        if(SUCCEEDED(D3DXCreateBuffer(cdwSize * sizeof(DWORD), ppCompiledDeclaration)))
        {
            Serialize(m_pDeclaration, (*ppCompiledDeclaration)->GetBufferPointer());
        }
    }

    // Create error buffer
    if(ppCompilationErrors)
    {
        *ppCompilationErrors = NULL;

        CNode *pNode;
        UINT cbSize = 1;

        for(pNode = m_pErrors; pNode; pNode = pNode->m_pStack)
            cbSize += strlen((char *) pNode->m_pdwData) + 1;

        if(SUCCEEDED(D3DXCreateBuffer(cbSize, ppCompilationErrors)))
        {
            BYTE *pb = (BYTE *) (*ppCompilationErrors)->GetBufferPointer() + cbSize;
            *--pb = '\0';

            for(pNode = m_pErrors; pNode; pNode = pNode->m_pStack)
            {
                UINT cb = strlen((char *) pNode->m_pdwData) + 1;

                pb -= cb;
                memcpy(pb, pNode->m_pdwData, cb);
                pb[cb - 1] = '\n';
            }   
        }
    }

    return hr;
}





int
CD3DXDeclarationCompiler::Token()
{
    char *pch;
    SIZE_T cch;

    while(m_pch < m_pchLim)
    {
        char ch = *m_pch;

        if(ch == '\n')
        {
            // newline
            m_pch++;
            m_pchLine = m_pch;
            m_iLine++;
        }
        else if(isspace(ch))
        {
            // whitespace
            m_pch++;
        }
        else if(ch == '#' && m_pch == m_pchLine)
        {
            // preprocessor directive
            pch = m_pch;
            for(m_pch++; m_pch < m_pchLim && *m_pch != '\n'; m_pch++);

            if(m_pch - pch > 5 && !memcmp(pch, "#line", 5))
            {
                // Skip #line
                for(; pch < m_pch && !isspace(*pch); pch++);

                // Skip whitespace
                for(; pch < m_pch && isspace(*pch); pch++);
                char *pchNum = pch;

                // Line number
                for(; pch < m_pch && !isspace(*pch); pch++);
                char *pchNumLim = pch;

                // Beginning of file name
                for(; pch < m_pch && *pch != '"'; pch++);
                char *pchFile = pch;

                if(pch < m_pch)
                    pch++;

                // End of file name
                for(; pch < m_pch && *pch != '"'; pch++);
                char *pchFileLim = pch;


                char sz[256];

                SIZE_T cchNum = pchNumLim - pchNum;
                SIZE_T cchFile = pchFileLim - pchFile;

                if(cchNum >= sizeof(sz))
                    cchNum = sizeof(sz) - 1;

                if(cchFile >= sizeof(m_szFile))
                    cchFile = sizeof(m_szFile) - 1;

                if(cchNum)
                {
                    memcpy(sz, pchNum, cchNum);
                    sz[cchNum] = 0;
                    m_iLine = atoi(sz) - 1;
                }

                if(cchFile > 1)
                {
                    // Copy filename, removing double backslashes
                    pchFile++;
                    pch = m_szFile;

                    while(pchFile < pchFileLim)
                    {
                        *pch = *pchFile++;

                        if(*pch == '\\' && pchFile < pchFileLim && *pchFile == '\\')
                            pchFile++;

                        pch++;
                    }

                    *pch = 0;
                }
            }
            else
            {
                Error("preprocessor directives are not supported.");
            }
        }
        else if(ch == '/' && m_pch + 1 < m_pchLim && m_pch[1] == '/')
        {
            // comment
            for(m_pch++; m_pch < m_pchLim && *m_pch != '\n'; m_pch++);
        }
        else if(isalpha(ch))
        {
            // keyword / identifier
            pch = m_pch;
            for(m_pch++; m_pch < m_pchLim && (isalnum(*m_pch) || '_' == *m_pch); m_pch++);

            cch = m_pch - pch;

            if(cch >= sizeof(m_szTok))
                cch = sizeof(m_szTok) - 1;

            memcpy(m_szTok, pch, cch);
            m_szTok[cch] = 0;

            if(!_stricmp(m_szTok, "stream"))     { return T_KW_STREAM; }
            if(!_stricmp(m_szTok, "skip"))       { return T_KW_SKIP; }
            if(!_stricmp(m_szTok, "fvf"))        { return T_KW_FVF; }
            if(!_stricmp(m_szTok, "ubyte"))      { return T_KW_UINT8; }
            if(!_stricmp(m_szTok, "short"))      { return T_KW_INT16; }
            if(!_stricmp(m_szTok, "float"))      { return T_KW_FLOAT; }
            if(!_stricmp(m_szTok, "d3dcolor"))   { return T_KW_D3DCOLOR; }

            return T_ID;
        }
        else if(ch == '0' && m_pch + 1 < m_pchLim && m_pch[1] == 'x')
        {
            // hex
            m_pch += 2;
            pch = m_pch;

            for(; m_pch < m_pchLim && (isdigit(*m_pch) || tolower(*m_pch) >= 'a' && tolower(*m_pch) <= 'f'); m_pch++);

            cch = m_pch - pch;

            if(cch >= sizeof(m_szTok))
                cch = sizeof(m_szTok) - 1;

            memcpy(m_szTok, pch, cch);
            m_szTok[cch] = 0;

            return T_HEX;
        }
        else if(isdigit(ch))
        {
            // number
            pch = m_pch;
            for(m_pch++; m_pch < m_pchLim && isdigit(*m_pch); m_pch++);
            cch = m_pch - pch;

            if(cch >= sizeof(m_szTok))
                cch = sizeof(m_szTok) - 1;

            memcpy(m_szTok, pch, cch);
            m_szTok[cch] = 0;

            return T_NUM;
        }
        else
        {
            // other character
            m_pch++;
            return ch;
        }
    }


    if(m_pch == m_pchLim)
    {
        m_pch++;
        return T_EOF;
    }

    return -1;
}



void
CD3DXDeclarationCompiler::Production(UINT uProduction, UINT uArgs)
{
    UINT i;
    DWORD pdwDecl[MAX_FVF_DECL_SIZE];

    CNode *pNode = NULL;
    CNode **ppArg = (CNode **) _alloca(uArgs * sizeof(CNode *));

    if(m_bFail)
        return;


    // Pop arguments off of stack
    for(i = uArgs; i--; )
        ppArg[i] = Pop();

    if(m_bFail)
        goto LDone;


    // Handle productions
    switch(uProduction)
    {
    case P_DECLARATION_0:
        if(FAILED(CreateNode(N_INTERNAL, 1, 0, &pNode)))
            break;

        pNode->m_pdwData[0] = D3DVSD_END();
        break;

    case P_DECLARATION_1:
        if((ppArg[0]->m_pdwData[0] & D3DVSD_TOKENTYPEMASK) != (D3DVSD_TOKEN_STREAM << D3DVSD_TOKENTYPESHIFT))
        {
            if(FAILED(CreateNode(N_INTERNAL, 1, 1, &pNode)))
                break;

            pNode->m_pdwData[0] = D3DVSD_STREAM(0);
            pNode->m_ppNodes[0] = ppArg[0];
            ppArg[0] = pNode;
        }

        for(pNode = ppArg[0]; pNode->m_ppNodes[0]; pNode = pNode->m_ppNodes[0]);

        if(FAILED(CreateNode(N_INTERNAL, 1, 0, &pNode->m_ppNodes[0])))
            break;

        pNode->m_ppNodes[0]->m_pdwData[0] = D3DVSD_END();

        pNode = ppArg[0];
        ppArg[0] = NULL;
        break;

    case P_STATEMENTS_0:
        pNode = ppArg[0];
        ppArg[0] = NULL;
        break;

    case P_STATEMENTS_1:
        pNode = ppArg[0];
        ppArg[0] = NULL;

        pNode->m_ppNodes[0] = ppArg[1];
        ppArg[1] = NULL;
        break;

    case P_STATEMENT_0:
        if(FAILED(CreateNode(N_INTERNAL, 1, 1, &pNode)))
            break;

        pNode->m_pdwData[0] = D3DVSD_STREAM(ppArg[0]->m_pdwData[0]);
        pNode->m_ppNodes[0] = NULL;
        break;
        
    case P_STATEMENT_1:
        if(FAILED(CreateNode(N_INTERNAL, 1, 1, &pNode)))
            break;

        pNode->m_pdwData[0] = D3DVSD_SKIP(ppArg[0]->m_pdwData[0]);
        pNode->m_ppNodes[0] = NULL;
        break;
        
    case P_STATEMENT_2:
        {
            DWORD cdw;

            if(FAILED(D3DXDeclaratorFromFVF(ppArg[0]->m_pdwData[0], pdwDecl)))
                break;

            for(cdw = 0; pdwDecl[cdw] != D3DVSD_END(); cdw++);

            if(FAILED(CreateNode(N_INTERNAL, cdw + 1, 1, &pNode)))
                break;

            memcpy(&pNode->m_pdwData[0], &pdwDecl[1], cdw * sizeof(DWORD));
            pNode->m_ppNodes[0] = NULL;
        }

        break;

    case P_STATEMENT_3:
        if(FAILED(CreateNode(N_INTERNAL, 1, 1, &pNode)))
            break;

        if(FAILED(LookupDecl(ppArg[0]->m_pdwData[0], (char *) ppArg[1]->m_pdwData, 1, &pNode->m_pdwData[0])))
            break;

        pNode->m_ppNodes[0] = NULL;
        break;

    case P_STATEMENT_4:
        if(FAILED(CreateNode(N_INTERNAL, 1, 1, &pNode)))
            break;

        if(FAILED(LookupDecl(ppArg[0]->m_pdwData[0], (char *) ppArg[1]->m_pdwData, ppArg[2]->m_pdwData[0], &pNode->m_pdwData[0])))
            break;

        pNode->m_ppNodes[0] = NULL;
        break;


    case P_TYPE_UINT8:
        if(FAILED(CreateNode(N_INTERNAL, 1, 0, &pNode)))
            break;

        pNode->m_pdwData[0] = TYPE_UINT8;
        break;

    case P_TYPE_INT16:
        if(FAILED(CreateNode(N_INTERNAL, 1, 0, &pNode)))
            break;

        pNode->m_pdwData[0] = TYPE_INT16;
        break;

    case P_TYPE_FLOAT:
        if(FAILED(CreateNode(N_INTERNAL, 1, 0, &pNode)))
            break;

        pNode->m_pdwData[0] = TYPE_FLOAT;
        break;

    case P_TYPE_D3DCOLOR:
        if(FAILED(CreateNode(N_INTERNAL, 1, 0, &pNode)))
            break;

        pNode->m_pdwData[0] = TYPE_D3DCOLOR;
        break;

    case P_FVF_0:
        if(FAILED(CreateNode(N_INTERNAL, 1, 0, &pNode)))
            break;

        pNode->m_pdwData[0] = 0;

        if(FAILED(LookupFvf((char *) ppArg[0]->m_pdwData, &pNode->m_pdwData[0])))
            break;

        break;

    case P_FVF_1:
        pNode = ppArg[1];
        ppArg[1] = NULL;

        if(FAILED(LookupFvf((char *) ppArg[0]->m_pdwData, &pNode->m_pdwData[0])))
            break;

        break;

    case P_NUM_0:
        if(FAILED(CreateNode(N_INTERNAL, 1, 0, &pNode)))
            break;

        sscanf(m_szTok, "%d", &pNode->m_pdwData[0]);
        break;

    case P_NUM_1:
        if(FAILED(CreateNode(N_INTERNAL, 1, 0, &pNode)))
            break;

        sscanf(m_szTok, "%x", &pNode->m_pdwData[1]);
        break;

    case P_ID_0:
        {
            UINT cb  = strlen(m_szTok) + 1;
            UINT cdw = (cb + 3) >> 2;

            if(FAILED(CreateNode(N_INTERNAL, cdw, 0, &pNode)))
                break;

            memset(pNode->m_pdwData, 0x00, cdw * sizeof(DWORD));
            memcpy(pNode->m_pdwData, m_szTok, cb);
        }

        break;
    }


    // Push the new node on the stack
    if(!m_bFail)
    {
        if(pNode)
            Push(pNode);
        else
            Error("Internal compiler error! %d", __LINE__);
    }


LDone:
    // Delete arguments
    for(i = 0; i < uArgs; i++)
    {
        if(ppArg[i])
            delete ppArg[i];
    }

    return;
}



void
CD3DXDeclarationCompiler::Push(CNode *pNode)
{
    if(!pNode || pNode->m_pStack)
    {
        Error("Internal compiler error! %d", __LINE__);
        return;
    }

    pNode->m_pStack = m_pDeclaration;
    m_pDeclaration = pNode;
    return;
}


CNode *
CD3DXDeclarationCompiler::Pop()
{
    if(!m_pDeclaration)
    {
        Error("Internal compiler error! %d", __LINE__);
        return NULL;
    }

    CNode *pNode = m_pDeclaration;
    m_pDeclaration = pNode->m_pStack;
    pNode->m_pStack = NULL;
    return pNode;
}


UINT 
CD3DXDeclarationCompiler::Serialize(CNode *pNode, LPVOID pv)
{
    UINT   cdw = 0;
    DWORD* pdw = (DWORD *) pv;
    
    if(!pNode)
        return 0;


    // Data
    if(pdw)
    {
        memcpy(pdw, pNode->m_pdwData, pNode->m_cdwData * sizeof(DWORD));
        pdw += pNode->m_cdwData;
    }

    cdw += pNode->m_cdwData;


    // Nodes
    for(UINT i = 0; i < pNode->m_cNodes; i++)
    {
        UINT cdwNode;

        cdwNode = Serialize(pNode->m_ppNodes[i], pdw);

        if(pdw)
            pdw += cdwNode;

        cdw += cdwNode;
    }

    return cdw;
}


void
CD3DXDeclarationCompiler::Error(char *szFormat, ...)
{
    char szA[256];
    char szB[256];


    // Format error
    va_list ap;
    va_start(ap, szFormat);
    _vsnprintf(szA, sizeof(szA), szFormat, ap);
    szA[255] = '\0';
    va_end(ap);

    _snprintf(szB, sizeof(szB), "%s(%d) : %s", m_szFile, m_iLine, szA);
    szB[255] = '\0';


    // Output error in debug spew
    DPF(0, "%s", szB);
    AppendError(szB);
    m_bFail = TRUE;
}


void
CD3DXDeclarationCompiler::AppendError(char *sz)
{
    UINT cb = strlen(sz) + 1;
    CNode *pNode = new CNode;

    if(pNode && SUCCEEDED(pNode->Initialize(0, (cb + 3) >> 2, 0)))
    {
        memcpy(pNode->m_pdwData, sz, cb);
        pNode->m_pStack = m_pErrors;
        m_pErrors = pNode;
    }
}


HRESULT 
CD3DXDeclarationCompiler::CreateNode(DWORD dwId, UINT cdwData, UINT cNodes, CNode **ppNode)
{
    CNode *pNode;

    D3DXASSERT(ppNode != NULL);

    if(!(pNode = new CNode))
    {
        Error("Out of memory!");
        return E_OUTOFMEMORY;
    }

    if(FAILED(pNode->Initialize(dwId, cdwData, cNodes)))
    {
        Error("Out of memory!");
        return E_OUTOFMEMORY;
    }

    *ppNode = pNode;
    return S_OK;
}


HRESULT 
CD3DXDeclarationCompiler::LookupDecl(DWORD dwType, char *szId, DWORD dwDim, DWORD *pdwDecl)
{
    DWORD dwTDim = (DWORD) -1;

    switch(dwType)
    {

    case TYPE_UINT8:
        switch(dwDim)
        {
        case 4: _asm int 3; break; // dwTDim = D3DVSDT_UBYTE4; break;
        }
        break;

    case TYPE_INT16:
        switch(dwDim)
        {
        case 2: dwTDim = D3DVSDT_SHORT2; break;
        case 4: dwTDim = D3DVSDT_SHORT4; break;
        }
        break;

    case TYPE_FLOAT:
        switch(dwDim)
        {
        case 1: dwTDim = D3DVSDT_FLOAT1; break;
        case 2: dwTDim = D3DVSDT_FLOAT2; break;
        case 3: dwTDim = D3DVSDT_FLOAT3; break;
        case 4: dwTDim = D3DVSDT_FLOAT4; break;
        }
        break;

    case TYPE_D3DCOLOR:
        switch(dwDim)
        {
        case 1: dwTDim = D3DVSDT_D3DCOLOR; break;
        }
        break;
    }

    if((DWORD) -1 == dwTDim)
    {
        Error("Invalid dimension");
        return E_FAIL;
    }


    UINT cch = strlen(szId);
    char *psz = szId;

    if(*psz != 'v')
    {
        Error("Vertex register expected (v#)");
        return E_FAIL;
    }

    for(psz++; *psz && isdigit(*psz); psz++);

    if(*psz)
    {
        Error("Vertex register expected (v#)");
        return E_FAIL;
    }

    DWORD dwReg = atoi(&szId[1]);

    *pdwDecl = D3DVSD_REG(dwReg, dwTDim);
    return S_OK;
}


HRESULT 
CD3DXDeclarationCompiler::LookupFvf(char *szId, DWORD *pdwFvf)
{
    RValue *prv = g_rvFvf;

    while(prv->szName)
    {
        if(!_stricmp(szId, prv->szName))
        {
            *pdwFvf |= prv->dwValue;
            return S_OK;
        }

        prv++;
    }

    Error("Unknown identifier in FVF experssion");
    return E_FAIL;
}
