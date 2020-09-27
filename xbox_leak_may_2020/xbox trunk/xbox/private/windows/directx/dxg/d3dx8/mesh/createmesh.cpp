/*//////////////////////////////////////////////////////////////////////////////
//
// File: createmesh.cpp
//
// Copyright (C) 1999 Microsoft Corporation. All Rights Reserved.
//
// @@BEGIN_MSINTERNAL
//
// History:
// -@-          (craigp)    - created 
// -@- 08/19/99 (mikemarr)  - prepend GX to gxfmath functions
//                          - started comment history
//                          - replace references to gxbasetype.h with gxmathcore.h
// -@- 09/23/99 (mikemarr)  - changed <> to "" on #includes
//
// @@END_MSINTERNAL
//
//////////////////////////////////////////////////////////////////////////////*/

#include "pchmesh.h"

#include "tri3mesh.h"
#include "pmeshhe.h"
#include "simplify.h"
#include "CD3DXBuffer.h"

HRESULT WINAPI D3DXCreateMesh
    (
    DWORD numFaces, 
    DWORD numVertices, 
    DWORD dwOptions, 
    CONST DWORD *pDeclaration, 
    LPDIRECT3DDEVICE8 pD3DDevice, 
    LPD3DXMESH* pptmNewMesh
    )
{
    DWORD dwFVF;
    HRESULT hr;

    hr = D3DXFVFFromDeclarator(pDeclaration, &dwFVF);
    if (FAILED(hr))
    {
        DPF(0,"CloneMesh: Declaration cannot be converted to FVF");
        return hr;
    }

    return D3DXCreateMeshFVF(numFaces, numVertices, dwOptions, dwFVF, pD3DDevice, pptmNewMesh);
}


HRESULT WINAPI D3DXCreateMeshFVF
    (
    DWORD numFaces, 
    DWORD numVertices, 
    DWORD dwOptions, 
    DWORD dwFVF, 
    LPDIRECT3DDEVICE8 pD3DDevice, 
    LPD3DXMESH* pptmNewMesh
    )
{
    HRESULT hr = S_OK;
    GXTri3Mesh<tp32BitIndex> *ptmNewMesh32;
    GXTri3Mesh<tp16BitIndex> *ptmNewMesh16;

    if (((dwOptions & ~D3DXMESH_VALIDBITS) != 0) || (pD3DDevice == NULL) || (pptmNewMesh == NULL))
    {
#if DBG
        if (dwOptions & D3DXMESH_VB_SHARE)
		    DPF(0, "D3DXCreateMesh: VB_SHARE flag only valid on clone mesh calls.\n");
        if ((dwOptions & ~D3DXMESH_VALIDBITS) != 0)
		    DPF(0, "D3DXCreateMesh: Invalid mesh flag specified\n");
        else if (pD3DDevice == NULL)
		    DPF(0, "D3DXCreateMesh: NULL device specified\n");
        else if (pptmNewMesh == NULL)
		    DPF(0, "D3DXCreateMesh: NULL mesh output parameter\n");
#endif
        hr = D3DERR_INVALIDCALL;
        goto e_Exit;
    }

	if ((numFaces == 0) || (numVertices == 0))
	{
		DPF(0, "D3DXCreateMesh: Number of faces and vertices must be greater than zero\n");
		hr = D3DERR_INVALIDCALL;
		goto e_Exit;
	}

    *pptmNewMesh = NULL;

    if (dwOptions & D3DXMESH_32BIT)
    {
        ptmNewMesh32 = new GXTri3Mesh<tp32BitIndex>(pD3DDevice, dwFVF, dwOptions);

        if (ptmNewMesh32 == NULL)
        {
            hr = E_OUTOFMEMORY;
            goto e_Exit;
        }

        hr = ptmNewMesh32->Resize(numFaces, numVertices);
        if (FAILED(hr))
            goto e_Exit;

        *pptmNewMesh = ptmNewMesh32;
    }
    else
    {
        if ((numFaces > UNUSED16) || (numVertices > UNUSED16))
        {
		    DPF(0, "D3DXCreateMesh: Too many faces/vertices for a 16bit mesh\n");

            hr = D3DERR_INVALIDCALL;
            goto e_Exit;
        }

        ptmNewMesh16 = new GXTri3Mesh<tp16BitIndex>(pD3DDevice, dwFVF, dwOptions);
        if (ptmNewMesh16 == NULL)
        {
            hr = E_OUTOFMEMORY;
            goto e_Exit;
        }

        hr = ptmNewMesh16->Resize(numFaces, numVertices);
        if (FAILED(hr))
            goto e_Exit;

        *pptmNewMesh = ptmNewMesh16;
    }

e_Exit:
    return hr;
}

// internal function used to abstract GXHalfEdgePMesh template from loading code
HRESULT WINAPI D3DXCreatePMeshFromData
    (
    LPD3DXMESH ptmMesh,
    DWORD dwOptions,
    DWORD *rgdwAdjacency,
    PBYTE pbPMData, 
    DWORD cbPMData,
    LPD3DXPMESH *pptmPMesh
    )
{
    HRESULT hr = S_OK;
    GXHalfEdgePMesh<tp16BitIndex> *ptmPMesh16 = NULL;
    GXHalfEdgePMesh<tp32BitIndex> *ptmPMesh32 = NULL;
    LPDIRECT3DDEVICE8 pDevice;

    ptmMesh->GetDevice(&pDevice);
    pDevice->Release();  // ptmMesh retains a ref, create will add a new permanent one

    GXASSERT(CheckAdjacency(rgdwAdjacency, ptmMesh->GetNumFaces()));

    if (!(ptmMesh->GetOptions() & D3DXMESH_32BIT))
    {
        ptmPMesh16 = new GXHalfEdgePMesh<tp16BitIndex>(pDevice, ptmMesh->GetFVF(), dwOptions);
        if (ptmPMesh16 == NULL)
        {
            hr = E_OUTOFMEMORY;
            goto e_Exit;
        }

        // fill the pmesh from the data loaded from the file
        hr = ptmPMesh16->LoadFromData(ptmMesh, rgdwAdjacency, pbPMData, cbPMData);
        if (FAILED(hr))
            goto e_Exit;

        *pptmPMesh = ptmPMesh16;
        ptmPMesh16 = NULL;
    }
    else
    {
        ptmPMesh32 = new GXHalfEdgePMesh<tp32BitIndex>(pDevice, ptmMesh->GetFVF(), dwOptions|D3DXMESH_32BIT);
        if (ptmPMesh32 == NULL)
        {
            hr = E_OUTOFMEMORY;
            goto e_Exit;
        }

        // fill the pmesh from the data loaded from the file
        hr = ptmPMesh32->LoadFromData(ptmMesh, rgdwAdjacency, pbPMData, cbPMData);
        if (FAILED(hr))
            goto e_Exit;

        *pptmPMesh = ptmPMesh32;
        ptmPMesh32 = NULL;
    }


e_Exit:
    GXRELEASE(ptmPMesh16);
    GXRELEASE(ptmPMesh32);

    return hr;
}

HRESULT WINAPI D3DXCreateSPMesh
    (
    LPD3DXMESH pMesh, 
    CONST DWORD* adjacency, 
    CONST LPD3DXATTRIBUTEWEIGHTS pVertexAttributeWeights,
    CONST FLOAT *rgfVertexWeights,
    LPD3DXSPMESH* ppsmNewMesh
    )
{
    HRESULT hr = S_OK;
    GXSimplifyMesh<tp16BitIndex> *ptmSimpMesh16 = NULL;
    GXSimplifyMesh<tp32BitIndex> *ptmSimpMesh32 = NULL;
    GXTri3Mesh<tp32BitIndex> *ptmSrcMesh32;
    GXTri3Mesh<tp16BitIndex> *ptmSrcMesh16;
    DWORD dwFVF;
    DWORD dwOptions;
    LPDIRECT3DDEVICE8 pDevice;

    if ((pMesh == NULL) || (adjacency == NULL) || (ppsmNewMesh == NULL))
    {
#if DBG
        if (pMesh == NULL)
		    DPF(0, "D3DXCreateSPMesh: NULL mesh provided\n");
        else if (adjacency == NULL) 
		    DPF(0, "D3DXCreateSPMesh: No adjacency provided\n");
        else if (ppsmNewMesh == NULL)
		    DPF(0, "D3DXCreateSPMesh: NULL output mesh parameter\n");
#endif
        hr = D3DERR_INVALIDCALL;
        goto e_Exit;
    }

#ifdef _DEBUG
	// in debug, make sure the validate method would succeed
	hr = D3DXValidMesh(pMesh, adjacency);
	if (FAILED(hr))
		return hr;
#endif

    // make the simp mesh have the same options as the input mesh
    dwFVF = pMesh->GetFVF();
    dwOptions = pMesh->GetOptions();
    pMesh->GetDevice(&pDevice);
    pDevice->Release();  // ptmMesh retains a ref, create will add a new permanent one

    if (dwOptions & D3DXMESH_32BIT)
    {
        ptmSrcMesh32 = (GXTri3Mesh<tp32BitIndex>*)pMesh;

        ptmSimpMesh32 = new GXSimplifyMesh<tp32BitIndex>(pDevice, dwFVF, dwOptions);
        if (ptmSimpMesh32 == NULL)
        {
            hr = E_OUTOFMEMORY;
            goto e_Exit;
        }

        hr = ptmSimpMesh32->SetupSimplification(ptmSrcMesh32, adjacency, pVertexAttributeWeights, rgfVertexWeights);
        if (FAILED(hr))
            goto e_Exit;

        *ppsmNewMesh = ptmSimpMesh32;
        ptmSimpMesh32 = NULL;
    }
    else
    {
        ptmSrcMesh16 = (GXTri3Mesh<tp16BitIndex>*)pMesh;

        ptmSimpMesh16 = new GXSimplifyMesh<tp16BitIndex>(pDevice, dwFVF, dwOptions);
        if (ptmSimpMesh16 == NULL)
        {
            hr = E_OUTOFMEMORY;
            goto e_Exit;
        }

        hr = ptmSimpMesh16->SetupSimplification(ptmSrcMesh16, adjacency, pVertexAttributeWeights, rgfVertexWeights);
        if (FAILED(hr))
            goto e_Exit;

        *ppsmNewMesh = ptmSimpMesh16;
        ptmSimpMesh16 = NULL;
    }

e_Exit:
    GXRELEASE(ptmSimpMesh16);
//    GXRELEASE(ptmSimpMesh32);
    return hr;
}



HRESULT WINAPI
    D3DXGeneratePMesh(
        LPD3DXMESH pMesh, 
        CONST DWORD* adjacency, 
        CONST LPD3DXATTRIBUTEWEIGHTS pVertexAttributeWeights,
        CONST FLOAT *rgfVertexWeights,
        DWORD minValue, 
        DWORD options, 
        LPD3DXPMESH* pptmNewMesh)
{
    HRESULT hr = S_OK;
    LPD3DXSPMESH ptmSimpMesh = NULL;
    LPDIRECT3DDEVICE8 pDevice;

    if ((pMesh == NULL) || ((options & ~D3DXMESHSIMP_VALIDBITS) != 0) 
        || (adjacency == NULL)
        || (pptmNewMesh == NULL))
    {
#if DBG
        if (pMesh == NULL)
		    DPF(0, "D3DXGeneratePMesh: NULL mesh provided\n");
        else if ((options & ~D3DXMESHSIMP_VALIDBITS) != 0)
		    DPF(0, "D3DXGeneratePMesh: Invalid simplification parameter specified\n");
        else if (adjacency == NULL) 
		    DPF(0, "D3DXGeneratePMesh: No adjacency provided\n");
        else if (pptmNewMesh == NULL)
		    DPF(0, "D3DXGeneratePMesh: NULL output mesh parameter\n");
#endif
        hr = D3DERR_INVALIDCALL;
        goto e_Exit;
    }

    if ((options == 0) || (options ==  (D3DXMESHSIMP_VERTEX | D3DXMESHSIMP_FACE)))
    {
#if DBG
        if (options == 0)
		    DPF(0, "D3DXGeneratePMesh: No simplification option specified\n");
        else if (options ==  (D3DXMESHSIMP_VERTEX | D3DXMESHSIMP_FACE))
		    DPF(0, "D3DXGeneratePMesh: Incompatible simplification options specified\n");
#endif
        hr = D3DERR_INVALIDCALL;
        goto e_Exit;
    }


    hr = D3DXCreateSPMesh(pMesh, adjacency, pVertexAttributeWeights, rgfVertexWeights, &ptmSimpMesh);
    if (FAILED(hr))
        goto e_Exit;

    if (options & D3DXMESHSIMP_VERTEX)
        hr = ptmSimpMesh->ReduceVertices(minValue);
    else 
        hr = ptmSimpMesh->ReduceFaces(minValue);

    if (FAILED(hr))
        goto e_Exit;

    ptmSimpMesh->GetDevice(&pDevice);
    pDevice->Release();  // ptmMesh retains a ref, create will add a new permanent one

    hr = ptmSimpMesh->ClonePMeshFVF(
                ptmSimpMesh->GetOptions(), 
                ptmSimpMesh->GetFVF(),
                pDevice,
                NULL, pptmNewMesh);
    if (FAILED(hr))
        goto e_Exit;

e_Exit:
    GXRELEASE(ptmSimpMesh);
    return hr;
}


HRESULT WINAPI
    D3DXSimplifyMesh(
        LPD3DXMESH pMesh, 
        CONST DWORD* adjacency, 
        CONST LPD3DXATTRIBUTEWEIGHTS pVertexAttributeWeights,
        CONST FLOAT *rgfVertexWeights,
        DWORD minValue, 
        DWORD options, 
        LPD3DXMESH* pptmNewMesh)
{
    HRESULT hr = S_OK;
    LPD3DXSPMESH ptmSimpMesh = NULL;
    LPDIRECT3DDEVICE8 pDevice;

    if ((pMesh == NULL) || ((options & ~D3DXMESHSIMP_VALIDBITS) != 0) 
        || (adjacency == NULL)
        || (pptmNewMesh == NULL))
    {
#if DBG
        if (pMesh == NULL)
		    DPF(0, "D3DXSimplifyMesh: NULL mesh provided\n");
        else if ((options & ~D3DXMESHSIMP_VALIDBITS) != 0)
		    DPF(0, "D3DXSimplifyMesh: Invalid simplification parameter specified\n");
        else if (adjacency == NULL) 
		    DPF(0, "D3DXSimplifyMesh: No adjacency provided\n");
        else if (pptmNewMesh == NULL)
		    DPF(0, "D3DXSimplifyMesh: NULL output mesh parameter\n");
#endif
        hr = D3DERR_INVALIDCALL;
        goto e_Exit;
    }

    if ((options == 0) || (options ==  (D3DXMESHSIMP_VERTEX | D3DXMESHSIMP_FACE)))
    {
#if DBG
        if (options == 0)
		    DPF(0, "D3DXSimplifyMesh: No simplification option specified\n");
        else if (options ==  (D3DXMESHSIMP_VERTEX | D3DXMESHSIMP_FACE))
		    DPF(0, "D3DXSimplifyMesh: Incompatible simplification options specified\n");
#endif
        hr = D3DERR_INVALIDCALL;
        goto e_Exit;
    }


    hr = D3DXCreateSPMesh(pMesh, adjacency, pVertexAttributeWeights, rgfVertexWeights, &ptmSimpMesh);
    if (FAILED(hr))
        goto e_Exit;

    if (options & D3DXMESHSIMP_VERTEX)
        hr = ptmSimpMesh->ReduceVertices(minValue);
    else 
        hr = ptmSimpMesh->ReduceFaces(minValue);

    if (FAILED(hr))
        goto e_Exit;

    ptmSimpMesh->GetDevice(&pDevice);
    pDevice->Release();  // ptmMesh retains a ref, create will add a new permanent one

    hr = ptmSimpMesh->CloneMeshFVF(
                ptmSimpMesh->GetOptions(), 
                ptmSimpMesh->GetFVF(),
                pDevice,
                NULL, NULL, pptmNewMesh);
    if (FAILED(hr))
        goto e_Exit;

e_Exit:
    GXRELEASE(ptmSimpMesh);
    return hr;
}

// -------------------------------------------------------------------------------
//  function    D3DXComputeBoundingSphere
//
//   devnote    Compute a basic bounding sphere for all the points in the mesh
//
//   returns    S_OK if success, failed otherwise
//
HRESULT WINAPI D3DXComputeBoundingSphere
    (
    PVOID pvPoints, 
    DWORD cVertices, 
    DWORD dwFVF,
    D3DXVECTOR3 *pvCenter, 
    float *pfRadius
    )
{
    D3DXVECTOR3 vDist;
    float fDistSq;
    float fRadiusSq;
    UINT iPoint;
    PBYTE pbCur;
    PBYTE pbPoints = (PBYTE)pvPoints;
    HRESULT hr = S_OK;
    D3DXVECTOR3 *pvCur;
    DXCrackFVF cfvf(dwFVF);

    if ((pvPoints == NULL) || (pvCenter == NULL) || (pfRadius == NULL))
    {
#if DBG
        if (pvPoints == NULL)
		    DPF(0, "D3DXComputeBoundingSphere: NULL input vertex array\n");
        else if (pvCenter == NULL)
		    DPF(0, "D3DXComputeBoundingSphere: NULL center output parameter\n");
        else if (pfRadius == NULL)
		    DPF(0, "D3DXComputeBoundingSphere: NULL radius output paramter\n");
#endif     
        hr = D3DERR_INVALIDCALL;
        goto e_Exit;
    }

    *pvCenter = D3DXVECTOR3(0.0, 0.0, 0.0);

    if (cVertices == 0)
    {
        *pfRadius = 1.0;

        hr = S_OK;
        goto e_Exit;
    }

    for( iPoint=0, pbCur = pbPoints; iPoint < cVertices; iPoint++, pbCur += cfvf.m_cBytesPerVertex )
    {
        pvCur = (D3DXVECTOR3*)pbCur;

        pvCenter->x += pvCur->x;
        pvCenter->y += pvCur->y;
        pvCenter->z += pvCur->z;
    }

    *(D3DXVECTOR3*)pvCenter /= (float)cVertices;


    fRadiusSq = 0.0;
    for( iPoint=0, pbCur = pbPoints; iPoint < cVertices; iPoint++, pbCur += cfvf.m_cBytesPerVertex )
    {
        pvCur = (D3DXVECTOR3*)pbCur;

        vDist = *pvCur - *pvCenter;

        fDistSq = D3DXVec3LengthSq(&vDist);

        if( fDistSq > fRadiusSq )
            fRadiusSq = fDistSq;
    }

    (*pfRadius) = (float)sqrt((double)fRadiusSq);

e_Exit:

    return hr;
}


// -------------------------------------------------------------------------------
//  function    D3DXComputeBoundingBox
//
//   devnote    Compute a basic axis aligned bounding box for all the points in the mesh
//
//   returns    S_OK if success, failed otherwise
//
HRESULT WINAPI D3DXComputeBoundingBox
    (
    PVOID pvPoints, 
    DWORD cVertices, 
    DWORD dwFVF,
    D3DXVECTOR3 *pvMin, 
    D3DXVECTOR3 *pvMax
    )
{
    D3DXVECTOR3 vDist;
    //float fDistSq;
    //float fRadiusSq;
    UINT iPoint;
    PBYTE pbCur;
    PBYTE pbPoints = (PBYTE)pvPoints;
    HRESULT hr = S_OK;
    D3DXVECTOR3 *pvCur;
    //UINT cNonZeroVertices;
    DXCrackFVF cfvf(dwFVF);

    if ((pvPoints == NULL) || (pvMin == NULL) || (pvMax == NULL))
    {
#if DBG
        if (pvPoints == NULL)
		    DPF(0, "D3DXComputeBoundingBox: NULL input vertex array\n");
        else if (pvMin == NULL)
		    DPF(0, "D3DXComputeBoundingBox: NULL min output parameter\n");
        else if (pvMax == NULL)
		    DPF(0, "D3DXComputeBoundingBox: NULL max output paramter\n");
#endif     
        hr = D3DERR_INVALIDCALL;
        goto e_Exit;
    }

    if (cVertices == 0)
    {
        *pvMin = D3DXVECTOR3(0.0f,0.0f,0.0f);
        *pvMax = D3DXVECTOR3(0.0f,0.0f,0.0f);

        hr = S_OK;
        goto e_Exit;
    }

    // initialize pvMin/pvMax
    *pvMin = *(D3DXVECTOR3*)pbPoints;
    *pvMax = *(D3DXVECTOR3*)pbPoints;

    for( iPoint=1, pbCur = pbPoints; iPoint < cVertices; iPoint++, pbCur += cfvf.m_cBytesPerVertex )
    {
        pvCur = (D3DXVECTOR3*)pbCur;

        if (pvCur->x < pvMin->x)
            pvMin->x = pvCur->x;
        else if (pvCur->x > pvMax->x)
            pvMax->x = pvCur->x;

        if (pvCur->y < pvMin->y)
            pvMin->y = pvCur->y;
        else if (pvCur->y > pvMax->y)
            pvMax->y = pvCur->y;

        if (pvCur->z < pvMin->z)
            pvMin->z = pvCur->z;
        else if (pvCur->z > pvMax->z)
            pvMax->z = pvCur->z;
    }

e_Exit:

    return hr;
}

// -------------------------------------------------------------------------------
//  function    D3DXCreateBuffer
//
//   devnote    Creates a buffer of the given size and returns
//                  it as an ID3DXBuffer, used to give a 
//                  generic memory buffer AddRef/Release sematics
//
//   returns    S_OK if success, failed otherwise
//
HRESULT WINAPI D3DXCreateBuffer
    (
    DWORD cBytes, 
    LPD3DXBUFFER *ppBuffer
    )
{
    CD3DXBuffer *pBuffer = NULL;
    HRESULT hr = S_OK;

    // first allocate the container
    pBuffer = new CD3DXBuffer();
    if (pBuffer == NULL)
    {
        hr = E_OUTOFMEMORY;
        goto e_Exit;
    }

    // now allocate the buffer
    hr = pBuffer->Init(cBytes);
    if (FAILED(hr))
        goto e_Exit;

    *ppBuffer = pBuffer;
    pBuffer = NULL;

e_Exit:
    delete pBuffer;
    return hr;
}


// -------------------------------------------------------------------------------
//  function    D3DXComputeNormals
//
//   devnote    Does an inplace calculation of smooth normals for the given mesh
//
//   returns    S_OK if success, failed otherwise
//
HRESULT WINAPI D3DXComputeNormals
    (
    LPD3DXBASEMESH pMesh
    )
{
    HRESULT hr = S_OK;

    D3DXVECTOR3 vNormal;
    D3DXVECTOR3 *pvNormal;
    D3DXVECTOR3 vEdge1;
    D3DXVECTOR3 vEdge2;
    D3DXVECTOR3 *pvPos0;
    D3DXVECTOR3 *pvPos1;
    D3DXVECTOR3 *pvPos2;
    WORD *pwFace;
    DWORD *pdwFace;
    UINT iVert;
    UINT cVertices;
    UINT iFace;
    UINT cFaces;
    UINT iPoint;
    PVOID pvPoint;
    LPDIRECT3DVERTEXBUFFER8 pibVertices = NULL;
    PVOID pvPoints = NULL;
    LPDIRECT3DINDEXBUFFER8 pibFaces = NULL;
    WORD *rgwFaces = NULL;
    DWORD *rgdwFaces;
    DXCrackFVF cfvf(D3DFVF_XYZ);
    BOOL b16BitIndex;
    DWORD pdwFaceCur[3];

    if ((pMesh == NULL) || !(pMesh->GetFVF() & D3DFVF_NORMAL))
    {
        if (pMesh == NULL)
        {
            DPF(0, "D3DXComputeNormals: Failed due to input mesh being NULL\n");
        }
        else
        {
            DPF(0, "D3DXComputeNormals: Normal field required on input mesh to be filled in (clonemesh can be used to add them)\n");
        }

        hr = D3DERR_INVALIDCALL;
        goto e_Exit;
    }

    cFaces = pMesh->GetNumFaces();
    cVertices = pMesh->GetNumVertices();
    cfvf = DXCrackFVF(pMesh->GetFVF());
    b16BitIndex = !(pMesh->GetOptions() & D3DXMESH_32BIT);

    hr = pMesh->GetIndexBuffer(&pibFaces);
    if (FAILED(hr))
        goto e_Exit;

    hr = pibFaces->Lock(0,0, (PBYTE*)&rgwFaces, 0 );
    if (FAILED(hr))
        goto e_Exit;
    rgdwFaces = (DWORD*)rgwFaces;

    hr = pMesh->GetVertexBuffer(&pibVertices);
    if (FAILED(hr))
        goto e_Exit;

    hr = pibVertices->Lock(0,0, (PBYTE*)&pvPoints, 0 );
    if (FAILED(hr))
        goto e_Exit;

    // reinitialize all normals to zero
    for (iVert = 0; iVert < cVertices; iVert++)
    {
        pvPoint = cfvf.GetArrayElem(pvPoints, iVert);

        pvNormal = cfvf.PvGetNormal(pvPoint);
        *pvNormal = D3DXVECTOR3(0.0, 0.0, 0.0);
    }

    // add in face normals
    for (iFace = 0; iFace < cFaces; iFace++)
    {
        // upconvert to 32 bit for simplicity
        if (b16BitIndex)
        {
            pwFace = &rgwFaces[iFace*3];

            pdwFaceCur[0] = pwFace[0];
            pdwFaceCur[1] = pwFace[1];
            pdwFaceCur[2] = pwFace[2];
        }
        else
        {
            pdwFace = &rgdwFaces[iFace*3];
            memcpy(pdwFaceCur, pdwFace, sizeof(DWORD)*3);
        }

        pvPos0 = cfvf.PvGetPosition(cfvf.GetArrayElem(pvPoints, pdwFaceCur[0]));
        pvPos1 = cfvf.PvGetPosition(cfvf.GetArrayElem(pvPoints, pdwFaceCur[1]));
        pvPos2 = cfvf.PvGetPosition(cfvf.GetArrayElem(pvPoints, pdwFaceCur[2]));

        vEdge1 = *pvPos0 - *pvPos1;
        vEdge2 = *pvPos0 - *pvPos2;

        // calculate the normal of the face from the two edge vectors
        D3DXVec3Cross(&vNormal, &vEdge1, &vEdge2);
//      vNormal /= D3DXVec3Length(&vNormal);

        for (iPoint = 0; iPoint < 3; iPoint++)
        {
            pvPoint = cfvf.GetArrayElem(pvPoints, pdwFaceCur[iPoint]);
            pvNormal = cfvf.PvGetNormal(pvPoint);

            *pvNormal += vNormal;
        }
    }

    // renormalize all normals
    for (iVert = 0; iVert < cVertices; iVert++)
    {
        pvPoint = cfvf.GetArrayElem(pvPoints, iVert);
        pvNormal = cfvf.PvGetNormal(pvPoint);

        D3DXVec3Normalize(pvNormal, pvNormal);
    }

e_Exit:
    if (rgwFaces != NULL)
    {
        pibFaces->Unlock();
    }
    GXRELEASE(pibFaces);

    if (pvPoints != NULL)
    {
        pibVertices->Unlock();
    }
    GXRELEASE(pibVertices);

    return hr;
}

// -------------------------------------------------------------------------------
//  function    D3DXDeclaratorFromFVF
//
//   devnote    Generates a declarator from a given FVF
//
//   returns    S_OK if success, failed otherwise
//
HRESULT WINAPI D3DXDeclaratorFromFVF
    (
    DWORD dwFVF,
    DWORD Declarator[MAX_FVF_DECL_SIZE]
    )
{
    DXCrackFVF cfvf(dwFVF);
    HRESULT hr = S_OK;

    DWORD iTemp;
    DWORD iTexCoord;
    DWORD nTexCoords;
    DWORD dwVsdt;
    DWORD dwTextureFormats;

    // UNDONE - need to make sure the FVF is valid

    iTemp = 0;
    Declarator[iTemp] = D3DVSD_STREAM(0);
    iTemp++;

    Declarator[iTemp] = D3DVSD_REG( D3DVSDE_POSITION,  D3DVSDT_FLOAT3),
    iTemp++;

    if (cfvf.CWeights() > 0)
    {
        if (cfvf.BIndexedWeights())
        {
            DPF(0, "D3DXDeclaratorFromFVF: Indexed weights not supported on Xbox");
            hr = D3DERR_INVALIDCALL;
            goto e_Exit;
        }
        else
        {
            if (cfvf.CWeights() >= 5)
            {
                DPF(0, "D3DXDeclaratorFromFVF: Five weights is not supported in declarator without blend indices bit set");
                hr = D3DERR_INVALIDCALL;
                goto e_Exit;
            }

            // subtract one to convert to D3DVSDT_FLOAT* 
            switch( cfvf.CWeights() - 1 )
            {
               case 1: 
                  Declarator[iTemp] = D3DVSD_REG( D3DVSDE_BLENDWEIGHT,  D3DVSDT_FLOAT1 );
                  break;
               case 2: 
                  Declarator[iTemp] = D3DVSD_REG( D3DVSDE_BLENDWEIGHT,  D3DVSDT_FLOAT2 );
                  break;
               case 3: 
                  Declarator[iTemp] = D3DVSD_REG( D3DVSDE_BLENDWEIGHT,  D3DVSDT_FLOAT3 );
                  break;
               case 4: 
                  Declarator[iTemp] = D3DVSD_REG( D3DVSDE_BLENDWEIGHT,  D3DVSDT_FLOAT4 );
                  break;
            }

            iTemp++;
        }
    }

    GXASSERT(  (D3DVSDT_FLOAT1 == 0) && (D3DVSDT_FLOAT4 == 3) );

    if (cfvf.BNormal())
    {
        Declarator[iTemp] = D3DVSD_REG( D3DVSDE_NORMAL,  D3DVSDT_FLOAT3),
        iTemp++;
    }

    if (cfvf.BDiffuse())
    {
        Declarator[iTemp] = D3DVSD_REG( D3DVSDE_DIFFUSE,  D3DVSDT_D3DCOLOR),
        iTemp++;
    }

    if (cfvf.BSpecular())
    {
        Declarator[iTemp] = D3DVSD_REG( D3DVSDE_SPECULAR,  D3DVSDT_D3DCOLOR),
        iTemp++;
    }

    if (cfvf.CTexCoords() > 0)
    {
        dwTextureFormats = dwFVF >> 16;
    
        if (dwTextureFormats == 0)
        {
            for (iTexCoord = 0; iTexCoord < cfvf.CTexCoords(); iTexCoord++)
            {
                Declarator[iTemp] = D3DVSD_REG( D3DVSDE_TEXCOORD0 + iTexCoord,  D3DVSDT_FLOAT2),
                iTemp++;
            }
        }
        else
        {
            for (iTexCoord = 0; iTexCoord < cfvf.CTexCoords(); iTexCoord++)
            {
                switch (dwTextureFormats & 3)
                {
                case D3DFVF_TEXTUREFORMAT1:
                    dwVsdt = D3DVSDT_FLOAT1;
                    break;

                case D3DFVF_TEXTUREFORMAT2:
                    dwVsdt = D3DVSDT_FLOAT2;
                    break;

                case D3DFVF_TEXTUREFORMAT3:
                    dwVsdt = D3DVSDT_FLOAT3;
                    break;

                case D3DFVF_TEXTUREFORMAT4:
                    dwVsdt = D3DVSDT_FLOAT4;
                    break;
                }
    
                Declarator[iTemp] = D3DVSD_REG(D3DVSDE_TEXCOORD0 + iTexCoord, dwVsdt),
                iTemp++;

                dwTextureFormats >>= 2;
            }
        }
    }

    Declarator[iTemp] = D3DVSD_END();
    iTemp++;

e_Exit:
    GXASSERT(iTemp <= MAX_FVF_DECL_SIZE);
    return hr;
}

// -------------------------------------------------------------------------------
//  function    D3DXDeclaratorFromFVF
//
//   devnote    Generates an FVF from a given declarator if possible.
//
//   returns    S_OK if success, failed otherwise
//
HRESULT WINAPI D3DXFVFFromDeclarator
    (
    CONST DWORD *pDeclarator,
    DWORD *pdwFVF
    )
{
    HRESULT hr = S_OK;
    DWORD dwFVF;
    DWORD iMaxTexCoord;
    DWORD dwDataType;
    DWORD dwToken;
    CONST DWORD *pdwCur;

    // UNDONE UNDONE - this does not reject a number of invalid declarator to fvf ....

    dwFVF = 0;
    iMaxTexCoord = 0;
    pdwCur = pDeclarator;
    while ( 1 )
    {
        dwToken = (*pdwCur & D3DVSD_TOKENTYPEMASK) >> D3DVSD_TOKENTYPESHIFT;

        switch (dwToken)
        {
            case D3DVSD_TOKEN_STREAM:
                if (0 != ((*pdwCur & D3DVSD_STREAMNUMBERMASK) >> D3DVSD_STREAMNUMBERSHIFT))
                {
                    DPF(0, "D3DXFVFFromDeclarator: Only single stream declarators can be converted to FVF");
                }
                break;

            case D3DVSD_TOKEN_STREAMDATA:
                dwDataType = ((*pdwCur & D3DVSD_DATATYPEMASK) >> D3DVSD_DATATYPESHIFT);

                switch ((*pdwCur & D3DVSD_VERTEXREGMASK) >> D3DVSD_VERTEXREGSHIFT)
                {
                    case D3DVSDE_POSITION:    
                        if (dwDataType != D3DVSDT_FLOAT3)
                        {
                            DPF(0, "D3DXFVFFromDeclarator: Position not specified as a FLOAT3\n");
                            hr = D3DERR_INVALIDCALL;
                            goto e_Exit;
                        }

                        dwFVF |= D3DFVF_XYZ;
                        break;

                    case D3DVSDE_BLENDWEIGHT: 
                        dwFVF &= ~D3DFVF_POSITION_MASK;
                        switch (dwDataType)
                        {
                            case D3DVSDT_FLOAT1:
                                dwFVF |= D3DFVF_XYZB1;
                                break;
                            case D3DVSDT_FLOAT2:
                                dwFVF |= D3DFVF_XYZB2;
                                break;
                            case D3DVSDT_FLOAT3:
                                dwFVF |= D3DFVF_XYZB3;
                                break;
                            case D3DVSDT_FLOAT4:
                                dwFVF |= D3DFVF_XYZB4;
                                break;
                            default:
                                DPF(0, "D3DXFVFFromDeclarator: Unsupported type for D3DVSDE_BLENDWEIGHT, only FLOAT1-FLOAT4 are supported");
                                hr = D3DERR_INVALIDCALL;
                                goto e_Exit;
                        }
                        break;

                    case D3DVSDE_NORMAL:      
                        if (dwDataType != D3DVSDT_FLOAT3)
                        {
                            DPF(0, "D3DXFVFFromDeclarator: Normal not specified as a FLOAT3\n");
                            hr = D3DERR_INVALIDCALL;
                            goto e_Exit;
                        }
                        
                        dwFVF |= D3DFVF_NORMAL;
                        break;

                    case D3DVSDE_DIFFUSE:     
                        if (dwDataType != D3DVSDT_D3DCOLOR)
                        {
                            DPF(0, "D3DXFVFFromDeclarator: Diffuse not specified as a D3DCOLOR\n");
                            hr = D3DERR_INVALIDCALL;
                            goto e_Exit;
                        }

                        dwFVF |= D3DFVF_DIFFUSE;
                        break;

                    case D3DVSDE_SPECULAR:    
                        if (dwDataType != D3DVSDT_D3DCOLOR)
                        {
                            DPF(0, "D3DXFVFFromDeclarator: Specular not specified as a D3DCOLOR\n");
                            hr = D3DERR_INVALIDCALL;
                            goto e_Exit;
                        }

                        dwFVF |= D3DFVF_SPECULAR;
                        break;

                    case D3DVSDE_TEXCOORD0:   
                    case D3DVSDE_TEXCOORD1:   
                    case D3DVSDE_TEXCOORD2:   
                    case D3DVSDE_TEXCOORD3:   
                        if (dwDataType != D3DVSDT_FLOAT2)
                        {
                            // UNDONE UNDONE (Bug 33526) need to support 1D-4D tex coords
                            DPF(0, "D3DXFVFFromDeclarator: Texcoord not specified as a FLOAT2\n");
                            hr = D3DERR_INVALIDCALL;
                            goto e_Exit;
                        }

                        iMaxTexCoord = max(iMaxTexCoord, ((*pdwCur & D3DVSD_VERTEXREGMASK) >> D3DVSD_VERTEXREGSHIFT) - D3DVSDE_TEXCOORD0 + 1);

                        break;
                }
                break;

            case D3DVSD_TOKEN_NOP:
            case D3DVSD_TOKEN_TESSELLATOR:
            case D3DVSD_TOKEN_CONSTMEM:   
            case D3DVSD_TOKEN_EXT:
                DPF(0, "D3DXFVFFromDeclarator: Unhandled token type found in declarator to be converted");
                hr = D3DERR_INVALIDCALL;
                goto e_Exit;

            case D3DVSD_TOKEN_END:
                goto EndToken;

            default:
                DPF(0, "D3DXFVFFromDeclarator: Invalid token type found in declarator to be converted");
                hr = D3DERR_INVALIDCALL;
                goto e_Exit;
        }

        pdwCur += 1;
    }
EndToken:
    // save setting the 
    if (iMaxTexCoord > 0)
    {
        dwFVF |= (D3DFVF_TEX1 * iMaxTexCoord);
    }

    *pdwFVF = dwFVF;

e_Exit:
    return hr;
}

// Compare function for qsort in GXTri3Mesh<UINT_IDX,b16BitIndex,UNUSED>::AttributeSort()
int __cdecl CmpFunc(const void* elem1, const void * elem2)
{
    return *((DWORD*)elem1) - *((DWORD*)elem2);
}

// -------------------------------------------------------------------------------
//  function    GenerateWedgeList
//
//   devnote    Helper function for D3DXWeldVertices, used to 
//
//   returns    S_OK if success, failed otherwise
//
HRESULT GenerateWedgeList
    (
    DWORD *rgdwPointReps, 
    DWORD cVertices, 
    BOOL *pbIdentityPointReps, 
    DWORD *rgdwWedgeList
    )
{
    BOOL bLinkFound = FALSE;
    DWORD iVertex;
    DWORD dwPointRep;

    // initialize all entries to point to themselves
    for (iVertex = 0; iVertex < cVertices; iVertex++)
    {
        rgdwWedgeList[iVertex] = iVertex;
    }

    // now go back and link them up using the pointreps
    for (iVertex = 0; iVertex < cVertices; iVertex++)
    {
        // get the representative for this set of wedges
        dwPointRep = rgdwPointReps[iVertex];

        if (dwPointRep != iVertex)
        {
            bLinkFound = TRUE;

            // link the new point in just after the representative vertex
            rgdwWedgeList[iVertex] = rgdwWedgeList[dwPointRep];
            rgdwWedgeList[dwPointRep] = iVertex;
        }
    }

    *pbIdentityPointReps = !bLinkFound;

    return S_OK;;
}

BOOL BFloatsEqual
    (
    float fEpsilon,
    float f1,
    float f2
    )
{
    // first do a bitwise compare
    if ((*(DWORD*)&f1) == (*(DWORD*)&f2))
        return TRUE;

    // next do an epsilon compare
    float fDiff = (f1 - f2);
    return (fDiff <= fEpsilon) && (fDiff >= -fEpsilon);
}

BOOL BEqualVertices
    (
    DXCrackFVF &cfvf, 
    float fEpsilon,
    PBYTE pbVertex1,
    PBYTE pbVertex2
    )
{
    BOOL bEqual = TRUE;
    DWORD cTexFloats;
    DWORD iTexFloat;
    float *pfTexFloats1;
    float *pfTexFloats2;
    D3DXVECTOR3 *pvNormal1;
    D3DXVECTOR3 *pvNormal2;
    float *pfWeights1;
    float *pfWeights2;
    DWORD iWeight;

    if (cfvf.BNormal())
    {
        pvNormal1 = cfvf.PvGetNormal(pbVertex1);
        pvNormal2 = cfvf.PvGetNormal(pbVertex2);

        if (!BFloatsEqual(fEpsilon, pvNormal1->x, pvNormal2->x)
            || !BFloatsEqual(fEpsilon, pvNormal1->y, pvNormal2->y)
            || !BFloatsEqual(fEpsilon, pvNormal1->z, pvNormal2->z))
        {
            bEqual = FALSE;
            goto e_Exit;
        }
    }

    // ignore epsilon for both diffuse and specular colors
    if (cfvf.BDiffuse())
    {
        if (cfvf.ColorGetDiffuse(pbVertex1) != cfvf.ColorGetDiffuse(pbVertex2))
        {
            bEqual = FALSE;
            goto e_Exit;
        }
    }

    if (cfvf.BSpecular())
    {
        if (cfvf.ColorGetSpecular(pbVertex1) != cfvf.ColorGetSpecular(pbVertex2))
        {
            bEqual = FALSE;
            goto e_Exit;
        }
    }

    if (cfvf.CWeights() > 0)
    {
        pfWeights1 = (float*)(((PBYTE)cfvf.PvGetPosition(pbVertex1)) + sizeof(D3DXVECTOR3));
        pfWeights2 = (float*)(((PBYTE)cfvf.PvGetPosition(pbVertex2)) + sizeof(D3DXVECTOR3));

        for (iWeight = 0; iWeight < cfvf.CWeights(); iWeight++)
        {
            if (!BFloatsEqual(fEpsilon, pfWeights1[iWeight], pfWeights2[iWeight]))
            {
                bEqual = FALSE;
                goto e_Exit;
            }
        }
    }

    if (cfvf.CTexCoords() > 0)
    {
		cTexFloats = (cfvf.m_cBytesPerVertex - cfvf.m_oTex1) / sizeof(float);
        pfTexFloats1 = (float*)cfvf.PuvGetTex1(pbVertex1);
        pfTexFloats2 = (float*)cfvf.PuvGetTex1(pbVertex2);

        for (iTexFloat = 0; iTexFloat < cTexFloats; iTexFloat++)
        {
            if (!BFloatsEqual(fEpsilon, pfTexFloats1[iTexFloat], pfTexFloats2[iTexFloat]))
            {
                bEqual = FALSE;
                goto e_Exit;
            }
        }
    }

e_Exit:
    return bEqual;
}


// -------------------------------------------------------------------------------
//  function    D3DXWeldVertices
//
//   devnote    Welds replicated vertices together that have attributes that are equal (using epsilon compare)
//                  NOTE: this requires vertices with equal position to already have been calculated
//                              represented by point reps
//
//   returns    S_OK if success, failed otherwise
//
HRESULT WINAPI D3DXWeldVertices
    (
    CONST LPD3DXMESH pMesh,         // mesh to weld from
    float fEpsilon,                 // error in comparing identical attributes
    CONST DWORD *rgdwAdjacencyIn, 
    DWORD *rgdwAdjacencyOut,
    DWORD* pFaceRemap, 
    LPD3DXBUFFER *ppbufVertexRemap
    )
{
    HRESULT hr = S_OK;
    BOOL bIdentityPointReps;
    DWORD *rgdwPointReps = NULL;
    DWORD *rgdwWedgeList = NULL;
    DWORD cVertices;
    DWORD *rgdwRemap = NULL;
    DWORD *rgdwAdjacencyComputed = NULL;
    PBYTE pbFaces;
    PDWORD pdwFaces;
    PWORD pwFaces;
    DWORD cFaces;
    DWORD iVertex;
    DWORD dwCurOuter;
    DWORD dwCurInner;
    DWORD iFace;
    BOOL b16BitMesh;
    BOOL bRemapFound = FALSE;
    PBYTE pbVertices = NULL;
    DXCrackFVF cfvf(D3DFVF_XYZ);

    if ((pMesh == NULL) || (fEpsilon < 0.0f))
    {
        if (pMesh == NULL)
		    DPF(0, "D3DXWeldVertices: No mesh specified to weld");
        else if (rgdwAdjacencyIn == NULL)
        {
		    DPF(0, "D3DXWeldVertices: No adjacency specified to D3DXWeldVertices");
		    DPF(0, "D3DXWeldVertices:   Adjacency info is required to identify replicated vertices");
        }
        else if (fEpsilon < 0.0f)
        {
		    DPF(0, "D3DXWeldVertices: Epsilon value must be greater than or equal to zero.");
        }

        hr = D3DERR_INVALIDCALL;
        goto e_Exit;
    }

    cVertices = pMesh->GetNumVertices();
    cFaces = pMesh->GetNumFaces();
    b16BitMesh = !(pMesh->GetOptions() & D3DXMESH_32BIT);
    cfvf = DXCrackFVF(pMesh->GetFVF());

    if (rgdwAdjacencyIn == NULL)
    {
        rgdwAdjacencyComputed = new DWORD[cFaces * 3];
        if (rgdwAdjacencyComputed == NULL)
        {
            hr = E_OUTOFMEMORY;
            goto e_Exit;
        }

        hr = pMesh->GenerateAdjacency(fEpsilon, rgdwAdjacencyComputed);
        if (FAILED(hr))
            goto e_Exit;

        rgdwAdjacencyIn = rgdwAdjacencyComputed;
    }

    rgdwRemap = new DWORD[cVertices];
    rgdwPointReps = new DWORD[cVertices];
    rgdwWedgeList = new DWORD[cVertices];
    if ((rgdwRemap == NULL) || (rgdwPointReps == NULL) || (rgdwWedgeList == NULL))
    {
        hr = E_OUTOFMEMORY;
        goto e_Exit;
    }

    // initialize all remapings to self
    for (iVertex = 0; iVertex < cVertices; iVertex++)
    {
        rgdwRemap[iVertex] = iVertex;
    }

    // first generate the point reps
    hr = pMesh->ConvertAdjacencyToPointReps(rgdwAdjacencyIn, rgdwPointReps);
    if (FAILED(hr))
        goto e_Exit;

    // next generate the wedge list
    hr = GenerateWedgeList(rgdwPointReps, cVertices, &bIdentityPointReps, rgdwWedgeList);
    if (FAILED(hr))
        goto e_Exit;

    hr = pMesh->LockVertexBuffer(D3DLOCK_READONLY, &pbVertices);
    if (FAILED(hr))
        goto e_Exit;

    // if identity point reps, nothing to weld
    if (!bIdentityPointReps) 
    {
        // now we need to look at each of the vertex loops to decide
        //   who we need to merge
        for (iVertex = 0; iVertex < cVertices; iVertex++)
        {
            // use the point reps to look at each loop only once
            //   find the head of a loop (the point rep) and then walk the wedge list
            // NOTE: also skip if the wedge list has only this vertex on it
            if ((rgdwPointReps[iVertex] == iVertex) && (rgdwWedgeList[iVertex] != iVertex))
            {
                dwCurOuter = iVertex;
                do
                {
                    // if a remapping for the vertex hasn't been found, check to see
                    //   if it matches any other vertices
                    if (rgdwRemap[dwCurOuter] == dwCurOuter)
                    {
                        dwCurInner = rgdwWedgeList[iVertex];
                        do 
                        {
                            // don't check for equalivalence if indices the same (had better be equal then)
                            //    and/or if the one being checked is already being remapped
                            if ((dwCurInner != dwCurOuter) && (rgdwRemap[dwCurInner] == dwCurInner))
                            {
                                // if the two vertices are equal, then remap one to the other
                                if (BEqualVertices(cfvf, fEpsilon, 
                                                        cfvf.GetArrayElem(pbVertices, dwCurInner), 
                                                        cfvf.GetArrayElem(pbVertices, dwCurOuter)))
                                {
                                    // remap the inner vertices to the outer...
                                    rgdwRemap[dwCurInner] = dwCurOuter;

                                    bRemapFound = TRUE;
                                }

                            }

                            dwCurInner = rgdwWedgeList[dwCurInner];
                        } while (dwCurInner != iVertex);
                    }

                    dwCurOuter = rgdwWedgeList[dwCurOuter];
                } while (dwCurOuter != iVertex);
            }
        }
    }

    // if one or more vertices was found to weld, then weld
    if (bRemapFound)
    {
        pMesh->LockIndexBuffer(0, &pbFaces);

        // first fixup the indices to not point at the redundant vertices
        if (b16BitMesh)
        {
            pwFaces = (WORD*)pbFaces;

            for (iFace = 0; iFace < cFaces * 3; iFace++)
            {
                pwFaces[iFace] = (WORD)rgdwRemap[pwFaces[iFace]];
            }
        }
        else  // 32bit indices
        {
            pdwFaces = (DWORD*)pbFaces;

            for (iFace = 0; iFace < cFaces * 3; iFace++)
            {
                pdwFaces[iFace] = rgdwRemap[pdwFaces[iFace]];
            }
        }

        pMesh->UnlockIndexBuffer();

    }

    pMesh->UnlockVertexBuffer();
    pbVertices = NULL;

    // last but not least, optimize to get rid of the dead vertices    
    hr = pMesh->OptimizeInplace(D3DXMESHOPT_ATTRSORT, rgdwAdjacencyIn, rgdwAdjacencyOut, pFaceRemap, ppbufVertexRemap);

    if (FAILED(hr))
        goto e_Exit;

e_Exit:
    if (pbVertices != NULL)
    {
        pMesh->UnlockVertexBuffer();
    }

    delete []rgdwWedgeList;
    delete []rgdwPointReps;
    delete []rgdwRemap;
    delete []rgdwAdjacencyComputed;

    return hr;
}
