// RegScan.h

#define _MAX_REG_BUFFER (4095)
class CRegistryScanner
{
// Construction
public:
	CRegistryScanner();
	~CRegistryScanner();

// Operations
public:
	BOOL Open(HKEY hKey, BSTR bstrSubKey);
	void Close();

	BOOL EnumKey(BSTR *strKeyName);

	BOOL GetValue(BSTR bstrKey, BSTR *rValue);
	BOOL QueryStringValue(BSTR bstrValueName, CStringW& rstrValue) throw();

// Attributes
public:
	HKEY m_hKeyRoot;
	DWORD m_iSubKey;
	CComBSTR m_bstrKey;
};
