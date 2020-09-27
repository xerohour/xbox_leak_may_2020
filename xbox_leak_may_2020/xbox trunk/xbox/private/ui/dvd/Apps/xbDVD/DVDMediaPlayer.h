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

//////////////////////////////////////////////////////////////////////////////
//
//  $Workfile: DVDMediaPlayer.h $
//
//  Copyright (c) 1999  Quadrant International, Inc.
//  All Rights Reserved.
//
//////////////////////////////////////////////////////////////////////////////
//
// $Archive: /Tproj/Apps/xbDVD/DVDMediaPlayer.h $
// $Author: Thermanson $
// $Modtime: 7/02/01 3:26p $
// $Date: 7/03/01 5:43p $
// $Revision: 26 $
//
////////////////////////////////////$NoKeywords: $////////////////////////////


#ifndef AFX_DVDMEDIAPLAYER_H__DD1FA08A_135D_11D3_810A_00A0CC23CB74__INCLUDED
#define AFX_DVDMEDIAPLAYER_H__DD1FA08A_135D_11D3_810A_00A0CC23CB74__INCLUDED

#pragma warning(disable : 4244)
#pragma warning(disable : 4100)
#pragma warning(disable : 4211)
#pragma warning(disable : 4710)
#pragma warning(disable : 4018)

#include "dvdevcod.h" // dvd event codes
#include "evcode.h" // dvd event codes
#include "QProperties.h"
#include <XBApp.h>
//#include <XBFile.h>
#include <XBFont.h>
#include "dvdpld32.h"

#define DNE_DVD_RAVISENTTIMEOUT			23
#define RAV_STOP_TO_MENU				0
#define VFW_E_DVD_RAVISENTTIMEOUT		((HRESULT)0xe0000001L)
#define REGISTRY_KEY "Software\\Ravisent\\VDRDVD"
#define C_NORMAL_PLAYBACK_SPEED 1000
#define	QIM_VID_WIDTH_DEF			640
#define QIM_VID_HEIGHT_DEF			480
#define C_MAX_STREAMS	8
#define G_C_MAX_VOLUME			100
#define G_C_MAX_VDR_VOLUME		10000
#define G_C_VDR_VOLUME_SCALE	G_C_MAX_VOLUME
#define MAX_VOLUME(a,b) (((a)>(b))?(a):(b))

#ifndef WM_USER
#define WM_USER                         0x0400
#endif //WM_USER

#define WM_RAVISENT_EVENT	WM_USER + 0x010
#define WM_RAVISENT_ERROR	WM_USER + 0x011
#define WM_RAVISENT_GAMEPAD	WM_USER + 0x012
#define EC_DVD_DISPLAY_MODE_CHANGE                     (EC_DVDBASE + 0x20)


typedef void (WINAPI* PEVENT_CALLBACK)(DWORD dwEvent,  DWORD dwInfo);

enum RavisentGamePadEvent {
	RAV_NAVIGATE_UP,
	RAV_NAVIGATE_DOWN,
	RAV_NAVIGATE_LEFT,
	RAV_NAVIGATE_RIGHT,
	RAV_ENTER,
	RAV_NEXT,
	RAV_BACK,
	RAV_PLAY,
	RAV_STOP,
	RAV_PAUSE,
	RAV_MENU,
	RAV_FSCAN,
	RAV_BSCAN,
	RAV_EXIT};


enum PictureControlTyp { E_CTL_Brightness, E_CTL_Contrast,
                         E_CTL_Saturation, E_CTL_Hue,
                         E_CTL_Gamma, E_CTL_Sharpness,
                         E_CTL_RedBrightness, E_CTL_GreenBrightness,
                         E_CTL_BlueBrightness,E_CTL_COUNT};
struct CSize {
	ULONG cx;
	ULONG cy;
};

struct CPoint {
	ULONG x;
	ULONG y;
};

typedef char* BSTR;

typedef LONG CWnd;

#define WM_SETFOCUS                     0x0007
#define WM_KILLFOCUS                    0x0008
#define WM_ENABLE                       0x000A
#define WM_SETREDRAW                    0x000B
#define WM_SETTEXT                      0x000C
#define WM_GETTEXT                      0x000D
#define WM_GETTEXTLENGTH                0x000E
#define WM_PAINT                        0x000F
#define WM_CLOSE                        0x0010
#ifndef _WIN32_WCE
#define WM_QUERYENDSESSION              0x0011
#define WM_QUERYOPEN                    0x0013
#define WM_ENDSESSION                   0x0016
#endif
#define WM_QUIT                         0x0012
#define WM_ERASEBKGND                   0x0014
#define WM_SYSCOLORCHANGE               0x0015
#define WM_SHOWWINDOW                   0x0018
#define WM_WININICHANGE                 0x001A
#if WINVER>=0x0400
#define WM_SETTINGCHANGE                WM_WININICHANGE
#endif /* WINVER >= 0x0400 */

#define MB_OK                       0x00000000L
#if WINVER>=0x0400
#define WM_APP                          0x8000
#endif /* WINVER >= 0x0400 */
#define WM_NCCREATE                     0x0081
#define WM_NCDESTROY                    0x0082
#define WM_NCCALCSIZE                   0x0083
#define WM_NCHITTEST                    0x0084
#define WM_NCPAINT                      0x0085
#define WM_NCACTIVATE                   0x0086
#define WM_GETDLGCODE                   0x0087
#ifndef _WIN32_WCE
#define WM_SYNCPAINT                    0x0088
#endif
#define WM_NCMOUSEMOVE                  0x00A0
#define WM_NCLBUTTONDOWN                0x00A1
#define WM_NCLBUTTONUP                  0x00A2
#define WM_NCLBUTTONDBLCLK              0x00A3
#define WM_NCRBUTTONDOWN                0x00A4
#define WM_NCRBUTTONUP                  0x00A5
#define WM_NCRBUTTONDBLCLK              0x00A6
#define WM_NCMBUTTONDOWN                0x00A7
#define WM_NCMBUTTONUP                  0x00A8
#define WM_NCMBUTTONDBLCLK              0x00A9


#if _WIN32_WINNT>=0x0500
#define WM_NCXBUTTONDOWN                0x00AB
#define WM_NCXBUTTONUP                  0x00AC
#define WM_NCXBUTTONDBLCLK              0x00AD
#endif /* _WIN32_WINNT >= 0x0500 */


#define WM_KEYFIRST                     0x0100
#define WM_KEYDOWN                      0x0100
#define WM_KEYUP                        0x0101
#define WM_CHAR                         0x0102
#define WM_DEADCHAR                     0x0103
#define WM_SYSKEYDOWN                   0x0104
#define WM_SYSKEYUP                     0x0105
#define WM_SYSCHAR                      0x0106
#define WM_SYSDEADCHAR                  0x0107
#define WM_KEYLAST                      0x0108

#if WINVER>=0x0400
#define WM_IME_STARTCOMPOSITION         0x010D
#define WM_IME_ENDCOMPOSITION           0x010E
#define WM_IME_COMPOSITION              0x010F
#define WM_IME_KEYLAST                  0x010F
#endif /* WINVER >= 0x0400 */

#define WM_INITDIALOG                   0x0110
#define WM_COMMAND                      0x0111
#define WM_SYSCOMMAND                   0x0112
#define WM_TIMER                        0x0113
#define WM_HSCROLL                      0x0114
#define WM_VSCROLL                      0x0115
#define WM_INITMENU                     0x0116
#define WM_INITMENUPOPUP                0x0117
#define WM_MENUSELECT                   0x011F
#define WM_MENUCHAR                     0x0120
#define WM_ENTERIDLE                    0x0121

/*
 * Virtual Keys, Standard Set
 */
#define VK_LBUTTON        0x01
#define VK_RBUTTON        0x02
#define VK_CANCEL         0x03
#define VK_MBUTTON        0x04    /* NOT contiguous with L & RBUTTON */

#if _WIN32_WINNT>=0x0500
#define VK_XBUTTON1       0x05    /* NOT contiguous with L & RBUTTON */
#define VK_XBUTTON2       0x06    /* NOT contiguous with L & RBUTTON */
#endif /* _WIN32_WINNT >= 0x0500 */

/*
 * 0x07 : unassigned
 */

#define VK_BACK           0x08
#define VK_TAB            0x09

/*
 * 0x0A - 0x0B : reserved
 */

#define VK_CLEAR          0x0C
#define VK_RETURN         0x0D

#define VK_SHIFT          0x10
#define VK_CONTROL        0x11
#define VK_MENU           0x12
#define VK_PAUSE          0x13
#define VK_CAPITAL        0x14

#define VK_KANA           0x15
#define VK_HANGEUL        0x15  /* old name - should be here for compatibility */
#define VK_HANGUL         0x15
#define VK_JUNJA          0x17
#define VK_FINAL          0x18
#define VK_HANJA          0x19
#define VK_KANJI          0x19

#define VK_ESCAPE         0x1B

#define VK_CONVERT        0x1C
#define VK_NONCONVERT     0x1D
#define VK_ACCEPT         0x1E
#define VK_MODECHANGE     0x1F

#define VK_SPACE          0x20
#define VK_PRIOR          0x21
#define VK_NEXT           0x22
#define VK_END            0x23
#define VK_HOME           0x24
#define VK_LEFT           0x25
#define VK_UP             0x26
#define VK_RIGHT          0x27
#define VK_DOWN           0x28
#define VK_SELECT         0x29
#define VK_PRINT          0x2A
#define VK_EXECUTE        0x2B
#define VK_SNAPSHOT       0x2C
#define VK_INSERT         0x2D
#define VK_DELETE         0x2E
#define VK_HELP           0x2F


typedef struct tagMSG {
    HWND        hwnd;
    UINT        message;
    WPARAM      wParam;
    LPARAM      lParam;
    DWORD       time;
    POINT       pt;
#ifdef _MAC
    DWORD       lPrivate;
#endif
} MSG, *PMSG, NEAR *NPMSG, FAR *LPMSG;

typedef
enum tagDVD_DOMAIN
    {	DVD_DOMAIN_FirstPlay	= 1,
	DVD_DOMAIN_VideoManagerMenu	= DVD_DOMAIN_FirstPlay + 1,
	DVD_DOMAIN_VideoTitleSetMenu	= DVD_DOMAIN_VideoManagerMenu + 1,
	DVD_DOMAIN_Title	= DVD_DOMAIN_VideoTitleSetMenu + 1,
	DVD_DOMAIN_Stop	= DVD_DOMAIN_Title + 1
    } 	DVD_DOMAIN;

typedef
enum tagDVD_MENU_ID
    {	DVD_MENU_Title	= 2,
	DVD_MENU_Root	= 3,
	DVD_MENU_Subpicture	= 4,
	DVD_MENU_Audio	= 5,
	DVD_MENU_Angle	= 6,
	DVD_MENU_Chapter	= 7
    } 	DVD_MENU_ID;

typedef
enum tagDVD_DISC_SIDE
    {	DVD_SIDE_A	= 1,
	DVD_SIDE_B	= 2
    } 	DVD_DISC_SIDE;

typedef
enum tagDVD_PREFERRED_DISPLAY_MODE
    {	DISPLAY_CONTENT_DEFAULT	= 0,
	DISPLAY_16x9	= 1,
	DISPLAY_4x3_PANSCAN_PREFERRED	= 2,
	DISPLAY_4x3_LETTERBOX_PREFERRED	= 3
    } 	DVD_PREFERRED_DISPLAY_MODE;

typedef WORD DVD_REGISTER;

typedef DVD_REGISTER GPRMARRAY[ 16 ];

typedef DVD_REGISTER SPRMARRAY[ 24 ];

typedef struct tagDVD_ATR
    {
    ULONG ulCAT;
    BYTE pbATRI[ 768 ];
    } 	DVD_ATR;

typedef BYTE DVD_VideoATR[ 2 ];

typedef BYTE DVD_AudioATR[ 8 ];

typedef BYTE DVD_SubpictureATR[ 6 ];

typedef
enum tagDVD_FRAMERATE
    {	DVD_FPS_25	= 1,
	DVD_FPS_30NonDrop	= 3
    } 	DVD_FRAMERATE;

typedef struct tagDVD_TIMECODE
{
   ULONG Hours1    :4; // Hours
   ULONG Hours10  :4; // Tens of Hours

   ULONG Minutes1  :4; // Minutes
   ULONG Minutes10:4; // Tens of Minutes

   ULONG Seconds1  :4; // Seconds
   ULONG Seconds10:4; // Tens of Seconds

   ULONG Frames1   :4; // Frames
   ULONG Frames10 :2; // Tens of Frames

   ULONG FrameRateCode: 2; // use DVD_FRAMERATE to indicate frames/sec and drop/non-drop
} DVD_TIMECODE;
typedef
enum tagDVD_TIMECODE_FLAGS
    {	DVD_TC_FLAG_25fps	= 0x1,
	DVD_TC_FLAG_30fps	= 0x2,
	DVD_TC_FLAG_DropFrame	= 0x4,
	DVD_TC_FLAG_Interpolated	= 0x8
    } 	DVD_TIMECODE_FLAGS;

typedef struct tagDVD_HMSF_TIMECODE
    {
    BYTE bHours;
    BYTE bMinutes;
    BYTE bSeconds;
    BYTE bFrames;
    } 	DVD_HMSF_TIMECODE;

typedef struct tagDVD_PLAYBACK_LOCATION2
    {
    ULONG TitleNum;
    ULONG ChapterNum;
    DVD_HMSF_TIMECODE TimeCode;
    ULONG TimeCodeFlags;
    } 	DVD_PLAYBACK_LOCATION2;

typedef struct tagDVD_PLAYBACK_LOCATION
    {
    ULONG TitleNum;
    ULONG ChapterNum;
    ULONG TimeCode;
    } 	DVD_PLAYBACK_LOCATION;

typedef DWORD VALID_UOP_SOMTHING_OR_OTHER;

typedef /* [public] */
enum __MIDL___MIDL_itf_strmif_0351_0001
    {	UOP_FLAG_Play_Title_Or_AtTime	= 0x1,
	UOP_FLAG_Play_Chapter	= 0x2,
	UOP_FLAG_Play_Title	= 0x4,
	UOP_FLAG_Stop	= 0x8,
	UOP_FLAG_ReturnFromSubMenu	= 0x10,
	UOP_FLAG_Play_Chapter_Or_AtTime	= 0x20,
	UOP_FLAG_PlayPrev_Or_Replay_Chapter	= 0x40,
	UOP_FLAG_PlayNext_Chapter	= 0x80,
	UOP_FLAG_Play_Forwards	= 0x100,
	UOP_FLAG_Play_Backwards	= 0x200,
	UOP_FLAG_ShowMenu_Title	= 0x400,
	UOP_FLAG_ShowMenu_Root	= 0x800,
	UOP_FLAG_ShowMenu_SubPic	= 0x1000,
	UOP_FLAG_ShowMenu_Audio	= 0x2000,
	UOP_FLAG_ShowMenu_Angle	= 0x4000,
	UOP_FLAG_ShowMenu_Chapter	= 0x8000,
	UOP_FLAG_Resume	= 0x10000,
	UOP_FLAG_Select_Or_Activate_Button	= 0x20000,
	UOP_FLAG_Still_Off	= 0x40000,
	UOP_FLAG_Pause_On	= 0x80000,
	UOP_FLAG_Select_Audio_Stream	= 0x100000,
	UOP_FLAG_Select_SubPic_Stream	= 0x200000,
	UOP_FLAG_Select_Angle	= 0x400000,
	UOP_FLAG_Select_Karaoke_Audio_Presentation_Mode	= 0x800000,
	UOP_FLAG_Select_Video_Mode_Preference	= 0x1000000
    } 	VALID_UOP_FLAG;

typedef /* [public] */
enum __MIDL___MIDL_itf_strmif_0351_0002
    {	DVD_CMD_FLAG_None	= 0,
	DVD_CMD_FLAG_Flush	= 0x1,
	DVD_CMD_FLAG_SendEvents	= 0x2,
	DVD_CMD_FLAG_Block	= 0x4,
	DVD_CMD_FLAG_StartWhenRendered	= 0x8,
	DVD_CMD_FLAG_EndAfterRendered	= 0x10
    } 	DVD_CMD_FLAGS;

typedef /* [public][public] */
enum __MIDL___MIDL_itf_strmif_0351_0003
    {	DVD_ResetOnStop	= 1,
	DVD_NotifyParentalLevelChange	= 2,
	DVD_HMSF_TimeCodeEvents	= 3
    } 	DVD_OPTION_FLAG;

typedef /* [public][public] */
enum __MIDL___MIDL_itf_strmif_0351_0004
    {	DVD_Relative_Upper	= 1,
	DVD_Relative_Lower	= 2,
	DVD_Relative_Left	= 3,
	DVD_Relative_Right	= 4
    } 	DVD_RELATIVE_BUTTON;

typedef
enum tagDVD_PARENTAL_LEVEL
    {	DVD_PARENTAL_LEVEL_8	= 0x8000,
	DVD_PARENTAL_LEVEL_7	= 0x4000,
	DVD_PARENTAL_LEVEL_6	= 0x2000,
	DVD_PARENTAL_LEVEL_5	= 0x1000,
	DVD_PARENTAL_LEVEL_4	= 0x800,
	DVD_PARENTAL_LEVEL_3	= 0x400,
	DVD_PARENTAL_LEVEL_2	= 0x200,
	DVD_PARENTAL_LEVEL_1	= 0x100
    } 	DVD_PARENTAL_LEVEL;

typedef
enum tagDVD_AUDIO_LANG_EXT
    {	DVD_AUD_EXT_NotSpecified	= 0,
	DVD_AUD_EXT_Captions	= 1,
	DVD_AUD_EXT_VisuallyImpaired	= 2,
	DVD_AUD_EXT_DirectorComments1	= 3,
	DVD_AUD_EXT_DirectorComments2	= 4
    } 	DVD_AUDIO_LANG_EXT;

typedef
enum tagDVD_SUBPICTURE_LANG_EXT
    {	DVD_SP_EXT_NotSpecified	= 0,
	DVD_SP_EXT_Caption_Normal	= 1,
	DVD_SP_EXT_Caption_Big	= 2,
	DVD_SP_EXT_Caption_Children	= 3,
	DVD_SP_EXT_CC_Normal	= 5,
	DVD_SP_EXT_CC_Big	= 6,
	DVD_SP_EXT_CC_Children	= 7,
	DVD_SP_EXT_Forced	= 9,
	DVD_SP_EXT_DirectorComments_Normal	= 13,
	DVD_SP_EXT_DirectorComments_Big	= 14,
	DVD_SP_EXT_DirectorComments_Children	= 15
    } 	DVD_SUBPICTURE_LANG_EXT;

typedef
enum tagDVD_AUDIO_APPMODE
    {	DVD_AudioMode_None	= 0,
	DVD_AudioMode_Karaoke	= 1,
	DVD_AudioMode_Surround	= 2,
	DVD_AudioMode_Other	= 3
    } 	DVD_AUDIO_APPMODE;

typedef
enum tagDVD_AUDIO_FORMAT
    {	DVD_AudioFormat_AC3	= 0,
	DVD_AudioFormat_MPEG1	= 1,
	DVD_AudioFormat_MPEG1_DRC	= 2,
	DVD_AudioFormat_MPEG2	= 3,
	DVD_AudioFormat_MPEG2_DRC	= 4,
	DVD_AudioFormat_LPCM	= 5,
	DVD_AudioFormat_DTS	= 6,
	DVD_AudioFormat_SDDS	= 7,
	DVD_AudioFormat_Other	= 8
    } 	DVD_AUDIO_FORMAT;

typedef
enum tagDVD_KARAOKE_DOWNMIX
    {	DVD_Mix_0to0	= 0x1,
	DVD_Mix_1to0	= 0x2,
	DVD_Mix_2to0	= 0x4,
	DVD_Mix_3to0	= 0x8,
	DVD_Mix_4to0	= 0x10,
	DVD_Mix_Lto0	= 0x20,
	DVD_Mix_Rto0	= 0x40,
	DVD_Mix_0to1	= 0x100,
	DVD_Mix_1to1	= 0x200,
	DVD_Mix_2to1	= 0x400,
	DVD_Mix_3to1	= 0x800,
	DVD_Mix_4to1	= 0x1000,
	DVD_Mix_Lto1	= 0x2000,
	DVD_Mix_Rto1	= 0x4000
    } 	DVD_KARAOKE_DOWNMIX;

typedef struct tagDVD_AudioAttributes
    {
    DVD_AUDIO_APPMODE AppMode;
    BYTE AppModeData;
    DVD_AUDIO_FORMAT AudioFormat;
    LCID Language;
    DVD_AUDIO_LANG_EXT LanguageExtension;
    BOOL fHasMultichannelInfo;
    DWORD dwFrequency;
    BYTE bQuantization;
    BYTE bNumberOfChannels;
    DWORD dwReserved[ 2 ];
    } 	DVD_AudioAttributes;

typedef struct tagDVD_MUA_MixingInfo
    {
    BOOL fMixTo0;
    BOOL fMixTo1;
    BOOL fMix0InPhase;
    BOOL fMix1InPhase;
    DWORD dwSpeakerPosition;
    } 	DVD_MUA_MixingInfo;

typedef struct tagDVD_MUA_Coeff
    {
    double log2_alpha;
    double log2_beta;
    } 	DVD_MUA_Coeff;

typedef struct tagDVD_MultichannelAudioAttributes
    {
    DVD_MUA_MixingInfo Info[ 8 ];
    DVD_MUA_Coeff Coeff[ 8 ];
    } 	DVD_MultichannelAudioAttributes;

typedef
enum tagDVD_KARAOKE_CONTENTS
    {	DVD_Karaoke_GuideVocal1	= 0x1,
	DVD_Karaoke_GuideVocal2	= 0x2,
	DVD_Karaoke_GuideMelody1	= 0x4,
	DVD_Karaoke_GuideMelody2	= 0x8,
	DVD_Karaoke_GuideMelodyA	= 0x10,
	DVD_Karaoke_GuideMelodyB	= 0x20,
	DVD_Karaoke_SoundEffectA	= 0x40,
	DVD_Karaoke_SoundEffectB	= 0x80
    } 	DVD_KARAOKE_CONTENTS;

typedef
enum tagDVD_KARAOKE_ASSIGNMENT
    {	DVD_Assignment_reserved0	= 0,
	DVD_Assignment_reserved1	= 1,
	DVD_Assignment_LR	= 2,
	DVD_Assignment_LRM	= 3,
	DVD_Assignment_LR1	= 4,
	DVD_Assignment_LRM1	= 5,
	DVD_Assignment_LR12	= 6,
	DVD_Assignment_LRM12	= 7
    } 	DVD_KARAOKE_ASSIGNMENT;

typedef struct tagDVD_KaraokeAttributes
    {
    BYTE bVersion;
    BOOL fMasterOfCeremoniesInGuideVocal1;
    BOOL fDuet;
    DVD_KARAOKE_ASSIGNMENT ChannelAssignment;
    WORD wChannelContents[ 8 ];
    } 	DVD_KaraokeAttributes;

typedef
enum tagDVD_VIDEO_COMPRESSION
    {	DVD_VideoCompression_Other	= 0,
	DVD_VideoCompression_MPEG1	= 1,
	DVD_VideoCompression_MPEG2	= 2
    } 	DVD_VIDEO_COMPRESSION;

typedef struct tagDVD_VideoAttributes
    {
    BOOL fPanscanPermitted;
    BOOL fLetterboxPermitted;
    ULONG ulAspectX;
    ULONG ulAspectY;
    ULONG ulFrameRate;
    ULONG ulFrameHeight;
    DVD_VIDEO_COMPRESSION Compression;
    BOOL fLine21Field1InGOP;
    BOOL fLine21Field2InGOP;
    ULONG ulSourceResolutionX;
    ULONG ulSourceResolutionY;
    BOOL fIsSourceLetterboxed;
    BOOL fIsFilmMode;
    } 	DVD_VideoAttributes;

typedef
enum tagDVD_SUBPICTURE_TYPE
    {	DVD_SPType_NotSpecified	= 0,
	DVD_SPType_Language	= 1,
	DVD_SPType_Other	= 2
    } 	DVD_SUBPICTURE_TYPE;

typedef
enum tagDVD_SUBPICTURE_CODING
    {	DVD_SPCoding_RunLength	= 0,
	DVD_SPCoding_Extended	= 1,
	DVD_SPCoding_Other	= 2
    } 	DVD_SUBPICTURE_CODING;

typedef struct tagDVD_SubpictureAttributes
    {
    DVD_SUBPICTURE_TYPE Type;
    DVD_SUBPICTURE_CODING CodingMode;
    LCID Language;
    DVD_SUBPICTURE_LANG_EXT LanguageExtension;
    } 	DVD_SubpictureAttributes;

typedef
enum tagDVD_TITLE_APPMODE
    {	DVD_AppMode_Not_Specified	= 0,
	DVD_AppMode_Karaoke	= 1,
	DVD_AppMode_Other	= 3
    } 	DVD_TITLE_APPMODE;

typedef struct tagDVD_TitleMainAttributes
    {
    DVD_TITLE_APPMODE AppMode;
    DVD_VideoAttributes VideoAttributes;
    ULONG ulNumberOfAudioStreams;
    DVD_AudioAttributes AudioAttributes[ 8 ];
    DVD_MultichannelAudioAttributes MultichannelAudioAttributes[ 8 ];
    ULONG ulNumberOfSubpictureStreams;
    DVD_SubpictureAttributes SubpictureAttributes[ 32 ];
    } 	DVD_TitleAttributes;

typedef struct tagDVD_MenuAttributes
    {
    BOOL fCompatibleRegion[ 8 ];
    DVD_VideoAttributes VideoAttributes;
    BOOL fAudioPresent;
    DVD_AudioAttributes AudioAttributes;
    BOOL fSubpicturePresent;
    DVD_SubpictureAttributes SubpictureAttributes;
    } 	DVD_MenuAttributes;


enum PictureDisplayPresentationMode
	{
	PDPM_FULLSIZE,
	PDPM_LETTERBOXED,
	PDPM_PANSCAN,
	PDPM_FULLSIZE16by9
	};


/////////////////////////////////////////////////////////////////////////////
// DVDMediaPlayer
class DVDMediaPlayer
	{
private:
	HWND m_hParentWindow;
	DisplayPresentationMode m_iDisplayMode;
	HINSTANCE		m_appInstance;
	CSize			m_currentRatio;
	CSize			m_newRatio;
	bool			m_bChangeRatio;
	ExtendedPlayerState m_eps;

	BSTR			m_bstrWindowCaption;
	int				m_iWindowLeft, m_iWindowTop, m_iWindowWidth, m_iWindowHeight;
	long			m_lWindowStyle, m_lWindowStyleEx;
	BSTR			m_bstrMediaString;
	BSTR			m_bstrFullMediaString;
	BOOL			m_bFSInitMode;
	BOOL			m_bFullScreenMode;
	RECT			m_rVideoRect;
	ULONG			m_ulCurrentSubpic;
	BOOL			m_bSubPicOn;
	ULONG			m_ulCurrentAudio;
	ULONG			m_ulCurrentAngle;
	WORD			m_wCurrentChapter;
	WORD			m_wCurrentTitle;
	ULONG			m_ulPreferredVideo;
	BOOL			m_bTestMode;
	RECT			m_rWndRect;
	WORD			m_wVPEColorControlFlags;
	WORD			m_wDefPicControls[E_CTL_COUNT];
  	long            m_lCurrentVolume;
	long			m_lMode;
	long			m_lCurrentDisplayMode;
	int	            m_iHasSPDIFSupport;
	BOOL            m_bCheckedForPassthrough;
	DWORD           m_dwCurrentUOPs;
	DVDDiskPlayer	m_ddpThePlayer;
	UnitSet			m_Units;
	VDRHandle		m_hVdrDriver;


	LCID					m_PreferredAudioLanguage;
	DVD_AUDIO_LANG_EXT		m_PreferredAudioExt;
	LCID					m_PreferredSubpicLanguage;
	DVD_SUBPICTURE_LANG_EXT	m_PreferredSubpicExt;




	BOOL			m_bAcceptParentalLevelChange;

	double m_dScanSpeed;       // Speed used during scan (2.0X and greater)
	double m_dPlaySpeed;       // Speed used during playback (Less than 2.0X)
	BOOL   m_bForwardPlayback; // Global so callback function can change its value

public:
	static HWND m_eventWindow;
	static PEVENT_CALLBACK  m_pEventCallback;



	DVDMediaPlayer(HINSTANCE hinst);//{} // HWND to parent window to recieve video, and HINSTANCE to the app.
	virtual ~DVDMediaPlayer(void);//{};
	void Shutdown();//{};
	HRESULT put_ParentWindow(HWND win){return S_OK;};
	BOOL CalcVideoSize(CSize sizeMonitor, CSize& sizeVideo);//{return S_OK;};
	BOOL CalcVideoPos(CSize sizeMonitor, CSize sizeVideo, CPoint& pntVideo);//{return S_OK;};
//	void UpdateDisplay(bool forceUpdate = false, HWND parent = NULL){};
	void UpdateDisplay(bool forceUpdate = false);
//	int PreTranslateMessage(MSG* pMsg){};
	void EventHandlerInstall(HWND EventWnd);
	void EventHandlerInstall(PEVENT_CALLBACK pEventCallback);
	void EventHandlerRemove(void);
//	HRESULT Initialize(HWND parentWin, char driveLetter, char* drivePath = NULL);//{return S_OK;};
	HRESULT Initialize(HWND parentWin, char driveLetter, BYTE region = NULL);//{return S_OK;};
	HRESULT CheckDrive(char driveLetter, char __far* id);//{return S_OK;};
	HRESULT CheckDrive(char* drivePath, char __far* id);//{return S_OK;};
	DVDPlayerMode GetPlayerMode(void);//{return (DVDPlayerMode)S_OK;};
	HRESULT StartPresentation(void);//{return S_OK;};
	DVDDiskType GetDiskType(void);//{return (DVDDiskType)S_OK;};
	HRESULT ForwardScan(short speed);//{return S_OK;};
	HRESULT BackwardScan(short speed);//{return S_OK;};
	HRESULT Get_VideoSize(WORD& x, WORD& y);//{return S_OK;};
	ULONG GetNumberOfTitles(void);//{return S_OK;};
	HRESULT GetExtendedPlayerState(ExtendedPlayerState& eps);//{return S_OK;};
	HRESULT GetAvailStreams(BYTE& af, ULONG& spf);//{return S_OK;};
	HRESULT EnableCC(bool enable);//{return S_OK;};

	HRESULT GetAudioDynRngCompMode(AC3DynamicRange& drm);
	HRESULT SetAudioDynRngCompMode(const AC3DynamicRange& drm);

	HRESULT GetAudioDownmixMode(AC3SpeakerConfig & dmm);
	HRESULT SetAudioDownmixMode(const AC3SpeakerConfig & dmm);

	HRESULT GetAudioDualMode(AC3DualModeConfig& plm);
	HRESULT SetAudioDualMode(const AC3DualModeConfig& plm);

	HRESULT GetAudioSpdifOutMode(SPDIFOutputMode& dmm);
	HRESULT SetAudioSpdifOutMode(const SPDIFOutputMode& dmm);

	HRESULT GetAudioLFEMode(BOOL& lfe);
	HRESULT SetAudioLFEMode(const BOOL& lfe);

	HRESULT GetAudioDialogNormMode(AC3DialogNorm& dn);
	HRESULT SetAudioDialogNormMode(const AC3DialogNorm& dn);

	HRESULT GetAudioCompMode(AC3OperationalMode& opm);
	HRESULT SetAudioCompMode(const AC3OperationalMode& opm);

	HRESULT GetAudioCompSFHigh(WORD& sfh);
	HRESULT SetAudioCompSFHigh(const WORD& sfh);

	HRESULT GetAudioCompSFLow(WORD& sfl);
	HRESULT SetAudioCompSFLow(const WORD& sfl);


	// 0x0000 == surround, 0x0001 == stereo, 0x0002 == mono.
	HRESULT GetSpeakerOutputMode(WORD& config);
	HRESULT SetSpeakerOutputMode(const WORD& config);

	// 0x0000 == PCM, 0x0001 == DTS, 0x0002 == AC3
	HRESULT GetDigitalOutputMode(WORD& config);
	HRESULT SetDigitalOutputMode(const WORD& config);

// interface resembling IDVDControl2
    virtual HRESULT STDMETHODCALLTYPE PlayTitle(
        /* [in] */ ULONG ulTitle);

    virtual HRESULT STDMETHODCALLTYPE PlayChapterInTitle(
        /* [in] */ ULONG ulTitle,
        /* [in] */ ULONG ulChapter);

    virtual HRESULT STDMETHODCALLTYPE PlayAtTimeInTitle(
        /* [in] */ ULONG ulTitle,
        /* [in] */ DVD_HMSF_TIMECODE *pStartTime);

    virtual HRESULT STDMETHODCALLTYPE PlayAtTimeInTitleForced(
        /* [in] */ ULONG ulTitle,
        /* [in] */ DVD_HMSF_TIMECODE *pStartTime);

	virtual HRESULT STDMETHODCALLTYPE Stop( void);

    virtual HRESULT STDMETHODCALLTYPE ReturnFromSubmenu(void);

    virtual HRESULT STDMETHODCALLTYPE PlayAtTime(
        /* [in] */ DVD_HMSF_TIMECODE *pTime);

    virtual HRESULT STDMETHODCALLTYPE PlayChapter(
        /* [in] */ ULONG ulChapter);

    virtual HRESULT STDMETHODCALLTYPE PlayPrevChapter(void);

    virtual HRESULT STDMETHODCALLTYPE ReplayChapter(void);

    virtual HRESULT STDMETHODCALLTYPE PlayNextChapter(void);

    virtual HRESULT STDMETHODCALLTYPE PlayForwards(
        /* [in] */ double dSpeed);

    virtual HRESULT STDMETHODCALLTYPE PlayBackwards(
        /* [in] */ double dSpeed);

    virtual HRESULT STDMETHODCALLTYPE ShowMenu(
        /* [in] */ DVD_MENU_ID MenuID);

    virtual HRESULT STDMETHODCALLTYPE Resume(void);

    virtual HRESULT STDMETHODCALLTYPE SelectRelativeButton(
        DVD_RELATIVE_BUTTON buttonDir);

    virtual HRESULT STDMETHODCALLTYPE ActivateButton( void);

//    virtual HRESULT STDMETHODCALLTYPE SelectButton(
//        /* [in] */ ULONG ulButton){return S_OK;};

    virtual HRESULT STDMETHODCALLTYPE SelectAndActivateButton(
        /* [in] */ ULONG ulButton);

    virtual HRESULT STDMETHODCALLTYPE StillOff( void);

    virtual HRESULT STDMETHODCALLTYPE Pause(
        /* [in] */ BOOL bState);

    virtual HRESULT STDMETHODCALLTYPE SelectAudioStream(
        /* [in] */ ULONG ulAudio);

    virtual HRESULT STDMETHODCALLTYPE SelectSubpictureStream(
        /* [in] */ ULONG ulSubPicture);

    virtual HRESULT STDMETHODCALLTYPE SetSubpictureState(
        /* [in] */ BOOL bState);

    virtual HRESULT STDMETHODCALLTYPE SelectAngle(
        /* [in] */ ULONG ulAngle);

    virtual HRESULT STDMETHODCALLTYPE SelectParentalLevel(
        /* [in] */ ULONG ulParentalLevel);

    virtual HRESULT STDMETHODCALLTYPE SelectParentalCountry(
        /* [in] */ WORD wCountry);

    virtual HRESULT STDMETHODCALLTYPE ActivateAtPosition(
        /* [in] */ POINT point);

    virtual HRESULT STDMETHODCALLTYPE SelectAtPosition(
        /* [in] */ POINT point);

    virtual HRESULT STDMETHODCALLTYPE PlayChaptersAutoStop(
        /* [in] */ ULONG ulTitle,
        /* [in] */ ULONG ulChapter,
        /* [in] */ ULONG ulChaptersToPlay);

    virtual HRESULT STDMETHODCALLTYPE AcceptParentalLevelChange(
        /* [in] */ BOOL bAccept);

    virtual HRESULT STDMETHODCALLTYPE PlayPeriodInTitleAutoStop(
		/* [in] */ ULONG ulTitle,
		/* [in] */ ULONG ulChapter,
	    /* [in] */ DVD_HMSF_TIMECODE *pStartTime,
		/* [in] */ DVD_HMSF_TIMECODE *pEndTime,
		/* [in] */ DWORD flags,
		/* [out]*/ DWORD& breakpointID);

    virtual HRESULT STDMETHODCALLTYPE ClearBreakpoint(
        /* [in] */ DWORD& breakpointID);

    virtual HRESULT STDMETHODCALLTYPE SelectDefaultMenuLanguage(
        /* [in] */ LCID Language);

    virtual HRESULT STDMETHODCALLTYPE SelectDefaultAudioLanguage(
        /* [in] */ LCID Language,
        /* [in] */ DVD_AUDIO_LANG_EXT audioExtension);

    virtual HRESULT STDMETHODCALLTYPE SelectDefaultSubpictureLanguage(
        /* [in] */ LCID Language,
        /* [in] */ DVD_SUBPICTURE_LANG_EXT subpictureExtension);



// interface resembling IDVDInfo2
    virtual HRESULT STDMETHODCALLTYPE GetCurrentDomain(
        /* [out] */ DVD_DOMAIN __RPC_FAR *pDomain);

    virtual HRESULT STDMETHODCALLTYPE GetCurrentLocation(
        /* [out] */ DVD_PLAYBACK_LOCATION2 __RPC_FAR *pLocation);

    virtual HRESULT STDMETHODCALLTYPE GetTotalTitleTime(
        /* [out] */ DVD_HMSF_TIMECODE __RPC_FAR *pTotalTime,
        /* [out] */ ULONG __RPC_FAR *ulTimeCodeFlags,
		/* [in]	 */ ULONG title = NULL);

    virtual HRESULT STDMETHODCALLTYPE GetCurrentButton(
        /* [out] */ ULONG __RPC_FAR *pulButtonsAvailable,
        /* [out] */ ULONG __RPC_FAR *pulCurrentButton);

    virtual HRESULT STDMETHODCALLTYPE GetCurrentAngle(
        /* [out] */ ULONG __RPC_FAR *pulAnglesAvailable,
        /* [out] */ ULONG __RPC_FAR *pulCurrentAngle);

    virtual HRESULT STDMETHODCALLTYPE GetCurrentAudio(
        /* [out] */ ULONG __RPC_FAR *pulStreamsAvailable,
        /* [out] */ ULONG __RPC_FAR *pulCurrentStream);

    virtual HRESULT STDMETHODCALLTYPE GetCurrentSubpicture(
        /* [out] */ ULONG __RPC_FAR *pulStreamsAvailable,
        /* [out] */ ULONG __RPC_FAR *pulCurrentStream,
        /* [out] */ BOOL __RPC_FAR *pbIsDisabled);

    virtual HRESULT STDMETHODCALLTYPE GetCurrentUOPS(
        /* [out] */ ULONG __RPC_FAR *pulUOPs);

    virtual HRESULT STDMETHODCALLTYPE GetAllSPRMs(
        /* [out] */ SPRMARRAY __RPC_FAR *pRegisterArray);

    virtual HRESULT STDMETHODCALLTYPE GetAllGPRMs(
        /* [out] */ GPRMARRAY __RPC_FAR *pRegisterArray);

    virtual HRESULT STDMETHODCALLTYPE GetAudioLanguage(
        /* [in] */ ULONG ulStream,
        /* [out] */ LCID __RPC_FAR *pLanguage);

    virtual HRESULT STDMETHODCALLTYPE GetAudioLanguage(
        /* [in] */ ULONG ulStream,
        /* [out] */ char* pLanguage, UINT nMaxLength);

    virtual HRESULT STDMETHODCALLTYPE GetSubpictureLanguage(
        /* [in] */ ULONG ulStream,
        /* [out] */ LCID __RPC_FAR *pLanguage);

    virtual HRESULT STDMETHODCALLTYPE GetSubpictureLanguage(
        /* [in] */ ULONG ulStream,
        /* [out] */ char* pLanguage, UINT nMaxLength);

    virtual HRESULT STDMETHODCALLTYPE GetTitleAttributes(
        /* [in] */ ULONG ulTitle,
        /* [out] */ DVD_ATR __RPC_FAR *pATR);

    virtual HRESULT STDMETHODCALLTYPE GetVMGAttributes(
        /* [out] */ DVD_ATR __RPC_FAR *pATR);

    virtual HRESULT STDMETHODCALLTYPE GetCurrentVideoAttributes(
        /* [out] */ DVD_VideoATR __RPC_FAR *pATR);

    virtual HRESULT STDMETHODCALLTYPE GetVideoAttributes(
        /* [in] */ ULONG ulTitle,
        /* [in] */ BOOL bForMenus,
        /* [out] */ DVD_VideoATR __RPC_FAR *pATR);

    virtual HRESULT STDMETHODCALLTYPE GetAudioAttributes(
        /* [in] */ ULONG ulStream,
        /* [out] */ DVD_AudioAttributes __RPC_FAR *pATR);

    virtual HRESULT STDMETHODCALLTYPE GetKaraokeAttributes(
        /* [in] */ ULONG ulStream,
        /* [out] */ DVD_KaraokeAttributes __RPC_FAR *pAttributes);

    virtual HRESULT STDMETHODCALLTYPE GetSubpictureAttributes(
        /* [in] */ ULONG ulStream,
        /* [out] */ DVD_SubpictureATR __RPC_FAR *pATR);

    virtual HRESULT STDMETHODCALLTYPE GetDVDVolumeInfo(
        /* [out] */ ULONG __RPC_FAR *pulNumOfVolumes,
        /* [out] */ ULONG __RPC_FAR *pulVolume,
        /* [out] */ DVD_DISC_SIDE __RPC_FAR *pSide,
        /* [out] */ ULONG __RPC_FAR *pulNumOfTitles);

    virtual HRESULT STDMETHODCALLTYPE GetDVDTextNumberOfLanguages(
        /* [out] */ ULONG __RPC_FAR *pulNumOfLangs);

    virtual HRESULT STDMETHODCALLTYPE GetDVDTextLanguageInfo(
        /* [in] */ ULONG ulLangIndex,
        /* [out] */ ULONG __RPC_FAR *pulNumOfStrings,
        /* [out] */ LCID __RPC_FAR *pLangCode,
        /* [out] */ enum DVD_TextCharSet __RPC_FAR *pbCharacterSet);

    virtual HRESULT STDMETHODCALLTYPE GetDVDTextStringAsNative(
        /* [in] */ ULONG ulLangIndex,
        /* [in] */ ULONG ulStringIndex,
        /* [out] */ BYTE __RPC_FAR *pbBuffer,
        /* [in] */ ULONG ulMaxBufferSize,
        /* [out] */ ULONG __RPC_FAR *pulActualSize,
        /* [out] */ enum DVD_TextStringType __RPC_FAR *pType);

    virtual HRESULT STDMETHODCALLTYPE GetDVDTextStringAsUnicode(
        /* [in] */ ULONG ulLangIndex,
        /* [in] */ ULONG ulStringIndex,
        /* [out] */ WCHAR __RPC_FAR *pchwBuffer,
        /* [in] */ ULONG ulMaxBufferSize,
        /* [out] */ ULONG __RPC_FAR *pulActualSize,
        /* [out] */ enum DVD_TextStringType __RPC_FAR *pType);

    virtual HRESULT STDMETHODCALLTYPE GetPlayerParentalLevel(
        /* [out] */ ULONG __RPC_FAR *pulParentalLevel,
        /* [out] */ WORD __RPC_FAR *pCountryCode);

    virtual HRESULT STDMETHODCALLTYPE GetNumberOfChapters(
        /* [in] */ ULONG ulTitle,
        /* [out] */ ULONG __RPC_FAR *pulNumOfChapters);

    virtual HRESULT STDMETHODCALLTYPE GetTitleParentalLevels(
        /* [in] */ ULONG ulTitle,
        /* [out] */ ULONG __RPC_FAR *pulParentalLevels);

    virtual HRESULT STDMETHODCALLTYPE GetDVDDirectory(
        /* [size_is][out] */ LPWSTR pszwPath,
        /* [in] */ ULONG cbMaxSize,
        /* [out] */ ULONG __RPC_FAR *pcbActualSize);

    virtual HRESULT STDMETHODCALLTYPE IsAudioStreamEnabled(
        /* [in] */ ULONG ulStreamNum,
        /* [out] */ BOOL __RPC_FAR *pbEnabled);

    virtual HRESULT STDMETHODCALLTYPE GetUniqueID(
        /* [in] */ LPCWSTR pszwPath,
        /* [out] */ ULONGLONG __RPC_FAR *pullUniqueID);

    virtual HRESULT STDMETHODCALLTYPE GetMenuLanguages(
        /* [out] */ LCID __RPC_FAR *pLanguages,
        /* [in] */ ULONG uMaxLanguages,
        /* [out] */ ULONG __RPC_FAR *puActualLanguages);

    virtual HRESULT STDMETHODCALLTYPE GetButtonAtPosition(
        /* [in] */ POINT point,
        /* [out] */ ULONG __RPC_FAR *puButtonIndex);

    virtual HRESULT STDMETHODCALLTYPE GetDefaultMenuLanguage(
        /* [out] */ LCID __RPC_FAR *pLanguage);

    virtual HRESULT STDMETHODCALLTYPE GetDefaultAudioLanguage(
        /* [out] */ LCID __RPC_FAR *pLanguage,
        /* [out] */ DVD_AUDIO_LANG_EXT __RPC_FAR *pAudioExtension);

    virtual HRESULT STDMETHODCALLTYPE GetDefaultSubpictureLanguage(
        /* [out] */ LCID __RPC_FAR *pLanguage,
        /* [out] */ DVD_SUBPICTURE_LANG_EXT __RPC_FAR *pSubpictureExtension);

	HRESULT NextChapter ();
	HRESULT PreviousChapter ();
	HRESULT NextTitle();
	HRESULT PreviousTitle();
	HRESULT put_WindowPosition(INT iLeft, INT iTop, INT iWidth, INT iHeight){return S_OK;};
//	HRESULT get_MessageWindow(LONG* plVal){return S_OK;};
	HRESULT put_MessageWindow(LONG pVal){return S_OK;};
	HRESULT get_FullScreenMode(BOOL* pbVal){return S_OK;};
	HRESULT put_FullScreenMode(BOOL lVal){return S_OK;};


	HRESULT ShowMenu (LONG lMenu);
	bool IsPlaying();
	HRESULT Play();
	HRESULT Pause();
	HRESULT TimeSearch(INT iHour, INT iMin, INT iSec);
	HRESULT ConvertError( ULONG ulErr );
	HRESULT Get_AspectRatios( ULONG& xFactor, ULONG& yFactor );//{return S_OK;};
	DisplayPresentationMode GetDisplayMode(void);
	HRESULT SetDisplayMode( DisplayPresentationMode mode );//{return S_OK;};
	HRESULT Set_AspectRatios( ULONG& xFactor, ULONG& yFactor );//{return S_OK;};
	HRESULT SetSystemRegion(char driveLetter, BYTE regMask);
	HRESULT GetSystemRegion(char driveLetter, BYTE &regMask);

	HRESULT put_Volume(LONG lVal);
	HRESULT get_Volume(LONG* plVal);

	HRESULT EnableScanAudio(bool bEnable);
	HRESULT GetColorkeyStatus(BOOL &bEnabled);
	HRESULT SetColorkey(DWORD dwColorkey);
	HRESULT SetDestinationRect(INT leftX, INT topY, INT width, INT height);
	HRESULT SetOutputRect(INT leftX, INT topY, INT width, INT height);

	HRESULT Trick_FrameForward();
	HRESULT Trick_FrameBackward();
	HRESULT Trick_ReversePlay(WORD wSpeed = C_NORMAL_PLAYBACK_SPEED);
	HRESULT Trick_Zoom(INT leftX, INT topY, INT width, INT height);
	HRESULT Trick_Zoom(INT leftX, INT topY, float scaleFactor);
	HRESULT Freeze(BYTE*& ppFreezeBuffer, DWORD& dwBufferSize);
	HRESULT Defrost(BYTE* pFreezeBuffer, DWORD dwBufferSize);

	HRESULT Get_CurrentScaleFactor(float& cSF);
    friend void WINAPI DDPCaptureEvent(DWORD dwEvent, VOID* pvBuffer, DWORD dwInfo);


};


typedef DVDMediaPlayer			CQVDRDVDMediaPlayer;


enum parental_level
{
	E_PARENTAL_GENERAL=1,
		E_PARENTAL_RESERVED1,
		E_PARENTAL_PG,
		E_PARENTAL_PG13,
		E_PARENTAL_RESERVED2,
		E_PARENTAL_R,
		E_PARENTAL_NC17,
		E_PARENTAL_RESTRICTED,
		E_PARENTAL_DISABLED=0xFFFFFFFF
};

struct LanguageData
	{
	WORD    code;
	BYTE    langID;
	};

const LanguageData LANGUAGE_DATA[] =
	{
	{ 0, LANG_NEUTRAL },
	{ (WORD)'a' << 8 | (BYTE)'f', LANG_AFRIKAANS },
	{ (WORD)'a' << 8 | (BYTE)'r', LANG_ARABIC },
	{ (WORD)'b' << 8 | (BYTE)'g', LANG_BULGARIAN },
	{ (WORD)'c' << 8 | (BYTE)'a', LANG_CATALAN },
	{ (WORD)'c' << 8 | (BYTE)'s', LANG_CZECH },
	{ (WORD)'d' << 8 | (BYTE)'a', LANG_DANISH },
	{ (WORD)'d' << 8 | (BYTE)'e', LANG_GERMAN },
	{ (WORD)'e' << 8 | (BYTE)'l', LANG_GREEK },
	{ (WORD)'e' << 8 | (BYTE)'n', LANG_ENGLISH },
	{ (WORD)'e' << 8 | (BYTE)'s', LANG_SPANISH },
	{ (WORD)'e' << 8 | (BYTE)'t', LANG_ESTONIAN },
	{ (WORD)'e' << 8 | (BYTE)'u', LANG_BASQUE },
	{ (WORD)'f' << 8 | (BYTE)'i', LANG_FINNISH },
	{ (WORD)'f' << 8 | (BYTE)'o', LANG_FAEROESE },
	{ (WORD)'f' << 8 | (BYTE)'r', LANG_FRENCH },
	{ (WORD)'h' << 8 | (BYTE)'r', LANG_CROATIAN },
	{ (WORD)'h' << 8 | (BYTE)'u', LANG_HUNGARIAN },
	{ (WORD)'i' << 8 | (BYTE)'n', LANG_INDONESIAN },
	{ (WORD)'i' << 8 | (BYTE)'s', LANG_ICELANDIC },
	{ (WORD)'i' << 8 | (BYTE)'t', LANG_ITALIAN },
	{ (WORD)'i' << 8 | (BYTE)'w', LANG_HEBREW },
	{ (WORD)'j' << 8 | (BYTE)'a', LANG_JAPANESE },
	{ (WORD)'k' << 8 | (BYTE)'o', LANG_KOREAN },
	{ (WORD)'l' << 8 | (BYTE)'r', LANG_LITHUANIAN },
	{ (WORD)'l' << 8 | (BYTE)'v', LANG_LATVIAN /*_LETTISH*/ },
	{ (WORD)'n' << 8 | (BYTE)'l', LANG_DUTCH },
	{ (WORD)'n' << 8 | (BYTE)'o', LANG_NORWEGIAN },
	{ (WORD)'p' << 8 | (BYTE)'l', LANG_POLISH },
	{ (WORD)'p' << 8 | (BYTE)'t', LANG_PORTUGUESE },
	{ (WORD)'r' << 8 | (BYTE)'m', 0x17 /*IDS_RHAETO_ROMANCE*/ },
	{ (WORD)'r' << 8 | (BYTE)'o', LANG_ROMANIAN },
	{ (WORD)'r' << 8 | (BYTE)'u', LANG_RUSSIAN },
	{ (WORD)'s' << 8 | (BYTE)'h', LANG_SERBIAN },
	{ (WORD)'s' << 8 | (BYTE)'k', LANG_SLOVAK },
	{ (WORD)'s' << 8 | (BYTE)'l', LANG_SLOVENIAN },
	{ (WORD)'s' << 8 | (BYTE)'q', LANG_ALBANIAN },
	{ (WORD)'s' << 8 | (BYTE)'r', LANG_SERBIAN },
	{ (WORD)'s' << 8 | (BYTE)'v', LANG_SWEDISH },
	{ (WORD)'t' << 8 | (BYTE)'h', LANG_THAI },
	{ (WORD)'t' << 8 | (BYTE)'r', LANG_TURKISH },
	{ (WORD)'u' << 8 | (BYTE)'k', LANG_UKRAINIAN },
	{ (WORD)'u' << 8 | (BYTE)'r', 0x20 /*IDS_URDU*/ },
	{ (WORD)'v' << 8 | (BYTE)'i', LANG_VIETNAMESE },
	{ (WORD)'z' << 8 | (BYTE)'h', LANG_CHINESE },
	{ (WORD)'b' << 8 | (BYTE)'e', LANG_BELARUSIAN /*IDS_BYELORUSSIAN*/ },
	{ (WORD)'f' << 8 | (BYTE)'a', LANG_FARSI /*IDS_PERSIAN*/ },
	//{ (WORD)'a' << 8 | (BYTE)'a', IDS_AFAR },
	//{ (WORD)'a' << 8 | (BYTE)'b', IDS_ABKHAZIAN },
	//{ (WORD)'a' << 8 | (BYTE)'m', IDS_AMEHARIC },
	//{ (WORD)'a' << 8 | (BYTE)'s', IDS_ASSAMESE },
	//{ (WORD)'a' << 8 | (BYTE)'y', IDS_AYMARA },
	//{ (WORD)'a' << 8 | (BYTE)'z', IDS_AZERBAIJANI },
	//{ (WORD)'b' << 8 | (BYTE)'a', IDS_BASHKIR },
	//{ (WORD)'b' << 8 | (BYTE)'h', IDS_BIHARI },
	//{ (WORD)'b' << 8 | (BYTE)'i', IDS_BISLAMA },
	//{ (WORD)'b' << 8 | (BYTE)'n', IDS_BENGALI },
	//{ (WORD)'b' << 8 | (BYTE)'o', IDS_TIBETAN },
	//{ (WORD)'b' << 8 | (BYTE)'r', IDS_BRETON },
	//{ (WORD)'c' << 8 | (BYTE)'o', IDS_CORSICAN },
	//{ (WORD)'c' << 8 | (BYTE)'y', IDS_WELSH },
	//{ (WORD)'d' << 8 | (BYTE)'z', IDS_BHUTANI },
	//{ (WORD)'e' << 8 | (BYTE)'o', IDS_ESPERANTO },
	//{ (WORD)'f' << 8 | (BYTE)'j', IDS_FIJI },
	//{ (WORD)'f' << 8 | (BYTE)'y', IDS_FRISIAN },
	//{ (WORD)'g' << 8 | (BYTE)'a', IDS_IRISH },
	//{ (WORD)'g' << 8 | (BYTE)'d', IDS_SCOTS_GAELIC },
	//{ (WORD)'g' << 8 | (BYTE)'l', IDS_GALIZIAN },
	//{ (WORD)'g' << 8 | (BYTE)'n', IDS_GUARANI },
	//{ (WORD)'g' << 8 | (BYTE)'u', IDS_GUJARATI },
	//{ (WORD)'h' << 8 | (BYTE)'a', IDS_HAUSA },
	//{ (WORD)'h' << 8 | (BYTE)'i', IDS_HINDI },
	//{ (WORD)'h' << 8 | (BYTE)'y', IDS_ARMENIAN },
	//{ (WORD)'i' << 8 | (BYTE)'a', IDS_INTERLINGUA },
	//{ (WORD)'i' << 8 | (BYTE)'e', IDS_INTERLINQUE },
	//{ (WORD)'i' << 8 | (BYTE)'k', IDS_INUPIAK },
	//{ (WORD)'j' << 8 | (BYTE)'i', IDS_YIDDISH },
	//{ (WORD)'j' << 8 | (BYTE)'w', IDS_JAVANESE },
	//{ (WORD)'k' << 8 | (BYTE)'a', IDS_GEORGIAN },
	//{ (WORD)'k' << 8 | (BYTE)'k', IDS_KAZAHK },
	//{ (WORD)'k' << 8 | (BYTE)'l', IDS_GREENLANDIC },
	//{ (WORD)'k' << 8 | (BYTE)'m', IDS_CAMBODIAN },
	//{ (WORD)'k' << 8 | (BYTE)'n', IDS_KANNADA },
	//{ (WORD)'k' << 8 | (BYTE)'s', IDS_KASHMIRI },
	//{ (WORD)'k' << 8 | (BYTE)'u', IDS_KURDISH },
	//{ (WORD)'k' << 8 | (BYTE)'y', IDS_KIRGHIZ },
	//{ (WORD)'l' << 8 | (BYTE)'a', IDS_LATIN },
	//{ (WORD)'l' << 8 | (BYTE)'n', IDS_LINGALA },
	//{ (WORD)'l' << 8 | (BYTE)'o', IDS_LAOTHIAN },
	//{ (WORD)'m' << 8 | (BYTE)'g', IDS_MALAGASY },
	//{ (WORD)'m' << 8 | (BYTE)'i', IDS_MAORI },
	//{ (WORD)'m' << 8 | (BYTE)'k', IDS_MACEDONIAN },
	//{ (WORD)'m' << 8 | (BYTE)'l', IDS_MALAYALAM },
	//{ (WORD)'m' << 8 | (BYTE)'n', IDS_MONGOLIAN },
	//{ (WORD)'m' << 8 | (BYTE)'o', IDS_MOLDAVIAN },
	//{ (WORD)'m' << 8 | (BYTE)'r', IDS_MARATHI },
	//{ (WORD)'m' << 8 | (BYTE)'s', IDS_MALAY },
	//{ (WORD)'m' << 8 | (BYTE)'t', IDS_MALTESE },
	//{ (WORD)'m' << 8 | (BYTE)'y', IDS_BURMESE },
	//{ (WORD)'n' << 8 | (BYTE)'a', IDS_NAURU },
	//{ (WORD)'n' << 8 | (BYTE)'e', IDS_NEPALI },
	//{ (WORD)'o' << 8 | (BYTE)'c', IDS_OCCITAN },
	//{ (WORD)'o' << 8 | (BYTE)'m', IDS_AFAN_OROMO },
	//{ (WORD)'o' << 8 | (BYTE)'r', IDS_ORIYA },
	//{ (WORD)'p' << 8 | (BYTE)'a', IDS_PANJABI },
	//{ (WORD)'p' << 8 | (BYTE)'s', IDS_PASHTO_PUSHTO },
	//{ (WORD)'q' << 8 | (BYTE)'u', IDS_QUECHUA },
	//{ (WORD)'r' << 8 | (BYTE)'n', IDS_KIRUNDI },
	//{ (WORD)'r' << 8 | (BYTE)'w', IDS_KINYARWANDA },
	//{ (WORD)'s' << 8 | (BYTE)'a', IDS_SANSKRIT },
	//{ (WORD)'s' << 8 | (BYTE)'d', IDS_SINDHI },
	//{ (WORD)'s' << 8 | (BYTE)'g', IDS_SANGHO },
	//{ (WORD)'s' << 8 | (BYTE)'i', IDS_SINGHALESE },
	//{ (WORD)'s' << 8 | (BYTE)'m', IDS_SAMOAN },
	//{ (WORD)'s' << 8 | (BYTE)'n', IDS_SHONA },
	//{ (WORD)'s' << 8 | (BYTE)'o', IDS_SOMALI },
	//{ (WORD)'s' << 8 | (BYTE)'s', IDS_SISWATI },
	//{ (WORD)'s' << 8 | (BYTE)'t', IDS_SESOTHO },
	//{ (WORD)'s' << 8 | (BYTE)'u', IDS_SUDANESE },
	//{ (WORD)'s' << 8 | (BYTE)'w', IDS_SWAHILI },
	//{ (WORD)'t' << 8 | (BYTE)'a', IDS_TAMIL },
	//{ (WORD)'t' << 8 | (BYTE)'e', IDS_TELUGU },
	//{ (WORD)'t' << 8 | (BYTE)'g', IDS_TAJIK },
	//{ (WORD)'t' << 8 | (BYTE)'i', IDS_TIGRINYA },
	//{ (WORD)'t' << 8 | (BYTE)'k', IDS_TURKMEN },
	//{ (WORD)'t' << 8 | (BYTE)'l', IDS_TAGALOG },
	//{ (WORD)'t' << 8 | (BYTE)'n', IDS_SETSWANA },
	//{ (WORD)'t' << 8 | (BYTE)'o', IDS_TONGA },
	//{ (WORD)'t' << 8 | (BYTE)'s', IDS_TSONGA },
	//{ (WORD)'t' << 8 | (BYTE)'t', IDS_TATAR },
	//{ (WORD)'t' << 8 | (BYTE)'w', IDS_TWI },
	//{ (WORD)'u' << 8 | (BYTE)'z', IDS_UZBEK },
	//{ (WORD)'v' << 8 | (BYTE)'o', IDS_VOLAPUK },
	//{ (WORD)'w' << 8 | (BYTE)'o', IDS_WOLOF },
	//{ (WORD)'x' << 8 | (BYTE)'h', IDS_XHOSA },
	//{ (WORD)'y' << 8 | (BYTE)'o', IDS_YORUBA },
	//{ (WORD)'z' << 8 | (BYTE)'u', IDS_ZULU }
	};


#endif //AFX_DVDMEDIAPLAYER_H__DD1FA08A_135D_11D3_810A_00A0CC23CB74__INCLUDED
