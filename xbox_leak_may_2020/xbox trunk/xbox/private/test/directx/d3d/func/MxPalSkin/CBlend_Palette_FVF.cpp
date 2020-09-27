// CBlend_Palette_FVF.cpp
// written by Trevor Schrock

#include "d3dlocus.h"
#include "cd3dtest.h"
#include "3dmath.h"
#include "cshapes.h"
#include "MxPalSkin.h"


CBlend_Palette_FVF::CBlend_Palette_FVF(int nMatrices)
: CVertexBlender(nMatrices, true, false)
{
  m_szTestName    = _T("Palette_FVF");
  m_szCommandKey  = _T("Palette_FVF");
}


CBlend_Palette_FVF::~CBlend_Palette_FVF()
{
}


UINT CBlend_Palette_FVF::TestInitialize()
{
  HRESULT hr;
  DWORD i;
  char sOut[128];
  
  UINT uiResult = CVertexBlender::TestInitialize();
  if(uiResult != D3DTESTINIT_RUN)
    return uiResult;
  
  _stprintf(m_szTitle, TEXT("Palette, Fixed function, Test #"));
  return D3DTESTINIT_RUN;
}

bool CBlend_Palette_FVF::ExecuteTest(UINT nTest)
{
  nTest--;    // set the test number starting from 0 instead of 1

  D3DXMATRIX mx;
  DWORD dwIdxStart;
  dwIdxStart = nTest / 10 * m_nMatrices;
  UINT nTestMod = nTest % 10;
  
  if(nTestMod < 5)
  {
    D3DXMatrixIdentity(&m_mx[0] );
    if(!SetTransform(D3DTS_WORLDMATRIX(dwIdxStart++), &m_mx[0] ) )
      return WriteToLog(_T("SetTransform() failed -- nTest = %d, idx = %d\n"), nTest, dwIdxStart - 1), false;
    
    D3DXMatrixRotationX(&m_mx[1], D3DX_PI / 2.0f);
    if(!SetTransform(D3DTS_WORLDMATRIX(dwIdxStart++), &m_mx[1] ) )
      return WriteToLog(_T("SetTransform() failed -- nTest = %d, idx = %d\n"), nTest, dwIdxStart - 1), false;
    
    if(m_nMatrices > 2)
    {
      D3DXMatrixRotationY(&m_mx[2], D3DX_PI / 2.0f);
      if(!SetTransform(D3DTS_WORLDMATRIX(dwIdxStart++), &m_mx[2] ) )
        return WriteToLog(_T("SetTransform() failed -- nTest = %d, idx = %d\n"), nTest, dwIdxStart - 1), false;
    }
    
    if(m_nMatrices > 3)
    {
      D3DXMatrixRotationZ(&m_mx[3], D3DX_PI / 2.0f);
      if(!SetTransform(D3DTS_WORLDMATRIX(dwIdxStart++), &m_mx[3] ) )
        return WriteToLog(_T("SetTransform() failed -- nTest = %d, idx = %d\n"), nTest, dwIdxStart - 1), false;
    }
  }
  else if(nTestMod < 10)
  {
    D3DXMatrixIdentity(&m_mx[0] );
    if(!SetTransform(D3DTS_WORLDMATRIX(dwIdxStart++), &m_mx[0] ) )
      return WriteToLog(_T("SetTransform() failed -- nTest = %d, idx = %d\n"), nTest, dwIdxStart - 1), false;
    
    D3DXMatrixRotationY(&m_mx[1], D3DX_PI / 2.0f);
    if(!SetTransform(D3DTS_WORLDMATRIX(dwIdxStart++), &m_mx[1] ) )
      return WriteToLog(_T("SetTransform() failed -- nTest = %d, idx = %d\n"), nTest, dwIdxStart - 1), false;
    
    if(m_nMatrices > 2)
    {
      D3DXMatrixRotationZ(&m_mx[2], D3DX_PI / 2.0f);
      if(!SetTransform(D3DTS_WORLDMATRIX(dwIdxStart++), &m_mx[2] ) )
        return WriteToLog(_T("SetTransform() failed -- nTest = %d, idx = %d\n"), nTest, dwIdxStart - 1), false;
    }
    
    if(m_nMatrices > 3)
    {
      D3DXMatrixScaling(&m_mx[3], 0.0f, 0.0f, 0.0f);
      if(!SetTransform(D3DTS_WORLDMATRIX(dwIdxStart++), &m_mx[3] ) )
        return WriteToLog(_T("SetTransform() failed -- nTest = %d, idx = %d\n"), nTest, dwIdxStart - 1), false;
    }
  }

  ApplyWeights(nTest);

  return CVertexBlender::ExecuteTest(nTest);
}

bool CBlend_Palette_FVF::ApplyWeights(UINT nTest)
{
  DWORD i, j, dwIdxStart;
  TestVertexP1 vx1;
  TestVertexP2 vx2;
  TestVertexP3 vx3;

  dwIdxStart = nTest / 10 * m_nMatrices;
  nTest %= 5;

  WriteStatus(_T("$gMatrices blended:"), _T("$y%d"), m_nMatrices);
  WriteStatus(_T("$gIndices start:"), _T("$y%d"), dwIdxStart);

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
        vx1 = ( (TestVertexP1 *)m_pvx)[j * VEDGE + i];

        vx1.m_fWeights[0] = 1.0f - (float) i * (float) (nTest % 5) / (VEDGE_1 * 5.0f);

        vx1.m_ucIndices[ IDX(0) ] = (UCHAR)dwIdxStart;
        vx1.m_ucIndices[ IDX(1) ] = (UCHAR)dwIdxStart + 1;
        vx1.m_ucIndices[ IDX(2) ] = 255;    // just as an obvious bad value
        vx1.m_ucIndices[ IDX(3) ] = 255;    // just as an obvious bad value

        ( (TestVertexP1 *)m_pvx)[j * VEDGE + i] = vx1;
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
        vx2 = ( (TestVertexP2 *)m_pvx)[j * VEDGE + i];

        vx2.m_fWeights[0] = 1.0f - ( (float) i) *
          (float) nTest / (VEDGE_1 * 5.0f);
        vx2.m_fWeights[1] = (1.0f - vx2.m_fWeights[0] ) * (VEDGE_1 - j) *
          (float) nTest / (VEDGE_1 * 5.0f);

        vx2.m_ucIndices[ IDX(0) ] = (UCHAR)dwIdxStart;
        vx2.m_ucIndices[ IDX(1) ] = (UCHAR)dwIdxStart + 1;
        vx2.m_ucIndices[ IDX(2) ] = (UCHAR)dwIdxStart + 2;
        vx2.m_ucIndices[ IDX(3) ] = 255;    // just as an obvious bad value

        ( (TestVertexP2 *)m_pvx)[j * VEDGE + i] = vx2;
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
        vx3 = ( (TestVertexP3 *)m_pvx)[j * VEDGE + i];

        float a, ai, b, bi;
        a = (float) (i * (nTest) ) / (VEDGE_1 * 5.0f);
        ai = 1.0f - a;
        b = (float) (j * (nTest) ) / (VEDGE_1 * 5.0f);
        bi = 1.0f - b;

        vx3.m_fWeights[0] = ai * bi;
        vx3.m_fWeights[1] = a * bi;
        vx3.m_fWeights[2] = ai * b;

        vx3.m_ucIndices[ IDX(0) ] = (UCHAR)dwIdxStart;
        vx3.m_ucIndices[ IDX(1) ] = (UCHAR)dwIdxStart + 1;
        vx3.m_ucIndices[ IDX(2) ] = (UCHAR)dwIdxStart + 2;
        vx3.m_ucIndices[ IDX(3) ] = (UCHAR)dwIdxStart + 3;

        ( (TestVertexP3 *)m_pvx)[j * VEDGE + i] = vx3;
      }
    }
  }

  return true;
}
