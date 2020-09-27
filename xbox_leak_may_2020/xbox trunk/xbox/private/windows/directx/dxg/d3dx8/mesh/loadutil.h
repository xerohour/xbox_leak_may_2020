#pragma once

#ifndef __LOADUTIL_H
#define __LOADUTIL_H

/*//////////////////////////////////////////////////////////////////////////////
//
// File: loadutil.h
//
// Copyright (C) 1999 Microsoft Corporation. All Rights Reserved.
//
// @@BEGIN_MSINTERNAL
//
// History:
// -@-          (craigp)    - created
// -@- 09/23/99 (mikemarr)  - copyright, started history
//
// @@END_MSINTERNAL
//
//////////////////////////////////////////////////////////////////////////////*/


struct SLoadVertex
{
    D3DXVECTOR3    m_vPos;
    D3DXCOLOR  m_color;
    D3DXVECTOR2    m_uvTex1;
    D3DXVECTOR3 m_vNormal;
    DWORD       m_wPointRep;
    DWORD       m_wPointList;
    bool        m_bOwned;
    WORD        m_iSmoothingGroup;
    WORD        m_cSmoothingGroupFaces;
    DWORD       m_iFVFDataOffset;
};

struct SLoadedFace
{
public:
    SLoadedFace()        
        { InitEmpty(); }
    SLoadedFace( DWORD w0, DWORD w1, DWORD w2 )
    {
        Init(w0, w1, w2);
    }
    SLoadedFace( DWORD wIndices[3] )
    {
        Init(wIndices[0], wIndices[1], wIndices[2]);
    }

    void Init(DWORD w0, DWORD w1, DWORD w2)
    {
        m_bColorSpecified = false;
        m_bMaterialSpecified = false;
        m_bAttributeSpecified = false;
        m_bSmoothingGroupSpecified = false;
        m_wIndices[0] = w0;
        m_wIndices[1] = w1;
        m_wIndices[2] = w2;
    }

    void InitEmpty()
    {
        Init(UNUSED32, UNUSED32, UNUSED32);
    }

    DWORD m_wIndices[3];
    WORD m_bColorSpecified:1;
    WORD m_bMaterialSpecified:1;
    WORD m_bAttributeSpecified:1;
    WORD m_bSmoothingGroupSpecified:1;
    D3DXCOLOR m_colorFace;
    DWORD m_matid;
    DWORD m_attr;
    WORD m_iSmoothingGroup;
};

struct SCorner
{
    DWORD        m_wPoint;
    DWORD        m_wFace;
    WORD        m_bNormalSpecified:1;
    WORD        m_bUvSpecified:1;
    D3DXVECTOR2    m_uvTex1;
    D3DXVECTOR3   m_vNormal;
};

// structure used to define additional data not in a SLoadVertex
struct SFVFData
{
    DWORD dwFVF;
    DWORD cBytesPerVertex;
    DWORD *rgiFVFData;
    DWORD iTexCoordOffset;  // 0 if no normal tex data present in x file, 1 if separate texcoord block of data specified
};

HRESULT InitVertices(SLoadVertex *&rglvVertices, DWORD cVertices);
HRESULT InitCorners(SCorner *&rgCorners, DWORD cCorners);
HRESULT InitFaces(SLoadedFace *&rglfFaces, DWORD cFaces);
bool BEqualWedges(SLoadVertex &lv1, SLoadVertex &lv2);

HRESULT SetVertexColor(SLoadedFace *pface, DWORD iPoint, D3DXCOLOR &color, SLoadVertex **prglvLoaded, DWORD *pcVertices, DWORD *pcVerticesMax);
HRESULT SetVertexAttributes(SLoadedFace *pface, DWORD iPoint, D3DXVECTOR3 *pvNormal, D3DXVECTOR2 *puvTex1, SLoadVertex **prglvLoaded, DWORD *pcVertices, DWORD *pcVerticesMax);
DWORD FindPoint(PDWORD pwIndices, DWORD iPointSearch, SLoadVertex *rglvVerts);
HRESULT SetMesh(SLoadedFace *&rglfFaces, DWORD cFaces, 
                SLoadVertex *&rglvLoaded, DWORD cVertices, 
                SCorner *rgCorners, DWORD cCorners,
                SFVFData *pFVFData, BOOL bUsePointRepData,
                DWORD options, DWORD fvf, LPDIRECT3DDEVICE8 pD3D, 
                LPD3DXMESH *ppMesh, LPD3DXBUFFER *ppbufAdjacency);

HRESULT
CreateMaterialBuffer(LPD3DXMATERIAL rgmat, DWORD cmat, LPD3DXBUFFER *ppbufMaterials);

HRESULT
MergeMaterialBuffers(LPD3DXBUFFER pbufMat1, DWORD cmat1, LPD3DXBUFFER pbufMat2, DWORD cmat2, LPD3DXBUFFER *ppbufMaterials);

const int x_cbBufferSize = 256;

class CFileContext
{
public:
    CFileContext(IStream *pstream)
        :m_pstream(pstream), m_ichBuffer(0), m_cchBuffer(0), m_bEndOfFile(false) {}

    HRESULT GetLine(char *szBuffer, int cchBuffer, bool &bEOF);

    HRESULT Rewind()
    {
        m_bEndOfFile = false;

        LARGE_INTEGER li;
        li.QuadPart = 0;
        return m_pstream->Seek(li, STREAM_SEEK_SET, NULL);
    }
private:

    inline HRESULT GetChar(char *szBuffer, bool &bEOF)
    {
        bEOF = false;

        if (m_ichBuffer < m_cchBuffer)
        {
            *szBuffer = m_szBuffer[m_ichBuffer];
            m_ichBuffer += 1;
            return S_OK;
        }
        else
        {
            return GetCharHelper(szBuffer, bEOF);
        }

    }

    HRESULT GetCharHelper(char *szBuffer, bool &bEOF);

    IStream *m_pstream;
    char m_szBuffer[x_cbBufferSize];

    ULONG m_ichBuffer;
    ULONG m_cchBuffer;
    bool m_bEndOfFile;
};

HRESULT WINAPI 
    D3DXCreateBuffer(
        DWORD numBytes, 
        LPD3DXBUFFER *ppBuffer);

#endif  // #ifndef __LOADUTIL_H

