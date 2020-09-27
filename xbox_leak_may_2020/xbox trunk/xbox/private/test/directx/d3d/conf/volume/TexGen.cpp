#include "d3dlocus.h"
#include "cd3dtest.h"
#include "Volume.h"
#include "Matrix.h"
//#include "cimageloader.h"

CVolTexGen::CVolTexGen()
{
	m_szTestName = TEXT("Volume Texture Generation");
	m_szCommandKey = TEXT("TexGen");

	// Inidicate that we are a TexGen test
    bTexGen = true;
}

CVolTexGen::~CVolTexGen()
{
}

bool CVolTexGen::SetDefaultMatrices(void)
{
    D3DVECTOR MinZ = {0.0f,0.0f,3.0f};
    CMatrix Matrix;

    if (!CVolumeTest::SetDefaultMatrices())
        return false;

    // Translate sphere to min depth
    Matrix.Identity();
	Matrix.Translate(MinZ);		
	if (!SetTransform(D3DTS_WORLD,&Matrix.m_Matrix))
	{
		WriteToLog("SetTransform(WorldMatrix) failed with HResult = %X.\n",GetLastError());
        return false;
	}

    return true;
}

UINT CVolTexGen::TestInitialize(void)
{
    bool bFormats = true;

    // Setup the Test range
	SetTestRange(1, (int)(360.0f/30.0f) * 3);

 	// Make sure we are on at lease DX8
	if (m_dwVersion <= 0x0700)
	{
		WriteToLog("This test requires at least Dx8.\n");
		return D3DTESTINIT_SKIPALL;
	}

    // Check if Volumes are available	
	DWORD dwTextureCaps = m_d3dcaps.TextureCaps;

	if (!(dwTextureCaps & D3DPTEXTURECAPS_VOLUMEMAP))
	{
		WriteToLog("Device capability not supported: Texture VolumeMap.\n");
		return D3DTESTINIT_SKIPALL;
	}
 
	// Check for the TexGen cap
	DWORD dwVertexProcessingCaps = m_d3dcaps.VertexProcessingCaps;

    if (!(dwVertexProcessingCaps & D3DVTXPCAPS_TEXGEN))
    {
		WriteToLog("Device capability not supported: VertexProcessing TexGen.\n");
		return D3DTESTINIT_SKIPALL;
    }

    // Check if any volume formats were available
	if (!bFormats) 
    {
		WriteToLog("No volume formats found.\n");
		return D3DTESTINIT_SKIPALL;
    }

    // Load sphere vertices
	if (!LoadSphere()) {
        return D3DTESTINIT_ABORT;
    }

	// Setup the colors for the Stripes array
//	DWORD   dwColors[4][4];

//	dwColors[0][0] = 0xff0000ff;    // Blue
//	dwColors[0][1] = 0xffff0000;	// Red
//	dwColors[0][2] = 0xff00ff00;    // Green
//	dwColors[0][3] = 0xffffff00;	// Yellow

//	dwColors[1][0] = 0xffffffff;    // White
//	dwColors[1][1] = 0xffff00ff;	// Magenta
//	dwColors[1][2] = 0xff00ff00;    // Green
//	dwColors[1][3] = 0xff00ffff;	// Cyan

//	dwColors[2][0] = 0xff0000ff;    // Blue
//	dwColors[2][1] = 0xffff0000;	// Red
//	dwColors[2][2] = 0xff00ff00;    // Green
//	dwColors[2][3] = 0xffffff00;	// Yellow

//	dwColors[3][0] = 0xff0000ff;    // Blue
//	dwColors[3][1] = 0xffff0000;	// Red
//	dwColors[3][2] = 0xff00ff00;    // Green
//	dwColors[3][3] = 0xffffff00;	// Yellow

	dwColors[0][0] = 0xffff0000;    // Blue
	dwColors[0][1] = 0xffff0000;	// Red
	dwColors[0][2] = 0xffff0000;    // Green
	dwColors[0][3] = 0xffff0000;	// Yellow

	dwColors[1][0] = 0xff0000ff;    // White
	dwColors[1][1] = 0xff0000ff;	// Magenta
	dwColors[1][2] = 0xff0000ff;    // Green
	dwColors[1][3] = 0xff0000ff;	// Cyan

	dwColors[2][0] = 0xff00ff00;    // Blue
	dwColors[2][1] = 0xff00ff00;	// Red
	dwColors[2][2] = 0xff00ff00;    // Green
	dwColors[2][3] = 0xff00ff00;	// Yellow

	dwColors[3][0] = 0xffffff00;    // Blue
	dwColors[3][1] = 0xffffff00;	// Red
	dwColors[3][2] = 0xffffff00;    // Green
	dwColors[3][3] = 0xffffff00;	// Yellow

    // Setup volume depth
//    VolImage.SetDepth(4);

    // Create the stripe image data
//    CImageLoader    Image;

//    for (int i=0; i<VolImage.GetDepth(); i++)
//    {
//        VolImage.GetData()[i] = Image.LoadGradient(64,64,dwColors[i]); 
//    }

    return D3DTESTINIT_RUN;
}

bool CVolTexGen::ExecuteTest(UINT uTestNum)
{
 	// Initialize some variables
	char szStr[100];
    DWORD dwWidth, dwHeight,dwDepth;
	CMatrix Matrix;
	msgString[0] = '\0';
	szStatus[0] = '\0';
	int nTest = (uTestNum-1) % (int)(360.0f/30.0f);
	float flAngle = 30.0f * (float)nTest;				// Get angle for current test
	while (flAngle>=360.0f) { flAngle-=360.0f; }
	szStr[0] = '\0';

	// Cleanup previous texture
	SetTexture(0, NULL);
	ReleaseTexture(pVolTexture);
    pVolTexture = NULL;

     // Tell the log that we are starting
	sprintf(szStr,"%sYaw %.2f degrees",szStr,flAngle);
 	BeginTestCase(szStr);

//    dwWidth = VolImage.GetSlice(0)->GetWidth();
//    dwHeight = VolImage.GetSlice(0)->GetHeight();
//    dwDepth = VolImage.GetDepth();

    // Create the volume texture
//	pVolTexture = CreateVolumeTexture8(dwWidth,dwHeight,dwDepth,1,CDDS_TEXTURE_VIDEO,&VolImage);

    pVolTexture = CreateGradientVolumeTexture(m_pDevice, 64, 64, 4, &dwColors[0][0], 4);

	// Tell log about CreateVolumeTexture failure
	if (NULL == pVolTexture) 
	{
		WriteToLog("CreateVolumeTexture() failed with HResult = %X.\n",GetLastError());
        UpdateStatus(false);
		return false;
	}

    // Set the valid texture
	SetTexture(0,pVolTexture);

    // Setup the TextureTransform states
    switch ((uTestNum-1) / (int)(360.0f/30.0f))
    {
        case 0: SetTextureStageState(0,D3DTSS_TEXCOORDINDEX,(DWORD)D3DTSS_TCI_CAMERASPACENORMAL);
            	sprintf(szStr,"%sCameraSpaceNormal, ",szStr);
                break;
        case 1: SetTextureStageState(0,D3DTSS_TEXCOORDINDEX,(DWORD)D3DTSS_TCI_CAMERASPACEPOSITION);
            	sprintf(szStr,"%sCameraSpacePosition, ",szStr);
                break;
        case 2: SetTextureStageState(0,D3DTSS_TEXCOORDINDEX,(DWORD)D3DTSS_TCI_CAMERASPACEREFLECTIONVECTOR);
            	sprintf(szStr,"%sCameraSpaceReflectionVector, ",szStr);
                break;
    }

    SetTextureStageState(0,D3DTSS_TEXTURETRANSFORMFLAGS,(DWORD)D3DTTFF_COUNT3);

    // Setup the texture matrix for rotating 
    Matrix.Identity();											   
	Matrix.RotateYaw(flAngle);									
	if (!SetTransform(D3DTS_TEXTURE0,&Matrix.m_Matrix)) 
	{
        // Tell the logfile about the SetTransform failure
		WriteToLog("SetTransform(Texture0) failed with HResult = %X.\n",GetLastError());
        UpdateStatus(false);
        return false;
	}

	return true;
}

bool CVolTexGen::TestTerminate()
{
    CVolumeTest::TestTerminate();

	// Cleanup image data
//    for (int i=0; i<VolImage.GetDepth(); i++)
//    {
//        RELEASE(VolImage.GetData()[i]); 
//    }

	return true;
}
