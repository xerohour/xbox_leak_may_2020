//-----------------------------------------------------------------------------
// File: XBG.h
//
// Desc: Structs for converting a DirectX geometry file (.x) into a geometry
//       file (.xbg) that can be more conveniently used by Xbox apps. An XBG
//       file stores the pre-parsed information needed to load and render
//       geometry. For comparison, parsing in .x files at load times requires
//       a large amount of code (internal to D3DX) and processing time.
//
//       After conversion from a .x file, geometry is typically a hierarchy of
//       frames, each potentially with a transformation matrix and meshdata.
//       Furthermore, each mesh has one or more subsets. A subset has the
//       material, texture, and range of primitives to use for rendering.
//
//       The .xbg file format is designed to load in two easy chunks: one
//       file read for the system memory objects (frames and indices) and one
//       file read for the video memory objects (vertices). Note that the
//       file format is forced (via the __declspec(align(16)) declarator) to
//       align structures such that D3XMATRIX structures are on 16-byte
//       boundaries. This is to be compatible with the Xbox fast math
//       functions (see xgmath.h).
//
//       The common framework used by XDK samples contain functions to load
//       XBG files. See the Common\include\XBMesh.h and Common\src\XBMesh.cpp
//       files for more information. All graphics samples on the XDK use
//       this common code to load .xbg files.
//
//       The file format of a .xbg looks like:
//
//       // Header info
//       struct XBG_HEADER
//       {
//          DWORD dwMagic;             // A file-type identifier
//          DWORD dwNumMeshFrames;     // Num of frames in the file
//          DWORD dwSysMemSize;        // Num system memory bytes req'd
//          DWORD dwVidMemSize;        // Num videro memorty bytes req'd
//       };
//
//       // Array of frames (XBMESH_FRAME x dwNumMeshFrames)
//       struct XBMESH_FRAME
//       {
//          D3DXMATRIX    m_matTransform; // Transformation matrix
//          XBMESH_DATA   m_MeshData;     // Mesh data
//          CHAR          m_strName[64];  // Name of this frame
//          XBMESH_FRAME* m_pChild;       // File offset to child frame
//          XBMESH_FRAME* m_pNext;        // File offset to siblingframe
//
//          // Where the mesh data is:
//          struct XBMESH_DATA
//          {
//             D3DVertexBuffer  m_VB;            // Xbox vertex buffer
//             DWORD            m_dwNumVertices; // Vertex count
//             D3DIndexBuffer   m_IB;            // Xbox index buffer
//             DWORD            m_dwNumIndices;  // Index count
//             DWORD            m_dwFVF;         // Vertex FVF
//             DWORD            m_dwVertexSize;  // Vertex size
//             D3DPRIMITIVETYPE m_dwPrimType;    // Primitive type
//             DWORD            m_dwNumSubsets;  // Num rendering subsets
//             XBMESH_SUBSET*   m_pSubsets;      // File offset to first subset
//           };
//       };
//
//       // Array of rendering subsets (all subsets for all frame's meshdata)
//       struct XBMESH_SUBSET
//       {
//          D3DMATERIAL8       mtrl;           // Material to render with
//          LPDIRECT3DTEXTURE8 pTexture;       // Texture to render with
//          CHAR               strTexture[64]; // Name of texture
//          DWORD              dwVertexStart;  // First vertex for this subset
//          DWORD              dwVertexCount;  // Num vertices for this subset
//          DWORD              dwIndexStart;   // First index for this subset
//          DWORD              dwIndexCount;   // Num indices for this subset
//       };
//
//       // The array of indices for all meshes in the file
//       WORD   indices[];             // Dump of index buffers contents

//       // Finally, the array of vertices for all meshes in the file
//       VERTEX vertices[];            // Dump of vertex buffers contents
//
// NOTE: THIS CODE, AND THE FILE FORMAT IT CREATES, IS SUBJECT TO CHANGE. FEEL
//       FREE, AS WELL, TO EXPAND OR MODIFY THE FILE FORMAT TO FIT YOUR 
//       PERSONAL REQUIREMENTS.
//
// Hist: 03.01.01 - New for April XDK release
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#include <tchar.h>
#include <stdio.h>
#include <stdlib.h>
#include <d3dx8.h>




// Rendering flags
#define D3DFILE_RENDEROPAQUESUBSETS    0x00000001
#define D3DFILE_RENDERALPHASUBSETS     0x00000002
#define D3DFILE_RENDERALLSUBSETS       (D3DFILE_RENDEROPAQUESUBSETS|D3DFILE_RENDERALPHASUBSETS)
#define D3DFILE_RENDERVISIBLEFRAMES    0x00000004
#define D3DFILE_RENDERALLFRAMES        0x00000008

// File writing flags
#define D3DFILE_WRITEMESHINFO    0x00000001
#define D3DFILE_WRITESUBSETS     0x00000002
#define D3DFILE_WRITEINDICES     0x00000004
#define D3DFILE_WRITEVERTICES    0x00000008

// The magic number to identify .xbg files
#define XBG_FILE_ID (((DWORD)'X'<<0)|(((DWORD)'B'<<8))|(((DWORD)'G'<<16))|(2<<24))




//-----------------------------------------------------------------------------
// Mimicking resource structures as the Xbox will see them
//-----------------------------------------------------------------------------
#define D3DCOMMON_TYPE_VERTEXBUFFER  0x00000000
#define D3DCOMMON_TYPE_INDEXBUFFER   0x00010000

typedef struct D3DResource
{
    DWORD Common;
    DWORD Data;
    DWORD Lock;
} D3DVertexBuffer, D3DIndexBuffer;




//-----------------------------------------------------------------------------
// Name: struct XBG_HEADER
// Desc: Header for a .xbg file
//-----------------------------------------------------------------------------
struct XBG_HEADER
{
    DWORD dwMagic;
    DWORD dwNumMeshFrames;
    DWORD dwSysMemSize;
    DWORD dwVidMemSize;
};




//-----------------------------------------------------------------------------
// Name: struct XBMESH_SUBSET
// Desc: Struct to hold data for rendering a mesh
//-----------------------------------------------------------------------------
struct XBMESH_SUBSET
{
    D3DMATERIAL8       mtrl;
    LPDIRECT3DTEXTURE8 pTexture;
    CHAR               strTexture[64];
    DWORD              dwVertexStart;
    DWORD              dwVertexCount;
    DWORD              dwIndexStart;
    DWORD              dwIndexCount;
};




//-----------------------------------------------------------------------------
// Name: struct XBMESH_DATA
// Desc: Struct to hold data for a mesh
//-----------------------------------------------------------------------------
struct XBMESH_DATA
{
    D3DVertexBuffer    m_VB;           // Mesh geometry
    DWORD              m_dwNumVertices;
    D3DIndexBuffer     m_IB;
    DWORD              m_dwNumIndices;
    
    DWORD              m_dwFVF;         // Mesh vertex info
    DWORD              m_dwVertexSize;
    D3DPRIMITIVETYPE   m_dwPrimType;

    DWORD              m_dwNumSubsets;  // Subset info, for rendering
    XBMESH_SUBSET*     m_pSubsets;
};




//-----------------------------------------------------------------------------
// Name: struct XBMESH_FRAME
// Desc: Struct to provide a hierarchial layout of meshes. Note: in order for
//       Xbox fast math (via xgmath.h) to work, all D3DXMATRIX's must be
//       16-byte aligned.
//-----------------------------------------------------------------------------
__declspec(align(16)) struct XBMESH_FRAME
{
    D3DXMATRIX         m_matTransform;  // Make sure this is 16-byte aligned!
    
    XBMESH_DATA        m_MeshData;
    
    CHAR               m_strName[64];
    
    XBMESH_FRAME*      m_pChild;
    XBMESH_FRAME*      m_pNext;
};




//-----------------------------------------------------------------------------
// Name: class CD3DFrame
// Desc: Class for providing a hierarchial layout of meshes.
//-----------------------------------------------------------------------------
class CD3DFrame
{
public:
    TCHAR      m_strFrameName[512];
    DWORD      m_dwEnumeratedID;   // Frame ID
    D3DXMATRIX m_matTransform;

    BOOL       m_bVisible;         // Allows visualization of a selected frame
    LONG       m_lSelectedSubset;  // Allows visualization of a selected mesh subset
    HTREEITEM  m_hFVFTreeItem;     // Tree ctrl item for updating the FVF text
    HTREEITEM  m_TreeData;         // Tree ctrl data

    CD3DFrame* m_pParent;          // Ptrs for the hierarchy
    CD3DFrame* m_pNext;
    CD3DFrame* m_pChild;

public:
    // Mesh properties
    D3DXVECTOR3             m_vMeshCenter;
    FLOAT                   m_fMeshRadius;

    // Mesh properties
    LPDIRECT3DVERTEXBUFFER8 m_pMeshVB;
    LPDIRECT3DINDEXBUFFER8  m_pMeshIB;
    DWORD                   m_dwNumMeshVertices;
    DWORD                   m_dwNumMeshIndices;
    DWORD                   m_dwNumMeshPolygons;
    DWORD                   m_dwMeshFVF;
    DWORD                   m_dwMeshVertexSize;
    D3DPRIMITIVETYPE        m_dwMeshPrimType;
    DWORD                   m_dwNumMeshSubsets;    // Subset info, for rendering
    XBMESH_SUBSET*          m_pMeshSubsets;

public:
    // Modification options
    HRESULT ComputeNormals();
    HRESULT SetMeshFVF( DWORD dwFVF );
    HRESULT Stripify( DWORD dwFlags );
	HRESULT Unstrip();

    // Functions to call a callback for every frame in the hierarchy
    BOOL    EnumFrames( BOOL (*EnumMeshCB)(CD3DFrame*,VOID*), VOID* pData );
    BOOL    EnumFrames( BOOL (*EnumMeshCB)(CD3DFrame*,D3DXMATRIX*,VOID*), VOID* pData );

    // Functions to render the frame hierarchy
    HRESULT RenderMeshSubsets( LPDIRECT3DDEVICE8 pd3dDevice, DWORD dwFirstSubset, 
                               DWORD dwLastSubset, DWORD dwFlags );
    HRESULT Render( LPDIRECT3DDEVICE8 pd3dDevice, DWORD dwFlags );

    // Creation/destruction
    HRESULT CreateMesh( LPDIRECT3DDEVICE8 pd3dDevice, TCHAR* strFilename );
    HRESULT CreateMesh( LPDIRECT3DDEVICE8 pd3dDevice, LPDIRECTXFILEDATA pFileData );
    HRESULT Destroy();

    // Construction
    CD3DFrame( LPDIRECT3DDEVICE8 pd3dDevice, CD3DFrame* pParent, XBMESH_FRAME* pXBFrame );
    CD3DFrame( TCHAR* strName = _T("") );
    virtual ~CD3DFrame();
};




//-----------------------------------------------------------------------------
// Name: class CD3DFile
// Desc: Class for loading and rendering file-based meshes
//-----------------------------------------------------------------------------
class CD3DFile : public CD3DFrame
{
public:
    // Global geoemtry attribtes
    DWORD       m_dwNumFrames;
    DWORD       m_dwNumMeshes;
    DWORD       m_dwNumVertices;
    DWORD       m_dwNumIndices;
    DWORD       m_dwNumPolygons;
    D3DXVECTOR3 m_vCenter;
    FLOAT       m_fRadius;

    // Functions for loading from a .x file
    HRESULT LoadFrame( LPDIRECT3DDEVICE8 pd3dDevice, LPDIRECTXFILEDATA pFileData, 
                       CD3DFrame* pParentFrame );

public:
    // Functions to read and write geometry files
    HRESULT LoadCollapsedMeshFromX( LPDIRECT3DDEVICE8 pd3dDevice, TCHAR* strFilename );
    HRESULT LoadFromX( LPDIRECT3DDEVICE8 pd3dDevice, TCHAR* strFilename );
    HRESULT LoadFromXBG( LPDIRECT3DDEVICE8 pd3dDevice, TCHAR* strFilename );
    HRESULT WriteToXBG( TCHAR* strFilename );

    // Create method
    HRESULT Create( LPDIRECT3DDEVICE8 pd3dDevice, TCHAR* strFilename, 
                    BOOL bCollapseMesh = FALSE );

    CD3DFile();
};




//-----------------------------------------------------------------------------
// Name: D3DFile_CreateTexture()
// Desc: Helper function to load a texture file
//-----------------------------------------------------------------------------
HRESULT D3DFile_CreateTexture( LPDIRECT3DDEVICE8 pd3dDevice, TCHAR* strTexture,
                               LPDIRECT3DTEXTURE8* ppTexture );




