/***************************************************************************
 *
 *  Copyright (C) 11/2/2001 Microsoft Corporation.  All Rights Reserved.
 *
 *  File:       writer.cpp
 *  Content:    Wave bank writer.
 *  History:
 *   Date       By      Reason
 *   ====       ==      ======
 *  11/2/2001   dereks  Created.
 *
 ****************************************************************************/

#include "wavbndli.h"

const DWORD CWaveBank::m_dwMinAlignment = 4;

BOOL CWaveBankEntry::m_fAutoLoad = TRUE;


/****************************************************************************
 *
 *  MakeHeaderString
 *
 *  Description:
 *      Removes all spaces from a string and replaces them with underscores.
 *
 *  Arguments:
 *      LPTSTR [in/out]: string.
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "MakeHeaderString"

static void
MakeHeaderString
(
    LPTSTR                  pszString
)
{
    while(*pszString)
    {
        if((*pszString >= TEXT('a')) && (*pszString <= TEXT('z')))
        {
            *pszString = TEXT('A') + *pszString - TEXT('a');
        }
        else if((*pszString >= TEXT('A')) && (*pszString <= TEXT('Z')))
        {
        }
        else if((*pszString >= TEXT('0')) && (*pszString <= TEXT('9')))
        {
        }
        else
        {
            *pszString = TEXT('_');
        }

        pszString++;
    }
}


/****************************************************************************
 *
 *  CWaveBankEntry
 *
 *  Description:
 *      Object constructor.
 *
 *  Arguments:
 *      CWaveBank * [in]: parent object.
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CWaveBankEntry::CWaveBankEntry"

CWaveBankEntry::CWaveBankEntry
(
    CWaveBank *             pParent
)
{
    DPF_ENTER();

    m_pParent = pParent;

    InsertTailListUninit(&m_pParent->m_lstEntries, &m_leEntry);

    m_pParent->m_dwEntryCount++;
    m_pParent->m_pParent->m_dwEntryCount++;

    DPF_LEAVE_VOID();
}


/****************************************************************************
 *
 *  ~CWaveBankEntry
 *
 *  Description:
 *      Object destructor.
 *
 *  Arguments:
 *      (void)
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CWaveBankEntry::~CWaveBankEntry"

CWaveBankEntry::~CWaveBankEntry
(
    void
)
{
    DPF_ENTER();

    RemoveEntryList(&m_leEntry);

    ASSERT(m_pParent->m_dwEntryCount);
    m_pParent->m_dwEntryCount--;

    ASSERT(m_pParent->m_pParent->m_dwEntryCount);
    m_pParent->m_pParent->m_dwEntryCount--;

    DPF_LEAVE_VOID();
}


/****************************************************************************
 *
 *  Initialize
 *
 *  Description:
 *      Initializes the object from a source file.
 *
 *  Arguments:
 *      LPCTSTR [in]: entry name.
 *      LPCTSTR [in]: wave file name.
 *      DWORD [in]: processing flags.
 *
 *  Returns:  
 *      HRESULT: COM result code.
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CWaveBankEntry::Initialize"

HRESULT
CWaveBankEntry::Initialize
(
    LPCTSTR                 pszEntryName,
    LPCTSTR                 pszFileName,
    DWORD                   dwFlags
)
{
    HRESULT                 hr          = S_OK;
    UINT                    nNameCount;
    UINT                    nFileCount;
    
    DPF_ENTER();

    ASSERT(pszFileName);

    //
    // Save the file path
    //

    GetFullPathName(pszFileName, NUMELMS(m_szFileName), m_szFileName, NULL);

    //
    // Save the flags
    //

    m_dwFlags = dwFlags;

    //
    // If no entry name was specified, generate one from the file name
    //

    if(pszEntryName)
    {
        _tcsncpy(m_szEntryName, pszEntryName, NUMELMS(m_szEntryName));
    }
    else
    {
        _splitpath(m_szFileName, NULL, NULL, m_szEntryName, NULL);
    }

    _tcsncpy(m_szHeaderName, m_szEntryName, NUMELMS(m_szHeaderName));
    MakeHeaderString(m_szHeaderName);

    FindDuplicateEntry(m_szHeaderName, &nNameCount, m_szFileName, &nFileCount);

    if(nNameCount)
    {
        DPF_ERROR("An entry with that name already exists in the bank, or that name has been reserved");
        hr = E_FAIL;
    }
    else if(nFileCount)
    {
        DPF_WARNING("An entry using that filename already exists in the bank");
    }

    //
    // Open the file
    //

    if(SUCCEEDED(hr) && m_fAutoLoad)
    {
        hr = Load();
    }
    
    DPF_LEAVE_HRESULT(hr);

    return hr;
}


/****************************************************************************
 *
 *  FindDuplicateEntry
 *
 *  Description:
 *      Checks the wave bank for a duplicate entry.
 *
 *  Arguments:
 *      LPCTSTR [in]: entry name.
 *      UINT * [out]: count of duplicate named entries found.
 *      LPCTSTR [in]: entry file path.
 *      UINT * [out]: count of duplicate pathed entries found.
 *
 *  Returns:  
 *      HRESULT: COM result code.
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CWaveBankEntry::FindDuplicateEntry"

HRESULT
CWaveBankEntry::FindDuplicateEntry
(
    LPCTSTR                 pszName,
    UINT *                  pnNameCount,
    LPCTSTR                 pszPath,
    UINT *                  pnPathCount
)
{
    HRESULT                 hr          = S_OK;
    PLIST_ENTRY             pleEntry;
    CWaveBankEntry *        pEntry;
    
    DPF_ENTER();

    ASSERT(pszName || pszPath);

    if(pnNameCount)
    {
        *pnNameCount = 0;
    }

    if(pnPathCount)
    {
        *pnPathCount = 0;
    }

    if(pszName)
    {
        if(!_tcsicmp(pszName, WBCHEADER_ENTRYCOUNTSHORTNAME))
        {
            hr = E_FAIL;
            
            if(pnNameCount)
            {
                *pnNameCount += 1;
            }
        }
    }

    for(pleEntry = m_pParent->m_lstEntries.Flink; pleEntry != &m_pParent->m_lstEntries; pleEntry = pleEntry->Flink)
    {
        if(FAILED(hr) && !pnNameCount && !pnPathCount)
        {
            break;
        }
        
        if(this == (pEntry = CONTAINING_RECORD(pleEntry, CWaveBankEntry, m_leEntry)))
        {
            continue;
        }

        if(pszName)
        {
            if(!_tcsicmp(pszName, pEntry->m_szHeaderName))
            {
                hr = E_FAIL;
                
                if(pnNameCount)
                {
                    *pnNameCount += 1;
                }
            }
        }

        if(pszPath)
        {
            if(!_tcsicmp(pszPath, pEntry->m_szFileName))
            {
                hr = E_FAIL;
                
                if(pnPathCount)
                {
                    *pnPathCount += 1;
                }
            }
        }
    }
    
    DPF_LEAVE_HRESULT(hr);

    return hr;
}


/****************************************************************************
 *
 *  SetName
 *
 *  Description:
 *      Renames the entry.
 *
 *  Arguments:
 *      LPCTSTR [in]: entry name.
 *
 *  Returns:  
 *      HRESULT: COM result code.
 *
 ****************************************************************************/
 
#undef DPF_FNAME
#define DPF_FNAME "CWaveBankEntry::SetName"

HRESULT 
CWaveBankEntry::SetName
(
    LPCTSTR                 pszName
)
{
    HRESULT                 hr                                      = S_OK;
    TCHAR                   szHeaderName[NUMELMS(m_szHeaderName)];

    if(_tcslen(pszName) >= NUMELMS(m_szEntryName))
    {
        DPF_ERROR("That name is too long");
        hr = E_FAIL;
    }

    if(SUCCEEDED(hr))
    {
        _tcsncpy(szHeaderName, pszName, NUMELMS(szHeaderName));
        MakeHeaderString(szHeaderName);

        hr = FindDuplicateEntry(szHeaderName, NULL, NULL, NULL);

        if(SUCCEEDED(hr))
        {
            _tcsncpy(m_szEntryName, pszName, NUMELMS(m_szEntryName));
            _tcsncpy(m_szHeaderName, szHeaderName, NUMELMS(m_szHeaderName));
        }
        else
        {
            DPF_ERROR("An entry with that name already exists in the bank, or that name has been reserved");
        }
    }

    return hr;
}


/****************************************************************************
 *
 *  Load
 *
 *  Description:
 *      Initializes the object from a wave file.
 *
 *  Arguments:
 *      BOOL: TRUE to force a reload.
 *
 *  Returns:  
 *      HRESULT: COM result code.
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CWaveBankEntry::Load"

HRESULT
CWaveBankEntry::Load
(
    BOOL                    fForce
)
{
    HRESULT                 hr          = S_OK;
    CWaveFile               WaveFile;
    WAVEBANKUNIWAVEFORMAT   Format;
    
    DPF_ENTER();

    if(fForce || !m_fLoaded)
    {
        //
        // Open the file
        //

        if(FAILED(hr = WaveFile.Open(m_szFileName)))
        {
            DPF_ERROR("An error occurred while attempting to open %s", m_szFileName);
        }

        //
        // Load the file format
        //

        if(SUCCEEDED(hr))
        {
            if(FAILED(hr = WaveFile.GetFileType(&m_dwFileType)))
            {
                DPF_ERROR("An error occurred while attempting to retrieve the file type");
            }
        }

        if(SUCCEEDED(hr))
        {
            if(FAILED(hr = WaveFile.GetFormat(&Format.WaveFormatEx, sizeof(Format))))
            {
                DPF_ERROR("An error occurred while attempting to retrieve the file format");
            }
        }

        if(SUCCEEDED(hr))
        {
            if(!WaveBankCompressFormat(&Format, &m_Format))
            {
                hr = E_NOTIMPL;
            }
        }

        //
        // Save the data chunk offset and length
        //

        if(SUCCEEDED(hr))
        {
            if(FAILED(hr = WaveFile.GetDataOffset(&m_PlayRegion.dwStart)))
            {
                DPF_ERROR("An error occurred while attempting to retrieve the wave data offset");
            }
        }
    
        if(SUCCEEDED(hr))
        {
            if(FAILED(hr = WaveFile.GetDuration(&m_PlayRegion.dwLength)))
            {
                DPF_ERROR("An error occurred while attempting to retrieve the wave data length");
            }
        }

        //
        // Save the loop region
        //

        if(SUCCEEDED(hr))
        {
            hr = WaveFile.GetLoopRegion(&m_LoopRegion.dwStart, &m_LoopRegion.dwLength);

            if(FAILED(hr))
            {
                m_LoopRegion.dwStart = 0;
                m_LoopRegion.dwLength = 0;
            
                hr = S_OK;
            }
        }
    
        //
        // Update the flags to account for having valid data
        //
    
        if(SUCCEEDED(hr))
        {
            SetFlags(~0UL, m_dwFlags);
        }

        //
        // Success
        //

        if(SUCCEEDED(hr))
        {
            m_fLoaded = TRUE;
        }
    }

    DPF_LEAVE_HRESULT(hr);

    return hr;
}


/****************************************************************************
 *
 *  GetValidFlags
 *
 *  Description:
 *      Gets the mask of flags available for the entry.
 *
 *  Arguments:
 *      (void)
 *
 *  Returns:  
 *      DWORD: valid flags mask.
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CWaveBankEntry::GetValidFlags"

DWORD
CWaveBankEntry::GetValidFlags
(
    void
)
{
    DWORD                   dwFlags = 0;
    
    DPF_ENTER();

    if(CXboxAdpcmFilter::IsValidFormat(&m_Format))
    {
        dwFlags |= WBFILTER_ADPCM;
    }

    if(C8BitFilter::IsValidFormat(&m_Format))
    {
        dwFlags |= WBFILTER_8BIT;
    }

    DPF_LEAVE(dwFlags);

    return dwFlags;
}


/****************************************************************************
 *
 *  SetFlags
 *
 *  Description:
 *      Sets entry flags.  Because of error-checking, the flags actually set
 *      by this method may not match those passed in.
 *
 *  Arguments:
 *      DWORD [in]: mask of flags to change.
 *      DWORD [in]: new flags.
 *
 *  Returns:  
 *      DWORD: new flags.
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CWaveBankEntry::SetFlags"

DWORD
CWaveBankEntry::SetFlags
(
    DWORD                   dwMask,
    DWORD                   dwFlags
)
{
    DPF_ENTER();

    //
    // Check the mask against what the filters can actually deal with
    //

    dwMask &= GetValidFlags();

    //
    // Mask off any unused bits
    //
    
    dwFlags &= dwMask;
    
    //
    // ADPCM and 8BIT are mutually exclusive
    //

    if((dwFlags & WBFILTER_ADPCM) && (dwFlags & WBFILTER_8BIT))
    {
        DPF_WARNING("ADPCM and 8BIT filtering are mutually exclusive");
        dwFlags &= ~WBFILTER_8BIT;
    }

    //
    // Set the flags
    //

    m_dwFlags &= ~dwMask;
    m_dwFlags |= dwFlags;

    //
    // Recalculate the size the entry will take in the wave bank, including
    // alignment
    //

    m_dwDestLength = m_PlayRegion.dwLength;

    if(m_dwFlags & WBFILTER_ADPCM)
    {
        m_dwDestLength = CXboxAdpcmFilter::GetSize(m_dwDestLength, m_Format.nChannels, TRUE);
    }
    else if(m_dwFlags & WBFILTER_8BIT)
    {
        m_dwDestLength = C8BitFilter::GetSize(m_dwDestLength, m_Format.nChannels);
    }

    DPF_LEAVE(dwFlags);

    return dwFlags;
}


/****************************************************************************
 *
 *  GetMetaData
 *
 *  Description:
 *      Generates entry meta-data.
 *
 *  Arguments:
 *      LPWAVEBANKENTRY [out]: entry meta-data.
 *      DWORD [in]: base file offset of the data segment.
 *      DWORD [in]: entry alignment.
 *      LPDWORD [in/out]: current offset into the data segment.
 *
 *  Returns:  
 *      HRESULT: COM result code.
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CWaveBankEntry::GetMetaData"

HRESULT
CWaveBankEntry::GetMetaData
(
    LPWAVEBANKENTRY         pMetaData,
    DWORD                   dwBaseOffset,
    DWORD                   dwAlignment,
    LPDWORD                 pdwOffset
)
{
    BOOL                    fSuccess;
    
    DPF_ENTER();

    ASSERT(!((m_dwFlags & WBFILTER_ADPCM) && (m_dwFlags & WBFILTER_8BIT)));
    ASSERT(m_fLoaded);

    //
    // Align the starting offset
    //

    *pdwOffset = BLOCKALIGNPAD(dwBaseOffset + *pdwOffset, dwAlignment) - dwBaseOffset;

    //
    // If we're compressing, write the compressed format, not the source
    //

    pMetaData->Format = m_Format;

    if(m_dwFlags & WBFILTER_ADPCM)
    {
        fSuccess = CXboxAdpcmFilter::ConvertFormat(&pMetaData->Format);
        ASSERT(fSuccess);
    }
    else if(m_dwFlags & WBFILTER_8BIT)
    {
        fSuccess = C8BitFilter::ConvertFormat(&pMetaData->Format);
        ASSERT(fSuccess);
    }

    //
    // Set the entry starting offset and length
    //

    pMetaData->PlayRegion.dwStart = *pdwOffset;
    pMetaData->PlayRegion.dwLength = m_dwDestLength;

    //
    // Convert the loop offsets
    //

    pMetaData->LoopRegion = m_LoopRegion;

    if(m_dwFlags & WBFILTER_ADPCM)
    {
        pMetaData->LoopRegion.dwStart = CXboxAdpcmFilter::GetSize(pMetaData->LoopRegion.dwStart, m_Format.nChannels);
        pMetaData->LoopRegion.dwLength = CXboxAdpcmFilter::GetSize(pMetaData->LoopRegion.dwLength, m_Format.nChannels);
    }
    else if(m_dwFlags & WBFILTER_8BIT)
    {
        pMetaData->LoopRegion.dwStart = C8BitFilter::GetSize(pMetaData->LoopRegion.dwStart, m_Format.nChannels);
        pMetaData->LoopRegion.dwLength = C8BitFilter::GetSize(pMetaData->LoopRegion.dwLength, m_Format.nChannels);
    }

    //
    // Update the file offset
    //

    *pdwOffset = pMetaData->PlayRegion.dwStart + pMetaData->PlayRegion.dwLength;

    DPF_LEAVE_HRESULT(S_OK);

    return S_OK;
}


/****************************************************************************
 *
 *  CommitWaveData
 *
 *  Description:
 *      Writes entry wave data to the bank file.
 *
 *  Arguments:
 *      CWaveBankCallback * [in]: callback object.
 *      CStdFileStream * [in]: bank file stream.
 *      DWORD [in]: base file offset of the data segment.
 *      LPCWAVEBANKENTRY [in]: entry meta-data generated by GetMetaData.
 *      LPVOID [in]: scratch memory.
 *      DWORD [in]: size of scratch buffer.
 *
 *  Returns:  
 *      HRESULT: COM result code.
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CWaveBankEntry::CommitWaveData"

HRESULT
CWaveBankEntry::CommitWaveData
(
    CWaveBankCallback *     pCallback,
    CStdFileStream *        pBankFile,
    DWORD                   dwBaseOffset,
    LPCWAVEBANKENTRY        pMetaData,
    LPVOID                  pvCopyBuffer,
    DWORD                   dwCopyBufferSize
)
{
    DWORD                   dwTotalRead     = 0;
    DWORD                   dwTotalWritten  = 0;
    HRESULT                 hr              = S_OK;
    CXboxAdpcmFilter        AdpcmFilter;
    C8BitFilter             BitFilter;
    CStdFileStream          WaveFile;
    DWORD                   dwAlignment;
    DWORD                   dwRead;
    DWORD                   dwWritten;
    DWORD                   dwAlignedRead;
    DWORD                   dwFilterRead;
    DWORD                   dwFileType;
    
    DPF_ENTER();

    ASSERT(!((m_dwFlags & WBFILTER_ADPCM) && (m_dwFlags & WBFILTER_8BIT)));
    ASSERT(m_fLoaded);
    
    if(!pBankFile)
    {
        ASSERT(dwCopyBufferSize >= pMetaData->PlayRegion.dwLength);
    }
    
    //
    // Call the callback
    //

    if(pCallback)
    {
        if(!pCallback->BeginEntry(this))
        {
            hr = E_ABORT;
        }
    }

    //
    // Open the source file and seek to the start of the data chunk
    //

    if(SUCCEEDED(hr))
    {
        if(FAILED(hr = WaveFile.Open(m_szFileName, GENERIC_READ, FILE_SHARE_READ, OPEN_EXISTING, FILE_FLAG_SEQUENTIAL_SCAN)))
        {
            DPF_ERROR("An error occurred while attempting to open %s", m_szFileName);
        }
    }

    if(SUCCEEDED(hr))
    {
        if(FAILED(hr = WaveFile.Seek(m_PlayRegion.dwStart, FILE_BEGIN)))
        {
            DPF_ERROR("An error occurred while attempting to read from the wave file");
        }
    }

    //
    // Seek to the proper location in the destination file
    //

    if(SUCCEEDED(hr))
    {
        if(FAILED(hr = pBankFile->Seek(dwBaseOffset + pMetaData->PlayRegion.dwStart, FILE_BEGIN)))
        {
            DPF_ERROR("An error occurred while attempting to read from the bank file");
        }
    }

    //
    // Initialize the ADPCM filter
    //

    if(SUCCEEDED(hr) && (m_dwFlags & WBFILTER_ADPCM))
    {
        if(!AdpcmFilter.Initialize(&m_Format))
        {
            DPF_ERROR("Failed to initialize the ADPCM filter");
            hr = E_FAIL;
        }
    }

    //
    // Initialize the 8BIT filter
    //

    if(SUCCEEDED(hr) && (m_dwFlags & WBFILTER_8BIT))
    {
        if(!BitFilter.Initialize(&m_Format))
        {
            DPF_ERROR("Failed to initialize the 8BIT filter");
            hr = E_FAIL;
        }
    }

    //
    // Get the filter alignment so we never have to seek backwards in the
    // file due to converting less data than we read.
    //

    if(SUCCEEDED(hr))
    {
        if(m_dwFlags & WBFILTER_ADPCM)
        {
            dwAlignment = AdpcmFilter.GetSourceAlignment();
        }
        else if(m_dwFlags & WBFILTER_8BIT)
        {
            dwAlignment = BitFilter.GetSourceAlignment();
        }
        else
        {
            dwAlignment = m_Format.nChannels;
            
            if(WAVEBANKMINIFORMAT_BITDEPTH_16 == m_Format.wBitsPerSample)
            {
                dwAlignment *= 2;
            }
        }

        ASSERT(dwCopyBufferSize >= dwAlignment);
    }

    //
    // Start copying data
    //

    while(SUCCEEDED(hr) && (dwTotalRead < m_PlayRegion.dwLength))
    {
        //
        // Don't read past the end of the file
        //

        dwRead = min(dwCopyBufferSize, m_PlayRegion.dwLength - dwTotalRead);

        //
        // Align the read amount so we never have to seek backwards in the
        // file due to converting less data than we read.  If there isn't
        // enough data left in the file to satisfy the alignment, pad with
        // silence.
        //

        if(dwRead >= dwAlignment)
        {
            dwRead = dwAlignedRead = BLOCKALIGN(dwRead, dwAlignment);
        }
        else
        {
            dwAlignedRead = BLOCKALIGNPAD(dwRead, dwAlignment);
        }

        dwWritten = dwAlignedRead;

        //
        // Read source data
        //
        
        hr = WaveFile.Read(pvCopyBuffer, dwRead);

        //
        // Convert if AIFF PCM
        //

        if(SUCCEEDED(hr) && (WAVELDR_FILETYPE_AIFF == m_dwFileType))
        {
            CWaveFile::ConvertAiffPcm(pvCopyBuffer, dwRead, (WAVEBANKMINIFORMAT_BITDEPTH_16 == m_Format.wBitsPerSample) ? 16 : 8);
        }

        //
        // Pad with silence if necessary
        //

        if(SUCCEEDED(hr) && (dwAlignedRead > dwRead))
        {
            ZeroMemory((LPBYTE)pvCopyBuffer + dwRead, dwAlignedRead - dwRead);
        }
        
        //
        // Convert (or not)
        //

        if(SUCCEEDED(hr) && (m_dwFlags & WBFILTER_ADPCM))
        {
            dwFilterRead = dwAlignedRead;
            
            if(AdpcmFilter.Convert(pvCopyBuffer, &dwFilterRead, pvCopyBuffer, &dwWritten))
            {
                ASSERT(dwFilterRead == dwAlignedRead);
            }
            else
            {
                DPF_ERROR("An error occurred while compressing PCM data");
                hr = E_FAIL;
            }
        }
            
        if(SUCCEEDED(hr) && (m_dwFlags & WBFILTER_8BIT))
        {
            dwFilterRead = dwAlignedRead;
            
            if(BitFilter.Convert(pvCopyBuffer, &dwFilterRead, pvCopyBuffer, &dwWritten))
            {
                ASSERT(dwFilterRead == dwAlignedRead);
            }
            else
            {
                DPF_ERROR("An error occurred while compressing PCM data");
                hr = E_FAIL;
            }
        }

        //
        // Write to the bank file
        //
        
        if(SUCCEEDED(hr))
        {
            if(FAILED(hr = pBankFile->Write(pvCopyBuffer, dwWritten)))
            {
                DPF_ERROR("An error occurred while attempting to write to the bank file");
            }
        }

        //
        // Increment counters
        //

        if(SUCCEEDED(hr))
        {
            dwTotalRead += dwRead;
            dwTotalWritten += dwWritten;
        }

        //
        // If no file was specified, bail out now
        //

        if(!pBankFile)
        {
            break;
        }

        //
        // Call the callback
        //

        if(pCallback)
        {
            if(!pCallback->ProcessEntry(this, dwTotalRead))
            {
                hr = E_ABORT;
            }
        }
    }

    if(SUCCEEDED(hr))
    {
        ASSERT(dwTotalRead == m_PlayRegion.dwLength);
        ASSERT(dwTotalWritten == pMetaData->PlayRegion.dwLength);
    }

    //
    // Call the callback
    //

    if(pCallback)
    {
        pCallback->EndEntry(this);
    }

    DPF_LEAVE_HRESULT(hr);

    return hr;
}


/****************************************************************************
 *
 *  CWaveBank
 *
 *  Description:
 *      Object constructor.
 *
 *  Arguments:
 *      (void)
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CWaveBank::CWaveBank"

CWaveBank::CWaveBank
(
    CWaveBankProject *      pParent
)
{
    DPF_ENTER();

    m_pParent = pParent;
    m_dwEntryCount = 0;
    m_dwAlignment = m_dwMinAlignment;

    InitializeListHead(&m_lstEntries);

    if(m_pParent)
    {
        InsertTailListUninit(&m_pParent->m_lstBanks, &m_leBank);

        m_pParent->m_dwBankCount++;
    }
    else
    {
        InitializeListHead(&m_leBank);
    }

    DPF_LEAVE_VOID();
}


/****************************************************************************
 *
 *  ~CWaveBank
 *
 *  Description:
 *      Object destructor.
 *
 *  Arguments:
 *      (void)
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CWaveBank::~CWaveBank"

CWaveBank::~CWaveBank
(
    void
)
{
    PLIST_ENTRY             pleEntry;
    CWaveBankEntry *        pEntry;
    
    DPF_ENTER();

    //
    // Free all the entries
    //
    
    while((pleEntry = m_lstEntries.Flink) != &m_lstEntries)
    {
        pEntry = CONTAINING_RECORD(pleEntry, CWaveBankEntry, m_leEntry);
        
        DELETE(pEntry);
    }

    //
    // Free ourselves from the parent list
    //

    RemoveEntryList(&m_leBank);

    if(m_pParent)
    {
        ASSERT(m_pParent->m_dwBankCount);
        m_pParent->m_dwBankCount--;
    }

    DPF_LEAVE_VOID();
}


/****************************************************************************
 *
 *  Initialize
 *
 *  Description:
 *      Initializes the object.
 *
 *  Arguments:
 *      LPCTSTR [in]: wave bank name.
 *      LPCTSTR [in]: bank file path.
 *      LPCTSTR [in]: header file path.
 *
 *  Returns:  
 *      HRESULT: COM result code.
 *
 ****************************************************************************/
 
#undef DPF_FNAME
#define DPF_FNAME "CWaveBank::Initialize"

HRESULT
CWaveBank::Initialize
(
    LPCTSTR                 pszBankName,
    LPCTSTR                 pszBankFile,
    LPCTSTR                 pszHeaderFile
)
{
    HRESULT                 hr                  = S_OK;
    UINT                    nNameCount;
    UINT                    nBankFileCount;
    UINT                    nHeaderFileCount;

    DPF_ENTER();

    _tcsncpy(m_szBankName, pszBankName, NUMELMS(m_szBankName));
    _tcsncpy(m_szHeaderName, pszBankName, NUMELMS(m_szHeaderName));
    MakeHeaderString(m_szHeaderName);

    GetFullPathName(pszBankFile, NUMELMS(m_szBankFile), m_szBankFile, NULL);
    GetFullPathName(pszHeaderFile, NUMELMS(m_szHeaderFile), m_szHeaderFile, NULL);

    FindDuplicateBank(m_szHeaderName, &nNameCount, m_szBankFile, &nBankFileCount, m_szHeaderFile, &nHeaderFileCount);

    if(nNameCount)
    {
        DPF_ERROR("A bank with that name (or a similar one) already exists in the project");
        hr = E_FAIL;
    }
    else if(nBankFileCount)
    {
        DPF_ERROR("A bank using that filename already exists in the project");
        hr = E_FAIL;
    }
    else if(nHeaderFileCount)
    {
        DPF_ERROR("A bank using that header filename already exists in the project");
        hr = E_FAIL;
    }

    DPF_LEAVE_HRESULT(hr);

    return hr;
}


/****************************************************************************
 *
 *  FindDuplicateBank
 *
 *  Description:
 *      Checks the wave bank for a duplicate Bank.
 *
 *  Arguments:
 *      LPCTSTR [in]: Bank name.
 *      UINT * [out]: count of duplicate named entries found.
 *      LPCTSTR [in]: Bank file path.
 *      UINT * [out]: count of duplicate pathed entries found.
 *
 *  Returns:  
 *      HRESULT: COM result code.
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CWaveBank::FindDuplicateBank"

HRESULT
CWaveBank::FindDuplicateBank
(
    LPCTSTR                 pszName,
    UINT *                  pnNameCount,
    LPCTSTR                 pszBankFile,
    UINT *                  pnBankFileCount,
    LPCTSTR                 pszHeaderFile,
    UINT *                  pnHeaderFileCount
)
{
    HRESULT                 hr          = S_OK;
    PLIST_ENTRY             pleBank;
    CWaveBank *             pBank;
    
    DPF_ENTER();

    ASSERT(pszName || pszBankFile);

    if(pnNameCount)
    {
        *pnNameCount = 0;
    }

    if(pnBankFileCount)
    {
        *pnBankFileCount = 0;
    }

    if(pnHeaderFileCount)
    {
        *pnHeaderFileCount = 0;
    }

    for(pleBank = m_pParent->m_lstBanks.Flink; pleBank != &m_pParent->m_lstBanks; pleBank = pleBank->Flink)
    {
        if(this == (pBank = CONTAINING_RECORD(pleBank, CWaveBank, m_leBank)))
        {
            continue;
        }

        if(pszName)
        {
            if(!_tcsicmp(pszName, pBank->m_szHeaderName))
            {
                hr = E_FAIL;
                
                if(pnNameCount)
                {
                    *pnNameCount = *pnNameCount + 1;
                }
                else if(!pnBankFileCount)
                {
                    break;
                }
            }
        }

        if(pszBankFile)
        {
            if(!_tcsicmp(pszBankFile, pBank->m_szBankFile))
            {
                hr = E_FAIL;
                
                if(pnBankFileCount)
                {
                    *pnBankFileCount = *pnBankFileCount + 1;
                }
                else if(!pnNameCount)
                {
                    break;
                }
            }
        }

        if(pszHeaderFile)
        {
            if(!_tcsicmp(pszHeaderFile, pBank->m_szHeaderFile))
            {
                hr = E_FAIL;
                
                if(pnHeaderFileCount)
                {
                    *pnHeaderFileCount = *pnHeaderFileCount + 1;
                }
                else if(!pnNameCount)
                {
                    break;
                }
            }
        }
    }
    
    DPF_LEAVE_HRESULT(hr);

    return hr;
}


/****************************************************************************
 *
 *  SetName
 *
 *  Description:
 *      Renames the Bank.
 *
 *  Arguments:
 *      LPCTSTR [in]: Bank name.
 *
 *  Returns:  
 *      HRESULT: COM result code.
 *
 ****************************************************************************/
 
#undef DPF_FNAME
#define DPF_FNAME "SetName"

HRESULT 
CWaveBank::SetName
(
    LPCTSTR                  pszName
)
{
    HRESULT                 hr                                      = S_OK;
    TCHAR                   szHeaderName[NUMELMS(m_szHeaderName)];

    if(_tcslen(pszName) >= NUMELMS(m_szBankName))
    {
        DPF_ERROR("That name is too long");
        hr = E_FAIL;
    }

    if(SUCCEEDED(hr))
    {
        _tcsncpy(szHeaderName, pszName, NUMELMS(szHeaderName));
        MakeHeaderString(szHeaderName);

        hr = FindDuplicateBank(szHeaderName, NULL, NULL, NULL, NULL, NULL);

        if(SUCCEEDED(hr))
        {
            _tcsncpy(m_szBankName, pszName, NUMELMS(m_szBankName));
            _tcsncpy(m_szHeaderName, szHeaderName, NUMELMS(m_szHeaderName));
        }
        else
        {
            DPF_ERROR("A bank with that name (or a similar one) already exists in the project");
        }
    }

    return hr;
}


/****************************************************************************
 *
 *  SetAlignment
 *
 *  Description:
 *      Sets entry data alignment.
 *
 *  Arguments:
 *      DWORD [in]: alignment.
 *
 *  Returns:  
 *      HRESULT: COM result code.
 *
 ****************************************************************************/
 
#undef DPF_FNAME
#define DPF_FNAME "SetAlignment"

HRESULT 
CWaveBank::SetAlignment
(
    DWORD                   dwAlignment
)
{
    HRESULT                 hr  = S_OK;
    
    if(dwAlignment % m_dwMinAlignment)
    {
        DPF_ERROR("Alignment must be a multiple of %lu", m_dwMinAlignment);
        hr = E_INVALIDARG;
    }

    if(SUCCEEDED(hr))
    {
        m_dwAlignment = dwAlignment;
    }

    return hr;
}


/****************************************************************************
 *
 *  AddEntry
 *
 *  Description:
 *      Adds an entry to the wave bank.
 *
 *  Arguments:
 *      LPCTSTR [in]: entry name.
 *      LPCTSTR [in]: wave file name.
 *      DWORD [in]: entry flags.
 *      CWaveBankEntry ** [out]: entry object.
 *
 *  Returns:  
 *      HRESULT: COM result code.
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CWaveBank::AddEntry"

HRESULT
CWaveBank::AddEntry
(
    LPCTSTR                 pszEntryName,
    LPCTSTR                 pszFileName,
    DWORD                   dwFlags,
    CWaveBankEntry **       ppEntry
)
{
    CWaveBankEntry *        pEntry;
    HRESULT                 hr;
    
    DPF_ENTER();

    hr = HRFROMP(pEntry = CreateEntry());

    if(SUCCEEDED(hr))
    {
        hr = pEntry->Initialize(pszEntryName, pszFileName, dwFlags);
    }

    if(SUCCEEDED(hr))
    {
        if(ppEntry)
        {
            *ppEntry = pEntry;
        }
    }
    else
    {
        DELETE(pEntry);
    }

    return hr;
}


/****************************************************************************
 *
 *  GenerateBank
 *
 *  Description:
 *      Saves files to the wave bank.
 *
 *  Arguments:
 *      CWaveBankCallback * [in]: callback object.
 *      BOOL [in]: TRUE to allow overwriting the wave bank file.
 *
 *  Returns:  
 *      HRESULT: COM result code.
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CWaveBank::GenerateBank"

HRESULT
CWaveBank::GenerateBank
(
    CWaveBankCallback *     pCallback,
    BOOL                    fAllowOverwrite
)                                   
{                                   
    static const DWORD      dwCopyBufferSize    = 256 * 1024;
    LPVOID                  pvCopyBuffer        = NULL;
    LPVOID                  pvHeader            = NULL;
    HRESULT                 hr                  = S_OK;
    CStdFileStream          BankFile;
    LPWAVEBANKHEADER        pHeader;
    LPWAVEBANKENTRY         paMetaData;
    PLIST_ENTRY             pleEntry;
    CWaveBankEntry *        pEntry;
    DWORD                   dwEntryIndex;
    DWORD                   dwHeaderSize;
    DWORD                   dwOffset;
    
    DPF_ENTER();

    if(m_dwEntryCount)
    {
        //
        // Call the callback
        //

        if(pCallback)
        {
            if(!pCallback->OpenBank(this))
            {
                hr = E_ABORT;
            }
        }

        //
        // Open the bank file
        //

        if(SUCCEEDED(hr))
        {
            if(FAILED(hr = BankFile.Open(m_szBankFile, GENERIC_READ | GENERIC_WRITE, 0, fAllowOverwrite ? CREATE_ALWAYS : CREATE_NEW, FILE_FLAG_SEQUENTIAL_SCAN)))
            {
                DPF_ERROR("An error occurred while attempting to open %s", m_szBankFile);
            }
        }
        
        //
        // Allocate temporary storage
        //

        if(SUCCEEDED(hr))
        {
            if(FAILED(hr = HRFROMP(pvCopyBuffer = MEMALLOC_NOINIT(BYTE, dwCopyBufferSize))))
            {
                DPF_ERROR("Out of memory");
            }
        }

        if(SUCCEEDED(hr))
        {
            dwHeaderSize = GetBankHeaderSize();
            
            if(FAILED(hr = HRFROMP(pvHeader = MEMALLOC(BYTE, dwHeaderSize))))
            {
                DPF_ERROR("Out of memory");
            }
        }

        if(SUCCEEDED(hr))
        {
            pHeader = (LPWAVEBANKHEADER)pvHeader;
            paMetaData = (LPWAVEBANKENTRY)(pHeader + 1);
        }

        //
        // Create the file header
        //

        if(SUCCEEDED(hr))
        {
            pHeader->dwSignature = WAVEBANKHEADER_SIGNATURE;
            pHeader->dwVersion = WAVEBANKHEADER_VERSION;
            pHeader->dwFlags = m_dwFlags;
            pHeader->dwEntryCount = m_dwEntryCount;
            pHeader->dwAlignment = m_dwAlignment;
            
            _tcsncpy(pHeader->szBankName, m_szBankName, NUMELMS(pHeader->szBankName));
        }

        //
        // Generate metadata
        //

        if(SUCCEEDED(hr))
        {
            for(pleEntry = m_lstEntries.Flink, dwEntryIndex = 0, dwOffset = 0; SUCCEEDED(hr) && (pleEntry != &m_lstEntries); pleEntry = pleEntry->Flink, dwEntryIndex++)
            {
                ASSERT(dwEntryIndex < m_dwEntryCount);
                
                pEntry = CONTAINING_RECORD(pleEntry, CWaveBankEntry, m_leEntry);

                hr = pEntry->Load(TRUE);

                if(SUCCEEDED(hr))
                {
                    hr = pEntry->GetMetaData(&paMetaData[dwEntryIndex], dwHeaderSize, m_dwAlignment, &dwOffset);
                }
            }
        }

        //
        // Write the file header
        //
        
        if(SUCCEEDED(hr))
        {
            if(FAILED(hr = BankFile.Write(pvHeader, dwHeaderSize)))
            {
                DPF_ERROR("An error occurred while attempting to write to the bank file");
            }
        }

        //
        // Write the data section
        //

        if(SUCCEEDED(hr))
        {
            for(pleEntry = m_lstEntries.Flink, dwEntryIndex = 0; SUCCEEDED(hr) && (pleEntry != &m_lstEntries); pleEntry = pleEntry->Flink, dwEntryIndex++)
            {
                ASSERT(dwEntryIndex < m_dwEntryCount);
                
                pEntry = CONTAINING_RECORD(pleEntry, CWaveBankEntry, m_leEntry);

                if(SUCCEEDED(hr))
                {
                    hr = pEntry->CommitWaveData(pCallback, &BankFile, dwHeaderSize, &paMetaData[dwEntryIndex], pvCopyBuffer, dwCopyBufferSize);
                }
            }
        }

        //
        // Call the callback
        //

        if(pCallback)
        {
            pCallback->CloseBank(this);
        }
    }
    else
    {
        DPF_ERROR("No entries added yet");
        hr = E_FAIL;
    }

    //
    // Clean up
    //

    MEMFREE(pvCopyBuffer);
    MEMFREE(pvHeader);

    DPF_LEAVE_HRESULT(hr);

    return hr;
}


/****************************************************************************
 *
 *  GenerateHeader
 *
 *  Description:
 *      Generates the C header to use with the wave bank.
 *
 *  Arguments:
 *      (void)
 *
 *  Returns:  
 *      HRESULT: COM result code.
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CWaveBank::GenerateHeader"

HRESULT
CWaveBank::GenerateHeader
(
    CWaveBankCallback *     pCallback,
    BOOL                    fAllowOverwrite
)                                   
{                                   
    static const LPCTSTR    pszFileBegin            = TEXT("#pragma once\r\n\r\n");
    static const LPCTSTR    pszEnumBegin            = TEXT("typedef enum\r\n{\r\n");
    static const LPCTSTR    pszEnumEnd              = TEXT("} " WBCHEADER_ENUMNAME ";\r\n\r\n");
    static const LPCTSTR    pszEnumEntry            = TEXT("    ") WBCHEADER_ENUMENTRYNAME TEXT(" = %lu,\r\n");
    static const LPCTSTR    pszEnumCount            = TEXT("#define ") WBCHEADER_ENTRYCOUNTNAME TEXT(" %lu\r\n");
    HRESULT                 hr                      = S_OK;
    CStdFileStream          HeaderFile;
    TCHAR                   szText[MAX_PATH];
    PLIST_ENTRY             pleEntry;
    CWaveBankEntry *        pEntry;
    DWORD                   dwEntry;
    
    DPF_ENTER();

    if(m_dwEntryCount)
    {
        //
        // Call the callback
        //

        if(pCallback)
        {
            if(!pCallback->OpenHeader(this))
            {
                hr = E_ABORT;
            }
        }

        //
        // Open the bank file
        //

        if(SUCCEEDED(hr))
        {
            if(FAILED(hr = HeaderFile.Open(m_szHeaderFile, GENERIC_READ | GENERIC_WRITE, 0, fAllowOverwrite ? CREATE_ALWAYS : CREATE_NEW, FILE_FLAG_SEQUENTIAL_SCAN)))
            {
                DPF_ERROR("An error occurred while attempting to open %s", m_szHeaderFile);
            }
        }

        //
        // Write the file header
        //

        if(SUCCEEDED(hr))
        {
            if(FAILED(hr = HeaderFile.Write(pszFileBegin, _tcslen(pszFileBegin))))
            {
                DPF_ERROR("An error occurred while attempting to write to the header file");
            }
        }

        //
        // Write the enumeration header
        //

        if(SUCCEEDED(hr))
        {
            if(FAILED(hr = HeaderFile.Write(pszEnumBegin, _tcslen(pszEnumBegin))))
            {
                DPF_ERROR("An error occurred while attempting to write to the header file");
            }
        }

        //
        // Begin the entry loop
        //

        if(SUCCEEDED(hr))
        {
            for(pleEntry = m_lstEntries.Flink, dwEntry = 0; (pleEntry != &m_lstEntries) && SUCCEEDED(hr); pleEntry = pleEntry->Flink, dwEntry++)
            {
                pEntry = CONTAINING_RECORD(pleEntry, CWaveBankEntry, m_leEntry);

                //
                // Add the entry to the enumeration
                //

                sprintf(szText, pszEnumEntry, m_szHeaderName, pEntry->m_szHeaderName, dwEntry);

                if(FAILED(hr = HeaderFile.Write(szText, _tcslen(szText))))
                {
                    DPF_ERROR("An error occurred while attempting to write to the header file");
                }
            }
        }

        //
        // End the enumeration
        //

        if(SUCCEEDED(hr))
        {
            sprintf(szText, pszEnumEnd, m_szHeaderName);

            if(FAILED(hr = HeaderFile.Write(szText, _tcslen(szText))))
            {
                DPF_ERROR("An error occurred while attempting to write to the header file");
            }
        }

        //
        // Add a count
        //

        if(SUCCEEDED(hr))
        {
            ASSERT(dwEntry == m_dwEntryCount);
            
            sprintf(szText, pszEnumCount, m_szHeaderName, dwEntry);

            if(FAILED(hr = HeaderFile.Write(szText, _tcslen(szText))))
            {
                DPF_ERROR("An error occurred while attempting to write to the header file");
            }
        }

        //
        // Call the callback
        //

        if(pCallback)
        {
            pCallback->CloseHeader(this);
        }
    }
    else
    {
        DPF_ERROR("No entries added yet");
        hr = E_FAIL;
    }

    DPF_LEAVE_HRESULT(hr);

    return hr;
}


/****************************************************************************
 *
 *  GetBankDataSize
 *
 *  Description:
 *      Calculates the size of the wave bank's data segment, in bytes.
 *
 *  Arguments:
 *      (void)
 *
 *  Returns:  
 *      DWORD: bank data size.
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CWaveBank::GetBankDataSize"

DWORD
CWaveBank::GetBankDataSize
(
    void
)                                   
{                                   
    PLIST_ENTRY             pleEntry;
    CWaveBankEntry *        pEntry;
    DWORD                   dwHeaderSize;
    DWORD                   dwSize;
    HRESULT                 hr;
    
    DPF_ENTER();

    //
    // Calculate the total size of the bank's data segment based on the
    // total size of all the entries in the bank.  We're aligning the size
    // so that it matches the aligned offsets that will appear in the bank
    // file.
    //

    dwHeaderSize = GetBankHeaderSize();

    for(pleEntry = m_lstEntries.Flink, dwSize = 0, hr = S_OK; SUCCEEDED(hr) && (pleEntry != &m_lstEntries); pleEntry = pleEntry->Flink)
    {
        dwSize = BLOCKALIGNPAD(dwHeaderSize + dwSize, m_dwAlignment) - dwHeaderSize;

        pEntry = CONTAINING_RECORD(pleEntry, CWaveBankEntry, m_leEntry);

        if(!pEntry->m_fLoaded)
        {
            hr = E_ABORT;
            break;
        }
    }

    if(FAILED(hr))
    {
        dwSize = 0;
    }

    DPF_LEAVE(dwSize);

    return dwSize;
}


/****************************************************************************
 *
 *  LoadEntries
 *
 *  Description:
 *      Loads (or reloads) entry data.
 *
 *  Arguments:
 *      BOOL [in]: TRUE to force a reload.
 *
 *  Returns:  
 *      HRESULT: COM result code.
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CWaveBank::LoadEntries"

HRESULT
CWaveBank::LoadEntries
(
    BOOL                    fForce
)                                   
{                                   
    HRESULT                 hr          = S_OK;
    PLIST_ENTRY             pleEntry;
    CWaveBankEntry *        pEntry;
    
    DPF_ENTER();

    for(pleEntry = m_lstEntries.Flink; SUCCEEDED(hr) && (pleEntry != &m_lstEntries); pleEntry = pleEntry->Flink)
    {
        pEntry = CONTAINING_RECORD(pleEntry, CWaveBankEntry, m_leEntry);

        hr = pEntry->Load(fForce);
    }

    DPF_LEAVE_HRESULT(hr);

    return hr;
}


/****************************************************************************
 *
 *  CWaveBankProject
 *
 *  Description:
 *      Object constructor.
 *
 *  Arguments:
 *      (void)
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CWaveBankProject::CWaveBankProject"

CWaveBankProject::CWaveBankProject
(
    void
)
{
    DPF_ENTER();

    m_dwBankCount = 0;
    m_dwEntryCount = 0;

    InitializeListHead(&m_lstBanks);

    DPF_LEAVE_VOID();
}


/****************************************************************************
 *
 *  ~CWaveBankProject
 *
 *  Description:
 *      Object destructor.
 *
 *  Arguments:
 *      (void)
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CWaveBankProject::~CWaveBankProject"

CWaveBankProject::~CWaveBankProject
(
    void
)
{
    PLIST_ENTRY             pleBank;
    CWaveBank *             pBank;
    
    DPF_ENTER();

    //
    // Free all the entries
    //
    
    while((pleBank = m_lstBanks.Flink) != &m_lstBanks)
    {
        pBank = CONTAINING_RECORD(pleBank, CWaveBank, m_leBank);
        
        DELETE(pBank);
    }

    DPF_LEAVE_VOID();
}


/****************************************************************************
 *
 *  AddBank
 *
 *  Description:
 *      Adds a wave bank to the project.
 *
 *  Arguments:
 *      LPCTSTR [in]: wave bank name.
 *      LPCTSTR [in]: bank file path.
 *      LPCTSTR [in]: header file path.
 *      CWaveBank ** [out]: bank object.
 *
 *  Returns:  
 *      HRESULT: COM result code.
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CWaveBankProject::AddBank"

HRESULT
CWaveBankProject::AddBank
(
    LPCTSTR                 pszBankName, 
    LPCTSTR                 pszBankFile, 
    LPCTSTR                 pszHeaderFile,
    CWaveBank **            ppBank
)
{
    CWaveBank *             pBank;
    HRESULT                 hr;
    
    DPF_ENTER();

    hr = HRFROMP(pBank = CreateBank());

    if(SUCCEEDED(hr))
    {
        hr = pBank->Initialize(pszBankName, pszBankFile, pszHeaderFile);
    }

    if(SUCCEEDED(hr))
    {
        if(ppBank)
        {
            *ppBank = pBank;
        }
    }
    else
    {
        DELETE(pBank);
    }

    DPF_LEAVE_HRESULT(hr);

    return hr;
}


/****************************************************************************
 *
 *  Generate
 *
 *  Description:
 *      Generates wave bank files.
 *
 *  Arguments:
 *      CWaveBankCallback * [in]: callback object.
 *      BOOL [in]: TRUE to allow overwriting of files.
 *
 *  Returns:  
 *      HRESULT: COM result code.
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CWaveBankProject::Generate"

HRESULT
CWaveBankProject::Generate
(
    CWaveBankCallback *     pCallback,
    BOOL                    fAllowOverwrite
)
{
    HRESULT                 hr          = S_OK;
    PLIST_ENTRY             pleBank;
    PLIST_ENTRY             pleEntry;
    CWaveBank *             pBank;

    DPF_ENTER();

    if(pCallback)
    {
        if(!pCallback->BeginProject(this))
        {
            hr = E_ABORT;
        }
    }

    for(pleBank = m_lstBanks.Flink; (pleBank != &m_lstBanks) && SUCCEEDED(hr); pleBank = pleBank->Flink)
    {
        pBank = CONTAINING_RECORD(pleBank, CWaveBank, m_leBank);

        hr = pBank->GenerateBank(pCallback, fAllowOverwrite);

        if(SUCCEEDED(hr))
        {
            hr = pBank->GenerateHeader(pCallback, fAllowOverwrite);
        }
    }

    if(pCallback)
    {
        pCallback->EndProject(this, hr);
    }

    DPF_LEAVE_HRESULT(hr);

    return hr;
}


