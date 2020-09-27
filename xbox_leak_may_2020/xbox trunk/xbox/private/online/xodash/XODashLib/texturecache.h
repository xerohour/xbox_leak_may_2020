//-----------------------------------------------------------------------------
// File: TextureCache.h
//
// Desc: Header file for the main texture manager
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#ifndef TEXTURECACHE_H
#define TEXTURECACHE_H

#define TOTAL_TEXTURE_CACHE_SIZE 10000000

#include <list>
#include "texture.h"

//-----------------------------------------------------------------------------
// Name: class CTextureItem
// Desc: Base data type for each texture object in the texture cache
//-----------------------------------------------------------------------------
class CTextureItem
{
public:
	CTextureItem();
	~CTextureItem();

	CHAR		     *m_sTextureName;		// Holds the filename of the image
	long			 m_nID;					// The ID index of the texture
	BOOL		     m_bProtected;			// Specifies if the image should always stay in the cache
	DWORD		     m_nRefCount;			// Stores the number of times the texture has been referenced
	unsigned __int64 m_nLastTouched;		// Stores the tick count when the texture was last referenced
	CTexture	     *m_pTexture;			// The actual texture data
};

using namespace std;
typedef list<CTextureItem*>  TextureList;

//-----------------------------------------------------------------------------
// Name: class CTextureCache
// Desc: Base texture manager class
//-----------------------------------------------------------------------------
class CTextureCache
{
public:
	CTextureCache();
	~CTextureCache();

	LPDIRECT3DTEXTURE8		 FindTexture( long *texIndex, CHAR *sTexName );
	LPDIRECT3DVOLUMETEXTURE8 FindVolumeTexture( CHAR *sTexName );
	LPDIRECT3DCUBETEXTURE8	 FindCubeTexture( CHAR *sTexName );
	unsigned int			 TotalTextures( void );
	DWORD					 TotalCacheSize( void );
	unsigned __int64		 GetTextureTime( void );

private:
	TextureList	m_TextureCache;   // The linked list that will store the textures
	DWORD		m_nTotalBytes;    // The total number of bytes the Texture cache is currently using
	long		m_nTotalTextures; // The total number of textures in the texture cache (-1 if empty)

	LPDIRECT3DTEXTURE8 		 LoadTexture( long *texIndex, CHAR *sTexName );
	
};

#endif

