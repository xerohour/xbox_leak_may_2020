//
//	xbs_app.cpp
//
///////////////////////////////////////////////////////////////////////////////
//  Copyright (C) 2001, Pipeworks Software Inc.
//				All rights reserved
#include "precomp.h"
#include "xbs_app.h"
#include "xbs_math.h"
#include "tex_gen.h"
#include "renderer.h"

#ifndef XMTA_TEST
#ifndef XSS_TEST
#ifndef _DEBUG
#define BOOTSOUND 1
#endif
#endif
#endif

#ifdef BOOTSOUND
#include "bootsound.h"
#endif // BOOTSOUND

#ifdef STARTUPANIMATION
#include "mslogo.h"
extern "C" {
#include "av.h"
#include "ke.h"
}
#endif // STARTUPANIMATION

#ifdef XSS_TEST

#include "xtestlib.h"

BOOL WINAPI Ani_Main(HINSTANCE hInstance, DWORD dwReason, LPVOID lpvReserved);
void WINAPI Ani_StartTest(HANDLE hLog);
void WINAPI Ani_EndTest(void);

#pragma data_seg(EXPORT_SECTION_NAME)
extern EXP_TABLE_ENTRY Ani_ExportTableEntry[];
__EXTRN_C EXP_TABLE_DIRECTORY Ani_ExportTableDirectory = {
    "bootanim",
    Ani_ExportTableEntry
};
#pragma data_seg()

__EXTRN_C EXP_TABLE_ENTRY Ani_ExportTableEntry[] = {
    { "StartTest", (PROC)Ani_StartTest },
    { "EndTest",   (PROC)Ani_EndTest },
    { "DllMain",   (PROC)Ani_Main },
    { NULL, NULL }
};

#endif // XSS_TEST

#ifdef _DEBUG
int gcMemAllocs = 0;
#endif // _DEBUG

unsigned long g_NVCLKSpeed = 200;

#ifdef STARTUPANIMATION
extern "C" BOOL g_bShortVersion;
#else
BOOL g_bShortVersion = FALSE;
#endif

#ifndef STARTUPANIMATION
int g_TimeRecId = 0;
#define BEGIN_TIME gApp.startTimeRec(g_TimeRecId);
#define END_TIME gApp.endTimeRec(g_TimeRecId++);
#else
#define BEGIN_TIME
#define END_TIME
#endif

extern "C" {
    LONG
    WINAPI
    HalReadSMBusValue(
        IN UCHAR SlaveAddress,
        IN UCHAR CommandCode,
        IN BOOLEAN ReadWordValue,
        OUT ULONG *DataValue
        );
 
     
    LONG
    WINAPI
    HalWriteSMBusValue(
        IN UCHAR SlaveAddress,
        IN UCHAR CommandCode,
        IN BOOLEAN WriteWordValue,
        IN ULONG DataValue
        );

     
    VOID
    WINAPI
    KeStallExecutionProcessor (
        IN ULONG MicroSeconds
        );
    
    }

DWORD WriteSMC(unsigned char addr, unsigned char value)
{
    return HalWriteSMBusValue(0x20, addr, FALSE, value);
}
 
DWORD ReadSMC(unsigned char addr)
{
    DWORD value = 0xCCCCCCCC;
    DWORD status = HalReadSMBusValue(0x21, addr, FALSE, &value);
    return value;
}

//#define NULL_THE_HARDWARE

#ifndef FINAL_BUILD
const int FRAMECOUNT_PERIOD = 0;	// in milliseconds
#endif

#ifdef STARTUPANIMATION
namespace D3DK
{

void MemFree(void* pv)
{
    ::MemFree(pv);
}

void* MemAlloc(ULONG cBytes)
{
    return ::MemAlloc(cBytes);
}

void* MemAllocNoZero(ULONG cBytes)
{
    return ::MemAllocNoZero(cBytes);
}

}
#endif // STARTUPANIMATION

#ifdef XSS_TEST

///////////////////////////////////////////////////////////////////////////////
BOOL WINAPI Ani_Main(HINSTANCE hInstance, DWORD dwReason, LPVOID lpvReserved) {

    switch (dwReason) {

        case DLL_PROCESS_ATTACH:
            break;

        case DLL_PROCESS_DETACH:
            break;
    }

    return TRUE;
}

///////////////////////////////////////////////////////////////////////////////
void WINAPI Ani_EndTest(void) {
}

#endif

XBoxStartupApp gApp;

#define NUM_APP_SPEEDS 3
///////////////////////////////////////////////////////////////////////////////
#ifdef STARTUPANIMATION
extern "C"
void AnipRunAnimation()
#else // STARTUPANIMATION
#ifdef XMTA_TEST
extern "C" HRESULT Grafx_StartupAnimation_TestMain(PGFXTEST_RESULTS pgtr)
#else
#ifdef XSS_TEST
void WINAPI Ani_StartTest(HANDLE hLog)
#else
void _cdecl main()
#endif // XSS_TEST
#endif // XMTA_TEST
#endif // STARTUPANIMATION
{
#ifdef XMTA_TEST
    GFXTEST_RESULTS gtr;
#endif

#ifdef XSS_TEST

    HANDLE hMutex;

    // If any other Direct3D test is running, block until it is finished
    hMutex = CreateMutex(NULL, FALSE, "XBox Direct3D Test");
    if (!hMutex) {
        OutputDebugString(TEXT("CreateMutex failed"));
        return;
    }

    if (WaitForSingleObject(hMutex, INFINITE) != WAIT_OBJECT_0) {
        OutputDebugString(TEXT("The test mutex was abandoned"));
        CloseHandle(hMutex);
        return;
    }

    OutputDebugString(TEXT("GRAPHICS: Entering Xbox startup animation test\r\n"));

#endif

#if BOOTSOUND
    LPDIRECTSOUND pDSound;

    //
    // reset the ACI, clamp audio
    //
    
    WriteSMC(0x0B,0x01);

    DirectSoundCreate( NULL, &pDSound, NULL );
#endif

    memset(&gApp, 0, sizeof(XBoxStartupApp));

#ifdef _DEBUG
    gcMemAllocs = 0;
#endif // _DEBUG

#ifndef STARTUPANIMATION
    g_bShortVersion = FALSE;
#endif

#ifndef STARTUPANIMATION
    g_TimeRecId = 0;
#endif

#ifndef XMTA_TEST
    gApp.Construct();
#else
    gApp.Construct(&gtr);
#endif

	if(!gApp.init(640,480))
	{
#ifndef STARTUPANIMATION
		OutputDebugString("Failed to Initalize\n");
#else // STARTUPANIMATION
        return;
#endif // STARTUPANIMATION
	}

    //
    // unclamp audio
    //

#if BOOTSOUND
    pDSound->Release();
    WriteSMC(0x0B,0x00);
#endif

	gApp.run();
	gApp.shutdown();

#ifdef _DEBUG
    // Make sure all allocations are gone!

#ifdef STARTUPANIMATION
    if (gcMemAllocsContiguous != 0)
    {
        __asm int 3;
    }
#endif // STARTUPANIMATION

    if (gcMemAllocs != 0)
    {
        __asm int 3;
    }
#endif // _DEBUG

#ifndef XMTA_TEST
#ifdef XSS_TEST
    OutputDebugString(TEXT("GRAPHICS: Leaving Xbox startup animation test\r\n"));
    ReleaseMutex(hMutex);
    CloseHandle(hMutex);
#else
#ifndef GENERATE_CHECKSUMS
    XLaunchNewImage("d:\\bootscreen.xbe", NULL);
#endif
#endif
#else // XMTA_TEST
    if (pgtr) {
        memcpy(pgtr, &gtr, sizeof(GFXTEST_RESULTS));
    }
    return gtr.hr;
#endif
}
///////////////////////////////////////////////////////////////////////////////
#ifndef XMTA_TEST
void XBoxStartupApp::Construct()
#else
void XBoxStartupApp::Construct(PGFXTEST_RESULTS pgfxResults)
#endif
{
#ifdef STARTUPANIMATION
    ULONG AvInfo;
    AvSendTVEncoderOption(NULL, AV_QUERY_AV_CAPABILITIES, 0, &AvInfo); 
    if (AvInfo & AV_FLAGS_WIDESCREEN)
    {
        bWideScreen = true;
    }
    else
    {
        bWideScreen = false;
    }
#else // STARTUPANIMATION
	bWideScreen = false;
#endif // STARTUPANIMATION

#ifdef _SHIELD
    shieldMgr.Init();
#endif
    greenFog.Init();

#if 0
    LARGE_INTEGER li;
    KeQuerySystemTime(&li);
    qrand.Init(li.LowPart);
#else
    qrand.Init();
#endif

    theRenderer.Init();
    theCamera.Init();
    vblob.Init();
    camController.Init();

#ifdef STARTUPANIMATION
    bLoop    = false;
#else

#ifndef FINAL_BUILD
	bLoop    = true;
#else 
	bLoop    = false;
#endif

#endif // STARTUPANIMATION

	bPaused  = false;
	bActive  = false;

	bCameraUnderControl = true;
	bUserControlledIntensity = false;

	appSpeedId = 0;
	
	fIIDT = 0.0f;
	fSmoothedBlobIntensity = fBlobIntensity = fBaseBlobIntensity = DEMO_START_INTENSITY;
    if (g_bShortVersion)
    {
        fTimeElapsed = 7.5f;
    }
    else
    {
	    fTimeElapsed = 0.0f;
    }


	bSceneShadowsOn = true;
	bFogOn = true;

#ifdef INCLUDE_INPUT
	dwNumInputDeviceTypes = 0;
    inputDeviceTypes = NULL;
#endif // INCLUDE_INPUT

	fCamRad = 90.f;
	fCamTheta = Pi;
	fCamPhi = 0.f;

 	if (bCameraUnderControl)
	{
		bs_swap(fCamRad, fSavedCamRad);
		bs_swap(fCamTheta, fSavedCamTheta);
		bs_swap(fCamPhi, fSavedCamPhi);
	}


	float fTest = 1.35f;
	int z = (int)(fTest * 2.f);

	ZeroMemory(&blobLight,sizeof(blobLight));
	Set( (D3DVECTOR4 *)&blobLight.Position, 0.f, 0.f,0.f );
	Set( (D3DVECTOR4 *)&blobLight.Ambient,0.0f,0.0f,0.0f,1.f );
	Set( (D3DVECTOR4 *)&blobLight.Diffuse,0.13f,0.13f,0.13f,1.f );
	Set( (D3DVECTOR4 *)&blobLight.Specular,1.f,1.f,1.f,1.f );
	blobLight.Attenuation0 = 1.f;
	blobLight.Attenuation1 = 0.001f;
	blobLight.Attenuation2 = 0.001f;

	ZeroMemory(&moodLight,sizeof(moodLight));
	Set( (D3DVECTOR4 *)&moodLight.Position, 0.f, -40.f,30.f );
	Set( (D3DVECTOR4 *)&moodLight.Ambient,0.f,0.f,0.f,1.f );
	Set( (D3DVECTOR4 *)&moodLight.Diffuse,0.25f,0.25f,0.25f,1.f );
	Set( (D3DVECTOR4 *)&moodLight.Specular,0.75f,0.75f,0.75f,1.f );
	moodLight.Attenuation0 = 1.f;
	moodLight.Attenuation1 = 0.001f;
	moodLight.Attenuation2 = 0.001f;

	pNormalCubeMapLoRes = NULL;
	pNormalCubeMapHiRes = NULL;

	pRoughBumpMap = NULL;

	pStaticReflectionCubeMap = NULL;
	pGlowMap = NULL;


#ifdef INCLUDE_PLACEMENT_DOODAD	
	bPositionLookatMode = false;
	Set(&ptLookatPosition, 0.0f, 0.0f, 0.0f);
	placementDoodad.Init();
#endif

#ifdef XMTA_TEST
    pgtr = pgfxResults;
    pgtr->hr = D3D_OK;
    pgtr->uFrame = (UINT)-1;
    for (UINT i = 0; i < 3; i++) {
        pgtr->dwCRCResult[0] = 0;
        pgtr->dwCRCExpected[0] = 0;
    }
#ifndef HADWARE_CHECKSUM
    pgtr->bHardwareChecksum = false;
#else
    pgtr->bHardwareChecksum = true;
#endif
#endif

#ifndef HADWARE_CHECKSUM
    // Initialize the CRC table
    DWORD dwCRC, i, j;
    for (i = 0; i < 256; i++) {
        dwCRC = i;
        for (j = 0; j < 8; j++) {
            dwCRC = (dwCRC & 1) ? ((dwCRC >> 1) ^ 0xEDB88320L) : (dwCRC >> 1);
        }
        m_dwCRCTable[i] = dwCRC;
    }
#endif

#ifdef GENERATE_CHECKSUMS
    DWORD dwWritten;
    hChecksumFile = CreateFile(TEXT("t:\\checksum.log"), GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    WriteFile(hChecksumFile, &dwWritten, sizeof(DWORD), &dwWritten, NULL);
#else
    pdwChecksums = (LPDWORD)XLoadSection("checksum.log");
//    dwChecksumSize = XGetSectionSize(XGetSectionHandle("checksum.log"));
    uNumRefChecksums = *(UINT*)pdwChecksums;
    pdwChecksums++;
#endif
    uNumScreenChecksums = 0;
}
///////////////////////////////////////////////////////////////////////////////
void XBoxStartupApp::Destruct()
{
#ifdef _SHIELD
    shieldMgr.UnInit();
#endif
    greenFog.UnInit();
    theRenderer.UnInit();
    theCamera.UnInit();
    vblob.UnInit();
    camController.UnInit();
#ifdef INCLUDE_PLACEMENT_DOODAD
	placementDoodad.UnInit();
#endif
}
///////////////////////////////////////////////////////////////////////////////
void XBoxStartupApp::pause()
{
	bPaused = !bPaused;
}
///////////////////////////////////////////////////////////////////////////////
void XBoxStartupApp::activate()
{
	bActive = !bActive;
}
///////////////////////////////////////////////////////////////////////////////
void XBoxStartupApp::advanceTime(float dt,float cam_dt)
{
	if (dt > 1.0f) dt = 0.001f;	// cap after a breakpoint
	fTimeElapsed += dt;


	if(!bUserControlledIntensity)
	{
		if (fTimeElapsed < BLOB_ZERO_INTENSE_END_TIME)
		{
			fBaseBlobIntensity = 0.0f;
		}
		else
		{
			float t = (fTimeElapsed - BLOB_ZERO_INTENSE_END_TIME) * OO_MAX_INTENSITY_DELTA;
			t = 0.5f * t*t + 0.5f * t;
			fBaseBlobIntensity = DEMO_START_INTENSITY + t * (1.0f - DEMO_START_INTENSITY);
		}


		float pulses = sumIntensityPulses(fTimeElapsed);

		fBlobIntensity = fBaseBlobIntensity + pulses;

		float s = 0.5f*dt;
		fSmoothedBlobIntensity = (1.0f - s) * fSmoothedBlobIntensity + s * fBlobIntensity;

		fIIDT += dt * fBlobIntensity;
	}

#ifdef INCLUDE_INPUT
	else
	{
		// check for ramp up / down of blob intensity.
		static const float f_int_vel = 2.f;

		if( defaultGamepad.bAnalogButtons[XINPUT_GAMEPAD_LEFT_TRIGGER])
		{
			float fval = ((float)defaultGamepad.bAnalogButtons[XINPUT_GAMEPAD_LEFT_TRIGGER]) / 255.0f;
			fBlobIntensity -= (f_int_vel * cam_dt * fval);
			fBlobIntensity = max(0.f,fBlobIntensity);
		}

		if( defaultGamepad.bAnalogButtons[XINPUT_GAMEPAD_RIGHT_TRIGGER])
		{
			float fval = ((float)defaultGamepad.bAnalogButtons[XINPUT_GAMEPAD_RIGHT_TRIGGER]) / 255.0f;
			fBlobIntensity += (f_int_vel * cam_dt * fval);
			fBlobIntensity = min(4.f,fBlobIntensity);
		}
	}
	static const float lin_vel = 100.0f;
	static const float rot_vel = Pi*0.5f;

	if (!bCameraUnderControl)
	{
#ifdef INCLUDE_PLACEMENT_DOODAD	
		if( bPositionLookatMode )
		{
			const float move_vel = 0.6f;
			D3DVECTOR cam_pos;
			theCamera.getCameraPos(&cam_pos);
			float doodad_rad = Distance(ptLookatPosition, cam_pos);
			float move_right = defaultGamepad.fX1 * move_vel * cam_dt * doodad_rad;
			float move_up    = defaultGamepad.fY1 * move_vel * cam_dt * doodad_rad;

			float z_delta = 0.0f;
			float z_mul = 0.6f;
			if (bPaused)
			{
				float dead_banded = defaultGamepad.fY2;
				if (dead_banded < 0.0f)	dead_banded = min(0.0f, dead_banded + 0.3f);
				else					dead_banded = max(0.0f, dead_banded - 0.3f);
				z_delta += dead_banded * z_mul * cam_dt * doodad_rad;
			}
			else
			{
				z_delta += defaultGamepad.fY2 * z_mul * cam_dt * doodad_rad;
			}

			D3DVECTOR right_dir = *((D3DVECTOR*)(&theCamera.matCTW._11));
			D3DVECTOR up_dir    = *((D3DVECTOR*)(&theCamera.matCTW._21));
			D3DVECTOR z_dir     = *((D3DVECTOR*)(&theCamera.matCTW._31));

			// Moves around in plane, rather than in screen space.
			Set(&z_dir, 0.0f, 0.0f, 1.0f);
			Cross(z_dir, right_dir, &up_dir);


			AddScaled(&ptLookatPosition, right_dir, move_right);
			AddScaled(&ptLookatPosition, up_dir, move_up);
			AddScaled(&ptLookatPosition, z_dir, z_delta);
		}
		else
#endif
		{
			fCamTheta += defaultGamepad.fX1 * rot_vel * cam_dt;
			fCamPhi += defaultGamepad.fY1 * rot_vel * cam_dt;

			if (bPaused)
			{
				float dead_banded = defaultGamepad.fY2;
				if (dead_banded < 0.0f)	dead_banded = min(0.0f, dead_banded + 0.5f);
				else					dead_banded = max(0.0f, dead_banded - 0.5f);
				fCamRad -= dead_banded * lin_vel * cam_dt;
			}
			else
			{
				fCamRad -= defaultGamepad.fY2 * lin_vel * cam_dt;
			}
		}
	}

	if( defaultGamepad.bPressedAnalogButtons[XINPUT_GAMEPAD_A] )
	{
		bFogOn = !bFogOn;	
	}

	if( defaultGamepad.bPressedAnalogButtons[XINPUT_GAMEPAD_B] )
	{
		bSceneShadowsOn = !bSceneShadowsOn;
	}

	if( defaultGamepad.bPressedAnalogButtons[XINPUT_GAMEPAD_Y] )
	{
		if(++appSpeedId >= NUM_APP_SPEEDS)
		{
			appSpeedId = 0;
			bPaused = false;
		}
		else if(appSpeedId == NUM_APP_SPEEDS - 1)
			pause();
	}
	
	// Trace out the current camera position as a frogcam array entry.
	if( defaultGamepad.bPressedAnalogButtons[XINPUT_GAMEPAD_WHITE] )
	{
		camController.buttonPressed();
	}

	if( defaultGamepad.bPressedAnalogButtons[XINPUT_GAMEPAD_BLACK] )
	{
		bCameraUnderControl = !bCameraUnderControl;
		bs_swap(fCamRad, fSavedCamRad);
		bs_swap(fCamTheta, fSavedCamTheta);
		bs_swap(fCamPhi, fSavedCamPhi);
	}

	if( defaultGamepad.bPressedAnalogButtons[XINPUT_GAMEPAD_X] )
	{
		// Toggle blob intensity mode.
		bUserControlledIntensity = !bUserControlledIntensity;
	}



#ifdef INCLUDE_PLACEMENT_DOODAD	
	if( defaultGamepad.wPressedButtons & XINPUT_GAMEPAD_LEFT_THUMB )
	{
		bPositionLookatMode = !bPositionLookatMode;
	}
#endif



	if(bPaused)
	{
		float f_time_vel = 0.02f;
		float f_jog_time = defaultGamepad.fX2 * f_time_vel;
		if (fast_fabs(defaultGamepad.fX2) > 0.9f) f_jog_time *= 3.0f;

		fTimeElapsed += f_jog_time;
		fJogDeltaTime += f_jog_time;
		if(fTimeElapsed < 0.f)
		{
			fJogDeltaTime += 0.0f - fTimeElapsed;
			fTimeElapsed = 0.0f;
		}
		if(fTimeElapsed > DEMO_TOTAL_TIME )
		{
			fJogDeltaTime += (DEMO_TOTAL_TIME - 0.0001f) - fTimeElapsed;
			fTimeElapsed = DEMO_TOTAL_TIME - 0.0001f; 
		}
	}


#endif // INCLUDE_INPUT

	if(fTimeElapsed >= DEMO_TOTAL_TIME)
	{
		if(!bLoop)
		{
			return;
		}
		else
		{
#ifdef BOOTSOUND
		    // start boot sound again with animation restart
            put_fifo(1);
#endif
			fIIDT = 0.0f;
			
			if(!bUserControlledIntensity)
				fSmoothedBlobIntensity = fBlobIntensity = fBaseBlobIntensity = DEMO_START_INTENSITY;

            if (g_bShortVersion)
            {
                fTimeElapsed = 7.5f;
            }
            else
            {
	            fTimeElapsed = 0.0f;
            }
            
			if(!bUserControlledIntensity)
			{
				initIntensityPulses();
				vblob.restart();
#ifdef _SHIELD
				shieldMgr.restart();
#endif
				greenFog.restart();
				camController.pickPath(-1);
			}
		}
	}

	if (!bCameraUnderControl)
	{
		if(fCamPhi > Pi*0.5f) fCamPhi = Pi*0.5f;
		if(fCamPhi < -Pi*0.5f) fCamPhi = -Pi*0.5f;
		fCamRad = max(fCamRad,10.f);
	}

	// I need camera stuff here so that the fog renders at the new viewpoint.
	D3DVECTOR pos,look,up;
	D3DMATRIX matRot,matX,matZ;
	SetXRotation(fCamPhi,&matX);
	SetZRotation(fCamTheta,&matZ);
	MulMats(matX,matZ, &matRot);
	
	D3DVECTOR tmp;
	Set(&tmp,0.f,fCamRad,-10.f);
	Set(&look,0.f,0.f,0.f);
	Set(&up,0.f,0.f,1.f);


	bRenderGeom = true;
	bRenderSlash = false;

	Set(&look,0.f,0.f,0.f);
	Set(&up,0.f,0.f,1.f);

	if (bCameraUnderControl)
	{
		float fCamTime = (fTimeElapsed * fTimeElapsed / 6.0f)*.8f;
		fCamTime = fCamTime + .2f*(fTimeElapsed * fast_sin(min(3.14159265354f/2.f,fTimeElapsed * .2833f)));
		camController.getPosition(fCamTime, &pos, &look, &bRenderGeom, &bRenderSlash);

		// Need to set cam rad, theta, and phi so the fog looks right.
		fCamRad = Length(pos);
		float oo_rad = 1.0f / fCamRad;
		fCamPhi = fast_asin(pos.z * oo_rad);
		oo_rad = 1.0f / fast_sqrt(Square(pos.x) + Square(pos.y));
		// Avoid the poles!
		float old_theta = fCamTheta;
		fCamTheta = AngleFromSinCos(oo_rad * pos.y, oo_rad * pos.x);

		if ((fast_fabs(old_theta) < 1000.0f) && (fast_fabs(fCamTheta - old_theta) > Pi))
		{
			// Must rewrap, so that the fog doesn't skip.
			while (old_theta - Pi > fCamTheta) fCamTheta += 2.0f * Pi;
			while (old_theta + Pi < fCamTheta) fCamTheta -= 2.0f * Pi;
		}
	}
	else
	{
		D3DMATRIX matRot,matX,matZ;
		SetXRotation(fCamPhi,&matX);
		SetZRotation(fCamTheta,&matZ);
		MulMats(matX,matZ, &matRot);
		
		D3DVECTOR tmp;
		Set(&tmp,0.f,fCamRad,-10.f);

		TransformVector(tmp,matRot,&pos);


#ifdef INCLUDE_PLACEMENT_DOODAD	
		look = ptLookatPosition;
		Add(ptLookatPosition, pos, &pos);
#endif
	}

	theCamera.lookAt(pos,look,up);
	fCamRadBlob = Distance(vblob.getCenter(), pos);

	if (bRenderGeom)
	{
		sceneGeom.advanceTime(fTimeElapsed, dt);
		vblob.advanceTime(fTimeElapsed, dt);

		if(bSceneShadowsOn) 
			sceneGeom.updateShadows();
#ifdef _SHIELD	
		if(!bUserControlledIntensity)
			shieldMgr.advanceTime(fTimeElapsed, dt);
#endif

		if(bFogOn)
			greenFog.advanceTime(fTimeElapsed, dt);
	}
	
	if (bRenderSlash)
	{
		logoGeom.advanceTime(fTimeElapsed, dt);	
	}

}
///////////////////////////////////////////////////////////////////////////////
void XBoxStartupApp::drawFrame()
{

	if( gpd3dDev->BeginScene() == D3D_OK )
	{
		gpd3dDev->Clear(0,NULL,
						D3DCLEAR_STENCIL | D3DCLEAR_ZBUFFER | D3DCLEAR_TARGET,
						0xff000000,
						1.f,
						0 );
		if (bRenderGeom)
		{
			if (getBlobIntensity() > 0.0f)
			{
				sceneGeom.render(bSceneShadowsOn,true);
			}
#ifdef _SHIELD
			shieldMgr.render(true);
#endif
			if (fTimeElapsed >= BLOB_STATIC_END_TIME) vblob.render();
#ifdef _SHIELD
			shieldMgr.render(false);
#endif


#ifdef INCLUDE_PLACEMENT_DOODAD	
			if (bPositionLookatMode)
			{
				placementDoodad.render(&ptLookatPosition, &theCamera.matWTC, &theCamera.matProj);
			}
#endif

		}

		bool b_render_fog = (bRenderGeom && bFogOn && ((getBlobIntensity() > 0.0f) || (fTimeElapsed < BLOB_STATIC_END_TIME)));
		
		if (bRenderSlash)
		{
			if (b_render_fog || bRenderGeom)
			{
				gpd3dDev->Clear(0,NULL,
								D3DCLEAR_STENCIL | D3DCLEAR_ZBUFFER,
								0xff000000,
								1.0f,
								0 );
			}
			
			const D3DMATRIX& xf_slash = camController.getSlashTransform();
			logoGeom.render(xf_slash);
		}

		if (b_render_fog)
		{
			greenFog.render(bRenderSlash);
		}

		
		gpd3dDev->EndScene();
		gpd3dDev->Present(NULL,NULL,NULL,NULL);

        verifyScreenChecksum();
	}
}

///////////////////////////////////////////////////////////////////////////////
bool XBoxStartupApp::init(int width,int height)
{
	fIIDT = 0.0f;
	fSmoothedBlobIntensity = fBlobIntensity = fBaseBlobIntensity = DEMO_START_INTENSITY;
	initIntensityPulses();

	nWidth  = width;
	nHeight = height; 
		
	BEGIN_TIME;
	theRenderer.init(nWidth,nHeight);
	END_TIME;

	BEGIN_TIME;
	pNormalCubeMapHiRes = CreateNormalizationCubeMap(128);
	END_TIME;
	
	BEGIN_TIME;
	pNormalCubeMapLoRes = CreateNormalizationCubeMap(64);
	END_TIME;
	
	BEGIN_TIME;
	pRoughBumpMap = CreateIntensityTexture(128,true,1.f/512.f,512);
	END_TIME;

	BEGIN_TIME;
	pGlowMap = CreateGlowTexture(256,256,0xffffffff,0,12345);
	END_TIME;

	DWORD dwBumpDecl[] =
    {
        D3DVSD_STREAM( 0 ),
		D3DVSD_REG( 0, D3DVSDT_FLOAT3 ),

		D3DVSD_STREAM( 1 ),
		D3DVSD_REG( 1, D3DVSDT_FLOAT2 ),
		D3DVSD_REG( 2, D3DVSDT_FLOAT3 ),     
		D3DVSD_REG( 3, D3DVSDT_FLOAT3 ),     
		D3DVSD_REG( 4, D3DVSDT_FLOAT3 ), 
		D3DVSD_END()
    };

	DWORD dwPhongDecl[] =
    {
        D3DVSD_STREAM( 0 ),
		D3DVSD_REG( 0, D3DVSDT_FLOAT3 ),

		D3DVSD_STREAM( 1 ),
		D3DVSD_REG( 1, D3DVSDT_FLOAT3 ),     
		D3DVSD_REG( 2, D3DVSDT_FLOAT3 ),     
		D3DVSD_REG( 3, D3DVSDT_FLOAT3 ), 
		D3DVSD_END()
    };

	DWORD dwDepthDecl[] = 
	{
		D3DVSD_STREAM( 0 ),
		
		D3DVSD_REG( 0, D3DVSDT_FLOAT3 ),
		
		D3DVSD_END()
	};

	BEGIN_TIME;
#ifndef BINARY_RESOURCE
	dwVShader[st_Phong] = gApp.loadVertexShader("d:\\shaders\\scene_phong.xvu",dwPhongDecl);
	dwPShader[st_Phong] = gApp.loadPixelShader ("d:\\shaders\\scene_phong.xpu");

	dwVShader[st_Bump] = gApp.loadVertexShader("d:\\shaders\\scene_bump.xvu",dwBumpDecl);
	dwPShader[st_Bump] = gApp.loadPixelShader ("d:\\shaders\\scene_bump.xpu");

	dwVShader[st_Depth] = gApp.loadVertexShader("d:\\shaders\\scene_zr.xvu",dwDepthDecl);
	dwPShader[st_Depth] = gApp.loadPixelShader ("d:\\shaders\\scene_zr.xpu");
#else // BINARY_RESOURCE
	dwVShader[st_Phong] = gApp.loadVertexShader(g_scene_phong_xvu,dwPhongDecl);
	dwPShader[st_Phong] = gApp.loadPixelShader (g_scene_phong_xpu);

	dwVShader[st_Bump] = gApp.loadVertexShader(g_scene_bump_xvu,dwBumpDecl);
	dwPShader[st_Bump] = gApp.loadPixelShader (g_scene_bump_xpu);

	dwVShader[st_Depth] = gApp.loadVertexShader(g_scene_zr_xvu,dwDepthDecl);
	dwPShader[st_Depth] = gApp.loadPixelShader (g_scene_zr_xpu);
#endif // BINARY_RESOURCE
	END_TIME;

    // Set the camera up, the fog will want the correct start position.
	D3DVECTOR pos,look,up;
	D3DMATRIX matRot,matX,matZ;
	SetXRotation(fCamPhi,&matX);
	SetZRotation(fCamTheta,&matZ);
	MulMats(matX,matZ, &matRot);
	
	D3DVECTOR tmp;
	Set(&tmp,0.f,fCamRad,-10.f);

	TransformVector(tmp,matRot,&pos);

	Set(&look,0.f,0.f,0.f);
	Set(&up,0.f,0.f,1.f);

	theCamera.lookAt(pos,look,up);

	float f_aspect = (bWideScreen) ? 9.f/16.f : 3.f/4.f;
	theCamera.setProjection(Pi/4.f, f_aspect, 0.4f, 800.0f);

	BEGIN_TIME;
	sceneGeom.create();
	END_TIME;

	BEGIN_TIME;
	logoGeom.create();
	END_TIME;

	BEGIN_TIME;
	vblob.create();
	END_TIME;

	BEGIN_TIME;
#ifdef _SHIELD
	shieldMgr.create();
#endif
	END_TIME;

#ifdef INCLUDE_PLACEMENT_DOODAD
	BEGIN_TIME;
	placementDoodad.create();
	END_TIME;
#endif

	BEGIN_TIME;
	greenFog.create();
	END_TIME;

	BEGIN_TIME;
	pStaticReflectionCubeMap = CreateStaticReflectionCubeMap(256);
	END_TIME;

#ifdef INCLUDE_INPUT
#ifndef XSS_TEST
    XInitDevices( dwNumInputDeviceTypes, inputDeviceTypes );
#endif
    XBInput_CreateGamepads( &pGamepad ); 
#endif // INCLUDE_INPUT

#ifndef STARTUPANIMATION
	dwLastFramecountTick = dwLastTick = GetTickCount();
#else // STARTUPANIMATION
	dwLastFramecountTick = dwLastTick = NtGetTickCount();
#endif // STARTUPANIMATION

#ifndef FINAL_BUILD
	numFrames = 0;
	numFramesTooSlow = 0;
	fSlowestFrame = 0.0f;
	fFastestFrame = 100.0f;
#endif

	
#ifndef STARTUPANIMATION
	float f_total_time = 0.f;
	for(int i = 0; i < g_TimeRecId; i++)
		f_total_time += fInitTimes[i];

#if DBG
	char buf[256];
	sprintf(buf,"Init time: %ff seconds\n",f_total_time);
	OutputDebugString(buf);
#endif

#endif

	return true;
}
///////////////////////////////////////////////////////////////////////////////
void XBoxStartupApp::shutdown()
{
	sceneGeom.destroy();
	logoGeom.destroy();
	vblob.destroy();
#ifdef _SHIELD
	shieldMgr.destroy();
#endif
#ifdef INCLUDE_PLACEMENT_DOODAD
	placementDoodad.destroy();
#endif
	greenFog.destroy();

	pNormalCubeMapLoRes->Release();
	pNormalCubeMapHiRes->Release();

	pStaticReflectionCubeMap->Release();

	pRoughBumpMap->Release();

	pGlowMap->Release();

	gpd3dDev->DeleteVertexShader(dwVShader[st_Phong]);
	gpd3dDev->DeleteVertexShader(dwVShader[st_Bump]);
	gpd3dDev->DeleteVertexShader(dwVShader[st_Depth]);
	gpd3dDev->DeletePixelShader(dwPShader[st_Phong]);
	gpd3dDev->DeletePixelShader(dwPShader[st_Bump]);
	gpd3dDev->DeletePixelShader(dwPShader[st_Depth]);

#ifdef STARTUPANIMATION
    WaitOnMicrosoftLogo();
#endif // STARTUPANIMATION

	theRenderer.shutdown();
	bActive = false;

#ifdef GENERATE_CHECKSUMS
    DWORD dwWritten;
    SetFilePointer(hChecksumFile, 0, NULL, FILE_BEGIN);
    WriteFile(hChecksumFile, &uNumScreenChecksums, sizeof(DWORD), &dwWritten, NULL);
    CloseHandle(hChecksumFile);
#else
    XFreeSection("checksum.log");
#endif
}

#ifdef HARDWARE_CHECKSUM

DWORD g_DoChecksum; 
DWORD g_Channel; 
DWORD g_Value[3]; 

//------------------------------------------------------------------------------ 
// Callback used to calculate the checksum for the current display.  This must
// be passed to D3DDevice_SetVerticalBlankCallback once before
// D3DTest_ScreenChecksum is called.

extern "C"
void __cdecl Ani_ChecksumCallback(
    DWORD dwContext)
{ 
    if (g_DoChecksum) 
    { 
        if (g_Channel > 0) 
        { 
            g_Value[g_Channel - 1] = *(volatile DWORD *)0xFD680840 & 0x00FFFFFF; 
            if (g_Value[g_Channel - 1] == 0xFFFFFF) {
                g_Channel--;
            }
        } 

        if (g_Channel < 3) 
        { 
            *(volatile DWORD *)0xFD680844 = 0x411 | (g_Channel << 8); 
            *(volatile DWORD *)0xFD680844 = 0x410 | (g_Channel << 8); 

            g_Channel++; 
        } 
        else 
        { 
            g_Channel = 0; 
            g_DoChecksum = 0; 
        } 
    } 
} 

//------------------------------------------------------------------------------ 
// Calculates the frame's checksum. 

extern "C"
void Ani_GetScreenChecksum(DWORD *pdwRed, DWORD* pdwGreen, DWORD* pdwBlue)
{ 
    g_DoChecksum = 1; 

    while (g_DoChecksum) 
    { 
        D3DDevice_BlockUntilVerticalBlank(); 
    } 

    *pdwRed   = g_Value[0]; 
    *pdwGreen = g_Value[1]; 
    *pdwBlue  = g_Value[2]; 
}

#endif

#ifdef HARDWARE_CHECKSUM

///////////////////////////////////////////////////////////////////////////////
void XBoxStartupApp::verifyScreenChecksum()
{
    DWORD dwColor[3];
    D3DDevice_BlockUntilVerticalBlank();
    Ani_GetScreenChecksum(&dwColor[0], &dwColor[1], &dwColor[2]);

#ifdef GENERATE_CHECKSUMS
    DWORD dwWritten;
    WriteFile(hChecksumFile, dwColor, 3 * sizeof(DWORD), &dwWritten, NULL);
#else
    if (uNumScreenChecksums < uNumRefChecksums) {
        if ((dwColor[0] & 0xFFFFFF) != (pdwChecksums[0] & 0xFFFFFF) ||
            (dwColor[1] & 0xFFFFFF) != (pdwChecksums[1] & 0xFFFFFF) ||
            (dwColor[2] & 0xFFFFFF) != (pdwChecksums[2] & 0xFFFFFF))
        {
            char szOut[512];
            UINT i;
            wsprintf(szOut, "ERROR: Screen checksum mismatch on frame %d - "
                     "Actual: [0x%06X, 0x%06X, 0x%06X]; "
                     "Expected: [0x%06X, 0x%06X, 0x%06X]\r\n", 
                     uNumScreenChecksums, dwColor[0] & 0xFFFFFF, 
                     dwColor[1] & 0xFFFFFF, dwColor[2] & 0xFFFFFF, 
                     pdwChecksums[0] & 0xFFFFFF, pdwChecksums[1] & 0xFFFFFF, 
                     pdwChecksums[2] & 0xFFFFFF);
            OutputDebugString(szOut);
#ifdef XMTA_TEST
            if (pgtr->hr == D3D_OK) {
                pgtr->hr = E_FAIL;
                pgtr->uFrame = uNumScreenChecksums;
                for (i = 0; i < 3; i++) {
                    pgtr->dwCRCResult[0] = dwColor[0] & 0xFFFFFF;
                    pgtr->dwCRCExpected[0] = pdwChecksums[0] & 0xFFFFFF;
                }
                pgtr->bHardwareChecksum = true;
            }
#else
            OutputDebugString(TEXT("BOOTSCREEN: Screen checksum mismatch\r\n"));
            __asm int 3;
#endif
        }
        pdwChecksums += 3;
    }
    else {
        char szOut[512];
        wsprintf(szOut, "WARNING: Frame %d cannot be verified\r\n", uNumScreenChecksums);
        OutputDebugString(szOut);
    }
#endif
    uNumScreenChecksums++;
}

#else

///////////////////////////////////////////////////////////////////////////////
DWORD XBoxStartupApp::ComputeSurfaceCRC32(LPDIRECT3DSURFACE8 pd3ds) {

    D3DSURFACE_DESC d3dsd;
    D3DLOCKED_RECT  d3dlr;
    DWORD           dwCRC = 0;
    LPBYTE          pbData;
    UINT            i, j, k;
    HRESULT         hr;

    hr = pd3ds->GetDesc(&d3dsd);
    if (FAILED(hr)) {
        return 0;
    }

    hr = pd3ds->LockRect(&d3dlr, NULL, 0);
    if (FAILED(hr)) {
        return 0;
    }

    switch (d3dsd.Format) {

        case D3DFMT_LIN_R5G6B5: {
            DWORD  dwPixel;
            LPWORD pwPixel = (LPWORD)d3dlr.pBits;
            for (i = 0; i < d3dsd.Height; i++) {
                for (j = 0; j < d3dsd.Width; j++) {
                    dwPixel = (pwPixel[j] & 0xF800) << 8 |
                              (pwPixel[j] & 0x07E0) << 5 |
                              (pwPixel[j] & 0x001F) << 3;
                    pbData = (LPBYTE)&dwPixel;
                    for (k = 0; k < 3; k++) {
                        dwCRC = m_dwCRCTable[(dwCRC ^ pbData[k]) & 0xFF] ^ (dwCRC >> 8);
                    }
                }
                pwPixel += d3dlr.Pitch >> 1;
            }
            break;
        }

        case D3DFMT_LIN_X1R5G5B5: {
            DWORD  dwPixel;
            LPWORD pwPixel = (LPWORD)d3dlr.pBits;
            for (i = 0; i < d3dsd.Height; i++) {
                for (j = 0; j < d3dsd.Width; j++) {
                    dwPixel = (pwPixel[j] & 0x7C00) << 9 |
                              (pwPixel[j] & 0x03E0) << 6 |
                              (pwPixel[j] & 0x001F) << 3;
                    pbData = (LPBYTE)&dwPixel;
                    for (k = 0; k < 3; k++) {
                        dwCRC = m_dwCRCTable[(dwCRC ^ pbData[k]) & 0xFF] ^ (dwCRC >> 8);
                    }
                }
                pwPixel += d3dlr.Pitch >> 1;
            }
            break;
        }

        case D3DFMT_LIN_A1R5G5B5: {
            DWORD  dwPixel;
            LPWORD pwPixel = (LPWORD)d3dlr.pBits;
            for (i = 0; i < d3dsd.Height; i++) {
                for (j = 0; j < d3dsd.Width; j++) {
                    dwPixel = (pwPixel[j] & 0x8000) << 16 |
                              (pwPixel[j] & 0x7C00) << 9 |
                              (pwPixel[j] & 0x03E0) << 6 |
                              (pwPixel[j] & 0x001F) << 3;
                    pbData = (LPBYTE)&dwPixel;
                    for (k = 0; k < 4; k++) {
                        dwCRC = m_dwCRCTable[(dwCRC ^ pbData[k]) & 0xFF] ^ (dwCRC >> 8);
                    }
                }
                pwPixel += d3dlr.Pitch >> 1;
            }
            break;
        }

        case D3DFMT_LIN_X8R8G8B8: {
            LPDWORD pdwPixel = (LPDWORD)d3dlr.pBits;
            for (i = 0; i < d3dsd.Height; i++) {
                for (j = 0; j < d3dsd.Width; j++) {
                    pbData = (LPBYTE)&pdwPixel[j];
                    for (k = 0; k < 3; k++) {
                        dwCRC = m_dwCRCTable[(dwCRC ^ pbData[k]) & 0xFF] ^ (dwCRC >> 8);
                    }
                }
                pdwPixel += d3dlr.Pitch >> 2;
            }
            break;
        }

        case D3DFMT_LIN_D24S8:
        case D3DFMT_LIN_A8R8G8B8: {
            LPDWORD pdwPixel = (LPDWORD)d3dlr.pBits;
            for (i = 0; i < d3dsd.Height; i++) {
                for (j = 0; j < d3dsd.Width; j++) {
                    pbData = (LPBYTE)&pdwPixel[j];
                    for (k = 0; k < 4; k++) {
                        dwCRC = m_dwCRCTable[(dwCRC ^ pbData[k]) & 0xFF] ^ (dwCRC >> 8);
                    }
                }
                pdwPixel += d3dlr.Pitch >> 2;
            }
            break;
        }
    }

    pd3ds->UnlockRect();

    return dwCRC;
}

///////////////////////////////////////////////////////////////////////////////
void XBoxStartupApp::verifyScreenChecksum()
{
    LPDIRECT3DSURFACE8 pd3ds;
    DWORD dwCRC;

    gpd3dDev->GetBackBuffer(-1, D3DBACKBUFFER_TYPE_MONO, &pd3ds);
    dwCRC = ComputeSurfaceCRC32(pd3ds);
    pd3ds->Release();

#ifdef GENERATE_CHECKSUMS
    DWORD dwWritten;
    WriteFile(hChecksumFile, &dwCRC, sizeof(DWORD), &dwWritten, NULL);
#else
    if (uNumScreenChecksums < uNumRefChecksums) {
        if (dwCRC != *pdwChecksums) {
            char szOut[512];
            wsprintf(szOut, "ERROR: Screen checksum mismatch on frame %d - "
                     "Actual: [0x%08X]; "
                     "Expected: [0x%08X]\r\n", 
                     uNumScreenChecksums, dwCRC, 
                     *pdwChecksums);
            OutputDebugString(szOut);
#ifdef XMTA_TEST
            if (pgtr->hr == D3D_OK) {
                pgtr->hr = E_FAIL;
                pgtr->uFrame = uNumScreenChecksums;
                pgtr->dwCRCResult[0] = dwCRC;
                pgtr->dwCRCExpected[0] = *pdwChecksums;
                pgtr->bHardwareChecksum = false;
            }
#else
            OutputDebugString(TEXT("BOOTSCREEN: Screen checksum mismatch\r\n"));
            __asm int 3;
#endif
        }
        ++pdwChecksums;
    }
    else {
        char szOut[512];
        wsprintf(szOut, "WARNING: Frame %d cannot be verified\r\n", uNumScreenChecksums);
        OutputDebugString(szOut);
    }
#endif
    uNumScreenChecksums++;
}

#endif

///////////////////////////////////////////////////////////////////////////////
#ifdef _DEBUG
#include <d3d8perf.h>
#endif
extern "C" BOOL D3D__NullHardware;
///////////////////////////////////////////////////////////////////////////////


int XBoxStartupApp::run()
{
#ifdef _DEBUG
//D3DPERF_SetShowFrameRateInterval(10000);
#endif
#ifdef NULL_THE_HARDWARE
	D3D__NullHardware = TRUE;			// Set to true to test CPU framerate, no graphics operation will stall.
#endif

#ifdef BOOTSOUND
    //
    // start the boot sound
    //
    if (!g_bShortVersion)
    {
        BootSound_Start();
    }
#endif // BOOTSOUND

	for(;fTimeElapsed < DEMO_TOTAL_TIME;)
	{
		process();
	}

#ifdef BOOTSOUND
    if (!g_bShortVersion)
    {
        BootSound_Stop();
    }
#endif // BOOTSOUND

	return 0;
}
///////////////////////////////////////////////////////////////////////////////
void XBoxStartupApp::process()
{
#ifdef INCLUDE_INPUT
	processInput();
#endif // INCLUDE_INPUT

#ifndef STARTUPANIMATION
	DWORD time = GetTickCount();
#else // STARTUPANIMATION
	DWORD time = NtGetTickCount();
#endif // STARTUPANIMATION


//	float fseconds_elapsed = ((float)(abs(dwLastTick - time))) * 0.001f;
    float fseconds_elapsed = 0.0333333f;//0.016667f;
	const float f_app_time_muls[NUM_APP_SPEEDS] = {1.f,0.5f,0.f};
	float last_jogdt = fJogDeltaTime;
	fJogDeltaTime = 0.0f;
	advanceTime(fseconds_elapsed * f_app_time_muls[appSpeedId] + last_jogdt, fseconds_elapsed);	

	drawFrame();


#ifndef FINAL_BUILD
	numFrames++;
	fSlowestFrame = max(fSlowestFrame, fseconds_elapsed);
	fFastestFrame = min(fFastestFrame, fseconds_elapsed);
	if (fseconds_elapsed > 1.0f / 30.0f) numFramesTooSlow++;

	if (FRAMECOUNT_PERIOD && (time > dwLastFramecountTick + FRAMECOUNT_PERIOD))
	{
		// Trace out the fps count.
		float fps = 1000.0f * ((float)numFrames) / ((float)(time-dwLastFramecountTick));

		char buf[512];
		sprintf(buf, "%6.2f fps, over %6.3fs [%6.3fs <-> %6.3fs], %3d of %3d frames were too slow.\n",
			fps, ((float)(time-dwLastFramecountTick))/1000.0f,
			fFastestFrame, fSlowestFrame, numFramesTooSlow, numFrames);
		OutputDebugString(buf);

		dwLastFramecountTick = time;
		numFrames = 0;
		numFramesTooSlow = 0;
		fSlowestFrame = 0.0f;
		fFastestFrame = 100.0f;
	}
#endif // FINAL_BUILD


	dwLastTick = time;
}

#ifndef BINARY_RESOURCE
///////////////////////////////////////////////////////////////////////////////
DWORD XBoxStartupApp::loadVertexShader(const TCHAR *pname,const DWORD *pdwDecl)
{
    HANDLE hFile = CreateFile( pname, GENERIC_READ, FILE_SHARE_READ, 
                               NULL, OPEN_EXISTING, 0, NULL );

    DWORD dwSize = GetFileSize( hFile, NULL );
    BYTE* pData  = (BYTE*)MemAlloc(sizeof(BYTE)*(dwSize+4));
    ZeroMemory( pData, dwSize+4 );

    DWORD dwBytesRead;
    ReadFile(hFile,pData,dwSize,&dwBytesRead,NULL);

	DWORD dwVertexShader;
    gpd3dDev->CreateVertexShader( pdwDecl, (const DWORD*)pData,&dwVertexShader, 0 );

    CloseHandle( hFile );
    MemFree(pData);
	
	return dwVertexShader;
}
///////////////////////////////////////////////////////////////////////////////
DWORD XBoxStartupApp::loadPixelShader(const TCHAR *pName)
{
	HANDLE hFile = CreateFile( pName, GENERIC_READ, FILE_SHARE_READ, 
                               NULL, OPEN_EXISTING, 0, NULL );
  
    D3DPIXELSHADERDEF_FILE psdf;
    DWORD                  dwBytesRead;
    ReadFile( hFile, &psdf, sizeof(D3DPIXELSHADERDEF_FILE), &dwBytesRead, NULL );
    CloseHandle( hFile );

	DWORD dwPixelShader;
    gpd3dDev->CreatePixelShader( &(psdf.Psd), &dwPixelShader );

	return dwPixelShader;
}
#else // BINARY_RESOURCE
///////////////////////////////////////////////////////////////////////////////
DWORD XBoxStartupApp::loadVertexShader(const BYTE *pb,const DWORD *pdwDecl)
{
	DWORD dwVertexShader;
    gpd3dDev->CreateVertexShader( pdwDecl, (const DWORD*)pb,&dwVertexShader, 0 );
	return dwVertexShader;
}
///////////////////////////////////////////////////////////////////////////////
DWORD XBoxStartupApp::loadPixelShader(const BYTE *pb)
{
    D3DPIXELSHADERDEF_FILE *ppsdf = (D3DPIXELSHADERDEF_FILE*)pb;
	DWORD dwPixelShader;
    gpd3dDev->CreatePixelShader( &ppsdf->Psd, &dwPixelShader );
	return dwPixelShader;
}
#endif // BINARY_RESOURCE

#ifdef INCLUDE_INPUT
///////////////////////////////////////////////////////////////////////////////
void XBoxStartupApp::processInput()
{
	XBInput_GetInput( pGamepad );
    ZeroMemory( &defaultGamepad, sizeof(defaultGamepad) );
    for( DWORD i=0; i<4; i++ )
    {
		if( pGamepad[i].hDevice )
        {
			defaultGamepad.sThumbLX += pGamepad[i].sThumbLX;
            defaultGamepad.sThumbLY += pGamepad[i].sThumbLY;
            defaultGamepad.sThumbRX += pGamepad[i].sThumbRX;
            defaultGamepad.sThumbRY += pGamepad[i].sThumbRY;
            defaultGamepad.fX1      += pGamepad[i].fX1;
            defaultGamepad.fY1      += pGamepad[i].fY1;
            defaultGamepad.fX2      += pGamepad[i].fX2;
            defaultGamepad.fY2      += pGamepad[i].fY2;
            defaultGamepad.wButtons        |= pGamepad[i].wButtons;
            defaultGamepad.wPressedButtons |= pGamepad[i].wPressedButtons;
            defaultGamepad.wLastButtons    |= pGamepad[i].wLastButtons;

            for( DWORD b=0; b<8; b++ )
            {
				defaultGamepad.bAnalogButtons[b]        |= pGamepad[i].bAnalogButtons[b];
                defaultGamepad.bPressedAnalogButtons[b] |= pGamepad[i].bPressedAnalogButtons[b];
                defaultGamepad.bLastAnalogButtons[b]    |= pGamepad[i].bLastAnalogButtons[b];
            }
        }
	}
}
#endif // INCLUDE_INPUT

///////////////////////////////////////////////////////////////////////////////
void XBoxStartupApp::initIntensityPulses()
{
//MTS	char buf[512];
//MTS	sprintf(buf, "Intesity pulses:\n");
//MTS	OutputDebugString(buf);
	for (int i=0; i<NUM_PULSES; i++)
	{
		// x=center time, y=time radius of effect, z=intensity
		vPulses[i].x = ((float)(i+1))/((float)(NUM_PULSES+1)) + fRand11() * 0.03f;
		vPulses[i].x = 1.0f - (0.5f*Square(vPulses[i].x) + 0.5f*vPulses[i].x);

		float temp = Square(1.2f - vPulses[i].x) * (gApp.fRand01() + 2.0f) * 0.05f;
        vPulses[i].y = max(0.1f, temp);
		vPulses[i].z = (vPulses[i].x + 0.5f) * (gApp.fRand01() + 1.0f) * 0.2f;

		vPulses[i].x = vPulses[i].x * BLOB_PULSE_ELAPSED + BLOB_PULSE_START;
		vPulses[i].x = max(vPulses[i].x, BLOB_PULSE_START + vPulses[i].y);
//MTS		sprintf(buf, "    %f, %f, %f\n", vPulses[i].x, vPulses[i].y, vPulses[i].z);
//MTS		OutputDebugString(buf);
	}
	vPulses[NUM_PULSES-1].x = BLOB_PULSE_START + vPulses[NUM_PULSES-1].y;
	vPulses[NUM_PULSES-1].z *= 3.0f;
//MTS	vPulses[NUM_PULSES-1].x = -10.0f;
}
///////////////////////////////////////////////////////////////////////////////
float XBoxStartupApp::sumIntensityPulses(float et)
{
	float sum = 0.0f;
	for (int i=0; i<NUM_PULSES; i++)
	{
		// x=center time, y=time radius of effect, z=intensity
		float fdt = fast_fabs(et-vPulses[i].x);
		if (fdt > vPulses[i].y) continue;

		float s,c;
		SinCos(fdt * 0.5f * Pi / vPulses[i].y,&s,&c);
		sum += vPulses[i].z * c;
	}
//MTS	char buf[512];
//MTS	sprintf(buf, "Elapsed time = %f, pulse sum = %f, total sum = %f\n", et, sum, sum + fBaseBlobIntensity);
//MTS	OutputDebugString(buf);
	return sum;
}
///////////////////////////////////////////////////////////////////////////////
