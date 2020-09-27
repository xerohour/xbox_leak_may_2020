#include "stdafx.h"
#include "mfcappwz.h"
#include "myaw.h"		// also includes "customaw.h"
#include "codegen.h"
#include "lang.h"

extern HINSTANCE GetResourceHandle();

/////////////////////////////////////////////////////////////////////////////
// customaw.cpp -- Implementation of CCustomAppWiz & CMyCustomAppWiz

CMyCustomAppWiz MyAWX;

// Based on lpszResource's extension, we determined whether the resource
//  is located in rsc-only DLL or in mfcapwz.dll.  We have a special return
//  value for when the extension is .rtf, so we know whether we should try
//  .rft.  (See FindExtensionResource below.)
enum { LOC_NO, LOC_YES, LOC_YES_RTF };
int IsLocalizedTemplate(LPCTSTR lpszResource)
{
	LPCTSTR ext = _tcschr(lpszResource, '.')+1;
	if (!_tcscmp(ext, "rtf"))
		return LOC_YES_RTF;

	if (!_tcscmp(ext, "rc") || !_tcscmp(ext, "rc2") || !_tcscmp(ext, "rft")
		|| !_tcscmp(ext, "bmp") || !_tcscmp(ext, "hpj"))
		return LOC_YES;

	return LOC_NO;
}


// In the olden days, AppWizard separated its templates into two resource
//  types: SRCDATA & BINDATA.  They're now combined into one type: "TEMPLATE".
//  For backward compatibility, we continue to check the original two
//  types if necessary.
// TODO: When the appwiz*.dlls are updated, GET RID OF THIS!!!
/*HRSRC FindResourceAnyType(HINSTANCE hInstance, LPCTSTR lpszResource)
{
	HRSRC hrsrc = ::FindResource(hInstance, lpszResource, "TEMPLATE");
	if (hrsrc == NULL)
	{
		hrsrc = ::FindResource(hInstance, lpszResource, "SRCDATA");
		if (hrsrc == NULL)
			hrsrc = ::FindResource(hInstance, lpszResource, "BINDATA");
	}
	return hrsrc;
}*/

// Helper used in FindExtensionResource.  Creates name like "loc_enu.rc" from "loc.rc"
static void MakeLocTplName(CString& rStrResource, LPCTSTR lpszResource, LPCTSTR lpszDefaultLang)
{
	ASSERT(lpszResource != NULL);
	ASSERT(lpszDefaultLang != NULL);

	LPCTSTR lpszPeriod = _tcschr(lpszResource, '.');
	if (lpszPeriod != NULL)
	{
		// Append '_langSuffix' just before first period
		rStrResource = lpszResource;
		rStrResource = ((rStrResource.Left(lpszPeriod - lpszResource)
							+ '_') + lpszDefaultLang) + lpszPeriod;
	}
	else
	{
		// If there is no period, just append '_langSuffix' to end
		rStrResource = (((CString) lpszResource) + '_') + lpszDefaultLang;
	}

}

// Looks for specified custom resource...
//  1) In hInstance; if not there...
//  2) Through normal route of mfcapwz.dll & appropriate appwiz*.dll.
// This allows the extension to simply specify AppWizard templates, without
//  having to provide its own copies of them.
// When the function returns, the HINSTANCE used to load the resource is
//  stored in hInstance.
HRSRC FindExtensionResource
	(LPCTSTR lpszResource, HINSTANCE& hInstance)
{
	ASSERT(lpszResource != NULL);

	HRSRC hrsrc;
	if (hInstance != NULL)
	{
		LPCTSTR lpszDefaultLang;
		if (IsUserAWX() && 
			(lpszDefaultLang = langDlls.GetDefaultLang()) != NULL)
		{
			// Extensions have their own way of handling multiple languages.  First,
			//  we try appending _langSuffix (like _enu) to end of template name.
			CString strResource;
			MakeLocTplName(strResource, lpszResource, lpszDefaultLang);
			hrsrc = ::FindResource(hInstance, strResource, "TEMPLATE");
			if (hrsrc != NULL)
				return hrsrc;
		}

		// If that doesn't work, we just look for it under its actual name
		hrsrc = ::FindResource(hInstance, lpszResource, "TEMPLATE");
		if (hrsrc != NULL)
			// We found in the extension itself
			return hrsrc;
	}

	// The template is not in the extension (or there is no extension).
	//  Try mfcapwz.dll next.

	hrsrc = ::FindResource(hInstance = GetResourceHandle(), lpszResource, "TEMPLATE");

	if (hrsrc == NULL)
	{
		// The template is not in mfcapwz.dll.  Let's try the langDlls.
		hrsrc = langDlls.FindLangResource(lpszResource, hInstance);
	}
	return hrsrc;
}

// We wrap all calls to LoadTemplate, since the custom appwiz may override it, and we need to
//  be sure its return value is non-NULL.  (We can be sure that *our* LoadTemplate will never
//  return NULL, since it will throw an exception in that case before it can return.)
LPCTSTR LoadTemplateWrapper(LPCTSTR lpszResource, DWORD& dwSize, HINSTANCE hInstance /* = NULL */)
{
	LPCTSTR szReturn = GetAWX()->LoadTemplate(lpszResource, dwSize, hInstance);
	if (szReturn == NULL)
	{
		if (IsUserAWX())
		{
			// It's the custom appwiz's fault
			ReportAndThrowExt(IDS_CG_CANT_LOAD_TPL, lpszResource);
		}
		else
		{
			// It must be the appwz dll's fault
			ASSERT(FALSE);
			ReportAndThrow(IDP_BAD_LANG_DLL, langDlls.m_astrNameDll[0]);
		}
	}
	return szReturn;
}

LPCTSTR CCustomAppWiz::LoadTemplate(LPCTSTR lpszResource,
	DWORD& dwSize, HINSTANCE hInstance /* = NULL */)
{
	ASSERT_VALID(this);
	ASSERT(lpszResource != NULL);

	// Get handle to resource if it wasn't already passed to us
	extern BOOL g_bLoadFromAppwiz;	// Defined at top of codegen.cpp
	if (hInstance == NULL && !g_bLoadFromAppwiz)
		hInstance = projOptions.m_UserExtension.GetInstanceHandle();
	g_bLoadFromAppwiz = FALSE;		// Reset flag now that we're done w/ it

	// If hInstance is still NULL, that means we're probably being called
	//  by CMyCustomAppWiz, so just look through mfcapwz.dll & appwz*.dll.
	// TODO: TRACE. This is a great place to put an extension debugging TRACE.

	// Find, load, and lock resource.
	HRSRC hrsrc = FindExtensionResource(lpszResource, hInstance);
	if (hrsrc == NULL)
	{
		if (IsUserAWX())
		{
			// It's the custom appwiz's fault
			ReportAndThrowExt(IDS_CG_CANT_LOAD_TPL, lpszResource);
		}
		else
		{
			// It must be the appwz dll's fault
			ASSERT(FALSE);
			ReportAndThrow(IDP_BAD_LANG_DLL, langDlls.m_astrNameDll[0]);
		}
	}

	// Get size-- better be nonzero or we tattle
	dwSize = ::SizeofResource(hInstance, hrsrc);
	if (dwSize == 0)
	{
		if (IsUserAWX())
		{
			// It's the extension's fault
			ReportAndThrowExt(IDS_CG_0SIZE_RSC, lpszResource);
		}
		else
		{
			// It must be the appwz dll's fault
			ASSERT(FALSE);
			ReportAndThrow(IDP_BAD_LANG_DLL, langDlls.m_astrNameDll[0]);
		}
	}

	// Finally load & lock resource-- better be non-NULL or we tattle
	HGLOBAL hglb = ::LoadResource(hInstance, hrsrc);
	LPCTSTR lpszTemplate = NULL;
	if (hglb == NULL || (lpszTemplate = (LPCTSTR) ::LockResource(hglb)) == NULL)
	{
		if (IsUserAWX())
		{
			// It's the extension's fault
			ReportAndThrowExt(IDS_CG_BAD_RSC, lpszResource);
		}
		else
		{
			// It must be the appwz dll's fault
			ASSERT(FALSE);
			ReportAndThrow(IDP_BAD_LANG_DLL, langDlls.m_astrNameDll[0]);
		}
	}
	return lpszTemplate;
}


// This is basically the same as base class's LoadTemplate, but with an error messagebox
LPCTSTR CMyCustomAppWiz::LoadTemplate(LPCTSTR lpszResource,
	DWORD& dwSize, HINSTANCE hInstance)
{
	ASSERT_VALID(this);
	ASSERT(lpszResource != NULL);

	LPCTSTR pTempStrm
		= CCustomAppWiz::LoadTemplate(lpszResource, dwSize, hInstance);

	if (pTempStrm == NULL)
	{
		// We couldn't load the template.
		ASSERT(FALSE);

		// It must be the appwz DLL's fault, so report message
		CString strPrompt;
		AfxFormatString1(strPrompt, IDP_BAD_LANG_DLL, langDlls.m_astrNameDll[0]);
		AfxMessageBox(strPrompt);
		AfxThrowResourceException();
    }
	return pTempStrm;
}


// Takes a template (like a binary file) and copies it verbatim to output, without
//  doing any parsing.
void CCustomAppWiz::CopyTemplate(LPCTSTR lpszInput, DWORD dwSize, OutputStream* pOutput)
{
	pOutput->WriteBlock(lpszInput, dwSize);
}


#define LOC_TAG	'@'

// This is now called only for templates which must be parsed & processed.  No
//  binary files!
void CCustomAppWiz::ProcessTemplate(LPCTSTR lpszInput, DWORD dwSize, OutputStream* pOutput)
{
	ASSERT_VALID(this);
	// If lpszInput is NULL, this error is reported in Go.

	// We need to strip the localization tags out of the template
	DWORD dwStippedSize = 0;
	LPTSTR pstr = (LPTSTR)malloc(dwSize);
	LPTSTR pOut = pstr;
	LPCTSTR pIn = lpszInput;
	LPCTSTR pInEnd = pIn + dwSize;
	TCHAR c;
#ifdef _DEBUG
	BOOL fInTags = FALSE;
#endif // _DEBUG
	if(!pstr)
		return;
	while(pIn < pInEnd)
	{
		c = *pIn++;
		if(c != LOC_TAG)
		{
			*pOut++ = c;
			if(isleadbyte(c) && pIn < pInEnd)
				*pOut++ = *pIn++;
		}
		else
		{
			// look ahead one
			if(pIn < pInEnd)
			{
				c = *pIn;
				if(c != LOC_TAG)
				{
					*pOut++ = LOC_TAG;
				}
				else
				{
					// Skip the Tags
					pIn++;
#if _DEBUG
					fInTags = !fInTags;
#endif // _DEBUG
				}
			}
			else
				*pOut++ = LOC_TAG;
		}
		// Cannot have a localization tag that spans more than one line!
		// You need to split them up if necessary
		ASSERT(c != '\n' || !fInTags);
	}
	CCodeGen codeGen;
	codeGen.Go(pstr, pOut-pstr, pOutput, &m_Dictionary);
	free(pstr);
}
