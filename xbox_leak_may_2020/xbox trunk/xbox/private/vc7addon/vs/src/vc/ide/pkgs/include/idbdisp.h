//-----------------------------------------------------------------------------
// 
//
// Microsoft Confidential
// Copyright 1994 - 1995 Microsoft Corporation. All Rights Reserved.
//
// File: idbdisp.h
// Area: DTG - ENT integration for debugging
// Contents:
//	Interface for displaying / editing / setting breakpoints
//
// Owner: cbrown		(8/14/96)	-	Created 
//-----------------------------------------------------------------------------

#ifndef _IDBDISP_H_
#define _IDBDISP_H_


//-----------------------------------------------------------------------------
// Name: IDBDisp
//
// Description:
// COM interface definition allowing ENT package to ask DTG to open stored
// procedures or triggers in edit windows during debug sessions
//-----------------------------------------------------------------------------
DECLARE_INTERFACE_(IDBTextDisp, IUnknown)
{
	// IUnknown methods
	STDMETHOD(QueryInterface) (REFIID, LPVOID *) PURE;
	STDMETHOD_(ULONG, AddRef) (VOID) PURE;
	STDMETHOD_(ULONG, Release) (VOID) PURE;

	// IDBDisp methods
	STDMETHOD(OpenFromName) (BSTR bstrName, BSTR bstrOwner, BSTR bstrDB, BSTR bstrServer, ISourceEdit** ppSEdit) PURE;
	STDMETHOD(OpenFromId) (UINT uProcId, ISourceEdit** ppSEdit) PURE;
	STDMETHOD(IdFromName) (BSTR bstrName, BSTR bstrOwner, BSTR bstrDB, BSTR bstrServer, UINT* pId) PURE;
	STDMETHOD(NameFromId)(BSTR* pbstrName, BSTR* pbstrOwner, BSTR bstrDB, BSTR bstrServer, UINT uId) PURE;
    STDMETHOD(ValidDoc)(ULONG uDocID) PURE;
};
typedef IDBTextDisp *PDBTEXTDISP;



#endif  // _IDBDISP_H_

