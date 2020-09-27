//-----------------------------------------------------------------------------
// File: MaterialLibrary.h
//
// Desc: Header file for the main material management class
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#ifndef MATERIALLIBRARY_H
#define MATERIALLIBRARY_H

#include "material.h"
#include "texturecache.h"
#include "xoconst.h"

enum eMaterialLib
{
	eFlatSurfaces = 0,
	eEggGlow,
	eHilightedType,
	eFlatSurfaces2sided3,
	eIconParts,
	eCellEgg_Parts,
	eGameHilite,
	eCellWallStructure,
	eGamePod,
	eInnerWall_02,
	eDarkenBacking,
	eInnerWall_01,
	eMetal_Chrome,
	eTubes,
	eNavType,
	eShell,
	eFlatSrfc_PodParts,
	eXBoxGreen,
	ePanelBacking_01,
	ePanelBacking_02,
	ePanelBacking_03,
	ePanelBacking_04,
	ePanelBacking_05,
	ePanelBacking_06,
	eCell_Light,
	eMenuCell,
	eWireframe,
	eOuterWall01,
    eredHighlight,
    egreenHighlight,
	eMissingMaterial,

	eNum_Materials
};

#define NUMBER_OF_MATERIALS eNum_Materials

//-----------------------------------------------------------------------------
// Name: class CMaterialLibrary
// Desc: Base material manager class
//-----------------------------------------------------------------------------
class CMaterialLibrary
{
public:
	long				m_nCurrentMat;						  // index of the current material in the array
	CBaseMaterial*		m_pMaterialLib[NUMBER_OF_MATERIALS];  // array of all the available 3D engine materials
	CTextureCache		m_TextureCache;
	
	long				m_nASEentries;	// the number of materials in the current ASE file
	long				*m_pASEtable;	// the conversion table when loading the ASE file

	CMaterialLibrary();
	~CMaterialLibrary();

	HRESULT Initialize( void );
	HRESULT Setup( long index, DWORD FVF );
	HRESULT LoadFromASE( long nMtrlID, BYTE* pBuffer );
	long	ReturnIndex( const long tableIndex );
	long	ReturnIndex( const char *szMatName );
	long	GetButtonTextMaterial( eButtonState curState = (eButtonState)eButtonDefault );
    bool    MaterialUsesTexture( long nMtrlID );
};

#endif