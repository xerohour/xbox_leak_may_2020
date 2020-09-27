#include "stdafx.h"
#include "mfcappwz.h"
#include "platname.h"

#ifndef VS_PACKAGE
#include <bldapi.h>
#include <bldguid.h>
#endif	// VS_PACKAGE

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

void CPlatNameConvert::Init()
{
#ifndef VS_PACKAGE
	m_NameMap.RemoveAll();
	LPBUILDPLATFORMS pPlatforms;
	VERIFY(SUCCEEDED(theApp.FindInterface(IID_IBuildPlatforms, (LPVOID FAR *)&pPlatforms)));
	const CStringList* pPlatformsUI;
	pPlatforms->GetAvailablePlatforms(&pPlatformsUI, NULL, TRUE);
	const CStringList* pPlatformsLong;
	pPlatforms->GetAvailablePlatforms(&pPlatformsLong, NULL, FALSE);

	POSITION posUI = pPlatformsUI->GetHeadPosition();
	POSITION posLong = pPlatformsLong->GetHeadPosition();
	while (posUI != NULL && posLong != NULL)
	{
		m_NameMap[pPlatformsUI->GetNext(posUI)]
			= pPlatformsLong->GetNext(posLong);
	}
	pPlatforms->Release();
#endif	// VS_PACKAGE
}

// Looks the UI name up in the map.  If a long name is assigned to  it, it's
//  returned in strLong; otherwise, strUI is returned in strLong.
void CPlatNameConvert::UItoLong(const CString& strUI, CString& strLong)
{
	if (!m_NameMap.Lookup(strUI, strLong))
		strLong = strUI;
}
	
// Here, we do a linear search for values that match Long, and return the
//  corresponding key in strUI; if not there, just return strLong in strUI.
//  We choose a linear search rather than keeping two maps simply because
//  this map is typically only going to have 2 or 3 elements in it.  In most
//  cases, just 1 (if the user doesn't have an add-on).
void CPlatNameConvert::LongToUI(const CString& strLong, CString& strUI)
{
	POSITION pos = m_NameMap.GetStartPosition();
	while (pos != NULL)
	{
		CString strValue;
		m_NameMap.GetNextAssoc(pos, strUI, strValue);
		if (strValue == strLong)
		{
			// We've found a match, and the value is already in strUI
			return;
		}
	}

	// If we got to this point, there is no match, so just set strUI to strLong.
	strUI = strLong;
}
