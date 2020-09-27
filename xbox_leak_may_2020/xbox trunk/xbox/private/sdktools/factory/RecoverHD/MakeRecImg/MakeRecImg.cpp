// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
// File:      MakeRecImg.cpp
// Contents:  MakeRecImg.exe main entry point.
// Revisions: 20-Sep-2001: Created (jeffsim)
//
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++


// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++ INCLUDE FILES ++++++++++++++++++++++++++++++++++++++++++++++++++++++
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// "stdafx.h"       -- Precompiled header file
#include "stdafx.h"


// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++ DEFINES ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// MAX_IMAGEBLD_CMD_LINE -- The maximum number of chars in the command line
//                          specified to imagebld.  Large # because it will
//                          hold a lot of "/INSERTFILE:..." strings.
#define MAX_IMAGEBLD_CMD_LINE 2000

// MAX_IMAGE_FILE_SIZE   -- The maximum size of a recovery image file.
#define MAX_IMAGE_FILE_SIZE   0x3000000     // 48 MB

// COPY_BUFFER_SIZE      -- Size of the buffer that holds the copied data
#define COPY_BUFFER_SIZE 32768


// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++ GLOBAL VARIABLES +++++++++++++++++++++++++++++++++++++++++++++++++++
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// g_nCurImageFile       -- Current image file # that we're writing to.
int g_nCurImageFile;

// g_dwCurImageOffset    -- Byte offset into the current image file
DWORD g_dwCurImageOffset;

// g_hfileCurImage       -- Handle to the current image file
HANDLE g_hfileCurImage;

// g_dwSourcePathLen     -- String length of the directory the app was run
//                          in.  Used to generate relative path names
DWORD g_dwSourcePathLen;

// g_xsum                -- Checksum of data in recovery image files.
CCheckSum g_xsum;

// g_cFiles				 -- Number of files in the image
int g_cFiles;

// g_uliSize             -- Total number of bytes in all files in image
ULARGE_INTEGER g_uliSize;

// g_szVersion           -- Version to embed
char g_szVersion[20];


// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++ STRUCTURES +++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// sImageHeader          -- Contains information about the image files.
//                          Written to the start of the first image file.
typedef struct
{
    USHORT usVerMajor, usVerMinor, usVerBuild, usVerQfe;
    DWORD cImageFiles;
    DWORD dwChecksum;
	ULARGE_INTEGER uliSize;
} sImageHeader;

// sFileHeader           -- Contains information about a particular file in
//                          an image file.  'szRelPathName' is the relative
//                          path and file name.
typedef struct
{
    char szRelPathName[MAX_PATH];
    DWORD dwSize;
} sFileHeader;
  
  
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++ FUNCTIONS ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  DumpUsage
// Purpose:   Dumps the application usage information to the console
// Arguments: None
// Return:    Always returns 'FALSE'
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
BOOL DumpUsage()
{
    printf("Usage:  MakeRecImg <SourcePath> <Version>\n\n"
           "    <SourcePath>    - Directory containing recovery files\n"
           "    <Version>       - Embedded Dash version (ie 1.0.3944.1)\n"
           "  Note:   'RecoverHD.exe' MUST exist in the current directory\n"
           "  Output: 'RecoverHD.xbe' in the current directory\n\n");

    return FALSE;
}

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  HandleCommandLine
// Purpose:   Parses the command line, grabs the appropriate strings from it
//            and verifies that they are valid.
// Arguments: argc          -- The number of arguments on the command line
//            argv          -- The list of arguments on the command line
// Return:    'TRUE' if successful, 'FALSE' otherwise
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
BOOL HandleCommandLine(int argc, char *argv[], char szSourcePath[MAX_PATH])
{
    DWORD dwAttr;
    
    // First, verify that the correct number of arguments was specified
    if (argc != 3)
        return DumpUsage();

    // Ensure that the specified path is an absolute (non-local) path
    MakePathAbsolute(argv[1], szSourcePath);

    // Verify that the specified path exists and is indeed a directory
    dwAttr = GetFileAttributes(szSourcePath);
    if (dwAttr == 0xFFFFFFFF)
    {
        printf("Error - path '%s' not found.\n", szSourcePath);
        return FALSE;
    }
    
    if (!(dwAttr & FILE_ATTRIBUTE_DIRECTORY))
    {
        printf("Error - '%s' is not a directory.\n", szSourcePath);
        return FALSE;
    }

    // Store the version
    strcpy(g_szVersion, argv[2]);

    // return success
    return TRUE;
}

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  HandleEndOfImage
// Purpose:   Closes out the current image file and creates a new one.
// Arguments: None
// Return:    'true' if successful, 'false' otherwise
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
BOOL HandleEndOfImage()
{
    char szImageFile[MAX_PATH];

    // Close the existing image file
    if (g_hfileCurImage)
        CloseHandle(g_hfileCurImage);

    g_nCurImageFile++;

    // Generate the name of the next image file
    sprintf(szImageFile, "Recovery%d.img", g_nCurImageFile);
    
    // Open the next image file
    g_hfileCurImage = CreateFile(szImageFile, GENERIC_WRITE, 0,
                                 NULL, CREATE_ALWAYS,
                                 FILE_ATTRIBUTE_NORMAL, NULL);
    if (g_hfileCurImage == INVALID_HANDLE_VALUE)
    {
        printf ("Error - Failed to create image file '%s'.\n",
                szImageFile);
        return FALSE;
    }
    
    g_dwCurImageOffset = 0;

    return TRUE;
}

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  WriteBuffer
// Purpose:   Writes a buffer of data to the specified image file.  If there
//            is not enough room in the image file, then this function makes
//            sure that transition happens correctly
// Arguments: 
// Return:    'TRUE' if successful, 'FALSE' otherwise
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
BOOL WriteBuffer(HANDLE hfileDest, BYTE *pbySource, DWORD dwBytesToWrite)
{
    while (dwBytesToWrite)
    {
        DWORD dw, dwWrite = min(dwBytesToWrite, MAX_IMAGE_FILE_SIZE -
                                                g_dwCurImageOffset);
        if (dwWrite)
        {
            if (!WriteFile(hfileDest, pbySource, dwWrite, &dw, NULL) ||
                dw != dwWrite)
            {
                printf("Error - Failed to write to recovery file.\n");
                return FALSE;
            }

            dwBytesToWrite     -= dwWrite;
            g_dwCurImageOffset += dwWrite;
        }

        if (dwBytesToWrite > 0)
        {
            assert (g_dwCurImageOffset == MAX_IMAGE_FILE_SIZE);
            
            // If there are more bytes to write, then it's because the
            // buffer spanned the end of the image file.  Open a new one.
            if (!HandleEndOfImage())
                return FALSE;
            
            pbySource += dwWrite;
        }
    }
    
    return TRUE;
}

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  AddFileToRecoveryImage
// Purpose:   Adds the specified file to the recovery image(s).  It may be
//            necessary for the file to span multiple image files.
// Arguments: szFilePath    -- Path/Name of file to add
// Return:    'TRUE' if successful, 'FALSE' otherwise
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
BOOL AddFileToRecoveryImage(char *szFilePath)
{
    DWORD dw, dwSourceSize, dwSourceHigh;
    BOOL  fRetVal = FALSE;
    sFileHeader filehdr;
    
    // Open the specified file
    HANDLE hfileSource = CreateFile(szFilePath, GENERIC_READ, 0, NULL,
                              OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hfileSource == INVALID_HANDLE_VALUE)
    {
        printf("Error - failed to open source file '%s'.\n", szFilePath);
        return FALSE;
    }

    // Copy the source file to the set of recovery image files.  We need
    // to be sure that the recovery image files don't exceed their maximum
    // size, so we may need to span the source file across multiple image
    // files.
    dwSourceSize = GetFileSize(hfileSource, &dwSourceHigh);

    // We don't support source files > 2gb in size
    if (dwSourceHigh)
    {
        printf("Error - source file '%s' is too big.  Tool requires"
               " updating.\n", szFilePath);
        goto done;
    }

    g_uliSize.QuadPart += (__int64)dwSourceSize;
    
    // Add the file header to the image
    filehdr.dwSize = dwSourceSize;
    strcpy(filehdr.szRelPathName, szFilePath + g_dwSourcePathLen+1);
    if (!WriteBuffer(g_hfileCurImage, (BYTE*)&filehdr, sizeof filehdr))
        goto done;

    while (dwSourceSize != 0)
    {
        BYTE rgbyCopyBuffer[COPY_BUFFER_SIZE];
        
        // There are more bytes to copy.  Copy as many as possible to the
        // current image file.
        
        // Determine how many to copy; maximum is limited by copy buffer
        DWORD dwBytesToCopy = min(COPY_BUFFER_SIZE, dwSourceSize);

        // Read the bytes
        if (!ReadFile(hfileSource, rgbyCopyBuffer, dwBytesToCopy, &dw,
                      NULL) || dw != dwBytesToCopy)
        {
            printf("Error - Failed to read file '%s'.\n", szFilePath);
            goto done;

        }

        // Write out the bytes
        if (!WriteBuffer(g_hfileCurImage, rgbyCopyBuffer, dwBytesToCopy))
            goto done;

        // Checksum the bytes
        g_xsum.SumBytes(rgbyCopyBuffer, dwBytesToCopy);

        dwSourceSize -= dwBytesToCopy;
    }

	g_cFiles++;

    // If here, we were successful!
    fRetVal = TRUE;

done:

    // We're done with the source file now.
    CloseHandle(hfileSource);

    return fRetVal;
}

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  StrToUpper
// Purpose:   Convert an arbitrary-cased string to upper case.
// Arguments: szSource      -- String to convert.
//            szDest        -- Buffer to hold upper case version of szSource
// Return:    None
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void StrToUpper(char *szSource, char *szDest)
{
    while (*szSource)
        *szDest++ = (char)toupper(*szSource++);
    *szDest = *szSource;
}

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  RecurseCreateRecoveryImages
// Purpose:   Given a directory, munges the files into a collection of
//            image files which are subsequently embedded in an exe.
// Arguments: szSourceDir   -- Directory which contains the files to munge.
// Return:    'TRUE' if successful, 'FALSE' otherwise
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
BOOL RecurseCreateRecoveryImages(char *szSourceDir)
{
    WIN32_FIND_DATA wfd;
    char szSearchString[MAX_PATH];

    // Find all files in the current directory and add them to the image
    // file.  If the image file reaches beyond the size limit, then cap it
    // and start a new image file.
    sprintf(szSearchString, "%s\\*", szSourceDir);
    HANDLE hfile = FindFirstFile(szSearchString, &wfd);
    if (hfile == INVALID_HANDLE_VALUE)
        return FALSE;

    do
    {
        char szFullPath[MAX_PATH], szFullPathUpper[MAX_PATH];

        // Ignore '.' and '..'
        if (!strcmp(wfd.cFileName, ".") || !strcmp(wfd.cFileName, ".."))
            continue;
        
        // Generate the full pathname to the current file.
        sprintf(szFullPath, "%s\\%s", szSourceDir, wfd.cFileName);

        // Force uppercase version of the path.
        StrToUpper(szFullPath, szFullPathUpper);

        // For retail recovery, we only add files that go into 'XDASH'.
        // The 'XDASH\' portion of the path is replaced with 'Y:\' by
        // RecoverHD.xbe;  The reason we leave things like they are is
        // so that we can support arbitrary folders later on if desired.
        if (!strstr(szFullPathUpper, "\\XDASH"))
            continue;

        if (wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
        {
            // Recurse into the directory; we don't explicitly add
            // directories to the image (they are implicit in the files
            // that are added).
            if (!RecurseCreateRecoveryImages(szFullPath))
                return FALSE;
        }
        else
        {
            // Add the file to the recovery image(s)
            if (!AddFileToRecoveryImage(szFullPath))
                return FALSE;
        }

    } while (FindNextFile(hfile, &wfd));
    
    // We've enumerated all files in 'szSourceDir'
    FindClose(hfile);
    
    return TRUE;
}

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  CreateRecoveryImages
// Purpose:   Sets up state necessary for RecurseCreateRecoveryImages, and
//            handles injecting data at the end.
// Arguments: szSourceDir   -- Directory which contains the files to munge.
// Return:    'TRUE' if successful, 'FALSE' otherwise
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
BOOL CreateRecoveryImages(char *szSourceDir)
{
    sImageHeader imghdr;
    DWORD        dwWritten;
    int          nMajor, nMinor, nBuild, nQFE;

    g_nCurImageFile    = 0;
    g_dwCurImageOffset = 0;
	g_cFiles		   = 0;
	g_uliSize.QuadPart = 0;
    g_hfileCurImage    = NULL;
    
    // Create the first image file (as Recovery1.img - recovery0.img contains
    // header and is handled afterwards).
    if (!HandleEndOfImage())
        return FALSE;

    if (!RecurseCreateRecoveryImages(szSourceDir))
        return FALSE;

    // Close the final image file
    if (g_hfileCurImage)
        CloseHandle(g_hfileCurImage);

    // We now want to embed the number of image files and checksum data into
    // the first recovery image file.  Open it and write the data in.
    HANDLE hfileImage0 = CreateFile("Recovery0.img", GENERIC_WRITE, 0, NULL,
                                    CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL,
                                    NULL);
    if (hfileImage0 == INVALID_HANDLE_VALUE)
    {
        printf("Error - Failed to open Recover0.img to write header.\n");
        return FALSE;
    }

    // Write the image header
    imghdr.cImageFiles = g_cFiles;
	imghdr.uliSize.QuadPart = g_uliSize.QuadPart;
    imghdr.dwChecksum  = g_xsum.DwFinalSum();
    
    // Add version to image header
    sscanf(g_szVersion, "%d.%d.%d.%d", &nMajor, &nMinor, &nBuild, &nQFE);
    *(WORD*)(&imghdr.usVerMajor) = (WORD)nMajor;
    *(WORD*)(&imghdr.usVerMinor) = (WORD)nMinor;
    *(WORD*)(&imghdr.usVerBuild) = (WORD)nBuild;
    *(WORD*)(&imghdr.usVerQfe)   = (WORD)nQFE;

    if (!WriteFile(hfileImage0, &imghdr, sizeof imghdr, &dwWritten, 0) ||
        dwWritten != sizeof imghdr)
    {
        printf("Error - Failed to inject header into Recovery0.img.\n");
        CloseHandle(hfileImage0);
        return FALSE;
    }

    CloseHandle(hfileImage0);

    return TRUE;
}

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  GenerateImgBldCmdLine
// Purpose:   Generates the necessary imagebld command line to embed the
//            list of image files.
// Arguments: szOrigDir         -- Directory that the exe was run from.
//            szImageBldCmdLine -- Destination buffer to hold the string.
// Return:    'TRUE' if successful, 'FALSE' otherwise
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
BOOL GenerateImgBldCmdLine(char *szOrigDir, char *szImageBldCmdLine)
{
    sprintf(szImageBldCmdLine, "imagebld.exe /NOSETUPHD /TESTMEDIATYPES:0x00000004 /TESTREGION:0xFFFFFFFF /INITFLAGS:0x00000000");
    for (int i = 0; i <= g_nCurImageFile; i++)
    {
        char szTemp[200];
        sprintf(szTemp, " /INSERTFILE:Recovery%d.img,.RCVR%d,RN", i, i);
        strcat(szImageBldCmdLine, szTemp);
    }

    strcat(szImageBldCmdLine, " /OUT:RecoverHD.xbe ");
    strcat(szImageBldCmdLine, szOrigDir);
    strcat(szImageBldCmdLine, "\\RecoverHD.exe");
    return TRUE;
}

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  SpawnImageBld
// Purpose:   Spawns imagebld with the specified command line.  If there are
//            any problems then output the error here.
// Arguments: szImageBldCmdLine -- The command line to send to ImageBld.
// Return:    'TRUE' if successful, 'FALSE' otherwise
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
BOOL SpawnImageBld(char *szImageBldCmdLine)
{
    if (_spawnlp( _P_WAIT, "imagebld.exe", szImageBldCmdLine, NULL) != 0)
	{
        printf("Error - Failed to spawn ImageBld with the following command"
               "line: '%s'.\n", szImageBldCmdLine);
        return FALSE;
	}
    return TRUE;
}

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  main
// Purpose:   This is the default entry point into a console application.  
//            When the application is started, Windows calls into this
//            function - when this function exits, the application ends.
// Arguments: argc          -- The number of arguments on the command line
//            argv          -- The list of arguments on the command line
// Return:    Result value application returns to the operating system
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
int __cdecl main(int argc, char *argv[])
{
    char szImageBldCmdLine[MAX_IMAGEBLD_CMD_LINE];
    char szOrigDir[MAX_PATH], szOrig[MAX_PATH];
    char szTempDir[MAX_PATH], szTemp[MAX_PATH];
    char szSourcePath[MAX_PATH];
    int  nRetVal = -1;

    // Parse the command line and grab the appropriate strings.
    if (!HandleCommandLine(argc, argv, szSourcePath))
        return -1;

    g_dwSourcePathLen = strlen(szSourcePath);

    // At this point, szSourcePath contains the absolute path of the source
    // directory (necessary since we may change drives for the temp dir)

    // Store the original directory so that we can restore it when done.
    GetCurrentDirectory(MAX_PATH, szOrigDir);

    // Create a temporary directory and change to it -- this allows us to
    // generate a large collection of files without messing up the user's
    // directory.
    if (!GetTempDirName(szTempDir))
        return -1;

    if (!SetCurrentDirectory(szTempDir))
    {
        printf("Failed to change directory to temp dir '%s'.\n", szTempDir);
        return -1;
    }

    printf("\nWorking...\n");
    
    // Munge all of the files in the specified source directory into the
    // set of recovery images.
    if (!CreateRecoveryImages(szSourcePath))
        goto done;

    // Generate the command line string for imagebld that embeds the
    // appropriate image files into the RecoverHD executable.
    if (!GenerateImgBldCmdLine(szOrigDir, szImageBldCmdLine))
        goto done;

    // Spawn imagebld with the generated command line.  This should generate
    // RecoverHD.xbe, with all of the image files embedded into it.
    if (!SpawnImageBld(szImageBldCmdLine))
        goto done;

    // Finally, copy the resultant RecoverHD.exe file from the temporary
    // directory to the user's current directory.
    sprintf(szOrig, "%s\\RecoverHD.xbe", szOrigDir);
    sprintf(szTemp, "%s\\RecoverHD.xbe", szTempDir);
    if (!CopyFile(szTemp, szOrig, FALSE))
    {
        printf("Error - failed to copy xbe from temp directory.\n");
        goto done;
    }

    // If here, then we were successful!
    printf("\nRecoverHD.xbe successfully created.\n\n");
    nRetVal = 0;

done:

    // Clean up after ourselves (delete the temporary directory)
    RecursiveRemoveDirectory(szTempDir);

    // Restore the original directory
    SetCurrentDirectory(szOrigDir);

    return nRetVal;
}

