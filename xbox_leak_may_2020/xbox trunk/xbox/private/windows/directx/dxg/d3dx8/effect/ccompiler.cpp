///////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) Microsoft Corporation.  All Rights Reserved.
//
//  File:       parse.cpp
//  Content:    Parse D3DX Effect files
//
///////////////////////////////////////////////////////////////////////////

#include "pcheffect.h"
#include <xgraphics.h>
#undef AssembleShader

//
// Productions
//

enum
{
    P_EFFECT_BODY0,
    P_EFFECT_BODY1,
    P_PARAMETERS0,
    P_PARAMETERS1,
    P_PARAMETER0,
    P_PARAMETER1,
    P_TECHNIQUES0,
    P_TECHNIQUES1,
    P_TECHNIQUE0,
    P_TECHNIQUE1,
    P_TECHNIQUE_BODY0,
    P_TECHNIQUE_BODY1,
    P_TECHNIQUE_BODY2,
    P_TECHNIQUE_BODY3,
    P_PASSES0,
    P_PASSES1,
    P_PASS0,
    P_PASS1,
    P_PASS_BODY0,
    P_PASS_BODY1,
    P_ASSIGNMENTS0,
    P_ASSIGNMENTS1,
    P_ASSIGNMENT0,
    P_ASSIGNMENT1,
    P_LVALUE0,
    P_LVALUE1,
    P_EXPRESSION0,
    P_EXPRESSION1,
    P_REFERENCE0,
    P_CONST_DWORD,
    P_CONST_FLOAT,
    P_CONST_VECTOR,
    P_CONST_MATRIX,
    P_CONST_VERTEXSHADER,
    P_CONST_PIXELSHADER,
    P_VALUE0,
    P_VALUE1,
    P_DWORD0,
    P_DWORD1,
    P_DWORD2,
    P_DWORD3,
    P_FLOAT0,
    P_FLOAT1,
    P_VECTOR0,
    P_VECTOR1,
    P_VECTOR2,
    P_VECTOR3,
    P_MATRIX0,
    P_VERTEXSHADER0,
    P_VERTEXSHADER1,
    P_PIXELSHADER0,
    P_FOURCC0,
    P_ID0,
    P_DECL0,
    P_ASM0
};

#define Prod(n, m) \
    CD3DXEffectCompiler::s_pCompiler->Production(n, m)

#define d3dxeffect_lex \
    CD3DXEffectCompiler::s_pCompiler->Token

#define d3dxeffect_error \
    CD3DXEffectCompiler::s_pCompiler->Error

#define lint
#pragma warning(disable : 4242)
#include "effect_y.h"
#pragma warning(default : 4242)




//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////


#define RVALUE_END() \
    { NULL, 0 }

#define LVALUE_END() \
    { NULL, D3DXPT_DWORD, 0, 0, NULL }


static RValue g_rvType[] =
{
    { "DWORD",        D3DXPT_DWORD         },
    { "FLOAT",        D3DXPT_FLOAT         },
    { "VECTOR",       D3DXPT_VECTOR        },
    { "MATRIX",       D3DXPT_MATRIX        },
    { "TEXTURE",      D3DXPT_TEXTURE       },
    { "VERTEXSHADER", D3DXPT_VERTEXSHADER  },
    { "PIXELSHADER",  D3DXPT_PIXELSHADER   },
    RVALUE_END()
};


static RValue g_rvBool[] =
{
    { "FALSE",  0 },
    { "TRUE",   1 },
    RVALUE_END()
};


static RValue g_rvSHADE[] =
{
    { "FLAT",       D3DSHADE_FLAT    },
    { "GOURAUD",    D3DSHADE_GOURAUD },
    RVALUE_END()            
};

static RValue g_rvFILL[] =
{
    { "POINT",      D3DFILL_POINT     },
    { "WIREFRAME",  D3DFILL_WIREFRAME },
    { "SOLID",      D3DFILL_SOLID     },
    RVALUE_END()
};

static RValue g_rvBLEND[] =
{
    { "ZERO",               D3DBLEND_ZERO            },
    { "ONE",                D3DBLEND_ONE             },
    { "SRCCOLOR",           D3DBLEND_SRCCOLOR        },
    { "INVSRCCOLOR",        D3DBLEND_INVSRCCOLOR     },
    { "SRCALPHA",           D3DBLEND_SRCALPHA        },
    { "INVSRCALPHA",        D3DBLEND_INVSRCALPHA     },
    { "DESTALPHA",          D3DBLEND_DESTALPHA       },
    { "INVDESTALPHA",       D3DBLEND_INVDESTALPHA    },
    { "DESTCOLOR",          D3DBLEND_DESTCOLOR       },
    { "INVDESTCOLOR",       D3DBLEND_INVDESTCOLOR    },
    { "SRCALPHASAT",        D3DBLEND_SRCALPHASAT     },
    RVALUE_END()
};

static RValue g_rvTADDRESS[] =
{
    { "WRAP",   D3DTADDRESS_WRAP   },
    { "MIRROR", D3DTADDRESS_MIRROR },
    { "CLAMP",  D3DTADDRESS_CLAMP  },
    { "BORDER", D3DTADDRESS_BORDER },
    RVALUE_END()
};

static RValue g_rvCULL[] =
{
    { "NONE",   D3DCULL_NONE },
    { "CW",     D3DCULL_CW   },
    { "CCW",    D3DCULL_CCW  },
    RVALUE_END()
};

static RValue g_rvCMP[] =
{
    { "NEVER",          D3DCMP_NEVER        },
    { "LESS",           D3DCMP_LESS         },
    { "EQUAL",          D3DCMP_EQUAL        },
    { "LESSEQUAL",      D3DCMP_LESSEQUAL    },
    { "GREATER",        D3DCMP_GREATER      },
    { "NOTEQUAL",       D3DCMP_NOTEQUAL     },
    { "GREATEREQUAL",   D3DCMP_GREATEREQUAL },
    { "ALWAYS",         D3DCMP_ALWAYS       },
    RVALUE_END()
};

static RValue g_rvSTENCILOP[] =
{
    { "KEEP",       D3DSTENCILOP_KEEP    },
    { "ZERO",       D3DSTENCILOP_ZERO    },
    { "REPLACE",    D3DSTENCILOP_REPLACE },
    { "INCRSAT",    D3DSTENCILOP_INCRSAT },
    { "DECRSAT",    D3DSTENCILOP_DECRSAT },
    { "INVERT",     D3DSTENCILOP_INVERT  },
    { "INCR",       D3DSTENCILOP_INCR    },
    { "DECR",       D3DSTENCILOP_DECR    },
    RVALUE_END()
};


static RValue g_rvFOG[] =
{
    { "NONE",   D3DFOG_NONE   },
    { "EXP",    D3DFOG_EXP    },
    { "EXP2",   D3DFOG_EXP2   },
    { "LINEAR", D3DFOG_LINEAR },
    RVALUE_END()
};


static RValue g_rvZB[] =
{
    { "TRUE",   D3DZB_TRUE  },
    { "FALSE",  D3DZB_FALSE },
    { "USEW",   D3DZB_USEW  },
    RVALUE_END()
};


static RValue g_rvMCS[] =
{
    { "MATERIAL",   D3DMCS_MATERIAL },
    { "COLOR1",     D3DMCS_COLOR1   },
    { "COLOR2",     D3DMCS_COLOR2   },
    RVALUE_END()
};


static RValue g_rvWRAP[] =
{
    { "U",      D3DWRAP_U      },
    { "V",      D3DWRAP_V      },
    { "W",      D3DWRAP_W      },
    { "COORD0", D3DWRAPCOORD_0 },
    { "COORD1", D3DWRAPCOORD_1 },
    { "COORD2", D3DWRAPCOORD_2 },
    { "COORD3", D3DWRAPCOORD_3 },
    RVALUE_END()
};


static RValue g_rvCOLORWRITEENABLE[] =
{
    { "RED",    D3DCOLORWRITEENABLE_RED   },
    { "GREEN",  D3DCOLORWRITEENABLE_GREEN },
    { "BLUE",   D3DCOLORWRITEENABLE_BLUE  },
    { "ALPHA",  D3DCOLORWRITEENABLE_ALPHA },
    RVALUE_END()
};


static RValue g_rvHOB[] =
{
    { "BEZIER",         D3DBASIS_BEZIER      },
    { "BSPLINE",        D3DBASIS_BSPLINE     },
    { "INTERPOLATE",    D3DBASIS_INTERPOLATE },
    RVALUE_END()
};


static RValue g_rvORDER[] =
{
    { "LINEAR", D3DORDER_LINEAR },
    { "CUBIC",  D3DORDER_CUBIC  },
    RVALUE_END()
};


static RValue g_rvVBF[] =
{
    { "DISABLE",    D3DVBF_DISABLE  },
//    { "0WEIGHTS",   D3DVBF_0WEIGHTS },
    { "1WEIGHTS",   D3DVBF_1WEIGHTS },
    { "2WEIGHTS",   D3DVBF_2WEIGHTS },
    { "3WEIGHTS",   D3DVBF_3WEIGHTS },
    RVALUE_END()
};


static RValue g_rvBLENDOP[] =
{
    { "ADD",            D3DBLENDOP_ADD         },
    { "SUBTRACT",       D3DBLENDOP_SUBTRACT    },
    { "REVSUBTRACT",    D3DBLENDOP_REVSUBTRACT },
    { "MIN",            D3DBLENDOP_MIN         },
    { "MAX",            D3DBLENDOP_MAX         },
    RVALUE_END()
};


static RValue g_rvTSS_TCI[] =
{
    { "PASSTHRU",                       D3DTSS_TCI_PASSTHRU                    },
    { "CAMERASPACENORMAL",              D3DTSS_TCI_CAMERASPACENORMAL           },
    { "CAMERASPACEPOSITION",            D3DTSS_TCI_CAMERASPACEPOSITION         },
    { "CAMERASPACEREFLECTIONVECTOR",    D3DTSS_TCI_CAMERASPACEREFLECTIONVECTOR },
    RVALUE_END()
};


static RValue g_rvTOP[] =
{
    { "DISABLE",                   D3DTOP_DISABLE                   },
    { "SELECTARG1",                D3DTOP_SELECTARG1                },
    { "SELECTARG2",                D3DTOP_SELECTARG2                },
    { "MODULATE",                  D3DTOP_MODULATE                  },
    { "MODULATE2X",                D3DTOP_MODULATE2X                },
    { "MODULATE4X",                D3DTOP_MODULATE4X                },
    { "ADD",                       D3DTOP_ADD                       },
    { "ADDSIGNED",                 D3DTOP_ADDSIGNED                 },
    { "ADDSIGNED2X",               D3DTOP_ADDSIGNED2X               },
    { "SUBTRACT",                  D3DTOP_SUBTRACT                  },
    { "ADDSMOOTH",                 D3DTOP_ADDSMOOTH                 },
    { "BLENDDIFFUSEALPHA",         D3DTOP_BLENDDIFFUSEALPHA         },
    { "BLENDTEXTUREALPHA",         D3DTOP_BLENDTEXTUREALPHA         },
    { "BLENDFACTORALPHA",          D3DTOP_BLENDFACTORALPHA          },
    { "BLENDTEXTUREALPHAPM",       D3DTOP_BLENDTEXTUREALPHAPM       },
    { "BLENDCURRENTALPHA",         D3DTOP_BLENDCURRENTALPHA         },
    { "PREMODULATE",               D3DTOP_PREMODULATE               },
    { "MODULATEALPHA_ADDCOLOR",    D3DTOP_MODULATEALPHA_ADDCOLOR    },
    { "MODULATECOLOR_ADDALPHA",    D3DTOP_MODULATECOLOR_ADDALPHA    },
    { "MODULATEINVALPHA_ADDCOLOR", D3DTOP_MODULATEINVALPHA_ADDCOLOR },
    { "MODULATEINVCOLOR_ADDALPHA", D3DTOP_MODULATEINVCOLOR_ADDALPHA },
    { "BUMPENVMAP",                D3DTOP_BUMPENVMAP                },
    { "BUMPENVMAPLUMINANCE",       D3DTOP_BUMPENVMAPLUMINANCE       },
    { "DOTPRODUCT3",               D3DTOP_DOTPRODUCT3               },
    { "MULTIPLYADD",               D3DTOP_MULTIPLYADD               },
    { "LERP",                      D3DTOP_LERP                      },
    RVALUE_END()
};


static RValue g_rvTA[] =
{
    { "SELECTMASK",     D3DTA_SELECTMASK     },
    { "DIFFUSE",        D3DTA_DIFFUSE        },
    { "CURRENT",        D3DTA_CURRENT        },
    { "TEXTURE",        D3DTA_TEXTURE        },
    { "TFACTOR",        D3DTA_TFACTOR        },
    { "SPECULAR",       D3DTA_SPECULAR       },
    { "TEMP",           D3DTA_TEMP           },
    { "COMPLEMENT",     D3DTA_COMPLEMENT     },
    { "ALPHAREPLICATE", D3DTA_ALPHAREPLICATE },
    RVALUE_END()
};


static RValue g_rvTEXF[] =
{
    { "NONE",           D3DTEXF_NONE          },
    { "POINT",          D3DTEXF_POINT         },
    { "LINEAR",         D3DTEXF_LINEAR        },
    { "ANISOTROPIC",    D3DTEXF_ANISOTROPIC   },
    { "FLATCUBIC",      D3DTEXF_QUINCUNX      },
    { "GAUSSIANCUBIC",  D3DTEXF_GAUSSIANCUBIC },
    RVALUE_END()
};


static RValue g_rvTTFF[] =
{
    { "FALSE",      D3DTTFF_DISABLE   },
    { "DISABLE",    D3DTTFF_DISABLE   },
    { "COUNT1",     D3DTTFF_COUNT1    },
    { "COUNT2",     D3DTTFF_COUNT2    },
    { "COUNT3",     D3DTTFF_COUNT3    },
    { "COUNT4",     D3DTTFF_COUNT4    },
    { "PROJECTED",  D3DTTFF_PROJECTED },
    RVALUE_END()
};


static RValue g_rvLT[] =
{
    { "POINT",       D3DLIGHT_POINT       },
    { "SPOT",        D3DLIGHT_SPOT        },
    { "DIRECTIONAL", D3DLIGHT_DIRECTIONAL },
    RVALUE_END()
};


static LValue g_lv[] =
{
    // RS    
    { "ZENABLE",                  D3DXPT_DWORD,        1, D3DXES_RS(D3DRS_ZENABLE),                  g_rvZB },
    { "FILLMODE",                 D3DXPT_DWORD,        1, D3DXES_RS(D3DRS_FILLMODE),                 g_rvFILL },
    { "SHADEMODE",                D3DXPT_DWORD,        1, D3DXES_RS(D3DRS_SHADEMODE),                g_rvSHADE },
    { "ZWRITEENABLE",             D3DXPT_DWORD,        1, D3DXES_RS(D3DRS_ZWRITEENABLE),             g_rvBool },
    { "ALPHATESTENABLE",          D3DXPT_DWORD,        1, D3DXES_RS(D3DRS_ALPHATESTENABLE),          g_rvBool },
    { "SRCBLEND",                 D3DXPT_DWORD,        1, D3DXES_RS(D3DRS_SRCBLEND),                 g_rvBLEND },
    { "DESTBLEND",                D3DXPT_DWORD,        1, D3DXES_RS(D3DRS_DESTBLEND),                g_rvBLEND },
    { "CULLMODE",                 D3DXPT_DWORD,        1, D3DXES_RS(D3DRS_CULLMODE),                 g_rvCULL },
    { "ZFUNC",                    D3DXPT_DWORD,        1, D3DXES_RS(D3DRS_ZFUNC),                    g_rvCMP },
    { "ALPHAREF",                 D3DXPT_DWORD,        1, D3DXES_RS(D3DRS_ALPHAREF),                 NULL },
    { "ALPHAFUNC",                D3DXPT_DWORD,        1, D3DXES_RS(D3DRS_ALPHAFUNC),                g_rvCMP },
    { "DITHERENABLE",             D3DXPT_DWORD,        1, D3DXES_RS(D3DRS_DITHERENABLE),             g_rvBool },
    { "ALPHABLENDENABLE",         D3DXPT_DWORD,        1, D3DXES_RS(D3DRS_ALPHABLENDENABLE),         g_rvBool },
    { "FOGENABLE",                D3DXPT_DWORD,        1, D3DXES_RS(D3DRS_FOGENABLE),                g_rvBool },
    { "SPECULARENABLE",           D3DXPT_DWORD,        1, D3DXES_RS(D3DRS_SPECULARENABLE),           g_rvBool },
    { "FOGCOLOR",                 D3DXPT_DWORD,        1, D3DXES_RS(D3DRS_FOGCOLOR),                 NULL },
    { "FOGTABLEMODE",             D3DXPT_DWORD,        1, D3DXES_RS(D3DRS_FOGTABLEMODE),             g_rvFOG },
    { "FOGSTART",                 D3DXPT_FLOAT,        1, D3DXES_RS(D3DRS_FOGSTART),                 NULL },
    { "FOGEND",                   D3DXPT_FLOAT,        1, D3DXES_RS(D3DRS_FOGEND),                   NULL },
    { "FOGDENSITY",               D3DXPT_FLOAT,        1, D3DXES_RS(D3DRS_FOGDENSITY),               NULL },
    { "EDGEANTIALIAS",            D3DXPT_DWORD,        1, D3DXES_RS(D3DRS_EDGEANTIALIAS),            g_rvBool },
    { "ZBIAS",                    D3DXPT_DWORD,        1, D3DXES_RS(D3DRS_ZBIAS),                    NULL },
    { "RANGEFOGENABLE",           D3DXPT_DWORD,        1, D3DXES_RS(D3DRS_RANGEFOGENABLE),           NULL },
    { "STENCILENABLE",            D3DXPT_DWORD,        1, D3DXES_RS(D3DRS_STENCILENABLE),            g_rvBool },
    { "STENCILFAIL",              D3DXPT_DWORD,        1, D3DXES_RS(D3DRS_STENCILFAIL),              g_rvSTENCILOP },
    { "STENCILZFAIL",             D3DXPT_DWORD,        1, D3DXES_RS(D3DRS_STENCILZFAIL),             g_rvSTENCILOP },
    { "STENCILPASS",              D3DXPT_DWORD,        1, D3DXES_RS(D3DRS_STENCILPASS),              g_rvSTENCILOP },
    { "STENCILFUNC",              D3DXPT_DWORD,        1, D3DXES_RS(D3DRS_STENCILFUNC),              g_rvCMP },
    { "STENCILREF",               D3DXPT_DWORD,        1, D3DXES_RS(D3DRS_STENCILREF),               NULL },
    { "STENCILMASK",              D3DXPT_DWORD,        1, D3DXES_RS(D3DRS_STENCILMASK),              NULL },
    { "STENCILWRITEMASK",         D3DXPT_DWORD,        1, D3DXES_RS(D3DRS_STENCILWRITEMASK),         NULL },
    { "TEXTUREFACTOR",            D3DXPT_DWORD,        1, D3DXES_RS(D3DRS_TEXTUREFACTOR),            NULL },
    { "WRAP0",                    D3DXPT_DWORD,        1, D3DXES_RS(D3DRS_WRAP0),                    g_rvWRAP },
    { "WRAP1",                    D3DXPT_DWORD,        1, D3DXES_RS(D3DRS_WRAP1),                    g_rvWRAP },
    { "WRAP2",                    D3DXPT_DWORD,        1, D3DXES_RS(D3DRS_WRAP2),                    g_rvWRAP },
    { "WRAP3",                    D3DXPT_DWORD,        1, D3DXES_RS(D3DRS_WRAP3),                    g_rvWRAP },
    { "LIGHTING",                 D3DXPT_DWORD,        1, D3DXES_RS(D3DRS_LIGHTING),                 g_rvBool },
    { "AMBIENT",                  D3DXPT_DWORD,        1, D3DXES_RS(D3DRS_AMBIENT),                  NULL },
    { "COLORVERTEX",              D3DXPT_DWORD,        1, D3DXES_RS(D3DRS_COLORVERTEX),              g_rvBool },
    { "LOCALVIEWER",              D3DXPT_DWORD,        1, D3DXES_RS(D3DRS_LOCALVIEWER),              g_rvBool },
    { "NORMALIZENORMALS",         D3DXPT_DWORD,        1, D3DXES_RS(D3DRS_NORMALIZENORMALS),         g_rvBool },
    { "DIFFUSEMATERIALSOURCE",    D3DXPT_DWORD,        1, D3DXES_RS(D3DRS_DIFFUSEMATERIALSOURCE),    g_rvMCS },
    { "SPECULARMATERIALSOURCE",   D3DXPT_DWORD,        1, D3DXES_RS(D3DRS_SPECULARMATERIALSOURCE),   g_rvMCS },
    { "AMBIENTMATERIALSOURCE",    D3DXPT_DWORD,        1, D3DXES_RS(D3DRS_AMBIENTMATERIALSOURCE),    g_rvMCS },
    { "EMISSIVEMATERIALSOURCE",   D3DXPT_DWORD,        1, D3DXES_RS(D3DRS_EMISSIVEMATERIALSOURCE),   g_rvMCS },
    { "VERTEXBLEND",              D3DXPT_DWORD,        1, D3DXES_RS(D3DRS_VERTEXBLEND),              g_rvVBF },
    { "POINTSIZE",                D3DXPT_FLOAT,        1, D3DXES_RS(D3DRS_POINTSIZE),                NULL },
    { "POINTSIZE_MIN",            D3DXPT_FLOAT,        1, D3DXES_RS(D3DRS_POINTSIZE_MIN),            NULL },
    { "POINTSIZE_MAX",            D3DXPT_FLOAT,        1, D3DXES_RS(D3DRS_POINTSIZE_MAX),            NULL },
    { "POINTSPRITEENABLE",        D3DXPT_DWORD,        1, D3DXES_RS(D3DRS_POINTSPRITEENABLE),        g_rvBool },
    { "POINTSCALEENABLE",         D3DXPT_DWORD,        1, D3DXES_RS(D3DRS_POINTSCALEENABLE),         g_rvBool },
    { "POINTSCALE_A",             D3DXPT_FLOAT,        1, D3DXES_RS(D3DRS_POINTSCALE_A),             NULL },
    { "POINTSCALE_B",             D3DXPT_FLOAT,        1, D3DXES_RS(D3DRS_POINTSCALE_B),             NULL },
    { "POINTSCALE_C",             D3DXPT_FLOAT,        1, D3DXES_RS(D3DRS_POINTSCALE_C),             NULL },
    { "MULTISAMPLEANTIALIAS",     D3DXPT_DWORD,        1, D3DXES_RS(D3DRS_MULTISAMPLEANTIALIAS),     g_rvBool },
    { "MULTISAMPLEMASK",          D3DXPT_DWORD,        1, D3DXES_RS(D3DRS_MULTISAMPLEMASK),          NULL },
    { "PATCHSEGMENTS",            D3DXPT_DWORD,        1, D3DXES_RS(D3DRS_PATCHSEGMENTS),            NULL },
    { "COLORWRITEENABLE",         D3DXPT_DWORD,        1, D3DXES_RS(D3DRS_COLORWRITEENABLE),         g_rvBool },
    { "BLENDOP",                  D3DXPT_DWORD,        1, D3DXES_RS(D3DRS_BLENDOP),                  g_rvBLENDOP },

    // TSS
    { "COLOROP",                  D3DXPT_DWORD,        8, D3DXES_TSS(D3DTSS_COLOROP),                g_rvTOP },
    { "COLORARG0",                D3DXPT_DWORD,        8, D3DXES_TSS(D3DTSS_COLORARG0),              g_rvTA },
    { "COLORARG1",                D3DXPT_DWORD,        8, D3DXES_TSS(D3DTSS_COLORARG1),              g_rvTA },
    { "COLORARG2",                D3DXPT_DWORD,        8, D3DXES_TSS(D3DTSS_COLORARG2),              g_rvTA },
    { "ALPHAOP",                  D3DXPT_DWORD,        8, D3DXES_TSS(D3DTSS_ALPHAOP),                g_rvTOP },
    { "ALPHAARG0",                D3DXPT_DWORD,        8, D3DXES_TSS(D3DTSS_ALPHAARG0),              g_rvTA },
    { "ALPHAARG1",                D3DXPT_DWORD,        8, D3DXES_TSS(D3DTSS_ALPHAARG1),              g_rvTA },
    { "ALPHAARG2",                D3DXPT_DWORD,        8, D3DXES_TSS(D3DTSS_ALPHAARG2),              g_rvTA },
    { "RESULTARG",                D3DXPT_DWORD,        8, D3DXES_TSS(D3DTSS_RESULTARG),              g_rvTA },
    { "BUMPENVMAT00",             D3DXPT_DWORD,        8, D3DXES_TSS(D3DTSS_BUMPENVMAT00),           NULL },
    { "BUMPENVMAT01",             D3DXPT_DWORD,        8, D3DXES_TSS(D3DTSS_BUMPENVMAT01),           NULL },
    { "BUMPENVMAT10",             D3DXPT_DWORD,        8, D3DXES_TSS(D3DTSS_BUMPENVMAT10),           NULL },
    { "BUMPENVMAT11",             D3DXPT_DWORD,        8, D3DXES_TSS(D3DTSS_BUMPENVMAT11),           NULL },
    { "TEXCOORDINDEX",            D3DXPT_DWORD,        8, D3DXES_TSS(D3DTSS_TEXCOORDINDEX),          g_rvTSS_TCI },
    { "ADDRESSU",                 D3DXPT_DWORD,        8, D3DXES_TSS(D3DTSS_ADDRESSU),               g_rvTADDRESS },
    { "ADDRESSV",                 D3DXPT_DWORD,        8, D3DXES_TSS(D3DTSS_ADDRESSV),               g_rvTADDRESS },
    { "ADDRESSW",                 D3DXPT_DWORD,        8, D3DXES_TSS(D3DTSS_ADDRESSW),               g_rvTADDRESS },
    { "BORDERCOLOR",              D3DXPT_DWORD,        8, D3DXES_TSS(D3DTSS_BORDERCOLOR),            NULL },
    { "MAGFILTER",                D3DXPT_DWORD,        8, D3DXES_TSS(D3DTSS_MAGFILTER),              g_rvTEXF },
    { "MINFILTER",                D3DXPT_DWORD,        8, D3DXES_TSS(D3DTSS_MINFILTER),              g_rvTEXF },
    { "MIPFILTER",                D3DXPT_DWORD,        8, D3DXES_TSS(D3DTSS_MIPFILTER),              g_rvTEXF },
    { "MIPMAPLODBIAS",            D3DXPT_DWORD,        8, D3DXES_TSS(D3DTSS_MIPMAPLODBIAS),          NULL },
    { "MAXMIPLEVEL",              D3DXPT_DWORD,        8, D3DXES_TSS(D3DTSS_MAXMIPLEVEL),            NULL },
    { "MAXANISOTROPY",            D3DXPT_DWORD,        8, D3DXES_TSS(D3DTSS_MAXANISOTROPY),          NULL },
    { "BUMPENVLSCALE",            D3DXPT_FLOAT,        8, D3DXES_TSS(D3DTSS_BUMPENVLSCALE),          NULL },
    { "BUMPENVLOFFSET",           D3DXPT_FLOAT,        8, D3DXES_TSS(D3DTSS_BUMPENVLOFFSET),         NULL },
    { "TEXTURETRANSFORMFLAGS",    D3DXPT_DWORD,        8, D3DXES_TSS(D3DTSS_TEXTURETRANSFORMFLAGS),  g_rvTTFF },

    // Light
    { "LIGHTTYPE",                D3DXPT_DWORD,       -1, D3DXES_LIGHTTYPE,                          g_rvLT },
    { "LIGHTDIFFUSE",             D3DXPT_VECTOR,      -1, D3DXES_LIGHTDIFFUSE,                       NULL },
    { "LIGHTSPECULAR",            D3DXPT_VECTOR,      -1, D3DXES_LIGHTSPECULAR,                      NULL },
    { "LIGHTAMBIENT",             D3DXPT_VECTOR,      -1, D3DXES_LIGHTAMBIENT,                       NULL },
    { "LIGHTPOSITION",            D3DXPT_VECTOR,      -1, D3DXES_LIGHTPOSITION,                      NULL },
    { "LIGHTDIRECTION",           D3DXPT_VECTOR,      -1, D3DXES_LIGHTDIRECTION,                     NULL },
    { "LIGHTRANGE",               D3DXPT_FLOAT,       -1, D3DXES_LIGHTRANGE,                         NULL },
    { "LIGHTFALLOFF",             D3DXPT_FLOAT,       -1, D3DXES_LIGHTFALLOFF,                       NULL },
    { "LIGHTATTENUATION0",        D3DXPT_FLOAT,       -1, D3DXES_LIGHTATTENUATION0,                  NULL },
    { "LIGHTATTENUATION1",        D3DXPT_FLOAT,       -1, D3DXES_LIGHTATTENUATION1,                  NULL },
    { "LIGHTATTENUATION2",        D3DXPT_FLOAT,       -1, D3DXES_LIGHTATTENUATION2,                  NULL },
    { "LIGHTTHETA",               D3DXPT_FLOAT,       -1, D3DXES_LIGHTTHETA,                         NULL },
    { "LIGHTPHI",                 D3DXPT_FLOAT,       -1, D3DXES_LIGHTPHI,                           NULL },
    { "LIGHTENABLE",              D3DXPT_DWORD,       -1, D3DXES_LIGHTENABLE,                        g_rvBool },

    // Material
    { "MATERIALDIFFUSE",          D3DXPT_VECTOR,       1, D3DXES_MATERIALDIFFUSE,                    NULL },
    { "MATERIALAMBIENT",          D3DXPT_VECTOR,       1, D3DXES_MATERIALAMBIENT,                    NULL },
    { "MATERIALSPECULAR",         D3DXPT_VECTOR,       1, D3DXES_MATERIALSPECULAR,                   NULL },
    { "MATERIALEMISSIVE",         D3DXPT_VECTOR,       1, D3DXES_MATERIALEMISSIVE,                   NULL },
    { "MATERIALPOWER",            D3DXPT_FLOAT,        1, D3DXES_MATERIALPOWER,                      NULL },

    // Misc
    { "VERTEXSHADER",             D3DXPT_VERTEXSHADER, 1, D3DXES_VERTEXSHADER,                       NULL },
    { "PIXELSHADER",              D3DXPT_PIXELSHADER,  1, D3DXES_PIXELSHADER,                        NULL },
    { "VERTEXSHADERCONSTANT",     D3DXPT_CONSTANT,    -1, D3DXES_VERTEXSHADERCONSTANT,               NULL },
    { "PIXELSHADERCONSTANT",      D3DXPT_CONSTANT,    -1, D3DXES_PIXELSHADERCONSTANT,                NULL },
    { "TEXTURE",                  D3DXPT_TEXTURE,      8, D3DXES_TEXTURE,                            NULL },
    { "PROJECTIONTRANSFORM",      D3DXPT_MATRIX,       1, D3DXES_PROJECTIONTRANSFORM,                NULL },
    { "VIEWTRANSFORM",            D3DXPT_MATRIX,       1, D3DXES_VIEWTRANSFORM,                      NULL },
    { "WORLDTRANSFORM",           D3DXPT_MATRIX,     256, D3DXES_WORLDTRANSFORM,                     NULL },
    { "TEXTURETRANSFORM",         D3DXPT_MATRIX,       8, D3DXES_TEXTURETRANSFORM,                   NULL },

    LVALUE_END()
};


//////////////////////////////////////////////////////////////////////////////
// CNode /////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

CNode::CNode()
{
    m_dwType  = 0;
    m_cdwData = 0;
    m_pdwData = NULL;
    m_cNodes  = 0;
    m_ppNodes = NULL;
    m_pStack  = NULL;
}


CNode::~CNode()
{
    if(m_pdwData)
        delete [] m_pdwData;

    if(m_ppNodes)
    {
        for(UINT i = 0; i < m_cNodes; i++)
        {
            if(m_ppNodes[i])
                delete m_ppNodes[i];
        }

        delete [] m_ppNodes;
    }

    if(m_pStack)
        delete m_pStack;
}


HRESULT 
CNode::Initialize(DWORD dwType, UINT cdwData, UINT cNodes)
{
    m_dwType  = dwType;
    m_cdwData = cdwData;
    m_cNodes  = cNodes;

    if(m_cdwData)
    {
        if(!(m_pdwData = new DWORD[m_cdwData]))
            return E_OUTOFMEMORY;
    }
    else
    {
        m_pdwData = NULL;
    }


    if(m_cNodes)
    {
        if(!(m_ppNodes = new CNode*[m_cNodes]))
            return E_OUTOFMEMORY;

        memset(m_ppNodes, 0x00, m_cNodes * sizeof(CNode*));
    }
    else
    {
        m_ppNodes = NULL;
    }

    return S_OK;
}



//////////////////////////////////////////////////////////////////////////////
// CEffectCompiler ///////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

CD3DXEffectCompiler* CD3DXEffectCompiler::s_pCompiler = NULL;

CD3DXEffectCompiler::CD3DXEffectCompiler()
{
    m_pEffect = NULL;
    m_pErrors = NULL;
    m_pParameters = NULL;

    m_bFail = FALSE;

    m_iLine     = 1;
    m_szFile[0] = '\0';
    m_pchSource = NULL;
    m_pchLine   = NULL;
    m_pch       = NULL;
    m_pchLim    = NULL;

    m_szTok[0]  = '\0';
    m_pchAsm    = NULL;
    m_cchAsm    = 0;

    s_pCompiler = this;
}


CD3DXEffectCompiler::~CD3DXEffectCompiler()
{
    if(m_pEffect)
        delete m_pEffect;

    if(m_pErrors)
        delete m_pErrors;
}



//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////



HRESULT
CD3DXEffectCompiler::Compile(LPCVOID pv, UINT cb, LPCSTR szFile,
        LPD3DXBUFFER *ppCompiledEffect, LPD3DXBUFFER *ppCompilationErrors)
{
    HRESULT hr = D3DERR_INVALIDCALL;

    if(!pv || !cb)
        goto LDone;

    m_bFail     = FALSE;
    m_iLine     = 1;
    m_pchSource = (char *) pv;
    m_pch       = m_pchSource;
    m_pchLine   = m_pchSource;
    m_pchLim    = m_pchSource + cb;

    _snprintf(m_szFile, 256, "%s", szFile ? szFile : "");



    // Parse

    if(yyparse() || m_bFail)
    {
        delete m_pEffect;
        m_pEffect = NULL;
        hr = E_FAIL;
        goto LDone;
    }

    hr = S_OK;
    goto LDone;


LDone:
    // Create code buffer
    if(ppCompiledEffect)
    {
        UINT cdwSize = Serialize(m_pEffect, NULL);
    
        if(SUCCEEDED(D3DXCreateBuffer(cdwSize * sizeof(DWORD), ppCompiledEffect)))
        {
            Serialize(m_pEffect, (*ppCompiledEffect)->GetBufferPointer());
        }
    }

    // Create error buffer
    if(ppCompilationErrors)
    {
        *ppCompilationErrors = NULL;

        CNode *pNode;
        UINT cbSize = 1;

        for(pNode = m_pErrors; pNode; pNode = pNode->m_pStack)
            cbSize += strlen((char *) pNode->m_pdwData) + 1;

        if(SUCCEEDED(D3DXCreateBuffer(cbSize, ppCompilationErrors)))
        {
            BYTE *pb = (BYTE *) (*ppCompilationErrors)->GetBufferPointer() + cbSize;
            *--pb = '\0';

            for(pNode = m_pErrors; pNode; pNode = pNode->m_pStack)
            {
                UINT cb = strlen((char *) pNode->m_pdwData) + 1;

                pb -= cb;
                memcpy(pb, pNode->m_pdwData, cb);
                pb[cb - 1] = '\n';
            }   
        }
    }

    return hr;
}





int
CD3DXEffectCompiler::Token()
{
    char *pch;
    int nToken;
    SIZE_T cch;

    int iAsm = 0;
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
        else if(ch == '/' && m_pch + 1 < m_pchLim && m_pch[1] == '/')
        {
            // comment
            for(m_pch++; m_pch < m_pchLim && *m_pch != '\n'; m_pch++);
        }
        else if(iAsm)
        {
            // Asm block
            if(ch == '{')
            {
                pchAsm = ++m_pch;
                strcpy(m_szFileAsm, m_szFile);
                m_iLineAsm = m_iLine;
            }
            else if(ch == '}')
            {
                m_pchAsm = pchAsm;
                m_cchAsm = (UINT)(m_pch - m_pchAsm);

                m_pch++;
                return iAsm;
            }
            else if((ch == ';') && (iAsm == T_ASM))
            {
                // comment
                for(m_pch++; m_pch < m_pchLim && *m_pch != '\n'; m_pch++);
            }
            else if(!pchAsm)
            {
                // failure!
                m_pchAsm = NULL;
                m_cchAsm = 0;

                return iAsm;
            }
            else
            {
                m_pch++;
            }
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

            if(!_stricmp(m_szTok, "technique")) { return T_KW_TECHNIQUE; }
            if(!_stricmp(m_szTok, "pass"))      { return T_KW_PASS; }
            if(!_stricmp(m_szTok, "asm"))       { iAsm = T_ASM;  continue; }
            if(!_stricmp(m_szTok, "decl"))      { iAsm = T_DECL; continue; }

            return T_ID;
        }
        else if(ch == '0' && m_pch + 1 < m_pchLim && m_pch[1] == 'x')
        {
            // hex
            m_pch += 2;
            pch = m_pch;

            for(; m_pch < m_pchLim && (isdigit(*m_pch) || tolower(*m_pch) >= 'a' && tolower(*m_pch) <= 'f'); m_pch++);

            cch = m_pch - pch;

            if(cch >= sizeof(m_szTok))
                cch = sizeof(m_szTok) - 1;

            memcpy(m_szTok, pch, cch);
            m_szTok[cch] = 0;

            return T_HEX;
        }
        else if(isdigit(ch) || (('.' == ch) && (m_pch + 1 < m_pchLim) && isdigit(m_pch[1])))
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
CD3DXEffectCompiler::Production(UINT uProduction, UINT uArgs)
{
    UINT i;
    CNode *pNode = NULL;
    CNode **ppArg = (CNode **) _alloca(uArgs * sizeof(CNode *));

    if(m_bFail)
        return;


    // Pop arguments off of stack
    for(i = uArgs; i--; )
        ppArg[i] = Pop();

    if(m_bFail)
        goto LDone;


    // Handle productions
    switch(uProduction)
    {
    case P_EFFECT_BODY0:
        if(FAILED(CreateNode(N_EFFECT, 0, 2, &pNode)))
            break;

        if(FAILED(CreateNode(N_PARAMETERS, 0, 0, &pNode->m_ppNodes[0])))
            break;

        pNode->m_ppNodes[1] = ppArg[0];
        ppArg[0] = NULL;

        break;

    case P_EFFECT_BODY1:
        if(FAILED(CreateNode(N_EFFECT, 0, 2, &pNode)))
            break;

        pNode->m_ppNodes[0] = ppArg[0];
        ppArg[0] = NULL;

        pNode->m_ppNodes[1] = ppArg[1];
        ppArg[1] = NULL;

        break;


    case P_PARAMETERS0:
        if(FAILED(CreateNode(N_PARAMETERS, 0, 1, &pNode)))
            break;

        pNode->m_ppNodes[0] = ppArg[0];
        ppArg[0] = NULL;

        m_pParameters = pNode;
        break;

    case P_PARAMETERS1:
        if(FAILED(UnusedParameter(ppArg[0]->m_pdwData[1])))
            break;

        if(FAILED(CreateNode(N_PARAMETERS, 0, 1 + ppArg[1]->m_cNodes, &pNode)))
            break;

        pNode->m_ppNodes[0] = ppArg[0];
        ppArg[0] = NULL;

        for(i = 0; i < ppArg[1]->m_cNodes; i++)
        {
            pNode->m_ppNodes[i + 1] = ppArg[1]->m_ppNodes[i];
            ppArg[1]->m_ppNodes[i] = NULL;
        }

        m_pParameters = pNode;
        break;


    case P_PARAMETER0:
        if(FAILED(CreateNode(N_PARAMETER, 2, 0, &pNode)))
            break;

        if(FAILED(LookupType((char *) ppArg[0]->m_pdwData, &pNode->m_pdwData[0])))
            break;

        pNode->m_pdwData[1] = ppArg[1]->m_pdwData[0];
        break;

    case P_PARAMETER1:
        if(FAILED(CreateNode(N_PARAMETER, 2, 1, &pNode)))
            break;

        if(FAILED(LookupType((char *) ppArg[0]->m_pdwData, &pNode->m_pdwData[0])))
            break;

        pNode->m_pdwData[1] = ppArg[1]->m_pdwData[0];
        pNode->m_ppNodes[0] = ppArg[2];
        ppArg[2] = NULL;

        if(FAILED(ValidAssignment(pNode->m_pdwData[0], pNode->m_ppNodes[0]->m_pdwData[0])))
            break;

        break;


    case P_TECHNIQUES0:
        if(FAILED(CreateNode(N_TECHNIQUES, 0, 1, &pNode)))
            break;

        pNode->m_ppNodes[0] = ppArg[0];
        ppArg[0] = NULL;

        break;

    case P_TECHNIQUES1:
        if(FAILED(CreateNode(N_TECHNIQUES, 0, 1 + ppArg[1]->m_cNodes, &pNode)))
            break;

        pNode->m_ppNodes[0] = ppArg[0];
        ppArg[0] = NULL;

        for(i = 0; i < ppArg[1]->m_cNodes; i++)
        {
            pNode->m_ppNodes[i + 1] = ppArg[1]->m_ppNodes[i];
            ppArg[1]->m_ppNodes[i] = NULL;
        }

        break;


    case P_TECHNIQUE0:
        if(FAILED(CreateNode(N_TECHNIQUE, 1, ppArg[0]->m_cNodes, &pNode)))
            break;

        pNode->m_pdwData[0] = 0;
        
        for(i = 0; i < ppArg[0]->m_cNodes; i++)
        {
            pNode->m_ppNodes[i] = ppArg[0]->m_ppNodes[i];
            ppArg[0]->m_ppNodes[i] = NULL;
        }

        break;

    case P_TECHNIQUE1:
        if(FAILED(CreateNode(N_TECHNIQUE, 1, ppArg[1]->m_cNodes, &pNode)))
            break;

        pNode->m_pdwData[0] = ppArg[0]->m_pdwData[0];
        
        for(i = 0; i < ppArg[1]->m_cNodes; i++)
        {
            pNode->m_ppNodes[i] = ppArg[1]->m_ppNodes[i];
            ppArg[1]->m_ppNodes[i] = NULL;
        }

        break;


    case P_TECHNIQUE_BODY0:
        if(FAILED(CreateNode(N_INTERNAL, 0, 0, &pNode)))
            break;

        break;

    case P_TECHNIQUE_BODY1:
        pNode = ppArg[0];
        ppArg[0] = NULL;
        break;

    case P_TECHNIQUE_BODY2:
        if(FAILED(CreateNode(N_INTERNAL, 0, 1, &pNode)))
            break;

        if(FAILED(CreateNode(N_PASS, 1, ppArg[0]->m_cNodes, &pNode->m_ppNodes[0])))
            break;

        pNode->m_ppNodes[0]->m_pdwData[0] = 0;

        for(i = 0; i < ppArg[0]->m_cNodes; i++)
        {
            pNode->m_ppNodes[0]->m_ppNodes[i] = ppArg[0]->m_ppNodes[i];
            ppArg[0]->m_ppNodes[i] = NULL;
        }

        break;

    case P_TECHNIQUE_BODY3:
        if(FAILED(CreateNode(N_INTERNAL, 0, ppArg[1]->m_cNodes, &pNode)))
            break;

        if(FAILED(CreateNode(N_PASS, 1, ppArg[0]->m_cNodes + ppArg[1]->m_ppNodes[0]->m_cNodes, &pNode->m_ppNodes[0])))
            break;

        pNode->m_ppNodes[0]->m_pdwData[0] = ppArg[1]->m_ppNodes[0]->m_pdwData[0];

        for(i = 0; i < ppArg[0]->m_cNodes; i++)
        {
            pNode->m_ppNodes[0]->m_ppNodes[i] = ppArg[0]->m_ppNodes[i];
            ppArg[0]->m_ppNodes[i] = NULL;
        }

        for(i = 0; i < ppArg[1]->m_ppNodes[0]->m_cNodes; i++)
        {
            pNode->m_ppNodes[0]->m_ppNodes[i + ppArg[0]->m_cNodes] = ppArg[1]->m_ppNodes[0]->m_ppNodes[i];
            ppArg[1]->m_ppNodes[0]->m_ppNodes[i] = NULL;
        }

        for(i = 1; i < ppArg[1]->m_cNodes; i++)
        {
            pNode->m_ppNodes[i] = ppArg[1]->m_ppNodes[i];
            ppArg[1]->m_ppNodes[i] = NULL;
        }

        break;


    case P_PASSES0:
        if(FAILED(CreateNode(N_INTERNAL, 0, 1, &pNode)))
            break;

        pNode->m_ppNodes[0] = ppArg[0];
        ppArg[0] = NULL;

        break;

    case P_PASSES1:
        if(FAILED(CreateNode(N_INTERNAL, 0, 1 + ppArg[1]->m_cNodes, &pNode)))
            break;

        pNode->m_ppNodes[0] = ppArg[0];
        ppArg[0] = NULL;

        for(i = 0; i < ppArg[1]->m_cNodes; i++)
        {
            pNode->m_ppNodes[i + 1] = ppArg[1]->m_ppNodes[i];
            ppArg[1]->m_ppNodes[i] = NULL;
        }

        break;


    case P_PASS0:
        if(FAILED(CreateNode(N_PASS, 1, ppArg[0]->m_cNodes, &pNode)))
            break;

        pNode->m_pdwData[0] = 0;

        for(i = 0; i < ppArg[0]->m_cNodes; i++)
        {
            pNode->m_ppNodes[i] = ppArg[0]->m_ppNodes[i];
            ppArg[0]->m_ppNodes[i] = NULL;
        }

        break;

    case P_PASS1:
        if(FAILED(CreateNode(N_PASS, 1, ppArg[1]->m_cNodes, &pNode)))
            break;

        pNode->m_pdwData[0] = ppArg[0]->m_pdwData[0];

        for(i = 0; i < ppArg[1]->m_cNodes; i++)
        {
            pNode->m_ppNodes[i] = ppArg[1]->m_ppNodes[i];
            ppArg[1]->m_ppNodes[i] = NULL;
        }

        break;


    case P_PASS_BODY0:
        if(FAILED(CreateNode(N_INTERNAL, 0, 0, &pNode)))
            break;

        break;

    case P_PASS_BODY1:
        pNode = ppArg[0];
        ppArg[0] = NULL;
        break;


    case P_ASSIGNMENTS0:
        if(FAILED(CreateNode(N_INTERNAL, 0, 1, &pNode)))
            break;

        pNode->m_ppNodes[0] = ppArg[0];
        ppArg[0] = NULL;

        break;

    case P_ASSIGNMENTS1:
        if(FAILED(CreateNode(N_INTERNAL, 0, 1 + ppArg[1]->m_cNodes, &pNode)))
            break;

        pNode->m_ppNodes[0] = ppArg[0];
        ppArg[0] = NULL;

        for(i = 0; i < ppArg[1]->m_cNodes; i++)
        {
            pNode->m_ppNodes[i + 1] = ppArg[1]->m_ppNodes[i];
            ppArg[1]->m_ppNodes[i] = NULL;
        }

        break;


    case P_ASSIGNMENT0:
        if(FAILED(CreateNode(N_ASSIGNMENT, 3, 1, &pNode)))
            break;

        pNode->m_pdwData[0] = ppArg[0]->m_pdwData[0];
        pNode->m_pdwData[1] = ppArg[0]->m_pdwData[1];
        pNode->m_pdwData[2] = ppArg[0]->m_pdwData[2];

        pNode->m_ppNodes[0] = ppArg[1];
        ppArg[1] = NULL;

        if(FAILED(EvalExpression(pNode->m_pdwData[1], &pNode->m_ppNodes[0])))
            break;

        if(FAILED(ValidAssignment(pNode->m_pdwData[0], pNode->m_ppNodes[0]->m_pdwData[0])))
            break;

        break;
        
    case P_ASSIGNMENT1:
        if(FAILED(CreateNode(N_ASSIGNMENT, 3, 1, &pNode)))
            break;

        pNode->m_pdwData[0] = ppArg[0]->m_pdwData[0];
        pNode->m_pdwData[1] = ppArg[0]->m_pdwData[1];
        pNode->m_pdwData[2] = ppArg[0]->m_pdwData[2];

        pNode->m_ppNodes[0] = ppArg[1];
        ppArg[1] = NULL;

        if(FAILED(ValidAssignment(pNode->m_pdwData[0], pNode->m_ppNodes[0]->m_pdwData[0])))
            break;

        break;

        
    case P_LVALUE0:
        if(FAILED(CreateNode(N_INTERNAL, 3, 0, &pNode)))
            break;

        if(FAILED(LookupState((char *) ppArg[0]->m_pdwData, 0, 
            &pNode->m_pdwData[0], &pNode->m_pdwData[1])))
        {
           break;
        }

        pNode->m_pdwData[2] = 0;
        break;

    case P_LVALUE1:
        if(FAILED(CreateNode(N_INTERNAL, 3, 0, &pNode)))
            break;

        if(FAILED(LookupState((char *) ppArg[0]->m_pdwData, ppArg[1]->m_pdwData[1], 
            &pNode->m_pdwData[0], &pNode->m_pdwData[1])))
        {
            break;
        }

        pNode->m_pdwData[2] = ppArg[1]->m_pdwData[1];
        break;


    case P_EXPRESSION0:
        pNode = ppArg[0];
        ppArg[0] = NULL;
        break;

    case P_EXPRESSION1:
        if(FAILED(CreateNode(ppArg[0]->m_dwType, ppArg[0]->m_cdwData, 1, &pNode)))
            break;

        for(i = 0; i < ppArg[0]->m_cdwData; i++)
            pNode->m_pdwData[i] = ppArg[0]->m_pdwData[i];

        pNode->m_ppNodes[0] = ppArg[1];
        ppArg[1] = NULL;
        break;


    case P_REFERENCE0:
        if(FAILED(CreateNode(N_REFERENCE, 2, 0, &pNode)))
            break;

        if(FAILED(LookupParameter(ppArg[0]->m_pdwData[0], &pNode->m_pdwData[0])))
            break;

        pNode->m_pdwData[1] = ppArg[0]->m_pdwData[0];
        break;


    case P_CONST_DWORD:
    case P_CONST_FLOAT:
    case P_CONST_VECTOR:
    case P_CONST_MATRIX:
    case P_CONST_VERTEXSHADER:
    case P_CONST_PIXELSHADER:
        pNode = ppArg[0];
        ppArg[0] = NULL;
        break;


    case P_VALUE0:
    case P_VALUE1:
        pNode = ppArg[0];
        ppArg[0] = NULL;
        break;


    case P_DWORD0:
        if(FAILED(CreateNode(N_VALUE, 2, 0, &pNode)))
            break;

        pNode->m_pdwData[0] = D3DXPT_DWORD;
        sscanf(m_szTok, "%d", &pNode->m_pdwData[1]);
        break;

    case P_DWORD1:
        if(FAILED(CreateNode(N_VALUE, 2, 0, &pNode)))
            break;

        pNode->m_pdwData[0] = D3DXPT_DWORD;
        sscanf(m_szTok, "%d", &pNode->m_pdwData[1]);
        pNode->m_pdwData[1] = 0 - pNode->m_pdwData[1];
        break;

    case P_DWORD2:
        if(FAILED(CreateNode(N_VALUE, 2, 0, &pNode)))
            break;

        pNode->m_pdwData[0] = D3DXPT_DWORD;
        sscanf(m_szTok, "%x", &pNode->m_pdwData[1]);
        break;

    case P_DWORD3:
        if(FAILED(CreateNode(N_VALUE, 2, 0, &pNode)))
            break;

        pNode->m_pdwData[0] = D3DXPT_DWORD;
        sscanf(m_szTok, "%x", &pNode->m_pdwData[1]);
        pNode->m_pdwData[1] = 0 - pNode->m_pdwData[1];
        break;


    case P_FLOAT0:
        if(FAILED(CreateNode(N_VALUE, 2, 0, &pNode)))
            break;

        pNode->m_pdwData[0] = D3DXPT_FLOAT;
        sscanf(m_szTok, "%f", &pNode->m_pdwData[1]);
        break;

    case P_FLOAT1:
        if(FAILED(CreateNode(N_VALUE, 2, 0, &pNode)))
            break;

        pNode->m_pdwData[0] = D3DXPT_FLOAT;
        sscanf(m_szTok, "%f", &pNode->m_pdwData[1]);
        *((FLOAT *) &pNode->m_pdwData[1]) = - *((FLOAT *) &pNode->m_pdwData[1]);
        break;


    case P_VECTOR0:
        if(FAILED(CreateNode(N_VALUE, 5, 0, &pNode)))
            break;

        pNode->m_pdwData[0] = D3DXPT_VECTOR;
        pNode->m_pdwData[1] = ppArg[0]->m_pdwData[0];

        ((FLOAT *) pNode->m_pdwData)[2] = 0.0f;
        ((FLOAT *) pNode->m_pdwData)[3] = 0.0f;
        ((FLOAT *) pNode->m_pdwData)[4] = 1.0f;
        break;

    case P_VECTOR1:
        if(FAILED(CreateNode(N_VALUE, 5, 0, &pNode)))
            break;

        pNode->m_pdwData[0] = D3DXPT_VECTOR;
        pNode->m_pdwData[1] = ppArg[0]->m_pdwData[0];
        pNode->m_pdwData[2] = ppArg[1]->m_pdwData[0];

        ((FLOAT *) pNode->m_pdwData)[3] = 0.0f;
        ((FLOAT *) pNode->m_pdwData)[4] = 1.0f;
        break;

    case P_VECTOR2:
        if(FAILED(CreateNode(N_VALUE, 5, 0, &pNode)))
            break;

        pNode->m_pdwData[0] = D3DXPT_VECTOR;
        pNode->m_pdwData[1] = ppArg[0]->m_pdwData[0];
        pNode->m_pdwData[2] = ppArg[1]->m_pdwData[0];
        pNode->m_pdwData[3] = ppArg[2]->m_pdwData[0];

        ((FLOAT *) pNode->m_pdwData)[4] = 1.0f;
        break;

    case P_VECTOR3:
        if(FAILED(CreateNode(N_VALUE, 5, 0, &pNode)))
            break;

        pNode->m_pdwData[0] = D3DXPT_VECTOR;
        pNode->m_pdwData[1] = ppArg[0]->m_pdwData[0];
        pNode->m_pdwData[2] = ppArg[1]->m_pdwData[0];
        pNode->m_pdwData[3] = ppArg[2]->m_pdwData[0];
        pNode->m_pdwData[4] = ppArg[3]->m_pdwData[0];
        break;


    case P_MATRIX0:
        if(FAILED(CreateNode(N_VALUE, 17, 0, &pNode)))
            break;

        pNode->m_pdwData[0] = D3DXPT_MATRIX;
        
        for(i = 0; i < 16; i++)
            pNode->m_pdwData[i + 1] = ppArg[i]->m_pdwData[0];

        break;


    case P_VERTEXSHADER0:
        AssembleShader(ppArg[0], NULL, &pNode);
        break;

    case P_VERTEXSHADER1:
        AssembleShader(ppArg[0], ppArg[1], &pNode);
        break;

    case P_PIXELSHADER0:
        AssembleShader(NULL, ppArg[0], &pNode);
        break;


    case P_FOURCC0:
        if(FAILED(CreateNode(N_INTERNAL, 1, 0, &pNode)))
            break;

        if(FAILED(MakeFourCC((char *) ppArg[0]->m_pdwData, &pNode->m_pdwData[0])))
            break;

        break;


    case P_ID0:
        {
            UINT cb  = strlen(m_szTok) + 1;
            UINT cdw = (cb + 3) >> 2;

            if(FAILED(CreateNode(N_INTERNAL, cdw, 0, &pNode)))
                break;

            memset(pNode->m_pdwData, 0x00, cdw * sizeof(DWORD));
            memcpy(pNode->m_pdwData, m_szTok, cb);
        }

        break;


    case P_DECL0:
    case P_ASM0:
        {
            UINT cb  = m_cchAsm + 1;
            UINT cdw = (cb + 3) >> 2;

            if(FAILED(CreateNode(N_INTERNAL, cdw, 0, &pNode)))
                break;

            memset(pNode->m_pdwData, 0x00, cdw * sizeof(DWORD));
            memcpy(pNode->m_pdwData, m_pchAsm, m_cchAsm);
        }

        break;
    }


    // Push the new node on the stack
    if(!m_bFail)
    {
        if(pNode)
            Push(pNode);
        else
            Error("Internal compiler error! %d", __LINE__);
    }


LDone:
    // Delete arguments
    for(i = 0; i < uArgs; i++)
    {
        if(ppArg[i])
            delete ppArg[i];
    }

    return;
}



void
CD3DXEffectCompiler::Push(CNode *pNode)
{
    if(!pNode || pNode->m_pStack)
    {
        Error("Internal compiler error! %d", __LINE__);
        return;
    }

    pNode->m_pStack = m_pEffect;
    m_pEffect = pNode;
    return;
}


CNode *
CD3DXEffectCompiler::Pop()
{
    if(!m_pEffect)
    {
        Error("Internal compiler error! %d", __LINE__);
        return NULL;
    }

    CNode *pNode = m_pEffect;
    m_pEffect = pNode->m_pStack;
    pNode->m_pStack = NULL;
    return pNode;
}


UINT 
CD3DXEffectCompiler::Serialize(CNode *pNode, LPVOID pv)
{
    UINT   cdw = 0;
    DWORD* pdw = (DWORD *) pv;
    
    if(!pNode)
        return 0;


    // Header
    if(pdw)
    {
        pdw[0] = pNode->m_dwType;
        pdw[1] = (DWORD) pNode->m_cdwData;
        pdw[2] = 0; // Filled in later;

        pdw += 3;
    }

    cdw += 3;


    // Data
    if(pdw)
    {
        memcpy(pdw, pNode->m_pdwData, pNode->m_cdwData * sizeof(DWORD));
        pdw += pNode->m_cdwData;
    }

    cdw += pNode->m_cdwData;


    // Nodes
    for(UINT i = 0; i < pNode->m_cNodes; i++)
    {
        UINT cdwNode;

        cdwNode = Serialize(pNode->m_ppNodes[i], pdw);

        if(pdw)
            pdw += cdwNode;

        cdw += cdwNode;
    }

    if(pdw)
        ((DWORD *) pv)[2] = (DWORD) (cdw - pNode->m_cdwData - 3);

    return cdw;
}


void
CD3DXEffectCompiler::Error(char *szFormat, ...)
{
    char szA[256];
    char szB[256];


    // Format error
    va_list ap;
    va_start(ap, szFormat);
    _vsnprintf(szA, sizeof(szA), szFormat, ap);
    szA[255] = '\0';
    va_end(ap);

    _snprintf(szB, sizeof(szB), "%s(%d) : %s", m_szFile, m_iLine, szA);
    szB[255] = '\0';


    // Output error in debug spew
    DPF(0, "%s", szB);
    AppendError(szB);
    m_bFail = TRUE;
}


void
CD3DXEffectCompiler::AppendError(char *sz)
{
    UINT cch = strlen(sz);

    if(cch && sz[cch - 1] == '\n')
        cch--;

    if(!cch)
        return;


    CNode *pNode = new CNode;

    if(pNode && SUCCEEDED(pNode->Initialize(0, (cch + 4) >> 2, 0)))
    {
        memcpy(pNode->m_pdwData, sz, cch);
        ((char *) pNode->m_pdwData)[cch] = 0;

        pNode->m_pStack = m_pErrors;
        m_pErrors = pNode;
    }
}


HRESULT 
CD3DXEffectCompiler::CreateNode(DWORD dwId, UINT cdwData, UINT cNodes, CNode **ppNode)
{
    CNode *pNode;

    D3DXASSERT(ppNode != NULL);

    if(!(pNode = new CNode))
    {
        Error("Out of memory!");
        return E_OUTOFMEMORY;
    }

    if(FAILED(pNode->Initialize(dwId, cdwData, cNodes)))
    {
        delete pNode;
        Error("Out of memory!");
        return E_OUTOFMEMORY;
    }

    *ppNode = pNode;
    return S_OK;
}


HRESULT 
CD3DXEffectCompiler::EvalExpression(DWORD dwState, CNode **ppNode)
{
    CNode *pNode = *ppNode;

    if((N_VALUE == pNode->m_dwType) && (0 == pNode->m_cNodes))
        return S_OK;

    DWORD dwType  = D3DXPT_DWORD;
    DWORD dwValue = 0;


    // Find RValue set for this state
    RValue *prvState;
    LValue *plv;

    for(plv = g_lv; plv->szName; plv++)
    {
        if(plv->dwState == dwState)
            break;
    }

    if(!plv->szName)
    {
        Error("Internal compiler error! %d", __LINE__);
        return E_FAIL;
    }

    prvState = plv->pRValue;


    // Loop thru expression
    while(pNode)
    {
        if(pNode->m_dwType == N_VALUE)
        {
            switch(pNode->m_pdwData[0])
            {
            case D3DXPT_DWORD:
                dwValue |= pNode->m_pdwData[1];
                break;

            case D3DXPT_VECTOR:                
                dwValue |= (DWORD) D3DXCOLOR((FLOAT *) &pNode->m_pdwData[1]);
                break;

            default:
                Error("Invalid type in DWORD expression");
                return E_FAIL;
            }
        }
        else if(prvState)
        {
            for(RValue *prv = prvState; prv->szName; prv++)
            {
                if(!_stricmp(prv->szName, (char *) pNode->m_pdwData))
                {
                    dwValue |= prv->dwValue;   
                    break;
                }
            }

            if(!prv->szName)
            {
                Error("Unknown value in DWORD expression: '%s'", (char *) pNode->m_pdwData);
                return E_FAIL;
            }
        }

        if(0 == pNode->m_cNodes)
            break;

        pNode = pNode->m_ppNodes[0];
    }

    HRESULT hr;
    if(FAILED(hr = CreateNode(N_VALUE, 2, 0, &pNode)))
        return hr;

    pNode->m_pdwData[0] = dwType;
    pNode->m_pdwData[1] = dwValue;

    delete *ppNode;
    *ppNode = pNode;

    return S_OK;
}


HRESULT 
CD3DXEffectCompiler::ValidAssignment(DWORD dwDstType, DWORD dwSrcType)
{
    static const BOOL bDstSrc[8][8] =
    {        
    //    Src                       Dest
    //    D  F  V  M  T  V  P
        { 1, 0, 1, 0, 0, 0, 0, 0 }, // DWORD
        { 0, 1, 0, 0, 0, 0, 0, 0 }, // FLOAT  
        { 1, 1, 1, 0, 0, 0, 0, 0 }, // VECTOR 
        { 0, 0, 0, 1, 0, 0, 0, 0 }, // MATRIX 
        { 0, 0, 0, 0, 1, 0, 0, 0 }, // TEXTURE
        { 0, 0, 0, 0, 0, 1, 0, 0 }, // VERTEXSHADER
        { 0, 0, 0, 0, 0, 0, 1, 0 }, // PIXELSHADER
        { 1, 1, 1, 1, 0, 0, 0, 0 }, // CONSTANT
    };


    if(!bDstSrc[dwDstType][dwSrcType])
    {
        static const char *szName[8] =
        {
            "DWORD",
            "FLOAT",
            "VECTOR",
            "MATRIX",
            "TEXTURE",
            "VERTEXSHADER",
            "PIXELSHADER",
            "CONSTANT"
        };

        Error("Invalid assignment: %s = %s", szName[dwDstType], szName[dwSrcType]);
        return E_FAIL;
    }
    
    return S_OK;
}


HRESULT
CD3DXEffectCompiler::LookupType(char *psz, DWORD *pdwType)
{
    for(RValue *prv = g_rvType; prv->szName; prv++)
    {
        if(!_stricmp(psz, prv->szName))
        {
            if(pdwType)
                *pdwType = prv->dwValue;

            return S_OK;
        }
    }

    Error("Unknown type in parameter declaration: '%s'", psz);
    return E_FAIL;
}


HRESULT 
CD3DXEffectCompiler::LookupState(char *psz, DWORD dwIndex, DWORD *pdwType, DWORD *pdwState)
{
    for(LValue *plv = g_lv; plv->szName; plv++)
    {
        if(!_stricmp(psz, plv->szName))
        {
            if(dwIndex >= plv->dwSize)
            {
                Error("Invalid index: '%s[%d]' (must be < %d)", psz, dwIndex, plv->dwSize);
                return E_FAIL;
            }

            if(pdwType)
                *pdwType = plv->dwType;

            if(pdwState)
                *pdwState = plv->dwState;

            return S_OK;
        }
    }

    Error("Unknown state '%s'", psz);
    return E_FAIL;
}


HRESULT 
CD3DXEffectCompiler::LookupParameter(DWORD dwName, DWORD *pdwType)
{
    // Find name in parameter list
    if(m_pParameters)
    {
        for(UINT iNode = 0; iNode < m_pParameters->m_cNodes; iNode++)
        {
            if(m_pParameters->m_ppNodes[iNode]->m_pdwData[1] == dwName)
            {
                if(pdwType)
                    *pdwType = m_pParameters->m_ppNodes[iNode]->m_pdwData[0];

                return S_OK;
            }
        }
    }

    Error("Undeclared parameter: '%.4s'", &dwName);
    return E_FAIL;
}


HRESULT 
CD3DXEffectCompiler::UnusedParameter(DWORD dwName)
{
    // Find name in parameter list
    if(m_pParameters)
    {
        for(UINT iNode = 0; iNode < m_pParameters->m_cNodes; iNode++)
        {
            if(m_pParameters->m_ppNodes[iNode]->m_pdwData[1] == dwName)
            {
                Error("Parameter declared more than once: '%.4s'", &dwName);
                return S_OK;
            }
        }
    }

    return S_OK;
}


HRESULT 
CD3DXEffectCompiler::MakeFourCC(char *psz, DWORD *pdwFourCC)
{
    D3DXASSERT(pdwFourCC != NULL);

    char ch[4];
    UINT cch = strlen(psz);

    if(cch > 4)
    {
        Error("Invalid FourCC: '%s'", psz);
        return E_FAIL;
    }

    memset(ch, 0x00, 4);
    memcpy(ch, psz, cch);

    *pdwFourCC = MAKEFOURCC(ch[0], ch[1], ch[2], ch[3]);
    return S_OK;
}


HRESULT 
CD3DXEffectCompiler::AssembleShader(CNode *pDeclNode, CNode *pFuncNode, CNode **ppNode)
{
    return E_NOTIMPL;

    HRESULT hr;

    LPD3DXBUFFER pDecl   = NULL;
    LPD3DXBUFFER pd3dxErrors = NULL;
    LPXGBUFFER pConst  = NULL;
    LPXGBUFFER pFunc   = NULL;
    LPXGBUFFER pErrors = NULL;

    CNode *pNode;

    DWORD cdwDecl, cdwConst, cdwFunc;
    DWORD *pdwDecl, *pdwConst, *pdwFunc;

    // Assemble declaration
    if(pDeclNode)
    {
        char *szDecl = (char *) pDeclNode->m_pdwData;
        UINT cchDecl = strlen(szDecl);

        CD3DXDeclarationCompiler compiler;

        hr = compiler.Compile(szDecl, cchDecl, m_szFileAsm, m_iLineAsm, &pDecl, &pd3dxErrors);

        if(pd3dxErrors)
        {
            AppendError((char *) pd3dxErrors->GetBufferPointer());
            RELEASE(pErrors);
        }

        if(FAILED(hr))
        {
            m_bFail = TRUE;
            goto LDone;
        }
    }


    // Assemble shader
    if(pFuncNode)
    {
        char *szFunc = (char *) pFuncNode->m_pdwData;
        UINT cchFunc = strlen(szFunc);

        XGAssembleShader(
                m_szFileAsm,
                szFunc,
                cchFunc,
                SASM_DEBUG,
                &pConst,
                &pFunc,
                &pErrors,
                NULL,
                NULL,
                NULL,
                NULL);


        if(pErrors)
        {
            AppendError((char *) pErrors->GetBufferPointer());
            RELEASE(pErrors);
        }

        if(FAILED(hr))
        {
            m_bFail = TRUE;
            goto LDone;
        }
    }


    // Create shader node
    cdwDecl  = pDecl  ? (pDecl ->GetBufferSize() >> 2) : 0;
    cdwConst = pConst ? (pConst->GetBufferSize() >> 2) : 0;
    cdwFunc  = pFunc  ? (pFunc ->GetBufferSize() >> 2) : 0;

    pdwDecl  = pDecl  ? (DWORD *) pDecl ->GetBufferPointer() : NULL;
    pdwConst = pConst ? (DWORD *) pConst->GetBufferPointer() : NULL;
    pdwFunc  = pFunc  ? (DWORD *) pFunc ->GetBufferPointer() : NULL;

    if(cdwConst && !pDeclNode)
    {
        Error("Constant definitions in pixel shaders currently unsupported.");
        hr = E_FAIL;
        goto LDone;
    }


    if(FAILED(hr = CreateNode(N_VALUE, 2 + cdwDecl + cdwConst + cdwFunc, 0, &pNode)))
        goto LDone;


    pNode->m_pdwData[0] = pDeclNode ? D3DXPT_VERTEXSHADER : D3DXPT_PIXELSHADER;
    pNode->m_pdwData[1] = (DWORD) (cdwDecl + cdwConst);

    memcpy(&pNode->m_pdwData[2], pdwDecl, cdwDecl << 2);
    memcpy(&pNode->m_pdwData[2 + cdwDecl], pdwConst, cdwConst << 2);
    memcpy(&pNode->m_pdwData[2 + cdwDecl + cdwConst], pdwFunc, cdwFunc << 2);

    *ppNode = pNode;

    hr = S_OK;
    goto LDone;


LDone:
    RELEASE(pDecl);
    RELEASE(pConst);
    RELEASE(pFunc);
    RELEASE(pErrors);
    return hr;
}


