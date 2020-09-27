//-----------------------------------------------------------------------------
// FILE: VSTUNE.H
//
// Desc: vertex shader tune header file
//
// Copyright (c) 1999-2000 Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------

struct TIMETESTRESULTS
{
    float fps;
    UINT cFrames;
    float TimeTillDoneDrawing;
    float TimeTillIdle;
    UINT64 cTrisDrawn;
    float mtps;
    float cycles;

    // precompiled results
    DWORD byteswritten;
    float throughput;
    DWORD pushbuffercalls;
    DWORD pushbuffersize;

    D3DWAITINFO PushBufferWaitInfo;
};

struct VSTSETTINGS
{
    UINT verboselevel;                  // chatty when loading pixelshader defs, etc.
    bool runtestonreload;               // after reloading fillrate.ini run test

    float screenwidth;                  // screen width
    float screenheight;                 // screen height
    D3DFORMAT BackBufferFormat;         // back buffer d3dformat
    D3DFORMAT AutoDepthStencilFormat;   // z buffer format (0 for none)

    D3DMULTISAMPLE_TYPE MSType;         // multisample type
    D3DMULTISAMPLE_TYPE MSFormat;       // multisample format

    DWORD FVF;                          // fvf type
    D3DPRIMITIVETYPE primitivetype;     // vertex buffer primitive type
    UINT indexcount;                    // count of indices
    UINT vertcount;                     // count of vertices in VB
    UINT TriCount;                      // count of triangles drawn

    DWORD uservertexshader;             // user vertex shader instruction count

    D3DCULL cullmode;                   // cullmode

    bool degenerates;                   // degenerate primitives?

    bool zenable;                       // enable z buffer
    bool zwriteenable;                  // enable z write
    D3DCMPFUNC d3dcmpfunc;              // z compare function

    float testtime;                     // number of seconds to time
    UINT vbdrawcount;                   // count of times to draw VB
    int precompiled;                    // use precompiled pushbuffer?
    int wcwritesinbkgnd;                // do wc background writes or block?
    int pushbuffernops;                 // insert noops into pushbuffer
    DWORD framesperpushbuffermax;       // max frames recorded in pushbuffer
    bool runusingcpucopy;               // copy pushbuffer
    DWORD numberofpushbuffers;          // number of pushbuffers (1 - 300) to use

    DWORD clearflags;                   // Clear() flags
    DWORD cclearval;                    // color clear val
    float zclearval;                    // z clear val
    DWORD sclearval;                    // stencil clear val

    bool alphablendenable;              // turn on alphablend
    bool alphatestenable;               // turn on alphatest

    D3DFILLMODE d3dfillmode;            // SOLID, WIREFRAME, ...

    bool localviewer;                   // D3DRS_LOCALVIEWER
    bool specularenable;                // D3DRS_SPECULARENABLE
    bool fogenable;                     // D3DRS_FOGENABLE
    bool normalizenormals;              // D3DRS_NORMALIZENORMALS
    bool colorvertex;                   // D3DRS_COLORVERTEX

    bool lighting;                      // D3DRS_LIGHTING
    DWORD numDirectionalLights;         // # of directional lights
    DWORD numPointLights;               // # of point lights
    DWORD numSpotLights;                // # of spotlights

    DWORD numtextures;                  // number of textures

    DWORD NumLights;                    // total number of lights enabled
    DWORD NumStreams;                   // total number of VB streams
};

// macros to help us define the size of our texcoords in vstune.ini
static const DWORD T0_SIZE1 = D3DFVF_TEXCOORDSIZE1(0);
static const DWORD T0_SIZE2 = D3DFVF_TEXCOORDSIZE2(0);
static const DWORD T0_SIZE3 = D3DFVF_TEXCOORDSIZE3(0);
static const DWORD T0_SIZE4 = D3DFVF_TEXCOORDSIZE4(0);

static const DWORD T1_SIZE1 = D3DFVF_TEXCOORDSIZE1(1);
static const DWORD T1_SIZE2 = D3DFVF_TEXCOORDSIZE2(1);
static const DWORD T1_SIZE3 = D3DFVF_TEXCOORDSIZE3(1);
static const DWORD T1_SIZE4 = D3DFVF_TEXCOORDSIZE4(1);

static const DWORD T2_SIZE1 = D3DFVF_TEXCOORDSIZE1(2);
static const DWORD T2_SIZE2 = D3DFVF_TEXCOORDSIZE2(2);
static const DWORD T2_SIZE3 = D3DFVF_TEXCOORDSIZE3(2);
static const DWORD T2_SIZE4 = D3DFVF_TEXCOORDSIZE4(2);

static const DWORD T3_SIZE1 = D3DFVF_TEXCOORDSIZE1(3);
static const DWORD T3_SIZE2 = D3DFVF_TEXCOORDSIZE2(3);
static const DWORD T3_SIZE3 = D3DFVF_TEXCOORDSIZE3(3);
static const DWORD T3_SIZE4 = D3DFVF_TEXCOORDSIZE4(3);

static const DWORD VSDT_FLOAT1 = D3DVSDT_FLOAT1 << 16;
static const DWORD VSDT_FLOAT2 = D3DVSDT_FLOAT2 << 16;
static const DWORD VSDT_FLOAT3 = D3DVSDT_FLOAT3 << 16;
static const DWORD VSDT_FLOAT4 = D3DVSDT_FLOAT4 << 16;
static const DWORD VSDT_D3DCOLOR = D3DVSDT_D3DCOLOR << 16;
static const DWORD VSDT_SHORT2 = D3DVSDT_SHORT2 << 16;
static const DWORD VSDT_SHORT4 = D3DVSDT_SHORT4 << 16;
static const DWORD VSDT_NORMSHORT1 = D3DVSDT_NORMSHORT1 << 16;
static const DWORD VSDT_NORMSHORT2 = D3DVSDT_NORMSHORT2 << 16;
static const DWORD VSDT_NORMSHORT3 = D3DVSDT_NORMSHORT3 << 16;
static const DWORD VSDT_NORMSHORT4 = D3DVSDT_NORMSHORT4 << 16;
static const DWORD VSDT_NORMPACKED3 = D3DVSDT_NORMPACKED3 << 16;
static const DWORD VSDT_SHORT1 = D3DVSDT_SHORT1 << 16;
static const DWORD VSDT_SHORT3 = D3DVSDT_SHORT3 << 16;
static const DWORD VSDT_PBYTE1 = D3DVSDT_PBYTE1 << 16;
static const DWORD VSDT_PBYTE2 = D3DVSDT_PBYTE2 << 16;
static const DWORD VSDT_PBYTE3 = D3DVSDT_PBYTE3 << 16;
static const DWORD VSDT_PBYTE4 = D3DVSDT_PBYTE4 << 16;
static const DWORD VSDT_FLOAT2H = D3DVSDT_FLOAT2H << 16;
static const DWORD VSDT_NONE = D3DVSDT_NONE << 16;

// vstune.cpp
bool ReadVSTIniFile();

void DoQuickTimedTest();
void DoPushBufferCallTest();
void RandomVS();

void DumpCurrentSettings();
float DoTimedTest(TIMETESTRESULTS *pttres = NULL);
void InitVertexShader();
void ClearScreen();
DWORD GetNVCLK();
void RenderFrame(int vbdrawcount, BOOL nops = FALSE);

