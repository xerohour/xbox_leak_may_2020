//-----------------------------------------------------------------------------
//  
//  File: texture.h
//  Copyright (C) 2001 Microsoft Corporation
//  All rights reserved.
//  
//  Texture resource description and display.
//-----------------------------------------------------------------------------
 
#define FMT_SWIZZLED   0x1
#define FMT_LINEAR     0x2
#define FMT_COMPRESSED 0x4

const struct TEXTUREFORMAT
{
    WCHAR* name;
    DWORD  id;
    DWORD  type;
} g_TextureFormats[] = 
{
    { L"D3DFMT_A8R8G8B8",        D3DFMT_A8R8G8B8,      FMT_SWIZZLED    },
    { L"D3DFMT_LIN_A8R8G8B8",    D3DFMT_LIN_A8R8G8B8,  FMT_LINEAR      },
    { L"D3DFMT_X8R8G8B8",        D3DFMT_X8R8G8B8,      FMT_SWIZZLED    },
    { L"D3DFMT_LIN_X8R8G8B8",    D3DFMT_LIN_X8R8G8B8,  FMT_LINEAR      },
    { L"D3DFMT_R5G6B5",          D3DFMT_R5G6B5,        FMT_SWIZZLED    },
    { L"D3DFMT_LIN_R5G6B5",      D3DFMT_LIN_R5G6B5,    FMT_LINEAR      },
    { L"D3DFMT_A1R5G5B5",        D3DFMT_A1R5G5B5,      FMT_SWIZZLED    },
    { L"D3DFMT_LIN_A1R5G5B5",    D3DFMT_LIN_A1R5G5B5,  FMT_LINEAR      },
    { L"D3DFMT_A4R4G4B4",        D3DFMT_A4R4G4B4,      FMT_SWIZZLED    },
    { L"D3DFMT_LIN_A4R4G4B4",    D3DFMT_LIN_A4R4G4B4,  FMT_LINEAR      },
    { L"D3DFMT_DXT1",            D3DFMT_DXT1,          FMT_COMPRESSED  },
    { L"D3DFMT_DXT2",            D3DFMT_DXT2,          FMT_COMPRESSED  },
    { L"D3DFMT_DXT4",            D3DFMT_DXT4,          FMT_COMPRESSED  },
    { L"",                       0,                    0               },
};


// Define our vertex format
struct CUSTOMVERTEX
{
    D3DXVECTOR3 p;          // Position
    D3DXVECTOR3 n;          // normal
    D3DCOLOR    diffuse;    // Diffuse color
    FLOAT       tu, tv;     // Texture coordinates
};
#define D3DFVF_CUSTOMVERTEX (D3DFVF_XYZ|D3DFVF_NORMAL|D3DFVF_DIFFUSE|D3DFVF_TEX1)


DWORD exptbl[] = {1,2,4,8,16,32,64,128,256,512,1024,2048,4096,0,0,0,0,0,0};

