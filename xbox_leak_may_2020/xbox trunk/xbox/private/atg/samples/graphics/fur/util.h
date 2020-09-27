//-----------------------------------------------------------------------------
// File: util.h
// Desc: Handy utility functions
// Copyright (c) 2001 Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#include <xtl.h>

//-----------------------------------------------------------------------------
//  Find the bounding sphere for an array of vertices
//-----------------------------------------------------------------------------
void UnionSphere(D3DXVECTOR3 *pvCenter, float *pfRadius, const D3DXVECTOR3 &vCenter, const float &fRadius);
HRESULT ComputeBoundingSphere(
        PVOID pPointsFVF, 
        DWORD NumVertices, 
        DWORD FVF,
        D3DXVECTOR3 *pvCenter, 
        FLOAT *pfRadius);

//-----------------------------------------------------------------------------
//  Find the bounding box for an array of vertices
//-----------------------------------------------------------------------------
HRESULT ComputeBoundingBox( PVOID pPoints,
							DWORD VertexCount,
							DWORD VertexSize,
							const D3DXMATRIX *pMatrix,	// may be NULL
							D3DXVECTOR3 *pvMin,
							D3DXVECTOR3 *pvMax );

//-----------------------------------------------------------------------------
// Set state and restore on scope exit
//-----------------------------------------------------------------------------
#define SET_D3DTSS(STAGE, NAME, VALUE)\
	struct _D3DTSS_STAGE##STAGE##_##NAME {\
 		DWORD dw; \
		 _D3DTSS_STAGE##STAGE##_##NAME() { /* constructor is called to save current value and set new value */ \
 			g_pd3dDevice->GetTextureStageState(STAGE, NAME, &dw);\
			g_pd3dDevice->SetTextureStageState(STAGE, NAME, VALUE);\
		}\
		 ~_D3DTSS_STAGE##STAGE##_##NAME() { /* on scope exit, saved value is restored */ \
			 g_pd3dDevice->SetTextureStageState(STAGE, NAME, dw);\
		}\
	} __D3DTSS_STAGE##STAGE##_##NAME
#define SET_D3DTEXTURE(STAGE, TEXTURE)\
	struct _D3DTEXTURE_STAGE##STAGE {\
 		D3DBaseTexture *__pTexture; \
		 _D3DTEXTURE_STAGE##STAGE(D3DBaseTexture *pTexture) { /* constructor is called to save current value and set new value */ \
 			g_pd3dDevice->GetTexture(STAGE, &__pTexture);\
			g_pd3dDevice->SetTexture(STAGE, pTexture);\
		}\
		 ~_D3DTEXTURE_STAGE##STAGE() { /* on scope exit, saved value is restored */ \
			 g_pd3dDevice->SetTexture(STAGE, __pTexture);\
			 if (__pTexture != NULL) __pTexture->Release();\
		}\
	} __D3DTEXTURE_STAGE##STAGE(TEXTURE)
#define SET_D3DRS(NAME, VALUE)\
	struct _D3DRS_##NAME {\
 		DWORD dw; \
		 _D3DRS_##NAME() { /* constructor is called to save current value and set new value */ \
 			g_pd3dDevice->GetRenderState(NAME, &dw);\
			g_pd3dDevice->SetRenderState(NAME, VALUE);\
		}\
		 ~_D3DRS_##NAME() { /* on scope exit, saved value is restored */ \
			 g_pd3dDevice->SetRenderState(NAME, dw);\
		}\
	} __D3DRS_##NAME
#define SET_D3DVS(VALUE)\
	struct _D3DVS {\
 		DWORD dw; \
		 _D3DVS() { /* constructor is called to save current value and set new value */ \
 			g_pd3dDevice->GetVertexShader(&dw);\
			g_pd3dDevice->SetVertexShader(VALUE);\
		}\
		 ~_D3DVS() { /* on scope exit, saved value is restored */ \
			 g_pd3dDevice->SetVertexShader(dw);\
		}\
	} __D3DVS
