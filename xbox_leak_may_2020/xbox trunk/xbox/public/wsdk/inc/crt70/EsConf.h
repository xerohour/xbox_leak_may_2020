/* this ALWAYS GENERATED file contains the definitions for the interfaces */


/* File created by MIDL compiler version 3.01.75 */
/* at Fri Mar 27 05:37:32 1998
 */
/* Compiler settings for J:\events\src\esconf\esconf.idl:
    Oicf (OptLev=i2), W1, Zp8, env=Win32, ms_ext, c_ext
    error checks: none
*/
//@@MIDL_FILE_HEADING(  )
#include "rpc.h"
#include "rpcndr.h"
#ifndef COM_NO_WINDOWS_H
#include "windows.h"
#include "ole2.h"
#endif /*COM_NO_WINDOWS_H*/

#ifndef __esconf_h__
#define __esconf_h__

#ifdef __cplusplus
extern "C"{
#endif 

/* Forward Declarations */ 

#ifndef __IEvents_FWD_DEFINED__
#define __IEvents_FWD_DEFINED__
typedef interface IEvents IEvents;
#endif 	/* __IEvents_FWD_DEFINED__ */


#ifndef __IHosts_FWD_DEFINED__
#define __IHosts_FWD_DEFINED__
typedef interface IHosts IHosts;
#endif 	/* __IHosts_FWD_DEFINED__ */


#ifndef __IHost_FWD_DEFINED__
#define __IHost_FWD_DEFINED__
typedef interface IHost IHost;
#endif 	/* __IHost_FWD_DEFINED__ */


#ifndef __IHandlers_FWD_DEFINED__
#define __IHandlers_FWD_DEFINED__
typedef interface IHandlers IHandlers;
#endif 	/* __IHandlers_FWD_DEFINED__ */


#ifndef __IHandler_FWD_DEFINED__
#define __IHandler_FWD_DEFINED__
typedef interface IHandler IHandler;
#endif 	/* __IHandler_FWD_DEFINED__ */


#ifndef __IBoundFolders_FWD_DEFINED__
#define __IBoundFolders_FWD_DEFINED__
typedef interface IBoundFolders IBoundFolders;
#endif 	/* __IBoundFolders_FWD_DEFINED__ */


#ifndef __IBoundFolder_FWD_DEFINED__
#define __IBoundFolder_FWD_DEFINED__
typedef interface IBoundFolder IBoundFolder;
#endif 	/* __IBoundFolder_FWD_DEFINED__ */


#ifndef __IEventBindings_FWD_DEFINED__
#define __IEventBindings_FWD_DEFINED__
typedef interface IEventBindings IEventBindings;
#endif 	/* __IEventBindings_FWD_DEFINED__ */


#ifndef __IEventBinding_FWD_DEFINED__
#define __IEventBinding_FWD_DEFINED__
typedef interface IEventBinding IEventBinding;
#endif 	/* __IEventBinding_FWD_DEFINED__ */


#ifndef __ISchedule_FWD_DEFINED__
#define __ISchedule_FWD_DEFINED__
typedef interface ISchedule ISchedule;
#endif 	/* __ISchedule_FWD_DEFINED__ */


#ifndef __IHosts_FWD_DEFINED__
#define __IHosts_FWD_DEFINED__
typedef interface IHosts IHosts;
#endif 	/* __IHosts_FWD_DEFINED__ */


#ifndef __IHost_FWD_DEFINED__
#define __IHost_FWD_DEFINED__
typedef interface IHost IHost;
#endif 	/* __IHost_FWD_DEFINED__ */


#ifndef __IHandlers_FWD_DEFINED__
#define __IHandlers_FWD_DEFINED__
typedef interface IHandlers IHandlers;
#endif 	/* __IHandlers_FWD_DEFINED__ */


#ifndef __IHandler_FWD_DEFINED__
#define __IHandler_FWD_DEFINED__
typedef interface IHandler IHandler;
#endif 	/* __IHandler_FWD_DEFINED__ */


#ifndef __IBoundFolders_FWD_DEFINED__
#define __IBoundFolders_FWD_DEFINED__
typedef interface IBoundFolders IBoundFolders;
#endif 	/* __IBoundFolders_FWD_DEFINED__ */


#ifndef __IBoundFolder_FWD_DEFINED__
#define __IBoundFolder_FWD_DEFINED__
typedef interface IBoundFolder IBoundFolder;
#endif 	/* __IBoundFolder_FWD_DEFINED__ */


#ifndef __IEventBindings_FWD_DEFINED__
#define __IEventBindings_FWD_DEFINED__
typedef interface IEventBindings IEventBindings;
#endif 	/* __IEventBindings_FWD_DEFINED__ */


#ifndef __IEventBinding_FWD_DEFINED__
#define __IEventBinding_FWD_DEFINED__
typedef interface IEventBinding IEventBinding;
#endif 	/* __IEventBinding_FWD_DEFINED__ */


#ifndef __ISchedule_FWD_DEFINED__
#define __ISchedule_FWD_DEFINED__
typedef interface ISchedule ISchedule;
#endif 	/* __ISchedule_FWD_DEFINED__ */


#ifndef __Events_FWD_DEFINED__
#define __Events_FWD_DEFINED__

#ifdef __cplusplus
typedef class Events Events;
#else
typedef struct Events Events;
#endif /* __cplusplus */

#endif 	/* __Events_FWD_DEFINED__ */


/* header files for imported files */
#include "oaidl.h"
#include "ocidl.h"

void __RPC_FAR * __RPC_USER MIDL_user_allocate(size_t);
void __RPC_USER MIDL_user_free( void __RPC_FAR * ); 

#ifndef __IEvents_INTERFACE_DEFINED__
#define __IEvents_INTERFACE_DEFINED__

/****************************************
 * Generated header for interface: IEvents
 * at Fri Mar 27 05:37:32 1998
 * using MIDL 3.01.75
 ****************************************/
/* [unique][dual][oleautomation][helpstring][uuid][object] */ 



EXTERN_C const IID IID_IEvents;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    interface DECLSPEC_UUID("2F42C690-C6A4-11D0-93E9-00AA0064D470")
    IEvents : public IDispatch
    {
    public:
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Session( 
            /* [retval][out] */ LPUNKNOWN __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_Session( 
            /* [in] */ VARIANT newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Hosts( 
            /* [retval][out] */ VARIANT __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Options( 
            /* [retval][out] */ ULONG __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_Options( 
            /* [in] */ ULONG newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_BoundFolder( 
            /* [in] */ VARIANT varFolder,
            /* [in] */ BOOL fCreate,
            /* [retval][out] */ VARIANT __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_PublicMDB( 
            /* [retval][out] */ LPUNKNOWN __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_EventBinding( 
            /* [in] */ BSTR strID,
            /* [retval][out] */ VARIANT __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE MoveBoundFolder( 
            /* [in] */ BSTR bstrHostName,
            /* [in] */ VARIANT varBoundFolder) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IEventsVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IEvents __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IEvents __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IEvents __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            IEvents __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            IEvents __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            IEvents __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            IEvents __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Session )( 
            IEvents __RPC_FAR * This,
            /* [retval][out] */ LPUNKNOWN __RPC_FAR *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Session )( 
            IEvents __RPC_FAR * This,
            /* [in] */ VARIANT newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Hosts )( 
            IEvents __RPC_FAR * This,
            /* [retval][out] */ VARIANT __RPC_FAR *pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Options )( 
            IEvents __RPC_FAR * This,
            /* [retval][out] */ ULONG __RPC_FAR *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Options )( 
            IEvents __RPC_FAR * This,
            /* [in] */ ULONG newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_BoundFolder )( 
            IEvents __RPC_FAR * This,
            /* [in] */ VARIANT varFolder,
            /* [in] */ BOOL fCreate,
            /* [retval][out] */ VARIANT __RPC_FAR *pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_PublicMDB )( 
            IEvents __RPC_FAR * This,
            /* [retval][out] */ LPUNKNOWN __RPC_FAR *pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_EventBinding )( 
            IEvents __RPC_FAR * This,
            /* [in] */ BSTR strID,
            /* [retval][out] */ VARIANT __RPC_FAR *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *MoveBoundFolder )( 
            IEvents __RPC_FAR * This,
            /* [in] */ BSTR bstrHostName,
            /* [in] */ VARIANT varBoundFolder);
        
        END_INTERFACE
    } IEventsVtbl;

    interface IEvents
    {
        CONST_VTBL struct IEventsVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IEvents_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IEvents_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IEvents_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IEvents_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IEvents_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IEvents_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IEvents_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IEvents_get_Session(This,pVal)	\
    (This)->lpVtbl -> get_Session(This,pVal)

#define IEvents_put_Session(This,newVal)	\
    (This)->lpVtbl -> put_Session(This,newVal)

#define IEvents_get_Hosts(This,pVal)	\
    (This)->lpVtbl -> get_Hosts(This,pVal)

#define IEvents_get_Options(This,pVal)	\
    (This)->lpVtbl -> get_Options(This,pVal)

#define IEvents_put_Options(This,newVal)	\
    (This)->lpVtbl -> put_Options(This,newVal)

#define IEvents_get_BoundFolder(This,varFolder,fCreate,pVal)	\
    (This)->lpVtbl -> get_BoundFolder(This,varFolder,fCreate,pVal)

#define IEvents_get_PublicMDB(This,pVal)	\
    (This)->lpVtbl -> get_PublicMDB(This,pVal)

#define IEvents_get_EventBinding(This,strID,pVal)	\
    (This)->lpVtbl -> get_EventBinding(This,strID,pVal)

#define IEvents_MoveBoundFolder(This,bstrHostName,varBoundFolder)	\
    (This)->lpVtbl -> MoveBoundFolder(This,bstrHostName,varBoundFolder)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IEvents_get_Session_Proxy( 
    IEvents __RPC_FAR * This,
    /* [retval][out] */ LPUNKNOWN __RPC_FAR *pVal);


void __RPC_STUB IEvents_get_Session_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE IEvents_put_Session_Proxy( 
    IEvents __RPC_FAR * This,
    /* [in] */ VARIANT newVal);


void __RPC_STUB IEvents_put_Session_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IEvents_get_Hosts_Proxy( 
    IEvents __RPC_FAR * This,
    /* [retval][out] */ VARIANT __RPC_FAR *pVal);


void __RPC_STUB IEvents_get_Hosts_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IEvents_get_Options_Proxy( 
    IEvents __RPC_FAR * This,
    /* [retval][out] */ ULONG __RPC_FAR *pVal);


void __RPC_STUB IEvents_get_Options_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE IEvents_put_Options_Proxy( 
    IEvents __RPC_FAR * This,
    /* [in] */ ULONG newVal);


void __RPC_STUB IEvents_put_Options_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IEvents_get_BoundFolder_Proxy( 
    IEvents __RPC_FAR * This,
    /* [in] */ VARIANT varFolder,
    /* [in] */ BOOL fCreate,
    /* [retval][out] */ VARIANT __RPC_FAR *pVal);


void __RPC_STUB IEvents_get_BoundFolder_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IEvents_get_PublicMDB_Proxy( 
    IEvents __RPC_FAR * This,
    /* [retval][out] */ LPUNKNOWN __RPC_FAR *pVal);


void __RPC_STUB IEvents_get_PublicMDB_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IEvents_get_EventBinding_Proxy( 
    IEvents __RPC_FAR * This,
    /* [in] */ BSTR strID,
    /* [retval][out] */ VARIANT __RPC_FAR *pVal);


void __RPC_STUB IEvents_get_EventBinding_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IEvents_MoveBoundFolder_Proxy( 
    IEvents __RPC_FAR * This,
    /* [in] */ BSTR bstrHostName,
    /* [in] */ VARIANT varBoundFolder);


void __RPC_STUB IEvents_MoveBoundFolder_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IEvents_INTERFACE_DEFINED__ */


#ifndef __IHosts_INTERFACE_DEFINED__
#define __IHosts_INTERFACE_DEFINED__

/****************************************
 * Generated header for interface: IHosts
 * at Fri Mar 27 05:37:32 1998
 * using MIDL 3.01.75
 ****************************************/
/* [unique][dual][oleautomation][helpstring][uuid][object] */ 



EXTERN_C const IID IID_IHosts;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    interface DECLSPEC_UUID("2F42C69A-C6A4-11D0-93E9-00AA0064D470")
    IHosts : public IDispatch
    {
    public:
        virtual /* [propget][id] */ HRESULT STDMETHODCALLTYPE get_Count( 
            /* [retval][out] */ long __RPC_FAR *pVal) = 0;
        
        virtual /* [propget][id] */ HRESULT STDMETHODCALLTYPE get_Item( 
            /* [in] */ VARIANT varIndex,
            /* [retval][out] */ VARIANT __RPC_FAR *retval) = 0;
        
        virtual /* [restricted][propget][id] */ HRESULT STDMETHODCALLTYPE get__NewEnum( 
            /* [retval][out] */ IUnknown __RPC_FAR *__RPC_FAR *retval) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IHostsVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IHosts __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IHosts __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IHosts __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            IHosts __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            IHosts __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            IHosts __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            IHosts __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Count )( 
            IHosts __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *pVal);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Item )( 
            IHosts __RPC_FAR * This,
            /* [in] */ VARIANT varIndex,
            /* [retval][out] */ VARIANT __RPC_FAR *retval);
        
        /* [restricted][propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get__NewEnum )( 
            IHosts __RPC_FAR * This,
            /* [retval][out] */ IUnknown __RPC_FAR *__RPC_FAR *retval);
        
        END_INTERFACE
    } IHostsVtbl;

    interface IHosts
    {
        CONST_VTBL struct IHostsVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IHosts_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IHosts_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IHosts_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IHosts_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IHosts_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IHosts_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IHosts_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IHosts_get_Count(This,pVal)	\
    (This)->lpVtbl -> get_Count(This,pVal)

#define IHosts_get_Item(This,varIndex,retval)	\
    (This)->lpVtbl -> get_Item(This,varIndex,retval)

#define IHosts_get__NewEnum(This,retval)	\
    (This)->lpVtbl -> get__NewEnum(This,retval)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [propget][id] */ HRESULT STDMETHODCALLTYPE IHosts_get_Count_Proxy( 
    IHosts __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *pVal);


void __RPC_STUB IHosts_get_Count_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget][id] */ HRESULT STDMETHODCALLTYPE IHosts_get_Item_Proxy( 
    IHosts __RPC_FAR * This,
    /* [in] */ VARIANT varIndex,
    /* [retval][out] */ VARIANT __RPC_FAR *retval);


void __RPC_STUB IHosts_get_Item_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [restricted][propget][id] */ HRESULT STDMETHODCALLTYPE IHosts_get__NewEnum_Proxy( 
    IHosts __RPC_FAR * This,
    /* [retval][out] */ IUnknown __RPC_FAR *__RPC_FAR *retval);


void __RPC_STUB IHosts_get__NewEnum_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IHosts_INTERFACE_DEFINED__ */


#ifndef __IHost_INTERFACE_DEFINED__
#define __IHost_INTERFACE_DEFINED__

/****************************************
 * Generated header for interface: IHost
 * at Fri Mar 27 05:37:32 1998
 * using MIDL 3.01.75
 ****************************************/
/* [unique][dual][oleautomation][helpstring][uuid][object] */ 



EXTERN_C const IID IID_IHost;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    interface DECLSPEC_UUID("4AA3F8E1-C6AF-11D0-93E9-00AA0064D470")
    IHost : public IDispatch
    {
    public:
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Name( 
            /* [retval][out] */ BSTR __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Handlers( 
            /* [retval][out] */ VARIANT __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_BoundFolders( 
            /* [retval][out] */ VARIANT __RPC_FAR *pVal) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IHostVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IHost __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IHost __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IHost __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            IHost __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            IHost __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            IHost __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            IHost __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Name )( 
            IHost __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Handlers )( 
            IHost __RPC_FAR * This,
            /* [retval][out] */ VARIANT __RPC_FAR *pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_BoundFolders )( 
            IHost __RPC_FAR * This,
            /* [retval][out] */ VARIANT __RPC_FAR *pVal);
        
        END_INTERFACE
    } IHostVtbl;

    interface IHost
    {
        CONST_VTBL struct IHostVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IHost_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IHost_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IHost_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IHost_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IHost_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IHost_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IHost_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IHost_get_Name(This,pVal)	\
    (This)->lpVtbl -> get_Name(This,pVal)

#define IHost_get_Handlers(This,pVal)	\
    (This)->lpVtbl -> get_Handlers(This,pVal)

#define IHost_get_BoundFolders(This,pVal)	\
    (This)->lpVtbl -> get_BoundFolders(This,pVal)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IHost_get_Name_Proxy( 
    IHost __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pVal);


void __RPC_STUB IHost_get_Name_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IHost_get_Handlers_Proxy( 
    IHost __RPC_FAR * This,
    /* [retval][out] */ VARIANT __RPC_FAR *pVal);


void __RPC_STUB IHost_get_Handlers_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IHost_get_BoundFolders_Proxy( 
    IHost __RPC_FAR * This,
    /* [retval][out] */ VARIANT __RPC_FAR *pVal);


void __RPC_STUB IHost_get_BoundFolders_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IHost_INTERFACE_DEFINED__ */


#ifndef __IHandlers_INTERFACE_DEFINED__
#define __IHandlers_INTERFACE_DEFINED__

/****************************************
 * Generated header for interface: IHandlers
 * at Fri Mar 27 05:37:32 1998
 * using MIDL 3.01.75
 ****************************************/
/* [unique][dual][oleautomation][helpstring][uuid][object] */ 



EXTERN_C const IID IID_IHandlers;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    interface DECLSPEC_UUID("2F42C69D-C6A4-11D0-93E9-00AA0064D470")
    IHandlers : public IDispatch
    {
    public:
        virtual /* [propget][id] */ HRESULT STDMETHODCALLTYPE get_Count( 
            /* [retval][out] */ long __RPC_FAR *pVal) = 0;
        
        virtual /* [propget][id] */ HRESULT STDMETHODCALLTYPE get_Item( 
            /* [in] */ VARIANT varIndex,
            /* [retval][out] */ VARIANT __RPC_FAR *retval) = 0;
        
        virtual /* [restricted][propget][id] */ HRESULT STDMETHODCALLTYPE get__NewEnum( 
            /* [retval][out] */ IUnknown __RPC_FAR *__RPC_FAR *retval) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Add( 
            /* [in] */ BSTR Name,
            /* [in] */ BSTR bstrCLSID,
            /* [retval][out] */ VARIANT __RPC_FAR *pvarNew) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Delete( 
            /* [in] */ BSTR bstrCLSID) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IHandlersVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IHandlers __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IHandlers __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IHandlers __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            IHandlers __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            IHandlers __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            IHandlers __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            IHandlers __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Count )( 
            IHandlers __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *pVal);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Item )( 
            IHandlers __RPC_FAR * This,
            /* [in] */ VARIANT varIndex,
            /* [retval][out] */ VARIANT __RPC_FAR *retval);
        
        /* [restricted][propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get__NewEnum )( 
            IHandlers __RPC_FAR * This,
            /* [retval][out] */ IUnknown __RPC_FAR *__RPC_FAR *retval);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Add )( 
            IHandlers __RPC_FAR * This,
            /* [in] */ BSTR Name,
            /* [in] */ BSTR bstrCLSID,
            /* [retval][out] */ VARIANT __RPC_FAR *pvarNew);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Delete )( 
            IHandlers __RPC_FAR * This,
            /* [in] */ BSTR bstrCLSID);
        
        END_INTERFACE
    } IHandlersVtbl;

    interface IHandlers
    {
        CONST_VTBL struct IHandlersVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IHandlers_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IHandlers_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IHandlers_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IHandlers_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IHandlers_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IHandlers_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IHandlers_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IHandlers_get_Count(This,pVal)	\
    (This)->lpVtbl -> get_Count(This,pVal)

#define IHandlers_get_Item(This,varIndex,retval)	\
    (This)->lpVtbl -> get_Item(This,varIndex,retval)

#define IHandlers_get__NewEnum(This,retval)	\
    (This)->lpVtbl -> get__NewEnum(This,retval)

#define IHandlers_Add(This,Name,bstrCLSID,pvarNew)	\
    (This)->lpVtbl -> Add(This,Name,bstrCLSID,pvarNew)

#define IHandlers_Delete(This,bstrCLSID)	\
    (This)->lpVtbl -> Delete(This,bstrCLSID)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [propget][id] */ HRESULT STDMETHODCALLTYPE IHandlers_get_Count_Proxy( 
    IHandlers __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *pVal);


void __RPC_STUB IHandlers_get_Count_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget][id] */ HRESULT STDMETHODCALLTYPE IHandlers_get_Item_Proxy( 
    IHandlers __RPC_FAR * This,
    /* [in] */ VARIANT varIndex,
    /* [retval][out] */ VARIANT __RPC_FAR *retval);


void __RPC_STUB IHandlers_get_Item_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [restricted][propget][id] */ HRESULT STDMETHODCALLTYPE IHandlers_get__NewEnum_Proxy( 
    IHandlers __RPC_FAR * This,
    /* [retval][out] */ IUnknown __RPC_FAR *__RPC_FAR *retval);


void __RPC_STUB IHandlers_get__NewEnum_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IHandlers_Add_Proxy( 
    IHandlers __RPC_FAR * This,
    /* [in] */ BSTR Name,
    /* [in] */ BSTR bstrCLSID,
    /* [retval][out] */ VARIANT __RPC_FAR *pvarNew);


void __RPC_STUB IHandlers_Add_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IHandlers_Delete_Proxy( 
    IHandlers __RPC_FAR * This,
    /* [in] */ BSTR bstrCLSID);


void __RPC_STUB IHandlers_Delete_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IHandlers_INTERFACE_DEFINED__ */


#ifndef __IHandler_INTERFACE_DEFINED__
#define __IHandler_INTERFACE_DEFINED__

/****************************************
 * Generated header for interface: IHandler
 * at Fri Mar 27 05:37:32 1998
 * using MIDL 3.01.75
 ****************************************/
/* [unique][dual][oleautomation][helpstring][uuid][object] */ 



EXTERN_C const IID IID_IHandler;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    interface DECLSPEC_UUID("4AA3F8E4-C6AF-11D0-93E9-00AA0064D470")
    IHandler : public IDispatch
    {
    public:
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Name( 
            /* [retval][out] */ BSTR __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_ClassID( 
            /* [retval][out] */ BSTR __RPC_FAR *pVal) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IHandlerVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IHandler __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IHandler __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IHandler __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            IHandler __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            IHandler __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            IHandler __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            IHandler __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Name )( 
            IHandler __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_ClassID )( 
            IHandler __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pVal);
        
        END_INTERFACE
    } IHandlerVtbl;

    interface IHandler
    {
        CONST_VTBL struct IHandlerVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IHandler_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IHandler_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IHandler_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IHandler_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IHandler_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IHandler_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IHandler_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IHandler_get_Name(This,pVal)	\
    (This)->lpVtbl -> get_Name(This,pVal)

#define IHandler_get_ClassID(This,pVal)	\
    (This)->lpVtbl -> get_ClassID(This,pVal)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IHandler_get_Name_Proxy( 
    IHandler __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pVal);


void __RPC_STUB IHandler_get_Name_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IHandler_get_ClassID_Proxy( 
    IHandler __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pVal);


void __RPC_STUB IHandler_get_ClassID_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IHandler_INTERFACE_DEFINED__ */


#ifndef __IBoundFolders_INTERFACE_DEFINED__
#define __IBoundFolders_INTERFACE_DEFINED__

/****************************************
 * Generated header for interface: IBoundFolders
 * at Fri Mar 27 05:37:32 1998
 * using MIDL 3.01.75
 ****************************************/
/* [unique][dual][oleautomation][helpstring][uuid][object] */ 



EXTERN_C const IID IID_IBoundFolders;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    interface DECLSPEC_UUID("B0C0F075-DEA0-11d0-84B0-00C04FC2F8DA")
    IBoundFolders : public IDispatch
    {
    public:
        virtual /* [propget][id] */ HRESULT STDMETHODCALLTYPE get_Count( 
            /* [retval][out] */ long __RPC_FAR *pVal) = 0;
        
        virtual /* [propget][id] */ HRESULT STDMETHODCALLTYPE get_Item( 
            /* [in] */ VARIANT varIndex,
            /* [retval][out] */ VARIANT __RPC_FAR *retval) = 0;
        
        virtual /* [restricted][propget][id] */ HRESULT STDMETHODCALLTYPE get__NewEnum( 
            /* [retval][out] */ IUnknown __RPC_FAR *__RPC_FAR *retval) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IBoundFoldersVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IBoundFolders __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IBoundFolders __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IBoundFolders __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            IBoundFolders __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            IBoundFolders __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            IBoundFolders __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            IBoundFolders __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Count )( 
            IBoundFolders __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *pVal);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Item )( 
            IBoundFolders __RPC_FAR * This,
            /* [in] */ VARIANT varIndex,
            /* [retval][out] */ VARIANT __RPC_FAR *retval);
        
        /* [restricted][propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get__NewEnum )( 
            IBoundFolders __RPC_FAR * This,
            /* [retval][out] */ IUnknown __RPC_FAR *__RPC_FAR *retval);
        
        END_INTERFACE
    } IBoundFoldersVtbl;

    interface IBoundFolders
    {
        CONST_VTBL struct IBoundFoldersVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IBoundFolders_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IBoundFolders_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IBoundFolders_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IBoundFolders_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IBoundFolders_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IBoundFolders_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IBoundFolders_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IBoundFolders_get_Count(This,pVal)	\
    (This)->lpVtbl -> get_Count(This,pVal)

#define IBoundFolders_get_Item(This,varIndex,retval)	\
    (This)->lpVtbl -> get_Item(This,varIndex,retval)

#define IBoundFolders_get__NewEnum(This,retval)	\
    (This)->lpVtbl -> get__NewEnum(This,retval)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [propget][id] */ HRESULT STDMETHODCALLTYPE IBoundFolders_get_Count_Proxy( 
    IBoundFolders __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *pVal);


void __RPC_STUB IBoundFolders_get_Count_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget][id] */ HRESULT STDMETHODCALLTYPE IBoundFolders_get_Item_Proxy( 
    IBoundFolders __RPC_FAR * This,
    /* [in] */ VARIANT varIndex,
    /* [retval][out] */ VARIANT __RPC_FAR *retval);


void __RPC_STUB IBoundFolders_get_Item_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [restricted][propget][id] */ HRESULT STDMETHODCALLTYPE IBoundFolders_get__NewEnum_Proxy( 
    IBoundFolders __RPC_FAR * This,
    /* [retval][out] */ IUnknown __RPC_FAR *__RPC_FAR *retval);


void __RPC_STUB IBoundFolders_get__NewEnum_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IBoundFolders_INTERFACE_DEFINED__ */


#ifndef __IBoundFolder_INTERFACE_DEFINED__
#define __IBoundFolder_INTERFACE_DEFINED__

/****************************************
 * Generated header for interface: IBoundFolder
 * at Fri Mar 27 05:37:32 1998
 * using MIDL 3.01.75
 ****************************************/
/* [unique][dual][oleautomation][helpstring][uuid][object] */ 



EXTERN_C const IID IID_IBoundFolder;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    interface DECLSPEC_UUID("B0C0F074-DEA0-11d0-84B0-00C04FC2F8DA")
    IBoundFolder : public IDispatch
    {
    public:
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_HostName( 
            /* [retval][out] */ BSTR __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_EntryID( 
            /* [retval][out] */ BSTR __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_MDBEntryID( 
            /* [retval][out] */ BSTR __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Bindings( 
            /* [retval][out] */ VARIANT __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_EventMask( 
            /* [retval][out] */ ULONG __RPC_FAR *pl) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Access( 
            /* [retval][out] */ ULONG __RPC_FAR *pl) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_ScheduleCount( 
            /* [retval][out] */ ULONG __RPC_FAR *pl) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SaveChanges( void) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_EntryID2( 
            /* [retval][out] */ BSTR __RPC_FAR *pVal) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IBoundFolderVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IBoundFolder __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IBoundFolder __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IBoundFolder __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            IBoundFolder __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            IBoundFolder __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            IBoundFolder __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            IBoundFolder __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_HostName )( 
            IBoundFolder __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_EntryID )( 
            IBoundFolder __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_MDBEntryID )( 
            IBoundFolder __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Bindings )( 
            IBoundFolder __RPC_FAR * This,
            /* [retval][out] */ VARIANT __RPC_FAR *pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_EventMask )( 
            IBoundFolder __RPC_FAR * This,
            /* [retval][out] */ ULONG __RPC_FAR *pl);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Access )( 
            IBoundFolder __RPC_FAR * This,
            /* [retval][out] */ ULONG __RPC_FAR *pl);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_ScheduleCount )( 
            IBoundFolder __RPC_FAR * This,
            /* [retval][out] */ ULONG __RPC_FAR *pl);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *SaveChanges )( 
            IBoundFolder __RPC_FAR * This);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_EntryID2 )( 
            IBoundFolder __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pVal);
        
        END_INTERFACE
    } IBoundFolderVtbl;

    interface IBoundFolder
    {
        CONST_VTBL struct IBoundFolderVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IBoundFolder_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IBoundFolder_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IBoundFolder_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IBoundFolder_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IBoundFolder_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IBoundFolder_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IBoundFolder_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IBoundFolder_get_HostName(This,pVal)	\
    (This)->lpVtbl -> get_HostName(This,pVal)

#define IBoundFolder_get_EntryID(This,pVal)	\
    (This)->lpVtbl -> get_EntryID(This,pVal)

#define IBoundFolder_get_MDBEntryID(This,pVal)	\
    (This)->lpVtbl -> get_MDBEntryID(This,pVal)

#define IBoundFolder_get_Bindings(This,pVal)	\
    (This)->lpVtbl -> get_Bindings(This,pVal)

#define IBoundFolder_get_EventMask(This,pl)	\
    (This)->lpVtbl -> get_EventMask(This,pl)

#define IBoundFolder_get_Access(This,pl)	\
    (This)->lpVtbl -> get_Access(This,pl)

#define IBoundFolder_get_ScheduleCount(This,pl)	\
    (This)->lpVtbl -> get_ScheduleCount(This,pl)

#define IBoundFolder_SaveChanges(This)	\
    (This)->lpVtbl -> SaveChanges(This)

#define IBoundFolder_get_EntryID2(This,pVal)	\
    (This)->lpVtbl -> get_EntryID2(This,pVal)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IBoundFolder_get_HostName_Proxy( 
    IBoundFolder __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pVal);


void __RPC_STUB IBoundFolder_get_HostName_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IBoundFolder_get_EntryID_Proxy( 
    IBoundFolder __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pVal);


void __RPC_STUB IBoundFolder_get_EntryID_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IBoundFolder_get_MDBEntryID_Proxy( 
    IBoundFolder __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pVal);


void __RPC_STUB IBoundFolder_get_MDBEntryID_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IBoundFolder_get_Bindings_Proxy( 
    IBoundFolder __RPC_FAR * This,
    /* [retval][out] */ VARIANT __RPC_FAR *pVal);


void __RPC_STUB IBoundFolder_get_Bindings_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IBoundFolder_get_EventMask_Proxy( 
    IBoundFolder __RPC_FAR * This,
    /* [retval][out] */ ULONG __RPC_FAR *pl);


void __RPC_STUB IBoundFolder_get_EventMask_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IBoundFolder_get_Access_Proxy( 
    IBoundFolder __RPC_FAR * This,
    /* [retval][out] */ ULONG __RPC_FAR *pl);


void __RPC_STUB IBoundFolder_get_Access_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IBoundFolder_get_ScheduleCount_Proxy( 
    IBoundFolder __RPC_FAR * This,
    /* [retval][out] */ ULONG __RPC_FAR *pl);


void __RPC_STUB IBoundFolder_get_ScheduleCount_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IBoundFolder_SaveChanges_Proxy( 
    IBoundFolder __RPC_FAR * This);


void __RPC_STUB IBoundFolder_SaveChanges_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IBoundFolder_get_EntryID2_Proxy( 
    IBoundFolder __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pVal);


void __RPC_STUB IBoundFolder_get_EntryID2_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IBoundFolder_INTERFACE_DEFINED__ */


#ifndef __IEventBindings_INTERFACE_DEFINED__
#define __IEventBindings_INTERFACE_DEFINED__

/****************************************
 * Generated header for interface: IEventBindings
 * at Fri Mar 27 05:37:32 1998
 * using MIDL 3.01.75
 ****************************************/
/* [unique][dual][oleautomation][helpstring][uuid][object] */ 



EXTERN_C const IID IID_IEventBindings;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    interface DECLSPEC_UUID("2F42C694-C6A4-11D0-93E9-00AA0064D470")
    IEventBindings : public IDispatch
    {
    public:
        virtual /* [propget][id] */ HRESULT STDMETHODCALLTYPE get_Count( 
            /* [retval][out] */ long __RPC_FAR *pVal) = 0;
        
        virtual /* [propget][id] */ HRESULT STDMETHODCALLTYPE get_Item( 
            /* [in] */ VARIANT varIndex,
            /* [retval][out] */ VARIANT __RPC_FAR *retval) = 0;
        
        virtual /* [restricted][propget][id] */ HRESULT STDMETHODCALLTYPE get__NewEnum( 
            /* [retval][out] */ IUnknown __RPC_FAR *__RPC_FAR *retval) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Add( 
            /* [retval][out] */ VARIANT __RPC_FAR *pvarNewBinding) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Delete( 
            /* [in] */ VARIANT varItem) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Clone( 
            /* [in] */ VARIANT varItem,
            /* [retval][out] */ VARIANT __RPC_FAR *varNewCopy) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IEventBindingsVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IEventBindings __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IEventBindings __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IEventBindings __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            IEventBindings __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            IEventBindings __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            IEventBindings __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            IEventBindings __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Count )( 
            IEventBindings __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *pVal);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Item )( 
            IEventBindings __RPC_FAR * This,
            /* [in] */ VARIANT varIndex,
            /* [retval][out] */ VARIANT __RPC_FAR *retval);
        
        /* [restricted][propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get__NewEnum )( 
            IEventBindings __RPC_FAR * This,
            /* [retval][out] */ IUnknown __RPC_FAR *__RPC_FAR *retval);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Add )( 
            IEventBindings __RPC_FAR * This,
            /* [retval][out] */ VARIANT __RPC_FAR *pvarNewBinding);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Delete )( 
            IEventBindings __RPC_FAR * This,
            /* [in] */ VARIANT varItem);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Clone )( 
            IEventBindings __RPC_FAR * This,
            /* [in] */ VARIANT varItem,
            /* [retval][out] */ VARIANT __RPC_FAR *varNewCopy);
        
        END_INTERFACE
    } IEventBindingsVtbl;

    interface IEventBindings
    {
        CONST_VTBL struct IEventBindingsVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IEventBindings_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IEventBindings_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IEventBindings_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IEventBindings_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IEventBindings_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IEventBindings_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IEventBindings_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IEventBindings_get_Count(This,pVal)	\
    (This)->lpVtbl -> get_Count(This,pVal)

#define IEventBindings_get_Item(This,varIndex,retval)	\
    (This)->lpVtbl -> get_Item(This,varIndex,retval)

#define IEventBindings_get__NewEnum(This,retval)	\
    (This)->lpVtbl -> get__NewEnum(This,retval)

#define IEventBindings_Add(This,pvarNewBinding)	\
    (This)->lpVtbl -> Add(This,pvarNewBinding)

#define IEventBindings_Delete(This,varItem)	\
    (This)->lpVtbl -> Delete(This,varItem)

#define IEventBindings_Clone(This,varItem,varNewCopy)	\
    (This)->lpVtbl -> Clone(This,varItem,varNewCopy)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [propget][id] */ HRESULT STDMETHODCALLTYPE IEventBindings_get_Count_Proxy( 
    IEventBindings __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *pVal);


void __RPC_STUB IEventBindings_get_Count_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget][id] */ HRESULT STDMETHODCALLTYPE IEventBindings_get_Item_Proxy( 
    IEventBindings __RPC_FAR * This,
    /* [in] */ VARIANT varIndex,
    /* [retval][out] */ VARIANT __RPC_FAR *retval);


void __RPC_STUB IEventBindings_get_Item_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [restricted][propget][id] */ HRESULT STDMETHODCALLTYPE IEventBindings_get__NewEnum_Proxy( 
    IEventBindings __RPC_FAR * This,
    /* [retval][out] */ IUnknown __RPC_FAR *__RPC_FAR *retval);


void __RPC_STUB IEventBindings_get__NewEnum_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IEventBindings_Add_Proxy( 
    IEventBindings __RPC_FAR * This,
    /* [retval][out] */ VARIANT __RPC_FAR *pvarNewBinding);


void __RPC_STUB IEventBindings_Add_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IEventBindings_Delete_Proxy( 
    IEventBindings __RPC_FAR * This,
    /* [in] */ VARIANT varItem);


void __RPC_STUB IEventBindings_Delete_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IEventBindings_Clone_Proxy( 
    IEventBindings __RPC_FAR * This,
    /* [in] */ VARIANT varItem,
    /* [retval][out] */ VARIANT __RPC_FAR *varNewCopy);


void __RPC_STUB IEventBindings_Clone_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IEventBindings_INTERFACE_DEFINED__ */


#ifndef __IEventBinding_INTERFACE_DEFINED__
#define __IEventBinding_INTERFACE_DEFINED__

/****************************************
 * Generated header for interface: IEventBinding
 * at Fri Mar 27 05:37:32 1998
 * using MIDL 3.01.75
 ****************************************/
/* [unique][dual][oleautomation][helpstring][uuid][object] */ 



EXTERN_C const IID IID_IEventBinding;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    interface DECLSPEC_UUID("2F42C697-C6A4-11D0-93E9-00AA0064D470")
    IEventBinding : public IDispatch
    {
    public:
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_HandlerClassID( 
            /* [retval][out] */ BSTR __RPC_FAR *pbstrVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_HandlerClassID( 
            /* [in] */ BSTR bstrVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_Handler( 
            /* [in] */ VARIANT newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Name( 
            /* [retval][out] */ BSTR __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_Name( 
            /* [in] */ BSTR newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Priority( 
            /* [retval][out] */ ULONG __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_Priority( 
            /* [in] */ ULONG newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Active( 
            /* [retval][out] */ BOOL __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_Active( 
            /* [in] */ BOOL newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_CustomProperty( 
            /* [in] */ VARIANT varPropId,
            /* [retval][out] */ VARIANT __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_CustomProperty( 
            /* [in] */ VARIANT varPropId,
            /* [in] */ VARIANT newVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SaveChanges( void) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_EventMask( 
            /* [retval][out] */ ULONG __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_EventMask( 
            /* [in] */ ULONG newVal) = 0;
        
        virtual /* [helpstring][restricted][hidden][id][propget] */ HRESULT STDMETHODCALLTYPE get_MAPIOBJECT( 
            /* [retval][out] */ VARIANT __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Schedule( 
            /* [retval][out] */ VARIANT __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_EntryID( 
            /* [retval][out] */ BSTR __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_CreationTime( 
            /* [retval][out] */ VARIANT __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Access( 
            /* [retval][out] */ ULONG __RPC_FAR *plAccess) = 0;
        
        virtual /* [helpstring][restricted][hidden][id][propget] */ HRESULT STDMETHODCALLTYPE get_MAPIOBJECT2( 
            /* [retval][out] */ VARIANT __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SaveCustomChanges( 
            /* [in] */ VARIANT varMAPIOBJECT) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_EntryID2( 
            /* [retval][out] */ BSTR __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_EntryID3( 
            /* [retval][out] */ BSTR __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][restricted][hidden][id][propget] */ HRESULT STDMETHODCALLTYPE get_MAPIOBJECT3( 
            /* [retval][out] */ VARIANT __RPC_FAR *pVal) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IEventBindingVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IEventBinding __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IEventBinding __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IEventBinding __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            IEventBinding __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            IEventBinding __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            IEventBinding __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            IEventBinding __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_HandlerClassID )( 
            IEventBinding __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pbstrVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_HandlerClassID )( 
            IEventBinding __RPC_FAR * This,
            /* [in] */ BSTR bstrVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Handler )( 
            IEventBinding __RPC_FAR * This,
            /* [in] */ VARIANT newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Name )( 
            IEventBinding __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Name )( 
            IEventBinding __RPC_FAR * This,
            /* [in] */ BSTR newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Priority )( 
            IEventBinding __RPC_FAR * This,
            /* [retval][out] */ ULONG __RPC_FAR *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Priority )( 
            IEventBinding __RPC_FAR * This,
            /* [in] */ ULONG newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Active )( 
            IEventBinding __RPC_FAR * This,
            /* [retval][out] */ BOOL __RPC_FAR *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Active )( 
            IEventBinding __RPC_FAR * This,
            /* [in] */ BOOL newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_CustomProperty )( 
            IEventBinding __RPC_FAR * This,
            /* [in] */ VARIANT varPropId,
            /* [retval][out] */ VARIANT __RPC_FAR *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_CustomProperty )( 
            IEventBinding __RPC_FAR * This,
            /* [in] */ VARIANT varPropId,
            /* [in] */ VARIANT newVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *SaveChanges )( 
            IEventBinding __RPC_FAR * This);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_EventMask )( 
            IEventBinding __RPC_FAR * This,
            /* [retval][out] */ ULONG __RPC_FAR *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_EventMask )( 
            IEventBinding __RPC_FAR * This,
            /* [in] */ ULONG newVal);
        
        /* [helpstring][restricted][hidden][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_MAPIOBJECT )( 
            IEventBinding __RPC_FAR * This,
            /* [retval][out] */ VARIANT __RPC_FAR *pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Schedule )( 
            IEventBinding __RPC_FAR * This,
            /* [retval][out] */ VARIANT __RPC_FAR *pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_EntryID )( 
            IEventBinding __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_CreationTime )( 
            IEventBinding __RPC_FAR * This,
            /* [retval][out] */ VARIANT __RPC_FAR *pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Access )( 
            IEventBinding __RPC_FAR * This,
            /* [retval][out] */ ULONG __RPC_FAR *plAccess);
        
        /* [helpstring][restricted][hidden][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_MAPIOBJECT2 )( 
            IEventBinding __RPC_FAR * This,
            /* [retval][out] */ VARIANT __RPC_FAR *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *SaveCustomChanges )( 
            IEventBinding __RPC_FAR * This,
            /* [in] */ VARIANT varMAPIOBJECT);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_EntryID2 )( 
            IEventBinding __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_EntryID3 )( 
            IEventBinding __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pVal);
        
        /* [helpstring][restricted][hidden][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_MAPIOBJECT3 )( 
            IEventBinding __RPC_FAR * This,
            /* [retval][out] */ VARIANT __RPC_FAR *pVal);
        
        END_INTERFACE
    } IEventBindingVtbl;

    interface IEventBinding
    {
        CONST_VTBL struct IEventBindingVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IEventBinding_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IEventBinding_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IEventBinding_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IEventBinding_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IEventBinding_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IEventBinding_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IEventBinding_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IEventBinding_get_HandlerClassID(This,pbstrVal)	\
    (This)->lpVtbl -> get_HandlerClassID(This,pbstrVal)

#define IEventBinding_put_HandlerClassID(This,bstrVal)	\
    (This)->lpVtbl -> put_HandlerClassID(This,bstrVal)

#define IEventBinding_put_Handler(This,newVal)	\
    (This)->lpVtbl -> put_Handler(This,newVal)

#define IEventBinding_get_Name(This,pVal)	\
    (This)->lpVtbl -> get_Name(This,pVal)

#define IEventBinding_put_Name(This,newVal)	\
    (This)->lpVtbl -> put_Name(This,newVal)

#define IEventBinding_get_Priority(This,pVal)	\
    (This)->lpVtbl -> get_Priority(This,pVal)

#define IEventBinding_put_Priority(This,newVal)	\
    (This)->lpVtbl -> put_Priority(This,newVal)

#define IEventBinding_get_Active(This,pVal)	\
    (This)->lpVtbl -> get_Active(This,pVal)

#define IEventBinding_put_Active(This,newVal)	\
    (This)->lpVtbl -> put_Active(This,newVal)

#define IEventBinding_get_CustomProperty(This,varPropId,pVal)	\
    (This)->lpVtbl -> get_CustomProperty(This,varPropId,pVal)

#define IEventBinding_put_CustomProperty(This,varPropId,newVal)	\
    (This)->lpVtbl -> put_CustomProperty(This,varPropId,newVal)

#define IEventBinding_SaveChanges(This)	\
    (This)->lpVtbl -> SaveChanges(This)

#define IEventBinding_get_EventMask(This,pVal)	\
    (This)->lpVtbl -> get_EventMask(This,pVal)

#define IEventBinding_put_EventMask(This,newVal)	\
    (This)->lpVtbl -> put_EventMask(This,newVal)

#define IEventBinding_get_MAPIOBJECT(This,pVal)	\
    (This)->lpVtbl -> get_MAPIOBJECT(This,pVal)

#define IEventBinding_get_Schedule(This,pVal)	\
    (This)->lpVtbl -> get_Schedule(This,pVal)

#define IEventBinding_get_EntryID(This,pVal)	\
    (This)->lpVtbl -> get_EntryID(This,pVal)

#define IEventBinding_get_CreationTime(This,pVal)	\
    (This)->lpVtbl -> get_CreationTime(This,pVal)

#define IEventBinding_get_Access(This,plAccess)	\
    (This)->lpVtbl -> get_Access(This,plAccess)

#define IEventBinding_get_MAPIOBJECT2(This,pVal)	\
    (This)->lpVtbl -> get_MAPIOBJECT2(This,pVal)

#define IEventBinding_SaveCustomChanges(This,varMAPIOBJECT)	\
    (This)->lpVtbl -> SaveCustomChanges(This,varMAPIOBJECT)

#define IEventBinding_get_EntryID2(This,pVal)	\
    (This)->lpVtbl -> get_EntryID2(This,pVal)

#define IEventBinding_get_EntryID3(This,pVal)	\
    (This)->lpVtbl -> get_EntryID3(This,pVal)

#define IEventBinding_get_MAPIOBJECT3(This,pVal)	\
    (This)->lpVtbl -> get_MAPIOBJECT3(This,pVal)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IEventBinding_get_HandlerClassID_Proxy( 
    IEventBinding __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pbstrVal);


void __RPC_STUB IEventBinding_get_HandlerClassID_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE IEventBinding_put_HandlerClassID_Proxy( 
    IEventBinding __RPC_FAR * This,
    /* [in] */ BSTR bstrVal);


void __RPC_STUB IEventBinding_put_HandlerClassID_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE IEventBinding_put_Handler_Proxy( 
    IEventBinding __RPC_FAR * This,
    /* [in] */ VARIANT newVal);


void __RPC_STUB IEventBinding_put_Handler_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IEventBinding_get_Name_Proxy( 
    IEventBinding __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pVal);


void __RPC_STUB IEventBinding_get_Name_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE IEventBinding_put_Name_Proxy( 
    IEventBinding __RPC_FAR * This,
    /* [in] */ BSTR newVal);


void __RPC_STUB IEventBinding_put_Name_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IEventBinding_get_Priority_Proxy( 
    IEventBinding __RPC_FAR * This,
    /* [retval][out] */ ULONG __RPC_FAR *pVal);


void __RPC_STUB IEventBinding_get_Priority_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE IEventBinding_put_Priority_Proxy( 
    IEventBinding __RPC_FAR * This,
    /* [in] */ ULONG newVal);


void __RPC_STUB IEventBinding_put_Priority_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IEventBinding_get_Active_Proxy( 
    IEventBinding __RPC_FAR * This,
    /* [retval][out] */ BOOL __RPC_FAR *pVal);


void __RPC_STUB IEventBinding_get_Active_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE IEventBinding_put_Active_Proxy( 
    IEventBinding __RPC_FAR * This,
    /* [in] */ BOOL newVal);


void __RPC_STUB IEventBinding_put_Active_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IEventBinding_get_CustomProperty_Proxy( 
    IEventBinding __RPC_FAR * This,
    /* [in] */ VARIANT varPropId,
    /* [retval][out] */ VARIANT __RPC_FAR *pVal);


void __RPC_STUB IEventBinding_get_CustomProperty_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE IEventBinding_put_CustomProperty_Proxy( 
    IEventBinding __RPC_FAR * This,
    /* [in] */ VARIANT varPropId,
    /* [in] */ VARIANT newVal);


void __RPC_STUB IEventBinding_put_CustomProperty_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IEventBinding_SaveChanges_Proxy( 
    IEventBinding __RPC_FAR * This);


void __RPC_STUB IEventBinding_SaveChanges_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IEventBinding_get_EventMask_Proxy( 
    IEventBinding __RPC_FAR * This,
    /* [retval][out] */ ULONG __RPC_FAR *pVal);


void __RPC_STUB IEventBinding_get_EventMask_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE IEventBinding_put_EventMask_Proxy( 
    IEventBinding __RPC_FAR * This,
    /* [in] */ ULONG newVal);


void __RPC_STUB IEventBinding_put_EventMask_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][restricted][hidden][id][propget] */ HRESULT STDMETHODCALLTYPE IEventBinding_get_MAPIOBJECT_Proxy( 
    IEventBinding __RPC_FAR * This,
    /* [retval][out] */ VARIANT __RPC_FAR *pVal);


void __RPC_STUB IEventBinding_get_MAPIOBJECT_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IEventBinding_get_Schedule_Proxy( 
    IEventBinding __RPC_FAR * This,
    /* [retval][out] */ VARIANT __RPC_FAR *pVal);


void __RPC_STUB IEventBinding_get_Schedule_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IEventBinding_get_EntryID_Proxy( 
    IEventBinding __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pVal);


void __RPC_STUB IEventBinding_get_EntryID_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IEventBinding_get_CreationTime_Proxy( 
    IEventBinding __RPC_FAR * This,
    /* [retval][out] */ VARIANT __RPC_FAR *pVal);


void __RPC_STUB IEventBinding_get_CreationTime_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IEventBinding_get_Access_Proxy( 
    IEventBinding __RPC_FAR * This,
    /* [retval][out] */ ULONG __RPC_FAR *plAccess);


void __RPC_STUB IEventBinding_get_Access_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][restricted][hidden][id][propget] */ HRESULT STDMETHODCALLTYPE IEventBinding_get_MAPIOBJECT2_Proxy( 
    IEventBinding __RPC_FAR * This,
    /* [retval][out] */ VARIANT __RPC_FAR *pVal);


void __RPC_STUB IEventBinding_get_MAPIOBJECT2_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IEventBinding_SaveCustomChanges_Proxy( 
    IEventBinding __RPC_FAR * This,
    /* [in] */ VARIANT varMAPIOBJECT);


void __RPC_STUB IEventBinding_SaveCustomChanges_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IEventBinding_get_EntryID2_Proxy( 
    IEventBinding __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pVal);


void __RPC_STUB IEventBinding_get_EntryID2_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IEventBinding_get_EntryID3_Proxy( 
    IEventBinding __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pVal);


void __RPC_STUB IEventBinding_get_EntryID3_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][restricted][hidden][id][propget] */ HRESULT STDMETHODCALLTYPE IEventBinding_get_MAPIOBJECT3_Proxy( 
    IEventBinding __RPC_FAR * This,
    /* [retval][out] */ VARIANT __RPC_FAR *pVal);


void __RPC_STUB IEventBinding_get_MAPIOBJECT3_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IEventBinding_INTERFACE_DEFINED__ */


#ifndef __ISchedule_INTERFACE_DEFINED__
#define __ISchedule_INTERFACE_DEFINED__

/****************************************
 * Generated header for interface: ISchedule
 * at Fri Mar 27 05:37:32 1998
 * using MIDL 3.01.75
 ****************************************/
/* [unique][dual][oleautomation][helpstring][uuid][object] */ 



EXTERN_C const IID IID_ISchedule;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    interface DECLSPEC_UUID("2F42C699-C6A4-11D0-93E9-00AA0064D470")
    ISchedule : public IDispatch
    {
    public:
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Interval( 
            /* [retval][out] */ ULONG __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_Interval( 
            /* [in] */ ULONG newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Days( 
            /* [retval][out] */ ULONG __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_Days( 
            /* [in] */ ULONG newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_At( 
            /* [retval][out] */ DATE __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_At( 
            /* [in] */ DATE newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_StartTime( 
            /* [retval][out] */ DATE __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_StartTime( 
            /* [in] */ DATE newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_EndTime( 
            /* [retval][out] */ DATE __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_EndTime( 
            /* [in] */ DATE newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Type( 
            /* [retval][out] */ ULONG __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_Type( 
            /* [in] */ ULONG newVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Clear( void) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Bitmap( 
            /* [retval][out] */ VARIANT __RPC_FAR *pVal) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IScheduleVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            ISchedule __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            ISchedule __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            ISchedule __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            ISchedule __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            ISchedule __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            ISchedule __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            ISchedule __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Interval )( 
            ISchedule __RPC_FAR * This,
            /* [retval][out] */ ULONG __RPC_FAR *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Interval )( 
            ISchedule __RPC_FAR * This,
            /* [in] */ ULONG newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Days )( 
            ISchedule __RPC_FAR * This,
            /* [retval][out] */ ULONG __RPC_FAR *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Days )( 
            ISchedule __RPC_FAR * This,
            /* [in] */ ULONG newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_At )( 
            ISchedule __RPC_FAR * This,
            /* [retval][out] */ DATE __RPC_FAR *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_At )( 
            ISchedule __RPC_FAR * This,
            /* [in] */ DATE newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_StartTime )( 
            ISchedule __RPC_FAR * This,
            /* [retval][out] */ DATE __RPC_FAR *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_StartTime )( 
            ISchedule __RPC_FAR * This,
            /* [in] */ DATE newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_EndTime )( 
            ISchedule __RPC_FAR * This,
            /* [retval][out] */ DATE __RPC_FAR *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_EndTime )( 
            ISchedule __RPC_FAR * This,
            /* [in] */ DATE newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Type )( 
            ISchedule __RPC_FAR * This,
            /* [retval][out] */ ULONG __RPC_FAR *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Type )( 
            ISchedule __RPC_FAR * This,
            /* [in] */ ULONG newVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Clear )( 
            ISchedule __RPC_FAR * This);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Bitmap )( 
            ISchedule __RPC_FAR * This,
            /* [retval][out] */ VARIANT __RPC_FAR *pVal);
        
        END_INTERFACE
    } IScheduleVtbl;

    interface ISchedule
    {
        CONST_VTBL struct IScheduleVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define ISchedule_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define ISchedule_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define ISchedule_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define ISchedule_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define ISchedule_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define ISchedule_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define ISchedule_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define ISchedule_get_Interval(This,pVal)	\
    (This)->lpVtbl -> get_Interval(This,pVal)

#define ISchedule_put_Interval(This,newVal)	\
    (This)->lpVtbl -> put_Interval(This,newVal)

#define ISchedule_get_Days(This,pVal)	\
    (This)->lpVtbl -> get_Days(This,pVal)

#define ISchedule_put_Days(This,newVal)	\
    (This)->lpVtbl -> put_Days(This,newVal)

#define ISchedule_get_At(This,pVal)	\
    (This)->lpVtbl -> get_At(This,pVal)

#define ISchedule_put_At(This,newVal)	\
    (This)->lpVtbl -> put_At(This,newVal)

#define ISchedule_get_StartTime(This,pVal)	\
    (This)->lpVtbl -> get_StartTime(This,pVal)

#define ISchedule_put_StartTime(This,newVal)	\
    (This)->lpVtbl -> put_StartTime(This,newVal)

#define ISchedule_get_EndTime(This,pVal)	\
    (This)->lpVtbl -> get_EndTime(This,pVal)

#define ISchedule_put_EndTime(This,newVal)	\
    (This)->lpVtbl -> put_EndTime(This,newVal)

#define ISchedule_get_Type(This,pVal)	\
    (This)->lpVtbl -> get_Type(This,pVal)

#define ISchedule_put_Type(This,newVal)	\
    (This)->lpVtbl -> put_Type(This,newVal)

#define ISchedule_Clear(This)	\
    (This)->lpVtbl -> Clear(This)

#define ISchedule_get_Bitmap(This,pVal)	\
    (This)->lpVtbl -> get_Bitmap(This,pVal)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE ISchedule_get_Interval_Proxy( 
    ISchedule __RPC_FAR * This,
    /* [retval][out] */ ULONG __RPC_FAR *pVal);


void __RPC_STUB ISchedule_get_Interval_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE ISchedule_put_Interval_Proxy( 
    ISchedule __RPC_FAR * This,
    /* [in] */ ULONG newVal);


void __RPC_STUB ISchedule_put_Interval_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE ISchedule_get_Days_Proxy( 
    ISchedule __RPC_FAR * This,
    /* [retval][out] */ ULONG __RPC_FAR *pVal);


void __RPC_STUB ISchedule_get_Days_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE ISchedule_put_Days_Proxy( 
    ISchedule __RPC_FAR * This,
    /* [in] */ ULONG newVal);


void __RPC_STUB ISchedule_put_Days_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE ISchedule_get_At_Proxy( 
    ISchedule __RPC_FAR * This,
    /* [retval][out] */ DATE __RPC_FAR *pVal);


void __RPC_STUB ISchedule_get_At_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE ISchedule_put_At_Proxy( 
    ISchedule __RPC_FAR * This,
    /* [in] */ DATE newVal);


void __RPC_STUB ISchedule_put_At_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE ISchedule_get_StartTime_Proxy( 
    ISchedule __RPC_FAR * This,
    /* [retval][out] */ DATE __RPC_FAR *pVal);


void __RPC_STUB ISchedule_get_StartTime_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE ISchedule_put_StartTime_Proxy( 
    ISchedule __RPC_FAR * This,
    /* [in] */ DATE newVal);


void __RPC_STUB ISchedule_put_StartTime_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE ISchedule_get_EndTime_Proxy( 
    ISchedule __RPC_FAR * This,
    /* [retval][out] */ DATE __RPC_FAR *pVal);


void __RPC_STUB ISchedule_get_EndTime_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE ISchedule_put_EndTime_Proxy( 
    ISchedule __RPC_FAR * This,
    /* [in] */ DATE newVal);


void __RPC_STUB ISchedule_put_EndTime_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE ISchedule_get_Type_Proxy( 
    ISchedule __RPC_FAR * This,
    /* [retval][out] */ ULONG __RPC_FAR *pVal);


void __RPC_STUB ISchedule_get_Type_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE ISchedule_put_Type_Proxy( 
    ISchedule __RPC_FAR * This,
    /* [in] */ ULONG newVal);


void __RPC_STUB ISchedule_put_Type_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISchedule_Clear_Proxy( 
    ISchedule __RPC_FAR * This);


void __RPC_STUB ISchedule_Clear_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE ISchedule_get_Bitmap_Proxy( 
    ISchedule __RPC_FAR * This,
    /* [retval][out] */ VARIANT __RPC_FAR *pVal);


void __RPC_STUB ISchedule_get_Bitmap_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __ISchedule_INTERFACE_DEFINED__ */



#ifndef __ESCONFIGLib_LIBRARY_DEFINED__
#define __ESCONFIGLib_LIBRARY_DEFINED__

/****************************************
 * Generated header for library: ESCONFIGLib
 * at Fri Mar 27 05:37:32 1998
 * using MIDL 3.01.75
 ****************************************/
/* [helpstring][version][uuid] */ 


typedef 
enum _EventMask
    {	ESEM_None	= 0,
	ESEM_ScheduledEvents	= 0x1,
	ESEM_NewItemEvents	= 0x2,
	ESEM_ChangedItemEvents	= 0x4,
	ESEM_DeletedItemEvents	= 0x8,
	ESEM_AllEvents	= 0xffff
    }	_EventMask;

typedef 
enum _ScheduleResolution
    {	ES_Resolution_Unknown	= 0,
	ES_Resolution_15Min	= ES_Resolution_Unknown + 1,
	ES_Resolution_1Hour	= ES_Resolution_15Min + 1
    }	_ScheduleResolution;

typedef 
enum _ScheduleDays
    {	ES_DOW_None	= 0,
	ES_DOW_Monday	= 0x1,
	ES_DOW_Tuesday	= 0x2,
	ES_DOW_Wednesday	= 0x4,
	ES_DOW_Thursday	= 0x8,
	ES_DOW_Friday	= 0x10,
	ES_DOW_Saturday	= 0x20,
	ES_DOW_Sunday	= 0x40,
	ES_DOW_Daily	= 0x7f
    }	_ScheduleDays;

typedef 
enum _ScheduleType
    {	ES_ScheduleTypeUnknown	= 0,
	ES_ScheduleTypeHourly	= ES_ScheduleTypeUnknown + 1,
	ES_ScheduleTypeDaily	= ES_ScheduleTypeHourly + 1,
	ES_ScheduleTypeWeekly	= ES_ScheduleTypeDaily + 1
    }	_ScheduleType;











EXTERN_C const IID LIBID_ESCONFIGLib;

#ifdef __cplusplus
EXTERN_C const CLSID CLSID_Events;

class DECLSPEC_UUID("2F42C693-C6A4-11D0-93E9-00AA0064D470")
Events;
#endif
#endif /* __ESCONFIGLib_LIBRARY_DEFINED__ */

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
