/* this ALWAYS GENERATED file contains the definitions for the interfaces */


/* File created by MIDL compiler version 3.01.75 */
/* at Wed Apr 22 20:00:57 1998
 */
/* Compiler settings for idxadminsdk.idl:
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

#ifndef __idxadminsdk_h__
#define __idxadminsdk_h__

#ifdef __cplusplus
extern "C"{
#endif 

/* Forward Declarations */ 

#ifndef __IIndexDs_FWD_DEFINED__
#define __IIndexDs_FWD_DEFINED__
typedef interface IIndexDs IIndexDs;
#endif 	/* __IIndexDs_FWD_DEFINED__ */


#ifndef __ICatalogSearchServerList_FWD_DEFINED__
#define __ICatalogSearchServerList_FWD_DEFINED__
typedef interface ICatalogSearchServerList ICatalogSearchServerList;
#endif 	/* __ICatalogSearchServerList_FWD_DEFINED__ */


/* header files for imported files */
#include "oaidl.h"

void __RPC_FAR * __RPC_USER MIDL_user_allocate(size_t);
void __RPC_USER MIDL_user_free( void __RPC_FAR * ); 

#ifndef __IIndexDs_INTERFACE_DEFINED__
#define __IIndexDs_INTERFACE_DEFINED__

/****************************************
 * Generated header for interface: IIndexDs
 * at Wed Apr 22 20:00:57 1998
 * using MIDL 3.01.75
 ****************************************/
/* [unique][helpstring][dual][uuid][public][object] */ 



EXTERN_C const IID IID_IIndexDs;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    interface DECLSPEC_UUID("798e43e0-c659-11d0-8f55-00c04fb67f46")
    IIndexDs : public IDispatch
    {
    public:
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_Name( 
            /* [retval][out] */ BSTR __RPC_FAR *pbstrName) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_PropDestination( 
            /* [retval][out] */ BSTR __RPC_FAR *pbstrDestination) = 0;
        
        virtual /* [helpstring][propput] */ HRESULT STDMETHODCALLTYPE put_PropDestination( 
            /* [in] */ BSTR bstrDestination) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IIndexDsVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IIndexDs __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IIndexDs __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IIndexDs __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            IIndexDs __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            IIndexDs __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            IIndexDs __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            IIndexDs __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Name )( 
            IIndexDs __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pbstrName);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_PropDestination )( 
            IIndexDs __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pbstrDestination);
        
        /* [helpstring][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_PropDestination )( 
            IIndexDs __RPC_FAR * This,
            /* [in] */ BSTR bstrDestination);
        
        END_INTERFACE
    } IIndexDsVtbl;

    interface IIndexDs
    {
        CONST_VTBL struct IIndexDsVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IIndexDs_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IIndexDs_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IIndexDs_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IIndexDs_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IIndexDs_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IIndexDs_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IIndexDs_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IIndexDs_get_Name(This,pbstrName)	\
    (This)->lpVtbl -> get_Name(This,pbstrName)

#define IIndexDs_get_PropDestination(This,pbstrDestination)	\
    (This)->lpVtbl -> get_PropDestination(This,pbstrDestination)

#define IIndexDs_put_PropDestination(This,bstrDestination)	\
    (This)->lpVtbl -> put_PropDestination(This,bstrDestination)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE IIndexDs_get_Name_Proxy( 
    IIndexDs __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pbstrName);


void __RPC_STUB IIndexDs_get_Name_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE IIndexDs_get_PropDestination_Proxy( 
    IIndexDs __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pbstrDestination);


void __RPC_STUB IIndexDs_get_PropDestination_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propput] */ HRESULT STDMETHODCALLTYPE IIndexDs_put_PropDestination_Proxy( 
    IIndexDs __RPC_FAR * This,
    /* [in] */ BSTR bstrDestination);


void __RPC_STUB IIndexDs_put_PropDestination_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IIndexDs_INTERFACE_DEFINED__ */


#ifndef __ICatalogSearchServerList_INTERFACE_DEFINED__
#define __ICatalogSearchServerList_INTERFACE_DEFINED__

/****************************************
 * Generated header for interface: ICatalogSearchServerList
 * at Wed Apr 22 20:00:57 1998
 * using MIDL 3.01.75
 ****************************************/
/* [unique][helpstring][dual][uuid][public][object] */ 



EXTERN_C const IID IID_ICatalogSearchServerList;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    interface DECLSPEC_UUID("549b5add-c1d1-11d0-8107-00c04fc29723")
    ICatalogSearchServerList : public IDispatch
    {
    public:
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_Count( 
            /* [retval][out] */ long __RPC_FAR *pCount) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Item( 
            /* [in] */ BSTR bstrName,
            /* [retval][out] */ VARIANT __RPC_FAR *pCatalog) = 0;
        
        virtual /* [helpstring][id][restricted][propget] */ HRESULT STDMETHODCALLTYPE get__NewEnum( 
            /* [retval][out] */ IUnknown __RPC_FAR *__RPC_FAR *pNewEnum) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Add( 
            /* [in] */ BSTR bstrServer,
            /* [retval][out] */ VARIANT __RPC_FAR *pIndexDs) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Remove( 
            /* [in] */ BSTR bstrServer) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct ICatalogSearchServerListVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            ICatalogSearchServerList __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            ICatalogSearchServerList __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            ICatalogSearchServerList __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            ICatalogSearchServerList __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            ICatalogSearchServerList __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            ICatalogSearchServerList __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            ICatalogSearchServerList __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Count )( 
            ICatalogSearchServerList __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *pCount);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Item )( 
            ICatalogSearchServerList __RPC_FAR * This,
            /* [in] */ BSTR bstrName,
            /* [retval][out] */ VARIANT __RPC_FAR *pCatalog);
        
        /* [helpstring][id][restricted][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get__NewEnum )( 
            ICatalogSearchServerList __RPC_FAR * This,
            /* [retval][out] */ IUnknown __RPC_FAR *__RPC_FAR *pNewEnum);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Add )( 
            ICatalogSearchServerList __RPC_FAR * This,
            /* [in] */ BSTR bstrServer,
            /* [retval][out] */ VARIANT __RPC_FAR *pIndexDs);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Remove )( 
            ICatalogSearchServerList __RPC_FAR * This,
            /* [in] */ BSTR bstrServer);
        
        END_INTERFACE
    } ICatalogSearchServerListVtbl;

    interface ICatalogSearchServerList
    {
        CONST_VTBL struct ICatalogSearchServerListVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define ICatalogSearchServerList_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define ICatalogSearchServerList_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define ICatalogSearchServerList_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define ICatalogSearchServerList_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define ICatalogSearchServerList_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define ICatalogSearchServerList_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define ICatalogSearchServerList_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define ICatalogSearchServerList_get_Count(This,pCount)	\
    (This)->lpVtbl -> get_Count(This,pCount)

#define ICatalogSearchServerList_get_Item(This,bstrName,pCatalog)	\
    (This)->lpVtbl -> get_Item(This,bstrName,pCatalog)

#define ICatalogSearchServerList_get__NewEnum(This,pNewEnum)	\
    (This)->lpVtbl -> get__NewEnum(This,pNewEnum)

#define ICatalogSearchServerList_Add(This,bstrServer,pIndexDs)	\
    (This)->lpVtbl -> Add(This,bstrServer,pIndexDs)

#define ICatalogSearchServerList_Remove(This,bstrServer)	\
    (This)->lpVtbl -> Remove(This,bstrServer)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE ICatalogSearchServerList_get_Count_Proxy( 
    ICatalogSearchServerList __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *pCount);


void __RPC_STUB ICatalogSearchServerList_get_Count_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE ICatalogSearchServerList_get_Item_Proxy( 
    ICatalogSearchServerList __RPC_FAR * This,
    /* [in] */ BSTR bstrName,
    /* [retval][out] */ VARIANT __RPC_FAR *pCatalog);


void __RPC_STUB ICatalogSearchServerList_get_Item_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][restricted][propget] */ HRESULT STDMETHODCALLTYPE ICatalogSearchServerList_get__NewEnum_Proxy( 
    ICatalogSearchServerList __RPC_FAR * This,
    /* [retval][out] */ IUnknown __RPC_FAR *__RPC_FAR *pNewEnum);


void __RPC_STUB ICatalogSearchServerList_get__NewEnum_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ICatalogSearchServerList_Add_Proxy( 
    ICatalogSearchServerList __RPC_FAR * This,
    /* [in] */ BSTR bstrServer,
    /* [retval][out] */ VARIANT __RPC_FAR *pIndexDs);


void __RPC_STUB ICatalogSearchServerList_Add_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ICatalogSearchServerList_Remove_Proxy( 
    ICatalogSearchServerList __RPC_FAR * This,
    /* [in] */ BSTR bstrServer);


void __RPC_STUB ICatalogSearchServerList_Remove_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __ICatalogSearchServerList_INTERFACE_DEFINED__ */


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
