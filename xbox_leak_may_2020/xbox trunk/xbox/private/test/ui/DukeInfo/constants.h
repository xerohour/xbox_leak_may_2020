#ifndef _CONSTANTS_H_
#define _CONSTANTS_H_

#define APP_TITLE_NAME_A                        "DukeInfo"

// Screen Dimensions
#define SCREEN_WIDTH                            640
#define SCREEN_HEIGHT                           480
#define SCREEN_X_CENTER                         ( SCREEN_WIDTH / 2 )
#define SCREEN_Y_CENTER                         ( SCREEN_HEIGHT / 2 )

// Launcher Menu Item Information
#define MENU_LAUNCH_MACHINENAME_X1              SCREEN_X_CENTER             // This is not used, it is calculated
#define MENU_LAUNCH_MACHINENAME_Y1              ( SCREEN_HEIGHT - 65.0f )
#define MENU_LAUNCH_NUMXBES_Y1                  MENUBOX_MAINAREA_Y1 - 28.0f
#define MENU_LAUNCH_MACHINENAME_FONTHEIGHT      28
#define MENU_LAUNCH_TOP_BORDER_SIZE             10.0f

// Menubox constants
#define MENUBOX_MAINAREA_X1                      99.0f
#define MENUBOX_MAINAREA_Y1                     134.0f
#define MENUBOX_MAINAREA_X2                     535.0f
#define MENUBOX_MAINAREA_Y2                     375.0f
#define MENUBOX_VALUE_X1                        225.0f
#define MENUBOX_RIGHT_COL_X1                    290.0f
#define MENUBOX_RIGHT_VALUE_X1                  455.0f

#define MENUBOX_SELECTBAR_X1                    MENUBOX_MAINAREA_X1
#define MENUBOX_SELECTBAR_X2                    ( MENUBOX_MAINAREA_X2 + 1 )

#define MENUBOX_TEXT_TOP_BORDER                   4.0f
#define MENUBOX_TEXT_BOTTOM_BORDER                0.0f
#define MENUBOX_TEXT_LSIDE_BORDER                 4.0f
#define MENUBOX_TEXT_RSIDE_BORDER                 4.0f

#define MENUBOX_TEXTAREA_X1                     ( MENUBOX_MAINAREA_X1 + MENUBOX_TEXT_LSIDE_BORDER )
#define MENUBOX_TEXTAREA_Y1                     ( MENUBOX_MAINAREA_Y1 + MENUBOX_TEXT_TOP_BORDER)
#define MENUBOX_TEXTAREA_X2                     ( MENUBOX_MAINAREA_X2 - MENUBOX_TEXT_RSIDE_BORDER )
#define MENUBOX_TEXTAREA_Y2                     ( MENUBOX_MAINAREA_Y2 - MENUBOX_TEXT_BOTTOM_BORDER )

// Behaviour constants
#define KEY_PRESS_DELAY                         400  // milliseconds

// Font info
#define FONT_DEFAULT_HEIGHT                     18
#define FONT_DEFAULT_WIDTH                      8
#define FONT_ALIAS_LEVEL                        4
#define FONT_DEFAULT_STYLE                      XFONT_NORMAL // XFONT_BOLD | XFONT_ITALICS | XFONT_BOLDITALICS

// Screen Item Constants
#define SCREEN_ITEM_TEXT_LENGTH                 128

// Colors
#define COLOR_WHITE                             0xFFFFFFFF
#define COLOR_BLACK                             0xFF000000
#define COLOR_LIGHT_GRAY                        0xFFC8C8C8
#define COLOR_LIME_GREEN                        0xFF9BD351
#define COLOR_DARK_GREEN                        0xFF2C531E

#define SCREEN_DEFAULT_BACKGROUND_COLOR         COLOR_BLACK
#define SCREEN_DEFAULT_TEXT_FG_COLOR            COLOR_DARK_GREEN
#define SCREEN_DEFAULT_TEXT_BG_COLOR            SCREEN_DEFAULT_BACKGROUND_COLOR

// Menu Item constants
#define ITEM_SELECTOR_BORDER_SPACE              2
#define ITEM_VERT_SPACING                       7.0f
#define ITEM_SELECTOR_COLOR                     COLOR_LIME_GREEN

// File path and location constants
#define FILE_DATA_DRIVE_W                       L"t:"
#define FILE_DATA_DRIVE_A                       "t:"
#define FILE_DATA_MEDIA_DIRECTORY_W             FILE_DATA_DRIVE_W L"\\media"
#define FILE_DATA_MEDIA_DIRECTORY_A             FILE_DATA_DRIVE_A "\\media"
#define FILE_DATA_MENU_DIRECTORY_A              FILE_DATA_DRIVE_A "\\menus"
#define FILE_DATA_IMAGE_DIRECTORY_A             FILE_DATA_DRIVE_A "\\images"
#define FILE_SPLASHSCREEN_LOCATION_A            FILE_DATA_IMAGE_DIRECTORY_A "\\loading.bmp"
#define FILE_DATA_DEFAULT_FONT_FILENAME         FILE_DATA_MEDIA_DIRECTORY_W L"\\tahoma.ttf"

// XBox Information
#define NUM_XBOX_PORTS                          4
#define NUM_XBOX_MU_SLOTS                       8
#define NUM_DUKE_BUTTONS                        8
#define NUM_DUKE_CONTROLS                       8
#define NUM_DUKE_SLOTS                          2

#endif // _CONSTANTS_H_