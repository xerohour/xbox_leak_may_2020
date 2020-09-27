#include "stdafx.h"
#include "XBSwizzle.h"

#define D3DFVF_FLATVERTEX (D3DFVF_XYZRHW|D3DFVF_DIFFUSE|D3DFVF_TEX1)

int calcWidth;

struct FLATVERTEX
{
    FLOAT x, y, z, rhw; // The transformed position for the vertex.
    DWORD color;        // The vertex color.
	FLOAT tu, tv;
};

FLATVERTEX g_Vertices[4];


Sprite3D::Sprite3D(void)
{
}

/*Sprite3D::Sprite3D(float width, D3DXCOLOR col) {
	initialize(width, col);
}*/

RECT newimage;
LPDIRECT3DSURFACE8 imageSurface;

void Sprite3D::initialize(IDirect3DDevice8 *pD3DDevice, int width, int height, Color *palette) {
    
	for(int i = 0; i < 256; i++) {
		atariPalette[i].red = palette[i].red;
		atariPalette[i].green = palette[i].green;
		atariPalette[i].blue = palette[i].blue;
	}

	float imgWidth = 640.0f;
	float imgHeight = 480.0f;
	int tempwidth, tempheight;

	atariWidth = width;
	atariHeight = height;
	if(width < 256)
		tempwidth = 256;
	else if(width >= 256)
		tempwidth = 512;

	if(height < 256)
		tempheight = 256;
	else if(height >= 256)
		tempheight = 512;

	float tuValue = (float)width/(float)tempwidth;
	float tvValue = (float)height/(float)tempheight;

	g_Vertices[0].x = 0.0f;
	g_Vertices[0].y = 0.0f;
	g_Vertices[0].z = 0.5f;
	g_Vertices[0].rhw = 1.0f;
	g_Vertices[0].color= 0xffffffff;
	g_Vertices[0].tu = 0.0;
	g_Vertices[0].tv = 0.0;
	
	g_Vertices[1].x = imgWidth;
	g_Vertices[1].y = 0.0f;
	g_Vertices[1].z = 0.5f;
	g_Vertices[1].rhw = 1.0f;
	g_Vertices[1].color= 0xffffffff;
	g_Vertices[1].tu = tuValue;
	g_Vertices[1].tv = 0.0;

	g_Vertices[2].x = 0.0f;
	g_Vertices[2].y = imgHeight;
	g_Vertices[2].z = 0.5f;
	g_Vertices[2].rhw = 1.0f;
	g_Vertices[2].color= 0xffffffff;
	g_Vertices[2].tu = 0.0;
	g_Vertices[2].tv = tvValue;

	g_Vertices[3].x = imgWidth;
	g_Vertices[3].y = imgHeight;
	g_Vertices[3].z = 0.5f;
	g_Vertices[3].rhw = 1.0f;
	g_Vertices[3].color= 0xffffffff;
	g_Vertices[3].tu = tuValue;
	g_Vertices[3].tv = tvValue;
	
//	D3DXCreateTextureFromFile( pD3DDevice, "test.bmp", &texture );

	D3DXCreateTexture( pD3DDevice,
					   width,  //width
					   height,  //height
					   1,
					   0,
					   D3DFMT_A8R8G8B8,
					   D3DPOOL_MANAGED,
					   &texture);

	texture->GetLevelDesc( 0, &d3dsd );
    dwDstPitch = (DWORD)d3dlr.Pitch;
	calcWidth = d3dsd.Width * 4;

	//setup texture
	pD3DDevice->SetTexture( 0, texture );
	pD3DDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
	pD3DDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
	pD3DDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
	pD3DDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_DISABLE );

	//setup vertex buffer
	IDirect3DVertexBuffer8* pD3DVertexBuffer; 
	pD3DDevice->CreateVertexBuffer( 4 *sizeof( FLATVERTEX ),
                                    0,
                                    D3DFVF_FLATVERTEX,
                                    D3DPOOL_DEFAULT, 
                                    &pD3DVertexBuffer );

	VOID* pVertices;
	pD3DVertexBuffer->Lock( 0, sizeof(g_Vertices), (BYTE**)&pVertices, 0 );
	memcpy( pVertices, g_Vertices, sizeof(g_Vertices) );
	pD3DVertexBuffer->Unlock();

	pD3DDevice->SetStreamSource( 0, pD3DVertexBuffer, sizeof(FLATVERTEX) );
	pD3DDevice->SetVertexShader( D3DFVF_FLATVERTEX );
}

void Sprite3D::translate(D3DXVECTOR4 v) {
	translateVector = v;
	translateVector.w = 1.0f;
}

inline int calcpos(int width, int height, int x, int y)
{
	return(calcWidth * y + (x * 4));
}

void Sprite3D::drawSprite( IDirect3DDevice8 *pD3DDevice, unsigned char *atariImage )
{
	// Get the texture dimensions
    D3DSURFACE_DESC desc;
    texture->GetLevelDesc( 0, &desc );
    DWORD dwTexWidth  = desc.Width;
    DWORD dwTexHeight = desc.Height;
    DWORD dwTexDepth  = 0;
    
	texture->LockRect( 0, &d3dlr, 0, 0 );
    BYTE* pDst = (BYTE*)d3dlr.pBits;

	//print out atari image
	int counter = 0;
	int localAtariImage = 0;

	CXBSwizzler s( dwTexWidth, dwTexHeight, dwTexDepth );

	s.SetV( 0 );
	for( int y = 0; y < atariHeight; y++ )
	{
		s.SetU( 0 );
		for( int x = 0; x < atariWidth; x++, counter++ )
		{
			/*
			int pos = calcpos(d3dsd.Width, d3dsd.Height, x, y);
			localAtariImage = atariImage[counter];
			(BYTE*)d3dlr.pBits[pos    ]   = atariPalette[localAtariImage].blue;
			(BYTE*)d3dlr.pBits[pos + 1]   = atariPalette[localAtariImage].green;
			(BYTE*)d3dlr.pBits[pos + 2]   = atariPalette[localAtariImage].red;
			(BYTE*)d3dlr.pBits[pos + 3]   = 0xff;
			*/
			int pos = calcpos(d3dsd.Width, d3dsd.Height, x, y);
			localAtariImage = atariImage[counter];
			((DWORD*)d3dlr.pBits)[s.Get2D()]   = ( 0xff << 24 ) |
				                                 (atariPalette[localAtariImage].red << 16) | 
											     (atariPalette[localAtariImage].green << 8) |
											     (atariPalette[localAtariImage].blue);

/*			localAtariImage     = atariImage[counter];
			pDst[s.Get2D()    ] = atariPalette[localAtariImage].blue;
			pDst[s.Get2D() + 1] = atariPalette[localAtariImage].green;
			pDst[s.Get2D() + 2] = atariPalette[localAtariImage].red;
			pDst[s.Get2D() + 3] = 0xff;
*/
			s.IncU();
		}
		s.IncV();
	}

    texture->UnlockRect(0);
    pD3DDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP, 0, 2 );
}
