// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
// File:      CFile.cpp
// Contents:  
// Revisions: 8-Jul-2001: Created (jeffsim)
//
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++


// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++ INCLUDE FILES +++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// "stdafx.h"       -- Precompiled header file
#include "stdafx.h"
#include <malloc.h>

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++ FUNCTIONS +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

CFile::CFile(char *szName, DWORD dwFlags) : CInitedObject()
{
    strcpy(m_szName, szName);
    m_fMemoryFile = dwFlags & FILE_MEMORY;
    if (m_fMemoryFile)
    {
        // File only exists in memory.  Allocate a block
        m_dwMemSize = 20000;
        m_rgbyData = (BYTE*)malloc(m_dwMemSize);
        if (!m_rgbyData)
            FatalError(E_OUTOFMEMORY);

        m_pbyMemCur = m_rgbyData;
    }
    else
    {
        // File exists on disk
        if (dwFlags & FILE_READ)
            m_hfile = CreateFile(szName, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
        else
            m_hfile = CreateFile(szName, GENERIC_READ | GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
        if (m_hfile == INVALID_HANDLE_VALUE)
        {
            m_fClosed = true;
            return;
        }
    }
    m_fClosed = false;
    SetInited(S_OK);
}

CFile::~CFile()
{
    Close();
}

void CFile::IncreaseMemSize(DWORD dwBytes)
{
    DWORD dwOrigSize = m_dwMemSize;

    assert(m_dwMemSize);
    while (m_dwMemSize < (m_pbyMemCur - m_rgbyData) + dwBytes)
    {
        assert(m_dwMemSize*2);
        m_dwMemSize *= 2;
    }

    if (dwOrigSize != m_dwMemSize)
    {
        DWORD dwOffset = m_pbyMemCur - m_rgbyData;
        m_rgbyData = (BYTE*)realloc(m_rgbyData, m_dwMemSize);
        if (!m_rgbyData)
            FatalError(E_OUTOFMEMORY);

        m_pbyMemCur = m_rgbyData + dwOffset;
    }
}

void CFile::Close()
{
    if (m_fClosed)
        return;

    if (m_fMemoryFile)
    {
        free(m_rgbyData);
    }
    else
        CloseHandle(m_hfile);

    m_fClosed = true;
}

bool CFile::WriteData(void *pvData, DWORD cBytes)
{
    if (m_fMemoryFile)
    {
        IncreaseMemSize(cBytes);
        memcpy(m_pbyMemCur, pvData, cBytes);
        m_pbyMemCur += cBytes;
    }
    else
    {
        DWORD dwWritten;
        if (!WriteFile(m_hfile, (BYTE*)pvData, cBytes, &dwWritten, NULL) || dwWritten != cBytes)
            return false;
    }

    return true;

}
bool CFile::ReadData(void *pvData, DWORD cBytes)
{
    if (m_fMemoryFile)
    {
        IncreaseMemSize(cBytes);
        memcpy(m_pbyMemCur, pvData, cBytes);
        m_pbyMemCur += cBytes;
    }
    else
    {
        DWORD dwRead;
        if (!ReadFile(m_hfile, (BYTE*)pvData, cBytes, &dwRead, NULL) || dwRead != cBytes)
            return FALSE;
    }

    return TRUE;

}

DWORD CFile::GetCurLoc()
{
    if (m_fMemoryFile)
    {
        return m_pbyMemCur - m_rgbyData;
    }
    else
    {
        // NOTE: only works with files < 2^32 bytes.  However, the GDF_DIR_ENTRY 'filesize' var can only be that big, so presumably this is okay...
        return SetFilePointer(m_hfile, 0, NULL, FILE_CURRENT);
    }
}

bool CFile::WriteBYTE(BYTE byData)
{
    if (m_fMemoryFile)
    {
        IncreaseMemSize(1);
        memcpy(m_pbyMemCur, &byData, 1);
        m_pbyMemCur += 1;
    }
    else
    {
        DWORD dwWritten;
        BYTE rgby[] = {byData};
        if (!WriteFile(m_hfile, rgby, 1, &dwWritten, NULL) || dwWritten != 1)
            return false;
    }
    return true;
}

bool CFile::WriteWORD(WORD wData)
{
    if (m_fMemoryFile)
    {
        IncreaseMemSize(2);
        memcpy(m_pbyMemCur, &wData, 2);
        m_pbyMemCur += 2;
    }
    else
    {
        DWORD dwWritten;
        WORD rgw[] = {wData};
        if (!WriteFile(m_hfile, rgw, 2, &dwWritten, NULL) || dwWritten != 2)
            return false;
    }

    return true;
}


bool CFile::WriteDWORD(DWORD dwData)
{
    if (m_fMemoryFile)
    {
        IncreaseMemSize(4);
        memcpy(m_pbyMemCur, &dwData, 4);
        m_pbyMemCur += 4;
    }
    else
    {
        DWORD dwWritten;
        DWORD rgdw[] = {dwData};
        if (!WriteFile(m_hfile, rgdw, 4, &dwWritten, NULL) || dwWritten != 4)
            return false;
    }
    return true;
}

bool CFile::WriteString(char *sz)
{
    if (m_fMemoryFile)
    {
        IncreaseMemSize(strlen(sz)+4);
        if (!WriteDWORD(strlen(sz)))
            return false;
        memcpy(m_pbyMemCur, sz, strlen(sz));
        m_pbyMemCur += strlen(sz);
    }
    else
    {
        DWORD dwWritten;
        if (!WriteDWORD(strlen(sz)))
            return false;
        if (!WriteFile(m_hfile, sz, strlen(sz), &dwWritten, NULL) || dwWritten != strlen(sz))
            return false;
    }
    return true;
}

bool CFile::ReadBYTE(BYTE *pbyData)
{
    if (m_fMemoryFile)
    {
        memcpy(pbyData, m_pbyMemCur, 1);
        m_pbyMemCur += 1;
    }
    else
    {
        DWORD dwRead;
        if (!ReadFile(m_hfile, pbyData, 1, &dwRead, NULL) || dwRead != 1)
            return false;
    }
    return true;
}

bool CFile::ReadWORD(WORD *pwData)
{
    if (m_fMemoryFile)
    {
        memcpy(pwData, m_pbyMemCur, 2);
        m_pbyMemCur += 2;
    }
    else
    {
        DWORD dwRead;
        if (!ReadFile(m_hfile, pwData, 2, &dwRead, NULL) || dwRead != 2)
            return false;
    }
    return true;
}

bool CFile::ReadDWORD(DWORD *pdwData)
{
    if (m_fMemoryFile)
    {
        memcpy(pdwData, m_pbyMemCur, 4);
        m_pbyMemCur += 4;
    }
    else
    {
        DWORD dwRead;
        if (!ReadFile(m_hfile, pdwData, 4, &dwRead, NULL) || dwRead != 4)
            return false;
    }
    return true;
}

bool CFile::ReadString(char *sz)
{
    if (m_fMemoryFile)
    {
        DWORD dwStrlen;
        if (!ReadDWORD(&dwStrlen))
            return false;
        memcpy(sz, m_pbyMemCur, dwStrlen);
        sz[dwStrlen] = '\0';
        m_pbyMemCur += dwStrlen;
    }
    else
    {
        DWORD dwStrlen, dwRead;
        if (!ReadDWORD(&dwStrlen))
            return false;
        if (!ReadFile(m_hfile, sz, dwStrlen, &dwRead, NULL) || dwRead != dwStrlen)
            return false;
        sz[dwStrlen] = '\0';
    }
    return true;
}

bool CFile::SetPos(DWORD dwPos)
{
    if (m_fMemoryFile)
        m_pbyMemCur = m_rgbyData + dwPos;
    else
        SetFilePointer(m_hfile, dwPos, 0, FILE_BEGIN);
    
    return true;
}