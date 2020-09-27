//  FILEHASH.CPP
//
//  Created 3-May-2001 [JonT]

#include "packer.h"

//---------------------------------------------------------------------

                  
CFileHash::CFileHash()
{
    ZeroMemory(m_lhHashTable, TABLE_SIZE * sizeof (LPFILENODE));
}
                  

//  CFileHash::FindDuplicate
//      Searches for a duplicate file from a file that has already been encountered.
//      The files are tracked with a hash table. If a match is found, we return the
//      target of the previous copy. If it's not found, we add it into the data structure.

LPSTR
CFileHash::FindDuplicate(
    LPCSTR lpName,
    LPCSTR lpCopyRoot,
    LPCSTR lpCopyTarget
    )
{
    // Get the information about the file
    WIN32_FIND_DATA fd;
    HANDLE hfind = FindFirstFile(lpName, &fd);
    if (hfind == INVALID_HANDLE_VALUE)
        return NULL;

    // Get the bucket number
    DWORD dwBucket = HashSizeToIndex(fd.nFileSizeLow);

    // Search this bucket for the matching size
    for (LPFILENODE pnode = m_lhHashTable[dwBucket] ; pnode ; pnode = pnode->pnext)
    {
        // The size matching is just a hint. If it matches, we have to compare the
        // file byte-by-byte. Also check to make sure it comes from the correct target root.
        if (pnode->m_dwSize == fd.nFileSizeLow)
        {
            if (_stricmp(lpCopyRoot, pnode->m_lpCopyRoot) == 0 &&
                FileCompare(lpName, pnode->m_lpName))
                break;
        }
    }

    // If we found the file, return that we did
    if (pnode != NULL)
        return pnode->m_lpCopyTarget;

    // Otherwise, add the file
    // Create a new node
    LPFILENODE pfn = new FILENODE;
    pfn->m_lpName = _strdup(lpName);
    pfn->m_lpCopyTarget = _strdup(lpCopyTarget);
    pfn->m_lpCopyRoot = _strdup(lpCopyRoot);
    pfn->m_dwSize = fd.nFileSizeLow;

    // Find the hash bucket to link it into
    DWORD dwHashBucket = HashSizeToIndex(fd.nFileSizeLow);

    // Link the node into the bucket list
    pfn->pnext = m_lhHashTable[dwHashBucket];
    m_lhHashTable[dwHashBucket] = pfn;

    return NULL;
}

//---------------------------------------------------------------------
//  CFileHash - private

DWORD
CFileHash::HashSizeToIndex(
    DWORD dwSize
    )
{
    return (1664525 * dwSize + 1013904223) % TABLE_SIZE;
}


//  CFileHash::FileCompare
//      Compares a file byte-by-byte. Returns TRUE if the files are identical

#define READ_SIZE 4096

BOOL
CFileHash::FileCompare(
    LPCSTR lpFile1,
    LPCSTR lpFile2
    )
{
    // Open the file and add to the end of the target file
    HANDLE fh1 = CreateFile(lpFile1, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL,
        OPEN_EXISTING, 0, NULL);
    HANDLE fh2 = CreateFile(lpFile2, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL,
        OPEN_EXISTING, 0, NULL);
    if (fh1 == INVALID_HANDLE_VALUE || fh2 == INVALID_HANDLE_VALUE)
    {
        return FALSE;
    }

    // Allocate an aligned buffers for comparisons
    LPBYTE lpb1 = (LPBYTE)VirtualAlloc(NULL, READ_SIZE, MEM_COMMIT, PAGE_READWRITE);
    LPBYTE lpb2 = (LPBYTE)VirtualAlloc(NULL, READ_SIZE, MEM_COMMIT, PAGE_READWRITE);
    DWORD dwc1;
    DWORD dwc2;
    BOOL fRead1;
    BOOL fRead2;
    BOOL fRet = TRUE;

    // Read chunks of both files. If anything doesn't match: return values, bytes read
    do
    {
        // Read a chunk
        fRead1 = ReadFile(fh1, lpb1, READ_SIZE, &dwc1, NULL);
        fRead2 = ReadFile(fh2, lpb2, READ_SIZE, &dwc2, NULL);

        // Compare the results. Any differences and we're out of here
        if (fRead1 != fRead2 || dwc1 != dwc2 || memcmp(lpb1, lpb2, dwc1) != 0)
        {
            fRet = FALSE;
            break;
        }
    }
    while (fRead1 && dwc1 == READ_SIZE);

    // All done, clean up
    VirtualFree(lpb1, 0, MEM_RELEASE);
    VirtualFree(lpb2, 0, MEM_RELEASE);
    CloseHandle(fh1);
    CloseHandle(fh2);

    return fRet;
}
