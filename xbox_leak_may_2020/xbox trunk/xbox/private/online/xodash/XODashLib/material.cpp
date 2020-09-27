//-----------------------------------------------------------------------------
// File: Material.cpp
//
// Desc: Classes designed to define the different types of materials.
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#include "std.h"
#include "material.h"
#include "texturecache.h"

extern LPDIRECT3DDEVICE8 g_pd3dDevice;

//-----------------------------------------------------------------------------
// Name: CBaseMaterial()
// Desc: Sets initial values of member variables and sets itself to the
//		 passed in material entry.
//-----------------------------------------------------------------------------
CBaseMaterial::CBaseMaterial()
{
    m_sMatName = NULL;
	m_MaterialDef.Diffuse.r  = m_MaterialDef.Diffuse.g  = m_MaterialDef.Diffuse.b  = m_MaterialDef.Diffuse.a  = 1.0f;
	m_MaterialDef.Ambient.r  = m_MaterialDef.Ambient.g  = m_MaterialDef.Ambient.b  = m_MaterialDef.Ambient.a  = 1.0f;
	m_MaterialDef.Specular.r = m_MaterialDef.Specular.g = m_MaterialDef.Specular.b = m_MaterialDef.Specular.a = 1.0f;
	m_MaterialDef.Emissive.r = m_MaterialDef.Emissive.g = m_MaterialDef.Emissive.b = m_MaterialDef.Emissive.a = 1.0f;
	m_MaterialDef.Power		 = 0.0f;
	m_ID		  = -1;
}

//-----------------------------------------------------------------------------
// Name: CBaseMaterial()
// Desc: Sets initial values of member variables and sets itself to the
//		 passed in material entry.
//-----------------------------------------------------------------------------
CBaseMaterial::CBaseMaterial(char *sMatName, int index)
{
    m_sMatName = new char[strlen(sMatName) + 1];
	strcpy( m_sMatName, sMatName );
	m_MaterialDef.Diffuse.r  = m_MaterialDef.Diffuse.g  = m_MaterialDef.Diffuse.b  = m_MaterialDef.Diffuse.a  = 1.0f;
	m_MaterialDef.Ambient.r  = m_MaterialDef.Ambient.g  = m_MaterialDef.Ambient.b  = m_MaterialDef.Ambient.a  = 1.0f;
	m_MaterialDef.Specular.r = m_MaterialDef.Specular.g = m_MaterialDef.Specular.b = m_MaterialDef.Specular.a = 1.0f;
	m_MaterialDef.Emissive.r = m_MaterialDef.Emissive.g = m_MaterialDef.Emissive.b = m_MaterialDef.Emissive.a = 1.0f;
	m_MaterialDef.Power		 = 0.0f;
	m_ID					 = index;
}

//-----------------------------------------------------------------------------
// Name: ~CBaseMaterial()
// Desc: Performs any needed cleanup before shutting down the object
//-----------------------------------------------------------------------------
CBaseMaterial::~CBaseMaterial()
{
	if(m_sMatName != NULL)
		delete [] m_sMatName;

	m_sMatName = NULL;
}

//-----------------------------------------------------------------------------
// Name: SetName()
// Desc: Sets the material's name
//-----------------------------------------------------------------------------
void CBaseMaterial::SetName(char *matName)
{
	m_sMatName = new char[strlen(matName) + 1];
	strcpy(m_sMatName, matName);
}

//-----------------------------------------------------------------------------
// Name: SetAllColors()
// Desc: Sets all of the materials colors if they are identical coming
//		 out of 3D Studio Max
//-----------------------------------------------------------------------------
void CBaseMaterial::SetAllColors(float r, float g, float b, float a)
{
	m_MaterialDef.Diffuse.r = m_MaterialDef.Ambient.r = m_MaterialDef.Specular.r = m_MaterialDef.Emissive.r = r;
	m_MaterialDef.Diffuse.g = m_MaterialDef.Ambient.g = m_MaterialDef.Specular.g = m_MaterialDef.Emissive.g = g;
	m_MaterialDef.Diffuse.b = m_MaterialDef.Ambient.b = m_MaterialDef.Specular.b = m_MaterialDef.Emissive.b = b;
	m_MaterialDef.Diffuse.a = m_MaterialDef.Ambient.a = m_MaterialDef.Specular.a = m_MaterialDef.Emissive.a = a;
}

//-----------------------------------------------------------------------------
// Name: SetDiffuse()
// Desc: Sets the diffuse color of the material
//-----------------------------------------------------------------------------
void CBaseMaterial::SetDiffuse(float r, float g, float b, float a)
{
	m_MaterialDef.Diffuse.r = r;
	m_MaterialDef.Diffuse.g = g;
	m_MaterialDef.Diffuse.b = b;
	m_MaterialDef.Diffuse.a = a;
}

//-----------------------------------------------------------------------------
// Name: SetAmbient()
// Desc: Sets the ambient color of the material
//-----------------------------------------------------------------------------
void CBaseMaterial::SetAmbient(float r, float g, float b, float a)
{
	m_MaterialDef.Ambient.r = r;
	m_MaterialDef.Ambient.g = g;
	m_MaterialDef.Ambient.b = b;
	m_MaterialDef.Ambient.a = a;
}

//-----------------------------------------------------------------------------
// Name: SetSpecular()
// Desc: Sets the specular color of the material
//-----------------------------------------------------------------------------
void CBaseMaterial::SetSpecular(float r, float g, float b, float a)
{
	m_MaterialDef.Specular.r = r;
	m_MaterialDef.Specular.g = g;
	m_MaterialDef.Specular.b = b;
	m_MaterialDef.Specular.a = a;
}

//-----------------------------------------------------------------------------
// Name: SetEmissive()
// Desc: Sets the emissive color of the material
//-----------------------------------------------------------------------------
void CBaseMaterial::SetEmissive(float r, float g, float b, float a)
{
	m_MaterialDef.Emissive.r = r;
	m_MaterialDef.Emissive.g = g;
	m_MaterialDef.Emissive.b = b;
	m_MaterialDef.Emissive.a = a;
}

//-----------------------------------------------------------------------------
// Name: SetPower()
// Desc: Sets the power of the material
//-----------------------------------------------------------------------------
void CBaseMaterial::SetPower(float power)
{
	m_MaterialDef.Power = power;
}

//-----------------------------------------------------------------------------
// Name: Setup()
// Desc: Initalizes the material for Direct3D
//-----------------------------------------------------------------------------
HRESULT CBaseMaterial::Setup( DWORD FVF, CTextureCache *pTexCache )
{
	if ( g_pd3dDevice != NULL )
	{
		g_pd3dDevice->SetVertexShader( FVF );

		// Set the material
		g_pd3dDevice->SetMaterial( &m_MaterialDef );

		// Set the texture
		g_pd3dDevice->SetTexture( 0, 0 );

		g_pd3dDevice->SetTextureStageState(0, D3DTSS_ADDRESSV, D3DTADDRESS_WRAP);
		g_pd3dDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
		g_pd3dDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TFACTOR);
		g_pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
		g_pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_DIFFUSE);
		g_pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_TFACTOR);
		D3DCOLOR DiffuseColor = D3DCOLOR_RGBA((BYTE)(m_MaterialDef.Diffuse.r * 255), 
											  (BYTE)(m_MaterialDef.Diffuse.g * 255),
											  (BYTE)(m_MaterialDef.Diffuse.b * 255), 
											  (BYTE)(m_MaterialDef.Diffuse.a * 255));
		g_pd3dDevice->SetRenderState(D3DRS_TEXTUREFACTOR, DiffuseColor);

		g_pd3dDevice->SetRenderState( D3DRS_ZENABLE, D3DZB_FALSE );

		// Edge Aliasing mode disable
		g_pd3dDevice->SetRenderState(D3DRS_EDGEANTIALIAS, TRUE);
		g_pd3dDevice->SetRenderState(D3DRS_MULTISAMPLEANTIALIAS, FALSE);

		g_pd3dDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
	}
	else
	{
		return E_FAIL;
	}

	return S_OK;
}

//-----------------------------------------------------------------------------
// Name: CSolidTexMaterial()
// Desc: Sets initial values of member variables
//-----------------------------------------------------------------------------
CSolidTexMaterial::CSolidTexMaterial() :
	CBaseMaterial()
{
	m_sTexName		= NULL;
	m_nTextureIndex = -1; 
}

//-----------------------------------------------------------------------------
// Name: CSolidTexMaterial()
// Desc: Sets initial values of member variables
//-----------------------------------------------------------------------------
CSolidTexMaterial::CSolidTexMaterial(char *sMatName, int index) :
	CBaseMaterial(sMatName, index)
{
	m_sTexName		= NULL;
	m_nTextureIndex = -1;
}

//-----------------------------------------------------------------------------
// Name: ~CSolidTexMaterial()
// Desc: Performs any needed cleanup before shutting down the object
//-----------------------------------------------------------------------------
CSolidTexMaterial::~CSolidTexMaterial()
{
	if( m_sTexName != NULL )
		delete [] m_sTexName;
}

//-----------------------------------------------------------------------------
// Name: SetTexture()
// Desc: Calls into the texture cache to load a texture for the requested
//		 material
//-----------------------------------------------------------------------------
HRESULT CSolidTexMaterial::SetTexture(char *texFileName)
{
	// Store the texture name in the material so we can request it during setup
	m_sTexName = new char[strlen(texFileName) + 1];
	strcpy(m_sTexName, texFileName);

	return S_OK;
}

//-----------------------------------------------------------------------------
// Name: Setup()
// Desc: Retrieves the texture from the texture cache, and sets up the
//		 appropriate states to render the scene.
//-----------------------------------------------------------------------------
HRESULT CSolidTexMaterial::Setup( DWORD FVF, CTextureCache *pTexCache )
{
	if ( g_pd3dDevice != NULL)
	{
		g_pd3dDevice->SetVertexShader( FVF );

		// set the texture
		IDirect3DBaseTexture8 *pTexture = pTexCache->FindTexture(&m_nTextureIndex, m_sTexName);

		if(pTexture == NULL)	// texture was not found
			return E_FAIL;

		g_pd3dDevice->SetTexture( 0, pTexture );

		// set the material
		g_pd3dDevice->SetMaterial( &m_MaterialDef );

		g_pd3dDevice->SetTextureStageState(0, D3DTSS_ADDRESSU, D3DTADDRESS_WRAP);
		g_pd3dDevice->SetTextureStageState(0, D3DTSS_ADDRESSV, D3DTADDRESS_WRAP);

		g_pd3dDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
		g_pd3dDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_DIFFUSE);
		g_pd3dDevice->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_TEXTURE);
		g_pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
		g_pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_DIFFUSE);
		g_pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_TEXTURE);

		g_pd3dDevice->SetRenderState( D3DRS_ZENABLE, D3DZB_FALSE );
		g_pd3dDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE);

		// Edge Aliasing mode disable
		g_pd3dDevice->SetRenderState(D3DRS_EDGEANTIALIAS, TRUE);
		g_pd3dDevice->SetRenderState(D3DRS_MULTISAMPLEANTIALIAS, FALSE);
	}
	else
	{
		return E_FAIL;
	}

	return S_OK;
}

//-----------------------------------------------------------------------------
// Name: CBackingTexMaterial()
// Desc: Sets initial values of member variables
//-----------------------------------------------------------------------------
CBackingTexMaterial::CBackingTexMaterial() :
	CBaseMaterial()
{
	m_sTexName		= NULL;
	m_nTextureIndex = -1; 
}

//-----------------------------------------------------------------------------
// Name: CBackingTexMaterial()
// Desc: Sets initial values of member variables
//-----------------------------------------------------------------------------
CBackingTexMaterial::CBackingTexMaterial(char *sMatName, int index) :
	CBaseMaterial(sMatName, index)
{
	m_sTexName		= NULL;
	m_nTextureIndex = -1;
}

//-----------------------------------------------------------------------------
// Name: ~CBackingTexMaterial()
// Desc: Performs any needed cleanup before shutting down the object
//-----------------------------------------------------------------------------
CBackingTexMaterial::~CBackingTexMaterial()
{
	if( m_sTexName != NULL )
		delete [] m_sTexName;
}

//-----------------------------------------------------------------------------
// Name: SetTexture()
// Desc: Calls into the texture cache to load a texture for the requested
//		 material
//-----------------------------------------------------------------------------
HRESULT CBackingTexMaterial::SetTexture(char *texFileName)
{
	// Store the texture name in the material so we can request it during setup
	m_sTexName = new char[strlen(texFileName) + 1];
	strcpy(m_sTexName, texFileName);

	return S_OK;
}

//-----------------------------------------------------------------------------
// Name: Setup()
// Desc: Retrieves the texture from the texture cache, and sets up the
//		 appropriate states to render the scene.
//-----------------------------------------------------------------------------
HRESULT CBackingTexMaterial::Setup( DWORD FVF, CTextureCache *pTexCache )
{
	if ( g_pd3dDevice != NULL)
	{
		g_pd3dDevice->SetVertexShader( FVF );

		// set the texture
		IDirect3DBaseTexture8 *pTexture = pTexCache->FindTexture(&m_nTextureIndex, m_sTexName);

		if(pTexture == NULL)	// texture was not found
			return E_FAIL;

		g_pd3dDevice->SetTexture( 0, pTexture );

		// set the material
		g_pd3dDevice->SetMaterial( &m_MaterialDef );

		g_pd3dDevice->SetTextureStageState(0, D3DTSS_ADDRESSV, D3DTADDRESS_WRAP);
		g_pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
		g_pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
		g_pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_TFACTOR);
		g_pd3dDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
		g_pd3dDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TFACTOR);
		g_pd3dDevice->SetRenderState(D3DRS_TEXTUREFACTOR, D3DCOLOR_RGBA( 0, 0, 0, 255));

		g_pd3dDevice->SetRenderState( D3DRS_ZENABLE, D3DZB_FALSE );
		g_pd3dDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE);

		// Edge Aliasing mode disable
		g_pd3dDevice->SetRenderState(D3DRS_EDGEANTIALIAS, FALSE);
		g_pd3dDevice->SetRenderState(D3DRS_MULTISAMPLEANTIALIAS, TRUE);
	}
	else
	{
		return E_FAIL;
	}

	return S_OK;
}

//-----------------------------------------------------------------------------
// Name: CFalloffMaterial()
// Desc: Sets initial values of member variables
//-----------------------------------------------------------------------------
CFalloffMaterial::CFalloffMaterial(char *sMatName, int index) :
	CBaseMaterial(sMatName, index)
{
	m_colorSide  = D3DCOLOR_RGBA(255, 255, 255, 255);
	m_colorFront = D3DCOLOR_RGBA(0, 0, 0, 0,);
}

//-----------------------------------------------------------------------------
// Name: ~CFalloffMaterial()
// Desc: Performs any needed cleanup before shutting down the object
//-----------------------------------------------------------------------------
CFalloffMaterial::~CFalloffMaterial()
{
}

//-----------------------------------------------------------------------------
// Name: SetFalloffColors()
// Desc: Sets the side color/alpha and front color/alpha for the side fade
//		 effect
//-----------------------------------------------------------------------------
void CFalloffMaterial::SetFalloffColors(D3DCOLOR side, D3DCOLOR front)
{
	m_colorSide  = side;
	m_colorFront = front;
}

//-----------------------------------------------------------------------------
// Name: Setup()
// Desc: Sets up the appropriate states to render the material.
//-----------------------------------------------------------------------------
HRESULT CFalloffMaterial::Setup( DWORD FVF, CTextureCache *pTexCache )
{
	if( g_pd3dDevice != NULL )
	{
		g_pd3dDevice->SetTextureStageState(0, D3DTSS_ADDRESSV, D3DTADDRESS_WRAP);
		
		g_pd3dDevice->SetVertexShader(GetEffectShader(1, FVF));
		SetFalloffShaderValues(m_colorSide, m_colorFront);
		SetFalloffShaderFrameValues();	
		
		g_pd3dDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_DISABLE);
		g_pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_DISABLE);

		g_pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, 1);
		g_pd3dDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
		g_pd3dDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

		g_pd3dDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
		g_pd3dDevice->SetRenderState( D3DRS_ZENABLE, D3DZB_FALSE );
   		g_pd3dDevice->SetRenderState( D3DRS_LIGHTING, 0 );

		// Edge Aliasing mode enable
		g_pd3dDevice->SetRenderState(D3DRS_EDGEANTIALIAS, TRUE);
		g_pd3dDevice->SetRenderState(D3DRS_MULTISAMPLEANTIALIAS, FALSE);
	}
	else
	{
		return E_FAIL;
	}
	

	return S_OK;
}

//-----------------------------------------------------------------------------
// Name: CFalloffTexMaterial()
// Desc: Sets initial values of member variables
//-----------------------------------------------------------------------------
CFalloffTexMaterial::CFalloffTexMaterial(char *sMatName, int index) :
	CBaseMaterial(sMatName, index)
{
	m_colorSide  = D3DCOLOR_RGBA(255, 255, 255, 255);
	m_colorFront = D3DCOLOR_RGBA(0, 0, 0, 0,);

	m_sTexName		= NULL;
	m_nTextureIndex = -1;
}

//-----------------------------------------------------------------------------
// Name: ~CFalloffTexMaterial()
// Desc: Performs any needed cleanup before shutting down the object
//-----------------------------------------------------------------------------
CFalloffTexMaterial::~CFalloffTexMaterial()
{
	if( m_sTexName != NULL )
		delete [] m_sTexName;
}

//-----------------------------------------------------------------------------
// Name: SetFalloffColors()
// Desc: Sets the side color/alpha and front color/alpha for the side fade
//		 effect
//-----------------------------------------------------------------------------
void CFalloffTexMaterial::SetFalloffColors(D3DCOLOR side, D3DCOLOR front)
{
	m_colorSide  = side;
	m_colorFront = front;
}

//-----------------------------------------------------------------------------
// Name: SetTexture()
// Desc: Calls into the texture cache to load a texture for the requested
//		 material
//-----------------------------------------------------------------------------
HRESULT CFalloffTexMaterial::SetTexture(char *texFileName)
{
	// Store the texture name in the material so we can request it during setup
	m_sTexName = new char[strlen(texFileName) + 1];
	strcpy(m_sTexName, texFileName);

	return S_OK;
}

//-----------------------------------------------------------------------------
// Name: Setup()
// Desc: Sets up the appropriate states to render the material.
//-----------------------------------------------------------------------------
HRESULT CFalloffTexMaterial::Setup( DWORD FVF, CTextureCache *pTexCache )
{
	if( g_pd3dDevice != NULL )
	{
		SetFalloffShaderFrameValues();

		g_pd3dDevice->SetVertexShader(GetEffectShader(3, FVF));

		// set the texture
		IDirect3DBaseTexture8 *pTexture = pTexCache->FindTexture(&m_nTextureIndex, m_sTexName);

		if(pTexture == NULL)	// texture was not found
			return E_FAIL;

		g_pd3dDevice->SetTexture( 0, pTexture );

		// set the material
		g_pd3dDevice->SetMaterial( &m_MaterialDef );

		g_pd3dDevice->SetTextureStageState(0, D3DTSS_ADDRESSV, D3DTADDRESS_WRAP);
		g_pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
		g_pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_DIFFUSE);
		g_pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_TEXTURE);
		g_pd3dDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
		g_pd3dDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_DIFFUSE);
		g_pd3dDevice->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_TEXTURE);
		g_pd3dDevice->SetTextureStageState(0, D3DTSS_TEXCOORDINDEX, 0);

		g_pd3dDevice->SetTextureStageState(1, D3DTSS_ALPHAOP, D3DTOP_DISABLE);
		g_pd3dDevice->SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_DISABLE);

		g_pd3dDevice->SetTextureStageState(0, D3DTSS_ADDRESSU, D3DTADDRESS_WRAP);
		g_pd3dDevice->SetTextureStageState(0, D3DTSS_ADDRESSV, D3DTADDRESS_WRAP);

//		g_pd3dDevice->SetRenderState( D3DRS_ZENABLE, TRUE ); 
		g_pd3dDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE);

		// Edge Aliasing mode enable
		g_pd3dDevice->SetRenderState(D3DRS_EDGEANTIALIAS, FALSE);
		g_pd3dDevice->SetRenderState(D3DRS_MULTISAMPLEANTIALIAS, TRUE);
        /*
		g_pd3dDevice->SetRenderState( D3DRS_ZENABLE, D3DZB_FALSE );
		g_pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, 1);
		g_pd3dDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
		g_pd3dDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

   		g_pd3dDevice->SetRenderState( D3DRS_LIGHTING, 0 );*/

		SetFalloffShaderValues(m_colorSide, m_colorFront);
	}
	else
	{
		return E_FAIL;
	}

	return S_OK;
}
