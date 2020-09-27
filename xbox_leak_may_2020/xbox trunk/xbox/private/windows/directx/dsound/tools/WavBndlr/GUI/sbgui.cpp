/***************************************************************************
 *
 *  Copyright (C) 11/11/2001 Microsoft Corporation.  All Rights Reserved.
 *
 *  File:       sbgui.cpp
 *  Content:    GUI versions of wave bank objects.
 *  History:
 *   Date       By      Reason
 *   ====       ==      ======
 *  11/11/2001   dereks  Created.
 *
 ****************************************************************************/

#include "wbndgui.h"

#define MAKE_COMMDLG_FILTER(desc, ext) \
    desc " Files (" ext ")\0" ext "\0"

#define LVCOLUMN_ORDER_DEFAULT  TRUE


/****************************************************************************
 *
 *  FormatNumber
 *
 *  Description:
 *      Converts a number to a string, adding comma-separators.
 *
 *  Arguments:
 *      int [in]: number.
 *      BOOL [in]: signed or unsigned.
 *      BOOL [in]: add commas.
 *      LPSTR [out]: string.
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "FormatNumber"

void
FormatNumber
(
    int                     nValue,
    BOOL                    fSigned,
    BOOL                    fThousands,
    LPSTR                   pszString
)
{
    LPSTR                   psz     = pszString;
    UINT                    nDigits = 0;
    UINT                    uValue;
    
    if(nValue)
    {
        if(!fSigned || (nValue > 0))
        {
            uValue = (UINT)nValue;
            
            while(uValue)
            {
                *psz++ = '0' + (uValue % 10);

                uValue /= 10;

                if(uValue && fThousands)
                {
                    if(2 == (nDigits % 3))
                    {
                        *psz++ = ',';
                    }
                }

                nDigits++;
            }
        }
        else
        {
            uValue = abs(nValue);
            
            while(uValue)
            {
                *psz++ = '0' + (uValue % 10);

                uValue /= 10;

                if(uValue && fThousands)
                {
                    if(2 == (nDigits % 3))
                    {
                        *psz++ = ',';
                    }
                }

                nDigits++;
            }

            *psz++ = '-';
        }
    }
    else
    {
        *psz++ = '0';
    }

    *psz = 0;

    _strrev(pszString);
}


/****************************************************************************
 *
 *  CGuiWaveBankEntry
 *
 *  Description:
 *      Object constructor.
 *
 *  Arguments:
 *      CGuiWaveBank * [in]: parent object.
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CGuiWaveBankEntry::CGuiWaveBankEntry"

LVCOLUMNDATA CGuiWaveBankEntry::m_aColumnData[] =
{
    { "Entry Name", TRUE, LVCOLUMN_ORDER_DEFAULT },
    { "Format", TRUE, LVCOLUMN_ORDER_DEFAULT },
    { "Rate", FALSE, LVCOLUMN_ORDER_DEFAULT },
    { "Bits", FALSE, LVCOLUMN_ORDER_DEFAULT },
    { "Channels", FALSE, LVCOLUMN_ORDER_DEFAULT },
    { "Size", FALSE, LVCOLUMN_ORDER_DEFAULT },
    { "Conversion", TRUE, LVCOLUMN_ORDER_DEFAULT },
    { "Source File", TRUE, LVCOLUMN_ORDER_DEFAULT }
};

CGuiWaveBankEntry::CGuiWaveBankEntry
(
    CGuiWaveBank *         pParent
)
:   CWaveBankEntry(pParent)
{
    m_pParent = pParent;
    m_hIcon = NULL;
}


/****************************************************************************
 *
 *  ~CGuiWaveBankEntry
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
#define DPF_FNAME "CGuiWaveBankEntry::~CGuiWaveBankEntry"

CGuiWaveBankEntry::~CGuiWaveBankEntry
(
    void
)
{
    if(m_hIcon)
    {
        DestroyIcon(m_hIcon);
    }
}


/****************************************************************************
 *
 *  Initialize
 *
 *  Description:
 *      Initializes the object from a wave file.
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
#define DPF_FNAME "CGuiWaveBankEntry::Initialize"

HRESULT
CGuiWaveBankEntry::Initialize
(
    LPCSTR                  pszEntryName,
    LPCSTR                  pszFileName,
    DWORD                   dwFlags
)
{
    SHFILEINFO              shfi;
    HRESULT                 hr;
    
    //
    // Initialize the base class
    //
    
    hr = CWaveBankEntry::Initialize(pszEntryName, pszFileName, dwFlags);

    //
    // Get the icon information for display
    //

    if(SUCCEEDED(hr))
    {
        ASSERT(!m_hIcon);

        if(SHGetFileInfo(pszFileName, 0, &shfi, sizeof(shfi), SHGFI_ICON))
        {
            m_hIcon = shfi.hIcon;
        }
    }

    //
    // Update display data
    //

    if(SUCCEEDED(hr))
    {
        UpdateListView();
    }
    
    //
    // Update the entry count in the bank list
    //

    if(SUCCEEDED(hr))
    {
        m_pParent->UpdateListView();
    }

    return hr;
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
#define DPF_FNAME "CGuiWaveBankEntry::SetFlags"

DWORD
CGuiWaveBankEntry::SetFlags
(
    DWORD                   dwMask,
    DWORD                   dwFlags
)
{
    //
    // Hand off to the base class
    //

    dwFlags = CWaveBankEntry::SetFlags(dwMask, dwFlags);

    //
    // Update the parent bank's list-view data since our size may have
    // changed
    //

    m_pParent->UpdateListView();

    return dwFlags;
}


/****************************************************************************
 *
 *  GetItemIndex
 *
 *  Description:
 *      Gets the object's list view item index.
 *
 *  Arguments:
 *      (void)
 *
 *  Returns:  
 *      UINT: item index or -1.
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CGuiWaveBankEntry::GetItemIndex"

UINT
CGuiWaveBankEntry::GetItemIndex
(
    void
)
{
    HWND                    hwnd        = m_pParent->m_pParent->m_hwndEntryList;
    int                     nItemCount;
    int                     nItem;

    if(!hwnd)
    {
        return -1;
    }
    
    nItemCount = ListView_GetItemCount(hwnd);

    for(nItem = 0; nItem < nItemCount; nItem++)
    {
        if(this == GetEntry(hwnd, nItem))
        {
            break;
        }
    }

    return (nItem < nItemCount) ? nItem : -1;
}


/****************************************************************************
 *
 *  UpdateListView
 *
 *  Description:
 *      Updates the list-view item data.
 *
 *  Arguments:
 *      (void)
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CGuiWaveBankEntry::UpdateListView"

void
CGuiWaveBankEntry::UpdateListView
(
    UINT                    nItem
)
{
    HWND                    hwnd            = m_pParent->m_pParent->m_hwndEntryList;
    HIMAGELIST              himl            = m_pParent->m_pParent->m_himlEntryList;
    UINT                    nSubItem        = 0;
    DWORD                   dwOffset        = 0;
    WAVEBANKENTRY           MetaData;
    CHAR                    szText[0x100];
    LVITEM                  lvi;
    HRESULT                 hr;

    if(!hwnd)
    {
        return;
    }

    if(m_pParent != m_pParent->m_pParent->m_pActiveBank)
    {
        return;
    }

    if(-1 == nItem)
    {
        nItem = GetItemIndex();
    }

    if(-1 == nItem)
    {
        lvi.mask = LVIF_IMAGE | LVIF_PARAM;
        lvi.iItem = ListView_GetItemCount(hwnd);
        lvi.iSubItem = 0;
        lvi.iImage = ImageList_AddIcon(himl, m_hIcon);
        lvi.lParam = (LPARAM)this;

        nItem = ListView_InsertItem(hwnd, &lvi);
    }
    else
    {
        ASSERT(this == GetEntry(hwnd, nItem));
    }

    hr = GetMetaData(&MetaData, &dwOffset);

    if(FAILED(hr))
    {
        return;
    }

    ListView_SetItemText(hwnd, nItem, nSubItem++, m_szEntryName);

    if(WAVEBANKMINIFORMAT_TAG_ADPCM == m_Format.wFormatTag)
    {
        ListView_SetItemText(hwnd, nItem, nSubItem++, "ADPCM");
    }
    else
    {
        ListView_SetItemText(hwnd, nItem, nSubItem++, "PCM");
    }

    ListView_SetItemText(hwnd, nItem, nSubItem++, "%lu", m_Format.nSamplesPerSec);

    if(WAVEBANKMINIFORMAT_TAG_ADPCM == m_Format.wFormatTag)
    {
        ListView_SetItemText(hwnd, nItem, nSubItem++, "4");
    }
    else
    {
        ListView_SetItemText(hwnd, nItem, nSubItem++, "%lu", (WAVEBANKMINIFORMAT_BITDEPTH_8 == m_Format.wBitsPerSample) ? 8 : 16);
    }

    ListView_SetItemText(hwnd, nItem, nSubItem++, "%lu", m_Format.nChannels);
    
    FormatNumber(MetaData.PlayRegion.dwLength, FALSE, TRUE, szText);
    ListView_SetItemText(hwnd, nItem, nSubItem++, szText);
    
    if(m_dwFlags & WBFILTER_ADPCM)
    {
        strcpy(szText, CXboxAdpcmFilter::GetName());
    }
    else if(m_dwFlags & WBFILTER_8BIT)
    {
        strcpy(szText, C8BitFilter::GetName());
    }
    else
    {
        strcpy(szText, "");
    }
    
    ListView_SetItemText(hwnd, nItem, nSubItem++, szText);
    ListView_SetItemText(hwnd, nItem, nSubItem++, m_szFileName);
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
#define DPF_FNAME "CGuiWaveBankEntry::SetName"

HRESULT 
CGuiWaveBankEntry::SetName
(
    LPCSTR                  pszName
)
{
    HRESULT                 hr;

    if(!pszName)
    {
        pszName = "";
    }
    
    hr = CWaveBankEntry::SetName(pszName);

    if(SUCCEEDED(hr))
    {
        UpdateListView();
    }

    return hr;
}


/****************************************************************************
 *
 *  Audition
 *
 *  Description:
 *      Auditions the entry on the target Xbox console.
 *
 *  Arguments:
 *      PDM_CONNECTION [in]: XBDM connection.
 *
 *  Returns:  
 *      HRESULT: COM result code.
 *
 ****************************************************************************/
 
#undef DPF_FNAME
#define DPF_FNAME "CGuiWaveBankEntry::Audition"

HRESULT 
CGuiWaveBankEntry::Audition
(
    PDM_CONNECTION          pxbdm
)
{
    static const LPCSTR     pszCommandTemplate      = "XAUD!WaveBank.PlayEntry \"%s\"";
    static const LPCSTR     pszDstPathTemplate      = "T:\\%s";
    static const DWORD      dwCopyBufferSize        = 256 * 1024;
    DWORD                   dwOffset                = 0;
    LPVOID                  pvCopyBuffer            = NULL;
    HRESULT                 hr                      = S_OK;
    CHAR                    szCurDir[MAX_PATH];
    CHAR                    szSrcFile[MAX_PATH];
    CHAR                    szDstFile[MAX_PATH];
    CHAR                    szFileTitle[MAX_PATH];
    CHAR                    szCommand[0x400];
    CStdFileStream          DataFile;
    WAVEBANKENTRY           Entry;

    //
    // Open a temporary file
    //

    GetCurrentDirectory(NUMELMS(szCurDir), szCurDir);
    GetTempFileName(szCurDir, "WBN", 0, szSrcFile);

    hr = DataFile.Open(szSrcFile, GENERIC_READ | GENERIC_WRITE, 0, CREATE_ALWAYS, FILE_ATTRIBUTE_TEMPORARY | FILE_FLAG_SEQUENTIAL_SCAN);

    //
    // Get entry meta-data and write it to the file
    //

    if(SUCCEEDED(hr))
    {
        hr = GetMetaData(&Entry, &dwOffset);
    }

    if(SUCCEEDED(hr))
    {
        hr = DataFile.Write(&Entry, sizeof(Entry));
    }
    
    //
    // Allocate the copy buffer
    //

    if(SUCCEEDED(hr))
    {
        hr = HRFROMP(pvCopyBuffer = MEMALLOC(BYTE, dwCopyBufferSize));
    }

    //
    // Get wave data
    //

    if(SUCCEEDED(hr))
    {
        hr = CommitWaveData(&DataFile, sizeof(Entry), &Entry, pvCopyBuffer, dwCopyBufferSize);
    }

    //
    // Close the file so XBDM can read it
    //

    if(SUCCEEDED(hr))
    {
        DataFile.Close();
    }

    //
    // Send the file to the Xbox
    //

    if(SUCCEEDED(hr))
    {
        sprintf(szDstFile, pszDstPathTemplate, m_pParent->m_szBankName);

        DmMkdir(szDstFile);
    }
    
    if(SUCCEEDED(hr))
    {
        _splitpath(m_szFileName, NULL, NULL, szFileTitle, NULL);
        
        strcat(szDstFile, "\\");
        strcat(szDstFile, szFileTitle);
        
        if(XBDM_NOERR == (hr = DmSendFile(szSrcFile, szDstFile)))
        {
            hr = S_OK;
        }
        else
        {
            DPF_INFO("DmSendFile failed with error %x", hr);
            DPF_ERROR("Unable to transfer data to the Xbox");
        }
    }

    //
    // Tell the Xbox to play the file
    //

    if(SUCCEEDED(hr))
    {
        sprintf(szCommand, pszCommandTemplate, szDstFile);
        
        if(XBDM_NOERR == (hr = DmSendCommand(pxbdm, szCommand, NULL, NULL)))
        {
            hr = S_OK;
        }
        else
        {
            DPF_INFO("DmSendCommand failed with error %x", hr);
            DPF_ERROR("Unable to send command to the Xbox");
        }
    }

    //
    // Clean up
    //

    DeleteFile(szSrcFile);
    
    MEMFREE(pvCopyBuffer);

    return hr;
}


/****************************************************************************
 *
 *  CGuiWaveBank
 *
 *  Description:
 *      Object constructor.
 *
 *  Arguments:
 *      CGuiProject * [in]: parent object.
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CGuiWaveBank::CGuiWaveBank"

LVCOLUMNDATA CGuiWaveBank::m_aColumnData[] =
{
    { "Bank Name", TRUE, LVCOLUMN_ORDER_DEFAULT },
    { "Entry Count", FALSE, LVCOLUMN_ORDER_DEFAULT },
    { "Bank Size", FALSE, LVCOLUMN_ORDER_DEFAULT },
    { "Bank File", TRUE, LVCOLUMN_ORDER_DEFAULT },
    { "Header File", TRUE, LVCOLUMN_ORDER_DEFAULT }
};

CGuiWaveBank::CGuiWaveBank
(
    CGuiWaveBankProject *  pParent
)
:   CWaveBank(pParent)
{
    m_pParent = pParent;
}


/****************************************************************************
 *
 *  ~CGuiWaveBank
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
#define DPF_FNAME "CGuiWaveBank::~CGuiWaveBank"

CGuiWaveBank::~CGuiWaveBank
(
    void
)
{
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
#define DPF_FNAME "CGuiWaveBank::Initialize"

HRESULT
CGuiWaveBank::Initialize
(
    LPCSTR                  pszBankName,
    LPCSTR                  pszBankFile,
    LPCSTR                  pszHeaderFile
)
{
    HRESULT                 hr;

    //
    // Initialize the base class
    //

    hr = CWaveBank::Initialize(pszBankName, pszBankFile, pszHeaderFile);

    //
    // Update display data
    //

    if(SUCCEEDED(hr))
    {
        UpdateListView();
    }

    return hr;
}


/****************************************************************************
 *
 *  Refresh
 *
 *  Description:
 *      Refreshes bank data.
 *
 *  Arguments:
 *      (void)
 *
 *  Returns:  
 *      HRESULT: COM result code.
 *
 ****************************************************************************/
 
#undef DPF_FNAME
#define DPF_FNAME "CGuiWaveBank::Refresh"

HRESULT
CGuiWaveBank::Refresh
(
    void
)
{
    HRESULT                 hr          = S_OK;
    PLIST_ENTRY             pleEntry;
    CGuiWaveBankEntry *     pEntry;

    //
    // Refresh entries
    //

    for(pleEntry = m_lstEntries.Flink; (pleEntry != &m_lstEntries) && SUCCEEDED(hr); pleEntry = pleEntry->Flink)
    {
        pEntry = CONTAINING_RECORD(pleEntry, CGuiWaveBankEntry, m_leEntry);

        hr = pEntry->Refresh();
    }

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
#define DPF_FNAME "CGuiWaveBank::SetName"

HRESULT 
CGuiWaveBank::SetName
(
    LPCSTR                  pszName
)
{
    HRESULT                 hr;

    if(!pszName)
    {
        pszName = "";
    }
    
    hr = CWaveBank::SetName(pszName);

    if(SUCCEEDED(hr))
    {
        UpdateListView();
    }

    return hr;
}


/****************************************************************************
 *
 *  GetItemIndex
 *
 *  Description:
 *      Gets the object's list view item index.
 *
 *  Arguments:
 *      (void)
 *
 *  Returns:  
 *      UINT: item index or -1.
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CGuiWaveBank::GetItemIndex"

UINT
CGuiWaveBank::GetItemIndex
(
    void
)
{
    HWND                    hwnd        = m_pParent->m_hwndBankList;
    int                     nItemCount;
    int                     nItem;

    if(!hwnd)
    {
        return -1;
    }
    
    nItemCount = ListView_GetItemCount(hwnd);

    for(nItem = 0; nItem < nItemCount; nItem++)
    {
        if(this == GetBank(hwnd, nItem))
        {
            break;
        }
    }

    return (nItem < nItemCount) ? nItem : -1;
}


/****************************************************************************
 *
 *  AddEntry
 *
 *  Description:
 *      Adds an entry to the bank.
 *
 *  Arguments:
 *      (void)
 *
 *  Returns:  
 *      HRESULT: COM result code.
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CGuiWaveBank::AddEntry"

HRESULT
CGuiWaveBank::AddEntry
(
    void
)
{
    static const LPCSTR     pszFilter           = MAKE_COMMDLG_FILTER("Sound", "*.wav;*.aif;*.aiff") MAKE_COMMDLG_FILTER("All", "*.*");
    static const LPCSTR     pszTitle            = "Add Wave Bank Entries";
    static const DWORD      dwOfnFlags          = OFN_ENABLESIZING | OFN_EXPLORER | OFN_HIDEREADONLY | OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST | OFN_ALLOWMULTISELECT;
    static const UINT       nMaxFiles           = 2048;
    LPSTR                   pszPath;
    LPSTR                   pszFile;
    CHAR                    szFile[MAX_PATH];
    HRESULT                 hr;
    OPENFILENAME            ofn;

    //
    // Allocate enough space for more than one file
    //

    hr = HRFROMP(pszPath = MEMALLOC(CHAR, nMaxFiles * MAX_PATH));

    //
    // Get the file name(s)
    //

    if(SUCCEEDED(hr))
    {
        ZeroMemory(&ofn, sizeof(ofn));

        ofn.lStructSize = sizeof(ofn);
        ofn.hwndOwner = m_pParent->m_pParent->m_hwnd;
        ofn.lpstrFilter = pszFilter;
        ofn.lpstrFile = pszPath;
        ofn.nMaxFile = nMaxFiles * MAX_PATH;
        ofn.lpstrTitle = pszTitle;
        ofn.Flags = dwOfnFlags;
        ofn.lpstrDefExt = WBWRITER_FILEEXTENSION_BANK;

        if(!GetOpenFileName(&ofn))
        {
            hr = E_ABORT;
        }
    }

    //
    // If more than one file was specified, the path will be first, followed
    // by each file name in a null-terminated list
    //

    if(SUCCEEDED(hr))
    {
        pszFile = pszPath + strlen(pszPath) + 1;

        if(*pszFile)
        {
            while(*pszFile)
            {
                _makepath(szFile, NULL, pszPath, pszFile, NULL);

                if(FAILED(CWaveBank::AddEntry(NULL, szFile, 0)))
                {
                    hr = S_FALSE;
                }

                pszFile += strlen(pszFile) + 1;
            }
        }
        else
        {
            hr = CWaveBank::AddEntry(NULL, pszPath, 0);
        }
    }

    //
    // Display a warning if not all of the entries were added
    //

    if(S_FALSE == hr)
    {
        DPF_WARNING("Not all entries were added to the wave bank");
    }

    //
    // Clean up
    //

    MEMFREE(pszPath);

    return hr;
}


/****************************************************************************
 *
 *  UpdateListView
 *
 *  Description:
 *      Updates the list-view item data.
 *
 *  Arguments:
 *      (void)
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CGuiWaveBank::UpdateListView"

void
CGuiWaveBank::UpdateListView
(
    UINT                    nItem
)
{
    HWND                    hwnd            = m_pParent->m_hwndBankList;
    UINT                    nSubItem        = 0;
    CHAR                    szText[0x100];
    DWORD                   dwSize;
    LVITEM                  lvi;

    if(!hwnd)
    {
        return;
    }

    if(-1 == nItem)
    {
        nItem = GetItemIndex();
    }

    if(-1 == nItem)
    {
        lvi.mask = LVIF_IMAGE | LVIF_PARAM;
        lvi.iItem = ListView_GetItemCount(hwnd);
        lvi.iSubItem = 0;
        lvi.iImage = m_pParent->m_nBankIcon;
        lvi.lParam = (LPARAM)this;

        nItem = ListView_InsertItem(hwnd, &lvi);
    }
    else
    {
        ASSERT(this == GetBank(hwnd, nItem));
    }

    dwSize = GetBankDataSize();

    ListView_SetItemText(hwnd, nItem, nSubItem++, m_szBankName);
    ListView_SetItemText(hwnd, nItem, nSubItem++, "%lu", m_dwEntryCount);

    FormatNumber(dwSize, FALSE, TRUE, szText);
    ListView_SetItemText(hwnd, nItem, nSubItem++, szText);

    ListView_SetItemText(hwnd, nItem, nSubItem++, m_szBankFile);
    ListView_SetItemText(hwnd, nItem, nSubItem++, m_szHeaderFile);
}


/****************************************************************************
 *
 *  SortEntryList
 *
 *  Description:
 *      Sorts the entry list.
 *
 *  Arguments:
 *      UINT [in]: column index to sort by.
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CGuiWaveBank::SortEntryList"

void
CGuiWaveBank::SortEntryList
(
    UINT                    nColumn
)
{
    const LPLVCOLUMNDATA    pColumn     = &CGuiWaveBankEntry::m_aColumnData[nColumn];
    PLIST_ENTRY             pleEntry;
    CGuiWaveBankEntry *     pEntry;
    int                     nItemCount;
    int                     nItem;

    ASSERT(nColumn < NUMELMS(CGuiWaveBankEntry::m_aColumnData));

    //
    // Let the list-view sort the list
    //

    ListView_SortItems(m_pParent->m_hwndEntryList, nColumn, pColumn->fString, pColumn->fAscending, NULL, NULL);

    //
    // Rebuild the list based on the list-view data
    //

    while(&m_lstEntries != RemoveHeadList(&m_lstEntries));

    nItemCount = ListView_GetItemCount(m_pParent->m_hwndEntryList);
    
    for(nItem = 0; nItem < nItemCount; nItem++)
    {
        pEntry = CGuiWaveBankEntry::GetEntry(m_pParent->m_hwndEntryList, nItem);
        ASSERT(pEntry);

        InsertTailList(&m_lstEntries, &pEntry->m_leEntry);
    }

    //
    // Swap the ascending flag
    //

    pColumn->fAscending = !pColumn->fAscending;
}


/****************************************************************************
 *
 *  CGuiWaveBankProject
 *
 *  Description:
 *      Object constructor.
 *
 *  Arguments:
 *      CWindow * [in]: parent window.
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CGuiWaveBankProject::CGuiWaveBankProject"

CGuiWaveBankProject::CGuiWaveBankProject
(
    CWindow *               pParent
)
{
    m_pParent = pParent;
    m_hwndBankList = NULL;
    m_hwndEntryList = NULL;
    m_pActiveBank = NULL;
    m_hBankIcon = NULL;
    m_nBankIcon = -1;

    m_szProjectFile[0] = 0;
}


/****************************************************************************
 *
 *  ~CGuiWaveBankProject
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
#define DPF_FNAME "CGuiWaveBankProject::~CGuiWaveBankProject"

CGuiWaveBankProject::~CGuiWaveBankProject
(
    void
)
{
    //
    // Clear all items from the lists
    //

    Detach();

    //
    // Free resources
    //
    
    if(m_hBankIcon)
    {
        DestroyIcon(m_hBankIcon);
    }
}


/****************************************************************************
 *
 *  AddBank
 *
 *  Description:
 *      Adds a new bank to the project.
 *
 *  Arguments:
 *      (void)
 *
 *  Returns:  
 *      HRESULT: COM result code.
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CGuiWaveBankProject::AddBank"

HRESULT
CGuiWaveBankProject::AddBank
(
    void
)
{
    static const LPCSTR     pszSbkFilter        = MAKE_COMMDLG_FILTER("Wave Bank", "*." WBWRITER_FILEEXTENSION_BANK) MAKE_COMMDLG_FILTER("All", "*.*");
    static const LPCSTR     pszHdrFilter        = MAKE_COMMDLG_FILTER("Header", "*." WBWRITER_FILEEXTENSION_HEADER) MAKE_COMMDLG_FILTER("All", "*.*");
    static const LPCSTR     pszSbkTitle         = "Create Wave Bank File";
    static const LPCSTR     pszHdrTitle         = "Create Header File";
    static const DWORD      dwOfnFlags          = OFN_ENABLESIZING | OFN_EXPLORER | OFN_HIDEREADONLY | OFN_PATHMUSTEXIST;
    CGuiWaveBank *          pBank               = NULL;
    HRESULT                 hr                  = S_OK;
    CHAR                    szSbkPath[MAX_PATH];
    CHAR                    szHdrPath[MAX_PATH];
    CHAR                    szDrive[MAX_PATH];
    CHAR                    szDir[MAX_PATH];
    CHAR                    szFile[MAX_PATH];
    CHAR                    szExt[MAX_PATH];
    PLIST_ENTRY             pleBank;
    OPENFILENAME            ofn;

    szSbkPath[0] = 0;
    szHdrPath[0] = 0;
    
    //
    // Get the wave bank file path
    //

    while(SUCCEEDED(hr))
    {
        ZeroMemory(&ofn, sizeof(ofn));

        ofn.lStructSize = sizeof(ofn);
        ofn.hwndOwner = m_pParent->m_hwnd;
        ofn.lpstrFilter = pszSbkFilter;
        ofn.lpstrFile = szSbkPath;
        ofn.nMaxFile = NUMELMS(szSbkPath);
        ofn.lpstrTitle = pszSbkTitle;
        ofn.Flags = dwOfnFlags;
        ofn.lpstrDefExt = WBWRITER_FILEEXTENSION_BANK;

        if(!GetSaveFileName(&ofn))
        {
            hr = E_ABORT;
        }

        if(SUCCEEDED(hr))
        {
            for(pleBank = m_lstBanks.Flink; pleBank != &m_lstBanks; pleBank = pleBank->Flink)
            {
                pBank = CONTAINING_RECORD(pleBank, CGuiWaveBank, m_leBank);

                if(!_stricmp(szSbkPath, pBank->m_szBankFile))
                {
                    break;
                }
            }

            if(pleBank == &m_lstBanks)
            {
                pBank = NULL;
                break;
            }
            else
            {
                m_pParent->MessageBox("That wave bank file already exists in the project", MB_OK | MB_ICONSTOP);
            }
        }
    }
    
    //
    // Get the header file path
    //
    
    while(SUCCEEDED(hr))
    {
        _splitpath(szSbkPath, szDrive, szDir, szFile, szExt);
        _makepath(szHdrPath, szDrive, szDir, szFile, "." WBWRITER_FILEEXTENSION_HEADER);

        ZeroMemory(&ofn, sizeof(ofn));

        ofn.lStructSize = sizeof(ofn);
        ofn.hwndOwner = m_pParent->m_hwnd;
        ofn.lpstrFilter = pszHdrFilter;
        ofn.lpstrFile = szHdrPath;
        ofn.nMaxFile = NUMELMS(szHdrPath);
        ofn.lpstrTitle = pszHdrTitle;
        ofn.Flags = dwOfnFlags;
        ofn.lpstrDefExt = WBWRITER_FILEEXTENSION_HEADER;

        if(!GetSaveFileName(&ofn))
        {
            hr = E_ABORT;
        }

        if(SUCCEEDED(hr))
        {
            for(pleBank = m_lstBanks.Flink; pleBank != &m_lstBanks; pleBank = pleBank->Flink)
            {
                pBank = CONTAINING_RECORD(pleBank, CGuiWaveBank, m_leBank);

                if(!_stricmp(szHdrPath, pBank->m_szHeaderFile))
                {
                    break;
                }
            }

            if(pleBank == &m_lstBanks)
            {
                pBank = NULL;
                break;
            }
            else
            {
                m_pParent->MessageBox("That header file already exists in the project", MB_OK | MB_ICONSTOP);
            }
        }
    }

    //
    // Create the wave bank object
    //

    if(SUCCEEDED(hr))
    {
        hr = CWaveBankProject::AddBank(szFile, szSbkPath, szHdrPath, (CWaveBank **)&pBank);
    }

    //
    // Resize the list-view columns
    //

    if(SUCCEEDED(hr))
    {
        ListView_AutoSizeColumns(m_hwndBankList);
    }

    //
    // Activate the new bank
    //
    
    if(SUCCEEDED(hr))
    {
        ListView_SelectItem(m_hwndBankList, ListView_GetItemCount(m_hwndBankList) - 1, TRUE, TRUE);
    }

    //
    // Set focus to the bank list
    //

    if(SUCCEEDED(hr))
    {
        SetFocus(m_hwndBankList);
    }

    //
    // Clean up
    //

    if(FAILED(hr))
    {
        DELETE(pBank);
    }

    return hr;
}


/****************************************************************************
 *
 *  RemoveBank
 *
 *  Description:
 *      Removes a bank from the project.
 *
 *  Arguments:
 *      UINT [in]: bank index.
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CGuiWaveBankProject::RemoveBank"

void
CGuiWaveBankProject::RemoveBank
(
    UINT                    nBankIndex
)
{
    CGuiWaveBank *          pBank;

    //
    // Get the bank pointer
    //

    pBank = CGuiWaveBank::GetBank(m_hwndBankList, nBankIndex);
    
    //
    // Remove the item from the list view
    //

    ListView_DeleteItem(m_hwndBankList, nBankIndex);

    // 
    // Reset the current bank selection and remove it from the project
    //

    if(pBank)
    {
        if(pBank == m_pActiveBank)
        {
            SelectBank(-1);
        }

        CWaveBankProject::RemoveBank(pBank);
    }
}


/****************************************************************************
 *
 *  RemoveEntry
 *
 *  Description:
 *      Removes an entry from the project.
 *
 *  Arguments:
 *      UINT [in]: entry index.
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CGuiWaveBankProject::RemoveEntry"

void
CGuiWaveBankProject::RemoveEntry
(
    UINT                    nEntryIndex
)
{
    CGuiWaveBankEntry *     pEntry;

    //
    // Get the entry pointer
    //

    pEntry = CGuiWaveBankEntry::GetEntry(m_hwndEntryList, nEntryIndex);
    
    //
    // Remove the item from the list view
    //

    ListView_DeleteItem(m_hwndEntryList, nEntryIndex);

    //
    // Remove the entry from the project and update the UI
    //

    if(pEntry && m_pActiveBank)
    {
        m_pActiveBank->RemoveEntry(pEntry);
        m_pActiveBank->UpdateListView();
    }
}


/****************************************************************************
 *
 *  SelectBank
 *
 *  Description:
 *      Sets the currently active bank.
 *
 *  Arguments:
 *      CGuiWaveBank * [in]: bank.
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CGuiWaveBankProject::SelectBank"

void
CGuiWaveBankProject::SelectBank
(
    UINT                    nItem
)
{
    CGuiWaveBank *          pBank   = CGuiWaveBank::GetBank(m_hwndBankList, nItem);
    UINT                    i;
    
    if(pBank != m_pActiveBank)
    {
        //
        // Clear the entry list
        //
        
        ClearEntryList();

        //
        // Select the bank in the bank list
        //

        if(m_pActiveBank = pBank)
        {
            ListView_SelectItem(m_hwndBankList, nItem, TRUE, TRUE);
            SetFocus(m_hwndBankList);
        }

        //
        // Refill the entry list

        FillEntryList();

        //
        // Resize the entry list columns
        //

        ListView_AutoSizeColumns(m_hwndEntryList);

        //
        // Reset the entry-list sort data
        //

        for(i = 0; i < NUMELMS(CGuiWaveBankEntry::m_aColumnData); i++)
        {
            CGuiWaveBankEntry::m_aColumnData[i].fAscending = LVCOLUMN_ORDER_DEFAULT;
        }
    }
}


/****************************************************************************
 *
 *  ClearEntryList
 *
 *  Description:
 *      Removes all items from the entry list.
 *
 *  Arguments:
 *      (void)
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CGuiWaveBankProject::ClearEntryList"

void
CGuiWaveBankProject::ClearEntryList
(
    void
)
{
    ListView_DeleteAllItems(m_hwndEntryList);

    ImageList_RemoveAll(m_himlEntryList);
}


/****************************************************************************
 *
 *  FillEntryList
 *
 *  Description:
 *      Adds all entries in the current bank to the entry list-view.
 *
 *  Arguments:
 *      (void)
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CGuiWaveBankProject::FillEntryList"

void
CGuiWaveBankProject::FillEntryList
(
    void
)
{
    PLIST_ENTRY             pleEntry;
    CGuiWaveBankEntry *    pEntry;
    
    if(m_pActiveBank)
    {
        if(m_pActiveBank->m_dwEntryCount)
        {
            for(pleEntry = m_pActiveBank->m_lstEntries.Flink; pleEntry != &m_pActiveBank->m_lstEntries; pleEntry = pleEntry->Flink)
            {
                pEntry = CONTAINING_RECORD(pleEntry, CGuiWaveBankEntry, m_leEntry);

                pEntry->UpdateListView();
            }
        }
    }
}


/****************************************************************************
 *
 *  Open
 *
 *  Description:
 *      Opens an existing wave bank project.
 *
 *  Arguments:
 *      LPCSTR [in]: optional project file path.
 *
 *  Returns:  
 *      HRESULT: COM result code.
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CGuiWaveBankProject::Open"

HRESULT
CGuiWaveBankProject::Open
(
    LPCSTR                  pszProjectFile
)
{
    static const LPCSTR     pszFilter   = MAKE_COMMDLG_FILTER("Project", "*." WBWRITER_FILEEXTENSION_PROJECT) MAKE_COMMDLG_FILTER("All", "*.*");
    static const LPCSTR     pszTitle    = "Open Project File";
    static const DWORD      dwOfnFlags  = OFN_ENABLESIZING | OFN_EXPLORER | OFN_HIDEREADONLY | OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST | OFN_ALLOWMULTISELECT;
    HRESULT                 hr          = S_OK;
    OPENFILENAME            ofn;

    //
    // Open the file
    //

    if(pszProjectFile)
    {
        strcpy(m_szProjectFile, pszProjectFile);
    }
    else
    {
        ZeroMemory(&ofn, sizeof(ofn));

        ofn.lStructSize = sizeof(ofn);
        ofn.hwndOwner = m_pParent->m_hwnd;
        ofn.lpstrFilter = pszFilter;
        ofn.lpstrFile = m_szProjectFile;
        ofn.nMaxFile = NUMELMS(m_szProjectFile);
        ofn.lpstrTitle = pszTitle;
        ofn.Flags = dwOfnFlags;
        ofn.lpstrDefExt = WBWRITER_FILEEXTENSION_PROJECT;

        if(!GetOpenFileName(&ofn))
        {
            hr = E_ABORT;
        }
    }

    if(SUCCEEDED(hr))
    {
        SetCursor(LoadCursor(NULL, IDC_WAIT));
        
        hr = ReadProjectData(m_szProjectFile);

        SetCursor(LoadCursor(NULL, IDC_ARROW));
    }

    //
    // Resize the list-view columns
    //

    if(SUCCEEDED(hr))
    {
        ListView_AutoSizeColumns(m_hwndBankList);
    }

    //
    // Select the first bank in the list
    //

    if(SUCCEEDED(hr))
    {
        SelectBank(0);
    }

    return hr;
}


/****************************************************************************
 *
 *  Save
 *
 *  Description:
 *      Saves the wave bank project.
 *
 *  Arguments:
 *      BOOL [in]: TRUE to prompt for the file name.
 *
 *  Returns:  
 *      HRESULT: COM result code.
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CGuiWaveBankProject::Save"

HRESULT
CGuiWaveBankProject::Save
(
    BOOL                    fPrompt
)
{
    static const LPCSTR     pszFilter   = MAKE_COMMDLG_FILTER("Project", "*." WBWRITER_FILEEXTENSION_PROJECT) MAKE_COMMDLG_FILTER("All", "*.*");
    static const LPCSTR     pszTitle    = "Save Project File";
    static const DWORD      dwOfnFlags  = OFN_ENABLESIZING | OFN_EXPLORER | OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_PATHMUSTEXIST;
    HRESULT                 hr          = S_OK;
    OPENFILENAME            ofn;

    if(!strlen(m_szProjectFile))
    {
        fPrompt = TRUE;
    }

    if(fPrompt)
    {
        ZeroMemory(&ofn, sizeof(ofn));

        ofn.lStructSize = sizeof(ofn);
        ofn.hwndOwner = m_pParent->m_hwnd;
        ofn.lpstrFilter = pszFilter;
        ofn.lpstrFile = m_szProjectFile;
        ofn.nMaxFile = NUMELMS(m_szProjectFile);
        ofn.lpstrTitle = pszTitle;
        ofn.Flags = dwOfnFlags;
        ofn.lpstrDefExt = WBWRITER_FILEEXTENSION_PROJECT;

        if(!GetSaveFileName(&ofn))
        {
            hr = E_ABORT;
        }
    }

    if(SUCCEEDED(hr))
    {
        hr = WriteProjectData(m_szProjectFile);
    }

    return hr;
}


/****************************************************************************
 *
 *  Attach
 *
 *  Description:
 *      Attaches the object to the list-view windows.
 *
 *  Arguments:
 *      HWND [in]: bank list-view.
 *      HWND [in]: entry list-view.
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CGuiWaveBankProject::Attach"

void
CGuiWaveBankProject::Attach
(
    HWND                    hwndBankList,
    HWND                    hwndEntryList
)
{
    HIMAGELIST              himlOld;
    
    m_hwndBankList = hwndBankList;
    m_hwndEntryList = hwndEntryList;

    //
    // Create image lists
    //

    if(m_himlBankList = ImageList_Create(16, 16, ILC_COLOR8 | ILC_MASK, 1, 0))
    {
        if(m_hBankIcon = (HICON)LoadImage(g_hInstance, MAKEINTRESOURCE(IDI_BANK), IMAGE_ICON, 16, 16, 0))
        {
            m_nBankIcon = ImageList_AddIcon(m_himlBankList, m_hBankIcon);
        }

        if(himlOld = ListView_SetImageList(m_hwndBankList, m_himlBankList, LVSIL_SMALL))
        {
            ImageList_Destroy(himlOld);
        }
    }

    if(m_himlEntryList = ImageList_Create(16, 16, ILC_COLOR8 | ILC_MASK, 1, -1))
    {
        if(himlOld = ListView_SetImageList(m_hwndEntryList, m_himlEntryList, LVSIL_SMALL))
        {
            ImageList_Destroy(himlOld);
        }
    }
}


/****************************************************************************
 *
 *  Detach
 *
 *  Description:
 *      Attaches the object from the list-view windows.
 *
 *  Arguments:
 *      BOOL [in]: TRUE to clear the lists.
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CGuiWaveBankProject::Detach"

void
CGuiWaveBankProject::Detach
(
    BOOL                    fClearList
)
{
    if(fClearList)
    {
        ListView_DeleteAllItems(m_hwndBankList);
        ListView_DeleteAllItems(m_hwndEntryList);
    }

    m_hwndBankList = NULL;
    m_hwndEntryList = NULL;
}


/****************************************************************************
 *
 *  SortBankList
 *
 *  Description:
 *      Sorts the entry list.
 *
 *  Arguments:
 *      UINT [in]: column index to sort by.
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CGuiWaveBank::SortBankList"

void
CGuiWaveBankProject::SortBankList
(
    UINT                    nColumn
)
{
    const LPLVCOLUMNDATA    pColumn     = &CGuiWaveBank::m_aColumnData[nColumn];
    PLIST_ENTRY             pleBank;
    CGuiWaveBank *          pBank;
    int                     nItemCount;
    int                     nItem;

    ASSERT(nColumn < NUMELMS(CGuiWaveBank::m_aColumnData));

    //
    // Let the list-view sort the list
    //

    ListView_SortItems(m_hwndBankList, nColumn, pColumn->fString, pColumn->fAscending, NULL, NULL);

    //
    // Rebuild the list based on the list-view data
    //

    while(&m_lstBanks != RemoveHeadList(&m_lstBanks));

    nItemCount = ListView_GetItemCount(m_hwndBankList);
    
    for(nItem = 0; nItem < nItemCount; nItem++)
    {
        pBank = CGuiWaveBank::GetBank(m_hwndBankList, nItem);
        ASSERT(pBank);

        InsertTailList(&m_lstBanks, &pBank->m_leBank);
    }

    //
    // Swap the ascending flag
    //

    pColumn->fAscending = !pColumn->fAscending;
}


/****************************************************************************
 *
 *  AddEntry
 *
 *  Description:
 *      Adds an entry to the active bank.
 *
 *  Arguments:
 *      (void)
 *
 *  Returns:  
 *      HRESULT: COM result code.
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CGuiWaveBankProject::AddEntry"

HRESULT
CGuiWaveBankProject::AddEntry
(
    void
)
{
    HRESULT                 hr;

    if(!m_pActiveBank)
    {
        return E_FAIL;
    }

    //
    // Add the entry
    //

    hr = m_pActiveBank->AddEntry();

    //
    // Resize columns
    //

    if(SUCCEEDED(hr))
    {
        ListView_AutoSizeColumns(m_hwndEntryList);
    }

    return hr;
}


/****************************************************************************
 *
 *  Generate
 *
 *  Description:
 *      Generates project files.
 *
 *  Arguments:
 *      CWaveBankCallback * [in]: callback object.
 *      BOOL [in]: TRUE to allow file overwrites.
 *
 *  Returns:  
 *      HRESULT: COM result code.
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CGuiWaveBankProject::Generate"

HRESULT
CGuiWaveBankProject::Generate
(
    void
)
{
    HANDLE                  hThread     = NULL;
    HRESULT                 hr          = S_OK;
    DWORD                   dwThreadId;
    HWND                    hwnd;
    
    //
    // Create the dialog
    //

    if(!(hwnd = m_GenerateDialog.Create(this)))
    {
        hr = E_FAIL;
    }

    //
    // Spawn the generation thread
    //

    hThread = CreateThread(NULL, 0, GenerateThreadProc, (LPVOID)this, 0, &dwThreadId);
    
    if(!IS_VALID_HANDLE_VALUE(hThread))
    {
        hr = E_OUTOFMEMORY;
    }
    
    //
    // Clean up
    //

    CLOSE_HANDLE(hThread);

    if(FAILED(hr))
    {
        EndDialog(hwnd, hr);
    }

    return hr;
}


/****************************************************************************
 *
 *  GenerateThreadProc
 *
 *  Description:
 *      Generation thread proc.
 *
 *  Arguments:
 *      LPVOID [in]: thread context.
 *
 *  Returns:  
 *      DWORD: thread return code.
 *
 ****************************************************************************/
 
#undef DPF_FNAME
#define DPF_FNAME "CGuiWaveBankProject::GenerateThreadProc"

DWORD 
CGuiWaveBankProject::GenerateThreadProc
(
    LPVOID                  pvContext
)
{
    return ((CGuiWaveBankProject *)pvContext)->GenerateThread();
}


/****************************************************************************
 *
 *  GenerateThread
 *
 *  Description:
 *      Generation thread proc.
 *
 *  Arguments:
 *      (void)
 *
 *  Returns:  
 *      HRESULT: COM result code.
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CGuiWaveBankProject::GenerateThread"

HRESULT
CGuiWaveBankProject::GenerateThread
(
    void
)
{
    HRESULT                 hr;

    //
    // BUGBUG: we're not doing any thread synchronization because we're
    // relying on the fact that the parent window is disabled and so won't
    // be doing anything.  This probably isn't a good thing.
    //

    //
    // Start generation
    //

    hr = CWaveBankProject::Generate(&m_GenerateDialog, TRUE);

    //
    // Wait for the dialog to close
    //

    if(SUCCEEDED(hr))
    {
        WaitForSingleObject(m_GenerateDialog.m_hTerminateEvent, INFINITE);
    }

    return hr;
}


/****************************************************************************
 *
 *  Refresh
 *
 *  Description:
 *      Refreshes bank data.
 *
 *  Arguments:
 *      (void)
 *
 *  Returns:  
 *      HRESULT: COM result code.
 *
 ****************************************************************************/
 
#undef DPF_FNAME
#define DPF_FNAME "CGuiWaveBankProject::Refresh"

HRESULT
CGuiWaveBankProject::Refresh
(
    void
)
{
    HRESULT                 hr          = S_OK;
    PLIST_ENTRY             pleBank;
    CGuiWaveBank *          pBank;

    //
    // Refresh all banks
    //

    for(pleBank = m_lstBanks.Flink; (pleBank != &m_lstBanks) && SUCCEEDED(hr); pleBank = pleBank->Flink)
    {
        pBank = CONTAINING_RECORD(pleBank, CGuiWaveBank, m_leBank);

        hr = pBank->Refresh();
    }

    //
    // Update list-view data
    //

    if(m_pActiveBank)
    {
        ClearEntryList();
        FillEntryList();
        
        m_pActiveBank->UpdateListView();
    }

    return hr;
}


/****************************************************************************
 *
 *  CGenerateDialog
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
#define DPF_FNAME "CGenerateDialog::CGenerateDialog"

CGenerateDialog::CGenerateDialog
(
    void
)
{
    m_hTerminateEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
}


/****************************************************************************
 *
 *  ~CGenerateDialog
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
#define DPF_FNAME "CGenerateDialog::~CGenerateDialog"

CGenerateDialog::~CGenerateDialog
(
    void
)
{
    CLOSE_HANDLE(m_hTerminateEvent);
}


/****************************************************************************
 *
 *  Create
 *
 *  Description:
 *      Creates the dialog.
 *
 *  Arguments:
 *      CGuiWaveBankProject * [in]: project.
 *      CWindow * [in]: parent window.
 *
 *  Returns:  
 *      HWND: window handle.
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CGenerateDialog::Create"

HWND
CGenerateDialog::Create
(
    CGuiWaveBankProject *   pProject
)
{
    HWND                    hwnd;
    
    m_fContinue = TRUE;
    m_fAllowClose = FALSE;
    m_nEntryIndex = 0;

    //
    // Create the dialog
    //
    
    if(!(hwnd = CModelessDialog::Create(IDD, NULL, pProject->m_pParent)))
    {
        return NULL;
    }

    //
    // Disable the parent window
    //

    if(m_pParent)
    {
        EnableWindow(m_pParent->m_hwnd, FALSE);
    }

    return hwnd;
}


/****************************************************************************
 *
 *  OnInitDialog
 *
 *  Description:
 *      Handles dialog initialization.
 *
 *  Arguments:
 *      LPVOID [in]: creation context.
 *      LRESULT * [out]: message result code.
 *
 *  Returns:  
 *      BOOL: FALSE to allow the message to be passed to the default handler.
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CGenerateDialog::OnInitDialog"

BOOL
CGenerateDialog::OnInitDialog
(
    LPVOID                  pvContext,
    LRESULT *               plResult
)
{
    CHAR                    szTemplate[0x100];
    CHAR                    szText[0x100];
    BOOL                    fHandled;
    
    fHandled = CModelessDialog::OnInitDialog(pvContext, plResult);
    
    //
    // Save the text format and clear the controls
    //

    GetDlgItemText(m_hwnd, IDC_BANK, m_szBankText, NUMELMS(m_szBankText));
    SetDlgItemText(m_hwnd, IDC_BANK, NULL);

    GetDlgItemText(m_hwnd, IDC_ENTRY, m_szEntryText, NUMELMS(m_szEntryText));
    SetDlgItemText(m_hwnd, IDC_ENTRY, NULL);

    //
    // Set the dialog title
    //

    GetWindowText(m_hwnd, szTemplate, NUMELMS(szTemplate));
    sprintf(szText, szTemplate, g_pszAppTitle);
    SetWindowText(m_hwnd, szText);

    //
    // Center the window
    //

    CenterWindow(m_hwnd, m_pParent ? m_pParent->m_hwnd : NULL);

    return fHandled;
}


/****************************************************************************
 *
 *  OnCancel
 *
 *  Description:
 *      Handles closing the window.
 *
 *  Arguments:
 *      (void)
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/
 
#undef DPF_FNAME
#define DPF_FNAME "CGenerateDialog::OnCancel"

void 
CGenerateDialog::OnCancel
(
    void
)
{
    static const LPCSTR     pszCancelling   = "Cancelling...";

    //
    // Toggle the continue member to FALSE
    //
    
    m_fContinue = FALSE;

    //
    // If we're allowed to close, do it now.  If not, let the user know that
    // we're working on it.
    //

    if(m_fAllowClose)
    {
        SetEvent(m_hTerminateEvent);
        EnableWindow(m_pParent->m_hwnd, TRUE);
        EndDialog(m_hwnd, 0);
    }
    else
    {
        SetDlgItemText(m_hwnd, IDC_BANK, pszCancelling);

        ShowWindow(GetDlgItem(m_hwnd, IDC_ENTRY), SW_HIDE);
        ShowWindow(GetDlgItem(m_hwnd, IDC_PROGRESS), SW_HIDE);

        EnableWindow(GetDlgItem(m_hwnd, IDCANCEL), FALSE);
    }
}


/****************************************************************************
 *
 *  BeginProject
 *
 *  Description:
 *      Callback function.
 *
 *  Arguments:
 *      UINT [in]: total number of banks in the project.
 *      UINT [in]: total number of entries in the project.
 *
 *  Returns:  
 *      BOOL: TRUE to continue processing.
 *
 ****************************************************************************/
 
#undef DPF_FNAME
#define DPF_FNAME "CGenerateDialog::BeginProject"

BOOL
CGenerateDialog::BeginProject
(
    UINT                    nBankCount, 
    UINT                    nEntryCount
)
{
    SendDlgItemMessage(m_hwnd, IDC_PROGRESS, PBM_SETRANGE32, 0, nEntryCount);

    return m_fContinue;
}


/****************************************************************************
 *
 *  EndProject
 *
 *  Description:
 *      Callback function.
 *
 *  Arguments:
 *      HRESULT [in]: return code.
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/
 
#undef DPF_FNAME
#define DPF_FNAME "CGenerateDialog::EndProject"

void
CGenerateDialog::EndProject
(
    HRESULT                 hr
)
{
    static const LPCSTR     pszSuccess  = "All entries were successfully written";
    static const LPCSTR     pszAbort    = "The operation was cancelled";
    static const LPCSTR     pszError    = "Errors occurred during processing";
    static const LPCSTR     pszClose    = "Close";
    
    if(SUCCEEDED(hr))
    {
        SetDlgItemText(m_hwnd, IDC_BANK, pszSuccess);
    }
    else if(E_ABORT == hr)
    {
        SetDlgItemText(m_hwnd, IDC_BANK, pszAbort);
    }        
    else
    {
        SetDlgItemText(m_hwnd, IDC_BANK, pszError);
    }

    ShowWindow(GetDlgItem(m_hwnd, IDC_ENTRY), SW_HIDE);
    ShowWindow(GetDlgItem(m_hwnd, IDC_PROGRESS), SW_HIDE);

    SetDlgItemText(m_hwnd, IDCANCEL, pszClose);
    EnableWindow(GetDlgItem(m_hwnd, IDCANCEL), TRUE);

    MessageBeep(-1);

    m_fAllowClose = TRUE;
}


/****************************************************************************
 *
 *  OpenBank
 *
 *  Description:
 *      Callback function.
 *
 *  Arguments:
 *      LPCSTR [in]: bank file name.
 *
 *  Returns:  
 *      BOOL: TRUE to continue processing.
 *
 ****************************************************************************/
 
#undef DPF_FNAME
#define DPF_FNAME "CGenerateDialog::OpenBank"

BOOL
CGenerateDialog::OpenBank
(
    LPCSTR                  pszFile
)
{
    CHAR                    szText[MAX_PATH + 0x100];
    CHAR                    szFile[MAX_PATH];

    _splitpath(pszFile, NULL, NULL, szFile, NULL);
    sprintf(szText, m_szBankText, szFile);

    SetDlgItemText(m_hwnd, IDC_BANK, szText);

    return m_fContinue;
}


/****************************************************************************
 *
 *  BeginEntry
 *
 *  Description:
 *      Callback function.
 *
 *  Arguments:
 *      LPCSTR [in]: entry file name.
 *      DWORD [in]: entry flags.
 *
 *  Returns:  
 *      BOOL: TRUE to continue processing.
 *
 ****************************************************************************/
 
#undef DPF_FNAME
#define DPF_FNAME "CGenerateDialog::BeginEntry"

BOOL
CGenerateDialog::BeginEntry
(
    LPCSTR                  pszFile,
    DWORD                   dwFlags
)
{
    static CHAR             szText[MAX_PATH + 0x100];
    CHAR                    szFile[MAX_PATH];

    _splitpath(pszFile, NULL, NULL, szFile, NULL);
    sprintf(szText, m_szEntryText, szFile);

    SetDlgItemText(m_hwnd, IDC_ENTRY, szText);

    return m_fContinue;
}


/****************************************************************************
 *
 *  EndEntry
 *
 *  Description:
 *      Callback function.
 *
 *  Arguments:
 *      LPCSTR [in]: entry file name.
 *      DWORD [in]: entry flags.
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/
 
#undef DPF_FNAME
#define DPF_FNAME "CGenerateDialog::EndEntry"

void
CGenerateDialog::EndEntry
(
    LPCSTR                  pszFile,
    DWORD                   dwFlags
)
{
    SendDlgItemMessage(m_hwnd, IDC_PROGRESS, PBM_SETPOS, ++m_nEntryIndex, 0);
}


