////////////////////////////////////////////////////////////////////////////
//
// Standard font handling
//

#include "stdafx.h"
#include "main.h"
#include "tchar.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

////////////////////////////////////////////////////////////////////////////

struct FontEntry
{
	TCHAR szName[LF_FACESIZE];
	int iSize;
	BOOL bAdjustSize;	// if true use -MulDiv(iSize, LOGPIXELSY, 72)
};

static BASED_CODE const char szFonts[] = "Fonts";
static BASED_CODE FontEntry FontNames[4][3] =
{
	{
		{ "MS Sans Serif",		 8, TRUE  },
		{ "‚l‚r ‚oƒSƒVƒbƒN",		 9, TRUE  },
		{ "System",  			10, TRUE  },
	}, {
		{ "Small Fonts",		-9, FALSE },
		{ "Small Fonts",		-9, FALSE },
		{ "Terminal",			-9, FALSE },
	}, {
		{ "Courier",			14, FALSE },
		{ "FixedSys",			14, FALSE },
		{ "FixedSys",			14, FALSE }
	}, {
		{ "MS Sans Serif",			14, TRUE },
		{ "MS Sans Serif",			14, TRUE },
		{ "MS Sans Serif",			14, TRUE }
	}
};

static BOOL ParseFontString( LPCTSTR lpsz, FontEntry *lpfe )
{
	ASSERT( lpfe );
	ASSERT( lpsz );
	int nLen = lstrlen( lpsz );

	if ( !nLen )
		return FALSE;

	// Search for a comma
	LPCTSTR lpszTmp = _tcschr( lpsz, _T(',') );
	if (lpszTmp == NULL || lpszTmp == lpsz) // not found, or in first position
		return FALSE;

	// Pick the font name out of the string
	if ((lpszTmp - lpsz) >= LF_FACESIZE)	// too long for our buffer
		return FALSE;

	// We've got a font name
	_tcsncpy( lpfe->szName, lpsz, lpszTmp - lpsz );
	lpfe->szName[LF_FACESIZE-1] = _T('\0');

	// Look for the integer size (could be minus!)
	lpszTmp++;	// skip over the comma
	while (lpszTmp && *lpszTmp && _istspace(*lpszTmp))
		lpszTmp = _tcsinc( lpszTmp );

	if (!lpszTmp || !*lpszTmp)
		return FALSE;

	// convert lpszTmp to a number
	lpfe->iSize = (int) _tcstol( lpszTmp, NULL, 10 );
	if (lpfe->iSize == 0)
		return FALSE;

	// see if we should use points or device units
	lpfe->bAdjustSize = (_tcsstr( lpszTmp, _T("pt") ) != NULL);
	return TRUE;
}

static FontEntry* GetStdFontInfo(int iType, int* pnCharset = NULL)
{
	ASSERT(iType >= 0 && iType < font_count);

	int iFacename, nCharset;

	nCharset = theApp.GetCharSet (&iFacename);
	
	if (pnCharset != NULL)
		*pnCharset = nCharset;

	static FontEntry fe;
	CString strValName;
	switch (iType/3)
	{
		case 0:
			strValName = "Normal";
			break;
		case 1:
			strValName = "Small";
			break;
		case 2:
			strValName = "Fixed";
			break;
		case 3:
			strValName = "Large";
			break;
		default:
			// An invalid font type
			ASSERT( 0 );
	}

	CString strReg = GetRegString( szFonts, strValName );
	// Parse strReg, then fill out and return fe.
	BOOL bValid;
	strReg.MakeLower();
	bValid = ParseFontString( strReg, &fe );

	if (!bValid)
		fe = FontNames[iType/3][iFacename];

	return &fe;
}


void GetStdFontInfo(int iType, CString& strFace, int& ptSize)
{
	FontEntry* pfe = GetStdFontInfo(iType);
	strFace = pfe->szName;
	ptSize = pfe->iSize;
}


// NOTE: The LOGFONT returned by this function is temporary!
const LOGFONT* GetStdLogfont( const int iType, CDC *pDC /* = NULL */ )
{
	static LOGFONT lf;
	
	int nCharset;
	FontEntry* pfe = GetStdFontInfo(iType, &nCharset);

	if (!pDC)
	{
		CWindowDC dc(NULL);
		lf.lfHeight = pfe->bAdjustSize ? 
				-MulDiv(pfe->iSize, dc.GetDeviceCaps(LOGPIXELSY), 72) :
				pfe->iSize;
	}
	else
	{
		lf.lfHeight = pfe->bAdjustSize ? 
				-MulDiv(pfe->iSize, pDC->GetDeviceCaps(LOGPIXELSY), 72) :
				pfe->iSize;
	}
	lf.lfWidth = 0;
	lf.lfEscapement = 0;
	lf.lfOrientation = 0;
	// [chauv] looks like we want to use BOLD where appropriate for MBCS platforms after all
	// for consistency with US and other FarEast platforms.
	// lf.lfWeight = ((iType % 3 == 1) && !theApp.m_fOEM_MBCS) ? FW_BOLD : FW_NORMAL;
	lf.lfWeight = (iType % 3 == 1) ? FW_BOLD : FW_NORMAL;
	lf.lfItalic = (iType % 3 == 2);
	lf.lfUnderline = FALSE;
	lf.lfStrikeOut = FALSE;
	lf.lfCharSet = (BYTE)nCharset;
	lf.lfOutPrecision = OUT_DEFAULT_PRECIS;
	lf.lfClipPrecision = CLIP_DEFAULT_PRECIS;
	lf.lfQuality = DEFAULT_QUALITY;
	lf.lfPitchAndFamily = DEFAULT_PITCH | FW_DONTCARE;
	lstrcpy( lf.lfFaceName, pfe->szName );

	return &lf;
}


CFont* GetStdFont(const int iType)
{
	static CFont fonts [font_count];
	
	ASSERT(iType >= 0 && iType < font_count);
	
	if (fonts[iType].m_hObject == NULL)
		VERIFY(fonts[iType].CreateFontIndirect(GetStdLogfont(iType)));
	
	return &fonts[iType];
}


UINT CTheApp::GetCharSet (int * piFaceName) const
{
int iFacename, nCharset;

	switch (theApp.GetCP())
	{
	case OEM_JAPAN:
		iFacename = 1;
		nCharset = SHIFTJIS_CHARSET;
		break;

	case OEM_PRC:
		iFacename = 2;
		nCharset = GB2312_CHARSET;
		break;

	case OEM_TAIWAN:
		iFacename = 2;
		nCharset = CHINESEBIG5_CHARSET;
		break;

	case OEM_KOR_WANGSUNG:
	case OEM_KOR_JOHAB:
		iFacename = 2;
		nCharset = DEFAULT_CHARSET;
		break;

	default:
		iFacename = 0;
		nCharset = ANSI_CHARSET;
		break;
	}

	if (piFaceName)
		*piFaceName = iFacename;

	return nCharset;
}
