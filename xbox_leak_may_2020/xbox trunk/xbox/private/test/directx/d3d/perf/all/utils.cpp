//-----------------------------------------------------------------------------
// FILE: FILLRATE.CPP
//
// Desc: a stupid fillrate test
//
// Copyright (c) 1999-2000 Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#include <xtl.h>
#include <stdio.h>

#include "bitfont.h"
#include "utils.h"

// CScrText font class
BitFont g_bitfont;

//=========================================================================
// Stringtize PS_TEXTUREMODES_*
//=========================================================================
LPCSTR GetTextureModeStr(DWORD TextureMode)
{
    #undef XTAG
    #define XTAG(_tag) { PS_TEXTUREMODES##_tag, "PS_TM" #_tag }
    static const struct
    {
        DWORD texturemode;
        const char *szD3DMSStr;
    } rgszTMStr[] =
    {
        XTAG(_NONE), XTAG(_PROJECT2D), XTAG(_PROJECT3D),
        XTAG(_CUBEMAP), XTAG(_PASSTHRU), XTAG(_CLIPPLANE),
        XTAG(_BUMPENVMAP), XTAG(_BUMPENVMAP_LUM), XTAG(_BRDF),
        XTAG(_DOT_ST), XTAG(_DOT_ZW), XTAG(_DOT_RFLCT_DIFF),
        XTAG(_DOT_RFLCT_SPEC), XTAG(_DOT_STR_3D), XTAG(_DOT_STR_CUBE),
        XTAG(_DPNDNT_AR), XTAG(_DPNDNT_GB), XTAG(_DOTPRODUCT),
        XTAG(_DOT_RFLCT_SPEC_CONST),
    };
    static const int cFmts = sizeof(rgszTMStr) / sizeof(rgszTMStr[0]);

    for(int ifmt = 0; ifmt < cFmts; ifmt++)
    {
        if(rgszTMStr[ifmt].texturemode == TextureMode)
            return rgszTMStr[ifmt].szD3DMSStr;
    }

    return rgszTMStr[cFmts - 1].szD3DMSStr;
}

//=========================================================================
// Stringtize D3DMS_*
//=========================================================================
LPCSTR GetMultiSampleStr(DWORD mstype)
{
    #undef XTAG
    #define XTAG(_tag) { _tag, #_tag }
    static const struct
    {
        DWORD mstype;
        const char *szD3DMSStr;
    } rgszMSStr[] =
    {
        XTAG(D3DMS_NONE), XTAG(D3DMS_2_SAMPLES_MULTISAMPLE_LINEAR),
        XTAG(D3DMS_2_SAMPLES_MULTISAMPLE_QUINCUNX), XTAG(D3DMS_2_SAMPLES_SUPERSAMPLE_HORIZONTAL_LINEAR),
        XTAG(D3DMS_2_SAMPLES_SUPERSAMPLE_VERTICAL_LINEAR), XTAG(D3DMS_4_SAMPLES_MULTISAMPLE_LINEAR),
        XTAG(D3DMS_4_SAMPLES_MULTISAMPLE_GAUSSIAN), XTAG(D3DMS_4_SAMPLES_SUPERSAMPLE_LINEAR),
        XTAG(D3DMS_4_SAMPLES_SUPERSAMPLE_GAUSSIAN), XTAG(D3DMS_9_SAMPLES_MULTISAMPLE_GAUSSIAN),
        XTAG(D3DMS_9_SAMPLES_SUPERSAMPLE_GAUSSIAN), XTAG(D3DMS_PREFILTER_FORMAT_DEFAULT),
        XTAG(D3DMS_PREFILTER_FORMAT_X1R5G5B5), XTAG(D3DMS_PREFILTER_FORMAT_R5G6B5),
        XTAG(D3DMS_PREFILTER_FORMAT_X8R8G8B8), XTAG(D3DMS_PREFILTER_FORMAT_A8R8G8B8),
    };
    static const int cFmts = sizeof(rgszMSStr) / sizeof(rgszMSStr[0]);

    for(int ifmt = 0; ifmt < cFmts; ifmt++)
    {
        if(rgszMSStr[ifmt].mstype == mstype)
            return rgszMSStr[ifmt].szD3DMSStr;
    }

    return rgszMSStr[cFmts - 1].szD3DMSStr;
}

//=========================================================================
// Stringtize D3DCMP_*
//=========================================================================
LPCSTR GetD3DCMPStr(D3DCMPFUNC d3dcmpfunc)
{
    static const char *rgszD3DCmp[] =
    {
        "D3DCMP_NEVER",         // 0x200,
        "D3DCMP_LESS",          // 0x201,
        "D3DCMP_EQUAL",         // 0x202,
        "D3DCMP_LESSEQUAL",     // 0x203,
        "D3DCMP_GREATER",       // 0x204,
        "D3DCMP_NOTEQUAL",      // 0x205,
        "D3DCMP_GREATEREQUAL",  // 0x206,
        "D3DCMP_ALWAYS",        // 0x207,
        "??",
    };

    DWORD id3dcmp = min(d3dcmpfunc & 0xf, sizeof(rgszD3DCmp) / sizeof(rgszD3DCmp[0]));
    return rgszD3DCmp[id3dcmp];
}

//=========================================================================
// Stringtize D3DFMT_*
//=========================================================================
LPCSTR GetD3DFormatStr(D3DFORMAT d3dformat)
{
    #undef XTAG
    #define XTAG(_tag) { _tag, #_tag }
    static const struct
    {
        D3DFORMAT d3dformat;
        const char *szD3DFormat;
    } rgszD3DFmt[] =
    {
        XTAG(D3DFMT_A8R8G8B8), XTAG(D3DFMT_X8R8G8B8), XTAG(D3DFMT_R5G6B5),
        XTAG(D3DFMT_R6G5B5), XTAG(D3DFMT_X1R5G5B5), XTAG(D3DFMT_A1R5G5B5),
        XTAG(D3DFMT_A4R4G4B4), XTAG(D3DFMT_A8), XTAG(D3DFMT_A8B8G8R8),
        XTAG(D3DFMT_B8G8R8A8), XTAG(D3DFMT_R4G4B4A4), XTAG(D3DFMT_R5G5B5A1),
        XTAG(D3DFMT_R8G8B8A8), XTAG(D3DFMT_R8B8), XTAG(D3DFMT_G8B8),
        XTAG(D3DFMT_P8), XTAG(D3DFMT_L8), XTAG(D3DFMT_A8L8),
        XTAG(D3DFMT_AL8), XTAG(D3DFMT_L16), XTAG(D3DFMT_V8U8),
        XTAG(D3DFMT_L6V5U5), XTAG(D3DFMT_X8L8V8U8), XTAG(D3DFMT_Q8W8V8U8),
        XTAG(D3DFMT_V16U16), XTAG(D3DFMT_D16_LOCKABLE), XTAG(D3DFMT_D16),
        XTAG(D3DFMT_D24S8), XTAG(D3DFMT_F16), XTAG(D3DFMT_F24S8),
        XTAG(D3DFMT_UYVY), XTAG(D3DFMT_YUY2), XTAG(D3DFMT_DXT1),
        XTAG(D3DFMT_DXT2), XTAG(D3DFMT_DXT3), XTAG(D3DFMT_DXT4),
        XTAG(D3DFMT_DXT5), XTAG(D3DFMT_LIN_A1R5G5B5), XTAG(D3DFMT_LIN_A4R4G4B4),
        XTAG(D3DFMT_LIN_A8), XTAG(D3DFMT_LIN_A8B8G8R8), XTAG(D3DFMT_LIN_A8R8G8B8),
        XTAG(D3DFMT_LIN_B8G8R8A8), XTAG(D3DFMT_LIN_G8B8), XTAG(D3DFMT_LIN_R4G4B4A4),
        XTAG(D3DFMT_LIN_R5G5B5A1), XTAG(D3DFMT_LIN_R5G6B5), XTAG(D3DFMT_LIN_R6G5B5),
        XTAG(D3DFMT_LIN_R8B8), XTAG(D3DFMT_LIN_R8G8B8A8), XTAG(D3DFMT_LIN_X1R5G5B5),
        XTAG(D3DFMT_LIN_X8R8G8B8), XTAG(D3DFMT_LIN_A8L8), XTAG(D3DFMT_LIN_AL8),
        XTAG(D3DFMT_LIN_L16), XTAG(D3DFMT_LIN_L8), XTAG(D3DFMT_LIN_D24S8),
        XTAG(D3DFMT_LIN_F24S8), XTAG(D3DFMT_LIN_D16), XTAG(D3DFMT_LIN_F16),
        XTAG(D3DFMT_UNKNOWN),
    };
    static const int cFmts = sizeof(rgszD3DFmt) / sizeof(rgszD3DFmt[0]);

    for(int ifmt = 0; ifmt < cFmts; ifmt++)
    {
        if(rgszD3DFmt[ifmt].d3dformat == d3dformat)
            return rgszD3DFmt[ifmt].szD3DFormat;
    }

    return rgszD3DFmt[cFmts - 1].szD3DFormat;
}

//=========================================================================
// Check whether specified format is linear
//=========================================================================
bool IsLinearFormat(DWORD Format)
{
    switch(Format)
    {
    case D3DFMT_LIN_A1R5G5B5:
    case D3DFMT_LIN_A4R4G4B4:
    case D3DFMT_LIN_A8:
    case D3DFMT_LIN_A8B8G8R8:
    case D3DFMT_LIN_A8R8G8B8:
    case D3DFMT_LIN_B8G8R8A8:
    case D3DFMT_LIN_G8B8:
    case D3DFMT_LIN_R4G4B4A4:
    case D3DFMT_LIN_R5G5B5A1:
    case D3DFMT_LIN_R5G6B5:
    case D3DFMT_LIN_R6G5B5:
    case D3DFMT_LIN_R8B8:
    case D3DFMT_LIN_R8G8B8A8:
    case D3DFMT_LIN_X1R5G5B5:
    case D3DFMT_LIN_X8R8G8B8:

    case D3DFMT_LIN_A8L8:
    case D3DFMT_LIN_AL8:
    case D3DFMT_LIN_L16:
    case D3DFMT_LIN_L8:

    case D3DFMT_LIN_D24S8:
    case D3DFMT_LIN_F24S8:
    case D3DFMT_LIN_D16:
    case D3DFMT_LIN_F16:
        return true;
    }

    return false;
}

//=========================================================================
// Return number of bits per pixel/texel.
//=========================================================================
DWORD BitsPerPixelOfD3DFORMAT(DWORD Format)
{
    DWORD retval = 0;

    switch(Format)
    {
    case D3DFMT_A8R8G8B8:   // Q8W8V8U8
    case D3DFMT_X8R8G8B8:   // X8L8V8U8
    case D3DFMT_A8B8G8R8:
    case D3DFMT_B8G8R8A8:
    case D3DFMT_R8G8B8A8:
    case D3DFMT_V16U16:
    case D3DFMT_D24S8:
    case D3DFMT_F24S8:
    case D3DFMT_UYVY:
    case D3DFMT_YUY2:
    case D3DFMT_LIN_A8B8G8R8:
    case D3DFMT_LIN_A8R8G8B8:
    case D3DFMT_LIN_B8G8R8A8:
    case D3DFMT_LIN_R8G8B8A8:
    case D3DFMT_LIN_X8R8G8B8:
    case D3DFMT_LIN_D24S8:
    case D3DFMT_LIN_F24S8:
        retval = 32;
        break;

    case D3DFMT_R5G6B5:
    case D3DFMT_X1R5G5B5:
    case D3DFMT_A1R5G5B5:
    case D3DFMT_A4R4G4B4:
    case D3DFMT_R4G4B4A4:
    case D3DFMT_R5G5B5A1:
    case D3DFMT_R8B8:
    case D3DFMT_G8B8:   // V8U8
    case D3DFMT_A8L8:
    case D3DFMT_L16:
    case D3DFMT_L6V5U5:
    case D3DFMT_D16_LOCKABLE:  // D16
    case D3DFMT_F16:
    case D3DFMT_LIN_A1R5G5B5:
    case D3DFMT_LIN_A4R4G4B4:
    case D3DFMT_LIN_G8B8:
    case D3DFMT_LIN_R4G4B4A4:
    case D3DFMT_LIN_R5G5B5A1:
    case D3DFMT_LIN_R5G6B5:
    case D3DFMT_LIN_R6G5B5:
    case D3DFMT_LIN_R8B8:
    case D3DFMT_LIN_X1R5G5B5:
    case D3DFMT_LIN_A8L8:
    case D3DFMT_LIN_L16:
    case D3DFMT_LIN_D16:
    case D3DFMT_LIN_F16:
        retval = 16;
        break;

    case D3DFMT_A8:
    case D3DFMT_P8:
    case D3DFMT_L8:
    case D3DFMT_AL8:
    case D3DFMT_LIN_A8:
    case D3DFMT_LIN_AL8:
    case D3DFMT_LIN_L8:
        retval = 8;
        break;

    case D3DFMT_DXT1:
        retval = 4;
        break;

    case D3DFMT_DXT2: // DXT3
    case D3DFMT_DXT4: // DXT5
        retval = 8;
        break;

    default:
        CheckHR(E_FAIL);
        retval = 0;
        break;
    }

    return retval;
}

//=========================================================================
// Return file's last write time
//=========================================================================
UINT64 GetFileTime(LPCSTR lpFileName)
{
    WIN32_FILE_ATTRIBUTE_DATA fad;

    GetFileAttributesExA(lpFileName, GetFileExInfoStandard, &fad);
    return *(UINT64 *)&fad.ftLastWriteTime;
}

//=========================================================================
// Display d3d error then break into debugger
//=========================================================================
void DisplayError(const char *szCall, HRESULT hr)
{
    char szErr[512];

    if(hr)
    {
        UINT cch = _snprintf(szErr, sizeof(szErr), "FATAL: %s failed 0x%08lX: ", szCall, hr);

        D3DXGetErrorStringA(hr, &szErr[cch], sizeof(szErr) - cch - 1);
    }
    else
    {
        lstrcpynA(szErr, szCall, sizeof(szErr) - 1);
    }

    lstrcatA(szErr, "\n");
    OutputDebugStringA(szErr);

    _asm int 3;
}

//=========================================================================
// Formatted debug squirty
//=========================================================================
void _cdecl dprintf(LPCSTR lpFmt, ...)
{
    va_list arglist;
    char lpOutput[256];

    va_start(arglist, lpFmt);
    _vsnprintf(lpOutput, sizeof(lpOutput), lpFmt, arglist);
    va_end(arglist);

    OutputDebugStringA(lpOutput);
}

//=========================================================================
// Draw CScrText text buffer to front buffer
//=========================================================================
void CScrText::drawtext(bool fDebugSquirty)
{
    IDirect3DSurface8 *pFrontBuffer = NULL;

    D3DDevice_GetBackBuffer(-1, D3DBACKBUFFER_TYPE_MONO, &pFrontBuffer);
    if(pFrontBuffer)
    {
        // render results to the frontbuffer
        DWORD dx = 40;
        DWORD dy = 20;
        DWORD dwHeight;

        g_bitfont.GetTextStringLength(NULL, &dwHeight, NULL);
        dwHeight += 2;

        for(int iBuf = 0; iBuf < m_iLine; iBuf++)
        {
            if(fDebugSquirty)
                dprintf("%s\n", m_rgText[iBuf]);

            if(m_rgText[iBuf][0])
//                g_bitfont.DrawText(pFrontBuffer, m_rgText[iBuf], dx, dy, 0, 0xff9f9f9f, 0);

            dy += dwHeight;
        }

        RELEASE(pFrontBuffer);
    }
}

//=========================================================================
// Add a string to the CScrText string buffer
//=========================================================================
void _cdecl CScrText::printf(LPCSTR lpFmt, ...)
{
    va_list arglist;

    va_start(arglist, lpFmt);
    _vsnprintf(m_rgText[m_iLine], LINE_WIDTH, lpFmt, arglist);
    va_end(arglist);

    m_iLine = (m_iLine + 1) % NUM_LINES;
}

