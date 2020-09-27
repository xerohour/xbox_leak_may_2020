/*//////////////////////////////////////////////////////////////////////////////
//
// File: loadmesh.cpp
//
// Copyright (C) 1999 Microsoft Corporation. All Rights Reserved.
//
// @@BEGIN_MSINTERNAL
//
// History:
// -@-          (craigp)    - created
// -@- 08/06/99 (mikemarr)  - prepend GX to all Luciform functions
//                          - started comment history
// -@- 08/26/99 (mikemarr)  - replace gxbasetype.h with gxmathcore.h
// -@- 09/23/99 (mikemarr)  - changed <> to "" in #includes
//
// @@END_MSINTERNAL
//
//////////////////////////////////////////////////////////////////////////////*/

#include "pchmesh.h"

#include "loadutil.h"
#include "skinmesh.h"
#include <initguid.h>
#define D3DRM_XTEMPLATES _D3DX_D3DRM_XTEMPLATES
#include "CD3dxbuffer.h"
#include "rmxftmpl.h"

typedef enum tagSTATFLAG {
    STATFLAG_DEFAULT= 0,
    STATFLAG_NONAME = 1,
    STATFLAG_NOOPEN = 2
} STATFLAG;

#undef DEFINE_GUID
#define DEFINE_GUID(name, l, w1, w2, b1, b2, b3, b4, b5, b6, b7, b8) \
    static const GUID name \
                    = { l, w1, w2, { b1, b2,  b3,  b4,  b5,  b6,  b7,  b8 } }
#include "rmxfguid.h"
#include "xskinexptemplates.h"

#include "savemesh.cpp"

// predecl
HRESULT WINAPI D3DXLoadMeshFromXofEx
    (
    LPDIRECTXFILEDATA pxofobjMesh,
    DWORD options,
    LPDIRECT3DDEVICE8 pD3DDevice,
    LPD3DXBUFFER *ppbufAdjacency,
    LPD3DXBUFFER *ppbufMaterials,
    PDWORD pcMaterials,
    LPD3DXMESH *ppMesh,
    PBYTE *ppbPMData,
    DWORD *pcbPMData,
    LPD3DXBUFFER *ppBoneNames,
    LPD3DXBUFFER *ppBoneTransforms,
    LPBONE *ppBones,
    DWORD *pNumBones
    );

DWORD DwCombineFVFs(DWORD dwFVF1, DWORD dwFVF2)
{
    DXCrackFVF cfvf1(dwFVF1);
    DXCrackFVF cfvf2(dwFVF2);
    DWORD dwWeights;
    DWORD dwTex;
    DWORD dwOut;

    dwWeights = max(cfvf1.CWeights(), cfvf2.CWeights());
    if (dwWeights > 0)
    {
        dwWeights *= 2;
        dwWeights += 4;
        dwOut = dwWeights;
    }
    else
    {
        dwOut = D3DFVF_XYZ;
    }

    if (cfvf1.BNormal() || cfvf2.BNormal())
        dwOut |= D3DFVF_NORMAL;

    if (cfvf1.BDiffuse() || cfvf2.BDiffuse())
        dwOut |= D3DFVF_DIFFUSE;

    if (cfvf1.BSpecular() || cfvf2.BSpecular())
        dwOut |= D3DFVF_SPECULAR;

    dwTex = max(cfvf1.CTexCoords(), cfvf2.CTexCoords());
    if (dwTex > 0)
    {
        dwOut |= (dwTex << D3DFVF_TEXCOUNT_SHIFT);
    }

    return dwOut;
}

// INTERNAL function, used to avoid header file issues with the PM template
HRESULT WINAPI
    D3DXCreatePMeshFromData
    (
    LPD3DXMESH ptmMesh,
    DWORD dwOptions,
    DWORD *rgdwAdjacency,
    PBYTE pbPMData,
    DWORD cbPMData,
    LPD3DXPMESH *pptmPMesh
    );

HRESULT D3DXMergeMeshes
    (
    LPD3DXMESH pMesh1,
    PDWORD rgdwAdjacency1,
    LPD3DXBUFFER pbufMaterials1,
    DWORD cmat1,
    LPD3DXMESH pMesh2,
    PDWORD rgdwAdjacency2,
    LPD3DXBUFFER pbufMaterials2,
    DWORD cmat2,
    LPDIRECT3DDEVICE8 pD3DDevice,
    LPD3DXMESH *ppMeshOut,
    LPD3DXBUFFER *ppbufAdjacencyOut,
    LPD3DXBUFFER *ppbufMaterialsOut,
    LPDWORD pcmatOut
    )
{
    HRESULT hr = S_OK;
    DWORD cVertices1;
    DWORD cVertices2;
    DWORD cVerticesOut;
    DWORD cFaces1;
    DWORD cFaces2;
    DWORD cFacesOut;
    LPD3DXMESH ptmMeshOut = NULL;
    LPD3DXBUFFER pbufAdjacencyOut = NULL;
    PDWORD rgdwAdjacencyOut;
    PDWORD rgdwAttribs1 = NULL;
    PDWORD rgdwAttribs2 = NULL;
    BOOL b16BitIndex;
    PBYTE pFacesOut = NULL;
    PDWORD rgdwAttribsOut = NULL;
    UINT iIndex;
    UINT iIndex2;
    DXCrackFVF cfvf(D3DFVF_XYZ);
    PBYTE pFaces1 = NULL;
    PBYTE pFaces2 = NULL;
    PBYTE pvPointsOut = NULL;
    PBYTE pvPoints1 = NULL;
    PBYTE    pvPoints2 = NULL;
    UINT cBytesPerIndex;
    LPD3DXBUFFER pbufMaterialsOut = NULL;
    DWORD dwFVF;
    DWORD dwOptions;
    LPD3DXMESH pTempMesh1 = NULL;
    LPD3DXMESH pTempMesh2 = NULL;

    // need all mesh pointers
    if ((pMesh1 == NULL) || (pMesh2 == NULL) || (ppMeshOut == NULL))
    {
        DPF(0, "Internal error - D3DXMergeMesh: Invalid pMesh1, pMesh2 or ppMeshOut pointers");
        return D3DXERR_INVALIDDATA;
    }

    // number of vertices
    cVertices1 = pMesh1->GetNumVertices();
    cVertices2 = pMesh2->GetNumVertices();
    cVerticesOut = cVertices1 + cVertices2;

    // number of faces
    cFaces1 = pMesh1->GetNumFaces();
    cFaces2 = pMesh2->GetNumFaces();
    cFacesOut = cFaces1 + cFaces2;

    dwOptions = pMesh1->GetOptions();

    if ((cFacesOut >= UNUSED16) 
        || (cVerticesOut >= UNUSED16)
        || (pMesh2->GetOptions() & D3DXMESH_32BIT))
    {
        dwOptions |= D3DXMESH_32BIT;
    }
    b16BitIndex = !(dwOptions & D3DXMESH_32BIT);

    if (pMesh1->GetFVF() != pMesh2->GetFVF())
    {
        dwFVF = DwCombineFVFs(pMesh1->GetFVF(), pMesh2->GetFVF());
    }
    else
    {
        dwFVF = pMesh1->GetFVF();
    }

    if ((pMesh1->GetOptions() != dwOptions) 
        || (pMesh1->GetFVF() != dwFVF))
    {
        hr = pMesh1->CloneMeshFVF(dwOptions, dwFVF, pD3DDevice, &pTempMesh1);
        if (FAILED(hr))
            return hr;

        // NOTE: don't release pMesh1, owned by caller, just release pTempMesh1 on exit
        pMesh1 = pTempMesh1;
    }

    if ((pMesh2->GetOptions() != dwOptions) 
        || (pMesh2->GetFVF() != dwFVF))
    {
        hr = pMesh2->CloneMeshFVF(dwOptions, dwFVF, pD3DDevice, &pTempMesh2);
        if (FAILED(hr))
            return hr;
        
        // NOTE: don't release pMesh2, owned by caller, just release pTempMesh2 on exit
        pMesh2 = pTempMesh2;
    }


    // had better have the same options
    GXASSERT((pMesh1->GetOptions() == pMesh2->GetOptions()) && (pMesh1->GetFVF() == pMesh2->GetFVF()));
    GXASSERT((pMesh1->GetOptions() == dwOptions) && (pMesh1->GetFVF() == dwFVF));
    GXASSERT(!b16BitIndex || ((cFacesOut < UNUSED16) && (cVerticesOut < UNUSED16)));
    GXASSERT((rgdwAdjacency1 != NULL) && (rgdwAdjacency2 != NULL) && (ppbufAdjacencyOut != NULL));


    hr = pMesh1->LockAttributeBuffer(D3DLOCK_READONLY, &rgdwAttribs1);
    if (FAILED(hr))
        goto e_Exit;

    hr = pMesh2->LockAttributeBuffer(D3DLOCK_READONLY, &rgdwAttribs2);
    if (FAILED(hr))
        goto e_Exit;

    hr = pMesh1->LockIndexBuffer(D3DLOCK_READONLY, &pFaces1);
    if (FAILED(hr))
        goto e_Exit;

    hr = pMesh2->LockIndexBuffer(D3DLOCK_READONLY, &pFaces2);
    if (FAILED(hr))
        goto e_Exit;

    if (b16BitIndex)
        cBytesPerIndex = sizeof(UINT16);
    else
        cBytesPerIndex = sizeof(UINT32);

    // generate a material buffer that is two material sets concated together
    hr = MergeMaterialBuffers(pbufMaterials1, cmat1, pbufMaterials2, cmat2, &pbufMaterialsOut);
    if (FAILED(hr))
        goto e_Exit;

    //create the mesh to fill
    hr = D3DXCreateMeshFVF(cFacesOut, cVerticesOut,
                                dwOptions, dwFVF,
                                pD3DDevice, &ptmMeshOut);
    if (FAILED(hr))
        goto e_Exit;

    hr = ptmMeshOut->LockIndexBuffer(0, &pFacesOut);
    if (FAILED(hr))
        goto e_Exit;

    hr = ptmMeshOut->LockAttributeBuffer(0, &rgdwAttribsOut);
    if (FAILED(hr))
        goto e_Exit;

    hr = D3DXCreateBuffer(cFacesOut * sizeof(DWORD) * 3, &pbufAdjacencyOut);
    if (FAILED(hr))
        goto e_Exit;
    rgdwAdjacencyOut = (PDWORD)pbufAdjacencyOut->GetBufferPointer();

    // now copy and remap the face data

    memcpy(pFacesOut, pFaces1, cBytesPerIndex * cFaces1 * 3);
    memcpy(rgdwAttribsOut, rgdwAttribs1, sizeof(DWORD) * cFaces1);
    for (iIndex = 0; iIndex < cFaces1*3; iIndex++)
    {
        if (rgdwAdjacency1 != NULL)
        {
            rgdwAdjacencyOut[iIndex] = rgdwAdjacency1[iIndex];
        }
        else
        {
            rgdwAdjacencyOut[iIndex] = UNUSED32;
        }
    }

    memcpy(pFacesOut + cBytesPerIndex * cFaces1 * 3, pFaces2, cBytesPerIndex * cFaces2 * 3);
    memcpy(rgdwAttribsOut + cFaces1, rgdwAttribs2, sizeof(DWORD) * cFaces2);
    for (iIndex = cFaces1 * 3, iIndex2 = 0; iIndex < cFacesOut*3; iIndex++, iIndex2++)
    {
        if (rgdwAdjacency2 != NULL)
        {
            if (rgdwAdjacency2[iIndex2] == UNUSED32)
                rgdwAdjacencyOut[iIndex] = UNUSED32;
            else
                rgdwAdjacencyOut[iIndex] = rgdwAdjacency2[iIndex2] + cFaces1;
        }
        else
        {
            rgdwAdjacencyOut[iIndex] = UNUSED32;
        }
    }


    if (b16BitIndex)
    {
        UINT16 *pFacesTemp;
        pFacesTemp = (UINT16*)pFacesOut;

        for (iIndex = cFaces1 * 3; iIndex < cFacesOut * 3; iIndex++)
        {
            if (pFacesTemp[iIndex] != UNUSED16)
                pFacesTemp[iIndex] += (UINT16)cVertices1;
        }
    }
    else
    {
        UINT32 *pFacesTemp;
        pFacesTemp = (UINT32*)pFacesOut;

        for (iIndex = cFaces1 * 3; iIndex < cFacesOut * 3; iIndex++)
        {
            if (pFacesTemp[iIndex] != UNUSED32)
                pFacesTemp[iIndex] += cVertices1;
        }
    }

    for (iIndex = cFaces1; iIndex < cFacesOut; iIndex++)
    {
        rgdwAttribsOut[iIndex] += cmat1;
    }

    // now copy the vertex data from the two buffers into the one

    hr = ptmMeshOut->LockVertexBuffer(0, &pvPointsOut);
    if (FAILED(hr))
        goto e_Exit;

    hr = pMesh1->LockVertexBuffer(D3DLOCK_READONLY, &pvPoints1);
    if (FAILED(hr))
        goto e_Exit;

    hr = pMesh2->LockVertexBuffer(D3DLOCK_READONLY, &pvPoints2);
    if (FAILED(hr))
        goto e_Exit;


    cfvf = DXCrackFVF(dwFVF);

    // finish by copying the two sets of vertex data into the destination buffer
    memcpy(pvPointsOut, pvPoints1, cfvf.m_cBytesPerVertex * cVertices1);
    memcpy((PBYTE)pvPointsOut + cfvf.m_cBytesPerVertex * cVertices1, pvPoints2,
                            cfvf.m_cBytesPerVertex * cVertices2);

    // all finished, setup return values
    *ppMeshOut = ptmMeshOut;
    ptmMeshOut->AddRef();
    if (ppbufAdjacencyOut != NULL)
    {
        *ppbufAdjacencyOut = pbufAdjacencyOut;
        pbufAdjacencyOut = NULL;
    }
    if (ppbufMaterialsOut != NULL)
    {
        *ppbufMaterialsOut = pbufMaterialsOut;
        pbufMaterialsOut = NULL;
    }
    if (pcmatOut != NULL)
        *pcmatOut = cmat1 + cmat2;

    GXASSERT(CheckAdjacency(rgdwAdjacencyOut, cFacesOut));

e_Exit:
    if (pFacesOut != NULL)
    {
        ptmMeshOut->UnlockIndexBuffer();
    }
    if (rgdwAttribsOut != NULL)
    {
        ptmMeshOut->UnlockAttributeBuffer();
    }
    if (rgdwAttribs1 != NULL)
    {
        pMesh1->UnlockAttributeBuffer();
    }
    if (rgdwAttribs2 != NULL)
    {
        pMesh2->UnlockAttributeBuffer();
    }

    if (pFaces1 != NULL)
    {
        pMesh1->UnlockIndexBuffer();
    }
    if (pFaces2 != NULL)
    {
        pMesh2->UnlockIndexBuffer();
    }

    if (pvPointsOut != NULL)
    {
        ptmMeshOut->UnlockVertexBuffer();
    }
    if (pvPoints1 != NULL)
    {
        pMesh1->UnlockVertexBuffer();
    }
    if (pvPoints2 != NULL)
    {
        pMesh2->UnlockVertexBuffer();
    }

    GXRELEASE(ptmMeshOut);
    GXRELEASE(pbufAdjacencyOut);
    GXRELEASE(pbufMaterialsOut);

    GXRELEASE(pTempMesh1);
    GXRELEASE(pTempMesh2);

    return hr;
}

HRESULT LoadMeshes
    (
    LPDIRECTXFILEDATA pxofobjCur,
    D3DXMATRIX &matCur,
    DWORD options,
    LPDIRECT3DDEVICE8 pD3DDevice,
    LPD3DXMESH *ppMesh,
    LPD3DXBUFFER *ppbufAdjacency,
    LPD3DXBUFFER *ppbufMaterials,
    LPDWORD pcmatOut
    )
{
    HRESULT hr = S_OK;
    LPDIRECTXFILEDATA pxofobjChild = NULL;
    LPDIRECTXFILEOBJECT pxofChild = NULL;
    const GUID *type;
    D3DXMATRIX matLocal;
    DWORD cbSize;
    D3DXMATRIX *pmatNew;
    LPDIRECT3DVERTEXBUFFER8 pVertexBuffer = NULL;
    PVOID pvPoints = NULL;
    LPD3DXMESH ptmMeshNew = NULL;
    LPD3DXBUFFER pbufAdjacencyNew = NULL;
    LPD3DXBUFFER pbufMaterialsNew = NULL;
    DWORD cmatNew;

    // Get the type of the object
    hr = pxofobjCur->GetType(&type);
    if (FAILED(hr))
        goto e_Exit;

    if (*type == TID_D3DRMMesh)
    {
        DXCrackFVF cfvf(D3DFVF_XYZ);
        D3DXVECTOR3 *pvVertexCur;
        UINT iVert;
        UINT cVertices;
        PVOID pvPoint;
        LPD3DXMESH ptmMeshTemp;
        LPD3DXBUFFER pbufAdjacencyTemp;
        LPD3DXBUFFER pbufMaterialsTemp = NULL;
        DWORD cmatTemp;

        hr = D3DXLoadMeshFromXofEx(pxofobjCur, options, pD3DDevice, &pbufAdjacencyNew, &pbufMaterialsNew, &cmatNew, &ptmMeshNew, NULL, NULL, NULL, NULL, NULL, NULL);
        if (FAILED(hr))
            goto e_Exit;

        // next apply the current frame transformation to the mesh

        cfvf = DXCrackFVF(ptmMeshNew->GetFVF());

        hr = ptmMeshNew->GetVertexBuffer(&pVertexBuffer);
        if (FAILED(hr))
            goto e_Exit;

        hr = pVertexBuffer->Lock(0,0, (PBYTE*)&pvPoints, 0 );
        if (FAILED(hr))
            goto e_Exit;

        cVertices = ptmMeshNew->GetNumVertices();

#if 1
        for (iVert = 0; iVert < cVertices; iVert++)
        {
            pvPoint = cfvf.GetArrayElem(pvPoints, iVert);

            pvVertexCur = cfvf.PvGetPosition(pvPoint);

            D3DXVec3TransformCoord(pvVertexCur, pvVertexCur, &matCur);

            if (cfvf.BNormal())
            {
                pvVertexCur = cfvf.PvGetNormal(pvPoint);

                D3DXVec3TransformNormal(pvVertexCur, pvVertexCur, &matCur);
                D3DXVec3Normalize(pvVertexCur, pvVertexCur);
            }
        }
#endif
        pVertexBuffer->Unlock();
        pvPoints = NULL;
        GXRELEASE(pVertexBuffer);

        if (*ppMesh == NULL)
        {
            *ppMesh = ptmMeshNew;
            *ppbufAdjacency = pbufAdjacencyNew;
            *ppbufMaterials = pbufMaterialsNew;
            *pcmatOut = cmatNew;
            ptmMeshNew = NULL;
            pbufAdjacencyNew = NULL;
            pbufMaterialsNew = NULL;
        }
        else
        {
            hr = D3DXMergeMeshes(*ppMesh, (PDWORD)((*ppbufAdjacency)->GetBufferPointer()),
                                                *ppbufMaterials, *pcmatOut,
                                        ptmMeshNew, (PDWORD)(pbufAdjacencyNew->GetBufferPointer()),
                                                pbufMaterialsNew, cmatNew,
                                        pD3DDevice, &ptmMeshTemp, &pbufAdjacencyTemp,
                                                &pbufMaterialsTemp, &cmatTemp);
            if (FAILED(hr))
                goto e_Exit;

            GXRELEASE(*ppMesh);
            GXRELEASE(*ppbufAdjacency);
            GXRELEASE(*ppbufMaterials);

            *ppMesh = ptmMeshTemp;
            *ppbufAdjacency = pbufAdjacencyTemp;
            *ppbufMaterials = pbufMaterialsTemp;
            *pcmatOut = cmatTemp;
        }
    }
    else if (*type == TID_D3DRMFrameTransformMatrix)
    {
        hr = pxofobjCur->GetData(NULL, &cbSize, (PVOID*)&pmatNew);
        if (FAILED(hr))
            goto e_Exit;

        // update the parents matrix with the new one
        //matCur *= *pmatNew;
        D3DXMatrixMultiply(&matCur, pmatNew, &matCur);
    }
    else if (*type == TID_D3DRMFrame)
    {
        matLocal = matCur;

        // Enumerate child objects.
        // Child object can be data, data reference or binary.
        // Use QueryInterface() to find what type of object a child is.
        while (SUCCEEDED(pxofobjCur->GetNextObject(&pxofChild)))
        {
            // Query the child for it's FileData
            hr = pxofChild->QueryInterface(IID_IDirectXFileData,
                                           (LPVOID *)&pxofobjChild);
            if (SUCCEEDED(hr))
            {
                hr = LoadMeshes(pxofobjChild, matLocal, options, pD3DDevice, ppMesh, ppbufAdjacency, ppbufMaterials, pcmatOut);
                if (FAILED(hr))
                    goto e_Exit;

                GXRELEASE(pxofobjChild);
            }

            GXRELEASE(pxofChild);
        }

    }

e_Exit:
    GXRELEASE(ptmMeshNew);
    GXRELEASE(pbufAdjacencyNew);
    GXRELEASE(pbufMaterialsNew);
    GXRELEASE(pxofobjChild);
    GXRELEASE(pxofChild);
    if (pvPoints != NULL)
        pVertexBuffer->Unlock();
    GXRELEASE(pVertexBuffer);
    return hr;
}

HRESULT WINAPI D3DXLoadMeshFromX
    (
    char *szFilename,
    DWORD options,
    LPDIRECT3DDEVICE8 pD3DDevice,
    LPD3DXBUFFER *ppbufAdjacency,
    LPD3DXBUFFER *ppbufMaterials,
    LPDWORD pcmatOut,
    LPD3DXMESH *ppMesh
    )
{
    HRESULT hr = S_OK;
    LPDIRECTXFILE pxofapi = NULL;
    LPDIRECTXFILEENUMOBJECT pxofenum = NULL;
    LPDIRECTXFILEDATA pxofobjCur = NULL;
    D3DXMATRIX matIdentity;
    LPD3DXBUFFER pbufAdjacencyDummy = NULL;
    LPD3DXBUFFER ppbufMaterialsDummy = NULL;
    DWORD cmatDummy;
    HINSTANCE hXof;
    LPDIRECTXFILECREATE pfnDirectXFileCreate;

    if ((ppMesh == NULL) || (szFilename == NULL))
    {
        hr = D3DERR_INVALIDCALL;
        goto e_Exit;
    }

    if (ppbufAdjacency == NULL)
        ppbufAdjacency = &pbufAdjacencyDummy;

    if (ppbufMaterials == NULL)
        ppbufMaterials = &ppbufMaterialsDummy;

    if (pcmatOut == NULL)
        pcmatOut = &cmatDummy;

#if 0
    // Create Xfile parser
    if(!(hXof = (HINSTANCE) GetModuleHandle("d3dxof.dll")) &&
       !(hXof = (HINSTANCE) LoadLibrary("d3dxof.dll")))
    {
        hr = HRESULT_FROM_WIN32(GetLastError());
        goto e_Exit;
    }
#endif
    if(!(pfnDirectXFileCreate = (LPDIRECTXFILECREATE)DirectXFileCreate))
    {
        hr = HRESULT_FROM_WIN32(GetLastError());
        goto e_Exit;
    }

    hr = pfnDirectXFileCreate(&pxofapi);
    if (FAILED(hr))
        goto e_Exit;

    // Registe templates for d3drm.
    hr = pxofapi->RegisterTemplates((LPVOID)D3DRM_XTEMPLATES,
                                    D3DRM_XTEMPLATE_BYTES);
    if (FAILED(hr))
        goto e_Exit;

    // Create enum object.
    hr = pxofapi->CreateEnumObject((LPVOID)szFilename,
                                   DXFILELOAD_FROMFILE,
                                   &pxofenum);
    if (FAILED(hr))
        goto e_Exit;

    // Enumerate top level objects.
    // Top level objects are always data object.
    *ppMesh = NULL;
    while (SUCCEEDED(pxofenum->GetNextDataObject(&pxofobjCur)))
    {
        D3DXMatrixIdentity(&matIdentity);

        hr = LoadMeshes(pxofobjCur, matIdentity, options, pD3DDevice,
                                ppMesh, ppbufAdjacency, ppbufMaterials, pcmatOut);
        if (FAILED(hr))
            goto e_Exit;

        GXRELEASE(pxofobjCur);
    }

    if (*ppMesh == NULL)
    {
        hr = E_FAIL;
        goto e_Exit;
    }

    // pointers are valid at this point, might be pointing to dummy variables
    if (*pcmatOut == 0)
    {
        GXASSERT(*ppbufMaterials == NULL);

        D3DXMATERIAL matGray;

        matGray.pTextureFilename = NULL;
        memset(&matGray.MatD3D, 0, sizeof(D3DMATERIAL8));
        matGray.MatD3D.Diffuse.r = 0.5f;
        matGray.MatD3D.Diffuse.g = 0.5f;
        matGray.MatD3D.Diffuse.b = 0.5f;
        matGray.MatD3D.Specular = matGray.MatD3D.Diffuse;

        hr = CreateMaterialBuffer(&matGray, 1, ppbufMaterials);
        if (FAILED(hr))
            goto e_Exit;

        *pcmatOut = 1;
    }


e_Exit:
    GXRELEASE(pbufAdjacencyDummy);
    GXRELEASE(ppbufMaterialsDummy);
    GXRELEASE(pxofobjCur);
    GXRELEASE(pxofenum);
    GXRELEASE(pxofapi);

    return hr;
}

HRESULT WINAPI D3DXLoadMeshFromXofEx
    (
    LPDIRECTXFILEDATA pxofobjMesh,
    DWORD options,
    LPDIRECT3DDEVICE8 pD3DDevice,
    LPD3DXBUFFER *ppbufAdjacency,
    LPD3DXBUFFER *ppbufMaterials,
    PDWORD pcMaterials,
    LPD3DXMESH *ppMesh,
    PBYTE *ppbPMData,
    DWORD *pcbPMData,
    LPD3DXBUFFER *ppBoneNames,
    LPD3DXBUFFER *ppBoneTransforms,
    LPBONE *ppBones,
    DWORD *pNumBones
    )
{
    HRESULT hr = S_OK;
    UINT cVertices;
    UINT cCorners;
    UINT cTriangles;
    UINT cPolygons;
    BOOL bNormalFound = FALSE;
    BOOL bTex1Found = FALSE;
    BOOL bVertexColorsFound = FALSE;
    UINT iVert;
    UINT iFace;
    UINT iTriangle;
    UINT iIndex;
    PBYTE pbCur;
    PBYTE pbMeshCur;
    PBYTE pbNormalCur;
    DWORD rgwIndicesCur[3];
    DWORD *rgwIndicesFile;
    DWORD cIndices;
    DWORD i;

    SLoadedFace        *rglfFaces = NULL;
    SLoadVertex *rglvLoaded = NULL;
    SCorner *rgCorners = NULL;
    LPDIRECTXFILEDATA pxofobjCur = NULL;
    LPDIRECTXFILEOBJECT pxofChild = NULL;
    LPDIRECTXFILEDATA pxofobjMaterial = NULL;
    LPDIRECTXFILEDATAREFERENCE pxofrefMaterial = NULL;
    LPDIRECTXFILEOBJECT pxofMaterial = NULL;
    LPDIRECTXFILEDATA pxofobjTexName = NULL;
    LPDIRECTXFILEOBJECT pxofTexName = NULL;
    PBYTE pbMeshData;
    PBYTE pbData;
    const GUID *type;
    DWORD cbSize;
    D3DXVECTOR3 *pvCurVertex;
    UINT iPoint;
    D3DXVECTOR3 *rgvNormals;
    UINT cNormals;
    UINT iCorner;
    UINT iNormal;
    DWORD *rgiMatID;
    UINT iMaterial;
    float *pfMaterialData;
    UINT cPolygonsDefined;
    D3DXMATERIAL *rgmatMaterials = NULL;
    UINT cMaterials = 0;
    char **pszTexName;
    UINT cbTexName;
    UINT cVertexIndices;
    DWORD dwFVF;
    DWORD cDWords;
    SFVFData *pFVFData = NULL;
    DWORD iColor;
    DWORD cVertexColors;
    D3DXCOLOR color;
    DWORD numBonesRead = 0;
    CD3DXStringBuffer* pBoneNames = NULL;
    LPBONE pBones = NULL;
    LPD3DXMATRIX pBoneTransforms = NULL;
    struct XSkinMeshHeader {
        WORD nMaxSkinWeightsPerVertex;
        WORD nMaxSkinWeightsPerFace;
        WORD numBones;
    } *header = NULL;
    BOOL bPointRepData = FALSE;

    // Initialize outputs
    if (ppbufAdjacency)
        *ppbufAdjacency = NULL;
    if (ppbufMaterials)
        *ppbufMaterials = NULL;
    if (ppMesh)
        *ppMesh = NULL;
    else
    {
        DPF(0, "ppMesh cannot be NULL");
        return D3DERR_INVALIDCALL;
    }
    if (ppbPMData)
        *ppbPMData = NULL;
    if (pcbPMData)
        *pcbPMData = NULL;
    if (ppBoneNames)
        *ppBoneNames = NULL;
    if (ppBoneTransforms)
        *ppBoneTransforms = NULL;
    if (ppBones)
        *ppBones = NULL;
    if (pNumBones)
        *pNumBones = 0;

    hr = pxofobjMesh->GetData(NULL, &cbSize, (PVOID*)&pbMeshData);
    if (FAILED(hr))
        goto e_Exit;

    cVertices = *(DWORD*)pbMeshData;
    cPolygons = *(DWORD*)(pbMeshData + sizeof(DWORD) + cVertices * 3 * sizeof(float));
    cCorners = 0;

    // make sure the size is correct
    if (cbSize < (sizeof(DWORD) * 2 + cVertices * 3 * sizeof(float) + sizeof(DWORD) * cPolygons))
    {
        hr = E_FAIL;
        goto e_Exit;
    }

    hr = InitVertices(rglvLoaded, cVertices);
    if (FAILED(hr))
        goto e_Exit;


    // load the positions of the verties,
    //     then load a decent default color
    pvCurVertex = (D3DXVECTOR3*)(pbMeshData + sizeof(DWORD));
    for (iVert = 0; iVert < cVertices; iVert++, pvCurVertex++)
    {
        rglvLoaded[iVert].m_vPos = *pvCurVertex;

        //rglvLoaded[iVert].m_color = D3DXCOLOR(0.9f, 0.6f, 0.4f, 0.0f);
        rglvLoaded[iVert].m_color = D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.0f);
    }

    // calculate the number of triangle by doing a pass over the triangle buffer
    pbCur = (PBYTE)(pbMeshData + sizeof(DWORD) * 2 + cVertices * 3 * sizeof(float));
    cTriangles = 0;
    for (iFace = 0; iFace < cPolygons; iFace++)
    {
        cIndices = *(DWORD*)pbCur;
        if (cIndices < 3)
        {
            hr = E_FAIL;
            goto e_Exit;
        }

        // find out the number of triangles (if 3, then 1 if 4, then 2, etc, etc.)
        cTriangles += (cIndices - 2);

        // skip over the indices and the count of indices
        pbCur += sizeof(DWORD) * (cIndices + 1);
    }

    hr = InitFaces(rglfFaces, cTriangles);
    if (FAILED(hr))
        goto e_Exit;

    // now do a second pass to load the indices into the prepared array,
    //   and convert any polygons to triangles
    pbCur = (PBYTE)(pbMeshData + sizeof(DWORD) * 2 + cVertices * 3 * sizeof(float));
    for (iFace = 0, iTriangle = 0; iFace < cPolygons; iFace++)
    {
        cIndices = *(DWORD*)pbCur;
        rgwIndicesFile = (DWORD*)(pbCur + sizeof(DWORD));

        // already checked when calculating the number of vertices
        GXASSERT(cIndices >= 3);

        rgwIndicesCur[0] = rgwIndicesFile[0];
        for (iIndex = 0; iIndex < cIndices - 2; iIndex++)
        {
            rgwIndicesCur[1] = rgwIndicesFile[iIndex+1];
            rgwIndicesCur[2] = rgwIndicesFile[iIndex+2];

            // copy the indices, both are 32 bit
            for (iPoint = 0; iPoint < 3; iPoint++)
            {
                rglfFaces[iTriangle].m_wIndices[iPoint] = rgwIndicesCur[iPoint];
            }
            iTriangle++;
        }

        // skip over the indices and the count of indices
        pbCur += sizeof(DWORD) * (cIndices + 1);
    }

    // Enumerate child objects.
    // Child object can be data, data reference or binary.
    // Use QueryInterface() to find what type of object a child is.
    while (SUCCEEDED(pxofobjMesh->GetNextObject(&pxofChild)))
    {
        // Query the child for it's FileData
        hr = pxofChild->QueryInterface(IID_IDirectXFileData,
                                       (LPVOID *)&pxofobjCur);
        if (SUCCEEDED(hr))
        {
            // Get the type of the object
            hr = pxofobjCur->GetType(&type);
            if (FAILED(hr))
                goto e_Exit;

            hr = pxofobjCur->GetData(NULL, &cbSize, (PVOID*)&pbData);
            if (FAILED(hr))
                goto e_Exit;


            if (*type == TID_D3DRMMeshTextureCoords)
            {
                bTex1Found = TRUE;

                // had better be enough texture coordinates
                if (*(DWORD*)pbData != cVertices)
                {
                    hr = E_FAIL;
                    goto e_Exit;
                }
                pbData += sizeof(DWORD);

                for (iVert = 0; iVert < cVertices; iVert++)
                {
                    rglvLoaded[iVert].m_uvTex1 = *(D3DXVECTOR2*)pbData;
                    pbData += sizeof(D3DXVECTOR2);
                }
            }
            else if (*type == TID_D3DRMMeshVertexColors)
            {
                bVertexColorsFound = TRUE;
                cVertexColors = *(DWORD*)pbData;

                pbData += sizeof(DWORD);

                for (iColor = 0; iColor < cVertexColors; iColor++)
                {
                    iIndex = *(DWORD*)pbData;
                    pbData += sizeof(DWORD);

                    color.r = *(float*)pbData;
                    pbData += sizeof(DWORD);
                    color.g = *(float*)pbData;
                    pbData += sizeof(DWORD);
                    color.b = *(float*)pbData;
                    pbData += sizeof(DWORD);
                    color.a = *(float*)pbData;
                    pbData += sizeof(DWORD);

                    rglvLoaded[iColor].m_color = (DWORD)color;
                }
            }
            // if this is a pm, load the normals slightly differently
            else if ((*type == TID_D3DRMMeshNormals) && (ppbPMData != NULL))
            {
                cNormals = *(DWORD*)pbData;
                bNormalFound = TRUE;

                // nFaceNormals has to match cPolygons
                if (cPolygons != *(DWORD*)(pbData + sizeof(DWORD) + cNormals * 3 * sizeof(float)))
                {
                    hr = E_FAIL;
                    goto e_Exit;
                }

                // normals MUST be provided for each vertex if this is a PM
                if (cNormals != cVertices)
                {
                    hr = E_FAIL;
                    goto e_Exit;
                }

                rgvNormals = (D3DXVECTOR3*)(pbData + sizeof(DWORD));

                for (iVert = 0; iVert < cVertices; iVert++)
                {
                    rglvLoaded[iVert].m_vNormal = rgvNormals[iVert];
                }
            }
            else if (*type == TID_D3DRMMeshNormals)
            {
                cCorners = cTriangles * 3;
                hr = InitCorners(rgCorners, cCorners);
                if (FAILED(hr))
                    goto e_Exit;


                // load the number of normals
                cNormals = *(DWORD*)pbData;
                bNormalFound = TRUE;

                // nFaceNormals has to match cPolygons
                if (cPolygons != *(DWORD*)(pbData + sizeof(DWORD) + cNormals * 3 * sizeof(float)))
                {
                    hr = E_FAIL;
                    goto e_Exit;
                }

                rgvNormals = (D3DXVECTOR3*)(pbData + sizeof(DWORD));

                pbMeshCur = (PBYTE)(pbMeshData + sizeof(DWORD) * 2 + cVertices * 3 * sizeof(float));
                pbNormalCur = (PBYTE)(pbData + 2 * sizeof(DWORD) + cNormals * 3 * sizeof(float));
                for (iFace = 0, iTriangle = 0, iCorner = 0; iFace < cPolygons; iFace++, pbCur++)
                {
                    cIndices = *(DWORD*)pbNormalCur;
                    rgwIndicesFile = (DWORD*)(pbNormalCur + sizeof(DWORD));
                    if ((cIndices < 3) || (cIndices != *(DWORD*)pbMeshCur))
                    {
                        hr = E_FAIL;
                        goto e_Exit;
                    }

                    rgwIndicesCur[0] = rgwIndicesFile[0];
                    for (iIndex = 0; iIndex < cIndices - 2; iIndex++)
                    {
                        rgwIndicesCur[1] = rgwIndicesFile[iIndex+1];
                        rgwIndicesCur[2] = rgwIndicesFile[iIndex+2];

                        // copy the indices, both are 32 bit
                        for (iPoint = 0; iPoint < 3; iPoint++)
                        {
                            iNormal = rgwIndicesCur[iPoint];

                            // load the data into the corner, including the normal
                            //   at the given offset provided by the index in the MeshFace
                            rgCorners[iCorner].m_wFace = iTriangle;
                            rgCorners[iCorner].m_wPoint = rglfFaces[iTriangle].m_wIndices[iPoint];
                            rgCorners[iCorner].m_bNormalSpecified = true;
                            rgCorners[iCorner].m_vNormal = rgvNormals[iNormal];
                            iCorner++;
                        }
                        iTriangle++;
                    }

                    // skip over the indices and the count of indices
                    pbMeshCur += sizeof(DWORD) * (cIndices + 1);
                    pbNormalCur += sizeof(DWORD) * (cIndices + 1);
                }

                GXASSERT(iTriangle == cTriangles);
                GXASSERT(iCorner == cCorners);
            }
            else if (*type == DXFILEOBJ_VertexDuplicationIndices)
            {
                cVertexIndices = *(DWORD*)(pbData);
                pbData += sizeof(DWORD);

                bPointRepData = TRUE;

                if ((cVertexIndices != cVertices) || (*(DWORD*)(pbData) > cVertexIndices))
                {
                    hr = D3DERR_INVALIDCALL;
                    goto e_Exit;
                }
                pbData += sizeof(DWORD);

                for (iVert = 0; iVert < cVertices; iVert++)
                {
                    rglvLoaded[iVert].m_wPointRep = *(DWORD*)pbData;
                    pbData += sizeof(DWORD);

                    if (rglvLoaded[iVert].m_wPointRep != iVert)
                    {
                        rglvLoaded[iVert].m_wPointList = rglvLoaded[rglvLoaded[iVert].m_wPointRep].m_wPointList;
                        rglvLoaded[rglvLoaded[iVert].m_wPointRep].m_wPointList = iVert;
                    }
                }
            }
            else if (*type == TID_D3DRMMeshMaterialList)
            {
                cMaterials = *(DWORD*)(pbData);
                if (cMaterials > 0)
                {
                    cPolygonsDefined = *(DWORD*)(pbData + sizeof(DWORD));

                    rgmatMaterials = new D3DXMATERIAL[cMaterials];
                    if (rgmatMaterials == NULL)
                    {
                        hr = E_OUTOFMEMORY;
                        goto e_Exit;
                    }

                    // get a pointer to the material index array
                    rgiMatID = (DWORD*)(pbData + 2 * sizeof(DWORD));

                    // need to do yet another pass over the mesh data to convert from
                    //  polygon to triangles
                    pbCur = (PBYTE)(pbMeshData + sizeof(DWORD) * 2 + cVertices * 3 * sizeof(float));
                    for (iFace = 0, iTriangle = 0; iFace < cPolygonsDefined; iFace++)
                    {
                        cIndices = *(DWORD*)pbCur;

                        // already checked when calculating the number of vertices
                        GXASSERT(cIndices >= 3);

                        for (iIndex = 0; iIndex < cIndices - 2; iIndex++)
                        {
                            rglfFaces[iTriangle].m_matid = rgiMatID[iFace];
                            iTriangle++;
                        }

                        // skip over the indices and the count of indices
                        pbCur += sizeof(DWORD) * (cIndices + 1);
                    }

                    // all triangles without a material id have a material id of zero
                    for ( ; iTriangle < cTriangles; iTriangle++)
                    {
                        rglfFaces[iTriangle].m_matid = 0;
                    }


                    iMaterial = 0;

                    // now load the materials
                    while (SUCCEEDED(pxofobjCur->GetNextObject(&pxofMaterial)))
                    {
                        // Query the child for it's FileData
                        hr = pxofMaterial->QueryInterface(IID_IDirectXFileData,
                                                       (LPVOID *)&pxofobjMaterial);

                        if (FAILED(hr))
                        {
                            hr = pxofMaterial->QueryInterface(IID_IDirectXFileDataReference,
                                                       (LPVOID *)&pxofrefMaterial);

                            // we had better be able to load each of the materials
                            if (FAILED(hr))
                                goto e_Exit;

                            hr = pxofrefMaterial->Resolve(&pxofobjMaterial);
                            if (FAILED(hr))
                                goto e_Exit;

                            GXRELEASE(pxofrefMaterial);
                        }

                        hr = pxofobjMaterial->GetData(NULL, &cbSize, (PVOID*)&pfMaterialData);
                        if (FAILED(hr))
                            goto e_Exit;

                        // load the material data from the child
                        rgmatMaterials[iMaterial].MatD3D.Diffuse.r = *pfMaterialData;
                        pfMaterialData++;
                        rgmatMaterials[iMaterial].MatD3D.Diffuse.g = *pfMaterialData;
                        pfMaterialData++;
                        rgmatMaterials[iMaterial].MatD3D.Diffuse.b = *pfMaterialData;
                        pfMaterialData++;
                        rgmatMaterials[iMaterial].MatD3D.Diffuse.a = *pfMaterialData;
                        pfMaterialData++;

                        rgmatMaterials[iMaterial].MatD3D.Power = *pfMaterialData;
                        pfMaterialData++;

                        rgmatMaterials[iMaterial].MatD3D.Specular.r = *pfMaterialData;
                        pfMaterialData++;
                        rgmatMaterials[iMaterial].MatD3D.Specular.g = *pfMaterialData;
                        pfMaterialData++;
                        rgmatMaterials[iMaterial].MatD3D.Specular.b = *pfMaterialData;
                        pfMaterialData++;
                        rgmatMaterials[iMaterial].MatD3D.Specular.a = 1.0f;

                        rgmatMaterials[iMaterial].MatD3D.Emissive.r = *pfMaterialData;
                        pfMaterialData++;
                        rgmatMaterials[iMaterial].MatD3D.Emissive.g = *pfMaterialData;
                        pfMaterialData++;
                        rgmatMaterials[iMaterial].MatD3D.Emissive.b = *pfMaterialData;
                        pfMaterialData++;
                        rgmatMaterials[iMaterial].MatD3D.Emissive.a = 1.0f;

                        rgmatMaterials[iMaterial].MatD3D.Ambient.r = 0.0f;
                        rgmatMaterials[iMaterial].MatD3D.Ambient.g = 0.0f;
                        rgmatMaterials[iMaterial].MatD3D.Ambient.b = 0.0f;

                        rgmatMaterials[iMaterial].pTextureFilename = NULL;

                        while (SUCCEEDED(pxofobjMaterial->GetNextObject(&pxofTexName)))
                        {
                            hr = pxofTexName->QueryInterface(IID_IDirectXFileData,
                                                           (LPVOID *)&pxofobjTexName);

                            if (FAILED(hr))
                            {
                                hr = pxofTexName->QueryInterface(IID_IDirectXFileDataReference,
                                                           (LPVOID *)&pxofrefMaterial);

                                // we had better be able to load each of the materials
                                if (FAILED(hr))
                                    goto e_Exit;

                                hr = pxofrefMaterial->Resolve(&pxofobjTexName);
                                if (FAILED(hr))
                                    goto e_Exit;

                                GXRELEASE(pxofrefMaterial);
                            }

                            hr = pxofobjTexName->GetType(&type);
                            if (FAILED(hr))
                                goto e_Exit;

                            if (*type == TID_D3DRMTextureFilename)
                            {
                                hr = pxofobjTexName->GetData(NULL, &cbSize, (PVOID*)&pszTexName);
                                if (FAILED(hr))
                                    goto e_Exit;

                                cbTexName = strlen(*pszTexName) + 1;

                                rgmatMaterials[iMaterial].pTextureFilename = new char[cbTexName];
                                if (rgmatMaterials[iMaterial].pTextureFilename == NULL)
                                {
                                    hr = E_OUTOFMEMORY;
                                    goto e_Exit;
                                }

                                memcpy(rgmatMaterials[iMaterial].pTextureFilename, *pszTexName, cbTexName);
                            }

                            GXRELEASE(pxofTexName);
                            GXRELEASE(pxofobjTexName);
                        }


                        GXRELEASE(pxofMaterial);
                        GXRELEASE(pxofobjMaterial);

                        iMaterial += 1;
                    }

                    // if we didn't find the correct number of materials, fail
                    if (iMaterial != cMaterials)
                    {
                        hr = E_FAIL;
                        goto e_Exit;
                    }

#if 1
                    for (iTriangle = 0; iTriangle < cTriangles; iTriangle++)
                    {
                        rglfFaces[iTriangle].m_attr = rglfFaces[iTriangle].m_matid;

                        rglfFaces[iTriangle].m_bAttributeSpecified = true;
                    }
#endif

#if 0
                    // UNDONE UNDONE - should make an options
                    // GXASSERT(0);
                    //   copy the material color to the face color
                    for (iTriangle = 0; iTriangle < cTriangles; iTriangle++)
                    {
                        rglfFaces[iTriangle].m_colorFace =
                            *(D3DXCOLOR*)&(rgmatMaterials[rglfFaces[iTriangle].m_matid].MatD3D.Diffuse);

                        rglfFaces[iTriangle].m_bColorSpecified = true;
                    }
#endif
                }
            }
            else if ((*type == DXFILEOBJ_PMInfo) && (ppbPMData != NULL))
            {
                // don't do anything with the PMInfo, just load it into an allocated buffer to return
                *pcbPMData = cbSize;
                *ppbPMData = new BYTE[cbSize];
                if (*ppbPMData == NULL)
                {
                    hr = E_OUTOFMEMORY;
                    goto e_Exit;
                }

                memcpy(*ppbPMData, pbData, cbSize);
            }
            else if ((*type == DXFILEOBJ_XSkinMeshHeader) && (ppBones != NULL))
            {
                header = reinterpret_cast<struct XSkinMeshHeader*>(pbData);
				*pNumBones = header->numBones;
#define _D3DX_AVG_BONENAME 32
                pBones = new CBone[header->numBones];
                if (pBones == NULL)
                    goto e_Exit;
                *ppBones = pBones;
                hr = D3DXCreateStringBuffer(_D3DX_AVG_BONENAME * header->numBones, &pBoneNames);
                if (FAILED(hr))
                    goto e_Exit;
                *ppBoneNames = pBoneNames;
                hr = D3DXCreateBuffer(sizeof(D3DXMATRIX) * header->numBones, ppBoneTransforms);
                if (FAILED(hr))
                    goto e_Exit;
                pBoneTransforms = reinterpret_cast<LPD3DXMATRIX>((*ppBoneTransforms)->GetBufferPointer());
            }
            else if ((*type == DXFILEOBJ_SkinWeights) && (pBoneTransforms != NULL))
            {

                // Copy BoneName
                pBoneNames->AddString(*(char**)pbData);
                // Copy numWeights
                pBones[numBonesRead].m_numWeights = ((DWORD*)pbData)[1];
                GXASSERT(2 * sizeof(DWORD) + (sizeof(DWORD) + sizeof(float)) * pBones[numBonesRead].m_numWeights + sizeof(D3DXMATRIX) == cbSize);
                // Copy vertex indices
                pBones[numBonesRead].m_pVertIndices = new DWORD[pBones[numBonesRead].m_numWeights];
                if (pBones[numBonesRead].m_pVertIndices == NULL)
                    goto e_Exit;
                memcpy(pBones[numBonesRead].m_pVertIndices, (LPDWORD)pbData + 2, sizeof(DWORD) * pBones[numBonesRead].m_numWeights);
                // Copy vertex weights
                pBones[numBonesRead].m_pWeights = new float[pBones[numBonesRead].m_numWeights];
                if (pBones[numBonesRead].m_pWeights == NULL)
                {
                    numBonesRead++;
                    goto e_Exit;
                }
                memcpy(pBones[numBonesRead].m_pWeights, (LPDWORD)pbData + 2 + pBones[numBonesRead].m_numWeights, sizeof(float) * pBones[numBonesRead].m_numWeights);
                // Copy bone transforms
                pBoneTransforms[numBonesRead] = *(D3DXMATRIX*)(pbData + sizeof(DWORD)*2 + (sizeof(DWORD) + sizeof(float)) * pBones[numBonesRead].m_numWeights);

                ++numBonesRead;
                GXASSERT(numBonesRead <= *pNumBones);
            }
            else if (*type == DXFILEOBJ_FVFData)
            {
                pFVFData = new SFVFData;
                if (pFVFData == NULL)
                {
                    hr = E_OUTOFMEMORY;
                    goto e_Exit;
                }

                pFVFData->dwFVF = *(DWORD*)pbData;
                pbData += sizeof(DWORD);

                cDWords = *(DWORD*)pbData;
                pbData += sizeof(DWORD);

                pFVFData->cBytesPerVertex = (cDWords * sizeof(DWORD)) / cVertices;

                pFVFData->rgiFVFData = new DWORD[cDWords];
                if (pFVFData->rgiFVFData == NULL)
                {
                    hr = E_OUTOFMEMORY;
                    goto e_Exit;
                }

                memcpy(pFVFData->rgiFVFData, pbData, cDWords * sizeof(DWORD));
            }

            GXRELEASE(pxofobjCur);
        }

        GXRELEASE(pxofChild);
    }

    if (!bNormalFound)
    {
        for (iFace = 0; iFace < cTriangles; iFace++)
        {
            if (rglfFaces[iFace].m_wIndices[0] != UNUSED32)
            {
                rglfFaces[iFace].m_bSmoothingGroupSpecified = true;
                rglfFaces[iFace].m_iSmoothingGroup = 1;
            }
        }
    }

    if (pFVFData != NULL)
    {
        dwFVF = pFVFData->dwFVF;
        pFVFData->iTexCoordOffset = 0;
    }
    else
    {
        dwFVF = D3DFVF_XYZ;
    }

    if (bNormalFound)
    {
        dwFVF |= D3DFVF_NORMAL;
    }

    if (bVertexColorsFound)
    {
        dwFVF |= D3DFVF_DIFFUSE;
    }

    if (bTex1Found)
    {
        // if separate block specified, then use it as the first tex coord and other fvf data as later ones
        if (pFVFData)
            pFVFData->iTexCoordOffset = 1;

        if ((dwFVF & D3DFVF_TEXCOUNT_MASK) == 0x800) // D3DFVF_TEX8
        {
            hr = D3DERR_INVALIDCALL;
            goto e_Exit;
        }

        dwFVF += D3DFVF_TEX1;
    }

    // TODO: Can we reduce the copying done by this function ? We can extend the skin mesh header to contain
    // more information and appear before the mesh object.
    hr = SetMesh(rglfFaces, cTriangles, rglvLoaded, cVertices, rgCorners, cCorners, pFVFData, bPointRepData,
                                options, dwFVF, pD3DDevice, ppMesh, ppbufAdjacency);
    if (FAILED(hr))
        goto e_Exit;

    if (ppbufMaterials != NULL)
    {
        if (cMaterials == 0)
        {
            *ppbufMaterials = NULL;
        }
        else
        {
            hr = CreateMaterialBuffer(rgmatMaterials, cMaterials, ppbufMaterials);
            if (FAILED(hr))
                goto e_Exit;
        }
    }

    if (pcMaterials != NULL)
    {
        *pcMaterials = cMaterials;
    }
	goto s_Exit;

e_Exit: 
    for (i = 0; i < numBonesRead; ++i)
    {
        GXASSERT(pBones[i].m_pVertIndices != NULL);
        delete [] pBones[i].m_pVertIndices;
        delete [] pBones[i].m_pWeights;
    }
    if (ppBones)
        delete [] *ppBones;
    if (ppBoneNames)
        GXRELEASE(*ppBoneNames);
    if (ppBoneTransforms)
        GXRELEASE(*ppBoneTransforms);
s_Exit:    
    delete []rglfFaces;
    delete []rglvLoaded;
    delete []rgCorners;
    if (pFVFData != NULL)
    {
        delete []pFVFData->rgiFVFData;
    }
    delete []pFVFData;

    if (rgmatMaterials != NULL)
    {
        for (iMaterial = 0; iMaterial < cMaterials; iMaterial++)
        {
            delete []rgmatMaterials[iMaterial].pTextureFilename;
        }
    }

    delete []rgmatMaterials;
    GXRELEASE(pxofChild);
    GXRELEASE(pxofobjCur);
    GXRELEASE(pxofMaterial);
    GXRELEASE(pxofobjMaterial);
    GXRELEASE(pxofrefMaterial);
    GXRELEASE(pxofobjTexName);
    GXRELEASE(pxofTexName);

    return hr;
}


HRESULT WINAPI D3DXLoadMeshFromXof
    (
    LPDIRECTXFILEDATA pxofobjMesh,
    DWORD options,
    LPDIRECT3DDEVICE8 pD3DDevice,
    LPD3DXBUFFER *ppbufAdjacency,
    LPD3DXBUFFER *ppbufMaterials,
    PDWORD pcMaterials,
    LPD3DXMESH *ppMesh
    )
{
    return D3DXLoadMeshFromXofEx(pxofobjMesh, options, pD3DDevice, ppbufAdjacency, ppbufMaterials, pcMaterials, ppMesh, NULL, NULL, NULL, NULL, NULL, NULL);
}

HRESULT WINAPI
    D3DXCreatePMeshFromStream
    (
    IStream *pstream,
    DWORD dwOptions,
    LPDIRECT3DDEVICE8 pD3DDevice,
    LPD3DXBUFFER *ppbufMaterials,
    DWORD *pcMaterials,
    LPD3DXPMESH *ppPMesh
    )
{
    HRESULT hr = S_OK;
    LPD3DXBUFFER pbufAdjacency = NULL;
    LPDIRECTXFILE pxofapi = NULL;
    LPDIRECTXFILEENUMOBJECT pxofenum = NULL;
    LPDIRECTXFILEDATA pxofobjCur = NULL;
    LPD3DXMESH ptmMesh = NULL;
    const GUID *type;
    DXFILELOADMEMORY dxfilemem;
    STATSTG statstg;
    DWORD cbPMData;
    PBYTE pbPMData = NULL;
    LPD3DXPMESH ptmPMesh = NULL;
    HINSTANCE hXof;
    LPDIRECTXFILECREATE pfnDirectXFileCreate;
   
    const DWORD x_dwOptionsFull = D3DXMESH_SYSTEMMEM;

    if (((dwOptions & ~D3DXMESH_VALIDBITS) != 0)
        || ((dwOptions & D3DXMESH_32BIT) != 0))
    {
#ifdef DBG
        if ((dwOptions & ~D3DXMESH_VALIDBITS) != 0)
		    DPF(0, "D3DXCreatePMeshFromStream: Invalid mesh flag specified\n");
        else if ((dwOptions & D3DXMESH_32BIT) != 0)
		    DPF(0, "D3DXCreatePMeshFromStream: Cannot specify 32bit, will be determined by file data.\n");
#endif
        hr = D3DERR_INVALIDCALL;
        goto e_Exit;
    }
#if 0
    // Create Xfile parser
    if(!(hXof = (HINSTANCE) GetModuleHandle("d3dxof.dll")) &&
       !(hXof = (HINSTANCE) LoadLibrary("d3dxof.dll")))
    {
        hr = HRESULT_FROM_WIN32(GetLastError());
        goto e_Exit;
    }
#endif
    if(!(pfnDirectXFileCreate = (LPDIRECTXFILECREATE)DirectXFileCreate))
    {
        hr = HRESULT_FROM_WIN32(GetLastError());
        goto e_Exit;
    }

    hr = pfnDirectXFileCreate(&pxofapi);
    if (FAILED(hr))
        goto e_Exit;

    // Registe templates for d3drm.
    hr = pxofapi->RegisterTemplates((LPVOID)D3DRM_XTEMPLATES,
                                    D3DRM_XTEMPLATE_BYTES);
    if (FAILED(hr))
        goto e_Exit;

    hr = pstream->Stat(&statstg, STATFLAG_NONAME);
    if (FAILED(hr))
        goto e_Exit;

    // sort of too much memory to allocate in 32 bit....
    if (statstg.cbSize.HighPart != 0)
    {
        hr = E_OUTOFMEMORY;
        goto e_Exit;
    }

    // since we can't handle streams with the X file loader, just copy it all into memory
    dxfilemem.lpMemory = new BYTE[statstg.cbSize.LowPart];
    if (dxfilemem.lpMemory == NULL)
    {
        hr = E_OUTOFMEMORY;
        goto e_Exit;
    }

    hr = pstream->Read(dxfilemem.lpMemory, statstg.cbSize.LowPart, &dxfilemem.dSize);
    if (FAILED(hr))
        goto e_Exit;

    hr = pxofapi->CreateEnumObject((LPVOID)&dxfilemem,
                                   DXFILELOAD_FROMMEMORY,
                                   &pxofenum);
    if (FAILED(hr))
        goto e_Exit;

    // Enumerate top level objects.
    // Top level objects are always data object.
    ptmMesh = NULL;
    while (SUCCEEDED(pxofenum->GetNextDataObject(&pxofobjCur)))
    {
        hr = pxofobjCur->GetType(&type);
        if (FAILED(hr))
            goto e_Exit;

        if (*type == TID_D3DRMMesh)
        {
            hr = D3DXLoadMeshFromXofEx(pxofobjCur, x_dwOptionsFull, pD3DDevice, &pbufAdjacency, ppbufMaterials, pcMaterials, &ptmMesh, &pbPMData, &cbPMData, NULL, NULL, NULL, NULL);
            if (FAILED(hr))
                goto e_Exit;

            break;
        }

        GXRELEASE(pxofobjCur);
    }

    if ((ptmMesh == NULL) || (pbPMData == NULL) || (cbPMData == 0))
    {
        hr = E_FAIL;
        goto e_Exit;
    }

    // fill the pmesh from the data loaded from the file
    hr = D3DXCreatePMeshFromData(ptmMesh, dwOptions, (DWORD*)pbufAdjacency->GetBufferPointer(), pbPMData, cbPMData, &ptmPMesh);
    if (FAILED(hr))
        goto e_Exit;

    *ppPMesh = ptmPMesh;
    ptmPMesh = NULL;
e_Exit:
    GXRELEASE(pbufAdjacency);
    GXRELEASE(pxofobjCur);
    GXRELEASE(pxofenum);
    GXRELEASE(pxofapi);
    GXRELEASE(ptmMesh);
    GXRELEASE(ptmPMesh);
    delete []pbPMData;

    return hr;
}

// Prototype for func defined in skinmesh.cpp
HRESULT WINAPI
D3DXCreateSkinMeshFromMeshAndBones(LPD3DXMESH pMesh, DWORD numBones, LPBONE pBones, LPD3DXSKINMESH* ppMesh);

HRESULT WINAPI
    D3DXLoadSkinMeshFromXof
    (
    LPDIRECTXFILEDATA pxofobjMesh, 
    DWORD options,
    LPDIRECT3DDEVICE8 pD3D,
    LPD3DXBUFFER* ppAdjacency,
    LPD3DXBUFFER* ppMaterials,
    PDWORD pMatOut,
    LPD3DXBUFFER* ppBoneNames,
    LPD3DXBUFFER* ppBoneTransforms,
    LPD3DXSKINMESH* ppMesh
    )
{
    LPBONE pBones = NULL;
    DWORD numBones, i;
    LPD3DXMESH pMesh = NULL;
    LPD3DXSKINMESH pSkinMesh = NULL;
    *ppAdjacency = NULL;
    *ppMaterials = NULL;
    *ppBoneNames = NULL;
    *ppBoneTransforms = NULL;
    *ppMesh = NULL;

    HRESULT hr = D3DXLoadMeshFromXofEx(pxofobjMesh, D3DXMESH_SYSTEMMEM, pD3D, ppAdjacency, ppMaterials, pMatOut, 
        &pMesh, NULL, NULL, ppBoneNames, ppBoneTransforms, &pBones, &numBones);
    if (FAILED(hr))
    {
        goto e_Exit;
    }

    hr = D3DXCreateSkinMeshFromMeshAndBones(pMesh, numBones, pBones, &pSkinMesh);
    if (FAILED(hr))
    {
        goto e_Exit;
    }

    pMesh->Release();

    *ppMesh = pSkinMesh;
    return S_OK;

e_Exit:
    if (*ppAdjacency)
        (*ppAdjacency)->Release();
    if (*ppMaterials)
        (*ppMaterials)->Release();
    if (pMesh)
        pMesh->Release();
    if (*ppBoneNames)
        (*ppBoneNames)->Release();
    if (*ppBoneTransforms)
        (*ppBoneTransforms)->Release();
    if (pBones)
        delete [] pBones;
    if (pSkinMesh)
        pSkinMesh->Release();
    return hr;
}
