///////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) 1999 Microsoft Corporation.  All Rights Reserved.
//
//  File:       compiler.h
//  Content:    Vertex Shader compiler
//
///////////////////////////////////////////////////////////////////////////

//----------------------------------------------------------------------------
// CD3DXAssembler
//----------------------------------------------------------------------------

namespace XGRAPHICS {

class CD3DXAssembler
{
public: // So we don't have to declare a ton of friend functions

    XAsmCD3DXSzStack m_Id;
    XAsmCD3DXDwStack m_Number;
    XAsmCD3DXDwStack m_Float;
    XAsmCD3DXDwStack m_Param;
    XAsmCD3DXDwStack m_Opcode;
    XAsmCD3DXDwStack m_Const;
    XAsmCD3DXDwStack m_Code;

	XD3DXErrorLog* m_pErrorLog;
    BOOL  m_bFail;
    UINT  m_cbConst;
    UINT  m_cbCode;

    BOOL  m_bPixel;
	BOOL  m_bXbox;
	BOOL  m_bWritable;
	BOOL  m_bStateShader;
    BOOL  m_bScreenSpace; // For vertex shaders, whether or not to append postfix instructions.
    BOOL  m_bDebug;
    DWORD m_dwVersion;

    char* m_pchSource;
    char* m_pchLine;
    char* m_pch;
    char* m_pchLim;

    char  m_szTok[256];
    DWORD m_dwOpcode;
    DWORD m_dwShiftSat;

    UINT  m_iLineNextToken;
    UINT  m_iLine;
    char  m_szFile[256];

    UINT  m_iLineDebug;
    char  m_szFileDebug[256];

public:
    static CD3DXAssembler* s_pAsm;

public:
    CD3DXAssembler();
   ~CD3DXAssembler();

    HRESULT Assemble(LPCVOID pv, UINT cb, LPCSTR szFile, 
        UINT uLine, DWORD dwFlags, LPXGBUFFER* ppConstants, 
        LPXGBUFFER* ppCode, XD3DXErrorLog* pErrorLog,
        LPDWORD pShaderType);

    int  Token();
    void Production(UINT nProduction);
    void Error(bool error, DWORD code, LPCSTR psz, ...);
    void YYError(char *psz, ...);
    void Error2(bool error, DWORD code, LPCSTR psz, va_list list);

    int   DecodeOpcode();
    DWORD DecodeMask(char *psz);
    DWORD DecodeSwizzle(char *psz);
    DWORD DecodeRegister(char *psz, DWORD dwAddr, BOOL bIndex);

    void ValidateDstRegister(DWORD dwReg);
    void ValidateSrcRegister(DWORD dwReg);

    void SetConstant(DWORD dwReg, D3DXVECTOR4* pVec);

    void UpdateDebugText();
    void UpdateDebugFileLine();
};

} // namespace XGRAPHICS