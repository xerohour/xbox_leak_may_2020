//-----------------------------------------------------------------------------
// File: xfvf.h
//
// Copyright (c) 2000-2001 Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#ifndef __FVF_H
#define __FVF_H

#ifdef _XBOX
#include <xtl.h>
#else
#include <d3d8.h>
#endif

#define FVF_XYZDIFF (D3DFVF_XYZ|D3DFVF_DIFFUSE)
#define FVF_XYZNORM (D3DFVF_XYZ|D3DFVF_NORMAL)
#define FVF_XYZTEX1 (D3DFVF_XYZ|D3DFVF_TEX1)
#define FVF_XYZRHWTEX1 (D3DFVF_XYZRHW|D3DFVF_TEX1)
#define FVF_XYZRHWDIFF (D3DFVF_XYZRHW|D3DFVF_DIFFUSE)
#define FVF_XYZDIFFTEX1 (D3DFVF_XYZ|D3DFVF_DIFFUSE|D3DFVF_TEX1)

#define FVF_XYZNORMTEX1 (D3DFVF_XYZ|D3DFVF_NORMAL|D3DFVF_TEX1)
#define FVF_XYZNORMDIFF (D3DFVF_XYZ|D3DFVF_NORMAL|D3DFVF_DIFFUSE)
#define FVF_XYZNORMDIFFTEX1 (D3DFVF_XYZ|D3DFVF_NORMAL|D3DFVF_DIFFUSE|D3DFVF_TEX1)

#ifdef __cplusplus
extern "C" {
#endif

// predefined flexible vertex formats

// D3DFVF_XYZ|D3DFVF_DIFFUSE
typedef struct sFVFT_XYZDIFF
{
	D3DVECTOR v;
	DWORD diff;
} FVFT_XYZDIFF;

// D3DFVF_XYZ|D3DFVF_NORMAL
typedef struct sFVFT_XYZNORM
{
	D3DVECTOR v;
	D3DVECTOR norm;
} FVFT_XYZNORM;

// D3DFVF_XYZ|D3DFVF_TEX1
typedef struct sFVFT_XYZTEX1
{
	D3DVECTOR v;
	float tu;
	float tv;
} FVFT_XYZTEX1;

// D3DFVF_XYZRHW|D3DFVF_TEX1
typedef struct sFVFT_XYZRHWTEX1
{
	D3DVECTOR v;
	float w;
	float tu;
	float tv;
} FVFT_XYZRHWTEX1;

// D3DFVF_XYZRHW|D3DFVF_TEX1
typedef struct sFVFT_XYZRHWDIFF
{
	D3DVECTOR v;
	float w;
	DWORD diff;
} FVFT_XYZRHWDIFF;

// D3DFVF_XYZ|D3DFV_DIFFUSE|D3DFVF_TEX1
typedef struct sFVFT_XYZDIFFTEX1
{
	D3DVECTOR v;
	DWORD diff;
	float tu;
	float tv;
} FVFT_XYZDIFFTEX1;

// D3DFVF_XYZ|D3DFVF_NORMAL|D3DFVF_TEX1
typedef struct sFVFT_XYZNORMTEX1
{
	D3DVECTOR v;
	D3DVECTOR norm;
	float tu;
	float tv;
} FVFT_XYZNORMTEX1;

// D3DFVF_XYZ|D3DFVF_NORMAL|D3DFVF_DIFFUSE
typedef struct sFVFT_XYZNORMDIFF
{
	D3DVECTOR v;
	D3DVECTOR norm;
	DWORD diff;
} FVFT_XYZNORMDIFF;

// D3DFVF_XYZ|D3DFVF_NORMAL|D3DFVF_DIFFUSE|D3DFVF_TEX1
typedef struct sFVFT_XYZNORMDIFFTEX1
{
	D3DVECTOR v;
	D3DVECTOR norm;
	DWORD diff;
	float tu;
	float tv;
} FVFT_XYZNORMDIFFTEX1;

DWORD FVF_VSize(DWORD fvf);

void FVF_GetVert(void *verts, D3DVECTOR *vout, DWORD first, DWORD cnt, DWORD fvf);
void FVF_GetNorm(void *verts, D3DVECTOR *nout, DWORD first, DWORD cnt, DWORD fvf);
void FVF_GetDiff(void *verts, DWORD *dout, DWORD first, DWORD cnt, DWORD fvf);
void FVF_GetUV(void *verts, float *uvout, DWORD first, DWORD cnt, DWORD fvf);

void FVF_SetVert(void *verts, D3DVECTOR *vin, DWORD first, DWORD cnt, DWORD fvf);
void FVF_SetNorm(void *verts, D3DVECTOR *nin, DWORD first, DWORD cnt, DWORD fvf);
void FVF_SetDiff(void *verts, DWORD *din, DWORD first, DWORD cnt, DWORD fvf);
void FVF_SetAllDiff(void *verts, DWORD din, DWORD first, DWORD cnt, DWORD fvf);
void FVF_SetUV(void *verts, float *uvin, DWORD first, DWORD cnt, DWORD fvf);

#ifdef __cplusplus
}
#endif

#endif
