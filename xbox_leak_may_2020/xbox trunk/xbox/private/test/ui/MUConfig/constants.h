#ifndef _CONSTANTS_H_
#define _CONSTANTS_H_

#define BREAK_INTO_DEBUGGER                     _asm { int 3 }
                                                
#define APP_TITLE_NAME_A                        "MUConfig"
                                                
// Video Display Dimensions                     
#define SCREEN_WIDTH                            640
#define SCREEN_HEIGHT                           480
#define SCREEN_X_CENTER                         ( SCREEN_WIDTH / 2.0f )
#define SCREEN_Y_CENTER                         ( SCREEN_HEIGHT / 2.0f )
#define SCREEN_DEFAULT_FOREGROUND_COLOR         COLOR_LIGHT_YELLOW
#define SCREEN_DEFAULT_BACKGROUND_COLOR         COLOR_BLACK
#define SCREEN_DEFAULT_FONT_HEIGHT              18
#define SCREEN_DEFAULT_FONT_ALIAS_LEVEL         4
#define SCREEN_DEFAULT_FONT_STYLE               XFONT_NORMAL // XFONT_BOLD | XFONT_ITALICS | XFONT_BOLDITALICS
#define SCREEN_DEFAULT_SELECTOR_COLOR           COLOR_DARK_GREEN
#define SCREEN_SELECTED_OPTION_COLOR            SCREEN_DEFAULT_FOREGROUND_COLOR
#define SCREEN_ACTIVE_OPTION_COLOR              SCREEN_DEFAULT_FOREGROUND_COLOR
#define SCREEN_INACTIVE_OPTION_COLOR            COLOR_DARK_GREEN
#define SCREEN_MOUNTED_OPTION_COLOR             COLOR_TV_LIGHT_BLUE
#define SCREEN_UNFORMATTED_OPTION_COLOR         COLOR_TV_YELLOW
                                                
// Menubox constants                            
#define MENUBOX_MAINAREA_X1                     102.0f
#define MENUBOX_MAINAREA_Y1                     135.0f
#define MENUBOX_MAINAREA_X2                     536.0f
#define MENUBOX_MAINAREA_Y2                     367.0f
#define MENUBOX_SELECTOR_X1                     MENUBOX_MAINAREA_X1
#define MENUBOX_SELECTOR_X2                     MENUBOX_MAINAREA_X2
#define MENUBOX_SELECTOR_BORDER_WIDTH           2.0f
#define MENUBOX_MAINAREA_BORDERWIDTH_TOP        2.0f
#define MENUBOX_MAINAREA_BORDERWIDTH_BOTTOM     2.0f
#define MENUBOX_MAINAREA_BORDERWIDTH_LEFT       10.0f
#define MENUBOX_MAINAREA_BORDERWIDTH_RIGHT      10.0f
#define MENUBOX_ITEM_VERT_SPACING               7.0f
#define MENUBOX_WINDOW_TITLE_X1                 130
#define MENUBOX_WINDOW_TITLE_Y1                 95
#define MENUBOX_WINDOW_FOOTER_X1                130
#define MENUBOX_WINDOW_FOOTER_Y1                387
                                                
// Input related items                          
#define INPUT_JOYSTICK_DEADZONE                 20000       // 0 - 32768
#define INPUT_BUTTON_THRESHOLD                  50          // 0 - 255
#define INPUT_KEYPRESS_INITIAL_DELAY            400         // milliseconds
#define INPUT_KEYPRESS_REPEAT_DELAY             50
#define REMOTE_BUTTON_REPEAT_THRESHOLD          200         // milliseconds
                                                
// Xbox related items                           
#define XBOX_CONSOLE_NUM_PORTS                  4
#define XBOX_CONTROLLER_NUM_BUTTONS             8
#define XBOX_CONTROLLER_NUM_CONTROLS            8
#define XBOX_CONTROLLER_NUM_SLOTS               2
#define XBOX_MU_NAME_LENGTH                     32
                                                
// Colors                                       
#define COLOR_WHITE                             0xFFFFFFFF
#define COLOR_BLACK                             0xFF000000
#define COLOR_RED                               0xFFFF0000
#define COLOR_TV_RED                            0xFF8D3232
#define COLOR_GREEN                             0xFF00FF00
#define COLOR_BLUE                              0xFF0000FF
#define COLOR_TV_LIGHT_BLUE                     0xFFA1A7FF
#define COLOR_LIGHT_YELLOW                      0xFFDDD078
#define COLOR_TV_YELLOW                         0xFFEFEF30
#define COLOR_DARK_GREEN                        0xFF357515
                                                
// Screen Item Constants                        
#define SCREEN_ITEM_TEXT_LENGTH                 150

// Data value limits
#define DATAVALUE_IPADDRESS_LENGTH              15
#define DATAVALUE_MACHINENAME_LENGTH            20
#define DATAVALUE_DISPLAYSETTING_TEXT_LENGTH    20
#define DATAVALUE_MUNAME_LENGTH                 20  // BUGBUG:Check with Chrispi on size
                                                
// File path and location constants
#define FILE_DATA_CDRIVE_PARTITION              "\\Device\\Harddisk0\\partition1"
#define FILE_DATA_YDRIVE_PARTITION              "\\Device\\Harddisk0\\partition2"
#define FILE_DATA_UDATA_PATH                    FILE_DATA_CDRIVE_PARTITION "\\udata"
#define FILE_DATA_CACHE_SIZE                    5120000
#define FILE_DATA_UNICODE_SIGNATURE             L'\xFEFF'
#define FILE_DATA_UDATA_DRIVE_LETTER_A          'X'
#define FILE_DATA_TDATA_DRIVE_LETTER_A          'V'
#define FILE_DATA_TDATA_DRIVE_LETTER            L't'
// #define FILE_DATA_DRIVE_A                       "t:"
// #define FILE_DATA_DRIVE                         L"t:"
#define FILE_DATA_DRIVE_A                       "d:"
#define FILE_DATA_DRIVE                         L"d:"
#define FILE_DATA_MEDIA_DIRECTORY               FILE_DATA_DRIVE  L"\\media"
#define FILE_DATA_MEDIA_DIRECTORY_A             FILE_DATA_DRIVE_A "\\media"
#define FILE_DATA_MENU_DIRECTORY_A              FILE_DATA_DRIVE_A "\\menus"
#define FILE_DATA_IMAGE_DIRECTORY_A             FILE_DATA_DRIVE_A "\\images"
#define FILE_DATA_DATA_DIRECTORY_A              FILE_DATA_DRIVE_A "\\data"
#define FILE_DATA_DEFAULT_FONT_FILENAME         FILE_DATA_MEDIA_DIRECTORY L"\\tahoma.ttf"
#define FILE_DATA_DEFAULT_FONT_FILENAME_A       FILE_DATA_MEDIA_DIRECTORY_A "\\tahoma.ttf"
#define FILE_SPLASHSCREEN_LOCATION_A            FILE_DATA_IMAGE_DIRECTORY_A "\\loading.bmp"
#define FILE_WMATRACK_LOCATION_A                FILE_DATA_DATA_DIRECTORY_A "\\%hs%u.wma"
#define FILE_TRACKINI_LOCATION_A                FILE_DATA_DATA_DIRECTORY_A "\\TrackInfo.ini"
// #define FILE_DATA_TITLE_IMAGE_FILENAME          "titleimage.xbx"
// #define FILE_DATA_SAVEGAME_IMAGE_FILENAME       "saveimage.xbx"
#define FILE_DATA_GAMEDATA_FILENAME             "game.xsv"
// #define FILE_DATA_TITLE_IMAGE_PATHINFO          FILE_DATA_DATA_DIRECTORY_A "\\" FILE_DATA_TITLE_IMAGE_FILENAME
// #define FILE_DATA_SAVEGAME_IMAGE_PATHINFO       FILE_DATA_DATA_DIRECTORY_A "\\" FILE_DATA_SAVEGAME_IMAGE_FILENAME
#define FILE_DATA_GAMEDATA_PATHINFO             FILE_DATA_DATA_DIRECTORY_A "\\" FILE_DATA_GAMEDATA_FILENAME
#define FILE_DATA_XDASH_TITLEID                 "fffe0000"
#define FILE_DATA_CONFIG_TITLEID                "00001123"
#define FILE_DATA_XDASH_XBE_LOCATION            NULL // "\\Device\\Harddisk0\\Partition1\\samples\\xdash\\xboxdash.xbe"

// Feedback information
#define CONTROLLER_RUMBLE_SPEED					65535
#define CONTROLLER_RUMBLE_MILLISECOND_TIME		250		// 1/4 of a second

// Menu specific information

// Root Menu
#define MENU_ROOT_MENU_FILENAME_A               FILE_DATA_MENU_DIRECTORY_A "\\root.mnu"

// Memory Menu
#define MENU_MEMORY_MENU_FILENAME_A             FILE_DATA_MENU_DIRECTORY_A "\\memory.mnu"

// HD Options Menu
#define MENU_HDOPTIONS_MENU_FILENAME_A          FILE_DATA_MENU_DIRECTORY_A "\\hdoptions.mnu"
#define MENU_HDOPTIONS_NUMBER_SIZE              MENU_MUOPTIONS_NUMBER_SIZE
#define MENU_HDOPTIONS_SAVEGAMENAME_SIZE        MENU_MUOPTIONS_SAVEGAMENAME_SIZE

// Entry Point Menu
#define MENU_XBCONFIG_MENU_FILENAME_A           FILE_DATA_MENU_DIRECTORY_A "\\xbconfig.mnu"

// Entry Point Menu
#define MENU_ENTRY_MENU_FILENAME_A              FILE_DATA_MENU_DIRECTORY_A "\\entry.mnu"
#define MENU_ENTRY_NUMBER_SIZE					11

// ST Menu
#define MENU_ST_MENU_FILENAME_A                 FILE_DATA_MENU_DIRECTORY_A "\\stmenu.mnu"
#define MENU_ST_NUMBER_SIZE                     10
#define MENU_ST_SOUNDTRACKNAME_SIZE             MAX_SOUNDTRACK_NAME
#define MENU_ST_TRACKNAME_SIZE                  MAX_SONG_NAME

// Game Region Menu
#define MENU_GAMEREGION_MENU_FILENAME_A         FILE_DATA_MENU_DIRECTORY_A "\\gamereg.mnu"
#define MENU_GAMEREGION_NUM_COLS                1
#define MENU_GAMEREGION_NUM_ROWS                GAMEREGIONMENU_ITEM_NUM_ITEMS
#define MENU_GAMEREGION_LINE_WIDTH              KEYPAD_LINE_WIDTH
#define MENU_GAMEREGION_BUTTON_X1               170
#define MENU_GAMEREGION_BUTTON_Y1               KEYPAD_TOPROW_Y1
#define MENU_GAMEREGION_BUTTON_WIDTH            300
#define MENU_GAMEREGION_CURRENT_REGION_TEXT     L"Current Xbox Game Region: "
#define MENU_GAMEREGION_TEXT_Y1                 135

// AV Region Menu
#define MENU_AVREGION_MENU_FILENAME_A           FILE_DATA_MENU_DIRECTORY_A "\\avreg.mnu"
#define MENU_AVREGION_NUM_COLS                  1
#define MENU_AVREGION_NUM_ROWS                  AVREGIONMENU_ITEM_NUM_ITEMS
#define MENU_AVREGION_LINE_WIDTH                KEYPAD_LINE_WIDTH
#define MENU_AVREGION_BUTTON_X1                 170
#define MENU_AVREGION_BUTTON_Y1                 KEYPAD_TOPROW_Y1
#define MENU_AVREGION_BUTTON_WIDTH              300
#define MENU_AVREGION_CURRENT_REGION_TEXT       L"Current Xbox AV Region: "
#define MENU_AVREGION_TEXT_Y1                   135

// DVD Region Menu
#define MENU_DVDREGION_MENU_FILENAME_A          FILE_DATA_MENU_DIRECTORY_A "\\dvdreg.mnu"
#define MENU_DVDREGION_NUM_COLS                 1
#define MENU_DVDREGION_NUM_ROWS                 DVDREGIONMENU_ITEM_NUM_ITEMS
#define MENU_DVDREGION_LINE_WIDTH               KEYPAD_LINE_WIDTH
#define MENU_DVDREGION_BUTTON_X1                170
#define MENU_DVDREGION_BUTTON_Y1                165
#define MENU_DVDREGION_BUTTON_WIDTH             300
#define MENU_DVDREGION_BUTTON_HEIGHT            27
#define MENU_DVDREGION_CURRENT_REGION_TEXT      L"Current Xbox DVD Region: "
#define MENU_DVDREGION_TEXT_Y1                  135


// Keypad Menu
#define MENU_KEYPAD_MENU_FILENAME_A             FILE_DATA_MENU_DIRECTORY_A "\\keypad.mnu"
#define KEYPAD_LINE_WIDTH                       1.0f
#define KEYPAD_TEXT_Y1                          150
#define KEYPAD_VALUE_Y1                         175
#define KEYPAD_TOPROW_Y1                        200
#define KEYPAD_TOPROW_X1                        210

#define KEY_PRESS_INITIAL_DELAY                 400         // milliseconds
#define KEY_PRESS_REPEAT_DELAY                  50

// Keyboard Menu
#define MENU_KEYBOARD_MENU_FILENAME_A           FILE_DATA_MENU_DIRECTORY_A "\\keybrd.mnu"
#define KEYBOARD_LINE_WIDTH                     1.0f
#define KEYBOARD_TEXT_Y1                        140
#define KEYBOARD_VALUE_Y1                       165
#define KEYBOARD_TOPROW_Y1                      200
#define KEYBOARD_TOPROW_X1                      110

// Memory Area Constants
#define MEMORY_AREA_BLOCK_SIZE_MAX_NUM_LOW_PART 262143
#define MEMORY_AREA_BLOCK_SIZE_IN_BYTES         16384

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

// XBOX Item names and enums

// USB Contant declarations for # of MU's and Controllers
static XDEVICE_PREALLOC_TYPE g_deviceTypes[] = 
{
    { XDEVICE_TYPE_GAMEPAD, 4 },
    { XDEVICE_TYPE_MEMORY_UNIT, 8 }
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

// Memory Unit names
static WCHAR* g_wpszMUPortNames[] = {
    L"MU 1A",
    L"MU 1B",
    L"MU 2A",
    L"MU 2B",
    L"MU 3A",
    L"MU 3B",
    L"MU 4A",
    L"MU 4B"
};


// Memory Menu Items
enum MemoryMenu
{
	MEMORYMENU_ITEM_MU1A,
    MEMORYMENU_ITEM_MU1B,
    MEMORYMENU_ITEM_MU2A,
    MEMORYMENU_ITEM_MU2B,
    MEMORYMENU_ITEM_MU3A,
    MEMORYMENU_ITEM_MU3B,
    MEMORYMENU_ITEM_MU4A,
    MEMORYMENU_ITEM_MU4B,
    MEMORYMENU_ITEM_XBOX_HARD_DRIVE,

    MEMORYMENU_ITEM_NUM_ITEMS
};

static WCHAR* g_wpszMemoryMenuItems[] = {
    g_wpszMUPortNames[0],
    g_wpszMUPortNames[1],
    g_wpszMUPortNames[2],
    g_wpszMUPortNames[3],
    g_wpszMUPortNames[4],
    g_wpszMUPortNames[5],
    g_wpszMUPortNames[6],
    g_wpszMUPortNames[7],
    L"Xbox Hard Drive"
};


// Root Menu Items
enum RootMenu
{
	ROOTMENU_ITEM_MEMORY,
    ROOTMENU_ITEM_SOUNDTRACKS,
    ROOTMENU_ITEM_XDASH_ENTRY_POINTS,
	ROOTMENU_ITEM_XBOX_CONFIGURATION,
    ROOTMENU_ITEM_INSTALL_MUCONFIG,
	
    ROOTMENU_ITEM_NUM_ITEMS
};

static WCHAR* g_wpszRootMenuItems[] = {
    L"Memory Areas",
    L"Soundtracks",
    L"XDash Entry Points",
	L"Xbox Configuration",
    L"Install MUConfig to Hard Disk"
};

// Save Game Menu Items
#define MENU_SAVEGAME_MENU_FILENAME_A               FILE_DATA_MENU_DIRECTORY_A "\\sgames.mnu" 

// Game Title Menu Items
#define MENU_GAMETITLE_MENU_FILENAME_A              FILE_DATA_MENU_DIRECTORY_A "\\gtitles.mnu"

// Xbox Configuration Menu Items
enum XBConfigMenu
{
    XBCONFIGMENU_ITEM_RESET_XBOX_TO_DEFAULTS,
    XBCONFIGMENU_ITEM_RESET_CLOCK,
    XBCONFIGMENU_ITEM_RESET_TIMEZONE,
    XBCONFIGMENU_ITEM_RESET_LANGUAGE,
	XBCONFIGMENU_ITEM_SET_GAME_REGION,
	XBCONFIGMENU_ITEM_SET_DVD_REGION,
    XBCONFIGMENU_ITEM_SET_AV_REGION,
    XBCONFIGMENU_ITEM_CHANGE_HDKEY,
    XBCONFIGMENU_ITEM_DISPLAY_RECKEY,
    XBCONFIGMENU_ITEM_RESET_HDKEY,
    XBCONFIGMENU_ITEM_CLEAR_REFERB,
    XBCONFIGMENU_ITEM_CORRUPT_CDRIVE,
    XBCONFIGMENU_ITEM_CORRUPT_YDRIVE,
    XBCONFIGMENU_ITEM_TOGGLE_DTS,
    XBCONFIGMENU_ITEM_TOGGLE_DOLBY,
    XBCONFIGMENU_ITEM_TOGGLE_1080I,
    XBCONFIGMENU_ITEM_TOGGLE_720P,
    XBCONFIGMENU_ITEM_TOGGLE_480P,
    XBCONFIGMENU_ITEM_TOGGLE_PAL60,
    XBCONFIGMENU_ITEM_TOGGLE_WIDESCREEN,
    XBCONFIGMENU_ITEM_TOGGLE_LETTERBOX,
    
    XBCONFIGMENU_ITEM_NUM_ITEMS
};

static WCHAR* g_wpszXBConfigMenuItems[] = {
    L"Reset the Xbox to its initial state",
    L"Reset the Xbox Clock",
    L"Reset the TimeZone",
    L"Reset the Language",
    L"Set the Xbox Game Region",
    L"Set the Xbox DVD Region",
    L"Set the Xbox AV Region",
    L"Change HD Key (randomly)",
    L"In-Dash Recovery Key",
    L"Reset HD Key to all O's",
    L"Clear Refurb Info",
    L"Corrupt the C Parition",
    L"Corrupt the Y Parition",
    L"Toggle DTS",
    L"Toggle Dolby Digital",
    L"Toggle 1080i",
    L"Toggle 720p",
    L"Toggle 480p",
    L"Toggle PAL60",
    L"Toggle Widescreen",
    L"Toggle Letterbox"
};

// DVD Region Menu Items
enum DVDRegionMenu
{
    DVDREGIONMENU_ITEM_REGION0,
    DVDREGIONMENU_ITEM_REGION1,
    DVDREGIONMENU_ITEM_REGION2,
    DVDREGIONMENU_ITEM_REGION3,
    DVDREGIONMENU_ITEM_REGION4,
    DVDREGIONMENU_ITEM_REGION5,
    DVDREGIONMENU_ITEM_REGION6,
    
    DVDREGIONMENU_ITEM_NUM_ITEMS
};

static WCHAR* g_wpszDVDRegionMenuItems[] = {
    L"Region 0 (not set)",
    L"Region 1 (North America)",
    L"Region 2 (Japan and Europe)",
    L"Region 3 (SE Asia)",
    L"Region 4 (Australia / SA)",
    L"Region 5 (NW Asia / N Africa)",
    L"Region 6 (China)"
};


// AV Region Menu Items
enum AVRegionMenu
{
    AVREGIONMENU_ITEM_REGION1,
    AVREGIONMENU_ITEM_REGION2,
    AVREGIONMENU_ITEM_REGION3,
    
    AVREGIONMENU_ITEM_NUM_ITEMS
};

static WCHAR* g_wpszAVRegionMenuItems[] = {
    L"Region 1 NTSC (North America)",
    L"Region 2 NTSC (Japan)",
    L"Region 3 PAL (Rest of World)"
};


// Game Region Menu Items
#define GAMEREGION_NONE 0

enum GameRegionMenu
{
    GAMEREGIONMENU_ITEM_REGION1,
    GAMEREGIONMENU_ITEM_REGION2,
    GAMEREGIONMENU_ITEM_REGION3,
    GAMEREGIONMENU_ITEM_NUM_ITEMS
};

static WCHAR* g_wpszGameRegionMenuItems[] = {
    L"Region 1 NTSC (North America)",
    L"Region 2 OTHER (Japan)",
    L"Region 3 PAL (Rest of World)"
};

// MU Options Menu Items
enum MUOptionsMenu
{
    MUOPTIONSMENU_ITEM_CREATE_TITLES_AND_SGAMES,
    MUOPTIONSMENU_ITEM_CREATE_SAVED_GAME,
    MUOPTIONSMENU_ITEM_SET_DATETIME_FOR_SAVED_GAME,
    MUOPTIONSMENU_ITEM_CREATE_FILE,
    MUOPTIONSMENU_ITEM_ERASE,
    MUOPTIONSMENU_ITEM_CREATEMU_A,
    MUOPTIONSMENU_ITEM_CREATEMU_B,
    MUOPTIONSMENU_ITEM_CREATEMU_C,
    MUOPTIONSMENU_ITEM_CREATEMU_D,
    MUOPTIONSMENU_ITEM_CREATEMU_E,
    MUOPTIONSMENU_ITEM_CREATEMU_F,
    MUOPTIONSMENU_ITEM_CREATEMU_G,
    MUOPTIONSMENU_ITEM_CREATEMU_H,
    MUOPTIONSMENU_ITEM_CREATEMU_I,
    MUOPTIONSMENU_ITEM_CREATEMU_J,
    MUOPTIONSMENU_ITEM_CREATEMU_K,
    MUOPTIONSMENU_ITEM_CREATEMU_L,
    MUOPTIONSMENU_ITEM_CREATEMU_M,
    MUOPTIONSMENU_ITEM_CREATEMU_N,
    MUOPTIONSMENU_ITEM_CREATEMU_O,
    MUOPTIONSMENU_ITEM_CREATEMU_P,
    MUOPTIONSMENU_ITEM_CREATEMU_Q,
    MUOPTIONSMENU_ITEM_CREATEMU_R,
    MUOPTIONSMENU_ITEM_CREATEMU_S,
    MUOPTIONSMENU_ITEM_CREATEMU_T,
    MUOPTIONSMENU_ITEM_CREATEMU_U,
    MUOPTIONSMENU_ITEM_CREATEMU_V,
    MUOPTIONSMENU_ITEM_CREATEMU_W,
    MUOPTIONSMENU_ITEM_CREATEMU_X,
    MUOPTIONSMENU_ITEM_CREATEMU_Y,
    MUOPTIONSMENU_ITEM_CREATEMU_Z,
    MUOPTIONSMENU_ITEM_CREATEMU_AA,
    MUOPTIONSMENU_ITEM_CREATEMU_BB,

    MUOPTIONSMENU_ITEM_NUM_ITEMS
};

static WCHAR* g_wpszMUOptionsMenuItems[] = {
    L"Create 'X' Game Titles w/ 'X' Saved Games",
    L"Create Single Saved Game",
    L"Set Date/Time for a Saved Game",
    L"Create File of Specified Size",
    L"Erase all files",
    L"Create MU - A (blank)",
    L"Create MU - B (M Name / 1 bl)",
    L"Create MU - C (1c Name / 1GT-0SG / 1/2 bl)",
    L"Create MU - D (M+1c Name / 1GT-1SG / MAX bl)",
    L"Create MU - E (LG Name / 2GT-2SG)",
    L"Create MU - F (Inv Name / Sorting)",
    L"Create MU - G (Emp Name / MAX GT-1SG)",
    L"Create MU - H (Emp Name / MAX GT-10SG)",
    L"Create MU - I (Emp Name / 1GT-4SG)",
    L"Create MU - J (Emp Name / 1GT-5SG)",
    L"Create MU - K (Emp Name / 1GT-6SG)",
    L"Create MU - L (1GT-MAX SG)",
    L"Create MU - M (1GT-MAX+1 SG)",
    L"Create MU - N (2GT-MAX SG)",
    L"Create MU - O (10GT-5SG)",
    L"Create MU - P (10GT-1st has 10SG)",
    L"Create MU - Q (10GT-1st has 15SG)",
    L"Create MU - R (10GT-1st has 20SG)",
    L"Create MU - S (10GT-1st has 25SG)",
    L"Create MU - T (10GT-1st has 30SG)",
    L"Create MU - U (10GT-1st has 35SG)",
    L"Create MU - V (10GT-1st has 40SG)",
    L"Create MU - W (10GT-1st has 45SG)",
    L"Create MU - X (10GT-1st has 50SG)",
    L"Create MU - Y (Meta Var 1 of 2)",
    L"Create MU - Z (Meta Var 2 of 2)",
    L"Create MU - AA (SG Many Files)",
	L"Create MU - BB (Jap Sorting)"
};

#define MENU_MUOPTIONS_MENU_FILENAME_A          FILE_DATA_MENU_DIRECTORY_A "\\muoptions.mnu"
#define MENU_MUOPTIONS_NUMBER_SIZE              10
#define MENU_MUOPTIONS_SAVEGAMENAME_SIZE        255

// HD Options Menu Items
enum HDOptionsMenu
{
	HDOPTIONSMENU_ITEM_UDATA_CREATE_TITLES,
    HDOPTIONSMENU_ITEM_UDATA_CREATE_TITLES_AND_SGAMES,
    HDOPTIONSMENU_ITEM_UDATA_CREATE_SAVED_GAME,
    HDOPTIONSMENU_ITEM_TDATA_ERASE,
    HDOPTIONSMENU_ITEM_UDATA_ERASE,
    HDOPTIONSMENU_ITEM_CREATE_FILE,
    HDOPTIONSMENU_ITEM_CONFIG_METAVARS,
    HDOPTIONSMENU_ITEM_CONFIG_SORTORDER,
	HDOPTIONSMENU_ITEM_CONFIG_SORTORDER_JAP,
    
    HDOPTIONSMENU_ITEM_NUM_ITEMS
};

static WCHAR* g_wpszHDOptionsMenuItems[] = {
    L"(UDATA) Create 'X' Game Titles",
    L"(UDATA) Create 'X' Game Titles w/ 'X' Saved Games",
    L"(UDATA) Create Single Saved Game",
    L"(TDATA) Erase all files",
    L"(UDATA) Erase all files",
    L"Create File of Specified Size",
    L"Configure for Saved Game Meta Variations",
    L"Configure for Saved Game Sorting Order (English)",
	L"Configure for Saved Game Sorting Order (Japanese)"
};


// DATE/TIME Menu Items
enum DateTimeMenu
{
	DATETIMEMENU_HOURS,
    DATETIMEMENU_MINUTES,
    DATETIMEMENU_SECONDS,
    DATETIMEMENU_MONTHS,
    DATETIMEMENU_DAYS,
    DATETIMEMENU_YEARS,

    DATETIMEMENU_NUM_ITEMS
};

#define DATETIMEMENU_FILENAME_A                 FILE_DATA_MENU_DIRECTORY_A "\\datetime.mnu"

// Soundtrack Menu Items
enum STMenu
{
    STMENU_ITEM_CREATE_SMALL_SOUNDTRACKS_AND_TRACKS,
	STMENU_ITEM_CREATE_SOUNDTRACKS_AND_TRACKS,
    STMENU_ITEM_CREATE_NAMED_SOUNDTRACK,
    STMENU_ITEM_ADD_NAMED_TRACK_TO_SOUNDTRACK,
    STMENU_ITEM_CREATE_SORTING,
	STMENU_ITEM_CREATE_SORTING_JAP,	
	STMENU_ITEM_REMOVE_ALL_SOUNDTRACKS,
    
    STMENU_ITEM_NUM_ITEMS
};

static WCHAR* g_wpszSTMenuItems[] = {
    L"Create 'X' Soundtracks w/ 'X' Tracks (small)",
    L"Create 'X' Soundtracks w/ 'X' Tracks",
    L"Create a named Soundtrack",
    L"Add a named Track to current Soundtrack",
	L"Create Soundtracks for Sorting (English)",
	L"Create Soundtracks for Sorting (Japanese)", 
    L"Remove all Soundtracks"
};

// Entry Point Menu Items
enum EntryMenu
{
	ENTRYMENU_ITEM_ROOT_MENU,
    ENTRYMENU_ITEM_MEMORY_CLEANUP,
    ENTRYMENU_ITEM_MUSIC,
    ENTRYMENU_ITEM_SETTINGS,
	ENTRYMENU_ITEM_SETTINGS_CLOCK,
	ENTRYMENU_ITEM_SETTINGS_TIMEZONE,
	ENTRYMENU_ITEM_SETTINGS_LANGUAGE,
	ENTRYMENU_ITEM_SETTINGS_CLOCK_TIMEZONE_LANGUAGE,
	ENTRYMENU_ITEM_SETTINGS_CLOCK_TIMEZONE,
	ENTRYMENU_ITEM_SETTINGS_CLOCK_LANGUAGE,
	ENTRYMENU_ITEM_SETTINGS_TIMEZONE_LANGUAGE,
    ENTRYMENU_ITEM_SETTINGS_COLLECTION,
    ENTRYMENU_ITEM_SETTINGS_VIDEO,
    ENTRYMENU_ITEM_SETTINGS_AUDIO,
    ENTRYMENU_ITEM_ERROR,
    ENTRYMENU_ITEM_ERROR_INVALID_XBE,
    ENTRYMENU_ITEM_ERROR_INVALID_HARD_DISK,
    ENTRYMENU_ITEM_ERROR_INVALID_XBE_REGION,
    ENTRYMENU_ITEM_ERROR_INVALID_XBE_PARENTAL_CONTROL,
    ENTRYMENU_ITEM_ERROR_INVALID_XBE_MEDIA_TYPE,
    
    ENTRYMENU_ITEM_NUM_ITEMS
};

static WCHAR* g_wpszEntryMenuItems[] = {
    L"XDash Root Menu",
    L"Memory Cleanup",
    L"Music",
    L"Settings",
	L"Settings - Clock",
	L"Settings - Timezone",
	L"Settings - Language",
	L"Settings - Clock,Timezone,Language",
	L"Settings - Clock,Timezone",
	L"Settings - Clock,Language",
	L"Settings - Timezone,Language",
	L"Settings - Collection",
    L"Settings - Video",
    L"Settings - Audio",
    L"Error Message",
    L"Error - Invalid XBE",
    L"Error - Invalid Hard Disk",
    L"Error - Invalid XBE Region",
    L"Error - Invalid XBE Parental Control",
    L"Error - Invalid XBE Media Type"
};

#define XSHELL_LAUNCH_CONTEXT_ROOT          1
#define XSHELL_LAUNCH_CONTEXT_MEMORY        2
#define XSHELL_LAUNCH_CONTEXT_ERROR         3
#define XSHELL_LAUNCH_CONTEXT_SETTINGS      4
#define XSHELL_LAUNCH_CONTEXT_MUSIC         5

#define ENTRYMENU_NUM_ITEMS_TO_DISPLAY      9

//
// META DATA CONTANTS
//
//#define META_FILE_METAXBX_FILENAME          "titlemeta.xbx"
//#define META_FILE_METAXBX_TITLENAME_TOKEN   L"TitleName="
//#define META_FILE_METAXBX_PUBNAME_TOKEN     L"PubName="
#define META_FILE_SAVEXBX_FILENAME          "savemeta.xbx"
//#define META_FILE_SAVEXBX_SAVENAME_TOKEN    L"Name="
/*

enum LangCodes
{
    ENUM_LANGUAGECODE_DEFAULT,
    ENUM_LANGUAGECODE_ENGLISH,
    ENUM_LANGUAGECODE_JAPANESE,
    ENUM_LANGUAGECODE_GERMAN,
    ENUM_LANGUAGECODE_ITALIAN,
    ENUM_LANGUAGECODE_FRENCH,
    ENUM_LANGUAGECODE_SPANISH,

    ENUM_LANGUAGECODE_NUM_ITEMS
};

static WCHAR* gwpszLanguageCodes[] = {
    L"[default]",
    L"[EN]",
    L"[JA]",
    L"[DE]",
    L"[IT]",
    L"[FR]",
    L"[ES]"
};
*/

#endif // _CONSTANTS_H_
