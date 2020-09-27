#ifndef _CONSTANTS_H_
#define _CONSTANTS_H_

#include <xtl.h>
#include <av.h>

#define APP_TITLE_NAME_A                        "Recovery"

// Screen Dimensions
#define SCREEN_WIDTH                            640
#define SCREEN_HEIGHT                           480
#define SCREEN_X_CENTER                         ( SCREEN_WIDTH / 2 )
#define SCREEN_Y_CENTER                         ( SCREEN_HEIGHT / 2 )

// Progress Bar Dimensions
#define PROGRESS_WIDTH                           200.0f
#define PROGRESS_HEIGHT                           32.0f
#define PROGRESS_BORDER_OUTER_OFFSET             7.0f
#define PROGRESS_BORDER_GAP                      3.0f
#define PROGRESS_BORDER_WIDTH                    5
#define PROGRESS_X1                               (SCREEN_X_CENTER - (PROGRESS_WIDTH / 2))
#define PROGRESS_X2                               (SCREEN_X_CENTER + (PROGRESS_WIDTH / 2))
#define PROGRESS_Y1                               264.0f
#define PROGRESS_Y2                               (PROGRESS_Y1 + PROGRESS_HEIGHT)

// Launcher Menu Item Information
#define MENU_LAUNCH_MACHINENAME_X1              SCREEN_X_CENTER             // This is not used, it is calculated
#define MENU_LAUNCH_MACHINENAME_Y1              ( SCREEN_HEIGHT - 65.0f )
#define MENU_LAUNCH_NUMXBES_Y1                  MENUBOX_MAINAREA_Y1 - 28.0f
#define MENU_LAUNCH_MACHINENAME_FONTHEIGHT      28
#define MENU_LAUNCH_TOP_BORDER_SIZE             10.0f
#define MENU_LAUNCH_UPARROW_X1                  113     // Bitmaps, use int
#define MENU_LAUNCH_UPARROW_Y1                  106     // Bitmaps, use int
#define MENU_LAUNCH_DOWNARROW_X1                113     // Bitmaps, use int
#define MENU_LAUNCH_DOWNARROW_Y1                380     // Bitmaps, use int

// Menubox constants
#define MENUBOX_MAINAREA_X1                      99.0f
#define MENUBOX_MAINAREA_Y1                     134.0f
#define MENUBOX_MAINAREA_X2                     535.0f
#define MENUBOX_MAINAREA_Y2                     375.0f

#define MENUBOX_SELECTBAR_X1                    MENUBOX_MAINAREA_X1
#define MENUBOX_SELECTBAR_X2                    ( MENUBOX_MAINAREA_X2 + 1 )

#define MENUBOX_TEXT_TOP_BORDER                   16.0f
#define MENUBOX_TEXT_BOTTOM_BORDER                0.0f
#define MENUBOX_TEXT_LSIDE_BORDER                 8.0f
#define MENUBOX_TEXT_RSIDE_BORDER                 8.0f

#define MENUBOX_TEXTAREA_X1                     ( MENUBOX_MAINAREA_X1 + MENUBOX_TEXT_LSIDE_BORDER )
#define MENUBOX_TEXTAREA_Y1                     ( MENUBOX_MAINAREA_Y1 + MENUBOX_TEXT_TOP_BORDER )
#define MENUBOX_TEXTAREA_X2                     ( MENUBOX_MAINAREA_X2 - MENUBOX_TEXT_RSIDE_BORDER )
#define MENUBOX_TEXTAREA_Y2                     ( MENUBOX_MAINAREA_Y2 - MENUBOX_TEXT_BOTTOM_BORDER )

#define MENUBOX_BUILDNUM_Y1                     ( MENUBOX_TEXTAREA_Y2 - 22.0f )

// Behaviour constants
#define KEY_PRESS_INITIAL_DELAY                 400         // milliseconds
#define KEY_PRESS_REPEAT_DELAY                  50
#define EXECUTABLE_DIRECTORY_POLL_INTERVAL      5000        // 5 seconds
#define CLEAR_SCREEN_PAUSE_MS                   250         // milliseconds

// Font info
#define FONT_DEFAULT_HEIGHT                     20
#define FONT_DEFAULT_WIDTH                      8
#define FONT_ALIAS_LEVEL                        4
#define FONT_DEFAULT_STYLE                      XFONT_NORMAL // XFONT_BOLD | XFONT_ITALICS | XFONT_BOLDITALICS
#define FILE_DATA_DEFAULT_FONT_FILENAME         FILE_DATA_MEDIA_DIRECTORY L"\\tahoma.ttf"

// Screen Item Constants
#define SCREEN_ITEM_TEXT_LENGTH                 40

// Colors
#define COLOR_WHITE                             0xFFFFFFFF
#define COLOR_BLACK                             0xFF000000
#define COLOR_LIGHT_YELLOW                      0xFFDFF77F
#define COLOR_YELLOW                            0xFFFFFF00
#define COLOR_LIGHT_GRAY                        0xFFC8C8C8
#define COLOR_DARK_GRAY                         0xFF828282
#define COLOR_LIME_GREEN                        0xFF9BD351
#define COLOR_BRIGHT_GREEN                      0xFF73C034
#define COLOR_MEDIUM_GREEN                      0xFF57932E
#define COLOR_DARK_GREEN2                       0xFF357515
#define COLOR_DARK_GREEN                        0xFF2C531E

#define SCREEN_DEFAULT_BACKGROUND_COLOR         COLOR_BLACK
#define SCREEN_DEFAULT_TEXT_FG_COLOR            COLOR_LIGHT_YELLOW
#define SCREEN_DEFAULT_TEXT_BG_COLOR            SCREEN_DEFAULT_BACKGROUND_COLOR

// Menu Item constants
#define ITEM_SELECTOR_BORDER_SPACE              2
#define ITEM_VERT_SPACING                       7.0f
#define ITEM_SELECTOR_COLOR                     COLOR_LIME_GREEN
#define ITEM_XBELIST_FONT_HEIGHT                17 // 18

// Keypad and Keyboard location information
#define KEYBOARD_TOPROW_Y1                      225
#define KEYPAD_TOPROW_Y1                        225
#define MENU_DISPSET_BUTTON_Y1                  211

// File path and location constants
#define FILE_DATA_DRIVE                        L"d:"
#define FILE_DATA_DRIVE_A                       "d:"
#define FILE_DATA_MEDIA_DIRECTORY               FILE_DATA_DRIVE  L"\\recmedia"
#define FILE_DATA_MEDIA_DIRECTORY_A             FILE_DATA_DRIVE_A "\\recmedia"
#define FILE_DATA_IMAGE_DIRECTORY_A             FILE_DATA_DRIVE_A "\\recmedia"

#endif // _CONSTANTS_H_
