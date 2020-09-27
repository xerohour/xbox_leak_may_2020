//-----------------------------------------------------------------------------
// File: WriteXBG.cpp
//
// Desc: Writes a .xbg file from a CD3DFile class (with a hierarchy of CD3DFrame
//       classes).
//
//       See the XBG.h header file for a better description of .xbg geometry
//       files.
//
// NOTE: THIS CODE, AND THE FILE FORMAT IT CREATES, IS SUBJECT TO CHANGE. FEEL
//       FREE, AS WELL, TO EXPAND OR MODIFY THE FILE FORMAT TO FIT YOUR 
//       PERSONAL REQUIREMENTS.
//
// Hist: 03.01.01 - New for April XDK release
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#include "stdafx.h"
#include <tchar.h>
#include <stdio.h>
#include <stdlib.h>
#include "xbg.h"


// File offset variables used during the writing of a geometry file to convert
// object pointers to file offsets.
DWORD g_dwMeshFileOffset;
DWORD g_dwSubsetFileOffset;
DWORD g_dwIndicesFileOffset;
DWORD g_dwVerticesFileOffset;

// Variables for how much file space each section of the file requires
DWORD g_dwNumFrames;
DWORD g_dwFrameSpace;
DWORD g_dwSubsetSpace;
DWORD g_dwIndicesSpace;
DWORD g_dwVerticesSpace;




//-----------------------------------------------------------------------------
// Name: ComputeMemoryRequirementsCB()
// Desc: Frame enumeration callback to compute memory requirements
//-----------------------------------------------------------------------------
BOOL ComputeMemoryRequirementsCB( CD3DFrame* pFrame, VOID* )
{
    g_dwNumFrames++;

    // Compute memory requirements
    g_dwFrameSpace    += sizeof(XBMESH_FRAME);
    g_dwSubsetSpace   += sizeof(XBMESH_SUBSET) * pFrame->m_dwNumMeshSubsets;
    g_dwIndicesSpace  += sizeof(WORD) * pFrame->m_dwNumMeshIndices;
    g_dwVerticesSpace += pFrame->m_dwMeshVertexSize * pFrame->m_dwNumMeshVertices;

    return TRUE;
}




//-----------------------------------------------------------------------------
// Name: WriteMeshInfoCB()
// Desc: Writes mesh info to a file
//-----------------------------------------------------------------------------
BOOL WriteMeshInfoCB( CD3DFrame* pFrame, VOID* pData )
{
    FILE* file = (FILE*)pData;

    // Set up mesh info to be written. Note that, in order for Xbox fast math
	// (via xgmath.h) to work, all D3DXMATRIX's must be 16-byte aligned.
    XBMESH_FRAME frame;
    strcpy( frame.m_strName, pFrame->m_strFrameName );
    frame.m_pChild       = NULL;
    frame.m_pNext        = NULL;
    frame.m_matTransform = pFrame->m_matTransform;

    frame.m_MeshData.m_VB.Common     = 1 | D3DCOMMON_TYPE_VERTEXBUFFER;
    frame.m_MeshData.m_VB.Data       = 0L;
    frame.m_MeshData.m_VB.Lock       = 0L;
    frame.m_MeshData.m_dwNumVertices = pFrame->m_dwNumMeshVertices;
    frame.m_MeshData.m_IB.Common     = 1 | D3DCOMMON_TYPE_INDEXBUFFER;
    frame.m_MeshData.m_IB.Data       = 0L;
    frame.m_MeshData.m_IB.Lock       = 0L;
    frame.m_MeshData.m_dwNumIndices  = pFrame->m_dwNumMeshIndices;
    frame.m_MeshData.m_dwFVF         = pFrame->m_dwMeshFVF;
    frame.m_MeshData.m_dwVertexSize  = pFrame->m_dwMeshVertexSize;
    frame.m_MeshData.m_dwPrimType    = (pFrame->m_dwMeshPrimType == D3DPT_TRIANGLELIST) ? (D3DPRIMITIVETYPE)5 : (D3DPRIMITIVETYPE)6;;
    frame.m_MeshData.m_dwNumSubsets  = pFrame->m_dwNumMeshSubsets;
    frame.m_MeshData.m_pSubsets      = NULL;

	DWORD size = sizeof(frame);


    // Write pointers as file offsets
    if( pFrame->m_pChild )  
        frame.m_pChild = (XBMESH_FRAME*)( g_dwMeshFileOffset + ( pFrame->m_pChild->m_dwEnumeratedID - 1 )* sizeof(XBMESH_FRAME) );
    if( pFrame->m_pNext )   
        frame.m_pNext  = (XBMESH_FRAME*)( g_dwMeshFileOffset + ( pFrame->m_pNext->m_dwEnumeratedID - 1 ) * sizeof(XBMESH_FRAME) );
    if( pFrame->m_dwNumMeshSubsets )
        frame.m_MeshData.m_pSubsets = (XBMESH_SUBSET*)g_dwSubsetFileOffset;
    if( frame.m_MeshData.m_dwNumIndices )
        frame.m_MeshData.m_IB.Data  = (DWORD)g_dwIndicesFileOffset;
    if( frame.m_MeshData.m_dwNumVertices )
        frame.m_MeshData.m_VB.Data  = (DWORD)g_dwVerticesFileOffset;

    g_dwSubsetFileOffset   += sizeof(XBMESH_SUBSET) * pFrame->m_dwNumMeshSubsets;
    g_dwIndicesFileOffset  += sizeof(WORD) * pFrame->m_dwNumMeshIndices;
    g_dwVerticesFileOffset += pFrame->m_dwMeshVertexSize * pFrame->m_dwNumMeshVertices;

    // Write out mesh info
    fwrite( &frame, 1, sizeof(XBMESH_FRAME), file ); 

    return TRUE;
}


    

//-----------------------------------------------------------------------------
// Name: WriteSubsetsCB()
// Desc: Write out the mesh subsets
//-----------------------------------------------------------------------------
BOOL WriteSubsetsCB( CD3DFrame* pFrame, VOID* pData )
{
    FILE* file = (FILE*)pData;

    VOID* pTexturePtr = NULL;

    for( DWORD i=0; i<pFrame->m_dwNumMeshSubsets; i++ )
    {
        fwrite( &pFrame->m_pMeshSubsets[i].mtrl, sizeof(D3DMATERIAL8), 1, file ); 
        fwrite( &pTexturePtr, sizeof(VOID*), 1, file ); 
        fwrite(  pFrame->m_pMeshSubsets[i].strTexture,    sizeof(CHAR), 64, file ); 
        fwrite( &pFrame->m_pMeshSubsets[i].dwVertexStart, sizeof(DWORD), 1, file ); 
        fwrite( &pFrame->m_pMeshSubsets[i].dwVertexCount, sizeof(DWORD), 1, file ); 
        fwrite( &pFrame->m_pMeshSubsets[i].dwIndexStart,  sizeof(DWORD), 1, file ); 
        fwrite( &pFrame->m_pMeshSubsets[i].dwIndexCount,  sizeof(DWORD), 1, file ); 
    }

    return TRUE;
}


    

//-----------------------------------------------------------------------------
// Name: WriteIndicesCB()
// Desc: Write out the mesh indices
//-----------------------------------------------------------------------------
BOOL WriteIndicesCB( CD3DFrame* pFrame, VOID* pData )
{
    FILE* file = (FILE*)pData;

    if( pFrame->m_dwNumMeshIndices )
    {
        BYTE* pIndexData;
        pFrame->m_pMeshIB->Lock( 0, 0, &pIndexData, 0 );
        fwrite( pIndexData, sizeof(WORD), pFrame->m_dwNumMeshIndices, file ); 
        pFrame->m_pMeshIB->Unlock();
    }

    return TRUE;
}


    

//-----------------------------------------------------------------------------
// Name: WriteVerticesCB()
// Desc: Write out the mesh vertices
//-----------------------------------------------------------------------------
BOOL WriteVerticesCB( CD3DFrame* pFrame, VOID* pData )
{
    FILE* file = (FILE*)pData;

    if( pFrame->m_dwNumMeshVertices )
    {
        BYTE* pVertexData;
        pFrame->m_pMeshVB->Lock( 0, 0, &pVertexData, 0 );
        fwrite( pVertexData, pFrame->m_dwMeshVertexSize, pFrame->m_dwNumMeshVertices, file ); 
        pFrame->m_pMeshVB->Unlock();
    }

    return TRUE;
}




//-----------------------------------------------------------------------------
// Name: WriteToXBG()
// Desc: Writes the geometry objects to a file
//-----------------------------------------------------------------------------
HRESULT CD3DFile::WriteToXBG( TCHAR* strFilename  )
{
    g_dwNumFrames = 0;

    // Before writing the file, walk the nodes to compute memory requirements
    g_dwFrameSpace    = 0;
    g_dwSubsetSpace   = 0;
    g_dwIndicesSpace  = 0;
    g_dwVerticesSpace = 0;
    m_pChild->EnumFrames( ComputeMemoryRequirementsCB, NULL );

    // As parts of the file are written, these global file offset variables
    // are used to convert object pointers to file offsets
    g_dwMeshFileOffset     = sizeof(XBG_HEADER);
    g_dwSubsetFileOffset   = g_dwMeshFileOffset + g_dwFrameSpace;
    g_dwIndicesFileOffset  = g_dwSubsetFileOffset + g_dwSubsetSpace;
    g_dwVerticesFileOffset = 0;

    // Setup the file header
    XBG_HEADER xbgHeader;
    xbgHeader.dwMagic         = XBG_FILE_ID;
    xbgHeader.dwNumMeshFrames = g_dwNumFrames;
    xbgHeader.dwSysMemSize    = g_dwFrameSpace + g_dwSubsetSpace + g_dwIndicesSpace;
    xbgHeader.dwVidMemSize    = g_dwVerticesSpace;

    // Open the file to write
    FILE* file = fopen( strFilename, "wb" );

    // Write out the header
    fwrite( &xbgHeader, sizeof(XBG_HEADER), 1, file ); 

    // Write the mesh's parts. Note that, starting at this file offset, in
	// order for Xbox fast math (via xgmath.h) to work, all D3DXMATRIX's must
	// be 16-byte aligned.
    m_pChild->EnumFrames( WriteMeshInfoCB, file );
    m_pChild->EnumFrames( WriteSubsetsCB,  file );
    m_pChild->EnumFrames( WriteIndicesCB,  file );
    m_pChild->EnumFrames( WriteVerticesCB, file );

    // Close the file
    fclose( file );

    return S_OK;
}



