#include "d3dlocus.h"
#include "cd3dtest.h"
#include "Volume.h"
#include "Matrix.h"
//#include "cimageloader.h"

#define LEVELS      3   // Mipmap levels to create
#define NUM_TESTS   8

CVolFilter::CVolFilter()
{
	m_szTestName=TEXT("Volume Filter");
	m_szCommandKey=TEXT("Filter");
}

CVolFilter::~CVolFilter()
{
}

UINT CVolFilter::TestInitialize(void)
{
    int i;
    bool bFormats = true;

    // Setup the Test range
	SetTestRange(1, (int)((360.0f / 45.0f) * LEVELS * NUM_TESTS));

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

    // Setup the colors for the RedStripes array
//	D3DCOLOR		RedStripes[8];

	RedStripes[0] = 0xffffffff; // White
	RedStripes[1] = 0xffff0000;	// Red
	RedStripes[2] = 0xffffffff; // White
	RedStripes[3] = 0xffff0000;	// Red
	RedStripes[4] = 0xffffffff; // White
	RedStripes[5] = 0xffff0000;	// Red
	RedStripes[6] = 0xffffffff; // White
	RedStripes[7] = 0xffff0000;	// Red

	// Setup the colors for the GreenStripes array
//	D3DCOLOR		GreenStripes[8];

	GreenStripes[0] = 0xffffffff;   // White
	GreenStripes[1] = 0xff00ff00;   // Green
	GreenStripes[2] = 0xffffffff;   // White
	GreenStripes[3] = 0xff00ff00;   // Green
	GreenStripes[4] = 0xffffffff;   // White
	GreenStripes[5] = 0xff00ff00;   // Green
	GreenStripes[6] = 0xffffffff;   // White
	GreenStripes[7] = 0xff00ff00;   // Green

	// Setup the colors for the BlueStripes array
//	D3DCOLOR		BlueStripes[8];

	BlueStripes[0] = 0xffffffff;    // White
	BlueStripes[1] = 0xff0000ff;	// Blue
	BlueStripes[2] = 0xffffffff;    // White
	BlueStripes[3] = 0xff0000ff;	// Blue
	BlueStripes[4] = 0xffffffff;    // White
	BlueStripes[5] = 0xff0000ff;	// Blue
	BlueStripes[6] = 0xffffffff;    // White
	BlueStripes[7] = 0xff0000ff;	// Blue

	// Setup the colors for the YellowStripes array
//	D3DCOLOR		YellowStripes[8];

	YellowStripes[0] = 0xffffffff;  // White
	YellowStripes[1] = 0xffffff00;  // Yellow
	YellowStripes[2] = 0xffffffff;  // White
	YellowStripes[3] = 0xffffff00;  // Yellow
	YellowStripes[4] = 0xffffffff;  // White
	YellowStripes[5] = 0xffffff00;  // Yellow
	YellowStripes[6] = 0xffffffff;  // White
	YellowStripes[7] = 0xffffff00;  // Yellow

	// Setup the colors for the MagentaStripes array
//	D3DCOLOR		MagentaStripes[8];

	MagentaStripes[0] = 0xffffffff; // White
	MagentaStripes[1] = 0xffff00ff;	// Magenta
	MagentaStripes[2] = 0xffffffff; // White
	MagentaStripes[3] = 0xffff00ff;	// Magenta
	MagentaStripes[4] = 0xffffffff; // White
	MagentaStripes[5] = 0xffff00ff;	// Magenta
	MagentaStripes[6] = 0xffffffff; // White
	MagentaStripes[7] = 0xffff00ff;	// Magenta

    // Setup volume depths
//    MinVolImage.SetDepth(4);
//    MagVolImage.SetDepth(4);

//    for (i=0; i < LEVELS; i++)
//        MipVolImage[i].SetDepth(4);

    // Create the stripe image data
//    CImageLoader    Image;

//    for (i=0; i < MinVolImage.GetDepth(); i++)
//    {
//        MinVolImage.GetData()[i] = Image.LoadStripes(16,16,8,RedStripes,false);
//    }

//    for (i=0; i < MagVolImage.GetDepth(); i++)
//    {
//        MagVolImage.GetData()[i] = Image.LoadStripes(256,256,8,GreenStripes,false);
//    }

//    for (i=0; i < MipVolImage[0].GetDepth(); i++)
//    {
//        MipVolImage[0].GetData()[i] = Image.LoadStripes(128,128,8,BlueStripes,false); 
//        MipVolImage[1].GetData()[i] = Image.LoadStripes(64,64,8,YellowStripes,false); 
//        MipVolImage[2].GetData()[i] = Image.LoadStripes(32,32,8,MagentaStripes,false); 
//    }

	return D3DTESTINIT_RUN;
}

bool CVolFilter::ExecuteTest(UINT uTestNum)
{
	CMatrix Matrix;
    UINT i;
	int iLevel;
	float flAngle;
    DWORD dwWidth, dwHeight, dwDepth;
    D3DVECTOR ZVector = {0.0f,0.0f,3.0f};
	int nTest = (uTestNum - 1) % (LEVELS * NUM_TESTS);
	int nGroup = (uTestNum - 1) / (LEVELS * NUM_TESTS);
	szStr[0] = '\0';

	// Get the Level
	iLevel = nTest / (UINT)(360.0f / 45.0f);

	// Get the Z distance of the current level
	ZVector.z = ((float)iLevel * LEVELS) + 3.0f;

	// Get the angle of the current test
	flAngle=45.0f*(float)nTest;
	while (flAngle>=360.0f) { flAngle-=360.0f; }

    // Cleanup previous texture
	SetTexture(0, NULL);
	ReleaseTexture(pVolTexture);
    pVolTexture = NULL;

    // Check MipVolume cap for mipmap cases
    if (nGroup >= 4)
    {
    	DWORD dwTextureCaps = m_d3dcaps.TextureCaps;

        // Check if MipVolumes are available	
    	if (!(dwTextureCaps & D3DPTEXTURECAPS_MIPVOLUMEMAP)) 
	    {
		    WriteToLog("\nDevice capability not supported: Texture MipVolumeMap.");
            TestFinish(D3DTESTFINISH_SKIPREMAINDER);
	    	return false;
	    }
    }

    // Setup the filter mode
    if (!SetupFilterMode(nGroup))
    {
        SkipTests((UINT)(360.0f / 45.0f) * LEVELS);
	    return false;
    }

	// Create the cubic environment mapping texture
    if (nGroup <= 3)
    {
        // Create the volume texture, using Min image data
        if (nGroup <= 1)
        {
//            dwWidth = MinVolImage.GetSlice(0)->GetWidth();
//            dwHeight = MinVolImage.GetSlice(0)->GetHeight();
//            dwDepth = MinVolImage.GetDepth();

//            pVolTexture = CreateVolumeTexture8(dwWidth,dwHeight,dwDepth,1,CDDS_TEXTURE_VIDEO,&MinVolImage);
            pVolTexture = CreateStripedVolumeTexture(m_pDevice, 16, 16, 4, 8, RedStripes, 0, false, false);//, m_fmtvCommon[nVolumeFormat]);
        }
        else    // Create the volume texture, using Mag image data
        {
//            dwWidth = MagVolImage.GetSlice(0)->GetWidth();
//            dwHeight = MagVolImage.GetSlice(0)->GetHeight();
//            dwDepth = MagVolImage.GetDepth();

//            pVolTexture = CreateVolumeTexture8(dwWidth,dwHeight,dwDepth,1,CDDS_TEXTURE_VIDEO,&MagVolImage);
            pVolTexture = CreateStripedVolumeTexture(m_pDevice, 256, 256, 4, 8, GreenStripes, 0, false, false);//, m_fmtvCommon[nVolumeFormat]);
        }

    	sprintf(szStr,"%sTexSize: %dx%dx%d, ",szStr,256,256,4);
    }
    else
    {
        CVolumeTexture8* pd3dtv;
        CVolume8 *pd3dvSrc, *pd3dvDst;
        D3DCOLOR* pcStripes[] = {
            BlueStripes,
            YellowStripes,
            MagentaStripes
        };

        // Create the volume texture, using Mip image data
//        dwWidth = MipVolImage[0].GetSlice(0)->GetWidth();
//        dwHeight = MipVolImage[0].GetSlice(0)->GetHeight();
//        dwDepth = MipVolImage[0].GetDepth();

//        pVolTexture = CreateVolumeTexture8(dwWidth,dwHeight,dwDepth,LEVELS,CDDS_TEXTURE_VIDEO,MipVolImage);
        pVolTexture = (CVolumeTexture8*)CreateTexture(m_pDevice, 128, 128, 4, D3DFMT_A8R8G8B8, TTYPE_VOLUME | TTYPE_MIPLEVELS(LEVELS));
        if (!pVolTexture) {
            return false;
        }

        for (i = 0; i < LEVELS; i++) {
            pd3dtv = CreateStripedVolumeTexture(m_pDevice, 128 >> i, 128 >> i, 4, 8, pcStripes[i], 0, false, false, D3DFMT_A8R8G8B8);
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

        // Get the current mip levels dimensions
        dwWidth = 128 >> iLevel;//MipVolImage[iLevel].GetSlice(0)->GetWidth();
        dwHeight = 128 >> iLevel;//MipVolImage[iLevel].GetSlice(0)->GetHeight();
        dwDepth = 4;//MipVolImage[iLevel].GetDepth();

        sprintf(szStr,"%sMipTexSize: %dx%dx%d, ",szStr,dwWidth,dwHeight,dwDepth);
    }

    // Tell the log that we are starting
	sprintf(szStr,"%sLevel %d, Pitch %.2f degrees",szStr,iLevel,flAngle);
	BeginTestCase(szStr);

	// Tell log about CreateVolumeTexture failure
	if (NULL == pVolTexture) 
	{
		WriteToLog("CreateVolumeTexture() failed with HResult = %X.\n",GetLastError());
        UpdateStatus(false);
		return false;
	}

    // Set the valid texture
	SetTexture(0, pVolTexture);

    // Setup the world matrix for the translating/rotating sphere
    Matrix.Identity();									
	Matrix.Translate(ZVector);		                        
	Matrix.RotatePitch(flAngle);								
	if (!SetTransform(D3DTS_WORLD,&Matrix.m_Matrix))
	{
        // Tell the logfile about the SetTransform failure
		WriteToLog("SetTransform(WorldMatrix) failed with HResult = %X.\n",GetLastError());
        UpdateStatus(false);
        return false;
	}

	return true;
}

bool CVolFilter::TestTerminate()
{
    int i, j;

    CVolumeTest::TestTerminate();

	// Cleanup min image data
//    for (i=0; i<MinVolImage.GetDepth(); i++)
//    {
//        RELEASE(MinVolImage.GetData()[i]); 
//    }

	// Cleanup mag image data
//    for (i=0; i<MagVolImage.GetDepth(); i++)
//    {
//        RELEASE(MagVolImage.GetData()[i]); 
//    }

	// Cleanup mip image data
//    for (i=0; i<LEVELS; i++)
//    {
//        for (j=0; j<MipVolImage[i].GetDepth(); j++)
//        {
//            RELEASE(MipVolImage[i].GetData()[j]);
//        }
//    }

	return true;
}

bool CVolFilter::SetupFilterMode(int nMode)
{
    DWORD dwVolumeTextureFilterCaps;

	// Check the caps we are interested in
   	dwVolumeTextureFilterCaps = m_d3dcaps.VolumeTextureFilterCaps;

    switch (nMode)
    {
        case 0:
        case 2:
            if (!(dwVolumeTextureFilterCaps & D3DPTFILTERCAPS_MINFLINEAR))       
            {
                WriteToLog("\nDevice capability not supported: VolumeTextureFilter MinFLinear.");
                return false;
            }

            if (!(dwVolumeTextureFilterCaps & D3DPTFILTERCAPS_MAGFPOINT))       
            {
	            WriteToLog("\nDevice capability not supported: VolumeTextureFilter MagFPoint.");
                return false;
            }

            SetTextureStageState(0,D3DTSS_MINFILTER,(DWORD)TEXF_LINEAR);
            SetTextureStageState(0,D3DTSS_MAGFILTER,(DWORD)TEXF_POINT);
            SetTextureStageState(0,D3DTSS_MIPFILTER,(DWORD)TEXF_NONE);
       	    sprintf(szStr,"%sFilter: LPN, ",szStr);
            break;
        case 1:
        case 3:
            if (!(dwVolumeTextureFilterCaps & D3DPTFILTERCAPS_MINFPOINT))       
            {
	            WriteToLog("\nDevice capability not supported: VolumeTextureFilter MinFPoint.");
                return false;
            }

            if (!(dwVolumeTextureFilterCaps & D3DPTFILTERCAPS_MAGFLINEAR))       
            {
	            WriteToLog("\nDevice capability not supported: VolumeTextureFilter MagFLinear.");
                return false;
            }

            SetTextureStageState(0,D3DTSS_MINFILTER,(DWORD)TEXF_POINT);
            SetTextureStageState(0,D3DTSS_MAGFILTER,(DWORD)TEXF_LINEAR);
            SetTextureStageState(0,D3DTSS_MIPFILTER,(DWORD)TEXF_NONE);
        	sprintf(szStr,"%sFilter: PLN, ",szStr);
            break;
        case 4:
            if (!(dwVolumeTextureFilterCaps & D3DPTFILTERCAPS_MINFPOINT))       
            {
	            WriteToLog("\nDevice capability not supported: VolumeTextureFilter MinFPoint.");
                return false;
            }

            if (!(dwVolumeTextureFilterCaps & D3DPTFILTERCAPS_MAGFPOINT))       
            {
	            WriteToLog("\nDevice capability not supported: VolumeTextureFilter MagFPoint.");
                return false;
            }

            if (!(dwVolumeTextureFilterCaps & D3DPTFILTERCAPS_MIPFPOINT))       
            {
	            WriteToLog("\nDevice capability not supported: VolumeTextureFilter MipFPoint.");
                return false;
            }

            SetTextureStageState(0,D3DTSS_MINFILTER,(DWORD)TEXF_POINT);
            SetTextureStageState(0,D3DTSS_MAGFILTER,(DWORD)TEXF_POINT);
            SetTextureStageState(0,D3DTSS_MIPFILTER,(DWORD)TEXF_POINT);
        	sprintf(szStr,"%sFilter: PPP, ",szStr);
            break;
        case 5:
            if (!(dwVolumeTextureFilterCaps & D3DPTFILTERCAPS_MINFLINEAR))       
            {
	            WriteToLog("\nDevice capability not supported: VolumeTextureFilter MinFLinear.");
                return false;
            }

            if (!(dwVolumeTextureFilterCaps & D3DPTFILTERCAPS_MAGFLINEAR))       
            {
	            WriteToLog("\nDevice capability not supported: VolumeTextureFilter MagFLinear.");
                return false;
            }

            if (!(dwVolumeTextureFilterCaps & D3DPTFILTERCAPS_MIPFPOINT))       
            {
	            WriteToLog("\nDevice capability not supported: VolumeTextureFilter MipFPoint.");
                return false;
            }

            SetTextureStageState(0,D3DTSS_MINFILTER,(DWORD)TEXF_LINEAR);
            SetTextureStageState(0,D3DTSS_MAGFILTER,(DWORD)TEXF_LINEAR);
            SetTextureStageState(0,D3DTSS_MIPFILTER,(DWORD)TEXF_POINT);
        	sprintf(szStr,"%sFilter: LLP, ",szStr);
            break;
        case 6:
            if (!(dwVolumeTextureFilterCaps & D3DPTFILTERCAPS_MINFPOINT))       
            {
	            WriteToLog("\nDevice capability not supported: VolumeTextureFilter MinFPoint.");
                return false;
            }

            if (!(dwVolumeTextureFilterCaps & D3DPTFILTERCAPS_MAGFPOINT))       
            {
	            WriteToLog("\nDevice capability not supported: VolumeTextureFilter MagFPoint.");
                return false;
            }

            if (!(dwVolumeTextureFilterCaps & D3DPTFILTERCAPS_MIPFLINEAR))       
            {
	            WriteToLog("\nDevice capability not supported: VolumeTextureFilter MipFLinear.");
                return false;
            }

            SetTextureStageState(0,D3DTSS_MINFILTER,(DWORD)TEXF_POINT);
            SetTextureStageState(0,D3DTSS_MAGFILTER,(DWORD)TEXF_POINT);
            SetTextureStageState(0,D3DTSS_MIPFILTER,(DWORD)TEXF_LINEAR);
        	sprintf(szStr,"%sFilter: PPL, ",szStr);
            break;
        case 7:
            if (!(dwVolumeTextureFilterCaps & D3DPTFILTERCAPS_MINFLINEAR))       
            {
	            WriteToLog("\nDevice capability not supported: VolumeTextureFilter MinFLinear.");
                return false;
            }

            if (!(dwVolumeTextureFilterCaps & D3DPTFILTERCAPS_MAGFLINEAR))       
            {
	            WriteToLog("\nDevice capability not supported: VolumeTextureFilter MagFLinear.");
                return false;
            }

            if (!(dwVolumeTextureFilterCaps & D3DPTFILTERCAPS_MIPFLINEAR))       
            {
	            WriteToLog("\nDevice capability not supported: VolumeTextureFilter MipFLinear.");
                return false;
            }

            SetTextureStageState(0,D3DTSS_MINFILTER,(DWORD)TEXF_LINEAR);
            SetTextureStageState(0,D3DTSS_MAGFILTER,(DWORD)TEXF_LINEAR);
            SetTextureStageState(0,D3DTSS_MIPFILTER,(DWORD)TEXF_LINEAR);
        	sprintf(szStr,"%sFilter: LLL, ",szStr);
            break;
    }

    return true;
}
