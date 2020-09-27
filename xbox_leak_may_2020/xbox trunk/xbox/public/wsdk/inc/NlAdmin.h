/* this ALWAYS GENERATED file contains the definitions for the interfaces */


/* File created by MIDL compiler version 3.01.75 */
/* at Wed Apr 22 20:03:10 1998
 */
/* Compiler settings for nladmin.idl:
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

#ifndef __nladmin_h__
#define __nladmin_h__

#ifdef __cplusplus
extern "C"{
#endif 

/* Forward Declarations */ 

#ifndef __ISearchAdmin_FWD_DEFINED__
#define __ISearchAdmin_FWD_DEFINED__
typedef interface ISearchAdmin ISearchAdmin;
#endif 	/* __ISearchAdmin_FWD_DEFINED__ */


#ifndef __IBuildServer_FWD_DEFINED__
#define __IBuildServer_FWD_DEFINED__
typedef interface IBuildServer IBuildServer;
#endif 	/* __IBuildServer_FWD_DEFINED__ */


#ifndef __IBuildCatalogs_FWD_DEFINED__
#define __IBuildCatalogs_FWD_DEFINED__
typedef interface IBuildCatalogs IBuildCatalogs;
#endif 	/* __IBuildCatalogs_FWD_DEFINED__ */


#ifndef __IBuildCatalog_FWD_DEFINED__
#define __IBuildCatalog_FWD_DEFINED__
typedef interface IBuildCatalog IBuildCatalog;
#endif 	/* __IBuildCatalog_FWD_DEFINED__ */


#ifndef __ISearchServer_FWD_DEFINED__
#define __ISearchServer_FWD_DEFINED__
typedef interface ISearchServer ISearchServer;
#endif 	/* __ISearchServer_FWD_DEFINED__ */


#ifndef __ISearchCatalogs_FWD_DEFINED__
#define __ISearchCatalogs_FWD_DEFINED__
typedef interface ISearchCatalogs ISearchCatalogs;
#endif 	/* __ISearchCatalogs_FWD_DEFINED__ */


#ifndef __ISearchCatalog_FWD_DEFINED__
#define __ISearchCatalog_FWD_DEFINED__
typedef interface ISearchCatalog ISearchCatalog;
#endif 	/* __ISearchCatalog_FWD_DEFINED__ */


#ifndef __IBuildServer_FWD_DEFINED__
#define __IBuildServer_FWD_DEFINED__
typedef interface IBuildServer IBuildServer;
#endif 	/* __IBuildServer_FWD_DEFINED__ */


#ifndef __IBuildCatalogs_FWD_DEFINED__
#define __IBuildCatalogs_FWD_DEFINED__
typedef interface IBuildCatalogs IBuildCatalogs;
#endif 	/* __IBuildCatalogs_FWD_DEFINED__ */


#ifndef __IBuildCatalog_FWD_DEFINED__
#define __IBuildCatalog_FWD_DEFINED__
typedef interface IBuildCatalog IBuildCatalog;
#endif 	/* __IBuildCatalog_FWD_DEFINED__ */


#ifndef __ISearchServer_FWD_DEFINED__
#define __ISearchServer_FWD_DEFINED__
typedef interface ISearchServer ISearchServer;
#endif 	/* __ISearchServer_FWD_DEFINED__ */


#ifndef __ISearchCatalogs_FWD_DEFINED__
#define __ISearchCatalogs_FWD_DEFINED__
typedef interface ISearchCatalogs ISearchCatalogs;
#endif 	/* __ISearchCatalogs_FWD_DEFINED__ */


#ifndef __ISearchCatalog_FWD_DEFINED__
#define __ISearchCatalog_FWD_DEFINED__
typedef interface ISearchCatalog ISearchCatalog;
#endif 	/* __ISearchCatalog_FWD_DEFINED__ */


#ifndef __SearchAdmin_FWD_DEFINED__
#define __SearchAdmin_FWD_DEFINED__

#ifdef __cplusplus
typedef class SearchAdmin SearchAdmin;
#else
typedef struct SearchAdmin SearchAdmin;
#endif /* __cplusplus */

#endif 	/* __SearchAdmin_FWD_DEFINED__ */


/* header files for imported files */
#include "oaidl.h"
#include "ocidl.h"
#include "gthradminsdk.h"
#include "schemaedit.h"
#include "idxadminsdk.h"
#include "catprops.h"

void __RPC_FAR * __RPC_USER MIDL_user_allocate(size_t);
void __RPC_USER MIDL_user_free( void __RPC_FAR * ); 

#ifndef __ISearchAdmin_INTERFACE_DEFINED__
#define __ISearchAdmin_INTERFACE_DEFINED__

/****************************************
 * Generated header for interface: ISearchAdmin
 * at Wed Apr 22 20:03:10 1998
 * using MIDL 3.01.75
 ****************************************/
/* [unique][helpstring][dual][uuid][object] */ 



EXTERN_C const IID IID_ISearchAdmin;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    interface DECLSPEC_UUID("0b63e355-9ccc-11d0-bcdb-00805fccce04")
    ISearchAdmin : public IDispatch
    {
    public:
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_HostName( 
            /* [retval][out] */ BSTR __RPC_FAR *pbstrName) = 0;
        
        virtual /* [helpstring][propput] */ HRESULT STDMETHODCALLTYPE put_HostName( 
            /* [in] */ BSTR bstrName) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_BuildServer( 
            /* [retval][out] */ VARIANT __RPC_FAR *pvarBuildServer) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_SearchServer( 
            /* [retval][out] */ VARIANT __RPC_FAR *pvarSearchServer) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct ISearchAdminVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            ISearchAdmin __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            ISearchAdmin __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            ISearchAdmin __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            ISearchAdmin __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            ISearchAdmin __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            ISearchAdmin __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            ISearchAdmin __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_HostName )( 
            ISearchAdmin __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pbstrName);
        
        /* [helpstring][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_HostName )( 
            ISearchAdmin __RPC_FAR * This,
            /* [in] */ BSTR bstrName);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_BuildServer )( 
            ISearchAdmin __RPC_FAR * This,
            /* [retval][out] */ VARIANT __RPC_FAR *pvarBuildServer);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_SearchServer )( 
            ISearchAdmin __RPC_FAR * This,
            /* [retval][out] */ VARIANT __RPC_FAR *pvarSearchServer);
        
        END_INTERFACE
    } ISearchAdminVtbl;

    interface ISearchAdmin
    {
        CONST_VTBL struct ISearchAdminVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define ISearchAdmin_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define ISearchAdmin_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define ISearchAdmin_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define ISearchAdmin_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define ISearchAdmin_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define ISearchAdmin_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define ISearchAdmin_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define ISearchAdmin_get_HostName(This,pbstrName)	\
    (This)->lpVtbl -> get_HostName(This,pbstrName)

#define ISearchAdmin_put_HostName(This,bstrName)	\
    (This)->lpVtbl -> put_HostName(This,bstrName)

#define ISearchAdmin_get_BuildServer(This,pvarBuildServer)	\
    (This)->lpVtbl -> get_BuildServer(This,pvarBuildServer)

#define ISearchAdmin_get_SearchServer(This,pvarSearchServer)	\
    (This)->lpVtbl -> get_SearchServer(This,pvarSearchServer)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE ISearchAdmin_get_HostName_Proxy( 
    ISearchAdmin __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pbstrName);


void __RPC_STUB ISearchAdmin_get_HostName_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propput] */ HRESULT STDMETHODCALLTYPE ISearchAdmin_put_HostName_Proxy( 
    ISearchAdmin __RPC_FAR * This,
    /* [in] */ BSTR bstrName);


void __RPC_STUB ISearchAdmin_put_HostName_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE ISearchAdmin_get_BuildServer_Proxy( 
    ISearchAdmin __RPC_FAR * This,
    /* [retval][out] */ VARIANT __RPC_FAR *pvarBuildServer);


void __RPC_STUB ISearchAdmin_get_BuildServer_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE ISearchAdmin_get_SearchServer_Proxy( 
    ISearchAdmin __RPC_FAR * This,
    /* [retval][out] */ VARIANT __RPC_FAR *pvarSearchServer);


void __RPC_STUB ISearchAdmin_get_SearchServer_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __ISearchAdmin_INTERFACE_DEFINED__ */


#ifndef __IBuildServer_INTERFACE_DEFINED__
#define __IBuildServer_INTERFACE_DEFINED__

/****************************************
 * Generated header for interface: IBuildServer
 * at Wed Apr 22 20:03:10 1998
 * using MIDL 3.01.75
 ****************************************/
/* [unique][helpstring][dual][uuid][object] */ 



EXTERN_C const IID IID_IBuildServer;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    interface DECLSPEC_UUID("0b63e356-9ccc-11d0-bcdb-00805fccce04")
    IBuildServer : public IDispatch
    {
    public:
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_UserAgent( 
            /* [retval][out] */ BSTR __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][propput] */ HRESULT STDMETHODCALLTYPE put_UserAgent( 
            /* [in] */ BSTR newVal) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_EmailAddress( 
            /* [retval][out] */ BSTR __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][propput] */ HRESULT STDMETHODCALLTYPE put_EmailAddress( 
            /* [in] */ BSTR newVal) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_TempPath( 
            /* [retval][out] */ BSTR __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][propput] */ HRESULT STDMETHODCALLTYPE put_TempPath( 
            /* [in] */ BSTR newVal) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_PerformanceLevel( 
            /* [retval][out] */ LONG __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][propput] */ HRESULT STDMETHODCALLTYPE put_PerformanceLevel( 
            /* [in] */ LONG newVal) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_DefaultCatalogsLocation( 
            /* [retval][out] */ BSTR __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][propput] */ HRESULT STDMETHODCALLTYPE put_DefaultCatalogsLocation( 
            /* [in] */ BSTR newVal) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_ConnectTimeout( 
            /* [retval][out] */ LONG __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][propput] */ HRESULT STDMETHODCALLTYPE put_ConnectTimeout( 
            /* [in] */ LONG newVal) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_DataTimeout( 
            /* [retval][out] */ LONG __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][propput] */ HRESULT STDMETHODCALLTYPE put_DataTimeout( 
            /* [in] */ LONG newVal) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_DefaultAccountName( 
            /* [retval][out] */ BSTR __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_DefaultPluginAccountName( 
            /* [retval][out] */ BSTR __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_SearchAdmin( 
            /* [retval][out] */ VARIANT __RPC_FAR *pvarSearchAdmin) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_BuildCatalogs( 
            /* [retval][out] */ VARIANT __RPC_FAR *pvarBuildCatalogs) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_Sites( 
            /* [retval][out] */ VARIANT __RPC_FAR *pvarServers) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_ProxyUsageSetting( 
            /* [retval][out] */ LONG __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_ProxyAddress( 
            /* [retval][out] */ BSTR __RPC_FAR *pbstrAddress) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_ProxyPortNumber( 
            /* [retval][out] */ LONG __RPC_FAR *pnPort) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_ProxyBypassForLocalAddresses( 
            /* [retval][out] */ LONG __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_ProxyBypassAddressesList( 
            /* [retval][out] */ BSTR __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_DefaultLogsLocation( 
            /* [retval][out] */ BSTR __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][propput] */ HRESULT STDMETHODCALLTYPE put_DefaultLogsLocation( 
            /* [in] */ BSTR newVal) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE Clone( 
            /* [retval][out] */ VARIANT __RPC_FAR *pvarBuildServer) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE SetDefaultAccount( 
            /* [in] */ BSTR bstrAccountName,
            /* [in] */ BSTR bstrPassword) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE SetDefaultPluginAccount( 
            /* [in] */ BSTR bstrAccountName,
            /* [in] */ BSTR bstrPassword) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE ExportCatalog( 
            /* [in] */ BSTR bstrCatalogName,
            /* [in] */ BSTR bstrFilePath) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE ImportCatalog( 
            /* [in] */ BSTR bstrCatalogName,
            /* [in] */ BSTR bstrFilePath) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE SetProxySettings( 
            /* [in] */ LONG nUsageSetting,
            /* [in] */ BSTR bstrAddress,
            /* [in] */ LONG nPort,
            /* [in] */ LONG nBypassForLocalAddresses,
            /* [in] */ BSTR bstrBypassAddressesList) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE LoadConfiguration( 
            /* [in] */ BSTR bstrFilePath) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE SaveConfiguration( 
            /* [in] */ BSTR bstrFilePath) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IBuildServerVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IBuildServer __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IBuildServer __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IBuildServer __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            IBuildServer __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            IBuildServer __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            IBuildServer __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            IBuildServer __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_UserAgent )( 
            IBuildServer __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pVal);
        
        /* [helpstring][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_UserAgent )( 
            IBuildServer __RPC_FAR * This,
            /* [in] */ BSTR newVal);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_EmailAddress )( 
            IBuildServer __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pVal);
        
        /* [helpstring][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_EmailAddress )( 
            IBuildServer __RPC_FAR * This,
            /* [in] */ BSTR newVal);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_TempPath )( 
            IBuildServer __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pVal);
        
        /* [helpstring][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_TempPath )( 
            IBuildServer __RPC_FAR * This,
            /* [in] */ BSTR newVal);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_PerformanceLevel )( 
            IBuildServer __RPC_FAR * This,
            /* [retval][out] */ LONG __RPC_FAR *pVal);
        
        /* [helpstring][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_PerformanceLevel )( 
            IBuildServer __RPC_FAR * This,
            /* [in] */ LONG newVal);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_DefaultCatalogsLocation )( 
            IBuildServer __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pVal);
        
        /* [helpstring][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_DefaultCatalogsLocation )( 
            IBuildServer __RPC_FAR * This,
            /* [in] */ BSTR newVal);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_ConnectTimeout )( 
            IBuildServer __RPC_FAR * This,
            /* [retval][out] */ LONG __RPC_FAR *pVal);
        
        /* [helpstring][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_ConnectTimeout )( 
            IBuildServer __RPC_FAR * This,
            /* [in] */ LONG newVal);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_DataTimeout )( 
            IBuildServer __RPC_FAR * This,
            /* [retval][out] */ LONG __RPC_FAR *pVal);
        
        /* [helpstring][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_DataTimeout )( 
            IBuildServer __RPC_FAR * This,
            /* [in] */ LONG newVal);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_DefaultAccountName )( 
            IBuildServer __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pVal);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_DefaultPluginAccountName )( 
            IBuildServer __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pVal);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_SearchAdmin )( 
            IBuildServer __RPC_FAR * This,
            /* [retval][out] */ VARIANT __RPC_FAR *pvarSearchAdmin);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_BuildCatalogs )( 
            IBuildServer __RPC_FAR * This,
            /* [retval][out] */ VARIANT __RPC_FAR *pvarBuildCatalogs);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Sites )( 
            IBuildServer __RPC_FAR * This,
            /* [retval][out] */ VARIANT __RPC_FAR *pvarServers);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_ProxyUsageSetting )( 
            IBuildServer __RPC_FAR * This,
            /* [retval][out] */ LONG __RPC_FAR *pVal);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_ProxyAddress )( 
            IBuildServer __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pbstrAddress);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_ProxyPortNumber )( 
            IBuildServer __RPC_FAR * This,
            /* [retval][out] */ LONG __RPC_FAR *pnPort);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_ProxyBypassForLocalAddresses )( 
            IBuildServer __RPC_FAR * This,
            /* [retval][out] */ LONG __RPC_FAR *pVal);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_ProxyBypassAddressesList )( 
            IBuildServer __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pVal);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_DefaultLogsLocation )( 
            IBuildServer __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pVal);
        
        /* [helpstring][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_DefaultLogsLocation )( 
            IBuildServer __RPC_FAR * This,
            /* [in] */ BSTR newVal);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Clone )( 
            IBuildServer __RPC_FAR * This,
            /* [retval][out] */ VARIANT __RPC_FAR *pvarBuildServer);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *SetDefaultAccount )( 
            IBuildServer __RPC_FAR * This,
            /* [in] */ BSTR bstrAccountName,
            /* [in] */ BSTR bstrPassword);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *SetDefaultPluginAccount )( 
            IBuildServer __RPC_FAR * This,
            /* [in] */ BSTR bstrAccountName,
            /* [in] */ BSTR bstrPassword);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *ExportCatalog )( 
            IBuildServer __RPC_FAR * This,
            /* [in] */ BSTR bstrCatalogName,
            /* [in] */ BSTR bstrFilePath);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *ImportCatalog )( 
            IBuildServer __RPC_FAR * This,
            /* [in] */ BSTR bstrCatalogName,
            /* [in] */ BSTR bstrFilePath);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *SetProxySettings )( 
            IBuildServer __RPC_FAR * This,
            /* [in] */ LONG nUsageSetting,
            /* [in] */ BSTR bstrAddress,
            /* [in] */ LONG nPort,
            /* [in] */ LONG nBypassForLocalAddresses,
            /* [in] */ BSTR bstrBypassAddressesList);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *LoadConfiguration )( 
            IBuildServer __RPC_FAR * This,
            /* [in] */ BSTR bstrFilePath);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *SaveConfiguration )( 
            IBuildServer __RPC_FAR * This,
            /* [in] */ BSTR bstrFilePath);
        
        END_INTERFACE
    } IBuildServerVtbl;

    interface IBuildServer
    {
        CONST_VTBL struct IBuildServerVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IBuildServer_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IBuildServer_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IBuildServer_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IBuildServer_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IBuildServer_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IBuildServer_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IBuildServer_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IBuildServer_get_UserAgent(This,pVal)	\
    (This)->lpVtbl -> get_UserAgent(This,pVal)

#define IBuildServer_put_UserAgent(This,newVal)	\
    (This)->lpVtbl -> put_UserAgent(This,newVal)

#define IBuildServer_get_EmailAddress(This,pVal)	\
    (This)->lpVtbl -> get_EmailAddress(This,pVal)

#define IBuildServer_put_EmailAddress(This,newVal)	\
    (This)->lpVtbl -> put_EmailAddress(This,newVal)

#define IBuildServer_get_TempPath(This,pVal)	\
    (This)->lpVtbl -> get_TempPath(This,pVal)

#define IBuildServer_put_TempPath(This,newVal)	\
    (This)->lpVtbl -> put_TempPath(This,newVal)

#define IBuildServer_get_PerformanceLevel(This,pVal)	\
    (This)->lpVtbl -> get_PerformanceLevel(This,pVal)

#define IBuildServer_put_PerformanceLevel(This,newVal)	\
    (This)->lpVtbl -> put_PerformanceLevel(This,newVal)

#define IBuildServer_get_DefaultCatalogsLocation(This,pVal)	\
    (This)->lpVtbl -> get_DefaultCatalogsLocation(This,pVal)

#define IBuildServer_put_DefaultCatalogsLocation(This,newVal)	\
    (This)->lpVtbl -> put_DefaultCatalogsLocation(This,newVal)

#define IBuildServer_get_ConnectTimeout(This,pVal)	\
    (This)->lpVtbl -> get_ConnectTimeout(This,pVal)

#define IBuildServer_put_ConnectTimeout(This,newVal)	\
    (This)->lpVtbl -> put_ConnectTimeout(This,newVal)

#define IBuildServer_get_DataTimeout(This,pVal)	\
    (This)->lpVtbl -> get_DataTimeout(This,pVal)

#define IBuildServer_put_DataTimeout(This,newVal)	\
    (This)->lpVtbl -> put_DataTimeout(This,newVal)

#define IBuildServer_get_DefaultAccountName(This,pVal)	\
    (This)->lpVtbl -> get_DefaultAccountName(This,pVal)

#define IBuildServer_get_DefaultPluginAccountName(This,pVal)	\
    (This)->lpVtbl -> get_DefaultPluginAccountName(This,pVal)

#define IBuildServer_get_SearchAdmin(This,pvarSearchAdmin)	\
    (This)->lpVtbl -> get_SearchAdmin(This,pvarSearchAdmin)

#define IBuildServer_get_BuildCatalogs(This,pvarBuildCatalogs)	\
    (This)->lpVtbl -> get_BuildCatalogs(This,pvarBuildCatalogs)

#define IBuildServer_get_Sites(This,pvarServers)	\
    (This)->lpVtbl -> get_Sites(This,pvarServers)

#define IBuildServer_get_ProxyUsageSetting(This,pVal)	\
    (This)->lpVtbl -> get_ProxyUsageSetting(This,pVal)

#define IBuildServer_get_ProxyAddress(This,pbstrAddress)	\
    (This)->lpVtbl -> get_ProxyAddress(This,pbstrAddress)

#define IBuildServer_get_ProxyPortNumber(This,pnPort)	\
    (This)->lpVtbl -> get_ProxyPortNumber(This,pnPort)

#define IBuildServer_get_ProxyBypassForLocalAddresses(This,pVal)	\
    (This)->lpVtbl -> get_ProxyBypassForLocalAddresses(This,pVal)

#define IBuildServer_get_ProxyBypassAddressesList(This,pVal)	\
    (This)->lpVtbl -> get_ProxyBypassAddressesList(This,pVal)

#define IBuildServer_get_DefaultLogsLocation(This,pVal)	\
    (This)->lpVtbl -> get_DefaultLogsLocation(This,pVal)

#define IBuildServer_put_DefaultLogsLocation(This,newVal)	\
    (This)->lpVtbl -> put_DefaultLogsLocation(This,newVal)

#define IBuildServer_Clone(This,pvarBuildServer)	\
    (This)->lpVtbl -> Clone(This,pvarBuildServer)

#define IBuildServer_SetDefaultAccount(This,bstrAccountName,bstrPassword)	\
    (This)->lpVtbl -> SetDefaultAccount(This,bstrAccountName,bstrPassword)

#define IBuildServer_SetDefaultPluginAccount(This,bstrAccountName,bstrPassword)	\
    (This)->lpVtbl -> SetDefaultPluginAccount(This,bstrAccountName,bstrPassword)

#define IBuildServer_ExportCatalog(This,bstrCatalogName,bstrFilePath)	\
    (This)->lpVtbl -> ExportCatalog(This,bstrCatalogName,bstrFilePath)

#define IBuildServer_ImportCatalog(This,bstrCatalogName,bstrFilePath)	\
    (This)->lpVtbl -> ImportCatalog(This,bstrCatalogName,bstrFilePath)

#define IBuildServer_SetProxySettings(This,nUsageSetting,bstrAddress,nPort,nBypassForLocalAddresses,bstrBypassAddressesList)	\
    (This)->lpVtbl -> SetProxySettings(This,nUsageSetting,bstrAddress,nPort,nBypassForLocalAddresses,bstrBypassAddressesList)

#define IBuildServer_LoadConfiguration(This,bstrFilePath)	\
    (This)->lpVtbl -> LoadConfiguration(This,bstrFilePath)

#define IBuildServer_SaveConfiguration(This,bstrFilePath)	\
    (This)->lpVtbl -> SaveConfiguration(This,bstrFilePath)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE IBuildServer_get_UserAgent_Proxy( 
    IBuildServer __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pVal);


void __RPC_STUB IBuildServer_get_UserAgent_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propput] */ HRESULT STDMETHODCALLTYPE IBuildServer_put_UserAgent_Proxy( 
    IBuildServer __RPC_FAR * This,
    /* [in] */ BSTR newVal);


void __RPC_STUB IBuildServer_put_UserAgent_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE IBuildServer_get_EmailAddress_Proxy( 
    IBuildServer __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pVal);


void __RPC_STUB IBuildServer_get_EmailAddress_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propput] */ HRESULT STDMETHODCALLTYPE IBuildServer_put_EmailAddress_Proxy( 
    IBuildServer __RPC_FAR * This,
    /* [in] */ BSTR newVal);


void __RPC_STUB IBuildServer_put_EmailAddress_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE IBuildServer_get_TempPath_Proxy( 
    IBuildServer __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pVal);


void __RPC_STUB IBuildServer_get_TempPath_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propput] */ HRESULT STDMETHODCALLTYPE IBuildServer_put_TempPath_Proxy( 
    IBuildServer __RPC_FAR * This,
    /* [in] */ BSTR newVal);


void __RPC_STUB IBuildServer_put_TempPath_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE IBuildServer_get_PerformanceLevel_Proxy( 
    IBuildServer __RPC_FAR * This,
    /* [retval][out] */ LONG __RPC_FAR *pVal);


void __RPC_STUB IBuildServer_get_PerformanceLevel_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propput] */ HRESULT STDMETHODCALLTYPE IBuildServer_put_PerformanceLevel_Proxy( 
    IBuildServer __RPC_FAR * This,
    /* [in] */ LONG newVal);


void __RPC_STUB IBuildServer_put_PerformanceLevel_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE IBuildServer_get_DefaultCatalogsLocation_Proxy( 
    IBuildServer __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pVal);


void __RPC_STUB IBuildServer_get_DefaultCatalogsLocation_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propput] */ HRESULT STDMETHODCALLTYPE IBuildServer_put_DefaultCatalogsLocation_Proxy( 
    IBuildServer __RPC_FAR * This,
    /* [in] */ BSTR newVal);


void __RPC_STUB IBuildServer_put_DefaultCatalogsLocation_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE IBuildServer_get_ConnectTimeout_Proxy( 
    IBuildServer __RPC_FAR * This,
    /* [retval][out] */ LONG __RPC_FAR *pVal);


void __RPC_STUB IBuildServer_get_ConnectTimeout_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propput] */ HRESULT STDMETHODCALLTYPE IBuildServer_put_ConnectTimeout_Proxy( 
    IBuildServer __RPC_FAR * This,
    /* [in] */ LONG newVal);


void __RPC_STUB IBuildServer_put_ConnectTimeout_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE IBuildServer_get_DataTimeout_Proxy( 
    IBuildServer __RPC_FAR * This,
    /* [retval][out] */ LONG __RPC_FAR *pVal);


void __RPC_STUB IBuildServer_get_DataTimeout_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propput] */ HRESULT STDMETHODCALLTYPE IBuildServer_put_DataTimeout_Proxy( 
    IBuildServer __RPC_FAR * This,
    /* [in] */ LONG newVal);


void __RPC_STUB IBuildServer_put_DataTimeout_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE IBuildServer_get_DefaultAccountName_Proxy( 
    IBuildServer __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pVal);


void __RPC_STUB IBuildServer_get_DefaultAccountName_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE IBuildServer_get_DefaultPluginAccountName_Proxy( 
    IBuildServer __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pVal);


void __RPC_STUB IBuildServer_get_DefaultPluginAccountName_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE IBuildServer_get_SearchAdmin_Proxy( 
    IBuildServer __RPC_FAR * This,
    /* [retval][out] */ VARIANT __RPC_FAR *pvarSearchAdmin);


void __RPC_STUB IBuildServer_get_SearchAdmin_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE IBuildServer_get_BuildCatalogs_Proxy( 
    IBuildServer __RPC_FAR * This,
    /* [retval][out] */ VARIANT __RPC_FAR *pvarBuildCatalogs);


void __RPC_STUB IBuildServer_get_BuildCatalogs_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE IBuildServer_get_Sites_Proxy( 
    IBuildServer __RPC_FAR * This,
    /* [retval][out] */ VARIANT __RPC_FAR *pvarServers);


void __RPC_STUB IBuildServer_get_Sites_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE IBuildServer_get_ProxyUsageSetting_Proxy( 
    IBuildServer __RPC_FAR * This,
    /* [retval][out] */ LONG __RPC_FAR *pVal);


void __RPC_STUB IBuildServer_get_ProxyUsageSetting_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE IBuildServer_get_ProxyAddress_Proxy( 
    IBuildServer __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pbstrAddress);


void __RPC_STUB IBuildServer_get_ProxyAddress_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE IBuildServer_get_ProxyPortNumber_Proxy( 
    IBuildServer __RPC_FAR * This,
    /* [retval][out] */ LONG __RPC_FAR *pnPort);


void __RPC_STUB IBuildServer_get_ProxyPortNumber_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE IBuildServer_get_ProxyBypassForLocalAddresses_Proxy( 
    IBuildServer __RPC_FAR * This,
    /* [retval][out] */ LONG __RPC_FAR *pVal);


void __RPC_STUB IBuildServer_get_ProxyBypassForLocalAddresses_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE IBuildServer_get_ProxyBypassAddressesList_Proxy( 
    IBuildServer __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pVal);


void __RPC_STUB IBuildServer_get_ProxyBypassAddressesList_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE IBuildServer_get_DefaultLogsLocation_Proxy( 
    IBuildServer __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pVal);


void __RPC_STUB IBuildServer_get_DefaultLogsLocation_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propput] */ HRESULT STDMETHODCALLTYPE IBuildServer_put_DefaultLogsLocation_Proxy( 
    IBuildServer __RPC_FAR * This,
    /* [in] */ BSTR newVal);


void __RPC_STUB IBuildServer_put_DefaultLogsLocation_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IBuildServer_Clone_Proxy( 
    IBuildServer __RPC_FAR * This,
    /* [retval][out] */ VARIANT __RPC_FAR *pvarBuildServer);


void __RPC_STUB IBuildServer_Clone_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IBuildServer_SetDefaultAccount_Proxy( 
    IBuildServer __RPC_FAR * This,
    /* [in] */ BSTR bstrAccountName,
    /* [in] */ BSTR bstrPassword);


void __RPC_STUB IBuildServer_SetDefaultAccount_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IBuildServer_SetDefaultPluginAccount_Proxy( 
    IBuildServer __RPC_FAR * This,
    /* [in] */ BSTR bstrAccountName,
    /* [in] */ BSTR bstrPassword);


void __RPC_STUB IBuildServer_SetDefaultPluginAccount_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IBuildServer_ExportCatalog_Proxy( 
    IBuildServer __RPC_FAR * This,
    /* [in] */ BSTR bstrCatalogName,
    /* [in] */ BSTR bstrFilePath);


void __RPC_STUB IBuildServer_ExportCatalog_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IBuildServer_ImportCatalog_Proxy( 
    IBuildServer __RPC_FAR * This,
    /* [in] */ BSTR bstrCatalogName,
    /* [in] */ BSTR bstrFilePath);


void __RPC_STUB IBuildServer_ImportCatalog_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IBuildServer_SetProxySettings_Proxy( 
    IBuildServer __RPC_FAR * This,
    /* [in] */ LONG nUsageSetting,
    /* [in] */ BSTR bstrAddress,
    /* [in] */ LONG nPort,
    /* [in] */ LONG nBypassForLocalAddresses,
    /* [in] */ BSTR bstrBypassAddressesList);


void __RPC_STUB IBuildServer_SetProxySettings_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IBuildServer_LoadConfiguration_Proxy( 
    IBuildServer __RPC_FAR * This,
    /* [in] */ BSTR bstrFilePath);


void __RPC_STUB IBuildServer_LoadConfiguration_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IBuildServer_SaveConfiguration_Proxy( 
    IBuildServer __RPC_FAR * This,
    /* [in] */ BSTR bstrFilePath);


void __RPC_STUB IBuildServer_SaveConfiguration_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IBuildServer_INTERFACE_DEFINED__ */


#ifndef __IBuildCatalogs_INTERFACE_DEFINED__
#define __IBuildCatalogs_INTERFACE_DEFINED__

/****************************************
 * Generated header for interface: IBuildCatalogs
 * at Wed Apr 22 20:03:10 1998
 * using MIDL 3.01.75
 ****************************************/
/* [unique][helpstring][dual][uuid][object] */ 



EXTERN_C const IID IID_IBuildCatalogs;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    interface DECLSPEC_UUID("0b63e360-9ccc-11d0-bcdb-00805fccce04")
    IBuildCatalogs : public IDispatch
    {
    public:
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_Count( 
            /* [retval][out] */ long __RPC_FAR *pCount) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Item( 
            /* [in] */ BSTR bstrName,
            /* [retval][out] */ VARIANT __RPC_FAR *pvarCatalog) = 0;
        
        virtual /* [helpstring][id][restricted][propget] */ HRESULT STDMETHODCALLTYPE get__NewEnum( 
            /* [retval][out] */ IUnknown __RPC_FAR *__RPC_FAR *pNewEnum) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE Add( 
            /* [in] */ BSTR bstrName,
            /* [in] */ BSTR bstrPath,
            /* [retval][out] */ VARIANT __RPC_FAR *pvarCatalog) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE Remove( 
            /* [in] */ BSTR bstrName) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IBuildCatalogsVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IBuildCatalogs __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IBuildCatalogs __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IBuildCatalogs __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            IBuildCatalogs __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            IBuildCatalogs __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            IBuildCatalogs __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            IBuildCatalogs __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Count )( 
            IBuildCatalogs __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *pCount);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Item )( 
            IBuildCatalogs __RPC_FAR * This,
            /* [in] */ BSTR bstrName,
            /* [retval][out] */ VARIANT __RPC_FAR *pvarCatalog);
        
        /* [helpstring][id][restricted][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get__NewEnum )( 
            IBuildCatalogs __RPC_FAR * This,
            /* [retval][out] */ IUnknown __RPC_FAR *__RPC_FAR *pNewEnum);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Add )( 
            IBuildCatalogs __RPC_FAR * This,
            /* [in] */ BSTR bstrName,
            /* [in] */ BSTR bstrPath,
            /* [retval][out] */ VARIANT __RPC_FAR *pvarCatalog);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Remove )( 
            IBuildCatalogs __RPC_FAR * This,
            /* [in] */ BSTR bstrName);
        
        END_INTERFACE
    } IBuildCatalogsVtbl;

    interface IBuildCatalogs
    {
        CONST_VTBL struct IBuildCatalogsVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IBuildCatalogs_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IBuildCatalogs_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IBuildCatalogs_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IBuildCatalogs_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IBuildCatalogs_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IBuildCatalogs_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IBuildCatalogs_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IBuildCatalogs_get_Count(This,pCount)	\
    (This)->lpVtbl -> get_Count(This,pCount)

#define IBuildCatalogs_get_Item(This,bstrName,pvarCatalog)	\
    (This)->lpVtbl -> get_Item(This,bstrName,pvarCatalog)

#define IBuildCatalogs_get__NewEnum(This,pNewEnum)	\
    (This)->lpVtbl -> get__NewEnum(This,pNewEnum)

#define IBuildCatalogs_Add(This,bstrName,bstrPath,pvarCatalog)	\
    (This)->lpVtbl -> Add(This,bstrName,bstrPath,pvarCatalog)

#define IBuildCatalogs_Remove(This,bstrName)	\
    (This)->lpVtbl -> Remove(This,bstrName)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE IBuildCatalogs_get_Count_Proxy( 
    IBuildCatalogs __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *pCount);


void __RPC_STUB IBuildCatalogs_get_Count_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IBuildCatalogs_get_Item_Proxy( 
    IBuildCatalogs __RPC_FAR * This,
    /* [in] */ BSTR bstrName,
    /* [retval][out] */ VARIANT __RPC_FAR *pvarCatalog);


void __RPC_STUB IBuildCatalogs_get_Item_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][restricted][propget] */ HRESULT STDMETHODCALLTYPE IBuildCatalogs_get__NewEnum_Proxy( 
    IBuildCatalogs __RPC_FAR * This,
    /* [retval][out] */ IUnknown __RPC_FAR *__RPC_FAR *pNewEnum);


void __RPC_STUB IBuildCatalogs_get__NewEnum_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IBuildCatalogs_Add_Proxy( 
    IBuildCatalogs __RPC_FAR * This,
    /* [in] */ BSTR bstrName,
    /* [in] */ BSTR bstrPath,
    /* [retval][out] */ VARIANT __RPC_FAR *pvarCatalog);


void __RPC_STUB IBuildCatalogs_Add_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IBuildCatalogs_Remove_Proxy( 
    IBuildCatalogs __RPC_FAR * This,
    /* [in] */ BSTR bstrName);


void __RPC_STUB IBuildCatalogs_Remove_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IBuildCatalogs_INTERFACE_DEFINED__ */


#ifndef __IBuildCatalog_INTERFACE_DEFINED__
#define __IBuildCatalog_INTERFACE_DEFINED__

/****************************************
 * Generated header for interface: IBuildCatalog
 * at Wed Apr 22 20:03:10 1998
 * using MIDL 3.01.75
 ****************************************/
/* [unique][helpstring][dual][uuid][object] */ 



EXTERN_C const IID IID_IBuildCatalog;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    interface DECLSPEC_UUID("0b63e354-9ccc-11d0-bcdb-00805fccce04")
    IBuildCatalog : public IDispatch
    {
    public:
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_Name( 
            /* [retval][out] */ BSTR __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_LogSuccess( 
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][propput] */ HRESULT STDMETHODCALLTYPE put_LogSuccess( 
            /* [in] */ VARIANT_BOOL newVal) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_LogExcluded( 
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][propput] */ HRESULT STDMETHODCALLTYPE put_LogExcluded( 
            /* [in] */ VARIANT_BOOL newVal) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_BuildServer( 
            /* [retval][out] */ VARIANT __RPC_FAR *pvarBuildServer) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_StartPages( 
            /* [retval][out] */ VARIANT __RPC_FAR *pvarStartPages) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_Sites( 
            /* [retval][out] */ VARIANT __RPC_FAR *pvarSites) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_Extensions( 
            /* [retval][out] */ VARIANT __RPC_FAR *pvarExtensions) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_Mappings( 
            /* [retval][out] */ VARIANT __RPC_FAR *pvarMappings) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_Protocols( 
            /* [retval][out] */ VARIANT __RPC_FAR *pvarProtocols) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_Status( 
            /* [retval][out] */ BSTR __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_StatusCode( 
            /* [retval][out] */ LONG __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_CurrentPages( 
            /* [retval][out] */ VARIANT __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_IsBuilding( 
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_SearchServers( 
            /* [retval][out] */ VARIANT __RPC_FAR *pSearchServers) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_FollowComplexUrls( 
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][propput] */ HRESULT STDMETHODCALLTYPE put_FollowComplexUrls( 
            /* [in] */ VARIANT_BOOL newVal) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_ProjectType( 
            /* [retval][out] */ LONG __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][propput] */ HRESULT STDMETHODCALLTYPE put_ProjectType( 
            /* [in] */ LONG newVal) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_NotificationSource( 
            /* [retval][out] */ BSTR __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][propput] */ HRESULT STDMETHODCALLTYPE put_NotificationSource( 
            /* [in] */ BSTR newVal) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_CurrentBuildProperties( 
            /* [retval][out] */ VARIANT __RPC_FAR *pvarCatalogProperties) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_PreviousBuildProperties( 
            /* [retval][out] */ VARIANT __RPC_FAR *pvarCatalogProperties) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_Logs( 
            /* [retval][out] */ VARIANT __RPC_FAR *pGatherLogs) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_AutoPropLimitEnabled( 
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][propput] */ HRESULT STDMETHODCALLTYPE put_AutoPropLimitEnabled( 
            /* [in] */ VARIANT_BOOL newVal) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_AutoPropLimit( 
            /* [retval][out] */ LONG __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][propput] */ HRESULT STDMETHODCALLTYPE put_AutoPropLimit( 
            /* [in] */ LONG newVal) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_IsLocalFileSchema( 
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][propput] */ HRESULT STDMETHODCALLTYPE put_IsLocalFileSchema( 
            /* [in] */ VARIANT_BOOL newVal) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_Schema( 
            /* [retval][out] */ VARIANT __RPC_FAR *pSearchSchema) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE Clone( 
            /* [retval][out] */ VARIANT __RPC_FAR *pvarBuildCatalog) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE StartIncrementalBuild( void) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE StartBuild( void) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE StopBuild( void) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE PauseBuild( void) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE ResumeBuild( void) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE ForceProp( void) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE AbortProp( void) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IBuildCatalogVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IBuildCatalog __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IBuildCatalog __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IBuildCatalog __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            IBuildCatalog __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            IBuildCatalog __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            IBuildCatalog __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            IBuildCatalog __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Name )( 
            IBuildCatalog __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pVal);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_LogSuccess )( 
            IBuildCatalog __RPC_FAR * This,
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pVal);
        
        /* [helpstring][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_LogSuccess )( 
            IBuildCatalog __RPC_FAR * This,
            /* [in] */ VARIANT_BOOL newVal);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_LogExcluded )( 
            IBuildCatalog __RPC_FAR * This,
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pVal);
        
        /* [helpstring][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_LogExcluded )( 
            IBuildCatalog __RPC_FAR * This,
            /* [in] */ VARIANT_BOOL newVal);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_BuildServer )( 
            IBuildCatalog __RPC_FAR * This,
            /* [retval][out] */ VARIANT __RPC_FAR *pvarBuildServer);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_StartPages )( 
            IBuildCatalog __RPC_FAR * This,
            /* [retval][out] */ VARIANT __RPC_FAR *pvarStartPages);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Sites )( 
            IBuildCatalog __RPC_FAR * This,
            /* [retval][out] */ VARIANT __RPC_FAR *pvarSites);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Extensions )( 
            IBuildCatalog __RPC_FAR * This,
            /* [retval][out] */ VARIANT __RPC_FAR *pvarExtensions);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Mappings )( 
            IBuildCatalog __RPC_FAR * This,
            /* [retval][out] */ VARIANT __RPC_FAR *pvarMappings);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Protocols )( 
            IBuildCatalog __RPC_FAR * This,
            /* [retval][out] */ VARIANT __RPC_FAR *pvarProtocols);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Status )( 
            IBuildCatalog __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pVal);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_StatusCode )( 
            IBuildCatalog __RPC_FAR * This,
            /* [retval][out] */ LONG __RPC_FAR *pVal);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_CurrentPages )( 
            IBuildCatalog __RPC_FAR * This,
            /* [retval][out] */ VARIANT __RPC_FAR *pVal);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_IsBuilding )( 
            IBuildCatalog __RPC_FAR * This,
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pVal);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_SearchServers )( 
            IBuildCatalog __RPC_FAR * This,
            /* [retval][out] */ VARIANT __RPC_FAR *pSearchServers);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_FollowComplexUrls )( 
            IBuildCatalog __RPC_FAR * This,
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pVal);
        
        /* [helpstring][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_FollowComplexUrls )( 
            IBuildCatalog __RPC_FAR * This,
            /* [in] */ VARIANT_BOOL newVal);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_ProjectType )( 
            IBuildCatalog __RPC_FAR * This,
            /* [retval][out] */ LONG __RPC_FAR *pVal);
        
        /* [helpstring][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_ProjectType )( 
            IBuildCatalog __RPC_FAR * This,
            /* [in] */ LONG newVal);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_NotificationSource )( 
            IBuildCatalog __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pVal);
        
        /* [helpstring][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_NotificationSource )( 
            IBuildCatalog __RPC_FAR * This,
            /* [in] */ BSTR newVal);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_CurrentBuildProperties )( 
            IBuildCatalog __RPC_FAR * This,
            /* [retval][out] */ VARIANT __RPC_FAR *pvarCatalogProperties);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_PreviousBuildProperties )( 
            IBuildCatalog __RPC_FAR * This,
            /* [retval][out] */ VARIANT __RPC_FAR *pvarCatalogProperties);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Logs )( 
            IBuildCatalog __RPC_FAR * This,
            /* [retval][out] */ VARIANT __RPC_FAR *pGatherLogs);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_AutoPropLimitEnabled )( 
            IBuildCatalog __RPC_FAR * This,
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pVal);
        
        /* [helpstring][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_AutoPropLimitEnabled )( 
            IBuildCatalog __RPC_FAR * This,
            /* [in] */ VARIANT_BOOL newVal);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_AutoPropLimit )( 
            IBuildCatalog __RPC_FAR * This,
            /* [retval][out] */ LONG __RPC_FAR *pVal);
        
        /* [helpstring][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_AutoPropLimit )( 
            IBuildCatalog __RPC_FAR * This,
            /* [in] */ LONG newVal);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_IsLocalFileSchema )( 
            IBuildCatalog __RPC_FAR * This,
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pVal);
        
        /* [helpstring][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_IsLocalFileSchema )( 
            IBuildCatalog __RPC_FAR * This,
            /* [in] */ VARIANT_BOOL newVal);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Schema )( 
            IBuildCatalog __RPC_FAR * This,
            /* [retval][out] */ VARIANT __RPC_FAR *pSearchSchema);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Clone )( 
            IBuildCatalog __RPC_FAR * This,
            /* [retval][out] */ VARIANT __RPC_FAR *pvarBuildCatalog);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *StartIncrementalBuild )( 
            IBuildCatalog __RPC_FAR * This);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *StartBuild )( 
            IBuildCatalog __RPC_FAR * This);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *StopBuild )( 
            IBuildCatalog __RPC_FAR * This);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *PauseBuild )( 
            IBuildCatalog __RPC_FAR * This);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *ResumeBuild )( 
            IBuildCatalog __RPC_FAR * This);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *ForceProp )( 
            IBuildCatalog __RPC_FAR * This);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *AbortProp )( 
            IBuildCatalog __RPC_FAR * This);
        
        END_INTERFACE
    } IBuildCatalogVtbl;

    interface IBuildCatalog
    {
        CONST_VTBL struct IBuildCatalogVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IBuildCatalog_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IBuildCatalog_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IBuildCatalog_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IBuildCatalog_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IBuildCatalog_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IBuildCatalog_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IBuildCatalog_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IBuildCatalog_get_Name(This,pVal)	\
    (This)->lpVtbl -> get_Name(This,pVal)

#define IBuildCatalog_get_LogSuccess(This,pVal)	\
    (This)->lpVtbl -> get_LogSuccess(This,pVal)

#define IBuildCatalog_put_LogSuccess(This,newVal)	\
    (This)->lpVtbl -> put_LogSuccess(This,newVal)

#define IBuildCatalog_get_LogExcluded(This,pVal)	\
    (This)->lpVtbl -> get_LogExcluded(This,pVal)

#define IBuildCatalog_put_LogExcluded(This,newVal)	\
    (This)->lpVtbl -> put_LogExcluded(This,newVal)

#define IBuildCatalog_get_BuildServer(This,pvarBuildServer)	\
    (This)->lpVtbl -> get_BuildServer(This,pvarBuildServer)

#define IBuildCatalog_get_StartPages(This,pvarStartPages)	\
    (This)->lpVtbl -> get_StartPages(This,pvarStartPages)

#define IBuildCatalog_get_Sites(This,pvarSites)	\
    (This)->lpVtbl -> get_Sites(This,pvarSites)

#define IBuildCatalog_get_Extensions(This,pvarExtensions)	\
    (This)->lpVtbl -> get_Extensions(This,pvarExtensions)

#define IBuildCatalog_get_Mappings(This,pvarMappings)	\
    (This)->lpVtbl -> get_Mappings(This,pvarMappings)

#define IBuildCatalog_get_Protocols(This,pvarProtocols)	\
    (This)->lpVtbl -> get_Protocols(This,pvarProtocols)

#define IBuildCatalog_get_Status(This,pVal)	\
    (This)->lpVtbl -> get_Status(This,pVal)

#define IBuildCatalog_get_StatusCode(This,pVal)	\
    (This)->lpVtbl -> get_StatusCode(This,pVal)

#define IBuildCatalog_get_CurrentPages(This,pVal)	\
    (This)->lpVtbl -> get_CurrentPages(This,pVal)

#define IBuildCatalog_get_IsBuilding(This,pVal)	\
    (This)->lpVtbl -> get_IsBuilding(This,pVal)

#define IBuildCatalog_get_SearchServers(This,pSearchServers)	\
    (This)->lpVtbl -> get_SearchServers(This,pSearchServers)

#define IBuildCatalog_get_FollowComplexUrls(This,pVal)	\
    (This)->lpVtbl -> get_FollowComplexUrls(This,pVal)

#define IBuildCatalog_put_FollowComplexUrls(This,newVal)	\
    (This)->lpVtbl -> put_FollowComplexUrls(This,newVal)

#define IBuildCatalog_get_ProjectType(This,pVal)	\
    (This)->lpVtbl -> get_ProjectType(This,pVal)

#define IBuildCatalog_put_ProjectType(This,newVal)	\
    (This)->lpVtbl -> put_ProjectType(This,newVal)

#define IBuildCatalog_get_NotificationSource(This,pVal)	\
    (This)->lpVtbl -> get_NotificationSource(This,pVal)

#define IBuildCatalog_put_NotificationSource(This,newVal)	\
    (This)->lpVtbl -> put_NotificationSource(This,newVal)

#define IBuildCatalog_get_CurrentBuildProperties(This,pvarCatalogProperties)	\
    (This)->lpVtbl -> get_CurrentBuildProperties(This,pvarCatalogProperties)

#define IBuildCatalog_get_PreviousBuildProperties(This,pvarCatalogProperties)	\
    (This)->lpVtbl -> get_PreviousBuildProperties(This,pvarCatalogProperties)

#define IBuildCatalog_get_Logs(This,pGatherLogs)	\
    (This)->lpVtbl -> get_Logs(This,pGatherLogs)

#define IBuildCatalog_get_AutoPropLimitEnabled(This,pVal)	\
    (This)->lpVtbl -> get_AutoPropLimitEnabled(This,pVal)

#define IBuildCatalog_put_AutoPropLimitEnabled(This,newVal)	\
    (This)->lpVtbl -> put_AutoPropLimitEnabled(This,newVal)

#define IBuildCatalog_get_AutoPropLimit(This,pVal)	\
    (This)->lpVtbl -> get_AutoPropLimit(This,pVal)

#define IBuildCatalog_put_AutoPropLimit(This,newVal)	\
    (This)->lpVtbl -> put_AutoPropLimit(This,newVal)

#define IBuildCatalog_get_IsLocalFileSchema(This,pVal)	\
    (This)->lpVtbl -> get_IsLocalFileSchema(This,pVal)

#define IBuildCatalog_put_IsLocalFileSchema(This,newVal)	\
    (This)->lpVtbl -> put_IsLocalFileSchema(This,newVal)

#define IBuildCatalog_get_Schema(This,pSearchSchema)	\
    (This)->lpVtbl -> get_Schema(This,pSearchSchema)

#define IBuildCatalog_Clone(This,pvarBuildCatalog)	\
    (This)->lpVtbl -> Clone(This,pvarBuildCatalog)

#define IBuildCatalog_StartIncrementalBuild(This)	\
    (This)->lpVtbl -> StartIncrementalBuild(This)

#define IBuildCatalog_StartBuild(This)	\
    (This)->lpVtbl -> StartBuild(This)

#define IBuildCatalog_StopBuild(This)	\
    (This)->lpVtbl -> StopBuild(This)

#define IBuildCatalog_PauseBuild(This)	\
    (This)->lpVtbl -> PauseBuild(This)

#define IBuildCatalog_ResumeBuild(This)	\
    (This)->lpVtbl -> ResumeBuild(This)

#define IBuildCatalog_ForceProp(This)	\
    (This)->lpVtbl -> ForceProp(This)

#define IBuildCatalog_AbortProp(This)	\
    (This)->lpVtbl -> AbortProp(This)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE IBuildCatalog_get_Name_Proxy( 
    IBuildCatalog __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pVal);


void __RPC_STUB IBuildCatalog_get_Name_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE IBuildCatalog_get_LogSuccess_Proxy( 
    IBuildCatalog __RPC_FAR * This,
    /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pVal);


void __RPC_STUB IBuildCatalog_get_LogSuccess_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propput] */ HRESULT STDMETHODCALLTYPE IBuildCatalog_put_LogSuccess_Proxy( 
    IBuildCatalog __RPC_FAR * This,
    /* [in] */ VARIANT_BOOL newVal);


void __RPC_STUB IBuildCatalog_put_LogSuccess_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE IBuildCatalog_get_LogExcluded_Proxy( 
    IBuildCatalog __RPC_FAR * This,
    /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pVal);


void __RPC_STUB IBuildCatalog_get_LogExcluded_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propput] */ HRESULT STDMETHODCALLTYPE IBuildCatalog_put_LogExcluded_Proxy( 
    IBuildCatalog __RPC_FAR * This,
    /* [in] */ VARIANT_BOOL newVal);


void __RPC_STUB IBuildCatalog_put_LogExcluded_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE IBuildCatalog_get_BuildServer_Proxy( 
    IBuildCatalog __RPC_FAR * This,
    /* [retval][out] */ VARIANT __RPC_FAR *pvarBuildServer);


void __RPC_STUB IBuildCatalog_get_BuildServer_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE IBuildCatalog_get_StartPages_Proxy( 
    IBuildCatalog __RPC_FAR * This,
    /* [retval][out] */ VARIANT __RPC_FAR *pvarStartPages);


void __RPC_STUB IBuildCatalog_get_StartPages_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE IBuildCatalog_get_Sites_Proxy( 
    IBuildCatalog __RPC_FAR * This,
    /* [retval][out] */ VARIANT __RPC_FAR *pvarSites);


void __RPC_STUB IBuildCatalog_get_Sites_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE IBuildCatalog_get_Extensions_Proxy( 
    IBuildCatalog __RPC_FAR * This,
    /* [retval][out] */ VARIANT __RPC_FAR *pvarExtensions);


void __RPC_STUB IBuildCatalog_get_Extensions_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE IBuildCatalog_get_Mappings_Proxy( 
    IBuildCatalog __RPC_FAR * This,
    /* [retval][out] */ VARIANT __RPC_FAR *pvarMappings);


void __RPC_STUB IBuildCatalog_get_Mappings_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE IBuildCatalog_get_Protocols_Proxy( 
    IBuildCatalog __RPC_FAR * This,
    /* [retval][out] */ VARIANT __RPC_FAR *pvarProtocols);


void __RPC_STUB IBuildCatalog_get_Protocols_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE IBuildCatalog_get_Status_Proxy( 
    IBuildCatalog __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pVal);


void __RPC_STUB IBuildCatalog_get_Status_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE IBuildCatalog_get_StatusCode_Proxy( 
    IBuildCatalog __RPC_FAR * This,
    /* [retval][out] */ LONG __RPC_FAR *pVal);


void __RPC_STUB IBuildCatalog_get_StatusCode_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE IBuildCatalog_get_CurrentPages_Proxy( 
    IBuildCatalog __RPC_FAR * This,
    /* [retval][out] */ VARIANT __RPC_FAR *pVal);


void __RPC_STUB IBuildCatalog_get_CurrentPages_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE IBuildCatalog_get_IsBuilding_Proxy( 
    IBuildCatalog __RPC_FAR * This,
    /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pVal);


void __RPC_STUB IBuildCatalog_get_IsBuilding_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE IBuildCatalog_get_SearchServers_Proxy( 
    IBuildCatalog __RPC_FAR * This,
    /* [retval][out] */ VARIANT __RPC_FAR *pSearchServers);


void __RPC_STUB IBuildCatalog_get_SearchServers_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE IBuildCatalog_get_FollowComplexUrls_Proxy( 
    IBuildCatalog __RPC_FAR * This,
    /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pVal);


void __RPC_STUB IBuildCatalog_get_FollowComplexUrls_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propput] */ HRESULT STDMETHODCALLTYPE IBuildCatalog_put_FollowComplexUrls_Proxy( 
    IBuildCatalog __RPC_FAR * This,
    /* [in] */ VARIANT_BOOL newVal);


void __RPC_STUB IBuildCatalog_put_FollowComplexUrls_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE IBuildCatalog_get_ProjectType_Proxy( 
    IBuildCatalog __RPC_FAR * This,
    /* [retval][out] */ LONG __RPC_FAR *pVal);


void __RPC_STUB IBuildCatalog_get_ProjectType_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propput] */ HRESULT STDMETHODCALLTYPE IBuildCatalog_put_ProjectType_Proxy( 
    IBuildCatalog __RPC_FAR * This,
    /* [in] */ LONG newVal);


void __RPC_STUB IBuildCatalog_put_ProjectType_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE IBuildCatalog_get_NotificationSource_Proxy( 
    IBuildCatalog __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pVal);


void __RPC_STUB IBuildCatalog_get_NotificationSource_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propput] */ HRESULT STDMETHODCALLTYPE IBuildCatalog_put_NotificationSource_Proxy( 
    IBuildCatalog __RPC_FAR * This,
    /* [in] */ BSTR newVal);


void __RPC_STUB IBuildCatalog_put_NotificationSource_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE IBuildCatalog_get_CurrentBuildProperties_Proxy( 
    IBuildCatalog __RPC_FAR * This,
    /* [retval][out] */ VARIANT __RPC_FAR *pvarCatalogProperties);


void __RPC_STUB IBuildCatalog_get_CurrentBuildProperties_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE IBuildCatalog_get_PreviousBuildProperties_Proxy( 
    IBuildCatalog __RPC_FAR * This,
    /* [retval][out] */ VARIANT __RPC_FAR *pvarCatalogProperties);


void __RPC_STUB IBuildCatalog_get_PreviousBuildProperties_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE IBuildCatalog_get_Logs_Proxy( 
    IBuildCatalog __RPC_FAR * This,
    /* [retval][out] */ VARIANT __RPC_FAR *pGatherLogs);


void __RPC_STUB IBuildCatalog_get_Logs_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE IBuildCatalog_get_AutoPropLimitEnabled_Proxy( 
    IBuildCatalog __RPC_FAR * This,
    /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pVal);


void __RPC_STUB IBuildCatalog_get_AutoPropLimitEnabled_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propput] */ HRESULT STDMETHODCALLTYPE IBuildCatalog_put_AutoPropLimitEnabled_Proxy( 
    IBuildCatalog __RPC_FAR * This,
    /* [in] */ VARIANT_BOOL newVal);


void __RPC_STUB IBuildCatalog_put_AutoPropLimitEnabled_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE IBuildCatalog_get_AutoPropLimit_Proxy( 
    IBuildCatalog __RPC_FAR * This,
    /* [retval][out] */ LONG __RPC_FAR *pVal);


void __RPC_STUB IBuildCatalog_get_AutoPropLimit_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propput] */ HRESULT STDMETHODCALLTYPE IBuildCatalog_put_AutoPropLimit_Proxy( 
    IBuildCatalog __RPC_FAR * This,
    /* [in] */ LONG newVal);


void __RPC_STUB IBuildCatalog_put_AutoPropLimit_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE IBuildCatalog_get_IsLocalFileSchema_Proxy( 
    IBuildCatalog __RPC_FAR * This,
    /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pVal);


void __RPC_STUB IBuildCatalog_get_IsLocalFileSchema_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propput] */ HRESULT STDMETHODCALLTYPE IBuildCatalog_put_IsLocalFileSchema_Proxy( 
    IBuildCatalog __RPC_FAR * This,
    /* [in] */ VARIANT_BOOL newVal);


void __RPC_STUB IBuildCatalog_put_IsLocalFileSchema_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE IBuildCatalog_get_Schema_Proxy( 
    IBuildCatalog __RPC_FAR * This,
    /* [retval][out] */ VARIANT __RPC_FAR *pSearchSchema);


void __RPC_STUB IBuildCatalog_get_Schema_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IBuildCatalog_Clone_Proxy( 
    IBuildCatalog __RPC_FAR * This,
    /* [retval][out] */ VARIANT __RPC_FAR *pvarBuildCatalog);


void __RPC_STUB IBuildCatalog_Clone_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IBuildCatalog_StartIncrementalBuild_Proxy( 
    IBuildCatalog __RPC_FAR * This);


void __RPC_STUB IBuildCatalog_StartIncrementalBuild_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IBuildCatalog_StartBuild_Proxy( 
    IBuildCatalog __RPC_FAR * This);


void __RPC_STUB IBuildCatalog_StartBuild_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IBuildCatalog_StopBuild_Proxy( 
    IBuildCatalog __RPC_FAR * This);


void __RPC_STUB IBuildCatalog_StopBuild_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IBuildCatalog_PauseBuild_Proxy( 
    IBuildCatalog __RPC_FAR * This);


void __RPC_STUB IBuildCatalog_PauseBuild_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IBuildCatalog_ResumeBuild_Proxy( 
    IBuildCatalog __RPC_FAR * This);


void __RPC_STUB IBuildCatalog_ResumeBuild_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IBuildCatalog_ForceProp_Proxy( 
    IBuildCatalog __RPC_FAR * This);


void __RPC_STUB IBuildCatalog_ForceProp_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IBuildCatalog_AbortProp_Proxy( 
    IBuildCatalog __RPC_FAR * This);


void __RPC_STUB IBuildCatalog_AbortProp_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IBuildCatalog_INTERFACE_DEFINED__ */


#ifndef __ISearchServer_INTERFACE_DEFINED__
#define __ISearchServer_INTERFACE_DEFINED__

/****************************************
 * Generated header for interface: ISearchServer
 * at Wed Apr 22 20:03:10 1998
 * using MIDL 3.01.75
 ****************************************/
/* [unique][helpstring][dual][uuid][object] */ 



EXTERN_C const IID IID_ISearchServer;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    interface DECLSPEC_UUID("0b63e357-9ccc-11d0-bcdb-00805fccce04")
    ISearchServer : public IDispatch
    {
    public:
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_SearchAdmin( 
            /* [retval][out] */ VARIANT __RPC_FAR *pvarSearchAdmin) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_SearchCatalogs( 
            /* [retval][out] */ VARIANT __RPC_FAR *pvarSearchCatalogs) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_PerformanceLevel( 
            /* [retval][out] */ LONG __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][propput] */ HRESULT STDMETHODCALLTYPE put_PerformanceLevel( 
            /* [in] */ LONG newVal) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_CatalogsLocation( 
            /* [retval][out] */ BSTR __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][propput] */ HRESULT STDMETHODCALLTYPE put_CatalogsLocation( 
            /* [in] */ BSTR newVal) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_DefaultCatalog( 
            /* [retval][out] */ BSTR __RPC_FAR *pbstrSearchCatalogName) = 0;
        
        virtual /* [helpstring][propput] */ HRESULT STDMETHODCALLTYPE put_DefaultCatalog( 
            /* [in] */ BSTR bstrSearchCatalogName) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE Clone( 
            /* [retval][out] */ VARIANT __RPC_FAR *pvarSearchServer) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct ISearchServerVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            ISearchServer __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            ISearchServer __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            ISearchServer __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            ISearchServer __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            ISearchServer __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            ISearchServer __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            ISearchServer __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_SearchAdmin )( 
            ISearchServer __RPC_FAR * This,
            /* [retval][out] */ VARIANT __RPC_FAR *pvarSearchAdmin);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_SearchCatalogs )( 
            ISearchServer __RPC_FAR * This,
            /* [retval][out] */ VARIANT __RPC_FAR *pvarSearchCatalogs);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_PerformanceLevel )( 
            ISearchServer __RPC_FAR * This,
            /* [retval][out] */ LONG __RPC_FAR *pVal);
        
        /* [helpstring][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_PerformanceLevel )( 
            ISearchServer __RPC_FAR * This,
            /* [in] */ LONG newVal);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_CatalogsLocation )( 
            ISearchServer __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pVal);
        
        /* [helpstring][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_CatalogsLocation )( 
            ISearchServer __RPC_FAR * This,
            /* [in] */ BSTR newVal);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_DefaultCatalog )( 
            ISearchServer __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pbstrSearchCatalogName);
        
        /* [helpstring][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_DefaultCatalog )( 
            ISearchServer __RPC_FAR * This,
            /* [in] */ BSTR bstrSearchCatalogName);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Clone )( 
            ISearchServer __RPC_FAR * This,
            /* [retval][out] */ VARIANT __RPC_FAR *pvarSearchServer);
        
        END_INTERFACE
    } ISearchServerVtbl;

    interface ISearchServer
    {
        CONST_VTBL struct ISearchServerVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define ISearchServer_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define ISearchServer_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define ISearchServer_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define ISearchServer_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define ISearchServer_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define ISearchServer_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define ISearchServer_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define ISearchServer_get_SearchAdmin(This,pvarSearchAdmin)	\
    (This)->lpVtbl -> get_SearchAdmin(This,pvarSearchAdmin)

#define ISearchServer_get_SearchCatalogs(This,pvarSearchCatalogs)	\
    (This)->lpVtbl -> get_SearchCatalogs(This,pvarSearchCatalogs)

#define ISearchServer_get_PerformanceLevel(This,pVal)	\
    (This)->lpVtbl -> get_PerformanceLevel(This,pVal)

#define ISearchServer_put_PerformanceLevel(This,newVal)	\
    (This)->lpVtbl -> put_PerformanceLevel(This,newVal)

#define ISearchServer_get_CatalogsLocation(This,pVal)	\
    (This)->lpVtbl -> get_CatalogsLocation(This,pVal)

#define ISearchServer_put_CatalogsLocation(This,newVal)	\
    (This)->lpVtbl -> put_CatalogsLocation(This,newVal)

#define ISearchServer_get_DefaultCatalog(This,pbstrSearchCatalogName)	\
    (This)->lpVtbl -> get_DefaultCatalog(This,pbstrSearchCatalogName)

#define ISearchServer_put_DefaultCatalog(This,bstrSearchCatalogName)	\
    (This)->lpVtbl -> put_DefaultCatalog(This,bstrSearchCatalogName)

#define ISearchServer_Clone(This,pvarSearchServer)	\
    (This)->lpVtbl -> Clone(This,pvarSearchServer)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE ISearchServer_get_SearchAdmin_Proxy( 
    ISearchServer __RPC_FAR * This,
    /* [retval][out] */ VARIANT __RPC_FAR *pvarSearchAdmin);


void __RPC_STUB ISearchServer_get_SearchAdmin_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE ISearchServer_get_SearchCatalogs_Proxy( 
    ISearchServer __RPC_FAR * This,
    /* [retval][out] */ VARIANT __RPC_FAR *pvarSearchCatalogs);


void __RPC_STUB ISearchServer_get_SearchCatalogs_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE ISearchServer_get_PerformanceLevel_Proxy( 
    ISearchServer __RPC_FAR * This,
    /* [retval][out] */ LONG __RPC_FAR *pVal);


void __RPC_STUB ISearchServer_get_PerformanceLevel_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propput] */ HRESULT STDMETHODCALLTYPE ISearchServer_put_PerformanceLevel_Proxy( 
    ISearchServer __RPC_FAR * This,
    /* [in] */ LONG newVal);


void __RPC_STUB ISearchServer_put_PerformanceLevel_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE ISearchServer_get_CatalogsLocation_Proxy( 
    ISearchServer __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pVal);


void __RPC_STUB ISearchServer_get_CatalogsLocation_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propput] */ HRESULT STDMETHODCALLTYPE ISearchServer_put_CatalogsLocation_Proxy( 
    ISearchServer __RPC_FAR * This,
    /* [in] */ BSTR newVal);


void __RPC_STUB ISearchServer_put_CatalogsLocation_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE ISearchServer_get_DefaultCatalog_Proxy( 
    ISearchServer __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pbstrSearchCatalogName);


void __RPC_STUB ISearchServer_get_DefaultCatalog_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propput] */ HRESULT STDMETHODCALLTYPE ISearchServer_put_DefaultCatalog_Proxy( 
    ISearchServer __RPC_FAR * This,
    /* [in] */ BSTR bstrSearchCatalogName);


void __RPC_STUB ISearchServer_put_DefaultCatalog_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE ISearchServer_Clone_Proxy( 
    ISearchServer __RPC_FAR * This,
    /* [retval][out] */ VARIANT __RPC_FAR *pvarSearchServer);


void __RPC_STUB ISearchServer_Clone_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __ISearchServer_INTERFACE_DEFINED__ */


#ifndef __ISearchCatalogs_INTERFACE_DEFINED__
#define __ISearchCatalogs_INTERFACE_DEFINED__

/****************************************
 * Generated header for interface: ISearchCatalogs
 * at Wed Apr 22 20:03:10 1998
 * using MIDL 3.01.75
 ****************************************/
/* [unique][helpstring][dual][uuid][object] */ 



EXTERN_C const IID IID_ISearchCatalogs;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    interface DECLSPEC_UUID("0b63e361-9ccc-11d0-bcdb-00805fccce04")
    ISearchCatalogs : public IDispatch
    {
    public:
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_Count( 
            /* [retval][out] */ long __RPC_FAR *pCount) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Item( 
            /* [in] */ BSTR bstrName,
            /* [retval][out] */ VARIANT __RPC_FAR *pvarSearchCatalog) = 0;
        
        virtual /* [helpstring][id][restricted][propget] */ HRESULT STDMETHODCALLTYPE get__NewEnum( 
            /* [retval][out] */ IUnknown __RPC_FAR *__RPC_FAR *pNewEnum) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE Add( 
            /* [in] */ BSTR bstrName,
            /* [retval][out] */ VARIANT __RPC_FAR *pvarSearchCatalog) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE Remove( 
            /* [in] */ BSTR bstrName) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct ISearchCatalogsVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            ISearchCatalogs __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            ISearchCatalogs __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            ISearchCatalogs __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            ISearchCatalogs __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            ISearchCatalogs __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            ISearchCatalogs __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            ISearchCatalogs __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Count )( 
            ISearchCatalogs __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *pCount);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Item )( 
            ISearchCatalogs __RPC_FAR * This,
            /* [in] */ BSTR bstrName,
            /* [retval][out] */ VARIANT __RPC_FAR *pvarSearchCatalog);
        
        /* [helpstring][id][restricted][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get__NewEnum )( 
            ISearchCatalogs __RPC_FAR * This,
            /* [retval][out] */ IUnknown __RPC_FAR *__RPC_FAR *pNewEnum);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Add )( 
            ISearchCatalogs __RPC_FAR * This,
            /* [in] */ BSTR bstrName,
            /* [retval][out] */ VARIANT __RPC_FAR *pvarSearchCatalog);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Remove )( 
            ISearchCatalogs __RPC_FAR * This,
            /* [in] */ BSTR bstrName);
        
        END_INTERFACE
    } ISearchCatalogsVtbl;

    interface ISearchCatalogs
    {
        CONST_VTBL struct ISearchCatalogsVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define ISearchCatalogs_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define ISearchCatalogs_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define ISearchCatalogs_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define ISearchCatalogs_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define ISearchCatalogs_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define ISearchCatalogs_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define ISearchCatalogs_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define ISearchCatalogs_get_Count(This,pCount)	\
    (This)->lpVtbl -> get_Count(This,pCount)

#define ISearchCatalogs_get_Item(This,bstrName,pvarSearchCatalog)	\
    (This)->lpVtbl -> get_Item(This,bstrName,pvarSearchCatalog)

#define ISearchCatalogs_get__NewEnum(This,pNewEnum)	\
    (This)->lpVtbl -> get__NewEnum(This,pNewEnum)

#define ISearchCatalogs_Add(This,bstrName,pvarSearchCatalog)	\
    (This)->lpVtbl -> Add(This,bstrName,pvarSearchCatalog)

#define ISearchCatalogs_Remove(This,bstrName)	\
    (This)->lpVtbl -> Remove(This,bstrName)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE ISearchCatalogs_get_Count_Proxy( 
    ISearchCatalogs __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *pCount);


void __RPC_STUB ISearchCatalogs_get_Count_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE ISearchCatalogs_get_Item_Proxy( 
    ISearchCatalogs __RPC_FAR * This,
    /* [in] */ BSTR bstrName,
    /* [retval][out] */ VARIANT __RPC_FAR *pvarSearchCatalog);


void __RPC_STUB ISearchCatalogs_get_Item_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][restricted][propget] */ HRESULT STDMETHODCALLTYPE ISearchCatalogs_get__NewEnum_Proxy( 
    ISearchCatalogs __RPC_FAR * This,
    /* [retval][out] */ IUnknown __RPC_FAR *__RPC_FAR *pNewEnum);


void __RPC_STUB ISearchCatalogs_get__NewEnum_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE ISearchCatalogs_Add_Proxy( 
    ISearchCatalogs __RPC_FAR * This,
    /* [in] */ BSTR bstrName,
    /* [retval][out] */ VARIANT __RPC_FAR *pvarSearchCatalog);


void __RPC_STUB ISearchCatalogs_Add_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE ISearchCatalogs_Remove_Proxy( 
    ISearchCatalogs __RPC_FAR * This,
    /* [in] */ BSTR bstrName);


void __RPC_STUB ISearchCatalogs_Remove_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __ISearchCatalogs_INTERFACE_DEFINED__ */


#ifndef __ISearchCatalog_INTERFACE_DEFINED__
#define __ISearchCatalog_INTERFACE_DEFINED__

/****************************************
 * Generated header for interface: ISearchCatalog
 * at Wed Apr 22 20:03:10 1998
 * using MIDL 3.01.75
 ****************************************/
/* [unique][helpstring][dual][uuid][object] */ 



EXTERN_C const IID IID_ISearchCatalog;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    interface DECLSPEC_UUID("0b63e35e-9ccc-11d0-bcdb-00805fccce04")
    ISearchCatalog : public IDispatch
    {
    public:
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_Name( 
            /* [retval][out] */ BSTR __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_BuildServerName( 
            /* [retval][out] */ BSTR __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_SearchServer( 
            /* [retval][out] */ VARIANT __RPC_FAR *pvarSearchServer) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_Status( 
            /* [retval][out] */ BSTR __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_QueryTimeout( 
            /* [retval][out] */ DWORD __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][propput] */ HRESULT STDMETHODCALLTYPE put_QueryTimeout( 
            /* [in] */ DWORD newVal) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_MaxResultRows( 
            /* [retval][out] */ DWORD __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][propput] */ HRESULT STDMETHODCALLTYPE put_MaxResultRows( 
            /* [in] */ DWORD newVal) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_Properties( 
            /* [retval][out] */ VARIANT __RPC_FAR *pvarCatalogProperties) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE Clone( 
            /* [retval][out] */ VARIANT __RPC_FAR *pvarSearchCatalog) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE Enable( void) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE Disable( void) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE Update( void) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct ISearchCatalogVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            ISearchCatalog __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            ISearchCatalog __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            ISearchCatalog __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            ISearchCatalog __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            ISearchCatalog __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            ISearchCatalog __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            ISearchCatalog __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Name )( 
            ISearchCatalog __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pVal);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_BuildServerName )( 
            ISearchCatalog __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pVal);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_SearchServer )( 
            ISearchCatalog __RPC_FAR * This,
            /* [retval][out] */ VARIANT __RPC_FAR *pvarSearchServer);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Status )( 
            ISearchCatalog __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pVal);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_QueryTimeout )( 
            ISearchCatalog __RPC_FAR * This,
            /* [retval][out] */ DWORD __RPC_FAR *pVal);
        
        /* [helpstring][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_QueryTimeout )( 
            ISearchCatalog __RPC_FAR * This,
            /* [in] */ DWORD newVal);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_MaxResultRows )( 
            ISearchCatalog __RPC_FAR * This,
            /* [retval][out] */ DWORD __RPC_FAR *pVal);
        
        /* [helpstring][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_MaxResultRows )( 
            ISearchCatalog __RPC_FAR * This,
            /* [in] */ DWORD newVal);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Properties )( 
            ISearchCatalog __RPC_FAR * This,
            /* [retval][out] */ VARIANT __RPC_FAR *pvarCatalogProperties);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Clone )( 
            ISearchCatalog __RPC_FAR * This,
            /* [retval][out] */ VARIANT __RPC_FAR *pvarSearchCatalog);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Enable )( 
            ISearchCatalog __RPC_FAR * This);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Disable )( 
            ISearchCatalog __RPC_FAR * This);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Update )( 
            ISearchCatalog __RPC_FAR * This);
        
        END_INTERFACE
    } ISearchCatalogVtbl;

    interface ISearchCatalog
    {
        CONST_VTBL struct ISearchCatalogVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define ISearchCatalog_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define ISearchCatalog_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define ISearchCatalog_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define ISearchCatalog_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define ISearchCatalog_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define ISearchCatalog_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define ISearchCatalog_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define ISearchCatalog_get_Name(This,pVal)	\
    (This)->lpVtbl -> get_Name(This,pVal)

#define ISearchCatalog_get_BuildServerName(This,pVal)	\
    (This)->lpVtbl -> get_BuildServerName(This,pVal)

#define ISearchCatalog_get_SearchServer(This,pvarSearchServer)	\
    (This)->lpVtbl -> get_SearchServer(This,pvarSearchServer)

#define ISearchCatalog_get_Status(This,pVal)	\
    (This)->lpVtbl -> get_Status(This,pVal)

#define ISearchCatalog_get_QueryTimeout(This,pVal)	\
    (This)->lpVtbl -> get_QueryTimeout(This,pVal)

#define ISearchCatalog_put_QueryTimeout(This,newVal)	\
    (This)->lpVtbl -> put_QueryTimeout(This,newVal)

#define ISearchCatalog_get_MaxResultRows(This,pVal)	\
    (This)->lpVtbl -> get_MaxResultRows(This,pVal)

#define ISearchCatalog_put_MaxResultRows(This,newVal)	\
    (This)->lpVtbl -> put_MaxResultRows(This,newVal)

#define ISearchCatalog_get_Properties(This,pvarCatalogProperties)	\
    (This)->lpVtbl -> get_Properties(This,pvarCatalogProperties)

#define ISearchCatalog_Clone(This,pvarSearchCatalog)	\
    (This)->lpVtbl -> Clone(This,pvarSearchCatalog)

#define ISearchCatalog_Enable(This)	\
    (This)->lpVtbl -> Enable(This)

#define ISearchCatalog_Disable(This)	\
    (This)->lpVtbl -> Disable(This)

#define ISearchCatalog_Update(This)	\
    (This)->lpVtbl -> Update(This)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE ISearchCatalog_get_Name_Proxy( 
    ISearchCatalog __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pVal);


void __RPC_STUB ISearchCatalog_get_Name_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE ISearchCatalog_get_BuildServerName_Proxy( 
    ISearchCatalog __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pVal);


void __RPC_STUB ISearchCatalog_get_BuildServerName_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE ISearchCatalog_get_SearchServer_Proxy( 
    ISearchCatalog __RPC_FAR * This,
    /* [retval][out] */ VARIANT __RPC_FAR *pvarSearchServer);


void __RPC_STUB ISearchCatalog_get_SearchServer_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE ISearchCatalog_get_Status_Proxy( 
    ISearchCatalog __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pVal);


void __RPC_STUB ISearchCatalog_get_Status_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE ISearchCatalog_get_QueryTimeout_Proxy( 
    ISearchCatalog __RPC_FAR * This,
    /* [retval][out] */ DWORD __RPC_FAR *pVal);


void __RPC_STUB ISearchCatalog_get_QueryTimeout_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propput] */ HRESULT STDMETHODCALLTYPE ISearchCatalog_put_QueryTimeout_Proxy( 
    ISearchCatalog __RPC_FAR * This,
    /* [in] */ DWORD newVal);


void __RPC_STUB ISearchCatalog_put_QueryTimeout_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE ISearchCatalog_get_MaxResultRows_Proxy( 
    ISearchCatalog __RPC_FAR * This,
    /* [retval][out] */ DWORD __RPC_FAR *pVal);


void __RPC_STUB ISearchCatalog_get_MaxResultRows_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propput] */ HRESULT STDMETHODCALLTYPE ISearchCatalog_put_MaxResultRows_Proxy( 
    ISearchCatalog __RPC_FAR * This,
    /* [in] */ DWORD newVal);


void __RPC_STUB ISearchCatalog_put_MaxResultRows_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE ISearchCatalog_get_Properties_Proxy( 
    ISearchCatalog __RPC_FAR * This,
    /* [retval][out] */ VARIANT __RPC_FAR *pvarCatalogProperties);


void __RPC_STUB ISearchCatalog_get_Properties_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE ISearchCatalog_Clone_Proxy( 
    ISearchCatalog __RPC_FAR * This,
    /* [retval][out] */ VARIANT __RPC_FAR *pvarSearchCatalog);


void __RPC_STUB ISearchCatalog_Clone_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE ISearchCatalog_Enable_Proxy( 
    ISearchCatalog __RPC_FAR * This);


void __RPC_STUB ISearchCatalog_Enable_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE ISearchCatalog_Disable_Proxy( 
    ISearchCatalog __RPC_FAR * This);


void __RPC_STUB ISearchCatalog_Disable_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE ISearchCatalog_Update_Proxy( 
    ISearchCatalog __RPC_FAR * This);


void __RPC_STUB ISearchCatalog_Update_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __ISearchCatalog_INTERFACE_DEFINED__ */



#ifndef __NLADMINLib_LIBRARY_DEFINED__
#define __NLADMINLib_LIBRARY_DEFINED__

/****************************************
 * Generated header for library: NLADMINLib
 * at Wed Apr 22 20:03:10 1998
 * using MIDL 3.01.75
 ****************************************/
/* [helpstring][version][uuid] */ 









EXTERN_C const IID LIBID_NLADMINLib;

#ifdef __cplusplus
EXTERN_C const CLSID CLSID_SearchAdmin;

class DECLSPEC_UUID("0b63e35b-9ccc-11d0-bcdb-00805fccce04")
SearchAdmin;
#endif
#endif /* __NLADMINLib_LIBRARY_DEFINED__ */

/* Additional Prototypes for ALL interfaces */

unsigned long             __RPC_USER  BSTR_UserSize(     unsigned long __RPC_FAR *, unsigned long            , BSTR __RPC_FAR * ); 
unsigned char __RPC_FAR * __RPC_USER  BSTR_UserMarshal(  unsigned long __RPC_FAR *, unsigned char __RPC_FAR *, BSTR __RPC_FAR * ); 
unsigned char __RPC_FAR * __RPC_USER  BSTR_UserUnmarshal(unsigned long __RPC_FAR *, unsigned char __RPC_FAR *, BSTR __RPC_FAR * ); 
void                      __RPC_USER  BSTR_UserFree(     unsigned long __RPC_FAR *, BSTR __RPC_FAR * ); 

unsigned long             __RPC_USER  VARIANT_UserSize(     unsigned long __RPC_FAR *, unsigned long            , VARIANT __RPC_FAR * ); 
unsigned char __RPC_FAR * __RPC_USER  VARIANT_UserMarshal(  unsigned long __RPC_FAR *, unsigned char __RPC_FAR *, VARIANT __RPC_FAR * ); 
unsigned char __RPC_FAR * __RPC_USER  VARIANT_UserUnmarshal(unsigned long __RPC_FAR *, unsigned char __RPC_FAR *, VARIANT __RPC_FAR * ); 
void                      __RPC_USER  VARIANT_UserFree(     unsigned long __RPC_FAR *, VARIANT __RPC_FAR * ); 

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif
