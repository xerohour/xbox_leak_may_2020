//-----------------------------------------------------------------------------
// FILE: FILLRATE.CPP
//
// Desc: Tool to help demonstrate different fillrates of the Xbox GPU
//
// Copyright (c) 2001 Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#include <xtl.h>
#include <stdio.h>
#include <assert.h>
#include <d3d8perf.h>

#include "utils.h"
#include "inifile.h"
#include "fillrate.h"




//-----------------------------------------------------------------------------
// Define for whether we use the VisibilityTest apis. Note that
// BeginVisibilityTest() will turn off some blend optimizations and
// it doesn't count pixels which aren't drawn due to alpha test or
// the like so we don't use it by default.
#define NO_VIS_TEST




//-----------------------------------------------------------------------------
// Globals
IDirect3DDevice8       *g_pDev = NULL;                  // our device
IDirect3DVertexBuffer8 *g_pVB = NULL;                   // our vertex buffer
IDirect3DBaseTexture8  *g_pTexture[4] = { NULL };       // 4 textures
IDirect3DSurface8      *g_pBackBuffer = NULL;

DWORD                   g_ShaderHandle = 0;             // pixel shader handle

DWORD                   g_dwFVFSize;                    // FVF size

UINT                    g_cindices = 0;                 // count of indices
WORD*                   g_pindices = NULL;              // indices allocation

HANDLE                  g_hInpDevice = NULL;            // input device

CScrText                g_scrtext;                      // text helper
CIniFile                g_inifile("d:\\fillrate.ini");  // Ini file

bool                    g_fRedrawScreen = true;         // refresh screen?
bool                    g_fdumpZ = false;               // dump Z val

FRSETTINGS              g_var = {0};                    // current settings

bool                    g_noINIFile = true;             //Do we have an INI file to test?


//-----------------------------------------------------------------------------
// Suite of available tests. Selectable via g_iCurTest in ini file
//  or by using left/right gamepad trigger.
//-----------------------------------------------------------------------------
struct
{
    LPCSTR szDesc;
    void (*pfnTest)();
} g_rgTests[] =
{
    { "Current",        DoQuickTimedTest },
    { "MultiSample",    DoMultiSampleTimedTest },
    { "FillRate",       DoCompleteTimedTest },
    { "Texture",   DoTextureTest },
};

UINT g_iCurTest = 0;




//-----------------------------------------------------------------------------
// Name: InitD3D
//
// Desc: Starts D3D
//-----------------------------------------------------------------------------
bool InitD3D()
{
    static D3DPRESENT_PARAMETERS d3dppCur = {0};
    D3DPRESENT_PARAMETERS d3dpp = {0};

    // Set the screen mode.
    d3dpp.BackBufferWidth                   = g_var.screenwidth;
    d3dpp.BackBufferHeight                  = g_var.screenheight;
    d3dpp.BackBufferFormat                  = g_var.BackBufferFormat;
    d3dpp.BackBufferCount                   = 1;
    d3dpp.EnableAutoDepthStencil            = !!g_var.AutoDepthStencilFormat;
    d3dpp.AutoDepthStencilFormat            = g_var.AutoDepthStencilFormat;
    d3dpp.SwapEffect                        = D3DSWAPEFFECT_DISCARD;
    d3dpp.FullScreen_RefreshRateInHz        = 60;
    d3dpp.FullScreen_PresentationInterval   = D3DPRESENT_INTERVAL_IMMEDIATE;
    d3dpp.Flags                             = D3DPRESENTFLAG_LOCKABLE_BACKBUFFER;

    if( g_var.MSType != D3DMULTISAMPLE_NONE )
        d3dpp.MultiSampleType = (D3DMULTISAMPLE_TYPE)((DWORD)(g_var.MSType) | 
                                                      (DWORD)(g_var.MSFormat));

    // if nothing has changed - don't reinit
    if( !memcmp( &d3dppCur, &d3dpp, sizeof( D3DPRESENT_PARAMETERS ) ) )
        return true;

    d3dppCur = d3dpp;

    if( g_pDev )
    {
        if( g_ShaderHandle )
        {
            CheckHR( g_pDev->SetPixelShader(0) );
            CheckHR( g_pDev->DeletePixelShader(g_ShaderHandle) );
            g_ShaderHandle = 0;
        }

        // make sure everything is freed
        RELEASE( g_pVB );
        for( int itex = 0; itex < 4; itex++ )
            RELEASE( g_pTexture[itex] );

        RELEASE( g_pBackBuffer );
        RELEASE( g_pDev );
    }

    // Create the device.
    if FAILED(Direct3D_CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL,
        NULL, D3DCREATE_HARDWARE_VERTEXPROCESSING, &d3dpp, &g_pDev) )
    {
        CheckHR( E_FAIL );
        return false;
    }

    D3DDevice_GetBackBuffer( 0, D3DBACKBUFFER_TYPE_MONO, &g_pBackBuffer );
    return true;
}




//-----------------------------------------------------------------------------
// Name: FillTextureBits3D
//
// Desc: Fill a volume texture with stuff
//-----------------------------------------------------------------------------
void FillTextureBits3D( D3DLOCKED_BOX *plockVol, D3DFORMAT d3dFormat, 
                        DWORD dwHeight, DWORD dwWidth, DWORD dwDepth ) 
{
    DWORD dwBits = BitsPerPixelOfD3DFORMAT( d3dFormat );

    assert( !IsLinearFormat( d3dFormat ) );

    BYTE *pBits = (BYTE *)plockVol->pBits;
    for( DWORD d = 0; d < dwDepth; d++ )
    {
        for( DWORD y = 0; y < dwHeight; y++ )
        {
            for( DWORD x = 0; x < dwWidth; x++ )
            {
                DWORD c = ( ~y & 1 ) * ( y + 1 ) * 0xff / dwHeight;
                DWORD c2 = d * 0xff / ( dwDepth - 1 );

                switch( dwBits )
                {
                case 4:
                    x++;
                case 8:
                    *pBits++ = LOBYTE( c );
                    break;
                case 16:
                    *(WORD *)pBits = MAKEWORD( c2, c );
                    pBits += 2;
                    break;
                case 32:
                    *(DWORD *)pBits = D3DCOLOR_ARGB( 0x0, c, c, c );
                    pBits += 4;
                    break;
                default:
                    CheckHR( E_FAIL );
                    break;
                }
            }
        }
    }
}




//-----------------------------------------------------------------------------
// Name: FillTextureBits
//
// Desc: Fill a 2D cubemap face or texture with stuff
//-----------------------------------------------------------------------------
void FillTextureBits( D3DLOCKED_RECT *plockRect, D3DFORMAT d3dFormat, 
                     DWORD dwHeight, DWORD dwWidth )
{
    bool fIsLinear = IsLinearFormat( d3dFormat );
    DWORD dwBits = BitsPerPixelOfD3DFORMAT( d3dFormat );
    DWORD dwLine = (DWORD)plockRect->pBits;

    for ( DWORD y = 0; y < dwHeight; y++ )
    {
        DWORD dwAddr = dwLine;
        DWORD c = (~y & 1 ) * ( y + 1 ) * 0xff / dwHeight;

        for ( DWORD x = 0; x < dwWidth; x++ )
        {
            switch( dwBits )
            {
            case 4:
                *(BYTE *)dwAddr = LOBYTE( c );
                dwAddr += 1;
                x++;
                break;
            case 8:
                *(BYTE *)dwAddr = LOBYTE( c );
                dwAddr += 1;
                break;
            case 16:
                *(WORD *)dwAddr = MAKEWORD( c, c );
                dwAddr += 2;
                break;
            case 32:
                *(DWORD *)dwAddr = D3DCOLOR_ARGB( 0x0, c, c, c );
                dwAddr += 4;
                break;
            }
        }

        // pitch is only valid for linear textures
        if( fIsLinear )
            dwLine += plockRect->Pitch;
        else
            dwLine += dwWidth * dwBits / 8;
    }
}




//-----------------------------------------------------------------------------
// Name: MyCreateTexture
//
// Desc: Create a texture
//-----------------------------------------------------------------------------
IDirect3DBaseTexture8 *MyCreateTexture( D3DRESOURCETYPE d3dtype, 
                                       D3DFORMAT d3dFormat, DWORD dwWidth, 
                                       DWORD dwHeight, DWORD Levels )
{
    IDirect3DBaseTexture8 *pTexRet = NULL;

    if( d3dtype == D3DRTYPE_TEXTURE )
    {
        D3DLOCKED_RECT lockRect;
        IDirect3DTexture8 *pTexture = NULL;

        CheckHR( g_pDev->CreateTexture(dwWidth, dwHeight, Levels, 0, d3dFormat,
            D3DPOOL_MANAGED, &pTexture ) );

        for( DWORD ilevel = 0; ilevel < pTexture->GetLevelCount(); ilevel++ )
        {
            CheckHR( pTexture->LockRect( ilevel, &lockRect, NULL, 0 ) );
            FillTextureBits( &lockRect, d3dFormat, dwHeight, dwWidth );
            CheckHR( pTexture->UnlockRect( ilevel ) );
        }

        pTexRet = pTexture;
    }
    else if( d3dtype == D3DRTYPE_CUBETEXTURE )
    {
        D3DLOCKED_RECT lockRect;
        IDirect3DCubeTexture8 *pCubeTexture = NULL;

        CheckHR( g_pDev->CreateCubeTexture( dwWidth, Levels, 0, d3dFormat,
            D3DPOOL_MANAGED, &pCubeTexture ) );

        for( DWORD ilevel = 0; ilevel < pCubeTexture->GetLevelCount(); ilevel++ )
        {
            for( int iFace = 0; iFace < D3DCUBEMAP_FACE_NEGATIVE_Z; iFace++ )
            {
                D3DCUBEMAP_FACES cmFace = (D3DCUBEMAP_FACES)iFace;

                CheckHR( pCubeTexture->LockRect( cmFace, ilevel, 
                                                 &lockRect, NULL, 0 ) );
                FillTextureBits( &lockRect, d3dFormat, dwHeight, dwWidth );
                CheckHR( pCubeTexture->UnlockRect( cmFace, ilevel ) );
            }
        }

        pTexRet = pCubeTexture;
    }
    else
    {
        assert( d3dtype == D3DRTYPE_VOLUMETEXTURE );

        D3DLOCKED_BOX lockVol;
        IDirect3DVolumeTexture8 *pVolumeTexture = NULL;

        CheckHR( g_pDev->CreateVolumeTexture( dwWidth, dwHeight, 
                                              dwHeight, Levels, 0,
                                              d3dFormat, D3DPOOL_MANAGED, 
                                              &pVolumeTexture ) );

        for( DWORD ilevel = 0; ilevel < pVolumeTexture->GetLevelCount(); ilevel++ )
        {
            CheckHR( pVolumeTexture->LockBox( ilevel, &lockVol, NULL, 0 ) );
            FillTextureBits3D( &lockVol, d3dFormat, dwHeight, dwWidth, dwWidth );
            CheckHR( pVolumeTexture->UnlockBox( ilevel ) );
        }

        pTexRet = pVolumeTexture;
    }

   return pTexRet;
}




//-----------------------------------------------------------------------------
// Name: CreateQuadList
//
// Desc: Create a quadlist vb with X quads and decreasing z values
//-----------------------------------------------------------------------------
void CreateQuadList( int nquads )
{
    RELEASE( g_pVB );

    if( g_cindices ) 
    {
        delete [] g_pindices;
        g_cindices = 0;
    }

    g_dwFVFSize = D3DXGetFVFVertexSize( g_var.FVF );

    if( g_var.verboselevel >= 2 )
        dprintf( "FVF:0x%08lx, FVFSize: %d\n", g_var.FVF, g_dwFVFSize );

    UINT VBLength = g_dwFVFSize * 4 * nquads;

    CheckHR( g_pDev->CreateVertexBuffer( VBLength,
        D3DUSAGE_WRITEONLY, g_var.FVF, D3DPOOL_MANAGED, &g_pVB ) );

    float *pVerts;
    CheckHR( g_pVB->Lock( 0, g_dwFVFSize, (BYTE **)&pVerts, 0 ) );

    float texscale = g_var.rgtex[0].type && 
                     IsLinearFormat( g_var.rgtex[0].d3dFormat ) ? 
                     g_var.rgtex[0].texsize : 1.0f;

    float dz = 1.0f / ( nquads * 2 );
    float zval = 1.0f - dz;
    for( int iquad = 0; iquad < nquads; iquad++, zval -= dz )
    {
        for( int y = 0; y <= 1; y++ )
        {
            for( int x = 0; x <= 1; x++ )
            {
                D3DXVECTOR4 pos;
                D3DXVECTOR4 tex0;

                // position
                pos.x = g_var.quadx + x * g_var.quadwidth;
                pos.y = g_var.quady + y * g_var.quadheight;
                pos.z = g_var.tiltz ? y : zval;
                pos.w = 1.0f;

                // texture coordinates
                if( g_var.rotatetex )
                {
                    tex0.x = ( 1.0f - y ) * texscale;
                    tex0.y = x * texscale;
                }
                else
                {
                    tex0.x = x * texscale;
                    tex0.y = y * texscale;
                }
                tex0.z = ( 1.0f - x ) * texscale;
                tex0.w = 1.0f;

                // diffuse color
                DWORD diffuse = D3DCOLOR_COLORVALUE( tex0.x,
                    tex0.y, 1.0f - tex0.x, 2.0f / 0xff );

                if( g_var.FVF & D3DFVF_XYZRHW )
                {
                    *(D3DXVECTOR4 *)pVerts = pos;
                    pVerts += 4;
                }
                else if( g_var.FVF & D3DFVF_XYZ )
                {
                    *(D3DXVECTOR3 *)pVerts = *(D3DXVECTOR3 *)&pos;
                    pVerts += 3;
                }

                if( g_var.FVF & D3DFVF_NORMAL )
                {
                    *(D3DXVECTOR3 *)pVerts = D3DXVECTOR3( 0, .707f, .707f );
                    pVerts += 3;
                }

                if( g_var.FVF & D3DFVF_DIFFUSE )
                    *(DWORD *)pVerts++ = diffuse;

                if( g_var.FVF & D3DFVF_SPECULAR )
                    *(DWORD *)pVerts++ = ~diffuse;

                // Texture coordinates
                UINT uNumTexCoords = ( ( ( g_var.FVF ) & D3DFVF_TEXCOUNT_MASK ) 
                                       >> D3DFVF_TEXCOUNT_SHIFT );
                UINT uTextureFormats = g_var.FVF >> 16;

                if( uTextureFormats )
                {
                    for( UINT i = 0; i < uNumTexCoords; i++ )
                    {
                        switch( uTextureFormats & 3 )
                        {
                        case D3DFVF_TEXTUREFORMAT1:     // u
                            *pVerts++ = tex0.x;
                            break;
                        case D3DFVF_TEXTUREFORMAT2:     // u,v
                            *(D3DXVECTOR2 *)pVerts = *(D3DXVECTOR2 *)&tex0;
                            pVerts += 2;
                            break;
                        case D3DFVF_TEXTUREFORMAT3:     // s,t,r
                            *(D3DXVECTOR3 *)pVerts = *(D3DXVECTOR3 *)&tex0;
                            pVerts += 3;
                            break;
                        case D3DFVF_TEXTUREFORMAT4:     // s,t,r,q
                            *(D3DXVECTOR4 *)pVerts = tex0;
                            pVerts += 4;
                            break;
                        }

                        uTextureFormats >>= 2;
                    }
                }
                else while( uNumTexCoords-- )
                {
                    // u,v
                    *(D3DXVECTOR2 *)pVerts = *(D3DXVECTOR2 *)&tex0;
                    pVerts += 2;
                }
            }
        }
    }

    CheckHR( g_pVB->Unlock( ) );
    g_pVB->MoveResourceMemory( D3DMEM_VIDEO );

    // create out index buffer
    g_cindices = nquads * 4;
    g_pindices = new WORD [g_cindices];

    WORD *pindices = g_pindices;

    for( iquad = 0; iquad < nquads; iquad++ )
    {
        *pindices++ = iquad * 4 + 0;
        *pindices++ = iquad * 4 + 1;
        *pindices++ = iquad * 4 + 3;
        *pindices++ = iquad * 4 + 2;
    }
}




//-----------------------------------------------------------------------------
// Name: InitFR
//
// Desc: Initialize fillrate app
//-----------------------------------------------------------------------------
bool InitFR( bool freadINIFile )
{
    if( freadINIFile && !ReadFRIniFile( ) )
        dprintf( "%s not found.\n", g_inifile.GetFileName( ) );

    // init d3d
    if( !InitD3D( ) )
    {
        dprintf( "InitD3D failed.\n" );
        return false;
    }

    CheckHR( g_pDev->SetRenderState( D3DRS_SPECULARENABLE, g_var.specularenable ) );

    CheckHR( g_pDev->SetRenderState( D3DRS_MULTISAMPLEANTIALIAS, FALSE ) );

    CheckHR( g_pDev->SetRenderState( D3DRS_SWATHWIDTH, g_var.SwathWidth ) );
    CheckHR( g_pDev->SetRenderState( D3DRS_FILLMODE, g_var.d3dfillmode ) );
    CheckHR( g_pDev->SetRenderState( D3DRS_COLORWRITEENABLE, g_var.colorwriteenable ) );

    CheckHR( g_pDev->SetRenderState( D3DRS_ZENABLE, g_var.zenable ) );
    CheckHR( g_pDev->SetRenderState( D3DRS_ZWRITEENABLE, g_var.zwriteenable ) );
    CheckHR( g_pDev->SetRenderState( D3DRS_ZFUNC, g_var.d3dcmpfunc ) );

    CheckHR( g_pDev->SetRenderState( D3DRS_ALPHABLENDENABLE, g_var.alphablendenable ) );
    CheckHR( g_pDev->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_SRCALPHA ) );
    CheckHR( g_pDev->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA ) );

    CheckHR( g_pDev->SetRenderState( D3DRS_BLENDOP, D3DBLENDOP_ADD ) );

    CheckHR( g_pDev->SetRenderState( D3DRS_ALPHATESTENABLE, g_var.alphatestenable ) );
    CheckHR( g_pDev->SetRenderState( D3DRS_ALPHAFUNC, D3DCMP_GREATEREQUAL ) );
    CheckHR( g_pDev->SetRenderState( D3DRS_ALPHAREF, 0x00000001 ) );

    if( g_var.userpixelshader )
        CheckHR( g_pDev->CreatePixelShader( &g_var.shaderdef, &g_ShaderHandle ) );

    for( int itex = 0; itex < 4; itex++ )
    {
        RELEASE( g_pTexture[itex] );

        if( g_var.rgtex[itex].type )
        {
            assert( g_var.rgtex[itex].type < TEX_Max );
            g_var.rgtex[itex].type = min( g_var.rgtex[itex].type, TEX_Max - 1 );

            static const D3DRESOURCETYPE rgrestype[] =
                { D3DRTYPE_TEXTURE, D3DRTYPE_CUBETEXTURE, D3DRTYPE_VOLUMETEXTURE };
            assert( TEX_2d == 1 && TEX_Cubemap == 2 && TEX_Volume == 3 );

            g_pTexture[itex] = MyCreateTexture(
                rgrestype[g_var.rgtex[itex].type - 1],
                g_var.rgtex[itex].d3dFormat,
                g_var.rgtex[itex].texsize,
                g_var.rgtex[itex].texsize,
                (g_var.rgtex[itex].mipfilter != D3DTEXF_NONE) ? 0 : 1 );

            CheckHR( g_pDev->SetTextureStageState( itex, D3DTSS_MINFILTER, 
                                                   g_var.rgtex[itex].filtertype ) );
            CheckHR( g_pDev->SetTextureStageState( itex, D3DTSS_MAGFILTER, 
                                                   g_var.rgtex[itex].filtertype ) );
            CheckHR( g_pDev->SetTextureStageState( itex, D3DTSS_MIPFILTER, 
                                                   g_var.rgtex[itex].mipfilter ) );

            // Set the texture stage states appropriately
            CheckHR( g_pDev->SetTextureStageState( itex, D3DTSS_COLORARG1, 
                                                   D3DTA_TEXTURE ) );
            CheckHR( g_pDev->SetTextureStageState( itex, D3DTSS_COLOROP, 
                                                   g_var.ColorOp ) );
            CheckHR( g_pDev->SetTextureStageState( itex, D3DTSS_COLORARG2, 
                                                   D3DTA_DIFFUSE ) );

            CheckHR( g_pDev->SetTextureStageState( itex, D3DTSS_ALPHAARG1, 
                                                   D3DTA_TEXTURE ) );
            CheckHR( g_pDev->SetTextureStageState( itex, D3DTSS_ALPHAOP, 
                                                   g_var.AlphaOp ) );
            CheckHR( g_pDev->SetTextureStageState( itex, D3DTSS_ALPHAARG2, 
                                                   D3DTA_DIFFUSE ) );
        }
        else
        {
            CheckHR( g_pDev->SetTextureStageState( itex, D3DTSS_COLOROP, 
                                                   D3DTOP_DISABLE ) );
            CheckHR( g_pDev->SetTextureStageState( itex, D3DTSS_ALPHAOP, 
                                                   D3DTOP_DISABLE ) );

            CheckHR( g_pDev->SetTextureStageState( itex, D3DTSS_MINFILTER, 
                                                   D3DTEXF_LINEAR ) );
            CheckHR( g_pDev->SetTextureStageState( itex, D3DTSS_MAGFILTER, 
                                                   D3DTEXF_LINEAR ) );
            CheckHR( g_pDev->SetTextureStageState( itex, D3DTSS_MIPFILTER, 
                                                   D3DTEXF_NONE ) );
        }

        CheckHR( g_pDev->SetTextureStageState( itex, D3DTSS_ADDRESSU, 
                                               D3DTADDRESS_CLAMP ) );
        CheckHR( g_pDev->SetTextureStageState( itex, D3DTSS_ADDRESSV, 
                                               D3DTADDRESS_CLAMP ) );
    }

    CreateQuadList( g_var.overdraw );
    return true;
}




//-----------------------------------------------------------------------------
// Name: DumpZ
//
// Desc: Dump the first entry in the z buffer
//-----------------------------------------------------------------------------
void DumpZ( )
{
    D3DLOCKED_RECT lockRect;
    D3DSURFACE_DESC desc;
    IDirect3DSurface8 *pZBuffer = NULL;

    if( !g_var.AutoDepthStencilFormat )
        return;

    CheckHR( g_pDev->GetDepthStencilSurface( &pZBuffer ) );

    CheckHR( pZBuffer->LockRect( &lockRect, NULL, D3DLOCK_READONLY ) );
    CheckHR( pZBuffer->GetDesc( &desc ) );

    DWORD dwVal;
    switch( desc.Format )
    {
    case D3DFMT_LIN_D16:
    case D3DFMT_D16:
        // v = z[15:0]  0.0 to 65535.0
        dprintf("D3DFMT_D16: 0x%08lx", *(WORD *)lockRect.pBits);
        break;

    case D3DFMT_LIN_D24S8:
    case D3DFMT_D24S8:
        // v = z[24:0] 0.0 to 16,777,215.0  (0 to FFFFFF)
        dwVal = ( *(DWORD *)lockRect.pBits );
        dprintf("D3DFMT_D24S8: 0x%08lx:%02lx %.2f", dwVal >> 8, LOBYTE( dwVal ),
               ( dwVal >> 8 ) / 16777215.0f );
        break;

    case D3DFMT_LIN_F16:
    case D3DFMT_F16:
        // e4m12
        // e = z[15:12]
        // m = z[11:0]
        // value = 2^(e-7) * (1 + m/4096)
        // value = 0.0 when e == 0 and m == 0
        dprintf( "D3DFMT_F16: 0x%08lx", *(WORD *)lockRect.pBits );
        break;

    case D3DFMT_LIN_F24S8:
    case D3DFMT_F24S8:
        // e8m16
        // e = z[23:16]
        // m = z[15:0]
        // value = 2^(e-127) * (1 + m/65536)
        // value = 0.0 when e == 0 and m == 0
        dprintf( "D3DFMT_F24S8: 0x%08lx", *(DWORD *)lockRect.pBits );
        break;
    }

    pZBuffer->UnlockRect();

    RELEASE( pZBuffer );
}




//-----------------------------------------------------------------------------
// Name: RenderFrame
//
// Desc: render the quads X # of times
//-----------------------------------------------------------------------------
void RenderFrame( int overdraw )
{
    UINT coverdrawverts = overdraw * 4;

    // clear stuff
    DWORD dwCFlags = g_var.clearflags;
    if( !g_var.AutoDepthStencilFormat )
        dwCFlags &= ~( D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL );

    CheckHR( g_pDev->Clear( 0, NULL, dwCFlags, g_var.cclearval,
        g_var.zclearval, g_var.sclearval ) );

    // begin scene
    CheckHR( g_pDev->BeginScene() );

    // set textures
    for( int itex = 0; itex < 4; itex++ )
        CheckHR( g_pDev->SetTexture( itex, g_pTexture[itex] ) );

    CheckHR( g_pDev->SetVertexShader( g_var.FVF ) );
    CheckHR( g_pDev->SetStreamSource( 0, g_pVB, g_dwFVFSize ) );

    CheckHR( g_pDev->SetPixelShader( g_ShaderHandle ) );

    if( g_var.zenable && g_var.primez )
    {
        // prime the z buffer to 0.5f
        CheckHR( g_pDev->SetRenderState( D3DRS_ZFUNC, g_var.primezcmpfunc ) );
        CheckHR( g_pDev->SetRenderState( D3DRS_ZWRITEENABLE, TRUE ) );

        // draw one quad (not the first one) to prime the z buffer
        CheckHR( g_pDev->DrawIndexedVertices( D3DPT_QUADLIST, 4, g_pindices + 4 ) );
        coverdrawverts -= 4;

        CheckHR( g_pDev->SetRenderState( D3DRS_ZFUNC, g_var.d3dcmpfunc ) );
        CheckHR( g_pDev->SetRenderState( D3DRS_ZWRITEENABLE, g_var.zwriteenable ) );
    }

    while( coverdrawverts )
    {
        UINT VertexCount = min( g_cindices, coverdrawverts );
        CheckHR( g_pDev->DrawIndexedVertices( D3DPT_QUADLIST, VertexCount, g_pindices ) );

        coverdrawverts -= VertexCount;
    }

    if( g_fdumpZ )
        DumpZ();

    CheckHR( g_pDev->EndScene() );
}




//-----------------------------------------------------------------------------
// Deadzone for thumbsticks
#define XBINPUT_DEADZONE 0.24f

//-----------------------------------------------------------------------------
// Global instance of input states
XINPUT_STATE g_InputStates[4];




//-----------------------------------------------------------------------------
// Name: struct XBGAMEPAD
// Desc: structure for holding Gamepad data
//-----------------------------------------------------------------------------
struct XBGAMEPAD : public XINPUT_GAMEPAD
{
    // The following members are inherited from XINPUT_GAMEPAD:
    //    WORD    wButtons;
    //    BYTE    bAnalogButtons[8];
    //    SHORT   sThumbLX;
    //    SHORT   sThumbLY;
    //    SHORT   sThumbRX;
    //    SHORT   sThumbRY;

    // Thumb stick values converted to range [-1,+1]
    FLOAT      fX1;
    FLOAT      fY1;
    FLOAT      fX2;
    FLOAT      fY2;
    
    // State of buttons tracked since last poll
    WORD       wLastButtons;
    BOOL       bLastAnalogButtons[8];
    WORD       wPressedButtons;
    BOOL       bPressedAnalogButtons[8];

    // Rumble properties
    XINPUT_RUMBLE   Rumble;
    XINPUT_FEEDBACK Feedback;

    // Device properties
    XINPUT_CAPABILITIES caps;
    HANDLE     hDevice;

    // Flags for whether gamepad was just inserted or removed
    BOOL       bInserted;
    BOOL       bRemoved;
};

//-----------------------------------------------------------------------------
// Global instance of custom gamepad devices
XBGAMEPAD g_Gamepads[4];




//-----------------------------------------------------------------------------
// Name: XBInput_CreateGamepads()
// Desc: Creates the gamepad devices
//-----------------------------------------------------------------------------
HRESULT XBInput_CreateGamepads( )
{
    // Get a mask of all currently available devices
    DWORD dwDeviceMask = XGetDevices( XDEVICE_TYPE_GAMEPAD );

    // Open the devices
    for( DWORD i=0; i < XGetPortCount(); i++ )
    {
        ZeroMemory( &g_InputStates[i], sizeof(XINPUT_STATE) );
        ZeroMemory( &g_Gamepads[i], sizeof(XBGAMEPAD) );
        if( dwDeviceMask & (1<<i) ) 
        {
            // Get a handle to the device
            g_Gamepads[i].hDevice = XInputOpen( XDEVICE_TYPE_GAMEPAD, i, 
                                                XDEVICE_NO_SLOT, NULL );

            // Store capabilites of the device
            XInputGetCapabilities( g_Gamepads[i].hDevice, &g_Gamepads[i].caps );
        }
    }

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: XBInput_GetInput()
// Desc: Processes input from the gamepads
//-----------------------------------------------------------------------------
VOID XBInput_GetInput( )
{
    XBGAMEPAD *pGamepads = g_Gamepads;

    // TCR 3-21 Controller Discovery
    // Get status about gamepad insertions and removals. Note that, in order to
    // not miss devices, we will check for removed device BEFORE checking for
    // insertions
    DWORD dwInsertions, dwRemovals;
    XGetDeviceChanges( XDEVICE_TYPE_GAMEPAD, &dwInsertions, &dwRemovals );

    // Loop through all gamepads
    for( DWORD i=0; i < XGetPortCount(); i++ )
    {
        // Handle removed devices.
        pGamepads[i].bRemoved = ( dwRemovals & (1<<i) ) ? TRUE : FALSE;
        if( pGamepads[i].bRemoved )
        {
            XInputClose( pGamepads[i].hDevice );
            pGamepads[i].hDevice = NULL;
            pGamepads[i].Feedback.Rumble.wLeftMotorSpeed  = 0;
            pGamepads[i].Feedback.Rumble.wRightMotorSpeed = 0;
        }

        // Handle inserted devices
        pGamepads[i].bInserted = ( dwInsertions & (1<<i) ) ? TRUE : FALSE;
        if( pGamepads[i].bInserted ) 
        {
            // TCR 1-14 Device Types
            pGamepads[i].hDevice = XInputOpen( XDEVICE_TYPE_GAMEPAD, i, 
                                               XDEVICE_NO_SLOT, NULL );
            XInputGetCapabilities( pGamepads[i].hDevice, &pGamepads[i].caps );
        }

        // If we have a valid device, poll it's state and track button changes
        if( pGamepads[i].hDevice )
        {
            // Read the input state
            XInputGetState( pGamepads[i].hDevice, &g_InputStates[i] );

            // Copy gamepad to local structure
            memcpy( &pGamepads[i], &g_InputStates[i].Gamepad, sizeof(XINPUT_GAMEPAD) );

            // Put Xbox device input for the gamepad into our custom format
            FLOAT fX1 = (pGamepads[i].sThumbLX+0.5f)/32767.5f;
            pGamepads[i].fX1 = ( fX1 >= 0.0f ? 1.0f : -1.0f ) *
                               max( 0.0f, (fabsf(fX1)-XBINPUT_DEADZONE)/(1.0f-XBINPUT_DEADZONE) );

            FLOAT fY1 = (pGamepads[i].sThumbLY+0.5f)/32767.5f;
            pGamepads[i].fY1 = ( fY1 >= 0.0f ? 1.0f : -1.0f ) *
                               max( 0.0f, (fabsf(fY1)-XBINPUT_DEADZONE)/(1.0f-XBINPUT_DEADZONE) );

            FLOAT fX2 = (pGamepads[i].sThumbRX+0.5f)/32767.5f;
            pGamepads[i].fX2 = ( fX2 >= 0.0f ? 1.0f : -1.0f ) *
                               max( 0.0f, (fabsf(fX2)-XBINPUT_DEADZONE)/(1.0f-XBINPUT_DEADZONE) );

            FLOAT fY2 = (pGamepads[i].sThumbRY+0.5f)/32767.5f;
            pGamepads[i].fY2 = ( fY2 >= 0.0f ? 1.0f : -1.0f ) *
                               max( 0.0f, (fabsf(fY2)-XBINPUT_DEADZONE)/(1.0f-XBINPUT_DEADZONE) );

            // Get the boolean buttons that have been pressed since the last
            // call. Each button is represented by one bit.
            pGamepads[i].wPressedButtons = ( pGamepads[i].wLastButtons ^ pGamepads[i].wButtons ) & pGamepads[i].wButtons;
            pGamepads[i].wLastButtons    = pGamepads[i].wButtons;

            // Get the analog buttons that have been pressed or released since
            // the last call.
            for( DWORD b=0; b<8; b++ )
            {
                // Turn the 8-bit polled value into a boolean value
                BOOL bPressed = ( pGamepads[i].bAnalogButtons[b] > XINPUT_GAMEPAD_MAX_CROSSTALK );

                if( bPressed )
                    pGamepads[i].bPressedAnalogButtons[b] = !pGamepads[i].bLastAnalogButtons[b];
                else
                    pGamepads[i].bPressedAnalogButtons[b] = FALSE;
                
                // Store the current state for the next time
                pGamepads[i].bLastAnalogButtons[b] = bPressed;
            }
        }
    }
}




//-----------------------------------------------------------------------------
// Name: FButtonDown
//
// Desc: Check if button is pressed.
//-----------------------------------------------------------------------------
bool FButtonDown( DWORD button )
{
    // Loop through all gamepads
    for( DWORD i=0; i < XGetPortCount(); i++ )
    {
        // If we have a valid device, poll it's state and track button changes
        if( g_Gamepads[i].hDevice && g_Gamepads[i].bPressedAnalogButtons[button] )
            return true;
    }

    return false;
}




//-----------------------------------------------------------------------------
// Name: ChkButtonDown
//
// Desc: Check if non-analog button is pressed.
//-----------------------------------------------------------------------------
bool ChkButtonDown( DWORD button )
{
    // Loop through all gamepads
    for( DWORD i=0; i < XGetPortCount(); i++ )
    {
        // If we have a valid device, poll it's state and track button changes
        if( g_Gamepads[i].hDevice && g_Gamepads[i].wPressedButtons & button )
            return true;
    }

    return false;
}




//-----------------------------------------------------------------------------
// Name: ChkReboot
//
// Desc: Check if they hit the magic reboot sequence
//-----------------------------------------------------------------------------
bool ChkReboot( void )
{
    for( DWORD i=0; i < XGetPortCount(); i++ )
    {
        // If we have a valid device, poll it's state and track button changes
        if( g_Gamepads[i].hDevice )
        {
            if( g_Gamepads[i].bAnalogButtons[XINPUT_GAMEPAD_LEFT_TRIGGER] > XINPUT_GAMEPAD_MAX_CROSSTALK )
            {
                if( g_Gamepads[i].bAnalogButtons[XINPUT_GAMEPAD_RIGHT_TRIGGER] > XINPUT_GAMEPAD_MAX_CROSSTALK )
                {
                    if( g_Gamepads[i].bAnalogButtons[XINPUT_GAMEPAD_BLACK] > XINPUT_GAMEPAD_MAX_CROSSTALK )
                    {
                        return true;
                    }
                }
            }
        }
    }


    return false;
}




//-----------------------------------------------------------------------------
// Name: HandleInput
//
// Desc: Handle Gamepad input
//-----------------------------------------------------------------------------
void HandleInput( DWORD dwTicks )
{
    static BOOL fRunTestRightNow = FALSE;

    if( g_inifile.FileChanged() )
    {
        InitFR( true );

        fRunTestRightNow = g_var.runtestonreload;

        g_fRedrawScreen = true;

        g_noINIFile = false;
    }

    // If we don't have a device don't let them start any tests.
    // They're going to have to change their settings in fillrate.ini
    // and just try again.
    if( g_noINIFile )
    {
        InitD3D();
        g_scrtext.cls();
        g_scrtext.printf("\n");
        g_scrtext.printf("Please be sure INI file was copied to d: drive on Xbox.\n");
        g_scrtext.printf("No INI File or could not parse INI file.\nNothing I can do.");
        g_scrtext.printf("You can copy INI right now!  I restart when it changes!");

        return;
    }

    XBInput_GetInput();

    if( fRunTestRightNow || FButtonDown( XINPUT_GAMEPAD_A ) )
    {
        fRunTestRightNow = FALSE;

        // run current test
        g_rgTests[g_iCurTest].pfnTest();

        // draw one frame with current settings
        RenderFrame( 1 );

        // flip that dude to the front
        CheckHR( g_pDev->Present( NULL, NULL, NULL, NULL ) );

        g_scrtext.drawtext( g_var.verboselevel >= 1 );
        g_fRedrawScreen = false;
    }

#ifdef _DEBUG
    if( FButtonDown( XINPUT_GAMEPAD_B ) )
    {
        D3DPUSHBUFFERINFO PushBufferInfo;
        D3DPERF_GetPushBufferInfo( &PushBufferInfo );

        D3DPERF_Dump();
        D3DPERF_Reset();
    }
#endif

    if( FButtonDown(XINPUT_GAMEPAD_LEFT_TRIGGER ) )
    {
        g_iCurTest = ( g_iCurTest + ARRAYSIZE( g_rgTests ) - 1 ) % 
                        ARRAYSIZE( g_rgTests );
        g_fRedrawScreen = true;
    }

    if( FButtonDown( XINPUT_GAMEPAD_RIGHT_TRIGGER ) )
    {
        g_iCurTest = ( g_iCurTest + 1 ) % ARRAYSIZE( g_rgTests );
        g_fRedrawScreen = true;
    }

    if( FButtonDown( XINPUT_GAMEPAD_Y ) )
    {
        // toggle wireframe and solid fill modes
        g_var.d3dfillmode = ( g_var.d3dfillmode == D3DFILL_WIREFRAME ) ?
            D3DFILL_SOLID : D3DFILL_WIREFRAME;

        CheckHR( g_pDev->SetRenderState( D3DRS_FILLMODE, g_var.d3dfillmode ) );
        g_fRedrawScreen = true;
    }

    if ( ChkButtonDown( XINPUT_GAMEPAD_BACK ) )
    {
        g_fRedrawScreen = true;
    }

    // Handle special input combo to trigger a reboot to the Xbox Dashboard
    if( ChkReboot() )
    {
        LD_LAUNCH_DASHBOARD LaunchData = { XLD_LAUNCH_DASHBOARD_MAIN_MENU };
        XLaunchNewImage( NULL, (LAUNCH_DATA*)&LaunchData );
    }
}




//-----------------------------------------------------------------------------
// Name: DoMultiSampleTimedTest
//
// Desc: Run through a suite of multisample/supersample tests
//-----------------------------------------------------------------------------
void DoMultiSampleTimedTest()
{
    D3DMULTISAMPLE_TYPE rgMSType[] =
    {
        D3DMS_NONE,
        D3DMS_2_SAMPLES_MULTISAMPLE_LINEAR,
        D3DMS_2_SAMPLES_MULTISAMPLE_QUINCUNX,
        D3DMS_4_SAMPLES_MULTISAMPLE_LINEAR,
        D3DMS_4_SAMPLES_MULTISAMPLE_GAUSSIAN,
        D3DMS_9_SAMPLES_MULTISAMPLE_GAUSSIAN,
        D3DMS_2_SAMPLES_SUPERSAMPLE_HORIZONTAL_LINEAR,
        D3DMS_2_SAMPLES_SUPERSAMPLE_VERTICAL_LINEAR,
        D3DMS_4_SAMPLES_SUPERSAMPLE_LINEAR,
        D3DMS_4_SAMPLES_SUPERSAMPLE_GAUSSIAN,
        D3DMS_9_SAMPLES_SUPERSAMPLE_GAUSSIAN
    };

    static const int cMSTypes = ARRAYSIZE( rgMSType );

    D3DFORMAT rgBackBuf[] = { D3DFMT_R5G6B5, D3DFMT_X8R8G8B8 };
    static const cBBufs = ARRAYSIZE( rgBackBuf );

    g_scrtext.cls();
    g_scrtext.printf( "" );
    for( int iBuf = 0; iBuf < cBBufs; iBuf++ )
    {
        g_scrtext.printf( "BackBufFmt: %-38s %-8s %-8s %s",
            GetD3DFormatStr( rgBackBuf[iBuf] ), "MPix/s", "fps", "frames" );

        for( int iType = 0; iType < cMSTypes; iType++ )
        {
            TIMETESTRESULTS ttres;

            g_var.MSType = rgMSType[iType];
            g_var.BackBufferFormat = rgBackBuf[iBuf];

            InitFR( false );
            DoTimedTest( &ttres );

            g_scrtext.printf( "    %-45s %7.2f  %8.2f  %d",
                GetMultiSampleStr( rgMSType[iType] ),
                ttres.fillrate, ttres.fps, ttres.cFrames );
        }

        //Display current results.
        // Hang out here until they press "a" to continue.
        if( (iBuf+1) < cBBufs )
        {
            g_scrtext.printf( "" );
            g_scrtext.printf( "Button (A)-Continue test.", 
                                g_rgTests[g_iCurTest].szDesc );

            // clear back buffer
            RenderFrame( 1 );

            // swap that guy to the front
            CheckHR( g_pDev->Present( NULL, NULL, NULL, NULL ) );

            // clear new back buffer
            RenderFrame( 1 );

            //DumpCurrentSettings();
            g_scrtext.drawtext( g_var.verboselevel >= 1 );

            g_scrtext.cls();

            g_scrtext.printf( "" );

            XBInput_GetInput();
    
            while( !FButtonDown( XINPUT_GAMEPAD_A ) )
                XBInput_GetInput();
        }
    }

    g_scrtext.printf( "" );
    g_scrtext.printf( "Button (A)-Run '%s' test.  Button (BACK)-Return to Main Screen.", 
                      g_rgTests[g_iCurTest].szDesc );

    // restore settings from ini file
    InitFR( true );
}




//-----------------------------------------------------------------------------
// Name: DoQuickTimedTest
//
// Desc: Do a quick test with the current settings
//-----------------------------------------------------------------------------
void DoQuickTimedTest()
{
    TIMETESTRESULTS ttres;

    DoTimedTest( &ttres );

    g_scrtext.cls();
    DumpCurrentSettings();

    g_scrtext.printf( " " );
    g_scrtext.printf( "fillrate: %.2fMP/s",  ttres.fillrate );
    g_scrtext.printf( "fps:      %.2f",      ttres.fps );
    g_scrtext.printf( "frames:   %d",        ttres.cFrames );
    g_scrtext.printf( "time:     %.2fms",    ttres.TimeTillIdle );
    g_scrtext.printf( "pixels:   %I64u",     ttres.cNumPixelsDrawn );
    g_scrtext.printf( " " );

    g_scrtext.printf( "" );
    g_scrtext.printf( "Button (A)-Run '%s' test.  Button (BACK)-Return to Main Screen.", 
                      g_rgTests[g_iCurTest].szDesc );

}




//-----------------------------------------------------------------------------
// Name: DoTextureTest
//
// Desc: Texture fillrate test
//-----------------------------------------------------------------------------
void DoTextureTest()
{
    D3DFORMAT rgFormats[] =
    {
        D3DFMT_A4R4G4B4,
        D3DFMT_A8R8G8B8,
        D3DFMT_DXT1,
        D3DFMT_DXT2,
        D3DFMT_P8,
        D3DFMT_LIN_A8R8G8B8
    };

    int rgTexSize[] = { 1, 16, 256 };
    float rgResults[ARRAYSIZE( rgTexSize )];

    g_scrtext.cls();

    g_scrtext.printf( "" );
    g_scrtext.printf( "%-20s %-10s %-10s %-10s",
        "texturefmt", "1x1", "16x16", "256x256" );

    for( int id3dformat = 0; id3dformat < ARRAYSIZE( rgFormats ); id3dformat++ )
    {
        char szBuf[128];

        szBuf[0] = 0;
        for( int itexsize = 0; itexsize < ARRAYSIZE( rgTexSize ); itexsize++ )
        {
            g_var.rgtex[0].type = TEX_2d;
            g_var.rgtex[0].texsize = rgTexSize[itexsize];
            g_var.rgtex[0].d3dFormat = rgFormats[id3dformat];

            InitFR( false );
            rgResults[itexsize] = DoTimedTest();
        }

        g_scrtext.printf(
            "%-20s %-10.2f %-10.2f %-10.2f",
            GetD3DFormatStr( g_var.rgtex[0].d3dFormat ),
            rgResults[0], rgResults[1], rgResults[2] );
    }

    g_scrtext.printf( "" );
    g_scrtext.printf( "Button (A)-Run '%s' test.  Button (BACK)-Return to Main Screen.", 
                      g_rgTests[g_iCurTest].szDesc );

    // restore settings from ini file
    InitFR( true );
}




//-----------------------------------------------------------------------------
// Name: DoCompleteTimedTest
//
// Desc: Run through a suite of fillrate tests
//-----------------------------------------------------------------------------
void DoCompleteTimedTest()
{
    D3DFORMAT rgFormats[] = { D3DFMT_DXT1, D3DFMT_A8R8G8B8, D3DFMT_A4R4G4B4 };

    g_scrtext.cls();

    g_scrtext.printf( "" );

    g_scrtext.printf(
        "zwrite zread texturefmt      texturesize fillrate  (alphablendenable:%d)",
        g_var.alphablendenable );

    for( int id3dformat = 0; id3dformat < ARRAYSIZE( rgFormats ); id3dformat++ )
    {
        for( int texsize = 0; texsize <= 512; texsize += 256 )
        {
            for( int zenable = 0; zenable < 2; zenable++ )
            {
                for( int zwriteenable = 0; zwriteenable < 2; zwriteenable++ )
                {
                    g_var.zenable = !!zenable;
                    g_var.zwriteenable = !!zwriteenable;

                    g_var.d3dcmpfunc = D3DCMP_NOTEQUAL;

                    g_var.rgtex[0].texsize = texsize;
                    g_var.rgtex[0].type = !!texsize;
                    g_var.rgtex[0].d3dFormat = rgFormats[id3dformat];

                    if( !g_var.zenable && zwriteenable )
                        continue;

                    if( !g_var.rgtex[0].type && id3dformat )
                        continue;

                    InitFR( false );
                    float fResult = DoTimedTest();

                    g_scrtext.printf(
                        "%-6d %-5d %-15s %-11d %.2f",
                        g_var.zwriteenable,
                        g_var.zenable,
                        texsize ? GetD3DFormatStr(g_var.rgtex[0].d3dFormat) : "",
                        texsize,
                        fResult );
                }
            }
        }
    }

    g_scrtext.printf( "" );
    g_scrtext.printf( "Button (A)-Run '%s' test.  Button (BACK)-Return to Main Screen.", 
                      g_rgTests[g_iCurTest].szDesc );

    // restore settings from ini file
    InitFR( true );
}




//-----------------------------------------------------------------------------
// Name: DumpCurrentSettings
//
// Desc: Dump the current options
//=========================================================================
void DumpCurrentSettings()
{
    g_scrtext.printf( "" );
    g_scrtext.printf( "Button (A) - Run '%s' test.", g_rgTests[g_iCurTest].szDesc );

#ifdef _DEBUG
    g_scrtext.printf( "Button (B) - Dump Performance counter Info (DEBUG Version ONLY!).", 
                      g_rgTests[g_iCurTest].szDesc );
#endif

    g_scrtext.printf( "Left/Right Trigger - Scroll through tests.");
    g_scrtext.printf( "" );


    g_scrtext.printf(
        "AutoDepthStencilFormat:%s "
        "BackBufferFormat:%s ",
        g_var.AutoDepthStencilFormat ? 
            GetD3DFormatStr( g_var.AutoDepthStencilFormat ) : "none",
        GetD3DFormatStr( g_var.BackBufferFormat ) );

    g_scrtext.printf(
        "MultiSampleType:%s ",
        GetMultiSampleStr( g_var.MSType ) );

    g_scrtext.printf(
        "MultiSampleFormat:%s",
        GetMultiSampleStr( g_var.MSFormat ) );

    g_scrtext.printf(
        "zenable:%d "
        "z-write:%d "
        "d3dcmpfunc:%s",
        g_var.zenable,
        g_var.zenable && g_var.zwriteenable,
        GetD3DCMPStr( g_var.d3dcmpfunc ) );

    g_scrtext.printf(
        "alpha-blend:%d "
        "alpha-test:%d "
        "swathwidth:%d ",
        g_var.alphablendenable,
        g_var.alphatestenable,
        ( g_var.SwathWidth == D3DSWATH_OFF ) ? -1 : ( 0x8 << g_var.SwathWidth ) );

    g_scrtext.printf(
        "timeflags:%s%s",
        ( g_var.timeflags & TIME_Render ) ? "TIME_Render " : "",
        ( g_var.timeflags & TIME_Present ) ? "TIME_Present" : "" );
                                         
    for( int itex = 0; itex < 4; itex++ )
    {
        if( g_var.rgtex[itex].type )
        {
            g_scrtext.printf(
                "%d) texfmt:%s "
                "texel size (bytes):%0.1f "
                "texsize: %d "
                "mips: %d",
                itex,
                GetD3DFormatStr( g_var.rgtex[itex].d3dFormat ),
                BitsPerPixelOfD3DFORMAT( g_var.rgtex[itex].d3dFormat ) / 8.0f,
                g_var.rgtex[itex].texsize,
                g_pTexture[itex]->GetLevelCount()
                );
        }
    }

    if( g_var.userpixelshader )
    {
        g_scrtext.printf(
            "CombinerCount:%d",
            g_var.shaderdef.PSCombinerCount & 0xf );

        UINT TexModes = g_var.shaderdef.PSTextureModes;
        for( int texmode = 0; texmode < 4; texmode++, TexModes >>= 5 )
        {
            if( TexModes & 0x1f )
            {
                g_scrtext.printf( "  T%d Mode:%s",
                    texmode, GetTextureModeStr( TexModes & 0x1f ) );
            }
        }
    }
}




//-----------------------------------------------------------------------------
// Name: EstimateTimeForScene
//
// Desc: Estimates how long GPU is spending rendering the scene
//-----------------------------------------------------------------------------
float EstimateTimeForScene()
{
    CTimer timer;
    int overdraw = min( 10, g_var.overdraw );

    // update the screen
    RenderFrame( 1 );
    CheckHR( g_pDev->Present( NULL, NULL, NULL, NULL ) );

    // wait until everything is cleared out
    g_pDev->BlockUntilIdle();

    // start timing
    timer.Start();

    if( g_var.timeflags & TIME_Render )
        RenderFrame(10);

    if( g_var.timeflags & TIME_Present )
    {
        CheckHR( g_pDev->Present( NULL, NULL, NULL, NULL ) );
    }

    // wait until GPU is idle again
    g_pDev->BlockUntilIdle();

    // stop timer
    timer.Stop();

    return g_var.overdraw * timer.getTime() / overdraw;
}




//-----------------------------------------------------------------------------
// Name: DoTimedTest
//
// Desc: Do a timed test and spit out the results
//-----------------------------------------------------------------------------
float DoTimedTest( TIMETESTRESULTS *pttres )
{
    CTimer timer;
    int frames = 0;
    int visibilityindex = 0;
    float TimeTillDoneDrawing = 0;

    // wait until everything is cleared out
    g_pDev->BlockUntilIdle();

    int numberscenes = g_var.numberscenes;

    if( !numberscenes )
    {
        // if there isn't an explicit count of scenes to render
        // guesstimate the time required for each scene
        float fEstimatedTimePerScene = EstimateTimeForScene();

        // and figure out how many scenes to render
        numberscenes = int( g_var.testtime / fEstimatedTimePerScene );
    }

    // put an upper cap and lower bound on the number of scenes
    numberscenes = max( 2, min( numberscenes, 2047 ) );

    // start timing
    timer.Start();

    // render X number of scenes
    for( frames = 0; frames < numberscenes; frames++ )
    {
#ifndef NO_VIS_TEST

        CheckHR( g_pDev->BeginVisibilityTest() );

#endif

        if( g_var.timeflags & TIME_Render )
            RenderFrame( g_var.overdraw );

        if( g_var.timeflags & TIME_Present )
        {
            CheckHR( g_pDev->Present( NULL, NULL, NULL, NULL ) );
        }

#ifndef NO_VIS_TEST

        CheckHR( g_pDev->EndVisibilityTest( visibilityindex++ ) );

#endif
    }

    TimeTillDoneDrawing = timer.getTime();

    // wait until GPU is idle again
    g_pDev->BlockUntilIdle();

    // stop timer
    timer.Stop();
    float TimeTillIdle = timer.getTime();

    // get count of pixels drawn
    UINT64 dwNumPixelsDrawn = 0;

#ifdef NO_VIS_TEST

    if( ( g_var.timeflags & TIME_Present ) == TIME_Present )
    {
        dwNumPixelsDrawn = (UINT64)g_var.screenwidth * 
                                   g_var.screenheight * frames;
    }
    else if( g_var.timeflags & TIME_Render ) 
    {
        // Get backbuffer size.
        D3DSURFACE_DESC Desc;
        CheckHR( g_pBackBuffer->GetDesc( &Desc ) );

        // Scale this bugger by the backbuffer size.
        DWORD Scale = ( Desc.Width * Desc.Height ) / 
                        ( g_var.screenwidth * g_var.screenheight );

        dwNumPixelsDrawn = (UINT64)g_var.screenwidth * g_var.screenheight *
                                    frames * g_var.overdraw * Scale;

        if( g_var.timeflags & TIME_Present )
            dwNumPixelsDrawn += (UINT64)g_var.screenwidth * 
                                        g_var.screenheight * frames;
    }

#else

    for( int ivis = 0; ivis < visibilityindex; ivis++ )
    {
        // Check the number of pixels drawn.
        UINT dwT;
        while( g_pDev->GetVisibilityTestResult( ivis, &dwT, NULL ) 
               == D3DERR_TESTINCOMPLETE )
            ;

        if( g_var.verboselevel >= 2 )
            dprintf( "pass:%d  %ld pixels\n", ivis, dwT );

        if( dwT == 0xffffffff )
            dwT = 0;

        dwNumPixelsDrawn += dwT;
    }

#endif

    // Calculate fillrate
    float fillrate = dwNumPixelsDrawn / ( TimeTillIdle * 1000.0f );

    if( pttres )
    {
        pttres->fillrate = fillrate;
        pttres->fps = frames * 1000 / TimeTillIdle;
        pttres->cFrames = frames;
        pttres->TimeTillDoneDrawing = TimeTillDoneDrawing;
        pttres->TimeTillIdle = TimeTillIdle;
        pttres->cNumPixelsDrawn = dwNumPixelsDrawn;
    }

    return fillrate;
}




//-----------------------------------------------------------------------------
// macros to help read entries from the ini file
//-----------------------------------------------------------------------------
static const char g_szOpts[] = "options";
#define get_option_val(_var)          g_inifile.GetIniInt(g_szOpts, #_var, g_var._var)
#define get_option_valf(_var)         g_inifile.GetIniFloat(g_szOpts, #_var, g_var._var)
#define get_option_val3(_s, _var, _d) g_inifile.GetIniInt(_s, _var, _d)




//-----------------------------------------------------------------------------
// Name: ReadFRIniFile
//
// Desc: Read ini file and update all fillrate settings
//-----------------------------------------------------------------------------
bool ReadFRIniFile()
{
    if( !g_inifile.ReadFile() )
        return false;

    if( g_ShaderHandle )
    {
        assert( g_pDev );
        CheckHR( g_pDev->SetPixelShader(0) );
        CheckHR( g_pDev->DeletePixelShader(g_ShaderHandle) );
        g_ShaderHandle = 0;
    }

    // read the verbose flag(s) first
    g_inifile.m_fverbose            = !!get_option_val3( g_szOpts, "verboseinireads", false );
    g_var.verboselevel              = get_option_val( verboselevel );

    // if the g_iCurTest entry exists, read it in
    if( g_inifile.IniEntryExists( g_szOpts, "g_iCurTest" ) )
        g_iCurTest = g_inifile.GetIniInt( g_szOpts, "g_iCurTest", g_iCurTest );

    g_var.runtestonreload           = !!get_option_val( runtestonreload );

    g_var.AutoDepthStencilFormat    = (D3DFORMAT)get_option_val( AutoDepthStencilFormat );
    g_var.BackBufferFormat          = (D3DFORMAT)get_option_val( BackBufferFormat );

    g_var.MSType                    = (D3DMULTISAMPLE_TYPE)get_option_val( MSType );
    g_var.MSFormat                  = (D3DMULTISAMPLE_TYPE)get_option_val( MSFormat );

    g_var.specularenable            = !!get_option_val( specularenable );
    g_var.zenable                   = !!get_option_val( zenable );
    g_var.zwriteenable              = !!get_option_val( zwriteenable );
    g_var.d3dcmpfunc                = (D3DCMPFUNC)get_option_val( d3dcmpfunc );

    g_var.primez                    = !!get_option_val( primez );
    g_var.primezcmpfunc             =  (D3DCMPFUNC)get_option_val( primezcmpfunc );

    g_var.alphablendenable          = !!get_option_val( alphablendenable );
    g_var.alphatestenable           = !!get_option_val( alphatestenable );

    g_var.rotatetex                 = !!get_option_val( rotatetex );
    g_var.tiltz                     = !!get_option_val( tiltz );

    g_var.testtime                  = get_option_valf( testtime );
    g_var.numberscenes              = get_option_val( numberscenes );
    g_var.timeflags                 = get_option_val( timeflags );

    g_var.SwathWidth                = (D3DSWATHWIDTH)get_option_val( SwathWidth );

    g_var.colorwriteenable          = get_option_val( colorwriteenable );

    g_var.quadwidth                 = get_option_valf( quadwidth );
    g_var.quadheight                = get_option_valf( quadheight );

    g_var.quadx                     = ( g_var.screenwidth - min( g_var.screenwidth, g_var.quadwidth ) ) / 2;
    g_var.quady                     = ( g_var.screenheight - min( g_var.screenheight, g_var.quadheight ) ) / 2;
    g_var.quadx                     = get_option_valf( quadx );
    g_var.quady                     = get_option_valf( quady );

    g_var.FVF                       = get_option_val( FVF );

    g_var.clearflags                = get_option_val( clearflags );
    g_var.zclearval                 = get_option_valf( zclearval );
    g_var.sclearval                 = get_option_val( sclearval );
    g_var.cclearval                 = get_option_val( cclearval );

    //First check if we have a pixel shader definition file.
    static const char *szDefPixelShader = "None";

    g_inifile.GetIniStrBuf( "Options", "userpixelshaderfile", szDefPixelShader, 
                            g_var.pixelShaderFileName, 1023 );

    if( memcmp( szDefPixelShader, g_var.pixelShaderFileName, 4 ) == 0)
        g_var.userpixelshader = 0;
    else
        g_var.userpixelshader = 1;

    if( g_var.userpixelshader )
        ReadPixelShaderFile( &g_var.shaderdef );
    else
    {
        //If not, we should look for the INI version of the definition for completeness.
        g_var.userpixelshader = !!get_option_val( userpixelshader );
        if (g_var.userpixelshader)
        {
            ReadPixelShaderIni( &g_var.shaderdef );
        }
    }

    for( int itex = 0; itex < 4; itex++ )
    {
        char szSect[128];
        _snprintf( szSect, sizeof(szSect), "texture%d", itex );
		szSect[127] = '\0';

        g_var.rgtex[itex].type = get_option_val3( szSect, "type", TEX_None );
        g_var.rgtex[itex].texsize = get_option_val3( szSect, "texsize", 8 );
        g_var.rgtex[itex].d3dFormat = (D3DFORMAT)get_option_val3( szSect, "d3dFormat", D3DFMT_DXT1 );
        g_var.rgtex[itex].filtertype = (D3DTEXTUREFILTERTYPE)get_option_val3( szSect, "filtertype", D3DTEXF_LINEAR );
        g_var.rgtex[itex].mipfilter = (D3DTEXTUREFILTERTYPE)get_option_val3( szSect, "mipfilter", D3DTEXF_NONE );
    }

    return true;
}

#if _DEBUG
    extern "C" { extern DWORD D3D__DeadlockTimeOutVal; }
#endif




//-----------------------------------------------------------------------------
// Name: main
//
// Desc: Main Entry point for fillrate app
//-----------------------------------------------------------------------------
void __cdecl main()
{
#if _DEBUG
    // Bump the dbg deadlock timeout value to 20 seconds otherwise some
    // of the tests will complain about d3d being deadlocked.
    D3D__DeadlockTimeOutVal = 20000;
#endif

    XInitDevices( 0, NULL );
    XBInput_CreateGamepads();

    // init vars
    g_var.screenwidth = 640;
    g_var.screenheight = 480;

    g_var.quadwidth = 640;
    g_var.quadheight = 480;

    g_var.quadx = ( g_var.screenwidth - 
                    min(g_var.screenwidth, g_var.quadwidth ) ) / 2;
    g_var.quady = ( g_var.screenheight - 
                    min(g_var.screenheight, g_var.quadheight ) ) / 2;

    g_var.FVF = D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1;

    g_var.alphablendenable = false;
    g_var.alphatestenable = false;

    // default to drawing 10 seconds
    g_var.numberscenes = 0;
    g_var.overdraw = 100;
    g_var.testtime = 10000.0f;
    g_var.timeflags = TIME_Render;

    g_var.SwathWidth = D3DSWATH_128;

    g_var.colorwriteenable = D3DCOLORWRITEENABLE_ALL;

    g_var.ColorOp = D3DTOP_SELECTARG1;
    g_var.AlphaOp = D3DTOP_SELECTARG1;
    g_var.d3dfillmode = D3DFILL_SOLID;

    g_var.zenable = false;
    g_var.zwriteenable = false;

    g_var.d3dcmpfunc = D3DCMP_NOTEQUAL;

    g_var.primez = false;
    g_var.primezcmpfunc = D3DCMP_ALWAYS;

    g_var.BackBufferFormat = D3DFMT_A8R8G8B8;
    g_var.AutoDepthStencilFormat = D3DFMT_D24S8;

    g_var.clearflags = D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL;
    g_var.cclearval = 0x00006688;

    DWORD dwLastTickCount = GetTickCount();
    for( ;; )
    {
        DWORD dwTickCount = GetTickCount();
        DWORD dwTicks = dwTickCount - dwLastTickCount;

        dwLastTickCount = dwTickCount;

        HandleInput( dwTicks );

        if( g_pDev && !g_noINIFile)
        {
            if( g_fRedrawScreen )
            {
                // clear back buffer
                RenderFrame( 1 );

                // swap that guy to the front
                CheckHR( g_pDev->Present( NULL, NULL, NULL, NULL ) );

                // clear new back buffer
                RenderFrame( 1 );

                // display current options on front buffer
                g_scrtext.cls();
                DumpCurrentSettings();
                g_scrtext.drawtext( g_var.verboselevel >= 1 );

                g_fRedrawScreen = false;
            }
            else
            {
                // 33.3 fps baby!
                Sleep( 30 );
            }
        }
        else
        {
            //No INI file.
            // Display the help text telling them to copy one over!!
            if( g_fRedrawScreen )
            {
                // clear back buffer
                RenderFrame( 0 );

                // swap that guy to the front
                CheckHR( g_pDev->Present( NULL, NULL, NULL, NULL ) );

                // clear new back buffer
                RenderFrame( 0 );

                // display current options on front buffer
                g_scrtext.drawtext( false );

                g_fRedrawScreen = false;
            }
        }
    }
}




//-----------------------------------------------------------------------------
// Name: ReadPixelShaderFile
//
// Desc: Initialize and create our pixel shader from a file
//-----------------------------------------------------------------------------
void ReadPixelShaderFile( D3DPIXELSHADERDEF *pshaderdef )
{
    HANDLE hFile = INVALID_HANDLE_VALUE;
    DWORD BytesRead;

    hFile = CreateFile( g_var.pixelShaderFileName,
                        GENERIC_READ, 
                        0, 
                        NULL, 
                        OPEN_EXISTING, 
                        0, 
                        NULL );
    
    if( INVALID_HANDLE_VALUE == hFile )
    {
        dprintf( "Unable to open file %s:  error %x\n", 
                 g_var.pixelShaderFileName, GetLastError() );
        g_var.userpixelshader = 0;
    }
    
    D3DPIXELSHADERDEF_FILE psdf;
    if(!ReadFile(hFile, &psdf, sizeof(D3DPIXELSHADERDEF_FILE), &BytesRead, NULL) ||
        BytesRead != sizeof(D3DPIXELSHADERDEF_FILE) )
	{
		CloseHandle(hFile);
		return;
	}

	   

    CloseHandle(hFile);

    memcpy( pshaderdef, &(psdf.Psd), sizeof(psdf.Psd) );
}




//-----------------------------------------------------------------------------
// Name: ReadPixelShaderIni
//
// Desc: Initialize and create our pixel shader 
//-----------------------------------------------------------------------------
void ReadPixelShaderIni( D3DPIXELSHADERDEF *pshaderdef )
{
    enum
    {
        SE_PSTextureModes,
        SE_PSDotMapping,
        SE_PSInputTexture,
        SE_PSCompareMode,
        SE_PSCombinerCount,
        SE_PSFinalCombinerConstant0,
        SE_PSFinalCombinerConstant1,
        SE_PSFinalCombinerInputsABCD,
        SE_PSFinalCombinerInputsEFG,
        SE_Max
    };

#define SE_ENTRY(_x) SE_##_x, #_x
    struct ShaderEntry
    {
        DWORD dwSectionID;
        LPCSTR szSectionName;
        LPCSTR rgkeys[4];
        int rgvals[4];
    } rgEntries[] =
    {
        { SE_ENTRY(PSTextureModes),             { "t0", "t1", "t2", "t3" } },
        { SE_ENTRY(PSDotMapping),               { "t0", "t1", "t2", "t3" } },
        { SE_ENTRY(PSInputTexture),             { "t2", "t3" } },
        { SE_ENTRY(PSCompareMode),              { "t0", "t1", "t2", "t3" } },
        { SE_ENTRY(PSCombinerCount),            { "count", "flags" } },
        { SE_ENTRY(PSFinalCombinerConstant0),   { "c" } },
        { SE_ENTRY(PSFinalCombinerConstant1),   { "c" } },
        { SE_ENTRY(PSFinalCombinerInputsABCD),  { "A", "B", "C", "D" } },
        { SE_ENTRY(PSFinalCombinerInputsEFG),   { "E", "F", "G", "flags" } },
    };

    enum
    {
        SSE_PSConstant0,
        SSE_PSConstant1,
        SSE_PSRGBInputs,
        SSE_PSAlphaInputs,
        SSE_PSRGBOutputs,
        SSE_PSAlphaOutputs,
        SSE_Max
    };

#define SSE_ENTRY(_x) SSE_##_x, #_x
    struct ShaderStageEntry
    {
        DWORD dwSectionID;
        LPCSTR szSectionName;
        LPCSTR rgkeys[4];
        int rgvals[4][8];
    } rgStageEntries[] =
    {
        { SSE_ENTRY(PSConstant0),       { "c" } },
        { SSE_ENTRY(PSConstant1),       { "c" } },
        { SSE_ENTRY(PSRGBInputs),       { "A", "B", "C", "D" } },
        { SSE_ENTRY(PSAlphaInputs),     { "A", "B", "C", "D" } },
        { SSE_ENTRY(PSRGBOutputs),      { "ab", "cd", "mux_sum", "flags" } },
        { SSE_ENTRY(PSAlphaOutputs),    { "ab", "cd", "mux_sum", "flags" } },
    };

    // read all the shader entries
    for(int iSE = 0; iSE < SE_Max; iSE++)
    {
        ShaderEntry *pse = &rgEntries[iSE];

        for(int ikey = 0; ikey < 4; ikey++)
        {
            if(pse->rgkeys[ikey])
            {
                int val = g_inifile.GetIniInt(pse->szSectionName, pse->rgkeys[ikey], -1);
                if(val != -1)
                {
                    pse->rgvals[ikey] = val;
                    if(g_var.verboselevel >= 2)
                    {
                        dprintf("%s.%s = 0x%lx\n",
                            pse->szSectionName,
                            pse->rgkeys[ikey],
                            val);
                    }
                }
            }
        }
    }

    // read all the shader stage entries
    for(int iSSE = 0; iSSE < SSE_Max; iSSE++)
    {
        char szKey[128];
        ShaderStageEntry *psse = &rgStageEntries[iSSE];

        for(int ikey = 0; ikey < 4; ikey++)
        {
            if(psse->rgkeys[ikey])
            {
                for(int istage = 0; istage < 8; istage++)
                {
                    _snprintf(szKey, sizeof(szKey), "%s%d", psse->rgkeys[ikey], istage);
					szKey[127] = '\0';
                    int val = g_inifile.GetIniInt(psse->szSectionName, szKey, -1);
                    if(val != -1)
                    {
                        psse->rgvals[ikey][istage] = val;
                        if(g_var.verboselevel >= 2)
                        {
                            dprintf("%s.%s[%d] = 0x%lx\n",
                                psse->szSectionName,
                                psse->rgkeys[ikey],
                                istage,
                                val);
                        }
                    }
                }
            }
        }
    }

    ZeroMemory(pshaderdef, sizeof(*pshaderdef));

    /*---------------------------------------------------------------------------*/
    /*  Texture configuration - The following members of the D3DPixelShaderDef   */
    /*  structure define the addressing modes of each of the four texture stages */
    /*---------------------------------------------------------------------------*/
    pshaderdef->PSTextureModes = PS_TEXTUREMODES(
        rgEntries[SE_PSTextureModes].rgvals[0],
        rgEntries[SE_PSTextureModes].rgvals[1],
        rgEntries[SE_PSTextureModes].rgvals[2],
        rgEntries[SE_PSTextureModes].rgvals[3]);

    pshaderdef->PSDotMapping = PS_DOTMAPPING(
        rgEntries[SE_PSDotMapping].rgvals[0],
        rgEntries[SE_PSDotMapping].rgvals[1],
        rgEntries[SE_PSDotMapping].rgvals[2],
        rgEntries[SE_PSDotMapping].rgvals[3]);

    pshaderdef->PSInputTexture = PS_INPUTTEXTURE(
        0,
        0,
        rgEntries[SE_PSInputTexture].rgvals[0],  // 0 and 1 valid
        rgEntries[SE_PSInputTexture].rgvals[1]); // 0, 1, and 2 valid

    pshaderdef->PSCompareMode = PS_COMPAREMODE(
        rgEntries[SE_PSCompareMode].rgvals[0],
        rgEntries[SE_PSCompareMode].rgvals[1],
        rgEntries[SE_PSCompareMode].rgvals[2],
        rgEntries[SE_PSCompareMode].rgvals[3]);

    /*---------------------------------------------------------------------------------*/
    /*  Color combiners - The following members of the D3DPixelShaderDef structure     */
    /*  define the state for the eight stages of color combiners                       */
    /*---------------------------------------------------------------------------------*/
    pshaderdef->PSCombinerCount = PS_COMBINERCOUNT(
        rgEntries[SE_PSCombinerCount].rgvals[0],
        rgEntries[SE_PSCombinerCount].rgvals[1]);

    for(int istage = 0; istage < 8; istage++)
    {
        pshaderdef->PSConstant0[istage] = rgStageEntries[SSE_PSConstant0].rgvals[0][istage];
        pshaderdef->PSConstant1[istage] = rgStageEntries[SSE_PSConstant1].rgvals[0][istage];

        // R0_ALPHA is initialized to T0_ALPHA in stage0

        // a,b,c,d each contain a value from PS_REGISTER, PS_CHANNEL, and PS_INPUTMAPPING
        pshaderdef->PSRGBInputs[istage] = PS_COMBINERINPUTS(
            rgStageEntries[SSE_PSRGBInputs].rgvals[0][istage],      // A
            rgStageEntries[SSE_PSRGBInputs].rgvals[1][istage],      // B
            rgStageEntries[SSE_PSRGBInputs].rgvals[2][istage],      // C
            rgStageEntries[SSE_PSRGBInputs].rgvals[3][istage]);     // D

        // R0_ALPHA is initialized to T0_ALPHA in stage0
        pshaderdef->PSAlphaInputs[istage] = PS_COMBINERINPUTS(
            rgStageEntries[SSE_PSAlphaInputs].rgvals[0][istage],    // A
            rgStageEntries[SSE_PSAlphaInputs].rgvals[1][istage],    // B
            rgStageEntries[SSE_PSAlphaInputs].rgvals[2][istage],    // C
            rgStageEntries[SSE_PSAlphaInputs].rgvals[3][istage]);   // D

        // mux_sum must be DISCARD if either AB_DOT_PRODUCT or CD_DOT_PRODUCT are set
        //  ie: Dot / Dot / Discard || Dot / Mult / Discard || Mult / Dot / Discard

        // ab,cd,mux_sum contain a value from PS_REGISTER
        // flags contains values from PS_COMBINEROUTPUT
        pshaderdef->PSRGBOutputs[istage] = PS_COMBINEROUTPUTS(
            rgStageEntries[SSE_PSRGBOutputs].rgvals[0][istage],     // ab
            rgStageEntries[SSE_PSRGBOutputs].rgvals[1][istage],     // cd
            rgStageEntries[SSE_PSRGBOutputs].rgvals[2][istage],     // mux_sum
            rgStageEntries[SSE_PSRGBOutputs].rgvals[3][istage]);    // flags
        pshaderdef->PSAlphaOutputs[istage] = PS_COMBINEROUTPUTS(
            rgStageEntries[SSE_PSAlphaOutputs].rgvals[0][istage],   // ab
            rgStageEntries[SSE_PSAlphaOutputs].rgvals[1][istage],   // cd
            rgStageEntries[SSE_PSAlphaOutputs].rgvals[2][istage],   // mux_sum
            rgStageEntries[SSE_PSAlphaOutputs].rgvals[3][istage]);  // flags
    }

    pshaderdef->PSFinalCombinerConstant0 = rgEntries[SE_PSFinalCombinerConstant0].rgvals[0];
    pshaderdef->PSFinalCombinerConstant1 = rgEntries[SE_PSFinalCombinerConstant1].rgvals[0];

    // FOG ALPHA is only available in final combiner
    // V1R0_SUM and EF_PROD are only available in final combiner A,B,C,D inputs
    // V1R0_SUM_ALPHA and EF_PROD_ALPHA are not available

    // AB + (1-A)C + D
    pshaderdef->PSFinalCombinerInputsABCD = PS_COMBINERINPUTS(
        rgEntries[SE_PSFinalCombinerInputsABCD].rgvals[0],          // A
        rgEntries[SE_PSFinalCombinerInputsABCD].rgvals[1],          // B
        rgEntries[SE_PSFinalCombinerInputsABCD].rgvals[2],          // C
        rgEntries[SE_PSFinalCombinerInputsABCD].rgvals[3]);         // D

    // E,F,G contain a value from PS_REGISTER, PS_CHANNEL, and PS_INPUTMAPPING
    // flags contains values from PS_FINALCOMBINERSETTING:
    //    PS_FINALCOMBINERSETTING_CLAMP_SUM         // V1+R0 sum clamped to [0,1]
    //    PS_FINALCOMBINERSETTING_COMPLEMENT_V1     // unsigned invert mapping
    //    PS_FINALCOMBINERSETTING_COMPLEMENT_R0     // unsigned invert mapping
    pshaderdef->PSFinalCombinerInputsEFG = PS_COMBINERINPUTS(
        rgEntries[SE_PSFinalCombinerInputsEFG].rgvals[0],           // E
        rgEntries[SE_PSFinalCombinerInputsEFG].rgvals[1],           // F
        rgEntries[SE_PSFinalCombinerInputsEFG].rgvals[2],           // G (alpha)
        rgEntries[SE_PSFinalCombinerInputsEFG].rgvals[3]);          // flags
}




//-----------------------------------------------------------------------------
// Name: GetIniConstf
//
// Desc: Callback from inifile.cpp to get float string value
//-----------------------------------------------------------------------------
bool GetIniConstf(const char *szStr, int cchStr, float *pval)
{
    return false;
}




//-----------------------------------------------------------------------------
// Name: GetIniConst
//
// Desc: Callback from inifile.cpp to get int string value
//-----------------------------------------------------------------------------
bool GetIniConst( const char *szStr, int cchStr, int *pval )
{
    #undef XTAG
    #define XTAG( _tag ) { _tag, #_tag }
    static const struct
    {
        int Val;
        const char *szStr;
    } rgszConsts[] =
    {
        // Pixel Shaders
        XTAG( PS_TEXTUREMODES_NONE ), XTAG( PS_TEXTUREMODES_PROJECT2D ), 
        XTAG( PS_TEXTUREMODES_PROJECT3D ), XTAG( PS_TEXTUREMODES_CUBEMAP ), 
        XTAG( PS_TEXTUREMODES_PASSTHRU ), XTAG( PS_TEXTUREMODES_CLIPPLANE ),
        XTAG( PS_TEXTUREMODES_BUMPENVMAP ), XTAG( PS_TEXTUREMODES_BUMPENVMAP_LUM ), 
        XTAG( PS_TEXTUREMODES_BRDF ), XTAG( PS_TEXTUREMODES_DOT_ST ), 
        XTAG( PS_TEXTUREMODES_DOT_ZW ), XTAG( PS_TEXTUREMODES_DOT_RFLCT_DIFF ),
        XTAG( PS_TEXTUREMODES_DOT_RFLCT_SPEC ), XTAG( PS_TEXTUREMODES_DOT_STR_3D ), 
        XTAG( PS_TEXTUREMODES_DOT_STR_CUBE ), XTAG( PS_TEXTUREMODES_DPNDNT_AR ), 
        XTAG( PS_TEXTUREMODES_DPNDNT_GB ), XTAG( PS_TEXTUREMODES_DOTPRODUCT ),
        XTAG( PS_TEXTUREMODES_DOT_RFLCT_SPEC_CONST ), XTAG( PS_COMPAREMODE_S_LT ), 
        XTAG( PS_COMPAREMODE_S_GE ), XTAG( PS_COMPAREMODE_T_LT ), 
        XTAG( PS_COMPAREMODE_T_GE ), XTAG( PS_COMPAREMODE_R_LT ),
        XTAG( PS_COMPAREMODE_R_GE ), XTAG( PS_COMPAREMODE_Q_LT ), 
        XTAG( PS_COMPAREMODE_Q_GE ), XTAG( PS_COMBINERCOUNT_MUX_LSB ), 
        XTAG( PS_COMBINERCOUNT_MUX_MSB ), XTAG( PS_COMBINERCOUNT_SAME_C0 ),
        XTAG( PS_COMBINERCOUNT_UNIQUE_C0 ), XTAG( PS_COMBINERCOUNT_SAME_C1 ), 
        XTAG( PS_COMBINERCOUNT_UNIQUE_C1 ), XTAG( PS_INPUTMAPPING_UNSIGNED_IDENTITY ), 
        XTAG( PS_INPUTMAPPING_UNSIGNED_INVERT ), XTAG( PS_INPUTMAPPING_EXPAND_NORMAL ),
        XTAG( PS_INPUTMAPPING_EXPAND_NEGATE ), XTAG( PS_INPUTMAPPING_HALFBIAS_NORMAL ), 
        XTAG( PS_INPUTMAPPING_HALFBIAS_NEGATE ), XTAG( PS_INPUTMAPPING_SIGNED_IDENTITY ), 
        XTAG( PS_INPUTMAPPING_SIGNED_NEGATE ), XTAG( PS_REGISTER_ZERO ),
        XTAG( PS_REGISTER_DISCARD ), XTAG( PS_REGISTER_C0 ), XTAG( PS_REGISTER_C1 ),
        XTAG( PS_REGISTER_FOG ), XTAG( PS_REGISTER_V0 ), XTAG( PS_REGISTER_V1 ),
        XTAG( PS_REGISTER_T0 ), XTAG( PS_REGISTER_T1 ), XTAG( PS_REGISTER_T2 ),
        XTAG( PS_REGISTER_T3 ), XTAG( PS_REGISTER_R0 ), XTAG( PS_REGISTER_R1 ),
        XTAG( PS_REGISTER_V1R0_SUM ), XTAG( PS_REGISTER_EF_PROD ), 
        XTAG( PS_REGISTER_ONE ), XTAG( PS_REGISTER_NEGATIVE_ONE ), 
        XTAG( PS_REGISTER_ONE_HALF ), XTAG( PS_REGISTER_NEGATIVE_ONE_HALF ),
        XTAG( PS_CHANNEL_RGB ), XTAG( PS_CHANNEL_BLUE ), XTAG( PS_CHANNEL_ALPHA ),
        XTAG( PS_FINALCOMBINERSETTING_CLAMP_SUM ), 
        XTAG( PS_FINALCOMBINERSETTING_COMPLEMENT_V1 ), 
        XTAG( PS_FINALCOMBINERSETTING_COMPLEMENT_R0 ),
        XTAG( PS_COMBINEROUTPUT_IDENTITY ), XTAG( PS_COMBINEROUTPUT_BIAS ), 
        XTAG( PS_COMBINEROUTPUT_SHIFTLEFT_1 ), XTAG( PS_COMBINEROUTPUT_SHIFTLEFT_1_BIAS ), 
        XTAG( PS_COMBINEROUTPUT_SHIFTLEFT_2 ), XTAG( PS_COMBINEROUTPUT_SHIFTRIGHT_1 ),
        XTAG( PS_COMBINEROUTPUT_AB_BLUE_TO_ALPHA ), 
        XTAG( PS_COMBINEROUTPUT_CD_BLUE_TO_ALPHA ), XTAG( PS_COMBINEROUTPUT_AB_MULTIPLY ),
        XTAG( PS_COMBINEROUTPUT_AB_DOT_PRODUCT ), XTAG( PS_COMBINEROUTPUT_CD_MULTIPLY ), 
        XTAG( PS_COMBINEROUTPUT_CD_DOT_PRODUCT ), XTAG( PS_COMBINEROUTPUT_AB_CD_SUM ), 
        XTAG( PS_COMBINEROUTPUT_AB_CD_MUX ), XTAG( PS_DOTMAPPING_ZERO_TO_ONE ), 
        XTAG( PS_DOTMAPPING_MINUS1_TO_1_D3D ), XTAG( PS_DOTMAPPING_MINUS1_TO_1_GL ),
        XTAG( PS_DOTMAPPING_MINUS1_TO_1 ), XTAG( PS_DOTMAPPING_HILO_1 ), 
        XTAG( PS_DOTMAPPING_HILO_HEMISPHERE_D3D ), 
        XTAG( PS_DOTMAPPING_HILO_HEMISPHERE_GL ),
        XTAG( PS_DOTMAPPING_HILO_HEMISPHERE ),

        // D3DFORMATs
        XTAG( D3DFMT_A8R8G8B8 ), XTAG( D3DFMT_X8R8G8B8 ), XTAG( D3DFMT_R5G6B5 ), 
        XTAG( D3DFMT_R6G5B5 ), XTAG( D3DFMT_X1R5G5B5 ), XTAG( D3DFMT_A1R5G5B5 ), 
        XTAG( D3DFMT_A4R4G4B4 ), XTAG( D3DFMT_A8 ), XTAG( D3DFMT_A8B8G8R8 ), 
        XTAG( D3DFMT_B8G8R8A8 ), XTAG( D3DFMT_R4G4B4A4 ), XTAG( D3DFMT_R5G5B5A1 ),
        XTAG( D3DFMT_R8G8B8A8 ), XTAG( D3DFMT_R8B8 ), XTAG( D3DFMT_G8B8 ), 
        XTAG( D3DFMT_P8 ), XTAG( D3DFMT_L8 ), XTAG( D3DFMT_A8L8 ), XTAG( D3DFMT_AL8 ), 
        XTAG( D3DFMT_L16 ), XTAG( D3DFMT_V8U8 ), XTAG( D3DFMT_L6V5U5 ), 
        XTAG( D3DFMT_X8L8V8U8 ), XTAG( D3DFMT_Q8W8V8U8 ), XTAG( D3DFMT_V16U16 ), 
        XTAG( D3DFMT_D16_LOCKABLE ), XTAG( D3DFMT_D16 ), XTAG( D3DFMT_D24S8 ),
        XTAG( D3DFMT_F16 ), XTAG( D3DFMT_F24S8 ), XTAG( D3DFMT_UYVY ), 
        XTAG( D3DFMT_YUY2 ), XTAG( D3DFMT_DXT1 ), XTAG( D3DFMT_DXT2 ), 
        XTAG( D3DFMT_DXT3 ), XTAG( D3DFMT_DXT4 ), XTAG( D3DFMT_DXT5 ), 
        XTAG( D3DFMT_LIN_A1R5G5B5 ), XTAG( D3DFMT_LIN_A4R4G4B4 ), XTAG( D3DFMT_LIN_A8 ),
        XTAG( D3DFMT_LIN_A8B8G8R8 ), XTAG( D3DFMT_LIN_A8R8G8B8 ), 
        XTAG( D3DFMT_LIN_B8G8R8A8 ), XTAG( D3DFMT_LIN_G8B8 ), XTAG( D3DFMT_LIN_R4G4B4A4 ), 
        XTAG( D3DFMT_LIN_R5G5B5A1 ), XTAG( D3DFMT_LIN_R5G6B5 ), XTAG( D3DFMT_LIN_R6G5B5 ),
        XTAG( D3DFMT_LIN_R8B8 ), XTAG( D3DFMT_LIN_R8G8B8A8 ), XTAG( D3DFMT_LIN_X1R5G5B5 ), 
        XTAG( D3DFMT_LIN_X8R8G8B8 ), XTAG( D3DFMT_LIN_A8L8 ), XTAG( D3DFMT_LIN_AL8 ), 
        XTAG( D3DFMT_LIN_L16 ), XTAG( D3DFMT_LIN_L8 ), XTAG( D3DFMT_LIN_D24S8 ), 
        XTAG( D3DFMT_LIN_F24S8 ), XTAG( D3DFMT_LIN_D16 ), XTAG( D3DFMT_LIN_F16 ),

        XTAG( D3DMULTISAMPLE_NONE ), XTAG( D3DMULTISAMPLE_2_SAMPLES_MULTISAMPLE_LINEAR ),
        XTAG( D3DMULTISAMPLE_2_SAMPLES_MULTISAMPLE_QUINCUNX ), 
        XTAG( D3DMULTISAMPLE_2_SAMPLES_SUPERSAMPLE_HORIZONTAL_LINEAR ),
        XTAG( D3DMULTISAMPLE_2_SAMPLES_SUPERSAMPLE_VERTICAL_LINEAR ), 
        XTAG( D3DMULTISAMPLE_4_SAMPLES_MULTISAMPLE_LINEAR ),
        XTAG( D3DMULTISAMPLE_4_SAMPLES_MULTISAMPLE_GAUSSIAN ),
        XTAG( D3DMULTISAMPLE_4_SAMPLES_SUPERSAMPLE_LINEAR ),
        XTAG( D3DMULTISAMPLE_4_SAMPLES_SUPERSAMPLE_GAUSSIAN ), 
        XTAG( D3DMULTISAMPLE_9_SAMPLES_MULTISAMPLE_GAUSSIAN ),
        XTAG( D3DMULTISAMPLE_9_SAMPLES_SUPERSAMPLE_GAUSSIAN ), 
        XTAG( D3DMULTISAMPLE_PREFILTER_FORMAT_DEFAULT ),
        XTAG( D3DMULTISAMPLE_PREFILTER_FORMAT_X1R5G5B5 ), 
        XTAG( D3DMULTISAMPLE_PREFILTER_FORMAT_R5G6B5 ),
        XTAG( D3DMULTISAMPLE_PREFILTER_FORMAT_X8R8G8B8 ), 
        XTAG( D3DMULTISAMPLE_PREFILTER_FORMAT_A8R8G8B8 ),

        XTAG( D3DMS_NONE ), XTAG( D3DMS_2_SAMPLES_MULTISAMPLE_LINEAR ),
        XTAG( D3DMS_2_SAMPLES_MULTISAMPLE_QUINCUNX ), 
        XTAG( D3DMS_2_SAMPLES_SUPERSAMPLE_HORIZONTAL_LINEAR ),
        XTAG( D3DMS_2_SAMPLES_SUPERSAMPLE_VERTICAL_LINEAR ), 
        XTAG( D3DMS_4_SAMPLES_MULTISAMPLE_LINEAR ),
        XTAG( D3DMS_4_SAMPLES_MULTISAMPLE_GAUSSIAN ), 
        XTAG( D3DMS_4_SAMPLES_SUPERSAMPLE_LINEAR ),
        XTAG( D3DMS_4_SAMPLES_SUPERSAMPLE_GAUSSIAN ), 
        XTAG( D3DMS_9_SAMPLES_MULTISAMPLE_GAUSSIAN ),
        XTAG( D3DMS_9_SAMPLES_SUPERSAMPLE_GAUSSIAN ), 
        XTAG( D3DMS_PREFILTER_FORMAT_DEFAULT ),
        XTAG( D3DMS_PREFILTER_FORMAT_X1R5G5B5 ), 
        XTAG( D3DMS_PREFILTER_FORMAT_R5G6B5 ),
        XTAG( D3DMS_PREFILTER_FORMAT_X8R8G8B8 ), 
        XTAG( D3DMS_PREFILTER_FORMAT_A8R8G8B8 ),

        XTAG( D3DCLEAR_TARGET ), XTAG( D3DCLEAR_ZBUFFER ), XTAG( D3DCLEAR_STENCIL ),

        // FVFs
        XTAG( D3DFVF_XYZ ), XTAG( D3DFVF_XYZRHW ), XTAG( D3DFVF_NORMAL ), 
        XTAG( D3DFVF_DIFFUSE ), XTAG( D3DFVF_SPECULAR ), XTAG( D3DFVF_TEX0 ), 
        XTAG( D3DFVF_TEX1 ), XTAG( D3DFVF_TEX2 ),
        XTAG( D3DFVF_TEX3 ), XTAG( D3DFVF_TEX4 ),

        // textcoord sizes
        XTAG( T0_SIZE1 ), XTAG( T0_SIZE2 ), XTAG( T0_SIZE3 ), XTAG( T0_SIZE4 ),
        XTAG( T1_SIZE1 ), XTAG( T1_SIZE2 ), XTAG( T1_SIZE3 ), XTAG( T1_SIZE4 ),
        XTAG( T2_SIZE1 ), XTAG( T2_SIZE2 ), XTAG( T2_SIZE3 ), XTAG( T2_SIZE4 ),
        XTAG( T3_SIZE1 ), XTAG( T3_SIZE2 ), XTAG( T3_SIZE3 ), XTAG( T3_SIZE4 ),

        // D3DCMPs
        XTAG( D3DCMP_NEVER ), XTAG( D3DCMP_LESS ), XTAG( D3DCMP_EQUAL ), 
        XTAG( D3DCMP_LESSEQUAL ), XTAG( D3DCMP_GREATER ), XTAG( D3DCMP_NOTEQUAL ), 
        XTAG( D3DCMP_GREATEREQUAL ), XTAG( D3DCMP_ALWAYS ),

        XTAG( D3DTEXF_NONE ), XTAG( D3DTEXF_POINT ), XTAG( D3DTEXF_LINEAR ), 
        XTAG( D3DTEXF_ANISOTROPIC ), XTAG( D3DTEXF_QUINCUNX ), 
        XTAG( D3DTEXF_GAUSSIANCUBIC ),

        XTAG( TEX_None ), XTAG( TEX_2d ), XTAG( TEX_Cubemap ), XTAG( TEX_Volume ),

        XTAG( TIME_Present ), XTAG( TIME_Render ),

        // swaths
        XTAG( D3DSWATH_8 ), XTAG( D3DSWATH_16 ), XTAG( D3DSWATH_32 ),
        XTAG( D3DSWATH_64 ), XTAG( D3DSWATH_128 ), XTAG( D3DSWATH_OFF ),

        XTAG( D3DCOLORWRITEENABLE_RED ), XTAG( D3DCOLORWRITEENABLE_GREEN ), 
        XTAG( D3DCOLORWRITEENABLE_BLUE ), XTAG( D3DCOLORWRITEENABLE_ALPHA ), 
        XTAG( D3DCOLORWRITEENABLE_ALL ),

        // misc
        XTAG( FALSE  ), XTAG( TRUE )
    };

    for( int ifmt = 0; ifmt < ARRAYSIZE( rgszConsts ); ifmt++ )
    {
        if( !_strnicmp( rgszConsts[ifmt].szStr, szStr, cchStr ) )
        {
            // set val
            *pval = rgszConsts[ifmt].Val;
            return true;
        }
    }

    return false;
}

