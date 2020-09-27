#ifdef _CONSOLE
	#include <afxwin.h>
	#include <windows.h>
	#include <ostream.h>
	#include "registry.h"
	#define __WB_H__
#endif
#include "stdafx.h"
#include "registry.h"
#include "guiv1.h"

#define new DEBUG_NEW

#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
static BOOL g_bListOnly = FALSE;

#ifdef _CONSOLE
	static CString strKeyName = cstrKeyRoot;
#else
	#if defined(V4LOW)
		static CString strKeyName = cstrKeyRoot + "Developer Standard";
	#else
		static CString strKeyName = cstrKeyRoot + "DevStudio\\6.0";
	#endif
#endif

CString gstrKeyExt = "_AUTO";
	
inline const CString GetRegistryKeyName( void )
{
	return strKeyName + gstrKeyExt;
}

void ReplaceKey( const CString& strRestoreFile )
{
	DWORD dwDisp;
	HKEY hKeyMain;
	VERIFY( RegCreateKeyEx( HKEY_CURRENT_USER, GetRegistryKeyName(),
		0, "", REG_OPTION_NON_VOLATILE, KEY_WRITE | KEY_READ, NULL,
		&hKeyMain, &dwDisp )	== ERROR_SUCCESS );

	SushiRestoreKey( hKeyMain, strRestoreFile );
	RegCloseKey( hKeyMain );
}

void DeleteKey(void)
{
	DWORD dwDisp;
	HKEY hKeyMain;
	TRACE("Deleting key '%s' use caution...\n", GetRegistryKeyName());
	if (RegCreateKeyEx(HKEY_CURRENT_USER, GetRegistryKeyName(),	0, "", REG_OPTION_NON_VOLATILE, KEY_WRITE | KEY_READ, NULL, &hKeyMain, &dwDisp ) == ERROR_SUCCESS) {
		DeleteSubKeys(hKeyMain, GetRegistryKeyName());
		RegCloseKey(hKeyMain);
	}
}

void SushiRestoreKey( HKEY hKey, const CString& str )
{
#ifdef _WIN32
	if ( !str.IsEmpty() )
	{
        HANDLE hToken;
        TOKEN_PRIVILEGES tkp;

        OpenProcessToken( GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken );

        // Get the LUID for restore privilege
        LookupPrivilegeValue( NULL, SE_RESTORE_NAME, &tkp.Privileges[0].Luid );

        tkp.PrivilegeCount = 1;  // one privilege to set
        tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

	        // Get restore privilege for this process.
        AdjustTokenPrivileges( hToken, FALSE, &tkp, 0, (PTOKEN_PRIVILEGES)NULL, 0 );

	#ifdef _CONSOLE
		if ( IsNTKey( str ) )
		{
			if ( ::GetVersion() & 0x80000000 )	// Win32s or Chicago
				cout << endl << endl << "*** Cannot use an NT binary key on this system!" << endl;
			else
				RegRestoreKey( hKey, str, 0 );
		}
		else
		{
			cout << endl << "Removing subkeys";
	#endif

			DeleteSubKeys( hKey, GetRegistryKeyName() );

	#ifdef _CONSOLE
			cout << endl << "Removing values";
	#endif

			DeleteValues( hKey, GetRegistryKeyName() );

	#ifdef _CONSOLE
		cout << endl << "Restoring key from " << (LPCTSTR)str;
	#endif

			RestoreKey( hKey, str );

	#ifdef _CONSOLE
			cout << endl;
		}
	#endif

		// Reset privilege.
        tkp.Privileges[0].Attributes = SE_PRIVILEGE_USED_FOR_ACCESS;
        AdjustTokenPrivileges( hToken, FALSE, &tkp, 0, (PTOKEN_PRIVILEGES)NULL, 0 );
	}
#endif
}

BOOL IsNTKey( const CString& strRestoreFile )
{
	TRY
	{
		CString strBuffer;
		CFile keyFile( strRestoreFile, CFile::modeRead | CFile::shareDenyNone );
		VERIFY( keyFile.Read( strBuffer.GetBufferSetLength( 4 ), 4 ) == 4 );
		strBuffer.ReleaseBuffer( 4 );
		BOOL bNTKey = (strBuffer == "regf");
		keyFile.Close();
		return bNTKey;
	}
	CATCH_ALL( e )
		return FALSE;
	END_CATCH_ALL
}

void DeleteSubKeys( HKEY hKey, const CString& strKeyName )
{
	TRY
	{
		DWORD iSubKey = 0;
		TCHAR szSubKey[MAX_PATH+1];
		CStringArray* pKeyArray = new CStringArray;
		while ( RegEnumKey( hKey, iSubKey++, szSubKey, MAX_PATH ) == ERROR_SUCCESS )
			pKeyArray->Add( szSubKey );

		for ( int iCur = 0, iTop = pKeyArray->GetUpperBound(); iCur <= iTop; iCur++ )
		{
			HKEY hSubKey;
			CString strSubKeyName = strKeyName + '\\' + (*pKeyArray)[iCur];
			LONG lResult = RegOpenKeyEx( HKEY_CURRENT_USER, strSubKeyName, 0, KEY_WRITE | KEY_READ, &hSubKey );
			ASSERT( lResult == ERROR_SUCCESS );
			if ( lResult == ERROR_SUCCESS )
			{
				DeleteSubKeys( hSubKey, strSubKeyName );
				RegCloseKey( hSubKey );
			}
			if ( !g_bListOnly )
				VERIFY( RegDeleteKey( hKey, (*pKeyArray)[iCur] ) == ERROR_SUCCESS );
			#ifdef _CONSOLE
				cout << endl << '\t' << (LPCTSTR)strSubKeyName;
			#endif
		}
		delete pKeyArray;
	}
	CATCH_ALL( e )
		ASSERT( FALSE );	//shdn't get to this statement
	END_CATCH_ALL
}

void DeleteValues( HKEY hKey, const CString& strKeyName )
{
	TRY
	{
		DWORD iValue = 0;
		DWORD cchValue = MAX_PATH;
		TCHAR szValue[MAX_PATH];
		CStringArray* pValueArray = new CStringArray;
		while ( RegEnumValue( hKey, iValue++, szValue, &cchValue, NULL, NULL, NULL, NULL ) == ERROR_SUCCESS )
		{
			pValueArray->Add( szValue );
			cchValue = MAX_PATH;
		}
		for ( int iCur = 0, iTop = pValueArray->GetUpperBound(); iCur <= iTop; iCur++ )
		{
			if ( !g_bListOnly )
				VERIFY( RegDeleteValue( hKey, (LPTSTR)(LPCTSTR)(*pValueArray)[iCur] ) == ERROR_SUCCESS );
			#ifdef _CONSOLE
				cout << endl << '\t' << (LPCTSTR)(*pValueArray)[iCur];
			#endif
		}
		delete pValueArray;
	}
	CATCH_ALL( e )
		ASSERT( FALSE );	//shdn't get to this statement
	END_CATCH_ALL
}

inline HKEY AddSubKey( HKEY hKey, LPCTSTR szName )
{
	DWORD dwDisp;
	HKEY hSubKey = NULL;
	if ( !g_bListOnly )
		VERIFY( RegCreateKeyEx( hKey, szName, 0, "", REG_OPTION_NON_VOLATILE, KEY_WRITE | KEY_READ, NULL,
								&hSubKey, &dwDisp )	== ERROR_SUCCESS );
	#ifdef _CONSOLE
		cout << endl << '\t' << szName;
	#endif
	return hSubKey;
}

inline void AddValue( HKEY hKey, LPTSTR szName, DWORD dwType, CONST BYTE* pbData, DWORD cbData )
{
	if ( !g_bListOnly )
		VERIFY( RegSetValueEx( hKey, szName, 0, dwType, pbData, cbData ) == ERROR_SUCCESS );
	#ifdef _CONSOLE
		cout << endl << "\t\t" << szName;
	#endif
}

void RestoreKey( HKEY hKey, const CString& strRestoreFile )
{
	TRY
	{
		CStdioFile file;
		if ( !file.Open( strRestoreFile, CFile::modeRead | CFile::typeText ) )
			return;

		char acOldName[256];
		VERIFY( file.ReadString( acOldName, 255 ) );
		char* pchCol = acOldName;
		do									// find colon
		{
			if ( IsDBCSLeadByte( *pchCol ) )
				pchCol += 2;
			else
				pchCol++;
		}
		while ( *pchCol != ':' );
		while ( isspace( *++pchCol ) );			// find next non-space
		int nCol = pchCol - acOldName;
		acOldName[strlen( acOldName ) - 1] = '\\';	// append backslash, and remove line-feed

		char acClassName[256];
		VERIFY( file.ReadString( acClassName, 255 ) );
		char acLastWrite[256];
		VERIFY( file.ReadString( acLastWrite, 255 ) );		// Last Write Time

		HKEY hSubKey = hKey;

		char acBuf[256];
		while ( file.ReadString( acBuf, 255 ) != NULL )
		{
			ASSERT( acBuf[strlen( acBuf ) - 1] == '\n' );		// if this fails, the line was longer than 255 characters
			acBuf[strlen( acBuf ) - 1] = '\0';				// remove line-feed

			if ( strcmp( acBuf," " ) == 0 )
				continue;	// no sense worrying about blank lines

			if ( (strlen( acBuf ) > strlen( acOldName ) ) &&
				(strncmp( acOldName + nCol, acBuf + nCol, strlen( acOldName + nCol ) ) == 0) )
			{							// this is a sub-key
				VERIFY( file.ReadString( acClassName, 255 ) );
				VERIFY( file.ReadString( acLastWrite, 255 ) );		// Last Write Time
				if ( hSubKey != hKey )
					RegCloseKey( hSubKey );
				hSubKey = AddSubKey( hKey, acBuf + strlen( acOldName ) );	// new key using only relative name of key
			}
			else						// this is a value
			{
				char acValueName[256];
				VERIFY( file.ReadString( acValueName, 255 ) );
				acValueName[strlen( acValueName )-1] = '\0';
				char acValueType[256];
				VERIFY( file.ReadString( acValueType, 255 ) );
				acValueType[strlen( acValueType )-1] = '\0';
				if ( strcmp( acValueType + nCol, "REG_SZ" ) == 0 )
				{
					char acString[2048];
					VERIFY( file.ReadString( acString, 2047 ) );
					ASSERT( strlen( acString ) > (size_t)nCol );				// failure means no string was read
					ASSERT( acString[strlen( acString )-1] == '\n' );	// failure means the string was bigger than our buffer
					acString[strlen( acString )-1] = '\0';
					AddValue( hSubKey, acValueName + nCol, REG_SZ, (CONST BYTE*)acString + nCol, strlen( acString + nCol ) + 1 );
				}
				else if ( strcmp( acValueType + nCol, "REG_MULTI_SZ" ) == 0 )
				{
					CStringArray sa;
					char acString[2048];
					VERIFY( file.ReadString( acString, 2047 ) );
					ASSERT( acString[strlen( acString )-1] == '\n' );	// failure means the string was bigger than our buffer
					acString[strlen( acString )-1] = '\0';
					sa.Add( acString + nCol );

					while ( TRUE )
					{
						VERIFY( file.ReadString( acString, 2047 ) );
						ASSERT( acString[strlen( acString )-1] == '\n' );	// failure means the string was bigger than our buffer
						acString[strlen( acString )-1] = '\0';
						ASSERT( strncmp( acString, "+++++++++ ", 10 ) == 0 );
						if ( acString[10] == '\0' )
							break;		// end of string list
						sa.Add( acString + 10 );	// add string to array
					}

					int nBytes = 1;		// final NULL
					int n;
					for ( n = 0; n < sa.GetSize(); n++ )
						nBytes += sa.GetAt( n ).GetLength() + 1;	// string length including NULL

					char* pchData = new char[nBytes];
					char* pch = pchData;
					for ( n = 0; n < sa.GetSize(); n++ )
					{
						strcpy( pch, sa.GetAt( n ) );
						pch += sa.GetAt( n ).GetLength() + 1;
					}
					*pch = '\0';		// final NULL

					AddValue( hSubKey, acValueName + nCol, REG_MULTI_SZ, (CONST BYTE*)pchData, nBytes );
					delete[] pchData;
				}
				else if ( strcmp( acValueType + nCol, "REG_DWORD" ) == 0 )
				{
					char acString[2048];
					VERIFY( file.ReadString( acString, 2047 ) );
					DWORD dw;
					if ( (acString[nCol] == '0') && (acString[nCol+1] == 'x') )
						sscanf( acString + nCol, "%x", &dw );
					else
						sscanf( acString + nCol, "%d", &dw );
					AddValue( hSubKey, acValueName + nCol, REG_DWORD, (CONST BYTE*)&dw, sizeof(DWORD) );
				}
				else if ( strcmp( acValueType + nCol, "REG_BINARY" ) == 0 )
				{
					char acLine[256];
					VERIFY( file.ReadString( acLine, 255 ) );		// "Data:"
					DWORD dwLine = 0x0;
					int nDataSize = 2048;
					BYTE* pbData = new BYTE[nDataSize];		// initially try this size
					int nBytes = 0;
					BYTE* pb = pbData;

					while ( TRUE )
					{
						VERIFY( file.ReadString( acLine, 255 ) );		// "00000000"
						CString strLine;
						strLine.Format( "%08x", dwLine );
						if ( strncmp( strLine, acLine, 8 ) != 0 )
							break;	// we're done, or something, cause this is not the next line of binary data

						char* pchData = acLine + 11;
						for ( int n = 0; n < 16; n++, pchData += 3, nBytes++, pb++ )
						{
							if ( n == 8 )
								pchData += 2;		// skip " - "
							if ( !isxdigit( *pchData ) )
								break;

							if ( (pchData[0] >= 'a') && (pchData[0] <= 'f') )
								*pb = ((pchData[0]) - 'a') + 10;
							else if ( (pchData[0] >= 'A') && (pchData[0] <= 'F') )
								*pb = ((pchData[0]) - 'A') + 10;
							else if ( isdigit( pchData[0] ) )
								*pb = ((pchData[0]) - '0');
							*pb *= 0x10;
							if ( (pchData[1] >= 'a') && (pchData[1] <= 'f') )
								*pb += ((pchData[1]) - 'a') + 10;
							else if ( (pchData[1] >= 'A') && (pchData[1] <= 'F') )
								*pb += ((pchData[1]) - 'A') + 10;
							else if ( isdigit( pchData[1] ) )
								*pb += ((pchData[1]) - '0');
						}
						if ( n < 16 )
							break;		// end of data

						if ( nBytes > nDataSize )
						{
							ASSERT( FALSE );		// NYI - grow data size
						}

						dwLine += 0x10;			// increment the number expected on the next line
					}

					AddValue( hSubKey, acValueName + nCol, REG_BINARY, pbData, nBytes );
					delete[] pbData;
				}
				else
					ASSERT( FALSE );		// REG_TYPE NYI
			}
		}

		if ( hSubKey != hKey )
			RegCloseKey( hSubKey );
	}
	CATCH_ALL( e )
		ASSERT( FALSE );	//shdn't get to this statement
	END_CATCH_ALL
}

#ifdef _CONSOLE
	void main( int argc, char* argv[] )
	{
		CString strValue;
		CCmdLine cmdline( argc, argv );
		if ( cmdline.GetBooleanSwitch( "?", FALSE ) )
		{
			cout << endl
				<< "Initializes a registry key in HKEY_CURRENT_USER\\Software\\Microsoft." << endl << endl
				<< "SETKEY [/list] [/name=keyname] [/key=keyfile] [/i=keysuffix]" << endl << endl
				<< "  /list\t\tList actions. (Does not modify key)" << endl
				<< "  /name\t\tName of key.  Default is Visual C++ 2.0" << endl
				<< "  /key\t\tName of file.  Default is SUSHI.KEY" << endl
				<< "  /i\t\tSuffix for key.  Default is _AUTO  (Use /i=no to omit suffix)" << endl;
				return;
		}
		cmdline.GetTextSwitch( "NAME", strValue, "Visual C++ 2.0" );
		strKeyName += strValue;
		if ( cmdline.GetBooleanSwitch( "I", TRUE ) )
		{
			cmdline.GetTextSwitch( "I", strValue, "_AUTO" );
			strKeyName += strValue;
		}
		cout << endl << "Initializing HKEY_CURRENT_USER\\" << (LPCTSTR)GetRegistryKeyName();
		g_bListOnly = cmdline.GetBooleanSwitch( "LIST", FALSE );
		cmdline.GetTextSwitch( "KEY", strValue, "SUSHI.KEY" );
		ReplaceKey( strValue );
	}

	/*	CCmdLine Constructor
	 *		Reads command line strings and stores switches (arguments preceded by '-' or '/')
	 *		in a CMapStringToString object for later retrieval.
	 *		Example: sniff.exe -c="-run=false post=true" -run -post:false -logdir=h:\logs
	 *		
	 *		This would map in the following manner:
	 *		
	 *		C 		->		"-run=false post=true"
	 *		RUN		->		""
	 *		POST	->		"false"
	 *		LOGDIR	->		"h:\logs"
	 *		
	 *		Note that the case of switch names is always ignored, but the case of their
	 *		value is preserved.
	 */
	CCmdLine::CCmdLine( int argc, char* argv[] )
	{
		CString strA, strV;

		for ( int iNext = 0; iNext < argc; iNext++ )
		{	
			ParseSwitch( argv[iNext], strA, strV );

			if ( strA.GetLength() )
			{
				strA.MakeUpper();
				m_SwitchMap.SetAt( strA, strV );
			}
		}
	}

	int CCmdLine::ParseSwitch( LPCSTR szCL, CString &strA, CString &strV )
	{
		strA = strV = "";

		if ( !szCL || !szCL[0] )
			return 0;

		int iNext = FindNextSwitch( szCL );
		if ( iNext == -1 )
			return -1;

		CString strCL = szCL + iNext;

		int iAssign = strCL.FindOneOf( ":=" );
		int iDelim = strCL.FindOneOf( " ,\t\"" );

		if ( (iDelim != -1) && (iDelim < iAssign) )
			iAssign = -1;

		if ( iDelim == 0 )	// if delimiter is next
			return 1;

		if ( (iAssign == -1) && (iDelim == -1) )	// if last parameter & no assignment
		{
			strA = strCL;
			return strCL.GetLength();	// point at '\0';
		}

		if ( iAssign == -1 )	// but iDelim > -1
		{
			strA = strCL.Left( iDelim );	// leave off delimiter
			return 1 + strA.GetLength();	// point past delimiter
		}

		if ( iDelim == -1 )	// if last parameter & uses assignment
		{
			strA = strCL.Left( iAssign );
			strV = strCL.Mid( 1 + iAssign );
			return strCL.GetLength();	// point at '\0';
		}

		// otherwise both are > -1

		if ( strCL[iDelim] == '"' )	// uh oh, they're using quotes
		{
			strA = strCL.Left( iAssign );
			if ( iDelim != 1 + iAssign )	// the user messed up, but try to cope
			{
				strV = strCL.Mid( 1 + iAssign, iDelim - iAssign - 1 );
				return -1;	// they improperly used quotes
			}
			strV = strCL.Mid( 2 + iAssign );	// the quoted text
			int iEndQuote = strV.Find( '"' );
			if ( iEndQuote != -1 )
			{
				strV = strV.Left( iEndQuote );
				return iEndQuote + 3 + iAssign;	// point past 2nd quote
			}
			else
				return -1;	// they didn't give a second quote
		}
		else		// no quotes, just x=y or x:y
		{
			strA = strCL.Left( iAssign );
			strV = strCL.Mid( 1 + iAssign, iDelim - iAssign - 1 );
			return 1 + iDelim;	// point past delimiter
		}
	}

	int CCmdLine::FindNextSwitch( LPCSTR szCL )
	{
		CString strCL = szCL;
		int i = strCL.FindOneOf( "-/" );
		if ( i == -1 )
			return -1;
		CString strX = strCL.Mid( i+1 );
		if ( strX.FindOneOf( " \t-/" ) == 0 )
			return FindNextSwitch( 2+i+szCL );
		return i+1;
	}


	/*	GetTextSwitch
	 *		This function returns the given value of a command-line switch.
	 *		Example: sniff.exe -c=" -run:0 -X:1 " -run:yes -post:"false" -logdir=h:\logs
	 *		
	 *		GetTextSwitch("c", strV): strV == " -run:0 -X:1 "
	 *		GetTextSwitch("run", strV): strV == "yes"
	 *		GetTextSwitch("RUN", strV): strV == "yes"
	 *		GetTextSwitch("LogDir", strV): strV == "h:\logs"
	 */
	BOOL CCmdLine::GetTextSwitch( LPCSTR szArg, CString &strR, LPCSTR szDefault /*=""*/ )
	{
		if ( (!szArg) || (!szArg[0]) )
		{
			strR = szDefault;
			return FALSE;
		}

		CString strArg = szArg;
		strArg.MakeUpper();

		if ( m_SwitchMap.Lookup( strArg, strR) )
			return TRUE;
		else
		{
			strR = szDefault;
			return FALSE;
		}

	}

	int CCmdLine::GetIntSwitch( LPCSTR szSwitch, int nDefault /*=0*/ )
	{
		CString strV;
	
		if ( GetTextSwitch( szSwitch, strV ) )
			return atoi( strV );
	
		return nDefault;
	}

	#define MAX_FALSE_STR 3
	LPCSTR aszFalseStr[1+MAX_FALSE_STR] = {"0","NO","FALSE","OFF"};
	#define MAX_TRUE_STR 3
	LPCSTR aszTrueStr[1+MAX_TRUE_STR] = {"1","YES","TRUE","ON"};

	/*	ParseBoolean
	 *		Used internally by GetBooleanSwitch.  Interprets text as a Boolean value.
	 */
	BOOL CCmdLine::ParseBoolean( LPCSTR szV, BOOL bDefault /*=FALSE*/ )
	{
		if ( (!szV) || (!szV[0]) )
			return TRUE;

		int i;

		CString strV( szV );
		strV.MakeUpper();

		for ( i = 0; i <= MAX_FALSE_STR; i++ )
		{
			if ( strV == aszFalseStr[i] )
				return FALSE;
		}

		for ( i = 0; i <= MAX_TRUE_STR; i++ )
		{
			if ( strV == aszTrueStr[i] )
				return TRUE;
		}

		return bDefault;
	}

	/*	GetBooleanSwitch
	 *		This function returns a Boolean interpretation of a command-line switch.
	 *		FALSE, NO, OFF, and 0 are interpreted as FALSE.
	 *		TRUE, YES, ON, 1, and no value are interpreted as TRUE.
	 *		Example: sniff.exe -debug=0 -run -post:"false" -logdir=h:\logs
	 *		
	 *		GetBooleanSwitch("debug") == FALSE
	 *		GetBooleanSwitch("run") == TRUE
	 *		GetBooleanSwitch("RUN") == TRUE
	 *		GetBooleanSwitch("Post") == FALSE
	 */
	BOOL CCmdLine::GetBooleanSwitch( LPCSTR szArg, BOOL bDefault /* = FALSE */ )
	{
		CString strV, strArg( szArg );
	
		if ( GetTextSwitch( strArg, strV ) )
		{
			return ParseBoolean( strV, bDefault );
		}

		if ( GetTextSwitch( "no"+strArg, strV ) )
		{
			return !ParseBoolean( strV, bDefault );
		}
	
		return bDefault;
	}
#endif	//_CONSOLE
