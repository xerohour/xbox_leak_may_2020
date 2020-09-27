#pragma once

// VCASSERT_*
//

#define VCASSERT(expression) VSASSERT(expression, #expression)

//SetErrorInfo
//
inline HRESULT VCSetErrorInfo(DWORD ResID, HRESULT hResult)
{
#if 0
	va_list args; va_start(args, hResult);
	CComBSTR bstrText;
	CStringW strText;

	if(bstrText.LoadString(_pModule->GetResourceInstance(), ResID))
	{
		strText.Format(bstrText, args);
		return AtlReportError(GUID_NULL, strText, IID_NULL, hResult);
	}
	return hResult;
#else
	return AtlReportError(GUID_NULL, ResID, IID_NULL, hResult, _pModule->GetResourceInstance());
#endif
}

inline HRESULT VCSetErrorInfo(DWORD ResID, LPCWSTR wstrName, HRESULT hResult, const IID & riid)
{
	CComBSTR bstrText;
	CStringW strText;

	if(bstrText.LoadString(_pModule->GetResourceInstance(), ResID))
	{
		strText.Format(bstrText, wstrName);
		return AtlReportError(GUID_NULL, strText, riid, hResult);
	}
	return hResult;
}
inline HRESULT VCSetErrorInfo(DWORD ResID, LPCWSTR wstrName, LPCWSTR wstrName2, HRESULT hResult, const IID & riid)
{
	CComBSTR bstrText;
	CStringW strText;

	if(bstrText.LoadString(_pModule->GetResourceInstance(), ResID))
	{
		strText.Format(bstrText, wstrName, wstrName2);
		return AtlReportError(GUID_NULL, strText, riid, hResult);
	}
	return hResult;
}
inline HRESULT VCSetErrorInfo(DWORD ResID, LPCWSTR wstrName, LPCWSTR wstrName2, LPCWSTR wstrName3, HRESULT hResult, const IID & riid)
{
	CComBSTR bstrText;
	CStringW strText;

	if(bstrText.LoadString(_pModule->GetResourceInstance(), ResID))
	{
		strText.Format(bstrText, wstrName, wstrName2, wstrName3);
		return AtlReportError(GUID_NULL, strText, riid, hResult);
	}
	return hResult;
}

inline HRESULT VCSetErrorInfo(DWORD ResID, LPCWSTR wstrName, HRESULT hResult)
{
	return VCSetErrorInfo(ResID, wstrName, hResult, IID_NULL);
}

// FAILED_*
//

// Just to be able to set a breakpoint and stop if something
// fails
//

inline bool VCFailedBreakpoint(void)
{
	static bool bBreakOnFailure(false);
	return bBreakOnFailure;
}

template<typename TResult>
bool VCFailed(TResult tr)
{
	if (TRUE == !tr)
	{
		return true;
	}
	else
	{
		return false;
	}
}

inline bool VCFailed(HRESULT hr)
{
	if (FAILED(hr))
	{
		return true;
	}
	else
	{
		return false;
	}
}

inline bool VCFailedStrict(HRESULT hr)
{
	if (S_OK != hr)
	{
		return true;
	}
	else
	{
		return false;
	}
}

class CVCUSetErrorInfo
{
public :

	const UINT m_nID;
	const UINT m_cpwsz;
	const WCHAR * m_rgpwsz[3]; // For now, just handle up to three optional strings for Format().

	CVCUSetErrorInfo(UINT nID, const WCHAR * pwszA = NULL, const WCHAR * pwszB = NULL, const WCHAR * pwszC = NULL) 
		: m_nID(nID), m_cpwsz((pwszA ? 1 : 0) + (pwszB ? 1 : 0) + (pwszC ? 1 : 0))
	{
		m_rgpwsz[0] = pwszA;
		m_rgpwsz[1] = pwszB;
		m_rgpwsz[2] = pwszC;
	}
};

template<typename TResult>
bool VCFailedSEI(TResult tr, const CVCUSetErrorInfo & sei)
{
	if (VCFailed(tr))
	{
		HRESULT hrSEI(S_OK);

		CStringW wstrMessageRaw;
		CStringW wstrMessage;

		hrSEI = wstrMessageRaw.LoadString(sei.m_nID);
		VCASSERT(SUCCEEDED(hrSEI));

		switch (sei.m_cpwsz)
		{
		case 0 :
			wstrMessage = wstrMessageRaw;
			break;
		case 1 :
			wstrMessage.Format(wstrMessageRaw, sei.m_rgpwsz[0]);
			break;
		case 2 :
			wstrMessage.Format(wstrMessageRaw, sei.m_rgpwsz[0], sei.m_rgpwsz[1]);
			break;
		case 3 :
			wstrMessage.Format(wstrMessageRaw, sei.m_rgpwsz[0], sei.m_rgpwsz[1], sei.m_rgpwsz[2]);
			break;
		default :
			VSFAIL(L"Need to add more space in CVCUSetErrorInfo");
			break;
		}

		hrSEI = AtlReportError(CLSID_NULL, wstrMessage, GUID_NULL, E_FAIL);
		VCASSERT(E_FAIL == hrSEI);

		return true;
	}

	return false;
}

inline bool VCFailedSEI(HRESULT hr, const CVCUSetErrorInfo & sei)
{
	if (VCFailed(hr))
	{
		HRESULT hrSEI(S_OK);

		CStringW wstrMessageRaw;
		CStringW wstrMessage;

		hrSEI = wstrMessageRaw.LoadString(sei.m_nID);
		VCASSERT(SUCCEEDED(hrSEI));

		switch (sei.m_cpwsz)
		{
		case 0 :
			wstrMessage = wstrMessageRaw;
			break;
		case 1 :
			wstrMessage.Format(wstrMessageRaw, sei.m_rgpwsz[0]);
			break;
		case 2 :
			wstrMessage.Format(wstrMessageRaw, sei.m_rgpwsz[0], sei.m_rgpwsz[1]);
			break;
		case 3 :
			wstrMessage.Format(wstrMessageRaw, sei.m_rgpwsz[0], sei.m_rgpwsz[1], sei.m_rgpwsz[2]);
			break;
		default :
			VSFAIL(L"Need to add more space in CVCUSetErrorInfo");
			break;
		}

		hrSEI = AtlReportError(CLSID_NULL, wstrMessage, GUID_NULL, hr);
		VCASSERT(hr == hrSEI);

		return true;
	}

	return false;
}

#define FAILED_RETURN_HR(expression) \
\
do \
{ \
	if (VCFailed(hr = (expression))) \
	{ \
		if (VCFailedBreakpoint()) \
		{ \
			VSFAIL(#expression); \
		} \
		\
		return hr; \
	} \
} while (false)

#define FAILED_STRICT_RETURN_HR(expression) \
\
do \
{ \
	if (VCFailedStrict(hr = (expression))) \
	{ \
		if (VCFailedBreakpoint()) \
		{ \
			VSFAIL(#expression); \
		} \
		\
		return hr; \
	} \
} while (false)

#define FAILED_SEI_RETURN_HR(argv, expression) \
\
do \
{ \
	if (VCFailed(hr = (expression))) \
	{ \
		VCFailedSEI(hr, CVCUSetErrorInfo##argv); \
		\
		if (VCFailedBreakpoint()) \
		{ \
			VSFAIL(#expression); \
		} \
		\
		return hr; \
	} \
} while (false)

#define FAILED_ASSERT_RETURN_HR(expression) \
\
do \
{ \
	if (VCFailed(hr = (expression))) \
	{ \
		VSFAIL(#expression); \
		return hr; \
	} \
} while (false)

#define FAILED_SEI_ASSERT_RETURN_HR(argv, expression) \
\
do \
{ \
	if (VCFailed(hr = (expression))) \
	{ \
		VCFailedSEI(hr, CVCUSetErrorInfo##argv); \
		VSFAIL(#expression); \
		return hr; \
	} \
} while (false)

#define FAILED_RETURN(return_expression, expression) \
\
do \
{ \
	if (VCFailed(expression)) \
	{ \
		if (VCFailedBreakpoint()) \
		{ \
			VSFAIL(#expression); \
		} \
		\
		return return_expression; \
	} \
} while (false)

#define FAILED_SEI_RETURN(argv, return_expression, expression) \
\
do \
{ \
	if (VCFailed(expression)) \
	{ \
		VCFailedSEI(E_FAIL, CVCUSetErrorInfo##argv); \
		\
		if (VCFailedBreakpoint()) \
		{ \
			VSFAIL(#expression); \
		} \
		\
		return return_expression; \
	} \
} while (false)

#define FAILED_ASSERT_RETURN(return_expression, expression) FAILED_ASSERTMESSAGE_RETURN(return_expression, expression, #expression)

#define FAILED_ASSERTMESSAGE_RETURN(return_expression, expression, message) \
\
do \
{ \
	if (VCFailed(expression)) \
	{ \
		VSFAIL(message); \
		return return_expression; \
	} \
} while (false)

#define FAILED_ASSERT_EXIT(expression) FAILED_ASSERTMESSAGE_EXIT(expression, #expression)

#define FAILED_ASSERTMESSAGE_EXIT(expression, message) \
\
do \
{ \
	if (VCFailed(expression)) \
	{ \
		VSFAIL(message); \
		return; \
	} \
} while (false)

#define FAILED_SEI_ASSERT_RETURN(argv, return_expression, expression) \
\
do \
{ \
	if (VCFailed(expression)) \
	{ \
		VCFailedSEI(E_FAIL, CVCUSetErrorInfo##argv); \
		VSFAIL(#expression); \
		return return_expression; \
	} \
} while (false)

#define FAILED_BREAK_HR(expression) \
\
if (VCFailed(hr = (expression))) \
{ \
	if (VCFailedBreakpoint()) \
	{ \
		VSFAIL(#expression); \
	} \
	\
	break; \
} \
else \
{ \
}


#define FAILED_SEI_BREAK(argv, break_expression, expression) \
\
if (VCFailed(expression)) \
{ \
	VCFailedSEI(E_FAIL, CVCUSetErrorInfo##argv); \
	\
	if (VCFailedBreakpoint()) \
	{ \
		VSFAIL(#expression); \
	} \
	\
	break_expression; \
	break; \
} \
else \
{ \
}

#define FAILED_SEI_BREAK_HR(argv, expression) \
\
if (VCFailed(hr = (expression))) \
{ \
	VCFailedSEI(hr, CVCUSetErrorInfo##argv); \
	\
	if (VCFailedBreakpoint()) \
	{ \
		VSFAIL(#expression); \
	} \
	\
	break; \
} \
else \
{ \
}

#define FAILED_ASSERT_BREAK_HR(expression) \
\
if (VCFailed(hr = (expression))) \
{ \
	VSFAIL(#expression); \
	break; \
} \
else \
{ \
}

#define FAILED_BREAK(break_expression, expression) \
\
if (VCFailed(expression)) \
{ \
	if (VCFailedBreakpoint()) \
	{ \
		VSFAIL(#expression); \
	} \
	\
	break_expression; \
	break; \
} \
else \
{ \
}

#define FAILED_ASSERT_BREAK(break_expression, expression) \
\
if (VCFailed(expression)) \
{ \
	VSFAIL(#expression); \
	break_expression; \
	break; \
} \
else \
{ \
}

#define FAILED_THROW_HR(expression) \
\
do \
{ \
	if (VCFailed(hr = (expression))) \
	{ \
		if (VCFailedBreakpoint()) \
		{ \
			VSFAIL(#expression); \
		} \
		\
		throw hr; \
	} \
} while (false)

#define FAILED_ASSERT_THROW_HR(expression) \
\
do \
{ \
	if (VCFailed(hr = (expression))) \
	{ \
		VSFAIL(#expression); \
		throw hr; \
	} \
} while (false)

#define FAILED_THROW(throw_expression, expression) \
\
do \
{ \
	if (VCFailed(expression)) \
	{ \
		if (VCFailedBreakpoint()) \
		{ \
			VSFAIL(#expression); \
		} \
		\
		throw throw_expression; \
	} \
} while (false)

#define FAILED_ASSERT_THROW(throw_expression, expression) \
\
do \
{ \
	if (VCFailed(expression)) \
	{ \
		VSFAIL(#expression); \
		throw throw_expression; \
	} \
} while (false)


// VALIDATE_* (E_INVALIDARG)
//

template<typename T>
bool ValidateReadPointer(T * pT, const char * pszName)
{
	if (IsBadReadPtr(pT, sizeof(T)))
	{
		#if defined(_DEBUG)
		CString strMessage;
		strMessage.Format(_T("'%s' is not a valid read pointer"), pszName);
		VSFAIL(strMessage);
		#endif
		return false; 
	}

	return true;
}

template<typename T>
bool ValidateWritePointer(T * pT, const char * pszName)
{
	if (IsBadWritePtr(pT, sizeof(T)))
	{
		#if defined(_DEBUG)
		CString strMessage;
		strMessage.Format(_T("'%s' is not a valid write pointer"), pszName);
		VSFAIL(strMessage);
		#endif
		return false; 
	}

	return true;
}

// REVIEW : Should ValidateAndClear return HRESULT?
template<typename T>
bool ValidateAndClear(T * pT, const char * pszName)
{
	FAILED_RETURN(false, ValidateWritePointer(pT, pszName));
	*pT = T();
	return true;
}

inline bool ValidateAndClear(VARIANT_BOOL * pT, const char * pszName)
{
	FAILED_RETURN(false, ValidateWritePointer(pT, pszName));
	*pT = VARIANT_FALSE;
	return true;
}

inline bool ValidateAndClear(VARIANT * pT, const char * pszName)
{
	FAILED_RETURN(false, ValidateWritePointer(pT, pszName));
	VariantInit(pT);
	return true;
}

inline bool ValidateAndClear(BSTR * pT, const char * pszName)
{
	FAILED_RETURN(false, ValidateWritePointer(pT, pszName));
	*pT = NULL;
	return true;
}

inline bool ValidateRange(INT nValue, INT nMinimum, INT nMaximum, const char * pszName)
{
	#if defined(_DEBUG)

	bool bInRange(nValue <= nMaximum && nValue >= nMinimum);

	if (!bInRange)
	{
		CString strMessage;
		strMessage.Format(_T("'%s' is out of range, must be between %d and %d."), pszName, nMinimum, nMaximum);
		VSFAIL(strMessage);
	}

	#endif

	return nValue <= nMaximum && nValue >= nMinimum;
}


#define VALIDATE_ARGUMENT(expression) FAILED_ASSERT_RETURN(E_INVALIDARG, (expression))
#define VALIDATE_ARGUMENTMESSAGE(expression, message) FAILED_ASSERTMESSAGE_RETURN(E_INVALIDARG, (expression), (message))
#define VALIDATE_MEMBER(expression) FAILED_ASSERT_RETURN(E_FAIL, (expression))
#define VALIDATE_MEMBERMESSAGE(expression, message) FAILED_ASSERTMESSAGE_RETURN(E_FAIL, (expression), (message))
// UNDONE : Rename VALIDATE_AND_CLEAR to VALIDATE_AND_CLEAR_POINTER
#define VALIDATE_AND_CLEAR(p) VALIDATE_AND_CLEAR_POINTER(p)
#define VALIDATE_AND_CLEAR_POINTER(p) FAILED_ASSERT_RETURN(E_INVALIDARG, ValidateAndClear(p, #p))
#define VALIDATE_READ_POINTER(return_expression, expression) FAILED_ASSERT_RETURN(return_expression, ValidateReadPointer(expression, #expression))
#define VALIDATE_READ_POINTER_HR(expression) VALIDATE_READ_POINTER(E_POINTER, expression)
#define VALIDATE_WRITE_POINTER(return_expression, expression) FAILED_ASSERT_RETURN(return_expression, ValidateWritePointer(expression, #expression))
#define VALIDATE_WRITE_POINTER_HR(expression) VALIDATE_WRITE_POINTER(E_POINTER, expression)
#define VALIDATE_RANGE_HR(expression, minimum, maximum) VALIDATE_ARGUMENT(ValidateRange(expression, minimum, maximum, #expression))

// CHECK_*
//

#define CHECK_STATE_RETURN(state) FAILED_RETURN(E_UNEXPECTED, (state))
#define CHECK_STATE_ASSERT_RETURN(state) FAILED_ASSERT_RETURN(E_UNEXPECTED, (state))
