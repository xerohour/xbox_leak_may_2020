#include "d3dlocus.h"
#include "cd3dtest.h"
#include "3dmath.h"
#include "cshapes.h"
#include "Texform.h"

bool Texform::SetProperties()
{
    int num = m_dwCurrentTestNumber;
    int 
//    m_dwInputSize = 1;
//    m_dwFVF = D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1 | D3DFVF_TEXCOORDSIZE1(0);
    
//    m_dwInputSize = 2;
//    m_dwFVF = D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1 | D3DFVF_TEXCOORDSIZE2(0);

    m_dwInputSize = 3;
    m_dwFVF = D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1 | D3DFVF_TEXCOORDSIZE3(0);

//    m_dwInputSize = 4;
//    m_dwFVF = D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1 | D3DFVF_TEXCOORDSIZE4(0);

    return true;
}

void Texform::SetVertices()
{
    float *p = (float*)m_pVertices;
    for (int i = 0; i < m_pShape->m_nVertices; i++)
    {
        p[0] = m_pShape->m_pVertices[i].x;
        p[1] = m_pShape->m_pVertices[i].y;
        p[2] = m_pShape->m_pVertices[i].z;
        p[3] = m_pShape->m_pVertices[i].nx;
        p[4] = m_pShape->m_pVertices[i].ny;
        p[5] = m_pShape->m_pVertices[i].nz;
        p += 6;

        switch (m_dwInputSize) {
        case 1:
            *p = m_pShape->m_pVertices[i].x;
            p += 1;
            break;
        case 2:
            p[0] = m_pShape->m_pVertices[i].x;
            p[1] = m_pShape->m_pVertices[i].y;
            p += 2;
            break;
        case 3:
            p[0] = m_pShape->m_pVertices[i].x;
            p[1] = m_pShape->m_pVertices[i].y;
            p[2] = m_pShape->m_pVertices[i].z;
            p += 3;
            break;
        case 4:
            p[0] = m_pShape->m_pVertices[i].x;
            p[1] = m_pShape->m_pVertices[i].y;
            p[2] = m_pShape->m_pVertices[i].z;
            p[3] = 1.5f;
            p += 4;
            break;
        }
    }

    D3DMATRIX M = IdentityMatrix();
    if (3 == m_dwInputSize)
    {
        M._13 = 10.f;
    }
    SetTransform(D3DTRANSFORMSTATE_TEXTURE0, &M);
    SetTextureStageState(0, D3DTSS_TEXTURETRANSFORMFLAGS, (DWORD)D3DTTFF_COUNT2);
}