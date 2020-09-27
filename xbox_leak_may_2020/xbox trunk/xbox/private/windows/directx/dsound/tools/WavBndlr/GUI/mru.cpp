/***************************************************************************
 *
 *  Copyright (C) 11/26/2001 Microsoft Corporation.  All Rights Reserved.
 *
 *  File:       mru.cpp
 *  Content:    Most-recently-used list.
 *  History:
 *   Date       By      Reason
 *   ====       ==      ======
 *  11/26/2001   dereks  Created.
 *
 ****************************************************************************/

#include "wbndgui.h"


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
    void
)
{
    UINT                    i;
    
    m_hmenu = NULL;

    for(i = 0; i < NUMELMS(m_aszPaths); i++)
    {
        m_aszPaths[i][0] = 0;
    }
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
 *  Initialize
 *
 *  Description:
 *      Initializees the object to a menu.
 *
 *  Arguments:
 *      LPCSTR [in]: parent registry key.
 *      HMENU [in]: menu handle.
 *      UINT [in]: first MRU item command identifier.
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CMRU::Initialize"

void
CMRU::Initialize
(
    LPCSTR                  pszParentKey,
    HMENU                   hmenu,
    UINT                    nCommandId
)
{
    m_hmenu = hmenu;
    m_nCommandId = nCommandId;

    Load(pszParentKey);
}


/****************************************************************************
 *
 *  Load
 *
 *  Description:
 *      Loads MRU data from the registry.
 *
 *  Arguments:
 *      LPCSTR [in]: parent registry key.
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
    LPCSTR                  pszParentKey
)
{
    CHAR                    szValue[]   = "a";
    HKEY                    hkeyParent  = NULL;
    HKEY                    hkey        = NULL;
    DWORD                   dwSize;
    LONG                    lResult;
    UINT                    i;

    if(pszParentKey)
    {
        m_pszParentKey = pszParentKey;
    }
    else
    {
        pszParentKey = m_pszParentKey;
    }

    for(i = 0; i < NUMELMS(m_aszPaths); i++)
    {
        m_aszPaths[i][0] = 0;
    }
    
    if(pszParentKey)
    {
        lResult = RegCreateKey(HKEY_CURRENT_USER, pszParentKey, &hkeyParent);

        if(ERROR_SUCCESS == lResult)
        {
            lResult = RegCreateKey(hkeyParent, MRU_KEY, &hkey);
        }

        for(i = 0; (ERROR_SUCCESS == lResult) && (i < NUMELMS(m_aszPaths)); i++)
        {
            dwSize = sizeof(m_aszPaths[i]);
            lResult = RegQueryValueEx(hkey, szValue, NULL, NULL, (LPBYTE)m_aszPaths[i], &dwSize);

            szValue[0]++;
        }
    }

    Apply();

    if(hkey)
    {
        RegCloseKey(hkey);
    }

    if(hkeyParent)
    {
        RegCloseKey(hkeyParent);
    }
}


/****************************************************************************
 *
 *  Save
 *
 *  Description:
 *      Saves MRU data to the registry.
 *
 *  Arguments:
 *      LPCSTR [in]: parent registry key.
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
    LPCSTR                  pszParentKey
)
{
    CHAR                    szValue[]   = "a";
    HKEY                    hkeyParent  = NULL;
    HKEY                    hkey        = NULL;
    DWORD                   dwSize;
    LONG                    lResult;
    UINT                    i;

    if(!pszParentKey)
    {
        pszParentKey = m_pszParentKey;
    }

    if(pszParentKey)
    {
        lResult = RegCreateKey(HKEY_CURRENT_USER, pszParentKey, &hkeyParent);

        if(ERROR_SUCCESS == lResult)
        {
            lResult = RegCreateKey(hkeyParent, MRU_KEY, &hkey);
        }

        for(i = 0; (ERROR_SUCCESS == lResult) && (i < NUMELMS(m_aszPaths)); i++)
        {
            if(!m_aszPaths[i][0])
            {
                break;
            }

            lResult = RegSetValueEx(hkey, szValue, 0, REG_SZ, (LPBYTE)m_aszPaths[i], strlen(m_aszPaths[i]) + 1);

            szValue[0]++;
        }
    }

    if(hkey)
    {
        RegCloseKey(hkey);
    }

    if(hkeyParent)
    {
        RegCloseKey(hkeyParent);
    }
}


/****************************************************************************
 *
 *  Apply
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
#define DPF_FNAME "CMRU::Apply"

void
CMRU::Apply
(
    void
)
{
    CHAR                    szString[MAX_PATH + 0x100];
    UINT                    i;
    
    if(!m_hmenu)
    {
        return;
    }

    //
    // Remove existing menu items
    //
    
    while(DeleteMenu(m_hmenu, 0, MF_BYPOSITION));

    //
    // Add MRU data
    //

    if(m_aszPaths[0][0])
    {
        for(i = 0; i < NUMELMS(m_aszPaths); i++)
        {
            if(!m_aszPaths[i][0])
            {
                break;
            }

            sprintf(szString, "&%lu\t%s", i + 1, m_aszPaths[i]);

            AppendMenu(m_hmenu, MF_STRING, m_nCommandId + i, szString);
        }
    }
    else
    {
        AppendMenu(m_hmenu, MF_STRING | MF_GRAYED, m_nCommandId, "Recent File");
    }
}


/****************************************************************************
 *
 *  AddItem
 *
 *  Description:
 *      Adds an item to the MRU list.
 *
 *  Arguments:
 *      LPCSTR [in]: item path.
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
    LPCSTR                  pszPath
)
{
    UINT                    nItemIndex          = -1;
    CHAR                    szTemp[MAX_PATH];
    UINT                    i;
    
    //
    // See if the item already appears in the list
    //

    for(i = 0; i < NUMELMS(m_aszPaths); i++)
    {
        if(!m_aszPaths[i][0])
        {
            break;
        }

        if(!_strcmpi(pszPath, m_aszPaths[i]))
        {
            nItemIndex = i;
            break;
        }
    }

    //
    // If the item's already in the list, just reorder so it's at the top.  If
    // not, add the item to the top of the list.
    //

    if(nItemIndex >= NUMELMS(m_aszPaths))
    {
        nItemIndex = NUMELMS(m_aszPaths) - 1;
    }

    for(i = nItemIndex; i > 0; i--)
    {
        strcpy(m_aszPaths[i], m_aszPaths[i - 1]);
    }

    strcpy(m_aszPaths[0], pszPath);

    //
    // Update UI
    //

    Apply();
}


