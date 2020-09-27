//-----------------------------------------------------------------------------
// File: util.h
// Desc: Handy utility functions
// Copyright (c) 2001 Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#include <xtl.h>

void UnionSphere(D3DXVECTOR3 *pvCenter, float *pfRadius, const D3DXVECTOR3 &vCenter, const float &fRadius);
HRESULT ComputeBoundingSphere(
        PVOID pPointsFVF, 
        DWORD NumVertices, 
        DWORD FVF,
        D3DXVECTOR3 *pvCenter, 
        FLOAT *pfRadius);

HRESULT DrawVertexVectors(DWORD nVertex,        // number of lines to draw
                          LPDIRECT3DVERTEXBUFFER8 pVBPosition, DWORD cbVertexSize, DWORD cbPositionOffset,  // origin of lines
                          LPDIRECT3DVERTEXBUFFER8 pVBVector, DWORD cbVectorSize, DWORD cbVectorOffset,      // offset of lines
                          float fVectorScale,   // scale
                          DWORD color0,         // color at base
                          DWORD color1);        // color at tip
HRESULT DrawNormals(DWORD nVertex, LPDIRECT3DVERTEXBUFFER8 pVB, DWORD dwFVF, float fNormalScale);

HRESULT DrawAxes();

void PrepareDeviceForDrawSprite(IDirect3DDevice8* pDevice);
void DrawSpriteSimple(LPDIRECT3DDEVICE8 pDevice,
					  LPDIRECT3DTEXTURE8 pTexture, 
					  const D3DXVECTOR3 &ptDest,
					  float alpha,
					  float scale,
					  const RECT *pSourceRect);

WCHAR *GetString_D3DTEXTUREOP(DWORD d3dtop);
