// ProjectOptions.cpp : Implementation of the basic project options handler

#include "stdafx.h"
#include "ProjectOptions.h"

void COptionHandlerBase::AdvanceCommandLine(CStringW& rstrCmdLine, CStringW& rstrOption, LPCOLESTR szSep /* = L" "*/)
{
	if (rstrOption.IsEmpty())
		return;

	if (!rstrCmdLine.IsEmpty())
		rstrCmdLine += szSep;

	rstrCmdLine += rstrOption;
}

void COptionHandlerBase::GetMultiPropSeparator(long id, BSTR* pbstrPreferred, BSTR* pbstrAll)
{
	if (pbstrPreferred)
	{
		CComBSTR bstrPreferred = L";";
		*pbstrPreferred = bstrPreferred.Detach();
	}
	if (pbstrAll)
	{
		CComBSTR bstrAll = L";,";
		*pbstrAll = bstrAll.Detach();
	}
}

static const wchar_t* szScriptStart = L"\
<HTML>\r\n\
<HEAD>\r\n\
<meta name=vs_targetSchema content=\"HTML 4.0\">\r\n\
<META NAME=\"Generator\" Content=\"Microsoft Visual Studio 7.0\">\r\n\
<style>\r\n\
body { font-family: \"ButtonFont\"; font-size: 10pt }\r\n\
a:hover\t{ color:red; TEXT-DECORATION:underline; cursor:hand}\r\n\
a\t{ color:\"ButtonText\"; }\r\n\
</style>\r\n\
</HEAD>\r\n\
<script language=javascript>\r\n\
function onHelp(str)\r\n\
{\r\n\
\twindow.alert(str);\r\n\
}\r\n\
</script>\r\n\
<BODY bgcolor=\"ButtonFace\" leftmargin=0 rightmargin=0 topmargin=0>";

static const wchar_t* szScriptEnd = L"\r\n\
</BODY>\r\n\
</HTML>";

static const wchar_t* szScriptItem = L"\r\n<a title=\"%s | %s | %s\" onclick=\"onHelp('VC.Project.%s.%s')\" href=\"settingspage.htm\">%s</a>";


HRESULT COptionHandlerBase::GenerateCommandLine(IVCPropertyContainer* pPropContainer, BOOL bForDisplay, 
	commandLineOptionStyle fStyle, CStringW& rstrCmdLine)
{
	rstrCmdLine.Empty();
	m_strTrailingPartForDisplay.Empty();

	SOptionEntry* pOptionTable = GetOptionTable();
	if (pOptionTable == NULL)
	{
		VSASSERT(FALSE, "No option table!");
		return DoSetErrorInfo(VCPROJ_E_INTERNAL_ERR, IDS_ERR_INTERNAL_ERROR);
	}

	CStringW strOption;
	HRESULT hr = S_OK;
	
	for (int nIdx = 0; pOptionTable[nIdx].entryType != stdOptEnd && SUCCEEDED(hr); nIdx++)
	{
		if (pOptionTable[nIdx].condType == conditionProp)	// don't evaluate these directly
			continue;

		hr = ProcessEntry(pPropContainer, &pOptionTable[nIdx], strOption, fStyle, m_strTrailingPartForDisplay, TRUE, TRUE, 
			bForDisplay);
		if (hr == S_FALSE || FAILED(hr) || strOption.IsEmpty())
			continue;

		AdvanceCommandLine(rstrCmdLine, strOption);
	}

	if (SUCCEEDED(hr))	// ignore any spurious S_FALSE's lying around near the end of the table
	{
		hr = S_OK;
		if (bForDisplay)
		{
			CStringW strTmp;
			strTmp.Format(L"%s%s%s", szScriptStart, rstrCmdLine, szScriptEnd);
			rstrCmdLine = strTmp;
		}
	}
	return hr;
}

HRESULT COptionHandlerBase::ProcessEntry(IVCPropertyContainer* pPropContainer, SOptionEntry* pEntry, CStringW& rstrOption, 
	commandLineOptionStyle fStyle, CStringW& rstrTrailing, BOOL bIncludeOptionalParts /* = TRUE */, BOOL bIncludeSlash /* = TRUE */, 
	BOOL bForDisplay /* = FALSE */)
{
	rstrOption.Empty();

	CComVariant var;
	HRESULT hr = S_OK;
	if (pEntry->type == single)
		hr = pPropContainer->GetProp(pEntry->idOption, &var);
	else
	{
		hr = pPropContainer->GetEvaluatedMultiProp(pEntry->idOption, L";,", VARIANT_TRUE, 
			pEntry->type == multipleNoCase ? VARIANT_FALSE : VARIANT_TRUE, &var.bstrVal);
		var.vt = VT_BSTR;
	}
	if (hr == S_FALSE)	// default
	{
		CComVariant varDef;
		if (SetEvenIfDefault(&varDef, pEntry->idOption))
		{
			var = varDef;
		}
		else if (SynthesizeOptionIfNeeded(pPropContainer, pEntry->idOption))
		{
			var.Clear();
			if (!SynthesizeOption(pPropContainer, pEntry->idOption, var))
			{
				VSASSERT(FALSE, "Failed to synthesize a synthesizable option");
				return S_FALSE;
			}
		}
		else
			return S_FALSE;		// don't generate anything
		hr = S_OK;
	}
	else if (FAILED(hr))
	{																
		// if you hit here, then a property name in the option table doesn't exist in the property container we're 
		// generating a switch for.  Double check both the table and the property container ancestry.  (Most likely 
		// it is the table at fault.)
		VSASSERT(SUCCEEDED(hr), "Property name not in option table");
		return S_FALSE;
	}
	else if (OverrideOptionSet(pPropContainer, pEntry->idOption))
		return S_FALSE;	// don't generate anything

	BOOL bEvaluateCondition = FALSE;
	switch (pEntry->entryType)
	{
	case stdOptEnum:
		{
			VSASSERT(var.vt == VT_I4, "Wrong variant type!");
			int nVal = var.intVal;
			if (nVal < pEntry->nStartEnumRange || nVal > pEntry->nEndEnumRange)
			{
				VSASSERT(FALSE, "Enum value out of range!");
				return DoSetErrorInfo2(VCPROJ_E_INTERNAL_ERR, IDS_ERR_ENUM_OUT_OF_BOUNDS, pEntry->szOptionName);
			}
			bEvaluateCondition = (nVal >= pEntry->nFirstTrue);
			if (pEntry->entryType == stdOptEnum)
				EvaluateEnumAtIndex(nVal, pEntry->szOption, bIncludeSlash, rstrOption);
			else
				EvaluateSpecialEnumAtIndex(nVal, pEntry->szOption, pEntry->idOption, pPropContainer, bIncludeSlash, 
					rstrOption);
		}
		break;
	case stdOptBool:
		{
			VSASSERT(var.vt == VT_BOOL || var.vt == VT_I2, "Wrong variant type!");
			int nVal = (var.boolVal == VARIANT_TRUE) ? 1 : 0;
			bEvaluateCondition = (nVal == 1);
			EvaluateEnumAtIndex(nVal, pEntry->szOption, bIncludeSlash, rstrOption);
		}
		break;
	case stdOptBstr:
	case stdOptBstrNoQuote:
	case specOptBstr:
		{
			VSASSERT(var.vt == VT_BSTR || var.vt == VT_EMPTY, "Wrong variant type!");
			CStringW strTmp;
			if (var.vt == VT_BSTR)
				strTmp = var.bstrVal;
			strTmp.TrimLeft();
			strTmp.TrimRight();
			bEvaluateCondition = !strTmp.IsEmpty();
			if (bEvaluateCondition)
			{
				CComBSTR bstrIn = strTmp;
				CComBSTR bstrOut;
				if (SUCCEEDED(pPropContainer->Evaluate(bstrIn, &bstrOut)))
					strTmp = bstrOut;
				if (pEntry->entryType == specOptBstr)
					EvaluateSpecialString(strTmp, pEntry->type != single, pEntry->szOption, pEntry->idOption, pEntry->szOptionName, 
						pEntry->idOptionPage, pPropContainer, bIncludeSlash, bForDisplay, fStyle, rstrTrailing, rstrOption);
				else
					EvaluateString(strTmp, pEntry->type != single, pEntry->szOption, pEntry->szOptionName, pEntry->idOption, 
						pEntry->idOptionPage, bIncludeSlash, pEntry->entryType == stdOptBstr, bForDisplay, rstrOption);
			}
		}
		break;
	case stdOptInt:
	case noZeroOptInt:
		{
			VSASSERT(var.vt == VT_I4, "Wrong variant type!");
			long nVal = var.lVal;
			bEvaluateCondition = (nVal != 0);
			if (bEvaluateCondition || pEntry->entryType == stdOptInt)
				EvaluateInteger(nVal, pEntry->szOption, bIncludeSlash, rstrOption);
		}
		break;
	default:
		VSASSERT(FALSE, "Unhandled option type!");	// shouldn't be able to get here!
		return S_FALSE;
	}

	if (bEvaluateCondition && bIncludeOptionalParts && pEntry->condType != conditionNone
		&& pEntry->idConditionalOption != NoNextProp)
	{
		CLookupByDispidMap* pMap = GetOptionLookupByDispid();
		RETURN_ON_NULL2(pMap, hr);

		SOptionEntry* pCondEntry = pMap->FindOptionEntry(pEntry->idConditionalOption);
		CStringW strTmp;
		hr = ProcessEntry(pPropContainer, pCondEntry, strTmp, fStyle, rstrTrailing, bIncludeOptionalParts, FALSE);
		if (hr == S_OK)
			rstrOption += strTmp;
		else if (SUCCEEDED(hr))	// got this far, so there was a non-default...
			hr = S_OK;
	}

	if (bIncludeSlash && bForDisplay && pEntry->type == single && !rstrOption.IsEmpty())
		FormatScriptItem(pEntry->szOptionName, pEntry->idOption, pEntry->idOptionPage, rstrOption);

	return S_OK;
}

LPCOLESTR COptionHandlerBase::PageName(long idOptionPage)
{
	if (idOptionPage != m_idLastOptionPage)
	{
		m_strOptionPage.LoadString(idOptionPage);
		m_idLastOptionPage = idOptionPage;
	}
	return m_strOptionPage;
}

LPCOLESTR COptionHandlerBase::ToolUIName()
{
	VSASSERT(SectionID() != 0, "Hey, must provide a valid string ID for the tool's property page section!");
	if (m_strToolUIName.IsEmpty())
		m_strToolUIName.LoadString(SectionID());
	return m_strToolUIName;
}

void COptionHandlerBase::FormatScriptItem(const wchar_t* szOptionName, long idOption, long idOptionPage, CStringW& rstrOption)
{
	VSASSERT(idOption != SPECIAL_HYPERLINK, "Must override COptionHandlerBase::FormatScriptItem if using SPECIAL_HYPERLINK!");
	if (idOptionPage == NO_HYPERLINK)
		return;	// already looks appropriate in this case

	CStringW strTmp;
	CStringW strProp;
	if (idOption)
	{
		strProp.LoadString(idOption);
		long nColon = strProp.Find(L": ");
		if (nColon)
			strProp = strProp.Left(nColon-1);
		strProp.TrimLeft();
		strProp.TrimRight();
	}
	strTmp.Format(szScriptItem, ToolUIName(), PageName(idOptionPage), strProp, ToolString(), szOptionName, rstrOption);
	rstrOption = strTmp;
}

void COptionHandlerBase::EvaluateEnumAtIndex(int nVal, LPCOLESTR szOption, BOOL bIncludeSlash, CStringW& rstrSwitch)
{
	int nMarker = 0;
	LPCOLESTR szTmp = szOption;
	while (nMarker < nVal && szTmp)
	{
		if (*szTmp == L'|')
			nMarker++;
		szTmp++;
	}
	rstrSwitch = szTmp;
	nMarker = rstrSwitch.Find(L'|');
	if (nMarker >= 0)
		rstrSwitch = rstrSwitch.Left(nMarker);
	if (bIncludeSlash && !rstrSwitch.IsEmpty())
		rstrSwitch = L"/" + rstrSwitch;
}

void COptionHandlerBase::EvaluateString(CStringW& rstrVal, BOOL bIsMultiple, LPCOLESTR szOption, LPCOLESTR szOptionName, 
	long idOption, long idOptionPage, BOOL bIncludeSlash, BOOL bQuoteIt, BOOL bForDisplay, CStringW& rstrSwitch)
{
	rstrSwitch.Empty();

	if (rstrVal.IsEmpty())	// don't generate anything on empty strings
		return;

	LPOLESTR szPtr = rstrVal.GetBuffer(rstrVal.GetLength());
	int nLast = 0, nCurrent = 0;
	int nMax = rstrVal.GetLength();

	BOOL bInQuotes = FALSE;
	BOOL bValueQuoted = FALSE;
	BOOL bFirstOption = TRUE;
	while (nLast < nMax && nCurrent < nMax)
	{
		CStringW strTmp;
		// ignore leading whitespace or we'll end up missing starting "s
		if( *szPtr == L' ' )
		{
			while( *++szPtr == L' ' );
		}

		bValueQuoted = (*szPtr == L'"');
		if (bIsMultiple)
		{
			BOOL bFoundElement = FALSE;
			while (!bFoundElement && *szPtr)
			{
				if (!bInQuotes && CharIsSeparator(*szPtr))
				{
					strTmp = rstrVal.Mid(nLast, nCurrent-nLast);
					nLast = nCurrent + 1;
					bFoundElement = TRUE;
					szPtr++;	// skip past the separator now
					nCurrent++;
				}
				else
				{
					if (*szPtr == L'"')
						bInQuotes = !bInQuotes;
					nCurrent++;
					szPtr++;
				}
			}
			if (!bFoundElement)	// got to end of string
			{
				strTmp = rstrVal.Right(nMax-nLast);
				strTmp.TrimLeft();
				nLast = nMax;
			}
		}
		else
		{
			strTmp = rstrVal;
			nLast = nMax;
		}
		if (!strTmp.IsEmpty())
		{
			int nLen = strTmp.GetLength();
			if (bValueQuoted && nLen > 1 && strTmp[nLen-2] == L'\\')	// looks like an escaped quote
			{
				CStringW strLast = strTmp[nLen-1];
				strTmp = strTmp.Left(nLen-1);	// drop the ending quote
				strTmp += L"\\";				// force the existing backslash escaped
				strTmp += strLast;				// and add the quote back on
				nLen++;
			}
			else if (nLen > 1 && strTmp[nLen-1] == L'\\')
				strTmp += L"\\";	// force it escaped
			if (!bValueQuoted && bQuoteIt)	// gotta quote it
			{
				strTmp = L'"' + strTmp;
				strTmp += L'"';
			}
			CStringW strTmp2;
			strTmp2.Format(szOption, strTmp);
			if (bIncludeSlash)
				strTmp2 = L"/" + strTmp2;
			if (bIsMultiple && bForDisplay)
				FormatScriptItem(szOptionName, idOption, idOptionPage, strTmp2);
			if (!bFirstOption)
				rstrSwitch += L" ";
			bFirstOption = FALSE;
			rstrSwitch += strTmp2;
		}
	}

	rstrVal.ReleaseBuffer();
}

void COptionHandlerBase::EvaluateInteger(long nVal, LPCOLESTR szOption, BOOL bIncludeSlash, CStringW& rstrSwitch)
{
	rstrSwitch.Format(szOption, nVal);
	if (bIncludeSlash)
		rstrSwitch = L"/" + rstrSwitch;
}

BOOL COptionHandlerBase::GetIntermediateDirectoryForFileCfg(IVCPropertyContainer* pPropContainer, CStringW& rstrIntDir)
{
	rstrIntDir.Empty();
	VSASSERT(pPropContainer != NULL, "Property container required");
	RETURN_ON_NULL2(pPropContainer, FALSE);
	CComBSTR bstrIntDir;
	if (FAILED(pPropContainer->GetEvaluatedStrProperty(VCCFGID_IntermediateDirectory, &bstrIntDir)))
		return FALSE;
	rstrIntDir = bstrIntDir;
	return TRUE;
}

BOOL COptionHandlerBase::GetFileNameForFileCfg(IVCPropertyContainer* pPropContainer, CStringW& rstrFileName)
{
	VSASSERT(pPropContainer != NULL, "Property container required");
	RETURN_ON_NULL2(pPropContainer, FALSE);

	rstrFileName.Empty();
	CComBSTR bstrFileName;
	if (FAILED(pPropContainer->GetEvaluatedStrProperty(VCFILEID_Name, &bstrFileName)) || !bstrFileName)
		return FALSE;
	rstrFileName = bstrFileName;
	return TRUE;
}

BOOL COptionHandlerBase::GetProjectForFileCfg(IVCPropertyContainer* pPropContainer, VCProject** ppProject)
{	
	CComQIPtr<VCFileConfiguration> spFileConfig = pPropContainer;
	RETURN_ON_NULL2(spFileConfig, FALSE);
	CComPtr<IDispatch> spDispFile;
	if (FAILED(spFileConfig->get_File(&spDispFile)) || spDispFile == NULL)
		return FALSE;
	CComQIPtr<VCFile> spFile = spDispFile;
	RETURN_ON_NULL2(spFile, FALSE);
	CComPtr<IDispatch> spDispProject;
	if (FAILED(spFile->get_Project(&spDispProject)) || spDispProject == NULL)
		return FALSE;
	CComQIPtr<VCProject> spProject = spDispProject;
	RETURN_ON_NULL2(spProject, FALSE);
	return (SUCCEEDED(spProject.CopyTo(ppProject)));
}

BOOL COptionHandlerBase::SetBstrInVariant(IVCPropertyContainer* pPropContainer, CComBSTR& bstrVal, CComVariant& rvar)
{
	HRESULT hr = pPropContainer->Evaluate(bstrVal, &bstrVal);
	RETURN_ON_FAIL2(hr, FALSE);
	rvar = bstrVal;
	return TRUE;
}


HRESULT COptionHandlerBase::FindSwitch(IVCPropertyContainer* pPropContainer, BSTR bstrProp, DISPID dispidProp, 
	CStringW& rstrSwitch)
{
	SOptionEntry* pOption = NULL;
	CLookupByDispidMap* pDispidMap = GetOptionLookupByDispid();
	if (dispidProp)
	{
		pOption = pDispidMap->FindOptionEntry(dispidProp);
	}
	else
	{
		CLookupByPropertyNameMap* pNameMap = GetOptionLookupByPropertyName();
		if (pNameMap)
			pOption = pNameMap->FindOptionEntry(bstrProp);
	}

	if (pOption == NULL)
		return DoSetErrorInfo(VCPROJ_E_INTERNAL_ERR, IDS_ERR_INTERNAL_ERROR);

	if (pDispidMap)
	{
		while (pOption && pOption->entryType == conditionProp)
			pOption = pDispidMap->FindOptionEntry(pOption->idParentOption);
	}

	CStringW strTrailing;
	return ProcessEntry(pPropContainer, pOption, rstrSwitch, cmdLineForBuild, strTrailing);
}

void COptionHandlerBase::InitializeDispidMap(BOOL bForce /* = FALSE */)
{
	if (!bForce && DispidLookupIsInitialized())
		return;		// all done

	CLookupByDispidMap* pMap = GetOptionLookupByDispid(FALSE /* no init */);
	if (pMap == NULL)
		return;		// no map, nothing to do

	SOptionEntry* pOptionTable = GetOptionTable();
	if (pOptionTable == NULL)
		return;		// no option table, nothing to do

	if (bForce && DispidLookupIsInitialized())
	{
		pMap->RemoveAll();
		SetDispidLookupInitialized(FALSE);
	}

	HRESULT hr = S_OK;
	for (int nIdx = 0; pOptionTable[nIdx].entryType != stdOptEnd && SUCCEEDED(hr); nIdx++)
		pMap->SetAt(pOptionTable[nIdx].idOption, (void *)&pOptionTable[nIdx]);

	SetDispidLookupInitialized();
}

void COptionHandlerBase::InitializePropertyNameMap(BOOL bForce /* = FALSE */)
{
	if (!bForce && PropertyNameLookupIsInitialized())
		return;		// all done

	CLookupByPropertyNameMap* pMap = GetOptionLookupByPropertyName(FALSE /* no init */);
	if (pMap == NULL)
		return;		// no map, nothing to do

	SOptionEntry* pOptionTable = GetOptionTable();
	if (pOptionTable == NULL)
		return;		// no option table, nothing to do

	if (bForce && PropertyNameLookupIsInitialized())
	{
		pMap->RemoveAll();
		SetPropertyNameLookupInitialized(FALSE);
	}

	HRESULT hr = S_OK;
	for (int nIdx = 0; pOptionTable[nIdx].entryType != stdOptEnd && SUCCEEDED(hr); nIdx++)
		pMap->SetAt(pOptionTable[nIdx].szOptionName, (void *)&pOptionTable[nIdx]);

	SetPropertyNameLookupInitialized();
}

SOptionEntry* CLookupByDispidMap::FindOptionEntry(DISPID dispidProperty)
{
	SOptionEntry* pEntry;
	if (Lookup(dispidProperty, (void *&)pEntry))
		return pEntry;
	
	return NULL;
}

SOptionEntry* CLookupByPropertyNameMap::FindOptionEntry(BSTR bstrProperty)
{
	CStringW strTmp = bstrProperty;

	SOptionEntry* pEntry;
	if (Lookup(strTmp, (void *&)pEntry))
		return pEntry;
	
	return NULL;
}

