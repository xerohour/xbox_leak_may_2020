#include "d3dlocus.h"
#include "cd3dtest.h"
#include "3dmath.h"
#include "cshapes.h"
#include "MultiLight.h"

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

    MultiLight* pMultiLight;
    UINT        i;
    BOOL        bQuit = FALSE, bRet = TRUE;

    // Verify that the display has been initialized
    if (!pDisplay) {
        return FALSE;
    }

    // Run through all five scenarios for the test
    for (i = 0; i < 5 && !bQuit && bRet; i++) {

        // Create the scene
        pMultiLight = new MultiLight(i);
        if (!pMultiLight) {
            return FALSE;
        }

        // Initialize the scene
        if (!pMultiLight->Create(pDisplay)) {
            pMultiLight->Release();
            return FALSE;
        }

        bRet = pMultiLight->Exhibit(pnExitCode) && bRet;

        bQuit = pMultiLight->AbortedExit();

        // Clean up the scene
        pMultiLight->Release();
    }

    return bRet;
}

MultiLight::MultiLight(DWORD dwScenario)
{
	m_dwScenario = dwScenario;
	m_dwMaxLights = 300;
	
//	m_pMaterial = NULL;
	m_pTexture = NULL;
	m_dwLightsNumber = 0;
	
	m_pStars = NULL;
	m_dwStarsNumber = 300;
    m_fScale = 2.f;

	m_dwCurrentTestNumber = 0;
	m_dwNumFailed = 0;

	m_szTestName = TEXT("MultiLight");
	m_szCommandKey = TEXT("MultiLight");

    m_bExit = FALSE;
}

MultiLight::~MultiLight()
{
	if (m_pStars)
		delete [] m_pStars;

//	RELEASE(m_pMaterial);
	ReleaseTexture(m_pTexture);
    m_pTexture = NULL;
}


UINT MultiLight::TestInitialize(void)
{
	
    // Check multiple lights support
    if (m_d3dcaps.MaxActiveLights < 4)
    {
        WriteToLog("Multilight: Device does not support multiple lights\n");
        SetTestRange(1, 1);
        return D3DTESTINIT_SKIPALL;
    }
    // Check directional lights support
    if (NULL == (m_d3dcaps.VertexProcessingCaps & D3DVTXPCAPS_DIRECTIONALLIGHTS))
    {
        WriteToLog("Multilight: Device does not support directional lights\n");
        SetTestRange(1, 1);
        return D3DTESTINIT_SKIPALL;
    }
    
    
    // Read input parameters
	int iLights, iStars;
	ReadInteger("LIGHTS", 300, &iLights);
	ReadInteger("STARS", 300, &iStars);
	m_dwMaxLights = (DWORD)iLights;
	m_dwStarsNumber = (DWORD)iStars;

    // Set maximum number of lights to the hardware limitation
    if (m_dwMaxLights > m_d3dcaps.MaxActiveLights)
        m_dwMaxLights = m_d3dcaps.MaxActiveLights;

	// Vertices
	m_pSphere = new CShapes;
	if (NULL == m_pSphere)
		return D3DTESTINIT_ABORT;
	if (!m_pSphere->NewShape(CS_SPHERE)) {
        return D3DTESTINIT_ABORT;
    }

	m_pSphere->Scale(20.f);

	// Texture
//	CImageLoader	Loader;
//	CImageData*		pImage = NULL;
	
//	pImage = Loader.Load(CIL_BMP, "EARTH.BMP");
//	m_pTexture = CreateTexture(256,256,NULL,pImage);
//	RELEASE(pImage);
    m_pTexture = (CTexture8*)CreateTexture(m_pDevice, TEXT("earth.bmp"),
                                D3DFMT_A8R8G8B8, TTYPE_TEXTURE,
                                NULL, 1.0f, COLORKEY_NONE,
                                POOL_DEFAULT, 256, 256);
	if (m_pTexture == NULL)
		return D3DTESTINIT_ABORT;
	SetTexture(0, m_pTexture);

	// Stars
	if (m_pStars)
	{
		delete [] m_pStars;
		m_pStars = NULL;
	}
	if (m_dwStarsNumber > 0)
	{
		m_pStars = new D3DTLVERTEX [m_dwStarsNumber];
		if (NULL == m_pStars)
			return D3DTESTINIT_ABORT;

		srand(7);
		for (int i = 0; i < (int)m_dwStarsNumber; i++)
		{
			m_pStars[i].sx = (float)m_vpTest.X + float(rand()) * (float)m_vpTest.Width / RAND_MAX;
			m_pStars[i].sy = (float)m_vpTest.Y + float(rand()) * (float)m_vpTest.Height / RAND_MAX;
			m_pStars[i].sz = 0.99f;
			m_pStars[i].rhw = 1.f;
			m_pStars[i].color = RGB_MAKE(250, 250, 150);
			m_pStars[i].specular = 0;
			m_pStars[i].tu = 0.f;
			m_pStars[i].tv = 0.f;
		}
	}

	// Put scenario number to the status window
	char buf[2];
	sprintf(buf, "%d", m_dwScenario+1);
	WriteStatus("Scenario", buf);
	SetTestRange(1, m_dwMaxLights + 10);

	return D3DTESTINIT_RUN;
}


bool MultiLight::ExecuteTest(UINT iTestNumber)
{
	int		i;
	
	m_dwCurrentTestNumber = (DWORD)iTestNumber;

	// Clear old lights
	if (m_dwScenario == 0)
	{
		for (i = 0; i < (int)m_dwMaxLights; i++)
			LightEnable(i, FALSE);
	}
	else if (m_dwScenario == 1)
	{
		for (i = 0; i < (int)m_dwLightsNumber; i++)
			LightEnable(i, FALSE);
	}
	else if (m_dwScenario == 2)
	{
		for (i = 0; i < (int)m_dwLightsNumber; i++)
		{
			int index = m_dwMaxLights - i - 1;
			LightEnable(index, FALSE);
		}
	}
	else if (m_dwScenario == 3 || m_dwScenario == 4)
	{
		for (i = 0; i < (int)m_dwLightsNumber; i++)
		{
			int index = (i * 3) % m_dwMaxLights;
			LightEnable(index, FALSE);
		}
	}
	else
	{
		WriteToLog("MultiLight: Unknown test scenario\n");
		return false;
	}


	m_dwLightsNumber = (m_dwCurrentTestNumber < m_dwMaxLights) ? m_dwCurrentTestNumber : m_dwMaxLights;
	for (i = 0; i < (int)m_dwLightsNumber; i++)
	{
//		CLight      LightObj(this);
        D3DLIGHT8   Light;
		D3DVECTOR	vLightPos = cD3DVECTOR(0.f, 50.f * m_fScale, -100.f * m_fScale);
		D3DVECTOR	vLightDir = cD3DVECTOR(0.f, 0.f, 1.f);
		int			index;

		ZeroMemory(&Light,sizeof(D3DLIGHT8));

		Light.Type          = D3DLIGHT_DIRECTIONAL;
		Light.Diffuse.r		= 0.2f + 0.8f / (float)m_dwMaxLights;
		Light.Diffuse.g		= Light.Diffuse.r;
		Light.Diffuse.b		= Light.Diffuse.r;
		Light.Diffuse.a		= 1.0f;
		Light.Position		= vLightPos;
		Light.Direction		= vLightDir;
		Light.Range         = D3DLIGHT_RANGE_MAX;
		Light.Attenuation0  = 0.f;
		Light.Attenuation1  = 0.f;
		Light.Attenuation2  = 0.f;
		Light.Theta			= pi/10;
		Light.Phi		    = pi/4;

//        LightObj.SetLight(&Light);

		switch (m_dwScenario)
		{
		case 2:
			index = m_dwMaxLights - i - 1;
			break;
		case 3:
		case 4:
			index = (i * 3) % m_dwMaxLights;
			break;
		case 0:
		case 1:
		default:
			index = i;
			break;
		}

		if (m_dwScenario != 4)
            m_pDevice->SetLight(index, &Light);
//			SetLight(index, &LightObj);

		LightEnable(index, TRUE);
	}

	// Earth rotation
    D3DMATRIX Matrix = MatrixMult(ScaleMatrix(m_fScale, m_fScale, m_fScale), RotateYMatrix(0.1f*m_dwCurrentTestNumber));
    if (!SetTransform(D3DTRANSFORMSTATE_WORLD,&Matrix))
        return false;


	BeginTestCase("MultiLight");
	return true;
}

void MultiLight::SceneRefresh(void)
{
	if (BeginScene())
	{
		if (m_dwStarsNumber > 0)
		{
			SetTextureStageState(0, D3DTSS_COLOROP,  (DWORD)D3DTOP_DISABLE);
			RenderPrimitive(D3DPT_POINTLIST, D3DFVF_TLVERTEX,
				m_pStars, m_dwStarsNumber, NULL, 0, (DWORD)0);
		}
		SetTextureStageState(0, D3DTSS_COLOROP,  (DWORD)D3DTOP_MODULATE);
		RenderPrimitive(D3DPT_TRIANGLELIST, D3DFVF_VERTEX,
			m_pSphere->m_pVertices, m_pSphere->m_nVertices,
			m_pSphere->m_pIndices, m_pSphere->m_nIndices,
			(DWORD)0);
		EndScene();
	}
}

bool MultiLight::ProcessFrame(void)
{
	bool    bResult;

    // This range (0.00002) basically allows 1 pixel to be different
    bResult = GetCompareResult(0.03f, 0.78f, 0);

    if (!bResult) {
		WriteToLog("%s: Test fails on frame %d\n", m_szTestName, m_dwCurrentTestNumber);
		Fail();
		m_dwNumFailed++;
    }
	else {
		Pass();
	}
	EndTestCase();

	return true;
}

void MultiLight::TestCompleted(void)
{
    WriteToLog("$g%Texture management test flush summary:\n");
    WriteToLog("$cErrors:\t\t$y%d\n",m_dwNumFailed);
}

bool MultiLight::TestTerminate(void)
{
	if (m_pStars)
	{
		delete [] m_pStars;
		m_pStars = NULL;
	}

//	RELEASE(m_pMaterial);
	ReleaseTexture(m_pTexture);
    m_pTexture = NULL;
	return true;
}

//******************************************************************************
BOOL MultiLight::AbortedExit(void) {

    return m_bExit;
}

//******************************************************************************
void MultiLight::ProcessInput() {

    CD3DTest::ProcessInput();

    if (BUTTON_PRESSED(m_jsJoys, JOYBUTTON_BACK)) {
        m_bExit = TRUE;
    }
}

//******************************************************************************
BOOL MultiLight::WndProc(LRESULT* plr, HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {

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

