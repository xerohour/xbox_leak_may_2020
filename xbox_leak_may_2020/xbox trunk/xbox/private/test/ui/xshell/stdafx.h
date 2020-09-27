#ifndef _STDAFX_H_
#define _STDAFX_H_

#define BREAK_INTO_DEBUGGER     _asm { int 3 }
#define ARRAYSIZE(a)            (sizeof(a) / sizeof(a[0]))
#define XFONT_TRUETYPE

#include <ntos.h>
#include <xapip.h>
#include <xtl.h>
#include <xfont.h>
#include <xdbg.h>
#include <stdio.h>
#include <xgmath.h>
#include <xbox.h>
#include <xboxp.h>
#include <xonlinep.h>
#include <dm.h>

// Libs
#include "usbmanager.h"
#include "xsettings.h"
#include "harddrive.h"
#include "memoryunit.h"
#include "linkedlist.h"
#include "bitmapfile.h"

#include "xfontfuncs.h"
#include "udtexture.h"
#include "soundcollection.h"
#include "xitem.h"
#include "constants.h"
#include "genkeyboardmenu.h"
#include "messagemenu.h"
#include "madisplaymenu.h"
#include "yesnomenu.h"

// Forward declarations
HRESULT ProcessInput( void );
HRESULT SetupMenusAndItems( void );
void CheckMUs( void );
HRESULT CleanUp( void );
HRESULT InitDSound( void );
HRESULT InitD3D( UINT uiWidth,
                 UINT uiHeight,
                 DWORD dwFlags );
HRESULT Init3DObjects( void );
HRESULT Init( void );
unsigned int GetScreenWidth();      // Get the Width of the screen
unsigned int GetScreenHeight();     // Get the Height of the screen
void UpdateAddressTexture( CUDTexture* pTexture );
void UpdateTitleTexture( CUDTexture* pTexture );
void AlphaTexture( IDirect3DTexture8* pTexture,
                   DWORD dwColor );
void ProcessKeySequences();
DWORD WINAPI XGetLaunchInfoShell( OUT PDWORD pdwLaunchDataType,
                                  OUT PLD_LAUNCH_DASHBOARD pLaunchDashboard );
void DisplayMessage( WCHAR* pwszTitle,              // Used to display a generic message to the user
                     WCHAR* pwszFormat, ... );
void DisplayErrorMessage( WCHAR* pwszFormat, ... ); // Used to display a generic error message to the user
void DisplayYesNoMessage( WCHAR* pwszTitle,         // Used to display a generic YES/NO message to the user
                          WCHAR* pwszFormat, ... );
BOOL YesNoMenuCancelled();                          // Used to determine if the Yes/No menu was cancelled

#endif // _STDAFX_H_