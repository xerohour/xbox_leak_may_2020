#include <stdio.h>
#include "bitmap.h"
#include "bootsec.h"
#include "debug.h"
#include "defs.h"
#include "diskutil.h"
#include "fatutil.h"
#include "mem.h"
#include "image.h"
#include "text.h"


typedef struct tagFINDUNALLOCATEDCLUSTERS
{
    DWORD                   dwStartCluster;
    LPIMGBITMAP             lpBitmap;
    DWORD                   dwTotalFound;
} FINDUNALLOCATEDCLUSTERS, *PFINDUNALLOCATEDCLUSTERS, FAR *LPFINDUNALLOCATEDCLUSTERS;

typedef struct tagFINDBADCLUSTERS
{
    LPDWORD FAR *           lplpList;
    DWORD                   dwTotalFound;
} FINDBADCLUSTERS, *PFINDBADCLUSTERS, FAR *LPFINDBADCLUSTERS;


// Converts a FAT entry as it sits on the disk into a DWORD cluster entry.
DWORD FATtoDWORD(LPBYTE lpbBuffer, DWORD dwOffset, DWORD dwCluster, BYTE bBitsPerEntry)
{
    DWORD                   dwRet = 0;
    
    lpbBuffer += dwOffset;
    
    switch(bBitsPerEntry)
    {
        case 12:
            if(dwCluster & 1)
            {
                dwRet = (DWORD)HINIBBLE(*lpbBuffer) | (DWORD)(*(lpbBuffer + 1)) << 4;
            }
            else
            {
                dwRet = (DWORD)*lpbBuffer | (DWORD)(LONIBBLE(*(lpbBuffer + 1)) << 8);
            }
           
            break;
       
        case 16:
            dwRet = (DWORD)*(LPWORD)lpbBuffer;
            
            break;
        
        case 32:
            dwRet = *(LPDWORD)lpbBuffer;
            
            break;
    }
    
    return dwRet;
}


int WalkFAT(LPDRIVEINFO lpDrive, BYTE bFAT, DWORD dwStartCluster, DWORD dwClusterCount, WALKFATCALLBACK fnCallback, LPVOID lpvContext)
{
    LPBYTE                  lpbBuffer;
    DWORD                   dwPage;
    DWORD                   dwOffset;
    DWORD                   dwCluster;
    
    // Don't be stupid
    if(bFAT != 1 && bFAT != 2)
    {
        return ERROR_DRIVEREAD;
    }
    
    // Modify the starting cluster.  Must be at least 2.
    dwStartCluster = max(2, dwStartCluster);
    
    // Modify the cluster count to be sure we don't try to read more than
    // there actually are.
    dwClusterCount = min(lpDrive->bs.dwTotalClusters - (dwStartCluster - 2), dwClusterCount);
    
    // Our internal cluster counter is 0-based from the starting cluster
    dwCluster = 0;

    // We're reading the FAT in 3-sector pages
    dwPage = (dwStartCluster * lpDrive->bs.dwBitsPerFATEntry / 8) / (lpDrive->bs.dwSectorSize * 3);
    
    // Allocate a 3-sector buffer
    if(!(lpbBuffer = MemAlloc((UINT)(lpDrive->bs.dwSectorSize * 3))))
    {
        return ERROR_OOM;
    }
    
    // Read in each page of the FAT.  Pass each entry to the callback function.
    while(dwPage < DIVUP(lpDrive->bs.dwFATSize, 3))
    {
        if(!ReadSector(lpDrive->bDrive, lpbBuffer, lpDrive->bs.dwReservedSectors + ((bFAT - 1) * lpDrive->bs.dwFATSize) + (dwPage * 3), 3))
        {
            MemFree(lpbBuffer);
            return ERROR_DRIVEREAD;
        }
        
        dwOffset = (dwCluster + dwStartCluster) * lpDrive->bs.dwBitsPerFATEntry / 8;
        dwOffset -= dwPage * lpDrive->bs.dwSectorSize * 3;

        while(dwOffset < lpDrive->bs.dwSectorSize * 3 && dwCluster < dwClusterCount)
        {
            // Call the callback function
            if(!(*fnCallback)(dwCluster + dwStartCluster, FATtoDWORD(lpbBuffer, dwOffset, dwCluster + dwStartCluster, (BYTE)lpDrive->bs.dwBitsPerFATEntry), (BYTE)lpDrive->bs.dwBitsPerFATEntry, lpvContext))
            {
                MemFree(lpbBuffer);
                return ERROR_NONE;
            }
            
            // Increment the offset.  Remember that if this is a 12-bit FAT, we have to
            // tweak the incrementation a bit.
            if(lpDrive->bs.dwBitsPerFATEntry == 12 && dwCluster & 1)
            {
                dwOffset += 2;
            }
            else
            {
                dwOffset += lpDrive->bs.dwBitsPerFATEntry / 8;
            }
            
            // Increment the cluster
            dwCluster++;
        }
        
        // Next page, please
        dwPage++;
    }
    
    // Free memory
    MemFree(lpbBuffer);
    
    // Return success
    return ERROR_NONE;
}


BOOL CALLBACK FindUnallocatedClustersCallback(DWORD dwCluster, DWORD dwValue, BYTE bBits, LPVOID lpvContext)
{
    LPFINDUNALLOCATEDCLUSTERS lpUn = (LPFINDUNALLOCATEDCLUSTERS)lpvContext;
    
    // If the cluster is unused, set it's corresponding bit in the bitmap
    if(dwValue == 0)
    {
        if(lpUn->lpBitmap)
        {
            SetBitmapBit(lpUn->lpBitmap, dwCluster - lpUn->dwStartCluster);
        }
        
        lpUn->dwTotalFound++;
    }
    
    return TRUE;
}


int FindUnallocatedClusters(LPEVERYTHING lpData, DWORD dwStartCluster, DWORD dwClusterCount, LPIMGBITMAP lpBitmap, LPDWORD lpdwCount, BOOL fQuiet)
{
    FINDUNALLOCATEDCLUSTERS un;
    int                     nRet;
    
    un.dwStartCluster = dwStartCluster;
    un.lpBitmap = lpBitmap;
    un.dwTotalFound = 0;

    if(lpData->lpImage->wFlags & OPTIONS_ALLOCATED)
    {
        if(!fQuiet)
        {

#ifdef WIN32

            SetProgressLabel(lpData->lpImageRes->hWnd, "Scanning for unallocated clusters...");

#else // WIN32
        
            printf("Scanning for unallocated clusters... ");

#endif // WIN32

        }
        
        if(nRet = WalkFAT(lpData->lpDrive, 1, dwStartCluster, dwClusterCount, FindUnallocatedClustersCallback, &un))
        {
            return nRet;
        }

        if(!fQuiet)
        {

#ifndef WIN32

            printf("%lu" CRLF CRLF, un.dwTotalFound);

#endif // WIN32

        }
    }
    
    if(lpdwCount)
    {
        *lpdwCount = un.dwTotalFound;
    }
    
    return ERROR_NONE;
}


BOOL CALLBACK FindBadClustersCallback(DWORD dwCluster, DWORD dwValue, BYTE bBits, LPVOID lpvContext)
{
    LPFINDBADCLUSTERS       lpBad   = (LPFINDBADCLUSTERS)lpvContext;
    LPDWORD                 lpList  = *(lpBad->lplpList);

    // Is this cluster marked bad?
    switch(bBits)
    {
        case 12:
            if(dwValue != 0xFF7)
            {
                return TRUE;
            }

            break;

        case 16:
            if(dwValue != 0xFFF7)
            {
                return TRUE;
            }

            break;

        case 32:
            if(dwValue != 0xFFFFFFF7)
            {
                return TRUE;
            }

            break;
    }
    
    // Add this cluster to the list
    lpList = (LPDWORD)MemReAlloc(lpList, (UINT)((++lpBad->dwTotalFound) * sizeof(DWORD)));

    if(!lpList)
    {
        return FALSE;
    }
        
    lpList[lpBad->dwTotalFound - 1] = dwCluster - 2;
    
    *(lpBad->lplpList) = lpList;
    
    return TRUE;
}


int FindBadClusters(LPEVERYTHING lpData, DWORD dwStartCluster, DWORD dwClusterCount, LPDWORD FAR * lplpList, LPDWORD lpdwCount, BOOL fQuiet)
{
    FINDBADCLUSTERS         bad;
    int                     nRet;
    
    if(lplpList)
    {
        *lplpList = NULL;
    }
    
    bad.lplpList = lplpList;
    bad.dwTotalFound = 0;
    
    if(lpData->lpImage->wFlags & OPTIONS_FINDBAD)
    {
        if(!fQuiet)
        {

#ifdef WIN32

            SetProgressLabel(lpData->lpImageRes->hWnd, "Scanning for bad clusters...");

#else // WIN32
        
            printf("Scanning for bad clusters... ");

#endif // WIN32

        }
        
        if(nRet = WalkFAT(lpData->lpDrive, 1, dwStartCluster, dwClusterCount, FindBadClustersCallback, &bad))
        {
            return nRet;
        }

        if(!fQuiet)
        {

#ifndef WIN32

            printf("%lu" CRLF CRLF, bad.dwTotalFound);

#endif // WIN32

        }
    }
    
    if(lpdwCount)
    {
        *lpdwCount = bad.dwTotalFound;
    }
    
    return ERROR_NONE;
}

