#pragma once


HANDLE WaveOpenFile
(
    IN  LPCSTR          szFileName,
    OUT WAVEFORMATEX**  ppWaveFormatEx,
    OUT PULONG          pcbData
);



BOOL
WaveSaveFile
(
    IN  LPCSTR          szFileName,
    OUT WAVEFORMATEX*   pWaveFormatEx,
    IN  PVOID           pvData,
    IN  ULONG           cbData
);


// -----------------------------------------------------------------------------------------
HANDLE
WaveOpenFileForSave
(
    IN  LPCSTR          szFileName,
    OUT WAVEFORMATEX*   pWaveFormatEx,
    DWORD               cbData

);




HRESULT Error(LPSTR szFormat, ...);
HRESULT Log  (LPSTR szFormat, ...);
HRESULT Merge(CWaveFile WaveFile[], LPSTR szOutputFile, DWORD dwNumChannels);
