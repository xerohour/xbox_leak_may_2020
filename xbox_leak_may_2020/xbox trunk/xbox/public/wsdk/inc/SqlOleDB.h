//--------------------------------------------------------------------
// Microsoft OLE DB Provider for SQL Server
// (C) Copyright 1997 By Microsoft Corporation.
//
// @doc
//
// @module SQLOLEDB.H | Provider Specific definitions
//
//--------------------------------------------------------------------

#ifndef  _SQLOLEDB_H_
#define  _SQLOLEDB_H_

//----------------------------------------------------------------------------
// Provider-specific Class Ids
#ifdef DBINITCONSTANTS
// Provider CLSID 		{0C7FF16C-38E3-11d0-97AB-00C04FC2AD98}
extern const GUID CLSID_SQLOLEDB      		= {0xc7ff16cL,0x38e3,0x11d0,{0x97,0xab,0x0,0xc0,0x4f,0xc2,0xad,0x98}};
// Error Lookup CLSID 		{C0932C62-38E5-11d0-97AB-00C04FC2AD98}
extern const GUID CLSID_SQLOLEDB_ERROR 		= {0xc0932c62L,0x38e5,0x11d0,{0x97,0xab,0x0,0xc0,0x4f,0xc2,0xad,0x98}};
// Enumerator CLSID 		{DFA22B8E-E68D-11d0-97E4-00C04FC2AD98}
extern const GUID CLSID_SQLOLEDB_ENUMERATOR 	= {0xdfa22b8eL,0xe68d,0x11d0,{0x97,0xe4,0x0,0xc0,0x4f,0xc2,0xad,0x98}};
#else // !DBINITCONSTANTS
extern const GUID CLSID_SQLOLEDB;
extern const GUID CLSID_SQLOLEDB_ERROR;
extern const GUID CLSID_SQLOLEDB_ENUMERATOR;
#endif // DBINITCONSTANTS

//----------------------------------------------------------------------------
// Provider-specific Interface Ids
#ifdef DBINITCONSTANTS
extern const GUID IID_ISQLServerErrorInfo	= {0x5cf4ca12,0xef21,0x11d0,{0x97,0xe7,0x0,0xc0,0x4f,0xc2,0xad,0x98}};
extern const GUID IID_IRowsetFastLoad 		= {0x5cf4ca13,0xef21,0x11d0,{0x97,0xe7,0x0,0xc0,0x4f,0xc2,0xad,0x98}};
#else // !DBINITCONSTANTS
extern const GUID IID_ISQLServerErrorInfo;
extern const GUID IID_IRowsetFastLoad;
#endif // DBINITCONSTANTS

//----------------------------------------------------------------------------
// Provider-specific schema rowsets
#ifdef DBINITCONSTANTS
extern const GUID DBSCHEMA_LINKEDSERVERS	= {0x9093caf4,0x2eac,0x11d1,{0x98,0x9,0x0,0xc0,0x4f,0xc2,0xad,0x98}};
#else // !DBINITCONSTANTS
extern const GUID DBSCHEMA_LINKEDSERVERS;
#endif // DBINITCONSTANTS

#define CRESTRICTIONS_DBSCHEMA_LINKEDSERVERS	1


//----------------------------------------------------------------------------
// Provider-specific property sets
#ifdef DBINITCONSTANTS
extern const GUID DBPROPSET_SQLSERVERDBINIT 	= {0x5cf4ca10,0xef21,0x11d0,{0x97,0xe7,0x0,0xc0,0x4f,0xc2,0xad,0x98}};
extern const GUID DBPROPSET_SQLSERVERROWSET 	= {0x5cf4ca11,0xef21,0x11d0,{0x97,0xe7,0x0,0xc0,0x4f,0xc2,0xad,0x98}};
extern const GUID DBPROPSET_SQLSERVERDATASOURCE = {0x28efaee4,0x2d2c,0x11d1,{0x98,0x7,0x0,0xc0,0x4f,0xc2,0xad,0x98}};
extern const GUID DBPROPSET_SQLSERVERSESSION	= {0x28efaee5,0x2d2c,0x11d1,{0x98,0x7,0x0,0xc0,0x4f,0xc2,0xad,0x98}};

#else // !DBINITCONSTANTS
extern const GUID DBPROPSET_SQLSERVERDBINIT;
extern const GUID DBPROPSET_SQLSERVERROWSET;
extern const GUID DBPROPSET_SQLSERVERDATASOURCE;
extern const GUID DBPROPSET_SQLSERVERSESSION;
#endif // DBINITCONSTANTS

//----------------------------------------------------------------------------
// PropIds for DBPROPSET_SQLSERVERDBINIT 
#define SSPROP_INIT_CURRENTLANGUAGE		4
#define SSPROP_INIT_NETWORKADDRESS		5
#define SSPROP_INIT_NETWORKLIBRARY		6
#define SSPROP_INIT_USEPROCFORPREP		7
#define SSPROP_INIT_AUTOTRANSLATE		8
#define SSPROP_INIT_PACKETSIZE			9
#define SSPROP_INIT_APPNAME			10
#define SSPROP_INIT_WSID			11
#define SSPROP_INIT_FILENAME			12

// Values for SSPROP_USEPROCFORPREP
#define SSPROPVAL_USEPROCFORPREP_OFF		0
#define SSPROPVAL_USEPROCFORPREP_ON		1
#define SSPROPVAL_USEPROCFORPREP_ON_DROP	2

//----------------------------------------------------------------------------
// PropIds for DBPROPSET_SQLSERVERDATASOURCE
#define SSPROP_ENABLEFASTLOAD			2

//----------------------------------------------------------------------------
// PropIds for DBPROPSET_SQLSERVERSESSION
#define SSPROP_QUOTEDCATALOGNAMES		2

//----------------------------------------------------------------------------
// PropIds for DBPROPSET_SQLSERVERROWSET 
#define SSPROP_MAXBLOBLENGTH			8
#define SSPROP_FASTLOADOPTIONS			9
#define SSPROP_FASTLOADKEEPNULLS		10
#define SSPROP_FASTLOADKEEPIDENTITY		11


/* this ALWAYS GENERATED file contains the definitions for the interfaces */


/* File created by MIDL compiler version 3.01.75 */
/* at Sun Sep 14 11:43:38 1997
 */
/* Compiler settings for ..\inc\sqloledb.idl:
    Os (OptLev=s), W1, Zp8, env=Win32, ms_ext, c_ext
    error checks: none
*/
//@@MIDL_FILE_HEADING(  )
#include "rpc.h"
#include "rpcndr.h"
#ifndef COM_NO_WINDOWS_H
#include "windows.h"
#include "ole2.h"
#endif /*COM_NO_WINDOWS_H*/

#ifndef __sqloledb_h__
#define __sqloledb_h__

#ifdef __cplusplus
extern "C"{
#endif 

/* Forward Declarations */ 

#ifndef __ISQLServerErrorInfo_FWD_DEFINED__
#define __ISQLServerErrorInfo_FWD_DEFINED__
typedef interface ISQLServerErrorInfo ISQLServerErrorInfo;
#endif 	/* __ISQLServerErrorInfo_FWD_DEFINED__ */


#ifndef __IRowsetFastLoad_FWD_DEFINED__
#define __IRowsetFastLoad_FWD_DEFINED__
typedef interface IRowsetFastLoad IRowsetFastLoad;
#endif 	/* __IRowsetFastLoad_FWD_DEFINED__ */


/* header files for imported files */
#include "unknwn.h"

void __RPC_FAR * __RPC_USER MIDL_user_allocate(size_t);
void __RPC_USER MIDL_user_free( void __RPC_FAR * ); 

/****************************************
 * Generated header for interface: __MIDL_itf_sqloledb_0000
 * at Sun Sep 14 11:43:38 1997
 * using MIDL 3.01.75
 ****************************************/
/* [local] */ 


// the structure returned by  ISQLServerErrorInfo::GetSQLServerInfo
typedef struct  tagSSErrorInfo
    {
    LPOLESTR pwszMessage;
    LPOLESTR pwszServer;
    LPOLESTR pwszProcedure;
    LONG lNative;
    BYTE bState;
    BYTE bClass;
    WORD wLineNumber;
    }	SSERRORINFO;



extern RPC_IF_HANDLE __MIDL_itf_sqloledb_0000_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_sqloledb_0000_v0_0_s_ifspec;

#ifndef __ISQLServerErrorInfo_INTERFACE_DEFINED__
#define __ISQLServerErrorInfo_INTERFACE_DEFINED__

/****************************************
 * Generated header for interface: ISQLServerErrorInfo
 * at Sun Sep 14 11:43:38 1997
 * using MIDL 3.01.75
 ****************************************/
/* [unique][object][local][uuid] */ 



EXTERN_C const IID IID_ISQLServerErrorInfo;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    interface ISQLServerErrorInfo : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE GetErrorInfo( 
            /* [out] */ SSERRORINFO __RPC_FAR *__RPC_FAR *ppErrorInfo,
            /* [out] */ OLECHAR __RPC_FAR *__RPC_FAR *ppStringsBuffer) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct ISQLServerErrorInfoVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            ISQLServerErrorInfo __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            ISQLServerErrorInfo __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            ISQLServerErrorInfo __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetErrorInfo )( 
            ISQLServerErrorInfo __RPC_FAR * This,
            /* [out] */ SSERRORINFO __RPC_FAR *__RPC_FAR *ppErrorInfo,
            /* [out] */ OLECHAR __RPC_FAR *__RPC_FAR *ppStringsBuffer);
        
        END_INTERFACE
    } ISQLServerErrorInfoVtbl;

    interface ISQLServerErrorInfo
    {
        CONST_VTBL struct ISQLServerErrorInfoVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define ISQLServerErrorInfo_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define ISQLServerErrorInfo_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define ISQLServerErrorInfo_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define ISQLServerErrorInfo_GetErrorInfo(This,ppErrorInfo,ppStringsBuffer)	\
    (This)->lpVtbl -> GetErrorInfo(This,ppErrorInfo,ppStringsBuffer)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE ISQLServerErrorInfo_GetErrorInfo_Proxy( 
    ISQLServerErrorInfo __RPC_FAR * This,
    /* [out] */ SSERRORINFO __RPC_FAR *__RPC_FAR *ppErrorInfo,
    /* [out] */ OLECHAR __RPC_FAR *__RPC_FAR *ppStringsBuffer);


void __RPC_STUB ISQLServerErrorInfo_GetErrorInfo_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __ISQLServerErrorInfo_INTERFACE_DEFINED__ */


/****************************************
 * Generated header for interface: __MIDL_itf_sqloledb_0006
 * at Sun Sep 14 11:43:38 1997
 * using MIDL 3.01.75
 ****************************************/
/* [local] */ 


typedef ULONG_PTR HACCESSOR;



extern RPC_IF_HANDLE __MIDL_itf_sqloledb_0006_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_sqloledb_0006_v0_0_s_ifspec;

#ifndef __IRowsetFastLoad_INTERFACE_DEFINED__
#define __IRowsetFastLoad_INTERFACE_DEFINED__

/****************************************
 * Generated header for interface: IRowsetFastLoad
 * at Sun Sep 14 11:43:38 1997
 * using MIDL 3.01.75
 ****************************************/
/* [unique][object][local][uuid] */ 



EXTERN_C const IID IID_IRowsetFastLoad;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    interface IRowsetFastLoad : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE InsertRow( 
            /* [in] */ HACCESSOR hAccessor,
            /* [in] */ void __RPC_FAR *pData) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Commit( 
            /* [in] */ BOOL fDone) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IRowsetFastLoadVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IRowsetFastLoad __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IRowsetFastLoad __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IRowsetFastLoad __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *InsertRow )( 
            IRowsetFastLoad __RPC_FAR * This,
            /* [in] */ HACCESSOR hAccessor,
            /* [in] */ void __RPC_FAR *pData);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Commit )( 
            IRowsetFastLoad __RPC_FAR * This,
            /* [in] */ BOOL fDone);
        
        END_INTERFACE
    } IRowsetFastLoadVtbl;

    interface IRowsetFastLoad
    {
        CONST_VTBL struct IRowsetFastLoadVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IRowsetFastLoad_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IRowsetFastLoad_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IRowsetFastLoad_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IRowsetFastLoad_InsertRow(This,hAccessor,pData)	\
    (This)->lpVtbl -> InsertRow(This,hAccessor,pData)

#define IRowsetFastLoad_Commit(This,fDone)	\
    (This)->lpVtbl -> Commit(This,fDone)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IRowsetFastLoad_InsertRow_Proxy( 
    IRowsetFastLoad __RPC_FAR * This,
    /* [in] */ HACCESSOR hAccessor,
    /* [in] */ void __RPC_FAR *pData);


void __RPC_STUB IRowsetFastLoad_InsertRow_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IRowsetFastLoad_Commit_Proxy( 
    IRowsetFastLoad __RPC_FAR * This,
    /* [in] */ BOOL fDone);


void __RPC_STUB IRowsetFastLoad_Commit_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IRowsetFastLoad_INTERFACE_DEFINED__ */


/* Additional Prototypes for ALL interfaces */

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif

#endif //_SQLOLEDB_H_
