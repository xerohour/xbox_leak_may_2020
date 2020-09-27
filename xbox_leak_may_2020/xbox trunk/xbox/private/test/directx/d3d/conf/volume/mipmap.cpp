#include "d3dlocus.h"
#include "cd3dtest.h"
#include "Volume.h"
#include "Matrix.h"
//#include "cimageloader.h"

//************************************************************************
// MipMap Test functions

CMipVolTest::CMipVolTest()
{
	m_szTestName = TEXT("Volume MipMap");
	m_szCommandKey = TEXT("MipMap");

	// Inidicate that we are a MipMap test
	bMipMap = true;
}

CMipVolTest::~CMipVolTest()
{
}

UINT CMipVolTest::TestInitialize(void)
{
    int i;
    bool bFormats = true;

    // Request all volume formats except bumpmap
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

    // Check if MipVolumes are available	
	if (!(dwTextureCaps & D3DPTEXTURECAPS_MIPVOLUMEMAP)) //commmented out until framework is fixed
	{
		WriteToLog("Device capability not supported: Texture MipVolumeMap.\n");
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

	// Setup the colors for the Mip array
//	D3DCOLOR		RedStripes[4];

	RedStripes[0] = 0xffffffff; // White
	RedStripes[1] = 0xffff0000; // Red
	RedStripes[2] = 0xffffffff; // White
	RedStripes[3] = 0xffff0000; // Red

	// Setup the colors for the Mip array
//	D3DCOLOR		GreenStripes[4];

	GreenStripes[0] = 0xffffffff;   // White
	GreenStripes[1] = 0xff00ff00;	// Green
	GreenStripes[2] = 0xffffffff;   // White
	GreenStripes[3] = 0xff00ff00;   // Green

	// Setup the colors for the Mip array
//	D3DCOLOR		BlueStripes[4];

	BlueStripes[0] = 0xffffffff;    // White
	BlueStripes[1] = 0xff0000ff;    // Blue
	BlueStripes[2] = 0xffffffff;    // White
	BlueStripes[3] = 0xff0000ff;    // Blue

    // Setup volume depth
//    for (i=0; i < 3; i++)
//        MipVolImage[i].SetDepth(4);

    // Create the stripe image data
//    CImageLoader    Image;

//    for (i=0; i < MipVolImage[0].GetDepth(); i++)
//    {
//        MipVolImage[0].GetData()[i] = Image.LoadStripes(256,256,4,RedStripes,false); 
//        MipVolImage[1].GetData()[i] = Image.LoadStripes(128,128,4,GreenStripes,false); 
//        MipVolImage[2].GetData()[i] = Image.LoadStripes(64,64,4,BlueStripes,false); 
//    }

	return D3DTESTINIT_RUN;
}

bool CMipVolTest::ExecuteTest(UINT uTestNum)
{
	// Initialize some variables
	int nTest = ((uTestNum-1) / m_uCommonVolumeFormats) + 1;
	float flAngle = 15.0f * (float)nTest;				// Get angle for current test
    DWORD dwWidth, dwHeight, dwDepth, dwMipLevels;
    D3DVECTOR ZVector = {0.0f,0.0f,(nTest + 2 * 1.0f)};
	CMatrix Matrix;
    UINT i;
    CVolumeTexture8* pd3dtv;
    CVolume8 *pd3dvSrc, *pd3dvDst;
    D3DCOLOR* pcStripes[] = {
        RedStripes,
        GreenStripes,
        BlueStripes
    };

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

//    dwWidth = MipVolImage[0].GetSlice(0)->GetWidth();
//    dwHeight = MipVolImage[0].GetSlice(0)->GetHeight();
//    dwDepth = MipVolImage[0].GetDepth();
    dwMipLevels = 3;

	// Create the mipmap volume texture
//	pVolTexture = CreateVolumeTexture8(dwWidth,dwHeight,dwDepth,dwMipLevels,CDDS_TEXTURE_VIDEO,MipVolImage);

    pVolTexture = (CVolumeTexture8*)CreateTexture(m_pDevice, 256, 256, 4, m_fmtvCommon[nVolumeFormat], TTYPE_VOLUME | TTYPE_MIPLEVELS(dwMipLevels));
    if (!pVolTexture) {
        return false;
    }

    for (i = 0; i < dwMipLevels; i++) {

        pd3dtv = CreateStripedVolumeTexture(m_pDevice, 256 >> i, 256 >> i, 4, 4, pcStripes[i], 0, false, false, m_fmtvCommon[nVolumeFormat]);
        if (!pd3dtv) {
            return false;
        }

        if (FAILED(pd3dtv->GetVolumeLevel(0, &pd3dvSrc))) {
            ReleaseTexture(pd3dtv);
            return false;
        }

        if (FAILED(pVolTexture->GetVolumeLevel(i, &pd3dvDst))) {
            pd3dvSrc->Release();
            ReleaseTexture(pd3dtv);
            return false;
        }

        D3DXLoadVolumeFromVolume(pd3dvDst->GetIDirect3DVolume8(), NULL, NULL, pd3dvSrc->GetIDirect3DVolume8(), NULL, NULL, D3DX_FILTER_POINT, 0);

        pd3dvDst->Release();
        pd3dvSrc->Release();
        ReleaseTexture(pd3dtv);
    }

    SyncTexture(pVolTexture);

	// Tell log about CreateVolumeTexture failure
	if (NULL == pVolTexture) 
	{
		WriteToLog("CreateVolumeTexture() failed with HResult = %X.\n",GetLastError());
        UpdateStatus(false);
		return false;
	}

    // Set the valid texture
	SetTexture(0,pVolTexture);
    SetTextureStageState(0,D3DTSS_MIPFILTER,(DWORD)TEXF_LINEAR);

    // Setup the world matrix for the translating/rotating sphere
    Matrix.Identity();									
	Matrix.Translate(ZVector);		                       
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

bool CMipVolTest::TestTerminate(void)
{
    CVolumeTest::TestTerminate();

	// Cleanup mip image data
//    for (int i=0; i<3; i++)
//    {
//        for (int j=0; j<MipVolImage[i].GetDepth(); j++)
//        {
//            RELEASE(MipVolImage[i].GetData()[j]);
//        }
//    }

	return true;
}

