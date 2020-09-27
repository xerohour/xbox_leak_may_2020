#ifndef _ENVIRONMENTS_H
#define _ENVIRONMENTS_H


// This structure will hold a list of Memory Devices (areas)
// with the first element being a device to Copy From, and the
// second element being the device to Copy To.
typedef struct _CopyFromToMD {
    enum MemoryAreasWithinList FromMD;
    enum MemoryAreasWithinList ToMD;
} CopyFromToMD;


// Types of devices that can be plugged in to the Xbox
enum DeviceTypes
{
    DEVICE_EMPTY,
    DEVICE_DUKE,
    DEVICE_MU,
    DEVICE_HAWK,
    DEVICE_MAX
};


// State of the Xbox tray
enum TrayStates
{
    TRAY_CLOSED,
    TRAY_OPEN,
    TRAY_MAX
};

// Media types that can be inserted in the Xbox tray
enum MediaTypes
{
    MEDIA_EMPTY,
    MEDIA_DVD_XBOX_GAME_RATED_NR,
    MEDIA_DVD_XBOX_GAME_RATED_ADULTS,
    MEDIA_DVD_XBOX_GAME_RATED_MATURE,
    MEDIA_DVD_XBOX_GAME_RATED_TEEN,
    MEDIA_DVD_XBOX_GAME_RATED_EVERYONE,
    MEDIA_DVD_XBOX_GAME_RATED_CHILDHOOD,
    MEDIA_DVD_XBOX_GAME_MULTIDISK_1,
    MEDIA_DVD_XBOX_GAME_MULTIDISK_2,
    MEDIA_DVD_MOVIE_NOREGION,
    MEDIA_DVD_MOVIE_REGION1_RATED_NR,
    MEDIA_DVD_MOVIE_REGION1_RATED_NC17,
    MEDIA_DVD_MOVIE_REGION1_RATED_R,
    MEDIA_DVD_MOVIE_REGION1_RATED_PG13,
    MEDIA_DVD_MOVIE_REGION1_RATED_PG,
    MEDIA_DVD_MOVIE_REGION1_RATED_G,
    MEDIA_DVD_MOVIE_REGION2,
    MEDIA_DVD_MOVIE_REGION3,
    MEDIA_DVD_MOVIE_REGION4,
    MEDIA_DVD_MOVIE_REGION5,
    MEDIA_DVD_MOVIE_REGION6,
    MEDIA_CD_AUDIO,
    MEDIA_MAX
};

// Video Output types
enum VideoTypes
{
    VIDEO_TV_43NORMAL,
    VIDEO_TV_43WIDESCREEN,
    VIDEO_TV_HDTV,
    VIDEO_MAX
};

// Audio Output types
enum AudioTypes
{
    AUDIO_NORMAL,
    AUDIO_PCM,
    AUDIO_RAW,
    AUDIO_MAX
};

// Network Connection State
enum NetworkCableState
{
    NETWORK_CABLE_UNPLUGGED,
    NETWORK_CABLE_ATTACHED,
    NETWORK_MAX
};

// Describes the ports on the available devices (if any)
struct Device
{
    int slots[4];
    Device(int a=DEVICE_EMPTY, int b=DEVICE_EMPTY, int c=DEVICE_EMPTY, int d=DEVICE_EMPTY) { slots[0] = a; slots[1] = b; slots[2] = c; slots[3] = d; };
};

struct XBOXPorts
{
    Device ports[4];
    XBOXPorts(Device a=DEVICE_EMPTY, Device b=DEVICE_EMPTY, Device c=DEVICE_EMPTY, Device d=DEVICE_EMPTY) { ports[0] = a; ports[1] = b; ports[2] = c; ports[3] = d; };
};


struct environment
{
    enum EnvironmentDesc description;
    int trayState;
    XBOXPorts PortConfig;
    int mediaType;
    int videoType;
    int audioType;
    int networkCable;
};

static XBOXPorts XboxPortConfigs[] =
{
    // No MU's installed
    XBOXPorts( Device(DEVICE_EMPTY), Device(DEVICE_EMPTY), Device(DEVICE_EMPTY), Device(DEVICE_EMPTY) ),
    XBOXPorts( Device(DEVICE_DUKE),  Device(DEVICE_EMPTY), Device(DEVICE_EMPTY), Device(DEVICE_EMPTY) ),
    XBOXPorts( Device(DEVICE_EMPTY), Device(DEVICE_DUKE),  Device(DEVICE_EMPTY), Device(DEVICE_EMPTY) ),
    XBOXPorts( Device(DEVICE_EMPTY), Device(DEVICE_EMPTY), Device(DEVICE_DUKE),  Device(DEVICE_EMPTY) ),
    XBOXPorts( Device(DEVICE_EMPTY), Device(DEVICE_EMPTY), Device(DEVICE_EMPTY), Device(DEVICE_DUKE)  ),
    XBOXPorts( Device(DEVICE_DUKE),  Device(DEVICE_DUKE),  Device(DEVICE_EMPTY), Device(DEVICE_EMPTY) ),
    XBOXPorts( Device(DEVICE_DUKE),  Device(DEVICE_EMPTY), Device(DEVICE_DUKE),  Device(DEVICE_EMPTY) ),
    XBOXPorts( Device(DEVICE_DUKE),  Device(DEVICE_EMPTY), Device(DEVICE_EMPTY), Device(DEVICE_DUKE)  ),
    XBOXPorts( Device(DEVICE_EMPTY), Device(DEVICE_DUKE),  Device(DEVICE_DUKE),  Device(DEVICE_EMPTY) ),
    XBOXPorts( Device(DEVICE_EMPTY), Device(DEVICE_DUKE),  Device(DEVICE_EMPTY), Device(DEVICE_DUKE)  ),
    XBOXPorts( Device(DEVICE_EMPTY), Device(DEVICE_EMPTY), Device(DEVICE_DUKE),  Device(DEVICE_DUKE)  ),
    XBOXPorts( Device(DEVICE_EMPTY), Device(DEVICE_DUKE),  Device(DEVICE_DUKE),  Device(DEVICE_DUKE)  ),
    XBOXPorts( Device(DEVICE_DUKE),  Device(DEVICE_EMPTY), Device(DEVICE_DUKE),  Device(DEVICE_DUKE)  ),
    XBOXPorts( Device(DEVICE_DUKE),  Device(DEVICE_DUKE),  Device(DEVICE_EMPTY), Device(DEVICE_DUKE)  ),
    XBOXPorts( Device(DEVICE_DUKE),  Device(DEVICE_DUKE),  Device(DEVICE_DUKE),  Device(DEVICE_EMPTY) ),

    // Single MU's installed
    XBOXPorts( Device(DEVICE_DUKE, DEVICE_MU),  Device(DEVICE_EMPTY),            Device(DEVICE_EMPTY),            Device(DEVICE_EMPTY)           ),
    XBOXPorts( Device(DEVICE_EMPTY),            Device(DEVICE_DUKE, DEVICE_MU),  Device(DEVICE_EMPTY),            Device(DEVICE_EMPTY)           ),
    XBOXPorts( Device(DEVICE_EMPTY),            Device(DEVICE_EMPTY),            Device(DEVICE_DUKE, DEVICE_MU),  Device(DEVICE_EMPTY)           ),
    XBOXPorts( Device(DEVICE_EMPTY),            Device(DEVICE_EMPTY),            Device(DEVICE_EMPTY),            Device(DEVICE_DUKE, DEVICE_MU) ),
    XBOXPorts( Device(DEVICE_DUKE, DEVICE_MU, DEVICE_EMPTY),  Device(DEVICE_DUKE, DEVICE_MU, DEVICE_EMPTY),  Device(DEVICE_DUKE, DEVICE_MU, DEVICE_EMPTY),  Device(DEVICE_DUKE, DEVICE_MU, DEVICE_EMPTY) ),
    XBOXPorts( Device(DEVICE_DUKE, DEVICE_EMPTY, DEVICE_MU),  Device(DEVICE_DUKE, DEVICE_EMPTY, DEVICE_MU),  Device(DEVICE_DUKE, DEVICE_EMPTY, DEVICE_MU),  Device(DEVICE_DUKE, DEVICE_EMPTY, DEVICE_MU) ),

    // Multiple MU's installed
    XBOXPorts( Device(DEVICE_DUKE, DEVICE_MU, DEVICE_MU),  Device(DEVICE_DUKE, DEVICE_MU, DEVICE_MU),  Device(DEVICE_DUKE, DEVICE_MU, DEVICE_MU),  Device(DEVICE_DUKE, DEVICE_MU, DEVICE_MU) ),
    XBOXPorts( Device(DEVICE_DUKE, DEVICE_MU, DEVICE_MU),  Device(DEVICE_EMPTY),                       Device(DEVICE_EMPTY),                       Device(DEVICE_EMPTY)                      ),
    XBOXPorts( Device(DEVICE_EMPTY),                       Device(DEVICE_DUKE, DEVICE_MU, DEVICE_MU),  Device(DEVICE_EMPTY),                       Device(DEVICE_EMPTY)                      ),
    XBOXPorts( Device(DEVICE_EMPTY),                       Device(DEVICE_EMPTY),                       Device(DEVICE_DUKE, DEVICE_MU, DEVICE_MU),  Device(DEVICE_EMPTY)                      ),
    XBOXPorts( Device(DEVICE_EMPTY),                       Device(DEVICE_EMPTY),                       Device(DEVICE_EMPTY),                       Device(DEVICE_DUKE, DEVICE_MU, DEVICE_MU) ),
};

// Describes the various hardware ports on the XBox
// and it's controllers
static const int gc_nNUM_XBOX_PORTS = 4;
static const int gc_nNUM_DUKE_SLOTS = 2;


// Used to reference specific duke and mu configurations for a port.
// If the Duke and MU array changes, you *MUST* update this list
enum PortConfigurationsWithinList
{
    EMPTY,
    DUKE_E_E,
    DUKE_M_E,
    DUKE_E_M,
    DUKE_M_M
};


// Valid Duke and MU configurations for a Port on the XBox
// If you change this list, you *MUST* update the ENUM list!!
static const WCHAR *validDukeAndMUConfigs[] = {
    L"Empty",
    L"Duke (E, E)",
    L"Duke (M, E)",
    L"Duke (E, M)",
    L"Duke (M, M)"
};

// Used to reference specific memory areas from our array of all memory areas
// If the Memory Area array changes, you *MUST* update this list
enum MemoryAreasWithinList
{
    MEMORY_AREA_MU1_DUKE1,
    MEMORY_AREA_MU2_DUKE1,
    MEMORY_AREA_MU1_DUKE2,
    MEMORY_AREA_MU2_DUKE2,
    MEMORY_AREA_MU1_DUKE3,
    MEMORY_AREA_MU2_DUKE3,
    MEMORY_AREA_MU1_DUKE4,
    MEMORY_AREA_MU2_DUKE4,
    MEMORY_AREA_XDISK
};

// Memory Area's available on the XBox
// If you change this list, you *MUST* update
// the ENUM list!!
static const WCHAR* gc_lpwszMEM_AREAS[] = {
    L"MU 1 in Duke 1",
    L"MU 2 in Duke 1",
    L"MU 1 in Duke 2",
    L"MU 2 in Duke 2",
    L"MU 1 in Duke 3",
    L"MU 2 in Duke 3",
    L"MU 1 in Duke 4",
    L"MU 2 in Duke 4",
    L"Xdisk"
};


// Used to reference specific memory area block configurations
// from our array of all memory area block configurations.
// If the Memory Area block configuration array changes, you *MUST* update this list
enum MemoryAreaBlockConfigsWithinList
{
    MEMORY_BLOCK_CONFIG_0_BLOCKS,
    MEMORY_BLOCK_CONFIG_1_BLOCK,
    MEMORY_BLOCK_CONFIG_HALF_BLOCKS,
    MEMORY_BLOCK_CONFIG_MAX_MINUS_1_BLOCKS,
    MEMORY_BLOCK_CONFIG_MAX__BLOCKS,
    MEMORY_BLOCK_CONFIG_MAX_PLUS_1_BLOCKS
};

// Memory Area standard block configurations
// If this list changes, you *MUST* update the enum list
static const WCHAR* gc_lpwszMEM_AREA_BLOCK_CONFIGS[] = {
    L"0 blocks used",
    L"1 block used",
    L"1/2 of MAX blocks used",
    L"MAX - 1 blocks used",
    L"MAX blocks used",
    L"MAX + 1 blocks used"
};

// Memory Area standard saved game configurations
static const WCHAR* gc_lpwszMEM_AREA_SAVEGAME_CONFIGS[] = {
    L"Empty",
    L"MAX Titles with 1 saved game, 1 block per saved game",
    L"MAX Titles with 10 saved games, 1 block per saved game",
    L"1 Title with MAX saved games, 1 block per saved game",
    L"2 Titles with MAX saved games, 1 block per saved game, each title uses 1/2 available space"
};


// List of saved game combinations for Single Saved game cases
static const WCHAR* gc_lpwszSINGLE_SAVED_GAME_CONFIGS[] = {
    L"1 Title (MAX saved games) - FIRST saved game",
    L"1 Title (MAX saved games) - MIDDLE saved game",
    L"1 Title (MAX saved games) - LAST saved game",
    L"2 Titles (1 saved game each) - Title 1's saved game",
    L"2 Titles (1 saved game each) - Title 2's saved game",
    L"2 Titles (Title 1 has 2 saved games, Title 2 has 1 saved game) - Title 1's FIRST saved game",
    L"2 Titles (Title 1 has 2 saved games, Title 2 has 1 saved game) - Title 1's LAST saved game",
    L"2 Titles (Title 1 has 1 saved game, Title 2 has 2 saved games) - Title 2's FIRST saved game",
    L"2 Titles (Title 1 has 1 saved game, Title 2 has 2 saved games) - Title 2's LAST saved game"
};

// List of saved game combinations for Multiple Saved game cases
static const WCHAR* gc_lpwszMULTIPLE_SAVED_GAME_CONFIGS[] = {
    L"1 Title (MAX saved games) - ALL saved games",
    L"1 Title (MAX saved games) - FIRST 5 saved games",
    L"1 Title (MAX saved games) - MIDDLE 5 saved games",
    L"1 Title (MAX saved games) - LAST 5 saved games",
    L"2 Titles (Title 1 has 10 saved games, Title 2 has 1 saved game) - ALL Title 1's saved games",
    L"2 Titles (Title 1 has 10 saved games, Title 2 has 1 saved game) - Title 1's FIRST 5 saved games",
    L"2 Titles (Title 1 has 10 saved games, Title 2 has 1 saved game) - Title 1's LAST 5 saved games",
    L"2 Titles (Title 1 has 1 saved game, Title 2 has 10 saved games) - ALL Title 2's saved games",
    L"2 Titles (Title 1 has 1 saved game, Title 2 has 10 saved games) - Title 2's FIRST 5 saved games",
    L"2 Titles (Title 1 has 1 saved game, Title 2 has 10 saved games) - Title 2's LAST 5 saved games"
};


// Used to reference specific title delete variations within the array
// If you change the array, you *MUST* update this list
enum DeleteTitleGameConfigsWithinList
{
    DELETE_1_TITLE_1_SAVED_GAME,
    DELETE_1_TITLE_MAX_SAVED_GAMES,
    DELETE_1_TITLE_MAX_PLUS_1_SAVED_GAMES,
    DELETE_2_TITLES_TITLE1_1_SAVED_GAME_TITLE2_1_SAVED_GAME_SELECT_TITLE1,
    DELETE_2_TITLES_TITLE1_1_SAVED_GAME_TITLE2_1_SAVED_GAME_SELECT_TITLE2,
    DELETE_2_TITLES_TITLE1_HALF_MAX_SAVED_GAMES_TITLE2_HALF_MAX_SAVED_GAMES_SELECT_TITLE1,
    DELETE_2_TITLES_TITLE1_1_SAVED_GAME_TITLE2_MAX_SAVED_GAMES_SELECT_TITLE1,
    DELETE_2_TITLES_TITLE1_MAX_SAVED_GAMES_TITLE2_1_SAVED_GAME_SELECT_TITLE2,
    DELETE_3_TITLES_TITLE1_1_SAVED_GAME_TITLE2_1_SAVED_GAME_TITLE3_1_SAVED_GAME_SELECT_TITLE2,
};

// List of Title and saved game combinations to be removed entirely from the XBox
// If you change this list you *MUST* update the ENUM list
static const WCHAR* gc_lpwszDELETE_TITLE_SAVED_GAME_CONFIGS[] = {
    L"1 Title (1 saved game)",
    L"1 Title (MAX saved games)",
    L"1 Title (MAX +1 saved games)",
    L"2 Titles (Title 1 has 1 saved game, Title 2 has 1 saved game) - Select Title 1",
    L"2 Titles (Title 1 has 1 saved game, Title 2 has 1 saved game) - Select Title 2",
    L"2 Titles (Title 1 has 1/2 MAX saved games, Title 2 has 1/2 MAX saved games) - Select Title 1",
    L"2 Titles (Title 1 has 1/2 MAX saved games, Title 2 has 1/2 MAX saved games) - Select Title 2",
    L"2 Titles (Title 1 has 1 saved game, Title 2 has MAX saved games) - Select Title 1",
    L"2 Titles (Title 1 has MAX saved games, Title 2 has 1 saved game) - Select Title 2",
    L"3 Titles (Title 1 has 1 saved game, Title 2 has 1 saved game, Title 3 has 1 saved game) - Select Title 2"
};


// Used to reference specific 'generic' saved games our array of 'generic' saved games
// If the 'generic' saved games array changes, you *MUST* update this list
enum SavedGamesGenericWithinList
{
    SAVED_GAMES_SINGLE,
    SAVED_GAMES_MULTIPLE,
    SAVED_GAMES_ALL
};

// Generic saved game configurations to be used in none-game specific tests
// If this list changes, you *MUST* update the ENUM list
static const WCHAR* gc_lpwszGENERIC_SAVED_GAME_CONFIGS[] = {
    L"1 saved game",
    L"Multiple saved games",
    L"All saved games"
};


// List of saved game actions to be performed for HW and other generic tests
static const WCHAR* gc_lpwszSAVED_GAME_ACTIONS[] = {
    L"Delete",
    L"Copy"
};


// Variations of names that Titles and Saved Games can have
static const WCHAR* gc_lpwszGAME_AND_TITLE_NAMES[] = {
    L"MAX Chars, uppercase, identical",
    L"MAX Chars, lowercase, identical",
    L"MAX Chars, mixed case, identical",
    L"MAX Chars, uppercase, last char different (A - Z)",
    L"MAX Chars, lowercase, last char different (a - z)",
    L"MAX Chars, mixed case, last char different (A - z)",
    L"MAX Chars, one has 'special' char as it's first char, the rest are MAX chars, mixed letters",
    L"MAX Chars, mixed case, identical except one has 'special' char as it's last char"
};

// SAVED GAME META: Variations in the name of a saved game
static const WCHAR* gc_lpwszMETA_SAVED_GAME_NAME_CONFIGS[] = {
    L"0 characters (saved game name missing)",
    L"1 character",
    L"MAX - 1 characters",
    L"MAX characters",
    L"MAX + 1 characters"
};

// SAVED GAME META: Variations in the name of the game that a saved game is from
static const WCHAR* gc_lpwszMETA_SAVED_GAME_PARENT_NAME_CONFIGS[] = {
    L"0 characters (parent game name missing)",
    L"1 character",
    L"MAX - 1 characters",
    L"MAX characters",
    L"MAX + 1 characters"
};

// SAVED GAME META: Variations in the size of the saved game
static const WCHAR* gc_lpwszMETA_SAVED_GAME_SIZE_CONFIGS[] = {
    L"Size missing (not in meta data)",
    L"0 blocks",
    L"1 block",
    L"MAX - 1 blocks",
    L"MAX blocks",
    L"MAX + 1 blocks"
};

// SAVED GAME META: Variations in the time that a game was saved
static const WCHAR* gc_lpwszMETA_SAVED_GAME_TIME_CONFIGS[] = {
    L"Time missing (not in meta data)",
    L"12:00am",
    L"12:00pm"
};

// SAVED GAME META: Variations in the images representing a saved game
static const WCHAR* gc_lpwszMETA_SAVED_GAME_IMAGE_CONFIGS[] = {
    L"Image missing (not in meta data)",
    L"Image normal size",
    L"Image to small",
    L"Image to large"
};

////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////// SETTINGS ///////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////

// Used to reference specific dates and times from the clock start and end
// range list.
// If the list array changes, you *MUST* update this list
enum ClockValidDataWithinList
{
    SETTINGS_AREA_CLOCK_START_RANGE,
    SETTINGS_AREA_CLOCK_LEAP_DATE,
    SETTINGS_AREA_CLOCK_NON_LEAP_DATE,
    SETTINGS_AREA_CLOCK_END_RANGE
};


// Clock valid data and end range
static const WCHAR* gc_lpwszCLOCK_VALID_DATA[] = {
    L"Month, Day, Year, XX:XX:XX",
    L"Feb, 29, 2004, 12:00:00",
    L"Feb, 28, 2001, 12:00:00",
    L"Month, Day, Year, XX:XX:XX"
};


// The list of regions / countries that we will ship
// an XBox to that has specific (different) configurations
static const WCHAR* gc_lpwszXBOX_NATIVE_REGIONS[] = {
    L"North America",
    L"Japan"
};

// The list of languages that the XBox default title supports
static const WCHAR* gc_lpwszXBOX_DT_LANGUAGES[] = {
    L"English",
    L"Japanese",
    L"German",
    L"French",
    L"Spanish",
    L"Italian"
};


// Used to reference specific video settings from the video settings list.
// If the list array changes, you *MUST* update this list
enum VideoSettingsWithinList
{
    SETTINGS_VIDEO_NORMAL,
    SETTINGS_VIDEO_DEFAULT = 0, // The setting that a fresh XBox defaults to
    SETTINGS_VIDEO_LETTERBOX,
    SETTINGS_VIDEO_WIDESCREEN
};

// The list of valid video settings in the XDash
// If this list changes, you *MUST* update the enum list
static const WCHAR* gc_lpwszXBOX_DT_VIDEO_OPTIONS[] = {
    L"Normal",
    L"Letterbox",
    L"Widescreen"
};


// Used to reference specific audio settings from the audio settings list.
// If the list array changes, you *MUST* update this list
enum AudioSettingsWithinList
{
    SETTINGS_AUDIO_STEREO,
    SETTINGS_AUDIO_DEFAULT = 0, // The setting that a fresh XBox defaults to
    SETTINGS_AUDIO_MONO,
    SETTINGS_AUDIO_ANALOG,
    SETTINGS_AUDIO_DIGITAL
};

// The list of valid video settings in the XDash
// If this list changes, you *MUST* update the enum list
static const WCHAR* gc_lpwszXBOX_DT_AUDIO_OPTIONS[] = {
    L"Stereo",
    L"Mono",
    L"Analog",
    L"Digital"
};


// Used to reference specific video settings from the video settings list.
// If the list array changes, you *MUST* update this list
enum AVPacksWithinList
{
    AVPACK_NONE,
    AVPACK_NORMAL,
    AVPACK_DIGITAL
};

// The list of valid A/V packs the XBox supports
// If this list changes, you *MUST* update the enum list
static const WCHAR* gc_lpwszXBOX_AV_PACKS[] = {
    L"None",
    L"Normal",
    L"Digital"
};


// Tray            : Open / Closed
// Ports           : Device Installed / What's in the device
// Media Type      : None, XBox Game, Audio CD, etc
// Video Output    : 4:3Normal, HDTV, etc.
// Audio Output    : Normal, PCM, Dolby Digital
// Network Cable   : Plugged in / Not plugged in


#endif // _ENVIRONMENTS_H