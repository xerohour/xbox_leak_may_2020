////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// Copyright 2000-2001 STMicroelectronics, Inc.  All Rights Reserved.         //
// HIGHLY CONFIDENTIAL INFORMATION:  This source code contains                //
// confidential and proprietary information of STMicroelectronics, Inc.       //
// This source code is provided to Microsoft Corporation under a written      //
// confidentiality agreement between STMicroelectronics and Microsoft.  This  //
// software may not be reproduced, distributed, modified, disclosed, used,    //
// displayed, stored in a retrieval system or transmitted in whole or in part,//
// in any form or by any means, electronic, mechanical, photocopying or       //
// otherwise, except as expressly authorized by STMicroelectronics. THE ONLY  //
// PERSONS WHO MAY HAVE ACCESS TO THIS SOFTWARE ARE THOSE PERSONS             //
// AUTHORIZED BY RAVISENT, WHO HAVE EXECUTED AND DELIVERED A                  //
// WRITTEN CONFIDENTIALITY AGREEMENT TO STMicroelectronics, IN THE FORM       //
// PRESCRIBED BY STMicroelectronics.                                          //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

// RavDvd.cpp : Defines the class behaviors for the application.
//

#include <XBApp.h>
//#include <XBFile.h>
#include <XBFont.h>
#include <XBUtil.h>
#include <XBHelp.h>
#include <XMenu.h>
#include "XBSplash.h"
#include "RavDvd.h"
#include "DVDMediaPlayer.h"
#include "PadInput.h"
#include "xbDVDStrings.h"
#include "XBRavMenu.h"
#include "QError.h"
#include "XBTga.h"

#define		STRESS_TEST_RANDOM_SEEK		0


void  WINAPI DVDEventHandler(DWORD dwEvent, DWORD dwInfo);


//#define ENABLE_DOLBY_SETTINGS	1
//#define DOLBY_TEST_MODE			1

//#define ENABLE_DMM_SETTINGS		0
//#define ENABLE_DTS_SETTINGS		0

#define AV_FLAGS_WIDESCREEN	0x00010000
#define AV_FLAGS_LETTERBOX	0x00100000

#define AV_ASPECT_RATIO_MASK	(AV_FLAGS_WIDESCREEN | AV_FLAGS_LETTERBOX)


#define VOL_INCREMENT	G_C_MAX_VOLUME/25
//#define RAV_COLORKEY	D3DCOLOR_XRGB(0xFF, 0x80, 0xd0)

CRavDvdApp* g_theApp;

// vertical blank callback
void __cdecl VBlankCallback(D3DVBLANKDATA *pData);
DWORD g_dwVBCount = 0;

bool g_fSettingsMenuActive = false;
bool g_fScaledDestination = false;

DWORD SubpicMenuProc(DWORD cmd, XMenuItem *it);
DWORD AudioMenuProc(DWORD cmd, XMenuItem *it);
DWORD AngleMenuProc(DWORD cmd, XMenuItem *it);
DWORD BookmarkMenuProc(DWORD cmd, XMenuItem *it);

/////////////////////////////////////////////////////////////////////////////
// CRavDvdApp construction


//int WinMain(HINSTANCE hinst, HINSTANCE hinstPrev, LPSTR lpszCmdLine, int showCmd)
//INT WINAPI wWinMain( HINSTANCE, HINSTANCE, LPWSTR, INT )
void __cdecl main()
{
	CRavDvdApp app;
	g_theApp = &app;

//	D3D__SingleStepPusher = 1;

	app.Create();
}

int g_curTitle = 0;

CRavDvdApp::CRavDvdApp() : CXBApplication(),
	m_dScanSpeed(0.0),
	m_nScanIndex(-1),
	m_nScanDir(SCAN_OFF),
	m_fOSDDisplayTime(0.0),
	m_bShowScanSpeed(false),
	m_bShowOSDText(false),
	m_bDrawHelp(false),
	m_bDisplayTime(false),
	m_bZoomed(false),
	m_nZoomX(0),
	m_nZoomY(0),
	m_nZoomH(0),
	m_nZoomW(0),
	m_fLThumbStickX(0.0),
	m_fLThumbStickY(0.0),
	m_fRThumbStickX(0.0),
	m_fRThumbStickY(0.0),
	m_bLThumbStickOn(false),
	m_bRThumbStickOn(false),
	m_bScanCycleUp(true),
	m_pPlayer(0),
	m_DiskID(0),
	m_bEnableScanAudio(false),
	m_fPlayerIsOpen(false),
	m_nHelpScreen(HELP_OFF),
	m_dwNewParentalLevel(PARENTAL_LEVEL_NOCHANGE),
	m_fParentalRightsTooLow(false),
	m_bDeleteMenusOnStop(true)
{
	m_CenterPoint.x = 0;
	m_CenterPoint.y = 0;
	m_LastDvdLoc.pData = NULL;
	m_LastDvdLoc.dwSize = 0;

	ZeroMemory(&m_ABRepeat.FreezeDataA, sizeof(m_ABRepeat.FreezeDataA));
	ZeroMemory(&m_ABRepeat.LocA, sizeof(m_ABRepeat.LocA));
	ZeroMemory(&m_ABRepeat.LocB, sizeof(m_ABRepeat.LocB));
	m_ABRepeat.fABRepeatState = ABREPEAT_OFF;
}

bool CRavDvdApp::InitPlayer()
{
	if(!m_pPlayer)
	{
		m_pPlayer = new DVDMediaPlayer((HINSTANCE)0x01);
	}

	static bool firstRun = true;
	BYTE regionCode = 0x00;

	if(firstRun)
	{
		regionCode = SYSTEM_REGION;
		firstRun = false;
	}

	if (S_OK == m_pPlayer->Initialize((HWND)0x01, 'D', regionCode))
	{
		if (m_LastDvdLoc.dwSize)
			delete[] m_LastDvdLoc.pData;

		ZeroMemory(&m_LastDvdLoc, sizeof(m_LastDvdLoc));
		ClearBookmarks();
		ResetABRepeat();

		// set the system region to SYSTEM_REGION (must be after intialize)
//		m_pPlayer->SetSystemRegion('D', SYSTEM_REGION);

		// FFH
		// set output rectangle as determined from configuration
		m_pPlayer->SetOutputRect(0, 0, m_uMaxWidth, m_uMaxHeight);
		m_pPlayer->SetDestinationRect(0, 0, m_uMaxWidth, m_uMaxHeight);
		MENU_RECT.x2 = (MENU_RECT.x1 + m_uMaxWidth/2) - 20;
		MENU_RECT.y2 = (MENU_RECT.y1 + m_uMaxHeight/2) - 20;


		m_pPlayer->SetColorkey(RAV_COLORKEY);


		m_pPlayer->EventHandlerInstall((PEVENT_CALLBACK) &DVDEventHandler);

		DWORD videoSetting;
		ULONG ulType, res;

		// get user's default setting
		videoSetting = XGetVideoFlags();

		if (videoSetting & XC_VIDEO_FLAGS_WIDESCREEN)
			{
			DP("Widescreen mode");
			m_pPlayer->SetDisplayMode(DPM_16BY9);
			}
		else if (videoSetting & XC_VIDEO_FLAGS_LETTERBOX)
			{
			DP("Letterbox mode");
			m_pPlayer->SetDisplayMode(DPM_LETTERBOXED);
			}

		else
			{
			DP("Normal mode");
			m_pPlayer->SetDisplayMode(DPM_4BY3);
			}


/*
		// ffh - set this based on output
		if (m_uWideScreen)
			m_pPlayer->SetDisplayMode(DPM_16BY9);
		else
			m_pPlayer->SetDisplayMode(DPM_LETTERBOXED);
*/

		// select a country code for audio that the decoder will automatically select if available
		m_pPlayer->SelectDefaultAudioLanguage(SystemDefaults.AudioLanguage, SystemDefaults.AudioExt);

		// select a country code for subpicture that the decoder will automatically select if available
		m_pPlayer->SelectDefaultSubpictureLanguage(SystemDefaults.SubpictureLanguage, SystemDefaults.SubpictureExt);

		// select a country code for dvd menus that the decoder will automatically select if available
		m_pPlayer->SelectDefaultMenuLanguage(SystemDefaults.MenuLanguage);

		// select a country code for parental control that will map the ensuing parental level
		m_pPlayer->SelectParentalCountry((WORD) SystemDefaults.ParentalCountry);

		// select a parental control level
		m_pPlayer->SelectParentalLevel(SystemDefaults.ParentalLevel);

		DWORD config = XGetAudioFlags();

		m_pPlayer->SetSpeakerOutputMode( (WORD) XC_AUDIO_FLAGS_BASIC(config) ); // 0x0000 == stereo, 0x0001 == mono, 0x0002 == surround.
		m_pPlayer->SetDigitalOutputMode( (WORD) (XC_AUDIO_FLAGS_BASIC(config) >> 16) ); // 0x0000 == PCM, 0x0001 == AC3, 0x0002 == DTS

		config = XGetAVPack();

		if (config == XC_AV_PACK_RFU)
		{
			m_pPlayer->SetSpeakerOutputMode( (WORD) XC_AUDIO_FLAGS_MONO ); // mono.
			m_pPlayer->SetAudioDynRngCompMode(AC3DYNRG_TV); // "TV Style"
		}

//		m_fPlayerIsOpen = true;
		return true;
	}
	else
		{
//		m_fPlayerIsOpen = false;
		return false;
		}
}


HRESULT CRavDvdApp::Create()
	{
	UINT wModes;
	int i;
	D3DDISPLAYMODE d3dMode;

	m_uMaxWidth = 0;
	m_uMaxHeight = 0;
	m_uWideScreen = 0;
	m_uProgressive = 0;

	wModes = Direct3D_GetAdapterModeCount(D3DADAPTER_DEFAULT);
	for (i = 0; i < wModes - 1; i++)
		{
		Direct3D_EnumAdapterModes(NULL, i, &d3dMode);
		if (d3dMode.Width > m_uMaxWidth)
			{
			if (d3dMode.Width < 1280)
				{
				m_uMaxWidth = d3dMode.Width;
				m_uMaxHeight = d3dMode.Height;
				m_uWideScreen = (d3dMode.Flags & D3DPRESENTFLAG_WIDESCREEN) >> 4;
				m_uProgressive = (d3dMode.Flags & D3DPRESENTFLAG_PROGRESSIVE) >> 6;
				}
			}
		else if (d3dMode.Height > m_uMaxHeight)
			{
			if (d3dMode.Width < 1280)
				{
				m_uMaxWidth = d3dMode.Width;
				m_uMaxHeight = d3dMode.Height;
				m_uWideScreen = (d3dMode.Flags & D3DPRESENTFLAG_WIDESCREEN) >> 4;
				m_uProgressive = (d3dMode.Flags & D3DPRESENTFLAG_PROGRESSIVE) >> 6;
				}
			}
		// prefer progressive, all else being equal
		else if ((d3dMode.Flags & D3DPRESENTFLAG_PROGRESSIVE) && (d3dMode.Width >= m_uMaxWidth) && (d3dMode.Height >= m_uMaxHeight))
			{
			if (d3dMode.Width < 1280)
				{
				m_uMaxWidth = d3dMode.Width;
				m_uMaxHeight = d3dMode.Height;
				m_uWideScreen = (d3dMode.Flags & D3DPRESENTFLAG_WIDESCREEN) >> 4;
				m_uProgressive = (d3dMode.Flags & D3DPRESENTFLAG_PROGRESSIVE) >> 6;
				}
			}

		// prefer widescreen, all else being equal
		else if ((d3dMode.Flags & D3DPRESENTFLAG_WIDESCREEN) && (d3dMode.Width >= m_uMaxWidth) && (d3dMode.Height >= m_uMaxHeight))
			{
			if (d3dMode.Width < 1280)
				{
				m_uMaxWidth = d3dMode.Width;
				m_uMaxHeight = d3dMode.Height;
				m_uWideScreen = (d3dMode.Flags & D3DPRESENTFLAG_WIDESCREEN) >> 4;
				m_uProgressive = (d3dMode.Flags & D3DPRESENTFLAG_PROGRESSIVE) >> 6;
				}
			}

		}


	// always disable progressive
	m_uProgressive = 0;


	DWORD videoSetting;
	ULONG ulType, res;

	// get user's default setting
	videoSetting = XGetVideoFlags();

	if (videoSetting & XC_VIDEO_FLAGS_WIDESCREEN)
		{
		DP("Widescreen setting selected");
		}
	else if (videoSetting & XC_VIDEO_FLAGS_LETTERBOX)
		{
		DP("Letterbox setting selected");
		}
	else
		{
		DP("Standard video setting selected");
		}


	DP("Width=%d", m_uMaxWidth);
	DP("Height=%d", m_uMaxHeight);
	DP("Progressive=%d", m_uProgressive);
	DP("WideScreen=%d", m_uWideScreen);

	// get the display height and width requirements from the title
    m_d3dpp.BackBufferWidth        = m_uMaxWidth;
    m_d3dpp.BackBufferHeight       = m_uMaxHeight;
    m_d3dpp.BackBufferFormat       = D3DFMT_A8R8G8B8;
    m_d3dpp.BackBufferCount        = 1;
    m_d3dpp.EnableAutoDepthStencil = TRUE;
    m_d3dpp.AutoDepthStencilFormat = D3DFMT_D24S8;
    m_d3dpp.SwapEffect             = D3DSWAPEFFECT_DISCARD;
//    m_d3dpp.Flags		           = (m_uWideScreen << 4) | (m_uProgressive << 6);
    m_d3dpp.Flags		           = (m_uWideScreen << 4) | D3DPRESENTFLAG_INTERLACED;

	return CXBApplication::Create();
	}


/////////////////////////////////////////////////////////////////////////////
// CRavDvdApp initialization

HRESULT CRavDvdApp::Initialize()
{
	printf("InitInstance");


    if( FAILED( m_Font.Create( m_pd3dDevice, "Fonts\\Font16.xpr") ) )
        return XBAPPERR_MEDIANOTFOUND;

//    if( FAILED( m_MenuFont.Create( m_pd3dDevice, "Fonts\\strip.tga",
//                                             "Fonts\\strip.abc" ) ) )
    if( FAILED( m_MenuFont.Create( m_pd3dDevice, "Fonts\\Font12.xpr") ) )
        return XBAPPERR_MEDIANOTFOUND;

	if( FAILED( m_Help.Create( m_pd3dDevice, "Gamepad.xpr") ) )
		return XBAPPERR_MEDIANOTFOUND;


	if( FAILED( m_Splash.Create( m_pd3dDevice, m_uMaxWidth, m_uMaxHeight) ) )
		return XBAPPERR_MEDIANOTFOUND;

	RECT rect;
	rect.top = OSD_ACTION_TOP;
	rect.bottom = rect.top + 64;
	rect.left = OSD_ACTION_LEFT;
	rect.right = rect.left + 64;

    if( FAILED( m_NotPermitted.Create( m_pd3dDevice, "Textures\\notallowed3.tga", rect) ) )
        return XBAPPERR_MEDIANOTFOUND;

	m_BookmarkOSD.Create( m_pd3dDevice, "Textures\\bookmark.tga", rect);

	XBRavMenu::SetFont(&m_MenuFont);

	m_pd3dDevice->EnableOverlay(true);

	this->m_GamePads = CXBApplication::m_Gamepad;

	LONGLONG diskID = 0;

	if (InitPlayer())
	{
		if(m_pPlayer->CheckDrive('D',(char*)&diskID) == S_OK)
		{

//#if _DEBUG
//		char buf[100];
//		wsprintf(buf,"this id=%d, lastID=%d", diskID,m_DiskID);
//		OutputDebugString(buf);
//#endif _DEBUG

//			if(diskID != m_DiskID)
//				{
//				InitPlayer();
				m_DiskID = diskID;
//				}

	//		m_fPlayerIsOpen = true;
			HRESULT hr = m_pPlayer->Play();
			if (S_OK != hr)
			{
				if (hr == QI_E_INSUFFICIENT_RIGHTS)
				{
					m_fParentalRightsTooLow = true;
				}
			}

			DVDPlayerMode mode = m_pPlayer->GetPlayerMode();
			if(mode == DPM_STOPPED)
			{
				hr = m_pPlayer->PlayTitle(1);

				if (S_OK != hr)
				{
					if (hr == QI_E_INSUFFICIENT_RIGHTS)
					{
						m_fParentalRightsTooLow = true;
					}
				}
			}

		}
	}
	else
		{
		}//m_fPlayerIsOpen = false;

	// register our vblank callback
	g_pd3dDevice->SetVerticalBlankCallback(VBlankCallback);

	return this->Run();
}

CRavDvdApp::~CRavDvdApp()
{
	if (m_pPlayer)
	{
		m_pPlayer->Stop();
		delete m_pPlayer;
		m_pPlayer = NULL;
	}
	m_pPlayer = NULL;
}


void CRavDvdApp::ClearBookmarks()
{
	for(vector<BOOKMARK_DATA>::iterator it = m_Bookmarks.begin(); it != m_Bookmarks.end(); it++)
	{
		if (it->FreezeData.dwSize)
			delete[] it->FreezeData.pData;
		m_Bookmarks.erase(it);
	}
}


void CRavDvdApp::CreateMonitor()
{
}

void CRavDvdApp::ResetOSD(WCHAR* pOSDText,bool bShowOSDText, bool bShowScanSpeed, int nTimerIncrement)
{
	m_bShowOSDText = bShowOSDText;
	m_bShowScanSpeed = bShowScanSpeed;

	if (pOSDText)
		wsprintfW(m_pOSDTextBuf, pOSDText);

	if (m_bShowScanSpeed)
	{
		if(m_dScanSpeed >= 0.0)
		{
			div_t t = div(m_dScanSpeed * 100, 100);

			if (m_nScanDir == SCAN_FWD)
			{
				if (t.rem != 0)
					wsprintfW(m_pScanSpeedTextBuf, L">>\t%.2fx", m_dScanSpeed);
				else
					wsprintfW(m_pScanSpeedTextBuf, L">>\t%dx", (int)m_dScanSpeed);
			}
			else
			{
				if (t.rem != 0)
					wsprintfW(m_pScanSpeedTextBuf, L"<<\t%.2fx", m_dScanSpeed);
				else
					wsprintfW(m_pScanSpeedTextBuf, L"<<\t%dx", (int)m_dScanSpeed);
			}
		}
	}

	m_fOSDDisplayTime = XBUtil_Timer(TIMER_GETAPPTIME) + nTimerIncrement;

	// immediately draw to screen for instant feedback
	if (!m_bDrawHelp )
    {
		m_pd3dDevice->BeginScene();

		DVD_DOMAIN domain;
		if (S_OK == m_pPlayer->GetCurrentDomain(&domain))
		{
			if (domain != DVD_DOMAIN_Stop)
				m_pd3dDevice->Clear( 0L, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER,
					 RAV_COLORKEY, 1.0f, 0L );
			else
			{
				m_pd3dDevice->Clear( 0L, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER,
									 0xFF000000, 1.0f, 0L );
				m_Splash.Render();
			}
		}
		else
		{
			m_pd3dDevice->Clear( 0L, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER,
								 0xFF000000, 1.0f, 0L );
			m_Splash.Render();
		}

		DrawOsd();
		m_pd3dDevice->EndScene();
		m_pd3dDevice->Present( NULL, NULL, NULL, NULL );
	}
}

void CRavDvdApp::ResetScanSpeed(bool bClearText, bool bClearData)
{
	if (bClearData)
	{
		m_dScanSpeed = 0.0;
		m_nScanIndex = -1;
		m_nScanDir = SCAN_OFF;
		m_bScanCycleUp = true;
	}

	if (bClearText)
	{
		m_bShowScanSpeed = false;
//		m_bShowOSDText = false;
	}
}

void CRavDvdApp::ResetMenus()
{
	if(m_bDeleteMenusOnStop)
	{
		while(XBRavMenu::IsActive())
		{
			XBRavMenu* curMenu = XBRavMenu::GetActiveMenu();
			curMenu->Activate(false);
			delete curMenu;
		}
	}
}

void CRavDvdApp::ResetABRepeat()
{
	if (m_ABRepeat.fABRepeatState != ABREPEAT_OFF)
	{
		delete[] m_ABRepeat.FreezeDataA.pData;
		m_ABRepeat.FreezeDataA.dwSize = 0;
		ZeroMemory(&m_ABRepeat.LocA, sizeof(m_ABRepeat.LocA));
		ZeroMemory(&m_ABRepeat.LocB, sizeof(m_ABRepeat.LocB));
		m_ABRepeat.fABRepeatState = ABREPEAT_OFF;
	}
}

void CRavDvdApp::ResetZoomVariables()
{
	m_CenterPoint.x = 0;
	m_CenterPoint.y = 0;
	m_nZoomX = 0;
	m_nZoomY = 0;
	m_nZoomH = 0;
	m_nZoomW = 0;
	m_bZoomed = false;
}

void CRavDvdApp::ResetCenterPoint()
{
	WORD wX, wY;
	float fScaleFactor;
	m_pPlayer->Get_VideoSize(wX, wY);

	if (S_OK != m_pPlayer->Get_CurrentScaleFactor(fScaleFactor))
		fScaleFactor = 1.0f;

	if (fScaleFactor != 1.0f)
	{
		m_CenterPoint.x = m_nZoomX + (float) ((float) wX / (fScaleFactor*2.0f));
		m_CenterPoint.y = m_nZoomY + (float) ((float) wY / (fScaleFactor*2.0f));
	}
	else
	{
		m_CenterPoint.x = wX/2;
		m_CenterPoint.y = wY/2;
	}
}

void CRavDvdApp::HandleParentalLevelChange()
{
	unsigned long ulParentalLevel = 8;
	WORD	wCountryCode;

	// get current level
	if (m_pPlayer)
		m_pPlayer->GetPlayerParentalLevel(&ulParentalLevel, &wCountryCode);

	// if new level is higher we need to get a password
	if (ulParentalLevel < g_theApp->m_dwNewParentalLevel)
	{
		// !!!PUT PASSWORD OVERRIDES HERE!!!
		// User should enter a password to override the current Parental Control level

		m_pPlayer->GetPlayerParentalLevel(&ulParentalLevel, &wCountryCode);
		m_pPlayer->SelectParentalLevel(g_theApp->m_dwNewParentalLevel);

		if (m_pPlayer)
			m_pPlayer->Play();
	}
	else
	{
		m_pPlayer->SelectParentalLevel(g_theApp->m_dwNewParentalLevel);

		if (m_pPlayer)
			m_pPlayer->Play();
	}

	// turn this off so we only handle it once
	g_theApp->m_dwNewParentalLevel = PARENTAL_LEVEL_NOCHANGE;
}

int gcd(int nNum1, int nNum2)
{
	int nRemainder = nNum2 % nNum1;

	if (nRemainder != 0)
		return(gcd(nRemainder, nNum1));

	return nNum1;
}

void ReduceRatio(int nNum, int nDen, int& nReducedNum, int& nReducedDen)
{
	int nGcd = gcd(nNum, nDen);

	nReducedNum = nNum/nGcd;
	nReducedDen = nDen/nGcd;
}



void CRavDvdApp::ProcessGamePadInput(DWORD dwInput)
{
	if (dwInput != XB_NO_INPUT)
	{
//		if(!m_fPlayerIsOpen)
//			InitPlayer();
	}


	if((m_pPlayer))// && (m_fPlayerIsOpen))
	{
		switch (dwInput)
		{

			// ROOT MENU ACTION
			case XB_Y_BUTTON:
						{
							ULONG uops;
							DVDPlayerMode lMode = m_pPlayer->GetPlayerMode();
							HRESULT ret = m_pPlayer->GetCurrentUOPS(&uops);

							if((S_OK == ret) && (lMode != DPM_STOPPED))
							{
								if(S_OK == ret && !(uops & UOP_MENU_CALL_ROOT))
								{
									ResetOSD(IDS_ROOTMENU, true, false, OSD_DISPLAY_TIME);
									m_pPlayer->ShowMenu(QMENU_ROOT);
								}
								else
									ResetOSD(&IDS_NOTPERMITTED, true, false, OSD_NOTPERMITTED_DISPLAY_TIME);
							}
							else
								ResetOSD(&IDS_NOTPERMITTED, true, false, OSD_NOTPERMITTED_DISPLAY_TIME);
							break;
						}
			// PLAY/PAUSE/ENTER ACTION
			case XB_A_BUTTON:
						{
							ULONG uops;
							DVDPlayerMode lMode = m_pPlayer->GetPlayerMode();

							HRESULT ret = m_pPlayer->GetCurrentUOPS(&uops);

							if(S_OK == ret || lMode == DPM_STOPPED)
							{

								if(S_OK == ret && !(uops & UOP_BUTTON))
								{
									m_pPlayer->ActivateButton();
//										ResetOSD(IDS_ACTIVATE, true, false, 1);
								}
								else if( lMode == DPM_STOPPED )
								{

									int seconds = 0;
									bool failed = false;
									LONGLONG diskID = 0;

									while(m_pPlayer->CheckDrive('D',(char*)&diskID) != S_OK && !failed)
									{
										Sleep(1000);
										if(seconds++ > 10)
											failed = true;
									}
								#if _DEBUG
										char buf[100];
										wsprintf(buf,"this id=%d, lastID=%d", diskID,m_DiskID);
										OutputDebugString(buf);
								#endif _DEBUG
									if(!failed)
									{
										if ((diskID != m_DiskID) || (0 == m_LastDvdLoc.dwSize))
										{
											if(InitPlayer())
											{
												ResetOSD(&IDS_PLAY, true, false, OSD_DISPLAY_TIME);

												m_DiskID = diskID;
												HRESULT hr = m_pPlayer->Play();
												if (S_OK != hr)
												{
													if (hr == QI_E_INSUFFICIENT_RIGHTS)
													{
														m_fParentalRightsTooLow = true;
													}
												}

												DVDPlayerMode mode = m_pPlayer->GetPlayerMode();
												if(mode == DPM_STOPPED)
												{
													hr = m_pPlayer->PlayTitle(1);

													if (S_OK != hr)
													{
														if (hr == QI_E_INSUFFICIENT_RIGHTS)
														{
															m_fParentalRightsTooLow = true;
														}
													}
												}
											}
										}
										else
										{
											ResetOSD(&IDS_PLAY, true, false, OSD_DISPLAY_TIME);
											m_pPlayer->Defrost(m_LastDvdLoc.pData, m_LastDvdLoc.dwSize);

											if (m_LastDvdLoc.dwSize)
												delete[] m_LastDvdLoc.pData;
											ZeroMemory(&m_LastDvdLoc, sizeof(m_LastDvdLoc));
										}
									}
								}
								else
								{
									switch (lMode)
									{
										case (DPM_NONE) :
										case (DPM_STILL) :
												// we are incapable of pausing in these modes
												break;

										case (DPM_REVERSEPLAY) :
										case (DPM_TRICKPLAY) :
										case (DPM_PAUSED) :
												ResetOSD(&IDS_PLAY, true, false, OSD_DISPLAY_TIME);
												m_pPlayer->Play();
												break;

										default:
												if (m_dScanSpeed != 0.0)
												{
													ResetOSD(&IDS_PLAY, true, false, OSD_DISPLAY_TIME);
													m_pPlayer->Play();
												}
												else if (!(uops & UOP_PAUSE_ON))
												{
													ResetOSD(&IDS_PAUSE, true, false, OSD_DISPLAY_TIME);
													m_pPlayer->Pause();
												}
												break;
									}
								}
							}

							// reset any scanning that may have been going on
							ResetScanSpeed(false, true);
						}

						break;

			// SHOW HELP ACTION
			case XB_BLACK_BUTTON:
						m_nHelpScreen++;

						if (m_nHelpScreen > HELP_OFF)
							m_nHelpScreen = HELP_NORMAL;

//							m_bDrawHelp = !m_bDrawHelp;
						break;

			// TOGGLE CHAP/TIME DISPLAY
			case XB_WHITE_BUTTON:
						m_bDisplayTime = !m_bDisplayTime;
						break;

			// ZOOM ACTION
			case XB_X_BUTTON:
						{
							DVD_DOMAIN domain;
							if (S_OK == m_pPlayer->GetCurrentDomain(&domain))

							{
								if (domain != DVD_DOMAIN_Stop)
								{
									float fScaleFactor;
									WORD wX, wY;
									WORD xRange, yRange;
									if (S_OK == m_pPlayer->Get_CurrentScaleFactor(fScaleFactor))
									{
										if (fScaleFactor >= 16.0)
										{
											m_pPlayer->Trick_Zoom(0, 0, 1.0);
											ResetZoomVariables();
										}
										else
										{
											m_pPlayer->Get_VideoSize(wX, wY);

											fScaleFactor = max(1, fScaleFactor);

											xRange = (float) wX * (1.0 - (1.0/fScaleFactor));
											yRange = (float) wY * (1.0 - (1.0/fScaleFactor));

											//make sure we are still in the viewable area
											if(m_nZoomX < 0)
												m_nZoomX = 0;
											else if(m_nZoomX > xRange)
												m_nZoomX = xRange;

											if(m_nZoomY < 0)
												m_nZoomY = 0;
											else if(m_nZoomY > yRange)
												m_nZoomY = yRange;

											HRESULT hr = m_pPlayer->Trick_Zoom(m_nZoomX, m_nZoomY, 2.0);
											m_bZoomed = true;

											m_nZoomW = wX - int((float) wX * (1.0 - (1.0/(fScaleFactor*2.0))));
											m_nZoomH = wY - int((float) wY * (1.0 - (1.0/(fScaleFactor*2.0))));
										}
									}
								}
								else
									ResetOSD(&IDS_NOTPERMITTED, true, false, OSD_NOTPERMITTED_DISPLAY_TIME);
							}
							else
								ResetOSD(&IDS_NOTPERMITTED, true, false, OSD_NOTPERMITTED_DISPLAY_TIME);
						}
						break;

			// STOP
			case XB_B_BUTTON:
						{
							ULONG uops;
							DVDPlayerMode lMode = m_pPlayer->GetPlayerMode();
							HRESULT ret = m_pPlayer->GetCurrentUOPS(&uops);

							if((S_OK == ret) && (lMode != DPM_STOPPED))
							{
								if(S_OK == ret && !(uops & UOP_STOP))
								{
									ResetOSD(&IDS_STOP, true, false, OSD_DISPLAY_TIME);

									if ((m_LastDvdLoc.dwSize != 0) ||
										(S_OK != m_pPlayer->Freeze(m_LastDvdLoc.pData, m_LastDvdLoc.dwSize) ))
									{
										delete[] m_LastDvdLoc.pData;
										ZeroMemory(&m_LastDvdLoc, sizeof(m_LastDvdLoc));
									}

									m_pPlayer->Stop();
								}
								else
									ResetOSD(&IDS_NOTPERMITTED, true, false, OSD_NOTPERMITTED_DISPLAY_TIME);
							}
							else if (m_LastDvdLoc.dwSize != 0)
							{
								delete[] m_LastDvdLoc.pData;
								ZeroMemory(&m_LastDvdLoc, sizeof(m_LastDvdLoc));
							}
						}
						break;

			// PREV CHAPTER ACTION
			case XB_LEFT_TRIGGER_BUTTON:
						{
							ULONG uops;
							HRESULT hr = m_pPlayer->GetCurrentUOPS(&uops);

							if ((S_OK == hr) && (!(uops & UOP_PREV_TOP_PG_SEARCH)))
							{
								WCHAR chChapText[64];
								DVD_PLAYBACK_LOCATION2 dvdLoc;

								if (S_OK == m_pPlayer->GetCurrentLocation(&dvdLoc))
								{
									if ((dvdLoc.TitleNum > 0) && (dvdLoc.ChapterNum > 1))
									{
										int nPrevChap = max(1, dvdLoc.ChapterNum - 1);

										wsprintfW(chChapText, L"Title %d\nChapter %d", dvdLoc.TitleNum, nPrevChap);
										ResetOSD(chChapText, true, false, OSD_DISPLAY_TIME);
										hr = m_pPlayer->PreviousChapter();
									}
									else
									{
										hr = m_pPlayer->PlayTitle(dvdLoc.TitleNum-1);
									}
								}

								if (S_OK == m_pPlayer->GetCurrentLocation(&dvdLoc) && dvdLoc.TitleNum != 0 && dvdLoc.ChapterNum)
								{
									wsprintfW(chChapText, L"Title %d\nChapter %d", dvdLoc.TitleNum, dvdLoc.ChapterNum);
									ResetOSD(chChapText, true, false, OSD_DISPLAY_TIME);
								}
							}
							else
								ResetOSD(&IDS_NOTPERMITTED, true, false, OSD_NOTPERMITTED_DISPLAY_TIME);
						}
						break;

			// NEXT CHAPTER ACTION
			case XB_RIGHT_TRIGGER_BUTTON:
						{
							ULONG uops;
							HRESULT hr = m_pPlayer->GetCurrentUOPS(&uops);

							if ((S_OK == hr) && (!(uops & UOP_NEXT_PG_SEARCH)))
							{
								WCHAR chChapText[64];
								DVD_PLAYBACK_LOCATION2 dvdLoc;

//								Advance title if last chap

								if (S_OK == m_pPlayer->GetCurrentLocation(&dvdLoc))
								{
									if ((dvdLoc.TitleNum > 0) && (dvdLoc.ChapterNum > 0))
									{
										int nNextChap = dvdLoc.ChapterNum + 1;

										ULONG lChapsInTitle = 0;
										if (m_pPlayer->GetNumberOfChapters(dvdLoc.TitleNum, &lChapsInTitle) != S_OK)
											lChapsInTitle = 0;

										if (nNextChap <= lChapsInTitle)
										{
											wsprintfW(chChapText, L"Title %d\nChapter %d", dvdLoc.TitleNum, nNextChap);
											ResetOSD(chChapText, true, false, OSD_DISPLAY_TIME);
											hr = m_pPlayer->NextChapter();
										}
										else
										{
											hr = m_pPlayer->PlayTitle(dvdLoc.TitleNum+1);
										}
									}
								}

//								hr = m_pPlayer->NextChapter();

								if (S_OK == m_pPlayer->GetCurrentLocation(&dvdLoc) && dvdLoc.TitleNum != 0 && dvdLoc.ChapterNum)
								{
									wsprintfW(chChapText, L"Title %d\nChapter %d", dvdLoc.TitleNum, dvdLoc.ChapterNum);
									ResetOSD(chChapText, true, false, OSD_DISPLAY_TIME);

								}
							}
							else
								ResetOSD(&IDS_NOTPERMITTED, true, false, OSD_NOTPERMITTED_DISPLAY_TIME);
						}
						break;


			// SLOW/FAST REV ACTION
			case XB_SHIFT_X_BUTTON:
						{
							ULONG uops;
							HRESULT hr = m_pPlayer->GetCurrentUOPS(&uops);

							if ((S_OK == hr) && (!(uops & UOP_BACKWARD_SCAN)))
							{
								DVDPlayerMode lMode = m_pPlayer->GetPlayerMode();

								if(m_pPlayer->IsPlaying() && !((lMode == DPM_PAUSED) || (lMode == DPM_STILL)))
								{
									if ((m_nScanIndex != -1) && (m_nScanDir == SCAN_REV))
									{
										// increment thru fast reverse speeds
										if (m_bScanCycleUp)
											m_nScanIndex ++;
										else
											m_nScanIndex --;

										if (m_dScanSpeed > 1.0)
										{
											// we are in fast rev mode
											m_nScanIndex = max(0, min(m_nScanIndex, NUM_FAST_MOTION - 1));

											if ((m_nScanIndex == 0) || (m_nScanIndex == NUM_FAST_MOTION - 1))
												m_bScanCycleUp = !m_bScanCycleUp;

											m_dScanSpeed = FAST_SCAN_VALUES[m_nScanIndex];
										}
										else if (m_dScanSpeed < 1.0)
										{
											// we are in slow rev mode
											m_nScanIndex = max(0, min(m_nScanIndex, NUM_SLOW_MOTION - 1));

											if ((m_nScanIndex == 0) || (m_nScanIndex == NUM_SLOW_MOTION - 1))
												m_bScanCycleUp = !m_bScanCycleUp;

											m_dScanSpeed = SLOW_SCAN_VALUES[m_nScanIndex];
										}
									}
									else if (m_nScanIndex != -1)
									{
										// must've been scanning forward; just keep same scan speed in reverse
										m_nScanDir = SCAN_REV;
										m_bScanCycleUp = true;
									}
									else
									{
										m_nScanDir = SCAN_REV;
										m_nScanIndex=0;
										m_bScanCycleUp = true;
										m_dScanSpeed = FAST_SCAN_VALUES[m_nScanIndex];
									}
								}
								else
								{
									m_nScanDir = SCAN_REV;
									m_nScanIndex=0;
									m_bScanCycleUp = true;
									m_dScanSpeed = SLOW_SCAN_VALUES[m_nScanIndex];
								}

								ResetOSD(NULL, false, true, OSD_DISPLAY_TIME-1);
								m_pPlayer->PlayBackwards(m_dScanSpeed);
							}
						}
						break;

			// SLOW/FAST FWD ACTION
			case XB_SHIFT_B_BUTTON:
						// increment thru forward speeds
						{
							ULONG uops;
							HRESULT hr = m_pPlayer->GetCurrentUOPS(&uops);

							if ((S_OK == hr) && (!(uops & UOP_FORWARD_SCAN)))
							{
								DVDPlayerMode lMode = m_pPlayer->GetPlayerMode();

								if(m_pPlayer->IsPlaying() && !((lMode == DPM_PAUSED) || (lMode == DPM_STILL)))

								{
									if ((m_nScanIndex != -1) && (m_nScanDir == SCAN_FWD))
									{
										// increment thru forward speeds
										if (m_bScanCycleUp)
											m_nScanIndex ++;
										else
											m_nScanIndex --;

										if (m_dScanSpeed > 1.0)


										{
											m_nScanIndex = max(0, min(m_nScanIndex, NUM_FAST_MOTION - 1));

											if ((m_nScanIndex == 0) || (m_nScanIndex == NUM_FAST_MOTION - 1))
												m_bScanCycleUp = !m_bScanCycleUp;


											// we are in fast fwd mode
											m_dScanSpeed = FAST_SCAN_VALUES[m_nScanIndex];

										}
										else if (m_dScanSpeed < 1.0)
										{
											// we are in slow fwd mode
											m_nScanIndex = max(0, min(m_nScanIndex, NUM_SLOW_MOTION - 1));

											if ((m_nScanIndex == 0) || (m_nScanIndex == NUM_SLOW_MOTION - 1))
												m_bScanCycleUp = !m_bScanCycleUp;

											m_dScanSpeed = SLOW_SCAN_VALUES[m_nScanIndex];
										}
									}
									else if (m_nScanIndex != -1)
									{
										// must've been scanning backward; just keep same scan speed but forward
										m_nScanDir = SCAN_FWD;
										m_bScanCycleUp = true;
									}
									else
									{
										// we're playing and just starting to scan
										m_nScanDir = SCAN_FWD;
										m_nScanIndex = 0;
										m_bScanCycleUp = true;
										m_dScanSpeed = FAST_SCAN_VALUES[m_nScanIndex];
									}

								}
								else
								{
									m_nScanDir = SCAN_FWD;
									m_nScanIndex = 0;
									m_bScanCycleUp = true;
									m_dScanSpeed = SLOW_SCAN_VALUES[m_nScanIndex];
								}

								ResetOSD(NULL, false, true, OSD_DISPLAY_TIME-1);
								m_pPlayer->PlayForwards(m_dScanSpeed);
							}
						}
						break;

			case XB_SHIFT_Y_BUTTON:
						{
							// show subpicture menu
							ULONG lNumSubpic, lCurSubpic, uops;
							BOOL bEnabled;
							m_pPlayer->GetCurrentSubpicture(&lNumSubpic, &lCurSubpic, &bEnabled);

							if (S_OK == m_pPlayer->GetCurrentUOPS(&uops))
							{
								if (!(uops & UOP_SUB_PICTURE_STREAM_CHANGE))
								{
									if (lNumSubpic > 0)
									{
										XBRavMenu*	pSubpicMenu = new XBRavMenu(NULL, 320.0f, 100.0f,
											lNumSubpic, MENU_WRAP, NULL);
										if (pSubpicMenu)
										{
											for(unsigned long i = 0; i < lNumSubpic; i++)
											{
												// add text description
			    								char strLangName[MAX_PATH];
			    								WCHAR wStrLangName[MAX_PATH];
				    							if (S_OK == m_pPlayer->GetSubpictureLanguage(i, strLangName, MAX_PATH))
												{
													MultiByteToWideChar(CP_ACP,0,strLangName,-1,wStrLangName, MAX_PATH);
													pSubpicMenu->AddItem(MITEM_ROUTINE, wStrLangName, &SubpicMenuProc);
												}
											}

											pSubpicMenu->AddItem(MITEM_ROUTINE, L"None", &SubpicMenuProc);

											if (bEnabled)
												pSubpicMenu->curitem = lCurSubpic;
											else
												pSubpicMenu->curitem = lNumSubpic;

											pSubpicMenu->Activate();
										}
									}
									else
									{
										ResetOSD(&IDS_NOTPERMITTED, true, false, OSD_NOTPERMITTED_DISPLAY_TIME);
									}
								}
								else
								{
									ResetOSD(&IDS_NOTPERMITTED, true, false, OSD_NOTPERMITTED_DISPLAY_TIME);
								}
							}
						}
						break;

			case XB_SHIFT_A_BUTTON:
						{
							// show audio menu
							ULONG lNumAudio, lCurAudio, uops;
							m_pPlayer->GetCurrentAudio(&lNumAudio, &lCurAudio);

							if(S_OK == m_pPlayer->GetCurrentUOPS(&uops))
							{
								if(!(uops & UOP_AUDIO_STREAM_CHANGE))
								{
									if (lNumAudio > 1)
									{
										XBRavMenu * pAudioMenu = new XBRavMenu(NULL, 320.0f, 100.0f,
											lNumAudio, MENU_WRAP, NULL);
										if (pAudioMenu)
										{
											for(unsigned long i = 0; i < lNumAudio; i++)
											{
												// add text description
			    								char strLangName[MAX_PATH];
			    								WCHAR wStrLangName[MAX_PATH];

				    							if (S_OK == m_pPlayer->GetAudioLanguage(i, strLangName, MAX_PATH))
												{
													MultiByteToWideChar(CP_ACP,0,strLangName,-1,wStrLangName, MAX_PATH);
													pAudioMenu->AddItem(MITEM_ROUTINE, wStrLangName, &AudioMenuProc);

												}


											}
											pAudioMenu->curitem = lCurAudio;
											pAudioMenu->Activate();
										}
									}
									else
									{
										ResetOSD(&IDS_NOTPERMITTED, true, false, OSD_NOTPERMITTED_DISPLAY_TIME);
									}
								}
								else
								{
									ResetOSD(&IDS_NOTPERMITTED, true, false, OSD_NOTPERMITTED_DISPLAY_TIME);
								}
							}
						}
						break;

			case XB_SHIFT_BLACK_BUTTON:
						// show settings menu

						// temporarily enable/disable audio during scan
//						m_bEnableScanAudio = !m_bEnableScanAudio;
//						m_pPlayer->EnableScanAudio(m_bEnableScanAudio);
						break;

			case XB_SHIFT_BACK:
//						ResetOSD(IDS_TITLEMENU, true, false, OSD_DISPLAY_TIME);
						m_pPlayer->ShowMenu(QMENU_TITLE);
						break;

			case XB_SHIFT_START:
						DisplayTitleMenu();
//						DisplayGlobalSettingsMenu();
						break;

			case XB_SHIFT_WHITE_BUTTON:
						{
							// show angle menu
							ULONG lNumAngles, lCurAngle, uops;
							m_pPlayer->GetCurrentAngle(&lNumAngles, &lCurAngle);

							if(S_OK == m_pPlayer->GetCurrentUOPS(&uops))
							{
								if(!(uops & UOP_ANGLE_CHANGE))
								{
									if (lNumAngles > 1)
									{
										XBRavMenu * pAngleMenu = new XBRavMenu(NULL, 320.0f, 100.0f,
											lNumAngles, MENU_WRAP, NULL);
										if (pAngleMenu)
										{
											for(unsigned long i = 0; i < lNumAngles; i++)
											{
			    								WCHAR strName[MAX_PATH];

												wsprintfW(strName, L"Angle %d", i + 1);
												pAngleMenu->AddItem(MITEM_ROUTINE, strName, &AngleMenuProc);
											}
											pAngleMenu->curitem = lCurAngle-1; // angle is 1-based
											pAngleMenu->Activate();
										}
									}
									else
									{
										ResetOSD(&IDS_NOANGLE, true, false, OSD_DISPLAY_TIME);
									}
								}
								else
								{
									ResetOSD(&IDS_NOTPERMITTED, true, false, OSD_NOTPERMITTED_DISPLAY_TIME);
								}
							}
						}
						break;

			case XB_SHIFT_LEFT_TRIGGER_BUTTON:
						// volume down
//						{
//							long lVol;
//							m_pPlayer->get_Volume(&lVol);
//							lVol = max(lVol-VOL_INCREMENT, 0);
//							ResetOSD(&IDS_VOLDOWN, true, false, OSD_DISPLAY_TIME);
//							m_pPlayer->put_Volume(lVol);
//						}
						break;

			case XB_SHIFT_RIGHT_TRIGGER_BUTTON:
						// volume up
//						{
//							long lVol;
//							m_pPlayer->get_Volume(&lVol);
//							lVol = min(lVol+VOL_INCREMENT, G_C_MAX_VOLUME);
//							ResetOSD(&IDS_VOLUP, true, false, OSD_DISPLAY_TIME);
//							m_pPlayer->put_Volume(lVol);
//						}
						break;


			case XB_SHIFT_DPAD_LEFT:
						{
							ULONG uops;
							DVD_DOMAIN domain;

							if(S_OK == m_pPlayer->GetCurrentUOPS(&uops))
							{
								if ((S_OK == m_pPlayer->GetCurrentDomain(&domain)) && (domain != DVD_DOMAIN_Stop)
									&& (!(uops & UOP_TIME_PLAY_SEARCH)))
								{
									BOOKMARK_DATA bookmark;

									// drop bookmark
									ResetOSD(&IDS_BOOKMARK, true, false, OSD_DISPLAY_TIME);

									if (S_OK == m_pPlayer->GetCurrentLocation(&bookmark.Loc))
									{
										if (S_OK == m_pPlayer->Freeze(bookmark.FreezeData.pData,
											bookmark.FreezeData.dwSize))
										{
											m_Bookmarks.push_back(bookmark);
										}
									}
								}
								else
								{
									ResetOSD(&IDS_NOTPERMITTED, true, false, OSD_NOTPERMITTED_DISPLAY_TIME);
								}
							}
						}
						break;

			case XB_SHIFT_DPAD_RIGHT:
						{
							ULONG uops;
							DVD_DOMAIN domain;

							if (S_OK == m_pPlayer->GetCurrentUOPS(&uops))
							{
								if ((S_OK == m_pPlayer->GetCurrentDomain(&domain)) && (domain != DVD_DOMAIN_Stop) &&
									(!(uops & UOP_TIME_PLAY_SEARCH)))
								{
									// show bookmark menu if any exist
									int nNumBookmarks = m_Bookmarks.size();
									WCHAR wchBookmarkText[MAX_PATH];

									if (nNumBookmarks)
									{
										for(vector<BOOKMARK_DATA>::iterator it = m_Bookmarks.begin();
											it != m_Bookmarks.end(); it++)
										{
											XBRavMenu * pBookmarkMenu = new XBRavMenu(NULL, 320.0f, 100.0f,
												nNumBookmarks, MENU_WRAP, NULL);
											if (pBookmarkMenu)
											{
												for(unsigned long i = 0; i < nNumBookmarks; i++)
												{
													wsprintfW(wchBookmarkText, L"Title %d %02d:%02d:%02d",
														m_Bookmarks[i].Loc.TitleNum, m_Bookmarks[i].Loc.TimeCode.bHours,
														m_Bookmarks[i].Loc.TimeCode.bMinutes, m_Bookmarks[i].Loc.TimeCode.bSeconds);

													pBookmarkMenu->AddItem(MITEM_ROUTINE, wchBookmarkText, &BookmarkMenuProc);
												}

												pBookmarkMenu->curitem = 0;
												pBookmarkMenu->Activate();
											}
										}
									}
								}
								else
								{
									ResetOSD(&IDS_NOTPERMITTED, true, false, OSD_NOTPERMITTED_DISPLAY_TIME);
								}
							}
						}
						break;


			case XB_SHIFT_DPAD_UP:
						{
							switch (m_ABRepeat.fABRepeatState)
							{
								case (ABREPEAT_A_SET) :
										if (S_OK == m_pPlayer->GetCurrentLocation(&m_ABRepeat.LocB))
										{
											DWORD dwBLoc	= (m_ABRepeat.LocB.TimeCode.bHours << 24) +
												(m_ABRepeat.LocB.TimeCode.bMinutes << 16) +
												(m_ABRepeat.LocB.TimeCode.bSeconds << 8) +
												(m_ABRepeat.LocB.TimeCode.bFrames);

											DWORD dwALoc	= (m_ABRepeat.LocA.TimeCode.bHours << 24) +
												(m_ABRepeat.LocA.TimeCode.bMinutes << 16) +
												(m_ABRepeat.LocA.TimeCode.bSeconds << 8) +
												(m_ABRepeat.LocA.TimeCode.bFrames);

											if (dwBLoc > dwALoc)
											{
												m_ABRepeat.fABRepeatState = ABREPEAT_A_AND_B_SET;

												m_pPlayer->Defrost(m_ABRepeat.FreezeDataA.pData,
													m_ABRepeat.FreezeDataA.dwSize);

												ResetOSD(&IDS_B_DOWN, true, false, OSD_DISPLAY_TIME);
												break;
											}
											else
											{
												// points are illegal; let switch pass thru to delete the points below
												ResetOSD(&IDS_ILLEGAL_AB, true, false, OSD_DISPLAY_TIME);
											}
										}

								case (ABREPEAT_A_AND_B_SET) :

										delete[] m_ABRepeat.FreezeDataA.pData;
										m_ABRepeat.FreezeDataA.dwSize = 0;
										ZeroMemory(&m_ABRepeat.LocB, sizeof(m_ABRepeat.LocB));
										m_ABRepeat.fABRepeatState = ABREPEAT_OFF;
										ResetOSD(&IDS_AB_OFF, true, false, OSD_DISPLAY_TIME);
										break;

								case (ABREPEAT_OFF) :

										if (S_OK == m_pPlayer->GetCurrentLocation(&m_ABRepeat.LocA))
										{
											if (S_OK == m_pPlayer->Freeze(m_ABRepeat.FreezeDataA.pData,
												m_ABRepeat.FreezeDataA.dwSize))
											{
												m_ABRepeat.fABRepeatState = ABREPEAT_A_SET;
												ResetOSD(&IDS_A_DOWN, true, false, OSD_DISPLAY_TIME);
											}
										}
										break;
							}
						}
						break;


			// MENU UP ACTION
			case XB_DPAD_UP:
						{
							ULONG uops;
							DVDPlayerMode lMode = m_pPlayer->GetPlayerMode();

							if(S_OK == m_pPlayer->GetCurrentUOPS(&uops))
							{
								if(!(uops & UOP_BUTTON))
								{
									m_pPlayer->SelectRelativeButton(DVD_Relative_Upper);
								}
								else if(m_bZoomed)
								{
									WORD wX, wY;
									float fScaleFactor;

									if (S_OK == m_pPlayer->Get_CurrentScaleFactor(fScaleFactor))
									{
										m_pPlayer->Get_VideoSize(wX, wY);

										fScaleFactor = max(1, fScaleFactor);

										WORD yRange = (float) wY * (1.0 - (1.0/fScaleFactor));
										m_nZoomY -= ZOOM_Y_INCREMENT;
										m_nZoomY = max(0, min(m_nZoomY, yRange));

										HRESULT hr = m_pPlayer->Trick_Zoom(m_nZoomX, m_nZoomY, 0.0);

										ResetCenterPoint();
									}
								}
							}
						}
						break;

			// MENU DOWN ACTION
			case XB_DPAD_DOWN:
						{
							ULONG uops;
							DVDPlayerMode lMode = m_pPlayer->GetPlayerMode();

							if(S_OK == m_pPlayer->GetCurrentUOPS(&uops))
							{
								if(!(uops & UOP_BUTTON))
								{
									m_pPlayer->SelectRelativeButton(DVD_Relative_Lower);
								}
								else if(m_bZoomed)
								{
									WORD wX, wY;
									float fScaleFactor;

									if (S_OK == m_pPlayer->Get_CurrentScaleFactor(fScaleFactor))
									{
										m_pPlayer->Get_VideoSize(wX, wY);

										fScaleFactor = max(1, fScaleFactor);

										WORD yRange = (float) wY * (1.0 - (1.0/fScaleFactor));
										m_nZoomY += ZOOM_Y_INCREMENT;
										m_nZoomY = max(0, min(m_nZoomY, yRange));

										HRESULT hr = m_pPlayer->Trick_Zoom(m_nZoomX, m_nZoomY, 0.0);

										ResetCenterPoint();
									}
								}
							}
						}
						break;


			// MENU LEFT ACTION
			case XB_DPAD_LEFT:
						{
							ULONG uops;
							if(S_OK == m_pPlayer->GetCurrentUOPS(&uops))
							{
								if(!(uops & UOP_BUTTON))

								{
									m_pPlayer->SelectRelativeButton(DVD_Relative_Left);
								}
								else
								{
									DVDPlayerMode lMode = m_pPlayer->GetPlayerMode();

									DVD_DOMAIN dom;
									m_pPlayer->GetCurrentDomain(&dom);

									if (m_bZoomed)
									{
										WORD wX, wY;
										float fScaleFactor;

										if (S_OK == m_pPlayer->Get_CurrentScaleFactor(fScaleFactor))
										{
											m_pPlayer->Get_VideoSize(wX, wY);

											fScaleFactor = max(1, fScaleFactor);

											WORD xRange = (float) wX * (1.0 - (1.0/fScaleFactor));
											m_nZoomX -= ZOOM_X_INCREMENT;
											m_nZoomX = max(0, min(m_nZoomX, xRange));

											HRESULT hr = m_pPlayer->Trick_Zoom(m_nZoomX, m_nZoomY, 0.0);
											ResetCenterPoint();
										}
									}
									else if (((lMode == DPM_PAUSED) || (lMode == DPM_STILL) ||
										(!m_bZoomed && (lMode == DPM_TRICKPLAY))) && (dom != DVD_DOMAIN_FirstPlay))
									{
										ResetOSD(&IDS_FRAMEREV, true, false, OSD_DISPLAY_TIME);
										m_pPlayer->Trick_FrameBackward();
									}
									else if(m_pPlayer->IsPlaying())
									{
										if (!(uops & UOP_PREV_TOP_PG_SEARCH))
										{
											WCHAR chChapText[64];
											DVD_PLAYBACK_LOCATION2 dvdLoc;

											if (S_OK == m_pPlayer->GetCurrentLocation(&dvdLoc))
											{
												if ((dvdLoc.TitleNum > 0) && (dvdLoc.ChapterNum > 1))
												{
													int nChap = max(1, dvdLoc.ChapterNum - 1);

													wsprintfW(chChapText, L"Title %d\nChapter %d", dvdLoc.TitleNum, nChap);
													ResetOSD((WCHAR*) &chChapText, true, false, OSD_DISPLAY_TIME);

												}
											}
											m_pPlayer->PreviousChapter();
										}
									}
								}
							}
						}
						break;

			// MENU RIGHT ACTION
			case XB_DPAD_RIGHT:
						{
							ULONG uops;
							if(S_OK == m_pPlayer->GetCurrentUOPS(&uops))
							{
								if(!(uops & UOP_BUTTON))
								{
									m_pPlayer->SelectRelativeButton(DVD_Relative_Right);
								}
								else
								{
									DVDPlayerMode lMode = m_pPlayer->GetPlayerMode();

									DVD_DOMAIN dom;
									m_pPlayer->GetCurrentDomain(&dom);

									if (m_bZoomed)
									{
										WORD wX, wY;
										float fScaleFactor;

										if (S_OK == m_pPlayer->Get_CurrentScaleFactor(fScaleFactor))
										{
											m_pPlayer->Get_VideoSize(wX, wY);

											fScaleFactor = max(1, fScaleFactor);

											WORD xRange = (float) wX * (1.0 - (1.0/fScaleFactor));
											m_nZoomX += ZOOM_X_INCREMENT;
											m_nZoomX = max(0, min(m_nZoomX, xRange));

											HRESULT hr = m_pPlayer->Trick_Zoom(m_nZoomX, m_nZoomY, 0.0);
											ResetCenterPoint();
										}
									}
									else if (((lMode == DPM_PAUSED) || (lMode == DPM_STILL) ||
										(!m_bZoomed && (lMode == DPM_TRICKPLAY))) && (dom != DVD_DOMAIN_FirstPlay))
									{
										ResetOSD(&IDS_FRAMEFWD, true, false, OSD_DISPLAY_TIME);
										m_pPlayer->Trick_FrameForward();
									}
									else if(m_pPlayer->IsPlaying())
									{
										if (!(uops & UOP_NEXT_PG_SEARCH))
										{
											WCHAR chChapText[64];
											DVD_PLAYBACK_LOCATION2 dvdLoc;

											if (S_OK == m_pPlayer->GetCurrentLocation(&dvdLoc))
											{
												if ((dvdLoc.TitleNum > 0) && (dvdLoc.ChapterNum > 0))
												{
													// NOT DONE - need to compare to num of chapters in title
													int nChap = dvdLoc.ChapterNum + 1;

													ULONG lChapsInTitle = 0;
													if (m_pPlayer->GetNumberOfChapters(dvdLoc.TitleNum,
														&lChapsInTitle) != S_OK)
															lChapsInTitle = 0;

													if (nChap <= lChapsInTitle)
													{
														wsprintfW(chChapText, L"Title %d\nChapter %d",
															dvdLoc.TitleNum, nChap);
														ResetOSD((WCHAR*) &chChapText, true, false, OSD_DISPLAY_TIME);
													}
												}
											}
											m_pPlayer->NextChapter();
										}
									}
								}
							}
						}
						break;

			// RESUME ACTION
			case XB_START:
						{
							ULONG uops;
							if(S_OK == m_pPlayer->GetCurrentUOPS(&uops))
							{
								if(!(uops & UOP_BUTTON))
								{
									m_pPlayer->Resume();

								}
								else if (!(uops & UOP_TITLE_PLAY))
								{
									ResetOSD(&IDS_PLAY, true, false, OSD_DISPLAY_TIME);
									m_pPlayer->Play();
								}
							}
						}
						break;

			// GO UP(RETURN) OR EXIT ACTION
			case XB_BACK:
						{
							ULONG uops;
							if(S_OK == m_pPlayer->GetCurrentUOPS(&uops))
							{
								if(!(uops & UOP_GO_UP))

								{
//										ResetOSD(IDS_RETURN, true, false, 1);
									m_pPlayer->ReturnFromSubmenu();
								}
								else
									ResetOSD(&IDS_NOTPERMITTED, true, false, OSD_NOTPERMITTED_DISPLAY_TIME);
							}
							else
							{
								// Exit(0);
							}
						}
						break;

			case XB_LTHUMB:
						break;

			case XB_RTHUMB:
						break;

			default:	break;
		}

		if(m_bLThumbStickOn)
		{
			DVD_DOMAIN domain;
			if (S_OK == m_pPlayer->GetCurrentDomain(&domain) && (domain != DVD_DOMAIN_Stop))
			{
				WORD wX, wY;
				float fScaleFactor;
				if (S_OK == m_pPlayer->Get_CurrentScaleFactor(fScaleFactor))
				{
					m_pPlayer->Get_VideoSize(wX, wY);

					fScaleFactor = max(1, fScaleFactor);

					WORD xRange = (float) wX * (1.0 - (1.0/fScaleFactor));
					WORD yRange = (float) wY * (1.0 - (1.0/fScaleFactor));

					if ((m_fLThumbStickX <= -0.75) || (m_fLThumbStickX >= 0.75))
					{
						m_nZoomX = (m_fLThumbStickX <= -0.75) ? m_nZoomX - ZOOM_X_INCREMENT:
							m_nZoomX + ZOOM_X_INCREMENT;
					}

					if ((m_fLThumbStickY <= -0.75) || (m_fLThumbStickY >= 0.75))
					{
						m_nZoomY = (m_fLThumbStickY <= -0.75) ? m_nZoomY + ZOOM_Y_INCREMENT:
							m_nZoomY - ZOOM_Y_INCREMENT;
					}

					m_nZoomX = max(0, min(m_nZoomX, xRange));
					m_nZoomY = max(0, min(m_nZoomY, yRange));

					ResetCenterPoint();

					HRESULT hr = m_pPlayer->Trick_Zoom(m_nZoomX, m_nZoomY, m_nZoomW, m_nZoomH);
				}
			}
		}

		if(m_bRThumbStickOn)
		{
			DVD_DOMAIN domain;
			if (S_OK == m_pPlayer->GetCurrentDomain(&domain) && (domain != DVD_DOMAIN_Stop))
			{
				if ((m_fRThumbStickY <= -0.75) || (m_fRThumbStickY >= 0.75))
				{
					// calc x and y zoom increment based on w/h ratio
					// this assures we get a smooth zoom with as little bob as possible due to scaling
					WORD wX, wY;
					if (S_OK == m_pPlayer->Get_VideoSize(wX, wY))
					{
						if (!m_nZoomW) // width/height not yet set
							{
								m_nZoomW = wX;
								m_nZoomH = wY;
							}

						// reduce the w/h ratio and use a multiple of those numbers as the base increment
						int wNum, wDen;

						if ((wX) && (wY))
						{
							ReduceRatio(wX, wY, wNum, wDen);

							UINT nZoomYInc = wDen * 4;
							UINT nZoomXInc = wNum * 4;

							// minimum width is 2 x Increment, maximum is full width
							m_nZoomW = (m_fRThumbStickY <= -0.75) ?
								min(m_nZoomW + nZoomXInc, wX) :
								max(m_nZoomW - nZoomXInc, nZoomXInc * 2);

							// minimum Height is 2 x Increment, maximum is full height
							m_nZoomH = (m_fRThumbStickY <= -0.75) ?
								min(m_nZoomH + nZoomYInc, wY) :
								max(m_nZoomH - nZoomYInc, nZoomYInc * 2);

							// x position depends on our new width
							WORD xRange = (float) wX - m_nZoomW;

							// y position depends on our new height
							WORD yRange = (float) wY - m_nZoomH;

							if (0 == xRange)
								ResetZoomVariables();
							else
								m_bZoomed = true;

							// maintain the fixed center point until user pans
							if ((!m_CenterPoint.x) && (!m_CenterPoint.y))
								ResetCenterPoint();

							m_nZoomX = m_CenterPoint.x - (m_nZoomW / 2.0f);
							m_nZoomY = m_CenterPoint.y - (m_nZoomH / 2.0f);

							m_nZoomX = max(0, min(m_nZoomX, xRange));
							m_nZoomY = max(0, min(m_nZoomY, yRange));

							HRESULT hr = m_pPlayer->Trick_Zoom(m_nZoomX, m_nZoomY, m_nZoomW, m_nZoomH);
						}
					}
				}
			}
		}
	}
}

void CRavDvdApp::DrawOsd()
{
	if(m_bShowOSDText)
	{
		if (wcscmp(m_pOSDTextBuf, IDS_NOTPERMITTED) == 0)
		{
			m_NotPermitted.Render();
		}
		else if (wcscmp(m_pOSDTextBuf, IDS_BOOKMARK) == 0)
		{
			m_BookmarkOSD.Render();
		}
		else
			m_Font.DrawText( OSD_ACTION_LEFT, OSD_ACTION_TOP, 0xffffffff, (WCHAR*) m_pOSDTextBuf);
	}

	if(m_bShowScanSpeed)
	{
		m_Font.DrawText( OSD_SCAN_LEFT, OSD_SCAN_TOP, 0xffffffff, (WCHAR*) m_pScanSpeedTextBuf);
	}

	if (m_bDisplayTime)
	{
		WCHAR chTime[256];
		DVD_PLAYBACK_LOCATION2 dvdLoc;

		if (m_pPlayer)
		{
			if (S_OK == m_pPlayer->GetCurrentLocation(&dvdLoc))
			{
				if ((dvdLoc.TitleNum > 0) && (dvdLoc.ChapterNum > 0))
				{
					wsprintfW(chTime, L"%02d:%02d:%02d", dvdLoc.TimeCode.bHours, dvdLoc.TimeCode.bMinutes,
							dvdLoc.TimeCode.bSeconds);
				}
				else
				{
					wsprintfW(chTime, L"--:--:--");
				}

				m_Font.DrawText( OSD_TIME_LEFT, OSD_TIME_TOP, 0xffffffff, (WCHAR*) chTime);
			}
		}
	}
}



//-----------------------------------------------------------------------------
// Name: Run()
// Desc:
//-----------------------------------------------------------------------------
HRESULT CRavDvdApp::Render()
{

#if STRESS_TEST_RANDOM_SEEK
	DVD_PLAYBACK_LOCATION2 dvdLoc;
	static unsigned int count = 0;

	if (m_pPlayer && !(count++%120))
	{
		if (S_OK == m_pPlayer->GetCurrentLocation(&dvdLoc))
		{
			if ((dvdLoc.TitleNum > 0) && (dvdLoc.ChapterNum > 0))
			{
//				wsprintfW(chTime, L"%02d:%02d:%02d", dvdLoc.TimeCode.bHours, dvdLoc.TimeCode.bMinutes,
//						dvdLoc.TimeCode.bSeconds);
				DVD_HMSF_TIMECODE time;
				if(S_OK == m_pPlayer->GetTotalTitleTime(&time, NULL, NULL) && (time.bMinutes || time.bHours))
				{
					static bool initRand = false;
					if(!initRand)
					{
						srand(1);
						initRand = true;
					}else
					{
//						srand(rand());
					}

					unsigned long nTime, tTime = time.bHours * 3600 + time.bMinutes * 60 + time.bSeconds;

					nTime = (tTime * rand()) / RAND_MAX;

					if((tTime - nTime) < 3)
					{
						nTime -= 3;
					}

					time.bHours = nTime / 3600;
					nTime %= 3600;
					time.bMinutes = nTime / 60;
					nTime %= 60;
					time.bSeconds = nTime;

					if(DPM_PLAYING == m_pPlayer->GetPlayerMode())
					{
						m_pPlayer->TimeSearch(time.bHours, time.bMinutes, time.bSeconds);
#if _DEBUG
						char buf[MAX_PATH];
						wsprintf(buf, "Seeking to time %d:%d:%d\n", time.bHours, time.bMinutes, time.bSeconds);
						OutputDebugString(buf);
#endif _DEBUG
					}
				}
			}
		}
	}
#endif

//	Sleep(20);
	m_pd3dDevice->BlockUntilVerticalBlank();

	if(m_bShowOSDText)
	{
		float fCurTime = XBUtil_Timer(TIMER_GETAPPTIME);
		if(fCurTime >= m_fOSDDisplayTime)
		{
			m_bShowOSDText = false;
		}
	}

	if (PARENTAL_LEVEL_NOCHANGE != m_dwNewParentalLevel)
		HandleParentalLevelChange();

	if (m_ABRepeat.fABRepeatState == ABREPEAT_A_AND_B_SET)
	{
		DVD_PLAYBACK_LOCATION2 dvdLoc;

		if (S_OK == m_pPlayer->GetCurrentLocation(&dvdLoc))
		{
			DWORD dwCurrent	= (dvdLoc.TimeCode.bHours << 24) + (dvdLoc.TimeCode.bMinutes << 16) +
				(dvdLoc.TimeCode.bSeconds << 8) + (dvdLoc.TimeCode.bFrames);
			DWORD dwBLoc	= (m_ABRepeat.LocB.TimeCode.bHours << 24) + (m_ABRepeat.LocB.TimeCode.bMinutes << 16) +
				(m_ABRepeat.LocB.TimeCode.bSeconds << 8) + (m_ABRepeat.LocB.TimeCode.bFrames);

			if (dwCurrent >= dwBLoc)
				m_pPlayer->Defrost(m_ABRepeat.FreezeDataA.pData,
					m_ABRepeat.FreezeDataA.dwSize);
		}
	}

	for(int i=0; i<4; i++)
	{
		#ifdef _DEBUG
			if(m_GamePads[i].wPressedButtons)
			{
				char buf[256];
				wsprintf(buf,"wPressedButtons: %d\n", m_GamePads[i].wPressedButtons);
				OutputDebugString(buf);
			}

			if(m_GamePads[i].wButtons)
			{
				char buf[256];
				wsprintf(buf,"wButtons: %d\n", m_GamePads[i].wButtons);
				OutputDebugString(buf);
			}
		#endif


		// first see if we're in an app menu
		bool bReturn = false;
		XBRavMenu* curMenu = XBRavMenu::GetActiveMenu();

		if(XBRavMenu::IsActive())
		{
			DWORD dwMenuCommand = curMenu->ProcessGamePadInput(&m_GamePads[i]);

			if (dwMenuCommand == MENU_ACTIVATE || dwMenuCommand == MENU_BACK)
				{
				curMenu->ProcessCommand(MENU_BACK);
				delete curMenu;
				curMenu = NULL;
				}
			else
				{
//				m_pd3dDevice->Clear( 0L, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER,
//                         BACKGROUND_COLOR, 1.0f, 0L );
				curMenu->ProcessCommand(dwMenuCommand);
				}

			bReturn = true;
		}
		else
		{
			// determine if button or control has changed and perform necessary action
			DWORD dwInputAction = GET_INPUT_ACTION(&m_GamePads[i]);

			m_fLThumbStickX = m_GamePads[i].fX1;
			m_fLThumbStickY = m_GamePads[i].fY1;
			m_fRThumbStickX = m_GamePads[i].fX2;
			m_fRThumbStickY = m_GamePads[i].fY2;

			if(m_fLThumbStickX != 0.0 || m_fLThumbStickY != 0.0)
				m_bLThumbStickOn = true;
			else
				m_bLThumbStickOn = false;

			if(m_fRThumbStickX != 0.0 || m_fRThumbStickY != 0.0)
				m_bRThumbStickOn = true;
			else
				m_bRThumbStickOn = false;

			if (m_fParentalRightsTooLow)
			{
				// THH -- discard the button press if parental screen is displayed
				if (dwInputAction != XB_NO_INPUT)
				{
					m_pPlayer->SelectParentalLevel(8);
					m_fParentalRightsTooLow = false;

					if (m_pPlayer)
						m_pPlayer->Play();
				}
			}
			else
				ProcessGamePadInput(dwInputAction);
		}
	}

	m_pd3dDevice->BeginScene();

	if (m_fParentalRightsTooLow)
	{
		m_pd3dDevice->Clear( 0L, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER,
							 0xFF000000, 1.0f, 0L );

		m_Font.DrawText( 155, 170, 0xffffffff, (WCHAR*) IDS_PARENTAL_WARNING);
		m_Font.DrawText( 184, 190, 0xffffffff, (WCHAR*) IDS_PARENTAL_WARNING2);
		m_Font.DrawText( 191, 292, 0xffffffff, (WCHAR*) IDS_PARENTAL_OVERRIDE);
	}
	else
	{
		DVD_DOMAIN domain;
		if (S_OK == m_pPlayer->GetCurrentDomain(&domain))
		{
			if (domain != DVD_DOMAIN_Stop)
			{
				BOOL bColorkeyEnabled = FALSE;
				if (S_OK == m_pPlayer->GetColorkeyStatus(bColorkeyEnabled))
				{
					if (bColorkeyEnabled)
					{
						if (g_fSettingsMenuActive)
						{
							m_pd3dDevice->Clear( 0L, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER,
									 0xFF000000, 1.0f, 0L );

							m_pd3dDevice->Clear( 1L, &MENU_RECT, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER,
										 RAV_COLORKEY, 1.0f, 0L );
						}
						else
							m_pd3dDevice->Clear( 0L, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER,
								 RAV_COLORKEY, 1.0f, 0L );
					}
					else
						m_pd3dDevice->Clear( 0L, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER,
								 0xFF000000, 1.0f, 0L );
				}
				else
					m_pd3dDevice->Clear( 0L, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER,
							 0xFF000000, 1.0f, 0L );
			}
			else
			{
				m_pd3dDevice->Clear( 0L, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER,
									 0xFF000000, 1.0f, 0L );
				if (!g_fSettingsMenuActive)
					m_Splash.Render();
			}
		}
		else
		{
			m_pd3dDevice->Clear( 0L, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER,
								 0xFF000000, 1.0f, 0L );
			m_Splash.Render();
		}
	}

	if (m_nHelpScreen != HELP_OFF)
	{
		if (m_nHelpScreen == HELP_NORMAL)
		{
			m_Help.Render( &m_Font, g_HelpCallouts, NUM_HELP_CALLOUTS);
			m_Font.DrawText( 124, 390, 0xffffffff, (WCHAR*) NORMAL_HELP_TITLE);
		}
		else
		{
			m_Help.Render( &m_Font, g_ShiftHelpCallouts, NUM_SHIFT_HELP_CALLOUTS);
			m_Font.DrawText( 320, 370, 0xffffffff, (WCHAR*) SHIFT_HELP_TITLE1, XBFONT_CENTER_X);
			m_Font.DrawText( 320, 392, 0xffffffff, (WCHAR*) SHIFT_HELP_TITLE2, XBFONT_CENTER_X);
			m_Font.DrawText( 320, 414, 0xffffffff, (WCHAR*) SHIFT_DEFINITION, XBFONT_CENTER_X);
		}
	}
    else
    {
		// we need to draw the OSD even if a menu is up, in case the time display is showing
		DrawOsd();

		// is a menu still active?
		if(XBRavMenu::IsActive())
			XBRavMenu::GetActiveMenu()->Render();
	}

	m_pd3dDevice->EndScene();

	if (g_fSettingsMenuActive && (!g_fScaledDestination) && XBRavMenu::IsActive())
	{
		m_pPlayer->SetDestinationRect(MENU_RECT.x1, MENU_RECT.y1, MENU_RECT.x2-MENU_RECT.x1,
			MENU_RECT.y2-MENU_RECT.y1);
		g_fScaledDestination = true;
	}
	else if(!XBRavMenu::IsActive())
	{
		if (g_fScaledDestination)
		{
			g_theApp->m_pPlayer->SetDestinationRect(MENU_RECT.x1, MENU_RECT.y1, 0, 0);
			g_fScaledDestination = false;
		}
		g_fSettingsMenuActive = false;
	}

	return S_OK;
}


void  WINAPI DVDEventHandler(DWORD dwEvent, DWORD dwInfo)
{
	// put application responses to various events here
	switch (dwEvent)
	{
		case (EC_DVD_TITLE_CHANGE):
							g_theApp->ResetZoomVariables();
							OutputDebugString("DVDEventHandler: EC_DVD_TITLE_CHANGE\n");
							break;

		case (EC_DVD_CHAPTER_START):
							OutputDebugString("DVDEventHandler: EC_DVD_CHAPTER_START\n");
							break;

		case (EC_DVD_VALID_UOPS_CHANGE):
							OutputDebugString("DVDEventHandler: EC_DVD_VALID_UOPS_CHANGE\n");
							break;

		case (EC_DVD_AUDIO_STREAM_CHANGE):
							OutputDebugString("DVDEventHandler: EC_DVD_AUDIO_STREAM_CHANGE\n");
							break;

		case (EC_DVD_SUBPICTURE_STREAM_CHANGE):
							OutputDebugString("DVDEventHandler: EC_DVD_SUBPICTURE_STREAM_CHANGE\n");
							break;

		case (EC_DVD_DOMAIN_CHANGE):
							OutputDebugString("DVDEventHandler: EC_DVD_DOMAIN_CHANGE\n");
							g_theApp->ResetZoomVariables();
							g_theApp->ResetScanSpeed(true, true);
							break;

		case (EC_DVD_PARENTAL_LEVEL_CHANGE):
							OutputDebugString("DVDEventHandler: EC_DVD_PARENTAL_LEVEL_CHANGE\n");
							g_theApp->m_dwNewParentalLevel = dwInfo;
							break;

		case (EC_DVD_STILL_ON):
							OutputDebugString("DVDEventHandler: EC_DVD_STILL_ON\n");
							break;

		case (EC_DVD_STILL_OFF):
							OutputDebugString("DVDEventHandler: EC_DVD_STILL_OFF\n");
							break;

		case (EC_DVD_PLAYBACK_STOPPED):
							OutputDebugString("DVDEventHandler: EC_DVD_PLAYBACK_STOPPED\n");
							g_theApp->ResetZoomVariables();
							g_theApp->ResetScanSpeed(true, true);
							g_theApp->ResetMenus();
							break;

		case (EC_DVD_ERROR):
							switch(dwInfo)
							{
								case GNR_PARENTAL_LEVEL_TOO_LOW:
									g_theApp->m_fParentalRightsTooLow = true;
									break;
							}
							OutputDebugString("DVDEventHandler: EC_DVD_ERROR\n");
							break;

		case (EC_DVD_PLAYBACK_RATE_CHANGE):
							OutputDebugString("DVDEventHandler: EC_DVD_PLAYBACK_RATE_CHANGE\n");
							if (dwInfo == 1000)
							{
								// we're back to normal speed because of domain change so we must
								// reset any scanning that may have been going on
								g_theApp->ResetScanSpeed(true, true);
							}
							break;

		case (EC_ERRORABORT):
							OutputDebugString("DVDEventHandler: EC_ERRORABORT\n");
							break;

		case (EC_DVD_DISPLAY_MODE_CHANGE):
							OutputDebugString("DVDEventHandler: EC_DVD_DISPLAY_MODE_CHANGE\n");
							break;

		case (EC_DVD_PLAYPERIOD_AUTOSTOP):
							OutputDebugString("DVDEventHandler: EC_DVD_PLAYPERIOD_AUTOSTOP\n");
							break;
		default:
							break;
	}
}


//-----------------------------------------------------------------------------
// Name: VBlankCallback
// Desc: This routine is called at the beginning of the vertical blank.
//-----------------------------------------------------------------------------
void __cdecl VBlankCallback(D3DVBLANKDATA *pData)
{
	g_dwVBCount++;
}


DWORD ChapterMenuProc(DWORD cmd, XMenuItem *it)
{
	if(it == NULL)
		return -1;

	int nChapter = ((XBRavMenu*)(it->menu))->GetItemIndex(it) + 1;

	if(g_theApp->m_pPlayer)
	{
		g_theApp->m_pPlayer->PlayChapterInTitle(g_curTitle, nChapter);
	}

	if(it->menu->parent)
	{
		delete ((XBRavMenu*)it->menu->parent);
		it->menu->parent = NULL;
	}

//	delete ((XBRavMenu*)(it->menu));
	return MROUTINE_DIE;
}

DWORD TitleMenuProc(DWORD cmd, XMenuItem *it)
{
	if(it == NULL)
		return -1;

	int nTitle = ((XBRavMenu*)(it->menu))->GetItemIndex(it) + 1;

	g_curTitle = nTitle;

	if(g_theApp->m_pPlayer)
	{
		ULONG nChapters = 0;
		g_theApp->m_pPlayer->GetNumberOfChapters(nTitle, &nChapters);
		if(nChapters > 1)
		{
			XBRavMenu * chaptMenu = new XBRavMenu(NULL, 320.0f, 100.0f,
						nChapters, MENU_WRAP, (DWORD*)ChapterMenuProc, (XBRavMenu*)(it->menu) );

			for(int i = 0; i<nChapters; i++)
			{
			    WCHAR strChaptName[MAX_PATH];

				wsprintfW(strChaptName, L"Chapter %d", i+1);

				chaptMenu->AddItem(MITEM_ROUTINE, strChaptName, &ChapterMenuProc);
			}

			DVD_PLAYBACK_LOCATION2 loc;

			if (S_OK == g_theApp->m_pPlayer->GetCurrentLocation(&loc) && nTitle == loc.TitleNum && loc.ChapterNum < nChapters)
			{
				chaptMenu->curitem = loc.ChapterNum - 1;
			}

			((XBRavMenu*)(it->menu))->Activate(false);
			chaptMenu->Activate();
			return MROUTINE_ABORT;
		}
		else
		{
			g_theApp->m_pPlayer->PlayTitle(nTitle);
//			delete ((XBRavMenu*)(it->menu));
			return MROUTINE_DIE;
		}
	}

	return MROUTINE_DIE;
}

DWORD AudioMenuProc(DWORD cmd, XMenuItem *it)
{
	if(it == NULL)
		return -1;

	int nAudioStream = ((XBRavMenu*)(it->menu))->GetItemIndex(it);

	// set audio stream
	if(g_theApp->m_pPlayer)
		g_theApp->m_pPlayer->SelectAudioStream(nAudioStream);

//	delete ((XBRavMenu*)(it->menu));
	return MROUTINE_DIE;
}

DWORD SubpicMenuProc(DWORD cmd, XMenuItem *it)
{
	if(it == NULL)
		return -1;

	int nSubpicStream = ((XBRavMenu*)(it->menu))->GetItemIndex(it);

	if(g_theApp->m_pPlayer)
	{
		ULONG lNumSubpic, lCurSubpic;
		BOOL bEnabled;

		g_theApp->m_pPlayer->GetCurrentSubpicture(&lNumSubpic, &lCurSubpic, &bEnabled);

		if (nSubpicStream < lNumSubpic)
			g_theApp->m_pPlayer->SelectSubpictureStream(nSubpicStream);
		else
			g_theApp->m_pPlayer->SetSubpictureState(FALSE);
	}

//	delete ((XBRavMenu*)(it->menu));
	return MROUTINE_DIE;
}

DWORD AngleMenuProc(DWORD cmd, XMenuItem *it)
{
	if(it == NULL)
		return -1;

	int nAngleStream = ((XBRavMenu*)(it->menu))->GetItemIndex(it);

	if(g_theApp->m_pPlayer)
		g_theApp->m_pPlayer->SelectAngle(nAngleStream+1);		// angle is 1-based

//	delete ((XBRavMenu*)(it->menu));
	return MROUTINE_DIE;
}

DWORD ParentalControlMenuProc(DWORD cmd, XMenuItem *it)
{
	if(it == NULL)
		return -1;

	int nAudioStream = ((XBRavMenu*)(it->menu))->GetItemIndex(it);

	// set audio stream
	if(g_theApp->m_pPlayer)
		g_theApp->m_pPlayer->SelectAudioStream(nAudioStream);

//	delete ((XBRavMenu*)(it->menu));
	return MROUTINE_DIE;
}

DWORD BookmarkMenuProc(DWORD cmd, XMenuItem *it)
{
	if(it == NULL)
		return -1;

	int nBookmark = ((XBRavMenu*)(it->menu))->GetItemIndex(it);

	if (g_theApp->m_Bookmarks[nBookmark].Loc.TitleNum > 0)
	{
		g_theApp->m_pPlayer->Defrost(g_theApp->m_Bookmarks[nBookmark].FreezeData.pData,
			g_theApp->m_Bookmarks[nBookmark].FreezeData.dwSize);
	}

	return MROUTINE_DIE;
}



#include "library\hardware\audio\generic\ac3setup.h"

DWORD AudioDynRngMenuProc(DWORD cmd, XMenuItem *it)
{
	if(it == NULL)
		return -1;

	AC3DynamicRange drm = (AC3DynamicRange) ((XBRavMenu*)(it->menu))->GetItemIndex(it);

	if(g_theApp->m_pPlayer && drm <= AC3DYNRG_TV)
		g_theApp->m_pPlayer->SetAudioDynRngCompMode(drm);
	else
		g_theApp->m_pPlayer->SetAudioDynRngCompMode((AC3DynamicRange) -1);

//	delete ((XBRavMenu*)(it->menu));
	return MROUTINE_DIE;
}

DWORD AudioDownmixModeMenuProc(DWORD cmd, XMenuItem *it)
{
	if(it == NULL)
		return -1;

	WORD value = (WORD) ((XBRavMenu*)(it->menu))->GetItemIndex(it);

	if(g_theApp->m_pPlayer)
		g_theApp->m_pPlayer->SetSpeakerOutputMode(value);


	return MROUTINE_DIE;
}

#if ENABLE_DMM_SETTINGS
DWORD AudioDualModeMenuProc(DWORD cmd, XMenuItem *it)
{
	if(it == NULL)
		return -1;

	AC3DualModeConfig dm = (AC3DualModeConfig) ((XBRavMenu*)(it->menu))->GetItemIndex(it);

	if(g_theApp->m_pPlayer)
		g_theApp->m_pPlayer->SetAudioDualMode(dm);

//	delete ((XBRavMenu*)(it->menu));
	return MROUTINE_DIE;
}
#endif ENABLE_DMM_SETTINGS

//DWORD AudioSPDIFOutMenuProc(DWORD cmd, XMenuItem *it)
//{
//	if(it == NULL)
//		return -1;
//
//	SPDIFOutputMode som = (SPDIFOutputMode) ((XBRavMenu*)(it->menu))->GetItemIndex(it);
//
//	if(g_theApp->m_pPlayer)
//		g_theApp->m_pPlayer->SetAudioSpdifOutMode(som);
//
////	delete ((XBRavMenu*)(it->menu));
////	g_theApp->m_bDeleteMenusOnStop = true;
//	return MROUTINE_DIE;
//}

DWORD AudioLFEMenuProc(DWORD cmd, XMenuItem *it)
{
	if(it == NULL)
		return -1;

	BOOL lfe = (BOOL) ((XBRavMenu*)(it->menu))->GetItemIndex(it);

	if(g_theApp->m_pPlayer)
		g_theApp->m_pPlayer->SetAudioLFEMode(lfe);

//	delete ((XBRavMenu*)(it->menu));
	return MROUTINE_DIE;
}

DWORD AudioDiagNormMenuProc(DWORD cmd, XMenuItem *it)
{
	if(it == NULL)
		return -1;

	AC3DialogNorm dnm = (AC3DialogNorm) ((XBRavMenu*)(it->menu))->GetItemIndex(it);

	if(g_theApp->m_pPlayer)
		g_theApp->m_pPlayer->SetAudioDialogNormMode(dnm);

//	delete ((XBRavMenu*)(it->menu));
	return MROUTINE_DIE;
}

DWORD AudioCompModeMenuProc(DWORD cmd, XMenuItem *it)
{
	if(it == NULL)
		return -1;

	AC3OperationalMode opm = (AC3OperationalMode) ((XBRavMenu*)(it->menu))->GetItemIndex(it);

	if(g_theApp->m_pPlayer)
		g_theApp->m_pPlayer->SetAudioCompMode(opm);

//	delete ((XBRavMenu*)(it->menu));
	return MROUTINE_DIE;
}

DWORD AudioCompSFHighMenuProc(DWORD cmd, XMenuItem *it)
{
	if(it == NULL)
		return -1;

	WORD sfh = 5000 * (((XBRavMenu*)(it->menu))->GetItemIndex(it)); // scale goes from 0 to 0x100

	if(g_theApp->m_pPlayer)
		g_theApp->m_pPlayer->SetAudioCompSFHigh(sfh);

//	delete ((XBRavMenu*)(it->menu));
	return MROUTINE_DIE;
}

DWORD AudioCompSFLowMenuProc(DWORD cmd, XMenuItem *it)
{
	if(it == NULL)
		return -1;

	WORD sfl = 5000 * (((XBRavMenu*)(it->menu))->GetItemIndex(it)); // scale goes from 0 to 0x100

	if(g_theApp->m_pPlayer)
		g_theApp->m_pPlayer->SetAudioCompSFLow(sfl);

//	delete ((XBRavMenu*)(it->menu));
	return MROUTINE_DIE;
}


#define NUM_SETTINGS_MENU_ITEMS 15
#define MENU_BACK_COLOR			0xff7e9f71
//#define MENU_BACK_COLOR			0xff005500
#define MENU_HIGHLIGHT_COLOR	0xffcedfad
//#define MENU_HIGHLIGHT_COLOR	0xff333333
#define MENU_TEXT_COLOR			0xffffffff
//#define MENU_TEXT_COLOR			RAV_COLORKEY


DWORD DolbySettingsMenuProc(DWORD cmd, XMenuItem *it)
{
	if(it == NULL)
		return -1;

	XBRavMenu * pSubMenu, * pCallingMenu = (XBRavMenu*)it->menu;
	XBMenuProfile profile;

	profile.title[0]			= NULL;
	profile.maxitems			= NUM_SETTINGS_MENU_ITEMS;
	profile.flags				= MENU_WRAP | MENU_RIGHT;
	profile.hideparent			= false;
	profile.showbackground		= true;
	profile.showhighlight		= true;
	profile.showtextedge		= false;
	profile.topcolor			= MENU_BACK_COLOR;
	profile.bottomcolor			= MENU_BACK_COLOR;
	profile.itemcolor			= MENU_TEXT_COLOR;
	profile.seltopcolor			= MENU_HIGHLIGHT_COLOR;
	profile.selbotcolor			= MENU_HIGHLIGHT_COLOR;


	WORD posX, posY;

	posX = pCallingMenu->GetX() + pCallingMenu->GetW() + 2;
	posY = pCallingMenu->GetY(); // temp fix while font size is miss reported...

	switch(pCallingMenu->GetItemIndex(it))
	{
		case 0://Dynamic Range Control
		{
			pSubMenu = new XBRavMenu(&profile, posX, posY, (DWORD*)AudioDynRngMenuProc, pCallingMenu);

			pSubMenu->AddItem(MITEM_ROUTINE, L"Standard - Def", (DWORD*)AudioDynRngMenuProc);
			pSubMenu->AddItem(MITEM_ROUTINE, L"Maximum", (DWORD*)AudioDynRngMenuProc);
			pSubMenu->AddItem(MITEM_ROUTINE, L"Minimum", (DWORD*)AudioDynRngMenuProc);
			pSubMenu->AddItem(MITEM_ROUTINE, L"\"TV Style\"", (DWORD*)AudioDynRngMenuProc);
			pSubMenu->AddItem(MITEM_DISABLED, L"Unknown", (DWORD*)AudioDynRngMenuProc);

			AC3DynamicRange drm;

			if(g_theApp->m_pPlayer->GetAudioDynRngCompMode(drm) == S_OK)
			{
				if((int) drm == -1)
					pSubMenu->curitem = pSubMenu->nitems - 1;
				else if((DWORD) drm < pSubMenu->nitems)
					pSubMenu->curitem = (DWORD) drm;
			}
			else
			{
				OutputDebugString("VDR call to get audio setting failed.");
			}
			break;
		}
		case 1://downmix
		{
			pSubMenu = new XBRavMenu(&profile, posX, posY, (DWORD*)AudioDownmixModeMenuProc, pCallingMenu);
			pSubMenu->AddItem(MITEM_ROUTINE, L"Surround (LtRt) - Def", (DWORD*)AudioDownmixModeMenuProc);
			pSubMenu->AddItem(MITEM_ROUTINE, L"Stereo (LoRo)", (DWORD*)AudioDownmixModeMenuProc);
			pSubMenu->AddItem(MITEM_ROUTINE, L"Mono", (DWORD*)AudioDownmixModeMenuProc);

			AC3SpeakerConfig dmm;

			if(g_theApp->m_pPlayer->GetAudioDownmixMode(dmm) == S_OK)
			{
				if(dmm == AC3SC_20_NORMAL)
					pSubMenu->curitem = (DWORD) 1;
				else if(dmm == AC3SC_10)
					pSubMenu->curitem = (DWORD) 2;
				else
					pSubMenu->curitem = (DWORD) 0;
			}
			else
			{
				OutputDebugString("VDR call to get audio setting failed.");
			}
			break;
		}
#if ENABLE_DMM_SETTINGS
		case 2://output mode
		{
			pSubMenu = new XBRavMenu(&profile, posX, posY, (DWORD*)AudioDualModeMenuProc, pCallingMenu);
			pSubMenu->AddItem(MITEM_ROUTINE, L"Stereo - Def", (DWORD*)AudioDualModeMenuProc);
			pSubMenu->AddItem(MITEM_ROUTINE, L"Left", (DWORD*)AudioDualModeMenuProc);
			pSubMenu->AddItem(MITEM_ROUTINE, L"Right", (DWORD*)AudioDualModeMenuProc);
			pSubMenu->AddItem(MITEM_ROUTINE, L"Mix", (DWORD*)AudioDualModeMenuProc);

			AC3DualModeConfig dm;

			if(g_theApp->m_pPlayer->GetAudioDualMode(dm) == S_OK && (DWORD) dm < pSubMenu->nitems)
			{
				pSubMenu->curitem = (DWORD) dm;
			}
			else
			{
				OutputDebugString("VDR call to get audio setting failed.");
			}
			break;
		}
#endif ENABLE_DMM_SETTINGS
/*		case 3://SPDIF out Mode
		{
			pSubMenu = new XBRavMenu(&profile, posX, posY, (DWORD*)AudioSPDIFOutMenuProc, pCallingMenu);
			pSubMenu->AddItem(MITEM_ROUTINE, L"AC3", (DWORD*)AudioSPDIFOutMenuProc);
			pSubMenu->AddItem(MITEM_ROUTINE, L"PCM - Def", (DWORD*)AudioSPDIFOutMenuProc);

			SPDIFOutputMode som;

//			g_theApp->m_bDeleteMenusOnStop = false; // set back to true in AudioSPDIFOutMenuProc callback

			if(g_theApp->m_pPlayer->GetAudioSpdifOutMode(som) == S_OK && (DWORD) som < pSubMenu->nitems)
			{
				pSubMenu->curitem = (DWORD) som;
			}
			else
			{
				OutputDebugString("VDR call to get audio setting failed.");
			}
			break;
		}
*/
#if ENABLE_DMM_SETTINGS
		case 4://LFE Mode
#else
		case 3://LFE Mode
#endif ENABLE_DMM_SETTINGS
		{
			pSubMenu = new XBRavMenu(&profile, posX, posY, (DWORD*)AudioLFEMenuProc, pCallingMenu);
			pSubMenu->AddItem(MITEM_ROUTINE, L"Off - Def", (DWORD*)AudioLFEMenuProc);
			pSubMenu->AddItem(MITEM_ROUTINE, L"On", (DWORD*)AudioLFEMenuProc);

			BOOL lfe;

			if(g_theApp->m_pPlayer->GetAudioLFEMode(lfe) == S_OK && (DWORD) lfe < pSubMenu->nitems)
			{
				pSubMenu->curitem = (DWORD) lfe;
			}
			else
			{
				OutputDebugString("VDR call to get audio setting failed.");
			}
			break;
		}
#if ENABLE_DMM_SETTINGS
		case 5://dialog normalization
#else
		case 4://dialog normalization
#endif ENABLE_DMM_SETTINGS
		{
			pSubMenu = new XBRavMenu(&profile, posX, posY, (DWORD*)AudioDiagNormMenuProc, pCallingMenu);
			pSubMenu->AddItem(MITEM_ROUTINE, L"On - Def", (DWORD*)AudioDiagNormMenuProc);
			pSubMenu->AddItem(MITEM_ROUTINE, L"Off", (DWORD*)AudioDiagNormMenuProc);

			AC3DialogNorm dn;
			if(g_theApp->m_pPlayer->GetAudioDialogNormMode(dn) == S_OK && (DWORD) dn < pSubMenu->nitems)
			{
				pSubMenu->curitem = (DWORD) dn;
			}
			else
			{
				OutputDebugString("VDR call to get audio setting failed.");
			}
			break;
		}
#if ENABLE_DMM_SETTINGS
		case 6://compression
#else
		case 5://compression
#endif ENABLE_DMM_SETTINGS
		{
			pSubMenu = new XBRavMenu(&profile, posX, posY, (DWORD*)AudioCompModeMenuProc, pCallingMenu);
			pSubMenu->AddItem(MITEM_ROUTINE, L"Line - Def", (DWORD*)AudioCompModeMenuProc);
			pSubMenu->AddItem(MITEM_ROUTINE, L"RF", (DWORD*)AudioCompModeMenuProc);
			pSubMenu->AddItem(MITEM_ROUTINE, L"Custom Mode 0", (DWORD*)AudioCompModeMenuProc);
			pSubMenu->AddItem(MITEM_ROUTINE, L"Custom Mode 1", (DWORD*)AudioCompModeMenuProc);

			AC3OperationalMode opm;

			if(g_theApp->m_pPlayer->GetAudioCompMode(opm) == S_OK && (DWORD) opm < pSubMenu->nitems)
			{
				pSubMenu->curitem = (DWORD) opm;
			}
			else
			{
				OutputDebugString("VDR call to get audio setting failed.");
			}
			break;
		}
#if ENABLE_DMM_SETTINGS
		case 7://comp SF High
#else
		case 6://comp SF High
#endif ENABLE_DMM_SETTINGS
		{
			pSubMenu = new XBRavMenu(&profile, posX, posY, (DWORD*)AudioCompSFHighMenuProc, pCallingMenu);
			pSubMenu->AddItem(MITEM_ROUTINE, L"0.0", (DWORD*)AudioCompSFHighMenuProc);
//			pSubMenu->AddItem(MITEM_ROUTINE, L"0.25", (DWORD*)AudioCompSFHighMenuProc);
			pSubMenu->AddItem(MITEM_ROUTINE, L"0.5", (DWORD*)AudioCompSFHighMenuProc);
//			pSubMenu->AddItem(MITEM_ROUTINE, L"0.75", (DWORD*)AudioCompSFHighMenuProc);
			pSubMenu->AddItem(MITEM_ROUTINE, L"1.0", (DWORD*)AudioCompSFHighMenuProc);

			WORD sfh;

			if(g_theApp->m_pPlayer->GetAudioCompSFHigh(sfh) == S_OK && (DWORD) (sfh / 5000) < pSubMenu->nitems)
			{
				pSubMenu->curitem = (DWORD) (sfh / 5000);
			}
			else
			{
				OutputDebugString("VDR call to get audio setting failed.");
			}
			break;
		}
#if ENABLE_DMM_SETTINGS
		case 8://comp SF Low
#else
		case 7://comp SF Low
#endif ENABLE_DMM_SETTINGS
		{
			pSubMenu = new XBRavMenu(&profile, posX, posY, (DWORD*)AudioCompSFLowMenuProc, pCallingMenu);
			pSubMenu->AddItem(MITEM_ROUTINE, L"0.0", (DWORD*)AudioCompSFLowMenuProc);
//			pSubMenu->AddItem(MITEM_ROUTINE, L"0.25", (DWORD*)AudioCompSFLowMenuProc);
			pSubMenu->AddItem(MITEM_ROUTINE, L"0.5", (DWORD*)AudioCompSFLowMenuProc);
//			pSubMenu->AddItem(MITEM_ROUTINE, L"0.75", (DWORD*)AudioCompSFLowMenuProc);
			pSubMenu->AddItem(MITEM_ROUTINE, L"1.0", (DWORD*)AudioCompSFLowMenuProc);

			WORD sfl;

			if(g_theApp->m_pPlayer->GetAudioCompSFLow(sfl) == S_OK && (DWORD) (sfl / 5000) < pSubMenu->nitems)
			{
				pSubMenu->curitem = (DWORD) (sfl / 5000);
			}
			else
			{
				OutputDebugString("VDR call to get audio setting failed.");
			}
			break;
		}
#if ENABLE_DMM_SETTINGS
		case 9:// Exit Menu
#else
		case 8:// Exit Menu
#endif ENABLE_DMM_SETTINGS
		default:
		{
//			if (g_fScaledDestination)
//			{
//				g_theApp->m_pPlayer->SetDestinationRect(MENU_RECT.x1, MENU_RECT.y1, 0, 0);
//				g_fScaledDestination = false;
//			}
//			g_fSettingsMenuActive = false;
			return MROUTINE_DIE;
		}

	}

	((XBRavMenu*)((_xmenu*)pSubMenu)->parent)->Activate(false);
	pSubMenu->Activate();

//	if (g_fScaledDestination)
//	{
//		g_theApp->m_pPlayer->SetDestinationRect(MENU_RECT.x1, MENU_RECT.y1, 0, 0);
//		g_fScaledDestination = false;
//	}
//	g_fSettingsMenuActive = false;

	return MROUTINE_RETURN;
}

DWORD PresentationModeMenuProc(DWORD cmd, XMenuItem *it)
{
	if(it == NULL)
		return -1;

	DisplayPresentationMode mode = (DisplayPresentationMode) (((XBRavMenu*)(it->menu))->GetItemIndex(it)); // scale goes from 0 to 0x100

	if(g_theApp->m_pPlayer)
		g_theApp->m_pPlayer->SetDisplayMode(mode);

//	delete ((XBRavMenu*)(it->menu));
	return MROUTINE_DIE;
}

void CRavDvdApp::DisplayPresentationModeMenu(XBRavMenu* parent)
{
	ULONG uops;
	if(m_pPlayer == NULL)
	{
		return;
	}

//	if(S_OK == m_pPlayer->GetCurrentUOPS(&uops))
//	{
//		if(!(uops & UOP_AUDIO_STREAM_CHANGE))
//		{
//			if (lNumAudio > 1)
//			{

				XBRavMenu*	pPModeMenu;
				XBMenuProfile profile;

				float posX, posY;

				if(parent != NULL)
				{
					wsprintfW(profile.title, L"Presentation Mode");
//					profile.title[0]			= NULL;
					profile.maxitems			= NUM_SETTINGS_MENU_ITEMS;
					profile.flags				= MENU_WRAP | MENU_RIGHT;
					profile.hideparent			= false;
					profile.showbackground		= true;
					profile.showhighlight		= true;
					profile.showtextedge		= false;
					profile.topcolor			= MENU_BACK_COLOR;
					profile.bottomcolor			= MENU_BACK_COLOR;
					profile.itemcolor			= MENU_TEXT_COLOR;
					profile.seltopcolor			= MENU_HIGHLIGHT_COLOR;
					profile.selbotcolor			= MENU_HIGHLIGHT_COLOR;

					posX = parent->GetX() + parent->GetW() + 2;
					posY = parent->GetY();

					parent->Activate(false);
				}
				else // Transparent Menu over video
				{
					profile.title[0]			= NULL;
					profile.maxitems			= NUM_SETTINGS_MENU_ITEMS;
					profile.flags				= MENU_WRAP;
					profile.hideparent			= true;
					profile.showbackground		= false;
					profile.showhighlight		= false;
					profile.showtextedge		= true;
					profile.topcolor			= RAV_COLORKEY;
					profile.bottomcolor			= RAV_COLORKEY;
					profile.itemcolor			= 0xff333333;
					profile.seltopcolor			= 0xffffffff;
					profile.selbotcolor			= 0xffffffff;

					posX = 320.0f;
					posY = 100.0f;
				}

				pPModeMenu = new XBRavMenu(&profile, posX, posY, (DWORD*)NULL, parent);
//					XBRavMenu*	pSubpicMenu = new XBRavMenu(NULL, 320.0f, 100.0f,
//						lNumSubpic, MENU_WRAP, NULL);

				if (pPModeMenu)
				{
					pPModeMenu->AddItem(MITEM_ROUTINE, L"4 by 3", &PresentationModeMenuProc);
					pPModeMenu->AddItem(MITEM_ROUTINE, L"16 by 9", &PresentationModeMenuProc);
					pPModeMenu->AddItem(MITEM_ROUTINE, L"LetterBox", &PresentationModeMenuProc);
					pPModeMenu->AddItem(MITEM_ROUTINE, L"Pan and Scan", &PresentationModeMenuProc);
					pPModeMenu->AddItem(MITEM_ROUTINE, L"none", &PresentationModeMenuProc);

					pPModeMenu->curitem = (ULONG) m_pPlayer->GetDisplayMode();
					pPModeMenu->Activate();
				}
//			}
//			else
//			{
//				ResetOSD(&IDS_NOTPERMITTED, true, false, OSD_NOTPERMITTED_DISPLAY_TIME);
//			}
//		}
//		else
//		{
//			ResetOSD(&IDS_NOTPERMITTED, true, false, OSD_NOTPERMITTED_DISPLAY_TIME);
//		}
//	}
}

DWORD RegionMenuProc(DWORD cmd, XMenuItem *it)
{
	if(it == NULL)
		return -1;

	BYTE region = (BYTE) (((XBRavMenu*)(it->menu))->GetItemIndex(it));

	if(g_theApp->m_pPlayer)
	{
		g_theApp->m_bDeleteMenusOnStop = false;

		if(region == 0x00)
			g_theApp->m_pPlayer->SetSystemRegion('D', (BYTE)0x3f); // region free, set all 6 region bits
		else
			g_theApp->m_pPlayer->SetSystemRegion('D', (BYTE)~(1 << (region-1)));

		g_theApp->m_bDeleteMenusOnStop = true;
	}

//	delete ((XBRavMenu*)(it->menu));
	return MROUTINE_DIE;
}

void CRavDvdApp::DisplayRegionMenu(XBRavMenu* parent)
{
	ULONG uops;
	if(m_pPlayer == NULL)
	{
		return;
	}
//	if(S_OK == m_pPlayer->GetCurrentUOPS(&uops))
//	{
//		if(!(uops & UOP_AUDIO_STREAM_CHANGE))
//		{
//			if (lNumAudio > 1)
//			{

				XBRavMenu*	pRegionMenu;
				XBMenuProfile profile;

				float posX, posY;

				if(parent != NULL)
				{
					wsprintfW(profile.title, L"Region");
//					profile.title[0]			= NULL;
					profile.maxitems			= NUM_SETTINGS_MENU_ITEMS;
					profile.flags				= MENU_WRAP | MENU_RIGHT;
					profile.hideparent			= false;
					profile.showbackground		= true;
					profile.showhighlight		= true;
					profile.showtextedge		= false;
					profile.topcolor			= MENU_BACK_COLOR;
					profile.bottomcolor			= MENU_BACK_COLOR;
					profile.itemcolor			= MENU_TEXT_COLOR;
					profile.seltopcolor			= MENU_HIGHLIGHT_COLOR;
					profile.selbotcolor			= MENU_HIGHLIGHT_COLOR;

					posX = parent->GetX() + parent->GetW() + 2;
					posY = parent->GetY();

					parent->Activate(false);
				}
				else // Transparent Menu over video
				{
					profile.title[0]			= NULL;
					profile.maxitems			= NUM_SETTINGS_MENU_ITEMS;
					profile.flags				= MENU_WRAP;
					profile.hideparent			= true;
					profile.showbackground		= false;
					profile.showhighlight		= false;
					profile.showtextedge		= true;
					profile.topcolor			= RAV_COLORKEY;
					profile.bottomcolor			= RAV_COLORKEY;
					profile.itemcolor			= 0xff333333;
					profile.seltopcolor			= 0xffffffff;
					profile.selbotcolor			= 0xffffffff;

					posX = 320.0f;
					posY = 100.0f;
				}

				pRegionMenu = new XBRavMenu(&profile, posX, posY, (DWORD*)NULL, parent);
//					XBRavMenu*	pSubpicMenu = new XBRavMenu(NULL, 320.0f, 100.0f,
//						lNumSubpic, MENU_WRAP, NULL);

				if (pRegionMenu)
				{
					pRegionMenu->AddItem(MITEM_ROUTINE, L"Region Free", &RegionMenuProc);
					pRegionMenu->AddItem(MITEM_ROUTINE, L"Region 1", &RegionMenuProc);
					pRegionMenu->AddItem(MITEM_ROUTINE, L"Region 2", &RegionMenuProc);
					pRegionMenu->AddItem(MITEM_ROUTINE, L"Region 3", &RegionMenuProc);
					pRegionMenu->AddItem(MITEM_ROUTINE, L"Region 4", &RegionMenuProc);
					pRegionMenu->AddItem(MITEM_ROUTINE, L"Region 5", &RegionMenuProc);
					pRegionMenu->AddItem(MITEM_ROUTINE, L"Region 6", &RegionMenuProc);

					BYTE region, mask = 0x00;
					if(m_pPlayer->GetSystemRegion('D', mask) == S_OK)
					{

						if(mask == 0x00)
						{
							region = 0x00;
						}
						else
						{
							region = ~mask;
//							BYTE temp = ~mask;
//							BYTE count = 0x01;
//							region = 0x00;
//							while(temp > 0x00 && region == 0x00)
//							{
//								if(temp & 0x01)
//								{
//									region = count;
//								}
//								else
//								{
//									count++;
//									temp = temp>>1;
//								}
//							}

						}

						char buf[MAX_PATH];
						wsprintf(buf, "GetSystemRegion return mask = %x, region = %x", mask, region);
						OutputDebugString(buf);

						if(region <= 6)
						{
							pRegionMenu->curitem = (ULONG) region;
						}
					}

					pRegionMenu->Activate();
				}
//			}
//			else
//			{
//				ResetOSD(&IDS_NOTPERMITTED, true, false, OSD_NOTPERMITTED_DISPLAY_TIME);
//			}
//		}
//		else
//		{
//			ResetOSD(&IDS_NOTPERMITTED, true, false, OSD_NOTPERMITTED_DISPLAY_TIME);
//		}
//	}
}


void CRavDvdApp::DisplayDolbySettingsMenu(XBRavMenu* parent)
{
}

DWORD SpeakerOutputModeMenuProc(DWORD cmd, XMenuItem *it)
{
	if(it == NULL)
		return -1;

	WORD config = (WORD) ((XBRavMenu*)(it->menu))->GetItemIndex(it);

	if(g_theApp->m_pPlayer)
		g_theApp->m_pPlayer->SetSpeakerOutputMode( config ); // 0x0000 == stereo, 0x0001 == stereo, 0x0002 == surround.

	return MROUTINE_DIE;
}

void CRavDvdApp::DisplaySpeakerOutputModeMenu(XBRavMenu* parent)
{
	ULONG uops;
	if(m_pPlayer == NULL)
	{
		return;
	}

	XBRavMenu*	pPModeMenu;
	XBMenuProfile profile;

	float posX, posY;

	if(parent != NULL)
	{
		wsprintfW(profile.title, L"Speaker Output Mode");
		profile.maxitems			= NUM_SETTINGS_MENU_ITEMS;
		profile.flags				= MENU_WRAP | MENU_RIGHT;
		profile.hideparent			= false;
		profile.showbackground		= true;
		profile.showhighlight		= true;
		profile.showtextedge		= false;
		profile.topcolor			= MENU_BACK_COLOR;
		profile.bottomcolor			= MENU_BACK_COLOR;
		profile.itemcolor			= MENU_TEXT_COLOR;
		profile.seltopcolor			= MENU_HIGHLIGHT_COLOR;
		profile.selbotcolor			= MENU_HIGHLIGHT_COLOR;

		posX = parent->GetX() + parent->GetW() + 2;
		posY = parent->GetY();

		parent->Activate(false);
	}
	else // Transparent Menu over video
	{
		profile.title[0]			= NULL;
		profile.maxitems			= NUM_SETTINGS_MENU_ITEMS;
		profile.flags				= MENU_WRAP;
		profile.hideparent			= true;
		profile.showbackground		= false;
		profile.showhighlight		= false;
		profile.showtextedge		= true;
		profile.topcolor			= RAV_COLORKEY;
		profile.bottomcolor			= RAV_COLORKEY;
		profile.itemcolor			= 0xff333333;
		profile.seltopcolor			= 0xffffffff;
		profile.selbotcolor			= 0xffffffff;

		posX = 320.0f;
		posY = 100.0f;
	}

	pPModeMenu = new XBRavMenu(&profile, posX, posY, (DWORD*)NULL, parent);

	if (pPModeMenu)
	{
//		pPModeMenu->AddItem(MITEM_ROUTINE, L"5.1 Surround - AC3", &SpeakerOutputModeMenuProc);
//		pPModeMenu->AddItem(MITEM_ROUTINE, L"5.1 Surround - DTS", &SpeakerOutputModeMenuProc);
		pPModeMenu->AddItem(MITEM_ROUTINE, L"Stereo", &SpeakerOutputModeMenuProc);
		pPModeMenu->AddItem(MITEM_ROUTINE, L"Mono", &SpeakerOutputModeMenuProc);
		pPModeMenu->AddItem(MITEM_ROUTINE, L"Surround", &SpeakerOutputModeMenuProc);

		WORD config;
		if( !FAILED(m_pPlayer->GetSpeakerOutputMode( config )) )
		{
			pPModeMenu->curitem = (ULONG) config;
		}
		pPModeMenu->Activate();
	}
}

DWORD DigitalDTSMenuProc(DWORD cmd, XMenuItem *it)
{
	if(it == NULL)
		return -1;

	WORD curConfig = 0x0000;

	if(g_theApp->m_pPlayer)
		g_theApp->m_pPlayer->GetDigitalOutputMode( curConfig );

	curConfig &= ~(XC_AUDIO_FLAGS_ENABLE_DTS >> 16);

	WORD config = (WORD) (((XBRavMenu*)(it->menu))->GetItemIndex(it)) << 1; // 0 -> 0, 0x0001 -> 0x0002

	if(g_theApp->m_pPlayer)
		g_theApp->m_pPlayer->SetDigitalOutputMode( config | curConfig ); // 0x0000 == PCM, 0x0001 == DTS, 0x0002 == AC3


	return MROUTINE_DIE;
}

void CRavDvdApp::DisplayDigitalDTSMenu(XBRavMenu* parent)
{
	ULONG uops;
	if(m_pPlayer == NULL)
	{
		return;
	}

	XBRavMenu*	pPModeMenu;
	XBMenuProfile profile;

	float posX, posY;

	if(parent != NULL)
	{
		wsprintfW(profile.title, L"Digital DTS Mode");
		profile.maxitems			= NUM_SETTINGS_MENU_ITEMS;
		profile.flags				= MENU_WRAP | MENU_RIGHT;
		profile.hideparent			= false;
		profile.showbackground		= true;
		profile.showhighlight		= true;
		profile.showtextedge		= false;
		profile.topcolor			= MENU_BACK_COLOR;
		profile.bottomcolor			= MENU_BACK_COLOR;
		profile.itemcolor			= MENU_TEXT_COLOR;
		profile.seltopcolor			= MENU_HIGHLIGHT_COLOR;
		profile.selbotcolor			= MENU_HIGHLIGHT_COLOR;

		posX = parent->GetX() + parent->GetW() + 2;
		posY = parent->GetY();

		parent->Activate(false);
	}
	else // Transparent Menu over video
	{
		profile.title[0]			= NULL;
		profile.maxitems			= NUM_SETTINGS_MENU_ITEMS;
		profile.flags				= MENU_WRAP;
		profile.hideparent			= true;
		profile.showbackground		= false;
		profile.showhighlight		= false;
		profile.showtextedge		= true;
		profile.topcolor			= RAV_COLORKEY;
		profile.bottomcolor			= RAV_COLORKEY;
		profile.itemcolor			= 0xff333333;
		profile.seltopcolor			= 0xffffffff;
		profile.selbotcolor			= 0xffffffff;

		posX = 320.0f;
		posY = 100.0f;
	}

	pPModeMenu = new XBRavMenu(&profile, posX, posY, (DWORD*)NULL, parent);

	if (pPModeMenu)
	{
//		pPModeMenu->AddItem(MITEM_ROUTINE, L"5.1 Surround - AC3", &PresentationModeMenuProc);
//		pPModeMenu->AddItem(MITEM_ROUTINE, L"5.1 Surround - DTS", &PresentationModeMenuProc);
		pPModeMenu->AddItem(MITEM_ROUTINE, L"Disabled", &DigitalDTSMenuProc);
		pPModeMenu->AddItem(MITEM_ROUTINE, L"Enabled (Content Dependent)", &DigitalDTSMenuProc);

		WORD config;
		if( !FAILED(m_pPlayer->GetDigitalOutputMode( config )) )
		{
			pPModeMenu->curitem = ((ULONG) config & (XC_AUDIO_FLAGS_ENABLE_DTS >> 16) >> 1);
		}
		pPModeMenu->Activate();
	}
}

DWORD DigitalAC3MenuProc(DWORD cmd, XMenuItem *it)
{
	if(it == NULL)
		return -1;

	WORD curConfig = 0x0000;

	if(g_theApp->m_pPlayer)
		g_theApp->m_pPlayer->GetDigitalOutputMode( curConfig );

	curConfig &= ~(XC_AUDIO_FLAGS_ENABLE_AC3 >> 16);

	WORD config = (WORD) (((XBRavMenu*)(it->menu))->GetItemIndex(it));

	if(g_theApp->m_pPlayer)
		g_theApp->m_pPlayer->SetDigitalOutputMode( config | curConfig ); // 0x0000 == PCM, 0x0001 == AC3, 0x0002 == DTS

	return MROUTINE_DIE;
}

void CRavDvdApp::DisplayDigitalAC3Menu(XBRavMenu* parent)
{
	ULONG uops;
	if(m_pPlayer == NULL)
	{
		return;
	}

	XBRavMenu*	pPModeMenu;
	XBMenuProfile profile;

	float posX, posY;

	if(parent != NULL)
	{
		wsprintfW(profile.title, L"Digital Output On/Off");
		profile.maxitems			= NUM_SETTINGS_MENU_ITEMS;
		profile.flags				= MENU_WRAP | MENU_RIGHT;
		profile.hideparent			= false;
		profile.showbackground		= true;
		profile.showhighlight		= true;
		profile.showtextedge		= false;
		profile.topcolor			= MENU_BACK_COLOR;
		profile.bottomcolor			= MENU_BACK_COLOR;
		profile.itemcolor			= MENU_TEXT_COLOR;
		profile.seltopcolor			= MENU_HIGHLIGHT_COLOR;
		profile.selbotcolor			= MENU_HIGHLIGHT_COLOR;

		posX = parent->GetX() + parent->GetW() + 2;
		posY = parent->GetY();

		parent->Activate(false);
	}
	else // Transparent Menu over video
	{
		profile.title[0]			= NULL;
		profile.maxitems			= NUM_SETTINGS_MENU_ITEMS;
		profile.flags				= MENU_WRAP;
		profile.hideparent			= true;
		profile.showbackground		= false;
		profile.showhighlight		= false;
		profile.showtextedge		= true;
		profile.topcolor			= RAV_COLORKEY;
		profile.bottomcolor			= RAV_COLORKEY;
		profile.itemcolor			= 0xff333333;
		profile.seltopcolor			= 0xffffffff;
		profile.selbotcolor			= 0xffffffff;

		posX = 320.0f;
		posY = 100.0f;
	}

	pPModeMenu = new XBRavMenu(&profile, posX, posY, (DWORD*)NULL, parent);

	if (pPModeMenu)
	{
		pPModeMenu->AddItem(MITEM_ROUTINE, L"Disabled", &DigitalAC3MenuProc);
		pPModeMenu->AddItem(MITEM_ROUTINE, L"Enabled (Content Dependent)", &DigitalAC3MenuProc);

		WORD config;
		if( !FAILED(m_pPlayer->GetDigitalOutputMode( config )) )
		{
			pPModeMenu->curitem = ((ULONG) (config & (XC_AUDIO_FLAGS_ENABLE_AC3 >> 16)));
		}
		pPModeMenu->Activate();
	}
}

DWORD GlobalSettingsMenuProc(DWORD cmd, XMenuItem *it)
{
	if(it == NULL)
		return -1;

	XBRavMenu * pCallingMenu = (XBRavMenu*)it->menu;

	switch(pCallingMenu->GetItemIndex(it))
	{
		case 0:// Audio Stream
		{
			g_theApp->DisplayAudioStreamMenu(pCallingMenu);
			break;
		}
		case 1:// Subpicture Stream
		{
			g_theApp->DisplaySubpictureStreamMenu(pCallingMenu);
			break;
		}
		case 2:// Dolby Settings
		{
			g_theApp->DisplayDolbySettingsMenu(pCallingMenu);
			break;
		}
		case 3:// Presentation Mode
		{
			g_theApp->DisplayPresentationModeMenu(pCallingMenu);
			break;
		}
		case 4:// Region
		{
			g_theApp->DisplayRegionMenu(pCallingMenu);
			break;
		}
		case 5:// Speaker Out Mode
		{
			g_theApp->DisplaySpeakerOutputModeMenu(pCallingMenu);
			break;
		}
		case 6:// Digital DTS
		{
			g_theApp->DisplayDigitalDTSMenu(pCallingMenu);
			break;
		}
		case 7:// Digital AC3
		{
			g_theApp->DisplayDigitalAC3Menu(pCallingMenu);
			break;
		}
		default: // Kill Menu
		{
			if (g_fScaledDestination)
			{
				g_theApp->m_pPlayer->SetDestinationRect(MENU_RECT.x1, MENU_RECT.y1, 0, 0);
				g_fScaledDestination = false;
			}
			g_fSettingsMenuActive = false;
			return MROUTINE_DIE;
		}
	}

	return MROUTINE_RETURN;
}

void CRavDvdApp::DisplayGlobalSettingsMenu(XBRavMenu* parent)
{
	ULONG uops;

//	if (S_OK == m_pPlayer->GetCurrentUOPS(&uops))
//	{
//		if (!(uops & UOP_TITLE_PLAY))
//		{
			XBRavMenu*	pSettingsMenu = NULL;
			XBMenuProfile profile;


			MultiByteToWideChar(CP_ACP,0,"ST DVD Settings",-1,profile.title, MAX_PATH);
//			profile.title				= L"Dolby Audio Settings";
			profile.maxitems			= NUM_SETTINGS_MENU_ITEMS;
			profile.flags				= MENU_WRAP | MENU_RIGHT;
			profile.hideparent			= false;
			profile.showbackground		= true;
			profile.showhighlight		= true;
			profile.showtextedge		= false;
			profile.topcolor			= MENU_BACK_COLOR;
			profile.bottomcolor			= MENU_BACK_COLOR;
			profile.itemcolor			= MENU_TEXT_COLOR;
			profile.seltopcolor			= MENU_HIGHLIGHT_COLOR;
			profile.selbotcolor			= MENU_HIGHLIGHT_COLOR;

			pSettingsMenu = new XBRavMenu(&profile, 30.0f, 30.0f, (DWORD*)GlobalSettingsMenuProc);
//				pSettingsMenu = new XBRavMenu(L"Dolby Audio Settings", 30.0f, 30.0f, NUM_SETTINGS_MENU_ITEMS, MENU_WRAP | MENU_RIGHT, (DWORD*)SettingsMenuProc,
//					MENU_BACK_COLOR, MENU_BACK_COLOR, MENU_TEXT_COLOR, MENU_HIGHLIGHT_COLOR, MENU_HIGHLIGHT_COLOR);
			if (pSettingsMenu)
			{

				pSettingsMenu->AddItem(MITEM_ROUTINE, L"Audio Stream", &GlobalSettingsMenuProc);
				pSettingsMenu->AddItem(MITEM_ROUTINE, L"Subpicture Stream", &GlobalSettingsMenuProc);
				pSettingsMenu->AddItem(MITEM_ROUTINE, L"Dolby Settings", &GlobalSettingsMenuProc);
				pSettingsMenu->AddItem(MITEM_ROUTINE, L"Presentation Mode", &GlobalSettingsMenuProc);
				pSettingsMenu->AddItem(MITEM_ROUTINE, L"Region", &GlobalSettingsMenuProc);
				pSettingsMenu->AddItem(MITEM_ROUTINE, L"Speaker Output Config", &GlobalSettingsMenuProc);
				pSettingsMenu->AddItem(MITEM_ROUTINE, L"Digital DTS Mode", &GlobalSettingsMenuProc);
				pSettingsMenu->AddItem(MITEM_ROUTINE, L"Digital AC3 Mode", &GlobalSettingsMenuProc);
				pSettingsMenu->AddItem(MITEM_ROUTINE, L"Exit", &GlobalSettingsMenuProc);
				pSettingsMenu->Activate();

				g_fSettingsMenuActive = true;
				g_fScaledDestination = false;
			}
//		}
//	}
}

void CRavDvdApp::DisplayTitleMenu(XBRavMenu* parent)
{
	ULONG uops, lNumTitles = m_pPlayer->GetNumberOfTitles();

	if (S_OK == m_pPlayer->GetCurrentUOPS(&uops))
	{
		if (!(uops & UOP_TITLE_PLAY))
		{
			if (lNumTitles > 0)
			{
				XBRavMenu*	pTitleMenu = NULL;

				pTitleMenu = new XBRavMenu(NULL, 320.0f, 100.0f,
					lNumTitles, MENU_WRAP, (DWORD*)TitleMenuProc);
				if (pTitleMenu)
				{
					for(unsigned long i = 0; i < lNumTitles; i++)
					{
						// add text description
			    		WCHAR strTitleName[MAX_PATH];

						wsprintfW(strTitleName, L"Title %d", i+1);

						pTitleMenu->AddItem(MITEM_ROUTINE, strTitleName, &TitleMenuProc);
					}

					DVD_PLAYBACK_LOCATION2 loc;
					loc.TitleNum = 0;

					if (S_OK == m_pPlayer->GetCurrentLocation(&loc) && loc.TitleNum < lNumTitles)
					{
						if(loc.TitleNum == 0)
							pTitleMenu->curitem = 0;
						else
							pTitleMenu->curitem = loc.TitleNum - 1;
					}

					pTitleMenu->Activate();
				}
			}
			else
				ResetOSD(&IDS_NOTPERMITTED, true, false, OSD_NOTPERMITTED_DISPLAY_TIME);
		}
		else
			ResetOSD(&IDS_NOTPERMITTED, true, false, OSD_NOTPERMITTED_DISPLAY_TIME);
	}
	else
		ResetOSD(&IDS_NOTPERMITTED, true, false, OSD_NOTPERMITTED_DISPLAY_TIME);
}

// show subpicture menu
void CRavDvdApp::DisplaySubpictureStreamMenu(XBRavMenu* parent)
{
	ULONG lNumSubpic, lCurSubpic, uops;
	BOOL bEnabled;

	if(m_pPlayer == NULL)
	{
		return;
	}

	m_pPlayer->GetCurrentSubpicture(&lNumSubpic, &lCurSubpic, &bEnabled);

	if (S_OK == m_pPlayer->GetCurrentUOPS(&uops))
	{
		if (!(uops & UOP_SUB_PICTURE_STREAM_CHANGE))
		{
			if (lNumSubpic > 0)
			{

				XBRavMenu*	pSubpicMenu;
				XBMenuProfile profile;

				float posX, posY;

				if(parent != NULL)
				{
					wsprintfW(profile.title, L"SubPicture Selection");
					profile.maxitems			= NUM_SETTINGS_MENU_ITEMS;
					profile.flags				= MENU_WRAP | MENU_RIGHT;
					profile.hideparent			= false;
					profile.showbackground		= true;
					profile.showhighlight		= true;
					profile.showtextedge		= false;
					profile.topcolor			= MENU_BACK_COLOR;
					profile.bottomcolor			= MENU_BACK_COLOR;
					profile.itemcolor			= MENU_TEXT_COLOR;
					profile.seltopcolor			= MENU_HIGHLIGHT_COLOR;
					profile.selbotcolor			= MENU_HIGHLIGHT_COLOR;

					posX = parent->GetX() + parent->GetW() + 2;
					posY = parent->GetY();

					parent->Activate(false);
				}
				else // Transparent Menu over video
				{
					profile.title[0]			= NULL;
					profile.maxitems			= NUM_SETTINGS_MENU_ITEMS;
					profile.flags				= MENU_WRAP;
					profile.hideparent			= true;
					profile.showbackground		= false;
					profile.showhighlight		= false;
					profile.showtextedge		= true;
					profile.topcolor			= RAV_COLORKEY;
					profile.bottomcolor			= RAV_COLORKEY;
					profile.itemcolor			= 0xff333333;
					profile.seltopcolor			= 0xffffffff;
					profile.selbotcolor			= 0xffffffff;

					posX = 320.0f;
					posY = 100.0f;

				}

				pSubpicMenu = new XBRavMenu(&profile, posX, posY, (DWORD*)NULL, parent);

//					XBRavMenu*	pSubpicMenu = new XBRavMenu(NULL, 320.0f, 100.0f,
//						lNumSubpic, MENU_WRAP, NULL);

				if (pSubpicMenu)
				{
					for(unsigned long i = 0; i < lNumSubpic; i++)
					{
						// add text description
			    		char strLangName[MAX_PATH];
			    		WCHAR wStrLangName[MAX_PATH];
				    	if (S_OK == m_pPlayer->GetSubpictureLanguage(i, strLangName, MAX_PATH))
						{
							MultiByteToWideChar(CP_ACP,0,strLangName,-1,wStrLangName, MAX_PATH);
							pSubpicMenu->AddItem(MITEM_ROUTINE, wStrLangName, &SubpicMenuProc);
						}
					}

					pSubpicMenu->AddItem(MITEM_ROUTINE, L"None", &SubpicMenuProc);

					if (bEnabled)
						pSubpicMenu->curitem = lCurSubpic;
					else
						pSubpicMenu->curitem = lNumSubpic;

					pSubpicMenu->Activate();
				}


			}
			else
			{
				ResetOSD(&IDS_NOTPERMITTED, true, false, OSD_NOTPERMITTED_DISPLAY_TIME);
			}
		}
		else
		{
			ResetOSD(&IDS_NOTPERMITTED, true, false, OSD_NOTPERMITTED_DISPLAY_TIME);
		}
	}
}

// show audio menu
void CRavDvdApp::DisplayAudioStreamMenu(XBRavMenu* parent)
{
	ULONG lNumAudio, lCurAudio, uops;
	if(m_pPlayer == NULL)
	{
		return;
	}
	m_pPlayer->GetCurrentAudio(&lNumAudio, &lCurAudio);

	if(S_OK == m_pPlayer->GetCurrentUOPS(&uops))
	{
		if(!(uops & UOP_AUDIO_STREAM_CHANGE))
		{
			if (lNumAudio > 1)
			{

				XBRavMenu*	pAudioMenu;
				XBMenuProfile profile;

				float posX, posY;

				if(parent != NULL)
				{
					wsprintfW(profile.title, L"Audio Selection");
					profile.maxitems			= NUM_SETTINGS_MENU_ITEMS;
					profile.flags				= MENU_WRAP | MENU_RIGHT;
					profile.hideparent			= false;
					profile.showbackground		= true;
					profile.showhighlight		= true;
					profile.showtextedge		= false;
					profile.topcolor			= MENU_BACK_COLOR;
					profile.bottomcolor			= MENU_BACK_COLOR;
					profile.itemcolor			= MENU_TEXT_COLOR;
					profile.seltopcolor			= MENU_HIGHLIGHT_COLOR;
					profile.selbotcolor			= MENU_HIGHLIGHT_COLOR;

					posX = parent->GetX() + parent->GetW() + 2;
					posY = parent->GetY();

					parent->Activate(false);
				}
				else // Transparent Menu over video
				{
					profile.title[0]			= NULL;
					profile.maxitems			= NUM_SETTINGS_MENU_ITEMS;
					profile.flags				= MENU_WRAP;
					profile.hideparent			= true;
					profile.showbackground		= false;
					profile.showhighlight		= false;
					profile.showtextedge		= true;
					profile.topcolor			= RAV_COLORKEY;
					profile.bottomcolor			= RAV_COLORKEY;
					profile.itemcolor			= 0xff333333;
					profile.seltopcolor			= 0xffffffff;
					profile.selbotcolor			= 0xffffffff;

					posX = 320.0f;
					posY = 100.0f;

				}

				pAudioMenu = new XBRavMenu(&profile, posX, posY, (DWORD*)NULL, parent);

//					XBRavMenu*	pSubpicMenu = new XBRavMenu(NULL, 320.0f, 100.0f,
//						lNumSubpic, MENU_WRAP, NULL);

				if (pAudioMenu)
				{
					for(unsigned long i = 0; i < lNumAudio; i++)
					{
						// add text description
			    		char strLangName[MAX_PATH];
			    		WCHAR wStrLangName[MAX_PATH];

				    	if (S_OK == m_pPlayer->GetAudioLanguage(i, strLangName, MAX_PATH))
						{
							MultiByteToWideChar(CP_ACP,0,strLangName,-1,wStrLangName, MAX_PATH);
							pAudioMenu->AddItem(MITEM_ROUTINE, wStrLangName, &AudioMenuProc);

						}


					}
					pAudioMenu->curitem = lCurAudio;
					pAudioMenu->Activate();
				}
			}
			else
			{
				ResetOSD(&IDS_NOTPERMITTED, true, false, OSD_NOTPERMITTED_DISPLAY_TIME);
			}
		}
		else
		{
			ResetOSD(&IDS_NOTPERMITTED, true, false, OSD_NOTPERMITTED_DISPLAY_TIME);
		}
	}
}

