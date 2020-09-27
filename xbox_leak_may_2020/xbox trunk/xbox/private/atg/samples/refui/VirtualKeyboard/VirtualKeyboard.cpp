//-----------------------------------------------------------------------------
// File: VirtualKeyboard.cpp
//
// Desc: Virtual keyboard reference UI
//
// Hist: 02.13.01 - New for March XDK release 
//       03.07.01 - Localized for April XDK release
//       04.10.01 - Updated for May XDK with full translations
//       06.06.01 - Japanese keyboard added
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//
// This sample is intended to show appropriate functionality only. Please 
// do not lift the graphics for use in your game. A description of the user 
// research that went into the creation of this sample is located in the 
// XDK documentation at Developing for Xbox - Reference User Interface
//
//-----------------------------------------------------------------------------
#include "VirtualKeyboard.h"
#include <cassert>
#include <algorithm>

// The following header file is generated from "Resource.rdf" file using the
// Bundler tool. In addition to the header, the tool outputs a binary file
// (Resource.xpr) which contains compiled (i.e. bundled) resources and is
// loaded at runtime using the CXBPackedResource class.
#include "Resource.h"

// The following header file is generated from "ControllerS.rdf" file using the
// Bundler tool. In addition to the header, the tool outputs a binary file
// (ControllerS.xpr) which contains compiled (i.e. bundled) resources and is
// loaded at runtime using the CXBPackedResource class.
#include "ControllerS.h"




//-----------------------------------------------------------------------------
// Help text
//-----------------------------------------------------------------------------
struct HelpStrings
{
    WCHAR* strSelect;
    WCHAR* strCancel;
    WCHAR* strToggle;
    WCHAR* strHelp;
    WCHAR* strBackspace;
    WCHAR* strSpace;
    WCHAR* strTrigger;
};
HelpStrings *g_HelpStr;

const UINT XC_LANGUAGE_MAX = XC_LANGUAGE_ITALIAN + 1;

// Must match XC_LANGUAGE_* constants in xbox.h
// Accented chars must be specified in Unicode to build properly on 
// Far East versions of Windows
const WCHAR* g_strLanguage[] =
{
    L"",                        // Unknown (not used)
    L"English",                 // English
    JH_NI JH_HO JH_N JH_GO,     // Japanese (displayed if VKEY_JAPAN #defined)
    L"Deutsch",                 // German
    L"Fran" L"\xE7" L"ais",     // French
    L"Espa" L"\xF1" L"ol",      // Spanish
    L"Italiano",                // Italian
};

UINT CXBVirtualKeyboard::Key::iLang = XC_LANGUAGE_ENGLISH;




//-----------------------------------------------------------------------------
// Constants
//-----------------------------------------------------------------------------

// These are the widths of the keys on the display. GAP2 is the distance
// between the mode keys on the left and the rest of the keyboard. The safe
// title area is 512 pixels across, so these values must meet the following
// guideline: MODEKEY_WIDTH + GAP2_WIDTH + (10*KEY_WIDTH) + (9*GAP_WIDTH) <=512
const DWORD GAP_WIDTH     = 0;
const DWORD GAP2_WIDTH    = 4;
const DWORD MODEKEY_WIDTH = 166;
const DWORD KEY_INSET     = 1;
const FLOAT KEY_HEIGHT    = 42.0f;

const DWORD MAX_KEYS_PER_ROW = 14;

// Must be this far from center on 0.0 - 1.0 scale
const FLOAT JOY_THRESHOLD = 0.25f; 

// How often (per second) the caret blinks
const FLOAT fCARET_BLINK_RATE = 1.0f;

// During the blink period, the amount the caret is visible. 0.5 equals
// half the time, 0.75 equals 3/4ths of the time, etc.
const FLOAT fCARET_ON_RATIO = 0.75f;

// Text colors for keys
const D3DCOLOR COLOR_TEXT          = 0xFF00FF00;   // green
const D3DCOLOR COLOR_HIGHLIGHT     = 0xFF00FF00;   // green
const D3DCOLOR COLOR_PRESSED       = 0xFF808080;   // gray
const D3DCOLOR COLOR_NORMAL        = 0xFF000000;   // black
const D3DCOLOR COLOR_DISABLED      = 0xFFFFFFFF;   // white
const D3DCOLOR COLOR_HELPTEXT      = 0xFFFFFFFF;   // white
const D3DCOLOR COLOR_FONT_DISABLED = 0xFF808080;   // gray
const D3DCOLOR COLOR_INVISIBLE     = 0xFF0000FF;   // blue
const D3DCOLOR COLOR_RED           = 0xFFFF0000;   // red

// Controller repeat values
const FLOAT fINITIAL_REPEAT = 0.333f; // 333 mS recommended for first repeat
const FLOAT fSTD_REPEAT     = 0.085f; // 85 mS recommended for repeat rate

// Maximum number of characters in string
const DWORD MAX_CHARS = 64;

// Width of text box
const FLOAT fTEXTBOX_WIDTH = 576.0f - 64.0f - 4.0f - 4.0f - 10.0f;

const FLOAT BUTTON_Y = 400.0f;          // button text line
const FLOAT BUTTON_OFFSET = 40.0f;      // space between button and text
const D3DCOLOR BUTTON_COLOR = 0xFFFFFFFF;
const D3DCOLOR BUTTON_TEXT  = 0xFFFFFFFF;
const FLOAT FIXED_JSL_SIZE = 3.0f;

// Xboxdings font button mappings
const WCHAR BTN_A = L'A';
const WCHAR BTN_B = L'B';
const WCHAR BTN_X = L'C';
const WCHAR BTN_Y = L'D';




//-----------------------------------------------------------------------------
// Standard text
// Order of text matches XC_LANGUAGE_XXX #definitions in Xbox.h
//-----------------------------------------------------------------------------

const WCHAR* const strCHOOSE_KEYBOARD[7] = 
{
    L"",
    L"Choose Keyboard",
    JK_KI JK_DASH JK_BO JK_DASH JK_DO JH_WO JH_E JH_RA JH_N JH_DE JH_KU
    JH_DA JH_SA JH_I, // JP
    L"Tastatur ausw" L"\xE4" L"hlen", // DE
    L"Choisir le clavier", // FR
    L"Elegir teclado", // ES
    L"Scegli tastiera", // IT
};

const WCHAR* const strILLUSTRATIVE_GRAPHICS[7] = 
{
    L"",
    L"Sample graphics. Don't use in your game",
    JH_TO JH_KU JH_BE JH_TU JH_SI JH_YO JH_U JH_NO JK_KI JK_DASH JK_BO
    JK_DASH JK_DO, // JP
    L"Grafiken nur zur Illustration", // DE
    L"Exemples de graphiques uniquement", // FR
    L"Gr" L"\xE1" L"ficos s" L"\xF3" L"lo de muestra", // ES
    L"Grafica solo dimostrativa", // IT
};

const WCHAR* const strA_SELECT[ 7 ] = 
{
    L"",
    L"select",
    JH_KE JH_LTU JH_TE JH_I, // JP
    L"ausw" L"\xE4" L"hlen", // DE
    L"s" L"\xE9" L"lectionner", // FR               
    L"seleccionar", // ES
    L"seleziona", // IT
};

const WCHAR* const strB_BACK[ 7 ] = 
{
    L"",
    L"back",
    JH_MO JH_DO JH_RU, // JP
    L"zur" L"\xFC" L"ck", // DE
    L"retour", // FR
    L"atr" L"\xE1" L"s", // ES
    L"indietro", // IT
};

const WCHAR* const strY_HELP[7] =
{
    L"",
    L"help",
    JK_HE JK_RU JK_PU, // JP
    L"Hilfe", // DE
    L"aide",  // FR
    L"ayuda", // ES
    L"aiuto", // IT
};




//-----------------------------------------------------------------------------
// Name: main()
// Desc: Entry point to the program. Initializes everything, and goes into a
//       message-processing loop. Idle time is used to render the scene.
//-----------------------------------------------------------------------------
VOID __cdecl main()
{
    CXBVirtualKeyboard xbApp;

    if( FAILED( xbApp.Create() ) )
        return;

    xbApp.Run();
}




//-----------------------------------------------------------------------------
// Name: CXBVirtualKeyboard::Key()
// Desc: Constructor
//-----------------------------------------------------------------------------
CXBVirtualKeyboard::Key::Key( Xkey xk, DWORD w )
:
    xKey( xk ),
    dwWidth( w ),
    strName()
{
    if( iLang == XC_LANGUAGE_JAPANESE )
    {
        // No key names for Japanese
        strName = L"";
        return;
    }

    // Special keys get their own names
    switch( xKey )
    {
        case XK_SPACE:
            switch( iLang )
            {   
                default:
                case XC_LANGUAGE_ENGLISH: strName = L"Space";             break;
                case XC_LANGUAGE_GERMAN:  strName = L"LEERTASTE";         break;
                case XC_LANGUAGE_FRENCH:  strName = L"Espace";            break;
                case XC_LANGUAGE_SPANISH: strName = L"Espacio";           break;
                case XC_LANGUAGE_ITALIAN: strName = L"BARRA SPAZIATRICE"; break;
            }
            break;
        case XK_BACKSPACE:
            switch( iLang )
            {   
                default:
                case XC_LANGUAGE_ENGLISH: strName = L"Backspace";         break;
                case XC_LANGUAGE_GERMAN:  strName = L"R" L"\xDC" L"CKTASTE"; break;
                case XC_LANGUAGE_FRENCH:  strName = L"Ret. arr";          break;
                case XC_LANGUAGE_SPANISH: strName = L"Retroceso";         break;
                case XC_LANGUAGE_ITALIAN: strName = L"BACKSPACE";         break;
            }
            break;
        case XK_SHIFT:
            switch( iLang )
            {   
                default:
                case XC_LANGUAGE_ENGLISH: strName = L"Shift";             break;
                case XC_LANGUAGE_GERMAN:  strName = L"UMSCHALTTASTE";     break;
                case XC_LANGUAGE_FRENCH:  strName = L"Maj.";              break;
                case XC_LANGUAGE_SPANISH: strName = L"May" L"\xFA" L"s";  break;
                case XC_LANGUAGE_ITALIAN: strName = L"MAIUSC";            break;
            }
            break;
        case XK_CAPSLOCK:
            switch( iLang )
            {   
                default:
                case XC_LANGUAGE_ENGLISH: strName = L"Caps Lock";         break;
                case XC_LANGUAGE_GERMAN:  strName = L"FESTSTELLTASTE";    break;
                case XC_LANGUAGE_FRENCH:  strName = L"Verr. maj.";        break;
                case XC_LANGUAGE_SPANISH: strName = L"Bloq may" L"\xFA" L"s"; break;
                case XC_LANGUAGE_ITALIAN: strName = L"BLOC MAIUSC";       break;
            }
            break;
        case XK_ALPHABET:
            switch( iLang )
            {   
                default:
                case XC_LANGUAGE_ENGLISH: strName = L"Alphabet";        break;
                case XC_LANGUAGE_GERMAN:  strName = L"Alphabet";        break;
                case XC_LANGUAGE_FRENCH:  strName = L"Alphabet";        break;
                case XC_LANGUAGE_SPANISH: strName = L"Alfabeto";        break;
                case XC_LANGUAGE_ITALIAN: strName = L"Alfabeto";        break;
            }
            break;
        case XK_SYMBOLS:
            switch( iLang )
            {   
                default:
                case XC_LANGUAGE_ENGLISH: strName = L"Symbols";         break;
                case XC_LANGUAGE_GERMAN:  strName = L"Symbole";         break;
                case XC_LANGUAGE_FRENCH:  strName = L"Symbole";         break;
                case XC_LANGUAGE_SPANISH: strName = L"Simbolos";        break;
                case XC_LANGUAGE_ITALIAN: strName = L"Simboli";         break;
            }
            break;
        case XK_ACCENTS:
            switch( iLang )
            {   
                default:
                case XC_LANGUAGE_ENGLISH: strName = L"Accents";         break;
                case XC_LANGUAGE_GERMAN:  strName = L"Akzente";         break;
                case XC_LANGUAGE_FRENCH:  strName = L"Accents";         break;
                case XC_LANGUAGE_SPANISH: strName = L"Accentos";        break;
                case XC_LANGUAGE_ITALIAN: strName = L"Accenti";         break;
            }
            break;
        case XK_OK:
            switch( iLang )
            {   
                default:
                case XC_LANGUAGE_ENGLISH: strName = L"Done";            break;
                case XC_LANGUAGE_GERMAN:  strName = L"Fertig";          break;
                case XC_LANGUAGE_FRENCH:  strName = L"Termin" L"\xE9";  break;
                case XC_LANGUAGE_SPANISH: strName = L"Hecho";           break;
                case XC_LANGUAGE_ITALIAN: strName = L"Chiudi";          break;
            }
            break;
    }
}




//-----------------------------------------------------------------------------
// Name: CXBVirtualKeyboard()
// Desc: Constructor
//-----------------------------------------------------------------------------
CXBVirtualKeyboard::CXBVirtualKeyboard()
:
    CXBApplication   (),
    m_xprResource    (),
    m_xprControllerS (),
    m_FontLatin      (),
    m_FontJapan      (),
    m_FontCaps       (),
    m_FontArrows     (),
    m_FontEuro       (),
    m_FontBtn        (),
    m_bIsCapsLockOn  ( FALSE ),
    m_bIsShiftOn     ( FALSE ),
    m_State          ( STATE_STARTSCREEN ),
    m_iLanguage      ( XC_LANGUAGE_ENGLISH ),
    m_strData        (),
    m_iPos           ( 0 ),
    m_KeyboardList   (),
    m_iCurrBoard     ( TYPE_ALPHABET ),
    m_iCurrRow       ( 0 ),
    m_iCurrKey       ( 0 ),
    m_iLastColumn    ( 0 ),
    m_RepeatTimer    ( FALSE ),
    m_fRepeatDelay   ( fINITIAL_REPEAT ),
    m_CaretTimer     ( TRUE ),
    m_Help           (),
    m_ptKey          ( NULL ),
    m_ptControllerS  ( NULL ),
    m_ClickSnd       (),
    m_dwCurrCtlrState( 0 ),
    m_dwOldCtlrState ( 0 ),
    m_xNextKeyJpn    ( XK_NULL ),
    m_bTrig          ( FALSE ),
    m_bKana          ( FALSE )
{
}




//-----------------------------------------------------------------------------
// Name: Initialize()
// Desc: Sets up the virtual keyboard example
//-----------------------------------------------------------------------------
HRESULT CXBVirtualKeyboard::Initialize()
{
    // Create the resources
    if( FAILED( m_xprResource.Create( m_pd3dDevice, "Resource.xpr", 
                                      resource_NUM_RESOURCES ) ) )
        return XBAPPERR_MEDIANOTFOUND;

    // Help text for controller mappings
    g_HelpStr = new HelpStrings [ XC_LANGUAGE_MAX ];

    g_HelpStr[ XC_LANGUAGE_ENGLISH ].strSelect = L"Select";
    g_HelpStr[ XC_LANGUAGE_GERMAN  ].strSelect = L"Ausw" L"\xE4" L"hlen";
    g_HelpStr[ XC_LANGUAGE_FRENCH  ].strSelect = L"S" L"\xE9" L"lectionner";
    g_HelpStr[ XC_LANGUAGE_SPANISH ].strSelect = L"Seleccionar";
    g_HelpStr[ XC_LANGUAGE_ITALIAN ].strSelect = L"Seleziona";

    g_HelpStr[ XC_LANGUAGE_ENGLISH ].strCancel = L"Cancel";
    g_HelpStr[ XC_LANGUAGE_GERMAN  ].strCancel = L"Abbrechen";
    g_HelpStr[ XC_LANGUAGE_FRENCH  ].strCancel = L"Annuler";
    g_HelpStr[ XC_LANGUAGE_SPANISH ].strCancel = L"Cancelar";
    g_HelpStr[ XC_LANGUAGE_ITALIAN ].strCancel = L"Annulla";

    g_HelpStr[ XC_LANGUAGE_ENGLISH ].strToggle = L"Toggle\nmode";
    g_HelpStr[ XC_LANGUAGE_GERMAN  ].strToggle = L"Modus\nwechseln";
    g_HelpStr[ XC_LANGUAGE_FRENCH  ].strToggle = L"Basculer\nles modes";
    g_HelpStr[ XC_LANGUAGE_SPANISH ].strToggle = L"Alternar\nmodo";
    g_HelpStr[ XC_LANGUAGE_ITALIAN ].strToggle = L"Alterna\nmodalit" L"\xE0";

    g_HelpStr[ XC_LANGUAGE_ENGLISH ].strHelp = L"Display help";
    g_HelpStr[ XC_LANGUAGE_GERMAN  ].strHelp = L"Hilfe anzeigen";
    g_HelpStr[ XC_LANGUAGE_FRENCH  ].strHelp = L"Afficher l'aide";
    g_HelpStr[ XC_LANGUAGE_SPANISH ].strHelp = L"Mostrar ayuda";
    g_HelpStr[ XC_LANGUAGE_ITALIAN ].strHelp = L"Visualizza\nla Guida";

    g_HelpStr[ XC_LANGUAGE_ENGLISH ].strBackspace = L"Backspace";
    g_HelpStr[ XC_LANGUAGE_GERMAN  ].strBackspace = L"R" L"\xFC" L"cktaste";
    g_HelpStr[ XC_LANGUAGE_FRENCH  ].strBackspace = L"Retour arri" L"\xE8" L"re";
    g_HelpStr[ XC_LANGUAGE_SPANISH ].strBackspace = L"Retroceso";
    g_HelpStr[ XC_LANGUAGE_ITALIAN ].strBackspace = L"\nBackspace";

    g_HelpStr[ XC_LANGUAGE_ENGLISH ].strSpace = L"Space";
    g_HelpStr[ XC_LANGUAGE_GERMAN  ].strSpace = L"Leertaste";
    g_HelpStr[ XC_LANGUAGE_FRENCH  ].strSpace = L"Espace";
    g_HelpStr[ XC_LANGUAGE_SPANISH ].strSpace = L"Espacio";
    g_HelpStr[ XC_LANGUAGE_ITALIAN ].strSpace = L"Spazio";

    g_HelpStr[ XC_LANGUAGE_ENGLISH ].strTrigger = L"Trigger buttons move cursor";
    g_HelpStr[ XC_LANGUAGE_GERMAN  ].strTrigger = L"Schalter-Tasten bewegen den Cursor";
    g_HelpStr[ XC_LANGUAGE_FRENCH  ].strTrigger = L"Les g" L"\xE2" L"chettes d" L"\xE9" L"placent le curseur";
    g_HelpStr[ XC_LANGUAGE_SPANISH ].strTrigger = L"Los disparadores mueven el cursor";
    g_HelpStr[ XC_LANGUAGE_ITALIAN ].strTrigger = L"I grilletti permettono di spostare il cursore";

    // Set the matrices
    D3DXVECTOR3 vEye(-2.5f, 2.0f, -4.0f );
    D3DXVECTOR3 vAt( 0.0f, 0.0f, 0.0f );
    D3DXVECTOR3 vUp( 0.0f, 1.0f, 0.0f );

    D3DXMATRIX matWorld, matView, matProj;
    D3DXMatrixIdentity( &matWorld );
    D3DXMatrixLookAtLH( &matView, &vEye,&vAt, &vUp );
    D3DXMatrixPerspectiveFovLH( &matProj, D3DX_PI/4, 4.0f/3.0f, 1.0f, 100.0f );

    m_pd3dDevice->SetTransform( D3DTS_WORLD,      &matWorld );
    m_pd3dDevice->SetTransform( D3DTS_VIEW,       &matView );
    m_pd3dDevice->SetTransform( D3DTS_PROJECTION, &matProj );

    // Arial Unicode MS 18, regular, 32-376, for keys
    if( FAILED( m_FontLatin.Create( g_pd3dDevice, "Font18.xpr" ) ) )
        return XBAPPERR_MEDIANOTFOUND;

    // Arial 12, bold, 32-255, for capital words on keys
    if( FAILED( m_FontCaps.Create( g_pd3dDevice, "Font12.xpr" ) ) )
        return XBAPPERR_MEDIANOTFOUND;

    // Arial Unicode MS, 8882 - 8883
    if( FAILED( m_FontArrows.Create( g_pd3dDevice, "FontArrows.xpr" ) ) )
        return XBAPPERR_MEDIANOTFOUND;

    // Arial Unicode MS 12, regular, 8364
    if( FAILED( m_FontEuro.Create( g_pd3dDevice, "FontEuro.xpr" ) ) )
        return XBAPPERR_MEDIANOTFOUND;

    // Xbox dingbats (buttons) 24
    if( FAILED( m_FontBtn.Create( g_pd3dDevice, "Xboxdings_24.xpr" ) ) )
        return XBAPPERR_MEDIANOTFOUND;

#ifdef VKEY_JAPAN
    // Only need to load these resources if want to support Japanese
    // virtual keyboard

    // Japanese font
    if( FAILED( m_FontJapan.Create( g_pd3dDevice, "FontJapanese.xpr" ) ) )
        return XBAPPERR_MEDIANOTFOUND;

    // ControllerS gamepad
    if( FAILED( m_xprControllerS.Create( m_pd3dDevice, "ControllerS.xpr", 
                                         ControllerS_NUM_RESOURCES ) ) )
        return XBAPPERR_MEDIANOTFOUND;

    m_ptControllerS = m_xprControllerS.GetTexture( 
                                        ControllerS_ControllerSTexture_OFFSET );
#endif

    // Load the click sound
    if( FAILED( m_ClickSnd.Create( "Sounds\\Click.wav" ) ) )
        return XBAPPERR_MEDIANOTFOUND;

    // Create the help
    if( FAILED( m_Help.Create( m_pd3dDevice, "Gamepad.xpr" ) ) )
        return XBAPPERR_MEDIANOTFOUND;

    // Validate key sizes
    assert( MODEKEY_WIDTH + GAP2_WIDTH + (10 * KEY_WIDTH) + (9 * GAP_WIDTH) <= 512 );

    // Create the keyboard key texture
    m_ptKey = m_xprResource.GetTexture( resource_MsgBox_OFFSET );

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: FrameMove()
// Desc: Called once per frame; the entry point for animating the scene
//-----------------------------------------------------------------------------
HRESULT CXBVirtualKeyboard::FrameMove()
{
    ValidateState();

    // Poll the system for events
    Event ev = GetEvent();

    if( m_iLanguage == XC_LANGUAGE_JAPANESE && m_State == STATE_KEYBOARD )
    {
        if( ev == EV_BACK_BUTTON )
        {
            m_State = STATE_MENU;
        }
        else
        {
            // Must get additional information from the controller
            m_dwCurrCtlrState = GetEventJapan();
            UpdateStateJapan();
        }
    }
    else
    {
        // Normal state update
        UpdateState( ev );
    }
    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: Render()
// Desc: Called once per frame, the call is the entry point for 3d rendering.
//       This function sets up render states, clears the viewport, and renders
//       the scene.
//-----------------------------------------------------------------------------
HRESULT CXBVirtualKeyboard::Render()
{
    // Clear the viewport, zbuffer, and stencil buffer
    m_pd3dDevice->Clear( 0L, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER | 
                         D3DCLEAR_STENCIL, 0x000000FF, 1.0f, 0L );

    switch( m_State )
    {
        case STATE_STARTSCREEN: RenderStartScreen(); break;
        case STATE_MENU:        RenderMenu();        break;
        case STATE_KEYBOARD:    RenderKeyboard();    break;
        case STATE_HELP:        RenderHelp();        break;
        default:                assert( FALSE );     break;
    }
    
    // Present the scene
    m_pd3dDevice->Present( NULL, NULL, NULL, NULL );

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: ValidateState()
// Desc: Check object invariants
//-----------------------------------------------------------------------------
VOID CXBVirtualKeyboard::ValidateState() const
{
    assert( m_State >= 0 );
    assert( m_State < STATE_MAX );
    assert( m_iLanguage > 0 );

#ifndef VKEY_JAPAN
    // If we're not supporting Japanese, the language should never be Japanese
    assert( m_iLanguage != XC_LANGUAGE_JAPANESE );
#endif

    assert( m_iLanguage < XC_LANGUAGE_MAX );
    assert( m_iPos <= m_strData.length() );
    assert( m_iCurrBoard <= m_KeyboardList.size() );
    assert( m_iCurrRow < MAX_ROWS );
    if( !m_KeyboardList.empty() )
        assert( m_iCurrKey < m_KeyboardList[ m_iCurrBoard ][ m_iCurrRow ].size() );
}




//-----------------------------------------------------------------------------
// Name: InitBoard()
// Desc: Sets up the virtual keyboard for the selected language
//-----------------------------------------------------------------------------
VOID CXBVirtualKeyboard::InitBoard()
{
    // Restore keyboard to default state
    m_iCurrRow = 1;
    m_iCurrKey = 1;
    m_iLastColumn = 1;
    m_iCurrBoard = TYPE_ALPHABET;
    m_bIsCapsLockOn = FALSE;
    m_bIsShiftOn = FALSE;
    m_strData.erase();
    m_iPos = 0;

    // Notify the keys of the language
    Key::iLang = m_iLanguage;

    // Destroy old keyboard
    m_KeyboardList.clear();

    // Japanese doesn't use a Latin keyboard layout, so we can skip the
    // keyboard initialization
    if( m_iLanguage == XC_LANGUAGE_JAPANESE )
        return;

    //-------------------------------------------------------------------------
    // Alpha keyboard
    //-------------------------------------------------------------------------

    Keyboard keyBoard;
    keyBoard.reserve( MAX_ROWS );
    keyBoard.clear();

    KeyRow keyRow;
    keyRow.reserve( MAX_KEYS_PER_ROW );

    // First row is Done, 1-0
    keyRow.clear();
    keyRow.push_back( Key( XK_OK, MODEKEY_WIDTH ) );
    keyRow.push_back( Key( XK_1 ) );
    keyRow.push_back( Key( XK_2 ) );
    keyRow.push_back( Key( XK_3 ) );
    keyRow.push_back( Key( XK_4 ) );
    keyRow.push_back( Key( XK_5 ) );
    keyRow.push_back( Key( XK_6 ) );
    keyRow.push_back( Key( XK_7 ) );
    keyRow.push_back( Key( XK_8 ) );
    keyRow.push_back( Key( XK_9 ) );
    keyRow.push_back( Key( XK_0 ) );
    keyBoard.push_back( keyRow );

    // Second row is Shift, A-J
    keyRow.clear();
    keyRow.push_back( Key( XK_SHIFT, MODEKEY_WIDTH ) );
    keyRow.push_back( Key( XK_A ) );
    keyRow.push_back( Key( XK_B ) );
    keyRow.push_back( Key( XK_C ) );
    keyRow.push_back( Key( XK_D ) );
    keyRow.push_back( Key( XK_E ) );
    keyRow.push_back( Key( XK_F ) );
    keyRow.push_back( Key( XK_G ) );
    keyRow.push_back( Key( XK_H ) );
    keyRow.push_back( Key( XK_I ) );
    keyRow.push_back( Key( XK_J ) );
    keyBoard.push_back( keyRow );

    // Third row is Caps Lock, K-T
    keyRow.clear();
    keyRow.push_back( Key( XK_CAPSLOCK, MODEKEY_WIDTH ) );
    keyRow.push_back( Key( XK_K ) );
    keyRow.push_back( Key( XK_L ) );
    keyRow.push_back( Key( XK_M ) );
    keyRow.push_back( Key( XK_N ) );
    keyRow.push_back( Key( XK_O ) );
    keyRow.push_back( Key( XK_P ) );
    keyRow.push_back( Key( XK_Q ) );
    keyRow.push_back( Key( XK_R ) );
    keyRow.push_back( Key( XK_S ) );
    keyRow.push_back( Key( XK_T ) );
    keyBoard.push_back( keyRow );

    // Fourth row is Symbols, U-Z, Backspace
    keyRow.clear();
    keyRow.push_back( Key( XK_SYMBOLS, MODEKEY_WIDTH ) );
    keyRow.push_back( Key( XK_U ) );
    keyRow.push_back( Key( XK_V ) );
    keyRow.push_back( Key( XK_W ) );
    keyRow.push_back( Key( XK_X ) );
    keyRow.push_back( Key( XK_Y ) );
    keyRow.push_back( Key( XK_Z ) );
    keyRow.push_back( Key( XK_BACKSPACE, (KEY_WIDTH * 4) + (GAP_WIDTH * 3) ) );
    keyBoard.push_back( keyRow );

    // Fifth row is Accents, Space, Left, Right
    keyRow.clear();
    keyRow.push_back( Key( XK_ACCENTS, MODEKEY_WIDTH ) );
    keyRow.push_back( Key( XK_SPACE, (KEY_WIDTH * 6) + (GAP_WIDTH * 5) ) );
    keyRow.push_back( Key( XK_ARROWLEFT, (KEY_WIDTH * 2) + (GAP_WIDTH * 1) ) );
    keyRow.push_back( Key( XK_ARROWRIGHT, (KEY_WIDTH * 2) + (GAP_WIDTH * 1) ) );
    keyBoard.push_back( keyRow );

    // Add the alpha keyboard to the list
    m_KeyboardList.push_back( keyBoard );

    //-------------------------------------------------------------------------
    // Symbol keyboard
    //-------------------------------------------------------------------------

    keyBoard.clear();

    // First row
    keyRow.clear();
    keyRow.push_back( Key( XK_OK, MODEKEY_WIDTH ) );
    keyRow.push_back( Key( XK_LPAREN ) );
    keyRow.push_back( Key( XK_RPAREN ) );
    keyRow.push_back( Key( XK_AMPER ) );
    keyRow.push_back( Key( XK_UNDERS ) );
    keyRow.push_back( Key( XK_CARET ) );
    keyRow.push_back( Key( XK_PERCENT ) );
    keyRow.push_back( Key( XK_BSLASH ) );
    keyRow.push_back( Key( XK_FSLASH ) );
    keyRow.push_back( Key( XK_AT ) );
    keyRow.push_back( Key( XK_NSIGN ) );
    keyBoard.push_back( keyRow );

    // Second row
    keyRow.clear();
    keyRow.push_back( Key( XK_SHIFT, MODEKEY_WIDTH ) );
    keyRow.push_back( Key( XK_LBRACK ) );
    keyRow.push_back( Key( XK_RBRACK ) );
    keyRow.push_back( Key( XK_DOLLAR ) );
    keyRow.push_back( Key( XK_POUND_SIGN ) );
    keyRow.push_back( Key( XK_YEN_SIGN ) );
    keyRow.push_back( Key( XK_EURO_SIGN ) );
    keyRow.push_back( Key( XK_SEMI ) );
    keyRow.push_back( Key( XK_COLON ) );
    keyRow.push_back( Key( XK_QUOTE ) );
    keyRow.push_back( Key( XK_DQUOTE ) );
    keyBoard.push_back( keyRow );

    // Third row
    keyRow.clear();
    keyRow.push_back( Key( XK_CAPSLOCK, MODEKEY_WIDTH ) );
    keyRow.push_back( Key( XK_LT ) );
    keyRow.push_back( Key( XK_GT ) );
    keyRow.push_back( Key( XK_QMARK ) );
    keyRow.push_back( Key( XK_EXCL ) );
    keyRow.push_back( Key( XK_INVERTED_QMARK ) );
    keyRow.push_back( Key( XK_INVERTED_EXCL ) );
    keyRow.push_back( Key( XK_DASH ) );
    keyRow.push_back( Key( XK_STAR ) );
    keyRow.push_back( Key( XK_PLUS ) );
    keyRow.push_back( Key( XK_EQUAL ) );
    keyBoard.push_back( keyRow );

    // Fourth row
    keyRow.clear();
    keyRow.push_back( Key( XK_ALPHABET, MODEKEY_WIDTH ) );
    keyRow.push_back( Key( XK_LBRACE ) );
    keyRow.push_back( Key( XK_RBRACE ) );
    keyRow.push_back( Key( XK_LT_DBL_ANGLE_QUOTE ) );
    keyRow.push_back( Key( XK_RT_DBL_ANGLE_QUOTE ) );
    keyRow.push_back( Key( XK_COMMA ) );
    keyRow.push_back( Key( XK_PERIOD ) );
    keyRow.push_back( Key( XK_BACKSPACE, (KEY_WIDTH * 4) + (GAP_WIDTH * 3) ) );
    keyBoard.push_back( keyRow );

    // Fifth row is Accents, Space, Left, Right
    keyRow.clear();
    keyRow.push_back( Key( XK_ACCENTS, MODEKEY_WIDTH ) );
    keyRow.push_back( Key( XK_SPACE, (KEY_WIDTH * 6) + (GAP_WIDTH * 5) ) );
    keyRow.push_back( Key( XK_ARROWLEFT, (KEY_WIDTH * 2) + (GAP_WIDTH * 1) ) );
    keyRow.push_back( Key( XK_ARROWRIGHT, (KEY_WIDTH * 2) + (GAP_WIDTH * 1) ) );
    keyBoard.push_back( keyRow );

    // Add the symbol keyboard to the list
    m_KeyboardList.push_back( keyBoard );

    //-------------------------------------------------------------------------
    // Accents keyboard
    //-------------------------------------------------------------------------

    keyBoard.clear();

    // First row
    keyRow.clear();
    keyRow.push_back( Key( XK_OK, MODEKEY_WIDTH ) );
    keyRow.push_back( Key( XK_1 ) );
    keyRow.push_back( Key( XK_2 ) );
    keyRow.push_back( Key( XK_3 ) );
    keyRow.push_back( Key( XK_4 ) );
    keyRow.push_back( Key( XK_5 ) );
    keyRow.push_back( Key( XK_6 ) );
    keyRow.push_back( Key( XK_7 ) );
    keyRow.push_back( Key( XK_8 ) );
    keyRow.push_back( Key( XK_9 ) );
    keyRow.push_back( Key( XK_0 ) );
    keyBoard.push_back( keyRow );

    // Second row
    keyRow.clear();
    keyRow.push_back( Key( XK_SHIFT, MODEKEY_WIDTH ) );
    keyRow.push_back( Key( XK_CAP_A_GRAVE ) );
    keyRow.push_back( Key( XK_CAP_A_ACUTE ) );
    keyRow.push_back( Key( XK_CAP_A_CIRCUMFLEX ) );
    keyRow.push_back( Key( XK_CAP_A_DIAERESIS ) );
    keyRow.push_back( Key( XK_CAP_C_CEDILLA ) );
    keyRow.push_back( Key( XK_CAP_E_GRAVE ) );
    keyRow.push_back( Key( XK_CAP_E_ACUTE ) );
    keyRow.push_back( Key( XK_CAP_E_CIRCUMFLEX ) );
    keyRow.push_back( Key( XK_CAP_E_DIAERESIS ) );
    keyRow.push_back( Key( XK_CAP_I_GRAVE ) );
    keyBoard.push_back( keyRow );

    // Third row
    keyRow.clear();
    keyRow.push_back( Key( XK_CAPSLOCK, MODEKEY_WIDTH ) );
    keyRow.push_back( Key( XK_CAP_I_ACUTE ) );
    keyRow.push_back( Key( XK_CAP_I_CIRCUMFLEX ) );
    keyRow.push_back( Key( XK_CAP_I_DIAERESIS ) );
    keyRow.push_back( Key( XK_CAP_N_TILDE ) );
    keyRow.push_back( Key( XK_CAP_O_GRAVE ) );
    keyRow.push_back( Key( XK_CAP_O_ACUTE ) );
    keyRow.push_back( Key( XK_CAP_O_CIRCUMFLEX ) );
    keyRow.push_back( Key( XK_CAP_O_TILDE ) );
    keyRow.push_back( Key( XK_CAP_O_DIAERESIS ) );
    keyRow.push_back( Key( XK_SM_SHARP_S ) );
    keyBoard.push_back( keyRow );

    // Fourth row
    keyRow.clear();
    keyRow.push_back( Key( XK_ALPHABET, MODEKEY_WIDTH ) );
    keyRow.push_back( Key( XK_CAP_U_GRAVE ) );
    keyRow.push_back( Key( XK_CAP_U_ACUTE ) );
    keyRow.push_back( Key( XK_CAP_U_CIRCUMFLEX ) );
    keyRow.push_back( Key( XK_CAP_U_DIAERESIS ) );
    keyRow.push_back( Key( XK_CAP_Y_ACUTE ) );
    keyRow.push_back( Key( XK_CAP_Y_DIAERESIS ) );
    keyRow.push_back( Key( XK_BACKSPACE, (KEY_WIDTH * 4) + (GAP_WIDTH * 3) ) );
    keyBoard.push_back( keyRow );

    // Fifth row
    keyRow.clear();
    keyRow.push_back( Key( XK_ACCENTS, MODEKEY_WIDTH ) );
    keyRow.push_back( Key( XK_SPACE, (KEY_WIDTH * 6) + (GAP_WIDTH * 5) ) );
    keyRow.push_back( Key( XK_ARROWLEFT, (KEY_WIDTH * 2) + (GAP_WIDTH * 1) ) );
    keyRow.push_back( Key( XK_ARROWRIGHT, (KEY_WIDTH * 2) + (GAP_WIDTH * 1) ) );
    keyBoard.push_back( keyRow );

    // Add the accents keyboard to the list
    m_KeyboardList.push_back( keyBoard );

}




//-----------------------------------------------------------------------------
// Name: GetEvent()
// Desc: Polls the controller for events. Returns EV_NULL if no event
//-----------------------------------------------------------------------------
CXBVirtualKeyboard::Event CXBVirtualKeyboard::GetEvent()
{
    // Query the primary controller
    Event evControllerClick = GetControllerEvent();

    if( evControllerClick != EV_NULL && 
        m_iLanguage != XC_LANGUAGE_JAPANESE &&
        m_State == STATE_KEYBOARD )
    {
        PlayClick();
    }

    return evControllerClick;
}




//-----------------------------------------------------------------------------
// Name: IsAnyButtonActive()
// Desc: TRUE if any button depressed or any thumbstick offset on the given
//       controller.
//-----------------------------------------------------------------------------
BOOL IsAnyButtonActive( const XBGAMEPAD* pGamePad )
{
    // Check digital buttons
    if( pGamePad->wButtons )
        return TRUE;

    // Check analog buttons
    for( DWORD i = 0; i < 8; ++i )
    {
        if( pGamePad->bAnalogButtons[ i ] > XINPUT_GAMEPAD_MAX_CROSSTALK )
            return TRUE;
    }

    // Check thumbsticks
    if( pGamePad->fX1 >  JOY_THRESHOLD ||
        pGamePad->fX1 < -JOY_THRESHOLD ||
        pGamePad->fY1 >  JOY_THRESHOLD ||
        pGamePad->fY1 < -JOY_THRESHOLD )
    {
        return TRUE;
    }

    if( pGamePad->fX2 >  JOY_THRESHOLD ||
        pGamePad->fX2 < -JOY_THRESHOLD ||
        pGamePad->fY2 >  JOY_THRESHOLD ||
        pGamePad->fY2 < -JOY_THRESHOLD )
    {
        return TRUE;
    }

    // Nothing active
    return FALSE;
}




//-----------------------------------------------------------------------------
// Name: GetPrimaryController()
// Desc: The primary controller is the first controller used by a player.
//       If no controller has been used or the controller has been removed,
//       the primary controller is the controller inserted at the lowest 
//       port number. Function returns NULL if no controller is inserted.
//-----------------------------------------------------------------------------
const XBGAMEPAD* GetPrimaryController()
{
    static INT nPrimaryController = -1;

    // If primary controller has been set and hasn't been removed, use it
    const XBGAMEPAD* pGamePad = NULL;
    if( nPrimaryController != -1 )
    {
        pGamePad = &g_Gamepads[ nPrimaryController ];
        if( pGamePad->hDevice != NULL )
            return pGamePad;
    }

    // Primary controller hasn't been set or has been removed...

    // Examine each inserted controller to see if any is being used
    INT nFirst = -1;
    for( DWORD i=0; i < XGetPortCount(); ++i )
    {
        pGamePad = &g_Gamepads[i];
        if( pGamePad->hDevice != NULL )
        {
            // Remember the lowest inserted controller ID
            if( nFirst == -1 )
                nFirst = i;

            // If any button is active, we found the primary controller
            if( IsAnyButtonActive( pGamePad ) )
            {
                nPrimaryController = i;
                return pGamePad;
            }
        }
    }

    // No controllers are inserted
    if( nFirst == -1 )
        return NULL;

    // The primary controller hasn't been set and no controller has been
    // used yet, so return the controller on the lowest port number
    pGamePad = &g_Gamepads[ nFirst ];
    return pGamePad;
}




//-----------------------------------------------------------------------------
// Name: GetEventJapan()
// Desc: Returns the full ControllerS state packed into a DWORD.
//       Remembers the "old" state of the controller.
//-----------------------------------------------------------------------------
DWORD CXBVirtualKeyboard::GetEventJapan()
{
    assert( m_iLanguage == XC_LANGUAGE_JAPANESE );
    const XBGAMEPAD* pGamePad = GetPrimaryController();
    if( pGamePad == NULL )
        return 0;

    DWORD dwCurrCtlrState = 0;

    // Primary buttons
    if( pGamePad->wButtons & XINPUT_GAMEPAD_START )
    {
        if( !(m_dwOldCtlrState & XKJ_START) )
        {
            dwCurrCtlrState |= XKJ_START;
            m_dwOldCtlrState |= XKJ_START;
        }
    }
    else
        m_dwOldCtlrState &= ~XKJ_START;

    // "A"
    if( pGamePad->bAnalogButtons[ XINPUT_GAMEPAD_A ] > XINPUT_GAMEPAD_MAX_CROSSTALK )
    {
        if( !(m_dwOldCtlrState & XKJ_A) )
        {
            dwCurrCtlrState |= XKJ_A;
            m_dwOldCtlrState |= XKJ_A;
        }
    }
    else
        m_dwOldCtlrState &= ~XKJ_A;

    // "B"
    if( pGamePad->bAnalogButtons[ XINPUT_GAMEPAD_B ] > XINPUT_GAMEPAD_MAX_CROSSTALK )
    {
        if( !(m_dwOldCtlrState & XKJ_B) )
        {
            dwCurrCtlrState |= XKJ_B;
            m_dwOldCtlrState |= XKJ_B;
        }
    }
    else
        m_dwOldCtlrState &= ~XKJ_B;

    // "X"
    if( pGamePad->bAnalogButtons[ XINPUT_GAMEPAD_X ] > XINPUT_GAMEPAD_MAX_CROSSTALK )
    {
        if( !(m_dwOldCtlrState & XKJ_X) )
        {
            dwCurrCtlrState |= XKJ_X;
            m_dwOldCtlrState |= XKJ_X;
        }
    }
    else
        m_dwOldCtlrState &= ~XKJ_X;

    // "Y"
    if( pGamePad->bAnalogButtons[ XINPUT_GAMEPAD_Y ] > XINPUT_GAMEPAD_MAX_CROSSTALK )
    {
        if( !(m_dwOldCtlrState & XKJ_Y) )
        {
            dwCurrCtlrState |= XKJ_Y;
            m_dwOldCtlrState |= XKJ_Y;
        }
    }
    else
        m_dwOldCtlrState &= ~XKJ_Y;

    // black
    if( pGamePad->bAnalogButtons[ XINPUT_GAMEPAD_BLACK ] > XINPUT_GAMEPAD_MAX_CROSSTALK )
    {
        if( !(m_dwOldCtlrState & XKJ_BLACK) )
        {
            dwCurrCtlrState |= XKJ_BLACK;
            m_dwOldCtlrState |= XKJ_BLACK;
        }
    }
    else
        m_dwOldCtlrState &= ~XKJ_BLACK;

    // white
    if( pGamePad->bAnalogButtons[ XINPUT_GAMEPAD_WHITE ] > XINPUT_GAMEPAD_MAX_CROSSTALK )
    {
        if( !(m_dwOldCtlrState & XKJ_WHITE) )
        {
            dwCurrCtlrState |= XKJ_WHITE;
            m_dwOldCtlrState |= XKJ_WHITE;
        }
    }
    else
        m_dwOldCtlrState &= ~XKJ_WHITE;

    // left trigger
    if( pGamePad->bAnalogButtons[ XINPUT_GAMEPAD_LEFT_TRIGGER ] > XINPUT_GAMEPAD_MAX_CROSSTALK )
    {
        if( !(m_dwOldCtlrState & XKJ_LEFTTR) )
        {
            dwCurrCtlrState |= XKJ_LEFTTR;
            m_dwOldCtlrState |= XKJ_LEFTTR;
        }
    }
    else
        m_dwOldCtlrState &= ~XKJ_LEFTTR;

    // right trigger
    if( pGamePad->bAnalogButtons[ XINPUT_GAMEPAD_RIGHT_TRIGGER ] > XINPUT_GAMEPAD_MAX_CROSSTALK )
    {
        if( !(m_dwOldCtlrState & XKJ_RIGHTTR) )
        {
            dwCurrCtlrState |= XKJ_RIGHTTR;
            m_dwOldCtlrState |= XKJ_RIGHTTR;
        }
    }
    else
        m_dwOldCtlrState &= ~XKJ_RIGHTTR;

    // Cursor Movement
    if( pGamePad->wButtons & XINPUT_GAMEPAD_DPAD_LEFT )
    {
        if( !(m_dwOldCtlrState & XKJ_DLEFT) )
        {
            dwCurrCtlrState |= XKJ_DLEFT;
            m_dwOldCtlrState |= XKJ_DLEFT;
        }
    }
    else
        m_dwOldCtlrState &= ~XKJ_DLEFT;

    if( pGamePad->wButtons & XINPUT_GAMEPAD_DPAD_RIGHT )
    {
        if( !(m_dwOldCtlrState & XKJ_DRIGHT) )
        {
            dwCurrCtlrState |= XKJ_DRIGHT;
            m_dwOldCtlrState |= XKJ_DRIGHT;
        }
    }
    else
        m_dwOldCtlrState &= ~XKJ_DRIGHT;

    if( pGamePad->fY1 > JOY_THRESHOLD )
        dwCurrCtlrState |= XKJ_UP;
    if( pGamePad->fY1 < -JOY_THRESHOLD )
        dwCurrCtlrState |= XKJ_DOWN;
    if( pGamePad->fX1 < -JOY_THRESHOLD )
        dwCurrCtlrState |= XKJ_LEFT;
    if( pGamePad->fX1 > JOY_THRESHOLD )
        dwCurrCtlrState |= XKJ_RIGHT;

    return dwCurrCtlrState;
}




//-----------------------------------------------------------------------------
// Name: GetControllerEvent()
// Desc: Polls the controller for events. Handles button repeats.
//-----------------------------------------------------------------------------
CXBVirtualKeyboard::Event CXBVirtualKeyboard::GetControllerEvent()
{
    const XBGAMEPAD* pGamePad = GetPrimaryController();
    if( pGamePad != NULL )
    {
        // Handle button press and joystick hold repeats
        BOOL bRepeat = FALSE;
        if( IsAnyButtonActive( pGamePad ) )
        {
            // If the timer is running, the button is being held. If it's
            // held long enough, it triggers a repeat. If the timer isn't
            // running, we start it.
            if( m_RepeatTimer.IsRunning() )
            {
                // If the timer is running but hasn't expired, bail out
                if( m_RepeatTimer.GetElapsedSeconds() < m_fRepeatDelay )
                    return EV_NULL;

                m_fRepeatDelay = fSTD_REPEAT;
                m_RepeatTimer.StartZero();
                bRepeat = TRUE;
            }
            else
            {
                m_fRepeatDelay = fINITIAL_REPEAT;
                m_RepeatTimer.StartZero();
            }
        }
        else
        {
            // No buttons or joysticks active; kill the repeat timer
            m_fRepeatDelay = fINITIAL_REPEAT;
            m_RepeatTimer.Stop();
        }

        // Only allow repeat for navigation or left/right arrows
        if( bRepeat )
        {
            if( pGamePad->bAnalogButtons[ XINPUT_GAMEPAD_A ] > XINPUT_GAMEPAD_MAX_CROSSTALK )
            {
                if( !m_KeyboardList.empty() )
                {
                    Key key = m_KeyboardList[ m_iCurrBoard ][ m_iCurrRow ][ m_iCurrKey ];
                    if( key.xKey == XK_ARROWLEFT ||
                        key.xKey == XK_ARROWRIGHT )
                    {
                        return EV_A_BUTTON;
                    }
                }
            }
        }
        else
        {
            // Primary buttons
            if( pGamePad->wButtons & XINPUT_GAMEPAD_START )
                return EV_START_BUTTON;
            if( pGamePad->wButtons & XINPUT_GAMEPAD_BACK )
                return EV_BACK_BUTTON;
            if( pGamePad->bAnalogButtons[ XINPUT_GAMEPAD_A ] > XINPUT_GAMEPAD_MAX_CROSSTALK )
                return EV_A_BUTTON;
            if( pGamePad->bAnalogButtons[ XINPUT_GAMEPAD_B ] > XINPUT_GAMEPAD_MAX_CROSSTALK )
                return EV_B_BUTTON;
            if( pGamePad->bAnalogButtons[ XINPUT_GAMEPAD_X ] > XINPUT_GAMEPAD_MAX_CROSSTALK )
                return EV_X_BUTTON;
            if( pGamePad->bAnalogButtons[ XINPUT_GAMEPAD_Y ] > XINPUT_GAMEPAD_MAX_CROSSTALK )
                return EV_Y_BUTTON;
            if( pGamePad->bAnalogButtons[ XINPUT_GAMEPAD_BLACK ] > XINPUT_GAMEPAD_MAX_CROSSTALK )
                return EV_BLACK_BUTTON;
            if( pGamePad->bAnalogButtons[ XINPUT_GAMEPAD_WHITE ] > XINPUT_GAMEPAD_MAX_CROSSTALK )
                return EV_WHITE_BUTTON;
        }

        // Cursor movement
        if( pGamePad->bAnalogButtons[ XINPUT_GAMEPAD_LEFT_TRIGGER ] > XINPUT_GAMEPAD_MAX_CROSSTALK )
            return EV_LEFT_BUTTON;
        if( pGamePad->bAnalogButtons[ XINPUT_GAMEPAD_RIGHT_TRIGGER ] > XINPUT_GAMEPAD_MAX_CROSSTALK )
            return EV_RIGHT_BUTTON;

        // Movement
        if( pGamePad->wButtons & XINPUT_GAMEPAD_DPAD_UP ||
            pGamePad->fY1 > JOY_THRESHOLD )
            return EV_UP;
        if( pGamePad->wButtons & XINPUT_GAMEPAD_DPAD_DOWN ||
            pGamePad->fY1 < -JOY_THRESHOLD )
            return EV_DOWN;
        if( pGamePad->wButtons & XINPUT_GAMEPAD_DPAD_LEFT ||
            pGamePad->fX1 < -JOY_THRESHOLD )
            return EV_LEFT;
        if( pGamePad->wButtons & XINPUT_GAMEPAD_DPAD_RIGHT ||
            pGamePad->fX1 > JOY_THRESHOLD )
            return EV_RIGHT;
    }

    // No controllers inserted or no button presses or no repeat
    return EV_NULL;
}




//-----------------------------------------------------------------------------
// Name: UpdateState()
// Desc: State machine updates the current context based on the incoming event
//-----------------------------------------------------------------------------
VOID CXBVirtualKeyboard::UpdateState( Event ev )
{
    switch( m_State )
    {
        case STATE_STARTSCREEN:
            switch( ev )
            {
                case EV_A_BUTTON:           // Select current key
                case EV_START_BUTTON:
                    m_State = STATE_MENU;
                    break;
            }
            break;
        case STATE_MENU:
            switch( ev )
            {
                case EV_A_BUTTON:           // Select current key
                case EV_START_BUTTON:
                    InitBoard();
                    m_State = STATE_KEYBOARD;
                    break;

                // Navigation
                case EV_UP:
                    if( m_iLanguage == XC_LANGUAGE_ENGLISH )
                        m_iLanguage = XC_LANGUAGE_MAX - 1;
                    else
                    {
                        --m_iLanguage;
#ifndef VKEY_JAPAN
                        // If we're not supporting the Japanese keyboard,
                        // it's not in the list
                        if( m_iLanguage == XC_LANGUAGE_JAPANESE )
                            --m_iLanguage;
#endif
                    }
                    break;
                case EV_DOWN:
                    if( m_iLanguage == XC_LANGUAGE_MAX - 1 )
                        m_iLanguage = XC_LANGUAGE_ENGLISH;
                    else
                    {
                        ++m_iLanguage;
#ifndef VKEY_JAPAN
                        // If we're not supporting the Japanese keyboard,
                        // it's not in the list
                        if( m_iLanguage == XC_LANGUAGE_JAPANESE )
                            ++m_iLanguage;
#endif
                    }
                    break;
            }
            break;
        case STATE_KEYBOARD:
            switch( ev )
            {
                case EV_A_BUTTON:           // Select current key
                case EV_START_BUTTON:
                    PressCurrent();
                    break;
                case EV_B_BUTTON:           // Shift mode
                case EV_BACK_BUTTON:        // Back
                    m_State = STATE_MENU;
                    break;
                case EV_X_BUTTON:           // Toggle keyboard
                    if( m_iLanguage == XC_LANGUAGE_ENGLISH )
                    {
                        Press( m_iCurrBoard == TYPE_SYMBOLS ?
                               XK_ALPHABET : XK_SYMBOLS );
                    }
                    else
                    {
                        switch( m_iCurrBoard )
                        {
                            case TYPE_ALPHABET: Press( XK_SYMBOLS  ); break;
                            case TYPE_SYMBOLS:  Press( XK_ACCENTS  ); break;
                            case TYPE_ACCENTS:  Press( XK_ALPHABET ); break;
                        }
                    }
                    break;
                case EV_Y_BUTTON:           // Show help
                    m_State = STATE_HELP;
                    break;
                case EV_WHITE_BUTTON:       // Backspace
                    Press( XK_BACKSPACE );
                    break;
                case EV_BLACK_BUTTON:       // Space
                    Press( XK_SPACE );
                    break;
                case EV_LEFT_BUTTON:        // Left
                    Press( XK_ARROWLEFT );
                    break;
                case EV_RIGHT_BUTTON:       // Right
                    Press( XK_ARROWRIGHT );
                    break;

                // Navigation
                case EV_UP:     MoveUp();    break;
                case EV_DOWN:   MoveDown();  break;
                case EV_LEFT:   MoveLeft();  break;
                case EV_RIGHT:  MoveRight(); break;
            }
            break;
        case STATE_HELP:
            // Any key returns to keyboard
            if( ev != EV_NULL )
                m_State = STATE_KEYBOARD;
            break;
        default:
            assert( FALSE );
            break;
    }
}




//-----------------------------------------------------------------------------
// Name: UpdateStateJapan()
// Desc: 日本語入力の子音、及びひらがな／カタカナ切替をここでする。
//       また押されたボタンによってはクリック音も出す。
//-----------------------------------------------------------------------------
VOID CXBVirtualKeyboard::UpdateStateJapan()
{
    assert( m_iLanguage == XC_LANGUAGE_JAPANESE );

    // See if right trigger is held
    if( m_dwCurrCtlrState & XKJ_RIGHTTR )
        m_bTrig ^= TRUE;

    // See if left trigger is held
    if( m_dwCurrCtlrState & XKJ_LEFTTR )
        m_bKana ^= TRUE;

    // Determine if we should play the click sound. The following
    // buttons give a click: A, B, X, Y, black, white, Dpad left/right
    const DWORD dwButtons = XKJ_A | XKJ_B | XKJ_X | XKJ_Y | XKJ_BLACK | 
                            XKJ_WHITE | XKJ_DLEFT | XKJ_DRIGHT;
    if( m_dwCurrCtlrState & dwButtons )
        PlayClick();

    // "Key press"
    if( m_xNextKeyJpn != NULL )
        Press( m_xNextKeyJpn );
}




//-----------------------------------------------------------------------------
// Name: PressCurrent()
// Desc: Press the current key on the keyboard
//-----------------------------------------------------------------------------
VOID CXBVirtualKeyboard::PressCurrent()
{
    // Determine the current key
    Key key = m_KeyboardList[ m_iCurrBoard ][ m_iCurrRow ][ m_iCurrKey ];

    // Press it
    Press( key.xKey );
}




//-----------------------------------------------------------------------------
// Name: Press()
// Desc: Press the given key on the keyboard
//-----------------------------------------------------------------------------
VOID CXBVirtualKeyboard::Press( Xkey xk )
{
    // If the key represents a character, add it to the word
    if( xk < 0x10000 && xk != XK_ARROWLEFT && xk != XK_ARROWRIGHT )
    {
        // Don't add more than the maximum characters, and don't allow 
        // text to exceed the width of the text entry field
        if( m_strData.length() < MAX_CHARS )
        {
            CXBFont& Font = ( m_iLanguage == XC_LANGUAGE_JAPANESE ) ? 
                              m_FontJapan : m_FontLatin;

            // Get the width of the string in pixels
            FLOAT fHeight;
            FLOAT fWidth;
            Font.GetTextExtent( m_strData.c_str(), &fWidth, &fHeight );

            // Account for any Euro symbols
            FLOAT fEuroWidth;
            WCHAR strEuro[] = { XK_EURO_SIGN, 0 };
            m_FontEuro.GetTextExtent( strEuro, &fEuroWidth, &fHeight );

            fWidth += std::count( m_strData.begin(), m_strData.end(), 
                                  XK_EURO_SIGN ) * fEuroWidth;

            if( fWidth < fTEXTBOX_WIDTH )
            {
                m_strData.insert( m_iPos, 1, GetChar( xk ) );
                ++m_iPos; // move the caret
            }
        }

        // Unstick the shift key
        m_bIsShiftOn = FALSE;
    }

    // Special cases
    else switch( xk )
    {
        case XK_BACKSPACE:
            if( m_iPos > 0 )
            {
                --m_iPos; // move the caret
                m_strData.erase( m_iPos, 1 );
            }
            break;
        case XK_DELETE: // Used for Japanese only
            if( m_strData.length() > 0 )
                m_strData.erase( m_iPos, 1 );
            break;
        case XK_SHIFT:
            m_bIsShiftOn = !m_bIsShiftOn;
            break;
        case XK_CAPSLOCK:
            m_bIsCapsLockOn = !m_bIsCapsLockOn;
            break;
        case XK_ALPHABET:
            m_iCurrBoard = TYPE_ALPHABET;

            // Adjust mode keys
            m_KeyboardList[m_iCurrBoard][3][0] = Key( XK_SYMBOLS, MODEKEY_WIDTH );
            m_KeyboardList[m_iCurrBoard][4][0] = Key( XK_ACCENTS, MODEKEY_WIDTH );

            break;
        case XK_SYMBOLS:
            m_iCurrBoard = TYPE_SYMBOLS;

            // Adjust mode keys
            m_KeyboardList[m_iCurrBoard][3][0] = Key( XK_ALPHABET, MODEKEY_WIDTH );
            m_KeyboardList[m_iCurrBoard][4][0] = Key( XK_ACCENTS, MODEKEY_WIDTH );

            break;
        case XK_ACCENTS:
            m_iCurrBoard = TYPE_ACCENTS;

            // Adjust mode keys
            m_KeyboardList[m_iCurrBoard][3][0] = Key( XK_ALPHABET, MODEKEY_WIDTH );
            m_KeyboardList[m_iCurrBoard][4][0] = Key( XK_SYMBOLS, MODEKEY_WIDTH );

            break;
        case XK_ARROWLEFT:
            if( m_iPos > 0 )
                --m_iPos;
            break;
        case XK_ARROWRIGHT:
            if( m_iPos < m_strData.length() )
                ++m_iPos;
            break;
        case XK_OK:
            m_iPos = 0;
            m_strData.erase();
            break;
    }
}




//-----------------------------------------------------------------------------
// Name: MoveUp()
// Desc: Move the cursor up
//-----------------------------------------------------------------------------
VOID CXBVirtualKeyboard::MoveUp()
{
    do
    {
        // Update key index for special cases
        switch( m_iCurrRow )
        {
            case 0:
                if( 1 < m_iCurrKey && m_iCurrKey < 7 )      // 2 - 6
                {
                    m_iLastColumn = m_iCurrKey;             // remember column
                    m_iCurrKey = 1;                         // move to spacebar
                }
                else if( 6 < m_iCurrKey && m_iCurrKey < 9 ) // 7 - 8
                {
                    m_iLastColumn = m_iCurrKey;             // remember column
                    m_iCurrKey = 2;                         // move to left arrow
                }
                else if( m_iCurrKey > 8 )                   // 9 - 0
                {
                    m_iLastColumn = m_iCurrKey;             // remember column
                    m_iCurrKey = 3;                         // move to right arrow
                }
                break;
            case 3:
                if( m_iCurrKey == 7 )                       // backspace
                    m_iCurrKey = max( 7, m_iLastColumn );   // restore column
                break;
            case 4:
                if( m_iCurrKey == 1 )                       // spacebar
                    m_iCurrKey = min( 6, m_iLastColumn );   // restore column
                else if( m_iCurrKey > 1 )                   // left and right
                    m_iCurrKey = 7;                         // backspace
                break;
        }

        // Update row
        m_iCurrRow = ( m_iCurrRow == 0 ) ? MAX_ROWS - 1 : m_iCurrRow - 1;

    } while( IsKeyDisabled() );
}




//-----------------------------------------------------------------------------
// Name: MoveDown()
// Desc: Move the cursor down
//-----------------------------------------------------------------------------
VOID CXBVirtualKeyboard::MoveDown()
{
    do
    {
        // Update key index for special cases
        switch( m_iCurrRow )
        {
            case 2:
                if( m_iCurrKey > 7 )                    // q - t
                {
                    m_iLastColumn = m_iCurrKey;         // remember column
                    m_iCurrKey = 7;                     // move to backspace
                }
                break;
            case 3:
                if( 0 < m_iCurrKey && m_iCurrKey < 7 )  // u - z
                {
                    m_iLastColumn = m_iCurrKey;         // remember column
                    m_iCurrKey = 1;                     // move to spacebar
                }
                else if( m_iCurrKey > 6 )               // backspace
                {
                    if( m_iLastColumn > 8 )
                        m_iCurrKey = 3;                 // move to right arrow
                    else
                        m_iCurrKey = 2;                 // move to left arrow
                }
                break;
            case 4:
                switch( m_iCurrKey )
                {
                    case 1:                             // spacebar
                        m_iCurrKey = min( 6, m_iLastColumn );
                        break;
                    case 2:                             // left arrow
                        m_iCurrKey = max( min( 8, m_iLastColumn ), 7 );
                        break;
                    case 3:                             // right arrow
                        m_iCurrKey = max( 9, m_iLastColumn );
                        break;
                }
                break;
        }

        // Update row
        m_iCurrRow = ( m_iCurrRow == MAX_ROWS - 1 ) ? 0 : m_iCurrRow + 1;

    } while( IsKeyDisabled() );
}




//-----------------------------------------------------------------------------
// Name: MoveLeft()
// Desc: Move the cursor left
//-----------------------------------------------------------------------------
VOID CXBVirtualKeyboard::MoveLeft()
{
    do
    {
        if( m_iCurrKey == 0 )
            m_iCurrKey = m_KeyboardList[ m_iCurrBoard ][ m_iCurrRow ].size() - 1;
        else
            --m_iCurrKey;

    } while( IsKeyDisabled() );

    SetLastColumn();
}




//-----------------------------------------------------------------------------
// Name: MoveRight()
// Desc: Move the cursor right
//-----------------------------------------------------------------------------
VOID CXBVirtualKeyboard::MoveRight()
{
    do
    {
        if( m_iCurrKey == m_KeyboardList[ m_iCurrBoard ][ m_iCurrRow ].size() - 1 )
            m_iCurrKey = 0;
        else
            ++m_iCurrKey;

    } while( IsKeyDisabled() );

    SetLastColumn();
}




//-----------------------------------------------------------------------------
// Name: SetLastColumn()
// Desc: Remember the column position if we're on a single letter character
//-----------------------------------------------------------------------------
VOID CXBVirtualKeyboard::SetLastColumn()
{
    // If the new key is a single character, remember it for later
    Key key = m_KeyboardList[ m_iCurrBoard ][ m_iCurrRow ][ m_iCurrKey ];
    if( key.strName.empty() )
    {
        switch( key.xKey )
        {
            // Adjust the last column for the arrow keys to confine it
            // within the range of the key width
            case XK_ARROWLEFT:
                m_iLastColumn = ( m_iLastColumn <= 7 ) ? 7 : 8; break;
            case XK_ARROWRIGHT:
                m_iLastColumn = ( m_iLastColumn <= 9 ) ? 9 : 10; break;

            // Single char, non-arrow
            default:
                m_iLastColumn = m_iCurrKey; break;
        }
    }
}




//-----------------------------------------------------------------------------
// Name: RenderKey()
// Desc: Render the key at the given position
//-----------------------------------------------------------------------------
VOID CXBVirtualKeyboard::RenderKey( FLOAT fX, FLOAT fY, const Key& key, 
                                    D3DCOLOR selKeyColor, 
                                    D3DCOLOR selTextColor ) const
{
    if( selKeyColor == COLOR_INVISIBLE )
        return;

    struct KEYVERTEX
    {
        D3DXVECTOR4 p;
        D3DXVECTOR2 t;
    };

    WCHAR strKey[2] = { GetChar( key.xKey ), 0 };
    const WCHAR* strName = key.strName.empty() ? strKey : key.strName.c_str();

    FLOAT x = fX + KEY_INSET;
    FLOAT y = fY + KEY_INSET;
    FLOAT z = fX + key.dwWidth - KEY_INSET;
    FLOAT w = fY + KEY_HEIGHT - KEY_INSET;

    LPDIRECT3DVERTEXBUFFER8 pVertexBuf;
    m_pd3dDevice->CreateVertexBuffer( 4 * sizeof( KEYVERTEX ), D3DUSAGE_WRITEONLY, 
                                      D3DFVF_XYZRHW | D3DFVF_TEX1, D3DPOOL_MANAGED, 
                                      &pVertexBuf );
    KEYVERTEX* pVertices;
    pVertexBuf->Lock( 0, 0, (BYTE **)&pVertices, 0L );
    pVertices[0].p = D3DXVECTOR4( x-0.5f, y-0.5f, 1.0f, 1.0f );  pVertices[0].t = D3DXVECTOR2( 0.0f, 0.0f );
    pVertices[1].p = D3DXVECTOR4( z-0.5f, y-0.5f, 1.0f, 1.0f );  pVertices[1].t = D3DXVECTOR2( 1.0f, 0.0f );
    pVertices[2].p = D3DXVECTOR4( x-0.5f, w-0.5f, 1.0f, 1.0f );  pVertices[2].t = D3DXVECTOR2( 0.0f, 1.0f );
    pVertices[3].p = D3DXVECTOR4( z-0.5f, w-0.5f, 1.0f, 1.0f );  pVertices[3].t = D3DXVECTOR2( 1.0f, 1.0f );
    pVertexBuf->Unlock();

    LPDIRECT3DINDEXBUFFER8 pIndexBuf;
    m_pd3dDevice->CreateIndexBuffer( 5 * 2, D3DUSAGE_WRITEONLY, D3DFMT_INDEX16, 
                                     D3DPOOL_MANAGED, &pIndexBuf );
    SHORT* pIndices;
    pIndexBuf->Lock( 0, 0, (BYTE **)&pIndices, 0L );
    pIndices[0] = 0;
    pIndices[1] = 1;
    pIndices[2] = 3;
    pIndices[3] = 2;
    pIndices[4] = 0;
    pIndexBuf->Unlock();

    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP, D3DTOP_SELECTARG1 );
    m_pd3dDevice->SetVertexShader( D3DFVF_XYZRHW | D3DFVF_TEX1 );
    m_pd3dDevice->SetStreamSource( 0, pVertexBuf, sizeof( KEYVERTEX ) );

    // Draw the key background
    m_pd3dDevice->SetTexture( 0, m_ptKey );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
    m_pd3dDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP, 0, 2 );
    m_pd3dDevice->SetTexture( 0, NULL );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TFACTOR );

    if( selKeyColor )
    {
        m_pd3dDevice->SetRenderState( D3DRS_TEXTUREFACTOR, selKeyColor );
        m_pd3dDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP, 0, 2 );
    }
    
    m_pd3dDevice->SetRenderState( D3DRS_TEXTUREFACTOR, 0xFF000000 );
    m_pd3dDevice->SetIndices( pIndexBuf, 0 );
    m_pd3dDevice->DrawIndexedPrimitive( D3DPT_LINESTRIP, 0, 5, 0, 4 );

    pVertexBuf->Release();
    pIndexBuf->Release();

    // Determine font. Arrow keys and euro sign have special fonts because
    // their Unicode values are unusually large. To save texture space,
    // those fonts just include the arrow keys/euro sign. If key name is
    // all uppercase, we use a slightly smaller font.
    CXBFont* pFont;
    if( key.xKey == XK_ARROWLEFT || key.xKey == XK_ARROWRIGHT )
        pFont = &m_FontArrows;
    else if( key.xKey == XK_EURO_SIGN )
        pFont = &m_FontEuro;
    else if( key.strName.length() > 1 && iswupper( key.strName[1] ) )
        pFont = &m_FontCaps;
    else
        pFont = &m_FontLatin;

    // Draw the key text
    pFont->DrawText( ( x + z ) / 2.0f, ( y + w ) / 2.0f , selTextColor, strName, 
                     XBFONT_CENTER_X | XBFONT_CENTER_Y );
}




//-----------------------------------------------------------------------------
// Name: DrawTextBox()
// Desc: Display box containing text input
//-----------------------------------------------------------------------------
VOID CXBVirtualKeyboard::DrawTextBox() const
{
    D3DXVECTOR4 avRect[5];
    m_pd3dDevice->SetRenderState( D3DRS_TEXTUREFACTOR, 0xFF000000 );
    m_pd3dDevice->SetTexture( 0, NULL );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP, D3DTOP_SELECTARG1 );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TFACTOR );
    m_pd3dDevice->SetVertexShader( D3DFVF_XYZRHW );

    avRect[0] = D3DXVECTOR4(  64 - 0.5f, 48 - 0.5f, 1.0f, 1.0f );
    avRect[1] = D3DXVECTOR4( 576 - 0.5f, 48 - 0.5f, 1.0f, 1.0f );
    avRect[2] = D3DXVECTOR4( 576 - 0.5f, 88 - 0.5f, 1.0f, 1.0f );
    avRect[3] = D3DXVECTOR4(  64 - 0.5f, 88 - 0.5f, 1.0f, 1.0f );
    avRect[4] = avRect[0];

    m_pd3dDevice->DrawPrimitiveUP( D3DPT_LINESTRIP, 4, avRect, sizeof( D3DXVECTOR4 ) );
}




//-----------------------------------------------------------------------------
// Name: RenderStartScreen()
// Desc: Startup screen
//-----------------------------------------------------------------------------
VOID CXBVirtualKeyboard::RenderStartScreen() const
{
    m_FontLatin.DrawText( 320.0f, 50.0f, 0xFFFFFFFF,
                          L"This sample is intended to show appropriate\n"
                          L"functionality only. Please do not lift the\n"
                          L"graphics for use in your game.\n"
                          L"Source code for this sample is located at\n"
                          L"Samples\\Xbox\\ReferenceUI\\VirtualKeyboard\n"
                          L"A description of the user research that\n"
                          L"went into the creation of this sample is\n"
                          L"located in the XDK documentation at\n"
                          L"Developing for Xbox - Reference User Interface.",
                          XBFONT_CENTER_X );

    WCHAR strButton[2] = { BTN_A, 0 };
    m_FontBtn.DrawText( 270.0f, BUTTON_Y, BUTTON_COLOR, strButton );
    m_FontLatin.DrawText( 270.0f + BUTTON_OFFSET, BUTTON_Y, BUTTON_TEXT, 
                          L"continue" );
}




//-----------------------------------------------------------------------------
// Name: RenderMenu()
// Desc: Main menu
//-----------------------------------------------------------------------------
VOID CXBVirtualKeyboard::RenderMenu() const
{
    const D3DCOLOR COLOR_DEFAULT = 0xFFFFFFFF; // white
    const D3DCOLOR COLOR_SELECT  = 0xFF00FF00; // green
    const D3DCOLOR COLOR_LIGHT   = 0xFF808080; // gray

    CXBFont& Font = ( m_iLanguage == XC_LANGUAGE_JAPANESE ) ? 
                    m_FontJapan : m_FontLatin;

    Font.DrawText( 320.0f, 60.0f, COLOR_DEFAULT, 
                   strCHOOSE_KEYBOARD[ m_iLanguage ], XBFONT_CENTER_X );
    UINT j = 0;
    for( UINT i = XC_LANGUAGE_ENGLISH; i < XC_LANGUAGE_MAX; ++i )
    {
#ifndef VKEY_JAPAN
        // Don't display Japanese as an option
        if( i == XC_LANGUAGE_JAPANESE )
            continue;
#endif

        D3DCOLOR selColor = COLOR_DEFAULT;
        if( m_iLanguage == i )
            selColor = COLOR_SELECT;
#ifdef VKEY_JAPAN
        if( i == XC_LANGUAGE_JAPANESE )
            m_FontJapan.DrawText( 280.0f, 105.0f + ( j++ * 30.0f ), selColor,
                                  g_strLanguage[i] );
        else
#endif
            m_FontLatin.DrawText( 280.0f, 100.0f + ( j++ * 30.0f ), selColor,
                                  g_strLanguage[i] );
    }

    if( m_iLanguage != XC_LANGUAGE_ENGLISH )
    {
        m_FontLatin.DrawText( 320.0f, 280.0f, COLOR_LIGHT, 
                              L"Localized terminology not final.\n"
                              L"See Xbox Guide for final terms.",
                              XBFONT_CENTER_X );
    }

    Font.DrawText( 320.0f, m_iLanguage == XC_LANGUAGE_ENGLISH ?
                   320.0f : 344.0f, COLOR_LIGHT, 
                   strILLUSTRATIVE_GRAPHICS[ m_iLanguage ],
                   XBFONT_CENTER_X );

    DrawButton( 270.0f, BTN_A );
}




//-----------------------------------------------------------------------------
// Name: RenderKeyboard()
// Desc: Display current keyboard
//-----------------------------------------------------------------------------
VOID CXBVirtualKeyboard::RenderKeyboard() const
{
    if( m_iLanguage == XC_LANGUAGE_JAPANESE )
        RenderKeyboardJapan();
    else
        RenderKeyboardLatin();
}




//-----------------------------------------------------------------------------
// Name: RenderKeyboardLatin()
// Desc: Display current latin keyboard
//-----------------------------------------------------------------------------
VOID CXBVirtualKeyboard::RenderKeyboardLatin() const
{
    // Show text and caret
    DrawTextBox();
    DrawText( 68.0f, 54.0f );

    // Draw each row
    FLOAT fY = 120.0f;
    const Keyboard& keyBoard = m_KeyboardList[ m_iCurrBoard ];
    for( DWORD row = 0; row < MAX_ROWS; ++row, fY += KEY_HEIGHT )
    {
        FLOAT fX = 64.0f;
        FLOAT fWidthSum = 0.0f;
        const KeyRow& keyRow = keyBoard[ row ];
        DWORD dwIndex = 0;
        for( KeyRow::const_iterator i = keyRow.begin(); 
             i != keyRow.end(); ++i, ++dwIndex )
        {
            // Determine key name
            const Key& key = *i;
            D3DCOLOR selKeyColor = 0x00000000;
            D3DCOLOR selTextColor = COLOR_NORMAL;

            // Handle special key coloring
            switch( key.xKey )
            {
                case XK_SHIFT:
                    switch( m_iCurrBoard )
                    {
                        case TYPE_ALPHABET:
                        case TYPE_ACCENTS:
                            if( m_bIsShiftOn )
                                selKeyColor = COLOR_PRESSED;
                            break;
                        case TYPE_SYMBOLS:
                            selKeyColor = COLOR_DISABLED;
                            selTextColor = COLOR_FONT_DISABLED;
                            break;
                    }
                    break;
                case XK_CAPSLOCK:
                    switch( m_iCurrBoard )
                    {
                        case TYPE_ALPHABET:
                        case TYPE_ACCENTS:
                            if( m_bIsCapsLockOn )
                                selKeyColor = COLOR_PRESSED;
                            break;
                        case TYPE_SYMBOLS:
                            selKeyColor = COLOR_DISABLED;
                            selTextColor = COLOR_FONT_DISABLED;
                            break;
                    }
                    break;
                case XK_ACCENTS:
                    if( m_iLanguage == XC_LANGUAGE_ENGLISH )
                    {
                        selKeyColor = COLOR_INVISIBLE;
                        selTextColor = COLOR_INVISIBLE;
                    }
                    break;
            }

            // Highlight the current key
            if( row == m_iCurrRow && dwIndex == m_iCurrKey )
                selKeyColor |= COLOR_HIGHLIGHT;

            RenderKey( fX + fWidthSum, fY, key, selKeyColor, selTextColor );

            fWidthSum += key.dwWidth;

            // There's a slightly larger gap between the leftmost keys (mode
            // keys) and the main keyboard
            if( dwIndex == 0 )
                fWidthSum += GAP2_WIDTH;
            else
                fWidthSum += GAP_WIDTH;
        }
    }

    DrawButton( 80.0f,  BTN_A );
    DrawButton( 280.0f, BTN_Y );
    DrawButton( 460.0f, BTN_B );
}




//-----------------------------------------------------------------------------
// Name: RenderKeyboardJapan()
// Desc: Display current Japanese keyboard
//-----------------------------------------------------------------------------
VOID CXBVirtualKeyboard::RenderKeyboardJapan() const
{
    assert( m_iLanguage == XC_LANGUAGE_JAPANESE );

    // Show the controller
    DrawControllerS();

    // Show text and caret
    DrawTextBox();
    DrawText( 68.0f, 54.0f );

    const WCHAR *cChar = JH_A JH_KA JH_SA JH_TA JH_NA JH_HA JH_MA JH_YA
                         JH_LA JH_GA JH_ZA JH_DA JH_PA JH_BA JH_RA JH_WA
                         JK_A JK_KA JK_SA JK_TA JK_NA JK_HA JK_MA JK_YA
                         JK_LA JK_GA JK_ZA JK_DA JK_PA JK_BA JK_RA JK_WA;

    const WCHAR *cCharX = JH_A JH_I JH_U JH_E JH_O JH_NULL JH_KA JH_KI JH_KU 
                          JH_KE JH_KO JH_NULL JH_SA JH_SI JH_SU JH_SE JH_SO 
                          JH_NULL JH_TA JH_TI JH_TU JH_TE JH_TO JH_NULL JH_NA 
                          JH_NI JH_NU JH_NE JH_NO JH_NULL JH_HA JH_HI JH_HU 
                          JH_HE JH_HO JH_NULL JH_MA JH_MI JH_MU JH_ME JH_MO 
                          JH_NULL JH_YA JH_LYA JH_YU JH_LYU JH_YO JH_LYO JH_N 
                          JH_LTU JH_WO JH_NULL JH_NULL JH_NULL JH_LA JH_LI 
                          JH_LU JH_LE JH_LO JH_NULL JH_GA JH_GI JH_GU JH_GE 
                          JH_GO JH_NULL JH_ZA JH_ZI JH_ZU JH_ZE JH_ZO JH_NULL 
                          JH_DA JH_DI JH_DU JH_DE JH_DO JH_NULL JH_PA JH_PI 
                          JH_PU JH_PE JH_PO JH_NULL JH_BA JH_BI JH_BU JH_BE 
                          JH_BO JH_NULL JH_RA JH_RI JH_RU JH_RE JH_RO JH_NULL 
                          JH_WA JH_WI JH_LWA JH_WE JH_WO JH_NULL JH_N JH_LTU 
                          JH_WO JH_NULL JH_NULL JH_NULL JK_A JK_I JK_U JK_E 
                          JK_O JK_NULL JK_KA JK_KI JK_KU JK_KE JK_KO JK_NULL
                          JK_SA JK_SI JK_SU JK_SE JK_SO JK_NULL JK_TA JK_TI 
                          JK_TU JK_TE JK_TO JK_NULL JK_NA JK_NI JK_NU JK_NE 
                          JK_NO JK_NULL JK_HA JK_HI JK_HU JK_HE JK_HO JK_NULL
                          JK_MA JK_MI JK_MU JK_ME JK_MO JK_NULL JK_YA JK_LYA
                          JK_YU JK_LYU JK_YO JK_LYO JK_N JK_LTU JK_WO JK_DASH
                          JK_NULL JK_NULL JK_LA JK_LI JK_LU JK_LE JK_LO JK_NULL
                          JK_GA JK_GI JK_GU JK_GE JK_GO JK_NULL JK_ZA JK_ZI 
                          JK_ZU JK_ZE JK_ZO JK_NULL JK_DA JK_DI JK_DU JK_DE 
                          JK_DO JK_NULL JK_PA JK_PI JK_PU JK_PE JK_PO JK_NULL
                          JK_BA JK_BI JK_BU JK_BE JK_BO JK_NULL JK_RA JK_RI 
                          JK_RU JK_RE JK_RO JK_NULL JK_WA JK_WI JK_VU JK_WE
                          JK_WO JK_NULL JK_N JK_LTU JK_WO JK_DASH JK_NULL
                          JK_NULL;

    const WCHAR *strHiragana = JH_HI JH_RA JH_GA JH_NA;
    const WCHAR *strKatakana = JK_KA JK_TA JK_KA JK_NA;

    const WCHAR *strDESCRIPTION[8] =
    {
        { JK_DASH JH_SO JH_U JH_SA JH_SE JH_TU JH_ME JH_I JK_DASH },
        { JH_HI JH_DA JH_RI JH_NO JK_A JK_NA JK_RO JK_GU JK_PA JK_LTU JK_DO 
          JH_DE JH_SI JH_I JH_N JH_WO JH_KI JH_ME JH_TE L"\n"
          JH_MI JH_GI JH_NO JK_BO JK_TA JK_N JH_DE JH_MO JH_ZI JH_WO JH_U 
          JH_TI JH_MA JH_SU },
        { JH_U JH_RA JH_GA JH_WA JH_NI JH_A JH_RU JH_MI JH_GI JH_NO JK_TO JK_RI JK_GA JH_DE JH_SI JH_I 
          JH_N JH_WO }, 
        { JH_HI JH_DA JH_RI JH_NO JK_TO JK_RI JK_GA JH_DE JH_HI JH_RA JH_GA
          JH_NA JH_TO JK_KA JK_TA JK_KA JK_NA JH_WO L"\n"
          JH_KO JH_U JH_GO JH_NI JH_KI JH_RI JH_KA JH_E JH_RU JH_KO JH_TO 
          JH_GA JH_DE JH_KI JH_MA JH_SU },
        { JK_DE JK_ZI JK_TA JK_RU JK_PA JK_LTU JK_DO JH_NO JH_SA JH_YU JH_U 
          JH_NO JK_KI JK_DASH JH_DE L"\n"
          JK_KA JK_DASH JK_SO JK_RU JH_GA JH_I JH_DO JH_U JH_SI JH_MA JH_SU },
        { JH_MO JH_SI JH_U JH_TI JH_MA JH_TI JH_GA JH_E JH_TA JH_RA L"\n"
          JH_KU JH_RO JH_KA JH_SI JH_RO JH_NO JK_BO JK_TA JK_N JH_DE JH_MO 
          JH_ZI JH_WO JH_SA JH_KU JH_ZI JH_LYO JH_SI JH_TE JH_KU JH_DA JH_SA JH_I },
        { JK_BA JK_LTU JK_KU JK_BO JK_TA JK_N JH_WO JH_O JH_SU JH_TO L"\n"
          JK_KI JK_DASH JK_BO JK_DASH JK_DO JK_SE JK_RE JK_KU JK_TO JH_NO 
          JK_ME JK_NI JK_LYU JK_DASH JH_NI JH_MO JH_DO JH_RI JH_MA JH_SU },
        { L"" }
    };

    // Determine color and direction values
    DWORD dwColor[16];
    for( INT i = 0; i < 16; i++ )
        dwColor[i] = 0xFFFFFFFF;

    INT nDirection; // 0 - 8
    if( m_dwCurrCtlrState & XKJ_UP )
    {
        if( m_dwCurrCtlrState & XKJ_RIGHT )
        {
            nDirection = 1;
            dwColor[1] = 0xFFFFFF00;
        }
        else if( m_dwCurrCtlrState & XKJ_LEFT )
        {
            nDirection = 7;
            dwColor[7] = 0xFFFFFF00;
        }
        else
        {
            nDirection = 0;
            dwColor[0] = 0xFFFFFF00;
        }
    }
    else if( m_dwCurrCtlrState & XKJ_DOWN )
    {
        if( m_dwCurrCtlrState & XKJ_RIGHT )
        {
            nDirection = 3;
            dwColor[3] = 0xFFFFFF00;
        }
        else if( m_dwCurrCtlrState & XKJ_LEFT )
        {
            nDirection = 5;
            dwColor[5] = 0xFFFFFF00;
        }
        else
        {
            nDirection = 4;
            dwColor[4] = 0xFFFFFF00;
        }
    }
    else if( m_dwCurrCtlrState & XKJ_RIGHT )
    {
        nDirection = 2;
        dwColor[2] = 0xFFFFFF00;
    }
    else if( m_dwCurrCtlrState & XKJ_LEFT )
    {
        nDirection = 6;
        dwColor[6] = 0xFFFFFF00;
    }
    else 
    {
        nDirection = 8;
    }

    if( m_dwOldCtlrState & XKJ_X )
        dwColor[8] = 0xFFFFFF00;
    if( m_dwOldCtlrState & XKJ_Y )
        dwColor[9] = 0xFFFFFF00;
    if( m_dwOldCtlrState & XKJ_B )
        dwColor[10] = 0xFFFFFF00;
    if( m_dwOldCtlrState & XKJ_A )
        dwColor[11] = 0xFFFFFF00;
    if( m_dwOldCtlrState & XKJ_WHITE )
        dwColor[12] = 0xFFFFFF00;
    if( m_dwOldCtlrState & XKJ_BLACK )
        dwColor[13] = 0xFFFFFF00;
    if( m_bTrig )
        dwColor[14] = 0xFFFFFF00;
    if( m_dwOldCtlrState & XKJ_LEFTTR )
        dwColor[15] = 0xFFFFFF00;

    WCHAR wcChar[8][2];
    for( i = 0; i < 8; i++ )
    {
        INT nStp = m_bTrig ? 1 : 0;
        if( m_bKana )
            nStp += 2;

        wcChar[i][0] = cChar[i + 8 * nStp];
        wcChar[i][1] = 0;
    }

    WCHAR wcCharX[6][2];
    DWORD wcCharXIdx[6];
    for( i = 0; i < 6; i++ )
    {
        INT nStp = m_bTrig ? ( 6 * 9 ) : 0;
        if( m_bKana ) 
            nStp += 6 * 9 * 2;

        wcCharXIdx[i] = i + 6 * nDirection + nStp;
        wcCharX[i][0] = cCharX[wcCharXIdx[i]];
        wcCharX[i][1] = 0;
    }

    // Japanese characters
    const Xkey cJAPAN_KEY[] =
    {
        XK_HIRAGANA_A,XK_HIRAGANA_I,XK_HIRAGANA_U,XK_HIRAGANA_E,XK_HIRAGANA_O,XK_DELETE,
        XK_HIRAGANA_KA,XK_HIRAGANA_KI,XK_HIRAGANA_KU,XK_HIRAGANA_KE,XK_HIRAGANA_KO,XK_DELETE,
        XK_HIRAGANA_SA,XK_HIRAGANA_SI,XK_HIRAGANA_SU,XK_HIRAGANA_SE,XK_HIRAGANA_SO,XK_DELETE,
        XK_HIRAGANA_TA,XK_HIRAGANA_TI,XK_HIRAGANA_TU,XK_HIRAGANA_TE,XK_HIRAGANA_TO,XK_DELETE,
        XK_HIRAGANA_NA,XK_HIRAGANA_NI,XK_HIRAGANA_NU,XK_HIRAGANA_NE,XK_HIRAGANA_NO,XK_DELETE,
        XK_HIRAGANA_HA,XK_HIRAGANA_HI,XK_HIRAGANA_HU,XK_HIRAGANA_HE,XK_HIRAGANA_HO,XK_DELETE,
        XK_HIRAGANA_MA,XK_HIRAGANA_MI,XK_HIRAGANA_MU,XK_HIRAGANA_ME,XK_HIRAGANA_MO,XK_DELETE,
        XK_HIRAGANA_YA,XK_HIRAGANA_LYA,XK_HIRAGANA_YU,XK_HIRAGANA_LYU,XK_HIRAGANA_YO,XK_HIRAGANA_LYO,
        XK_HIRAGANA_N,XK_HIRAGANA_LTU,XK_HIRAGANA_WO,XK_SHIFT,XK_BACKSPACE,XK_DELETE,

        XK_HIRAGANA_LA,XK_HIRAGANA_LI,XK_HIRAGANA_LU,XK_HIRAGANA_LE,XK_HIRAGANA_LO,XK_DELETE,
        XK_HIRAGANA_GA,XK_HIRAGANA_GI,XK_HIRAGANA_GU,XK_HIRAGANA_GE,XK_HIRAGANA_GO,XK_DELETE,
        XK_HIRAGANA_ZA,XK_HIRAGANA_ZI,XK_HIRAGANA_ZU,XK_HIRAGANA_ZE,XK_HIRAGANA_ZO,XK_DELETE,
        XK_HIRAGANA_DA,XK_HIRAGANA_DI,XK_HIRAGANA_DU,XK_HIRAGANA_DE,XK_HIRAGANA_DO,XK_DELETE,
        XK_HIRAGANA_PA,XK_HIRAGANA_PI,XK_HIRAGANA_PU,XK_HIRAGANA_PE,XK_HIRAGANA_PO,XK_DELETE,
        XK_HIRAGANA_BA,XK_HIRAGANA_BI,XK_HIRAGANA_BU,XK_HIRAGANA_BE,XK_HIRAGANA_BO,XK_DELETE,
        XK_HIRAGANA_RA,XK_HIRAGANA_RI,XK_HIRAGANA_RU,XK_HIRAGANA_RE,XK_HIRAGANA_RO,XK_DELETE,
        XK_HIRAGANA_WA,XK_HIRAGANA_WI,XK_HIRAGANA_LWA,XK_HIRAGANA_WE,XK_HIRAGANA_WO,XK_DELETE,
        XK_HIRAGANA_N,XK_HIRAGANA_LTU,XK_HIRAGANA_WO,XK_SHIFT,XK_BACKSPACE,XK_DELETE,

        XK_KATAKANA_A,XK_KATAKANA_I,XK_KATAKANA_U,XK_KATAKANA_E,XK_KATAKANA_O,XK_DELETE,
        XK_KATAKANA_KA,XK_KATAKANA_KI,XK_KATAKANA_KU,XK_KATAKANA_KE,XK_KATAKANA_KO,XK_DELETE,
        XK_KATAKANA_SA,XK_KATAKANA_SI,XK_KATAKANA_SU,XK_KATAKANA_SE,XK_KATAKANA_SO,XK_DELETE,
        XK_KATAKANA_TA,XK_KATAKANA_TI,XK_KATAKANA_TU,XK_KATAKANA_TE,XK_KATAKANA_TO,XK_DELETE,
        XK_KATAKANA_NA,XK_KATAKANA_NI,XK_KATAKANA_NU,XK_KATAKANA_NE,XK_KATAKANA_NO,XK_DELETE,
        XK_KATAKANA_HA,XK_KATAKANA_HI,XK_KATAKANA_HU,XK_KATAKANA_HE,XK_KATAKANA_HO,XK_DELETE,
        XK_KATAKANA_MA,XK_KATAKANA_MI,XK_KATAKANA_MU,XK_KATAKANA_ME,XK_KATAKANA_MO,XK_DELETE,
        XK_KATAKANA_YA,XK_KATAKANA_LYA,XK_KATAKANA_YU,XK_KATAKANA_LYU,XK_KATAKANA_YO,XK_KATAKANA_LYO,
        XK_KATAKANA_N,XK_KATAKANA_LTU,XK_KATAKANA_WO,XK_KATAKANA_DASH,XK_BACKSPACE,XK_DELETE,

        XK_KATAKANA_LA,XK_KATAKANA_LI,XK_KATAKANA_LU,XK_KATAKANA_LE,XK_KATAKANA_LO,XK_DELETE,
        XK_KATAKANA_GA,XK_KATAKANA_GI,XK_KATAKANA_GU,XK_KATAKANA_GE,XK_KATAKANA_GO,XK_DELETE,
        XK_KATAKANA_ZA,XK_KATAKANA_ZI,XK_KATAKANA_ZU,XK_KATAKANA_ZE,XK_KATAKANA_ZO,XK_DELETE,
        XK_KATAKANA_DA,XK_KATAKANA_DI,XK_KATAKANA_DU,XK_KATAKANA_DE,XK_KATAKANA_DO,XK_DELETE,
        XK_KATAKANA_PA,XK_KATAKANA_PI,XK_KATAKANA_PU,XK_KATAKANA_PE,XK_KATAKANA_PO,XK_DELETE,
        XK_KATAKANA_BA,XK_KATAKANA_BI,XK_KATAKANA_BU,XK_KATAKANA_BE,XK_KATAKANA_BO,XK_DELETE,
        XK_KATAKANA_RA,XK_KATAKANA_RI,XK_KATAKANA_RU,XK_KATAKANA_RE,XK_KATAKANA_RO,XK_DELETE,
        XK_KATAKANA_WA,XK_KATAKANA_WI,XK_KATAKANA_VU,XK_KATAKANA_WE,XK_KATAKANA_WO,XK_DELETE,
        XK_KATAKANA_N,XK_KATAKANA_LTU,XK_KATAKANA_WO,XK_KATAKANA_DASH,XK_BACKSPACE,XK_DELETE,
    };

    // Determine the key to press
    m_xNextKeyJpn = XK_NULL;
    if( m_dwCurrCtlrState & XKJ_X )
        m_xNextKeyJpn = cJAPAN_KEY[ wcCharXIdx[0] ];
    else if( m_dwCurrCtlrState & XKJ_Y )
        m_xNextKeyJpn = cJAPAN_KEY[wcCharXIdx[1]];
    else if( m_dwCurrCtlrState & XKJ_B )
        m_xNextKeyJpn = cJAPAN_KEY[ wcCharXIdx[2] ];
    else if( m_dwCurrCtlrState & XKJ_A )
        m_xNextKeyJpn = cJAPAN_KEY[ wcCharXIdx[3] ];
    else if( m_dwCurrCtlrState & XKJ_WHITE )
        m_xNextKeyJpn = cJAPAN_KEY[ wcCharXIdx[4] ];
    else if( m_dwCurrCtlrState & XKJ_BLACK )
        m_xNextKeyJpn = cJAPAN_KEY[ wcCharXIdx[5] ];
    else if( m_dwCurrCtlrState & XKJ_DLEFT )
        m_xNextKeyJpn = XK_ARROWLEFT;
    else if( m_dwCurrCtlrState & XKJ_DRIGHT )
        m_xNextKeyJpn = XK_ARROWRIGHT;

    DWORD dwFontFlags = XBFONT_CENTER_X | XBFONT_CENTER_Y;
    DrawJapanese( 230.0f, 142.0f, dwColor[0], wcChar[0], dwFontFlags );
    DrawJapanese( 252.0f, 151.0f, dwColor[1], wcChar[1], dwFontFlags );
    DrawJapanese( 268.0f, 169.0f, dwColor[2], wcChar[2], dwFontFlags );
    DrawJapanese( 261.0f, 193.0f, dwColor[3], wcChar[3], dwFontFlags );
    DrawJapanese( 239.0f, 206.0f, dwColor[4], wcChar[4], dwFontFlags );
    DrawJapanese( 217.0f, 199.0f, dwColor[5], wcChar[5], dwFontFlags );
    DrawJapanese( 200.0f, 178.0f, dwColor[6], wcChar[6], dwFontFlags );
    DrawJapanese( 208.0f, 156.0f, dwColor[7], wcChar[7], dwFontFlags );
    DrawJapanese( 400.0f, 200.0f, dwColor[8], wcCharX[0], dwFontFlags );
    DrawJapanese( 420.0f, 180.0f, dwColor[9], wcCharX[1], dwFontFlags );
    DrawJapanese( 450.0f, 200.0f, dwColor[10], wcCharX[2], dwFontFlags );
    DrawJapanese( 430.0f, 220.0f, dwColor[11], wcCharX[3], dwFontFlags );
    DrawJapanese( 440.0f, 280.0f, dwColor[12], wcCharX[4], dwFontFlags );
    DrawJapanese( 460.0f, 260.0f, dwColor[13], wcCharX[5], dwFontFlags );

    DrawJapanese( 140.0f, 238.0f, 0xFF00FFFF, JK_ME JK_NI JK_LYU JK_DASH,
                  dwFontFlags );

    // Kana or Hiragana
    const WCHAR *strKanaGana = m_bKana ? strKatakana : strHiragana;
    DrawJapanese( 135.0f, 145.0f, dwColor[15], strKanaGana, dwFontFlags );

    // Show description string
    static INT nCounter = 0;
    ++nCounter &= 0x00000fff;
    INT nDescript = (nCounter >> 9) & 7;
    DrawJapanese( 320.0f, 410.0f, 0xFFFFFFFF, strDESCRIPTION[ nDescript ], 
                  dwFontFlags );
}




//-----------------------------------------------------------------------------
// Name: RenderHelp()
// Desc: Display controller mappings
//-----------------------------------------------------------------------------
VOID CXBVirtualKeyboard::RenderHelp() const
{
    RenderKeyboard();

    const DWORD NUM_HELP_CALLOUTS = 9;
    const WORD PLC1 = XBHELP_PLACEMENT_1;
    const WORD PLC2 = XBHELP_PLACEMENT_2;
    XBHELP_CALLOUT HelpCallouts[] =
    {
        { XBHELP_A_BUTTON,     PLC2, g_HelpStr[ m_iLanguage ].strSelect    },
        { XBHELP_START_BUTTON, PLC2, g_HelpStr[ m_iLanguage ].strSelect    },
        { XBHELP_B_BUTTON,     PLC2, g_HelpStr[ m_iLanguage ].strCancel    },
        { XBHELP_BACK_BUTTON,  PLC2, g_HelpStr[ m_iLanguage ].strCancel    },
        { XBHELP_X_BUTTON,     PLC1, g_HelpStr[ m_iLanguage ].strToggle    },
        { XBHELP_Y_BUTTON,     PLC2, g_HelpStr[ m_iLanguage ].strHelp      },
        { XBHELP_WHITE_BUTTON, PLC2, g_HelpStr[ m_iLanguage ].strBackspace },
        { XBHELP_BLACK_BUTTON, PLC2, g_HelpStr[ m_iLanguage ].strSpace     },
        { XBHELP_MISC_CALLOUT, PLC2, g_HelpStr[ m_iLanguage ].strTrigger   }
    };

    m_Help.Render( &m_FontLatin, HelpCallouts, NUM_HELP_CALLOUTS );
}




//-----------------------------------------------------------------------------
// Name: DrawText()
// Desc: Draw text in the text input area, accounting for special characters
//-----------------------------------------------------------------------------
VOID CXBVirtualKeyboard::DrawText( FLOAT x, FLOAT y ) const
{
    if( m_iLanguage == XC_LANGUAGE_JAPANESE )
        DrawTextJapan( x, y );
    else
        DrawTextLatin( x, y );
}




//-----------------------------------------------------------------------------
// Name: DrawTextLatin()
// Desc: Draw text in the text input area, accounting for special characters
//-----------------------------------------------------------------------------
VOID CXBVirtualKeyboard::DrawTextLatin( FLOAT x, FLOAT y ) const
{
    FLOAT fEuroWidth = 0.0f;

    // We have to do some unusual text handling for large-value Unicode
    // symbols that aren't in the regular font file. The reason they're not
    // in the regular font file is that they'd make the bitmapped font
    // image enormous.
    String::const_iterator iStart = m_strData.begin();
    while ( iStart != m_strData.end() )
    {
        String::const_iterator iEnd = std::find( iStart, m_strData.end(), XK_EURO_SIGN );

        // Determine the string to be drawn
        String strDraw;
        BOOL bFoundEuro = iEnd != m_strData.end();
        if( bFoundEuro )
        {
            if( iEnd > iStart )
                strDraw.assign( iStart, iEnd );
            iStart = iEnd + 1;
        }
        else
        {
            strDraw.assign( iStart, iEnd );
            iStart = iEnd;
        }

        // Draw everything up to, but not including, the Euro character
        m_FontLatin.DrawText( x, y, COLOR_TEXT, strDraw.c_str() );

        // Move the drawing position
        FLOAT fWidth;
        FLOAT fHeight;
        m_FontLatin.GetTextExtent( strDraw.c_str(), &fWidth, &fHeight );
        x += fWidth;

        // Draw the Euro character
        if( bFoundEuro )
        {
            WCHAR strEuro[] = { XK_EURO_SIGN, 0 };
            m_FontEuro.DrawText( x, y, COLOR_TEXT, strEuro );

            // Move the drawing position
            m_FontEuro.GetTextExtent( strEuro, &fEuroWidth, &fHeight );
            x += fEuroWidth;
        }
    }

    // Determine caret location
    String strUpToCaret( m_strData, 0, m_iPos );
    FLOAT fWidth;
    FLOAT fHeight;
    m_FontLatin.GetTextExtent( strUpToCaret.c_str(), &fWidth, &fHeight );

    // Account for any Euro symbols
    fWidth += std::count( strUpToCaret.begin(), strUpToCaret.end(), XK_EURO_SIGN ) *
              fEuroWidth;

    // Draw blinking caret using a font for simplicity. Better method
    // would draw an "I-bar" using line primitives.
    if( fmod( m_CaretTimer.GetElapsedSeconds(), fCARET_BLINK_RATE ) < 
        fCARET_ON_RATIO )
    {
        m_FontLatin.DrawText( 68.0f + fWidth - 2.0f, 56.0f, COLOR_HIGHLIGHT, L"|" );
        m_FontLatin.DrawText( 68.0f + fWidth - 2.0f, 46.0f, COLOR_HIGHLIGHT, L"|" );
    }
}




//-----------------------------------------------------------------------------
// Name: DrawTextJapan()
// Desc: Draw text in the text input area, accounting for special characters
//-----------------------------------------------------------------------------
VOID CXBVirtualKeyboard::DrawTextJapan( FLOAT x, FLOAT y ) const
{
    String::const_iterator it;
    WCHAR jChar = 0;
    for( it = m_strData.begin(); it != m_strData.end(); it++ )
    {
        BOOL bJChar = FALSE; 
        String::const_reference currentChar = *it;

        for( jChar = XK_HIRAGANA_SP; jChar <= XK_HIRAGANA_VU; jChar++ )
        {
            if( currentChar == jChar)
            {
                bJChar = TRUE;
                break;
            }
        }
        if( !bJChar )
        {
            for( jChar = XK_KATAKANA_SP; jChar <= XK_KATAKANA_DASH; jChar++ )
            {
                if( currentChar == jChar)
                {
                    bJChar = TRUE;
                    break;
                }
            }
        }
        if( bJChar )
        {
            WCHAR strJapan[] = { currentChar, 0 };
            FLOAT fFixedY = 0.0f;

#if FIXED_JAPANESE_SMALL_LETTER

            // Arial MS Unicodeフォントの小さい文字の位置がずれているので表示位置を調節する
            WCHAR cSmallLetter[] =
            {
                XK_HIRAGANA_LA, XK_HIRAGANA_LI, XK_HIRAGANA_LU, XK_HIRAGANA_LE, 
                XK_HIRAGANA_LO, XK_HIRAGANA_LTU, XK_HIRAGANA_LYA, XK_HIRAGANA_LYU, 
                XK_HIRAGANA_LYO, XK_HIRAGANA_LWA, XK_KATAKANA_LA, XK_KATAKANA_LI, 
                XK_KATAKANA_LU, XK_KATAKANA_LE, XK_KATAKANA_LO, XK_KATAKANA_LTU, 
                XK_KATAKANA_LYA, XK_KATAKANA_LYU, XK_KATAKANA_LYO, XK_KATAKANA_LWA,
            };

            for( INT i = sizeof(cSmallLetter) / sizeof(WCHAR); i; i-- )
            {
                if( cSmallLetter[i - 1] == currentChar )
                    break;
            }
            if( i )
                fFixedY = FIXED_JSL_SIZE;
#endif
            m_FontJapan.DrawText( x, y + fFixedY, COLOR_TEXT, strJapan );

            // Move the drawing position
            FLOAT fWidth;
            FLOAT fHeight;
            m_FontJapan.GetTextExtent( strJapan, &fWidth, &fHeight );
            x += fWidth;
        }
    }

    String strUpToCaret( m_strData, 0, m_iPos );
    FLOAT fWidth;
    FLOAT fHeight;

    // Determine caret location
    m_FontLatin.GetTextExtent( strUpToCaret.c_str(), &fWidth, &fHeight );

    // キャレット位置をすべてのひらがなとカタカナから得る
    // Account for any Japanese symbols
    for( jChar = XK_HIRAGANA_SP; jChar <= XK_HIRAGANA_VU; jChar++ )
    {
        fWidth += std::count( strUpToCaret.begin(), strUpToCaret.end(), jChar ) *
                  21.0f;
    }
    for( jChar = XK_KATAKANA_SP; jChar <= XK_KATAKANA_DASH; jChar++ )
    {
        fWidth += std::count( strUpToCaret.begin(), strUpToCaret.end(), jChar ) *
                  21.0f;
    }

    // Draw blinking caret using a font for simplicity. Better method
    // would draw an "I-bar" using line primitives.
    if( fmod( m_CaretTimer.GetElapsedSeconds(), fCARET_BLINK_RATE ) < 
        fCARET_ON_RATIO )
    {
        m_FontLatin.DrawText( 68.0f + fWidth - 2.0f, 56.0f, COLOR_HIGHLIGHT, L"|" );
        m_FontLatin.DrawText( 68.0f + fWidth - 2.0f, 46.0f, COLOR_HIGHLIGHT, L"|" );
    }
}




//-----------------------------------------------------------------------------
// Name: DrawJapanese()
// Desc: 日本語用のキーボード表示
//       日本語フォントを影つきで表示する
//-----------------------------------------------------------------------------
VOID CXBVirtualKeyboard::DrawJapanese( FLOAT x, FLOAT y, DWORD dwColor, 
                                       const WCHAR* str, DWORD flag ) const
{
    m_FontJapan.DrawText( x+2.0f, y+2.0f, COLOR_RED, str, flag );
    m_FontJapan.DrawText( x, y, dwColor, str, flag );
}




//-----------------------------------------------------------------------------
// Name: DrawButton()
// Desc: Display button image and descriptive text
//-----------------------------------------------------------------------------
VOID CXBVirtualKeyboard::DrawButton( FLOAT fX, WCHAR chButton ) const
{
    const WCHAR* strText = NULL;
    switch( chButton )
    {
        case BTN_A : strText = strA_SELECT[ m_iLanguage ]; break;
        case BTN_B : strText = strB_BACK  [ m_iLanguage ]; break;
        case BTN_Y : strText = strY_HELP  [ m_iLanguage ]; break;
        default : assert( FALSE ); return;
    }
    WCHAR strButton[2] = { chButton, 0 };

    m_FontBtn.DrawText( fX, BUTTON_Y, BUTTON_COLOR, strButton );
#ifdef VKEY_JAPAN
    if( m_iLanguage == XC_LANGUAGE_JAPANESE )
        m_FontJapan.DrawText( fX + BUTTON_OFFSET, BUTTON_Y, BUTTON_TEXT, strText );
    else
#endif
        m_FontLatin.DrawText( fX + BUTTON_OFFSET, BUTTON_Y, BUTTON_TEXT, strText );
}




//-----------------------------------------------------------------------------
// Name: DrawControllerS()
// Desc: 日本向けコントローラのイメージを表示する
//-----------------------------------------------------------------------------
HRESULT CXBVirtualKeyboard::DrawControllerS() const
{
    assert( m_iLanguage == XC_LANGUAGE_JAPANESE );

    // Setup vertices for a background-covering quad
    struct BACKGROUNDVERTEX
    { 
        FLOAT x;    // x,y,z,w form D3DXVECTOR4
        FLOAT y;
        FLOAT z;
        FLOAT w;
        FLOAT u;
        FLOAT v;
    };
    BACKGROUNDVERTEX v[4] =
    {
        {   0+120 - 0.5f,   0+120 - 0.5f, 1.0f, 1.0f,    0,   0 },
        { 400+120 - 0.5f,   0+120 - 0.5f, 1.0f, 1.0f,  400,   0 },
        {   0+120 - 0.5f, 320+120 - 0.5f, 1.0f, 1.0f,    0, 320 },
        { 400+120 - 0.5f, 320+120 - 0.5f, 1.0f, 1.0f,  400, 320 }
    };

    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ADDRESSU, D3DTADDRESS_CLAMP );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ADDRESSV, D3DTADDRESS_CLAMP );
    m_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );

    // Set states
    m_pd3dDevice->SetTexture( 0, m_ptControllerS );
    m_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE ); 
    m_pd3dDevice->SetVertexShader( D3DFVF_XYZRHW|D3DFVF_TEX1 );
    m_pd3dDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE);

    // Render the quad
    m_pd3dDevice->DrawPrimitiveUP( D3DPT_TRIANGLESTRIP, 2, v, 
                                   sizeof(BACKGROUNDVERTEX) );
    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: IsDisabled()
// Desc: TRUE if the current key (m_iCurrBoard, m_iCurrRow, m_iCurrKey) is
//       disabled.
//-----------------------------------------------------------------------------
BOOL CXBVirtualKeyboard::IsKeyDisabled() const
{
    Key key = m_KeyboardList[ m_iCurrBoard ][ m_iCurrRow ][ m_iCurrKey ];

    // On the symbols keyboard, Shift and Caps Lock are disabled
    if( m_iCurrBoard == TYPE_SYMBOLS )
    {
        if( key.xKey == XK_SHIFT || key.xKey == XK_CAPSLOCK )
            return TRUE;
    }

    // On the English keyboard, the Accents key is disabled
    if( m_iLanguage == XC_LANGUAGE_ENGLISH )
    {
        if( key.xKey == XK_ACCENTS )
            return TRUE;
    }

    return FALSE;
}




//-----------------------------------------------------------------------------
// Name: PlayClick()
// Desc: Produces an audible click to meet certification requirements
//-----------------------------------------------------------------------------
VOID CXBVirtualKeyboard::PlayClick() const
{
    m_ClickSnd.Play();
}




//-----------------------------------------------------------------------------
// Name: GetChar()
// Desc: Convert Xkey value to WCHAR given current capitalization settings
//-----------------------------------------------------------------------------
WCHAR CXBVirtualKeyboard::GetChar( Xkey xk ) const
{
    // Handle case conversion
    WCHAR wc = WCHAR( xk );

    if( ( m_bIsCapsLockOn && !m_bIsShiftOn ) ||
        ( !m_bIsCapsLockOn && m_bIsShiftOn ) )
        wc = ToUpper( wc );
    else
        wc = ToLower( wc );

    return wc;
}




//-----------------------------------------------------------------------------
// Name: ToUpper()
// Desc: Convert WCHAR to upper case. Handles accented characters properly.
//-----------------------------------------------------------------------------
WCHAR CXBVirtualKeyboard::ToUpper( WCHAR c ) // static
{
#ifdef USE_CONVERSTION_TABLE

    // The table-based solution is faster, but requires 512 bytes of space
    static const WCHAR arrToUpper[] =
    {
        0,   1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, 15,
        16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31,
        32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47,
        48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63,
        64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79,
        80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 91, 92, 93, 94, 95, 96

        // alpha mapping here
        'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M',
        'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z',
        
        123, 124, 125, 126, 127,
        0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87,
        0x88, 0x89, 0x8A, 0x8B, 0x8C, 0x8D, 0x8E, 0x8F,
        0x90, 0x91, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97,
        0x98, 0x99, 0x9A, 0x9B, 0x9C, 0x9D, 0x9E, 0x9F,
        0xA0, 0xA1, 0xA2, 0xA3, 0xA4, 0xA5, 0xA6, 0xA7,
        0xA8, 0xA9, 0xAA, 0xAB, 0xAC, 0xAD, 0xAE, 0xAF,
        0xB0, 0xB1, 0xB2, 0xB3, 0xB4, 0xB5, 0xB6, 0xB7,
        0xB8, 0xB9, 0xBA, 0xBB, 0xBC, 0xBD, 0xBE, 0xBF,
        0xC0, 0xC1, 0xC2, 0xC3, 0xC4, 0xC5, 0xC6, 0xC7,
        0xC8, 0xC9, 0xCA, 0xCB, 0xCC, 0xCD, 0xCE, 0xCF,
        0xD0, 0xD1, 0xD2, 0xD3, 0xD4, 0xD5, 0xD6, 0xD7,
        0xD8, 0xD9, 0xDA, 0xDB, 0xDC, 0xDD, 0xDE, 0xDF,

        // accented character mapping here
        0xC0, 0xC1, 0xC2, 0xC3, 0xC4, 0xC5, 0xC6, 0xC7,
        0xC8, 0xC9, 0xCA, 0xCB, 0xCC, 0xCD, 0xCE, 0xCF,
        0xD0, 0xD1, 0xD2, 0xD3, 0xD4, 0xD5, 0xD6, 0xD7,
        0xD8, 0xD9, 0xDA, 0xDB, 0xDC, 0xDD, 0xFE, 0x0178,
    };

    if( c > 0xFF )
        return c;
    return arrToUpper[ c ];

#else

    // The code solution is slower but smaller
    if( c >= 'a' && c <= 'z' )
        return c - ('a' - 'A');
    if( c >= 0xE0 && c <= 0xFD )
        return c - (0xE0 - 0xC0);
    if( c == XK_SM_Y_DIAERESIS )    // 0x00FF
        return XK_CAP_Y_DIAERESIS;  // 0x0178
    return c;

#endif

}




//-----------------------------------------------------------------------------
// Name: ToLower()
// Desc: Convert WCHAR to lower case. Handles accented characters properly.
//-----------------------------------------------------------------------------
WCHAR CXBVirtualKeyboard::ToLower( WCHAR c ) // static
{
#ifdef USE_CONVERSTION_TABLE

    // The table-based solution is faster, but requires 512 bytes of space
    static const WCHAR arrToLower[] =
    {
        0,   1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, 15,
        16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31,
        32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47,
        48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64

        // alpha mapping here
        'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm',
        'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z',

        91, 92, 93, 94, 95, 96, 97, 98, 99, 100, 101, 102, 103, 104,
        105, 106, 107, 108, 109, 110, 111, 112, 113, 114, 115, 116, 117,
        118, 119, 120, 121, 122, 123, 124, 125, 126, 127,

        0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87,
        0x88, 0x89, 0x8A, 0x8B, 0x8C, 0x8D, 0x8E, 0x8F,
        0x90, 0x91, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97,
        0x98, 0x99, 0x9A, 0x9B, 0x9C, 0x9D, 0x9E, 0x9F,
        0xA0, 0xA1, 0xA2, 0xA3, 0xA4, 0xA5, 0xA6, 0xA7,
        0xA8, 0xA9, 0xAA, 0xAB, 0xAC, 0xAD, 0xAE, 0xAF,
        0xB0, 0xB1, 0xB2, 0xB3, 0xB4, 0xB5, 0xB6, 0xB7,
        0xB8, 0xB9, 0xBA, 0xBB, 0xBC, 0xBD, 0xBE, 0xBF,

        // accented character mapping here
        0xE0, 0xE1, 0xE2, 0xE3, 0xE4, 0xE5, 0xE6, 0xE7,
        0xE8, 0xE9, 0xEA, 0xEB, 0xEC, 0xED, 0xEE, 0xEF,
        0xF0, 0xF1, 0xF2, 0xF3, 0xF4, 0xF5, 0xF6, 0xF7,
        0xF8, 0xF9, 0xFA, 0xFB, 0xFC, 0xFD, 
        
        0xDE, 0xDF,
        0xE0, 0xE1, 0xE2, 0xE3, 0xE4, 0xE5, 0xE6, 0xE7,
        0xE8, 0xE9, 0xEA, 0xEB, 0xEC, 0xED, 0xEE, 0xEF,
        0xF0, 0xF1, 0xF2, 0xF3, 0xF4, 0xF5, 0xF6, 0xF7,
        0xF8, 0xF9, 0xFA, 0xFB, 0xFC, 0xFD, 0xFE, 0xFF,
    };

    if( c == XK_CAP_Y_DIAERESIS ) // 0x0178
        return XK_SM_Y_DIAERESIS;
    if( c > 0xFF )
        return c;
    return arrToLower[ c ];

#else

    // The code solution is slower but smaller
    if( c >= 'A' && c <= 'Z' )
        return c + ( 'a' - 'A' );
    if( c >= 0xC0 && c <= 0xDD )
        return c + ( 0xE0 - 0xC0 );
    if( c == XK_CAP_Y_DIAERESIS ) // 0x0178
        return XK_SM_Y_DIAERESIS;
    return c;

#endif
}
