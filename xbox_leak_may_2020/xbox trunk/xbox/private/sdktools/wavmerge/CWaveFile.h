#pragma once

#include "globals.h"

class CWaveFile
{
public:
    CWaveFile(void);
    ~CWaveFile(void);
    HRESULT OpenForRead     (const LPSTR szFileName);
    HRESULT OpenForSilence  (const WAVEFORMATEX &wfx);
    HRESULT OpenForWrite    (const LPSTR szFileName, const WAVEFORMATEX &wfx, DWORD cbSize);
    HRESULT ReadData        (VOID *pData, DWORD dwSize, LPDWORD pcbRead);
    HRESULT WriteData       (VOID *pData, DWORD dwSize, LPDWORD pcbWritten);
    HRESULT Close           (void);
    
    HRESULT GetWaveFormatEx(WAVEFORMATEX *pWfx);
    HRESULT GetFileName(LPSTR szFileName);
    HRESULT GetLength(LPDWORD pdwLength);

private:

    HRESULT InternalFree(void);
    bool         m_bLoadedForRead;
    bool         m_bLoadedForWrite;
    LPWAVEFORMATEX m_pWfx;
    DWORD        m_cbDataTotal;
    DWORD        m_cbDataRemaining;
    CHAR         m_szFileName[MAX_PATH];
    HANDLE       m_hFile;
    BOOL         m_bSilence;
};






