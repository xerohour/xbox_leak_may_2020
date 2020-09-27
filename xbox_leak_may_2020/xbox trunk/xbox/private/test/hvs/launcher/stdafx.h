#ifndef _STDAFX_H_
#define _STDAFX_H_

#define XFONT_TRUETYPE
#define APP_TITLE_NAME                                  "HVSLauncher"

/////////////////////////////////
// Includes
/////////////////////////////////
#include <ntos.h>
#include <xtl.h>
#include <xfontfuncs.h>
// #include <xdbg.h>
#include <stdio.h>
#include <hvsUtils.h>

// UI Libs / Tools
#include <linkedlist.h>
#include <usbmanager.h>

///////////////////////////
// Constants and Structures
///////////////////////////

// Colors
const DWORD         gc_dwCOLOR_WHITE =                              0xFFFFFFFF;
const DWORD         gc_dwCOLOR_BLACK =                              0xFF000000;
const DWORD         gc_dwCOLOR_RED =                                0xFFFF0000;
const DWORD         gc_dwCOLOR_GREEN =                              0xFF00FF00;
const DWORD         gc_dwCOLOR_DARK_GREEN =                         0xFF276F07;
const DWORD         gc_dwCOLOR_BLUE =                               0xFF0000FF;
const DWORD         gc_dwCOLOR_DARK_GREY =                          0xFF808080;
const DWORD         gc_dwCOLOR_LIGHT_GREY =                         0xFFC0C0C0;
const DWORD         gc_dwCOLOR_DIM_GREY =                           0xFF272727;
const DWORD         gc_dwCOLOR_DIM_YELLOW =                         0xFFC0BF5A;
const DWORD         gc_dwCOLOR_BRIGHT_YELLOW =                      0xFFC0BF5A;

const DWORD         gc_dwCOLOR_TVSAFE_GREEN =                       0xFF2CDF2C;
const DWORD         gc_dwCOLOR_TVSAFE_RED =                         0xFF8D3232;
const DWORD         gc_dwCOLOR_TVSAFE_BLUE =                        0xFF20209F;
const DWORD         gc_dwCOLOR_TVSAFE_YELLOW =                      0xFFF0F02F;

const DWORD         gc_dwCOLOR_TEXT_GREEN =                         0xFF34DD07;
const DWORD         gc_dwCOLOR_TEXT_YELLOW =                        0xFFFFF200;
const DWORD         gc_dwCOLOR_SELECTOR_BLACK =                     0x66000000;

// Button Colors
const DWORD         gc_dwCOLOR_A_BUTTON =                           gc_dwCOLOR_TVSAFE_GREEN;
const DWORD         gc_dwCOLOR_B_BUTTON =                           gc_dwCOLOR_TVSAFE_RED;
const DWORD         gc_dwCOLOR_X_BUTTON =                           gc_dwCOLOR_TVSAFE_BLUE;
const DWORD         gc_dwCOLOR_Y_BUTTON =                           gc_dwCOLOR_TVSAFE_YELLOW;
const DWORD         gc_dwCOLOR_L_BUTTON =                           gc_dwCOLOR_DARK_GREY;
const DWORD         gc_dwCOLOR_R_BUTTON =                           gc_dwCOLOR_DARK_GREY;
const DWORD         gc_dwCOLOR_WHITE_BUTTON =                       gc_dwCOLOR_WHITE;
const DWORD         gc_dwCOLOR_BLACK_BUTTON =                       gc_dwCOLOR_DIM_GREY;
const DWORD         gc_dwCOLOR_START_BUTTON =                       gc_dwCOLOR_DARK_GREY;
const DWORD         gc_dwCOLOR_BACK_BUTTON =                        gc_dwCOLOR_DARK_GREY;

const float         gc_fBUTTON_WIDTH_ADJUST =                       22;

// Program information
const unsigned int  gc_uiCACHE_SIZE =                               (1024 * 1024 * 8);	// Eight Megs
const WCHAR         gc_pwszFONT_HEADER_FILENAME[] =                 L"d:\\media\\fonts\\xbox.ttf";
const WCHAR         gc_pwszFONT_BODY_FILENAME[] =                   L"d:\\media\\fonts\\xbox_rg.ttf";
const WCHAR         gc_pwszFONT_BUTTON_FILENAME[] =                 L"d:\\media\\fonts\\xboxdings.ttf";
const unsigned int  gc_uiDEFAULT_FONT_HEIGHT =                      20;
const unsigned int  gc_uiDEFAULT_FONT_ALIAS_LEVEL =                 0;
const unsigned int  gc_uiSCREEN_HEIGHT =                            480;
const unsigned int  gc_uiSCREEN_WIDTH =                             640;
const char          gc_pszAutoRunFile[] =                           "T:\\AUTORUN.TXT";
const char          gc_pszResultsFile[] =                           "T:\\RESULTS.TXT";
const char          gc_pszCurrentTestInfoFile[] =                   "T:\\RUNNINGTEST.TXT";
const char          gc_pszRunningTestsPath[] =                      "T:\\RUNNINGTESTS";
                                                                    
// Input constants
const int           gc_nBUTTON_THRESHOLD =                          50;    // 0 to 255
const int           gc_nJOYSTICK_DEADZONE =                         20000; // 0 to 32768

// Default Texture Vertex Type
struct MYTEXVERTEX
{
    D3DXVECTOR3 v;
    float       fRHW;
    D3DCOLOR    diffuse;
    float       tu, tv;    // The texture coordinates
};

#define D3DFVF_MYTEXVERTEX  ( D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1 )

///////////////////////////////////
// HVS Launcher dependent includes
///////////////////////////////////
#include "hvslfunctions.h"

// Libs

// Menus and Commands


#endif // _STDAFX_H_