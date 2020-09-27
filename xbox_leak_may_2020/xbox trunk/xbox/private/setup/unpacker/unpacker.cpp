//  UNPACKER.CPP
//
//  Created 13-Mar-2001 [JonT]

#include "unpacker.h"

//---------------------------------------------------------------------

int WINAPI
WinMain(
    HINSTANCE hInst,
    HINSTANCE hPrevInst,
    LPSTR lpCmdLine,
    int nCmdShow
    )
{
    CUnpacker app(hInst, lpCmdLine);
    CWizard wiz;
    int nRet;


#ifdef TEST_JPN
    SetThreadLocale(MAKELCID(MAKELANGID(LANG_JAPANESE, SUBLANG_NEUTRAL), SORT_DEFAULT));
#endif
    
    // Make sure we just have one installer running at a time
    static HANDLE s_hevNamespace = CreateEvent(NULL, TRUE, TRUE, _TEXT("Local\\X_SETUP_ENGINE"));
    if (GetLastError() == ERROR_ALREADY_EXISTS)
    {
        TCHAR szTitle[MAX_PATH];
        TCHAR szError[MAX_PATH];
        LoadString(hInst, IDS_WIZARD_TITLE, szTitle, MAX_PATH);
        LoadString(hInst, IDS_ERRORTWOINSTALLERS, szError, MAX_PATH);
        MessageBox(NULL, szError, szTitle, MB_OK);

        return -1;
    }

    // Initialize COM
    CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);

    // Process command line
    if (!app.CommandLine(lpCmdLine))
        return -1;

    // Unpack the settings cab
    if(!app.UnpackSettingsCab())
        return -1;
    
    // We are about to run the wizard
    SetWizRunningState(TRUE);

    // Drive everything through the wizard
    nRet = wiz.DoModal(NULL);

    // The wizard has returned
    SetWizRunningState(FALSE);

    // If we were successful, call the custom postsetup stuff
    if (nRet != -1)
        app.PostSetup(nRet);

    CloseHandle(s_hevNamespace);

    // Reboot if the wizard told us to.
    if(1==nRet)
        Reboot();

    return 0;
}


//---------------------------------------------------------------------
//  CUnpacker class


// Static member variables and contained classes
    HINSTANCE CUnpacker::m_shinst = NULL;
    HINSTANCE CUnpacker::m_scustomhinst = NULL;
    CUnpacker* CUnpacker::m_spThis = NULL;
    CCustom*   CUnpacker::m_spCustom = NULL;
    CSettingsFile CUnpacker::m_settings;
    BOOL          CUnpacker::m_sfWizardRunning=FALSE;
    
    CUninstall CUnpacker::m_uninstall;
    CManifest CUnpacker::m_manifest(&CUnpacker::m_settings);
    CDirectories CUnpacker::m_dirs(&CUnpacker::m_settings);

//  CUnpacker::CUnpacker
//      Singleton application class

CUnpacker::CUnpacker(
    HINSTANCE hinst,
    LPSTR lpCommandline
    )
{
    m_shinst = hinst;
    m_spThis = this;
    m_fTempDir = FALSE;
    m_lpCommandline = lpCommandline;
    m_ProductRegKey.hRootKey = NULL;
    m_ProductRegKey.lpKey = NULL;
    m_ProductRegKey.lpSubkey = NULL;
    m_lpszProductName = NULL;
    m_dwDefaultLang = 0;


    m_szUninstallDir[0] = 0;
    m_szDefaultDir[0] = 0;
    m_dwFlags = 0;
    m_szCabSource[0] = 0;
}


CUnpacker::~CUnpacker()
{
    if(m_scustomhinst)
        FreeLibrary(m_scustomhinst);
    if (m_fTempDir)
        DeleteDirectory(m_szUninstallDir);
    if(m_lpszProductName)
        free(m_lpszProductName);
    if(m_ProductRegKey.lpKey)
        free(m_ProductRegKey.lpKey);
}


//  CUnpacker::CommandLine
//      Processes the command line and sets flags on the CUnpacker object

BOOL
CUnpacker::CommandLine(
    LPSTR lpCmdLine
    )
{
    LPSTR lp;
    LPSTR lpNew;
    TCHAR szTitle[MAX_PATH];
    TCHAR szUsage[2048];
    BOOL fQuote;

    // Scan through looking for slash or -
    for (lp = lpCmdLine ; *lp ; ++lp)
    {
        if (*lp == '-' || *lp == '/')
        {
            if (*++lp == 0)
                break;
            switch (*lp)
            {
            case '?':
                LoadString(::GetInstanceHandle(), IDS_WIZARD_TITLE, szTitle, MAX_PATH);
                LoadString(::GetInstanceHandle(), IDS_MB_USAGE, szUsage, 2048);
                MessageBox(NULL, szUsage, szTitle, MB_OK);
                return FALSE;
            
            case 'f':
            case 'F':
                m_dwFlags |= UNPACKER_FLAGS_FORCEINSTALL;
                break;

            case 's':
            case 'S':
                m_dwFlags |= UNPACKER_FLAGS_SKIPPRESETUPCHECKS;
                break;

            case 'e':
            case 'E':
                ++lp; // skip 'e' character

                // skip whitespace
                while (*lp == ' ' || *lp == '\t')
                    ++lp;
                fQuote = (*lp == '"');
                if (fQuote)
                    ++lp;

                // Loop through the string copying it into our exe name buffer
                lpNew = m_szCabSource;
                while (*lp != 0)
                {
                    if (fQuote)
                    {
                        if (*lp == '"')
                        {
                            ++lp;
                            break;
                        }
                    }
                    else
                    {
                        if (*lp == ' ' || *lp == '\t')
                            break;
                    }
                    *lpNew++ = *lp++;
                }
                *lpNew = 0;
                break;
            }
        }
    }

    return TRUE;
}

//  CUnpacker::UnpackSettingsCab
//      Unpacks the first cab. This one has the manifest, the settings file,
//      and the custom dll.
//      Leaves the files in a temporary directory.

BOOL
CUnpacker::UnpackSettingsCab()
{
    PFNGETCUSTOM pfnGetCustom;
    LPSTR        lpProductKeyName;
    TCHAR szPath[MAX_PATH];
    CCab cabSettings(&m_settings, &m_dirs, &m_uninstall, this);

    // Initialize the settings cabfile
    if (!cabSettings.Init())
    {
        DebugPrint("No cab resources: this executable hasn't been packed\n");
        SetLastError(IDS_UNPACKERNOTPACKED, 1001);
        return FALSE;
    }

    // Create the directory we're going to write the settings cabfile to
    GetTempPath(MAX_PATH, m_szUninstallDir);
    GetTempFileName(m_szUninstallDir, "unp", 0, m_szUninstallDir);
    DeleteFile(m_szUninstallDir);
    ::AppendSlash(m_szUninstallDir);
    if (!CreateDirectory(m_szUninstallDir, NULL))
    {
        DebugPrint("CreateDirectory of %s failed (error=%08x)\n", m_szUninstallDir, GetLastError());
        SetLastError(IDS_CORRUPT_INSTALLER, 1002);
        return FALSE;
    }
    m_fTempDir = TRUE;

    // Unpack the settings cabfile into the temporary directory. Note that the settings cabfile
    // is the second file in the list, and in the zero-based parameter, this is one.
    cabSettings.ProcessNoManifest(m_szUninstallDir, 1);
    cabSettings.Close();

    // Now try to point to the settings file
    strcpy(szPath, m_szUninstallDir);
    ::AppendSlash(szPath);
    strcat(szPath, SCAB_SETTINGS);
    if (!m_settings.SetFile(szPath, FALSE))
    {
        DebugPrint("Couldn't locate settings file\n");
        SetLastError(IDS_UNPACKERNOTPACKED, 1003);
        return FALSE;
    }

    // Read the supported languages
    LPSTR lpLangs = m_settings.GetSetting(CSTR_LANGUAGES);
    if(!*lpLangs)
    {
        SetLastError(IDS_UNPACKERNOTPACKED, 1091);
        return FALSE;
    }
    m_SupportedLangs.ParseLangIDs( lpLangs );

    // Read the default language
    LPSTR lpDefaultLang = m_settings.GetSetting(CSTR_DEFAULTLANGUAGE);
    if(!*lpDefaultLang)
    {
        SetLastError(IDS_UNPACKERNOTPACKED, 1091);
        return FALSE;
    }
    LPSTR lpStopScan = NULL;
    m_dwDefaultLang = strtol( lpDefaultLang, &lpStopScan, 16 );

    // Read the product name
    m_lpszProductName = m_settings.GetSetting(CSTR_PRODUCTNAME);
    if(!*m_lpszProductName)
    {
        SetLastError(IDS_UNPACKERNOTPACKED, 1091);
        return FALSE;
    }
    
    //  Read the product key
    lpProductKeyName= m_settings.GetSetting(CSTR_PRODUCTKEY);
    if(!*lpProductKeyName)
    {
        free(lpProductKeyName);
        SetLastError(IDS_UNPACKERNOTPACKED, 1089);
        return FALSE;
    }
    ParseLine(lpProductKeyName, (LPSTR*)&m_ProductRegKey, INIREGKEY_DWORDS, NULL);

    // Get the root key, from the equivalent strings
    if (_stricmp(m_ProductRegKey.lpKey, "HKLM") == 0)
        m_ProductRegKey.hRootKey = HKEY_LOCAL_MACHINE;
    else if (_stricmp(m_ProductRegKey.lpKey, "HKCU") == 0)
        m_ProductRegKey.hRootKey = HKEY_CURRENT_USER;
    else
    {
       SetLastError(IDS_UNPACKERNOTPACKED, 1090);
       return FALSE;
    }

    // Try to load library the custom dll.
    strcpy(szPath, m_szUninstallDir);
    ::AppendSlash(szPath);
    strcat(szPath, SCAB_CUSTOMDLL);
    m_scustomhinst = LoadLibrary(szPath);
    if(!m_scustomhinst)
    {
        DebugPrint("No custom dll\n");
        return TRUE;
    }

    //  Get a pointer to CCustom
    pfnGetCustom = (PFNGETCUSTOM) GetProcAddress(m_scustomhinst, GETCUSTOMPROC);
    if(pfnGetCustom)
    {
        m_spCustom = pfnGetCustom(this);
    }

    return TRUE;
}


//  CUnpacker::ProcessDirectories
//      Walks the target directories and fills them in the best way possible.
//      If we return TRUE, this is an upgrade, FALSE if it's a new install.

BOOL
CUnpacker::ProcessDirectories()
{
    // walk through the directories, setting everything possible
    m_dirs.ParseTargetDirs();

    // Read the the install path. If we find it, we're good and this is an upgrade, return TRUE;
    // If not, this is not an upgrade.  The UI will set the directory instead, return FALSE;
    return GetRegistryString(m_szDefaultDir, MAX_PATH, m_ProductRegKey.hRootKey, m_ProductRegKey.lpSubkey, XSETUP_INSTALL_REGVALUE);
}

//  CreateVersionString
//      Creates a version string from four WORD values
//      Parameter string must be long enough to accept it (MAX_PATH is plenty safe)

void
CreateVersionString(
    LPSTR lp,
    WORD w1,
    WORD w2,
    WORD w3,
    WORD w4
    )
{
    sprintf(lp, "%d,%d,%d,%d", (DWORD)w1, (DWORD)w2, (DWORD)w3, (DWORD)w4);
}

VOID
CUnpacker::WriteInstallInfo()
{
    HKEY hKey;
    SYSTEMTIME st;
    FILETIME   ft;
    char  szBuild[MAX_PATH];
    
    // Write the registry key out
    if (ERROR_SUCCESS==::RegCreateKeyEx(m_ProductRegKey.hRootKey, m_ProductRegKey.lpSubkey, 0, NULL, 0, KEY_ALL_ACCESS, NULL, &hKey, NULL))
    {
        // Write the product key values (product name, product version, install path and install time
        RegSetValueEx(hKey, XSETUP_PRODUCT_NAME_REGVALUE, 0, REG_SZ, (LPBYTE)m_lpszProductName, strlen(m_lpszProductName) + 1);
        CreateVersionString(szBuild, VER_PRODUCTVERSION); // VER_PRODUCTVERSION contains 4 parameters
        RegSetValueEx(hKey, XSETUP_FULL_BUILD_REGVALUE, 0, REG_SZ, (LPBYTE)szBuild, strlen(szBuild) + 1);
        RegSetValueEx(hKey, XSETUP_INSTALL_REGVALUE, 0, REG_SZ, (LPBYTE)m_szDefaultDir, strlen(m_szDefaultDir) + 1);
        GetLocalTime(&st);
        SystemTimeToFileTime(&st, &ft);
        RegSetValueEx(hKey, XSETUP_INSTALLTIME_REGVALUE, 0, REG_QWORD, (LPBYTE)&(((LARGE_INTEGER*)&ft)->QuadPart), sizeof (__int64));
        RegCloseKey(hKey);
    }

    // BUGBUG: This shouldn't be necessary. Uninstall should be reading and erasing these automatically.
    m_uninstall.AddReg(m_ProductRegKey.lpKey, m_ProductRegKey.lpSubkey, XSETUP_PRODUCT_NAME_REGVALUE);
    m_uninstall.AddReg(m_ProductRegKey.lpKey, m_ProductRegKey.lpSubkey, XSETUP_FULL_BUILD_REGVALUE);
    m_uninstall.AddReg(m_ProductRegKey.lpKey, m_ProductRegKey.lpSubkey, XSETUP_INSTALL_REGVALUE);
    m_uninstall.AddReg(m_ProductRegKey.lpKey, m_ProductRegKey.lpSubkey, XSETUP_INSTALLTIME_REGVALUE);
}

//
//  This handles legacy installs of the XDK, before moving to
//  this setup engine.  Instead of an InstallPath there was an
//  SDK Doc Path.  So we read this, munge it to produce the proper
//  install path and write it back out.
//
VOID CUnpacker::ProcessLegacySetup()
{
    HKEY hKey;
    // open the product key.
    if (RegOpenKey(m_ProductRegKey.hRootKey, m_ProductRegKey.lpSubkey, &hKey) == ERROR_SUCCESS)
    {
        // Try to read the install time.  If the install time is there than this is not a legacy setup.
        FILETIME ft;
        DWORD dwType;
        DWORD dwLen = sizeof (ft);
        if (RegQueryValueEx(hKey, XSETUP_INSTALLTIME_REGVALUE, NULL, &dwType,
                            (LPBYTE)&ft, &dwLen) == ERROR_SUCCESS && dwType == REG_QWORD)
        {
            // Also set this flag UNPACKER_FLAGS_SETUPHASRUNBEFORE.
            // This tells us to skip a bunch of commands in .csv file
            SetFlags(UNPACKER_FLAGS_SETUPHASRUNBEFORE);
        } else
        {
            // Check for the key that old installshield setup put in for the XDK. If we find it,
            // rewrite it as the new key so we still upgrade to the same directory as before
            TCHAR szOldPath[MAX_PATH];
            dwLen = sizeof (szOldPath);
            if (RegQueryValueEx(hKey, XDK_OLD_INSTALL_REGVALUE, NULL, &dwType,
                                (LPBYTE)szOldPath, &dwLen) == ERROR_SUCCESS && dwType == REG_SZ)
            {
                // We have the old string. Strip the last path component off (since this was
                // the 'doc path'
                LPTSTR lp = szOldPath + strlen(szOldPath) - 1;
                for (; lp > szOldPath && *lp != '\\' && *lp != ':' ; --lp)
                    ;
                if (*lp == '\\')
                    *lp = 0;
                else if (*lp == ':')
                    *(lp + 1) = 0;
                ::AppendSlash(szOldPath);

                // Write the string back out in the new way
                RegSetValueEx(hKey, XSETUP_INSTALL_REGVALUE, 0, REG_SZ, (LPBYTE)szOldPath, strlen(szOldPath) + 1);
            }
        }
        RegCloseKey(hKey);
    }
    return;
}

//  CUnpacker::DoPreSetupChecks
//     Called to verify that setup may proceed on the current system.
//     The following checks are performed:
//        Disk Space, User Privileges, Previous Version (for QFE
//        packages).
//
//

BOOL 
CUnpacker::DoPreSetupChecks()
{
    // Skip all presetup checks if the flag was passed in to tell us so...
    // This is an "udocumented, at your own risk" feature.
    if (GetFlags() & UNPACKER_FLAGS_SKIPPRESETUPCHECKS)
        return TRUE;
    
    // Check for administrator
    if (!IsAdministrator())
    {
        SetLastError(IDS_ERRORNOTADMINISTRATOR, 2007);
        return FALSE;
    }

    // Check Windows Version
    if (!CheckWindows())
        return FALSE;

    // Check Upgrade Version
    if (!CheckUpgradeVersion())
        return FALSE;

    //Perform Custom PreSetup checks if any.
    if(m_spCustom && !m_spCustom->DoPreSetupChecks())
        return FALSE;

    // The Japanese build should not be run on a machine without the locale ID set to
    // Japanese or the fonts won't work correctly.
#ifdef UNPACKER_JPN
    LCID lcid = GetThreadLocale();
    LCID lcidCompare = MAKELCID(MAKELANGID(LANG_JAPANESE, SUBLANG_NEUTRAL), SORT_DEFAULT);
    lcidCompare = ConvertDefaultLocale(lcidCompare);
    if (lcid != lcidCompare)
    {
        SetLastError(IDS_JAPANESENOTAVAILABLE, 2015);
        return FALSE;
    }
#endif
    return TRUE;
}


//  CUnpacker::CheckForOtherProduct
//      Due to the way uninstall works only one Xbox Install Technology product
//      can be installed to a single directory.  A product is different if it 
//      should be separately uninstallable.  This routine works by opening the
//      settings.ini file in the uninstall directory and checking the product's
//      default install directory.  If it is the same as this products default
//      install directory the routine turn false.  Additionally, lpOtherProduct
//      is filled in with the product name of the product that occupies lpTarget.
//
BOOL 
CUnpacker::CheckForOtherProduct(LPSTR lpTarget, LPSTR lpOtherProduct, DWORD cbOtherProduct)
{
    BOOL  fAllowInstall = TRUE;
    LPSTR lpOtherProductDefaultInstallDir;
    LPSTR lpDefaultInstallDir;
    
    //Until we find another product, there isn't one.
    *lpOtherProduct = '\0';

    // Get the path to where the settings file would be if there is another
    // product installed at lpTarget.
    TCHAR szOtherProductsSettingsPath[MAX_PATH];
    strcpy(szOtherProductsSettingsPath, lpTarget);
    ::AppendSlash(szOtherProductsSettingsPath);
    strcat(szOtherProductsSettingsPath, SCAB_UNINSTALLDIR);
    ::AppendSlash(szOtherProductsSettingsPath);
    strcat(szOtherProductsSettingsPath, SCAB_SETTINGS);
    
    // Create a CSettingsFile and set the name.  Note that
    // this does not actually open the file, so that this
    // cannot fail with the args we are using.
    CSettingsFile otherProductSettings;
    otherProductSettings.SetFile(szOtherProductsSettingsPath);
    lpOtherProductDefaultInstallDir = otherProductSettings.GetSetting(CSTR_DEFAULTDIRTEXT);
    if(lpOtherProductDefaultInstallDir)
    {
        // If the other product string exists (is not an empty string)
        // Compare it to this product's default install dir.
        if(*lpOtherProductDefaultInstallDir)
        {
            // Get this products default install dir.
            lpDefaultInstallDir = m_settings.GetSetting(CSTR_DEFAULTDIRTEXT);
            if(lpDefaultInstallDir)
            {
                //If the default install directories are not identical, these are
                //different products, don't allow installation to the same directory.
                if(strcmp(lpOtherProductDefaultInstallDir, lpDefaultInstallDir))
                {
                    fAllowInstall = FALSE;
                    //Fill out the other product's name so that the wizard may
                    //Display it in the UI.
                    LPSTR lpOtherProductTemp = otherProductSettings.GetSetting(CSTR_PRODUCTNAME);
                    if(lpOtherProductTemp)
                    {
                        if(*lpOtherProductTemp)
                        {
                            strncpy(lpOtherProduct, lpOtherProductTemp, cbOtherProduct);
                        } else
                        {
                            strncpy(lpOtherProduct, lpOtherProductDefaultInstallDir, cbOtherProduct);
                        }
                        free(lpOtherProductTemp);
                    }
                }
                free(lpDefaultInstallDir);
            }
        }
        free(lpOtherProductDefaultInstallDir);
    }
    return fAllowInstall;
}

//  CUnpacker::ValidateDirectories
//      Called to let us validate all the directories we've parsed out
//      of the settings file. If things aren't good, we need to be able to
//      put up a descriptive error message before going on.
//      If we return FALSE, things aren't good. We've set an error string
//      with WizSetLastError before returning.

BOOL
CUnpacker::ValidateDirectories()
{
    LPTARGETDIR lpTarget;
    DWORD dwc;
    DWORD i;

    // Get the pointer to the first target and count
    lpTarget = m_dirs.AllTargets(&dwc);

    // Walk through all the targets validating them
    for (i = 0 ; i < dwc ; ++i, ++lpTarget)
    {
        // Fill in the DEFAULT command paths. We couldn't do this earlier
        // because we didn't have it from the user yet.
        if (_stricmp(lpTarget->lpCommand, "DEFAULT") == 0)
            strcpy(lpTarget->szPath, m_szDefaultDir);

        // Call the custom directory routine to give it a chance to
        // either fill in the path or return an error (and set the error ID)
        if (m_spCustom && !m_spCustom->ValidateDirectory(m_szDefaultDir, lpTarget->lpID,
                           lpTarget->lpCommand, lpTarget->lpData, lpTarget->szPath))
        {
            SetLastError(IDS_CORRUPT_INSTALLER, 1018);
            return FALSE;
        }

        // As a sanity check, make sure there's something in the path or
        // do the general error ourselves
        if (lpTarget->szPath[0] == 0)
        {
            SetLastError(IDS_CORRUPT_INSTALLER, 1004);
            return FALSE;
        }
    }

    return TRUE;
}


//  CUnpacker::MoveSettingsCab
//      The settings cab was originally expanded into a temp directory
//      because we didn't know where to put it. Now that we know where
//      it's supposed to go, move it there

BOOL
CUnpacker::MoveSettingsCab()
{
    TCHAR szPath[MAX_PATH];

    // Build uninstall directory
    strcpy(szPath, m_szDefaultDir);
    ::AppendSlash(szPath);
    strcat(szPath, SCAB_UNINSTALLDIR);
    ::AppendSlash(szPath);

    // Move all settings to the new uninstall directory, deleting the tempdir
    if (!MoveDirectory(m_szUninstallDir, szPath))
    {
        SetLastError(IDS_CORRUPT_INSTALLER, 1007);
        return FALSE;
    }

    // We're done with the tempdir now. Make our 'uninstall' directory the new dir
    m_fTempDir = FALSE;
    strcpy(m_szUninstallDir, szPath);

    // Reset the settings file
    strcat(szPath, SCAB_SETTINGS);
    m_settings.SetFile(szPath);

    return TRUE;
}

//  CUnpacker::CheckDiskSpace
//      Checks disk space on all drives.
//      Displays UI (MessageBox) if there is inadequate space.
//      Returns 0 if the disk check was successful and there's adequate disk space.
//      Returns 1 if the disk check found that the disk space was exceeded by the
//          default directory. The user can change drives to remedy.
//      Returns -1 if the disk check finds that the disk space was exceeded by the
//          directories other than the default directory. They're screwed.

int
CUnpacker::CheckDiskSpace()
{
    if (!AccumulateFileSizes())
        return -1;

    return CheckAvailableSpace();
}


//  CUnpacker::StartCopy
//      Kicks off the background thread copy routine.
//      We communicate with the progress bar through globals since
//      the FDI routines call us with C functions.

DWORD WINAPI ThreadStart(CUnpacker* punpacker);

VOID
CUnpacker::StartCopy(
    HWND hwndDlg,
    HWND hwndProgress,
    HWND hwndActionText,
    HWND hwndInProgressText
    )
{
    LPTARGETDIR lpTarget;
    DWORD dwc;
    DWORD dwTotalSize;
    DWORD i;

    // Compute the total number of bytes to be copied
    lpTarget = m_dirs.AllTargets(&dwc);
    dwTotalSize = 0;
    for (i = 0 ; i < dwc ; ++i, ++lpTarget)
        dwTotalSize += lpTarget->dwDiskSpaceNeeded;

    // Set up the progress indicator
    SendMessage(hwndProgress, PBM_SETRANGE32, 0, dwTotalSize);
    SendMessage(hwndProgress, PBM_SETSTEP, 1, 0);
    SendMessage(hwndProgress, PBM_SETPOS, 0, 0);

    // Keep the hwnds around for use during the copy process
    m_dwBytesCopiedSoFar = 0;
    m_hwndProgress = hwndProgress;
    m_hwndDlg = hwndDlg;
    m_hwndActionText = hwndActionText;
    m_hwndInProgressText = hwndInProgressText;

    // Get a table of widths for each character so we don't have to do it per-file
    HDC hdc = GetDC(m_hwndInProgressText);
    HFONT hfont = (HFONT)SendMessage(m_hwndInProgressText, WM_GETFONT, 0, 0);
    HFONT hfontOld = (HFONT)SelectObject(hdc, hfont);
    GetCharABCWidths(hdc, 0, 255, m_abc);
    SelectObject(hdc, hfontOld);
    ReleaseDC(m_hwndInProgressText, hdc);

    // Initialize the uninstall info
    m_uninstall.Init(m_dwcManifestLines, m_szUninstallDir);

    // Kick off a thread to do the copying
    m_hthread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ThreadStart, (LPVOID)this, 0, &dwc);
}


//  CUnpacker::UpdateProgressBar
//      Updates the progress bar by adding the current number of bytes in

VOID
CUnpacker::UpdateProgressBar(
    DWORD dwSize,
    DWORD dwID,
    LPCTSTR lpText
    )
{
    if (m_hwndProgress)
    {
        PostMessage(m_hwndProgress, PBM_SETPOS, m_dwBytesCopiedSoFar, 0);
        if (dwSize > 0)
            m_dwBytesCopiedSoFar += ::DWORDtoDiskSpace(dwSize);
    }

    if (m_hwndActionText && dwID != 0)
    {
        TCHAR sz[MAX_PATH];
        LoadString(::GetInstanceHandle(), dwID, sz, MAX_PATH);
        SetWindowText(m_hwndActionText, sz);
    }

    if (m_hwndInProgressText && lpText != NULL)
    {
        TCHAR szEllipses[MAX_PATH];
        ConvertToEllipses(m_hwndInProgressText, szEllipses, lpText);
        SetWindowText(m_hwndInProgressText, szEllipses);
    }
}


//  CUnpacker::ConvertToEllipses
//      Makes a string fit in the edit control by whacking out the middle of the
//      string and replacing with ellipses

VOID
CUnpacker::ConvertToEllipses(
    HWND hwnd,
    LPSTR lpOut,
    LPCTSTR lpIn
    )
{
    HDC hdc = GetDC(hwnd);
    HFONT hfont = (HFONT)SendMessage(hwnd, WM_GETFONT, 0, 0);
    HFONT hfontOld = (HFONT)SelectObject(hdc, hfont);

    // Figure out how much space the current text would take
    SIZE size;
    DWORD dwcLen = strlen(lpIn);
    GetTextExtentPoint(hdc, lpIn, dwcLen, &size);

    // Get the size of the control. Fudge it by one char because the control probably isn't
    // an incremental char width wide.
    RECT rect;
    GetClientRect(hwnd, &rect);
    rect.right -= 8;

    // If the text will fit, copy it and go
    if (size.cx <= rect.right - rect.left)
    {
        strcpy(lpOut, lpIn);
    }

    // Otherwise, create the ellipses
    else
    {
        // Build the first part of the string
        strncpy(lpOut, lpIn, 5);
        lpOut[5] = 0;
        strcat(lpOut, "...");
        SIZE sizeFirst;
        GetTextExtentPoint(hdc, lpOut, 8, &sizeFirst);

        // Loop through all characters in the name assigning widths
        int nTotalWidth = rect.right - rect.left - sizeFirst.cx;
        LPCSTR lp = lpIn + strlen(lpIn) - 1;
        while (TRUE)
        {
            // Check for bail out case: we don't need ellipses after all!
            if (lp < lpIn + 5)
            {
                lp = NULL;
                break;
            }

            // See if we have made it big enough
            if (nTotalWidth <= 0)
                break;

            // Otherwise, subtract off this character's contribution and move to the next
            nTotalWidth -= m_abc[*lp].abcA + m_abc[*lp].abcB + m_abc[*lp].abcC;
            --lp;
        }

        // We're done: copy the bytes, clean up and get out
        if (lp == NULL)
            strcpy(lpOut, lpIn);
        else
            strcat(lpOut, lp + 1);
    }

    // Clean up
    SelectObject(hdc, hfontOld);
    ReleaseDC(hwnd, hdc);
}


//  CUnpacker::AccumulateFlags
//      Accumulate flags into a DWORD from a flag string.
//      Flag strings are strings composed of letters. Every letter (A-Z)
//      is mapped to a bit by doing 1 << (ch - 'A')

DWORD
CUnpacker::AccumulateFlags(
    LPCSTR lp
    )
{
    UCHAR ch;
    DWORD dwAccum;

    // Add flag values for each letter in the list
    for (dwAccum = 0 ; *lp != 0 ; ++lp)
    {
        ch = (*lp | 32) - 'a';
        if (ch < 26)
            dwAccum |= 1 << ch;
    }

    return dwAccum;
}

void CUnpacker::SetLastError(DWORD dwStringID, DWORD dwErrorCode)
{
    if(m_sfWizardRunning)
    {
        WizSetLastError(dwStringID, dwErrorCode);
        return;
    }

    // If the wizard is not running, just pop a MessageBox
    TCHAR szErrorText[4096];
    TCHAR szErrorCaption[MAX_PATH];
    TCHAR szErrorCodeText[10];

    // If there's is not set, display the general one
    if (dwStringID == 0) dwStringID = IDS_ERROR_GENERAL;
    if(m_lpszProductName)
    {
        strcpy(szErrorCaption, m_lpszProductName);
    } else
    {
        LoadString(GetInstanceHandle(), IDS_WIZARD_TITLE, szErrorCaption, MAX_PATH);
    }
    LoadString(GetInstanceHandle(), dwStringID , szErrorText, 4096);
    sprintf(szErrorCodeText, TEXT(": %d"), dwErrorCode);
    strcat(szErrorText, szErrorCodeText);
    MessageBox(NULL, szErrorText, szErrorCaption, MB_OK|MB_ICONSTOP);
}

void CUnpacker::SetLastErrorString(LPCTSTR lpString, DWORD dwErrorCode)
{
    if(m_sfWizardRunning)
    {
        WizSetLastErrorString(lpString, dwErrorCode);
        return;
    }
    
    // If the wizard is not running, just pop a MessageBox
    TCHAR szErrorCaption[MAX_PATH];
    if(m_lpszProductName)
    {
        strcpy(szErrorCaption, m_lpszProductName);
    } else
    {
        LoadString(GetInstanceHandle(), IDS_WIZARD_TITLE, szErrorCaption, MAX_PATH);
    }

    TCHAR szErrorText[4096];
    sprintf(szErrorText, TEXT("%s: %d"), lpString, dwErrorCode);
    MessageBox(NULL, szErrorText, szErrorCaption, MB_OK|MB_ICONSTOP);
}

//---------------------------------------------------------------------
//  Private routines


//  CUnpacker::MoveDirectory
//      Moves all the files in a directory to another and deletes the old directory

BOOL
CUnpacker::MoveDirectory(
    LPCTSTR lpSource,
    LPCTSTR lpDest
    )
{
    WIN32_FIND_DATA find;
    TCHAR szSourcePath[MAX_PATH];
    TCHAR szDestPath[MAX_PATH];
    TCHAR szFindPath[MAX_PATH];
    LPTSTR lpDestFile;
    LPTSTR lpSourceFile;
    HANDLE hfind;

    // Create the find path with a \*.* for the search
    strcpy(szFindPath, lpSource);
    ::AppendSlash(szFindPath);
    strcat(szFindPath, "*.*");

    // Create the source and dest dirs such that we can add the target filenames on
    strcpy(szSourcePath, lpSource);
    ::AppendSlash(szSourcePath);
    lpSourceFile = szSourcePath + strlen(szSourcePath);
    strcpy(szDestPath, lpDest);
    ::AppendSlash(szDestPath);
    lpDestFile = szDestPath + strlen(szDestPath);

    // Make sure the dest dir exists
    CCab::MakeDirectory(szDestPath);

    // Loop through all the files in source
    if ((hfind = FindFirstFile(szFindPath, &find)) != INVALID_HANDLE_VALUE)
    {
        do
        {
            // Skip . and ..
            if (find.cFileName[0] == '.')
                continue;

            // Skip any directories we might find
            if (find.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
            {
                DebugPrint("Directory found in uninstall dir! temp directory won't get deleted...\n");
                continue;
            }

            // For any other file, move it to the right spot
            *lpSourceFile = 0;
            strcat(lpSourceFile, find.cFileName);
            *lpDestFile = 0;
            strcat(lpDestFile, find.cFileName);
            if (!MoveFileEx(szSourcePath, szDestPath,
                MOVEFILE_COPY_ALLOWED | MOVEFILE_REPLACE_EXISTING))
            {
                DebugPrint("Error moving file from temp directory (error=%08x)\n", GetLastError());
            }
        }
        while (FindNextFile(hfind, &find));
    }
    FindClose(hfind);

    // Nuke the directory, don't worry if it fails, in at least
    // one case, we expect it to.
    RemoveDirectory(lpSource);

    return TRUE;
}


//  CUnpacker::DeleteDirectory
//      Called in case we need to nuke the temp directory before
//      we've had a chance to move it to the real place

void
CUnpacker::DeleteDirectory(
    LPCTSTR lpDir
    )
{
    HANDLE hfind;
    WIN32_FIND_DATA find;
    TCHAR szPath[MAX_PATH];

    // Create the find path with a \*.* for the search
    strcpy(szPath, lpDir);
    ::AppendSlash(szPath);
    strcat(szPath, "*.*");

    // Loop through all the files deleting them
    if ((hfind = FindFirstFile(szPath, &find)) != INVALID_HANDLE_VALUE)
    {
        do
        {
            // Skip . and ..
            if (find.cFileName[0] == '.')
                continue;

            // Skip any directories we might find
            if (find.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
            {
                DebugPrint("Directory found in uninstall dir! temp directory won't get deleted...\n");
                continue;
            }

            strcpy(szPath, lpDir);
            ::AppendSlash(szPath);
            strcat(szPath, find.cFileName);
            DeleteFile(szPath);
        }
        while (FindNextFile(hfind, &find));
        FindClose(hfind);
    }

    // Nuke the directory
    if (!RemoveDirectory(lpDir))
    {
        DebugPrint("Error removing temp directory (error=%08x)\n", GetLastError());
    }
}


//  CUnpacker::AddCommas
//      Adds commas in the disk size numbers

LPCTSTR
CUnpacker::AddCommas(
    __int64 li
    )
{
    static TCHAR sz[32];
    LPTSTR lp;
    DWORD i = 0;

    // Initialize and check for special case of zero
    sz[31] = 0;
    lp = &sz[30];
    if (li == 0)
    {
        *lp = '0';
        return lp;
    }

    // Loop until there are no more digits, adding digits and commas
    while (li)
    {
        *lp-- = (TCHAR)(UCHAR)(li % 10) + '0';
        li /= 10;
        if (++i == 3 && li != 0)
        {
            *lp-- = ',';
            i = 0;
        }
    }

    return lp + 1;
}


//  CUnpacker::CopyFiles
//      Called on a separate thread and copies all the files.

DWORD WINAPI
ThreadStart(
    CUnpacker* punpacker
    )
{
    // Initialize COM
    CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);

    DWORD dwRet = punpacker->CopyFiles();;
    CoUninitialize();
    return dwRet;
}

DWORD
CUnpacker::CopyFiles()
{
    CCab cabFiles(&m_settings, &m_dirs, &m_uninstall, this);

    // Initialize the file cab
    if (!cabFiles.Init())
    {
        PostCopyError(IDS_CORRUPT_INSTALLER, 1018);
        return -1;
    }

    // Copy all the files from the cab and other manifest-driven work
    // It's odd to be driving this through the cab object, but is done
    // because how FDI works: it makes you get called back once for each
    // file in the cab.
    // Note that the zero parameter is the zero-based index into the list
    // of files appended to the EXE. The file cab is the first file
    if (!cabFiles.ProcessWithManifest(&m_manifest, 0))
        return -1;      // We've already told the UI to skip to the error page

    // Update Version info in the registry
    WriteInstallInfo();

    // Do the custom stuff that's to be done after the copying is complete
    if(m_spCustom)
        m_spCustom->AfterCopy();

    // Write the uninstall information
    m_uninstall.Flush();

    // Since we're done and the thread is going to exit, tell the UI
    PostMessage(m_hwndDlg, WMP_COPYCOMPLETE, 0, 0);

    return 0;
}



//  CUnpacker::PostError
//      Tells the copying wizard page about a fatal error.

VOID
CUnpacker::PostCopyError(
    DWORD dwID,
    DWORD dwErrorCode
    )
{
    PostMessage(m_hwndDlg, WMP_ERROR, dwID, dwErrorCode);
}


//  CUnpacker::AccumulateFileSizes
//      Walks the manifest and adds in the size for each file already on the
//      disk into the manifest. Also accumulates the total size needed for
//      all files into the directory structure.

BOOL
CUnpacker::AccumulateFileSizes()
{
    LPSTR lp;
    LPSTR lpCommand;
    DWORD dwc;
    HANDLE hfile;
    LPTARGETDIR lpTarget;
    CFileInfo fi;
    LPSTR lpDest;
    LPSTR lpRoot;
    DWORD i;

    // Initialize the manifest
    TCHAR szPath[MAX_PATH];
    strcpy(szPath, m_szUninstallDir);
    strcat(szPath, SCAB_MANIFEST);
    if (!m_manifest.SetFile(szPath))
    {
        DebugPrint("Manifest.SetFile() failed.\n");
        GetUnpacker()->PostCopyError(IDS_CORRUPT_INSTALLER, 1019);
        return FALSE;
    }
    m_manifest.ResetPointer();
    m_dwcManifestLines = 0;

    // Make sure all the targets are initially zeroed
    lpTarget = m_dirs.AllTargets(&dwc);
    for (i = 0 ; i < dwc ; ++i, ++lpTarget)
    {
        lpTarget->dwDiskSpaceUsed = 0;
        lpTarget->dwDiskSpaceNeeded = 0;
    }

    // Walk through all lines in the manifest accumulating sizes
    do
    {
        // Get the line
        lp = m_manifest.GetLine();
        ++m_dwcManifestLines;

        // Skip the langIDs
        ::ZapComma(lp);

        // Get the command. We're only interested in 'file' or 'copy' commands
        lpCommand = lp;
        ::ZapComma(lp);
        if (_stricmp(lpCommand, "file") == 0)
        {
            // Parse the line
            PARSEFILE pf;
            ::ParseLine(lp, (LPSTR*)&pf, PARSEFILE_DWORDS, NULL);
            lpDest = pf.lpDest;
            lpRoot = pf.lpRoot;

            // Compute the size needed by the uncompressed file from the
            // file info in the manifest
            fi.Load(pf.lpFileInfo);
        }
        else if (_stricmp(lpCommand, "copy") == 0)
        {
            PARSECOPY pc;
            ::ParseLine(lp, (LPSTR*)&pc, PARSECOPY_DWORDS, NULL);
            lpDest = pc.lpDest;
            lpRoot = pc.lpRoot;

            // Compute the size needed by the uncompressed file from the
            // file info in the manifest
            fi.Load(pc.lpFileInfo);
        }

        // Not 'file' or 'copy,' skip
        else
            continue;

        // If we didn't find a target, the file is bogus
        lpTarget = m_dirs.FindTarget(lpRoot);
        if (lpTarget == NULL)
        {
            SetLastError(IDS_CORRUPT_INSTALLER, 1021);
            return FALSE;
        }

        strcpy(szPath, lpTarget->szPath);
        ::AppendSlash(szPath);
        strcat(szPath, lpDest);

        // Get the file size if it exists
        hfile = CreateFile(szPath, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE,
            NULL, OPEN_EXISTING, 0, NULL);
        if (hfile != INVALID_HANDLE_VALUE)
        {
            dwc = GetFileSize(hfile, NULL);
            CloseHandle(hfile);

            lpTarget->dwDiskSpaceUsed += ::DWORDtoDiskSpace(dwc);
        }
        lpTarget->dwDiskSpaceNeeded += ::DWORDtoDiskSpace(fi.GetSize());
    }
    while (!m_manifest.SkipToNextLine());

    return TRUE;
}


//  CUnpacker::CheckAvailableSpace
//      Checks all drives we're going to copy onto to see if they have
//      enough space.

int
CUnpacker::CheckAvailableSpace()
{
    DWORD i;
    DWORD j;
    int nDrives[26] = { 0 };
    static char szDrivePath[] = "x:";
    ULARGE_INTEGER liFree;
    ULARGE_INTEGER liTotal;
    ULARGE_INTEGER liNeeded;
    LPTARGETDIR lpTarget;
    DWORD dwc;

    // Walk through each dir and add into a drive map
    lpTarget = m_dirs.AllTargets(&dwc);
    for (i = 0 ; i < dwc ; ++i, ++lpTarget)
    {
        j = lpTarget->szPath[0] | 32;
        if (j >= 'a' && j <= 'z')
        {
            nDrives[j - 'a'] += (int)lpTarget->dwDiskSpaceNeeded -
                (int)lpTarget->dwDiskSpaceUsed;
        }
    }

    // Get the drive letter for the default directory
    DWORD dwDefault;
    dwDefault = m_szDefaultDir[0] | 32;
    if (dwDefault >= 'a' && dwDefault <= 'z')
        dwDefault -= 'a';
    else
        dwDefault = 0xffffffff;

    // Now walk through all the drives to see if there is enough space
    for (i = 0 ; i < 26 ; ++i)
    {
        if (nDrives[i] > 0)
        {
            szDrivePath[0] = (char)(i + 'A');
            GetDiskFreeSpaceEx(szDrivePath, &liFree, &liTotal, NULL);
            liNeeded.QuadPart = ((__int64)nDrives[i]) << 15;
            if (liNeeded.QuadPart > liFree.QuadPart)
            {
                char szMessage1[MAX_PATH];
                char szMessage2[MAX_PATH];
                char szMessage3[MAX_PATH];
                char szMessage[3 * MAX_PATH];

                // Get the localized part
                LoadString(GetInstanceHandle(), IDS_NOT_ENOUGH_SPACE_ON, szMessage1, MAX_PATH);
                sprintf(szMessage2, szMessage1, szDrivePath);
                LoadString(GetInstanceHandle(), IDS_SPACE_REQUIRED, szMessage1, MAX_PATH);
                sprintf(szMessage3, szMessage1, AddCommas(liNeeded.QuadPart));
                LoadString(GetInstanceHandle(), IDS_SPACE_AVAILABLE, szMessage, MAX_PATH);
                sprintf(szMessage1, szMessage, AddCommas(liFree.QuadPart));

                // Merge the numbers in
                sprintf(szMessage, "%s\n\n%s\n\n%s", szMessage2, szMessage3, szMessage1);
                SetLastErrorString(szMessage, 1008);

                // Return 1 (try again with default dir) if it's on the default drive.
                // Otherwise, return -1 which means they can't correct it.
                if (i == dwDefault)
                    return 1;
                else
                    return -1;
            }
        }
    }

    return 0;
}

typedef BOOL (*LPGETVERSIONEX)(OSVERSIONINFOEX*);
//
//  Check for a compatible version of Windows.  Currently,
//  this checks for Windows 2000.  This really should read
//  the required Windows version from the settings file.
//
BOOL
CUnpacker::CheckWindows()
{
    OSVERSIONINFOEX ver;
    LPGETVERSIONEX lpGetVersionEx;
    HMODULE hlib;

    // Get the extended version info function
    hlib = LoadLibrary("kernel32.dll");
    lpGetVersionEx = (LPGETVERSIONEX)GetProcAddress(hlib, _TEXT("GetVersionExA"));
    FreeLibrary(hlib);
    if (lpGetVersionEx == NULL)
    {
        SetLastError(IDS_ERRORNOTWINDOWS2000, 2008);
        return FALSE;
    }

    // Get the version information
    ver.dwOSVersionInfoSize = sizeof (OSVERSIONINFOEX);
    if (!(*lpGetVersionEx)(&ver))
    {
        SetLastError(IDS_ERRORNOTWINDOWS2000, 2009);
        return FALSE;
    }

    // Compare the version information to what we require (Windows 2000 SP1)
    if (ver.dwMajorVersion < 5)
    {
        SetLastError(IDS_ERRORNOTWINDOWS2000, 2010);
        return FALSE;
    }

#if 0 // No service pack check
    if (ver.wServicePackMajor < 1)
    {
        SetLastError(IDS_ERRORNOTSP1, 2011);
        return FALSE;
    }
#endif

    return TRUE;
}


//  VersionWORDsToQWORD
//      Converts version values as WORD components into a single QWORD

__int64
VersionWORDsToQWORD(
    WORD w1,
    WORD w2,
    WORD w3,
    WORD w4
    )
{
    return (__int64)w4 +
           (__int64)w3 * 65536 +
           (__int64)w2 * 65536 * 65536 +
           (__int64)w1 * 65536 * 65536 * 65536;
}


//  VersionStringToQWORD
//      Converts a version written out as a string as a QWORD for comparisons

__int64
VersionStringToQWORD(
    LPSTR lpVersion
    )
{
    LPSTR lp[4];
    __int64 i64Out;

    // Split the line into components
    LPSTR lpCopy = _strdup(lpVersion);
    LPSTR lpT = lpCopy;
    if (!ParseLine(lpT, lp, 4, NULL))
        return 0;

    i64Out = VersionWORDsToQWORD((WORD)atoi(lp[0]), (WORD)atoi(lp[1]), (WORD)atoi(lp[2]), (WORD)atoi(lp[3]));
    free(lpCopy);
    return i64Out;
}


//  CheckUpgradeVersion
//      Routine to check the existing version in upgrade scenarious.
//      This feature is designed for QFE's.  QFE's perform an upgrade
//      by replacing only select files (and perhaps registry keys), it
//      important to ensure that a compatible version of the product is
//      already installed.  Set MinVersionAllowed in the settings file
//      to invoke this feature.
BOOL
CUnpacker::CheckUpgradeVersion()
{

    LPSTR lpMinInstallVersion;

    // Get the registry path for the upgrade key for product

    //  Legacy:  Make sure the new full build version is written if not
    //           read the old format and write back out in the new format.
    TCHAR szBuild[MAX_PATH];
    szBuild[0] = 0;
    if (!GetRegistryString(szBuild, MAX_PATH, m_ProductRegKey.hRootKey, m_ProductRegKey.lpSubkey, XSETUP_FULL_BUILD_REGVALUE))
    {
        // Get the old one (just the build number)
        if (GetRegistryString(szBuild, MAX_PATH, m_ProductRegKey.hRootKey, m_ProductRegKey.lpSubkey, XSETUP_BUILD_REGVALUE))
        {
            // Write out the new string
            TCHAR szNewBuild[MAX_PATH];
            CreateVersionString(szNewBuild, 1, 0, (WORD)atoi(szBuild), 1);
            WriteRegistryString(m_ProductRegKey.hRootKey, m_ProductRegKey.lpSubkey, XSETUP_FULL_BUILD_REGVALUE, szNewBuild);
        }
    }

    // Make sure we aren't overwriting a newer build. Note that this flag
    // allows the end user to override the build check and overwrite the version with
    // an older version. Bad stuff could happen...
    if (!(GetFlags() & UNPACKER_FLAGS_FORCEINSTALL))
    {
        __int64 i64Installed = 0;
        __int64 i64TryingToInstall = VersionWORDsToQWORD(VER_PRODUCTVERSION); // Note that this contains 4 parameters

        // Get the current version number installed
        if (GetRegistryString(szBuild, MAX_PATH, m_ProductRegKey.hRootKey, m_ProductRegKey.lpSubkey, XSETUP_FULL_BUILD_REGVALUE))
        {
            i64Installed = VersionStringToQWORD(szBuild);
            if (i64Installed > i64TryingToInstall)
            {
                SetLastError(IDS_ERRORINSTALLOVERNEWER, 2012);
                return FALSE;
            }
        }

        // If the setup package has a setting that specifies a lower bound to install over,
        // check it here

        // Read the MinVersionAllowed setting.
        lpMinInstallVersion = GetSetting(CSTR_MINVERSIONALLOWED);
        if (*lpMinInstallVersion)
        {
            // Less than the min bound?
            __int64 i64MinBound = VersionStringToQWORD(lpMinInstallVersion);
            if (i64Installed < i64MinBound)
            {
                TCHAR szBuildWarning[MAX_PATH];
                TCHAR szCurrent[MAX_PATH];
                TCHAR szCurrentOutput[MAX_PATH];
                TCHAR szErrorOutput[MAX_PATH * 2];

                // Get the first part of the string warning them
                LoadString(::GetInstanceHandle(), IDS_ERRORINCORRECTBUILD, szBuildWarning, sizeof (szBuildWarning));
                sprintf(szErrorOutput, szBuildWarning, lpMinInstallVersion);

                // Build the second part of the string: depends on the version they have installed already
                if (i64Installed == 0)
                {
                    LoadString(::GetInstanceHandle(), IDS_ERRORNOBUILDINSTALLED, szCurrentOutput, sizeof (szCurrentOutput));
                }
                else
                {
                    LoadString(::GetInstanceHandle(), IDS_ERRORCURRENTBUILD, szCurrent, sizeof (szCurrent));
                    sprintf(szCurrentOutput, szCurrent, szBuild);
                }

                // Finally, concatenate the two and display the error message
                strcat(szErrorOutput, szCurrentOutput);
                SetLastErrorString(szErrorOutput, 2014);
                free(lpMinInstallVersion);
                return FALSE;
            }
        }
        free(lpMinInstallVersion);
    }
    return TRUE;
}