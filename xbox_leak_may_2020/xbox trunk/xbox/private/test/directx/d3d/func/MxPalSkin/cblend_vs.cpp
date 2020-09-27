// CBlend_VS.cpp
// written by Trevor Schrock

// NOTES:  This test uses the TestVertexPn classes for index entries

#include "d3dlocus.h"
#include "cd3dtest.h"
#include "3dmath.h"
#include "cshapes.h"
#include "MxPalSkin.h"


CBlend_VS::CBlend_VS(int nMatrices)
: CVertexBlender(nMatrices, true, true)
{
  m_szTestName    = _T("ProgVS");
  m_szCommandKey  = _T("ProgVS");
}

CBlend_VS::~CBlend_VS()
{
}

UINT CBlend_VS::TestInitialize()
{
  HRESULT hr;
  DWORD i;

  UINT uiResult = CVertexBlender::TestInitialize();
  if(uiResult != D3DTESTINIT_RUN)
    return uiResult;

  // Check caps for vshader caps
  if( ( (m_d3dcaps.VertexShaderVersion >> 8) & 0xff) < 1)
    return WriteToLog(_T("VShader version 1.1 required.  Skipping all.\n") ), D3DTESTINIT_SKIPALL;

  if( ( (m_d3dcaps.VertexShaderVersion & 0xff) < 1) && 
      ( ( (m_d3dcaps.VertexShaderVersion >> 8) & 0xff) == 1) )
    return WriteToLog(_T("VShader version 1.1 required.  Skipping all.\n") ), D3DTESTINIT_SKIPALL;

  // VS declaration:
  DWORD dwWeightFloats;
  switch(m_nMatrices)
  {
  case 2: dwWeightFloats = D3DVSDT_FLOAT1; break;
  case 3: dwWeightFloats = D3DVSDT_FLOAT2; break;
  case 4: dwWeightFloats = D3DVSDT_FLOAT3; break;
  }

  DWORD dwDecl[] = 
  {
    D3DVSD_STREAM(0),
    D3DVSD_REG( 0, D3DVSDT_FLOAT3 ),
    D3DVSD_REG( 1, dwWeightFloats ),
#ifndef UNDER_XBOX
    D3DVSD_REG( 2, D3DVSDT_UBYTE4 ),
#else
    D3DVSD_REG( 2, D3DVSDT_SHORT4 ),
#endif
    D3DVSD_REG( 3, D3DVSDT_FLOAT3 ),
    D3DVSD_REG( 7, D3DVSDT_FLOAT2 ),
    D3DVSD_END()
  };

  // VS function:
  TCHAR sSrc[MAX_PATH];

  switch(m_nMatrices)
  {
  case 2:
//    if(!ResolvePath(SHADER2, sSrc) )
//      return WriteToLog(_T("Failed to ResolvePath()\n") ), D3DTESTINIT_ABORT;
    _tcscpy(sSrc, SHADER2);
    break;
  case 3:
//    if(!ResolvePath(SHADER3, sSrc) )
//      return WriteToLog(_T("Failed to ResolvePath()\n") ), D3DTESTINIT_ABORT;
    _tcscpy(sSrc, SHADER3);
    break;
  case 4:
//    if(!ResolvePath(SHADER4, sSrc) )
//      return WriteToLog(_T("Failed to ResolvePath()\n") ), D3DTESTINIT_ABORT;
    _tcscpy(sSrc, SHADER4);
    break;
  }

#ifndef UNDER_XBOX

  LPD3DXBUFFER pConsts = NULL, pCode = NULL, pErrors = NULL;
  hr = D3DXAssembleShaderFromFile(sSrc, D3DXASM_DEBUG, &pConsts, &pCode, &pErrors);
  if(FAILED(hr) )
  {
    if(pErrors)
    {
      if(pErrors->GetBufferSize() )
      {
        WriteToLog(_T("%s\n"), pErrors->GetBufferPointer() );

        if(pConsts)
          pConsts->Release();
        if(pCode)
          pCode->Release();
        pErrors->Release();
      }
    }
    return WriteToLog(_T("Failed to D3DXAssembleVertexShader()\n") ), D3DTESTINIT_ABORT;
  }

  hr = CreateVertexShader(dwDecl, (LPDWORD)(pCode->GetBufferPointer() ), &m_dwShader, 0);
  if(FAILED(hr) )
    return WriteToLog(_T("Failed to CreateVertexShader()\n") ), D3DTESTINIT_ABORT;

  if(pConsts)
    pConsts->Release();
  if(pCode)
    pCode->Release();
  if(pErrors)
    pErrors->Release();

#else

  m_dwShader = ::CreateVertexShader(m_pDevice, dwDecl, sSrc, 0);
  if (m_dwShader == INVALID_SHADER_HANDLE) {
    return WriteToLog(_T("Failed to CreateVertexShader()\n") ), D3DTESTINIT_ABORT;
  }

#endif // UNDER_XBOX

  hr = SetVertexShader(m_dwShader);
  if(FAILED(hr) )
    return WriteToLog(_T("Failed to set vertex shader.\n") ), D3DTESTINIT_ABORT;

  // TODO: Set the appropriate VS constants here
  /*
; constants:
;				c0:			    (0.0f, 1.0f, 4.0f, 0.0f)
;				c1 - c4:	  transform mx into clip space from world
;				c8 - c11:	  blending mx #0
;				c12 - c15:	blending mx #1
;				c16 - c19:	blending mx #3
;				...
;				c52 - c55:	blending mx #11
  */

  // matrices must be transposed!

  D3DXMATRIX mx[12];

  D3DXVECTOR4 vc(0.0f, 1.0f, 4.0f, 0.0f);
  hr = SetVertexShaderConstant(0, &vc, 1);
  if(FAILED(hr) )
    return WriteToLog(_T("Failed to SetVertexShaderConstant().\n") ), D3DTESTINIT_ABORT;

  D3DXMatrixMultiply(&mx[0], &m_mxView, &m_mxProj);
  D3DXMatrixTranspose(&mx[0], &mx[0] );

  hr = SetVertexShaderConstant(1, mx, 4);
  if(FAILED(hr) )
    return WriteToLog(_T("Failed to SetVertexShaderConstant().\n") ), D3DTESTINIT_ABORT;

  D3DXMatrixIdentity(&mx[0] );
  D3DXMatrixRotationX(&mx[1], D3DX_PI / 2.0f);
  D3DXMatrixRotationY(&mx[2], D3DX_PI / 2.0f);
  D3DXMatrixRotationZ(&mx[3], D3DX_PI / 2.0f);

  D3DXMatrixIdentity(&mx[4] );
  D3DXMatrixTranslation(&mx[5], 1.0f, 0.0f, 0.0f);
  D3DXMatrixTranslation(&mx[6], 0.0f, 1.0f, 0.0f);
  D3DXMatrixTranslation(&mx[7], 0.0f, 0.0f, 1.0f);

  D3DXMatrixIdentity(&mx[8] );
  D3DXMatrixScaling(&mx[ 9], 0.0f, 1.0f, 1.0f);
  D3DXMatrixScaling(&mx[10], 1.0f, 0.0f, 1.0f);
  D3DXMatrixScaling(&mx[11], 0.0f, 0.0f, 0.0f);

  for(i = 0; i < 12; i++)
    D3DXMatrixTranspose(&mx[i], &mx[i] );

  hr = SetVertexShaderConstant(8, mx, 48);
  if(FAILED(hr) )
    return WriteToLog(_T("Failed to SetVertexShaderConstant().\n") ), D3DTESTINIT_ABORT;

  _stprintf(m_szTitle, TEXT("Programmable VShader, Test #"));
  return D3DTESTINIT_RUN;
}

bool CBlend_VS::ExecuteTest(UINT nTest)
{
  nTest--;

  ApplyWeights(nTest);
  return CVertexBlender::ExecuteTest(nTest);
}

bool CBlend_VS::ApplyWeights(UINT nTest)
{
  DWORD i, j, dwIdxStart, dwRunLength;
  TestVertexP1 vx1;
  TestVertexP2 vx2;
  TestVertexP3 vx3;

  dwIdxStart = nTest / 10 * 4;
  dwRunLength = 10;
  nTest %= dwRunLength;

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

        vx1.m_fWeights[0] = 1.0f - (float) i * (float) (nTest % dwRunLength) / (VEDGE_1 * dwRunLength);

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
          (float) nTest / (VEDGE_1 * dwRunLength);
        vx2.m_fWeights[1] = (1.0f - vx2.m_fWeights[0] ) * (VEDGE_1 - j) *
          (float) nTest / (VEDGE_1 * dwRunLength);

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
        a = (float) (i * (nTest) ) / (VEDGE_1 * dwRunLength);
        ai = 1.0f - a;
        b = (float) (j * (nTest) ) / (VEDGE_1 * dwRunLength);
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

bool CBlend_VS::TestTerminate()
{
  HRESULT hr;
  
  DeleteVertexShader(m_dwShader);

  return true;
}
