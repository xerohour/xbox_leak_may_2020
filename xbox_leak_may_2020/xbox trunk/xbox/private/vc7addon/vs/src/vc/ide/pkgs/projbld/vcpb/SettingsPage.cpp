// SettingsPage.cpp : Implementation of CSettingsPage
#include "stdafx.h"
#include "SettingsPage.h"

// static helper functions
//

static void ReplaceSeparatorsWithNewlines( BSTR in, CComBSTR &out, CStringW& strPreferredSeparator )
{
	CStringW temp = in;
	int idx = 0;
	int nLen = temp.GetLength();
	bool bInSingleQuotes = false, bInDoubleQuotes = false;
	wchar_t cPreferred = strPreferredSeparator.IsEmpty() ? L';' : strPreferredSeparator[0];
	while (idx < nLen)
	{
		if (temp[idx] == L'"')
			bInDoubleQuotes = !bInDoubleQuotes;
		else if (temp[idx] == L'\'')
			bInSingleQuotes = !bInSingleQuotes;

		if (bInDoubleQuotes || bInSingleQuotes)
		{
			idx++;
			continue;	// don't want to do any replacements inside quotes
		}

		if (temp[idx] == cPreferred)
		{
			temp.Delete(idx, 1);
			temp.Insert(idx, L"\r\n");
			nLen++;		// just got a little bigger
			idx++;	// skip over the extra character we added
		}
		idx++;
	}

	out = temp;
	return;
}

static void ReplaceNewlinesWithSeparators( BSTR in, CComBSTR &out, CStringW& strPreferredSeparator )
{
	CStringW temp = in;
	int idx = 0;
	int nMax = temp.GetLength();
	nMax--;		// don't care about very last character
	bool bInSingleQuotes = false, bInDoubleQuotes = false;
	wchar_t cPreferred = strPreferredSeparator.IsEmpty() ? L';' : strPreferredSeparator[0];
	while (idx < nMax)
	{
		if (temp[idx] == L'"')
			bInDoubleQuotes = !bInDoubleQuotes;
		else if (temp[idx] == L'\'')
			bInSingleQuotes = !bInSingleQuotes;

		if (bInDoubleQuotes || bInSingleQuotes)
		{
			idx++;
			continue;	// don't want to do any replacements inside quotes
		}

		if (temp[idx] == L'\r' && temp[idx+1] == L'\n')
		{
			temp.Delete(idx, 2);
			temp.Insert(idx, cPreferred);
			nMax--;	// just lost a character
		}
		idx++;
	}

	out = temp;
	return;
}

/////////////////////////////////////////////////////////////////////////////////
// CMultiLineDlg
//

void CMultiLineDlg::DoChildOnSize()
{
	double dTotalUnits;
	double dWindowUnits;
	DoSharedChildOnSize(dTotalUnits, dWindowUnits);
	
	// list view
	double heightIncrement = (7.8/14.3)*dWindowUnits;
	SizeSubWindow(IDC_MULTILINELIST, EDITSTRINGS_RESIZE_EDGE, EDITSTRINGS_RESIZE_EDGE, EDITSTRINGS_RESIZE_EDGE, heightIncrement);
}

void CMultiLineDlg::DoChildInit()
{
	if (!m_hFont)
		return;

	DoSetWindowFont(IDC_MULTILINELIST);
	DoSharedChildInit();
}

void CMultiLineDlg::Reset()
{
	CComBSTR bstrFormatted;
	DoSharedReset(bstrFormatted);

 	CWindow editProp( GetDlgItem( IDC_MULTILINELIST ) );
 	::SetWindowTextW( editProp, bstrFormatted );
}

void CMultiLineDlg::RetrieveSpecificInfo(CComBSTR& bstrFixedData)
{
	CComBSTR bstrListData;
	RetrieveDynamicInfo(IDC_MULTILINELIST, bstrListData);
 	ReplaceNewlinesWithSeparators( bstrListData, bstrFixedData, m_strPreferredSeparator );
}

void CMultiDirectoryPickerEditorDlg::DoChildOnSize()
{
	double dTotalUnits;
	double dWindowUnits;
	DoSharedChildOnSize(dTotalUnits, dWindowUnits);
	
	// directory picker
	double heightIncrement = (7.8/14.3)*dWindowUnits;
	SizeSubWindow(IDC_DIRCONTROL, EDITSTRINGS_RESIZE_EDGE, EDITSTRINGS_RESIZE_EDGE, EDITSTRINGS_RESIZE_EDGE, heightIncrement);
}

void CMultiDirectoryPickerEditorDlg::DoChildInit()
{
	// this QS is being done in order to get the directory picker SetSite'd properly
	CComPtr<IUnknown> pUnk;
	ExternalQueryService(SID_SDirList, IID_IUnknown, (void **)&pUnk);

	RECT rectChild = { 0, 26, 263, 151 };
	m_wndDirList.Create(m_hWnd, rectChild, L"{AFD20A96-EED8-4D8C-A4EA-18AB96F6C432}",
		WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN, 0, IDC_DIRCONTROL);
	m_wndDirList.QueryControl(&m_pList);
	CComQIPtr<IDirListSite> pListSite = m_pList;
	if (pListSite)
		pListSite->SetSite((IUnknown *)CVCProjectEngine::s_pServiceProvider);
	if (m_pList)
	{
		m_pList->put_ShowTitle(VARIANT_FALSE);
		m_pList->put_ShowCheck(VARIANT_FALSE);
		m_pList->put_Description(L"   ");
	}

	DoSharedChildInit();
}

void CMultiDirectoryPickerEditorDlg::Reset()
{
	CComBSTR bstrFormatted;
	DoSharedReset(bstrFormatted);

	if( m_pList )
	{
		m_pList->Reset();
		CStringW temp = bstrFormatted;
		int loc = temp.Find(L"\r\n");
		while( -1 != loc )
		{
			CComBSTR bstr = temp.Left( loc );
			temp = temp.Right( temp.GetLength() - loc - 2);
			m_pList->Add(bstr);
			loc = temp.Find(L"\r\n");
		}
		if (!temp.IsEmpty())	// pick up if there is one more on the end...
		{
			CComBSTR bstr = temp;
			m_pList->Add(bstr);
		}
	}
}

void CMultiDirectoryPickerEditorDlg::RetrieveSpecificInfo(CComBSTR& bstrFixedData)
{
	bstrFixedData.Empty();
	if (!m_pList)
		return;

	if( m_pList )
	{
		CComBSTR bstrSeparator = (m_strPreferredSeparator.IsEmpty() ? L";" : m_strPreferredSeparator);
		//
		long i;
		long j;
		m_pList->get_Count(&j);
		for( i=0; i< j; i++)
		{
			CComBSTR bstrItem;
			m_pList->Index(i, &bstrItem);
			if (bstrItem.Length() > 0)
			{
				bstrFixedData += bstrItem;
				bstrFixedData += bstrSeparator;
			}
		}

		CleanUpString(bstrFixedData, m_strPreferredSeparator, m_strAllSeparators);
	}
}

/////////////////////////////////////////////////////////////////////////////////
// CSingleDirectoryPickerEditorDlg
//

void CSingleDirectoryPickerEditorDlg::DoChildInit()
{
	// this QS is being done in order to get the directory picker SetSite'd properly
	CComPtr<IUnknown> pUnk;
	ExternalQueryService(SID_SDirList, IID_IUnknown, (void **)&pUnk);

	RECT rectChild = { 0, 26, 263, 151 };
	m_wndDirList.Create(m_hWnd, rectChild, L"{AFD20A96-EED8-4D8C-A4EA-18AB96F6C432}",
		WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN, 0, IDC_DIRCONTROL);
	m_wndDirList.QueryControl(&m_pList);
	CComQIPtr<IDirListSite> pListSite = m_pList;
	if (pListSite)
		pListSite->SetSite((IUnknown *)CVCProjectEngine::s_pServiceProvider);
	if (m_pList)
	{
		m_pList->put_ShowTitle(VARIANT_FALSE);
		m_pList->put_ShowCheck(VARIANT_FALSE);
		m_pList->put_Description(L"   ");
	}
}

void CSingleDirectoryPickerEditorDlg::DoChildOnSize()
{
	double dHeight = (double)(m_nExpansionHeight - m_yBorder)/(m_unit.cy ? m_unit.cy : 1.0);
	dHeight -= 2*EDITSTRINGS_RESIZE_EDGE;	// take out the top and bottom edge spacing
	
	// directory picker
	SizeSubWindow(IDC_DIRCONTROL, EDITSTRINGS_RESIZE_EDGE, EDITSTRINGS_RESIZE_EDGE, EDITSTRINGS_RESIZE_EDGE, dHeight);
}

void CSingleDirectoryPickerEditorDlg::Reset()
{
	CStringW strSeparator = L";";
	CleanUpString( m_bstrProperty, strSeparator, strSeparator );
	// replace separators with newlines
	CComBSTR bstrFormatted;
 	ReplaceSeparatorsWithNewlines( m_bstrProperty, bstrFormatted, strSeparator );

	if( m_pList )
	{
		m_pList->Reset();
		CStringW temp = bstrFormatted;
		int loc = temp.Find(L"\r\n");
		while( -1 != loc )
		{
			CComBSTR bstr = temp.Left( loc );
			temp = temp.Right( temp.GetLength() - loc - 2);
			m_pList->Add(bstr);
			loc = temp.Find(L"\r\n");
		}
		if (!temp.IsEmpty())	// pick up if there is one more on the end...
		{
			CComBSTR bstr = temp;
			m_pList->Add(bstr);
		}
	}
}

void CSingleDirectoryPickerEditorDlg::RetrieveInfo()
{
	m_bstrProperty.Empty();
	if (!m_pList)
		return;

	if( m_pList )
	{
		CComBSTR bstrSeparator = L";";
		CStringW strSeparator = L";";
		//
		long i;
		long j;
		m_pList->get_Count(&j);
		for( i=0; i< j; i++)
		{
			CComBSTR bstrItem;
			m_pList->Index(i, &bstrItem);
			if (bstrItem.Length() > 0)
			{
				m_bstrProperty += bstrItem;
				m_bstrProperty += bstrSeparator;
			}
		}

		CleanUpString(m_bstrProperty, strSeparator, strSeparator);
	}
}

/////////////////////////////////////////////////////////////////////////////////
// CSingleLineNoReturnsDlg
//

void CSingleLineNoReturnsDlg::Reset()
{
 	CWindow editProp( GetDlgItem( IDC_STRING_EDIT ) );
 	::SetWindowTextW( editProp, m_bstrProperty );
}

void CSingleLineNoReturnsDlg::RetrieveInfo()
{
	// property edit box
	RetrieveDynamicInfo(IDC_STRING_EDIT, m_bstrProperty);
}

LRESULT CSingleLineNoReturnsDlg::DoOnMinMaxInfo( LPMINMAXINFO lpMinMax )
{
	RECT rectWorkArea;
	SystemParametersInfo(SPI_GETWORKAREA, 0, &rectWorkArea, 0);
	lpMinMax->ptMinTrackSize.x = (long)(43.5*m_unit.cx + 2*m_xBorder); // min track width
	if (lpMinMax->ptMinTrackSize.x > rectWorkArea.right - rectWorkArea.left)
		lpMinMax->ptMinTrackSize.x = rectWorkArea.right - rectWorkArea.left;
	if (m_bExpanded)	// min track height
		lpMinMax->ptMinTrackSize.y = (long)(31*m_unit.cy + 2*m_yBorder + m_yCaptionSize);
	else
		lpMinMax->ptMinTrackSize.y = (long)(9*m_unit.cy + 2*m_yBorder + m_yCaptionSize);
	if (lpMinMax->ptMinTrackSize.y > rectWorkArea.bottom - rectWorkArea.top)
		lpMinMax->ptMinTrackSize.y = rectWorkArea.bottom - rectWorkArea.top;
	return 0;
}

void CSingleLineNoReturnsDlg::DoChildOnSize()
{
	SizeSubWindow(IDC_STRING_EDIT, EDITSTRINGS_RESIZE_EDGE, EDITSTRINGS_RESIZE_EDGE, EDITSTRINGS_RESIZE_EDGE, 3.0);
}

void CSingleLineNoReturnsDlg::ResetParentBottom(RECT& rect)
{
	rect.bottom = rect.top + m_yBorder + (long)(9.3*m_unit.cy);
}
/////////////////////////////////////////////////////////////////////////////////
// CSingleLineWithReturnsDlg
//

void CSingleLineWithReturnsDlg::Reset()
{
	CleanUpString( m_bstrProperty, m_strSeparator, m_strSeparator );
	// replace separators with newlines
	CComBSTR bstrFormatted;
 	ReplaceSeparatorsWithNewlines( m_bstrProperty, bstrFormatted, m_strSeparator );

 	CWindow editProp( GetDlgItem( IDC_STRING_EDIT ) );
 	::SetWindowTextW( editProp, bstrFormatted );
}

void CSingleLineWithReturnsDlg::RetrieveInfo()
{
	// property edit box
	CComBSTR bstrListData;
	RetrieveDynamicInfo(IDC_STRING_EDIT, bstrListData);
 	ReplaceNewlinesWithSeparators( bstrListData, m_bstrProperty, m_strSeparator );
}

void CSingleLineWithReturnsDlg::DoChildOnSize()
{
	double dHeight = (double)(m_nExpansionHeight - m_yBorder)/(m_unit.cy ? m_unit.cy : 1.0);
	dHeight -= 2*EDITSTRINGS_RESIZE_EDGE;	// take out the top and bottom edge spacing
	SizeSubWindow(IDC_STRING_EDIT, EDITSTRINGS_RESIZE_EDGE, EDITSTRINGS_RESIZE_EDGE, EDITSTRINGS_RESIZE_EDGE, 
		dHeight);
}

/////////////////////////////////////////////////////////////////////////////////
// CCommandsDlg
//

LRESULT CCommandsDlg::OnInitDialog( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled )
{
	return CBaseEditStringDlg<CCommandsDlg>::OnInitDialog(uMsg, wParam, lParam, bHandled);
}

LRESULT CCommandsDlg::OnSize( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled )
{
	return CBaseEditStringDlg<CCommandsDlg>::DoOnSize(uMsg, wParam, lParam, bHandled);
}

LRESULT CCommandsDlg::OnPaint( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled )
{
	return CBaseEditStringDlg<CCommandsDlg>::DoOnPaint();
}

LRESULT CCommandsDlg::OnMinMaxInfo( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled )
{
	return CBaseEditStringDlg<CCommandsDlg>::DoOnMinMaxInfo((LPMINMAXINFO)lParam);
}

void CCommandsDlg::DoChildOnSize()
{
	double dHeight = (double)(m_nExpansionHeight - m_yBorder)/(m_unit.cy ? m_unit.cy : 1.0);
	dHeight -= 2*EDITSTRINGS_RESIZE_EDGE;	// take out the top and bottom edge spacing
	SizeSubWindow(IDC_COMMANDS_EDIT, EDITSTRINGS_RESIZE_EDGE, EDITSTRINGS_RESIZE_EDGE, EDITSTRINGS_RESIZE_EDGE, 
		dHeight);
}

void CCommandsDlg::DoChildInit()
{
	if (m_hFont)
		DoSetWindowFont(IDC_COMMANDS_EDIT);
}

LRESULT CCommandsDlg::OnSetFocus( WORD, UINT, HWND, BOOL& )
{
	return CBaseEditStringDlg<CCommandsDlg>::DoOnSetFocus();
}

void CCommandsDlg::Reset()
{
 	CWindow editProp( GetDlgItem( IDC_COMMANDS_EDIT ) );
 	::SetWindowTextW( editProp, m_bstrProperty );
}

void CCommandsDlg::RetrieveInfo()
{
	// property edit box
	RetrieveDynamicInfo(IDC_COMMANDS_EDIT, m_bstrProperty);
}

LRESULT CCommandsDlg::OnOk( WORD, UINT, HWND, BOOL& )
{
	return CBaseEditStringDlg<CCommandsDlg>::DoOnOk();
}

LRESULT CCommandsDlg::OnCancel( WORD, UINT, HWND, BOOL& )
{
	return CBaseEditStringDlg<CCommandsDlg>::DoOnCancel();
}

LRESULT CCommandsDlg::OnHelp( WORD, UINT, HWND, BOOL& )
{
	return CBaseEditStringDlg<CCommandsDlg>::DoOnHelp();
}

LRESULT CCommandsDlg::OnMacros( WORD, UINT, HWND, BOOL& )
{
	return CBaseEditStringDlg<CCommandsDlg>::DoOnMacros();
}

LRESULT CCommandsDlg::OnInsertMacro( WORD, UINT, HWND, BOOL& )
{
	return CBaseEditStringDlg<CCommandsDlg>::DoOnInsertMacro();
}

/////////////////////////////////////////////////////////////////////////////////
// CMultiLineNoInheritDlg
//

LRESULT CMultiLineNoInheritDlg::OnInitDialog( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled )
{
	return CBaseEditStringDlg<CMultiLineNoInheritDlg>::OnInitDialog(uMsg, wParam, lParam, bHandled);
}

LRESULT CMultiLineNoInheritDlg::OnSize( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled )
{
	return CBaseEditStringDlg<CMultiLineNoInheritDlg>::DoOnSize(uMsg, wParam, lParam, bHandled);
}

LRESULT CMultiLineNoInheritDlg::OnPaint( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled )
{
	return CBaseEditStringDlg<CMultiLineNoInheritDlg>::DoOnPaint();
}

LRESULT CMultiLineNoInheritDlg::OnMinMaxInfo( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled )
{
	return CBaseEditStringDlg<CMultiLineNoInheritDlg>::DoOnMinMaxInfo((LPMINMAXINFO)lParam);
}

void CMultiLineNoInheritDlg::DoChildOnSize()
{
	double dHeight = (double)(m_nExpansionHeight - m_yBorder)/(m_unit.cy ? m_unit.cy : 1.0);
	dHeight -= 2*EDITSTRINGS_RESIZE_EDGE;	// take out the top and bottom edge spacing
	SizeSubWindow(IDC_MULTILINELIST, EDITSTRINGS_RESIZE_EDGE, EDITSTRINGS_RESIZE_EDGE, EDITSTRINGS_RESIZE_EDGE, 
		dHeight);
}

void CMultiLineNoInheritDlg::DoChildInit()
{
	if (m_hFont)
		DoSetWindowFont(IDC_MULTILINELIST);
}

LRESULT CMultiLineNoInheritDlg::OnSetFocus( WORD, UINT, HWND, BOOL& )
{
	return CBaseEditStringDlg<CMultiLineNoInheritDlg>::DoOnSetFocus();
}

void CMultiLineNoInheritDlg::Reset()
{
	CleanUpString( m_bstrProperty, m_strPreferredSeparator, m_strAllSeparators );
	// replace separators with newlines
	CComBSTR bstrFormatted;
 	ReplaceSeparatorsWithNewlines( m_bstrProperty, bstrFormatted, m_strPreferredSeparator );

  	CWindow editProp( GetDlgItem( IDC_MULTILINELIST ) );
	::SetWindowTextW( editProp, bstrFormatted );
}


void CMultiLineNoInheritDlg::RetrieveInfo()
{
	// property edit box
	CComBSTR bstrListData;
	RetrieveDynamicInfo(IDC_MULTILINELIST, bstrListData);
 	ReplaceNewlinesWithSeparators( bstrListData, m_bstrProperty, m_strPreferredSeparator );
}

LRESULT CMultiLineNoInheritDlg::OnOk( WORD, UINT, HWND, BOOL& )
{
	return CBaseEditStringDlg<CMultiLineNoInheritDlg>::DoOnOk();
}

LRESULT CMultiLineNoInheritDlg::OnCancel( WORD, UINT, HWND, BOOL& )
{
	return CBaseEditStringDlg<CMultiLineNoInheritDlg>::DoOnCancel();
}

LRESULT CMultiLineNoInheritDlg::OnHelp( WORD, UINT, HWND, BOOL& )
{
	return CBaseEditStringDlg<CMultiLineNoInheritDlg>::DoOnHelp();
}

LRESULT CMultiLineNoInheritDlg::OnMacros( WORD, UINT, HWND, BOOL& )
{
	return CBaseEditStringDlg<CMultiLineNoInheritDlg>::DoOnMacros();
}

LRESULT CMultiLineNoInheritDlg::OnInsertMacro( WORD, UINT, HWND, BOOL& )
{
	return CBaseEditStringDlg<CMultiLineNoInheritDlg>::DoOnInsertMacro();
}
