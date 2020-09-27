/* this ALWAYS GENERATED file contains the definitions for the interfaces */


/* File created by MIDL compiler version 3.01.75 */
/* at Wed Apr 22 19:58:52 1998
 */
/* Compiler settings for gthradminsdk.idl:
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

#ifndef __gthradminsdk_h__
#define __gthradminsdk_h__

#ifdef __cplusplus
extern "C"{
#endif 

/* Forward Declarations */ 

#ifndef __IServers_FWD_DEFINED__
#define __IServers_FWD_DEFINED__
typedef interface IServers IServers;
#endif 	/* __IServers_FWD_DEFINED__ */


#ifndef __IServer_FWD_DEFINED__
#define __IServer_FWD_DEFINED__
typedef interface IServer IServer;
#endif 	/* __IServer_FWD_DEFINED__ */


#ifndef __ISiteRestrictions_FWD_DEFINED__
#define __ISiteRestrictions_FWD_DEFINED__
typedef interface ISiteRestrictions ISiteRestrictions;
#endif 	/* __ISiteRestrictions_FWD_DEFINED__ */


#ifndef __ISiteRestriction_FWD_DEFINED__
#define __ISiteRestriction_FWD_DEFINED__
typedef interface ISiteRestriction ISiteRestriction;
#endif 	/* __ISiteRestriction_FWD_DEFINED__ */


#ifndef __ISitePaths_FWD_DEFINED__
#define __ISitePaths_FWD_DEFINED__
typedef interface ISitePaths ISitePaths;
#endif 	/* __ISitePaths_FWD_DEFINED__ */


#ifndef __ISitePath_FWD_DEFINED__
#define __ISitePath_FWD_DEFINED__
typedef interface ISitePath ISitePath;
#endif 	/* __ISitePath_FWD_DEFINED__ */


#ifndef __IExtensions_FWD_DEFINED__
#define __IExtensions_FWD_DEFINED__
typedef interface IExtensions IExtensions;
#endif 	/* __IExtensions_FWD_DEFINED__ */


#ifndef __IExtension_FWD_DEFINED__
#define __IExtension_FWD_DEFINED__
typedef interface IExtension IExtension;
#endif 	/* __IExtension_FWD_DEFINED__ */


#ifndef __IStartPages_FWD_DEFINED__
#define __IStartPages_FWD_DEFINED__
typedef interface IStartPages IStartPages;
#endif 	/* __IStartPages_FWD_DEFINED__ */


#ifndef __IStartPage_FWD_DEFINED__
#define __IStartPage_FWD_DEFINED__
typedef interface IStartPage IStartPage;
#endif 	/* __IStartPage_FWD_DEFINED__ */


#ifndef __IMappings_FWD_DEFINED__
#define __IMappings_FWD_DEFINED__
typedef interface IMappings IMappings;
#endif 	/* __IMappings_FWD_DEFINED__ */


#ifndef __IMapping_FWD_DEFINED__
#define __IMapping_FWD_DEFINED__
typedef interface IMapping IMapping;
#endif 	/* __IMapping_FWD_DEFINED__ */


#ifndef __IProtocols_FWD_DEFINED__
#define __IProtocols_FWD_DEFINED__
typedef interface IProtocols IProtocols;
#endif 	/* __IProtocols_FWD_DEFINED__ */


#ifndef __IProtocol_FWD_DEFINED__
#define __IProtocol_FWD_DEFINED__
typedef interface IProtocol IProtocol;
#endif 	/* __IProtocol_FWD_DEFINED__ */


#ifndef __IGatherLog_FWD_DEFINED__
#define __IGatherLog_FWD_DEFINED__
typedef interface IGatherLog IGatherLog;
#endif 	/* __IGatherLog_FWD_DEFINED__ */


#ifndef __IGatherLogs_FWD_DEFINED__
#define __IGatherLogs_FWD_DEFINED__
typedef interface IGatherLogs IGatherLogs;
#endif 	/* __IGatherLogs_FWD_DEFINED__ */


/* header files for imported files */
#include "oaidl.h"

void __RPC_FAR * __RPC_USER MIDL_user_allocate(size_t);
void __RPC_USER MIDL_user_free( void __RPC_FAR * ); 

#ifndef __IServers_INTERFACE_DEFINED__
#define __IServers_INTERFACE_DEFINED__

/****************************************
 * Generated header for interface: IServers
 * at Wed Apr 22 19:58:52 1998
 * using MIDL 3.01.75
 ****************************************/
/* [unique][public][helpstring][dual][uuid][object] */ 



EXTERN_C const IID IID_IServers;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    interface DECLSPEC_UUID("0b63e31f-9ccc-11d0-bcdb-00805fccce04")
    IServers : public IDispatch
    {
    public:
        virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_Count( 
            /* [retval][out] */ long __RPC_FAR *pCount) = 0;
        
        virtual /* [id][propget] */ HRESULT STDMETHODCALLTYPE get_Item( 
            /* [in] */ BSTR bstrSite,
            /* [retval][out] */ VARIANT __RPC_FAR *pServerVar) = 0;
        
        virtual /* [id][restricted][propget] */ HRESULT STDMETHODCALLTYPE get__NewEnum( 
            /* [retval][out] */ IUnknown __RPC_FAR *__RPC_FAR *pNewEnum) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Add( 
            /* [in] */ BSTR bstrSite,
            /* [retval][out] */ VARIANT __RPC_FAR *pServerVar) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Remove( 
            /* [in] */ BSTR bstrSite) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE ChangeOrder( 
            /* [in] */ BSTR bstrSite,
            /* [in] */ VARIANT_BOOL fUp) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IServersVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IServers __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IServers __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IServers __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            IServers __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            IServers __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            IServers __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            IServers __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Count )( 
            IServers __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *pCount);
        
        /* [id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Item )( 
            IServers __RPC_FAR * This,
            /* [in] */ BSTR bstrSite,
            /* [retval][out] */ VARIANT __RPC_FAR *pServerVar);
        
        /* [id][restricted][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get__NewEnum )( 
            IServers __RPC_FAR * This,
            /* [retval][out] */ IUnknown __RPC_FAR *__RPC_FAR *pNewEnum);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Add )( 
            IServers __RPC_FAR * This,
            /* [in] */ BSTR bstrSite,
            /* [retval][out] */ VARIANT __RPC_FAR *pServerVar);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Remove )( 
            IServers __RPC_FAR * This,
            /* [in] */ BSTR bstrSite);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *ChangeOrder )( 
            IServers __RPC_FAR * This,
            /* [in] */ BSTR bstrSite,
            /* [in] */ VARIANT_BOOL fUp);
        
        END_INTERFACE
    } IServersVtbl;

    interface IServers
    {
        CONST_VTBL struct IServersVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IServers_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IServers_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IServers_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IServers_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IServers_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IServers_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IServers_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IServers_get_Count(This,pCount)	\
    (This)->lpVtbl -> get_Count(This,pCount)

#define IServers_get_Item(This,bstrSite,pServerVar)	\
    (This)->lpVtbl -> get_Item(This,bstrSite,pServerVar)

#define IServers_get__NewEnum(This,pNewEnum)	\
    (This)->lpVtbl -> get__NewEnum(This,pNewEnum)

#define IServers_Add(This,bstrSite,pServerVar)	\
    (This)->lpVtbl -> Add(This,bstrSite,pServerVar)

#define IServers_Remove(This,bstrSite)	\
    (This)->lpVtbl -> Remove(This,bstrSite)

#define IServers_ChangeOrder(This,bstrSite,fUp)	\
    (This)->lpVtbl -> ChangeOrder(This,bstrSite,fUp)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [propget] */ HRESULT STDMETHODCALLTYPE IServers_get_Count_Proxy( 
    IServers __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *pCount);


void __RPC_STUB IServers_get_Count_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propget] */ HRESULT STDMETHODCALLTYPE IServers_get_Item_Proxy( 
    IServers __RPC_FAR * This,
    /* [in] */ BSTR bstrSite,
    /* [retval][out] */ VARIANT __RPC_FAR *pServerVar);


void __RPC_STUB IServers_get_Item_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][restricted][propget] */ HRESULT STDMETHODCALLTYPE IServers_get__NewEnum_Proxy( 
    IServers __RPC_FAR * This,
    /* [retval][out] */ IUnknown __RPC_FAR *__RPC_FAR *pNewEnum);


void __RPC_STUB IServers_get__NewEnum_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IServers_Add_Proxy( 
    IServers __RPC_FAR * This,
    /* [in] */ BSTR bstrSite,
    /* [retval][out] */ VARIANT __RPC_FAR *pServerVar);


void __RPC_STUB IServers_Add_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IServers_Remove_Proxy( 
    IServers __RPC_FAR * This,
    /* [in] */ BSTR bstrSite);


void __RPC_STUB IServers_Remove_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IServers_ChangeOrder_Proxy( 
    IServers __RPC_FAR * This,
    /* [in] */ BSTR bstrSite,
    /* [in] */ VARIANT_BOOL fUp);


void __RPC_STUB IServers_ChangeOrder_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IServers_INTERFACE_DEFINED__ */


#ifndef __IServer_INTERFACE_DEFINED__
#define __IServer_INTERFACE_DEFINED__

/****************************************
 * Generated header for interface: IServer
 * at Wed Apr 22 19:58:52 1998
 * using MIDL 3.01.75
 ****************************************/
/* [unique][helpstring][public][dual][uuid][object] */ 



EXTERN_C const IID IID_IServer;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    interface DECLSPEC_UUID("0b63e320-9ccc-11d0-bcdb-00805fccce04")
    IServer : public IDispatch
    {
    public:
        virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_Name( 
            /* [retval][out] */ BSTR __RPC_FAR *pbstrName) = 0;
        
        virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_HitInterval( 
            /* [retval][out] */ LONG __RPC_FAR *plInterval) = 0;
        
        virtual /* [propput] */ HRESULT STDMETHODCALLTYPE put_HitInterval( 
            /* [in] */ LONG lInterval) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IServerVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IServer __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IServer __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IServer __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            IServer __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            IServer __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            IServer __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            IServer __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Name )( 
            IServer __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pbstrName);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_HitInterval )( 
            IServer __RPC_FAR * This,
            /* [retval][out] */ LONG __RPC_FAR *plInterval);
        
        /* [propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_HitInterval )( 
            IServer __RPC_FAR * This,
            /* [in] */ LONG lInterval);
        
        END_INTERFACE
    } IServerVtbl;

    interface IServer
    {
        CONST_VTBL struct IServerVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IServer_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IServer_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IServer_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IServer_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IServer_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IServer_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IServer_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IServer_get_Name(This,pbstrName)	\
    (This)->lpVtbl -> get_Name(This,pbstrName)

#define IServer_get_HitInterval(This,plInterval)	\
    (This)->lpVtbl -> get_HitInterval(This,plInterval)

#define IServer_put_HitInterval(This,lInterval)	\
    (This)->lpVtbl -> put_HitInterval(This,lInterval)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [propget] */ HRESULT STDMETHODCALLTYPE IServer_get_Name_Proxy( 
    IServer __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pbstrName);


void __RPC_STUB IServer_get_Name_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget] */ HRESULT STDMETHODCALLTYPE IServer_get_HitInterval_Proxy( 
    IServer __RPC_FAR * This,
    /* [retval][out] */ LONG __RPC_FAR *plInterval);


void __RPC_STUB IServer_get_HitInterval_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propput] */ HRESULT STDMETHODCALLTYPE IServer_put_HitInterval_Proxy( 
    IServer __RPC_FAR * This,
    /* [in] */ LONG lInterval);


void __RPC_STUB IServer_put_HitInterval_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IServer_INTERFACE_DEFINED__ */


#ifndef __ISiteRestrictions_INTERFACE_DEFINED__
#define __ISiteRestrictions_INTERFACE_DEFINED__

/****************************************
 * Generated header for interface: ISiteRestrictions
 * at Wed Apr 22 19:58:52 1998
 * using MIDL 3.01.75
 ****************************************/
/* [unique][helpstring][dual][uuid][public][object] */ 



EXTERN_C const IID IID_ISiteRestrictions;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    interface DECLSPEC_UUID("0b63e326-9ccc-11d0-bcdb-00805fccce04")
    ISiteRestrictions : public IDispatch
    {
    public:
        virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_Count( 
            /* [retval][out] */ long __RPC_FAR *pCount) = 0;
        
        virtual /* [id][restricted][propget] */ HRESULT STDMETHODCALLTYPE get__NewEnum( 
            /* [retval][out] */ IUnknown __RPC_FAR *__RPC_FAR *pNewEnum) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Add( 
            /* [in] */ BSTR bstrName,
            /* [in] */ VARIANT_BOOL fIncluded,
            /* [retval][out] */ VARIANT __RPC_FAR *pVarSiteRestriction) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Remove( 
            /* [in] */ BSTR bstrName) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE ChangeOrder( 
            /* [in] */ BSTR bstrSite,
            /* [in] */ VARIANT_BOOL fUp) = 0;
        
        virtual /* [id][propget] */ HRESULT STDMETHODCALLTYPE get_Item( 
            /* [in] */ BSTR bstrName,
            /* [retval][out] */ VARIANT __RPC_FAR *pSiteRestrictionVar) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct ISiteRestrictionsVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            ISiteRestrictions __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            ISiteRestrictions __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            ISiteRestrictions __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            ISiteRestrictions __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            ISiteRestrictions __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            ISiteRestrictions __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            ISiteRestrictions __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Count )( 
            ISiteRestrictions __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *pCount);
        
        /* [id][restricted][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get__NewEnum )( 
            ISiteRestrictions __RPC_FAR * This,
            /* [retval][out] */ IUnknown __RPC_FAR *__RPC_FAR *pNewEnum);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Add )( 
            ISiteRestrictions __RPC_FAR * This,
            /* [in] */ BSTR bstrName,
            /* [in] */ VARIANT_BOOL fIncluded,
            /* [retval][out] */ VARIANT __RPC_FAR *pVarSiteRestriction);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Remove )( 
            ISiteRestrictions __RPC_FAR * This,
            /* [in] */ BSTR bstrName);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *ChangeOrder )( 
            ISiteRestrictions __RPC_FAR * This,
            /* [in] */ BSTR bstrSite,
            /* [in] */ VARIANT_BOOL fUp);
        
        /* [id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Item )( 
            ISiteRestrictions __RPC_FAR * This,
            /* [in] */ BSTR bstrName,
            /* [retval][out] */ VARIANT __RPC_FAR *pSiteRestrictionVar);
        
        END_INTERFACE
    } ISiteRestrictionsVtbl;

    interface ISiteRestrictions
    {
        CONST_VTBL struct ISiteRestrictionsVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define ISiteRestrictions_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define ISiteRestrictions_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define ISiteRestrictions_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define ISiteRestrictions_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define ISiteRestrictions_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define ISiteRestrictions_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define ISiteRestrictions_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define ISiteRestrictions_get_Count(This,pCount)	\
    (This)->lpVtbl -> get_Count(This,pCount)

#define ISiteRestrictions_get__NewEnum(This,pNewEnum)	\
    (This)->lpVtbl -> get__NewEnum(This,pNewEnum)

#define ISiteRestrictions_Add(This,bstrName,fIncluded,pVarSiteRestriction)	\
    (This)->lpVtbl -> Add(This,bstrName,fIncluded,pVarSiteRestriction)

#define ISiteRestrictions_Remove(This,bstrName)	\
    (This)->lpVtbl -> Remove(This,bstrName)

#define ISiteRestrictions_ChangeOrder(This,bstrSite,fUp)	\
    (This)->lpVtbl -> ChangeOrder(This,bstrSite,fUp)

#define ISiteRestrictions_get_Item(This,bstrName,pSiteRestrictionVar)	\
    (This)->lpVtbl -> get_Item(This,bstrName,pSiteRestrictionVar)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [propget] */ HRESULT STDMETHODCALLTYPE ISiteRestrictions_get_Count_Proxy( 
    ISiteRestrictions __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *pCount);


void __RPC_STUB ISiteRestrictions_get_Count_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][restricted][propget] */ HRESULT STDMETHODCALLTYPE ISiteRestrictions_get__NewEnum_Proxy( 
    ISiteRestrictions __RPC_FAR * This,
    /* [retval][out] */ IUnknown __RPC_FAR *__RPC_FAR *pNewEnum);


void __RPC_STUB ISiteRestrictions_get__NewEnum_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ISiteRestrictions_Add_Proxy( 
    ISiteRestrictions __RPC_FAR * This,
    /* [in] */ BSTR bstrName,
    /* [in] */ VARIANT_BOOL fIncluded,
    /* [retval][out] */ VARIANT __RPC_FAR *pVarSiteRestriction);


void __RPC_STUB ISiteRestrictions_Add_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ISiteRestrictions_Remove_Proxy( 
    ISiteRestrictions __RPC_FAR * This,
    /* [in] */ BSTR bstrName);


void __RPC_STUB ISiteRestrictions_Remove_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ISiteRestrictions_ChangeOrder_Proxy( 
    ISiteRestrictions __RPC_FAR * This,
    /* [in] */ BSTR bstrSite,
    /* [in] */ VARIANT_BOOL fUp);


void __RPC_STUB ISiteRestrictions_ChangeOrder_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propget] */ HRESULT STDMETHODCALLTYPE ISiteRestrictions_get_Item_Proxy( 
    ISiteRestrictions __RPC_FAR * This,
    /* [in] */ BSTR bstrName,
    /* [retval][out] */ VARIANT __RPC_FAR *pSiteRestrictionVar);


void __RPC_STUB ISiteRestrictions_get_Item_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __ISiteRestrictions_INTERFACE_DEFINED__ */


#ifndef __ISiteRestriction_INTERFACE_DEFINED__
#define __ISiteRestriction_INTERFACE_DEFINED__

/****************************************
 * Generated header for interface: ISiteRestriction
 * at Wed Apr 22 19:58:52 1998
 * using MIDL 3.01.75
 ****************************************/
/* [unique][helpstring][dual][uuid][public][object] */ 



EXTERN_C const IID IID_ISiteRestriction;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    interface DECLSPEC_UUID("0b63e327-9ccc-11d0-bcdb-00805fccce04")
    ISiteRestriction : public IDispatch
    {
    public:
        virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_Name( 
            /* [retval][out] */ BSTR __RPC_FAR *pbstrName) = 0;
        
        virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_Included( 
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pfIncluded) = 0;
        
        virtual /* [propput] */ HRESULT STDMETHODCALLTYPE put_Included( 
            /* [in] */ VARIANT_BOOL fIncluded) = 0;
        
        virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_AccountName( 
            /* [retval][out] */ BSTR __RPC_FAR *pbstrAccountName) = 0;
        
        virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_AuthenticationType( 
            /* [retval][out] */ LONG __RPC_FAR *plAuthenticationType) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetAccount( 
            /* [in] */ BSTR bstrAccount,
            /* [in] */ BSTR bstrPassword,
            /* [in] */ LONG lAuthenticationType) = 0;
        
        virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_Paths( 
            /* [retval][out] */ VARIANT __RPC_FAR *pPathsVar) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct ISiteRestrictionVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            ISiteRestriction __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            ISiteRestriction __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            ISiteRestriction __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            ISiteRestriction __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            ISiteRestriction __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            ISiteRestriction __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            ISiteRestriction __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Name )( 
            ISiteRestriction __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pbstrName);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Included )( 
            ISiteRestriction __RPC_FAR * This,
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pfIncluded);
        
        /* [propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Included )( 
            ISiteRestriction __RPC_FAR * This,
            /* [in] */ VARIANT_BOOL fIncluded);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_AccountName )( 
            ISiteRestriction __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pbstrAccountName);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_AuthenticationType )( 
            ISiteRestriction __RPC_FAR * This,
            /* [retval][out] */ LONG __RPC_FAR *plAuthenticationType);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *SetAccount )( 
            ISiteRestriction __RPC_FAR * This,
            /* [in] */ BSTR bstrAccount,
            /* [in] */ BSTR bstrPassword,
            /* [in] */ LONG lAuthenticationType);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Paths )( 
            ISiteRestriction __RPC_FAR * This,
            /* [retval][out] */ VARIANT __RPC_FAR *pPathsVar);
        
        END_INTERFACE
    } ISiteRestrictionVtbl;

    interface ISiteRestriction
    {
        CONST_VTBL struct ISiteRestrictionVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define ISiteRestriction_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define ISiteRestriction_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define ISiteRestriction_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define ISiteRestriction_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define ISiteRestriction_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define ISiteRestriction_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define ISiteRestriction_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define ISiteRestriction_get_Name(This,pbstrName)	\
    (This)->lpVtbl -> get_Name(This,pbstrName)

#define ISiteRestriction_get_Included(This,pfIncluded)	\
    (This)->lpVtbl -> get_Included(This,pfIncluded)

#define ISiteRestriction_put_Included(This,fIncluded)	\
    (This)->lpVtbl -> put_Included(This,fIncluded)

#define ISiteRestriction_get_AccountName(This,pbstrAccountName)	\
    (This)->lpVtbl -> get_AccountName(This,pbstrAccountName)

#define ISiteRestriction_get_AuthenticationType(This,plAuthenticationType)	\
    (This)->lpVtbl -> get_AuthenticationType(This,plAuthenticationType)

#define ISiteRestriction_SetAccount(This,bstrAccount,bstrPassword,lAuthenticationType)	\
    (This)->lpVtbl -> SetAccount(This,bstrAccount,bstrPassword,lAuthenticationType)

#define ISiteRestriction_get_Paths(This,pPathsVar)	\
    (This)->lpVtbl -> get_Paths(This,pPathsVar)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [propget] */ HRESULT STDMETHODCALLTYPE ISiteRestriction_get_Name_Proxy( 
    ISiteRestriction __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pbstrName);


void __RPC_STUB ISiteRestriction_get_Name_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget] */ HRESULT STDMETHODCALLTYPE ISiteRestriction_get_Included_Proxy( 
    ISiteRestriction __RPC_FAR * This,
    /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pfIncluded);


void __RPC_STUB ISiteRestriction_get_Included_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propput] */ HRESULT STDMETHODCALLTYPE ISiteRestriction_put_Included_Proxy( 
    ISiteRestriction __RPC_FAR * This,
    /* [in] */ VARIANT_BOOL fIncluded);


void __RPC_STUB ISiteRestriction_put_Included_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget] */ HRESULT STDMETHODCALLTYPE ISiteRestriction_get_AccountName_Proxy( 
    ISiteRestriction __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pbstrAccountName);


void __RPC_STUB ISiteRestriction_get_AccountName_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget] */ HRESULT STDMETHODCALLTYPE ISiteRestriction_get_AuthenticationType_Proxy( 
    ISiteRestriction __RPC_FAR * This,
    /* [retval][out] */ LONG __RPC_FAR *plAuthenticationType);


void __RPC_STUB ISiteRestriction_get_AuthenticationType_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ISiteRestriction_SetAccount_Proxy( 
    ISiteRestriction __RPC_FAR * This,
    /* [in] */ BSTR bstrAccount,
    /* [in] */ BSTR bstrPassword,
    /* [in] */ LONG lAuthenticationType);


void __RPC_STUB ISiteRestriction_SetAccount_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget] */ HRESULT STDMETHODCALLTYPE ISiteRestriction_get_Paths_Proxy( 
    ISiteRestriction __RPC_FAR * This,
    /* [retval][out] */ VARIANT __RPC_FAR *pPathsVar);


void __RPC_STUB ISiteRestriction_get_Paths_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __ISiteRestriction_INTERFACE_DEFINED__ */


#ifndef __ISitePaths_INTERFACE_DEFINED__
#define __ISitePaths_INTERFACE_DEFINED__

/****************************************
 * Generated header for interface: ISitePaths
 * at Wed Apr 22 19:58:52 1998
 * using MIDL 3.01.75
 ****************************************/
/* [unique][helpstring][dual][uuid][public][object] */ 



EXTERN_C const IID IID_ISitePaths;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    interface DECLSPEC_UUID("0b63e328-9ccc-11d0-bcdb-00805fccce04")
    ISitePaths : public IDispatch
    {
    public:
        virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_Count( 
            /* [retval][out] */ long __RPC_FAR *pCount) = 0;
        
        virtual /* [id][restricted][propget] */ HRESULT STDMETHODCALLTYPE get__NewEnum( 
            /* [retval][out] */ IUnknown __RPC_FAR *__RPC_FAR *pNewEnum) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Add( 
            /* [in] */ BSTR bstrPath,
            /* [in] */ VARIANT_BOOL fIncluded,
            /* [retval][out] */ VARIANT __RPC_FAR *pVarSitePath) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Remove( 
            /* [in] */ BSTR bstrPath) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE ChangeOrder( 
            /* [in] */ BSTR bstrPath,
            /* [in] */ VARIANT_BOOL fUp) = 0;
        
        virtual /* [id][propget] */ HRESULT STDMETHODCALLTYPE get_Item( 
            /* [in] */ BSTR bstrName,
            /* [retval][out] */ VARIANT __RPC_FAR *pSitePathVar) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct ISitePathsVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            ISitePaths __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            ISitePaths __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            ISitePaths __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            ISitePaths __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            ISitePaths __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            ISitePaths __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            ISitePaths __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Count )( 
            ISitePaths __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *pCount);
        
        /* [id][restricted][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get__NewEnum )( 
            ISitePaths __RPC_FAR * This,
            /* [retval][out] */ IUnknown __RPC_FAR *__RPC_FAR *pNewEnum);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Add )( 
            ISitePaths __RPC_FAR * This,
            /* [in] */ BSTR bstrPath,
            /* [in] */ VARIANT_BOOL fIncluded,
            /* [retval][out] */ VARIANT __RPC_FAR *pVarSitePath);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Remove )( 
            ISitePaths __RPC_FAR * This,
            /* [in] */ BSTR bstrPath);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *ChangeOrder )( 
            ISitePaths __RPC_FAR * This,
            /* [in] */ BSTR bstrPath,
            /* [in] */ VARIANT_BOOL fUp);
        
        /* [id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Item )( 
            ISitePaths __RPC_FAR * This,
            /* [in] */ BSTR bstrName,
            /* [retval][out] */ VARIANT __RPC_FAR *pSitePathVar);
        
        END_INTERFACE
    } ISitePathsVtbl;

    interface ISitePaths
    {
        CONST_VTBL struct ISitePathsVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define ISitePaths_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define ISitePaths_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define ISitePaths_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define ISitePaths_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define ISitePaths_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define ISitePaths_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define ISitePaths_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define ISitePaths_get_Count(This,pCount)	\
    (This)->lpVtbl -> get_Count(This,pCount)

#define ISitePaths_get__NewEnum(This,pNewEnum)	\
    (This)->lpVtbl -> get__NewEnum(This,pNewEnum)

#define ISitePaths_Add(This,bstrPath,fIncluded,pVarSitePath)	\
    (This)->lpVtbl -> Add(This,bstrPath,fIncluded,pVarSitePath)

#define ISitePaths_Remove(This,bstrPath)	\
    (This)->lpVtbl -> Remove(This,bstrPath)

#define ISitePaths_ChangeOrder(This,bstrPath,fUp)	\
    (This)->lpVtbl -> ChangeOrder(This,bstrPath,fUp)

#define ISitePaths_get_Item(This,bstrName,pSitePathVar)	\
    (This)->lpVtbl -> get_Item(This,bstrName,pSitePathVar)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [propget] */ HRESULT STDMETHODCALLTYPE ISitePaths_get_Count_Proxy( 
    ISitePaths __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *pCount);


void __RPC_STUB ISitePaths_get_Count_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][restricted][propget] */ HRESULT STDMETHODCALLTYPE ISitePaths_get__NewEnum_Proxy( 
    ISitePaths __RPC_FAR * This,
    /* [retval][out] */ IUnknown __RPC_FAR *__RPC_FAR *pNewEnum);


void __RPC_STUB ISitePaths_get__NewEnum_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ISitePaths_Add_Proxy( 
    ISitePaths __RPC_FAR * This,
    /* [in] */ BSTR bstrPath,
    /* [in] */ VARIANT_BOOL fIncluded,
    /* [retval][out] */ VARIANT __RPC_FAR *pVarSitePath);


void __RPC_STUB ISitePaths_Add_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ISitePaths_Remove_Proxy( 
    ISitePaths __RPC_FAR * This,
    /* [in] */ BSTR bstrPath);


void __RPC_STUB ISitePaths_Remove_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ISitePaths_ChangeOrder_Proxy( 
    ISitePaths __RPC_FAR * This,
    /* [in] */ BSTR bstrPath,
    /* [in] */ VARIANT_BOOL fUp);


void __RPC_STUB ISitePaths_ChangeOrder_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propget] */ HRESULT STDMETHODCALLTYPE ISitePaths_get_Item_Proxy( 
    ISitePaths __RPC_FAR * This,
    /* [in] */ BSTR bstrName,
    /* [retval][out] */ VARIANT __RPC_FAR *pSitePathVar);


void __RPC_STUB ISitePaths_get_Item_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __ISitePaths_INTERFACE_DEFINED__ */


#ifndef __ISitePath_INTERFACE_DEFINED__
#define __ISitePath_INTERFACE_DEFINED__

/****************************************
 * Generated header for interface: ISitePath
 * at Wed Apr 22 19:58:52 1998
 * using MIDL 3.01.75
 ****************************************/
/* [unique][helpstring][dual][uuid][public][object] */ 



EXTERN_C const IID IID_ISitePath;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    interface DECLSPEC_UUID("0b63e329-9ccc-11d0-bcdb-00805fccce04")
    ISitePath : public IDispatch
    {
    public:
        virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_Path( 
            /* [retval][out] */ BSTR __RPC_FAR *pbstrPath) = 0;
        
        virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_Included( 
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pfIncluded) = 0;
        
        virtual /* [propput] */ HRESULT STDMETHODCALLTYPE put_Included( 
            /* [in] */ VARIANT_BOOL fIncluded) = 0;
        
        virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_IncludeSubdirs( 
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pfIncludeSubdirs) = 0;
        
        virtual /* [propput] */ HRESULT STDMETHODCALLTYPE put_IncludeSubdirs( 
            /* [in] */ VARIANT_BOOL fIncludeSubdirs) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct ISitePathVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            ISitePath __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            ISitePath __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            ISitePath __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            ISitePath __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            ISitePath __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            ISitePath __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            ISitePath __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Path )( 
            ISitePath __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pbstrPath);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Included )( 
            ISitePath __RPC_FAR * This,
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pfIncluded);
        
        /* [propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Included )( 
            ISitePath __RPC_FAR * This,
            /* [in] */ VARIANT_BOOL fIncluded);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_IncludeSubdirs )( 
            ISitePath __RPC_FAR * This,
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pfIncludeSubdirs);
        
        /* [propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_IncludeSubdirs )( 
            ISitePath __RPC_FAR * This,
            /* [in] */ VARIANT_BOOL fIncludeSubdirs);
        
        END_INTERFACE
    } ISitePathVtbl;

    interface ISitePath
    {
        CONST_VTBL struct ISitePathVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define ISitePath_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define ISitePath_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define ISitePath_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define ISitePath_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define ISitePath_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define ISitePath_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define ISitePath_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define ISitePath_get_Path(This,pbstrPath)	\
    (This)->lpVtbl -> get_Path(This,pbstrPath)

#define ISitePath_get_Included(This,pfIncluded)	\
    (This)->lpVtbl -> get_Included(This,pfIncluded)

#define ISitePath_put_Included(This,fIncluded)	\
    (This)->lpVtbl -> put_Included(This,fIncluded)

#define ISitePath_get_IncludeSubdirs(This,pfIncludeSubdirs)	\
    (This)->lpVtbl -> get_IncludeSubdirs(This,pfIncludeSubdirs)

#define ISitePath_put_IncludeSubdirs(This,fIncludeSubdirs)	\
    (This)->lpVtbl -> put_IncludeSubdirs(This,fIncludeSubdirs)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [propget] */ HRESULT STDMETHODCALLTYPE ISitePath_get_Path_Proxy( 
    ISitePath __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pbstrPath);


void __RPC_STUB ISitePath_get_Path_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget] */ HRESULT STDMETHODCALLTYPE ISitePath_get_Included_Proxy( 
    ISitePath __RPC_FAR * This,
    /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pfIncluded);


void __RPC_STUB ISitePath_get_Included_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propput] */ HRESULT STDMETHODCALLTYPE ISitePath_put_Included_Proxy( 
    ISitePath __RPC_FAR * This,
    /* [in] */ VARIANT_BOOL fIncluded);


void __RPC_STUB ISitePath_put_Included_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget] */ HRESULT STDMETHODCALLTYPE ISitePath_get_IncludeSubdirs_Proxy( 
    ISitePath __RPC_FAR * This,
    /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pfIncludeSubdirs);


void __RPC_STUB ISitePath_get_IncludeSubdirs_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propput] */ HRESULT STDMETHODCALLTYPE ISitePath_put_IncludeSubdirs_Proxy( 
    ISitePath __RPC_FAR * This,
    /* [in] */ VARIANT_BOOL fIncludeSubdirs);


void __RPC_STUB ISitePath_put_IncludeSubdirs_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __ISitePath_INTERFACE_DEFINED__ */


#ifndef __IExtensions_INTERFACE_DEFINED__
#define __IExtensions_INTERFACE_DEFINED__

/****************************************
 * Generated header for interface: IExtensions
 * at Wed Apr 22 19:58:52 1998
 * using MIDL 3.01.75
 ****************************************/
/* [unique][helpstring][dual][uuid][public][object] */ 



EXTERN_C const IID IID_IExtensions;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    interface DECLSPEC_UUID("0b63e32a-9ccc-11d0-bcdb-00805fccce04")
    IExtensions : public IDispatch
    {
    public:
        virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_Count( 
            /* [retval][out] */ long __RPC_FAR *pCount) = 0;
        
        virtual /* [id][restricted][propget] */ HRESULT STDMETHODCALLTYPE get__NewEnum( 
            /* [retval][out] */ IUnknown __RPC_FAR *__RPC_FAR *pNewEnum) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Add( 
            /* [in] */ BSTR bstrExtension,
            /* [retval][out] */ VARIANT __RPC_FAR *pVarExtension) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Remove( 
            /* [in] */ BSTR bstrExtension) = 0;
        
        virtual /* [id][propget] */ HRESULT STDMETHODCALLTYPE get_Item( 
            /* [in] */ BSTR bstrExtension,
            /* [retval][out] */ VARIANT __RPC_FAR *pSitePathVar) = 0;
        
        virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_IncludedExtensions( 
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pfIncludedExtensions) = 0;
        
        virtual /* [propput] */ HRESULT STDMETHODCALLTYPE put_IncludedExtensions( 
            /* [in] */ VARIANT_BOOL fIncludedExtensions) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IExtensionsVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IExtensions __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IExtensions __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IExtensions __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            IExtensions __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            IExtensions __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            IExtensions __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            IExtensions __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Count )( 
            IExtensions __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *pCount);
        
        /* [id][restricted][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get__NewEnum )( 
            IExtensions __RPC_FAR * This,
            /* [retval][out] */ IUnknown __RPC_FAR *__RPC_FAR *pNewEnum);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Add )( 
            IExtensions __RPC_FAR * This,
            /* [in] */ BSTR bstrExtension,
            /* [retval][out] */ VARIANT __RPC_FAR *pVarExtension);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Remove )( 
            IExtensions __RPC_FAR * This,
            /* [in] */ BSTR bstrExtension);
        
        /* [id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Item )( 
            IExtensions __RPC_FAR * This,
            /* [in] */ BSTR bstrExtension,
            /* [retval][out] */ VARIANT __RPC_FAR *pSitePathVar);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_IncludedExtensions )( 
            IExtensions __RPC_FAR * This,
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pfIncludedExtensions);
        
        /* [propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_IncludedExtensions )( 
            IExtensions __RPC_FAR * This,
            /* [in] */ VARIANT_BOOL fIncludedExtensions);
        
        END_INTERFACE
    } IExtensionsVtbl;

    interface IExtensions
    {
        CONST_VTBL struct IExtensionsVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IExtensions_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IExtensions_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IExtensions_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IExtensions_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IExtensions_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IExtensions_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IExtensions_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IExtensions_get_Count(This,pCount)	\
    (This)->lpVtbl -> get_Count(This,pCount)

#define IExtensions_get__NewEnum(This,pNewEnum)	\
    (This)->lpVtbl -> get__NewEnum(This,pNewEnum)

#define IExtensions_Add(This,bstrExtension,pVarExtension)	\
    (This)->lpVtbl -> Add(This,bstrExtension,pVarExtension)

#define IExtensions_Remove(This,bstrExtension)	\
    (This)->lpVtbl -> Remove(This,bstrExtension)

#define IExtensions_get_Item(This,bstrExtension,pSitePathVar)	\
    (This)->lpVtbl -> get_Item(This,bstrExtension,pSitePathVar)

#define IExtensions_get_IncludedExtensions(This,pfIncludedExtensions)	\
    (This)->lpVtbl -> get_IncludedExtensions(This,pfIncludedExtensions)

#define IExtensions_put_IncludedExtensions(This,fIncludedExtensions)	\
    (This)->lpVtbl -> put_IncludedExtensions(This,fIncludedExtensions)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [propget] */ HRESULT STDMETHODCALLTYPE IExtensions_get_Count_Proxy( 
    IExtensions __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *pCount);


void __RPC_STUB IExtensions_get_Count_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][restricted][propget] */ HRESULT STDMETHODCALLTYPE IExtensions_get__NewEnum_Proxy( 
    IExtensions __RPC_FAR * This,
    /* [retval][out] */ IUnknown __RPC_FAR *__RPC_FAR *pNewEnum);


void __RPC_STUB IExtensions_get__NewEnum_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IExtensions_Add_Proxy( 
    IExtensions __RPC_FAR * This,
    /* [in] */ BSTR bstrExtension,
    /* [retval][out] */ VARIANT __RPC_FAR *pVarExtension);


void __RPC_STUB IExtensions_Add_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IExtensions_Remove_Proxy( 
    IExtensions __RPC_FAR * This,
    /* [in] */ BSTR bstrExtension);


void __RPC_STUB IExtensions_Remove_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propget] */ HRESULT STDMETHODCALLTYPE IExtensions_get_Item_Proxy( 
    IExtensions __RPC_FAR * This,
    /* [in] */ BSTR bstrExtension,
    /* [retval][out] */ VARIANT __RPC_FAR *pSitePathVar);


void __RPC_STUB IExtensions_get_Item_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget] */ HRESULT STDMETHODCALLTYPE IExtensions_get_IncludedExtensions_Proxy( 
    IExtensions __RPC_FAR * This,
    /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pfIncludedExtensions);


void __RPC_STUB IExtensions_get_IncludedExtensions_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propput] */ HRESULT STDMETHODCALLTYPE IExtensions_put_IncludedExtensions_Proxy( 
    IExtensions __RPC_FAR * This,
    /* [in] */ VARIANT_BOOL fIncludedExtensions);


void __RPC_STUB IExtensions_put_IncludedExtensions_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IExtensions_INTERFACE_DEFINED__ */


#ifndef __IExtension_INTERFACE_DEFINED__
#define __IExtension_INTERFACE_DEFINED__

/****************************************
 * Generated header for interface: IExtension
 * at Wed Apr 22 19:58:52 1998
 * using MIDL 3.01.75
 ****************************************/
/* [unique][helpstring][dual][uuid][public][object] */ 



EXTERN_C const IID IID_IExtension;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    interface DECLSPEC_UUID("0b63e32b-9ccc-11d0-bcdb-00805fccce04")
    IExtension : public IDispatch
    {
    public:
        virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_Extension( 
            /* [retval][out] */ BSTR __RPC_FAR *pbstrExtension) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IExtensionVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IExtension __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IExtension __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IExtension __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            IExtension __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            IExtension __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            IExtension __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            IExtension __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Extension )( 
            IExtension __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pbstrExtension);
        
        END_INTERFACE
    } IExtensionVtbl;

    interface IExtension
    {
        CONST_VTBL struct IExtensionVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IExtension_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IExtension_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IExtension_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IExtension_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IExtension_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IExtension_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IExtension_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IExtension_get_Extension(This,pbstrExtension)	\
    (This)->lpVtbl -> get_Extension(This,pbstrExtension)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [propget] */ HRESULT STDMETHODCALLTYPE IExtension_get_Extension_Proxy( 
    IExtension __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pbstrExtension);


void __RPC_STUB IExtension_get_Extension_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IExtension_INTERFACE_DEFINED__ */


#ifndef __IStartPages_INTERFACE_DEFINED__
#define __IStartPages_INTERFACE_DEFINED__

/****************************************
 * Generated header for interface: IStartPages
 * at Wed Apr 22 19:58:52 1998
 * using MIDL 3.01.75
 ****************************************/
/* [unique][helpstring][dual][uuid][public][object] */ 



EXTERN_C const IID IID_IStartPages;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    interface DECLSPEC_UUID("0b63e32c-9ccc-11d0-bcdb-00805fccce04")
    IStartPages : public IDispatch
    {
    public:
        virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_Count( 
            /* [retval][out] */ long __RPC_FAR *pCount) = 0;
        
        virtual /* [id][propget] */ HRESULT STDMETHODCALLTYPE get_Item( 
            /* [in] */ BSTR bstrURL,
            /* [retval][out] */ VARIANT __RPC_FAR *pStartPageVar) = 0;
        
        virtual /* [id][restricted][propget] */ HRESULT STDMETHODCALLTYPE get__NewEnum( 
            /* [retval][out] */ IUnknown __RPC_FAR *__RPC_FAR *pNewEnum) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Add( 
            /* [in] */ BSTR bstrURL,
            /* [retval][out] */ VARIANT __RPC_FAR *pStartPageVar) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Remove( 
            /* [in] */ BSTR bstrURL) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IStartPagesVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IStartPages __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IStartPages __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IStartPages __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            IStartPages __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            IStartPages __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            IStartPages __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            IStartPages __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Count )( 
            IStartPages __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *pCount);
        
        /* [id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Item )( 
            IStartPages __RPC_FAR * This,
            /* [in] */ BSTR bstrURL,
            /* [retval][out] */ VARIANT __RPC_FAR *pStartPageVar);
        
        /* [id][restricted][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get__NewEnum )( 
            IStartPages __RPC_FAR * This,
            /* [retval][out] */ IUnknown __RPC_FAR *__RPC_FAR *pNewEnum);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Add )( 
            IStartPages __RPC_FAR * This,
            /* [in] */ BSTR bstrURL,
            /* [retval][out] */ VARIANT __RPC_FAR *pStartPageVar);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Remove )( 
            IStartPages __RPC_FAR * This,
            /* [in] */ BSTR bstrURL);
        
        END_INTERFACE
    } IStartPagesVtbl;

    interface IStartPages
    {
        CONST_VTBL struct IStartPagesVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IStartPages_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IStartPages_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IStartPages_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IStartPages_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IStartPages_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IStartPages_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IStartPages_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IStartPages_get_Count(This,pCount)	\
    (This)->lpVtbl -> get_Count(This,pCount)

#define IStartPages_get_Item(This,bstrURL,pStartPageVar)	\
    (This)->lpVtbl -> get_Item(This,bstrURL,pStartPageVar)

#define IStartPages_get__NewEnum(This,pNewEnum)	\
    (This)->lpVtbl -> get__NewEnum(This,pNewEnum)

#define IStartPages_Add(This,bstrURL,pStartPageVar)	\
    (This)->lpVtbl -> Add(This,bstrURL,pStartPageVar)

#define IStartPages_Remove(This,bstrURL)	\
    (This)->lpVtbl -> Remove(This,bstrURL)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [propget] */ HRESULT STDMETHODCALLTYPE IStartPages_get_Count_Proxy( 
    IStartPages __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *pCount);


void __RPC_STUB IStartPages_get_Count_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propget] */ HRESULT STDMETHODCALLTYPE IStartPages_get_Item_Proxy( 
    IStartPages __RPC_FAR * This,
    /* [in] */ BSTR bstrURL,
    /* [retval][out] */ VARIANT __RPC_FAR *pStartPageVar);


void __RPC_STUB IStartPages_get_Item_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][restricted][propget] */ HRESULT STDMETHODCALLTYPE IStartPages_get__NewEnum_Proxy( 
    IStartPages __RPC_FAR * This,
    /* [retval][out] */ IUnknown __RPC_FAR *__RPC_FAR *pNewEnum);


void __RPC_STUB IStartPages_get__NewEnum_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IStartPages_Add_Proxy( 
    IStartPages __RPC_FAR * This,
    /* [in] */ BSTR bstrURL,
    /* [retval][out] */ VARIANT __RPC_FAR *pStartPageVar);


void __RPC_STUB IStartPages_Add_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IStartPages_Remove_Proxy( 
    IStartPages __RPC_FAR * This,
    /* [in] */ BSTR bstrURL);


void __RPC_STUB IStartPages_Remove_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IStartPages_INTERFACE_DEFINED__ */


#ifndef __IStartPage_INTERFACE_DEFINED__
#define __IStartPage_INTERFACE_DEFINED__

/****************************************
 * Generated header for interface: IStartPage
 * at Wed Apr 22 19:58:52 1998
 * using MIDL 3.01.75
 ****************************************/
/* [unique][helpstring][dual][uuid][public][object] */ 



EXTERN_C const IID IID_IStartPage;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    interface DECLSPEC_UUID("0b63e32d-9ccc-11d0-bcdb-00805fccce04")
    IStartPage : public IDispatch
    {
    public:
        virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_URL( 
            /* [retval][out] */ BSTR __RPC_FAR *pbstrURL) = 0;
        
        virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_HostDepth( 
            /* [retval][out] */ LONG __RPC_FAR *plHostDepth) = 0;
        
        virtual /* [propput] */ HRESULT STDMETHODCALLTYPE put_HostDepth( 
            /* [in] */ LONG lHostDepth) = 0;
        
        virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_EnumerationDepth( 
            /* [retval][out] */ LONG __RPC_FAR *plEnumerationDepth) = 0;
        
        virtual /* [propput] */ HRESULT STDMETHODCALLTYPE put_EnumerationDepth( 
            /* [in] */ LONG lEnumerationDepth) = 0;
        
        virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_FollowDirectories( 
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pfFollowDirectories) = 0;
        
        virtual /* [propput] */ HRESULT STDMETHODCALLTYPE put_FollowDirectories( 
            /* [in] */ VARIANT_BOOL fFollowDirectories) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IStartPageVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IStartPage __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IStartPage __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IStartPage __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            IStartPage __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            IStartPage __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            IStartPage __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            IStartPage __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_URL )( 
            IStartPage __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pbstrURL);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_HostDepth )( 
            IStartPage __RPC_FAR * This,
            /* [retval][out] */ LONG __RPC_FAR *plHostDepth);
        
        /* [propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_HostDepth )( 
            IStartPage __RPC_FAR * This,
            /* [in] */ LONG lHostDepth);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_EnumerationDepth )( 
            IStartPage __RPC_FAR * This,
            /* [retval][out] */ LONG __RPC_FAR *plEnumerationDepth);
        
        /* [propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_EnumerationDepth )( 
            IStartPage __RPC_FAR * This,
            /* [in] */ LONG lEnumerationDepth);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_FollowDirectories )( 
            IStartPage __RPC_FAR * This,
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pfFollowDirectories);
        
        /* [propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_FollowDirectories )( 
            IStartPage __RPC_FAR * This,
            /* [in] */ VARIANT_BOOL fFollowDirectories);
        
        END_INTERFACE
    } IStartPageVtbl;

    interface IStartPage
    {
        CONST_VTBL struct IStartPageVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IStartPage_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IStartPage_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IStartPage_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IStartPage_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IStartPage_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IStartPage_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IStartPage_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IStartPage_get_URL(This,pbstrURL)	\
    (This)->lpVtbl -> get_URL(This,pbstrURL)

#define IStartPage_get_HostDepth(This,plHostDepth)	\
    (This)->lpVtbl -> get_HostDepth(This,plHostDepth)

#define IStartPage_put_HostDepth(This,lHostDepth)	\
    (This)->lpVtbl -> put_HostDepth(This,lHostDepth)

#define IStartPage_get_EnumerationDepth(This,plEnumerationDepth)	\
    (This)->lpVtbl -> get_EnumerationDepth(This,plEnumerationDepth)

#define IStartPage_put_EnumerationDepth(This,lEnumerationDepth)	\
    (This)->lpVtbl -> put_EnumerationDepth(This,lEnumerationDepth)

#define IStartPage_get_FollowDirectories(This,pfFollowDirectories)	\
    (This)->lpVtbl -> get_FollowDirectories(This,pfFollowDirectories)

#define IStartPage_put_FollowDirectories(This,fFollowDirectories)	\
    (This)->lpVtbl -> put_FollowDirectories(This,fFollowDirectories)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [propget] */ HRESULT STDMETHODCALLTYPE IStartPage_get_URL_Proxy( 
    IStartPage __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pbstrURL);


void __RPC_STUB IStartPage_get_URL_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget] */ HRESULT STDMETHODCALLTYPE IStartPage_get_HostDepth_Proxy( 
    IStartPage __RPC_FAR * This,
    /* [retval][out] */ LONG __RPC_FAR *plHostDepth);


void __RPC_STUB IStartPage_get_HostDepth_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propput] */ HRESULT STDMETHODCALLTYPE IStartPage_put_HostDepth_Proxy( 
    IStartPage __RPC_FAR * This,
    /* [in] */ LONG lHostDepth);


void __RPC_STUB IStartPage_put_HostDepth_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget] */ HRESULT STDMETHODCALLTYPE IStartPage_get_EnumerationDepth_Proxy( 
    IStartPage __RPC_FAR * This,
    /* [retval][out] */ LONG __RPC_FAR *plEnumerationDepth);


void __RPC_STUB IStartPage_get_EnumerationDepth_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propput] */ HRESULT STDMETHODCALLTYPE IStartPage_put_EnumerationDepth_Proxy( 
    IStartPage __RPC_FAR * This,
    /* [in] */ LONG lEnumerationDepth);


void __RPC_STUB IStartPage_put_EnumerationDepth_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget] */ HRESULT STDMETHODCALLTYPE IStartPage_get_FollowDirectories_Proxy( 
    IStartPage __RPC_FAR * This,
    /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pfFollowDirectories);


void __RPC_STUB IStartPage_get_FollowDirectories_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propput] */ HRESULT STDMETHODCALLTYPE IStartPage_put_FollowDirectories_Proxy( 
    IStartPage __RPC_FAR * This,
    /* [in] */ VARIANT_BOOL fFollowDirectories);


void __RPC_STUB IStartPage_put_FollowDirectories_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IStartPage_INTERFACE_DEFINED__ */


#ifndef __IMappings_INTERFACE_DEFINED__
#define __IMappings_INTERFACE_DEFINED__

/****************************************
 * Generated header for interface: IMappings
 * at Wed Apr 22 19:58:52 1998
 * using MIDL 3.01.75
 ****************************************/
/* [unique][helpstring][dual][uuid][public][object] */ 



EXTERN_C const IID IID_IMappings;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    interface DECLSPEC_UUID("0b63e32e-9ccc-11d0-bcdb-00805fccce04")
    IMappings : public IDispatch
    {
    public:
        virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_Count( 
            /* [retval][out] */ long __RPC_FAR *pCount) = 0;
        
        virtual /* [id][propget] */ HRESULT STDMETHODCALLTYPE get_Item( 
            /* [in] */ BSTR bstrName,
            /* [retval][out] */ VARIANT __RPC_FAR *pMapping) = 0;
        
        virtual /* [id][restricted][propget] */ HRESULT STDMETHODCALLTYPE get__NewEnum( 
            /* [retval][out] */ IUnknown __RPC_FAR *__RPC_FAR *pNewEnum) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Add( 
            /* [in] */ BSTR bstrFrom,
            /* [in] */ BSTR bstrTo,
            /* [retval][out] */ VARIANT __RPC_FAR *pMappingVar) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Remove( 
            /* [in] */ BSTR bstrFrom) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IMappingsVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IMappings __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IMappings __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IMappings __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            IMappings __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            IMappings __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            IMappings __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            IMappings __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Count )( 
            IMappings __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *pCount);
        
        /* [id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Item )( 
            IMappings __RPC_FAR * This,
            /* [in] */ BSTR bstrName,
            /* [retval][out] */ VARIANT __RPC_FAR *pMapping);
        
        /* [id][restricted][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get__NewEnum )( 
            IMappings __RPC_FAR * This,
            /* [retval][out] */ IUnknown __RPC_FAR *__RPC_FAR *pNewEnum);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Add )( 
            IMappings __RPC_FAR * This,
            /* [in] */ BSTR bstrFrom,
            /* [in] */ BSTR bstrTo,
            /* [retval][out] */ VARIANT __RPC_FAR *pMappingVar);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Remove )( 
            IMappings __RPC_FAR * This,
            /* [in] */ BSTR bstrFrom);
        
        END_INTERFACE
    } IMappingsVtbl;

    interface IMappings
    {
        CONST_VTBL struct IMappingsVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IMappings_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IMappings_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IMappings_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IMappings_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IMappings_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IMappings_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IMappings_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IMappings_get_Count(This,pCount)	\
    (This)->lpVtbl -> get_Count(This,pCount)

#define IMappings_get_Item(This,bstrName,pMapping)	\
    (This)->lpVtbl -> get_Item(This,bstrName,pMapping)

#define IMappings_get__NewEnum(This,pNewEnum)	\
    (This)->lpVtbl -> get__NewEnum(This,pNewEnum)

#define IMappings_Add(This,bstrFrom,bstrTo,pMappingVar)	\
    (This)->lpVtbl -> Add(This,bstrFrom,bstrTo,pMappingVar)

#define IMappings_Remove(This,bstrFrom)	\
    (This)->lpVtbl -> Remove(This,bstrFrom)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [propget] */ HRESULT STDMETHODCALLTYPE IMappings_get_Count_Proxy( 
    IMappings __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *pCount);


void __RPC_STUB IMappings_get_Count_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propget] */ HRESULT STDMETHODCALLTYPE IMappings_get_Item_Proxy( 
    IMappings __RPC_FAR * This,
    /* [in] */ BSTR bstrName,
    /* [retval][out] */ VARIANT __RPC_FAR *pMapping);


void __RPC_STUB IMappings_get_Item_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][restricted][propget] */ HRESULT STDMETHODCALLTYPE IMappings_get__NewEnum_Proxy( 
    IMappings __RPC_FAR * This,
    /* [retval][out] */ IUnknown __RPC_FAR *__RPC_FAR *pNewEnum);


void __RPC_STUB IMappings_get__NewEnum_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IMappings_Add_Proxy( 
    IMappings __RPC_FAR * This,
    /* [in] */ BSTR bstrFrom,
    /* [in] */ BSTR bstrTo,
    /* [retval][out] */ VARIANT __RPC_FAR *pMappingVar);


void __RPC_STUB IMappings_Add_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IMappings_Remove_Proxy( 
    IMappings __RPC_FAR * This,
    /* [in] */ BSTR bstrFrom);


void __RPC_STUB IMappings_Remove_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IMappings_INTERFACE_DEFINED__ */


#ifndef __IMapping_INTERFACE_DEFINED__
#define __IMapping_INTERFACE_DEFINED__

/****************************************
 * Generated header for interface: IMapping
 * at Wed Apr 22 19:58:52 1998
 * using MIDL 3.01.75
 ****************************************/
/* [unique][helpstring][dual][uuid][public][object] */ 



EXTERN_C const IID IID_IMapping;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    interface DECLSPEC_UUID("0b63e32f-9ccc-11d0-bcdb-00805fccce04")
    IMapping : public IDispatch
    {
    public:
        virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_From( 
            /* [retval][out] */ BSTR __RPC_FAR *pbstrFrom) = 0;
        
        virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_To( 
            /* [retval][out] */ BSTR __RPC_FAR *pbstrTo) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IMappingVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IMapping __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IMapping __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IMapping __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            IMapping __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            IMapping __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            IMapping __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            IMapping __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_From )( 
            IMapping __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pbstrFrom);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_To )( 
            IMapping __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pbstrTo);
        
        END_INTERFACE
    } IMappingVtbl;

    interface IMapping
    {
        CONST_VTBL struct IMappingVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IMapping_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IMapping_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IMapping_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IMapping_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IMapping_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IMapping_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IMapping_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IMapping_get_From(This,pbstrFrom)	\
    (This)->lpVtbl -> get_From(This,pbstrFrom)

#define IMapping_get_To(This,pbstrTo)	\
    (This)->lpVtbl -> get_To(This,pbstrTo)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [propget] */ HRESULT STDMETHODCALLTYPE IMapping_get_From_Proxy( 
    IMapping __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pbstrFrom);


void __RPC_STUB IMapping_get_From_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget] */ HRESULT STDMETHODCALLTYPE IMapping_get_To_Proxy( 
    IMapping __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pbstrTo);


void __RPC_STUB IMapping_get_To_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IMapping_INTERFACE_DEFINED__ */


#ifndef __IProtocols_INTERFACE_DEFINED__
#define __IProtocols_INTERFACE_DEFINED__

/****************************************
 * Generated header for interface: IProtocols
 * at Wed Apr 22 19:58:52 1998
 * using MIDL 3.01.75
 ****************************************/
/* [unique][helpstring][dual][uuid][public][object] */ 



EXTERN_C const IID IID_IProtocols;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    interface DECLSPEC_UUID("0b63e330-9ccc-11d0-bcdb-00805fccce04")
    IProtocols : public IDispatch
    {
    public:
        virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_Count( 
            /* [retval][out] */ long __RPC_FAR *pCount) = 0;
        
        virtual /* [id][propget] */ HRESULT STDMETHODCALLTYPE get_Item( 
            /* [in] */ BSTR bstrUrlName,
            /* [retval][out] */ VARIANT __RPC_FAR *pProtocol) = 0;
        
        virtual /* [id][restricted][propget] */ HRESULT STDMETHODCALLTYPE get__NewEnum( 
            /* [retval][out] */ IUnknown __RPC_FAR *__RPC_FAR *pNewEnum) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IProtocolsVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IProtocols __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IProtocols __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IProtocols __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            IProtocols __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            IProtocols __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            IProtocols __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            IProtocols __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Count )( 
            IProtocols __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *pCount);
        
        /* [id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Item )( 
            IProtocols __RPC_FAR * This,
            /* [in] */ BSTR bstrUrlName,
            /* [retval][out] */ VARIANT __RPC_FAR *pProtocol);
        
        /* [id][restricted][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get__NewEnum )( 
            IProtocols __RPC_FAR * This,
            /* [retval][out] */ IUnknown __RPC_FAR *__RPC_FAR *pNewEnum);
        
        END_INTERFACE
    } IProtocolsVtbl;

    interface IProtocols
    {
        CONST_VTBL struct IProtocolsVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IProtocols_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IProtocols_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IProtocols_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IProtocols_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IProtocols_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IProtocols_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IProtocols_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IProtocols_get_Count(This,pCount)	\
    (This)->lpVtbl -> get_Count(This,pCount)

#define IProtocols_get_Item(This,bstrUrlName,pProtocol)	\
    (This)->lpVtbl -> get_Item(This,bstrUrlName,pProtocol)

#define IProtocols_get__NewEnum(This,pNewEnum)	\
    (This)->lpVtbl -> get__NewEnum(This,pNewEnum)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [propget] */ HRESULT STDMETHODCALLTYPE IProtocols_get_Count_Proxy( 
    IProtocols __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *pCount);


void __RPC_STUB IProtocols_get_Count_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propget] */ HRESULT STDMETHODCALLTYPE IProtocols_get_Item_Proxy( 
    IProtocols __RPC_FAR * This,
    /* [in] */ BSTR bstrUrlName,
    /* [retval][out] */ VARIANT __RPC_FAR *pProtocol);


void __RPC_STUB IProtocols_get_Item_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][restricted][propget] */ HRESULT STDMETHODCALLTYPE IProtocols_get__NewEnum_Proxy( 
    IProtocols __RPC_FAR * This,
    /* [retval][out] */ IUnknown __RPC_FAR *__RPC_FAR *pNewEnum);


void __RPC_STUB IProtocols_get__NewEnum_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IProtocols_INTERFACE_DEFINED__ */


#ifndef __IProtocol_INTERFACE_DEFINED__
#define __IProtocol_INTERFACE_DEFINED__

/****************************************
 * Generated header for interface: IProtocol
 * at Wed Apr 22 19:58:52 1998
 * using MIDL 3.01.75
 ****************************************/
/* [unique][helpstring][dual][uuid][public][object] */ 



EXTERN_C const IID IID_IProtocol;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    interface DECLSPEC_UUID("0b63e331-9ccc-11d0-bcdb-00805fccce04")
    IProtocol : public IDispatch
    {
    public:
        virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_UrlName( 
            /* [retval][out] */ BSTR __RPC_FAR *pbstrUrlName) = 0;
        
        virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_ProgIdHandler( 
            /* [retval][out] */ BSTR __RPC_FAR *pbstrProgIdHandler) = 0;
        
        virtual /* [propput] */ HRESULT STDMETHODCALLTYPE put_Included( 
            /* [in] */ VARIANT_BOOL fIncluded) = 0;
        
        virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_Included( 
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pfIncluded) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IProtocolVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IProtocol __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IProtocol __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IProtocol __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            IProtocol __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            IProtocol __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            IProtocol __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            IProtocol __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_UrlName )( 
            IProtocol __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pbstrUrlName);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_ProgIdHandler )( 
            IProtocol __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pbstrProgIdHandler);
        
        /* [propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Included )( 
            IProtocol __RPC_FAR * This,
            /* [in] */ VARIANT_BOOL fIncluded);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Included )( 
            IProtocol __RPC_FAR * This,
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pfIncluded);
        
        END_INTERFACE
    } IProtocolVtbl;

    interface IProtocol
    {
        CONST_VTBL struct IProtocolVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IProtocol_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IProtocol_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IProtocol_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IProtocol_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IProtocol_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IProtocol_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IProtocol_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IProtocol_get_UrlName(This,pbstrUrlName)	\
    (This)->lpVtbl -> get_UrlName(This,pbstrUrlName)

#define IProtocol_get_ProgIdHandler(This,pbstrProgIdHandler)	\
    (This)->lpVtbl -> get_ProgIdHandler(This,pbstrProgIdHandler)

#define IProtocol_put_Included(This,fIncluded)	\
    (This)->lpVtbl -> put_Included(This,fIncluded)

#define IProtocol_get_Included(This,pfIncluded)	\
    (This)->lpVtbl -> get_Included(This,pfIncluded)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [propget] */ HRESULT STDMETHODCALLTYPE IProtocol_get_UrlName_Proxy( 
    IProtocol __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pbstrUrlName);


void __RPC_STUB IProtocol_get_UrlName_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget] */ HRESULT STDMETHODCALLTYPE IProtocol_get_ProgIdHandler_Proxy( 
    IProtocol __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pbstrProgIdHandler);


void __RPC_STUB IProtocol_get_ProgIdHandler_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propput] */ HRESULT STDMETHODCALLTYPE IProtocol_put_Included_Proxy( 
    IProtocol __RPC_FAR * This,
    /* [in] */ VARIANT_BOOL fIncluded);


void __RPC_STUB IProtocol_put_Included_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget] */ HRESULT STDMETHODCALLTYPE IProtocol_get_Included_Proxy( 
    IProtocol __RPC_FAR * This,
    /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pfIncluded);


void __RPC_STUB IProtocol_get_Included_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IProtocol_INTERFACE_DEFINED__ */


#ifndef __IGatherLog_INTERFACE_DEFINED__
#define __IGatherLog_INTERFACE_DEFINED__

/****************************************
 * Generated header for interface: IGatherLog
 * at Wed Apr 22 19:58:52 1998
 * using MIDL 3.01.75
 ****************************************/
/* [unique][helpstring][dual][uuid][public][object] */ 



EXTERN_C const IID IID_IGatherLog;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    interface DECLSPEC_UUID("0b63e372-9ccc-11d0-bcdb-00805fccce04")
    IGatherLog : public IDispatch
    {
    public:
        virtual /* [id][propget] */ HRESULT STDMETHODCALLTYPE get_Name( 
            /* [retval][out] */ BSTR __RPC_FAR *pbsLogFileName) = 0;
        
        virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_Created( 
            /* [retval][out] */ DATE __RPC_FAR *pdtCreated) = 0;
        
        virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_Size( 
            /* [retval][out] */ double __RPC_FAR *pdSize) = 0;
        
        virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_Path( 
            /* [retval][out] */ BSTR __RPC_FAR *pbsLogFilePath) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IGatherLogVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IGatherLog __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IGatherLog __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IGatherLog __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            IGatherLog __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            IGatherLog __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            IGatherLog __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            IGatherLog __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        /* [id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Name )( 
            IGatherLog __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pbsLogFileName);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Created )( 
            IGatherLog __RPC_FAR * This,
            /* [retval][out] */ DATE __RPC_FAR *pdtCreated);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Size )( 
            IGatherLog __RPC_FAR * This,
            /* [retval][out] */ double __RPC_FAR *pdSize);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Path )( 
            IGatherLog __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pbsLogFilePath);
        
        END_INTERFACE
    } IGatherLogVtbl;

    interface IGatherLog
    {
        CONST_VTBL struct IGatherLogVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IGatherLog_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IGatherLog_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IGatherLog_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IGatherLog_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IGatherLog_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IGatherLog_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IGatherLog_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IGatherLog_get_Name(This,pbsLogFileName)	\
    (This)->lpVtbl -> get_Name(This,pbsLogFileName)

#define IGatherLog_get_Created(This,pdtCreated)	\
    (This)->lpVtbl -> get_Created(This,pdtCreated)

#define IGatherLog_get_Size(This,pdSize)	\
    (This)->lpVtbl -> get_Size(This,pdSize)

#define IGatherLog_get_Path(This,pbsLogFilePath)	\
    (This)->lpVtbl -> get_Path(This,pbsLogFilePath)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [id][propget] */ HRESULT STDMETHODCALLTYPE IGatherLog_get_Name_Proxy( 
    IGatherLog __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pbsLogFileName);


void __RPC_STUB IGatherLog_get_Name_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget] */ HRESULT STDMETHODCALLTYPE IGatherLog_get_Created_Proxy( 
    IGatherLog __RPC_FAR * This,
    /* [retval][out] */ DATE __RPC_FAR *pdtCreated);


void __RPC_STUB IGatherLog_get_Created_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget] */ HRESULT STDMETHODCALLTYPE IGatherLog_get_Size_Proxy( 
    IGatherLog __RPC_FAR * This,
    /* [retval][out] */ double __RPC_FAR *pdSize);


void __RPC_STUB IGatherLog_get_Size_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget] */ HRESULT STDMETHODCALLTYPE IGatherLog_get_Path_Proxy( 
    IGatherLog __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pbsLogFilePath);


void __RPC_STUB IGatherLog_get_Path_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IGatherLog_INTERFACE_DEFINED__ */


#ifndef __IGatherLogs_INTERFACE_DEFINED__
#define __IGatherLogs_INTERFACE_DEFINED__

/****************************************
 * Generated header for interface: IGatherLogs
 * at Wed Apr 22 19:58:52 1998
 * using MIDL 3.01.75
 ****************************************/
/* [unique][helpstring][dual][uuid][public][object] */ 



EXTERN_C const IID IID_IGatherLogs;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    interface DECLSPEC_UUID("0b63e371-9ccc-11d0-bcdb-00805fccce04")
    IGatherLogs : public IDispatch
    {
    public:
        virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_Count( 
            /* [retval][out] */ long __RPC_FAR *pCount) = 0;
        
        virtual /* [id][restricted][propget] */ HRESULT STDMETHODCALLTYPE get__NewEnum( 
            /* [retval][out] */ IUnknown __RPC_FAR *__RPC_FAR *pNewEnum) = 0;
        
        virtual /* [id][propget] */ HRESULT STDMETHODCALLTYPE get_Item( 
            /* [in] */ BSTR bstrName,
            /* [retval][out] */ VARIANT __RPC_FAR *pLogVar) = 0;
        
        virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_MaxLogs( 
            /* [retval][out] */ LONG __RPC_FAR *plMaxLogs) = 0;
        
        virtual /* [propput] */ HRESULT STDMETHODCALLTYPE put_MaxLogs( 
            /* [in] */ LONG lMaxLogs) = 0;
        
        virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_CurrentLog( 
            /* [retval][out] */ VARIANT __RPC_FAR *pCurrentLogVar) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IGatherLogsVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IGatherLogs __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IGatherLogs __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IGatherLogs __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            IGatherLogs __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            IGatherLogs __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            IGatherLogs __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            IGatherLogs __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Count )( 
            IGatherLogs __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *pCount);
        
        /* [id][restricted][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get__NewEnum )( 
            IGatherLogs __RPC_FAR * This,
            /* [retval][out] */ IUnknown __RPC_FAR *__RPC_FAR *pNewEnum);
        
        /* [id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Item )( 
            IGatherLogs __RPC_FAR * This,
            /* [in] */ BSTR bstrName,
            /* [retval][out] */ VARIANT __RPC_FAR *pLogVar);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_MaxLogs )( 
            IGatherLogs __RPC_FAR * This,
            /* [retval][out] */ LONG __RPC_FAR *plMaxLogs);
        
        /* [propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_MaxLogs )( 
            IGatherLogs __RPC_FAR * This,
            /* [in] */ LONG lMaxLogs);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_CurrentLog )( 
            IGatherLogs __RPC_FAR * This,
            /* [retval][out] */ VARIANT __RPC_FAR *pCurrentLogVar);
        
        END_INTERFACE
    } IGatherLogsVtbl;

    interface IGatherLogs
    {
        CONST_VTBL struct IGatherLogsVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IGatherLogs_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IGatherLogs_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IGatherLogs_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IGatherLogs_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IGatherLogs_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IGatherLogs_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IGatherLogs_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IGatherLogs_get_Count(This,pCount)	\
    (This)->lpVtbl -> get_Count(This,pCount)

#define IGatherLogs_get__NewEnum(This,pNewEnum)	\
    (This)->lpVtbl -> get__NewEnum(This,pNewEnum)

#define IGatherLogs_get_Item(This,bstrName,pLogVar)	\
    (This)->lpVtbl -> get_Item(This,bstrName,pLogVar)

#define IGatherLogs_get_MaxLogs(This,plMaxLogs)	\
    (This)->lpVtbl -> get_MaxLogs(This,plMaxLogs)

#define IGatherLogs_put_MaxLogs(This,lMaxLogs)	\
    (This)->lpVtbl -> put_MaxLogs(This,lMaxLogs)

#define IGatherLogs_get_CurrentLog(This,pCurrentLogVar)	\
    (This)->lpVtbl -> get_CurrentLog(This,pCurrentLogVar)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [propget] */ HRESULT STDMETHODCALLTYPE IGatherLogs_get_Count_Proxy( 
    IGatherLogs __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *pCount);


void __RPC_STUB IGatherLogs_get_Count_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][restricted][propget] */ HRESULT STDMETHODCALLTYPE IGatherLogs_get__NewEnum_Proxy( 
    IGatherLogs __RPC_FAR * This,
    /* [retval][out] */ IUnknown __RPC_FAR *__RPC_FAR *pNewEnum);


void __RPC_STUB IGatherLogs_get__NewEnum_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propget] */ HRESULT STDMETHODCALLTYPE IGatherLogs_get_Item_Proxy( 
    IGatherLogs __RPC_FAR * This,
    /* [in] */ BSTR bstrName,
    /* [retval][out] */ VARIANT __RPC_FAR *pLogVar);


void __RPC_STUB IGatherLogs_get_Item_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget] */ HRESULT STDMETHODCALLTYPE IGatherLogs_get_MaxLogs_Proxy( 
    IGatherLogs __RPC_FAR * This,
    /* [retval][out] */ LONG __RPC_FAR *plMaxLogs);


void __RPC_STUB IGatherLogs_get_MaxLogs_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propput] */ HRESULT STDMETHODCALLTYPE IGatherLogs_put_MaxLogs_Proxy( 
    IGatherLogs __RPC_FAR * This,
    /* [in] */ LONG lMaxLogs);


void __RPC_STUB IGatherLogs_put_MaxLogs_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget] */ HRESULT STDMETHODCALLTYPE IGatherLogs_get_CurrentLog_Proxy( 
    IGatherLogs __RPC_FAR * This,
    /* [retval][out] */ VARIANT __RPC_FAR *pCurrentLogVar);


void __RPC_STUB IGatherLogs_get_CurrentLog_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IGatherLogs_INTERFACE_DEFINED__ */


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
