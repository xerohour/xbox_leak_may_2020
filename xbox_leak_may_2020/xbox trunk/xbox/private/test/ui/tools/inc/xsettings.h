/*****************************************************
*** xsettings.h
***
*** Header file for our XSettings class.
*** This is class which will hold the settings
*** that live on the Xbox.
***
*** by James N. Helm
*** December 2nd, 2000
***
*****************************************************/

#ifndef _XSETTINGS_H_
#define _XSETTINGS_H_

#include <xconfig.h>
#include <PerBoxData.h>
#include <ntddscsi.h>
#include <cryptkeys.h>
#include <av.h>
#include <winsockp.h>

struct TZDATE {
    BYTE month;
    BYTE day;
    BYTE dayofweek;
    BYTE hour;
};

#define countof(n) (sizeof (n) / sizeof (n[0]))

struct TZINFO {
    const char* dispname;
    SHORT dltflag;
    SHORT bias;
    SHORT stdbias;
    SHORT dltbias;
    const WCHAR* stdname;
    struct TZDATE stddate;
    const WCHAR* dltname;
    struct TZDATE dltdate;
};

// Display Settings
enum XboxDisplaySettings
{
    XSETTINGS_DISPLAY_SETTING_NONE    = 0,
    XSETTINGS_DISPLAY_SETTING_NTSC_M  = AV_STANDARD_NTSC_M | AV_FLAGS_60Hz,
    XSETTINGS_DISPLAY_SETTING_NTSC_J  = AV_STANDARD_NTSC_J | AV_FLAGS_60Hz,
    XSETTINGS_DISPLAY_SETTING_PAL_I   = AV_STANDARD_PAL_I  | AV_FLAGS_50Hz,
};

// DVD Regions
enum XboxDVDRegions
{
    XSETTINGS_DVD_REGION0 = 0,
    XSETTINGS_DVD_REGION1,
    XSETTINGS_DVD_REGION2,
    XSETTINGS_DVD_REGION3,
    XSETTINGS_DVD_REGION4,
    XSETTINGS_DVD_REGION5,
    XSETTINGS_DVD_REGION6,
};

#define SETTINGS_FACTORY_SERIAL_NUM_SIZE        12
#define SETTINGS_FACTORY_ETHERNET_SIZE          6
#define SETTINGS_FACTORY_RESERVED1_SIZE         2
#define SETTINGS_FACTORY_ONLINEKEY_SIZE         16
#define SETTINGS_USER_TZSTDNAME_SIZE            XC_TZNAMELEN
#define SETTINGS_USER_TZDLTNAME_SIZE            XC_TZNAMELEN
#define SETTINGS_USER_TZSTDDATE_SIZE            sizeof( XBOX_TIMEZONE_DATE )
#define SETTINGS_USER_TZDLTDATE_SIZE            sizeof( XBOX_TIMEZONE_DATE )
#define SETTINGS_USER_RESERVED_SIZE             2
#define SETTINGS_ENC_CHECKSUM_SIZE              20
#define SETTINGS_ENC_CONFOUNDER_SIZE            8
#define SETTINGS_ENC_HDKEY_SIZE                 XBOX_KEY_LENGTH

const UCHAR SETTINGS_EEPROM_DEVKIT_KEY[] = {
    0x7B,
    0x35,
    0xA8,
    0xB7,
    0x27,
    0xED,
    0x43,
    0x7A,
    0xA0,
    0xBA,
    0xFB,
    0x8F,
    0xA4,
    0x38,
    0x61,
    0x80
};


class CXSettings
{
public:
    // Constructors and Destructors
    CXSettings();
    ~CXSettings();

    // Public Methods
    HRESULT Init();                                         // Initialize our object
    HRESULT LoadSettings();                                 // Load / Update the current settings with those from the Config Sector
    BOOL    SettingsHaveChanged();                          // Will compare the latest settings to the previous settings, and determine if they have changed
    HRESULT SetIPAddress( char* IPAddress );                // Set the IP Address on the Xbox
    HRESULT SetIPAddress( WCHAR* IPAddress );               // Set the IP Address on the Xbox
    HRESULT SetDebugIPAddress( char* IPAddress );           // Set the Debug IP Address on the Xbox
    HRESULT SetDebugIPAddress( WCHAR* IPAddress );          // Set the Debug IP Address on the Xbox
	HRESULT SetPrimaryDNS( char* IPAddress );				// Set the Primary DNS Address on the Xbox
	HRESULT SetPrimaryDNS( WCHAR* IPAddress );				// Set the Primary DNS Address on the Xbox
	HRESULT SetSecondaryDNS( char* IPAddress );				// Set the Secondary DNS Address on the Xbox
	HRESULT SetSecondaryDNS( WCHAR* IPAddress );			// Set the Secondary DNS Address on the Xbox
    HRESULT SetSubnetMask( char* SubnetMask );              // Set the SubnetMask on the Xbox
    HRESULT SetSubnetMask( WCHAR* SubnetMask );             // Set the SubnetMask on the Xbox
    HRESULT SetGateway( char* Gateway );                    // Set the Gateway on the Xbox
    HRESULT SetGateway( WCHAR* Gateway );                   // Set the Gateway on the Xbox
    HRESULT SetMachineName( char* MachineName );            // Set the MachineName on the Xbox
    HRESULT SetMachineName( WCHAR* MachineName );           // Set the MachineName on the Xbox
    HRESULT SetAVRegion( DWORD dwAVRegion );                // Set the Xbox AV Region
    HRESULT SetDVDRegion( DWORD dwDVDRegion );              // Set the Xbox DVD Region
    HRESULT SetGameRegion( DWORD dwGameRegion );            // Set the Xbox Game Region     [ENCRYPTED]
    HRESULT SetHDKey( UCHAR* puszHDKeyBuff );               // Set the Xbox HD Key          [ENCRYPTED]
    HRESULT SetLanguage( DWORD dwLanguage );                // Set the language
    HRESULT SetDTS( BOOL bEnable );                         // Enable or Disable DTS
    HRESULT SetDolbyDigital( BOOL bEnable );                // Enable or Disable Dolby Digital
    HRESULT Set1080i( BOOL bEnable );                       // Enable or Disable 1080i
    HRESULT Set720p( BOOL bEnable );                        // Enable or Disable 720p
    HRESULT Set480p( BOOL bEnable );                        // Enable or Disable 480p
    HRESULT SetPAL60( BOOL bEnable );                       // Enable or Disable PAL 60
    HRESULT SetWidescreen( BOOL bEnable );                  // Enable or Disable Widescreen
    HRESULT SetLetterbox( BOOL bEnable );                   // Enable or Disable Letterbox
    HRESULT SetTimeZoneInfo( unsigned int uiIndex );        // Set the Time Zone information

    HRESULT ResetAllToDefaults();							// Reset ALL Xbox Settings to their Initial State values

    DWORD GetNewGlobalChecksum();                           // Get a checksum for ALL of the settings
    DWORD GetNewFactoryChecksum();                          // Get a checksum for the Factory settings
    DWORD GetNewUserChecksum();                             // Get a checksum for the User settings
    DWORD GetNewEncryptedChecksum();                        // Get a checksum for the Encrypted settings

    // Accessors
    DWORD GetAVRegion();                                    // Get the Xbox AV Region
    DWORD GetGameRegion();                                  // Get the Xbox Game Region
    DWORD GetDVDRegion();                                   // Get the Xbox DVD Region
    DWORD GetLanguage();                                    // Get the Xbox Language
    char* GetIPAddress();                                   // Get the Xbox IP address
    char* GetDebugIPAddress();                              // Get the Xbox Debug IP address
	char* GetPrimaryDNS();									// Get the Xbox Primary DNS address
	char* GetSecondaryDNS();								// Get the Xbox Secondary DNS address
    char* GetSubnetMask();                                  // Get the Xbox Subnet mask
    char* GetGateway();                                     // Get the Xbox Default Gateway
    char* GetMachineName();                                 // Get the Xbox Machine Name
    BOOL GetDTS();                                          // Get the state of DTS
    BOOL GetDolbyDigital();                                 // Get the state of Dolby Digital
    BOOL Get1080i();                                        // Get the state of 1080i
    BOOL Get720p();                                         // Get the state of 720p
    BOOL Get480p();                                         // Get the state of 480p
    BOOL GetPAL60();                                        // Get the state of PAL 60
    BOOL GetWidescreen();                                   // Get the state of Widescreen
    BOOL GetLetterbox();                                    // Get the state of Letterbox
    unsigned int GetCurrentTimeZoneIndex();                 // Get the Index of the current timezone in the EEPROM
    TZINFO* GetCurrentTimeZoneInfo();                       // Get the currently set Time Zone information from the EEPROM
    TZINFO* GetTimeZoneInfo( unsigned int uiIndex );        // Get the Xbox Time Zone Information for a specified index
    void GetHDRecoveryKey( OUT UCHAR* pucBuffer,            // Returns the recovery key generated from the current HD Key
                           IN ULONG cbBufferSize );


    // Factory Settings Accessors
    ULONG GetFactoryChecksum();
    void GetFactorySerialNumber( OUT UCHAR* pucBuffer,
                                 IN ULONG cbBufferSize );
    void GetFactoryEthernetAddr( OUT UCHAR* pucBuffer,
                                 IN ULONG cbBufferSize );
    void GetFactoryReserved1( OUT UCHAR* pucBuffer,
                              IN ULONG cbBufferSize );
    void GetFactoryOnlineKey( OUT UCHAR* pucBuffer,
                              IN ULONG cbBufferSize );
    ULONG GetFactoryAVRegion();
    ULONG GetFactoryReserved2();

    // User Settings Accessors
    ULONG GetUserChecksum();
    ULONG GetUserTimeZoneBias();
    void GetUserTimeZoneStdName( OUT WCHAR* pwszBuffer,
                                 IN ULONG cbBufferSize );
    void GetUserTimeZoneDltName( OUT WCHAR* pwszBuffer,
                                 IN ULONG cbBufferSize );
    void GetUserTimeZoneStdDate( OUT XBOX_TIMEZONE_DATE* pTZStdDateBuffer,
                                 IN ULONG cbBufferSize );
    void GetUserTimeZoneDltDate( OUT XBOX_TIMEZONE_DATE* pTZDltDateBuffer,
                                 IN ULONG cbBufferSize );
    void GetUserReserved( OUT ULONG* pulBuffer,
                          IN ULONG cbBufferSize );
    ULONG GetUserTimeZoneStdBias();
    ULONG GetUserTimeZoneDltBias();
    ULONG GetUserLanguage();
    ULONG GetUserVideoFlags();
    ULONG GetUserAudioFlags();
    ULONG GetUserParentalControlGames();
    ULONG GetUserParentalControlPassword();
    ULONG GetUserParentalControlMovies();
    ULONG GetUserOnlineIpAddress();
    ULONG GetUserOnlineDnsAddress();
    ULONG GetUserOnlineDefaultGatewayAddress();
    ULONG GetUserOnlineSubnetMask();
    ULONG GetUserMiscFlags();
    ULONG GetUserDvdRegion();

    // Encrypted Settings Accessors
    void GetEncChecksum( OUT UCHAR* pucBuffer,
                         IN ULONG cbBufferSize );
    void GetEncConfounder( OUT UCHAR* pucBuffer,
                           IN ULONG cbBufferSize );
    void GetEncHDKey( OUT UCHAR* pucBuffer,
                      IN ULONG cbBufferSize );
    ULONG GetEncGameRegion();

private:
    // Private Properties
    EEPROM_LAYOUT               m_AllSettings;          // All EEPROM Settings
    EEPROM_LAYOUT               m_PrevAllSettings;      // All Previous EEPROM Settings
    XNetConfigParams            m_XNetConfigParams;     // All XNet configuration settings

    XBOX_USER_SETTINGS*         m_pUserSettings;        // All Configuration Settings
    XBOX_FACTORY_SETTINGS*      m_pFactorySettings;     // Xbox Factory Settings
    XBOX_ENCRYPTED_SETTINGS*    m_pEncSettings;         // Xbox Encrypted Settings

    char* m_pszIPAddress;                               // The IP Address of the Xbox in string form
    char* m_pszDebugIPAddress;                          // The Debug IP Address of the Xbox in string form
    char* m_pszPrimaryDNS;                              // The Primary DNS address of the Xbox in string form
    char* m_pszSecondaryDNS;                            // The Secondary DNS address of the Xbox in string form
    char* m_pszSubnetMask;                              // The Subnet Mask of the Xbox in string form
    char* m_pszGateway;                                 // The Gateway of the Xbox in string form
    char* m_pszDisplaySetting;                          // The Display Setting in string form
    char* m_pszMachineName;                             // The Machine Name of the Xbox
    BOOL bInitialized;                                  // Ensure we don't initialize twice

    // Private Methods
    
    void TZDateToSysTime(const struct TZDATE* tzdate,   // Convert a timezone date into a system time date
                         SYSTEMTIME* systime);
    BOOL ParseIPAddress( /*[in]*/char* szIPAddr,        // Convert a string IP to a DWORD
                         /*[out]*/DWORD& dwIPAddr );
    char* StringFromIPAddress( DWORD dwIPAddr );        // Returns a string formatted IP Address
	BOOL XboxNameExistsOnNet( char* pszName );          // Returns TRUE if the Xbox Name was found on the network
};
#define XSETTINGS_APP_TITLE_NAME_A              "XSET"
#define XSETTINGS_MACHINE_NAME                  ( XC_MAX_OS + 1 )   // 1 greater than the largest normal value

#define XSETTINGS_IPADDRESS_LENGTH              15
#define XSETTINGS_MACHINENAME_LENGTH            20
#define XSETTINGS_DISPLAYSETTING_TEXT_LENGTH    20

#define _TIMEDISPLAY(x) x

#define XSETTINGS_TIMEZONECOUNT (sizeof(g_timezoneinfo) / sizeof(g_timezoneinfo[0]))

static struct TZINFO g_timezoneinfo[] = {
    {
      _TIMEDISPLAY("GMT-12 Tokelau"),
      0,
      720,
      0,
      0,
      L"IDLW", // L"Dateline Standard Time",
      {  0, 0, 0, 0 },
      NULL,
      {  0, 0, 0, 0 },
    },

    {
      _TIMEDISPLAY("GMT-11 Samoa"),
      0,
      660,
      0,
      0,
      L"NT", // L"Samoa Standard Time",
      {  0, 0, 0, 0 },
      NULL,
      {  0, 0, 0, 0 },
    },

    {
      _TIMEDISPLAY("GMT-10 Hawaii"),
      0,
      600,
      0,
      0,
      L"HST", // L"Hawaiian Standard Time",
      {  0, 0, 0, 0 },
      NULL,
      {  0, 0, 0, 0 },
    },

    {
      _TIMEDISPLAY("GMT-09 Alaska"),
      1,
      540,
      0,
      -60,
      L"YST", // L"Alaskan Standard Time",
      { 10, 5, 0, 2 },
      L"YDT", // L"Alaskan Daylight Time",
      {  4, 1, 0, 2 },
    },

    {
      _TIMEDISPLAY("GMT-08 Pacific"),
      1,
      480,
      0,
      -60,
      L"PST", // L"Pacific Standard Time",
      { 10, 5, 0, 2 },
      L"PDT", // L"Pacific Daylight Time",
      {  4, 1, 0, 2 },
    },

    {
      _TIMEDISPLAY("GMT-07 Arizona"),
      0,
      420,
      0,
      0,
      L"MST", // L"US Mountain Standard Time",
      {  0, 0, 0, 0 },
      NULL,
      {  0, 0, 0, 0 },
    },

    {
      _TIMEDISPLAY("GMT-07 Mountain"),
      1,
      420,
      0,
      -60,
      L"MST", // L"Mountain Standard Time",
      { 10, 5, 0, 2 },
      L"MST", // L"Mountain Daylight Time",
      {  4, 1, 0, 2 },
    },

    {
      _TIMEDISPLAY("GMT-06 Central America"),
      0,
      360,
      0,
      0,
      L"CAST", // L"Central America Standard Time",
      {  0, 0, 0, 0 },
      NULL,
      {  0, 0, 0, 0 },
    },

    {
      _TIMEDISPLAY("GMT-06 Central"),
      1,
      360,
      0,
      -60,
      L"CST", // L"Central Standard Time",
      { 10, 5, 0, 2 },
      L"CDT", // L"Central Daylight Time",
      {  4, 1, 0, 2 },
    },

    {
      _TIMEDISPLAY("GMT-06 Mexico City"),
      1,
      360,
      0,
      -60,
      L"MST", // L"Mexico Standard Time",
      { 10, 5, 0, 2 },
      L"MDT", // L"Mexico Daylight Time",
      {  4, 1, 0, 2 },
    },

    {
      _TIMEDISPLAY("GMT-06 Saskatchewan"),
      0,
      360,
      0,
      0,
      L"CCST", // L"Canada Central Standard Time",
      {  0, 0, 0, 0 },
      NULL,
      {  0, 0, 0, 0 },
    },

    {
      _TIMEDISPLAY("GMT-05 Eastern"),
      1,
      300,
      0,
      -60,
      L"EST", // L"Eastern Standard Time",
      { 10, 5, 0, 2 },
      L"EDT", // L"Eastern Daylight Time",
      {  4, 1, 0, 2 },
    },

    {
      _TIMEDISPLAY("GMT-05 Indiana"),
      0,
      300,
      0,
      0,
      L"EST", // L"US Eastern Standard Time",
      {  0, 0, 0, 0 },
      NULL,
      {  0, 0, 0, 0 },
    },

    {
      _TIMEDISPLAY("GMT-05 Bogota"),
      0,
      300,
      0,
      0,
      L"SPST", // L"SA Pacific Standard Time",
      {  0, 0, 0, 0 },
      NULL,
      {  0, 0, 0, 0 },
    },

    {
      _TIMEDISPLAY("GMT-04 Atlantic"),
      1,
      240,
      0,
      -60,
      L"AST", // L"Atlantic Standard Time",
      { 10, 5, 0, 2 },
      L"ADT", // L"Atlantic Daylight Time",
      {  4, 1, 0, 2 },
    },

    {
      _TIMEDISPLAY("GMT-04 Caracas"),
      0,
      240,
      0,
      0,
      L"SWST", // L"SA Western Standard Time",
      {  0, 0, 0, 0 },
      NULL,
      {  0, 0, 0, 0 },
    },

    {
      _TIMEDISPLAY("GMT-04 Santiago"),
      1,
      240,
      0,
      -60,
      L"PSST", // L"Pacific SA Standard Time",
      {  3, 2, 6, 0 },
      L"PSDT", // L"Pacific SA Daylight Time",
      { 10, 2, 6, 0 },
    },

    {
      _TIMEDISPLAY("GMT-03:30 Newfoundland"),
      1,
      210,
      0,
      -60,
      L"NST", // L"Newfoundland Standard Time",
      { 10, 5, 0, 2 },
      L"NDT", // L"Newfoundland Daylight Time",
      {  4, 1, 0, 2 },
    },

    {
      _TIMEDISPLAY("GMT-03 Brasilia"),
      1,
      180,
      0,
      -60,
      L"ESST", // L"E. South America Standard Time",
      {  2, 2, 0, 2 },
      L"ESDT", // L"E. South America Daylight Time",
      { 10, 3, 0, 2 },
    },

    {
      _TIMEDISPLAY("GMT-03 Buenos Aires"),
      0,
      180,
      0,
      0,
      L"SEST", // L"SA Eastern Standard Time",
      {  0, 0, 0, 0 },
      NULL,
      {  0, 0, 0, 0 },
    },

    {
      _TIMEDISPLAY("GMT-03 Greenland"),
      1,
      180,
      0,
      -60,
      L"GST", // L"Greenland Standard Time",
      { 10, 5, 0, 2 },
      L"GDT", // L"Greenland Daylight Time",
      {  4, 1, 0, 2 },
    },

    {
      _TIMEDISPLAY("GMT-02 Mid-Atlantic"),
      1,
      120,
      0,
      -60,
      L"MAST", // L"Mid-Atlantic Standard Time",
      {  9, 5, 0, 2 },
      L"MADT", // L"Mid-Atlantic Daylight Time",
      {  3, 5, 0, 2 },
    },

    {
      _TIMEDISPLAY("GMT-01 Azores"),
      1,
      60,
      0,
      -60,
      L"AST", // L"Azores Standard Time",
      { 10, 5, 0, 3 },
      L"ADT", // L"Azores Daylight Time",
      {  3, 5, 0, 2 },
    },

    {
      _TIMEDISPLAY("GMT-01 Cape Verde Is."),
      0,
      60,
      0,
      0,
      L"WAT", // L"Cape Verde Standard Time",
      {  0, 0, 0, 0 },
      NULL,
      {  0, 0, 0, 0 },
    },

    {
      _TIMEDISPLAY("GMT+00 Casablanca"),
      0,
      0,
      0,
      0,
      L"GST", // L"Greenwich Standard Time",
      {  0, 0, 0, 0 },
      NULL,
      {  0, 0, 0, 0 },
    },

    {
      _TIMEDISPLAY("GMT+00 London"),
      1,
      0,
      0,
      -60,
      L"GMT", // L"GMT Standard Time",
      { 10, 5, 0, 2 },
      L"BST", // L"GMT Daylight Time",
      {  3, 5, 0, 1 },
    },

    {
      _TIMEDISPLAY("GMT+01 Berlin"),
      1,
      -60,
      0,
      -60,
      L"WEST", // L"W. Europe Standard Time",
      { 10, 5, 0, 3 },
      L"WEDT", // L"W. Europe Daylight Time",
      {  3, 5, 0, 2 },
    },

    {
      _TIMEDISPLAY("GMT+01 Belgrade"),
      1,
      -60,
      0,
      -60,
      L"CEST", // L"Central Europe Standard Time",
      { 10, 5, 0, 3 },
      L"CEDT", // L"Central Europe Daylight Time",
      {  3, 5, 0, 2 },
    },

    {
      _TIMEDISPLAY("GMT+01 Paris"),
      1,
      -60,
      0,
      -60,
      L"RST", // L"Romance Standard Time",
      { 10, 5, 0, 3 },
      L"RDT", // L"Romance Daylight Time",
      {  3, 5, 0, 2 },
    },

    {
      _TIMEDISPLAY("GMT+01 Sarajevo"),
      1,
      -60,
      0,
      -60,
      L"SCST", // L"Central European Standard Time",
      { 10, 5, 0, 3 },
      L"SCDT", // L"Central European Daylight Time",
      {  3, 5, 0, 2 },
    },

    {
      _TIMEDISPLAY("GMT+01 W. Central Africa"),
      0,
      -60,
      0,
      0,
      L"WAST", // L"W. Central Africa Standard Time",
      {  0, 0, 0, 0 },
      NULL,
      {  0, 0, 0, 0 },
    },

    {
      _TIMEDISPLAY("GMT+02 Athens"),
      1,
      -120,
      0,
      -60,
      L"GTST", // L"GTB Standard Time",
      { 10, 5, 0, 3 },
      L"GTDT", // L"GTB Daylight Time",
      {  3, 5, 0, 2 },
    },

    {
      _TIMEDISPLAY("GMT+02 Bucharest"),
      1,
      -120,
      0,
      -60,
      L"EEST", // L"E. Europe Standard Time",
      {  9, 5, 0, 1 },
      L"EEDT", // L"E. Europe Daylight Time",
      {  3, 5, 0, 0 },
    },

    {
      _TIMEDISPLAY("GMT+02 Cairo"),
      1,
      -120,
      0,
      -60,
      L"EST", // L"Egypt Standard Time",
      {  9, 5, 3, 2 },
      L"EDT", // L"Egypt Daylight Time",
      {  5, 1, 5, 2 },
    },

    {
      _TIMEDISPLAY("GMT+02 Pretoria"),
      0,
      -120,
      0,
      0,
      L"SAST", // L"South Africa Standard Time",
      {  0, 0, 0, 0 },
      NULL,
      {  0, 0, 0, 0 },
    },

    {
      _TIMEDISPLAY("GMT+02 Helsinki"),
      1,
      -120,
      0,
      -60,
      L"FLST", // L"FLE Standard Time",
      { 10, 5, 0, 4 },
      L"FLDT", // L"FLE Daylight Time",
      {  3, 5, 0, 3 },
    },

    {
      _TIMEDISPLAY("GMT+02 Jerusalem"),
      0,
      -120,
      0,
      0,
      L"JST", // L"Jerusalem Standard Time",
      {  0, 0, 0, 0 },
      NULL,
      {  0, 0, 0, 0 },
    },

    {
      _TIMEDISPLAY("GMT+03 Baghdad"),
      1,
      -180,
      0,
      -60,
      L"AST", // L"Arabic Standard Time",
      { 10, 1, 0, 4 },
      L"ADT", // L"Arabic Daylight Time",
      {  4, 1, 0, 3 },
    },

    {
      _TIMEDISPLAY("GMT+03 Kuwait"),
      0,
      -180,
      0,
      0,
      L"AST", // L"Arab Standard Time",
      {  0, 0, 0, 0 },
      NULL,
      {  0, 0, 0, 0 },
    },

    {
      _TIMEDISPLAY("GMT+03 Moscow"),
      1,
      -180,
      0,
      -60,
      L"RST", // L"Russian Standard Time",
      { 10, 5, 0, 3 },
      L"RDT", // L"Russian Daylight Time",
      {  3, 5, 0, 2 },
    },

    {
      _TIMEDISPLAY("GMT+03 Nairobi"),
      0,
      -180,
      0,
      0,
      L"EAST", // L"E. Africa Standard Time",
      {  0, 0, 0, 0 },
      NULL,
      {  0, 0, 0, 0 },
    },

    {
      _TIMEDISPLAY("GMT+03:30 Tehran"),
      1,
      -210,
      0,
      -60,
      L"IST", // L"Iran Standard Time",
      {  9, 4, 2, 2 },
      L"IDT", // L"Iran Daylight Time",
      {  3, 1, 0, 2 },
    },

    {
      _TIMEDISPLAY("GMT+04 Abu Dhabi"),
      0,
      -240,
      0,
      0,
      L"AST", // L"Arabian Standard Time",
      {  0, 0, 0, 0 },
      NULL,
      {  0, 0, 0, 0 },
    },

    {
      _TIMEDISPLAY("GMT+04 Baku"),
      1,
      -240,
      0,
      -60,
      L"CST", // L"Caucasus Standard Time",
      { 10, 5, 0, 3 },
      L"CDT", // L"Caucasus Daylight Time",
      {  3, 5, 0, 2 },
    },

    {
      _TIMEDISPLAY("GMT+04:30 Kabul"),
      0,
      -270,
      0,
      0,
      L"AST", // L"Afghanistan Standard Time",
      {  0, 0, 0, 0 },
      NULL,
      {  0, 0, 0, 0 },
    },

    {
      _TIMEDISPLAY("GMT+05 Ekaterinburg"),
      1,
      -300,
      0,
      -60,
      L"EST", // L"Ekaterinburg Standard Time",
      { 10, 5, 0, 3 },
      L"EDT", // L"Ekaterinburg Daylight Time",
      {  3, 5, 0, 2 },
    },

    {
      _TIMEDISPLAY("GMT+05 Islamabad"),
      0,
      -300,
      0,
      0,
      L"WAST", // L"West Asia Standard Time",
      {  0, 0, 0, 0 },
      NULL,
      {  0, 0, 0, 0 },
    },

    {
      _TIMEDISPLAY("GMT+05:30 New Delhi"),
      0,
      -330,
      0,
      0,
      L"IST", // L"India Standard Time",
      {  0, 0, 0, 0 },
      NULL,
      {  0, 0, 0, 0 },
    },

    {
      _TIMEDISPLAY("GMT+05:45 Kathmandu"),
      0,
      -345,
      0,
      0,
      L"NST", // L"Nepal Standard Time",
      {  0, 0, 0, 0 },
      NULL,
      {  0, 0, 0, 0 },
    },

    {
      _TIMEDISPLAY("GMT+06 Almaty"),
      1,
      -360,
      0,
      -60,
      L"NCST", // L"N. Central Asia Standard Time",
      { 10, 5, 0, 3 },
      L"NCDT", // L"N. Central Asia Daylight Time",
      {  3, 5, 0, 2 },
    },

    {
      _TIMEDISPLAY("GMT+06 Dhaka"),
      0,
      -360,
      0,
      0,
      L"CAST", // L"Central Asia Standard Time",
      {  0, 0, 0, 0 },
      NULL,
      {  0, 0, 0, 0 },
    },

    {
      _TIMEDISPLAY("GMT+06 Sri Lanka"),
      0,
      -360,
      0,
      0,
      L"SRST", // L"Sri Lanka Standard Time",
      {  0, 0, 0, 0 },
      NULL,
      {  0, 0, 0, 0 },
    },

    {
      _TIMEDISPLAY("GMT+06:30 Yangon"),
      0,
      -390,
      0,
      0,
      L"MST", // L"Myanmar Standard Time",
      {  0, 0, 0, 0 },
      NULL,
      {  0, 0, 0, 0 },
    },

    {
      _TIMEDISPLAY("GMT+07 Bangkok"),
      0,
      -420,
      0,
      0,
      L"SAST", // L"SE Asia Standard Time",
      {  0, 0, 0, 0 },
      NULL,
      {  0, 0, 0, 0 },
    },

    {
      _TIMEDISPLAY("GMT+07 Krasnoyarsk"),
      1,
      -420,
      0,
      -60,
      L"NAST", // L"North Asia Standard Time",
      { 10, 5, 0, 3 },
      L"NADT", // L"North Asia Daylight Time",
      {  3, 5, 0, 2 },
    },

    {
      _TIMEDISPLAY("GMT+08 Beijing"),
      0,
      -480,
      0,
      0,
      L"CST", // L"China Standard Time",
      {  0, 0, 0, 0 },
      NULL,
      {  0, 0, 0, 0 },
    },

    {
      _TIMEDISPLAY("GMT+08 Irkutsk"),
      1,
      -480,
      0,
      -60,
      L"NEST", // L"North Asia East Standard Time",
      { 10, 5, 0, 3 },
      L"NEDT", // L"North Asia East Daylight Time",
      {  3, 5, 0, 2 },
    },

    {
      _TIMEDISPLAY("GMT+08 Singapore"),
      0,
      -480,
      0,
      0,
      L"MPST", // L"Malay Peninsula Standard Time",
      {  0, 0, 0, 0 },
      NULL,
      {  0, 0, 0, 0 },
    },

    {
      _TIMEDISPLAY("GMT+08 Perth"),
      0,
      -480,
      0,
      0,
      L"AWST", // L"W. Australia Standard Time",
      {  0, 0, 0, 0 },
      NULL,
      {  0, 0, 0, 0 },
    },

    {
      _TIMEDISPLAY("GMT+08 Taipei"),
      0,
      -480,
      0,
      0,
      L"TST", // L"Taipei Standard Time",
      {  0, 0, 0, 0 },
      NULL,
      {  0, 0, 0, 0 },
    },

    {
      _TIMEDISPLAY("GMT+09 Tokyo"),
      0,
      -540,
      0,
      0,
      L"TST", // L"Tokyo Standard Time",
      {  0, 0, 0, 0 },
      NULL,
      {  0, 0, 0, 0 },
    },

    {
      _TIMEDISPLAY("GMT+09 Seoul"),
      0,
      -540,
      0,
      0,
      L"KST", // L"Korea Standard Time",
      {  0, 0, 0, 0 },
      NULL,
      {  0, 0, 0, 0 },
    },

    {
      _TIMEDISPLAY("GMT+09 Yakutsk"),
      1,
      -540,
      0,
      -60,
      L"YST", // L"Yakutsk Standard Time",
      { 10, 5, 0, 3 },
      L"YDT", // L"Yakutsk Daylight Time",
      {  3, 5, 0, 2 },
    },

    {
      _TIMEDISPLAY("GMT+09:30 Adelaide"),
      1,
      -570,
      0,
      -60,
      L"ACST", // L"Cen. Australia Standard Time",
      {  3, 5, 0, 2 },
      L"ACDT", // L"Cen. Australia Daylight Time",
      { 10, 5, 0, 2 },
    },

    {
      _TIMEDISPLAY("GMT+09:30 Darwin"),
      0,
      -570,
      0,
      0,
      L"ACST", // L"AUS Central Standard Time",
      {  0, 0, 0, 0 },
      NULL,
      {  0, 0, 0, 0 },
    },

    {
      _TIMEDISPLAY("GMT+10 Brisbane"),
      0,
      -600,
      0,
      0,
      L"AEST", // L"E. Australia Standard Time",
      {  0, 0, 0, 0 },
      NULL,
      {  0, 0, 0, 0 },
    },

    {
      _TIMEDISPLAY("GMT+10 Sydney"),
      1,
      -600,
      0,
      -60,
      L"AEST", // L"AUS Eastern Standard Time",
      {  3, 5, 0, 2 },
      L"AEDT", // L"AUS Eastern Daylight Time",
      { 10, 5, 0, 2 },
    },

    {
      _TIMEDISPLAY("GMT+10 Guam"),
      0,
      -600,
      0,
      0,
      L"WPST", // L"West Pacific Standard Time",
      {  0, 0, 0, 0 },
      NULL,
      {  0, 0, 0, 0 },
    },

    {
      _TIMEDISPLAY("GMT+10 Hobart"),
      1,
      -600,
      0,
      -60,
      L"TST", // L"Tasmania Standard Time",
      {  3, 5, 0, 2 },
      L"TDT", // L"Tasmania Daylight Time",
      { 10, 1, 0, 2 },
    },

    {
      _TIMEDISPLAY("GMT+10 Vladivostok"),
      1,
      -600,
      0,
      -60,
      L"VST", // L"Vladivostok Standard Time",
      { 10, 5, 0, 3 },
      L"VDT", // L"Vladivostok Daylight Time",
      {  3, 5, 0, 2 },
    },

    {
      _TIMEDISPLAY("GMT+11 Solomon Islands"),
      0,
      -660,
      0,
      0,
      L"CPST", // L"Central Pacific Standard Time",
      {  0, 0, 0, 0 },
      NULL,
      {  0, 0, 0, 0 },
    },

    {
      _TIMEDISPLAY("GMT+12 Auckland"),
      1,
      -720,
      0,
      -60,
      L"NZST", // L"New Zealand Standard Time",
      {  3, 3, 0, 2 },
      L"NZDT", // L"New Zealand Daylight Time",
      { 10, 1, 0, 2 },
    },

    {
      _TIMEDISPLAY("GMT+12 Fiji Islands"),
      0,
      -720,
      0,
      0,
      L"FST", // L"Fiji Standard Time",
      {  0, 0, 0, 0 },
      NULL,
      {  0, 0, 0, 0 },
    },

    {
      _TIMEDISPLAY("GMT+13 Nuku'alofa"),
      0,
      -780,
      0,
      0,
      L"TST", // L"Tonga Standard Time",
      {  0, 0, 0, 0 },
      NULL,
      {  0, 0, 0, 0 },
    },

    {
      _TIMEDISPLAY("GMT+14 Kiribati"),
      0,
      -840,
      0,
      0,
      L"KST", // L"Kiribati Standard Time",
      {  0, 0, 0, 0 },
      NULL,
      {  0, 0, 0, 0 },
    },
};



#endif // _XSETTINGS_H_
