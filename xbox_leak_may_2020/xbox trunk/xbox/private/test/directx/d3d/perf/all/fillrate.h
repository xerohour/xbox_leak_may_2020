//-----------------------------------------------------------------------------
// FILE: FILLRATE.H
//
// Desc: fillrate header
//
// Copyright (c) 1999-2000 Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------

enum { TEX_None, TEX_2d, TEX_Cubemap, TEX_Volume, TEX_Max };
enum { TIME_Present = 0x1, TIME_Render = 0x2 };

struct TIMETESTRESULTS
{
    float   fillrate;
    float   fps;
    int     cFrames;
    float   TimeTillDoneDrawing;
    float   TimeTillIdle;
    UINT64  cNumPixelsDrawn;
};

struct FRSETTINGS
{
    UINT overdraw;                      // number of overdraws in a scene
    UINT verboselevel;                  // chatty when loading pixelshader defs, etc.
    UINT runtestonreload;               // after reloading fillrate.ini run test

    float screenwidth;                  // screen width
    float screenheight;                 // screen height
    D3DFORMAT BackBufferFormat;         // back buffer d3dformat
    D3DFORMAT AutoDepthStencilFormat;   // z buffer format (0 for none)

    D3DMULTISAMPLE_TYPE MSType;         // multisample type
    D3DMULTISAMPLE_TYPE MSFormat;       // multisample format

    DWORD FVF;                          // fvf type
    float quadwidth;                    // quad width
    float quadheight;                   // quad height
    float quadx;
    float quady;

    bool zenable;                       // enable z buffer
    bool zwriteenable;                  // enable z write
    D3DCMPFUNC d3dcmpfunc;              // z compare function

    bool primez;                        // prime z with quad/D3DCMP_ALWAYS before drawing scene
    D3DCMPFUNC primezcmpfunc;           // z prime compare function

    UINT timeflags;                     // what to time: TIME_Present, TIME_Render, ...
    int numberscenes;                   // number of scenes to time (0 to use testtime)
    float testtime;                     // number of seconds to time

    DWORD clearflags;                   // Clear() flags
    DWORD cclearval;                    // color clear val
    float zclearval;                    // z clear val
    DWORD sclearval;                    // stencil clear val

    bool userpixelshader;               // use a userpixelshader
    D3DPIXELSHADERDEF shaderdef;        // the users pixel shader def

    bool alphablendenable;              // turn on alphablending
    bool alphatestenable;               // turn on alphatesting
    bool rotatetex;                     // rotate texture

    struct FRTEX
    {
        UINT type;                      // type of texture (TEX_None, TEX_2d, TEX_Cubemap, TEX_Volume)
        UINT texsize;                   // size of texture
        D3DFORMAT d3dFormat;            // format of tetxure
        D3DTEXTUREFILTERTYPE filtertype;// filtertype
    } rgtex[4];

    D3DSWATHWIDTH SwathWidth;           // swadth width

    D3DTEXTUREOP ColorOp;               // color op (if !userpixelshader)
    D3DTEXTUREOP AlphaOp;               // alpha op (if !userpixelshader)

    D3DFILLMODE d3dfillmode;            // SOLID, WIREFRAME, ...
    DWORD colorwriteenable;             // D3DRS_COLORWRITEENABLE flags
};

// macros to help us define the size of our texcoords in fillrate.ini
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

// fillrate.cpp
float DoTimedTest(TIMETESTRESULTS *pttres = NULL);
void DoCompleteTimedTest();
void DoQuickTimedTest();
void DoMultiSampleTimedTest();
void DumpCurrentSettings();
void DoTextureTest();

void ReadPixelShaderIni(D3DPIXELSHADERDEF *pshaderdef);
bool ReadFRIniFile();

