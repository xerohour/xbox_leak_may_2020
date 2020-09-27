//+---------------------------------------------------------------------
//
//  Microsoft Windows
//  Copyright (C) Microsoft Corporation, 1993 - 1994.
//
//  File:       dhtmlapi.h
//
//  Contents:   Defines the interface from the editor to the dhtml package
//
//  Classes:    
//
//  Functions:  
//
//  History:    29-Jul-98   BruceMa    Created.
//
//----------------------------------------------------------------------

#ifndef __DHTMLAPI_H__
#define __DHTMLAPI_H__



/////////////////////////////////////////////////////////////////////////////
//	IDhtmlPkg interface

#undef  INTERFACE
#define INTERFACE IDhtmlPkg


DECLARE_INTERFACE_(IDhtmlPkg, IUnknown)
{
	// IUnknown methods
	STDMETHOD(QueryInterface)(THIS_ REFIID riid, LPVOID *ppvObj) PURE;
	STDMETHOD_(ULONG,AddRef)(THIS) PURE;
	STDMETHOD_(ULONG,Release)(THIS) PURE;

        // IDhtmlPkg methods
	STDMETHOD(RegisterExternalEditor)(THIS_ LPEXTERNALRESPKG pExtRes, 
                                          REFCLSID clsidEditor,
                                          LPCSTR szEditorName,
                                          LPCSTR szResType) PURE;
};


typedef IDhtmlPkg *LPDHTMLPKG;


#endif	// __DHTMLAPI_H__
