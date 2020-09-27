/*******************************************************************************

Copyright (c) 2000 Microsoft Corporation.  All rights reserved.

File Name:

    vigil.h

Description:

    Memory surveillance.

*******************************************************************************/

#ifndef __VIGIL_H__
#define __VIGIL_H__

#define FVF_TVERTEX             (D3DFVF_XYZRHW | D3DFVF_TEX1)

//******************************************************************************
typedef struct _TVERTEX {
    XGVECTOR4 vPosition;
    float       u0, v0;

    _TVERTEX() { }
    _TVERTEX(const D3DVECTOR& v, float fRHW, float _u0, float _v0) { 
        vPosition.x = v.x; vPosition.y = v.y; vPosition.z = v.z; vPosition.w = fRHW;
        u0 = _u0; v0 = _v0;
    }
} TVERTEX, *PTVERTEX;

//******************************************************************************
typedef enum _TEXLAYOUT {
    TL_SWIZZLED = 0,
    TL_LINEAR   = 1,
    TL_TILED    = 2
} TEXLAYOUT, *PTEXLAYOUT;

//******************************************************************************
class CVigil {

protected:

    LPDIRECT3D8                     m_pd3d;
    LPDIRECT3DDEVICE8               m_pDevice;

    DWORD                           m_dwCRCTable[256];
    POINT                           m_wQuadOrder[2][2][5];

    UINT                            m_uNumTiles;
    UINT                            m_uTileDim;

    LPDIRECT3DTEXTURE8*             m_ppd3dt;
    LPVOID                          m_ppvTexture[6];
    D3DTexture                      m_pd3dt[6];
    UINT                            m_uTiledTexIndex[6];
    UINT                            m_uTextureSize;
    LPDIRECT3DVERTEXBUFFER8*        m_ppd3dr;
    UINT                            m_uVertexBufferSize;
    LPWORD*                         m_ppwTile;
    UINT                            m_uNumTileIndexLists;
    UINT                            m_uIndexListSize;
    UINT                            m_uNumTileIndices;

    D3DTexture                      m_d3dtColorBuffer[2];
    TVERTEX                         m_prSwizzledQuad[4];
    TVERTEX                         m_prLinearQuad[4];

    DWORD                           m_dwFrontBufferCRC;
    DWORD                           m_dwBackBufferCRC;
    DWORD                           m_dwDepthBufferCRC;
    LPDWORD                         m_pdwTextureCRC;
    LPDWORD                         m_pdwVertexBufferCRC;
    LPDWORD                         m_pdwIndexListCRC;
#if defined(VIGILGENERATECRCS)
    HANDLE                          m_hCRCFile;
#else
    LPDWORD                         m_pdwCRCs;
#endif


    BOOL                            m_b128M;

    TCHAR                           m_szOutput[2304];

public:

                                    CVigil();
                                    ~CVigil();

    virtual BOOL                    Create();
    virtual BOOL                    Run();

    BOOL                            CreateVertexBuffers();
    BOOL                            CreateTextures();
    BOOL                            CreateIndexLists();

    DWORD                           ComputeCRC32(LPVOID pvData, UINT uNumBytes);
    DWORD                           ComputeTextureCRC32(LPDIRECT3DTEXTURE8 pd3dt);
    DWORD                           ComputeSurfaceCRC32(LPDIRECT3DSURFACE8 pd3ds);

    void                            OutputString(LPCTSTR szFormat, ...);
#if VIGILVERBOSE
    void                            OutputStringV(LPCTSTR szFormat, ...);
    inline void                     OutputStringN(LPCTSTR szFormat, ...) {}
#else
    inline void                     OutputStringV(LPCTSTR szFormat, ...) {}
    void                            OutputStringN(LPCTSTR szFormat, ...);
#endif
    BOOL                            ResultFailed(HRESULT hr, LPCTSTR sz);
};

#endif //__VIGIL_H__
