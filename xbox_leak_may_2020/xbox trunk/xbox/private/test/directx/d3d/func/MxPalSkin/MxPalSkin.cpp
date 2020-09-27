// MxPalSkin.cpp
// written by Trevor Schrock

#include "d3dlocus.h"
#include "cd3dtest.h"
#include "3dmath.h"
#include "cshapes.h"
#include "MxPalSkin.h"

//
// Declare the framework
//
                   
//CD3DWindowFramework	App;

//******************************************************************************
//
// Function:
//
//     ValidateDisplay
//
// Description:
//
//     Evaluate the given display information in order to determine whether or
//     not the display is capable of rendering the scene.  If not, the given 
//     display will not be included in the display list.
//
// Arguments:
//
//     CDirect3D8* pd3d                 - Pointer to the Direct3D object
//
//     D3DCAPS8* pd3dcaps               - Capabilities of the device
//
//     D3DDISPLAYMODE*                  - Display mode into which the device
//                                        will be placed
//
// Return Value:
//
//     TRUE if the scene can be rendered using the given display, FALSE if
//     it cannot.
//
//******************************************************************************
extern "C" BOOL PREPEND_MODULE(_ValidateDisplay)(CDirect3D8* pd3d, D3DCAPS8* pd3dcaps, D3DDISPLAYMODE* pd3ddm) {

    return TRUE;
}

//******************************************************************************
//
// Function:
//
//     ExhibitScene
//
// Description:
//
//     Create the scene, pump messages, process user input,
//     update the scene, render the scene, and release the scene when finished.
//
// Arguments:
//
//     CDisplay* pDisplay           - Pointer to the Display object
//
//     int* pnExitCode              - Optional pointer to an integer that will
//                                    be set to the exit value contained in the 
//                                    wParam parameter of the WM_QUIT message 
//                                    (if received)
//
// Return Value:
//
//     TRUE if the display remains functional on exit, FALSE otherwise.
//
//******************************************************************************
extern "C" BOOL PREPEND_MODULE(_ExhibitScene)(CDisplay* pDisplay, int* pnExitCode) {

    CVertexBlender* pBlender;
    UINT        i, j;
    BOOL        bQuit = FALSE, bRet = TRUE;

    // Verify that the display has been initialized
    if (!pDisplay) {
        return FALSE;
    }

    // Run through all scenarios for the test
#ifndef UNDER_XBOX
    for (j = 0; j < 3; j++) {
#else
    for (j = 0; j < 1; j++) {
#endif

        for (i = 2; i < 5 && !bQuit && bRet; i++) {

            // Create the scene
            switch (j) {
                case 0:
                    pBlender = new CBlend_NoPalette_FVF(i);
                    break;
#ifndef UNDER_XBOX
                case 1:
                    pBlender = new CBlend_VS(i);
                    break;
                case 2:
                    pBlender = new CBlend_Palette_FVF(i);
                    break;
#endif
            }
            if (!pBlender) {
                return FALSE;
            }

            // Initialize the scene
            if (!pBlender->Create(pDisplay)) {
                pBlender->Release();
                return FALSE;
            }

            bRet = pBlender->Exhibit(pnExitCode) && bRet;

            bQuit = pBlender->AbortedExit();

            // Clean up the scene
            pBlender->Release();
        }
    }

    return bRet;
}

CVertexBlender::CVertexBlender(int nMatrices, bool bPalette, bool bVS)
{
  m_ModeOptions.fReference    = true;
  m_ModeOptions.fRefEqualsSrc = false;
  m_ModeOptions.fTextures     = true;
  m_ModeOptions.fZBuffer      = true;
  m_ModeOptions.bSWDevices    = false;
  m_ModeOptions.bHWDevices    = true;
//  m_ModeOptions.dwDeviceTypes = DEVICETYPE_HAL | DEVICETYPE_PURE | DEVICETYPE_TNL;    // test software pipelines thru command line
  m_ModeOptions.uMinDXVersion = 0x800;

  memset(&m_Material, 0, sizeof(D3DMATERIAL8));
//  m_pMaterial = NULL;
  memset(&m_Light, 0, sizeof(D3DLIGHT8));
//  m_pLight = NULL;
  m_pvx = NULL;

  m_nMatrices = nMatrices;
  m_bPalette = bPalette;
  m_bVS = bVS;

  m_bExit = FALSE;
}

CVertexBlender::~CVertexBlender()
{
}

bool CVertexBlender::SetDefaultMatrices()
{
  // maliciously copied from robald's stateblock conf tests

  D3DMATRIX     mx;

  /**/
  D3DVECTOR     vAt   = D3DXVECTOR3( 0.0f,    0.0f,   0.5f  ),
                vLoc  = D3DXVECTOR3( 0.0f,    0.0f,   -1.5f ),
                vUp   = D3DXVECTOR3( 0.0f,    1.0f,   0.0f  );
  /*/
  D3DVECTOR     vAt   = D3DXVECTOR3( 0.0f,    0.0f,   0.3f  ),
                vLoc  = D3DXVECTOR3( 0.0f,    0.0f,   0.0f ),
                vUp   = D3DXVECTOR3( 0.0f,    1.0f,   0.0f  );
  /**/

  m_mxProj = ProjectionMatrix(.1f, 1000.0f, pi / 4.0f);
  if(!SetTransform(D3DTS_PROJECTION, &m_mxProj) )
    return false;
  
  m_mxView = ViewMatrix(vLoc, vAt, vUp);
  if(!SetTransform(D3DTS_VIEW, &m_mxView) )
    return false;
  
  mx = IdentityMatrix();
  if(!SetTransform(D3DTS_WORLD, &mx) )
    return false;

  D3DXMatrixIdentity(&m_mx[0] );
  D3DXMatrixIdentity(&m_mx[1] );
  D3DXMatrixIdentity(&m_mx[2] );
  D3DXMatrixIdentity(&m_mx[3] );

  if(!SetTransform(D3DTS_WORLDMATRIX(0), &m_mx[0] ) )
    return false;

  if(!SetTransform(D3DTS_WORLDMATRIX(1), &m_mx[1] ) )
    return false;

  if(m_nMatrices > 2)
    if(!SetTransform(D3DTS_WORLDMATRIX(2), &m_mx[2] ) )
      return false;

  if(m_nMatrices > 3)
    if(!SetTransform(D3DTS_WORLDMATRIX(3), &m_mx[3] ) )
      return false;

//  SetupViewport();

  return true;
}

bool CVertexBlender::SetDefaultMaterials()
{
  ZeroMemory(&m_Material, sizeof(D3DMATERIAL8) );
  m_Material.Diffuse  = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
  m_Material.Ambient  = D3DXCOLOR(0.5f, 0.5f, 0.5f, 0.5f);
  m_Material.Specular = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
  m_Material.Emissive = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
  m_Material.Power    = 0.0f;

  m_pDevice->SetMaterial(&m_Material);

  // Load the texture image, eh.
/*
  TCHAR sImg[MAX_PATH];
  if(!ResolvePath(IMAGE_FILE, sImg) )
    return false;

  CImageLoader IL;
  m_pImage = IL.Load(CIL_BMP, sImg);

  if(m_pImage == NULL)
  {
    WriteToLog(_T("Unable to load ze image %s.\n"), IMAGE_FILE);
    return D3DTESTINIT_ABORT;
  }
  
  m_pTexture = CreateTexture(m_pImage->GetWidth(), m_pImage->GetHeight(), CDDS_TEXTURE_VIDEO, m_pImage);
*/
  m_pTexture = (CTexture8*)CreateTexture(m_pDevice, IMAGE_FILE);
  if(m_pTexture == NULL)
  {
    WriteToLog(_T("CreateTexture() failed.\n") );
    return D3DTESTINIT_ABORT;
  }

  SetTexture(0, m_pTexture);

  return true;
}

bool CVertexBlender::SetDefaultLights()
{
  HRESULT hr;

  ZeroMemory(&m_Light, sizeof(D3DLIGHT8) );
  m_Light.Type        = D3DLIGHT_DIRECTIONAL;
  m_Light.Diffuse     = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
  m_Light.Specular    = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
  m_Light.Ambient     = D3DXCOLOR(0.0f, 0.0f, 0.0f, 1.0f);
  m_Light.Direction   = D3DXVECTOR3(1.0f, -1.0f, 1.0f);

  hr = m_pDevice->SetLight(0, &m_Light);
  if (FAILED(hr)) {
      return false;
  }

  hr = m_pDevice->LightEnable(0, TRUE);
  return (hr == D3D_OK);
}

bool CVertexBlender::SetDefaultRenderStates(void)
{
  SetRenderState(D3DRS_AMBIENT, (DWORD)RGBA_MAKE(64, 64, 64, 255) );
  SetRenderState(D3DRS_CULLMODE, (DWORD)D3DCULL_NONE);
  SetTextureStageState(0, D3DTSS_MINFILTER, (DWORD) D3DTEXF_POINT);
  SetTextureStageState(0, D3DTSS_MAGFILTER, (DWORD) D3DTEXF_POINT);

  switch(m_nMatrices)
  {
  case 2:
    SetRenderState(D3DRS_VERTEXBLEND, (DWORD)D3DVBF_1WEIGHTS);
    break;
  case 3:
    SetRenderState(D3DRS_VERTEXBLEND, (DWORD)D3DVBF_2WEIGHTS);
    break;
  case 4:
    SetRenderState(D3DRS_VERTEXBLEND, (DWORD)D3DVBF_3WEIGHTS);
    break;
  default:
    SetRenderState(D3DRS_VERTEXBLEND, (DWORD)D3DVBF_DISABLE);
    break;
  }

#ifndef UNDER_XBOX
  if(!m_bPalette)
    SetRenderState(D3DRS_INDEXEDVERTEXBLENDENABLE, (DWORD)FALSE);
  else
    SetRenderState(D3DRS_INDEXEDVERTEXBLENDENABLE, (DWORD)TRUE);
#else
    if (m_bPalette) {
        DebugString(TEXT("D3DRS_INDEXEDVERTEXBLENDENABLE unsupported under Xbox"));
        __asm int 3;
    }
#endif

  return true;
}

UINT CVertexBlender::TestInitialize()
{
//  UINT n = m_pMode->nSrcDevice;
//  m_pDesc = &(m_pAdapter->Devices[n].Desc);     // CDirect3D.h line 253 -> CD3D.h line 261

  // Maximum number of blending matrices we can apply, eh.
  m_dwMaxBlends = m_d3dcaps.MaxVertexBlendMatrices;

  // Maximum blending index we can use.
  if(!m_bVS)
    m_dwMaxIndex = m_d3dcaps.MaxVertexBlendMatrixIndex;
  else
    m_dwMaxIndex = 3 * m_nMatrices - 1;         // keeps the same number of tests for 2,3,4

  // If palette skinning, non vshader test, skip on max idx of 0.
  if(m_bPalette && !m_bVS && m_dwMaxIndex == 0)
    return D3DTESTINIT_SKIPALL;

  // Device does not support x number of blends.
  if((int)m_dwMaxBlends < m_nMatrices)
    return D3DTESTINIT_SKIPALL;

  // Directional light support (just to be sure).
  if( !(m_d3dcaps.VertexProcessingCaps & D3DVTXPCAPS_DIRECTIONALLIGHTS) )
    return D3DTESTINIT_SKIPALL;

  // Create the mesh, don'tchyaknow.
  if (!m_Mesh.NewShape(CS_MESH, VEDGE_1, VEDGE_1)) {
    return D3DTESTINIT_ABORT;
  }

  if (!SetupGeometry()) {
    return D3DTESTINIT_ABORT;
  }

  if(!m_bPalette)
    SetTestRange(1, 200);
  else
    SetTestRange(1, 10 * ( (m_dwMaxIndex + 1) / m_nMatrices) );

  return D3DTESTINIT_RUN;
}

bool CVertexBlender::ExecuteTest(UINT nTest)
{
  TCHAR   szTitle[128];

  nTest++;      // counter the nTest-- in each subclass

  _stprintf(szTitle, TEXT("%s%04u"), m_szTitle, nTest);
  
  BeginTestCase(szTitle, nTest);
  {
    WriteStatus(TEXT("$gTest name: $g"), szTitle);
  }

  return true;
}

void CVertexBlender::SceneRefresh(void)
{
    HRESULT hr;

    if(!m_bVS)
    {
      hr = SetVertexShader(m_dwShader);
      if(FAILED(hr) )
        WriteToLog(_T("Failed to SetVertexShader().\n") ), Fail();
    }

    hr = RenderBlender();
    if(FAILED(hr) )
      WriteToLog(_T("Failed to render.\n") ), Fail();
}

bool CVertexBlender::ProcessFrame()
{
  TCHAR   szBuffer[80];
  static  int nPass = 0;
  static  int nFail = 0;
  bool    bResult;
  
  bResult = GetCompareResult(0.05f, 0.95f, 0);
  
  // Tell the logfile how they did
  if(bResult)
  {
//    (m_fIncrementForward) ? nPass++ : nPass--;
    nPass++;
    wsprintf(szBuffer,TEXT("$yPass: %d, Fail: %d"), nPass, nFail);
    Pass();
  }
  else
  {
//    (m_fIncrementForward) ? nFail++ : nFail--;
    nFail++;
    wsprintf(szBuffer,TEXT("$yPass: %d, Fail: %d"), nPass, nFail);
    Fail();
  }
  
  WriteStatus(TEXT("$gOverall Results"), szBuffer);
  
  return bResult;
}


bool CVertexBlender::TestTerminate()
{
//  delete m_pLight;
//  delete m_pMaterial;

  delete [] m_pvx;

//  m_pImage->Release();
  ReleaseTexture(m_pTexture);

  return true;
}

bool CVertexBlender::SetupGeometry()
{
  int i;

  if(m_pvx)
    delete [] m_pvx;

  switch(m_nMatrices)
  {
  case 2:
    if(!m_bPalette)
    {
      m_pvx = new TestVertexNP1[m_Mesh.m_nVertices];
      if (!m_pvx) {
        return false;
      }
      m_dwShader = D3DFVF_NP1;

      for(i = 0; i < m_Mesh.m_nVertices; i++)
      {
        TestVertexNP1 vx;

        vx.m_vPos.x = m_Mesh.m_pVertices[i].x;
        vx.m_vPos.y = m_Mesh.m_pVertices[i].y;
        vx.m_vPos.z = m_Mesh.m_pVertices[i].z;
        vx.m_vNormal.x = m_Mesh.m_pVertices[i].nx;
        vx.m_vNormal.y = m_Mesh.m_pVertices[i].ny;
        vx.m_vNormal.z = m_Mesh.m_pVertices[i].nz;
        vx.m_fTex0[0] = m_Mesh.m_pVertices[i].tu;
        vx.m_fTex0[1] = m_Mesh.m_pVertices[i].tv;

        ( (TestVertexNP1 *)m_pvx)[i] = vx;
      }
    }
    else
    {
#ifndef UNDER_XBOX
      m_pvx = new TestVertexP1[m_Mesh.m_nVertices];
      if (!m_pvx) {
        return false;
      }
      m_dwShader = D3DFVF_P1;

      for(i = 0; i < m_Mesh.m_nVertices; i++)
      {
        TestVertexP1 vx;

        vx.m_vPos.x = m_Mesh.m_pVertices[i].x;
        vx.m_vPos.y = m_Mesh.m_pVertices[i].y;
        vx.m_vPos.z = m_Mesh.m_pVertices[i].z;
        vx.m_vNormal.x = m_Mesh.m_pVertices[i].nx;
        vx.m_vNormal.y = m_Mesh.m_pVertices[i].ny;
        vx.m_vNormal.z = m_Mesh.m_pVertices[i].nz;
        vx.m_fTex0[0] = m_Mesh.m_pVertices[i].tu;
        vx.m_fTex0[1] = m_Mesh.m_pVertices[i].tv;

        ( (TestVertexP1 *)m_pvx)[i] = vx;
      }
#else
      DebugString(TEXT("D3DFVF_LASTBETA_UBYTE4 unsupported under Xbox"));
      __asm int 3;
#endif
    }
    break;

  case 3:
    if(!m_bPalette)
    {
      m_pvx = new TestVertexNP2[m_Mesh.m_nVertices];
      if (!m_pvx) {
        return false;
      }
      m_dwShader = D3DFVF_NP2;

      for(i = 0; i < m_Mesh.m_nVertices; i++)
      {
        TestVertexNP2 vx;

        vx.m_vPos.x = m_Mesh.m_pVertices[i].x;
        vx.m_vPos.y = m_Mesh.m_pVertices[i].y;
        vx.m_vPos.z = m_Mesh.m_pVertices[i].z;
        vx.m_vNormal.x = m_Mesh.m_pVertices[i].nx;
        vx.m_vNormal.y = m_Mesh.m_pVertices[i].ny;
        vx.m_vNormal.z = m_Mesh.m_pVertices[i].nz;
        vx.m_fTex0[0] = m_Mesh.m_pVertices[i].tu;
        vx.m_fTex0[1] = m_Mesh.m_pVertices[i].tv;

        ( (TestVertexNP2 *)m_pvx)[i] = vx;
      }
    }
    else
    {
#ifndef UNDER_XBOX
      m_pvx = new TestVertexP2[m_Mesh.m_nVertices];
      if (!m_pvx) {
        return false;
      }
      m_dwShader = D3DFVF_P2;

      for(i = 0; i < m_Mesh.m_nVertices; i++)
      {
        TestVertexP2 vx;

        vx.m_vPos.x = m_Mesh.m_pVertices[i].x;
        vx.m_vPos.y = m_Mesh.m_pVertices[i].y;
        vx.m_vPos.z = m_Mesh.m_pVertices[i].z;
        vx.m_vNormal.x = m_Mesh.m_pVertices[i].nx;
        vx.m_vNormal.y = m_Mesh.m_pVertices[i].ny;
        vx.m_vNormal.z = m_Mesh.m_pVertices[i].nz;
        vx.m_fTex0[0] = m_Mesh.m_pVertices[i].tu;
        vx.m_fTex0[1] = m_Mesh.m_pVertices[i].tv;

        ( (TestVertexP2 *)m_pvx)[i] = vx;
      }
#else
      DebugString(TEXT("D3DFVF_LASTBETA_UBYTE4 unsupported under Xbox"));
      __asm int 3;
#endif
    }
    break;

  case 4:
    if(!m_bPalette)
    {
      m_pvx = new TestVertexNP3[m_Mesh.m_nVertices];
      if (!m_pvx) {
        return false;
      }
      m_dwShader = D3DFVF_NP3;

      for(i = 0; i < m_Mesh.m_nVertices; i++)
      {
        TestVertexNP3 vx;

        vx.m_vPos.x = m_Mesh.m_pVertices[i].x;
        vx.m_vPos.y = m_Mesh.m_pVertices[i].y;
        vx.m_vPos.z = m_Mesh.m_pVertices[i].z;
        vx.m_vNormal.x = m_Mesh.m_pVertices[i].nx;
        vx.m_vNormal.y = m_Mesh.m_pVertices[i].ny;
        vx.m_vNormal.z = m_Mesh.m_pVertices[i].nz;
        vx.m_fTex0[0] = m_Mesh.m_pVertices[i].tu;
        vx.m_fTex0[1] = m_Mesh.m_pVertices[i].tv;

        ( (TestVertexNP3 *)m_pvx)[i] = vx;
      }
    }
    else
    {
#ifndef UNDER_XBOX
      m_pvx = new TestVertexP3[m_Mesh.m_nVertices];
      if (!m_pvx) {
        return false;
      }
      m_dwShader = D3DFVF_P3;

      for(i = 0; i < m_Mesh.m_nVertices; i++)
      {
        TestVertexP3 vx;

        vx.m_vPos.x = m_Mesh.m_pVertices[i].x;
        vx.m_vPos.y = m_Mesh.m_pVertices[i].y;
        vx.m_vPos.z = m_Mesh.m_pVertices[i].z;
        vx.m_vNormal.x = m_Mesh.m_pVertices[i].nx;
        vx.m_vNormal.y = m_Mesh.m_pVertices[i].ny;
        vx.m_vNormal.z = m_Mesh.m_pVertices[i].nz;
        vx.m_fTex0[0] = m_Mesh.m_pVertices[i].tu;
        vx.m_fTex0[1] = m_Mesh.m_pVertices[i].tv;

        ( (TestVertexP3 *)m_pvx)[i] = vx;
      }
#else
      DebugString(TEXT("D3DFVF_LASTBETA_UBYTE4 unsupported under Xbox"));
      __asm int 3;
#endif
    }
    break;

  default:
    return false;
  }

  return true;
}

HRESULT CVertexBlender::RenderBlender()
{
  DWORD i;
  HRESULT hr;
  char sOut[256];

  hr = BeginScene();
  if(FAILED(hr) )
    return WriteToLog(_T("Failed to BeginScene(): 0x%s\n"), DecodeHResult(hr) ), hr;

  PBYTE pData;
  DWORD dwSize, dwStride;
  switch(m_nMatrices)
  {
  case 2: dwStride = m_bPalette ? sizeof(TestVertexP1) : sizeof(TestVertexNP1); break;
  case 3: dwStride = m_bPalette ? sizeof(TestVertexP2) : sizeof(TestVertexNP2); break;
  case 4: dwStride = m_bPalette ? sizeof(TestVertexP3) : sizeof(TestVertexNP3); break;
  }
  dwSize = dwStride * m_Mesh.m_nVertices;

  CVertexBuffer8* pVBSrc = CreateVertexBuffer(m_pDevice, NULL, dwSize, 0, 0);
  if (!pVBSrc) {
    return E_FAIL;
  }
  CVertexBuffer8* pVBRef = NULL;// = CreateVertexBuffer(0, dwSize, 0, true);
  CIndexBuffer8* pIBSrc  = CreateIndexBuffer(m_pDevice, NULL, m_Mesh.m_nIndices * sizeof(WORD), 0, D3DFMT_INDEX16);
  if (!pIBSrc) {
    ReleaseVertexBuffer(pVBSrc);
    return E_FAIL;
  }
  CIndexBuffer8* pIBRef = NULL;//  = CreateIndexBuffer(m_Mesh.m_nIndices, D3DFMT_INDEX16, true);

//  pVBSrc->Lock( (PVOID *)(&pData) );
  pVBSrc->Lock(0, dwSize, (LPBYTE *)(&pData), 0);
  memcpy(pData, m_pvx, dwSize);
  pVBSrc->Unlock();
//  pVBRef->Lock( (PVOID *)(&pData) );
//  memcpy(pData, m_pvx, dwSize);
//  pVBRef->Unlock();

//  pIBSrc->Lock( (PVOID *)(&pData) );
  pIBSrc->Lock(0, sizeof(WORD) * m_Mesh.m_nIndices, (LPBYTE *)(&pData), 0);
  memcpy(pData, m_Mesh.m_pIndices, sizeof(WORD) * m_Mesh.m_nIndices);
  pIBSrc->Unlock();
//  pIBRef->Lock( (PVOID *)(&pData) );
//  memcpy(pData, m_Mesh.m_pIndices, sizeof(WORD) * m_Mesh.m_nIndices);
//  pIBRef->Unlock();

  if(!SetStreamSource(0, pVBSrc, pVBRef, dwStride, 0) )
    return WriteToLog(_T("Failed to SetStreamSource(): %s\n"), DecodeHResult(GetLastError() ) ), GetLastError();

  if(!SetIndices(pIBSrc, pIBRef, 0, 0) )
    return WriteToLog(_T("Failed to SetIndices(): %s\n"), DecodeHResult(GetLastError() ) ), GetLastError();

  /**/
//  if(!DrawIndexedPrimitive8(D3DPT_TRIANGLELIST, 0, NUM_VERTICES, 0, 2 * VEDGE_1 * VEDGE_1, 0) )
  if (FAILED(m_pDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, NUM_VERTICES, 0, 2 * VEDGE_1 * VEDGE_1)))
    return WriteToLog(_T("Failed to DrawIndexedPrimitive8(): %s\n"), DecodeHResult(GetLastError() ) ), GetLastError();
  /*/

  CVertexBuffer* pVBCap = CreateVertexBuffer(D3DFVF_CAP, m_Mesh.m_nVertices, 0, false);

  sprintf(sOut, "FVF code: 0x%08x\n", D3DFVF_CAP);
  OutputDebugString(sOut);

  if(!ProcessVertices(0, 0, m_Mesh.m_nVertices, pVBCap, 0) )
    return WriteToLog(_T("Failed to ProcessVertices(): %s\n"), DecodeHResult(GetLastError() ) ), GetLastError();

  pVBCap->Lock( (PVOID *)(&pData) );
  for(i = 0; i < m_Mesh.m_nVertices; i++)
  {
    CapturedVertex cv;
    cv = ( (CapturedVertex *)pData)[i];
    sprintf(sOut, "x: %f,   y: %f,   z: %f,   rhw: %f,   diffuse: 0x%08x,   specular: 0x%08x,   tex0: %f,   tex1: %f,   tex2: %f,   tex3: %f\n",
      cv.m_vPos.x, cv.m_vPos.y, cv.m_vPos.z, cv.m_fRHW, cv.m_dwDiffuse, cv.m_dwSpecular, cv.m_fTex0[0], cv.m_fTex0[1], cv.m_fTex0[2], cv.m_fTex0[3] );
    OutputDebugString(sOut);
  }
  pVBCap->Unlock();

  pVBCap->Release();
  /*/
  /*
  if(!DrawPrimitive(D3DPT_TRIANGLELIST, m_dwShader,
    m_pvx,
    m_Mesh.m_nVertices,
    m_Mesh.m_pIndices,
    m_Mesh.m_nIndices,
    0) )
    return EndScene(), WriteToLog(_T("Failed to DrawPrimitive(): %s\n"), DecodeHResult(GetLastError() ) ), GetLastError();
  */

  ReleaseVertexBuffer(pVBSrc);
//  pVBRef->Release();
  ReleaseIndexBuffer(pIBSrc);
//  pIBRef->Release();

  hr = EndScene();
  if(FAILED(hr) )
    return WriteToLog(_T("Failed to EndScene(): %s\n"), DecodeHResult(hr) ), hr;

  return D3D_OK;
}

//******************************************************************************
BOOL CVertexBlender::AbortedExit(void) {

    return m_bExit;
}

//******************************************************************************
void CVertexBlender::ProcessInput() {

    CD3DTest::ProcessInput();

    if (BUTTON_PRESSED(m_jsJoys, JOYBUTTON_BACK)) {
        m_bExit = TRUE;
    }
}

//******************************************************************************
BOOL CVertexBlender::WndProc(LRESULT* plr, HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {

    *plr = 0;

#ifndef UNDER_XBOX

    switch (uMsg) {

        case WM_KEYDOWN:

            switch (wParam) {

                case VK_ESCAPE: // Exit
                    m_bExit = TRUE;
            }

            break;
    }

#endif // !UNDER_XBOX

    return CD3DTest::WndProc(plr, hWnd, uMsg, wParam, lParam);
}

