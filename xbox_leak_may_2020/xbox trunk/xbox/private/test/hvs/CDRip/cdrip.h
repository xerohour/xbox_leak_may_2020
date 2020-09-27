/*****************************************************************************

Copyright (C) Microsoft Corporation.  All rights reserved.

Module Name:

    CDRip.h

Abstract:

    Audio CD Ripping program

Notes:

*****************************************************************************/

#ifndef _CDRIP_H_
#define _CDRIP_H_

#ifdef _VC
//#define DEVICE_TYPE ULONG
#define NTLEANANDMEAN
#define _X86_
#define _WIN32_WINNT 0x0500
#endif

#pragma warning(disable: 4200) // nonstandard extension used : zero-sized array in struct/union

#include <ntos.h>
#include <nt.h>
#include <ntrtl.h>
#include <nturtl.h>
#include <ntexapi.h>
#include <scsi.h>
#include <ntddscsi.h>
#include <ntdddisk.h>
#include <ntddcdrm.h>
#include <ntddcdvd.h>

#include <dm.h>
#include <xtl.h>
#include <stdio.h>
#include <smcdef.h>
#include <wmfencode.h>

#include "../utils/hvsUtils.h"
#include "bitfont.h"
#include "ntiosvc.h"
#include "cdda.h"

extern CNtIoctlCdromService g_DVDRom;


//
// utility functions
//
void DebugPrint( char* format,
                 ... );

long GetNumber( char *string );

void GetDriveID( HANDLE hDevice,
                 bool cdrom,
                 char* model,
                 char* serial,
                 char* firmware );

void RebootToShell( DWORD dwNumPassed,  // Used to reboot to our launching program
                    DWORD dwNumFailed,
                    char* pszNotes );

int CopyAudioTrack( int nTrack );      // Copy a Track from the Audio CD to the Hard Disc


bool CompressAudio( char* pszSrcPath,   // Compress our audio file in to a WMA File
                    char* pszDestPath,
                    short sMax );

DWORD WINAPI AudioTrackCopyThread( LPVOID lpParameter );    // Copy Audio Track Thread Function

//
// Initialization calls
//
void Initialize( void );
void CleanUp( void );
HRESULT DeInitGraphics( void );
HRESULT InitGraphics( void );

//
// User input
// 
void InputDukeInsertions(DWORD add, DWORD remove=0);
void InputMUInsertions(DWORD add, DWORD remove=0);
bool InputCheckExitCombo(void);
bool InputCheckButton(int button);
bool InputCheckDigitalButton(int button);
void Pause(void);
int ThumbStickScale(int stick, int max);
void AdjustValue(DWORD &value, DWORD min, DWORD max, XINPUT_STATE &input, DWORD maxThumbStickScale);


//
// Graphics / GUI methods
//
enum DISPLAY_MODE
    {
    DISPLAYMODE_PAUSE,
    DISPLAYMODE_RUN_TIME,
    DISPLAYMODE_CONFIG_READONLY,
    DISPLAYMODE_CONFIG_INTERACTIVE
    };
void UpdateGraphics(unsigned mode = DISPLAYMODE_RUN_TIME);
void GUI(void);
void GraphicPrint(IDirect3DSurface8* pd3ds,
                  unsigned x,
                  unsigned line,
                  DWORD color,
                  char* format,
                  ... );


//
// Utility functions
//
void HexDump(const unsigned char * buffer, DWORD length);
WCHAR* Ansi2UnicodeHack(char *str);
BOOL CompareBuff( unsigned char *buff1, unsigned char *buff2, unsigned length );
int GetDiscType();
void RandomizeTracks( DWORD* adwTracks );

//
// Protected/Shared Memory Functions
//
DWORD GetCurrentTrack( void );
void SetCurrentTrack( DWORD dwCurrentTrack );
void IncCurrentTrack( void );

DWORD GetTracksRipped( void );
void SetTracksRipped( DWORD dwTracksRipped );
void IncTracksRipped( void );

BOOL GetExitThread( void );
void SetExitThread( BOOL bThreadExit );

DWORD GetTotalRipTime( void );
void SetTotalRipTime( DWORD dwRipTime );

float GetMegabytesRipped( void );
float GetMegabytesRippedPerSecond();
void SetMegabytesRipped( float fMegabytesRipped );


//
// Media Types
//
#define DISC_NONE               0
#define DISC_BAD                1
#define DISC_TITLE              2
#define DISC_AUDIO              3
#define DISC_VIDEO              4

//
// CD Rip Info
//
#define BLOCK_SIZE              16384
#define COPY_NO_ERROR           0
#define COPY_HARDDISK_FULL      1
#define COPY_TOO_MANY_SONGS     2
#define COPY_DISC_EJECTED       3
#define COPY_DISC_READ_ERROR    4

//
// User input
//
extern HANDLE hDuke[XGetPortCount()];
extern HANDLE hSlot[XGetPortCount()*2];
static const DWORD USBPortMasks[] = {
    XDEVICE_PORT0_MASK,
    XDEVICE_PORT1_MASK,
    XDEVICE_PORT2_MASK,
    XDEVICE_PORT3_MASK
    };
static const DWORD USBSlotMasks[] = {
    XDEVICE_PORT0_BOTTOM_MASK,
    XDEVICE_PORT0_TOP_MASK,
    XDEVICE_PORT1_BOTTOM_MASK,
    XDEVICE_PORT1_TOP_MASK,
    XDEVICE_PORT2_BOTTOM_MASK,
    XDEVICE_PORT2_TOP_MASK,
    XDEVICE_PORT3_BOTTOM_MASK,
    XDEVICE_PORT3_TOP_MASK
    };

//
// config settings
//
struct ConfigSettings : public CoreConfigSettings
{
    char pszCDTitle[MAX_PATH];
    char pszCDSerial[MAX_PATH];
    DWORD dwStartTrack;
    DWORD dwEndTrack;
    DWORD dwRipSequence;
    DWORD dwRandSeed;
    DWORD dwPauseOnExit;

    ConfigSettings()
    {
        ZeroMemory( pszCDTitle, MAX_PATH );
        ZeroMemory( pszCDSerial, MAX_PATH );
        dwStartTrack = 0;
        dwEndTrack = 0;
        dwRipSequence = 0;
        dwRandSeed = 0;
        dwPauseOnExit = 0;
    }
};

extern ConfigSettings globalSettings;

//
// stats
//
extern DWORD g_dwTrackRipStartTime;
extern DWORD g_dwTrackRipEndTime;
extern DWORD g_dwNumTimesThrough;
extern DWORD g_dwTestPassNumber;
extern DWORD g_dwNumPass;
extern DWORD g_dwNumFail;
extern DWORD g_dwTime;
extern char g_pszModel[64];
extern char g_pszSerial[64];
extern char g_pszFirmware[64];
extern BYTE* g_pbyTrackBuffer;
extern DWORD* g_adwTracks;

//
// graphics
//
extern IDirect3D8*		    g_d3d;
extern IDirect3DDevice8*	g_pDevice;
extern BitFont				g_font;
extern IDirect3DTexture8*	g_pd3dtText;
extern TVertex				g_prText[4];
extern D3DLOCKED_RECT		g_d3dlr;

#endif //_CDRIP_H_
