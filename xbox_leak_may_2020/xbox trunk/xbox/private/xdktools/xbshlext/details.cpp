/*++

Copyright (c) Microsoft Corporation

Module Name:

    details.cpp

Abstract:

   Implementation of GetDetails and GetColumnHeaderDetails
   for CXboxFolder and its derivatives.

Environment:

    Windows 2000 and Later 
    User Mode
    Compiles UNICODE, but uses many ANSI APIs explictly.

Revision History:
    
    07-11-2001 : created

--*/

#include "stdafx.h"


//---------------------------------------------------------------------------------------------
//  Table for storing column information
//---------------------------------------------------------------------------------------------
typedef struct _XBOX_COLUMN_HEADER_INFORMATION
{
    //For SHCOLUMNID
    const GUID   *pfmtid;
    DWORD        pid;
    //For SHELLDETAILS
    int          fmt;
    int          cxChar;
    //For Finding Resources
    int          ResourceId;
} XBOX_COLUMN_HEADER_INFORMATION, *PXBOX_COLUMN_HEADER_INFORMATION;

static XBOX_COLUMN_HEADER_INFORMATION RootColumns[] = 
{
  /*XBOX_CONSOLE_COLUMN_NAME*/   {&FMTID_Storage, PID_STG_NAME, LVCFMT_LEFT, 30, IDS_COLUMN_HEADER_NAME},
  /*XBOX_CONSOLE_COLUMN_IPADDR*/ {&FMTID_ShellDetails, PID_DESCRIPTIONID, LVCFMT_LEFT, 16, IDS_COLUMN_HEADER_IPADDR}
};

static XBOX_COLUMN_HEADER_INFORMATION ConsoleColumns[] =
{
  /*XBOX_VOLUME_COLUMN_NAME*/ {&FMTID_Storage, PID_STG_NAME, LVCFMT_LEFT, 30, IDS_COLUMN_HEADER_NAME},
  /*XBOX_VOLUME_COLUMN_TYPE*/ {&FMTID_Storage, PID_STG_STORAGETYPE, LVCFMT_LEFT, 20, IDS_COLUMN_HEADER_TYPE},
  /*XBOX_VOLUME_COLUMN_FREE_CAPACITY*/{&FMTID_Volume, PID_VOLUME_FREE, LVCFMT_CENTER, 10, IDS_COLUMN_HEADER_FREE_CAPACITY}, 
  /*XBOX_VOLUME_COLUMN_TOTAL_CAPACITY*/{&FMTID_Storage, PID_STG_SIZE, LVCFMT_CENTER, 10, IDS_COLUMN_HEADER_TOTAL_CAPACITY}
};

static XBOX_COLUMN_HEADER_INFORMATION FileSystemFolderColumns[] =
{
  /*XBOX_FILE_COLUMN_NAME*/ {&FMTID_Storage, PID_STG_NAME, LVCFMT_LEFT, 30, IDS_COLUMN_HEADER_NAME},
  /*XBOX_FILE_COLUMN_SIZE*/ {&FMTID_Storage, PID_STG_SIZE, LVCFMT_RIGHT, 10, IDS_COLUMN_HEADER_SIZE},
  /*XBOX_FILE_COLUMN_TYPE*/ {&FMTID_Storage, PID_STG_STORAGETYPE, LVCFMT_LEFT, 15, IDS_COLUMN_HEADER_TYPE},
  /*XBOX_FILE_COLUMN_MODIFIED*/ {&FMTID_Storage, PID_STG_WRITETIME, LVCFMT_LEFT, 20, IDS_COLUMN_HEADER_MODIFIED},
  /*XBOX_FILE_COLUMN_ATTRIBUTE*/{&FMTID_Storage, PID_STG_ATTRIBUTES, LVCFMT_LEFT, 8, IDS_COLUMN_HEADER_ATTRIBUTES}
};

//---------------------------------------------------------------------------------------------
//  Table Implementation
//---------------------------------------------------------------------------------------------
static HRESULT GetColumnHeaderDetailsFromTable(PXBOX_COLUMN_HEADER_INFORMATION pTable, UINT uColumnCount, UINT iColumn, SHELLDETAILS *pShellDetails)
{
    if(iColumn >= uColumnCount)
    {
        return E_FAIL;
    }
    pShellDetails->cxChar = pTable[iColumn].cxChar;
    pShellDetails->fmt = pTable[iColumn].fmt;
    pShellDetails->str.uType = STRRET_CSTR;
    if(!LoadStringA(_Module.GetModuleInstance(), pTable[iColumn].ResourceId, pShellDetails->str.cStr, MAX_PATH))
    {
        return HRESULT_FROM_WIN32(GetLastError());
    }
    return S_OK;
}

HRESULT CXboxRoot::GetColumnHeaderDetails(UINT iColumn, SHELLDETAILS *pShellDetails)
{
    UINT uColumnCount = (sizeof(RootColumns)/sizeof(XBOX_COLUMN_HEADER_INFORMATION));
    return GetColumnHeaderDetailsFromTable(RootColumns, uColumnCount, iColumn, pShellDetails);
}

HRESULT CXboxConsole::GetColumnHeaderDetails(UINT iColumn, SHELLDETAILS *pShellDetails)
{
    UINT uColumnCount = (sizeof(ConsoleColumns)/sizeof(XBOX_COLUMN_HEADER_INFORMATION));
    return GetColumnHeaderDetailsFromTable(ConsoleColumns, uColumnCount, iColumn, pShellDetails);
}

HRESULT CXboxFileSystemFolder::GetColumnHeaderDetails(UINT iColumn, SHELLDETAILS *pShellDetails)
{
    UINT uColumnCount = (sizeof(FileSystemFolderColumns)/sizeof(XBOX_COLUMN_HEADER_INFORMATION));
    return GetColumnHeaderDetailsFromTable(FileSystemFolderColumns, uColumnCount, iColumn, pShellDetails);
}

HRESULT CXboxRoot::GetDetails(LPCITEMIDLIST pidl, UINT iColumn, SHELLDETAILS *pShellDetails)
{
    UINT uIndex;
    LPSTR pszParse;
    HRESULT hr = RefreshChildren();
    if(FAILED(hr))
    {
        return hr;
    }   

    hr = GetChildIndex((LPCSTR)pidl->mkid.abID, &uIndex);
    if(FAILED(hr))
    {
        return hr;        
    }

    //
    //  Fill out the column information
    //

    pShellDetails->str.uType = STRRET_CSTR;
    pszParse = m_rgpszChildNames[uIndex];
    switch(iColumn)
    {
        // Get the Actual Name
        case XBOX_CONSOLE_COLUMN_NAME:
          if(*pszParse=='?')
          {
             pszParse++;
             strcpy(pShellDetails->str.cStr, pszParse);
          } else
          {
             strcpy(pShellDetails->str.cStr, GetActualName(uIndex));
          }
          break;
        case XBOX_CONSOLE_COLUMN_IPADDR:
          if(*pszParse!='?')
          {
              DWORD dwChildIPAddress = GetChildIpAddress(uIndex);
              wsprintfA(pShellDetails->str.cStr,
                        "%d.%d.%d.%d",
                        (dwChildIPAddress >> 24)&0xFF,
                        (dwChildIPAddress >> 16)&0xFF,
                        (dwChildIPAddress >> 8)&0xFF,
                         dwChildIPAddress&0xFF
                        );
          } else
          {
            *pShellDetails->str.cStr = '\0';
          }
          break;
        default:
          hr = E_INVALIDARG;
    }
    return hr;
}

HRESULT CXboxConsole::GetDetails(LPCITEMIDLIST pidl, UINT iColumn, SHELLDETAILS *pShellDetails)
{
    UINT uIndex;
    LPSTR pszParse;
    HRESULT hr = RefreshChildren();
    if(FAILED(hr))
    {
        return hr;
    }   

    hr = GetChildIndex((LPCSTR)pidl->mkid.abID, &uIndex);
    if(FAILED(hr))
    {
        return hr;        
    }

    //
    //  Fill out the column information
    //
    pszParse = m_rgpszChildNames[uIndex];
    pShellDetails->str.uType = STRRET_CSTR;
    switch(iColumn)
    {
        //return the name for both the name and IP address for now.
        case XBOX_VOLUME_COLUMN_NAME:
          strcpy(pShellDetails->str.cStr, pszParse);
          break;
        case XBOX_VOLUME_COLUMN_TYPE:
          LoadStringA(_Module.GetModuleInstance(), m_rguChildVolumeType[uIndex], pShellDetails->str.cStr, MAX_PATH);
          break;
        case XBOX_VOLUME_COLUMN_FREE_CAPACITY:
          FormatUtils::FileSize(m_rgullChildFreeSpace[uIndex], pShellDetails->str.cStr);
          break;
        case XBOX_VOLUME_COLUMN_TOTAL_CAPACITY:
          FormatUtils::FileSize(m_rgullChildTotalSpace[uIndex], pShellDetails->str.cStr);
          break;
        default:
          hr = E_INVALIDARG;
    }
    return hr;
}

HRESULT CXboxFileSystemFolder::GetDetails(LPCITEMIDLIST pidl, UINT iColumn, SHELLDETAILS *pShellDetails)
{
    UINT uIndex;
    HRESULT hr = RefreshChildren();
    if(FAILED(hr))
    {
        return hr;
    }   

    hr = GetChildIndex((LPCSTR)pidl->mkid.abID, &uIndex);
    if(FAILED(hr))
    {
        return hr;        
    }

    //
    //  Fill out the column information
    //

    pShellDetails->str.uType = STRRET_CSTR;
    switch(iColumn)
    {
        case XBOX_FILE_COLUMN_NAME:
          strcpy(pShellDetails->str.cStr, m_rgpszChildNames[uIndex]);
          break;
        case XBOX_FILE_COLUMN_TYPE:
          strcpy(pShellDetails->str.cStr, m_rgszChildTypeNames[uIndex]);
          break;
        case XBOX_FILE_COLUMN_SIZE:
          if(m_rgChildFileAttributes[uIndex].Attributes&FILE_ATTRIBUTE_DIRECTORY)
          {
            *pShellDetails->str.cStr = '\0';
          } else
          {
              ULARGE_INTEGER uli;
              uli.HighPart = m_rgChildFileAttributes[uIndex].SizeHigh;
              uli.LowPart = m_rgChildFileAttributes[uIndex].SizeLow;
              FormatUtils::FileSize(uli.QuadPart, pShellDetails->str.cStr);
          }
          break;
        case XBOX_FILE_COLUMN_MODIFIED:
          FormatUtils::FileTime(&m_rgChildFileAttributes[uIndex].ChangeTime, pShellDetails->str.cStr);
          break;
        case XBOX_FILE_COLUMN_ATTRIBUTE:
          FormatUtils::FileAttributes(m_rgChildFileAttributes[uIndex].Attributes, pShellDetails->str.cStr);
          break;
        default:
          hr = E_INVALIDARG;
    }
    return hr;
}


HRESULT CXboxRoot::SetStatusBarText(IShellBrowser *pShellBrowser, LPCITEMIDLIST pidlSelect)
{
    UINT uIndex;
    LPSTR pszParse;
    LRESULT lr;
    HRESULT hr = RefreshChildren();
    if(SUCCEEDED(hr))
    {
        hr = GetChildIndex((LPCSTR)pidlSelect->mkid.abID, &uIndex);
        if(SUCCEEDED(hr))
        {
            if('?'==*m_rgpszChildNames[uIndex])
            {
                hr = pShellBrowser->SendControlMsg(
                        FCW_STATUS,
                        SB_SETTEXTA,
                        (WPARAM)0,
                        (LPARAM)WindowUtils::GetPreloadedString(IDS_PRELOAD_STATUS_ADD_CONSOLE_FORMAT),
                        &lr
                        );
            } else
            {
                char szStatusText[30];
                DWORD dwChildIPAddress = GetChildIpAddress(uIndex);
                wsprintfA(szStatusText,
                        "%s (%d.%d.%d.%d)",
                        GetActualName(uIndex),
                        (dwChildIPAddress >> 24)&0xFF,
                        (dwChildIPAddress >> 16)&0xFF,
                        (dwChildIPAddress >> 8)&0xFF,
                         dwChildIPAddress&0xFF
                        );
                hr = pShellBrowser->SendControlMsg(
                        FCW_STATUS,
                        SB_SETTEXTA,
                        (WPARAM)0,
                        (LPARAM)szStatusText,
                        &lr
                        );
            }
        }
    }
    return hr;
}
HRESULT CXboxConsole::SetStatusBarText(IShellBrowser *pShellBrowser, LPCITEMIDLIST pidlSelect)
{
    UINT uIndex;
    LPSTR pszParse;
    LRESULT lr;
    HRESULT hr = RefreshChildren();
    if(SUCCEEDED(hr)) 
    {
        hr = GetChildIndex((LPCSTR)pidlSelect->mkid.abID, &uIndex);
        if(SUCCEEDED(hr))
        {
            char szFree[30];
            char szTotal[30];
            char szStatusText[128];
            FormatUtils::FileSize(m_rgullChildFreeSpace[uIndex], szFree);
            FormatUtils::FileSize(m_rgullChildTotalSpace[uIndex], szTotal);
            WindowUtils::rsprintf(szStatusText, IDS_STATUS_VOLUME, szFree, szTotal);
            hr = pShellBrowser->SendControlMsg(
                        FCW_STATUS,
                        SB_SETTEXTA,
                        (WPARAM)0,
                        (LPARAM)szStatusText,
                        &lr
                        );
        }
    }

    return hr;
}


HRESULT CXboxFileSystemFolder::SetStatusBarText(IShellBrowser *pShellBrowser, LPCITEMIDLIST pidlSelect)
{
    UINT uIndex;
    LRESULT lr;
    HRESULT hr = RefreshChildren();
    if(SUCCEEDED(hr))
    {
        hr = GetChildIndex((LPCSTR)pidlSelect->mkid.abID, &uIndex);
        if(SUCCEEDED(hr))
        {
            char szStatusText[196];
            if(m_rgChildFileAttributes[uIndex].Attributes&FILE_ATTRIBUTE_DIRECTORY)
            {
                WindowUtils::rsprintf(szStatusText, IDS_STATUS_SELOBJECT_COUNT_FORMAT, 1);
            } else
            {
                char szDate[50];
                char szSize[30];
                ULARGE_INTEGER uli;

                FormatUtils::FileTime(&m_rgChildFileAttributes[uIndex].ChangeTime, szDate);
                uli.HighPart = m_rgChildFileAttributes[uIndex].SizeHigh;
                uli.LowPart = m_rgChildFileAttributes[uIndex].SizeLow;
                FormatUtils::FileSize(uli.QuadPart, szSize);
                WindowUtils::rsprintf(szStatusText, IDS_STATUS_FILE, m_rgszChildTypeNames[uIndex], szDate, szSize);
            }
            hr = pShellBrowser->SendControlMsg(
                    FCW_STATUS,
                    SB_SETTEXTA,
                    (WPARAM)0,
                    (LPARAM)szStatusText,
                    &lr
                    );
        }
    }
    return hr;
}
