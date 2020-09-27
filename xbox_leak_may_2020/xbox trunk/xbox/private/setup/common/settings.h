//  SETTINGS.H
//
//  Created 24-Mar-2001 [JonT]

#ifndef _SETTINGS_H
#define _SETTINGS_H

//---------------------------------------------------------------------
//  CSettingsFile
//      Manages the .INI file associated with the package

typedef struct _KEYINFO
{
    LPTSTR lpKey;
    LPTSTR lpValue;
} KEYINFO, *LPKEYINFO;

typedef struct _SECTIONENUM
{
    DWORD dwcKeys;
    KEYINFO keys[1];
} SECTIONENUM;
typedef SECTIONENUM* LPSECTIONENUM;


class CSettingsFile
{
private:
    LPTSTR m_lpIniFile;
    LPTSTR m_lpEnumAll;
    LPTSTR m_lpEnum;
    BOOL m_fMakeCopy;

public:
    CSettingsFile()
    {
        m_lpIniFile = NULL;
        m_lpEnumAll = NULL;
        m_fMakeCopy = FALSE;
    }
    ~CSettingsFile();

    // Sets the name of INI file to use
    BOOL SetFile(LPCTSTR lpIniFile, BOOL fMakeCopy=FALSE);

    // Return name of ini file or copy
    LPCTSTR GetFile() { return m_lpIniFile; }

    // Done with our copy of the file if any
    void Close();

    // Returns a string from the INI file correctly processed for
    // string and environment variable substitution
    LPTSTR GetString(DWORD dwSection, DWORD dwKey);
    LPTSTR GetString(DWORD dwSection, LPCTSTR lpKey);
    LPTSTR GetSetting(DWORD dwKey);
    LPTSTR GetSetting(LPCSTR lpKey);

    // Writes a string
    BOOL WriteString(DWORD dwSection, DWORD dwKey, LPCTSTR lpValue);
    BOOL WriteString(DWORD dwSection, LPCTSTR lpKey, LPCTSTR lpValue);
    BOOL WriteSetting(DWORD dwKey, LPCTSTR lpValue);

    // Wipes out a section
    VOID NukeSection(DWORD dwSection);
    VOID NukeINISection(LPCTSTR lpSection);

    // Section enumeration: returns a dynamically allocated array of
    // pointers into a static buffer where the section is read in
    LPSECTIONENUM EnumerateSection(DWORD dwSection);
    void EnumerateFree(LPSECTIONENUM lpEnum);

    // Versions of the methods that don't use the stringtable.
    LPTSTR GetINIString(LPCTSTR lpSection, LPCTSTR lpKey);
    BOOL WriteINIString(LPCTSTR lpSection, LPCTSTR lpKey, LPCTSTR lpValue);
    LPSECTIONENUM EnumerateINISection(LPCTSTR lpSection);

    static const LPCTSTR m_strTable[];
};

#define CSTR_PACKERSETTINGS     0
#define CSTR_STRINGS            1
#define CSTR_CABPATH            2
#define CSTR_MANIFEST           3
#define CSTR_TARGETDIRECTORIES  4
#define CSTR_PACKINGLIST        5
#define CSTR_UNPACKERTEMPLATE   6
#define CSTR_UNPACKERNEWEXE     7
#define CSTR_DIRSIZES           8
#define CSTR_UNPACKSETTINGS     9
#define CSTR_UNINSTALLDIR       10
#define CSTR_DIRFLAGS           11
#define CSTR_LICENSEFILE        12
#define CSTR_BUILDFLAVOR        13
#define CSTR_PACKERPATHS        14
#define CSTR_PRODUCTKEY         15
#define CSTR_DEFAULTDIRTEXT     16
#define CSTR_UNINSTALLER        17
#define CSTR_STARTMENUFOLDER    18
#define CSTR_COMPRESSIONTYPE    19
#define CSTR_COMPRESSIONLEVEL   20
#define CSTR_MINVERSIONALLOWED  21
#define CSTR_UPDATEDATESTO      22
#define CSTR_CUSTOMDLL          23
#define CSTR_PRODUCTNAME        24
#define CSTR_WELCOMEMSG         25
#define CSTR_PRODUCTDESC        26
#define CSTR_LICENSE_TITLE      27
#define CSTR_LICENSE_SUBTITLE   28
#define CSTR_LICENSE_INSTRUCT   29
#define CSTR_SKIP_LICENSE_PAGE  30
#define CSTR_TOTALINSTALLSIZE   31
#define CSTR_LANGUAGES          32
#define CSTR_DEFAULTLANGUAGE    33

#define CSTR_LICENSE_MAX_FILES  200

#ifdef UNPACKER
#define CSTR_SETTINGS CSTR_UNPACKSETTINGS
#else
#define CSTR_SETTINGS CSTR_PACKERSETTINGS
#endif

#ifdef SETTINGS
    // Initialize the section and key names
    const LPCTSTR CSettingsFile::m_strTable[] =
    {
        TEXT("PackerSettings"),
        TEXT("Strings"),
        TEXT("CabPath"),
        TEXT("Manifest"),
        TEXT("UnpackerPaths"),
        TEXT("PackingList"),
        TEXT("UnpackerTemplate"),
        TEXT("UnpackerNewExe"),
        TEXT("UnpackerDirSizes"),
        TEXT("UnpackerSettings"),
        TEXT("UninstallDir"),
        TEXT("UnpackerDirFlags"),
        TEXT("LicenseFile"),
        TEXT("BuildFlavor"),
        TEXT("PackerPaths"),
        TEXT("ProductKey"),
        TEXT("DefaultDirText"),
        TEXT("Uninstaller"),
        TEXT("StartMenuFolder"),
        TEXT("CompressionType"),
        TEXT("CompressionLevel"),
        TEXT("MinVersionAllowed"),
        TEXT("UpdateDatesToAtLeast"),
        TEXT("CustomDll"),
        TEXT("ProductName"),
        TEXT("WelcomeMessage"),
        TEXT("ProductDescription"),
        TEXT("LicenseTitle"),
        TEXT("LicenseSubtitle"),
        TEXT("LicenseInstruction"),
        TEXT("SkipLicensePage"),
        TEXT("TotalInstallSize"),
        TEXT("Languages"),
        TEXT("DefaultLanguage")
    };
#endif

inline LPTSTR
CSettingsFile::GetSetting(
    DWORD dwKey
    )
{
    return GetINIString(m_strTable[CSTR_SETTINGS], m_strTable[dwKey]);
}

inline LPTSTR
CSettingsFile::GetSetting(
    LPCSTR lpKey
    )
{
    return GetINIString(m_strTable[CSTR_SETTINGS], lpKey);
}

inline LPTSTR
CSettingsFile::GetString(
    DWORD dwSection,
    DWORD dwKey
    )
{
    return GetINIString(m_strTable[dwSection], m_strTable[dwKey]);
}

inline LPTSTR
CSettingsFile::GetString(
    DWORD dwSection,
    LPCTSTR lpKey
    )
{
    return GetINIString(m_strTable[dwSection], lpKey);
}

inline VOID
CSettingsFile::NukeSection(
    DWORD dwSection
    )
{
    NukeINISection(m_strTable[dwSection]);
}

inline LPSECTIONENUM
CSettingsFile::EnumerateSection(
    DWORD dwSection
    )
{
    return EnumerateINISection(m_strTable[dwSection]);
}


inline BOOL
CSettingsFile::WriteString(
    DWORD dwSection,
    DWORD dwKey,
    LPCTSTR lpValue
    )
{
    return WriteINIString(m_strTable[dwSection], m_strTable[dwKey], lpValue);
}

inline BOOL
CSettingsFile::WriteString(
    DWORD dwSection,
    LPCTSTR lpKey,
    LPCTSTR lpValue
    )
{
    return WriteINIString(m_strTable[dwSection], lpKey, lpValue);
}

inline BOOL
CSettingsFile::WriteSetting(
    DWORD dwKey,
    LPCTSTR lpValue
    )
{
    return WriteINIString(m_strTable[CSTR_SETTINGS], m_strTable[dwKey], lpValue);
}

#endif // #ifndef _SETTINGS_H
