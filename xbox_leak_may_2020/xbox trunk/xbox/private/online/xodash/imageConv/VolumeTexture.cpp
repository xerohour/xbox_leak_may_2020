//-----------------------------------------------------------------------------
// File: VolumeTexture.cpp
//
// Desc: Contains the VolumeTexture-specific logic for the bundler tool
//
// Hist: 04.11.01 - New for May XDK release
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#include "VolumeTexture.h"
#include "Bundler.h"




//-----------------------------------------------------------------------------
// Name: CVolumeTexture()
// Desc: Initializes member variables
//-----------------------------------------------------------------------------
CVolumeTexture::CVolumeTexture( CBundler* pBundler )
               :CBaseTexture( pBundler )
{
    m_pVolumeTexture  = NULL;
    m_apSurface       = NULL;
    m_astrSource      = NULL;
    m_astrAlphaSource = NULL;
    m_dwWidth         = 0L;
    m_dwHeight        = 0L;
    m_dwDepth         = 0L;
}




//-----------------------------------------------------------------------------
// Name: ~CVolumeTexture()
// Desc: Performs any cleanup needed before the object is destroyed
//-----------------------------------------------------------------------------
CVolumeTexture::~CVolumeTexture()
{
    if( m_pVolumeTexture )
        m_pVolumeTexture->Release();

    if( m_apSurface )
    {
        for( DWORD i=0; i<m_dwDepth; i++ )
        {
            if( m_apSurface[i] )
                m_apSurface[i]->Release();
        }
        delete[] m_apSurface;
    }

    if( m_astrSource )
        delete[] m_astrSource;
    if( m_astrAlphaSource )
        delete[] m_astrAlphaSource;
}




//-----------------------------------------------------------------------------
// Name: SaveToBundle()
// Desc: Handles saving the appropriate data to the packed resource file
//-----------------------------------------------------------------------------
HRESULT CVolumeTexture::SaveToBundle( DWORD* pcbHeader, DWORD* pcbData )
{
    HRESULT hr;

    hr = LoadVolumeTexture();
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

    // Save voleume texture data
    (*pcbData) = 0;

    SaveVolumeTexture( pcbData, m_dwLevels, m_pVolumeTexture );

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: LoadVolumeTexture()
// Desc: Loads the texture from the file, and sets any properties that were
//       not specified with values from the file (width, height, format, etc)
//-----------------------------------------------------------------------------
HRESULT CVolumeTexture::LoadVolumeTexture()
{
    D3DSURFACE_DESC desc;
    HRESULT hr;

    // Try to look up our format string
    m_nFormat = FormatFromString( m_strFormat );
    if( m_nFormat < -1 )
    {
        m_pBundler->ErrorMsg( "Error: Invalid texture format: %s", m_strFormat );
        return E_FAIL;
    }
    lstrcpyA( m_strFormat, g_TextureFormats[m_nFormat].strFormat );

    // Make sure depth is a power of two for non-linear textures
    if( g_TextureFormats[m_nFormat].Type == FMT_LINEAR )
    {
        m_pBundler->ErrorMsg( "Error: Volume textures must be swizzled or compressed.\n" );
        return E_FAIL;
    }

    // Make sure depth is a power of two
    DWORD dwDepth;
    for( dwDepth=1; dwDepth < m_dwDepth; dwDepth<<=1 );
    if( dwDepth != m_dwDepth )
    {
        m_pBundler->ErrorMsg( "Error: Volume texture dimensions must be a power-of-two" );
        return E_FAIL;
    }

    // If an alpha source was specified, make sure the format supports alpha
    for( DWORD i=0; i<m_dwDepth; i++ )
    {
        if( m_astrAlphaSource[i][0] ) 
        {
            if( g_TextureFormats[m_nFormat].dwNumAlphaBits == 0 )
            {
                m_pBundler->ErrorMsg( "Warning: an ALPHASOURCE was specified, yet the " \
                                      "requested texture format, %s,  does not have " \
                                      "any alpha.\n", m_strFormat );
            }
        }
    }

    // Load the image surfaces from the file (using default width, height, and a
    // A8R8G8B8 surface format)
    m_apSurface = new LPDIRECT3DSURFACE8[m_dwDepth];
    for( i=0; i<m_dwDepth; i++ )
    {
        if( FAILED( hr = LoadSurface( m_astrSource[i], m_astrAlphaSource[i], &m_apSurface[i] ) ) )
        {
			if( m_astrAlphaSource[i] )
				m_pBundler->ErrorMsg( "Volume texture: Couldn't load source file <%s> or <%s>", m_astrSource[i], m_astrAlphaSource[i] );
			else
				m_pBundler->ErrorMsg( "Volume texture: Couldn't load source file <%s>", m_astrSource[i] );
            return hr;
        }
    }

    // If width or height wasn't specified, use dimensions from first source texture
    if( m_dwWidth==0 || m_dwHeight==0 )
    {
        hr = m_apSurface[0]->GetDesc( &desc );
        m_dwWidth  = desc.Width;
        m_dwHeight = desc.Height;

        // Enforce power-of-two dimensions
        for( m_dwWidth=1;   m_dwWidth  < desc.Width;   m_dwWidth<<=1 );
        for( m_dwHeight=1;  m_dwHeight < desc.Height;  m_dwHeight<<=1 );
    }

    // Keep dimensions in check
    if( m_dwWidth>512 || m_dwHeight>512 || m_dwDepth>512 )
    {
        m_pBundler->ErrorMsg( "Error: The max for volume texture dimensions is 512.\n" );
        return hr;
    }

    // Determine final number of miplevels
    DWORD dwLevels = 1; 
    while( (1UL<<(dwLevels-1)) < min( m_dwWidth, min( m_dwHeight, m_dwDepth ) ) )
        dwLevels++;
    
    if( m_dwLevels < 1 || m_dwLevels > dwLevels )
        m_dwLevels = dwLevels;

    // Change the size of the surfaces
    for( i=0; i<m_dwDepth; i++ )
    {
        hr = ResizeSurface( m_dwWidth, m_dwHeight, &m_apSurface[i] );
    }

    // Create the volume texture
    m_pBundler->m_pd3ddev->CreateVolumeTexture( m_dwWidth, m_dwHeight, m_dwDepth, 1, 
                                                0, D3DFMT_A8R8G8B8, D3DPOOL_SYSTEMMEM, 
                                                &m_pVolumeTexture );

    D3DLOCKED_BOX destlock;
    m_pVolumeTexture->LockBox( 0, &destlock, 0, 0 );
    for( i=0; i<m_dwDepth; i++ )
    {
        D3DLOCKED_RECT srclock;
        m_apSurface[i]->LockRect( &srclock, 0, 0 );
        memcpy( destlock.pBits, srclock.pBits, sizeof(DWORD)*m_dwWidth*m_dwHeight );
        m_apSurface[i]->UnlockRect();
        destlock.pBits = ((BYTE*)destlock.pBits) + sizeof(DWORD)*m_dwWidth*m_dwHeight;
    }
    m_pVolumeTexture->UnlockBox( 0 );

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: SaveHeaderInfo()
// Desc: Saves the appropriate data to the header file
//-----------------------------------------------------------------------------
HRESULT CVolumeTexture::SaveHeaderInfo( DWORD dwStart, DWORD* pcbHeader )
{
    D3DTexture d3dtex;

    XGSetVolumeTextureHeader( m_dwWidth, m_dwHeight, m_dwDepth, m_dwLevels, 0,
                              (D3DFORMAT)g_TextureFormats[m_nFormat].dwXboxFormat,
                              D3DPOOL_DEFAULT, (IDirect3DVolumeTexture8*)&d3dtex,
                              dwStart, 0 );

    // Overwrite the error in XGSetVolumeTextureHeader
    {
        for( DWORD LogWidth=0;  (1UL<<LogWidth)  <= m_dwWidth;  LogWidth++ )
        for( DWORD LogHeight=0; (1UL<<LogHeight) <= m_dwHeight; LogHeight++ )
        for( DWORD LogDepth=0;  (1UL<<LogDepth)  <= m_dwDepth;  LogDepth++ )
        d3dtex.Format =  3 << D3DFORMAT_DIMENSION_SHIFT /* UNDONE: verify */
                         | g_TextureFormats[m_nFormat].dwXboxFormat << D3DFORMAT_FORMAT_SHIFT
                         | m_dwLevels << D3DFORMAT_MIPMAP_SHIFT
                         | LogWidth   << D3DFORMAT_USIZE_SHIFT
                         | LogHeight  << D3DFORMAT_VSIZE_SHIFT
                         | LogDepth   << D3DFORMAT_PSIZE_SHIFT
                         | D3DFORMAT_DMACHANNEL_B
                         | D3DFORMAT_BORDERSOURCE_COLOR;
    }

    // Write the resource header out
    if( FAILED( m_pBundler->WriteHeader( &d3dtex, sizeof( d3dtex ) ) ) )
        return E_FAIL;

    (*pcbHeader) = sizeof(d3dtex);

    return S_OK;
}




