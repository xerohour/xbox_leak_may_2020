/***************************************************************************
 *
 *  Copyright (C) 2/12/2002 Microsoft Corporation.  All Rights Reserved.
 *
 *  File:       registry.cpp
 *  Content:    Registry wrapper objects.
 *  History:
 *   Date       By      Reason
 *   ====       ==      ======
 *  2/12/2002   dereks  Created.
 *
 ****************************************************************************/

#include "xactgui.h"


/****************************************************************************
 *
 *  CRegistryKey
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
#define DPF_FNAME "CRegistryKey::CRegistryKey"

CRegistryKey::CRegistryKey
(
    void
)
{
    m_hKey = NULL;
}


/****************************************************************************
 *
 *  ~CRegistryKey
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
#define DPF_FNAME "CRegistryKey::~CRegistryKey"

CRegistryKey::~CRegistryKey(void)
{
    Close();
}


/****************************************************************************
 *
 *  Create
 *
 *  Description:
 *      Creates a new key.
 *
 *  Arguments:
 *      HKEY [in]: parent key.
 *      LPCTSTR [in]: key name.
 *
 *  Returns:  
 *      BOOL: TRUE on success.
 *
 ****************************************************************************/
 
#undef DPF_FNAME
#define DPF_FNAME "CRegistryKey::Create"

BOOL 
CRegistryKey::Create
(   
    HKEY                    hParentKey, 
    LPCTSTR                 pszKeyName
)
{
    Close();
    
    return !RegCreateKey(hParentKey, pszKeyName, &m_hKey);
}


/****************************************************************************
 *
 *  Open
 *
 *  Description:
 *      Opens an existing key.
 *
 *  Arguments:
 *      HKEY [in]: parent key.
 *      LPCTSTR [in]: key name.
 *
 *  Returns:  
 *      BOOL: TRUE on success.
 *
 ****************************************************************************/
 
#undef DPF_FNAME
#define DPF_FNAME "CRegistryKey::Open"

BOOL 
CRegistryKey::Open
(
    HKEY                    hParentKey, 
    LPCTSTR                 pszKeyName
)
{
    Close();
    
    return !RegOpenKey(hParentKey, pszKeyName, &m_hKey);
}


/****************************************************************************
 *
 *  Close
 *
 *  Description:
 *      Closes an open key.
 *
 *  Arguments:
 *      (void)
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/
 
#undef DPF_FNAME
#define DPF_FNAME "CRegistryKey::Close"

void 
CRegistryKey::Close
(
    void
)
{
    if(m_hKey)
    {
        RegCloseKey(m_hKey);
        m_hKey = NULL;
    }
}


/****************************************************************************
 *
 *  GetNumericValue
 *
 *  Description:
 *      Reads a value from the registry.
 *
 *  Arguments:
 *      LPCTSTR [in]: value name.
 *      int * [out]: value data.
 *
 *  Returns:  
 *      BOOL: TRUE on success.
 *
 ****************************************************************************/
 
#undef DPF_FNAME
#define DPF_FNAME "CRegistryKey::GetNumericValue"

BOOL 
CRegistryKey::GetNumericValue
(
    LPCTSTR                 pszValueName, 
    int *                   pnValue
)
{
    DWORD                   dwType  = REG_DWORD;
    DWORD                   dwSize  = sizeof(*pnValue);
    
    if(RegQueryValueEx(m_hKey, pszValueName, NULL, &dwType, (LPBYTE)pnValue, &dwSize))
    {
        return FALSE;
    }

    ASSERT(REG_DWORD == dwType);
    ASSERT(sizeof(*pnValue) == dwSize);

    return TRUE;
}


/****************************************************************************
 *
 *  SetNumericValue
 *
 *  Description:
 *      Stores a value in the registry.
 *
 *  Arguments:
 *      LPCTSTR [in]: value name.
 *      int [in]: value data.
 *
 *  Returns:  
 *      BOOL: TRUE on success.
 *
 ****************************************************************************/
 
#undef DPF_FNAME
#define DPF_FNAME "CRegistryKey::SetNumericValue"

BOOL 
CRegistryKey::SetNumericValue
(
    LPCTSTR                 pszValueName, 
    int                     nValue
)
{
    return !RegSetValueEx(m_hKey, pszValueName, NULL, REG_DWORD, (const BYTE *)&nValue, sizeof(nValue));
}


/****************************************************************************
 *
 *  GetStringValue
 *
 *  Description:
 *      Reads a value from the registry.
 *
 *  Arguments:
 *      LPCTSTR [in]: value name.
 *      LPTSTR [out]: value data.
 *      UINT [in]: value data buffer length, in characters.
 *
 *  Returns:  
 *      BOOL: TRUE on success.
 *
 ****************************************************************************/
 
#undef DPF_FNAME
#define DPF_FNAME "CRegistryKey::GetStringValue"

BOOL 
CRegistryKey::GetStringValue
(
    LPCTSTR                 pszValueName, 
    LPTSTR                  pszValue, 
    UINT                    nLength
)
{
    DWORD                   dwType  = REG_SZ;
    DWORD                   dwSize  = nLength * sizeof(TCHAR);
    
    if(RegQueryValueEx(m_hKey, pszValueName, NULL, &dwType, (LPBYTE)pszValue, &dwSize))
    {
        return FALSE;
    }

    ASSERT(REG_SZ == dwType);
    ASSERT(nLength >= dwSize);

    pszValue[nLength - 1] = 0;

    return TRUE;
}


/****************************************************************************
 *
 *  SetStringValue
 *
 *  Description:
 *      Stores a value in the registry.
 *
 *  Arguments:
 *      LPCTSTR [in]: value name.
 *      LPCTSTR [in]: value data.
 *
 *  Returns:  
 *      BOOL: TRUE on success.
 *
 ****************************************************************************/
 
#undef DPF_FNAME
#define DPF_FNAME "CRegistryKey::SetStringValue"

BOOL 
CRegistryKey::SetStringValue
(
    LPCTSTR                 pszValueName, 
    LPCTSTR                 pszValue
)
{
    const int               nLength = _tcslen(pszValue) + 1;
    
    return !RegSetValueEx(m_hKey, pszValueName, NULL, REG_SZ, (const BYTE *)pszValue, nLength * sizeof(TCHAR));
}


/****************************************************************************
 *
 *  GetBinaryValue
 *
 *  Description:
 *      Reads a value from the registry.
 *
 *  Arguments:
 *      LPCTSTR [in]: value name.
 *      LPVOID [out]: value data.
 *      UINT [in]: value data buffer length, in bytes.
 *
 *  Returns:  
 *      BOOL: TRUE on success.
 *
 ****************************************************************************/
 
#undef DPF_FNAME
#define DPF_FNAME "CRegistryKey::GetBinaryValue"

BOOL 
CRegistryKey::GetBinaryValue
(
    LPCTSTR                 pszValueName, 
    LPVOID                  pvValue, 
    UINT                    nSize
)
{
    DWORD                   dwType  = REG_BINARY;
    DWORD                   dwSize  = nSize;
    
    if(RegQueryValueEx(m_hKey, pszValueName, NULL, &dwType, (BYTE *)pvValue, &dwSize))
    {
        return FALSE;
    }

    ASSERT(REG_BINARY == dwType);
    ASSERT(nSize == dwSize);

    return TRUE;
}


/****************************************************************************
 *
 *  SetBinaryValue
 *
 *  Description:
 *      Stores a value in the registry.
 *
 *  Arguments:
 *      LPCTSTR [in]: value name.
 *      LPCVOID [in]: value data.
 *      UINT [in]: value data buffer length, in bytes.
 *
 *  Returns:  
 *      BOOL: TRUE on success.
 *
 ****************************************************************************/
 
#undef DPF_FNAME
#define DPF_FNAME "CRegistryKey::SetBinaryValue"

BOOL 
CRegistryKey::SetBinaryValue
(
    LPCTSTR                 pszValueName, 
    LPCVOID                 pvValue, 
    UINT                    nSize
)
{
    return !RegSetValueEx(m_hKey, pszValueName, NULL, REG_BINARY, (const BYTE *)pvValue, nSize);
}


/****************************************************************************
 *
 *  CRegistry
 *
 *  Description:
 *      Object constructor.
 *
 *  Arguments:
 *      HKEY [in]: parent key.
 *      LPCTSTR [in]: application key path.
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CRegistry::CRegistry"

CRegistry::CRegistry
(
    HKEY                    hRootKey, 
    LPCTSTR                 pszAppPath
)
{
    m_AppKey.Create(hRootKey, pszAppPath);
}    


/****************************************************************************
 *
 *  ~CRegistry
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
#define DPF_FNAME "CRegistry::~CRegistry"

CRegistry::~CRegistry
(
    void
)
{
}


/****************************************************************************
 *
 *  GetNumericValue
 *
 *  Description:
 *      Reads a value from the registry.
 *
 *  Arguments:
 *      LPCTSTR [in]: subkey name.
 *      LPCTSTR [in]: value name.
 *      int * [out]: value data.
 *
 *  Returns:  
 *      BOOL: TRUE on success.
 *
 ****************************************************************************/
 
#undef DPF_FNAME
#define DPF_FNAME "CRegistry::GetNumericValue"

BOOL 
CRegistry::GetNumericValue
(
    LPCTSTR                 pszKeyName, 
    LPCTSTR                 pszValueName, 
    int *                   pnValue
)
{
    if(pszKeyName)
    {
        CRegistryKey Key;

        if(!Key.Open(m_AppKey, pszKeyName))
        {
            return FALSE;
        }

        return Key.GetNumericValue(pszValueName, pnValue);
    }
    else
    {
        return m_AppKey.GetNumericValue(pszValueName, pnValue);
    }
}


/****************************************************************************
 *
 *  SetNumericValue
 *
 *  Description:
 *      Stores a value in the registry.
 *
 *  Arguments:
 *      LPCTSTR [in]: subkey name.
 *      LPCTSTR [in]: value name.
 *      int [in]: value data.
 *
 *  Returns:  
 *      BOOL: TRUE on success.
 *
 ****************************************************************************/
 
#undef DPF_FNAME
#define DPF_FNAME "CRegistry::SetNumericValue"

BOOL 
CRegistry::SetNumericValue
(
    LPCTSTR                 pszKeyName, 
    LPCTSTR                 pszValueName, 
    int                     nValue
)
{
    if(pszKeyName)
    {
        CRegistryKey Key;

        if(!Key.Create(m_AppKey, pszKeyName))
        {
            return FALSE;
        }

        return Key.SetNumericValue(pszValueName, nValue);
    }
    else
    {
        return m_AppKey.SetNumericValue(pszValueName, nValue);
    }
}


/****************************************************************************
 *
 *  GetStringValue
 *
 *  Description:
 *      Reads a value from the registry.
 *
 *  Arguments:
 *      LPCTSTR [in]: subkey name.
 *      LPCTSTR [in]: value name.
 *      LPTSTR [out]: value data.
 *      UINT [in]: value data buffer length, in characters.
 *
 *  Returns:  
 *      BOOL: TRUE on success.
 *
 ****************************************************************************/
 
#undef DPF_FNAME
#define DPF_FNAME "CRegistry::GetStringValue"

BOOL 
CRegistry::GetStringValue
(
    LPCTSTR                 pszKeyName, 
    LPCTSTR                 pszValueName, 
    LPTSTR                  pszValue, 
    UINT                    nLength
)
{
    if(pszKeyName)
    {
        CRegistryKey Key;

        if(!Key.Open(m_AppKey, pszKeyName))
        {
            return FALSE;
        }

        return Key.GetStringValue(pszValueName, pszValue, nLength);
    }
    else
    {
        return m_AppKey.GetStringValue(pszValueName, pszValue, nLength);
    }
}


/****************************************************************************
 *
 *  SetStringValue
 *
 *  Description:
 *      Stores a value in the registry.
 *
 *  Arguments:
 *      LPCTSTR [in]: subkey name.
 *      LPCTSTR [in]: value name.
 *      LPCTSTR [in]: value data.
 *
 *  Returns:  
 *      BOOL: TRUE on success.
 *
 ****************************************************************************/
 
#undef DPF_FNAME
#define DPF_FNAME "CRegistry::SetStringValue"

BOOL 
CRegistry::SetStringValue
(
    LPCTSTR                 pszKeyName, 
    LPCTSTR                 pszValueName, 
    LPCTSTR                 pszValue
)
{
    if(pszKeyName)
    {
        CRegistryKey Key;

        if(!Key.Create(m_AppKey, pszKeyName))
        {
            return FALSE;
        }

        return Key.SetStringValue(pszValueName, pszValue);
    }
    else
    {
        return m_AppKey.SetStringValue(pszValueName, pszValue);
    }
}


/****************************************************************************
 *
 *  GetBinaryValue
 *
 *  Description:
 *      Reads a value from the registry.
 *
 *  Arguments:
 *      LPCTSTR [in]: subkey name.
 *      LPCTSTR [in]: value name.
 *      LPVOID [out]: value data.
 *      UINT [in]: value data buffer length, in bytes.
 *
 *  Returns:  
 *      BOOL: TRUE on success.
 *
 ****************************************************************************/
 
#undef DPF_FNAME
#define DPF_FNAME "CRegistry::GetBinaryValue"

BOOL 
CRegistry::GetBinaryValue
(
    LPCTSTR                 pszKeyName, 
    LPCTSTR                 pszValueName, 
    LPVOID                  pvValue, 
    UINT                    nSize
)
{
    if(pszKeyName)
    {
        CRegistryKey Key;

        if(!Key.Open(m_AppKey, pszKeyName))
        {
            return FALSE;
        }

        return Key.GetBinaryValue(pszValueName, pvValue, nSize);
    }
    else
    {
        return m_AppKey.GetBinaryValue(pszValueName, pvValue, nSize);
    }
}


/****************************************************************************
 *
 *  SetBinaryValue
 *
 *  Description:
 *      Stores a value in the registry.
 *
 *  Arguments:
 *      LPCTSTR [in]: subkey name.
 *      LPCTSTR [in]: value name.
 *      LPCVOID [in]: value data.
 *      UINT [in]: value data buffer length, in bytes.
 *
 *  Returns:  
 *      BOOL: TRUE on success.
 *
 ****************************************************************************/
 
#undef DPF_FNAME
#define DPF_FNAME "CRegistry::SetBinaryValue"

BOOL 
CRegistry::SetBinaryValue
(
    LPCTSTR                 pszKeyName, 
    LPCTSTR                 pszValueName, 
    LPCVOID                 pvValue, 
    UINT                    nSize
)
{
    if(pszKeyName)
    {
        CRegistryKey Key;

        if(!Key.Create(m_AppKey, pszKeyName))
        {
            return FALSE;
        }

        return Key.SetBinaryValue(pszValueName, pvValue, nSize);
    }
    else
    {
        return m_AppKey.SetBinaryValue(pszValueName, pvValue, nSize);
    }
}


/****************************************************************************
 *
 *  CMRU
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
#define DPF_FNAME "CMRU::CMRU"

CMRU::CMRU
(
    HKEY                    hParentKey, 
    LPCTSTR                 pszKeyName
)
{
    m_Key.Create(hParentKey, pszKeyName);

    Load();
}


/****************************************************************************
 *
 *  ~CMRU
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
#define DPF_FNAME "CMRU::~CMRU"

CMRU::~CMRU
(
    void
)
{
    Save();
}


/****************************************************************************
 *
 *  Load
 *
 *  Description:
 *      Loads MRU data from the registry.
 *
 *  Arguments:
 *      (void)
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CMRU::Load"

void
CMRU::Load
(
    void
)
{
    TCHAR                   szValueName[0x100];
    UINT                    i;
    
    //
    // Reinitialize list data
    //

    InitializeListHead(&m_lstItems);
    InitializeListHead(&m_lstFree);

    //
    // Read each item from the registry
    //

    for(i = 0; i < NUMELMS(m_aItems); i++)
    {
        _stprintf(szValueName, TEXT("%u"), i);
        
        if(!m_Key.GetStringValue(szValueName, m_aItems[i].szPath, NUMELMS(m_aItems[i].szPath)))
        {
            break;
        }

        InsertTailListUninit(&m_lstItems, &m_aItems[i].leItems);
    }

    //
    // Move the remaining items into the free list
    //

    for(; i < NUMELMS(m_aItems); i++)
    {
        InsertTailListUninit(&m_lstFree, &m_aItems[i].leItems);
    }

    //
    // Update the menu
    //

    UpdateMenu();
}


/****************************************************************************
 *
 *  Save
 *
 *  Description:
 *      Saves MRU data to the registry.
 *
 *  Arguments:
 *      (void)
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CMRU::Save"

void
CMRU::Save
(
    void
)
{
    TCHAR                   szValueName[0x100];
    DWORD                   dwLength;
    PLIST_ENTRY             pleEntry;
    LPMRUITEM               pItem;
    UINT                    i;
    
    //
    // Delete all values from the registry key
    //

    while(TRUE)
    {
        dwLength = NUMELMS(szValueName);
        
        if(RegEnumValue(m_Key, 0, szValueName, &dwLength, NULL, NULL, NULL, NULL))
        {
            break;
        }

        if(!RegDeleteValue(m_Key, szValueName))
        {
            break;
        }
    }

    //
    // Write all entries from the list
    //

    for(pleEntry = m_lstItems.Flink, i = 0; pleEntry != &m_lstItems; pleEntry = pleEntry->Flink, i++)
    {
        pItem = CONTAINING_RECORD(pleEntry, MRUITEM, leItems);
        
        _stprintf(szValueName, TEXT("%u"), i);

        m_Key.SetStringValue(szValueName, pItem->szPath);
    }
}


/****************************************************************************
 *
 *  AddItem
 *
 *  Description:
 *      Adds an item to the list.
 *
 *  Arguments:
 *      LPCTSTR [in]: item path.
 *      BOOL [in]: TRUE to update the MRU menu.
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CMRU::AddItem"

void
CMRU::AddItem
(
    LPCTSTR                 pszPath,
    BOOL                    fUpdateMenu
)
{
    PLIST_ENTRY             pleEntry;
    LPMRUITEM               pItem;

    //
    // Look for the item in the current list
    //

    for(pleEntry = m_lstItems.Flink; pleEntry != &m_lstItems; pleEntry = pleEntry->Flink)
    {
        pItem = CONTAINING_RECORD(pleEntry, MRUITEM, leItems);

        if(!_tcsicmp(pszPath, pItem->szPath))
        {
            break;
        }
    }

    //
    // If the item's already in the list, just move it to the head.  If not,
    // add it to the tail.  If we're out of free items, use the last one in
    // the current list.
    //

    if(pleEntry != &m_lstItems)
    {
        RemoveEntryList(pleEntry);
    }
    else if(!IsListEmpty(&m_lstFree))
    {
        pleEntry = RemoveHeadList(&m_lstFree);
    }
    else
    {
        pleEntry = RemoveTailList(&m_lstItems);
    }

    pItem = CONTAINING_RECORD(pleEntry, MRUITEM, leItems);

    _tcscpy(pItem->szPath, pszPath);
    
    InsertHeadList(&m_lstItems, pleEntry);

    //
    // Update the menu
    //

    if(fUpdateMenu)
    {
        UpdateMenu();
    }
}


/****************************************************************************
 *
 *  UpdateMenu
 *
 *  Description:
 *      Applies MRU data to the menu.
 *
 *  Arguments:
 *      (void)
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CMRU::UpdateMenu"

void
CMRU::UpdateMenu
(
    void
)
{
    TCHAR                   szString[MAX_PATH + 0x100];
    PLIST_ENTRY             pleEntry;
    LPMRUITEM               pItem;
    UINT                    i;
    
    if(!m_hMenu)
    {
        return;
    }

    //
    // Save the first item that already exists in the menu.  If there's
    // nothing in the MRU, we'll use this as a placeholder.
    //

    if(!m_szPlaceholder[0])
    {
        GetMenuString(m_hMenu, 0, m_szPlaceholder, NUMELMS(m_szPlaceholder), MF_BYPOSITION);
    }

    //
    // Remove existing menu items
    //
    
    while(DeleteMenu(m_hMenu, 0, MF_BYPOSITION));

    //
    // Add MRU data
    //

    for(pleEntry = m_lstItems.Flink, i = 0; pleEntry != &m_lstItems; pleEntry = pleEntry->Flink, i++)
    {
        pItem = CONTAINING_RECORD(pleEntry, MRUITEM, leItems);

        _stprintf(szString, TEXT("&%lu\t%s"), i + 1, pItem->szPath);

        AppendMenu(m_hMenu, MF_STRING, m_nCommandId + i, szString);
    }

    //
    // If we didn't add anything, add a placeholder
    //

    if(!i && m_szPlaceholder[0])
    {
        AppendMenu(m_hMenu, MF_STRING, m_nCommandId, m_szPlaceholder);
        EnableMenuItem(m_hMenu, 0, MF_BYPOSITION | MF_GRAYED);
    }
}


/****************************************************************************
 *
 *  TranslateCommand
 *
 *  Description:
 *      Converts a command identifier to a file path.
 *
 *  Arguments:
 *      UINT [in]: command identifier.
 *
 *  Returns:  
 *      LPCTSTR: file path or NULL.
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CMRU::TranslateCommand"

LPCTSTR
CMRU::TranslateCommand
(
    UINT                    nCommandId
)
{
    PLIST_ENTRY             pleEntry;
    LPMRUITEM               pItem;
    UINT                    i;

    if((nCommandId < m_nCommandId) || (nCommandId >= m_nCommandId + NUMELMS(m_aItems)))
    {
        return NULL;
    }

    pleEntry = m_lstItems.Flink;
    i = m_nCommandId;

    while((pleEntry != &m_lstItems) && (i != nCommandId))
    {
        pleEntry = pleEntry->Flink;
        i++;
    }

    if(i != nCommandId)
    {
        return NULL;
    }

    pItem = CONTAINING_RECORD(pleEntry, MRUITEM, leItems);

    return pItem->szPath;
}


/****************************************************************************
 *
 *  AttachMenu
 *
 *  Description:
 *      Attaches the MRU to a menu.
 *
 *  Arguments:
 *      HMENU [in]: menu handle.
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CMRU::AttachMenu"

void
CMRU::AttachMenu
(
    HMENU                   hMenu
)
{
    m_hMenu = hMenu;

    //
    // Get the identifier of the first item in the menu.  All MRU items will
    // increment from this base command identifier.
    //

    m_nCommandId = GetMenuItemID(m_hMenu, 0);
    ASSERT(-1 != m_nCommandId);

    //
    // Reset the menu
    //

    UpdateMenu();
}


/****************************************************************************
 *
 *  GetItems
 *
 *  Description:
 *      Retrieves the items in the MRU list.
 *
 *  Arguments:
 *      (void)
 *
 *  Returns:  
 *      LPTSTR: double-NULL-terminated string containing all the MRU items.
 *              the caller is responsible for freeing this buffer.
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CMRU::GetItems"

LPTSTR
CMRU::GetItems
(
    void
)
{
    LPTSTR                  pszBuffer;
    PLIST_ENTRY             pleEntry;
    LPMRUITEM               pItem;
    LPTSTR                  psz;

    //
    // Allocate a buffer large enough to hold the maximum number of items
    // possible
    //

    if(!(pszBuffer = MEMALLOC(TCHAR, MAX_PATH * MAX_ITEMS + 1)))
    {
        return NULL;
    }

    //
    // Start copying the items
    //

    psz = pszBuffer;
    
    for(pleEntry = m_lstItems.Flink; pleEntry != &m_lstItems; pleEntry = pleEntry->Flink)
    {
        pItem = CONTAINING_RECORD(pleEntry, MRUITEM, leItems);

        _tcscpy(psz, pItem->szPath);

        psz += _tcslen(psz) + 1;
    }

    return pszBuffer;
}


