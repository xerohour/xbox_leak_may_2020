///////////////////////////////////////////////////////////////////////////////
// File: GreenFog.h
//
// Copyright 2001 Pipeworks Software
///////////////////////////////////////////////////////////////////////////////
#ifndef GREENFOG_H
#define GREENFOG_H



class GreenFog
{
public:
	void Init();
	void UnInit() { destroy(); }

	
	void create();
	void destroy();

	void advanceTime(float fElapsedTime, float fDt);

	void render(bool b_cmp_to_one);

	void restart();


	LPDIRECT3DVERTEXBUFFER8 borrowScreenQuad() { return pQuadVB; }

protected:
	enum { NUM_PLASMAS = 3 };
	LPDIRECT3DVERTEXBUFFER8		pQuadVB;
	LPDIRECT3DVERTEXBUFFER8		pIntensityQuadVB;
	LPDIRECT3DTEXTURE8			pPlasmaTexture[NUM_PLASMAS];
	LPDIRECT3DTEXTURE8			pIntensityTextureR;		// for rendering (swapped in render)
	LPDIRECT3DTEXTURE8			pIntensityTextureU;		// for updating
	LPDIRECT3DSURFACE8			pIntensityZ;

	DWORD		dwFogPShader, dwFogVShader;


	void renderIntensityTexture();
};
///////////////////////////////////////////////////////////////////////////////
struct GreenFogVertexBuffer
{
	D3DVECTOR	pos;
	FLOAT		tu0, tv0;	// texture coordinate in intensity lookup
	FLOAT		tu1, tv1;	// texture coordinate in plasma, which wraps
};



#endif // GREENFOG_H
