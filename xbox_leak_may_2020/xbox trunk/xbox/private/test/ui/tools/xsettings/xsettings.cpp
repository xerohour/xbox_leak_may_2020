/*****************************************************
*** xsettings.cpp
***
*** CPP file for our XSettings class.
*** This is class which will hold the settings
*** that live on the Xbox.
***
*** by James N. Helm
*** December 2nd, 2000
***
*****************************************************/

#include "xsetpch.h"
#include "xsettings.h"

DWORD Crc32Checksum(const VOID* buf, DWORD bufsize);    // Computer a CRC32 Checksum
BOOL HDSetPassword( BYTE* password, DWORD dwLen );      // Set the password of the Hard Drive

//
// Functions for dealing with the encrypted section of the EEPROM
//

//
// The EEPROM encrypted section is unencrypted if the hard drive key field is all 0's
//
inline BOOL IsEEPROMEncryptedSectionUnencrypted(const XBOX_ENCRYPTED_SETTINGS* encryptedSection)
{
    /*
    // Use the HDKey to determine if we are encrypted
    const UCHAR* hdkey = encryptedSection->HDKey;
    for (INT i=0; i < XBOX_KEY_LENGTH; i++)
        if (hdkey[i] != 0) return FALSE;
    */

    // Use the Confounder to determine if we are encrypted
    const UCHAR* confounder = encryptedSection->Confounder;
    for (INT i=0; i < 8; i++)
        if (confounder[i] != 0) return FALSE;

    return TRUE;
}

//
// Decrypted the encrypted section of the EEPROM
//
static DWORD DecryptEEPROMEncryptedSection(XBOX_ENCRYPTED_SETTINGS* encryptedSection)
{
    if (IsEEPROMEncryptedSectionUnencrypted(encryptedSection))
        return ERROR_SUCCESS;

    #define RC4_CONFOUNDER_LEN FIELD_SIZE(XBOX_ENCRYPTED_SETTINGS, Confounder)

    UCHAR Checksum[XC_SERVICE_DIGEST_SIZE];
    UCHAR Confounder[RC4_CONFOUNDER_LEN];
    UCHAR LocalKey[XC_SERVICE_DIGEST_SIZE];
    UCHAR Rc4KeyStruct[XC_SERVICE_RC4_KEYSTRUCT_SIZE];
    UCHAR* Data;
    ULONG DataSize;
    XBOX_ENCRYPTED_SETTINGS EncryptedSettings;
    UCHAR* EepromKey = (UCHAR*) *XboxEEPROMKey;

    EncryptedSettings = *encryptedSection;
    Data = EncryptedSettings.HDKey;
    DataSize = EEPROM_ENCRYPTED_SECTION_SIZE - FIELD_OFFSET(XBOX_ENCRYPTED_SETTINGS, HDKey);
    memcpy(Confounder, EncryptedSettings.Confounder, RC4_CONFOUNDER_LEN);

    //
    // HMAC the checksum into the key
    //
    XcHMAC(
        EepromKey,
        XBOX_KEY_LENGTH,
        EncryptedSettings.Checksum,
        XC_SERVICE_DIGEST_SIZE,
        NULL,
        0,
        LocalKey);

    //
    // Use the generated key as the RC4 encryption key
    //
    XcRC4Key(Rc4KeyStruct, XC_SERVICE_DIGEST_SIZE, LocalKey);

    //
    // Decrypt confounder and data
    //
    XcRC4Crypt(Rc4KeyStruct, RC4_CONFOUNDER_LEN, Confounder);
    XcRC4Crypt(Rc4KeyStruct, DataSize, Data);

    //
    // Now verify the checksum.
    //
    XcHMAC(
        EepromKey,
        XBOX_KEY_LENGTH,
        Confounder,
        RC4_CONFOUNDER_LEN,
        Data,
        DataSize,
        Checksum);

    //
    // Decrypt is successful only if checksum matches
    //
    if (memcmp(Checksum, EncryptedSettings.Checksum, XC_SERVICE_DIGEST_SIZE) != 0)
        return ERROR_INVALID_PASSWORD;

    memset(encryptedSection, 0, sizeof(*encryptedSection));
    memcpy(encryptedSection->HDKey, EncryptedSettings.HDKey, XBOX_KEY_LENGTH);
    encryptedSection->GameRegion = EncryptedSettings.GameRegion;
    return ERROR_SUCCESS;
}

//
// Encrypt the encrypted section of the EEPROM
//
static DWORD EncryptEEPROMEncryptedSection(XBOX_ENCRYPTED_SETTINGS* encryptedSection)
{
    if (!IsEEPROMEncryptedSectionUnencrypted(encryptedSection))
        return ERROR_SUCCESS;

    UCHAR Confounder[RC4_CONFOUNDER_LEN] = { 0x4c,0x70,0x33,0xcb,0x5b,0xb5,0x97,0xd2 };
    UCHAR LocalKey[XC_SERVICE_DIGEST_SIZE];
    UCHAR Rc4KeyStruct[XC_SERVICE_RC4_KEYSTRUCT_SIZE];
    UCHAR* Data;
    ULONG DataSize;
    UCHAR* EepromKey = (UCHAR*) *XboxEEPROMKey;

    Data = encryptedSection->HDKey;
    DataSize = EEPROM_ENCRYPTED_SECTION_SIZE - FIELD_OFFSET(XBOX_ENCRYPTED_SETTINGS, HDKey);

    //
    // Create the header - the confounder & checksum
    //
    memset(encryptedSection->Checksum, 0, sizeof(encryptedSection->Checksum));
    memcpy(encryptedSection->Confounder, Confounder, sizeof(encryptedSection->Confounder));

    //
    // Checksum everything but the checksum
    //
    XcHMAC(EepromKey,
           XBOX_KEY_LENGTH,
           encryptedSection->Confounder,
           RC4_CONFOUNDER_LEN,
           Data,
           DataSize,
           encryptedSection->Checksum);

    //
    // HMAC the checksum into the key
    //
    XcHMAC(EepromKey,
           XBOX_KEY_LENGTH,
           encryptedSection->Checksum,
           XC_SERVICE_DIGEST_SIZE,
           NULL,
           0,
           LocalKey);

    //
    // Use the generated key as the RC4 encryption key
    //
    XcRC4Key(Rc4KeyStruct, XC_SERVICE_DIGEST_SIZE, LocalKey);

    //
    // Encrypt everything but the checksum
    //
    XcRC4Crypt(Rc4KeyStruct, RC4_CONFOUNDER_LEN, encryptedSection->Confounder);
    XcRC4Crypt(Rc4KeyStruct, DataSize, Data);

    return ERROR_SUCCESS;
}

//
// Make sure the encrypted section of the EEPROM is encrypted or decrypted
//
static DWORD EncryptDecryptEEPROMEncryptedSection(BOOL encrypted)
{
    EEPROM_LAYOUT eepromData;
    XBOX_ENCRYPTED_SETTINGS* encryptedSection;
    ULONG type, length;
    DWORD err;

    //
    // Read the existing content of the encrypted section
    //
    err = XQueryValue(XC_MAX_ALL, &type, &eepromData, sizeof(eepromData), &length);
    if (err != ERROR_SUCCESS) return err;

    encryptedSection = (XBOX_ENCRYPTED_SETTINGS*) eepromData.EncryptedSection;
    if (encrypted) {
        // If the current content is already encrypted, do nothing
        if (!IsEEPROMEncryptedSectionUnencrypted(encryptedSection))
            return ERROR_SUCCESS;
        err = EncryptEEPROMEncryptedSection(encryptedSection);
    } else {
        if (IsEEPROMEncryptedSectionUnencrypted(encryptedSection))
            return ERROR_SUCCESS;
        err = DecryptEEPROMEncryptedSection(encryptedSection);
    }

    XDBGWRN("EEPROM", "%scrypting the encrypted section of the EEPROM", encrypted ? "En" : "De");

    if (err == ERROR_SUCCESS) {
        err = XSetValue(XC_MAX_ALL, REG_BINARY, &eepromData, sizeof(eepromData));
    }

    if (err != ERROR_SUCCESS) {
        XDBGWRN("EEPROM", "Failed to update EEPROM content (error = %d)",  err);
    }
    return err;
}

// Constructor
CXSettings::CXSettings()
: bInitialized( FALSE ),
m_pUserSettings( NULL ),
m_pFactorySettings( NULL ),
m_pEncSettings( NULL ),
m_pszIPAddress( NULL ),
m_pszDebugIPAddress( NULL ),
m_pszPrimaryDNS( NULL ),
m_pszSecondaryDNS( NULL ),
m_pszSubnetMask( NULL ),
m_pszGateway( NULL ),
m_pszDisplaySetting( NULL ),
m_pszMachineName( NULL )
{
    XDBGTRC( XSETTINGS_APP_TITLE_NAME_A, "CXSettings::CXSettings()" );

    m_pszMachineName = new char[XSETTINGS_MACHINENAME_LENGTH + 1];
    m_pszDisplaySetting = new char[XSETTINGS_DISPLAYSETTING_TEXT_LENGTH + 1];
    if( ( !m_pszMachineName ) || ( !m_pszDisplaySetting ) )
        XDBGERR( XSETTINGS_APP_TITLE_NAME_A, "***CXSettings::CXSettings():Failed to allocate memory!!" );
    else
    {
        ZeroMemory( m_pszMachineName, XSETTINGS_MACHINENAME_LENGTH + 1 );
        ZeroMemory( m_pszDisplaySetting, XSETTINGS_DISPLAYSETTING_TEXT_LENGTH + 1 );
    }

    // Clear out our Memory
    ZeroMemory( &m_AllSettings, sizeof( m_AllSettings ) );
    ZeroMemory( &m_PrevAllSettings, sizeof( m_PrevAllSettings ) );

    m_pUserSettings = (XBOX_USER_SETTINGS*)m_AllSettings.UserConfigSection;
    m_pFactorySettings = (XBOX_FACTORY_SETTINGS*)m_AllSettings.FactorySection;
    m_pEncSettings = (XBOX_ENCRYPTED_SETTINGS*)m_AllSettings.EncryptedSection;

    if( FAILED( Init() ) )
    {
        XDBGWRN( XSETTINGS_APP_TITLE_NAME_A, "CXSettings::CXSettings():Failed to Initialized and Load Settings!!" );
    }
}  


// Destructor
CXSettings::~CXSettings()
{
    XDBGTRC( XSETTINGS_APP_TITLE_NAME_A, "CXSettings::~CXSettings()" );

    if( m_pszIPAddress != NULL )
    {
        delete[] m_pszIPAddress;
        m_pszIPAddress = NULL;
    }
    
    if( m_pszDebugIPAddress != NULL  )
    {
        delete[] m_pszDebugIPAddress;
        m_pszDebugIPAddress = NULL;
    }

	if( m_pszPrimaryDNS != NULL  )
	{
		delete[] m_pszPrimaryDNS;
		m_pszPrimaryDNS = NULL;
	}

	if( m_pszSecondaryDNS != NULL  )
	{
		delete[] m_pszSecondaryDNS;
		m_pszSecondaryDNS = NULL;
	}
    
    if( m_pszSubnetMask != NULL  )
    {
        delete[] m_pszSubnetMask;
        m_pszSubnetMask = NULL;
    }

    if( m_pszGateway != NULL  )
    {
        delete[] m_pszGateway;
        m_pszGateway = NULL;
    }
    
    if( m_pszMachineName != NULL  )
    {
        delete[] m_pszMachineName;
        m_pszMachineName = NULL;
    }

    if( m_pszDisplaySetting != NULL  )
    {
        delete[] m_pszDisplaySetting;
        m_pszDisplaySetting = NULL;
    }
}


// Initialize our object
HRESULT CXSettings::Init()
{
    XDBGTRC( XSETTINGS_APP_TITLE_NAME_A, "CXSettings::Init()" );

    HRESULT hr = S_OK;

    /////////////////////////
    // Initialize EEPROM Key
    /////////////////////////
    if( 0 == memcmp( XboxEEPROMKey, (void*)"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0", XBOX_KEY_LENGTH ) )
    {
        XDBGWRN( XSETTINGS_APP_TITLE_NAME_A, "Init():EEPROM Key was all Zeros... changing" );

        memcpy( (void*)XboxEEPROMKey, SETTINGS_EEPROM_DEVKIT_KEY, XBOX_KEY_LENGTH );
    }

    // Load our settings in to our variables
    if( FAILED( hr = LoadSettings() ) )
        XDBGERR( XSETTINGS_APP_TITLE_NAME_A, "CXSettings::Init():LoadSettings() failed!! - '%X'", hr );

    return hr;
}


// Get the Xbox IP address
char* CXSettings::GetIPAddress()
{
    // Free our memory
    if( m_pszIPAddress )
    {
        delete[] m_pszIPAddress;
        m_pszIPAddress = NULL;
    }

    // Get the new IP Address
    m_pszIPAddress = StringFromIPAddress( m_XNetConfigParams.ina.s_addr );

    return m_pszIPAddress;
}

// Get the Xbox Debug IP address
char* CXSettings::GetDebugIPAddress()
{
    // Free our memory
    if( m_pszDebugIPAddress )
    {
        delete[] m_pszDebugIPAddress;
        m_pszDebugIPAddress = NULL;
    }

    // Get the Debug IP Address
    m_pszDebugIPAddress = StringFromIPAddress( (ULONG)DmTell(DMTELL_GETDBGIP, NULL) );

    return m_pszDebugIPAddress;
}


// Get the Xbox Subnet mask
char* CXSettings::GetSubnetMask()
{
    // Free our memory
    if( m_pszSubnetMask )
    {
        delete[] m_pszSubnetMask;
        m_pszSubnetMask = NULL;
    }

    // Get the new Subnet Mask
    m_pszSubnetMask = StringFromIPAddress( m_XNetConfigParams.inaMask.s_addr );
    
    return m_pszSubnetMask;
}


// Get the Xbox Subnet mask
char* CXSettings::GetGateway()
{
    // Free our memory
    if( m_pszGateway )
    {
        delete[] m_pszGateway;
        m_pszGateway = NULL;
    }

    // Get the new Gateway Address
    m_pszGateway = StringFromIPAddress( m_XNetConfigParams.inaGateway.s_addr );
    
    return m_pszGateway;
}

// Get the Xbox Primary DNS address
char* CXSettings::GetPrimaryDNS()
{
    // Free our memory
    if( m_pszPrimaryDNS )
    {
        delete[] m_pszPrimaryDNS;
        m_pszPrimaryDNS = NULL;
    }

    // Get the new Gateway Address
    m_pszPrimaryDNS = StringFromIPAddress( m_XNetConfigParams.inaDnsPrimary.s_addr );
    
    return m_pszPrimaryDNS;
}


// Get the Xbox Secondary DNS address
char* CXSettings::GetSecondaryDNS()
{
    // Free our memory
    if( m_pszSecondaryDNS )
    {
        delete[] m_pszSecondaryDNS;
        m_pszSecondaryDNS = NULL;
    }

    // Get the new Gateway Address
    m_pszSecondaryDNS = StringFromIPAddress( m_XNetConfigParams.inaDnsSecondary.s_addr );
    
    return m_pszSecondaryDNS;
}


// Get the Xbox Machine name
char* CXSettings::GetMachineName()
{
    // Zero out the machine name
    DWORD mnSize = XSETTINGS_MACHINENAME_LENGTH + 1;
    ZeroMemory( m_pszMachineName, mnSize );

    // Get Machine Name
    if( FAILED( DmGetXboxName( m_pszMachineName, &mnSize ) ) )
    {
        XDBGWRN( XSETTINGS_APP_TITLE_NAME_A, "CXSettings::GetMachineName():failed to get the Machine Name!!" );
    }

    return m_pszMachineName;
}


// Will compare the latest settings to the previous settings, and determine if they have changed
BOOL CXSettings::SettingsHaveChanged()
{
    // Check the Settings
    if( 0 != memcmp( &m_PrevAllSettings, &m_AllSettings, sizeof( m_PrevAllSettings ) ) )
    {
        XDBGTRC( XSETTINGS_APP_TITLE_NAME_A, "CXSettings::SettingsHaveChanged():Settings Changed!!" );

        return TRUE;
    }

    return FALSE;
}


// Load the settings in to our variables from the settings file
HRESULT CXSettings::LoadSettings()
{
    XDBGTRC( XSETTINGS_APP_TITLE_NAME_A, "CXSettings::LoadSettings()" );

    // Store the previous settings
    memcpy( &m_PrevAllSettings, &m_AllSettings, sizeof( m_PrevAllSettings ) );

    // Get ALL EEPROM Settings
    DWORD dwType;
    HRESULT hr = S_OK;
    if( FAILED( XQueryValue( XC_MAX_ALL, &dwType, &m_AllSettings, sizeof( m_AllSettings ), NULL ) ) )
    {
        XDBGWRN( XSETTINGS_APP_TITLE_NAME_A, "CXSettings::LoadSettings():failed to get All EEPROM Settings!! - '0x%.8X (%d)'", hr, hr );

        hr = E_FAIL;
    }

    // Get XNet config settings
    XNetLoadConfigParams(&m_XNetConfigParams);

    // The in-memory representation of the encrypted section is always unencrypted
    if( DecryptEEPROMEncryptedSection( m_pEncSettings ) != ERROR_SUCCESS )
    {
        XDBGWRN( XSETTINGS_APP_TITLE_NAME_A, "CXSettings::LoadSettings():Failed to decrypt the EEPROM!!" );
        
        hr = E_FAIL;
    }

    return hr;
}


// Enable or Disable 1080i
HRESULT CXSettings::Set1080i( BOOL bEnable )
{
    DWORD dw1080iSetting = m_pUserSettings->VideoFlags;

    if( bEnable )
    {
        dw1080iSetting |= AV_FLAGS_HDTV_1080i;
    }
    else
    {
        dw1080iSetting &= ~AV_FLAGS_HDTV_1080i;
    }

    // Store the DTS Setting in to the Xbox EEPROM
    HRESULT hr = S_OK;
    if( FAILED( hr = XSetValue( XC_VIDEO_FLAGS, REG_DWORD, &dw1080iSetting, sizeof( dw1080iSetting ) ) ) )
    {
        XDBGWRN( XSETTINGS_APP_TITLE_NAME_A, "CXSettings::Set1080i():Failed to set 1080i!! - '0x%.8X (%d)'", hr, hr );
    }

    // Load the updated settings
    HRESULT hr2 = S_OK;
    if( FAILED( hr2 = LoadSettings() ) )
    {
        XDBGWRN( XSETTINGS_APP_TITLE_NAME_A, "CXSettings::Set1080i():There was a problem reloading the settings!! - '0x.8X (%d)'", hr2, hr2 );
    }

    return hr;
}


// Enable or Disable 720p
HRESULT CXSettings::Set720p( BOOL bEnable )
{
    DWORD dw720pSetting = m_pUserSettings->VideoFlags;

    if( bEnable )
    {
        dw720pSetting |= AV_FLAGS_HDTV_720p;
    }
    else
    {
        dw720pSetting &= ~AV_FLAGS_HDTV_720p;
    }

    // Store the DTS Setting in to the Xbox EEPROM
    HRESULT hr = S_OK;
    if( FAILED( hr = XSetValue( XC_VIDEO_FLAGS, REG_DWORD, &dw720pSetting, sizeof( dw720pSetting ) ) ) )
    {
        XDBGWRN( XSETTINGS_APP_TITLE_NAME_A, "CXSettings::Set720p():Failed to set 720p!! - '0x%.8X (%d)'", hr, hr );
    }

    // Load the updated settings
    HRESULT hr2 = S_OK;
    if( FAILED( hr2 = LoadSettings() ) )
    {
        XDBGWRN( XSETTINGS_APP_TITLE_NAME_A, "CXSettings::Set720p():There was a problem reloading the settings!! - '0x.8X (%d)'", hr2, hr2 );
    }

    return hr;
}


// Enable or Disable 480p
HRESULT CXSettings::Set480p( BOOL bEnable )
{
    DWORD dw480pSetting = m_pUserSettings->VideoFlags;

    if( bEnable )
    {
        dw480pSetting |= AV_FLAGS_HDTV_480p;
    }
    else
    {
        dw480pSetting &= ~AV_FLAGS_HDTV_480p;
    }

    // Store the DTS Setting in to the Xbox EEPROM
    HRESULT hr = S_OK;
    if( FAILED( hr = XSetValue( XC_VIDEO_FLAGS, REG_DWORD, &dw480pSetting, sizeof( dw480pSetting ) ) ) )
    {
        XDBGWRN( XSETTINGS_APP_TITLE_NAME_A, "CXSettings::Set480p():Failed to set 480p!! - '0x%.8X (%d)'", hr, hr );
    }

    // Load the updated settings
    HRESULT hr2 = S_OK;
    if( FAILED( hr2 = LoadSettings() ) )
    {
        XDBGWRN( XSETTINGS_APP_TITLE_NAME_A, "CXSettings::Set480p():There was a problem reloading the settings!! - '0x.8X (%d)'", hr2, hr2 );
    }

    return hr;
}


// Enable or Disable PAL 60
HRESULT CXSettings::SetPAL60( BOOL bEnable )
{
    DWORD dwVideoSetting = m_pUserSettings->VideoFlags;

    if( bEnable )
    {
        dwVideoSetting |= AV_FLAGS_60Hz;
    }
    else
    {
        dwVideoSetting &= ~AV_FLAGS_60Hz;
    }

    // Store the DTS Setting in to the Xbox EEPROM
    HRESULT hr = S_OK;
    if( FAILED( hr = XSetValue( XC_VIDEO_FLAGS, REG_DWORD, &dwVideoSetting, sizeof( dwVideoSetting ) ) ) )
    {
        XDBGWRN( XSETTINGS_APP_TITLE_NAME_A, "CXSettings::SetPAL60():Failed to set PAL60!! - '0x%.8X (%d)'", hr, hr );
    }

    // Load the updated settings
    HRESULT hr2 = S_OK;
    if( FAILED( hr2 = LoadSettings() ) )
    {
        XDBGWRN( XSETTINGS_APP_TITLE_NAME_A, "CXSettings::SetPAL60():There was a problem reloading the settings!! - '0x.8X (%d)'", hr2, hr2 );
    }

    return hr;
}


// Enable or Disable Widescreen
HRESULT CXSettings::SetWidescreen( BOOL bEnable )
{
    DWORD dwVideoSetting = m_pUserSettings->VideoFlags;

    dwVideoSetting &= ~(AV_FLAGS_WIDESCREEN | AV_FLAGS_LETTERBOX);

    if( bEnable )
    {
        dwVideoSetting |= AV_FLAGS_WIDESCREEN;
    }

    // Store the DTS Setting in to the Xbox EEPROM
    HRESULT hr = S_OK;
    if( FAILED( hr = XSetValue( XC_VIDEO_FLAGS, REG_DWORD, &dwVideoSetting, sizeof( dwVideoSetting ) ) ) )
    {
        XDBGWRN( XSETTINGS_APP_TITLE_NAME_A, "CXSettings::SetWidescreen():Failed to set Widescreen!! - '0x%.8X (%d)'", hr, hr );
    }

    // Load the updated settings
    HRESULT hr2 = S_OK;
    if( FAILED( hr2 = LoadSettings() ) )
    {
        XDBGWRN( XSETTINGS_APP_TITLE_NAME_A, "CXSettings::SetWidescreen():There was a problem reloading the settings!! - '0x.8X (%d)'", hr2, hr2 );
    }

    return hr;
}


// Enable or Disable Letterbox
HRESULT CXSettings::SetLetterbox( BOOL bEnable )
{
    DWORD dwVideoSetting = m_pUserSettings->VideoFlags;

    dwVideoSetting &= ~(AV_FLAGS_WIDESCREEN | AV_FLAGS_LETTERBOX);

    if( bEnable )
    {
        dwVideoSetting |= AV_FLAGS_LETTERBOX;
    }

    // Store the DTS Setting in to the Xbox EEPROM
    HRESULT hr = S_OK;
    if( FAILED( hr = XSetValue( XC_VIDEO_FLAGS, REG_DWORD, &dwVideoSetting, sizeof( dwVideoSetting ) ) ) )
    {
        XDBGWRN( XSETTINGS_APP_TITLE_NAME_A, "CXSettings::SetLetterbox():Failed to set Letterbox!! - '0x%.8X (%d)'", hr, hr );
    }

    // Load the updated settings
    HRESULT hr2 = S_OK;
    if( FAILED( hr2 = LoadSettings() ) )
    {
        XDBGWRN( XSETTINGS_APP_TITLE_NAME_A, "CXSettings::SetLetterbox():There was a problem reloading the settings!! - '0x.8X (%d)'", hr2, hr2 );
    }

    return hr;
}


// Enable or Disable DTS
HRESULT CXSettings::SetDTS( BOOL bEnable )
{
    DWORD dwAudioSetting = m_pUserSettings->AudioFlags;

    if( bEnable )
    {
        dwAudioSetting |= XC_AUDIO_FLAGS_ENABLE_DTS;
    }
    else
    {
        dwAudioSetting &= ~XC_AUDIO_FLAGS_ENABLE_DTS;
    }

    // Store the DTS Setting in to the Xbox EEPROM
    HRESULT hr = S_OK;
    if( FAILED( hr = XSetValue( XC_AUDIO_FLAGS, REG_DWORD, &dwAudioSetting, sizeof( dwAudioSetting ) ) ) )
    {
        XDBGWRN( XSETTINGS_APP_TITLE_NAME_A, "CXSettings::SetDTS():Failed to set DTS!! - '0x%.8X (%d)'", hr, hr );
    }

    // Load the updated settings
    HRESULT hr2 = S_OK;
    if( FAILED( hr2 = LoadSettings() ) )
    {
        XDBGWRN( XSETTINGS_APP_TITLE_NAME_A, "CXSettings::SetDTS( ):There was a problem reloading the settings!! - '0x.8X (%d)'", hr2, hr2 );
    }

    return hr;
}

// Enable or Disable Dolby Digital
HRESULT CXSettings::SetDolbyDigital( BOOL bEnable )
{
    DWORD dwAudioSetting = m_pUserSettings->AudioFlags;

    if( bEnable )
    {
        dwAudioSetting |= XC_AUDIO_FLAGS_ENABLE_AC3;
    }
    else
    {
        dwAudioSetting &= ~XC_AUDIO_FLAGS_ENABLE_AC3;
    }

    // Store the AC3 Setting in to the Xbox EEPROM
    HRESULT hr = S_OK;
    if( FAILED( hr = XSetValue( XC_AUDIO_FLAGS, REG_DWORD, &dwAudioSetting, sizeof( dwAudioSetting ) ) ) )
    {
        XDBGWRN( XSETTINGS_APP_TITLE_NAME_A, "CXSettings::SetDolbyDigital():Failed to set AC3!! - '0x%.8X (%d)'", hr, hr );
    }

    // Load the updated settings
    HRESULT hr2 = S_OK;
    if( FAILED( hr2 = LoadSettings() ) )
    {
        XDBGWRN( XSETTINGS_APP_TITLE_NAME_A, "CXSettings::SetDolbyDigital( ):There was a problem reloading the settings!! - '0x.8X (%d)'", hr2, hr2 );
    }

    return hr;
}



// This method will convert the address in to a char* and then
// set it in to the EEPROM
HRESULT CXSettings::SetDebugIPAddress( WCHAR* IPAddress )
{
    XDBGTRC( XSETTINGS_APP_TITLE_NAME_A, "CIPMenu::SetDebugIPAddress( WCHAR* )" );

    if( !IPAddress )
    {
        XDBGWRN( XSETTINGS_APP_TITLE_NAME_A, "CXSettings::SetDebugIPAddress( WCHAR ):Invalid arg!!" );

        return E_INVALIDARG;
    }

    char locIPAddress[XSETTINGS_IPADDRESS_LENGTH + 1];
    ZeroMemory( locIPAddress, XSETTINGS_IPADDRESS_LENGTH + 1 );

    _snprintf( locIPAddress, XSETTINGS_IPADDRESS_LENGTH, "%S", IPAddress );
    HRESULT hr = SetDebugIPAddress( locIPAddress );

    return hr;
}


// Set the Debug IP Address on the Xbox
HRESULT CXSettings::SetDebugIPAddress( char* IPAddress )
{
    XDBGTRC( XSETTINGS_APP_TITLE_NAME_A, "CIPMenu::SetDebugIPAddress( char* )" );

    if( !IPAddress )
    {
        XDBGWRN( XSETTINGS_APP_TITLE_NAME_A, "CXSettings::SetDebugIPAddress( char ):Invalid arg!!" );

        return E_INVALIDARG;
    }

    DWORD dwAddress = 0L;

    // Convert IP Address in to a DWORD
    if( !ParseIPAddress( IPAddress, dwAddress ) )
    {
        XDBGWRN( XSETTINGS_APP_TITLE_NAME_A, "CXSettings::SetDebugIPAddress( char ):IP Address not valid!!" );

        return E_INVALIDARG;
    }

    // Store the valid address in to the Xbox EEPROM
    DmTell(DMTELL_SETDBGIP, (PVOID)dwAddress);

    // Load the updated settings
    if( FAILED( LoadSettings() ) )
    {
        XDBGWRN( XSETTINGS_APP_TITLE_NAME_A, "CXSettings::SetDebugIPAddress( char ):There was a problem reloading the settings!!" );
    }

    // Determine if the user set DHCP (blank Debug IP), and adjust the other variables accordingly
    if( ( '\0' == GetDebugIPAddress()[0] ) && ( '\0' == GetIPAddress()[0] ) )
    {
        if( FAILED( SetSubnetMask( "\0" ) ) )
        {
            XDBGWRN( XSETTINGS_APP_TITLE_NAME_A, "CXSettings::SetDebugIPAddress( char ):Failed to set Subnet Mask to '\\0'!!" );
        }

        if( FAILED( SetGateway( "\0" ) ) )
        {
            XDBGWRN( XSETTINGS_APP_TITLE_NAME_A, "CXSettings::SetDebugIPAddress( char ):Failed to set Gateway to '\\0'!!" );
        }
    }

    // Load the updated settings after we set the Subnet mask and the gateway
    if( FAILED( LoadSettings() ) )
    {
        XDBGWRN( XSETTINGS_APP_TITLE_NAME_A, "CXSettings::SetDebugIPAddress( char ):There was a problem reloading the settings!!" );
    }

    return S_OK;
}


// This method will convert the address in to a char* and then
// set it in to the EEPROM
HRESULT CXSettings::SetIPAddress( WCHAR* IPAddress )
{
    XDBGTRC( XSETTINGS_APP_TITLE_NAME_A, "CIPMenu::SetIPAddress( WCHAR* )" );

    if( !IPAddress )
    {
        XDBGWRN( XSETTINGS_APP_TITLE_NAME_A, "CXSettings::SetIPAddress( WCHAR ):Invalid arg!!" );

        return E_INVALIDARG;
    }

    char locIPAddress[XSETTINGS_IPADDRESS_LENGTH + 1];
    ZeroMemory( locIPAddress, XSETTINGS_IPADDRESS_LENGTH + 1 );

    _snprintf( locIPAddress, XSETTINGS_IPADDRESS_LENGTH, "%S", IPAddress );
    HRESULT hr = SetIPAddress( locIPAddress );

    return hr;
}


// Set the IP Address on the Xbox
HRESULT CXSettings::SetIPAddress( char* IPAddress )
{
    XDBGTRC( XSETTINGS_APP_TITLE_NAME_A, "CIPMenu::SetIPAddress( char* )" );

    if( !IPAddress )
    {
        XDBGWRN( XSETTINGS_APP_TITLE_NAME_A, "CXSettings::SetIPAddress( char ):Invalid arg!!" );
        return E_INVALIDARG;
    }

    DWORD dwAddress = 0L;

    // Convert IP Address in to a DWORD
    if( !ParseIPAddress( IPAddress, dwAddress ) )
    {
        XDBGWRN( XSETTINGS_APP_TITLE_NAME_A, "CXSettings::SetIPAddress( char ):IP Address not valid!!" );

        return E_INVALIDARG;
    }

    // Store the valid address in the xnet config sector
    XNetConfigParams xncp;
    XNetLoadConfigParams(&xncp);
    xncp.ina.s_addr = dwAddress;
    XNetSaveConfigParams(&xncp);

    // Store the valid address in to the Xbox EEPROM (for backward compat)
    HRESULT hr = S_OK;
    if( FAILED( hr = XSetValue( XC_ONLINE_IP_ADDRESS, REG_DWORD, &dwAddress, sizeof( dwAddress ) ) ) )
    {
        XDBGERR( XSETTINGS_APP_TITLE_NAME_A, "CXSettings::SetIPAddress( char ):Failed to set IP Address in the EEPROM!! - (0x%X)", hr );

        return hr;
    }

    // Load the updated settings
    if( FAILED( LoadSettings() ) )
        XDBGWRN( XSETTINGS_APP_TITLE_NAME_A, "CXSettings::SetIPAddress( char ):There was a problem reloading the settings!!" );

    // Determine if the user set DHCP (blank IP), and adjust the other variables accordingly
    if( ( '\0' == GetDebugIPAddress()[0] ) && ( '\0' == GetIPAddress()[0] ) )
    {
        if( FAILED( SetSubnetMask( "\0" ) ) )
        {
            XDBGWRN( XSETTINGS_APP_TITLE_NAME_A, "CXSettings::SetIPAddress( char ):Failed to set Subnet Mask to '\\0'!!" );
        }

        if( FAILED( SetGateway( "\0" ) ) )
        {
            XDBGWRN( XSETTINGS_APP_TITLE_NAME_A, "CXSettings::SetIPAddress( char ):Failed to set Gateway to '\\0'!!" );
        }

        if( FAILED( SetPrimaryDNS( "\0" ) ) )
        {
            XDBGWRN( XSETTINGS_APP_TITLE_NAME_A, "CXSettings::SetPrimaryDNS( char ):Failed to set Primary DNS to '\\0'!!" );
        }

        if( FAILED( SetSecondaryDNS( "\0" ) ) )
        {
            XDBGWRN( XSETTINGS_APP_TITLE_NAME_A, "CXSettings::SetSecondaryDNS( char ):Failed to set Secondary DNS to '\\0'!!" );
        }
    }

    // Load the updated settings after we set the Subnet mask and the gateway
    if( FAILED( LoadSettings() ) )
    {
        XDBGWRN( XSETTINGS_APP_TITLE_NAME_A, "CXSettings::SetIPAddress( char ):There was a problem reloading the settings!!" );
    }

    return hr;
}



// This method will convert the address in to a char* and then
// set it in to the EEPROM
HRESULT CXSettings::SetPrimaryDNS( WCHAR* IPAddress )
{
    XDBGTRC( XSETTINGS_APP_TITLE_NAME_A, "CIPMenu::SetPrimaryDNS( WCHAR* )" );

    if( !IPAddress )
    {
        XDBGWRN( XSETTINGS_APP_TITLE_NAME_A, "CXSettings::SetPrimaryDNS( WCHAR ):Invalid arg!!" );

        return E_INVALIDARG;
    }

    char locIPAddress[XSETTINGS_IPADDRESS_LENGTH + 1];
    ZeroMemory( locIPAddress, XSETTINGS_IPADDRESS_LENGTH + 1 );

    _snprintf( locIPAddress, XSETTINGS_IPADDRESS_LENGTH, "%S", IPAddress );
    HRESULT hr = SetPrimaryDNS( locIPAddress );

    return hr;
}


// Set the Primary DNS Address on the Xbox
HRESULT CXSettings::SetPrimaryDNS( char* IPAddress )
{
    XDBGTRC( XSETTINGS_APP_TITLE_NAME_A, "CIPMenu::SetPrimaryDNS( char* )" );

    if( !IPAddress )
    {
        XDBGWRN( XSETTINGS_APP_TITLE_NAME_A, "CXSettings::SetPrimaryDNS( char ):Invalid arg!!" );
        return E_INVALIDARG;
    }

    DWORD dwAddress = 0L;

    // Convert IP Address in to a DWORD
    if( !ParseIPAddress( IPAddress, dwAddress ) )
    {
        XDBGWRN( XSETTINGS_APP_TITLE_NAME_A, "CXSettings::SetPrimaryDNS( char ):IP Address not valid!!" );

        return E_INVALIDARG;
    }

    // Store the valid address in the xnet config sector
    XNetConfigParams xncp;
    XNetLoadConfigParams(&xncp);
    xncp.inaDnsPrimary.s_addr = dwAddress;
    XNetSaveConfigParams(&xncp);

    // Load the updated settings
    if( FAILED( LoadSettings() ) )
    {
        XDBGWRN( XSETTINGS_APP_TITLE_NAME_A, "CXSettings::SetPrimaryDNS( char ):There was a problem reloading the settings!!" );
    }

    return S_OK;
}

// This method will convert the address in to a char* and then
// set it in to the EEPROM
HRESULT CXSettings::SetSecondaryDNS( WCHAR* IPAddress )
{
    XDBGTRC( XSETTINGS_APP_TITLE_NAME_A, "CIPMenu::SetSecondaryDNS( WCHAR* )" );

    if( !IPAddress )
    {
        XDBGWRN( XSETTINGS_APP_TITLE_NAME_A, "CXSettings::SetSecondaryDNS( WCHAR ):Invalid arg!!" );

        return E_INVALIDARG;
    }

    char locIPAddress[XSETTINGS_IPADDRESS_LENGTH + 1];
    ZeroMemory( locIPAddress, XSETTINGS_IPADDRESS_LENGTH + 1 );

    _snprintf( locIPAddress, XSETTINGS_IPADDRESS_LENGTH, "%S", IPAddress );
    HRESULT hr = SetSecondaryDNS( locIPAddress );

    return hr;
}


// Set the Secondary DNS Address on the Xbox
HRESULT CXSettings::SetSecondaryDNS( char* IPAddress )
{
    XDBGTRC( XSETTINGS_APP_TITLE_NAME_A, "CIPMenu::SetSecondaryDNS( char* )" );

    if( !IPAddress )
    {
        XDBGWRN( XSETTINGS_APP_TITLE_NAME_A, "CXSettings::SetSecondaryDNS( char ):Invalid arg!!" );
        return E_INVALIDARG;
    }

    DWORD dwAddress = 0L;

    // Convert IP Address in to a DWORD
    if( !ParseIPAddress( IPAddress, dwAddress ) )
    {
        XDBGWRN( XSETTINGS_APP_TITLE_NAME_A, "CXSettings::SetSecondaryDNS( char ):IP Address not valid!!" );

        return E_INVALIDARG;
    }

    // Store the valid address in the xnet config sector
    XNetConfigParams xncp;
    XNetLoadConfigParams(&xncp);
    xncp.inaDnsSecondary.s_addr = dwAddress;
    XNetSaveConfigParams(&xncp);

    // Load the updated settings
    if( FAILED( LoadSettings() ) )
    {
        XDBGWRN( XSETTINGS_APP_TITLE_NAME_A, "CXSettings::SetSecondaryDNS( char ):There was a problem reloading the settings!!" );
    }

    return S_OK;
}


// Set the SubnetMask on the Xbox
HRESULT CXSettings::SetSubnetMask( WCHAR* SubnetMask )
{
    if( !SubnetMask )
    {
        XDBGWRN( XSETTINGS_APP_TITLE_NAME_A, "CXSettings::SetSubnetMask( WCHAR ):Invalid arg!!" );

        return E_INVALIDARG;
    }

    char locSubnetMask[XSETTINGS_IPADDRESS_LENGTH + 1];
    ZeroMemory( locSubnetMask, XSETTINGS_IPADDRESS_LENGTH + 1 );

    _snprintf( locSubnetMask, XSETTINGS_IPADDRESS_LENGTH, "%S", SubnetMask );
    HRESULT hr = SetSubnetMask( locSubnetMask );

    return hr;
}


// Set the SubnetMask on the Xbox
HRESULT CXSettings::SetSubnetMask( char* SubnetMask )
{
    if( !SubnetMask )
    {
        XDBGWRN( XSETTINGS_APP_TITLE_NAME_A, "CXSettings::SetSubnetMask( char ):Invalid arg!!" );

        return E_INVALIDARG;
    }

    DWORD dwAddress = 0L;

    // Convert IP Address in to a DWORD
    if( !ParseIPAddress( SubnetMask, dwAddress ) )
    {
        XDBGWRN( XSETTINGS_APP_TITLE_NAME_A, "CXSettings::SetSubnetMask( char ):Subnet Mask not valid!!" );

        return E_INVALIDARG;
    }

    // Store the valid address in the xnet config sector
    XNetConfigParams xncp;
    XNetLoadConfigParams(&xncp);
    xncp.inaMask.s_addr = dwAddress;
    XNetSaveConfigParams(&xncp);

    // Store the valid address in to the Xbox EEPROM (for backward compat)
    HRESULT hr = S_OK;
    if( FAILED( hr = XSetValue( XC_ONLINE_SUBNET_ADDRESS, REG_DWORD, &dwAddress, sizeof( dwAddress ) ) ) )
    {
        XDBGERR( XSETTINGS_APP_TITLE_NAME_A, "CXSettings::SetSubnetMask( char ):Subnet Mask not saved in the EEPROM!! - (0x%X)", hr );

		return hr;
    }

    // Load the updated settings
    if( FAILED( LoadSettings() ) )
        XDBGWRN( XSETTINGS_APP_TITLE_NAME_A, "CXSettings::SetSubnetMask( char ):There was a problem reloading the settings!!" );

    return hr;
}


// Set the Gateway on the Xbox
HRESULT CXSettings::SetGateway( WCHAR* Gateway )
{
    if( !Gateway )
    {
        XDBGWRN( XSETTINGS_APP_TITLE_NAME_A, "CXSettings::SetGateway( WCHAR ):Invalid arg!!" );

        return E_INVALIDARG;
    }

    char locGateway[XSETTINGS_IPADDRESS_LENGTH + 1];
    ZeroMemory( locGateway, XSETTINGS_IPADDRESS_LENGTH + 1 );

    _snprintf( locGateway, XSETTINGS_IPADDRESS_LENGTH, "%S", Gateway );
    HRESULT hr = SetGateway( locGateway );

    return hr;
}


// Set the Gateway on the Xbox
HRESULT CXSettings::SetGateway( char* Gateway )
{
    if( !Gateway )
    {
        XDBGWRN( XSETTINGS_APP_TITLE_NAME_A, "CXSettings::SetGateway( char ):Invalid arg!!" );

        return E_INVALIDARG;
    }

    DWORD dwAddress = 0L;

    // Convert IP Address in to a DWORD
    if( !ParseIPAddress( Gateway, dwAddress ) )
    {
        XDBGWRN( XSETTINGS_APP_TITLE_NAME_A, "CXSettings::SetGateway( char ):Gateway not valid!!" );

        return E_INVALIDARG;
    }

    // Store the valid address in the xnet config sector
    XNetConfigParams xncp;
    XNetLoadConfigParams(&xncp);
    xncp.inaGateway.s_addr = dwAddress;
    XNetSaveConfigParams(&xncp);

    // Store the valid address in to the Xbox EEPROM (for backward compat)
    HRESULT hr = S_OK;
    if( FAILED( hr = XSetValue( XC_ONLINE_DEFAULT_GATEWAY_ADDRESS, REG_DWORD, &dwAddress, sizeof( dwAddress ) ) ) )
    {
        XDBGERR( XSETTINGS_APP_TITLE_NAME_A, "CXSettings::SetGateway( char ):Gateway not saved in the EEPROM!!" );

        return hr;
    }

    // Load the updated settings
    if( FAILED( LoadSettings() ) )
        XDBGWRN( XSETTINGS_APP_TITLE_NAME_A, "CXSettings::SetGateway( char ):There was a problem reloading the settings!!" );

    return hr;
}


// Set the Gateway on the Xbox
HRESULT CXSettings::SetMachineName( WCHAR* MachineName )
{
    if( !MachineName )
    {
        XDBGWRN( XSETTINGS_APP_TITLE_NAME_A, "CXSettings::SetMachineName( WCHAR ):Invalid arg!!" );

        return E_INVALIDARG;
    }

    char locMachineName[XSETTINGS_MACHINENAME_LENGTH + 1];
    ZeroMemory( locMachineName, XSETTINGS_MACHINENAME_LENGTH + 1 );

    _snprintf( locMachineName, XSETTINGS_MACHINENAME_LENGTH, "%S", MachineName );
    HRESULT hr = SetMachineName( locMachineName );

    return hr;
}


// Set the MachineName on the Xbox
HRESULT CXSettings::SetMachineName( char* MachineName )
{
    if( !MachineName )
    {
        XDBGWRN( XSETTINGS_APP_TITLE_NAME_A, "CXSettings::SetMachineName():Invalid arg!!" );

        return E_INVALIDARG;
    }

    if( XboxNameExistsOnNet( MachineName ) )
    {
        XDBGWRN( XSETTINGS_APP_TITLE_NAME_A, "CXSettings::SetMachineName():Machine Name Exists on the Network!!" );
        
        return E_FAIL;
    }

    HRESULT hr = S_OK;
    if( FAILED( hr = DmSetXboxName( MachineName ) ) )
        XDBGERR( XSETTINGS_APP_TITLE_NAME_A, "CXSettings::SetMachineName():Failed to set the name!!" );

    return hr;
}


// Functions from config.cpp of XApp

// Converts an IP Address from a string to a DWORD
BOOL CXSettings::ParseIPAddress( char* szIPAddr, DWORD& dwIPAddr )
{
	if ( szIPAddr[0] == 0 )
	{
		dwIPAddr = 0;
		return TRUE;
	}

	unsigned int b1, b2, b3, b4;
	if ( sscanf( szIPAddr, "%d.%d.%d.%d", &b1, &b2, &b3, &b4 ) != 4 || b1 > 255 || b2 > 255 || b3 > 255 || b4 > 255 )
		return FALSE;

	dwIPAddr = (b4 << 24) | (b3 << 16) | (b2 << 8) | b1;

	return TRUE;
}


HRESULT CXSettings::SetHDKey( UCHAR* puszHDKeyBuff )
{
    if( NULL == puszHDKeyBuff )
    {
        XDBGWRN( XSETTINGS_APP_TITLE_NAME_A, "CXSettings::SetHDKey()Invalid parameter passed in!!" );

        return E_INVALIDARG;
    }

    // Make sure the encrypted section of the EEPROM is unencrypted
    DWORD err = EncryptDecryptEEPROMEncryptedSection(FALSE);
    if (err != ERROR_SUCCESS)
    {
        XDBGWRN( XSETTINGS_APP_TITLE_NAME_A, "CXSettings::SetHDKey()Failed to Decrypt the EEPROM!!" );

        return E_FAIL;
    }

    // Unlock the Hard Drive
    HRESULT hr = UnlockXboxHardDrive( NULL, 0 );
    if( FAILED( hr ) )
    {
        XDBGWRN( XSETTINGS_APP_TITLE_NAME_A, "CXSettings::SetHDKey()Failed to Unlock the Hard Drive!!" );
    }

    if( SUCCEEDED( hr ) )
    {
        // Set the HDKey to the user passed in value
        memcpy( m_pEncSettings->HDKey, puszHDKeyBuff, XBOX_KEY_LENGTH );

        // Write the EEPROM data back to store the new key
        NTSTATUS status = ExSaveNonVolatileSetting( XC_MAX_ALL, REG_BINARY, &m_AllSettings, EEPROM_TOTAL_MEMORY_SIZE );
        if ( !NT_SUCCESS( status ) )
        {
            XDBGWRN( XSETTINGS_APP_TITLE_NAME_A, "CXSettings::SetHDKey()Failed to write the EEPROM data!!" );

            hr = E_FAIL;
        }
    }
    
    if( SUCCEEDED( hr ) )
    {
        //
        // Compute Key to use to lock hard drive
        //
        BYTE FinalHardDriveKey[XC_SERVICE_DIGEST_SIZE];

        XcHMAC( m_pEncSettings->HDKey, XBOX_KEY_LENGTH,
                (LPBYTE)HalDiskModelNumber->Buffer, HalDiskModelNumber->Length,
                (LPBYTE)HalDiskSerialNumber->Buffer, HalDiskSerialNumber->Length,
                FinalHardDriveKey );

        HDSetPassword( FinalHardDriveKey, XC_SERVICE_DIGEST_SIZE );
    }

    // Encrypt the EEPROM
    err = EncryptDecryptEEPROMEncryptedSection( TRUE );
    if (err != ERROR_SUCCESS)
    {
        XDBGWRN( XSETTINGS_APP_TITLE_NAME_A, "CXSettings::SetHDKey()Failed to Encrypt the EEPROM!!" );

        hr = E_FAIL;
    }

    // Load the updated settings
    if( FAILED( LoadSettings() ) )
        XDBGWRN( XSETTINGS_APP_TITLE_NAME_A, "CXSettings::SetHDKey():There was a problem reloading the settings!!" );

    return hr;
}


// Set the Xbox Game Region
HRESULT CXSettings::SetGameRegion( DWORD dwGameRegion )
{
    // Make sure the encrypted section of the EEPROM is unencrypted
    DWORD err = EncryptDecryptEEPROMEncryptedSection(FALSE);
    if (err != ERROR_SUCCESS)
        return E_FAIL;

    // Store the Game Region to the Xbox EEPROM
    HRESULT hr = XSetValue( XC_FACTORY_GAME_REGION, REG_DWORD, &dwGameRegion, sizeof( dwGameRegion ) );
    if( FAILED( hr ) )
    {
        XDBGWRN( XSETTINGS_APP_TITLE_NAME_A, "CXSettings::SetGameRegion():Game Region not saved in to the EEPROM!! Region - '%d' - '0x%0.8X (%d)'", dwGameRegion, hr, hr );
    }

    // Encrypt the EEPROM Encrypted Section
    err = EncryptDecryptEEPROMEncryptedSection( TRUE );
    if (err != ERROR_SUCCESS)
    {
        XDBGWRN( XSETTINGS_APP_TITLE_NAME_A, "CXSettings::SetGameRegion():Failed to Encrypt the EEPROM!!" );

        hr = E_FAIL;
    }

    // Load the updated settings
    if( FAILED( LoadSettings() ) )
        XDBGWRN( XSETTINGS_APP_TITLE_NAME_A, "CXSettings::SetGameRegion():There was a problem reloading the settings!!" );

    return hr;
}


// Get the Xbox Game Region
DWORD CXSettings::GetGameRegion()
{
    return m_pEncSettings->GameRegion;
}

// Set the Xbox AV Region
HRESULT CXSettings::SetAVRegion( DWORD dwAVRegion )
{
    // Store the Display Setting to the Xbox EEPROM
    HRESULT hr = S_OK;
    if( FAILED( hr = XSetValue( XC_FACTORY_AV_REGION, REG_DWORD, &dwAVRegion, sizeof( dwAVRegion) ) ) )
        XDBGERR( XSETTINGS_APP_TITLE_NAME_A, "CXSettings::SetAVRegion():AV region not saved in to the EEPROM!! Region - '%d', Error - '0x%0.8X (%d)'", dwAVRegion, hr, hr );

    // Load the updated settings
    if( FAILED( LoadSettings() ) )
        XDBGWRN( XSETTINGS_APP_TITLE_NAME_A, "CXSettings::SetAVRegion():There was a problem reloading the settings!!" );

    return hr;
}

// Get the Xbox AV Region
DWORD CXSettings::GetAVRegion()
{
    return m_pFactorySettings->AVRegion;
}


// Set the Xbox Language
HRESULT CXSettings::SetLanguage( DWORD dwLanguage )
{
    // Store the Languageto the Xbox EEPROM
    HRESULT hr = XSetValue( XC_LANGUAGE, REG_DWORD, &dwLanguage, sizeof( dwLanguage ) );
    if( FAILED( hr ) )
    {
        XDBGWRN( XSETTINGS_APP_TITLE_NAME_A, "CXSettings::SetLanguage():Language not saved in to the EEPROM!! - '0x%.8X'", hr );

		return hr;
    }

    // Load the updated settings
    if( FAILED( LoadSettings() ) )
    {
        XDBGWRN( XSETTINGS_APP_TITLE_NAME_A, "CXSettings::SetLanguage():There was a problem reloading the settings!!" );
    }

    return S_OK;
}

// Get the Xbox Language
DWORD CXSettings::GetLanguage()
{
    return m_pUserSettings->Language;
}


// Set the Time Zone information
HRESULT CXSettings::SetTimeZoneInfo( unsigned int uiIndex )
{
    if ( uiIndex >= XSETTINGS_TIMEZONECOUNT )
    {
        XDBGWRN( XSETTINGS_APP_TITLE_NAME_A, "CXSettings::SetTimeZoneInfo():Invalid index passed in!! Index - '%u', Max - '%u'", uiIndex, XSETTINGS_TIMEZONECOUNT - 1 );

        return E_INVALIDARG;
    }

    TIME_ZONE_INFORMATION tzinfo;
    ZeroMemory( &tzinfo, sizeof( tzinfo ) );

    const struct TZINFO* srcinfo = &g_timezoneinfo[uiIndex];

    tzinfo.Bias = srcinfo->bias;
    tzinfo.StandardBias = srcinfo->stdbias;
    tzinfo.DaylightBias = srcinfo->dltbias;

    wcsncpy(tzinfo.StandardName, srcinfo->stdname, countof(tzinfo.StandardName));
    wcsncpy(tzinfo.DaylightName,
            srcinfo->dltname ? srcinfo->dltname : srcinfo->stdname,
            countof(tzinfo.DaylightName));

    TZDateToSysTime(&srcinfo->stddate, &tzinfo.StandardDate);
    TZDateToSysTime(&srcinfo->dltdate, &tzinfo.DaylightDate);

    XapipSetTimeZoneInformation(&tzinfo);

    // Load the updated settings
    if( FAILED( LoadSettings() ) )
    {
        XDBGWRN( XSETTINGS_APP_TITLE_NAME_A, "CXSettings::SetTimeZoneInfo():There was a problem reloading the settings!!" );
    }

    return S_OK;
}

// Convert a timezone date into a system time date
void CXSettings::TZDateToSysTime(const struct TZDATE* tzdate, SYSTEMTIME* systime)
{
    systime->wMonth = tzdate->month;
    systime->wDay = tzdate->day;
    systime->wDayOfWeek = tzdate->dayofweek;
    systime->wHour = tzdate->hour;
}

// Get the Index of the current timezone in the EEPROM
unsigned int CXSettings::GetCurrentTimeZoneIndex()
{
    int iBestMatch, iBestResult;
    iBestMatch = iBestResult = -1;
    WCHAR pwszNameBuf[40];
    ZeroMemory( pwszNameBuf, sizeof( WCHAR ) * 40 );
    for (unsigned int uiIndex = 0; uiIndex < XSETTINGS_TIMEZONECOUNT; uiIndex++ )
    {
        const struct TZINFO* srcinfo = &g_timezoneinfo[uiIndex];
        if( srcinfo->bias == GetUserTimeZoneBias() )
        {
            int iMatch = 1;
            if( srcinfo->stdbias == GetUserTimeZoneStdBias() &&
                srcinfo->dltbias == GetUserTimeZoneDltBias() )
            {
                 iMatch++;

                // Compare only the first 4 characters of the time zone name
                // because that's what we store in the EEPROM.
                // If everything matches, no need to continue searching.
                GetUserTimeZoneStdName( pwszNameBuf, 39 );
                if( wcsncmp( srcinfo->stdname, pwszNameBuf, 4 ) == 0 )
                {
                    return uiIndex;
                }
            }

            if ( iMatch > iBestMatch )
            {
                iBestMatch = iMatch;
                iBestResult = uiIndex;
            }
        }
    }

    return iBestResult;
}


// Get the currently set Time Zone information from the EEPROM
TZINFO* CXSettings::GetCurrentTimeZoneInfo()
{
    unsigned int uiCurrentTZIndex = GetCurrentTimeZoneIndex();

    return &g_timezoneinfo[uiCurrentTZIndex];
}


// Get the Xbox Time Zone Information for a specified index
TZINFO* CXSettings::GetTimeZoneInfo( unsigned int uiIndex )
{
    if( uiIndex >= XSETTINGS_TIMEZONECOUNT )
    {
        XDBGWRN( XSETTINGS_APP_TITLE_NAME_A, "CXSettings::GetTimeZoneInfo():Requested invalid index!! Index - '%u', Max - '%u'", uiIndex, XSETTINGS_TIMEZONECOUNT - 1 );

        return NULL;
    }

    return &g_timezoneinfo[uiIndex];
}


// Get the state of 1080i
BOOL CXSettings::Get1080i()
{
    if( ( m_pUserSettings->VideoFlags & AV_FLAGS_HDTV_1080i ) > 0 )
    {
        return TRUE;
    }

    return FALSE;
}

// Get the state of 720p
BOOL CXSettings::Get720p()
{
    if( ( m_pUserSettings->VideoFlags & AV_FLAGS_HDTV_720p ) > 0 )
    {
        return TRUE;
    }

    return FALSE;
}


// Get the state of 480p
BOOL CXSettings::Get480p()
{
    if( ( m_pUserSettings->VideoFlags & AV_FLAGS_HDTV_480p ) > 0 )
    {
        return TRUE;
    }

    return FALSE;
}

// Get the state of PAL 60
BOOL CXSettings::GetPAL60()
{
    if( ( m_pUserSettings->VideoFlags & AV_FLAGS_60Hz ) > 0 )
    {
        return TRUE;
    }

    return FALSE;
}


// Get the state of Widescreen
BOOL CXSettings::GetWidescreen()
{
    if( ( m_pUserSettings->VideoFlags & AV_FLAGS_WIDESCREEN ) > 0 )
    {
        return TRUE;
    }

    return FALSE;
}


// Get the state of Letterbox
BOOL CXSettings::GetLetterbox()
{
    if( ( m_pUserSettings->VideoFlags & AV_FLAGS_LETTERBOX ) > 0 )
    {
        return TRUE;
    }

    return FALSE;
}


// Get the state of DTS
BOOL CXSettings::GetDTS()
{
    if( ( m_pUserSettings->AudioFlags & XC_AUDIO_FLAGS_ENABLE_DTS ) > 0 )
    {
        return TRUE;
    }

    return FALSE;
}


// Get the state of Dolby Digital
BOOL CXSettings::GetDolbyDigital()
{
    if( ( m_pUserSettings->AudioFlags & XC_AUDIO_FLAGS_ENABLE_AC3 ) > 0 )
    {
        return TRUE;
    }

    return FALSE;
}



// Set the Xbox DVD Region
HRESULT CXSettings::SetDVDRegion( DWORD dwDVDRegion )
{
    //
    // Set the AV Region in to the EEPROM
    //
    HRESULT hr = S_OK;
    if( FAILED( hr = XSetValue( XC_DVD_REGION, REG_DWORD, &dwDVDRegion, sizeof( dwDVDRegion) ) ) )
    {
        XDBGWRN( XSETTINGS_APP_TITLE_NAME_A, "CXSettings::SetDVDRegion():DVD region not saved in to the EEPROM!! Region - '%d', Error - '0x%0.8X (%d)'", dwDVDRegion, hr, hr );
    }

    // Load the updated settings
    if( FAILED( LoadSettings() ) )
    {
        XDBGWRN( XSETTINGS_APP_TITLE_NAME_A, "CXSettings::SetDVDRegion():There was a problem reloading the settings!!" );
    }

    return hr;
}


// Get the Xbox DVD Region
DWORD CXSettings::GetDVDRegion()
{
    return m_pUserSettings->DvdRegion;
}


// Returns the recovery key generated from the current HD Key
void CXSettings::GetHDRecoveryKey( OUT UCHAR* pucBuffer, IN ULONG cbBufferSize )
{
    if( NULL == pucBuffer )
    {
        XDBGWRN( XSETTINGS_APP_TITLE_NAME_A, "CXSettings::GetHDRecoveryKey():Invalid parameter passed in!!" );

        return;
    }

    ASSERT( cbBufferSize >= RECOVERY_KEY_LEN );

    ComputeRecoveryKey((LPBYTE)m_pEncSettings->HDKey, (char*)pucBuffer);
}

// Resets all values on the Xbox back to their "Default" state
HRESULT CXSettings::ResetAllToDefaults()
{
    //
    // Reset all User Configuration Data
    //
	XBOX_USER_SETTINGS  UserSettings;	// All User Configuration Settings
	ZeroMemory( &UserSettings, sizeof( UserSettings ) );

    DWORD dwType;
    HRESULT hr = S_OK;
    if( FAILED( hr = XSetValue( XC_MAX_OS, 0, &UserSettings, sizeof( UserSettings ) ) ) )
    {
        XDBGWRN( XSETTINGS_APP_TITLE_NAME_A, "CXSettings::ResetAllToDefaults():failed to reset!! - '0x%0.8X'", hr );
    }

    //
    // Clear the Refurb sector
    //
    NTSTATUS status;
    XBOX_REFURB_INFO refurbInfo;

    ZeroMemory( &refurbInfo, sizeof( refurbInfo ) );

    status = ExReadWriteRefurbInfo(&refurbInfo, sizeof(refurbInfo), TRUE);
    if( !NT_SUCCESS(status) )
    {
        XDBGWRN( XSETTINGS_APP_TITLE_NAME_A, "CXSettings::ResetAllToDefaults():Failed to Clear the RefurbInfo!! Error - '0x%0.8X (%d)'", status, status );
    }

    //
    // Set the Clock to a very early date
    //
    SYSTEMTIME sysTime;
    ZeroMemory( &sysTime, sizeof( sysTime ) );

    sysTime.wHour = 0;
    sysTime.wMinute = 0;
    sysTime.wSecond = 0;
    sysTime.wMilliseconds = 0;
    sysTime.wMonth = 1;
    sysTime.wDay = 1;
    sysTime.wDayOfWeek = 0;
    sysTime.wYear = 1980;

    XapiSetLocalTime(&sysTime);

	return hr;
}


// Converts an IP Address from a DWORD to a string
char* CXSettings::StringFromIPAddress( DWORD dwIPAddr )
{
	if ( dwIPAddr == 0l )
    {
        char* szBuf = new char[1];
        szBuf[0] = '\0';
		
        return szBuf;
    }

	BYTE b4 = (BYTE)( dwIPAddr >> 24 );
	BYTE b3 = (BYTE)( dwIPAddr >> 16 );
	BYTE b2 = (BYTE)( dwIPAddr >> 8 );
	BYTE b1 = (BYTE)dwIPAddr;

	char* szBuf = new char[XSETTINGS_IPADDRESS_LENGTH + 1];
    szBuf[XSETTINGS_IPADDRESS_LENGTH] = '\0';
	_snprintf( szBuf, XSETTINGS_IPADDRESS_LENGTH, "%d.%d.%d.%d", b1, b2, b3, b4 );

	return szBuf;
}

typedef struct _NM {
	BYTE bRequest;
	BYTE cchName;
	char szName[256];
} NM;

// If the Xbox name is found on the network, this will return TRUE, otherwise FALSE
BOOL CXSettings::XboxNameExistsOnNet( char* pszName )
{
	struct sockaddr_in sinU;
    NM nm;
	SOCKET s;
	int cRetries;
	BOOL f;
    const DWORD dwRetry = 750;  // Number of milliseconds to sleep when searching for an Xbox Name

    // return TRUE;

	s = socket( AF_INET, SOCK_DGRAM, IPPROTO_UDP );
	if( INVALID_SOCKET == s )
    {
        XDBGWRN( XSETTINGS_APP_TITLE_NAME_A, "CXSettings::XboxNameExistsOnNet():Invalid Socket!!" );

		return FALSE;
    }
	
    f = TRUE;
	
    if(0 != setsockopt( s, SOL_SOCKET, SO_BROADCAST, (char*)&f, sizeof f ) )
    {
		closesocket( s );

        XDBGWRN( XSETTINGS_APP_TITLE_NAME_A, "CXSettings::XboxNameExistsOnNet():setsockopt did not return 0!!" );
		
        return FALSE;
	}
	
    sinU.sin_family = AF_INET;
	sinU.sin_port = htons( 0x2db );     // Debugger Port
	sinU.sin_addr.s_addr = htonl( INADDR_BROADCAST );

    nm.cchName = strlen( pszName );
    strncpy( nm.szName, pszName, 256 );
    nm.bRequest = 1;

    f = FALSE;
	for(cRetries = 3; !f && cRetries--; ) {
        DWORD tickTry = GetTickCount() + dwRetry;
		if(sendto(s, (char *)&nm, nm.cchName + 2, 0,
			(struct sockaddr *)&sinU, sizeof sinU) == nm.cchName + 2)
		{
			int fSel;

            do {
                int tickRetry;

                tickRetry = tickTry - GetTickCount();
                if(tickRetry <= 0)
                    fSel = 0;
                else {
			        fd_set fds;
			        struct timeval tv;

			        FD_ZERO(&fds);
			        FD_SET(s, &fds);
			        tv.tv_sec = tickRetry / 1000;
			        tv.tv_usec = (tickRetry % 1000) * 1000;
			        fSel = select(0, &fds, NULL, NULL, &tv);
                }
			    if(fSel > 0) {
				    NM nmT;
				    int cbAddr = sizeof sinU;
				    /* Got some data, is it for us? */
				    if(recvfrom(s, (char *)&nmT, sizeof nmT, 0,
						    (struct sockaddr *)&sinU, &cbAddr) <= 0)
					    fSel = -1;
				    else {
					    if(nmT.bRequest == 2 && nmT.cchName == nm.cchName &&
							!_strnicmp(nm.szName, nmT.szName, nm.cchName))
                        {
						    /* Got it! */
                            f = TRUE;
                            fSel = 0;
						    break;
                        }
				    }
			    }
                if(fSel < 0) {
				    /* Error, need to sleep */
				    Sleep(tickRetry);
                    fSel = 0;
                }
            } while(fSel);
		} else {
			int err = WSAGetLastError();

            XDBGWRN( XSETTINGS_APP_TITLE_NAME_A, "CXSettings::XboxNameExistsOnNet():Send Failed with Error - '%d'!!", err );

            /* Send failed, need to sleep */
			Sleep(dwRetry);
		}
	}

	closesocket(s);
	
    if( cRetries < 0 )
    {
        XDBGTRC( XSETTINGS_APP_TITLE_NAME_A, "CXSettings::XboxNameExistsOnNet():'%s' not found after %d tries!!", nm.szName, 3 );
        
        return FALSE;
    }

    /* We have our answer */
	return TRUE;
}


//
// Factory Settings Accessors
//

ULONG CXSettings::GetFactoryChecksum()
{
    return m_pFactorySettings->Checksum;
}

void CXSettings::GetFactorySerialNumber( OUT UCHAR* pucBuffer, IN ULONG cbBufferSize )
{
    if( NULL == pucBuffer )
    {
        XDBGWRN( XSETTINGS_APP_TITLE_NAME_A, "CXSettings::GetFactorySerialNumber():Invalid parameter passed in!!" );

        return;
    }

    ASSERT( cbBufferSize >= SETTINGS_FACTORY_SERIAL_NUM_SIZE );

    memcpy( pucBuffer, m_pFactorySettings->SerialNumber, SETTINGS_FACTORY_SERIAL_NUM_SIZE );
}

void CXSettings::GetFactoryEthernetAddr( OUT UCHAR* pucBuffer, IN ULONG cbBufferSize )
{
    if( NULL == pucBuffer )
    {
        XDBGWRN( XSETTINGS_APP_TITLE_NAME_A, "CXSettings::GetFactoryEthernetAddr():Invalid parameter passed in!!" );

        return;
    }

    ASSERT( cbBufferSize >= SETTINGS_FACTORY_ETHERNET_SIZE );

    memcpy( pucBuffer, m_pFactorySettings->EthernetAddr, SETTINGS_FACTORY_ETHERNET_SIZE );
}

void CXSettings::GetFactoryReserved1( OUT UCHAR* pucBuffer, IN ULONG cbBufferSize )
{
    if( NULL == pucBuffer )
    {
        XDBGWRN( XSETTINGS_APP_TITLE_NAME_A, "CXSettings::GetFactoryReserved1():Invalid parameter passed in!!" );

        return;
    }

    ASSERT( cbBufferSize >= SETTINGS_FACTORY_RESERVED1_SIZE );

    memcpy( pucBuffer, m_pFactorySettings->Reserved1, SETTINGS_FACTORY_RESERVED1_SIZE );
}

void CXSettings::GetFactoryOnlineKey( OUT UCHAR* pucBuffer, IN ULONG cbBufferSize )
{
    if( NULL == pucBuffer )
    {
        XDBGWRN( XSETTINGS_APP_TITLE_NAME_A, "CXSettings::GetFactoryOnlineKey():Invalid parameter passed in!!" );

        return;
    }

    ASSERT( cbBufferSize >= SETTINGS_FACTORY_ONLINEKEY_SIZE );

    memcpy( pucBuffer, m_pFactorySettings->OnlineKey, SETTINGS_FACTORY_ONLINEKEY_SIZE );
}

ULONG CXSettings::GetFactoryAVRegion()
{
    return m_pFactorySettings->AVRegion;
}

ULONG CXSettings::GetFactoryReserved2()
{
    return m_pFactorySettings->Reserved2;
}


//
// User Settings Accessors
//

ULONG CXSettings::GetUserChecksum()
{
    return m_pUserSettings->Checksum;
}

ULONG CXSettings::GetUserTimeZoneBias()
{
    return m_pUserSettings->TimeZoneBias;
}

void CXSettings::GetUserTimeZoneStdName( OUT WCHAR* pwszBuffer, IN ULONG cbBufferSize )
{
    if( NULL == pwszBuffer )
    {
        XDBGWRN( XSETTINGS_APP_TITLE_NAME_A, "CXSettings::GetUserTimeZoneStdName():Invalid parameter passed in!!" );

        return;
    }

    ASSERT( cbBufferSize >= ( (XC_TZNAMELEN+1) * sizeof( WCHAR ) ) );
    XboxTimeZoneNameToWstr(m_pUserSettings->TimeZoneStdName, pwszBuffer);
}

void CXSettings::GetUserTimeZoneDltName( OUT WCHAR* pwszBuffer, IN ULONG cbBufferSize )
{
    if( NULL == pwszBuffer )
    {
        XDBGWRN( XSETTINGS_APP_TITLE_NAME_A, "CXSettings::GetUserTimeZoneDltName():Invalid parameter passed in!!" );

        return;
    }

    ASSERT( cbBufferSize >= ( (XC_TZNAMELEN+1) * sizeof( WCHAR ) ) );
    XboxTimeZoneNameToWstr(m_pUserSettings->TimeZoneDltName, pwszBuffer);
}

void CXSettings::GetUserTimeZoneStdDate( OUT XBOX_TIMEZONE_DATE* pTZStdDateBuffer, IN ULONG cbBufferSize )
{
    if( NULL == pTZStdDateBuffer )
    {
        XDBGWRN( XSETTINGS_APP_TITLE_NAME_A, "CXSettings::GetUserTimeZoneStdDate():Invalid parameter passed in!!" );

        return;
    }

    ASSERT( cbBufferSize >= SETTINGS_USER_TZSTDDATE_SIZE );

    memcpy( pTZStdDateBuffer, &m_pUserSettings->TimeZoneStdDate, SETTINGS_USER_TZSTDDATE_SIZE );
}

void CXSettings::GetUserTimeZoneDltDate( OUT XBOX_TIMEZONE_DATE* pTZDltDateBuffer, IN ULONG cbBufferSize )
{
    if( NULL == pTZDltDateBuffer )
    {
        XDBGWRN( XSETTINGS_APP_TITLE_NAME_A, "CXSettings::GetUserTimeZoneDltDate():Invalid parameter passed in!!" );

        return;
    }

    ASSERT( cbBufferSize >= SETTINGS_USER_TZDLTDATE_SIZE );

    memcpy( pTZDltDateBuffer, &m_pUserSettings->TimeZoneDltDate, SETTINGS_USER_TZDLTDATE_SIZE );
}

void CXSettings::GetUserReserved( OUT ULONG* pulBuffer, IN ULONG cbBufferSize )
{
    if( NULL == pulBuffer )
    {
        XDBGWRN( XSETTINGS_APP_TITLE_NAME_A, "CXSettings::GetUserReserved():Invalid parameter passed in!!" );

        return;
    }

    ASSERT( cbBufferSize >= ( SETTINGS_USER_RESERVED_SIZE * sizeof( ULONG ) ) );

    memcpy( pulBuffer, m_pUserSettings->Reserved2, SETTINGS_USER_RESERVED_SIZE * sizeof( ULONG ) );
}

ULONG CXSettings::GetUserTimeZoneStdBias()
{
    return m_pUserSettings->TimeZoneStdBias;
}

ULONG CXSettings::GetUserTimeZoneDltBias()
{
    return m_pUserSettings->TimeZoneDltBias;
}

ULONG CXSettings::GetUserLanguage()
{
    return m_pUserSettings->Language;
}

ULONG CXSettings::GetUserVideoFlags()
{
    return m_pUserSettings->VideoFlags;
}

ULONG CXSettings::GetUserAudioFlags()
{
    return m_pUserSettings->AudioFlags;
}

ULONG CXSettings::GetUserParentalControlGames()
{
    return m_pUserSettings->ParentalControlGames;
}

ULONG CXSettings::GetUserParentalControlPassword()
{
    return m_pUserSettings->ParentalControlPassword;
}

ULONG CXSettings::GetUserParentalControlMovies()
{
    return m_pUserSettings->ParentalControlMovies;
}

ULONG CXSettings::GetUserOnlineIpAddress()
{
    return m_pUserSettings->OnlineIpAddress;
}

ULONG CXSettings::GetUserOnlineDnsAddress()
{
    return m_pUserSettings->OnlineDnsAddress;
}

ULONG CXSettings::GetUserOnlineDefaultGatewayAddress()
{
    return m_pUserSettings->OnlineDefaultGatewayAddress;
}

ULONG CXSettings::GetUserOnlineSubnetMask()
{
    return m_pUserSettings->OnlineSubnetMask;
}

ULONG CXSettings::GetUserMiscFlags()
{
    return m_pUserSettings->MiscFlags;
}

ULONG CXSettings::GetUserDvdRegion()
{
    return m_pUserSettings->DvdRegion;
}


//
// Encrypted Settings Accessors
//

void CXSettings::GetEncChecksum( OUT UCHAR* pucBuffer, IN ULONG cbBufferSize )
{
    if( NULL == pucBuffer )
    {
        XDBGWRN( XSETTINGS_APP_TITLE_NAME_A, "CXSettings::GetEncChecksum():Invalid parameter passed in!!" );

        return;
    }

    ASSERT( cbBufferSize >= SETTINGS_ENC_CHECKSUM_SIZE );

    memcpy( pucBuffer, m_pEncSettings->Checksum, SETTINGS_ENC_CHECKSUM_SIZE );
}

void CXSettings::GetEncConfounder( OUT UCHAR* pucBuffer, IN ULONG cbBufferSize )
{
    if( NULL == pucBuffer )
    {
        XDBGWRN( XSETTINGS_APP_TITLE_NAME_A, "CXSettings::GetEncConfounder():Invalid parameter passed in!!" );

        return;
    }

    ASSERT( cbBufferSize >= SETTINGS_ENC_CONFOUNDER_SIZE );

    memcpy( pucBuffer, m_pEncSettings->Confounder, SETTINGS_ENC_CONFOUNDER_SIZE );
}

void CXSettings::GetEncHDKey( OUT UCHAR* pucBuffer, IN ULONG cbBufferSize )
{
    if( NULL == pucBuffer )
    {
        XDBGWRN( XSETTINGS_APP_TITLE_NAME_A, "CXSettings::GetEncHDKey():Invalid parameter passed in!!" );

        return;
    }

    ASSERT( cbBufferSize >= SETTINGS_ENC_HDKEY_SIZE );

    memcpy( pucBuffer, m_pEncSettings->HDKey, SETTINGS_ENC_HDKEY_SIZE );
}

ULONG CXSettings::GetEncGameRegion()
{
    return m_pEncSettings->GameRegion;
}


// Get a checksum for ALL of the settings
DWORD CXSettings::GetNewGlobalChecksum()
{
    return Crc32Checksum( (VOID*)&m_AllSettings, sizeof( m_AllSettings ) );
}


// Get a checksum for the Factory settings
DWORD CXSettings::GetNewFactoryChecksum()
{
    return Crc32Checksum( (VOID*)m_pFactorySettings, sizeof( XBOX_FACTORY_SETTINGS ) );
}


// Get a checksum for the User settings
DWORD CXSettings::GetNewUserChecksum()
{
    return Crc32Checksum( (VOID*)m_pUserSettings, sizeof( XBOX_USER_SETTINGS ) );
}


// Get a checksum for the Encrypted settings
DWORD CXSettings::GetNewEncryptedChecksum()
{
    return Crc32Checksum( (VOID*)m_pEncSettings, sizeof( XBOX_ENCRYPTED_SETTINGS ) );
}



//
// TODO: Move these to a library
//

//
// 32 bit ANSI X3.66 CRC checksum table - polynomial 0xedb88320
//
static const DWORD Crc32Table[] = {
    0x00000000, 0x77073096, 0xee0e612c, 0x990951ba, 0x076dc419, 0x706af48f, 0xe963a535, 0x9e6495a3,
    0x0edb8832, 0x79dcb8a4, 0xe0d5e91e, 0x97d2d988, 0x09b64c2b, 0x7eb17cbd, 0xe7b82d07, 0x90bf1d91,
    0x1db71064, 0x6ab020f2, 0xf3b97148, 0x84be41de, 0x1adad47d, 0x6ddde4eb, 0xf4d4b551, 0x83d385c7,
    0x136c9856, 0x646ba8c0, 0xfd62f97a, 0x8a65c9ec, 0x14015c4f, 0x63066cd9, 0xfa0f3d63, 0x8d080df5,
    0x3b6e20c8, 0x4c69105e, 0xd56041e4, 0xa2677172, 0x3c03e4d1, 0x4b04d447, 0xd20d85fd, 0xa50ab56b,
    0x35b5a8fa, 0x42b2986c, 0xdbbbc9d6, 0xacbcf940, 0x32d86ce3, 0x45df5c75, 0xdcd60dcf, 0xabd13d59,
    0x26d930ac, 0x51de003a, 0xc8d75180, 0xbfd06116, 0x21b4f4b5, 0x56b3c423, 0xcfba9599, 0xb8bda50f,
    0x2802b89e, 0x5f058808, 0xc60cd9b2, 0xb10be924, 0x2f6f7c87, 0x58684c11, 0xc1611dab, 0xb6662d3d,
    0x76dc4190, 0x01db7106, 0x98d220bc, 0xefd5102a, 0x71b18589, 0x06b6b51f, 0x9fbfe4a5, 0xe8b8d433,
    0x7807c9a2, 0x0f00f934, 0x9609a88e, 0xe10e9818, 0x7f6a0dbb, 0x086d3d2d, 0x91646c97, 0xe6635c01,
    0x6b6b51f4, 0x1c6c6162, 0x856530d8, 0xf262004e, 0x6c0695ed, 0x1b01a57b, 0x8208f4c1, 0xf50fc457,
    0x65b0d9c6, 0x12b7e950, 0x8bbeb8ea, 0xfcb9887c, 0x62dd1ddf, 0x15da2d49, 0x8cd37cf3, 0xfbd44c65,
    0x4db26158, 0x3ab551ce, 0xa3bc0074, 0xd4bb30e2, 0x4adfa541, 0x3dd895d7, 0xa4d1c46d, 0xd3d6f4fb,
    0x4369e96a, 0x346ed9fc, 0xad678846, 0xda60b8d0, 0x44042d73, 0x33031de5, 0xaa0a4c5f, 0xdd0d7cc9,
    0x5005713c, 0x270241aa, 0xbe0b1010, 0xc90c2086, 0x5768b525, 0x206f85b3, 0xb966d409, 0xce61e49f,
    0x5edef90e, 0x29d9c998, 0xb0d09822, 0xc7d7a8b4, 0x59b33d17, 0x2eb40d81, 0xb7bd5c3b, 0xc0ba6cad,
    0xedb88320, 0x9abfb3b6, 0x03b6e20c, 0x74b1d29a, 0xead54739, 0x9dd277af, 0x04db2615, 0x73dc1683,
    0xe3630b12, 0x94643b84, 0x0d6d6a3e, 0x7a6a5aa8, 0xe40ecf0b, 0x9309ff9d, 0x0a00ae27, 0x7d079eb1,
    0xf00f9344, 0x8708a3d2, 0x1e01f268, 0x6906c2fe, 0xf762575d, 0x806567cb, 0x196c3671, 0x6e6b06e7,
    0xfed41b76, 0x89d32be0, 0x10da7a5a, 0x67dd4acc, 0xf9b9df6f, 0x8ebeeff9, 0x17b7be43, 0x60b08ed5,
    0xd6d6a3e8, 0xa1d1937e, 0x38d8c2c4, 0x4fdff252, 0xd1bb67f1, 0xa6bc5767, 0x3fb506dd, 0x48b2364b,
    0xd80d2bda, 0xaf0a1b4c, 0x36034af6, 0x41047a60, 0xdf60efc3, 0xa867df55, 0x316e8eef, 0x4669be79,
    0xcb61b38c, 0xbc66831a, 0x256fd2a0, 0x5268e236, 0xcc0c7795, 0xbb0b4703, 0x220216b9, 0x5505262f,
    0xc5ba3bbe, 0xb2bd0b28, 0x2bb45a92, 0x5cb36a04, 0xc2d7ffa7, 0xb5d0cf31, 0x2cd99e8b, 0x5bdeae1d,
    0x9b64c2b0, 0xec63f226, 0x756aa39c, 0x026d930a, 0x9c0906a9, 0xeb0e363f, 0x72076785, 0x05005713,
    0x95bf4a82, 0xe2b87a14, 0x7bb12bae, 0x0cb61b38, 0x92d28e9b, 0xe5d5be0d, 0x7cdcefb7, 0x0bdbdf21,
    0x86d3d2d4, 0xf1d4e242, 0x68ddb3f8, 0x1fda836e, 0x81be16cd, 0xf6b9265b, 0x6fb077e1, 0x18b74777,
    0x88085ae6, 0xff0f6a70, 0x66063bca, 0x11010b5c, 0x8f659eff, 0xf862ae69, 0x616bffd3, 0x166ccf45,
    0xa00ae278, 0xd70dd2ee, 0x4e048354, 0x3903b3c2, 0xa7672661, 0xd06016f7, 0x4969474d, 0x3e6e77db,
    0xaed16a4a, 0xd9d65adc, 0x40df0b66, 0x37d83bf0, 0xa9bcae53, 0xdebb9ec5, 0x47b2cf7f, 0x30b5ffe9,
    0xbdbdf21c, 0xcabac28a, 0x53b39330, 0x24b4a3a6, 0xbad03605, 0xcdd70693, 0x54de5729, 0x23d967bf,
    0xb3667a2e, 0xc4614ab8, 0x5d681b02, 0x2a6f2b94, 0xb40bbe37, 0xc30c8ea1, 0x5a05df1b, 0x2d02ef8d
};

DWORD Crc32Checksum(const VOID* buf, DWORD bufsize)
{
    DWORD checksum = 0xffffffff;
    const BYTE* p = (const BYTE*) buf;

    while (bufsize--)
        checksum = Crc32Table[(checksum ^ *p++) & 0xff] ^ (checksum >> 8);

    return ~checksum;
}

/*
// Set the password of the Hard Drive
BOOL HDSetPassword( BYTE* password, DWORD dwLen )
{
    NTSTATUS status;
    HANDLE h;
    OBJECT_ATTRIBUTES obja;
    IO_STATUS_BLOCK iosb;
    OBJECT_STRING name;
    ULONG AtaPassThroughSize;
    ATA_PASS_THROUGH AtaPassThrough;
    IDE_SECURITY_DATA SecurityData;
    BOOL fReturn;
    DWORD cbBytes;

    RtlInitObjectString(&name, "\\Device\\Harddisk0\\Partition0");
    InitializeObjectAttributes(&obja, &name, 0, NULL, 0);

    status = NtOpenFile(&h, GENERIC_READ | GENERIC_WRITE | SYNCHRONIZE, &obja, &iosb,
        FILE_SHARE_WRITE | FILE_SHARE_READ, FILE_SYNCHRONOUS_IO_NONALERT);

    if (NT_SUCCESS(status)) {

        RtlZeroMemory(&AtaPassThrough, sizeof(ATA_PASS_THROUGH));
        RtlZeroMemory(&SecurityData, sizeof(IDE_SECURITY_DATA));

        AtaPassThrough.IdeReg.bHostSendsData = TRUE;
        AtaPassThrough.IdeReg.bCommandReg = IDE_COMMAND_SECURITY_SET_PASSWORD;
        AtaPassThrough.DataBufferSize = sizeof(IDE_SECURITY_DATA);
        AtaPassThrough.DataBuffer = &SecurityData;
        SecurityData.Maximum = TRUE;
        SecurityData.Master = FALSE;

        RtlCopyMemory(SecurityData.Password, password, dwLen);

        fReturn = DeviceIoControl(h, IOCTL_IDE_PASS_THROUGH,
            &AtaPassThrough, sizeof(ATA_PASS_THROUGH),
            &AtaPassThrough, sizeof(ATA_PASS_THROUGH),
            &cbBytes, NULL);

        if (!fReturn) {
            XDBGTRC("HDSetPassword", "API error!");
            return FALSE;
        } else if (AtaPassThrough.IdeReg.bCommandReg & 1) {
            XDBGTRC("HDSetPassword", "IDE error!");
            return FALSE;
        }

        NtClose(h);
    }

    return TRUE;
}
*/