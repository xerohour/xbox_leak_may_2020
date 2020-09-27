//=============================================================================
// File: XImage.h
//
// Desc: 
// Created: 07/12/2001 by Michael Lyons (mlyons@microsoft.com)
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//=============================================================================

#pragma once


//=============================================================================
// include files
//=============================================================================
#include <xtl.h>
#include <assert.h>

//=============================================================================
//=============================================================================
typedef struct tagXIMAGEVERTEX
{
    float x,y,z,w;
    float u,v;
} XIMAGEVERTEX;

#define D3DFVF_XIMAGEVERTEX (D3DFVF_XYZRHW|D3DFVF_TEX1)

//=============================================================================
//=============================================================================
class XImage
{
public:
	XImage();

	IDirect3DTexture8 *		m_pTexture;
	IDirect3DDevice8 *		m_pDevice;
	int						m_iWidth;
	int						m_iHeight;

	bool					Load(IDirect3DDevice8 *pDevice, char *szFileName);
	bool					Blt(RECT *r=NULL);
	bool					Blt(int x, int y);
	bool					Blt(int x, int y, int w, int h);
	bool					FillArea(int x, int y, int w, int h, DWORD color);
	bool					Free(void);

	static XIMAGEVERTEX		m_ScreenVertices[4];
};



