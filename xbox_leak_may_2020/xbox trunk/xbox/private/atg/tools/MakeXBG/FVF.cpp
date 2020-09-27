//-----------------------------------------------------------------------------
// File: FVF.cpp
//
// Desc: Code for dealing with flexible vertex formats.
//
// Hist: 03.01.01 - New for April XDK release
//       11.08.01 - Fixed FVF-cracking problem
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#include "stdafx.h"
#include <d3dx8.h>
#include "FVF.h"




//-----------------------------------------------------------------------------
// Name: CrackFVF()
// Desc: Cracks an FVF code into individual pieces stored in a VERTEXFORMAT
//       structure. It makes life easier when deailing with vertices of any
//       format.
//-----------------------------------------------------------------------------
VOID CrackFVF( DWORD dwFVF, VERTEXFORMAT& fvf )
{
    DWORD dwOffset = 0;
    ZeroMemory( &fvf, sizeof(fvf) );

    // Handle position
    if( dwFVF & D3DFVF_XYZ )
    {
        fvf.bPosition        = TRUE;
        fvf.dwPositionOffset = dwOffset;
        dwOffset += 3 * sizeof(FLOAT);
    }

    // Handle blendweights
    switch( dwFVF & D3DFVF_POSITION_MASK )
    {
        case D3DFVF_XYZB1: fvf.dwNumBlendWeights = 1; break;
        case D3DFVF_XYZB2: fvf.dwNumBlendWeights = 2; break;
        case D3DFVF_XYZB3: fvf.dwNumBlendWeights = 3; break;
        case D3DFVF_XYZB4: fvf.dwNumBlendWeights = 4; break;
        default:           fvf.dwNumBlendWeights = 0; break;
    }
    fvf.dwBlendWeightsOffset = dwOffset;
    dwOffset += fvf.dwNumBlendWeights * sizeof(FLOAT);

    // Handle normal
    if( dwFVF & D3DFVF_NORMAL )
    {
        fvf.bNormal        = TRUE;
        fvf.dwNormalOffset = dwOffset;
        dwOffset += 3 * sizeof(FLOAT);
    }

    // Handle diffuse
    if( dwFVF & D3DFVF_DIFFUSE )
    {
        fvf.bDiffuse        = TRUE;
        fvf.dwDiffuseOffset = dwOffset;
        dwOffset += sizeof(DWORD);
    }

    // Handle specular
    if( dwFVF & D3DFVF_SPECULAR )
    {
        fvf.bSpecular        = TRUE;
        fvf.dwSpecularOffset = dwOffset;
        dwOffset += sizeof(DWORD);
    }

    // Handle texturecoods
    DWORD dwNumTexStages = ( dwFVF & D3DFVF_TEXCOUNT_MASK ) >> D3DFVF_TEXCOUNT_SHIFT;

    for( DWORD i=0; i<4; i++ )
    {
        fvf.dwNumTexCoords[i] = 0;

        if( i < dwNumTexStages )
        {
            DWORD dwNumTexturesCode = 0x00000003 & ( dwFVF >> (i*2+16) );
            if( D3DFVF_TEXTUREFORMAT1 == dwNumTexturesCode )
                fvf.dwNumTexCoords[i] = 1;
            if( D3DFVF_TEXTUREFORMAT2 == dwNumTexturesCode )
                fvf.dwNumTexCoords[i] = 2;
            if( D3DFVF_TEXTUREFORMAT3 == dwNumTexturesCode )
                fvf.dwNumTexCoords[i] = 3;
            if( D3DFVF_TEXTUREFORMAT4 == dwNumTexturesCode )
                fvf.dwNumTexCoords[i] = 4;
        
            fvf.dwTexCoordsOffset[i] = dwOffset;
            dwOffset += fvf.dwNumTexCoords[i] * sizeof(FLOAT);
        }
    }

    fvf.dwVertexSize = dwOffset;
}




//-----------------------------------------------------------------------------
// Name: EncodeFVF()
// Desc: The opposite of CrackFVF, this builds an FVF code from the vertex
//       description in the VERTEXFORMAT structure.
//-----------------------------------------------------------------------------
DWORD EncodeFVF( VERTEXFORMAT& fvf )
{
    DWORD dwFVF = 0;

    // Handle position and blendweights
    switch( fvf.dwNumBlendWeights )
    {
        case 0: dwFVF = D3DFVF_XYZ;   break;
        case 1: dwFVF = D3DFVF_XYZB1; break;
        case 2: dwFVF = D3DFVF_XYZB2; break;
        case 3: dwFVF = D3DFVF_XYZB3; break;
        case 4: dwFVF = D3DFVF_XYZB4; break;
    }

    // Handle normal, diffuse, and specular
    if( fvf.bNormal )   dwFVF |= D3DFVF_NORMAL;
    if( fvf.bDiffuse )  dwFVF |= D3DFVF_DIFFUSE;
    if( fvf.bSpecular ) dwFVF |= D3DFVF_SPECULAR;

    DWORD dwNumTexStages = 0;

    // Handle texturecoods
    for( DWORD i=0; i<4; i++ )
    {
        if( fvf.dwNumTexCoords[i] > 0 )
        {
            dwNumTexStages++;
            if( fvf.dwNumTexCoords[i] == 1 ) dwFVF |= D3DFVF_TEXCOORDSIZE1(i);
            if( fvf.dwNumTexCoords[i] == 2 ) dwFVF |= D3DFVF_TEXCOORDSIZE2(i);
            if( fvf.dwNumTexCoords[i] == 3 ) dwFVF |= D3DFVF_TEXCOORDSIZE3(i);
            if( fvf.dwNumTexCoords[i] == 4 ) dwFVF |= D3DFVF_TEXCOORDSIZE4(i);
        }
    }

    dwFVF |= ( dwNumTexStages << D3DFVF_TEXCOUNT_SHIFT );

    return dwFVF;
}




//-----------------------------------------------------------------------------
// Name: CopyVertexBufferWithNewFVF()
// Desc: Makes a copy of a vertex buffer, using a new FVF code.
//-----------------------------------------------------------------------------
HRESULT CopyVertexBufferWithNewFVF( LPDIRECT3DVERTEXBUFFER8 pOldVB, DWORD dwNumVertices,
                                    DWORD dwOldFVF, DWORD dwNewFVF,
                                    LPDIRECT3DVERTEXBUFFER8* ppNewVB )
{
    // Crack the FVF codes
    VERTEXFORMAT fvfOld;
    VERTEXFORMAT fvfNew;
    CrackFVF( dwOldFVF, fvfOld );
    CrackFVF( dwNewFVF, fvfNew );

    // Create new vertex buffer
    LPDIRECT3DDEVICE8 pd3dDevice;
    pOldVB->GetDevice( &pd3dDevice );
    pd3dDevice->CreateVertexBuffer( dwNumVertices * fvfNew.dwVertexSize, 
                                    D3DUSAGE_WRITEONLY, 0, D3DPOOL_MANAGED, 
                                    ppNewVB );
    pd3dDevice->Release();
    
    // Copy vertices
    BYTE* pDstVertices;
    BYTE* pSrcVertices;
    pOldVB->Lock( 0, 0, (BYTE**)&pSrcVertices, 0 );
    (*ppNewVB)->Lock( 0, 0, (BYTE**)&pDstVertices, 0 );
    
    for( DWORD i=0; i<dwNumVertices; i++ )
    {
        D3DXVECTOR3 vPos( 0.0f, 0.0f, 0.0f );
        FLOAT       fWeights[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
        D3DXVECTOR3 vNormal( 0.0f, 0.0f, 0.0f );
        DWORD       dwDiffuse  = 0x00000000;
        DWORD       dwSpecular = 0x00000000;
        D3DXVECTOR4 vT0( 0.0f, 0.0f, 0.0f, 0.0f );
        D3DXVECTOR4 vT1( 0.0f, 0.0f, 0.0f, 0.0f );
        D3DXVECTOR4 vT2( 0.0f, 0.0f, 0.0f, 0.0f );
        D3DXVECTOR4 vT3( 0.0f, 0.0f, 0.0f, 0.0f );

        // Get source position
        if( fvfOld.bPosition )
            memcpy( &vPos, pSrcVertices+fvfOld.dwPositionOffset, sizeof(D3DXVECTOR3) );

        // Get source blend weights
        memcpy( &fWeights[0], pSrcVertices+fvfOld.dwBlendWeightsOffset, fvfOld.dwNumBlendWeights * sizeof(FLOAT) );

        // Get source normal, diffuse, and specular
        if( fvfOld.bNormal )   memcpy( &vNormal,    pSrcVertices+fvfOld.dwNormalOffset, sizeof(D3DXVECTOR3) );
        if( fvfOld.bDiffuse )  memcpy( &dwDiffuse,  pSrcVertices+fvfOld.dwDiffuseOffset, sizeof(DWORD) );
        if( fvfOld.bSpecular ) memcpy( &dwSpecular, pSrcVertices+fvfOld.dwSpecularOffset, sizeof(DWORD) );

        // Get source texture coords
        memcpy( &vT0, pSrcVertices+fvfOld.dwTexCoordsOffset[0], fvfOld.dwNumTexCoords[0] * sizeof(FLOAT) );
        memcpy( &vT1, pSrcVertices+fvfOld.dwTexCoordsOffset[1], fvfOld.dwNumTexCoords[1] * sizeof(FLOAT) );
        memcpy( &vT2, pSrcVertices+fvfOld.dwTexCoordsOffset[2], fvfOld.dwNumTexCoords[2] * sizeof(FLOAT) );
        memcpy( &vT3, pSrcVertices+fvfOld.dwTexCoordsOffset[3], fvfOld.dwNumTexCoords[3] * sizeof(FLOAT) );

        // Write destination position
        if( fvfNew.bPosition )
            memcpy( pDstVertices+fvfNew.dwPositionOffset, &vPos, sizeof(D3DXVECTOR3) );

        // Write destination blend weights
        memcpy( pDstVertices+fvfNew.dwBlendWeightsOffset, &fWeights[0], fvfOld.dwNumBlendWeights * sizeof(FLOAT) );

        // Write destination normal, diffuse, and specular
        if( fvfNew.bNormal )   memcpy( pDstVertices+fvfNew.dwNormalOffset,   &vNormal,    sizeof(D3DXVECTOR3) );
        if( fvfNew.bDiffuse )  memcpy( pDstVertices+fvfNew.dwDiffuseOffset,  &dwDiffuse,  sizeof(DWORD) );
        if( fvfNew.bSpecular ) memcpy( pDstVertices+fvfNew.dwSpecularOffset, &dwSpecular, sizeof(DWORD) );

        // Write destination texture coords
        memcpy( pDstVertices+fvfNew.dwTexCoordsOffset[0], &vT0, fvfNew.dwNumTexCoords[0] * sizeof(FLOAT) );
        memcpy( pDstVertices+fvfNew.dwTexCoordsOffset[1], &vT1, fvfNew.dwNumTexCoords[1] * sizeof(FLOAT) );
        memcpy( pDstVertices+fvfNew.dwTexCoordsOffset[2], &vT2, fvfNew.dwNumTexCoords[2] * sizeof(FLOAT) );
        memcpy( pDstVertices+fvfNew.dwTexCoordsOffset[3], &vT3, fvfNew.dwNumTexCoords[3] * sizeof(FLOAT) );
        
        // Advance to the new vertex
        pSrcVertices += fvfOld.dwVertexSize;
        pDstVertices += fvfNew.dwVertexSize;
    }
    
    pOldVB->Unlock();
    (*ppNewVB)->Unlock();

    return S_OK;
}

