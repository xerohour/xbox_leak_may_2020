///////////////////////////////////////////////////////////////////////////////
// File: GreenFog.cpp
//
// Copyright 2001 Pipeworks Software
///////////////////////////////////////////////////////////////////////////////
#include "precomp.h"
#include "xbs_math.h"
#include "xbs_app.h"
#include "qrand.h"
#include "GreenFog.h"
#include "tex_gen.h"
///////////////////////////////////////////////////////////////////////////////
#define PLASMA_SIZE		(256)
// 1024x512 appears to be only .1 fps slower than 512x256.
#define INTENSITY_TEX_X	(1024)
#define INTENSITY_TEX_Y (512)
//MTS#define INTENSITY_TEX_X	(512)
//MTS#define INTENSITY_TEX_Y (256)
const float MUL_SCALE = 0.005f;
const float MAIN_FOG_RAD = 40.0f;
///////////////////////////////////////////////////////////////////////////////
void GreenFog::Init()
{
	pQuadVB = NULL;
	pIntensityQuadVB = NULL;
	for (int i=0; i<NUM_PLASMAS; i++) pPlasmaTexture[i] = NULL;
	pIntensityTextureU = pIntensityTextureR = NULL;
	pIntensityZ = NULL;
	dwFogPShader = dwFogVShader = 0;
}
///////////////////////////////////////////////////////////////////////////////
void GreenFog::create()
{
	destroy();


	const float f_plasma_dim = ((float)PLASMA_SIZE);

	gpd3dDev->CreateTexture(INTENSITY_TEX_X, INTENSITY_TEX_Y, 1, D3DUSAGE_RENDERTARGET, D3DFMT_A8R8G8B8, 0, &pIntensityTextureU);
	gpd3dDev->CreateTexture(INTENSITY_TEX_X, INTENSITY_TEX_Y, 1, D3DUSAGE_RENDERTARGET, D3DFMT_A8R8G8B8, 0, &pIntensityTextureR);
	gpd3dDev->CreateDepthStencilSurface(INTENSITY_TEX_X,INTENSITY_TEX_Y,D3DFMT_LIN_D24S8, D3DMULTISAMPLE_2_SAMPLES_MULTISAMPLE_LINEAR , &pIntensityZ);



	gpd3dDev->CreateVertexBuffer(4*sizeof(GreenFogVertexBuffer), 0, 0, 0, &pQuadVB);

	GreenFogVertexBuffer* p_verts;
	pQuadVB->Lock(0, 0, (BYTE**) &p_verts, 0);

	Set(&p_verts[0].pos, -1.0f, -1.0f, 1.0f);
	Set(&p_verts[1].pos, -1.0f, +1.0f, 1.0f);
	Set(&p_verts[2].pos, +1.0f, +1.0f, 1.0f);
	Set(&p_verts[3].pos, +1.0f, -1.0f, 1.0f);

	p_verts[0].tu0 = 0.0f;	p_verts[0].tv0 = 1.0f;
	p_verts[1].tu0 = 0.0f;	p_verts[1].tv0 = 0.0f;
	p_verts[2].tu0 = 1.0f;	p_verts[2].tv0 = 0.0f;
	p_verts[3].tu0 = 1.0f;	p_verts[3].tv0 = 1.0f;

	for (int i=0; i<4; i++)
	{
		p_verts[i].tv1 = -(2.0f*p_verts[i].tu0 - 1.0f) * 640.0f/f_plasma_dim;
		p_verts[i].tu1 = -(2.0f*p_verts[i].tv0 - 1.0f) * 480.0f/f_plasma_dim;
//MTS		p_verts[i].tu1 = (2.0f*p_verts[i].tu0 - 1.0f) * 640.0f/f_plasma_dim;
//MTS		p_verts[i].tv1 = (2.0f*p_verts[i].tv0 - 1.0f) * 480.0f/f_plasma_dim;
	}

	pQuadVB->Unlock();



	// These are used to render the backdrop.
	gpd3dDev->CreateVertexBuffer(4*sizeof(BaseStream), 0, 0, 0, &pIntensityQuadVB);
	BaseStream* p_bs_verts;
	pIntensityQuadVB->Lock(0, 0, (BYTE**) &p_bs_verts, 0);

	Set(&p_bs_verts[0].p, -1.0f, -1.0f, 1.0f);
	Set(&p_bs_verts[1].p, -1.0f, +1.0f, 1.0f);
	Set(&p_bs_verts[2].p, +1.0f, +1.0f, 1.0f);
	Set(&p_bs_verts[3].p, +1.0f, -1.0f, 1.0f);

	pIntensityQuadVB->Unlock();



    DWORD dwShaderVertexDecl[] =
    {
        D3DVSD_STREAM( 0 ),
        D3DVSD_REG( 0, D3DVSDT_FLOAT3 ),	// position
        D3DVSD_REG( 1, D3DVSDT_FLOAT2 ),	// intensity texture
        D3DVSD_REG( 2, D3DVSDT_FLOAT2 ),	// plasma 0
        D3DVSD_END()
    };

#ifndef BINARY_RESOURCE
	dwFogPShader = gApp.loadPixelShader( "d:\\shaders\\greenfog.xpu" );
	dwFogVShader = gApp.loadVertexShader( "d:\\shaders\\greenfog.xvu", dwShaderVertexDecl );
#else // BINARY_RESOURCE
	dwFogPShader = gApp.loadPixelShader( g_greenfog_xpu );
	dwFogVShader = gApp.loadVertexShader( g_greenfog_xvu, dwShaderVertexDecl );
#endif // BINARY_RESOURCE

	restart();	// renders the texture
}
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void GreenFog::destroy()
{
#define XBS_RESOURCE_RELEASE(a) if (a) a->Release(); a = NULL;
	XBS_RESOURCE_RELEASE(pQuadVB);
	XBS_RESOURCE_RELEASE(pIntensityQuadVB);
	for (int i=0; i<NUM_PLASMAS; i++) { XBS_RESOURCE_RELEASE(pPlasmaTexture[i]); }
	XBS_RESOURCE_RELEASE(pIntensityTextureU);
	XBS_RESOURCE_RELEASE(pIntensityTextureR);
	XBS_RESOURCE_RELEASE(pIntensityZ);
#undef XBS_RESOURCE_RELEASE
	if (dwFogPShader) gpd3dDev->DeletePixelShader( dwFogPShader );
	if (dwFogVShader) gpd3dDev->DeleteVertexShader( dwFogVShader );
	dwFogPShader = dwFogVShader = 0;
}
///////////////////////////////////////////////////////////////////////////////
void GreenFog::advanceTime(float fElapsedTime, float fDt)
{
	renderIntensityTexture();
}
///////////////////////////////////////////////////////////////////////////////
void GreenFog::render(bool b_cmp_to_one)
{
	int i;

	gpd3dDev->SetVertexShader( dwFogVShader );
	gpd3dDev->SetPixelShader ( dwFogPShader );

	// Swap render and update textures.
	LPDIRECT3DTEXTURE8 p_swap = pIntensityTextureR;
	pIntensityTextureR = pIntensityTextureU;
	pIntensityTextureU = p_swap;

	// Use what was the update(d) texture.
	gpd3dDev->SetTexture(0, pIntensityTextureR);



    gpd3dDev->SetTextureStageState( 0, D3DTSS_MINFILTER, D3DTEXF_LINEAR );
    gpd3dDev->SetTextureStageState( 0, D3DTSS_MAGFILTER, D3DTEXF_LINEAR );
    gpd3dDev->SetTextureStageState( 0, D3DTSS_MIPFILTER, D3DTEXF_NONE );
	gpd3dDev->SetTextureStageState( 0, D3DTSS_ADDRESSU, D3DTADDRESS_CLAMP);
	gpd3dDev->SetTextureStageState( 0, D3DTSS_ADDRESSV, D3DTADDRESS_CLAMP);

	for (i=0; i<NUM_PLASMAS; i++)
	{
		gpd3dDev->SetTexture(i+1, pPlasmaTexture[i]);
		gpd3dDev->SetTextureStageState( i+1, D3DTSS_MINFILTER, D3DTEXF_LINEAR );
		gpd3dDev->SetTextureStageState( i+1, D3DTSS_MAGFILTER, D3DTEXF_LINEAR );
		gpd3dDev->SetTextureStageState( i+1, D3DTSS_MIPFILTER, D3DTEXF_NONE );
		gpd3dDev->SetTextureStageState( i+1, D3DTSS_ADDRESSU, D3DTADDRESS_WRAP);
		gpd3dDev->SetTextureStageState( i+1, D3DTSS_ADDRESSV, D3DTADDRESS_WRAP);
	}
	
    gpd3dDev->SetRenderState( D3DRS_ALPHATESTENABLE,	TRUE );
//MTS    gpd3dDev->SetRenderState( D3DRS_ZFUNC,				D3DCMP_GREATEREQUAL );
    gpd3dDev->SetRenderState( D3DRS_ZFUNC,				(b_cmp_to_one) ? D3DCMP_EQUAL : D3DCMP_ALWAYS );

    gpd3dDev->SetRenderState( D3DRS_ALPHAREF,			0x00000001 );
    gpd3dDev->SetRenderState( D3DRS_ALPHABLENDENABLE,	TRUE );
    gpd3dDev->SetRenderState( D3DRS_SRCBLEND,			D3DBLEND_SRCALPHA );
    gpd3dDev->SetRenderState( D3DRS_DESTBLEND,			D3DBLEND_ONE );
//MTS    gpd3dDev->SetRenderState( D3DRS_SRCBLEND,			D3DBLEND_ONE );		// Just render intensity
//MTS    gpd3dDev->SetRenderState( D3DRS_DESTBLEND,			D3DBLEND_ZERO );	// Just render intensity
    gpd3dDev->SetRenderState( D3DRS_ZWRITEENABLE,		FALSE );


	D3DVECTOR4 val[2*NUM_PLASMAS];
	float f_intensity = max(0.0f, gApp.getBlobIntensity() * 0.7f - 0.1f);
	Set(&val[0], 0.0f*f_intensity, f_intensity, 0.0f*f_intensity, 1.0f);

	
	float f_glow = 0.75f * max(0.0f, min(1.0f, (gApp.getElapsedTime() - GLOW_FADE_SCREEN_START) * GLOW_FADE_SCREEN_MUL));
//MTS	Set(&val[1], f_glow*0.625f, f_glow*1.0f, f_glow*0.0625f, 0.0f);
	Set(&val[1], f_glow*0.625f, f_glow, f_glow * 0.4f, 0.0f);

	
	gpd3dDev->SetPixelShaderConstant(0, &val[0], 2);




	D3DVECTOR4 origin, origin_scr;
	Set(&origin, 0.0f, 0.0f, 0.0f, 1.0f);
	TransformPoint(origin, gApp.theCamera.getWTP(), &origin_scr);
	Scale(&origin_scr, 1.0f / origin_scr.w);


	const float f_plasma_dim = ((float)PLASMA_SIZE);

	// Set the transforms for the plasma texture coordinates.
	for (i=0; i<NUM_PLASMAS; i++)
	{
		float rad = 0.6f * (((float)(NUM_PLASMAS-i-1)) / ((float)NUM_PLASMAS) - 0.2f);
		float x_mul = 0.5f * gApp.getCameraRadiusFromBlob() * MUL_SCALE;
		float y_mul = 1.0f * gApp.theCamera.getAspectRatio() * gApp.getCameraRadiusFromBlob() * MUL_SCALE;

		float x_add =  rad * gApp.getCameraTheta() - origin_scr.x * x_mul * 640.0f/f_plasma_dim;
		float y_add = -rad * gApp.getCameraPhi()   + origin_scr.y * y_mul * 480.0f/f_plasma_dim;
		// I don't know why y_mul is 1.0f* and x_mul is 0.5f*, but it looks less stretched in y this way.


		Set(&val[2*i+0], y_add, x_add, 0.0f, 0.0f);
		Set(&val[2*i+1], -y_mul, -x_mul, 1.0f, 1.0f);
//MTS		Set(&val[2*i+0], x_add, y_add, 0.0f, 0.0f);
//MTS		Set(&val[2*i+1], x_mul, y_mul, 1.0f, 1.0f);
	}

	gpd3dDev->SetVertexShaderConstant(0, &val[0], 2*NUM_PLASMAS);


	gpd3dDev->SetStreamSource(0, pQuadVB, sizeof(GreenFogVertexBuffer));
	gpd3dDev->DrawPrimitive(D3DPT_TRIANGLEFAN, 0, 2);

	gpd3dDev->SetPixelShader (NULL);
	gpd3dDev->SetVertexShader(NULL);

	for (i=0; i<1+NUM_PLASMAS; i++) gpd3dDev->SetTexture(i, NULL);


	// Render fade to yellow.

	f_glow = max(0.0f, min(1.0f, (gApp.getElapsedTime() - GLOW_FADE_CIRCLE_START) * GLOW_FADE_CIRCLE_MUL));

	if (gApp.getElapsedTime() < BLOB_STATIC_END_TIME)
	{
		float t = gApp.getElapsedTime();
		f_glow = (t < BLOB_STATIC_END_TIME * 0.2f) ? (t / (BLOB_STATIC_END_TIME*0.2f)) : (1.0f - (t-BLOB_STATIC_END_TIME * 0.2f)/BLOB_STATIC_END_TIME);
	}

	int alpha = max(0, min( 255, (int) ((255.0f * f_glow))));
//MTS	alpha >>= 1;
	alpha = min(196, alpha*2);


	if (alpha)
	{
//MTS		DWORD glow_factor = 0x00a0ff10 | (alpha << 24);
		DWORD glow_factor = 0x00A0FF60 | (alpha << 24);


		gpd3dDev->SetRenderState(D3DRS_TEXTUREFACTOR, glow_factor);
		gpd3dDev->SetTextureStageState(0,D3DTSS_COLORARG1,D3DTA_TFACTOR);
		gpd3dDev->SetTextureStageState(0,D3DTSS_COLORARG2,D3DTA_TEXTURE);
		gpd3dDev->SetTextureStageState(0,D3DTSS_COLOROP,D3DTOP_MODULATE);
		gpd3dDev->SetTextureStageState(1,D3DTSS_COLOROP,D3DTOP_DISABLE);
		gpd3dDev->SetTextureStageState(0,D3DTSS_ALPHAARG1,D3DTA_TFACTOR);
		gpd3dDev->SetTextureStageState(0,D3DTSS_ALPHAARG2,D3DTA_TEXTURE);
		gpd3dDev->SetTextureStageState(0,D3DTSS_ALPHAOP,D3DTOP_MODULATE);


		// Correct center.
		float x_mul = gApp.getCameraRadiusFromBlob() * MUL_SCALE;
		float y_mul = gApp.getCameraRadiusFromBlob() * MUL_SCALE * gApp.theCamera.getAspectRatio();

		D3DVECTOR4 origin, origin_screen;
		Set(&origin, 0.0f, 0.0f, 0.0f, 1.0f);
		TransformPoint(origin, gApp.theCamera.getWTP(), &origin_screen);
		Scale(&origin_screen, 1.0f / origin_screen.w);
		//origin_screen.x = 0.5f - x_mul * origin_screen.x;
		//origin_screen.y = 0.5f - y_mul * origin_screen.y;


		D3DMATRIX mat_tex;
		float mul = 0.33f / f_glow;

		if (gApp.getElapsedTime() < BLOB_STATIC_END_TIME)
		{
			mul = gApp.getCameraRadiusFromBlob() * BLOB_STATIC_END_TIME / (gApp.getElapsedTime() * 8.0f);
		}

		SetScale(mul, mul, 1.0f, &mat_tex);
		mat_tex._31 = 0.5f*(1.0f-mul) - 0.5f * origin_screen.x * mul * 1.33f;
		mat_tex._32 = 0.5f*(1.0f-mul) + 0.5f * origin_screen.y * mul;




		
		D3DMATRIX iden,scaled;
		SetIdentity(&iden);
		SetScale(1.0f / mul, 1.0f / mul, 0.0f, &scaled);
		scaled._43 = 1.0f;
		gpd3dDev->SetTransform(D3DTS_WORLD, &scaled);
		gpd3dDev->SetTransform(D3DTS_VIEW, &iden);
		gpd3dDev->SetTransform(D3DTS_PROJECTION, &iden);


		gpd3dDev->SetTexture(0,gApp.pGlowMap);

		gpd3dDev->SetVertexShader(D3DFVF_XYZ | D3DFVF_TEX2);

		
		gpd3dDev->DrawPrimitive(D3DPT_TRIANGLEFAN, 0, 2);
	}
    
	gpd3dDev->SetRenderState( D3DRS_ZWRITEENABLE,		TRUE );
    gpd3dDev->SetRenderState( D3DRS_DESTBLEND,			D3DBLEND_INVSRCALPHA );
    gpd3dDev->SetRenderState( D3DRS_ZFUNC,				D3DCMP_LESSEQUAL );
}
///////////////////////////////////////////////////////////////////////////////
void GreenFog::restart()
{
	int tffonp = 255 / NUM_PLASMAS;
	for (int i=0; i<NUM_PLASMAS; i++)
	{
		if (pPlasmaTexture[i]) pPlasmaTexture[i]->Release();
	}
	CreateIntensityTexture_8Bit(pPlasmaTexture, NUM_PLASMAS, PLASMA_SIZE, 5 * tffonp, gApp.rand(),
					((tffonp*3)/4) << 8, 255/max(1,NUM_PLASMAS-0));
}
///////////////////////////////////////////////////////////////////////////////
void GreenFog::renderIntensityTexture()
{
	// Get camera position and render geometry into background.

	// Store old render targets.
	LPDIRECT3DSURFACE8 pOldRT,pOldZ;
	gpd3dDev->GetRenderTarget(&pOldRT);
	gpd3dDev->GetDepthStencilSurface(&pOldZ);


	// Leave the camera position where it is.
	LPDIRECT3DSURFACE8 p_update_surf;
	pIntensityTextureU->GetSurfaceLevel(0, &p_update_surf);
	gpd3dDev->SetRenderTarget(p_update_surf, pIntensityZ);

	if( gpd3dDev->BeginScene() == D3D_OK )
	{
		gpd3dDev->Clear(0,NULL,
						D3DCLEAR_STENCIL | D3DCLEAR_ZBUFFER | D3DCLEAR_TARGET,
						0xffff0000,
						1.f,
						0 );

		// Render a backdrop.

		gpd3dDev->SetRenderState( D3DRS_ALPHATESTENABLE,	TRUE );
		gpd3dDev->SetRenderState( D3DRS_ALPHAFUNC,			D3DCMP_GREATEREQUAL );
		gpd3dDev->SetRenderState( D3DRS_ALPHAREF,			0x00000001 );
		gpd3dDev->SetRenderState( D3DRS_ALPHABLENDENABLE,	TRUE );
		gpd3dDev->SetRenderState( D3DRS_SRCBLEND,			D3DBLEND_ONE );
		gpd3dDev->SetRenderState( D3DRS_DESTBLEND,			D3DBLEND_ZERO );

		gpd3dDev->SetVertexShader(gApp.dwVShader[st_Depth]);
		gpd3dDev->SetPixelShader (gApp.dwPShader[st_Depth]);


		//#define Z_ADD			c16
		//#define Z_MUL			c17
		//#define POS_MUL		c18
		//#define POS_SHIFT		c19


		D3DVECTOR4 val[4];
		float max_color_dist = 2.0f * MAIN_FOG_RAD;
		float z_mul = 1.0f;	// set later on for the geometry case, this is just the backdrop
		float z_add = 0.0f;

		Set(&val[0], z_mul, z_mul, z_mul, 1.0f );
		Set(&val[1], z_add, z_add, z_add, 1.0f );

		float x_mul = gApp.getCameraRadiusFromBlob() * MUL_SCALE;
		float y_mul = gApp.getCameraRadiusFromBlob() * MUL_SCALE * gApp.theCamera.getAspectRatio();

		Set(&val[2], x_mul, y_mul, 0.0f, 1.0f);


		D3DVECTOR4 origin;
		Set(&origin, 0.0f, 0.0f, 0.0f, 1.0f);
		TransformPoint(origin, gApp.theCamera.getWTP(), &val[3]);
		Scale(&val[3], 1.0f / val[3].w);
		val[3].x = 0.5f - val[2].x * val[3].x;
		val[3].y = 0.5f - val[2].y * val[3].y;
		val[3].z = 0.5f;
		val[3].w = 0.0f;
		// If looking at origin, then Set(&val[3], 0.5f, 0.5f, 0.5f, 0.0f);
//MTS		Set(&val[3], 0.5f, 0.5f, 0.5f, 0.0f);
		gpd3dDev->SetVertexShaderConstant(16, &val[0], 4);


		D3DMATRIX iden;
		SetIdentity(&iden);
		iden._43 = 1.0f;
		iden._44 = 0.0f;
		gpd3dDev->SetVertexShaderConstant(0, &iden, 4);


		gpd3dDev->SetStreamSource(0, pIntensityQuadVB, sizeof(BaseStream));
		gpd3dDev->DrawPrimitive(D3DPT_TRIANGLEFAN, 0, 2);


		gpd3dDev->SetRenderState( D3DRS_ZWRITEENABLE,		TRUE);
		gpd3dDev->SetRenderState( D3DRS_ZENABLE,			TRUE);

		z_add = MAIN_FOG_RAD - gApp.getCameraRadiusFromBlob();
		z_mul = 1.0f / max_color_dist;
		Set(&val[0], z_mul, z_mul, z_mul, 1.0f );
		Set(&val[1], z_add, z_add, z_add, 1.0f );
		gpd3dDev->SetVertexShaderConstant(16, &val, 2);

		gApp.sceneGeom.renderZ();

		// Without NULLing the pixel shader, the shields aren't transparent. Dunno why.
		gpd3dDev->SetPixelShader (NULL);
		gpd3dDev->SetVertexShader(NULL);


		// Clear stencil and depth buffer, but not the "target", or ARGB values.
		gpd3dDev->Clear(0,NULL,
						D3DCLEAR_STENCIL | D3DCLEAR_ZBUFFER,
						0xffffffff,
						1.f,
						0 );



		gpd3dDev->EndScene();
	}
	else
	{
		int a = 0;
	}

	gpd3dDev->SetRenderTarget(pOldRT,pOldZ);
	p_update_surf->Release();
	pOldRT->Release();
	pOldZ->Release();
}
///////////////////////////////////////////////////////////////////////////////
