//-----------------------------------------------------------------------------
// Microsoft DaVinci
//
// Microsoft Confidential
// Copyright 1994 - 1995 Microsoft Corporation. All Rights Reserved.
//
// File: inamespc.h
// Area: TNamespace (Database)
// Contents:
// INamespace and related COM interface definitions
//
// Owner: zacha		(3/17/95)	-	Created 
//-----------------------------------------------------------------------------

#ifndef _INAMESPC_H_
#define _INAMESPC_H_

#include "idsref.h"

//-----------------------------------------------------------------------------
// Name: NSItemID
//
// Description:
// INamespace ItemID uniquely identifying an item within its container
//
// Hungarian:  nsiid
//-----------------------------------------------------------------------------
typedef struct tagNSItemID
{
	USHORT	cb;			// ItemID byte data byte count
	BYTE	rgbID[1];	// ItemID byte data
} NSItemID,	*PNSItemID;




//-----------------------------------------------------------------------------
// Name: NSIDPath
//
// Description:
// INamespace ItemID path uniquely identifying an item within its containers
//
// Hungarian:  nsidp
//-----------------------------------------------------------------------------
typedef struct tagNSIDPath
{
	NSItemID	nsiid;	// First NSItemID in contiguous list
} NSIDPath,	*PNSIDPath;




//-----------------------------------------------------------------------------
// Name: IEnumNSItemChild
//
// Description:
// IEnumNSItemChild COM interface definition
//-----------------------------------------------------------------------------
DECLARE_INTERFACE_(IEnumNSItemChild, IUnknown)
{
	// IUnknown methods
	STDMETHOD(QueryInterface) (REFIID, LPVOID *) PURE;
	STDMETHOD_(ULONG, AddRef) (VOID) PURE;
	STDMETHOD_(ULONG, Release) (VOID) PURE;

	// IEnumNSItemChild methods
	STDMETHOD(Next) (
		ULONG cChild,
		PNSIDPath *rgpnsidpItemChild,
		ULONG *pcChild) PURE;
	STDMETHOD(Skip) (ULONG cChild) PURE;
	STDMETHOD(Reset) (VOID) PURE;
	STDMETHOD(Clone) (IEnumNSItemChild **ppIEnumNSItemChild) PURE;
};
typedef IEnumNSItemChild *PENUMNSITEMCHILD;




//-----------------------------------------------------------------------------
// Name: INSEventSink
//
// Description:
// INSEventSink COM interface definition
//-----------------------------------------------------------------------------
DECLARE_INTERFACE_(INSEventSink, IUnknown)
{
	// IUnknown methods
	STDMETHOD(QueryInterface) (REFIID, LPVOID *) PURE;
	STDMETHOD_(ULONG, AddRef) (VOID) PURE;
	STDMETHOD_(ULONG, Release) (VOID) PURE;

	// INSEventSink methods
	STDMETHOD_(VOID, OnItemAdded) (const PNSIDPath pnsidpItemAdded) PURE;
	STDMETHOD_(VOID, OnItemDeleted) (const PNSIDPath pnsidpItemDeleted) PURE;
	STDMETHOD_(VOID, OnItemRenamed) (const PNSIDPath pnsidpItemRenamed) PURE;
	STDMETHOD_(VOID, OnPropertyChanged) (const PNSIDPath pnsidpPropertyChanged) PURE;
};
typedef INSEventSink *PNSEVENTSINK;




//-----------------------------------------------------------------------------
// Name: INamespace
//
// Description:
// INamespace COM interface definition
//-----------------------------------------------------------------------------
DECLARE_INTERFACE_(INamespace, IUnknown)
{
	// IUnknown methods
	STDMETHOD(QueryInterface) (REFIID, LPVOID *) PURE;
	STDMETHOD_(ULONG, AddRef) (void) PURE;
	STDMETHOD_(ULONG, Release) (void) PURE;

	// INamespace methods
	STDMETHOD(GetItemChildEnum) (
		const PNSIDPath pnsidpItemCurr,
		PENUMNSITEMCHILD *ppIEnumNSItemChild) PURE;
	STDMETHOD(GetItemName) (
		const PNSIDPath pnsidpItemCurr,
		BSTR *pbstrName) PURE;
	STDMETHOD(GetItemProperties) (
		const PNSIDPath pnsidpItemCurr,
		LPPROPERTYSETSTORAGE *ppIPropertySetStorage) PURE;
	STDMETHOD(AppendDSRef) (
		const PNSIDPath pnsidpItemCurr,
		PDSREFPROVIDER pIDSRefProvider) PURE;
	STDMETHOD(Reduce) (
		const PNSIDPath pnsidpItemCurr,
		INamespace **ppINamespace,
		PNSIDPath *ppnsidpItemReduced) PURE;
	STDMETHOD(GetImageList) (HIMAGELIST *phil) PURE;
	STDMETHOD(GetItemImageListIndex) (
		const PNSIDPath pnsidpItemCurr,
		INT *piImageListIndex) PURE;
	STDMETHOD(GetItemTypeGUID) (
		const PNSIDPath pnsidpItemCurr,
		GUID * pGuid) PURE;
	STDMETHOD(GetItemChildEnumEx) (
		const PNSIDPath pnsidpItemCurr,
		REFGUID guidType,
		PENUMNSITEMCHILD *ppIEnumNSItemChild) PURE;
	STDMETHOD(AddChild) (
		REFGUID guidType,
		LPCOLESTR pszItemName,
		LPCOLESTR pszItemOwner) PURE;
	STDMETHOD(RemoveChild) (
		const PNSIDPath pnsidpItemCurr) PURE;
	STDMETHOD(AddChildEx) (
		REFGUID guidType,
		const PNSIDPath pnsidpParent,
		LPCOLESTR pszItemName,
		LPCOLESTR pszItemOwner) PURE;
	STDMETHOD(RenameItem) (
		const PNSIDPath pnsidpItem,
		LPCOLESTR pszNewName,
		LPCOLESTR pszNewOwner) PURE;
	STDMETHOD(UpdateItemProperties) (
		const PNSIDPath pnsidpItem) PURE;

};
typedef INamespace *PNAMESPACE;

#endif	// _INAMESPC_H_
