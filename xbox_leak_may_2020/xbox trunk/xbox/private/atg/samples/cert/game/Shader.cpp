//-----------------------------------------------------------------------------
// File: Shader.cpp
//
// Desc: Shader classes for advanced lighting
//
// Hist: 04.10.01 - New for May XDK release 
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#include "Shader.h"
#include "Globals.h"
#include "Light.h"
#include "File.h"




//-----------------------------------------------------------------------------
// Statics
//-----------------------------------------------------------------------------
LightList Shader::m_LightList;




//-----------------------------------------------------------------------------
// Name: CreateShaderOfType()
// Desc: Make a shader of the specified type
//-----------------------------------------------------------------------------
Shader* Shader::CreateShaderOfType( DWORD dwShaderType )
{
    switch( dwShaderType )
    {
//      case TypeShaderFlat:         return new ShaderFlat();
        case TypeShaderLit:          return new ShaderLit();
//      case TypeShaderTexturedLit:  return new ShaderTexturedLit();
//      case TypeShaderTexturedBump: return new ShaderTexturedBump();

        default: assert( FALSE ); break;
    }
    return NULL;
}




#if (0)
//-----------------------------------------------------------------------------
// Name: ShaderFlat()
// Desc: Construct empty shader
//-----------------------------------------------------------------------------
ShaderFlat::ShaderFlat()
:
    m_Material()
{
    ZeroMemory( &m_Material, sizeof(D3DMATERIAL8) );
}




//-----------------------------------------------------------------------------
// Name: ShaderFlat()
// Desc: Initialize with the given color
//-----------------------------------------------------------------------------
ShaderFlat::ShaderFlat( FLOAT fRed, FLOAT fGreen, FLOAT fBlue )
:
    m_Material()
{
    ZeroMemory( &m_Material, sizeof(D3DMATERIAL8) );
    m_Material.Diffuse.r = m_Material.Ambient.r = fRed;
    m_Material.Diffuse.g = m_Material.Ambient.g = fGreen;
    m_Material.Diffuse.b = m_Material.Ambient.b = fBlue;
    m_Material.Diffuse.a = m_Material.Ambient.a = 1.0f;
}




//-----------------------------------------------------------------------------
// Name: Save()
// Desc: Write shader info to file
//-----------------------------------------------------------------------------
HRESULT ShaderFlat::Save( const File& file )
{
    if( !file.Write( &m_Material.Diffuse.r, sizeof(m_Material.Diffuse.r) ) ||
        !file.Write( &m_Material.Diffuse.g, sizeof(m_Material.Diffuse.g) ) ||
        !file.Write( &m_Material.Diffuse.b, sizeof(m_Material.Diffuse.b) ) ||
        !file.Write( &m_Material.Diffuse.a, sizeof(m_Material.Diffuse.a) ) )
    {
        return E_FAIL;
    }

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: Load()
// Desc: Load shader from file
//-----------------------------------------------------------------------------
HRESULT ShaderFlat::Load( const File& file )
{
    DWORD r;
    if( !file.Read( &m_Material.Diffuse.r, sizeof(m_Material.Diffuse.r), r ) ||
        !file.Read( &m_Material.Diffuse.g, sizeof(m_Material.Diffuse.g), r ) ||
        !file.Read( &m_Material.Diffuse.b, sizeof(m_Material.Diffuse.b), r ) ||
        !file.Read( &m_Material.Diffuse.a, sizeof(m_Material.Diffuse.a), r ) )
    {
        return E_FAIL;
    }

    m_Material.Ambient.r = m_Material.Diffuse.r;
    m_Material.Ambient.g = m_Material.Diffuse.g;
    m_Material.Ambient.b = m_Material.Diffuse.b;
    m_Material.Ambient.a = m_Material.Diffuse.a;

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: Output()
// Desc: Output shader
//-----------------------------------------------------------------------------
VOID ShaderFlat::Output( INT )
{
    g_pd3dDevice->SetMaterial( &m_Material );
    g_pd3dDevice->SetVertexShader( D3DFVF_XYZ | D3DFVF_NORMAL );

    g_pd3dDevice->SetRenderState( D3DRS_LIGHTING,       TRUE );
    g_pd3dDevice->SetRenderState( D3DRS_SPECULARENABLE, FALSE );
    g_pd3dDevice->SetRenderState( D3DRS_ZFUNC,          D3DCMP_LESSEQUAL );
    g_pd3dDevice->SetRenderState( D3DRS_CULLMODE,       D3DCULL_CCW );
    g_pd3dDevice->SetRenderState( D3DRS_SHADEMODE,      D3DSHADE_GOURAUD );

    for( INT i = 0; i < 4; ++i )
        g_pd3dDevice->SetTexture( i, 0 );
}
#endif




//-----------------------------------------------------------------------------
// Name: ShaderLit()
// Desc: Construct empty shader
//-----------------------------------------------------------------------------
ShaderLit::ShaderLit()
:
    m_Material()
{
    ZeroMemory( &m_Material, sizeof(D3DMATERIAL8) );
}




//-----------------------------------------------------------------------------
// Name: ShaderLit()
// Desc: Initialize from given material
//-----------------------------------------------------------------------------
ShaderLit::ShaderLit( const D3DMATERIAL8& d3dMat )
:
    m_Material( d3dMat )
{
}




//-----------------------------------------------------------------------------
// Name: Save()
// Desc: Save shader info to file
//-----------------------------------------------------------------------------
HRESULT ShaderLit::Save( const File& file )
{
    if( !file.Write( &m_Material, sizeof( m_Material) ) )
        return E_FAIL;
    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: Load()
// Desc: Load shader info from file
//-----------------------------------------------------------------------------
HRESULT ShaderLit::Load( const File& file )
{
    DWORD r;
    if( !file.Read( &m_Material, sizeof(m_Material), r ) )
        return E_FAIL;
    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: Output()
// Desc: Output shader
//-----------------------------------------------------------------------------
VOID ShaderLit::Output( INT )
{
    g_pd3dDevice->SetMaterial( &m_Material );
    g_pd3dDevice->SetVertexShader( D3DFVF_XYZ | D3DFVF_NORMAL );

    g_pd3dDevice->SetRenderState( D3DRS_LIGHTING,       TRUE );
    g_pd3dDevice->SetRenderState( D3DRS_SPECULARENABLE, FALSE );
    g_pd3dDevice->SetRenderState( D3DRS_ZFUNC,          D3DCMP_LESSEQUAL );
    g_pd3dDevice->SetRenderState( D3DRS_CULLMODE,       D3DCULL_CCW );
    g_pd3dDevice->SetRenderState( D3DRS_SHADEMODE,      D3DSHADE_GOURAUD );

    for( INT i = 0; i < 4; ++i )
        g_pd3dDevice->SetTexture( i, 0 );
}




#if (0)
//-----------------------------------------------------------------------------
// Name: ShaderTexturedLit()
// Desc: Construct empty shader
//-----------------------------------------------------------------------------
ShaderTexturedLit::ShaderTexturedLit()
:
    m_pTexture( NULL ),
    m_Material()
{
    ZeroMemory( &m_Material, sizeof(D3DMATERIAL8) );
}




//-----------------------------------------------------------------------------
// Name: ShaderTexturedLit()
// Desc: Construct from given texture and material
//-----------------------------------------------------------------------------
ShaderTexturedLit::ShaderTexturedLit( Texture* pTexture, 
                                      const D3DMATERIAL8& d3dMat )
:
    m_pTexture( pTexture ),
    m_Material( d3dMat )
{
}




//-----------------------------------------------------------------------------
// Name: ~ShaderTexturedLit()
// Desc: Destroy shader
//-----------------------------------------------------------------------------
ShaderTexturedLit::~ShaderTexturedLit()
{
    Texture::ReleaseTexture( m_pTexture );
}




//-----------------------------------------------------------------------------
// Name: Save()
// Desc: Save shader info to file
//-----------------------------------------------------------------------------
HRESULT ShaderTexturedLit::Save( const File& file )
{
    if( !file.Write( &m_Material, sizeof(m_Material) ) )
        return E_FAIL;

    if( !Texture::SaveTextureID( file, m_pTexture ) )
        return E_FAIL;

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: Load()
// Desc: Load shader info from file
//-----------------------------------------------------------------------------
HRESULT ShaderTexturedLit::Load( const File& file )
{
    DWORD r;
    if( !file.Read( &m_Material, sizeof( m_Material ), r ) )
        return E_FAIL;

    m_pTexture = Texture::CreateTextureFromID( file );
    if( m_pTexture == NULL )
        return E_FAIL;

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: Output()
// Desc: Output shader
//-----------------------------------------------------------------------------
VOID ShaderTexturedLit::Output( INT )
{
    g_pd3dDevice->SetMaterial( &m_Material );
    g_pd3dDevice->SetVertexShader( D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1 );

    g_pd3dDevice->SetRenderState( D3DRS_LIGHTING,       TRUE );
    g_pd3dDevice->SetRenderState( D3DRS_SPECULARENABLE, FALSE );
    g_pd3dDevice->SetRenderState( D3DRS_ZFUNC,          D3DCMP_LESSEQUAL );
    g_pd3dDevice->SetRenderState( D3DRS_CULLMODE,       D3DCULL_CCW );
    g_pd3dDevice->SetRenderState( D3DRS_SHADEMODE,      D3DSHADE_GOURAUD );

    g_pd3dDevice->SetTexture( 0, ( m_pTexture != NULL ) ? 
                                    m_pTexture->GetD3DTexture() : 0 );

    for( INT i = 1; i < 4; ++i )
        g_pd3dDevice->SetTexture( i, 0 );
}




//-----------------------------------------------------------------------------
// Name: ShaderTexturedBump()
// Desc: Construct empty shader
//-----------------------------------------------------------------------------
ShaderTexturedBump::ShaderTexturedBump()
:
    m_pImageTexture             ( NULL ),
    m_pBumpTexture              ( NULL ),
    m_dwBumpVertexShaderHandle  ( 0 ),
    m_dwImageVertexShaderHandle ( 0 ),
    m_dwBumpPixelShaderHandle   ( 0 ),
    m_dwImagePixelShaderHandle  ( 0 ),
    m_pNormalizationCubemap     ( NULL )
{
    InitShaders(); 
}




//-----------------------------------------------------------------------------
// Name: ShaderTexturedBump()
// Desc: Construct shader given texture and bump
//-----------------------------------------------------------------------------
ShaderTexturedBump::ShaderTexturedBump( Texture* pImageTexture, 
                                        Texture* pBumpTexture )
:
    m_pImageTexture             ( pImageTexture ),
    m_pBumpTexture              ( pBumpTexture ),
    m_dwBumpVertexShaderHandle  ( 0 ),
    m_dwImageVertexShaderHandle ( 0 ),
    m_dwBumpPixelShaderHandle   ( 0 ),
    m_dwImagePixelShaderHandle  ( 0 ),
    m_pNormalizationCubemap     ( NULL )
{
    InitShaders();
}




//-----------------------------------------------------------------------------
// Name: ~ShaderTexturedBump()
// Desc: Destroy shader
//-----------------------------------------------------------------------------
ShaderTexturedBump::~ShaderTexturedBump()
{
    Texture::ReleaseTexture( m_pImageTexture );
    Texture::ReleaseTexture( m_pBumpTexture );

    if( m_dwBumpVertexShaderHandle )
        g_pd3dDevice->DeleteVertexShader( m_dwBumpVertexShaderHandle );

    if( m_dwImageVertexShaderHandle )
        g_pd3dDevice->DeleteVertexShader( m_dwImageVertexShaderHandle );

    if( m_dwBumpPixelShaderHandle )
        g_pd3dDevice->DeletePixelShader( m_dwBumpPixelShaderHandle );

    if( m_dwImagePixelShaderHandle )
        g_pd3dDevice->DeletePixelShader( m_dwImagePixelShaderHandle );

    SAFE_RELEASE( m_pNormalizationCubemap );
}




//-----------------------------------------------------------------------------
// Name: Save()
// Desc: Save shader info to file
//-----------------------------------------------------------------------------
HRESULT ShaderTexturedBump::Save( const File& file )
{
    if( !Texture::SaveTextureID( file, m_pImageTexture ) ||
        !Texture::SaveTextureID( file, m_pBumpTexture ) )
    {
        return E_FAIL;
    }

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: Load()
// Desc: Load shader into from file
//-----------------------------------------------------------------------------
HRESULT ShaderTexturedBump::Load( const File& file )
{
    m_pImageTexture = Texture::CreateTextureFromID( file );
    m_pBumpTexture = Texture::CreateTextureFromID( file );
    
    if( m_pImageTexture == NULL || m_pBumpTexture == NULL )
        return E_FAIL;

    return S_OK;
}





//-----------------------------------------------------------------------------
// Name: GetNumPasses()
// Desc: Number of passes required by shader
//-----------------------------------------------------------------------------
INT ShaderTexturedBump::GetNumPasses()
{
#if defined(_XBOX)
	return m_LightList.size()+1;
#else
	return 1;
#endif
}




//-----------------------------------------------------------------------------
// Name: Output()
// Desc: Output shader
//-----------------------------------------------------------------------------
VOID ShaderTexturedBump::Output( INT iPass )
{
#if defined(_XBOX)

	if (iPass < (int)m_LightList.size())
	{
		// Light #iPass
		if (iPass == 0)
		{
			// First pass.
			g_pd3dDevice->SetRenderState( D3DRS_LIGHTING, FALSE );
			g_pd3dDevice->SetRenderState( D3DRS_SPECULARENABLE, FALSE );
			g_pd3dDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_CCW );
			g_pd3dDevice->SetRenderState( D3DRS_SHADEMODE, D3DSHADE_GOURAUD );

			g_pd3dDevice->SetRenderState( D3DRS_ZFUNC, D3DCMP_LESSEQUAL );
			g_pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE, TRUE );
			g_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );

			g_pd3dDevice->SetTexture( 0, m_pBumpTexture->GetD3DTexture() );
			g_pd3dDevice->SetTextureStageState( 0, D3DTSS_ADDRESSU, D3DTADDRESS_WRAP );
			g_pd3dDevice->SetTextureStageState( 0, D3DTSS_ADDRESSV, D3DTADDRESS_WRAP );
			g_pd3dDevice->SetTextureStageState( 0, D3DTSS_MAGFILTER, D3DTEXF_LINEAR );
			g_pd3dDevice->SetTextureStageState( 0, D3DTSS_MIPFILTER, D3DTEXF_LINEAR );

			g_pd3dDevice->SetTexture( 1, m_pNormalizationCubemap );
			g_pd3dDevice->SetTextureStageState( 1, D3DTSS_ADDRESSU, D3DTADDRESS_CLAMP );
			g_pd3dDevice->SetTextureStageState( 1, D3DTSS_ADDRESSV, D3DTADDRESS_CLAMP );
			g_pd3dDevice->SetTextureStageState( 1, D3DTSS_ADDRESSW, D3DTADDRESS_CLAMP );
			g_pd3dDevice->SetTextureStageState( 1, D3DTSS_MAGFILTER, D3DTEXF_LINEAR );
			g_pd3dDevice->SetTextureStageState( 1, D3DTSS_MIPFILTER, D3DTEXF_NONE );

			g_pd3dDevice->SetTexture( 2, m_pNormalizationCubemap );
			g_pd3dDevice->SetTextureStageState( 2, D3DTSS_ADDRESSU, D3DTADDRESS_CLAMP );
			g_pd3dDevice->SetTextureStageState( 2, D3DTSS_ADDRESSV, D3DTADDRESS_CLAMP );
			g_pd3dDevice->SetTextureStageState( 2, D3DTSS_ADDRESSW, D3DTADDRESS_CLAMP );
			g_pd3dDevice->SetTextureStageState( 2, D3DTSS_MAGFILTER, D3DTEXF_LINEAR );
			g_pd3dDevice->SetTextureStageState( 2, D3DTSS_MIPFILTER, D3DTEXF_NONE );

			// Set vertex shader.
			g_pd3dDevice->SetVertexShader( m_dwBumpVertexShaderHandle );

			// Set pixel shader.
			g_pd3dDevice->SetPixelShader( m_dwBumpPixelShaderHandle );

			// Get current transforms.
			D3DXMATRIX matWorld, matView, matProj;
			g_pd3dDevice->GetTransform( D3DTS_WORLD, &matWorld );		// local->world
			g_pd3dDevice->GetTransform( D3DTS_VIEW, &matView );			// world->view
			g_pd3dDevice->GetTransform( D3DTS_PROJECTION, &matProj );	// view->screen

			// Set composite matrix.
			D3DXMATRIX matComposite;
			D3DXMatrixMultiply( &matComposite, &matWorld, &matView );
			D3DXMatrixMultiply( &matComposite, &matComposite, &matProj );
			D3DXMatrixTranspose( &matComposite, &matComposite );

			g_pd3dDevice->SetVertexShaderConstant( 0, &matComposite, 4 );

			// Set viewer position in object space.
			D3DXMATRIX matViewInverse, matWorldInverse;
			D3DXMatrixInverse( &matViewInverse, 0, &matView );
			D3DXMatrixInverse( &matWorldInverse, 0, &matWorld );

			D3DXVECTOR4 v4LocalViewerPos(0.0f,0.0f,0.0f,1.0f);
			D3DXVec4Transform( &v4LocalViewerPos, &v4LocalViewerPos, &matViewInverse );
			D3DXVec4Transform( &v4LocalViewerPos, &v4LocalViewerPos, &matWorldInverse );

			g_pd3dDevice->SetVertexShaderConstant( 8, &v4LocalViewerPos, 1 );
		}
		else if (iPass == 1)
		{
			// Subsequent passes.
			g_pd3dDevice->SetRenderState( D3DRS_ZFUNC, D3DCMP_EQUAL );
			g_pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE, FALSE );

			g_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
			g_pd3dDevice->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_ONE );
			g_pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_ONE );
		}

		// All passes.

		// Set light volume.
		g_pd3dDevice->SetTexture( 3, m_LightList[iPass]->GetLightFalloffVolume() );
		g_pd3dDevice->SetTextureStageState( 3, D3DTSS_ADDRESSU, D3DTADDRESS_BORDER );
		g_pd3dDevice->SetTextureStageState( 3, D3DTSS_ADDRESSV, D3DTADDRESS_BORDER );
		g_pd3dDevice->SetTextureStageState( 3, D3DTSS_ADDRESSW, D3DTADDRESS_BORDER );
		g_pd3dDevice->SetTextureStageState( 3, D3DTSS_MAGFILTER, D3DTEXF_LINEAR );
		g_pd3dDevice->SetTextureStageState( 3, D3DTSS_MIPFILTER, D3DTEXF_NONE );
		g_pd3dDevice->SetTextureStageState( 3, D3DTSS_BORDERCOLOR, 0 );

		// Get world transform.
		D3DXMATRIX matWorld, matWorldInverse;
		g_pd3dDevice->GetTransform( D3DTS_WORLD, &matWorld );
		D3DXMatrixInverse( &matWorldInverse, 0, &matWorld );

		// Set object space to light space transform.
		D3DXMATRIX matLocalToLight = m_LightList[iPass]->GetWorldToLightTransform();
		D3DXMatrixMultiply( &matLocalToLight, &matWorld, &matLocalToLight );
		D3DXMatrixTranspose( &matLocalToLight, &matLocalToLight );
		g_pd3dDevice->SetVertexShaderConstant( 4, &matLocalToLight, 3 );

		// Light position in object space.
		D3DXVECTOR4 v4LocalLightPos = m_LightList[iPass]->GetPosition();
		D3DXVec4Transform( &v4LocalLightPos, &v4LocalLightPos, &matWorldInverse );
		g_pd3dDevice->SetVertexShaderConstant( 7, &v4LocalLightPos, 1 );

		float fDiffuse[4] = { 0.9f, 0.9f, 0.9f, 0.9f };
		float fSpecular[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
		float fAmbient[4] = { 0.2f, 0.2f, 0.2f, 0.2f };
		g_pd3dDevice->SetPixelShaderConstant( 0, fDiffuse, 1 );
		g_pd3dDevice->SetPixelShaderConstant( 1, fSpecular, 1 );
		g_pd3dDevice->SetPixelShaderConstant( 2, fAmbient, 1 );
	}
	else
	{
		// Final pass, modulate color with lighting.
		g_pd3dDevice->SetRenderState( D3DRS_ZFUNC, D3DCMP_EQUAL );
		g_pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE, FALSE );

		g_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
		g_pd3dDevice->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_DESTCOLOR );
		g_pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_ZERO );
//		g_pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_SRCCOLOR );

		g_pd3dDevice->SetVertexShader( m_dwImageVertexShaderHandle );
		g_pd3dDevice->SetPixelShader( m_dwImagePixelShaderHandle );

		// Modulate image texture with lighting.
		g_pd3dDevice->SetTexture( 0, m_pImageTexture->GetD3DTexture() );
		g_pd3dDevice->SetTextureStageState( 0, D3DTSS_ADDRESSU, D3DTADDRESS_WRAP );
		g_pd3dDevice->SetTextureStageState( 0, D3DTSS_ADDRESSV, D3DTADDRESS_WRAP );
		g_pd3dDevice->SetTextureStageState( 0, D3DTSS_MAGFILTER, D3DTEXF_LINEAR );
		g_pd3dDevice->SetTextureStageState( 0, D3DTSS_MIPFILTER, D3DTEXF_LINEAR );

		g_pd3dDevice->SetTexture( 1, 0 );

		g_pd3dDevice->SetTexture( 2, 0 );

		g_pd3dDevice->SetTexture( 3, 0 );
	}

#else

	g_pd3dDevice->SetVertexShader( m_dwImageVertexShaderHandle );

	g_pd3dDevice->SetRenderState( D3DRS_LIGHTING,	   FALSE );
	g_pd3dDevice->SetRenderState( D3DRS_SPECULARENABLE, FALSE );
	g_pd3dDevice->SetRenderState( D3DRS_ZFUNC,	  D3DCMP_LESSEQUAL );
	g_pd3dDevice->SetRenderState( D3DRS_CULLMODE,  D3DCULL_CCW );
	g_pd3dDevice->SetRenderState( D3DRS_SHADEMODE, D3DSHADE_GOURAUD );

	if (m_pImageTexture)
		g_pd3dDevice->SetTexture( 0, m_pImageTexture->GetD3DTexture() );
	else
		g_pd3dDevice->SetTexture( 0, 0 );

#endif
}




//-----------------------------------------------------------------------------
// Name: InitShaders()
// Desc: Initialize shaders
//-----------------------------------------------------------------------------
VOID ShaderTexturedBump::InitShaders()
{
#if defined(_XBOX)

	HRESULT hr;

	// Create vertex shaders for lighting.
    DWORD dwBumpVertexShaderDecl[] =
    {
        D3DVSD_STREAM( 0 ),
        D3DVSD_REG( 0, D3DVSDT_FLOAT3 ), // v0 = position
        D3DVSD_REG( 1, D3DVSDT_FLOAT3 ), // v1 = normal
        D3DVSD_REG( 2, D3DVSDT_FLOAT2 ), // v2 = texture co-oridnate
        D3DVSD_REG( 3, D3DVSDT_FLOAT3 ), // v3 = basis vector S
        D3DVSD_REG( 4, D3DVSDT_FLOAT3 ), // v4 = basis vector T
        D3DVSD_REG( 5, D3DVSDT_FLOAT3 ), // v5 = basis vector SxT
        D3DVSD_END()
    };

	hr = XBUtil_CreateVertexShader( g_pd3dDevice, "Shaders\\BumpMap.xvu",
									dwBumpVertexShaderDecl, &m_dwBumpVertexShaderHandle );
	assert(hr == D3D_OK);

    DWORD dwImageVertexShaderDecl[] =
    {
        D3DVSD_STREAM( 0 ),
        D3DVSD_REG( 0, D3DVSDT_FLOAT3 ), // v0 = position
        D3DVSD_REG( 1, D3DVSDT_FLOAT3 ), // v1 = normal
        D3DVSD_REG( 2, D3DVSDT_FLOAT2 ), // v2 = texture co-oridnate
        D3DVSD_END()
    };

	hr = XBUtil_CreateVertexShader( g_pd3dDevice, "Shaders\\ImageMap.xvu",
									dwImageVertexShaderDecl, &m_dwImageVertexShaderHandle );
	assert(hr == D3D_OK);

	// Create pixel shader for bumpmap lighting.
	D3DPIXELSHADERDEF psd;
	memset( &psd, 0, sizeof(psd) );
	psd.PSCombinerCount=PS_COMBINERCOUNT(
		8,
		PS_COMBINERCOUNT_MUX_MSB | PS_COMBINERCOUNT_UNIQUE_C0 | PS_COMBINERCOUNT_UNIQUE_C1);
	psd.PSTextureModes=PS_TEXTUREMODES(
		PS_TEXTUREMODES_PROJECT2D,
		PS_TEXTUREMODES_CUBEMAP,
		PS_TEXTUREMODES_CUBEMAP,
		PS_TEXTUREMODES_PROJECT3D);

	//------------- Stage 0 -------------
	psd.PSRGBInputs[0]=PS_COMBINERINPUTS(
		PS_REGISTER_T0 | PS_CHANNEL_RGB | PS_INPUTMAPPING_EXPAND_NORMAL,
		PS_REGISTER_T1 | PS_CHANNEL_RGB | PS_INPUTMAPPING_EXPAND_NORMAL,
		PS_REGISTER_T0 | PS_CHANNEL_RGB | PS_INPUTMAPPING_EXPAND_NORMAL,
		PS_REGISTER_T2 | PS_CHANNEL_RGB | PS_INPUTMAPPING_EXPAND_NORMAL);
	psd.PSAlphaInputs[0]=PS_COMBINERINPUTS(
		PS_REGISTER_T1 | PS_CHANNEL_BLUE | PS_INPUTMAPPING_EXPAND_NORMAL,
		PS_REGISTER_ZERO | PS_CHANNEL_ALPHA | PS_INPUTMAPPING_UNSIGNED_INVERT,
		PS_REGISTER_T1 | PS_CHANNEL_BLUE | PS_INPUTMAPPING_EXPAND_NORMAL,
		PS_REGISTER_ZERO | PS_CHANNEL_ALPHA | PS_INPUTMAPPING_UNSIGNED_INVERT);
	psd.PSRGBOutputs[0]=PS_COMBINEROUTPUTS(
		PS_REGISTER_R0,
		PS_REGISTER_R1,
		PS_REGISTER_DISCARD,
		PS_COMBINEROUTPUT_IDENTITY | PS_COMBINEROUTPUT_AB_DOT_PRODUCT | PS_COMBINEROUTPUT_CD_DOT_PRODUCT | PS_COMBINEROUTPUT_AB_CD_SUM);
	psd.PSAlphaOutputs[0]=PS_COMBINEROUTPUTS(
		PS_REGISTER_DISCARD,
		PS_REGISTER_DISCARD,
		PS_REGISTER_R0,
		PS_COMBINEROUTPUT_SHIFTLEFT_2 | PS_COMBINEROUTPUT_AB_MULTIPLY | PS_COMBINEROUTPUT_CD_MULTIPLY | PS_COMBINEROUTPUT_AB_CD_SUM);
	psd.PSConstant0[0]=0x007f7fff;
	psd.PSConstant1[0]=0x00000000;

	// R0 = L dot N
	// R1 = H dot N
	// R0.a = S(self)

	//------------- Stage 1 -------------
	psd.PSRGBInputs[1]=PS_COMBINERINPUTS(
		PS_REGISTER_R1 | PS_CHANNEL_RGB | PS_INPUTMAPPING_UNSIGNED_IDENTITY,
		PS_REGISTER_ZERO | PS_CHANNEL_RGB | PS_INPUTMAPPING_UNSIGNED_INVERT,
		PS_REGISTER_C0 | PS_CHANNEL_RGB | PS_INPUTMAPPING_SIGNED_NEGATE,
		PS_REGISTER_ZERO | PS_CHANNEL_RGB | PS_INPUTMAPPING_UNSIGNED_INVERT);
	psd.PSAlphaInputs[1]=PS_COMBINERINPUTS(
		PS_REGISTER_T2 | PS_CHANNEL_BLUE | PS_INPUTMAPPING_EXPAND_NORMAL,
		PS_REGISTER_R0 | PS_CHANNEL_ALPHA | PS_INPUTMAPPING_UNSIGNED_IDENTITY,
		PS_REGISTER_T2 | PS_CHANNEL_BLUE | PS_INPUTMAPPING_EXPAND_NORMAL,
		PS_REGISTER_R0 | PS_CHANNEL_ALPHA | PS_INPUTMAPPING_UNSIGNED_IDENTITY);
	psd.PSRGBOutputs[1]=PS_COMBINEROUTPUTS(
		PS_REGISTER_DISCARD,
		PS_REGISTER_DISCARD,
		PS_REGISTER_R1,
		PS_COMBINEROUTPUT_SHIFTLEFT_2 | PS_COMBINEROUTPUT_AB_MULTIPLY | PS_COMBINEROUTPUT_CD_MULTIPLY | PS_COMBINEROUTPUT_AB_CD_SUM);
	psd.PSAlphaOutputs[1]=PS_COMBINEROUTPUTS(
		PS_REGISTER_R1,
		PS_REGISTER_DISCARD,
		PS_REGISTER_DISCARD,
		PS_COMBINEROUTPUT_SHIFTLEFT_2 | PS_COMBINEROUTPUT_AB_MULTIPLY | PS_COMBINEROUTPUT_CD_MULTIPLY | PS_COMBINEROUTPUT_AB_CD_SUM);
	psd.PSConstant0[1]=0xc0c0c0c0;
	psd.PSConstant1[1]=0x00000000;

	// R1 = 4*(max(0,(H dot N))-0.75)
	// R1.a = S(self) for specular

	//------------- Stage 2 -------------
	psd.PSRGBInputs[2]=PS_COMBINERINPUTS(
		PS_REGISTER_R1 | PS_CHANNEL_RGB | PS_INPUTMAPPING_UNSIGNED_IDENTITY,
		PS_REGISTER_R1 | PS_CHANNEL_RGB | PS_INPUTMAPPING_UNSIGNED_IDENTITY,
		PS_REGISTER_ZERO | PS_CHANNEL_RGB | PS_INPUTMAPPING_UNSIGNED_IDENTITY,
		PS_REGISTER_ZERO | PS_CHANNEL_RGB | PS_INPUTMAPPING_UNSIGNED_IDENTITY);
	psd.PSAlphaInputs[2]=PS_COMBINERINPUTS(
		PS_REGISTER_ZERO | PS_CHANNEL_ALPHA | PS_INPUTMAPPING_UNSIGNED_IDENTITY,
		PS_REGISTER_ZERO | PS_CHANNEL_ALPHA | PS_INPUTMAPPING_UNSIGNED_IDENTITY,
		PS_REGISTER_ZERO | PS_CHANNEL_ALPHA | PS_INPUTMAPPING_UNSIGNED_IDENTITY,
		PS_REGISTER_ZERO | PS_CHANNEL_ALPHA | PS_INPUTMAPPING_UNSIGNED_IDENTITY);
	psd.PSRGBOutputs[2]=PS_COMBINEROUTPUTS(
		PS_REGISTER_R1,
		PS_REGISTER_DISCARD,
		PS_REGISTER_DISCARD,
		PS_COMBINEROUTPUT_IDENTITY | PS_COMBINEROUTPUT_AB_MULTIPLY | PS_COMBINEROUTPUT_CD_MULTIPLY | PS_COMBINEROUTPUT_AB_CD_SUM);
	psd.PSAlphaOutputs[2]=PS_COMBINEROUTPUTS(
		PS_REGISTER_DISCARD,
		PS_REGISTER_DISCARD,
		PS_REGISTER_DISCARD,
		PS_COMBINEROUTPUT_IDENTITY | PS_COMBINEROUTPUT_AB_MULTIPLY | PS_COMBINEROUTPUT_CD_MULTIPLY | PS_COMBINEROUTPUT_AB_CD_SUM);
	psd.PSConstant0[2]=0x00000000;
	psd.PSConstant1[2]=0x00000000;

	// R1 = max(0,4*(max(0,(H dot N))-0.75))^2

	for (int i = 3; i <= 4; i++)
	{
		//------------- Stage 3,4 -------------
		psd.PSRGBInputs[i]=PS_COMBINERINPUTS(
			PS_REGISTER_R1 | PS_CHANNEL_RGB | PS_INPUTMAPPING_UNSIGNED_IDENTITY,
			PS_REGISTER_R1 | PS_CHANNEL_RGB | PS_INPUTMAPPING_UNSIGNED_IDENTITY,
			PS_REGISTER_ZERO | PS_CHANNEL_RGB | PS_INPUTMAPPING_UNSIGNED_IDENTITY,
			PS_REGISTER_ZERO | PS_CHANNEL_RGB | PS_INPUTMAPPING_UNSIGNED_IDENTITY);
		psd.PSAlphaInputs[i]=PS_COMBINERINPUTS(
			PS_REGISTER_ZERO | PS_CHANNEL_ALPHA | PS_INPUTMAPPING_UNSIGNED_IDENTITY,
			PS_REGISTER_ZERO | PS_CHANNEL_ALPHA | PS_INPUTMAPPING_UNSIGNED_IDENTITY,
			PS_REGISTER_ZERO | PS_CHANNEL_ALPHA | PS_INPUTMAPPING_UNSIGNED_IDENTITY,
			PS_REGISTER_ZERO | PS_CHANNEL_ALPHA | PS_INPUTMAPPING_UNSIGNED_IDENTITY);
		psd.PSRGBOutputs[i]=PS_COMBINEROUTPUTS(
			//PS_REGISTER_DISCARD,
			PS_REGISTER_R1,
			PS_REGISTER_DISCARD,
			PS_REGISTER_DISCARD,
			PS_COMBINEROUTPUT_IDENTITY | PS_COMBINEROUTPUT_AB_MULTIPLY | PS_COMBINEROUTPUT_CD_MULTIPLY | PS_COMBINEROUTPUT_AB_CD_SUM);
		psd.PSAlphaOutputs[i]=PS_COMBINEROUTPUTS(
			PS_REGISTER_DISCARD,
			PS_REGISTER_DISCARD,
			PS_REGISTER_DISCARD,
			PS_COMBINEROUTPUT_IDENTITY | PS_COMBINEROUTPUT_AB_MULTIPLY | PS_COMBINEROUTPUT_CD_MULTIPLY | PS_COMBINEROUTPUT_AB_CD_SUM);
		psd.PSConstant0[i]=0x00000000;
		psd.PSConstant1[i]=0x00000000;
	}

	//------------- Stage 5 -------------
	psd.PSRGBInputs[5]=PS_COMBINERINPUTS(
		PS_REGISTER_R0 | PS_CHANNEL_RGB | PS_INPUTMAPPING_UNSIGNED_IDENTITY,
		PS_REGISTER_R0 | PS_CHANNEL_ALPHA | PS_INPUTMAPPING_UNSIGNED_IDENTITY,
		PS_REGISTER_R1 | PS_CHANNEL_RGB | PS_INPUTMAPPING_UNSIGNED_IDENTITY,
		PS_REGISTER_R1 | PS_CHANNEL_ALPHA | PS_INPUTMAPPING_UNSIGNED_IDENTITY);
	psd.PSAlphaInputs[5]=PS_COMBINERINPUTS(
		PS_REGISTER_ZERO | PS_CHANNEL_ALPHA | PS_INPUTMAPPING_UNSIGNED_IDENTITY,
		PS_REGISTER_ZERO | PS_CHANNEL_ALPHA | PS_INPUTMAPPING_UNSIGNED_IDENTITY,
		PS_REGISTER_ZERO | PS_CHANNEL_ALPHA | PS_INPUTMAPPING_UNSIGNED_IDENTITY,
		PS_REGISTER_ZERO | PS_CHANNEL_ALPHA | PS_INPUTMAPPING_UNSIGNED_IDENTITY);
	psd.PSRGBOutputs[5]=PS_COMBINEROUTPUTS(
		PS_REGISTER_R0,
		PS_REGISTER_R1,
		PS_REGISTER_DISCARD,
		PS_COMBINEROUTPUT_IDENTITY | PS_COMBINEROUTPUT_AB_MULTIPLY | PS_COMBINEROUTPUT_CD_MULTIPLY | PS_COMBINEROUTPUT_AB_CD_SUM);
	psd.PSAlphaOutputs[5]=PS_COMBINEROUTPUTS(
		PS_REGISTER_DISCARD,
		PS_REGISTER_DISCARD,
		PS_REGISTER_DISCARD,
		PS_COMBINEROUTPUT_IDENTITY | PS_COMBINEROUTPUT_AB_MULTIPLY | PS_COMBINEROUTPUT_CD_MULTIPLY | PS_COMBINEROUTPUT_AB_CD_SUM);
	psd.PSConstant0[5]=0x00000000;
	psd.PSConstant1[5]=0x00000000;

	// R0 = S(self)*max(0,(L dot N))
	// R1 = S(self)*max(0,(H dot N))^shininess

	//------------- Stage 6 -------------
	psd.PSRGBInputs[6]=PS_COMBINERINPUTS(
		PS_REGISTER_R0 | PS_CHANNEL_RGB | PS_INPUTMAPPING_UNSIGNED_IDENTITY,
		PS_REGISTER_C0 | PS_CHANNEL_RGB | PS_INPUTMAPPING_UNSIGNED_IDENTITY,
		PS_REGISTER_R1 | PS_CHANNEL_RGB | PS_INPUTMAPPING_UNSIGNED_IDENTITY,
		PS_REGISTER_C1 | PS_CHANNEL_RGB | PS_INPUTMAPPING_UNSIGNED_IDENTITY);
	psd.PSAlphaInputs[6]=PS_COMBINERINPUTS(
		PS_REGISTER_ZERO | PS_CHANNEL_ALPHA | PS_INPUTMAPPING_UNSIGNED_IDENTITY,
		PS_REGISTER_ZERO | PS_CHANNEL_ALPHA | PS_INPUTMAPPING_UNSIGNED_IDENTITY,
		PS_REGISTER_ZERO | PS_CHANNEL_ALPHA | PS_INPUTMAPPING_UNSIGNED_IDENTITY,
		PS_REGISTER_ZERO | PS_CHANNEL_ALPHA | PS_INPUTMAPPING_UNSIGNED_IDENTITY);
	psd.PSRGBOutputs[6]=PS_COMBINEROUTPUTS(
		PS_REGISTER_DISCARD,
		PS_REGISTER_DISCARD,
		PS_REGISTER_R0,
		PS_COMBINEROUTPUT_IDENTITY | PS_COMBINEROUTPUT_AB_MULTIPLY | PS_COMBINEROUTPUT_CD_MULTIPLY | PS_COMBINEROUTPUT_AB_CD_SUM);
	psd.PSAlphaOutputs[6]=PS_COMBINEROUTPUTS(
		PS_REGISTER_DISCARD,
		PS_REGISTER_DISCARD,
		PS_REGISTER_DISCARD,
		PS_COMBINEROUTPUT_IDENTITY | PS_COMBINEROUTPUT_AB_MULTIPLY | PS_COMBINEROUTPUT_CD_MULTIPLY | PS_COMBINEROUTPUT_AB_CD_SUM);
	psd.PSConstant0[6]=0xdfdfdfdf;
	psd.PSConstant1[6]=0xdfdfdfdf;

	// R0 = kDiffuse*S(self)*max(0,(L dot N)) + kSpecular*S(self)*max(0,(H dot N))^shininess

	//------------- Stage 7 -------------
	psd.PSRGBInputs[7]=PS_COMBINERINPUTS(
		PS_REGISTER_R0 | PS_CHANNEL_RGB | PS_INPUTMAPPING_UNSIGNED_IDENTITY,
		PS_REGISTER_T3 | PS_CHANNEL_RGB | PS_INPUTMAPPING_UNSIGNED_IDENTITY,
		PS_REGISTER_C0 | PS_CHANNEL_RGB | PS_INPUTMAPPING_UNSIGNED_IDENTITY,
		PS_REGISTER_ZERO | PS_CHANNEL_RGB | PS_INPUTMAPPING_UNSIGNED_INVERT);
	psd.PSAlphaInputs[7]=PS_COMBINERINPUTS(
		PS_REGISTER_ZERO | PS_CHANNEL_ALPHA | PS_INPUTMAPPING_UNSIGNED_IDENTITY,
		PS_REGISTER_ZERO | PS_CHANNEL_ALPHA | PS_INPUTMAPPING_UNSIGNED_IDENTITY,
		PS_REGISTER_ZERO | PS_CHANNEL_ALPHA | PS_INPUTMAPPING_UNSIGNED_IDENTITY,
		PS_REGISTER_ZERO | PS_CHANNEL_ALPHA | PS_INPUTMAPPING_UNSIGNED_IDENTITY);
	psd.PSRGBOutputs[7]=PS_COMBINEROUTPUTS(
		PS_REGISTER_DISCARD,
		PS_REGISTER_DISCARD,
		PS_REGISTER_R0,
		PS_COMBINEROUTPUT_IDENTITY | PS_COMBINEROUTPUT_AB_MULTIPLY | PS_COMBINEROUTPUT_CD_MULTIPLY | PS_COMBINEROUTPUT_AB_CD_SUM);
	psd.PSAlphaOutputs[7]=PS_COMBINEROUTPUTS(
		PS_REGISTER_DISCARD,
		PS_REGISTER_DISCARD,
		PS_REGISTER_DISCARD,
		PS_COMBINEROUTPUT_IDENTITY | PS_COMBINEROUTPUT_AB_MULTIPLY | PS_COMBINEROUTPUT_CD_MULTIPLY | PS_COMBINEROUTPUT_AB_CD_SUM);
	psd.PSConstant0[7]=0x20202020;
	psd.PSConstant1[7]=0x00000000;

	// R0 = R0*falloff + ambient

	//------------- FinalCombiner -------------
	psd.PSFinalCombinerInputsABCD=PS_COMBINERINPUTS(
		PS_REGISTER_R0 | PS_CHANNEL_RGB | PS_INPUTMAPPING_UNSIGNED_IDENTITY,
		PS_REGISTER_ZERO | PS_CHANNEL_RGB | PS_INPUTMAPPING_UNSIGNED_INVERT,
		PS_REGISTER_ZERO | PS_CHANNEL_RGB | PS_INPUTMAPPING_UNSIGNED_IDENTITY,
		PS_REGISTER_ZERO | PS_CHANNEL_RGB | PS_INPUTMAPPING_UNSIGNED_IDENTITY);
	psd.PSFinalCombinerInputsEFG=PS_COMBINERINPUTS(
		PS_REGISTER_ZERO | PS_CHANNEL_RGB | PS_INPUTMAPPING_UNSIGNED_IDENTITY,
		PS_REGISTER_ZERO | PS_CHANNEL_RGB | PS_INPUTMAPPING_UNSIGNED_IDENTITY,
		PS_REGISTER_ZERO | PS_CHANNEL_ALPHA | PS_INPUTMAPPING_UNSIGNED_INVERT,
		0);

	psd.PSFinalCombinerConstant0 = 0x00000000;
	psd.PSFinalCombinerConstant1 = 0x00000000;

	psd.PSC0Mapping = PS_CONSTANTMAPPING(15,15,15,15,15,15,0,2);
	psd.PSC1Mapping = PS_CONSTANTMAPPING(15,15,15,15,15,15,1,15);
	psd.PSFinalCombinerConstants = PS_FINALCOMBINERCONSTANTS(15,15,PS_GLOBALFLAGS_NO_TEXMODE_ADJUST);

	hr = g_pd3dDevice->CreatePixelShader( &psd, &m_dwBumpPixelShaderHandle );
	assert(hr == D3D_OK);

	// Create pixel shader for image map blend.
	memset( &psd, 0, sizeof(psd) );
	psd.PSCombinerCount=PS_COMBINERCOUNT(
		1,
		PS_COMBINERCOUNT_MUX_MSB | PS_COMBINERCOUNT_UNIQUE_C0 | PS_COMBINERCOUNT_UNIQUE_C1);
	psd.PSTextureModes=PS_TEXTUREMODES(
		PS_TEXTUREMODES_PROJECT2D,
		PS_TEXTUREMODES_NONE,
		PS_TEXTUREMODES_NONE,
		PS_TEXTUREMODES_NONE);

	//------------- Stage 0 -------------
	psd.PSRGBInputs[0]=DWORD(PS_COMBINERINPUTS(
		PS_REGISTER_T0 | PS_CHANNEL_RGB | PS_INPUTMAPPING_SIGNED_IDENTITY,
		PS_REGISTER_ZERO | PS_CHANNEL_RGB | PS_INPUTMAPPING_UNSIGNED_INVERT,
		PS_REGISTER_ZERO | PS_CHANNEL_RGB | PS_INPUTMAPPING_UNSIGNED_IDENTITY,
		PS_REGISTER_ZERO | PS_CHANNEL_RGB | PS_INPUTMAPPING_UNSIGNED_IDENTITY));
	psd.PSAlphaInputs[0]=PS_COMBINERINPUTS(
		PS_REGISTER_T0 | PS_CHANNEL_ALPHA | PS_INPUTMAPPING_EXPAND_NORMAL,
		PS_REGISTER_ZERO | PS_CHANNEL_ALPHA | PS_INPUTMAPPING_UNSIGNED_INVERT,
		PS_REGISTER_ZERO | PS_CHANNEL_ALPHA | PS_INPUTMAPPING_UNSIGNED_IDENTITY,
		PS_REGISTER_ZERO | PS_CHANNEL_ALPHA | PS_INPUTMAPPING_UNSIGNED_IDENTITY);
	psd.PSRGBOutputs[0]=PS_COMBINEROUTPUTS(
		PS_REGISTER_R0,
		PS_REGISTER_DISCARD,
		PS_REGISTER_DISCARD,
		PS_COMBINEROUTPUT_IDENTITY | PS_COMBINEROUTPUT_AB_MULTIPLY | PS_COMBINEROUTPUT_CD_MULTIPLY | PS_COMBINEROUTPUT_AB_CD_SUM);
	psd.PSAlphaOutputs[0]=PS_COMBINEROUTPUTS(
		PS_REGISTER_R0,
		PS_REGISTER_DISCARD,
		PS_REGISTER_DISCARD,
		PS_COMBINEROUTPUT_IDENTITY | PS_COMBINEROUTPUT_AB_MULTIPLY | PS_COMBINEROUTPUT_CD_MULTIPLY | PS_COMBINEROUTPUT_AB_CD_SUM);
	psd.PSConstant0[0]=0x00000000;
	psd.PSConstant1[0]=0x00000000;

	//------------- FinalCombiner -------------
	psd.PSFinalCombinerInputsABCD=PS_COMBINERINPUTS(
		PS_REGISTER_R0 | PS_CHANNEL_RGB | PS_INPUTMAPPING_UNSIGNED_IDENTITY,
		PS_REGISTER_ZERO | PS_CHANNEL_RGB | PS_INPUTMAPPING_UNSIGNED_INVERT,
		PS_REGISTER_ZERO | PS_CHANNEL_RGB | PS_INPUTMAPPING_UNSIGNED_IDENTITY,
		PS_REGISTER_ZERO | PS_CHANNEL_RGB | PS_INPUTMAPPING_UNSIGNED_IDENTITY);
	psd.PSFinalCombinerInputsEFG=PS_COMBINERINPUTS(
		PS_REGISTER_ZERO | PS_CHANNEL_RGB | PS_INPUTMAPPING_UNSIGNED_IDENTITY,
		PS_REGISTER_ZERO | PS_CHANNEL_RGB | PS_INPUTMAPPING_UNSIGNED_IDENTITY,
		PS_REGISTER_R0 | PS_CHANNEL_ALPHA | PS_INPUTMAPPING_UNSIGNED_IDENTITY,
		0);

	psd.PSFinalCombinerConstants = PS_FINALCOMBINERCONSTANTS(15,15,PS_GLOBALFLAGS_TEXMODE_ADJUST);

	hr = g_pd3dDevice->CreatePixelShader( &psd, &m_dwImagePixelShaderHandle );
	assert(hr == D3D_OK);

	// Create vector normalization cube map.
	hr = XBUtil_CreateNormalizationCubeMap( g_pd3dDevice, 64, &m_pNormalizationCubemap );
	assert(hr == D3D_OK);
   
#else

	DWORD dwDecl[] =
	{
		D3DVSD_STREAM( 0 ),
		D3DVSD_REG( D3DVSDE_POSITION,  D3DVSDT_FLOAT3 ),
		D3DVSD_REG( D3DVSDE_NORMAL,    D3DVSDT_FLOAT3 ),
		D3DVSD_REG( D3DVSDE_TEXCOORD0, D3DVSDT_FLOAT2 ),
		D3DVSD_REG( D3DVSDE_TEXCOORD1, D3DVSDT_FLOAT3 ),
		D3DVSD_REG( D3DVSDE_TEXCOORD2, D3DVSDT_FLOAT3 ),
		D3DVSD_REG( D3DVSDE_TEXCOORD3, D3DVSDT_FLOAT3 ),
		D3DVSD_END()
	};

	m_dwBumpVertexShaderHandle = 0;
	g_pd3dDevice->CreateVertexShader(dwDecl, 0, &m_dwImageVertexShaderHandle, 0);

	m_dwImagePixelShaderHandle = 0;
	m_dwBumpPixelShaderHandle = 0;

#endif
}
#endif // 0