#include "cwavefile.h"
#include "helpers.h"

/********************************************************************************
********************************************************************************/
CWaveFile::CWaveFile(void)
{
m_pWfx = NULL;
memset(m_szFileName, 0, MAX_PATH);
m_cbDataTotal = 0;
m_cbDataRemaining=0;
m_hFile = NULL;
m_bSilence = FALSE;
m_bLoadedForRead  = FALSE;
m_bLoadedForWrite = FALSE;
};


/********************************************************************************
********************************************************************************/
CWaveFile::~CWaveFile(void)
{
    InternalFree();
};

/********************************************************************************
********************************************************************************/
HRESULT CWaveFile::OpenForRead(const LPSTR szFileName)
{
    HRESULT hr = S_OK;
    DWORD   dwRead = 0;
    BOOL bResult = false;
    
    //Check to see if this object already has data.
    if (m_bLoadedForRead)
    {
        Log("Loading %s over already-loaded file %s; unloading %s", szFileName, m_szFileName, m_szFileName);
        InternalFree();
    }
    
    //Save the filenane
    strncpy(m_szFileName, szFileName, MAX_PATH-1);

    //Get the WFX and file length, and set m_hFile to the beginning of the wave data.
    if (SUCCEEDED(hr))
    {
        m_hFile = WaveOpenFile(szFileName, &m_pWfx, &m_cbDataTotal);
        if (INVALID_HANDLE_VALUE == m_hFile)
        {
            m_hFile = NULL;
            hr = E_FAIL;
        }
    }

    if (SUCCEEDED(hr))
    {
        m_pWfx->cbSize = 0;
        m_cbDataRemaining = m_cbDataTotal;
    }


    //If this procedure failed, free everything..
    if (FAILED(hr))
    {
        InternalFree();
    }
    else
        m_bLoadedForRead = true;

    return hr;
};


/********************************************************************************
********************************************************************************/
HRESULT CWaveFile::OpenForSilence(const WAVEFORMATEX &wfx)
{
    //Check to see if this object already has data.
    if (m_bLoadedForRead)
    {
        Log("Internal Error: Loading Silence over already-loaded file %s; unloading %s", m_szFileName, m_szFileName);
        InternalFree();
    }

    m_pWfx = (LPWAVEFORMATEX)LocalAlloc(LPTR, sizeof(WAVEFORMATEX));
    *m_pWfx = wfx;

    //m_pvData = LocalAlloc(LPTR, wfx.nBlockAlign);
    m_cbDataTotal = 0;
    m_bLoadedForRead = true;
    m_bSilence = TRUE;
    strcpy(m_szFileName, szSilence);
    return S_OK;
};




/********************************************************************************
********************************************************************************/
HRESULT CWaveFile::ReadData(LPVOID pvData, DWORD cbSize, LPDWORD pcbRead)
{
    DWORD cbDataRead = 0;
    HRESULT hr = S_OK;
    BOOL bResult = TRUE;

    if (m_bSilence)
    {
        memset(pvData, (m_pWfx->wBitsPerSample==16) ? 0 : 0x80 , cbSize);
        *pcbRead = cbSize;
    }
    else
    {
        bResult = ReadFile(m_hFile, pvData, cbSize, &cbDataRead, NULL);
        if (!bResult)
        {
            Error("Failure reading %s", m_szFileName);
            hr = E_FAIL;
        }
        else
        {
            //If we read more than we thought we had, that's weird.  But handle it by just saying we read the correct amount.
            if (cbDataRead > m_cbDataRemaining)
                cbDataRead = m_cbDataRemaining;

            m_cbDataRemaining -= cbDataRead;
        }

        *pcbRead = cbDataRead;
    }

    return hr;
};



/********************************************************************************
********************************************************************************/
HRESULT CWaveFile::OpenForWrite(const LPSTR szFileName, const WAVEFORMATEX &wfx, DWORD cbSize)
{
HRESULT hr = S_OK;
BOOL bResult = true; //true = success, false = failure.
DWORD dwRes = 0;
CHAR szFullPathName[MAX_PATH * 2] = {0};
ULARGE_INTEGER li = {0};
CHAR *szFilePart = NULL;

m_pWfx = (LPWAVEFORMATEX)LocalAlloc(LPTR, sizeof(WAVEFORMATEX));
*m_pWfx = wfx;

//Is something already loaded?
if (m_bLoadedForWrite)
{
    Log("Opening %s for write over already-loaded-for-write file %s; unloading %s", szFileName, m_szFileName, m_szFileName);
    hr = E_FAIL;
}

//Can we open this?
if (SUCCEEDED(hr))
{
    strncpy(m_szFileName, szFileName, MAX_PATH - 1);
    m_hFile = WaveOpenFileForSave(m_szFileName, m_pWfx, cbSize);
    if (!m_hFile || m_hFile==INVALID_HANDLE_VALUE)
    {
        Error("Can't save data to %s", m_szFileName);
        hr = E_FAIL;
    }
}


//Check the disk space.
if (SUCCEEDED(hr))
{
    dwRes = GetFullPathName(szFileName, MAX_PATH * 2 - 1, szFullPathName, &szFilePart);
    if (!dwRes)
    {
        Log("Error: Couldn't GetFullPathName for %s; can't check for available disk space.", szFileName);
    }
    else
    {
        //Wipe off the file name completely.
        szFilePart[0] = NULL;
        GetDiskFreeSpaceEx(szFullPathName, &li, 0, 0);
        if (li.QuadPart <= cbSize)
        {
            Log("Error: Not enough disk space.");
            Log("       You need %d bytes but only have %I64u.", cbSize, li.QuadPart);
            hr = E_FAIL;
        }
    }
}


if (SUCCEEDED(hr))
    m_bLoadedForWrite = TRUE;
else
{
    InternalFree();
}

return hr;
};


/********************************************************************************
********************************************************************************/
HRESULT CWaveFile::WriteData(LPVOID pvData, DWORD cbSize, LPDWORD pcbWritten)
{
HRESULT hr = S_OK;
DWORD cbWritten = 0;
BOOL bResult;

    bResult = WriteFile(m_hFile, pvData, cbSize, pcbWritten, NULL);
    if (!bResult)
    {
        Error("Failure writing %s", m_szFileName);
        hr = E_FAIL;
    }
    else if (*pcbWritten != cbSize)
    {
        Error("Error: Wrote %u instead of %u bytes from %s", *pcbWritten, cbSize, m_szFileName);
        hr = E_FAIL;
    }

    return hr;
};



/********************************************************************************
********************************************************************************/
HRESULT  CWaveFile::Close(void)
{
    HRESULT hr = S_OK;
    if (!m_bLoadedForRead && !m_bLoadedForWrite)
    {
        Error("Trying to free a file that's not loaded for read or write");
        hr = E_FAIL;
    }
    InternalFree();

    return hr;
};



/********************************************************************************
********************************************************************************/
HRESULT CWaveFile::InternalFree(void)
{
    HRESULT hr = S_OK;

    if (m_pWfx)
    {
        LocalFree(m_pWfx);
        m_pWfx = NULL;
    }

    memset(m_szFileName, 0, MAX_PATH);
    m_cbDataTotal     = 0;
    m_cbDataRemaining = 0;
    m_bLoadedForRead = false;
    m_bSilence = false;
    if (m_hFile)
    {        
        CloseHandle(m_hFile);
        m_hFile = NULL;
    }
    return hr;
};


/********************************************************************************
********************************************************************************/
HRESULT CWaveFile::GetWaveFormatEx(WAVEFORMATEX *pWfx)
{
    if (m_bLoadedForRead || m_bLoadedForWrite)
    {
        //Copy the data out.
        *pWfx = *m_pWfx;    
        return S_OK;
    }
    else
    {
        return E_FAIL;
    }
}




/********************************************************************************
********************************************************************************/
HRESULT CWaveFile::GetFileName(LPSTR szFileName)
{
    if (m_bLoadedForRead || m_bLoadedForWrite)
    {
        strncpy(szFileName, m_szFileName, MAX_PATH - 1);
        return S_OK;
    }
    else
    {
        return E_FAIL;
    }
};




/********************************************************************************
********************************************************************************/
HRESULT CWaveFile::GetLength(LPDWORD pcbData)
{
    *pcbData = m_cbDataTotal;
    return S_OK;
}