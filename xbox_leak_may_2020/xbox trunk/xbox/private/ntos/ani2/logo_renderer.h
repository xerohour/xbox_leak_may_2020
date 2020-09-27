///////////////////////////////////////////////////////////////////////////////
// File: logo_renderer.h
//
// Copyright 2001 Pipeworks Software
///////////////////////////////////////////////////////////////////////////////
#ifndef __LOGO_RENDERER_H__
#define __LOGO_RENDERER_H__

class LogoRenderer
{
private:

	LPDIRECT3DVERTEXBUFFER8  pLipVB;
	WORD                    *pLipIndices;
	DWORD					 nLipVerts;
	DWORD					 nLipIndices;		
	LPDIRECT3DTEXTURE8       pLipTex;

	LPDIRECT3DVERTEXBUFFER8  pSurfaceVB;
	WORD					*pSurfaceIndices;
	DWORD					 nSurfaceVerts;
	DWORD					 nSurfaceIndices;
	LPDIRECT3DTEXTURE8       pSurfaceTex;
	

	LPDIRECT3DVERTEXBUFFER8  pSurfaceTopVB;
	WORD					*pSurfaceTopIndices;
	DWORD					 nSurfaceTopVerts;
	DWORD					 nSurfaceTopIndices;
	LPDIRECT3DTEXTURE8       pSurfaceTopTex;

	LPDIRECT3DVERTEXBUFFER8  pInteriorVB;
	WORD					*pInteriorIndices;
	DWORD					 nInteriorVerts;
	DWORD					 nInteriorIndices;

	bool					 bRenderText;

	LPDIRECT3DVERTEXBUFFER8  pText_VB;
	WORD					*pText_Indices;
	DWORD					 nText_Verts;
	DWORD					 nText_Indices;
	D3DMATRIX				 matText_Anim;

	LPDIRECT3DVERTEXBUFFER8  pSlashTM_VB;
	WORD					*pSlashTM_Indices;
	DWORD					 nSlashTM_Verts;
	DWORD					 nSlashTM_Indices;

	LPDIRECT3DVERTEXBUFFER8  pTextTM_VB;
	WORD					*pTextTM_Indices;
	DWORD					 nTextTM_Verts;
	DWORD					 nTextTM_Indices;

	LPDIRECT3DTEXTURE8       pTMTex;
	float					 fTMAlpha;

	DWORD					 dwInteriorPShader;
	DWORD					 dwInteriorVShader;

	void createSlash();
	void createText();
	void createTMs();


	LPDIRECT3DTEXTURE8		pSlashTexture;
	bool					bHasWordmark;
	void createSlashSurface();	// don't call until camera is in final position


	// Decompressed slash data.
	WORD		* indices_xboxlogolip_0;
	float		* verts_xboxlogolip_0;
	WORD		* indices_xboxlogosurface_0;
	float		* verts_xboxlogosurface_0;
	WORD		* indices_xboxlogosurfacetop_0;
	float	    * verts_xboxlogosurfacetop_0;
	WORD        * indices_xboxlogointerior_0;
	float		* verts_xboxlogointerior_0;
	WORD		* indices_tm_wordmark_0;
	float		* verts_tm_wordmark_0;
	WORD		* indices_tm_slash_0;
	float		* verts_tm_slash_0;

	// Decompressed wordmark data.
	WORD		* indices_text_0;
	float		* verts_text_0;
	
	void decompressData();

	float * decompressPosData(short *pdata,int ncount,float f_oo_pos_scale,float f_pos_delta );
	float * decompressPosTexData(short *pdata,int ncount,float f_oo_pos_scale,float f_pos_delta,
														 float f_oo_tex_scale,float f_tex_delta );
	WORD  * decompressIndexData(char *pdata,int ncount);

public:

	void create();
	void destroy();

	void advanceTime(float fElapsedTime, float fDt);
	void render(const D3DMATRIX &mat_otw, bool b_force_3d_render = false);
};

#endif // __LOGO_RENDERER_H__
