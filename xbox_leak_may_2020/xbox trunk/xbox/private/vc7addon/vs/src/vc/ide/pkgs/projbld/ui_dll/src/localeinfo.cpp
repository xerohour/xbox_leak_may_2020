// localeinfo.cpp : Implementation of CLocaleInfo
#include "stdafx.h"

#include "localeinfo.h"
#include "path2.h"
#include "locale.h"
#include "register.h" // for CreateBaseKey
#include "profile.h"	// for GetRegString
#include "bldpkg.h"	// for ExternalQueryService

CLocaleInfo::CLocaleInfo()
{
	m_hInst = NULL;
	m_hInstMain = NULL;
}

CLocaleInfo::CLocaleInfo(HINSTANCE hInst, IServiceProvider *pServiceProvider)
{
	m_hInst = NULL;
	m_hInstMain = hInst;
}


CLocaleInfo::~CLocaleInfo() 
{
}


// returns NULL if can't find.
HINSTANCE CLocaleInfo::GetUIHostLib()
{
	if (m_hInst == NULL)
	{
		HRESULT hr = E_FAIL;
		HINSTANCE hInstUIDll = NULL;
		CComPtr<IUIHostLocale> pUIHostLocale;
		if (SUCCEEDED(ExternalQueryService(SID_SUIHostLocale, __uuidof(IUIHostLocale) /*IID_IUIHostLocale*/, (LPVOID *)&pUIHostLocale)))
		{
			CComQIPtr<IUIHostLocale2> pUIHostLocale2 = pUIHostLocale;
			if (pUIHostLocale2)
			{
				if (m_hInstMain)
				{
					CStringW strRoot = CBuildPackage::s_bstrAltKey;

					CComBSTR bstrPath;
					CComBSTR bstrDllName;
					LPOLESTR lpszBuf = NULL;
					StringFromCLSID(__uuidof(BuildPackage), &lpszBuf);
					CStringW strRegKey = strRoot + L"\\Packages\\";
					strRegKey += lpszBuf;
					strRegKey += L"\\SatelliteDLL";
					::CoTaskMemFree(lpszBuf);
					
					hr = GetRegStringW(strRegKey, L"Path", &bstrPath );
					hr = GetRegStringW(strRegKey, L"DllName", &bstrDllName );

					if( bstrPath.Length() == 0 || bstrDllName.Length() == 0 )
					{
						// Couldn't get the info from the registry. Fallback to 1033 directory
                        CStringW strModuleFileName;
						if (UtilGetModuleFileNameW(_Module.GetModuleInstance(), strModuleFileName))
						{
							wchar_t drive[_MAX_DRIVE];
							wchar_t dir[_MAX_DIR];
							wchar_t fname[_MAX_FNAME];
							wchar_t ext[_MAX_EXT];

							_wsplitpath( strModuleFileName, drive, dir, fname, ext );
							CStringW strDllName = drive;	
							strDllName += dir;	
							strDllName += L"1033\\";	
							strDllName += fname;
							strDllName += L"UI";
							strDllName += ext;
							hInstUIDll = LoadLibraryW( strDllName );
						}
					}
					else
					{
						hr = pUIHostLocale2->LoadUILibrary(bstrPath, bstrDllName, /*LOAD_LIBRARY_AS_DATAFILE |*/ DONT_RESOLVE_DLL_REFERENCES, (DWORD_PTR *)&hInstUIDll);
						VSASSERT(SUCCEEDED(hr) && hInstUIDll, "Failed to load the UI library");
					}
				}
			}
		}
		if(SUCCEEDED(hr))
		{
			m_hInst = hInstUIDll;
		}
	}
	return(m_hInst);
}




