/* this ALWAYS GENERATED file contains the definitions for the interfaces */


/* File created by MIDL compiler version 3.01.75 */
/* at Wed Apr 22 20:03:41 1998
 */
/* Compiler settings for objcreator.idl:
    Os (OptLev=s), W1, Zp8, env=Win32, ms_ext, c_ext
    error checks: none
*/
//@@MIDL_FILE_HEADING(  )
#include "rpc.h"
#include "rpcndr.h"

#ifndef __objcreator_h__
#define __objcreator_h__

#ifdef __cplusplus
extern "C"{
#endif 

/* Forward Declarations */ 

#ifndef __ObjCreator_FWD_DEFINED__
#define __ObjCreator_FWD_DEFINED__

#ifdef __cplusplus
typedef class ObjCreator ObjCreator;
#else
typedef struct ObjCreator ObjCreator;
#endif /* __cplusplus */

#endif 	/* __ObjCreator_FWD_DEFINED__ */


#ifndef __IObjCreator_FWD_DEFINED__
#define __IObjCreator_FWD_DEFINED__
typedef interface IObjCreator IObjCreator;
#endif 	/* __IObjCreator_FWD_DEFINED__ */


#ifndef __IObjCreatorAuth_FWD_DEFINED__
#define __IObjCreatorAuth_FWD_DEFINED__
typedef interface IObjCreatorAuth IObjCreatorAuth;
#endif 	/* __IObjCreatorAuth_FWD_DEFINED__ */


void __RPC_FAR * __RPC_USER MIDL_user_allocate(size_t);
void __RPC_USER MIDL_user_free( void __RPC_FAR * ); 

/****************************************
 * Generated header for interface: __MIDL_itf_objcreator_0000
 * at Wed Apr 22 20:03:41 1998
 * using MIDL 3.01.75
 ****************************************/
/* [local] */ 






extern RPC_IF_HANDLE __MIDL_itf_objcreator_0000_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_objcreator_0000_v0_0_s_ifspec;


#ifndef __CREATORLib_LIBRARY_DEFINED__
#define __CREATORLib_LIBRARY_DEFINED__

/****************************************
 * Generated header for library: CREATORLib
 * at Wed Apr 22 20:03:41 1998
 * using MIDL 3.01.75
 ****************************************/
/* [helpstring][version][uuid] */ 



EXTERN_C const IID LIBID_CREATORLib;

#ifdef __cplusplus
EXTERN_C const CLSID CLSID_ObjCreator;

class DECLSPEC_UUID("C9C419D1-8B6A-11D0-BCD9-00805FCCCE04")
ObjCreator;
#endif

#ifndef __IObjCreator_INTERFACE_DEFINED__
#define __IObjCreator_INTERFACE_DEFINED__

/****************************************
 * Generated header for interface: IObjCreator
 * at Wed Apr 22 20:03:41 1998
 * using MIDL 3.01.75
 ****************************************/
/* [object][oleautomation][dual][helpstring][uuid] */ 



EXTERN_C const IID IID_IObjCreator;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    interface DECLSPEC_UUID("C9C419D0-8B6A-11D0-BCD9-00805FCCCE04")
    IObjCreator : public IDispatch
    {
    public:
        virtual /* [id] */ HRESULT __stdcall CreateObj( 
            /* [in] */ BSTR bstrProgId,
            /* [optional][in] */ BSTR bstrServer,
            /* [retval][out] */ VARIANT __RPC_FAR *pVarObject) = 0;
        
        virtual /* [id] */ HRESULT __stdcall CreateObjAuth( 
            /* [in] */ BSTR bstrProgId,
            /* [optional][in] */ BSTR bstrServer,
            /* [retval][out] */ VARIANT __RPC_FAR *pVarObject) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IObjCreatorVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IObjCreator __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IObjCreator __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IObjCreator __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            IObjCreator __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            IObjCreator __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            IObjCreator __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            IObjCreator __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        /* [id] */ HRESULT ( __stdcall __RPC_FAR *CreateObj )( 
            IObjCreator __RPC_FAR * This,
            /* [in] */ BSTR bstrProgId,
            /* [optional][in] */ BSTR bstrServer,
            /* [retval][out] */ VARIANT __RPC_FAR *pVarObject);
        
        /* [id] */ HRESULT ( __stdcall __RPC_FAR *CreateObjAuth )( 
            IObjCreator __RPC_FAR * This,
            /* [in] */ BSTR bstrProgId,
            /* [optional][in] */ BSTR bstrServer,
            /* [retval][out] */ VARIANT __RPC_FAR *pVarObject);
        
        END_INTERFACE
    } IObjCreatorVtbl;

    interface IObjCreator
    {
        CONST_VTBL struct IObjCreatorVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IObjCreator_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IObjCreator_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IObjCreator_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IObjCreator_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IObjCreator_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IObjCreator_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IObjCreator_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IObjCreator_CreateObj(This,bstrProgId,bstrServer,pVarObject)	\
    (This)->lpVtbl -> CreateObj(This,bstrProgId,bstrServer,pVarObject)

#define IObjCreator_CreateObjAuth(This,bstrProgId,bstrServer,pVarObject)	\
    (This)->lpVtbl -> CreateObjAuth(This,bstrProgId,bstrServer,pVarObject)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [id] */ HRESULT __stdcall IObjCreator_CreateObj_Proxy( 
    IObjCreator __RPC_FAR * This,
    /* [in] */ BSTR bstrProgId,
    /* [optional][in] */ BSTR bstrServer,
    /* [retval][out] */ VARIANT __RPC_FAR *pVarObject);


void __RPC_STUB IObjCreator_CreateObj_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id] */ HRESULT __stdcall IObjCreator_CreateObjAuth_Proxy( 
    IObjCreator __RPC_FAR * This,
    /* [in] */ BSTR bstrProgId,
    /* [optional][in] */ BSTR bstrServer,
    /* [retval][out] */ VARIANT __RPC_FAR *pVarObject);


void __RPC_STUB IObjCreator_CreateObjAuth_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IObjCreator_INTERFACE_DEFINED__ */


#ifndef __IObjCreatorAuth_INTERFACE_DEFINED__
#define __IObjCreatorAuth_INTERFACE_DEFINED__

/****************************************
 * Generated header for interface: IObjCreatorAuth
 * at Wed Apr 22 20:03:41 1998
 * using MIDL 3.01.75
 ****************************************/
/* [object][helpstring][uuid] */ 



EXTERN_C const IID IID_IObjCreatorAuth;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    interface DECLSPEC_UUID("0B63E394-9CCC-11D0-BCDB-00805FCCCE04")
    IObjCreatorAuth : public IUnknown
    {
    public:
        virtual HRESULT __stdcall CreateClsObjAuth( 
            /* [in] */ GUID __RPC_FAR *rClsid,
            /* [in] */ BSTR bstrServer,
            /* [out] */ IUnknown __RPC_FAR *__RPC_FAR *ppUnknown) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IObjCreatorAuthVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IObjCreatorAuth __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IObjCreatorAuth __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IObjCreatorAuth __RPC_FAR * This);
        
        HRESULT ( __stdcall __RPC_FAR *CreateClsObjAuth )( 
            IObjCreatorAuth __RPC_FAR * This,
            /* [in] */ GUID __RPC_FAR *rClsid,
            /* [in] */ BSTR bstrServer,
            /* [out] */ IUnknown __RPC_FAR *__RPC_FAR *ppUnknown);
        
        END_INTERFACE
    } IObjCreatorAuthVtbl;

    interface IObjCreatorAuth
    {
        CONST_VTBL struct IObjCreatorAuthVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IObjCreatorAuth_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IObjCreatorAuth_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IObjCreatorAuth_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IObjCreatorAuth_CreateClsObjAuth(This,rClsid,bstrServer,ppUnknown)	\
    (This)->lpVtbl -> CreateClsObjAuth(This,rClsid,bstrServer,ppUnknown)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT __stdcall IObjCreatorAuth_CreateClsObjAuth_Proxy( 
    IObjCreatorAuth __RPC_FAR * This,
    /* [in] */ GUID __RPC_FAR *rClsid,
    /* [in] */ BSTR bstrServer,
    /* [out] */ IUnknown __RPC_FAR *__RPC_FAR *ppUnknown);


void __RPC_STUB IObjCreatorAuth_CreateClsObjAuth_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IObjCreatorAuth_INTERFACE_DEFINED__ */

#endif /* __CREATORLib_LIBRARY_DEFINED__ */

/* Additional Prototypes for ALL interfaces */

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif
