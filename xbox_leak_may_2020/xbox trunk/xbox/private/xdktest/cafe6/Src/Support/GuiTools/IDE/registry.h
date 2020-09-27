#ifndef __REGISTRY_H__
#define __REGISTRY_H__

#ifndef __AFXWIN_H__
	#error include 'afxwin.h' before including this file
#endif

#ifdef _CONSOLE
	class CCmdLine
	{
	public:
		CCmdLine( int argc, char* argv[] );
		BOOL GetTextSwitch( LPCSTR szSwitch, CString& strValue, LPCSTR szDefault = "" );
		int GetIntSwitch( LPCSTR szSwitch, int nDefault = 0 );
		BOOL GetBooleanSwitch( LPCSTR szSwitch, BOOL bDefault = FALSE );
	protected:
		int FindNextSwitch( LPCSTR szCL );
		int ParseSwitch( LPCSTR szCL, CString &strA, CString &strV );
		BOOL ParseBoolean( LPCSTR szText, BOOL bDefault );
	private:
		CMapStringToString m_SwitchMap;
	};
#endif

void ReplaceKey( const CString& strRestoreFile );
void DeleteKey(void);
inline const CString GetRegistryKeyName( void );
void SushiRestoreKey( HKEY hKey, const CString& strRestoreFile );
BOOL IsNTKey( const CString& strRestoreFile );
void DeleteSubKeys( HKEY hKey, const CString& strKeyName );
void DeleteValues( HKEY hKey, const CString& strKeyName );
inline HKEY AddSubKey( HKEY hKey, LPCTSTR szName );
inline void AddValue( HKEY hKey, LPTSTR szName, DWORD dwType, CONST BYTE* pbData, DWORD cbData );
void RestoreKey( HKEY hKey, const CString& strRestoreFile );

// specify the extension to the key
extern CString gstrKeyExt;

#endif //__REGISTRY_H__
