/*-----------------------------------------------------------------------------
Microsoft Sterling

Microsoft Confidential
Copyright 1994-1996 Microsoft Corporation.  All Rights Reserved.

File:		msodc.h
Contents:	This file describes the data context interfaces
-----------------------------------------------------------------------------*/

#ifndef _MSODC_H_
#define _MSODC_H_

#include <sql.h>
#include "inamespc.h"

// Forward Declarations -------------------------------------------------------
interface INamespace;
interface IDSRefConsumer;
interface IPkgProject;

///////////////////////////////////////////////////////////////////////////////
//
//    INamespaceSource
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// NSTYPE enumeration
typedef enum tagNSTYPE {
	NSTYPE_PROJECT		= 0x00000001,
	NSTYPE_DATASTORE	= 0x00000002
} NSTYPE;

#undef	INTERFACE
#define	INTERFACE	INamespaceSource
DECLARE_INTERFACE_(INamespaceSource, IUnknown)
{
	// IUnknown methods
	STDMETHOD(QueryInterface)(THIS_ REFIID riid, VOID** ppvObj) PURE;
	STDMETHOD_(ULONG,AddRef)(THIS) PURE;
	STDMETHOD_(ULONG,Release)(THIS) PURE;

	// INamespaceSource methods
	STDMETHOD(GetNamespace)(THIS_ DWORD nstype, INamespace** ppNamespace) PURE;
};


///////////////////////////////////////////////////////////////////////////////
//
//    IPersistDataStore
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// DSPERSIST enumeration
typedef enum tagDSPERSIST {
	DSPERSIST_NONE          = 0x00000000,
	DSPERSIST_DATASTORE		= 0x00000001,
	DSPERSIST_STORAGE		= 0x00000002,
	DSPERSIST_STREAM		= 0x00000004,

	// new ones added for Schema
	DSPERSIST_DEFERREDDDL	= 0x00000008,
	DSPERSIST_CHANGESCRIPT	= 0x00000010,

	// added to identify Query Builder Mode
	DSPERSIST_QUERYBUILDER	= 0x00000020,
	// open Query data pane only & run query
	DSPERSIST_RUNQUERY		= 0x00000040,

	// new ones added for Schema (R2)
	DSPERSIST_SAVEDIAGRAM		= 0x00000080,
	DSPERSIST_ALLOWOVERWRITE    = 0x00000100,
	DSPERSIST_NOCANCEL          = 0x00000200
} DSPERSIST;


// flags for PromptIfDirty() method to return to caller what User action to do.
typedef enum tagDSPERSISTUSERACTION {
	DSPERSIST_SAVE,
	DSPERSIST_NOSAVE,
	DSPERSIST_CANCEL
} DSPERSISTUSERACTION;



#undef	INTERFACE
#define	INTERFACE	IPersistDataStore
DECLARE_INTERFACE_(IPersistDataStore, IPersist)
{
	// IUnknown methods
	STDMETHOD(QueryInterface)(THIS_ REFIID riid, VOID** ppvObj) PURE;
	STDMETHOD_(ULONG,AddRef)(THIS) PURE;
	STDMETHOD_(ULONG,Release)(THIS) PURE;

	// IPersist methods
	STDMETHOD(GetClassID)(THIS_ CLSID* pClassID) PURE;

	// IPersistDataStore methods
	STDMETHOD(IsDirty)(THIS_ DWORD grfDSPersist) PURE;
	STDMETHOD(InitNew)(THIS_ IDSRefConsumer* pDSRef,
					IUnknown* pUnkConnectObj, IStream* pstm, IStorage* pstg,
					DWORD grfDSPersist) PURE;
	STDMETHOD(Load)(THIS_ IDSRefConsumer* pDSRef,
					IUnknown* pUnkConnectObj, IStream* pstm, IStorage* pstg,
					DWORD grfDSPersist) PURE;
	STDMETHOD(Save)(THIS_ IDSRefConsumer* pDSRef,
					IUnknown* pUnkConnectObj, IStream* pstm, IStorage* pstg,
					DWORD grfDSPersist,
					BOOL fSameAsLoad) PURE;
	STDMETHOD(SaveCompleted)(THIS_ IDSRefConsumer* pDSRef,
					IUnknown* pUnkConnectObj, IStream* pstm, IStorage* pstg,
					DWORD grfDSPersist) PURE;
	STDMETHOD(HandsOffDataStore)(THIS) PURE;
	STDMETHOD(PromptIfDirty)(DWORD * pUserAction, DWORD * pgrfDSPersist) PURE;
	STDMETHOD(GetName)(BSTR * pbstrName) PURE;
};



///////////////////////////////////////////////////////////////////////////////
//
//    IDBConnectObject
//
///////////////////////////////////////////////////////////////////////////////

// {B7A1D4A0-7210-11cf-BE16-00AA0062C2EF}
// DEFINE_GUID(IID_IDBConnectObject, 0xb7a1d4a0, 0x7210, 0x11cf, 0xbe, 0x16, 0x0, 0xaa, 0x0, 0x62, 0xc2, 0xef);
//
enum {DBCONN_SHOW_CONNECTION, DBCONN_SHOW_PROPERTY};

#undef	INTERFACE
#define	INTERFACE	IDBConnectObject
DECLARE_INTERFACE_(IDBConnectObject, IUnknown)
{
	// IUnknown methods
	STDMETHOD(QueryInterface)(THIS_ REFIID riid, VOID** ppvObj) PURE;
	STDMETHOD_(ULONG,AddRef)(THIS) PURE;
	STDMETHOD_(ULONG,Release)(THIS) PURE;

	// IDBConnectObject methods
	STDMETHOD(LockHandle)(THIS_ VOID** ppvConnectHandle) PURE;
	STDMETHOD_(VOID,UnlockHandle)(THIS) PURE;
	STDMETHOD(GetConnectString)(THIS_ LPOLESTR* ppszConnectString) PURE;
	STDMETHOD(GetNamespace)(THIS_ INamespace** ppNamespace) PURE;
	STDMETHOD(GetDisplayName)(THIS_ LPOLESTR * ppszDisplayName) PURE; 
	STDMETHOD(SetDisplayName)(THIS_ LPCOLESTR pszNewDisplayName) PURE; 
	STDMETHOD(GetNSIDPath)(THIS_ REFGUID guidObjType, LPCOLESTR pszObjName, LPCOLESTR pszOwnerName, NSIDPath ** ppNSIDPath) PURE; 
	STDMETHOD(Show)(THIS_ BOOL fUserControl, DWORD dwShow) PURE; 
};



///////////////////////////////////////////////////////////////////////////////
//
//    IODBCConnectObject
//
///////////////////////////////////////////////////////////////////////////////


// {B7A1D4A1-7210-11cf-BE16-00AA0062C2EF}
// DEFINE_GUID(IID_IODBCConnectObject, 0xb7a1d4a1, 0x7210, 0x11cf, 0xbe, 0x16, 0x0, 0xaa, 0x0, 0x62, 0xc2, 0xef);


#undef	INTERFACE
#define	INTERFACE	IODBCConnectObject
DECLARE_INTERFACE_(IODBCConnectObject, IDBConnectObject)
{
	STDMETHOD_(VOID,ShowError)(THIS_ HSTMT hstmt) PURE;
	STDMETHOD_(BOOL,FShowSystemObjects)() PURE;
};


///////////////////////////////////////////////////////////////////////////////
//
//    IJetConnectObject
//
///////////////////////////////////////////////////////////////////////////////

// {B7A1D4A2-7210-11cf-BE16-00AA0062C2EF}
// DEFINE_GUID(IID_IJetConnectObject, 0xb7a1d4a2, 0x7210, 0x11cf, 0xbe, 0x16, 0x0, 0xaa, 0x0, 0x62, 0xc2, 0xef);

#undef	INTERFACE
#define	INTERFACE	IJetConnectObject
DECLARE_INTERFACE_(IJetConnectObject, IDBConnectObject)
{
};


///////////////////////////////////////////////////////////////////////////////
//
//    IDBConnectionManager
//
///////////////////////////////////////////////////////////////////////////////

typedef enum {
CONNSTR_ODBCSTR = 0, 
CONNSTR_ODBCFILEDSN = 1,
CONNSTR_JET = 2, 
} CONNSTR;

// {B7A1D4A3-7210-11cf-BE16-00AA0062C2EF}
// DEFINE_GUID(IID_IDBConnectionManager, 0xb7a1d4a3, 0x7210, 0x11cf, 0xbe, 0x16, 0x0, 0xaa, 0x0, 0x62, 0xc2, 0xef);

#undef	INTERFACE
#define	INTERFACE	IDBConnectionManager
DECLARE_INTERFACE_(IDBConnectionManager, IUnknown)
{
	// IUnknown methods
	STDMETHOD(QueryInterface)(THIS_ REFIID riid, VOID** ppvObj) PURE;
	STDMETHOD_(ULONG,AddRef)(THIS) PURE;
	STDMETHOD_(ULONG,Release)(THIS) PURE;

	// IDBConnectionManager methods
	STDMETHOD(GetConnectObjFromString)(THIS_
								LPCOLESTR pszDisplayName,
								LPCOLESTR pszConnectStr,
								DWORD dwConnStrType,
								REFIID iidConnectObj,
								IUnknown* punkPkgProject,
								void ** ppConnectObj) PURE;
	STDMETHOD(GetConnectObjects)(THIS_
								ULONG cMaxConnections,
								IDBConnectObject **rgpConnObj, 
								ULONG *pcNumConnectObj) PURE;
	STDMETHOD(AddNewConnection)(THIS_
								DWORD dwConnStrType,
								LPCOLESTR pszDisplayName, 
								LPCOLESTR pszConnStrIn,
								IUnknown* punkPkgProject,
								LPOLESTR * ppszConnStrOut,
								IDBConnectObject ** ppConnectObj) PURE;
};

// {B7A1D4A4-7210-11cf-BE16-00AA0062C2EF}
// DEFINE_GUID(IID_IAsyncQueryBuilder, 0xb7a1d4a4, 0x7210, 0x11cf, 0xbe, 0x16, 0x0, 0xaa, 0x0, 0x62, 0xc2, 0xef);

#undef	INTERFACE
#define	INTERFACE	IAsyncQueryBuilder
DECLARE_INTERFACE_(IAsyncQueryBuilder, IUnknown)
{
	// IUnknown methods
	STDMETHOD(QueryInterface)(THIS_ REFIID riid, VOID** ppvObj) PURE;
	STDMETHOD_(ULONG,AddRef)(THIS) PURE;
	STDMETHOD_(ULONG,Release)(THIS) PURE;

    STDMETHOD(Execute)(THIS_ 
            /* [in] */ IDispatch * pdispApp,
            /* [in] */ HWND * hwndBuilderOwner,
            /* [in] */ LPCOLESTR pszBuilderOwner,
            /* [in] */ IDBConnectObject * pDBConnObj,
            /* [in] */ IDispatch * pdispCaller,
            /* [in] */ DISPID  dispidSQLText,
            /* [in] */ CALPOLESTR * pcaRunPropNames,
            /* [in] */ CADWORD * pcaRunPropDISPIDs,
            /* [in] */ ULONG cDesignProp,
            /* [in] */ VARIANTARG * pvargDesignPropNames,
            /* [in] */ VARIANTARG * pvargDesignPropValues) PURE;
};

// {B7A1D4A6-7210-11cf-BE16-00AA0062C2EF}
// DEFINE_GUID(IID_IDataSrcClient, 0xb7a1d4a6, 0x7210, 0x11cf, 0xbe, 0x16, 0x0, 0xaa, 0x0, 0x62, 0xc2, 0xef);

#undef	INTERFACE
#define	INTERFACE	IDataSrcClient
DECLARE_INTERFACE_(IDataSrcClient, IUnknown)
{
	// IUnknown methods
	STDMETHOD(QueryInterface)(THIS_ REFIID riid, VOID** ppvObj) PURE;
	STDMETHOD_(ULONG,AddRef)(THIS) PURE;
	STDMETHOD_(ULONG,Release)(THIS) PURE;

    STDMETHOD(AddDataSource)(THIS) PURE;
};

// {BE377880-0B1D-11d0-8923-00AA00BF0506}
// DEFINE_GUID(IDataSrcDebug, 0xbe377880, 0xb1d, 0x11d0, 0x89, 0x23, 0x0, 0xaa, 0x0, 0xbf, 0x5, 0x6);

#undef	INTERFACE
#define INTERFACE   IDataSrcDebug
DECLARE_INTERFACE_(IDataSrcDebug, IUnknown)
{
	// IUnknown methods
	STDMETHOD(QueryInterface)(THIS_ REFIID riid, VOID** ppvObj) PURE;
	STDMETHOD_(ULONG,AddRef)(THIS) PURE;
	STDMETHOD_(ULONG,Release)(THIS) PURE;

    STDMETHOD(InitDebugging)(THIS_ ULONG hStmt, ULONG hEvent) PURE;
	STDMETHOD(ExitDebugging)(THIS) PURE;
    STDMETHOD(SetObjectName)(THIS_ LPOLESTR, LPOLESTR, LPOLESTR) PURE;
    STDMETHOD_(BOOL, CanDebugObject)(THIS_ BOOL fCheckLocal) PURE;
	STDMETHOD(SetDebugLines)(THIS_ LPVOID) PURE;
	STDMETHOD(GetDebugStatus)(THIS_ ULONG*) PURE;
	STDMETHOD(SetDebugStatus)(THIS_ ULONG) PURE;
};

typedef IDataSrcDebug *PDBDBG;

// {b0c47fd0-1220-11d0-9d16-00c04fd9dfd9}
// DEFINE_GUID(IDataClientDebugConsumer, 0xb0c47fd0, 0x1220, 0x11d0, 0x9d, 0x16, 0x00, 0xc0, 0x4f, 0xd90x, 0xdf, 0xd9);

#define ACTIVE_DOC_TYPE_SP			0
#define ACTIVE_DOC_TYPE_TRIGGER		1
#define ACTIVE_DOC_TYPE_TABLE		2
#define ACTIVE_DOC_TYPE_DATABASE	3
#define ACTIVE_DOC_TYPE_ERROR       0x100
#define ACTIVE_DOC_TYPE_NOACTIVE    0x101
#define ACTIVE_DOC_TYPE_OTHER       0x102

#undef	INTERFACE
#define	INTERFACE	IDataSrcDebugConsumer
DECLARE_INTERFACE_(IDataSrcDebugConsumer, IUnknown)
{
	// IUnknown methods
    STDMETHOD(QueryInterface)(THIS_ REFIID riid, VOID** ppvObj) PURE;
    STDMETHOD_(ULONG,AddRef)(THIS) PURE;
    STDMETHOD_(ULONG,Release)(THIS) PURE;

    STDMETHOD(StopDebugging)(void) PURE ;
    STDMETHOD(BuildDebugLineTable)(LPTSTR pchBuf, unsigned short** pprgElems, unsigned short* pcElems) PURE;
	STDMETHOD_(DWORD,GetActiveDocType)(void) PURE ;
};

typedef IDataSrcDebugConsumer *PDBDBGCONSUMER;


#endif //_MSODC_H_
