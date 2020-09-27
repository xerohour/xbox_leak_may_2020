//-----------------------------------------------------------------------------
// File: VirtualKeyboard.h
//
// Desc: Virtual keyboard reference UI
//
// Hist: 02.13.01 - New for March XDK release 
//       03.07.01 - Localized for April XDK release
//       04.10.01 - Updated for May XDK with full translations
//       06.06.01 - Japanese keyboard added
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#ifndef XBVIRTUAL_KEYBOARD_H
#define XBVIRTUAL_KEYBOARD_H

#pragma warning( disable: 4710 4786 )
#include <xtl.h>
#include <XbApp.h>
#include <XbFont.h>
#include <XbStopWatch.h>
#include <Xbhelp.h>
#include <XbResource.h>
#include <XbSound.h>

#pragma warning( push, 3 )  // Suppress VC warnings when compiling at W4
#include <string>
#include <vector>
#pragma warning( pop )




//-----------------------------------------------------------------------------
// Define VKEY_JAPAN to turn on support for Japanese virtual keyboard
//-----------------------------------------------------------------------------
#define VKEY_JAPAN                  1
#define FIXED_JAPANESE_SMALL_LETTER 1




//-----------------------------------------------------------------------------
// Name: class CXBVirtualKeyboard
// Desc: Application object for load and save game reference UI
//-----------------------------------------------------------------------------
class CXBVirtualKeyboard : public CXBApplication
{
    //-------------------------------------------------------------------------
    // Constants
    //-------------------------------------------------------------------------

    static const DWORD MAX_ROWS = 5;     // rows of keys
    static const DWORD KEY_WIDTH = 34;   // width of std key in pixels

    //-------------------------------------------------------------------------
    // Types
    //-------------------------------------------------------------------------

    typedef std::basic_string< WCHAR > String;
    typedef String::size_type          StringIndex;

    //-------------------------------------------------------------------------
    // Enums
    //-------------------------------------------------------------------------

    enum KeyboardTypes
    {
        TYPE_ALPHABET,
        TYPE_SYMBOLS,
        TYPE_ACCENTS,

        TYPE_MAX
    };

    enum State
    {
        STATE_STARTSCREEN,  // First screen
        STATE_MENU,         // Main menu
        STATE_KEYBOARD,     // Keyboard display
        STATE_HELP,         // Controller mappings

        STATE_MAX
    };

    enum ControllerSState
    {
        XKJ_START   = 1 << 0,
        XKJ_BACK    = 1 << 1,
        XKJ_A       = 1 << 2,
        XKJ_B       = 1 << 3,
        XKJ_X       = 1 << 4,
        XKJ_Y       = 1 << 5,
        XKJ_BLACK   = 1 << 6,
        XKJ_WHITE   = 1 << 7,
        XKJ_LEFTTR  = 1 << 8,
        XKJ_RIGHTTR = 1 << 9,

        XKJ_DUP     = 1 << 12,
        XKJ_DDOWN   = 1 << 13,
        XKJ_DLEFT   = 1 << 14,
        XKJ_DRIGHT  = 1 << 15,
        XKJ_UP      = 1 << 16,
        XKJ_DOWN    = 1 << 17,
        XKJ_LEFT    = 1 << 18,
        XKJ_RIGHT   = 1 << 19
    };

    enum Event
    {
        EV_NULL,            // No events
        EV_A_BUTTON,        // A button
        EV_START_BUTTON,    // Start button
        EV_B_BUTTON,        // B button
        EV_BACK_BUTTON,     // Back button
        EV_X_BUTTON,        // X button
        EV_Y_BUTTON,        // Y button
        EV_WHITE_BUTTON,    // White button
        EV_BLACK_BUTTON,    // Black button
        EV_LEFT_BUTTON,     // Left trigger
        EV_RIGHT_BUTTON,    // Right trigger
        EV_UP,              // Up Dpad or left joy
        EV_DOWN,            // Down Dpad or left joy
        EV_LEFT,            // Left Dpad or left joy
        EV_RIGHT,           // Right Dpad or left joy

        EVENT_MAX
    };

    enum Xkey
    {
        XK_NULL         = 0,

        XK_SPACE        = ' ',
        XK_LBRACK       = '[',
        XK_RBRACK       = ']',
        XK_LBRACE       = '{',
        XK_RBRACE       = '}',
        XK_LPAREN       = '(',
        XK_RPAREN       = ')',
        XK_FSLASH       = '/',
        XK_BSLASH       = '\\',
        XK_LT           = '<',
        XK_GT           = '>',
        XK_AT           = '@',
        XK_SEMI         = ';',
        XK_COLON        = ':',
        XK_QUOTE        = '\'',
        XK_DQUOTE       = '\"',
        XK_AMPER        = '&',
        XK_STAR         = '*',
        XK_QMARK        = '?',
        XK_COMMA        = ',',
        XK_PERIOD       = '.',
        XK_DASH         = '-',
        XK_UNDERS       = '_',
        XK_PLUS         = '+',
        XK_EQUAL        = '=',
        XK_DOLLAR       = '$',
        XK_PERCENT      = '%',
        XK_CARET        = '^',
        XK_TILDE        = '~',
        XK_APOS         = '`',
        XK_EXCL         = '!',
        XK_VERT         = '|',
        XK_NSIGN        = '#',

        // Numbers
        XK_0            = '0',
        XK_1,
        XK_2,
        XK_3,
        XK_4,
        XK_5,
        XK_6,
        XK_7,
        XK_8,
        XK_9,
        
        // Letters
        XK_A            = 'A',
        XK_B,
        XK_C,
        XK_D,
        XK_E,
        XK_F,
        XK_G,
        XK_H,
        XK_I,
        XK_J,
        XK_K,
        XK_L,
        XK_M,
        XK_N,
        XK_O,
        XK_P,
        XK_Q,
        XK_R,
        XK_S,
        XK_T,
        XK_U,
        XK_V,
        XK_W,
        XK_X,
        XK_Y,
        XK_Z,

        // Accented characters and other special characters

        XK_INVERTED_EXCL        = 0xA1, // ¡
        XK_CENT_SIGN            = 0xA2, // ¢
        XK_POUND_SIGN           = 0xA3, // £
        XK_YEN_SIGN             = 0xA5, // ¥
        XK_COPYRIGHT_SIGN       = 0xA9, // ©
        XK_LT_DBL_ANGLE_QUOTE   = 0xAB, // <<
        XK_REGISTERED_SIGN      = 0xAE, // ®
        XK_SUPERSCRIPT_TWO      = 0xB2, // ²
        XK_SUPERSCRIPT_THREE    = 0xB3, // ³
        XK_ACUTE_ACCENT         = 0xB4, // ´
        XK_MICRO_SIGN           = 0xB5, // µ
        XK_SUPERSCRIPT_ONE      = 0xB9, // ¹
        XK_RT_DBL_ANGLE_QUOTE   = 0xBB, // >>
        XK_INVERTED_QMARK       = 0xBF, // ¿
        XK_CAP_A_GRAVE          = 0xC0, // À
        XK_CAP_A_ACUTE          = 0xC1, // Á
        XK_CAP_A_CIRCUMFLEX     = 0xC2, // Â
        XK_CAP_A_TILDE          = 0xC3, // Ã
        XK_CAP_A_DIAERESIS      = 0xC4, // Ä
        XK_CAP_A_RING           = 0xC5, // Å
        XK_CAP_AE               = 0xC6, // Æ
        XK_CAP_C_CEDILLA        = 0xC7, // Ç
        XK_CAP_E_GRAVE          = 0xC8, // È
        XK_CAP_E_ACUTE          = 0xC9, // É
        XK_CAP_E_CIRCUMFLEX     = 0xCA, // Ê
        XK_CAP_E_DIAERESIS      = 0xCB, // Ë
        XK_CAP_I_GRAVE          = 0xCC, // Ì
        XK_CAP_I_ACUTE          = 0xCD, // Í
        XK_CAP_I_CIRCUMFLEX     = 0xCE, // Î
        XK_CAP_I_DIAERESIS      = 0xCF, // Ï
        XK_CAP_N_TILDE          = 0xD1, // Ñ
        XK_CAP_O_GRAVE          = 0xD2, // Ò
        XK_CAP_O_ACUTE          = 0xD3, // Ó
        XK_CAP_O_CIRCUMFLEX     = 0xD4, // Ô
        XK_CAP_O_TILDE          = 0xD5, // Õ
        XK_CAP_O_DIAERESIS      = 0xD6, // Ö
        XK_CAP_O_STROKE         = 0xD8, // Ø
        XK_CAP_U_GRAVE          = 0xD9, // Ù
        XK_CAP_U_ACUTE          = 0xDA, // Ú
        XK_CAP_U_CIRCUMFLEX     = 0xDB, // Û
        XK_CAP_U_DIAERESIS      = 0xDC, // Ü
        XK_CAP_Y_ACUTE          = 0xDD, // Ý
        XK_SM_SHARP_S           = 0xDF, // ß
        XK_SM_A_GRAVE           = 0xE0, // à
        XK_SM_A_ACUTE           = 0xE1, // á
        XK_SM_A_CIRCUMFLEX      = 0xE2, // â
        XK_SM_A_TILDE           = 0xE3, // ã
        XK_SM_A_DIAERESIS       = 0xE4, // ä
        XK_SM_A_RING            = 0xE5, // å
        XK_SM_AE                = 0xE6, // æ
        XK_SM_C_CEDILLA         = 0xE7, // ç
        XK_SM_E_GRAVE           = 0xE8, // è
        XK_SM_E_ACUTE           = 0xE9, // é
        XK_SM_E_CIRCUMFLEX      = 0xEA, // ê
        XK_SM_E_DIAERESIS       = 0xEB, // ë
        XK_SM_I_GRAVE           = 0xEC, // ì
        XK_SM_I_ACUTE           = 0xED, // í
        XK_SM_I_CIRCUMFLEX      = 0xEE, // î
        XK_SM_I_DIAERESIS       = 0xEF, // ï
        XK_SM_N_TILDE           = 0xF1, // ñ
        XK_SM_O_GRAVE           = 0xF2, // ò
        XK_SM_O_ACUTE           = 0xF3, // ó
        XK_SM_O_CIRCUMFLEX      = 0xF4, // ô
        XK_SM_O_TILDE           = 0xF5, // õ
        XK_SM_O_DIAERESIS       = 0xF6, // ö
        XK_SM_O_STROKE          = 0xF8, // ø
        XK_SM_U_GRAVE           = 0xF9, // ù
        XK_SM_U_ACUTE           = 0xFA, // ú
        XK_SM_U_CIRCUMFLEX      = 0xFB, // û
        XK_SM_U_DIAERESIS       = 0xFC, // ü
        XK_SM_Y_ACUTE           = 0xFD, // ý
        XK_SM_Y_DIAERESIS       = 0xFF, // ÿ

        // Unicode
        XK_CAP_Y_DIAERESIS = 0x0178, // Y umlaut
        XK_EURO_SIGN       = 0x20AC, // Euro symbol
        XK_ARROWLEFT       = 0x22B2, // left arrow
        XK_ARROWRIGHT      = 0x22B3, // right arrow

        // Japanese Unicode
        XK_HIRAGANA_SP  = 0x3040,
        XK_HIRAGANA_LA  = 0x3041,
        XK_HIRAGANA_A   = 0x3042,
        XK_HIRAGANA_LI  = 0x3043,
        XK_HIRAGANA_I   = 0x3044,
        XK_HIRAGANA_LU  = 0x3045,
        XK_HIRAGANA_U   = 0x3046,
        XK_HIRAGANA_LE  = 0x3047,
        XK_HIRAGANA_E   = 0x3048,
        XK_HIRAGANA_LO  = 0x3049,
        XK_HIRAGANA_O   = 0x304A,

        XK_HIRAGANA_KA  = 0x304B,
        XK_HIRAGANA_GA  = 0x304C,
        XK_HIRAGANA_KI  = 0x304D,
        XK_HIRAGANA_GI  = 0x304E,
        XK_HIRAGANA_KU  = 0x304F,
        XK_HIRAGANA_GU  = 0x3050,
        XK_HIRAGANA_KE  = 0x3051,
        XK_HIRAGANA_GE  = 0x3052,
        XK_HIRAGANA_KO  = 0x3053,
        XK_HIRAGANA_GO  = 0x3054,

        XK_HIRAGANA_SA  = 0x3055,
        XK_HIRAGANA_ZA  = 0x3056,
        XK_HIRAGANA_SI  = 0x3057,
        XK_HIRAGANA_ZI  = 0x3058,
        XK_HIRAGANA_SU  = 0x3059,
        XK_HIRAGANA_ZU  = 0x305A,
        XK_HIRAGANA_SE  = 0x305B,
        XK_HIRAGANA_ZE  = 0x305C,
        XK_HIRAGANA_SO  = 0x305D,
        XK_HIRAGANA_ZO  = 0x305E,

        XK_HIRAGANA_TA  = 0x305F,
        XK_HIRAGANA_DA  = 0x3060,
        XK_HIRAGANA_TI  = 0x3061,
        XK_HIRAGANA_DI  = 0x3062,
        XK_HIRAGANA_LTU = 0x3063,
        XK_HIRAGANA_TU  = 0x3064,
        XK_HIRAGANA_DU  = 0x3065,
        XK_HIRAGANA_TE  = 0x3066,
        XK_HIRAGANA_DE  = 0x3067,
        XK_HIRAGANA_TO  = 0x3068,
        XK_HIRAGANA_DO  = 0x3069,

        XK_HIRAGANA_NA  = 0x306A,
        XK_HIRAGANA_NI  = 0x306B,
        XK_HIRAGANA_NU  = 0x306C,
        XK_HIRAGANA_NE  = 0x306D,
        XK_HIRAGANA_NO  = 0x306E,

        XK_HIRAGANA_HA  = 0x306F,
        XK_HIRAGANA_BA  = 0x3070,
        XK_HIRAGANA_PA  = 0x3071,
        XK_HIRAGANA_HI  = 0x3072,
        XK_HIRAGANA_BI  = 0x3073,
        XK_HIRAGANA_PI  = 0x3074,
        XK_HIRAGANA_HU  = 0x3075,
        XK_HIRAGANA_BU  = 0x3076,
        XK_HIRAGANA_PU  = 0x3077,
        XK_HIRAGANA_HE  = 0x3078,
        XK_HIRAGANA_BE  = 0x3079,
        XK_HIRAGANA_PE  = 0x307A,
        XK_HIRAGANA_HO  = 0x307B,
        XK_HIRAGANA_BO  = 0x307C,
        XK_HIRAGANA_PO  = 0x307D,

        XK_HIRAGANA_MA  = 0x307E,
        XK_HIRAGANA_MI  = 0x307F,
        XK_HIRAGANA_MU  = 0x3080,
        XK_HIRAGANA_ME  = 0x3081,
        XK_HIRAGANA_MO  = 0x3082,

        XK_HIRAGANA_LYA = 0x3083,
        XK_HIRAGANA_YA  = 0x3084,
        XK_HIRAGANA_LYU = 0x3085,
        XK_HIRAGANA_YU  = 0x3086,
        XK_HIRAGANA_LYO = 0x3087,
        XK_HIRAGANA_YO  = 0x3088,

        XK_HIRAGANA_RA  = 0x3089,
        XK_HIRAGANA_RI  = 0x308A,
        XK_HIRAGANA_RU  = 0x308B,
        XK_HIRAGANA_RE  = 0x308C,
        XK_HIRAGANA_RO  = 0x308D,

        XK_HIRAGANA_LWA = 0x308E,
        XK_HIRAGANA_WA  = 0x308F,
        XK_HIRAGANA_WI  = 0x3090,
        XK_HIRAGANA_WE  = 0x3091,
        XK_HIRAGANA_WO  = 0x3092,
        XK_HIRAGANA_N   = 0x3093,
        XK_HIRAGANA_VU  = 0x3094,

        // Katakana
        XK_KATAKANA_SP  = 0x30A0,
        XK_KATAKANA_LA  = 0x30A1,
        XK_KATAKANA_A   = 0x30A2,
        XK_KATAKANA_LI  = 0x30A3,
        XK_KATAKANA_I   = 0x30A4,
        XK_KATAKANA_LU  = 0x30A5,
        XK_KATAKANA_U   = 0x30A6,
        XK_KATAKANA_LE  = 0x30A7,
        XK_KATAKANA_E   = 0x30A8,
        XK_KATAKANA_LO  = 0x30A9,
        XK_KATAKANA_O   = 0x30AA,

        XK_KATAKANA_KA  = 0x30AB,
        XK_KATAKANA_GA  = 0x30AC,
        XK_KATAKANA_KI  = 0x30AD,
        XK_KATAKANA_GI  = 0x30AE,
        XK_KATAKANA_KU  = 0x30AF,
        XK_KATAKANA_GU  = 0x30B0,
        XK_KATAKANA_KE  = 0x30B1,
        XK_KATAKANA_GE  = 0x30B2,
        XK_KATAKANA_KO  = 0x30B3,
        XK_KATAKANA_GO  = 0x30B4,

        XK_KATAKANA_SA  = 0x30B5,
        XK_KATAKANA_ZA  = 0x30B6,
        XK_KATAKANA_SI  = 0x30B7,
        XK_KATAKANA_ZI  = 0x30B8,
        XK_KATAKANA_SU  = 0x30B9,
        XK_KATAKANA_ZU  = 0x30BA,
        XK_KATAKANA_SE  = 0x30BB,
        XK_KATAKANA_ZE  = 0x30BC,
        XK_KATAKANA_SO  = 0x30BD,
        XK_KATAKANA_ZO  = 0x30BE,

        XK_KATAKANA_TA  = 0x30BF,
        XK_KATAKANA_DA  = 0x30C0,
        XK_KATAKANA_TI  = 0x30C1,
        XK_KATAKANA_DI  = 0x30C2,
        XK_KATAKANA_LTU = 0x30C3,
        XK_KATAKANA_TU  = 0x30C4,
        XK_KATAKANA_DU  = 0x30C5,
        XK_KATAKANA_TE  = 0x30C6,
        XK_KATAKANA_DE  = 0x30C7,
        XK_KATAKANA_TO  = 0x30C8,
        XK_KATAKANA_DO  = 0x30C9,

        XK_KATAKANA_NA  = 0x30CA,
        XK_KATAKANA_NI  = 0x30CB,
        XK_KATAKANA_NU  = 0x30CC,
        XK_KATAKANA_NE  = 0x30CD,
        XK_KATAKANA_NO  = 0x30CE,

        XK_KATAKANA_HA  = 0x30CF,
        XK_KATAKANA_BA  = 0x30D0,
        XK_KATAKANA_PA  = 0x30D1,
        XK_KATAKANA_HI  = 0x30D2,
        XK_KATAKANA_BI  = 0x30D3,
        XK_KATAKANA_PI  = 0x30D4,
        XK_KATAKANA_HU  = 0x30D5,
        XK_KATAKANA_BU  = 0x30D6,
        XK_KATAKANA_PU  = 0x30D7,
        XK_KATAKANA_HE  = 0x30D8,
        XK_KATAKANA_BE  = 0x30D9,
        XK_KATAKANA_PE  = 0x30DA,
        XK_KATAKANA_HO  = 0x30DB,
        XK_KATAKANA_BO  = 0x30DC,
        XK_KATAKANA_PO  = 0x30DD,

        XK_KATAKANA_MA  = 0x30DE,
        XK_KATAKANA_MI  = 0x30DF,
        XK_KATAKANA_MU  = 0x30E0,
        XK_KATAKANA_ME  = 0x30E1,
        XK_KATAKANA_MO  = 0x30E2,

        XK_KATAKANA_LYA = 0x30E3,
        XK_KATAKANA_YA  = 0x30E4,
        XK_KATAKANA_LYU = 0x30E5,
        XK_KATAKANA_YU  = 0x30E6,
        XK_KATAKANA_LYO = 0x30E7,
        XK_KATAKANA_YO  = 0x30E8,

        XK_KATAKANA_RA  = 0x30E9,
        XK_KATAKANA_RI  = 0x30EA,
        XK_KATAKANA_RU  = 0x30EB,
        XK_KATAKANA_RE  = 0x30EC,
        XK_KATAKANA_RO  = 0x30ED,

        XK_KATAKANA_LWA = 0x30EE,
        XK_KATAKANA_WA  = 0x30EF,
        XK_KATAKANA_WI  = 0x30F0,
        XK_KATAKANA_WE  = 0x30F1,
        XK_KATAKANA_WO  = 0x30F2,
        XK_KATAKANA_N   = 0x30F3,
        XK_KATAKANA_VU  = 0x30F4,
        XK_KATAKANA_DASH= 0x30FC,

        // Special
        XK_BACKSPACE = 0x10000, // backspace
        XK_DELETE,              // delete           // !!!
        XK_SHIFT,               // shift
        XK_CAPSLOCK,            // capslock
        XK_ALPHABET,            // alphabet
        XK_SYMBOLS,             // symbols
        XK_ACCENTS,             // accents
        XK_OK                   // "done"
    };

    //-------------------------------------------------------------------------
    // Classes
    //-------------------------------------------------------------------------

    // Keyboard key information
    struct Key
    {
        Xkey    xKey;       // virtual key code
        DWORD   dwWidth;    // width of the key
        String  strName;    // name of key when vKey >= 0x10000
        static UINT iLang;  // current language

        explicit Key( Xkey, DWORD = KEY_WIDTH );
    };

    //-------------------------------------------------------------------------
    // Types
    //-------------------------------------------------------------------------

    typedef std::vector< Key > KeyRow;              // list of keys in a row
    typedef KeyRow::size_type KeyIndex;

    typedef std::vector< KeyRow > Keyboard;         // list of rows = keyboard
    typedef Keyboard::size_type RowIndex;

    typedef std::vector< Keyboard > KeyboardList;   // alpha, symbol, etc.
    typedef KeyboardList::size_type BoardIndex;

    //-------------------------------------------------------------------------
    // Data
    //-------------------------------------------------------------------------

    CXBPackedResource  m_xprResource;      // General app packed resource
    CXBPackedResource  m_xprControllerS;   // ControllerS gamepad resource

    mutable CXBFont    m_FontLatin;        // Font renderer for most keys
    mutable CXBFont    m_FontJapan;        // Japanese font (Unicode 30xx - 30xx)
    mutable CXBFont    m_FontCaps;         // Font renderer for capital name keys
    mutable CXBFont    m_FontArrows;       // Arrow font (Unicode 8882 - 8883)
    mutable CXBFont    m_FontEuro;         // Euro font (Unicode 8364)
    mutable CXBFont    m_FontBtn;          // Xbox Button font

    BOOL               m_bIsCapsLockOn;    // CapsLock status
    BOOL               m_bIsShiftOn;       // Sticky shift key status
    State              m_State;            // Current state
    UINT               m_iLanguage;        // Current language
    String             m_strData;          // Current string
    StringIndex        m_iPos;             // Current pos of caret in string
    KeyboardList       m_KeyboardList;     // All potential keyboards
    BoardIndex         m_iCurrBoard;       // Current keyboard
    RowIndex           m_iCurrRow;         // Current row
    KeyIndex           m_iCurrKey;         // Current key in the current row           
    KeyIndex           m_iLastColumn;      // Column of last single char key
    CXBStopWatch       m_RepeatTimer;      // Controller button repeat timer
    FLOAT              m_fRepeatDelay;     // Time between button repeats
    CXBStopWatch       m_CaretTimer;       // Controls caret blink
    mutable CXBHelp    m_Help;             // Help
    LPDIRECT3DTEXTURE8 m_ptKey;            // Keyboard "key" texture
    LPDIRECT3DTEXTURE8 m_ptControllerS;    // ControllerS gamepad texture
    CXBSound           m_ClickSnd;         // Click sound

    // These items are used exclusively for the Japanese vkeyboard

    DWORD              m_dwCurrCtlrState;  // Current state of ControllerS
    DWORD              m_dwOldCtlrState;   // Previous state of ControllerS
    mutable Xkey       m_xNextKeyJpn;      // Japanese key press
    BOOL               m_bTrig;            // TRUE if trigger held
    BOOL               m_bKana;            // TRUE if Katakana; FALSE if Hiragana

public:

    CXBVirtualKeyboard();

    virtual HRESULT Initialize();
    virtual HRESULT FrameMove();
    virtual HRESULT Render();

private:

    VOID ValidateState() const;
    VOID InitBoard();

    Event GetEvent();
    DWORD GetEventJapan();
    Event GetControllerEvent();

    VOID UpdateState( Event );
    VOID UpdateStateJapan();
    VOID PressCurrent();
    VOID Press( Xkey );
    VOID MoveUp();
    VOID MoveDown();
    VOID MoveLeft();
    VOID MoveRight();
    VOID SetLastColumn();

    VOID DrawTextBox() const;
    VOID DrawButton( FLOAT fX, WCHAR chButton ) const;
    VOID RenderStartScreen() const;
    VOID RenderMenu() const;
    VOID RenderKeyboard() const;
    VOID RenderKeyboardLatin() const;
    VOID RenderKeyboardJapan() const;
    VOID RenderHelp() const;
    VOID RenderKey( FLOAT fX, FLOAT fY, const Key & pKey, D3DCOLOR selKeyColor,
                    D3DCOLOR selTextColor ) const;
    VOID DrawText( FLOAT x, FLOAT y ) const;
    VOID DrawTextLatin( FLOAT x, FLOAT y ) const;
    VOID DrawTextJapan( FLOAT x, FLOAT y ) const;

    VOID    DrawJapanese( FLOAT x, FLOAT y, DWORD, const WCHAR*, DWORD ) const;
    HRESULT DrawControllerS() const;

    BOOL IsKeyDisabled() const;
    VOID PlayClick() const;
    WCHAR GetChar( Xkey ) const;
    static WCHAR ToUpper( WCHAR );
    static WCHAR ToLower( WCHAR );

};

// Hiragana characters
#define JH_SP   L"\x3040"
#define JH_LA   L"\x3041"
#define JH_A    L"\x3042"
#define JH_LI   L"\x3043"
#define JH_I    L"\x3044"
#define JH_LU   L"\x3045"
#define JH_U    L"\x3046"
#define JH_LE   L"\x3047"
#define JH_E    L"\x3048"
#define JH_LO   L"\x3049"
#define JH_O    L"\x304A"
#define JH_KA   L"\x304B"
#define JH_GA   L"\x304C"
#define JH_KI   L"\x304D"
#define JH_GI   L"\x304E"
#define JH_KU   L"\x304F"
#define JH_GU   L"\x3050"
#define JH_KE   L"\x3051"
#define JH_GE   L"\x3052"
#define JH_KO   L"\x3053"
#define JH_GO   L"\x3054"
#define JH_SA   L"\x3055"
#define JH_ZA   L"\x3056"
#define JH_SI   L"\x3057"
#define JH_ZI   L"\x3058"
#define JH_SU   L"\x3059"
#define JH_ZU   L"\x305A"
#define JH_SE   L"\x305B"
#define JH_ZE   L"\x305C"
#define JH_SO   L"\x305D"
#define JH_ZO   L"\x305E"
#define JH_TA   L"\x305F"
#define JH_DA   L"\x3060"
#define JH_TI   L"\x3061"
#define JH_DI   L"\x3062"
#define JH_LTU  L"\x3063"
#define JH_TU   L"\x3064"
#define JH_DU   L"\x3065"
#define JH_TE   L"\x3066"
#define JH_DE   L"\x3067"
#define JH_TO   L"\x3068"
#define JH_DO   L"\x3069"
#define JH_NA   L"\x306A"
#define JH_NI   L"\x306B"
#define JH_NU   L"\x306C"
#define JH_NE   L"\x306D"
#define JH_NO   L"\x306E"
#define JH_HA   L"\x306F"
#define JH_BA   L"\x3070"
#define JH_PA   L"\x3071"
#define JH_HI   L"\x3072"
#define JH_BI   L"\x3073"
#define JH_PI   L"\x3074"
#define JH_HU   L"\x3075"
#define JH_BU   L"\x3076"
#define JH_PU   L"\x3077"
#define JH_HE   L"\x3078"
#define JH_BE   L"\x3079"
#define JH_PE   L"\x307A"
#define JH_HO   L"\x307B"
#define JH_BO   L"\x307C"
#define JH_PO   L"\x307D"
#define JH_MA   L"\x307E"
#define JH_MI   L"\x307F"
#define JH_MU   L"\x3080"
#define JH_ME   L"\x3081"
#define JH_MO   L"\x3082"
#define JH_LYA  L"\x3083"
#define JH_YA   L"\x3084"
#define JH_LYU  L"\x3085"
#define JH_YU   L"\x3086"
#define JH_LYO  L"\x3087"
#define JH_YO   L"\x3088"
#define JH_RA   L"\x3089"
#define JH_RI   L"\x308A"
#define JH_RU   L"\x308B"
#define JH_RE   L"\x308C"
#define JH_RO   L"\x308D"
#define JH_LWA  L"\x308E"
#define JH_WA   L"\x308F"
#define JH_WI   L"\x3090"
#define JH_WE   L"\x3091"
#define JH_WO   L"\x3092"
#define JH_N    L"\x3093"
#define JH_VU   L"\x3094"
#define JH_NULL L"\xFFFF"

// Katakana characters
#define JK_SP   L"\x30A0"
#define JK_LA   L"\x30A1"
#define JK_A    L"\x30A2"
#define JK_LI   L"\x30A3"
#define JK_I    L"\x30A4"
#define JK_LU   L"\x30A5"
#define JK_U    L"\x30A6"
#define JK_LE   L"\x30A7"
#define JK_E    L"\x30A8"
#define JK_LO   L"\x30A9"
#define JK_O    L"\x30AA"
#define JK_KA   L"\x30AB"
#define JK_GA   L"\x30AC"
#define JK_KI   L"\x30AD"
#define JK_GI   L"\x30AE"
#define JK_KU   L"\x30AF"
#define JK_GU   L"\x30B0"
#define JK_KE   L"\x30B1"
#define JK_GE   L"\x30B2"
#define JK_KO   L"\x30B3"
#define JK_GO   L"\x30B4"
#define JK_SA   L"\x30B5"
#define JK_ZA   L"\x30B6"
#define JK_SI   L"\x30B7"
#define JK_ZI   L"\x30B8"
#define JK_SU   L"\x30B9"
#define JK_ZU   L"\x30BA"
#define JK_SE   L"\x30BB"
#define JK_ZE   L"\x30BC"
#define JK_SO   L"\x30BD"
#define JK_ZO   L"\x30BE"
#define JK_TA   L"\x30BF"
#define JK_DA   L"\x30C0"
#define JK_TI   L"\x30C1"
#define JK_DI   L"\x30C2"
#define JK_LTU  L"\x30C3"
#define JK_TU   L"\x30C4"
#define JK_DU   L"\x30C5"
#define JK_TE   L"\x30C6"
#define JK_DE   L"\x30C7"
#define JK_TO   L"\x30C8"
#define JK_DO   L"\x30C9"
#define JK_NA   L"\x30CA"
#define JK_NI   L"\x30CB"
#define JK_NU   L"\x30CC"
#define JK_NE   L"\x30CD"
#define JK_NO   L"\x30CE"
#define JK_HA   L"\x30CF"
#define JK_BA   L"\x30D0"
#define JK_PA   L"\x30D1"
#define JK_HI   L"\x30D2"
#define JK_BI   L"\x30D3"
#define JK_PI   L"\x30D4"
#define JK_HU   L"\x30D5"
#define JK_BU   L"\x30D6"
#define JK_PU   L"\x30D7"
#define JK_HE   L"\x30D8"
#define JK_BE   L"\x30D9"
#define JK_PE   L"\x30DA"
#define JK_HO   L"\x30DB"
#define JK_BO   L"\x30DC"
#define JK_PO   L"\x30DD"
#define JK_MA   L"\x30DE"
#define JK_MI   L"\x30DF"
#define JK_MU   L"\x30E0"
#define JK_ME   L"\x30E1"
#define JK_MO   L"\x30E2"
#define JK_LYA  L"\x30E3"
#define JK_YA   L"\x30E4"
#define JK_LYU  L"\x30E5"
#define JK_YU   L"\x30E6"
#define JK_LYO  L"\x30E7"
#define JK_YO   L"\x30E8"
#define JK_RA   L"\x30E9"
#define JK_RI   L"\x30EA"
#define JK_RU   L"\x30EB"
#define JK_RE   L"\x30EC"
#define JK_RO   L"\x30ED"
#define JK_LWA  L"\x30EE"
#define JK_WA   L"\x30EF"
#define JK_WI   L"\x30F0"
#define JK_WE   L"\x30F1"
#define JK_WO   L"\x30F2"
#define JK_N    L"\x30F3"
#define JK_VU   L"\x30F4"
#define JK_DASH L"\x30FC"
#define JK_NULL L"\xFFFF"

#endif // XBVIRTUAL_KEYBOARD_H
