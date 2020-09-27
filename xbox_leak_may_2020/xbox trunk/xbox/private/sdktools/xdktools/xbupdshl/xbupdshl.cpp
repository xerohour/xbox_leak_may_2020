#include <precomp.h>

HINSTANCE g_hInstance;

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,  LPSTR lpCmdLine, int nCmdShow)
/*++
  Routine Description:
    Walks through all the steps of setup.
--*/
{
    HRESULT hr;

    g_hInstance = hInstance;

    //Initialize COM
    CoInitialize(NULL);

    // Check the system requirements.
    if(!CheckSystemRequirements()) return -1;
    
    SETUP_PARAMETERS setupParameters;
    setupParameters.dwInstallType = INSTALL_TYPE_CLEAN;
    setupParameters.fRebootRequired = false;
    setupParameters.InstallLocation[0] = TEXT('\0');
    ExtractCabFile(&setupParameters);
    SetDefaultInstallPath(&setupParameters);
    
    // Find the existinf installtion.
    FindExistingInstallation(&setupParameters);

    // Exectue the setup wizard.
    CWizard wizard;
    hr = wizard.Initialize(4);

    // If we couldn't initialize the setup wizard then we are hosed.
    // Rell the user and scram
    if(FAILED(hr))
    {
        TCHAR szErrorBuffer[256];
        FormatErrorCode(szErrorBuffer, hr);
        RMessageBox(GetDesktopWindow(), IDS_ERROR_FORMAT_TEXT, IDS_ERROR_CAPTION, MB_OK|MB_ICONSTOP, szErrorBuffer);
        return -1;
    }

    // Instantiate all the pages and add them to the wizard

    CWelcomePage         welcomePage(&setupParameters);
    CUpgradePage         upgradePage(&setupParameters);
    CInstallLocationPage installLocationPage(&setupParameters);
    CFinishPage          finishPage(&setupParameters);

    wizard.AddPage(&welcomePage);
    wizard.AddPage(&upgradePage);
    wizard.AddPage(&installLocationPage);
    wizard.AddPage(&finishPage);

    // Execute the wizard

    hr = wizard.DoWizard(GetDesktopWindow(), g_hInstance);

    // If the wizard failed to execute, display error
    if(FAILED(hr))
    {
      TCHAR szErrorBuffer[256];
      FormatErrorCode(szErrorBuffer, hr);
      RMessageBox(GetDesktopWindow(), IDS_ERROR_FORMAT_TEXT, IDS_ERROR_CAPTION, MB_OK|MB_ICONSTOP, szErrorBuffer);
      return -1;
    }

    if(setupParameters.pidlInstallFolder)
    {
        IMalloc *pMalloc;
        SHGetMalloc(&pMalloc);
        pMalloc->Free(setupParameters.pidlInstallFolder);
        pMalloc->Release();
        setupParameters.pidlInstallFolder = NULL;
    }
    
    CoUninitialize();

    if(setupParameters.fRebootRequired)
    {
        HandleReboot();
    }
    // We are done, the wizard handled everything.
    return 0;
}

bool CheckSystemRequirements()
/*++
  Routine Description:

    Two requirements:

        1) The shell extension requires Windows 2000 or later.
        2) Installation requires administrator priviledges.

    If the system meets this requirement return true.
    
        Otherwise, pop up a top-level dialog and inform the user
        what is wrong.  Then, return false.
--*/
{
    OSVERSIONINFO versionInfo;
    
    versionInfo.dwOSVersionInfoSize = sizeof(versionInfo);
    
    // If we couldn't get the version inform user and fail
    if(!GetVersionEx(&versionInfo))
    {
        TCHAR szErrorBuffer[256];
        FormatErrorCode(szErrorBuffer, GetLastError());
        RMessageBox(GetDesktopWindow(), IDS_ERROR_FORMAT_TEXT, IDS_ERROR_CAPTION, MB_OK|MB_ICONSTOP, szErrorBuffer);
        return false;
    }

    // Verify Version
    if( (versionInfo.dwPlatformId != VER_PLATFORM_WIN32_NT) || (versionInfo.dwMajorVersion < 5) )
    {
        RMessageBox(GetDesktopWindow(), IDS_ERROR_OLD_OS_VERSION, IDS_ERROR_OLD_OS_VERSION_CAPTION, MB_OK|MB_ICONSTOP);
        return false;
    }

    // TODO: Verify Install Priviledges (for now it will blow up somewhere later if you have insufficient priviledge).
    
    return true;
}

void ExtractCabFile(PSETUP_PARAMETERS pSetupParameters)
{
    HRSRC  hResource = FindResource(g_hInstance, MAKEINTRESOURCE(IDCAB_PRODUCT), TEXT("CABFILES"));
    HGLOBAL hMemory = LoadResource(g_hInstance, hResource);
    PVOID   pData = LockResource(hMemory);
    ULONG   uSize = SizeofResource(g_hInstance, hResource);
    TCHAR   szCabinetName[MAX_PATH];
    DWORD   dwError;

    dwError = GetTempPath(MAX_PATH, pSetupParameters->SourcePath);
    if(!dwError)
    {

    }
    ULONG uLen = _tcslen(pSetupParameters->SourcePath);
    LPTSTR pszParse = pSetupParameters->SourcePath + uLen-1;
    if(*pszParse++!=TEXT('\\')) *pszParse++  = TEXT('\\');
    _tcscpy(pszParse, TEMP_SUBDIR_NAME);
    CreateDirectory(pSetupParameters->SourcePath, NULL);
    wsprintf(szCabinetName, "CABFILE_APPEND_FROMAT", pSetupParameters->SourcePath);

    HANDLE hFile = CreateFile(szCabinetName, GENERIC_WRITE, 0, NULL,CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if(INVALID_HANDLE_VALUE!=hFile)
    {   
        DWORD dwBytesWritten;
        WriteFile(hFile, pData, uSize, &dwBytesWritten, NULL);
        CloseHandle(hFile);
        SetupIterateCabinet(szCabinetName, 0, UnpackFileCB, (PVOID)pSetupParameters->SourcePath);
    }
}

UINT UnpackFileCB( PVOID Context, UINT Notification, UINT_PTR Param1,  UINT_PTR Param2)
{
    if(Notification == SPFILENOTIFY_FILEINCABINET)
    {
        PSETUP_PARAMETERS     pSetupParameters= (LPSTR)Context;
        PFILE_IN_CABINET_INFO pFileCabinetInfo = (PFILE_IN_CABINET_INFO)Param1;
        
        wsprintf(pFileCabinetInfo->FullTargetName, TEXT("%s\\%s"), pszSourceBuffer, pFileCabinetInfo->NameInCabinet);
        return FILEOP_DOIT;
    }
    return NO_ERROR;
}
void PlaceFiles(PSETUP_PARAMETERS pSetupParameters)
{

}


void SetDefaultInstallPath(PSETUP_PARAMETERS pSetupParameters)
{
    SHGetSpecialFolderPath(GetDesktopWindow(), pSetupParameters->InstallLocation, CSIDL_PROGRAM_FILES, FALSE);
    UINT uLen = _tcslen(pSetupParameters->InstallLocation);
    LPTSTR pszParse = pSetupParameters->InstallLocation+uLen;
    *pszParse++ = '\\';
    _tcscpy(pszParse, TEXT("xbnspace"));
    SHGetSpecialFolderLocation(GetDesktopWindow(), CSIDL_PROGRAM_FILES, &pSetupParameters->pidlInstallFolder);
}

void FindExistingInstallation(PSETUP_PARAMETERS pSetupParameters)
{
    //
    //  Look in the registery to find the previous installation
    //
    HKEY hKey;
    if(ERROR_SUCCESS==RegOpenKey(HKEY_CLASSES_ROOT, TEXT("CLSID\\{DB15FEDD-96B8-4DA9-97E0-7E5CCA05CC44}\\InProcServer32"), &hKey))
    {
        //
        //  Get the install path
        //
        DWORD dwInstallPath = sizeof(pSetupParameters->InstallLocation);
        RegQueryValueEx(hKey, NULL, NULL, NULL, (PBYTE)pSetupParameters->InstallLocation, &dwInstallPath);
        RegCloseKey(hKey);

        //
        //  TODO: Check and compare versions:
        //
        /*
        HANDLE hFileVersion;
        DWORD  dwVersionInfoSize;
        GetFileVersionInfoSize(pSetupParameters->InstallLocation, &hFileVersion);
        */
        pSetupParameters->dwInstallType = INSTALL_TYPE_UPGRADE_NEWER;
    } else
    {
        pSetupParameters->dwInstallType = INSTALL_TYPE_CLEAN;
        return;
    }

}

void PerformUpgrade(HWND hWnd, PSETUP_PARAMETERS pSetupParameters)
{
    //
    //  Unregister the existing dll
    //
    HRESULT hr = E_FAIL;
    HMODULE hLibrary = LoadLibrary(pSetupParameters->InstallLocation);
    if(hLibrary)
    {
        HRESULT (__stdcall *pfnDllUnregisterServer)(void) = (HRESULT(__stdcall *)(void))GetProcAddress(hLibrary, "DllUnregisterServer");
        if(pfnDllUnregisterServer)
        {
            hr = pfnDllUnregisterServer();
        }
        FreeLibrary(hLibrary);
    }
    
    if(FAILED(hr))
    {
        TCHAR szErrorBuffer[256];
        FormatErrorCode(szErrorBuffer, hr);
        RMessageBox(hWnd, IDS_UPGRADE_FAILURE, IDS_UPGRADE_FAILURE_CAPTION, MB_OK|MB_ICONSTOP, szErrorBuffer);
    }

    //
    //  Try to delete the existing file
    //
    if(DeleteFile(pSetupParameters->InstallLocation))
    {
        // Strip xbshlext.dll from the install location
        LPTSTR pszParse = _tcsrchr(pSetupParameters->InstallLocation, TEXT('\\'));
        *pszParse = TEXT('\0');

        // The delete worked, the file was not in use, so just follow the normal
        // Perform Install path
        
        PerformInstall(hWnd, pSetupParameters);
        return;
    }

    //
    //  The delete file failed, on the old file.
    //  
    TCHAR szTempFileName[MAX_PATH];
    TCHAR szSourceFileName[MAX_PATH];

    // 1) Reboot required.
    pSetupParameters->fRebootRequired = true;

    // 2) Strip xbshlext.dll from the install location
    LPTSTR pszParse = _tcsrchr(pSetupParameters->InstallLocation, TEXT('\\'));
    *pszParse = TEXT('\0');

    // 3) Install xbshlext.dll to a temporary name and set MoveFileEx to install it on reboot
    GetTempFileName(pSetupParameters->InstallLocation, "XBS", 0, szTempFileName);
    wsprintf(szSourceFileName, TEXT("%s\\xbshlext.dll"), pSetupParameters->SourcePath);
    CopyFile(szSourceFileName, szTempFileName, FALSE);
    *pszParse = TEXT('\\');
    MoveFileEx(szTempFileName, pSetupParameters->InstallLocation, MOVEFILE_DELAY_UNTIL_REBOOT|MOVEFILE_REPLACE_EXISTING);

    // 4) Setup RunOnce to call regsvr32.exe on the xbshlext.dll.
    RegisterOnBoot(hWnd, pSetupParameters->InstallLocation);
}

void PerformInstall(HWND hWnd, PSETUP_PARAMETERS pSetupParameters)
{
    bool fDisplayError = false;
    TCHAR szErrorBuffer[256];

    // 1) make sure that the target directory exists
    EnsureDirectoryExists(pSetupParameters->InstallLocation);
    
    // 2) Copy the file 
    TCHAR szSourceFileName[MAX_PATH];
    TCHAR szTargetFileName[MAX_PATH];
    wsprintf(szSourceFileName, TEXT("%s\\xbshlext.dll"), pSetupParameters->SourcePath);
    wsprintf(szTargetFileName, TEXT("%s\\xbshlext.dll"), pSetupParameters->InstallLocation);
    CopyFile(szSourceFileName, szTargetFileName, FALSE);

    // 2) Register the file.
    HMODULE hLibrary = LoadLibrary(szTargetFileName);
    if(hLibrary)
    {
        HRESULT (__stdcall *pfnDllRegisterServer)(void) = (HRESULT(__stdcall *)(void))GetProcAddress(hLibrary, "DllRegisterServer");
        if(pfnDllRegisterServer)
        {
            HRESULT hr = pfnDllRegisterServer();
            if(FAILED(hr))
            {
                fDisplayError = true;
                FormatErrorCode(szErrorBuffer, hr);
            }
        } else
        {
           fDisplayError = true;
           FormatErrorCode(szErrorBuffer, GetLastError());
        }
        FreeLibrary(hLibrary);
    } else
    {
        fDisplayError = true;
        FormatErrorCode(szErrorBuffer, GetLastError());
    }
    if(fDisplayError) RMessageBox(hWnd, IDS_ERROR_FORMAT_TEXT, IDS_ERROR_CAPTION, MB_OK|MB_ICONSTOP, szErrorBuffer);
}

void RegisterOnBoot(HWND hWnd, LPCTSTR szComDll)
{
    TCHAR  szRegisterCommand[512];
    TCHAR  systemPath[MAX_PATH];
    LPTSTR pszParse = szRegisterCommand;
    UINT   uLen;
    HKEY   hRunOnceKey;
    if(GetSystemDirectory(systemPath, MAX_PATH))
    {
        wsprintf(szRegisterCommand, TEXT("\"%s\\regsvr32.exe\" /s \"%s\""), systemPath, szComDll);
        if(ERROR_SUCCESS==RegOpenKey(HKEY_LOCAL_MACHINE, RUN_ONCE_KEY, &hRunOnceKey))
        {
            // Get the length of the command in bytes
            uLen = _tcslen(szRegisterCommand)+1;
            uLen *= sizeof(TCHAR);
            //Get the name of the DLL to use as the unique ID.
            pszParse = _tcsrchr(szComDll, TEXT('\\'));
            pszParse++;
            RegSetValueEx(hRunOnceKey, pszParse, 0, REG_SZ, (LPBYTE)szRegisterCommand, uLen);
            RegCloseKey(hRunOnceKey);
        }
    }
}

int RMessageBox(HWND hWnd, UINT uResourceIdText, UINT uResourceIdCaption, UINT uType,...)
{
    va_list vl;
    TCHAR szFormat[256];
    TCHAR szText[512];
    TCHAR szCaption[512];
    
    // Format the text.
    LoadString(g_hInstance, uResourceIdText, szFormat, sizeof(szFormat)/sizeof(TCHAR));
    va_start(vl, uType); 
    wvsprintfA(szText, szFormat, vl);
    va_end(vl);
    
    // Load the caption
    LoadString(g_hInstance, uResourceIdCaption, szCaption, sizeof(szCaption)/sizeof(TCHAR));
    
    // Call message box
    return MessageBox(hWnd, szText, szCaption, uType);
}

int RSetWindowText(HWND hWnd, UINT uResourceId,...)
{
    va_list vl;
    TCHAR szFormat[256];
    TCHAR szText[512];
    
    // Format the text.
    LoadString(g_hInstance, uResourceId, szFormat, sizeof(szFormat)/sizeof(TCHAR));
    va_start(vl, uResourceId); 
    wvsprintf(szText, szFormat, vl);
    va_end(vl);

    return SetWindowText(hWnd, szText);
}

int FormatErrorCode(LPTSTR pszBuffer, HRESULT hr)
{
    return FormatMessage(
            FORMAT_MESSAGE_IGNORE_INSERTS | FORMAT_MESSAGE_FROM_SYSTEM,
            NULL, hr, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), pszBuffer,
            MAX_PATH, NULL);
}

void EnsureDirectoryExists(LPTSTR pszDirectory)
{
    DWORD dwAttributes = GetFileAttributes(pszDirectory);
    if(INVALID_HANDLE_VALUE == (HANDLE)dwAttributes)
    {
        LPTSTR pszParse = _tcsrchr(pszDirectory, TEXT('\\'));
        *pszParse = TEXT('\0');
        EnsureDirectoryExists(pszDirectory);
        *pszParse = TEXT('\\');
        CreateDirectory(pszDirectory, NULL);
    }
}

void HandleReboot()
{
    struct {
        DWORD PrivilegeCount; 
        LUID_AND_ATTRIBUTES RebootPrivilege;
    } tokenPrivilege;

    if(IDYES==RMessageBox(GetDesktopWindow(), IDS_PROMPT_REBOOT, IDS_PROMPT_REBOOT_CAPTION, MB_YESNO|MB_ICONQUESTION))
    {
        HANDLE hProcessToken;
        if(OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY|TOKEN_ADJUST_PRIVILEGES, &hProcessToken))
        {
            tokenPrivilege.PrivilegeCount = 1;
            tokenPrivilege.RebootPrivilege.Attributes = SE_PRIVILEGE_ENABLED;
            if(LookupPrivilegeValue( NULL,SE_SHUTDOWN_NAME, &tokenPrivilege.RebootPrivilege.Luid))
            {
                if(AdjustTokenPrivileges(hProcessToken, FALSE, (PTOKEN_PRIVILEGES)&tokenPrivilege, sizeof(tokenPrivilege), NULL, NULL))
                {
                    ExitWindowsEx(EWX_REBOOT, 0);
                    return;
                }
            }
        }
        RMessageBox(GetDesktopWindow(), IDS_INSUFFICIENT_REBOOT_PRIVILEGE, IDS_PROMPT_REBOOT_CAPTION, MB_OK|MB_ICONERROR);
    }
}



