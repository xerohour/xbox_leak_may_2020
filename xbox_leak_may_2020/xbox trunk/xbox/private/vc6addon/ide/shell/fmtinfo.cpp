//						FMTINFO.CPP
//
//
/////

#include "stdafx.h"
#include "resource.h"
#include "fcdialog.h"
#include <stddef.h>

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

IMPLEMENT_DYNAMIC (CFormatInfo, CObject);

//////////////////////////////////////////////////////////////////////////////
//																			//
//							class CFormatInfo								//
//																			//
//////////////////////////////////////////////////////////////////////////////

_TCHAR	CFormatInfo::m_szRegKey[]	= _TEXT("Format");
_TCHAR	CFormatInfo::m_szFontFace[] = _TEXT("FontFace");
_TCHAR	CFormatInfo::m_szFontSize[] = _TEXT("FontSize");


int		CFormatInfo::m_nPixPerInchY = 0;

///// Constructor
//
///
CFormatInfo::CFormatInfo (
CPackage *	pPackage
) :
m_pPackage	(pPackage),
m_pWinGroup (NULL),
m_nWinGroups (0)
{
	if (m_nPixPerInchY == 0)
	{
		CDC * pDC = theApp.m_pMainWnd->GetDC();
		m_nPixPerInchY = pDC->GetDeviceCaps (LOGPIXELSY);
		theApp.m_pMainWnd->ReleaseDC (pDC);
	}
}


///// Destructors
//
//	Deallocate everything
//
///
CFormatInfo::~CFormatInfo ()
{
	Clear ();
}


///// Clear - Blow off all data
//
///
void CFormatInfo::Clear ()
{
	if (m_pWinGroup == NULL) {
		ASSERT (m_nWinGroups == 0);
		return;
		}

	for (	int iWinGroup = 0; iWinGroup < m_nWinGroups; iWinGroup++ )
	{

		for ( int iWindow = 0; iWindow < m_pWinGroup[iWinGroup].nWindows; iWindow++ )
		{
			FMT_WINDOW * pWindow = &m_pWinGroup[iWinGroup].rgWindows[iWindow];

			for (	FMT_ELEMENT * pElement = &pWindow->rgElements[0];
					pElement <  &pWindow->rgElements[pWindow->nElements];
					pElement++ )
			{
				delete [] pElement->szElement;
			}

			delete [] pWindow->rgElements;

			delete [] pWindow->szWindow;

			delete [] pWindow->szRegEntry;

			if (pWindow->pLogFont)
				delete pWindow->pLogFont;
			if (pWindow->szFontFace)
				delete [] pWindow->szFontFace;
	
		}

		if (m_pWinGroup[iWinGroup].rgWindows)
			delete [] m_pWinGroup[iWinGroup].rgWindows;
		if (m_pWinGroup[iWinGroup].szName)
			delete [] m_pWinGroup[iWinGroup].szName;
	}

	delete [] m_pWinGroup;

	m_pWinGroup = NULL;
	m_nWinGroups = 0;
}


///// GetFormatInfo - Query package for its Format Information
//
///
BOOL CFormatInfo::GetFormatInfo (
CPackage * pPackage
) {

	if (pPackage == NULL) {
		// Either the constructor or this function must establish the m_pPackage field
		ASSERT (m_pPackage != NULL);
		pPackage = m_pPackage;
		}
	else
	if (m_pPackage == NULL)
		m_pPackage = pPackage;

	m_pFormatInfoFromPackage = pPackage->GetFormatInfo();

	if (m_pFormatInfoFromPackage != NULL) {

		(*this) = *m_pFormatInfoFromPackage;

		UpdateAllAutoColors ();

		return TRUE;
		}
	else
		return FALSE;
}


///// SaveToRegistry - Update registry
//
///
void CFormatInfo::SaveToRegistry () const
{
	if (m_pWinGroup == NULL) {
		ASSERT (m_nWinGroups == 0);
		return;
		}

	CString strMasterKey = GetRegistryKeyName();
	strMasterKey += chKeySep;
	strMasterKey += m_szRegKey;
	strMasterKey += chKeySep;

	for ( int iWinGroup = 0; iWinGroup < m_nWinGroups; iWinGroup++)
	{
		FMT_WINGROUP * pWinGroup = &m_pWinGroup[iWinGroup];

		for ( int iWindow = 0; iWindow < pWinGroup->nWindows; iWindow++)
		{

			FMT_WINDOW * pWindow = &pWinGroup->rgWindows[iWindow];

			HKEY	hkey;
			DWORD	dwResult;

			if (ERROR_SUCCESS == RegCreateKeyEx (
									HKEY_CURRENT_USER,		// Master Key
									strMasterKey + pWindow->szRegEntry,
									0,						// Reserved
									NULL,					// We have no class
									0,						// Default volatility
									KEY_ALL_ACCESS,			// What the heck
									NULL,					// Default security
									&hkey,
									&dwResult ))			// Created/Opened
			{

				if (pWindow->szFontFace)
				{
					if (ERROR_SUCCESS != RegSetValueEx (hkey, m_szFontFace, 0, REG_SZ, (CONST BYTE *)pWindow->szFontFace, _tcslen(pWindow->szFontFace)+1))
					{
						ASSERT (FALSE);
						return;
					}

					if (ERROR_SUCCESS != RegSetValueEx (hkey, m_szFontSize, 0, REG_DWORD, (CONST BYTE *)&pWindow->nFontSize, sizeof(pWindow->nFontSize)))
					{
						ASSERT (FALSE);
						return;
					}

				}

				for ( int iElement = 0; iElement < pWindow->nElements; iElement++ )
				{

					FMT_ELEMENT * pElement = &pWindow->rgElements[iElement];

					if (ERROR_SUCCESS != RegSetValueEx (
											hkey,
											pElement->szElement,
											0,
											REG_BINARY,
											(BYTE *)pElement + offsetof (FMT_ELEMENT, rgbText),
		 									sizeof (FMT_ELEMENT) - offsetof(FMT_ELEMENT, rgbText)
											))
					{
						ASSERT (FALSE);
						return;
					}
				}
			}			
		}
	}
}


///// UpdateFromRegistry () - Update current FMT_WINGROUP data from registry
//
//	This supports reading the registry more than once. 
///
void CFormatInfo::UpdateFromRegistry ()
{
	if (m_pWinGroup == NULL) {
		ASSERT (m_nWinGroups == 0);
		return;
		}

	CString strMasterKey = GetRegistryKeyName();
	strMasterKey += chKeySep;
	strMasterKey += m_szRegKey;
	strMasterKey += chKeySep;

	for ( int iWinGroup = 0; iWinGroup < m_nWinGroups; iWinGroup++)
	{
		FMT_WINGROUP * pWinGroup = &m_pWinGroup[iWinGroup];

		for ( int iWindow = 0; iWindow < pWinGroup->nWindows; iWindow++)
		{

			FMT_WINDOW * pWindow = &pWinGroup->rgWindows[iWindow];

			HKEY	hkey;

			if (ERROR_SUCCESS == RegOpenKeyEx (
										HKEY_CURRENT_USER,		// Master Key
										strMasterKey + pWindow->szRegEntry,	
										0,						// Reserved
										KEY_ALL_ACCESS,			// What the heck
										&hkey ))
			{
				DWORD dwType;
				_TCHAR rgchBuffer[60];	// Arbitrary   
				DWORD cbBuffer = sizeof(rgchBuffer);

			
				if (ERROR_SUCCESS == RegQueryValueEx (hkey, m_szFontFace, NULL, &dwType, (PUCHAR)rgchBuffer, &cbBuffer))
				{
					ASSERT (dwType == REG_SZ);

					if (pWindow->szFontFace)
						if (_tcsicmp (pWindow->szFontFace, rgchBuffer))
						{
							delete [] pWindow->szFontFace;
							pWindow->szFontFace = NewString (rgchBuffer);
						}
						else
							;
				}

				cbBuffer = sizeof(pWindow->nFontSize);

				if (ERROR_SUCCESS == RegQueryValueEx (hkey, m_szFontSize, NULL, &dwType, (PUCHAR)&pWindow->nFontSize, &cbBuffer))
				{
					ASSERT (dwType == REG_DWORD);
				}

				UpdateLogFont (pWindow);

				for ( int iElement = 0; iElement < pWindow->nElements; iElement++ )
				{

					FMT_ELEMENT * pElement = &pWindow->rgElements[iElement];

					cbBuffer = sizeof(FMT_ELEMENT) - offsetof(FMT_ELEMENT, rgbText);

					if (ERROR_SUCCESS == RegQueryValueEx (hkey, pElement->szElement, NULL, &dwType, (PUCHAR)&pElement->rgbText, &cbBuffer))
					{
						ASSERT (dwType == REG_BINARY);
						ASSERT (cbBuffer == sizeof (FMT_ELEMENT) - offsetof(FMT_ELEMENT, rgbText));
					}
				}

				RegCloseKey(hkey);
			}
		}
	}

	UpdateAllAutoColors ();
}


///// Update - Update from one instance to another without changing structure
//
//	Assume that the CFormatInfo argument is structured the same as this.
//	iWinGroupUpd == -1 means do all groups
//	iWinUpd == -1 means do all Windows
///
void CFormatInfo::Update (const CFormatInfo& fmtInfo, int iWinGroupUpd, int iWindowUpd)
{
	for ( int iWinGroup = 0; iWinGroup < m_nWinGroups; iWinGroup++)
	{
		if (iWinGroupUpd != -1 && iWinGroup != iWinGroupUpd)
			continue;

		FMT_WINGROUP * pWinGroup = &m_pWinGroup[iWinGroup];

		for ( int iWindow = 0; iWindow < pWinGroup->nWindows; iWindow++ )
		{
			if (iWindowUpd != -1 && iWindow != iWindowUpd)
				continue;

			FMT_WINDOW * pWinDst = &pWinGroup->rgWindows[iWindow];
			FMT_WINDOW * pWinSrc = fmtInfo.GetWindow (pWinDst->szWindow);

			if (pWinSrc != NULL)
			{
				pWinDst->bChanged = TRUE;

				if (	pWinSrc->szFontFace == NULL
					||	pWinDst->szFontFace == NULL
					|| _tcsicmp (pWinDst->szFontFace, pWinSrc->szFontFace))
				{
					if (pWinDst->szFontFace)
						delete [] pWinDst->szFontFace;
					
					if (pWinSrc->szFontFace)
						pWinDst->szFontFace = NewString (pWinSrc->szFontFace);
					else
						pWinDst->szFontFace = NULL;
				}

				pWinDst->nFontSize = pWinSrc->nFontSize;

				if (pWinSrc->pLogFont)
				{
					if (pWinDst->pLogFont == NULL)
						pWinDst->pLogFont = new LOGFONT;

					*(pWinDst->pLogFont) = *(pWinSrc->pLogFont);
				}
				else
					UpdateLogFont (pWinDst);

				ASSERT (pWinDst->nElements == pWinSrc->nElements);

				for ( int iElement = 0; iElement < pWinDst->nElements; iElement++)
				{
					FMT_ELEMENT * pElSrc = &pWinSrc->rgElements[iElement];
					FMT_ELEMENT * pElDst = &pWinDst->rgElements[iElement];

					ASSERT (!_tcsicmp (pElDst->szElement, pElSrc->szElement));

					pElDst->rgbText			= pElSrc->rgbText;
					pElDst->rgbBackground	= pElSrc->rgbBackground;
					pElDst->autoFore		= pElSrc->autoFore;
					pElDst->autoBack		= pElSrc->autoBack;
				}

			}
		}
	}
}


///// UpdateLogFonts
//
//	Regenerate all LOGFONT structures.  Used when responding to WM_FONTCHANGE.
///
void CFormatInfo::UpdateAllLogFonts ()
{
	for ( int iWinGroup = 0; iWinGroup < m_nWinGroups; iWinGroup++)
	{
		FMT_WINGROUP * pWinGroup = &m_pWinGroup[iWinGroup];

		for ( int iWindow = 0; iWindow < pWinGroup->nWindows; iWindow++ )
		{
			FMT_WINDOW * pWindow = &pWinGroup->rgWindows[iWindow];

			UpdateLogFont (pWindow);
		}
	}

}


///// UpdateAllAutoColors
//
//	Update Auto colors for all windows
///
void CFormatInfo::UpdateAllAutoColors ()
{
	for (int iGroup = 0; iGroup < m_nWinGroups; iGroup++)
	{
		for (int iWindow = 0; iWindow < m_pWinGroup[iGroup].nWindows; iWindow++)
		{
			UpdateAutoColors(&m_pWinGroup[iGroup].rgWindows[iWindow]);
		}
	}
}


///// operator=
//
//	This does the following:
//
//		1. Clears all existing Window/element information
//		2. Creates a copy of the the passed in fmtinfo.
//		3. Updates the color/font particulars from the registry.	
///
const CFormatInfo& CFormatInfo::operator= (
const FMT_WINGROUP& WinGroup 
) {

	Clear ();

	return *this += WinGroup;
}

const CFormatInfo& CFormatInfo::operator= (
const CFormatInfo& fmtInfo
) {
	Clear ();

	for ( int iWinGroup = 0; iWinGroup < fmtInfo.m_nWinGroups; iWinGroup++ )
		*this += fmtInfo.m_pWinGroup[iWinGroup];

	return *this;
}

///// operator+= - Add the given information
//
//	Same as operator=, but without clearing the existing information first.
///
const CFormatInfo& CFormatInfo::operator+= (const FMT_WINGROUP& WinGroupSrc)
{
	if (WinGroupSrc.nWindows == 0) {
		return *this;
		}

	// Extend the FMT_WINGROUP array by one.
	//
	FMT_WINGROUP * pWinGroupNew = new FMT_WINGROUP[m_nWinGroups + 1];
	if (m_pWinGroup != NULL)
	{
		memcpy (pWinGroupNew, m_pWinGroup, m_nWinGroups * sizeof FMT_WINGROUP);
		delete [] m_pWinGroup;
	}		
	m_pWinGroup = pWinGroupNew;

	// pWinGroupNew points to new, empty WINGROUP.
	//
	pWinGroupNew = &m_pWinGroup[m_nWinGroups++];

	pWinGroupNew->nWindows	= WinGroupSrc.nWindows;
	pWinGroupNew->bSingleGroup = WinGroupSrc.bSingleGroup;
	pWinGroupNew->szName	= NewString (WinGroupSrc.szName);
	pWinGroupNew->rgWindows	= new FMT_WINDOW[WinGroupSrc.nWindows];
	memcpy (pWinGroupNew->rgWindows, WinGroupSrc.rgWindows, WinGroupSrc.nWindows * sizeof FMT_WINDOW);

	// We now have a bitcopy of all the Window information.  We need to
	// make copies of all the "pointed-to" stuff.
	for (int iWindow = 0; iWindow < WinGroupSrc.nWindows; iWindow++)
	{
		FMT_WINDOW * pWinDst = &pWinGroupNew->rgWindows[iWindow];
		FMT_WINDOW * pWinSrc = &WinGroupSrc.rgWindows[iWindow];

		pWinDst->szWindow	= NewString (pWinSrc->szWindow);
		pWinDst->szRegEntry	= NewString (pWinSrc->szRegEntry);
		if (pWinSrc->szFontFace)
			pWinDst->szFontFace	= NewString (pWinSrc->szFontFace);
		else
			pWinDst->szFontFace	= NULL;

		// We want a copy of the LOGFONT, unless there is none to copy.  In that case,
		// we want to generate a LOGFONT from the other information.
		//
		if (pWinSrc->pLogFont != NULL)
		{
			pWinDst->pLogFont = new LOGFONT;
			*pWinDst->pLogFont = *pWinSrc->pLogFont;
		}
		else
		{
			UpdateLogFont (pWinDst);
		}

		// Finally, copy all the Element stuff
		//
		if (pWinSrc->nElements)
		{
			pWinDst->rgElements = new FMT_ELEMENT [pWinSrc->nElements];
			memcpy (pWinDst->rgElements, pWinSrc->rgElements, pWinSrc->nElements * sizeof (FMT_ELEMENT));
													 
			for ( int iElement = 0; iElement < pWinSrc->nElements; iElement++ )
			{
				pWinDst->rgElements[iElement].szElement = NewString (pWinSrc->rgElements[iElement].szElement);
			}
		}
	}

	return *this;
}


///// GetWindow - Gets FMT_WINDOW from name
//
///
FMT_WINDOW * CFormatInfo::GetWindow ( const _TCHAR * szWindow ) const
{
	for ( int iWinGroup = 0; iWinGroup < m_nWinGroups; iWinGroup++ )
	{
		FMT_WINGROUP * pWinGroup = &m_pWinGroup[iWinGroup];

		for ( int iWindow = 0; iWindow < pWinGroup->nWindows; iWindow++ )
		{
			FMT_WINDOW * pWindow = &pWinGroup->rgWindows[iWindow];

			if (!_tcsicmp ( szWindow, pWindow->szWindow ))
				return pWindow;
		}
	}

	return NULL;
}


///// operator GetElementList - Get Window from string
//
///
FMT_ELEMENT * CFormatInfo::GetElementList ( const TCHAR * szWindow ) const
{
	FMT_WINDOW * pWindow = GetWindow (szWindow);

	if (pWindow != NULL)
		return pWindow->rgElements;
	else
		return NULL;
}

///// SetForeColor - Set the foreground color of the current element
//
///
void CFormatInfo::SetForeColor ( UINT iGroup, UINT iWindow, UINT iElement, COLORREF rgb )
{
FMT_WINDOW * pWindow = &m_pWinGroup[iGroup].rgWindows[iWindow];
FMT_ELEMENT * pElement = &pWindow->rgElements[iElement];

	pWindow->bChanged = TRUE;

	if (rgb == CColorCombo::rgbAuto) {
		pElement->autoFore.bOn = TRUE;
		// ->rgbText will be set in UpdateAutoColors()
		}
	else {
		pElement->autoFore.bOn = FALSE;
		pElement->rgbText = rgb; 
		}
	UpdateAutoColors (pWindow);
}



///// SetBackColor - Set the background color of the current element
//
///
void CFormatInfo::SetBackColor ( UINT iGroup, UINT iWindow, UINT iElement, COLORREF rgb )
{
FMT_WINDOW * pWindow = &m_pWinGroup[iGroup].rgWindows[iWindow];
FMT_ELEMENT * pElement = &pWindow->rgElements[iElement];

	pWindow->bChanged = TRUE;

	if (rgb == CColorCombo::rgbAuto) {
		pElement->autoBack.bOn = TRUE;
		// ->rgbBackground will be set in UpdateAutoColors()
		}
	else {
		pElement->autoBack.bOn = FALSE;
		pElement->rgbBackground = rgb;
		}

	UpdateAutoColors (pWindow);
}

///// UpdateColor - update a color
//
// Return:
//  TRUE if the color was updated, FALSE if it's source is not 
//  up to date
//
BOOL UpdateColor(AUTO_COLOR & acDst, COLORREF & rgbDst, 
				 const AUTO_COLOR & acSrc, COLORREF rgbSrc, 
				 const AUTO_COLOR & acSrcRev, COLORREF rgbSrcRev)
{
	BOOL bRet = FALSE;
	if (acDst.bRev)
	{
		if (acSrcRev.bUpd)
		{
			rgbDst = rgbSrcRev;
			bRet = acDst.bUpd = TRUE;
		}
	}
	else
	{
		if (acSrc.bUpd)
		{
			rgbDst = rgbSrc;
			bRet = acDst.bUpd = TRUE;
		}
	}
	return bRet;
}

// define this to 1 to get trace messages from this function
#if 0
#define UACTRACE TRACE
#else
#define UACTRACE
#endif

///// UpdateAutoColors - Walk through current Window and update rgb fields in Auto elements
//
//	An Element's color comes from one of the following:
//
//	1.	User					- The user selects the color manually
//	2.	Auto System				- The color comes from a system color
//	3.	Auto Reference			- The color comes from another element...
//		3a.	Ref to type 1		- Source element is type 1, 2 or 3 above.
//		3b. Ref to type 2		
//		3c. Ref to type 3
//
// To properly update automatic colors, we make 1 to N passes, where
// N is the number of elements in the window.
//
// Pass 1.
//  Set .bUpd bit for all type 1, clear .bUpd for all others.
//	Update color from system and set .bUpd for all type 2s.
//
// Pass 2.
//	Update color and set .bUpd for all type 3s whose source element has .bUpd set.
//  Repeat until a full pass is made without updating any colors.
//  In the absence of a cycle, each pass updates at least one color.
//  
///
void CFormatInfo::UpdateAutoColors (FMT_WINDOW * pWindow)
{

	if (NULL == pWindow->rgElements || 0 >= pWindow->nElements)
		return;

	FMT_ELEMENT * pElement;

	for (pElement = &pWindow->rgElements[0]; pElement < &pWindow->rgElements[pWindow->nElements]; pElement++)
	{
		// Foreground

		pElement->autoFore.bUpd = !pElement->autoFore.bOn;
		
		// Update system colors.
		if (pElement->autoFore.bOn && pElement->autoFore.bSys)
		{
			pElement->rgbText = GetSysColor(pElement->autoFore.index);
			pElement->autoFore.bUpd = TRUE;
		}

		// Background

		pElement->autoBack.bUpd = !pElement->autoBack.bOn;

		// Update system colors.
		if (pElement->autoBack.bOn && pElement->autoBack.bSys)
		{
			pElement->rgbBackground = GetSysColor(pElement->autoBack.index);
			pElement->autoBack.bUpd = TRUE;
		}
	}

	int cPasses;	
	BOOL bDoItAgain = TRUE;
	for (cPasses = 0; bDoItAgain && (cPasses < pWindow->nElements); cPasses++)
	{
		bDoItAgain = FALSE;

		for (	pElement = &pWindow->rgElements[0];
				pElement < &pWindow->rgElements[pWindow->nElements];
				pElement++)
		{
			FMT_ELEMENT * pElSource;

			// Foreground
			//
			if (!pElement->autoFore.bUpd)
			{
				if (pElement->autoFore.bSrc)
					pElSource = &m_pWinGroup[0].rgWindows[0].rgElements[pElement->autoFore.index];
				else		
					pElSource = &pWindow->rgElements[pElement->autoFore.index];

				bDoItAgain = !UpdateColor(
					pElement->autoFore, pElement->rgbText, 
					pElSource->autoFore, pElSource->rgbText,
					pElSource->autoBack, pElSource->rgbBackground);
			}

			// Background
			//
			if (!pElement->autoBack.bUpd)
			{
				if (pElement->autoBack.bSrc)
					pElSource = &m_pWinGroup[0].rgWindows[0].rgElements[pElement->autoBack.index];
				else		
					pElSource = &pWindow->rgElements[pElement->autoBack.index];

				bDoItAgain = !UpdateColor(
					pElement->autoBack, pElement->rgbBackground,
					pElSource->autoBack, pElSource->rgbBackground,
					pElSource->autoFore, pElSource->rgbText);
			}
		}
	}

	UACTRACE( "CFormatInfo::UpdateAutoColors: %s is %sup to date after pass %d.\n", 
		pWindow->szRegEntry,
		bDoItAgain ? "not " : "", 
		cPasses);

// FUTURE: HACK ALERT: InfoViewer triggers this assert and needs to fix it.
ASSERT(!bDoItAgain || (0 == _tcscmp(pWindow->szRegEntry, _T("InfoViewer Topic Window"))));
//  When InfoViewer is fixed, use this ASSERT
//	ASSERT(!bDoItAgain); // table has a cycle or bad entries
}


///// Commit () - Copy back to package's original CFormatInfo
//
///
BOOL CFormatInfo::Commit () const
{
	if (IsDirty()) {
		ASSERT (m_pFormatInfoFromPackage != NULL);
		m_pFormatInfoFromPackage->Update (*this);
		SaveToRegistry ();
		m_pPackage->OnFormatChanged ();
		return TRUE;
		}
	else
		return FALSE;
}


///// IsDirty () - Have any changes been made?
//
//	The changed state is kept on a per-Window basis.  When you need to know whether a change
//	has been made at all, you call this.
//
///
BOOL CFormatInfo::IsDirty () const
{
	for ( int iWinGroup = 0; iWinGroup < m_nWinGroups; iWinGroup++ )
	{
		FMT_WINGROUP * pWinGroup = &m_pWinGroup[iWinGroup];

		for ( int iWindow = 0; iWindow < pWinGroup->nWindows; iWindow++ )
		{
			if (pWinGroup->rgWindows[iWindow].bChanged)
				return TRUE;
		}
	}
	
	return FALSE;
}


///// Clean () - Mark all Windows as unchanged
//
///
void CFormatInfo::Clean ()
{
	for ( int iWinGroup = 0; iWinGroup < m_nWinGroups; iWinGroup++ )
	{
		FMT_WINGROUP * pWinGroup = &m_pWinGroup[iWinGroup];

		for ( int iWindow = 0; iWindow < pWinGroup->nWindows; iWindow++ )
		{
			pWinGroup->rgWindows[iWindow].bChanged = FALSE;
			// HACK!  The PRJ package sometimes hands you a bolded font.
			// That is not what we want.
			if (pWinGroup->rgWindows[iWindow].pLogFont)
				pWinGroup->rgWindows[iWindow].pLogFont->lfWeight = FW_NORMAL;
		}
	}
}


///// GetLogFont () - Find a LOGFONT structure matching the name/size combo.
///
//	** Font sizes and LOGFONT and TEXTMETRIC **
//
//		The TEXTMETRIC member tmHeight describes the height of the character "cell"
//		in device units (i.e. pixels). This cell includes the character below the
//		"baseline" (the descender, like the tail of 'q'), as well as room above the
//		character for accents and other diactrical marks.  This latter area is called
//		"internal leading" (pronounced ledding).
//
//		The "point" size of a character differs in two ways from tmHeight: 1) Internal
//		leading is *not* included, and 2) it is expressed in units of 1/72 of an inch.
//
//		The LOGFONT member lfHeight is an input value to the font API.  When positive, it
//		expresses the same type of value as tmHeight.  When lfHeight is negative, the
//		absolute value of lfHeight is the size of the character not including the internal
//		leading.
//		
///
int CFormatInfo::GetLogFont (
LOGFONTSEARCH * plfs
) {
	if (plfs->szFontFace == NULL)
		return 0;

	CDC * pDC = theApp.m_pMainWnd->GetDC();

	// First, let's try to find an exact match for the face/size pair.
	if (!EnumFontFamilies (pDC->m_hDC, plfs->szFontFace, (FONTENUMPROC)CFormatInfo::SearchCallBack, (LPARAM)plfs)) {
		theApp.m_pMainWnd->ReleaseDC (pDC);
		return 1;
		}

	//
	// That failed, so now let's let the system find a close match
	//
	plfs->LogFont.lfHeight		= -MulDiv (plfs->nFontSize, plfs->nPixPerInchY, 72);
	plfs->LogFont.lfWidth		= 0;
	plfs->LogFont.lfEscapement	= 0;
	plfs->LogFont.lfOrientation = 0;
	plfs->LogFont.lfWeight		= 0;
	plfs->LogFont.lfItalic		= FALSE;
	plfs->LogFont.lfUnderline	= FALSE;
	plfs->LogFont.lfStrikeOut	= FALSE;
	plfs->LogFont.lfCharSet		= plfs->nCharSet;
	plfs->LogFont.lfOutPrecision= OUT_DEFAULT_PRECIS;
	plfs->LogFont.lfClipPrecision=CLIP_DEFAULT_PRECIS;
	plfs->LogFont.lfQuality		= DEFAULT_QUALITY;
	plfs->LogFont.lfPitchAndFamily= DEFAULT_PITCH | FF_DONTCARE;
	_tcsncpy (plfs->LogFont.lfFaceName, plfs->szFontFace, sizeof plfs->LogFont.lfFaceName);

	CFont font;
	font.CreateFontIndirect ( &plfs->LogFont );

	CFont * pFontPrev = pDC->SelectObject ( &font );
	ASSERT (pFontPrev );

	TEXTMETRIC tm;
	VERIFY (pDC->GetTextMetrics ( &tm )); 

	plfs->LogFont.lfHeight		= -(tm.tmHeight - tm.tmInternalLeading);
	plfs->LogFont.lfWidth		= tm.tmAveCharWidth;
	plfs->LogFont.lfWeight		= tm.tmWeight;
	plfs->LogFont.lfCharSet		= tm.tmCharSet;
	// Yes, this is right - the constant TMPF_FIXED_PITCH is the opposite of
	// what the name implies.
	plfs->LogFont.lfPitchAndFamily = ((tm.tmPitchAndFamily & TMPF_FIXED_PITCH) ? VARIABLE_PITCH : FIXED_PITCH)
									 | (tm.tmPitchAndFamily & 0xF0);
	pDC->GetTextFace (sizeof plfs->LogFont.lfFaceName, plfs->LogFont.lfFaceName);

	pDC->SelectObject ( pFontPrev );
	font.DeleteObject ();

	theApp.m_pMainWnd->ReleaseDC (pDC);

	// This stuff may be different than what we asked for.  The caller should check
	// for the negative return and update its corresponding values.
	if (_tcsicmp (plfs->LogFont.lfFaceName, plfs->szFontFace))
		plfs->szFontFace	= plfs->LogFont.lfFaceName;
	plfs->nFontSize		= (UCHAR)MulDiv(tm.tmHeight - tm.tmInternalLeading, 72, plfs->nPixPerInchY);
	plfs->nCharSet		= tm.tmCharSet;
	plfs->bMonospace	= !(tm.tmPitchAndFamily & TMPF_FIXED_PITCH);
	plfs->bTrueType		= tm.tmPitchAndFamily & TMPF_TRUETYPE;
	return -1;
}

void CFormatInfo::UpdateLogFont (FMT_WINDOW * pWindow)
{
LOGFONTSEARCH lfs;

	lfs.szFontFace	= pWindow->szFontFace;
	lfs.nFontSize	= (UCHAR)pWindow->nFontSize;
	lfs.nCharSet	= DEFAULT_CHARSET;
	lfs.nPixPerInchY = m_nPixPerInchY;
	lfs.bMonospace	= pWindow->bMonospace;

	switch (GetLogFont (&lfs))
	{
		case 0:
			if (pWindow->pLogFont != NULL)
				delete pWindow->pLogFont;
			pWindow->pLogFont = NULL;
			break;

		case -1:
			// Yes, this compares pointer values, but we trust GetLogFont
			// to change lfs.szFontFace only if the name actually changes.
			if (lfs.szFontFace != pWindow->szFontFace)
			{
				delete [] pWindow->szFontFace;
				pWindow->szFontFace = NewString (lfs.szFontFace);
			}

			pWindow->nFontSize = lfs.nFontSize;
			ASSERT (!pWindow->bMonospace || lfs.bMonospace);
					
		case 1:
			pWindow->bChanged = TRUE;
			if (pWindow->pLogFont == NULL)
				pWindow->pLogFont = new LOGFONT;
			*pWindow->pLogFont = lfs.LogFont;
			break;

	}
}


int CALLBACK CFormatInfo::SearchCallBack(
CONST ENUMLOGFONT * pEnumLogFont,
CONST NEWTEXTMETRIC *	pTextMetric,
int	fontType,
LPARAM	lParam )
{
LOGFONTSEARCH * plfs = (LOGFONTSEARCH * )lParam;

	if (!(fontType & TRUETYPE_FONTTYPE)) {
		//DWORD height = 	(fontType & RASTER_FONTTYPE) ? pTextMetric->tmHeight - pTextMetric->tmInternalLeading : pEnumLogFont->elfLogFont.lfHeight;
		DWORD height = 	pTextMetric->tmHeight - pTextMetric->tmInternalLeading;
		DWORD cPoints = MulDiv(height, 72, plfs->nPixPerInchY);

		if (cPoints != plfs->nFontSize)
			return 1;
		}
	else {
		}

	// Before it can be a match, the character set has to be right
	//
	if (plfs->nCharSet == DEFAULT_CHARSET || pTextMetric->tmCharSet == OEM_CHARSET)
	{
		plfs->nCharSet = pTextMetric->tmCharSet;
	}
	else
	if (plfs->nCharSet != pTextMetric->tmCharSet)
		return 1;

	//
	// Match!
	//
	memcpy ( &plfs->LogFont, &pEnumLogFont->elfLogFont, sizeof (LOGFONT));

	if (fontType & TRUETYPE_FONTTYPE) {
		// Using a negative value in the lfHeight member causes GDI to use the value
		// as the height of the character glyphs, not the height of the character cell.
		plfs->LogFont.lfHeight = -MulDiv (plfs->nFontSize, plfs->nPixPerInchY, 72);
		plfs->LogFont.lfWidth = 0;		
		plfs->bTrueType = TRUE;
		}
	else 
		plfs->bTrueType = FALSE;

	return 0;
}


///// MewString - duplicate a string, using operator new
//
///
_TCHAR * NewString (const _TCHAR * sz)
{
	_TCHAR * szCopy = new _TCHAR[_tcslen(sz) + 1];
	_tcscpy (szCopy, sz);
	return szCopy;
}

//////////////////////////////////////////////////////////////////////////////
//						class CFmtIterator									//
//						class CFmtGroupIterator								//
//						class CFmtWindowIterator							//
//						class CFmtElementIterator							//
//																			//
//	Iteration classes for CFormatInfo.  Each class iterates over one type.	//
//	Normally, the domain is the array of <type> attached to the current		//
//  <basetype>, where <basetype> is the type of the base class.				//
//	Iteration can also be done "linearly", meaning that all instances of	//
//	<type> are reached in a "flat addressing" fashion.						//
//																			//
//////////////////////////////////////////////////////////////////////////////

///// Base class - handles array of FormatInfo
//
///
CFmtIterator::CFmtIterator (const CObArray& rgFmtInfo)
:
m_rgFmtInfo (rgFmtInfo),
m_iFmtInfo  (0)
{}


CFormatInfo * CFmtIterator::Get ()
{
	CFormatInfo * pFmtInfo = Peek ();
	
	if (pFmtInfo)
		Inc();

	return pFmtInfo;
}


CFormatInfo * CFmtIterator::Peek () const
{
	if (m_iFmtInfo >= m_rgFmtInfo.GetSize())
		return NULL;

	return (CFormatInfo *)m_rgFmtInfo[m_iFmtInfo];
}


///// First derived class - handles array of groups within a CFormatInfo
//
///
CFmtGroupIterator::CFmtGroupIterator (const CObArray& rgFmtInfo)
:
CFmtIterator (rgFmtInfo),
m_iGroup (0)
{}


FMT_WINGROUP * CFmtGroupIterator::Get ()
{
	FMT_WINGROUP * pGroup = Peek();

	if (pGroup)
		Inc();

	return pGroup;
}


FMT_WINGROUP * CFmtGroupIterator::Peek () const
{
	CFormatInfo * pFI = CFmtIterator::Peek();

	if (pFI == NULL)
		return NULL;

	if (m_iGroup >= pFI->m_nWinGroups)
		return NULL;

	return &pFI->m_pWinGroup[m_iGroup];
}


FMT_WINGROUP *  CFmtGroupIterator::GetLinear ()
{
	FMT_WINGROUP * pGroup = Get();

	if (pGroup)
		return pGroup;

	// Current FormatInfo is exhausted, try to move to next
	if (CFmtIterator::Get() && CFmtIterator::Peek())
	{
		m_iGroup = 0;
		return Get();
	}
	else
		return NULL;
}

/////
//
// Like Peek(), except that out-of-bounds indices are fixed up first
///
FMT_WINGROUP * CFmtGroupIterator::PeekLinear ()
{
	CFormatInfo * pFI = CFmtIterator::Peek();

	if (pFI == NULL)
		return NULL;

	if (m_iGroup >= pFI->m_nWinGroups)
	{
		(void)CFmtIterator::Get();
		if (NULL == (pFI = CFmtIterator::Peek()))
			return NULL;
		m_iGroup = 0;
	}

	return &pFI->m_pWinGroup[m_iGroup];
}


void	CFmtGroupIterator::SetLinear (UINT iGroupLinear)
{
	// Translate the linear iGroup argument into a fmtinfo/igroup pair.
	//
	m_iGroup = iGroupLinear;
	for (m_iFmtInfo = 0; m_iFmtInfo < m_rgFmtInfo.GetSize(); m_iFmtInfo++)
	{
		CFormatInfo * pFmtInfo = (CFormatInfo *)m_rgFmtInfo[m_iFmtInfo];

		if (m_iGroup >= pFmtInfo->m_nWinGroups)
			m_iGroup -= pFmtInfo->m_nWinGroups;
		else
			return;	// We're done!
	}
}


void	CFmtGroupIterator::Set (FMT_WINGROUP * pGroup)
{
	// Find this group, then set iterator values
	for (m_iFmtInfo = 0; m_iFmtInfo < m_rgFmtInfo.GetSize (); m_iFmtInfo++)
		for (m_iGroup = 0; m_iGroup < ((CFormatInfo *)m_rgFmtInfo[m_iFmtInfo])->m_nWinGroups; m_iGroup++)
			if (pGroup == &((CFormatInfo *)m_rgFmtInfo[m_iFmtInfo])->m_pWinGroup[m_iGroup])
				return;

	ASSERT (FALSE);
}


///// Second derived class - handles array of Windows within a group
//
///
CFmtWindowIterator::CFmtWindowIterator (const CObArray& rgFmtInfo)
:
CFmtGroupIterator (rgFmtInfo),
m_iWindow (0)
{
}


FMT_WINDOW * CFmtWindowIterator::Peek () const
{
	FMT_WINGROUP * pGroup = CFmtGroupIterator::Peek();

	if (pGroup == NULL)
		return NULL;

	if (m_iWindow >= pGroup->nWindows)
		return NULL;

	return &pGroup->rgWindows[m_iWindow];
}


FMT_WINDOW * CFmtWindowIterator::Get ()
{
	FMT_WINDOW * pWindow = Peek();

	if (pWindow)
		Inc();

	return pWindow;
}


FMT_WINDOW * CFmtWindowIterator::GetLinear ()
{
	FMT_WINDOW * pWindow = Get();

	if (pWindow)
		return pWindow;

	// Current WINGROUP is exhausted, try to move to next
	if (CFmtGroupIterator::GetLinear() && CFmtGroupIterator::PeekLinear())
	{
		m_iWindow = 0;
		return Get();
	}
	else
		return NULL;
}


void CFmtWindowIterator::SetLinear (UINT iWinLinear)
{

	// Translate the linear nWindow argument into a fmtinfo/igroup/iWindow set,
	//
	m_iWindow = iWinLinear;
	for (m_iFmtInfo = 0; m_iFmtInfo < m_rgFmtInfo.GetSize(); m_iFmtInfo++)
	{
		CFormatInfo * pFmtInfo = (CFormatInfo *)m_rgFmtInfo[m_iFmtInfo];

		for (	m_iGroup = 0;
				m_iGroup < pFmtInfo->m_nWinGroups;
				m_iGroup++ )
		{
			if (m_iWindow >= pFmtInfo->m_pWinGroup[m_iGroup].nWindows)
				m_iWindow -= pFmtInfo->m_pWinGroup[m_iGroup].nWindows;
			else
				return;	// We're done!
		}
	}
}


void	CFmtWindowIterator::Set (FMT_WINDOW * pWindow)
{
	// Find this group, then set iterator values
	for (m_iFmtInfo = 0; m_iFmtInfo < m_rgFmtInfo.GetSize (); m_iFmtInfo++)
		for (m_iGroup = 0; m_iGroup < ((CFormatInfo *)m_rgFmtInfo[m_iFmtInfo])->m_nWinGroups; m_iGroup++)
			for (m_iWindow = 0; m_iWindow < ((CFormatInfo *)m_rgFmtInfo[m_iFmtInfo])->m_pWinGroup[m_iGroup].nWindows; m_iWindow++)
				if (pWindow == &((CFormatInfo *)m_rgFmtInfo[m_iFmtInfo])->m_pWinGroup[m_iGroup].rgWindows[m_iWindow])
					return;

	ASSERT (FALSE);
}


FMT_WINDOW * CFmtWindowIterator::PeekLinear ()
{
	FMT_WINGROUP * pGroup = CFmtGroupIterator::PeekLinear();

	if (pGroup == NULL)
		return NULL;

	if (m_iWindow >= pGroup->nWindows)
	{
		(void)CFmtGroupIterator::Get ();
		if (NULL == (pGroup = CFmtGroupIterator::PeekLinear()))
			return NULL;
		m_iWindow = 0;
	}

	return &pGroup->rgWindows[m_iWindow];
}


///// Third derived class - handles array of Elements within a Window
//
///
CFmtElementIterator::CFmtElementIterator (const CObArray& rgFmtInfo)
:
CFmtWindowIterator (rgFmtInfo),
m_iElement (0)
{}


FMT_ELEMENT * CFmtElementIterator::Peek () const
{
	FMT_WINDOW * pWindow = CFmtWindowIterator::Peek();

	if (pWindow == NULL)
		return NULL;

	if (m_iElement >= pWindow->nElements)
		return NULL;

	return &pWindow->rgElements[m_iElement];
}


FMT_ELEMENT * CFmtElementIterator::Get ()
{
	FMT_ELEMENT * pElement = Peek ();

	if (pElement)
		Inc();

	return pElement;
}


///// CFmtElementIterator Linear functions
//
// The normal Get() function retrieves ELEMENTS in the current window, then stops.
// GetLinear() retrieves all elements in linear fashion.
//
///
FMT_ELEMENT * CFmtElementIterator::GetLinear ()
{
	do
	{

		FMT_ELEMENT * pElement = Get();

		if (pElement)
			return pElement;

		// Current WINDOW is exhausted, try to move to next
		if (CFmtWindowIterator::GetLinear() && CFmtWindowIterator::PeekLinear())
		{
			m_iElement = 0;
		}
		else
			return NULL;

	} while (TRUE);
		
}

void CFmtElementIterator::SetLinear (UINT iElementLinear)
{

	// Translate the linear nWindow argument into a fmtinfo/igroup/iWindow set,
	//
	m_iElement = iElementLinear;
	for (m_iFmtInfo = 0; m_iFmtInfo < m_rgFmtInfo.GetSize(); m_iFmtInfo++)
	{
		CFormatInfo * pFmtInfo = (CFormatInfo *)m_rgFmtInfo[m_iFmtInfo];

		for (	m_iGroup = 0;
				m_iGroup < pFmtInfo->m_nWinGroups;
				m_iGroup++ )
		{
			for (	m_iWindow = 0;
					m_iWindow < pFmtInfo->m_pWinGroup[m_iGroup].nWindows;
					m_iWindow++ )
			{
			
				if (m_iElement >= pFmtInfo->m_pWinGroup[m_iGroup].rgWindows[m_iWindow].nElements)
					m_iElement -= pFmtInfo->m_pWinGroup[m_iGroup].rgWindows[m_iWindow].nElements;
				else
					return;	// We're done!
			}
		}
	}
}

FMT_ELEMENT * CFmtElementIterator::PeekLinear ()
{
	FMT_WINDOW * pWindow = CFmtWindowIterator::PeekLinear();

	if (pWindow == NULL)
		return NULL;

	while (m_iElement >= pWindow->nElements)
	{
		(void)CFmtWindowIterator::Get();
		if (NULL == (pWindow = CFmtWindowIterator::PeekLinear()))
			return NULL;
		m_iElement = 0;
	}

	return &pWindow->rgElements[m_iElement];
}


//////////////////////////////////////////////////////////////////////////////
//							class CElList									//
//																			//
// A list of unique element names.  Uniqueness is enforced.  Will also		//
// iterate a given name - i.e. will sequentially find all elements with		//
// the given name.															//
//																			//
//////////////////////////////////////////////////////////////////////////////

CElList::CElList (UINT inc)
: m_inc(inc)
{
	m_rgStrings.SetSize(0, inc);
}

CElList::~CElList ()
{
	Clear ();
}

BOOL CElList::AddString (const CString& str)
{
	for (int iString = 0; iString < m_rgStrings.GetSize(); iString++)
	{
		if ((_TCHAR *)m_rgStrings[iString] == str)
			return FALSE;
	}

	m_rgStrings.Add (NewString (str));

	return TRUE;
}

///// CElList Iteration functions
//
// This is a lot cleaner if the passed-in iterator is allowed to come
// back one past the found item.  However, some callers really need the
// iterator to point to the returned ELEMENT...
///
FMT_ELEMENT * CElList::GetFirst (_TCHAR * szName, CFmtElementIterator& it, int iFormat, int iGroup)
{
	it.SetLinear (0);

	m_strSearch = szName;

	it.Dec ();	// HACK!

	return GetNext (it, iFormat, iGroup);
}


FMT_ELEMENT * CElList::GetFirst (UINT iEl, CFmtElementIterator& it, int iFormat, int iGroup)
{
	return GetFirst ((_TCHAR *)m_rgStrings[iEl], it, iFormat, iGroup);
}


FMT_ELEMENT * CElList::GetNext (CFmtElementIterator& it, int iFormat, int iGroup)
{
	FMT_ELEMENT * pElement;
	do
	{
		it.Inc();

		if (pElement = it.PeekLinear())
		{
			if (!_tcsicmp (m_strSearch, pElement->szElement)
				&& ( (iFormat == -1)
					|| (	(it.CFmtIterator::GetIndex() == (UINT)iFormat)
						&&	(it.CFmtGroupIterator::GetIndex() == (UINT)iGroup) )))
				return pElement;
		}
		else
			break;
	} while (TRUE);

	return NULL;
}


///// IsFore/BackColorUnique
//
//	Is the color in each of the elements using the given name the same?
//
//  'rgb' is a return value.	If IFCTS() returns TRUE, rgb is set to the color in question.
//								If IFCTS() returns FALSE, rgb is undefined.
///
BOOL CElList::IsForeColorTheSame (UINT iEl, CFmtElementIterator& itEl, int iFormat, int iGroup, COLORREF& rgb)
{
	FMT_ELEMENT * pElement = GetFirst (iEl, itEl, iFormat, iGroup);
	ASSERT (pElement);
	FMT_ELEMENT * pElementNext;

	do
	{
		pElementNext = GetNext (itEl, iFormat, iGroup);

		if (pElementNext && ((pElement->rgbText != pElementNext->rgbText) || (pElement->autoFore.bOn != pElementNext->autoFore.bOn)))
				return FALSE;

		rgb = pElement->autoFore.bOn ? CColorCombo::rgbAuto : pElement->rgbText;

	} while (pElement = pElementNext);

	return TRUE;
}

BOOL CElList::IsBackColorTheSame (UINT iEl, CFmtElementIterator& itEl, int iFormat, int iGroup, COLORREF& rgb)
{
	FMT_ELEMENT * pElement = GetFirst (iEl, itEl, iFormat, iGroup);
	ASSERT (pElement);
	FMT_ELEMENT * pElementNext;

	do
	{

		pElementNext = GetNext (itEl, iFormat, iGroup);

		if (pElementNext && ((pElement->rgbBackground != pElementNext->rgbBackground) || (pElement->autoBack.bOn != pElementNext->autoBack.bOn)))
				return FALSE;

		rgb = pElement->autoBack.bOn ? CColorCombo::rgbAuto : pElement->rgbBackground;

	} while (pElement = pElementNext);

	return TRUE;
}


/////
//
///
void CElList::Fill (CListBox& lbox) const
{
	for (int iString = 0; iString < m_rgStrings.GetSize(); iString++)
	{
		lbox.AddString ((_TCHAR *)m_rgStrings[iString]);
	}
}



void CElList::Clear ()
{
	for (int iString = 0; iString < m_rgStrings.GetSize(); iString++)
	{
		delete [] m_rgStrings[iString];
	}

	m_rgStrings.RemoveAll ();
}

//////////////////////////////////////////////////////////////////////////////
//							class CWinList									//
//																			//
//	A list of FMT_WINDOW and FMT_WINGROUP.  Used for the list of windows	//
//	under "Category" in the Fonts dialogs.									//
//																			//
//////////////////////////////////////////////////////////////////////////////

CWinList::CWinList (UINT inc)
{
	m_rgGroups.SetSize (0, inc);
	m_rgWindows.SetSize (0, inc);
}

CWinList::~CWinList ()
{
}

void CWinList::Add (FMT_WINGROUP * pGroup)
{
	if (pGroup->nWindows == 1)
	{
		if (pGroup->bSingleGroup)
			m_rgGroups.Add (pGroup);
		else
			m_rgWindows.Add (pGroup->rgWindows);
	}
	else
	{
		m_rgGroups.Add (pGroup);

		for (int iWin = 0; iWin < pGroup->nWindows; iWin++)
		{
			m_rgWindows.Add (&pGroup->rgWindows[iWin]);
		}	
	}

}


void CWinList::Fill (CListBox& lbox) const
{
	CString strAllWindows;
	strAllWindows.LoadString (IDS_ALLWINDOWS);
	lbox.AddString (strAllWindows);
	
	for (int iGroup = 0; iGroup < m_rgGroups.GetSize(); iGroup++)
		lbox.AddString (((FMT_WINGROUP *)(m_rgGroups[iGroup]))->szName);

	for (int iWindow = 0; iWindow < m_rgWindows.GetSize(); iWindow++)
		lbox.AddString (((FMT_WINDOW *)m_rgWindows[iWindow])->szWindow);
}

BOOL CWinList::GetName (int index, CString& strName) const
{
	// The index value is the item ID from the listbox.  So, 0 == "All Windows",
	// 1 -> m_rgGroups.GetSize () are group names, and the rest are window names.
	if (index == 0)
	{
		strName.LoadString (IDS_ALLWINDOWS);
		return TRUE;
	}
	else
	if (index > m_rgGroups.GetSize ())
	{
		index -= m_rgGroups.GetSize () + 1;
		ASSERT (index < m_rgWindows.GetSize ());
		strName = ((FMT_WINDOW *)m_rgWindows[index])->szWindow;
		return FALSE;
	}
	else
	{
		strName = ((FMT_WINGROUP *)m_rgGroups[index - 1])->szName;
		return TRUE;
	}
}

BOOL CWinList::GetPtr (int index, void * &p) const
{
	if (index == 0)
	{
		return TRUE;
	}
	else
	if (index > m_rgGroups.GetSize ())
	{
		index -= m_rgGroups.GetSize () + 1;
		ASSERT (index < m_rgWindows.GetSize ());
		p = m_rgWindows[index];
		return FALSE;
	}
	else
	{
		p = m_rgGroups[index - 1];
		return TRUE;
	}
}

int CWinList::FirstWindowIndex () const
{
	return m_rgGroups.GetSize() + 1;
}

//////////////////////////////////////////////////////////////////////////////
//							class CFCDialogState							//
//																			//
//	This class acts as an intermediary between a CFontColorDlg and the		//
//	CFormatInfo objects underlying it.  It locates and manages the data		//
//	from the packages, it keeps track of the state of the dialog and		//
//  propogates changes to the underlying data.  It also propogates state	//
//	changes in one part of the dialog to the rest of the dialog.			//
//																			//
//////////////////////////////////////////////////////////////////////////////

CFCDialogState::CFCDialogState ( CFontColorDlg * pDialog )
:
m_pDialog(pDialog),
m_state (PreDialog),
m_nFmtInfoCur (0),
m_nWinGroupCur (0),
m_nWindowCur (0),
m_nElementCur (0)
{
	CFormatInfo * pFormatInfo;
	CPackage * pPackage;
	POSITION pos = theApp.m_packages.GetHeadPosition();
	while (pos != NULL)
	{
		pPackage = (CPackage*)theApp.m_packages.GetNext(pos);
		if (pPackage->m_flags & PKS_FORMAT)
		{
			pFormatInfo = new CFormatInfo (pPackage);
			if (pFormatInfo->GetFormatInfo (pPackage))
			{
				pFormatInfo->Clean ();
				m_rgFmtInfo.Add (pFormatInfo);
			}
			else
			{
				ASSERT(0); // the package lied! PKS_FORMAT, but GetFormatInfo returned NULL
				delete pFormatInfo;
			}
		}
	}
}

CFCDialogState::~CFCDialogState ()
{
int cFmtInfo = m_rgFmtInfo.GetSize();

	for (int i = 0; i < cFmtInfo; i++)
		delete m_rgFmtInfo[i];

	// Let CObArray destructor do the rest
}


void CFCDialogState::InitDialog ()
{
	// First, we fill the dialog's m_lstWindows combo box with the names of
	// all the windows.
#if 0
	int nGroups = 1;
	CString strAllWindows;
	strAllWindows.LoadString (IDS_ALLWINDOWS);
	m_pDialog->m_lstWindows.AddString (strAllWindows);

	FMT_WINGROUP * pGroup;
	CFmtGroupIterator  itGroup (m_rgFmtInfo);
	while (pGroup = itGroup.GetLinear())
	{
		m_pDialog->m_lstWindows.AddString (pGroup->szName);
		nGroups++;
	}
	m_pDialog->m_lstWindows.SetFirstWindowIndex (nGroups);

	CFmtWindowIterator itWin (m_rgFmtInfo);
	FMT_WINDOW * pWindow;
	while (pWindow = itWin.GetLinear())
	{
		//if (!pWindow->bSingleton)
			m_pDialog->m_lstWindows.AddString (pWindow->szWindow);
	}
#endif
	FMT_WINGROUP * pGroup;
	CFmtGroupIterator  itGroup (m_rgFmtInfo);
	while (pGroup = itGroup.GetLinear())
	{
		m_winList.Add (pGroup);
	}

	m_pDialog->m_lstWindows.SetWinList (&m_winList);

	m_winList.Fill (m_pDialog->m_lstWindows);
	
	m_pDialog->m_lstWindows.SetCurSel (0);

	m_pDialog->OnChangeWindow ();

	m_state = SelAll;
}


///// CFCDialogState::SetCurrentWindow ()
//
//	"iWindow" means the i'th element in the Window combo has been selected.  This function
//	translates that into a window reference/state-change as needed, and propogates the change
//	to the rest of the dialog.
//
///
void CFCDialogState::SetCurrentWindow (int iWindow)
{
	// First we decide whether the user has selected an individual
	// window, a group of windows, or ALL windows.
	void * ptr;
	BOOL bIsGroup = m_winList.GetPtr (iWindow, ptr);

	m_iCurWin = iWindow;
	m_nElementCur	= 0;

	m_elList.Clear ();

	if (!bIsGroup)
	{	// This is the single window case.
		//
		m_state = SelWindow;
	

		CFmtElementIterator itEl (m_rgFmtInfo);
		itEl.CFmtWindowIterator::Set ((FMT_WINDOW *)ptr);

		m_nFmtInfoCur	= itEl.CFmtIterator::GetIndex();
		m_nWinGroupCur	= itEl.CFmtGroupIterator::GetIndex();
		m_nWindowCur	= itEl.CFmtWindowIterator::GetIndex();

		// Now we refill the Elements listbox
		//
		FMT_ELEMENT * pElement;
		while (pElement = itEl.Get())
		{
			m_elList.AddString (pElement->szElement);
		}
	}
	else
	{
		if ( iWindow > 0 )
		{	// This is the WinGroup case
			m_state = SelGroup;

			CFmtElementIterator itEl (m_rgFmtInfo);
			itEl.CFmtGroupIterator::Set ((FMT_WINGROUP *)ptr);

			m_nFmtInfoCur	= itEl.CFmtIterator::GetIndex();
			m_nWinGroupCur	= itEl.CFmtGroupIterator::GetIndex();
			LPCTSTR pszGroupName = ((FMT_WINGROUP *)ptr)->szName;
			m_nWindowCur	= (UINT)-1;

			// Now we refill the Elements listbox
			//
			FMT_ELEMENT * pElement;
			while (pElement = itEl.GetLinear())
			{
				// Is this element in the current group?
				FMT_WINGROUP *pwgCur = itEl.CFmtGroupIterator::Peek();
				ASSERT(pwgCur);
				if (0 != _tcscmp(pszGroupName,pwgCur->szName))
					continue;

				m_elList.AddString (pElement->szElement);

			}
		
		}
		else
		{	// This is the All Windows case
			ASSERT (iWindow == 0);
			m_state = SelAll;
			CFmtElementIterator itEl (m_rgFmtInfo);

			m_nFmtInfoCur	= 0;
			m_nWinGroupCur	= 0;
			m_nWindowCur	= (UINT)-1;

			// Now we refill the Elements listbox
			//
			FMT_ELEMENT * pElement;
			while (pElement = itEl.GetLinear())
			{
				m_elList.AddString (pElement->szElement);
			}
		}
		
	}

	if (m_elList.GetSize())
	{
		m_pDialog->m_lstElements.ResetContent();
		m_elList.Fill (m_pDialog->m_lstElements);
		m_pDialog->m_lstElements.EnableWindow (m_benElements = TRUE);
		m_pDialog->m_cmbBackColor.EnableWindow (m_benBackColor = TRUE);
		m_pDialog->m_cmbForeColor.EnableWindow (m_benForeColor = TRUE);
		m_pDialog->m_lstElements.SetCurSel (0);
		m_pDialog->OnChangeElement ();
	}
	else
	{
		m_pDialog->m_lstElements.EnableWindow (m_benElements = FALSE);
		m_pDialog->m_cmbBackColor.EnableWindow (m_benBackColor = FALSE);
		m_pDialog->m_cmbForeColor.EnableWindow (m_benForeColor = FALSE);
		m_pDialog->m_lstElements.ResetContent();

	}


	// Now decide whether proportional fonts are enabled in the new window, and
	// fill the font list if it needs it.

	m_pDialog->m_cmbFontName.Enumerate ( WindowWalk (FontPitch) == PitchProportional );

	switch (WindowWalk (FontEquality))
	{
		case SameFont:
			FMT_WINDOW * pWinCur;
			int iCurFont;
			pWinCur = GetWinCur (FALSE, TRUE);
			ASSERT (pWinCur->szFontFace[0]);

			// Now set the font name and size 
			m_pDialog->m_cmbFontName.EnableWindow ( TRUE );
			iCurFont = m_pDialog->m_cmbFontName.FindStringExact (0, pWinCur->szFontFace);
			if (iCurFont < 0)
			{
				// Bug Fix for Orion 96 Bug 19746
				goto diffont ;
			}

			m_pDialog->m_cmbFontName.SetCurSel ( iCurFont );

			m_pDialog->RegenSizes (FALSE);
			m_pDialog->m_cmbFontSize.SetPreferredSize (pWinCur->nFontSize);
			m_pDialog->m_cmbFontSize.SetToSize (pWinCur->nFontSize, !(m_pDialog->m_cmbFontName.GetItemData (iCurFont) & TRUETYPE_FONTTYPE));
			break;

		case NoWindows:
		case NoFont:
			m_pDialog->m_cmbFontName.EnableWindow ( FALSE );
			goto diffont;
			
		case FontsDiffer:
			m_pDialog->m_cmbFontName.EnableWindow ( TRUE );
		diffont:
			m_pDialog->m_cmbFontName.SetCurSel ( -1 );
			m_pDialog->m_cmbFontSize.SetCurSel ( -1 );
			m_pDialog->m_cmbFontSize.EnableWindow (FALSE);
			break;
	}

}


///// SetCurrentElement
//
///
void CFCDialogState::SetCurrentElement  (int iElement)
{
	FMT_ELEMENT * pElement;
	CFmtElementIterator it (m_rgFmtInfo);

	m_nElementCur = iElement;

	switch (m_state)
	{
		case SelGroup:
		case SelAll:
			int iFormat;
			int iGroup;

			if (m_state == SelAll)
			{
				iFormat = -1;
				iGroup = -1;
			}
			else
			{
				iFormat = m_nFmtInfoCur;
				iGroup = m_nWinGroupCur;
			}

			COLORREF rgb;

			if (m_elList.IsForeColorTheSame (m_nElementCur, it, iFormat, iGroup, rgb))
				m_pDialog->m_cmbForeColor.SetCurColor (rgb);
			else
			{
				m_pDialog->m_cmbForeColor.SetCurSel (-1);
			}

			if (m_elList.IsBackColorTheSame (m_nElementCur, it, iFormat, iGroup, rgb))
				m_pDialog->m_cmbBackColor.SetCurColor (rgb);
			else
			{
				m_pDialog->m_cmbBackColor.SetCurSel (-1);
			}

			break;

		case SelWindow:
			ASSERT (iElement >= -1 && iElement < GetWinCur(FALSE, FALSE)->nElements);
			pElement =  &GetWinCur()->rgElements[m_nElementCur];

			m_pDialog->m_cmbForeColor.SetCurColor (pElement->autoFore.bOn ? CColorCombo::rgbAuto : pElement->rgbText);
			m_pDialog->m_cmbBackColor.SetCurColor (pElement->autoBack.bOn ? CColorCombo::rgbAuto : pElement->rgbBackground);
			break;

		default:
			ASSERT(FALSE);
	}
}


///// SetCurrentFont
//
//	Change the font and size in the current window.
//
///
void CFCDialogState::SetCurrentFont (const CString& strFontName, int ptSize, BOOL bTrueType)
{
FMT_WINDOW * pWindow;
CFmtWindowIterator itWin (m_rgFmtInfo);

	// First figure out what font size to use.  The given size may not be available, so we try to
	// set the size in the Font Size control, then use whatever it came up with.
	//
	ptSize = m_pDialog->m_cmbFontSize.GetSize (m_pDialog->m_cmbFontSize.SetToSize (ptSize, !bTrueType));


	switch (m_state)
	{
		case SelGroup:
			itWin.CFmtGroupIterator::Set(m_nFmtInfoCur, m_nWinGroupCur);
			while (pWindow = itWin.Get())
			{
				if (pWindow->szFontFace != NULL)
				{
					delete [] pWindow->szFontFace;

					pWindow->szFontFace = NewString (strFontName);
					pWindow->nFontSize = ptSize;

					CFormatInfo::UpdateLogFont (pWindow);
				}
			}
			break;

		case SelAll:
			itWin.CFmtGroupIterator::SetLinear(0);
			while (pWindow = itWin.GetLinear())
			{
				if (pWindow->szFontFace != NULL)
				{
					delete [] pWindow->szFontFace;

					pWindow->szFontFace = NewString (strFontName);
					pWindow->nFontSize = ptSize;


					CFormatInfo::UpdateLogFont (pWindow);
				}
			}
			break;

		case SelWindow:
			pWindow = GetWinCur(FALSE, TRUE);

			// Set the face
			//
			ASSERT (pWindow->szFontFace != NULL);
			delete [] pWindow->szFontFace;

			pWindow->szFontFace = NewString (strFontName);
			pWindow->nFontSize = ptSize;

			CFormatInfo::UpdateLogFont (pWindow);
			break;

		default:
			ASSERT(FALSE);
	}
}

/////
//
///
void CFCDialogState::SetCurrentFontSize (int nFontSize)
{
FMT_WINDOW * pWindow;
CFmtWindowIterator itWin (m_rgFmtInfo);

	switch (m_state)
	{
		case SelGroup:
			itWin.CFmtGroupIterator::Set(m_nFmtInfoCur, m_nWinGroupCur);
			while (pWindow = itWin.Get())
			{
				pWindow->nFontSize	= nFontSize;
				CFormatInfo::UpdateLogFont (pWindow);
			}
			break;

		case SelAll:
			itWin.CFmtGroupIterator::SetLinear(0);
			while (pWindow = itWin.GetLinear())
			{
				pWindow->nFontSize	= nFontSize;
				CFormatInfo::UpdateLogFont (pWindow);
			}
			break;

		case SelWindow:
			pWindow = GetWinCur(FALSE, TRUE);

			pWindow->nFontSize	= nFontSize;
			CFormatInfo::UpdateLogFont (pWindow);
			break;

		default:
			ASSERT(FALSE);
	}
}

void CFCDialogState::SetForeColor ( COLORREF rgb )
{
CFmtElementIterator itEl (m_rgFmtInfo);
FMT_WINGROUP * pGroupCur = NULL;

void * p;
	m_winList.GetPtr (m_iCurWin, p);

	switch (m_state)
	{
		case SelGroup:
			// FUTURE: why won't the compiler allow pGroupCur to be passed?
			pGroupCur = (FMT_WINGROUP *)p;

		case SelAll:
			m_elList.GetFirst (m_nElementCur, itEl);
			do
			{
				if (m_state == SelGroup && itEl.CFmtGroupIterator::Peek() != pGroupCur)
					continue;

				((CFormatInfo *)m_rgFmtInfo[itEl.CFmtIterator::GetIndex()])
					->SetForeColor (itEl.CFmtGroupIterator::GetIndex()
									, itEl.CFmtWindowIterator::GetIndex()
									, itEl.GetIndex()
									, rgb);
			} while ( m_elList.GetNext (itEl));
			
			break;

		case SelWindow:
			itEl.CFmtWindowIterator::Set ((FMT_WINDOW *)p);

			((CFormatInfo *)m_rgFmtInfo[itEl.CFmtIterator::GetIndex()])-> SetForeColor (itEl.CFmtGroupIterator::GetIndex(), itEl.CFmtWindowIterator::GetIndex(), m_nElementCur, rgb);
			break;

		default:
			ASSERT (FALSE);
	}

	UpdateAllAutoColors ();
}

void CFCDialogState::SetBackColor ( COLORREF rgb )
{
CFmtElementIterator itEl (m_rgFmtInfo);
FMT_WINGROUP * pGroupCur = NULL;

	void * p;
	m_winList.GetPtr (m_iCurWin, p);

	switch (m_state)
	{
		case SelGroup:
			pGroupCur = (FMT_WINGROUP *)p;

		case SelAll:
			m_elList.GetFirst (m_nElementCur, itEl);
			do
			{
				if (m_state == SelGroup && itEl.CFmtGroupIterator::Peek() != pGroupCur)
					continue;

				((CFormatInfo *)m_rgFmtInfo[itEl.CFmtIterator::GetIndex()])
					->SetBackColor (itEl.CFmtGroupIterator::GetIndex()
									, itEl.CFmtWindowIterator::GetIndex()
									, itEl.GetIndex()
									, rgb);
			} while ( m_elList.GetNext (itEl));
			
			break;

		case SelWindow:
			itEl.CFmtWindowIterator::Set ((FMT_WINDOW *)p);

			((CFormatInfo *)m_rgFmtInfo[itEl.CFmtIterator::GetIndex()])-> SetBackColor (itEl.CFmtGroupIterator::GetIndex(), itEl.CFmtWindowIterator::GetIndex(), m_nElementCur, rgb);
			break;

		default:
			ASSERT (FALSE);
	}

	UpdateAllAutoColors ();
}


///// CFCDialogState::UpdateAllAutoColors() - Update *ALL* auto colors
//
///
void CFCDialogState::UpdateAllAutoColors ()
{
CFmtIterator it (m_rgFmtInfo);

	CFormatInfo * pFmt;

	while (pFmt = it.Get())
		pFmt->UpdateAllAutoColors ();
}


COLORREF CFCDialogState::GetForeColor ()
{
CFmtElementIterator itEl (m_rgFmtInfo);

	if (m_benForeColor)
	{
		switch (m_state)
		{
			case SelAll:
				return m_elList.GetFirst (m_nElementCur, itEl)->rgbText;

			case SelGroup:
				return  m_elList.GetFirst (m_nElementCur, itEl, m_nFmtInfoCur, m_nWinGroupCur)->rgbText;

			case SelWindow:
				return GetWinCur(FALSE, FALSE)->rgElements[m_nElementCur].rgbText;

			default:
				ASSERT (FALSE);
				return 0;
		}	
	}
	else
		return GetSysColor (COLOR_WINDOWTEXT);	
}



COLORREF CFCDialogState::GetBackColor ()
{
CFmtElementIterator itEl (m_rgFmtInfo);

	if (m_benBackColor)
	{
		switch (m_state)
		{
			case SelAll:
				return m_elList.GetFirst (m_nElementCur, itEl)->rgbBackground;

			case SelGroup:
				return  m_elList.GetFirst (m_nElementCur, itEl, m_nFmtInfoCur, m_nWinGroupCur)->rgbBackground;

			case SelWindow:
				return GetWinCur(FALSE, FALSE)->rgElements[m_nElementCur].rgbBackground;

			default:
				ASSERT (FALSE);
				return 0;
		}	
	}
	else
		return GetSysColor (COLOR_WINDOW);	
}


///// IsFontUnique ()
//
//	Possible acts are:
//
//		FontEquality - Are all the windows using the same font
//		FontPitch - Are all the windows fixed-pitch only, or all not or mixed.
///
CFCDialogState::WW_RESULT CFCDialogState::WindowWalk (WW_ACTION act) const
{
CFmtWindowIterator itWin (m_rgFmtInfo);
LOGFONT lf;
BOOL bFirstTime = TRUE;
FMT_WINDOW * pWindow;
WW_RESULT result = NoWindows;


	switch (m_state)
	{
		case SelWindow:
			if (NULL == (pWindow = GetWinCur()))
				break;

			if (pWindow->szFontFace == NULL)
				return NoFont;

			switch (act)
			{
				case FontEquality:
					return SameFont;

				case FontPitch:
					ASSERT (pWindow);
					return pWindow->bMonospace ? PitchFixed : PitchProportional;

				default:
					ASSERT(FALSE);
			}

		case SelAll:
			while (pWindow = itWin.GetLinear ())
			{
				if (bFirstTime)
				{
					if (pWindow->pLogFont)
					{
						lf = *pWindow->pLogFont;
						bFirstTime = FALSE;
						switch (act)
						{
							case FontEquality:
								result = SameFont;
								break;

							case FontPitch:
								result = pWindow->bMonospace ? PitchFixed : PitchProportional;
								break;

							default:
								ASSERT (FALSE);
						}
					}
					else
						;
				}
				else
				switch (act)
				{
					case FontEquality:
						if (	pWindow->pLogFont &&
							(	lf.lfHeight != pWindow->pLogFont->lfHeight
							||	_tcscmp (lf.lfFaceName, pWindow->pLogFont->lfFaceName)))
							return FontsDiffer;
						break;

					case FontPitch:
						if (	pWindow->pLogFont &&
							(	((result == PitchFixed) && !pWindow->bMonospace)
							||	((result == PitchProportional) && pWindow->bMonospace)))
							return PitchMixed;
						break;

					default:
						ASSERT (FALSE);
				}
			}
			break;

		case SelGroup:
			itWin.CFmtGroupIterator::Set(m_nFmtInfoCur, m_nWinGroupCur);
			while (pWindow = itWin.Get())
			{
				if (bFirstTime)
				{
					if (pWindow->pLogFont)
					{
						lf = *pWindow->pLogFont;
						bFirstTime = FALSE;
						switch (act)
						{
							case FontEquality:
								result = SameFont;
								break;

							case FontPitch:
								result = pWindow->bMonospace ? PitchFixed : PitchProportional;
								break;

							default:
								ASSERT (FALSE);
						}
					}
					else
						;
				}
				else
				switch (act)
				{
					case FontEquality:
						if (	pWindow->pLogFont &&
							(	lf.lfHeight != pWindow->pLogFont->lfHeight
							||	_tcscmp (lf.lfFaceName, pWindow->pLogFont->lfFaceName)))
							return FontsDiffer;
						break;

					case FontPitch:
						if (	pWindow->pLogFont &&
							(	((result == PitchFixed) && !pWindow->bMonospace)
							||	((result == PitchProportional) && pWindow->bMonospace)))
							return PitchMixed;
						break;

					default:
						ASSERT (FALSE);
				}
			}
			break;
	}

	return result;
}


///// RestoreDefaults
//
///
void CFCDialogState::RestoreDefaults ()
{
CFormatInfo fiDefault;
CFmtIterator itFmt(m_rgFmtInfo);
CFmtGroupIterator itGroup(m_rgFmtInfo);
CFmtWindowIterator itWin(m_rgFmtInfo);

	switch (m_state)
	{
		case SelAll:
			// Loop through all CFormatInfo's
			CFormatInfo * pFI;

			while (pFI = itFmt.Get ())
			{	
				pFI->m_pPackage->GetDefaultFormatInfo (fiDefault);
				pFI->Update (fiDefault);
			}		
			UpdateAllAutoColors ();
			break;

		case SelGroup:

			// Find group, update 
			((CFormatInfo *)(m_rgFmtInfo[m_nFmtInfoCur]))->m_pPackage->GetDefaultFormatInfo (fiDefault);
			((CFormatInfo *)(m_rgFmtInfo[m_nFmtInfoCur]))->Update (fiDefault, m_nWinGroupCur);
			break;

		case SelWindow:
			// Find window, update 
			((CFormatInfo *)(m_rgFmtInfo[m_nFmtInfoCur]))->m_pPackage->GetDefaultFormatInfo (fiDefault);
			((CFormatInfo *)(m_rgFmtInfo[m_nFmtInfoCur]))->Update (fiDefault, m_nWinGroupCur, m_nWindowCur);
			break;
	}

	UpdateAllAutoColors ();
	m_pDialog->OnChangeWindow();	// This will refresh the dialog controls.
}


/////
//
///
FMT_WINDOW * CFCDialogState::GetWinCur (BOOL bReturnNull, BOOL bMustHaveFont) const
{
UINT iWindow = m_nWindowCur;
FMT_WINDOW * pWindow;

// FUTURE: What if !bReturnNull and there are no windows at all?
//
	pWindow =  ((iWindow != -1) || (!bReturnNull && !(iWindow = 0)))
		? &((CFormatInfo *)m_rgFmtInfo[m_nFmtInfoCur])->m_pWinGroup[m_nWinGroupCur].rgWindows[iWindow]
		: NULL;

	if (pWindow && bMustHaveFont && pWindow->szFontFace == NULL)
	{
		if (bReturnNull)
			pWindow = NULL;
		else
		{
			CFmtWindowIterator itWin (m_rgFmtInfo);

			itWin.Set (pWindow);
			while ( pWindow = itWin.GetLinear() )
			{
				if (pWindow->szFontFace)
					break;
			}

			ASSERT (pWindow);	// Can't return NULL, must return NULL, oops.
		}
	}

	return pWindow;
}


/////
//
///
PLOGFONT CFCDialogState::GetCurrentLogFont () const
{
	if (SameFont == WindowWalk (FontEquality))
		return GetWinCur(FALSE, TRUE)->pLogFont;
	else
		return NULL;
}


/////
//
///
void CFCDialogState::Commit()
{
	for (int i = 0; i < m_rgFmtInfo.GetSize (); i++)
		((CFormatInfo *)m_rgFmtInfo [i])->Commit ();

	m_state = PostDialog;
}
