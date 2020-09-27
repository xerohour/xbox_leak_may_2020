/***************************************************************************
 *
 *  Copyright (C) 02/13/2002 Microsoft Corporation.  All Rights Reserved.
 *
 *  File:       loader.cpp
 *  Content:    Wave bank project loader.
 *  History:
 *   Date       By      Reason
 *   ====       ==      ======
 *  02/13/2002  dereks  Created.
 *
 ****************************************************************************/

#include "wavbndli.h"


/****************************************************************************
 *
 *  ExtractDirectory
 *
 *  Description:
 *      Extracts the first directory from a path.
 *
 *  Arguments:
 *      LPTSTR [in]: path.
 *      LPTSTR [out]: directory name.
 *
 *  Returns:  
 *      LPTSTR: path.
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "ExtractDirectory"

static LPTSTR
ExtractDirectory
(
    LPTSTR                   pszSrc,
    LPTSTR                   pszDst
)
{
    if(TEXT('\\') == *pszSrc)
    {
        pszSrc++;
    }

    while(TRUE)
    {
        if(TEXT('\\') == *pszSrc)
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
 *      LPCTSTR [in]: parent path.
 *      LPCTSTR [in]: child path.
 *      LPTSTR [out]: relative path.
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "GetRelativePath"

static void
GetRelativePath
(
    LPCTSTR                 pszInputParentPath,
    LPCTSTR                 pszInputChildPath,
    LPTSTR                  pszRelativePath
)
{
    BOOL                    fSuccess                = TRUE;
    TCHAR                   szParentPath[MAX_PATH];
    TCHAR                   szChildPath[MAX_PATH];
    TCHAR                   szParentPart[MAX_PATH];
    TCHAR                   szChildPart[MAX_PATH];
    LPTSTR                  pszParentPath;
    LPTSTR                  pszChildPath;
    LPTSTR                  pszParentTemp;
    LPTSTR                  pszChildTemp;

    *pszRelativePath = 0;

    GetFullPathName(pszInputParentPath, NUMELMS(szParentPath), szParentPath, NULL);
    GetFullPathName(pszInputChildPath, NUMELMS(szChildPath), szChildPath, NULL);

    if((szParentPath[0] >= TEXT('a')) && (szParentPath[0] <= TEXT('z')))
    {
        szParentPath[0] = TEXT('A') + (szParentPath[0] - TEXT('a'));
    }

    if((szChildPath[0] >= TEXT('a')) && (szChildPath[0] <= TEXT('z')))
    {
        szChildPath[0] = TEXT('A') + (szChildPath[0] - TEXT('a'));
    }

    pszParentPath = szParentPath;
    pszChildPath = szChildPath;

    //
    // If the paths aren't on the same drive, bail
    //

    if((szParentPath[0] >= TEXT('A')) && (szParentPath[0] <= TEXT('Z')) && (TEXT(':') == szParentPath[1]))
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

            if(_tcsicmp(szParentPart, szChildPart))
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
                if(!_tcsicmp(szParentPart, szChildPart))
                {
                    break;
                }
            }

            _tcscat(pszRelativePath, TEXT("..\\"));

            pszParentPath = pszParentTemp;
        }
    }

    //
    // If anything went wrong, just copy the full child path.  If all is good,
    // build the relative path.
    //
    
    if(fSuccess)
    {
        _tcscat(pszRelativePath, pszChildPath);
    }
    else
    {
        _tcscpy(pszRelativePath, szChildPath);
    }
}


/****************************************************************************
 *
 *  Load
 *
 *  Description:
 *      Loads project data.
 *
 *  Arguments:
 *      LPCTSTR [in]: project file name.
 *
 *  Returns:  
 *      HRESULT: COM result code.
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CWaveBankProjectFile::Load"

HRESULT
CWaveBankProjectFile::Load
(
    CWaveBankProject *      pProject,
    LPCTSTR                 pszProjectFile
)
{
    HRESULT                 hr          = S_OK;
    LPTSTR                  pszFilePart;
    DWORD                   dwBankCount;
    TCHAR                   c;
    DWORD                   i;
    
    DPF_ENTER();

    //
    // Save a pointer to the project
    //

    m_pProject = pProject;

    //
    // Convert the project file name to a full path.  The INI APIs prefer it
    //

    GetFullPathName(pszProjectFile, NUMELMS(m_szProjectFile), m_szProjectFile, &pszFilePart);

    //
    // Set the current directory to the project file's to make sure any 
    // relative paths are expanded properly
    //

    c = *pszFilePart;
    *pszFilePart = 0;
    
    if(!SetCurrentDirectory(m_szProjectFile))
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
        dwBankCount = GetPrivateProfileInt(WBPROJECT_PROJECT_MAIN_SECTION, WBPROJECT_PROJECT_BANKCOUNT_KEY, 0, m_szProjectFile);

        for(i = 0; (i < dwBankCount) && SUCCEEDED(hr); i++)
        {
            hr = LoadBank(i);
        }
    }

    DPF_LEAVE_HRESULT(hr);

    return hr;
}


/****************************************************************************
 *
 *  Save
 *
 *  Description:
 *      Stores project data.
 *
 *  Arguments:
 *      LPCTSTR [in]: project file name.
 *
 *  Returns:  
 *      HRESULT: COM result code.
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CWaveBankProjectFile::Save"

HRESULT
CWaveBankProjectFile::Save
(
    CWaveBankProject *      pProject,
    LPCTSTR                 pszProjectFile
)
{
    HRESULT                 hr                      = S_OK;
    CStdFileStream          ProjectFile;
    TCHAR                   szBankCount[MAX_PATH];
    LPTSTR                  pszFilePart;
    PLIST_ENTRY             pleEntry;
    CWaveBank *             pBank;
    DWORD                   i;
    
    DPF_ENTER();

    //
    // Save a pointer to the project
    //

    m_pProject = pProject;

    //
    // Convert the project file name to a full path.  The INI APIs prefer it
    //

    GetFullPathName(pszProjectFile, NUMELMS(m_szProjectFile), m_szProjectFile, &pszFilePart);

    //
    // Truncate the existing file
    //

    DeleteFile(m_szProjectFile);

    //
    // Write project data
    //

    if(SUCCEEDED(hr))
    {
        sprintf(szBankCount, TEXT("%lu"), m_pProject->m_dwBankCount);
        
        if(!WritePrivateProfileString(WBPROJECT_PROJECT_MAIN_SECTION, WBPROJECT_PROJECT_BANKCOUNT_KEY, szBankCount, m_szProjectFile))
        {
            DPF_ERROR("An error occurred while attempting to write to the project file");
            hr = E_FAIL;
        }
    }

    //
    // Write all bank data
    //

    for(pleEntry = m_pProject->m_lstBanks.Flink, i = 0; (pleEntry != &m_pProject->m_lstBanks) && SUCCEEDED(hr); pleEntry = pleEntry->Flink, i++)
    {
        ASSERT(i < m_pProject->m_dwBankCount);
        
        pBank = CONTAINING_RECORD(pleEntry, CWaveBank, m_leBank);

        hr = SaveBank(pBank, i);
    }

    if(SUCCEEDED(hr))
    {
        ASSERT(i == m_pProject->m_dwBankCount);
    }

    DPF_LEAVE_HRESULT(hr);

    return hr;
}


/****************************************************************************
 *
 *  LoadBank
 *
 *  Description:
 *      Loads a bank from the project file.
 *
 *  Arguments:
 *      DWORD [in]: bank index.
 *
 *  Returns:  
 *      HRESULT: COM result code.
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CWaveBankProjectFile::LoadBank"

HRESULT
CWaveBankProjectFile::LoadBank
(
    DWORD                   dwBankIndex
)
{
    CWaveBank *             pBank                   = NULL;
    HRESULT                 hr                      = S_OK;
    TCHAR                   szSectionName[MAX_PATH];
    TCHAR                   szBankName[MAX_PATH];
    TCHAR                   szBankFile[MAX_PATH];
    TCHAR                   szHeaderFile[MAX_PATH];
    DWORD                   dwEntryCount;
    CWaveBankEntry *        pEntry;
    HRESULT                 hrEntry;
    DWORD                   i;
    
    DPF_ENTER();

    sprintf(szSectionName, WBPROJECT_BANK_KEY, dwBankIndex);

    GetPrivateProfileString(szSectionName, WBPROJECT_BANK_NAME_KEY, TEXT(""), szBankName, NUMELMS(szBankName), m_szProjectFile);
    GetPrivateProfileString(szSectionName, WBPROJECT_BANK_FILE_KEY, TEXT(""), szBankFile, NUMELMS(szBankFile), m_szProjectFile);
    GetPrivateProfileString(szSectionName, WBPROJECT_BANK_HEADER_KEY, TEXT(""), szHeaderFile, NUMELMS(szHeaderFile), m_szProjectFile);

    hr = m_pProject->AddBank(szBankName, szBankFile, szHeaderFile, &pBank);

    if(SUCCEEDED(hr))
    {
        dwEntryCount = GetPrivateProfileInt(szSectionName, WBPROJECT_BANK_ENTRYCOUNT_KEY, 0, m_szProjectFile);

        for(i = 0; (i < dwEntryCount) && SUCCEEDED(hr); i++)
        {
            hr = LoadEntry(szSectionName, pBank, i);
        }
    }

    DPF_LEAVE_HRESULT(hr);

    return hr;
}


/****************************************************************************
 *
 *  SaveBank
 *
 *  Description:
 *      Saves project data for the bank.
 *
 *  Arguments:
 *      CWaveBank * [in]: bank object.
 *      DWORD [in]: bank index.
 *
 *  Returns:  
 *      HRESULT: COM result code.
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CWaveBankProjectFile::SaveBank"

HRESULT
CWaveBankProjectFile::SaveBank
(
    CWaveBank *             pBank,
    DWORD                   dwBankIndex
)
{
    HRESULT                 hr                      = S_OK;
    TCHAR                   szSectionName[MAX_PATH];
    TCHAR                   szText[MAX_PATH];
    PLIST_ENTRY             pleEntry;
    CWaveBankEntry *        pEntry;
    DWORD                   i;
    
    DPF_ENTER();

    sprintf(szSectionName, WBPROJECT_BANK_KEY, dwBankIndex);
    
    if(!WritePrivateProfileString(szSectionName, WBPROJECT_BANK_NAME_KEY, pBank->m_szBankName, m_szProjectFile))
    {
        DPF_ERROR("An error occurred while attempting to write to the project file");
        hr = E_FAIL;
    }        

    if(SUCCEEDED(hr))
    {
        GetRelativePath(m_szProjectFile, pBank->m_szBankFile, szText);
        
        if(!WritePrivateProfileString(szSectionName, WBPROJECT_BANK_FILE_KEY, szText, m_szProjectFile))
        {
            DPF_ERROR("An error occurred while attempting to write to the project file");
            hr = E_FAIL;
        }
    }

    if(SUCCEEDED(hr))
    {
        GetRelativePath(m_szProjectFile, pBank->m_szHeaderFile, szText);
        
        if(!WritePrivateProfileString(szSectionName, WBPROJECT_BANK_HEADER_KEY, szText, m_szProjectFile))
        {
            DPF_ERROR("An error occurred while attempting to write to the project file");
            hr = E_FAIL;
        }        
    }

    if(SUCCEEDED(hr))
    {
        sprintf(szText, TEXT("%lu"), pBank->m_dwEntryCount);

        if(!WritePrivateProfileString(szSectionName, WBPROJECT_BANK_ENTRYCOUNT_KEY, szText, m_szProjectFile))
        {
            DPF_ERROR("An error occurred while attempting to write to the project file");
            hr = E_FAIL;
        }        
    }

    if(SUCCEEDED(hr))
    {
        for(pleEntry = pBank->m_lstEntries.Flink, i = 0; (pleEntry != &pBank->m_lstEntries) && SUCCEEDED(hr); pleEntry = pleEntry->Flink, i++)
        {
            ASSERT(i < pBank->m_dwEntryCount);
        
            pEntry = CONTAINING_RECORD(pleEntry, CWaveBankEntry, m_leEntry);

            hr = SaveEntry(szSectionName, pEntry, i);
        }
    }

    if(SUCCEEDED(hr))
    {
        ASSERT(i == pBank->m_dwEntryCount);
    }

    DPF_LEAVE_HRESULT(hr);

    return hr;
}


/****************************************************************************
 *
 *  LoadEntry
 *
 *  Description:
 *      Loads an entry from the project file.
 *
 *  Arguments:
 *      LPCTSTR [in]: section name.
 *      CWaveBank * [in]: parent bank.
 *      DWORD [in]: entry index.
 *
 *  Returns:  
 *      HRESULT: COM result code.
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CWaveBankProjectFile::LoadEntry"

HRESULT
CWaveBankProjectFile::LoadEntry
(
    LPCTSTR                 pszSectionName,
    CWaveBank *             pBank,
    DWORD                   dwEntryIndex
)
{
    TCHAR                   szKeyName[0x100];
    TCHAR                   szEntryData[MAX_PATH + 0x200];
    TCHAR                   szEntryName[0x100];
    TCHAR                   szFileName[MAX_PATH];
    DWORD                   dwFlags;
    LPCTSTR                 pszSrc;
    LPTSTR                  pszDst;
    HRESULT                 hr;
    
    DPF_ENTER();

    //
    // Load the whole entry string
    //

    sprintf(szKeyName, WBPROJECT_ENTRY_KEY, dwEntryIndex);

    GetPrivateProfileString(pszSectionName, szKeyName, "", szEntryData, NUMELMS(szEntryData), m_szProjectFile);

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
    // Create the entry
    //

    hr = pBank->AddEntry(szEntryName, szFileName, dwFlags);

    DPF_LEAVE_HRESULT(hr);

    return hr;
}


/****************************************************************************
 *
 *  SaveEntry
 *
 *  Description:
 *      Stores project data for the entry.
 *
 *  Arguments:
 *      LPCTSTR [in]: section name.
 *      CWaveBankEntry * [in]: entry.
 *      DWORD [in]: entry index.
 *
 *  Returns:  
 *      HRESULT: COM result code.
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CWaveBankProjectFile::SaveEntry"

HRESULT
CWaveBankProjectFile::SaveEntry
(
    LPCTSTR                 pszSectionName,
    CWaveBankEntry *        pEntry,
    DWORD                   dwEntryIndex
)
{
    HRESULT                 hr                              = S_OK;
    TCHAR                   szKeyName[0x100];
    TCHAR                   szEntryData[MAX_PATH + 0x200];
    
    DPF_ENTER();

    sprintf(szKeyName, WBPROJECT_ENTRY_KEY, dwEntryIndex);

    _tcscpy(szEntryData, pEntry->m_szEntryName);
    _tcscat(szEntryData, WBPROJECT_ENTRYEXT_SEPARATOR_STR);

    GetRelativePath(m_szProjectFile, pEntry->m_szFileName, &szEntryData[_tcslen(szEntryData)]);

    if(pEntry->m_dwFlags & WBFILTER_MASK)
    {
        _tcscat(szEntryData, WBPROJECT_ENTRYEXT_SEPARATOR_STR);

        if(pEntry->m_dwFlags & WBFILTER_ADPCM)
        {
            _tcscat(szEntryData, WBPROJECT_ENTRYEXT_ADPCMFILTER_STR);
        }

        if(pEntry->m_dwFlags & WBFILTER_8BIT)
        {
            _tcscat(szEntryData, WBPROJECT_ENTRYEXT_8BITFILTER_STR);
        }
    }

    if(!WritePrivateProfileString(pszSectionName, szKeyName, szEntryData, m_szProjectFile))
    {
        DPF_ERROR("Error %lu occurred writing to the project file", GetLastError());
        hr = E_FAIL;
    }        

    DPF_LEAVE_HRESULT(hr);

    return hr;
}


