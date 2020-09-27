/* this ALWAYS GENERATED file contains the definitions for the interfaces */


/* File created by MIDL compiler version 3.00.45 */
/* at Thu Sep 12 18:04:33 1996
 */
/* Compiler settings for ipcits.idl:
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

#ifndef __ipcits_h__
#define __ipcits_h__

#ifdef __cplusplus
extern "C"{
#endif 

/* Forward Declarations */ 

#ifndef __IInternalTrackSelection_FWD_DEFINED__
#define __IInternalTrackSelection_FWD_DEFINED__
typedef interface IInternalTrackSelection IInternalTrackSelection;
#endif 	/* __IInternalTrackSelection_FWD_DEFINED__ */


/* header files for imported files */
#include "oaidl.h"
#include "ocidl.h"

void __RPC_FAR * __RPC_USER MIDL_user_allocate(size_t);
void __RPC_USER MIDL_user_free( void __RPC_FAR * ); 

/****************************************
 * Generated header for interface: __MIDL__intf_0000
 * at Thu Sep 12 18:04:33 1996
 * using MIDL 3.00.45
 ****************************************/
/* [local] */ 


//+-------------------------------------------------------------------------
//
//  Microsoft Windows
//  Copyright (C) Microsoft Corporation, 1993 - 1996.
//
//--------------------------------------------------------------------------


//----------------------------------------------------------------
// IInternalTrackSelection interface
//----------------------------------------------------------------
// {BBB459C0-FF27-11cf-A1E4-00AA00C09209}
DEFINE_GUID(IID_IInternalTrackSelection, 
0xbbb459c0, 0xff27, 0x11cf, 0xa1, 0xe4, 0x0, 0xaa, 0x0, 0xc0, 0x92, 0x9);


extern RPC_IF_HANDLE __MIDL__intf_0000_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL__intf_0000_v0_0_s_ifspec;

#ifndef __IInternalTrackSelection_INTERFACE_DEFINED__
#define __IInternalTrackSelection_INTERFACE_DEFINED__

/****************************************
 * Generated header for interface: IInternalTrackSelection
 * at Thu Sep 12 18:04:33 1996
 * using MIDL 3.00.45
 ****************************************/
/* [unique][uuid][object] */ 



EXTERN_C const IID IID_IInternalTrackSelection;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    interface IInternalTrackSelection : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE IsExternalSelectChange( 
            /* [out] */ VARIANT_BOOL __RPC_FAR *pbReturn) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE OnInternalSelectChange( 
            /* [in] */ DWORD dwCookie) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetInternalSelectList( 
            /* [out] */ CALPOLESTR __RPC_FAR *pcaStringsOut,
            /* [out] */ CADWORD __RPC_FAR *pcaCookiesOut) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IInternalTrackSelectionVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IInternalTrackSelection __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IInternalTrackSelection __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IInternalTrackSelection __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *IsExternalSelectChange )( 
            IInternalTrackSelection __RPC_FAR * This,
            /* [out] */ VARIANT_BOOL __RPC_FAR *pbReturn);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *OnInternalSelectChange )( 
            IInternalTrackSelection __RPC_FAR * This,
            /* [in] */ DWORD dwCookie);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetInternalSelectList )( 
            IInternalTrackSelection __RPC_FAR * This,
            /* [out] */ CALPOLESTR __RPC_FAR *pcaStringsOut,
            /* [out] */ CADWORD __RPC_FAR *pcaCookiesOut);
        
        END_INTERFACE
    } IInternalTrackSelectionVtbl;

    interface IInternalTrackSelection
    {
        CONST_VTBL struct IInternalTrackSelectionVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IInternalTrackSelection_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IInternalTrackSelection_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IInternalTrackSelection_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IInternalTrackSelection_IsExternalSelectChange(This,pbReturn)	\
    (This)->lpVtbl -> IsExternalSelectChange(This,pbReturn)

#define IInternalTrackSelection_OnInternalSelectChange(This,dwCookie)	\
    (This)->lpVtbl -> OnInternalSelectChange(This,dwCookie)

#define IInternalTrackSelection_GetInternalSelectList(This,pcaStringsOut,pcaCookiesOut)	\
    (This)->lpVtbl -> GetInternalSelectList(This,pcaStringsOut,pcaCookiesOut)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IInternalTrackSelection_IsExternalSelectChange_Proxy( 
    IInternalTrackSelection __RPC_FAR * This,
    /* [out] */ VARIANT_BOOL __RPC_FAR *pbReturn);


void __RPC_STUB IInternalTrackSelection_IsExternalSelectChange_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IInternalTrackSelection_OnInternalSelectChange_Proxy( 
    IInternalTrackSelection __RPC_FAR * This,
    /* [in] */ DWORD dwCookie);


void __RPC_STUB IInternalTrackSelection_OnInternalSelectChange_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IInternalTrackSelection_GetInternalSelectList_Proxy( 
    IInternalTrackSelection __RPC_FAR * This,
    /* [out] */ CALPOLESTR __RPC_FAR *pcaStringsOut,
    /* [out] */ CADWORD __RPC_FAR *pcaCookiesOut);


void __RPC_STUB IInternalTrackSelection_GetInternalSelectList_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IInternalTrackSelection_INTERFACE_DEFINED__ */


/* Additional Prototypes for ALL interfaces */

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif
