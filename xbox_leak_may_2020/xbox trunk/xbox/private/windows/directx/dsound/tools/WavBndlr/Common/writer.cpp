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


/****************************************************************************
 *
 *  MakeHeaderString
 *
 *  Description:
 *      Removes all spaces from a string and replaces them with underscores.
 *
 *  Arguments:
 *      LPSTR [in/out]: string.
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "MakeHeaderString"

void
MakeHeaderString
(
    LPSTR                   pszString
)
{
    while(*pszString)
    {
        if((*pszString >= 'a') && (*pszString <= 'z'))
        {
            *pszString = 'A' + *pszString - 'a';
        }
        else if((*pszString >= 'A') && (*pszString <= 'Z'))
        {
        }
        else if((*pszString >= '0') && (*pszString <= '9'))
        {
        }
        else
        {
            *pszString = '_';
        }

        pszString++;
    }
}


/****************************************************************************
 *
 *  ExtractDirectory
 *
 *  Description:
 *      Extracts the first directory from a path.
 *
 *  Arguments:
 *      LPSTR [in]: path.
 *      LPSTR [out]: directory name.
 *
 *  Returns:  
 *      LPSTR: path.
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "ExtractDirectory"

LPSTR
ExtractDirectory
(
    LPSTR                   pszSrc,
    LPSTR                   pszDst
)
{
    if('\\' == *pszSrc)
    {
        pszSrc++;
    }

    while(TRUE)
    {
        if('\\' == *pszSrc)
        {
            *pszDst = 0;
            pszSrc++;
            break;
        }
        
        *pszDst = *pszSrc;

        if(!*pszDst)
        {
            break;
        }

        pszDst++;
        pszSrc++;
    }

    return pszSrc;
}


/****************************************************************************
 *
 *  GetRelativePath
 *
 *  Description:
 *      Converts an absolute path to a relative one.
 *
 *  Arguments:
 *      LPCSTR [in]: parent path.
 *      LPCSTR [in]: child path.
 *      LPSTR [out]: relative path.
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "GetRelativePath"

void
GetRelativePath
(
    LPCSTR                  pszInputParentPath,
    LPCSTR                  pszInputChildPath,
    LPSTR                   pszRelativePath
)
{
    BOOL                    fSuccess                = TRUE;
    CHAR                    szParentPath[MAX_PATH];
    CHAR                    szChildPath[MAX_PATH];
    CHAR                    szParentPart[MAX_PATH];
    CHAR                    szChildPart[MAX_PATH];
    LPSTR                   pszParentPath;
    LPSTR                   pszChildPath;
    LPSTR                   pszParentTemp;
    LPSTR                   pszChildTemp;

    *pszRelativePath = 0;

    GetFullPathName(pszInputParentPath, NUMELMS(szParentPath), szParentPath, NULL);
    GetFullPathName(pszInputChildPath, NUMELMS(szChildPath), szChildPath, NULL);

    if((szParentPath[0] >= 'a') && (szParentPath[0] <= 'z'))
    {
        szParentPath[0] = 'A' + (szParentPath[0] - 'a');
    }

    if((szChildPath[0] >= 'a') && (szChildPath[0] <= 'z'))
    {
        szChildPath[0] = 'A' + (szChildPath[0] - 'a');
    }

    pszParentPath = szParentPath;
    pszChildPath = szChildPath;

    //
    // If the paths aren't on the same drive, bail
    //

    if((szParentPath[0] >= 'A') && (szParentPath[0] <= 'Z') && (':' == szParentPath[1]))
    {
        fSuccess = ((szParentPath[0] == szChildPath[0]) && (szParentPath[1] == szChildPath[1]));

        if(fSuccess)
        {
            pszParentPath = &szParentPath[2];
            pszChildPath = &szChildPath[2];
        }
    }

    //
    // Skip over any matching directories
    //

    if(fSuccess)
    {
        while(TRUE)
        {
            pszParentTemp = ExtractDirectory(pszParentPath, szParentPart);

            if(!*pszParentTemp)
            {
                break;
            }

            pszChildTemp = ExtractDirectory(pszChildPath, szChildPart);

            if(!*pszChildTemp)
            {
                break;
            }

            if(_stricmp(szParentPart, szChildPart))
            {
                break;
            }

            pszParentPath = pszParentTemp;
            pszChildPath = pszChildTemp;
        }
    }

    //
    // For every directory that appears in the parent but not the child, add
    // a "..\"
    //

    if(fSuccess)
    {
        while(TRUE)
        {
            pszParentTemp = ExtractDirectory(pszParentPath, szParentPart);

            if(!*pszParentTemp)
            {
                break;
            }

            pszChildTemp = ExtractDirectory(pszChildPath, szChildPart);

            if(*pszChildTemp)
            {
                if(!_stricmp(szParentPart, szChildPart))
                {
                    break;
                }
            }

            strcat(pszRelativePath, "..\\");

            pszParentPath = pszParentTemp;
        }
    }

    //
    // If anything went wrong, just copy the full child path.  If all is good,
    // build the relative path.
    //
    
    if(fSuccess)
    {
        strcat(pszRelativePath, pszChildPath);
    }
    else
    {
        strcpy(pszRelativePath, szChildPath);
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
    CWaveBank *            pParent
)
{
    DPF_ENTER();

    m_pParent = pParent;

    InsertTailListUninit(&m_pParent->m_lstEntries, &m_leEntry);

    m_pParent->m_dwEntryCount++;

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
 *      LPCSTR [in]: entry name.
 *      LPCSTR [in]: wave file name.
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
    LPCSTR                  pszEntryName,
    LPCSTR                  pszFileName,
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
        strncpy(m_szEntryName, pszEntryName, NUMELMS(m_szEntryName));
    }
    else
    {
        _splitpath(m_szFileName, NULL, NULL, m_szEntryName, NULL);
    }

    strncpy(m_szHeaderName, m_szEntryName, NUMELMS(m_szHeaderName));
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

    if(SUCCEEDED(hr))
    {
        hr = LoadWaveFile();
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
 *      LPCSTR [in]: entry name.
 *      UINT * [out]: count of duplicate named entries found.
 *      LPCSTR [in]: entry file path.
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
    LPCSTR                  pszName,
    UINT *                  pnNameCount,
    LPCSTR                  pszPath,
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
        if(!_strcmpi(pszName, WBCHEADER_ENTRYCOUNTSHORTNAME))
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
            if(!_strcmpi(pszName, pEntry->m_szHeaderName))
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
            if(!_strcmpi(pszPath, pEntry->m_szFileName))
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
 *      LPCSTR [in]: entry name.
 *
 *  Returns:  
 *      HRESULT: COM result code.
 *
 ****************************************************************************/
 
#undef DPF_FNAME
#define DPF_FNAME "SetName"

HRESULT 
CWaveBankEntry::SetName
(
    LPCSTR                  pszName
)
{
    HRESULT                 hr                                      = S_OK;
    CHAR                    szHeaderName[NUMELMS(m_szHeaderName)];

    if(strlen(pszName) >= NUMELMS(m_szEntryName))
    {
        DPF_ERROR("That name is too long");
        hr = E_FAIL;
    }

    if(SUCCEEDED(hr))
    {
        strncpy(szHeaderName, pszName, NUMELMS(szHeaderName));
        MakeHeaderString(szHeaderName);

        hr = FindDuplicateEntry(szHeaderName, NULL, NULL, NULL);

        if(SUCCEEDED(hr))
        {
            strncpy(m_szEntryName, pszName, NUMELMS(m_szEntryName));
            strncpy(m_szHeaderName, szHeaderName, NUMELMS(m_szHeaderName));
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
 *  LoadWaveFile
 *
 *  Description:
 *      Initializes the object from a wave file.
 *
 *  Arguments:
 *      LPCSTR [in]: wave file name.
 *      DWORD [in]: processing flags.
 *
 *  Returns:  
 *      HRESULT: COM result code.
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CWaveBankEntry::LoadWaveFile"

HRESULT
CWaveBankEntry::LoadWaveFile
(
    void
)
{
    CWaveFile               WaveFile;
    WAVEBANKUNIWAVEFORMAT   Format;
    HRESULT                 hr;
    
    DPF_ENTER();
    
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
    LPDWORD                 pdwOffset
)
{
    static const DWORD      dwAlignment = 4;
    
    DPF_ENTER();

    ASSERT(!((m_dwFlags & WBFILTER_ADPCM) && (m_dwFlags & WBFILTER_8BIT)));

    //
    // If we're compressing, write the compressed format, not the source
    //

    pMetaData->Format = m_Format;
    
    if(m_dwFlags & WBFILTER_ADPCM)
    {
        if(!CXboxAdpcmFilter::ConvertFormat(&pMetaData->Format))
        {
            m_dwFlags &= ~WBFILTER_ADPCM;
        }
    }
    
    if(m_dwFlags & WBFILTER_8BIT)
    {
        if(!C8BitFilter::ConvertFormat(&pMetaData->Format))
        {
            m_dwFlags &= ~WBFILTER_8BIT;
        }
    }

    //
    // Block-align the file offset
    //
        
    pMetaData->PlayRegion.dwStart = BLOCKALIGNPAD(*pdwOffset, dwAlignment);

    //
    // If we're compressing, calculate the compressed data size as well
    //

    pMetaData->PlayRegion.dwLength = m_PlayRegion.dwLength;

    if(m_dwFlags & WBFILTER_ADPCM)
    {
        pMetaData->PlayRegion.dwLength = CXboxAdpcmFilter::GetSize(pMetaData->PlayRegion.dwLength, m_Format.nChannels, TRUE);
    }
    
    if(m_dwFlags & WBFILTER_8BIT)
    {
        pMetaData->PlayRegion.dwLength = C8BitFilter::GetSize(pMetaData->PlayRegion.dwLength, m_Format.nChannels);
    }

    //
    // ... and the loop offsets
    //

    pMetaData->LoopRegion = m_LoopRegion;

    if(m_dwFlags & WBFILTER_ADPCM)
    {
        pMetaData->LoopRegion.dwStart = CXboxAdpcmFilter::GetSize(pMetaData->LoopRegion.dwStart, m_Format.nChannels);
        pMetaData->LoopRegion.dwLength = CXboxAdpcmFilter::GetSize(pMetaData->LoopRegion.dwLength, m_Format.nChannels);
    }
    
    if(m_dwFlags & WBFILTER_8BIT)
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
    CStdFileStream *        pBankFile,
    DWORD                   dwBaseOffset,
    LPCWAVEBANKENTRY        pMetaData,
    LPVOID                  pvCopyBuffer,
    DWORD                   dwCopyBufferSize
)
{
    DWORD                   dwTotalRead     = 0;
    DWORD                   dwTotalWritten  = 0;
    CXboxAdpcmFilter        AdpcmFilter;
    C8BitFilter             BitFilter;
    CStdFileStream          WaveFile;
    DWORD                   dwAlignment;
    DWORD                   dwRead;
    DWORD                   dwWritten;
    DWORD                   dwAlignedRead;
    DWORD                   dwFilterRead;
    DWORD                   dwFileType;
    HRESULT                 hr;
    
    DPF_ENTER();

    ASSERT(!((m_dwFlags & WBFILTER_ADPCM) && (m_dwFlags & WBFILTER_8BIT)));

    if(!pBankFile)
    {
        ASSERT(dwCopyBufferSize >= pMetaData->PlayRegion.dwLength);
    }

    //
    // Open the source file and seek to the start of the data chunk
    //

    if(FAILED(hr = WaveFile.Open(m_szFileName, GENERIC_READ, FILE_SHARE_READ, OPEN_EXISTING, FILE_FLAG_SEQUENTIAL_SCAN)))
    {
        DPF_ERROR("An error occurred while attempting to open %s", m_szFileName);
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
    }

    if(SUCCEEDED(hr))
    {
        ASSERT(dwTotalRead == m_PlayRegion.dwLength);
        ASSERT(dwTotalWritten == pMetaData->PlayRegion.dwLength);
    }

    DPF_LEAVE_HRESULT(hr);

    return hr;
}


/****************************************************************************
 *
 *  ReadProjectData
 *
 *  Description:
 *      Initializes the entry based on saved project data.
 *
 *  Arguments:
 *      LPCSTR [in]: project file name.
 *      LPCSTR [in]: section name.
 *      DWORD [in]: entry index.
 *
 *  Returns:  
 *      HRESULT: COM result code.
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CWaveBankEntry::ReadProjectData"

HRESULT
CWaveBankEntry::ReadProjectData
(
    LPCSTR                  pszProjectFile, 
    LPCSTR                  pszSectionName,
    DWORD                   dwEntryIndex
)
{
    CHAR                    szKeyName[0x100];
    CHAR                    szEntryData[MAX_PATH + 0x200];
    CHAR                    szEntryName[0x100];
    CHAR                    szFileName[MAX_PATH];
    DWORD                   dwFlags;
    LPCSTR                  pszSrc;
    LPSTR                   pszDst;
    HRESULT                 hr;
    
    DPF_ENTER();

    //
    // Load the whole entry string
    //

    sprintf(szKeyName, WBPROJECT_ENTRY_KEY, dwEntryIndex);

    GetPrivateProfileString(pszSectionName, szKeyName, "", szEntryData, NUMELMS(szEntryData), pszProjectFile);

    pszSrc = szEntryData;
    
    //
    // Extract the entry name
    //
    
    pszDst = szEntryName;

    while(TRUE)
    {
        switch(tolower(*pszSrc))
        {
            case WBPROJECT_ENTRYEXT_SEPARATOR_LCHAR:
                pszSrc++;
                *pszDst = 0;
                break;

            default:
                *pszDst = *pszSrc;
                break;
        }

        if(!*pszDst)
        {
            break;
        }

        pszDst++;
        pszSrc++;
    }

    //
    // Extract the file name
    //
    
    pszDst = szFileName;

    while(TRUE)
    {
        switch(tolower(*pszSrc))
        {
            case WBPROJECT_ENTRYEXT_SEPARATOR_LCHAR:
                pszSrc++;
                *pszDst = 0;
                break;

            default:
                *pszDst = *pszSrc;
                break;
        }

        if(!*pszDst)
        {
            break;
        }

        pszDst++;
        pszSrc++;
    }

    //
    // Extract the flags
    //

    dwFlags = 0;

    while(*pszSrc)
    {
        switch(tolower(*pszSrc))
        {
            case WBPROJECT_ENTRYEXT_SEPARATOR_LCHAR:
                break;

            case WBPROJECT_ENTRYEXT_ADPCMFILTER_LCHAR:
                dwFlags |= WBFILTER_ADPCM;
                break;

            case WBPROJECT_ENTRYEXT_8BITFILTER_LCHAR:
                dwFlags |= WBFILTER_8BIT;
                break;

            default:
                DPF_WARNING("Unrecognized flag character in project file");
                break;
        }

        pszSrc++;
    }

    //
    // Initialize the entry
    //

    hr = Initialize(szEntryName, szFileName, dwFlags);

    DPF_LEAVE_HRESULT(hr);

    return hr;
}


/****************************************************************************
 *
 *  WriteProjectData
 *
 *  Description:
 *      Stores project data for the entry.
 *
 *  Arguments:
 *      LPCSTR [in]: project file name.
 *      LPCSTR [in]: section name.
 *      DWORD [in]: entry index.
 *
 *  Returns:  
 *      HRESULT: COM result code.
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CWaveBankEntry::WriteProjectData"

HRESULT
CWaveBankEntry::WriteProjectData
(
    LPCSTR                  pszProjectFile, 
    LPCSTR                  pszSectionName,
    DWORD                   dwEntryIndex
)
{
    HRESULT                 hr                              = S_OK;
    CHAR                    szKeyName[0x100];
    CHAR                    szEntryData[MAX_PATH + 0x200];
    
    DPF_ENTER();

    sprintf(szKeyName, WBPROJECT_ENTRY_KEY, dwEntryIndex);

    strcpy(szEntryData, m_szEntryName);
    strcat(szEntryData, WBPROJECT_ENTRYEXT_SEPARATOR_STR);

    GetRelativePath(pszProjectFile, m_szFileName, &szEntryData[strlen(szEntryData)]);

    if(m_dwFlags & WBFILTER_MASK)
    {
        strcat(szEntryData, WBPROJECT_ENTRYEXT_SEPARATOR_STR);

        if(m_dwFlags & WBFILTER_ADPCM)
        {
            strcat(szEntryData, WBPROJECT_ENTRYEXT_ADPCMFILTER_STR);
        }

        if(m_dwFlags & WBFILTER_8BIT)
        {
            strcat(szEntryData, WBPROJECT_ENTRYEXT_8BITFILTER_STR);
        }
    }

    if(!WritePrivateProfileString(pszSectionName, szKeyName, szEntryData, pszProjectFile))
    {
        DPF_ERROR("Error %lu occurred writing to the project file", GetLastError());
        hr = E_FAIL;
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
    CWaveBankProject *     pParent
)
{
    DPF_ENTER();

    m_pParent = pParent;
    m_dwEntryCount = 0;

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
    CWaveBankEntry *       pEntry;
    
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
 *      LPCSTR [in]: wave bank name.
 *      LPCSTR [in]: bank file path.
 *      LPCSTR [in]: header file path.
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
    LPCSTR                  pszBankName,
    LPCSTR                  pszBankFile,
    LPCSTR                  pszHeaderFile
)
{
    HRESULT                 hr                  = S_OK;
    UINT                    nNameCount;
    UINT                    nBankFileCount;
    UINT                    nHeaderFileCount;

    DPF_ENTER();

    strncpy(m_szBankName, pszBankName, NUMELMS(m_szBankName));
    strncpy(m_szHeaderName, pszBankName, NUMELMS(m_szHeaderName));
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
 *      LPCSTR [in]: Bank name.
 *      UINT * [out]: count of duplicate named entries found.
 *      LPCSTR [in]: Bank file path.
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
    LPCSTR                  pszName,
    UINT *                  pnNameCount,
    LPCSTR                  pszBankFile,
    UINT *                  pnBankFileCount,
    LPCSTR                  pszHeaderFile,
    UINT *                  pnHeaderFileCount
)
{
    HRESULT                 hr          = S_OK;
    PLIST_ENTRY             pleBank;
    CWaveBank *            pBank;
    
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
            if(!_strcmpi(pszName, pBank->m_szHeaderName))
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
            if(!_strcmpi(pszBankFile, pBank->m_szBankFile))
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
            if(!_strcmpi(pszHeaderFile, pBank->m_szHeaderFile))
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
 *      LPCSTR [in]: Bank name.
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
    LPCSTR                  pszName
)
{
    HRESULT                 hr                                      = S_OK;
    CHAR                    szHeaderName[NUMELMS(m_szHeaderName)];

    if(strlen(pszName) >= NUMELMS(m_szBankName))
    {
        DPF_ERROR("That name is too long");
        hr = E_FAIL;
    }

    if(SUCCEEDED(hr))
    {
        strncpy(szHeaderName, pszName, NUMELMS(szHeaderName));
        MakeHeaderString(szHeaderName);

        hr = FindDuplicateBank(szHeaderName, NULL, NULL, NULL, NULL, NULL);

        if(SUCCEEDED(hr))
        {
            strncpy(m_szBankName, pszName, NUMELMS(m_szBankName));
            strncpy(m_szHeaderName, szHeaderName, NUMELMS(m_szHeaderName));
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
 *  AddEntry
 *
 *  Description:
 *      Adds an entry to the wave bank.
 *
 *  Arguments:
 *      LPCSTR [in]: entry name.
 *      LPCSTR [in]: wave file name.
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
    LPCSTR                  pszEntryName,
    LPCSTR                  pszFileName,
    DWORD                   dwFlags,
    CWaveBankEntry **      ppEntry
)
{
    CWaveBankEntry *       pEntry;
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
    LPWAVEBANKENTRY         paMetaData          = NULL;
    HRESULT                 hr                  = S_OK;
    CStdFileStream          BankFile;
    WAVEBANKHEADER          Header;
    PLIST_ENTRY             pleEntry;
    CWaveBankEntry *        pEntry;
    DWORD                   dwEntryIndex;
    DWORD                   dwOffset;
    
    DPF_ENTER();

    if(m_dwEntryCount)
    {
        //
        // Call the callback
        //

        if(pCallback)
        {
            if(!pCallback->OpenBank(m_szBankFile))
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

        //
        // Allocate data for the entire metadata section
        //

        if(SUCCEEDED(hr))
        {
            if(FAILED(hr = HRFROMP(paMetaData = MEMALLOC(WAVEBANKENTRY, m_dwEntryCount))))
            {
                DPF_ERROR("Out of memory");
            }
        }

        //
        // Write the file header
        //

        if(SUCCEEDED(hr))
        {
            Header.dwSignature = WAVEBANKHEADER_SIGNATURE;
            Header.dwVersion = WAVEBANKHEADER_VERSION;
            Header.dwFlags = m_dwFlags;
            Header.dwEntryCount = m_dwEntryCount;
            Header.dwAlignment = 4;
            
            strncpy(Header.szBankName, m_szBankName, NUMELMS(Header.szBankName));

            if(FAILED(hr = BankFile.Write(&Header, sizeof(Header))))
            {
                DPF_ERROR("An error occurred while attempting to write to the bank file");
            }
        }

        //
        // Generate and write metadata
        //

        if(SUCCEEDED(hr))
        {
            for(pleEntry = m_lstEntries.Flink, dwEntryIndex = 0, dwOffset = 0; SUCCEEDED(hr) && (pleEntry != &m_lstEntries); pleEntry = pleEntry->Flink, dwEntryIndex++)
            {
                ASSERT(dwEntryIndex < m_dwEntryCount);
                
                pEntry = CONTAINING_RECORD(pleEntry, CWaveBankEntry, m_leEntry);

                hr = pEntry->GetMetaData(&paMetaData[dwEntryIndex], &dwOffset);
            }
        }
        
        if(SUCCEEDED(hr))
        {
            if(FAILED(hr = BankFile.Write(paMetaData, sizeof(paMetaData[0]) * m_dwEntryCount)))
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

                if(pCallback)
                {
                    if(!pCallback->BeginEntry(pEntry->m_szFileName, pEntry->m_dwFlags))
                    {
                        hr = E_ABORT;
                    }
                }
        
                if(SUCCEEDED(hr))
                {
                    dwOffset = sizeof(Header) + (sizeof(paMetaData[0]) * Header.dwEntryCount);
                    
                    hr = pEntry->CommitWaveData(&BankFile, dwOffset, &paMetaData[dwEntryIndex], pvCopyBuffer, dwCopyBufferSize);
                }

                if(SUCCEEDED(hr) && pCallback)
                {
                    pCallback->EndEntry(pEntry->m_szFileName, pEntry->m_dwFlags);
                }
            }
        }

        //
        // Call the callback
        //

        if(pCallback)
        {
            pCallback->CloseBank(m_szBankFile);
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
    MEMFREE(paMetaData);

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
    static const LPCSTR     pszFileBegin            = "#pragma once\r\n\r\n";
    static const LPCSTR     pszEnumBegin            = "typedef enum\r\n{\r\n";
    static const LPCSTR     pszEnumEnd              = "} " WBCHEADER_ENUMNAME ";\r\n\r\n";
    static const LPCSTR     pszEnumEntry            = "    " WBCHEADER_ENUMENTRYNAME " = %lu,\r\n";
    static const LPCSTR     pszEnumCount            = "#define " WBCHEADER_ENTRYCOUNTNAME " %lu\r\n";
    HRESULT                 hr                      = S_OK;
    CStdFileStream          HeaderFile;
    CHAR                    szText[MAX_PATH];
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
            if(!pCallback->OpenHeader(m_szHeaderFile))
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
            if(FAILED(hr = HeaderFile.Write(pszFileBegin, strlen(pszFileBegin))))
            {
                DPF_ERROR("An error occurred while attempting to write to the header file");
            }
        }

        //
        // Write the enumeration header
        //

        if(SUCCEEDED(hr))
        {
            if(FAILED(hr = HeaderFile.Write(pszEnumBegin, strlen(pszEnumBegin))))
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

                if(FAILED(hr = HeaderFile.Write(szText, strlen(szText))))
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

            if(FAILED(hr = HeaderFile.Write(szText, strlen(szText))))
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

            if(FAILED(hr = HeaderFile.Write(szText, strlen(szText))))
            {
                DPF_ERROR("An error occurred while attempting to write to the header file");
            }
        }

        //
        // Call the callback
        //

        if(pCallback)
        {
            pCallback->CloseHeader(m_szHeaderFile);
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
 *  ReadProjectData
 *
 *  Description:
 *      Initializes the entry based on saved project data.
 *
 *  Arguments:
 *      LPCSTR [in]: project file name.
 *      DWORD [in]: bank index.
 *
 *  Returns:  
 *      HRESULT: COM result code.
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CWaveBank::ReadProjectData"

HRESULT
CWaveBank::ReadProjectData
(
    LPCSTR                  pszProjectFile, 
    DWORD                   dwBankIndex
)
{
    HRESULT                 hr                      = S_OK;
    CHAR                    szSectionName[MAX_PATH];
    CHAR                    szBankName[MAX_PATH];
    CHAR                    szBankFile[MAX_PATH];
    CHAR                    szHeaderFile[MAX_PATH];
    DWORD                   dwEntryCount;
    CWaveBankEntry *        pEntry;
    HRESULT                 hrEntry;
    DWORD                   i;
    
    DPF_ENTER();

    sprintf(szSectionName, WBPROJECT_BANK_KEY, dwBankIndex);

    GetPrivateProfileString(szSectionName, WBPROJECT_BANK_NAME_KEY, "", szBankName, NUMELMS(szBankName), pszProjectFile);
    GetPrivateProfileString(szSectionName, WBPROJECT_BANK_FILE_KEY, "", szBankFile, NUMELMS(szBankFile), pszProjectFile);
    GetPrivateProfileString(szSectionName, WBPROJECT_BANK_HEADER_KEY, "", szHeaderFile, NUMELMS(szHeaderFile), pszProjectFile);

    hr = Initialize(szBankName, szBankFile, szHeaderFile);

    if(SUCCEEDED(hr))
    {
        dwEntryCount = GetPrivateProfileInt(szSectionName, WBPROJECT_BANK_ENTRYCOUNT_KEY, 0, pszProjectFile);

        for(i = 0; (i < dwEntryCount) && SUCCEEDED(hr); i++)
        {
            hrEntry = HRFROMP(pEntry = CreateEntry());

            if(SUCCEEDED(hrEntry))
            {
                hrEntry = pEntry->ReadProjectData(pszProjectFile, szSectionName, i);
            }

            if(FAILED(hrEntry))
            {
                DELETE(pEntry);
            }
        }
    }

    DPF_LEAVE_HRESULT(hr);

    return hr;
}


/****************************************************************************
 *
 *  WriteProjectData
 *
 *  Description:
 *      Saves project data for the bank.
 *
 *  Arguments:
 *      LPCSTR [in]: project file name.
 *      DWORD [in]: bank index.
 *
 *  Returns:  
 *      HRESULT: COM result code.
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CWaveBank::WriteProjectData"

HRESULT
CWaveBank::WriteProjectData
(
    LPCSTR                  pszProjectFile, 
    DWORD                   dwBankIndex
)
{
    HRESULT                 hr                      = S_OK;
    CHAR                    szSectionName[MAX_PATH];
    CHAR                    szText[MAX_PATH];
    PLIST_ENTRY             pleEntry;
    CWaveBankEntry *        pEntry;
    DWORD                   i;
    
    DPF_ENTER();

    sprintf(szSectionName, WBPROJECT_BANK_KEY, dwBankIndex);
    
    if(!WritePrivateProfileString(szSectionName, WBPROJECT_BANK_NAME_KEY, m_szBankName, pszProjectFile))
    {
        DPF_ERROR("An error occurred while attempting to write to the project file");
        hr = E_FAIL;
    }        

    if(SUCCEEDED(hr))
    {
        GetRelativePath(pszProjectFile, m_szBankFile, szText);
        
        if(!WritePrivateProfileString(szSectionName, WBPROJECT_BANK_FILE_KEY, szText, pszProjectFile))
        {
            DPF_ERROR("An error occurred while attempting to write to the project file");
            hr = E_FAIL;
        }
    }

    if(SUCCEEDED(hr))
    {
        GetRelativePath(pszProjectFile, m_szHeaderFile, szText);
        
        if(!WritePrivateProfileString(szSectionName, WBPROJECT_BANK_HEADER_KEY, szText, pszProjectFile))
        {
            DPF_ERROR("An error occurred while attempting to write to the project file");
            hr = E_FAIL;
        }        
    }

    if(SUCCEEDED(hr))
    {
        sprintf(szText, "%lu", m_dwEntryCount);

        if(!WritePrivateProfileString(szSectionName, WBPROJECT_BANK_ENTRYCOUNT_KEY, szText, pszProjectFile))
        {
            DPF_ERROR("An error occurred while attempting to write to the project file");
            hr = E_FAIL;
        }        
    }

    if(SUCCEEDED(hr))
    {
        for(pleEntry = m_lstEntries.Flink, i = 0; (pleEntry != &m_lstEntries) && SUCCEEDED(hr); pleEntry = pleEntry->Flink, i++)
        {
            ASSERT(i < m_dwEntryCount);
        
            pEntry = CONTAINING_RECORD(pleEntry, CWaveBankEntry, m_leEntry);

            hr = pEntry->WriteProjectData(pszProjectFile, szSectionName, i);
        }
    }

    if(SUCCEEDED(hr))
    {
        ASSERT(i == m_dwEntryCount);
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
    DWORD                   dwSize      = 0;
    HRESULT                 hr          = S_OK;
    WAVEBANKENTRY           MetaData;
    PLIST_ENTRY             pleEntry;
    CWaveBankEntry *        pEntry;
    
    DPF_ENTER();

    //
    // Get entry meta-data and calculate the size based on the final file 
    // offset returned.
    //

    for(pleEntry = m_lstEntries.Flink; SUCCEEDED(hr) && (pleEntry != &m_lstEntries); pleEntry = pleEntry->Flink)
    {
        pEntry = CONTAINING_RECORD(pleEntry, CWaveBankEntry, m_leEntry);

        hr = pEntry->GetMetaData(&MetaData, &dwSize);
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
 *      LPCSTR [in]: wave bank name.
 *      LPCSTR [in]: bank file path.
 *      LPCSTR [in]: header file path.
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
    LPCSTR                  pszBankName, 
    LPCSTR                  pszBankFile, 
    LPCSTR                  pszHeaderFile,
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
    UINT                    nBankCount;
    UINT                    nEntryCount;

    DPF_ENTER();

    nBankCount = 0;
    nEntryCount = 0;
    
    for(pleBank = m_lstBanks.Flink; (pleBank != &m_lstBanks) && SUCCEEDED(hr); pleBank = pleBank->Flink)
    {
        pBank = CONTAINING_RECORD(pleBank, CWaveBank, m_leBank);

        for(pleEntry = pBank->m_lstEntries.Flink; (pleEntry != &pBank->m_lstEntries) && SUCCEEDED(hr); pleEntry = pleEntry->Flink)
        {
            nEntryCount++;
        }

        nBankCount++;
    }

    if(pCallback)
    {
        if(!pCallback->BeginProject(nBankCount, nEntryCount))
        {
            hr = E_ABORT;
        }
    }

    if(!nBankCount)
    {
        DPF_ERROR("No banks in the project");
        hr = E_FAIL;
    }
    else if(!nEntryCount)
    {
        DPF_ERROR("No entries in the project");
        hr = E_FAIL;
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
        pCallback->EndProject(hr);
    }

    DPF_LEAVE_HRESULT(hr);

    return hr;
}


/****************************************************************************
 *
 *  ReadProjectData
 *
 *  Description:
 *      Initializes the project based on saved project data.
 *
 *  Arguments:
 *      LPCSTR [in]: project file name.
 *
 *  Returns:  
 *      HRESULT: COM result code.
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CWaveBankProject::ReadProjectData"

HRESULT
CWaveBankProject::ReadProjectData
(
    LPCSTR                  pszProjectFile
)
{
    HRESULT                 hr                          = S_OK;
    CHAR                    szProjectFile[MAX_PATH];
    LPSTR                   pszFilePart;
    DWORD                   dwBankCount;
    CWaveBank *             pBank;
    char                    c;
    DWORD                   i;
    
    DPF_ENTER();

    //
    // Convert the project file name to a full path.  The INI APIs prefer it
    //

    GetFullPathName(pszProjectFile, NUMELMS(szProjectFile), szProjectFile, &pszFilePart);

    //
    // Set the current directory to the project file's to make sure any 
    // relative paths are expanded properly
    //

    c = *pszFilePart;
    *pszFilePart = 0;
    
    if(!SetCurrentDirectory(szProjectFile))
    {
        DPF_ERROR("Unable to change to project file directory");
        hr = E_FAIL;
    }

    *pszFilePart = c;

    //
    // Start loading banks
    //

    if(SUCCEEDED(hr))
    {
        dwBankCount = GetPrivateProfileInt(WBPROJECT_PROJECT_MAIN_SECTION, WBPROJECT_PROJECT_BANKCOUNT_KEY, 0, szProjectFile);

        for(i = 0; (i < dwBankCount) && SUCCEEDED(hr); i++)
        {
            hr = HRFROMP(pBank = CreateBank());

            if(SUCCEEDED(hr))
            {
                hr = pBank->ReadProjectData(szProjectFile, i);
            }
        }
    }

    DPF_LEAVE_HRESULT(hr);

    return hr;
}


/****************************************************************************
 *
 *  WriteProjectData
 *
 *  Description:
 *      Stores project data.
 *
 *  Arguments:
 *      LPCSTR [in]: project file name.
 *
 *  Returns:  
 *      HRESULT: COM result code.
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CWaveBankProject::WriteProjectData"

HRESULT
CWaveBankProject::WriteProjectData
(
    LPCSTR                  pszProjectFile
)
{
    HRESULT                 hr                      = S_OK;
    CStdFileStream          ProjectFile;
    CHAR                    szBankCount[MAX_PATH];
    PLIST_ENTRY             pleEntry;
    CWaveBank *             pBank;
    DWORD                   i;
    
    DPF_ENTER();

    //
    // Truncate the existing file
    //

    DeleteFile(pszProjectFile);

    //
    // Write project data
    //

    if(SUCCEEDED(hr))
    {
        sprintf(szBankCount, "%lu", m_dwBankCount);
        
        if(!WritePrivateProfileString(WBPROJECT_PROJECT_MAIN_SECTION, WBPROJECT_PROJECT_BANKCOUNT_KEY, szBankCount, pszProjectFile))
        {
            DPF_ERROR("An error occurred while attempting to write to the project file");
            hr = E_FAIL;
        }
    }

    //
    // Write all bank data
    //

    for(pleEntry = m_lstBanks.Flink, i = 0; (pleEntry != &m_lstBanks) && SUCCEEDED(hr); pleEntry = pleEntry->Flink, i++)
    {
        ASSERT(i < m_dwBankCount);
        
        pBank = CONTAINING_RECORD(pleEntry, CWaveBank, m_leBank);

        hr = pBank->WriteProjectData(pszProjectFile, i);
    }

    if(SUCCEEDED(hr))
    {
        ASSERT(i == m_dwBankCount);
    }

    DPF_LEAVE_HRESULT(hr);

    return hr;
}


