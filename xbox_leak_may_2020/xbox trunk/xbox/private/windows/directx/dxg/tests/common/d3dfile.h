//-----------------------------------------------------------------------------
// File: D3DFile.h
//
// Desc: Support code for loading DirectX .X files.
//
// Copyright (c) 1997-1999 Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#ifndef D3DFILE_H
#define D3DFILE_H
#define STRICT
#include <d3d8.h>
#include <d3dx8.h>




//-----------------------------------------------------------------------------
// Name: class CD3DMesh
// Desc: Class for loading and rendering file-based meshes
//-----------------------------------------------------------------------------
class CD3DMesh
{
public:
	CHAR                m_strName[512];

    LPD3DXMESH          m_pSysMemMesh;    // SysSem mesh, lives through resize
    LPD3DXMESH          m_pLocalMesh;     // Local mesh, rebuilt on resize
    
    DWORD               m_dwNumMaterials; // Materials for the mesh
    D3DMATERIAL8*       m_pMaterials;
    LPDIRECT3DTEXTURE8* m_pTextures;
	BOOL                m_bUseMaterials;

public:
	// Rendering
    void Render( LPDIRECT3DDEVICE8 pd3dDevice, 
		            BOOL bDrawOpaqueSubsets = TRUE,
		            BOOL bDrawAlphaSubsets = TRUE );

	// Mesh access
    LPD3DXMESH GetSysMemMesh() { return m_pSysMemMesh; }
    LPD3DXMESH GetLocalMesh()  { return m_pLocalMesh; }

	// Rendering options
	VOID    UseMeshMaterials( BOOL bFlag ) { m_bUseMaterials = bFlag; }
	void SetFVF( LPDIRECT3DDEVICE8 pd3dDevice, DWORD dwFVF );

	// Initializing
    void RestoreDeviceObjects( LPDIRECT3DDEVICE8 pd3dDevice );
    void InvalidateDeviceObjects();

	// Creation/destruction
	void Create( LPDIRECT3DDEVICE8 pd3dDevice, CHAR* strFilename );
	void Create( LPDIRECT3DDEVICE8 pd3dDevice, LPDIRECTXFILEDATA pFileData );
	void Destroy();

    CD3DMesh( CHAR* strName = "CD3DFile_Mesh" );
    virtual ~CD3DMesh();
};




//-----------------------------------------------------------------------------
// Name: class CD3DFrame
// Desc: Class for loading and rendering file-based meshes
//-----------------------------------------------------------------------------
class CD3DFrame
{
public:
	CHAR       m_strName[512];
	D3DXMATRIX m_mat;
	CD3DMesh*  m_pMesh;
	CHAR       m_strMeshName[512];

	CD3DFrame* m_pNext;
	CD3DFrame* m_pChild;

public:
	// Matrix access
    VOID        SetMatrix( D3DXMATRIX* pmat ) { m_mat = *pmat; }
    D3DXMATRIX* GetMatrix()                   { return &m_mat; }

	CD3DMesh*   FindMesh( CHAR* strMeshName );
	CD3DFrame*  FindFrame( CHAR* strFrameName );
    BOOL        EnumMeshes( BOOL (*EnumMeshCB)(CD3DMesh*,VOID*), 
		                    VOID* pContext );

	void Destroy();
    void RestoreDeviceObjects( LPDIRECT3DDEVICE8 pd3dDevice );
    void InvalidateDeviceObjects();
    void Render( LPDIRECT3DDEVICE8 pd3dDevice, 
		            BOOL bDrawOpaqueSubsets = TRUE,
		            BOOL bDrawAlphaSubsets = TRUE );
	
	CD3DFrame( CHAR* strName = "CD3DFile_Frame" );
	virtual ~CD3DFrame();
};




//-----------------------------------------------------------------------------
// Name: class CD3DFile
// Desc: Class for loading and rendering file-based meshes
//-----------------------------------------------------------------------------
class CD3DFile : public CD3DFrame
{
	void LoadMesh( LPDIRECT3DDEVICE8 pd3dDevice, LPDIRECTXFILEDATA pFileData, 
					  CD3DFrame* pParentFrame );
	void LoadFrame( LPDIRECT3DDEVICE8 pd3dDevice, LPDIRECTXFILEDATA pFileData, 
		               CD3DFrame* pParentFrame );
public:
	void Create( LPDIRECT3DDEVICE8 pd3dDevice, CHAR* strFilename );
	void Render( LPDIRECT3DDEVICE8 pd3dDevice );

	CD3DFile() : CD3DFrame( "CD3DFile_Root" ) {}
};




#endif



