#include "stdafx.h"
#pragma hdrstop

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

const DWORD DEBUG_INI_VERSION = 0x00000001;

typedef enum INITYPE			// The type of an INI file variable
{
	IT_UINT,
	IT_BOOL,
	IT_LONG,
	IT_BYTE
} INITYPE;

typedef struct INIVAL			// Describes a value that goes in the INI file
{
	INITYPE		it;				// The value's type
	union {
		void * pv;				// Where the value is kept in memory
		UINT * pu;
		BOOL * pbool;
		BYTE * pb;
		long * pl;
	};
	LPCTSTR		szKey;			// Name of the value
} INIVAL;

inline static void LoadIniValue(LPCTSTR szSection, const INIVAL *piv)
{
	switch (piv->it)
	{
	case IT_UINT:
		*piv->pu = GetRegInt(szSection, piv->szKey, *piv->pu);
		break;

	case IT_BOOL:
		*piv->pbool = GetRegInt(szSection, piv->szKey, *piv->pbool);
		break;

	case IT_LONG:
		*piv->pl = (long)GetRegInt(szSection, piv->szKey, *piv->pl);
		break;

	case IT_BYTE:
		*piv->pb = (BYTE)GetRegInt(szSection, piv->szKey, *piv->pb);
		break;
								  
	default:
		ASSERT(FALSE);
		break;
	}
}

inline static void SaveIniValue(LPCTSTR szSection, const INIVAL *piv)
{
	switch (piv->it)
	{
	case IT_UINT:
		// It's okay for us to write this as an int rather than uint
		WriteRegInt(szSection, piv->szKey, (int)*piv->pu);
		break;

	case IT_BOOL:
		WriteRegInt(szSection, piv->szKey, (int)*piv->pbool);
		break;

	case IT_LONG:
		WriteRegInt(szSection, piv->szKey, *piv->pl);
		break;

	case IT_BYTE:
		WriteRegInt(szSection, piv->szKey, *piv->pb);
		break;

	default:
		ASSERT(FALSE);
		break;
	}
}

static LPCTSTR cszGeneral               = _T("General");
static LPCTSTR cszDebug                 = _T("Debug");
static LPCTSTR cszVersion               = _T("Version");
static LPCTSTR cszRunDebugMode          = _T("DebugMode");
static LPCTSTR cszExtendedRegisters     = _T("ExtendedRegisters");
static LPCTSTR cszOleRpc                = _T("OleRpcDebugging");
static LPCTSTR cszReturnValues          = _T("ShowReturnValues");
static LPCTSTR cszDataTips              = _T("ShowDataTips");
static LPCTSTR cszWatchExt              = _T("AutoWindow");
static LPCTSTR cszDecimalRadix          = _T("Radix");
static LPCTSTR cszFiberDebugging		= _T("FiberDebugging");
static LPCTSTR cszCallParams            = _T("CallParameters");
static LPCTSTR cszCallTypes             = _T("CallTypes");
static LPCTSTR cszDAMAnnotate           = _T("DisassemblyAnnotations");
static LPCTSTR cszDAMCodeBytes          = _T("DisassemblyCodeBytes");
static LPCTSTR cszDAMLowerCase          = _T("DisassemblyLowerCase");
static LPCTSTR cszDAMSymbols            = _T("DisassemblySymbols");
static LPCTSTR cszCPURegs               = _T("CPURegisters");
static LPCTSTR cszCPUFlags              = _T("CPUFlags");
static LPCTSTR cszCPUEA                 = _T("CPUEffectiveAddress");
static LPCTSTR cszCPUFloat              = _T("CPUFloatingPoint");
static LPCTSTR cszLocalsToolbar         = _T("LocalsToolbar");
static LPCTSTR cszWatchToolbar          = _T("WatchToolbar");
static LPCTSTR cszMemoryToolbar         = _T("MemoryToolbar");
static LPCTSTR cszVarsToolbar           = _T("VariablesToolbar");
static LPCTSTR cszWatchNameColWidth     = _T("WatchColWidth");
static LPCTSTR cszVarsNameColWidth      = _T("VariablesColWidth");
static LPCTSTR cszQWNameColWidth        = _T("QuickWatchColWidth");
static LPCTSTR cszDebugCommandLine      = _T("DebugCommandLine");
static LPCTSTR cszUnicodeStrings		= _T("UnicodeStrings");
static LPCTSTR cszENCDebugging   		= _T("EditAndContinueDebugging");
static LPCTSTR cszLoadExports			= _T("LoadExports");
static LPCTSTR cszENCRelink		   		= _T("ENCRelink");
static LPCTSTR cszENCTraceLevel   		= _T("ENCTraceLevel");
static LPCTSTR cszENCRepro		   		= _T("ENCRepro");
static LPCTSTR cszENCLoadIdb			= _T("ENCLoadIdbFiles");

//static LPCTSTR csz		= _T("");

static const INIVAL rgivDebug[] =	// Table of debug options (runDebugParams vars)
{
	IT_BYTE,    &runDebugParams.debugMode,              cszRunDebugMode,
	IT_BOOL,    &runDebugParams.extendedRegisters,      cszExtendedRegisters,
	IT_BOOL,    &runDebugParams.fOleRpc,                cszOleRpc,
	IT_BOOL,    &runDebugParams.fReturnValues,          cszReturnValues,
	IT_BOOL,    &runDebugParams.fDataTips,              cszDataTips,
	IT_BOOL,    &runDebugParams.fWatchExt,              cszWatchExt,
	IT_BOOL,    &runDebugParams.decimalRadix,           cszDecimalRadix,
//	IT_BOOL,	&runDebugParams.fFiberDebugging,		cszFiberDebugging,
	IT_BOOL,    &runDebugParams.fCallParams,            cszCallParams,
	IT_BOOL,    &runDebugParams.fCallTypes,             cszCallTypes,
	IT_BOOL,    &runDebugParams.fDAMAnnotate,           cszDAMAnnotate,
	IT_BOOL,    &runDebugParams.fDAMCodeBytes,          cszDAMCodeBytes,
	IT_BOOL,    &runDebugParams.fDAMLowerCase,          cszDAMLowerCase,
	IT_BOOL,    &runDebugParams.fDAMSymbols,            cszDAMSymbols,
	IT_BOOL,    &runDebugParams.fCPURegs,               cszCPURegs,
	IT_BOOL,    &runDebugParams.fCPUFlags,              cszCPUFlags,
	IT_BOOL,    &runDebugParams.fCPUEA,                 cszCPUEA,
	IT_BOOL,    &runDebugParams.fCPUFloat,              cszCPUFloat,
	IT_BOOL,    &runDebugParams.fLocalsToolbar,         cszLocalsToolbar,
	IT_BOOL,    &runDebugParams.fWatchToolbar,          cszWatchToolbar,
	IT_BOOL,    &runDebugParams.fMemoryToolbar,         cszMemoryToolbar,
	IT_UINT,    &runDebugParams.iWatchNameColWidth,     cszWatchNameColWidth,
	IT_UINT,    &runDebugParams.iVarsNameColWidth,      cszVarsNameColWidth,
	IT_UINT,    &runDebugParams.iQWNameColWidth,        cszQWNameColWidth,
	IT_BOOL,    &runDebugParams.fVarsToolbar,           cszVarsToolbar,
	IT_BOOL,	&runDebugParams.fUnicodeStrings,		cszUnicodeStrings,
	IT_BOOL,	&runDebugParams.fENCDebugging,   		cszENCDebugging,
	IT_BOOL,	&runDebugParams.fLoadExports,			cszLoadExports,
	IT_BOOL,	&runDebugParams.fENCRelink,				cszENCRelink,
	IT_UINT,	&runDebugParams.iENCTraceLevel,			cszENCTraceLevel,
	IT_BOOL,	&runDebugParams.fENCRepro,				cszENCRepro,
	IT_BOOL,	&runDebugParams.fENCLoadIdb,			cszENCLoadIdb,
	(INITYPE)0, NULL,                                   0
};

static void ClearKey()
{
	CString str(GetRegistryKeyName());
	str += chKeySep;
	str += cszDebug;
	RegDeleteKey(HKEY_CURRENT_USER, str);
}

/****************************************************************************

	FUNCTION:	GetIniString

	RETURNS:	TRUE if the string was read successfully, FALSE if string empty.

****************************************************************************/

static BOOL GetIniString(LPCTSTR szSectionName, LPCTSTR szKeyName, LPCTSTR szDefault, LPTSTR sz, UINT cb)
{
	static CString str;
	str = GetRegString( szSectionName,szKeyName,szDefault);
	if (!str.IsEmpty())
	{
		_tcsncpy(sz, str, cb);
		return TRUE;
	}
	return FALSE;
}

/****************************************************************************

	FUNCTION:	IniExist

****************************************************************************/

static BOOL IniExist()
{
	HKEY hkeyMain;
	BOOL bRet = (0 == RegOpenKey(HKEY_CURRENT_USER, GetRegistryKeyName(), &hkeyMain));
	RegCloseKey(hkeyMain);
	return bRet;
}

void CDebugPackage::SaveSettings()
{
	WriteRegInt(cszDebug, cszVersion, DEBUG_INI_VERSION);
	for (int iLoop=0; rgivDebug[iLoop].pv; ++iLoop)
		SaveIniValue(cszDebug, &rgivDebug[iLoop]);

	WriteRegString(cszDebug, cszDebugCommandLine, runDebugParams.commandLine);

}


/****************************************************************************

	FUNCTION:	HammerOleRegKey
	Purpose:	The user might enable OLE debugging in one IDE and then step
				into a new IDE where OLE debugging will still be disabled
				because the changes are not comitted to the registry till
				we exit the IDE. So we have special function to force the
				registry update for this key.
	Note:		runDebugParams must be updated prior to calling this function.
******************************************************************************/
void HammerOleRegKey()
{
	
	for ( int i = 0; i < (sizeof(rgivDebug)/sizeof(rgivDebug[0])) ; i++ )
	{
		if ( rgivDebug[i].szKey == cszOleRpc )
		{
			SaveIniValue(cszDebug, &rgivDebug[i]);
		 	return;
		}
	}
	// We should find the OLE key in the debugger ini table.
	ASSERT(0);
}

void HammerLoadExportsRegKey()
{
	for ( int i = 0; i < (sizeof(rgivDebug)/sizeof(rgivDebug[0])) ; i++ )
	{
		if ( rgivDebug[i].szKey == cszLoadExports )
		{
			SaveIniValue(cszDebug, &rgivDebug[i]);
		 	return;
		}
	}
	// We should find the LoadExports key in the debugger ini table.
	ASSERT(0);
}

/****************************************************************************

	FUNCTION:	LoadIniFile

	INPUT:
		cchLastProject = length of buffer pointed to by szLastProject

	OUTPUT:
		szLastProject filled in with last project name.  May be NULL.

****************************************************************************/
void LoadIniFile()
{
	if (!IniExist() || theApp.m_bFirstInstance)
	{
		InitDefaults();
		return;
	}
	if (GetRegInt(cszDebug, cszVersion, 0) != DEBUG_INI_VERSION ||
		!GetRegString( cszGeneral, cszVersion ).IsEmpty() )
	{
		ClearKey();
		InitDefaults();
		return;
	}
	for (int i = 0; rgivDebug[i].pv; ++i)
		LoadIniValue(cszDebug, &rgivDebug[i]);
// Code below turns on fibers iff an unpublished key is set
	CString str(GetRegistryKeyName());
	str += chKeySep;
	str += cszDebug;
	HKEY hKeyNames;
	DWORD rc = RegOpenKeyEx(HKEY_CURRENT_USER,(LPCTSTR)str,0,KEY_ALL_ACCESS,&hKeyNames);
	if(rc == ERROR_SUCCESS){
		DWORD dwTypeBuf;
		BYTE bValBuf[4];
		DWORD cbValBuf = sizeof(bValBuf);
		RegQueryValueEx(hKeyNames,_T("FiberDebugging"),NULL,&dwTypeBuf,bValBuf,&cbValBuf);
		if(*((DWORD *)bValBuf)){
			runDebugParams.fFiberDebugging = TRUE;
		}
		RegCloseKey(hKeyNames);
	}
	GetIniString(cszDebug, cszDebugCommandLine, 0, runDebugParams.commandLine, sizeof(runDebugParams.commandLine));
}
