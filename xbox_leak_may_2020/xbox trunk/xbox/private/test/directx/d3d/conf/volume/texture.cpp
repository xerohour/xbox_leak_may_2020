#include "d3dlocus.h"
#include "cd3dtest.h"
#include "Volume.h"
#include "Matrix.h"
//#include "cimageloader.h"

//************************************************************************
// Texture Test functions

CVolTexTest::CVolTexTest()
{
	m_szTestName = TEXT("Volume Texture");
	m_szCommandKey = TEXT("Texture");
}

CVolTexTest::~CVolTexTest()
{
}

UINT CVolTexTest::TestInitialize(void)
{
    bool bFormats = true;

    // Request all volume formats except bumpmap
//	if (!RequestVolumeFormats(PF_FOURCC))//PF_RGB | PF_FOURCC | PF_LUMINANCE))
//	if (!RequestVolumeFormats(PF_RGB))// | PF_FOURCC | PF_LUMINANCE))
//		bFormats = false;

	// Setup the Test range 
    SetTestRange(1, MAX_TESTS * m_uCommonVolumeFormats);

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
//	D3DCOLOR		Stripes[8];

	Stripes[0] = 0xffffffff;    // White
	Stripes[1] = 0xffff0000;	// Red
	Stripes[2] = 0xff00ff00;	// Green
	Stripes[3] = 0xff0000ff;	// Blue
	Stripes[4] = 0xffffff00;	// Yellow
	Stripes[5] = 0xffff00ff;	// Magenta
	Stripes[6] = 0xff00ffff;	// Cyan
	Stripes[7] = 0xff808080;    // Grey

    // Setup volume depth
//    VolImage.SetDepth(8);

    // Create the stripe image data
//    CImageLoader    Image;

//    for (int i=0; i<VolImage.GetDepth(); i++)
//    {
//        VolImage.GetData()[i] = Image.LoadStripes(64,64,1,&Stripes[i],false); 
//    }

	return D3DTESTINIT_RUN;
}

bool CVolTexTest::ExecuteTest(UINT uTestNum)
{
	// Initialize some variables
	int nTest = ((uTestNum-1) / m_uCommonVolumeFormats) + 1;
	float flAngle = 15.0f * (float)nTest;				// Get angle for current test
    D3DVECTOR MinZ = {0.0f,0.0f,3.0f};
    DWORD dwWidth, dwHeight, dwDepth;
	CMatrix Matrix;
	msgString[0] = '\0';
	szStatus[0] = '\0';

	// Cleanup previous texture
	SetTexture(0, NULL);
	ReleaseTexture(pVolTexture);
    pVolTexture = NULL;

	// Cycle the common volume formats
	nVolumeFormat = (uTestNum-1) % m_uCommonVolumeFormats;

    sprintf(msgString,"%sTex: %s",msgString,D3DFmtToString(m_fmtvCommon[nVolumeFormat]));

    // Tell the log that we are starting
	BeginTestCase(msgString);

//    dwWidth = VolImage.GetSlice(0)->GetWidth();
//    dwHeight = VolImage.GetSlice(0)->GetHeight();
//    dwDepth = VolImage.GetDepth();

    // Create the volume texture
//	pVolTexture = CreateVolumeTexture8(dwWidth,dwHeight,dwDepth,1,CDDS_TEXTURE_VIDEO,&VolImage);

    pVolTexture = CreateStripedVolumeTexture(m_pDevice, 64, 64, 8, 1, Stripes, 1, false, false, m_fmtvCommon[nVolumeFormat]);

	// Tell log about CreateVolumeTexture failure
	if (NULL == pVolTexture) 
	{
		WriteToLog("CreateVolumeTexture() failed with HResult = %X.\n",GetLastError());
        UpdateStatus(false);
		return false;
	}

    // Set the valid texture
	SetTexture(0, pVolTexture);

    // Setup the world matrix for the rotating sphere
    Matrix.Identity();									
	Matrix.Translate(MinZ);		                        
	Matrix.RotateYaw(flAngle);							
	if (!SetTransform(D3DTS_WORLD,&Matrix.m_Matrix))  
	{
        // Tell the logfile about the SetTransform failure
		WriteToLog("SetTransform(WorldMatrix) failed with HResult = %X.\n",GetLastError());
        UpdateStatus(false);
        return false;
	}

    return true;
}

bool CVolTexTest::TestTerminate(void)
{
    CVolumeTest::TestTerminate();

	// Cleanup image data
//    for (int i=0; i<VolImage.GetDepth(); i++)
//    {
//        RELEASE(VolImage.GetData()[i]); 
//    }

	return true;
}
