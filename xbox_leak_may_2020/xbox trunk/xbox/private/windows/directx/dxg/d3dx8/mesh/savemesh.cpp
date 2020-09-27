/*//////////////////////////////////////////////////////////////////////////////
//
// File: savemesh.cpp
//
// Copyright (C) 1999 Microsoft Corporation. All Rights Reserved.
//
// @@BEGIN_MSINTERNAL
//
// History:
// -@-          (craigp)    - created
//
// @@END_MSINTERNAL
//
//////////////////////////////////////////////////////////////////////////////*/

#if 0
#include "pchmesh.h"

#define D3DRM_XTEMPLATES _D3DX2_D3DRM_XTEMPLATES
#include "rmxfguid.h"
#include "rmxftmpl.h"  
#include "xskinexptemplates.h"
#endif

#define WRITE_DWORD(pbCur, dword) {*(DWORD*)pbCur = dword; pbCur += sizeof(DWORD); }
#define WRITE_FLOAT(pbCur, value) {*(float*)pbCur = value; pbCur += sizeof(float); }
#define WRITE_VECTOR3(pbCur, vPos) {*(D3DXVECTOR3*)pbCur = vPos; pbCur += sizeof(D3DXVECTOR3); }
#define WRITE_RGBA(pbCur, vPos) {*(D3DCOLORVALUE*)pbCur = vPos; pbCur += sizeof(D3DCOLORVALUE); }
#define WRITE_RGB(pbCur, vColor) { WRITE_FLOAT(pbCur, vColor.r); WRITE_FLOAT(pbCur, vColor.g); WRITE_FLOAT(pbCur, vColor.b);}

HRESULT
AddNormals
    (
    LPDIRECTXFILESAVEOBJECT pxofsave,
    PBYTE pbVertices,
    DWORD cVertices,
    PWORD rgwFaces,
    PDWORD rgdwFaces,
    DWORD cFaces,
    DXCrackFVF &cfvf,
    LPDIRECTXFILEDATA pParent
    )
{
    HRESULT        hr = S_OK;
    LPDIRECTXFILEDATA pDataObject = NULL;
    LPBYTE         pbData;
    LPBYTE         pbCur = NULL;
    DWORD          cbSize;
    PWORD          pwFace;
    PDWORD         pdwFace;
    DWORD          iFace;
    DWORD          iVertex;

    GXASSERT(pbVertices != NULL);
    GXASSERT(rgwFaces != NULL);
    GXASSERT(pParent != NULL);

    cbSize = sizeof(DWORD) // nNormals
             + 3*sizeof(float)*cVertices // normals
             + sizeof(DWORD) // nFaces
             + cFaces* // MeshFace array
                (sizeof(DWORD) //nFaceVertexIndices (number of normal indices)
                 + 3*sizeof(DWORD)); // faceVertexIndices (normal indices)

    pbCur = pbData = new BYTE[cbSize];
    if (pbData == NULL)
    {
        hr = E_OUTOFMEMORY;
        goto e_Exit;
    }

    // nNormals
    WRITE_DWORD(pbCur, cVertices);

    // normals
    for (iVertex = 0; iVertex < cVertices; iVertex++)
    {
        WRITE_VECTOR3(pbCur, *cfvf.PvGetNormal(cfvf.GetArrayElem(pbVertices, iVertex)));
    }

    // nFaces
    WRITE_DWORD(pbCur, cFaces);

    if (rgdwFaces != NULL)
    {
        // MeshFace array
        for( iFace = 0; iFace < cFaces; iFace++ )
        {
            WRITE_DWORD(pbCur, 3); // nFaceVertexIndices (number of normal indices)

            pdwFace = rgdwFaces + iFace * 3;
            WRITE_DWORD(pbCur, pdwFace[0]);
            WRITE_DWORD(pbCur, pdwFace[1]);
            WRITE_DWORD(pbCur, pdwFace[2]);
        }
    }
    else
    {
        // MeshFace array
        for( iFace = 0; iFace < cFaces; iFace++ )
        {
            WRITE_DWORD(pbCur, 3); // nFaceVertexIndices (number of normal indices)

            pwFace = rgwFaces + iFace * 3;
            WRITE_DWORD(pbCur, pwFace[0]);
            WRITE_DWORD(pbCur, pwFace[1]);
            WRITE_DWORD(pbCur, pwFace[2]);
        }
    }

    hr = pxofsave->CreateDataObject(TID_D3DRMMeshNormals,
                                    NULL,
                                    NULL,
                                    cbSize,
                                    pbData,
                                    &pDataObject
                                    );
    if (FAILED(hr))
    {
        OutputDebugString(L"Failed to create x file data object!");
        goto e_Exit;
    }

    hr = pParent->AddDataObject(pDataObject);
    if (FAILED(hr))
    {
        OutputDebugString(L"Failed to add x file data object!");
        goto e_Exit;
    }

    // falling through
e_Exit:
    GXRELEASE(pDataObject);
    delete []pbData;
    return hr;
}

HRESULT
AddMaterial
    (
    LPDIRECTXFILESAVEOBJECT pxofsave,
    CONST D3DXMATERIAL *pMaterial,
    DWORD xFormat,
    LPDIRECTXFILEDATA pParent
    )
{
    LPDIRECTXFILEDATA pDataObject = NULL;
    LPDIRECTXFILEDATA pTextureObject = NULL;
    LPBYTE pbData = NULL;
    DWORD          cbSize;
    LPBYTE pbCur = NULL;
    HRESULT    hr = S_OK;
    char *szTextureFilename = NULL;
    DWORD cchFilename;

    GXASSERT(pMaterial != NULL);
    GXASSERT(pxofsave != NULL);

    cbSize = 4*sizeof(float) // colorRGBA
             + sizeof(float) //power
             + 3*sizeof(float) //specularColor
             + 3*sizeof(float); //emissiveColor

    pbCur = pbData = new BYTE[cbSize];
    if (pbData == NULL)
    {
        OutputDebugString(L"Out of memory!");
        hr = E_OUTOFMEMORY;
        goto e_Exit;
    }

    //RGBA
    WRITE_RGBA(pbCur, pMaterial->MatD3D.Diffuse);
    //power
    WRITE_FLOAT(pbCur, pMaterial->MatD3D.Power);
    // specular color
    WRITE_RGB(pbCur, pMaterial->MatD3D.Specular);
    // emissiveColor (ambient in 3DS assumed.. is this valid?)
    WRITE_RGB(pbCur, pMaterial->MatD3D.Ambient);

    hr = pxofsave->CreateDataObject(TID_D3DRMMaterial,
                                    NULL,
                                    NULL,
                                    cbSize,
                                    pbData,
                                    &pDataObject
                                    );
    if (FAILED(hr))
    {
        OutputDebugString(L"Failed to create x file data object!");
        goto e_Exit;
    }

    hr = pParent->AddDataObject(pDataObject);
    if (FAILED(hr))
    {
        OutputDebugString(L"Failed to add x file data object!");
        goto e_Exit;
    }

    // if text format, insert '\\', else just add texture file name
    if (pMaterial->pTextureFilename != NULL)
    {
        char *pchCur;
        char *pchDest;
        DWORD cBackslashes;

        cchFilename = strlen(pMaterial->pTextureFilename) + 1;
        if (xFormat == DXFILEFORMAT_TEXT)
        {
            pchCur = pMaterial->pTextureFilename;
            cBackslashes = 0;
            while (*pchCur != '\0')
            {
                if (*pchCur == '\\')
                    cBackslashes++;

                pchCur += 1;
            }

            szTextureFilename = new char[cchFilename + cBackslashes];
            if (szTextureFilename == NULL)
            {
                hr = E_OUTOFMEMORY;
                goto e_Exit;
            }

            pchDest = szTextureFilename;
            pchCur = pMaterial->pTextureFilename;
            while (*pchCur != '\0')
            {
                *pchDest = *pchCur;

                if (*pchCur == '\\')
                {
                    pchDest += 1;
                    *pchDest = *pchCur;
                }

                pchCur += 1;
                pchDest += 1;
            }
            *pchDest = '\0';
        }
        else  // binary, just copy the filename... technically doesn't need to be copied, but cleans up exit code
        {
            szTextureFilename = new char[cchFilename];
            if (szTextureFilename == NULL)
            {
                hr = E_OUTOFMEMORY;
                goto e_Exit;
            }
            memcpy(szTextureFilename, pMaterial->pTextureFilename, sizeof(char) * cchFilename);
        }

        cbSize = sizeof(TCHAR**);

        hr = pxofsave->CreateDataObject(TID_D3DRMTextureFilename,
                                        NULL,
                                        NULL,
                                        cbSize,
                                        &szTextureFilename,
                                        &pTextureObject
                                        );
        if (FAILED(hr))
        {
            OutputDebugString(L"Failed to create x file data object!");
            goto e_Exit;
        }

        hr = pDataObject->AddDataObject(pTextureObject);
        if (FAILED(hr))
        {
            OutputDebugString(L"Failed to add x file data object!");
            goto e_Exit;
        }
    }


    // falling through
e_Exit:
    // UNDONE UNDONE - need to figure how how to delete the texturefilename later
    //delete []szTextureFilename;
    GXRELEASE(pTextureObject);
    GXRELEASE(pDataObject);
    delete []pbData;
    return hr;
}

HRESULT AddTextureCoordinates
    (
    LPDIRECTXFILESAVEOBJECT pxofsave,
    PBYTE pbVertices,
    DWORD cVertices,
    DXCrackFVF &cfvf,
    LPDIRECTXFILEDATA pParent
    )
{
    LPDIRECTXFILEDATA pDataObject = NULL;
    LPBYTE pbData = NULL;
    DWORD          cbSize;
    LPBYTE pbCur = NULL;
    HRESULT    hr = S_OK;
    DWORD iVertex;
    D3DXVECTOR2 *pvTexPos;

    GXASSERT(pxofsave != NULL);
    GXASSERT(pbVertices != NULL);
    GXASSERT(pParent != NULL);
    GXASSERT(cVertices > 0);

    cbSize = sizeof(DWORD) //nTextureCoords
             + cVertices * 2 * sizeof(float); //texture coords

    pbCur = pbData = new BYTE[cbSize];
    if (pbData == NULL)
    {
        hr = E_OUTOFMEMORY;
        goto e_Exit;
    }

    WRITE_DWORD(pbCur, cVertices); //nTextureCoords

    for (iVertex = 0; iVertex < cVertices; iVertex++)
    {
        pvTexPos = cfvf.PuvGetTex1(cfvf.GetArrayElem(pbVertices, iVertex));

        WRITE_FLOAT(pbCur, pvTexPos->x); //u
        WRITE_FLOAT(pbCur, pvTexPos->y); //v
    }

    hr = pxofsave->CreateDataObject(TID_D3DRMMeshTextureCoords,
                                    NULL,
                                    NULL,
                                    cbSize,
                                    pbData,
                                    &pDataObject
                                    );
    if (FAILED(hr))
    {
        OutputDebugString(L"Failed to create x file data object!");
        goto e_Exit;
    }

    hr = pParent->AddDataObject(pDataObject);
    if (FAILED(hr))
    {
        OutputDebugString(L"Failed to add x file data object!");
        goto e_Exit;
    }

    // falling through
e_Exit:
    GXRELEASE(pDataObject);
    delete []pbData;

    return hr;
}

HRESULT AddVertexColors
    (
    LPDIRECTXFILESAVEOBJECT pxofsave,
    PBYTE pbVertices,
    DWORD cVertices,
    DXCrackFVF &cfvf,
    LPDIRECTXFILEDATA pParent
    )
{
    LPDIRECTXFILEDATA pDataObject = NULL;
    LPBYTE pbData = NULL;
    DWORD cbSize;
    LPBYTE pbCur = NULL;
    HRESULT    hr = S_OK;
    DWORD iVertex;
    D3DXCOLOR color;

    GXASSERT(pxofsave != NULL);
    GXASSERT(pbVertices != NULL);
    GXASSERT(pParent != NULL);
    GXASSERT(cVertices > 0);

    cbSize = sizeof(DWORD) //cVertexColorEntries
             + cVertices * 5 * sizeof(float); //indexed color entries(index + RGBA)

    pbCur = pbData = new BYTE[cbSize];
    if (pbData == NULL)
    {
        hr = E_OUTOFMEMORY;
        goto e_Exit;
    }

    WRITE_DWORD(pbCur, cVertices); //cVertexColorEntries

    for (iVertex = 0; iVertex < cVertices; iVertex++)
    {
        color = D3DXCOLOR(cfvf.ColorGetDiffuse(cfvf.GetArrayElem(pbVertices, iVertex)));

        WRITE_DWORD(pbCur, iVertex); // index
        WRITE_FLOAT(pbCur, color.r); // r
        WRITE_FLOAT(pbCur, color.g); // g
        WRITE_FLOAT(pbCur, color.b); // b
        WRITE_FLOAT(pbCur, color.a); // a
    }

    hr = pxofsave->CreateDataObject(TID_D3DRMMeshVertexColors,
                                    NULL,
                                    NULL,
                                    cbSize,
                                    pbData,
                                    &pDataObject
                                    );
    if (FAILED(hr))
    {
        OutputDebugString(L"Failed to create x file data object!");
        goto e_Exit;
    }

    hr = pParent->AddDataObject(pDataObject);
    if (FAILED(hr))
    {
        OutputDebugString(L"Failed to add x file data object!");
        goto e_Exit;
    }

    // falling through
e_Exit:
    GXRELEASE(pDataObject);
    delete []pbData;

    return hr;
}

HRESULT AddVertexDuplicationIndices
    (
    LPDIRECTXFILESAVEOBJECT pxofsave,
    LPD3DXMESH ptmMesh,
    CONST DWORD *rgdwAdjacency,
    LPDIRECTXFILEDATA pParent
    )
{
    LPDIRECTXFILEDATA pDataObject = NULL;
    PBYTE          pbData = NULL;
    PBYTE          pbCur = NULL;
    DWORD          cbSize;
    DWORD          cVertices;
    DWORD          cVerticesBeforeDuplication;
    HRESULT        hr = S_OK;
    DWORD          *rgiPointRep;
    DWORD          iVertex;

    GXASSERT(pxofsave != NULL);
    GXASSERT(ptmMesh != NULL);
    GXASSERT(pParent != NULL);

    cVertices = ptmMesh->GetNumVertices();
    cbSize = sizeof(DWORD) //nIndices
             + sizeof(DWORD) //nVerticesBeforeDuplication
             + cVertices*sizeof(DWORD); // array of indices

    pbCur = pbData = new BYTE[cbSize];
    if (pbData == NULL)
    {
        hr = E_OUTOFMEMORY;
        goto e_Exit;
    }

    // the point reps go immediately after the two counts
    rgiPointRep = (DWORD*)(pbCur + sizeof(DWORD) * 2);

    // compute the point reps directly into the buffer
    hr = ptmMesh->ConvertAdjacencyToPointReps(rgdwAdjacency, rgiPointRep);
    if (FAILED(hr))
        goto e_Exit;

    // how many "logical" vertices are there
    cVerticesBeforeDuplication = 0;
    for (iVertex = 0; iVertex < cVertices; iVertex++)
    {
        if (rgiPointRep[iVertex] == iVertex)
        {
            cVerticesBeforeDuplication += 1;
        }
    }

    // if the point rep array is "identity", then there is no reason to save it
    if (cVerticesBeforeDuplication == cVertices)
    {
        // skip saving an identity point rep array
        goto e_Exit;
    }

    WRITE_DWORD(pbCur, cVertices) // nIndices;
    WRITE_DWORD(pbCur, cVerticesBeforeDuplication) // nVerticesBeforeDuplication

    hr = pxofsave->CreateDataObject(DXFILEOBJ_VertexDuplicationIndices,
                                    NULL,
                                    NULL,
                                    cbSize,
                                    pbData,
                                    &pDataObject
                                    );
    if (FAILED(hr))
    {
        OutputDebugString(L"Failed to create x file data object!");
        goto e_Exit;
    }

    hr = pParent->AddDataObject(pDataObject);
    if (FAILED(hr))
    {
        OutputDebugString(L"Failed to add x file data object!");
        goto e_Exit;
    }

    // falling through
e_Exit:
    GXRELEASE(pDataObject);

    delete []pbData;
    return hr;
}

HRESULT AddFaceMaterials
    (
    LPDIRECTXFILESAVEOBJECT pxofsave,
    PDWORD rgiAttribIds,
    DWORD cFaces,
    CONST D3DXMATERIAL *rgMaterials,
    DWORD cMaterials,
    DWORD xFormat,
    LPDIRECTXFILEDATA pParent
    )
{
    HRESULT     hr = S_OK;
    LPDIRECTXFILEDATA pDataObject = NULL;
    LPBYTE      pbData = NULL;
    LPBYTE      pbCur = NULL;
    DWORD       cbSize;
    DWORD       iFace;
    //DWORD       iVertex;
    DWORD       iMaterial;

    GXASSERT(pParent != NULL);
    GXASSERT(rgMaterials != NULL);
    GXASSERT(pxofsave != NULL);
    GXASSERT(rgiAttribIds != NULL);
    GXASSERT(cFaces > 0 && cMaterials > 0);

    cbSize = sizeof(DWORD) // nMaterials
                + sizeof(DWORD) // nFaceIndexes
                + cFaces * sizeof(DWORD); // face indexes

    pbCur = pbData = new BYTE[cbSize];
    if (pbData == NULL)
    {
        return E_OUTOFMEMORY;
    }

    WRITE_DWORD(pbCur, cMaterials); // nMaterials

    // face indexes
    WRITE_DWORD(pbCur, cFaces); // nFaceIndexes
    for( iFace = 0; iFace < cFaces; iFace++ )
    {
        WRITE_DWORD(pbCur, rgiAttribIds[iFace]);
    }

    hr = pxofsave->CreateDataObject(TID_D3DRMMeshMaterialList,
                                    NULL,
                                    NULL,
                                    cbSize,
                                    pbData,
                                    &pDataObject
                                    );
    if (FAILED(hr))
    {
        OutputDebugString(L"Failed to create x file data object!");
        goto e_Exit;
    }

    hr = pParent->AddDataObject(pDataObject);
    if (FAILED(hr))
    {
        OutputDebugString(L"Failed to add x file data object!");
        goto e_Exit;
    }

    for (iMaterial = 0; iMaterial < cMaterials; iMaterial++)
    {
        hr = AddMaterial(pxofsave, &rgMaterials[iMaterial], xFormat, pDataObject);
        if (FAILED(hr))
            goto e_Exit;
    }

    // falling through
e_Exit:
    GXRELEASE(pDataObject);

    delete []pbData;
    return hr;
}

HRESULT AddOtherFVFData
    (
    LPDIRECTXFILESAVEOBJECT pxofsave,
    PBYTE pbPoints,
    DWORD cVertices,
    DWORD dwFVF,
    DXCrackFVF &cfvfOrig,
    LPDIRECTXFILEDATA pParent
    )
{
    HRESULT     hr = S_OK;
    LPDIRECTXFILEDATA pDataObject = NULL;
    LPBYTE      pbData = NULL;
    LPBYTE      pbCur = NULL;
    DWORD       cbSize;
    DWORD       iVertex;
    DWORD       cbBytesPerVertex;
    DXCrackFVF  cfvf(D3DFVF_XYZ);
    DWORD       iWeight;
    DWORD       iTexCoord;
    DWORD       dwFVFOther;
    PBYTE       pbCurVertex;
    BOOL        bCopyTex1;
    DWORD       rgdwTexCoordSizes[8];

    GXASSERT(pxofsave != NULL);
    GXASSERT(pbPoints != NULL);
    GXASSERT(cVertices > 0);
    GXASSERT(pParent != NULL);

    dwFVFOther = dwFVF;

    // remove data added other places in the xfile
    dwFVFOther &= ~D3DFVF_NORMAL;
    dwFVFOther &= ~D3DFVF_DIFFUSE;

    // remove the texture if it is 2d
    bCopyTex1 = cfvfOrig.BTex1();
    if (cfvfOrig.BTex1() && (cfvfOrig.CbTexCoordSize(0) == 8))
    {
        dwFVFOther -= D3DFVF_TEX1;
        bCopyTex1 = FALSE;
    }
    cfvfOrig.GetTexCoordSizes(rgdwTexCoordSizes);

    cfvf = DXCrackFVF(dwFVFOther);

    // even though the position is specified by the FVF, it is not stored there
    cbBytesPerVertex = cfvf.m_cBytesPerVertex - 12;

    // if nothing left, skip adding this structure
    if (cbBytesPerVertex == 0)
    {
        goto e_Exit;
    }

    // had better be a multiple of 4
    if (cbBytesPerVertex % 4 != 0)
    {
        hr = D3DERR_INVALIDCALL;
        goto e_Exit;
    }

    cbSize = sizeof(DWORD) // nMaterials
                + sizeof(DWORD) // nFaceIndexes
                + cVertices * cbBytesPerVertex; // face indexes

    pbCur = pbData = new BYTE[cbSize];
    if (pbData == NULL)
    {
        return E_OUTOFMEMORY;
    }

    WRITE_DWORD(pbCur, dwFVFOther);
    WRITE_DWORD(pbCur, cVertices * cbBytesPerVertex / 4); // nFaceIndexes
    pbCurVertex = pbPoints;
    for( iVertex = 0; iVertex < cVertices; iVertex++ )
    {
        // skip position
        pbCurVertex += sizeof(D3DXVECTOR3);

        for (iWeight = 0; iWeight < cfvfOrig.CWeights(); iWeight++)
        {
            WRITE_DWORD(pbCur, *(DWORD*)pbCurVertex);
            pbCurVertex += sizeof(DWORD);
        }

        if (cfvfOrig.BNormal())
        {
            pbCurVertex += sizeof(D3DXVECTOR3);
        }

        if (cfvfOrig.BDiffuse())
        {
            if (cfvf.BDiffuse())
                WRITE_DWORD(pbCur, *(DWORD*)pbCurVertex);

            pbCurVertex += sizeof(DWORD);
        }

        if (cfvfOrig.BSpecular())
        {
            WRITE_DWORD(pbCur, *(DWORD*)pbCurVertex);
            pbCurVertex += sizeof(DWORD);
        }

        // first either skip or copy the first texture coordinate
        if (bCopyTex1)
        {
            memcpy(pbCur, pbCurVertex, rgdwTexCoordSizes[0]);
            pbCur += rgdwTexCoordSizes[0];
            pbCurVertex += rgdwTexCoordSizes[0];
        }
        else
        {
            pbCurVertex += rgdwTexCoordSizes[0];
        }

        // next copy the other texture coordinates
        if (cfvfOrig.CTexCoords() > 1)
        {
            for (iTexCoord = 1; iTexCoord < cfvfOrig.CTexCoords(); iTexCoord++)
            {
                memcpy(pbCur, pbCurVertex, rgdwTexCoordSizes[iTexCoord]);
                pbCur += rgdwTexCoordSizes[iTexCoord];
                pbCurVertex += rgdwTexCoordSizes[iTexCoord];
            }
        }
    }

    hr = pxofsave->CreateDataObject(DXFILEOBJ_FVFData,
                                    NULL,
                                    NULL,
                                    cbSize,
                                    pbData,
                                    &pDataObject
                                    );
    if (FAILED(hr))
    {
        OutputDebugString(L"Failed to create x file data object!");
        goto e_Exit;
    }

    hr = pParent->AddDataObject(pDataObject);
    if (FAILED(hr))
    {
        OutputDebugString(L"Failed to add x file data object!");
        goto e_Exit;
    }

    // falling through
e_Exit:
    GXRELEASE(pDataObject);

    delete []pbData;
    return hr;
}

const GUID* x_rgTemplateIds[] = {&DXFILEOBJ_VertexDuplicationIndices, &DXFILEOBJ_FVFData};
const GUID* x_rgPMTemplateIds[] = {&DXFILEOBJ_PMAttributeRange, &DXFILEOBJ_PMVSplitRecord, &DXFILEOBJ_PMInfo};

// implements mesh saving with pm data
//
HRESULT WINAPI D3DXSaveMeshToXEx
    (
    char *szFilename,
    LPD3DXMESH pMesh,
    CONST DWORD *rgdwAdjacency,
    CONST LPD3DXMATERIAL rgMaterials,
    DWORD cMaterials,
    DWORD xFormat,
    PBYTE pbPMData,
    DWORD cbPMData
    )
{
#if 1
    return E_NOTIMPL;
#else
    HRESULT hr = S_OK;

    DWORD cFaces;
    DWORD cVertices;
    DWORD iVertex;
    DWORD iFace;
    PBYTE pbPoints = NULL;
    PBYTE pbPoint;
    LPDIRECT3DVERTEXBUFFER8 pVertexBuffer = NULL;
    UINT16 *rgwFaces = NULL;
    DWORD *rgdwFaces = NULL;
    LPDIRECT3DINDEXBUFFER8 pIndexBuffer = NULL;
    LPDIRECTXFILEDATA pDataObject = NULL;
    LPDIRECTXFILEDATA pPMDataObject = NULL;
    LPDIRECTXFILE pxofapi = NULL;
    LPDIRECTXFILESAVEOBJECT pxofsave = NULL;
    PBYTE pbCur;
    PBYTE pbData = NULL;
    DWORD cbSize;
    DWORD *rgiAttribIds = NULL;
    DXCrackFVF cfvf(D3DFVF_XYZ);
    HINSTANCE hXof;
    LPDIRECTXFILECREATE pfnDirectXFileCreate;


    if ((pMesh == NULL) || ((rgMaterials == NULL) && (cMaterials > 0)))
    {
        hr = D3DERR_INVALIDCALL;
    }

    hr = pMesh->GetVertexBuffer(&pVertexBuffer);
    if (FAILED(hr))
        goto e_Exit;

    hr = pVertexBuffer->Lock(0,0, &pbPoints, 0 );
    if (FAILED(hr))
        goto e_Exit;

    hr = pMesh->GetIndexBuffer(&pIndexBuffer);
    if (FAILED(hr))
        goto e_Exit;

    hr = pMesh->LockAttributeBuffer(D3DLOCK_READONLY, &rgiAttribIds);
    if (FAILED(hr))
        goto e_Exit;

    hr = pIndexBuffer->Lock(0,0, (PBYTE*)&rgwFaces, 0 );
    if (FAILED(hr))
        goto e_Exit;

    if (pMesh->GetOptions() & D3DXMESH_32BIT)
    {
        rgdwFaces = (DWORD*)rgwFaces;
    }

    cVertices = pMesh->GetNumVertices();
    cFaces = pMesh->GetNumFaces();
    cbSize = sizeof(DWORD) // nVertices
             + cVertices*sizeof(float)*3 // vertices
             + sizeof(DWORD) // nFaces
             + cFaces*(sizeof(DWORD) /*nFaceVertexIndices*/
                            + sizeof(DWORD)*3 /*faceVertexIndices*/); // faces

    cfvf = DXCrackFVF(pMesh->GetFVF());

    pbCur = pbData = new BYTE[cbSize];
    if( NULL == pbData )
    {
        hr = E_OUTOFMEMORY;
        goto e_Exit;;
    }

    // write cVertices
    WRITE_DWORD(pbCur, cVertices);

    for (iVertex = 0; iVertex < cVertices; iVertex++)
    {
        pbPoint = cfvf.GetArrayElem(pbPoints, iVertex);
        WRITE_VECTOR3(pbCur, *cfvf.PvGetPosition(pbPoint));
    }

    // write cFaces
    WRITE_DWORD(pbCur, cFaces);

    if (rgdwFaces == NULL)
    {
        for (iFace = 0; iFace < cFaces; iFace++)
        {
            WRITE_DWORD(pbCur, 3);

            WRITE_DWORD(pbCur, (DWORD)rgwFaces[iFace * 3 + 0]);
            WRITE_DWORD(pbCur, (DWORD)rgwFaces[iFace * 3 + 1]);
            WRITE_DWORD(pbCur, (DWORD)rgwFaces[iFace * 3 + 2]);
        }
    }
    else
    {
        for (iFace = 0; iFace < cFaces; iFace++)
        {
            WRITE_DWORD(pbCur, 3);

            WRITE_DWORD(pbCur, rgdwFaces[iFace * 3 + 0]);
            WRITE_DWORD(pbCur, rgdwFaces[iFace * 3 + 1]);
            WRITE_DWORD(pbCur, rgdwFaces[iFace * 3 + 2]);
        }
    }

    // Create Xfile parser
    if(!(hXof = (HINSTANCE) GetModuleHandle("d3dxof.dll")) &&
       !(hXof = (HINSTANCE) LoadLibrary("d3dxof.dll")))
    {
        hr = HRESULT_FROM_WIN32(GetLastError());
        goto e_Exit;
    }

    if(!(pfnDirectXFileCreate = (LPDIRECTXFILECREATE) GetProcAddress(hXof, "DirectXFileCreate")))
    {
        hr = HRESULT_FROM_WIN32(GetLastError());
        goto e_Exit;
    }

    hr = pfnDirectXFileCreate(&pxofapi);
    if (FAILED(hr))
        goto e_Exit;

    // Register templates for d3drm.
    hr = pxofapi->RegisterTemplates((LPVOID)D3DRM_XTEMPLATES,
                                    D3DRM_XTEMPLATE_BYTES);
    if (FAILED(hr))
        goto e_Exit;

    hr = pxofapi->RegisterTemplates((LPVOID)XSKINEXP_TEMPLATES,
                                    strlen(XSKINEXP_TEMPLATES));
    if (FAILED(hr))
        goto e_Exit;

    hr = pxofapi->RegisterTemplates((LPVOID)XEXTENSIONS_TEMPLATES,
                                    strlen(XEXTENSIONS_TEMPLATES));
    if (FAILED(hr))
        goto e_Exit;

    hr = pxofapi->CreateSaveObject(szFilename,    // filename
                                   xFormat,  // binary or text
                                   &pxofsave);
    if (FAILED(hr))
        goto e_Exit;

    hr = pxofsave->SaveTemplates(2, x_rgTemplateIds);
    if (FAILED(hr))
        goto e_Exit;

    if (pbPMData != NULL)
    {
        hr = pxofsave->SaveTemplates(3, x_rgPMTemplateIds);
        if (FAILED(hr))
            goto e_Exit;
    }

    hr = pxofsave->CreateDataObject(TID_D3DRMMesh,
                                    NULL,
                                    NULL,
                                    cbSize,
                                    pbData,
                                    &pDataObject
                                    );
    if (FAILED(hr))
        goto e_Exit;

    if (cfvf.BNormal())
    {
        hr = AddNormals(pxofsave, pbPoints, cVertices, rgwFaces, rgdwFaces, cFaces, cfvf, pDataObject);
        if (FAILED(hr))
            goto e_Exit;
    }

    pIndexBuffer->Unlock();
    rgwFaces = NULL;

    // if there is a tex1 that has a sizeof 2 floats, then output it here, 
    //    otherwise it will go in the other fvf data section
    if (cfvf.BTex1() && (cfvf.CbTexCoordSize(0) == 8))
    {
        hr = AddTextureCoordinates(pxofsave, pbPoints, cVertices, cfvf, pDataObject);
        if (FAILED(hr))
            goto e_Exit;
    }

    if (cfvf.BDiffuse())
    {
        hr = AddVertexColors(pxofsave, pbPoints, cVertices, cfvf, pDataObject);
        if (FAILED(hr))
            goto e_Exit;
    }

    if (cMaterials > 0)
    {
        hr = AddFaceMaterials(pxofsave, rgiAttribIds, cFaces, rgMaterials, cMaterials, xFormat, pDataObject);
        if (FAILED(hr))
            goto e_Exit;
    }

    // add adjacency info if present
    if (rgdwAdjacency != NULL)
    {
        hr = AddVertexDuplicationIndices(pxofsave, pMesh, rgdwAdjacency, pDataObject);
        if (FAILED(hr))
            goto e_Exit;
    }

    hr = AddOtherFVFData(pxofsave, pbPoints, cVertices, pMesh->GetFVF(), cfvf, pDataObject);
    if (FAILED(hr))
        goto e_Exit;

    // if PM data provided, save it in a PMInfo group
    if (pbPMData != NULL)
    {
        GXASSERT(cbPMData > 0);

        hr = pxofsave->CreateDataObject(DXFILEOBJ_PMInfo,
                                        NULL,
                                        NULL,
                                        cbPMData,
                                        pbPMData,
                                        &pPMDataObject
                                        );
        if (FAILED(hr))
            goto e_Exit;

        hr = pDataObject->AddDataObject(pPMDataObject);
        if (FAILED(hr))
        {
            goto e_Exit;
        }
    }

    hr = pxofsave->SaveData(pDataObject);
    if (FAILED(hr))
        goto e_Exit;

e_Exit:
    if (pbPoints != NULL)
    {
        GXASSERT(pVertexBuffer != NULL);
        pVertexBuffer->Unlock();
    }
    GXRELEASE(pVertexBuffer);

    if (rgwFaces != NULL)
    {
        GXASSERT(pIndexBuffer != NULL);
        pIndexBuffer->Unlock();
    }
    GXRELEASE(pIndexBuffer);

    if (rgiAttribIds != NULL)
    {
        pMesh->UnlockAttributeBuffer();
    }

    delete []pbData;
    GXRELEASE(pDataObject);
    GXRELEASE(pPMDataObject);
    GXRELEASE(pxofapi);
    GXRELEASE(pxofsave);

    return hr;
#endif
}

// fall through to the extended save mesh, used to combine PM save and normal save
HRESULT WINAPI D3DXSaveMeshToX
    (
    char *szFilename,
    LPD3DXMESH pMesh,
    CONST DWORD *rgdwAdjacency,
    CONST LPD3DXMATERIAL rgMaterials,
    DWORD cMaterials,
    DWORD xFormat
    )
{
    return D3DXSaveMeshToXEx(szFilename, pMesh, rgdwAdjacency, rgMaterials, cMaterials, xFormat, NULL, 0);
}
