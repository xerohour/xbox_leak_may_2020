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

// RavDvd.h : main header file for the RAVDVD application
//

#if !defined
#define AFX_RAVDVD_H__7D3E038B_A4E1_11D4_B8BE_0001031A0E82__INCLUDED_

#if _MSC_VER>1000
#pragma once
#endif // _MSC_VER > 1000

//#ifndef __AFXWIN_H__
//	#error include 'stdafx.h' before including this file for PCH
//#endif

#include "DVDMediaPlayer.h"
//#include "XBUtil.h"
#include <XBApp.h>
#include "XBUtil.h"
#include "XBRavMenu.h"
#include "XBTga.h"

//-----------------------------------------------------------------------------
// Callouts for labelling the gamepad on the help screen
//-----------------------------------------------------------------------------
XBHELP_CALLOUT g_HelpCallouts[] =
{
    { XBHELP_DPAD,         XBHELP_PLACEMENT_2, (WCHAR*) (L"Menu Navigation\nFrame Fwd/Rev") },
    { XBHELP_X_BUTTON,     XBHELP_PLACEMENT_1, (WCHAR*) (L"Stepped Zoom") },
    { XBHELP_Y_BUTTON,     XBHELP_PLACEMENT_1, (WCHAR*) (L"Root Menu") },
    { XBHELP_A_BUTTON,     XBHELP_PLACEMENT_1, (WCHAR*) (L"Play/Pause/Enter") },
    { XBHELP_B_BUTTON,     XBHELP_PLACEMENT_1, (WCHAR*) (L"Stop") },
    { XBHELP_WHITE_BUTTON, XBHELP_PLACEMENT_1, (WCHAR*) (L"Display Time") },
    { XBHELP_BLACK_BUTTON, XBHELP_PLACEMENT_1, (WCHAR*) (L"Display Help") },
    { XBHELP_START_BUTTON, XBHELP_PLACEMENT_1, (WCHAR*) (L"Resume/Play") },
    { XBHELP_BACK_BUTTON, XBHELP_PLACEMENT_1, (WCHAR*) (L"[Return] from \nSubmenu") },
	{ XBHELP_LEFTSTICK, XBHELP_PLACEMENT_1, (WCHAR*) (L"Pan") },
	{ XBHELP_RIGHTSTICK, XBHELP_PLACEMENT_1, (WCHAR*) (L"Continuous Zoom") }
//	{ XBHELP_LEFT_TRIGGER, XBHELP_PLACEMENT_1, _T("Previous \nChapter") },
//	{ XBHELP_RIGHT_TRIGGER, XBHELP_PLACEMENT_1, _T("Next Chapter") }
};

#define NUM_HELP_CALLOUTS 11

#define NORMAL_HELP_TITLE	L"GAME PAD FUNCTIONS FOR DVD PLAYER"
#define SHIFT_HELP_TITLE1	L"GAME PAD FUNCTIONS FOR DVD PLAYER"
#define SHIFT_HELP_TITLE2	L"WITH SHIFT BUTTON HELD"
#define SHIFT_DEFINITION	L"(SHIFT = LEFT TRIGGER UNDERNEATH PAD)"


XBHELP_CALLOUT g_ShiftHelpCallouts[] =
{
    { XBHELP_X_BUTTON,     XBHELP_PLACEMENT_1, (WCHAR*) (L"Rev Scan") },
    { XBHELP_Y_BUTTON,     XBHELP_PLACEMENT_1, (WCHAR*) (L"Subpic Menu") },
    { XBHELP_A_BUTTON,     XBHELP_PLACEMENT_1, (WCHAR*) (L"Audio Menu") },
    { XBHELP_B_BUTTON,     XBHELP_PLACEMENT_1, (WCHAR*) (L"Fwd Scan") },
    { XBHELP_WHITE_BUTTON, XBHELP_PLACEMENT_1, (WCHAR*) (L"Angle Menu") },
    { XBHELP_BLACK_BUTTON, XBHELP_PLACEMENT_1, (WCHAR*) (L"Dolby Settings") },
    { XBHELP_BACK_BUTTON, XBHELP_PLACEMENT_1, (WCHAR*) (L"Title Menu") },
};

#define NUM_SHIFT_HELP_CALLOUTS 7



#define BACKGROUND_COLOR		0x00000000
#define	PARENTAL_LEVEL_NOCHANGE	0xffffffff

#define OSD_DISPLAY_TIME	3
#define OSD_NOTPERMITTED_DISPLAY_TIME	2
#define ZOOM_Y_INCREMENT	3
#define ZOOM_X_INCREMENT	4

#define OSD_ACTION_TOP	50
#define OSD_ACTION_LEFT	64
#define OSD_SCAN_TOP	50
#define OSD_SCAN_LEFT	500
#define OSD_TIME_TOP	25
#define OSD_TIME_LEFT	500


// Normal Slowmotion and Fast motion speed tables
const double SLOW_SCAN_VALUES[] =
{
	0.75,
	0.5,
	0.25
};

const double FAST_SCAN_VALUES[] =
{
	1.25,
	1.5,
	2.0,
	10.0,
	20.0
};

#define NUM_FAST_MOTION  (sizeof(FAST_SCAN_VALUES)/sizeof(double))
#define NUM_SLOW_MOTION  (sizeof(SLOW_SCAN_VALUES)/sizeof(double))

enum
{
	SCAN_FWD,
	SCAN_REV,
	SCAN_OFF
};

enum
{
	HELP_NORMAL,
	HELP_SHIFT,
	HELP_OFF
};

#define SYSTEM_REGION	0xfe		// byte mask for region 1



#define    US_PARENTAL_NONE    8
#define    US_PARENTAL_NC17    7
#define    US_PARENTAL_R       6
#define    US_PARENTAL_PG13    4
#define    US_PARENTAL_PG      3
#define    US_PARENTAL_G       1

#define    UNSPEC_PARENTAL_NONE         9
#define    UNSPEC_PARENTAL_ADULT        8
#define    UNSPEC_PARENTAL_THEATRICAL   6
#define    UNSPEC_PARENTAL_KIDSSAFE     1

#define MKLANG(a, b)  ((WORD) a << 8 | (BYTE) b)

#define LCID_FRENCH			MKLANG('f', 'r')
#define LCID_SPANISH		MKLANG('e', 's')
#define LCID_ENGLISH		MKLANG('e', 'n')
#define ISO3166_US			0x5553

typedef struct tagSYSTEM_DEFAULTS
{
	LCID					AudioLanguage;
	DVD_AUDIO_LANG_EXT		AudioExt;
	LCID					SubpictureLanguage;
	DVD_SUBPICTURE_LANG_EXT	SubpictureExt;
	LCID					MenuLanguage;
	LCID					ParentalCountry;
	DWORD					ParentalLevel;
	DWORD					SubpictureState;
}	SYSTEM_DEFAULTS;

typedef struct tagFREEZE_DATA
{
	BYTE*					pData;
	DWORD					dwSize;
} FREEZE_DATA;


enum ABREPEAT_ENUM
{
	ABREPEAT_OFF,
	ABREPEAT_A_SET,
	ABREPEAT_A_AND_B_SET
};

typedef struct tagAB_REPEAT_DATA
{
	ABREPEAT_ENUM			fABRepeatState;
	FREEZE_DATA				FreezeDataA;
	DVD_PLAYBACK_LOCATION2	LocA;
	DVD_PLAYBACK_LOCATION2	LocB;
} AB_REPEAT_DATA;

typedef struct tagBOOKMARK_DATA
{
	FREEZE_DATA				FreezeData;
	DVD_PLAYBACK_LOCATION2	Loc;
} BOOKMARK_DATA;

const SYSTEM_DEFAULTS SystemDefaults =
{
	LCID_ENGLISH,				// Audio
//	LCID_FRENCH,				// Audio
	(DVD_AUDIO_LANG_EXT) 0xffff,	// Audio extension
	LANG_NEUTRAL,				// Subpic
//	LCID_SPANISH,				// Subpic
	DVD_SP_EXT_Caption_Big,		// Subpic extension
	LCID_ENGLISH,				// Menu
//	LCID_FRENCH,				// Menu
	ISO3166_US,					// Parental Country
	US_PARENTAL_G,				// Parental Level
	TRUE						// Subpicture state (TRUE = On)
};

//const D3DRECT MENU_RECT = { 320, 220, 620, 460 };
D3DRECT MENU_RECT = { 320, 220, 620, 460 };

/////////////////////////////////////////////////////////////////////////////
// CRavDvdApp:
// See RavDvd.cpp for the implementation of this class
//

//class CRavDvdApp : public CWinApp//, CXBApplication
class CRavDvdApp : public CXBApplication
{
public:
	CRavDvdApp();
	virtual ~CRavDvdApp();
    virtual HRESULT Initialize();
    virtual HRESULT Render();
    HRESULT Create();

	vector <BOOKMARK_DATA> m_Bookmarks;

	DVDMediaPlayer*			m_pPlayer;
	DWORD		m_dwNewParentalLevel;  // parental level change event must be handled asyncronously on a different thread
	bool		m_fParentalRightsTooLow;
    void ResetScanSpeed(bool bClearText, bool bClearData);
    void ResetZoomVariables();
	void ResetMenus();
	void ResetABRepeat();

	bool m_bDeleteMenusOnStop;

	void DisplayGlobalSettingsMenu(XBRavMenu* parent = NULL);
	void DisplayTitleMenu(XBRavMenu* parent = NULL);
	void DisplayAudioStreamMenu(XBRavMenu* parent = NULL);
	void DisplaySubpictureStreamMenu(XBRavMenu* parent = NULL);
	void DisplayDolbySettingsMenu(XBRavMenu* parent = NULL);
	void DisplayPresentationModeMenu(XBRavMenu* parent = NULL);
	void DisplayRegionMenu(XBRavMenu* parent = NULL);
	void DisplayDigitalDTSMenu(XBRavMenu* parent = NULL);
	void DisplayDigitalAC3Menu(XBRavMenu* parent = NULL);
	void DisplaySpeakerOutputModeMenu(XBRavMenu* parent = NULL);

private:
	void CreateMonitor();
	void ClearBookmarks();
	bool InitPlayer();
	void ProcessGamePadInput(DWORD dwInput);
	void HandleParentalLevelChange();
	void ResetCenterPoint();

	void ResetOSD(WCHAR* pOSDText, bool bShowOSDText, bool bShowScanSpeed, int nTimerIncrement);
	void DrawOsd();

    // The game controller
	LONGLONG	m_DiskID;
    XBGAMEPAD*  m_GamePads;
    CXBFont     m_Font;
	CXBFont		m_MenuFont;
	DWORD		m_dwMenuCommand;
    CXBHelp     m_Help;
    CXBSplash   m_Splash;
	CXBTga		m_NotPermitted;
	CXBTga		m_BookmarkOSD;

	char m_chLastVolName[MAX_PATH];
//	DVD_PLAYBACK_LOCATION2 m_LastDvdLoc;
	FREEZE_DATA m_LastDvdLoc;
	AB_REPEAT_DATA	m_ABRepeat;

    BOOL        m_bDrawHelp;
	BOOL		m_bShowOSDText;
	BOOL		m_bShowScanSpeed;
	double		m_dScanSpeed;
	int			m_nScanIndex;
	int			m_nScanDir;
	bool		m_bScanCycleUp;			// are we cycling up or down thru scan speeds
	WCHAR 		m_pOSDTextBuf[256];
	WCHAR		m_pScanSpeedTextBuf[256];
	BOOL		m_bDisplayTime;			// show DVD elapsed time/chapter info overtop video
	float		m_fOSDDisplayTime;
	bool		m_bZoomed;
	int			m_nZoomX, m_nZoomY, m_nZoomW, m_nZoomH;
	POINT		m_CenterPoint;
	float		m_fLThumbStickX, m_fLThumbStickY, m_fRThumbStickX, m_fRThumbStickY;
	bool		m_bLThumbStickOn, m_bRThumbStickOn;
	bool		m_bEnableScanAudio;
	bool		m_fPlayerIsOpen;
	int			m_nHelpScreen;

	UINT		m_uMaxWidth, m_uMaxHeight;
	UINT		m_uWideScreen;
	UINT		m_uProgressive;
};

#endif // !defined(AFX_RAVDVD_H__7D3E038B_A4E1_11D4_B8BE_0001031A0E82__INCLUDED_)
