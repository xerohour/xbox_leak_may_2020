// Globals.h
// implementation of the object implementing the IVCGlobals interface
///////////////////////////////////////////////////////////////////////////

#pragma once

#ifndef _GLOBALS_H_
#define _GLOBALS_H_

#include "vccolls.h"

class ATL_NO_VTABLE CVCGlobals :
	public CComObjectRoot,
	public IVCGlobals
{
public:
	CVCGlobals();
	~CVCGlobals();
	static HRESULT CreateInstance( CVCGlobals** ppGlobals );

BEGIN_COM_MAP( CVCGlobals )
	COM_INTERFACE_ENTRY( IVCGlobals )
END_COM_MAP()

protected:
	void Initialize(IDispatch* pProjects);

public:
	// IVCGlobals
	STDMETHOD(GetValue)( BSTR bstrName, BSTR *pbstrValue );
	STDMETHOD(SetValue)( BSTR bstrName, BSTR bstrValue );
	STDMETHOD(GetPersistance)( BSTR bstrName, VARIANT_BOOL *pbPersists );
	STDMETHOD(SetPersistance)( BSTR bstrName, VARIANT_BOOL bPersists );
	STDMETHOD(SaveObject)( IStream *xml, IVCPropertyContainer *pPropCnt, long nIndent );
	STDMETHOD(GetCount)( ULONG *count );
	STDMETHOD(GetFirstItem)( BSTR *pbstrName, BSTR *pbstrValue );
	STDMETHOD(GetNextItem)( BSTR *pbstrName, BSTR *pbstrValue );

private:
	// map globals' names -> values
	CVCMapStringWToStringW m_GlobalsMap;
	// map of the names of the persisting globals
	// (globals that are not in the array do NOT persist)
	CVCMapStringWToPtr m_GlobalsPersistanceMap;
};

#endif // _GLOBALS_H_
