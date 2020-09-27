//-----------------------------------------------------------------------------
// FILE: FILLRATE.CPP
//
// Desc: vertex shader tune app
//
// Copyright (c) 1999-2000 Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#include <xtl.h>
#include <d3d8perf.h>
#include <stdio.h>
#include <assert.h>

// With the profile build we can spew out how much time
// was spent in pushbuffer waits. However you must link
// with d3d8i.lib so it's off by default.
#define PROFILE     0

#include "utils.h"
#include "inifile.h"
#include "vstune.h"

/*
 * globals
 */
IDirect3DDevice8       *g_pDev = NULL;                  // our device
IDirect3DVertexBuffer8 *g_pVB[16] = {NULL};             // stream vertex buffers
IDirect3DTexture8      *g_pTextures[4] = { NULL };

DWORD                  *g_pAlloc = NULL;                // a random WC allocation
const int               g_AllocSize = 1024*1024;        // random allocation size

DWORD                   g_dwVertexShader;               // vertex shader handle

DWORD                   g_dwFVFSize[16];                // FVF sizes for each stream

WORD                   *g_pindices = NULL;              // out indices

HANDLE                  g_hInpDevice = NULL;            // input device

CScrText                g_scrtext;                      // text helper
CIniFile                g_inifile("d:\\vstune.ini");    // Ini file

bool                    g_fRedrawScreen = true;         // refresh screen?
volatile bool           g_gpudrawingdone = false;       // is the gpu done drawing?

VSTSETTINGS             g_var = {0};                    // current settings

DWORD                   g_Declaration[MAX_FVF_DECL_SIZE]; // vertex shader declaration
DWORD                   g_nvclk = 233;

HANDLE                  g_hThread = NULL;

D3DPERF                 *g_pPerf;

struct
{
    LPCSTR szDesc;
    void (*pfnTest)();
} g_rgTests[] =
{
    { "Current",                DoQuickTimedTest },
    { "PushBuffer Call Test",   DoPushBufferCallTest },
};
UINT g_iCurTest = 0;

//=========================================================================
// Init Direct3d
//=========================================================================
bool InitD3D()
{
    static D3DPRESENT_PARAMETERS d3dppCur = {0};
    D3DPRESENT_PARAMETERS d3dpp = {0};

    // Set the screen mode.
    d3dpp.BackBufferWidth                   = UINT(g_var.screenwidth);
    d3dpp.BackBufferHeight                  = UINT(g_var.screenheight);
    d3dpp.BackBufferFormat                  = g_var.BackBufferFormat;
    d3dpp.BackBufferCount                   = 1;
    d3dpp.EnableAutoDepthStencil            = !!g_var.AutoDepthStencilFormat;
    d3dpp.AutoDepthStencilFormat            = g_var.AutoDepthStencilFormat;
    d3dpp.SwapEffect                        = D3DSWAPEFFECT_DISCARD;
    d3dpp.FullScreen_RefreshRateInHz        = 60;
    d3dpp.FullScreen_PresentationInterval   = D3DPRESENT_INTERVAL_IMMEDIATE;
    d3dpp.Flags                             = // D3DCREATE_NOPRESENTTHROTTLE |
                                                D3DPRESENTFLAG_LOCKABLE_BACKBUFFER;

    if(g_var.MSType != D3DMULTISAMPLE_NONE)
        d3dpp.MultiSampleType = (D3DMULTISAMPLE_TYPE)((DWORD)(g_var.MSType) | (DWORD)(g_var.MSFormat));

    // if nothing has changed - don't reinit
    if(!memcmp(&d3dppCur, &d3dpp, sizeof(D3DPRESENT_PARAMETERS)))
        return true;
    d3dppCur = d3dpp;

    if(g_pDev)
    {
        for(int itex = 0; itex < 4; itex++)
            RELEASE(g_pTextures[itex]);

        if(g_dwVertexShader)
        {
            g_pDev->SetVertexShader(D3DFVF_XYZ);
            g_pDev->DeleteVertexShader(g_dwVertexShader);
            g_dwVertexShader = 0;
        }

        // make sure everything is freed
        for(int ivb = 0; ivb < ARRAYSIZE(g_pVB); ivb++)
            RELEASE(g_pVB[ivb]);
        RELEASE(g_pDev);
    }

    // Create the device.
    if(Direct3D_CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL,
        NULL, D3DCREATE_HARDWARE_VERTEXPROCESSING, &d3dpp, &g_pDev))
    {
        CheckHR(E_FAIL);
        return false;
    }

    for(int itex = 0; itex < 4; itex++)
    {
        CheckHR(g_pDev->CreateTexture(4, 4, 1, 0, D3DFMT_A4R4G4B4,
            D3DPOOL_MANAGED, &g_pTextures[itex]));
    }

    g_nvclk = GetNVCLK();
    dprintf("gpu clk: %d\n", g_nvclk);

    return true;
}

//=========================================================================
// quick routine to return token type from decl
//=========================================================================
inline DWORD VSDGetTokenType(DWORD dwDecl)
{
    return (dwDecl & D3DVSD_TOKENTYPEMASK) >> D3DVSD_TOKENTYPESHIFT;
}

//=========================================================================
// return data type from decl
//=========================================================================
inline DWORD VSDGetDataType(DWORD dwDecl)
{
    return (dwDecl & D3DVSD_DATATYPEMASK) >> D3DVSD_DATATYPESHIFT;
}

//=========================================================================
// get skip count from decl
//=========================================================================
inline DWORD VSDGetSkipCount(DWORD dwDecl)
{
    return (dwDecl & D3DVSD_SKIPCOUNTMASK) >> D3DVSD_SKIPCOUNTSHIFT;
}

//=========================================================================
// Given a D3DVSD_TOKEN_STREAMDATA return # of bytes it requires
//=========================================================================
DWORD dwBytesForStreamData(DWORD dwDecl)
{
    if(dwDecl & 0x08000000)
    {
        // D3DVSD_SKIPBYTES
        return VSDGetSkipCount(dwDecl);
    }
    else if(dwDecl & 0x10000000)
    {
        // D3DVSD_SKIP DWORDS
        return VSDGetSkipCount(dwDecl) * sizeof(DWORD);
    }
    else
    {
        DWORD dwType = VSDGetDataType(dwDecl);
        DWORD dwCount = (dwType >> 4) & 0xf;

        switch(dwType & 0xf)
        {
        case 0x0:   // D3DCOLOR
            return dwCount * sizeof(BYTE);
        case 0x2:   // float
            return dwCount * sizeof(float);
        case 0x1:   // NORMSHORT
        case 0x5:   // short
            return dwCount * sizeof(short);
        case 0x6:   // NORMPACKED
            return dwCount * sizeof(DWORD);
        case 0x4:   // PBYTE
            return dwCount * sizeof(BYTE);
        }
    }

    return 0;
}

//=========================================================================
// Return vert size for specific stream in declaration
//=========================================================================
DWORD GetDeclVertexSize(DWORD dwStream, DWORD *pDecl)
{
    DWORD dwBytes = 0;
    DWORD dwCurStream = (DWORD)-1;

    while(*pDecl != D3DVSD_END())
    {
        if(VSDGetTokenType(*pDecl) == D3DVSD_TOKEN_STREAM)
        {
            dwCurStream = *pDecl & D3DVSD_STREAMNUMBERMASK;
        }
        else if(dwCurStream == dwStream &&
            VSDGetTokenType(*pDecl) == D3DVSD_TOKEN_STREAMDATA)
        {
            dwBytes += dwBytesForStreamData(*pDecl);
        }

        pDecl++;
    }

    return dwBytes;
}

//=========================================================================
// Create our index buffer
//=========================================================================
void CreateIB()
{
    if(g_pindices)
    {
        delete [] g_pindices;
        g_pindices = NULL;
    }

    if(g_var.pushbuffernops)
        g_var.TriCount = g_var.indexcount;
    else
    {
        switch(g_var.primitivetype)
        {
        case D3DPT_POINTLIST:
            g_var.TriCount = g_var.indexcount;
            break;
        case D3DPT_TRIANGLELIST:
            g_var.indexcount = (g_var.indexcount / 3) * 3;
            g_var.TriCount = g_var.indexcount / 3;
            break;
        case D3DPT_TRIANGLESTRIP:
        case D3DPT_TRIANGLEFAN:
            g_var.TriCount = g_var.indexcount - 2;
            break;
        case D3DPT_QUADLIST:
            g_var.indexcount = (g_var.indexcount / 4) * 4;
            g_var.TriCount = g_var.indexcount / 2;
            break;
        case D3DPT_QUADSTRIP:
            g_var.indexcount = (g_var.indexcount / 2) * 2;
            g_var.TriCount = (g_var.indexcount / 2 - 1) * 2;
            break;
        case D3DPT_POLYGON:
        case D3DPT_LINELIST:
        case D3DPT_LINELOOP:
        case D3DPT_LINESTRIP:
        default:
            g_var.TriCount = g_var.indexcount;
            _asm int 3;
            break;
        }
    }

    // create our index buffer
    g_pindices = new WORD [g_var.indexcount];

    WORD *pindices = g_pindices;
    for(UINT ivert = 0; ivert < g_var.indexcount; ivert++)
    {
        *pindices++ = LOWORD(ivert % g_var.vertcount);
    }
}

//=========================================================================
// Copy count of floats from src to dst
//=========================================================================
inline float *MyMemCpy(float *dst, const float *src, int count)
{
    memcpy(dst, src, count * sizeof(float));
    return dst + count;
}

//=========================================================================
// Create our vertex buffer for specific stream defined in g_pDeclaration
//=========================================================================
void CreateVB(DWORD istream)
{
    RELEASE(g_pVB[istream]);

    if(!g_var.FVF)
    {
        // calculate stream size and length
        g_dwFVFSize[istream] = GetDeclVertexSize(istream, g_Declaration);

        UINT VBLength = g_dwFVFSize[istream] * g_var.vertcount;

        CheckHR(g_pDev->CreateVertexBuffer(VBLength, 0, 0, 0, &g_pVB[istream]));

        byte *pVerts;
        CheckHR(g_pVB[istream]->Lock(0, VBLength, (BYTE **)&pVerts, 0));

        // Fill it up.
        memset(pVerts, 0x11, VBLength);

        CheckHR(g_pVB[istream]->Unlock());
    }
    else
    {
        g_dwFVFSize[istream] = D3DXGetFVFVertexSize(g_var.FVF);

        UINT VBLength = g_dwFVFSize[istream] * g_var.vertcount;

        CheckHR(g_pDev->CreateVertexBuffer(VBLength, 0, 0, 0, &g_pVB[istream]));

        float *pVerts;
        CheckHR(g_pVB[istream]->Lock(0, VBLength, (BYTE **)&pVerts, 0));

        int dy = 0;
        int iIndex = 0;
        D3DXVECTOR4 tex0(0, 0, 0, 1.0f);
        D3DXVECTOR4 pos(-2000, -2000, .5f, 1.0f);
        D3DXVECTOR3 norm(0, .707f, .707f);
        DWORD diffuse = D3DCOLOR_COLORVALUE(tex0.x, tex0.y, 1.0f - tex0.x, 2.0f / 0xff);

        for(UINT ivert = 0; ivert < g_var.vertcount; ivert++)
        {
            if(!g_var.degenerates)
            {
                static float xval = 4.0f;
                static float yval = 4.0f;

                pos.x = xval * (iIndex / 2);
                pos.y = dy + yval * (iIndex & 0x1);

                // if we're not doing just degenerate tris then move along
                // in a nice little grid pattern
                iIndex++;

                if(pos.x + xval > g_var.screenwidth)
                {
                    iIndex = 0;
                    dy += int(yval) + 1;
                }
            }

            if(g_var.FVF & D3DFVF_XYZRHW)
                pVerts = MyMemCpy(pVerts, pos, 4);
            else if(g_var.FVF & D3DFVF_XYZ)
                pVerts = MyMemCpy(pVerts, pos, 3);

            if(g_var.FVF & D3DFVF_NORMAL)
                pVerts = MyMemCpy(pVerts, norm, 3);

            if(g_var.FVF & D3DFVF_DIFFUSE)
                *(DWORD *)pVerts++ = diffuse;

            if(g_var.FVF & D3DFVF_SPECULAR)
                *(DWORD *)pVerts++ = ~diffuse;

            // Texture coordinates
            UINT uNumTexCoords = (((g_var.FVF) & D3DFVF_TEXCOUNT_MASK) >> D3DFVF_TEXCOUNT_SHIFT);
            UINT uTextureFormats = g_var.FVF >> 16;

            if(uTextureFormats)
            {
                for(UINT i = 0; i < uNumTexCoords; i++)
                {
                    switch(uTextureFormats & 3)
                    {
                    case D3DFVF_TEXTUREFORMAT1:     // u
                        *pVerts++ = tex0.x;
                        break;
                    case D3DFVF_TEXTUREFORMAT2:     // u,v
                        pVerts = MyMemCpy(pVerts, tex0, 2);
                        break;
                    case D3DFVF_TEXTUREFORMAT3:     // s,t,r
                        pVerts = MyMemCpy(pVerts, tex0, 3);
                        break;
                    case D3DFVF_TEXTUREFORMAT4:     // s,t,r,q
                        pVerts = MyMemCpy(pVerts, tex0, 4);
                        break;
                    }

                    uTextureFormats >>= 2;
                }
            }
            else while(uNumTexCoords--)
            {
                // u,v
                pVerts = MyMemCpy(pVerts, tex0, 2);
            }
        }

        CheckHR(g_pVB[istream]->Unlock());
    }
}

//=========================================================================
// initialize our lights
//=========================================================================
void InitLights()
{
    // disable all our existing lights
    for(DWORD i = 0; i < g_var.NumLights; i++)
        CheckHR(g_pDev->LightEnable(g_var.NumLights, FALSE));

    // get count of new lights
    g_var.NumLights =
        g_var.numDirectionalLights +
        g_var.numPointLights +
        g_var.numSpotLights;

    // setup lights
    float fIntensity = 1.0f / g_var.NumLights;
    D3DXCOLOR Diffuse(fIntensity, fIntensity, fIntensity, 1.0f);

    DWORD dwIndex = 0;
    for(i = 0; i < g_var.numDirectionalLights; i++)
    {
        D3DLIGHT8 lig = { D3DLIGHT_DIRECTIONAL };

        lig.Diffuse     =  Diffuse;
        lig.Direction   =  D3DXVECTOR3(0.2f, 0.3f, -0.8f);

        CheckHR(g_pDev->SetLight(dwIndex, &lig));
        CheckHR(g_pDev->LightEnable(dwIndex, TRUE));
        dwIndex++;
    }

    for(i = 0; i < g_var.numPointLights; i++)
    {
        D3DLIGHT8 lig = { D3DLIGHT_POINT };

        lig.Diffuse      =  Diffuse;
        lig.Position     =  D3DXVECTOR3(0.0f, 0.0f, 5.0f);
        lig.Range        = 9999.0f;
        lig.Attenuation0 = 1.0f;

        CheckHR(g_pDev->SetLight(dwIndex, &lig));
        CheckHR(g_pDev->LightEnable(dwIndex, TRUE));
        dwIndex++;
    }

    for(i = 0; i < g_var.numSpotLights; i++)
    {
        D3DLIGHT8 lig = { D3DLIGHT_SPOT };

        lig.Diffuse      = Diffuse;
        lig.Position     = D3DXVECTOR3(-1.0f, -5.0f, 6.0f);
        lig.Direction    = D3DXVECTOR3(0.1f, 0.5f, -0.6f);
        lig.Range        = 999.0f;
        lig.Theta        = 0.1f;
        lig.Phi          = 0.5f;
        lig.Falloff      = 1.0f;
        lig.Attenuation0 = 1.0f;

        CheckHR(g_pDev->SetLight(dwIndex, &lig));
        CheckHR(g_pDev->LightEnable(dwIndex, TRUE));
        dwIndex++;
    }
}


//=========================================================================
// Initialize fillrate app
//=========================================================================
bool InitVST(bool freadINIFile)
{
    if(freadINIFile && !ReadVSTIniFile())
        dprintf("%s not found", g_inifile.GetFileName());

    // init d3d
    if(!InitD3D())
    {
        dprintf("InitD3D failed.\n");
        return false;
    }

    // init our render states
    CheckHR(g_pDev->SetRenderState(D3DRS_LIGHTING,  g_var.lighting));
    CheckHR(g_pDev->SetRenderState(D3DRS_SPECULARENABLE, g_var.specularenable));
    CheckHR(g_pDev->SetRenderState(D3DRS_LOCALVIEWER, g_var.localviewer));
    CheckHR(g_pDev->SetRenderState(D3DRS_NORMALIZENORMALS, g_var.normalizenormals));

    CheckHR(g_pDev->SetRenderState(D3DRS_COLORVERTEX, g_var.colorvertex));

    static DWORD dwMatSource = D3DMCS_COLOR1;
    CheckHR(g_pDev->SetRenderState(D3DRS_DIFFUSEMATERIALSOURCE,  dwMatSource));
    CheckHR(g_pDev->SetRenderState(D3DRS_SPECULARMATERIALSOURCE, dwMatSource));
    CheckHR(g_pDev->SetRenderState(D3DRS_AMBIENTMATERIALSOURCE,  dwMatSource));
    CheckHR(g_pDev->SetRenderState(D3DRS_EMISSIVEMATERIALSOURCE, dwMatSource));

    CheckHR(g_pDev->SetRenderState(D3DRS_SHADEMODE, D3DSHADE_FLAT));

    CheckHR(g_pDev->SetRenderState(D3DRS_FOGENABLE, g_var.fogenable));

    // z buffer
    CheckHR(g_pDev->SetRenderState(D3DRS_ZENABLE, g_var.zenable));
    CheckHR(g_pDev->SetRenderState(D3DRS_ZWRITEENABLE, g_var.zwriteenable));
    CheckHR(g_pDev->SetRenderState(D3DRS_ZFUNC, g_var.d3dcmpfunc));

    // alpha blending
    CheckHR(g_pDev->SetRenderState(D3DRS_ALPHABLENDENABLE, g_var.alphablendenable));
    CheckHR(g_pDev->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA));
    CheckHR(g_pDev->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA));

    CheckHR(g_pDev->SetRenderState(D3DRS_ALPHATESTENABLE, g_var.alphatestenable));
    CheckHR(g_pDev->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_ALWAYS));
    CheckHR(g_pDev->SetRenderState(D3DRS_ALPHAREF, 0x00000001));

    // fill and cull mode
    CheckHR(g_pDev->SetRenderState(D3DRS_FILLMODE, g_var.d3dfillmode));
    CheckHR(g_pDev->SetRenderState(D3DRS_CULLMODE, g_var.cullmode));

    // initialize lights
    InitLights();

    // initialize vertex shader or FVF declaration
    InitVertexShader();

    // set up texture stages
    for(DWORD itex = 0; itex < 4; itex++)
    {
        if(itex < g_var.numtextures)
        {
            CheckHR(g_pDev->SetTexture(itex, g_pTextures[itex]));

            CheckHR(g_pDev->SetTextureStageState(0, D3DTSS_COLORARG1,D3DTA_TEXTURE));
            CheckHR(g_pDev->SetTextureStageState(0, D3DTSS_COLOROP,  D3DTOP_MODULATE));
            CheckHR(g_pDev->SetTextureStageState(0, D3DTSS_COLORARG2,D3DTA_DIFFUSE));

            CheckHR(g_pDev->SetTextureStageState(0, D3DTSS_ALPHAARG1,D3DTA_TEXTURE));
            CheckHR(g_pDev->SetTextureStageState(0, D3DTSS_ALPHAOP,  D3DTOP_MODULATE));
            CheckHR(g_pDev->SetTextureStageState(0, D3DTSS_ALPHAARG2,D3DTA_DIFFUSE));
        }
        else
        {
            CheckHR(g_pDev->SetTexture(itex, NULL));

            CheckHR(g_pDev->SetTextureStageState(itex, D3DTSS_COLOROP, D3DTOP_DISABLE));
            CheckHR(g_pDev->SetTextureStageState(itex, D3DTSS_ALPHAOP, D3DTOP_DISABLE));
        }

        CheckHR(g_pDev->SetTextureStageState(itex, D3DTSS_ADDRESSU, D3DTADDRESS_CLAMP));
        CheckHR(g_pDev->SetTextureStageState(itex, D3DTSS_ADDRESSV, D3DTADDRESS_CLAMP));

        CheckHR(g_pDev->SetTextureStageState(itex, D3DTSS_MINFILTER, D3DTEXF_LINEAR));
        CheckHR(g_pDev->SetTextureStageState(itex, D3DTSS_MAGFILTER, D3DTEXF_LINEAR));
    }

    // create our index buffer
    CreateIB();

    // create all our vertex buffer streams
    for(DWORD istream = 0; istream < g_var.NumStreams; istream++)
        CreateVB(istream);

    // Initialize our VAB entries so we have consistent and unique values
    for(DWORD Register = 0; Register < 16; Register++)
    {
        float val = (float)Register;

        g_pDev->SetVertexData4f(Register,
            val + .1f, val + .2f, val + .3f, val + .4f);
    }

    // if we're not writing stuff in the background then suspend our thread
    if(g_var.wcwritesinbkgnd != 2)
        SuspendThread(g_hThread);
    else
    {
        while(ResumeThread(g_hThread) > 1)
            ;
    }

    return true;
}

//=========================================================================
// clear surface
//=========================================================================
void ClearScreen()
{
    // clear stuff
    DWORD dwCFlags = g_var.clearflags;

    if(!g_var.AutoDepthStencilFormat)
        dwCFlags &= ~(D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL);

    CheckHR(g_pDev->Clear(0, NULL, dwCFlags, g_var.cclearval,
        g_var.zclearval, g_var.sclearval));
}

//=========================================================================
// Initialize the vertex shader and stream source(s)
//=========================================================================
void InitFrame()
{
    CheckHR(g_pDev->SetVertexShader(g_dwVertexShader));

    for(DWORD istream = 0; istream < g_var.NumStreams; istream++)
        CheckHR(g_pDev->SetStreamSource(istream, g_pVB[istream], g_dwFVFSize[istream]));
}

//=========================================================================
// Render the vertices x times
//=========================================================================
void RenderFrame(int vbdrawcount, BOOL nops)
{
    if(nops)
    {
        while(vbdrawcount--)
        {
            for(DWORD index = 0; index < g_var.indexcount; index++)
                D3DDevice_Nop();
        }
    }
    else
    {
        while(vbdrawcount--)
        {
            CheckHR(g_pDev->DrawIndexedVertices(g_var.primitivetype, g_var.indexcount, g_pindices));
        }
    }
}

//=========================================================================
// Handle joystick input
//=========================================================================
void HandleInput(DWORD dwTicks)
{
    static BOOL fRunTestRightNow = FALSE;

    if(g_inifile.FileChanged())
    {
        InitVST(true);

        fRunTestRightNow = g_var.runtestonreload;

        g_fRedrawScreen = true;
    }

    // If we don't have a device don't let them start any tests.
    // They're going to have to change their settings in fillrate.ini
    // and just try again.
    if(!g_pDev)
        return;

    if(!g_hInpDevice)
        g_hInpDevice = XInputOpen(XDEVICE_TYPE_GAMEPAD, XDEVICE_PORT0, XDEVICE_NO_SLOT, NULL);

    if(g_hInpDevice)
    {
        XINPUT_STATE xinpstate = {0};
        static XINPUT_STATE xinpstatelast;
#define FButtonDown(_btn) \
        (xinpstate.Gamepad.bAnalogButtons[_btn] && \
            (!xinpstate.Gamepad.bAnalogButtons[_btn] != !xinpstatelast.Gamepad.bAnalogButtons[_btn]))

        XInputGetState(g_hInpDevice, &xinpstate);

        if(fRunTestRightNow || FButtonDown(XINPUT_GAMEPAD_A))
        {
            fRunTestRightNow = FALSE;

            // run current test
            g_rgTests[g_iCurTest].pfnTest();

            // draw one scene with current settings
            ClearScreen();
            InitFrame();
            RenderFrame(1);

            // flip that dude to the front
            CheckHR(g_pDev->Present(NULL, NULL, NULL, NULL));

            // plaster our results on the front buffer
            g_scrtext.drawtext(g_var.verboselevel >= 1);
            g_fRedrawScreen = false;
        }

        if(FButtonDown(XINPUT_GAMEPAD_LEFT_TRIGGER))
        {
            g_iCurTest = (g_iCurTest + ARRAYSIZE(g_rgTests) - 1) % ARRAYSIZE(g_rgTests);
            g_fRedrawScreen = true;
        }

        if(FButtonDown(XINPUT_GAMEPAD_RIGHT_TRIGGER))
        {
            g_iCurTest = (g_iCurTest + 1) % ARRAYSIZE(g_rgTests);
            g_fRedrawScreen = true;
        }

        if(FButtonDown(XINPUT_GAMEPAD_X))
        {
            InitVST(true);
            g_fRedrawScreen = true;
        }

        if(FButtonDown(XINPUT_GAMEPAD_Y))
        {
            // toggle wireframe and solid fill modes
            g_var.d3dfillmode = (g_var.d3dfillmode == D3DFILL_WIREFRAME) ?
                D3DFILL_SOLID : D3DFILL_WIREFRAME;
            CheckHR(g_pDev->SetRenderState(D3DRS_FILLMODE, g_var.d3dfillmode));
            g_fRedrawScreen = true;
        }

        xinpstatelast = xinpstate;
    }
}

//=========================================================================
//
//=========================================================================
const char *szGetPTType(DWORD pt)
{
    static const char *rgszD3DPT[] =
    {
        "D3DPT_0",
        "D3DPT_POINTLIST",
        "D3DPT_LINELIST",
        "D3DPT_LINELOOP",
        "D3DPT_LINESTRIP",
        "D3DPT_TRIANGLELIST",
        "D3DPT_TRIANGLESTRIP",
        "D3DPT_TRIANGLEFAN",
        "D3DPT_QUADLIST",
        "D3DPT_QUADSTRIP",
        "D3DPT_POLYGON",
        "D3DPT_?",
    };

    return rgszD3DPT[min(pt, ARRAYSIZE(rgszD3DPT))];
}

//=========================================================================
// Do a series of pushbuffer calls w/ varying pushbuffer sizes
//=========================================================================
void DoPushBufferCallTest()
{
    // clear screen text
    g_scrtext.cls();

    g_var.vbdrawcount = 1;
    g_var.framesperpushbuffermax = 1;

    DumpCurrentSettings();

    char szIndices[256];
    static const char *szDefIndices =
        "50, 100, 250, 500, 1000, 1500, 2000, 2500, 5000, 10000, 20000, 40000";

    g_inifile.GetIniStrBuf("pushbuffercalltest", "indices",
        szDefIndices, szIndices, sizeof(szIndices));

    dprintf("** PushBufferCallTest (%s) **\n", szIndices);

    static const char *rgHdrsNops[2] =
        { "Mnops", "cycles/nop" };
    static const char *rgHdrsIndices[2] =
        { "Mtps", "cycles/vert" };
    const char **rgHdrs = g_var.pushbuffernops ? rgHdrsNops : rgHdrsIndices;
    char *sizehdr = g_var.precompiled ? "pushbufsize" : "# indices";

    g_scrtext.printf("%-12s %-7s %-7s %-10s(%d)"
#if PROFILE
        " waits waittime(ms)"
#endif
        , sizehdr,
        rgHdrs[0], rgHdrs[1], "throughput", g_var.wcwritesinbkgnd);

    char *token;
    const char seps[]   = " ,\t";
    token = strtok(szIndices, seps);
    while(token)
    {
        g_var.indexcount = atoi(token);

        if(g_var.verboselevel >= 1)
            dprintf("%d ", g_var.indexcount);

        if(g_var.indexcount)
        {
            InitVST(false);

            TIMETESTRESULTS ttres = {0};
            DoTimedTest(&ttres);

            g_scrtext.printf("%11d %6.2f  %12.2f %10.2f"
#if PROFILE
                " %8d-%.2f"
#endif
                , g_var.precompiled ? ttres.pushbuffersize : g_var.indexcount,
                ttres.mtps, ttres.cycles, ttres.throughput,
                ttres.PushBufferWaitInfo.Count,
                (double)(ttres.PushBufferWaitInfo.Cycles / 733) / 1000.0);
        }

        token = strtok(NULL, seps);
    }

    dprintf("\n\n");
    g_scrtext.printf("");

    // restore our settings
    InitVST(true);
}

//=========================================================================
// Do a quick test with the current settings
//=========================================================================
void DoQuickTimedTest()
{
    static DWORD testnum = 0;
    dprintf("\n*** test #%ld ***\n", ++testnum);

    // clear screen text
    g_scrtext.cls();

    // run test
    TIMETESTRESULTS ttres = {0};
    DoTimedTest(&ttres);

    DumpCurrentSettings();

    g_scrtext.printf("");

    // if we did the wc writes in the background spew the results
    if(g_var.wcwritesinbkgnd)
    {
        g_scrtext.printf("writer thread:  %s",
            (g_var.wcwritesinbkgnd == 1) ? "main" : "low prio bkgnd");
        g_scrtext.printf("bytes written:  %lu", ttres.byteswritten);
        g_scrtext.printf("throughput:     %.2f MB/s", ttres.throughput);
        g_scrtext.printf("");
    }

    g_scrtext.printf("pushbuffercalls:%d", ttres.pushbuffercalls);
    g_scrtext.printf("pushbuffersize :%d", ttres.pushbuffersize);

#if PROFILE
    g_scrtext.printf("pushbufferwaits:%d",   ttres.PushBufferWaitInfo.Count);
    g_scrtext.printf("pshbfwaittime:%.2fms", (double)(ttres.PushBufferWaitInfo.Cycles / 733) / 1000.0);
#endif

    // print out results
    g_scrtext.printf("");

    g_scrtext.printf("fps:            %.2f",     ttres.fps);
    g_scrtext.printf("frames:         %d",       ttres.cFrames);
    g_scrtext.printf("time:           %.2fms",   ttres.TimeTillIdle);

    if(g_var.pushbuffernops)
    {
        g_scrtext.printf("nops:           %I64u",    ttres.cTrisDrawn);
        g_scrtext.printf("Mnops:          %.2f",     ttres.mtps);
        g_scrtext.printf("cycles/nop:     %.2f",     ttres.cycles);
    }
    else
    {
        g_scrtext.printf("triangles:      %I64u",    ttres.cTrisDrawn);
        g_scrtext.printf("Mtps:           %.2f",     ttres.mtps);
        g_scrtext.printf("cycles/vert:    %.2f",     ttres.cycles);
    }
}

//=========================================================================
//
//=========================================================================
inline char *MyStrCpy(char *dst, const char *src)
{
    while(*src && (*dst++ = *src++))
        ;

    return dst;
}

//=========================================================================
// Return a static char buffer with stream declaration
//=========================================================================
const char *SzGetDeclDescr(DWORD dwStream, DWORD *pDecl)
{
    static char szBuf[128];
    DWORD dwBytes = 0;
    DWORD dwCurStream = (DWORD)-1;

    char *psz = szBuf;

    while(*pDecl != D3DVSD_END())
    {
        if(VSDGetTokenType(*pDecl) == D3DVSD_TOKEN_STREAM)
        {
            dwCurStream = *pDecl & D3DVSD_STREAMNUMBERMASK;
        }
        else if(dwStream == dwCurStream &&
            VSDGetTokenType(*pDecl) == D3DVSD_TOKEN_STREAMDATA)
        {
            static const char rgReg[] = "0123456789abcdef";

            if(psz != szBuf)
                *psz++ = ',';

            if(*pDecl & 0x18000000)
            {
                psz = MyStrCpy(psz, (*pDecl & 0x08000000) ? "x:BYTES" : "x:DWRDS");
                *psz++ = char('0' + VSDGetSkipCount(*pDecl));
            }
            else
            {
                static const char *rgType[] =
                    { "CLR", "NRMSHRT", "FLT", "?", "PBYTE", "SHRT", "NRMPCKD", "??" };
                DWORD dwType = VSDGetDataType(*pDecl);
                DWORD dwCount = (dwType >> 4) & 0xf;

                *psz++ = char(rgReg[*pDecl & 0xf]);
                *psz++ = ':';

                psz = MyStrCpy(psz, rgType[min(dwType & 0xf, ARRAYSIZE(rgType))]);
                *psz++ = char('0' + dwCount);
            }
        }

        pDecl++;
    }

    *psz = 0;
    return szBuf;
}

//=========================================================================
// Dump the current options
//=========================================================================
void DumpCurrentSettings()
{
    if(g_var.pushbuffernops)
    {
        g_scrtext.printf("precompiled:    %d (w/ Kelvin nops)", g_var.precompiled);
        g_scrtext.printf("runusingcpucopy:%d",      g_var.runusingcpucopy);
        g_scrtext.printf("nop count:      %d * %d", g_var.indexcount, g_var.vbdrawcount);
    }
    else
    {
        DWORD dwFVFSizeTot = 0;

        for(DWORD istream = 0; istream < g_var.NumStreams; istream++)
            dwFVFSizeTot += g_dwFVFSize[istream];

        g_scrtext.printf("type:          %s",       szGetPTType(g_var.primitivetype));
        g_scrtext.printf("precompiled:   %d - %d pushbuffer(s)",
            g_var.precompiled, g_var.numberofpushbuffers);

        if(g_var.precompiled)
            g_scrtext.printf("runusingcpucopy:%d",      g_var.runusingcpucopy);

        if(g_var.uservertexshader)
            g_scrtext.printf("vertshaderlen: %d",   g_var.uservertexshader);

        g_scrtext.printf("vertcount:     %d",       g_var.vertcount);
        g_scrtext.printf("indexcount:    %d*%d",    g_var.indexcount, g_var.vbdrawcount);

        g_scrtext.printf("numtextures:   %d",       g_var.numtextures);
        g_scrtext.printf("FVF size:      %d bytes", dwFVFSizeTot);

        for(DWORD istream = 0; istream < g_var.NumStreams; istream++)
            g_scrtext.printf("  stream % d:   %s", istream, SzGetDeclDescr(istream, g_Declaration));

#undef XTAG
#define XTAG(_tag)    ((g_var. ## _tag) ? #_tag : "!" #_tag)

        // spew various render states
        g_scrtext.printf("%s %s %s %s",
            XTAG(zenable), XTAG(zwriteenable),
            XTAG(alphablendenable), XTAG(alphatestenable));

        g_scrtext.printf("%s %s %s %s %s",
            XTAG(localviewer), XTAG(specularenable),
            XTAG(fogenable), XTAG(normalizenormals), XTAG(colorvertex));

        if(g_var.lighting)
        {
            g_scrtext.printf("%s: directional:%d point:%d spot:%d", XTAG(lighting),
                g_var.numDirectionalLights, g_var.numPointLights, g_var.numSpotLights);
        }
        else
        {
            g_scrtext.printf("%s", XTAG(lighting));
        }
    }
}

//=========================================================================
// Take a wild guess at the amount of time one scene will take
//=========================================================================
float EstimateTimeForScene()
{
    CTimer timer;

    // clear the screen
    ClearScreen();
    CheckHR(g_pDev->Present(NULL, NULL, NULL, NULL));

    InitFrame();

    // wait until everything is cleared out
    g_pDev->BlockUntilIdle();

    // start timing
    timer.Start();

    // draw one scene
    RenderFrame(g_var.vbdrawcount, g_var.pushbuffernops);

    // wait until GPU is idle again
    g_pDev->BlockUntilIdle();

    // stop timer
    timer.Stop();

    return timer.getTime();
}

// helper struct used in PrecompiledCallback
struct GPUINFO
{
    CTimer *ptimer;
    DWORD byteswritten;
    DWORD bytes;
} g_gpuInfo;

//=========================================================================
// Called back at DPC level when the test is done
//=========================================================================
void __cdecl PrecompiledCallback(DWORD context)
{
    GPUINFO* pgpuInfo = (GPUINFO *)context;

    // stop the timer
    pgpuInfo->ptimer->Stop();

    // record how many bytes we've written
    pgpuInfo->byteswritten = pgpuInfo->bytes;

    g_gpudrawingdone = true;
}

//=========================================================================
// Block or do wc writes until GPU is idle
//=========================================================================
void WaitOrWriteUntilIdle()
{
    DWORD batch = 0;

    g_pDev->InsertCallback(D3DCALLBACK_WRITE, PrecompiledCallback, (DWORD)&g_gpuInfo);
    g_pDev->KickPushBuffer();

    if(g_var.wcwritesinbkgnd == 1)
    {
        // do some wc writes w/ the main thread while waiting
        while(!g_gpudrawingdone)
        {
            DWORD* pDst = g_pAlloc;

            batch = g_AllocSize / sizeof(DWORD);

            // pDst = (DWORD*) ((DWORD) pDst | 0xf0000000);

            do {
                *pDst++ = g_gpuInfo.bytes;

                g_gpuInfo.bytes += 4;

            } while (--batch != 0);
        }
    }
    else
    {
        // wait until GPU is idle again
        g_pDev->BlockUntilIdle();
    }
}

//=========================================================================
//
//=========================================================================
void CreatePushBuffers(IDirect3DPushBuffer8 *pPushbuffer[300],
    int numberscenes, DWORD *psize, int *pframesperpushbuffer)
{
    DWORD size;
    int framesperpushbuffer;
    const int MaxPushbufferSize = 32*1024*1024;

    // calculate pushbuffer size
    IDirect3DPushBuffer8 *pPushBufferT;
    CheckHR(g_pDev->CreatePushBuffer(MaxPushbufferSize, g_var.runusingcpucopy, &pPushBufferT));

    // render one frame - and get the size
    CheckHR(g_pDev->BeginPushBuffer(pPushBufferT));
    RenderFrame(g_var.vbdrawcount, g_var.pushbuffernops);
    CheckHR(g_pDev->GetPushBufferOffset(&size));

    // we're capped by the push-buffer size or the total frame count
    framesperpushbuffer = min(numberscenes, MaxPushbufferSize / (int)size);

    // cap the guy at the max # of frames allowed in our pushbuffer
    if(g_var.framesperpushbuffermax)
        framesperpushbuffer = min(g_var.framesperpushbuffermax, (DWORD)framesperpushbuffer);

    // render the remaining frames in there
    for(int i = 1; i < framesperpushbuffer; i++)
    {
        RenderFrame(g_var.vbdrawcount, g_var.pushbuffernops);
    }

    CheckHR(g_pDev->GetPushBufferOffset(&size));

    CheckHR(g_pDev->EndPushBuffer());

    // ok - cruise through and create X number of pushbuffers
    g_var.numberofpushbuffers = min(300, max(1, g_var.numberofpushbuffers));

    if(g_var.numberofpushbuffers == 1)
    {
        pPushbuffer[0] = pPushBufferT;
        pPushBufferT = NULL;
    }
    else
    {
        for(DWORD ipushbuf = 0; ipushbuf < g_var.numberofpushbuffers; ipushbuf++)
        {
            // create precompiled pushbuffer
            CheckHR(g_pDev->CreatePushBuffer(max(512, size) + sizeof(DWORD),
                g_var.runusingcpucopy, &pPushbuffer[ipushbuf]));

            assert(pPushbuffer[ipushbuf]->AllocationSize >=
                pPushBufferT->Size);

            memcpy((void *)pPushbuffer[ipushbuf]->Data,
                (void *)pPushBufferT->Data, pPushBufferT->Size);

            pPushbuffer[ipushbuf]->Size = pPushBufferT->Size;
        }
    }

    *psize = size;
    *pframesperpushbuffer = framesperpushbuffer;

    RELEASE(pPushBufferT);
}

//=========================================================================
// Do a timed test and spit out the results
//=========================================================================
float DoTimedTest(TIMETESTRESULTS *pttres)
{
    CTimer timer;
    int frames = 0;
    int visibilityindex = 0;
    float TimeTillDoneDrawing = 0;
    float TimeTillIdle = 0;
    UINT64 cIndicesDrawn = 0;
    UINT64 cTrisDrawn = 0;
    D3DWAITINFO *pPushBufferWaitInfo =
        &g_pPerf->m_PerformanceCounters[PERF_PUSHBUFFER_WAITS];

    // if there isn't an explicit count of scenes to render
    // guesstimate the time required for each scene
    float fEstimatedTimePerScene = EstimateTimeForScene();

    // and figure out how many scenes to render
    int numberscenes = int(g_var.testtime / fEstimatedTimePerScene);

    // put an upper cap and lower bound on the number of scenes
    numberscenes = max(2, min(numberscenes, INT_MAX));

    // set up stream sources
    InitFrame();

    g_gpuInfo.ptimer = &timer;
    g_gpuInfo.byteswritten = 0;
    g_gpuInfo.bytes = 0;

    if(g_var.precompiled)
    {
        DWORD size;
        int framesperpushbuffer;
        static IDirect3DPushBuffer8 *pPushbuffer[300];

        // Create our pushbuffers
        CreatePushBuffers(pPushbuffer, numberscenes, &size, &framesperpushbuffer);

        int pushbuffercalls = (numberscenes / framesperpushbuffer) + 1;

        pPushBufferWaitInfo->Count = 0;
        pPushBufferWaitInfo->Cycles = 0;

        // wait until everything is cleared out
        g_pDev->BlockUntilIdle();

        // start timing
        timer.Start();
        g_gpudrawingdone = false;

        // render X number of scenes
        DWORD ipushbuf = 0;
        for(int pushbuffers = 0; pushbuffers < pushbuffercalls; pushbuffers++)
        {
            CheckHR(g_pDev->RunPushBuffer(pPushbuffer[ipushbuf], NULL));

            if(++ipushbuf >= g_var.numberofpushbuffers)
                ipushbuf = 0;
        }

        TimeTillDoneDrawing = timer.getTime();

        WaitOrWriteUntilIdle();

        TimeTillIdle = timer.getTime();

        frames = pushbuffercalls * framesperpushbuffer;
        cIndicesDrawn = (UINT64)pushbuffercalls * framesperpushbuffer * g_var.vbdrawcount * g_var.indexcount;
        cTrisDrawn = (UINT64)pushbuffercalls * framesperpushbuffer * g_var.vbdrawcount * g_var.TriCount;

        if(pttres)
        {
            pttres->pushbuffercalls = pushbuffercalls;
            pttres->pushbuffersize = size;
        }

        for(ipushbuf = 0; ipushbuf < g_var.numberofpushbuffers; ipushbuf++)
            RELEASE(pPushbuffer[ipushbuf]);
    }
    else
    {
        pPushBufferWaitInfo->Count = 0;
        pPushBufferWaitInfo->Cycles = 0;

        // wait until everything is cleared out
        g_pDev->BlockUntilIdle();

        // start timing
        timer.Start();
        g_gpudrawingdone = false;

        // render X number of scenes
        for(frames = 0; frames < numberscenes; frames++)
        {
            RenderFrame(g_var.vbdrawcount);
        }

        TimeTillDoneDrawing = timer.getTime();

        // wait until GPU is idle again
        WaitOrWriteUntilIdle();

        TimeTillIdle = timer.getTime();

        cIndicesDrawn = (UINT64)frames * g_var.vbdrawcount * g_var.indexcount;
        cTrisDrawn = (UINT64)frames * g_var.vbdrawcount * g_var.TriCount;
    }

    if(pttres)
    {
        pttres->fps = frames * 1000 / TimeTillIdle;
        pttres->cFrames = frames;
        pttres->TimeTillDoneDrawing = TimeTillDoneDrawing;
        pttres->TimeTillIdle = TimeTillIdle;
        pttres->cTrisDrawn = cTrisDrawn;
        pttres->mtps = (float)(pttres->cTrisDrawn / (pttres->TimeTillIdle * 1000));
        pttres->cycles = g_nvclk * pttres->TimeTillIdle * 1000.0f / cIndicesDrawn;

        pttres->byteswritten = g_gpuInfo.byteswritten;
        pttres->throughput = g_gpuInfo.byteswritten / (TimeTillIdle * 1000.0f);
        pttres->PushBufferWaitInfo = *pPushBufferWaitInfo;
    }

    return pttres->fps;
}

//=========================================================================
// macros to help read entries from the ini file
//=========================================================================
static const char g_szOpts[] = "options";
#define get_option_val(_var)          g_inifile.GetIniInt(g_szOpts, #_var, g_var._var)
#define get_option_valf(_var)         g_inifile.GetIniFloat(g_szOpts, #_var, g_var._var)
#define get_option_val3(_s, _var, _d) g_inifile.GetIniInt(_s, _var, _d)

//=========================================================================
// Read ini file and update all fr settings
//=========================================================================
bool ReadVSTIniFile()
{
    if(!g_inifile.ReadFile())
        return false;

    // read the verbose flag(s) first
    g_inifile.m_fverbose            = !!get_option_val3(g_szOpts, "verboseinireads", false);
    g_var.verboselevel              = get_option_val(verboselevel);

    // if the g_iCurTest entry exists, read it in
    if(g_inifile.IniEntryExists(g_szOpts, "g_iCurTest"))
        g_iCurTest = g_inifile.GetIniInt(g_szOpts, "g_iCurTest", g_iCurTest);

    g_var.runtestonreload           = !!get_option_val(runtestonreload);

    g_var.AutoDepthStencilFormat    = (D3DFORMAT)get_option_val(AutoDepthStencilFormat);
    g_var.BackBufferFormat          = (D3DFORMAT)get_option_val(BackBufferFormat);

    g_var.MSType                    = (D3DMULTISAMPLE_TYPE)get_option_val(MSType);
    g_var.MSFormat                  = (D3DMULTISAMPLE_TYPE)get_option_val(MSFormat);

    g_var.zenable                   = !!get_option_val(zenable);
    g_var.zwriteenable              = !!get_option_val(zwriteenable);
    g_var.d3dcmpfunc                = (D3DCMPFUNC)get_option_val(d3dcmpfunc);

    g_var.alphablendenable          = !!get_option_val(alphablendenable);
    g_var.alphatestenable           = !!get_option_val(alphatestenable);

    g_var.testtime                  = get_option_valf(testtime);
    g_var.vbdrawcount               = get_option_val(vbdrawcount);
    g_var.precompiled               = get_option_val(precompiled);
    g_var.framesperpushbuffermax    = get_option_val(framesperpushbuffermax);
    g_var.pushbuffernops            = g_var.precompiled ? get_option_val(pushbuffernops) : 0;
    g_var.runusingcpucopy           = g_var.precompiled && !!get_option_val(runusingcpucopy);
    g_var.numberofpushbuffers       = get_option_val(numberofpushbuffers);
    g_var.wcwritesinbkgnd           = get_option_val(wcwritesinbkgnd);

    g_var.FVF                       = get_option_val(FVF);
    g_var.primitivetype             = (D3DPRIMITIVETYPE)get_option_val(primitivetype);
    g_var.indexcount                = get_option_val(indexcount);
    g_var.vertcount                 = get_option_val(vertcount);

    g_var.cullmode                  = (D3DCULL)get_option_val(cullmode);
    g_var.d3dfillmode               = (D3DFILLMODE)get_option_val(d3dfillmode);

    g_var.degenerates               = !!get_option_val(degenerates);

    g_var.clearflags                = get_option_val(clearflags);
    g_var.zclearval                 = get_option_valf(zclearval);
    g_var.sclearval                 = get_option_val(sclearval);
    g_var.cclearval                 = get_option_val(cclearval);

    g_var.uservertexshader          = get_option_val(uservertexshader);

    g_var.colorvertex               = !!get_option_val(colorvertex);
    g_var.normalizenormals          = !!get_option_val(normalizenormals);
    g_var.fogenable                 = !!get_option_val(fogenable);

    g_var.localviewer               = !!get_option_val(localviewer);
    g_var.specularenable            = !!get_option_val(specularenable);

    g_var.lighting                  = !!get_option_val(lighting);
    g_var.numDirectionalLights      = get_option_val(numDirectionalLights);
    g_var.numPointLights            = get_option_val(numPointLights);
    g_var.numSpotLights             = get_option_val(numSpotLights);

    g_var.numtextures               = get_option_val(numtextures);

    // Read in the explicit declaration if we don't have an FVF code
    g_var.NumStreams = 1;
    if(!g_var.FVF)
    {
        char szStream[20];
        char szEntry[20];
        int idecl = 0;

        for(int istream = 0; istream < 15; istream++)
        {
            wsprintfA(szStream, "stream%d", istream);
            if(!g_inifile.IniEntryExists(szStream, "Decl0"))
            {
                break;
            }
            else
            {
                g_Declaration[idecl++] = D3DVSD_STREAM(istream);

                for(int ientry = 0; ientry < 15; ientry++)
                {
                    wsprintfA(szEntry, "Decl%d", ientry);
                    if(!g_inifile.IniEntryExists(szStream, szEntry))
                        break;

                    DWORD dwDecl = g_inifile.GetIniInt(szStream, szEntry, 0);

                    if(dwDecl & 0x08000000)
                        g_Declaration[idecl++] = D3DVSD_SKIPBYTES(LOWORD(dwDecl));
                    else if(dwDecl & 0x10000000)
                        g_Declaration[idecl++] = D3DVSD_SKIP(LOWORD(dwDecl));
                    else
                        g_Declaration[idecl++] = D3DVSD_REG(LOWORD(dwDecl), HIWORD(dwDecl));
                }
            }
        }

        g_Declaration[idecl] = D3DVSD_END();

        g_var.NumStreams = istream;
    }

    return true;
}

//=========================================================================
// Background thread loop.
//=========================================================================
DWORD WINAPI ThreadProc(LPVOID lpParameter)
{
    DWORD batch;

    for(;;)
    {
        if(!g_gpudrawingdone)
        {
            DWORD* pDst = g_pAlloc;

            batch = g_AllocSize / sizeof(DWORD);

            // pDst = (DWORD*) ((DWORD) pDst | 0xf0000000);
            do {
                *pDst++ = g_gpuInfo.bytes;

                g_gpuInfo.bytes += 4;

            } while (--batch != 0);
        }
    }

    return 0;
}


//=========================================================================
// main
//=========================================================================
void __cdecl main()
{
    DWORD dwT = 0x418937;

    dprintf("TEST: %lu %lu\n", dwT * 1000 / 48000, (dwT + 1) * 1000 / 48000);
    
    g_hThread = CreateThread(NULL, 0, ThreadProc, 0, CREATE_SUSPENDED, NULL);
    SetThreadPriority(g_hThread, THREAD_PRIORITY_BELOW_NORMAL);

#if PROFILE
    g_pPerf = D3DPERF_GetStatistics();
#else
    static D3DPERF perf = {0};
    g_pPerf = &perf;
#endif

    /*
     * init vars
     */
    g_var.screenwidth = 640.0f;
    g_var.screenheight = 480.0f;

    g_var.FVF = D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1;
    g_var.primitivetype = D3DPT_TRIANGLELIST;
    g_var.indexcount = 4998;
    g_var.vertcount = 27;

    g_var.cullmode = D3DCULL_NONE;

    g_var.degenerates = false;

    g_var.alphablendenable = false;
    g_var.alphatestenable = false;

    // default to drawing 10 seconds
    g_var.testtime = 10000.0f;
    g_var.vbdrawcount = 50;

    g_var.d3dfillmode = D3DFILL_SOLID;

    g_var.zenable = false;
    g_var.zwriteenable = false;

    g_var.d3dcmpfunc = D3DCMP_NOTEQUAL;

    g_var.BackBufferFormat = D3DFMT_A8R8G8B8;
    g_var.AutoDepthStencilFormat = D3DFMT_D24S8;

    g_var.clearflags = D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL;
    g_var.cclearval = 0x00006688;


    g_pAlloc = (DWORD*) XPhysicalAlloc(g_AllocSize, MAXULONG_PTR, 4096,
                                       PAGE_READWRITE | PAGE_WRITECOMBINE);

    CheckHR(g_pAlloc != NULL ? S_OK : E_OUTOFMEMORY);

    /*
     * Init vstune
     */
    XInitDevices(0, NULL);

    DWORD dwLastTickCount = GetTickCount();
    for(;;)
    {
        DWORD dwTickCount = GetTickCount();
        DWORD dwTicks = dwTickCount - dwLastTickCount;

        dwLastTickCount = dwTickCount;

        HandleInput(dwTicks);

        if(g_pDev)
        {
            if(g_fRedrawScreen)
            {
                ClearScreen();

                // draw back frame with current settings
                InitFrame();
                RenderFrame(1);

                // swap that guy to the front
                CheckHR(g_pDev->Present(NULL, NULL, NULL, NULL));

                // display current options on front buffer
                g_scrtext.cls();

                g_scrtext.printf("Btn A) run '%s' test.", g_rgTests[g_iCurTest].szDesc);
                g_scrtext.printf("");
                DumpCurrentSettings();
                g_scrtext.drawtext(g_var.verboselevel >= 1);

                g_fRedrawScreen = false;
            }
            else
            {
                // 33.3 fps baby!
                Sleep(30);
            }
        }
    }
}

//=========================================================================
// Callback from inifile.cpp to get float string value
//=========================================================================
bool GetIniConstf(const char *szStr, int cchStr, float *pval)
{
    return false;
}

//=========================================================================
// Callback from inifile.cpp to get int string value
//=========================================================================
bool GetIniConst(const char *szStr, int cchStr, int *pval)
{
    #undef XTAG
    #define XTAG(_tag) { _tag, #_tag }
    static const struct
    {
        int Val;
        const char *szStr;
    } rgszConsts[] =
    {
        // D3DFORMATs
        XTAG(D3DFMT_A8R8G8B8), XTAG(D3DFMT_X8R8G8B8), XTAG(D3DFMT_R5G6B5), XTAG(D3DFMT_R6G5B5),
        XTAG(D3DFMT_X1R5G5B5), XTAG(D3DFMT_A1R5G5B5), XTAG(D3DFMT_A4R4G4B4), XTAG(D3DFMT_A8),
        XTAG(D3DFMT_A8B8G8R8), XTAG(D3DFMT_B8G8R8A8), XTAG(D3DFMT_R4G4B4A4), XTAG(D3DFMT_R5G5B5A1),
        XTAG(D3DFMT_R8G8B8A8), XTAG(D3DFMT_R8B8), XTAG(D3DFMT_G8B8), XTAG(D3DFMT_P8),
        XTAG(D3DFMT_L8), XTAG(D3DFMT_A8L8), XTAG(D3DFMT_AL8), XTAG(D3DFMT_L16),
        XTAG(D3DFMT_V8U8), XTAG(D3DFMT_L6V5U5), XTAG(D3DFMT_X8L8V8U8), XTAG(D3DFMT_Q8W8V8U8),
        XTAG(D3DFMT_V16U16), XTAG(D3DFMT_D16_LOCKABLE), XTAG(D3DFMT_D16), XTAG(D3DFMT_D24S8),
        XTAG(D3DFMT_F16), XTAG(D3DFMT_F24S8), XTAG(D3DFMT_UYVY), XTAG(D3DFMT_YUY2),
        XTAG(D3DFMT_DXT1), XTAG(D3DFMT_DXT2), XTAG(D3DFMT_DXT3), XTAG(D3DFMT_DXT4),
        XTAG(D3DFMT_DXT5), XTAG(D3DFMT_LIN_A1R5G5B5), XTAG(D3DFMT_LIN_A4R4G4B4), XTAG(D3DFMT_LIN_A8),
        XTAG(D3DFMT_LIN_A8B8G8R8), XTAG(D3DFMT_LIN_A8R8G8B8), XTAG(D3DFMT_LIN_B8G8R8A8), XTAG(D3DFMT_LIN_G8B8),
        XTAG(D3DFMT_LIN_R4G4B4A4), XTAG(D3DFMT_LIN_R5G5B5A1), XTAG(D3DFMT_LIN_R5G6B5), XTAG(D3DFMT_LIN_R6G5B5),
        XTAG(D3DFMT_LIN_R8B8), XTAG(D3DFMT_LIN_R8G8B8A8), XTAG(D3DFMT_LIN_X1R5G5B5), XTAG(D3DFMT_LIN_X8R8G8B8),
        XTAG(D3DFMT_LIN_A8L8), XTAG(D3DFMT_LIN_AL8), XTAG(D3DFMT_LIN_L16), XTAG(D3DFMT_LIN_L8),
        XTAG(D3DFMT_LIN_D24S8), XTAG(D3DFMT_LIN_F24S8), XTAG(D3DFMT_LIN_D16), XTAG(D3DFMT_LIN_F16),

        XTAG(D3DMULTISAMPLE_NONE), XTAG(D3DMULTISAMPLE_2_SAMPLES_MULTISAMPLE_LINEAR),
        XTAG(D3DMULTISAMPLE_2_SAMPLES_MULTISAMPLE_QUINCUNX), XTAG(D3DMULTISAMPLE_2_SAMPLES_SUPERSAMPLE_HORIZONTAL_LINEAR),
        XTAG(D3DMULTISAMPLE_2_SAMPLES_SUPERSAMPLE_VERTICAL_LINEAR), XTAG(D3DMULTISAMPLE_4_SAMPLES_MULTISAMPLE_LINEAR),
        XTAG(D3DMULTISAMPLE_4_SAMPLES_MULTISAMPLE_GAUSSIAN), XTAG(D3DMULTISAMPLE_4_SAMPLES_SUPERSAMPLE_LINEAR),
        XTAG(D3DMULTISAMPLE_4_SAMPLES_SUPERSAMPLE_GAUSSIAN), XTAG(D3DMULTISAMPLE_9_SAMPLES_MULTISAMPLE_GAUSSIAN),
        XTAG(D3DMULTISAMPLE_9_SAMPLES_SUPERSAMPLE_GAUSSIAN), XTAG(D3DMULTISAMPLE_PREFILTER_FORMAT_DEFAULT),
        XTAG(D3DMULTISAMPLE_PREFILTER_FORMAT_X1R5G5B5), XTAG(D3DMULTISAMPLE_PREFILTER_FORMAT_R5G6B5),
        XTAG(D3DMULTISAMPLE_PREFILTER_FORMAT_X8R8G8B8), XTAG(D3DMULTISAMPLE_PREFILTER_FORMAT_A8R8G8B8),

        XTAG(D3DMS_NONE), XTAG(D3DMS_2_SAMPLES_MULTISAMPLE_LINEAR),
        XTAG(D3DMS_2_SAMPLES_MULTISAMPLE_QUINCUNX), XTAG(D3DMS_2_SAMPLES_SUPERSAMPLE_HORIZONTAL_LINEAR),
        XTAG(D3DMS_2_SAMPLES_SUPERSAMPLE_VERTICAL_LINEAR), XTAG(D3DMS_4_SAMPLES_MULTISAMPLE_LINEAR),
        XTAG(D3DMS_4_SAMPLES_MULTISAMPLE_GAUSSIAN), XTAG(D3DMS_4_SAMPLES_SUPERSAMPLE_LINEAR),
        XTAG(D3DMS_4_SAMPLES_SUPERSAMPLE_GAUSSIAN), XTAG(D3DMS_9_SAMPLES_MULTISAMPLE_GAUSSIAN),
        XTAG(D3DMS_9_SAMPLES_SUPERSAMPLE_GAUSSIAN), XTAG(D3DMS_PREFILTER_FORMAT_DEFAULT),
        XTAG(D3DMS_PREFILTER_FORMAT_X1R5G5B5), XTAG(D3DMS_PREFILTER_FORMAT_R5G6B5),
        XTAG(D3DMS_PREFILTER_FORMAT_X8R8G8B8), XTAG(D3DMS_PREFILTER_FORMAT_A8R8G8B8),

        XTAG(D3DCLEAR_TARGET), XTAG(D3DCLEAR_ZBUFFER), XTAG(D3DCLEAR_STENCIL),

        // FVFs
        XTAG(D3DFVF_XYZ), XTAG(D3DFVF_XYZRHW), XTAG(D3DFVF_NORMAL), XTAG(D3DFVF_DIFFUSE),
        XTAG(D3DFVF_SPECULAR), XTAG(D3DFVF_TEX0), XTAG(D3DFVF_TEX1), XTAG(D3DFVF_TEX2),
        XTAG(D3DFVF_TEX3), XTAG(D3DFVF_TEX4),

        // textcoord sizes
        XTAG(T0_SIZE1), XTAG(T0_SIZE2), XTAG(T0_SIZE3), XTAG(T0_SIZE4),
        XTAG(T1_SIZE1), XTAG(T1_SIZE2), XTAG(T1_SIZE3), XTAG(T1_SIZE4),
        XTAG(T2_SIZE1), XTAG(T2_SIZE2), XTAG(T2_SIZE3), XTAG(T2_SIZE4),
        XTAG(T3_SIZE1), XTAG(T3_SIZE2), XTAG(T3_SIZE3), XTAG(T3_SIZE4),

        XTAG(VSDT_FLOAT1), XTAG(VSDT_FLOAT2), XTAG(VSDT_FLOAT3), XTAG(VSDT_FLOAT4),
        XTAG(VSDT_D3DCOLOR), XTAG(VSDT_SHORT2), XTAG(VSDT_SHORT4), XTAG(VSDT_NORMSHORT1),
        XTAG(VSDT_NORMSHORT2), XTAG(VSDT_NORMSHORT3), XTAG(VSDT_NORMSHORT4), XTAG(VSDT_NORMPACKED3),
        XTAG(VSDT_SHORT1), XTAG(VSDT_SHORT3), XTAG(VSDT_PBYTE1), XTAG(VSDT_PBYTE2),
        XTAG(VSDT_PBYTE3), XTAG(VSDT_PBYTE4), XTAG(VSDT_FLOAT2H), XTAG(VSDT_NONE),

        XTAG(D3DVSDE_POSITION), XTAG(D3DVSDE_BLENDWEIGHT), XTAG(D3DVSDE_NORMAL), XTAG(D3DVSDE_DIFFUSE),
        XTAG(D3DVSDE_SPECULAR), XTAG(D3DVSDE_FOG), XTAG(D3DVSDE_BACKDIFFUSE), XTAG(D3DVSDE_BACKSPECULAR),
        XTAG(D3DVSDE_TEXCOORD0), XTAG(D3DVSDE_TEXCOORD1), XTAG(D3DVSDE_TEXCOORD2), XTAG(D3DVSDE_TEXCOORD3),

        // primitive types
        XTAG(D3DPT_POINTLIST), XTAG(D3DPT_LINELIST), XTAG(D3DPT_LINELOOP),
        XTAG(D3DPT_LINESTRIP), XTAG(D3DPT_TRIANGLELIST), XTAG(D3DPT_TRIANGLESTRIP),
        XTAG(D3DPT_TRIANGLEFAN), XTAG(D3DPT_QUADLIST), XTAG(D3DPT_QUADSTRIP),
        XTAG(D3DPT_POLYGON),

        XTAG(D3DCULL_NONE), XTAG(D3DCULL_CCW), XTAG(D3DCULL_CW),
        XTAG(D3DFILL_POINT), XTAG(D3DFILL_WIREFRAME), XTAG(D3DFILL_SOLID),

        // D3DCMPs
        XTAG(D3DCMP_NEVER), XTAG(D3DCMP_LESS), XTAG(D3DCMP_EQUAL), XTAG(D3DCMP_LESSEQUAL),
        XTAG(D3DCMP_GREATER), XTAG(D3DCMP_NOTEQUAL), XTAG(D3DCMP_GREATEREQUAL), XTAG(D3DCMP_ALWAYS),

        // misc
        XTAG(FALSE), XTAG(TRUE)
    };

    for(int ifmt = 0; ifmt < ARRAYSIZE(rgszConsts); ifmt++)
    {
        if(!_strnicmp(rgszConsts[ifmt].szStr, szStr, cchStr))
        {
            // set val
            *pval = rgszConsts[ifmt].Val;
            return true;
        }
    }

    return false;
}

//=========================================================================
//  function    MyNotBrokenD3DXDeclaratorFromFVF
//
//   devnote    Generates a declarator from a given FVF. And pays attention
//              to the D3DFVF_TEXTUREFORMAT1 bits unlike the piece of crap
//              D3DX routine.
//
//              Note that it doesn't handle D3DFVF_XYZB*
//=========================================================================
void MyNotBrokenD3DXDeclaratorFromFVF(DWORD dwFVF, DWORD Declarator[MAX_FVF_DECL_SIZE])
{
    DWORD iTemp = 0;

    Declarator[iTemp++] = D3DVSD_STREAM(0);

    if(dwFVF & D3DFVF_XYZ)
        Declarator[iTemp++] = D3DVSD_REG( D3DVSDE_POSITION,  D3DVSDT_FLOAT3);
    else if(dwFVF & D3DFVF_XYZRHW)
        Declarator[iTemp++] = D3DVSD_REG( D3DVSDE_POSITION,  D3DVSDT_FLOAT4);

    if(dwFVF & D3DFVF_NORMAL)
        Declarator[iTemp++] = D3DVSD_REG( D3DVSDE_NORMAL,  D3DVSDT_FLOAT3);

    if(dwFVF & D3DFVF_DIFFUSE)
        Declarator[iTemp++] = D3DVSD_REG( D3DVSDE_DIFFUSE,  D3DVSDT_D3DCOLOR);

    if(dwFVF & D3DFVF_SPECULAR)
        Declarator[iTemp++] = D3DVSD_REG( D3DVSDE_SPECULAR,  D3DVSDT_D3DCOLOR);

    // Texture coordinates
    UINT uNumTexCoords = ((dwFVF & D3DFVF_TEXCOUNT_MASK) >> D3DFVF_TEXCOUNT_SHIFT);
    UINT uTextureFormats = dwFVF >> 16;

    if(uTextureFormats)
    {
        for(UINT i = 0; i < uNumTexCoords; i++)
        {
            DWORD dwVsdt;

            switch(uTextureFormats & 3)
            {
            case D3DFVF_TEXTUREFORMAT1:     // u
                dwVsdt = D3DVSDT_FLOAT1;
                break;
            case D3DFVF_TEXTUREFORMAT2:     // u,v
                dwVsdt = D3DVSDT_FLOAT2;
                break;
            case D3DFVF_TEXTUREFORMAT3:     // s,t,r
                dwVsdt = D3DVSDT_FLOAT3;
                break;
            case D3DFVF_TEXTUREFORMAT4:     // s,t,r,q
                dwVsdt = D3DVSDT_FLOAT4;
                break;
            }

            Declarator[iTemp++] = D3DVSD_REG(D3DVSDE_TEXCOORD0 + i, dwVsdt),
            uTextureFormats >>= 2;
        }
    }
    else while(uNumTexCoords--)
    {
        // u,v
        Declarator[iTemp++] = D3DVSD_REG(D3DVSDE_TEXCOORD0 + uNumTexCoords, D3DVSDT_FLOAT2);
    }

    Declarator[iTemp++] = D3DVSD_END();
}

//=========================================================================
// Initialize the vertex shader
//=========================================================================
void InitVertexShader()
{
    static const char szShaderName[] = "d:\\vstune.xvu";

    if(!g_var.FVF)
    {
        // make sure our position is initialized
        g_pDev->SetVertexData4f(D3DVSDE_VERTEX, -2000, -2000, .5f, 1.0f);
    }
    else
    {
        MyNotBrokenD3DXDeclaratorFromFVF(g_var.FVF, g_Declaration);
    }

    if(g_dwVertexShader)
    {
        g_pDev->SetVertexShader(D3DFVF_XYZ);
        g_pDev->DeleteVertexShader(g_dwVertexShader);
        g_dwVertexShader = 0;
    }

    if(g_var.uservertexshader)
    {
        HANDLE hFile = CreateFile(szShaderName,
            GENERIC_READ, FILE_SHARE_READ, NULL,
            OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
        if(hFile != INVALID_HANDLE_VALUE)
        {
            DWORD dwSize = GetFileSize(hFile, NULL);
            DWORD *pdwVertexShader = new DWORD [dwSize / sizeof(DWORD) + 1];

            DWORD NumberOfBytesRead;
            ReadFile(hFile, pdwVertexShader, dwSize, &NumberOfBytesRead, NULL);

            CheckHR(g_pDev->CreateVertexShader(g_Declaration, pdwVertexShader,
                &g_dwVertexShader, 0));

            g_var.uservertexshader = HIWORD(*pdwVertexShader);

            delete [] pdwVertexShader;
            CloseHandle(hFile);
        }
        else
        {
            dprintf("load '%s' failed: 0x%08lx\n", szShaderName);

            // set this guy to 0 and do the declaration thing below
            g_var.uservertexshader = 0;
        }
    }

    if(!g_var.uservertexshader)
    {
        CheckHR(g_pDev->CreateVertexShader(g_Declaration, NULL, &g_dwVertexShader, 0));
    }
}

//  helper routines for pinging gpu registers
extern "C" volatile DWORD *D3D__GpuReg;

_forceinline ULONG
REG_RD32(VOID* Ptr, ULONG Addr)
{
   return *((volatile DWORD*)((BYTE*)(Ptr) + (Addr)));
}

//=========================================================================
// Get the NVCLK speed
//=========================================================================
DWORD GetNVCLK()
{
    BYTE *RegBase = (BYTE *)D3D__GpuReg;

    // Calculate nvclk
    DWORD nvpll = REG_RD32(RegBase, 0x00680500);
    DWORD m = nvpll & 0xFF;
    DWORD n = (nvpll >> 8)  & 0xFF;
    DWORD p = (nvpll >> 16) & 0xFF;
    FLOAT xtal = 16.6667f;

    DWORD nvclk = (m != 0) ? (DWORD)((n * xtal / (1 << p) / m)) : 0;

    return nvclk;
}

