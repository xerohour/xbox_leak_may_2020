/* this ALWAYS GENERATED file contains the definitions for the interfaces */


/* File created by MIDL compiler version 3.01.75 */
/* at Wed Apr 22 19:59:28 1998
 */
/* Compiler settings for gthrntfy.idl:
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

#ifndef __gthrntfy_h__
#define __gthrntfy_h__

#ifdef __cplusplus
extern "C"{
#endif 

/* Forward Declarations */ 

#ifndef __IGatherNotify_FWD_DEFINED__
#define __IGatherNotify_FWD_DEFINED__
typedef interface IGatherNotify IGatherNotify;
#endif 	/* __IGatherNotify_FWD_DEFINED__ */


/* header files for imported files */
#include "oaidl.h"

void __RPC_FAR * __RPC_USER MIDL_user_allocate(size_t);
void __RPC_USER MIDL_user_free( void __RPC_FAR * ); 

#ifndef __IGatherNotify_INTERFACE_DEFINED__
#define __IGatherNotify_INTERFACE_DEFINED__

/****************************************
 * Generated header for interface: IGatherNotify
 * at Wed Apr 22 19:59:28 1998
 * using MIDL 3.01.75
 ****************************************/
/* [helpstring][public][dual][unique][uuid][object] */ 


typedef 
enum TagChangeAdviseCodes
    {	GTHR_CA_ADD	= 0,
	GTHR_CA_DELETE	= GTHR_CA_ADD + 1,
	GTHR_CA_MODIFY	= GTHR_CA_DELETE + 1
    }	ChangeAdviseCodes;


EXTERN_C const IID IID_IGatherNotify;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    interface DECLSPEC_UUID("0b63e376-9ccc-11d0-bcdb-00805fccce04")
    IGatherNotify : public IDispatch
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE Init( 
            /* [in] */ BSTR bstrSourceName) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE OnDataChange( 
            /* [in] */ LONG eChangeAdvise,
            /* [in] */ BSTR bstrPhysicalAddress,
            /* [optional][in] */ BSTR bstrLogicalAddress) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IGatherNotifyVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IGatherNotify __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IGatherNotify __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IGatherNotify __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            IGatherNotify __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            IGatherNotify __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            IGatherNotify __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            IGatherNotify __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Init )( 
            IGatherNotify __RPC_FAR * This,
            /* [in] */ BSTR bstrSourceName);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *OnDataChange )( 
            IGatherNotify __RPC_FAR * This,
            /* [in] */ LONG eChangeAdvise,
            /* [in] */ BSTR bstrPhysicalAddress,
            /* [optional][in] */ BSTR bstrLogicalAddress);
        
        END_INTERFACE
    } IGatherNotifyVtbl;

    interface IGatherNotify
    {
        CONST_VTBL struct IGatherNotifyVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IGatherNotify_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IGatherNotify_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IGatherNotify_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IGatherNotify_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IGatherNotify_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IGatherNotify_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IGatherNotify_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IGatherNotify_Init(This,bstrSourceName)	\
    (This)->lpVtbl -> Init(This,bstrSourceName)

#define IGatherNotify_OnDataChange(This,eChangeAdvise,bstrPhysicalAddress,bstrLogicalAddress)	\
    (This)->lpVtbl -> OnDataChange(This,eChangeAdvise,bstrPhysicalAddress,bstrLogicalAddress)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IGatherNotify_Init_Proxy( 
    IGatherNotify __RPC_FAR * This,
    /* [in] */ BSTR bstrSourceName);


void __RPC_STUB IGatherNotify_Init_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IGatherNotify_OnDataChange_Proxy( 
    IGatherNotify __RPC_FAR * This,
    /* [in] */ LONG eChangeAdvise,
    /* [in] */ BSTR bstrPhysicalAddress,
    /* [optional][in] */ BSTR bstrLogicalAddress);


void __RPC_STUB IGatherNotify_OnDataChange_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IGatherNotify_INTERFACE_DEFINED__ */


/* Additional Prototypes for ALL interfaces */

unsigned long             __RPC_USER  BSTR_UserSize(     unsigned long __RPC_FAR *, unsigned long            , BSTR __RPC_FAR * ); 
unsigned char __RPC_FAR * __RPC_USER  BSTR_UserMarshal(  unsigned long __RPC_FAR *, unsigned char __RPC_FAR *, BSTR __RPC_FAR * ); 
unsigned char __RPC_FAR * __RPC_USER  BSTR_UserUnmarshal(unsigned long __RPC_FAR *, unsigned char __RPC_FAR *, BSTR __RPC_FAR * ); 
void                      __RPC_USER  BSTR_UserFree(     unsigned long __RPC_FAR *, BSTR __RPC_FAR * ); 

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif
