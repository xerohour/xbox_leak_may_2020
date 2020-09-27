#ifndef _CONSTANTS_H_
#define _CONSTANTS_H_

#include <xtl.h>

#define KEYPAD_NUM_ROWS    4
#define KEYPAD_NUM_COLS    4
#define DISPLAY_NUM_ROWS    4
#define DISPLAY_NUM_COLS    4

#define APP_TITLE_NAME_A                        "XShell"

// Screen Dimensions
#define SCREEN_WIDTH                            640
#define SCREEN_HEIGHT                           480
#define SCREEN_X_CENTER                         ( SCREEN_WIDTH / 2 )
#define SCREEN_Y_CENTER                         ( SCREEN_HEIGHT / 2 )

// Menubox constants
#define MENUBOX_MAINAREA_X1                     102.0f
#define MENUBOX_MAINAREA_Y1                     135.0f
#define MENUBOX_MAINAREA_X2                     536.0f
#define MENUBOX_MAINAREA_Y2                     367.0f

#define MENUBOX_TITLEBAR_TEXT_XPOS              127.0f

#define MENUBOX_SELECTBAR_X1                    MENUBOX_MAINAREA_X1 + 5.0f
#define MENUBOX_SELECTBAR_X2                    MENUBOX_MAINAREA_X2 - 5.0f

#define MENUBOX_TEXT_TOP_BORDER                   2.0f
#define MENUBOX_TEXT_BOTTOM_BORDER                0.0f
#define MENUBOX_TEXT_LSIDE_BORDER                 9.0f
#define MENUBOX_TEXT_RSIDE_BORDER                 4.0f

#define MENUBOX_TEXTAREA_X1                     ( MENUBOX_MAINAREA_X1 + MENUBOX_TEXT_LSIDE_BORDER )
#define MENUBOX_TEXTAREA_Y1                     ( MENUBOX_MAINAREA_Y1 + MENUBOX_TEXT_TOP_BORDER)
#define MENUBOX_TEXTAREA_X2                     ( MENUBOX_MAINAREA_X2 - MENUBOX_TEXT_RSIDE_BORDER )
#define MENUBOX_TEXTAREA_Y2                     ( MENUBOX_MAINAREA_Y2 - MENUBOX_TEXT_BOTTOM_BORDER )

// Display Settings Menu constants
#define DISPSETMENU_VIDEOHEADER_YPOS            149.0f      // Also in dispset.mnu file -- must be identical

// Behaviour constants
#define KEY_PRESS_INITIAL_DELAY                 400         // milliseconds
#define KEY_PRESS_REPEAT_DELAY                  50
#define EXECUTABLE_DIRECTORY_POLL_INTERVAL      5000        // 5 seconds
#define CLEAR_SCREEN_PAUSE_MS                   250         // milliseconds

// Font info
#define FONT_DEFAULT_HEIGHT                     20
#define FONT_DEFAULT_WIDTH                      8
#define FONT_DEFAULT_STYLE                      XFONT_NORMAL // XFONT_BOLD | XFONT_ITALICS | XFONT_BOLDITALICS
#define FONT_ALIAS_LEVEL                        4

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
#define COLOR_MEDIUM_GREEN                      0xFF57932E
#define COLOR_DARK_GREEN2                       0xFF357515
#define COLOR_DARK_GREEN                        0xFF2C531E

#define SCREEN_DEFAULT_BACKGROUND_COLOR         COLOR_BLACK
#define SCREEN_DEFAULT_TEXT_FG_COLOR            COLOR_LIGHT_YELLOW
#define SCREEN_DEFAULT_TEXT_BG_COLOR            SCREEN_DEFAULT_BACKGROUND_COLOR
#define SCREEN_DEFAULT_TRANS_COLOR              SCREEN_DEFAULT_BACKGROUND_COLOR

// Settings Menu contants - These should match the numbers in 'settings.mnu'
#define SETTINGSMENU_OPTIONHEADER_COLOR          COLOR_DARK_GREEN2 // COLOR_MEDIUM_GREEN
#define SETTINGSMENU_VIDSET_YPOS                 173.0f
#define SETTINGSMENU_VIDSET_TEXT                 L"Video :"
#define SETTINGSMENU_IPADDR_YPOS                 249.0f
#define SETTINGSMENU_IPADDR_TEXT                 L"IP address :"
#define SETTINGSMENU_SUBNET_YPOS                 277.0f
#define SETTINGSMENU_SUBNET_TEXT                 L"Subnet mask :"
#define SETTINGSMENU_GATEWY_YPOS                 305.0f
#define SETTINGSMENU_GATEWY_TEXT                 L"Gateway :"
#define SETTINGSMENU_MANAME_YPOS                 334.0f
#define SETTINGSMENU_MANAME_TEXT                 L"Machine name :"

static float g_fSettingsItemYPos[] = {
    SETTINGSMENU_VIDSET_YPOS,             // Item 0
    SETTINGSMENU_IPADDR_YPOS,             // Item 1
    SETTINGSMENU_SUBNET_YPOS,             // Item 2
    SETTINGSMENU_GATEWY_YPOS,             // Item 3
    SETTINGSMENU_MANAME_YPOS              // Item 4
};

// Launcher Menu Item Information
#define MENU_LAUNCH_MACHINENAME_X1              SCREEN_X_CENTER             // This is not used, it is calculated
#define MENU_LAUNCH_MACHINENAME_Y1              MENUBOX_MAINAREA_Y2 + 20.0f // ( SCREEN_HEIGHT - 65.0f )
#define MENU_LAUNCH_NUMXBES_Y1                  MENUBOX_MAINAREA_Y1 - 40.0f
#define MENU_LAUNCH_MACHINENAME_FONTHEIGHT      18
#define MENU_LAUNCH_MACHINENAME_COLOR           SCREEN_DEFAULT_TEXT_FG_COLOR
#define MENU_LAUNCH_TOP_BORDER_SIZE             10.0f
#define MENU_LAUNCH_UPARROW_X1                  114     // Bitmaps, use int
#define MENU_LAUNCH_UPARROW_Y1                  126     // Bitmaps, use int
#define MENU_LAUNCH_DOWNARROW_X1                114     // Bitmaps, use int
#define MENU_LAUNCH_DOWNARROW_Y1                361     // Bitmaps, use int

// Menu Item constants
#define ITEM_SELECTOR_BORDER_SPACE              2
#define ITEM_VERT_SPACING                       7.0f
#define ITEM_SELECTOR_COLOR                     COLOR_DARK_GREEN2
#define ITEM_SELECTED_HEADER_COLOR              COLOR_YELLOW
#define ITEM_SELECTED_VALUE_COLOR               SCREEN_DEFAULT_TEXT_FG_COLOR
#define ITEM_SELECTED_TEXT_COLOR                SCREEN_DEFAULT_TEXT_FG_COLOR
#define ITEM_XBELIST_FONT_HEIGHT                17
#define ITEM_FORMATMU_FONT_HEIGHT               17

// MU Format Dlg constants
#define MUFORMATDLG_SCREEN_DIM_VALUE            0x8F000000 // MMX way - 0x00707070
#define MUFORMATDLG_UNAVAIL_MU_COLOR            COLOR_DARK_GREEN2
#define MUFORMATDLG_AVAIL_MU_COLOR              SCREEN_DEFAULT_TEXT_FG_COLOR
#define MUFORMATDLG_SELECTED_MU_COLOR           ITEM_SELECTED_TEXT_COLOR
#define MUFORMATDLG_DELETE_MSG_YPOS             164.0f
#define MUFORMATDLG_DELETE_MSG                  L"All data will be erased from"
#define MUFORMATDLG_FORMATTING_MSG              L"Formatting..."
#define MUFORMATDLG_SUCCESS_MSG                 L"Format Successful!"
#define MUFORMATDLG_FAIL_MSG                    L"Format Failed!"
#define MUFORMATDLG_MSG_PAUSE                   1500

// Error Message Menu constants
#define ERRORMSGMENU_BUTTON1_TEXT               L"ok"
#define ERRORMSGMENU_MSG_FG_COLOR               SCREEN_DEFAULT_TEXT_FG_COLOR
#define ERRORMSGMENU_MSG_BG_COLOR               SCREEN_DEFAULT_TEXT_BG_COLOR
#define ERRORMSGMENU_MSG_YPOS                   189.0f

// Keypad and Keyboard location information
#define KEYBOARD_LINE_WIDTH                     1.0f
#define KEYPAD_LINE_WIDTH                       1.0f
#define KEYBOARD_TOPROW_Y1                      210
#define KEYPAD_TOPROW_Y1                        210
#define MENU_DISPSET_BUTTON_Y1                  186
#define MUFORMATDLG_BUTTON_Y1                   250
#define MUFORMATDLG_BUTTON_X1                   265
#define ERRORMSGMENU_BUTTON_Y1                  270
#define ERRORMSGMENU_BUTTON_X1                  265
#define KEYBOARD_MACHINENAMEHEADER_XPOS         153.0f                  // Must be identical to data in 'machine.mnu'
#define KEYBOARD_MACHINENAMEHEADER_YPOS         159.0f                  // Must be identical to data in 'machine.mnu'
#define KEYPAD_IPADDRHEADER_TEXT                L"IP address :"         // Must be identical to text in 'ip.mnu', 'gateway.mnu', and 'subnet.mnu'
#define KEYPAD_IPADDRHEADER_XPOS                191.0f                  // Must be identical to text in 'ip.mnu', 'gateway.mnu', and 'subnet.mnu'
#define KEYPAD_IPADDRHEADER_YPOS                159.0f                  // Must be identical to text in 'ip.mnu', 'gateway.mnu', and 'subnet.mnu'


// File path and location constants
#define FILE_EXECUTABLE_DEVICE_PATH_A           "\\Device\\Harddisk0\\Partition1"
#define FILE_EXECUTABLE_DRIVE_A                 "c:"
#define FILE_EXECUTABLE_PATH_A                  "\\devkit"
#define FILE_EXECUTABLE_DIRECTORY_A             FILE_EXECUTABLE_DRIVE_A FILE_EXECUTABLE_PATH_A
#define FILE_EXECUTABLE_ALIAS_A                 "e:"
#define FILE_EXECUTABLE_EXENSION_A              ".XBE"
#define FILE_DATA_DRIVE_A                       "t:"
#define FILE_DATA_DRIVE                         L"t:"
#define FILE_DATA_MEDIA_DIRECTORY               FILE_DATA_DRIVE L"\\media"
#define FILE_DATA_MEDIA_DIRECTORY_A             FILE_DATA_DRIVE_A "\\media"
#define FILE_DATA_MENU_DIRECTORY_A              FILE_DATA_DRIVE_A "\\menus"
#define FILE_DATA_IMAGE_DIRECTORY_A             FILE_DATA_DRIVE_A "\\images"
#define FILE_DATA_DEFAULT_FONT_FILENAME         FILE_DATA_MEDIA_DIRECTORY L"\\tahoma.ttf"

#define FILE_DATA_SETTINGS_FILENAME_A           "c:\\devkit\\xbdm.ini"
#define FILE_DATA_SETTINGS_SECTION_NAME_A       "[xbdm]"
#define FILE_DATA_SETTINGS_IP_TOKEN_A           "staticip addr="
#define FILE_DATA_SETTINGS_SUBNET_TOKEN_A       "subnetmask addr="
#define FILE_DATA_SETTINGS_GATEWAY_TOKEN_A      "defgateway addr="
#define FILE_DATA_SETTINGS_MACHINENAME_TOKEN_A  "dbgname name="
#define FILE_SPLASHSCREEN_LOCATION_A            FILE_DATA_IMAGE_DIRECTORY_A "\\loading.bmp"

// Data value limits
#define DATAVALUE_IPADDRESS_LENGTH              15
#define DATAVALUE_MACHINENAME_LENGTH            20
#define DATAVALUE_DISPLAYSETTING_TEXT_LENGTH    20

// Screen Saver
#define SCREEN_SAVER_TIMEOUT                    600000  // 10 minutes, 1200000 <-- 20 minutes
#define SCREEN_SAVER_UPDATE_INTERVAL            30000   // 30 seconds
#define SCREEN_SAVER_IMAGE                      FILE_DATA_IMAGE_DIRECTORY_A "\\abutton.bmp"
#define SCREEN_SAVER_BACK_COLOR                 COLOR_BLACK

// Controller and XBox Port and Slot Information
#define NUM_XBOX_PORTS                          4
#define NUM_XBOX_MU_SLOTS                       8
#define NUM_DUKE_BUTTONS                        8
#define NUM_DUKE_CONTROLS                       8
#define NUM_DUKE_SLOTS                          2

// Display Settings
enum XBoxDisplaySettings
{
    DISPLAY_SETTING_NONE        = 0,
    DISPLAY_SETTING_MONITOR     = 1,
    DISPLAY_SETTING_NTSC_M      = 2,
    DISPLAY_SETTING_NTSC_J,
    DISPLAY_SETTING_PAL_M       = 4,
    DISPLAY_SETTING_PAL_A,              // PAL B, D, G, H, I
    DISPLAY_SETTING_PAL_N,
    DISPLAY_SETTING_PAL_NC,
    DISPLAY_SETTING_NTSC_M_COMP = 10,
    DISPLAY_SETTING_PAL_M_COMP  = 12
};

// Joystick and Button Dead Zone and Threshold info
#define JOYSTICK_DEAD_ZONE                      20000       // 0 - 32768
#define BUTTON_THRESHOLD                        50          // 0 - 255

enum DisplaySettings
{
    ENUM_DISPSET_TEXT_MONITOR = 0,
    ENUM_DISPSET_TEXT_NTSC_SV,
    ENUM_DISPSET_TEXT_NTSC_C,
    ENUM_DISPSET_TEXT_PAL_SV,
    ENUM_DISPSET_TEXT_PAL_C    
};

// Memory Unit BitMasks used for detection
static DWORD g_dwMUBitMask[] = {
    XDEVICE_PORT0_TOP_MASK,
    XDEVICE_PORT0_BOTTOM_MASK,
    XDEVICE_PORT1_TOP_MASK,
    XDEVICE_PORT1_BOTTOM_MASK,
    XDEVICE_PORT2_TOP_MASK,
    XDEVICE_PORT2_BOTTOM_MASK,
    XDEVICE_PORT3_TOP_MASK,
    XDEVICE_PORT3_BOTTOM_MASK
};

static WCHAR* g_wpszMUPortNames[] = {
    L"Controller 1 - Top Slot",
    L"Controller 1 - Bottom Slot",
    L"Controller 2 - Top Slot",
    L"Controller 2 - Bottom Slot",
    L"Controller 3 - Top Slot",
    L"Controller 3 - Bottom Slot",
    L"Controller 4 - Top Slot",
    L"Controller 4 - Bottom Slot"
};

// Display Settings
static WCHAR* g_wpszDISPLAYSETTINGS[] = {
    L"Monitor",
    L"NTSC (S-Video)",
    L"NTSC (Composite)",
    L"PAL (S-Video)",
    L"PAL (Composite)"
};

// XShell Menu Identifiers
enum XShellMenuIds
{
    ENUM_SHELLMENUID_LAUNCH = 0,
    ENUM_SHELLMENUID_SETTINGS,
    ENUM_SHELLMENUID_MEMORY,
    ENUM_SHELLMENUID_SETTINGS_MACHINENAME,
    ENUM_SHELLMENUID_SETTINGS_GATEWAY,
    ENUM_SHELLMENUID_SETTINGS_IPADDRESS,
    ENUM_SHELLMENUID_SETTINGS_DISPLAY
};

// XShell States (bit mask)
#define SHELLSTATE_NORMAL       0
#define SHELLSTATE_ERROR        1
#define SHELLSTATE_NOCONFIG     2

// XShell Errors
// This list should stay in sync with the "g_wpszERRORCODEMSG" list
#define ERRORCODE_NO_ERROR          0
#define ERRORCODE_INVALID_XBE       1
#define ERRORCODE_UNKNOWN_ERROR     2

// This list should stay in sync with the "ERRORCODE" defines
static WCHAR* g_wpszERRORCODEMSG[] = {
    L"No Error Has Occurred",
    L"An Error Occurred Loading an XBox Executable",
    L"An Unknown Error Has Occurred"
};

// Screen item locations
#define ITEM_MACHINENAME_DATA_X                 ( ( ( MENUBOX_SELECTBAR_X2 - MENUBOX_SELECTBAR_X1 ) / 2.0f ) + MENUBOX_SELECTBAR_X1 - 17.0F )

#endif // _CONSTANTS_H_