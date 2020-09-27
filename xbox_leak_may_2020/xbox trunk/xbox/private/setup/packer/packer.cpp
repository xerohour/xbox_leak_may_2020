//  PACKER.CPP
//
//  Created 11-Mar-2001 [JonT]

// TODO: add complete .INI file validation: check for all required keys, sections
// TODO: Nuke [PackerSettings] section before copying to .exe

#include "packer.h"

int __cdecl
main(
    int argc,
    char** argv
    )
{
    DWORD dwTickStart = GetTickCount();
    int nRet;

    // Create our application object
    CPacker App;
    
    // Make sure the command line is OK
    if (!App.ParseCommandLine(argc, argv))
    {
        App.PrintUsage();
        return -1;
    }

    // Do all the work
    nRet = App.Process();

    // Print out elapsed time
    if (nRet == 0)
    {
        DWORD dwTickTotal = GetTickCount() - dwTickStart;
        DWORD dwHours = dwTickTotal / (1000 * 60 * 60);
        DWORD dwMinutes = (dwTickTotal % (1000 * 60 * 60)) / (1000 * 60);
        DWORD dwSeconds = ((dwTickTotal % (1000 * 60 * 60)) % (1000 * 60)) / 1000;
    
        printf("Time elapsed: %02d:%02d:%02d\n", dwHours, dwMinutes, dwSeconds);
    }

    return nRet;
}


//---------------------------------------------------------------------
//  CPacker -- our application class

    TCHAR CPacker::m_szIniName[MAX_PATH];

BOOL
CPacker::ParseCommandLine(
    int argc,
    char** argv
    )
{
    int nArg;

    *m_szIniName = 0;

    // Get the executable name
    --argc;
    m_lpLaunchName = argv[0];
    nArg = 1;

    // Process arguments
    for (; argc ; --argc, ++nArg)
    {
        // Is it a switch?
        if (*argv[nArg] == '-' || *argv[nArg] == '/')
        {
            switch (argv[nArg][1] | 32)
            {
            // Test (don't emit anything)
            case 't':
                m_fTestPack = TRUE;
                break;

            // Nologo
            case 'n':
                m_fNoLogo = TRUE;
                break;

            // Usage
            case '?':
            default:
                return FALSE;
            }
        }

        // Must be the ini file name
        else
        {
            // Fully-qualify the INI name
            if (_fullpath(m_szIniName, argv[nArg], _MAX_PATH) == NULL)
                return FALSE;
        }
    }

    // Make sure we got an .INI file
    if (*m_szIniName == 0)
        return FALSE;

    // Display banner if not nologo
    if (!m_fNoLogo)
    {
        printf("Microsoft (R) Self-Extracting Image Packer [%s]\n"
            "Copyright (C) Microsoft Corporation. All rights reserved.\n\n", __DATE__);
    }

    // If we're in test mode, tell them!
    if (m_fTestPack)
        printf("\nTEST MODE: no files will be generated.\n\n");

    return TRUE;
}


//  CPacker::PrintUsage
//      Displays usage message

void
CPacker::PrintUsage()
{
    printf("Usage: xpacker [flags] [filename]\n\n");
    printf("Packer builds a self-extracting executable as part of a simple\n"
           "setup engine.\n\n"
           "\t-? This message\n"
           "\t-t test build (don't emit any files but report errors)\n"
           "\n"
           "It requires a .INI file that describes where to locate the\n"
           "file packing list and the extractor .EXE to bundle in.\n"
           "It builds a .CAB file and a file manifest and attaches these\n"
           "to the extractor executable, which is a setup engine.\n\n");
}


//  CPacker::Process
//      Top-level code for packing the setup files

int
CPacker::Process()
{
    if (!OpenFiles())
        return -1;


    if (!ProcessFiles())
        return -1;

    if (!CreateSettingsCab())
        return -1;


    if (!WriteSelfExtractingExe())
        return -1;

    return 0;
}


//  CPacker::OpenFiles
//      Opens files and prepares for bulk of work in Process()

BOOL
CPacker::OpenFiles()
{
    LPSECTIONENUM lpSection;
    LPTSTR lpPackingList;
    LPSTR lpBuildFlavor;
    DWORD i;

    // Set up the settings file and make a copy so we can modify
    if (!m_settings.SetFile(m_szIniName, TRUE))
    {
        ::ReportError(m_szIniName, 0, "couldn't open settings file");
        return FALSE;
    }

    // Locate the packing list
    lpPackingList = m_settings.GetSetting(CSTR_PACKINGLIST);
    if (!m_plist.SetFile(&m_settings, lpPackingList, m_fTestPack))
    {
        ::ReportSettingsError(CSTR_PACKINGLIST, "couldn't open packing list \"%s\", error=0x%x", lpPackingList, m_plist.GetError());
        return FALSE;
    }
    free(lpPackingList);
    lpPackingList = NULL;

    // Open the manifest
    m_lpManifestName = m_settings.GetSetting(CSTR_MANIFEST);
    if (!m_manifest.SetFile(m_lpManifestName))
    {
        ::ReportSettingsError(CSTR_MANIFEST, "Couldn't open manifest file \"%s\", error=0x%x", m_lpManifestName, m_manifest.GetError());
        return FALSE;
    }

    // Get all the directories so we can sum disk usage for them
    lpSection = m_settings.EnumerateSection(CSTR_TARGETDIRECTORIES);
    m_dirs.Init(lpSection->dwcKeys);
    for (i = 0 ; i < lpSection->dwcKeys ; i++)
        m_dirs.Add(lpSection->keys[i].lpKey);
    m_settings.EnumerateFree(lpSection);

    // Create the cabfile for the files
    m_lpCabPath = m_settings.GetSetting(CSTR_CABPATH);
    strcpy(m_szFileCab, m_lpCabPath);
    if (*(m_lpCabPath + strlen(m_lpCabPath) - 1) != '\\')
        strcat(m_szFileCab, "\\");
    strcpy(m_szSettingsCab, m_szFileCab);
    strcat(m_szFileCab, TEXT("files.cab"));
    strcat(m_szSettingsCab, TEXT("settings.cab"));
    if (!m_cabFiles.SetFile(m_szFileCab, m_fTestPack))
    {
        ::ReportSettingsError(CSTR_CABPATH, "Couldn't create file cab \"%s\", error=0x%x", m_szFileCab, m_cabFiles.GetError()->erfOper);
        return FALSE;
    }

    // Get the build flavor
    lpBuildFlavor = m_settings.GetSetting(CSTR_BUILDFLAVOR);
    if (lpBuildFlavor == NULL || *lpBuildFlavor == 0)
    {
        ::ReportSettingsError(CSTR_BUILDFLAVOR, "no build flavor set");
        return FALSE;
    }
    m_plist.SetBuildFlavor(lpBuildFlavor);
    free(lpBuildFlavor);

    // Set the compression type
    LPSTR lpCompressionType = m_settings.GetSetting(CSTR_COMPRESSIONTYPE);
    LPSTR lpCompressionLevel = m_settings.GetSetting(CSTR_COMPRESSIONLEVEL);
    if (*lpCompressionType == 0)
    {
        free(lpCompressionType);
        lpCompressionType = _strdup("NONE");
    }
    if (!m_cabFiles.SetCompression(lpCompressionType, lpCompressionLevel) ||
        !m_cabSettings.SetCompression(lpCompressionType, lpCompressionLevel))
    {
        return FALSE;
    }
    if (_stricmp(lpCompressionType, "LZX") == 0)
        printf("Compression type set to LZX level %s (of 6)\n", lpCompressionLevel);
    else
        printf("Compression type set to %s\n", lpCompressionType);
    free(lpCompressionType);
    free(lpCompressionLevel);

    return TRUE;
}


//  CPacker::ProcessFiles
//      Walks through all the lines in the packing list and adds files
//      to the cabfile and lines to the manifest.

BOOL
CPacker::ProcessFiles()
{
    return m_plist.ProcessLines(&m_manifest, &m_cabFiles, &m_dirs);
}


//  CPacker::CreateSettingsCab
//      Makes a cabfile that contains the .INI file and the manifest

BOOL
CPacker::CreateSettingsCab()
{
    DWORD i;
    LPDIRINFO lpdi;
    TCHAR szDest[MAX_PATH];
    LPSTR lpCustomDll;
    LPSTR lpLicenseFile;
    LPSTR lpUninstaller;
    

    // Whack the sensitive packer sections from the ini file we're writing out
    m_settings.NukeSection(CSTR_PACKERPATHS);

    // Write out the directory sizes to the INI file
    m_dwTotalSize = 0;
    for (i = 0, lpdi = m_dirs.GetAll() ; i < m_dirs.GetCount() ; ++i, ++lpdi)
    {
        sprintf(szDest, "%d", lpdi->dwSize);
        m_settings.WriteString(CSTR_DIRSIZES, lpdi->szID, szDest);
        m_dwTotalSize += lpdi->dwSize;
    }

    // Write out the total size (in MB) to the ini file.
    // Don't write out 0 though.
    DWORD dwTotalSizeInBigUnits = m_dwTotalSize>>20;
    if(dwTotalSizeInBigUnits)
    {
        sprintf(szDest, "%d MB", dwTotalSizeInBigUnits);
    } else
    {
        dwTotalSizeInBigUnits = m_dwTotalSize>>10;
        if(!dwTotalSizeInBigUnits) dwTotalSizeInBigUnits = 1;
        sprintf(szDest, "%d KB", dwTotalSizeInBigUnits);
    }
    m_settings.WriteString(CSTR_DIRSIZES, CSTR_TOTALINSTALLSIZE, szDest);

    // Get the filenames for the custom dll, the license file and uninstaller exe
    lpCustomDll   = m_settings.GetSetting(CSTR_CUSTOMDLL);
    lpLicenseFile = m_settings.GetSetting(CSTR_LICENSEFILE);
    lpUninstaller = m_settings.GetSetting(CSTR_UNINSTALLER);

    // Create the settings cab and put stuff in it
    if (!m_cabSettings.SetFile(m_szSettingsCab, m_fTestPack))
    {
        ::ReportSettingsError(CSTR_CABPATH, "Couldn't create file cab \"%s\", error=0x%x", m_szSettingsCab, m_cabSettings.GetError()->erfOper);
        return FALSE;
    }
    if (!m_cabSettings.AddFile(m_settings.GetFile(), SCAB_SETTINGS) ||
        !m_cabSettings.AddFile(m_lpManifestName, SCAB_MANIFEST))
    {
        ::ReportSettingsError(CSTR_CABPATH, "Couldn't add files to settings cab, error=0x%x", m_cabSettings.GetError()->erfOper);
        return FALSE;
    }

    // Add the custom dll
    if (*lpCustomDll && !m_cabSettings.AddFile(lpCustomDll, SCAB_CUSTOMDLL))
    {
        ::ReportSettingsError(CSTR_CUSTOMDLL, "Couldn't find the custom dll file \"%s\" and add it to cab, error=0x%x", lpCustomDll, m_cabSettings.GetError()->erfOper);
        return FALSE;
    }

    // Handle multiple license files
    BOOL fLicFileFound = TRUE;
    TCHAR lpLicFilePattern[MAX_PATH+1];
    TCHAR lpPackedLicFilePattern[MAX_PATH+1];
    unsigned int uiCount = 1;
    
    lpLicFilePattern[MAX_PATH] = '\0';
    lpPackedLicFilePattern[MAX_PATH] = '\0';

    strcpy( lpPackedLicFilePattern, SCAB_LICENSE );
    do
    {
        // Add the license file
        if (!m_cabSettings.AddFile(lpLicenseFile, lpPackedLicFilePattern))
        {
            ::ReportSettingsError(CSTR_LICENSEFILE, "Couldn't find license file \"%s\" and add it to cab, error=0x%x", lpLicenseFile, m_cabSettings.GetError()->erfOper);
            return FALSE;
        }

        // Update our file name patterns for the license files
        sprintf( lpLicFilePattern, "%s%u", m_settings.m_strTable[CSTR_LICENSEFILE], uiCount );
        sprintf( lpPackedLicFilePattern, "%s%u", SCAB_LICENSE, uiCount );

        // Get the next license file name (if any) from the INI
        free( lpLicenseFile );
        lpLicenseFile = m_settings.GetSetting( lpLicFilePattern );
        
        if( ( NULL == lpLicenseFile ) || ( '\0' == *lpLicenseFile ) )
            fLicFileFound = FALSE;

        if( uiCount >= CSTR_LICENSE_MAX_FILES )
            fLicFileFound = FALSE;

        ++uiCount;
    } while( fLicFileFound );

    // Add the uninstaller
    if (!m_cabSettings.AddFile(lpUninstaller, SCAB_UNINSTALLER))
    {
        ::ReportSettingsError(CSTR_UNINSTALLER, "Couldn't find uninstall executable \"%s\" and add it to cab, error=0x%x", lpUninstaller, m_cabSettings.GetError()->erfOper);
        return FALSE;
    }
    m_cabSettings.Flush();

    return TRUE;
}


//  CPacker::WriteSelfExtractingExe
//      Modifies the template EXE file (the unpacker) and adds the both cabfiles as
//      resources.

BOOL
CPacker::WriteSelfExtractingExe()
{
    LPTSTR lpTemplate;
    LPTSTR lpExe;
    CExeResource res;

    // Build the extractor by inserting the cab and manifest into the extractor
    lpTemplate = m_settings.GetSetting(CSTR_UNPACKERTEMPLATE);
    lpExe = m_settings.GetSetting(CSTR_UNPACKERNEWEXE);

    // Prepare the EXE then insert the resources
    if (!res.SetFile(lpTemplate, lpExe))
    {
        ::ReportSettingsError(CSTR_UNPACKERTEMPLATE, "Couldn't update %s as template and make %s target .EXE file and add it to cab, error=0x%x", lpTemplate, lpExe, res.GetError());
        return FALSE;
    }

    // If this is just a test, all we do is delete the file and bail, we've validated sufficiently
    if (m_fTestPack)
    {
        res.Close();
        DeleteFile(lpExe);
        return TRUE;
    }

    // Get the total file sizes and tell the resource stuff about it
    HANDLE fh;
    DWORD dwSizeTotal = 0;
    DWORD dwFileCabSize = 0;
    if ((fh = CreateFile(m_szFileCab, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL,
            OPEN_EXISTING, 0, NULL)) != INVALID_HANDLE_VALUE)
    {
        dwFileCabSize = GetFileSize(fh, NULL);
        dwSizeTotal += dwFileCabSize;
        CloseHandle(fh);
    }
    if ((fh = CreateFile(m_szSettingsCab, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL,
            OPEN_EXISTING, 0, NULL)) != INVALID_HANDLE_VALUE)
    {
        dwSizeTotal += GetFileSize(fh, NULL);
        CloseHandle(fh);
    }
    res.SetTotalSize(dwSizeTotal);

    // Add the cabs to the file, first the file cab, then the settings cab
    if (!res.AddResourceFromFile(m_szFileCab) ||
        !res.AddResourceFromFile(m_szSettingsCab))
    {
        ::ReportSettingsError(CSTR_UNPACKERNEWEXE, "Couldn't add files to exe: %s, error=0x%x", lpExe, res.GetError());
        res.Close();
        DeleteFile(lpExe);
    }

    // Write the exe out
    else
        res.Close();

    // Clean up status
    fprintf(stderr, "Complete: %d of %d bytes             \n", dwSizeTotal, dwSizeTotal);

    // Get final statistics
    DWORD dwFinalSize = 0;
    if ((fh = CreateFile(lpExe, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL,
            OPEN_EXISTING, 0, NULL)) != INVALID_HANDLE_VALUE)
    {
        dwFinalSize = GetFileSize(fh, NULL);
        CloseHandle(fh);
    }
    printf("\nFile cab size: %s bytes\n", AddCommas(dwFileCabSize));
    printf("Worst-case uncompressed file size: %s bytes\n", AddCommas(m_dwTotalSize));
    printf("Compression ratio: %.2f%%\n", ((float)dwFileCabSize / (float)m_dwTotalSize) * 100.0);
    printf("Unpacking overhead: %s bytes\n", AddCommas(dwFinalSize - dwFileCabSize));
    printf("Final installer size: %s bytes\n", AddCommas(dwFinalSize));

    return TRUE;
}


//  CPacker::AddCommas
//      Adds commas in the disk size numbers

LPCTSTR
CPacker::AddCommas(
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



//---------------------------------------------------------------------
//  Error reporting

//  _ReportError
//      Since processing a packing list is akin to compiling a file,
//      make a compiler-style error message when we find an error

void
_ReportError(
    LPCTSTR lpFile,
    DWORD dwLine,
    LPCTSTR lpszFormat,
    va_list vararg
    )
{
    char szOutput[4096];

    // Get the full error message
    vsprintf(szOutput, lpszFormat, vararg);

    // Display the message
    if (dwLine != 0)
        printf("%s(%d) : error: %s\n", lpFile, dwLine, szOutput);
    else
        printf("%s : error: %s\n", lpFile, szOutput);
}


//  _ReportSettingsError
//      Report an error in settings in a different format since we don't
//      know the line number.

void
_ReportSettingsError(
    DWORD dwSetting,
    LPCTSTR lpszFormat,
    va_list vararg
    )
{
    char szOutput[4096];

    // Get the full error message
    vsprintf(szOutput, lpszFormat, vararg);

    // Display the message
    printf("%s : [%s]%s : error: %s\n", CPacker::m_szIniName,
           CSettingsFile::m_strTable[CSTR_SETTINGS],
           CSettingsFile::m_strTable[dwSetting], szOutput);
}

