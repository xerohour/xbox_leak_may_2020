//-----------------------------------------------------------------------------
// File: Texture.cpp
//
// Desc: Contains the texture-specific logic for the bundler tool
//
// Hist: 2001.02.06 - New for March XDK release
//       2001.04.13 - Reorganized for May XDK release
//       2002.01.31 - Updated to avoid using D3D
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#include <stdio.h>
#include "Texture.h"
#include "Bundler.h"




//-----------------------------------------------------------------------------
// Name: CTexture2D()
// Desc: Initializes member variables
//-----------------------------------------------------------------------------
CTexture2D::CTexture2D( CBundler* pBundler )
           :CBaseTexture( pBundler )
{
    m_strSource[0]      = 0;
    m_strAlphaSource[0] = 0;
    m_pImage            = NULL;
    m_dwWidth           = 0;
    m_dwHeight          = 0;
}




//-----------------------------------------------------------------------------
// Name: ~CTexture2D()
// Desc: Performs any cleanup needed before the object is destroyed
//-----------------------------------------------------------------------------
CTexture2D::~CTexture2D()
{
	if (m_pImage)
		delete m_pImage;
}




//-----------------------------------------------------------------------------
// Name: SaveToBundle()
// Desc: Handles saving the appropriate data to the packed resource file
//-----------------------------------------------------------------------------
HRESULT CTexture2D::SaveToBundle( DWORD* pcbHeader, DWORD* pcbData )
{
    HRESULT hr;

    // Load the texture from disk, and set default values from it
    hr = LoadTexture();
    if( FAILED( hr ) )
        return hr;

    // Pad data file to proper alignment for the start of the texture
    hr = m_pBundler->PadToAlignment( D3DTEXTURE_ALIGNMENT );
    if( FAILED( hr ) )
        return hr;

    // Save resource header
    hr = SaveHeaderInfo( m_pBundler->m_cbData, pcbHeader );
    if( FAILED( hr ) )
        return hr;

    // Save texture data
    (*pcbData) = 0;
    SaveImage( pcbData, m_dwLevels, m_pImage );
     
    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: LoadTexture()
// Desc: Loads the texture from the file, and sets any properties that were
//       not specified with values from the file (width, height, format, etc)
//-----------------------------------------------------------------------------
HRESULT CTexture2D::LoadTexture()
{
    HRESULT hr;

    // Load the imagesurfaces from the file (using default width, height, and a
    // A8R8G8B8 surface format)
    hr = LoadImage( m_strSource, m_strAlphaSource, &m_pImage );
    if( FAILED( hr ) )
    {
        // If the conventional load methods failed, we can try using D3DX as a
        // last ditch effort. This will load DDS files, JPGs, etc., but has
        // limitations like only pow-2 dimensions.

        hr = LoadImageUsingD3DX( m_strSource, m_strAlphaSource, &m_pImage );
        if( FAILED( hr ) )
        {
            if( m_strAlphaSource[0] )
                m_pBundler->ErrorMsg( "Texture: Couldn't load source file <%s> or <%s>\n", m_strSource, m_strAlphaSource );
            else
                m_pBundler->ErrorMsg( "Texture: Couldn't load source file <%s>\n", m_strSource );
            return hr;
        }
    }

    // Try to look up our format string
    if( m_strFormat[0] )
    {
        m_nFormat = FormatFromString( m_strFormat );
        if( m_nFormat < -1 )
        {
            m_pBundler->ErrorMsg( "Error: Invalid texture format: %s", m_strFormat );
            return E_FAIL;
        }
    }

    // If the format is still not determined, resort to A8R8G8B8
    if( m_nFormat < 0 )
    {
        lstrcpyA( m_strFormat, "D3DFMT_A8R8G8B8" );
        m_nFormat = FormatFromString( m_strFormat );
    }

    // Determine final width and height
    if( m_dwWidth==0 || m_dwHeight==0 )
    {
        if( g_TextureFormats[m_nFormat].Type == FMT_LINEAR )
        {
            // Linear textures can have any size
            m_dwWidth  = m_pImage->m_Width;
            m_dwHeight = m_pImage->m_Height;
        }
        else
        {
            // Enforce power-of-two dimensions for swizzled and compressed textures
            for( m_dwWidth=1;  m_dwWidth  < m_pImage->m_Width;  m_dwWidth<<=1 );
            for( m_dwHeight=1; m_dwHeight < m_pImage->m_Height; m_dwHeight<<=1 );
        }
    }

    // Determine final number of miplevels
    if( g_TextureFormats[m_nFormat].Type == FMT_LINEAR )
    {
        m_dwLevels = 1;
    }
    else
    {
        // Count levels
        DWORD dwLevels = 1; 
        while( ( (1UL<<(dwLevels-1)) < m_dwWidth ) && ( (1UL<<(dwLevels-1))<m_dwHeight ) )
            dwLevels++;
            
        if( m_dwLevels < 1 || m_dwLevels > dwLevels )
            m_dwLevels = dwLevels;
    }

    // Change the size of the surface
    hr = ResizeImage( m_dwWidth, m_dwHeight, &m_pImage );

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: SaveHeaderInfo()
// Desc: Saves the appropriate data to the header file
//-----------------------------------------------------------------------------
HRESULT CTexture2D::SaveHeaderInfo( DWORD dwStart, DWORD * pcbHeader )
{
    XD3DTexture d3dtex;

    XGSetTextureHeader(m_dwWidth, m_dwHeight, m_dwLevels, 0,
                       (D3DFORMAT)g_TextureFormats[m_nFormat].dwXboxFormat,
                       D3DPOOL_DEFAULT, (IDirect3DTexture8*)&d3dtex,
                       dwStart, 0);

    // Write the resource header out
    if( FAILED( m_pBundler->WriteHeader( &d3dtex, sizeof( d3dtex ) ) ) )
        return E_FAIL;

    *pcbHeader = sizeof( d3dtex );

    return S_OK;
}



//-----------------------------------------------------------------------------
// Name: SaveHeaderInfo()
// Desc: Fill a texture texel-by-texel.
//-----------------------------------------------------------------------------
HRESULT CTexture2DFill::SetNextTexel(DWORD dwColor)
{
	HRESULT hr = S_OK;
	if (m_pImage == NULL)
	{
		// Allocate surface
		if (m_dwWidth == 0 || m_dwHeight == 0)
		{
			m_pBundler->ErrorMsg( "Width and height attributes required for inline textures.\n" );
			return E_FAIL;
		}
		m_pImage = new CImage( m_dwWidth, m_dwHeight, D3DFMT_A8R8G8B8 );
		if (m_pImage == NULL)
		{
			m_pBundler->ErrorMsg( "Unable to create image surface for inline texture.\n" );
			return E_OUTOFMEMORY;
		}
		m_iTexel = 0;
	}
	if (m_iTexel >= m_dwWidth * m_dwHeight)
	{
		m_pBundler->ErrorMsg( "Too many texels (%d) added to inline texture (width=%d height=%d).\n", m_iTexel, m_dwWidth, m_dwHeight );
		return E_FAIL;
	}
	assert(m_pImage->m_Format == D3DFMT_A8R8G8B8);	// Image must be ARGB format
	DWORD *pPixel = (DWORD *)(m_pImage->m_pData) + m_iTexel;
	*pPixel = dwColor;
	m_iTexel++;
	return S_OK;
}
