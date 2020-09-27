/*//////////////////////////////////////////////////////////////////////////////
//
// File: tri3drawmesh.cpp
//
// Copyright (C) 1999 Microsoft Corporation. All Rights Reserved.
//
// @@BEGIN_MSINTERNAL
//
// History:
// -@-          (craigp)    - created 
// -@- 08/06/99 (mikemarr)  - prepend GX to all Luciform functions
//                          - started comment history
// -@- 08/19/99 (mikemarr)  - prepend GX to gxfmath functions
//
// @@END_MSINTERNAL
//
//////////////////////////////////////////////////////////////////////////////*/

// -------------------------------------------------------------------------------
//  function    DrawSubset
//
//   devnote    draws all the triangles in the mesh with the given attribute bundle
//                  NOTE: does NOT transform the vertices, assumes that it was done
//                      with a call to TransformVertices
//
//   returns    S_OK if success, failed otherwise
//
template <class UINT_IDX, int b16BitIndex, unsigned int UNUSED>
HRESULT
GXTri3Mesh<UINT_IDX,b16BitIndex,UNUSED>::DrawSubset(DWORD attribId)
{
    HRESULT hr = S_OK;
    UINT iFace;
    UINT cTris;
    UINT iae;
    //DWORD dwVOP;
    PBYTE pbVerticesSrc = NULL;

    GXASSERT(m_pFaces == NULL);

    if (b16BitIndex)
    {
        // if no vertices, just return ok
        if (m_pVBVertices == NULL)
        {
            GXASSERT((m_cVertices == 0) && (m_cFaces == 0));
            goto e_Exit;
        }

        m_pD3DDevice->SetVertexShader(m_dwFVF);
        m_pD3DDevice->SetStreamSource(0, m_pVBVertices, m_cBytesPerVertex);
        m_pD3DDevice->SetIndices(m_pibFaces, 0);

        // if optimized, jump straight to the triangles of the given attribute bundle
        if (m_dwOptionsInt & D3DXMESHINT_ATTRIBUTETABLE)
        {

            if ((attribId < m_caeAttributeTable) && (m_rgaeAttributeTable[attribId].AttribId == attribId))
            {
                iae = attribId;
            }
            else
            {
                // look for the correct attribute table entry to draw
                for (iae = 0; iae < m_caeAttributeTable; iae++)
                {
                    if (m_rgaeAttributeTable[iae].AttribId == attribId)
                    {
                        break;
                    }
                }
            }

            // if present in the table, then draw it
            if ((iae < m_caeAttributeTable) && (m_rgaeAttributeTable[iae].FaceCount > 0))
            {

                m_pD3DDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 
                                             m_rgaeAttributeTable[iae].VertexStart, m_rgaeAttributeTable[iae].VertexCount,
                                             m_rgaeAttributeTable[iae].FaceStart * 3, m_rgaeAttributeTable[iae].FaceCount);

            }
        }
        else  // have to work around holes and triangles with a different attribute bundle than the one to draw
        {
            cTris = 0;
            for (iFace = 0; iFace < m_cFaces; iFace++)
            {
                // if not UINT max, then a triangle is present
                if (/*(m_pFaces[iFace].m_wIndices[0] != UNUSED) && */(m_rgiAttributeIds[iFace] == attribId))
                {
                    // add one to the count of tris in this "list"
                    cTris += 1;
                }
                else // found a "hole", draw any triangles and reset the counters
                {
                    // if there are any triangles, then draw them
                    if (cTris > 0)
                    {
                        m_pD3DDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 
                                                     0, m_cVertices,
                                                     (iFace - cTris) * 3, cTris);
                    }

                    cTris = 0;
                    GXASSERT( /*(m_pFaces[iFace].m_wIndices[0] == UNUSED) || */(m_rgiAttributeIds[iFace] != attribId));
                }
            }

            // draw the last set of triangles
            if (cTris  > 0)
            {
                m_pD3DDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 
                                             0, m_cVertices,
                                             (iFace - cTris) * 3, cTris);
            }
        }
    }
    else // 32 bit indices
    {
        //UNDONE UNDONE could make faster by gathering into one larger buffer,
        //                  but that is unnecessary once we have 32 bit index buffer


        // if no vertices, just return ok
        if (m_pVBVertices == NULL)
        {
            GXASSERT((m_cVertices == 0) && (m_cFaces == 0));
            goto e_Exit;
        }

        m_pD3DDevice->SetVertexShader(m_dwFVF);

        UINT_IDX *pwFace;
        bool bBracketed = false;
        UINT iStartFace = 0;
        UINT iEndFace = m_cFaces;
        UINT iStartVertex = 0;
        UINT iEndVertex = m_cVertices;

        PBYTE pbVertices;
        PBYTE pbCur;

        pbVertices = (PBYTE)_alloca(m_cBytesPerVertex * 3);

        if (BHasAttributeTable())
        {
            // look for the correct attribute table entry to draw
            for (iae = 0; iae < m_caeAttributeTable; iae++)
            {
                if (m_rgaeAttributeTable[iae].AttribId == attribId)
                {
                    break;
                }
            }

            // if not present, then just return
            if (iae == m_caeAttributeTable)
                return S_OK;

            iStartFace = m_rgaeAttributeTable[iae].FaceStart;
            iEndFace = m_rgaeAttributeTable[iae].FaceCount;
            iStartVertex = m_rgaeAttributeTable[iae].VertexStart;
            iEndVertex = m_rgaeAttributeTable[iae].VertexCount;

            bBracketed = true;
        }

        hr = LockVB(&pbVerticesSrc);
        if (FAILED(hr))
            goto e_Exit;

        hr = LockIB((PBYTE*)&m_pFaces);
        if (FAILED(hr))
            goto e_Exit;


        for (iFace = iStartFace; iFace < iEndFace; iFace++)
        {
            pwFace = m_pFaces[iFace].m_wIndices;

            GXASSERT(pwFace[0] != UNUSED);

            if (!bBracketed && m_rgiAttributeIds[iFace] != attribId)
                continue;

            pbCur = pbVertices;
            memcpy(pbCur, pbVerticesSrc + pwFace[0] * m_cBytesPerVertex, m_cBytesPerVertex);
            pbCur += m_cBytesPerVertex;
            memcpy(pbCur, pbVerticesSrc + pwFace[1] * m_cBytesPerVertex, m_cBytesPerVertex);
            pbCur += m_cBytesPerVertex;
            memcpy(pbCur, pbVerticesSrc + pwFace[2] * m_cBytesPerVertex, m_cBytesPerVertex);
            pbCur += m_cBytesPerVertex;
        
            hr = m_pD3DDevice->DrawPrimitiveUP(D3DPT_TRIANGLELIST, 1, pbVertices, m_cBytesPerVertex);
            if (FAILED(hr))
                goto e_Exit;                
        }
    }

e_Exit:
    if (!b16BitIndex)
    {
        if (pbVerticesSrc != NULL)
        {
            UnlockVB();
        }

        if (m_pFaces != NULL)
        {
            UnlockIB();
            m_pFaces = NULL;
        }
    }

    return hr;
}

