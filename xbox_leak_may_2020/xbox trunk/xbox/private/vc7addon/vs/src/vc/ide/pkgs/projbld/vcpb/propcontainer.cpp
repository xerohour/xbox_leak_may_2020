//////////////////////////////////////////////////////////////////
// Basic property container base class

#include "stdafx.h"
#include "propcontainer.h"
#include "buildengine.h"
#include "settingspage.h"

HRESULT CPropertyContainerImpl::DoSetProp(long id, BOOL bCheckSpecial, VARIANT varValue, long nOverrideID /* = -1 */)
{
	if (CheckCanDirty())
	{
		CHECK_OK_TO_DIRTY(id);
	}
	m_PropMap.SetAt(id, varValue);
	return FinishSetPropOrClear(id, nOverrideID);
}

HRESULT CPropertyContainerImpl::FinishSetPropOrClear(long id, long nOverride)
{
	return S_OK;	// this method is only for handling side effects of dirtying properties
}

STDMETHODIMP CPropertyContainerImpl::SetProp(long id, VARIANT varValue)
{
	return DoSetProp(id, TRUE, varValue);
}

HRESULT CPropertyContainerImpl::DoGetProp(long id, BOOL bCheckSpecialProps, VARIANT* pVarValue)
{
	CHECK_POINTER_NULL(pVarValue);

	// Check if it's here
	HRESULT hr = DoGetLocalProp(id, bCheckSpecialProps, pVarValue);
	if (hr != S_FALSE)
		return hr;

	// It's not here, so check your parents
	return DoGetParentProp(id, VARIANT_TRUE /* allow inherit */, bCheckSpecialProps, pVarValue);
}

HRESULT CPropertyContainerImpl::DoGetParentProp(long id, VARIANT_BOOL bAllowInherit, BOOL bCheckSpecialProps, VARIANT* pVarValue)
{
	// for a standard property container, always inherit when doing GetParentProp...
	CHECK_POINTER_NULL(pVarValue);

	long lcItems = m_rgParents.GetSize();
	for (long index = 0; index < lcItems; index++)
	{
		CComPtr<IVCPropertyContainer> pParent;
		pParent = m_rgParents.GetAt(index);
		if (pParent)
		{
			CComPtr<IVCStagedPropertyContainer> spStagedParent;
			pParent->GetStagedPropertyContainer(VARIANT_FALSE, &spStagedParent);
			if (spStagedParent)
			{
				CComQIPtr<IVCPropertyContainer> spPropContainer = spStagedParent;
				if (spPropContainer)
				{
					if (spPropContainer->GetProp(id, pVarValue) == S_OK)
						return S_OK;
					else
						return S_FALSE;
				}
			}
			if (pParent->GetProp(id, pVarValue) == S_OK)
				return S_OK;
		}
	}

	// We didn't find it
	return S_FALSE;  
}

STDMETHODIMP CPropertyContainerImpl::GetParentProp(long id, VARIANT_BOOL bAllowInherit, VARIANT *pvarValue)
{
	return DoGetParentProp(id, bAllowInherit, TRUE, pvarValue);
}

STDMETHODIMP CPropertyContainerImpl::GetProp(long id, VARIANT *pVarValue)
{
	return DoGetProp(id, TRUE, pVarValue);
}

STDMETHODIMP CPropertyContainerImpl::GetMultiProp(long id, LPCOLESTR szSeparator, VARIANT_BOOL bSkipLocal, BSTR *pbstrValue)
{
	CHECK_POINTER_NULL(pbstrValue);
	*pbstrValue = NULL;

	CComBSTR bstrLocal, bstrParent;
	HRESULT hrReturn = S_FALSE;
	
	if (!bSkipLocal)
	{
		// look in here.
		CComVariant var;
		BOOL b;
		b = m_PropMap.Lookup(id, var);
		if( b )
		{
			if (NoInheritOnMultiProp(var.bstrVal, szSeparator, pbstrValue))
				return S_OK;	// means we're not doing inheritance
			bstrLocal = var.bstrVal;
			hrReturn = S_OK;
		}
	}
	
	// And check your parents
	long lcItems = m_rgParents.GetSize();
	bool bNeedSeparator = false;
	for (long index = 0; index < lcItems; index++)
	{
		CComPtr<IVCPropertyContainer> pParent;
		pParent = m_rgParents.GetAt(index);
		CComBSTR bstr;
		if (pParent != NULL)
		{
			HRESULT hr = pParent->GetMultiProp(id, szSeparator, VARIANT_FALSE, &bstr);
			if( hr == S_OK )
			{
				hrReturn = S_OK;
				if (!bNeedSeparator)
					bNeedSeparator = (bstrParent.Length() > 0);
				if (bNeedSeparator && bstr.Length() > 0)
				{
					CStringW strTmp = szSeparator;
					if (strTmp.GetLength() > 1)
					{
						strTmp = strTmp.Left(1);
						CComBSTR bstrTmp = strTmp;
						bstrParent.Append(bstrTmp);
					}
					else
						bstrParent.Append(szSeparator);
				}
				bstrParent.Append(bstr);
			}
		}
	}

	if (hrReturn != S_OK)
		return hrReturn;

	return MakeMultiPropString(bstrLocal, bstrParent, szSeparator, pbstrValue);
}

STDMETHODIMP CPropertyContainerImpl::GetEvaluatedMultiProp(long idProp, LPCOLESTR szSeparator, VARIANT_BOOL bCollapseMultiples, 
	VARIANT_BOOL bCaseSensitive, BSTR* pbstrValue)
{
	CHECK_POINTER_NULL(pbstrValue);
	*pbstrValue = NULL;

	HRESULT hr1 = GetMultiProp(idProp, szSeparator, VARIANT_FALSE, pbstrValue);
	RETURN_ON_FAIL(hr1);

	HRESULT hr2 = Evaluate(*pbstrValue, pbstrValue);
	RETURN_ON_FAIL(hr2);

	if (bCollapseMultiples)
	{
		HRESULT hr3 = CollapseMultiples(*pbstrValue, szSeparator, bCaseSensitive, pbstrValue);
		RETURN_ON_FAIL(hr3);
	}

	return hr1;
}

STDMETHODIMP CPropertyContainerImpl::IsMultiProp(long id, VARIANT_BOOL* pbIsMultiProp)
{
	CHECK_POINTER_NULL(pbIsMultiProp);
	*pbIsMultiProp = VARIANT_FALSE;
	return S_FALSE;
}

STDMETHODIMP CPropertyContainerImpl::GetMultiPropSeparator(long id, BSTR* pbstrPreferred, BSTR* pbstrAll)
{
	CHECK_POINTER_NULL(pbstrPreferred);
	CHECK_POINTER_NULL(pbstrAll);
	*pbstrPreferred = NULL;
	*pbstrAll = NULL;
	return S_FALSE;
}

HRESULT CPropertyContainerImpl::DoGetStrProperty(long idProp, BOOL bSpecialPropsOnly, BSTR* pbstrValue)
{
	CHECK_POINTER_NULL(pbstrValue);
	CComVariant var;
	HRESULT hr = DoGetProp(idProp, FALSE, &var);
	if (hr == S_OK)
	{
		if (var.vt != VT_BSTR)	// if it is not a BSTR, then we really didn't get the property no matter WHAT DoGetProp says...
			return S_FALSE;

		CComBSTR bstrVal = var.bstrVal;
		if (bstrVal.Length() > 0)
			*pbstrValue = bstrVal.Detach();
	}

	return hr;
}

STDMETHODIMP CPropertyContainerImpl::GetStrProperty(long idProp, BSTR* pbstrValue)
{
	return DoGetStrProperty(idProp, FALSE, pbstrValue);
}

HRESULT CPropertyContainerImpl::DoSetStrProperty(long idProp, BOOL bCheckSpecial, BSTR bstrValue, long nOverrideID /* = -1 */)
{
	CComVariant var;
	var = bstrValue;	// vt set properly during this assignment
	return DoSetProp(idProp, FALSE, var, nOverrideID);
}

STDMETHODIMP CPropertyContainerImpl::SetStrProperty(long idProp, BSTR bstrValue)
{
	return DoSetStrProperty(idProp, TRUE, bstrValue);
}

STDMETHODIMP CPropertyContainerImpl::GetEvaluatedStrProperty(long idProp, BSTR* pbstrValue)
{
	CHECK_POINTER_NULL(pbstrValue);
	*pbstrValue = NULL;

	HRESULT hr1 = GetStrProperty(idProp, pbstrValue);
	RETURN_ON_FAIL(hr1);

	HRESULT hr2 = EvaluateWithValidation(*pbstrValue, idProp, pbstrValue);
	RETURN_ON_FAIL(hr2);

	return hr1;
}

STDMETHODIMP CPropertyContainerImpl::Evaluate(BSTR bstrIn, BSTR *pbstrOut)
{
	CHECK_POINTER_NULL(pbstrOut);
	return ExpandMacros(pbstrOut, bstrIn, this, FALSE);
}

STDMETHODIMP CPropertyContainerImpl::EvaluateWithValidation(BSTR bstrIn, long idProp, BSTR* pbstrOut)
{
	CHECK_POINTER_NULL(pbstrOut);
	return ExpandMacros(pbstrOut, bstrIn, this, FALSE, idProp);
}

HRESULT CPropertyContainerImpl::DoGetIntProperty(long idProp, BOOL bCheckSpecial, long* pnValue)
{
	CHECK_POINTER_NULL(pnValue);
	*pnValue = 0;

	CComVariant var;
	HRESULT hr = DoGetProp(idProp, bCheckSpecial, &var);
	if (hr == S_OK)
		*pnValue = var.intVal;
	return hr;
}

STDMETHODIMP CPropertyContainerImpl::GetIntProperty(long idProp, long* pnValue)
{
	return DoGetIntProperty(idProp, TRUE, pnValue);
}

HRESULT CPropertyContainerImpl::GetParentIntProperty(long idProp, long* pnValue)
{
	CHECK_POINTER_NULL(pnValue);
	*pnValue = 0;

	CComVariant var;
	HRESULT hr = DoGetParentProp(idProp, VARIANT_TRUE /* allow inherit */, TRUE, &var);
	if (hr == S_OK)
		*pnValue = var.intVal;

	return hr;
}

HRESULT CPropertyContainerImpl::GetLocalIntProperty(long idProp, long* pnValue)
{
	CHECK_POINTER_NULL(pnValue);
	*pnValue = 0;

	CComVariant var;
	HRESULT hr = DoGetLocalProp(idProp, TRUE, &var);
	if (hr == S_OK)
		*pnValue = var.intVal;

	return hr;
}

HRESULT CPropertyContainerImpl::DoSetIntProperty(long idProp, BOOL bCheckSpecial, long nValue, long nOverrideID /* = -1 */)
{
	CComVariant var(nValue);
	return DoSetProp(idProp, FALSE, var, nOverrideID);
}

STDMETHODIMP CPropertyContainerImpl::SetIntProperty(long idProp, long nValue)
{
	return DoSetIntProperty(idProp, TRUE, nValue);
}

HRESULT CPropertyContainerImpl::DoGetBoolProperty(long idProp, BOOL bCheckSpecial, VARIANT_BOOL* pbValue)
{
	CHECK_POINTER_NULL(pbValue);
	*pbValue = VARIANT_FALSE;

	CComVariant var;
	HRESULT hr = GetProp(idProp, &var);
	if (hr == S_OK)
		*pbValue = var.boolVal;
	return hr;
}

STDMETHODIMP CPropertyContainerImpl::GetBoolProperty(long idProp, VARIANT_BOOL* pbValue)
{
	return DoGetBoolProperty(idProp, TRUE, pbValue);
}

HRESULT CPropertyContainerImpl::DoSetBoolProperty(long idProp, BOOL bCheckSpecial, VARIANT_BOOL bValue, long nOverrideID /* = -1 */)
{
	CComVariant var(bValue);
	return DoSetProp(idProp, bCheckSpecial, var, nOverrideID);
}

STDMETHODIMP CPropertyContainerImpl::SetBoolProperty(long idProp, VARIANT_BOOL bValue)
{
	return DoSetBoolProperty(idProp, TRUE, bValue);
}

HRESULT CPropertyContainerImpl::DoClear(long id, long nOverrideID /* = -1 */)
{
	m_PropMap.RemoveKey(id);
	return FinishSetPropOrClear(id, nOverrideID);
}

STDMETHODIMP CPropertyContainerImpl::Clear(long id)
{
	return DoClear(id);
}

HRESULT CPropertyContainerImpl::DoGetLocalProp(long id, BOOL bCheckSpecial, VARIANT *pvarValue)
{
	CComVariant varVal;

	// Check if it's here first
	if( m_PropMap.Lookup(id, varVal) )
	{
		if(pvarValue)
			varVal.Detach(pvarValue);
		return S_OK;
	}

	return S_FALSE;
}

STDMETHODIMP CPropertyContainerImpl::GetLocalProp(long id, VARIANT *pvarValue)
{
	return DoGetLocalProp(id, TRUE, pvarValue);
}

HRESULT CPropertyContainerImpl::DoClearAll()
{
	CHECK_OK_TO_DIRTY(0);

	// this loop allows the property container to handle side effects of dirtying the props that got cleared
	VCPOSITION pos = m_PropMap.GetStartPosition();
	while (pos != NULL)
	{
		long id;
		CComVariant var;
		m_PropMap.GetNextAssoc(pos, id, var);
		FinishSetPropOrClear(id, GetOverrideID(id));
	}

	m_PropMap.RemoveAll();
	return S_OK;
}

STDMETHODIMP CPropertyContainerImpl::ClearAll()
{
	return DoClearAll();
}

STDMETHODIMP CPropertyContainerImpl::GetStagedPropertyContainer(VARIANT_BOOL bCreateIfNecessary, IVCStagedPropertyContainer** ppPropContainer)
{
	CHECK_POINTER_NULL(ppPropContainer);

	if (!m_spStagedContainer)
	{
		if (bCreateIfNecessary)
		{
			HRESULT hr = CVCStagedPropertyContainer::CreateInstance(this, &m_spStagedContainer);
			RETURN_ON_FAIL(hr);
		}
		else
		{
			*ppPropContainer = NULL;
			return S_OK;
		}
	}

	return m_spStagedContainer.CopyTo(ppPropContainer);
}

STDMETHODIMP CPropertyContainerImpl::ReleaseStagedPropertyContainer()		// intended for use ONLY by the staged property container itself
{
	if (m_spStagedContainer)
	{
		m_spStagedContainer->Close();
		m_spStagedContainer.Release();
	}

	return S_OK;
}

HRESULT CPropertyContainerImpl::FinishClose()
{
	if (m_spStagedContainer)
		m_spStagedContainer->Close();
	m_spStagedContainer.Release();
	m_rgParents.RemoveAll();
	return S_OK;
}

STDMETHODIMP CPropertyContainerImpl::HasLocalStorage(VARIANT_BOOL bForSave, VARIANT_BOOL* pbHasLocalStorage)
{
	CHECK_POINTER_NULL(pbHasLocalStorage);
	*pbHasLocalStorage = VARIANT_TRUE;	// always have storage at this level; override when this isn't the case
	return S_OK;
}
/* static */
HRESULT CPropertyContainerImpl::WritePropertyToStream(IStream *pStream, VARIANT_BOOL bProp, DISPID dispid)
{
	ULONG iByteCount;
	VARTYPE type = VT_BOOL;

	//Write out type
	HRESULT hr = pStream->Write(&type, sizeof(VARTYPE), &iByteCount);
	RETURN_ON_FAIL(hr);

	//Write dispid
	hr = pStream->Write(&dispid, sizeof(DISPID), &iByteCount);
	RETURN_ON_FAIL(hr);

	//Write out bool
	hr = pStream->Write(&bProp, sizeof(VARIANT_BOOL), &iByteCount);
	RETURN_ON_FAIL(hr);

	return S_OK;
}

/* static */
HRESULT CPropertyContainerImpl::WritePropertyToStream(IStream *pStream, CComBSTR& bstrProp, DISPID dispid)
{
	ULONG iByteCount;
	VARTYPE type = VT_BSTR;

	//Write out type
	HRESULT hr = pStream->Write(&type, sizeof(VARTYPE), &iByteCount);
	RETURN_ON_FAIL(hr);

	//Write dispid
	hr = pStream->Write(&dispid, sizeof(DISPID), &iByteCount);
	RETURN_ON_FAIL(hr);

	//Write out bstr property
	hr = bstrProp.WriteToStream(pStream);
	RETURN_ON_FAIL(hr);
	
	return S_OK;
}

/* static */
HRESULT CPropertyContainerImpl::WritePropertyToStream(IStream *pStream, long lProp, DISPID dispid)
{
	ULONG iByteCount;
	VARTYPE type = VT_I4;

	//Write out type
	HRESULT hr = pStream->Write(&type, sizeof(VARTYPE), &iByteCount);
	RETURN_ON_FAIL(hr);

	//Write dispid
	hr = pStream->Write(&dispid, sizeof(DISPID), &iByteCount);
	RETURN_ON_FAIL(hr);

	//Write out remote bool
	hr = pStream->Write(&lProp, sizeof(long), &iByteCount);
	RETURN_ON_FAIL(hr);

	return S_OK;

}

/* static */
HRESULT CPropertyContainerImpl::WritePropertyToStream(IStream *pStream, CComVariant& variantProp, DISPID dispid)
{
	ULONG iByteCount;
	VARTYPE type = VT_VARIANT;

	//Write out type
	HRESULT hr = pStream->Write(&type, sizeof(VARTYPE), &iByteCount);
	RETURN_ON_FAIL(hr);

	//Write dispid
	hr = pStream->Write(&dispid, sizeof(DISPID), &iByteCount);
	RETURN_ON_FAIL(hr);

	//Write out variant
	hr = variantProp.WriteToStream(pStream);
	RETURN_ON_FAIL(hr);

	return S_OK;
}

HRESULT CPropertyContainerImpl::DoGetVCProjectEngine(IDispatch** ppProjEngine)
{
	CHECK_POINTER_VALID(ppProjEngine);	// we're providing a single source implementation for automation here...
	RETURN_ON_NULL(g_pProjectEngine);
	return g_pProjectEngine->QueryInterface(IID_IDispatch, (void **)ppProjEngine);
}

HRESULT CVCStagedPropertyContainer::CreateInstance(IVCPropertyContainer* pParentPropContainer, 
	IVCStagedPropertyContainer **ppStagedPropertyContainer)
{
	CVCStagedPropertyContainer *pVar;
	CComObject<CVCStagedPropertyContainer> *pObj;
	HRESULT hr = CComObject<CVCStagedPropertyContainer>::CreateInstance(&pObj);
	if (SUCCEEDED(hr))
	{
		pVar = pObj;
		pVar->AddRef();
		*ppStagedPropertyContainer = pVar;
		hr = pVar->Initialize(pParentPropContainer);
	}
	return hr;
}

HRESULT CVCStagedPropertyContainer::Initialize(IVCPropertyContainer* pParentPropContainer)
{
	m_spParentPropContainer = pParentPropContainer;
	return (m_spParentPropContainer ? S_OK : E_UNEXPECTED);
}

HRESULT CVCStagedPropertyContainer::DoGetParentProp(long id, VARIANT_BOOL bAllowInherit, BOOL bCheckSpecialProps, VARIANT* pVarValue)
{
	CHECK_POINTER_NULL(pVarValue);
	RETURN_ON_NULL2(m_spParentPropContainer, S_FALSE);

	if (bAllowInherit)
	{
		if (m_spParentPropContainer->GetProp(id, pVarValue) == S_OK )
			return S_OK;
	}
	else
	{
		if (m_spParentPropContainer->GetLocalProp(id, pVarValue) == S_OK )
			return S_OK;
	}

	// We didn't find it
	return S_FALSE;  
}

STDMETHODIMP CVCStagedPropertyContainer::GetMultiProp(long id, LPCOLESTR szSeparator, VARIANT_BOOL bSkipLocal, BSTR *pbstrValue)
{
	CHECK_POINTER_NULL(pbstrValue);
	*pbstrValue = NULL;

	CComBSTR bstrLocal, bstrParent;
	HRESULT hrReturn = S_FALSE;
	
	if (!bSkipLocal)
	{
		// look in here.
		CComVariant var;
		BOOL b;
		b = m_PropMap.Lookup(id, var);
		if( b )
		{
			if (NoInheritOnMultiProp(var.bstrVal, szSeparator, pbstrValue))
				return S_OK;	// means we're not doing inheritance
			bstrLocal = var.bstrVal;
			hrReturn = S_OK;
		}
		else if (m_spParentPropContainer)
		{
			HRESULT hr = m_spParentPropContainer->GetLocalProp(id, &var);
			if (hr == S_OK)
			{
				if (NoInheritOnMultiProp(var.bstrVal, szSeparator, pbstrValue))
					return S_OK;	// means we're not doing inheritance
				bstrLocal = var.bstrVal;
				hrReturn = S_OK;
			}
		}
	}
	
	// And check your parents
	if (m_spParentPropContainer && 
		m_spParentPropContainer->GetMultiProp(id, szSeparator, VARIANT_TRUE, &bstrParent) == S_OK)
		hrReturn = S_OK;

	if (hrReturn != S_OK)
		return hrReturn;

	return MakeMultiPropString(bstrLocal, bstrParent, szSeparator, pbstrValue);
}

STDMETHODIMP CVCStagedPropertyContainer::GetMultiPropSeparator(long id, BSTR* pbstrPreferred, BSTR* pbstrAll)
{
	if (m_spParentPropContainer)
		return m_spParentPropContainer->GetMultiPropSeparator(id, pbstrPreferred, pbstrAll);

	// no parent?  make an educated guess
	CHECK_POINTER_NULL(pbstrPreferred);
	CHECK_POINTER_NULL(pbstrAll);
	CComBSTR bstrPreferred = L";";
	CComBSTR bstrAll = L";,";
	*pbstrPreferred = bstrPreferred.Detach();
	*pbstrAll = bstrAll.Detach();
	return S_OK;
}

STDMETHODIMP CVCStagedPropertyContainer::ClearPropertyRange(long nMinID, long nMaxID)
{
	VCPOSITION pos = m_PropMap.GetStartPosition();
	while (pos)
	{
		long key;
		CComVariant varVal;
		m_PropMap.GetNextAssoc(pos, key, varVal);
		if (key >= nMinID && key <= nMaxID)
			m_PropMap.RemoveKey(key);
	}

	return S_OK;
}

STDMETHODIMP CVCStagedPropertyContainer::get_Parent(IVCPropertyContainer** ppParent)
{
	CHECK_POINTER_NULL(ppParent);
	return m_spParentPropContainer.CopyTo(ppParent);
}

STDMETHODIMP CVCStagedPropertyContainer::GetGrandParentProp(long id, VARIANT *pvarValue)		// skip to parent of our non-staged parent
{
	CHECK_ZOMBIE(m_spParentPropContainer, IDS_ERR_CFG_ZOMBIE);
	return m_spParentPropContainer->GetParentProp(id, VARIANT_TRUE /* allow inherit */, pvarValue);
}

STDMETHODIMP CVCStagedPropertyContainer::Close()
{
	m_spParentPropContainer.Release();
	return S_OK;
}

STDMETHODIMP CVCStagedPropertyContainer::CommitPropertyRange(long nMinID, long nMaxID)
{
	return DoCommitPropertyRange(nMinID, nMaxID, false);
}

STDMETHODIMP CVCStagedPropertyContainer::Commit()
{
	return DoCommitPropertyRange(0, 0, true);
}

HRESULT CVCStagedPropertyContainer::DoCommitPropertyRange(long nMinID, long nMaxID, bool bIgnoreRange)
{
	CHECK_ZOMBIE(m_spParentPropContainer, IDS_ERR_CFG_ZOMBIE);

	VCPOSITION iter = m_PropMap.GetStartPosition();
	// iterate over each item in the map
	HRESULT hr = S_OK;
	while( iter && SUCCEEDED(hr))
	{
		long key;
		CComVariant varVal;
		m_PropMap.GetNextAssoc(iter, key, varVal);

		if (!bIgnoreRange && (key < nMinID || key > nMaxID))
			continue;

		BOOL bSetProp = TRUE;
		if ((varVal.vt == VT_I4 && varVal.intVal == INHERIT_PROP_VALUE) || 
			(varVal.vt == VT_I2 && varVal.iVal == INHERIT_PROP_VALUE) || 
			(varVal.vt == VT_BSTR && wcscmp( L"-2", varVal.bstrVal ) == 0))
			bSetProp = FALSE;
		else if (varVal.vt == VT_BSTR && SysStringLen( varVal.bstrVal ) == 0 )
		{
			VARIANT_BOOL bIsMulti = VARIANT_FALSE;
			if (m_spParentPropContainer->IsMultiProp(key, &bIsMulti) != S_OK || bIsMulti == VARIANT_TRUE)
				bSetProp = FALSE;
		}
		if (bSetProp)
			hr = m_spParentPropContainer->SetProp(key, varVal);
		else
			hr = m_spParentPropContainer->Clear(key);
		RETURN_ON_FAIL(hr);
		m_PropMap.RemoveKey(key);
	}

	return S_OK;
}

STDMETHODIMP CVCStagedPropertyContainer::NextElement(long* pKey, VARIANT* pVal)
{
	CHECK_POINTER_NULL(pKey);
	*pKey = 0;
	CHECK_POINTER_NULL(pVal);
	pVal->vt = VT_EMPTY;
	RETURN_ON_NULL2(m_pos, S_FALSE);

	long id;
	CComVariant var;
	m_PropMap.GetNextAssoc(m_pos, id, var);
	*pKey = id;
	var.Detach(pVal);

	return S_OK;
}

STDMETHODIMP CVCStagedPropertyContainer::Reset(void)
{
	m_pos = m_PropMap.GetStartPosition();
	return S_OK;
}

STDMETHODIMP CVCStagedPropertyContainer::HasDefaultValue(DISPID dispid, BOOL *pfDefault)
{
	CHECK_POINTER_NULL(pfDefault);
	*pfDefault = FALSE;
	
	// Check if its here and not clear
	CComVariant varVal;
	if( m_PropMap.Lookup(dispid, varVal) )
	{
		if( (varVal.vt == VT_I4 && varVal.intVal == INHERIT_PROP_VALUE) || (varVal.vt == VT_I2 && varVal.iVal == INHERIT_PROP_VALUE) || (varVal.vt == VT_BSTR && wcscmp( L"-2", varVal.bstrVal ) == 0) )
			*pfDefault = TRUE;
		return S_OK;
	}

	// OK so is it in the parent ?
	if (!m_spParentPropContainer || m_spParentPropContainer->GetLocalProp( dispid, NULL ) != S_OK)
		*pfDefault = TRUE;

	return S_OK;
}

STDMETHODIMP CVCStagedPropertyContainer::HoldStagedContainer()			// number of calls to Hold/Release StagedContainer need to match
{
	m_cStaged++;
	return S_OK;
}

STDMETHODIMP CVCStagedPropertyContainer::ReleaseStagedContainer()		// number of calls to Hold/Release StagedContainer need to match
{
	if (m_cStaged > 0)
		m_cStaged--;
	if (m_cStaged == 0 && m_spParentPropContainer)
		m_spParentPropContainer->ReleaseStagedPropertyContainer();

	return S_OK;
}
