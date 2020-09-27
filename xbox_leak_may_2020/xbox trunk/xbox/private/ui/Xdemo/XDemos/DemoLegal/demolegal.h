#ifndef DEMOLEGAL_H
#define DEMOLEGAL_H

#include "gamepad.h"

#define SCREEN_WIDTH   640
#define SCREEN_HEIGHT  480

#define TEXTUREWIDTH  1024
#define TEXTUREHEIGHT 1024

extern LPDIRECT3D8             g_pD3D;		 // Used to create the D3DDevice
extern LPDIRECT3DDEVICE8       g_pd3dDevice; // Our rendering device
extern LPDIRECT3DVERTEXBUFFER8 g_pVB;		 // Buffer to hold vertices

//-----------------------------------------------------------------------------
// Name: struct CUSTOMVERTEX
// Desc: structure for holding emulator vertex data
//-----------------------------------------------------------------------------
struct CUSTOMVERTEX
{
    FLOAT x, y, z, rhw; 
    DWORD color;        
	FLOAT tu, tv;
};
#define D3DFVF_CUSTOMVERTEX (D3DFVF_XYZRHW|D3DFVF_DIFFUSE|D3DFVF_TEX1)

static const CUSTOMVERTEX g_Vertices[] =
{
    { 0.0f,   0.0f,    0.5f, 1.0f, 0xffffffff, 0.0f, 0.0f}, // x, y, z, rhw, color, tu, tv
    { 640.0f, 0.0f,    0.5f, 1.0f, 0xffffffff, 1.0f, 0.0f},
    { 0.0f,   480.0f,  0.5f, 1.0f, 0xffffffff, 0.0f, 1.0f},
	{ 640.0f, 480.0f,  0.5f, 1.0f, 0xffffffff, 1.0f, 1.0f},
};

#endif