#include "bitmap.h"
#include "defs.h"
#include "file.h"
#include "mem.h"


// Initializes the compression bitmap
int InitBitmap(LPIMGBITMAP lpBitmap, DWORD dwSize, FILEHANDLE hFile, DWORD dwFileOffset, BOOL fExisting)
{
    DWORD                   dwFilePos;
    int                     nRet;
    DWORD                   dw;
    
    // Fill the structure
    lpBitmap->dwSize = dwSize;
    lpBitmap->dwPage = BITMAP_PAGE_ERROR;
    lpBitmap->fDirty = FALSE;
    lpBitmap->hFile = hFile;
    lpBitmap->dwFileOffset = dwFileOffset;
    
    // Allocate memory for the bit buffer
    if(!(lpBitmap->lpbBuffer = MemAlloc(CBSEC)))
    {
        return ERROR_OOM;
    }
    
    // If this is a new bitmap, write 0's out to the file
    if(!fExisting)
    {
        MemSet(lpBitmap->lpbBuffer, 0, CBSEC);
        
        // Save the current file position
        dwFilePos = FileGetPos(hFile);
    
        // Seek to the beginning of the bitmap
        FileSetPos(hFile, dwFileOffset * CBSEC, SEEK_SET);
    
        // Write 0's to the file
        for(dw = 0; dw < dwSize; dw++)
        {
            if(FileWrite(hFile, lpBitmap->lpbBuffer, CBSEC) != CBSEC)
            {
                return ERROR_FILEWRITE;
            }
        }

        // Restore the file position
        FileSetPos(hFile, dwFilePos, SEEK_SET);
    }
    
    // Load the first bitmap page
    if(nRet = ClusterInMemory(lpBitmap, 0))
    {
        return nRet;
    }
    
    // Return success
    return ERROR_NONE;
}


// Reads the necessary bitmap page into memory
int ClusterInMemory(LPIMGBITMAP lpBitmap, DWORD dwCluster)
{
    DWORD                   dwPageNeeded;
    DWORD                   dwFilePos;
    int                     nRet;
    
    dwPageNeeded = dwCluster / 8 / CBSEC;
    
    // Is this cluster already in memory?
    if(dwPageNeeded == lpBitmap->dwPage)
    {
        return ERROR_NONE;
    }
    
    // Flush the buffer
    if(nRet = FlushBitmapBuffer(lpBitmap))
    {
        return nRet;
    }

    // Save the current file position
    dwFilePos = FileGetPos(lpBitmap->hFile);
    
    // Seek to the beginning of the bitmap page
    FileSetPos(lpBitmap->hFile, (lpBitmap->dwFileOffset + dwPageNeeded) * CBSEC, SEEK_SET);
        
    // Read in the bitmap pages
    if(FileRead(lpBitmap->hFile, lpBitmap->lpbBuffer, CBSEC) != CBSEC)
    {
    	return ERROR_FILEREAD;
    }
        
    // Restore the file position
    FileSetPos(lpBitmap->hFile, dwFilePos, SEEK_SET);
    
    // Update the bitmap info
    lpBitmap->dwPage = dwPageNeeded;
    lpBitmap->fDirty = FALSE;
    
    // Return success
    return ERROR_NONE;
}


// Flushes the bitmap's dirty buffer
int FlushBitmapBuffer(LPIMGBITMAP lpBitmap)
{
    DWORD                   dwFilePos;
    
    // If there's nothing dirty, return immediately
    if(!lpBitmap->fDirty)
    {
        return ERROR_NONE;
    }
    
    // Make sure no-one's being stupid
    if(lpBitmap->dwPage == BITMAP_PAGE_ERROR)
    {
        return ERROR_FILEWRITE;
    }
    
    // Save the current file position
    dwFilePos = FileGetPos(lpBitmap->hFile);
    
    // Seek to the correct point in the bitmap
    FileSetPos(lpBitmap->hFile, (lpBitmap->dwFileOffset + lpBitmap->dwPage) * CBSEC, SEEK_SET);
    
    // Write the buffer
    if(FileWrite(lpBitmap->hFile, lpBitmap->lpbBuffer, CBSEC) != CBSEC)
    {
        return ERROR_FILEWRITE;
    }
    
    // Restore the file position
    FileSetPos(lpBitmap->hFile, dwFilePos, SEEK_SET);
    
    // Set the bitmap's dirty flag to false
    lpBitmap->fDirty = FALSE;
    
    // Return success
    return ERROR_NONE;
}


// Retrieves a bitmap bit
int GetBitmapBit(LPIMGBITMAP lpBitmap, DWORD dwCluster, LPBOOL lpfSet)
{
    int                     nRet;
    
    // Get the correct page
    if(nRet = ClusterInMemory(lpBitmap, dwCluster))
    {
        return nRet;
    }
        
    dwCluster -= lpBitmap->dwPage * CBSEC * 8;
    
    // Get the cluster's value
    if(lpfSet)
    {
        *lpfSet = *(lpBitmap->lpbBuffer + (dwCluster / 8)) & (1 << (dwCluster % 8));
    }
    
    // Return success
    return ERROR_NONE;
}


// Sets a bitmap bit
int SetBitmapBit(LPIMGBITMAP lpBitmap, DWORD dwCluster)
{
    int                     nRet;
    
    // Get the correct page
    if(nRet = ClusterInMemory(lpBitmap, dwCluster))
    {
        return nRet;
    }
        
    dwCluster -= lpBitmap->dwPage * CBSEC * 8;
    
    // Set the cluster's value
    *(lpBitmap->lpbBuffer + (dwCluster / 8)) |= 1 << (dwCluster % 8);
    
    // Set the bitmap's buffer to dirty
    lpBitmap->fDirty = TRUE;
    
    // Return success
    return ERROR_NONE;
}


int DiscardBitmap(LPIMGBITMAP lpBitmap)
{
    int                     nRet;
    
    // Flush the bitmap's buffer
    if(nRet = FlushBitmapBuffer(lpBitmap))
    {
        return nRet;
    }
    
    // Free memory
    MemFree(lpBitmap->lpbBuffer);
    lpBitmap->lpbBuffer = NULL;
    
    // Return success
    return ERROR_NONE;
}