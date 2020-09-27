//
//	Registry I/O class
//
//	Author: Shinji Chiba 1997-1-23
//

#include "stdafx.h"

REGISTRY::REGISTRY()
{
	hMainKey = NULL;
}

REGISTRY::~REGISTRY()
{
}

void REGISTRY::Init( HKEY hKey, char *szName )
{
	hMainKey = hKey;
	lstrcpy( szSubkey, szName );
}

BOOL REGISTRY::GetString( char *szName, char *szValue )
{
	HKEY hKey;
	DWORD dwType = REG_SZ;
	DWORD dwByte = REG_STRMAX;

	if ( ! hMainKey ) return FALSE;
	if ( RegOpenKeyEx( hMainKey, szSubkey, 0, KEY_READ, &hKey ) ) return FALSE;
	if( RegQueryValueEx( hKey, szName, NULL, &dwType, (LPBYTE) szValue, &dwByte )) return FALSE;
	RegCloseKey( hKey );
	return TRUE;
}

BOOL REGISTRY::GetValue( char *szName, LPDWORD dwRet )
{
	HKEY hKey;
	DWORD dwType = REG_DWORD;
	DWORD dwByte = REG_STRMAX;

	if ( ! hMainKey ) return FALSE;
	if ( RegOpenKeyEx( hMainKey, szSubkey, 0, KEY_READ, &hKey ) ) return FALSE;
	if( RegQueryValueEx( hKey, szName, NULL, &dwType, (LPBYTE) dwRet, &dwByte )) return FALSE;
	RegCloseKey( hKey );
	return TRUE;
}

BOOL REGISTRY::SetString( char *szName, char *szData )
{
	HKEY hKey;
	DWORD dw;
	LONG lResult;

	if ( ! hMainKey ) return FALSE;
	RegCreateKeyEx( hMainKey, szSubkey, 0, "", REG_OPTION_NON_VOLATILE, KEY_WRITE, NULL, &hKey, &dw );
	lResult = RegSetValueEx( hKey, szName, 0, REG_SZ, (CONST LPBYTE) szData, MyStrlen( szData ) );
	RegCloseKey( hKey );
	return TRUE;
}

BOOL REGISTRY::SetValue( char *szName, DWORD dwData )
{
	HKEY hKey;
	DWORD dw;

	if ( ! hMainKey ) return FALSE;
	RegCreateKeyEx( hMainKey, szSubkey, 0, "", REG_OPTION_NON_VOLATILE, KEY_WRITE, NULL, &hKey, &dw );
	RegSetValueEx( hKey, szName, 0, REG_DWORD, (CONST LPBYTE) &dwData, sizeof(DWORD) );
	RegCloseKey( hKey );
	return TRUE;
}

BOOL REGISTRY::Delete( char *sub/* = NULL */, HKEY main/* = NULL */ )
{
	if ( ! main )
	{
		if ( ! hMainKey ) return FALSE;
		main = hMainKey;
	}
	if ( sub ) RegDeleteKey( main, sub );
	else RegDeleteKey( main, szSubkey );
	return TRUE;
}
