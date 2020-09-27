
#pragma warning( disable: 4049 )  /* more than 64k source lines */

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


 /* File created by MIDL compiler version 6.00.0347 */
/* at Sat Jan 05 01:43:50 2002
 */
/* Compiler settings for webref.idl:
    Oicf, W0, Zp8, env=Win32 (32b run)
    protocol : dce , ms_ext, c_ext
    error checks: allocation ref bounds_check enum stub_data , no_format_optimization
    VC __declspec() decoration level: 
         __declspec(uuid()), __declspec(selectany), __declspec(novtable)
         DECLSPEC_UUID(), MIDL_INTERFACE()
*/
//@@MIDL_FILE_HEADING(  )


/* verify that the <rpcndr.h> version is high enough to compile this file*/
#ifndef __REQUIRED_RPCNDR_H_VERSION__
#define __REQUIRED_RPCNDR_H_VERSION__ 440
#endif

#include "rpc.h"
#include "rpcndr.h"

#ifndef __RPCNDR_H_VERSION__
#error this stub requires an updated version of <rpcndr.h>
#endif // __RPCNDR_H_VERSION__

#ifndef COM_NO_WINDOWS_H
#include "windows.h"
#include "ole2.h"
#endif /*COM_NO_WINDOWS_H*/

#ifndef __webref_h__
#define __webref_h__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

/* Forward Declarations */ 

#ifndef __IVsAddWebReference_FWD_DEFINED__
#define __IVsAddWebReference_FWD_DEFINED__
typedef interface IVsAddWebReference IVsAddWebReference;
#endif 	/* __IVsAddWebReference_FWD_DEFINED__ */


/* header files for imported files */
#include "oaidl.h"
#include "ocidl.h"

#ifdef __cplusplus
extern "C"{
#endif 

void * __RPC_USER MIDL_user_allocate(size_t);
void __RPC_USER MIDL_user_free( void * ); 

#ifndef __IVsAddWebReference_INTERFACE_DEFINED__
#define __IVsAddWebReference_INTERFACE_DEFINED__

/* interface IVsAddWebReference */
/* [object][unique][uuid] */ 


EXTERN_C const IID IID_IVsAddWebReference;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("D85F3427-8160-4d1f-8A2B-A8A1E3011C2D")
    IVsAddWebReference : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE AddWebReferenceEx( 
            /* [in] */ LPCOLESTR pszUrl) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IVsAddWebReferenceVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IVsAddWebReference * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IVsAddWebReference * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IVsAddWebReference * This);
        
        HRESULT ( STDMETHODCALLTYPE *AddWebReferenceEx )( 
            IVsAddWebReference * This,
            /* [in] */ LPCOLESTR pszUrl);
        
        END_INTERFACE
    } IVsAddWebReferenceVtbl;

    interface IVsAddWebReference
    {
        CONST_VTBL struct IVsAddWebReferenceVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IVsAddWebReference_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IVsAddWebReference_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IVsAddWebReference_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IVsAddWebReference_AddWebReferenceEx(This,pszUrl)	\
    (This)->lpVtbl -> AddWebReferenceEx(This,pszUrl)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IVsAddWebReference_AddWebReferenceEx_Proxy( 
    IVsAddWebReference * This,
    /* [in] */ LPCOLESTR pszUrl);


void __RPC_STUB IVsAddWebReference_AddWebReferenceEx_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IVsAddWebReference_INTERFACE_DEFINED__ */


/* Additional Prototypes for ALL interfaces */

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif


