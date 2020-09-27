///////////////////////////////////////////////////////////////////////////////
// File: PlacementDoodad.h
//
// Copyright 2001 Pipeworks Software
///////////////////////////////////////////////////////////////////////////////
#ifndef __PLACEMENTDOODAD_H__
#define __PLACEMENTDOODAD_H__


#include "defines.h"

#ifdef INCLUDE_PLACEMENT_DOODAD


// Renders a visible item at a specific location.
   

class PlacementDoodad
{
protected:
    LPDIRECT3DVERTEXBUFFER8 m_pVB;
    LPDIRECT3DINDEXBUFFER8  m_pIB;
    DWORD					m_dwNumVertices;
    DWORD					m_dwNumIndices;


public:
	void Init();
	void UnInit()
    {
        destroy();
    }

	void create();
	void destroy();

	void render(const D3DVECTOR* p_pos, const D3DMATRIX* view_mat, const D3DMATRIX* proj_mat);
};


#endif // INCLUDE_PLACEMENT_DOODAD
#endif // __PLACEMENTDOODAD_H__

