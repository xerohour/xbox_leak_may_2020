//-----------------------------------------------------------------------------
// File: mload.cpp
//
// Copyright (c) 1999-2000 Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#include <xtl.h>
#include <XBApp.h>
#include <XBFont.h>
#include <XBResource.h>
#include <XBUtil.h>
#include "rmxfguid.h"
#include "rmxftmpl.h"
#include "SkinnedMesh.h"


CXBPackedResource* g_pResource = NULL;



CSkinnedMesh::CSkinnedMesh()
{
    m_pmcSelectedMesh = NULL;
    m_pframeSelected  = NULL;
    m_pdeHead         = NULL;
    m_pdeSelected     = NULL;
    m_dwFVF           = D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_NORMAL | D3DFVF_TEX1;
    m_method          = SKIN_NONINDEXED;
}



HRESULT CalculateSum( SFrame* pFrame, D3DXMATRIX* pmatCur, D3DXVECTOR3* pvCenter, 
                      UINT* pdwNumVertices )
{
    HRESULT         hr;
    PBYTE           pbPoints = NULL;
    UINT            cVerticesLocal = 0;
    PBYTE           pbCur;
    D3DXVECTOR3*    pvCur;
    D3DXVECTOR3     vTransformedCur;
    UINT            iPoint;
    UINT            cVertices;
    D3DXMATRIX      matLocal;
    
    D3DXMatrixMultiply( &matLocal, &pFrame->m_matRot, pmatCur );
    
    SMeshContainer* pmcCur = pFrame->m_pMeshContainer;
    while( pmcCur != NULL )
    {
        DWORD fvfsize = D3DXGetFVFVertexSize(pmcCur->m_pMesh->GetFVF());
        
        cVertices = pmcCur->m_pMesh->GetNumVertices();
        
        hr = pmcCur->m_pMesh->LockVertexBuffer( 0, &pbPoints );
        if( FAILED(hr) )
			return hr;
        
        for( iPoint=0, pbCur = pbPoints; iPoint < cVertices; iPoint++, pbCur += fvfsize )
        {
            pvCur = (D3DXVECTOR3*)pbCur;
            
            if( (pvCur->x != 0.0) || (pvCur->y != 0.0) || (pvCur->z != 0.0) )
            {
                cVerticesLocal++;
                
                D3DXVec3TransformCoord(&vTransformedCur, pvCur, &matLocal);
                
                pvCenter->x += vTransformedCur.x;
                pvCenter->y += vTransformedCur.y;
                pvCenter->z += vTransformedCur.z;
            }
        }
        
        pmcCur->m_pMesh->UnlockVertexBuffer();
        pbPoints = NULL;
        
        pmcCur = pmcCur->m_pNext;
    }
    
    (*pdwNumVertices) += cVerticesLocal;
    
    SFrame* pCurrentFrame = pFrame->m_pChild;
    while( pCurrentFrame )
    {
        hr = CalculateSum( pCurrentFrame, &matLocal, pvCenter, pdwNumVertices );
        if( FAILED(hr) )
			return hr;
        
        pCurrentFrame = pCurrentFrame->m_pSibling;
    }
    
    return S_OK;
}




HRESULT CalculateRadius( SFrame* pFrame, D3DXMATRIX* pmatCur, D3DXVECTOR3* pvCenter, 
                         float* pfRadiusSq )
{
    HRESULT         hr = S_OK;
    PBYTE           pbPoints = NULL;
    PBYTE           pbCur;
    D3DXVECTOR3*    pvCur;
    D3DXVECTOR3     vDist;
    UINT            iPoint;
    UINT            dwNumVertices;
    float           fRadiusLocalSq;
    float           fDistSq;
    D3DXMATRIX      matLocal;
    
    D3DXMatrixMultiply( &matLocal, &pFrame->m_matRot, pmatCur );
    
    SMeshContainer* pmcCur = pFrame->m_pMeshContainer;
    fRadiusLocalSq = (*pfRadiusSq);
    while( pmcCur != NULL )
    {
        DWORD fvfsize = D3DXGetFVFVertexSize( pmcCur->m_pMesh->GetFVF() );
        
        dwNumVertices = pmcCur->m_pMesh->GetNumVertices();
        
        hr = pmcCur->m_pMesh->LockVertexBuffer( 0, &pbPoints );
        if( FAILED(hr) )
			return hr;
        
        for( iPoint=0, pbCur = pbPoints; iPoint < dwNumVertices; iPoint++, pbCur += fvfsize )
        {
            pvCur = (D3DXVECTOR3*)pbCur;
            
            if ((pvCur->x == 0.0) && (pvCur->y == 0.0) && (pvCur->z == 0.0))
                continue;
            
            D3DXVec3TransformCoord( &vDist, pvCur, &matLocal );
            
            vDist -= (*pvCenter);
            
            fDistSq = D3DXVec3LengthSq( &vDist );
            
            if( fDistSq > fRadiusLocalSq )
                fRadiusLocalSq = fDistSq;
        }
        
        
        pmcCur->m_pMesh->UnlockVertexBuffer();
        pbPoints = NULL;
        
        pmcCur = pmcCur->m_pNext;
    }
    
    (*pfRadiusSq) = fRadiusLocalSq;
    
    SFrame* pCurrentFrame = pFrame->m_pChild;
    while( pCurrentFrame != NULL )
    {
        hr = CalculateRadius( pCurrentFrame, &matLocal, pvCenter, pfRadiusSq );
        if( FAILED(hr) )
            return hr;
        
        pCurrentFrame = pCurrentFrame->m_pSibling;
    }
    
    return S_OK;
}




HRESULT CalculateBoundingSphere( SDrawElement* pDrawElement )
{
    D3DXVECTOR3 vCenter(0,0,0);
    UINT        dwNumVertices = 0;
    float       fRadiusSq     = 0.0f;
    D3DXMATRIX  matCur;
    HRESULT     hr;
    
    D3DXMatrixIdentity( &matCur );
    hr = CalculateSum( pDrawElement->m_pRootFrame, &matCur, &vCenter, &dwNumVertices );
    if( FAILED(hr) )
	    return hr;
    
    if( dwNumVertices > 0 )
    {
        vCenter /= (float)dwNumVertices;
        
        D3DXMatrixIdentity( &matCur );
        hr = CalculateRadius( pDrawElement->m_pRootFrame, &matCur, &vCenter, &fRadiusSq );
        if( FAILED(hr) )
			return hr;
    }
    
    pDrawElement->m_fRadius = sqrtf( fRadiusSq );
    pDrawElement->m_vCenter = vCenter;
    return S_OK;
}




HRESULT CSkinnedMesh::FindBones( SFrame* pCurrentFrame, SDrawElement* pDrawElement )
{
    HRESULT hr;
    
    SMeshContainer* pMeshContainer = pCurrentFrame->m_pMeshContainer;
    while( pMeshContainer )
    {
        if( pMeshContainer->m_pSkinMesh )
        {
            char** pBoneName = static_cast<char**>(pMeshContainer->m_pBoneNamesBuf->GetBufferPointer());
            for( DWORD i = 0; i < pMeshContainer->m_pSkinMesh->GetNumBones(); i++ )
            {
                SFrame* pFrame = pDrawElement->FindFrame(pBoneName[i]);
                pMeshContainer->m_pBoneMatrix[i] = &(pFrame->m_matCombined);
            }
        }
        pMeshContainer = pMeshContainer->m_pNext;
    }
    
    SFrame* pChildFrame = pCurrentFrame->m_pChild;
    while( pChildFrame )
    {
        hr = FindBones( pChildFrame, pDrawElement );
        if( FAILED(hr) )
            return hr;
        
        pChildFrame = pChildFrame->m_pSibling;
    }
    
    return S_OK;
}




HRESULT CSkinnedMesh::LoadMeshHierarchy( CHAR* strMesh, CXBPackedResource* pResource )
{
    SDrawElement*           pdeMesh    = NULL;
    HRESULT                 hr         = S_OK;
    LPDIRECTXFILE           pxofapi    = NULL;
    LPDIRECTXFILEENUMOBJECT pxofenum   = NULL;
    LPDIRECTXFILEDATA       pxofobjCur = NULL;
    DWORD                   dwOptions;
    int                     cchFileName;

	g_pResource = pResource;

    if( strMesh == NULL )
        return E_INVALIDARG;
    
    pdeMesh = new SDrawElement();
    
    delete pdeMesh->m_pRootFrame;
    pdeMesh->m_pframeAnimHead = NULL;
    
    pdeMesh->m_pRootFrame = new SFrame();
    if( pdeMesh->m_pRootFrame == NULL )
    {
        hr = E_OUTOFMEMORY;
        goto e_Exit;
    }
        
    dwOptions = 0;
    
    cchFileName = strlen( strMesh );
    if( cchFileName < 2 )
    {
        hr = E_FAIL;
        goto e_Exit;
    }
    
    hr = DirectXFileCreate( &pxofapi );
    if( FAILED(hr) )
        goto e_Exit;
    
    // Register templates for d3drm.
    hr = pxofapi->RegisterTemplates( (VOID*)D3DRM_XTEMPLATES, D3DRM_XTEMPLATE_BYTES );
    if( FAILED(hr) )
        goto e_Exit;
    
    // Create enum object.
    hr = pxofapi->CreateEnumObject( (VOID*)strMesh, DXFILELOAD_FROMFILE, &pxofenum );
    if( FAILED(hr) )
        goto e_Exit;
        
    // Enumerate top level objects.
    // Top level objects are always data object.
    while( SUCCEEDED( pxofenum->GetNextDataObject( &pxofobjCur ) ) )
    {
        hr = LoadFrames( pxofobjCur, pdeMesh, dwOptions, m_dwFVF, pdeMesh->m_pRootFrame );
        SAFE_RELEASE( pxofobjCur );
        
        if( FAILED(hr) )
            goto e_Exit;
    }
    
    hr = FindBones(pdeMesh->m_pRootFrame, pdeMesh);
    if( FAILED(hr) )
        goto e_Exit;
    
    delete []pdeMesh->m_strName;
    pdeMesh->m_strName = new char[cchFileName+1];
    if (pdeMesh->m_strName == NULL)
    {
        hr = E_OUTOFMEMORY;
        goto e_Exit;
    }
    memcpy(pdeMesh->m_strName, strMesh, cchFileName+1);

    // delete the current mesh, now that the load has succeeded
    DeleteSelectedMesh();

    // link into the draw list
    pdeMesh->m_pNext = m_pdeHead;
    m_pdeHead = pdeMesh;
    
    m_pdeSelected = pdeMesh;
    m_pmcSelectedMesh = pdeMesh->m_pRootFrame->m_pMeshContainer;
    
    
    m_pframeSelected = pdeMesh->m_pRootFrame;
    
    hr = CalculateBoundingSphere(pdeMesh);
    if( FAILED(hr) )
        goto e_Exit;
    
    m_pdeSelected->m_fCurrentTime = 0.0f;
    m_pdeSelected->m_fMaxTime     = 200.0f;
    
    D3DXMatrixTranslation( &m_pdeSelected->m_pRootFrame->m_matRot,
                           -pdeMesh->m_vCenter.x, 
						   -pdeMesh->m_vCenter.y, 
						   -pdeMesh->m_vCenter.z );
    m_pdeSelected->m_pRootFrame->m_matRotOrig = m_pdeSelected->m_pRootFrame->m_matRot;
    
e_Exit:
    SAFE_RELEASE( pxofobjCur );
    SAFE_RELEASE( pxofenum );
    SAFE_RELEASE( pxofapi );
    
    if( FAILED(hr) )
        delete pdeMesh;
    
    return hr;
}




HRESULT CSkinnedMesh::LoadAnimation( LPDIRECTXFILEDATA pxofobjCur, SDrawElement* pDrawElement,
                                     DWORD dwOptions, DWORD dwFVF, SFrame* pParentFrame )
{
    SRotateKeyXFile*    pFileRotateKey;
    SScaleKeyXFile*     pFileScaleKey;
    SPositionKeyXFile*  pFilePosKey;
    SMatrixKeyXFile*    pFileMatrixKey;
    SFrame*             pCurrentFrame;
    LPDIRECTXFILEDATA   pxofobjChild = NULL;
    LPDIRECTXFILEOBJECT pxofChild = NULL;
    LPDIRECTXFILEDATAREFERENCE pxofobjChildRef = NULL;
    const GUID*         type;
    DWORD   dwSize;
    PBYTE   pData;
    DWORD   dwKeyType;
    DWORD   dwNumKeys;
    DWORD   iKey;
    DWORD   cchName;
    char*   strFrameName;
    HRESULT hr = S_OK;
    
    pCurrentFrame = new SFrame();
    if( pCurrentFrame == NULL )
    {
        hr = E_OUTOFMEMORY;
        goto e_Exit;
    }
    pCurrentFrame->m_bAnimationFrame = true;
    
    pParentFrame->AddFrame( pCurrentFrame );
    pDrawElement->AddAnimationFrame( pCurrentFrame );
    
    // Enumerate child objects.
    // Child object can be data, data reference or binary.
    // Use QueryInterface() to find what type of object a child is.
    while( SUCCEEDED(pxofobjCur->GetNextObject( &pxofChild ) ) )
    {
        // Query the child for it's FileDataReference
        hr = pxofChild->QueryInterface( IID_IDirectXFileDataReference,
                                        (VOID**)&pxofobjChildRef );
        if (SUCCEEDED(hr))
        {
            hr = pxofobjChildRef->Resolve( &pxofobjChild );
            if( SUCCEEDED(hr) )
            {
                hr = pxofobjChild->GetType( &type );
                if( FAILED(hr) )
                    goto e_Exit;
                
                if( TID_D3DRMFrame == (*type) )
                {
                    if( pCurrentFrame->m_pframeToAnimate != NULL )
                    {
                        hr = E_INVALIDARG;
                        goto e_Exit;
                    }
                    
                    hr = pxofobjChild->GetName( NULL, &cchName );
                    if( FAILED(hr) )
                        goto e_Exit;
                    
                    if( cchName == 0 )
                    {
                        hr = E_INVALIDARG;
                        goto e_Exit;
                        
                    }
                    
                    strFrameName = (char*)_alloca( cchName );
                    if( strFrameName == NULL )
                    {
                        hr = E_OUTOFMEMORY;
                        goto e_Exit;
                    }
                    
                    hr = pxofobjChild->GetName( strFrameName, &cchName );
                    if( FAILED(hr) )
                        goto e_Exit;
                    
                    pCurrentFrame->m_pframeToAnimate = pDrawElement->FindFrame( strFrameName );
                    if( pCurrentFrame->m_pframeToAnimate == NULL )
                    {
                        hr = E_INVALIDARG;
                        goto e_Exit;
                    }
                }
                
                SAFE_RELEASE( pxofobjChild );
            }
            
            SAFE_RELEASE( pxofobjChildRef );
        }
        else
        {
            // Query the child for it's FileData
            hr = pxofChild->QueryInterface( IID_IDirectXFileData,
                                            (VOID**)&pxofobjChild );
            if( SUCCEEDED(hr) )
            {
                hr = pxofobjChild->GetType( &type );
                if( FAILED(hr) )
                    goto e_Exit;
                
                if( TID_D3DRMFrame == (*type) )
                {
                    hr = LoadFrames( pxofobjChild, pDrawElement, dwOptions, dwFVF, pCurrentFrame );
                    if( FAILED(hr) )
                        goto e_Exit;
                }
                else if( TID_D3DRMAnimationOptions == (*type) )
                {
                    //ParseAnimOptions(pChildData,pParentFrame);
                    //i=2;
                }
                else if( TID_D3DRMAnimationKey == (*type) )
                {
                    hr = pxofobjChild->GetData( NULL, &dwSize, (VOID**)&pData );
                    if( FAILED(hr) )
                        goto e_Exit;
                    
                    dwKeyType = ((DWORD*)pData)[0];
                    dwNumKeys = ((DWORD*)pData)[1];
                    
                    if( dwKeyType == 0 )
                    {
                        if( pCurrentFrame->m_pRotateKeys != NULL )
                        {
                            hr = E_INVALIDARG;
                            goto e_Exit;
                        }
                        
                        pCurrentFrame->m_pRotateKeys = new SRotateKey[dwNumKeys];
                        if (pCurrentFrame->m_pRotateKeys == NULL)
                        {
                            hr = E_OUTOFMEMORY;
                            goto e_Exit;
                        }
                        
                        pCurrentFrame->m_dwNumRotateKeys = dwNumKeys;
                        //NOTE x files are w x y z and QUATERNIONS are x y z w
                        
                        pFileRotateKey =  (SRotateKeyXFile*)(pData + (sizeof(DWORD) * 2));
                        for( iKey = 0;iKey < dwNumKeys; iKey++ )
                        {
                            pCurrentFrame->m_pRotateKeys[iKey].dwTime = pFileRotateKey->dwTime;
                            pCurrentFrame->m_pRotateKeys[iKey].quatRotate.x = pFileRotateKey->x;
                            pCurrentFrame->m_pRotateKeys[iKey].quatRotate.y = pFileRotateKey->y;
                            pCurrentFrame->m_pRotateKeys[iKey].quatRotate.z = pFileRotateKey->z;
                            pCurrentFrame->m_pRotateKeys[iKey].quatRotate.w = pFileRotateKey->w;
                            
                            pFileRotateKey += 1;
                        }
                    }
                    else if( dwKeyType == 1 )
                    {
                        if (pCurrentFrame->m_pScaleKeys != NULL)
                        {
                            hr = E_INVALIDARG;
                            goto e_Exit;
                        }
                        
                        pCurrentFrame->m_pScaleKeys = new SScaleKey[dwNumKeys];
                        if (pCurrentFrame->m_pScaleKeys == NULL)
                        {
                            hr = E_OUTOFMEMORY;
                            goto e_Exit;
                        }
                        
                        pCurrentFrame->m_dwNumScaleKeys = dwNumKeys;
                        
                        pFileScaleKey =  (SScaleKeyXFile*)(pData + (sizeof(DWORD) * 2));
                        for( iKey = 0;iKey < dwNumKeys; iKey++ )
                        {
                            pCurrentFrame->m_pScaleKeys[iKey].dwTime = pFileScaleKey->dwTime;
                            pCurrentFrame->m_pScaleKeys[iKey].vScale = pFileScaleKey->vScale;
                            
                            pFileScaleKey += 1;
                        }
                    }
                    else if( dwKeyType == 2 )
                    {
                        if (pCurrentFrame->m_pPositionKeys != NULL)
                        {
                            hr = E_INVALIDARG;
                            goto e_Exit;
                        }
                        
                        pCurrentFrame->m_pPositionKeys = new SPositionKey[dwNumKeys];
                        if (pCurrentFrame->m_pPositionKeys == NULL)
                        {
                            hr = E_OUTOFMEMORY;
                            goto e_Exit;
                        }
                        
                        pCurrentFrame->m_dwNumPositionKeys = dwNumKeys;
                        
                        pFilePosKey =  (SPositionKeyXFile*)(pData + (sizeof(DWORD) * 2));
                        for( iKey = 0;iKey < dwNumKeys; iKey++ )
                        {
                            pCurrentFrame->m_pPositionKeys[iKey].dwTime = pFilePosKey->dwTime;
                            pCurrentFrame->m_pPositionKeys[iKey].vPos = pFilePosKey->vPos;
                            
                            pFilePosKey += 1;
                        }
                    }
                    else if( dwKeyType == 4 )
                    {
                        if( pCurrentFrame->m_pMatrixKeys != NULL )
                        {
                            hr = E_INVALIDARG;
                            goto e_Exit;
                        }
                        
                        pCurrentFrame->m_pMatrixKeys = new SMatrixKey[dwNumKeys];
                        if( pCurrentFrame->m_pMatrixKeys == NULL )
                        {
                            hr = E_OUTOFMEMORY;
                            goto e_Exit;
                        }
                        
                        pCurrentFrame->m_dwNumMatrixKeys = dwNumKeys;
                        
                        pFileMatrixKey = (SMatrixKeyXFile*)(pData + (sizeof(DWORD) * 2));
                        for( iKey = 0;iKey < dwNumKeys; iKey++ )
                        {
                            pCurrentFrame->m_pMatrixKeys[iKey].dwTime = pFileMatrixKey->dwTime;
                            memcpy( &pCurrentFrame->m_pMatrixKeys[iKey].mat, &pFileMatrixKey->mat, sizeof(D3DXMATRIX) );
                            pFileMatrixKey++;
                        }
                    }
                    else
                    {
                        hr = E_INVALIDARG;
                        goto e_Exit;
                    }
                }
                
                SAFE_RELEASE(pxofobjChild);
            }
        }
        
        SAFE_RELEASE(pxofChild);
    }
    
e_Exit:
    SAFE_RELEASE(pxofobjChild);
    SAFE_RELEASE(pxofChild);
    SAFE_RELEASE(pxofobjChildRef);
    return hr;
}




HRESULT CSkinnedMesh::LoadAnimationSet( LPDIRECTXFILEDATA pxofobjCur, SDrawElement* pDrawElement,
                                        DWORD dwOptions, DWORD dwFVF, SFrame* pParentFrame )
{
    SFrame* pCurrentFrame;
    const GUID* type;
    HRESULT hr = S_OK;
    LPDIRECTXFILEDATA pxofobjChild = NULL;
    LPDIRECTXFILEOBJECT pxofChild = NULL;
    DWORD cchName;
    
    pCurrentFrame = new SFrame();
    if( pCurrentFrame == NULL )
    {
        hr = E_OUTOFMEMORY;
        goto e_Exit;
    }
    pCurrentFrame->m_bAnimationFrame = true;
    
    pParentFrame->AddFrame( pCurrentFrame );
    
    hr = pxofobjCur->GetName( NULL, &cchName );
    if( FAILED(hr) )
        goto e_Exit;
    
    if (cchName > 0)
    {
        pCurrentFrame->m_strName = new char[cchName];
        if( pCurrentFrame->m_strName == NULL )
        {
            hr = E_OUTOFMEMORY;
            goto e_Exit;
        }
        
        hr = pxofobjCur->GetName( pCurrentFrame->m_strName, &cchName );
        if( FAILED(hr) )
            goto e_Exit;
    }
    
    // Enumerate child objects.
    // Child object can be data, data reference or binary.
    // Use QueryInterface() to find what type of object a child is.
    while( SUCCEEDED( pxofobjCur->GetNextObject( &pxofChild ) ) )
    {
        // Query the child for it's FileData
        hr = pxofChild->QueryInterface( IID_IDirectXFileData,
                                        (VOID**)&pxofobjChild );
        if (SUCCEEDED(hr))
        {
            hr = pxofobjChild->GetType( &type );
            if( FAILED(hr) )
                goto e_Exit;
            
            if( TID_D3DRMAnimation == (*type) )
            {
                hr = LoadAnimation( pxofobjChild, pDrawElement, dwOptions, dwFVF, pCurrentFrame );
                if( FAILED(hr) )
                    goto e_Exit;
            }
            
            SAFE_RELEASE( pxofobjChild );
        }
        
        SAFE_RELEASE( pxofChild );
    }
    
e_Exit:
    SAFE_RELEASE( pxofobjChild );
    SAFE_RELEASE( pxofChild );
    return hr;
}




HRESULT CSkinnedMesh::GenerateMesh( SMeshContainer* pMeshContainer )
{
    // ASSUMPTION:  pMeshContainer->m_rgiAdjacency contains the current adjacency

    HRESULT hr           = S_OK;
    DWORD   dwNumFaces   = pMeshContainer->m_pSkinMesh->GetNumFaces();
    DWORD*  pAdjacencyIn = new DWORD[pMeshContainer->m_pSkinMesh->GetNumFaces() * 3];

    if( pAdjacencyIn == NULL )
    {
        hr = E_OUTOFMEMORY;
        goto e_Exit;
    }
    
    memcpy( pAdjacencyIn, pMeshContainer->m_rgiAdjacency, dwNumFaces * 3 * sizeof(DWORD) );

    SAFE_RELEASE( pMeshContainer->m_pMesh );
    pMeshContainer->m_pMesh      = NULL;
    
    if( m_method == SKIN_NONINDEXED )
    {
        DWORD* rgiAdjacency = NULL;

        rgiAdjacency = new DWORD[dwNumFaces * 3];

        if( rgiAdjacency == NULL )
        {
            hr  = E_OUTOFMEMORY;
            goto e_ExitNONINDEXED;
        }

        hr = pMeshContainer->m_pSkinMesh->ConvertToBlendedMesh( 0, pAdjacencyIn, rgiAdjacency, 
                                                                &pMeshContainer->m_cpattr, 
																&pMeshContainer->m_pBoneCombinationBuf, 
																&pMeshContainer->m_pMesh );
        if( FAILED(hr) )
            goto e_ExitNONINDEXED;

        // Calculate the max face influence count
        if( (pMeshContainer->m_pMesh->GetFVF() & D3DFVF_POSITION_MASK) != D3DFVF_XYZ )
        {
            pMeshContainer->m_maxFaceInfl = 1 + ((pMeshContainer->m_pMesh->GetFVF() & D3DFVF_POSITION_MASK) - D3DFVF_XYZRHW) / 2;
        }
        else
        {
            pMeshContainer->m_maxFaceInfl = 1;
        }

        // Vertex cache optimize the mesh
        LPD3DXMESH pMeshOpt;
        hr = pMeshContainer->m_pMesh->Optimize( D3DXMESHOPT_VERTEXCACHE, rgiAdjacency,
			                                    NULL, NULL, NULL, &pMeshOpt );
        if( SUCCEEDED(hr) )
        {
			pMeshContainer->m_pMesh->Release();
			pMeshContainer->m_pMesh = pMeshOpt;
			pMeshOpt = NULL;
        }

        // Need to clone the mesh to be WRITEONLY since we will not now read back from it.
 		LPD3DXMESH pMeshVid;
		hr = pMeshContainer->m_pMesh->CloneMeshFVF( pMeshContainer->m_pMesh->GetOptions() | D3DXMESH_WRITEONLY, pMeshContainer->m_pMesh->GetFVF(),
										            m_pd3dDevice, &pMeshVid );
		if( SUCCEEDED(hr) )
		{
			pMeshContainer->m_pMesh->Release();
			pMeshContainer->m_pMesh = pMeshVid;
			pMeshVid = NULL;
		}

e_ExitNONINDEXED:
        SAFE_DELETE_ARRAY( rgiAdjacency );

        if( FAILED(hr) )
            goto e_Exit;
    }
    else if( m_method == SKIN_INDEXED )
    {
        hr = pMeshContainer->m_pSkinMesh->ConvertToIndexedBlendedMesh( D3DXMESH_SYSTEMMEM, pAdjacencyIn, 255, 
			                                                    NULL, &pMeshContainer->m_cpattr, 
																&pMeshContainer->m_pBoneCombinationBuf, 
																&pMeshContainer->m_pMesh );
        if( FAILED(hr) )
            goto e_Exit;

        // Here we are talking of max vertex influence which we determine from 
        // the FVF of the returned mesh
        if( (pMeshContainer->m_pMesh->GetFVF() & D3DFVF_POSITION_MASK) != D3DFVF_XYZ )
        {
            pMeshContainer->m_maxFaceInfl = ((pMeshContainer->m_pMesh->GetFVF() & D3DFVF_POSITION_MASK) - D3DFVF_XYZRHW) / 2;
        }
        else
        {
            pMeshContainer->m_maxFaceInfl = 1;
        }
    }
    pMeshContainer->m_Method = m_method;

e_Exit:
    delete[] pAdjacencyIn;
    return hr;
}




HRESULT CSkinnedMesh::LoadMesh( LPDIRECTXFILEDATA pxofobjCur, DWORD dwOptions, DWORD dwFVF, 
							    SFrame* pParentFrame )
{
    SMeshContainer* pMeshContainer = NULL;
    LPD3DXBUFFER    pbufMaterials  = NULL;
    LPD3DXBUFFER    pbufAdjacency  = NULL;
    DWORD   cchName;
    UINT    dwNumFaces;
    UINT    iMaterial;
    HRESULT hr = S_OK;
    
    pMeshContainer = new SMeshContainer();
    if( pMeshContainer == NULL )
    {
        hr = E_OUTOFMEMORY;
        goto e_Exit;
    }
    
    hr = pxofobjCur->GetName( NULL, &cchName );
    if( FAILED(hr) )
        goto e_Exit;
    
    if( cchName > 0 )
    {
        pMeshContainer->m_strName = new char[cchName];
        if( pMeshContainer->m_strName == NULL )
        {
            hr = E_OUTOFMEMORY;
            goto e_Exit;
        }
        
        hr = pxofobjCur->GetName( pMeshContainer->m_strName, &cchName );
        if( FAILED(hr) )
            goto e_Exit;
    }
    
    hr = D3DXLoadSkinMeshFromXof( pxofobjCur, dwOptions, m_pd3dDevice, &pbufAdjacency, 
		                          &pbufMaterials, &pMeshContainer->m_dwNumMaterials, 
								  &pMeshContainer->m_pBoneNamesBuf, 
								  &pMeshContainer->m_pBoneOffsetBuf, 
								  &pMeshContainer->m_pSkinMesh );
    if( FAILED(hr) )
        goto e_Exit;
    
    dwNumFaces = pMeshContainer->m_pSkinMesh->GetNumFaces();

    // Process skinning data
    if( pMeshContainer->m_pSkinMesh->GetNumBones() )
    {
        pMeshContainer->m_pBoneMatrix = new D3DXMATRIX*[pMeshContainer->m_pSkinMesh->GetNumBones()];
        if( pMeshContainer->m_pBoneMatrix == NULL )
            goto e_Exit;
        pMeshContainer->m_pBoneOffsetMat = reinterpret_cast<D3DXMATRIX*>(pMeshContainer->m_pBoneOffsetBuf->GetBufferPointer());
        DWORD* pAdjacencyIn = static_cast<LPDWORD>(pbufAdjacency->GetBufferPointer());

        pMeshContainer->m_rgiAdjacency = new DWORD[dwNumFaces * 3];

        if( pMeshContainer->m_rgiAdjacency == NULL )
        {
            hr = E_OUTOFMEMORY;
            goto e_Exit;
        }

        memcpy( pMeshContainer->m_rgiAdjacency, pAdjacencyIn, dwNumFaces * 3 * sizeof(DWORD) );
        
        hr = GenerateMesh( pMeshContainer );
        if( FAILED(hr) )
            goto e_Exit;
    }
    else
    {
        pMeshContainer->m_pSkinMesh->GetOriginalMesh(&(pMeshContainer->m_pMesh));
        pMeshContainer->m_pSkinMesh->Release();
        pMeshContainer->m_pSkinMesh = NULL;
        pMeshContainer->m_cpattr    = pMeshContainer->m_dwNumMaterials;
    }
    
    if( (pbufMaterials == NULL) || (pMeshContainer->m_dwNumMaterials == 0) )
    {
        pMeshContainer->m_pMaterials = new D3DMATERIAL8[1];
        pMeshContainer->m_pTextures  = new LPDIRECT3DTEXTURE8[1];
        if( pMeshContainer->m_pMaterials == NULL || pMeshContainer->m_pTextures == NULL )
        {
            hr = E_OUTOFMEMORY;
            goto e_Exit;
        }
        
        memset(pMeshContainer->m_pMaterials, 0, sizeof(D3DXMATERIAL));
        pMeshContainer->m_pMaterials[0].Diffuse.r = 0.5f;
        pMeshContainer->m_pMaterials[0].Diffuse.g = 0.5f;
        pMeshContainer->m_pMaterials[0].Diffuse.b = 0.5f;
        pMeshContainer->m_pMaterials[0].Specular  = pMeshContainer->m_pMaterials[0].Diffuse;
        pMeshContainer->m_pTextures[0]            = NULL;
    }
    else
    {
        pMeshContainer->m_pMaterials = new D3DMATERIAL8[pMeshContainer->m_dwNumMaterials];
        pMeshContainer->m_pTextures  = new LPDIRECT3DTEXTURE8[pMeshContainer->m_dwNumMaterials];
        if( pMeshContainer->m_pMaterials == NULL || pMeshContainer->m_pTextures == NULL )
        {
            hr = E_OUTOFMEMORY;
            goto e_Exit;
        }
        
        LPD3DXMATERIAL pMaterials = (LPD3DXMATERIAL)pbufMaterials->GetBufferPointer();
        
        for( iMaterial = 0; iMaterial < pMeshContainer->m_dwNumMaterials; iMaterial++ )
        {
            pMeshContainer->m_pMaterials[iMaterial] = pMaterials[iMaterial].MatD3D;
            pMeshContainer->m_pTextures[iMaterial]  = NULL;

            if( g_pResource && pMaterials[iMaterial].pTextureFilename )
				pMeshContainer->m_pTextures[iMaterial] = g_pResource->GetTexture( pMaterials[iMaterial].pTextureFilename );
        }
    }
    
    // Add the mesh to the parent frame
    pParentFrame->AddMesh( pMeshContainer );
    pMeshContainer = NULL;
    
e_Exit:
    delete pMeshContainer;
    
    SAFE_RELEASE( pbufAdjacency );
    SAFE_RELEASE( pbufMaterials );

    return hr;
}




HRESULT CSkinnedMesh::LoadFrames( LPDIRECTXFILEDATA pxofobjCur, SDrawElement* pDrawElement,
                                  DWORD dwOptions, DWORD dwFVF, SFrame* pParentFrame )
{
    HRESULT hr = S_OK;
    LPDIRECTXFILEDATA pxofobjChild = NULL;
    LPDIRECTXFILEOBJECT pxofChild = NULL;
    const GUID* type;
    DWORD cbSize;
    D3DXMATRIX* pmatNew;
    SFrame* pCurrentFrame;
    DWORD cchName;
    
    // Get the type of the object
    hr = pxofobjCur->GetType(&type);
    if( FAILED(hr) )
        goto e_Exit;
       
    if (*type == TID_D3DRMMesh)
    {
        hr = LoadMesh( pxofobjCur, dwOptions, dwFVF, pParentFrame );
        if( FAILED(hr) )
            goto e_Exit;
    }
    else if (*type == TID_D3DRMFrameTransformMatrix)
    {
        hr = pxofobjCur->GetData( NULL, &cbSize, (VOID**)&pmatNew );
        if( FAILED(hr) )
            goto e_Exit;
        
        // update the parents matrix with the new one
        pParentFrame->m_matRot     = (*pmatNew);
        pParentFrame->m_matRotOrig = (*pmatNew);
    }
    else if( *type == TID_D3DRMAnimationSet )
    {
        LoadAnimationSet( pxofobjCur, pDrawElement, dwOptions, dwFVF, pParentFrame );
    }
    else if( *type == TID_D3DRMAnimation )
    {
        LoadAnimation( pxofobjCur, pDrawElement, dwOptions, dwFVF, pParentFrame );
    }
    else if( *type == TID_D3DRMFrame )
    {
        pCurrentFrame = new SFrame();
        if( pCurrentFrame == NULL )
        {
            hr = E_OUTOFMEMORY;
            goto e_Exit;
        }
        
        hr = pxofobjCur->GetName( NULL, &cchName );
        if( FAILED(hr) )
            goto e_Exit;
        
        if (cchName > 0)
        {
            pCurrentFrame->m_strName = new char[cchName];
            if (pCurrentFrame->m_strName == NULL)
            {
                hr = E_OUTOFMEMORY;
                goto e_Exit;
            }
            
            hr = pxofobjCur->GetName( pCurrentFrame->m_strName, &cchName );
            if( FAILED(hr) )
                goto e_Exit;
        }
        
        pParentFrame->AddFrame( pCurrentFrame );
        
        // Enumerate child objects.
        // Child object can be data, data reference or binary.
        // Use QueryInterface() to find what type of object a child is.
        while( SUCCEEDED(pxofobjCur->GetNextObject( &pxofChild ) ) )
        {
            // Query the child for it's FileData
            hr = pxofChild->QueryInterface( IID_IDirectXFileData, (VOID**)&pxofobjChild );
            if( SUCCEEDED(hr) )
            {
                hr = LoadFrames( pxofobjChild, pDrawElement, dwOptions, dwFVF, pCurrentFrame );
                if( FAILED(hr) )
                    goto e_Exit;
                
                SAFE_RELEASE( pxofobjChild );
            }
            SAFE_RELEASE( pxofChild );
        }
    }
    
e_Exit:
    SAFE_RELEASE( pxofobjChild );
    SAFE_RELEASE( pxofChild );
    return hr;
}


         
                                      
HRESULT CSkinnedMesh::DeleteSelectedMesh()
{
    if( m_pdeSelected )
    {
        SDrawElement* pCurrent = m_pdeHead;
        SDrawElement* pPrev    = NULL;
        
		while( (pCurrent) && (pCurrent!=m_pdeSelected) )
        {
            pPrev    = pCurrent;
            pCurrent = pCurrent->m_pNext;
        }

        if( pPrev == NULL )
        {
            m_pdeHead = m_pdeHead->m_pNext;
        }
        else
        {
            pPrev->m_pNext = pCurrent->m_pNext;
        }

        m_pdeSelected->m_pNext = NULL;
        if( m_pdeHead == m_pdeSelected )
            m_pdeHead = NULL;
        delete m_pdeSelected;
        m_pdeSelected = NULL;
    }

    return S_OK;
}
