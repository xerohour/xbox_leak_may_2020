/****************************************************************************
 *
 *  Copyright (C) 1996-1999 Microsoft Corporation.  All Rights Reserved.
 *
 *  File:       dinput.h
 *  Content:    DirectInput include file
 *
 ****************************************************************************/

#ifndef __DINPUT_INCLUDED__
#define __DINPUT_INCLUDED__

#ifndef DIJ_RINGZERO

#ifdef _WIN32
#define COM_NO_WINDOWS_H
#include <objbase.h>
#endif

#endif /* DIJ_RINGZERO */

#ifdef __cplusplus
extern "C" {
#endif




/*
 *  To build applications for older versions of DirectInput
 *
 *  #define DIRECTINPUT_VERSION 0x0300
 *  or
 *  #define DIRECTINPUT_VERSION 0x0500
 *
 *  before #include <dinput.h>.  By default, #include <dinput.h>
 *  will produce a DirectX 7-compatible header file.
 *
 */

/*
 *  To build applications for older versions of DirectInput
 *
 *  #define DIRECTINPUT_VERSION [ 0x0300 | 0x0500 | 0x0700 ]
 *
 *  before #include <dinput.h>.  By default, #include <dinput.h>
 *  will produce a DirectX 8-compatible header file.
 *
 */
/*
 *  XBOX Titles are required to build with the version
 *	of DX supplied with the SDK it is build against.
 *
 *	Titles shall not redefine DIRECTINPUT_VERSION.
 */
#define DIRECTINPUT_VERSION 0x0800


/****************************************************************************
 *
 *      Interfaces
 *
 ****************************************************************************/
DEFINE_GUID(IID_IDirectInput8W,    0xBF798031,0x483A,0x4DA2,0xAA,0x99,0x5D,0x64,0xED,0x36,0x97,0x00);
DEFINE_GUID(IID_IDirectInputDevice8W,0x54D41081,0xDC15,0x4833,0xA4,0x1B,0x74,0x8F,0x73,0xA3,0x81,0x79);
/****************************************************************************
 *
 *      Predefined object types
 *
 ****************************************************************************/

DEFINE_GUID(GUID_XAxis,   0xA36D02E0,0xC9F3,0x11CF,0xBF,0xC7,0x44,0x45,0x53,0x54,0x00,0x00);
DEFINE_GUID(GUID_YAxis,   0xA36D02E1,0xC9F3,0x11CF,0xBF,0xC7,0x44,0x45,0x53,0x54,0x00,0x00);
DEFINE_GUID(GUID_ZAxis,   0xA36D02E2,0xC9F3,0x11CF,0xBF,0xC7,0x44,0x45,0x53,0x54,0x00,0x00);
DEFINE_GUID(GUID_RxAxis,  0xA36D02F4,0xC9F3,0x11CF,0xBF,0xC7,0x44,0x45,0x53,0x54,0x00,0x00);
DEFINE_GUID(GUID_RyAxis,  0xA36D02F5,0xC9F3,0x11CF,0xBF,0xC7,0x44,0x45,0x53,0x54,0x00,0x00);
DEFINE_GUID(GUID_RzAxis,  0xA36D02E3,0xC9F3,0x11CF,0xBF,0xC7,0x44,0x45,0x53,0x54,0x00,0x00);
DEFINE_GUID(GUID_Slider,  0xA36D02E4,0xC9F3,0x11CF,0xBF,0xC7,0x44,0x45,0x53,0x54,0x00,0x00);

DEFINE_GUID(GUID_Button,  0xA36D02F0,0xC9F3,0x11CF,0xBF,0xC7,0x44,0x45,0x53,0x54,0x00,0x00);
DEFINE_GUID(GUID_Key,     0x55728220,0xD33C,0x11CF,0xBF,0xC7,0x44,0x45,0x53,0x54,0x00,0x00);

DEFINE_GUID(GUID_POV,     0xA36D02F2,0xC9F3,0x11CF,0xBF,0xC7,0x44,0x45,0x53,0x54,0x00,0x00);

DEFINE_GUID(GUID_Unknown, 0xA36D02F3,0xC9F3,0x11CF,0xBF,0xC7,0x44,0x45,0x53,0x54,0x00,0x00);

/****************************************************************************
 *
 *      Interfaces and Structures...
 *
 ****************************************************************************/
#if(DIRECTINPUT_VERSION >= 0x0500)

/****************************************************************************
 *
 *      IDirectInputEffect
 *
 ****************************************************************************/

#define DIEFT_ALL                   0x00000000

#define DIEFT_CONSTANTFORCE         0x00000001
#define DIEFT_RAMPFORCE             0x00000002
#define DIEFT_PERIODIC              0x00000003
#define DIEFT_CONDITION             0x00000004
#define DIEFT_CUSTOMFORCE           0x00000005
#define DIEFT_HARDWARE              0x000000FF
#define DIEFT_FFATTACK              0x00000200
#define DIEFT_FFFADE                0x00000400
#define DIEFT_SATURATION            0x00000800
#define DIEFT_POSNEGCOEFFICIENTS    0x00001000
#define DIEFT_POSNEGSATURATION      0x00002000
#define DIEFT_DEADBAND              0x00004000
#define DIEFT_STARTDELAY            0x00008000
#define DIEFT_GETTYPE(n)            LOBYTE(n)

#define DI_DEGREES                  100
#define DI_FFNOMINALMAX             10000
#define DI_SECONDS                  1000000

typedef struct DICONSTANTFORCE {
    LONG  lMagnitude;
} DICONSTANTFORCE, *LPDICONSTANTFORCE;
typedef const DICONSTANTFORCE *LPCDICONSTANTFORCE;

typedef struct DIRAMPFORCE {
    LONG  lStart;
    LONG  lEnd;
} DIRAMPFORCE, *LPDIRAMPFORCE;
typedef const DIRAMPFORCE *LPCDIRAMPFORCE;

typedef struct DIPERIODIC {
    DWORD dwMagnitude;
    LONG  lOffset;
    DWORD dwPhase;
    DWORD dwPeriod;
} DIPERIODIC, *LPDIPERIODIC;
typedef const DIPERIODIC *LPCDIPERIODIC;

typedef struct DICONDITION {
    LONG  lOffset;
    LONG  lPositiveCoefficient;
    LONG  lNegativeCoefficient;
    DWORD dwPositiveSaturation;
    DWORD dwNegativeSaturation;
    LONG  lDeadBand;
} DICONDITION, *LPDICONDITION;
typedef const DICONDITION *LPCDICONDITION;

typedef struct DICUSTOMFORCE {
    DWORD cChannels;
    DWORD dwSamplePeriod;
    DWORD cSamples;
    LPLONG rglForceData;
} DICUSTOMFORCE, *LPDICUSTOMFORCE;
typedef const DICUSTOMFORCE *LPCDICUSTOMFORCE;


typedef struct DIENVELOPE {
    DWORD dwSize;                   /* sizeof(DIENVELOPE)   */
    DWORD dwAttackLevel;
    DWORD dwAttackTime;             /* Microseconds         */
    DWORD dwFadeLevel;
    DWORD dwFadeTime;               /* Microseconds         */
} DIENVELOPE, *LPDIENVELOPE;
typedef const DIENVELOPE *LPCDIENVELOPE;


/* This structure is defined for DirectX 5.0 compatibility */
typedef struct DIEFFECT_DX5 {
    DWORD dwSize;                   /* sizeof(DIEFFECT_DX5) */
    DWORD dwFlags;                  /* DIEFF_*              */
    DWORD dwDuration;               /* Microseconds         */
    DWORD dwSamplePeriod;           /* Microseconds         */
    DWORD dwGain;
    DWORD dwTriggerButton;          /* or DIEB_NOTRIGGER    */
    DWORD dwTriggerRepeatInterval;  /* Microseconds         */
    DWORD cAxes;                    /* Number of axes       */
    LPDWORD rgdwAxes;               /* Array of axes        */
    LPLONG rglDirection;            /* Array of directions  */
    LPDIENVELOPE lpEnvelope;        /* Optional             */
    DWORD cbTypeSpecificParams;     /* Size of params       */
    LPVOID lpvTypeSpecificParams;   /* Pointer to params    */
} DIEFFECT_DX5, *LPDIEFFECT_DX5;
typedef const DIEFFECT_DX5 *LPCDIEFFECT_DX5;

typedef struct DIEFFECT {
    DWORD dwSize;                   /* sizeof(DIEFFECT)     */
    DWORD dwFlags;                  /* DIEFF_*              */
    DWORD dwDuration;               /* Microseconds         */
    DWORD dwSamplePeriod;           /* Microseconds         */
    DWORD dwGain;
    DWORD dwTriggerButton;          /* or DIEB_NOTRIGGER    */
    DWORD dwTriggerRepeatInterval;  /* Microseconds         */
    DWORD cAxes;                    /* Number of axes       */
    LPDWORD rgdwAxes;               /* Array of axes        */
    LPLONG rglDirection;            /* Array of directions  */
    LPDIENVELOPE lpEnvelope;        /* Optional             */
    DWORD cbTypeSpecificParams;     /* Size of params       */
    LPVOID lpvTypeSpecificParams;   /* Pointer to params    */
#if(DIRECTINPUT_VERSION >= 0x0600)
    DWORD  dwStartDelay;            /* Microseconds         */
#endif /* DIRECTINPUT_VERSION >= 0x0600 */
} DIEFFECT, *LPDIEFFECT;
typedef DIEFFECT DIEFFECT_DX6;
typedef LPDIEFFECT LPDIEFFECT_DX6;
typedef const DIEFFECT *LPCDIEFFECT;


#if(DIRECTINPUT_VERSION >= 0x0700)
#ifndef DIJ_RINGZERO
typedef struct DIFILEEFFECT{
    DWORD       dwSize;
    GUID        GuidEffect;
    LPCDIEFFECT lpDiEffect;
    CHAR        szFriendlyName[MAX_PATH];
}DIFILEEFFECT, *LPDIFILEEFFECT;
typedef const DIFILEEFFECT *LPCDIFILEEFFECT;
typedef BOOL (FAR PASCAL * LPDIENUMEFFECTSINFILECALLBACK)(LPCDIFILEEFFECT , LPVOID);
#endif /* DIJ_RINGZERO */
#endif /* DIRECTINPUT_VERSION >= 0x0700 */

#define DIEFF_OBJECTIDS             0x00000001
#define DIEFF_OBJECTOFFSETS         0x00000002
#define DIEFF_CARTESIAN             0x00000010
#define DIEFF_POLAR                 0x00000020
#define DIEFF_SPHERICAL             0x00000040

#define DIEP_DURATION               0x00000001
#define DIEP_SAMPLEPERIOD           0x00000002
#define DIEP_GAIN                   0x00000004
#define DIEP_TRIGGERBUTTON          0x00000008
#define DIEP_TRIGGERREPEATINTERVAL  0x00000010
#define DIEP_AXES                   0x00000020
#define DIEP_DIRECTION              0x00000040
#define DIEP_ENVELOPE               0x00000080
#define DIEP_TYPESPECIFICPARAMS     0x00000100
#if(DIRECTINPUT_VERSION >= 0x0600)
#define DIEP_STARTDELAY             0x00000200
#define DIEP_ALLPARAMS_DX5          0x000001FF
#define DIEP_ALLPARAMS              0x000003FF
#else /* DIRECTINPUT_VERSION < 0x0600 */
#define DIEP_ALLPARAMS              0x000001FF
#endif /* DIRECTINPUT_VERSION < 0x0600 */
#define DIEP_START                  0x20000000
#define DIEP_NORESTART              0x40000000
#define DIEP_NODOWNLOAD             0x80000000
#define DIEB_NOTRIGGER              0xFFFFFFFF

#define DIES_SOLO                   0x00000001
#define DIES_NODOWNLOAD             0x80000000

#define DIEGES_PLAYING              0x00000001
#define DIEGES_EMULATED             0x00000002

typedef struct DIEFFESCAPE {
    DWORD   dwSize;
    DWORD   dwCommand;
    LPVOID  lpvInBuffer;
    DWORD   cbInBuffer;
    LPVOID  lpvOutBuffer;
    DWORD   cbOutBuffer;
} DIEFFESCAPE, *LPDIEFFESCAPE;

#ifndef DIJ_RINGZERO

#undef INTERFACE
#define INTERFACE IDirectInputEffect

DECLARE_INTERFACE_(IDirectInputEffect, IUnknown)
{
    /*** IUnknown methods ***/
    STDMETHOD(QueryInterface)(THIS_ REFIID riid, LPVOID * ppvObj) PURE;
    STDMETHOD_(ULONG,AddRef)(THIS) PURE;
    STDMETHOD_(ULONG,Release)(THIS) PURE;

    /*** IDirectInputEffect methods ***/
    STDMETHOD(Initialize)(THIS_ HINSTANCE,DWORD,REFGUID) PURE;
    STDMETHOD(GetEffectGuid)(THIS_ LPGUID) PURE;
    STDMETHOD(GetParameters)(THIS_ LPDIEFFECT,DWORD) PURE;
    STDMETHOD(SetParameters)(THIS_ LPCDIEFFECT,DWORD) PURE;
    STDMETHOD(Start)(THIS_ DWORD,DWORD) PURE;
    STDMETHOD(Stop)(THIS) PURE;
    STDMETHOD(GetEffectStatus)(THIS_ LPDWORD) PURE;
    STDMETHOD(Download)(THIS) PURE;
    STDMETHOD(Unload)(THIS) PURE;
    STDMETHOD(Escape)(THIS_ LPDIEFFESCAPE) PURE;
};

typedef struct IDirectInputEffect *LPDIRECTINPUTEFFECT;

#if !defined(__cplusplus) || defined(CINTERFACE)
#define IDirectInputEffect_QueryInterface(p,a,b) (p)->lpVtbl->QueryInterface(p,a,b)
#define IDirectInputEffect_AddRef(p) (p)->lpVtbl->AddRef(p)
#define IDirectInputEffect_Release(p) (p)->lpVtbl->Release(p)
#define IDirectInputEffect_Initialize(p,a,b,c) (p)->lpVtbl->Initialize(p,a,b,c)
#define IDirectInputEffect_GetEffectGuid(p,a) (p)->lpVtbl->GetEffectGuid(p,a)
#define IDirectInputEffect_GetParameters(p,a,b) (p)->lpVtbl->GetParameters(p,a,b)
#define IDirectInputEffect_SetParameters(p,a,b) (p)->lpVtbl->SetParameters(p,a,b)
#define IDirectInputEffect_Start(p,a,b) (p)->lpVtbl->Start(p,a,b)
#define IDirectInputEffect_Stop(p) (p)->lpVtbl->Stop(p)
#define IDirectInputEffect_GetEffectStatus(p,a) (p)->lpVtbl->GetEffectStatus(p,a)
#define IDirectInputEffect_Download(p) (p)->lpVtbl->Download(p)
#define IDirectInputEffect_Unload(p) (p)->lpVtbl->Unload(p)
#define IDirectInputEffect_Escape(p,a) (p)->lpVtbl->Escape(p,a)
#else
#define IDirectInputEffect_QueryInterface(p,a,b) (p)->QueryInterface(a,b)
#define IDirectInputEffect_AddRef(p) (p)->AddRef()
#define IDirectInputEffect_Release(p) (p)->Release()
#define IDirectInputEffect_Initialize(p,a,b,c) (p)->Initialize(a,b,c)
#define IDirectInputEffect_GetEffectGuid(p,a) (p)->GetEffectGuid(a)
#define IDirectInputEffect_GetParameters(p,a,b) (p)->GetParameters(a,b)
#define IDirectInputEffect_SetParameters(p,a,b) (p)->SetParameters(a,b)
#define IDirectInputEffect_Start(p,a,b) (p)->Start(a,b)
#define IDirectInputEffect_Stop(p) (p)->Stop()
#define IDirectInputEffect_GetEffectStatus(p,a) (p)->GetEffectStatus(a)
#define IDirectInputEffect_Download(p) (p)->Download()
#define IDirectInputEffect_Unload(p) (p)->Unload()
#define IDirectInputEffect_Escape(p,a) (p)->Escape(a)
#endif

#endif /* DIJ_RINGZERO */

#endif /* DIRECTINPUT_VERSION >= 0x0500 */

/****************************************************************************
 *
 *      IDirectInputDevice
 *
 ****************************************************************************/

#define DIDEVTYPE_DEVICE        1
#define DIDEVTYPE_MOUSE         2
#define DIDEVTYPE_KEYBOARD      3
#define DIDEVTYPE_JOYSTICK      4

#if DIRECTINPUT_VERSION > 0x700
#define DI8DEVCLASS_ALL             0
#define DI8DEVCLASS_DEVICE          1
#define DI8DEVCLASS_POINTER         2
#define DI8DEVCLASS_KEYBOARD        3
#define DI8DEVCLASS_GAMECTRL        4

#define DI8DEVTYPE_DEVICE           0x11
#define DI8DEVTYPE_MOUSE            0x12
#define DI8DEVTYPE_KEYBOARD         0x13
#define DI8DEVTYPE_JOYSTICK         0x14
#define DI8DEVTYPE_GAMEPAD          0x15
#define DI8DEVTYPE_DRIVING          0x16
#define DI8DEVTYPE_FLIGHT           0x17
#define DI8DEVTYPE_1STPERSON        0x18
#define DI8DEVTYPE_DEVICECTRL       0x19
#define DI8DEVTYPE_SCREENPOINTER    0x1A
#define DI8DEVTYPE_REMOTE           0x1B
#define DI8DEVTYPE_SUPPLEMENTAL     0x1C
#endif /* DIRECTINPUT_VERSION > 0x700 */

#define DIDEVTYPE_HID           0x00010000

#define DIDEVTYPEMOUSE_UNKNOWN          1
#define DIDEVTYPEMOUSE_TRADITIONAL      2
#define DIDEVTYPEMOUSE_FINGERSTICK      3
#define DIDEVTYPEMOUSE_TOUCHPAD         4
#define DIDEVTYPEMOUSE_TRACKBALL        5

#define DIDEVTYPEKEYBOARD_UNKNOWN       0
#define DIDEVTYPEKEYBOARD_PCXT          1
#define DIDEVTYPEKEYBOARD_OLIVETTI      2
#define DIDEVTYPEKEYBOARD_PCAT          3
#define DIDEVTYPEKEYBOARD_PCENH         4
#define DIDEVTYPEKEYBOARD_NOKIA1050     5
#define DIDEVTYPEKEYBOARD_NOKIA9140     6
#define DIDEVTYPEKEYBOARD_NEC98         7
#define DIDEVTYPEKEYBOARD_NEC98LAPTOP   8
#define DIDEVTYPEKEYBOARD_NEC98106      9
#define DIDEVTYPEKEYBOARD_JAPAN106     10
#define DIDEVTYPEKEYBOARD_JAPANAX      11
#define DIDEVTYPEKEYBOARD_J3100        12

#define DIDEVTYPEJOYSTICK_UNKNOWN       1
#define DIDEVTYPEJOYSTICK_TRADITIONAL   2
#define DIDEVTYPEJOYSTICK_FLIGHTSTICK   3
#define DIDEVTYPEJOYSTICK_GAMEPAD       4
#define DIDEVTYPEJOYSTICK_RUDDER        5
#define DIDEVTYPEJOYSTICK_WHEEL         6
#define DIDEVTYPEJOYSTICK_HEADTRACKER   7

#if DIRECTINPUT_VERSION > 0x700
#define DI8DEVTYPEMOUSE_UNKNOWN                     1
#define DI8DEVTYPEMOUSE_TRADITIONAL                 2
#define DI8DEVTYPEMOUSE_FINGERSTICK                 3
#define DI8DEVTYPEMOUSE_TOUCHPAD                    4
#define DI8DEVTYPEMOUSE_TRACKBALL                   5
#define DI8DEVTYPEMOUSE_ABSOLUTE                    6

#define DI8DEVTYPEKEYBOARD_UNKNOWN                  0
#define DI8DEVTYPEKEYBOARD_PCXT                     1
#define DI8DEVTYPEKEYBOARD_OLIVETTI                 2
#define DI8DEVTYPEKEYBOARD_PCAT                     3
#define DI8DEVTYPEKEYBOARD_PCENH                    4
#define DI8DEVTYPEKEYBOARD_NOKIA1050                5
#define DI8DEVTYPEKEYBOARD_NOKIA9140                6
#define DI8DEVTYPEKEYBOARD_NEC98                    7
#define DI8DEVTYPEKEYBOARD_NEC98LAPTOP              8
#define DI8DEVTYPEKEYBOARD_NEC98106                 9
#define DI8DEVTYPEKEYBOARD_JAPAN106                10
#define DI8DEVTYPEKEYBOARD_JAPANAX                 11
#define DI8DEVTYPEKEYBOARD_J3100                   12

#define DI8DEVTYPE_LIMITEDGAMESUBTYPE               1

#define DI8DEVTYPEJOYSTICK_LIMITED                  DI8DEVTYPE_LIMITEDGAMESUBTYPE
#define DI8DEVTYPEJOYSTICK_STANDARD                 2
#define DI8DEVTYPEJOYSTICK_ENHANCED                 3

#define DI8DEVTYPEGAMEPAD_LIMITED                   DI8DEVTYPE_LIMITEDGAMESUBTYPE
#define DI8DEVTYPEGAMEPAD_STANDARD                  2
#define DI8DEVTYPEGAMEPAD_TILT                      3

#define DI8DEVTYPEDRIVING_LIMITED                   DI8DEVTYPE_LIMITEDGAMESUBTYPE
#define DI8DEVTYPEDRIVING_COMBINEDPEDALS            2
#define DI8DEVTYPEDRIVING_DUALPEDALS                3
#define DI8DEVTYPEDRIVING_THREEPEDALS               4
#define DI8DEVTYPEDRIVING_HANDHELD                  5

#define DI8DEVTYPEFLIGHT_LIMITED                    DI8DEVTYPE_LIMITEDGAMESUBTYPE
#define DI8DEVTYPEFLIGHT_STICK                      2
#define DI8DEVTYPEFLIGHT_YOKE                       3
#define DI8DEVTYPEFLIGHT_RC                         4

#define DI8DEVTYPE1STPERSON_LIMITED                 DI8DEVTYPE_LIMITEDGAMESUBTYPE
#define DI8DEVTYPE1STPERSON_UNKNOWN                 2
#define DI8DEVTYPE1STPERSON_SIXDOF                  3
#define DI8DEVTYPE1STPERSON_SHOOTER                 4

#define DI8DEVTYPESCREENPTR_UNKNOWN                 2
#define DI8DEVTYPESCREENPTR_LIGHTGUN                3
#define DI8DEVTYPESCREENPTR_LIGHTPEN                4
#define DI8DEVTYPESCREENPTR_TOUCH                   5

#define DI8DEVTYPEREMOTE_UNKNOWN                    2

#define DI8DEVTYPEDEVICECTRL_UNKNOWN                2
#define DI8DEVTYPEDEVICECTRL_COMMSSELCTION          3

#define DI8DEVTYPESUPPLEMENTAL_UNKNOWN              2
#define DI8DEVTYPESUPPLEMENTAL_2NDHANDCONTROLLER    3
#define DI8DEVTYPESUPPLEMENTAL_HEADTRACKER          4
#define DI8DEVTYPESUPPLEMENTAL_HANDTRACKER          5
#define DI8DEVTYPESUPPLEMENTAL_SHIFTSTICKGATE       6
#define DI8DEVTYPESUPPLEMENTAL_SHIFTER              7
#define DI8DEVTYPESUPPLEMENTAL_THROTTLE             8
#define DI8DEVTYPESUPPLEMENTAL_SPLITTHROTTLE        9
#define DI8DEVTYPESUPPLEMENTAL_COMBINEDPEDALS      10
#define DI8DEVTYPESUPPLEMENTAL_DUALPEDALS          11
#define DI8DEVTYPESUPPLEMENTAL_THREEPEDALS         12
#define DI8DEVTYPESUPPLEMENTAL_RUDDERPEDALS        13
#endif /* DIRECTINPUT_VERSION > 0x700 */

#define GET_DIDEVICE_TYPE(dwDevType)    LOBYTE(dwDevType)
#define GET_DIDEVICE_SUBTYPE(dwDevType) HIBYTE(dwDevType)

#if(DIRECTINPUT_VERSION >= 0x0500)
/* This structure is defined for DirectX 3.0 compatibility */
typedef struct DIDEVCAPS_DX3 {
    DWORD   dwSize;
    DWORD   dwFlags;
    DWORD   dwDevType;
    DWORD   dwAxes;
    DWORD   dwButtons;
    DWORD   dwPOVs;
} DIDEVCAPS_DX3, *LPDIDEVCAPS_DX3;
#endif /* DIRECTINPUT_VERSION >= 0x0500 */

typedef struct DIDEVCAPS {
    DWORD   dwSize;
    DWORD   dwFlags;
    DWORD   dwDevType;
    DWORD   dwAxes;
    DWORD   dwButtons;
    DWORD   dwPOVs;
#if(DIRECTINPUT_VERSION >= 0x0500)
    DWORD   dwFFSamplePeriod;
    DWORD   dwFFMinTimeResolution;
    DWORD   dwFirmwareRevision;
    DWORD   dwHardwareRevision;
    DWORD   dwFFDriverVersion;
#endif /* DIRECTINPUT_VERSION >= 0x0500 */
} DIDEVCAPS, *LPDIDEVCAPS;


#ifndef DIJ_RINGZERO

typedef struct _DIOBJECTDATAFORMAT {
    const GUID *pguid;
    DWORD   dwOfs;
    DWORD   dwType;
    DWORD   dwFlags;
} DIOBJECTDATAFORMAT, *LPDIOBJECTDATAFORMAT;
typedef const DIOBJECTDATAFORMAT *LPCDIOBJECTDATAFORMAT;

typedef struct _DIDATAFORMAT {
    DWORD   dwSize;
    DWORD   dwObjSize;
    DWORD   dwFlags;
    DWORD   dwDataSize;
    DWORD   dwNumObjs;
    LPDIOBJECTDATAFORMAT rgodf;
} DIDATAFORMAT, *LPDIDATAFORMAT;
typedef const DIDATAFORMAT *LPCDIDATAFORMAT;

#define DIDF_ABSAXIS            0x00000001
#define DIDF_RELAXIS            0x00000002

#ifdef __cplusplus
extern "C" {
#endif
extern const DIDATAFORMAT c_dfDIMouse;

#if(DIRECTINPUT_VERSION >= 0x0700)
extern const DIDATAFORMAT c_dfDIMouse2;
#endif /* DIRECTINPUT_VERSION >= 0x0700 */

extern const DIDATAFORMAT c_dfDIKeyboard;

#if(DIRECTINPUT_VERSION >= 0x0500)
extern const DIDATAFORMAT c_dfDIJoystick;
extern const DIDATAFORMAT c_dfDIJoystick2;
#endif /* DIRECTINPUT_VERSION >= 0x0500 */

#ifdef __cplusplus
};
#endif


#if DIRECTINPUT_VERSION > 0x0700

typedef struct _DIACTIONA {
                UINT_PTR    uAppData;
                DWORD       dwSemantic;
    OPTIONAL    DWORD       dwFlags;
    OPTIONAL    union {
                    LPCSTR      lptszActionName;
                    UINT        uResIdString;
                };
    OPTIONAL    GUID        guidInstance;
    OPTIONAL    DWORD       dwObjID;
    OPTIONAL    DWORD       dwHow;
} DIACTIONA, *LPDIACTIONA ;
typedef struct _DIACTIONW {
                UINT_PTR    uAppData;
                DWORD       dwSemantic;
    OPTIONAL    DWORD       dwFlags;
    OPTIONAL    union {
                    LPCWSTR     lptszActionName;
                    UINT        uResIdString;
                };
    OPTIONAL    GUID        guidInstance;
    OPTIONAL    DWORD       dwObjID;
    OPTIONAL    DWORD       dwHow;
} DIACTIONW, *LPDIACTIONW ;
#ifdef UNICODE
typedef DIACTIONW DIACTION;
typedef LPDIACTIONW LPDIACTION;
#else
typedef DIACTIONA DIACTION;
typedef LPDIACTIONA LPDIACTION;
#endif // UNICODE

typedef const DIACTIONA *LPCDIACTIONA;
typedef const DIACTIONW *LPCDIACTIONW;
#ifdef UNICODE
typedef DIACTIONW DIACTION;
typedef LPCDIACTIONW LPCDIACTION;
#else
typedef DIACTIONA DIACTION;
typedef LPCDIACTIONA LPCDIACTION;
#endif // UNICODE
typedef const DIACTION *LPCDIACTION;


#define DIA_FORCEFEEDBACK       0x00000001
#define DIA_APPMAPPED           0x00000002
#define DIA_APPNOMAP            0x00000004
#define DIA_NORANGE             0x00000008

#define DIAH_UNMAPPED           0x00000000
#define DIAH_USERCONFIG         0x00000001
#define DIAH_APPREQUESTED       0x00000002
#define DIAH_HWAPP              0x00000004
#define DIAH_HWDEFAULT          0x00000008
#define DIAH_OTHERAPP           0x00000010
#define DIAH_DEFAULT            0x00000020
#define DIAH_ERROR              0x80000000

typedef struct _DIACTIONFORMATA {
                DWORD       dwSize;
                DWORD       dwActionSize;
                DWORD       dwDataSize;
                DWORD       dwNumActions;
                LPDIACTIONA rgoAction;
                GUID        guidActionMap;
                DWORD       dwGenre;
                DWORD       dwBufferSize;
    OPTIONAL    LONG        lAxisMin;
    OPTIONAL    LONG        lAxisMax;
    OPTIONAL    HINSTANCE   hInstString;
                FILETIME    ftTimeStamp;
                DWORD       dwCRC;
                CHAR        tszActionMap[MAX_PATH];
} DIACTIONFORMATA, *LPDIACTIONFORMATA;
typedef struct _DIACTIONFORMATW {
                DWORD       dwSize;
                DWORD       dwActionSize;
                DWORD       dwDataSize;
                DWORD       dwNumActions;
                LPDIACTIONW rgoAction;
                GUID        guidActionMap;
                DWORD       dwGenre;
                DWORD       dwBufferSize;
    OPTIONAL    LONG        lAxisMin;
    OPTIONAL    LONG        lAxisMax;
    OPTIONAL    HINSTANCE   hInstString;
                FILETIME    ftTimeStamp;
                DWORD       dwCRC;
                WCHAR       tszActionMap[MAX_PATH];
} DIACTIONFORMATW, *LPDIACTIONFORMATW;
#ifdef UNICODE
typedef DIACTIONFORMATW DIACTIONFORMAT;
typedef LPDIACTIONFORMATW LPDIACTIONFORMAT;
#else
typedef DIACTIONFORMATA DIACTIONFORMAT;
typedef LPDIACTIONFORMATA LPDIACTIONFORMAT;
#endif // UNICODE
typedef const DIACTIONFORMATA *LPCDIACTIONFORMATA;
typedef const DIACTIONFORMATW *LPCDIACTIONFORMATW;
#ifdef UNICODE
typedef DIACTIONFORMATW DIACTIONFORMAT;
typedef LPCDIACTIONFORMATW LPCDIACTIONFORMAT;
#else
typedef DIACTIONFORMATA DIACTIONFORMAT;
typedef LPCDIACTIONFORMATA LPCDIACTIONFORMAT;
#endif // UNICODE
typedef const DIACTIONFORMAT *LPCDIACTIONFORMAT;

#define DIAFTS_NEWDEVICELOW     0xFFFFFFFF
#define DIAFTS_NEWDEVICEHIGH    0xFFFFFFFF
#define DIAFTS_UNUSEDDEVICELOW  0x00000000
#define DIAFTS_UNUSEDDEVICEHIGH 0x00000000

#define DIDBAM_DEFAULT          0x00000000
#define DIDBAM_PRESERVE         0x00000001
#define DIDBAM_INITIALIZE       0x00000002
#define DIDBAM_HWDEFAULTS       0x00000004

#define DIDSAM_DEFAULT          0x00000000
#define DIDSAM_NOUSER           0x00000001
#define DIDSAM_FORCESAVE        0x00000002

#define DICD_DEFAULT            0x00000000
#define DICD_EDIT               0x00000001

/*
 * The following definition is normally defined in d3dtypes.h
 */
#ifndef D3DCOLOR_DEFINED
typedef DWORD D3DCOLOR;
#define D3DCOLOR_DEFINED
#endif

typedef struct _DICOLORSET{
    DWORD dwSize;
    D3DCOLOR cTextFore;
    D3DCOLOR cTextHighlight;
    D3DCOLOR cCalloutLine;
    D3DCOLOR cCalloutHighlight;
    D3DCOLOR cBorder;
    D3DCOLOR cControlFill;
    D3DCOLOR cHighlightFill;
    D3DCOLOR cAreaFill;
} DICOLORSET, *LPDICOLORSET;
typedef const DICOLORSET *LPCDICOLORSET;


typedef struct _DICONFIGUREDEVICESPARAMSA{
     DWORD             dwSize;
     DWORD             dwcUsers;
     LPSTR             lptszUserNames;
     DWORD             dwcFormats;
     LPDIACTIONFORMATA lprgFormats;
     HWND              hwnd;
     DICOLORSET        dics;
     IUnknown FAR *    lpUnkDDSTarget;
} DICONFIGUREDEVICESPARAMSA, *LPDICONFIGUREDEVICESPARAMSA;
typedef struct _DICONFIGUREDEVICESPARAMSW{
     DWORD             dwSize;
     DWORD             dwcUsers;
     LPWSTR            lptszUserNames;
     DWORD             dwcFormats;
     LPDIACTIONFORMATW lprgFormats;
     HWND              hwnd;
     DICOLORSET        dics;
     IUnknown FAR *    lpUnkDDSTarget;
} DICONFIGUREDEVICESPARAMSW, *LPDICONFIGUREDEVICESPARAMSW;
#ifdef UNICODE
typedef DICONFIGUREDEVICESPARAMSW DICONFIGUREDEVICESPARAMS;
typedef LPDICONFIGUREDEVICESPARAMSW LPDICONFIGUREDEVICESPARAMS;
#else
typedef DICONFIGUREDEVICESPARAMSA DICONFIGUREDEVICESPARAMS;
typedef LPDICONFIGUREDEVICESPARAMSA LPDICONFIGUREDEVICESPARAMS;
#endif // UNICODE
typedef const DICONFIGUREDEVICESPARAMSA *LPCDICONFIGUREDEVICESPARAMSA;
typedef const DICONFIGUREDEVICESPARAMSW *LPCDICONFIGUREDEVICESPARAMSW;
#ifdef UNICODE
typedef DICONFIGUREDEVICESPARAMSW DICONFIGUREDEVICESPARAMS;
typedef LPCDICONFIGUREDEVICESPARAMSW LPCDICONFIGUREDEVICESPARAMS;
#else
typedef DICONFIGUREDEVICESPARAMSA DICONFIGUREDEVICESPARAMS;
typedef LPCDICONFIGUREDEVICESPARAMSA LPCDICONFIGUREDEVICESPARAMS;
#endif // UNICODE
typedef const DICONFIGUREDEVICESPARAMS *LPCDICONFIGUREDEVICESPARAMS;


#define DIDIFT_CONFIGURATION    0x00000001
#define DIDIFT_OVERLAY          0x00000002
#define DIDIFT_SELECTION        0x00000004
#define DIDIFT_VIEWSELECT       0x00000008
#define DIDIFT_IMAGE2D_PNG      0x00010000

#define DIDAL_CENTERED      0x00000000
#define DIDAL_LEFTALIGNED   0x00000001
#define DIDAL_RIGHTALIGNED  0x00000002
#define DIDAL_MIDDLE        0x00000000
#define DIDAL_TOPALIGNED    0x00000004
#define DIDAL_BOTTOMALIGNED 0x00000008

typedef struct _DIDEVICEIMAGEINFOA {
    CHAR        tszImagePath[MAX_PATH];
    DWORD       dwFlags; 
    // These are valid if DIDIFT_OVERLAY is present in dwFlags.
    DWORD       dwOverlayOffset;      
    RECT        rcOverlay;             
    DWORD       dwObjID;
    DWORD       dwcValidPts;
    POINT       rgptCalloutLine[5];  
    RECT        rcCalloutRect;  
    DWORD       dwTextAlign;     
} DIDEVICEIMAGEINFOA, *LPDIDEVICEIMAGEINFOA;
typedef struct _DIDEVICEIMAGEINFOW {
    WCHAR       tszImagePath[MAX_PATH];
    DWORD       dwFlags; 
    // These are valid if DIDIFT_OVERLAY is present in dwFlags.
    DWORD       dwOverlayOffset;      
    RECT        rcOverlay;             
    DWORD       dwObjID;
    DWORD       dwcValidPts;
    POINT       rgptCalloutLine[5];  
    RECT        rcCalloutRect;  
    DWORD       dwTextAlign;     
} DIDEVICEIMAGEINFOW, *LPDIDEVICEIMAGEINFOW;
#ifdef UNICODE
typedef DIDEVICEIMAGEINFOW DIDEVICEIMAGEINFO;
typedef LPDIDEVICEIMAGEINFOW LPDIDEVICEIMAGEINFO;
#else
typedef DIDEVICEIMAGEINFOA DIDEVICEIMAGEINFO;
typedef LPDIDEVICEIMAGEINFOA LPDIDEVICEIMAGEINFO;
#endif // UNICODE
typedef const DIDEVICEIMAGEINFOA *LPCDIDEVICEIMAGEINFOA;
typedef const DIDEVICEIMAGEINFOW *LPCDIDEVICEIMAGEINFOW;
#ifdef UNICODE
typedef DIDEVICEIMAGEINFOW DIDEVICEIMAGEINFO;
typedef LPCDIDEVICEIMAGEINFOW LPCDIDEVICEIMAGEINFO;
#else
typedef DIDEVICEIMAGEINFOA DIDEVICEIMAGEINFO;
typedef LPCDIDEVICEIMAGEINFOA LPCDIDEVICEIMAGEINFO;
#endif // UNICODE
typedef const DIDEVICEIMAGEINFO *LPCDIDEVICEIMAGEINFO;

typedef struct _DIDEVICEIMAGEINFOHEADERA {
    DWORD       dwSize;
    DWORD       dwSizeImageInfo;
    DWORD       dwcViews;
    DWORD       dwcButtons;
    DWORD       dwcAxes;
    DWORD       dwcPOVs;
    DWORD       dwBufferSize;
    DWORD       dwBufferUsed;
    LPDIDEVICEIMAGEINFOA lprgImageInfoArray;
} DIDEVICEIMAGEINFOHEADERA, *LPDIDEVICEIMAGEINFOHEADERA;
typedef struct _DIDEVICEIMAGEINFOHEADERW {
    DWORD       dwSize;
    DWORD       dwSizeImageInfo;
    DWORD       dwcViews;
    DWORD       dwcButtons;
    DWORD       dwcAxes;
    DWORD       dwcPOVs;
    DWORD       dwBufferSize;
    DWORD       dwBufferUsed;
    LPDIDEVICEIMAGEINFOW lprgImageInfoArray;
} DIDEVICEIMAGEINFOHEADERW, *LPDIDEVICEIMAGEINFOHEADERW;
#ifdef UNICODE
typedef DIDEVICEIMAGEINFOHEADERW DIDEVICEIMAGEINFOHEADER;
typedef LPDIDEVICEIMAGEINFOHEADERW LPDIDEVICEIMAGEINFOHEADER;
#else
typedef DIDEVICEIMAGEINFOHEADERA DIDEVICEIMAGEINFOHEADER;
typedef LPDIDEVICEIMAGEINFOHEADERA LPDIDEVICEIMAGEINFOHEADER;
#endif // UNICODE
typedef const DIDEVICEIMAGEINFOHEADERA *LPCDIDEVICEIMAGEINFOHEADERA;
typedef const DIDEVICEIMAGEINFOHEADERW *LPCDIDEVICEIMAGEINFOHEADERW;
#ifdef UNICODE
typedef DIDEVICEIMAGEINFOHEADERW DIDEVICEIMAGEINFOHEADER;
typedef LPCDIDEVICEIMAGEINFOHEADERW LPCDIDEVICEIMAGEINFOHEADER;
#else
typedef DIDEVICEIMAGEINFOHEADERA DIDEVICEIMAGEINFOHEADER;
typedef LPCDIDEVICEIMAGEINFOHEADERA LPCDIDEVICEIMAGEINFOHEADER;
#endif // UNICODE
typedef const DIDEVICEIMAGEINFOHEADER *LPCDIDEVICEIMAGEINFOHEADER;

#endif /* DIRECTINPUT_VERSION > 0x0700 */

#if(DIRECTINPUT_VERSION >= 0x0500)
/* These structures are defined for DirectX 3.0 compatibility */

typedef struct DIDEVICEOBJECTINSTANCE_DX3A {
    DWORD   dwSize;
    GUID    guidType;
    DWORD   dwOfs;
    DWORD   dwType;
    DWORD   dwFlags;
    CHAR    tszName[MAX_PATH];
} DIDEVICEOBJECTINSTANCE_DX3A, *LPDIDEVICEOBJECTINSTANCE_DX3A;
typedef struct DIDEVICEOBJECTINSTANCE_DX3W {
    DWORD   dwSize;
    GUID    guidType;
    DWORD   dwOfs;
    DWORD   dwType;
    DWORD   dwFlags;
    WCHAR   tszName[MAX_PATH];
} DIDEVICEOBJECTINSTANCE_DX3W, *LPDIDEVICEOBJECTINSTANCE_DX3W;
#ifdef UNICODE
typedef DIDEVICEOBJECTINSTANCE_DX3W DIDEVICEOBJECTINSTANCE_DX3;
typedef LPDIDEVICEOBJECTINSTANCE_DX3W LPDIDEVICEOBJECTINSTANCE_DX3;
#else
typedef DIDEVICEOBJECTINSTANCE_DX3A DIDEVICEOBJECTINSTANCE_DX3;
typedef LPDIDEVICEOBJECTINSTANCE_DX3A LPDIDEVICEOBJECTINSTANCE_DX3;
#endif // UNICODE
typedef const DIDEVICEOBJECTINSTANCE_DX3A *LPCDIDEVICEOBJECTINSTANCE_DX3A;
typedef const DIDEVICEOBJECTINSTANCE_DX3W *LPCDIDEVICEOBJECTINSTANCE_DX3W;
typedef const DIDEVICEOBJECTINSTANCE_DX3  *LPCDIDEVICEOBJECTINSTANCE_DX3;
#endif /* DIRECTINPUT_VERSION >= 0x0500 */

typedef struct DIDEVICEOBJECTINSTANCEA {
    DWORD   dwSize;
    GUID    guidType;
    DWORD   dwOfs;
    DWORD   dwType;
    DWORD   dwFlags;
    CHAR    tszName[MAX_PATH];
#if(DIRECTINPUT_VERSION >= 0x0500)
    DWORD   dwFFMaxForce;
    DWORD   dwFFForceResolution;
    WORD    wCollectionNumber;
    WORD    wDesignatorIndex;
    WORD    wUsagePage;
    WORD    wUsage;
    DWORD   dwDimension;
    WORD    wExponent;
    WORD    wReportId;
#endif /* DIRECTINPUT_VERSION >= 0x0500 */
} DIDEVICEOBJECTINSTANCEA, *LPDIDEVICEOBJECTINSTANCEA;
typedef struct DIDEVICEOBJECTINSTANCEW {
    DWORD   dwSize;
    GUID    guidType;
    DWORD   dwOfs;
    DWORD   dwType;
    DWORD   dwFlags;
    WCHAR   tszName[MAX_PATH];
#if(DIRECTINPUT_VERSION >= 0x0500)
    DWORD   dwFFMaxForce;
    DWORD   dwFFForceResolution;
    WORD    wCollectionNumber;
    WORD    wDesignatorIndex;
    WORD    wUsagePage;
    WORD    wUsage;
    DWORD   dwDimension;
    WORD    wExponent;
    WORD    wReportId;
#endif /* DIRECTINPUT_VERSION >= 0x0500 */
} DIDEVICEOBJECTINSTANCEW, *LPDIDEVICEOBJECTINSTANCEW;
#ifdef UNICODE
typedef DIDEVICEOBJECTINSTANCEW DIDEVICEOBJECTINSTANCE;
typedef LPDIDEVICEOBJECTINSTANCEW LPDIDEVICEOBJECTINSTANCE;
#else
typedef DIDEVICEOBJECTINSTANCEA DIDEVICEOBJECTINSTANCE;
typedef LPDIDEVICEOBJECTINSTANCEA LPDIDEVICEOBJECTINSTANCE;
#endif // UNICODE
typedef const DIDEVICEOBJECTINSTANCEA *LPCDIDEVICEOBJECTINSTANCEA;
typedef const DIDEVICEOBJECTINSTANCEW *LPCDIDEVICEOBJECTINSTANCEW;
typedef const DIDEVICEOBJECTINSTANCE  *LPCDIDEVICEOBJECTINSTANCE;

typedef BOOL (FAR PASCAL * LPDIENUMDEVICEOBJECTSCALLBACKA)(LPCDIDEVICEOBJECTINSTANCEA, LPVOID);
typedef BOOL (FAR PASCAL * LPDIENUMDEVICEOBJECTSCALLBACKW)(LPCDIDEVICEOBJECTINSTANCEW, LPVOID);
#ifdef UNICODE
#define LPDIENUMDEVICEOBJECTSCALLBACK  LPDIENUMDEVICEOBJECTSCALLBACKW
#else
#define LPDIENUMDEVICEOBJECTSCALLBACK  LPDIENUMDEVICEOBJECTSCALLBACKA
#endif // !UNICODE

#if(DIRECTINPUT_VERSION >= 0x0500)
#define DIDOI_FFACTUATOR        0x00000001
#define DIDOI_FFEFFECTTRIGGER   0x00000002
#define DIDOI_POLLED            0x00008000
#define DIDOI_ASPECTPOSITION    0x00000100
#define DIDOI_ASPECTVELOCITY    0x00000200
#define DIDOI_ASPECTACCEL       0x00000300
#define DIDOI_ASPECTFORCE       0x00000400
#define DIDOI_ASPECTMASK        0x00000F00
#endif /* DIRECTINPUT_VERSION >= 0x0500 */
#if(DIRECTINPUT_VERSION >= 0x050a)
#define DIDOI_GUIDISUSAGE       0x00010000
#endif /* DIRECTINPUT_VERSION >= 0x050a */

typedef struct DIPROPHEADER {
    DWORD   dwSize;
    DWORD   dwHeaderSize;
    DWORD   dwObj;
    DWORD   dwHow;
} DIPROPHEADER, *LPDIPROPHEADER;
typedef const DIPROPHEADER *LPCDIPROPHEADER;

#define DIPH_DEVICE             0
#define DIPH_BYOFFSET           1
#define DIPH_BYID               2

typedef struct DIPROPDWORD {
    DIPROPHEADER diph;
    DWORD   dwData;
} DIPROPDWORD, *LPDIPROPDWORD;
typedef const DIPROPDWORD *LPCDIPROPDWORD;

#ifdef __cplusplus
#define MAKEDIPROP(prop)    (*(const GUID *)(prop))
#else
#define MAKEDIPROP(prop)    ((REFGUID)(prop))
#endif
#define DIPROP_GRANULARITY      MAKEDIPROP(3)

#define DIPROP_PLAYERSLOT                 MAKEDIPROP(128)
#define DIPROP_AUTOPOLL                   MAKEDIPROP(129)

typedef struct DIDEVICEOBJECTDATA_DX3 {
    DWORD       dwOfs;
    DWORD       dwData;
    DWORD       dwTimeStamp;
    DWORD       dwSequence;
} DIDEVICEOBJECTDATA_DX3, *LPDIDEVICEOBJECTDATA_DX3;
typedef const DIDEVICEOBJECTDATA_DX3 *LPCDIDEVICEOBJECTDATA_DX;

typedef struct DIDEVICEOBJECTDATA {
    DWORD       dwOfs;
    DWORD       dwData;
    DWORD       dwTimeStamp;
    DWORD       dwSequence;
#if(DIRECTINPUT_VERSION >= 0x0701)
    UINT_PTR    uAppData;
#endif /* DIRECTINPUT_VERSION >= 0x0701 */
} DIDEVICEOBJECTDATA, *LPDIDEVICEOBJECTDATA;
typedef const DIDEVICEOBJECTDATA *LPCDIDEVICEOBJECTDATA;

#define DIGDD_PEEK          0x00000001

#define DISEQUENCE_COMPARE(dwSequence1, cmp, dwSequence2) \
                        ((int)((dwSequence1) - (dwSequence2)) cmp 0)
#define DISCL_EXCLUSIVE     0x00000001
#define DISCL_NONEXCLUSIVE  0x00000002
#define DISCL_FOREGROUND    0x00000004
#define DISCL_BACKGROUND    0x00000008
#define DISCL_NOWINKEY      0x00000010

#if(DIRECTINPUT_VERSION >= 0x0500)
/* These structures are defined for DirectX 3.0 compatibility */

typedef struct DIDEVICEINSTANCE_DX3A {
    DWORD   dwSize;
    GUID    guidInstance;
    GUID    guidProduct;
    DWORD   dwDevType;
    CHAR    tszInstanceName[MAX_PATH];
    CHAR    tszProductName[MAX_PATH];
} DIDEVICEINSTANCE_DX3A, *LPDIDEVICEINSTANCE_DX3A;
typedef struct DIDEVICEINSTANCE_DX3W {
    DWORD   dwSize;
    GUID    guidInstance;
    GUID    guidProduct;
    DWORD   dwDevType;
    WCHAR   tszInstanceName[MAX_PATH];
    WCHAR   tszProductName[MAX_PATH];
} DIDEVICEINSTANCE_DX3W, *LPDIDEVICEINSTANCE_DX3W;
#ifdef UNICODE
typedef DIDEVICEINSTANCE_DX3W DIDEVICEINSTANCE_DX3;
typedef LPDIDEVICEINSTANCE_DX3W LPDIDEVICEINSTANCE_DX3;
#else
typedef DIDEVICEINSTANCE_DX3A DIDEVICEINSTANCE_DX3;
typedef LPDIDEVICEINSTANCE_DX3A LPDIDEVICEINSTANCE_DX3;
#endif // UNICODE
typedef const DIDEVICEINSTANCE_DX3A *LPCDIDEVICEINSTANCE_DX3A;
typedef const DIDEVICEINSTANCE_DX3W *LPCDIDEVICEINSTANCE_DX3W;
typedef const DIDEVICEINSTANCE_DX3  *LPCDIDEVICEINSTANCE_DX3;
#endif /* DIRECTINPUT_VERSION >= 0x0500 */

typedef struct DIDEVICEINSTANCEA {
    DWORD   dwSize;
    GUID    guidInstance;
    GUID    guidProduct;
    DWORD   dwDevType;
    CHAR    tszInstanceName[MAX_PATH];
    CHAR    tszProductName[MAX_PATH];
#if(DIRECTINPUT_VERSION >= 0x0500)
    GUID    guidFFDriver;
    WORD    wUsagePage;
    WORD    wUsage;
#endif /* DIRECTINPUT_VERSION >= 0x0500 */
} DIDEVICEINSTANCEA, *LPDIDEVICEINSTANCEA;
typedef struct DIDEVICEINSTANCEW {
    DWORD   dwSize;
    GUID    guidInstance;
    GUID    guidProduct;
    DWORD   dwDevType;
    WCHAR   tszInstanceName[MAX_PATH];
    WCHAR   tszProductName[MAX_PATH];
#if(DIRECTINPUT_VERSION >= 0x0500)
    GUID    guidFFDriver;
    WORD    wUsagePage;
    WORD    wUsage;
#endif /* DIRECTINPUT_VERSION >= 0x0500 */
} DIDEVICEINSTANCEW, *LPDIDEVICEINSTANCEW;
#ifdef UNICODE
typedef DIDEVICEINSTANCEW DIDEVICEINSTANCE;
typedef LPDIDEVICEINSTANCEW LPDIDEVICEINSTANCE;
#else
typedef DIDEVICEINSTANCEA DIDEVICEINSTANCE;
typedef LPDIDEVICEINSTANCEA LPDIDEVICEINSTANCE;
#endif // UNICODE

typedef const DIDEVICEINSTANCEA *LPCDIDEVICEINSTANCEA;
typedef const DIDEVICEINSTANCEW *LPCDIDEVICEINSTANCEW;
#ifdef UNICODE
typedef DIDEVICEINSTANCEW DIDEVICEINSTANCE;
typedef LPCDIDEVICEINSTANCEW LPCDIDEVICEINSTANCE;
#else
typedef DIDEVICEINSTANCEA DIDEVICEINSTANCE;
typedef LPCDIDEVICEINSTANCEA LPCDIDEVICEINSTANCE;
#endif // UNICODE
typedef const DIDEVICEINSTANCE  *LPCDIDEVICEINSTANCE;

#undef INTERFACE
#define INTERFACE IDirectInputDeviceW

DECLARE_INTERFACE_(IDirectInputDeviceW, IUnknown)
{
    /*** IUnknown methods ***/
    STDMETHOD(QueryInterface)(THIS_ REFIID riid, LPVOID * ppvObj) PURE;
    STDMETHOD_(ULONG,AddRef)(THIS) PURE;
    STDMETHOD_(ULONG,Release)(THIS) PURE;

    /*** IDirectInputDeviceW methods ***/
    STDMETHOD(GetCapabilities)(THIS_ LPDIDEVCAPS) PURE;
    STDMETHOD(EnumObjects)(THIS_ LPDIENUMDEVICEOBJECTSCALLBACKW,LPVOID,DWORD) PURE;
    STDMETHOD(GetProperty)(THIS_ REFGUID,LPDIPROPHEADER) PURE;
    STDMETHOD(SetProperty)(THIS_ REFGUID,LPCDIPROPHEADER) PURE;
    STDMETHOD(Acquire)(THIS) PURE;
    STDMETHOD(Unacquire)(THIS) PURE;
    STDMETHOD(GetDeviceState)(THIS_ DWORD,LPVOID) PURE;
    STDMETHOD(GetDeviceData)(THIS_ DWORD,LPDIDEVICEOBJECTDATA,LPDWORD,DWORD) PURE;
    STDMETHOD(SetDataFormat)(THIS_ LPCDIDATAFORMAT) PURE;
    STDMETHOD(SetEventNotification)(THIS_ HANDLE) PURE;
    STDMETHOD(SetCooperativeLevel)(THIS_ HWND,DWORD) PURE;
    STDMETHOD(GetObjectInfo)(THIS_ LPDIDEVICEOBJECTINSTANCEW,DWORD,DWORD) PURE;
    STDMETHOD(GetDeviceInfo)(THIS_ LPDIDEVICEINSTANCEW) PURE;
    STDMETHOD(RunControlPanel)(THIS_ HWND,DWORD) PURE;
    STDMETHOD(Initialize)(THIS_ HINSTANCE,DWORD,REFGUID) PURE;
};

typedef struct IDirectInputDeviceW *LPDIRECTINPUTDEVICEW;

#undef INTERFACE
#define INTERFACE IDirectInputDeviceA

DECLARE_INTERFACE_(IDirectInputDeviceA, IUnknown)
{
    /*** IUnknown methods ***/
    STDMETHOD(QueryInterface)(THIS_ REFIID riid, LPVOID * ppvObj) PURE;
    STDMETHOD_(ULONG,AddRef)(THIS) PURE;
    STDMETHOD_(ULONG,Release)(THIS) PURE;

    /*** IDirectInputDeviceA methods ***/
    STDMETHOD(GetCapabilities)(THIS_ LPDIDEVCAPS) PURE;
    STDMETHOD(EnumObjects)(THIS_ LPDIENUMDEVICEOBJECTSCALLBACKA,LPVOID,DWORD) PURE;
    STDMETHOD(GetProperty)(THIS_ REFGUID,LPDIPROPHEADER) PURE;
    STDMETHOD(SetProperty)(THIS_ REFGUID,LPCDIPROPHEADER) PURE;
    STDMETHOD(Acquire)(THIS) PURE;
    STDMETHOD(Unacquire)(THIS) PURE;
    STDMETHOD(GetDeviceState)(THIS_ DWORD,LPVOID) PURE;
    STDMETHOD(GetDeviceData)(THIS_ DWORD,LPDIDEVICEOBJECTDATA,LPDWORD,DWORD) PURE;
    STDMETHOD(SetDataFormat)(THIS_ LPCDIDATAFORMAT) PURE;
    STDMETHOD(SetEventNotification)(THIS_ HANDLE) PURE;
    STDMETHOD(SetCooperativeLevel)(THIS_ HWND,DWORD) PURE;
    STDMETHOD(GetObjectInfo)(THIS_ LPDIDEVICEOBJECTINSTANCEA,DWORD,DWORD) PURE;
    STDMETHOD(GetDeviceInfo)(THIS_ LPDIDEVICEINSTANCEA) PURE;
    STDMETHOD(RunControlPanel)(THIS_ HWND,DWORD) PURE;
    STDMETHOD(Initialize)(THIS_ HINSTANCE,DWORD,REFGUID) PURE;
};

typedef struct IDirectInputDeviceA *LPDIRECTINPUTDEVICEA;

#ifdef UNICODE
#define IID_IDirectInputDevice IID_IDirectInputDeviceW
#define IDirectInputDevice IDirectInputDeviceW
#define IDirectInputDeviceVtbl IDirectInputDeviceWVtbl
#else
#define IID_IDirectInputDevice IID_IDirectInputDeviceA
#define IDirectInputDevice IDirectInputDeviceA
#define IDirectInputDeviceVtbl IDirectInputDeviceAVtbl
#endif
typedef struct IDirectInputDevice *LPDIRECTINPUTDEVICE;

#if !defined(__cplusplus) || defined(CINTERFACE)
#define IDirectInputDevice_QueryInterface(p,a,b) (p)->lpVtbl->QueryInterface(p,a,b)
#define IDirectInputDevice_AddRef(p) (p)->lpVtbl->AddRef(p)
#define IDirectInputDevice_Release(p) (p)->lpVtbl->Release(p)
#define IDirectInputDevice_GetCapabilities(p,a) (p)->lpVtbl->GetCapabilities(p,a)
#define IDirectInputDevice_EnumObjects(p,a,b,c) (p)->lpVtbl->EnumObjects(p,a,b,c)
#define IDirectInputDevice_GetProperty(p,a,b) (p)->lpVtbl->GetProperty(p,a,b)
#define IDirectInputDevice_SetProperty(p,a,b) (p)->lpVtbl->SetProperty(p,a,b)
#define IDirectInputDevice_Acquire(p) (p)->lpVtbl->Acquire(p)
#define IDirectInputDevice_Unacquire(p) (p)->lpVtbl->Unacquire(p)
#define IDirectInputDevice_GetDeviceState(p,a,b) (p)->lpVtbl->GetDeviceState(p,a,b)
#define IDirectInputDevice_GetDeviceData(p,a,b,c,d) (p)->lpVtbl->GetDeviceData(p,a,b,c,d)
#define IDirectInputDevice_SetDataFormat(p,a) (p)->lpVtbl->SetDataFormat(p,a)
#define IDirectInputDevice_SetEventNotification(p,a) (p)->lpVtbl->SetEventNotification(p,a)
#define IDirectInputDevice_SetCooperativeLevel(p,a,b) (p)->lpVtbl->SetCooperativeLevel(p,a,b)
#define IDirectInputDevice_GetObjectInfo(p,a,b,c) (p)->lpVtbl->GetObjectInfo(p,a,b,c)
#define IDirectInputDevice_GetDeviceInfo(p,a) (p)->lpVtbl->GetDeviceInfo(p,a)
#define IDirectInputDevice_RunControlPanel(p,a,b) (p)->lpVtbl->RunControlPanel(p,a,b)
#define IDirectInputDevice_Initialize(p,a,b,c) (p)->lpVtbl->Initialize(p,a,b,c)
#else
#define IDirectInputDevice_QueryInterface(p,a,b) (p)->QueryInterface(a,b)
#define IDirectInputDevice_AddRef(p) (p)->AddRef()
#define IDirectInputDevice_Release(p) (p)->Release()
#define IDirectInputDevice_GetCapabilities(p,a) (p)->GetCapabilities(a)
#define IDirectInputDevice_EnumObjects(p,a,b,c) (p)->EnumObjects(a,b,c)
#define IDirectInputDevice_GetProperty(p,a,b) (p)->GetProperty(a,b)
#define IDirectInputDevice_SetProperty(p,a,b) (p)->SetProperty(a,b)
#define IDirectInputDevice_Acquire(p) (p)->Acquire()
#define IDirectInputDevice_Unacquire(p) (p)->Unacquire()
#define IDirectInputDevice_GetDeviceState(p,a,b) (p)->GetDeviceState(a,b)
#define IDirectInputDevice_GetDeviceData(p,a,b,c,d) (p)->GetDeviceData(a,b,c,d)
#define IDirectInputDevice_SetDataFormat(p,a) (p)->SetDataFormat(a)
#define IDirectInputDevice_SetEventNotification(p,a) (p)->SetEventNotification(a)
#define IDirectInputDevice_SetCooperativeLevel(p,a,b) (p)->SetCooperativeLevel(a,b)
#define IDirectInputDevice_GetObjectInfo(p,a,b,c) (p)->GetObjectInfo(a,b,c)
#define IDirectInputDevice_GetDeviceInfo(p,a) (p)->GetDeviceInfo(a)
#define IDirectInputDevice_RunControlPanel(p,a,b) (p)->RunControlPanel(a,b)
#define IDirectInputDevice_Initialize(p,a,b,c) (p)->Initialize(a,b,c)
#endif

#endif /* DIJ_RINGZERO */


#if(DIRECTINPUT_VERSION >= 0x0500)

#define DISFFC_RESET            0x00000001
#define DISFFC_STOPALL          0x00000002
#define DISFFC_PAUSE            0x00000004
#define DISFFC_CONTINUE         0x00000008
#define DISFFC_SETACTUATORSON   0x00000010
#define DISFFC_SETACTUATORSOFF  0x00000020

#define DIGFFS_EMPTY            0x00000001
#define DIGFFS_STOPPED          0x00000002
#define DIGFFS_PAUSED           0x00000004
#define DIGFFS_ACTUATORSON      0x00000010
#define DIGFFS_ACTUATORSOFF     0x00000020
#define DIGFFS_POWERON          0x00000040
#define DIGFFS_POWEROFF         0x00000080
#define DIGFFS_SAFETYSWITCHON   0x00000100
#define DIGFFS_SAFETYSWITCHOFF  0x00000200
#define DIGFFS_USERFFSWITCHON   0x00000400
#define DIGFFS_USERFFSWITCHOFF  0x00000800
#define DIGFFS_DEVICELOST       0x80000000

#ifndef DIJ_RINGZERO

typedef struct DIEFFECTINFOA {
    DWORD   dwSize;
    GUID    guid;
    DWORD   dwEffType;
    DWORD   dwStaticParams;
    DWORD   dwDynamicParams;
    CHAR    tszName[MAX_PATH];
} DIEFFECTINFOA, *LPDIEFFECTINFOA;
typedef struct DIEFFECTINFOW {
    DWORD   dwSize;
    GUID    guid;
    DWORD   dwEffType;
    DWORD   dwStaticParams;
    DWORD   dwDynamicParams;
    WCHAR   tszName[MAX_PATH];
} DIEFFECTINFOW, *LPDIEFFECTINFOW;
#ifdef UNICODE
typedef DIEFFECTINFOW DIEFFECTINFO;
typedef LPDIEFFECTINFOW LPDIEFFECTINFO;
#else
typedef DIEFFECTINFOA DIEFFECTINFO;
typedef LPDIEFFECTINFOA LPDIEFFECTINFO;
#endif // UNICODE
typedef const DIEFFECTINFOA *LPCDIEFFECTINFOA;
typedef const DIEFFECTINFOW *LPCDIEFFECTINFOW;
typedef const DIEFFECTINFO  *LPCDIEFFECTINFO;

#define DISDD_CONTINUE          0x00000001

typedef BOOL (FAR PASCAL * LPDIENUMEFFECTSCALLBACKA)(LPCDIEFFECTINFOA, LPVOID);
typedef BOOL (FAR PASCAL * LPDIENUMEFFECTSCALLBACKW)(LPCDIEFFECTINFOW, LPVOID);
#ifdef UNICODE
#define LPDIENUMEFFECTSCALLBACK  LPDIENUMEFFECTSCALLBACKW
#else
#define LPDIENUMEFFECTSCALLBACK  LPDIENUMEFFECTSCALLBACKA
#endif // !UNICODE
typedef BOOL (FAR PASCAL * LPDIENUMCREATEDEFFECTOBJECTSCALLBACK)(LPDIRECTINPUTEFFECT, LPVOID);

#undef INTERFACE
#define INTERFACE IDirectInputDevice2W

DECLARE_INTERFACE_(IDirectInputDevice2W, IDirectInputDeviceW)
{
    /*** IUnknown methods ***/
    STDMETHOD(QueryInterface)(THIS_ REFIID riid, LPVOID * ppvObj) PURE;
    STDMETHOD_(ULONG,AddRef)(THIS) PURE;
    STDMETHOD_(ULONG,Release)(THIS) PURE;

    /*** IDirectInputDeviceW methods ***/
    STDMETHOD(GetCapabilities)(THIS_ LPDIDEVCAPS) PURE;
    STDMETHOD(EnumObjects)(THIS_ LPDIENUMDEVICEOBJECTSCALLBACKW,LPVOID,DWORD) PURE;
    STDMETHOD(GetProperty)(THIS_ REFGUID,LPDIPROPHEADER) PURE;
    STDMETHOD(SetProperty)(THIS_ REFGUID,LPCDIPROPHEADER) PURE;
    STDMETHOD(Acquire)(THIS) PURE;
    STDMETHOD(Unacquire)(THIS) PURE;
    STDMETHOD(GetDeviceState)(THIS_ DWORD,LPVOID) PURE;
    STDMETHOD(GetDeviceData)(THIS_ DWORD,LPDIDEVICEOBJECTDATA,LPDWORD,DWORD) PURE;
    STDMETHOD(SetDataFormat)(THIS_ LPCDIDATAFORMAT) PURE;
    STDMETHOD(SetEventNotification)(THIS_ HANDLE) PURE;
    STDMETHOD(SetCooperativeLevel)(THIS_ HWND,DWORD) PURE;
    STDMETHOD(GetObjectInfo)(THIS_ LPDIDEVICEOBJECTINSTANCEW,DWORD,DWORD) PURE;
    STDMETHOD(GetDeviceInfo)(THIS_ LPDIDEVICEINSTANCEW) PURE;
    STDMETHOD(RunControlPanel)(THIS_ HWND,DWORD) PURE;
    STDMETHOD(Initialize)(THIS_ HINSTANCE,DWORD,REFGUID) PURE;

    /*** IDirectInputDevice2W methods ***/
    STDMETHOD(CreateEffect)(THIS_ REFGUID,LPCDIEFFECT,LPDIRECTINPUTEFFECT *,LPUNKNOWN) PURE;
    STDMETHOD(EnumEffects)(THIS_ LPDIENUMEFFECTSCALLBACKW,LPVOID,DWORD) PURE;
    STDMETHOD(GetEffectInfo)(THIS_ LPDIEFFECTINFOW,REFGUID) PURE;
    STDMETHOD(GetForceFeedbackState)(THIS_ LPDWORD) PURE;
    STDMETHOD(SendForceFeedbackCommand)(THIS_ DWORD) PURE;
    STDMETHOD(EnumCreatedEffectObjects)(THIS_ LPDIENUMCREATEDEFFECTOBJECTSCALLBACK,LPVOID,DWORD) PURE;
    STDMETHOD(Escape)(THIS_ LPDIEFFESCAPE) PURE;
    STDMETHOD(Poll)(THIS) PURE;
    STDMETHOD(SendDeviceData)(THIS_ DWORD,LPCDIDEVICEOBJECTDATA,LPDWORD,DWORD) PURE;
};

typedef struct IDirectInputDevice2W *LPDIRECTINPUTDEVICE2W;

#undef INTERFACE
#define INTERFACE IDirectInputDevice2A

DECLARE_INTERFACE_(IDirectInputDevice2A, IDirectInputDeviceA)
{
    /*** IUnknown methods ***/
    STDMETHOD(QueryInterface)(THIS_ REFIID riid, LPVOID * ppvObj) PURE;
    STDMETHOD_(ULONG,AddRef)(THIS) PURE;
    STDMETHOD_(ULONG,Release)(THIS) PURE;

    /*** IDirectInputDeviceA methods ***/
    STDMETHOD(GetCapabilities)(THIS_ LPDIDEVCAPS) PURE;
    STDMETHOD(EnumObjects)(THIS_ LPDIENUMDEVICEOBJECTSCALLBACKA,LPVOID,DWORD) PURE;
    STDMETHOD(GetProperty)(THIS_ REFGUID,LPDIPROPHEADER) PURE;
    STDMETHOD(SetProperty)(THIS_ REFGUID,LPCDIPROPHEADER) PURE;
    STDMETHOD(Acquire)(THIS) PURE;
    STDMETHOD(Unacquire)(THIS) PURE;
    STDMETHOD(GetDeviceState)(THIS_ DWORD,LPVOID) PURE;
    STDMETHOD(GetDeviceData)(THIS_ DWORD,LPDIDEVICEOBJECTDATA,LPDWORD,DWORD) PURE;
    STDMETHOD(SetDataFormat)(THIS_ LPCDIDATAFORMAT) PURE;
    STDMETHOD(SetEventNotification)(THIS_ HANDLE) PURE;
    STDMETHOD(SetCooperativeLevel)(THIS_ HWND,DWORD) PURE;
    STDMETHOD(GetObjectInfo)(THIS_ LPDIDEVICEOBJECTINSTANCEA,DWORD,DWORD) PURE;
    STDMETHOD(GetDeviceInfo)(THIS_ LPDIDEVICEINSTANCEA) PURE;
    STDMETHOD(RunControlPanel)(THIS_ HWND,DWORD) PURE;
    STDMETHOD(Initialize)(THIS_ HINSTANCE,DWORD,REFGUID) PURE;

    /*** IDirectInputDevice2A methods ***/
    STDMETHOD(CreateEffect)(THIS_ REFGUID,LPCDIEFFECT,LPDIRECTINPUTEFFECT *,LPUNKNOWN) PURE;
    STDMETHOD(EnumEffects)(THIS_ LPDIENUMEFFECTSCALLBACKA,LPVOID,DWORD) PURE;
    STDMETHOD(GetEffectInfo)(THIS_ LPDIEFFECTINFOA,REFGUID) PURE;
    STDMETHOD(GetForceFeedbackState)(THIS_ LPDWORD) PURE;
    STDMETHOD(SendForceFeedbackCommand)(THIS_ DWORD) PURE;
    STDMETHOD(EnumCreatedEffectObjects)(THIS_ LPDIENUMCREATEDEFFECTOBJECTSCALLBACK,LPVOID,DWORD) PURE;
    STDMETHOD(Escape)(THIS_ LPDIEFFESCAPE) PURE;
    STDMETHOD(Poll)(THIS) PURE;
    STDMETHOD(SendDeviceData)(THIS_ DWORD,LPCDIDEVICEOBJECTDATA,LPDWORD,DWORD) PURE;
};

typedef struct IDirectInputDevice2A *LPDIRECTINPUTDEVICE2A;

#ifdef UNICODE
#define IID_IDirectInputDevice2 IID_IDirectInputDevice2W
#define IDirectInputDevice2 IDirectInputDevice2W
#define IDirectInputDevice2Vtbl IDirectInputDevice2WVtbl
#else
#define IID_IDirectInputDevice2 IID_IDirectInputDevice2A
#define IDirectInputDevice2 IDirectInputDevice2A
#define IDirectInputDevice2Vtbl IDirectInputDevice2AVtbl
#endif
typedef struct IDirectInputDevice2 *LPDIRECTINPUTDEVICE2;

#if !defined(__cplusplus) || defined(CINTERFACE)
#define IDirectInputDevice2_QueryInterface(p,a,b) (p)->lpVtbl->QueryInterface(p,a,b)
#define IDirectInputDevice2_AddRef(p) (p)->lpVtbl->AddRef(p)
#define IDirectInputDevice2_Release(p) (p)->lpVtbl->Release(p)
#define IDirectInputDevice2_GetCapabilities(p,a) (p)->lpVtbl->GetCapabilities(p,a)
#define IDirectInputDevice2_EnumObjects(p,a,b,c) (p)->lpVtbl->EnumObjects(p,a,b,c)
#define IDirectInputDevice2_GetProperty(p,a,b) (p)->lpVtbl->GetProperty(p,a,b)
#define IDirectInputDevice2_SetProperty(p,a,b) (p)->lpVtbl->SetProperty(p,a,b)
#define IDirectInputDevice2_Acquire(p) (p)->lpVtbl->Acquire(p)
#define IDirectInputDevice2_Unacquire(p) (p)->lpVtbl->Unacquire(p)
#define IDirectInputDevice2_GetDeviceState(p,a,b) (p)->lpVtbl->GetDeviceState(p,a,b)
#define IDirectInputDevice2_GetDeviceData(p,a,b,c,d) (p)->lpVtbl->GetDeviceData(p,a,b,c,d)
#define IDirectInputDevice2_SetDataFormat(p,a) (p)->lpVtbl->SetDataFormat(p,a)
#define IDirectInputDevice2_SetEventNotification(p,a) (p)->lpVtbl->SetEventNotification(p,a)
#define IDirectInputDevice2_SetCooperativeLevel(p,a,b) (p)->lpVtbl->SetCooperativeLevel(p,a,b)
#define IDirectInputDevice2_GetObjectInfo(p,a,b,c) (p)->lpVtbl->GetObjectInfo(p,a,b,c)
#define IDirectInputDevice2_GetDeviceInfo(p,a) (p)->lpVtbl->GetDeviceInfo(p,a)
#define IDirectInputDevice2_RunControlPanel(p,a,b) (p)->lpVtbl->RunControlPanel(p,a,b)
#define IDirectInputDevice2_Initialize(p,a,b,c) (p)->lpVtbl->Initialize(p,a,b,c)
#define IDirectInputDevice2_CreateEffect(p,a,b,c,d) (p)->lpVtbl->CreateEffect(p,a,b,c,d)
#define IDirectInputDevice2_EnumEffects(p,a,b,c) (p)->lpVtbl->EnumEffects(p,a,b,c)
#define IDirectInputDevice2_GetEffectInfo(p,a,b) (p)->lpVtbl->GetEffectInfo(p,a,b)
#define IDirectInputDevice2_GetForceFeedbackState(p,a) (p)->lpVtbl->GetForceFeedbackState(p,a)
#define IDirectInputDevice2_SendForceFeedbackCommand(p,a) (p)->lpVtbl->SendForceFeedbackCommand(p,a)
#define IDirectInputDevice2_EnumCreatedEffectObjects(p,a,b,c) (p)->lpVtbl->EnumCreatedEffectObjects(p,a,b,c)
#define IDirectInputDevice2_Escape(p,a) (p)->lpVtbl->Escape(p,a)
#define IDirectInputDevice2_Poll(p) (p)->lpVtbl->Poll(p)
#define IDirectInputDevice2_SendDeviceData(p,a,b,c,d) (p)->lpVtbl->SendDeviceData(p,a,b,c,d)
#else
#define IDirectInputDevice2_QueryInterface(p,a,b) (p)->QueryInterface(a,b)
#define IDirectInputDevice2_AddRef(p) (p)->AddRef()
#define IDirectInputDevice2_Release(p) (p)->Release()
#define IDirectInputDevice2_GetCapabilities(p,a) (p)->GetCapabilities(a)
#define IDirectInputDevice2_EnumObjects(p,a,b,c) (p)->EnumObjects(a,b,c)
#define IDirectInputDevice2_GetProperty(p,a,b) (p)->GetProperty(a,b)
#define IDirectInputDevice2_SetProperty(p,a,b) (p)->SetProperty(a,b)
#define IDirectInputDevice2_Acquire(p) (p)->Acquire()
#define IDirectInputDevice2_Unacquire(p) (p)->Unacquire()
#define IDirectInputDevice2_GetDeviceState(p,a,b) (p)->GetDeviceState(a,b)
#define IDirectInputDevice2_GetDeviceData(p,a,b,c,d) (p)->GetDeviceData(a,b,c,d)
#define IDirectInputDevice2_SetDataFormat(p,a) (p)->SetDataFormat(a)
#define IDirectInputDevice2_SetEventNotification(p,a) (p)->SetEventNotification(a)
#define IDirectInputDevice2_SetCooperativeLevel(p,a,b) (p)->SetCooperativeLevel(a,b)
#define IDirectInputDevice2_GetObjectInfo(p,a,b,c) (p)->GetObjectInfo(a,b,c)
#define IDirectInputDevice2_GetDeviceInfo(p,a) (p)->GetDeviceInfo(a)
#define IDirectInputDevice2_RunControlPanel(p,a,b) (p)->RunControlPanel(a,b)
#define IDirectInputDevice2_Initialize(p,a,b,c) (p)->Initialize(a,b,c)
#define IDirectInputDevice2_CreateEffect(p,a,b,c,d) (p)->CreateEffect(a,b,c,d)
#define IDirectInputDevice2_EnumEffects(p,a,b,c) (p)->EnumEffects(a,b,c)
#define IDirectInputDevice2_GetEffectInfo(p,a,b) (p)->GetEffectInfo(a,b)
#define IDirectInputDevice2_GetForceFeedbackState(p,a) (p)->GetForceFeedbackState(a)
#define IDirectInputDevice2_SendForceFeedbackCommand(p,a) (p)->SendForceFeedbackCommand(a)
#define IDirectInputDevice2_EnumCreatedEffectObjects(p,a,b,c) (p)->EnumCreatedEffectObjects(a,b,c)
#define IDirectInputDevice2_Escape(p,a) (p)->Escape(a)
#define IDirectInputDevice2_Poll(p) (p)->Poll()
#define IDirectInputDevice2_SendDeviceData(p,a,b,c,d) (p)->SendDeviceData(a,b,c,d)
#endif

#endif /* DIJ_RINGZERO */

#endif /* DIRECTINPUT_VERSION >= 0x0500 */

#if(DIRECTINPUT_VERSION >= 0x0700)
#define DIFEF_DEFAULT               0x00000000
#define DIFEF_INCLUDENONSTANDARD    0x00000001
#define DIFEF_MODIFYIFNEEDED            0x00000010

#ifndef DIJ_RINGZERO

#undef INTERFACE
#define INTERFACE IDirectInputDevice7W

DECLARE_INTERFACE_(IDirectInputDevice7W, IDirectInputDevice2W)
{
    /*** IUnknown methods ***/
    STDMETHOD(QueryInterface)(THIS_ REFIID riid, LPVOID * ppvObj) PURE;
    STDMETHOD_(ULONG,AddRef)(THIS) PURE;
    STDMETHOD_(ULONG,Release)(THIS) PURE;

    /*** IDirectInputDevice2W methods ***/
    STDMETHOD(GetCapabilities)(THIS_ LPDIDEVCAPS) PURE;
    STDMETHOD(EnumObjects)(THIS_ LPDIENUMDEVICEOBJECTSCALLBACKW,LPVOID,DWORD) PURE;
    STDMETHOD(GetProperty)(THIS_ REFGUID,LPDIPROPHEADER) PURE;
    STDMETHOD(SetProperty)(THIS_ REFGUID,LPCDIPROPHEADER) PURE;
    STDMETHOD(Acquire)(THIS) PURE;
    STDMETHOD(Unacquire)(THIS) PURE;
    STDMETHOD(GetDeviceState)(THIS_ DWORD,LPVOID) PURE;
    STDMETHOD(GetDeviceData)(THIS_ DWORD,LPDIDEVICEOBJECTDATA,LPDWORD,DWORD) PURE;
    STDMETHOD(SetDataFormat)(THIS_ LPCDIDATAFORMAT) PURE;
    STDMETHOD(SetEventNotification)(THIS_ HANDLE) PURE;
    STDMETHOD(SetCooperativeLevel)(THIS_ HWND,DWORD) PURE;
    STDMETHOD(GetObjectInfo)(THIS_ LPDIDEVICEOBJECTINSTANCEW,DWORD,DWORD) PURE;
    STDMETHOD(GetDeviceInfo)(THIS_ LPDIDEVICEINSTANCEW) PURE;
    STDMETHOD(RunControlPanel)(THIS_ HWND,DWORD) PURE;
    STDMETHOD(Initialize)(THIS_ HINSTANCE,DWORD,REFGUID) PURE;
    STDMETHOD(CreateEffect)(THIS_ REFGUID,LPCDIEFFECT,LPDIRECTINPUTEFFECT *,LPUNKNOWN) PURE;
    STDMETHOD(EnumEffects)(THIS_ LPDIENUMEFFECTSCALLBACKW,LPVOID,DWORD) PURE;
    STDMETHOD(GetEffectInfo)(THIS_ LPDIEFFECTINFOW,REFGUID) PURE;
    STDMETHOD(GetForceFeedbackState)(THIS_ LPDWORD) PURE;
    STDMETHOD(SendForceFeedbackCommand)(THIS_ DWORD) PURE;
    STDMETHOD(EnumCreatedEffectObjects)(THIS_ LPDIENUMCREATEDEFFECTOBJECTSCALLBACK,LPVOID,DWORD) PURE;
    STDMETHOD(Escape)(THIS_ LPDIEFFESCAPE) PURE;
    STDMETHOD(Poll)(THIS) PURE;
    STDMETHOD(SendDeviceData)(THIS_ DWORD,LPCDIDEVICEOBJECTDATA,LPDWORD,DWORD) PURE;

    /*** IDirectInputDevice7W methods ***/
    STDMETHOD(EnumEffectsInFile)(THIS_ LPCWSTR,LPDIENUMEFFECTSINFILECALLBACK,LPVOID,DWORD) PURE;
    STDMETHOD(WriteEffectToFile)(THIS_ LPCWSTR,DWORD,LPDIFILEEFFECT,DWORD) PURE;
};

typedef struct IDirectInputDevice7W *LPDIRECTINPUTDEVICE7W;

#undef INTERFACE
#define INTERFACE IDirectInputDevice7A

DECLARE_INTERFACE_(IDirectInputDevice7A, IDirectInputDevice2A)
{
    /*** IUnknown methods ***/
    STDMETHOD(QueryInterface)(THIS_ REFIID riid, LPVOID * ppvObj) PURE;
    STDMETHOD_(ULONG,AddRef)(THIS) PURE;
    STDMETHOD_(ULONG,Release)(THIS) PURE;

    /*** IDirectInputDevice2A methods ***/
    STDMETHOD(GetCapabilities)(THIS_ LPDIDEVCAPS) PURE;
    STDMETHOD(EnumObjects)(THIS_ LPDIENUMDEVICEOBJECTSCALLBACKA,LPVOID,DWORD) PURE;
    STDMETHOD(GetProperty)(THIS_ REFGUID,LPDIPROPHEADER) PURE;
    STDMETHOD(SetProperty)(THIS_ REFGUID,LPCDIPROPHEADER) PURE;
    STDMETHOD(Acquire)(THIS) PURE;
    STDMETHOD(Unacquire)(THIS) PURE;
    STDMETHOD(GetDeviceState)(THIS_ DWORD,LPVOID) PURE;
    STDMETHOD(GetDeviceData)(THIS_ DWORD,LPDIDEVICEOBJECTDATA,LPDWORD,DWORD) PURE;
    STDMETHOD(SetDataFormat)(THIS_ LPCDIDATAFORMAT) PURE;
    STDMETHOD(SetEventNotification)(THIS_ HANDLE) PURE;
    STDMETHOD(SetCooperativeLevel)(THIS_ HWND,DWORD) PURE;
    STDMETHOD(GetObjectInfo)(THIS_ LPDIDEVICEOBJECTINSTANCEA,DWORD,DWORD) PURE;
    STDMETHOD(GetDeviceInfo)(THIS_ LPDIDEVICEINSTANCEA) PURE;
    STDMETHOD(RunControlPanel)(THIS_ HWND,DWORD) PURE;
    STDMETHOD(Initialize)(THIS_ HINSTANCE,DWORD,REFGUID) PURE;
    STDMETHOD(CreateEffect)(THIS_ REFGUID,LPCDIEFFECT,LPDIRECTINPUTEFFECT *,LPUNKNOWN) PURE;
    STDMETHOD(EnumEffects)(THIS_ LPDIENUMEFFECTSCALLBACKA,LPVOID,DWORD) PURE;
    STDMETHOD(GetEffectInfo)(THIS_ LPDIEFFECTINFOA,REFGUID) PURE;
    STDMETHOD(GetForceFeedbackState)(THIS_ LPDWORD) PURE;
    STDMETHOD(SendForceFeedbackCommand)(THIS_ DWORD) PURE;
    STDMETHOD(EnumCreatedEffectObjects)(THIS_ LPDIENUMCREATEDEFFECTOBJECTSCALLBACK,LPVOID,DWORD) PURE;
    STDMETHOD(Escape)(THIS_ LPDIEFFESCAPE) PURE;
    STDMETHOD(Poll)(THIS) PURE;
    STDMETHOD(SendDeviceData)(THIS_ DWORD,LPCDIDEVICEOBJECTDATA,LPDWORD,DWORD) PURE;

    /*** IDirectInputDevice7A methods ***/
    STDMETHOD(EnumEffectsInFile)(THIS_ LPCSTR,LPDIENUMEFFECTSINFILECALLBACK,LPVOID,DWORD) PURE;
    STDMETHOD(WriteEffectToFile)(THIS_ LPCSTR,DWORD,LPDIFILEEFFECT,DWORD) PURE;
};

typedef struct IDirectInputDevice7A *LPDIRECTINPUTDEVICE7A;

#ifdef UNICODE
#define IID_IDirectInputDevice7 IID_IDirectInputDevice7W
#define IDirectInputDevice7 IDirectInputDevice7W
#define IDirectInputDevice7Vtbl IDirectInputDevice7WVtbl
#else
#define IID_IDirectInputDevice7 IID_IDirectInputDevice7A
#define IDirectInputDevice7 IDirectInputDevice7A
#define IDirectInputDevice7Vtbl IDirectInputDevice7AVtbl
#endif
typedef struct IDirectInputDevice7 *LPDIRECTINPUTDEVICE7;

#if !defined(__cplusplus) || defined(CINTERFACE)
#define IDirectInputDevice7_QueryInterface(p,a,b) (p)->lpVtbl->QueryInterface(p,a,b)
#define IDirectInputDevice7_AddRef(p) (p)->lpVtbl->AddRef(p)
#define IDirectInputDevice7_Release(p) (p)->lpVtbl->Release(p)
#define IDirectInputDevice7_GetCapabilities(p,a) (p)->lpVtbl->GetCapabilities(p,a)
#define IDirectInputDevice7_EnumObjects(p,a,b,c) (p)->lpVtbl->EnumObjects(p,a,b,c)
#define IDirectInputDevice7_GetProperty(p,a,b) (p)->lpVtbl->GetProperty(p,a,b)
#define IDirectInputDevice7_SetProperty(p,a,b) (p)->lpVtbl->SetProperty(p,a,b)
#define IDirectInputDevice7_Acquire(p) (p)->lpVtbl->Acquire(p)
#define IDirectInputDevice7_Unacquire(p) (p)->lpVtbl->Unacquire(p)
#define IDirectInputDevice7_GetDeviceState(p,a,b) (p)->lpVtbl->GetDeviceState(p,a,b)
#define IDirectInputDevice7_GetDeviceData(p,a,b,c,d) (p)->lpVtbl->GetDeviceData(p,a,b,c,d)
#define IDirectInputDevice7_SetDataFormat(p,a) (p)->lpVtbl->SetDataFormat(p,a)
#define IDirectInputDevice7_SetEventNotification(p,a) (p)->lpVtbl->SetEventNotification(p,a)
#define IDirectInputDevice7_SetCooperativeLevel(p,a,b) (p)->lpVtbl->SetCooperativeLevel(p,a,b)
#define IDirectInputDevice7_GetObjectInfo(p,a,b,c) (p)->lpVtbl->GetObjectInfo(p,a,b,c)
#define IDirectInputDevice7_GetDeviceInfo(p,a) (p)->lpVtbl->GetDeviceInfo(p,a)
#define IDirectInputDevice7_RunControlPanel(p,a,b) (p)->lpVtbl->RunControlPanel(p,a,b)
#define IDirectInputDevice7_Initialize(p,a,b,c) (p)->lpVtbl->Initialize(p,a,b,c)
#define IDirectInputDevice7_CreateEffect(p,a,b,c,d) (p)->lpVtbl->CreateEffect(p,a,b,c,d)
#define IDirectInputDevice7_EnumEffects(p,a,b,c) (p)->lpVtbl->EnumEffects(p,a,b,c)
#define IDirectInputDevice7_GetEffectInfo(p,a,b) (p)->lpVtbl->GetEffectInfo(p,a,b)
#define IDirectInputDevice7_GetForceFeedbackState(p,a) (p)->lpVtbl->GetForceFeedbackState(p,a)
#define IDirectInputDevice7_SendForceFeedbackCommand(p,a) (p)->lpVtbl->SendForceFeedbackCommand(p,a)
#define IDirectInputDevice7_EnumCreatedEffectObjects(p,a,b,c) (p)->lpVtbl->EnumCreatedEffectObjects(p,a,b,c)
#define IDirectInputDevice7_Escape(p,a) (p)->lpVtbl->Escape(p,a)
#define IDirectInputDevice7_Poll(p) (p)->lpVtbl->Poll(p)
#define IDirectInputDevice7_SendDeviceData(p,a,b,c,d) (p)->lpVtbl->SendDeviceData(p,a,b,c,d)
#define IDirectInputDevice7_EnumEffectsInFile(p,a,b,c,d) (p)->lpVtbl->EnumEffectsInFile(p,a,b,c,d)
#define IDirectInputDevice7_WriteEffectToFile(p,a,b,c,d) (p)->lpVtbl->WriteEffectToFile(p,a,b,c,d)
#else
#define IDirectInputDevice7_QueryInterface(p,a,b) (p)->QueryInterface(a,b)
#define IDirectInputDevice7_AddRef(p) (p)->AddRef()
#define IDirectInputDevice7_Release(p) (p)->Release()
#define IDirectInputDevice7_GetCapabilities(p,a) (p)->GetCapabilities(a)
#define IDirectInputDevice7_EnumObjects(p,a,b,c) (p)->EnumObjects(a,b,c)
#define IDirectInputDevice7_GetProperty(p,a,b) (p)->GetProperty(a,b)
#define IDirectInputDevice7_SetProperty(p,a,b) (p)->SetProperty(a,b)
#define IDirectInputDevice7_Acquire(p) (p)->Acquire()
#define IDirectInputDevice7_Unacquire(p) (p)->Unacquire()
#define IDirectInputDevice7_GetDeviceState(p,a,b) (p)->GetDeviceState(a,b)
#define IDirectInputDevice7_GetDeviceData(p,a,b,c,d) (p)->GetDeviceData(a,b,c,d)
#define IDirectInputDevice7_SetDataFormat(p,a) (p)->SetDataFormat(a)
#define IDirectInputDevice7_SetEventNotification(p,a) (p)->SetEventNotification(a)
#define IDirectInputDevice7_SetCooperativeLevel(p,a,b) (p)->SetCooperativeLevel(a,b)
#define IDirectInputDevice7_GetObjectInfo(p,a,b,c) (p)->GetObjectInfo(a,b,c)
#define IDirectInputDevice7_GetDeviceInfo(p,a) (p)->GetDeviceInfo(a)
#define IDirectInputDevice7_RunControlPanel(p,a,b) (p)->RunControlPanel(a,b)
#define IDirectInputDevice7_Initialize(p,a,b,c) (p)->Initialize(a,b,c)
#define IDirectInputDevice7_CreateEffect(p,a,b,c,d) (p)->CreateEffect(a,b,c,d)
#define IDirectInputDevice7_EnumEffects(p,a,b,c) (p)->EnumEffects(a,b,c)
#define IDirectInputDevice7_GetEffectInfo(p,a,b) (p)->GetEffectInfo(a,b)
#define IDirectInputDevice7_GetForceFeedbackState(p,a) (p)->GetForceFeedbackState(a)
#define IDirectInputDevice7_SendForceFeedbackCommand(p,a) (p)->SendForceFeedbackCommand(a)
#define IDirectInputDevice7_EnumCreatedEffectObjects(p,a,b,c) (p)->EnumCreatedEffectObjects(a,b,c)
#define IDirectInputDevice7_Escape(p,a) (p)->Escape(a)
#define IDirectInputDevice7_Poll(p) (p)->Poll()
#define IDirectInputDevice7_SendDeviceData(p,a,b,c,d) (p)->SendDeviceData(a,b,c,d)
#define IDirectInputDevice7_EnumEffectsInFile(p,a,b,c,d) (p)->EnumEffectsInFile(a,b,c,d)
#define IDirectInputDevice7_WriteEffectToFile(p,a,b,c,d) (p)->WriteEffectToFile(a,b,c,d)
#endif

#endif /* DIJ_RINGZERO */

#endif /* DIRECTINPUT_VERSION >= 0x0700 */

#if(DIRECTINPUT_VERSION >= 0x0701)

#ifndef DIJ_RINGZERO

#undef INTERFACE
#define INTERFACE IDirectInputDevice8W

DECLARE_INTERFACE_(IDirectInputDevice8W, IUnknown)
{
    /*** IUnknown methods ***/
    STDMETHOD(QueryInterface)(THIS_ REFIID riid, LPVOID * ppvObj) PURE;
    STDMETHOD_(ULONG,AddRef)(THIS) PURE;
    STDMETHOD_(ULONG,Release)(THIS) PURE;

    /*** IDirectInputDevice8W methods ***/
    STDMETHOD(GetCapabilities)(THIS_ LPDIDEVCAPS) PURE;
    STDMETHOD(EnumObjects)(THIS_ LPDIENUMDEVICEOBJECTSCALLBACKW,LPVOID,DWORD) PURE;
    STDMETHOD(GetProperty)(THIS_ REFGUID,LPDIPROPHEADER) PURE;
    STDMETHOD(SetProperty)(THIS_ REFGUID,LPCDIPROPHEADER) PURE;
    STDMETHOD(Acquire)(THIS) PURE;
    STDMETHOD(Unacquire)(THIS) PURE;
    STDMETHOD(GetDeviceState)(THIS_ DWORD,LPVOID) PURE;
    STDMETHOD(GetDeviceData)(THIS_ DWORD,LPDIDEVICEOBJECTDATA,LPDWORD,DWORD) PURE;
    STDMETHOD(SetDataFormat)(THIS_ LPCDIDATAFORMAT) PURE;
    STDMETHOD(SetEventNotification)(THIS_ HANDLE) PURE;
    STDMETHOD(SetCooperativeLevel)(THIS_ HWND,DWORD) PURE;
    STDMETHOD(GetObjectInfo)(THIS_ LPDIDEVICEOBJECTINSTANCEW,DWORD,DWORD) PURE;
    STDMETHOD(GetDeviceInfo)(THIS_ LPDIDEVICEINSTANCEW) PURE;
    STDMETHOD(RunControlPanel)(THIS_ HWND,DWORD) PURE;
    STDMETHOD(Initialize)(THIS_ HINSTANCE,DWORD,REFGUID) PURE;
    STDMETHOD(CreateEffect)(THIS_ REFGUID,LPCDIEFFECT,LPDIRECTINPUTEFFECT *,LPUNKNOWN) PURE;
    STDMETHOD(EnumEffects)(THIS_ LPDIENUMEFFECTSCALLBACKW,LPVOID,DWORD) PURE;
    STDMETHOD(GetEffectInfo)(THIS_ LPDIEFFECTINFOW,REFGUID) PURE;
    STDMETHOD(GetForceFeedbackState)(THIS_ LPDWORD) PURE;
    STDMETHOD(SendForceFeedbackCommand)(THIS_ DWORD) PURE;
    STDMETHOD(EnumCreatedEffectObjects)(THIS_ LPDIENUMCREATEDEFFECTOBJECTSCALLBACK,LPVOID,DWORD) PURE;
    STDMETHOD(Escape)(THIS_ LPDIEFFESCAPE) PURE;
    STDMETHOD(Poll)(THIS) PURE;
    STDMETHOD(SendDeviceData)(THIS_ DWORD,LPCDIDEVICEOBJECTDATA,LPDWORD,DWORD) PURE;
    STDMETHOD(EnumEffectsInFile)(THIS_ LPCWSTR,LPDIENUMEFFECTSINFILECALLBACK,LPVOID,DWORD) PURE;
    STDMETHOD(WriteEffectToFile)(THIS_ LPCWSTR,DWORD,LPDIFILEEFFECT,DWORD) PURE;
    STDMETHOD(BuildActionMap)(THIS_ LPDIACTIONFORMATW,LPCWSTR,DWORD) PURE;
    STDMETHOD(SetActionMap)(THIS_ LPDIACTIONFORMATW,LPCWSTR,DWORD) PURE;
    STDMETHOD(GetImageInfo)(THIS_ LPDIDEVICEIMAGEINFOHEADERW) PURE;
};

typedef struct IDirectInputDevice8W *LPDIRECTINPUTDEVICE8W;

#undef INTERFACE
#define INTERFACE IDirectInputDevice8A

DECLARE_INTERFACE_(IDirectInputDevice8A, IUnknown)
{
    /*** IUnknown methods ***/
    STDMETHOD(QueryInterface)(THIS_ REFIID riid, LPVOID * ppvObj) PURE;
    STDMETHOD_(ULONG,AddRef)(THIS) PURE;
    STDMETHOD_(ULONG,Release)(THIS) PURE;

    /*** IDirectInputDevice8A methods ***/
    STDMETHOD(GetCapabilities)(THIS_ LPDIDEVCAPS) PURE;
    STDMETHOD(EnumObjects)(THIS_ LPDIENUMDEVICEOBJECTSCALLBACKA,LPVOID,DWORD) PURE;
    STDMETHOD(GetProperty)(THIS_ REFGUID,LPDIPROPHEADER) PURE;
    STDMETHOD(SetProperty)(THIS_ REFGUID,LPCDIPROPHEADER) PURE;
    STDMETHOD(Acquire)(THIS) PURE;
    STDMETHOD(Unacquire)(THIS) PURE;
    STDMETHOD(GetDeviceState)(THIS_ DWORD,LPVOID) PURE;
    STDMETHOD(GetDeviceData)(THIS_ DWORD,LPDIDEVICEOBJECTDATA,LPDWORD,DWORD) PURE;
    STDMETHOD(SetDataFormat)(THIS_ LPCDIDATAFORMAT) PURE;
    STDMETHOD(SetEventNotification)(THIS_ HANDLE) PURE;
    STDMETHOD(SetCooperativeLevel)(THIS_ HWND,DWORD) PURE;
    STDMETHOD(GetObjectInfo)(THIS_ LPDIDEVICEOBJECTINSTANCEA,DWORD,DWORD) PURE;
    STDMETHOD(GetDeviceInfo)(THIS_ LPDIDEVICEINSTANCEA) PURE;
    STDMETHOD(RunControlPanel)(THIS_ HWND,DWORD) PURE;
    STDMETHOD(Initialize)(THIS_ HINSTANCE,DWORD,REFGUID) PURE;
    STDMETHOD(CreateEffect)(THIS_ REFGUID,LPCDIEFFECT,LPDIRECTINPUTEFFECT *,LPUNKNOWN) PURE;
    STDMETHOD(EnumEffects)(THIS_ LPDIENUMEFFECTSCALLBACKA,LPVOID,DWORD) PURE;
    STDMETHOD(GetEffectInfo)(THIS_ LPDIEFFECTINFOA,REFGUID) PURE;
    STDMETHOD(GetForceFeedbackState)(THIS_ LPDWORD) PURE;
    STDMETHOD(SendForceFeedbackCommand)(THIS_ DWORD) PURE;
    STDMETHOD(EnumCreatedEffectObjects)(THIS_ LPDIENUMCREATEDEFFECTOBJECTSCALLBACK,LPVOID,DWORD) PURE;
    STDMETHOD(Escape)(THIS_ LPDIEFFESCAPE) PURE;
    STDMETHOD(Poll)(THIS) PURE;
    STDMETHOD(SendDeviceData)(THIS_ DWORD,LPCDIDEVICEOBJECTDATA,LPDWORD,DWORD) PURE;
    STDMETHOD(EnumEffectsInFile)(THIS_ LPCSTR,LPDIENUMEFFECTSINFILECALLBACK,LPVOID,DWORD) PURE;
    STDMETHOD(WriteEffectToFile)(THIS_ LPCSTR,DWORD,LPDIFILEEFFECT,DWORD) PURE;
    STDMETHOD(BuildActionMap)(THIS_ LPDIACTIONFORMATA,LPCSTR,DWORD) PURE;
    STDMETHOD(SetActionMap)(THIS_ LPDIACTIONFORMATA,LPCSTR,DWORD) PURE;
    STDMETHOD(GetImageInfo)(THIS_ LPDIDEVICEIMAGEINFOHEADERA) PURE;
};

typedef struct IDirectInputDevice8A *LPDIRECTINPUTDEVICE8A;

#ifdef UNICODE
#define IID_IDirectInputDevice8 IID_IDirectInputDevice8W
#define IDirectInputDevice8 IDirectInputDevice8W
#define IDirectInputDevice8Vtbl IDirectInputDevice8WVtbl
#else
#define IID_IDirectInputDevice8 IID_IDirectInputDevice8A
#define IDirectInputDevice8 IDirectInputDevice8A
#define IDirectInputDevice8Vtbl IDirectInputDevice8AVtbl
#endif
typedef struct IDirectInputDevice8 *LPDIRECTINPUTDEVICE8;

#if !defined(__cplusplus) || defined(CINTERFACE)
#define IDirectInputDevice8_QueryInterface(p,a,b) (p)->lpVtbl->QueryInterface(p,a,b)
#define IDirectInputDevice8_AddRef(p) (p)->lpVtbl->AddRef(p)
#define IDirectInputDevice8_Release(p) (p)->lpVtbl->Release(p)
#define IDirectInputDevice8_GetCapabilities(p,a) (p)->lpVtbl->GetCapabilities(p,a)
#define IDirectInputDevice8_EnumObjects(p,a,b,c) (p)->lpVtbl->EnumObjects(p,a,b,c)
#define IDirectInputDevice8_GetProperty(p,a,b) (p)->lpVtbl->GetProperty(p,a,b)
#define IDirectInputDevice8_SetProperty(p,a,b) (p)->lpVtbl->SetProperty(p,a,b)
#define IDirectInputDevice8_Acquire(p) (p)->lpVtbl->Acquire(p)
#define IDirectInputDevice8_Unacquire(p) (p)->lpVtbl->Unacquire(p)
#define IDirectInputDevice8_GetDeviceState(p,a,b) (p)->lpVtbl->GetDeviceState(p,a,b)
#define IDirectInputDevice8_GetDeviceData(p,a,b,c,d) (p)->lpVtbl->GetDeviceData(p,a,b,c,d)
#define IDirectInputDevice8_SetDataFormat(p,a) (p)->lpVtbl->SetDataFormat(p,a)
#define IDirectInputDevice8_SetEventNotification(p,a) (p)->lpVtbl->SetEventNotification(p,a)
#define IDirectInputDevice8_SetCooperativeLevel(p,a,b) (p)->lpVtbl->SetCooperativeLevel(p,a,b)
#define IDirectInputDevice8_GetObjectInfo(p,a,b,c) (p)->lpVtbl->GetObjectInfo(p,a,b,c)
#define IDirectInputDevice8_GetDeviceInfo(p,a) (p)->lpVtbl->GetDeviceInfo(p,a)
#define IDirectInputDevice8_RunControlPanel(p,a,b) (p)->lpVtbl->RunControlPanel(p,a,b)
#define IDirectInputDevice8_Initialize(p,a,b,c) (p)->lpVtbl->Initialize(p,a,b,c)
#define IDirectInputDevice8_CreateEffect(p,a,b,c,d) (p)->lpVtbl->CreateEffect(p,a,b,c,d)
#define IDirectInputDevice8_EnumEffects(p,a,b,c) (p)->lpVtbl->EnumEffects(p,a,b,c)
#define IDirectInputDevice8_GetEffectInfo(p,a,b) (p)->lpVtbl->GetEffectInfo(p,a,b)
#define IDirectInputDevice8_GetForceFeedbackState(p,a) (p)->lpVtbl->GetForceFeedbackState(p,a)
#define IDirectInputDevice8_SendForceFeedbackCommand(p,a) (p)->lpVtbl->SendForceFeedbackCommand(p,a)
#define IDirectInputDevice8_EnumCreatedEffectObjects(p,a,b,c) (p)->lpVtbl->EnumCreatedEffectObjects(p,a,b,c)
#define IDirectInputDevice8_Escape(p,a) (p)->lpVtbl->Escape(p,a)
#define IDirectInputDevice8_Poll(p) (p)->lpVtbl->Poll(p)
#define IDirectInputDevice8_SendDeviceData(p,a,b,c,d) (p)->lpVtbl->SendDeviceData(p,a,b,c,d)
#define IDirectInputDevice8_EnumEffectsInFile(p,a,b,c,d) (p)->lpVtbl->EnumEffectsInFile(p,a,b,c,d)
#define IDirectInputDevice8_WriteEffectToFile(p,a,b,c,d) (p)->lpVtbl->WriteEffectToFile(p,a,b,c,d)
#define IDirectInputDevice8_BuildActionMap(p,a,b,c) (p)->lpVtbl->BuildActionMap(p,a,b,c)
#define IDirectInputDevice8_SetActionMap(p,a,b,c) (p)->lpVtbl->SetActionMap(p,a,b,c)
#define IDirectInputDevice8_GetImageInfo(p,a) (p)->lpVtbl->GetImageInfo(p,a)
#else
#define IDirectInputDevice8_QueryInterface(p,a,b) (p)->QueryInterface(a,b)
#define IDirectInputDevice8_AddRef(p) (p)->AddRef()
#define IDirectInputDevice8_Release(p) (p)->Release()
#define IDirectInputDevice8_GetCapabilities(p,a) (p)->GetCapabilities(a)
#define IDirectInputDevice8_EnumObjects(p,a,b,c) (p)->EnumObjects(a,b,c)
#define IDirectInputDevice8_GetProperty(p,a,b) (p)->GetProperty(a,b)
#define IDirectInputDevice8_SetProperty(p,a,b) (p)->SetProperty(a,b)
#define IDirectInputDevice8_Acquire(p) (p)->Acquire()
#define IDirectInputDevice8_Unacquire(p) (p)->Unacquire()
#define IDirectInputDevice8_GetDeviceState(p,a,b) (p)->GetDeviceState(a,b)
#define IDirectInputDevice8_GetDeviceData(p,a,b,c,d) (p)->GetDeviceData(a,b,c,d)
#define IDirectInputDevice8_SetDataFormat(p,a) (p)->SetDataFormat(a)
#define IDirectInputDevice8_SetEventNotification(p,a) (p)->SetEventNotification(a)
#define IDirectInputDevice8_SetCooperativeLevel(p,a,b) (p)->SetCooperativeLevel(a,b)
#define IDirectInputDevice8_GetObjectInfo(p,a,b,c) (p)->GetObjectInfo(a,b,c)
#define IDirectInputDevice8_GetDeviceInfo(p,a) (p)->GetDeviceInfo(a)
#define IDirectInputDevice8_RunControlPanel(p,a,b) (p)->RunControlPanel(a,b)
#define IDirectInputDevice8_Initialize(p,a,b,c) (p)->Initialize(a,b,c)
#define IDirectInputDevice8_CreateEffect(p,a,b,c,d) (p)->CreateEffect(a,b,c,d)
#define IDirectInputDevice8_EnumEffects(p,a,b,c) (p)->EnumEffects(a,b,c)
#define IDirectInputDevice8_GetEffectInfo(p,a,b) (p)->GetEffectInfo(a,b)
#define IDirectInputDevice8_GetForceFeedbackState(p,a) (p)->GetForceFeedbackState(a)
#define IDirectInputDevice8_SendForceFeedbackCommand(p,a) (p)->SendForceFeedbackCommand(a)
#define IDirectInputDevice8_EnumCreatedEffectObjects(p,a,b,c) (p)->EnumCreatedEffectObjects(a,b,c)
#define IDirectInputDevice8_Escape(p,a) (p)->Escape(a)
#define IDirectInputDevice8_Poll(p) (p)->Poll()
#define IDirectInputDevice8_SendDeviceData(p,a,b,c,d) (p)->SendDeviceData(a,b,c,d)
#define IDirectInputDevice8_EnumEffectsInFile(p,a,b,c,d) (p)->EnumEffectsInFile(a,b,c,d)
#define IDirectInputDevice8_WriteEffectToFile(p,a,b,c,d) (p)->WriteEffectToFile(a,b,c,d)
#define IDirectInputDevice8_BuildActionMap(p,a,b,c) (p)->BuildActionMap(a,b,c)
#define IDirectInputDevice8_SetActionMap(p,a,b,c) (p)->SetActionMap(a,b,c)
#define IDirectInputDevice8_GetImageInfo(p,a) (p)->GetImageInfo(a)
#endif

#endif /* DIJ_RINGZERO */

#endif /* DIRECTINPUT_VERSION >= 0x0701 */

/****************************************************************************
 *
 *      Mouse
 *
 ****************************************************************************/

#ifndef DIJ_RINGZERO

typedef struct _DIMOUSESTATE {
    LONG    lX;
    LONG    lY;
    LONG    lZ;
    BYTE    rgbButtons[4];
} DIMOUSESTATE, *LPDIMOUSESTATE;

#if DIRECTINPUT_VERSION >= 0x0700
typedef struct _DIMOUSESTATE2 {
    LONG    lX;
    LONG    lY;
    LONG    lZ;
    BYTE    rgbButtons[8];
} DIMOUSESTATE2, *LPDIMOUSESTATE2;
#endif


#define DIMOFS_X        FIELD_OFFSET(DIMOUSESTATE, lX)
#define DIMOFS_Y        FIELD_OFFSET(DIMOUSESTATE, lY)
#define DIMOFS_Z        FIELD_OFFSET(DIMOUSESTATE, lZ)
#define DIMOFS_BUTTON0 (FIELD_OFFSET(DIMOUSESTATE, rgbButtons) + 0)
#define DIMOFS_BUTTON1 (FIELD_OFFSET(DIMOUSESTATE, rgbButtons) + 1)
#define DIMOFS_BUTTON2 (FIELD_OFFSET(DIMOUSESTATE, rgbButtons) + 2)
#define DIMOFS_BUTTON3 (FIELD_OFFSET(DIMOUSESTATE, rgbButtons) + 3)
#if (DIRECTINPUT_VERSION >= 0x0700)
#define DIMOFS_BUTTON4 (FIELD_OFFSET(DIMOUSESTATE2, rgbButtons) + 4)
#define DIMOFS_BUTTON5 (FIELD_OFFSET(DIMOUSESTATE2, rgbButtons) + 5)
#define DIMOFS_BUTTON6 (FIELD_OFFSET(DIMOUSESTATE2, rgbButtons) + 6)
#define DIMOFS_BUTTON7 (FIELD_OFFSET(DIMOUSESTATE2, rgbButtons) + 7)
#endif
#endif /* DIJ_RINGZERO */

/****************************************************************************
 *
 *      Keyboard
 *
 ****************************************************************************/

#ifndef DIJ_RINGZERO

/****************************************************************************
 *
 *      DirectInput keyboard scan codes
 *
 ****************************************************************************/

/*
 *  Alternate names for keys, to facilitate transition from DOS.
 */
#define DIK_BACKSPACE       DIK_BACK            /* backspace */
#define DIK_NUMPADSTAR      DIK_MULTIPLY        /* * on numeric keypad */
#define DIK_LALT            DIK_LMENU           /* left Alt */
#define DIK_CAPSLOCK        DIK_CAPITAL         /* CapsLock */
#define DIK_NUMPADMINUS     DIK_SUBTRACT        /* - on numeric keypad */
#define DIK_NUMPADPLUS      DIK_ADD             /* + on numeric keypad */
#define DIK_NUMPADPERIOD    DIK_DECIMAL         /* . on numeric keypad */
#define DIK_NUMPADSLASH     DIK_DIVIDE          /* / on numeric keypad */
#define DIK_RALT            DIK_RMENU           /* right Alt */
#define DIK_UPARROW         DIK_UP              /* UpArrow on arrow keypad */
#define DIK_PGUP            DIK_PRIOR           /* PgUp on arrow keypad */
#define DIK_LEFTARROW       DIK_LEFT            /* LeftArrow on arrow keypad */
#define DIK_RIGHTARROW      DIK_RIGHT           /* RightArrow on arrow keypad */
#define DIK_DOWNARROW       DIK_DOWN            /* DownArrow on arrow keypad */
#define DIK_PGDN            DIK_NEXT            /* PgDn on arrow keypad */

/*
 *  Alternate names for keys originally not used on US keyboards.
 */
#define DIK_CIRCUMFLEX      DIK_PREVTRACK       /* Japanese keyboard */

#endif /* DIJ_RINGZERO */

/****************************************************************************
 *
 *      Joystick
 *
 ****************************************************************************/

#ifndef DIJ_RINGZERO

typedef struct _DIXBOXCONTROLLERSTATE {
	WORD	Reserved;
	WORD	bmButtons;
	BYTE	rgbAnalogButtons[8];
	WORD	wDPAD;
	WORD	wThumb1X;
	WORD	wThumb1Y;
	WORD	wThumb2X;
	WORD	wThumb2Y;
	WORD	wTiltX;
	WORD	wTiltY;
} DIXBOXCONTROLLERSTATE, *LPDIXBOXCONTROLLERSTATE;

typedef struct _DIXBOXRUMBLESTATE {
	WORD	Reserved;
	BYTE	bLeftMotorSpeed;
	BYTE	bRightMotorSpeed;
} DIXBOXRUMBLESTATE, *LPDIXBOXRUMBLESTATE;

#define DIXROFS_LEFTMOTOR	2
#define DIXROFS_RIGHTMOTOR	3


#endif /* DIJ_RINGZERO */

/****************************************************************************
 *
 *  IDirectInput
 *
 ****************************************************************************/

#ifndef DIJ_RINGZERO

#define DIENUM_STOP             0
#define DIENUM_CONTINUE         1

typedef BOOL (FAR PASCAL * LPDIENUMDEVICESCALLBACKA)(LPCDIDEVICEINSTANCEA, LPVOID);
typedef BOOL (FAR PASCAL * LPDIENUMDEVICESCALLBACKW)(LPCDIDEVICEINSTANCEW, LPVOID);
#ifdef UNICODE
#define LPDIENUMDEVICESCALLBACK  LPDIENUMDEVICESCALLBACKW
#else
#define LPDIENUMDEVICESCALLBACK  LPDIENUMDEVICESCALLBACKA
#endif // !UNICODE
typedef BOOL (FAR PASCAL * LPDICONFIGUREDEVICESCALLBACK)(IUnknown FAR *, LPVOID);

#define DIEDFL_ENUMANDNOTIFYINSERT	0x00000000
#define DIEDFL_NOTIFYREMOVE			0x01000000
#define DIEDFL_HALTNOTIFYINSERT		0x80000000
#define DIEDFL_HALTNOTIFYREMOVE		0x81000000


#if(DIRECTINPUT_VERSION >= 0x0701)
typedef BOOL (FAR PASCAL * LPDIENUMDEVICESBYSEMANTICSCBA)(LPCDIDEVICEINSTANCEA, LPDIRECTINPUTDEVICE8A, DWORD, DWORD, LPVOID);
typedef BOOL (FAR PASCAL * LPDIENUMDEVICESBYSEMANTICSCBW)(LPCDIDEVICEINSTANCEW, LPDIRECTINPUTDEVICE8W, DWORD, DWORD, LPVOID);
#ifdef UNICODE
#define LPDIENUMDEVICESBYSEMANTICSCB  LPDIENUMDEVICESBYSEMANTICSCBW
#else
#define LPDIENUMDEVICESBYSEMANTICSCB  LPDIENUMDEVICESBYSEMANTICSCBA
#endif // !UNICODE
#endif /* DIRECTINPUT_VERSION >= 0x0701 */

#if(DIRECTINPUT_VERSION >= 0x0701)
#define DIEDBS_MAPPEDPRI1         0x00000001
#define DIEDBS_MAPPEDPRI2         0x00000002
#define DIEDBS_RECENTDEVICE       0x00000010
#define DIEDBS_NEWDEVICE          0x00000020
#endif /* DIRECTINPUT_VERSION >= 0x0701 */

#if(DIRECTINPUT_VERSION >= 0x0701)
#define DIEDBSFL_ATTACHEDONLY     0x00000000
#define DIEDBSFL_THISUSER         0x00000010
#define DIEDBSFL_FORCEFEEDBACK    DIEDFL_FORCEFEEDBACK
#define DIEDBSFL_AVAILABLEDEVICES 0x00001000
#define DIEDBSFL_VALID            0x00001110
#endif /* DIRECTINPUT_VERSION >= 0x0701 */

#undef INTERFACE
#define INTERFACE IDirectInputW

DECLARE_INTERFACE_(IDirectInputW, IUnknown)
{
    /*** IUnknown methods ***/
    STDMETHOD(QueryInterface)(THIS_ REFIID riid, LPVOID * ppvObj) PURE;
    STDMETHOD_(ULONG,AddRef)(THIS) PURE;
    STDMETHOD_(ULONG,Release)(THIS) PURE;

    /*** IDirectInputW methods ***/
    STDMETHOD(CreateDevice)(THIS_ REFGUID,LPDIRECTINPUTDEVICEW *,LPUNKNOWN) PURE;
    STDMETHOD(EnumDevices)(THIS_ DWORD,LPDIENUMDEVICESCALLBACKW,LPVOID,DWORD) PURE;
    STDMETHOD(GetDeviceStatus)(THIS_ REFGUID) PURE;
    STDMETHOD(RunControlPanel)(THIS_ HWND,DWORD) PURE;
    STDMETHOD(Initialize)(THIS_ HINSTANCE,DWORD) PURE;
};

typedef struct IDirectInputW *LPDIRECTINPUTW;

#undef INTERFACE
#define INTERFACE IDirectInputA

DECLARE_INTERFACE_(IDirectInputA, IUnknown)
{
    /*** IUnknown methods ***/
    STDMETHOD(QueryInterface)(THIS_ REFIID riid, LPVOID * ppvObj) PURE;
    STDMETHOD_(ULONG,AddRef)(THIS) PURE;
    STDMETHOD_(ULONG,Release)(THIS) PURE;

    /*** IDirectInputA methods ***/
    STDMETHOD(CreateDevice)(THIS_ REFGUID,LPDIRECTINPUTDEVICEA *,LPUNKNOWN) PURE;
    STDMETHOD(EnumDevices)(THIS_ DWORD,LPDIENUMDEVICESCALLBACKA,LPVOID,DWORD) PURE;
    STDMETHOD(GetDeviceStatus)(THIS_ REFGUID) PURE;
    STDMETHOD(RunControlPanel)(THIS_ HWND,DWORD) PURE;
    STDMETHOD(Initialize)(THIS_ HINSTANCE,DWORD) PURE;
};

typedef struct IDirectInputA *LPDIRECTINPUTA;

#ifdef UNICODE
#define IID_IDirectInput IID_IDirectInputW
#define IDirectInput IDirectInputW
#define IDirectInputVtbl IDirectInputWVtbl
#else
#define IID_IDirectInput IID_IDirectInputA
#define IDirectInput IDirectInputA
#define IDirectInputVtbl IDirectInputAVtbl
#endif
typedef struct IDirectInput *LPDIRECTINPUT;

#if !defined(__cplusplus) || defined(CINTERFACE)
#define IDirectInput_QueryInterface(p,a,b) (p)->lpVtbl->QueryInterface(p,a,b)
#define IDirectInput_AddRef(p) (p)->lpVtbl->AddRef(p)
#define IDirectInput_Release(p) (p)->lpVtbl->Release(p)
#define IDirectInput_CreateDevice(p,a,b,c) (p)->lpVtbl->CreateDevice(p,a,b,c)
#define IDirectInput_EnumDevices(p,a,b,c,d) (p)->lpVtbl->EnumDevices(p,a,b,c,d)
#define IDirectInput_GetDeviceStatus(p,a) (p)->lpVtbl->GetDeviceStatus(p,a)
#define IDirectInput_RunControlPanel(p,a,b) (p)->lpVtbl->RunControlPanel(p,a,b)
#define IDirectInput_Initialize(p,a,b) (p)->lpVtbl->Initialize(p,a,b)
#else
#define IDirectInput_QueryInterface(p,a,b) (p)->QueryInterface(a,b)
#define IDirectInput_AddRef(p) (p)->AddRef()
#define IDirectInput_Release(p) (p)->Release()
#define IDirectInput_CreateDevice(p,a,b,c) (p)->CreateDevice(a,b,c)
#define IDirectInput_EnumDevices(p,a,b,c,d) (p)->EnumDevices(a,b,c,d)
#define IDirectInput_GetDeviceStatus(p,a) (p)->GetDeviceStatus(a)
#define IDirectInput_RunControlPanel(p,a,b) (p)->RunControlPanel(a,b)
#define IDirectInput_Initialize(p,a,b) (p)->Initialize(a,b)
#endif

#undef INTERFACE
#define INTERFACE IDirectInput2W

DECLARE_INTERFACE_(IDirectInput2W, IDirectInputW)
{
    /*** IUnknown methods ***/
    STDMETHOD(QueryInterface)(THIS_ REFIID riid, LPVOID * ppvObj) PURE;
    STDMETHOD_(ULONG,AddRef)(THIS) PURE;
    STDMETHOD_(ULONG,Release)(THIS) PURE;

    /*** IDirectInputW methods ***/
    STDMETHOD(CreateDevice)(THIS_ REFGUID,LPDIRECTINPUTDEVICEW *,LPUNKNOWN) PURE;
    STDMETHOD(EnumDevices)(THIS_ DWORD,LPDIENUMDEVICESCALLBACKW,LPVOID,DWORD) PURE;
    STDMETHOD(GetDeviceStatus)(THIS_ REFGUID) PURE;
    STDMETHOD(RunControlPanel)(THIS_ HWND,DWORD) PURE;
    STDMETHOD(Initialize)(THIS_ HINSTANCE,DWORD) PURE;

    /*** IDirectInput2W methods ***/
    STDMETHOD(FindDevice)(THIS_ REFGUID,LPCWSTR,LPGUID) PURE;
};

typedef struct IDirectInput2W *LPDIRECTINPUT2W;

#undef INTERFACE
#define INTERFACE IDirectInput2A

DECLARE_INTERFACE_(IDirectInput2A, IDirectInputA)
{
    /*** IUnknown methods ***/
    STDMETHOD(QueryInterface)(THIS_ REFIID riid, LPVOID * ppvObj) PURE;
    STDMETHOD_(ULONG,AddRef)(THIS) PURE;
    STDMETHOD_(ULONG,Release)(THIS) PURE;

    /*** IDirectInputA methods ***/
    STDMETHOD(CreateDevice)(THIS_ REFGUID,LPDIRECTINPUTDEVICEA *,LPUNKNOWN) PURE;
    STDMETHOD(EnumDevices)(THIS_ DWORD,LPDIENUMDEVICESCALLBACKA,LPVOID,DWORD) PURE;
    STDMETHOD(GetDeviceStatus)(THIS_ REFGUID) PURE;
    STDMETHOD(RunControlPanel)(THIS_ HWND,DWORD) PURE;
    STDMETHOD(Initialize)(THIS_ HINSTANCE,DWORD) PURE;

    /*** IDirectInput2A methods ***/
    STDMETHOD(FindDevice)(THIS_ REFGUID,LPCSTR,LPGUID) PURE;
};

typedef struct IDirectInput2A *LPDIRECTINPUT2A;

#ifdef UNICODE
#define IID_IDirectInput2 IID_IDirectInput2W
#define IDirectInput2 IDirectInput2W
#define IDirectInput2Vtbl IDirectInput2WVtbl
#else
#define IID_IDirectInput2 IID_IDirectInput2A
#define IDirectInput2 IDirectInput2A
#define IDirectInput2Vtbl IDirectInput2AVtbl
#endif
typedef struct IDirectInput2 *LPDIRECTINPUT2;

#if !defined(__cplusplus) || defined(CINTERFACE)
#define IDirectInput2_QueryInterface(p,a,b) (p)->lpVtbl->QueryInterface(p,a,b)
#define IDirectInput2_AddRef(p) (p)->lpVtbl->AddRef(p)
#define IDirectInput2_Release(p) (p)->lpVtbl->Release(p)
#define IDirectInput2_CreateDevice(p,a,b,c) (p)->lpVtbl->CreateDevice(p,a,b,c)
#define IDirectInput2_EnumDevices(p,a,b,c,d) (p)->lpVtbl->EnumDevices(p,a,b,c,d)
#define IDirectInput2_GetDeviceStatus(p,a) (p)->lpVtbl->GetDeviceStatus(p,a)
#define IDirectInput2_RunControlPanel(p,a,b) (p)->lpVtbl->RunControlPanel(p,a,b)
#define IDirectInput2_Initialize(p,a,b) (p)->lpVtbl->Initialize(p,a,b)
#define IDirectInput2_FindDevice(p,a,b,c) (p)->lpVtbl->FindDevice(p,a,b,c)
#else
#define IDirectInput2_QueryInterface(p,a,b) (p)->QueryInterface(a,b)
#define IDirectInput2_AddRef(p) (p)->AddRef()
#define IDirectInput2_Release(p) (p)->Release()
#define IDirectInput2_CreateDevice(p,a,b,c) (p)->CreateDevice(a,b,c)
#define IDirectInput2_EnumDevices(p,a,b,c,d) (p)->EnumDevices(a,b,c,d)
#define IDirectInput2_GetDeviceStatus(p,a) (p)->GetDeviceStatus(a)
#define IDirectInput2_RunControlPanel(p,a,b) (p)->RunControlPanel(a,b)
#define IDirectInput2_Initialize(p,a,b) (p)->Initialize(a,b)
#define IDirectInput2_FindDevice(p,a,b,c) (p)->FindDevice(a,b,c)
#endif


#undef INTERFACE
#define INTERFACE IDirectInput7W

DECLARE_INTERFACE_(IDirectInput7W, IDirectInput2W)
{
    /*** IUnknown methods ***/
    STDMETHOD(QueryInterface)(THIS_ REFIID riid, LPVOID * ppvObj) PURE;
    STDMETHOD_(ULONG,AddRef)(THIS) PURE;
    STDMETHOD_(ULONG,Release)(THIS) PURE;

    /*** IDirectInput2W methods ***/
    STDMETHOD(CreateDevice)(THIS_ REFGUID,LPDIRECTINPUTDEVICEW *,LPUNKNOWN) PURE;
    STDMETHOD(EnumDevices)(THIS_ DWORD,LPDIENUMDEVICESCALLBACKW,LPVOID,DWORD) PURE;
    STDMETHOD(GetDeviceStatus)(THIS_ REFGUID) PURE;
    STDMETHOD(RunControlPanel)(THIS_ HWND,DWORD) PURE;
    STDMETHOD(Initialize)(THIS_ HINSTANCE,DWORD) PURE;
    STDMETHOD(FindDevice)(THIS_ REFGUID,LPCWSTR,LPGUID) PURE;

    /*** IDirectInput7W methods ***/
    STDMETHOD(CreateDeviceEx)(THIS_ REFGUID,REFIID,LPVOID *,LPUNKNOWN) PURE;
};

typedef struct IDirectInput7W *LPDIRECTINPUT7W;

#undef INTERFACE
#define INTERFACE IDirectInput7A

DECLARE_INTERFACE_(IDirectInput7A, IDirectInput2A)
{
    /*** IUnknown methods ***/
    STDMETHOD(QueryInterface)(THIS_ REFIID riid, LPVOID * ppvObj) PURE;
    STDMETHOD_(ULONG,AddRef)(THIS) PURE;
    STDMETHOD_(ULONG,Release)(THIS) PURE;

    /*** IDirectInput2A methods ***/
    STDMETHOD(CreateDevice)(THIS_ REFGUID,LPDIRECTINPUTDEVICEA *,LPUNKNOWN) PURE;
    STDMETHOD(EnumDevices)(THIS_ DWORD,LPDIENUMDEVICESCALLBACKA,LPVOID,DWORD) PURE;
    STDMETHOD(GetDeviceStatus)(THIS_ REFGUID) PURE;
    STDMETHOD(RunControlPanel)(THIS_ HWND,DWORD) PURE;
    STDMETHOD(Initialize)(THIS_ HINSTANCE,DWORD) PURE;
    STDMETHOD(FindDevice)(THIS_ REFGUID,LPCSTR,LPGUID) PURE;

    /*** IDirectInput7A methods ***/
    STDMETHOD(CreateDeviceEx)(THIS_ REFGUID,REFIID,LPVOID *,LPUNKNOWN) PURE;
};

typedef struct IDirectInput7A *LPDIRECTINPUT7A;

#ifdef UNICODE
#define IID_IDirectInput7 IID_IDirectInput7W
#define IDirectInput7 IDirectInput7W
#define IDirectInput7Vtbl IDirectInput7WVtbl
#else
#define IID_IDirectInput7 IID_IDirectInput7A
#define IDirectInput7 IDirectInput7A
#define IDirectInput7Vtbl IDirectInput7AVtbl
#endif
typedef struct IDirectInput7 *LPDIRECTINPUT7;

#if !defined(__cplusplus) || defined(CINTERFACE)
#define IDirectInput7_QueryInterface(p,a,b) (p)->lpVtbl->QueryInterface(p,a,b)
#define IDirectInput7_AddRef(p) (p)->lpVtbl->AddRef(p)
#define IDirectInput7_Release(p) (p)->lpVtbl->Release(p)
#define IDirectInput7_CreateDevice(p,a,b,c) (p)->lpVtbl->CreateDevice(p,a,b,c)
#define IDirectInput7_EnumDevices(p,a,b,c,d) (p)->lpVtbl->EnumDevices(p,a,b,c,d)
#define IDirectInput7_GetDeviceStatus(p,a) (p)->lpVtbl->GetDeviceStatus(p,a)
#define IDirectInput7_RunControlPanel(p,a,b) (p)->lpVtbl->RunControlPanel(p,a,b)
#define IDirectInput7_Initialize(p,a,b) (p)->lpVtbl->Initialize(p,a,b)
#define IDirectInput7_FindDevice(p,a,b,c) (p)->lpVtbl->FindDevice(p,a,b,c)
#define IDirectInput7_CreateDeviceEx(p,a,b,c,d) (p)->lpVtbl->CreateDeviceEx(p,a,b,c,d)
#else
#define IDirectInput7_QueryInterface(p,a,b) (p)->QueryInterface(a,b)
#define IDirectInput7_AddRef(p) (p)->AddRef()
#define IDirectInput7_Release(p) (p)->Release()
#define IDirectInput7_CreateDevice(p,a,b,c) (p)->CreateDevice(a,b,c)
#define IDirectInput7_EnumDevices(p,a,b,c,d) (p)->EnumDevices(a,b,c,d)
#define IDirectInput7_GetDeviceStatus(p,a) (p)->GetDeviceStatus(a)
#define IDirectInput7_RunControlPanel(p,a,b) (p)->RunControlPanel(a,b)
#define IDirectInput7_Initialize(p,a,b) (p)->Initialize(a,b)
#define IDirectInput7_FindDevice(p,a,b,c) (p)->FindDevice(a,b,c)
#define IDirectInput7_CreateDeviceEx(p,a,b,c,d) (p)->CreateDeviceEx(a,b,c,d)
#endif

#if DIRECTINPUT_VERSION > 0x0700
#undef INTERFACE
#define INTERFACE IDirectInput8W

DECLARE_INTERFACE_(IDirectInput8W, IUnknown)
{
    /*** IUnknown methods ***/
    STDMETHOD(QueryInterface)(THIS_ REFIID riid, LPVOID * ppvObj) PURE;
    STDMETHOD_(ULONG,AddRef)(THIS) PURE;
    STDMETHOD_(ULONG,Release)(THIS) PURE;

    /*** IDirectInput8W methods ***/
    STDMETHOD(CreateDevice)(THIS_ REFGUID,LPDIRECTINPUTDEVICE8W *,LPUNKNOWN) PURE;
    STDMETHOD(EnumDevices)(THIS_ DWORD,LPDIENUMDEVICESCALLBACKW,LPVOID,DWORD) PURE;
    STDMETHOD(GetDeviceStatus)(THIS_ REFGUID) PURE;
    STDMETHOD(RunControlPanel)(THIS_ HWND,DWORD) PURE;
    STDMETHOD(Initialize)(THIS_ HINSTANCE,DWORD) PURE;
    STDMETHOD(FindDevice)(THIS_ REFGUID,LPCWSTR,LPGUID) PURE;
    STDMETHOD(EnumDevicesBySemantics)(THIS_ LPCWSTR,LPDIACTIONFORMATW,LPDIENUMDEVICESBYSEMANTICSCBW,LPVOID,DWORD) PURE;
    STDMETHOD(ConfigureDevices)(THIS_ LPDICONFIGUREDEVICESCALLBACK,LPDICONFIGUREDEVICESPARAMSW,DWORD,LPVOID) PURE;
};

typedef struct IDirectInput8W *LPDIRECTINPUT8W;

#undef INTERFACE
#define INTERFACE IDirectInput8A

DECLARE_INTERFACE_(IDirectInput8A, IUnknown)
{
    /*** IUnknown methods ***/
    STDMETHOD(QueryInterface)(THIS_ REFIID riid, LPVOID * ppvObj) PURE;
    STDMETHOD_(ULONG,AddRef)(THIS) PURE;
    STDMETHOD_(ULONG,Release)(THIS) PURE;

    /*** IDirectInput8A methods ***/
    STDMETHOD(CreateDevice)(THIS_ REFGUID,LPDIRECTINPUTDEVICE8A *,LPUNKNOWN) PURE;
    STDMETHOD(EnumDevices)(THIS_ DWORD,LPDIENUMDEVICESCALLBACKA,LPVOID,DWORD) PURE;
    STDMETHOD(GetDeviceStatus)(THIS_ REFGUID) PURE;
    STDMETHOD(RunControlPanel)(THIS_ HWND,DWORD) PURE;
    STDMETHOD(Initialize)(THIS_ HINSTANCE,DWORD) PURE;
    STDMETHOD(FindDevice)(THIS_ REFGUID,LPCSTR,LPGUID) PURE;
    STDMETHOD(EnumDevicesBySemantics)(THIS_ LPCSTR,LPDIACTIONFORMATA,LPDIENUMDEVICESBYSEMANTICSCBA,LPVOID,DWORD) PURE;
    STDMETHOD(ConfigureDevices)(THIS_ LPDICONFIGUREDEVICESCALLBACK,LPDICONFIGUREDEVICESPARAMSA,DWORD,LPVOID) PURE;
};

typedef struct IDirectInput8A *LPDIRECTINPUT8A;

#ifdef UNICODE
#define IID_IDirectInput8 IID_IDirectInput8W
#define IDirectInput8 IDirectInput8W
#define IDirectInput8Vtbl IDirectInput8WVtbl
#else
#define IID_IDirectInput8 IID_IDirectInput8A
#define IDirectInput8 IDirectInput8A
#define IDirectInput8Vtbl IDirectInput8AVtbl
#endif
typedef struct IDirectInput8 *LPDIRECTINPUT8;

#if !defined(__cplusplus) || defined(CINTERFACE)
#define IDirectInput8_QueryInterface(p,a,b) (p)->lpVtbl->QueryInterface(p,a,b)
#define IDirectInput8_AddRef(p) (p)->lpVtbl->AddRef(p)
#define IDirectInput8_Release(p) (p)->lpVtbl->Release(p)
#define IDirectInput8_CreateDevice(p,a,b,c) (p)->lpVtbl->CreateDevice(p,a,b,c)
#define IDirectInput8_EnumDevices(p,a,b,c,d) (p)->lpVtbl->EnumDevices(p,a,b,c,d)
#define IDirectInput8_GetDeviceStatus(p,a) (p)->lpVtbl->GetDeviceStatus(p,a)
#define IDirectInput8_RunControlPanel(p,a,b) (p)->lpVtbl->RunControlPanel(p,a,b)
#define IDirectInput8_Initialize(p,a,b) (p)->lpVtbl->Initialize(p,a,b)
#define IDirectInput8_FindDevice(p,a,b,c) (p)->lpVtbl->FindDevice(p,a,b,c)
#define IDirectInput8_EnumDevicesBySemantics(p,a,b,c,d,e) (p)->lpVtbl->EnumDevicesBySemantics(p,a,b,c,d,e)
#define IDirectInput8_ConfigureDevices(p,a,b,c,d) (p)->lpVtbl->ConfigureDevices(p,a,b,c,d)
#else
#define IDirectInput8_QueryInterface(p,a,b) (p)->QueryInterface(a,b)
#define IDirectInput8_AddRef(p) (p)->AddRef()
#define IDirectInput8_Release(p) (p)->Release()
#define IDirectInput8_CreateDevice(p,a,b,c) (p)->CreateDevice(a,b,c)
#define IDirectInput8_EnumDevices(p,a,b,c,d) (p)->EnumDevices(a,b,c,d)
#define IDirectInput8_GetDeviceStatus(p,a) (p)->GetDeviceStatus(a)
#define IDirectInput8_RunControlPanel(p,a,b) (p)->RunControlPanel(a,b)
#define IDirectInput8_Initialize(p,a,b) (p)->Initialize(a,b)
#define IDirectInput8_FindDevice(p,a,b,c) (p)->FindDevice(a,b,c)
#define IDirectInput8_EnumDevicesBySemantics(p,a,b,c,d,e) (p)->EnumDevicesBySemantics(a,b,c,d,e)
#define IDirectInput8_ConfigureDevices(p,a,b,c,d) (p)->ConfigureDevices(a,b,c,d)
#endif
#endif /* DIRECTINPUT_VERSION > 0x0700 */

#if DIRECTINPUT_VERSION > 0x0700

extern HRESULT WINAPI DirectInput8Create(HINSTANCE hinst, DWORD dwVersion, REFIID riidltf, LPVOID *ppvOut, LPUNKNOWN punkOuter);

#else
extern HRESULT WINAPI DirectInputCreateA(HINSTANCE hinst, DWORD dwVersion, LPDIRECTINPUTA *ppDI, LPUNKNOWN punkOuter);
extern HRESULT WINAPI DirectInputCreateW(HINSTANCE hinst, DWORD dwVersion, LPDIRECTINPUTW *ppDI, LPUNKNOWN punkOuter);
#ifdef UNICODE
#define DirectInputCreate  DirectInputCreateW
#else
#define DirectInputCreate  DirectInputCreateA
#endif // !UNICODE

extern HRESULT WINAPI DirectInputCreateEx(HINSTANCE hinst, DWORD dwVersion, REFIID riidltf, LPVOID *ppvOut, LPUNKNOWN punkOuter);

#endif /* DIRECTINPUT_VERSION > 0x700 */

#endif /* DIJ_RINGZERO */


/****************************************************************************
 *
 *  Return Codes
 *
 ****************************************************************************/

/*
 *  The operation completed successfully.
 */
#define DI_OK                           S_OK

/*
 *  The device exists but is not currently attached.
 */
#define DI_NOTATTACHED                  S_FALSE

/*
 *  The device buffer overflowed.  Some input was lost.
 */
#define DI_BUFFEROVERFLOW               S_FALSE

/*
 *  The change in device properties had no effect.
 */
#define DI_PROPNOEFFECT                 S_FALSE

/*
 *  The operation had no effect.
 */
#define DI_NOEFFECT                     S_FALSE

/*
 *  The device is a polled device.  As a result, device buffering
 *  will not collect any data and event notifications will not be
 *  signalled until GetDeviceState is called.
 */
#define DI_POLLEDDEVICE                 ((HRESULT)0x00000002L)

/*
 *  The parameters of the effect were successfully updated by
 *  IDirectInputEffect::SetParameters, but the effect was not
 *  downloaded because the device is not exclusively acquired
 *  or because the DIEP_NODOWNLOAD flag was passed.
 */
#define DI_DOWNLOADSKIPPED              ((HRESULT)0x00000003L)

/*
 *  The parameters of the effect were successfully updated by
 *  IDirectInputEffect::SetParameters, but in order to change
 *  the parameters, the effect needed to be restarted.
 */
#define DI_EFFECTRESTARTED              ((HRESULT)0x00000004L)

/*
 *  The settings have been successfully applied but could not be 
 *  persisted because access to the persistence media was denied.
 */
#define DI_SETTINGSNOTSAVED_ACCESSDENIED    ((HRESULT)0x00000005L)

/*
 *  The settings have been successfully applied but could not be 
 *  persisted because the persistence media did not have enough space.
 */
#define DI_SETTINGSNOTSAVED_DISKFULL        ((HRESULT)0x00000006L)

/*
 *  The parameters of the effect were successfully updated by
 *  IDirectInputEffect::SetParameters, but some of them were
 *  beyond the capabilities of the device and were truncated.
 */
#define DI_TRUNCATED                    ((HRESULT)0x00000008L)

/*
 *  Equal to DI_EFFECTRESTARTED | DI_TRUNCATED.
 */
#define DI_TRUNCATEDANDRESTARTED        ((HRESULT)0x0000000CL)

/*
 *  The application requires a newer version of DirectInput.
 */
#define DIERR_OLDDIRECTINPUTVERSION     \
    MAKE_HRESULT(SEVERITY_ERROR, FACILITY_WIN32, ERROR_OLD_WIN_VERSION)

/*
 *  The application was written for an unsupported prerelease version
 *  of DirectInput.
 */
#define DIERR_BETADIRECTINPUTVERSION    \
    MAKE_HRESULT(SEVERITY_ERROR, FACILITY_WIN32, ERROR_RMODE_APP)

/*
 *  The object could not be created due to an incompatible driver version
 *  or mismatched or incomplete driver components.
 */
#define DIERR_BADDRIVERVER              \
    MAKE_HRESULT(SEVERITY_ERROR, FACILITY_WIN32, ERROR_BAD_DRIVER_LEVEL)

/*
 * The device or device instance or effect is not registered with DirectInput.
 */
#define DIERR_DEVICENOTREG              REGDB_E_CLASSNOTREG

/*
 * The requested object does not exist.
 */
#define DIERR_NOTFOUND                  \
    MAKE_HRESULT(SEVERITY_ERROR, FACILITY_WIN32, ERROR_FILE_NOT_FOUND)

/*
 * The requested object does not exist.
 */
#define DIERR_OBJECTNOTFOUND            \
    MAKE_HRESULT(SEVERITY_ERROR, FACILITY_WIN32, ERROR_FILE_NOT_FOUND)

/*
 * An invalid parameter was passed to the returning function,
 * or the object was not in a state that admitted the function
 * to be called.
 */
#define DIERR_INVALIDPARAM              E_INVALIDARG

/*
 * The specified interface is not supported by the object
 */
#define DIERR_NOINTERFACE               E_NOINTERFACE

/*
 * An undetermined error occured inside the DInput subsystem
 */
#define DIERR_GENERIC                   E_FAIL

/*
 * The DInput subsystem couldn't allocate sufficient memory to complete the
 * caller's request.
 */
#define DIERR_OUTOFMEMORY               E_OUTOFMEMORY

/*
 * The function called is not supported at this time
 */
#define DIERR_UNSUPPORTED               E_NOTIMPL

/*
 * This object has not been initialized
 */
#define DIERR_NOTINITIALIZED            \
    MAKE_HRESULT(SEVERITY_ERROR, FACILITY_WIN32, ERROR_NOT_READY)

/*
 * This object is already initialized
 */
#define DIERR_ALREADYINITIALIZED        \
    MAKE_HRESULT(SEVERITY_ERROR, FACILITY_WIN32, ERROR_ALREADY_INITIALIZED)

/*
 * This object does not support aggregation
 */
#define DIERR_NOAGGREGATION             CLASS_E_NOAGGREGATION

/*
 * Another app has a higher priority level, preventing this call from
 * succeeding.
 */
#define DIERR_OTHERAPPHASPRIO           E_ACCESSDENIED

/*
 * Access to the device has been lost.  It must be re-acquired.
 */
#define DIERR_INPUTLOST                 \
    MAKE_HRESULT(SEVERITY_ERROR, FACILITY_WIN32, ERROR_READ_FAULT)

/*
 * The operation cannot be performed while the device is acquired.
 */
#define DIERR_ACQUIRED                  \
    MAKE_HRESULT(SEVERITY_ERROR, FACILITY_WIN32, ERROR_BUSY)

/*
 * The operation cannot be performed unless the device is acquired.
 */
#define DIERR_NOTACQUIRED               \
    MAKE_HRESULT(SEVERITY_ERROR, FACILITY_WIN32, ERROR_INVALID_ACCESS)

/*
 * The specified property cannot be changed.
 */
#define DIERR_READONLY                  E_ACCESSDENIED

/*
 * The device already has an event notification associated with it.
 */
#define DIERR_HANDLEEXISTS              E_ACCESSDENIED

/*
 * The device already has an enumeration callback for the desired notification.
 */
#define DIERR_CALLBACKEXISTS             E_ACCESSDENIED
/*
 * There is no enumeration callback to cancel.
 */
#define DIERR_NOCALLBACK				E_ACCESSDENIED

/*
 * Data is not yet available.
 */
#ifndef E_PENDING
#define E_PENDING                       0x8000000AL
#endif

/*
 * Unable to IDirectInputJoyConfig_Acquire because the user
 * does not have sufficient privileges to change the joystick
 * configuration.
 */
#define DIERR_INSUFFICIENTPRIVS         0x80040200L

/*
 * The device is full.
 */
#define DIERR_DEVICEFULL                0x80040201L

/*
 * Not all the requested information fit into the buffer.
 */
#define DIERR_MOREDATA                  0x80040202L

/*
 * The effect is not downloaded.
 */
#define DIERR_NOTDOWNLOADED             0x80040203L

/*
 *  The device cannot be reinitialized because there are still effects
 *  attached to it.
 */
#define DIERR_HASEFFECTS                0x80040204L

/*
 *  The operation cannot be performed unless the device is acquired
 *  in DISCL_EXCLUSIVE mode.
 */
#define DIERR_NOTEXCLUSIVEACQUIRED      0x80040205L

/*
 *  The effect could not be downloaded because essential information
 *  is missing.  For example, no axes have been associated with the
 *  effect, or no type-specific information has been created.
 */
#define DIERR_INCOMPLETEEFFECT          0x80040206L

/*
 *  Attempted to read buffered device data from a device that is
 *  not buffered.
 */
#define DIERR_NOTBUFFERED               0x80040207L

/*
 *  An attempt was made to modify parameters of an effect while it is
 *  playing.  Not all hardware devices support altering the parameters
 *  of an effect while it is playing.
 */
#define DIERR_EFFECTPLAYING             0x80040208L

/*
 *  The operation could not be completed because the device is not
 *  plugged in.
 */
#define DIERR_UNPLUGGED                 0x80040209L

/*
 *  SendDeviceData failed because more information was requested
 *  to be sent than can be sent to the device.  Some devices have
 *  restrictions on how much data can be sent to them.  (For example,
 *  there might be a limit on the number of buttons that can be
 *  pressed at once.)
 */
#define DIERR_REPORTFULL                0x8004020AL


#ifdef __cplusplus
};
#endif

#endif  /* __DINPUT_INCLUDED__ */
