//------------------------------------------------------------------------------
// fontsandcolors.h
// Author: Josh Stevens [joshs]
//
// Fonts & Colors helper macros.  See common\idl\vs\vsshell.idl for related
// interfaces and enums.
//------------------------------------------------------------------------------


#pragma once


// Sometimes includers of this file define INIT_GUID and sometimes they don't,
// so rather than using the DEFINE_GUID macro and hoping it works, we'll just
// define the GUIDs by hand.

// {1F987C00-E7C4-4869-8A17-23FD602268B0}
EXTERN_C const GUID DECLSPEC_SELECTANY GUID_DialogsAndToolWindowsFC = { 0x1f987c00, 0xe7c4, 0x4869, { 0x8a, 0x17, 0x23, 0xfd, 0x60, 0x22, 0x68, 0xb0 } };

// {A27B4E24-A735-4d1d-B8E7-9716E1E3D8E0}
EXTERN_C const GUID DECLSPEC_SELECTANY GUID_TextEditorFC = { 0xa27b4e24, 0xa735, 0x4d1d, { 0xb8, 0xe7,  0x97,  0x16,  0xe1,  0xe3,  0xd8,  0xe0 } };

// {C34C709B-C855-459e-B38C-3021F162D3B1}
EXTERN_C const GUID DECLSPEC_SELECTANY GUID_TextOutputToolWindowsFC = { 0xc34c709b, 0xc855, 0x459e, { 0xb3, 0x8c, 0x30, 0x21, 0xf1, 0x62, 0xd3, 0xb1 } };

// {47724E70-AF55-48fb-A928-BB161C1D0C05}
EXTERN_C const GUID DECLSPEC_SELECTANY GUID_PrinterFC = { 0x47724e70, 0xaf55, 0x48fb, { 0xa9, 0x28, 0xbb, 0x16, 0x1c, 0x1d, 0xc, 0x5 } };


// Only font size between these two values (inclusive) are listed in the
// Fonts & Colors page.
#define FONT_SIZE_MIN 6
#define FONT_SIZE_MAX 24

#define INITFONTINFO(f) { ZeroMemory(&(f), sizeof(FontInfo)); (f).bFaceNameValid = (f).bPointSizeValid = (f).bCharSetValid = FALSE; }

#define DESTROYFONTINFO(f) \
    { \
        if ((f).bstrFaceName != NULL) ::SysFreeString((f).bstrFaceName); \
        INITFONTINFO(f); \
    }

#define COPYFONTINFO(a, b) \
    { \
        DESTROYFONTINFO(a); \
        memcpy(&(a), &(b), sizeof(FontInfo)); \
        if ((b).bstrFaceName != NULL) \
        { \
            (a).bstrFaceName = ::SysAllocString((b).bstrFaceName); \
        } \
        else \
        { \
            (a).bstrFaceName = NULL; \
        } \
    }


#define INITCOLORABLEITEMINFO(c) { ZeroMemory(&(c), sizeof(ColorableItemInfo)); (c).bForegroundValid = (c).bBackgroundValid = (c).bFontFlagsValid = FALSE; }
#define COPYCOLORABLEITEMINFO(a, b) { memcpy(&(a), &(b), sizeof(ColorableItemInfo)); }


#define INITALLCOLORABLEITEMINFO(a) \
    { \
        ZeroMemory(&(a), sizeof(AllColorableItemInfo)); \
        INITCOLORABLEITEMINFO((a).Info); \
        (a).bNameValid = \
        (a).bLocalizedNameValid = \
        (a).bAutoForegroundValid = \
        (a).bAutoBackgroundValid = \
        (a).bMarkerVisualStyleValid = \
        (a).bLineStyleValid = \
        (a).bFlagsValid = \
        (a).bDescriptionValid = FALSE; \
    }

#define DESTROYALLCOLORABLEITEMINFO(a) \
    { \
        if ((a).bstrName != NULL) ::SysFreeString((a).bstrName); \
        if ((a).bstrLocalizedName != NULL) ::SysFreeString((a).bstrLocalizedName); \
        if ((a).bstrDescription != NULL) ::SysFreeString((a).bstrDescription); \
        INITALLCOLORABLEITEMINFO(a); \
    }

#define COPYALLCOLORABLEITEMINFO(a, b) \
    {  \
        DESTROYALLCOLORABLEITEMINFO(a); \
        memcpy(&(a), &(b), sizeof(AllColorableItemInfo)); \
        COPYCOLORABLEITEMINFO((a).Info, (b).Info); \
        if ((b).bstrName != NULL) \
        { \
            (a).bstrName = ::SysAllocString((b).bstrName); \
        } \
        else \
        { \
            (a).bstrName = NULL; \
        } \
        if ((b).bstrLocalizedName != NULL) \
        { \
            (a).bstrLocalizedName = ::SysAllocString((b).bstrLocalizedName); \
        } \
        else \
        { \
            (a).bstrLocalizedName = NULL; \
        } \
        if ((b).bstrDescription != NULL) \
        { \
            (a).bstrDescription = ::SysAllocString((b).bstrDescription); \
        } \
        else \
        { \
            (a).bstrDescription = NULL; \
        } \
    }


// This constant means that bits 0-23 are an index into the following color table:
// 0  - generic text (foreground)
// 1  - generic text (background)
// 2  - black
// 3  - white
// 4  - maroon
// 5  - dark green
// 6  - brown
// 7  - dark blue
// 8  - purple
// 9  - aquamarine
// 10 - light gray
// 11 - dark gray
// 12 - red
// 13 - green
// 14 - yellow
// 15 - blue
// 16 - magenta
// 17 - cyan
// 18 - selected text (foreground) == ::GetSysColor(COLOR_HIGHLIGHTTEXT)
// 19 - selected text (background) == ::GetSysColor(COLOR_HIGHLIGHT)
// 20 - inactive selection (foreground) == ::GetSysColor(COLOR_INACTIVECAPTIONTEXT)
// 21 - inactive selection (background) == ::GetSysColor(COLOR_INACTIVECAPTION)
// 22 - indicator margin (background) == ::GetSysColor(COLOR_3DFACE)
// 23 - plain text (foreground) == ::GetSysColor(COLOR_WINDOWTEXT)
// 24 - plain text (background) == ::GetSysColor(COLOR_WINDOW)
// Entries 0-1 and 18-24 are tracking (i.e., dynamic)
#define _COLORREF_INDEXED (0x01000000)

// This indicates that bits 0-23 are a COLOR_* value, the raw RGB equivalent of which
// is returned from the GetSysColor() function.
#define _COLORREF_SYS (0x10000000)

// This indicates that bits 0-23 are a VSCOLOR_* value, the raw RGB equivalent of which
// is returned from IVsUIShell::GetVSSysColor().
#define _COLORREF_VS (0x20000000)

// These three constants define the range of "real" colors in our palette--that is,
// colors that correspond to raw RGB constants and don't "track" system color
// settings.
#define FIRST_REAL_COLOR (2)
#define LAST_REAL_COLOR  (17)
#define REAL_COLOR_COUNT (LAST_REAL_COLOR - FIRST_REAL_COLOR + 1)

// Call these to create COLORREF that represents the color specified by the given
// index.  The value returned is NOT a raw RGB value.
#define INDEXED_COLORREF(c)     (_COLORREF_INDEXED | (c))
#define INDEXED_SYS_COLORREF(c) (_COLORREF_SYS     | (c))
#define INDEXED_VS_COLORREF(c)  (_COLORREF_VS      | (c))

// Call this to extract the index from an indexed COLORREF (e.g., a value returned
// by INDEXED_COLORREF().  The value returned is NOT a raw RGB value, nor is it
// an index-coded COLORREF; it is simply an index.
#define GET_COLORREF_INDEX(c) ((c) & 0x00ffffff)

// Returns whether the specified value is an index-coded COLORREF.
#define IS_INDEXED_COLORREF(c) ((c) & (_COLORREF_INDEXED | _COLORREF_SYS | _COLORREF_VS))

// Returns whether the given COLORREF is an index-coded value that corresponds to
// one of the system color values (i.e., not a "real" color value).
#define IS_AUTO_COLORREF(c) (IS_INDEXED_COLORREF(c) && (GET_COLORREF_INDEX(c) < FIRST_REAL_COLOR || GET_COLORREF_INDEX(c) > LAST_REAL_COLOR))

// This constant means that the color tracks the 'automatic' color specified
// in the crAutoForeground or crAutoBackground member.
#define COLORREF_AUTO (0x02000000)

// This constant means that the color is invalid, or not set.
#define COLORREF_INVALID (0xff000000)

// This constant means that the color tracks the foreground color of another
// item in the list, specified by an index in bits 0-23.
#define COLORREF_TRACK_FG_BY_INDEX (0x04000000)

#define COLORREF_TRACK_BG_BY_INDEX (0x08000000)

#define PALETTE_SIZE (16 + 2 + 7)
#define ACTUAL_COLOR(Color, Auto) GetRGBValue(Color, Auto)

extern int ShellAutoToWin32Constant(int index);
extern COLORREF GetRGBValue(COLORREF crColor, COLORREF crAuto = COLORREF_INVALID, IServiceProvider *pSP = NULL, const GUID *pguidCategory = NULL);
extern void SetPaletteColor(int index, COLORREF color);
extern void SetUserTextColors(REFGUID rguidCategory, COLORREF crForeground, COLORREF crBackground);
extern COLORREF GetPaletteColor(int index);
extern BOOL GetUserTextColors(REFGUID rguidCategory, COLORREF *pcrForeground, COLORREF *pcrBackground);
extern COLORREF* GetColorPalette();
