/*++

Copyright (c) Microsoft Corporation

Module Name:

    console.cpp

Abstract:

   Declaration of CManageConsoles

Environment:

    Windows 2000 and Later 
    User Mode
    Compiles UNICODE, but uses many ANSI APIs explictly.

Revision History:
    
    06-29-2001 : created

--*/

class CManageConsoles
{
  public:
    CManageConsoles();
    ~CManageConsoles();

    //  Add Remove
    BOOL Add(LPSTR pszConsoleName);
    BOOL Remove(LPSTR pszConsoleName);

    //  Set default
    BOOL SetDefault(LPSTR pszConsoleName);
    BOOL IsDefault(LPSTR pszConsoleName);
    //  Enum
    void  ResetEnum() {m_dwEnumIndex=0;}
    BOOL  GetNext(LPSTR pszConsoleName, DWORD *pdwConsoleNameLength);
    DWORD GetMaxCount() {return m_dwMaxCount;}
  private:
    HKEY  m_hKey;
    DWORD m_dwEnumIndex;
    DWORD m_dwMaxCount;
    char  m_szDefaultConsole[60];
    bool  m_fDefaultConsoleValid;
};