// CBlend_NoPalette_FVF.cpp
// written by Trevor Schrock

#include "d3dlocus.h"
#include "cd3dtest.h"
#include "3dmath.h"
#include "cshapes.h"
#include "MxPalSkin.h"


CBlend_NoPalette_FVF::CBlend_NoPalette_FVF(int nMatrices)
: CVertexBlender(nMatrices, false, false)
{
  m_szTestName    = _T("NoPalette_FVF");
  m_szCommandKey  = _T("NoPalette_FVF");
}


CBlend_NoPalette_FVF::~CBlend_NoPalette_FVF()
{
}


UINT CBlend_NoPalette_FVF::TestInitialize()
{
  HRESULT hr;
  DWORD i;

  UINT uiResult = CVertexBlender::TestInitialize();
  if(uiResult != D3DTESTINIT_RUN)
    return uiResult;

  _stprintf(m_szTitle, TEXT("NoPalette, Fixed function, Test #"));
  return D3DTESTINIT_RUN;
}

bool CBlend_NoPalette_FVF::ExecuteTest(UINT nTest)
{
  D3DXMATRIX mx;

  nTest--;

  if(nTest < 50)
  {
    D3DXMatrixIdentity(&m_mx[0] );
    if(!SetTransform(D3DTS_WORLDMATRIX(0), &m_mx[0] ) )
      return false;

    D3DXMatrixRotationX(&m_mx[1], D3DX_PI / 2.0f);
    if(!SetTransform(D3DTS_WORLDMATRIX(1), &m_mx[1] ) )
      return false;

    if(m_nMatrices > 2)
    {
      D3DXMatrixRotationY(&m_mx[2], D3DX_PI / 2.0f);
      if(!SetTransform(D3DTS_WORLDMATRIX(2), &m_mx[2] ) )
        return false;
    }

    if(m_nMatrices > 3)
    {
      D3DXMatrixRotationZ(&m_mx[3], D3DX_PI / 2.0f);
      if(!SetTransform(D3DTS_WORLDMATRIX(3), &m_mx[3] ) )
        return false;
    }
  }
  else if(nTest < 100)
  {
    D3DXMatrixIdentity(&m_mx[0] );
    if(!SetTransform(D3DTS_WORLDMATRIX(0), &m_mx[0] ) )
      return false;

    D3DXMatrixRotationY(&m_mx[1], D3DX_PI / 2.0f);
    if(!SetTransform(D3DTS_WORLDMATRIX(1), &m_mx[1] ) )
      return false;

    if(m_nMatrices > 2)
    {
      D3DXMatrixRotationZ(&m_mx[2], D3DX_PI / 2.0f);
      if(!SetTransform(D3DTS_WORLDMATRIX(2), &m_mx[2] ) )
        return false;
    }

    if(m_nMatrices > 3)
    {
      D3DXMatrixScaling(&m_mx[3], 0.0f, 0.0f, 0.0f);
      if(!SetTransform(D3DTS_WORLDMATRIX(3), &m_mx[3] ) )
        return false;
    }
  }
  else if(nTest < 150)
  {
    D3DXMatrixIdentity(&m_mx[0] );
    if(!SetTransform(D3DTS_WORLDMATRIX(0), &m_mx[0] ) )
      return false;

    D3DXMatrixRotationZ(&m_mx[1], D3DX_PI / 2.0f);
    if(!SetTransform(D3DTS_WORLDMATRIX(1), &m_mx[1] ) )
      return false;

    if(m_nMatrices > 2)
    {
      D3DXMatrixRotationX(&m_mx[2], D3DX_PI / 2.0f);
      if(!SetTransform(D3DTS_WORLDMATRIX(2), &m_mx[2] ) )
        return false;
    }

    if(m_nMatrices > 3)
    {
      D3DXMatrixScaling(&m_mx[3], 0.0f, 0.0f, 0.0f);
      if(!SetTransform(D3DTS_WORLDMATRIX(3), &m_mx[3] ) )
        return false;
    }
  }
  else if(nTest < 200)
  {
    D3DXMatrixIdentity(&m_mx[0] );
    if(!SetTransform(D3DTS_WORLDMATRIX(0), &m_mx[0] ) )
      return false;

    D3DXMatrixScaling(&m_mx[1], 0.0f, 0.0f, 0.0f);
    if(!SetTransform(D3DTS_WORLDMATRIX(1), &m_mx[1] ) )
      return false;

    if(m_nMatrices > 2)
    {
      D3DXMatrixRotationY(&m_mx[2], D3DX_PI / 2.0f);
      if(!SetTransform(D3DTS_WORLDMATRIX(2), &m_mx[2] ) )
        return false;
    }

    if(m_nMatrices > 3)
    {
      D3DXMatrixRotationX(&m_mx[3], D3DX_PI / 2.0f);
      if(!SetTransform(D3DTS_WORLDMATRIX(3), &m_mx[3] ) )
        return false;
    }
  }

  ApplyWeights(nTest);
  return CVertexBlender::ExecuteTest(nTest);
}

bool CBlend_NoPalette_FVF::ApplyWeights(UINT nTest)
{
  DWORD i, j;
  TestVertexNP1 vx1;
  TestVertexNP2 vx2;
  TestVertexNP3 vx3;

  nTest %= 50;

  for(j = 0; j < VEDGE; j++)
  {
    for(i = 0; i < VEDGE; i++)
    {
      switch(m_nMatrices)
      {
      case 2:
        /*
            Bend m2:

            (1.0, 0.0)  (0.5, 0.5)  (0.0, 1.0)
                          _____
                         |     |
            (1.0, 0.0)   |     |    (0.0, 1.0)
                         |_____|

            (1.0, 0.0)  (0.5, 0.5)  (0.0, 1.0)
        */
        vx1 = ( (TestVertexNP1 *)m_pvx)[j * VEDGE + i];
        if(nTest < 25)
        {
          vx1.m_fWeights[0] = 1.0f - (float) i * (float) (nTest % 25) / (VEDGE_1 * 25.0f);
        }
        else
        {
          vx1.m_fWeights[0] = 1.0f - (float) (VEDGE_1 - i) * (float) (nTest % 25) / (VEDGE_1 * 25.0f);
        }
        ( (TestVertexNP1 *)m_pvx)[j * VEDGE + i] = vx1;
        break;

      case 3:
        /*
            Bend m3:

            (1.0, 0.0, 0.0)  (0.5, 0.5, 0.0)  (0.0, 1.0, 0.0)
                                  _____
                                 |     |
            (1.0, 0.0, 0.0)      |     |      (0.0, 0.5, 0.5)
                                 |_____|

            (1.0, 0.0, 0.0)  (0.5, 0.0, 0.5)  (0.0, 0.0, 1.0)
        */
        vx2 = ( (TestVertexNP2 *)m_pvx)[j * VEDGE + i];
        if(nTest < 25)
        {
          vx2.m_fWeights[0] = 1.0f - (i *
            (float) nTest / (VEDGE_1 * 25.0f) );
          vx2.m_fWeights[1] = (1.0f - vx2.m_fWeights[0] ) * (VEDGE_1 - j) *
            (float) nTest / (VEDGE_1 * 25.0f);
        }
        else
        {
          vx2.m_fWeights[0] = 1.0f - ( ( VEDGE_1 - i) *
            (float) nTest / (VEDGE_1 * 25.0f) );
          vx2.m_fWeights[1] = (1.0f - vx2.m_fWeights[0] ) * (VEDGE_1 - j) *
            (float) nTest / (VEDGE_1 * 25.0f);
        }
        ( (TestVertexNP2 *)m_pvx)[j * VEDGE + i] = vx2;
        break;

      case 4:
        /*
            Bend m4:

            (1.0, 0.0, 0.0, 0.0)  (0.5, 0.5, 0.0, 0.0)  (0.0, 1.0, 0.0, 0.0)
                                         _____
                                        |     |
            (0.5, 0.0, 0.5, 0.0)        |     |         (0.0, 0.5, 0.0, 0.5)
                                        |_____|

            (0.0, 0.0, 1.0, 0.0)  (0.0, 0.0, 0.5, 0.5)  (0.0, 0.0, 0.0, 1.0)
        */
        vx3 = ( (TestVertexNP3 *)m_pvx)[j * VEDGE + i];

        float a, ai, b, bi;

        if(nTest < 25)
        {
          a = (float) (i * (nTest % 25) ) / (VEDGE_1 * 25.0f);    // 0.0 -> 1.0
          ai = 1.0f - a;                                          // 1.0 -> 0.0
          b = (float) (j * (nTest % 25) ) / (VEDGE_1 * 25.0f);    // 0.0 -> 1.0
          bi = 1.0f - b;                                          // 1.0 -> 0.0
        }
        else
        {
          a = (float) ( (VEDGE_1 - i) * (nTest % 25) ) / (VEDGE_1 * 25.0f);    // 0.0 -> 1.0
          ai = 1.0f - a;                                          // 1.0 -> 0.0
          b = (float) (j * (nTest % 25) ) / (VEDGE_1 * 25.0f);    // 0.0 -> 1.0
          bi = 1.0f - b;                                          // 1.0 -> 0.0
        }

        vx3.m_fWeights[0] = ai * bi;
        vx3.m_fWeights[1] = a * bi;
        vx3.m_fWeights[2] = ai * b;

        ( (TestVertexNP3 *)m_pvx)[j * VEDGE + i] = vx3;
      }
    }
  }

  return true;
}
