//////////////////////////////////////////////////////////////////////////////////////////////
// vcstring.h: CString-like class that can be used in both MFC and ATL projects (CString
// is one or the other, but not allowed to be both...)

#pragma once

#if _MSC_VER < 1300

// NOTE: this class looks more or less identical to StrTraitATL except that it is OK to 
// use it in mixed MFC/ATL projects
class StrVCTraitATL : public ChTraitsOS<TCHAR>
{
public:
	static HINSTANCE GetResourceHandle()
	{
		return _pModule->GetResourceInstance();
	}

	static void ThrowMemoryException()
	{
		CSTRING_ASSERT(FALSE);
	}

	static void ThrowResourceException()
	{
		CSTRING_ASSERT(FALSE);
	}

	static CStringData* GetMemory(int nLen)
	{
		// nLen is in XCHARs
		CStringData* pData;

		int nTotal = sizeof(CStringData) + (nLen+1)*sizeof(TCHAR);
		pData = (CStringData*) malloc(nTotal);
		pData->nAllocLength = nLen;
		return pData;
	}

	static void ReleaseMemory(CStringData* pVoid)
	{
		free(pVoid);
	}
		
	static void Trace(const char* pstr)
	{
		OutputDebugStringA(pstr);
	}

	static void Trace(const char* pstr, unsigned int n)
	{
		char szBuffer[512];
		wsprintfA(szBuffer, pstr, n);
		OutputDebugStringA(szBuffer);
	}

	static int LoadStringImpl(HINSTANCE hRes, UINT nID,	TCHAR* lpszBuf, UINT nMaxBuf)
	{
		CSTRING_ASSERT(CStringIsValidAddress(lpszBuf, nMaxBuf));
#ifdef _DEBUG
		// LoadString without annoying warning from the Debug kernel if the
		//  segment containing the string is not present
		if (::FindResource(hRes, MAKEINTRESOURCE((nID>>4)+1),
			RT_STRING) == NULL)
		{
			lpszBuf[0] = '\0';
			return 0; // not found
		}
#endif //_DEBUG
		int nLen = LoadWindowsString(hRes, nID, lpszBuf, nMaxBuf);
		if (nLen == 0)
			lpszBuf[0] = '\0';
		return nLen;
	}
};

typedef CStringT<TCHAR, StrVCTraitATL> CVCString;

#else  // _MSC_VER < 1300

#include <atlstr.h>

//#ifdef _ATL_CSTRING_NO_STRMGRTRAITS
typedef ATL::CAtlString CVCString;
//#else
//typedef CStringT< TCHAR, StrTraitATL< TCHAR > > CVCString;
//#endif

#endif  // _MSC_VER < 1300




