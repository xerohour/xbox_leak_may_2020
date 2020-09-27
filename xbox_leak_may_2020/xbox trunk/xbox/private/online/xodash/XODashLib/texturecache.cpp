//-----------------------------------------------------------------------------
// File: TextureCache.cpp
//
// Desc: The texture cache is designed to store a linked list of textures that
//		 are available
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#include "std.h"
#include "texturecache.h"
#include "xoconst.h"
#include "file.h"
#include "utilities.h"

extern char* g_pScenePath;

//-----------------------------------------------------------------------------
// Name: CTextureItem()
// Desc: Sets initial values of member variables
//-----------------------------------------------------------------------------
CTextureItem::CTextureItem()
{
	m_sTextureName  = NULL;		
	m_nID			= -1;					
	m_bProtected	= false;			
	m_nLastTouched	= 0;		
	m_nRefCount		= 0;			
	m_pTexture		= NULL;		
}

//-----------------------------------------------------------------------------
// Name: ~CTextureItem()
// Desc: Performs any needed cleanup before shutting down the object
//-----------------------------------------------------------------------------
CTextureItem::~CTextureItem()
{
	if( m_sTextureName != NULL )
	{
		delete [] m_sTextureName;
		m_sTextureName = NULL;
	}

	if( m_pTexture != NULL )
	{
		delete m_pTexture;
		m_pTexture = NULL;
	} 
}

//-----------------------------------------------------------------------------
// Name: CTextureCache()
// Desc: Sets initial values of member variables
//-----------------------------------------------------------------------------
CTextureCache::CTextureCache()
{
	m_nTotalBytes	 = 0;
	m_nTotalTextures = -1;
}

//-----------------------------------------------------------------------------
// Name: ~CTextureCache()
// Desc: Performs any needed cleanup before shutting down the object
//-----------------------------------------------------------------------------
CTextureCache::~CTextureCache()
{
	// Clean up the texture items in the texture cache
	TextureList::iterator i;
	for(i = m_TextureCache.begin(); i != m_TextureCache.end(); i++)
	{
		if ( *i != NULL )
			delete (*i);
	}

	m_TextureCache.clear();
}

//-----------------------------------------------------------------------------
// Name: FindTexture()
// Desc: Find texture is the main function for getting a pointer to a texture
//		 on the Xbox.  It will first traverse the existing list of textures in
//		 the texture cache until it finds the texture, then it will return the
//		 pointer to that texture.  If texture is not found in the cache, the
//		 function will clear out the necessary space for the texture, and load
//		 it into the cache.  It will then return the pointer to this newly
//		 loaded texture.
//-----------------------------------------------------------------------------
LPDIRECT3DTEXTURE8 CTextureCache::FindTexture( long *texIndex, CHAR *sTexName )
{
	// Check to make sure the texture string is passed in
	if(sTexName == NULL)
		return NULL;

	// If the texture hasn't already been indexed, load it for the first time and assign an index
	if(*texIndex < 0)
	{
		// Traverse the texture cache to see if the texture is already loaded
		TextureList::iterator i;
		for(i = m_TextureCache.begin(); i != m_TextureCache.end(); i++)
		{
			if(strcmp((*i)->m_sTextureName, sTexName) == 0)
			{
				if((*i)->m_pTexture != NULL)
				{
					// Found a texture match in the cache, set the index and return the texture
					*texIndex = (*i)->m_nID;
					(*i)->m_nRefCount++;
					(*i)->m_nLastTouched = GetTextureTime();
					return (LPDIRECT3DTEXTURE8)(*i)->m_pTexture->m_pTexture;
				}
			}
		}

		// The requested texture isn't currently in the cache, so load it and return the 
		// proper texture index
		return LoadTexture( &*texIndex, sTexName );
	}
	else   
	{
		// Traverse the texture cache to see if the texture is already loaded
		TextureList::iterator i;
		i = m_TextureCache.begin();
		for(long j = 0; j < *texIndex; j++)
		{
			i++;
		}

		if((*i)->m_pTexture != NULL)
		{
			// Found a texture match in the cache, set the index and return the texture
			(*i)->m_nRefCount++;
			(*i)->m_nLastTouched = GetTextureTime();
			return (LPDIRECT3DTEXTURE8)(*i)->m_pTexture->m_pTexture;
		}

		// The texture has been indexed before, so traverse to that index point and make
		// sure that the file is still in memory
		return LoadTexture( &*texIndex, sTexName );
	}

	return NULL;
}

//------------------------------------------------------------------------------
// Name: LoadTexture()
// Desc: A local function designed to do all of the necessary loading of a texture
//		 file into the cache
//------------------------------------------------------------------------------

LPDIRECT3DTEXTURE8 CTextureCache::LoadTexture( long *texIndex, CHAR *sTexName )
{
	// Get an approximate file size of the texture on disk
	char *xbxPath		 = new char[wcslen(XBX_PATH) + 1];
	Ansi(xbxPath, XBX_PATH, wcslen(XBX_PATH) + 1);

	char *pTexturePath = new char[strlen(xbxPath) + strlen(sTexName) + 1];
	sprintf(pTexturePath, "%s%s", xbxPath, sTexName);
	CTexture *pTexture = new CTexture();
	DWORD nTextureSize = pTexture->SizeOnDisk(pTexturePath);

	if(nTextureSize == -1)		
	{
		// Texture was not found in the specified location
		delete [] pTexturePath;
		delete pTexture;
		delete [] xbxPath;	 
		return NULL;
	}

	// Check if the texture cache needs to clear an entry to import a new texture
	m_nTotalBytes = TotalCacheSize();
	
	// Traverse through the texture cache, and LRU textures until we have enough space
	while((m_nTotalBytes + nTextureSize) > TOTAL_TEXTURE_CACHE_SIZE)
	{
		unsigned __int64 nOldestTickCount = 0xffffffffffffffff;
		TextureList::iterator iOldestTextureItem;

		// The current texture cache, plus the addition of the new texture
		// is currently too large.  Find the LRU texture, and remove it from the cache
		TextureList::iterator i;
		for(i = m_TextureCache.begin(); i != m_TextureCache.end(); i++)
		{
			// Is the current texture older than the previous?
			if((*i)->m_pTexture != NULL)
			{
				if((*i)->m_nLastTouched < nOldestTickCount)
				{
					nOldestTickCount = (*i)->m_nLastTouched;
					iOldestTextureItem = i;
				}
			}
		}

		// Remove the oldest referenced texture from the cache, but leave it's info in the linked list
		m_nTotalBytes -= (*iOldestTextureItem)->m_pTexture->m_nTexSizeBytes;
		delete (*iOldestTextureItem)->m_pTexture;
		(*iOldestTextureItem)->m_pTexture = NULL;
	}

	// Texture cache has enough room to store the texture.  Push the texture on
	// to the texture cache

	if(FAILED(pTexture->LoadTexture(pTexturePath)))
	{
		delete [] pTexturePath;
		delete pTexture;
		delete [] xbxPath;	
		return NULL;
	}
	delete [] pTexturePath;
	delete [] xbxPath;	

	// If the texture hasn't already been indexed, load it for the first time and assign an index
	if(*texIndex < 0)
	{
		m_nTotalTextures += 1;

		CTextureItem *pNewTexItem = new CTextureItem();

		pNewTexItem->m_sTextureName = new char[strlen(sTexName) + 1];
		strcpy(pNewTexItem->m_sTextureName, sTexName);
		pNewTexItem->m_nLastTouched = GetTextureTime();
		pNewTexItem->m_nRefCount	= 1;
		pNewTexItem->m_nID			= m_nTotalTextures;
		*texIndex					= m_nTotalTextures;

		pNewTexItem->m_pTexture		= pTexture;

		m_TextureCache.push_back(pNewTexItem);
	}
	else
	{
		// Texture has already been loaded, so just update the pointer and member data
		TextureList::iterator i;
		i = m_TextureCache.begin();
		for(long j = 0; j < *texIndex; j++)
			i++;

		(*i)->m_pTexture	 = pTexture;
		(*i)->m_nLastTouched = GetTextureTime();
		(*i)->m_nRefCount	 = 1;
	}

	// Return the pointer to the recently loaded texture
	return (LPDIRECT3DTEXTURE8)pTexture->m_pTexture;
}

//-----------------------------------------------------------------------------
// Name: GetCurrentTime()
// Desc: Returns the current time from the system and parses it to a friendly
//		 format for us
//-----------------------------------------------------------------------------
unsigned __int64 CTextureCache::GetTextureTime( void )
{
	LPFILETIME nCurrentTime = new FILETIME();
	GetSystemTimeAsFileTime(nCurrentTime);
	unsigned __int64 nTime = nCurrentTime->dwHighDateTime << 32 | nCurrentTime->dwLowDateTime;
	delete nCurrentTime;
	return nTime;
}

//-----------------------------------------------------------------------------
// Name: TotalCacheSize()
// Desc: Returns the current size of the texture cache in bytes
//-----------------------------------------------------------------------------
DWORD CTextureCache::TotalCacheSize( void )
{
	DWORD nTotalBytes = 0;
	TextureList::iterator i;
	for(i = m_TextureCache.begin(); i != m_TextureCache.end(); i++)
	{
		// summing the total sizes of all of the textures
		if((*i)->m_pTexture != NULL)
			nTotalBytes += (*i)->m_pTexture->m_nTexSizeBytes;
	}

	return nTotalBytes;
}
