//NONSHIP

// This is a part of the Active Template Library.
// Copyright (C) 1996-1998 Microsoft Corporation
// All rights reserved.
//
// This source code is only intended as a supplement to the
// Active Template Library Reference and related
// electronic documentation provided with the library.
// See these sources for detailed information regarding the
// Active Template Library product.

#ifndef __HTMLEDBASE_H__
#define __HTMLEDBASE_H__

#pragma once

#include <mshtml.h>
#include <dhtmled.h>
#include <triedcid.h>

template <class T>
class CHtmlEditCtrlBase 
{
public:
//Methods
	BOOL ExecCommand(long cmdID, long cmdExecOpt, VARIANT* pInVar,VARIANT* pOutVar) const
	{
		T* pT = (T*)this;
		CComQIPtr<IDHTMLEdit> spDoc = pT->GetControlUnknown();

		HRESULT hr;
		ATLASSERT(spDoc != NULL);
 
		CComVariant vaIn;
		CComVariant vaOut;

		if (pInVar)
			vaIn.Copy(pInVar);
		if (pOutVar)
			vaOut.Copy(pOutVar);

		hr = spDoc->ExecCommand((DHTMLEDITCMDID)cmdID,(OLECMDEXECOPT)cmdExecOpt,&vaIn,&vaOut);
		if (FAILED(hr) && pOutVar != NULL)	
		{
			pOutVar->vt = VT_ERROR;
			pOutVar->scode = hr;
			return FALSE;
		} 
		else
		{
			if(pInVar)
				::VariantCopy(pInVar,&vaIn);
			if(pOutVar)
				::VariantCopy(pOutVar,&vaOut);
			return TRUE;
		}
	}

	long QueryStatus(long cmdID) const
	{
		T* pT = (T*)this;
		CComQIPtr<IDHTMLEdit> spDoc = pT->GetControlUnknown();

		ATLASSERT(spDoc != NULL);
		DHTMLEDITCMDF cmdF;
		spDoc->QueryStatus((DHTMLEDITCMDID)cmdID, &cmdF);
		return (long)cmdF;
	}

	BOOL SetContextMenu(LPTSTR *ppMenuStrings, OLE_TRISTATE *pState, DWORD cItems)
	{
		T* pT = (T*)this;
		CComQIPtr<IDHTMLEdit> spDoc = pT->GetControlUnknown();
		
		ATLASSERT(spDoc != NULL);
		if(ppMenuStrings==NULL || pState==NULL)
			return FALSE;
		
		USES_CONVERSION;
		HRESULT hr = E_FAIL;
		SAFEARRAY *psaStr = NULL, *psaTS = NULL;
		long index = 0;
		BSTR szItem = NULL;
		VARIANT vStrings,vState;
		vStrings.vt = VT_ARRAY|VT_BSTR;
		vState.vt = VT_ARRAY|VT_I4;
	
		//create arrays for the strings and tristates
		SAFEARRAYBOUND saBound = {cItems,0};
		psaStr = ::SafeArrayCreate(VT_BSTR,1,&saBound);
		psaTS = ::SafeArrayCreate(VT_I4, 1, &saBound);
		if (psaStr && !psaTS)
		{
			::SafeArrayDestroy(psaStr);
			return FALSE;
		}
		if (!psaStr && psaTS)
		{
			::SafeArrayDestroy(psaTS);
			return FALSE;
		}
	
		for(index = 0; index < (long)cItems; index++)
		{
			szItem = ::SysAllocString( T2OLE(ppMenuStrings[index]) );
			if (szItem)
			{
				hr = ::SafeArrayPutElement(psaStr, &index, szItem);
				if(FAILED(hr))
					break; //couldn't add to the safearray
				hr = ::SafeArrayPutElement(psaTS, &index, &pState[index]);
				if(FAILED(hr))
					break;
			}
			else
			{ 
				//couldn't allocate the BSTR
				hr = E_OUTOFMEMORY;
				break;
			}
		}
	
		if (FAILED(hr))
		{
			//failed to create all elements of the arrays
			//better free any BSTRs we allocated
			for(long n = 0; n < index; n++)
				if(SUCCEEDED(::SafeArrayGetElement(psaStr, &n, reinterpret_cast<void*>(&szItem))))
					::SysFreeString(szItem);
	
			::SafeArrayDestroy(psaStr);
			::SafeArrayDestroy(psaTS);
			return FALSE;
		}
	
		vStrings.parray = psaStr;
		vState.parray = psaTS;
	
		//arrays are created, call the method
		return SUCCEEDED(spDoc->SetContextMenu(&vStrings, &vState));
	}
	
	BOOL NewDocument()
	{
		T* pT = (T*)this;
		CComQIPtr<IDHTMLEdit> spDoc = pT->GetControlUnknown();
		
		ATLASSERT(spDoc != NULL);
		return SUCCEEDED(spDoc->NewDocument());
	}

	BOOL LoadURL(LPCTSTR szURL)
	{
		USES_CONVERSION;
		T* pT = (T*)this;
		CComQIPtr<IDHTMLEdit> spDoc = pT->GetControlUnknown();

		ATLASSERT(spDoc != NULL);
		CComBSTR var(szURL);
		return SUCCEEDED(spDoc->LoadURL(var));
	}

	BOOL FilterSourceCode(LPCTSTR szSourceCodeIn, BSTR *pszSourceOut)
	{
		USES_CONVERSION;
		T* pT = (T*)this;
		CComQIPtr<IDHTMLEdit> spDoc = pT->GetControlUnknown();

		ATLASSERT(spDoc != NULL);
		CComBSTR var(szSourceCodeIn);
		return SUCCEEDED(spDoc->FilterSourceCode(var, pszSourceOut));
	}

	BOOL Refresh()
	{
		T* pT = (T*)this;
		CComQIPtr<IDHTMLEdit> spDoc = pT->GetControlUnknown();

		ATLASSERT(spDoc != NULL);
		return SUCCEEDED(spDoc->Refresh());
	}

	BOOL LoadDocument(LPCTSTR szName)
	{
		T* pT = (T*)this;
		CComQIPtr<IDHTMLEdit> spDoc = pT->GetControlUnknown();
		ATLASSERT(spDoc != NULL);
		
		CComVariant vtName(szName);
		CComVariant vtPromptUser;
		vtPromptUser.vt = VT_BOOL;
		vtPromptUser.boolVal = ATL_VARIANT_FALSE;

		return SUCCEEDED(spDoc->LoadDocument(&vtName,&vtPromptUser));
	}
	
	BOOL SaveDocument(LPCTSTR szName)
	{
		T* pT = (T*)this;
		CComQIPtr<IDHTMLEdit> spDoc = pT->GetControlUnknown();
		ATLASSERT(spDoc != NULL);
		CComVariant vtName(szName);
		CComVariant vtPromptUser;
		vtPromptUser.vt = VT_BOOL;
		vtPromptUser.boolVal = ATL_VARIANT_FALSE;

		return SUCCEEDED(spDoc->SaveDocument(&vtName,&vtPromptUser));
	}
	
	BOOL PrintHTML(BOOL bShowUI=FALSE)
	{
		T* pT = (T*)this;
		CComQIPtr<IDHTMLEdit> spDoc = pT->GetControlUnknown();
		
		ATLASSERT(spDoc != NULL);
		CComVariant vPrint(bShowUI);
		return SUCCEEDED(spDoc->PrintDocument(&vPrint));
	}

//Properties
	BOOL GetDocument(IHTMLDocument2** ppDoc)
	{	
		T* pT = (T*)this;
		CComQIPtr<IDHTMLEdit> spDoc = pT->GetControlUnknown();
		
		ATLASSERT(spDoc != NULL);
		return SUCCEEDED(spDoc->get_DOM(ppDoc));
	}

	BOOL GetDocumentHTML(BSTR *pszBuff)
	{
		T* pT = (T*)this;
		CComQIPtr<IDHTMLEdit> spDoc = pT->GetControlUnknown();

		USES_CONVERSION;
		
		ATLASSERT(spDoc != NULL);
		return SUCCEEDED(spDoc->get_DocumentHTML(pszBuff));
	}
	
	BOOL SetDocumentHTML(LPCTSTR szHTML)
	{
		USES_CONVERSION;
		T* pT = (T*)this;
		CComQIPtr<IDHTMLEdit> spDoc = pT->GetControlUnknown();
		
		ATLASSERT(spDoc != NULL);
		return SUCCEEDED(spDoc->put_DocumentHTML(CComBSTR(szHTML)));
	}
	
	BOOL GetActivateApplets()
	{
		T* pT = (T*)this;
		CComQIPtr<IDHTMLEdit> spDoc = pT->GetControlUnknown();
		
		ATLASSERT(spDoc != NULL);
		VARIANT_BOOL bActivate;
		spDoc->get_ActivateApplets(&bActivate);
		return bActivate != ATL_VARIANT_FALSE ? TRUE : FALSE;
	}
	
	BOOL SetActivateApplets(BOOL bNewValue)
	{
		T* pT = (T*)this;
		CComQIPtr<IDHTMLEdit> spDoc = pT->GetControlUnknown();
		
		ATLASSERT(spDoc != NULL);
		return S_OK == spDoc->put_ActivateApplets(bNewValue ? ATL_VARIANT_TRUE : ATL_VARIANT_FALSE);
	}
	
	BOOL GetActivateActiveXControls()
	{
		T* pT = (T*)this;
		CComQIPtr<IDHTMLEdit> spDoc = pT->GetControlUnknown();
		
		ATLASSERT(spDoc != NULL);
	
		VARIANT_BOOL bActivate;
		spDoc->get_ActivateActiveXControls(&bActivate);
		return bActivate != ATL_VARIANT_FALSE ? TRUE : FALSE;
	}
	
	BOOL SetActivateActiveXControls(BOOL bNewValue)
	{
		T* pT = (T*)this;
		CComQIPtr<IDHTMLEdit> spDoc = pT->GetControlUnknown();
		
		ATLASSERT(spDoc != NULL);
		return SUCCEEDED(spDoc->put_ActivateActiveXControls(bNewValue ? ATL_VARIANT_TRUE : ATL_VARIANT_FALSE));
	}
	
	BOOL GetActivateDTCs()
	{
		T* pT = (T*)this;
		CComQIPtr<IDHTMLEdit> spDoc = pT->GetControlUnknown();
		
		ATLASSERT(spDoc != NULL);
		VARIANT_BOOL bActivate;
		spDoc->get_ActivateDTCs(&bActivate);
		return bActivate != ATL_VARIANT_FALSE ? TRUE : FALSE;
	}
	
	BOOL SetActivateDTCs(BOOL bNewValue)
	{
		T* pT = (T*)this;
		CComQIPtr<IDHTMLEdit> spDoc = pT->GetControlUnknown();
		
		ATLASSERT(spDoc != NULL);
		return S_OK == spDoc->put_ActivateDTCs(bNewValue ? ATL_VARIANT_TRUE : ATL_VARIANT_FALSE);
	}

	BOOL GetShowDetails()
	{
		T* pT = (T*)this;
		CComQIPtr<IDHTMLEdit> spDoc = pT->GetControlUnknown();
		ATLASSERT(spDoc != NULL);
	
		VARIANT_BOOL bShowDetails;
		spDoc->get_ShowDetails(&bShowDetails);
		return bShowDetails != ATL_VARIANT_FALSE ? TRUE : FALSE;
	}
	
	BOOL SetShowDetails(BOOL bNewValue)
	{
		T* pT = (T*)this;
		CComQIPtr<IDHTMLEdit> spDoc = pT->GetControlUnknown();

		ATLASSERT(spDoc != NULL);
		return S_OK == spDoc->put_ShowDetails(bNewValue ? ATL_VARIANT_TRUE : ATL_VARIANT_FALSE);
	}
	
	BOOL GetShowBorders()
	{
		T* pT = (T*)this;
		CComQIPtr<IDHTMLEdit> spDoc = pT->GetControlUnknown();
		
		ATLASSERT(spDoc != NULL);
	
		VARIANT_BOOL bShowBorders;
		spDoc->get_ShowBorders(&bShowBorders);
		return bShowBorders != ATL_VARIANT_FALSE ? TRUE : FALSE;
	}
	
	BOOL SetShowBorders(BOOL bNewValue)
	{
		T* pT = (T*)this;
		CComQIPtr<IDHTMLEdit> spDoc = pT->GetControlUnknown();
		
		ATLASSERT(spDoc != NULL);
		return S_OK == spDoc->put_ShowBorders(bNewValue ? ATL_VARIANT_TRUE : ATL_VARIANT_FALSE);
	}
	
	long GetAppearance()
	{
		T* pT = (T*)this;
		CComQIPtr<IDHTMLEdit> spDoc = pT->GetControlUnknown();
		
		ATLASSERT(spDoc != NULL);
	
		long nAppearance;
		spDoc->get_Appearance((DHTMLEDITAPPEARANCE*)&nAppearance);
		return nAppearance;
	}
	
	BOOL SetAppearance(long nNewValue)
	{
		T* pT = (T*)this;
		CComQIPtr<IDHTMLEdit> spDoc = pT->GetControlUnknown();
		
		ATLASSERT(spDoc != NULL);
		return SUCCEEDED(spDoc->put_Appearance((DHTMLEDITAPPEARANCE)nNewValue));
	}
	
	BOOL GetScrollbars()
	{
		T* pT = (T*)this;
		CComQIPtr<IDHTMLEdit> spDoc = pT->GetControlUnknown();
		
		ATLASSERT(spDoc != NULL);
	
		VARIANT_BOOL bShowScrollBars;
		spDoc->get_Scrollbars(&bShowScrollBars);
		return bShowScrollBars != ATL_VARIANT_FALSE ? TRUE : FALSE;
	}
	
	BOOL SetScrollbars(BOOL bNewValue)
	{
		T* pT = (T*)this;
		CComQIPtr<IDHTMLEdit> spDoc = pT->GetControlUnknown();
		
		ATLASSERT(spDoc != NULL);
		return S_OK == spDoc->put_Scrollbars(bNewValue ? ATL_VARIANT_TRUE : ATL_VARIANT_FALSE);
	}
	
	long GetScrollbarAppearance()
	{
		T* pT = (T*)this;
		CComQIPtr<IDHTMLEdit> spDoc = pT->GetControlUnknown();
		
		ATLASSERT(spDoc != NULL);
	
		long nAppearance;
		spDoc->get_Appearance((DHTMLEDITAPPEARANCE*)&nAppearance);
		return nAppearance;
	}
	
	BOOL SetScrollbarAppearance(long nNewValue)
	{
		T* pT = (T*)this;
		CComQIPtr<IDHTMLEdit> spDoc = pT->GetControlUnknown();
		
		ATLASSERT(spDoc != NULL);
		return SUCCEEDED(spDoc->put_ScrollbarAppearance((DHTMLEDITAPPEARANCE)nNewValue));
	}
	
	BOOL GetSourceCodePreservation()
	{
		T* pT = (T*)this;
		CComQIPtr<IDHTMLEdit> spDoc = pT->GetControlUnknown();
		
		ATLASSERT(spDoc != NULL);
	
		VARIANT_BOOL bSrcPrev;
		spDoc->get_SourceCodePreservation(&bSrcPrev);
		return bSrcPrev != ATL_VARIANT_FALSE ? TRUE : FALSE;
	}
	
	BOOL SetSourceCodePreservation(BOOL bNewValue)
	{
		T* pT = (T*)this;
		CComQIPtr<IDHTMLEdit> spDoc = pT->GetControlUnknown();
		
		ATLASSERT(spDoc != NULL);
		return S_OK == spDoc->put_SourceCodePreservation(bNewValue ? ATL_VARIANT_TRUE : ATL_VARIANT_FALSE);
	}
	
	BOOL GetAbsoluteDropMode()
	{
		T* pT = (T*)this;
		CComQIPtr<IDHTMLEdit> spDoc = pT->GetControlUnknown();
		
		ATLASSERT(spDoc != NULL);
		VARIANT_BOOL bAbsDropMode;
		spDoc->get_AbsoluteDropMode(&bAbsDropMode);
		return bAbsDropMode != ATL_VARIANT_FALSE ? TRUE : FALSE;
	}
	
	BOOL SetAbsoluteDropMode(BOOL bNewValue)
	{
		T* pT = (T*)this;
		CComQIPtr<IDHTMLEdit> spDoc = pT->GetControlUnknown();
		
		ATLASSERT(spDoc != NULL);
		return S_OK == spDoc->put_AbsoluteDropMode(bNewValue ? ATL_VARIANT_TRUE : ATL_VARIANT_FALSE);
	}
	
	long GetSnapToGridX()
	{
		T* pT = (T*)this;
		CComQIPtr<IDHTMLEdit> spDoc = pT->GetControlUnknown();
		
		ATLASSERT(spDoc != NULL);
		long nSnap;
		spDoc->get_SnapToGridX(&nSnap);
		return nSnap;
	}
	
	BOOL SetSnapToGridX(long nNewValue)
	{
		T* pT = (T*)this;
		CComQIPtr<IDHTMLEdit> spDoc = pT->GetControlUnknown();
		
		ATLASSERT(spDoc != NULL);
		return SUCCEEDED(spDoc->put_SnapToGridX(nNewValue));
	}
	
	long GetSnapToGridY()
	{
		T* pT = (T*)this;
		CComQIPtr<IDHTMLEdit> spDoc = pT->GetControlUnknown();
		
		ATLASSERT(spDoc != NULL);
		long nSnap;
		spDoc->get_SnapToGridY(&nSnap);
		return nSnap;
	}
	
	BOOL SetSnapToGridY(long nNewValue)
	{
		T* pT = (T*)this;
		CComQIPtr<IDHTMLEdit> spDoc = pT->GetControlUnknown();
		
		ATLASSERT(spDoc != NULL);
		return SUCCEEDED(spDoc->put_SnapToGridY(nNewValue));
	}
	
	BOOL GetSnapToGrid()
	{
		T* pT = (T*)this;
		CComQIPtr<IDHTMLEdit> spDoc = pT->GetControlUnknown();
		
		ATLASSERT(spDoc != NULL);
	
		VARIANT_BOOL bSnap;
		spDoc->get_SnapToGrid(&bSnap);
		return bSnap != ATL_VARIANT_FALSE ? TRUE : FALSE;
	}
	
	BOOL SetSnapToGrid(BOOL bNewValue)
	{
		T* pT = (T*)this;
		CComQIPtr<IDHTMLEdit> spDoc = pT->GetControlUnknown();
		
		ATLASSERT(spDoc != NULL);
		return S_OK == spDoc->put_SnapToGrid(bNewValue ? ATL_VARIANT_TRUE : ATL_VARIANT_FALSE);
	}
	
	BOOL GetIsDirty()
	{
		T* pT = (T*)this;
		CComQIPtr<IDHTMLEdit> spDoc = pT->GetControlUnknown();
		
		ATLASSERT(spDoc != NULL);
		VARIANT_BOOL bIsDirty;
		spDoc->get_IsDirty(&bIsDirty);
		return bIsDirty != ATL_VARIANT_FALSE ? TRUE : FALSE;
	}
	
	BOOL GetCurrentDocumentPath(BSTR *pszPath)
	{
		USES_CONVERSION;
		T* pT = (T*)this;
		CComQIPtr<IDHTMLEdit> spDoc = pT->GetControlUnknown();
		
		ATLASSERT(spDoc != NULL);
		return SUCCEEDED(spDoc->get_CurrentDocumentPath(pszPath));
	}
	
	BOOL GetBaseURL(BSTR *pszURL)
	{
		USES_CONVERSION;
		T* pT = (T*)this;
		CComQIPtr<IDHTMLEdit> spDoc = pT->GetControlUnknown();
		
		ATLASSERT(spDoc != NULL);
		return SUCCEEDED(spDoc->get_BaseURL(pszURL));
	}
	
	BOOL SetBaseURL(LPCTSTR lpszNewValue)
	{
		USES_CONVERSION;
		T* pT = (T*)this;
		CComQIPtr<IDHTMLEdit> spDoc = pT->GetControlUnknown();
		
		ATLASSERT(spDoc != NULL);
		CComBSTR var(lpszNewValue);
		return SUCCEEDED(spDoc->put_BaseURL(var));
	}
	
	BOOL GetDocumentTitle(BSTR* pszTitle)
	{
		USES_CONVERSION;
		T* pT = (T*)this;
		CComQIPtr<IDHTMLEdit> spDoc = pT->GetControlUnknown();
		
		ATLASSERT(spDoc != NULL);
		return SUCCEEDED(spDoc->get_DocumentTitle(pszTitle));
	}

	BOOL GetUseDivOnCarriageReturn()
	{
		T* pT = (T*)this;
		CComQIPtr<IDHTMLEdit> spDoc = pT->GetControlUnknown();
		
		ATLASSERT(spDoc != NULL);
		VARIANT_BOOL bValue;
		spDoc->get_UseDivOnCarriageReturn(&bValue);
		return bValue != ATL_VARIANT_FALSE ? TRUE : FALSE;
	}

	BOOL SetUseDivOnCarriageReturn(BOOL bNewValue)
	{
		T* pT = (T*)this;
		CComQIPtr<IDHTMLEdit> spDoc = pT->GetControlUnknown();
		
		ATLASSERT(spDoc != NULL);
		return S_OK == spDoc->put_UseDivOnCarriageReturn(bNewValue ? ATL_VARIANT_TRUE : ATL_VARIANT_FALSE);
	}

	BOOL GetBusy()
	{
		T* pT = (T*)this;
		CComQIPtr<IDHTMLEdit> spDoc = pT->GetControlUnknown();
		
		ATLASSERT(spDoc != NULL);
		VARIANT_BOOL bValue;
		spDoc->get_Busy(&bValue);
		return bValue != ATL_VARIANT_FALSE ? TRUE : FALSE;
	}

	BOOL GetBrowseMode()
	{
		T* pT = (T*)this;
		CComQIPtr<IDHTMLEdit> spDoc = pT->GetControlUnknown();
		
		ATLASSERT(spDoc != NULL);
		VARIANT_BOOL bMode;
		spDoc->get_BrowseMode(&bMode);
		return bMode != ATL_VARIANT_FALSE ? TRUE : FALSE;
	}
	
	BOOL SetBrowseMode(BOOL bNewValue)
	{
		T* pT = (T*)this;
		CComQIPtr<IDHTMLEdit> spDoc = pT->GetControlUnknown();
		
		ATLASSERT(spDoc != NULL);
		return S_OK == spDoc->put_BrowseMode(bNewValue ? ATL_VARIANT_TRUE : ATL_VARIANT_FALSE);
	}
	
//Commands
	BOOL Bold() const
	{
		return ExecCommand(DECMD_BOLD, OLECMDEXECOPT_DODEFAULT, NULL, NULL);
	}

	BOOL Copy() const
	{
		return ExecCommand(DECMD_COPY, OLECMDEXECOPT_DODEFAULT, NULL, NULL);
	}

	BOOL Cut() const
	{
		return ExecCommand(DECMD_CUT, OLECMDEXECOPT_DODEFAULT, NULL, NULL);
	}

	BOOL Delete() const
	{
		return ExecCommand(DECMD_DELETE, OLECMDEXECOPT_DODEFAULT, NULL, NULL);
	}	

	BOOL DeleteCells() const
	{
		return ExecCommand(DECMD_DELETECELLS, OLECMDEXECOPT_DODEFAULT, NULL, NULL);
	}

	BOOL DeleteColumns() const
	{
		return ExecCommand(DECMD_DELETECOLS, OLECMDEXECOPT_DODEFAULT, NULL, NULL);
	}

	BOOL DeleteRows() const
	{
		return ExecCommand(DECMD_DELETEROWS, OLECMDEXECOPT_DODEFAULT, NULL, NULL);
	}

	BOOL FindText() const
	{
		return ExecCommand(DECMD_FINDTEXT, OLECMDEXECOPT_DODEFAULT, NULL, NULL);
	}

	BOOL Font() const
	{
		return ExecCommand(DECMD_FONT,OLECMDEXECOPT_DODEFAULT,NULL, NULL);
	}

	BOOL GetBackColor(BSTR *pszColor) const
	{
		VARIANT vaRet;
		VariantInit(&vaRet);
		if(!pszColor)
			return FALSE;
		
		if (ExecCommand(DECMD_GETBACKCOLOR,OLECMDEXECOPT_DODEFAULT,
			NULL, &vaRet) && vaRet.vt==VT_BSTR)
		{
			*pszColor = vaRet.bstrVal;
			return TRUE;
		}

		return FALSE;
	}

	BOOL GetBlockFormat(BSTR *pszFormat) const
	{
		USES_CONVERSION;
		VARIANT vaRet;
		VariantInit(&vaRet);
		if (!pszFormat)
			return FALSE;

		if (ExecCommand(DECMD_GETBLOCKFMT,OLECMDEXECOPT_DODEFAULT,
			NULL, &vaRet) && vaRet.vt==VT_BSTR)
			{
				*pszFormat = vaRet.bstrVal;
				return TRUE;
			}

		return FALSE;
	}

	BOOL GetBlockFormatNames(SAFEARRAY **ppArray)
	{
		CComPtr<IDEGetBlockFmtNamesParam> spFmt;
		BOOL bRet = FALSE;
	
		//create a block format holder object
		if (FAILED(spFmt.CoCreateInstance(L"DEGetBlockFmtNamesParam.DEGetBlockFmtNamesParam")))
			return FALSE;
		
		CComVariant vaRet;
		CComVariant vaNameObj((LPUNKNOWN)spFmt);
		if (ExecCommand(DECMD_GETBLOCKFMTNAMES,OLECMDEXECOPT_DONTPROMPTUSER,&vaNameObj, NULL))
		{						 
			VariantInit(&vaRet);
			spFmt->get_Names(&vaRet);
			if((vaRet.vt & VT_ARRAY) && (vaRet.vt & VT_BSTR) && ppArray)
			{
				SafeArrayCopy(vaRet.parray, ppArray);
				bRet = TRUE;
			}
		}
	
		return bRet;
	}

	BOOL GetFontFace(BSTR *pszFace) const
	{
		USES_CONVERSION;
		VARIANT vaRet;
		VariantInit(&vaRet);
		if (!pszFace)
			return FALSE;
		
		if (ExecCommand(DECMD_GETFONTNAME,OLECMDEXECOPT_DODEFAULT,NULL,
			&vaRet) && vaRet.vt==VT_BSTR)
		{
			*pszFace = vaRet.bstrVal;
			return TRUE;
		}
		return FALSE;
	}

	short GetFontSize() const
	{
		CComVariant vaRet;
		if (ExecCommand(DECMD_GETFONTSIZE,OLECMDEXECOPT_DODEFAULT,NULL,&vaRet ) && vaRet.vt==VT_I4)
			return vaRet.iVal;
		return -1;
	}

	BOOL GetForeColor(BSTR *pszColor) const
	{
		USES_CONVERSION;
		VARIANT vaRet;
		VariantInit(&vaRet);
		if (!pszColor)
			return FALSE;

		if (ExecCommand(DECMD_GETFORECOLOR,OLECMDEXECOPT_DODEFAULT,
			NULL, &vaRet) && vaRet.vt==VT_BSTR)
		{
			*pszColor, vaRet.bstrVal;
			return TRUE;
		}
		return FALSE;
	}

	BOOL HyperLink() const
	{
		return ExecCommand(DECMD_HYPERLINK, OLECMDEXECOPT_DODEFAULT, NULL, NULL);
	}

	BOOL Image() const
	{
		return ExecCommand(DECMD_IMAGE, OLECMDEXECOPT_DODEFAULT, NULL, NULL);
	}

	BOOL Indent() const
	{
		return ExecCommand(DECMD_INDENT, OLECMDEXECOPT_DODEFAULT, NULL, NULL);
	}

	BOOL InsertCell() const
	{
		return ExecCommand(DECMD_INSERTCELL, OLECMDEXECOPT_DODEFAULT, NULL, NULL);
	}

	BOOL InsertColumn() const
	{
		return ExecCommand(DECMD_INSERTCOL, OLECMDEXECOPT_DODEFAULT, NULL, NULL);
	}

	BOOL InsertRow() const
	{
		return ExecCommand(DECMD_INSERTROW, OLECMDEXECOPT_DODEFAULT, NULL, NULL);
	}

	BOOL InsertTable(DWORD dwRows, DWORD dwCols, LPCTSTR szTableAttr, LPCTSTR szCellAttr, LPCTSTR szCaption) const
	{
		CComPtr<IDEInsertTableParam> spTable = NULL;
	
		VARIANT vaTable;
		VariantInit(&vaTable);
		USES_CONVERSION;
	
		if(FAILED(spTable.CoCreateInstance(L"DEInsertTableParam.DEInsertTableParam")))
			return FALSE;
	
		CComBSTR sztable(szTableAttr), szcell(szCellAttr), szcap(szCaption);

		//set all of the table object's properties
		if(FAILED(spTable->put_NumRows(dwRows)))
			return FALSE;
		if(FAILED(spTable->put_NumCols(dwCols)))
			return FALSE;
		if(FAILED(spTable->put_TableAttrs(sztable)))
			return FALSE;
		if(FAILED(spTable->put_CellAttrs(szcell)))
			return FALSE;
		if(FAILED(spTable->put_Caption(szcap)))
			return FALSE;
	
		vaTable.vt = VT_UNKNOWN;
		vaTable.punkVal = (LPUNKNOWN) spTable;
	
		if(!ExecCommand(DECMD_INSERTTABLE,OLECMDEXECOPT_DONTPROMPTUSER, &vaTable,NULL))
			return FALSE;
		
		return TRUE;
	}

	BOOL Italic() const
	{
		return ExecCommand(DECMD_ITALIC, OLECMDEXECOPT_DODEFAULT, NULL, NULL);
	}

	BOOL JustifyCenter() const
	{
		return ExecCommand(DECMD_JUSTIFYCENTER, OLECMDEXECOPT_DODEFAULT, NULL, NULL);
	}

	BOOL JustifyLeft() const
	{
		return ExecCommand(DECMD_JUSTIFYLEFT, OLECMDEXECOPT_DODEFAULT, NULL, NULL);
	}

	BOOL JustifyRight() const
	{
		return ExecCommand(DECMD_JUSTIFYRIGHT, OLECMDEXECOPT_DODEFAULT, NULL, NULL);
	}

	BOOL LockElement() const
	{
		return ExecCommand(DECMD_LOCK_ELEMENT, OLECMDEXECOPT_DODEFAULT, NULL, NULL);
	}

	BOOL MakeAbsolute() const
	{
		return ExecCommand(DECMD_MAKE_ABSOLUTE, OLECMDEXECOPT_DODEFAULT, NULL, NULL);
	}

	BOOL MergeCells() const
	{
		return ExecCommand(DECMD_MERGECELLS, OLECMDEXECOPT_DODEFAULT, NULL, NULL);
	}

	BOOL OrderList() const
	{
		return ExecCommand(DECMD_ORDERLIST, OLECMDEXECOPT_DODEFAULT, NULL, NULL);
	}

	BOOL Outdent() const
	{
		return ExecCommand(DECMD_OUTDENT, OLECMDEXECOPT_DODEFAULT, NULL, NULL);
	}

	BOOL Paste() const
	{
		return ExecCommand(DECMD_PASTE, OLECMDEXECOPT_DODEFAULT, NULL, NULL);
	}

	BOOL Redo() const
	{
		return ExecCommand(DECMD_REDO, OLECMDEXECOPT_DODEFAULT, NULL, NULL);
	}

	BOOL RemoveFormat() const
	{
		return ExecCommand(DECMD_REMOVEFORMAT, OLECMDEXECOPT_DODEFAULT, NULL, NULL);
	}

	BOOL SelectAll() const
	{
		return ExecCommand(DECMD_SELECTALL, OLECMDEXECOPT_DODEFAULT, NULL, NULL);
	}

	BOOL SendBackward() const
	{
		return ExecCommand(DECMD_SEND_BACKWARD, OLECMDEXECOPT_DODEFAULT, NULL, NULL);
	}

	BOOL BringForward() const
	{
		return ExecCommand(DECMD_BRING_FORWARD, OLECMDEXECOPT_DODEFAULT, NULL, NULL);
	}

	BOOL SendBelowText() const
	{
		return ExecCommand(DECMD_SEND_BELOW_TEXT, OLECMDEXECOPT_DODEFAULT, NULL, NULL);
	}
	
	BOOL BringAboveText() const
	{
		return ExecCommand(DECMD_BRING_ABOVE_TEXT, OLECMDEXECOPT_DODEFAULT, NULL, NULL);
	}

	BOOL SendToBack() const
	{
		return ExecCommand(DECMD_SEND_TO_BACK, OLECMDEXECOPT_DODEFAULT, NULL, NULL);
	}
	
	BOOL BringToFront() const
	{
		return ExecCommand(DECMD_BRING_TO_FRONT, OLECMDEXECOPT_DODEFAULT, NULL, NULL);
	}

	BOOL SetBackColor(LPCTSTR szColor) const
	{
		CComVariant vaName(szColor);
		return ExecCommand(DECMD_SETBACKCOLOR,OLECMDEXECOPT_DODEFAULT,&vaName, NULL);
	}

	BOOL SetBlockFormat(LPCTSTR szFormat) const
	{
		CComVariant vaName(szFormat);
		return ExecCommand(DECMD_SETBLOCKFMT,OLECMDEXECOPT_DODEFAULT,&vaName, NULL);
	}

	BOOL SetFontFace(LPCTSTR szFont) const
	{
		CComVariant vaName(szFont);
		return ExecCommand(DECMD_SETFONTNAME,OLECMDEXECOPT_DODEFAULT,&vaName, NULL);
	}

	BOOL SetFontSize(short size) const
	{
		CComVariant vaSize(size);
		return ExecCommand(DECMD_SETFONTSIZE,OLECMDEXECOPT_DODEFAULT,&vaSize, NULL);
	}
	
	BOOL SetForeColor(LPCTSTR szColor) const
	{
		CComVariant vaName(szColor);
		return ExecCommand(DECMD_SETFORECOLOR,OLECMDEXECOPT_DODEFAULT,&vaName, NULL);
	}

	BOOL SplitCell() const
	{
		return ExecCommand(DECMD_SPLITCELL, OLECMDEXECOPT_DODEFAULT, NULL, NULL);
	}

	BOOL Underline() const
	{
		return ExecCommand(DECMD_UNDERLINE, OLECMDEXECOPT_DODEFAULT, NULL, NULL);
	}

	BOOL Undo() const
	{
		return ExecCommand(DECMD_UNDO, OLECMDEXECOPT_DODEFAULT, NULL, NULL);
	}

	BOOL Unlink() const
	{
		return ExecCommand(DECMD_UNLINK, OLECMDEXECOPT_DODEFAULT, NULL, NULL);
	}

	BOOL UnorderList() const
	{
		return ExecCommand(DECMD_UNORDERLIST, OLECMDEXECOPT_DODEFAULT, NULL, NULL);
	}

	BOOL Properties() const
	{
		return ExecCommand(DECMD_PROPERTIES, OLECMDEXECOPT_DODEFAULT, NULL, NULL);
	}
}; //CHtmlEditCtrlBase

#endif //__HTMLEDBASE_H__
