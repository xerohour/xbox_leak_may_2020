//-----------------------------------------------------------------------------
// File: Texture.cpp
//
// Desc: Classes designed to support the creation, loading, and
//		 clearing of texture surfaces.
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#include "std.h"
#include "texture.h"
#include "file.h"

extern LPDIRECT3DDEVICE8 g_pd3dDevice;

//-----------------------------------------------------------------------------
// Name: CBaseTexture()
// Desc: Sets initial values of member variables
//-----------------------------------------------------------------------------
CBaseTexture::CBaseTexture()
{
    m_sTexDirectory	   = NULL;
	m_nTexSizeBytes	   = 0;
	m_TexFormat		   = D3DFMT_UNKNOWN;
    m_pTexture		   = NULL;
}

//-----------------------------------------------------------------------------
// Name: ~CBaseTexture()
// Desc: Performs any needed cleanup before shutting down the object
//-----------------------------------------------------------------------------
CBaseTexture::~CBaseTexture()
{
	if ( m_sTexDirectory != NULL )
	{
		delete [] m_sTexDirectory;
		m_sTexDirectory = NULL;
	}

	// Because we allocate memory in another structure and point to it with m_pTexture
	// I simply set the variable to NULL without a delete
	m_pTexture = NULL;
}

//-----------------------------------------------------------------------------
// Name: CTexture()
// Desc: Sets initial values of member variables
//-----------------------------------------------------------------------------
CTexture::CTexture() : 
	CBaseTexture()
{
    m_nWidth  = 0;
	m_nHeight = 0;
	m_pResourceSysMemData = NULL;
	m_pResourceVidMemData = NULL;
}

//-----------------------------------------------------------------------------
// Name: ~CTexture()
// Desc: Performs any needed cleanup before shutting down the object
//-----------------------------------------------------------------------------
CTexture::~CTexture()
{
	// Free up texture memory
	if( m_pResourceSysMemData != NULL )
	{
		delete [] m_pResourceSysMemData;
		m_pResourceSysMemData = NULL;
	}

	if( m_pResourceVidMemData != NULL )
	{
		D3D_FreeContiguousMemory(m_pResourceVidMemData);
		m_pResourceVidMemData = NULL;
	}
}

//-----------------------------------------------------------------------------
// Name: CreateTexture()
// Desc: Creates an empty texture canvas of the specified dimensions
//-----------------------------------------------------------------------------
HRESULT CTexture::CreateTexture(int &nWidth, int &nHeight, D3DFORMAT format)
{
	if (FAILED(g_pd3dDevice->CreateTexture(nWidth, nHeight, 1, 0, format, D3DPOOL_MANAGED, (LPDIRECT3DTEXTURE8*)m_pTexture)))
	{
		return E_FAIL;
	}

	// Populate the texture member data (width, height, size, format)
	if(FAILED(GetTextureInfo()))
		return E_FAIL;

	return S_OK;
}

//-----------------------------------------------------------------------------
// Name: GetTextureSize()
// Desc: Updates the values of the texture dimensions
//-----------------------------------------------------------------------------
HRESULT CTexture::GetTextureInfo( void )
{
	D3DSURFACE_DESC sd;
	if((((LPDIRECT3DTEXTURE8)m_pTexture)->GetLevelDesc(0, &sd)) != D3D_OK )
	{
		return E_FAIL;
	}

	m_nWidth		= (int)sd.Width;
	m_nHeight		= (int)sd.Height;
	m_nTexSizeBytes	= sd.Size;
	m_TexFormat		= sd.Format;


	return S_OK;
}

//-----------------------------------------------------------------------------
// Name: LoadTexture()
// Desc: Loads a bundled file into a texture surface, and store the
//		 width, height, format, and size of the texture into member
//		 variables.
//-----------------------------------------------------------------------------
HRESULT CTexture::LoadTexture( const char* sFilepath )
{
	// Variables necessary for extracting bundled images from our .XBX files
	DWORD resource_NUM_RESOURCES	 = 1UL;
	DWORD resource_background_OFFSET = 0UL;

	// Copy filepath to the member variable
	m_sTexDirectory = new char [strlen(sFilepath) + 1];
	strcpy(m_sTexDirectory, sFilepath);

	if (m_sTexDirectory[0] == '\0')
	{
		delete [] m_sTexDirectory;
		m_sTexDirectory = NULL;
		return E_FAIL;
	}

    // Open the bundled file to read the XPR headers
	File inputFile;
	if(inputFile.open( m_sTexDirectory, READ ) != NO_ERR)
	{
		DbgPrint( "Unable to open texture file %s\n", sFilepath ); 
		ASSERT(false);
        return E_FAIL;
	}

    // Read in and verify the XPR magic header
    XPR_HEADER xprh;
    inputFile.read( (BYTE*)&xprh, sizeof(XPR_HEADER) );
    if( xprh.dwMagic != XPR_MAGIC_VALUE )
    {
        OutputDebugStringA( "ERROR: Invalid Xbox Packed Resource (.xpr) file" );
        inputFile.close();
        return E_INVALIDARG;
    }

    // Compute memory requirements
    DWORD dwSysMemDataSize = xprh.dwHeaderSize - sizeof(XPR_HEADER);
    DWORD dwVidMemDataSize = xprh.dwTotalSize - xprh.dwHeaderSize;

    // Allocate memory
    m_pResourceSysMemData = new BYTE[dwSysMemDataSize];
    m_pResourceVidMemData = (BYTE*)D3D_AllocContiguousMemory( dwVidMemDataSize, D3DTEXTURE_ALIGNMENT );

    // Read in the data from the file
    inputFile.read( m_pResourceSysMemData, dwSysMemDataSize );
    inputFile.read( m_pResourceVidMemData, dwVidMemDataSize );

    // Done with the file
    inputFile.close();
    
    // Loop over resources, calling Register()
    BYTE* pData = m_pResourceSysMemData;

    for( DWORD i = 0; i < resource_NUM_RESOURCES; i++ )
    {
        // Get the resource
        LPDIRECT3DRESOURCE8 pResource = (LPDIRECT3DRESOURCE8)pData;

        // Register the resource
        pResource->Register( m_pResourceVidMemData );
    
        // Advance the pointer
        switch( pResource->GetType() )
        {
            case D3DRTYPE_TEXTURE:       pData += sizeof(D3DTexture);       break;
            case D3DRTYPE_VOLUMETEXTURE: pData += sizeof(D3DVolumeTexture); break;
            case D3DRTYPE_CUBETEXTURE:   pData += sizeof(D3DCubeTexture);   break;
            case D3DRTYPE_VERTEXBUFFER:  pData += sizeof(D3DVertexBuffer);  break;
            case D3DRTYPE_INDEXBUFFER:   pData += sizeof(D3DIndexBuffer);   break;
            case D3DRTYPE_PALETTE:       pData += sizeof(D3DPalette);       break;
            default:                     return E_FAIL;
        }
    }

	m_pTexture = (LPDIRECT3DTEXTURE8)&m_pResourceSysMemData[ resource_background_OFFSET ];

	// Populate the texture member data (width, height, size, format)
	if(FAILED(GetTextureInfo()))
		return E_FAIL;

    return S_OK;
}

//-----------------------------------------------------------------------------
// Name: SizeOnDisk()
// Desc: Find the approximate size of the texture by getting the file size
//		 from the disk
//-----------------------------------------------------------------------------
DWORD CTexture::SizeOnDisk (const char* sFilepath )
{
	if(sFilepath == NULL)
		return -1;
	
	File textureFile;
	if(FAILED(textureFile.open(sFilepath, READ)))
	{
		// File cannot be opened
		return -1;
	}

	return textureFile.fileSize();
}
