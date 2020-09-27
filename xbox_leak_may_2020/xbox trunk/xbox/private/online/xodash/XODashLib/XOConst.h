#pragma once

#include "winerror.h"
// DVD types enun

#include "ButtonName.h"
#include "ButtonId.h"
#include "ButtonHelp.h"
#include "SceneName.h"
#include "SceneId.h"

enum DiscType
{ 
	eDiscNone = 0,
	eDiscBad ,
	eDiscTitle ,
	eDiscAudio ,
	eDiscVideo
};


#define MAX_BLOCKS_TO_SHOW 50000
#define LF_FACESIZE 32


#define BODY_FONT 0
#define HEADING_FONT 1


#define MAX_SCENE_NUM 300
#define MAX_CHILD_SCENE_NUM 50
#define MAX_BUTTON_IN_SCENE 50
#define MAX_EXIT_POINT_NUM 20
#define MAX_URL_SIZE 100
#define MAX_NAVIGATOR_STACK_SIZE 30
#define MAX_INPUT_TEXT_SIZE  20

// NTS specific consts
#define IP_ADDRESS_SIZE 15
#define MAX_NTS_SIZE 64

//-----------------------------------------------------------------------------
// Error codes
//-----------------------------------------------------------------------------
#define XBAPP_S_BASE 0x00000040
#define XBMSG_HAD_TO_WAIT       XBAPP_S_BASE+1



#define XBAPP_INFO_BASE 0x82000000
#define XBMSG_OK                XBAPP_INFO_BASE+1
#define XBMSG_CANCEL            XBAPP_INFO_BASE+2
#define XBMSG_NEED_WAITCURSOR   XBAPP_INFO_BASE+3
#define XBMSG_BACK              XBAPP_INFO_BASE+4

#define XBAPPERR_BASE  XBAPP_INFO_BASE+1000
#define XBAPPERR_MEDIANOTFOUND  (XBAPPERR_BASE)+1
#define XBAPPERR_INVALIDIP		(XBAPPERR_BASE)+2
#define XBAPPERR_INVALIDSTR		(XBAPPERR_BASE)+3
#define XBAPPERR_MEMORYERR		(XBAPPERR_BASE)+3
#define XBAPPERR_INTERNAL_ERROR (XBAPPERR_BASE)+4

#define XBAPP_ERR_LOGON_CANCELED (XBAPPERR_BASE)+5
#define XBAPP_ERR_LOGON_FAILED (XBAPPERR_BASE)+6

#define XACT_E_USER_TOO_YOUNG (XBAPPERR_BASE)+7



#define XBAPPWRN_BASE  XBAPPERR_BASE+1000
#define XBAPPWRN_SCENE_CHANGED  XBAPPWRN_BASE+1

// bottom bits = bitField of button ID to press next
// this is the return value from some validation calls
#define XBAPP_BUTTON_ID 0x83000000



enum eSoundType
{
    eKeyboardSound,
    eStandardSound,
    eLastSoundType
};

enum eSound
{
    eSoundAPress,
    eSoundBPress,
    eSoundHighlight,
    eSoundDisabled,
    eLastSound
};

static TCHAR* SoundInfo[eLastSoundType][eLastSound] = 
{
    {_T("mainaudio\\Global Keyboard Stroke 2.wav"), _T("mainaudio\\Global B Button Back.wav"), 
        _T("mainaudio\\Global Keyboard Stroke 1.wav"), _T("mainaudio\\Global Error Message B.wav") },
    {_T("mainaudio\\Global A Button Select.wav"), _T("mainaudio\\Global B Button Back.wav"), 
            _T("mainaudio\\Global Scroll Beep.wav"), _T("mainaudio\\Global Error Message B.wav") },
};





typedef HRESULT (*ButtonCallBack)(void);

struct ErrorStrInfo
{
    HRESULT             m_hrError;
    const TCHAR*        m_pErrorID;
};

#define ERROR_STR_COUNT 9
static ErrorStrInfo ErrorStringIDs[ERROR_STR_COUNT] = 
{
    { XBAPPERR_MEDIANOTFOUND, _T("HELP_SETTINGS") },
    { XBAPPERR_INVALIDIP, _T("ERR_INVALID_IP")},
    { XBAPPERR_INVALIDSTR, _T("ERR_INVALID_STR")},
    { XONLINE_E_INTERNAL_ERROR,     _T("ERR_NO_SERVER")},
    //{ XUAC_E_NAME_TAKEN,           _T("ERR_NAME_UNAVAILABLE")},
    { XONLINE_E_ACCOUNTS_INVALID_KINGDOM,      _T("ERR_INAVLID_KINGDOM")},
    { XONLINE_E_ACCOUNTS_INVALID_USER,         _T("ERR_INVALID_USER")},
    { ERROR_SERVICE_SPECIFIC_ERROR, _T("ERR_NO_SERVER")},
    { XACT_E_USER_TOO_YOUNG,    _T("ERR_TOO_YOUNG")}
};

enum eGlobalModels
{
	eCellWallId = 0,
	eAButton,
	eBButton
};

enum eButtonState
{
	eButtonDefault = 0,
	eButtonPressed ,
	eButtonHighlighted,
	eButtonDisabled,
    eButtonDisabledHighlighted, // focused, but not pressable
    eButtonStateMax
};

enum eButtonOrientation
{
	eButtonVerticalOrientation,
	eButtonHorizontalOrientation,
};

enum eInputState
{
	eNoInput = 0,
	eFirstTimePress,
	eRepetitivePress
};

enum eAutoTestData
{
	eSceneInit,
	eChangeFocus,
	ePressed
};

// RTTI could not be enabled with XBox OS code, reinventing the weel
enum eButtonRTTI
{
	eOneTextButton,
	eTwoTextButton
};


// pathes app uses
#define LOCALE  "Locale\\"
#define XBG     "Xbg\\"
#define XBX     "Xbx\\"
#define ASE     "Ase\\"
#define SOUNDS  "Audio\\"
#define CONTENT "Content\\"

#define DASH_PARTITION		     "\\Device\\Harddisk0\\Partition2"
#define DASH_DRIVE_LETTER        'd'

#define APPLICATION_PATH_ANSI     "d:\\"
#define MEDIA_PATH_ANSI           "d:\\Media\\"
#define LOCALE_PATH_ANSI          MEDIA_PATH_ANSI LOCALE
#define XBG_PATH_ANSI             MEDIA_PATH_ANSI XBG
#define XBX_PATH_ANSI             MEDIA_PATH_ANSI XBX 
#define XTF_PATH_ANSI             APPLICATION_PATH_ANSI 
#define ASE_PATH_ANSI             MEDIA_PATH_ANSI ASE 
#define SOUNDS_PATH_ANSI          APPLICATION_PATH_ANSI SOUNDS
#define CONTENT_PATH_ANSI         MEDIA_PATH_ANSI CONTENT 


#define APPLICATION_PATH         _T(APPLICATION_PATH_ANSI) 
#define MEDIA_PATH               _T(MEDIA_PATH_ANSI) 
#define LOCALE_PATH              MEDIA_PATH _T(LOCALE) 
#define XBG_PATH                 MEDIA_PATH _T(XBG)
#define XBX_PATH                 MEDIA_PATH _T(XBX) 
#define XTF_PATH                 APPLICATION_PATH  
#define ASE_PATH                 MEDIA_PATH _T(ASE)  
#define SOUNDS_PATH              APPLICATION_PATH _T(SOUNDS)
#define CONTENT_PATH             MEDIA_PATH _T(CONTENT)  

#define LOCALE_FILE_ANSI         "locale"

