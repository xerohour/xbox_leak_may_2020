// projtype.cpp : implementation file
//

#include "stdafx.h"
#include "mfcappwz.h"
#include "symbols.h"
#include "oleutil.h"
#include "ddxddv.h"
#include "lang.h"
#include "util.h"

#include <malloc.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

extern CWnd* pLaunchParent;
static char BASED_CODE szMFCPathKey[] = "MFCPath32Bit";


/////////////////////////////////////////////////////////////////////////////
// Private helper functions

static void SetUpperAndLowerFile(CString strKey, CString strValue)
{
    // strip any suffix
    int iDot = strValue.ReverseFind('.');
	if (iDot != -1)
		strValue = strValue.Left(iDot);

    ASSERT(!strKey.IsEmpty());
    ASSERT(!strValue.IsEmpty());

    strKey.MakeUpper();
    SetSymbol(strKey, strValue);

    strKey.MakeLower();
    SetSymbol(strKey, strValue);
}

/////////////////////////////////////////////////////////////////////////////
// Project Type / Language options

static const char* szProjTypeString[] =
{
    "PROJTYPE_SDI",
	"PROJTYPE_MDI",
    "PROJTYPE_DLG",
    "PROJTYPE_DLL",
};

void SetProjectTypeSymbol()
{
	for (int i = 0; i < PROJTYPE_COUNT; i++)
	{
		RemoveSymbol(szProjTypeString[i]);
	}
	SetSymbol(szProjTypeString[projOptions.m_nProjType], "1");

	if( projOptions.m_bNoDocView )
		SetSymbol("NODOCVIEW", "1");
	else
		RemoveSymbol("NODOCVIEW");

	
	if(projOptions.m_nProjType == 3)
	{
		int i = GetRegInt( "AppWizard","StartRes", 1000 );
		CString strStart;
		strStart.Format("%d",i);
		SetSymbol( "START_RES", strStart );
	}
	
}

void GetProjectTypeSymbol()
{
	projOptions.m_nProjType = -1;
    for (int i = 0; i < PROJTYPE_COUNT; i++)
	{
        if (DoesSymbolExist(szProjTypeString[i]))
			projOptions.m_nProjType = i;
	}
	projOptions.m_bNoDocView = FALSE;
	if (DoesSymbolExist("NODOCVIEW")){
		projOptions.m_bNoDocView = TRUE;
	}

	ASSERT(projOptions.m_nProjType != -1);
}

static LPCTSTR szPreambleLines[] =
{
	"#if !defined(AFX_RESOURCE_DLL) || defined(AFX_TARG_%s)",
	"#ifdef _WIN32",
	"LANGUAGE %d, %d",
	"#pragma code_page(%d)",
	"#endif //_WIN32",
	NULL
};

// Defined in lang.cpp
WORD CodepageFromTranslation(DWORD dwTranslation);

void MakePreamble(CString& strPreamble, CString& strPreambleInQuotes,
					LPCTSTR szLangSuffix, DWORD dwTranslation)
{
	// Make format strings
	
	CString strFormat, strFormatInQuotes;
	LPCTSTR szLine;
	for (int i=0; ((szLine = szPreambleLines[i]) != NULL); i++)
	{
		strFormat = (strFormat + szLine) + '\n';
		strFormatInQuotes = (((strFormatInQuotes + "\t\"") + szLine) + "\\r\\n\"\n");
	}
	// Chop off ending \n
	strFormat = strFormat.Left(strFormat.GetLength() - 1);
	strFormatInQuotes = strFormatInQuotes.Left(strFormatInQuotes.GetLength() - 1);

	// Make arguments

	// Suffix
	CString strSuffix = szLangSuffix;
	strSuffix.MakeUpper();

	// Langid...
	LANGID langid = (WORD) (dwTranslation >> 16);
	//		primary...
	WORD nPrimaryLang = (USHORT)PRIMARYLANGID(langid);
	//		sub...
	WORD nSubLang = (USHORT)SUBLANGID(langid);

	// Code page
	WORD nCodePage = CodepageFromTranslation(dwTranslation);

	// Do it!!
	strPreamble.Format
		(strFormat, (LPCTSTR) strSuffix, nPrimaryLang, nSubLang, nCodePage);
	strPreambleInQuotes.Format
		(strFormatInQuotes, (LPCTSTR) strSuffix, nPrimaryLang, nSubLang, nCodePage);
}


void SetLanguageSymbols()
{
	CString strLangListSuffixes;
	RemoveSymbol(_T("DO_KANA"));		// only want this if we've got Japanese in our language set
	int nSize = langDlls.GetSize();
	for (int nDll = 0; nDll < nSize && langDlls.IsValidEntry(nDll); nDll++)
	{
		CString strKey;
		strKey.Format("LANGUAGE_%d", nDll);
		if (!langDlls.m_astrNameLang[nDll].IsEmpty())
			SetSymbol(strKey, langDlls.m_astrNameLang[nDll]);
		else
			SetSymbol(strKey, " ");

		// Set loop symbols for languages

		// Let's get the suffix for the languages
		CString strLangSuffix = langDlls.m_astrNameDll[nDll];
		strLangSuffix.MakeLower();
		ASSERT(strLangSuffix.Left(5) == "appwz");
		int nPeriod = strLangSuffix.Find('.');
		ASSERT(nPeriod-5 <= 3);	// i.e., suffix <= 3 characters
		strLangSuffix = strLangSuffix.Mid(5, nPeriod-5);
		strKey.Format("LANG_SUFFIX_%d", nDll);
		SetSymbol(strKey, strLangSuffix);
		
		// Japanese specific symbol needed
		strLangSuffix.MakeLower();
		if (!strLangSuffix.Compare(_T("jpn")))
			SetSymbol(_T("DO_KANA"), _T("1"));

		// HLP directory name
		CString strHlpDir;
		strLangSuffix.MakeLower();
		strKey.Format("HLP_DIR_LOC_%d", nDll);
		strHlpDir.Format("hlp\\%s", (LPCTSTR) strLangSuffix);
		SetSymbol(strKey, strHlpDir);

		// RES directory name
		CString strResDir;
		strKey.Format("RES_DIR_LOC_DBLSLASH_%d", nDll);
		strResDir.Format("res\\\\%s\\\\", (LPCTSTR) strLangSuffix);
		SetSymbol(strKey, strResDir);
		strKey.Format("RES_DIR_LOC_%d", nDll);
		strResDir.Format("res\\%s", (LPCTSTR) strLangSuffix);
		SetSymbol(strKey, strResDir);

		// MFC resource include directory name
		CString strMFCDir;
		strKey.Format("MFC_DIR_LOC_%d", nDll);
		if (strLangSuffix == "enu")
			// Special case: MFC rsc inc for enu is top-level
			strMFCDir = "";
		else
			strMFCDir.Format("l.%s\\\\", (LPCTSTR) strLangSuffix);
		SetSymbol(strKey, strMFCDir);

		// Set preamble string
		CString strPreamble, strPreambleInQuotes;
		DWORD dwTranslation = langDlls.m_adwTranslation[nDll];
		MakePreamble(strPreamble, strPreambleInQuotes, strLangSuffix, dwTranslation);
		strKey.Format("LANG_PREAMBLE_%d", nDll);
		SetSymbol(strKey, strPreamble);
		strKey.Format("LANG_PREAMBLE_INQUOTES_%d", nDll);
		SetSymbol(strKey, strPreambleInQuotes);

		// Postamble string
		strKey.Format("LANG_POSTAMBLE_%d", nDll);
		SetSymbol(strKey, "#endif");
		strKey.Format("LANG_POSTAMBLE_INQUOTES_%d", nDll);
		SetSymbol(strKey, "\t\"#endif\\r\\n\"");

		// Tack suffix onto end of strLangListSuffixes
		strLangListSuffixes += ", " + strLangSuffix;
	}
	CString strNumLangs;
	strNumLangs.Format("%d", nDll);
	SetSymbol("NUM_LANGS", strNumLangs);

	
	if (nDll == 1)
	{
		// Special case: Only one language.

		SetSymbol("HLP_DIR_LOC_0", "hlp");
		SetSymbol("RES_DIR_LOC_DBLSLASH_0", "res\\\\");
		SetSymbol("RES_DIR_LOC_0", "res");
		
		/*SetSymbol("LANG_PREAMBLE_0", "");
		SetSymbol("LANG_PREAMBLE_INQUOTES_0", "");
		SetSymbol("LANG_POSTAMBLE_0", "");
		SetSymbol("LANG_POSTAMBLE_INQUOTES_0", "");*/

		RemoveSymbol("MULTIPLE_LANGS");

		SetSymbol("HLPARG_MAC", "1");
		SetSymbol("HLPARG_MACPATH", "2");
	}
	else if (nDll > 1)
	{
		SetSymbol("MULTIPLE_LANGS", "1");
		SetSymbol("HLPARG_MAC", "2");
		SetSymbol("HLPARG_MACPATH", "3");

		// Take out leading ", " in strLangSuffix
		ASSERT(strLangListSuffixes.GetLength() > 2);
		strLangListSuffixes = strLangListSuffixes.Mid(2);
		SetSymbol("LANG_LIST_SUFFIXES", strLangListSuffixes);
	}


	SetSymbol("LOC_RC", "loc.rc");
	SetSymbol("MACLOC_RC", "macloc.rc");
	SetSymbol("DLGLOC_RC", "dlgloc.rc");

	/* CString strLangid;
	sprintf(strLangid.GetBuffer(256), "%#x", projOptions.m_langid);
	strLangid.ReleaseBuffer();
	SetSymbol("LANGID", strLangid);*/
}

void GetLanguageSymbols()
{
	// TODO: Fix me
	GetSymbol("LANGUAGE", langDlls.m_astrNameLang[0]);
/*	CString strLangid;
	int langid;
	GetSymbol("LANGID", strLangid);
	sscanf(strLangid, "0x%x", &langid);
	projOptions.m_langid = (WORD) langid;*/
}

/////////////////////////////////////////////////////////////////////////////
// Database options

struct RFXData
{
	LPCTSTR m_lpszCType;	// C Type corresponding to ODBC type
	LPCTSTR m_lpszInit;		// Initial value for the recordset member variable
	LPCTSTR m_lpszRFX;		// The RFX_ function to use w/ this recordset mem var
};

// These arrays are indexed by ODBC data type.  Format:
//  { C type, initializer value, RFX_ (or DFX_) suffix }
#define RFX_TABLE_SIZE 20

// This is for ODBC recordsets
static RFXData rdRFX[] =
{
	{ "BOOL", "FALSE", "Bool" },
	{ "BYTE", "0", "Byte" },
	{ "CString", "_T(\"\")", "Text" },
	{ "CLongBinary", "", "LongBinary" },
	{ "CByteArray", "", "Binary" },
	{ "CByteArray", "", "Binary" },
	{ "CString", "_T(\"\")", "Text" },
	{ NULL, NULL, NULL },
	{ "CString", "_T(\"\")", "Text" },
	{ "CString", "_T(\"\")", "Text" },
	{ "CString", "_T(\"\")", "Text" },
	{ "long", "0", "Long" },
	{ "int", "0", "Int" },
	{ "double", "0.0", "Double" },
	{ "float", "0.0f", "Single" },
	{ "double", "0.0", "Double" },
	{ "CTime", "0", "Date" },
	{ "CTime", "0", "Date" },
	{ "CTime", "0", "Date" },
	{ "CString","_T(\"\")", "Text" },
};

// This is for DAO recordsets
static RFXData rdDFX[] =
{
	{ "BOOL", "FALSE", "Bool" },
	{ "BYTE", "0", "Byte" },
	{ "COleCurrency", "", "Currency" },
	{ "CLongBinary", "", "LongBinary" },
	{ "CByteArray", "", "Binary" },
	{ "CByteArray", "", "Binary" },
	{ "CString", "_T(\"\")", "Text" },
	{ NULL, NULL, NULL },
	{ "CString", "_T(\"\")", "Text" },
	{ "CString", "_T(\"\")", "Text" },
	{ "CString", "_T(\"\")", "Text" },
	{ "long", "0", "Long" },
	{ "short", "0", "Short" },
	{ "double", "0.0", "Double" },
	{ "float", "0.0f", "Single" },
	{ "double", "0.0", "Double" },
	{ "COleDateTime", "(DATE)0", "DateTime" },
	{ "COleDateTime", "(DATE)0", "DateTime" },
	{ "COleDateTime", "(DATE)0", "DateTime" },
	{ "CString","_T(\"\")", "Text" },
};

static BOOL ShouldDelimitName(LPCTSTR szName)
{
	return FALSE;
	/*// Name should be delmimited if there exist non-EnglishAlphanumerics
	while (*szName != '\0')
	{
		if (!('a' <= *szName && *szName <= 'z')
			&& !('A' <= *szName && *szName <= 'Z')
			&& !('0' <= *szName && *szName <= '9'))
		{
			return TRUE;
		}
		szName++;
	}
	return FALSE;*/
}

BOOL GenerateDBBindings(CString& strDecls, CString& strInits, CString& strRFXs)
{
    ASSERT(strDecls == "");
    ASSERT(strInits == "");
    ASSERT(strRFXs == "");

	// Initialize the strings to 100 characters each to avoid
	// at least some of the unnecessary growing of buffers
	strDecls.GetBuffer(100);
	strDecls.ReleaseBuffer();
	strInits.GetBuffer(100);
	strInits.ReleaseBuffer();
	strRFXs.GetBuffer(100);
	strRFXs.ReleaseBuffer();

    // Use the nameMap to guarantee uniqueness of member variable names
	CMapStringToPtr nameMap;

	CString strCols;
	GetSymbol("DB_COLSINFO", strCols);
	CString strVars;
	GetSymbol("DB_VARSINFO", strVars);

    LPCSTR lpszLine = _tcschr(strCols, '=');
	int nCannedNameIndex = 1;
    while(lpszLine != NULL)
    {
        lpszLine++;       // Move past the '='
        // Get the variable's name
        CString strName;
        CString strNumType;
        AfxExtractSubString(strName, lpszLine, 0, ',');
        AfxExtractSubString(strNumType, lpszLine, 1, ',');

        // Extract its type info from the appropriate array
		RFXData* rdFX = rdRFX;
		if( projOptions.m_nDataSource == dbDao )
			rdFX = rdDFX;
        int nType;
        sscanf(strNumType, "%d", &nType);
		nType += 7;							// Get it into table's range
		if (nType < 0 || nType >= RFX_TABLE_SIZE || nType == 7)
		{
			// Somehow we managed to get an invalid ODBC type
			ASSERT(FALSE);

			// But let's be gracious and go on to the next one anyway
	        lpszLine = _tcschr(lpszLine, '\n');
	        if (lpszLine == NULL) break;
			lpszLine++;
	        lpszLine = _tcschr(lpszLine, '=');
			continue;
		}
		
        CString strCType = rdFX[nType].m_lpszCType;
        CString strInit = rdFX[nType].m_lpszInit;
        CString strRFX = rdFX[nType].m_lpszRFX;

		int nPos = strVars.Find('\n');
		CString strCName = strVars.Left(nPos);
		strVars = strVars.Mid(nPos + 1);

        // Build the three output strings
		BOOL bDelim = ShouldDelimitName(strName);
        strDecls += '\t' + strCType + '\t' + strCName + ";\n";  // Declaration
        if (strInit != "")
            strInits += '\t' + strCName + " = " + strInit + ";\n";  // Initialization

		if( projOptions.m_nDataSource == dbDao )
			strRFXs += "\tDFX_";
		else 
			strRFXs += "\tRFX_";

		strRFXs += strRFX + "(pFX, _T(\"";

		if (bDelim)
			strRFXs += "[" + strName + "]\"), " + strCName + ");\n"; // RFX call
		else
			strRFXs += strName + "\"), " + strCName + ");\n";        // RFX call

        lpszLine = _tcschr(lpszLine, '\n');
        if (lpszLine == NULL) break;
		lpszLine++;
        lpszLine = _tcschr(lpszLine, '=');
    }

    // Get rid of final '\n's and add m_nFields initialization.
	if (strDecls.GetLength() != 0)
		strDecls = strDecls.Left(strDecls.GetLength() - 1);
	if (strRFXs.GetLength() != 0)
		strRFXs = strRFXs.Left(strRFXs.GetLength() - 1);

	CString strNumCols;
	GetSymbol("DB_NUMCOLS", strNumCols);
    strInits += "\tm_nFields = " + strNumCols + ';';
    return TRUE;
}

void SetDBSymbols()					// Can throw an exception!!
{
    // Any database support at all?
    if (projOptions.m_bDBHeader &&
    	(projOptions.m_nProjType == PROJTYPE_MDI
		|| projOptions.m_nProjType == PROJTYPE_SDI))
	{
        SetSymbol("DB", "1");
	}
    else
	{
        RemoveSymbol("DB");
		RemoveSymbol("DB_COLSINFO");
		RemoveSymbol("DB_VARSINFO");
		RemoveSymbol("DB_NUMCOLS");
		RemoveSymbol("DB_SOURCE");
		RemoveSymbol("DB_TABLE_TYPE");
		RemoveSymbol("DB_TABLECLASS");
		RemoveSymbol("DB_CONNECT");
		RemoveSymbol("DB_DSN");
		RemoveSymbol("PARAM_VARS");
		RemoveSymbol("PARAM_VAR_BINDINGS");
		RemoveSymbol("PARAM_RFX");
	}

	// File support?
    if (projOptions.m_bDBSimple)
		SetSymbol("DB_NO_FILE", "1");
    else
 		RemoveSymbol("DB_NO_FILE");

	RemoveSymbol("OLEDB");
	RemoveSymbol("DAO");
	if (projOptions.m_nDataSource == dbDao ) {
		RemoveSymbol("CRecordView");
		RemoveSymbol("COleDBRecordView");
		SetSymbol("DAO", "1");
	} else if (projOptions.m_nDataSource == dbOledb) {
		SetSymbol("OLEDB", "1");
		RemoveSymbol("CRecordView");
		RemoveSymbol("CDaoRecordView");
	} else {
		RemoveSymbol("OleDBRecordView");
		RemoveSymbol("CDaoRecordView");
	}
		

	if (projOptions.m_bAutoDetect)
		RemoveSymbol("DB_NO_DETECT");
	else
		SetSymbol("DB_NO_DETECT", "1");

	// Set connection & recset column symbols

	SetSymbol("RECSET_VARS", "");
    SetSymbol("RECSET_VAR_BINDINGS", "");
    SetSymbol("RECSET_RFX", "");

	if (!DoesSymbolExist("DB"))
		return;

	const TCHAR *pCompare = "";
	switch(projOptions.m_nDataSource)
	{
		case dbOdbc:
			pCompare = "CRecordView";
			break;
		case dbDao:	
			pCompare = "CDaoRecordView";
			break;
		case dbOledb:	
			pCompare = "COleDBRecordView";
			break;
		default:
			break;
	}

	
    if (projOptions.m_names.strBaseClass[classView] == pCompare)
    {
        if (!GenerateDBBindings(CreateSymbol("RECSET_VARS"),
            CreateSymbol("RECSET_VAR_BINDINGS"),
            CreateSymbol("RECSET_RFX")))
        {
            AfxThrowUserException();
        }

        // now synthesize the name for the embedded
        //  CRecordset class that is in the document
        CString strVarName = "m_";
        CString strTmp, strRecsetClass;
        GetSymbol("RECSET_CLASS", strTmp);
		strRecsetClass = strTmp;
		// FUTURE: We're copying this because we'll modify strRecsetClass
		//  but we don't want to modify the RECSET_CLASS symbol.  For next
		//  version, change GetSymbol to return a copy of the string


        LPCSTR szTemp = strRecsetClass;

        BOOL bStartsWithC = (*szTemp == 'C');
        while (*szTemp != '\0' && isupper(*szTemp++))
            ;

        BOOL bAllUpper = (*szTemp == '\0');

        szTemp = strRecsetClass.GetBuffer(strRecsetClass.GetLength());

        if (bStartsWithC)
        {
            if (isupper(*++szTemp))
            {
                if (bAllUpper)
                {
                    strRecsetClass.MakeLower();
                    szTemp = strRecsetClass;
                    strVarName += ++szTemp;
                }
                else
                {
                    // MFC style name
                    *(LPSTR) szTemp = (TCHAR)tolower(*szTemp);
                    strVarName += szTemp;
                }
            }
            else
            {
                // not MFC style name
                szTemp--;
                *(LPSTR) szTemp = (TCHAR)tolower(*szTemp);
                strVarName += szTemp;
            }
        }
        else
        {
            // can't be an MFC style name
            if (bAllUpper)
            {
                strRecsetClass.MakeLower();
                strVarName += strRecsetClass;
            }
            else
            {
                *(LPSTR)szTemp = (TCHAR)tolower(*szTemp);
                strVarName += szTemp;
            }
        }
        SetSymbol("RECSET_VARIABLE", strVarName);
        TRACE("Setting recordset variable = %s\n", (LPCSTR) strVarName);
    }
}

void GetDBSymbols()
{
	// No minimal support for any database?
	if (!DoesSymbolExist("DB"))
	{
		projOptions.m_bDBHeader = FALSE;
		if (projOptions.m_names.strBaseClass[classView] == "CRecordView")
			projOptions.m_names.strBaseClass[classView] = "CView";
		projOptions.m_bDBSimple = FALSE;
		return;
	}
	projOptions.m_bDBHeader = TRUE;

	// No record view?
	if (!DoesSymbolExist("CRecordView"))
	{
		if (projOptions.m_names.strBaseClass[classView] == "CRecordView")
			projOptions.m_names.strBaseClass[classView] = "CView";
		projOptions.m_bDBSimple = FALSE;
	}
	else
		projOptions.m_names.strBaseClass[classView] = "CRecordView";

	// Are we simple?
	if (DoesSymbolExist("DB_NO_FILE"))
		projOptions.m_bDBSimple = TRUE;
	else
		projOptions.m_bDBSimple = FALSE;
}


/////////////////////////////////////////////////////////////////////////////
// OLE options

// Function to generate GUIDs for OLE.  First string is entered into
//  the main .cpp file, second string in registration database.
void generateGUID(CString& str, CString& strReg, CString& strMacro)
{
/*    if (!CheckOleVersion())
    {
#ifndef BOGUSGUID
        AfxMessageBox(IDS_NEED_OLE_201);
		if (pLaunchParent)
			pLaunchParent->EnableWindow(FALSE);
        AfxThrowUserException();
#else
		static BOOL bFirstTime = TRUE;
		if (bFirstTime)
		{
			AfxMessageBox("OLE 2.0 does not yet support unique GUID creation.  Because of this,\
 AppWizard has not been able to provide your application with a\
 globally unique GUID which is necessary for complete OLE\
 interoperability.  Please read the comment in your project's main\
 source file for more information.");
 			if (pLaunchParent)
				pLaunchParent->EnableWindow(FALSE);
			bFirstTime = FALSE;
		}
#endif //BOGUSGUID
	}*/
	
    GUID guid;
    if(!GetValidGuid(guid))
    {
        AfxMessageBox(IDS_ERR_COCREATEGUID_FAILED);
		if (pLaunchParent)
			pLaunchParent->EnableWindow(FALSE);
        AfxThrowUserException();
    }

    str.Empty();
    wsprintf(str.GetBuffer(200),
        "{ %#lx, %#x, %#x, { %#x, %#x, %#x, %#x, %#x, %#x, %#x, %#x } }",
        guid.Data1, guid.Data2, guid.Data3, (int) guid.Data4[0],
        (int) guid.Data4[1], (int) guid.Data4[2], (int) guid.Data4[3],
        (int) guid.Data4[4], (int) guid.Data4[5], (int) guid.Data4[6], (int) guid.Data4[7]);
    str.ReleaseBuffer();
    strReg.Empty();
    wsprintf(strReg.GetBuffer(200),
        "%08lX-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X",
        guid.Data1, guid.Data2, guid.Data3, (int) guid.Data4[0],
        (int) guid.Data4[1], (int) guid.Data4[2], (int) guid.Data4[3],
        (int) guid.Data4[4], (int) guid.Data4[5], (int) guid.Data4[6], (int) guid.Data4[7]);
    strReg.ReleaseBuffer();
    strMacro.Empty();
    wsprintf(strMacro.GetBuffer(200),
        "%#lx, %#x, %#x, %#x, %#x, %#x, %#x, %#x, %#x, %#x, %#x",
        guid.Data1, guid.Data2, guid.Data3, (int) guid.Data4[0],
        (int) guid.Data4[1], (int) guid.Data4[2], (int) guid.Data4[3],
        (int) guid.Data4[4], (int) guid.Data4[5], (int) guid.Data4[6], (int) guid.Data4[7]);
    strMacro.ReleaseBuffer();
}


// Takes text of guid in registration db form, and converts it to a C++
//  text description of its struct
void regToStructGuid(const char* szRegGuid, CString& strStructGuid)
{
	USES_CONVERSION;
	CString strRegGuid = szRegGuid;
    GUID guid;
/*	if (!CheckOleVersion())
    {
#ifndef BOGUSGUID
        AfxMessageBox(IDS_NEED_OLE_201);
        AfxThrowUserException();
	}*/
    //LoadOleEntryPoints(FALSE);
	// CLSIDFromString expects braces around the guid string
	strRegGuid = "{" + strRegGuid + "}";
    HRESULT hres = CLSIDFromString(A2W(strRegGuid.GetBuffer(strRegGuid.GetLength())), &guid);
	strRegGuid.ReleaseBuffer();
/*#else
    	sscanf(strRegGuid,
	        "%08lX-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X",
	        &(guid.Data1), &(guid.Data2), &(guid.Data3), &(guid.Data4[0]),
	        &(guid.Data4[1]), &(guid.Data4[2]), &(guid.Data4[3]),
	        &(guid.Data4[4]), &(guid.Data4[5]), &(guid.Data4[6]), &(guid.Data4[7]));
	    strStructGuid.Empty();
	}
#endif //BOGUSGUID
*/
	wsprintf(strStructGuid.GetBuffer(200),
	    "{ %#lx, %#x, %#x, { %#x, %#x, %#x, %#x, %#x, %#x, %#x, %#x } }",
	    guid.Data1, guid.Data2, guid.Data3, guid.Data4[0],
	    guid.Data4[1], guid.Data4[2], guid.Data4[3], guid.Data4[4],
	    guid.Data4[5], guid.Data4[6], guid.Data4[7]);
	strStructGuid.ReleaseBuffer();
}

// Ties above functions together.
void recordGuid(const CString& strDocTag)
{
	// We only want to record the guids if we're being called by CConfirmDlg
	if (theDLL.m_pMainDlg->m_pConfirmDlg == NULL)
		return;

	// If we haven't yet chosen the doc GUID yet, do it now.
	LPCTSTR szProgIDSuffix = (projOptions.m_nProjType == PROJTYPE_DLG) ?
		".Application" : ".Document";
	HKEY hkDoc;
	if (RegOpenKey(HKEY_CLASSES_ROOT,
		strDocTag + szProgIDSuffix, &hkDoc) == ERROR_SUCCESS)
	{
		// Application is already in the registration database.
		// Give user a chance to use the old guid or generate a new one
		char szBuf[100];
		LONG lSize = sizeof(szBuf);
		if (RegQueryValue(hkDoc, "CLSID", (LPSTR) szBuf, &lSize) == ERROR_SUCCESS)
		{
			// Put up dialog so user can choose which guid to use
			int nResponse = AfxMessageBox(IDP_GUID_REGISTERED, MB_YESNOCANCEL, 0);
			if (pLaunchParent)
				pLaunchParent->EnableWindow(FALSE);
			switch(nResponse)
			{
			case IDYES:
			{
				// Enter registration db guid into our symbol table.
				CString strRegGuid = szBuf;
				if (strRegGuid.Left(1) == "{" && strRegGuid.Right(1) == "}")
				{
					// Chop off leading & trailing braces
					strRegGuid = strRegGuid.Mid(1, strRegGuid.GetLength()-2);
            		SetSymbol("APP_CLSID_REG", strRegGuid);
            		regToStructGuid(strRegGuid, CreateSymbol("APP_CLSID"));
				}
				else
					// The string in the registration DB is not of the assumed format
					//  so just make a new guid.
					generateGUID(CreateSymbol("APP_CLSID"), CreateSymbol("APP_CLSID_REG"), CreateSymbol("APP_CLSID_MACRO"));
				break;
			}
			case IDNO:
				// User wants to generate a new guid
				generateGUID(CreateSymbol("APP_CLSID"), CreateSymbol("APP_CLSID_REG"), CreateSymbol("APP_CLSID_MACRO"));
				break;
			case IDCANCEL:
				// Abort
				RegCloseKey(hkDoc);
				AfxThrowUserException();
				break;
			}
		}
		else
		{
			// The application is registered, but no guid there.  Create new one.
			generateGUID(CreateSymbol("APP_CLSID"), CreateSymbol("APP_CLSID_REG"), CreateSymbol("APP_CLSID_MACRO"));
		}
		RegCloseKey(hkDoc);
	}
	else
	{
		// The application is not registered, so make new guid
		generateGUID(CreateSymbol("APP_CLSID"), CreateSymbol("APP_CLSID_REG"), CreateSymbol("APP_CLSID_MACRO"));
	}


	CString tmp, tmp2;

	// For app with OLE Automation, the .odl file needs two more guids.
	if (DoesSymbolExist("AUTOMATION"))
	{
		generateGUID(tmp, CreateSymbol("LIB_CLSID_ODL"), tmp2);
		generateGUID(CreateSymbol("DISPIID_CLSID"), CreateSymbol("DISPIID_CLSID_ODL"), tmp2);
	}
}

char* oleOptions[5] =
{ NULL, "CONTAINER", "MINI_SERVER", "FULL_SERVER", "CONTAINER_SERVER" };

void ResetOleDocSymbols()
{
    for (int i = 1; i < OLE_MAX; i++)
    {
        if (oleOptions[i] != NULL)
            RemoveSymbol(oleOptions[i]);
    }
	RemoveSymbol("COMPFILE");
	RemoveSymbol("ACTIVE_DOC_SERVER");
	RemoveSymbol("ACTIVE_DOC_CONTAINER");
}

void ResetOleSymbols()
{
	ResetOleDocSymbols();
	RemoveSymbol("AUTOMATION");
}

void SetOleSymbols()
{
    // Set flag for OLE, erase other OLE flags
    ResetOleSymbols();
    ASSERT(projOptions.m_nOleType < sizeof(oleOptions)/sizeof(oleOptions[0]));
    if (oleOptions[projOptions.m_nOleType])
        SetSymbol(oleOptions[projOptions.m_nOleType], "1");
    if (projOptions.m_bOleAutomation)
        SetSymbol("AUTOMATION", "1");
    if (projOptions.m_bOleCompFile)
        SetSymbol("COMPFILE", "1");
	if (projOptions.m_bOcx)
		SetSymbol("OLECTL", "1");
	else
		RemoveSymbol("OLECTL");	// NOTE: This is not removed in resetOleSymbols, since
			// dlg-based apps and possibly DLLs need this set.

	if (projOptions.m_bActiveDocServer)
		SetSymbol("ACTIVE_DOC_SERVER", "1");
	if (projOptions.m_bActiveDocContainer)
		SetSymbol("ACTIVE_DOC_CONTAINER", "1");

	// Set GUID symbols
    if (DoesSymbolExist("FULL_SERVER")
        || DoesSymbolExist("MINI_SERVER")
        || DoesSymbolExist("CONTAINER_SERVER")
        || DoesSymbolExist("AUTOMATION"))
    {
        // Either creates a new guid or selects existing one from registry,
        //  then records it in symbol table
		// TODO: Change this to the nonlocalized safe doc tag
        recordGuid(projOptions.m_strProj);
    }
}

void GetOleSymbols()
{
	projOptions.m_nOleType = OLE_NO;
	for (int i=1; i < OLE_MAX; i++)
	{
		if (DoesSymbolExist(oleOptions[i]))
			projOptions.m_nOleType = i;
	}

	projOptions.m_bActiveDocServer = DoesSymbolExist("ACTIVE_DOC_SERVER");
	projOptions.m_bActiveDocContainer = DoesSymbolExist("ACTIVE_DOC_CONTAINER");

	if (DoesSymbolExist("AUTOMATION"))
		projOptions.m_bOleAutomation = TRUE;
	else
		projOptions.m_bOleAutomation = FALSE;

	if (DoesSymbolExist("OLECTL"))
		projOptions.m_bOcx = TRUE;
	else
		projOptions.m_bOcx = FALSE;
}

/////////////////////////////////////////////////////////////////////////////
// Application options

// order must match IDCD_ values
char* rgszOptions[] =
{
	"MAPI",
    "TOOLBAR",
	"STATUSBAR",
    "PRINT",
    "HELP",
	"SOCKETS",
	"3D",
    "ABOUT",
};

void SetAppOptionsSymbols()
{
    for (int i = 0; i < PO_TOTAL_COUNT; i++)
    {
        char* szOption = rgszOptions[i];
        if (projOptions.m_options & (1<<i))
        {
            SetSymbol(szOption, "1");
        }
        else
        {
            // remove key just in case set from previous attempt
            RemoveSymbol(szOption);
        }
    }
}

void GetAppOptionsSymbols()
{
    for (int i = 0; i < PO_TOTAL_COUNT; i++)
    {
        char* szOption = rgszOptions[i];
		if (DoesSymbolExist(szOption))
			projOptions.m_options |= (1<<i);
		else
			projOptions.m_options &= ~(1<<i);
	}
}

// MRU

void SetSizeMRUSymbols(UINT nSizeMRU)
{
	if (nSizeMRU != 0)
		SetSymbol("HAS_MRU", "1");
	else
		RemoveSymbol("HAS_MRU");

	if (nSizeMRU == 4)
	{
		// This is the default
		SetSymbol("SIZE_MRU", "");
		return;
	}

	ASSERT(nSizeMRU <= 16);

	CString strMRU;
	sprintf(strMRU.GetBuffer(16), "%d", nSizeMRU);
	strMRU.ReleaseBuffer();
	SetSymbol("SIZE_MRU", strMRU);
}

void GetSizeMRUSymbols()
{
	if (!DoesSymbolExist("HAS_MRU"))
	{
		projOptions.m_nSizeMRU = 0;
		return;
	}

	CString strMRU;
	GetSymbol("SIZE_MRU", strMRU);
	sscanf(strMRU, "%ud", &projOptions.m_nSizeMRU);
}

// ADVANCED DIALOG TABS

// Document Symbols

void SetDocumentSymbols()
{
	// Set nonlocalizable strings
	if (!projOptions.m_names.strDocFileExt.IsEmpty())
	{
		SetSymbol("SUFFIX", '.' + projOptions.m_names.strDocFileExt);
		if (!projOptions.m_bDBSimple)
			SetSymbol("HAS_SUFFIX", "1");
		else
			RemoveSymbol("HAS_SUFFIX");
	}
	else
		RemoveSymbol("HAS_SUFFIX");
	SetSymbol("DOC_REGID", projOptions.m_names.strDocRegID);

	// document coclass name (probably 'Document')
	CString strCoClass;
	int idxPeriod = projOptions.m_names.strDocRegID.Find(_T('.'));
	if (idxPeriod < 0)
		strCoClass = projOptions.m_names.strDocRegID;
	else
		strCoClass = projOptions.m_names.strDocRegID.Mid(idxPeriod+1);
	idxPeriod = strCoClass.Find(_T('.'));
	if (idxPeriod >= 0)
		strCoClass = strCoClass.Left(idxPeriod);
	SetSymbol("DOC_COCLASS", strCoClass);

	// Set localizable strings
	int nSize = langDlls.GetSize();
	for (int i = 0; i < nSize && !langDlls.m_astrDocTag[i].IsEmpty(); i++)
	{
		CString strNum;
		strNum.Format("_%d", i);
	    SetSymbol("Doc" + strNum, langDlls.m_astrDocTag[i]);
	    SetSymbol("DOC" + strNum, Upper(langDlls.m_astrDocTag[i]));
		SetSymbol("DOC_FILENEW" + strNum, langDlls.m_astrDocFileNew[i]);
		SetSymbol("DOC_FILTER" + strNum, langDlls.m_astrDocFilter[i]);
		SetSymbol("DOC_REGNAME" + strNum, langDlls.m_astrDocRegName[i]);
		SetSymbol("TITLE" + strNum, langDlls.m_astrTitle[i]);
	}

	// FUTURE: When the templates are in order take out stuff below
	if (i > 0)
	{
		// FUTURE: With multiple langs, replace these lines w/ what's commented
		//  out below them.  We need to separate out uses of $$DOC$$ that should
		//  be localized (e.g., in the big doc string in the string table) from
		//  those uses that should NOT be localized (e.g., in the name of the
		//  resource ID of that string mentioned above, and in the .odl ID, etc.)
		SetSymbol("Doc", langDlls.m_astrDocTag[0]);
		SetSymbol("DOC", Upper(langDlls.m_astrDocTag[0]));
		//SetSymbol("Doc", projOptions.m_strProjTag.Left(6));
		//SetSymbol("DOC", Upper(projOptions.m_strProjTag.Left(6)));

		SetSymbol("DOC_FILENEW", langDlls.m_astrDocFileNew[0]);
		SetSymbol("DOC_FILTER", langDlls.m_astrDocFilter[0]);
		SetSymbol("DOC_REGNAME", langDlls.m_astrDocRegName[0]);
		SetSymbol("TITLE", langDlls.m_astrTitle[0]);
	}
	else if (nSize > 0)
		SetSymbol("TITLE", langDlls.m_astrTitle[0]);

}

void GetDocumentSymbols()
{
	if (DoesSymbolExist("HAS_SUFFIX"))
	{
		// There is a doc suffix, so get it from the symbols.
		GetSymbol("SUFFIX", projOptions.m_names.strDocFileExt);
		if (projOptions.m_names.strDocFileExt.Left(1) == ".")
		{
			// Take out period
			projOptions.m_names.strDocFileExt =
				projOptions.m_names.strDocFileExt.Right
					(projOptions.m_names.strDocFileExt.GetLength()-1);
		}
		projOptions.m_bDBSimple = FALSE;
	}
	else
	{
		// No doc suffix, so clear out corresponding option
		projOptions.m_names.strDocFileExt.Empty();
	}
	GetSymbol("DOC_REGID", projOptions.m_names.strDocRegID);

	// TODO: Fix this to work with langDlls.
/*
    GetSymbol("Doc", projOptions.m_names.strDocTag);
	GetSymbol("DOC_FILENEW", projOptions.m_names.strDocFileNew);
	GetSymbol("DOC_FILTER", projOptions.m_names.strDocFilter);
	GetSymbol("DOC_REGNAME", projOptions.m_names.strDocRegName);*/
}

// Main Frame symbols

char* rgszFrameStyles[] =
{
	" | WS_THICKFRAME",
	" | WS_SYSMENU",
	" | WS_MINIMIZEBOX",
	" | WS_MINIMIZE",
	" | WS_MAXIMIZEBOX",
	" | WS_MAXIMIZE",
};

void SetFrameSymbols()
{
	if ((projOptions.m_nProjType != PROJTYPE_MDI
		&& projOptions.m_nProjType != PROJTYPE_SDI)
		|| projOptions.m_nFrameStyles == GetDefaultFrameStyles())
	{
		RemoveSymbol("FRAME_STYLES");
		if (projOptions.m_nProjType == PROJTYPE_SDI)
			SetSymbol("SW_ARG", "SW_SHOW");
		else
			SetSymbol("SW_ARG", "m_nCmdShow");

		return;
	}

	SetSymbol("FRAME_STYLES", "1");
	CString strStyles = "";
	for (int i=0; i < FRAME_STYLES_END - FRAME_STYLES_START + 1; i++)
	{
		if (projOptions.m_nFrameStyles & (1 << i))
			strStyles += rgszFrameStyles[i];
	}

	// Minimized & Maximized are special cases, since we need to also alter
	//  the ShowWindow call.
	if (projOptions.m_nFrameStyles & (1 << (IDC_MINIMIZE-FRAME_STYLES_START)))
		SetSymbol("SW_ARG", "SW_SHOWMINIMIZED");
	else if (projOptions.m_nFrameStyles & (1 << (IDC_MAXIMIZE-FRAME_STYLES_START)))
		SetSymbol("SW_ARG", "SW_SHOWMAXIMIZED");
	else if (projOptions.m_nProjType == PROJTYPE_SDI)
		SetSymbol("SW_ARG", "SW_SHOW");
	else
		SetSymbol("SW_ARG", "m_nCmdShow");

	// Get rid of leading space.
	if (strStyles.Left(1) == " ")
		strStyles = strStyles.Mid(1);
	SetSymbol("FRAME_STYLE_FLAGS", strStyles);
}

void GetFrameSymbols()
{
	if (!DoesSymbolExist("FRAME_STYLES"))
	{
		projOptions.m_nFrameStyles = GetDefaultFrameStyles();
		return;
	}
	CString strStyles;
	GetSymbol("FRAME_STYLE_FLAGS", strStyles);
	for (int i=0; i < FRAME_STYLES_END - FRAME_STYLES_START + 1; i++)
	{
		// See if the flag (minus the leading space which is removed from
		//  the first flag) is a substring of strStyles
		CString strSearchString = ((LPCTSTR) rgszFrameStyles[i])+1;
		if (strStyles.Find(strSearchString + ' ') != -1
			|| strStyles.Right(strSearchString.GetLength()) == strSearchString)
		{
			projOptions.m_nFrameStyles |= (1<<i);
		}
		else
			projOptions.m_nFrameStyles &= ~(1<<i);
	}
}

void SetSplitterSymbols()
{
	BOOL fHandled = FALSE;

	if (projOptions.m_bSplitter)
	{
		SetSymbol("SPLITTER", "1");
		if (projOptions.m_nProjType == PROJTYPE_MDI)
		{
			SetSymbol("SPLITTER_MDI", "1");
			RemoveSymbol("SPLITTER_SDI");
			fHandled = TRUE;
		}
		else if (projOptions.m_nProjType == PROJTYPE_SDI)
		{
			SetSymbol("SPLITTER_SDI", "1");
			RemoveSymbol("SPLITTER_MDI");
			fHandled = TRUE;
		}
	}
	if (!fHandled) // other projects, such as dlg and dll.
	{
		RemoveSymbol("SPLITTER");
		RemoveSymbol("SPLITTER_MDI");
		RemoveSymbol("SPLITTER_SDI");
	}
}

void GetSplitterSymbols()
{
	if (DoesSymbolExist("SPLITTER"))
		projOptions.m_bSplitter = TRUE;
	else
		projOptions.m_bSplitter = FALSE;
}

void SetMenuSymbols()
{
	BOOL fHandled = TRUE;

	RemoveSymbol("REBAR");
	RemoveSymbol("NORMALMENU");
	switch (projOptions.m_nMenuStyle)
	{
		case NORMAL_MENUS:
			SetSymbol("NORMALMENU", "1");
			break;
		case REBAR_MENUS:
			SetSymbol("REBAR", "1");
			break;
		default:
			ASSERT(FALSE);
			break;
	}
}

void GetMenuSymbols()
{
	projOptions.m_nMenuStyle = NORMAL_MENUS;
	if (DoesSymbolExist("REBAR"))
		projOptions.m_nMenuStyle = REBAR_MENUS;
}

// Child Frame symbols	

char* rgszChildFrameStyles[] =
{
	" | WS_THICKFRAME",
	" | WS_MINIMIZEBOX",
	"",   //" | WS_MINIMIZE"
	" | WS_MAXIMIZEBOX",
	"", 	//" | WS_MAXIMIZE",
};

void SetChildFrameSymbols()
{
	// Set symbol for emitting childfrm files.  (We now *always* emit child frame for MD apps.)
	if (IsMDI())
		SetSymbol("MDICHILD", "1");
	else
		RemoveSymbol("MDICHILD");

	int nDefChildFrameStyles = GetDefaultChildFrameStyles();
	if (!IsMDI()
		|| (projOptions.m_nChildFrameStyles == nDefChildFrameStyles
			&& !projOptions.m_bSplitter))
	{
		// Reset child frame symbols
		RemoveSymbol("CHILD_FRAME_STYLE_FLAGS");
		RemoveSymbol("CHILD_FRAME_STYLES");
		RemoveSymbol("CHILD_FRAME_MAXIMIZED");
		RemoveSymbol("CHILD_FRAME_MINIMIZED");
		SetSplitterSymbols();
		return;
	}

	// Set symbol for emitting PreCreateWindow if necessary
	if (projOptions.m_nChildFrameStyles != nDefChildFrameStyles)
		SetSymbol("CHILD_FRAME_STYLES", "1");
	else
		RemoveSymbol("CHILD_FRAME_STYLES");

	// Set style flags symbol
	CString strStyles = "";
	for (int i=0; i < CHILD_FRAME_STYLES_END - CHILD_FRAME_STYLES_START + 1; i++)
	{
		if (projOptions.m_nChildFrameStyles & (1 << i))
			strStyles += rgszChildFrameStyles[i];
	}
	SetSymbol("CHILD_FRAME_STYLE_FLAGS", strStyles);
	if (projOptions.m_nChildFrameStyles & (1 << 4 ))
		SetSymbol("CHILD_FRAME_MAXIMIZED", "1");
	if (projOptions.m_nChildFrameStyles & (1 << 2 ))
		SetSymbol("CHILD_FRAME_MINIMIZED", "1");

	SetSplitterSymbols();
}

void GetChildFrameSymbols()
{
	int nDefChildFrameStyles = GetDefaultChildFrameStyles();
	if (!DoesSymbolExist("MDICHILD"))
	{
		projOptions.m_nChildFrameStyles = nDefChildFrameStyles;
		GetSplitterSymbols();
		return;
	}

	// Get child frame styles
	if (DoesSymbolExist("CHILD_FRAME_STYLES"))
	{
		CString strStyles;
		GetSymbol("CHILD_FRAME_STYLE_FLAGS", strStyles);
		for (int i=0; i < CHILD_FRAME_STYLES_END - CHILD_FRAME_STYLES_START + 1; i++)
		{
			// See if the flag is a substring of strStyles
			CString strSearchString = ((LPCTSTR) rgszChildFrameStyles[i])+1;
			if (strStyles.Find(strSearchString + ' ') != -1
				|| strStyles.Right(strSearchString.GetLength()) == strSearchString)
			{
				projOptions.m_nChildFrameStyles |= (1<<i);
			} else {
				projOptions.m_nChildFrameStyles &= ~(1<<i);
			}
		}
		if( DoesSymbolExist("CHILD_FRAME_MAXIMIZED") ) {
			projOptions.m_nChildFrameStyles &= (1<<4);
		} else {
			projOptions.m_nChildFrameStyles &= ~(1<<4);
		}
		if( DoesSymbolExist("CHILD_FRAME_MINIMIZED") ) {
			projOptions.m_nChildFrameStyles &= (1<<2);
		} else {
			projOptions.m_nChildFrameStyles &= ~(1<<2);
		}
	}
	else
		projOptions.m_nChildFrameStyles = nDefChildFrameStyles;
	GetSplitterSymbols();
}

/////////////////////////////////////////////////////////////////////////////
// Project options

char* rgszProjOptions[] =
{
	"VERBOSE",
	"MFCDLL",
};

void SetProjOptionsSymbols()
{
    for (int i = 0; i < PO_PROJ_COUNT; i++)
    {
        char* szOption = rgszProjOptions[i];
        if (projOptions.m_projOptions & (1<<i))
        {
            SetSymbol(szOption, "1");
        }
        else
        {
            // remove key just in case set from previous attempt
            RemoveSymbol(szOption);
        }
    }
    if (projOptions.m_bATLSupport)
    {
        SetSymbol("ATL_SUPPORT", "1");
    }
    else
    {
        // remove key just in case set from previous attempt
        RemoveSymbol("ATL_SUPPORT");
    }
    if (projOptions.m_nProjStyle == PS_EXPLORER  )
	{
        if (projOptions.m_names.strBaseClass[classView] == "CView" )
		{
            projOptions.m_names.strBaseClass[classView] = "CListView";
		}
        SetSymbol("PROJTYPE_EXPLORER", "1");
	}
	else
	{
		if( DoesSymbolExist("PROJTYPE_EXPLORER") )
		{
			if (projOptions.m_names.strBaseClass[classView] == "CListView" )
			{
				projOptions.m_names.strBaseClass[classView] = "CView";
			}
			RemoveSymbol("PROJTYPE_EXPLORER");
		}
	}
}

void GetProjOptionsSymbols()
{
    for (int i = 0; i < PO_PROJ_COUNT; i++)
    {
        char* szOption = rgszProjOptions[i];
		if (DoesSymbolExist(szOption))
			projOptions.m_projOptions |= (1<<i);
		else
			projOptions.m_projOptions &= ~(1<<i);
	}
	if (DoesSymbolExist("ATL_SUPPORT")) 
		projOptions.m_bATLSupport = TRUE;
	else
		projOptions.m_bATLSupport = FALSE;
    if (DoesSymbolExist("PROJTYPE_EXPLORER")  )
	{
		projOptions.m_nProjStyle = PS_EXPLORER;
	}
	else
	{
		projOptions.m_nProjStyle = PS_MFCNORMAL;
	}
}

LPCTSTR szExtdll = "EXTDLL";

void SetDllSymbol()
{
	if (projOptions.m_bExtdll)
		SetSymbol(szExtdll, "1");
	else
		RemoveSymbol(szExtdll);
}

void GetDllSymbol()
{
	projOptions.m_bExtdll = DoesSymbolExist(szExtdll);
}

/////////////////////////////////////////////////////////////////////////////
// Classes dialog names

static char* classPrefix[classMax] =
{ "VIEW", "APP", "FRAME", "CHILD_FRAME", "DOC", "DLG",
  "RECSET", "SRVRITEM", "CNTRITEM",	"IPFRAME",  "DLGAUTOPROXY", "TREEVIEW", "WNDVIEW" };

void SetClassesSymbols()
{
    for (int iClass = 0; iClass < classMax; iClass++)
    {
        CString prefix = classPrefix[iClass];
        SetSymbol(prefix + "_CLASS", projOptions.m_names.strClass[iClass]);
        SetSymbol(prefix + "_BASE_CLASS", projOptions.m_names.strBaseClass[iClass]);
        SetUpperAndLowerFile(prefix + "_IFILE", projOptions.m_names.strIFile[iClass]);
        SetUpperAndLowerFile(prefix + "_HFILE", projOptions.m_names.strHFile[iClass]);
    }

	SetBaseViewSymbols();
}

void GetClassesSymbols()
{
    for (int iClass = 0; iClass < classMax; iClass++)
    {
        CString prefix = classPrefix[iClass];
        GetSymbol(prefix + "_CLASS", projOptions.m_names.strClass[iClass]);
        GetSymbol(prefix + "_BASE_CLASS", projOptions.m_names.strBaseClass[iClass]);
        GetSymbol(prefix + "_IFILE", projOptions.m_names.strIFile[iClass]);
        GetSymbol(prefix + "_HFILE", projOptions.m_names.strHFile[iClass]);
    }
}

char* szDefaultViews[DEFAULT_VIEWS_COUNT] =
{
	"CView",
	"CScrollView",
	"CFormView",
	"CListView",
	"CTreeView",
	"CRichEditView",
};

static char* szOtherViews[OTHER_VIEWS_COUNT] =
{
	"CEditView",
	"CRecordView",
	"CDaoRecordView",
	"COleDBRecordView",
	"CHtmlView",
};

void SetBaseViewSymbols()
{
    // Set flag for type of view, erase other view-type flags
    for (int i=0; i < DEFAULT_VIEWS_COUNT; i++)
    	RemoveSymbol(szDefaultViews[i]);
	for (i=0; i < OTHER_VIEWS_COUNT; i++)
    	RemoveSymbol(szOtherViews[i]);
    SetSymbol(projOptions.m_names.strBaseClass[classView], "1");

	// Bring up the dialog if its a FormView Derived Class
	if(
		projOptions.m_names.strBaseClass[classView] == szDefaultViews[2] ||
		projOptions.m_names.strBaseClass[classView] == szOtherViews[1] ||
		projOptions.m_names.strBaseClass[classView] == szOtherViews[2] ||
		projOptions.m_names.strBaseClass[classView] == szOtherViews[3]
	  )
	{
		CString strDialog;
		CString strTemp;
		BOOL fDialog;
	
		GetSymbol(_T("SAFE_ROOT"), strTemp);
		strDialog.Format("IDD_%s_FORM", strTemp); // name of dialog resource to open
		SetSymbol( _T("APPWIZ_INITIAL_DIALOG"),strDialog);
	
	}
}

/////////////////////////////////////////////////////////////////////////////
// Set miscellaneous symbols

void SetMFCPathSymbol()
{
	// get MFC path from Registry
    CString strMFCPath;

#ifndef VS_PACKAGE
    HKEY hVCKey;
	CString strVCKey = GetRegistryKeyName();
	strVCKey += "\\General";
    if (RegOpenKeyEx(HKEY_CURRENT_USER, strVCKey, 0, KEY_QUERY_VALUE, &hVCKey) == ERROR_SUCCESS)
    {
        DWORD dwLength = _MAX_PATH;
        DWORD dwType;
        RegQueryValueEx(hVCKey, szMFCPathKey, NULL, &dwType,
            (LPBYTE) strMFCPath.GetBuffer(_MAX_PATH), &dwLength);
        strMFCPath.ReleaseBuffer();

        RegCloseKey(hVCKey);
    }
#endif	// VS_PACKAGE

    int nLen = strMFCPath.GetLength();
    if (nLen != 0)
	{
		TCHAR ch = *_tcsdec((const TCHAR*) strMFCPath, ((const TCHAR*) strMFCPath) + nLen);
		if (ch != '\\' && ch != '/')
			strMFCPath += "\\";
	}
    SetSymbol("MFCPath", strMFCPath);
}

// Set year for copyright
void SetYearSymbol()
{
	time_t osBinaryTime;  // C run-time time (defined in <time.h>)
	time( &osBinaryTime ) ;  // Get the current time from the
	CTime osTime( osBinaryTime );  // CTime from C run-time time
	
	int year = osTime.GetYear(); // extract the year and turn it into a string
	CString str;
	str.Format("%d",year);
						 
    SetSymbol("YEAR", str);  // at last check, this will last us until the year 2038
}

void RemoveInapplicableSymbols()
{
    // Those project options that don't jibe with current app-type
    //  must have their symbols un-set
    if (projOptions.m_nProjType == PROJTYPE_DLG)
    {
        // Dialog-based app doesn't use these symbols
        RemoveSymbol("TOOLBAR");
        RemoveSymbol("PRINT");
        RemoveSymbol("CRecordView");
		RemoveSymbol("SPLITTER_MDI");
		RemoveSymbol("SPLITTER_SDI");
		RemoveSymbol("SPLITTER");
		RemoveSymbol("MAPI");
		ResetOleDocSymbols();
    }
    else
    {
        // Doc/view app doesn't use this symbol
        RemoveSymbol("ABOUT");
    }
}



/////////////////////////////////////////////////////////////////////////////
// Dictionary lookup functions

void GetSymbol(const TCHAR* tszKey, CString& rStrValue)
{
	GetAWX()->m_Dictionary.Lookup(tszKey, rStrValue);
}

void SetSymbol(const TCHAR* tszKey, const TCHAR* tszValue)
{
	GetAWX()->m_Dictionary[tszKey] = tszValue;
}

CString& CreateSymbol(const TCHAR* tszKey)
{
	return GetAWX()->m_Dictionary[tszKey];
}
	
BOOL DoesSymbolExist(const TCHAR* tszKey)
{
	CString tmp;
	return (GetAWX()->m_Dictionary.Lookup(tszKey, tmp));
}

void RemoveSymbol(const TCHAR* tszKey)
{
	GetAWX()->m_Dictionary.RemoveKey(tszKey);
}

