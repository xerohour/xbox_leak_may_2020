//-----------------------------------------------------------------------------
// File: Material.h
//
// Desc: Header file for the main material definition class
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#ifndef MATERIAL_H
#define MATERIAL_H

#include "texturecache.h"

// free functions used by this class
void SetFalloffShaderValues(const D3DXCOLOR& sideColor, const D3DXCOLOR& frontColor);
DWORD GetEffectShader(int nEffect, DWORD fvf);
void SetFalloffShaderFrameValues();
void SetReflectShaderFrameValues();

//-----------------------------------------------------------------------------
// Name: class CBaseMaterial
// Desc: Base material class
//-----------------------------------------------------------------------------
class CBaseMaterial
{
public:
	CBaseMaterial();
	CBaseMaterial(char *sMatName, int index);
	virtual ~CBaseMaterial();

	char			*m_sMatName;	// Name of the material
	D3DMATERIAL8	m_MaterialDef;	// D3D material definition (ambient, diffuse, specular)
	int				m_ID;			// index into array where its stored

	void SetName(char *matName);
	void SetAllColors(float r, float g, float b, float a);
	void SetDiffuse(float r, float g, float b, float a);
	void SetAmbient(float r, float g, float b, float a);
	void SetSpecular(float r, float g, float b, float a);
	void SetEmissive(float r, float g, float b, float a);
	void SetPower(float power);
	virtual HRESULT Setup( DWORD FVF, CTextureCache *pTexCache );
    virtual bool    HasTexture() const { return false;}
};

//-----------------------------------------------------------------------------
// Name: class CSolidMaterial
// Desc: Solid material class
//-----------------------------------------------------------------------------
class CSolidTexMaterial : public CBaseMaterial
{
public:
	CSolidTexMaterial();
	CSolidTexMaterial(char *sMatName, int index);
	virtual ~CSolidTexMaterial();
	
	char    *m_sTexName;
	long	m_nTextureIndex;

	HRESULT SetTexture(char *texFileName);
	HRESULT Setup( DWORD FVF, CTextureCache *pTexCache );
    virtual bool    HasTexture() const { return true;}

};

//-----------------------------------------------------------------------------
// Name: class CBackingMaterial
// Desc: Backing material class
//-----------------------------------------------------------------------------
class CBackingTexMaterial : public CBaseMaterial
{
public:
	CBackingTexMaterial();
	CBackingTexMaterial(char *sMatName, int index);
	virtual ~CBackingTexMaterial();
	
	char    *m_sTexName;
	long	m_nTextureIndex;

	HRESULT SetTexture(char *texFileName);
	HRESULT Setup( DWORD FVF, CTextureCache *pTexCache );
    virtual bool    HasTexture() const { return true;}
};

//-----------------------------------------------------------------------------
// Name: class CFalloffMaterial
// Desc: Solid material class
//-----------------------------------------------------------------------------
class CFalloffMaterial : public CBaseMaterial
{
public:
	CFalloffMaterial(char *sMatName, int index);
	virtual ~CFalloffMaterial();

	D3DCOLOR m_colorSide;
	D3DCOLOR m_colorFront;

	void	SetFalloffColors(D3DCOLOR side, D3DCOLOR front);
	HRESULT Setup( DWORD FVF, CTextureCache *pTexCache );
};	

//-----------------------------------------------------------------------------
// Name: class CFalloffTexMaterial
// Desc: Solid material class
//-----------------------------------------------------------------------------
class CFalloffTexMaterial : public CBaseMaterial
{
public:
	CFalloffTexMaterial(char *sMatName, int index);
	virtual ~CFalloffTexMaterial();

	D3DCOLOR m_colorSide;
	D3DCOLOR m_colorFront;

	char	*m_sTexName;
	long	m_nTextureIndex;

	HRESULT SetTexture(char *texFileName);
	void	SetFalloffColors(D3DCOLOR side, D3DCOLOR front);
	HRESULT Setup( DWORD FVF, CTextureCache *pTexCache );
    virtual bool    HasTexture() const { return true;}
};	

#endif


