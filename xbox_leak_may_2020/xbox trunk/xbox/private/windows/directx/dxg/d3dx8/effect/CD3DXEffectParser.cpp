///////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) 1999 Microsoft Corporation.  All Rights Reserved.
//
//  File:       parse.cpp
//  Content:    Parse D3DX Effect files
//
///////////////////////////////////////////////////////////////////////////

#include "pcheffect.h"


//
// Productions
//

enum
{
    P_EFFECT_BODY0,
    P_EFFECT_BODY1,
    P_DECLARATIONS0,
    P_DECLARATIONS1,
    P_DECLARATION0,
    P_TECHNIQUES0,
    P_TECHNIQUES1,
    P_TECHNIQUE0,
    P_TECHNIQUE1,
    P_TECHNIQUE_BODY0,
    P_TECHNIQUE_BODY1,
    P_TECHNIQUE_BODY2,
    P_TECHNIQUE_BODY3,
    P_TECHNIQUE_BODY4,
    P_TECHNIQUE_BODY5,
    P_TECHNIQUE_BODY6,
    P_TECHNIQUE_BODY7,
    P_PASSES0,
    P_PASSES1,
    P_PASS0,
    P_PASS1,
    P_PASS_BODY0,
    P_PASS_BODY1,
    P_PASS_BODY2,
    P_PASS_BODY3,
    P_STAGES0,
    P_STAGES1,
    P_STAGE0,
    P_STAGE1,
    P_STAGE2,
    P_STAGE_BODY0,
    P_STAGE_BODY1,
    P_SHADERS0,
    P_SHADERS1,
    P_SHADERS2,
    P_VSHADER0,
    P_VSHADER_BODY0,
    P_VSHADER_BODY1,
    P_PSHADER0,
    P_PSHADER_BODY0,
    P_PSHADER_BODY1,
    P_ASSIGNMENTS0,
    P_ASSIGNMENTS1,
    P_ASSIGNMENT0,
    P_EXPRESSION0,
    P_EXPRESSION1,
    P_VALUE0,
    P_VALUE1,
    P_VALUE2,
    P_VALUE3,
    P_VALUE4,
    P_VECTOR0,
    P_VECTOR1,
    P_VECTOR2,
    P_VECTOR3,
    P_NUM0,
    P_NUM1,
    P_INT0,
    P_HEX0,
    P_FLOAT0,
    P_ID0,
    P_ASM0
};

#define Prod(n) \
    CD3DXEffectParser::s_pParser->Production(n)

#define d3dxeffect_lex \
    CD3DXEffectParser::s_pParser->Token

#define d3dxeffect_error \
    CD3DXEffectParser::s_pParser->Error

#define lint

#include "effect_y.h"


//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////


#define BIND_END() \
    { NULL, 0, NULL }


static Binding g_BindDecl[] =
{
    { "DWORD",      0,  NULL },
    { "FLOAT",      1,  NULL },
    { "COLOR",      2,  NULL },
    { "VECTOR",     3,  NULL },
    { "MATRIX",     4,  NULL },
    { "TEXTURE",    5,  NULL },
    BIND_END()
};


static Binding g_BindBool[] =
{
    { "FALSE",  0, NULL },
    { "TRUE",   1, NULL },
    BIND_END()
};


static Binding g_BindSHADE[] =
{
    { "FLAT",       D3DSHADE_FLAT,      NULL },
    { "GOURAUD",    D3DSHADE_GOURAUD,   NULL },
    { "PHONG",      D3DSHADE_PHONG,     NULL },
    BIND_END()
};

static Binding g_BindFILL[] =
{
    { "POINT",      D3DFILL_POINT,      NULL },
    { "WIREFRAME",  D3DFILL_WIREFRAME,  NULL },
    { "SOLID",      D3DFILL_SOLID,      NULL },
    BIND_END()
};

static Binding g_BindBLEND[] =
{
    { "ZERO",               D3DBLEND_ZERO,              NULL },
    { "ONE",                D3DBLEND_ONE,               NULL },
    { "SRCCOLOR",           D3DBLEND_SRCCOLOR,          NULL },
    { "INVSRCCOLOR",        D3DBLEND_INVSRCCOLOR,       NULL },
    { "SRCALPHA",           D3DBLEND_SRCALPHA,          NULL },
    { "INVSRCALPHA",        D3DBLEND_INVSRCALPHA,       NULL },
    { "DESTALPHA",          D3DBLEND_DESTALPHA,         NULL },
    { "INVDESTALPHA",       D3DBLEND_INVDESTALPHA,      NULL },
    { "DESTCOLOR",          D3DBLEND_DESTCOLOR,         NULL },
    { "INVDESTCOLOR",       D3DBLEND_INVDESTCOLOR,      NULL },
    { "SRCALPHASAT",        D3DBLEND_SRCALPHASAT,       NULL },
    { "BOTHSRCALPHA",       D3DBLEND_BOTHSRCALPHA,      NULL },
    { "BOTHINVSRCALPHA",    D3DBLEND_BOTHINVSRCALPHA,   NULL },
    BIND_END()
};

static Binding g_BindTADDRESS[] =
{
    { "WRAP",   D3DTADDRESS_WRAP,   NULL },
    { "MIRROR", D3DTADDRESS_MIRROR, NULL },
    { "CLAMP",  D3DTADDRESS_CLAMP,  NULL },
    { "BORDER", D3DTADDRESS_BORDER, NULL },
    BIND_END()
};

static Binding g_BindCULL[] =
{
    { "NONE",   D3DCULL_NONE,   NULL },
    { "CW",     D3DCULL_CW,     NULL },
    { "CCW",    D3DCULL_CCW,    NULL },
    BIND_END()
};

static Binding g_BindCMP[] =
{
    { "NEVER",          D3DCMP_NEVER,           NULL },
    { "LESS",           D3DCMP_LESS,            NULL },
    { "EQUAL",          D3DCMP_EQUAL,           NULL },
    { "LESSEQUAL",      D3DCMP_LESSEQUAL,       NULL },
    { "GREATER",        D3DCMP_GREATER,         NULL },
    { "NOTEQUAL",       D3DCMP_NOTEQUAL,        NULL },
    { "GREATEREQUAL",   D3DCMP_GREATEREQUAL,    NULL },
    { "ALWAYS",         D3DCMP_ALWAYS,          NULL },
    BIND_END()
};

static Binding g_BindSTENCILOP[] =
{
    { "KEEP",       D3DSTENCILOP_KEEP,      NULL },
    { "ZERO",       D3DSTENCILOP_ZERO,      NULL },
    { "REPLACE",    D3DSTENCILOP_REPLACE,   NULL },
    { "INCRSAT",    D3DSTENCILOP_INCRSAT,   NULL },
    { "DECRSAT",    D3DSTENCILOP_DECRSAT,   NULL },
    { "INVERT",     D3DSTENCILOP_INVERT,    NULL },
    { "INCR",       D3DSTENCILOP_INCR,      NULL },
    { "DECR",       D3DSTENCILOP_DECR,      NULL },
    BIND_END()
};


static Binding g_BindFOG[] =
{
    { "NONE",   D3DFOG_NONE,    NULL },
    { "EXP",    D3DFOG_EXP,     NULL },
    { "EXP2",   D3DFOG_EXP2,    NULL },
    { "LINEAR", D3DFOG_LINEAR,  NULL },
    BIND_END()
};


static Binding g_BindZB[] =
{
    { "TRUE",   D3DZB_TRUE,     NULL },
    { "FALSE",  D3DZB_FALSE,    NULL },
    { "USEW",   D3DZB_USEW,     NULL },
    BIND_END()
};


static Binding g_BindMCS[] =
{
    { "MATERIAL",   D3DMCS_MATERIAL,    NULL },
    { "COLOR1",     D3DMCS_COLOR1,      NULL },
    { "COLOR2",     D3DMCS_COLOR2,  NULL },
    BIND_END()
};


static Binding g_BindWRAP[] =
{
    { "U",      D3DWRAP_U,      NULL },
    { "V",      D3DWRAP_V,      NULL },
    { "W",      D3DWRAP_W,      NULL },
    { "COORD0", D3DWRAPCOORD_0, NULL },
    { "COORD1", D3DWRAPCOORD_1, NULL },
    { "COORD2", D3DWRAPCOORD_2, NULL },
    { "COORD3", D3DWRAPCOORD_3, NULL },
    BIND_END()
};


static Binding g_BindCOLORWRITEENABLE[] =
{
    { "RED",    D3DCOLORWRITEENABLE_RED,    NULL },
    { "GREEN",  D3DCOLORWRITEENABLE_GREEN,  NULL },
    { "BLUE",   D3DCOLORWRITEENABLE_BLUE,   NULL },
    { "ALPHA",  D3DCOLORWRITEENABLE_ALPHA,  NULL },
    BIND_END()
};


static Binding g_BindHOB[] =
{
    { "BEZIER",         D3DBASIS_BEZIER,      NULL },
    { "BSPLINE",        D3DBASIS_BSPLINE,     NULL },
    { "INTERPOLATE",    D3DBASIS_INTERPOLATE, NULL },
    BIND_END()
};


static Binding g_BindORDER[] =
{
    { "LINEAR", D3DORDER_LINEAR,    NULL },
    { "CUBIC",  D3DORDER_CUBIC,     NULL },
    BIND_END()
};


static Binding g_BindVBF[] =
{
    { "DISABLE",    D3DVBF_DISABLE,  NULL },
    { "0WEIGHTS",   D3DVBF_0WEIGHTS, NULL },
    { "1WEIGHTS",   D3DVBF_1WEIGHTS, NULL },
    { "2WEIGHTS",   D3DVBF_2WEIGHTS, NULL },
    { "3WEIGHTS",   D3DVBF_3WEIGHTS, NULL },
    BIND_END()
};


static Binding g_BindTSS_TCI[] =
{
    { "PASSTHRU",                       D3DTSS_TCI_PASSTHRU,                    NULL },
    { "CAMERASPACENORMAL",              D3DTSS_TCI_CAMERASPACENORMAL,           NULL },
    { "CAMERASPACEPOSITION",            D3DTSS_TCI_CAMERASPACEPOSITION,         NULL },
    { "CAMERASPACEREFLECTIONVECTOR",    D3DTSS_TCI_CAMERASPACEREFLECTIONVECTOR, NULL },
    BIND_END()
};


static Binding g_BindTOP[] =
{
    { "DISABLE",                    D3DTOP_DISABLE,                     NULL },
    { "SELECTARG1",                 D3DTOP_SELECTARG1,                  NULL },
    { "SELECTARG2",                 D3DTOP_SELECTARG2,                  NULL },
    { "MODULATE",                   D3DTOP_MODULATE,                    NULL },
    { "MODULATE2X",                 D3DTOP_MODULATE2X,                  NULL },
    { "MODULATE4X",                 D3DTOP_MODULATE4X,                  NULL },
    { "ADD",                        D3DTOP_ADD,                         NULL },
    { "ADDSIGNED",                  D3DTOP_ADDSIGNED,                   NULL },
    { "ADDSIGNED2X",                D3DTOP_ADDSIGNED2X,                 NULL },
    { "SUBTRACT",                   D3DTOP_SUBTRACT,                    NULL },
    { "ADDSMOOTH",                  D3DTOP_ADDSMOOTH,                   NULL },
    { "BLENDDIFFUSEALPHA",          D3DTOP_BLENDDIFFUSEALPHA,           NULL },
    { "BLENDTEXTUREALPHA",          D3DTOP_BLENDTEXTUREALPHA,           NULL },
    { "BLENDFACTORALPHA",           D3DTOP_BLENDFACTORALPHA,            NULL },
    { "BLENDTEXTUREALPHAPM",        D3DTOP_BLENDTEXTUREALPHAPM,         NULL },
    { "BLENDCURRENTALPHA",          D3DTOP_BLENDCURRENTALPHA,           NULL },
    { "PREMODULATE",                D3DTOP_PREMODULATE,                 NULL },
    { "MODULATEALPHA_ADDCOLOR",     D3DTOP_MODULATEALPHA_ADDCOLOR,      NULL },
    { "MODULATECOLOR_ADDALPHA",     D3DTOP_MODULATECOLOR_ADDALPHA,      NULL },
    { "MODULATEINVALPHA_ADDCOLOR",  D3DTOP_MODULATEINVALPHA_ADDCOLOR,   NULL },
    { "MODULATEINVCOLOR_ADDALPHA",  D3DTOP_MODULATEINVCOLOR_ADDALPHA,   NULL },
    { "BUMPENVMAP",                 D3DTOP_BUMPENVMAP,                  NULL },
    { "BUMPENVMAPLUMINANCE",        D3DTOP_BUMPENVMAPLUMINANCE,         NULL },
    { "DOTPRODUCT3",                D3DTOP_DOTPRODUCT3,                 NULL },
    BIND_END()
};


static Binding g_BindTA[] =
{
    { "SELECTMASK",     D3DTA_SELECTMASK,       NULL },
    { "DIFFUSE",        D3DTA_DIFFUSE,          NULL },
    { "CURRENT",        D3DTA_CURRENT,          NULL },
    { "TEXTURE",        D3DTA_TEXTURE,          NULL },
    { "TFACTOR",        D3DTA_TFACTOR,          NULL },
    { "SPECULAR",       D3DTA_SPECULAR,         NULL },
    { "COMPLEMENT",     D3DTA_COMPLEMENT,       NULL },
    { "ALPHAREPLICATE", D3DTA_ALPHAREPLICATE,   NULL },
    BIND_END()
};


static Binding g_BindTEXF[] =
{
    { "NONE",           D3DTEXF_NONE,           NULL },
    { "POINT",          D3DTEXF_POINT,          NULL },
    { "LINEAR",         D3DTEXF_LINEAR,         NULL },
    { "ANISOTROPIC",    D3DTEXF_ANISOTROPIC,    NULL },
    { "FLATCUBIC",      D3DTEXF_QUINCUNX,       NULL },
    { "GAUSSIANCUBIC",  D3DTEXF_GAUSSIANCUBIC,  NULL },
    BIND_END()
};


static Binding g_BindTTFF[] =
{
    { "FALSE",      D3DTTFF_DISABLE,    NULL },
    { "DISABLE",    D3DTTFF_DISABLE,    NULL },
    { "COUNT1",     D3DTTFF_COUNT1,     NULL },
    { "COUNT2",     D3DTTFF_COUNT2,     NULL },
    { "COUNT3",     D3DTTFF_COUNT3,     NULL },
    { "COUNT4",     D3DTTFF_COUNT4,     NULL },
    { "PROJECTED",  D3DTTFF_PROJECTED,  NULL },
    BIND_END()
};


static Binding g_BindFVF[] =
{
    { "XYZ",        D3DFVF_XYZ,         NULL },
    { "XYZRHW",     D3DFVF_XYZRHW,      NULL },
    { "XYZB1",      D3DFVF_XYZB1,       NULL },
    { "XYZB2",      D3DFVF_XYZB2,       NULL },
    { "XYZB3",      D3DFVF_XYZB3,       NULL },
    { "XYZB4",      D3DFVF_XYZB4,       NULL },
    { "XYZB5",      D3DFVF_XYZB5,       NULL },
    { "NORMAL",     D3DFVF_NORMAL,      NULL },
    { "PSIZE",      D3DFVF_PSIZE,       NULL },
    { "DIFFUSE",    D3DFVF_DIFFUSE,     NULL },
    { "SPECULAR",   D3DFVF_SPECULAR,    NULL },
    { "TEX0",       D3DFVF_TEX0,        NULL },
    { "TEX1",       D3DFVF_TEX1,        NULL },
    { "TEX2",       D3DFVF_TEX2,        NULL },
    { "TEX3",       D3DFVF_TEX3,        NULL },
    { "TEX4",       D3DFVF_TEX4,        NULL },
    { "TEX5",       D3DFVF_TEX5,        NULL },
    { "TEX6",       D3DFVF_TEX6,        NULL },
    { "TEX7",       D3DFVF_TEX7,        NULL },
    { "TEX8",       D3DFVF_TEX8,        NULL },
    { "VERTEX",     D3DFVF_VERTEX,      NULL },
    { "TLVERTEX",   D3DFVF_TLVERTEX,    NULL },
    BIND_END()
};


static Binding g_BindRS[] =
{
    { "ZENABLE",                    D3DRS_ZENABLE,                  g_BindZB },
    { "FILLMODE",                   D3DRS_FILLMODE,                 g_BindFILL },
    { "SHADEMODE",                  D3DRS_SHADEMODE,                g_BindSHADE },
    { "LINEPATTERN",                D3DRS_LINEPATTERN,              NULL },
    { "ZWRITEENABLE",               D3DRS_ZWRITEENABLE,             g_BindBool },
    { "ALPHATESTENABLE",            D3DRS_ALPHATESTENABLE,          g_BindBool },
    { "LASTPIXEL",                  D3DRS_LASTPIXEL,                g_BindBool },
    { "SRCBLEND",                   D3DRS_SRCBLEND,                 g_BindBLEND },
    { "DESTBLEND",                  D3DRS_DESTBLEND,                g_BindBLEND },
    { "CULLMODE",                   D3DRS_CULLMODE,                 g_BindCULL },
    { "ZFUNC",                      D3DRS_ZFUNC,                    g_BindCMP },
    { "ALPHAREF",                   D3DRS_ALPHAREF,                 NULL },
    { "ALPHAFUNC",                  D3DRS_ALPHAFUNC,                g_BindCMP },
    { "DITHERENABLE",               D3DRS_DITHERENABLE,             g_BindBool },
    { "ALPHABLENDENABLE",           D3DRS_ALPHABLENDENABLE,         g_BindBool },
    { "FOGENABLE",                  D3DRS_FOGENABLE,                g_BindBool },
    { "SPECULARENABLE",             D3DRS_SPECULARENABLE,           g_BindBool },
    { "ZVISIBLE",                   D3DRS_ZVISIBLE,                 g_BindBool },
    { "FOGCOLOR",                   D3DRS_FOGCOLOR,                 NULL },
    { "FOGTABLEMODE",               D3DRS_FOGTABLEMODE,             g_BindFOG },
    { "FOGSTART",                   D3DRS_FOGSTART,                 NULL },
    { "FOGEND",                     D3DRS_FOGEND,                   NULL },
    { "FOGDENSITY",                 D3DRS_FOGDENSITY,               NULL },
    { "EDGEANTIALIAS",              D3DRS_EDGEANTIALIAS,            g_BindBool },
    { "ZBIAS",                      D3DRS_ZBIAS,                    NULL },
    { "RANGEFOGENABLE",             D3DRS_RANGEFOGENABLE,           NULL },
    { "STENCILENABLE",              D3DRS_STENCILENABLE,            g_BindBool },
    { "STENCILFAIL",                D3DRS_STENCILFAIL,              g_BindSTENCILOP },
    { "STENCILZFAIL",               D3DRS_STENCILZFAIL,             g_BindSTENCILOP },
    { "STENCILPASS",                D3DRS_STENCILPASS,              g_BindSTENCILOP },
    { "STENCILFUNC",                D3DRS_STENCILFUNC,              g_BindCMP },
    { "STENCILREF",                 D3DRS_STENCILREF,               NULL },
    { "STENCILMASK",                D3DRS_STENCILMASK,              NULL },
    { "STENCILWRITEMASK",           D3DRS_STENCILWRITEMASK,         NULL },
    { "TEXTUREFACTOR",              D3DRS_TEXTUREFACTOR,            NULL },
    { "WRAP0",                      D3DRS_WRAP0,                    g_BindWRAP },
    { "WRAP1",                      D3DRS_WRAP1,                    g_BindWRAP },
    { "WRAP2",                      D3DRS_WRAP2,                    g_BindWRAP },
    { "WRAP3",                      D3DRS_WRAP3,                    g_BindWRAP },
    { "WRAP4",                      D3DRS_WRAP4,                    g_BindWRAP },
    { "WRAP5",                      D3DRS_WRAP5,                    g_BindWRAP },
    { "WRAP6",                      D3DRS_WRAP6,                    g_BindWRAP },
    { "WRAP7",                      D3DRS_WRAP7,                    g_BindWRAP },
    { "CLIPPING",                   D3DRS_CLIPPING,                 g_BindBool },
    { "LIGHTING",                   D3DRS_LIGHTING,                 g_BindBool },
    { "AMBIENT",                    D3DRS_AMBIENT,                  NULL },
    { "FOGVERTEXMODE",              D3DRS_FOGVERTEXMODE,            NULL },
    { "COLORVERTEX",                D3DRS_COLORVERTEX,              g_BindBool },
    { "LOCALVIEWER",                D3DRS_LOCALVIEWER,              g_BindBool },
    { "NORMALIZENORMALS",           D3DRS_NORMALIZENORMALS,         g_BindBool },
    { "DIFFUSEMATERIALSOURCE",      D3DRS_DIFFUSEMATERIALSOURCE,    g_BindMCS },
    { "SPECULARMATERIALSOURCE",     D3DRS_SPECULARMATERIALSOURCE,   g_BindMCS },
    { "AMBIENTMATERIALSOURCE",      D3DRS_AMBIENTMATERIALSOURCE,    g_BindMCS },
    { "EMISSIVEMATERIALSOURCE",     D3DRS_EMISSIVEMATERIALSOURCE,   g_BindMCS },
    { "VERTEXBLEND",                D3DRS_VERTEXBLEND,              g_BindVBF },
    { "CLIPPLANEENABLE",            D3DRS_CLIPPLANEENABLE,          g_BindBool },
    { "SOFTWAREVERTEXPROCESSING",   D3DRS_SOFTWAREVERTEXPROCESSING, g_BindBool },
    { "POINTSIZE",                  D3DRS_POINTSIZE,                NULL },
    { "POINTSIZE_MIN",              D3DRS_POINTSIZE_MIN,            NULL },
    { "POINTSIZE_MAX",              D3DRS_POINTSIZE_MAX,            NULL },
    { "POINTSPRITEENABLE",          D3DRS_POINTSPRITEENABLE,        g_BindBool },
    { "POINTSCALEENABLE",           D3DRS_POINTSCALEENABLE,         g_BindBool },
    { "POINTSCALE_A",               D3DRS_POINTSCALE_A,             NULL },
    { "POINTSCALE_B",               D3DRS_POINTSCALE_B,             NULL },
    { "POINTSCALE_C",               D3DRS_POINTSCALE_C,             NULL },
    { "MULTISAMPLEANTIALIAS",       D3DRS_MULTISAMPLEANTIALIAS,     g_BindBool },
    { "MULTISAMPLEMASK",            D3DRS_MULTISAMPLEMASK,          NULL },
    { "PATCHSEGMENTS",              D3DRS_PATCHSEGMENTS,            NULL },
    { "DEBUGMONITORTOKEN",          D3DRS_DEBUGMONITORTOKEN,        NULL },
    { "INDEXEDVERTEXBLENDENABLE",   D3DRS_INDEXEDVERTEXBLENDENABLE, g_BindBool },
    { "COLORWRITEENABLE",           D3DRS_COLORWRITEENABLE,         g_BindBool },
    { "FVF",                        D3DRS_FVF,                      g_BindFVF },
    BIND_END()
};


static Binding g_BindTSS[] =
{
    { "COLOROP",                D3DTSS_COLOROP,                 g_BindTOP },
    { "COLORARG1",              D3DTSS_COLORARG1,               g_BindTA },
    { "COLORARG2",              D3DTSS_COLORARG2,               g_BindTA },
    { "ALPHAOP",                D3DTSS_ALPHAOP,                 g_BindTOP },
    { "ALPHAARG1",              D3DTSS_ALPHAARG1,               g_BindTA },
    { "ALPHAARG2",              D3DTSS_ALPHAARG2,               g_BindTA },
    { "BUMPENVMAT00",           D3DTSS_BUMPENVMAT00,            NULL },
    { "BUMPENVMAT01",           D3DTSS_BUMPENVMAT01,            NULL },
    { "BUMPENVMAT10",           D3DTSS_BUMPENVMAT10,            NULL },
    { "BUMPENVMAT11",           D3DTSS_BUMPENVMAT11,            NULL },
    { "TEXCOORDINDEX",          D3DTSS_TEXCOORDINDEX,           g_BindTSS_TCI },
    { "ADDRESS",                D3DTSS_ADDRESS,                 g_BindTADDRESS },
    { "ADDRESSU",               D3DTSS_ADDRESSU,                g_BindTADDRESS },
    { "ADDRESSV",               D3DTSS_ADDRESSV,                g_BindTADDRESS },
    { "ADDRESSW",               D3DTSS_ADDRESSW,                g_BindTADDRESS },
    { "BORDERCOLOR",            D3DTSS_BORDERCOLOR,             NULL },
    { "MAGFILTER",              D3DTSS_MAGFILTER,               g_BindTEXF },
    { "MINFILTER",              D3DTSS_MINFILTER,               g_BindTEXF },
    { "MIPFILTER",              D3DTSS_MIPFILTER,               g_BindTEXF },
    { "MIPMAPLODBIAS",          D3DTSS_MIPMAPLODBIAS,           NULL },
    { "MAXMIPLEVEL",            D3DTSS_MAXMIPLEVEL,             NULL },
    { "MAXANISOTROPY",          D3DTSS_MAXANISOTROPY,           NULL },
    { "BUMPENVLSCALE",          D3DTSS_BUMPENVLSCALE,           NULL },
    { "BUMPENVLOFFSET",         D3DTSS_BUMPENVLOFFSET,          NULL },
    { "TEXTURETRANSFORMFLAGS",  D3DTSS_TEXTURETRANSFORMFLAGS,   g_BindTTFF },
    BIND_END()
};


//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

Node::Node(DWORD dwType)
{
    m_dwType = dwType;
    m_pNext = NULL;
    m_pStack = NULL;
}

Node::~Node()
{
    delete m_pNext;
    delete m_pStack;
}

EffectNode::EffectNode() : Node(NODE_EFFECT)
{
    m_pDeclarations = NULL;
    m_pTechniques = NULL;
}

EffectNode::~EffectNode()
{
    delete m_pDeclarations;
    delete m_pTechniques;
}


DeclarationNode::DeclarationNode() : Node(NODE_DECLARATION)
{
    m_pType = NULL;
    m_pName = NULL;
}

DeclarationNode::~DeclarationNode()
{
    delete m_pType;
    delete m_pName;
}


TechniqueNode::TechniqueNode() : Node(NODE_TECHNIQUE)
{
    m_pLOD    = NULL;
    m_pStates = NULL;
    m_pPasses = NULL;
}

TechniqueNode::~TechniqueNode()
{
    delete m_pLOD;
    delete m_pStates;
    delete m_pPasses;
}


PassNode::PassNode() : Node(NODE_PASS)
{
    m_pSrcBlend = NULL;
    m_pDstBlend = NULL;
    m_pStates   = NULL;
    m_pStages   = NULL;
    m_pShaders  = NULL;
}

PassNode::~PassNode()
{
    delete m_pSrcBlend;
    delete m_pDstBlend;
    delete m_pStates;
    delete m_pStages;
}


StageNode::StageNode() : Node(NODE_STAGE)
{
    m_pTexture   = NULL;
    m_pMatrix    = NULL;
    m_pStates    = NULL;
}

StageNode::~StageNode()
{
    delete m_pTexture;
    delete m_pMatrix;
    delete m_pStates;
}


VShaderNode::VShaderNode() : Node(NODE_VSHADER)
{
    m_pConst = NULL;
    m_pAsm   = NULL;
}

VShaderNode::~VShaderNode()
{
    delete m_pConst;
    delete m_pAsm;
}


PShaderNode::PShaderNode() : Node(NODE_PSHADER)
{
    m_pConst = NULL;
    m_pAsm   = NULL;
}

PShaderNode::~PShaderNode()
{
    delete m_pConst;
    delete m_pAsm;
}


StateNode::StateNode() : Node(NODE_STATE)
{
    m_pLValue = NULL;
    m_pRValue = NULL;
}

StateNode::~StateNode()
{
    delete m_pLValue;
    delete m_pRValue;
}


IdNode::IdNode(char *sz) : Node(NODE_ID)
{
    m_szId = _strdup(sz);
}

IdNode::~IdNode()
{
    free(m_szId);
}


DwordNode::DwordNode(DWORD dwValue) : Node(NODE_DWORD)
{
    m_dwValue = dwValue;
}


FloatNode::FloatNode(float fValue) : Node(NODE_FLOAT)
{
    m_fValue = fValue;
}


VectorNode::VectorNode(float x, float y, float z, float w) : Node(NODE_VECTOR)
{
    m_vec.x = x;
    m_vec.y = y;
    m_vec.z = z;
    m_vec.w = w;
}


AsmNode::AsmNode(char *pchAsm, UINT cchAsm) : Node(NODE_ASM)
{
    m_pchAsm = pchAsm;
    m_cchAsm = cchAsm;
}



//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

CD3DXEffectParser* CD3DXEffectParser::s_pParser = NULL;

CD3DXEffectParser::CD3DXEffectParser()
{
    m_pNode = NULL;
    m_bFail = FALSE;
    m_cErrors = 0;
    m_cbErrors = 0;

    m_pchSource = NULL;
    m_pchLine   = NULL;
    m_pch       = NULL;
    m_pchLim    = NULL;

    m_iLine = 1;

    m_pEffectNode = NULL;
    m_pEffectData = NULL;

    s_pParser = this;
}


CD3DXEffectParser::~CD3DXEffectParser()
{
    if(m_pNode)
        delete m_pNode;

    if(m_pEffectData)
        delete m_pEffectData;
}


void
CD3DXEffectParser::Push(Node *pNode)
{
    if(!pNode)
    {
        m_bFail = TRUE;
        return;
    }

    pNode->m_pStack = m_pNode;
    m_pNode = pNode;
    return;
}


Node *
CD3DXEffectParser::Pop()
{
    if(!m_pNode)
    {
        m_bFail = TRUE;
        return NULL;
    }

    Node *pNode = m_pNode;
    m_pNode = pNode->m_pStack;
    pNode->m_pStack = NULL;
    return pNode;
}


void
CD3DXEffectParser::Combine()
{
    if(!m_pNode || !m_pNode->m_pStack || m_pNode->m_pStack->m_pNext)
    {
        m_bFail = TRUE;
        return;
    }

    Node *pNode = m_pNode;
    m_pNode->m_pStack->m_pNext = pNode;
    m_pNode = m_pNode->m_pStack;
    pNode->m_pStack = NULL;
    return;
}


//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////


int
CD3DXEffectParser::Token()
{
    char *pch;
    int nToken;
    SIZE_T cch;

    BOOL bAsm = FALSE;
    char *pchAsm = NULL;

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
        else if(bAsm)
        {
            // Asm block
            if(ch == '{')
            {
                pchAsm = m_pch++;
            }
            else if(ch == '}')
            {
                m_pchAsm = pchAsm;
                m_cchAsm = (UINT)(m_pchAsm - m_pch);

                m_pch++;

                bAsm = FALSE;
                pchAsm = NULL;
            }
            else if(ch == ';')
            {
                // comment
                for(m_pch++; m_pch < m_pchLim && *m_pch != '\n'; m_pch++);
            }
            else if(!pchAsm)
            {
                // failure!
                m_pch = m_pchLim;
            }
            else
            {
                m_pch++;
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

            if(!_stricmp(m_szTok, "technique"))    return T_TECHNIQUE;
            if(!_stricmp(m_szTok, "pass"))         return T_PASS;
            if(!_stricmp(m_szTok, "stage"))        return T_STAGE;
            if(!_stricmp(m_szTok, "vertexshader")) return T_VSHADER;
            if(!_stricmp(m_szTok, "pixelshader"))  return T_PSHADER;

            if(!_stricmp(m_szTok, "asm"))
            {
                bAsm = TRUE;
                pchAsm = NULL;
                continue;
            }

            return T_ID;
        }
        else if(ch == '0' && m_pch + 1 < m_pchLim && m_pch[1] == 'x')
        {
            // hex
            m_pch += 2;
            pch = m_pch;

            for(; m_pch < m_pchLim && (isalpha(*m_pch) || tolower(*m_pch) >= 'a' && tolower(*m_pch) <= 'f'); m_pch++);

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
            nToken = T_INT;

            for(m_pch++; m_pch < m_pchLim; m_pch++)
            {
                if(isdigit(*m_pch))
                    continue;

                if(*m_pch == '.')
                {
                    nToken = T_FLOAT;
                    continue;
                }

                break;
            }

            cch = m_pch - pch;

            if(cch >= sizeof(m_szTok))
                cch = sizeof(m_szTok) - 1;

            memcpy(m_szTok, pch, cch);
            m_szTok[cch] = 0;

            if(m_pch < m_pchLim && *m_pch == 'f')
            {
                nToken = T_FLOAT;
                m_pch++;
            }

            return nToken;
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
CD3DXEffectParser::Production(UINT nProd)
{
    DWORD dw;
    float f;

    EffectNode      *pEffectNode;
    DeclarationNode *pDeclarationNode;
    TechniqueNode   *pTechniqueNode;
    PassNode        *pPassNode;
    VShaderNode     *pVShaderNode;
    PShaderNode     *pPShaderNode;
    StageNode       *pStageNode;
    StateNode       *pStateNode;
    DwordNode       *pDwordNode;
    FloatNode       *pFloatNodeA, *pFloatNodeB, *pFloatNodeC, *pFloatNodeD;


    if(m_bFail)
        return;


    switch(nProd)
    {
    case P_EFFECT_BODY0:
        if(!(pEffectNode = new EffectNode()))
            goto LFail;

        pEffectNode->m_pTechniques = Pop();
        Push(pEffectNode);
        break;

    case P_EFFECT_BODY1:
        if(!(pEffectNode = new EffectNode()))
            goto LFail;

        pEffectNode->m_pTechniques = Pop();
        pEffectNode->m_pDeclarations = Pop();
        Push(pEffectNode);
        break;

    case P_DECLARATIONS0:
        break;

    case P_DECLARATIONS1:
        Combine();
        break;


    case P_DECLARATION0:
        if(!(pDeclarationNode = new DeclarationNode()))
            goto LFail;

        pDeclarationNode->m_pName = Pop();
        pDeclarationNode->m_pType = Pop();

        Push(pDeclarationNode);
        break;

    case P_TECHNIQUES0:
        break;

    case P_TECHNIQUES1:
        Combine();
        break;


    case P_TECHNIQUE0:
        break;

    case P_TECHNIQUE1:
        pTechniqueNode = (TechniqueNode *) Pop();
        pTechniqueNode->m_pLOD  = Pop();
        Push(pTechniqueNode);
        break;


    case P_TECHNIQUE_BODY0:
        if(!(pTechniqueNode = new TechniqueNode()))
            goto LFail;

        Push(pTechniqueNode);
        break;

    case P_TECHNIQUE_BODY1:
        if(!(pTechniqueNode = new TechniqueNode()))
            goto LFail;

        pTechniqueNode->m_pStates = Pop();
        Push(pTechniqueNode);
        break;

    case P_TECHNIQUE_BODY2:
        if(!(pTechniqueNode = new TechniqueNode()))
            goto LFail;

        pTechniqueNode->m_pPasses = Pop();
        Push(pTechniqueNode);
        break;

    case P_TECHNIQUE_BODY3:
        if(!(pTechniqueNode = new TechniqueNode()))
            goto LFail;

        pTechniqueNode->m_pShaders = Pop();
        Push(pTechniqueNode);
        break;

    case P_TECHNIQUE_BODY4:
        if(!(pTechniqueNode = new TechniqueNode()))
            goto LFail;

        pTechniqueNode->m_pPasses = Pop();
        pTechniqueNode->m_pStates = Pop();
        Push(pTechniqueNode);
        break;

    case P_TECHNIQUE_BODY5:
        if(!(pTechniqueNode = new TechniqueNode()))
            goto LFail;

        pTechniqueNode->m_pShaders = Pop();
        pTechniqueNode->m_pStates = Pop();
        Push(pTechniqueNode);
        break;

    case P_TECHNIQUE_BODY6:
        if(!(pTechniqueNode = new TechniqueNode()))
            goto LFail;

        pTechniqueNode->m_pShaders = Pop();
        pTechniqueNode->m_pPasses = Pop();
        Push(pTechniqueNode);
        break;

    case P_TECHNIQUE_BODY7:
        if(!(pTechniqueNode = new TechniqueNode()))
            goto LFail;

        pTechniqueNode->m_pShaders = Pop();
        pTechniqueNode->m_pPasses = Pop();
        pTechniqueNode->m_pStates = Pop();
        Push(pTechniqueNode);
        break;


    case P_PASSES0:
        break;

    case P_PASSES1:
        Combine();
        break;


    case P_PASS0:
        break;

    case P_PASS1:
        pPassNode = (PassNode *) Pop();
        pPassNode->m_pDstBlend = Pop();
        pPassNode->m_pSrcBlend = Pop();
        Push(pPassNode);
        break;


    case P_PASS_BODY0:
        if(!(pPassNode = new PassNode()))
            goto LFail;

        Push(pPassNode);
        break;

    case P_PASS_BODY1:
        if(!(pPassNode = new PassNode()))
            goto LFail;

        pPassNode->m_pStates = Pop();
        Push(pPassNode);
        break;

    case P_PASS_BODY2:
        if(!(pPassNode = new PassNode()))
            goto LFail;

        pPassNode->m_pStages = Pop();
        Push(pPassNode);
        break;

    case P_PASS_BODY3:
        if(!(pPassNode = new PassNode()))
            goto LFail;

        pPassNode->m_pStages = Pop();
        pPassNode->m_pStates = Pop();
        Push(pPassNode);
        break;


    case P_STAGES0:
        break;

    case P_STAGES1:
        Combine();
        break;


    case P_STAGE0:
        break;

    case P_STAGE1:
        pStageNode = (StageNode *) Pop();
        pStageNode->m_pTexture = Pop();
        Push(pStageNode);
        break;

    case P_STAGE2:
        pStageNode = (StageNode *) Pop();
        pStageNode->m_pMatrix = Pop();
        pStageNode->m_pTexture = Pop();
        Push(pStageNode);
        break;


    case P_STAGE_BODY0:
        if(!(pStageNode = new StageNode()))
            goto LFail;

        Push(pStageNode);
        break;

    case P_STAGE_BODY1:
        if(!(pStageNode = new StageNode()))
            goto LFail;

        pStageNode->m_pStates = Pop();
        Push(pStageNode);
        break;


    case P_SHADERS0:
    case P_SHADERS1:
        break;

    case P_SHADERS2:
        Combine();
        break;


    case P_VSHADER0:
        break;

    case P_VSHADER_BODY0:
        if(!(pVShaderNode = new VShaderNode()))
            goto LFail;

        pVShaderNode->m_pAsm = Pop();
        Push(pVShaderNode);
        break;

    case P_VSHADER_BODY1:
        if(!(pVShaderNode = new VShaderNode()))
            goto LFail;

        pVShaderNode->m_pAsm = Pop();
        pVShaderNode->m_pConst = Pop();
        Push(pVShaderNode);
        break;



    case P_PSHADER0:
        break;


    case P_PSHADER_BODY0:
        if(!(pPShaderNode = new PShaderNode()))
            goto LFail;

        pPShaderNode->m_pAsm = Pop();
        Push(pPShaderNode);
        break;

    case P_PSHADER_BODY1:
        if(!(pPShaderNode = new PShaderNode()))
            goto LFail;

        pPShaderNode->m_pAsm = Pop();
        pPShaderNode->m_pConst = Pop();
        Push(pPShaderNode);
        break;



    case P_ASSIGNMENTS0:
        break;

    case P_ASSIGNMENTS1:
        Combine();
        break;


    case P_ASSIGNMENT0:
        if(!(pStateNode = new StateNode()))
            goto LFail;

        pStateNode->m_pRValue = Pop();
        pStateNode->m_pLValue = Pop();
        Push(pStateNode);
        break;


    case P_EXPRESSION0:
        break;

    case P_EXPRESSION1:
        Combine();
        break;


    case P_VALUE0:
    case P_VALUE1:
    case P_VALUE2:
    case P_VALUE3:
    case P_VALUE4:
        break;

    case P_VECTOR0:
        pFloatNodeA = (FloatNode *) Pop();
        Push(new VectorNode(pFloatNodeA->m_fValue, 0.0f, 0.0f, 1.0f));
        delete pFloatNodeA;
        break;

    case P_VECTOR1:
        pFloatNodeB = (FloatNode *) Pop();
        pFloatNodeA = (FloatNode *) Pop();

        Push(new VectorNode(pFloatNodeA->m_fValue, pFloatNodeB->m_fValue, 0.0f, 1.0f));

        delete pFloatNodeA;
        delete pFloatNodeB;
        break;

    case P_VECTOR2:
        pFloatNodeC = (FloatNode *) Pop();
        pFloatNodeB = (FloatNode *) Pop();
        pFloatNodeA = (FloatNode *) Pop();

        Push(new VectorNode(pFloatNodeA->m_fValue, pFloatNodeB->m_fValue, pFloatNodeC->m_fValue, 1.0f));

        delete pFloatNodeA;
        delete pFloatNodeB;
        delete pFloatNodeC;
        break;

    case P_VECTOR3:
        pFloatNodeD = (FloatNode *) Pop();
        pFloatNodeC = (FloatNode *) Pop();
        pFloatNodeB = (FloatNode *) Pop();
        pFloatNodeA = (FloatNode *) Pop();

        Push(new VectorNode(pFloatNodeA->m_fValue, pFloatNodeB->m_fValue, pFloatNodeC->m_fValue, pFloatNodeD->m_fValue));

        delete pFloatNodeA;
        delete pFloatNodeB;
        delete pFloatNodeC;
        delete pFloatNodeD;
        break;


    case P_NUM0:
        break;

    case P_NUM1:
        pDwordNode = (DwordNode *) Pop();
        Push(new FloatNode((float) pDwordNode->m_dwValue));
        delete pDwordNode;
        break;

    case P_INT0:
        sscanf(m_szTok, "%d", &dw);
        Push(new DwordNode(dw));
        break;

    case P_HEX0:
        sscanf(m_szTok, "%x", &dw);
        Push(new DwordNode(dw));
        break;


    case P_FLOAT0:
        sscanf(m_szTok, "%f", &f);
        Push(new FloatNode(f));
        break;


    case P_ID0:
        Push(new IdNode(m_szTok));
        break;


    case P_ASM0:
        Push(new AsmNode(m_pchAsm, m_cchAsm));
        break;


    default:
        goto LFail;
    }

    return;

LFail:
    m_bFail = TRUE;
    return;
}



void
CD3DXEffectParser::Error(char *szFormat, ...)
{
    char szA[256];
    char szB[256];

    va_list ap;
    va_start(ap, szFormat);
    _vsnprintf(szA, sizeof(szB), szFormat, ap);
    szA[255] = '\0';
    va_end(ap);

    _snprintf(szB, sizeof(szB), "%s(%d) : %s", m_szFile, m_iLine, szA);
    szB[255] = '\0';

    DPF(0, "%s", szB);
/*
    m_Errors.Push(szB);
    m_cErrors++;
    m_cbErrors += strlen(szB) + 1;
*/
    m_bFail = TRUE;
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////



HRESULT
CD3DXEffectParser::Parse(LPCVOID pv, UINT cb, LPCSTR szFile,
                         LPDIRECT3DDEVICE8 pDevice, LPD3DXEFFECT *ppEffect,
                         LPD3DXBUFFER *ppErrors)
{
    HRESULT hr;
    CD3DXEffect *pEffect;

    if(ppErrors)
        *ppErrors = NULL;

    if(!ppEffect)
        return E_INVALIDARG;

    if(ppEffect)
        *ppEffect = NULL;

    if(!pv)
        return E_INVALIDARG;

    if(!pDevice)
        return E_INVALIDARG;

    if(szFile)
        _snprintf(m_szFile, 256, "%s", szFile);
    else
        m_szFile[0] = 0;


    m_pchSource = (char *) pv;
    m_pch       = m_pchSource;
    m_pchLine   = m_pchSource;
    m_pchLim    = m_pchSource + cb;

    m_bFail    = FALSE;
    m_cErrors  = 0;
    m_cbErrors = 0;
    m_iLine    = 1;


    // Parse
    if(yyparse())
        goto LFail;

    if(FAILED(hr = Compile()))
        goto LDone;

    if(!(pEffect = new CD3DXEffect()))
        goto LOutOfMemory;

    if(FAILED(hr = pEffect->Initialize(pDevice, m_pEffectData)))
    {
        delete pEffect;
        goto LDone;
    }

    *ppEffect = pEffect;
    m_pEffectData = NULL;

    hr = S_OK;
    goto LDone;


LFail:
    hr = E_FAIL;
    goto LDone;

LOutOfMemory:
    hr = E_OUTOFMEMORY;
    goto LDone;

LDone:
    if(m_pNode)
        delete m_pNode;

    if(m_pEffectData)
        delete m_pEffectData;

    m_pNode = NULL;
    m_pEffectNode = NULL;
    m_pEffectData = NULL;

    // XXXlorenmcq - Fill in error buffer
    return hr;
}



//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////


BOOL
CD3DXEffectParser::Eval(Node *pNode, Binding *pBind, DWORD *pdw, Binding **ppBind)
{
    DWORD dw = 0;

    while(pNode)
    {
        if(NODE_DWORD == pNode->m_dwType)
        {
            DwordNode *pDwordNode = (DwordNode *) pNode;
            dw |= pDwordNode->m_dwValue;
        }
        else if(NODE_FLOAT == pNode->m_dwType)
        {
            FloatNode *pFloatNode = (FloatNode *) pNode;
            dw |= *((DWORD *) &pFloatNode->m_fValue);
        }
        else if(NODE_ID == pNode->m_dwType)
        {
            IdNode *pIdNode = (IdNode *) pNode;
            DWORD dwBind = 0;

            while(pBind[dwBind].m_szName && _stricmp(pIdNode->m_szId, pBind[dwBind].m_szName))
                dwBind++;

            if(pBind[dwBind].m_szName)
            {
                dw |= pBind[dwBind].m_dwValue;

                if(ppBind)
                    *ppBind = pBind[dwBind].m_pBind;
            }
            else
            {
                DPF(0, "'%s' unexpected", pIdNode->m_szId);
                return FALSE;
            }
        }

        pNode = pNode->m_pNext;
    }

    *pdw = dw;
    return TRUE;
}


HRESULT
CD3DXEffectParser::Compile()
{
    DWORD dwType;
    DWORD dwSrcBlend;
    DWORD dwDstBlend;
    DWORD dwTechniqueFVF;
    DWORD dwPassFVF;

    UINT uDwords;
    UINT uFloats;
    UINT uColors;
    UINT uVectors;
    UINT uMatrices;
    UINT uTextures;
    UINT uTechniques;

    UINT uMinLOD;
    UINT uMaxLOD;

    Node *pNode;

    TechniqueData **ppTechniqueData;
    TechniqueNode *pTechniqueNode;
    TechniqueData *pTechniqueData;

    DeclarationNode *pDeclarationNode;

    PassData **ppPassData;
    PassNode *pPassNode;
    PassData *pPassData;

    StageData **ppStageData;
    StageNode *pStageNode;
    StageData *pStageData;

    StateData **ppStateData;
    StateNode *pStateNode;
    StateData *pStateData;

    VShaderNode *pVShaderNode;
    VShaderData *pVShaderData;

    PShaderNode *pPShaderNode;
    PShaderData *pPShaderData;

    AsmNode *pAsmNode;

    Binding *pBind;
    Binding *pBindDword   = NULL;
    Binding *pBindFloat   = NULL;
    Binding *pBindColor   = NULL;
    Binding *pBindVector  = NULL;
    Binding *pBindMatrix  = NULL;
    Binding *pBindTexture = NULL;


    if(!m_pNode)
        return E_FAIL;

    if(m_pNode->m_dwType != NODE_EFFECT)
        return E_FAIL;

    if(m_pNode->m_pStack != NULL)
        return E_FAIL;

    m_pEffectNode = (EffectNode *) m_pNode;

    if(!(m_pEffectData = new EffectData))
        return NULL;


    // Count variables
    uDwords   = 0;
    uFloats   = 0;
    uColors   = 0;
    uVectors  = 0;
    uMatrices = 0;
    uTextures = 0;

    pDeclarationNode = (DeclarationNode *) m_pEffectNode->m_pDeclarations;

    while(pDeclarationNode)
    {
        if(!Eval(pDeclarationNode->m_pType, g_BindDecl, &dwType, NULL))
            goto LFail;

        switch(dwType)
        {
        case 0: uDwords++;   break;
        case 1: uFloats++;   break;
        case 2: uColors++;   break;
        case 3: uVectors++;  break;
        case 4: uMatrices++; break;
        case 5: uTextures++; break;
        }

        pDeclarationNode = (DeclarationNode *) pDeclarationNode->m_pNext;
    }

    m_pEffectData->m_uDwords   = uDwords;
    m_pEffectData->m_uFloats   = uFloats;
    m_pEffectData->m_uColors   = uColors;
    m_pEffectData->m_uVectors  = uVectors;
    m_pEffectData->m_uMatrices = uMatrices;
    m_pEffectData->m_uTextures = uTextures;


    // Create bindings for textures & matrices
    if(!(pBindDword   = new Binding[uDwords   + 1]) ||
       !(pBindFloat   = new Binding[uFloats   + 1]) ||
       !(pBindColor   = new Binding[uColors   + 1]) ||
       !(pBindVector  = new Binding[uVectors  + 1]) ||
       !(pBindMatrix  = new Binding[uMatrices + 1]) ||
       !(pBindTexture = new Binding[uTextures + 1]))
    {
        goto LFail;
    }

    uDwords   = 0;
    uFloats   = 0;
    uColors   = 0;
    uVectors  = 0;
    uMatrices = 0;
    uTextures = 0;

    pDeclarationNode = (DeclarationNode *) m_pEffectNode->m_pDeclarations;


    while(pDeclarationNode)
    {
        UINT uBind;

        if(!Eval(pDeclarationNode->m_pType, g_BindDecl, &dwType, NULL))
            goto LFail;

        switch(dwType)
        {
        case 0: pBind = pBindDword;   uBind = uDwords++;   break;
        case 1: pBind = pBindFloat;   uBind = uFloats++;   break;
        case 2: pBind = pBindColor;   uBind = uColors++;   break;
        case 3: pBind = pBindVector;  uBind = uVectors++;  break;
        case 4: pBind = pBindMatrix;  uBind = uMatrices++; break;
        case 5: pBind = pBindTexture; uBind = uTextures++; break;
        }

        pBind[uBind].m_szName  = ((IdNode *) (pDeclarationNode->m_pName))->m_szId;
        pBind[uBind].m_dwValue = (DWORD) uBind;
        pBind[uBind].m_pBind   = NULL;

        pDeclarationNode = (DeclarationNode *) pDeclarationNode->m_pNext;
    }

    memset(&pBindDword  [uDwords],   0x00, sizeof(Binding));
    memset(&pBindFloat  [uFloats],   0x00, sizeof(Binding));
    memset(&pBindColor  [uColors],   0x00, sizeof(Binding));
    memset(&pBindVector [uVectors],  0x00, sizeof(Binding));
    memset(&pBindMatrix [uMatrices], 0x00, sizeof(Binding));
    memset(&pBindTexture[uTextures], 0x00, sizeof(Binding));


    // Process techniques
    uTechniques = 0;
    uMinLOD = (DWORD) -1;
    uMaxLOD = 0;

    ppTechniqueData = &m_pEffectData->m_pTechnique;
    pTechniqueNode = (TechniqueNode *) m_pEffectNode->m_pTechniques;

    while(pTechniqueNode)
    {
        if(!(pTechniqueData = new TechniqueData))
            goto LFail;

        *ppTechniqueData = pTechniqueData;
        ppTechniqueData = &pTechniqueData->m_pNext;


        // LOD
        if(pTechniqueNode->m_pLOD)
        {
            DwordNode *pDwordNode = (DwordNode *) pTechniqueNode->m_pLOD;
            pTechniqueData->m_uLOD = pDwordNode->m_dwValue;

            if(uMaxLOD < pDwordNode->m_dwValue)
                uMaxLOD = pDwordNode->m_dwValue;

            if(uMinLOD > pDwordNode->m_dwValue)
                uMinLOD = pDwordNode->m_dwValue;
        }


        // Renderstates
        dwTechniqueFVF = 0;

        ppStateData = &pTechniqueData->m_pRS;
        pStateNode = (StateNode *) pTechniqueNode->m_pStates;

        while(pStateNode)
        {
            if(!(pStateData = new StateData))
                goto LFail;

            *ppStateData = pStateData;
            ppStateData = &pStateData->m_pNext;

            if(!Eval(pStateNode->m_pLValue, g_BindRS, &pStateData->m_dwState, &pBind))
                goto LFail;

            if(!Eval(pStateNode->m_pRValue, pBind, &pStateData->m_dwValue, NULL))
                goto LFail;

            if(pStateData->m_dwState == D3DRS_FVF)
                dwTechniqueFVF = pStateData->m_dwValue;

            pStateNode = (StateNode *) pStateNode->m_pNext;
        }


        // Passes
        ppPassData = &pTechniqueData->m_pPass;
        pPassNode = (PassNode *) pTechniqueNode->m_pPasses;

        while(pPassNode)
        {
            if(!(pPassData = new PassData))
                goto LFail;

            *ppPassData = pPassData;
            ppPassData = &pPassData->m_pNext;


            // Enable AlphaBlend if Src/DstBlend is specified
            pStateData;
            ppStateData = &pPassData->m_pRS;

            if(!(pStateData = new StateData))
                goto LFail;

            pStateData->m_dwState = D3DRS_ALPHABLENDENABLE;
            pStateData->m_dwValue = (pPassNode->m_pSrcBlend && pPassNode->m_pDstBlend);
            pStateData->m_pNext   = NULL;

            *ppStateData = pStateData;
            ppStateData = &pStateData->m_pNext;


            // Add Src/DstBlend states
            if(pPassNode->m_pSrcBlend && pPassNode->m_pDstBlend)
            {
                if(!Eval(pPassNode->m_pSrcBlend, g_BindBLEND, &dwSrcBlend, NULL))
                    goto LFail;

                if(!Eval(pPassNode->m_pDstBlend, g_BindBLEND, &dwDstBlend, NULL))
                    goto LFail;

                if(!(pStateData = new StateData))
                    goto LFail;

                pStateData->m_dwState = D3DRS_SRCBLEND;
                pStateData->m_dwValue = dwSrcBlend;
                pStateData->m_pNext   = NULL;

                *ppStateData = pStateData;
                ppStateData = &pStateData->m_pNext;

                if(!(pStateData = new StateData))
                    goto LFail;

                pStateData->m_dwState = D3DRS_DESTBLEND;
                pStateData->m_dwValue = dwDstBlend;
                pStateData->m_pNext   = NULL;

                *ppStateData = pStateData;
                ppStateData = &pStateData->m_pNext;
            }



            // Renderstates
            dwPassFVF = dwTechniqueFVF;
            pStateNode = (StateNode *) pPassNode->m_pStates;

            while(pStateNode)
            {
                if(!(pStateData = new StateData))
                    goto LFail;

                *ppStateData = pStateData;
                ppStateData = &pStateData->m_pNext;

                if(!Eval(pStateNode->m_pLValue, g_BindRS, &pStateData->m_dwState, &pBind))
                    goto LFail;

                if(!Eval(pStateNode->m_pRValue, pBind, &pStateData->m_dwValue, NULL))
                    goto LFail;

                if(pStateData->m_dwState == D3DRS_FVF)
                    dwPassFVF = pStateData->m_dwValue;

                pStateNode = (StateNode *) pStateNode->m_pNext;
            }


            // Stages
            ppStageData = &pPassData->m_pStage;
            pStageNode = (StageNode *) pPassNode->m_pStages;

            while(pStageNode)
            {
                if(!(pStageData = new StageData))
                    goto LFail;

                *ppStageData = pStageData;
                ppStageData = &pStageData->m_pNext;


                if(pStageNode->m_pTexture)
                {
                    if(!Eval(pStageNode->m_pTexture, pBindTexture, (DWORD *) &pStageData->m_uTexture, NULL))
                        goto LFail;
                }

                if(pStageNode->m_pMatrix)
                {
                    if(!Eval(pStageNode->m_pMatrix, pBindMatrix, (DWORD *) &pStageData->m_uMatrix, NULL))
                        goto LFail;
                }

                // TextureStageStates
                ppStateData = &pStageData->m_pTSS;
                pStateNode = (StateNode *) pStageNode->m_pStates;

                while(pStateNode)
                {
                    if(!(pStateData = new StateData))
                        goto LFail;

                    *ppStateData = pStateData;
                    ppStateData = &pStateData->m_pNext;

                    if(!Eval(pStateNode->m_pLValue, g_BindTSS, (DWORD *) &pStateData->m_dwState, &pBind))
                        goto LFail;

                    if(!Eval(pStateNode->m_pRValue, pBind, (DWORD *) &pStateData->m_dwValue, NULL))
                        goto LFail;

                    pStateNode = (StateNode *) pStateNode->m_pNext;
                }

                pStageNode = (StageNode *) pStageNode->m_pNext;
            }


            // VertexShader
            pNode = pPassNode->m_pShaders;

            if(pNode && pNode->m_dwType == NODE_VSHADER)
            {
                pVShaderNode = (VShaderNode *) pNode;

                if(!(pVShaderData = new VShaderData))
                    goto LFail;

                pPassData->m_pVShader = pVShaderData;

                // FVF
                if(dwPassFVF)
                {
                    if(FAILED(D3DXDeclaratorFromFVF(dwPassFVF, pVShaderData->m_pDeclaration)))
                        goto LFail;
                }

                // Consts
                if(pVShaderNode->m_pConst)
                {
                    // UNDONE
                }

                // Asm
                pAsmNode = (AsmNode *) pVShaderNode->m_pAsm;

                if(FAILED(D3DXAssembleVertexShader(pAsmNode->m_pchAsm, pAsmNode->m_cchAsm, &pVShaderData->m_pShader, NULL)))
                    goto LFail;

                pNode = pNode->m_pNext;
            }


            // Pixel Shader
            if(pNode && pNode->m_dwType == NODE_PSHADER)
            {
                pPShaderNode = (PShaderNode *) pNode;

                if(!(pPShaderData = new PShaderData))
                    goto LFail;

                pPassData->m_pPShader = pPShaderData;

                // Consts
                if(pPShaderNode->m_pConst)
                {
                    // UNDONE
                }

                // Asm
                pAsmNode = (AsmNode *) pVShaderNode->m_pAsm;

                if(FAILED(D3DXAssemblePixelShader(pAsmNode->m_pchAsm, pAsmNode->m_cchAsm, &pPShaderData->m_pShader, NULL)))
                    goto LFail;
            }


            pTechniqueData->m_uPasses++;
            pPassNode = (PassNode *) pPassNode->m_pNext;
        }


        uTechniques++;
        pTechniqueNode = (TechniqueNode *) pTechniqueNode->m_pNext;
    }

    if(uMinLOD > uMaxLOD)
        uMinLOD = uMaxLOD;

    m_pEffectData->m_uMinLOD = uMinLOD;
    m_pEffectData->m_uMaxLOD = uMaxLOD;
    m_pEffectData->m_uTechniques = uTechniques;

    return S_OK;

LFail:
    if(pBindDword)   delete pBindDword;
    if(pBindFloat)   delete pBindFloat;
    if(pBindColor)   delete pBindColor;
    if(pBindVector)  delete pBindVector;
    if(pBindMatrix)  delete pBindMatrix;
    if(pBindTexture) delete pBindTexture;

    return E_FAIL;
}
