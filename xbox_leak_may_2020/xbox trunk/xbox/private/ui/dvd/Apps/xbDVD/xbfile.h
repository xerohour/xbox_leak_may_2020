//-----------------------------------------------------------------------------
// File: XBFile.h
//
// Desc: Support code for loading DirectX .X files.
//
//       Note: the mesh code also supports loading .xbg files. These files are
//       more-or-less pure dumps of a vertex buffer and an index buffer for a
//       mesh, so they are trivial and fast to load. They are created with the
//       MakeXBG tool, found on the XDK.
//
// Hist: 11.01.00 - New for November XDK release
//       12.15.00 - Changes for December XDK release
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#ifndef XBFILE_H
#define XBFILE_H
#include "XBUtil.h"



// Rendering flags. Most common usage is:
//    XBGMESH_RENDERALLSUBSETS|XBGMESH_USEALLMESHPROPERTIES

#define XBGMESH_RENDEROPAQUESUBSETS  0x00000001
#define XBGMESH_RENDERALPHASUBSETS   0x00000002
#define XBGMESH_RENDERALLSUBSETS     0x0000000f

#define XBGMESH_USEMESHMATERIALS     0x00000010
#define XBGMESH_USEMESHTEXTURES      0x00000020
#define XBGMESH_USEMESHFVF           0x00000040
#define XBGMESH_USEALLMESHPROPERTIES 0x000000f0


// The magic number to identify .xbg files
#define XBG_FILE_ID  (((DWORD)'X'<<0)|(((DWORD)'B'<<8))|(((DWORD)'G'<<16)))




//-----------------------------------------------------------------------------
// Name: struct XBGMESH_SUBSET
// Desc: Struct to hold data for rendering a mesh
//-----------------------------------------------------------------------------
struct XBGMESH_SUBSET
{
    D3DXMATRIX         mat;
    D3DMATERIAL8       mtrl;
    LPDIRECT3DTEXTURE8 pTexture;
    DWORD              dwVertexStart;
    DWORD              dwVertexCount;
    DWORD              dwIndexStart;
    DWORD              dwIndexCount;
};




//-----------------------------------------------------------------------------
// Name: struct XBGMESH_INFO
// Desc: Struct to hold data for rendering a mesh
//-----------------------------------------------------------------------------
struct XBGMESH_INFO
{
    D3DVertexBuffer         m_VB;           // Mesh geometry
    DWORD                   m_dwNumVertices;
    D3DIndexBuffer          m_IB;
    DWORD                   m_dwNumIndices;
    
    DWORD                   m_dwFVF;         // Mesh vertex info
    DWORD                   m_dwVertexSize;
    D3DPRIMITIVETYPE        m_dwPrimType;

    DWORD                   m_dwNumSubsets;  // Subset info, for rendering
};




//-----------------------------------------------------------------------------
// Name: class CXBGMesh
// Desc: Class for loading and rendering file-based meshes
//-----------------------------------------------------------------------------
class CXBGMesh : public XBGMESH_INFO
{
public:
    XBGMESH_SUBSET*         m_pSubsets;

public:
    // Mesh access
    LPDIRECT3DVERTEXBUFFER8 GetVertexBuffer() { return &m_VB; }
    DWORD                   GetNumVertices()  { return  m_dwNumVertices; }
    LPDIRECT3DINDEXBUFFER8  GetIndexBuffer()  { return &m_IB; }
    DWORD                   GetNumIndices()   { return  m_dwNumIndices; }
    DWORD                   GetFVF()          { return  m_dwFVF; }
    DWORD                   GetVertexSize()   { return  m_dwVertexSize; }
    D3DPRIMITIVETYPE        GetPrimType()     { return  m_dwPrimType; }

    // A callback so that a derived class can tweak state before rendering 
    // each subset. It returns a boolean for whether to still render the mesh.
    virtual BOOL    RenderCallback( LPDIRECT3DDEVICE8, DWORD, 
                                    XBGMESH_SUBSET* ) { return TRUE; }
    
    // Tha main render function, with lots of override functionality
    virtual HRESULT Render( LPDIRECT3DDEVICE8 pd3dDevice, DWORD dwFlags );

    // Creation/destruction
    virtual HRESULT Create( LPDIRECT3DDEVICE8 pd3dDevice, CHAR* strFileName );
    virtual VOID    Destroy();

	// Reference counting
	DWORD   m_dwRefCount;
	DWORD	AddRef()  { return ++m_dwRefCount; }
	DWORD   Release() { if( --m_dwRefCount ) 
		                    return m_dwRefCount;
	                    delete this; 
						return 0L; }

	CXBGMesh();
   ~CXBGMesh();
};




//-----------------------------------------------------------------------------
// Name: struct XBMESH_SUBSET
// Desc: Struct to hold data for rendering a mesh
//-----------------------------------------------------------------------------
struct XBMESH_SUBSET
{
    D3DMATERIAL8       mtrl;
    LPDIRECT3DTEXTURE8 pTexture;
    DWORD              dwVertexStart;
    DWORD              dwVertexCount;
    DWORD              dwIndexStart;
    DWORD              dwIndexCount;
};




//-----------------------------------------------------------------------------
// Name: class CXBMesh
// Desc: Class for loading and rendering file-based meshes
//-----------------------------------------------------------------------------
class CXBMesh
{
public:
    CHAR                    m_strName[512];  // Name of the mesh
	
    LPDIRECT3DVERTEXBUFFER8 m_pVB;           // Mesh geometry
    DWORD                   m_dwNumVertices;
    LPDIRECT3DINDEXBUFFER8  m_pIB;
    DWORD                   m_dwNumIndices;
    
    DWORD                   m_dwFVF;         // Mesh vertex info
    DWORD                   m_dwVertexSize;
	D3DPRIMITIVETYPE        m_dwPrimType;

    DWORD                   m_dwNumSubsets;  // Subset info, for rendering
    XBMESH_SUBSET*          m_pSubsets;

    BOOL                    m_bUseMaterials; // Whether to override materials
    BOOL                    m_bUseFVF;       // Whether to override FVF

	DWORD					m_dwRefCount;	 // Reference count

public:
    // Rendering
    HRESULT Render( LPDIRECT3DDEVICE8 pd3dDevice, 
                    BOOL bDrawOpaqueSubsets = TRUE,
                    BOOL bDrawAlphaSubsets = TRUE );

    // Mesh access
    LPDIRECT3DVERTEXBUFFER8 GetVertexBuffer() { return m_pVB; }
    DWORD                   GetNumVertices()  { return m_dwNumVertices; }
    LPDIRECT3DINDEXBUFFER8  GetIndexBuffer()  { return m_pIB; }
    DWORD                   GetNumIndices()   { return m_dwNumIndices; }
	DWORD                   GetFVF()          { return m_dwFVF; }
	DWORD                   GetVertexSize()   { return m_dwVertexSize; }
	D3DPRIMITIVETYPE        GetPrimType()     { return m_dwPrimType; }

	// Reference counting
	DWORD	AddRef()  { return ++m_dwRefCount; }
	DWORD   Release() { if( --m_dwRefCount ) 
		                    return m_dwRefCount;
	                    delete this; 
						return 0L; }

    // Rendering options
    HRESULT ComputeNormals();
    VOID    UseMeshMaterials( BOOL bFlag ) { m_bUseMaterials = bFlag; }
    VOID    UseMeshFVF( BOOL bFlag )       { m_bUseFVF       = bFlag; }

    // Creation/destruction
	HRESULT CreateFromXBG( LPDIRECT3DDEVICE8 pd3dDevice, CHAR* strFileName );
	HRESULT CreateFromD3DXMesh( LPDIRECT3DDEVICE8 pd3dDevice, LPD3DXMESH pD3DXMesh,
                                LPD3DXBUFFER pAdjacencyBuffer, LPD3DXBUFFER pMtrlBuffer, 
                                DWORD dwFVF=0L );
    HRESULT CreateFromX( LPDIRECT3DDEVICE8 pd3dDevice, CHAR* strFilename,
                         DWORD dwFVF=0L );
    HRESULT CreateFromXFileData( LPDIRECT3DDEVICE8 pd3dDevice, LPDIRECTXFILEDATA pFileData, 
                                 DWORD dwFVF=0L );
	HRESULT Create( LPDIRECT3DDEVICE8 pd3dDevice, CHAR* strFilename, DWORD dwFVF = 0L );
    HRESULT Destroy();

    CXBMesh( CHAR* strName = "XBFile_Mesh" );
    virtual ~CXBMesh();
};




//-----------------------------------------------------------------------------
// Name: class CXBFrame
// Desc: Class for loading and rendering file-based meshes
//-----------------------------------------------------------------------------
class CXBFrame
{
public:
    CHAR       m_strName[512];
    D3DXMATRIX m_mat;
    CXBMesh*   m_pMesh;
    CHAR       m_strMeshName[512];

    CXBFrame*  m_pNext;
    CXBFrame*  m_pChild;

public:
    // Matrix access
    VOID        SetMatrix( D3DXMATRIX* pmat ) { m_mat = *pmat; }
    D3DXMATRIX* GetMatrix()                   { return &m_mat; }

    CXBMesh*    FindMesh( CHAR* strMeshName );
    CXBFrame*   FindFrame( CHAR* strFrameName );
    BOOL        EnumMeshes( BOOL (*EnumMeshCB)(CXBMesh*,VOID*), 
                            VOID* pContext );

    HRESULT Destroy();
    HRESULT Render( LPDIRECT3DDEVICE8 pd3dDevice, 
                    BOOL bDrawOpaqueSubsets = TRUE,
                    BOOL bDrawAlphaSubsets = TRUE );
    
    CXBFrame( CHAR* strName = "XBFile_Frame" );
    virtual ~CXBFrame();
};




//-----------------------------------------------------------------------------
// Name: class CXBFile
// Desc: Class for loading and rendering file-based meshes
//-----------------------------------------------------------------------------
class CXBFile : public CXBFrame
{
    HRESULT LoadMesh( LPDIRECT3DDEVICE8 pd3dDevice, LPDIRECTXFILEDATA pFileData, 
                      CXBFrame* pParentFrame, DWORD dwFVF=0L );
    HRESULT LoadFrame( LPDIRECT3DDEVICE8 pd3dDevice, LPDIRECTXFILEDATA pFileData, 
                       CXBFrame* pParentFrame, DWORD dwFVF=0L );
public:
    HRESULT Create( LPDIRECT3DDEVICE8 pd3dDevice, CHAR* strFilename, 
                    DWORD dwFVF=0L );
    HRESULT Render( LPDIRECT3DDEVICE8 pd3dDevice );

    CXBFile() : CXBFrame( "XBFile_Root" ) {}
};




#endif
