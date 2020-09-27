#include <stdio.h>
#include <stdlib.h>
#include "bitmap.h"
#include "bootsec.h"
#include "defs.h"
#include "diskutil.h"
#include "image.h"
#include "file.h"
#include "mem.h"
#include "text.h"
#include "debug.h"

#ifdef WIN32
#include "win32\resource.h"
#include "win32\winmisc.h"
#endif // WIN32


// Used with GetFragNames
typedef struct tagGETFRAGNAMES
{
    DWORD                   dwFirst;
    DWORD                   dwCount;
    LPSTR                   lpszList;
} GETFRAGNAMES, *LPGETFRAGNAMES;


// Image signatures
const char                  sig11[] = { 'D', 'S' };
const char                  sig30[] = { 'R', 'R' };


// Local prototypes
BOOL SectorIOWrapper(BYTE, LPVOID, DWORD, DWORD, DWORD, BOOL);


// Reads the image Pad Sector and Lead Sector
int ReadImageHeader(FILEHANDLE fhImage, LPLEADSECTOR lpLeadSector, LPDWORD lpdwFirstData, BOOL fRequireCurrentVersion)
{
    BYTE                    bSector[CBSEC];
    LPLEADSECTOR            lpls;
    BOOL                    fOldImage;
    UINT                    i;
    
    // Seek to the beginning of the file
    FileSetPos(fhImage, 0, SEEK_SET);

    // Read the Pad Sector
    if(FileRead(fhImage, &bSector, CBSEC) != CBSEC)
    {
        return ERROR_FILEREAD;
    }

    // Validate the Pad Sector
    for(i = 0; i < CBSEC - 2; i++)
    {
        if(bSector[i] != 0xFF)
        {
            return ERROR_INVALIDIMAGE;
        }
    }
    
    // Check the image version
    if(bSector[CBSEC - 2] == sig11[0] && bSector[CBSEC - 1] == sig11[1])
    {
        fOldImage = TRUE;
    }
    if(bSector[CBSEC - 2] == sig30[0] && bSector[CBSEC - 1] == sig30[1])
    {
        fOldImage = FALSE;
    }
    else
    {
        return ERROR_INVALIDIMAGE;
    }

    if(fRequireCurrentVersion && fOldImage)
    {
        return ERROR_INVALIDIMAGE;
    }

    // Read the Lead Sector
    if(FileRead(fhImage, &bSector, CBSEC) != CBSEC)
    {
        return ERROR_FILEREAD;
    }

    lpls = (LPLEADSECTOR)&bSector;
    
    // Certain flags were unavailable in old versions
    if(fOldImage)
    {
        if(lpls->wFlags & OPTIONS_COMPRESS || lpls->wFlags & OPTIONS_ALLOCATED)
        {
            return ERROR_INVALIDIMAGE;
        }
    }
    
    // Make sure we got a valid number of fragments.
    // This is also a good check for the structure's
    // integrity.
    if(lpls->bFragCount < 1 || lpls->bFragCount > MAX_IMAGE_FRAGMENTS)
    {
        return ERROR_INVALIDIMAGE;
    }
    
    // Copy the Lead Sector
    MemCopy(lpLeadSector, lpls, sizeof(LEADSECTOR));
    
    // Return the offset to the first data area
    if(lpdwFirstData)
    {
        if(fOldImage)
        {
            if(lpLeadSector->bSequence == 0)
            {
                *lpdwFirstData = (2 + (lpLeadSector->Reserved1 ? lpLeadSector->Reserved1 : lpLeadSector->dwCompBitmapSize)) * CBSEC;
            }
            else
            {
                *lpdwFirstData = 2 * CBSEC;
            }
        }
        else
        {
            *lpdwFirstData = (2 + lpLeadSector->dwCompBitmapSize + lpLeadSector->dwAllocBitmapSize) * CBSEC;
        }
    }

    // Update the Lead Sector to the 3.0 format
    if(fOldImage)
    {
        if(lpLeadSector->bFragCount == 1)
        {
            lpLeadSector->dwClustersPerFrag = lpLeadSector->dwClusterCount;
        }
        
        lpLeadSector->dwCompBitmapSize = 0;
        lpLeadSector->dwAllocBitmapSize = 0;
        lpLeadSector->dwFirstCluster = lpLeadSector->dwClustersPerFrag * lpLeadSector->bSequence;
        lpLeadSector->dwLastCluster = lpLeadSector->dwFirstCluster + lpLeadSector->dwClustersPerFrag - 1;
        lpLeadSector->fCompleted = TRUE;
    }

    // Return success
    return ERROR_NONE;
}


// Writes the image header (Pad Sector and Lead Sector)
int WriteImageHeader(FILEHANDLE fhImage, LPLEADSECTOR lpLeadSector, LPDWORD lpdwFirstData)
{
    BYTE                    bSector[CBSEC];

    // Seek to the beginning of the file
    FileSetPos(fhImage, 0, SEEK_SET);

    // Write the Pad Sector
    MemSet(&bSector, 0xFF, CBSEC - 2);

    bSector[CBSEC - 2] = sig30[0];
    bSector[CBSEC - 1] = sig30[1];
    
    if(FileWrite(fhImage, &bSector, CBSEC) != CBSEC)
    {
        return ERROR_FILEWRITE;
    }
    
    // Write the Lead Sector    
    MemCopy(&bSector, lpLeadSector, sizeof(LEADSECTOR));
    MemSet((LPBYTE)&bSector + sizeof(LEADSECTOR), 0, CBSEC - sizeof(LEADSECTOR));
    
    if(FileWrite(fhImage, &bSector, CBSEC) != CBSEC)
    {
        return ERROR_FILEWRITE;
    }
    
    // Return the offset to the first data area
    if(lpdwFirstData)
    {
        *lpdwFirstData = (2 + lpLeadSector->dwCompBitmapSize + lpLeadSector->dwAllocBitmapSize) * CBSEC;
    }

    // Return success
    return ERROR_NONE;
}


// Determines whether an aborted image can be recovered
BOOL CanRecoverImage(FILEHANDLE fhImage, BYTE bDrive)
{
    BYTE                    bSector[2][CBSEC];
    LEADSECTOR              ls;
    DWORD                   dwFirstData;
    RRBOOTSECTOR            bsImage;
    RRBOOTSECTOR            bsDrive;
    WORD                    i;
    int						nRet;

    // Read the image header
    if(ReadImageHeader(fhImage, &ls, &dwFirstData, TRUE))
    {
        return FALSE;
    }
    
    // Can't safely recover an image with allocated clusters only or compression
    if(ls.dwAllocBitmapSize || ls.dwCompBitmapSize)
    {
        return FALSE;
    }

    // Seek to the first data byte
    FileSetPos(fhImage, dwFirstData, SEEK_SET);

    // Read the boot sector from the image
    if(FileRead(fhImage, &bSector[0], CBSEC) != CBSEC)
    {
        return FALSE;
    }

    // Read the boot sector from the drive
    if(!ReadSector(bDrive, &bSector[1], 0, 1))
    {
        return FALSE;
    }
    
    // Compare the boot sectors
    if(MemCmp(&bSector[0], &bSector[1], CBSEC))
    {
        return FALSE;
    }

    // Convert the boot sectors
    if(nRet = ConvertBootSector((LPBGBOOTSECTOR)&bSector[0], &bsImage))
    {
        return FALSE;
    }

    if(nRet = ConvertBootSector((LPBGBOOTSECTOR)&bSector[1], &bsDrive))
    {
        return FALSE;
    }
    
    // Compare the FATs
    for(i = 0; i < bsImage.dwFATSize; i++)
    {
        if(FileRead(fhImage, &bSector[0], CBSEC) != CBSEC)
        {
            return FALSE;
        }
        
        if(!ReadSector(bDrive, &bSector[1], i + 1, 1))
        {
            return FALSE;
        }
        
        if(MemCmp(&bSector[0], &bSector[1], CBSEC))
        {
            return FALSE;
        }
    }
    
    return TRUE;
}


int GetClustersInFragment(LPLEADSECTOR lpLeadSector, LPIMGBITMAP lpAllocBitmap, LPDWORD lpdwClusters)
{
    DWORD                   dwCluster;
    BOOL                    fSet;
    int                     nRet;
    
    *lpdwClusters = lpLeadSector->dwClustersPerFrag;
    
    if(lpLeadSector->dwAllocBitmapSize && lpAllocBitmap)
    {
        for(dwCluster = 0; dwCluster < *lpdwClusters; dwCluster++)
        {
            if(nRet = GetBitmapBit(lpAllocBitmap, dwCluster, &fSet))
            {
                return nRet;
            }
            
            if(fSet)
            {
                (*lpdwClusters)++;
            }
        }
    }
    
    return ERROR_NONE;
}


int GetDriveInfo(BYTE bDrive, LPDRIVEINFO lpDrive)
{
    BGBOOTSECTOR            bs;
    int                     nRet;
    
    // Initialize the structure
    MemSet(lpDrive, 0, sizeof(DRIVEINFO));

    lpDrive->bDrive = bDrive;

    // Get the DPB
    if(!GetDPB(bDrive, &(lpDrive->dpb)))
    {
        return ERROR_DPB;
    }
    
    // Read the DOS boot sector
    if(!ReadSector(bDrive, &bs, 0, 1))
    {
        return ERROR_DRIVEREAD;
    }

    // Convert the DOS boot sector to our own internal version
    if(nRet = ConvertBootSector(&bs, &(lpDrive->bs)))
    {
        return nRet;
    }

    // Return success
    return ERROR_NONE;
}


int ConvertBootSector(LPBGBOOTSECTOR lpSource, LPRRBOOTSECTOR lpDest)
{
    LPBOOTSECTOR            lpbs;
    UINT                    i;

    // Check for NT FAT16
    if(!lpSource->bpb.wSectorSize || lpSource->bpb.wSectorSize > CBSEC)
    {
        return ERROR_UNSUPPORTEDFS;
    }

    if(lpSource->bpb.bSectorsPerCluster > 64)
    {
        return ERROR_DRIVECLUSTERSTOOBIG;
    }

    // Save the original boot sector
    MemCopy(&(lpDest->bsOriginal), lpSource, sizeof(BGBOOTSECTOR));

    // Save the important parts of the boot sector
    lpDest->dwSectorSize = lpSource->bpb.wSectorSize;
    DPF("dwSectorSize = %lu", lpDest->dwSectorSize);
     
    lpDest->dwSectorsPerCluster = lpSource->bpb.bSectorsPerCluster;
    DPF("dwSectorsPerCluster = %lu", lpDest->dwSectorsPerCluster);

    lpDest->dwReservedSectors = lpSource->bpb.wReservedSectors;
    DPF("dwReservedSectors = %lu", lpDest->dwReservedSectors);

    lpDest->dwFATCount = lpSource->bpb.bFATCount;
    DPF("dwFATCount = %lu", lpDest->dwFATCount);

    lpDest->dwFATSize = lpSource->bpb.wFATSize ? lpSource->bpb.wFATSize : lpSource->bpb.dwBigFATSize;
    DPF("dwFATSize = %lu", lpDest->dwFATSize);

    lpDest->dwRootDirEntries = lpSource->bpb.wRootDirEntries;
    DPF("dwRootDirEntries = %lu", lpDest->dwRootDirEntries);

    lpDest->dwPreDataSectors = lpDest->dwReservedSectors + (lpDest->dwFATCount * lpDest->dwFATSize) + ((lpDest->dwRootDirEntries * 32) / lpDest->dwSectorSize);
    DPF("dwPreDataSectors = %lu", lpDest->dwPreDataSectors);

    lpDest->dwTotalSectors = lpSource->bpb.wTotalSectors ? lpSource->bpb.wTotalSectors : lpSource->bpb.dwBigTotalSectors;
    DPF("dwTotalSectors = %lu", lpDest->dwTotalSectors);

    lpDest->dwTotalClusters = (lpDest->dwTotalSectors - lpDest->dwPreDataSectors) / lpDest->dwSectorsPerCluster;
    DPF("dwTotalClusters = %lu", lpDest->dwTotalClusters);

    lpDest->dwUsableSectors = lpDest->dwPreDataSectors + (lpDest->dwTotalClusters * lpDest->dwSectorsPerCluster);
    DPF("dwUsableSectors = %lu", lpDest->dwUsableSectors);

    if(lpDest->dwTotalClusters < 4096)
    {
        lpDest->dwBitsPerFATEntry = 12;
    }
    else if(lpDest->dwTotalClusters < 65526)
    {
        lpDest->dwBitsPerFATEntry = 16;
    }
    else
    {
        lpDest->dwBitsPerFATEntry = 32;
    }

    DPF("dwBitsPerFATEntry = %lu", lpDest->dwBitsPerFATEntry);

    if(lpSource->bpb.wFATSize)
    {
        lpbs = (LPBOOTSECTOR)lpSource;

        for(i = 0; i < sizeof(lpbs->abVolumeLabel); i++)
        {
            lpDest->szVolumeLabel[i] = lpbs->abVolumeLabel[i];
        }

        for(i = sizeof(lpbs->abVolumeLabel) - 1; i >= 0; i--)
        {
            if(' ' == lpDest->szVolumeLabel[i])
            {
                lpDest->szVolumeLabel[i] = 0;
            }
            else
            {
                break;
            }
        }

        for(i = 0; i < sizeof(lpbs->abFSType); i++)
        {
            lpDest->szFSType[i] = lpbs->abFSType[i];
        }

        for(i = sizeof(lpbs->abFSType) - 1; i >= 0; i--)
        {
            if(' ' == lpDest->szFSType[i])
            {
                lpDest->szFSType[i] = 0;
            }
            else
            {
                break;
            }
        }
    }
    else
    {
        for(i = 0; i < sizeof(lpSource->abVolumeLabel); i++)
        {
            lpDest->szVolumeLabel[i] = lpSource->abVolumeLabel[i];
        }

        for(i = sizeof(lpSource->abVolumeLabel) - 1; i >= 0; i--)
        {
            if(' ' == lpDest->szVolumeLabel[i])
            {
                lpDest->szVolumeLabel[i] = 0;
            }
            else
            {
                break;
            }
        }

        for(i = 0; i < sizeof(lpSource->abFSType); i++)
        {
            lpDest->szFSType[i] = lpSource->abFSType[i];
        }

        for(i = sizeof(lpSource->abFSType) - 1; i >= 0; i--)
        {
            if(' ' == lpDest->szFSType[i])
            {
                lpDest->szFSType[i] = 0;
            }
            else
            {
                break;
            }
        }

    }

    DPF("szVolumeLabel = %s", lpDest->szVolumeLabel);
    DPF("szFSType = %s", lpDest->szFSType);

    // Return success
    return ERROR_NONE;
}


#ifdef WIN32

BOOL CALLBACK FragListDlgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    static LPGETFRAGNAMES       lpgfn;
    DWORD                       i;
    int                         nSel;
    
    switch(uMsg)
    {
        case WM_INITDIALOG:
            lpgfn = (LPGETFRAGNAMES)lParam;

            // Add each fragment to the list box
            for(i = lpgfn->dwFirst; i < lpgfn->dwCount; i++)
            {
                SendDlgItemMessage(hWnd, IDC_FRAGLIST, LB_ADDSTRING, 0, (LPARAM)(lpgfn->lpszList + MAX_PATH));
            }

            SendDlgItemMessage(hWnd, IDC_FRAGLIST, LB_SETCURSEL, 0, 0);

            break;

        case WM_CLOSE:
            EndDialog(hWnd, ERROR_USERCANCEL);

            break;

        case WM_COMMAND:
            switch(LOWORD(wParam))
            {
                case IDOK:
                    EndDialog(hWnd, ERROR_NONE);

                    break;

                case IDCANCEL:
                    SendMessage(hWnd, WM_CLOSE, 0, 0);

                    break;

                case IDC_FRAGLIST:
                    if(HIWORD(wParam) != LBN_DBLCLK)
                    {
                        break;
                    }

                    // fall through

                case IDC_CHANGE:
                    if((nSel = SendDlgItemMessage(hWnd, IDC_FRAGLIST, LB_GETCURSEL, 0, 0)) == LB_ERR)
                    {
                        MessageBeep(MB_ICONEXCLAMATION);
                        break;
                    }
                    
                    if(GetFragmentName(hWnd, nSel + lpgfn->dwFirst + 1, lpgfn->lpszList + (MAX_PATH * (nSel + lpgfn->dwFirst))))
                    {
                        SendDlgItemMessage(hWnd, IDC_FRAGLIST, LB_DELETESTRING, nSel, 0);
                        SendDlgItemMessage(hWnd, IDC_FRAGLIST, LB_INSERTSTRING, nSel, (LPARAM)(lpgfn->lpszList + (MAX_PATH * (nSel + lpgfn->dwFirst))));
                        SendDlgItemMessage(hWnd, IDC_FRAGLIST, LB_SETCURSEL, nSel, 0);
                    }

                    break;
            }

            break;
    }

    return FALSE;
}

#endif // WIN32


void GetDefaultFragNames(LPSTR lpszFile, LPIMAGESTRUCT lpImage, DWORD dwFirst, DWORD dwCount)
{
    char                        szDrivePart[_MAX_DRIVE] = { 0 };
    char                        szDirPart[_MAX_DIR] = { 0 };
    char                        szFilePart[_MAX_FNAME] = { 0 };
    char                        szExtPart[_MAX_EXT] = { 0 };
    char                        szPath[MAX_PATH];
    int                         nPathLen;
    int                         nExtLen;
    LPSTR                       lpsz;
    DWORD                       i;

    if(lpszFile && *lpszFile)
    {
        // Separate the file name into it's component parts
        _splitpath(lpszFile, szDrivePart, szDirPart, szFilePart, szExtPart);

#ifndef WIN32

        // The filename may have been generated from the image label.  If this is
        // the case, remove any invalid characters.
        for(lpsz = szFilePart; *lpsz; lpsz++)
        {
            if(' ' == *lpsz)
            {
                *lpsz = '_';
            }
        }

#endif // WIN32

    }

    if(!szFilePart[0])
    {
        StrCpy(szFilePart, "Unnamed");
    }

    if(!szExtPart[0])
    {
        StrCpy(szExtPart, ".img");
    }

    _makepath(szPath, szDrivePart, szDirPart, szFilePart, szExtPart);

    nPathLen = GetFullPathInPlace(szPath, MAX_PATH);
    nExtLen = StrLen(szExtPart) - 1;
        
    // Fill in the fragment names
    for(i = dwFirst; dwCount; dwCount--, i++)
    {
        if(i > 0)
        {
            lpsz = &szPath[nPathLen - nExtLen];
            
            if((i + 1 < 100) && (nExtLen >= 1))
            {
                lpsz++;
            
                if((i + 1 < 10) && (nExtLen >= 2))
                {
                    lpsz++;
                }
            }

            itoa((int)(i + 1), lpsz, 10);
        }

        StrCpy(lpImage->aszFragNames[i], szPath);
    }
}


int GetFragNames(LPIMAGESTRUCT lpImage, DWORD dwFirst, DWORD dwCount)
{

#ifdef WIN32

    GETFRAGNAMES                gfn;
    int                         nRet;

#else // WIN32

    char                        szFile[MAX_PATH];
    DWORD                       i;

#endif // WIN32

    if(!dwCount)
    {
        return ERROR_NONE;
    }

#ifdef WIN32

    gfn.dwFirst = dwFirst;
    gfn.dwCount = dwCount;
    gfn.lpszList = lpImage->aszFragNames[0];

    if(nRet = DialogBoxParam(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_FRAGLIST), lpImage->hWnd, FragListDlgProc, (LPARAM)&gfn))
    {
        return nRet;
    }

#else // WIN32

    for(i = dwFirst; dwCount; dwCount--, i++)
    {
        while(TRUE)
        {
            printf("Enter the filename for fragment %lu:  ", i + 1);
            gets(szFile);
            
            if(StrLen(szFile))
            {
                if(!ValidateFileName(lpImage->aszFragNames[i]))
                {
                    GetFullPath(lpImage->aszFragNames[i], szFile, MAX_PATH);
                    break;
                }
            }
        }
    }

    printf(CRLF);

#endif // WIN32

    return ERROR_NONE;
}


FILEHANDLE OpenFragmentForRead(BYTE bFrag, LPIMAGESTRUCT lpImage)
{
    FILEHANDLE              fhImage = INVALID_FILE_HANDLE;

    while((fhImage = FileOpen(lpImage->aszFragNames[bFrag], OPEN_ACCESS_READONLY, 0, ACTION_FILE_OPEN)) == INVALID_FILE_HANDLE)
    {

#ifdef WIN32

        ImageMessageBox(lpImage->hWnd, MB_ICONEXCLAMATION, "Unable to open %s", lpImage->aszFragNames[bFrag]);
        
        if(!GetFileName(lpImage->hWnd, "Please enter the correct path...", lpImage->aszFragNames[bFrag]))
        {
            return INVALID_FILE_HANDLE;
        }

#else // WIN32

        FlushKeyboardBuffer();
        
        printf(CRLF "Unable to open %s" CRLF, lpImage->aszFragNames[bFrag]);
        printf("Please enter the correct path:  ");
        
        gets(lpImage->aszFragNames[bFrag]);
        
        printf(CRLF);

#endif // WIN32

    }
    
    return fhImage;
}


FILEHANDLE OpenFragmentForWrite(BYTE bFrag, LPIMAGESTRUCT lpImage)
{
    FILEHANDLE              fhImage = INVALID_FILE_HANDLE;
    
    if((fhImage = FileOpen(lpImage->aszFragNames[bFrag], OPEN_ACCESS_READWRITE, ATTR_NORMAL, ACTION_FILE_CREATE)) == INVALID_FILE_HANDLE)
    {
        if((lpImage->wFlags & OPTIONS_OVERWRITE) || (lpImage->wFlags & OPTIONS_FORCE))
        {
            fhImage = FileOpen(lpImage->aszFragNames[bFrag], OPEN_ACCESS_READWRITE, 0, ACTION_FILE_TRUNCATE);
        }
        else
        {
            if(AskUser(lpImage, "%s already exists." CRLF "Would you like to overwrite this file?", lpImage->aszFragNames[bFrag]))
            {
                fhImage = FileOpen(lpImage->aszFragNames[bFrag], OPEN_ACCESS_READWRITE, 0, ACTION_FILE_TRUNCATE);
            }
            else
            {
                fhImage = INVALID_FILE_HANDLE;
            }
        }
    }
            
    return fhImage;
}


BOOL ReadSectorEx(BYTE bDrive, LPVOID lpvBuffer, DWORD dwSector, DWORD dwCount, DWORD dwSectorSize)
{
    LPBYTE                  lpbBuffer   = (LPBYTE)lpvBuffer;
    WORD                    wCount;
    BOOL                    fSuccess;

    while(dwCount)
    {
        wCount = (WORD)min(dwCount, SECTORIO_MAX_SECTORS);
        fSuccess = ReadSector(bDrive, lpbBuffer, dwSector, wCount);

        if(!fSuccess)
        {
            return FALSE;
        }

        lpbBuffer += wCount * dwSectorSize;
        dwSector += wCount;
        dwCount -= wCount;
    }

    return TRUE;
}


BOOL WriteSectorEx(BYTE bDrive, LPVOID lpvBuffer, DWORD dwSector, DWORD dwCount, DWORD dwSectorSize)
{
    LPBYTE                  lpbBuffer   = (LPBYTE)lpvBuffer;
    WORD                    wCount;
    BOOL                    fSuccess;

    while(dwCount)
    {
        wCount = (WORD)min(dwCount, SECTORIO_MAX_SECTORS);
        fSuccess = WriteSector(bDrive, lpbBuffer, dwSector, wCount);

        if(!fSuccess)
        {
            return FALSE;
        }

        lpbBuffer += wCount * dwSectorSize;
        dwSector += wCount;
        dwCount -= wCount;
    }

    return TRUE;
}