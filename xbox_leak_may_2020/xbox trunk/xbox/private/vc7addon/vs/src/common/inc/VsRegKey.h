#pragma once

/////////////////////////////////////////////////////////////////////////////
// CVsRegKeyW

#define VsRegKey_ExpectedPtr(ptr)               \
    VSASSERT(ptr != NULL, "Invalid arg");       \
    if (ptr == NULL)                            \
        { return E_POINTER; }

class CVsRegKeyW
{
public:
  CVsRegKeyW();
  ~CVsRegKeyW();

// Attributes
public:
  operator HKEY() const;
  HKEY m_hKey;

// Operations
public:
  HRESULT  QueryValue(LPCOLESTR lpszValueName, BSTR * pbstrValue);
  HRESULT  QueryValue(LPCOLESTR lpszValueName, GUID *pGuid);
  HRESULT  QueryValue(LPCOLESTR lpszValueName, DWORD * pdwValue);

  HRESULT  SetValue(LPCOLESTR lpszValueName, LPCOLESTR lpszValue);
  HRESULT  SetValue(LPCOLESTR lpszValueName, DWORD dwValue);
  //HRESULT  SetValueW(LPCOLESTR lpszValueName, REFGUID rGuid);

  HRESULT SetKeyValue(LPCOLESTR lpszKeyName, LPCOLESTR lpszValueName, LPCOLESTR lpszValue);
  static HRESULT WINAPI SetValue(HKEY hKeyParent, LPCOLESTR lpszKeyName,
	  LPCOLESTR lpszValueName, LPCOLESTR lpszValue);

  HRESULT Create(HKEY hKeyParent, LPCOLESTR lpszKeyName,
	  LPOLESTR lpszClass = REG_NONE, DWORD dwOptions = REG_OPTION_NON_VOLATILE,
	  REGSAM samDesired = KEY_ALL_ACCESS,
	  LPSECURITY_ATTRIBUTES lpSecAttr = NULL,
	  LPDWORD lpdwDisposition = NULL);
  HRESULT Open(HKEY hKeyParent, LPCOLESTR lpszKeyName,
	  REGSAM samDesired = KEY_ALL_ACCESS);
  HRESULT Close();
  HKEY    Detach();
  void    Attach(HKEY hKey);
  HRESULT DeleteSubKey(LPCOLESTR lpszSubKey);
  HRESULT RecurseDeleteKey(LPCOLESTR lpszKey);
  HRESULT DeleteValue(LPCOLESTR lpszValue);
  HRESULT EnumKeyEx(DWORD dwIndex, BSTR * pbstrName, BSTR * pbstrClass = NULL, PFILETIME lpftLastWriteTime = NULL);
  HRESULT GetSubkeyCount(DWORD * pdwCount); 
  HRESULT EnumValue(DWORD dwIndex, BSTR * pbstrName, DWORD * pType = NULL);
  HRESULT GetValueCount(DWORD * pdwCount); 
};

//-------------------------------------------------

inline CVsRegKeyW::CVsRegKeyW()
{
  m_hKey = NULL;
}

inline CVsRegKeyW::~CVsRegKeyW()
{
  Close();
}

inline CVsRegKeyW::operator HKEY() const
{ 
  return m_hKey;
}

inline HKEY CVsRegKeyW::Detach()
{
  HKEY hKey = m_hKey;
  m_hKey = NULL;
  return hKey;
}

inline void CVsRegKeyW::Attach(HKEY hKey)
{
  ATLASSERT(m_hKey == NULL);
  m_hKey = hKey;
}

//-------------------------------------------------

inline HRESULT CVsRegKeyW::DeleteSubKey(LPCOLESTR lpszSubKey)
{
  ATLASSERT(m_hKey != NULL);
  return HRESULT_FROM_WIN32(RegDeleteKeyW(m_hKey, lpszSubKey));
}

//-------------------------------------------------

inline HRESULT CVsRegKeyW::DeleteValue(LPCOLESTR lpszValue)
{
  ATLASSERT(m_hKey != NULL);
  return HRESULT_FROM_WIN32(RegDeleteValueW(m_hKey, (LPOLESTR)lpszValue));
}

//-------------------------------------------------

inline LONG CVsRegKeyW::RecurseDeleteKey(LPCOLESTR lpszKey)
{
  CVsRegKeyW key;
  LONG lRes = key.Open(m_hKey, lpszKey, KEY_READ | KEY_WRITE);
  if (lRes != ERROR_SUCCESS)
    return lRes;
  FILETIME time;
  DWORD dwSize = 256;
  WCHAR szBuffer[256];
  while (RegEnumKeyExW(key.m_hKey, 0, szBuffer, &dwSize, NULL, NULL, NULL,
	  &time)==ERROR_SUCCESS)
  {
    lRes = key.RecurseDeleteKey(szBuffer);
    if (lRes != ERROR_SUCCESS)
	    return lRes;
    dwSize = 256;
  }
  key.Close();
  return DeleteSubKey(lpszKey);
}

//-------------------------------------------------

inline HRESULT CVsRegKeyW::Close()
{
  LONG lRes = ERROR_SUCCESS;
  if (m_hKey != NULL)
  {
    lRes = RegCloseKey(m_hKey);
    m_hKey = NULL;
  }
  return HRESULT_FROM_WIN32(lRes);
}

//-------------------------------------------------

inline HRESULT CVsRegKeyW::Create(HKEY hKeyParent, LPCOLESTR lpszKeyName,
  LPOLESTR lpszClass, DWORD dwOptions, REGSAM samDesired,
  LPSECURITY_ATTRIBUTES lpSecAttr, LPDWORD lpdwDisposition)
{
  ATLASSERT(hKeyParent != NULL);
  DWORD dw;
  HKEY hKey = NULL;
  LONG lRes = RegCreateKeyExW(hKeyParent, lpszKeyName, 0,
	  lpszClass, dwOptions, samDesired, lpSecAttr, &hKey, &dw);
  if (lpdwDisposition != NULL)
    *lpdwDisposition = dw;
  if (lRes == ERROR_SUCCESS)
  {
    lRes = Close();
    m_hKey = hKey;
  }
  return HRESULT_FROM_WIN32(lRes);
}

//-------------------------------------------------

inline HRESULT CVsRegKeyW::Open(HKEY hKeyParent, LPCOLESTR lpszKeyName, REGSAM samDesired)
{
  ATLASSERT(hKeyParent != NULL);
  HKEY hKey = NULL;
  LONG lRes = RegOpenKeyExW(hKeyParent, lpszKeyName, 0, samDesired, &hKey);
  if (lRes == ERROR_SUCCESS)
  {
    Close();
    m_hKey = hKey;
  }
  return HRESULT_FROM_WIN32(lRes);
}

//-------------------------------------------------
// Query DWORD
//-------------------------------------------------

inline HRESULT CVsRegKeyW::QueryValue(LPCOLESTR lpszValueName, DWORD * pdwValue)
{
  VsRegKey_ExpectedPtr(pdwValue);

  DWORD dwType = NULL;
  DWORD dwCount = sizeof(DWORD);
  LONG lRes = RegQueryValueExW(m_hKey, (LPOLESTR)lpszValueName, NULL, &dwType,
	  (LPBYTE)pdwValue, &dwCount);
  ATLASSERT((lRes!=ERROR_SUCCESS) || (dwType == REG_DWORD));
  ATLASSERT((lRes!=ERROR_SUCCESS) || (dwCount == sizeof(DWORD)));
  return HRESULT_FROM_WIN32(lRes);
}

//-------------------------------------------------
// Query String
//-------------------------------------------------

inline HRESULT CVsRegKeyW::QueryValue(LPCOLESTR lpszValueName, BSTR * pbstrValue)
{
  VsRegKey_ExpectedPtr(pbstrValue);

  * pbstrValue = NULL;

  DWORD cbBuffer = 0;
  DWORD dwType = 0;
  LONG lRes = RegQueryValueExW(m_hKey, lpszValueName, NULL, &dwType, NULL, &cbBuffer);

  ATLASSERT((lRes!=ERROR_SUCCESS) || (dwType == REG_SZ) ||
		   (dwType == REG_MULTI_SZ) || (dwType == REG_EXPAND_SZ));

  if (ERROR_SUCCESS == lRes && cbBuffer)
  {
    CComBSTR bstrValue(cbBuffer / sizeof(WCHAR) - 1);
    if(bstrValue != NULL)
    {
      lRes = RegQueryValueExW(m_hKey, lpszValueName, NULL,
				       NULL, (LPBYTE)(bstrValue.m_str), &cbBuffer);

      if(ERROR_SUCCESS == lRes && bstrValue.Length() != 0)
	 *pbstrValue = bstrValue.Detach();

    }
    else
      return E_OUTOFMEMORY;
  }

  return HRESULT_FROM_WIN32(lRes);
}

//-------------------------------------------------
// Query GUID
//-------------------------------------------------

inline HRESULT CVsRegKeyW::QueryValue(LPCOLESTR lpszValueName, GUID *pGuid)
{
  VsRegKey_ExpectedPtr(pGuid);

  CComBSTR bstrGuid;
  HRESULT hr = QueryValue(lpszValueName, &bstrGuid);
  if(SUCCEEDED(hr))
    hr = CLSIDFromString(bstrGuid, pGuid);

  return hr;
}

//-------------------------------------------------
// Set Value
//-------------------------------------------------

inline HRESULT WINAPI CVsRegKeyW::SetValue(HKEY hKeyParent, LPCOLESTR lpszKeyName, LPCOLESTR lpszValue, LPCOLESTR lpszValueName)
{
  ATLASSERT(lpszValue != NULL);
  CVsRegKeyW key;
  LONG lRes = key.Create(hKeyParent, lpszKeyName);
  if (lRes == ERROR_SUCCESS)
    lRes = key.SetValue(lpszValue, lpszValueName);
  return HRESULT_FROM_WIN32(lRes);
}

inline HRESULT CVsRegKeyW::SetKeyValue(LPCOLESTR lpszKeyName, LPCOLESTR lpszValue, LPCOLESTR lpszValueName)
{
  ATLASSERT(lpszValue != NULL);
  CVsRegKeyW key;
  LONG lRes = key.Create(m_hKey, lpszKeyName);
  if (lRes == ERROR_SUCCESS)
	  lRes = key.SetValue(lpszValue, lpszValueName);
  return HRESULT_FROM_WIN32(lRes);
}

//-------------------------------------------------
// Set DWORD
//-------------------------------------------------

inline LONG CVsRegKeyW::SetValue(LPCOLESTR lpszValueName, DWORD dwValue)
{
  ATLASSERT(m_hKey != NULL);
  LONG lRes = RegSetValueExW(m_hKey, lpszValueName, NULL, REG_DWORD,
	  (BYTE * const)&dwValue, sizeof(DWORD));
  return HRESULT_FROM_WIN32(lRes);
}

//-------------------------------------------------
// Set String
//-------------------------------------------------

inline LONG CVsRegKeyW::SetValue(LPCOLESTR lpszValueName, LPCOLESTR lpszValue)
{
  ATLASSERT(m_hKey != NULL);

  LONG lRes;

  if(!lpszValue)
  {
    lRes = RegSetValueExW(m_hKey, lpszValueName, NULL, REG_SZ,
	   NULL, 0);
    if(ERROR_SUCCESS != lRes) // SetValue with NULL fails on Win98
    {
      lRes = RegSetValueExW(m_hKey, lpszValueName, NULL, REG_SZ,
	  (BYTE * const)L"", sizeof(WCHAR));
    }
  }
  else
  {
    lRes = RegSetValueExW(m_hKey, lpszValueName, NULL, REG_SZ,
	  (BYTE * const)lpszValue, (DWORD)((wcslen(lpszValue)+1)*sizeof(WCHAR)));
  }
  return HRESULT_FROM_WIN32(lRes);
}

//-------------------------------------------------
// enum key
//-------------------------------------------------

inline HRESULT CVsRegKeyW::EnumKeyEx(DWORD dwIndex, BSTR * pbstrName, BSTR * pbstrClass, PFILETIME lpftLastWriteTime)
{
  WCHAR * pszName = NULL;
  WCHAR * pszClass = NULL;
  DWORD dwNameSize;
  DWORD dwClassSize;
  LONG  lRes;

  lRes = RegQueryInfoKeyW(m_hKey, NULL, NULL, NULL, NULL, &dwNameSize, &dwClassSize, NULL, NULL, NULL, NULL, NULL); 
  if(ERROR_SUCCESS == lRes)
  {
    dwNameSize++;
    dwClassSize++;
    if(pbstrName)
      pszName = (WCHAR *)_alloca(dwNameSize * sizeof(WCHAR));
    else
      dwNameSize = 0;

    if(pbstrClass)
      pszClass = (WCHAR *)_alloca(dwClassSize * sizeof(WCHAR));
    else
      dwClassSize = 0;

    lRes = RegEnumKeyExW(m_hKey, dwIndex, pszName, &dwNameSize, 
                NULL, pszClass, &dwClassSize, lpftLastWriteTime);
    if(ERROR_SUCCESS == lRes)
    {
      if(pbstrName)
        *pbstrName = ::SysAllocString(pszName);
      if(pbstrClass)
        *pbstrClass = ::SysAllocString(pszClass);
    }
  }
  return HRESULT_FROM_WIN32(lRes);
}

/*
inline HRESULT CVsRegKeyW::EnumKeyEx(DWORD dwIndex, BSTR * pbstrName, BSTR * pbstrClass, PFILETIME lpftLastWriteTime)
{
  WCHAR pszName[256];
  WCHAR szClass[256];
  DWORD dwNameSize = NUMBER_OF(szName);
  DWORD dwClassSize = NUMBER_OF(szClass);
  LONG  lRes;

  lRes = RegEnumKeyExW(m_hKey, dwIndex, szName, &dwNameSize, 
              NULL, szClass, &dwClassSize, lpftLastWriteTime);
  if(ERROR_SUCCESS == lRes)
  {
    if(pbstrName)
      *pbstrName = ::SysAllocString(szName);
    if(pbstrClass)
      *pbstrClass = ::SysAllocString(szClass);
  }

  return HRESULT_FROM_WIN32(lRes);
}
*/

inline HRESULT CVsRegKeyW::GetSubkeyCount(DWORD * pdwCount)
{
  LONG lRes = RegQueryInfoKeyW(m_hKey, NULL, NULL, NULL, pdwCount, NULL, NULL, NULL, NULL, NULL, NULL, NULL); 
  return HRESULT_FROM_WIN32(lRes);
} 

//-------------------------------------------------
// enum value
//-------------------------------------------------

inline HRESULT CVsRegKeyW::EnumValue(DWORD dwIndex, BSTR * pbstrName, DWORD * pType)
{
  VsRegKey_ExpectedPtr(pbstrName);

  WCHAR * pszName;
  DWORD dwNameSize;
  LONG lRes;

  lRes = RegQueryInfoKeyW(m_hKey, NULL, NULL, NULL, NULL, NULL, NULL, NULL, &dwNameSize, NULL, NULL, NULL); 
  if(ERROR_SUCCESS == lRes)
  {
    dwNameSize++;
    pszName = (WCHAR *)_alloca(dwNameSize * sizeof(WCHAR));

    lRes = RegEnumValueW(m_hKey, dwIndex, pszName, &dwNameSize, NULL, pType, NULL, NULL);
    if(ERROR_SUCCESS == lRes)
    {
      *pbstrName = ::SysAllocString(pszName);
    }
  }
  return HRESULT_FROM_WIN32(lRes);
}

/*
inline HRESULT CVsRegKeyW::EnumValue(DWORD dwIndex, BSTR * pbstrName, DWORD * pType)
{
  WCHAR szName[256];
  DWORD dwNameSize = NUMBER_OF(szName);

  LONG lRes = RegEnumValueW(m_hKey, dwIndex, szName, &dwNameSize, NULL, pType, NULL, NULL);

  if(ERROR_SUCCESS == lRes)
  {
    if(pbstrName)
      *pbstrName = ::SysAllocString(szName);
  }

  return HRESULT_FROM_WIN32(lRes);
}
*/

inline HRESULT CVsRegKeyW::GetValueCount(DWORD * pdwCount)
{
  LONG lRes = RegQueryInfoKeyW(m_hKey, NULL, NULL, NULL, NULL, NULL, NULL, pdwCount, NULL, NULL, NULL, NULL); 
  return HRESULT_FROM_WIN32(lRes);
} 



