#ifdef WIN32
#include <windows.h>
#else // WIN32
#include <conio.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#endif // WIN32

#include <stdarg.h>
#include "bootsec.h"
#include "defs.h"
#include "diskutil.h"
#include "image.h"
#include "text.h"
#include "debug.h"


const LPSTR             g_lpszErrorMsg[] =
{
    "",
    "An error occurred while attempting to open the image file.",
    "An error occurred while attempting to write to the image file.",
    "An error occurred while attempting to read from the image file.",
    "An error occurred while attempting to read from the drive.",
    "An error occurred while attempting to write to the drive.",
    "Out of memory.",
    "This operation would require more than the maximum number of image fragments.",
    "This is not a valid " RAWREAD "/" RAWWRITE " image.",
    "The target drive is too small to write this image.",
    "Lock failed.",
    "The image framents are out of sequence.",
    "Can't write to a compressed drive.",
    "Can't run on a drive with clusters greater than 32kb.",
    "Can't restore an image with clusters greater than 32kb.",
    "A fatal error occurred while compressing data.",
    "A fatal error occurred while uncompressing data.",
    "Unable to retrieve the DPB.",
    "This version of the MS-DOS kernel does not support FAT32.",
    "The source drive is too large to image with this version of " RAWREAD ".",
    "The recovery failed because the previous image was created successfully.",
    "The target drive's file system type differs from the image's.",
    "An internal error has occurred.  The current cluster is not equal to the" CRLF "offset stored in the image's Lead Sector.",
    "Unable to recover this image.",
    "Verification of the file write failed.  The image is corrupt.",
    "Verification of the disk write failed.  The drive is corrupt.",
    "Operation cancelled.",
    "Long filenames are not supported.",
    "The source drive's filesystem is not supported.",
};


#ifdef WIN32

int ImageMessageBox(HWND hWnd, UINT uType, LPCSTR lpszFormat, ...)
{
    va_list             va;
    char                szString[0xFF];

    va_start(va, lpszFormat);
    wvsprintf(szString, lpszFormat, va);
    va_end(va);

    MessageBeep(uType);
    
    return MessageBox(hWnd, szString, APPTITLE, MB_OK | MB_TASKMODAL | uType);
}

#else // WIN32

void GoPropaganda(void)
{

#ifdef PROPAGANDA1

    printf(PROPAGANDA1 CRLF);
   
#endif // PROPAGANDA1    

#ifdef PROPAGANDA2

    printf(PROPAGANDA2 CRLF);
   
#endif // PROPAGANDA2


#ifdef PROPAGANDA3

    printf(PROPAGANDA3 CRLF);
   
#endif // PROPAGANDA3


#ifdef PROPAGANDA4

    printf(PROPAGANDA4 CRLF);
   
#endif // PROPAGANDA4

#if defined(PROPAGANDA1) || defined(PROPAGANDA2) || defined(PROPAGANDA3) || defined(PROPAGANDA4)

    printf(CRLF);

#endif // defined(PROPAGANDA1) || defined(PROPAGANDA2) || defined(PROPAGANDA3) || defined(PROPAGANDA4))

}


int GoRawReadHelp(void)
{
    printf("Usage:  " RAWREAD " <drive:> [imagefile] [maxsize]" CRLF);
    printf(CRLF);
    printf("Options:" CRLF);
    printf("    /A          Only read allocated clusters." CRLF);
    printf("    /B          Scan the FAT for bad clusters." CRLF);
    printf("    /C          Compress drive data." CRLF);
    printf("    /D          Dump drive information." CRLF);
    printf("    /F          Ignore errors.  Should only be used by advanced" CRLF);
    printf("                users." CRLF);
    printf("    /L          Do not attempt to lock the drive." CRLF);
    printf("    /N          Automatically generate image fragment names." CRLF);
    printf("    /O          Overwrites if the image file already exists." CRLF);
    printf("    /Q          Do not print status information." CRLF);
    printf("    /R          Do not attempt to recover an aborted image." CRLF);
    printf("    /S          Span media." CRLF);
    printf("    /T          Store a text comment in the image.  The next parameter" CRLF);
    printf("                after this switch should be the comment enclosed in" CRLF);
    printf("                quotation marks (\" \")" CRLF);
    printf("    /V          Verify data was written to the file properly." CRLF);
    
    cprintf(CRLF "-- More --");
    FlushKeyboardBuffer();
    getch();
    cprintf(SZBACK);

    printf("Required parameters:" CRLF);
    printf("    drive:      Drive to image." CRLF);
    printf(CRLF);
    printf("Optional parameters:" CRLF);
    printf("    imagefile   File to write image to.  If this parameter is omitted," CRLF);
    printf("                the volume label of the source drive will be used." CRLF);
    printf("    maxsize     Maximum image fragment size in megabytes.  Note that" CRLF);
    printf("                maxsize must appear as the last parameter.  Leaving this" CRLF);
    printf("                field blank or 0 indicates an unlimited image size." CRLF);
    
    return -1;
}


int GoRawWriteHelp(void)
{
    printf("Usage:  " RAWWRITE " <imagefile> <drive:>" CRLF);
    printf(CRLF);
    printf("Options:" CRLF);
    printf("    /B          Scan the FAT for bad clusters." CRLF);
    printf("    /D          Dump drive and image information.  When this option is" CRLF);
    printf("                specified, the image file name or drive path may be left off." CRLF);
    printf("    /F          Ignore errors.  Should only be used by advanced users." CRLF);
    printf("    /I          Ignore image type." CRLF);
    printf("    /Q          Do not print status information." CRLF);
    printf("    /V          Verify data was written to the drive properly." CRLF);
    printf("    /W          Wipe unused clusters." CRLF);
    printf(CRLF);
    printf("Required parameters:" CRLF);
    printf("    imagefile   Image file to read from." CRLF);
    printf("    drive:      Drive to write image to." CRLF);
    
    return -1;
}

#endif // WIN32


int ErrorMsg(LPIMAGESTRUCT lpImage, int n)
{

#ifdef WIN32

    HWND                    hWnd    = NULL;

    if(lpImage)
    {
        hWnd = lpImage->hWnd;
    }
    
    ImageMessageBox(hWnd, MB_ICONHAND, g_lpszErrorMsg[n]);

#else // WIN32

    printf(CRLF CRLF "\a%s" CRLF, g_lpszErrorMsg[n]);

#endif // WIN32
    
    return n;
} 

BOOL AskUser(LPIMAGESTRUCT lpImage, LPCSTR lpszFormat, ...)
{
    BOOL                    fResult;

#ifdef WIN32

    // BUGBUG: this will only work on x86
    fResult = ImageMessageBox(lpImage->hWnd, MB_YESNO | MB_TASKMODAL | MB_ICONQUESTION, lpszFormat, *(&lpszFormat + 1)) == IDYES;

#else // WIN32

    va_list                 va;
    char                    c;
    
    printf(CRLF);
    
    va_start(va, lpszFormat);
    vprintf(lpszFormat, va);
    va_end(va);

    printf(" (Y/N) ");
    
    FlushKeyboardBuffer();

    do
    {
        c = getch();
        c = toupper(c);
    }
    while(c != 'Y' && c != 'N');
   
    cprintf("%c" CRLF CRLF, c);
    
    fResult = (c == 'Y') ? TRUE : FALSE;

#endif // WIN32

    return fResult;
}


void InsertDisk(LPIMAGESTRUCT lpImage, LPCSTR lpszFilename, char cDrive)
{

#ifdef WIN32

    ImageMessageBox(lpImage->hWnd, MB_ICONEXCLAMATION | MB_OK | MB_TASKMODAL, "Please insert the disk for %s in drive %c:", lpszFilename, cDrive);

#else // WIN32

    FlushKeyboardBuffer();
    cprintf("\a" CRLF "Please insert the disk for %s in drive %c: and press any key... ", lpszFilename, cDrive);
    getch();
    cprintf(CRLF CRLF);

#endif // WIN32

}


#ifndef WIN32
#pragma optimize("", off)

void FlushKeyboardBuffer(void)
{
    _asm
    {
        mov     ax, 0C00h
        int     21h
    }
}

#pragma optimize("", on)
#endif // WIN32


int GetFullPath(LPSTR lpszFullPath, LPCSTR lpszRelPath, UINT uBufSize)
{

#ifdef WIN32

    if(!GetFullPathName(lpszRelPath, uBufSize, lpszFullPath, NULL))
    {
        return -1;
    }

#else // WIN32

    if(!_fullpath(lpszFullPath, lpszRelPath, uBufSize))
    {
        return -1;
    }

#endif // WIN32

    return StrLen(lpszFullPath);
}


int GetFullPathInPlace(LPSTR lpszPath, UINT uBufSize)
{
    char                    szFullPath[MAX_PATH];
    int                     nRet;

    if((nRet = GetFullPath(szFullPath, lpszPath, sizeof(szFullPath))) == -1)
    {
        return -1;
    }

    strncpy(lpszPath, szFullPath, uBufSize);

    return nRet;
}


int SplitPath(LPCSTR lpszPath, LPSTR lpszDir, LPSTR lpszFile)
{
    char                    szFullPath[MAX_PATH];
    LPSTR                   lpsz;

    StrCpy(szFullPath, lpszPath);

    // Walk backwards from the end of the absolute path until
    // we hit a : or \ indicating part of the path
    for(lpsz = szFullPath; *lpsz; lpsz++);
    
    while(*lpsz != '\\')
    {
        lpsz--;
    }

    // Null terminate the string here
    *lpsz = 0;

    // Copy the strings
    if(lpszDir)
    {
        StrCpy(lpszDir, szFullPath);
    }

    if(lpszFile)
    {
        StrCpy(lpszFile, lpsz + 1);
    }

    return 0;
}


int GetFileLocation(LPCSTR lpszFile, LPSTR lpszDirPart, LPSTR lpszFilePart)
{
    char                    szFullPath[MAX_PATH];

    if(GetFullPath(szFullPath, lpszFile, sizeof(szFullPath)) == -1)
    {
        return -1;
    }

    return SplitPath(szFullPath, lpszDirPart, lpszFilePart);
}


#ifdef WIN32

int GetShortFileName(LPCSTR lpszPath, LPSTR lpszFile)
{
    char                    szShortPath[MAX_PATH];

    if(GetShortPathName(lpszPath, szShortPath, sizeof(szShortPath)))
    {
        return -1;
    }

    return GetFileLocation(szShortPath, NULL, lpszFile);
}

#endif // WIN32


int SetFilePath(LPCSTR lpszFile, LPCSTR lpszDir, LPSTR lpszResult)
{
    char                    szFile[MAX_FILE];
    char                    szDir[MAX_DIR];

    // Split the paths
    if(GetFileLocation(lpszFile, NULL, szFile) == -1)
    {
        return -1;
    }

    if(GetFileLocation(lpszDir, szDir, NULL) == -1)
    {
        return -1;
    }

    // Append the path and file
    StrCpy(lpszResult, szDir);
    
    if(lpszResult[StrLen(lpszResult) - 1] != '\\')
    {
        StrCat(lpszResult, "\\");
    }

    StrCat(lpszResult, szFile);

    return StrLen(lpszResult);
}


int ValidateFileName(LPCSTR lpszFile)
{
    char                    szFile[MAX_FILE];
    LPSTR                   lpsz;
    UINT                    uFileLen, uExtLen;
    BOOL                    fSpace;

    GetFileLocation(lpszFile, NULL, szFile);

    uFileLen = 0;
    uExtLen = 0;
    fSpace = FALSE;

    lpsz = szFile;

    while(*lpsz && *lpsz != '.')
    {
        if(*lpsz == ' ')
        {
            fSpace = TRUE;
        }
        
        uFileLen++;
        lpsz++;
    }

    if(*lpsz == '.')
    {
        lpsz++;
    }
    
    while(*lpsz)
    {
        if(*lpsz == ' ')
        {
            fSpace = TRUE;
        }
        
        uExtLen++;
        lpsz++;
    }

    if(uFileLen > 8 || uExtLen > 3 || fSpace)
    {
        return ERROR_LFN;
    }

    return ERROR_NONE;
}


LPSTR StrCpy(LPSTR lpszDest, LPCSTR lpszSrc)
{

#ifdef WIN32

    return lstrcpy(lpszDest, lpszSrc);

#else // WIN32

    return _fstrcpy(lpszDest, lpszSrc);

#endif // WIN32

}


int StrLen(LPCSTR lpszString)
{

#ifdef WIN32

    return lstrlen(lpszString);

#else // WIN32

    return _fstrlen(lpszString);

#endif // WIN32

}


int StrCmp(LPCSTR lpszString1, LPCSTR lpszString2)
{

#ifdef WIN32

    return lstrcmp(lpszString1, lpszString2);

#else // WIN32

    return _fstrcmp(lpszString1, lpszString2);

#endif // WIN32

}


int StrCmpI(LPCSTR lpszString1, LPCSTR lpszString2)
{

#ifdef WIN32

    return lstrcmpi(lpszString1, lpszString2);

#else // WIN32

    return _fstricmp(lpszString1, lpszString2);

#endif // WIN32

}


LPSTR StrCat(LPSTR lpszDest, LPCSTR lpszSrc)
{

#ifdef WIN32

    return lstrcat(lpszDest, lpszSrc);

#else // WIN32

    return _fstrcat(lpszDest, lpszSrc);

#endif // WIN32

}


void DumpBootSector(LPSTR lpszText, LPRRBOOTSECTOR lpbs)
{
    printf("%s" CRLF "%s", lpszText, LINE);

    printf("Sector size:                                %lu bytes" CRLF, lpbs->dwSectorSize);
    printf("Sectors per cluster:                        %lu" CRLF, lpbs->dwSectorsPerCluster);
    printf("Reserved sectors:                           %lu" CRLF, lpbs->dwReservedSectors);
    printf("Number of FATs:                             %lu" CRLF, lpbs->dwFATCount);
    printf("FAT size:                                   %lu bytes" CRLF, lpbs->dwFATSize);
    printf("Number of root directory entries:           %lu" CRLF, lpbs->dwRootDirEntries);
    printf("Number of sectors prior to first cluster:   %lu" CRLF, lpbs->dwPreDataSectors);
    printf("Total number of drive sectors:              %lu" CRLF, lpbs->dwTotalSectors);
    printf("Total number of drive clusters:             %lu" CRLF, lpbs->dwTotalClusters);
    printf("Total number of usable sectors:             %lu" CRLF, lpbs->dwUsableSectors);
    printf("Number of bits per FAT entry:               %lu" CRLF, lpbs->dwBitsPerFATEntry);
    printf("Volume label:                               %s" CRLF, lpbs->szVolumeLabel);
    printf("Filesystem type:                            %s" CRLF, lpbs->szFSType);
    
    printf(CRLF);
}


void DumpDPB(LPSTR lpszText, LPDPB lpdpb)
{
    WORD                    wDosVer = GetDosVersion();

    printf("%s" CRLF "%s", lpszText, LINE);

    printf("Drive number:                               %u" CRLF, lpdpb->bDrive);
    printf("Unit number:                                %u" CRLF, lpdpb->bUnit);
                                                        
    printf("Sector size:                                %u bytes" CRLF, lpdpb->wSectorSize);
    printf("Sectors per cluster:                        %u" CRLF, lpdpb->bClusterMask + 1);
    printf("First FAT sector:                           %u" CRLF, lpdpb->wFirstFAT);
    printf("FAT count:                                  %u" CRLF, lpdpb->bFATCount);
    
    if(!IS_FAT32_KERNEL(wDosVer))
    {
        printf("Number of root directory entries:           %u" CRLF, lpdpb->wOldRootEntries);
        printf("Sector number of cluster 2:                 %u" CRLF, lpdpb->wOldFirstSector);
        printf("Maximun number of drive clusters:           %u" CRLF, lpdpb->wOldMaxCluster - 1);
        printf("FAT size:                                   %u sectors" CRLF, lpdpb->wOldFATSize);
        printf("First root directory sector:                %u" CRLF, lpdpb->wOldDirSector);
    }
    
    printf("Device driver header address:               0x%8.8lX" CRLF, lpdpb->dwDriverAddr);
    printf("Media descriptor:                           %u" CRLF, lpdpb->bMedia);
    printf("Access byte:                                %u" CRLF, lpdpb->bFlags);
    printf("Next DPB address:                           0x%8.8lX" CRLF, lpdpb->dwNextDPB);
    
    if(!IS_FAT32_KERNEL(wDosVer))
    {
        printf("Last allocated cluster:                     %u" CRLF, lpdpb->wOldNextFree);
    }
    
    printf("Free clusters:                              %lu" CRLF, lpdpb->dwFreeCount);
    
    if(IS_FAT32_KERNEL(wDosVer))
    {
        printf("Extended flags:                             %u" CRLF, lpdpb->wExtFlags);
        printf("Filesystem info sector:                     %u" CRLF, lpdpb->wFSInfoSector);
        printf("Backup boot sector:                         %u" CRLF, lpdpb->wBackupBootSector);
        printf("Sector number of cluster 2:                 %lu" CRLF, lpdpb->dwFirstSector);
        printf("Maximun number of drive clusters:           %lu" CRLF, lpdpb->dwMaxCluster - 1);
        printf("FAT size:                                   %lu sectors" CRLF, lpdpb->dwFATSize);
        printf("First root directory cluster:               %lu" CRLF, lpdpb->dwRootCluster);
        printf("Last allocated cluster:                     %lu" CRLF, lpdpb->dwNextFree);
    }
    
    printf(CRLF);
}


void DumpLeadSector(LPSTR lpszText, LPLEADSECTOR lpls)
{
    printf("%s" CRLF "%s", lpszText, LINE);

    printf("Number of clusters in the image:            %lu" CRLF, lpls->dwClusterCount);
    printf("Fragment sequence number:                   %u" CRLF, lpls->bSequence);
    printf("Number of fragments in the image:           %u" CRLF, lpls->bFragCount);
    printf("Number of clusters per fragment:            %lu" CRLF, lpls->dwClustersPerFrag);
    printf("Next fragment filename:                     %s" CRLF, lpls->szNextFrag);
    printf("Flags:                                      0x%4.4X" CRLF, lpls->wFlags);
    printf("Compression bitmap size:                    %lu sectors" CRLF, lpls->dwCompBitmapSize);
    printf("Allocation bitmap size:                     %lu sectors" CRLF, lpls->dwAllocBitmapSize);
    printf("First cluster in this fragment:             %lu" CRLF, lpls->dwFirstCluster + 2);
    printf("Last cluster in this fragment:              %lu" CRLF, lpls->dwLastCluster + 2);
    printf("Fragment was successfully created:          %s" CRLF, lpls->fCompleted ? "TRUE" : "FALSE");
    
    if(StrLen(lpls->szComment))
    {
        printf(CRLF "%s" CRLF, lpls->szComment);
    }
    
    printf(CRLF);
}


BOOL IsDrivePath(LPSTR pszPath, LPBYTE pbDriveNumber)
{
    char                    cDrive;
    
    cDrive = toupper(pszPath[0]);

    if(cDrive < 'A' || cDrive > 'Z')
    {
        return FALSE;
    }

    if(pszPath[1] != ':')
    {
        return FALSE;
    }

    if(pszPath[2] != 0)
    {
        return FALSE;
    }

    if(pbDriveNumber)
    {
        *pbDriveNumber = cDrive - 'A';
    }

    return TRUE;
}