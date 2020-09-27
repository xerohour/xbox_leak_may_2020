#ifndef _CONSTANTS_H_
#define _CONSTANTS_H_

#define BREAK_INTO_DEBUGGER                     _asm { int 3 }
                                                
#define APP_TITLE_NAME_A                        "GREG3"
                                                
// Video Display Dimensions                     
#define SCREEN_WIDTH                            640
#define SCREEN_HEIGHT                           480
#define SCREEN_X_CENTER                         ( SCREEN_WIDTH / 2.0f )
#define SCREEN_Y_CENTER                         ( SCREEN_HEIGHT / 2.0f )
#define SCREEN_DEFAULT_FOREGROUND_COLOR         COLOR_TV_BLUE
#define SCREEN_DEFAULT_BACKGROUND_COLOR         COLOR_BLACK
#define SCREEN_DEFAULT_FONT_HEIGHT              32
#define SCREEN_DEFAULT_FONT_ALIAS_LEVEL         4
#define SCREEN_DEFAULT_FONT_STYLE               XFONT_NORMAL // XFONT_BOLD | XFONT_ITALICS | XFONT_BOLDITALICS

#define TEXT_GAME_REGION_INFO                   L"Game Region 3"

// Colors                                       
#define COLOR_WHITE                             0xFFFFFFFF
#define COLOR_BLACK                             0xFF000000
#define COLOR_RED                               0xFFFF0000
#define COLOR_TV_RED                            0xFF8D3232
#define COLOR_GREEN                             0xFF00FF00
#define COLOR_BLUE                              0xFF0000FF
#define COLOR_TV_LIGHT_BLUE                     0xFFA1A7FF
#define COLOR_TV_BLUE                           0xFF20209F
#define COLOR_LIGHT_YELLOW                      0xFFDDD078
#define COLOR_TV_YELLOW                         0xFFEFEF30
#define COLOR_DARK_GREEN                        0xFF357515

// File path and location constants
#define FILE_DATA_CACHE_SIZE                    5120000
#define FILE_DATA_DRIVE_A                       "t:"
#define FILE_DATA_DRIVE                         L"t:"
#define FILE_DATA_MEDIA_DIRECTORY               FILE_DATA_DRIVE  L"\\media"
#define FILE_DATA_MEDIA_DIRECTORY_A             FILE_DATA_DRIVE_A "\\media"
#define FILE_DATA_IMAGE_DIRECTORY_A             FILE_DATA_DRIVE_A "\\images"
#define FILE_DATA_DEFAULT_FONT_FILENAME         FILE_DATA_MEDIA_DIRECTORY L"\\tahoma.ttf"
#define FILE_DATA_DEFAULT_FONT_FILENAME_A       FILE_DATA_MEDIA_DIRECTORY_A "\\tahoma.ttf"
#define FILE_DATA_DEFAULT_STARTSOUND_FILENAME_A FILE_DATA_MEDIA_DIRECTORY_A "\\startsound.wav"
#define FILE_DATA_DEFAULT_BG_IMAGE_FILENAME     FILE_DATA_IMAGE_DIRECTORY_A "\\background.bmp"

// Vertex Definitions
struct MYVERTEX
{
	D3DXVECTOR3 v;
	float       fRHW;
	D3DCOLOR    cDiffuse;
};

struct MYTEXVERTEX
{
	D3DXVECTOR3 v;
	float       fRHW;
	D3DCOLOR    diffuse;
    float       tu, tv;    // The texture coordinates
};

#define D3DFVF_MYVERTEX     ( D3DFVF_XYZRHW | D3DFVF_DIFFUSE )
#define D3DFVF_MYTEXVERTEX  ( D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1 )

#endif // _CONSTANTS_H_
