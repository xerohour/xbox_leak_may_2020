#ifndef _CONSTANTS_H_
#define _CONSTANTS_H_

#include <xtl.h>
#include <av.h>

#define APP_TITLE_NAME_A                        "XShell"

// Menu Titles
#define MENU_TITLE_ERRORMSG                     L"Error Message"
#define MENU_TITLE_DISPSET                      L"Settings : Video"
#define MENU_TITLE_IP                           L"Settings : IP Address"
#define MENU_TITLE_DNS                          L"Settings : DNS Address"
#define MENU_TITLE_SUBNET                       L"Settings : Subnet Mask"
#define MENU_TITLE_GATEWAY                      L"Settings : Gateway"
#define MENU_TITLE_MACHINE                      L"Settings : Machine Name"
#define MENU_TITLE_OPTIONS                      L"Options"
#define MENU_TITLE_ONLINE                       L"Online"
#define MENU_TITLE_GAMEREGION                   L"Options : Game Region"
#define MENU_TITLE_AVREGION                     L"Options : AV Region"
#define MENU_TITLE_VIDEORES                     L"Options : Video Resolution"
#define MENU_TITLE_NETWORK                      L"Options : Network Settings"
#define MENU_TITLE_CERT                         L"Options : Certification Tools"
#define MENU_TITLE_MEMORY                       L"Options : Memory Areas"
#define MENU_TITLE_MEMORYOPTIONS                L"Memory Areas : Options"
#define MENU_TITLE_MUFORMAT                     L"Memory Unit Format"
#define MENU_TITLE_MUUNFORMAT                   L"Memory Unit Unformat"
#define MENU_TITLE_MUNAME                       L"Memory Unit Name"
#define MENU_TITLE_MESSAGE                      L"Message"
#define MENU_TITLE_GENKEYBOARD                  L"Keyboard"
#define MENU_TITLE_HEXKEYPAD                    L"HEX Keypad"
#define MENU_TITLE_PASSCODE                     L"Passcode"
#define MENU_TITLE_YESNO                        L"Yes / No"
#define MENU_TITLE_DUMPCACHE                    L"Clear Cache Partition"
#define MENU_TITLE_LANGUAGE                     L"Language"
#define MENU_TITLE_TIMEZONES                    L"Time Zones"
#define MENU_TITLE_EEPROM                       L"EEPROM Settings"
#define MENU_TITLE_SYSINFO                      L"System Information"
#define MENU_TITLE_MADISPLAY                    L"Memory Area Display"
#define MENU_TITLE_OLUSER                       L"Xbox Hard Disk"
#define MENU_TITLE_OLUSEROPTION                 L"Online User Options"


// Texture Dimensions and Info
#define TEXTURE_MOVIESCREEN_WIDTH               512
#define TEXTURE_MOVIESCREEN_HEIGHT              512
#define TEXTURE_XDKTITLE_WIDTH                  512
#define TEXTURE_XDKTITLE_HEIGHT                 512
#define TEXTURE_XDKADDRESS_WIDTH                512
#define TEXTURE_XDKADDRESS_HEIGHT               512
#define TEXTURE_XDKADDRESS_PIXEL_WIDTH          230
#define TEXTURE_BUILDNO_WIDTH                   512
#define TEXTURE_BUILDNO_HEIGHT                  512

// Screen Dimensions
#define SCREEN_WIDTH                            640
#define SCREEN_HEIGHT                           480
#define SCREEN_HDTV_SAFE_PERCENTAGE             90.0f
#define SCREEN_NONHDTV_SAFE_PERCENTAGE          85.0f

// Menubox constants
#define MENUBOX_MAINAREA_X1                     0.0f   // 102.0f
#define MENUBOX_MAINAREA_Y1                     0.0f   // 135.0f
#define MENUBOX_MAINAREA_X2                     434.0f // 536.0f
#define MENUBOX_MAINAREA_Y2                     224.0f // 367.0f

#define MENUBOX_TITLE_TEXT_XPOS                 2.0f
#define MENUBOX_TITLE_TEXT_YPOS                 0.0f
#define MENUBOX_TITLE_TEXT_FONT_HEIGHT          17
#define MENUBOX_TITLE_TEXT_COLOR                COLOR_WHITE

#define MENUBOX_NAMEADDR_TEXT_XPOS              1.0f
#define MENUBOX_NAMEADDR_TEXT_YPOS              0.0f
#define MENUBOX_NAMEADDR_TEXT_HEIGHT            14
#define MENUBOX_NAMEADDR_TEXT_COLOR             COLOR_WHITE

#define MENUBOX_BUILDNO_TEXT_XPOS               0.0f
#define MENUBOX_BUILDNO_TEXT_YPOS               0.0f
#define MENUBOX_BUILDNO_TEXT_HEIGHT             15 // MENUBOX_NAMEADDR_TEXT_HEIGHT
#define MENUBOX_BUILDNO_TEXT_COLOR              COLOR_WHITE

#define MENUBOX_SELECTBAR_X1                    MENUBOX_MAINAREA_X1
#define MENUBOX_SELECTBAR_X2                    MENUBOX_MAINAREA_X2

#define MENUBOX_TEXT_TOP_BORDER                 7.0f // 10.0f
#define MENUBOX_TEXT_BOTTOM_BORDER              0.0f
#define MENUBOX_TEXT_LSIDE_BORDER               9.0f
#define MENUBOX_TEXT_RSIDE_BORDER               4.0f

#define MENUBOX_TEXTAREA_X1                     ( MENUBOX_MAINAREA_X1 + MENUBOX_TEXT_LSIDE_BORDER )
#define MENUBOX_TEXTAREA_Y1                     ( MENUBOX_MAINAREA_Y1 + MENUBOX_TEXT_TOP_BORDER )
#define MENUBOX_TEXTAREA_X2                     ( MENUBOX_MAINAREA_X2 - MENUBOX_TEXT_RSIDE_BORDER )
#define MENUBOX_TEXTAREA_Y2                     ( MENUBOX_MAINAREA_Y2 - MENUBOX_TEXT_BOTTOM_BORDER )
#define MENUBOX_TEXTAREA_X_CENTER               ( MENUBOX_MAINAREA_X2 / 2 )
#define MENUBOX_TEXTAREA_Y_CENTER               ( MENUBOX_MAINAREA_Y2 / 2 )

// Display Settings Menu constants
#define DISPSETMENU_VIDEOHEADER_YPOS             14.0f      // Also in dispset.mnu file -- must be identical

// Behaviour constants
#define KEY_PRESS_INITIAL_DELAY                 400         // milliseconds
#define KEY_PRESS_REPEAT_DELAY                  50
#define EXECUTABLE_DIRECTORY_POLL_INTERVAL      5000        // 5 seconds

// Font info
#define FONT_DEFAULT_HEIGHT                     20
#define FONT_DEFAULT_STYLE                      XFONT_NORMAL // XFONT_BOLD | XFONT_ITALICS | XFONT_BOLDITALICS
#define FONT_ALIAS_LEVEL                        0

// Screen Item Constants
#define SCREEN_ITEM_TEXT_LENGTH                 60

// Colors
#define COLOR_WHITE                             0xFFFFFFFF
#define COLOR_BLACK                             0xFF000000
#define COLOR_RED                               0xFFFF0000
#define COLOR_LIGHT_YELLOW                      0xFFDFF77F
#define COLOR_LIGHT_YELLOW2                     0xFFDDD078
#define COLOR_YELLOW                            0xFFFFFF00
// #define COLOR_TVSAFE_YELLOW                     0xFFEFEF30
#define COLOR_MEDIUM_GREEN                      0xFF57932E
#define COLOR_DARK_GREEN2                       0xFF357515

#define COLOR_TVSAFE_GREEN                      0xFF2CDF2C
#define COLOR_TVSAFE_RED                        0xFF8D3232
#define COLOR_TVSAFE_BLUE                       0xFF20209F
#define COLOR_TVSAFE_YELLOW                     0xFFF0F02F
#define COLOR_DARK_GREY                         0xFF808080
#define COLOR_DIM_GREY                          0xFF272727

#define COLOR_A_BUTTON                          COLOR_TVSAFE_GREEN
#define COLOR_B_BUTTON                          COLOR_TVSAFE_RED
#define COLOR_X_BUTTON                          COLOR_TVSAFE_BLUE
#define COLOR_Y_BUTTON                          COLOR_TVSAFE_YELLOW
#define COLOR_L_BUTTON                          COLOR_DARK_GREY
#define COLOR_R_BUTTON                          COLOR_DARK_GREY
#define COLOR_WHITE_BUTTON                      COLOR_WHITE
#define COLOR_BLACK_BUTTON                      COLOR_DARK_GREY // COLOR_DIM_GREY
#define COLOR_START_BUTTON                      COLOR_DARK_GREY
#define COLOR_BACK_BUTTON                       COLOR_DARK_GREY
#define COLOR_DPAD_UP_BUTTON                    COLOR_DARK_GREY
#define COLOR_DPAD_DOWN_BUTTON                  COLOR_DARK_GREY
#define COLOR_DPAD_LEFT_BUTTON                  COLOR_DARK_GREY
#define COLOR_DPAD_RIGHT_BUTTON                 COLOR_DARK_GREY


#define SCREEN_DEFAULT_BACKGROUND_COLOR         COLOR_BLACK
#define SCREEN_DEFAULT_TEXT_FG_COLOR            COLOR_LIGHT_YELLOW2
#define SCREEN_DEFAULT_TEXT_BG_COLOR            SCREEN_DEFAULT_BACKGROUND_COLOR

#define IPMENU_TITLE_IP_HEADER_TEXT             L"Title IP Address : "
#define IPMENU_DEBUG_IP_HEADER_TEXT             L"Debug IP Address : "

#define SUBNETMENU_HEADER_TEXT                  L"Subnet mask : "

#define GATEWAYMENU_HEADER_TEXT                 L"Gateway : "

// Launcher Menu Item Information
#define MENU_LAUNCH_TOP_BORDER_SIZE             0.0f // 10.0f

// Machine Name Menu Constants
#define MENU_MACHINENAME_INUSE_MESSAGE          L"That machine name is already in use..."
#define MENU_MACHINENAME_INUSE_YPOS             48.0f

// Machine Name Menu Constants
#define MENU_MACHINENAME_INUSE_MESSAGE          L"That machine name is already in use..."
#define MENU_MACHINENAME_INUSE_YPOS             48.0f

// IP Address Menu Constants
#define IPMENU_ADDRESS_INUSE_MESSAGE            L"That IP Address is already in use..."
#define IPMENU_ADDRESS_HEADER_TEXT_YPOS         24.0f
#define IPMENU_ADDRESS_INUSE_YPOS               48.0f

// DNS Address Menu Constants
#define DNSMENU_ADDRESS_HEADER_TEXT_YPOS        24.0f
#define DNSMENU_PRIMARY_DNS_HEADER_TEXT         L"Primary DNS : "
#define DNSMENU_SECONDARY_DNS_HEADER_TEXT       L"Secondary DNS : "

// Menu Item constants
#define ITEM_SELECTOR_BORDER_SPACE              2
#define ITEM_VERT_SPACING                       7.0f
#define ITEM_SELECTOR_COLOR                     COLOR_DARK_GREEN2
#define ITEM_HEADER_COLOR                       COLOR_DARK_GREEN2
#define ITEM_SELECTED_HEADER_COLOR              COLOR_YELLOW
#define ITEM_SELECTED_VALUE_COLOR               SCREEN_DEFAULT_TEXT_FG_COLOR
#define ITEM_SELECTED_TEXT_COLOR                SCREEN_DEFAULT_TEXT_FG_COLOR
#define ITEM_XBELIST_FONT_HEIGHT                17
#define ITEM_FORMATMU_FONT_HEIGHT               17
#define ITEM_OPTIONMENU_FONT_HEIGHT             17
#define ITEM_ONLINEMENU_FONT_HEIGHT             17
#define ITEM_LANGUAGEMENU_FONT_HEIGHT           17
#define ITEM_DUMPCACHEMENU_FONT_HEIGHT          17
#define ITEM_MADISPLAYMENU_FONT_HEIGHT          17
#define ITEM_OLUSERMENU_FONT_HEIGHT             17

// Memory Options Menu Screen constants
#define MENU_MEMORYOPTIONS_BUTTON_Y1            115
#define MENU_MEMORYOPTIONS_BUTTON_X1            100
#define MENU_MEMORYOPTIONS_OPTION1_TEXT         L"format"
#define MENU_MEMORYOPTIONS_OPTION2_TEXT         L"unformat"
#define MENU_MEMORYOPTIONS_OPTION3_TEXT         L"name"
#define MENU_MEMORYOPTIONS_OPTION4_TEXT         L"create file"
#define MENU_MEMORYOPTIONS_ACTION_TEXT          L"What would you like to do with"
#define MENU_MEMORYOPTOINS_ACTION_TEXT_YPOS     29.0f

// MU Name Dlg constants
#define MUNAMEDLG_MU_TEXT_YPOS                  14.0f
#define MUNAMEDLG_HEADER_YPOS                   42.0f
#define MUNAMEDLG_VALUE_XPOS                    ( MENUBOX_TEXTAREA_X_CENTER - 17.0F )

// MU Unformat Dlg constants
#define MUUNFORMATDLG_DELETE_MSG_YPOS           29.0f
#define MUUNFORMATDLG_DELETE_MSG                L"All data will be erased from"
#define MUUNFORMATDLG_FORMATTING_MSG            L"Unformatting..."
#define MUUNFORMATDLG_SUCCESS_MSG               L"Unformat Successful!"
#define MUUNFORMATDLG_FAIL_MSG                  L"Unformat Failed!"
#define MUUNFORMATDLG_MSG_PAUSE                 1500

// MU Format Dlg constants
#define MUFORMATDLG_UNAVAIL_MU_COLOR            COLOR_DARK_GREEN2
#define MUFORMATDLG_TOP_BORDER_SIZE             10.0f
#define MUFORMATDLG_AVAIL_MU_COLOR              SCREEN_DEFAULT_TEXT_FG_COLOR
#define MUFORMATDLG_SELECTED_MU_COLOR           ITEM_SELECTED_TEXT_COLOR
#define MUFORMATDLG_UNFORMATTED_MU_COLOR        COLOR_TVSAFE_YELLOW
#define MUFORMATDLG_DELETE_MSG_YPOS             29.0f
#define MUFORMATDLG_DELETE_MSG                  L"All data will be erased from"
#define MUFORMATDLG_FORMATTING_MSG              L"Formatting..."
#define MUFORMATDLG_SUCCESS_MSG                 L"Format Successful!"
#define MUFORMATDLG_FAIL_MSG                    L"Format Failed!"
#define MUFORMATDLG_MSG_PAUSE                   1500

// Error Message Menu constants
#define ERRORMSGMENU_BUTTON1_TEXT               L"ok"
#define ERRORMSGMENU_MSG_FG_COLOR               SCREEN_DEFAULT_TEXT_FG_COLOR
#define ERRORMSGMENU_MSG_BG_COLOR               SCREEN_DEFAULT_TEXT_BG_COLOR
#define ERRORMSGMENU_MSG_YPOS                    54.0f

//////////////////////////////////////////////
// Video Resolution Menu Items and Information
//////////////////////////////////////////////
#define VIDEORESMENU_FILENAME_A                 FILE_DATA_MENU_DIRECTORY_A "\\videores.mnu"


/////////////////////////////////////////
// Options Menu Items and Information
/////////////////////////////////////////
enum OptionsValues
{
    ENUM_OPTIONS_SET_GAMEREGION,
    ENUM_OPTIONS_SET_AVREGION,
    ENUM_OPTIONS_NETWORK,
    ENUM_OPTIONS_SET_VIDEORES,
    ENUM_OPTIONS_MEMORY,
    ENUM_OPTIONS_CERT_TOOLS,

    ENUM_OPTIONS_MAX_MENUITEMS
};

static WCHAR* g_pwszOptionsMenuItems[] = {
    L"Set the Xbox Game Region",
    L"Set the Xbox AV (Video) Region",
    L"Set the Xbox Network Settings",
    L"Set the XDK Launcher Video Resolution",
    L"Configure Memory Areas",
    L"Certification Tools"
};


/////////////////////////////////////////
// Online Menu Items and Information
/////////////////////////////////////////
enum OnlineValues
{
    ENUM_ONLINE_PING,
    ENUM_ONLINE_CREATEUSER,
    ENUM_ONLINE_MANAGEUSERS,

    ENUM_ONLINE_MAX_MENUITEMS
};

static WCHAR* g_pwszOnlineMenuItems[] = {
    L"Ping",
    L"Create a User Account",
    L"Manage Existing User Accounts"
};

#define ONLINE_PING_IN_PROGRESS_MSG             L"Ping in Progress... Please Wait"
#define ONLINE_PING_SUCCESS_MSG                 L"Successful"
#define ONLINE_PING_FAILED_MSG                  L"Failed!"
#define ONLINE_CREATE_USER_IN_PROGRESS_MSG      L"Creating User... Please Wait"
#define ONLINE_CREATE_USER_SUCCESS_MSG          L"Successful"
#define ONLINE_CREATE_USER_FAILED_MSG           L"Failed!"

////////////////////////////////////////////
// Memory Area Display Items and Information
////////////////////////////////////////////
#define MADISPLAYMENU_FILENAME_A                FILE_DATA_MENU_DIRECTORY_A "\\madisplay.mnu"
#define MADISPLAYMENU_TOP_BORDER_SIZE           0.0f
#define MADISPLAYMENU_UNFORMATTED_MU_COLOR      COLOR_TVSAFE_YELLOW
#define MADISPLAYMENU_AVAIL_MU_COLOR            SCREEN_DEFAULT_TEXT_FG_COLOR

///////////////////////////////////////////
// Certification Menu Items and Information
///////////////////////////////////////////
enum CertMenu
{
    CERTMENU_ITEM_REQUIREMENT10_1_11,
    CERTMENU_ITEM_REQUIREMENT10_1_12,
    CERTMENU_ITEM_REQUIREMENT10_2_01,
    CERTMENU_ITEM_REQUIREMENT10_2_02,
    CERTMENU_ITEM_REQUIREMENT10_2_07,
    CERTMENU_ITEM_REQUIREMENT10_2_10,
    CERTMENU_ITEM_REQUIREMENT10_2_18,
    CERTMENU_ITEM_REQUIREMENT10_2_19,
    CERTMENU_ITEM_REQUIREMENT10_3_04,
    CERTMENU_ITEM_REQUIREMENT10_3_11,
    CERTMENU_ITEM_REQUIREMENT10_3_14,
    CERTMENU_ITEM_REQUIREMENT10_3_15,
    CERTMENU_ITEM_REQUIREMENT10_3_26_1,
    CERTMENU_ITEM_REQUIREMENT10_3_26_2,
    CERTMENU_ITEM_REQUIREMENT10_3_26_3,
    CERTMENU_ITEM_REQUIREMENT10_3_26_4,
    CERTMENU_ITEM_REQUIREMENT10_3_33,
    CERTMENU_ITEM_REQUIREMENT10_6_05,
    CERTMENU_ITEM_CLEANUP,
    
    CERTMENU_ITEM_NUM_ITEMS
};

static WCHAR* g_pwszCertMenuItems[] = {
    L"1-11 Remove system files: Soundtracks & Nicknames",
    L"1-12 List all global settings",
    L"2-01 Fragments Title and User data directories",
    L"2-02 Removes all soundtracks",
    L"2-07 Fills User Data directory with 4096 entries",
    L"2-10 Clears utility partition for <titleid>",
    L"2-18 Draws outline of non-HDTV safe area (white)",
    L"2-19 Draws outline of HDTV safe area (yellow)",
    L"3-02, 3-03, 3-04 Fills Title and User data directories",
    L"3-11 Lists available blocks for all memory devices",
    L"3-14 Change language setting",
    L"3-15 Change time zone setting",
    L"3-26 Create Soundtracks with localized text",
    L"3-26 Create Nicknames with localized text",
    L"3-26 Create MU names with localized text",
    L"3-26 Create Saved Game names with localized text",
    L"3-33 List all files on Xbox; hard disk and MUs",
    L"6-05 Corrupt saved games for <titleid>",
    L"Clean up temp files created by the Cert Tools"
};

#define MENU_CERT_MENU_FILENAME_A               FILE_DATA_MENU_DIRECTORY_A "\\cert.mnu"
#define MENU_CERT_NUM_ITEMS_TO_DISPLAY          8
#define MENU_CERT_PROCESSING_TEXT               L"Processing..."
#define MENU_CERT_TITLE_ID_STR_LEN              8

/////////////////////////////////////////
// Online User Menu Items and Information
/////////////////////////////////////////
#define MENU_OLUSER_MENU_FILENAME_A             FILE_DATA_MENU_DIRECTORY_A "\\oluser.mnu"
#define MENU_OLUSER_NUM_ITEMS_TO_DISPLAY        8

/////////////////////////////////////////////////
// Online User Option Menu Items and Information
/////////////////////////////////////////////////
#define MENU_OLUSEROPTION_MENU_FILENAME_A       FILE_DATA_MENU_DIRECTORY_A "\\oluseropt.mnu"

//////////////////////////////////////
// Yes / No Menu Items and Information
//////////////////////////////////////
#define MENU_YESNO_MENU_FILENAME_A              FILE_DATA_MENU_DIRECTORY_A "\\yesno.mnu"

///////////////////////////////////////////
// Dump Cache partition Information
///////////////////////////////////////////
#define MENU_DUMPCACHE_MENU_FILENAME_A          FILE_DATA_MENU_DIRECTORY_A "\\dumpcache.mnu"
#define MENU_DUMPCACHE_NUM_ITEMS_TO_DISPLAY     8
#define MENU_DUMPCACHE_NOITEM_TEXT              L"There are no Titles using a cache partition"


/////////////////////////////////////////
// AV Region Menu Items and Information
/////////////////////////////////////////
enum AVRegionMenu
{
    AVREGIONMENU_ITEM_REGION1,
    AVREGIONMENU_ITEM_REGION2,
    AVREGIONMENU_ITEM_REGION3,
    
    AVREGIONMENU_ITEM_NUM_ITEMS
};

static WCHAR* g_pwszAVRegionMenuItems[] = {
    L"NTSC-M",
    L"NTSC-J",
    L"PAL-I"
};

#define MENU_AVREGION_MENU_FILENAME_A           FILE_DATA_MENU_DIRECTORY_A "\\avreg.mnu"
#define MENU_AVREGION_NUM_COLS                  1
#define MENU_AVREGION_NUM_ROWS                  AVREGIONMENU_ITEM_NUM_ITEMS
#define MENU_AVREGION_LINE_WIDTH                1.0f
#define MENU_AVREGION_BUTTON_Y1                 90
#define MENU_AVREGION_BUTTON_TEXT_BORDER        16
#define MENU_AVREGION_CURRENT_REGION_TEXT       L"Current Xbox AV Region: "
#define MENU_AVREGION_TEXT_Y1                   20.0f


/////////////////////////////////////////////
// EEPROM Settings Menu Items and Information
/////////////////////////////////////////////
#define EEPROMMENU_FILENAME_A                   FILE_DATA_MENU_DIRECTORY_A "\\eeprom.mnu"

#ifdef _DEBUG
    #define EEPROMMENU_NUM_ITEMS                    72
#else // _DEBUG
    #define EEPROMMENU_NUM_ITEMS                    8
#endif // _DEBUG

#define EEPROMMENU_NUM_ITEMS_TO_DISPLAY         8


/////////////////////////////////////
// SYSINFO Menu Items and Information
/////////////////////////////////////
#define SYSINFOMENU_FILENAME_A                  FILE_DATA_MENU_DIRECTORY_A "\\sysinfo.mnu"
#define SYSINFOMENU_NUM_ITEMS                   16
#define SYSINFOMENU_NUM_ITEMS_TO_DISPLAY        8

/////////////////////////////////////////
// Game Region Menu Items and Information
/////////////////////////////////////////
#define GAMEREGION_NONE 0

enum GameRegionMenu
{
    GAMEREGIONMENU_ITEM_REGION1,
    GAMEREGIONMENU_ITEM_REGION2,
    GAMEREGIONMENU_ITEM_REGION3,

    GAMEREGIONMENU_ITEM_NUM_ITEMS
};

static WCHAR* g_pwszGameRegionMenuItems[] = {
    L"Region 1 (North America)",
    L"Region 2 (Japan)",
    L"Region 3 (Rest of the World)"
};

#define MENU_GAMEREGION_MENU_FILENAME_A         FILE_DATA_MENU_DIRECTORY_A "\\gamereg.mnu"
#define MENU_GAMEREGION_NUM_COLS                1
#define MENU_GAMEREGION_NUM_ROWS                GAMEREGIONMENU_ITEM_NUM_ITEMS
#define MENU_GAMEREGION_LINE_WIDTH              1.0f
#define MENU_GAMEREGION_BUTTON_X1               65
#define MENU_GAMEREGION_BUTTON_Y1               90
#define MENU_GAMEREGION_BUTTON_WIDTH            300
#define MENU_GAMEREGION_CURRENT_REGION_TEXT     L"Current Xbox Game Region: "
#define MENU_GAMEREGION_TEXT_Y1                 20.0f


//////////////////////////////////////////////
// Generic Keyboard Menu Items and Information
//////////////////////////////////////////////
#define MENU_GENKEYBOARD_MENU_FILENAME          FILE_DATA_MENU_DIRECTORY_A "\\genkey.mnu"


/////////////////////////////////////////////
// Generic Message Menu Items and Information
/////////////////////////////////////////////
#define MENU_MESSAGE_MENU_FILENAME              FILE_DATA_MENU_DIRECTORY_A "\\message.mnu"


//////////////////////////////////////////////
// Generic Keyboard Menu Items and Information
//////////////////////////////////////////////
#define MENU_PASSCODE_MENU_FILENAME             FILE_DATA_MENU_DIRECTORY_A "\\passcode.mnu"
#define MENU_PASSCODE_ENTER_CODE_MSG            L"Enter a 4 Control Passcode:"
#define MENU_PASSCODE_INVALID_MSG               L"You must enter 0 or 4 items"
#define MENU_PASSCODE_HELP_TEXT1                L"(Left-Thumb Button to restart)"


//////////////////////////////////////
// Language Menu Items and Information
//////////////////////////////////////

enum LanguageMenu
{
    LANGUAGEMENU_ITEM_NOT_SET,
    LANGUAGEMENU_ITEM_ENGLISH,
    LANGUAGEMENU_ITEM_JAPANESE,
    LANGUAGEMENU_ITEM_GERMAN,
    LANGUAGEMENU_ITEM_FRENCH,
    LANGUAGEMENU_ITEM_SPANISH,
    LANGUAGEMENU_ITEM_ITALIAN,

    LANGUAGEMENU_ITEM_NUM_ITEMS
};

static WCHAR* g_pwszLanguageMenuItems[] = {
    L"No Language Set",
    L"English",
    L"Japanese",
    L"German",
    L"French",
    L"Spanish",
    L"Italian"
};

#define MENU_LANGUAGE_MENU_FILENAME_A           FILE_DATA_MENU_DIRECTORY_A "\\language.mnu"


/////////////////////////////////////
// Network Menu Items and Information
/////////////////////////////////////
enum NetworkMenu
{
    NETWORKMENU_ITEM_IPADDRESS,
    NETWORKMENU_ITEM_DEBUGIPADDRESS,
	NETWORKMENU_ITEM_PRIMARYDNS,
	NETWORKMENU_ITEM_SECONDARYDNS,
    NETWORKMENU_ITEM_SUBNETMASK,
    NETWORKMENU_ITEM_GATEWAY,
    NETWORKMENU_ITEM_MACHINENAME,
    
    NETWORKMENU_ITEM_NUM_ITEMS
};

static WCHAR* g_pwszNetworkMenuItems[] = {
    L"Title IP address :",
    L"Debug IP address :",
	L"Primary DNS :",
	L"Secondary DNS :",
    L"Subnet mask :",
    L"Gateway :",
    L"Machine name :"
};

#define NETWORKMENU_FILENAME_A                  FILE_DATA_MENU_DIRECTORY_A "\\network.mnu"
#define NETWORKMENU_OPTIONHEADER_COLOR          ITEM_HEADER_COLOR
#define NETWORKMENU_IPADDR_YPOS                 12.0f
#define NETWORKMENU_DEBUGIPADDR_YPOS            40.0f
#define NETWORKMENU_PRIMARYDNS_YPOS				68.0f
#define NETWORKMENU_SECONDARYDNS_YPOS           96.0f
#define NETWORKMENU_SUBNET_YPOS                 124.0f
#define NETWORKMENU_GATEWY_YPOS                 152.0f
#define NETWORKMENU_MANAME_YPOS                 180.0f

static float g_fNetworkItemYPos[] = {
    NETWORKMENU_IPADDR_YPOS,             // Item 0
    NETWORKMENU_DEBUGIPADDR_YPOS,        // Item 1
	NETWORKMENU_PRIMARYDNS_YPOS,		 // Item 2
	NETWORKMENU_SECONDARYDNS_YPOS,		 // Item 3
    NETWORKMENU_SUBNET_YPOS,             // Item 4
    NETWORKMENU_GATEWY_YPOS,             // Item 5
    NETWORKMENU_MANAME_YPOS              // Item 6
};


///////////////////////////////////////////
// Keypad and Keyboard location information
///////////////////////////////////////////
#define KEYBOARD_LINE_WIDTH                     1.0f
#define KEYPAD_LINE_WIDTH                       1.0f
#define KEYBOARD_TOPROW_Y1                       75
#define KEYBOARD_TOPROW_X1                        8
#define KEYPAD_TOPROW_Y1                         75
#define KEYPAD_TOPROW_X1                        108
#define MENU_DISPSET_BUTTON_Y1                   51
#define MENU_DISPSET_BUTTON_X1                  108
#define MUFORMATDLG_BUTTON_Y1                   115
#define MUFORMATDLG_BUTTON_X1                   163
#define MUUNFORMATDLG_BUTTON_Y1                 115
#define MUUNFORMATDLG_BUTTON_X1                 163
#define ERRORMSGMENU_BUTTON_Y1                  135
#define ERRORMSGMENU_BUTTON_X1                  163
#define KEYBOARD_MACHINENAMEHEADER_YPOS         24.0f                  // Must be identical to data in 'machine.mnu'
#define KEYPAD_IPADDRHEADER_YPOS                24.0f

///////////////////////////////////////////////
// Hex Keypad location and constant information
///////////////////////////////////////////////
#define HEXKEYPAD_LINE_WIDTH                     1.0f
#define HEXKEYPAD_TEXT_Y1                        24.0f
#define HEXKEYPAD_VALUE_Y1                       47.0f
#define HEXKEYPAD_TOPROW_Y1                      75
#define HEXKEYPAD_TOPROW_X1                      70
#define HEXKEYPAD_TOPROW_LARGE_BUTTONS_X1        HEXKEYPAD_TOPROW_X1 + 183;

//
// File path and location constants
//
#define FILE_EXECUTABLE_DEVICE_PATH_A           "\\Device\\Harddisk0\\Partition1"
#define FILE_DASH_PARTITION_PATH_A              "\\Device\\Harddisk0\\Partition2"
#define FILE_DVDDRIVE_PARTITION_PATH_A          "\\Device\\CdRom0"
#define FILE_DATA_UDATA_DRIVE_LETTER_A          'X'
#define FILE_DATA_TDATA_DRIVE_LETTER_A          'V'
#define FILE_DATA_UDATA_PATH_A                  "\\Device\\Harddisk0\\partition1\\udata"
#define FILE_DATA_DVD_DRIVE_A                   "a:"
#define FILE_DEVELOPER_DRIVE_A                  "e:"
#define FILE_EXECUTABLE_DRIVE_A                 "c:"
#define FILE_EXECUTABLE_PATH_A                  "\\devkit"
#define FILE_EXECUTABLE_DIRECTORY_A             FILE_EXECUTABLE_DRIVE_A FILE_EXECUTABLE_PATH_A
#define FILE_EXECUTABLE_EXENSION_A              ".XBE"
#define FILE_DATA_DRIVE_A                       "y:"
#define FILE_DATA_DRIVE                         L"y:"
#define FILE_DATA_MEDIA_DIRECTORY               FILE_DATA_DRIVE L"\\media"
#define FILE_DATA_MENU_DIRECTORY_A              FILE_DATA_DRIVE_A "\\menus"
#define FILE_DATA_IMAGE_DIRECTORY_A             FILE_DATA_DRIVE_A "\\images"
#define FILE_DATA_3DINFO_DIRECTORY_A            FILE_DATA_DRIVE_A "\\3dinfo"
#define FILE_DATA_SOUND_DIRECTORY_A             FILE_DATA_DRIVE_A "\\sounds"
#define FILE_DATA_DATA_DIRECTORY_A              FILE_DATA_DRIVE_A "\\data"
#define FILE_DATA_DEFAULT_FONT_FILENAME         FILE_DATA_MEDIA_DIRECTORY L"\\tahoma.ttf"
#define FILE_DATA_BUTTON_FONT_FILENAME          FILE_DATA_MEDIA_DIRECTORY L"\\xboxdings.ttf"
#define FILE_DATA_XDASH_TITLEID                 "fffe0000"
#define FILE_DATA_DIR_ENTRY_PATTERN             "xdk_entry_"
#define FILE_DATA_DASHBOARD_DRIVE_A             "y:"
#define FILE_DATA_DASHBOARD_DIRECTORY_A         FILE_DATA_DASHBOARD_DRIVE_A ""  // Empty if it's the root directory
#define FILE_DATA_DASHBOARD_FILENAME_A          "XBOXDASH.XBE"
#define FILE_DATA_ONLINE_DASHBOARD_FILENAME_A   "XONLINEDASH.XBE"
#define FILE_DATA_DASHBOARD_LOCATION_A          FILE_DATA_DASHBOARD_DIRECTORY_A "\\" FILE_DATA_DASHBOARD_FILENAME_A
#define FILE_DATA_ONLINE_DASHBOARD_LOCATION_A   FILE_DATA_DASHBOARD_DIRECTORY_A "\\" FILE_DATA_ONLINE_DASHBOARD_FILENAME_A
#define FILE_DATA_DEFAULT_FILENAME_A            "DEFAULT.XBE"
#define FILE_DATA_DEFAULT_LOCATION_A            FILE_DATA_DVD_DRIVE_A "\\" FILE_DATA_DEFAULT_FILENAME_A

#define FILE_DATA_CACHE_SIZE                    524288

#define STRING_DATA_LOC_CHARS                   L"\x0E01\x30E1\x30E2\x30EA\x0001\x0002\x0003"

////////////////////////////////////
// XShell Sounds & Sound Information
////////////////////////////////////
#define SOUND_SELECT_BUTTON                     FILE_DATA_SOUND_DIRECTORY_A "\\select_button.wav"
#define SOUND_BACK_BUTTON                       FILE_DATA_SOUND_DIRECTORY_A "\\back_button.wav"
#define SOUND_MENU_ITEM_SELECT                  FILE_DATA_SOUND_DIRECTORY_A "\\menu_item_select.wav"
#define SOUND_KEYBOARD_STROKE                   FILE_DATA_SOUND_DIRECTORY_A "\\keyboard_stroke.wav"

#define SOUND_VOLUME_ADJUST                     -1200

enum ShellSounds
{
    ENUM_SOUND_SELECT = 0,
    ENUM_SOUND_BACK,
    ENUM_SOUND_MENU_ITEM_SELECT,
    ENUM_SOUND_KEYBOARD_STROKE,

    ENUM_SOUND_NUM_SOUNDS
};

// Data value limits
#define DATAVALUE_IPADDRESS_LENGTH              15
#define DATAVALUE_MACHINENAME_LENGTH            20
#define DATAVALUE_DISPLAYSETTING_TEXT_LENGTH    20

// Controller and Xbox Port and Slot Information
#define NUM_XBOX_MU_SLOTS                       8

// Joystick and Button Dead Zone and Threshold info
#define JOYSTICK_DEAD_ZONE                      20000       // 0 - 32768
#define BUTTON_THRESHOLD                        32          // 0 - 255

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

static WCHAR* g_pwszMUPortNames[] = {
    L"Memory Unit 1A",
    L"Memory Unit 1B",
    L"Memory Unit 2A",
    L"Memory Unit 2B",
    L"Memory Unit 3A",
    L"Memory Unit 3B",
    L"Memory Unit 4A",
    L"Memory Unit 4B"
};

// Display Settings
enum DisplaySettings
{
    ENUM_DISPSET_TEXT_NTSC_M = 0,
    ENUM_DISPSET_TEXT_NTSC_J,
    ENUM_DISPSET_TEXT_PAL_I,
    ENUM_DISPSET_TEXT_CUSTOM
};

static WCHAR* g_pwszDISPLAYSETTINGS[] = {
    L"NTSC (Mex/Can/US)",
    L"NTSC (Japan)",
    L"PAL (Europe)",
    L"Custom (xbsetcfg)"
};

// XShell Menu Identifiers
enum XShellMenuIds
{
    ENUM_SHELLMENUID_LAUNCH = 0,
    ENUM_SHELLMENUID_MEMORY,
    ENUM_SHELLMENUID_OPTIONS,
    ENUM_SHELLMENUID_OPTIONS_NETWORK,
    ENUM_SHELLMENUID_OPTIONS_GAMEREGION,
    ENUM_SHELLMENUID_OPTIONS_AVREGION,
    ENUM_SHELLMENUID_OPTIONS_MEMORY,
    ENUM_SHELLMENUID_OPTIONS_CERT,
    ENUM_SHELLMENUID_NETWORK_IPADDRESS,
    ENUM_SHELLMENUID_NETWORK_SUBNET,
	ENUM_SHELLMENUID_NETWORK_PRIMARYDNS,
	ENUM_SHELLMENUID_NETWORK_SECONDARYDNS,
    ENUM_SHELLMENUID_NETWORK_GATEWAY,
    ENUM_SHELLMENUID_NETWORK_MACHINENAME
};

// XShell States (bit mask)
#define SHELLSTATE_NORMAL       0
#define SHELLSTATE_ERROR        1
#define SHELLSTATE_NOCONFIG     2

// XShell Errors
// This list should stay in sync with the "g_pwszERRORCODEMSG" list
#define ERRORCODE_NO_ERROR          0
#define ERRORCODE_INVALID_XBE       1
#define ERRORCODE_UNKNOWN_ERROR     2

// This list should stay in sync with the "ERRORCODE" defines
static WCHAR* g_pwszERRORCODEMSG[] = {
    L"No Error Has Occurred",
    L"An Error Occurred Loading an Xbox Executable",
    L"An Unknown Error Has Occurred"
};

// Screen item locations
#define ITEM_MACHINENAME_DATA_X                 ( MENUBOX_TEXTAREA_X_CENTER - 17.0F )

// Default Vertex structure for the XboxVideo
struct MYVERTEX
{
	D3DXVECTOR3 v;
	float       fRHW;
	D3DCOLOR    cDiffuse;
};

// Vertex Definitions
#define D3DFVF_D3DVERTEX    ( D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_DIFFUSE )
#define D3DFVF_TEXVERTEX    ( D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_DIFFUSE | D3DFVF_TEX1 )
#endif // _CONSTANTS_H_
