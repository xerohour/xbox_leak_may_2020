///////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) 2000 Microsoft Corporation.  All Rights Reserved.
//
//
///////////////////////////////////////////////////////////////////////////

//
// D3DX core precompiled header
//

#ifndef __PCHCORE_H__
#define __PCHCORE_H__

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <malloc.h>

#ifdef XBOX
#include <xgrphseg.h>
#include <xtl.h>
#include <xboxverp.h>
#else
#include "windows.h"
#include "d3d8-xbox.h"
#include "../../../private/inc/xboxverp.h"

#endif // XBOX

#ifndef __D3DX8MATH_H__

typedef struct D3DXVECTOR4
{
    FLOAT x, y, z, w;
} D3DXVECTOR4, *LPD3DXVECTOR4;

#endif

#include "xgraphics.h"
#include "debug.h"

#include "microcodeformat.h"

namespace XGRAPHICS {

struct XD3DXErrorLog {
    virtual HRESULT Log(bool error, DWORD errorCode, LPCSTR filePath, DWORD line, LPCSTR message) = 0;
};

class Buffer {
    DWORD m_size;
    char* m_buf;
    DWORD m_used;

public:
    Buffer();
    ~Buffer();
    HRESULT Initialize(DWORD size);
    HRESULT GrowTo(DWORD size);
    HRESULT Printf(const char* msg,...);
    HRESULT Append(LPCVOID data, DWORD length);
    HRESULT Append(char c);
    HRESULT Append(const char* c);
    HRESULT Append(Buffer& buffer);
    char* GetText();
    char* GetTextEnd();
    DWORD GetUsed();
    void Clear();
};

// A Constructor for XGBUFFER that copies data from a Buffer object

HRESULT XGBufferCreateFromBuffer(Buffer* pBuf, LPXGBUFFER* ppXGBuf);


HRESULT
XGPreprocess(LPCSTR pSourceFileName,
    SASM_ResolverCallback pResolver,
    LPVOID pResolverUserData,
    LPXGBUFFER  pInput,
    LPXGBUFFER* pOutput,
    XD3DXErrorLog* pErrorLog);

} // namespace XGRAPHICS

#include "CD3DXStack.h"
#include "CD3DXAssembler.h"

namespace XGRAPHICS {

#define RELEASE(x) \
    do { if(x) { x->Release(); x = NULL; } } while(0)

// Uncomment this if you want to break into the debugger when an error occurs
// #define BREAK_ON_ERROR

#if defined(DBG) && defined(BREAK_ON_ERROR)
#define SETERROR(HR,CODE) \
    do { \
if(FAILED((HR) = (CODE))){ \
            DebugBreak(); \
        } \
    } while(0)
#else
#define SETERROR(HR,CODE) \
    ((HR) = (CODE))
#endif

// Just to get things to compile

#define D3DXASSERT(X) ASSERT(X)

#define D3DXERR_INVALIDDATA E_FAIL

#define D3DVS_TEMPREG_MAX_V1_1 12
#define D3DVS_INPUTREG_MAX_V1_1 16
#define D3DVS_ADDRREG_MAX_V1_1 1
#define D3DVS_ATTROUTREG_MAX_V1_1 2
#define D3DVS_TCRDOUTREG_MAX_V1_1 4

#ifndef MAKEFOURCC
#define MAKEFOURCC(ch0, ch1, ch2, ch3)                              \
                ((DWORD)(BYTE)(ch0) | ((DWORD)(BYTE)(ch1) << 8) |   \
                ((DWORD)(BYTE)(ch2) << 16) | ((DWORD)(BYTE)(ch3) << 24 ))


#endif

#define XBOX_EXTENSIONS

// Source operand addressing modes

typedef enum _D3DVS_ADDRESSMODE_TYPE
{
    D3DVS_ADDRMODE_ABSOLUTE  = (0 << D3DVS_ADDRESSMODE_SHIFT),
    D3DVS_ADDRMODE_RELATIVE  = (1 << D3DVS_ADDRESSMODE_SHIFT),   // Relative to register A0
    D3DVS_ADDRMODE_FORCE_DWORD = 0x7fffffff, // force 32-bit size enum
} D3DVS_ADDRESSMODE_TYPE;

HRESULT CompilePixelShaderToUCode(bool optimize, DWORD shaderType,
                                  LPXGBUFFER pTokenizedShader, 
                                  LPXGBUFFER* ppCompiledShader,
                                  Buffer* pListing,
                                  XD3DXErrorLog* pErrorLog);

} // namespace XGRAPHICS

// shader validator
#include "valbase.h"
#include "vshdrval.h"
#include "pshdrval.h"
#include "errorcodes.h"

#endif //__PCHCORE_H__
