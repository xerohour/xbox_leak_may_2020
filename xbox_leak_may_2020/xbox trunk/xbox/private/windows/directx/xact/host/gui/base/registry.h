/***************************************************************************
 *
 *  Copyright (C) 2/12/2002 Microsoft Corporation.  All Rights Reserved.
 *
 *  File:       registry.h
 *  Content:    Registry wrapper objects.
 *  History:
 *   Date       By      Reason
 *   ====       ==      ======
 *  2/12/2002   dereks  Created.
 *
 ****************************************************************************/

#ifndef __REGISTRY_H__
#define __REGISTRY_H__

#ifdef __cplusplus

//
// Generic key object
//

class CRegistryKey
{
protected:
    HKEY                    m_hKey;         // Registry key handle

public:
    CRegistryKey(void);
    virtual ~CRegistryKey(void);

public:
    // Initialization
    virtual BOOL Create(HKEY hParentKey, LPCTSTR pszKeyName);
    virtual BOOL Open(HKEY hParentKey, LPCTSTR pszKeyName);

    virtual void Close(void);

    // Value data
    virtual BOOL GetNumericValue(LPCTSTR pszValueName, int *pnValue);
    virtual BOOL SetNumericValue(LPCTSTR pszValueName, int nValue);
    
    virtual BOOL GetStringValue(LPCTSTR pszValueName, LPTSTR pszValue, UINT nLength);
    virtual BOOL SetStringValue(LPCTSTR pszValueName, LPCTSTR pszValue);
    
    virtual BOOL GetBinaryValue(LPCTSTR pszValueName, LPVOID pvValue, UINT nSize);
    virtual BOOL SetBinaryValue(LPCTSTR pszValueName, LPCVOID pvValue, UINT nSize);

    // Operators
    virtual operator HKEY(void);
};

__inline CRegistryKey::operator HKEY(void)
{
    return m_hKey;
}

//
// Registry wrapper object
//

class CRegistry
{
protected:
    CRegistryKey            m_AppKey;       // Application root key

public:
    CRegistry(HKEY hRootKey, LPCTSTR pszAppPath);
    virtual ~CRegistry(void);

public:
    // Value data
    virtual BOOL GetNumericValue(LPCTSTR pszKeyName, LPCTSTR pszValueName, int *pnValue);
    virtual BOOL SetNumericValue(LPCTSTR pszKeyName, LPCTSTR pszValueName, int nValue);
    
    virtual BOOL GetStringValue(LPCTSTR pszKeyName, LPCTSTR pszValueName, LPTSTR pszValue, UINT nLength);
    virtual BOOL SetStringValue(LPCTSTR pszKeyName, LPCTSTR pszValueName, LPCTSTR pszValue);
    
    virtual BOOL GetBinaryValue(LPCTSTR pszKeyName, LPCTSTR pszValueName, LPVOID pvValue, UINT nSize);
    virtual BOOL SetBinaryValue(LPCTSTR pszKeyName, LPCTSTR pszValueName, LPCVOID pvValue, UINT nSize);

    // Operators
    virtual operator HKEY(void);
};

__inline CRegistry::operator HKEY(void)
{
    return m_AppKey;
}

//
// Most-recently-used list
//

class CMRU
{
public:
    enum
    {
        MAX_ITEMS = 4
    };

    typedef struct
    {
        LIST_ENTRY          leItems;                // Item list entry
        TCHAR               szPath[MAX_PATH];       // File path
    } MRUITEM, *LPMRUITEM;

protected:
    CRegistryKey            m_Key;                  // Registry key
    MRUITEM                 m_aItems[MAX_ITEMS];    // Item data
    LIST_ENTRY              m_lstItems;             // Item list
    LIST_ENTRY              m_lstFree;              // Free list
    HMENU                   m_hMenu;                // Menu handle
    UINT                    m_nCommandId;           // Menu item identifier
    TCHAR                   m_szPlaceholder[0x100]; // Placeholder item string

public:
    CMRU(HKEY hParentKey, LPCTSTR pszKeyName);
    virtual ~CMRU(void);

public:
    // Initialization
    virtual void AttachMenu(HMENU hMenu);
    virtual void UpdateMenu(void);

    // MRU data
    virtual void AddItem(LPCSTR pszPath, BOOL fUpdateMenu = TRUE);
    virtual LPTSTR GetItems(void);

    // Command handler
    virtual LPCTSTR TranslateCommand(UINT nCommandId);

protected:
    // MRU data
    virtual void Load(void);
    virtual void Save(void);
};

#endif // __cplusplus

#endif // __REGISTRY_H__
