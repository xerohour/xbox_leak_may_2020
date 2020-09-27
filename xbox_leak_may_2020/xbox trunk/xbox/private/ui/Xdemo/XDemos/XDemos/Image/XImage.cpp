//=============================================================================
// File: XImage.cpp
//
// Desc: 
// Created: 07/12/2001 by Michael Lyons (mlyons@microsoft.com)
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//=============================================================================

#include "XImage.h"
#include <xgraphics.h>
#include "PNG\png.h"

//=============================================================================
//=============================================================================
#define RELEASE(x) if (x) { (x)->Release(); (x)=NULL; }

//=============================================================================
//=============================================================================
XIMAGEVERTEX XImage::m_ScreenVertices[] = {
	{   0.0f,   0.0f, 0.0f, 1.0f,   0.0f-0.5f,   0.0f-0.5f },
	{ 640.0f,   0.0f, 0.0f, 1.0f, 640.0f-0.5f,   0.0f-0.5f },
	{ 640.0f, 480.0f, 0.0f, 1.0f, 640.0f-0.5f, 480.0f-0.5f },
	{   0.0f, 480.0f, 0.0f, 1.0f,   0.0f-0.5f, 480.0f-0.5f },
};

//=============================================================================
//=============================================================================
XImage::XImage() :
	m_pTexture(NULL),
	m_pDevice(NULL),
	m_iWidth(0),
	m_iHeight(0)
{
		return;
}

//=============================================================================
//=============================================================================
template <class T> void __forceinline SWAP(T &v1, T &v2)
{
	*(int *)(&v1) ^= *(int *)(&v2);
	*(int *)(&v2) ^= *(int *)(&v1);
	*(int *)(&v1) ^= *(int *)(&v2);
}

//========================================================================
//========================================================================
void PNGAPI user_error_fn(png_structp a, png_const_charp b)
{
}

//========================================================================
//========================================================================
void PNGAPI user_warning_fn(png_structp a, png_const_charp b)
{
}

//========================================================================
//========================================================================
void * user_malloc_fn(png_structp png, size_t size)
{
	return malloc(size);
}

//========================================================================
//========================================================================
void user_free_fn(png_structp png, void * p)
{
	free(p);
}

//=============================================================================
//=============================================================================
bool XImage::Load(IDirect3DDevice8 *pDevice, char *szFileName)
{
	m_pDevice = pDevice;
	m_pDevice->AddRef();

#if 0
    if (FAILED(D3DXCreateTextureFromFileExA( m_pDevice, szFileName,
		D3DX_DEFAULT,
		D3DX_DEFAULT,
		1,
		0,
		D3DFMT_LIN_A8R8G8B8,
		0,
		D3DX_DEFAULT,
		D3DX_DEFAULT,
		0xFF000000,
		NULL,
		NULL,
		&m_pTexture)))
		return false;

	D3DSURFACE_DESC ld;

	m_pTexture->GetLevelDesc(0, &ld);

	m_iWidth = ld.Width;
	m_iHeight = ld.Height;

	if (ld.Format != D3DFMT_LIN_X8R8G8B8)
	{
		IDirect3DTexture8 *pTexture;

		m_pDevice->CreateTexture(ld.Width, ld.Height, 1, 0, D3DFMT_LIN_A8R8G8B8, 0, &pTexture);

		SWAP(pTexture, m_pTexture);

		D3DLOCKED_RECT lr1, lr2;

		pTexture->LockRect(0, &lr1, NULL, 0);
		m_pTexture->LockRect(0, &lr2, NULL, 0);

		XGUnswizzleRect(lr1.pBits, ld.Width, ld.Height, NULL, lr2.pBits, 0, NULL, 4);

		pTexture->UnlockRect(0);
		m_pTexture->UnlockRect(0);

		pTexture->Release();
	}

#else

	if (true)
	{
		FILE *fp=fopen(szFileName, "rb");
		if (fp)
		{
			png_structp	png_ptr;
			png_infop	info_ptr;
			png_infop	end_info;

			// read in the PNG header info
			if (true)
			{
				png_ptr = png_create_read_struct_2(PNG_LIBPNG_VER_STRING, NULL, user_error_fn, user_warning_fn, NULL, user_malloc_fn, user_free_fn);
				if (!png_ptr)
				{
					fclose(fp);
					return false;
				}

				info_ptr = png_create_info_struct(png_ptr);
				if (!info_ptr)
				{
					png_destroy_read_struct(&png_ptr, (png_infopp)NULL, (png_infopp)NULL);
					fclose(fp);
					return false;
				}

				end_info = png_create_info_struct(png_ptr);
				if (!end_info)
				{
					png_destroy_read_struct(&png_ptr, &info_ptr, (png_infopp)NULL);
					fclose(fp);
					return false;
				}

				png_init_io(png_ptr, fp);
				png_set_sig_bytes(png_ptr, 0);

				png_read_info(png_ptr, info_ptr);

				png_set_bgr(png_ptr);

				if (info_ptr->channels == 3)
				{
					png_set_filler(png_ptr, 0xFF, PNG_FILLER_AFTER);
				}

			}

			// create the texture and blit it
			if (true)
			{
				m_iWidth	= info_ptr->width;
				m_iHeight	= info_ptr->height;

				m_pDevice->CreateTexture(m_iWidth, m_iHeight, 1, 0, D3DFMT_LIN_A8R8G8B8, 0, &m_pTexture);

				D3DLOCKED_RECT lr;
				BYTE **row_pointers = (BYTE **)malloc(sizeof(BYTE *) * info_ptr->height);

				m_pTexture->LockRect(0, &lr, NULL, 0);

				unsigned int y;
				for (y=0 ; y<info_ptr->height ; y++)
				{
					row_pointers[y] = (BYTE *)lr.pBits + y*lr.Pitch;
				}

				png_read_image(png_ptr, row_pointers);

				m_pTexture->UnlockRect(0);
			}

			// load the bits
			if (true)
			{

			}

			// free the texture
			if (true)
			{
				png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
			}


			fclose(fp);
		}
	}

#endif



	return true;
}

//=============================================================================
//=============================================================================
bool XImage::Blt(RECT *r)
{
	if (r)
	{
		m_ScreenVertices[0].x = (float)r->left;
		m_ScreenVertices[0].y = (float)r->top;

		m_ScreenVertices[1].x = (float)r->right;
		m_ScreenVertices[1].y = (float)r->top;

		m_ScreenVertices[2].x = (float)r->right;
		m_ScreenVertices[2].y = (float)r->bottom;

		m_ScreenVertices[3].x = (float)r->left;
		m_ScreenVertices[3].y = (float)r->bottom;
	}
	else
	{
		m_ScreenVertices[0].x = 0;
		m_ScreenVertices[0].y = 0;

		m_ScreenVertices[1].x = 640;
		m_ScreenVertices[1].y = 0;

		m_ScreenVertices[2].x = 640;
		m_ScreenVertices[2].y = 480;

		m_ScreenVertices[3].x = 0;
		m_ScreenVertices[3].y = 480;
	}

	if (true)
	{
		m_ScreenVertices[0].u =      0.0f - 0.5f;
		m_ScreenVertices[0].v =      0.0f - 0.5f;
		m_ScreenVertices[1].u =  m_iWidth - 0.5f;
		m_ScreenVertices[1].v =      0.0f - 0.5f;
		m_ScreenVertices[2].u =  m_iWidth - 0.5f;
		m_ScreenVertices[2].v = m_iHeight - 0.5f;
		m_ScreenVertices[3].u =      0.0f - 0.5f;
		m_ScreenVertices[3].v = m_iHeight - 0.5f;
	}


    m_pDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,	TRUE );
    m_pDevice->SetRenderState( D3DRS_ALPHATESTENABLE,	TRUE );
    m_pDevice->SetRenderState( D3DRS_ALPHAFUNC,			D3DCMP_GREATER );

    m_pDevice->SetRenderState( D3DRS_SRCBLEND,			D3DBLEND_SRCALPHA );
    m_pDevice->SetRenderState( D3DRS_DESTBLEND,			D3DBLEND_INVSRCALPHA );


	// Rendering of scene objects happens here
	m_pDevice->SetTextureStageState( 0, D3DTSS_COLOROP,					D3DTOP_SELECTARG1 );
	m_pDevice->SetTextureStageState( 0, D3DTSS_COLORARG1,				D3DTA_TEXTURE );
	m_pDevice->SetTextureStageState( 0, D3DTSS_COLORARG2,				0 );
	m_pDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,					D3DTOP_SELECTARG1 );
	m_pDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1,				D3DTA_TEXTURE );
	m_pDevice->SetTextureStageState( 0, D3DTSS_ADDRESSU,				D3DTADDRESS_CLAMP );
	m_pDevice->SetTextureStageState( 0, D3DTSS_ADDRESSV,				D3DTADDRESS_CLAMP );
	m_pDevice->SetTextureStageState( 0, D3DTSS_MAGFILTER,				D3DTEXF_POINT );
	m_pDevice->SetTextureStageState( 0, D3DTSS_MINFILTER,				D3DTEXF_POINT );
	m_pDevice->SetTextureStageState( 0, D3DTSS_MIPFILTER,				D3DTEXF_POINT );

    m_pDevice->SetTexture(0, m_pTexture);
    m_pDevice->SetVertexShader( D3DFVF_XIMAGEVERTEX );

	m_pDevice->DrawPrimitiveUP(D3DPT_TRIANGLEFAN, 2, m_ScreenVertices, sizeof(m_ScreenVertices[0]));


	return true;
}

//=============================================================================
//=============================================================================
bool XImage::Blt(int x, int y, int w, int h)
{
	if (true)
	{
		m_ScreenVertices[0].x = (float)x;
		m_ScreenVertices[0].y = (float)y;

		m_ScreenVertices[1].x = (float)(x+w);
		m_ScreenVertices[1].y = (float)y;

		m_ScreenVertices[2].x = (float)(x+w);
		m_ScreenVertices[2].y = (float)(y+h);

		m_ScreenVertices[3].x = (float)x;
		m_ScreenVertices[3].y = (float)(y+h);
	}

	if (true)
	{
		m_ScreenVertices[0].u =      0.0f - 0.5f;
		m_ScreenVertices[0].v =      0.0f - 0.5f;
		m_ScreenVertices[1].u =  m_iWidth - 0.5f;
		m_ScreenVertices[1].v =      0.0f - 0.5f;
		m_ScreenVertices[2].u =  m_iWidth - 0.5f;
		m_ScreenVertices[2].v = m_iHeight - 0.5f;
		m_ScreenVertices[3].u =      0.0f - 0.5f;
		m_ScreenVertices[3].v = m_iHeight - 0.5f;
	}


    m_pDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,	TRUE );
    m_pDevice->SetRenderState( D3DRS_ALPHATESTENABLE,	TRUE );
    m_pDevice->SetRenderState( D3DRS_ALPHAFUNC,			D3DCMP_GREATER );

    m_pDevice->SetRenderState( D3DRS_SRCBLEND,			D3DBLEND_SRCALPHA );
    m_pDevice->SetRenderState( D3DRS_DESTBLEND,			D3DBLEND_INVSRCALPHA );


	// Rendering of scene objects happens here
	m_pDevice->SetTextureStageState( 0, D3DTSS_COLOROP,					D3DTOP_SELECTARG1 );
	m_pDevice->SetTextureStageState( 0, D3DTSS_COLORARG1,				D3DTA_TEXTURE );
	m_pDevice->SetTextureStageState( 0, D3DTSS_COLORARG2,				0 );
	m_pDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,					D3DTOP_SELECTARG1 );
	m_pDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1,				D3DTA_TEXTURE );
	m_pDevice->SetTextureStageState( 0, D3DTSS_ADDRESSU,				D3DTADDRESS_CLAMP );
	m_pDevice->SetTextureStageState( 0, D3DTSS_ADDRESSV,				D3DTADDRESS_CLAMP );
	m_pDevice->SetTextureStageState( 0, D3DTSS_MAGFILTER,				D3DTEXF_POINT );
	m_pDevice->SetTextureStageState( 0, D3DTSS_MINFILTER,				D3DTEXF_POINT );
	m_pDevice->SetTextureStageState( 0, D3DTSS_MIPFILTER,				D3DTEXF_POINT );

    m_pDevice->SetTexture(0, m_pTexture);
    m_pDevice->SetVertexShader( D3DFVF_XIMAGEVERTEX );

	m_pDevice->DrawPrimitiveUP(D3DPT_TRIANGLEFAN, 2, m_ScreenVertices, sizeof(m_ScreenVertices[0]));


	return true;
}

//=============================================================================
//=============================================================================
bool XImage::FillArea(int x, int y, int w, int h, DWORD dwColor)
{
	if (true)
	{
		m_ScreenVertices[0].x = (float)x;
		m_ScreenVertices[0].y = (float)y;

		m_ScreenVertices[1].x = (float)(x+w);
		m_ScreenVertices[1].y = (float)y;

		m_ScreenVertices[2].x = (float)(x+w);
		m_ScreenVertices[2].y = (float)(y+h);

		m_ScreenVertices[3].x = (float)x;
		m_ScreenVertices[3].y = (float)(y+h);
	}

    //m_pDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );
    //m_pDevice->SetRenderState( D3DRS_ALPHATESTENABLE,  FALSE );

    m_pDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
    m_pDevice->SetRenderState( D3DRS_ALPHATESTENABLE,  TRUE );



	// Rendering of scene objects happens here
	m_pDevice->SetRenderState( D3DRS_TEXTUREFACTOR,						dwColor );
	m_pDevice->SetRenderState( D3DRS_ZENABLE,							FALSE );
	m_pDevice->SetTextureStageState( 0, D3DTSS_COLOROP,					D3DTOP_SELECTARG1 );
	m_pDevice->SetTextureStageState( 0, D3DTSS_COLORARG1,				D3DTA_TFACTOR);
	m_pDevice->SetTextureStageState( 0, D3DTSS_COLORARG2,				0 );
	m_pDevice->SetTextureStageState( 0, D3DTSS_COLORARG2,				0 );
	m_pDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,					D3DTOP_SELECTARG1 );
	m_pDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1,				D3DTA_TFACTOR );
	m_pDevice->SetTextureStageState( 0, D3DTSS_ADDRESSU,				D3DTADDRESS_CLAMP );
	m_pDevice->SetTextureStageState( 0, D3DTSS_ADDRESSV,				D3DTADDRESS_CLAMP );
	m_pDevice->SetTextureStageState( 0, D3DTSS_MAGFILTER,				D3DTEXF_POINT );
	m_pDevice->SetTextureStageState( 0, D3DTSS_MINFILTER,				D3DTEXF_POINT );
	m_pDevice->SetTextureStageState( 0, D3DTSS_MIPFILTER,				D3DTEXF_POINT );

    m_pDevice->SetTexture(0, m_pTexture);
    m_pDevice->SetVertexShader( D3DFVF_XIMAGEVERTEX );

	m_pDevice->DrawPrimitiveUP(D3DPT_TRIANGLEFAN, 2, m_ScreenVertices, sizeof(m_ScreenVertices[0]));


	return true;
}

//=============================================================================
//=============================================================================
bool XImage::Free(void)
{
	RELEASE(m_pTexture);
	RELEASE(m_pDevice);

	return true;
}



