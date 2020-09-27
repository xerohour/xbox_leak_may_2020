//-----------------------------------------------------------------------------
//  
//  File: XBR.h
//  Copyright (C) 2001 Microsoft Corporation
//  All rights reserved.
//  
//  XML DirectX compiled resource drawing and previewing.
//
//-----------------------------------------------------------------------------
#pragma once
#include "Resource.h"
#include "xbrc.h"
#include "anim.h"
#include "effect.h"
#include "frame.h"
// #include "texture.h"

class XBR : public Resource
{
 public:
	
	// Called by OnIOComplete to patch pointers to all the effect types.
	virtual HRESULT Patch(DWORD dwType,		// resource type
						  BYTE *pHeader);	// pointer to resource header

    // Cleanup the resource before unloading.
	virtual HRESULT Cleanup(DWORD dwType,	// resource type
							BYTE *pHeader);	// pointer to resource header

	// Set a resource to be active
	HRESULT SetVertexShaderResource(DWORD VertexShaderResourceIndex);
	HRESULT SetPixelShaderResource(DWORD PixelShaderResourceIndex);
	HRESULT SetIndexBufferResource(DWORD IndexBufferResourceIndex);
	HRESULT SetTextureResource(DWORD iTextureStage, DWORD TextureResourceIndex);

	// Set the time for all of the animations in the resource list
	HRESULT SetTime(float fGlobalTime);
	
	// Draw the effect
	HRESULT DrawEffect(Effect *pEffect);

	// Debugging output
	HRESULT PrintVertexShaderDeclaration(DWORD VertexShaderResourceIndex);
	HRESULT PrintVertexShaderInputs(DWORD VertexShaderResourceIndex);

	// Constructor
	XBR()	{	m_dwSymbolCount = 0;	}

protected:
	// Symbol table was set if non-zero.  m_pResourceTags holds the (name, offset) tags
	DWORD m_dwSymbolCount;
public:
	DWORD SymbolCount()	{ return m_dwSymbolCount; }
	CONST CHAR *GetSymbolName(DWORD SymbolIndex);
	DWORD GetResourceIndexBySymbolIndex(DWORD SymbolIndex);
	DWORD GetSymbolIndex(CONST CHAR *strSymbol);
};


