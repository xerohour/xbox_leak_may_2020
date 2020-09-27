//
//	logo_renderer.cpp
//
///////////////////////////////////////////////////////////////////////////////
//  Copyright (C) 2001, Pipeworks Software Inc.
//				All rights reserved
#include "precomp.h"
#include "xbs_app.h"
#include "logo_renderer.h"
#include "logo_geometry.h"
#include "text_geometry.h"
#include "tm_pixels.h"
#include "renderer.h"
#include "tex_gen.h"
#include "xbs_app.h"


///////////////////////////////////////////////////////////////////////////////
float * LogoRenderer::decompressPosData(short *pdata,int ncount,float f_oo_scale,float f_delta)
{
	float * p_data = (float *)MemAlloc(ncount * sizeof(float) * 3);
	for(int i = 0; i < ncount * 3;i++)
	{
		p_data[i] = ( (float)pdata[i] ) * f_oo_scale + f_delta;
	}
	return p_data;
}
///////////////////////////////////////////////////////////////////////////////
float * LogoRenderer::decompressPosTexData(short *pdata,int ncount,
										   float f_oo_pos_scale,float f_pos_delta,
										   float f_oo_tex_scale,float f_tex_delta )
{
	float * p_data = (float *)MemAlloc(ncount * sizeof(float) * 5);
	for(int i = 0; i < ncount*5;i+=5)
	{
		p_data[i  ] = ( (float)pdata[i  ] ) * f_oo_pos_scale + f_pos_delta;
		p_data[i+1] = ( (float)pdata[i+1] ) * f_oo_pos_scale + f_pos_delta;
		p_data[i+2] = ( (float)pdata[i+2] ) * f_oo_pos_scale + f_pos_delta;
		p_data[i+3] = ( (float)pdata[i+3] ) * f_oo_tex_scale + f_tex_delta;
		p_data[i+4] = ( (float)pdata[i+4] ) * f_oo_tex_scale + f_tex_delta;
	}
	return p_data;	
}															
///////////////////////////////////////////////////////////////////////////////
WORD * LogoRenderer::decompressIndexData(char *pdata,int ncount)
{
	WORD * p_data = (WORD *)MemAlloc(ncount * sizeof(WORD));
	
	p_data[0] = (WORD)pdata[0];

	char * pbytes = pdata;
	for(int i = 1; i < ncount; i++)
	{
		if(pbytes[i] == 126)
		{
			char hi = pbytes[i+1];
			char lo = pbytes[i+2];

			p_data[i] = ((((short)hi)&0xff)<<8) | (((short)lo)&0xff);
			p_data[i] += p_data[i-1];
			pbytes += 2;
		}
		else
		{
			p_data[i] = p_data[i-1] + pbytes[i];
		}
	}
	return p_data;
}
///////////////////////////////////////////////////////////////////////////////
void LogoRenderer::create()
{
	decompressData();
	createSlash();
	createText();
	createTMs();

	pSlashTexture = NULL;

#ifndef BINARY_RESOURCE
	dwInteriorPShader = gApp.loadPixelShader ("d:\\shaders\\slash_interior.xpu");
#else // BINARY_RESOURCE
	dwInteriorPShader = gApp.loadPixelShader ( g_slash_interior_xpu );
#endif // BINARY_RESOURCE
	
	DWORD dwDecl[] =
    {
        D3DVSD_STREAM( 0 ),
		D3DVSD_REG( 0, D3DVSDT_FLOAT3 ),
		D3DVSD_REG( 1, D3DVSDT_FLOAT2 ),     
		D3DVSD_END()
    };

#ifndef BINARY_RESOURCE
	dwInteriorVShader = gApp.loadVertexShader("d:\\shaders\\slash_interior.xvu",dwDecl);
#else // BINARY_RESOURCE
	dwInteriorVShader = gApp.loadVertexShader ( g_slash_interior_xvu,dwDecl );
#endif // BINARY_RESOURCE
}
///////////////////////////////////////////////////////////////////////////////
void LogoRenderer::decompressData()
{
	indices_xboxlogolip_0 = decompressIndexData(indices_xboxlogolip_0C,index_count_xboxlogolip_0);
	verts_xboxlogolip_0 = decompressPosTexData(verts_xboxlogolip_0C,
											   vertex_count_xboxlogolip_0,
											   xbl_OO_POS_SCALE,xbl_POS_DELTA,
											   xbl_OO_TEX_SCALE,xbl_TEX_DELTA );

	indices_xboxlogosurface_0 = decompressIndexData(indices_xboxlogosurface_0C,index_count_xboxlogosurface_0);
	verts_xboxlogosurface_0 = decompressPosTexData(verts_xboxlogosurface_0C,
												   vertex_count_xboxlogosurface_0,
												   xbl_OO_POS_SCALE,xbl_POS_DELTA,
												   xbl_OO_TEX_SCALE,xbl_TEX_DELTA );

	indices_xboxlogosurfacetop_0 = decompressIndexData(indices_xboxlogosurfacetop_0C,index_count_xboxlogosurfacetop_0);
	verts_xboxlogosurfacetop_0 = decompressPosTexData(verts_xboxlogosurfacetop_0C,
												      vertex_count_xboxlogosurfacetop_0,
												      xbl_OO_POS_SCALE,xbl_POS_DELTA,
												      xbl_OO_TEX_SCALE,xbl_TEX_DELTA );

	indices_xboxlogointerior_0 = decompressIndexData(indices_xboxlogointerior_0C,index_count_xboxlogointerior_0);
	verts_xboxlogointerior_0 = decompressPosTexData(verts_xboxlogointerior_0C,
												    vertex_count_xboxlogointerior_0,
												    xbl_OO_POS_SCALE,xbl_POS_DELTA,
												    xbl_OO_TEX_SCALE,xbl_TEX_DELTA );

	indices_tm_slash_0 = decompressIndexData(indices_tm_slash_0C,index_count_tm_slash_0);
	verts_tm_slash_0 = decompressPosTexData(verts_tm_slash_0C,
										    vertex_count_tm_slash_0,
											xbl_OO_POS_SCALE,xbl_POS_DELTA,
											xbl_OO_TEX_SCALE,xbl_TEX_DELTA );

	indices_tm_wordmark_0 = decompressIndexData(indices_tm_wordmark_0C,index_count_tm_wordmark_0);
	verts_tm_wordmark_0 = decompressPosTexData(verts_tm_wordmark_0C,
											   vertex_count_tm_wordmark_0,
											   xbl_OO_POS_SCALE,xbl_POS_DELTA,
											   xbl_OO_TEX_SCALE,xbl_TEX_DELTA );
	
	indices_text_0 = decompressIndexData(indices_text_0C,index_count_text_0);
	verts_text_0 = decompressPosData(verts_text_0C,vertex_count_text_0,xbt_OO_POS_SCALE,xbt_POS_DELTA );
}
///////////////////////////////////////////////////////////////////////////////
void LogoRenderer::createSlash()
{
	xbl_vertex *pv = NULL;

	nLipIndices = index_count_xboxlogolip_0;
	nLipVerts = vertex_count_xboxlogolip_0;
	pLipIndices = indices_xboxlogolip_0;
	gpd3dDev->CreateVertexBuffer(sizeof(xbl_vertex) * nLipVerts,NULL,FVF_xbl,NULL,&pLipVB);
	pLipVB->Lock(0,0,(BYTE **)&pv,0);
	memcpy(pv,verts_xboxlogolip_0,sizeof(xbl_vertex) * nLipVerts);
	pLipVB->Unlock();
	pLipTex = CreateGradientTexture(16,128,0xff000100,0xff4b9b4b);

	nSurfaceIndices = index_count_xboxlogosurface_0;
	nSurfaceVerts = vertex_count_xboxlogosurface_0;
	pSurfaceIndices = indices_xboxlogosurface_0;
	gpd3dDev->CreateVertexBuffer(sizeof(xbl_vertex) * nSurfaceVerts,NULL,FVF_xbl,NULL,&pSurfaceVB);
	pSurfaceVB->Lock(0,0,(BYTE **)&pv,0);
	memcpy(pv,verts_xboxlogosurface_0,sizeof(xbl_vertex) * nSurfaceVerts);
	pSurfaceVB->Unlock();
	pSurfaceTex = CreateHighlightTexture(256,6,false,0.5f,0.5f);

	nSurfaceTopIndices = index_count_xboxlogosurfacetop_0;
	nSurfaceTopVerts = vertex_count_xboxlogosurfacetop_0;
	pSurfaceTopIndices = indices_xboxlogosurfacetop_0;
	gpd3dDev->CreateVertexBuffer(sizeof(xbl_vertex) * nSurfaceVerts,NULL,FVF_xbl,NULL,&pSurfaceTopVB);
	pSurfaceTopVB->Lock(0,0,(BYTE **)&pv,0);
	memcpy(pv,verts_xboxlogosurfacetop_0,sizeof(xbl_vertex) * nSurfaceTopVerts);

	// HACK to enforce border color.
	for(int i=0; i < (int)nSurfaceTopVerts;i++)
	{
		if(fast_fabs(pv[i].v0-1.f) <= 0.01f) 
			pv[i].v0 = -1.f;
	}

	pSurfaceTopVB->Unlock();
	pSurfaceTopTex = CreateGradientTexture(16,128,0xff000000,0xffffffff);

	nInteriorIndices = index_count_xboxlogointerior_0;
	nInteriorVerts = vertex_count_xboxlogointerior_0;
	pInteriorIndices = indices_xboxlogointerior_0;
	gpd3dDev->CreateVertexBuffer(sizeof(xbl_vertex) * nInteriorVerts,NULL,FVF_xbl,NULL,&pInteriorVB);
	pInteriorVB->Lock(0,0,(BYTE **)&pv,0);
	memcpy(pv,verts_xboxlogointerior_0,sizeof(xbl_vertex) * nInteriorVerts);
	pInteriorVB->Unlock();
}
///////////////////////////////////////////////////////////////////////////////
void LogoRenderer::createText()
{ 	
	xbt_vertex *pv;

	nText_Verts = vertex_count_text_0;
	nText_Indices = index_count_text_0;
	pText_Indices = indices_text_0;
	gpd3dDev->CreateVertexBuffer(sizeof(xbt_vertex) * nText_Verts,NULL,FVF_xbt,NULL,&pText_VB);
	pText_VB->Lock(0,0,(BYTE **)&pv,0);
	memcpy(pv,verts_text_0,sizeof(xbt_vertex) * nText_Verts);
	pText_VB->Unlock();
	SetIdentity(&matText_Anim);

	bRenderText = false;
}
///////////////////////////////////////////////////////////////////////////////
void LogoRenderer::createTMs()
{
	xbl_vertex *pv;

	nSlashTM_Verts = vertex_count_tm_slash_0;
	nSlashTM_Indices = index_count_tm_slash_0;
	pSlashTM_Indices = indices_tm_slash_0;
	gpd3dDev->CreateVertexBuffer(sizeof(xbl_vertex) * nSlashTM_Verts,NULL,FVF_xbl,NULL,&pSlashTM_VB);
	pSlashTM_VB->Lock(0,0,(BYTE **)&pv,0);
	memcpy(pv,verts_tm_slash_0,sizeof(xbl_vertex) * nSlashTM_Verts);
	pSlashTM_VB->Unlock();

	nTextTM_Verts = vertex_count_tm_wordmark_0;
	nTextTM_Indices = index_count_tm_wordmark_0;
	pTextTM_Indices = indices_tm_wordmark_0;
	gpd3dDev->CreateVertexBuffer(sizeof(xbl_vertex) * nTextTM_Verts,NULL,FVF_xbl,NULL,&pTextTM_VB);
	pTextTM_VB->Lock(0,0,(BYTE **)&pv,0);
	memcpy(pv,verts_tm_wordmark_0,sizeof(xbl_vertex) * nTextTM_Verts);
	pTextTM_VB->Unlock();

	gpd3dDev->CreateTexture( 16,16,1,0,D3DFMT_A8R8G8B8,0,&pTMTex);
	
	D3DLOCKED_RECT rc;
	pTMTex->LockRect(0,&rc,NULL,0);
	XGSwizzleRect( tm_pixels,
				   0,
				   NULL,
				   rc.pBits,
				   16,
				   16,
				   NULL,
				   sizeof(DWORD) );

	pTMTex->UnlockRect(0);

	fTMAlpha = 0.f;
}
///////////////////////////////////////////////////////////////////////////////
void LogoRenderer::createSlashSurface()
{
	bool b_want_wordmark = (gApp.getElapsedTime() >= TEXT_ANIM_START_TIME + TEXT_ANIM_LEN);
	if (pSlashTexture && (b_want_wordmark == bHasWordmark)) return;

	LPDIRECT3DSURFACE8		pSlashSurface;
	LPDIRECT3DSURFACE8		pSlashDepthBuf;

	D3DFORMAT fmt = D3DFMT_A8R8G8B8;
	DWORD mst = D3DMULTISAMPLE_2_SAMPLES_MULTISAMPLE_LINEAR;
	int x_dim = 1024;	
	int y_dim = 1024;
	gpd3dDev->CreateRenderTarget(x_dim, y_dim, fmt, mst, TRUE, &pSlashSurface);
	gpd3dDev->CreateDepthStencilSurface(x_dim,y_dim,D3DFMT_LIN_D24S8, mst, &pSlashDepthBuf);
	if (!pSlashTexture) gpd3dDev->CreateTexture(x_dim, y_dim, 1, 0, fmt, 0, &pSlashTexture);


	LPDIRECT3DSURFACE8 pOldRT,pOldZ;
	gpd3dDev->GetRenderTarget(&pOldRT);
	gpd3dDev->GetDepthStencilSurface(&pOldZ);

	gpd3dDev->SetRenderTarget(pSlashSurface, pSlashDepthBuf);

	if( gpd3dDev->BeginScene() == D3D_OK )
	{
		gpd3dDev->Clear(0,NULL,
						D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL,
						0xff000000,
						1.f,
						0 );

		bool b_old_render_text = bRenderText;
		bRenderText = b_want_wordmark;

		render(gApp.camController.getSlashTransform(), true);

		bRenderText = b_old_render_text;

		gpd3dDev->EndScene();


		LPDIRECT3DSURFACE8 p_tex_surf;
		pSlashTexture->GetSurfaceLevel(0, &p_tex_surf);

		gpd3dDev->CopyRects(pSlashSurface, NULL, 0, p_tex_surf, NULL);
		p_tex_surf->Release();

		bHasWordmark = b_want_wordmark;
	}

	pSlashSurface->Release();
	pSlashDepthBuf->Release();

	gpd3dDev->SetRenderTarget(pOldRT,pOldZ);
	pOldRT->Release();
	pOldZ->Release();
}
///////////////////////////////////////////////////////////////////////////////
void LogoRenderer::destroy()
{
	pLipVB->Release();
	pLipTex->Release();
	pSurfaceVB->Release();
	pSurfaceTex->Release();
	pSurfaceTopVB->Release();
	pSurfaceTopTex->Release();
	pInteriorVB->Release();
	pText_VB->Release();
	pSlashTM_VB->Release();
	pTextTM_VB->Release();
	pTMTex->Release();
	pSlashTexture->Release();

	MemFree(indices_xboxlogolip_0);
	MemFree(verts_xboxlogolip_0);
	MemFree(indices_xboxlogosurface_0);
	MemFree(verts_xboxlogosurface_0);
	MemFree(indices_xboxlogosurfacetop_0);
	MemFree(verts_xboxlogosurfacetop_0);
	MemFree(indices_xboxlogointerior_0);
	MemFree(verts_xboxlogointerior_0);
	MemFree(indices_tm_wordmark_0);
	MemFree(verts_tm_wordmark_0);
	MemFree(indices_tm_slash_0);
	MemFree(verts_tm_slash_0);
	MemFree(indices_text_0);
	MemFree(verts_text_0);

	gpd3dDev->DeleteVertexShader(dwInteriorVShader);
	gpd3dDev->DeletePixelShader (dwInteriorPShader);
}
///////////////////////////////////////////////////////////////////////////////
void LogoRenderer::advanceTime(float fElapsedTime, float fDt)
{
	if(fElapsedTime >= TEXT_ANIM_START_TIME)
	{
		bRenderText = true;

		int n_samples = sizeof(pos_anim_text)/sizeof( D3DVECTOR );

		float f_norm_pos = (fElapsedTime - TEXT_ANIM_START_TIME) / TEXT_ANIM_LEN;
		float f_pos = f_norm_pos * (n_samples-1);

		int pos_idx;

		 __asm
		{
			cvttss2si eax, f_pos
			mov pos_idx, eax
		}

		if(f_norm_pos <= 0.f)
		{
			matText_Anim._41 = pos_anim_text[0].x;
			matText_Anim._42 = pos_anim_text[0].y;
			matText_Anim._43 = pos_anim_text[0].z;
		}
		else if(f_norm_pos >= 1.f)
		{
			int last_pos = n_samples - 1;
			
			matText_Anim._41 = pos_anim_text[last_pos].x;
			matText_Anim._42 = pos_anim_text[last_pos].y;
			matText_Anim._43 = pos_anim_text[last_pos].z;
		}
		else
		{
			float f_frac = f_pos - ((float)pos_idx);

			D3DVECTOR &s = pos_anim_text[pos_idx];
			D3DVECTOR &e = pos_anim_text[pos_idx+1];

			matText_Anim._41 = s.x * (1.f-f_frac) + e.x * f_frac;
			matText_Anim._42 = s.y * (1.f-f_frac) + e.y * f_frac;
			matText_Anim._43 = s.z * (1.f-f_frac) + e.z * f_frac;
		}

		
		fTMAlpha = (fElapsedTime - (TEXT_ANIM_START_TIME)) / (TEXT_ANIM_LEN);
		fTMAlpha = max(0.f,min(1.f,fTMAlpha));
		fTMAlpha *= 255.f;
	}
	else
	{
		bRenderText = false;
		fTMAlpha = 0.f;
	}

	if (fElapsedTime >= FINISH_STOP_TIME)
	{
		createSlashSurface();
	}
}
///////////////////////////////////////////////////////////////////////////////
void LogoRenderer::render(const D3DMATRIX &mat_otw, bool b_force_3d_render)
{
	gpd3dDev->SetVertexShader(FVF_xbl);
	gpd3dDev->SetPixelShader (NULL);

	if (b_force_3d_render || (gApp.getElapsedTime() < FINISH_STOP_TIME))
	{
		Camera &cam = gApp.theCamera;

		gpd3dDev->SetTransform(D3DTS_WORLD,&mat_otw);
		gpd3dDev->SetTransform(D3DTS_VIEW,&cam.matWTC);
		gpd3dDev->SetTransform(D3DTS_PROJECTION,&cam.matProj);

		gpd3dDev->SetTextureStageState(0,D3DTSS_COLOROP,D3DTOP_SELECTARG1);
		gpd3dDev->SetTextureStageState(0,D3DTSS_COLORARG1,D3DTA_TEXTURE);
		gpd3dDev->SetTextureStageState(1,D3DTSS_COLOROP,D3DTOP_DISABLE);

		gpd3dDev->SetTextureStageState(0,D3DTSS_ADDRESSU,    D3DTADDRESS_BORDER );
  		gpd3dDev->SetTextureStageState(0,D3DTSS_ADDRESSV,    D3DTADDRESS_BORDER );
  		gpd3dDev->SetTextureStageState(0,D3DTSS_BORDERCOLOR, 0xff000000 );
  
  		gpd3dDev->SetRenderState(D3DRS_ALPHABLENDENABLE,FALSE);
  		gpd3dDev->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
  		gpd3dDev->SetRenderState(D3DRS_CULLMODE,D3DCULL_CW);


		gpd3dDev->SetStreamSource(0,pLipVB,sizeof(xbl_vertex));
		gpd3dDev->SetTexture(0,pLipTex);
		gpd3dDev->DrawIndexedVertices(D3DPT_TRIANGLELIST,nLipIndices,pLipIndices);

		gpd3dDev->SetStreamSource(0,pSurfaceVB,sizeof(xbl_vertex));
		gpd3dDev->SetTexture(0,pSurfaceTex);
		gpd3dDev->DrawIndexedVertices(D3DPT_TRIANGLELIST,nSurfaceIndices,pSurfaceIndices);

		gpd3dDev->SetStreamSource(0,pSurfaceTopVB,sizeof(xbl_vertex));
		gpd3dDev->SetTexture(0,pSurfaceTopTex);
		gpd3dDev->DrawIndexedVertices(D3DPT_TRIANGLELIST,nSurfaceTopIndices,pSurfaceTopIndices);

		gpd3dDev->SetStreamSource(0,pInteriorVB,sizeof(xbl_vertex));

		float fmag = -1.0f + 2.0f * (gApp.getElapsedTime() - SLASH_GRADIENT_TRANSITION_START) * SLASH_GRADIENT_TRANSITION_MUL;
		float w1 = max(0.f, min(1.f,-fmag));
		float w3 = max(0.f, min(1.f,+fmag));
		float w2 = max(0.f, min(1.f,1.f - w1 - w3));

		gpd3dDev->SetVertexShader(dwInteriorVShader);
		gpd3dDev->SetPixelShader (dwInteriorPShader);

		D3DMATRIX mat_final,tmp;
		MulMats(mat_otw,cam.getWTP(),&tmp);
		SetTranspose(tmp,&mat_final);
		gpd3dDev->SetVertexShaderConstant(0,(void *)&mat_final,4);

		D3DVECTOR4 vals[5];
		if(fmag < 0.f)
		{
			Set(&vals[0],0.81568f,1.f,0.5921f,1.f);
			Set(&vals[1],0.81568f,1.f,0.5921f,1.f);
			Set(&vals[2],0.81568f,1.f,0.5294f,1.f);
			Set(&vals[3],0.81568f,1.f,0.5294f,1.f);
			Set(&vals[4],w1,w1,w1,1.f);
		}
		else
		{
			Set(&vals[0],0.81568f,1.f,0.5294f,1.f);
			Set(&vals[1],0.81568f,1.f,0.5294f,1.f);
			Set(&vals[2],0.796f,0.8745f,0.0039f,1.f);
			Set(&vals[3],0.1294f,0.4168f,0.0901f,1.f);
			Set(&vals[4],w2,w2,w2,1.f);
		}

		gpd3dDev->SetVertexShaderConstant(4,(void *)&vals,5);
		gpd3dDev->DrawIndexedVertices(D3DPT_TRIANGLELIST,nInteriorIndices,pInteriorIndices);
	}

	else
	{				
		gpd3dDev->SetRenderState( D3DRS_ALPHABLENDENABLE,   TRUE);
   		gpd3dDev->SetRenderState( D3DRS_SRCBLEND,			D3DBLEND_ONE );
   		gpd3dDev->SetRenderState( D3DRS_DESTBLEND,			D3DBLEND_ZERO );
   		gpd3dDev->SetRenderState( D3DRS_ZWRITEENABLE,		FALSE );
   
   		D3DMATRIX iden;
   		SetIdentity(&iden);
   		gpd3dDev->SetTransform(D3DTS_WORLD, &iden);
   		gpd3dDev->SetTransform(D3DTS_VIEW, &iden);
   		gpd3dDev->SetTransform(D3DTS_PROJECTION, &iden);
   
  		gpd3dDev->SetRenderState(D3DRS_TEXTUREFACTOR, 0xFFFFFFFF);
   		gpd3dDev->SetTextureStageState(0,D3DTSS_COLOROP,D3DTOP_SELECTARG1);
   		gpd3dDev->SetTextureStageState(0,D3DTSS_COLORARG1,D3DTA_TEXTURE);
  		gpd3dDev->SetTextureStageState(0,D3DTSS_COLORARG2,D3DTA_TEXTURE);
  		gpd3dDev->SetTextureStageState(0,D3DTSS_ALPHAOP,D3DTOP_SELECTARG1);
  		gpd3dDev->SetTextureStageState(0,D3DTSS_ALPHAARG1,D3DTA_TFACTOR);
  		gpd3dDev->SetTextureStageState(0,D3DTSS_ALPHAARG2,D3DTA_TFACTOR);
  		gpd3dDev->SetTextureStageState(1,D3DTSS_COLOROP,D3DTOP_DISABLE);
  
   		gpd3dDev->SetTextureStageState(0,D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_DISABLE);
   		
   
   		gpd3dDev->SetTexture(0,pSlashTexture);
   
   		gpd3dDev->SetVertexShader(D3DFVF_XYZ | D3DFVF_TEX2);
   
   		
   		gpd3dDev->SetStreamSource(0, gApp.greenFog.borrowScreenQuad(), sizeof(GreenFogVertexBuffer));
   		gpd3dDev->DrawPrimitive(D3DPT_TRIANGLEFAN, 0, 2);
   
  		gpd3dDev->SetRenderState( D3DRS_ZWRITEENABLE,		TRUE );
  
   		// Prepare for rendering text...
   		gpd3dDev->SetRenderState(D3DRS_ALPHABLENDENABLE,FALSE);
   		gpd3dDev->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
  		gpd3dDev->SetRenderState(D3DRS_CULLMODE,D3DCULL_CW);
 		gpd3dDev->SetRenderState( D3DRS_ZWRITEENABLE,TRUE );
   
  		gpd3dDev->SetTextureStageState(0,D3DTSS_ADDRESSU,    D3DTADDRESS_BORDER );
  		gpd3dDev->SetTextureStageState(0,D3DTSS_ADDRESSV,    D3DTADDRESS_BORDER );
  		gpd3dDev->SetTextureStageState(0,D3DTSS_BORDERCOLOR, 0xff000000 );
     
   		gpd3dDev->SetTransform(D3DTS_WORLD,&mat_otw);
   		gpd3dDev->SetTransform(D3DTS_VIEW,&gApp.theCamera.matWTC);
   		gpd3dDev->SetTransform(D3DTS_PROJECTION,&gApp.theCamera.matProj);

	}

	if(bRenderText && !bHasWordmark)
	{
		gpd3dDev->SetVertexShader(FVF_xbl);
		gpd3dDev->SetPixelShader (NULL);

		gpd3dDev->SetRenderState(D3DRS_ZENABLE, D3DZB_FALSE);

		DWORD dw_alpha;
		float f_alpha = fTMAlpha;

		__asm
		{
			cvttss2si eax, f_alpha
			shl eax,24
			mov dw_alpha,eax
		}

		gpd3dDev->SetRenderState(D3DRS_ALPHABLENDENABLE,TRUE);
		gpd3dDev->SetRenderState(D3DRS_SRCBLEND,D3DBLEND_SRCALPHA);
		gpd3dDev->SetRenderState(D3DRS_DESTBLEND,D3DBLEND_INVSRCALPHA);

		gpd3dDev->SetRenderState(D3DRS_TEXTUREFACTOR,dw_alpha);

		gpd3dDev->SetTextureStageState(0,D3DTSS_COLOROP,D3DTOP_SELECTARG1);
		gpd3dDev->SetTextureStageState(0,D3DTSS_COLORARG1,D3DTA_TEXTURE);
		gpd3dDev->SetTextureStageState(0,D3DTSS_COLORARG2,D3DTA_TFACTOR);
		gpd3dDev->SetTextureStageState(0,D3DTSS_ALPHAOP,D3DTOP_MODULATE);
		gpd3dDev->SetTextureStageState(0,D3DTSS_ALPHAARG1,D3DTA_TEXTURE);
		gpd3dDev->SetTextureStageState(0,D3DTSS_ALPHAARG2,D3DTA_TFACTOR);
		gpd3dDev->SetTextureStageState(1,D3DTSS_COLOROP,D3DTOP_DISABLE);
		gpd3dDev->SetTexture(0,pTMTex);

		gpd3dDev->SetStreamSource(0,pSlashTM_VB,sizeof(xbl_vertex));
		gpd3dDev->DrawIndexedVertices(D3DPT_TRIANGLELIST,nSlashTM_Indices,pSlashTM_Indices);

		gpd3dDev->SetStreamSource(0,pTextTM_VB,sizeof(xbl_vertex));
		gpd3dDev->DrawIndexedVertices(D3DPT_TRIANGLELIST,nTextTM_Indices,pTextTM_Indices);

		gpd3dDev->SetRenderState(D3DRS_ALPHABLENDENABLE,FALSE);

		gpd3dDev->SetRenderState(D3DRS_TEXTUREFACTOR,0xff62ca13);
		gpd3dDev->SetTextureStageState(0,D3DTSS_COLORARG1,D3DTA_TFACTOR);

		D3DMATRIX anim_otw;

		D3DMATRIX flip;
		SetXRotation(Pi/2.f,&flip);

		D3DMATRIX tmp;

		MulMats(flip,matText_Anim,&tmp);
		MulMats(tmp,mat_otw,&anim_otw);

		gpd3dDev->SetVertexShader(FVF_xbt);

		gpd3dDev->SetTransform(D3DTS_WORLD,&anim_otw);
		gpd3dDev->SetStreamSource(0,pText_VB,sizeof(xbt_vertex));
		gpd3dDev->DrawIndexedVertices(D3DPT_TRIANGLELIST,nText_Indices,pText_Indices);

		gpd3dDev->SetRenderState(D3DRS_ZENABLE, D3DZB_TRUE);
	}

	gpd3dDev->SetRenderState(D3DRS_CULLMODE,D3DCULL_CCW);
}