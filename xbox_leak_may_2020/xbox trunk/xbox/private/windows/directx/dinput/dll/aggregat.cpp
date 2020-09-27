/*++

Copyright (c) 2000  Microsoft Corporation

Module Name:

    di.cpp

Abstract:

    Implementation of Support Class for Aggregation

Environment:

	XAPI

Revision History:

    05-10-00 : created

--*/

#include <objbase.h>
#include <aggregat.h>

STDMETHODIMP CImplInnerUnk::QueryInterface(REFIID riid, LPVOID * ppvObj)
{
	return m_pAggregable->ImplQI(riid, ppvObj);
}
STDMETHODIMP_(ULONG) CImplInnerUnk::AddRef()
{
	return m_pAggregable->ImplAddRef();
}
STDMETHODIMP_(ULONG) CImplInnerUnk::Release()
{
	return m_pAggregable->ImplRelease();
}
