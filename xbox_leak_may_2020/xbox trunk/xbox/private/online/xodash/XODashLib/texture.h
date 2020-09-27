//-----------------------------------------------------------------------------
// File: Texture.h
//
// Desc: Header file for the main texture loading/clearing class
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#ifndef TEXTURE_H
#define TEXTURE_H

//-----------------------------------------------------------------------------
// Name: class CBaseTexture
// Desc: Base texture class
//-----------------------------------------------------------------------------
class CBaseTexture
{
public:
	CBaseTexture();
	virtual ~CBaseTexture();

	char					*m_sTexDirectory;
	unsigned int			m_nTexSizeBytes;
	D3DFORMAT				m_TexFormat;
	IDirect3DBaseTexture8	*m_pTexture;
};

class CTexture : public CBaseTexture
{
private:
	BYTE	*m_pResourceSysMemData; // Sysmem data for the packed resource
	BYTE	*m_pResourceVidMemData; // Vidmem data for the packed resource

public:
	CTexture();
	virtual ~CTexture();

	unsigned int m_nWidth;
	unsigned int m_nHeight;


	HRESULT CreateTexture( int &nWidth, int &nHeight, D3DFORMAT format );
	HRESULT GetTextureInfo( void );
	HRESULT LoadTexture( const char* sFilepath );
	DWORD	SizeOnDisk (const char* sFilepath );
};

class CVolumeTexture : public CBaseTexture
{
public:
	unsigned int m_nWidth;
	unsigned int m_nHeight;
	unsigned int m_nDepth;
};

class CCubeTexture : public CBaseTexture
{
public:
	unsigned int m_nLength;
};

#endif