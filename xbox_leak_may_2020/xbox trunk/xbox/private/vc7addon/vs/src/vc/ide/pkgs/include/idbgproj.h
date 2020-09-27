//-----------------------------------------------------------------------------
// 
//
// Microsoft Confidential
// Copyright 1994 - 1995 Microsoft Corporation. All Rights Reserved.
//
// File: idbgproj.h
// Area: Non-Buildable Projects that can still Debug
// Contents:
//	Interface for Debugging without Building
//
// Owner: apennell		(10/2/96)	-	Created 
//-----------------------------------------------------------------------------

#ifndef _IDBGPROJ_H_
#define _IDBGPROJ_H_


//-----------------------------------------------------------------------------
// Name: IDBGProj
//
// Description:
// COM interface definition allowing non-builder projects to support debugging
//-----------------------------------------------------------------------------
DECLARE_INTERFACE_(IDBGProj, IUnknown)
{
	// IUnknown methods
	STDMETHOD(QueryInterface) (REFIID, LPVOID *) PURE;
	STDMETHOD_(ULONG, AddRef) (VOID) PURE;
	STDMETHOD_(ULONG, Release) (VOID) PURE;

	// IDBDisp methods
	STDMETHOD(SupportsDebugging)(UINT*) PURE;
	STDMETHOD(CommandExecute)(UINT) PURE;
	STDMETHOD(CommandEnabled)( UINT, BOOL* ) PURE;
};
typedef IDBGProj *PDBGPROJ;



#endif  // _IDBDGPROJ_H_

