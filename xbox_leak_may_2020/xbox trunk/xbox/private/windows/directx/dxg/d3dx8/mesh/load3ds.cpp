/*//////////////////////////////////////////////////////////////////////////////
//
// File: load3ds.cpp
//
// Copyright (C) 1999 Microsoft Corporation. All Rights Reserved.
//
// @@BEGIN_MSINTERNAL
//
// History:
// -@-          (craigp)    - created
// -@- 08/26/99 (mikemarr)  - started comment history
//                          - replace gxbasetype.h with gxmathcore.h
// -@- 09/23/99 (mikemarr)  - replaced <> with "" on #includes
//
// @@END_MSINTERNAL
//
//////////////////////////////////////////////////////////////////////////////*/
#include "pchmesh.h"

#include "loadutil.h"

#include "3ds/3dsftk.h"
#define ON_ERROR_FAIL(hrNew) {if(ftkerr3ds && !ignoreftkerr3ds) { hr = hrNew; goto e_Exit;} }

UINT ISmoothingGroupID(UINT iSmoothingGroup)
{
    UINT iCur;
    UINT i;

    if (iSmoothingGroup == 0)
        return 0;

    iCur = iSmoothingGroup;
    for (i = 0; i < 32; i++)
    {
        if (iCur & 0x1)
            break;

        iCur = iCur >> 1;
    }

    GXASSERT(i <= 31);
    GXASSERT(iCur & 0x1);

    return i+1;
}

HRESULT WINAPI
D3DXLoadMeshFrom3ds(char *szFilename, DWORD options, DWORD fvf,
                    LPDIRECT3DDEVICE8 pD3DDevice,
                    LPD3DXMESH *ppMesh, LPD3DXBUFFER *ppbufAdjacency,
                    LPD3DXBUFFER *ppbufMaterials, PDWORD pcMaterials)
{
    file3ds *file= NULL;
    database3ds *db = NULL;
    ulong3ds cMeshes;
    ulong3ds iMesh;
    ulong3ds cVertices;
    ulong3ds cFaces;
    mesh3ds **rgpMesh = NULL;
    mesh3ds *pMesh;
    ulong3ds iVertex;
    ulong3ds iVertexIndex;
    point3ds *pvCur;
    ulong3ds cVerticesCur;
    ulong3ds iFaceIndex;
    ulong3ds iVertexOffset;
    face3ds *pfaceCur;
    ulong3ds cFacesCur;
    ulong3ds iFace;
    ulong3ds cMaterials = 0;
    ulong3ds iMaterial;
    material3ds *pmat;
    material3ds **rgMaterials = NULL;
    HRESULT hr = S_OK;
    LPD3DXMATERIAL rgmatMaterials = NULL;
    char *szTexName;
    char *szName;
    ulong3ds cMatFaces;
    ushort3ds *rgiMatFaces;
    ulong3ds i;
    UINT cchTexName;
    //IGXAttributeBundleDX7 *pattrCur;
    //IGXAttributeBundleDX7 *pattrEmpty = NULL;

    PBYTE       pvPoints = NULL;
    PBYTE       pvCurPoint = NULL;

    SLoadedFace *rglfFaces = NULL;
    SLoadVertex *rglvLoaded = NULL;

    SLoadedFace *plfFace;


    if ((ppMesh == NULL) || (szFilename == NULL))
    {
        hr = D3DERR_INVALIDCALL;
        goto e_Exit;
    }

    file = OpenFile3ds(szFilename, "rw");
    ON_ERROR_FAIL(E_FAIL);

    InitDatabase3ds(&db);
    ON_ERROR_FAIL(E_FAIL);

    CreateDatabase3ds(file, db);
    ON_ERROR_FAIL(E_FAIL);

    cMaterials = GetMaterialCount3ds(db);
    ON_ERROR_FAIL(E_FAIL);

    cMeshes = GetMeshCount3ds(db);
    ON_ERROR_FAIL(E_FAIL);

    //printf("# of meshes: %d\n", cMeshes);
    //fprintf(stderr, "# of meshes: %d\n", cMeshes);

    //fprintf(stderr, "# of materials: %d\n", cMaterials);

    rgMaterials = new material3ds*[cMaterials];
    rgmatMaterials = new D3DXMATERIAL[cMaterials];
    if ((rgMaterials == NULL) || (rgmatMaterials == NULL))
    {
        hr = E_OUTOFMEMORY;
        goto e_Exit;
    }
    memset(rgMaterials, 0, sizeof(material3ds*) * cMaterials);
    memset(rgmatMaterials, 0, sizeof(D3DXMATERIAL) * cMaterials);

    // first retrieve all the materials
    for (iMaterial = 0; iMaterial < cMaterials; iMaterial++)
    {
        pmat = NULL;
        GetMaterialByIndex3ds(db, iMaterial, &pmat);
        ON_ERROR_FAIL(E_FAIL);

        rgMaterials[iMaterial] = pmat;
    }

    for (iMaterial = 0; iMaterial < cMaterials; iMaterial++)
    {
        rgmatMaterials[iMaterial].MatD3D.Diffuse.r = rgMaterials[iMaterial]->diffuse.r;
        rgmatMaterials[iMaterial].MatD3D.Diffuse.g = rgMaterials[iMaterial]->diffuse.g;
        rgmatMaterials[iMaterial].MatD3D.Diffuse.b = rgMaterials[iMaterial]->diffuse.b;
        rgmatMaterials[iMaterial].MatD3D.Diffuse.a = 1.0f;

        rgmatMaterials[iMaterial].MatD3D.Specular.r = rgMaterials[iMaterial]->specular.r;
        rgmatMaterials[iMaterial].MatD3D.Specular.g = rgMaterials[iMaterial]->specular.g;
        rgmatMaterials[iMaterial].MatD3D.Specular.b = rgMaterials[iMaterial]->specular.b;
        rgmatMaterials[iMaterial].MatD3D.Specular.a = 1.0f;

        rgmatMaterials[iMaterial].MatD3D.Ambient.r = rgMaterials[iMaterial]->ambient.r;
        rgmatMaterials[iMaterial].MatD3D.Ambient.g = rgMaterials[iMaterial]->ambient.g;
        rgmatMaterials[iMaterial].MatD3D.Ambient.b = rgMaterials[iMaterial]->ambient.b;
        rgmatMaterials[iMaterial].MatD3D.Ambient.a = 1.0f;

        szTexName = rgMaterials[iMaterial]->texture.map.name;
        cchTexName = strlen(szTexName) + 1;
        if (cchTexName > 0)
        {
            // allocate a buffer to store the texture name in
            rgmatMaterials[iMaterial].pTextureFilename = new char[cchTexName];
            if (rgmatMaterials[iMaterial].pTextureFilename == NULL)
            {
                hr = E_OUTOFMEMORY;
                goto e_Exit;
            }

            // copy the texture filename into the prepared buffer
            strcpy(rgmatMaterials[iMaterial].pTextureFilename, szTexName);
        }
    }

    rgpMesh = new mesh3ds*[cMeshes];
    if (rgpMesh == NULL)
    {
        hr = E_OUTOFMEMORY;
        goto e_Exit;
    }

    for (iMesh = 0; iMesh < cMeshes; iMesh++)
    {
        pMesh = NULL;
        GetMeshByIndex3ds(db, iMesh, &pMesh);
        ON_ERROR_FAIL(E_FAIL);

        if (_stricmp(pMesh->name, "SUELO") == 0)
        {
            pMesh = NULL;
        }

        rgpMesh[iMesh] = pMesh;
    }

    cVertices = 0;
    cFaces = 0;
    for (iMesh = 0; iMesh < cMeshes; iMesh++)
    {
        if (rgpMesh[iMesh] != NULL)
        {
            cVertices += rgpMesh[iMesh]->nvertices;
            cFaces += rgpMesh[iMesh]->nfaces;
        }
    }

    // allocate memory to load data into

    hr = InitVertices(rglvLoaded, cVertices);
    if (FAILED(hr))
        goto e_Exit;

    hr = InitFaces(rglfFaces, cFaces);
    if (FAILED(hr))
        goto e_Exit;

    iVertexIndex = 0;
    for (iMesh = 0; iMesh < cMeshes; iMesh++)
    {
        if (rgpMesh[iMesh] == NULL)
            continue;

        cVerticesCur = rgpMesh[iMesh]->nvertices;
        for (iVertex = 0; iVertex < cVerticesCur; iVertex++)
        {
            pvCur = &rgpMesh[iMesh]->vertexarray[iVertex];

            rglvLoaded[iVertexIndex].m_vPos = *(D3DXVECTOR3*)pvCur;

            if (rgpMesh[iMesh]->ntextverts > 0)
            {
                rglvLoaded[iVertexIndex].m_uvTex1 = *(D3DXVECTOR2*)&rgpMesh[iMesh]->textarray[iVertex];
                rglvLoaded[iVertexIndex].m_uvTex1.y = 1 - rglvLoaded[iVertexIndex].m_uvTex1.y;
            }

            iVertexIndex += 1;
       }
   }

    iFaceIndex = 0;
    iVertexOffset = 0;
    for (iMesh = 0; iMesh < cMeshes; iMesh++)
    {
        if (rgpMesh[iMesh] == NULL)
            continue;

        cFacesCur = rgpMesh[iMesh]->nfaces;
        for (iFace = 0; iFace < cFacesCur; iFace++)
        {
            pfaceCur = &rgpMesh[iMesh]->facearray[iFace];

            rglfFaces[iFaceIndex].m_wIndices[0] = pfaceCur->v1 + iVertexOffset;
            rglfFaces[iFaceIndex].m_wIndices[1] = pfaceCur->v3 + iVertexOffset;
            rglfFaces[iFaceIndex].m_wIndices[2] = pfaceCur->v2 + iVertexOffset;

            rglfFaces[iFaceIndex].m_attr = 0;

            if (rgpMesh[iMesh]->smootharray)
            {
                rglfFaces[iFaceIndex].m_bSmoothingGroupSpecified = true;
                rglfFaces[iFaceIndex].m_iSmoothingGroup = (UINT16)(iMesh * 32 + ISmoothingGroupID(rgpMesh[iMesh]->smootharray[iFace]));
            }

            iFaceIndex += 1;
        }

        iVertexOffset += rgpMesh[iMesh]->nvertices;
   }

   iFaceIndex = 0;
   for (iMesh = 0; iMesh < cMeshes; iMesh++)
   {
        if (rgpMesh[iMesh] == NULL)
            continue;

        for (iMaterial = 0; iMaterial < rgpMesh[iMesh]->nmats; iMaterial++)
        {
            szName = rgpMesh[iMesh]->matarray[iMaterial].name;
            cMatFaces = rgpMesh[iMesh]->matarray[iMaterial].nfaces;
            rgiMatFaces = rgpMesh[iMesh]->matarray[iMaterial].faceindex;

            // lookup the index of the material
            for (i = 0; i < cMaterials; i++)
            {
                if (strcmp(rgMaterials[i]->name, szName) == 0)
                {
                    break;
                }
            }

            // check for not finding the material by name
            if (i == cMaterials)
            {
                hr = E_FAIL;
                goto e_Exit;
            }

            // now walk the index list, assigning the correct attribute bundle for
            //    the given material
            for (iFace = 0; iFace < cMatFaces; iFace++)
            {
                plfFace = &rglfFaces[iFaceIndex + rgiMatFaces[iFace]];

                plfFace->m_bAttributeSpecified = true;
                plfFace->m_attr = i;

                // UNDONE - should have option specifying whether to push
                    //   material colors down to vertices or not
                //plfFace->m_bColorSpecified = true;
                //plfFace->m_colorFace = *(D3DXCOLOR*)&rgMaterials[i]->diffuse;
            }
        }

        iFaceIndex += rgpMesh[iMesh]->nfaces;
   }

#if 0
    hr = GXCreateAttributeBundleDX7(&pattrEmpty);
    if (FAILED(hr))
    {
        goto e_Exit;
    }

    // for all faces without a texture, fill in with an attribute bundle that
    //   unsets the texture stage state
    for (iFace = 0; iFace < cFaces; iFace++)
    {
        plfFace = &rglfFaces[iFace];

        if (plfFace->m_pattr == NULL)
        {
            plfFace->m_bAttributeSpecified = true;
            plfFace->m_pattr = pattrEmpty;
        }
    }
#endif

    // now create the mesh
    hr = SetMesh(rglfFaces, cFaces, rglvLoaded, cVertices, NULL, 0, NULL, FALSE,
                        options, fvf, pD3DDevice, ppMesh, ppbufAdjacency);
    if (FAILED(hr))
        goto e_Exit;


    if (ppbufMaterials != NULL)
    {
        hr = CreateMaterialBuffer(rgmatMaterials, cMaterials, ppbufMaterials);
        if (FAILED(hr))
            goto e_Exit;
    }

    if (pcMaterials != NULL)
    {
        *pcMaterials = cMaterials;
    }

e_Exit:

    if (rgmatMaterials != NULL)
    {
        for (iMaterial = 0; iMaterial < cMaterials; iMaterial++)
        {
            delete []rgmatMaterials[iMaterial].pTextureFilename;
        }
    }

    delete []rgmatMaterials;
    delete []rgMaterials;

    delete []rglfFaces;
    delete []rglvLoaded;

    if (db != NULL)
    {
        ReleaseDatabase3ds(&db);
        //hr = E_FAIL;
        //ON_ERROR_RETURNR(E_FAIL);
    }

    if (file != NULL)
    {
        CloseFile3ds(file);
        //hr = E_FAIL;
        //ON_ERROR_RETURNR(E_FAIL);
    }

   return hr;
}

