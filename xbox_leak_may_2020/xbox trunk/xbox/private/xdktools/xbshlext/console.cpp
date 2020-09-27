/*++

Copyright (c) Microsoft Corporation

Module Name:

    console.cpp

Abstract:

   Implementation of CManageConsoles.  Handles the registry with respect to consoles.

Environment:

    Windows 2000 and Later 
    User Mode
    Compiles UNICODE, but uses many ANSI APIs explictly.

Revision History:
    
    06-29-2001 : created

--*/
#include <stdafx.h>

CManageConsoles::CManageConsoles() : 
    m_hKey(NULL),
    m_dwEnumIndex(0),
    m_dwMaxCount(0),
    m_fDefaultConsoleValid(false)
{
    DWORD   dwBogus;
    DWORD   dwBufferSize;
    LONG    lError;
    
    //
    //  Get the name of the default console
    //
    dwBufferSize = sizeof(m_szDefaultConsole);
    if(SUCCEEDED(DmGetXboxName(m_szDefaultConsole, &dwBufferSize)))
    {
        m_fDefaultConsoleValid = true;
    }

    //
    //  Open\Create the console key
    //
    lError =  RegCreateKeyExA(
                  HKEY_CURRENT_USER,
                  XBOX_SHELL_EXTENSION_CONSOLE_KEY,
                  0,
                  NULL,
                  REG_OPTION_NON_VOLATILE,
                  KEY_ALL_ACCESS,
                  NULL,
                  &m_hKey,
                  NULL
                  );
    if(ERROR_SUCCESS!=lError)
    {
        m_hKey = NULL;
        return;
    }
    
    //
    //  Read the number of consoles
    //
    dwBufferSize = sizeof(DWORD);
    lError = RegQueryValueExA(m_hKey, NULL, NULL, NULL, (PBYTE)&m_dwMaxCount, &dwBufferSize);
    if(ERROR_SUCCESS!=lError)
    {
        m_dwMaxCount = 0;
        RegSetValueEx(m_hKey, NULL, 0, REG_DWORD, (BYTE *)&m_dwMaxCount, sizeof(m_dwMaxCount));
    }

    //
    // Add the default console, note Add is smart enough not to add it, if
    // it is already known.
    //
    if(m_fDefaultConsoleValid)
    {
        Add(m_szDefaultConsole);        
    }
}

CManageConsoles::~CManageConsoles()
{
    if(m_hKey)
    {
        RegCloseKey(m_hKey);
        m_hKey = NULL;
    }
}

BOOL CManageConsoles::Add(LPSTR pszConsoleName)
{
    DWORD dwBogus;
    DWORD dwBufferSize;
    LONG  lError;

    //
    //  Can't do anything if we don't have a key.
    //
    if(!m_hKey) return FALSE;

    //
    //  First, see if the console is already known.
    //
    dwBufferSize = sizeof(dwBogus);
    lError = RegQueryValueExA(
                m_hKey,
                pszConsoleName,
                NULL,
                NULL,
                (PBYTE)&dwBogus,
                &dwBufferSize
                );
    //
    //  If the console is not already known,
    //  add it.
    //
    if(lError != ERROR_SUCCESS)
    {
        lError = RegSetValueExA(
                    m_hKey,
                    pszConsoleName,
                    0,
                    REG_DWORD,
                    (BYTE *)&dwBogus,
                    sizeof(dwBogus)
                    );
        if(ERROR_SUCCESS == lError)
        {
            //
            //  Increment the count of know consoles.
            //
            
            m_dwMaxCount++;
            lError = RegSetValueExA(
                m_hKey,
                NULL,
                0,             
                REG_DWORD,
                (BYTE *)&m_dwMaxCount,
                sizeof(DWORD)
                );
        }
    }

    //
    //  Return our results
    //
    return (ERROR_SUCCESS==lError) ? TRUE : FALSE;
}

BOOL CManageConsoles::Remove(LPSTR pszConsoleName)
{
    DWORD dwBogus = 0;
    DWORD dwBufferSize;
    LONG  lError;

    //
    //  Can't do anything if we don't have a key.
    //
    if(!m_hKey) return FALSE;

    //
    //  If the console is the default console, we must fail.
    //

    if(IsDefault(pszConsoleName))
    {
        return FALSE;
    }

    //
    //  First, see if the console is already known.
    //
    dwBufferSize = sizeof(dwBogus);
    lError = RegQueryValueExA(
                m_hKey,
                pszConsoleName,
                NULL,
                NULL,
                (PBYTE)&dwBogus,
                &dwBufferSize
                );
    //
    //  If the console is known remove it.
    //  
    //
    if(lError == ERROR_SUCCESS)
    {
        lError = RegDeleteValueA(m_hKey, pszConsoleName);      
        if(lError == ERROR_SUCCESS)
        {
            m_dwMaxCount--;
            RegSetValueExA(
                m_hKey,
                NULL,
                0,             
                REG_DWORD,
                (BYTE *)&m_dwMaxCount,
                sizeof(DWORD)
                );
        }
    } else
    {
        // Return success, as there was no need to delete it realy.
        return TRUE;
    }

    // Return success or failure
    return (ERROR_SUCCESS==lError) ? TRUE : FALSE;
}

BOOL CManageConsoles::SetDefault(LPSTR pszConsoleName)
{
    HRESULT hr = E_FAIL;
    
    // Make sure that this has been add first.
    if(pszConsoleName)
    {
        if(Add(pszConsoleName))
        {
            hr = DmSetXboxName(pszConsoleName);
        }
    } else
    {
        hr = DmSetXboxName("");
    }
    
    // Send notification to clear check on current default.
    WCHAR szPath[512];

    if(*m_szDefaultConsole)
    {
        wsprintf(szPath, L"%hs\\%hs", ROOT_GUID_NAME, m_szDefaultConsole);
        SHChangeNotify(SHCNE_UPDATEITEM, SHCNF_PATH|SHCNF_FLUSH, szPath, NULL);
    }
    
    // Handle new name
    if(pszConsoleName)
    {
        strcpy(m_szDefaultConsole, pszConsoleName);
    
        //Notification of new default
        wsprintf(szPath, L"%hs\\%hs", ROOT_GUID_NAME, pszConsoleName);
        SHChangeNotify(SHCNE_UPDATEITEM, SHCNF_PATH|SHCNF_FLUSH, szPath, NULL);
    } else
    {
        m_szDefaultConsole[0] = '\0';
    }

    return SUCCEEDED(hr) ? TRUE : FALSE;
}

BOOL CManageConsoles::IsDefault(LPSTR pszConsoleName)
{
    if(m_fDefaultConsoleValid && (0==_stricmp(pszConsoleName, m_szDefaultConsole)))
    {
        return TRUE;
    }
    
    return FALSE;
}


BOOL CManageConsoles::GetNext(LPSTR pszConsoleName, DWORD *pdwConsoleNameLength)
{
    LONG lError; 
    DWORD dwConsoleNameLength = *pdwConsoleNameLength;
    do
    {
       *pdwConsoleNameLength = dwConsoleNameLength;
       lError = RegEnumValueA(
                   m_hKey,
                   m_dwEnumIndex++,
                   pszConsoleName,
                   pdwConsoleNameLength,
                   NULL,// reserved
                   NULL,// don't want the value type
                   NULL,// or the value
                   NULL // or the value size.
                   );
       // Don't accept the default value as a console name
    }while((ERROR_SUCCESS==lError)&& (*pszConsoleName=='\0'));

    return (ERROR_SUCCESS==lError) ? TRUE : FALSE;
}