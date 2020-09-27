#pragma once

// this handy, dandy template helps keep track of marshalling for you

template <class PrimaryIface>
class CBldMarshaller
{
public:
	CBldMarshaller() : m_pStream(NULL) {}
	~CBldMarshaller()
	{
		Clear();
	}
	HRESULT Init(PrimaryIface* pClass)
	{
		Clear();	// in case of reuse
		m_spClass = pClass;
		if (pClass == NULL || m_spClass == NULL)
			return E_UNEXPECTED;
		return AtlMarshalPtrInProc((IUnknown *)pClass, __uuidof(PrimaryIface), &m_pStream);
	}
	HRESULT Clear()
	{
		HRESULT hr = S_OK;
		if (m_pStream != NULL)
			hr = AtlFreeMarshalStream(m_pStream);
		m_pStream = NULL;
		m_spClass.Release();
		return hr;
	}
	HRESULT UnmarshalPtr(PrimaryIface** ppClass)
	{
		if (ppClass == NULL)
			return E_POINTER;
		*ppClass = NULL;
		return AtlUnmarshalPtr(m_pStream, __uuidof(PrimaryIface), (IUnknown **)ppClass);
	}
	BOOL IsInit()
	{
		return (m_pStream != NULL);
	}
	HRESULT GetPtr(PrimaryIface** ppClass)	// call this only inproc
	{
		return m_spClass.CopyTo(ppClass);
	}
		

protected:
	IStream* m_pStream;
	CComPtr<PrimaryIface> m_spClass;
};

