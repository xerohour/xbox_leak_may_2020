/* this ALWAYS GENERATED file contains the definitions for the interfaces */


/* File created by MIDL compiler version 3.01.75 */
/* at Fri Apr 24 13:18:39 1998
 */
/* Compiler settings for .\CDFGen.idl:
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

#ifndef __CDFGen_h__
#define __CDFGen_h__

#ifdef __cplusplus
extern "C"{
#endif 

/* Forward Declarations */ 

#ifndef __ICDFGenerator_FWD_DEFINED__
#define __ICDFGenerator_FWD_DEFINED__
typedef interface ICDFGenerator ICDFGenerator;
#endif 	/* __ICDFGenerator_FWD_DEFINED__ */


#ifndef __ICDFGenHelper_FWD_DEFINED__
#define __ICDFGenHelper_FWD_DEFINED__
typedef interface ICDFGenHelper ICDFGenHelper;
#endif 	/* __ICDFGenHelper_FWD_DEFINED__ */


#ifndef __CDFGenerator_FWD_DEFINED__
#define __CDFGenerator_FWD_DEFINED__

#ifdef __cplusplus
typedef class CDFGenerator CDFGenerator;
#else
typedef struct CDFGenerator CDFGenerator;
#endif /* __cplusplus */

#endif 	/* __CDFGenerator_FWD_DEFINED__ */


#ifndef __CDFGenHelper_FWD_DEFINED__
#define __CDFGenHelper_FWD_DEFINED__

#ifdef __cplusplus
typedef class CDFGenHelper CDFGenHelper;
#else
typedef struct CDFGenHelper CDFGenHelper;
#endif /* __cplusplus */

#endif 	/* __CDFGenHelper_FWD_DEFINED__ */


/* header files for imported files */
#include "oaidl.h"
#include "ocidl.h"

void __RPC_FAR * __RPC_USER MIDL_user_allocate(size_t);
void __RPC_USER MIDL_user_free( void __RPC_FAR * ); 

#ifndef __ICDFGenerator_INTERFACE_DEFINED__
#define __ICDFGenerator_INTERFACE_DEFINED__

/****************************************
 * Generated header for interface: ICDFGenerator
 * at Fri Apr 24 13:18:39 1998
 * using MIDL 3.01.75
 ****************************************/
/* [unique][helpstring][dual][uuid][object] */ 



EXTERN_C const IID IID_ICDFGenerator;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    interface DECLSPEC_UUID("578FA761-FD5D-11D0-BC62-00C04FC9B58C")
    ICDFGenerator : public IDispatch
    {
    public:
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE OnStartPage( 
            /* [in] */ IUnknown __RPC_FAR *piUnk) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE OnEndPage( void) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Run( 
            /* [in] */ VARIANT vtCDFGenHelper) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct ICDFGeneratorVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            ICDFGenerator __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            ICDFGenerator __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            ICDFGenerator __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            ICDFGenerator __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            ICDFGenerator __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            ICDFGenerator __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            ICDFGenerator __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *OnStartPage )( 
            ICDFGenerator __RPC_FAR * This,
            /* [in] */ IUnknown __RPC_FAR *piUnk);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *OnEndPage )( 
            ICDFGenerator __RPC_FAR * This);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Run )( 
            ICDFGenerator __RPC_FAR * This,
            /* [in] */ VARIANT vtCDFGenHelper);
        
        END_INTERFACE
    } ICDFGeneratorVtbl;

    interface ICDFGenerator
    {
        CONST_VTBL struct ICDFGeneratorVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define ICDFGenerator_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define ICDFGenerator_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define ICDFGenerator_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define ICDFGenerator_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define ICDFGenerator_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define ICDFGenerator_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define ICDFGenerator_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define ICDFGenerator_OnStartPage(This,piUnk)	\
    (This)->lpVtbl -> OnStartPage(This,piUnk)

#define ICDFGenerator_OnEndPage(This)	\
    (This)->lpVtbl -> OnEndPage(This)

#define ICDFGenerator_Run(This,vtCDFGenHelper)	\
    (This)->lpVtbl -> Run(This,vtCDFGenHelper)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ICDFGenerator_OnStartPage_Proxy( 
    ICDFGenerator __RPC_FAR * This,
    /* [in] */ IUnknown __RPC_FAR *piUnk);


void __RPC_STUB ICDFGenerator_OnStartPage_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ICDFGenerator_OnEndPage_Proxy( 
    ICDFGenerator __RPC_FAR * This);


void __RPC_STUB ICDFGenerator_OnEndPage_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ICDFGenerator_Run_Proxy( 
    ICDFGenerator __RPC_FAR * This,
    /* [in] */ VARIANT vtCDFGenHelper);


void __RPC_STUB ICDFGenerator_Run_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __ICDFGenerator_INTERFACE_DEFINED__ */


#ifndef __ICDFGenHelper_INTERFACE_DEFINED__
#define __ICDFGenHelper_INTERFACE_DEFINED__

/****************************************
 * Generated header for interface: ICDFGenHelper
 * at Fri Apr 24 13:18:39 1998
 * using MIDL 3.01.75
 ****************************************/
/* [unique][helpstring][dual][uuid][object] */ 



EXTERN_C const IID IID_ICDFGenHelper;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    interface DECLSPEC_UUID("578FA763-FD5D-11D0-BC62-00C04FC9B58C")
    ICDFGenHelper : public IDispatch
    {
    public:
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetSelectionListPropName( 
            /* [out] */ VARIANT __RPC_FAR *pvtSelectionListPropName,
            /* [out] */ VARIANT __RPC_FAR *pvtSelectionStylePropName) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetCDFString( 
            /* [in] */ const VARIANT vtSelectionList,
            /* [in] */ const VARIANT_BOOL fExclusionList,
            /* [out] */ VARIANT __RPC_FAR *pvtUTF8,
            /* [retval][out] */ BSTR __RPC_FAR *pbstrCDFString) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Load( 
            /* [in] */ BSTR bstrProject) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Project( 
            /* [retval][out] */ VARIANT __RPC_FAR *pvtProject) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct ICDFGenHelperVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            ICDFGenHelper __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            ICDFGenHelper __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            ICDFGenHelper __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            ICDFGenHelper __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            ICDFGenHelper __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            ICDFGenHelper __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            ICDFGenHelper __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetSelectionListPropName )( 
            ICDFGenHelper __RPC_FAR * This,
            /* [out] */ VARIANT __RPC_FAR *pvtSelectionListPropName,
            /* [out] */ VARIANT __RPC_FAR *pvtSelectionStylePropName);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetCDFString )( 
            ICDFGenHelper __RPC_FAR * This,
            /* [in] */ const VARIANT vtSelectionList,
            /* [in] */ const VARIANT_BOOL fExclusionList,
            /* [out] */ VARIANT __RPC_FAR *pvtUTF8,
            /* [retval][out] */ BSTR __RPC_FAR *pbstrCDFString);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Load )( 
            ICDFGenHelper __RPC_FAR * This,
            /* [in] */ BSTR bstrProject);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Project )( 
            ICDFGenHelper __RPC_FAR * This,
            /* [retval][out] */ VARIANT __RPC_FAR *pvtProject);
        
        END_INTERFACE
    } ICDFGenHelperVtbl;

    interface ICDFGenHelper
    {
        CONST_VTBL struct ICDFGenHelperVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define ICDFGenHelper_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define ICDFGenHelper_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define ICDFGenHelper_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define ICDFGenHelper_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define ICDFGenHelper_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define ICDFGenHelper_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define ICDFGenHelper_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define ICDFGenHelper_GetSelectionListPropName(This,pvtSelectionListPropName,pvtSelectionStylePropName)	\
    (This)->lpVtbl -> GetSelectionListPropName(This,pvtSelectionListPropName,pvtSelectionStylePropName)

#define ICDFGenHelper_GetCDFString(This,vtSelectionList,fExclusionList,pvtUTF8,pbstrCDFString)	\
    (This)->lpVtbl -> GetCDFString(This,vtSelectionList,fExclusionList,pvtUTF8,pbstrCDFString)

#define ICDFGenHelper_Load(This,bstrProject)	\
    (This)->lpVtbl -> Load(This,bstrProject)

#define ICDFGenHelper_get_Project(This,pvtProject)	\
    (This)->lpVtbl -> get_Project(This,pvtProject)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ICDFGenHelper_GetSelectionListPropName_Proxy( 
    ICDFGenHelper __RPC_FAR * This,
    /* [out] */ VARIANT __RPC_FAR *pvtSelectionListPropName,
    /* [out] */ VARIANT __RPC_FAR *pvtSelectionStylePropName);


void __RPC_STUB ICDFGenHelper_GetSelectionListPropName_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ICDFGenHelper_GetCDFString_Proxy( 
    ICDFGenHelper __RPC_FAR * This,
    /* [in] */ const VARIANT vtSelectionList,
    /* [in] */ const VARIANT_BOOL fExclusionList,
    /* [out] */ VARIANT __RPC_FAR *pvtUTF8,
    /* [retval][out] */ BSTR __RPC_FAR *pbstrCDFString);


void __RPC_STUB ICDFGenHelper_GetCDFString_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ICDFGenHelper_Load_Proxy( 
    ICDFGenHelper __RPC_FAR * This,
    /* [in] */ BSTR bstrProject);


void __RPC_STUB ICDFGenHelper_Load_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE ICDFGenHelper_get_Project_Proxy( 
    ICDFGenHelper __RPC_FAR * This,
    /* [retval][out] */ VARIANT __RPC_FAR *pvtProject);


void __RPC_STUB ICDFGenHelper_get_Project_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __ICDFGenHelper_INTERFACE_DEFINED__ */



#ifndef __CDFGENLib_LIBRARY_DEFINED__
#define __CDFGENLib_LIBRARY_DEFINED__

/****************************************
 * Generated header for library: CDFGENLib
 * at Fri Apr 24 13:18:39 1998
 * using MIDL 3.01.75
 ****************************************/
/* [helpstring][version][uuid] */ 



EXTERN_C const IID LIBID_CDFGENLib;

#ifdef __cplusplus
EXTERN_C const CLSID CLSID_CDFGenerator;

class DECLSPEC_UUID("578FA762-FD5D-11D0-BC62-00C04FC9B58C")
CDFGenerator;
#endif

#ifdef __cplusplus
EXTERN_C const CLSID CLSID_CDFGenHelper;

class DECLSPEC_UUID("578FA764-FD5D-11D0-BC62-00C04FC9B58C")
CDFGenHelper;
#endif
#endif /* __CDFGENLib_LIBRARY_DEFINED__ */

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
