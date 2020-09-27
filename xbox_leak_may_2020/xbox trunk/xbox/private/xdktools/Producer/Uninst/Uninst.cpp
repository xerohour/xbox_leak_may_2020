
#include <windows.h>
#include <process.h>
#include <tchar.h>
#include <ole2.h>

BOOL GetBaseDir(LPTSTR,LPTSTR);
void SplitName(LPCTSTR,LPTSTR,LPTSTR,TCHAR);

TCHAR szBaseDir[_MAX_PATH];
static char _szDllUnregSvr[] = "DllUnregisterServer";
#define MAX_STRING 1024

BOOL WINAPI DllMain(HINSTANCE hinstDLL,DWORD fdwReason,LPVOID lpvReserved)
{
   	return (1);
}

extern "C"
{

void UnRegister( LPCTSTR szName )
{
	// Load the library.	
	HINSTANCE hLib = LoadLibrary(szName);
	if (hLib >= (HINSTANCE)HINSTANCE_ERROR)
	{
		HRESULT (FAR STDAPICALLTYPE * lpDllEntryPoint)(void);

		// Find the entry point.		
		(FARPROC&)lpDllEntryPoint = GetProcAddress(hLib, _szDllUnregSvr);
		if (lpDllEntryPoint != NULL)
		{
			if (SUCCEEDED((*lpDllEntryPoint)()))
			{
				// We've succeeded - display fireworks here
				//TCHAR szMessage[_MAX_PATH];
				//wsprintf(szMessage,_T("Successfully UnRegistered:\n\n%s"),szName);
				//MessageBox(GetFocus(), szMessage ,"",MB_OK);
			}
		}
		FreeLibrary(hLib);
	}
}

void DeleteKeyHelper( LPCTSTR szName )
{
	HKEY hKey;
	if( ERROR_SUCCESS == RegOpenKeyEx( HKEY_LOCAL_MACHINE, szName, 0, KEY_QUERY_VALUE, &hKey ) )
	{
		DWORD dwSubKeys = 0;
		DWORD dwValues = 0;
		if( ERROR_SUCCESS == RegQueryInfoKey( hKey, NULL, NULL, NULL, &dwSubKeys, NULL, NULL, &dwValues, NULL, NULL, NULL, NULL ) )
		{
			if( dwSubKeys == 0 )
			{
				// Only delete the key if it has zero subkeys
				RegDeleteKey( HKEY_LOCAL_MACHINE, szName );
			}
			RegCloseKey( hKey );
		}
		else
		{
			RegCloseKey( hKey );
		}
	}
}

int UninstInitialize(HWND hwndDlg, HWND hInstance, long int lReserved)
{
	LPTSTR lpStr;
	TCHAR szCurrentDirectory[_MAX_PATH];
	TCHAR szTemp[_MAX_PATH];
	TCHAR szTemp2[_MAX_PATH];
	
	//Figure where the LOG and DLL file are located
	lpStr = GetCommandLine();
	
	GetBaseDir(lpStr,szBaseDir);
	
	_tcscpy( szTemp, szBaseDir );
	_tcscat( szTemp, _T("DMUSProd.exe") );
	_tcscpy( szTemp2, _T("/UNREGSERVER") );

	// Save the current directory
	if( 0 == GetCurrentDirectory( _MAX_PATH, szCurrentDirectory ) )
	{
		szCurrentDirectory[0] = 0;
	}

	// Set the current directory to Producer's installation directory
	SetCurrentDirectory( szBaseDir );

	int nResult;
	nResult = _tspawnl( _P_WAIT, szTemp, szTemp, szTemp2, NULL );
	Sleep( 10000 );
	/*
	if( !nResult )
	{
		// We've succeeded - display fireworks here
		TCHAR szMessage[_MAX_PATH];
		wsprintf(szMessage,_T("Successfully UnRegistered:\n\n%s"),szTemp);
		MessageBox(GetFocus(), szMessage ,"",MB_OK);
	}
	else
	{
		// We've failed - display bomb here
		TCHAR szMessage[_MAX_PATH];
		wsprintf(szMessage,_T("UnSuccessfully UnRegistered:\n\n%s"),szTemp);
		MessageBox(GetFocus(), szMessage ,"",MB_OK);
	}
	*/

	if (SUCCEEDED(OleInitialize(NULL)))
	{
		_tcscpy( szTemp, szBaseDir );
		_tcscat( szTemp, _T("ContainerDesigner.ocx") );
		UnRegister( szTemp );

		_tcscpy( szTemp, szBaseDir );
		_tcscat( szTemp, _T("AudioPathDesigner.ocx") );
		UnRegister( szTemp );

		/*
		_tcscpy( szTemp, szBaseDir );
		_tcscat( szTemp, _T("MelGenStripMgr.dll") );
		UnRegister( szTemp );
		*/

		_tcscpy( szTemp, szBaseDir );
		_tcscat( szTemp, _T("XboxAddin.ocx") );
		UnRegister( szTemp );

		_tcscpy( szTemp, szBaseDir );
		_tcscat( szTemp, _T("xboxsynth.dll") );
		UnRegister( szTemp );

		_tcscpy( szTemp, szBaseDir );
		_tcscat( szTemp, _T("ParamStripMgr.dll") );
		UnRegister( szTemp );

		_tcscpy( szTemp, szBaseDir );
		_tcscat( szTemp, _T("WaveStripMgr.dll") );
		UnRegister( szTemp );

		_tcscpy( szTemp, szBaseDir );
		_tcscat( szTemp, _T("MarkerStripMgr.dll") );
		UnRegister( szTemp );

		_tcscpy( szTemp, szBaseDir );
		_tcscat( szTemp, _T("ToolGraphDesigner.ocx") );
		UnRegister( szTemp );

		/*
		_tcscpy( szTemp, szBaseDir );
		_tcscat( szTemp, _T("SongDesigner.ocx") );
		UnRegister( szTemp );
		*/

		_tcscpy( szTemp, szBaseDir );
		_tcscat( szTemp, _T("SegmentStripMgr.dll") );
		UnRegister( szTemp );

		_tcscpy( szTemp, szBaseDir );
		_tcscat( szTemp, _T("ScriptStripMgr.dll") );
		UnRegister( szTemp );

		_tcscpy( szTemp, szBaseDir );
		_tcscat( szTemp, _T("ScriptDesigner.ocx") );
		UnRegister( szTemp );

		_tcscpy( szTemp, szBaseDir );
		_tcscat( szTemp, _T("SegmentDesigner.ocx") );
		UnRegister( szTemp );

		_tcscpy( szTemp, szBaseDir );
		_tcscat( szTemp, _T("StyleDesigner.ocx") );
		UnRegister( szTemp );

		_tcscpy( szTemp, szBaseDir );
		_tcscat( szTemp, _T("ChordMapDesigner.ocx") );
		UnRegister( szTemp );

		_tcscpy( szTemp, szBaseDir );
		_tcscat( szTemp, _T("DLSDesigner.ocx") );
		UnRegister( szTemp );

		_tcscpy( szTemp, szBaseDir );
		_tcscat( szTemp, _T("BandEditor.ocx") );
		UnRegister( szTemp );

		_tcscpy( szTemp, szBaseDir );
		_tcscat( szTemp, _T("PanVol.ocx") );
		UnRegister( szTemp );

		_tcscpy( szTemp, szBaseDir );
		_tcscat( szTemp, _T("ADSREnvelope.ocx") );
		UnRegister( szTemp );

		_tcscpy( szTemp, szBaseDir );
		_tcscat( szTemp, _T("RegionKeyboard.ocx") );
		UnRegister( szTemp );

		_tcscpy( szTemp, szBaseDir );
		_tcscat( szTemp, _T("CommandStripMgr.dll") );
		UnRegister( szTemp );

		_tcscpy( szTemp, szBaseDir );
		_tcscat( szTemp, _T("ChordStripMgr.dll") );
		UnRegister( szTemp );

		_tcscpy( szTemp, szBaseDir );
		_tcscat( szTemp, _T("ChordMapStripMgr.dll") );
		UnRegister( szTemp );

		_tcscpy( szTemp, szBaseDir );
		_tcscat( szTemp, _T("MIDIStripMgr.dll") );
		UnRegister( szTemp );

		_tcscpy( szTemp, szBaseDir );
		_tcscat( szTemp, _T("SignPostStripMgr.dll") );
		UnRegister( szTemp );

		_tcscpy( szTemp, szBaseDir );
		_tcscat( szTemp, _T("LyricStripMgr.dll") );
		UnRegister( szTemp );

		_tcscpy( szTemp, szBaseDir );
		_tcscat( szTemp, _T("StyleRefStripMgr.dll") );
		UnRegister( szTemp );

		_tcscpy( szTemp, szBaseDir );
		_tcscat( szTemp, _T("ChordMapRefStripMgr.dll") );
		UnRegister( szTemp );

		_tcscpy( szTemp, szBaseDir );
		_tcscat( szTemp, _T("BandStripMgr.dll") );
		UnRegister( szTemp );

		_tcscpy( szTemp, szBaseDir );
		_tcscat( szTemp, _T("TempoStripMgr.dll") );
		UnRegister( szTemp );

		_tcscpy( szTemp, szBaseDir );
		_tcscat( szTemp, _T("SequenceStripMgr.dll") );
		UnRegister( szTemp );

		_tcscpy( szTemp, szBaseDir );
		_tcscat( szTemp, _T("MuteStripMgr.dll") );
		UnRegister( szTemp );

		_tcscpy( szTemp, szBaseDir );
		_tcscat( szTemp, _T("TimeSigStripMgr.dll") );
		UnRegister( szTemp );

		_tcscpy( szTemp, szBaseDir );
		_tcscat( szTemp, _T("Msflxgrd.ocx") );
		UnRegister( szTemp );

		_tcscpy( szTemp, szBaseDir );
		_tcscat( szTemp, _T("Timeline.dll") );
		UnRegister( szTemp );

		_tcscpy( szTemp, szBaseDir );
		_tcscat( szTemp, _T("Conductor.dll") );
		UnRegister( szTemp );

		OleUninitialize();
	}

	if( szCurrentDirectory[0] )
	{
		SetCurrentDirectory( szCurrentDirectory );
	}

	// Now check to see if HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\DMUSProducer\Components,
	// HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\DMUSProducer\Container Objects,
	// HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\DMUSProducer\StripEditors, and
	// HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\DMUSProducer are empty, and remove them if they are

	DeleteKeyHelper( _T("SOFTWARE\\Microsoft\\DMUSProducer\\Components") );
	DeleteKeyHelper( _T("SOFTWARE\\Microsoft\\DMUSProducer\\Container Objects") );
	DeleteKeyHelper( _T("SOFTWARE\\Microsoft\\DMUSProducer\\StripEditors") );
	DeleteKeyHelper( _T("SOFTWARE\\Microsoft\\DMUSProducer") );

	return(0);
}

int UninstUnInitialize(HWND hwndDlg, HWND hInstance, long int lReserved)
{
	return 0;
}

} ;

// Check the last character in a string to see if it is a given character
BOOL LastChar(LPTSTR szString,TCHAR chSearch)
{
	LPTSTR lpstr,lplast;

	if (lstrlen(szString)==0)
		return(FALSE);

	lplast=(LPTSTR)szString;
	lpstr=CharNext(szString);
	while (*lpstr)
	{
		lplast=lpstr;
		lpstr=CharNext(lplast);
	} 

	//if (IsDBCSLeadByte(*lplast))
	//	return(FALSE);

	if (*lplast==chSearch)
		return(TRUE);
	else
		return(FALSE);
}

//Get the directory where the DLL is found
BOOL GetBaseDir(LPTSTR lpLine,LPTSTR szBase)
{
	TCHAR szLine[_MAX_PATH],szFile[_MAX_PATH];
	LPTSTR ptr;
	int len;

	lstrcpy(szLine,lpLine);
	ptr=_tcsstr(szLine,_T("-c"));
	if (ptr==NULL)
	{
		szBase[0]=0;
		return(0);
	}

	ptr=CharNext(ptr); //skip past -c and up to filename portion
	ptr=CharNext(ptr); 

	//remove leading " if present
	//if ((IsDBCSLeadByte(*ptr)==0) && (*ptr=='"'))
	if ( *ptr=='"' )
		++ptr;
	lstrcpy(szFile,ptr);

	//remove trailing " if present
	if (LastChar(szFile,'"'))
	{
		len=lstrlen(szFile);
		szFile[len-1]=0;
	}

	//split the path\my.dll string
	SplitName(szFile,szBase,szLine,'\\');
	return(TRUE);
}

//Split a string at the last occurance of a given character
void SplitName(LPCTSTR szWholeName,LPTSTR szPath,LPTSTR szFile,TCHAR chSep)
{ 
	int i,len,j;
	LPTSTR lpstr,lplast;

	len=i=lstrlen(szWholeName);
	if (len==0)
	{
		szPath[0]=0; szFile[0]=0; return;
	}

	//First walk out to the end of the string
	lplast=(LPTSTR)szWholeName;
	lpstr=CharNext(szWholeName);
	while (*lpstr)
	{
		lplast=lpstr;
		lpstr=CharNext(lplast);
	} 

	//Now walk backward to the beginning
	while (*lplast)
	{
		/*
		if (IsDBCSLeadByte(*lplast))
		{
			lplast=CharPrev(szWholeName,lplast);
			if (lplast==szWholeName)
			{
				szPath[0]=0;
				lstrcpy(szFile,szWholeName);
				return;
			}
		}
		else*/ if (*lplast==chSep)
		{										 
			i=lplast-szWholeName+1;
			lstrcpyn(szPath,szWholeName,i+1);
			szPath[i]=0;
			j=len-i;
			lstrcpyn(szFile,&szWholeName[i],j+1);
			szFile[j]=0;
			return;
		}
		else
		{
			lplast=CharPrev(szWholeName,lplast);
			if (lplast==szWholeName)
			{
				szPath[0]=0;
				lstrcpy(szFile,szWholeName);
				return;
			}
		}
	}

	//Didn't find it so just default the values
	szPath[0]=0;
	lstrcpy(szFile,szWholeName);
}