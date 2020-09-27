//-----------------------------------------------------------------------------
// File: Farm.cpp
//
// Desc: Plays a script file using DirectMusic
//
//@@BEGIN_MSINTERNAL
//
// Hist: 02.24.00 - forrest - Created from PlaySegment
//
//@@END_MSINTERNAL
// Copyright (c) 2000 Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#define STRICT
#include <windows.h>
#include <basetsd.h>
#include <commdlg.h>
#include <objbase.h>
#include <stdio.h>
#include <dmusicc.h>
#include <dmusici.h>
#include <dxerr8.h>
#include "resource.h"
#include <tchar.h>
#include "DMUtil.h"
#include "DXUtil.h"




//-----------------------------------------------------------------------------
// Function-prototypes
//-----------------------------------------------------------------------------
INT_PTR CALLBACK MainDlgProc( HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam );
HRESULT OnInitDialog( HWND hDlg );
HRESULT OnChangeScriptFile( HWND hDlg, TCHAR* strFileName );
HRESULT OnCallRoutine( HWND hDlg, TCHAR* strRoutine );
HRESULT UpdateVariables( HWND hDlg );
HRESULT SetVariable( HWND hDlg, int nIDDlgItem );
void    DrawButton( int nDlgItem, HWND hDlgItem, LPDRAWITEMSTRUCT lpDrawItemStruct );
void    DrawStatic( int nDlgItem, HWND hDlgItem, LPDRAWITEMSTRUCT lpDrawItemStruct );
void    PlayButton( HWND hDlg, int nDlgItem );
HRESULT ProcessDirectMusicMessages( HWND hDlg );
BOOL    LoadBitmapFromResource( int nResourceID, HBITMAP *phBitmap, HPALETTE *phPalette );
HRESULT CreateAudioPathFromFile( IDirectMusicAudioPath** ppAudioPath, TCHAR* strFileName );

#define ORANGE_COLOR        RGB(255, 130, 0)
#define GREEN_COLOR         RGB(0, 195, 0)
#define YELLOW_COLOR        RGB(198, 195, 0)
#define LT_ORANGE_COLOR     RGB(255, 195, 132)
#define LT_GREEN_COLOR      RGB(132, 255, 132)
#define LT_YELLOW_COLOR     RGB(255, 255, 198)
#define RED_COLOR           RGB(255, 0, 0)
#define BACKGROUND_COLOR    RGB(198, 65, 0)
#define BLACK_COLOR         RGB(0, 0, 0)
#define WHITE_COLOR         RGB(255, 255, 255)




//-----------------------------------------------------------------------------
// Defines, constants, and global variables
//-----------------------------------------------------------------------------
#define NUM_PLAY_BUTTONS (IDC_ALARM - IDC_BIRD + 1)

CMusicManager* g_pMusicManager      = NULL;
CMusicScript*  g_pMusicScript       = NULL;
IDirectMusicAudioPath* g_pFarmAudioPath = NULL;
HINSTANCE      g_hInst              = NULL;
HBRUSH         g_hOrangeBrush       = NULL;
HBRUSH         g_hGreenBrush        = NULL;
HBRUSH         g_hYellowBrush       = NULL;
HBRUSH         g_hLtOrangeBrush     = NULL;
HBRUSH         g_hLtGreenBrush      = NULL;
HBRUSH         g_hLtYellowBrush     = NULL;
HBRUSH         g_hBackgroundBrush   = NULL;
HBRUSH         g_hRedBrush          = NULL;
BOOL           g_bButtonPlaying[ NUM_PLAY_BUTTONS ];
HBITMAP        g_hBitmap            = NULL;
HPALETTE       g_hPalette           = NULL;





//-----------------------------------------------------------------------------
// Name: WinMain()
// Desc: Entry point for the application.  Since we use a simple dialog for 
//       user interaction we don't need to pump messages.
//-----------------------------------------------------------------------------
INT APIENTRY WinMain( HINSTANCE hInst, HINSTANCE hPrevInst, LPSTR pCmdLine, 
                      INT nCmdShow )
{
    HWND    hDlg = NULL;
    BOOL    bDone = FALSE;
    int     nExitCode;
    MSG     msg;

    g_hInst = hInst;
    ZeroMemory( g_bButtonPlaying, sizeof(BOOL)*NUM_PLAY_BUTTONS );

    // Display the main dialog box.
    hDlg = CreateDialog( hInst, MAKEINTRESOURCE(IDD_MAIN), 
                         NULL, MainDlgProc );

    while( !bDone ) 
    { 
        // Windows messages are available
        while( PeekMessage( &msg, NULL, 0, 0, PM_REMOVE ) ) 
        { 
            if( !IsDialogMessage( hDlg, &msg ) )  
            {
                TranslateMessage( &msg ); 
                DispatchMessage( &msg ); 
            }

            if( msg.message == WM_QUIT )
            {
                nExitCode = (int)msg.wParam;
                bDone     = TRUE;
                DestroyWindow( hDlg );
            }
        }
    }

    return nExitCode;
}




//-----------------------------------------------------------------------------
// Name: MainDlgProc()
// Desc: Handles dialog messages
//-----------------------------------------------------------------------------
INT_PTR CALLBACK MainDlgProc( HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam )
{
    HRESULT hr;

    switch( msg ) 
    {
        case WM_INITDIALOG:
            if( FAILED( hr = OnInitDialog( hDlg ) ) )
            {
                DXTRACE_ERR( TEXT("OnInitDialog"), hr );
                MessageBox( hDlg, "Error initializing DirectMusic.  Sample will now exit.", 
                                  "DirectMusic Sample", MB_OK | MB_ICONERROR );
                PostQuitMessage( IDCANCEL );
                return TRUE;
            }
            break;

        case WM_PAINT:
        {
            PAINTSTRUCT   ps;
            HBITMAP       hOldBitmap;
            HPALETTE      hOldPalette;
            HDC           hDC, hMemDC;
            BITMAP        bm;

            hDC = BeginPaint( hDlg, &ps );

            if( g_hBitmap )
            {
                GetObject( g_hBitmap, sizeof(BITMAP), &bm );
                hMemDC = CreateCompatibleDC( hDC );
                hOldBitmap = (HBITMAP)SelectObject( hMemDC, g_hBitmap );
                hOldPalette = SelectPalette( hDC, g_hPalette, FALSE );
                RealizePalette( hDC );

                BitBlt( hDC, 0, 0, bm.bmWidth, bm.bmHeight, hMemDC, 0, 0, SRCCOPY );

                SelectObject( hMemDC, hOldBitmap );
                SelectPalette( hDC, hOldPalette, FALSE );
            }

            EndPaint( hDlg, &ps );
            break;
        }

        case WM_DRAWITEM:
            switch( ((LPDRAWITEMSTRUCT) lParam)->CtlType )
            {
            case ODT_BUTTON:
                DrawButton( wParam, GetDlgItem(hDlg, wParam), (LPDRAWITEMSTRUCT) lParam );
                break;

            case ODT_STATIC:
                DrawStatic( wParam, GetDlgItem(hDlg, wParam), (LPDRAWITEMSTRUCT) lParam );
                break;

            default:
                return FALSE;
            }

            return TRUE;
            break;

        case WM_COMMAND:
            switch( LOWORD(wParam) )
            {
                case IDC_ALLSTOP:
                case IDC_BIRD:
                case IDC_NIGHT:
                case IDC_PREDAWN:
                case IDC_DAWN:
                case IDC_END:
                case IDC_COUGAR:
                case IDC_COW:
                case IDC_ROOSTER:
                case IDC_SHEEP:
                case IDC_WOLF:
                case IDC_ALARM:
                {
                    PlayButton( hDlg, LOWORD(wParam) );
                    break;
                }

                case IDCANCEL:
                {
                    PostQuitMessage( IDCANCEL );
                    break;
                }

                default:
                {
                    return FALSE; // Didn't handle message
                }
            }
            break;

        case WM_DESTROY:
            // Cleanup everything
            SAFE_RELEASE( g_pFarmAudioPath );
            SAFE_DELETE( g_pMusicScript );
            SAFE_DELETE( g_pMusicManager );

            DeleteObject( g_hOrangeBrush );
            DeleteObject( g_hGreenBrush );
            DeleteObject( g_hYellowBrush );
            DeleteObject( g_hLtOrangeBrush );
            DeleteObject( g_hLtGreenBrush );
            DeleteObject( g_hLtYellowBrush );
            DeleteObject( g_hRedBrush );
            DeleteObject( g_hBackgroundBrush );
            DeleteObject( g_hBitmap );
            DeleteObject( g_hPalette );

            break; 

        default:
            return FALSE; // Didn't handle message
    }

    return TRUE; // Handled message
}




//-----------------------------------------------------------------------------
// Name: OnInitDialog()
// Desc: Initializes the dialogs (sets up UI controls, etc.)
//-----------------------------------------------------------------------------
HRESULT OnInitDialog( HWND hDlg )
{
    HRESULT hr;

    // Load the icon
    HICON hIcon = LoadIcon( g_hInst, MAKEINTRESOURCE( IDR_MAINFRAME ) );

    // Set the icon for this dialog.
    SendMessage( hDlg, WM_SETICON, ICON_BIG,   (LPARAM) hIcon );  // Set big icon
    SendMessage( hDlg, WM_SETICON, ICON_SMALL, (LPARAM) hIcon );  // Set small icon

    g_hOrangeBrush    = CreateSolidBrush(ORANGE_COLOR);
    g_hGreenBrush     = CreateSolidBrush(GREEN_COLOR);
    g_hYellowBrush    = CreateSolidBrush(YELLOW_COLOR);
    g_hLtOrangeBrush  = CreateSolidBrush(LT_ORANGE_COLOR);
    g_hLtGreenBrush   = CreateSolidBrush(LT_GREEN_COLOR);
    g_hLtYellowBrush  = CreateSolidBrush(LT_YELLOW_COLOR);
    g_hRedBrush       = CreateSolidBrush(RED_COLOR);
    g_hBackgroundBrush= CreateSolidBrush(BACKGROUND_COLOR);

    g_pMusicManager = new CMusicManager();

    if( FAILED( hr = g_pMusicManager->Initialize( hDlg ) ) )
        return hr;

    // Free any previous script, and make a new one
    SAFE_DELETE( g_pMusicScript );

    // Have the loader collect any garbage now that the old 
    // script has been released
    g_pMusicManager->CollectGarbage();

    // Set the default media path (something like C:\MSSDK\SAMPLES\MULTIMEDIA\MEDIA)
    // to be used as the search directory for finding DirectMusic content.
    TCHAR strPath[MAX_PATH];

    GetCurrentDirectory( MAX_PATH, strPath );
    lstrcat( strPath, TEXT("\\res") );
    g_pMusicManager->SetSearchDirectory( strPath );

    // Load the script file
    if( FAILED( hr = g_pMusicManager->CreateScriptFromFile( &g_pMusicScript, "farmmusic.spt" ) ) )
    {
        GetCurrentDirectory( MAX_PATH, strPath );
        lstrcat( strPath, TEXT("\\..\\res") );
        g_pMusicManager->SetSearchDirectory( strPath );

        if( FAILED( hr = g_pMusicManager->CreateScriptFromFile( &g_pMusicScript, "farmmusic.spt" ) ) )
            return DXTRACE_ERR( TEXT("CreateScriptFromFile"), hr );
    }

    LoadBitmapFromResource( IDB_BITMAP1, &g_hBitmap, &g_hPalette );

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: OnCallRoutine()
// Desc: 
//-----------------------------------------------------------------------------
HRESULT OnCallRoutine( HWND hDlg, TCHAR* strRoutine )
{
    HRESULT hr;

    if( FAILED( hr = g_pMusicScript->CallRoutine( strRoutine ) ) )
        return DXTRACE_ERR( TEXT("CallRoutine"), hr );

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: DrawButton()
// Desc: 
//-----------------------------------------------------------------------------
void DrawButton( int nDlgItem, HWND hDlgItem, LPDRAWITEMSTRUCT lpDrawItemStruct )  
{
    TCHAR strText[MAX_PATH];
    UINT uStyle = DFCS_BUTTONPUSH;
    COLORREF crBkColor;
    HBRUSH* phBrush = NULL;
    int nIndex = nDlgItem - IDC_BIRD;
    
    // If drawing selected, add the pushed style to DrawFrameControl.
    if (lpDrawItemStruct->itemState & ODS_SELECTED)
        uStyle |= DFCS_PUSHED;
    
    switch( nDlgItem )
    {
        case IDCANCEL:
        case IDC_ALLSTOP:
        {
            phBrush = &g_hRedBrush;
            crBkColor = RED_COLOR;
            break;
        }

        case IDC_BIRD:
        {
            if( g_bButtonPlaying[nIndex] )
            {
                phBrush = &g_hLtYellowBrush;
                crBkColor = LT_YELLOW_COLOR;
            }
            else
            {
                phBrush = &g_hYellowBrush;
                crBkColor = YELLOW_COLOR;
            }
            break;
        }

        case IDC_NIGHT:
        case IDC_PREDAWN:
        case IDC_DAWN:
        case IDC_END:
        {
            if( g_bButtonPlaying[nIndex] )
            {
                phBrush = &g_hLtGreenBrush;
                crBkColor = LT_GREEN_COLOR;
            }
            else
            {
                phBrush = &g_hGreenBrush;
                crBkColor = GREEN_COLOR;
            }
            break;
        }

        case IDC_COUGAR:
        case IDC_COW:
        case IDC_ROOSTER:
        case IDC_SHEEP:
        case IDC_WOLF:
        case IDC_ALARM:
        {
            if( g_bButtonPlaying[nIndex] )
            {
                phBrush = &g_hLtOrangeBrush;
                crBkColor = LT_ORANGE_COLOR;
            }
            else
            {
                phBrush = &g_hOrangeBrush;
                crBkColor = ORANGE_COLOR;
            }
            break;
        }
    }

    SetTextColor( lpDrawItemStruct->hDC, BLACK_COLOR );
    SetBkColor( lpDrawItemStruct->hDC, crBkColor );
    SelectObject( lpDrawItemStruct->hDC, *phBrush );

    // Draw the button frame.
    DrawFrameControl( lpDrawItemStruct->hDC, &lpDrawItemStruct->rcItem, 
        DFC_BUTTON, uStyle );

    // Fill center with new color
    RECT rc = lpDrawItemStruct->rcItem;
    rc.left     += 2;
    rc.top      += 2;
    rc.bottom   -= 2;
    rc.right    -= 2;
    FillRect( lpDrawItemStruct->hDC, &rc, *phBrush );

    // Draw text
    GetWindowText( hDlgItem, strText, MAX_PATH );
    DrawText( lpDrawItemStruct->hDC, strText, _tcslen(strText), 
              &lpDrawItemStruct->rcItem, DT_SINGLELINE|DT_VCENTER|DT_CENTER );
}




//-----------------------------------------------------------------------------
// Name: DrawStatic()
// Desc: 
//-----------------------------------------------------------------------------
void DrawStatic( int nDlgItem, HWND hDlgItem, LPDRAWITEMSTRUCT lpDrawItemStruct )  
{
    SetTextColor( lpDrawItemStruct->hDC, WHITE_COLOR );
    SetBkMode( lpDrawItemStruct->hDC, TRANSPARENT );

    // Draw text
    TCHAR strText[MAX_PATH];
    GetWindowText( hDlgItem, strText, MAX_PATH );
    DrawText( lpDrawItemStruct->hDC, strText, _tcslen(strText), 
              &lpDrawItemStruct->rcItem, DT_SINGLELINE|DT_VCENTER|DT_CENTER );

    SetBkMode( lpDrawItemStruct->hDC, OPAQUE );
}




//-----------------------------------------------------------------------------
// Name: PlayButton()
// Desc: 
//-----------------------------------------------------------------------------
void PlayButton( HWND hDlg, int nDlgItem )
{
    int nIndex = nDlgItem - IDC_BIRD;
    switch( nDlgItem )
    {       
        case IDC_ALLSTOP:
            g_pMusicScript->CallRoutine( TEXT("dmAllStop") );
            break;

        case IDC_BIRD:
            g_pMusicScript->CallRoutine( TEXT("dmSSBird") );
            break;

        case IDC_NIGHT:
            g_pMusicScript->CallRoutine( TEXT("dmBGNight") );
            break;

        case IDC_PREDAWN:
            g_pMusicScript->CallRoutine( TEXT("dmBGPredawn") );
            break;

        case IDC_DAWN:
            g_pMusicScript->CallRoutine( TEXT("dmBGDawn") );
            break;

        case IDC_END:
            g_pMusicScript->CallRoutine( TEXT("dmEnding") );
            break;

        case IDC_COUGAR:
            g_pMusicScript->CallRoutine( TEXT("dmSfxCougar") );
            break;

        case IDC_COW:
            g_pMusicScript->CallRoutine( TEXT("dmSfxCow") );
            break;

        case IDC_ROOSTER:
            g_pMusicScript->CallRoutine( TEXT("dmSfxRooster") );
            break;

        case IDC_SHEEP:
            g_pMusicScript->CallRoutine( TEXT("dmSfxSheep") );
            break;

        case IDC_WOLF:            
            g_pMusicScript->CallRoutine( TEXT("dmSfxWolf") );
            break;

        case IDC_ALARM:
            g_pMusicScript->CallRoutine( TEXT("dmSfxAlarm") );
            break;
    }

    InvalidateRect( GetDlgItem( hDlg, nDlgItem ), NULL, TRUE );
}




//-----------------------------------------------------------------------------
// Name: LoadBitmapFromResource()
// Desc: 
//-----------------------------------------------------------------------------
BOOL LoadBitmapFromResource( int nResourceID, HBITMAP *phBitmap, HPALETTE *phPalette )
{
    
    BITMAP  bm;
    
    *phBitmap = NULL;
    *phPalette = NULL;
    
    // Use LoadImage() to get the image loaded into a DIBSection
    *phBitmap = (HBITMAP)LoadImage( g_hInst, MAKEINTRESOURCE(nResourceID), IMAGE_BITMAP, 0, 0,
                                    LR_CREATEDIBSECTION | LR_DEFAULTSIZE );
    if( *phBitmap == NULL )
        return FALSE;
    
    // Get the color depth of the DIBSection
    GetObject(*phBitmap, sizeof(BITMAP), &bm );

    // If the DIBSection is 256 color or less, it has a color table
    if( ( bm.bmBitsPixel * bm.bmPlanes ) <= 8 )
    {
        HDC           hMemDC;
        HBITMAP       hOldBitmap;
        RGBQUAD       rgb[256];
        LPLOGPALETTE  pLogPal;
        WORD          i;
        
        // Create a memory DC and select the DIBSection into it
        hMemDC = CreateCompatibleDC( NULL );
        hOldBitmap = (HBITMAP)SelectObject( hMemDC, *phBitmap );
        // Get the DIBSection's color table
        GetDIBColorTable( hMemDC, 0, 256, rgb );
        // Create a palette from the color tabl
        pLogPal = (LOGPALETTE *)malloc( sizeof(LOGPALETTE) + (256*sizeof(PALETTEENTRY)) );
        pLogPal->palVersion = 0x300;
        pLogPal->palNumEntries = 256;
        for(i=0;i<256;i++)
        {
            pLogPal->palPalEntry[i].peRed = rgb[i].rgbRed;
            pLogPal->palPalEntry[i].peGreen = rgb[i].rgbGreen;
            pLogPal->palPalEntry[i].peBlue = rgb[i].rgbBlue;
            pLogPal->palPalEntry[i].peFlags = 0;
        }
        *phPalette = CreatePalette( pLogPal );
        // Clean up
        free( pLogPal );
        SelectObject( hMemDC, hOldBitmap );
        DeleteDC( hMemDC );
    }
    else   
    {
        // It has no color table, so use a halftone palette
        HDC    hRefDC;       
        hRefDC = GetDC( NULL );
        *phPalette = CreateHalftonePalette( hRefDC );
        ReleaseDC( NULL, hRefDC );
    }

    return TRUE;   
} 



//-----------------------------------------------------------------------------
// Name: CreateAudioPathFromFile()
// Desc: 
//-----------------------------------------------------------------------------
HRESULT CreateAudioPathFromFile( IDirectMusicAudioPath8** ppAudioPath, TCHAR* strFileName )
{
    HRESULT               hr;
    IDirectMusicSegment8* pSegment = NULL;
    IUnknown*             pUnknown = NULL;

    // DMusic only takes wide strings
    WCHAR wstrFileName[MAX_PATH];
    DXUtil_ConvertGenericStringToWide( wstrFileName, strFileName );

    IDirectMusicLoader8* pLoader = g_pMusicManager->GetLoader();

    if ( FAILED( hr = pLoader->LoadObjectFromFile( CLSID_DirectMusicAudioPathConfig,
                                                   IID_IUnknown,
                                                   wstrFileName,
                                                   (LPVOID*) &pUnknown ) ) )
    {
        if( hr == DMUS_E_LOADER_FAILEDOPEN )
            return hr;
        return DXTRACE_ERR( TEXT("LoadObjectFromFile"), hr );
    }

    IDirectMusicPerformance8* pPerformance = g_pMusicManager->GetPerformance();

    if( FAILED( hr = pPerformance->CreateAudioPath( pUnknown, TRUE, ppAudioPath ) ) )
        return DXTRACE_ERR( TEXT("CreateAudioPath"), hr );

    return S_OK;
}




