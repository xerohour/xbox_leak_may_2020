///////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) 1999 Microsoft Corporation.  All Rights Reserved.
//
//
///////////////////////////////////////////////////////////////////////////

#include "pchshadeasm.h"

// #include "d3dhal.h"

namespace XGRAPHICS {

	inline int Isspace(int ch) {
        return isspace(ch) || ch == '\x1a';
	}

enum
{
    P_SHADER_0,
    P_SHADER_1,
    P_SHADER_2,
    P_SHADER_3,
    P_VERSION_0,
    P_VERSION_1,
    P_VERSION_XVS,
    P_VERSION_XVSW,
    P_VERSION_XVSS,
    P_VERSION_XPS,
    P_BODY_0,
    P_BODY_1,
    P_CONSTANTS_0,
    P_CONSTANTS_1,
    P_CONSTANT_0,
    P_STATEMENTS_0,
    P_STATEMENTS_1,
    P_STATEMENT_0,
    P_STATEMENT_1,
    P_STATEMENT_2,
    P_STATEMENT_3,
    P_STATEMENT_4,
    P_STATEMENT_5,
    P_STATEMENT_6,
    P_STATEMENT_7,
    P_DST_0,
    P_DST_1,
    P_SRC_0,
    P_SRC_1,
    P_SRC_2,
    P_SRC2_0,
    P_SRC2_1,
    P_REG_0,
    P_REG_1,
    P_REG_2,
    P_OFFSET_0,
    P_OFFSET_1,
    P_OFFSET_2,
    P_OFFSET_3,
    P_OFFSET_4,
    P_OFFSET_5,
    P_OFFSET_6,
    P_OFFSET_7,
    P_OFFSET_8,
    P_OFFSET_9,
    P_OFFSET_10,
    P_OFFSET_11,
    P_OP0_0,
    P_OP0_1,
    P_OP1_0,
    P_OP1_1,
    P_OP2_0,
    P_OP2_1,
    P_OP3_0,
    P_OP3_1,
    P_OP4_0,
    P_OP4_1,
    P_OP5_0,
    P_OP5_1,
    P_OP6_0,
    P_OP6_1,
    P_OP7_0,
    P_OP7_1,
    P_VAL_0,
    P_VAL_1,
    P_VAL_2,
    P_VAL_3,
    P_VAL_4,
    P_VAL_5,
    P_ID,
    P_NUM,
    P_FLT
};

#define Prod(n) \
    CD3DXAssembler::s_pAsm->Production(n)

#define d3dxasm_lex \
    CD3DXAssembler::s_pAsm->Token

#define d3dxasm_error \
    CD3DXAssembler::s_pAsm->YYError

#define lint
#pragma warning(disable : 4242)
#include "shadeasm.h"
#pragma warning(default : 4242)

#define FOURCC_LINE  MAKEFOURCC('L', 'I', 'N', 'E')
#define FOURCC_FILE  MAKEFOURCC('F', 'I', 'L', 'E')
#define FOURCC_TEXT  MAKEFOURCC('T', 'E', 'X', 'T')



//----------------------------------------------------------------------------
// CD3DXAssembler
//----------------------------------------------------------------------------

#define SHADER_VALIDATOR_LOG_ERRORS  0x1

#if 0
typedef HRESULT (WINAPI* LPVALIDATEPIXELSHADER)(DWORD*, D3DCAPS8*, DWORD, char**);
typedef HRESULT (WINAPI* LPVALIDATEVERTEXSHADER)(DWORD*, DWORD*, D3DCAPS8*, DWORD, char**);
#endif

CD3DXAssembler* CD3DXAssembler::s_pAsm = NULL;


CD3DXAssembler::CD3DXAssembler()
{
    s_pAsm = this;

    m_bFail  = FALSE;
    m_cbConst = 0;
    m_cbCode = 0;
	m_pErrorLog = 0;
}


CD3DXAssembler::~CD3DXAssembler()
{
}


HRESULT
CD3DXAssembler::Assemble(LPCVOID pv, UINT cb, LPCSTR pszFile, UINT iLine,
    DWORD dwFlags, LPXGBUFFER *ppConst, LPXGBUFFER *ppCode,
    XD3DXErrorLog* pErrorLog, LPDWORD pShaderType)
{
    HRESULT hr;

    XGBuffer *pConst;
    XGBuffer *pCode;

	m_pErrorLog = pErrorLog;

    if(ppConst)
        *ppConst = NULL;

    if(ppCode)
        *ppCode = NULL;

    if(!pv || !cb)
    {
        DPF(0, "Could not read data from source");
        return D3DERR_INVALIDCALL;
    }

    m_pchSource = (char *) pv;
    m_pchLine   = m_pchSource;
    m_pch       = m_pchSource;
    m_pchLim    = m_pchSource + cb;
    m_iLine     = iLine ? iLine : 1;
    m_iLineNextToken = m_iLine;

    m_bPixel    = FALSE;
    m_bDebug    = dwFlags != 0;
    m_bScreenSpace = FALSE;
    m_dwVersion = 0;


    if(pszFile)
    {
        strcpy(m_szFile, pszFile);
    }
    else
    {
        m_szFile[0] = 0;
        UpdateDebugText();
    }

    m_iLineDebug = 0;
    m_szFileDebug[0] = 0;



    if(yyparse())
        m_bFail = TRUE;




    // Constants
    if(!m_bFail && ppConst && !m_bPixel)
    {
        hr = XGBufferCreate(m_cbConst, &pConst);
		if(FAILED(hr)){
			return hr;
		}

        BYTE *pbConst = (BYTE *) pConst->GetBufferPointer();

        while(m_cbConst)
        {
            m_cbConst -= sizeof(DWORD);
            m_Const.Pop((DWORD *) (pbConst + m_cbConst));
        }

        *ppConst = pConst;
    }


    // Compiled code
    if(!m_bFail && ppCode)
    {
#ifdef WRITE_COMMENT
        const char szComment[] = "D3DX8 Shader Assembler Version 0.91";
        UINT cdwComment = (sizeof(szComment) + 3) >> 2;
        UINT cbComment = cdwComment << 2;

        UINT cbObj = 3 * sizeof(DWORD) + m_cbCode + cbComment;
#else
		UINT cbObj = 2 * sizeof(DWORD) + m_cbCode; // Version, code, end
#endif

        if(m_bPixel)
            cbObj += m_cbConst;

        hr = XGBufferCreate(cbObj, &pCode);
		if(FAILED(hr)){
			return hr;
		}

        BYTE *pbObj = (BYTE *) pCode->GetBufferPointer();

        *((DWORD *) (pbObj + 0 * sizeof(DWORD))) = m_dwVersion;
#ifdef WRITE_COMMENT
        *((DWORD *) (pbObj + 1 * sizeof(DWORD))) = D3DSHADER_COMMENT(cdwComment);
        memset( pbObj + 2 * sizeof(DWORD), 0x0, cbComment );
        memcpy( pbObj + 2 * sizeof(DWORD), szComment, sizeof(szComment) );
#endif
        *((DWORD *) (pbObj + cbObj - sizeof(DWORD))) = 0x0000FFFF;
        pbObj = pbObj + cbObj - 2 * sizeof(DWORD);

        while(m_cbCode)
        {
            m_Code.Pop((DWORD *) pbObj);

            pbObj  -= sizeof(DWORD);
            m_cbCode -= sizeof(DWORD);
        }

        if(m_bPixel)
        {
            while(m_cbConst)
            {
                m_Const.Pop((DWORD *) pbObj);

                pbObj -= sizeof(DWORD);
                m_cbConst -= sizeof(DWORD);
            }
        }


        *ppCode = pCode;
    }


#if 0 // TODO: Implement this
    if(!m_bFail && !(dwFlags & D3DXASM_SKIPVALIDATION))
    {
        HINSTANCE hD3D8;

        if((hD3D8 = (HINSTANCE) GetModuleHandle("d3d8.dll")) ||
           (hD3D8 = (HINSTANCE) LoadLibrary("d3d8.dll")))
        {
            char *psz = NULL;
            char **ppsz = ppErrors ? &psz : NULL;
            DWORD dwLog = ppErrors ? SHADER_VALIDATOR_LOG_ERRORS : 0;


            if(m_bPixel)
            {
                LPVALIDATEPIXELSHADER pfnPS;

                if((pfnPS = (LPVALIDATEPIXELSHADER) GetProcAddress(hD3D8, "ValidatePixelShader")))
                    hr = pfnPS((DWORD *) pCode->GetBufferPointer(), NULL, dwLog, ppsz);
            }
            else
            {
                LPVALIDATEVERTEXSHADER pfnVS;

                if((pfnVS = (LPVALIDATEVERTEXSHADER) GetProcAddress(hD3D8, "ValidateVertexShader")))
                    hr = pfnVS((DWORD *) pCode->GetBufferPointer(), NULL, NULL, dwLog, ppsz);
            }

            if(FAILED(hr))
                m_bFail = TRUE;


            if(psz)
            {
                m_Errors.Push(psz);

                m_cErrors++;
                m_cbErrors += strlen(psz) + 1;

                HeapFree(GetProcessHeap(), 0, psz);
            }
        }
    }
#endif // if 0
    
    if(m_bFail)
    {
        DPF(0, "Error assembling shader");
        return D3DXERR_INVALIDDATA;
    }

    if(pShaderType){
        DWORD shaderType;
        if(m_bPixel){
            shaderType = SASMT_PIXELSHADER;
        }
        else {
            if(m_bStateShader){
                shaderType = SASMT_VERTEXSTATESHADER;
            }
            else if(m_bWritable){
                shaderType = SASMT_READWRITE_VERTEXSHADER;
            }
            else{
                shaderType = SASMT_VERTEXSHADER;
            }
        }
        *pShaderType = shaderType;
    }

    return S_OK;
}


int
CD3DXAssembler::Token()
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
            m_iLineNextToken++;
        }
        else if(Isspace(ch))
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
                for(; pch < m_pch && !Isspace(*pch); pch++);

                // Skip whitespace
                for(; pch < m_pch && Isspace(*pch); pch++);
                char *pchNum = pch;

                // Line number
                for(; pch < m_pch && !Isspace(*pch); pch++);
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
                    m_iLineNextToken = atoi(sz) - 1; // -1 because we add one when we see the '\n' at the end of the #line directive.
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
            else if(m_pch - pch > 7 && !memcmp(pch, "#pragma", 7))
            {
                pch += 7;
                while(Isspace(*pch) && pch < m_pch){
                    pch++;
                }
                if(m_pch - pch > 11 && !memcmp(pch, "screenspace", 11))
                {
                    m_bScreenSpace = TRUE;
                }
                else {
                    m_iLine = m_iLineNextToken;
                    Error(false, WARNING_ASM_UNKNOWN_PRAGMA, "unknown pragma");
                }
            }
            else
            {
                m_iLine = m_iLineNextToken;
                Error(true, ERROR_ASM_UNSUPPORTED_PREPROCESSOR_DIRECTIVE, "preprocessor directives are not supported.");
            }
        }
        else if((ch == ';') || (ch == '/' && m_pch + 1 < m_pchLim && m_pch[1] == '/'))
        {
            // comment
            for(m_pch++; m_pch < m_pchLim && *m_pch != '\n'; m_pch++);
        }
        else if(isalpha(ch))
        {
            m_iLine = m_iLineNextToken;
            // keyword / identifier
            pch = m_pch;
            for(m_pch++; m_pch < m_pchLim && (isalnum(*m_pch) || '_' == *m_pch
                || ('-' == *m_pch && *pch == 'c' && m_pch == pch + 1) // allow negative c registers to be written as 'c-3'
                ); m_pch++);

            cch = m_pch - pch;

            if(cch >= sizeof(m_szTok))
                cch = sizeof(m_szTok) - 1;

            memcpy(m_szTok, pch, cch);
            m_szTok[cch] = 0;

            if(!lstrcmpiA(m_szTok, "vs" )) return T_VS;
            if(!lstrcmpiA(m_szTok, "xvs" )) return T_XVS;
            if(!lstrcmpiA(m_szTok, "xvsw" )) return T_XVSW;
            if(!lstrcmpiA(m_szTok, "xvss" )) return T_XVSS;
            if(!lstrcmpiA(m_szTok, "ps" )) return T_PS;
            if(!lstrcmpiA(m_szTok, "xps" )) return T_XPS;
            if(!lstrcmpiA(m_szTok, "def")) return T_DEF;

            return DecodeOpcode();
        }
        else if(isdigit(ch))
        {
            m_iLine = m_iLineNextToken;
            // number / float
            pch = m_pch;
            int nToken = T_NUM;

            for(m_pch++; m_pch < m_pchLim; m_pch++)
            {
                if(isdigit(*m_pch))
                    continue;

                if(m_dwVersion && ((*m_pch == '.') || *m_pch == 'e') ||
                   ((*m_pch == '+' || *m_pch == '-') && *(m_pch - 1) == 'e'))
                {
                    nToken = T_FLT;
                    continue;
                }

                break;
            }

            cch = m_pch - pch;

            if(cch >= sizeof(m_szTok))
                cch = sizeof(m_szTok) - 1;

            memcpy(m_szTok, pch, cch);
            m_szTok[cch] = 0;

            if(m_dwVersion && (m_pch < m_pchLim) && (*m_pch == 'f'))
            {
                nToken = T_FLT;
                m_pch++;
            }

            return nToken;
        }
        else
        {
            m_iLine = m_iLineNextToken;
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
CD3DXAssembler::Production(UINT nProduction)
{
    char *psz;
    FLOAT f;
    DWORD dw;
    DWORD dwOpcode, dwShiftSat;
    DWORD dwParam, dwParam2, dwParam3, dwParam4;
    DWORD dwParam5, dwParam6, dwParam7;
    D3DXVECTOR4 v;
    int o1, o2;
    DWORD sum;


    switch(nProduction)
    {
    case P_SHADER_0:
    case P_SHADER_1:
        break;

    case P_VERSION_0:
        m_Number.Pop(&dwParam2);
        m_Number.Pop(&dwParam);

        if((dwParam | dwParam2) & ~0xff)
        {
            Error(true, ERROR_ASM_INVALID_VERSION, "invalid version");
            break;
        }

        m_bPixel    = FALSE;
		m_bXbox		= FALSE;
		m_bWritable = FALSE;
		m_bStateShader = FALSE;
        m_dwVersion = D3DVS_VERSION(dwParam, dwParam2);
        break;

    case P_VERSION_1:
        m_Number.Pop(&dwParam2);
        m_Number.Pop(&dwParam);

        if((dwParam | dwParam2) & ~0xff)
        {
            Error(true, ERROR_ASM_INVALID_VERSION, "invalid version");
            break;
        }

        m_bPixel    = TRUE;
		m_bXbox		= FALSE;
		m_bWritable = FALSE;
		m_bStateShader = FALSE;
        m_dwVersion = D3DPS_VERSION(dwParam, dwParam2);
        break;

    case P_VERSION_XPS:
        m_Number.Pop(&dwParam2);
        m_Number.Pop(&dwParam);

        if((dwParam | dwParam2) & ~0xff)
        {
            Error(true, ERROR_ASM_INVALID_VERSION, "invalid version");
            break;
        }

        m_bPixel    = TRUE;
		m_bXbox		= TRUE;
		m_bWritable = FALSE;
		m_bStateShader = FALSE;
        m_dwVersion = D3DPS_VERSION(dwParam, dwParam2+10);
        break;

    case P_VERSION_XVS: 
        m_Number.Pop(&dwParam2);
        m_Number.Pop(&dwParam);

        if((dwParam | dwParam2) & ~0xff)
        {
            Error(true, ERROR_ASM_INVALID_VERSION, "invalid version");
            break;
        }

        m_bPixel    = FALSE;
		m_bXbox		= TRUE;
		m_bWritable = FALSE;
		m_bStateShader = FALSE;
        m_dwVersion = D3DVS_VERSION(dwParam, dwParam2);
        break;

    case P_VERSION_XVSW:
        m_Number.Pop(&dwParam2);
        m_Number.Pop(&dwParam);

        if((dwParam | dwParam2) & ~0xff)
        {
            Error(true, ERROR_ASM_INVALID_VERSION, "invalid version");
            break;
        }

        m_bPixel    = FALSE;
		m_bXbox		= TRUE;
		m_bWritable = TRUE;
		m_bStateShader = FALSE;
        m_dwVersion = D3DVS_VERSION(dwParam, dwParam2);
        break;

    case P_VERSION_XVSS:
        m_Number.Pop(&dwParam2);
        m_Number.Pop(&dwParam);

        if((dwParam | dwParam2) & ~0xff)
        {
            Error(true, ERROR_ASM_INVALID_VERSION, "invalid version");
            break;
        }

        m_bPixel    = FALSE;
 		m_bXbox		= TRUE;
		m_bWritable = TRUE;
		m_bStateShader = TRUE;
       m_dwVersion = D3DVS_VERSION(dwParam, dwParam2);
        break;

    case P_BODY_0:
    case P_BODY_1:
        break;

    case P_CONSTANTS_0:
    case P_CONSTANTS_1:
        break;

    case P_CONSTANT_0:
        m_Param.Pop(&dwParam);

        m_Float.Pop((DWORD *) &v.w);
        m_Float.Pop((DWORD *) &v.z);
        m_Float.Pop((DWORD *) &v.y);
        m_Float.Pop((DWORD *) &v.x);

        SetConstant(dwParam, &v);
        break;

    case P_STATEMENTS_0:
    case P_STATEMENTS_1:
        break;

    case P_STATEMENT_0:
        m_Opcode.Pop(&dwShiftSat);
        m_Opcode.Pop(&dwOpcode);

        m_Code.Push(dwOpcode);
        m_cbCode += sizeof(DWORD);
        break;

    case P_STATEMENT_1:
        m_Param.Pop(&dwParam);
        m_Opcode.Pop(&dwShiftSat);
        m_Opcode.Pop(&dwOpcode);

        m_Code.Push(dwOpcode);
        m_Code.Push(dwParam | dwShiftSat);
        m_cbCode += 2 * sizeof(DWORD);
        break;

    case P_STATEMENT_2:
        m_Param.Pop(&dwParam2);
        m_Param.Pop(&dwParam);
        m_Opcode.Pop(&dwShiftSat);
        m_Opcode.Pop(&dwOpcode);

        m_Code.Push(dwOpcode);
        m_Code.Push(dwParam | dwShiftSat);
        m_Code.Push(dwParam2);
        m_cbCode += 3 * sizeof(DWORD);
        break;

    case P_STATEMENT_3:
        m_Param.Pop(&dwParam3);
        m_Param.Pop(&dwParam2);
        m_Param.Pop(&dwParam);
        m_Opcode.Pop(&dwShiftSat);
        m_Opcode.Pop(&dwOpcode);

        // Emulate SUB instruction for vertex shaders
        if(!m_bPixel && (D3DSIO_SUB == (dwOpcode & D3DSI_OPCODE_MASK)))
        {
            dwOpcode = (dwOpcode & ~D3DSI_OPCODE_MASK) | D3DSIO_ADD;
            dwParam3 ^= D3DSPSM_NEG;
        }

        m_Code.Push(dwOpcode);
        m_Code.Push(dwParam | dwShiftSat);
        m_Code.Push(dwParam2);
        m_Code.Push(dwParam3);
        m_cbCode += 4 * sizeof(DWORD);
        break;

    case P_STATEMENT_4:
        m_Param.Pop(&dwParam4);
        m_Param.Pop(&dwParam3);
        m_Param.Pop(&dwParam2);
        m_Param.Pop(&dwParam);
        m_Opcode.Pop(&dwShiftSat);
        m_Opcode.Pop(&dwOpcode);

        m_Code.Push(dwOpcode);
        m_Code.Push(dwParam | dwShiftSat);
        m_Code.Push(dwParam2);
        m_Code.Push(dwParam3);
        m_Code.Push(dwParam4);
        m_cbCode += 5 * sizeof(DWORD);
        break;


    case P_STATEMENT_5:
        m_Param.Pop(&dwParam7);
        m_Param.Pop(&dwParam6);
        m_Param.Pop(&dwParam5);
        m_Param.Pop(&dwParam4);
        m_Param.Pop(&dwParam3);
        m_Param.Pop(&dwParam2);
        m_Param.Pop(&dwParam);
        m_Opcode.Pop(&dwShiftSat);
        m_Opcode.Pop(&dwOpcode);

        m_Code.Push(dwOpcode);
        m_Code.Push(dwParam | dwShiftSat);
        m_Code.Push(dwParam2);
        m_Code.Push(dwParam3);
        m_Code.Push(dwParam4);
        m_Code.Push(dwParam5);
        m_Code.Push(dwParam6);
        m_Code.Push(dwParam7);
        m_cbCode += 8 * sizeof(DWORD);
        break;


    case P_STATEMENT_6:
        m_Param.Pop(&dwParam6);
        m_Param.Pop(&dwParam5);
        m_Param.Pop(&dwParam4);
        m_Param.Pop(&dwParam3);
        m_Param.Pop(&dwParam2);
        m_Param.Pop(&dwParam);
        m_Opcode.Pop(&dwShiftSat);
        m_Opcode.Pop(&dwOpcode);

        m_Code.Push(dwOpcode);
        m_Code.Push(dwParam | dwShiftSat);
        m_Code.Push(dwParam2);
        m_Code.Push(dwParam3);
        m_Code.Push(dwParam4);
        m_Code.Push(dwParam5);
        m_Code.Push(dwParam6);
        m_cbCode += 7 * sizeof(DWORD);
        break;


    case P_STATEMENT_7:
        m_Param.Pop(&dwParam7);
        m_Param.Pop(&dwParam6);
        m_Param.Pop(&dwParam5);
        m_Param.Pop(&dwParam4);
        m_Param.Pop(&dwParam3);
        m_Param.Pop(&dwParam2);
        m_Param.Pop(&dwParam);
        m_Opcode.Pop(&dwShiftSat);
        m_Opcode.Pop(&dwOpcode);

        m_Code.Push(dwOpcode);
        m_Code.Push(dwParam | dwShiftSat);
        m_Code.Push(dwParam2);
        m_Code.Push(dwParam3);
        m_Code.Push(dwParam4);
        m_Code.Push(dwParam5);
        m_Code.Push(dwParam6);
        m_Code.Push(dwParam7);
        m_cbCode += 8 * sizeof(DWORD);
        break;


    case P_DST_0:
        m_Param.Pop(&dwParam);
        dwParam |= D3DSP_WRITEMASK_ALL;
        ValidateDstRegister(dwParam);
        m_Param.Push(dwParam);
        break;

    case P_DST_1:
        m_Id.Pop(&psz);
        m_Param.Pop(&dwParam);
        dwParam = (dwParam & ~D3DSP_WRITEMASK_ALL) | DecodeMask(psz);
        ValidateDstRegister(dwParam);
        m_Param.Push(dwParam);
        delete [] psz;
        break;

    case P_SRC_0:
        m_Param.Pop(&dwParam);
        ValidateSrcRegister(dwParam);
        m_Param.Push(dwParam);
        break;

    case P_SRC_1:
        m_Param.Pop(&dwParam);

        switch(dwParam & D3DSP_SRCMOD_MASK)
        {
        case D3DSPSM_NONE:
            dwParam = (dwParam & ~D3DSP_SRCMOD_MASK) | D3DSPSM_NEG;
            break;

        case D3DSPSM_BIAS:
            dwParam = (dwParam & ~D3DSP_SRCMOD_MASK) | D3DSPSM_BIASNEG;
            break;

        case D3DSPSM_SIGN:
            dwParam = (dwParam & ~D3DSP_SRCMOD_MASK) | D3DSPSM_SIGNNEG;
            break;
        }

        ValidateSrcRegister(dwParam);
        m_Param.Push(dwParam);
        break;

    case P_SRC_2:
        m_Number.Pop(&dw);
        m_Param.Pop(&dwParam);

        if(!m_bPixel)
            Error(true, ERROR_ASM_COMPLEMENT_NOT_SUPPORTED, "complement not supported in vertex shaders");
        else if(D3DSPSM_NONE != (dwParam & D3DSP_SRCMOD_MASK))
            Error(true, ERROR_ASM_COMPLEMENT_CANNOT_BE_USED_WITH_OTHER_MODIFIERS, "complement cannot be used with other modifiers");

        dwParam = (dwParam & ~D3DSP_SRCMOD_MASK) | D3DSPSM_COMP;
        ValidateSrcRegister(dwParam);
        m_Param.Push(dwParam);
        break;

    case P_SRC2_0:
        m_Param.Pop(&dwParam);
        m_Param.Push((dwParam & ~D3DVS_SWIZZLE_MASK) | D3DVS_NOSWIZZLE);
        break;

    case P_SRC2_1:
        m_Id.Pop(&psz);
        m_Param.Pop(&dwParam);
        m_Param.Push((dwParam & ~D3DVS_SWIZZLE_MASK) | DecodeSwizzle(psz));
        delete [] psz;
        break;

    case P_REG_0:
        m_Id.Pop(&psz);
        m_Param.Push(DecodeRegister(psz, 0, FALSE) | (1 << 31));
        delete [] psz;
        break;

    case P_REG_1:
        m_Id.Pop(&psz);
        m_Param.Pop(&dw);
        m_Param.Push(DecodeRegister(psz, dw, TRUE) | (1 << 31));
        delete [] psz;
        break;

    case P_REG_2:
        char combine[1024], *psz2;
        m_Id.Pop(&psz2);
        m_Param.Pop(&dw);
        m_Id.Pop(&psz);
        sprintf(combine, "%s%d_%s", psz, dw, psz2);
        m_Param.Push(DecodeRegister(combine, 0, FALSE) | (1 << 31));
        delete [] psz;
        break;

    case P_OFFSET_0:
        m_Param.Pop(&dw);

        if(m_bPixel)
            Error(true, ERROR_ASM_SYNTAX_ERROR, "syntax error");
        else if(dw != (D3DSPR_ADDR | D3DVS_X_X | D3DVS_Y_X | D3DVS_Z_X | D3DVS_W_X | (1 << 31)))
            Error(true, ERROR_ASM_ILLEGAL_REGISTER_IN_INDEX_EXPRESSION, "illegal register in index expression");

        m_Param.Push(D3DVS_ADDRMODE_RELATIVE);
        break;

    case P_OFFSET_1:
        m_Number.Pop(&dw);
        m_Param.Push(dw & D3DSP_REGNUM_MASK);
        break;

    case P_OFFSET_2:
        m_Param.Pop(&dwParam);
        m_Param.Pop(&dw);

        if(m_bPixel)
            Error(true, ERROR_ASM_SYNTAX_ERROR, "syntax error");
        else if(dw != (D3DSPR_ADDR | D3DVS_X_X | D3DVS_Y_X | D3DVS_Z_X | D3DVS_W_X | (1 << 31)))
            Error(true, ERROR_ASM_ILLEGAL_REGISTER_IN_INDEX_EXPRESSION, "illegal register in index expression");
        else if(dwParam & D3DVS_ADDRMODE_RELATIVE)
            Error(true, ERROR_ASM_A0_X_CAN_ONLY_APPEAR_ONCE_IN_AN_INDEX_EXPRESSION, "a0.x can only appear once in an index expression");

        m_Param.Push(dwParam | D3DVS_ADDRMODE_RELATIVE);
        break;

    case P_OFFSET_3:
        m_Number.Pop(&dw);
        m_Param.Pop(&dwParam);
        m_Param.Push((dwParam & ~D3DSP_REGNUM_MASK) | ((dwParam + dw) & D3DSP_REGNUM_MASK));
        break;

    case P_OFFSET_4:
		// Just like P_OFFSET_2, but with a negative offset.
		// example: c[A0.x-12]
        m_Param.Pop(&dwParam);
        m_Param.Pop(&dw);

        if(m_bPixel)
            Error(true, ERROR_ASM_SYNTAX_ERROR, "syntax error");
        else if(dw != (D3DSPR_ADDR | D3DVS_X_X | D3DVS_Y_X | D3DVS_Z_X | D3DVS_W_X | (1 << 31)))
            Error(true, ERROR_ASM_ILLEGAL_REGISTER_IN_INDEX_EXPRESSION, "illegal register in index expression");
        else if(dwParam & D3DVS_ADDRMODE_RELATIVE)
            Error(true, ERROR_ASM_A0_X_CAN_ONLY_APPEAR_ONCE_IN_AN_INDEX_EXPRESSION, "a0.x can only appear once in an index expression");

		// Negate existing offset
		{
			int offset = (dwParam & D3DSP_REGNUM_MASK);
			offset = -offset;
			dwParam = (dwParam & ~D3DSP_REGNUM_MASK)
				| (offset & D3DSP_REGNUM_MASK);
		}

        m_Param.Push(dwParam | D3DVS_ADDRMODE_RELATIVE);
        break;

    case P_OFFSET_5:
        // Like 1, but with a negative offset
        // example: c[-5]
        m_Number.Pop(&dw);
		// Negate existing offset
		{
			int offset = (dw & D3DSP_REGNUM_MASK);
			offset = -offset;
			dw = (dw & ~D3DSP_REGNUM_MASK)
				| (offset & D3DSP_REGNUM_MASK);
		}
        m_Param.Push(dw & D3DSP_REGNUM_MASK);
        break;

    case P_OFFSET_6:
        break;

    case P_OFFSET_7:
        m_Param.Pop(&dwParam);
        m_Param.Pop(&dw);

        if((dwParam & D3DVS_ADDRMODE_RELATIVE) &&
           (dw      & D3DVS_ADDRMODE_RELATIVE))
            Error(true, ERROR_ASM_A0_X_CAN_ONLY_APPEAR_ONCE_IN_AN_INDEX_EXPRESSION, "a0.x can only appear once in an index expression");
        
        // find sum of offsets
        o1 = dwParam & D3DSP_REGNUM_MASK;
        if(o1 & 0x800)
            o1 |= 0xfffff000;

        o2 = dw & D3DSP_REGNUM_MASK;
        if(o2 & 0x800)
            o2 |= 0xfffff000;

        sum = (o1 + o2) & D3DSP_REGNUM_MASK;
        if((dwParam & D3DVS_ADDRMODE_RELATIVE) ||
           (dw      & D3DVS_ADDRMODE_RELATIVE))
            sum |= D3DVS_ADDRMODE_RELATIVE;

        m_Param.Push(sum);
        break;

    case P_OFFSET_8:
        m_Param.Pop(&dwParam);
        m_Param.Pop(&dw);

        if((dwParam & D3DVS_ADDRMODE_RELATIVE) &&
           (dw      & D3DVS_ADDRMODE_RELATIVE))
            Error(true, ERROR_ASM_A0_X_CAN_ONLY_APPEAR_ONCE_IN_AN_INDEX_EXPRESSION, "a0.x can only appear once in an index expression");
        
        if(dwParam & D3DVS_ADDRMODE_RELATIVE)
            Error(true, ERROR_ASM_A0_X_CANT_BE_NEGATED, "a0.x can't be negated");
        
        // find sum of offsets
        int o1, o2;
        o1 = dwParam & D3DSP_REGNUM_MASK;
        if(o1 & 0x800)
            o1 |= 0xfffff000;

        o2 = dw & D3DSP_REGNUM_MASK;
        if(o2 & 0x800)
            o2 |= 0xfffff000;

        DWORD sum;
        sum = (o2 - o1) & D3DSP_REGNUM_MASK;
        if((dwParam & D3DVS_ADDRMODE_RELATIVE) ||
           (dw      & D3DVS_ADDRMODE_RELATIVE))
            sum |= D3DVS_ADDRMODE_RELATIVE;

        m_Param.Push(sum);
        break;

    case P_OFFSET_9:
        m_Number.Pop(&dw);
        m_Param.Pop(&dwParam);
        if(dwParam & D3DVS_ADDRMODE_RELATIVE)
            Error(true, ERROR_ASM_A0_X_CANT_BE_NEGATED, "a0.x can't be negated");
        
        m_Param.Push((dwParam & ~D3DSP_REGNUM_MASK) | ((dw - dwParam) & D3DSP_REGNUM_MASK));
        break;

    case P_OFFSET_10:
        m_Number.Pop(&dw);
        m_Param.Pop(&dwParam);
        m_Param.Push((dwParam & ~D3DSP_REGNUM_MASK) | ((-(int)dw + dwParam) & D3DSP_REGNUM_MASK));
        break;

    case P_OFFSET_11:
        m_Number.Pop(&dw);
        m_Param.Pop(&dwParam);
        if(dwParam & D3DVS_ADDRMODE_RELATIVE)
            Error(true, ERROR_ASM_A0_X_CANT_BE_NEGATED, "a0.x can't be negated");
        
        m_Param.Push((dwParam & ~D3DSP_REGNUM_MASK) | ((-(int)dw - dwParam) & D3DSP_REGNUM_MASK));
        break;

    case P_OP0_0:
    case P_OP1_0:
    case P_OP2_0:
    case P_OP3_0:
    case P_OP4_0:
    case P_OP5_0:
    case P_OP6_0:
    case P_OP7_0:
        m_Opcode.Push(m_dwOpcode);
        m_Opcode.Push(m_dwShiftSat);

        UpdateDebugFileLine();
        break;

    case P_OP0_1:
    case P_OP1_1:
    case P_OP2_1:
    case P_OP3_1:
    case P_OP4_1:
    case P_OP5_1:
    case P_OP6_1:
    case P_OP7_1:
        if(!m_bPixel && !m_bXbox){
            Error(true, ERROR_ASM_INVALID_INSTRUCTION_COMBINATION, "Instruction combination is not allowed in a vs shader. Use xvs instead.");
            m_Opcode.Push(m_dwOpcode);
        }
        else {
            m_Opcode.Push(m_dwOpcode | D3DSI_COISSUE);
        }
        m_Opcode.Push(m_dwShiftSat);

        UpdateDebugFileLine();
        break;

    case P_VAL_0:
    case P_VAL_1:
        m_Number.Pop(&dw);
        f = (FLOAT) dw;
        m_Float.Push(*((DWORD *) &f));
        break;

    case P_VAL_2:
        m_Number.Pop(&dw);
        f = -((FLOAT) dw);
        m_Float.Push(*((DWORD *) &f));
        break;

    case P_VAL_3:
    case P_VAL_4:
        break;

    case P_VAL_5:
        m_Float.Pop((DWORD *) &f);
        f = -f;
        m_Float.Push(*((DWORD *) &f));
        break;

    case P_ID:
        m_Id.Push(m_szTok);
        break;

    case P_NUM:
        m_Number.Push(atoi(m_szTok));
        break;

    case P_FLT:
        sscanf(m_szTok, "%f", &f);
        m_Float.Push(*((DWORD *) &f));
        break;
    }


    if(FAILED(m_Id    .GetLastError()) ||
       FAILED(m_Number.GetLastError()) ||
       FAILED(m_Param .GetLastError()) ||
       FAILED(m_Code  .GetLastError()) ||
       FAILED(m_Float .GetLastError()))
    {
        Error(true, ERROR_ASM_INTERNAL_ASSEMBLER_ERROR, "Internal assembler error! %d", __LINE__);
    }
}


void
CD3DXAssembler::YYError(char *szFormat, ...)
{
	va_list ap;
	va_start(ap, szFormat);
    // Special case a missing version statement
    if(m_dwVersion == 0){
        Error(true, ERROR_ASM_EXPECTED_VERSION, "Expected a version statement ('ps', 'xps', 'vs', 'xvs', 'xvss', or 'xvsw').");
        return;
    }
	Error2(true, ERROR_ASM_SYNTAX_ERROR, szFormat, ap);
	va_end(ap);
}

void
CD3DXAssembler::Error(bool error, DWORD code, LPCSTR szFormat, ...)
{
	va_list ap;
	va_start(ap, szFormat);
	Error2(error, code, szFormat, ap);
	va_end(ap);
}

void
CD3DXAssembler::Error2(bool error, DWORD code, LPCSTR szFormat, va_list list)
{
	if(m_pErrorLog){
		char szA[512];

		_vsnprintf(szA, sizeof(szA), szFormat, list);
		szA[sizeof(szA)-1] = '\0';

		m_pErrorLog->Log(error, code, m_szFile, m_iLine, szA);
	}
    m_bFail = TRUE;
}

int
CD3DXAssembler::DecodeOpcode()
{
    char sz[16];

    int   nTok        = T_ID;
    DWORD dwOpcode    = 0;
    DWORD dwShiftSat  = 0;

    char *psz         = m_szTok;
    char *pszOp       = NULL;
    char *pszOpLim    = NULL;
    char *pszShift    = NULL;
    char *pszShiftLim = NULL;


    // Parse string
    pszOp = psz;
    for(; *psz && *psz != '_'; psz++);
    pszOpLim = psz;

    if(*psz)
    {
        pszShift = ++psz;
        for(; *psz && *psz != '_'; psz++);
        pszShiftLim = psz;
    }

    if(*psz)
        return T_ID;    // only one destination modifier allowed

    // Validate opcode
    SIZE_T cchOp = pszOpLim - pszOp;

    if(cchOp > sizeof(sz) - 1)
        return T_ID;

    memcpy(sz, pszOp, cchOp);
    sz[cchOp] = 0;


    // Common opodes
    if     (!lstrcmpiA(sz, "nop"))  { dwOpcode = D3DSIO_NOP;  nTok = T_OP0; }
    else if(!lstrcmpiA(sz, "mov"))  { dwOpcode = D3DSIO_MOV;  nTok = T_OP2; }
    else if(!lstrcmpiA(sz, "add"))  { dwOpcode = D3DSIO_ADD;  nTok = T_OP3; }
    else if(!lstrcmpiA(sz, "sub"))  { dwOpcode = D3DSIO_SUB;  nTok = T_OP3; }
    else if(!lstrcmpiA(sz, "mad"))  { dwOpcode = D3DSIO_MAD;  nTok = T_OP4; }
    else if(!lstrcmpiA(sz, "mul"))  { dwOpcode = D3DSIO_MUL;  nTok = T_OP3; }
    else if(!lstrcmpiA(sz, "dp3"))  { dwOpcode = D3DSIO_DP3;  nTok = T_OP3; }
    else if(!lstrcmpiA(sz, "dp4"))  { dwOpcode = D3DSIO_DP4;  nTok = T_OP3; }


    // Pixel shader opcodes
    if((T_ID == nTok) && m_bPixel)
    {
        if     (!lstrcmpiA(sz, "lrp"         ))  { dwOpcode = D3DSIO_LRP;          nTok = T_OP4; }
        else if(!lstrcmpiA(sz, "cnd"         ))  { dwOpcode = D3DSIO_CND;          nTok = T_OP4; }
        else if(!lstrcmpiA(sz, "texcoord"    ))  { dwOpcode = D3DSIO_TEXCOORD;     nTok = T_OP1; }
        else if(!lstrcmpiA(sz, "texkill"     ))  { dwOpcode = D3DSIO_TEXKILL;      nTok = T_OP1; }
        else if(!lstrcmpiA(sz, "tex"         ))  { dwOpcode = D3DSIO_TEX;          nTok = T_OP1; }
        else if(!lstrcmpiA(sz, "texbem"      ))  { dwOpcode = D3DSIO_TEXBEM;       nTok = T_OP2; }
        else if(!lstrcmpiA(sz, "texbeml"     ))  { dwOpcode = D3DSIO_TEXBEML;      nTok = T_OP2; }
        else if(!lstrcmpiA(sz, "texreg2ar"   ))  { dwOpcode = D3DSIO_TEXREG2AR;    nTok = T_OP2; }
        else if(!lstrcmpiA(sz, "texreg2gb"   ))  { dwOpcode = D3DSIO_TEXREG2GB;    nTok = T_OP2; }
        else if(!lstrcmpiA(sz, "texm3x2pad"  ))  { dwOpcode = D3DSIO_TEXM3x2PAD;   nTok = T_OP2; }
        else if(!lstrcmpiA(sz, "texm3x2tex"  ))  { dwOpcode = D3DSIO_TEXM3x2TEX;   nTok = T_OP2; }
        else if(!lstrcmpiA(sz, "texm3x3pad"  ))  { dwOpcode = D3DSIO_TEXM3x3PAD;   nTok = T_OP2; }
        else if(!lstrcmpiA(sz, "texm3x3tex"  ))  { dwOpcode = D3DSIO_TEXM3x3TEX;   nTok = T_OP2; }
        else if(!lstrcmpiA(sz, "texm3x3diff" ))  { dwOpcode = D3DSIO_TEXM3x3DIFF;  nTok = T_OP2; }
        else if(!lstrcmpiA(sz, "texm3x3spec" ))  { dwOpcode = D3DSIO_TEXM3x3SPEC;  nTok = T_OP3; }
        else if(!lstrcmpiA(sz, "texm3x3vspec"))  { dwOpcode = D3DSIO_TEXM3x3VSPEC; nTok = T_OP2; }
#ifdef XBOX_EXTENSIONS
        else if(!lstrcmpiA(sz, "xmma" )) { dwOpcode = D3DSIO_XMMA;  nTok = T_OP7; }
        else if(!lstrcmpiA(sz, "xmmc" )) { dwOpcode = D3DSIO_XMMC;  nTok = T_OP7; }
        else if(!lstrcmpiA(sz, "xdm"  )) { dwOpcode = D3DSIO_XDM;   nTok = T_OP6; }
        else if(!lstrcmpiA(sz, "xdd"  )) { dwOpcode = D3DSIO_XDD;   nTok = T_OP6; }
        else if(!lstrcmpiA(sz, "xfc"  )) { dwOpcode = D3DSIO_XFC;   nTok = T_OP5; }
        else if(!lstrcmpiA(sz, "texm3x2depth"))  { dwOpcode = D3DSIO_TEXM3x2DEPTH; nTok = T_OP2; }
        else if(!lstrcmpiA(sz, "texbrdf"))       { dwOpcode = D3DSIO_TEXBRDF;      nTok = T_OP1; }
#endif
#if 0 // not implemented for now
        else if(!lstrcmpiA(sz, "texdp3"))        { dwOpcode = D3DSIO_TEXDP3;       nTok = T_OP2; }
        else if(!lstrcmpiA(sz, "texreg2rgb"))    { dwOpcode = D3DSIO_TEXREG2RGB;   nTok = T_OP2; }
#endif // 0
    }


    // Vertex shader opcodes
    if((T_ID == nTok) && !m_bPixel)
    {
        if     (!lstrcmpiA(sz, "rcp"))  { dwOpcode = D3DSIO_RCP;  nTok = T_OP2; }
        else if(!lstrcmpiA(sz, "rsq"))  { dwOpcode = D3DSIO_RSQ;  nTok = T_OP2; }
        else if(!lstrcmpiA(sz, "min"))  { dwOpcode = D3DSIO_MIN;  nTok = T_OP3; }
        else if(!lstrcmpiA(sz, "max"))  { dwOpcode = D3DSIO_MAX;  nTok = T_OP3; }
        else if(!lstrcmpiA(sz, "slt"))  { dwOpcode = D3DSIO_SLT;  nTok = T_OP3; }
        else if(!lstrcmpiA(sz, "sge"))  { dwOpcode = D3DSIO_SGE;  nTok = T_OP3; }
        else if(!lstrcmpiA(sz, "exp"))  { dwOpcode = D3DSIO_EXP;  nTok = T_OP2; }
        else if(!lstrcmpiA(sz, "log"))  { dwOpcode = D3DSIO_LOG;  nTok = T_OP2; }
        else if(!lstrcmpiA(sz, "lit"))  { dwOpcode = D3DSIO_LIT;  nTok = T_OP2; }
        else if(!lstrcmpiA(sz, "dst"))  { dwOpcode = D3DSIO_DST;  nTok = T_OP3; }
        else if(!lstrcmpiA(sz, "frc"))  { dwOpcode = D3DSIO_FRC;  nTok = T_OP2; }
        else if(!lstrcmpiA(sz, "m4x4")) { dwOpcode = D3DSIO_M4x4; nTok = T_OP3; }
        else if(!lstrcmpiA(sz, "m4x3")) { dwOpcode = D3DSIO_M4x3; nTok = T_OP3; }
        else if(!lstrcmpiA(sz, "m3x4")) { dwOpcode = D3DSIO_M3x4; nTok = T_OP3; }
        else if(!lstrcmpiA(sz, "m3x3")) { dwOpcode = D3DSIO_M3x3; nTok = T_OP3; }
        else if(!lstrcmpiA(sz, "m3x2")) { dwOpcode = D3DSIO_M3x2; nTok = T_OP3; }
        else if(!lstrcmpiA(sz, "expp")) { dwOpcode = D3DSIO_EXPP; nTok = T_OP2; }
        else if(!lstrcmpiA(sz, "logp")) { dwOpcode = D3DSIO_LOGP; nTok = T_OP2; }
#ifdef XBOX_EXTENSIONS
		else if(m_bXbox){
			if(!lstrcmpiA(sz, "dph")) { dwOpcode = D3DSIO_DPH; nTok = T_OP3; }
			else if(!lstrcmpiA(sz, "rcc")) { dwOpcode = D3DSIO_RCC; nTok = T_OP2; }
		}
#endif // XBOX_EXTENSIONS
	}

    if(T_ID == nTok)
        return T_ID;



    // Validate shift
    if(pszShift)
    {
        if(m_bPixel)
        {
            SIZE_T cchShift = pszShiftLim - pszShift;

            if(cchShift > sizeof(sz) - 1)
                return T_ID;

            memcpy(sz, pszShift, cchShift);
            sz[cchShift] = 0;


            if     (!lstrcmpiA(sz, "x4")) dwShiftSat = (( 2) << D3DSP_DSTSHIFT_SHIFT) & D3DSP_DSTSHIFT_MASK;
            else if(!lstrcmpiA(sz, "x2")) dwShiftSat = (( 1) << D3DSP_DSTSHIFT_SHIFT) & D3DSP_DSTSHIFT_MASK;
            else if(!lstrcmpiA(sz, "d2")) dwShiftSat = ((-1) << D3DSP_DSTSHIFT_SHIFT) & D3DSP_DSTSHIFT_MASK;
            else if(!lstrcmpiA(sz, "bias")) dwShiftSat = D3DSPDM_BIAS;
            else if(!lstrcmpiA(sz, "bx2")) 
            {
                dwShiftSat = (( 1) << D3DSP_DSTSHIFT_SHIFT) & D3DSP_DSTSHIFT_MASK;
                dwShiftSat |= D3DSPDM_BIAS;
            }
            else return T_ID;
        }
        else
        {
            return T_ID;
        }
    }
    
    m_dwOpcode   = dwOpcode;
    m_dwShiftSat = dwShiftSat;

    return nTok;
}



DWORD
CD3DXAssembler::DecodeMask(char *psz)
{
    DWORD dwMask  = 0;
    DWORD dwShift = 0;
    DWORD dwComp;
    DWORD dwCompLast;

    if(!psz || !*psz)
        return D3DSP_WRITEMASK_ALL;

    for(char *pszT = psz; *pszT; pszT++)
    {
        switch(*pszT)
        {
        case 'x':
        case 'r':
            dwComp = 0;
            dwMask |= D3DSP_WRITEMASK_0;
            break;

        case 'y':
        case 'g':
            dwComp = 1;
            dwMask |= D3DSP_WRITEMASK_1;
            break;

        case 'z':
        case 'b':
            dwComp = 2;
            dwMask |= D3DSP_WRITEMASK_2;
            break;

        case 'w':
        case 'a':
            dwComp = 3;
            dwMask |= D3DSP_WRITEMASK_3;
            break;

        default:
            goto LFail;
        }


        if(pszT != psz && dwComp <= dwCompLast)
            goto LFail;

        dwCompLast = dwComp;
    }

    return dwMask;

LFail:
    Error(true, ERROR_ASM_INVALID_MASK, "invalid mask '%s'", psz);
    m_bFail = TRUE;
    return 0;
}



DWORD
CD3DXAssembler::DecodeSwizzle(char *psz)
{
    DWORD dwSrc = 0;
    DWORD dwDst = 0;
    DWORD dwSwizzle = 0;
    char *pszT = psz;

    if(!psz || !*psz)
        return D3DVS_NOSWIZZLE;

    for(dwDst = 0; dwDst < 4; dwDst++)
    {
//        dwSrc = dwDst;  // default is no swizzle
        if(*pszT)
        {
            switch(*pszT)
            {
            case 'x':
            case 'r':
                dwSrc = 0;
                break;

            case 'y':
            case 'g':
                dwSrc = 1;
                break;

            case 'z':
            case 'b':
                dwSrc = 2;
                break;

            case 'w':
            case 'a':
                dwSrc = 3;
                break;

            default:
                goto LFail;
            }

            pszT++;
        }

        dwSwizzle |= dwSrc << (D3DVS_SWIZZLE_SHIFT + 2 * dwDst);
    }

    if(*pszT)
        goto LFail;

    return dwSwizzle;

LFail:
    Error(true, ERROR_ASM_INVALID_SWIZZLE, "invalid swizzle '%s'", psz);
    m_bFail = TRUE;
    return 0;
}




DWORD
CD3DXAssembler::DecodeRegister(char *psz, DWORD dwAddr, BOOL bIndex)
{
    char chNum;
    char *pszNum;
    char *pszEnd;

    for(pszNum = psz;    *pszNum && isalpha(*pszNum); pszNum++);
    for(pszEnd = pszNum;
        *pszEnd && (isdigit(*pszEnd)
            || (*pszEnd == '-' && *psz == 'c' && pszEnd == pszNum) // Allow c-3
            );
        pszEnd++);

    BOOL bNum = *pszNum != 0;
    DWORD dwBias = 0;


    if(m_bPixel && ('_' == *pszEnd))
    {
        if(!lstrcmpiA(pszEnd, "_bias"))
        {
            dwBias = D3DSPSM_BIAS;
            pszEnd += 5;
        }
        else if(!lstrcmpiA(pszEnd, "_bx2") || !lstrcmpiA(pszEnd, "_sgn"))
        {
            dwBias = D3DSPSM_SIGN;
            pszEnd += 4;
        }
        else if(!lstrcmpiA(pszEnd, "_sat"))
        {
            dwBias = D3DSPSM_SAT;
            pszEnd += 4;
        }
        // handle dot-mapping modifiers for texture stage inputs
        else if(!lstrcmpiA(pszEnd, "_sign1"))
        {
            dwBias = D3DSPSM_SIGN;
            pszEnd += 5;
        }
        else if(!lstrcmpiA(pszEnd, "_sign2"))
        {
            dwBias = D3DSPSM_NEG;
            pszEnd += 6;
        }
        else if(!lstrcmpiA(pszEnd, "_sign3"))
        {
            dwBias = D3DSPSM_BIAS;
            pszEnd += 6;
        }
        else if(!lstrcmpiA(pszEnd, "_hl"))
        {
            dwBias = D3DSPSM_BIASNEG;
            pszEnd += 3;
        }
        else if(!lstrcmpiA(pszEnd, "_hemi1"))
        {
            dwBias = D3DSPSM_SIGNNEG;
            pszEnd += 6;
        }
        else if(!lstrcmpiA(pszEnd, "_hemi2"))
        {
            dwBias = D3DSPSM_COMP;
            pszEnd += 6;
        }
        else if(!lstrcmpiA(pszEnd, "_hemi3"))
        {
            dwBias = D3DSPSM_SAT;
            pszEnd += 6;
        }
    }


    if(!*pszEnd)
    {
        if(!bIndex)
        {
            int iNum = atoi(pszNum);
            dwAddr = iNum & D3DSP_REGNUM_MASK;
            chNum = *pszNum;
            *pszNum = 0;

            if(m_bPixel)
            {
                if(!lstrcmpiA(psz, "v") && bNum)
                {
                    if(0 <= iNum && iNum < D3DPS_INPUTREG_MAX_DX8)
                        return D3DSPR_INPUT | dwAddr | dwBias;
                }
                else if(!lstrcmpiA(psz, "r") && bNum)
                {
                    if(0 <= iNum && iNum < D3DPS_TEMPREG_MAX_DX8)
                        return D3DSPR_TEMP | dwAddr | dwBias;
                }
                else if(!lstrcmpiA(psz, "c") && bNum)
                {
                    if(0 <= iNum && iNum < D3DPS_CONSTREG_MAX_DX8)
                        return D3DSPR_CONST | dwAddr | dwBias;
                }
                else if(!lstrcmpiA(psz, "t") && bNum)
                {
                    if(0 <= iNum && iNum < D3DPS_TEXTUREREG_MAX_DX8)
                        return D3DSPR_TEXTURE | dwAddr | dwBias;
                }
                else if((!lstrcmpiA(psz, "zero")) || (!lstrcmpiA(psz, "discard")))
                {
                    return D3DSPR_TEMP | 2 | dwBias;    // zero/discard is R2
                }
                else if(!lstrcmpiA(psz, "fog"))
                {
                    return D3DSPR_TEMP | 3 | dwBias;    // fog is R3
                }
                else if(!lstrcmpiA(psz, "prod"))
                {
                    return D3DSPR_INPUT | 3 | dwBias;    // prod is V3
                }
                else if(!lstrcmpiA(psz, "sum"))
                {
                    return D3DSPR_INPUT | 2 | dwBias;    // sum is V2
                }
            }
            else
            {
                if(!lstrcmpiA(psz, "oPos") && !bNum)
                {
                    return D3DSPR_RASTOUT | D3DSRO_POSITION;
                }
                else if(!lstrcmpiA(psz, "oPts") && !bNum)
                {
                    return D3DSPR_RASTOUT | D3DSRO_POINT_SIZE;
                }
                else if(!lstrcmpiA(psz, "oFog") && !bNum)
                {
                    return D3DSPR_RASTOUT | D3DSRO_FOG;
                }
                else if(!lstrcmpiA(psz, "v") && bNum)
                {
                    if(0 <= iNum && iNum < D3DVS_INPUTREG_MAX_V1_1)
                        return D3DSPR_INPUT | dwAddr;
                }
                else if(!lstrcmpiA(psz, "r") && bNum)
                {
                    if(0 <= iNum && iNum < (m_bXbox ? 13 : D3DVS_TEMPREG_MAX_V1_1))
                        return D3DSPR_TEMP | dwAddr;
                }
                else if(!lstrcmpiA(psz, "c") && bNum)
                {
                    // Limit constants to 0..191 for normal shaders, -192..191 for xbox shaders
 					int cregMin = m_bXbox ? -192 : 0;
                    int cregMax = 192;
                    if(cregMin <= iNum && iNum < cregMax) {
                        return D3DSPR_CONST | dwAddr;
                    }
                }
                else if(!lstrcmpiA(psz, "a") && bNum)
                {
                    if(0 <= iNum && iNum < D3DVS_ADDRREG_MAX_V1_1)
                        return D3DSPR_ADDR | dwAddr;
                }
                else if(!lstrcmpiA(psz, "oD") && bNum)
                {
					int dwOutRegMax = m_bXbox ? 2 : D3DVS_ATTROUTREG_MAX_V1_1;
                    if(0 <= iNum && iNum < dwOutRegMax)
                        return D3DSPR_ATTROUT | dwAddr;
                }
                else if(!lstrcmpiA(psz, "oB") && bNum)
                {
					int dwOutRegMax = m_bXbox ? 2 : 0;
                    if(0 <= iNum && iNum < dwOutRegMax)
                        return D3DSPR_ATTROUT | 0x100 | dwAddr;
                }
                else if(!lstrcmpiA(psz, "oT") && bNum)
                {
                    if(0 <= iNum && iNum < D3DVS_TCRDOUTREG_MAX_V1_1)
                        return D3DSPR_TEXCRDOUT | dwAddr;
                }
            }

            *pszNum = chNum;
        }
        else
        {
            if(m_bPixel)
            {
                if(!lstrcmpiA(psz, "c") && !bNum)
                {
                    if((dwAddr & D3DSP_REGNUM_MASK) < D3DPS_CONSTREG_MAX_DX8)
                        return D3DSPR_CONST | dwAddr | dwBias;
                }
            }
            else
            {
                if(!lstrcmpiA(psz, "c") && !bNum)
                    return D3DSPR_CONST | dwAddr;
            }
        }
    }

    if(!bIndex)
    {
        Error(true, ERROR_ASM_INVALID_REGISTER, "invalid register '%s'", psz);
    }
    else
    {
        if(dwAddr & D3DVS_ADDRMODE_RELATIVE)
            Error(true, ERROR_ASM_INVALID_REGISTER, "invalid register '%s[a0.x + %d]'", psz, dwAddr & D3DSP_REGNUM_MASK);
        else
            Error(true, ERROR_ASM_INVALID_REGISTER, "invalid register '%s[%d]'", psz, dwAddr & D3DSP_REGNUM_MASK);
    }

    m_bFail = TRUE;
    return 0;
}


void
CD3DXAssembler::ValidateDstRegister(DWORD dwReg)
{
    if(dwReg & D3DSP_SRCMOD_MASK)
    {
        Error(true, ERROR_ASM_SOURCE_MODIFIERS_NOT_ALLOWED_ON_DESTINATION_REGISTERS, "source modifiers are not allowed on destination registers");
        goto LFail;
    }


    if(m_bPixel)
    {
        switch(dwReg & D3DSP_REGTYPE_MASK)
        {
        case D3DSPR_TEMP:
        case D3DSPR_INPUT:
        case D3DSPR_TEXTURE:
            break;

        default:
            Error(true, ERROR_ASM_INVALID_DESTINATION_REGISTER, "invalid destination register");
            goto LFail;
        }

        // Only valid masks: RGBA, RGB, A
        switch(dwReg & D3DSP_WRITEMASK_ALL)
        {
        case D3DSP_WRITEMASK_0 | D3DSP_WRITEMASK_1 | D3DSP_WRITEMASK_2 | D3DSP_WRITEMASK_3:
        case D3DSP_WRITEMASK_0 | D3DSP_WRITEMASK_1 | D3DSP_WRITEMASK_2:
        case D3DSP_WRITEMASK_3:
            break;

        default:
            Error(true, ERROR_ASM_INVALID_WRITE_MASK, "invalid write mask");
            goto LFail;
        }
    }
    else
    {
        bool ok = false;
		switch(dwReg & D3DSP_REGTYPE_MASK)
        {
        case D3DSPR_TEMP:
            // Don't allow r12 to be used as a destination register.
            // r12 is valid as a source register for xbox, which is why it's allowed to get this far.
            if ((dwReg & D3DSP_REGNUM_MASK)  >= D3DVS_TEMPREG_MAX_V1_1){
                ok = FALSE;
            }
            else {
                ok = TRUE;
            }
            break;
        case D3DSPR_ADDR:
			ok = true;
            break;
        case D3DSPR_RASTOUT:
        case D3DSPR_ATTROUT:
        case D3DSPR_TEXCRDOUT:
			ok = ! m_bStateShader;
            break;

        case D3DSPR_CONST:
			ok = m_bWritable != FALSE;
            if(ok && (dwReg & D3DVS_ADDRMODE_RELATIVE)){
			    Error(true, ERROR_ASM_INVALID_DESTINATION_REGISTER, "Can't use A0.x within a destination register.");
                goto LFail;
            }
			break;
		}
		if(!ok){
			Error(true, ERROR_ASM_INVALID_DESTINATION_REGISTER, "invalid destination register");
            goto LFail;
        }
    }

    return;

LFail:
    m_bFail = TRUE;
    return;
}


void
CD3DXAssembler::ValidateSrcRegister(DWORD dwReg)
{
    if(m_bPixel)
    {
        switch(dwReg & D3DSP_REGTYPE_MASK)
        {
        case D3DSPR_TEMP:
        case D3DSPR_INPUT:
        case D3DSPR_CONST:
        case D3DSPR_TEXTURE:
            break;

        default:
            Error(true, ERROR_ASM_INVALID_SOURCE_REGISTER, "invalid source register");
            goto LFail;
        }

        // Only valid swizzles: RGBA, AAAA, RGBB, BBBB
        switch(dwReg & D3DVS_SWIZZLE_MASK)
        {
        case D3DVS_X_X | D3DVS_Y_Y | D3DVS_Z_Z | D3DVS_W_W:
        case D3DVS_X_W | D3DVS_Y_W | D3DVS_Z_W | D3DVS_W_W:
        case D3DVS_X_X | D3DVS_Y_Y | D3DVS_Z_Z | D3DVS_W_Z:
        case D3DVS_X_Z | D3DVS_Y_Z | D3DVS_Z_Z | D3DVS_W_Z:
            break;

        default:
            Error(true, ERROR_ASM_INVALID_SWIZZLE, "invalid swizzle");
            goto LFail;
        }

    }
    else
    {
        switch(dwReg & D3DSP_REGTYPE_MASK)
        {
        case D3DSPR_TEMP:
        case D3DSPR_INPUT:
        case D3DSPR_CONST:
        case D3DSPR_ADDR:
            break;

        default:
            Error(true, ERROR_ASM_INVALID_SOURCE_REGISTER, "invalid source register");
            goto LFail;
        }
    }

    return;

LFail:
    m_bFail = TRUE;
    return;
}


void
CD3DXAssembler::SetConstant(DWORD dwReg, D3DXVECTOR4* pVec)
{
    if(D3DSPR_CONST != (dwReg & D3DSP_REGTYPE_MASK))
        goto LFail;

    if(dwReg & D3DVS_ADDRMODE_RELATIVE)
        goto LFail;

    if(m_bPixel)
    {
        m_cbConst += 6 * sizeof(DWORD);

        m_Const.Push( D3DSIO_DEF );
        m_Const.Push(dwReg | D3DSP_WRITEMASK_ALL);
    }
    else
    {
        m_cbConst += 5 * sizeof(DWORD);

        m_Const.Push(D3DVSD_CONST( dwReg & D3DSP_REGNUM_MASK, 1 ));
    }

    m_Const.Push(*((DWORD *) &pVec->x));
    m_Const.Push(*((DWORD *) &pVec->y));
    m_Const.Push(*((DWORD *) &pVec->z));
    m_Const.Push(*((DWORD *) &pVec->w));

    return;

LFail:
    Error(true, ERROR_ASM_INVALID_CONSTANT, "invalid constant");
    m_bFail = TRUE;
    return;
}



void
CD3DXAssembler::UpdateDebugText()
{
    if(!m_bDebug)
        return;

    char *pch = m_pchSource;
    UINT_PTR cch = m_pchLim - pch;

    m_Code.Push(D3DSHADER_COMMENT(((cch + 3) >> 2) + 1));
    m_Code.Push(FOURCC_TEXT);
    m_cbCode += 2 * sizeof(DWORD);

    while(cch >= sizeof(DWORD))
    {
        m_Code.Push(*((DWORD *) pch));
        m_cbCode += sizeof(DWORD);

        pch += sizeof(DWORD);
        cch -= sizeof(DWORD);
    }

    if(cch)
    {
        DWORD dw = 0;
        memcpy(&dw, pch, cch);

        m_Code.Push(dw);
        m_cbCode += sizeof(DWORD);
    }

    if(cch == sizeof(DWORD))
    {
        m_Code.Push(0);
        m_cbCode += sizeof(DWORD);
    }
}


void
CD3DXAssembler::UpdateDebugFileLine()
{
    if(!m_bDebug)
    {
        m_iLineDebug = m_iLine;
        return;
    }

    // Update file name, if needed
    if(strcmp(m_szFileDebug, m_szFile))
    {
        char *pch = m_szFile;
        UINT cch = strlen(pch) + 1;

        m_Code.Push(D3DSHADER_COMMENT(((cch + 3) >> 2) + 1));
        m_Code.Push(FOURCC_FILE);
        m_cbCode += 2 * sizeof(DWORD);

        while(cch >= sizeof(DWORD))
        {
            m_Code.Push(*((DWORD *) pch));
            m_cbCode += sizeof(DWORD);

            pch += sizeof(DWORD);
            cch -= sizeof(DWORD);
        }

        if(cch)
        {
            DWORD dw = 0;
            memcpy(&dw, pch, cch);

            m_Code.Push(dw);
            m_cbCode += sizeof(DWORD);
        }

        strcpy(m_szFileDebug, m_szFile);
    }

    // Update line number, if needed
    if(m_iLineDebug != m_iLineNextToken)
    {
        m_Code.Push(D3DSHADER_COMMENT(2));
        m_Code.Push(FOURCC_LINE);
        m_Code.Push((DWORD) m_iLineNextToken);
        m_cbCode += 3 * sizeof(DWORD);

        m_iLineDebug = m_iLineNextToken;
    }
}

} // namespace XGRAPHICS
