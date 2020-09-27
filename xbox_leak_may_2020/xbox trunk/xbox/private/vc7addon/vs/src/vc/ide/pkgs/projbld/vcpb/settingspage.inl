// This file contains the method definitions for the template classes in SettingsPage.h

////////////////////////////////////////////////////////////////////////////////////////////////
// CSettingsPageBase

CSettingsPageBaseTemplate inline 
CSettingsPageBaseType::CSettingsPageBase() 
{
	m_dwTitleID = TTitleID;
	m_dwHelpFileID = IDS_VCPROJECT; // dummy ID to get the help method invoked
	m_bActive = FALSE;
	m_nLastObjects = 0;
	m_ppUnkArray = NULL;
}

CSettingsPageBaseTemplate inline 
CSettingsPageBaseType::~CSettingsPageBase() 
{
	if(m_ppUnkArray == NULL)
		return;

	UINT i;
	for (i = 0; i < m_nLastObjects; i++)
	{
		if( m_ppUnkArray[i] != NULL )
		{
			CComQIPtr<IVCStagedPropertyContainer> spStagedContainer = m_ppUnkArray[i];
			if (spStagedContainer)
				spStagedContainer->ReleaseStagedContainer();
			m_ppUnkArray[i]->Release();
		}
	}
	delete [] m_ppUnkArray;
	m_ppUnkArray = NULL;
}

CSettingsPageBaseTemplate inline 
IVSMDPropertyGrid* CSettingsPageBaseType::GetGrid()
{
	if( s_pGrid == NULL )
	{
		CComQIPtr<IVCProjectEngineImpl> spProjEngineImpl = g_pProjectEngine;
		VSASSERT(spProjEngineImpl != NULL, "Project engine isn't what we expect or is not initialized.  Deep, deep trouble.");
		RETURN_ON_NULL2(spProjEngineImpl, NULL);
		HRESULT hr = spProjEngineImpl->CreatePropertyGrid(&s_pGrid);
	}
	return s_pGrid;
}

CSettingsPageBaseTemplate inline 
STDMETHODIMP CSettingsPageBaseType::OnShowPage(BOOL fActivated)
{
	RETURN_ON_NULL2(fActivated, S_OK);
	RETURN_ON_NULL(GetGrid());
	s_pLastActive = this;

	HWND hwndGrid;
	RECT rectGrid;
	
	// get the font to use for dlgs
	CComQIPtr<IVCProjectEngineImpl> pProjEngImpl = g_pProjectEngine;
	RETURN_ON_NULL2(pProjEngImpl, E_NOINTERFACE);
	HFONT hFont;
	pProjEngImpl->GetDialogFont( FALSE, &hFont );
	SetFont( hFont );
	HDC dc = GetDC();
	SelectObject( dc, hFont );
	// get a base "unit" for sizing the dlg
	SIZE unit;
	GetTextExtentPoint32W( dc, L"X", 1, &unit );
	ReleaseDC(dc);
	DeleteObject( hFont );

	rectGrid.top = 0;
	rectGrid.left = 0;
	rectGrid.bottom = m_size.cy >= unit.cy * 24 ? m_size.cy : unit.cy * 24;
	rectGrid.right = m_size.cx >= unit.cx * 67 ? m_size.cx : unit.cx * 67;
	
	SetWindowPos( HWND_TOP, &rectGrid, SWP_FRAMECHANGED );

	GetGrid()->get_Handle(&hwndGrid);
	CWindow wndGrid;
	wndGrid.Attach(hwndGrid);
	wndGrid.SetParent(m_hWnd);

	VARIANT vt;
	VariantInit(&vt);
	V_VT(&vt) = VT_BOOL;
	V_BOOL(&vt) = VARIANT_FALSE;
	GetGrid()->SetOption(PGOPT_TOOLBAR, vt);
	wndGrid.SetWindowPos( HWND_TOP, &rectGrid, SWP_FRAMECHANGED );

	UpdateObjects();
	return S_OK;
}

CSettingsPageBaseTemplate inline 
STDMETHODIMP CSettingsPageBaseType::get_CategoryTitle( UINT iLevel, BSTR *pbstrCategory )
{
	if( (iLevel == 0) && TGroupID )
	{
		CComBSTR bstrTitle;
		bstrTitle.LoadString(TGroupID);
		*pbstrCategory = bstrTitle.Detach();
		return S_OK;
	}
	return S_FALSE;
}

CSettingsPageBaseTemplate inline 
STDMETHODIMP CSettingsPageBaseType::Apply(void)
{
	if (m_ppUnkArray)
	{
		UINT i;
		for (i = 0; i < m_nObjects; i++)
		{
			if( m_ppUnkArray && (m_ppUnkArray[i] != NULL) )
			{
				CComQIPtr<IVCPropertyContainer> pContainer = m_ppUnkArray[i];
				RETURN_ON_NULL(pContainer);
				HRESULT hr = pContainer->Commit();
				RETURN_ON_FAIL(hr);
			}
		}
	}
	SetDirty(FALSE);
	if (GetGrid())
		GetGrid()->Refresh();

	return S_OK;
}

CSettingsPageBaseTemplate inline 
STDMETHODIMP CSettingsPageBaseType::Help(LPCOLESTR pszHelpDir)
{
	CComQIPtr<IVCProjectEngineImpl> pProjEngineImpl = g_pProjectEngine;
	RETURN_ON_NULL(pProjEngineImpl);
	RETURN_ON_NULL(m_ppUnkArray);
	RETURN_ON_NULL(m_ppUnkArray[0]);

	CComQIPtr<IDispatch> pdisp = m_ppUnkArray[0];
	RETURN_ON_NULL(pdisp);

	CComPtr<ITypeInfo> pTypeInfo;
	HRESULT hr = pdisp->GetTypeInfo(0, LANG_NEUTRAL, &pTypeInfo);
	RETURN_ON_FAIL_OR_NULL(hr, pTypeInfo);

	CComBSTR bstrIFace;
	hr = pTypeInfo->GetDocumentation(-1, NULL, &bstrIFace, NULL, NULL);
	RETURN_ON_FAIL(hr);
	
	if( bstrIFace.Length() == 0 )
		return E_FAIL;

	CComBSTR bstrTopic;
	bstrTopic = L"VC.Project.";
	bstrTopic += bstrIFace;
	bstrTopic += L".";
	CComBSTR bstrProp;
	if( GetGrid() )
		GetGrid()->get_SelectedPropertyName(&bstrProp);
	bstrTopic += bstrProp;

	OpenTopic(pProjEngineImpl, bstrTopic);
	return S_OK;
}

CSettingsPageBaseTemplate inline 
STDMETHODIMP CSettingsPageBaseType::Deactivate(void)
{
	s_cGrids--;
	if (s_cGrids <= 0 && s_pGrid)
	{
		s_pGrid = NULL;
		s_cGrids = 0;
		s_pLastActive = NULL;
	}

	// Delete the page objects
	if(m_ppUnkArray)
	{
		UINT i;
		for (i = 0; i < m_nLastObjects; i++)
		{
			if( m_ppUnkArray[i] != NULL )
			{
				CComQIPtr<IVCPropertyContainer> spContainer = m_ppUnkArray[i];
				if (spContainer)
					spContainer->ReleaseStagedPropertyContainer();
				CComQIPtr<IVCPropertyPageObject> spPO = m_ppUnkArray[i];
				if( spPO )
					spPO->Close();
				m_ppUnkArray[i]->Release();
			}
		}
		delete [] m_ppUnkArray;
		m_ppUnkArray = NULL;
		m_nLastObjects = 0;
	}
	
	HRESULT hr = IPropertyPageImpl<T>::Deactivate();
	m_bActive = FALSE;
	return hr;
}

CSettingsPageBaseTemplate inline 
STDMETHODIMP CSettingsPageBaseType::Activate(HWND hWndParent, LPCRECT pRect, BOOL /* bModal */)
{
	ATLTRACE2(atlTraceControls,2,_T("IPropertyPageImpl::Activate\n"));

	if (pRect == NULL)
	{
		ATLTRACE2(atlTraceControls,2,_T("Error : Passed a NULL rect\n"));
		return E_POINTER;
	}
	m_hWnd = Create(hWndParent);
	s_cGrids++;

	if( m_nObjects == 0 )
		return S_OK;

	m_size.cx = pRect->right - pRect->left;
	m_size.cy = pRect->bottom - pRect->top;

	m_bActive = TRUE;

	return S_OK;
}

CSettingsPageBaseTemplate inline 
STDMETHODIMP CSettingsPageBaseType::GetPageInfo(PROPPAGEINFO* pPageInfo)
{
	// call through to atl parent version
	HRESULT hr = IPropertyPageImpl<T>::GetPageInfo( pPageInfo );

	// get the font to use for dlgs
	CComQIPtr<IVCProjectEngineImpl> pProjEngImpl = g_pProjectEngine;
	RETURN_ON_NULL2(pProjEngImpl, E_NOINTERFACE);
	HDC dc;
	dc = ::CreateDCW( L"DisPLAY", NULL, NULL, NULL );
	HFONT hFont;
	pProjEngImpl->GetDialogFont( FALSE, &hFont );
	SelectObject( dc, hFont );
	// get a base "unit" for sizing the dlg
	SIZE unit;
	GetTextExtentPoint32W( dc, L"X", 1, &unit );
	DeleteObject( hFont );
	DeleteDC( dc );

	pPageInfo->size.cx = m_size.cx >= unit.cx * 67 ? m_size.cx : unit.cx * 67;
	pPageInfo->size.cy = m_size.cy >= unit.cy * 24 ? m_size.cy : unit.cy * 24;

	return hr;
}

CSettingsPageBaseTemplate inline 
STDMETHODIMP CSettingsPageBaseType::SetObjects(ULONG nObjects, IUnknown **ppUnk)
{
	HRESULT hr;

	// call base-class implementation
	hr = IPropertyPageImpl<T>::SetObjects( nObjects, ppUnk );
	RETURN_ON_FAIL(hr);

	// update the objects if we have ever been activated.
	if( m_bActive )
		hr = UpdateObjects();
		
	return hr;
}

CSettingsPageBaseTemplate inline 
STDMETHODIMP CSettingsPageBaseType::TranslateAccelerator(MSG *pMsg)
{
	if ((pMsg->message < WM_KEYFIRST || pMsg->message > WM_KEYLAST) &&
		(pMsg->message < WM_MOUSEFIRST || pMsg->message > WM_MOUSELAST))
		return S_FALSE;

	return (::IsDialogMessage(m_hWnd, pMsg)) ? S_OK : S_FALSE;
}

CSettingsPageBaseTemplate 
inline STDMETHODIMP CSettingsPageBaseType::Dirty( void )
{
	SetDirty(TRUE);
	return S_OK;
}

CSettingsPageBaseTemplate inline 
STDMETHODIMP CSettingsPageBaseType::get_NumObjects( int* pNumObjects )
{
	*pNumObjects = m_nLastObjects;
	return S_OK;
}

CSettingsPageBaseTemplate inline 
STDMETHODIMP CSettingsPageBaseType::GetObject( int index, IUnknown **ppUnkObject )
{
	if( (DWORD)index >= m_nLastObjects )
	{
		*ppUnkObject = NULL;
		RETURN_INVALID();
	}
	else if (m_ppUnkArray == NULL)
	{
		*ppUnkObject = NULL;
		return E_UNEXPECTED;
	}
	else
	{
		CComPtr<IUnknown> pRet = m_ppUnkArray[index];
		*ppUnkObject = pRet.Detach();
		return S_OK;
	}
}

CSettingsPageBaseTemplate inline 
STDMETHODIMP CSettingsPageBaseType::Refresh( void )
{
	if (GetGrid())
		GetGrid()->Refresh();
	return S_OK;
}

CSettingsPageBaseTemplate inline 
STDMETHODIMP CSettingsPageBaseType::UpdateObjects( void )
{
	// we want some overlap in refcounts here, so start off by saving off the old list
	IUnknown ** ppOldUnkArray = m_ppUnkArray;
	UINT nOldLastObjects = m_nLastObjects;

	// Make a new list
	m_ppUnkArray = new IUnknown*[ m_nObjects ];
	memset(m_ppUnkArray, 0, m_nObjects*sizeof(IUnknown*));
	m_nLastObjects = m_nObjects;

	UINT i;
	for (i = 0; i < m_nObjects; i++)
	{
		//QI for IVCCfg
		CComQIPtr<IVCCfg> pVcCfg = m_ppUnk[i];
		if (pVcCfg == NULL)
		{
			VSFAIL("Hey, bad config sent to CSettingsPage!");
			continue;
		}
		CComPtr<IDispatch> pDisp;
		HRESULT hr = pVcCfg->get_Object( &pDisp );
		
		CComPtr<IVCSettingsPage> pPage;
		QueryInterface(__uuidof(IVCSettingsPage), (void**)&pPage);

		CComQIPtr<IVCPropertyContainer> pContainer = pDisp;
		CComQIPtr<IVCPropertyContainer> spLiveContainer;
		if (pContainer)
		{
			CComQIPtr<IVCStagedPropertyContainer> spStagedContainer;
			pContainer->GetStagedPropertyContainer(VARIANT_TRUE, &spStagedContainer);
			if (spStagedContainer)
				spStagedContainer->HoldStagedContainer();
			spLiveContainer = spStagedContainer;
		}
		if (spLiveContainer == NULL)
			spLiveContainer = pContainer;

		DoCreatePageObject(i, pDisp, spLiveContainer, pPage);
	}

		// Get Rid of the old list
	if (ppOldUnkArray)
	{
		for (i = 0; i < nOldLastObjects; i++)
		{
			if( ppOldUnkArray[i] != NULL )
			{
				CComQIPtr<IVCPropertyContainer> spContainer = ppOldUnkArray[i];
				if (spContainer)
					spContainer->ReleaseStagedPropertyContainer();
				ppOldUnkArray[i]->Release();
			}
		}
		delete [] ppOldUnkArray;
	}

if (GetGrid() && s_pLastActive == this)
	{
		if(m_nObjects && m_ppUnkArray && m_ppUnkArray[0])
		{
			CComPtr<ICategorizeProperties> pCat;
			m_ppUnkArray[0]->QueryInterface( IID_ICategorizeProperties, (void **)&pCat );

			if( pCat )
				GetGrid()->put_GridSort(PGSORT_CATEGORIZED);
			else
				GetGrid()->put_GridSort(PGSORT_NOSORT);
		}
		GetGrid()->SetSelectedObjects(m_nObjects, m_ppUnkArray);
		GetGrid()->Refresh();
	}

	return S_OK;
}

CSettingsPageBaseTemplate inline 
void CSettingsPageBaseType::DoCreatePageObject(UINT i, IDispatch* pDisp, IVCPropertyContainer* pLiveContainer, IVCSettingsPage* pPage)
{
	HRESULT hr = S_OK;
#ifdef AARDVARK
	CComQIPtr<IVCStyleSheetImpl> pStyle = pDisp;
	if (pStyle)
	{
		hr = pStyle->CreatePageObject(&m_ppUnkArray[i], (CLSID*)Tpclsid, pLiveContainer, pPage);
		return;
	}
#endif	// AARDVARK
	// Ask the Config to create the page object
	CComQIPtr<IVCConfigurationImpl> pCfg = pDisp;
	if( pCfg )
	{
		hr = pCfg->CreatePageObject(&m_ppUnkArray[i], (CLSID*)Tpclsid, pLiveContainer, pPage);
	} else {
		CComQIPtr<IVCFileConfigurationImpl> pCfg = pDisp;
		if( pCfg )
			hr = pCfg->CreatePageObject(&m_ppUnkArray[i], (CLSID*)Tpclsid, pLiveContainer, pPage);
		else
		{
			VSASSERT(FALSE, "Wrong kind of object!");
			m_ppUnkArray[i] = NULL;
		}

	}
}

////////////////////////////////////////////////////////////////////////////////////////////////
// CPageObjectImpl

CPageObjectImplTemplate inline /* static */
HRESULT CPageObjectImplType::CreateInstance(IUnknown **pI, IVCPropertyContainer *pContainer, IVCSettingsPage *pPage)
{
	HRESULT hr;
	CComObject<T> *pObj;
	hr = CComObject<T>::CreateInstance(&pObj);
	if (SUCCEEDED(hr))
	{
		pObj->QueryInterface(__uuidof(IUnknown), (void**)pI);
		pObj->m_pContainer = pContainer;
		pObj->m_pPage = pPage;
		pObj->ResetParentIfNeeded();
	}
	return hr;
}

CPageObjectImplTemplate inline
STDMETHODIMP CPageObjectImplType::Close()		// turn this object into a ZOMBIE
{
	m_pContainer = NULL;
	m_pPage = NULL;
	m_PropMap.RemoveAll();
	return S_OK;
}

CPageObjectImplTemplate inline
STDMETHODIMP CPageObjectImplType::Commit()
{
	CComQIPtr<IVCStagedPropertyContainer> spStagedContainer = m_pContainer;
	if (spStagedContainer)
		return spStagedContainer->CommitPropertyRange(nMin, nMax);

	VCPOSITION iter = m_PropMap.GetStartPosition();
	// iterate over each item in the map
	HRESULT hr = S_OK;
	while( iter )
	{
		long key;
		CComVariant varVal;
		m_PropMap.GetNextAssoc( iter, key, varVal );
		if( (varVal.vt == VT_I4 && varVal.intVal == INHERIT_PROP_VALUE) || (varVal.vt == VT_I2 && varVal.iVal == INHERIT_PROP_VALUE) || (varVal.vt == VT_BSTR && wcscmp( L"-2", varVal.bstrVal ) == 0) || (varVal.vt == VT_BSTR && SysStringLen( varVal.bstrVal ) == 0) )
			hr = m_pContainer->Clear(key);
		else
			hr = m_pContainer->SetProp( key, varVal );
		RETURN_ON_FAIL(hr);
	}
	m_PropMap.RemoveAll();

	return S_OK;
}

// this helper is for anybody who manipulates settings outside its own ID range (like the config general page does)
CPageObjectImplTemplate inline
HRESULT CPageObjectImplType::CommitSingleProp(long id)
{
	CComQIPtr<IVCStagedPropertyContainer> spStagedContainer = m_pContainer;
	if (spStagedContainer)
		return spStagedContainer->CommitPropertyRange(id, id);
	return S_OK;
}

CPageObjectImplTemplate inline
STDMETHODIMP CPageObjectImplType::ClearAll()
{
	CComQIPtr<IVCStagedPropertyContainer> spStagedContainer = m_pContainer;
	if (spStagedContainer)
		return spStagedContainer->ClearPropertyRange(nMin, nMax);

	m_PropMap.RemoveAll();
	return S_OK;
}

CPageObjectImplTemplate inline
STDMETHODIMP CPageObjectImplType::GetLocalProp(long id, /*[out]*/ VARIANT *pvarValue)
{
	CComQIPtr<IVCStagedPropertyContainer> spStagedContainer = m_pContainer;
	if (spStagedContainer)
		return m_pContainer->GetLocalProp(id, pvarValue);

	CComVariant varVal;
	if( !m_PropMap.Lookup(id, varVal) )
		return S_FALSE;

	if( (varVal.vt == VT_I4 && varVal.intVal == INHERIT_PROP_VALUE) || (varVal.vt == VT_I2 && varVal.iVal == INHERIT_PROP_VALUE) || (varVal.vt == VT_BSTR && wcscmp( L"-2", varVal.bstrVal ) == 0) )
		return S_FALSE;

	if(pvarValue)
		varVal.Detach(pvarValue);

	return S_OK;
}

CPageObjectImplTemplate inline
STDMETHODIMP CPageObjectImplType::GetParentProp(long id, VARIANT_BOOL bAllowInherit, VARIANT *pvarValue)
{
	CComQIPtr<IVCStagedPropertyContainer> spStagedContainer = m_pContainer;
	if (spStagedContainer)
		return m_pContainer->GetParentProp(id, bAllowInherit, pvarValue);

	return m_pContainer->GetProp( id, pvarValue );
}

CPageObjectImplTemplate inline
STDMETHODIMP CPageObjectImplType::Clear(long id)
{
	CComQIPtr<IVCStagedPropertyContainer> spStagedContainer = m_pContainer;
	if (spStagedContainer)
		m_pContainer->Clear(id);

	m_PropMap.RemoveKey(id);
	return S_OK;
}

CPageObjectImplTemplate inline
STDMETHODIMP CPageObjectImplType::SetProp(long id, VARIANT varValue)
{
	CComQIPtr<IVCStagedPropertyContainer> spStagedContainer = m_pContainer;

	// if we're not in "set prop now" mode and "Edit..." was chosen, 
	// we need to launch the dlg box to edit this property
	if( !s_bSetPropNow && varValue.vt == VT_BSTR && wcscmp( L"-3", varValue.bstrVal ) == 0 )
	{
		HRESULT hr;
		CComVariant var;
		CComBSTR bstrProp;
		CComBSTR bstrName, bstrDesc;

		// if this is a part of an "unfinished" multi-select, skip it
		// (only the first item in a multi-select actually brings up a dlg)
		if( s_cDlgs > 0 )
		{
			s_cDlgs--;
			return S_OK;
		}

		// allows a particular property page (such as the custom build tool page) to give us a
		// different loc id than the property id itself.
		long locId = KludgeLocID(id);

		GetLocalizedPropertyInfo( locId, LOCALE_USER_DEFAULT, &bstrName, &bstrDesc );

		// if this is the first item in a multi-select, don't get a
		// default value
		int cObjects = 0;
		m_pPage->get_NumObjects( &cObjects );
		if( cObjects > 1 )
		{
			s_cDlgs = cObjects;
			bstrProp = L"";
		}

		else
		{
			// get the prop value, if not local, then get from parent
			if (spStagedContainer)
				hr = m_pContainer->GetProp(id, &var);
			else
			{
				if( !m_PropMap.Lookup( id, var ) )
					hr = m_pContainer->GetProp( id, &var );
			}

			// if we didn't get a property, give the page a chance to tell us what the default would be
			if( hr == S_FALSE )
			{
				var = CComBSTR(L"");
				GetBaseDefault(id, var);
			}

			// if it was an inherit flag value, we need to get the inherited value instead
			if( var.bstrVal && wcscmp( L"-2", var.bstrVal ) == 0 )
			{
				// skip the parent and look it up one level higher still !
				if (spStagedContainer)
					hr = spStagedContainer->GetGrandParentProp(id, &var);
				else
					hr = m_pContainer->GetParentProp( id, VARIANT_TRUE /* allow inherit */, &var );
			}

			// again, if we didn't get a property, try to figure out what the default is
			// NOTE: we MUST do this check separately, or the grid will
			// throw up a msgbox that we don't want during the previous step.
			if( !var.bstrVal || hr == S_FALSE )
			{
				var = CComBSTR(L"");
				GetBaseDefault(id, var);	// allow the page to give us something for a default
			}

			bstrProp = var.bstrVal;
		}

		// get a pointer to the project engine impl
		CComQIPtr<IVCProjectEngineImpl> pProjEngImpl = g_pProjectEngine;
		RETURN_ON_NULL(pProjEngImpl);

		HWND hwndShell;
		pProjEngImpl->get_DialogOwner( &hwndShell );
		RETURN_ON_NULL(hwndShell);

		INT_PTR dlgRet = IDCANCEL;

		// single-line string prop editor
		// create the dlg with the value
		if (SupportsMultiLine(id))
		{
			CStringW strSeparator;
			GetPreferredSeparator(id, strSeparator);
			CSingleLineWithReturnsDlg dlg( m_pContainer, bstrName, bstrProp, strSeparator );
			pProjEngImpl->EnableModeless( FALSE );
			// run the dlg as modal (parented to the shell's main dlg owner window)
			dlgRet = dlg.DoModalW( hwndShell );
			if( dlgRet == IDOK )
				// get the prop
				var = dlg.GetPropertyString();
		}
		else	// multi-value, non-multi-prop
		{
			CSingleLineNoReturnsDlg dlg( m_pContainer, bstrName, bstrProp );
			pProjEngImpl->EnableModeless( FALSE );
			// run the dlg as modal (parented to the shell's main dlg owner window)
			dlgRet = dlg.DoModalW( hwndShell );
			if( dlgRet == IDOK )
				// get the prop
				var = dlg.GetPropertyString();
		}
		
		if( dlgRet == IDOK )
		{
			if( bstrProp != var.bstrVal )
			{
				// if we have multiple objects to set the prop on
				if( cObjects > 1 )
				{
					// set this property on each object
					s_bSetPropNow = true;
					for( int i = 0; i < cObjects; i++ )
					{
						CComPtr<IUnknown> pUnk;
						if( FAILED( m_pPage->GetObject( i, &pUnk ) ) || !pUnk )
							continue;
						CComQIPtr<IVCPropertyContainer> pPropCnt = pUnk;
						if( !pPropCnt )
							continue;
						pPropCnt->SetProp( id, var );
					}
					s_bSetPropNow = false;

					// reduce the number of dialogs that we have to skip
					s_cDlgs--;
				}
				else
				{
					// property changed and OK clicked, so apply the change.
					if( m_pPage )
						m_pPage->Dirty();
					if (spStagedContainer)
						m_pContainer->SetProp(id, var );
					else
						m_PropMap.SetAt(id, var );
				}
			}
		}

		pProjEngImpl->EnableModeless( TRUE );
		
		return S_OK;
	}
	
	// otherwise, set the prop directly
	if (m_pPage)
		m_pPage->Dirty();

	if (spStagedContainer)
		m_pContainer->SetProp(id, varValue);
	else
		m_PropMap.SetAt(id, varValue);
	return S_OK;
}

CPageObjectImplTemplate inline
STDMETHODIMP CPageObjectImplType::GetProp(long id, /*[in]*/ VARIANT *pVarValue)
{
	CComQIPtr<IVCStagedPropertyContainer> spStagedContainer = m_pContainer;
	if (spStagedContainer)
		return m_pContainer->GetProp(id, pVarValue);
	
	CComVariant varVal;
	if( m_PropMap.Lookup(id, varVal) )
	{
		if( (varVal.vt == VT_I4 && varVal.intVal == INHERIT_PROP_VALUE) || (varVal.vt == VT_I2 && varVal.iVal == INHERIT_PROP_VALUE) || (varVal.vt == VT_BSTR && wcscmp( L"-2", varVal.bstrVal ) == 0) )
		{
			// skip you parent and look it up one level higher still !
			HRESULT hr = m_pContainer->GetParentProp( id, VARIANT_TRUE /* allow inherit */, pVarValue );
			return hr;
		}
		else
		{
			varVal.Detach(pVarValue);
			return S_OK;
		}
	}
	HRESULT hr = m_pContainer->GetProp( id, pVarValue );
	return hr;
}

CPageObjectImplTemplate inline
STDMETHODIMP CPageObjectImplType::GetMultiProp(long id, LPCOLESTR szSeparator, VARIANT_BOOL bSkipLocal, BSTR *pbstrValue)
{
	CComQIPtr<IVCStagedPropertyContainer> spStagedContainer = m_pContainer;
	if (spStagedContainer)
		return m_pContainer->GetMultiProp(id, szSeparator, bSkipLocal, pbstrValue);

	CHECK_POINTER_NULL(pbstrValue);
	*pbstrValue = NULL;

	CComBSTR bstrVal;
	if (!bSkipLocal)
	{
		// look in here.
		CComVariant var;
		BOOL b;
		b = m_PropMap.Lookup(id, var);
		if( b )
			bstrVal = var.bstrVal;
	}
	
	// And! check your parents
	HRESULT hr;
	CComBSTR bstr;
	hr = m_pContainer->GetMultiProp( id, szSeparator, VARIANT_FALSE, &bstr );
	if( hr == S_OK )
	{
		bstrVal.Append(szSeparator);
		bstrVal.Append(bstr);
	}

	// did we get anything at all ?
	if( !bstrVal )
		return S_FALSE;
		
	*pbstrValue = bstrVal.Detach();
	return S_OK;
}

CPageObjectImplTemplate inline
STDMETHODIMP CPageObjectImplType::GetEvaluatedMultiProp(long id, LPCOLESTR szSeparator, VARIANT_BOOL bCollapseMultiples, VARIANT_BOOL bCaseSensitive, BSTR* pbstrValue)
{
	return GetMultiProp(id, szSeparator, VARIANT_FALSE, pbstrValue);
}

CPageObjectImplTemplate inline
STDMETHODIMP CPageObjectImplType::IsMultiProp(long id, VARIANT_BOOL* pbIsMultiProp)
{
	CHECK_POINTER_NULL(pbIsMultiProp);
	
	*pbIsMultiProp = VARIANT_FALSE;
	T* pT = static_cast<T*>(this);
	
	// what type is this?
	CComVariant var;
	HRESULT hr = CComDispatchDriver::GetProperty( pT, id, &var );
	RETURN_ON_FAIL2(hr, S_FALSE);

	switch( var.vt )
	{
		case VT_BSTR:
		{
			CComQIPtr<IDispatch> pdisp = pT;
			RETURN_ON_NULL(pdisp);
	
			CComPtr<ITypeInfo> pTypeInfo;
			HRESULT hr = pdisp->GetTypeInfo(0, LANG_NEUTRAL, &pTypeInfo);
			CComQIPtr<ITypeInfo2> pTypeInfo2 = pTypeInfo;
			RETURN_ON_FAIL_OR_NULL(hr, pTypeInfo2);
	
			// find the property we are interested in
			TYPEATTR *pTypeAttr;
			pTypeInfo2->GetTypeAttr(&pTypeAttr);				
			UINT nIndex = -1;
			FUNCDESC *pFuncDesc = NULL;
			for (int i = 7; i < pTypeAttr->cFuncs; i++) // Skip over IUnknown and IDispatch
			{
				// if we do not get the vardesc, just continue
				if (FAILED(pTypeInfo2->GetFuncDesc(i, &pFuncDesc)))
					continue;

				// always look for the put method
				if( id == pFuncDesc->memid && pFuncDesc->invkind == INVOKE_PROPERTYPUT)
				{
					CComVariant var2;
					// this call fails and variant is not set if the custom data is not found
					hr = pTypeInfo2->GetFuncCustData(i, GUID_MULTIEDIT, &var2);
					if( SUCCEEDED( hr ) && var2.vt != VT_EMPTY && var2.lVal == 1 )
						*pbIsMultiProp = VARIANT_TRUE;
					pTypeInfo2->ReleaseFuncDesc(pFuncDesc);
					break;
				}
				pTypeInfo2->ReleaseFuncDesc(pFuncDesc);
				pFuncDesc = NULL;
			}
			pTypeInfo2->ReleaseTypeAttr(pTypeAttr);
		}
	}
	return S_OK;
}

CPageObjectImplTemplate inline
STDMETHODIMP CPageObjectImplType::GetMultiPropSeparator(long id, BSTR* pbstrPreferred, BSTR* pbstrAll)
{
	CHECK_POINTER_NULL(pbstrPreferred);
	CHECK_POINTER_NULL(pbstrAll);
	CComBSTR bstrPreferred = L";";
	CComBSTR bstrAll = L";,";
	*pbstrPreferred = bstrPreferred.Detach();
	*pbstrAll = bstrAll.Detach();
	return S_OK;
}

CPageObjectImplTemplate inline
STDMETHODIMP CPageObjectImplType::GetStrProperty(long idProp, BSTR* pbstrValue)
{
	CComVariant var;
	HRESULT hr = GetProp(idProp, &var);
	if (hr == S_OK)
	{
		CComBSTR bstrVal = var.bstrVal;
		*pbstrValue = bstrVal.Detach();
	}
	return hr;
}

CPageObjectImplTemplate inline
STDMETHODIMP CPageObjectImplType::GetEvaluatedStrProperty(long idProp, BSTR* pbstrValue)
{
	return GetStrProperty(idProp, pbstrValue);
}

CPageObjectImplTemplate inline
STDMETHODIMP CPageObjectImplType::SetStrProperty(long idProp, BSTR bstrValue)
{
	CComVariant var( bstrValue );
	return SetProp(idProp, var);
}

CPageObjectImplTemplate inline
STDMETHODIMP CPageObjectImplType::GetIntProperty(long idProp, long* pnValue)
{
	CComQIPtr<IVCStagedPropertyContainer> spStagedContainer = m_pContainer;
	if (spStagedContainer)
		return m_pContainer->GetIntProperty(idProp, pnValue);

	CHECK_POINTER_NULL(pnValue);
	*pnValue = 0;

	CComVariant var;
	HRESULT hr = GetProp(idProp, &var);
	if (hr == S_OK)
		*pnValue = var.intVal;
	return hr;
}

CPageObjectImplTemplate inline
STDMETHODIMP CPageObjectImplType::SetIntProperty(long idProp, long nValue)
{
	CComVariant var(nValue);
	return SetProp(idProp, var);
}

CPageObjectImplTemplate inline
STDMETHODIMP CPageObjectImplType::GetBoolProperty(long idProp, VARIANT_BOOL* pbValue)
{
	CComQIPtr<IVCStagedPropertyContainer> spStagedContainer = m_pContainer;
	if (spStagedContainer)
		return m_pContainer->GetBoolProperty(idProp, pbValue);

	CHECK_POINTER_NULL(pbValue);
	*pbValue = VARIANT_FALSE;

	CComVariant var;
	HRESULT hr = GetProp(idProp, &var);
	if (hr == S_OK)
		*pbValue = var.boolVal;
	return hr;
}

CPageObjectImplTemplate inline
HRESULT CPageObjectImplType::GetEnumBoolProperty(long idProp, enumBOOL* pbValue)
{
	CHECK_POINTER_NULL(pbValue);
	*pbValue = (enumBOOL)VARIANT_FALSE;

	CComVariant var;
	HRESULT hr = GetProp(idProp, &var);
	if (hr == S_OK)
		*pbValue = (enumBOOL)var.boolVal;
	return hr;
}

CPageObjectImplTemplate inline
HRESULT CPageObjectImplType::GetEnumBoolProperty2(long idProp, long* pbValue, COptionHandlerBase* pOptHandler /* = NULL */)
{
	CHECK_POINTER_NULL(pbValue);
	*pbValue = (long)VARIANT_FALSE;

	CComVariant var;
	HRESULT hr = GetProp(idProp, &var);
	if (hr == S_FALSE)
	{
		var.vt = VT_I2;
		if (pOptHandler)
		{
			VARIANT_BOOL bVal = VARIANT_FALSE;
			pOptHandler->GetDefaultValue(idProp, &bVal, m_pContainer);
			var.boolVal = bVal;
		}
		else
		{
			var.lVal = 0;
			GetBaseDefault(idProp, var);
		}
	}
	if (SUCCEEDED(hr))	// INCLUDES S_FALSE
		*pbValue = (long)var.boolVal;
	return hr;
}

CPageObjectImplTemplate inline
HRESULT CPageObjectImplType::ToolGetIntProperty(long idProp, COptionHandlerBase* pOptHandler, long* pVal)
{
	CHECK_POINTER_NULL( pVal );
	HRESULT hr = GetIntProperty(idProp, pVal);
	if (hr == S_FALSE && pOptHandler)
		pOptHandler->GetDefaultValue( idProp, pVal, m_pContainer );
	return hr;
}

CPageObjectImplTemplate inline
HRESULT CPageObjectImplType::ToolGetStrProperty(long idProp, COptionHandlerBase* pOptHandler, BSTR* pVal, bool bLocalOnly /* = false */)
{
	CHECK_POINTER_NULL( pVal );
	HRESULT hr = S_OK;
	
	if (bLocalOnly)
	{
		CComVariant var;
		*pVal = NULL;
		if (m_pContainer->GetLocalProp(idProp, &var) != S_OK)
			hr = m_pContainer->GetParentProp(idProp, VARIANT_FALSE /* no inherit */, &var);
		if (hr == S_OK)
		{
			CComBSTR bstrProp = var.bstrVal;
			*pVal = bstrProp.Detach();
		}
	}
	else
	{
		hr = GetStrProperty(idProp, pVal);
		if (hr == S_FALSE && pOptHandler)
			pOptHandler->GetDefaultValue( idProp, pVal, m_pContainer );
	}
	return hr;
}

CPageObjectImplTemplate inline
HRESULT CPageObjectImplType::ToolGetBoolProperty(long idProp, COptionHandlerBase* pOptHandler, VARIANT_BOOL* pVal)
{
	CHECK_POINTER_NULL( pVal );
	HRESULT hr = GetBoolProperty(idProp, pVal);
	if (hr == S_FALSE && pOptHandler)
		pOptHandler->GetDefaultValue( idProp, pVal, m_pContainer );
	return hr;
}

CPageObjectImplTemplate inline
STDMETHODIMP CPageObjectImplType::SetBoolProperty(long idProp, VARIANT_BOOL bValue)
{
	if( bValue > 0 || bValue <-2 ) // allow "inherit" through
		RETURN_INVALID();
		
	CComVariant var(bValue);
	return SetProp(idProp, var);
}

CPageObjectImplTemplate inline
STDMETHODIMP CPageObjectImplType::HasLocalStorage(VARIANT_BOOL /* bForSave */, VARIANT_BOOL* pbHasLocalStorage)
{
	CHECK_POINTER_NULL(pbHasLocalStorage);
	*pbHasLocalStorage = VARIANT_TRUE;
	return S_OK;
}

CPageObjectImplTemplate inline
STDMETHODIMP CPageObjectImplType::GetStagedPropertyContainer(VARIANT_BOOL bCreateIfNecessary, IVCStagedPropertyContainer** ppStagedContainer)
{
	CHECK_POINTER_NULL(ppStagedContainer);
	*ppStagedContainer = NULL;
	return E_NOTIMPL;
}

CPageObjectImplTemplate inline
STDMETHODIMP CPageObjectImplType::ReleaseStagedPropertyContainer()		// intended for use ONLY by the staged property container itself
{ 
	CComQIPtr<IVCStagedPropertyContainer> spStagedContainer = m_pContainer;
	if( spStagedContainer )
		spStagedContainer->ReleaseStagedContainer();
	return S_OK; 
}

CPageObjectImplTemplate inline
STDMETHODIMP CPageObjectImplType::HideProperty( DISPID dispid, BOOL *pfHide)
{
	CHECK_POINTER_NULL(pfHide);
	*pfHide = FALSE;
	return E_NOTIMPL;
};

CPageObjectImplTemplate inline
STDMETHODIMP CPageObjectImplType::DisplayChildProperties( DISPID dispid, BOOL *pfDisplay)
{
	CHECK_POINTER_NULL(pfDisplay);
	*pfDisplay = FALSE;
	return S_OK;
}

CPageObjectImplTemplate inline
STDMETHODIMP CPageObjectImplType::GetLocalizedPropertyInfo( DISPID dispid, LCID localeID, BSTR *pbstrName, BSTR *pbstrDesc)
{
	T* pT = static_cast<T*>(this);

	CComQIPtr<IDispatch> pdisp = pT;
	RETURN_ON_NULL(pdisp);

	CComPtr<ITypeInfo> pTypeInfo;
	HRESULT hr = pdisp->GetTypeInfo(0, LANG_NEUTRAL, &pTypeInfo);
	CComQIPtr<ITypeInfo2> pTypeInfo2 = pTypeInfo;
	RETURN_ON_FAIL_OR_NULL(hr, pTypeInfo2);

	CComBSTR bstrDoc;
	hr = pTypeInfo2->GetDocumentation2(dispid, localeID, &bstrDoc, NULL, NULL);
	RETURN_ON_FAIL(hr);
	
	RETURN_ON_NULL(bstrDoc.m_str);

	LPOLESTR pDesc = wcsstr( bstrDoc, L": " );
	RETURN_ON_NULL(pDesc);

	int nSize = (int)(pDesc - bstrDoc);
	pDesc+=2;

	CComBSTR bstrName( nSize-1, bstrDoc );
	if (pbstrName != NULL)
		*pbstrName = bstrName.Detach();

	CComBSTR bstrDesc( pDesc );
	if (pbstrDesc != NULL)
		*pbstrDesc = bstrDesc.Detach();
	
	return S_OK;
}

CPageObjectImplTemplate inline
STDMETHODIMP CPageObjectImplType::HasDefaultValue( DISPID dispid, BOOL *pfDefault)
{
	CComQIPtr<IVCStagedPropertyContainer> spStagedContainer = m_pContainer;
	if (spStagedContainer)
		return spStagedContainer->HasDefaultValue(dispid, pfDefault);

	CHECK_POINTER_NULL(pfDefault);
	
	// Check if its here and not clear
	CComVariant varVal;
	if( m_PropMap.Lookup(dispid, varVal) )
	{
		if( (varVal.vt == VT_I4 && varVal.intVal == INHERIT_PROP_VALUE) || (varVal.vt == VT_I2 && varVal.iVal == INHERIT_PROP_VALUE) || (varVal.vt == VT_BSTR && wcscmp( L"-2", varVal.bstrVal ) == 0) )
		{
			*pfDefault = TRUE;
			return S_OK;
		}
		*pfDefault = FALSE;
		return S_OK;
	}
	// OK so is it in the parent ?
	HRESULT hr = m_pContainer->GetLocalProp( dispid, NULL );
	if( hr == S_OK )
	{
		*pfDefault = FALSE;
		return S_OK;
	}
	*pfDefault = TRUE;
	return S_OK;
}

CPageObjectImplTemplate inline
STDMETHODIMP CPageObjectImplType::IsPropertyReadOnly( DISPID dispid, BOOL *fReadOnly)
{
	CHECK_POINTER_NULL(fReadOnly);
	CStringW str;
	*fReadOnly = GetTruncatedStringForProperty(dispid, str);
	return S_OK;
}

CPageObjectImplTemplate inline
STDMETHODIMP CPageObjectImplType::GetDisplayString( DISPID dispID, BSTR *pBstr )
{
	CHECK_POINTER_NULL(pBstr)
	*pBstr = NULL;
			
	HRESULT hr;

	T* pT = static_cast<T*>(this);
	
	// what type is this?
	CComVariant var;
	hr = CComDispatchDriver::GetProperty( pT, dispID, &var );
	RETURN_ON_FAIL2(hr, S_FALSE);

	switch( var.vt )
	{
		case VT_BSTR:
		{
			// if it was a bstr, return it and let the default behavior happen
			CComBSTR bstrTemp = var.bstrVal;
			*pBstr = bstrTemp.Detach();
			return S_FALSE;
		}
		case VT_BOOL:
		{
			if( var.boolVal == VARIANT_TRUE )
			{
				CComBSTR bstrTrue;
				bstrTrue.LoadString(VCTDENUM_Yes);
				*pBstr = bstrTrue.Detach();
			}
			else 
			{
				CComBSTR bstrFalse;
				bstrFalse.LoadString(VCTDENUM_No);
				*pBstr = bstrFalse.Detach();
			}
			return S_OK;
		}
		case VT_I2:
		case VT_I4:
		{
			T* pT = static_cast<T*>(this);
		
			CComQIPtr<IDispatch> pdisp = pT;
			RETURN_ON_NULL(pdisp);
		
			CComPtr<ITypeInfo> pTypeInfo;
			HRESULT hr = pdisp->GetTypeInfo(0, LANG_NEUTRAL, &pTypeInfo);
			CComQIPtr<ITypeInfo2> pTypeInfo2 = pTypeInfo;
			RETURN_ON_FAIL_OR_NULL(hr, pTypeInfo2);
		
			// find the property we are interested in
			TYPEATTR *pTypeAttr;
			pTypeInfo2->GetTypeAttr(&pTypeAttr);
			UINT nIndex = -1;
			FUNCDESC *pFuncDesc = NULL;
			for (int i = 7; i < pTypeAttr->cFuncs; i++)		// Skip over IUnknown and IDispatch
			{
				// if we do not get the vardesc, just continue
				if (FAILED(pTypeInfo2->GetFuncDesc(i, &pFuncDesc)))
					continue;

				// always look for the put method
				if( dispID == pFuncDesc->memid && pFuncDesc->invkind == INVOKE_PROPERTYPUT)
				{
					break;
				}
				pTypeInfo2->ReleaseFuncDesc(pFuncDesc);
				pFuncDesc = NULL;
			}
			pTypeInfo2->ReleaseTypeAttr(pTypeAttr);
			RETURN_ON_NULL(pFuncDesc);
			
			// look at the first parameter
			TYPEDESC * pTypeDesc = &pFuncDesc->lprgelemdescParam->tdesc;
			HREFTYPE hrt = pTypeDesc->hreftype;

			CComPtr<ITypeInfo> pTypeInfoEnum;
			pTypeInfo2->GetRefTypeInfo(hrt, &pTypeInfoEnum);
			pTypeInfo2->ReleaseFuncDesc(pFuncDesc);

			CComQIPtr<ITypeInfo2> pTypeInfo2Enum = pTypeInfoEnum;
			RETURN_ON_NULL(pTypeInfo2Enum);

			TYPEATTR *pTypeAttrEnum = NULL;
			pTypeInfo2Enum->GetTypeAttr(&pTypeAttrEnum);
			RETURN_ON_NULL(pTypeAttrEnum);
				
			if( pTypeAttrEnum->typekind != TKIND_ENUM )
			{
				// this isn't an enum after all. It must be a number
				pTypeInfoEnum->ReleaseTypeAttr(pTypeAttrEnum);
				return S_FALSE;
			}

			// look for the enum value that this prop is set to.
			for (int j = 0; j < pTypeAttrEnum->cVars; j++)
			{
				CComBSTR	bstrVal;
				VARDESC		*pVarDesc;
				// if we do not get the vardesc, just continue
				if (FAILED(pTypeInfo2Enum->GetVarDesc(j, &pVarDesc)))
					continue;

				long lCurrentValue = pVarDesc->lpvarValue->lVal;
				if (lCurrentValue == var.lVal)
				{
					// we found it so return the string.
					CComBSTR bstrDoc;
					hr = pTypeInfo2Enum->GetDocumentation2(pVarDesc->memid, NULL, &bstrDoc, NULL, NULL);
					if( bstrDoc.m_str == NULL )
					{
						hr = pTypeInfo2Enum->GetDocumentation(pVarDesc->memid, &bstrDoc, NULL, NULL, NULL);
					}

					*pBstr = bstrDoc.Detach();
					pTypeInfoEnum->ReleaseVarDesc(pVarDesc);
					pTypeInfoEnum->ReleaseTypeAttr(pTypeAttrEnum);
					return hr;
				}
				else
				{
					pTypeInfoEnum->ReleaseVarDesc(pVarDesc);
				}
			}

			pTypeInfoEnum->ReleaseTypeAttr(pTypeAttrEnum);
			return S_FALSE;
		}
		default:
		{
			// unhandled type, do default
			return S_FALSE;
		}
	}
}

CPageObjectImplTemplate inline
STDMETHODIMP CPageObjectImplType::GetPredefinedStrings( DISPID dispID, CALPOLESTR *pCaStringsOut, CADWORD *pCaCookiesOut )
{
	CHECK_POINTER_NULL(pCaStringsOut)
	CHECK_POINTER_NULL(pCaCookiesOut)
	
	HRESULT hr;

	T* pT = static_cast<T*>(this);
	
	// what type is this?
	CComVariant var;
	hr = CComDispatchDriver::GetProperty( pT, dispID, &var );
	RETURN_ON_FAIL2(hr, S_FALSE);

	CComBSTR bstrInherit;
	CComPtr<IVCPropertyContainer> spContainer;
	CComQIPtr<IVCStagedPropertyContainer> spStagedContainer = m_pContainer;
	if (spStagedContainer)
		spStagedContainer->get_Parent(&spContainer);
	else
		spContainer = m_pContainer;

	CComQIPtr<VCConfiguration> pPrj = spContainer;
	if( pPrj )
		bstrInherit.LoadString(IDS_INHERITPRJDEF);
	else
		bstrInherit.LoadString(IDS_INHERITPRJ);

	CComBSTR bstrEdit;
	bstrEdit.LoadString(IDS_EDIT);

	pCaCookiesOut->cElems = 0;
	pCaCookiesOut->pElems = NULL;
	pCaStringsOut->cElems = 0;
	pCaStringsOut->pElems = NULL;

	BOOL bInherited;
	HasDefaultValue( dispID, &bInherited );
	
	switch( var.vt )
	{
		case VT_BOOL:
		{
			CComBSTR bstrTrue;
			bstrTrue.LoadString(VCTDENUM_Yes);
			CComBSTR bstrFalse;
			bstrFalse.LoadString(VCTDENUM_No);
			int nElems = 2;

			pCaCookiesOut->pElems = (DWORD*)CoTaskMemAlloc( nElems * sizeof(DWORD) );
			if(pCaCookiesOut->pElems)
			{
				pCaCookiesOut->pElems[0] = 0;
				pCaCookiesOut->pElems[1] = 1;
				pCaCookiesOut->cElems = nElems;
			}
			
			pCaStringsOut->pElems = (LPOLESTR*)CoTaskMemAlloc( nElems * sizeof(LPOLESTR) );
			if(pCaStringsOut->pElems)
			{
				pCaStringsOut->cElems = nElems;
				pCaStringsOut->pElems[0] = (LPOLESTR)CoTaskMemAlloc( (SysStringLen( bstrFalse ) + 1) * sizeof(OLECHAR) );
				wcscpy( pCaStringsOut->pElems[0], bstrFalse );
				pCaStringsOut->pElems[1] = (LPOLESTR)CoTaskMemAlloc( (SysStringLen( bstrTrue ) + 1) * sizeof(OLECHAR) );
				wcscpy( pCaStringsOut->pElems[1], bstrTrue );
			}
			return S_OK;
		}
		case VT_I2:
		case VT_I4:
		{
			T* pT = static_cast<T*>(this);
	
			CComQIPtr<IDispatch> pdisp = pT;
			RETURN_ON_NULL(pdisp);
	
			CComPtr<ITypeInfo> pTypeInfo;
			HRESULT hr = pdisp->GetTypeInfo(0, LANG_NEUTRAL, &pTypeInfo);
			CComQIPtr<ITypeInfo2> pTypeInfo2 = pTypeInfo;
			RETURN_ON_FAIL_OR_NULL(hr, pTypeInfo2);
	
			// find the property we are interested in
			TYPEATTR *pTypeAttr;
			pTypeInfo2->GetTypeAttr(&pTypeAttr);				
			UINT nIndex = -1;
			FUNCDESC *pFuncDesc = NULL;
			for (int i = 7; i < pTypeAttr->cFuncs; i++) // Skip over IUnknown and IDispatch
			{
				// if we do not get the vardesc, just continue
				if (FAILED(pTypeInfo2->GetFuncDesc(i, &pFuncDesc)))
					continue;

				// always look for the put method
				if( dispID == pFuncDesc->memid && pFuncDesc->invkind == INVOKE_PROPERTYPUT)
				{
					break;
				}
				pTypeInfo2->ReleaseFuncDesc(pFuncDesc);
				pFuncDesc = NULL;
			}
			pTypeInfo2->ReleaseTypeAttr(pTypeAttr);
			RETURN_ON_NULL(pFuncDesc);

			// Get the TypeInfo of the first (enum) argument
			TYPEDESC * pTypeDesc = &pFuncDesc->lprgelemdescParam->tdesc;
			HREFTYPE hrt = pTypeDesc->hreftype;
			if( hrt == NULL )
			{
				// this isn't an enum after all. It must be a number
				// determine if we can inherit and display option only if we
				// can
				if( bInherited == FALSE )
				{
					pCaCookiesOut->pElems = (DWORD*)CoTaskMemAlloc( sizeof(DWORD) );
					if(pCaCookiesOut->pElems)
					{
						pCaCookiesOut->cElems = 1;
						pCaCookiesOut->pElems[0] = INHERIT_PROP_VALUE;
					}

					pCaStringsOut->pElems = (LPOLESTR*)CoTaskMemAlloc( sizeof(LPOLESTR) );
					if(pCaStringsOut->pElems)
					{
						pCaStringsOut->cElems = 1;
						pCaStringsOut->pElems[0] = (LPOLESTR)CoTaskMemAlloc( (SysStringLen( bstrInherit ) + 1) * sizeof(OLECHAR) );
						wcscpy( pCaStringsOut->pElems[0], bstrInherit );
					}
				}
				return S_OK;
			}

			CComPtr<ITypeInfo> pTypeInfoEnum;
			pTypeInfo2->GetRefTypeInfo(hrt, &pTypeInfoEnum);
			pTypeInfo2->ReleaseFuncDesc(pFuncDesc);

			CComQIPtr<ITypeInfo2> pTypeInfo2Enum = pTypeInfoEnum;
			RETURN_ON_NULL(pTypeInfo2Enum);

			TYPEATTR *pTypeAttrEnum = NULL;
			pTypeInfo2Enum->GetTypeAttr(&pTypeAttrEnum);
			RETURN_ON_NULL(pTypeAttrEnum);
			
			if( pTypeAttrEnum->typekind != TKIND_ENUM )
			{
				// error condition should have been handled as "just a number"
				return S_OK;
			}

			// Generate the list of enum values
			int n = pTypeAttrEnum->cVars;
			if( bInherited == FALSE )
			{
				n++;
			}

			pCaCookiesOut->pElems = (DWORD*)CoTaskMemAlloc( n * sizeof(DWORD) );
			if(pCaCookiesOut->pElems)
				pCaCookiesOut->cElems = n;
			pCaStringsOut->pElems = (LPOLESTR*)CoTaskMemAlloc( n * sizeof(LPOLESTR) );
			if( pCaStringsOut->pElems )
				pCaStringsOut->cElems = n;
				
			if( pCaStringsOut->cElems && pCaCookiesOut->cElems )
			{
				for (int j = 0; j < pTypeAttrEnum->cVars; j++)
				{
					CComBSTR	bstrVal;
					VARDESC		*pVarDesc;
					// if we do not get the vardesc, just continue
					if (FAILED(pTypeInfo2Enum->GetVarDesc(j, &pVarDesc)))
					{
						// REVIEW(kperry): is it OK to have NULLs in the array?
						pCaCookiesOut->pElems[j] = 0;
						pCaStringsOut->pElems[j] = NULL;
						continue;
					}
	
					// try to get the localized name if possible.
					CComBSTR bstrDoc;
					GetLocalizedName(dispID, pVarDesc->memid, pTypeInfo2Enum, pVarDesc->lpvarValue->lVal, bstrDoc);

					// now fill in the element in the return array.
					pCaCookiesOut->pElems[j] = pVarDesc->lpvarValue->lVal;
					pCaStringsOut->pElems[j] = (LPOLESTR)CoTaskMemAlloc( (bstrDoc.Length()+1) * sizeof(OLECHAR) );
					wcscpy( pCaStringsOut->pElems[j], bstrDoc );
			
					pTypeInfoEnum->ReleaseVarDesc(pVarDesc);
				}
				
				// now add the <inherit> value
				if( bInherited == FALSE )
				{
					// now fill in the lement in the return array.
					pCaCookiesOut->pElems[n-1] = INHERIT_PROP_VALUE;
					pCaStringsOut->pElems[n-1] = (LPOLESTR)CoTaskMemAlloc( (bstrInherit.Length()+1) * sizeof(OLECHAR) );
					wcscpy( pCaStringsOut->pElems[n-1], bstrInherit );
				}
			}

			pTypeInfoEnum->ReleaseTypeAttr(pTypeAttrEnum);
			return S_OK;
		}
		case VT_BSTR:
		{
			if( MapPropertyToBuilder( dispID, NULL, NULL, NULL ) == S_OK )
				return S_OK;

			// determine if we can inherit and display option only if we
			// can
			if( bInherited == FALSE )
			{
				pCaCookiesOut->pElems = (DWORD*)CoTaskMemAlloc( 2* sizeof(DWORD) );
				if(pCaCookiesOut->pElems)
				{
					pCaCookiesOut->cElems = 2;
					pCaCookiesOut->pElems[0] = INHERIT_PROP_VALUE;
					pCaCookiesOut->pElems[1] = EDIT_PROP_VALUE;
				}

				pCaStringsOut->pElems = (LPOLESTR*)CoTaskMemAlloc( 2 * sizeof(LPOLESTR) );
				if( pCaStringsOut->pElems )
				{
					pCaStringsOut->cElems = 2;
					pCaStringsOut->pElems[0] = (LPOLESTR)CoTaskMemAlloc( (SysStringLen( bstrInherit ) + 1) * sizeof(OLECHAR) );
					wcscpy( pCaStringsOut->pElems[0], bstrInherit );
					pCaStringsOut->pElems[1] = (LPOLESTR)CoTaskMemAlloc( (SysStringLen( bstrEdit ) + 1) * sizeof(OLECHAR) );
					wcscpy( pCaStringsOut->pElems[1], bstrEdit );
				}
				return S_OK;
			}
			else
			{
				pCaCookiesOut->pElems = (DWORD*)CoTaskMemAlloc( sizeof(DWORD) );
				if(pCaCookiesOut->pElems)
				{
					pCaCookiesOut->cElems = 1;
					pCaCookiesOut->pElems[0] = EDIT_PROP_VALUE;
				}

				pCaStringsOut->pElems = (LPOLESTR*)CoTaskMemAlloc( sizeof(LPOLESTR) );
				if(pCaStringsOut->pElems)
				{
					pCaStringsOut->cElems = 1;
					pCaStringsOut->pElems[0] = (LPOLESTR)CoTaskMemAlloc( (SysStringLen( bstrEdit ) + 1) * sizeof(OLECHAR) );
					wcscpy( pCaStringsOut->pElems[0], bstrEdit );
				}
				return S_OK;
			}
		}
		default:
		{
			// unhandled type, do default
			return S_FALSE;
		}
	}
	return E_FAIL;
}

CPageObjectImplTemplate inline
STDMETHODIMP CPageObjectImplType::GetPredefinedValue( DISPID dispID, DWORD dwCookie, VARIANT* pVarOut )
{
	CHECK_POINTER_NULL(pVarOut)
	
	HRESULT hr;
	T* pT = static_cast<T*>(this);
	
	// what type is this?
	CComVariant var;
	hr = CComDispatchDriver::GetProperty( pT, dispID, &var );
	RETURN_ON_FAIL2(hr, S_FALSE);
	
	CComVariant varOut;
	
	switch( var.vt )
	{
	// boolean
	case VT_BOOL:
	{
		if( dwCookie == 0 )
		{
			// varOut.boolVal = VARIANT_FALSE;
			CComBSTR bstrFalse(L"False");
			varOut = bstrFalse;
		}
		else if( dwCookie == 1 )
		{
			CComBSTR bstrTrue(L"True");
			varOut = bstrTrue;
			// varOut.boolVal = VARIANT_TRUE;
			// varOut = L"True";
		}
		break;
	}
	case VT_I2:
	case VT_I4:
	{
		// the cookie is always == the enum value
		varOut = (int)dwCookie; // unsigned, to signed conversion intentional.
		break;
	}
	case VT_BSTR:
		// 0 indicates string
		if( dwCookie == 0 )
			varOut = var;
		else
			varOut = (int)dwCookie;
		break;
	default:
		// unhandled type, do default
		return S_FALSE;
	}
	varOut.Detach(pVarOut);
	return S_OK;
}

CPageObjectImplTemplate inline
STDMETHODIMP CPageObjectImplType::MapPropertyToBuilder( LONG dispid, LONG *pdwCtlBldType, BSTR *pbstrGuidBldr, VARIANT_BOOL *pfRetVal )
{
	HRESULT hr = S_OK;
	const wchar_t *szGuid = NULL;
	VARIANT_BOOL bIsMulti = VARIANT_FALSE;
	IsMultiProp( dispid, &bIsMulti );
	if ( UseDirectoryPickerDialog( dispid ) )
	{
		szGuid = L"{7E23E102-4A22-4764-B6A5-E7ED5E2F3C79}";
	}
	else if( bIsMulti == VARIANT_TRUE )
	{
		szGuid = L"{7E23E103-4A22-4764-B6A5-E7ED5E2F3C79}";
	}
	else if( UseCommandsDialog( dispid ) )
	{
		szGuid = L"{7E23E104-4A22-4764-B6A5-E7ED5E2F3C79}";
	}
	else if ( UseSinglePropDirectoryPickerDialog( dispid ))
	{
		szGuid = L"{7E23E105-4A22-4764-B6A5-E7ED5E2F3C79}";
	}
	else if ( UseMultiLineNoInheritDialog( dispid ))
	{
		szGuid = L"{7E23E106-4A22-4764-B6A5-E7ED5E2F3C79}";
	}
	else if( IsFile( dispid ) )
	{
		szGuid = L"{7E23E107-4A22-4764-B6A5-E7ED5E2F3C79}";
	}
	else if( IsDir( dispid ) )
	{
		szGuid = L"{7E23E108-4A22-4764-B6A5-E7ED5E2F3C79}";
	}
	else
		hr = S_FALSE;

	if( hr == S_OK )
	{
		if( pfRetVal != NULL )
			*pfRetVal = VARIANT_TRUE;
		if( pdwCtlBldType != NULL )
			*pdwCtlBldType = CTLBLDTYPE_FINTERNALBUILDER;
		if( pbstrGuidBldr != NULL )
			*pbstrGuidBldr = SysAllocString(szGuid);
	}
	else
	{
		if( pfRetVal != NULL )
			*pfRetVal = VARIANT_FALSE;
	}
	
	return hr;
}

CPageObjectImplTemplate inline
STDMETHODIMP CPageObjectImplType::ExecuteBuilder( LONG dispid, BSTR bstrGuidBldr, IDispatch *pdispApp, 
	LONG_PTR hwndBldrOwner, VARIANT *pvarValue, VARIANT_BOOL *pfRetVal )
{
	HRESULT hr = S_OK;
	CComVariant var;
	var.Attach( pvarValue );

	CComQIPtr<IVCProjectEngineImpl> pProjEngImpl = g_pProjectEngine;
	RETURN_ON_NULL2(pProjEngImpl, E_NOINTERFACE);

	CComBSTR bstrName, bstrDesc, bstrInherit;
	GetLocalizedPropertyInfo( dispid, LOCALE_USER_DEFAULT, &bstrName, &bstrDesc );
	// get the prop value
	CComBSTR bstrProp = (var.vt == VT_BSTR) ? var.bstrVal : L"";

	// get the hwnd for the shell
	HWND hwndShell;
	pProjEngImpl->get_DialogOwner( &hwndShell );
	RETURN_ON_NULL(hwndShell);

	if( wcscmp(bstrGuidBldr, L"{7E23E102-4A22-4764-B6A5-E7ED5E2F3C79}") == 0 || 
		wcscmp(bstrGuidBldr, L"{7E23E103-4A22-4764-B6A5-E7ED5E2F3C79}") == 0 )
	{
		// get the "inherit from XXXX" string for this prop
		CComPtr<IVCPropertyContainer> spContainer;
		CComQIPtr<IVCStagedPropertyContainer> spStagedContainer = m_pContainer;
		if (spStagedContainer)
			spStagedContainer->get_Parent(&spContainer);
		else
			spContainer = m_pContainer;
		CComQIPtr<VCConfiguration> pPrj = spContainer;
		if( pPrj )
			bstrInherit.LoadString( IDS_INHERITPRJDEF2 );
		else
			bstrInherit.LoadString( IDS_INHERITPRJ2 );

		// create the dlg with the value
		CComVariant var2;
		if (m_pContainer->GetLocalProp(dispid, &var2) != S_OK)
			m_pContainer->GetParentProp(dispid, VARIANT_FALSE /* no inherit */, &var2);
		CComBSTR bstrLocalOnly;
		if (var2.vt == VT_BSTR)
			bstrLocalOnly = bstrProp;
		BOOL bOK = FALSE;
		if (wcscmp(bstrGuidBldr, L"{7E23E102-4A22-4764-B6A5-E7ED5E2F3C79}") == 0)	// using directory picker
		{
			CMultiDirectoryPickerEditorDlg dlg(m_pContainer, dispid, bstrName, bstrLocalOnly, bstrInherit);
			pProjEngImpl->EnableModeless( FALSE );
			// run the dlg as modal
			bool bAlreadyRun = false;
			if (m_pContainer)
			{
				CComBSTR bstrProjDir;
				m_pContainer->GetStrProperty(VCPROJID_ProjectDirectory, &bstrProjDir);
				if (bstrProjDir.Length())
				{
					CCurDirW dir(bstrProjDir);
					bOK = ( dlg.DoModalW( hwndShell ) == IDOK );
					bAlreadyRun = true;
				}
			}
			if (!bAlreadyRun)
				bOK = ( dlg.DoModalW( hwndShell ) == IDOK );
			if (bOK)
				var = dlg.GetPropertyString();
		}
		else	// using multi-prop string editor
		{
			CMultiLineDlg dlg( m_pContainer, dispid, bstrName, bstrLocalOnly, bstrInherit );
			pProjEngImpl->EnableModeless( FALSE );
			// run the dlg as modal
			bOK = ( dlg.DoModalW( hwndShell ) == IDOK );
			if (bOK)
				var = dlg.GetPropertyString();
		}
		if (bOK)
		{
			// set the prop
			if( pfRetVal )
				*pfRetVal = VARIANT_TRUE;
		}
		else
		{
			var.Clear();
			if( pfRetVal )
				*pfRetVal = VARIANT_FALSE;
		}
	}
	else if( wcscmp(bstrGuidBldr, L"{7E23E104-4A22-4764-B6A5-E7ED5E2F3C79}") == 0 )
	{
		// create the dlg with the value
		CCommandsDlg dlg( m_pContainer, bstrName, bstrProp );
		pProjEngImpl->EnableModeless( FALSE );
		// run the dlg as modal (parented to the shell's main dlg owner window)
		if( dlg.DoModalW( hwndShell ) == IDOK )
		{
			// set the prop
			var = dlg.GetPropertyString();
			if( pfRetVal )
				*pfRetVal = VARIANT_TRUE;
		}
		else
		{
			var.Clear();
			if( pfRetVal )
				*pfRetVal = VARIANT_FALSE;
		}
	}
	else if( wcscmp(bstrGuidBldr, L"{7E23E105-4A22-4764-B6A5-E7ED5E2F3C79}") == 0 )
	{
		// create the dlg with the value
		CSingleDirectoryPickerEditorDlg dlg( m_pContainer, bstrName, bstrProp );
		pProjEngImpl->EnableModeless( FALSE );
		// run the dlg as modal (parented to the shell's main dlg owner window)
		if( dlg.DoModalW( hwndShell ) == IDOK )
		{
			// set the prop
			var = dlg.GetPropertyString();
			if( pfRetVal )
				*pfRetVal = VARIANT_TRUE;
		}
		else
		{
			var.Clear();
			if( pfRetVal )
				*pfRetVal = VARIANT_FALSE;
		}
	}
	else if( wcscmp(bstrGuidBldr, L"{7E23E106-4A22-4764-B6A5-E7ED5E2F3C79}") == 0 )
	{
		// create the dlg with the value
		CMultiLineNoInheritDlg dlg( m_pContainer, bstrName, bstrProp );
		pProjEngImpl->EnableModeless( FALSE );
		// run the dlg as modal (parented to the shell's main dlg owner window)
		if( dlg.DoModalW( hwndShell ) == IDOK )
		{
			// set the prop
			var = dlg.GetPropertyString();
			if( pfRetVal )
				*pfRetVal = VARIANT_TRUE;
		}
		else
		{
			var.Clear();
			if( pfRetVal )
				*pfRetVal = VARIANT_FALSE;
		}
	}
	else if( wcscmp(bstrGuidBldr, L"{7E23E107-4A22-4764-B6A5-E7ED5E2F3C79}") == 0 )
	{
		CComBSTR bstrTitle;
		bstrTitle.LoadString(dispid);
		wchar_t szBuf[4096];

		VSOPENFILENAMEW openFileName = {0};
		CStringW strFilter( MAKEINTRESOURCE( IDS_EXEFilter ));
		strFilter.Replace( L';', 0 );
		strFilter += L'\0';
		
		openFileName.lStructSize = sizeof (VSOPENFILENAMEW);
		openFileName.hwndOwner = hwndShell;
		openFileName.pwzDlgTitle = bstrTitle;
		openFileName.pwzFileName = szBuf;
		openFileName.nMaxFileName = 4096;
		openFileName.pwzInitialDir = NULL;
		openFileName.pwzFilter = strFilter;
		openFileName.nFilterIndex = 0;
		openFileName.nFileOffset = 0;
		openFileName.nFileExtension = 0;
		openFileName.dwHelpTopic = 0;
		openFileName.dwFlags = 0;
		
		pProjEngImpl->EnableModeless( FALSE );
		hr = static_cast<CVCProjectEngine*>(g_pProjectEngine)->GetOpenFileNameViaDlg( &openFileName );
		if( hr == S_OK )
		{
			var = openFileName.pwzFileName;
			if( pfRetVal )
				*pfRetVal = VARIANT_TRUE;
		}
		else
		{
			if( pfRetVal )
				*pfRetVal = VARIANT_FALSE;
		}
	}
	else if( wcscmp(bstrGuidBldr, L"{7E23E108-4A22-4764-B6A5-E7ED5E2F3C79}") == 0 )
	{
		CComBSTR bstrTitle;
		bstrTitle.LoadString(dispid);
		wchar_t szBuf[4096];

		CComBSTR bstrProjDir;
		if (m_pContainer)
		{
			m_pContainer->GetStrProperty(VCPROJID_ProjectDirectory, &bstrProjDir);
		}

		VSBROWSEINFOW dir;
		dir.lStructSize = sizeof(VSBROWSEINFOW);
		dir.hwndOwner = hwndShell;
		dir.pwzDlgTitle = bstrTitle;
		dir.pwzDirName = szBuf;
		dir.nMaxDirName = 4096;
		dir.pwzInitialDir = bstrProp;
		dir.dwHelpTopic = NULL;
		dir.dwFlags = NULL; // BIF_* flags
		pProjEngImpl->EnableModeless( FALSE );
		if (bstrProjDir.Length())
		{
			CCurDirW cwd(bstrProjDir);
			hr = static_cast<CVCProjectEngine*>(g_pProjectEngine)->GetDirViaDlg( &dir );
		}
		else
		{
			// chances of going through this code path a negligible
			hr = static_cast<CVCProjectEngine*>(g_pProjectEngine)->GetDirViaDlg( &dir );
		}

		if( hr == S_OK )
		{
			var = dir.pwzDirName;
			if( pfRetVal )
				*pfRetVal = VARIANT_TRUE;
		}
		else
		{
			if( pfRetVal )
				*pfRetVal = VARIANT_FALSE;
		}
	}
	
	pProjEngImpl->EnableModeless( TRUE );

	var.Detach( pvarValue );
	return S_OK;
}

CPageObjectImplTemplate inline
void CPageObjectImplType::GetBaseDefault(long id, CComVariant& varValue) 
{ 
	if (varValue.vt == VT_BSTR)
		varValue = CComBSTR(L""); 
	else
	{
		varValue.vt = VT_I2;
		varValue.lVal = 0;
	}
}

CPageObjectImplTemplate inline
void CPageObjectImplType::GetLocalizedName(DISPID dispID, MEMBERID memid, ITypeInfo2* pTypeInfo2Enum, long lVal, CComBSTR& bstrDoc)
{
	HRESULT hr = pTypeInfo2Enum->GetDocumentation2(memid, NULL, &bstrDoc, NULL, NULL);
	if( bstrDoc.m_str == NULL )
	{
		// localized name not available. Use object model name.
		hr = pTypeInfo2Enum->GetDocumentation(memid, &bstrDoc, NULL, NULL, NULL);
	}
}

CPageObjectImplTemplate inline
BOOL CPageObjectImplType::GetTruncatedStringForProperty(DISPID dispid, CStringW & strOut)
{
#ifdef TRUNCATE_MULTILINE_STRINGS
	if (!UseCommandsDialog(dispid))
		return FALSE;

	if (strOut.IsEmpty())
	{
		CComVariant var;
		HRESULT hr = GetProp(dispid, &var);
		if (FAILED(hr))
			return FALSE;

		if(V_VT(&var) != VT_BSTR)
			return FALSE;

		strOut = var.bstrVal;
	}

	int i = strOut.FindOneOf(L"\r\n");
	if (-1 == i)
		return FALSE;

	strOut = strOut.Left(i);
	strOut += L"...";

	return TRUE;
#else
	return FALSE;
#endif
}

CPageObjectImplTemplate inline
void CPageObjectImplType::RefreshPropGrid(void)
{
#ifdef TRUNCATE_MULTILINE_STRINGS
	if(m_pPage)
		m_pPage->Refresh();
#else
	;
#endif
}

////////////////////////////////////////////////////////////////////////////////////////////////
// CAdditionalOptionsPage

CAdditionalOptionsPageTemplate inline
CAdditionalOptionsPageType::CAdditionalOptionsPage() 
{
	m_dwTitleID = IDS_Additional;
	m_dwHelpFileID = IDS_VCPROJECT; // dummy ID to get the help method invoked
	m_bActive = FALSE;
	m_hFont = NULL;
}

CAdditionalOptionsPageTemplate inline
LRESULT CAdditionalOptionsPageType::OnFocus(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	bHandled = FALSE;
	UpdateObjects(true);
	return FALSE;
}
	
CAdditionalOptionsPageTemplate inline
LRESULT CAdditionalOptionsPageType::OnChange(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	bHandled = TRUE;
	CComBSTR bstrOptions;
	HWND hwndOpt;
	hwndOpt = ::GetDlgItem(m_hWnd, IDC_ADDOPTIONS);
	wchar_t buf[16000];
	::GetWindowTextW(hwndOpt, buf, 16000);
	bstrOptions = buf;
	bool bUpdateObjects = false;
	
	UINT i;
	for (i = 0; i < m_nObjects; i++)
	{
		CComPtr<IDispatch> pDisp;
		CComQIPtr<IVCCfg> pVcCfg = m_ppUnk[i];
		HRESULT hr = pVcCfg->get_Object( &pDisp );
		CComQIPtr<IVCPropertyContainer> spPropContainer = pDisp;
		if (spPropContainer == NULL)
			continue;

		CComPtr<IVCStagedPropertyContainer> spStagedContainer;
		spPropContainer->GetStagedPropertyContainer(VARIANT_TRUE, &spStagedContainer);
		CComQIPtr<IVCPropertyContainer> spStage = spStagedContainer;
		if (spStagedContainer)
		{
			spStagedContainer->HoldStagedContainer();
			CComBSTR bstrOldOptions;
			spStage->GetStrProperty(nID, &bstrOldOptions);
			if (bstrOldOptions != bstrOptions)
			{
				bUpdateObjects = true;
				spStage->SetStrProperty(nID, bstrOptions);
			}
			spStagedContainer->ReleaseStagedContainer();
		}
		else
			spPropContainer->SetStrProperty(nID, bstrOptions);
	}
	SetDirty(TRUE);
	if (bUpdateObjects)
		UpdateObjects(true);
	return TRUE;
}

CAdditionalOptionsPageTemplate inline
STDMETHODIMP CAdditionalOptionsPageType::Dirty( void )
{
	SetDirty(TRUE);
	return S_OK;
}

CAdditionalOptionsPageTemplate inline
STDMETHODIMP CAdditionalOptionsPageType::get_NumObjects( int* pNumObjects )
{
	*pNumObjects = m_nObjects;
	return S_OK;
}

CAdditionalOptionsPageTemplate inline
STDMETHODIMP CAdditionalOptionsPageType::GetObject( int index, IUnknown **ppUnkObject )
{
	if( (DWORD)index >= m_nObjects )
	{
		*ppUnkObject = NULL;
		RETURN_INVALID();
	}
	else
	{
		CComPtr<IUnknown> pRet = m_ppUnk[index];
		*ppUnkObject = pRet.Detach();
		return S_OK;
	}
}

CAdditionalOptionsPageTemplate inline
STDMETHODIMP CAdditionalOptionsPageType::Refresh( void )
{
	return S_OK;	// not a grid, so this method doesn't matter much
}
	
CAdditionalOptionsPageTemplate inline
STDMETHODIMP CAdditionalOptionsPageType::get_CategoryTitle( UINT iLevel, BSTR *pbstrCategory )
{
	if( (iLevel == 0) && TGroupID )
	{
		CComBSTR bstrTitle;
		bstrTitle.LoadString(TGroupID);
		*pbstrCategory = bstrTitle.Detach();
		return S_OK;
	}
	return S_FALSE;
}

CAdditionalOptionsPageTemplate inline
STDMETHODIMP CAdditionalOptionsPageType::Apply(void)
{
	// set the additional options string here
	CComBSTR bstrOptions;
	HWND hwndOpt;
	hwndOpt = ::GetDlgItem(m_hWnd, IDC_ADDOPTIONS);
	wchar_t buf[16000];
	::GetWindowTextW(hwndOpt, buf, 16000);
	bstrOptions = buf;
	
	UINT i;
	HRESULT hr = S_OK;
	for (i = 0; i < m_nObjects; i++)
	{
		CComPtr<IDispatch> pDisp;
		CComQIPtr<IVCCfg> pVcCfg = m_ppUnk[i];
		HRESULT hr2 = pVcCfg->get_Object( &pDisp );
		CComQIPtr<IVCPropertyContainer> spPropContainer = pDisp;
		if (spPropContainer == NULL)
			continue;

		CComPtr<IVCStagedPropertyContainer> spStagedContainer;
		spPropContainer->GetStagedPropertyContainer(VARIANT_TRUE, &spStagedContainer);
		CComQIPtr<IVCPropertyContainer> spStage = spStagedContainer;
		if (spStagedContainer)
		{
			spStagedContainer->HoldStagedContainer();
			if (SUCCEEDED(hr))
			{
				spStage->SetStrProperty(nID, bstrOptions);
				hr = spStagedContainer->CommitPropertyRange(nID, nID);
			}
			spStagedContainer->ReleaseStagedContainer();
		}
		else
			hr = spPropContainer->SetStrProperty(nID, bstrOptions);
	}
	if (SUCCEEDED(hr))
		SetDirty(FALSE);
	return hr;
}

CAdditionalOptionsPageTemplate inline
STDMETHODIMP CAdditionalOptionsPageType::Help(LPCOLESTR pszHelpDir)
{
	CComQIPtr<IVCProjectEngineImpl> pProjEngineImpl = g_pProjectEngine;
	RETURN_ON_NULL(pProjEngineImpl);

	CComBSTR bstrTopic = L"vc.project.AdditionalOptionsPage";

	OpenTopic(pProjEngineImpl, bstrTopic);
	return S_OK;
}

CAdditionalOptionsPageTemplate inline
STDMETHODIMP CAdditionalOptionsPageType::Deactivate(void)
{
	HRESULT hr;

	hr = IPropertyPageImpl<CAdditionalOptionsPage>::Deactivate();
	m_bActive = FALSE;
	DeleteObject( m_hFont );
	return hr;
}

CAdditionalOptionsPageTemplate inline
STDMETHODIMP CAdditionalOptionsPageType::Activate(HWND hWndParent, LPCRECT pRect, BOOL /* bModal */)
{
	ATLTRACE2(atlTraceControls,2,_T("IPropertyPageImpl::Activate\n"));

	if (pRect == NULL)
	{
		ATLTRACE2(atlTraceControls,2,_T("Error : Passed a NULL rect\n"));
		return E_POINTER;
	}
	m_hWnd = CreateW(hWndParent);

	if( m_nObjects == 0 )
		return S_OK;

	m_size.cx = pRect->right - pRect->left;
	m_size.cy = pRect->bottom - pRect->top;

	RECT rectGrid;
	// get the font to use for dlgs
	CComQIPtr<IVCProjectEngineImpl> pProjEngImpl = g_pProjectEngine;
	RETURN_ON_NULL2(pProjEngImpl, E_NOINTERFACE);
	pProjEngImpl->GetDialogFont( FALSE, &m_hFont );
	SetFont( m_hFont );

	HDC dc;
	dc = ::CreateDCW( L"DisPLAY", NULL, NULL, NULL );
	SelectObject( dc, m_hFont );
	// get a base "unit" for sizing the dlg
	SIZE unit;
	GetTextExtentPoint32W( dc, L"X", 1, &unit );
	DeleteDC( dc );

	rectGrid.top = 0;
	rectGrid.left = 0;
	rectGrid.bottom = m_size.cy >= unit.cy * 24 ? m_size.cy : unit.cy * 24;
	rectGrid.right = m_size.cx >= unit.cx * 67 ? m_size.cx : unit.cx * 67;
	
	SetWindowPos( HWND_TOP, &rectGrid, SWP_FRAMECHANGED );

	// set the control font & sizes
	RECT rect;

	// first static text
	CWindow static1( GetDlgItem( IDC_STATIC_ALLOPTIONS ) );
	static1.SetFont( m_hFont );
	rect.left = 0;
	rect.top = (long)(0.5*unit.cy);
	rect.right = (long)(m_size.cx >= unit.cx * 67 ? m_size.cx : unit.cx * 67);
	rect.bottom = (long)rect.top+(1*unit.cy);
	static1.SetWindowPos( HWND_TOP, &rect, 0 );

	// all options edit
	CWindow allopt( GetDlgItem( IDC_ALLOPTIONS ) );
	allopt.SetFont( m_hFont );
	rect.left = 0;
	rect.top = (long)(2*unit.cy);
	rect.right = (long)(m_size.cx >= unit.cx * 67 ? m_size.cx : unit.cx * 67);
	rect.bottom = (long)rect.top+(14*unit.cy);
	allopt.SetWindowPos( HWND_TOP, &rect, 0 );

	// second static text
	CWindow static2( GetDlgItem( IDC_STATIC_ADDOPTIONS ) );
	static2.SetFont( m_hFont );
	rect.left = 0;
	rect.top = (long)(16.5*unit.cy);
	rect.right = (long)(m_size.cx >= unit.cx * 67 ? m_size.cx : unit.cx * 67);
	rect.bottom = (long)rect.top+(1*unit.cy);
	static2.SetWindowPos( HWND_TOP, &rect, 0 );

	// additional options edit
	CWindow addopt( GetDlgItem( IDC_ADDOPTIONS ) );
	addopt.SetFont( m_hFont );
	rect.left = 0;
	rect.top = (long)(18*unit.cy);
	rect.right = (long)(m_size.cx >= unit.cx * 67 ? m_size.cx : unit.cx * 67);
	rect.bottom = (long)rect.top+(5*unit.cy);
	addopt.SetWindowPos( HWND_TOP, &rect, 0 );

	// set the command line box.
	UpdateObjects();
	m_bActive = TRUE;

	return S_OK;
}

CAdditionalOptionsPageTemplate inline
STDMETHODIMP CAdditionalOptionsPageType::GetPageInfo(PROPPAGEINFO* pPageInfo)
{
	CHECK_POINTER_NULL(pPageInfo);
	// call through to atl parent version
	HRESULT hr = IPropertyPageImpl<CAdditionalOptionsPage>::GetPageInfo( pPageInfo );

	// get the font to use for dlgs
	CComQIPtr<IVCProjectEngineImpl> pProjEngImpl = g_pProjectEngine;
	RETURN_ON_NULL2(pProjEngImpl, E_NOINTERFACE);
	HDC dc;
	dc = ::CreateDCW( L"DisPLAY", NULL, NULL, NULL );
	HFONT hFont;
	pProjEngImpl->GetDialogFont( FALSE, &hFont );
	SelectObject( dc, hFont );
	// get a base "unit" for sizing the dlg
	SIZE unit;
	GetTextExtentPoint32W( dc, L"X", 1, &unit );
	DeleteObject( hFont );
	DeleteDC( dc );

	pPageInfo->size.cx = m_size.cx >= unit.cx * 67 ? m_size.cx : unit.cx * 67;
	pPageInfo->size.cy = m_size.cy >= unit.cy * 24 ? m_size.cy : unit.cy * 24;

	return hr;
}

CAdditionalOptionsPageTemplate inline
STDMETHODIMP CAdditionalOptionsPageType::SetObjects(ULONG nObjects, IUnknown **ppUnk)
{
	// call base-class implementation
	HRESULT hr = IPropertyPageImpl<CAdditionalOptionsPage>::SetObjects( nObjects, ppUnk );
	RETURN_ON_FAIL(hr);

	// update the objects if we have ever been activated.
	if( m_bActive )
		hr = UpdateObjects();
		
	return hr;
}

CAdditionalOptionsPageTemplate inline
STDMETHODIMP CAdditionalOptionsPageType::TranslateAccelerator(MSG *pMsg)
{
	if ((pMsg->message < WM_KEYFIRST || pMsg->message > WM_KEYLAST) &&
		(pMsg->message < WM_MOUSEFIRST || pMsg->message > WM_MOUSELAST))
		return S_FALSE;

	return (::IsDialogMessage(m_hWnd, pMsg)) ? S_OK : S_FALSE;
}

CAdditionalOptionsPageTemplate inline
HRESULT CAdditionalOptionsPageType::UpdateObjects( bool bUpdateAllOnly = false )
{
	CComBSTR	bstrOptionsFirst;
		CComBSTR bstrAllOptions;

	UINT i;
	for (i = 0; i < m_nObjects; i++)
	{
		CComPtr<IDispatch> pDisp;
		CComQIPtr<IVCCfg> pVcCfg = m_ppUnk[i];
		HRESULT hr = pVcCfg->get_Object( &pDisp );
		CComQIPtr<VCConfiguration> pCfg = pDisp;
		CComQIPtr<VCFileConfiguration> pFileCfg = pDisp;
		
		CComQIPtr<IVCToolImpl> pToolImpl;
#ifdef AARDVARK
		CComQIPtr<VCStyleSheet> pStyle = pDisp;
		if( pStyle != NULL )
		{
			CComPtr<IDispatch> pDispTools;
			CComQIPtr<IVCCollection> pCollTools;

			if (pCfg && (pStyle->get_Tools(&pDispTools)==S_OK))
			{
				pCollTools = pDispTools;
				CComPtr<IEnumVARIANT> pEnumTools;
				hr = pCollTools->_NewEnum((IUnknown**)&pEnumTools);
				pEnumTools->Reset();
				while (TRUE)
				{
					CComVariant var;
					hr = pEnumTools->Next(1, &var, NULL);
					BREAK_ON_DONE(hr);
					CComQIPtr<I> pTool;
					pTool = var.pdispVal;
					if( pTool )
					{
						pToolImpl = pTool;
						break;
					}
				}
			}
			if( pToolImpl == NULL )
				return S_OK;
		}
#endif	// AARDVARK
		if( pCfg != NULL )
		{
			CComPtr<IDispatch> pDispTools;
			CComQIPtr<IVCCollection> pCollTools;

			if (pCfg && (pCfg->get_Tools(&pDispTools)==S_OK))
			{
				pCollTools = pDispTools;
				CComPtr<IEnumVARIANT> pEnumTools;
				hr = pCollTools->_NewEnum((IUnknown**)&pEnumTools);
				pEnumTools->Reset();
				while (TRUE)
				{
					CComVariant var;
					hr = pEnumTools->Next(1, &var, NULL);
					BREAK_ON_DONE(hr);
					CComQIPtr<I> pTool;
					pTool = var.pdispVal;
					if( pTool )
					{
						pToolImpl = pTool;
						break;
					}
				}
			}
			RETURN_ON_NULL2(pToolImpl, S_OK);
		}
		else if( pFileCfg != NULL )
		{

			CComPtr<IDispatch> pDisp;
		
			hr = pFileCfg->get_Tool( &pDisp );
			if( FAILED( hr ) ) 
				continue;
		
			// if this file has a tool
			pToolImpl = pDisp;
			if( pToolImpl == NULL )
				continue;
		}

		CComQIPtr<IVCPropertyContainer> pContainer = pDisp;
		CComQIPtr<IVCPropertyContainer> spLiveContainer;
		if (pContainer)
		{
			CComPtr<IVCStagedPropertyContainer> spStagedPropContainer;
			pContainer->GetStagedPropertyContainer(VARIANT_TRUE, &spStagedPropContainer);
			if (spStagedPropContainer)
				spLiveContainer = spStagedPropContainer;
		}
		if (spLiveContainer == NULL)
			spLiveContainer = pDisp;

		// set the additional options string here
		if( m_nObjects == 1 )
		{
			hr = pToolImpl->GetCommandLineOptionsForDisplay(spLiveContainer, VARIANT_FALSE /* !include additional options */, 
				&bstrAllOptions);
			CComBSTR bstrParentOptions;
			HRESULT hr2 = pToolImpl->GetAdditionalOptionsInternal(spLiveContainer, VARIANT_TRUE, VARIANT_TRUE, 
				&bstrParentOptions);
			if (bstrParentOptions.Length())
			{
				if (bstrAllOptions.Length())
					bstrAllOptions += L" ";
				bstrAllOptions += bstrParentOptions;
			}
		}
		else if( bstrAllOptions == NULL )
		{
			bstrAllOptions.LoadString(IDS_MULTISELECT);
		}
		
		if (!bUpdateAllOnly)
		{
			CComBSTR bstrOptions;
			hr = pToolImpl->GetAdditionalOptionsInternal(spLiveContainer, VARIANT_FALSE, VARIANT_FALSE, &bstrOptions);

			if( i == 0 )
			{
				bstrOptionsFirst = bstrOptions;
			}
			else if ( bstrOptionsFirst != bstrOptions )
			{
				bstrOptionsFirst = "";
				break;
			}
		}

	}
	HWND hwndOpt;
	if (!bUpdateAllOnly)
	{
		hwndOpt = ::GetDlgItem(m_hWnd, IDC_ADDOPTIONS);
		::SetWindowTextW(hwndOpt, bstrOptionsFirst);
	}

	hwndOpt = ::GetDlgItem(m_hWnd, IDC_ALLOPTIONS);
	::SetWindowTextW(hwndOpt, bstrAllOptions);

	return S_OK;
}

////////////////////////////////////////////////////////////////////////////////////////////////
// CBaseEditStringDlg

CBaseEditStringDlgTemplate inline
CBaseEditStringDlgType::CBaseEditStringDlg(IVCPropertyContainer* pPropCnt, wchar_t* wszName, wchar_t* wszProp) :
	m_bstrName( wszName ),
	m_bstrProperty( wszProp ),
	m_hFont(NULL),
	m_bExpanded(false),
	m_nExpansionHeight(150),
	m_bInitialSizeDone(false),
	m_wOriginalHeight(-1),
	m_wHeight(-1),
	m_wWidth(-1),
	m_xBorder(-1),
	m_yBorder(-1),
	m_yCaptionSize(-1),
	m_buttonWidth(10),
	m_hpenBtnHilite(NULL),
	m_hpenBtnShadow(NULL),
	m_cxResizeBox(0),
	m_bInPaint(false)

{
	m_pPropContainer = pPropCnt;
}

CBaseEditStringDlgTemplate inline
LRESULT CBaseEditStringDlgType::OnInitDialog( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled )
{
	// set the caption
	::SetWindowTextW( m_hWnd, m_bstrName );

	// set the appropriate font
	CComQIPtr<IVCProjectEngineImpl> pProjEngImpl = g_pProjectEngine;
	RECT rect;
	if( pProjEngImpl )
	{
		// get the font to use
		pProjEngImpl->GetDialogFont(FALSE, &m_hFont );
		SetFont( m_hFont );

		DoSetWindowFont(IDC_INSERTMACRO);
		DoSetWindowFont(IDC_MACROPICKER);
		DoSetWindowFont(IDCANCEL);
		DoSetWindowFont(IDOK);
		DoSetWindowFont(IDC_SETTINGSEDIT_HELP);

		SetupScreenUnits();

		// set the dlg size
		// 27x21.5 'units'
		MINMAXINFO minMax;
		DoOnMinMaxInfo(&minMax);
		rect.left = 0;
		rect.top = 0;
		rect.right = 0;
		rect.bottom = 0;
		CStringW strRoot  = CVCProjectEngine::s_bstrAlternateRegistryRoot + "\\VC";
		DWORD dwWidth = 0;
		HRESULT hr = GetRegIntW(strRoot, GetSavedWidthString(), &dwWidth);
		rect.right = dwWidth;
		if (rect.right <= 0)
			rect.right = minMax.ptMinTrackSize.x;

		DWORD dwHeight = 0;
		hr = GetRegIntW(strRoot, GetSavedHeightString(), &dwHeight);
		rect.bottom = (DWORD)dwHeight;
		if (rect.bottom <= 0)
			rect.bottom = minMax.ptMinTrackSize.y;
		m_wOriginalHeight = m_wHeight = (WORD)rect.bottom;
		m_wWidth = (WORD)rect.right;
	}
	else
	{
		GetWindowRect(&rect);
		m_wOriginalHeight = m_wHeight = (WORD)(rect.bottom - rect.top);
		m_wWidth = (WORD)(rect.right - rect.left);
	}

	DoChildInit();

	// hide the Macros button
	::ShowWindow(GetDlgItem(IDC_INSERTMACRO), SW_HIDE);

	SetWindowPos( HWND_TOP, &rect, 0 );
	HWND hwndPropDlg = ::GetActiveWindow();
	CenterWindow( hwndPropDlg );

	// Establish the dlg controls' contents
	Reset();

	ShowWindow( SW_SHOWNORMAL );
	SendDlgItemMessage(GetMacroPickerParent(), WM_SETFOCUS, 0, 0);

	DoOnPaint();

	bHandled = TRUE;
	return FALSE;  // don't let the system set the focus
}

CBaseEditStringDlgTemplate inline
void CBaseEditStringDlgType::DoSetWindowFont(int idControl)
{
	CWindow wnd;
	wnd.Attach(GetDlgItem(idControl));
	if (wnd)
		wnd.SetFont(m_hFont);
}

CBaseEditStringDlgTemplate inline
void CBaseEditStringDlgType::SizeButton(int idControl, double left, double top, double right, double bottom)
{
	RECT rect;
	CWindow wnd( GetDlgItem( idControl ) );
	rect.right = m_wWidth - m_xBorder - (long)(right*m_unit.cx);
	if (rect.right < 0)
		rect.right = 0;
	rect.left = (long)(rect.right - left*m_unit.cx);
	if (rect.left < 0)
		rect.left = 0;
	rect.top = (long)(m_wHeight - m_yBorder - top*m_unit.cy);
	if (rect.top < 0)
		rect.top = 0;
	rect.bottom = (long)(rect.top+(bottom*m_unit.cy));
	wnd.SetWindowPos( HWND_TOP, &rect, 0 );
}

CBaseEditStringDlgTemplate inline
void CBaseEditStringDlgType::SizeSubWindow(int idControl, double left, double top, double right, double bottom)
{
	CWindow wnd( GetDlgItem( idControl ) );
	RECT rect;
	rect.left = (long)(left*m_unit.cx);
	rect.top = (long)(top*m_unit.cy);
	rect.right = m_wWidth - m_xBorder - (long)(right*m_unit.cx);
	if (rect.right < rect.left)
		rect.right = rect.left;
	rect.bottom = rect.top+(long)(bottom*m_unit.cy);
	wnd.SetWindowPos( HWND_TOP, &rect, 0 );
}

CBaseEditStringDlgTemplate inline
void CBaseEditStringDlgType::SetExpansionHeight()
{
	if (m_bExpanded)
		m_nExpansionHeight = (long)(0.5*(m_wHeight - 5.0*m_unit.cy));
	else
		m_nExpansionHeight = (long)(m_wHeight - 5.0*m_unit.cy);
}

CBaseEditStringDlgTemplate inline
double CBaseEditStringDlgType::GetExpansionExtent()
{
	if (m_bExpanded)
		return m_nExpansionHeight;
	else
		return 0.0;
}

CBaseEditStringDlgTemplate inline
LRESULT CBaseEditStringDlgType::DoOnSize( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled )
{
	WORD wOldWidth = m_wWidth;
	WORD wOldHeight = m_wHeight;

	m_wWidth = LOWORD( lParam );
	m_wHeight = HIWORD( lParam );

	// remember the initial size when the dlg is first sized (on creation)
	if( !m_bInitialSizeDone )
	{
		m_wOriginalHeight = m_wHeight;
		m_bInitialSizeDone = true;
		m_hpenBtnShadow = ::CreatePen(PS_SOLID, 0, ::GetSysColor(COLOR_BTNSHADOW));
		m_hpenBtnHilite = ::CreatePen(PS_SOLID, 0, ::GetSysColor(COLOR_BTNHIGHLIGHT));
	}

	SetupScreenUnits();

	if (m_wWidth != wOldWidth || m_wHeight != wOldHeight)
		InvalidateRect(NULL);	// force a repaint

	// help button
	SizeButton(IDC_SETTINGSEDIT_HELP, m_buttonWidth, 2.25, 0.75, 2);

	// cancel button
	SizeButton(IDCANCEL, m_buttonWidth, 2.25, m_buttonWidth+1.25, 2);

	// ok button
	SizeButton(IDOK, m_buttonWidth, 2.25, 2*m_buttonWidth+1.75, 2);

	if (m_bExpanded && MacroPickerSupported())
		SizeButton(IDC_INSERTMACRO, m_buttonWidth, 2.25, 3*m_buttonWidth+2.25, 2);

	// macro button
	SizeButton(IDC_MACROPICKER, m_buttonWidth, 4.75, 0.75, 2);

	// set up the window area the child dialog gets to mess with
	SetExpansionHeight();

	// macro picker list sizes to fit the remaining space below the child windows and above the buttons, so can't 
	// use either SizeButton or SizeSubWindow to size it
	if (m_bExpanded)
	{
		RECT rect;
		rect.left = (long)(EDITSTRINGS_RESIZE_EDGE*m_unit.cx);
		rect.top = GetMacroPickerTop();
		rect.right = m_wWidth - m_xBorder - (long)(EDITSTRINGS_RESIZE_EDGE*m_unit.cx);
		if (rect.right < rect.left)
			rect.right = rect.left;
		rect.bottom = (long)(m_wHeight - m_yBorder - 5.5*m_unit.cy);
		if (rect.bottom < rect.top)
			rect.bottom = rect.top;
		m_wndMacroPicker.SetWindowPos( HWND_TOP, &rect, 0 );
	}

	// do OnSize for anything in the child dialog; this puts the child's subwindows FIRST in the tab order...
	DoChildOnSize();

	DoOnPaint();

	return 0;
}

CBaseEditStringDlgTemplate inline
LRESULT CBaseEditStringDlgType::DoOnPaint()
{
	PAINTSTRUCT paintStruct;
	BeginPaint(&paintStruct);
	// that little resize thingie in the lower right corner
	if (m_hpenBtnShadow && m_hpenBtnHilite)
	{
		RECT rectResizer;
		GetClientRect(&rectResizer);
		rectResizer.top = rectResizer.bottom - m_cxResizeBox;
		rectResizer.left = rectResizer.right - m_cxResizeBox;
		HDC dc = GetDC();
		HGDIOBJ hObjOld = SelectObject(dc, m_hpenBtnHilite);
		int i;
		for (i = 0; i < m_cxResizeBox; i += 4)
		{
			MoveToEx(dc, rectResizer.left+i, rectResizer.bottom, NULL);
			LineTo(dc, rectResizer.left+m_cxResizeBox, rectResizer.bottom-m_cxResizeBox+i);
		}

		SelectObject(dc, m_hpenBtnShadow);
		for (i = 1; i < m_cxResizeBox; i += 4)
		{
			MoveToEx(dc, rectResizer.left+i, rectResizer.bottom, NULL);
			LineTo(dc, rectResizer.left+m_cxResizeBox, rectResizer.bottom-m_cxResizeBox+i);
		}
		for (i = 2; i < m_cxResizeBox; i += 4)
		{
			MoveToEx(dc, rectResizer.left+i, rectResizer.bottom, NULL);
			LineTo(dc, rectResizer.left+m_cxResizeBox, rectResizer.bottom-m_cxResizeBox+i);
		}

		SelectObject(dc, hObjOld);
		ReleaseDC(dc);
	}

	EndPaint(&paintStruct);
	return 0;
}

CBaseEditStringDlgTemplate inline
void CBaseEditStringDlgType::SetupScreenUnits()
{
	CComBSTR bstrOK, bstrCancel, bstrHelp, bstrMacros, bstrInsert;
	RetrieveDynamicInfo(IDOK, bstrOK);
	RetrieveDynamicInfo(IDCANCEL, bstrCancel);
	RetrieveDynamicInfo(IDC_SETTINGSEDIT_HELP, bstrHelp);
	RetrieveDynamicInfo(IDC_MACROPICKER, bstrMacros);
	if (MacroPickerSupported())
		RetrieveDynamicInfo(IDC_INSERTMACRO, bstrInsert);

	HDC dc = GetDC();
	SelectObject( dc, m_hFont );
	// get a base "unit" for sizing the dlg
	GetTextExtentPoint32W( dc, L"X", 1, &m_unit );

	// figure out the minimum sizing for the buttons
	SIZE unitOK, unitCancel, unitHelp, unitMacros, unitInsert;
	GetTextExtentPoint32W( dc, bstrOK, bstrOK.Length(), &unitOK );
	GetTextExtentPoint32W( dc, bstrCancel, bstrCancel.Length(), &unitCancel );
	GetTextExtentPoint32W( dc, bstrHelp, bstrHelp.Length(), &unitHelp );
	GetTextExtentPoint32W( dc, bstrMacros, bstrMacros.Length(), &unitMacros );
	if (MacroPickerSupported())
		GetTextExtentPoint32W( dc, bstrInsert, bstrInsert.Length(), &unitInsert );
	else
		unitInsert.cx = unitInsert.cy = 0;
	ReleaseDC(dc);

	int minButtonWidth = unitOK.cx;
	if (unitCancel.cx > minButtonWidth)
		minButtonWidth = unitCancel.cx;
	if (unitHelp.cx > minButtonWidth)
		minButtonWidth = unitHelp.cx;
	if (unitMacros.cx > minButtonWidth)
		minButtonWidth = unitMacros.cx;
	if (unitInsert.cx > minButtonWidth)
		minButtonWidth = unitInsert.cx;
	if (minButtonWidth > 9 * m_unit.cx)
		m_buttonWidth = (long)(minButtonWidth / m_unit.cx) + 2;

	m_xBorder = GetSystemMetrics( SM_CXSIZEFRAME );
	m_yBorder = GetSystemMetrics( SM_CYSIZEFRAME );
	m_yCaptionSize = GetSystemMetrics( SM_CYCAPTION );
	m_cxResizeBox = (3*GetSystemMetrics(SM_CXVSCROLL)+1)/4;
}

CBaseEditStringDlgTemplate inline
void CBaseEditStringDlgType::RetrieveDynamicInfo(long id, CComBSTR& bstrBuffer)
{
	CStringW strBuffer;
	CWindow editProp(::GetDlgItem(m_hWnd, id));
	int nLen = editProp.GetWindowTextLength();
	wchar_t* szBuf = strBuffer.GetBuffer(nLen+2);
	if (szBuf)
	{
		::GetDlgItemTextW(m_hWnd, id, szBuf, nLen+1);
		szBuf[nLen] = L'\0';
		strBuffer.ReleaseBuffer();
	}
	bstrBuffer = strBuffer;
}

CBaseEditStringDlgTemplate inline
LRESULT CBaseEditStringDlgType::DoOnSetFocus()
{
	CWindow editProp( GetDlgItem( GetMacroPickerParent() ) );
	// de-select
	editProp.SendMessage( EM_SETSEL, -1, 0 );
	return 0;
}

CBaseEditStringDlgTemplate inline
LRESULT CBaseEditStringDlgType::DoOnMacros()
{
	RECT rectParent;
	GetWindowRect(&rectParent);
	bool bFinish = false;
	int nWiggle = 0;

	CStringW strMacros;
	if (m_bExpanded)
	{
		if (m_pMacroPicker)
		{
			SetExpansionHeight();
			::ShowWindow(m_wndMacroPicker, SW_HIDE);
			m_bExpanded = false;
			if (!strMacros.LoadString(IDS_MACROS_CONTRACTED))
				strMacros = "&Macros>>";
			::ShowWindow(GetDlgItem(IDC_INSERTMACRO), SW_HIDE);
			ResetParentBottom(rectParent);
			bFinish = true;
		}
	}
	else
	{
		if (!m_wndMacroPicker)
		{
			RECT rectChild = { 3, 210, 150, 210 };
			m_wndMacroPicker.Create(m_hWnd, rectChild, _T("MacroPicker.VCMacroPicker"), 
				WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN, 0, IDC_MACROPICKER_LIST);
			m_pMacroPicker.Release();
			m_wndMacroPicker.QueryControl(__uuidof(IVCMacroPicker), (void **)&m_pMacroPicker);
			if (m_pMacroPicker)
			{
				HWND hwndPickerParent = GetMacroPickerParentEditWindow();
				CComBSTR bstrName, bstrValue;
				bstrName.LoadString(IDS_NAME);
				bstrValue.LoadString(IDS_VALUE);
				m_pMacroPicker->Initialize(m_pPropContainer, g_pProjectEngine, (LONG_PTR)hwndPickerParent,
					bstrName, bstrValue);
				m_wndMacroPicker.MoveWindow(&rectChild);
			}
		}
		if (m_pMacroPicker)
		{
			SetExpansionHeight();
			m_bExpanded = true;
			if (!strMacros.LoadString(IDS_MACROS_EXPANDED))
				strMacros = "&Macros<<";
			if (MacroPickerSupported())
				::ShowWindow(GetDlgItem(IDC_INSERTMACRO), SW_SHOW);
			::ShowWindow(m_wndMacroPicker, SW_SHOW);
			rectParent.bottom += m_nExpansionHeight;
			RECT rectWorkArea;
			SystemParametersInfo(SPI_GETWORKAREA, 0, &rectWorkArea, 0);
			if (rectWorkArea.bottom < rectParent.bottom)
			{
				rectParent.top = rectParent.top - (rectParent.bottom - rectWorkArea.bottom);
				if (rectParent.top < rectWorkArea.top)
				{
					m_nExpansionHeight -= rectWorkArea.top - rectParent.top;
					rectParent.top = rectWorkArea.top;
				}
				rectParent.bottom = rectWorkArea.bottom;
			}
			bFinish = true;
		}
	}
	if (bFinish)
	{
		    CString strMacroText = strMacros;
		SetDlgItemText(IDC_MACROPICKER, strMacroText);
		MoveWindow(&rectParent);
	}
	
	return 0;
}

CBaseEditStringDlgTemplate inline
LRESULT CBaseEditStringDlgType::DoOnInsertMacro()
{
	if (m_pMacroPicker)
		m_pMacroPicker->OnInsertMacro();
	return 0;
}

CBaseEditStringDlgTemplate inline
LRESULT CBaseEditStringDlgType::DoOnOk()
{
	RetrieveInfo();

	// the caller will now need to call GetPropertyString() to retrieve the
	// data...
	
	DoCleanup();

	EndDialog( IDOK );
	return 0;
}

CBaseEditStringDlgTemplate inline
LRESULT CBaseEditStringDlgType::DoOnCancel()
{
	DoCleanup();

	EndDialog( IDCANCEL );
	return 0;
}

CBaseEditStringDlgTemplate inline
void CBaseEditStringDlgType::DoCleanup()
{
	// take care of the macro dlg
	ReleaseMacroPicker();

	if (m_hFont)
	{
		DeleteObject( m_hFont );
		m_hFont = NULL;
	}

	if (m_hpenBtnHilite)
	{
		DeleteObject(m_hpenBtnHilite);
		m_hpenBtnHilite = NULL;
	}
	if (m_hpenBtnShadow)
	{
		DeleteObject(m_hpenBtnShadow);
		m_hpenBtnShadow = NULL;
	}

	CStringW strRoot  = CVCProjectEngine::s_bstrAlternateRegistryRoot + "\\VC";
	DWORD dwWidth = (DWORD)m_wWidth;
	WriteRegIntW(strRoot, GetSavedWidthString(), dwWidth);
	DWORD nHeight = (DWORD)m_wHeight;
	if (m_bExpanded)
		nHeight -= m_nExpansionHeight;
	WriteRegIntW(strRoot, GetSavedHeightString(), nHeight);
}

CBaseEditStringDlgTemplate inline
LRESULT CBaseEditStringDlgType::DoOnHelp()
{
	CComQIPtr<IVCProjectEngineImpl> pProjEngineImpl = g_pProjectEngine;
	RETURN_ON_NULL2(pProjEngineImpl, 1);
	OpenTopic( pProjEngineImpl, CComBSTR( L"VC.Project.NotAProp.Edit" ) );

	return 0;
}

CBaseEditStringDlgTemplate inline
LRESULT CBaseEditStringDlgType::DoOnMinMaxInfo( LPMINMAXINFO lpMinMax )
{
	RECT rectWorkArea;
	SystemParametersInfo(SPI_GETWORKAREA, 0, &rectWorkArea, 0);
	lpMinMax->ptMinTrackSize.x = (long)((4*m_buttonWidth + 3.5)*m_unit.cx + 2*m_xBorder); // min track width
	if (lpMinMax->ptMinTrackSize.x > rectWorkArea.right - rectWorkArea.left)
		lpMinMax->ptMinTrackSize.x = rectWorkArea.right - rectWorkArea.left;
	if (m_bExpanded)	// min track height
		lpMinMax->ptMinTrackSize.y = (long)(44*m_unit.cy + 2*m_yBorder + m_yCaptionSize);
	else
		lpMinMax->ptMinTrackSize.y = (long)(22*m_unit.cy + 2*m_yBorder + m_yCaptionSize);
	if (lpMinMax->ptMinTrackSize.y > rectWorkArea.bottom - rectWorkArea.top)
		lpMinMax->ptMinTrackSize.y = rectWorkArea.bottom - rectWorkArea.top;
	return 0;
}

CBaseEditStringDlgTemplate inline
void CBaseEditStringDlgType::ReleaseMacroPicker()
{
	if (!m_pMacroPicker)
		return;

	HWND hwndMacroPicker = NULL;
	m_pMacroPicker->get_HWND((LONG_PTR*)&hwndMacroPicker);
	if (hwndMacroPicker)
		::SendMessage(hwndMacroPicker, WM_CLOSE, 0, 0);
	m_pMacroPicker.Release();
}

CBaseEditStringDlgTemplate inline
void CBaseEditStringDlgType::CleanUpString( CComBSTR & bstrVal, CStringW& strPreferredSeparator, CStringW& strAllSeparators )
{
	CStringW strPropIn = bstrVal;
	strPropIn.TrimLeft();
	strPropIn.TrimRight();

	// if we have an empty string, don't bother
	if( strPropIn.IsEmpty() )
		return;

	CStringW strPropOut;
	// first replace any non-preferred separators with preferred ones;  make sure to respect anything that has been quoted
	int nMax = strAllSeparators.GetLength();
	wchar_t cPreferred = strPreferredSeparator.IsEmpty() ? L';' : strPreferredSeparator[0];
	int nLen = strPropIn.GetLength();
	bool bInSingleQuote = false, bInDoubleQuote = false, bInWhiteSpace = false, bInSeparator = false;
	bool bAtStart = true, bNeedSeparator = false, bRecentChange = false;
	int idx = 0;

	for (idx = 0; idx < nLen; idx++)
	{
		if (strPropIn[idx] == L'"')
		{
			bInDoubleQuote = !bInDoubleQuote;
		}
		else if (strPropIn[idx] == L'\'')
		{
			bInSingleQuote = !bInSingleQuote;
		}

		if (bInSingleQuote || bInDoubleQuote)
		{
			if (bNeedSeparator)
				strPropOut += cPreferred;
			strPropOut += strPropIn[idx];
			bAtStart = false;
			bNeedSeparator = false;
			continue;
		}

		if (strPropIn[idx] == cPreferred || strPropIn[idx] == L'\r' || strPropIn[idx] == L'\n')
		{
			bInWhiteSpace = true;
			bNeedSeparator = true;
			continue;
		}
		
		bInSeparator = false;
		for (int idx2 = 1; idx2 < nMax && !bInSeparator; idx2++)
		{
			wchar_t cCurrent = strAllSeparators[idx2];
			if (strPropIn[idx] == cCurrent)
				bInSeparator = true;
		}

		if (bInSeparator)
		{
			bInWhiteSpace = true;
			bNeedSeparator = true;
			continue;
		}

		if (bNeedSeparator)
		{
			strPropOut += cPreferred;
			bNeedSeparator = false;
		}

		strPropOut += strPropIn[idx];
	}
	
	// save it away
	bstrVal = strPropOut;
}

////////////////////////////////////////////////////////////////////////////////////////////////
// CMultiLineDlgBase

CMultiLineDlgBaseTemplate inline
CMultiLineDlgBaseType::CMultiLineDlgBase( IVCPropertyContainer *pPropCnt, long propid, wchar_t *wszName, wchar_t *wszProp, wchar_t *wszInherit ) :
	CBaseEditStringDlg<T>(pPropCnt, wszName, wszProp),	
	m_lPropId( propid ),
	m_bstrInherit( wszInherit ),
	m_bUserSetInherit( false )
{
	if (pPropCnt)
	{
		CComBSTR bstrPreferred, bstrAll;
		pPropCnt->GetMultiPropSeparator(propid, &bstrPreferred, &bstrAll);
		m_strPreferredSeparator = bstrPreferred;
		m_strAllSeparators = bstrAll;
	}
	else
	{
		m_strPreferredSeparator = L";";
		m_strAllSeparators = L";,";
	}
}

CMultiLineDlgBaseTemplate inline
void CMultiLineDlgBaseType::RetrieveInfo()
{
	CComBSTR bstrFixedData;
	RetrieveSpecificInfo(bstrFixedData);

	// if the user didn't type an inherit macro into the field, use the checkbox
	if( !HasInheritMacros( bstrFixedData ) )
	{
		// check inherit state
		if( !m_bUserSetInherit )
		{
			UINT state = IsDlgButtonChecked( IDC_INHERIT );
			if( state == BST_UNCHECKED )
			{
				wchar_t cPreferred = m_strPreferredSeparator.IsEmpty() ? L';' : m_strPreferredSeparator[0];
				if( bstrFixedData.Length() > 0 && bstrFixedData.m_str[bstrFixedData.Length()-1] != cPreferred )
					bstrFixedData.Append(cPreferred);
				bstrFixedData.Append( L"$(NOINHERIT)" );
			}
		}
	}
	m_bstrProperty = bstrFixedData;
	CleanUpString( m_bstrProperty, m_strPreferredSeparator, m_strAllSeparators );
}

CMultiLineDlgBaseTemplate inline
void CMultiLineDlgBaseType::CheckForInheritOnOpen( CComBSTR & bstrVal )
{
	CStringW strProp = bstrVal;
	int pos;

	// ensure no trailing whitespace
	strProp.TrimRight();

	// look for $(NOINHERIT) first, as it overrides $(INHERIT)
	pos = FindNoCaseW( strProp, L"$(NOINHERIT)" );
	if( pos != -1 )
	{
		// strip it out
		bstrVal = strProp.Left( pos ) + strProp.Right( lstrlenW( bstrVal ) - (pos + lstrlenW( L"$(NOINHERIT)" )) );
		// check box is un-checked by default...
		return;
	}
	// look for $(INHERIT) at the end of the list 
	// find the last newline
	pos = strProp.ReverseFind( L'\n' );
	if( pos != -1 )
	{
		// search forward from there for the macro
		int pos2 = FindNoCaseW( strProp, L"$(INHERIT)", pos );
		if( pos2 != -1 )
		{
			// remove it
 			bstrVal = strProp.Left( pos - 1 );
			// check box is un-checked by default...
			CheckDlgButton( IDC_INHERIT, BST_CHECKED );
			return;
		}
	}
	else
	{
		// found no newlines, so check the whole string 
		// search forward from there for the macro
		int pos2 = FindNoCaseW( strProp, L"$(INHERIT)" );
		if( pos2 != -1 )
		{
			// remove it
			bstrVal = L"";
			// check box is un-checked by default...
			CheckDlgButton( IDC_INHERIT, BST_CHECKED );
			return;
		}
	}
	// look for $(INHERIT) in the rest of the string
	pos = FindNoCaseW( strProp, L"$(INHERIT)" );
	// if we find it in the middle somewhere, grey out the inherit check box
	if( pos != -1 )
	{
		// grey out the checkbox
		CWindow checkBox( GetDlgItem( IDC_INHERIT ) );
		checkBox.SetWindowLong( GWL_STYLE, BS_AUTOCHECKBOX|WS_DISABLED|WS_VISIBLE );
		// set our internal variable
		m_bUserSetInherit = true;
		// (and DON'T remove the macro!)
		return;
	}
	// no macros
	CheckDlgButton( IDC_INHERIT, BST_CHECKED );
	return;
}

CMultiLineDlgBaseTemplate inline
bool CMultiLineDlgBaseType::HasInheritMacros( BSTR bstrVal )
{
	// look for inherit macros
	CStringW strVal = bstrVal;
	if( FindNoCaseW( strVal, L"$(NOINHERIT)" ) != -1 || FindNoCaseW( strVal, L"$(INHERIT)" ) != -1 )
		// return true if we find them
		return true;
	else
		return false;
}

CMultiLineDlgBaseTemplate inline
void CMultiLineDlgBaseType::DoSharedChildOnSize(double& dTotalUnits, double& dWindowUnits)
{
	dTotalUnits = (m_nExpansionHeight - m_yBorder)/(m_unit.cy ? m_unit.cy : 1.0);
	dWindowUnits = dTotalUnits - 3.4;	// this is what's intended for resizable areas

	// inherit check box
	HWND hwndInherit = GetDlgItem( IDC_INHERIT );
	::SetWindowTextW( hwndInherit, m_bstrInherit );
	// figure out how much space we need for the 'Inherit from project' string
	SIZE unit;
	bool bNeedOwnLine = false;
	if (!m_bExpanded)
	{
		HDC dc = GetDC();
		SelectObject( dc, GetFont() );
		GetTextExtentPoint32W( dc, m_bstrInherit, m_bstrInherit.Length(), &unit );
		ReleaseDC(dc);
		if (unit.cx > m_wWidth - (long)(m_unit.cx * (EDITSTRINGS_RESIZE_EDGE + m_buttonWidth + 1.5)))
			bNeedOwnLine = true;
	}

	double lastHeight = dTotalUnits - 1.0;
	double inheritHeight = lastHeight;
	double heightIncrement = 1.0;
	double inheritRight = 0.25;
	if (m_bExpanded)
		inheritHeight += 0.5;
	else if (bNeedOwnLine)
	{
		dWindowUnits -= 0.25;	// need a little extra room for the string...
		lastHeight -= 0.5;
	}
	else	// line it up with the Macros button if the Macro Picker isn't there...
	{
		inheritHeight += 1.5;
		dWindowUnits += 0.5;	// don't need this bit left out for the inherit check box
		lastHeight += 0.5;
		inheritRight = m_buttonWidth + 1.5;
	}
	SizeSubWindow(IDC_INHERIT, EDITSTRINGS_RESIZE_EDGE+0.25, inheritHeight, inheritRight, 1.0);

	// inheritable values read-only edit field
	heightIncrement = (6.5/14.3)*dWindowUnits;
	lastHeight -= heightIncrement;
	SizeSubWindow(IDC_INHERITED, EDITSTRINGS_RESIZE_EDGE, lastHeight, EDITSTRINGS_RESIZE_EDGE, heightIncrement);

	// static inherit text
	lastHeight -= 1.3;
	heightIncrement = 1.0;
	SizeSubWindow(IDC_STATIC_INHERIT, EDITSTRINGS_RESIZE_EDGE, lastHeight, EDITSTRINGS_RESIZE_EDGE, heightIncrement);
}

CMultiLineDlgBaseTemplate inline
void CMultiLineDlgBaseType::DoSharedChildInit()
{
	if (!m_hFont)
		return;

	DoSetWindowFont(IDC_STATIC_INHERIT);
	DoSetWindowFont(IDC_INHERITED);
	DoSetWindowFont(IDC_INHERIT);
}

CMultiLineDlgBaseTemplate inline
void CMultiLineDlgBaseType::DoSharedReset(CComBSTR& bstrFormatted)
{
	CleanUpString( m_bstrProperty, m_strPreferredSeparator, m_strAllSeparators );
	// replace separators with newlines
 	ReplaceSeparatorsWithNewlines( m_bstrProperty, bstrFormatted, m_strPreferredSeparator );
	CheckForInheritOnOpen( bstrFormatted );

	// set up the inheritable values read-only edit field
	CWindow inheritable( GetDlgItem( IDC_INHERITED ) );

	// get the parents values
	CComBSTR bstrProp;
	m_pPropContainer->GetMultiProp( m_lPropId, m_strPreferredSeparator, VARIANT_TRUE, &bstrProp );
	
	CleanUpString( bstrProp, m_strPreferredSeparator, m_strAllSeparators );
	// replace separators with newlines
	CComBSTR bstrFormatted2;
 	ReplaceSeparatorsWithNewlines( bstrProp, bstrFormatted2, m_strPreferredSeparator );
	CheckForInheritOnOpen( bstrFormatted2 );
 	::SetWindowTextW( inheritable, bstrFormatted2 );
}

CMultiLineDlgBaseTemplate inline /* static */
int CMultiLineDlgBaseType::FindNoCaseW( const CStringW & str, const wchar_t* const substr, int pos = 0 )
{
	CStringW strFull = str;
	strFull.MakeLower();
	CStringW strSub = substr;
	strSub.MakeLower();
	return strFull.Find( strSub, pos );
}
