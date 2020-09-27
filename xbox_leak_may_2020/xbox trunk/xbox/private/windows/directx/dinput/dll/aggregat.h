/*++

Copyright (c) 2000  Microsoft Corporation

Module Name:

    di.cpp

Abstract:

    Declaration of Support Classes for Aggregation

Environment:

	XAPI

Revision History:

    05-10-00 : created

--*/

//
//	Class that need to be aggregable inherits CAggregable
//	in addition to the interfaces that they are implementing
//	(This assumes Multiple Inheritance of pure abstract base classes,
//	is the technique for exposing interfaces).
//

class CAggregable
{
	friend class CImplInnerUnk;
	virtual HRESULT ImplQI(REFIID riid, LPVOID * ppvObj) = 0;
	virtual ULONG ImplAddRef() = 0;
	virtual ULONG ImplRelease() = 0;
};


//
// This class provide the vtable for the "real" IUnknown
// when aggregation is in effect.  It delegates to
// the ImplQI, ImplAddRef, and ImplRelease methods
//	pf CAggregable.
//
class CImplInnerUnk : public IUnknown
{
	public:
		STDMETHOD(QueryInterface)(REFIID riid, LPVOID * ppvObj);
		STDMETHOD_(ULONG,AddRef)();
		STDMETHOD_(ULONG,Release)();
		void Init(CAggregable *pInnerObj)
		{
			m_pAggregable = pInnerObj;
		}
	private:
		CAggregable *m_pAggregable;
};
