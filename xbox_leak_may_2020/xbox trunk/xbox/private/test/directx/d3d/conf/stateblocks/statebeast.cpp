// ============================================================================
//
//  StateBeast.cpp
//
// ============================================================================
#include "d3dlocus.h"
#include "cd3dtest.h"
#include "3dmath.h"
#include "cshapes.h"
#include "StateBeast.h"
//#include "CImageLoader.h"
//#include "rnumber.h" // random-number generator
//#include "d3d.h"

int g_nSBT = 3;
FLAGS g_SBT[] = { { D3DSBT_ALL, _T("D3DSBT_ALL")},
				  { D3DSBT_PIXELSTATE, _T("D3DSBT_PIXEL")},
				  { D3DSBT_VERTEXSTATE, _T("D3DSBT_VERTEX")}};

// ----------------------------------------------------------------------------

CStateBeast::CStateBeast()
{ 
    m_nViewports  = 2;
	m_ModeOptions.uMinDXVersion = 0x700;
//    m_ModeOptions.dwDeviceTypes = DEVICETYPE_HAL | DEVICETYPE_TNL | DEVICETYPE_MIXED | DEVICETYPE_PURE;
	m_dwVSShader = m_dwVSShader2 = 0;
    m_dwPSShader = 0;
	m_nTotalDX8 = 0;
	m_nTotalDX7 = 28;
    m_pSrcVB = NULL;
    m_pRefVB = NULL;
    m_pSrcIB = NULL;
    m_pRefIB = NULL;
	m_bCmp = false;

	// alloc the view data
	m_pVMatrix = new D3DMATRIX;
	m_pvLoc = new D3DVECTOR;
	m_pvUp = new D3DVECTOR;
    m_pProjMatrix = new D3DMATRIX;

	// Set the test name -- different for stress mode
	if(KeySet(_T("stress")))
	{
		m_szTestName =	_T("StateBeast");
		m_szCommandKey=	_T("StateBeast");
//	    SetLogName(_T("StateBeast"));
	}
	else if(KeySet(_T("cmp")))
	{
		m_bCmp = true;
		m_szTestName = _T("StateCmp");
		m_szCommandKey = _T("StateCmp");
//		SetLogName(_T("StateCmp"));
		m_ModeOptions.fRefEqualsSrc = true;
		g_nSBT = 1;  // Disable Pixel/Vertex types - compare not valid
	}
	else
	{
		m_szCommandKey=	_T("StateBlocks");
		m_szTestName=	_T("StateBlocks");
	}
}


// ----------------------------------------------------------------------------

CStateBeast::~CStateBeast()
{
	// release the view data
    SAFEDELETE(m_pProjMatrix);
	SAFEDELETE(m_pVMatrix);
    SAFEDELETE(m_pvLoc);
    SAFEDELETE(m_pvUp);
}

// ----------------------------------------------------------------------------

bool CStateBeast::SetDefaultRenderStates(void)
{
	// Get the current screen extents
    int nWidth = (int)m_pDisplay->GetWidth();//m_pModeList->D3DMode.dwWidth;
    int nHeight = (int)m_pDisplay->GetHeight();//m_pModeList->D3DMode.dwHeight;

    // Create the viewports
    m_pViewports = (VIEWPORT*) malloc(m_nViewports * sizeof(VIEWPORT));
    if(m_pViewports)
    {
		// Store the default viewport as VP 0
        memset(&m_pViewports[0],0, sizeof(VIEWPORT));
		m_pViewports[0].dwWidth = nWidth;
        m_pViewports[0].dwHeight = nHeight;
        m_pViewports[0].dwX = 0;
        m_pViewports[0].dwY = 0;
		m_pViewports[0].dwMinZ = 0;
		m_pViewports[0].dwMaxZ = 1;

		// Create some other interesting viewports
		memset(&m_pViewports[1], 0, sizeof(VIEWPORT));
		m_pViewports[1].dwWidth = nWidth / 2;
        m_pViewports[1].dwHeight = nHeight / 2;
        m_pViewports[1].dwX = m_pViewports[1].dwWidth / 2;
        m_pViewports[1].dwY = m_pViewports[1].dwHeight / 2;
        m_pViewports[1].dwMinZ = 0;
		m_pViewports[1].dwMaxZ = 1;
    }
	else
	{
		WriteToLog(_T("SetDefaultRenderStates() - couldn't allocate viewports.\n"));
		return false;
	}
       
	if(m_dwVersion >= 0x0800)
		Init8();

    // Create a stateblock to hold all state
//    CreateStateBlock(D3DSBT_ALL, &m_OriginalBlock.dwSrcHandle, 
//                                 &m_OriginalBlock.dwRefHandle);
    m_pDevice->CreateStateBlock(D3DSBT_ALL, &m_OriginalBlock.dwSrcHandle);

	// Capture the current state
//    CaptureStateBlock(m_OriginalBlock.dwSrcHandle, m_OriginalBlock.dwRefHandle);   
    m_pDevice->CaptureStateBlock(m_OriginalBlock.dwSrcHandle);

    return true;
}

// ----------------------------------------------------------------------------

bool CStateBeast::SetDefaultMatrices(void)
{
    D3DMATRIX           Matrix;
    D3DVECTOR         at = cD3DVECTOR(0.f, 0.f, 0.f);

    *m_pvLoc = cD3DVECTOR(0.f,0.f,-2.5f);
    *m_pvUp  = cD3DVECTOR(0.0f,1.0f,0.0f);

    *m_pProjMatrix = ProjectionMatrix(1.0f,1000.0f,pi/4.0f);
    if(!SetTransform(D3DTRANSFORMSTATE_PROJECTION, m_pProjMatrix))
        return false;

    *m_pVMatrix = ViewMatrix(*m_pvLoc,at,*m_pvUp);
    if(!SetTransform(D3DTRANSFORMSTATE_VIEW, m_pVMatrix))
        return false;

    Matrix = IdentityMatrix();
    if(!SetTransform(D3DTRANSFORMSTATE_WORLD, &Matrix))
        return false;

    return true;
}
// ----------------------------------------------------------------------------

bool CStateBeast::SetDefaultMaterials(void)
{
        
    // set up the textures
//    CImageData *  pImageData;
//	CImageLoader  Image;
		
    const TCHAR   ptcsTextures[NTEXTURES][14] = {
			_T("checker.bmp"),
			_T("rocks.bmp"),
            _T("poko0.bmp"),
            _T("poko1.bmp"),
            _T("poko2.bmp"),
            _T("poko3.bmp"),
            _T("poko4.bmp"),
            _T("poko5.bmp"),
            _T("poko6.bmp"),
            _T("poko7.bmp")};

	for(int i = 0; i < NTEXTURES; i++)
	{
//	    pImageData = Image.Load(CIL_BMP, ptcsTextures[i]);
//		if(pImageData)
//		{
//			m_pTextures[i] = CreateTexture(pImageData->GetWidth(), pImageData->GetHeight(), 0, pImageData);
            m_pTextures[i] = (CTexture8*)CreateTexture(m_pDevice, ptcsTextures[i]);
//			RELEASE(pImageData);
			
			if(!m_pTextures[i])
			{
				WriteToLog(_T("SetDefaultMaterials - Texture creation failed.\n"));
				return false;
			}
//		}
//		else
//			return false;
    }
	
	// Create the default materials
//    m_pFrontMat = CreateMaterial();
	D3DMATERIAL8 Material;
    memset(&Material, 0 , sizeof(D3DMATERIAL8));
    Material.Diffuse.r = 1.0f;
    Material.Diffuse.g = 1.0f;
    Material.Diffuse.b = 1.0f;
    Material.Diffuse.a = 0.5f;
    Material.Ambient.r = 0.1f;
    Material.Ambient.g = 0.4f;   
    Material.Ambient.b = 0.1f; 
	Material.Specular.r = 1.f;
    Material.Specular.g = 1.f;
    Material.Specular.b = 1.f;
    Material.Power = 10.0f;
//    m_pFrontMat->SetMaterial(&Material);
    memcpy(&m_FrontMat, &Material, sizeof(D3DMATERIAL8));

//    m_pFrontMat2 = CreateMaterial();
    memset(&Material, 0 , sizeof(D3DMATERIAL8));
    Material.Diffuse.r = 1.0f;
    Material.Diffuse.g = 0.0f;
    Material.Diffuse.b = 0.0f;
    Material.Diffuse.a = 0.5f;
    Material.Ambient.r = 1.0f;
    Material.Ambient.g = 0.0f;   
    Material.Ambient.b = 0.0f; 
    Material.Power = 10.0f;
//    m_pFrontMat2->SetMaterial(&Material);
    memcpy(&m_FrontMat2, &Material, sizeof(D3DMATERIAL8));
	
//    m_pBackMat = CreateMaterial();
    memset(&Material, 0 , sizeof(D3DMATERIAL8));
    Material.Diffuse.r = 1.0f;
    Material.Diffuse.g = 1.0f;
    Material.Diffuse.b = 1.0f;
    Material.Diffuse.a = 0.5f;
    Material.Ambient.r = 1.0f;
    Material.Ambient.g = 0.0f;   
    Material.Ambient.b = 0.0f; 
    Material.Power = 10.0f;
//    m_pBackMat->SetMaterial(&Material);
    memcpy(&m_BackMat, &Material, sizeof(D3DMATERIAL8));

    return true;
}


// ----------------------------------------------------------------------------

bool CStateBeast::SetDefaultLights(void)
{
    D3DLIGHT8 LightData;
    
    // Create the CLIGHTS
//    for(int i=0; i<NLIGHTS; i++)
//        m_pLight[i] = CreateLight();

    
    // Check what kind of lights are available
//    DEVICEDESC SrcDesc = m_pAdapter->Devices[m_pMode->nSrcDevice].Desc;

    if(m_d3dcaps.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT)
    {
        m_dwPosLightsSupported = (m_d3dcaps.VertexProcessingCaps & D3DVTXPCAPS_POSITIONALLIGHTS);

        if(!m_dwPosLightsSupported && !(m_d3dcaps.VertexProcessingCaps & D3DVTXPCAPS_DIRECTIONALLIGHTS))
        {
            WriteToLog("(FAIL) T&L Device does not support at least Directional Lights.  Test cannot continue.\n");
            return false;
        }
    }
	else
		m_dwPosLightsSupported = TRUE;


    // Light0
    ZeroMemory(&LightData,sizeof(D3DLIGHT8));
    LightData.Type             = m_dwPosLightsSupported ? D3DLIGHT_POINT : D3DLIGHT_DIRECTIONAL;
    LightData.Diffuse.r        = 1.0f;
    LightData.Diffuse.g        = 1.0f;
    LightData.Diffuse.b        = 1.0f;
    LightData.Ambient.r        = 1.0f;
    LightData.Ambient.g        = 1.0f;
    LightData.Ambient.b        = 1.0f;
    LightData.Specular.r       = 1.0f;
    LightData.Specular.g       = 1.0f;
    LightData.Specular.b       = 1.0f;
    LightData.Position       = cD3DVECTOR(1.0f, 0.0f, -0.5f);
    LightData.Direction      = cD3DVECTOR(0.0f, 0.0f, 1.0f);
    LightData.Attenuation0   = 0.98f;
    LightData.Attenuation1   = 0.015f;
    LightData.Attenuation2   = 0.005f;
    LightData.Range          = 15.0f;
//    m_pLight[0]->SetLight(&LightData);
//    SetLight(0, m_pLight[0]);
//    LightEnable(0, FALSE);
    memcpy(&m_Light[0], &LightData, sizeof(D3DLIGHT8));
    m_pDevice->SetLight(0, &LightData);
    m_pDevice->LightEnable(0, TRUE);

                          
    // Light1
    LightData.Type             = m_dwPosLightsSupported ? D3DLIGHT_SPOT : D3DLIGHT_DIRECTIONAL;
    LightData.Range             = 15.0f;
    LightData.Position       = cD3DVECTOR(-1.0f, 0.0f, -1.0f);
    LightData.Direction      = cD3DVECTOR(0.0f, 0.0f, 1.0f);
    LightData.Specular.r       = 1.0f;
    LightData.Specular.g       = 1.0f;
    LightData.Specular.b       = 0.0f;
    LightData.Diffuse.r        = 0.0f;
    LightData.Diffuse.g        = 0.0f;
    LightData.Diffuse.b        = 1.0f;
    LightData.Theta             = pi / 2;
    LightData.Phi               = pi;
    LightData.Falloff           = 1.0f;
//    m_pLight[1]->SetLight(&LightData);
//    SetLight(1, m_pLight[1]);
//    LightEnable(1, FALSE);
    memcpy(&m_Light[1], &LightData, sizeof(D3DLIGHT8));
    m_pDevice->SetLight(1, &LightData);
    m_pDevice->LightEnable(1, TRUE);

    // Light2
    LightData.Position       = cD3DVECTOR(1.0f, 0.0f, -1.0f);
    LightData.Direction      = cD3DVECTOR(0.0f, 0.0f, 1.0f);
    LightData.Diffuse.r        = 1.0f;
    LightData.Diffuse.g        = 0.0f;
    LightData.Diffuse.b        = 0.0f;
    LightData.Specular.r       = 0.0f;
    LightData.Specular.g       = 0.0f;
    LightData.Specular.b       = 1.0f;
//    m_pLight[2]->SetLight(&LightData);
//    SetLight(2, m_pLight[2]);
//    LightEnable(2, FALSE);
    memcpy(&m_Light[2], &LightData, sizeof(D3DLIGHT8));
    m_pDevice->SetLight(2, &LightData);
    m_pDevice->LightEnable(2, TRUE);

    // Light3
    LightData.Position          = cD3DVECTOR(0.0f, 0.0f, -1.0f);
    LightData.Direction         = cD3DVECTOR(0.0f,0.0f,1.0f);
    LightData.Theta             = pi / 4;
    LightData.Phi               = pi / 3;
    LightData.Diffuse.r        = 0.0f;
    LightData.Diffuse.g        = 1.0f;
    LightData.Diffuse.b        = 0.0f;
    LightData.Specular.r       = 1.0f;
    LightData.Specular.g       = 1.0f;
    LightData.Specular.b       = 1.0f;
//    m_pLight[3]->SetLight(&LightData);
//    SetLight(3, m_pLight[3]);
//    LightEnable(3, FALSE);
    memcpy(&m_Light[3], &LightData, sizeof(D3DLIGHT8));
    m_pDevice->SetLight(3, &LightData);
    m_pDevice->LightEnable(3, TRUE);

    return true;
}

// ----------------------------------------------------------------------------

//bool CStateBeast::InitializeSB(void)
//{
//    return CD3DTest::Initialize();
//}


// ----------------------------------------------------------------------------

bool CStateBeast::PreDeviceSetup(bool fReference)
{
/*
	if(!KeySet(_T("stress")))
	{
        //
		// Disable EmulateStateBlocks regkey if needed
		// This must be done before the D3DDevice is initialized
		//
		
		HRESULT hr;
		HKEY hKey = (HKEY) NULL;

		hr = RegOpenKey(HKEY_LOCAL_MACHINE, "Software\\Microsoft\\Direct3D", &hKey); 
		if(hr == ERROR_SUCCESS)
		{
			hr = RegDeleteValue(hKey, "EmulateStateBlocks");
			
			if(hr != ERROR_SUCCESS && hr != ERROR_FILE_NOT_FOUND)
			{
				WriteToLog("Couldn't delete EmulateStateBlocks regkey. hr=%x\n", hr);
				return false;
			}
	
			RegCloseKey(hKey);
		}
		else
		{
			WriteToLog("Can't open Software\\Microsoft\\Direct3D regkey. D3D installed? hr=%x\n", hr);
			return false;        
		}
	}
*/
	return true;
}

// ----------------------------------------------------------------------------

void CStateBeast::CommandLineHelp(void)
{
//    WriteCommandLineHelp("$y-TESTBLOCK <int>\t$wTest a single stateblock");
//    WriteCommandLineHelp("$y-STARTWITH <int>\t$wStart execution at a specific stateblock");
//    WriteCommandLineHelp("$y-SWAP <int>\t$wSet the interval for swapping states");
//    WriteCommandLineHelp("$y-BEASTS <int>\t$wSet the number of beasts");
//    WriteCommandLineHelp("$y-!CYCLE\t$wDo not cylce the state blocks");
//    WriteCommandLineHelp("$y-SFEMUL\t$wDo not enforce DX7 DDI checking");
//    WriteCommandLineHelp("$y-!STATIC\t$wSet objects in motion");           
}

// ----------------------------------------------------------------------------

UINT CStateBeast::TestInitialize(void)
{              
	m_nStates = m_nTotalDX7 + m_nTotalDX8;
	m_nTotalBlocks = m_nStates * (g_nSBT + 1);

    // Allow software emulation if not supported in HW
    m_bSWEmulation = KeySet(_T("SFEmul")) ? true : false;
    
    // 
    // Validate the DX version, device caps
    //
   
    // Make sure we are on DX7
	if (m_dwVersion < 0x0700)
	{
        SetTestRange(1,1);

        WriteToLog(_T("This test requires Dx7.\n"));
		ReleaseAll();     
        return D3DTESTINIT_SKIPALL;
	}
    
	if(!KeySet(_T("stress")))
	{
		// Make sure that we're on a DX7 DDI
		DWORD dwDevCaps = m_d3dcaps.DevCaps;
		if (!(dwDevCaps & D3DDEVCAPS_DRAWPRIMITIVES2EX))
		{   
			if(m_bSWEmulation)
			{
				WriteToLog(_T("No DX7 DDI available, running with software emulation.\n"));
			}
			else
			{
				SetTestRange(1,1);

				WriteToLog(_T("\nTestInitialize(ERROR) - This test requires a Dx7 Driver. Exiting.\n"));
				ReleaseAll();
				return D3DTESTINIT_SKIPALL;
			}
		}
	}

	// Init some vars
    m_nUnInitLights = 0;
	m_nLightOff = 10;

    // Generate the geometry
	if (!m_FrontPlane.NewShape(CS_MESH, 3, 3)) {
        return false;
    }
	m_FrontPlane.Scale(2,2,2);
    if (!m_FrontPlane2.NewShape(CS_MESH, 3, 3)) {
        return false;
    }
    m_FrontPlane2.Scale(2,2,2);
    m_FrontPlane2.Translate(0,0,0.1f);
    if (!m_BackPlane.NewShape(CS_MESH, 3, 3)) {
        return false;
    }
	m_BackPlane.Scale(2,2,2);
	m_BackPlane.Rotate(0, pi, 0);
	m_BackPlane.Translate(0,0,0.2f);
	if(m_dwVersion > 0x700)
	{
       	// Create the VB/IB
		if(!SetupBuffers(&m_FrontPlane, &m_pSrcVB, &m_pSrcIB, false) ||
		   (m_ModeOptions.fReference && !SetupBuffers(&m_FrontPlane, &m_pRefVB, &m_pRefIB, true)))
		{
			WriteToLog(_T("CStateBeast::TestInitialize() - buffer setup failed."));
			return false;
		}
	}

	// Allocate the stateblock data
    m_pBlocks = new BLOCKINFO[m_nTotalBlocks];
	if(m_pBlocks == NULL)
	{
		WriteToLog(_T("TestInitialize() - Out of memory."));
		return false;
	}
	memset(m_pBlocks, 0, m_nTotalBlocks * sizeof(BLOCKINFO));

    // Create the state blocks, checking for valid caps
    CreateStateBlocks();

	// Set the test range
	SetTestRange( 1, m_nTotalBlocks);

    return D3DTESTINIT_RUN;
}

// ----------------------------------------------------------------------------

void CStateBeast::ClearState()
{
	for(int i=0; i<m_nUnInitLights; i++)
		LightEnable(m_nLightOff+i, FALSE);

    if(m_dwVersion > 0x700)
		ClearState8();
	
    m_pDevice->ApplyStateBlock(m_OriginalBlock.dwSrcHandle);//,m_OriginalBlock.dwRefHandle);
}

// ----------------------------------------------------------------------------

void CStateBeast::PostApply(UINT nBlock)
{
	DWORD dwSBT = m_pBlocks[m_nCurrentBlock].dwCreateType;
	int nState = nBlock / (g_nSBT + 1);

	switch(nState)
	{
    case 10:
		SetRenderState(D3DRENDERSTATE_AMBIENT,RGBA_MAKE(255,255,255,0));
		m_pDevice->SetMaterial(&m_FrontMat2);
        // Render the second plane
		if(!RenderPrimitive(D3DPT_TRIANGLELIST, D3DFVF_VERTEX,
						m_FrontPlane2.m_pVertices, 
						m_FrontPlane2.m_nVertices,
						m_FrontPlane2.m_pIndices, 
						m_FrontPlane2.m_nIndices,
						0))
		{
			WriteToLog(_T("CStateBeast::SceneRefresh -- RenderPrimitive failed, hr=%X\n"),
						GetLastError());
			Fail();
		}
		break;
	case 2:
	case 3:
	case 7:
	case 11:
	case 18:
	case 21:
	case 22:
	case 24:
	case 25:
		{
			SetRenderState(D3DRENDERSTATE_AMBIENT,RGBA_MAKE(255,255,255,0));
		}
		break;
	case 26:
		{
			DWORD dwSrc, dwRef;
			m_pDevice->BeginStateBlock();
            SetTexture( 0, NULL);
			m_pDevice->EndStateBlock(&dwSrc);//, &dwRef);
		}
		break;
	default:
		PostApply8(nState-m_nTotalDX7);
	}
}
// ----------------------------------------------------------------------------

bool CStateBeast::ExecuteTest(UINT uTestNumber)
{
	m_nCurrentBlock = uTestNumber-1;

	// Begin the test case
    BeginTestCase(m_pBlocks[m_nCurrentBlock].tcsDesc);

	if(!m_pBlocks[m_nCurrentBlock].dwSrcHandle)// && !m_pBlocks[m_nCurrentBlock].dwRefHandle)
	{
//		SkipTestRange(uTestNumber, uTestNumber);
        SkipTests(1);
		return false;
	}

	// Render the scene
    BeginScene();

    SwitchAPI((UINT)m_fFrame);

    m_pDevice->SetViewport(&m_vpFull);
//    m_pDevice->Clear(0, NULL, D3DCLEAR_TARGET, RGB_MAKE(74, 150, 202), 1.0f, 0);

//    m_pDevice->SetViewport(&m_vpTest);

    ClearFrame();
    
    // Reset the state
    ClearState();
    
	if(m_bCmp)
	{
		int nState = m_nCurrentBlock / (g_nSBT + 1);
		m_pDevice->ApplyStateBlock(m_pBlocks[m_nCurrentBlock].dwSrcHandle);//,0);
		SetState(nState, NULL, NULL, CD3D_REF);
	}
	else
	{
		// Apply  the stateblock
		m_pDevice->ApplyStateBlock(m_pBlocks[m_nCurrentBlock].dwSrcHandle);//,
//						m_pBlocks[m_nCurrentBlock].dwRefHandle);
	}
	
	// Do any post apply setup
	PostApply(m_nCurrentBlock);

	if(m_pBlocks[m_nCurrentBlock].nVersion == 7)
	{
		m_pDevice->SetMaterial(&m_FrontMat);
	
		// Render the plane
		if(!RenderPrimitive(D3DPT_TRIANGLELIST, D3DFVF_VERTEX,
							m_FrontPlane.m_pVertices, 
							m_FrontPlane.m_nVertices,
							m_FrontPlane.m_pIndices, 
							m_FrontPlane.m_nIndices,
							0))
			{
				WriteToLog(_T("CStateBeast::ExecuteTest -- RenderPrimitive failed, hr=%s\n"),
							GetLastError());
				Fail();
			}
	
	
		// Render the back plane
		m_pDevice->SetMaterial(&m_BackMat);        
		// Render the plane
		if(!RenderPrimitive(D3DPT_TRIANGLELIST, D3DFVF_VERTEX,
							m_BackPlane.m_pVertices, 
							m_BackPlane.m_nVertices,
							m_BackPlane.m_pIndices, 
							m_BackPlane.m_nIndices,
							0))
			{
				WriteToLog(_T("CStateBeast::ExecuteTest -- RenderPrimitive failed, hr=%s\n"),
							GetLastError());
				Fail();
			}
	}
	else if(m_pBlocks[m_nCurrentBlock].nVersion == 8)
	{
//		DrawIndexedPrimitive8(D3DPT_TRIANGLELIST, 0,(DWORD)m_FrontPlane.m_nVertices, 
//								 0, (DWORD)m_FrontPlane.m_nIndices / 3, CD3D_BOTH);
        m_pDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0,(DWORD)m_FrontPlane.m_nVertices, 
								 0, (DWORD)m_FrontPlane.m_nIndices / 3);
	}
	else
	{
		WriteToLog(_T("CStateBeast::ExecuteTest -- Unknown stateblock version.\n"));
		Fail();
	}

    // Display the adapter mode
    ShowDisplayMode();

    // Display the frame rate
    ShowFrameRate();

    // Display the console
    m_pDisplay->ShowConsole();

	EndScene();

    // Update the screen
    m_pDevice->Present(NULL, NULL, NULL, NULL);

//    return true;
    return false;
}

// ----------------------------------------------------------------------------

bool CStateBeast::TestTerminate(void)
{
    ReleaseAll();

    return true;
}

// ----------------------------------------------------------------------------

bool CStateBeast::ProcessFrame(void)
{
    char	szBuffer[80];
	static	int nPass = 0;
	static  int nFail = 0;
    bool	bResult;

	// Use the standard 15% comparison
	bResult = GetCompareResult(0.15f, 0.78f, 0);

	// Tell the logfile how they did
	if (bResult)
	{
		(m_fIncrementForward) ? nPass++ : nPass--;
		sprintf(szBuffer,"$yPass: %d, Fail: %d", nPass, nFail);
		Pass();
	}
	else
	{
		(m_fIncrementForward) ? nFail++ : nFail--;
		sprintf(szBuffer,"$yPass: %d, Fail: %d", nPass, nFail);
		Fail();
	}

	WriteStatus("$gOverall Results",szBuffer);

    // Output the block number to the display
    WriteStatus(_T("$gBlock Description"), _T("$y%s"), m_pBlocks[m_nCurrentBlock].tcsDesc);

    return bResult;
}

// ----------------------------------------------------------------------------

void CStateBeast::CreateStateBlocks()
{   
	TCHAR tcsBuffer[MAX_PATH];
	memset(tcsBuffer, 0, MAX_PATH * sizeof(TCHAR));
	DWORD dwState = 0;

    for(int i=0; i<m_nStates; i++)
	{
		bool bSuccess;
		
		// Create the stateblocks with the Begin / End
		m_pDevice->BeginStateBlock();
		bSuccess = SetState(i, tcsBuffer, &(m_pBlocks[dwState].nVersion));
        m_pDevice->EndStateBlock(&(m_pBlocks[dwState].dwSrcHandle));//, 
//                      &(m_pBlocks[dwState].dwRefHandle));
		_stprintf(m_pBlocks[dwState].tcsDesc, _T("Set %s within Begin/End"),tcsBuffer);
      
		if(!bSuccess)
		{
			m_pDevice->DeleteStateBlock(m_pBlocks[dwState].dwSrcHandle);//,m_pBlocks[dwState].dwRefHandle);
            m_pBlocks[dwState].dwSrcHandle = 0;
			m_pBlocks[dwState].dwRefHandle = 0;
		}
        m_pBlocks[dwState].dwCreateType = D3DSBT_CUSTOM;
		dwState++;

		// For each SBT, create a stateblock and capture the state
		for(int j=0; j< g_nSBT; j++)
		{            
			// Create the stateblock
			m_pDevice->CreateStateBlock((D3DSTATEBLOCKTYPE)g_SBT[j].dwFlags, &m_pBlocks[dwState].dwSrcHandle);//, &m_pBlocks[dwState].dwRefHandle);
			// Set the state
            bSuccess = SetState(i, tcsBuffer, &(m_pBlocks[dwState].nVersion));

            _stprintf(m_pBlocks[dwState].tcsDesc, _T("Capture %s in %s block"),tcsBuffer, g_SBT[j].tcsDesc);
			if(!bSuccess)
			{
				m_pDevice->DeleteStateBlock(m_pBlocks[dwState].dwSrcHandle);//,m_pBlocks[dwState].dwRefHandle);
				m_pBlocks[dwState].dwSrcHandle = 0;
				m_pBlocks[dwState].dwRefHandle = 0;
			}
			else
			{
				// Capture the new state
				m_pDevice->CaptureStateBlock(m_pBlocks[dwState].dwSrcHandle);//,m_pBlocks[dwState].dwRefHandle);
				
				// Reset the state
				ClearState();
			}
            m_pBlocks[dwState].dwCreateType = g_SBT[j].dwFlags;
			dwState++;
		}
	}
}

bool CStateBeast::SetState(int nIndex, TCHAR *szName, int *nVersion, DWORD dwFlags)
{
//    UINT n = m_pMode->nSrcDevice;
//    DEVICEDESC SrcDesc = m_pAdapter->Devices[m_pMode->nSrcDevice].Desc;
	if(nVersion) *nVersion = 7;
    
	switch(nIndex)
	{
	case 0:
        if(szName) _tcscpy(szName, _T("No state(empty)"));
		return true;		  
	case 1:
        if(szName) _tcscpy(szName, _T("RS_AMBIENT"));
		SetRenderState(D3DRENDERSTATE_AMBIENT,RGBA_MAKE(255,255,255,0), dwFlags);
        return true;
	case 2:
		if(szName) _tcscpy(szName, _T("RS_FILLMODE"));
        SetRenderState(D3DRENDERSTATE_FILLMODE, (DWORD) D3DFILL_SOLID, dwFlags);
        return true;
	case 3:
        if(szName) _tcscpy(szName, _T("RS_CULLMODE"));
        SetRenderState(D3DRENDERSTATE_CULLMODE, (DWORD) D3DCULL_CW, dwFlags);
        return true;   
	case 4:
        if(szName) _tcscpy(szName, _T("SetLight, Enable=false"));
		m_pDevice->SetLight(0, &m_Light[0]);
        LightEnable(0, (DWORD) FALSE, dwFlags);
        SetRenderState(D3DRENDERSTATE_SHADEMODE, (DWORD) D3DSHADE_FLAT, dwFlags);
        return true;
	case 5:
        if(szName) _tcscpy(szName, _T("SetLight, Enable=true, RS_SHADEMODE(GOURAUD)"));
        if(m_d3dcaps.ShadeCaps & D3DPSHADECAPS_COLORGOURAUDRGB)
		{
			m_pDevice->SetLight(0, &m_Light[0]);
			LightEnable(0, (DWORD) TRUE, dwFlags);
			SetRenderState(D3DRENDERSTATE_SHADEMODE, (DWORD) D3DSHADE_GOURAUD, dwFlags);
			SetRenderState(D3DRENDERSTATE_CULLMODE, (DWORD) D3DCULL_CCW, dwFlags);
			return true;
		}
        break;
	case 6:
        if(szName) _tcscpy(szName, _T("SetTexture"));
        if(m_d3dcaps.TextureOpCaps & D3DTEXOPCAPS_SELECTARG1)
		{
			SetTexture( 0, m_pTextures[1], dwFlags);
			SetTextureStageState((DWORD)0, D3DTSS_COLOROP, (DWORD)D3DTOP_SELECTARG1, dwFlags);
			SetTextureStageState((DWORD)0, D3DTSS_COLORARG1, (DWORD)D3DTA_TEXTURE, dwFlags);
			return true;
		}
        break;
	case 7:
		if(szName) _tcscpy(szName, _T("COLOROP = MODULATE"));
		if(m_d3dcaps.TextureOpCaps & D3DTEXOPCAPS_MODULATE)
		{
			SetTexture( 0, m_pTextures[0], dwFlags);
			SetTextureStageState((DWORD)0, D3DTSS_COLOROP, (DWORD)D3DTOP_MODULATE, dwFlags);
			SetTextureStageState((DWORD)0, D3DTSS_COLORARG1, (DWORD)D3DTA_TEXTURE, dwFlags);
			SetTextureStageState((DWORD)0, D3DTSS_COLORARG2, (DWORD)D3DTA_DIFFUSE, dwFlags);
			return true;
		}
		break;
   	case 8:
		if(szName) _tcscpy(szName, _T("COLOROP = MODULATE, Light"));
		if(m_d3dcaps.TextureOpCaps & D3DTEXOPCAPS_MODULATE)
		{
			m_pDevice->SetLight(0, &m_Light[0]);
			LightEnable(0, (DWORD) TRUE, dwFlags);
			SetTexture( 0, m_pTextures[0], dwFlags);
			SetTextureStageState((DWORD)0, D3DTSS_COLOROP, (DWORD)D3DTOP_MODULATE, dwFlags);
			SetTextureStageState((DWORD)0, D3DTSS_COLORARG1, (DWORD)D3DTA_TEXTURE, dwFlags);
			SetTextureStageState((DWORD)0, D3DTSS_COLORARG2, (DWORD)D3DTA_DIFFUSE, dwFlags);
			return true;
		}
		break;
	case 9:
        if(szName) _tcscpy(szName, _T("MAGFILTER,MINFILTER = TEXF_LINEAR"));
        if((m_d3dcaps.TextureFilterCaps & D3DPTFILTERCAPS_MINFLINEAR) &&
           (m_d3dcaps.TextureFilterCaps & D3DPTFILTERCAPS_MAGFLINEAR))
        {
			m_pDevice->SetLight(0, &m_Light[0]);
			LightEnable(0, (DWORD) TRUE, dwFlags);
			SetTexture( 0, m_pTextures[1], dwFlags);
			SetTextureStageState((DWORD)0, D3DTSS_MAGFILTER, (DWORD)TEXF_LINEAR, dwFlags);
			SetTextureStageState((DWORD)0, D3DTSS_MINFILTER, (DWORD)TEXF_LINEAR, dwFlags);
			return true;
		}
        break;
	case 10:
        if(szName) _tcscpy(szName, _T("ALPHABLENDENABLE=TRUE"));
		if( (m_d3dcaps.TextureOpCaps & D3DTEXOPCAPS_MODULATE) && 
            (m_d3dcaps.DestBlendCaps & D3DPBLENDCAPS_INVSRCALPHA) &&
            (m_d3dcaps.SrcBlendCaps & D3DPBLENDCAPS_SRCALPHA))
		{
			SetRenderState(D3DRENDERSTATE_ALPHABLENDENABLE, (DWORD) TRUE, dwFlags);
			SetRenderState(D3DRENDERSTATE_SRCBLEND,  (DWORD)D3DBLEND_SRCALPHA, dwFlags);
			SetRenderState(D3DRENDERSTATE_DESTBLEND, (DWORD)D3DBLEND_INVSRCALPHA, dwFlags);
			SetTexture( 0, m_pTextures[0], dwFlags);
			SetTextureStageState((DWORD)0, D3DTSS_ALPHAOP, (DWORD)D3DTOP_MODULATE, dwFlags);
			SetTextureStageState((DWORD)0, D3DTSS_ALPHAARG1, (DWORD)D3DTA_TEXTURE, dwFlags);
			SetTextureStageState((DWORD)0, D3DTSS_ALPHAARG2, (DWORD)D3DTA_DIFFUSE, dwFlags);
			return true;
		}
        break;
	case 11:
		{
            DWORD dwColor = 0x0000FF;
            float fStart = -1.0f, fEnd = 1.0f;
			if(szName) _tcscpy(szName, _T("FOGENABLE=true"));
			if( (m_d3dcaps.TextureOpCaps & D3DTEXOPCAPS_MODULATE) && 
				(m_d3dcaps.RasterCaps & D3DPRASTERCAPS_FOGVERTEX) &&
				(m_d3dcaps.ShadeCaps & D3DPSHADECAPS_COLORGOURAUDRGB))
			{
				m_pDevice->SetLight(0, &m_Light[0]);
				LightEnable(0, (DWORD) TRUE, dwFlags);
				SetRenderState(D3DRENDERSTATE_SHADEMODE, (DWORD)D3DSHADE_GOURAUD, dwFlags);
				SetRenderState(D3DRENDERSTATE_FOGENABLE,     (DWORD) TRUE, dwFlags);
#ifndef UNDER_XBOX
				SetRenderState(D3DRENDERSTATE_FOGVERTEXMODE, (DWORD) D3DFOG_LINEAR, dwFlags);
#else
                SetRenderState(D3DRENDERSTATE_FOGTABLEMODE, (DWORD) D3DFOG_LINEAR, dwFlags);
#endif // UNDER_XBOX
				SetRenderState(D3DRENDERSTATE_FOGCOLOR, dwColor, dwFlags);
				SetRenderState(D3DRENDERSTATE_FOGSTART, *(DWORD*)&fStart, dwFlags);
				SetRenderState(D3DRENDERSTATE_FOGEND,   *(DWORD*)&fEnd, dwFlags);
				return true;
			}
			break;        
		}
    case 12:
	case 13:
	case 14:
	case 15:
	case 16:
	case 17:
    case 18:
		{
			int nTexIndex = 2;
			int nTexStages =  m_d3dcaps.MaxSimultaneousTextures;
			int nRequested =  nIndex-10;
			if(szName) _stprintf(szName, _T("%d Stage multi-texturing"), nRequested);
			if(nRequested <= nTexStages)
			{
				for(int j=0; j< nRequested; j++)
				{
					if(j == 0)
					{
						SetTexture( 0, m_pTextures[nTexIndex], dwFlags);
						SetTextureStageState((DWORD)0, D3DTSS_COLOROP, (DWORD)D3DTOP_SELECTARG1, dwFlags);
						SetTextureStageState((DWORD)0, D3DTSS_COLORARG1, (DWORD)D3DTA_TEXTURE, dwFlags);
						SetTextureStageState((DWORD)0, D3DTSS_ALPHAOP, (DWORD)D3DTOP_SELECTARG1, dwFlags);
						SetTextureStageState((DWORD)0, D3DTSS_ALPHAARG1, (DWORD)D3DTA_TEXTURE, dwFlags);
					}
					else
					{
						SetTexture( j, m_pTextures[nTexIndex+j], dwFlags);
						SetTextureStageState((DWORD)j, D3DTSS_COLOROP, (DWORD)D3DTOP_MODULATE, dwFlags);
						SetTextureStageState((DWORD)j, D3DTSS_COLORARG1, (DWORD)D3DTA_TEXTURE, dwFlags);
						SetTextureStageState((DWORD)j, D3DTSS_COLORARG2, (DWORD)D3DTA_CURRENT, dwFlags);
						SetTextureStageState((DWORD)0, D3DTSS_ALPHAOP, (DWORD)D3DTOP_SELECTARG2, dwFlags);
						SetTextureStageState((DWORD)0, D3DTSS_ALPHAARG2, (DWORD)D3DTA_CURRENT, dwFlags);
					}
				}
				return true;
			}
		}
		break;
	case 19:
		if(szName) _tcscpy(szName, _T("Spotlights"));
		if(m_dwPosLightsSupported && 
		   (m_d3dcaps.TextureOpCaps & D3DTEXOPCAPS_MODULATE))
		{
			m_pDevice->SetLight(0, &m_Light[1]);
			m_pDevice->SetLight(1, &m_Light[2]);
			m_pDevice->SetLight(2, &m_Light[3]);
			LightEnable(0, (DWORD) TRUE, dwFlags);
			LightEnable(1, (DWORD) TRUE, dwFlags);
			LightEnable(2, (DWORD) TRUE, dwFlags);
			SetTexture( 0, m_pTextures[1], dwFlags);
			SetTextureStageState((DWORD)0, D3DTSS_COLOROP, (DWORD)D3DTOP_MODULATE, dwFlags);
			SetTextureStageState((DWORD)0, D3DTSS_COLORARG1, (DWORD)D3DTA_TEXTURE, dwFlags);
			SetTextureStageState((DWORD)0, D3DTSS_COLORARG2, (DWORD)D3DTA_DIFFUSE, dwFlags);
			return true;
		}
        break;
	case 20:
		if(szName) _tcscpy(szName, _T("Specular lighting"));
		if((m_d3dcaps.ShadeCaps & D3DPSHADECAPS_SPECULARGOURAUDRGB) &&
           m_dwPosLightsSupported)
		{
			m_pDevice->SetLight(0, &m_Light[1]);
			m_pDevice->SetLight(1, &m_Light[2]);
			m_pDevice->SetLight(2, &m_Light[3]);
			LightEnable(0, (DWORD) TRUE, dwFlags);
			LightEnable(1, (DWORD) TRUE, dwFlags);
			LightEnable(2, (DWORD) TRUE, dwFlags);
			SetRenderState(D3DRENDERSTATE_SPECULARENABLE, (DWORD) TRUE, dwFlags);
			return true;
		}
		break;
	case 21:
        if(szName) _tcscpy(szName, _T("Viewport"));  		
        SetViewport(&m_pViewports[1], dwFlags);
        return true;			
	case 22:

#ifndef UNDER_XBOX

        {
            D3DVALUE p1[4];
			if(szName) _tcscpy(szName, _T("Local UD clip-plane"));
			if(m_d3dcaps.MaxUserClipPlanes > 0)
			{
				memset(&p1, 0, sizeof(D3DVALUE) * 4);
				p1[0] = -0.7071067f; 
				p1[2] = -0.7071067f;
				p1[3] =  0.0f;
				SetClipPlane(0, (D3DVALUE*) &p1, dwFlags);
				SetRenderState(D3DRENDERSTATE_CLIPPLANEENABLE, (DWORD)D3DCLIPPLANE0, dwFlags);
				return true;
			}
		}

#else

        if(szName) _tcscpy(szName, _T("Local spot lights"));  		
		{
			D3DLIGHT8 LightData;
//			CLight *pLight = NULL;
//			pLight = CreateLight();
			ZeroMemory(&LightData,sizeof(D3DLIGHT8));
			LightData.Type             = D3DLIGHT_SPOT;
			LightData.Diffuse.r        = 1.0f;
			LightData.Diffuse.g        = 1.0f;
			LightData.Diffuse.b        = 1.0f;
			LightData.Ambient.r        = 1.0f;
			LightData.Ambient.g        = 1.0f;
			LightData.Ambient.b        = 1.0f;
			LightData.Specular.r       = 1.0f;
			LightData.Specular.g       = 1.0f;
			LightData.Specular.b       = 1.0f;
			LightData.Position       = cD3DVECTOR(1.0f, 0.0f, -0.5f);
			LightData.Direction      = cD3DVECTOR(0.0f, 0.0f, 1.0f);
			LightData.Attenuation0   = 0.98f;
			LightData.Attenuation1   = 0.015f;
			LightData.Attenuation2   = 0.005f;
			LightData.Range          = 15.0f;
//            pLight->SetLight(&LightData);
//			SetLight(0, pLight, dwFlags);
//            pLight->Release();
            m_pDevice->SetLight(0, &LightData);
			return true;
		}

#endif // UNDER_XBOX

		break;
	case 23:
        if(szName) _tcscpy(szName, _T("Local lights"));  		
		{
			D3DLIGHT8 LightData;
//			CLight *pLight = NULL;
//			pLight = CreateLight();
			ZeroMemory(&LightData,sizeof(D3DLIGHT8));
			LightData.Type             = m_dwPosLightsSupported ? D3DLIGHT_POINT : D3DLIGHT_DIRECTIONAL;
			LightData.Diffuse.r        = 1.0f;
			LightData.Diffuse.g        = 1.0f;
			LightData.Diffuse.b        = 1.0f;
			LightData.Ambient.r        = 1.0f;
			LightData.Ambient.g        = 1.0f;
			LightData.Ambient.b        = 1.0f;
			LightData.Specular.r       = 1.0f;
			LightData.Specular.g       = 1.0f;
			LightData.Specular.b       = 1.0f;
			LightData.Position       = cD3DVECTOR(1.0f, 0.0f, -0.5f);
			LightData.Direction      = cD3DVECTOR(0.0f, 0.0f, 1.0f);
			LightData.Attenuation0   = 0.98f;
			LightData.Attenuation1   = 0.015f;
			LightData.Attenuation2   = 0.005f;
			LightData.Range          = 15.0f;
//            pLight->SetLight(&LightData);
//			SetLight(0, pLight, dwFlags);
//            pLight->Release();
            m_pDevice->SetLight(0, &LightData);
			return true;
		}
		break;
	case 24:
        if(szName) _tcscpy(szName, _T("Local viewport"));
		{
			VIEWPORT vp;
            memset(&vp,0, sizeof(VIEWPORT));
			vp.dwWidth = m_pDisplay->GetWidth() / 2;//m_pModeList->D3DMode.dwWidth/2;
			vp.dwHeight = m_pDisplay->GetHeight() / 2;//m_pModeList->D3DMode.dwHeight/2;
			vp.dwX = 0;
			vp.dwY = 0;
			vp.dwMinZ = 0;
			vp.dwMaxZ = 1;
            SetViewport(&vp, dwFlags);
			return true;
		}
		break;
  	case 25:
        if(szName) _tcscpy(szName, _T("Local TRANSFORM"));
		{
            D3DMATRIX mat;
            mat = RotateYMatrix(pi / 3.f);
			SetTransform(D3DTRANSFORMSTATE_WORLD, &mat, dwFlags);
			return true;
		}
		break;
	case 26:
        if(szName) _tcscpy(szName, _T("record != set state"));
		if(m_d3dcaps.TextureOpCaps & D3DTEXOPCAPS_SELECTARG1)
		{
			SetTexture( 0, m_pTextures[0], dwFlags);
			SetTextureStageState((DWORD)0, D3DTSS_COLOROP, (DWORD)D3DTOP_SELECTARG1, dwFlags);
			SetTextureStageState((DWORD)0, D3DTSS_COLORARG1, (DWORD)D3DTA_TEXTURE, dwFlags);
			return true;
		}
		break;
	case 27:
        if(szName) _tcscpy(szName, _T("LightEnable with uninitialized light"));
		{
			LightEnable(m_nLightOff + m_nUnInitLights++, TRUE, dwFlags);
			return true;
		}
		break;

	default:
		return SetState8(nIndex-m_nTotalDX7, szName, nVersion, dwFlags);
	}
    return false;    
}

bool CStateBeast::SetClipPlane(DWORD nId, D3DVALUE *pPlane, DWORD dwFlags)
{
    return CD3DTest::SetClipPlane(nId, pPlane, dwFlags);
}

bool CStateBeast::SetViewport(VIEWPORT *pVp, DWORD dwFlags)
{
	return CD3DTest::SetViewport(pVp->dwX, pVp->dwY,
					   pVp->dwWidth, pVp->dwHeight,
					   pVp->dwMinZ, pVp->dwMaxZ, dwFlags);
}

void CStateBeast::ReleaseAll()
{
	if(m_pBlocks)
	{
		for(int i=0; i<m_nTotalBlocks; i++)
		{
			if(m_pBlocks[i].dwSrcHandle)
				m_pDevice->DeleteStateBlock(m_pBlocks[i].dwSrcHandle);//,m_pBlocks[i].dwRefHandle);
		}
       	delete m_pBlocks;
		m_pBlocks = NULL;
	}

	if(m_OriginalBlock.dwSrcHandle)
	{
		m_pDevice->DeleteStateBlock(m_OriginalBlock.dwSrcHandle);//, m_OriginalBlock.dwRefHandle);
		memset(&m_OriginalBlock, 0, sizeof(BLOCKINFO));
	}

	if(m_dwVSShader)
	{
		m_pDevice->DeleteVertexShader(m_dwVSShader);
		m_dwVSShader = 0;
	}

	if(m_dwPSShader)
	{
        m_pDevice->SetPixelShader(NULL);
		m_pDevice->DeletePixelShader(m_dwPSShader);
		m_dwPSShader = 0;
	}

	ReleaseVertexBuffer(m_pSrcVB);
    ReleaseVertexBuffer(m_pRefVB);
    ReleaseIndexBuffer(m_pSrcIB);
    ReleaseIndexBuffer(m_pRefIB);
	
    
    // Delete the app tools
/*
    for(int i=0; i<NLIGHTS; i++)
    {
        if(m_pLight[i])
        {
            RELEASE(m_pLight[i]);
            m_pLight[i] = NULL;
        }
    }
*/
    for(int i=0; i<NTEXTURES; i++)
    {
        if(m_pTextures[i])
        {
            ReleaseTexture(m_pTextures[i]);
            m_pTextures[i] = NULL;
        }
    }
    
//	if(m_pFrontMat)
//	{
//		RELEASE(m_pFrontMat);
//		m_pFrontMat = NULL;
//	}

//	if(m_pBackMat)
//	{
//		RELEASE(m_pBackMat);
//		m_pBackMat = NULL;
//	}


    if(m_pViewports)
    {
        free(m_pViewports);
        m_pViewports = NULL;
    }
}
