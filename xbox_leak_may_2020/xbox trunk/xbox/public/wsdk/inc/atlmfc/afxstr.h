// This is a part of the Microsoft Foundation Classes C++ library.
// Copyright (C) 1992-2001 Microsoft Corporation
// All rights reserved.
//
// This source code is only intended as a supplement to the
// Microsoft Foundation Classes Reference and related
// electronic documentation provided with the library.
// See these sources for detailed information regarding the
// Microsoft Foundation Classes product.

/////////////////////////////////////////////////////////////////////////////
// AFXSTR.H - Framework-independent, templateable string class

#ifndef __AFXSTR_H__
#define __AFXSTR_H__

#if _MSC_VER > 1000
#pragma once
#endif

#ifndef _AFX
#error afxstr.h can only be used in MFC projects.  Use atlstr.h
#endif

#include <mbstring.h>

HINSTANCE AFXAPI AfxGetResourceHandle();
HINSTANCE AFXAPI AfxFindStringResourceHandle(UINT nID);

UINT_PTR AFXAPI AfxReadStringLength(CArchive& ar, int& nCharSize);
void AFXAPI AfxWriteStringLength(CArchive& ar, UINT_PTR nLength, BOOL bUnicode);

#include <crtdbg.h>
#include <atlbase.h>
#include <cstringt.h>

ATL::IAtlStringMgr* AFXAPI AfxGetStringManager();

template< typename _CharType = char, class StringIterator = ATL::ChTraitsCRT< _CharType > >
class StrTraitMFC : 
	public StringIterator
{
public:
	static HINSTANCE FindStringResourceInstance( UINT nID ) throw()
	{
		return( AfxFindStringResourceHandle( nID ) );
	}

	static ATL::IAtlStringMgr* GetDefaultManager() throw()
	{
		return( AfxGetStringManager() );
	}
};

// MFC-enabled compilation. Use MFC memory management and exceptions;
// also, use MFC module state.
typedef ATL::CStringT< wchar_t, StrTraitMFC< wchar_t > > CStringW;
typedef ATL::CStringT< char, StrTraitMFC< char > > CStringA;
typedef ATL::CStringT< TCHAR, StrTraitMFC< TCHAR > > CString;

template< typename BaseType, class StringTraits >
CArchive& AFXAPI operator<<(CArchive& ar,
	const ATL::CStringT<BaseType, StringTraits>& str)
{
	AfxWriteStringLength(ar, str.GetLength(), sizeof(BaseType) == sizeof(wchar_t));
	ar.Write(str, str.GetLength()*sizeof(BaseType));

	return ar;
}

template< typename BaseType, class StringTraits >
CArchive& AFXAPI operator>>(CArchive& ar,
	ATL::CStringT<BaseType, StringTraits>& str)
{
	int nCharSize;  // 1 = char, 2 = wchar_t
	UINT nLength = UINT( AfxReadStringLength(ar, nCharSize) );
	if (nCharSize == sizeof(char))
	{
		CTempBuffer< char > pszBufferA(nLength+1);

		pszBufferA[nLength] = '\0';
		UINT nBytesRead = ar.Read(pszBufferA, nLength*sizeof(char));
		if (nBytesRead != (nLength*sizeof(char)))
			AfxThrowArchiveException(CArchiveException::endOfFile);
		str = pszBufferA;
	}
	else
	{
		ASSERT(nCharSize == sizeof(wchar_t));

		CTempBuffer< wchar_t > pszBufferW( nLength+1 );

		pszBufferW[nLength] = L'\0';
		UINT nBytesRead = ar.Read(pszBufferW, nLength*sizeof(wchar_t));
		if (nBytesRead != (nLength*sizeof(wchar_t)))
			AfxThrowArchiveException(CArchiveException::endOfFile);
		str = pszBufferW;
	}

	return ar;
}

//////////////////////////////////////////////////////////////////////////////
// Diagnostic support

#ifdef _DEBUG
template< typename BaseType, class StringTraits >
inline CDumpContext& AFXAPI operator<<(CDumpContext& dc, const ATL::CStringT<BaseType, 
	StringTraits>& str)
{
	dc << static_cast< const BaseType* >( str );
	return dc;
}
#endif //_DEBUG

#endif	// __AFXSTR_H__ (whole file)
