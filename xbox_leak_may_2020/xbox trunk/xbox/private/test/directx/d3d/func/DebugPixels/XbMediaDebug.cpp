//-----------------------------------------------------------------------------
// File: XbMediaDebug.cpp
//
// Desc: Miscellaneous functions to aid debugging of media/graphics/etc.
//
// Hist: 11.01.01 - New for November 2001 XDK release
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#include "XbMediaDebug.h"


//-----------------------------------------------------------------------------
// Macros to set new state, then restore old state upon leaving scope.
//-----------------------------------------------------------------------------
#define SET_D3DTSS(STAGE, NAME, VALUE)\
    struct D3DTSS_STAGE##STAGE##_##NAME {\
        DWORD m_dw; \
        D3DTSS_STAGE##STAGE##_##NAME(DWORD dw) { /* ctor saves curr value and sets new value */ \
            D3DDevice_GetTextureStageState(STAGE, NAME, &m_dw);\
            D3DDevice_SetTextureStageState(STAGE, NAME, dw);\
        }\
        ~D3DTSS_STAGE##STAGE##_##NAME() { /* on scope exit, saved value is restored */ \
            D3DDevice_SetTextureStageState(STAGE, NAME, m_dw);\
        }\
    } _D3DTSS_STAGE##STAGE##_##NAME(VALUE)

#define SET_D3DTEXTURE(STAGE, TEXTURE)\
    struct D3DTEXTURE_STAGE##STAGE {\
        D3DBaseTexture *m_pTexture; \
        D3DTEXTURE_STAGE##STAGE(D3DBaseTexture *pTexture) { /* ctor saves curr value and sets new value */ \
            D3DDevice_GetTexture(STAGE, &m_pTexture);\
            D3DDevice_SetTexture(STAGE, pTexture);\
        }\
        ~D3DTEXTURE_STAGE##STAGE() { /* on scope exit, saved value is restored */ \
            D3DDevice_SetTexture(STAGE, m_pTexture);\
            if (m_pTexture != NULL) m_pTexture->Release();\
        }\
    } _D3DTEXTURE_STAGE##STAGE(TEXTURE)

#define SET_D3DRS(NAME, VALUE)\
    struct D3DRS_##NAME {\
        DWORD m_dw; \
        D3DRS_##NAME(DWORD dw) { /* ctor saves curr value and sets new value */ \
            D3DDevice_GetRenderState(NAME, &m_dw);\
            D3DDevice_SetRenderState(NAME, dw);\
        }\
        ~D3DRS_##NAME() { /* on scope exit, saved value is restored */ \
            D3DDevice_SetRenderState(NAME, m_dw);\
        }\
    } _D3DRS_##NAME(VALUE)

#define SET_D3DVERTEXSHADER(VALUE)\
    struct D3DVERTEXSHADER {\
        DWORD m_dw; \
        D3DVERTEXSHADER(DWORD dw) { /* ctor saves curr value and sets new value */ \
            D3DDevice_GetVertexShader(&m_dw);\
            D3DDevice_SetVertexShader(dw);\
        }\
        ~D3DVERTEXSHADER() { /* on scope exit, saved value is restored */ \
            D3DDevice_SetVertexShader(m_dw);\
        }\
    } _D3DVERTEXSHADER(VALUE)

#define SET_D3DPIXELSHADER(VALUE)\
    struct D3DPIXELSHADER {\
        DWORD m_dw; \
        D3DPIXELSHADER(DWORD dw) { /* ctor saves curr value and sets new value */ \
            D3DDevice_GetPixelShader(&m_dw);\
            D3DDevice_SetPixelShader(dw);\
        }\
        ~D3DPIXELSHADER() { /* on scope exit, saved value is restored */ \
            D3DDevice_SetPixelShader(m_dw);\
        }\
    } _D3DPIXELSHADER(VALUE)

#define SET_D3DVIEWPORT(PVIEWPORT)\
    struct D3DVIEWPORT {\
        D3DVIEWPORT8 m_viewport; \
        D3DVIEWPORT(D3DVIEWPORT *pViewport) { /* ctor saves curr value and sets new value */ \
            D3DDevice_GetViewport(&m_viewport);\
            D3DDevice_SetViewport(pViewport);\
        }\
        ~D3DVIEWPORT() { /* on scope exit, saved value is restored */ \
            D3DDevice_SetViewport(&m_viewport);\
        }\
    } _D3DVIEWPORT(PVIEWPORT)

#define SET_D3DTRANSFORM(NAME, PTRANSFORM)\
    struct D3DTRANSFORM_##NAME {\
        D3DMATRIX m_transform; \
        D3DTRANSFORM_##NAME(D3DMATRIX *pTransform) { /* ctor saves curr value and sets new value */ \
            D3DDevice_GetTransform(NAME, &m_transform);\
            D3DDevice_SetTransform(NAME, pTransform);\
        }\
        ~D3DTRANSFORM_##NAME() { /* on scope exit, saved value is restored */ \
            D3DDevice_SetTransform(NAME, &m_transform);\
        }\
    } _D3DTRANSFORM_##NAME(PTRANSFORM)


#define SETUP_RENDER_TARGET() \
    struct DebugSetupRenderTarget \
    { \
        IDirect3DSurface8* m_pPrevColorBuffer; \
        IDirect3DSurface8* m_pPrevDepthBuffer; \
      \
        DebugSetupRenderTarget() \
        { \
            /* Save info about the current render target */ \
            D3DDevice_GetRenderTarget( &m_pPrevColorBuffer ); \
            D3DDevice_GetDepthStencilSurface( &m_pPrevDepthBuffer ); \
          \
            /* Make the front buffer the new render target */ \
            IDirect3DSurface8* pFrontBuffer = NULL; \
            D3DDevice_GetBackBuffer( -1, D3DBACKBUFFER_TYPE_MONO, &pFrontBuffer ); \
            D3DDevice_SetRenderTarget( pFrontBuffer, NULL ); \
            pFrontBuffer->Release(); \
          \
            /* Make sure all the commands so far are done */ \
            D3DDevice_BlockUntilIdle(); \
        } \
      \
        ~DebugSetupRenderTarget() \
        { \
            /* Restore render target, etc. */ \
            D3DDevice_SetRenderTarget( m_pPrevColorBuffer, m_pPrevDepthBuffer ); \
            if( m_pPrevColorBuffer )  { m_pPrevColorBuffer->Release(); } \
            if( m_pPrevDepthBuffer )  { m_pPrevDepthBuffer->Release(); } \
        } \
    } __DebugSetupRenderTarget;



//-----------------------------------------------------------------------------
// Name: DebugPresent
// Desc: Forces all GPU commands to be processed, so that data will appear
//       on-screen.
//-----------------------------------------------------------------------------
HRESULT __cdecl DebugPresent()
{
    D3DDevice_KickPushBuffer();
    D3DDevice_BlockUntilIdle();
    return S_OK;
}



///////////////////////////////////////////////////////////////////////////////
// FORMAT REMAPPING
///////////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------------
// Name: MapLinearToSwizzledFormat
// Desc: Convert from a linear D3DFORMAT to the equivalent swizzled format.
//-----------------------------------------------------------------------------
D3DFORMAT __cdecl MapLinearToSwizzledFormat( D3DFORMAT fmt )
{
    switch( fmt )
    {
        case D3DFMT_LIN_A1R5G5B5:   return D3DFMT_A1R5G5B5;
        case D3DFMT_LIN_A4R4G4B4:   return D3DFMT_A4R4G4B4;
        case D3DFMT_LIN_A8:         return D3DFMT_A8;
        case D3DFMT_LIN_A8B8G8R8:   return D3DFMT_A8B8G8R8;
        case D3DFMT_LIN_A8R8G8B8:   return D3DFMT_A8R8G8B8;
        case D3DFMT_LIN_B8G8R8A8:   return D3DFMT_B8G8R8A8;
        case D3DFMT_LIN_G8B8:       return D3DFMT_G8B8;
        case D3DFMT_LIN_R4G4B4A4:   return D3DFMT_R4G4B4A4;
        case D3DFMT_LIN_R5G5B5A1:   return D3DFMT_R5G5B5A1;
        case D3DFMT_LIN_R5G6B5:     return D3DFMT_R5G6B5;
        case D3DFMT_LIN_R6G5B5:     return D3DFMT_R6G5B5;
        case D3DFMT_LIN_R8B8:       return D3DFMT_R8B8;
        case D3DFMT_LIN_R8G8B8A8:   return D3DFMT_R8G8B8A8;
        case D3DFMT_LIN_X1R5G5B5:   return D3DFMT_X1R5G5B5;
        case D3DFMT_LIN_X8R8G8B8:   return D3DFMT_X8R8G8B8;
        case D3DFMT_LIN_A8L8:       return D3DFMT_A8L8;
        case D3DFMT_LIN_AL8:        return D3DFMT_AL8;
        case D3DFMT_LIN_L16:        return D3DFMT_L16;
        case D3DFMT_LIN_L8:         return D3DFMT_L8;
        case D3DFMT_LIN_V16U16:     return D3DFMT_V16U16;
//These constants have same value as other constants above.
//        case D3DFMT_LIN_V8U8:       return D3DFMT_V8U8;
//        case D3DFMT_LIN_L6V5U5:     return D3DFMT_L6V5U5;
//        case D3DFMT_LIN_X8L8V8U8:   return D3DFMT_X8L8V8U8;
//        case D3DFMT_LIN_Q8W8V8U8:   return D3DFMT_Q8W8V8U8;
        case D3DFMT_LIN_D24S8:      return D3DFMT_D24S8;
        case D3DFMT_LIN_F24S8:      return D3DFMT_F24S8;
        case D3DFMT_LIN_D16:        return D3DFMT_D16;
        case D3DFMT_LIN_F16:        return D3DFMT_F16;
        default:
            return fmt;
    }
}



//-----------------------------------------------------------------------------
// Name: MapSwizzledToLinearFormat
// Desc: Convert from a swizzled D3DFORMAT to the equivalent linear format.
//-----------------------------------------------------------------------------
D3DFORMAT __cdecl MapSwizzledToLinearFormat( D3DFORMAT fmt )
{
    switch( fmt )
    {
        case D3DFMT_A1R5G5B5:   return D3DFMT_LIN_A1R5G5B5;
        case D3DFMT_A4R4G4B4:   return D3DFMT_LIN_A4R4G4B4;
        case D3DFMT_A8:         return D3DFMT_LIN_A8;
        case D3DFMT_A8B8G8R8:   return D3DFMT_LIN_A8B8G8R8;
        case D3DFMT_A8R8G8B8:   return D3DFMT_LIN_A8R8G8B8;
        case D3DFMT_B8G8R8A8:   return D3DFMT_LIN_B8G8R8A8;
        case D3DFMT_G8B8:       return D3DFMT_LIN_G8B8;
        case D3DFMT_R4G4B4A4:   return D3DFMT_LIN_R4G4B4A4;
        case D3DFMT_R5G5B5A1:   return D3DFMT_LIN_R5G5B5A1;
        case D3DFMT_R5G6B5:     return D3DFMT_LIN_R5G6B5;
        case D3DFMT_R6G5B5:     return D3DFMT_LIN_R6G5B5;
        case D3DFMT_R8B8:       return D3DFMT_LIN_R8B8;
        case D3DFMT_R8G8B8A8:   return D3DFMT_LIN_R8G8B8A8;
        case D3DFMT_X1R5G5B5:   return D3DFMT_LIN_X1R5G5B5;
        case D3DFMT_X8R8G8B8:   return D3DFMT_LIN_X8R8G8B8;
        case D3DFMT_A8L8:       return D3DFMT_LIN_A8L8;
        case D3DFMT_AL8:        return D3DFMT_LIN_AL8;
        case D3DFMT_L16:        return D3DFMT_LIN_L16;
        case D3DFMT_L8:         return D3DFMT_LIN_L8;
        case D3DFMT_V16U16:     return D3DFMT_LIN_V16U16;
//These constants have same value as other constants above.
//        case D3DFMT_V8U8:       return D3DFMT_LIN_V8U8;
//        case D3DFMT_L6V5U5:     return D3DFMT_LIN_L6V5U5;
//        case D3DFMT_X8L8V8U8:   return D3DFMT_LIN_X8L8V8U8;
//        case D3DFMT_Q8W8V8U8:   return D3DFMT_LIN_Q8W8V8U8;
        case D3DFMT_D24S8:      return D3DFMT_LIN_D24S8;
        case D3DFMT_F24S8:      return D3DFMT_LIN_F24S8;
        case D3DFMT_D16:        return D3DFMT_LIN_D16;
        case D3DFMT_F16:        return D3DFMT_LIN_F16;
        default:
            return fmt;
    }
}



//-----------------------------------------------------------------------------
// Name: MapToColorFormat
// Desc: Given a D3DFORMAT, return a format with which the data can be
//       displayed visually.  For example, we might convert from D24S8 to
//       R8G8B8A8.  Useful for debugging texture/surface contents visually.
// Note: For some formats, it's not clear what the "right" choice is, but we
//       try to pick something reasonable.
//-----------------------------------------------------------------------------
D3DFORMAT __cdecl MapToColorFormat( D3DFORMAT fmt )
{
    switch( fmt )
    {
        // Swizzled formats
        //case D3DFMT_P8:  // don't remap; just use current palette
        case D3DFMT_A8:
        case D3DFMT_AL8:
            return D3DFMT_L8;

        case D3DFMT_A8L8:
        case D3DFMT_L16:
        case D3DFMT_V8U8:
            return D3DFMT_G8B8;

        case D3DFMT_L6V5U5:
            return D3DFMT_R6G5B5;

        case D3DFMT_X8L8V8U8:
        case D3DFMT_Q8W8V8U8:
        case D3DFMT_V16U16:
            return D3DFMT_A8R8G8B8;

        case D3DFMT_D16:
        case D3DFMT_F16:
            return D3DFMT_R5G6B5;

        case D3DFMT_D24S8:
        case D3DFMT_F24S8:
            return D3DFMT_R8G8B8A8;


        // Linear formats
        case D3DFMT_LIN_A8:
        case D3DFMT_LIN_AL8:
            return D3DFMT_LIN_L8;

        case D3DFMT_LIN_A8L8:
        case D3DFMT_LIN_L16:
        case D3DFMT_LIN_V8U8:
            return D3DFMT_LIN_G8B8;

        case D3DFMT_LIN_L6V5U5:
            return D3DFMT_LIN_R6G5B5;

        case D3DFMT_LIN_X8L8V8U8:
        case D3DFMT_LIN_Q8W8V8U8:
        case D3DFMT_LIN_V16U16:
            return D3DFMT_LIN_A8R8G8B8;

        case D3DFMT_LIN_D16:
        case D3DFMT_LIN_F16:
            return D3DFMT_LIN_R5G6B5;
    
        case D3DFMT_LIN_D24S8:
        case D3DFMT_LIN_F24S8:
            return D3DFMT_LIN_R8G8B8A8;

        default:
            return fmt;
    }
}



///////////////////////////////////////////////////////////////////////////////
// BUFFER CLEARING
///////////////////////////////////////////////////////////////////////////////

D3DXCOLOR g_DebugClearColor(0.5f, 0.5f, 0.5f, 1.0f); // bkgrnd color when this lib clears screen


//-----------------------------------------------------------------------------
// Name: DebugClear
// Desc: Clears the current color buffer (but not z or stencil), using the
//       value specified in 'g_DebugClearColor'.
//-----------------------------------------------------------------------------
void __cdecl DebugClear()
{
    D3DDevice_Clear( 0L, NULL, D3DCLEAR_TARGET, g_DebugClearColor, 1.0f, 0 );
}



///////////////////////////////////////////////////////////////////////////////
// TEXTURE/SURFACE DEBUGGING
///////////////////////////////////////////////////////////////////////////////

bool g_bDebugShrinkToFit = true;   // decrease size (if necess) to fit screen
bool g_bDebugExpandToFit = false;  // increase size (if necess) to fill screen
bool g_bDebugAlpha = false;  // view the alpha channel when rendering


//-----------------------------------------------------------------------------
// Name: DebugPixels
// Desc: Displays a pTexture or pSurface (you can pass-in either one here).
//       For convenience, you can also pass-in special pre-defined values for
//       convenient access to common surfaces:
//          1 == 1st back-color-buffer (note: differs from GetBackBuffer param)
//          2 == 2nd back-color-buffer
//          N == Nth back-color-buffer
//         -1 == depth-stencil-buffer
// Note: This function can be called via the debugger watch window, in which
//       case it will be executed with each step through the code.
//-----------------------------------------------------------------------------
HRESULT __cdecl DebugPixels( void* pTextureOrSurface, UINT iLevel )
{
    D3DPixelContainer* pPixelContainer = (D3DPixelContainer*)pTextureOrSurface;
    
    //
    // Check for special-code argument
    //
    bool bSpecialCodeSurface = false;
    if( int(pTextureOrSurface) >= 1  &&  int(pTextureOrSurface) <= 8 )
    {
        // requested a back color buffer
        bSpecialCodeSurface = true;
        D3DDevice_GetBackBuffer( int(pTextureOrSurface) - 1,
                                 D3DBACKBUFFER_TYPE_MONO,
                                 (D3DSurface**)&pPixelContainer );
    }
    else if( int(pTextureOrSurface) == -1 )
    {
        // requested depth-stencil buffer
        bSpecialCodeSurface = true;
        D3DDevice_GetDepthStencilSurface( (D3DSurface**)&pPixelContainer );
    }

    if( NULL == pPixelContainer )  { return S_FALSE; }


    //
    // Get info about the width, height, format, etc. of the texture/surface
    //
    D3DSURFACE_DESC desc;
    DWORD dwPixelContainerType = (pPixelContainer->Common & D3DCOMMON_TYPE_MASK);
    switch( dwPixelContainerType )
    {
        case D3DCOMMON_TYPE_TEXTURE:
        {
            D3DTexture* pTexture = (D3DTexture*)pPixelContainer;

            DWORD nLevels = pTexture->GetLevelCount();
            if(iLevel >= nLevels)
            {
                return S_FALSE;
            }

            pTexture->GetLevelDesc(iLevel, &desc);
        }
        break;

        case D3DCOMMON_TYPE_SURFACE:
        {
            D3DSurface* pSurface = (D3DSurface*)pPixelContainer;

            pSurface->GetDesc(&desc);
            iLevel = 0;  // no other choice for surface; don't let caller specify something else
        }
        break;

        default:
            return S_FALSE; // we only handle textures and surfaces
    }


    //
    // In case the texture/surface was in tiled memory, make a temp copy here
    // (w/o tiling), so that we will always view "real" contents w/o distortion
    // caused by tiling. (Yeah, this isn't optimal, and it isn't always
    // required, but it makes this function much more robust. And this is only
    // a debug function anyway....)
    //
    D3DTexture* pTexCopy = NULL;
    D3DSurface* pSurfSrc  = NULL;
    D3DSurface* pSurfDest = NULL;

    // Copy the desired level
    D3DDevice_CreateTexture( desc.Width, desc.Height, 1, 0, desc.Format, 0, &pTexCopy );
    if( NULL == pTexCopy )  { return S_FALSE; }
    {
        pTexCopy->GetSurfaceLevel( 0, &pSurfDest );
        switch(dwPixelContainerType)
        {
          case D3DCOMMON_TYPE_TEXTURE:
            ((D3DTexture*)pPixelContainer)->GetSurfaceLevel(iLevel, &pSurfSrc);
            break;
          case D3DCOMMON_TYPE_SURFACE:
            pSurfSrc = (D3DSurface*)pPixelContainer;
            break;
        }

        D3DDevice_CopyRects( pSurfSrc, NULL, 0, pSurfDest, NULL );

        pSurfDest->Release();
        switch(dwPixelContainerType)
        {
          case D3DCOMMON_TYPE_TEXTURE:  
            pSurfSrc->Release();
            break;
          case D3DCOMMON_TYPE_SURFACE:
            break;
        }
    }


    //
    // Hack the format info so that it's a viewable color-based format
    // (in case it was z-based, or whatever)
    //
    D3DFORMAT fmtOld = (D3DFORMAT)((pPixelContainer->Format & D3DFORMAT_FORMAT_MASK) >> D3DFORMAT_FORMAT_SHIFT);
    D3DFORMAT fmtTemp = MapToColorFormat(fmtOld); // remap it
    pTexCopy->Format = (pTexCopy->Format & ~D3DFORMAT_FORMAT_MASK) | (fmtTemp << D3DFORMAT_FORMAT_SHIFT);


    //
    // Calculate output size and position
    //
    UINT outpSizeX, outpSizeY;
    if(    (g_bDebugShrinkToFit && (desc.Width > 400 || desc.Height > 400))
        || (g_bDebugExpandToFit && (desc.Width < 400 && desc.Height < 400)) )
    {
        // modify size to fit well on screen (but maintain aspect ratio)
        if( desc.Width > desc.Height )
        {
            outpSizeX = 400;
            outpSizeY = int( desc.Height * (400.0f / float(desc.Width))  );
        }
        else
        {
            outpSizeY = 400;
            outpSizeX = int( desc.Width  * (400.0f / float(desc.Height)) );
        }
    }
    else
    {
        // don't modify size of texture/surface when displaying on screen
        outpSizeX = desc.Width;
        outpSizeY = desc.Height;
    }

    D3DDISPLAYMODE DisplayMode;
    D3DDevice_GetDisplayMode(&DisplayMode);
    UINT outpOffsetX = (DisplayMode.Width  - outpSizeX) / 2;  // center texture
    UINT outpOffsetY = (DisplayMode.Height - outpSizeY) / 2;

    // Account for linear vs swizzled tex coords.
    float maxU, maxV;
    if( XGIsSwizzledFormat(desc.Format) )
    {
        maxU = 1.0f;
        maxV = 1.0f;
    } else {
        maxU = (float)(desc.Width);
        maxV = (float)(desc.Height);
    }

    // Setup geometry for displaying texture/surface on screen.
    struct
    {
        float x, y, z, w;
        float u, v;
    }
    rgQuad[4] =
    {
        {outpOffsetX - 0.5f,             outpOffsetY - 0.5f,             1.0f, 1.0f, 0.0f, 0.0f },
        {outpOffsetX + outpSizeX - 0.5f, outpOffsetY - 0.5f,             1.0f, 1.0f, maxU, 0.0f },
        {outpOffsetX - 0.5f,             outpOffsetY + outpSizeY - 0.5f, 1.0f, 1.0f, 0.0f, maxV },
        {outpOffsetX + outpSizeX - 0.5f, outpOffsetY + outpSizeY - 0.5f, 1.0f, 1.0f, maxU, maxV }
    };


    //
    // Setup render states and texture states as necessary
    //
    // Notice that we set a lot of state here, because we may be called at any
    // time.  The device could be set to some arbitrary state, which could
    // cause rendering problems unless we set all states that we rely upon.
    //
    SETUP_RENDER_TARGET();

    SET_D3DVERTEXSHADER( D3DFVF_XYZRHW | D3DFVF_TEX1 );
    SET_D3DPIXELSHADER( 0 );

    SET_D3DRS( D3DRS_FILLMODE,                     D3DFILL_SOLID );
    SET_D3DRS( D3DRS_BACKFILLMODE,                 D3DFILL_SOLID );
    SET_D3DRS( D3DRS_CULLMODE,                     D3DCULL_NONE );
    SET_D3DRS( D3DRS_DITHERENABLE,                 TRUE );
    SET_D3DRS( D3DRS_ALPHATESTENABLE,              FALSE );
    SET_D3DRS( D3DRS_ALPHABLENDENABLE,             FALSE );
    SET_D3DRS( D3DRS_FOGENABLE,                    FALSE );
    SET_D3DRS( D3DRS_EDGEANTIALIAS,                FALSE );
    SET_D3DRS( D3DRS_STENCILENABLE,                FALSE );
    SET_D3DRS( D3DRS_LIGHTING,                     FALSE );
    SET_D3DRS( D3DRS_MULTISAMPLEMASK,              0xffffffff );
    SET_D3DRS( D3DRS_LOGICOP,                      D3DLOGICOP_NONE );
    SET_D3DRS( D3DRS_COLORWRITEENABLE,             D3DCOLORWRITEENABLE_ALL );
    SET_D3DRS( D3DRS_YUVENABLE,                    FALSE );
    SET_D3DRS( D3DRS_SPECULARENABLE,               FALSE );
    SET_D3DRS( D3DRS_ZBIAS,                        0 );
    SET_D3DRS( D3DRS_MULTISAMPLERENDERTARGETMODE,  D3DMULTISAMPLEMODE_1X );

    SET_D3DRS( D3DRS_ZENABLE,                      D3DZB_FALSE );

    SET_D3DTEXTURE( 0, pTexCopy );
    SET_D3DTSS( 0, D3DTSS_COLOROP,       D3DTOP_SELECTARG1 );
    SET_D3DTSS( 0, D3DTSS_COLORARG1,     g_bDebugAlpha ? D3DTA_TEXTURE|D3DTA_ALPHAREPLICATE : D3DTA_TEXTURE );
    SET_D3DTSS( 0, D3DTSS_ALPHAOP,       D3DTOP_SELECTARG1 );
    SET_D3DTSS( 0, D3DTSS_ALPHAARG1,     D3DTA_TEXTURE );
    SET_D3DTSS( 0, D3DTSS_TEXCOORDINDEX, 0 );
    SET_D3DTSS( 0, D3DTSS_ADDRESSU,      D3DTADDRESS_CLAMP );
    SET_D3DTSS( 0, D3DTSS_ADDRESSV,      D3DTADDRESS_CLAMP );
    SET_D3DTSS( 0, D3DTSS_ADDRESSW,      D3DTADDRESS_CLAMP );
    SET_D3DTSS( 0, D3DTSS_COLORKEYOP,    D3DTCOLORKEYOP_DISABLE );
    SET_D3DTSS( 0, D3DTSS_COLORSIGN,     0 );
    SET_D3DTSS( 0, D3DTSS_ALPHAKILL,     D3DTALPHAKILL_DISABLE );
    SET_D3DTSS( 0, D3DTSS_MINFILTER,     D3DTEXF_POINT );
    SET_D3DTSS( 0, D3DTSS_MAGFILTER,     D3DTEXF_POINT );

    float fMipBias = -1000.f; // bias mipmap toward the more detailed level
    SET_D3DTSS( 0, D3DTSS_MIPMAPLODBIAS, *((DWORD*) (&fMipBias)) );
    SET_D3DTSS( 0, D3DTSS_MAXMIPLEVEL,   0 );
    SET_D3DTSS( 0, D3DTSS_MIPFILTER,     D3DTEXF_POINT );
    SET_D3DTEXTURE( 1, NULL );
    SET_D3DTSS( 1, D3DTSS_COLOROP,       D3DTOP_DISABLE );
    SET_D3DTSS( 1, D3DTSS_ALPHAOP,       D3DTOP_DISABLE );
    SET_D3DTSS( 1, D3DTSS_ALPHAKILL,     D3DTALPHAKILL_DISABLE );
    SET_D3DTEXTURE( 2, NULL );
    SET_D3DTSS( 2, D3DTSS_COLOROP,       D3DTOP_DISABLE );
    SET_D3DTSS( 2, D3DTSS_ALPHAOP,       D3DTOP_DISABLE );
    SET_D3DTSS( 2, D3DTSS_ALPHAKILL,     D3DTALPHAKILL_DISABLE );
    SET_D3DTEXTURE( 3, NULL );
    SET_D3DTSS( 3, D3DTSS_COLOROP,       D3DTOP_DISABLE );
    SET_D3DTSS( 3, D3DTSS_ALPHAOP,       D3DTOP_DISABLE );
    SET_D3DTSS( 3, D3DTSS_ALPHAKILL,     D3DTALPHAKILL_DISABLE );


    //
    // Draw the texture/surface
    //
    DebugClear();
    D3DDevice_DrawVerticesUP(D3DPT_TRIANGLESTRIP, 4, rgQuad, sizeof(rgQuad[0]));
    DebugPresent();

    D3DDevice_SetTexture(0,NULL); // so D3D won't complain when we reset tex-addressing modes
                                  // (eg, to avoid case where linear tex + wrap mode gets set)

    //
    // Cleanup
    //
    if( NULL != pTexCopy )  { pTexCopy->Release(); }
    if( bSpecialCodeSurface  &&  NULL != pPixelContainer )
    {
        pPixelContainer->Release();
    }

    return S_OK;
}




