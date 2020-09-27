//  RESOURCE.CPP
//
//  Created 13-Mar-2001 [JonT]

#include "packer.h"

//---------------------------------------------------------------------

//  CExeResource::SetFile
//      Sets an EXE for adding resources to

BOOL CExeResource::SetFile(
    LPCTSTR lpTemplateExe,
    LPCTSTR lpNewExe
    )
{
    Close();

    // Copy the template .EXE to the new .EXE name
    if (!CopyFile(lpTemplateExe, lpNewExe, FALSE))
    {
        m_error = GetLastError();
        return FALSE;
    }

    // Open the file and prepare to append to it
    m_handle = CreateFile(lpNewExe, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);
    if (m_handle == INVALID_HANDLE_VALUE)
    {
        m_error = GetLastError();
        return FALSE;
    }
    SetFilePointer(m_handle, 0, NULL, FILE_END);

    return TRUE;
}


//  CExeResource::Close
//      Finishes the resource update and either writes or doesn't write the changes

typedef struct _EOFSTRUCT
{
    DWORD dwcFiles;
    DWORD dwSignature;
} EOFSTRUCT;

void
CExeResource::Close()
{
    EOFSTRUCT es;
    DWORD dwc;

    // Make sure we have something actually open
    if (m_handle == INVALID_HANDLE_VALUE)
        return;

    // Write out the variable-sized array of file sizes
    WriteFile(m_handle, m_dwSizes, m_dwcFiles * sizeof (DWORD), &dwc, NULL);

    // Write the end of file structure out
    es.dwcFiles = m_dwcFiles;
    es.dwSignature = 'XOBX';
    WriteFile(m_handle, &es, sizeof (EOFSTRUCT), &dwc, NULL);

    // Make sure the EXE size is set correctly so signing tools don't tromp us
    DWORD dwSeekPosition = SetFilePointer(m_handle, 0, NULL, FILE_CURRENT);

    // Seek back to the start of the file and read the dos EXE header.
    // Write it back out with our seek position tucked away in it.
    IMAGE_DOS_HEADER dos;
    SetFilePointer(m_handle, 0, NULL, FILE_BEGIN);
    ReadFile(m_handle, &dos, sizeof (dos), &dwc, NULL);
    *(DWORD*)(&dos.e_res[0]) = dwSeekPosition;
    SetFilePointer(m_handle, 0, NULL, FILE_BEGIN);
    WriteFile(m_handle, &dos, sizeof (dos), &dwc, NULL);

    CloseHandle(m_handle);
    m_handle = INVALID_HANDLE_VALUE;
}


//  CExeResource::AddResourceFromFile
//      Reads in an entire file and inserts it as a resource into the executable.

#define MAX_COPY_SIZE 65536

BOOL
CExeResource::AddResourceFromFile(
    LPCTSTR lpFile
    )
{
    HANDLE fh;
    DWORD dwc;
    DWORD dwcWritten;
    DWORD dwcTotal = 0;

    // Open the file and add to the end of the target file
    fh = CreateFile(lpFile, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING,
                    FILE_FLAG_NO_BUFFERING, NULL);
    if (fh == INVALID_HANDLE_VALUE)
    {
        m_error = GetLastError();
        return FALSE;
    }

    // Allocate an aligned buffer for unbuffered copies
    LPBYTE lpb = (LPBYTE)VirtualAlloc(NULL, MAX_COPY_SIZE, MEM_COMMIT, PAGE_READWRITE);

    // Read chunks of the input file and write to the output file
    do
    {
        // Read a chunk
        if (!ReadFile(fh, lpb, MAX_COPY_SIZE, &dwc, NULL))
            break;

        // Keep track of the number of bytes written
        dwcTotal += dwc;
        m_dwCurrent += dwc;

        // Display progress as we go
        fprintf(stderr, "progress: %d of %d bytes           \r", m_dwCurrent, m_dwTotal);

        // Write out the number of bytes read
        if (!WriteFile(m_handle, lpb, dwc, &dwcWritten, NULL))
            break;
    }
    while (dwc == MAX_COPY_SIZE);
    CloseHandle(fh);
    VirtualFree(lpb, 0, MEM_RELEASE);

    // Return error if we couldn't read anything
    if (dwcTotal == 0)
    {
        m_error = GetLastError();
        return FALSE;
    }

    // Record the number of bytes written in this file
    m_dwSizes[m_dwcFiles++] = dwcTotal;

    return TRUE;
}
