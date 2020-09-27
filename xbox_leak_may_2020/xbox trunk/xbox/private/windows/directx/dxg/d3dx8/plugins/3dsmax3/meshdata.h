//-----------------------------------------------------------------------------
// File: MeshData.h
//
// Desc: Structures to store a reformated mesh in (split vertices, etc)
//
// Copyright (C) 1998-2000 Microsoft Corporation. All Rights Reserved.
//-----------------------------------------------------------------------------
#ifndef __MESHDATA_H__
#define __MESHDATA_H__

// ================================================== CVertexNode
// Node for a data stucture what will help expand an array of vertices
// into a list of vertices each of which may have more than one normal based
// on the smoothing groups that the vertex belongs to.  This new expanded
// list of vertices (with duplicated verts) will be what gets exported out.
struct SVertexData
{
    Point3 vNormal;
    DWORD iPointRep; // index not including duplicated vertices (raw vertex index)
    DWORD iWedgeList; // index to next vertex in a list of vertices 

    DWORD iTextureIndex;
    DWORD iSmoothingGroupIndex;
    DWORD iMaterial;
};

// ================================================== CFaceData
struct SFaceData
{
    DWORD index[3];
};

struct SMeshData
{
    SMeshData()
        :m_rgVertices(NULL), m_rgFaces(NULL) {}

    ~SMeshData()
    {
        delete []m_rgVertices;
        delete []m_rgFaces;
    }

    BOOL m_bTexCoordsPresent;
    DWORD m_cFaces;
    DWORD m_cVertices;
    DWORD m_cVerticesBeforeDuplication;

    SVertexData *m_rgVertices;
    SFaceData *m_rgFaces;

};

HRESULT GenerateMeshData
    (
    Mesh *pMesh,
    SMeshData *pMeshData,
    DWORD *rgdwMeshMaterials
    );

struct SPatchVertexData
{
    Point3 vPosition;
    DWORD iPointRep; // index not including duplicated vertices (raw vertex index)
    DWORD iWedgeList; // index to next vertex in a list of vertices 

    DWORD iTextureIndex;
};

struct SPatchData
{
	DWORD m_cControl;
	DWORD m_rgdwControl[16];
};

struct SPatchMeshData
{
    SPatchMeshData()
        :m_rgVertices(NULL), m_rgPatches(NULL) {}

    ~SPatchMeshData()
    {
        delete []m_rgVertices;
        delete []m_rgPatches;
    }

    BOOL m_bTexCoordsPresent;
    DWORD m_cPatches;
    DWORD m_cVertices;
    DWORD m_cVerticesBeforeDuplication;

    SPatchVertexData *m_rgVertices;
    SPatchData *m_rgPatches;
};

HRESULT GeneratePatchMeshData
    (
    PatchMesh *pPatchMesh,
    SPatchMeshData *pPatchMeshData 
    );

enum 
{ 
	bmtex_clipu,bmtex_clipv,bmtex_clipw,bmtex_cliph,
	bmtex_jitter,bmtex_usejitter,
	bmtex_apply,bmtex_crop_place,
	bmtex_filtering,
	bmtex_monooutput,
	bmtex_rgboutput,
	bmtex_alphasource,
	bmtex_premultalpha,
	bmtex_bitmap,
	bmtex_coords,	 // access for UVW mapping
	bmtex_output,	 //output window
	bmtex_filename   // bitmap filename virtual parameter, JBW 2/23/99
};

struct SCropInfo
{
    float fClipU;
    float fClipV;
    float fClipW;
    float fClipH;
};

#endif // __MESHDATA_H__