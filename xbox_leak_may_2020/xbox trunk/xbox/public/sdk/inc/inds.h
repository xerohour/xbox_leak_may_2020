/* this ALWAYS GENERATED file contains the definitions for the interfaces */


/* File created by MIDL compiler version 3.02.88 */
/* at Thu Oct 30 10:33:16 1997
 */
/* Compiler settings for nds.odl:
    Os (OptLev=s), W1, Zp8, env=Win32, ms_ext, c_ext
    error checks: none
*/
//@@MIDL_FILE_HEADING(  )
#include "rpc.h"
#include "rpcndr.h"

#ifndef __inds_h__
#define __inds_h__

#if _MSC_VER > 1000
#pragma once
#endif

#ifdef __cplusplus
extern "C"{
#endif

/* Forward Declarations */

#ifndef __NDSNamespace_FWD_DEFINED__
#define __NDSNamespace_FWD_DEFINED__

#ifdef __cplusplus
typedef class NDSNamespace NDSNamespace;
#else
typedef struct NDSNamespace NDSNamespace;
#endif /* __cplusplus */

#endif 	/* __NDSNamespace_FWD_DEFINED__ */


#ifndef __NDSGenObject_FWD_DEFINED__
#define __NDSGenObject_FWD_DEFINED__

#ifdef __cplusplus
typedef class NDSGenObject NDSGenObject;
#else
typedef struct NDSGenObject NDSGenObject;
#endif /* __cplusplus */

#endif 	/* __NDSGenObject_FWD_DEFINED__ */


#ifndef __NDSProvider_FWD_DEFINED__
#define __NDSProvider_FWD_DEFINED__

#ifdef __cplusplus
typedef class NDSProvider NDSProvider;
#else
typedef struct NDSProvider NDSProvider;
#endif /* __cplusplus */

#endif 	/* __NDSProvider_FWD_DEFINED__ */


#ifndef __NDSTree_FWD_DEFINED__
#define __NDSTree_FWD_DEFINED__

#ifdef __cplusplus
typedef class NDSTree NDSTree;
#else
typedef struct NDSTree NDSTree;
#endif /* __cplusplus */

#endif 	/* __NDSTree_FWD_DEFINED__ */


#ifndef __NDSSchema_FWD_DEFINED__
#define __NDSSchema_FWD_DEFINED__

#ifdef __cplusplus
typedef class NDSSchema NDSSchema;
#else
typedef struct NDSSchema NDSSchema;
#endif /* __cplusplus */

#endif 	/* __NDSSchema_FWD_DEFINED__ */


#ifndef __NDSClass_FWD_DEFINED__
#define __NDSClass_FWD_DEFINED__

#ifdef __cplusplus
typedef class NDSClass NDSClass;
#else
typedef struct NDSClass NDSClass;
#endif /* __cplusplus */

#endif 	/* __NDSClass_FWD_DEFINED__ */


#ifndef __NDSProperty_FWD_DEFINED__
#define __NDSProperty_FWD_DEFINED__

#ifdef __cplusplus
typedef class NDSProperty NDSProperty;
#else
typedef struct NDSProperty NDSProperty;
#endif /* __cplusplus */

#endif 	/* __NDSProperty_FWD_DEFINED__ */


#ifndef __NDSSyntax_FWD_DEFINED__
#define __NDSSyntax_FWD_DEFINED__

#ifdef __cplusplus
typedef class NDSSyntax NDSSyntax;
#else
typedef struct NDSSyntax NDSSyntax;
#endif /* __cplusplus */

#endif 	/* __NDSSyntax_FWD_DEFINED__ */


#ifndef __INDSCaseIgnoreList_FWD_DEFINED__
#define __INDSCaseIgnoreList_FWD_DEFINED__
typedef interface INDSCaseIgnoreList INDSCaseIgnoreList;
#endif 	/* __INDSCaseIgnoreList_FWD_DEFINED__ */


#ifndef __NDSCaseIgnoreList_FWD_DEFINED__
#define __NDSCaseIgnoreList_FWD_DEFINED__

#ifdef __cplusplus
typedef class NDSCaseIgnoreList NDSCaseIgnoreList;
#else
typedef struct NDSCaseIgnoreList NDSCaseIgnoreList;
#endif /* __cplusplus */

#endif 	/* __NDSCaseIgnoreList_FWD_DEFINED__ */


#ifndef __INDSFaxNumber_FWD_DEFINED__
#define __INDSFaxNumber_FWD_DEFINED__
typedef interface INDSFaxNumber INDSFaxNumber;
#endif 	/* __INDSFaxNumber_FWD_DEFINED__ */


#ifndef __NDSFaxNumber_FWD_DEFINED__
#define __NDSFaxNumber_FWD_DEFINED__

#ifdef __cplusplus
typedef class NDSFaxNumber NDSFaxNumber;
#else
typedef struct NDSFaxNumber NDSFaxNumber;
#endif /* __cplusplus */

#endif 	/* __NDSFaxNumber_FWD_DEFINED__ */


#ifndef __INDSNetAddress_FWD_DEFINED__
#define __INDSNetAddress_FWD_DEFINED__
typedef interface INDSNetAddress INDSNetAddress;
#endif 	/* __INDSNetAddress_FWD_DEFINED__ */


#ifndef __NDSNetAddress_FWD_DEFINED__
#define __NDSNetAddress_FWD_DEFINED__

#ifdef __cplusplus
typedef class NDSNetAddress NDSNetAddress;
#else
typedef struct NDSNetAddress NDSNetAddress;
#endif /* __cplusplus */

#endif 	/* __NDSNetAddress_FWD_DEFINED__ */


#ifndef __INDSOctetList_FWD_DEFINED__
#define __INDSOctetList_FWD_DEFINED__
typedef interface INDSOctetList INDSOctetList;
#endif 	/* __INDSOctetList_FWD_DEFINED__ */


#ifndef __NDSOctetList_FWD_DEFINED__
#define __NDSOctetList_FWD_DEFINED__

#ifdef __cplusplus
typedef class NDSOctetList NDSOctetList;
#else
typedef struct NDSOctetList NDSOctetList;
#endif /* __cplusplus */

#endif 	/* __NDSOctetList_FWD_DEFINED__ */


#ifndef __INDSEmail_FWD_DEFINED__
#define __INDSEmail_FWD_DEFINED__
typedef interface INDSEmail INDSEmail;
#endif 	/* __INDSEmail_FWD_DEFINED__ */


#ifndef __NDSEmail_FWD_DEFINED__
#define __NDSEmail_FWD_DEFINED__

#ifdef __cplusplus
typedef class NDSEmail NDSEmail;
#else
typedef struct NDSEmail NDSEmail;
#endif /* __cplusplus */

#endif 	/* __NDSEmail_FWD_DEFINED__ */


#ifndef __INDSPath_FWD_DEFINED__
#define __INDSPath_FWD_DEFINED__
typedef interface INDSPath INDSPath;
#endif 	/* __INDSPath_FWD_DEFINED__ */


#ifndef __NDSPath_FWD_DEFINED__
#define __NDSPath_FWD_DEFINED__

#ifdef __cplusplus
typedef class NDSPath NDSPath;
#else
typedef struct NDSPath NDSPath;
#endif /* __cplusplus */

#endif 	/* __NDSPath_FWD_DEFINED__ */


#ifndef __INDSReplicaPointer_FWD_DEFINED__
#define __INDSReplicaPointer_FWD_DEFINED__
typedef interface INDSReplicaPointer INDSReplicaPointer;
#endif 	/* __INDSReplicaPointer_FWD_DEFINED__ */


#ifndef __NDSReplicaPointer_FWD_DEFINED__
#define __NDSReplicaPointer_FWD_DEFINED__

#ifdef __cplusplus
typedef class NDSReplicaPointer NDSReplicaPointer;
#else
typedef struct NDSReplicaPointer NDSReplicaPointer;
#endif /* __cplusplus */

#endif 	/* __NDSReplicaPointer_FWD_DEFINED__ */


#ifndef __INDSAcl_FWD_DEFINED__
#define __INDSAcl_FWD_DEFINED__
typedef interface INDSAcl INDSAcl;
#endif 	/* __INDSAcl_FWD_DEFINED__ */


#ifndef __NDSAcl_FWD_DEFINED__
#define __NDSAcl_FWD_DEFINED__

#ifdef __cplusplus
typedef class NDSAcl NDSAcl;
#else
typedef struct NDSAcl NDSAcl;
#endif /* __cplusplus */

#endif 	/* __NDSAcl_FWD_DEFINED__ */


#ifndef __INDSTimeStamp_FWD_DEFINED__
#define __INDSTimeStamp_FWD_DEFINED__
typedef interface INDSTimeStamp INDSTimeStamp;
#endif 	/* __INDSTimeStamp_FWD_DEFINED__ */


#ifndef __NDSTimeStamp_FWD_DEFINED__
#define __NDSTimeStamp_FWD_DEFINED__

#ifdef __cplusplus
typedef class NDSTimeStamp NDSTimeStamp;
#else
typedef struct NDSTimeStamp NDSTimeStamp;
#endif /* __cplusplus */

#endif 	/* __NDSTimeStamp_FWD_DEFINED__ */


#ifndef __INDSPostalAddress_FWD_DEFINED__
#define __INDSPostalAddress_FWD_DEFINED__
typedef interface INDSPostalAddress INDSPostalAddress;
#endif 	/* __INDSPostalAddress_FWD_DEFINED__ */


#ifndef __NDSPostalAddress_FWD_DEFINED__
#define __NDSPostalAddress_FWD_DEFINED__

#ifdef __cplusplus
typedef class NDSPostalAddress NDSPostalAddress;
#else
typedef struct NDSPostalAddress NDSPostalAddress;
#endif /* __cplusplus */

#endif 	/* __NDSPostalAddress_FWD_DEFINED__ */


#ifndef __INDSBackLink_FWD_DEFINED__
#define __INDSBackLink_FWD_DEFINED__
typedef interface INDSBackLink INDSBackLink;
#endif 	/* __INDSBackLink_FWD_DEFINED__ */


#ifndef __NDSBackLink_FWD_DEFINED__
#define __NDSBackLink_FWD_DEFINED__

#ifdef __cplusplus
typedef class NDSBackLink NDSBackLink;
#else
typedef struct NDSBackLink NDSBackLink;
#endif /* __cplusplus */

#endif 	/* __NDSBackLink_FWD_DEFINED__ */


#ifndef __INDSTypedName_FWD_DEFINED__
#define __INDSTypedName_FWD_DEFINED__
typedef interface INDSTypedName INDSTypedName;
#endif 	/* __INDSTypedName_FWD_DEFINED__ */


#ifndef __NDSTypedName_FWD_DEFINED__
#define __NDSTypedName_FWD_DEFINED__

#ifdef __cplusplus
typedef class NDSTypedName NDSTypedName;
#else
typedef struct NDSTypedName NDSTypedName;
#endif /* __cplusplus */

#endif 	/* __NDSTypedName_FWD_DEFINED__ */


#ifndef __INDSHold_FWD_DEFINED__
#define __INDSHold_FWD_DEFINED__
typedef interface INDSHold INDSHold;
#endif 	/* __INDSHold_FWD_DEFINED__ */


#ifndef __NDSHold_FWD_DEFINED__
#define __NDSHold_FWD_DEFINED__

#ifdef __cplusplus
typedef class NDSHold NDSHold;
#else
typedef struct NDSHold NDSHold;
#endif /* __cplusplus */

#endif 	/* __NDSHold_FWD_DEFINED__ */


void __RPC_FAR * __RPC_USER MIDL_user_allocate(size_t);
void __RPC_USER MIDL_user_free( void __RPC_FAR * );


#ifndef __NDSOle_LIBRARY_DEFINED__
#define __NDSOle_LIBRARY_DEFINED__

/****************************************
 * Generated header for library: NDSOle
 * at Thu Oct 30 10:33:16 1997
 * using MIDL 3.02.88
 ****************************************/
/* [helpstring][version][uuid] */



EXTERN_C const IID LIBID_NDSOle;

EXTERN_C const CLSID CLSID_NDSNamespace;

#ifdef __cplusplus

class DECLSPEC_UUID("51d11c90-7b9d-11cf-b03d-00aa006e0975")
NDSNamespace;
#endif

EXTERN_C const CLSID CLSID_NDSGenObject;

#ifdef __cplusplus

class DECLSPEC_UUID("8b645280-7ba4-11cf-b03d-00aa006e0975")
NDSGenObject;
#endif

EXTERN_C const CLSID CLSID_NDSProvider;

#ifdef __cplusplus

class DECLSPEC_UUID("323991f0-7bad-11cf-b03d-00aa006e0975")
NDSProvider;
#endif

EXTERN_C const CLSID CLSID_NDSTree;

#ifdef __cplusplus

class DECLSPEC_UUID("47e94340-994f-11cf-a5f2-00aa006e05d3")
NDSTree;
#endif

EXTERN_C const CLSID CLSID_NDSSchema;

#ifdef __cplusplus

class DECLSPEC_UUID("65e252b0-b4c8-11cf-a2b5-00aa006e05d3")
NDSSchema;
#endif

EXTERN_C const CLSID CLSID_NDSClass;

#ifdef __cplusplus

class DECLSPEC_UUID("946260e0-b505-11cf-a2b5-00aa006e05d3")
NDSClass;
#endif

EXTERN_C const CLSID CLSID_NDSProperty;

#ifdef __cplusplus

class DECLSPEC_UUID("93f8fbf0-b67b-11cf-a2b5-00aa006e05d3")
NDSProperty;
#endif

EXTERN_C const CLSID CLSID_NDSSyntax;

#ifdef __cplusplus

class DECLSPEC_UUID("953dbc50-ebdb-11cf-8abc-00c04fd8d503")
NDSSyntax;
#endif

#ifndef __INDSCaseIgnoreList_INTERFACE_DEFINED__
#define __INDSCaseIgnoreList_INTERFACE_DEFINED__

/****************************************
 * Generated header for interface: INDSCaseIgnoreList
 * at Thu Oct 30 10:33:16 1997
 * using MIDL 3.02.88
 ****************************************/
/* [object][dual][oleautomation][uuid] */



EXTERN_C const IID IID_INDSCaseIgnoreList;

#if defined(__cplusplus) && !defined(CINTERFACE)

    interface DECLSPEC_UUID("7b66b533-4680-11d1-a3b4-00c04fb950dc")
    INDSCaseIgnoreList : public IDispatch
    {
    public:
        virtual /* [id][propget] */ HRESULT STDMETHODCALLTYPE get_CaseIgnoreList(
            /* [retval][out] */ VARIANT __RPC_FAR *retval) = 0;

        virtual /* [id][propput] */ HRESULT STDMETHODCALLTYPE put_CaseIgnoreList(
            /* [in] */ VARIANT vCaseIgnoreList) = 0;

    };

#else 	/* C style interface */

    typedef struct INDSCaseIgnoreListVtbl
    {
        BEGIN_INTERFACE

        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )(
            INDSCaseIgnoreList __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);

        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )(
            INDSCaseIgnoreList __RPC_FAR * This);

        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )(
            INDSCaseIgnoreList __RPC_FAR * This);

        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )(
            INDSCaseIgnoreList __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);

        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )(
            INDSCaseIgnoreList __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);

        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )(
            INDSCaseIgnoreList __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);

        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )(
            INDSCaseIgnoreList __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);

        /* [id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_CaseIgnoreList )(
            INDSCaseIgnoreList __RPC_FAR * This,
            /* [retval][out] */ VARIANT __RPC_FAR *retval);

        /* [id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_CaseIgnoreList )(
            INDSCaseIgnoreList __RPC_FAR * This,
            /* [in] */ VARIANT vCaseIgnoreList);

        END_INTERFACE
    } INDSCaseIgnoreListVtbl;

    interface INDSCaseIgnoreList
    {
        CONST_VTBL struct INDSCaseIgnoreListVtbl __RPC_FAR *lpVtbl;
    };



#ifdef COBJMACROS


#define INDSCaseIgnoreList_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define INDSCaseIgnoreList_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define INDSCaseIgnoreList_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define INDSCaseIgnoreList_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define INDSCaseIgnoreList_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define INDSCaseIgnoreList_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define INDSCaseIgnoreList_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define INDSCaseIgnoreList_get_CaseIgnoreList(This,retval)	\
    (This)->lpVtbl -> get_CaseIgnoreList(This,retval)

#define INDSCaseIgnoreList_put_CaseIgnoreList(This,vCaseIgnoreList)	\
    (This)->lpVtbl -> put_CaseIgnoreList(This,vCaseIgnoreList)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [id][propget] */ HRESULT STDMETHODCALLTYPE INDSCaseIgnoreList_get_CaseIgnoreList_Proxy(
    INDSCaseIgnoreList __RPC_FAR * This,
    /* [retval][out] */ VARIANT __RPC_FAR *retval);


void __RPC_STUB INDSCaseIgnoreList_get_CaseIgnoreList_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propput] */ HRESULT STDMETHODCALLTYPE INDSCaseIgnoreList_put_CaseIgnoreList_Proxy(
    INDSCaseIgnoreList __RPC_FAR * This,
    /* [in] */ VARIANT vCaseIgnoreList);


void __RPC_STUB INDSCaseIgnoreList_put_CaseIgnoreList_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __INDSCaseIgnoreList_INTERFACE_DEFINED__ */


EXTERN_C const CLSID CLSID_NDSCaseIgnoreList;

#ifdef __cplusplus

class DECLSPEC_UUID("15f88a55-4680-11d1-a3b4-00c04fb950dc")
NDSCaseIgnoreList;
#endif

#ifndef __INDSFaxNumber_INTERFACE_DEFINED__
#define __INDSFaxNumber_INTERFACE_DEFINED__

/****************************************
 * Generated header for interface: INDSFaxNumber
 * at Thu Oct 30 10:33:16 1997
 * using MIDL 3.02.88
 ****************************************/
/* [object][dual][oleautomation][uuid] */



EXTERN_C const IID IID_INDSFaxNumber;

#if defined(__cplusplus) && !defined(CINTERFACE)

    interface DECLSPEC_UUID("a910dea9-4680-11d1-a3b4-00c04fb950dc")
    INDSFaxNumber : public IDispatch
    {
    public:
        virtual /* [id][propget] */ HRESULT STDMETHODCALLTYPE get_TelephoneNumber(
            /* [retval][out] */ BSTR __RPC_FAR *retval) = 0;

        virtual /* [id][propput] */ HRESULT STDMETHODCALLTYPE put_TelephoneNumber(
            /* [in] */ BSTR bstrTelephoneNumber) = 0;

        virtual /* [id][propget] */ HRESULT STDMETHODCALLTYPE get_Parameters(
            /* [retval][out] */ VARIANT __RPC_FAR *retval) = 0;

        virtual /* [id][propput] */ HRESULT STDMETHODCALLTYPE put_Parameters(
            /* [in] */ VARIANT vParameters) = 0;

    };

#else 	/* C style interface */

    typedef struct INDSFaxNumberVtbl
    {
        BEGIN_INTERFACE

        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )(
            INDSFaxNumber __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);

        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )(
            INDSFaxNumber __RPC_FAR * This);

        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )(
            INDSFaxNumber __RPC_FAR * This);

        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )(
            INDSFaxNumber __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);

        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )(
            INDSFaxNumber __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);

        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )(
            INDSFaxNumber __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);

        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )(
            INDSFaxNumber __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);

        /* [id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_TelephoneNumber )(
            INDSFaxNumber __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *retval);

        /* [id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_TelephoneNumber )(
            INDSFaxNumber __RPC_FAR * This,
            /* [in] */ BSTR bstrTelephoneNumber);

        /* [id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Parameters )(
            INDSFaxNumber __RPC_FAR * This,
            /* [retval][out] */ VARIANT __RPC_FAR *retval);

        /* [id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Parameters )(
            INDSFaxNumber __RPC_FAR * This,
            /* [in] */ VARIANT vParameters);

        END_INTERFACE
    } INDSFaxNumberVtbl;

    interface INDSFaxNumber
    {
        CONST_VTBL struct INDSFaxNumberVtbl __RPC_FAR *lpVtbl;
    };



#ifdef COBJMACROS


#define INDSFaxNumber_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define INDSFaxNumber_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define INDSFaxNumber_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define INDSFaxNumber_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define INDSFaxNumber_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define INDSFaxNumber_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define INDSFaxNumber_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define INDSFaxNumber_get_TelephoneNumber(This,retval)	\
    (This)->lpVtbl -> get_TelephoneNumber(This,retval)

#define INDSFaxNumber_put_TelephoneNumber(This,bstrTelephoneNumber)	\
    (This)->lpVtbl -> put_TelephoneNumber(This,bstrTelephoneNumber)

#define INDSFaxNumber_get_Parameters(This,retval)	\
    (This)->lpVtbl -> get_Parameters(This,retval)

#define INDSFaxNumber_put_Parameters(This,vParameters)	\
    (This)->lpVtbl -> put_Parameters(This,vParameters)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [id][propget] */ HRESULT STDMETHODCALLTYPE INDSFaxNumber_get_TelephoneNumber_Proxy(
    INDSFaxNumber __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *retval);


void __RPC_STUB INDSFaxNumber_get_TelephoneNumber_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propput] */ HRESULT STDMETHODCALLTYPE INDSFaxNumber_put_TelephoneNumber_Proxy(
    INDSFaxNumber __RPC_FAR * This,
    /* [in] */ BSTR bstrTelephoneNumber);


void __RPC_STUB INDSFaxNumber_put_TelephoneNumber_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propget] */ HRESULT STDMETHODCALLTYPE INDSFaxNumber_get_Parameters_Proxy(
    INDSFaxNumber __RPC_FAR * This,
    /* [retval][out] */ VARIANT __RPC_FAR *retval);


void __RPC_STUB INDSFaxNumber_get_Parameters_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propput] */ HRESULT STDMETHODCALLTYPE INDSFaxNumber_put_Parameters_Proxy(
    INDSFaxNumber __RPC_FAR * This,
    /* [in] */ VARIANT vParameters);


void __RPC_STUB INDSFaxNumber_put_Parameters_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __INDSFaxNumber_INTERFACE_DEFINED__ */


EXTERN_C const CLSID CLSID_NDSFaxNumber;

#ifdef __cplusplus

class DECLSPEC_UUID("a5062215-4681-11d1-a3b4-00c04fb950dc")
NDSFaxNumber;
#endif

#ifndef __INDSNetAddress_INTERFACE_DEFINED__
#define __INDSNetAddress_INTERFACE_DEFINED__

/****************************************
 * Generated header for interface: INDSNetAddress
 * at Thu Oct 30 10:33:16 1997
 * using MIDL 3.02.88
 ****************************************/
/* [object][dual][oleautomation][uuid] */



EXTERN_C const IID IID_INDSNetAddress;

#if defined(__cplusplus) && !defined(CINTERFACE)

    interface DECLSPEC_UUID("b21a50a9-4080-11d1-a3ac-00c04fb950dc")
    INDSNetAddress : public IDispatch
    {
    public:
        virtual /* [id][propget] */ HRESULT STDMETHODCALLTYPE get_AddressType(
            /* [retval][out] */ long __RPC_FAR *retval) = 0;

        virtual /* [id][propput] */ HRESULT STDMETHODCALLTYPE put_AddressType(
            /* [in] */ long lnAddressType) = 0;

        virtual /* [id][propget] */ HRESULT STDMETHODCALLTYPE get_Address(
            /* [retval][out] */ VARIANT __RPC_FAR *retval) = 0;

        virtual /* [id][propput] */ HRESULT STDMETHODCALLTYPE put_Address(
            /* [in] */ VARIANT vAddress) = 0;

    };

#else 	/* C style interface */

    typedef struct INDSNetAddressVtbl
    {
        BEGIN_INTERFACE

        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )(
            INDSNetAddress __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);

        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )(
            INDSNetAddress __RPC_FAR * This);

        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )(
            INDSNetAddress __RPC_FAR * This);

        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )(
            INDSNetAddress __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);

        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )(
            INDSNetAddress __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);

        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )(
            INDSNetAddress __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);

        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )(
            INDSNetAddress __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);

        /* [id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_AddressType )(
            INDSNetAddress __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *retval);

        /* [id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_AddressType )(
            INDSNetAddress __RPC_FAR * This,
            /* [in] */ long lnAddressType);

        /* [id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Address )(
            INDSNetAddress __RPC_FAR * This,
            /* [retval][out] */ VARIANT __RPC_FAR *retval);

        /* [id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Address )(
            INDSNetAddress __RPC_FAR * This,
            /* [in] */ VARIANT vAddress);

        END_INTERFACE
    } INDSNetAddressVtbl;

    interface INDSNetAddress
    {
        CONST_VTBL struct INDSNetAddressVtbl __RPC_FAR *lpVtbl;
    };



#ifdef COBJMACROS


#define INDSNetAddress_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define INDSNetAddress_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define INDSNetAddress_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define INDSNetAddress_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define INDSNetAddress_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define INDSNetAddress_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define INDSNetAddress_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define INDSNetAddress_get_AddressType(This,retval)	\
    (This)->lpVtbl -> get_AddressType(This,retval)

#define INDSNetAddress_put_AddressType(This,lnAddressType)	\
    (This)->lpVtbl -> put_AddressType(This,lnAddressType)

#define INDSNetAddress_get_Address(This,retval)	\
    (This)->lpVtbl -> get_Address(This,retval)

#define INDSNetAddress_put_Address(This,vAddress)	\
    (This)->lpVtbl -> put_Address(This,vAddress)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [id][propget] */ HRESULT STDMETHODCALLTYPE INDSNetAddress_get_AddressType_Proxy(
    INDSNetAddress __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *retval);


void __RPC_STUB INDSNetAddress_get_AddressType_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propput] */ HRESULT STDMETHODCALLTYPE INDSNetAddress_put_AddressType_Proxy(
    INDSNetAddress __RPC_FAR * This,
    /* [in] */ long lnAddressType);


void __RPC_STUB INDSNetAddress_put_AddressType_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propget] */ HRESULT STDMETHODCALLTYPE INDSNetAddress_get_Address_Proxy(
    INDSNetAddress __RPC_FAR * This,
    /* [retval][out] */ VARIANT __RPC_FAR *retval);


void __RPC_STUB INDSNetAddress_get_Address_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propput] */ HRESULT STDMETHODCALLTYPE INDSNetAddress_put_Address_Proxy(
    INDSNetAddress __RPC_FAR * This,
    /* [in] */ VARIANT vAddress);


void __RPC_STUB INDSNetAddress_put_Address_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __INDSNetAddress_INTERFACE_DEFINED__ */


EXTERN_C const CLSID CLSID_NDSNetAddress;

#ifdef __cplusplus

class DECLSPEC_UUID("b0b71247-4080-11d1-a3ac-00c04fb950dc")
NDSNetAddress;
#endif

#ifndef __INDSOctetList_INTERFACE_DEFINED__
#define __INDSOctetList_INTERFACE_DEFINED__

/****************************************
 * Generated header for interface: INDSOctetList
 * at Thu Oct 30 10:33:16 1997
 * using MIDL 3.02.88
 ****************************************/
/* [object][dual][oleautomation][uuid] */



EXTERN_C const IID IID_INDSOctetList;

#if defined(__cplusplus) && !defined(CINTERFACE)

    interface DECLSPEC_UUID("7b28b80f-4680-11d1-a3b4-00c04fb950dc")
    INDSOctetList : public IDispatch
    {
    public:
        virtual /* [id][propget] */ HRESULT STDMETHODCALLTYPE get_OctetList(
            /* [retval][out] */ VARIANT __RPC_FAR *retval) = 0;

        virtual /* [id][propput] */ HRESULT STDMETHODCALLTYPE put_OctetList(
            /* [in] */ VARIANT vOctetList) = 0;

    };

#else 	/* C style interface */

    typedef struct INDSOctetListVtbl
    {
        BEGIN_INTERFACE

        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )(
            INDSOctetList __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);

        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )(
            INDSOctetList __RPC_FAR * This);

        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )(
            INDSOctetList __RPC_FAR * This);

        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )(
            INDSOctetList __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);

        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )(
            INDSOctetList __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);

        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )(
            INDSOctetList __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);

        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )(
            INDSOctetList __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);

        /* [id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_OctetList )(
            INDSOctetList __RPC_FAR * This,
            /* [retval][out] */ VARIANT __RPC_FAR *retval);

        /* [id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_OctetList )(
            INDSOctetList __RPC_FAR * This,
            /* [in] */ VARIANT vOctetList);

        END_INTERFACE
    } INDSOctetListVtbl;

    interface INDSOctetList
    {
        CONST_VTBL struct INDSOctetListVtbl __RPC_FAR *lpVtbl;
    };



#ifdef COBJMACROS


#define INDSOctetList_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define INDSOctetList_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define INDSOctetList_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define INDSOctetList_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define INDSOctetList_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define INDSOctetList_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define INDSOctetList_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define INDSOctetList_get_OctetList(This,retval)	\
    (This)->lpVtbl -> get_OctetList(This,retval)

#define INDSOctetList_put_OctetList(This,vOctetList)	\
    (This)->lpVtbl -> put_OctetList(This,vOctetList)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [id][propget] */ HRESULT STDMETHODCALLTYPE INDSOctetList_get_OctetList_Proxy(
    INDSOctetList __RPC_FAR * This,
    /* [retval][out] */ VARIANT __RPC_FAR *retval);


void __RPC_STUB INDSOctetList_get_OctetList_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propput] */ HRESULT STDMETHODCALLTYPE INDSOctetList_put_OctetList_Proxy(
    INDSOctetList __RPC_FAR * This,
    /* [in] */ VARIANT vOctetList);


void __RPC_STUB INDSOctetList_put_OctetList_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __INDSOctetList_INTERFACE_DEFINED__ */


EXTERN_C const CLSID CLSID_NDSOctetList;

#ifdef __cplusplus

class DECLSPEC_UUID("1241400f-4680-11d1-a3b4-00c04fb950dc")
NDSOctetList;
#endif

#ifndef __INDSEmail_INTERFACE_DEFINED__
#define __INDSEmail_INTERFACE_DEFINED__

/****************************************
 * Generated header for interface: INDSEmail
 * at Thu Oct 30 10:33:16 1997
 * using MIDL 3.02.88
 ****************************************/
/* [object][dual][oleautomation][uuid] */



EXTERN_C const IID IID_INDSEmail;

#if defined(__cplusplus) && !defined(CINTERFACE)

    interface DECLSPEC_UUID("97af011a-478e-11d1-a3b4-00c04fb950dc")
    INDSEmail : public IDispatch
    {
    public:
        virtual /* [id][propget] */ HRESULT STDMETHODCALLTYPE get_Type(
            /* [retval][out] */ long __RPC_FAR *retval) = 0;

        virtual /* [id][propput] */ HRESULT STDMETHODCALLTYPE put_Type(
            /* [in] */ long lnType) = 0;

        virtual /* [id][propget] */ HRESULT STDMETHODCALLTYPE get_Address(
            /* [retval][out] */ BSTR __RPC_FAR *retval) = 0;

        virtual /* [id][propput] */ HRESULT STDMETHODCALLTYPE put_Address(
            /* [in] */ BSTR bstrAddress) = 0;

    };

#else 	/* C style interface */

    typedef struct INDSEmailVtbl
    {
        BEGIN_INTERFACE

        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )(
            INDSEmail __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);

        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )(
            INDSEmail __RPC_FAR * This);

        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )(
            INDSEmail __RPC_FAR * This);

        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )(
            INDSEmail __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);

        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )(
            INDSEmail __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);

        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )(
            INDSEmail __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);

        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )(
            INDSEmail __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);

        /* [id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Type )(
            INDSEmail __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *retval);

        /* [id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Type )(
            INDSEmail __RPC_FAR * This,
            /* [in] */ long lnType);

        /* [id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Address )(
            INDSEmail __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *retval);

        /* [id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Address )(
            INDSEmail __RPC_FAR * This,
            /* [in] */ BSTR bstrAddress);

        END_INTERFACE
    } INDSEmailVtbl;

    interface INDSEmail
    {
        CONST_VTBL struct INDSEmailVtbl __RPC_FAR *lpVtbl;
    };



#ifdef COBJMACROS


#define INDSEmail_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define INDSEmail_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define INDSEmail_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define INDSEmail_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define INDSEmail_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define INDSEmail_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define INDSEmail_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define INDSEmail_get_Type(This,retval)	\
    (This)->lpVtbl -> get_Type(This,retval)

#define INDSEmail_put_Type(This,lnType)	\
    (This)->lpVtbl -> put_Type(This,lnType)

#define INDSEmail_get_Address(This,retval)	\
    (This)->lpVtbl -> get_Address(This,retval)

#define INDSEmail_put_Address(This,bstrAddress)	\
    (This)->lpVtbl -> put_Address(This,bstrAddress)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [id][propget] */ HRESULT STDMETHODCALLTYPE INDSEmail_get_Type_Proxy(
    INDSEmail __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *retval);


void __RPC_STUB INDSEmail_get_Type_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propput] */ HRESULT STDMETHODCALLTYPE INDSEmail_put_Type_Proxy(
    INDSEmail __RPC_FAR * This,
    /* [in] */ long lnType);


void __RPC_STUB INDSEmail_put_Type_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propget] */ HRESULT STDMETHODCALLTYPE INDSEmail_get_Address_Proxy(
    INDSEmail __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *retval);


void __RPC_STUB INDSEmail_get_Address_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propput] */ HRESULT STDMETHODCALLTYPE INDSEmail_put_Address_Proxy(
    INDSEmail __RPC_FAR * This,
    /* [in] */ BSTR bstrAddress);


void __RPC_STUB INDSEmail_put_Address_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __INDSEmail_INTERFACE_DEFINED__ */


EXTERN_C const CLSID CLSID_NDSEmail;

#ifdef __cplusplus

class DECLSPEC_UUID("8f92a857-478e-11d1-a3b4-00c04fb950dc")
NDSEmail;
#endif

#ifndef __INDSPath_INTERFACE_DEFINED__
#define __INDSPath_INTERFACE_DEFINED__

/****************************************
 * Generated header for interface: INDSPath
 * at Thu Oct 30 10:33:16 1997
 * using MIDL 3.02.88
 ****************************************/
/* [object][dual][oleautomation][uuid] */



EXTERN_C const IID IID_INDSPath;

#if defined(__cplusplus) && !defined(CINTERFACE)

    interface DECLSPEC_UUID("b287fcd5-4080-11d1-a3ac-00c04fb950dc")
    INDSPath : public IDispatch
    {
    public:
        virtual /* [id][propget] */ HRESULT STDMETHODCALLTYPE get_Type(
            /* [retval][out] */ long __RPC_FAR *retval) = 0;

        virtual /* [id][propput] */ HRESULT STDMETHODCALLTYPE put_Type(
            /* [in] */ long lnType) = 0;

        virtual /* [id][propget] */ HRESULT STDMETHODCALLTYPE get_VolumeName(
            /* [retval][out] */ BSTR __RPC_FAR *retval) = 0;

        virtual /* [id][propput] */ HRESULT STDMETHODCALLTYPE put_VolumeName(
            /* [in] */ BSTR bstrVolumeName) = 0;

        virtual /* [id][propget] */ HRESULT STDMETHODCALLTYPE get_Path(
            /* [retval][out] */ BSTR __RPC_FAR *retval) = 0;

        virtual /* [id][propput] */ HRESULT STDMETHODCALLTYPE put_Path(
            /* [in] */ BSTR bstrPath) = 0;

    };

#else 	/* C style interface */

    typedef struct INDSPathVtbl
    {
        BEGIN_INTERFACE

        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )(
            INDSPath __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);

        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )(
            INDSPath __RPC_FAR * This);

        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )(
            INDSPath __RPC_FAR * This);

        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )(
            INDSPath __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);

        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )(
            INDSPath __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);

        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )(
            INDSPath __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);

        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )(
            INDSPath __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);

        /* [id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Type )(
            INDSPath __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *retval);

        /* [id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Type )(
            INDSPath __RPC_FAR * This,
            /* [in] */ long lnType);

        /* [id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_VolumeName )(
            INDSPath __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *retval);

        /* [id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_VolumeName )(
            INDSPath __RPC_FAR * This,
            /* [in] */ BSTR bstrVolumeName);

        /* [id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Path )(
            INDSPath __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *retval);

        /* [id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Path )(
            INDSPath __RPC_FAR * This,
            /* [in] */ BSTR bstrPath);

        END_INTERFACE
    } INDSPathVtbl;

    interface INDSPath
    {
        CONST_VTBL struct INDSPathVtbl __RPC_FAR *lpVtbl;
    };



#ifdef COBJMACROS


#define INDSPath_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define INDSPath_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define INDSPath_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define INDSPath_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define INDSPath_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define INDSPath_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define INDSPath_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define INDSPath_get_Type(This,retval)	\
    (This)->lpVtbl -> get_Type(This,retval)

#define INDSPath_put_Type(This,lnType)	\
    (This)->lpVtbl -> put_Type(This,lnType)

#define INDSPath_get_VolumeName(This,retval)	\
    (This)->lpVtbl -> get_VolumeName(This,retval)

#define INDSPath_put_VolumeName(This,bstrVolumeName)	\
    (This)->lpVtbl -> put_VolumeName(This,bstrVolumeName)

#define INDSPath_get_Path(This,retval)	\
    (This)->lpVtbl -> get_Path(This,retval)

#define INDSPath_put_Path(This,bstrPath)	\
    (This)->lpVtbl -> put_Path(This,bstrPath)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [id][propget] */ HRESULT STDMETHODCALLTYPE INDSPath_get_Type_Proxy(
    INDSPath __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *retval);


void __RPC_STUB INDSPath_get_Type_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propput] */ HRESULT STDMETHODCALLTYPE INDSPath_put_Type_Proxy(
    INDSPath __RPC_FAR * This,
    /* [in] */ long lnType);


void __RPC_STUB INDSPath_put_Type_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propget] */ HRESULT STDMETHODCALLTYPE INDSPath_get_VolumeName_Proxy(
    INDSPath __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *retval);


void __RPC_STUB INDSPath_get_VolumeName_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propput] */ HRESULT STDMETHODCALLTYPE INDSPath_put_VolumeName_Proxy(
    INDSPath __RPC_FAR * This,
    /* [in] */ BSTR bstrVolumeName);


void __RPC_STUB INDSPath_put_VolumeName_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propget] */ HRESULT STDMETHODCALLTYPE INDSPath_get_Path_Proxy(
    INDSPath __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *retval);


void __RPC_STUB INDSPath_get_Path_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propput] */ HRESULT STDMETHODCALLTYPE INDSPath_put_Path_Proxy(
    INDSPath __RPC_FAR * This,
    /* [in] */ BSTR bstrPath);


void __RPC_STUB INDSPath_put_Path_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __INDSPath_INTERFACE_DEFINED__ */


EXTERN_C const CLSID CLSID_NDSPath;

#ifdef __cplusplus

class DECLSPEC_UUID("b2538919-4080-11d1-a3ac-00c04fb950dc")
NDSPath;
#endif

#ifndef __INDSReplicaPointer_INTERFACE_DEFINED__
#define __INDSReplicaPointer_INTERFACE_DEFINED__

/****************************************
 * Generated header for interface: INDSReplicaPointer
 * at Thu Oct 30 10:33:16 1997
 * using MIDL 3.02.88
 ****************************************/
/* [object][dual][oleautomation][uuid] */



EXTERN_C const IID IID_INDSReplicaPointer;

#if defined(__cplusplus) && !defined(CINTERFACE)

    interface DECLSPEC_UUID("f60fb803-4080-11d1-a3ac-00c04fb950dc")
    INDSReplicaPointer : public IDispatch
    {
    public:
        virtual /* [id][propget] */ HRESULT STDMETHODCALLTYPE get_ServerName(
            /* [retval][out] */ BSTR __RPC_FAR *retval) = 0;

        virtual /* [id][propput] */ HRESULT STDMETHODCALLTYPE put_ServerName(
            /* [in] */ BSTR bstrServerName) = 0;

        virtual /* [id][propget] */ HRESULT STDMETHODCALLTYPE get_ReplicaType(
            /* [retval][out] */ long __RPC_FAR *retval) = 0;

        virtual /* [id][propput] */ HRESULT STDMETHODCALLTYPE put_ReplicaType(
            /* [in] */ long lnReplicaType) = 0;

        virtual /* [id][propget] */ HRESULT STDMETHODCALLTYPE get_ReplicaNumber(
            /* [retval][out] */ long __RPC_FAR *retval) = 0;

        virtual /* [id][propput] */ HRESULT STDMETHODCALLTYPE put_ReplicaNumber(
            /* [in] */ long lnReplicaNumber) = 0;

        virtual /* [id][propget] */ HRESULT STDMETHODCALLTYPE get_Count(
            /* [retval][out] */ long __RPC_FAR *retval) = 0;

        virtual /* [id][propput] */ HRESULT STDMETHODCALLTYPE put_Count(
            /* [in] */ long lnCount) = 0;

        virtual /* [id][propget] */ HRESULT STDMETHODCALLTYPE get_ReplicaAddressHints(
            /* [retval][out] */ VARIANT __RPC_FAR *retval) = 0;

        virtual /* [id][propput] */ HRESULT STDMETHODCALLTYPE put_ReplicaAddressHints(
            /* [in] */ VARIANT vReplicaAddressHints) = 0;

    };

#else 	/* C style interface */

    typedef struct INDSReplicaPointerVtbl
    {
        BEGIN_INTERFACE

        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )(
            INDSReplicaPointer __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);

        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )(
            INDSReplicaPointer __RPC_FAR * This);

        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )(
            INDSReplicaPointer __RPC_FAR * This);

        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )(
            INDSReplicaPointer __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);

        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )(
            INDSReplicaPointer __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);

        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )(
            INDSReplicaPointer __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);

        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )(
            INDSReplicaPointer __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);

        /* [id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_ServerName )(
            INDSReplicaPointer __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *retval);

        /* [id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_ServerName )(
            INDSReplicaPointer __RPC_FAR * This,
            /* [in] */ BSTR bstrServerName);

        /* [id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_ReplicaType )(
            INDSReplicaPointer __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *retval);

        /* [id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_ReplicaType )(
            INDSReplicaPointer __RPC_FAR * This,
            /* [in] */ long lnReplicaType);

        /* [id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_ReplicaNumber )(
            INDSReplicaPointer __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *retval);

        /* [id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_ReplicaNumber )(
            INDSReplicaPointer __RPC_FAR * This,
            /* [in] */ long lnReplicaNumber);

        /* [id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Count )(
            INDSReplicaPointer __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *retval);

        /* [id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Count )(
            INDSReplicaPointer __RPC_FAR * This,
            /* [in] */ long lnCount);

        /* [id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_ReplicaAddressHints )(
            INDSReplicaPointer __RPC_FAR * This,
            /* [retval][out] */ VARIANT __RPC_FAR *retval);

        /* [id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_ReplicaAddressHints )(
            INDSReplicaPointer __RPC_FAR * This,
            /* [in] */ VARIANT vReplicaAddressHints);

        END_INTERFACE
    } INDSReplicaPointerVtbl;

    interface INDSReplicaPointer
    {
        CONST_VTBL struct INDSReplicaPointerVtbl __RPC_FAR *lpVtbl;
    };



#ifdef COBJMACROS


#define INDSReplicaPointer_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define INDSReplicaPointer_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define INDSReplicaPointer_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define INDSReplicaPointer_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define INDSReplicaPointer_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define INDSReplicaPointer_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define INDSReplicaPointer_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define INDSReplicaPointer_get_ServerName(This,retval)	\
    (This)->lpVtbl -> get_ServerName(This,retval)

#define INDSReplicaPointer_put_ServerName(This,bstrServerName)	\
    (This)->lpVtbl -> put_ServerName(This,bstrServerName)

#define INDSReplicaPointer_get_ReplicaType(This,retval)	\
    (This)->lpVtbl -> get_ReplicaType(This,retval)

#define INDSReplicaPointer_put_ReplicaType(This,lnReplicaType)	\
    (This)->lpVtbl -> put_ReplicaType(This,lnReplicaType)

#define INDSReplicaPointer_get_ReplicaNumber(This,retval)	\
    (This)->lpVtbl -> get_ReplicaNumber(This,retval)

#define INDSReplicaPointer_put_ReplicaNumber(This,lnReplicaNumber)	\
    (This)->lpVtbl -> put_ReplicaNumber(This,lnReplicaNumber)

#define INDSReplicaPointer_get_Count(This,retval)	\
    (This)->lpVtbl -> get_Count(This,retval)

#define INDSReplicaPointer_put_Count(This,lnCount)	\
    (This)->lpVtbl -> put_Count(This,lnCount)

#define INDSReplicaPointer_get_ReplicaAddressHints(This,retval)	\
    (This)->lpVtbl -> get_ReplicaAddressHints(This,retval)

#define INDSReplicaPointer_put_ReplicaAddressHints(This,vReplicaAddressHints)	\
    (This)->lpVtbl -> put_ReplicaAddressHints(This,vReplicaAddressHints)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [id][propget] */ HRESULT STDMETHODCALLTYPE INDSReplicaPointer_get_ServerName_Proxy(
    INDSReplicaPointer __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *retval);


void __RPC_STUB INDSReplicaPointer_get_ServerName_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propput] */ HRESULT STDMETHODCALLTYPE INDSReplicaPointer_put_ServerName_Proxy(
    INDSReplicaPointer __RPC_FAR * This,
    /* [in] */ BSTR bstrServerName);


void __RPC_STUB INDSReplicaPointer_put_ServerName_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propget] */ HRESULT STDMETHODCALLTYPE INDSReplicaPointer_get_ReplicaType_Proxy(
    INDSReplicaPointer __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *retval);


void __RPC_STUB INDSReplicaPointer_get_ReplicaType_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propput] */ HRESULT STDMETHODCALLTYPE INDSReplicaPointer_put_ReplicaType_Proxy(
    INDSReplicaPointer __RPC_FAR * This,
    /* [in] */ long lnReplicaType);


void __RPC_STUB INDSReplicaPointer_put_ReplicaType_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propget] */ HRESULT STDMETHODCALLTYPE INDSReplicaPointer_get_ReplicaNumber_Proxy(
    INDSReplicaPointer __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *retval);


void __RPC_STUB INDSReplicaPointer_get_ReplicaNumber_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propput] */ HRESULT STDMETHODCALLTYPE INDSReplicaPointer_put_ReplicaNumber_Proxy(
    INDSReplicaPointer __RPC_FAR * This,
    /* [in] */ long lnReplicaNumber);


void __RPC_STUB INDSReplicaPointer_put_ReplicaNumber_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propget] */ HRESULT STDMETHODCALLTYPE INDSReplicaPointer_get_Count_Proxy(
    INDSReplicaPointer __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *retval);


void __RPC_STUB INDSReplicaPointer_get_Count_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propput] */ HRESULT STDMETHODCALLTYPE INDSReplicaPointer_put_Count_Proxy(
    INDSReplicaPointer __RPC_FAR * This,
    /* [in] */ long lnCount);


void __RPC_STUB INDSReplicaPointer_put_Count_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propget] */ HRESULT STDMETHODCALLTYPE INDSReplicaPointer_get_ReplicaAddressHints_Proxy(
    INDSReplicaPointer __RPC_FAR * This,
    /* [retval][out] */ VARIANT __RPC_FAR *retval);


void __RPC_STUB INDSReplicaPointer_get_ReplicaAddressHints_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propput] */ HRESULT STDMETHODCALLTYPE INDSReplicaPointer_put_ReplicaAddressHints_Proxy(
    INDSReplicaPointer __RPC_FAR * This,
    /* [in] */ VARIANT vReplicaAddressHints);


void __RPC_STUB INDSReplicaPointer_put_ReplicaAddressHints_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __INDSReplicaPointer_INTERFACE_DEFINED__ */


EXTERN_C const CLSID CLSID_NDSReplicaPointer;

#ifdef __cplusplus

class DECLSPEC_UUID("f5d1badf-4080-11d1-a3ac-00c04fb950dc")
NDSReplicaPointer;
#endif

#ifndef __INDSAcl_INTERFACE_DEFINED__
#define __INDSAcl_INTERFACE_DEFINED__

/****************************************
 * Generated header for interface: INDSAcl
 * at Thu Oct 30 10:33:16 1997
 * using MIDL 3.02.88
 ****************************************/
/* [object][dual][oleautomation][uuid] */



EXTERN_C const IID IID_INDSAcl;

#if defined(__cplusplus) && !defined(CINTERFACE)

    interface DECLSPEC_UUID("8452d3ab-0869-11d1-a377-00c04fb950dc")
    INDSAcl : public IDispatch
    {
    public:
        virtual /* [id][propget] */ HRESULT STDMETHODCALLTYPE get_ProtectedAttrName(
            /* [retval][out] */ BSTR __RPC_FAR *retval) = 0;

        virtual /* [id][propput] */ HRESULT STDMETHODCALLTYPE put_ProtectedAttrName(
            /* [in] */ BSTR bstrProtectedAttrName) = 0;

        virtual /* [id][propget] */ HRESULT STDMETHODCALLTYPE get_SubjectName(
            /* [retval][out] */ BSTR __RPC_FAR *retval) = 0;

        virtual /* [id][propput] */ HRESULT STDMETHODCALLTYPE put_SubjectName(
            /* [in] */ BSTR bstrSubjectName) = 0;

        virtual /* [id][propget] */ HRESULT STDMETHODCALLTYPE get_Privileges(
            /* [retval][out] */ long __RPC_FAR *retval) = 0;

        virtual /* [id][propput] */ HRESULT STDMETHODCALLTYPE put_Privileges(
            /* [in] */ long lnPrivileges) = 0;

        virtual /* [id] */ HRESULT STDMETHODCALLTYPE CopyAcl(
            /* [retval][out] */ IDispatch __RPC_FAR *__RPC_FAR *ppAcl) = 0;

    };

#else 	/* C style interface */

    typedef struct INDSAclVtbl
    {
        BEGIN_INTERFACE

        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )(
            INDSAcl __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);

        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )(
            INDSAcl __RPC_FAR * This);

        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )(
            INDSAcl __RPC_FAR * This);

        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )(
            INDSAcl __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);

        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )(
            INDSAcl __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);

        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )(
            INDSAcl __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);

        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )(
            INDSAcl __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);

        /* [id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_ProtectedAttrName )(
            INDSAcl __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *retval);

        /* [id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_ProtectedAttrName )(
            INDSAcl __RPC_FAR * This,
            /* [in] */ BSTR bstrProtectedAttrName);

        /* [id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_SubjectName )(
            INDSAcl __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *retval);

        /* [id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_SubjectName )(
            INDSAcl __RPC_FAR * This,
            /* [in] */ BSTR bstrSubjectName);

        /* [id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Privileges )(
            INDSAcl __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *retval);

        /* [id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Privileges )(
            INDSAcl __RPC_FAR * This,
            /* [in] */ long lnPrivileges);

        /* [id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *CopyAcl )(
            INDSAcl __RPC_FAR * This,
            /* [retval][out] */ IDispatch __RPC_FAR *__RPC_FAR *ppAcl);

        END_INTERFACE
    } INDSAclVtbl;

    interface INDSAcl
    {
        CONST_VTBL struct INDSAclVtbl __RPC_FAR *lpVtbl;
    };



#ifdef COBJMACROS


#define INDSAcl_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define INDSAcl_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define INDSAcl_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define INDSAcl_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define INDSAcl_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define INDSAcl_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define INDSAcl_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define INDSAcl_get_ProtectedAttrName(This,retval)	\
    (This)->lpVtbl -> get_ProtectedAttrName(This,retval)

#define INDSAcl_put_ProtectedAttrName(This,bstrProtectedAttrName)	\
    (This)->lpVtbl -> put_ProtectedAttrName(This,bstrProtectedAttrName)

#define INDSAcl_get_SubjectName(This,retval)	\
    (This)->lpVtbl -> get_SubjectName(This,retval)

#define INDSAcl_put_SubjectName(This,bstrSubjectName)	\
    (This)->lpVtbl -> put_SubjectName(This,bstrSubjectName)

#define INDSAcl_get_Privileges(This,retval)	\
    (This)->lpVtbl -> get_Privileges(This,retval)

#define INDSAcl_put_Privileges(This,lnPrivileges)	\
    (This)->lpVtbl -> put_Privileges(This,lnPrivileges)

#define INDSAcl_CopyAcl(This,ppAcl)	\
    (This)->lpVtbl -> CopyAcl(This,ppAcl)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [id][propget] */ HRESULT STDMETHODCALLTYPE INDSAcl_get_ProtectedAttrName_Proxy(
    INDSAcl __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *retval);


void __RPC_STUB INDSAcl_get_ProtectedAttrName_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propput] */ HRESULT STDMETHODCALLTYPE INDSAcl_put_ProtectedAttrName_Proxy(
    INDSAcl __RPC_FAR * This,
    /* [in] */ BSTR bstrProtectedAttrName);


void __RPC_STUB INDSAcl_put_ProtectedAttrName_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propget] */ HRESULT STDMETHODCALLTYPE INDSAcl_get_SubjectName_Proxy(
    INDSAcl __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *retval);


void __RPC_STUB INDSAcl_get_SubjectName_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propput] */ HRESULT STDMETHODCALLTYPE INDSAcl_put_SubjectName_Proxy(
    INDSAcl __RPC_FAR * This,
    /* [in] */ BSTR bstrSubjectName);


void __RPC_STUB INDSAcl_put_SubjectName_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propget] */ HRESULT STDMETHODCALLTYPE INDSAcl_get_Privileges_Proxy(
    INDSAcl __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *retval);


void __RPC_STUB INDSAcl_get_Privileges_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propput] */ HRESULT STDMETHODCALLTYPE INDSAcl_put_Privileges_Proxy(
    INDSAcl __RPC_FAR * This,
    /* [in] */ long lnPrivileges);


void __RPC_STUB INDSAcl_put_Privileges_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id] */ HRESULT STDMETHODCALLTYPE INDSAcl_CopyAcl_Proxy(
    INDSAcl __RPC_FAR * This,
    /* [retval][out] */ IDispatch __RPC_FAR *__RPC_FAR *ppAcl);


void __RPC_STUB INDSAcl_CopyAcl_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __INDSAcl_INTERFACE_DEFINED__ */


EXTERN_C const CLSID CLSID_NDSAcl;

#ifdef __cplusplus

class DECLSPEC_UUID("7af1efb6-0869-11d1-a377-00c04fb950dc")
NDSAcl;
#endif

#ifndef __INDSTimeStamp_INTERFACE_DEFINED__
#define __INDSTimeStamp_INTERFACE_DEFINED__

/****************************************
 * Generated header for interface: INDSTimeStamp
 * at Thu Oct 30 10:33:16 1997
 * using MIDL 3.02.88
 ****************************************/
/* [object][dual][oleautomation][uuid] */



EXTERN_C const IID IID_INDSTimeStamp;

#if defined(__cplusplus) && !defined(CINTERFACE)

    interface DECLSPEC_UUID("b2f5a901-4080-11d1-a3ac-00c04fb950dc")
    INDSTimeStamp : public IDispatch
    {
    public:
        virtual /* [id][propget] */ HRESULT STDMETHODCALLTYPE get_WholeSeconds(
            /* [retval][out] */ long __RPC_FAR *retval) = 0;

        virtual /* [id][propput] */ HRESULT STDMETHODCALLTYPE put_WholeSeconds(
            /* [in] */ long lnWholeSeconds) = 0;

        virtual /* [id][propget] */ HRESULT STDMETHODCALLTYPE get_EventID(
            /* [retval][out] */ long __RPC_FAR *retval) = 0;

        virtual /* [id][propput] */ HRESULT STDMETHODCALLTYPE put_EventID(
            /* [in] */ long lnEventID) = 0;

    };

#else 	/* C style interface */

    typedef struct INDSTimeStampVtbl
    {
        BEGIN_INTERFACE

        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )(
            INDSTimeStamp __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);

        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )(
            INDSTimeStamp __RPC_FAR * This);

        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )(
            INDSTimeStamp __RPC_FAR * This);

        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )(
            INDSTimeStamp __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);

        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )(
            INDSTimeStamp __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);

        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )(
            INDSTimeStamp __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);

        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )(
            INDSTimeStamp __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);

        /* [id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_WholeSeconds )(
            INDSTimeStamp __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *retval);

        /* [id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_WholeSeconds )(
            INDSTimeStamp __RPC_FAR * This,
            /* [in] */ long lnWholeSeconds);

        /* [id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_EventID )(
            INDSTimeStamp __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *retval);

        /* [id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_EventID )(
            INDSTimeStamp __RPC_FAR * This,
            /* [in] */ long lnEventID);

        END_INTERFACE
    } INDSTimeStampVtbl;

    interface INDSTimeStamp
    {
        CONST_VTBL struct INDSTimeStampVtbl __RPC_FAR *lpVtbl;
    };



#ifdef COBJMACROS


#define INDSTimeStamp_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define INDSTimeStamp_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define INDSTimeStamp_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define INDSTimeStamp_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define INDSTimeStamp_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define INDSTimeStamp_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define INDSTimeStamp_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define INDSTimeStamp_get_WholeSeconds(This,retval)	\
    (This)->lpVtbl -> get_WholeSeconds(This,retval)

#define INDSTimeStamp_put_WholeSeconds(This,lnWholeSeconds)	\
    (This)->lpVtbl -> put_WholeSeconds(This,lnWholeSeconds)

#define INDSTimeStamp_get_EventID(This,retval)	\
    (This)->lpVtbl -> get_EventID(This,retval)

#define INDSTimeStamp_put_EventID(This,lnEventID)	\
    (This)->lpVtbl -> put_EventID(This,lnEventID)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [id][propget] */ HRESULT STDMETHODCALLTYPE INDSTimeStamp_get_WholeSeconds_Proxy(
    INDSTimeStamp __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *retval);


void __RPC_STUB INDSTimeStamp_get_WholeSeconds_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propput] */ HRESULT STDMETHODCALLTYPE INDSTimeStamp_put_WholeSeconds_Proxy(
    INDSTimeStamp __RPC_FAR * This,
    /* [in] */ long lnWholeSeconds);


void __RPC_STUB INDSTimeStamp_put_WholeSeconds_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propget] */ HRESULT STDMETHODCALLTYPE INDSTimeStamp_get_EventID_Proxy(
    INDSTimeStamp __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *retval);


void __RPC_STUB INDSTimeStamp_get_EventID_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propput] */ HRESULT STDMETHODCALLTYPE INDSTimeStamp_put_EventID_Proxy(
    INDSTimeStamp __RPC_FAR * This,
    /* [in] */ long lnEventID);


void __RPC_STUB INDSTimeStamp_put_EventID_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __INDSTimeStamp_INTERFACE_DEFINED__ */


EXTERN_C const CLSID CLSID_NDSTimeStamp;

#ifdef __cplusplus

class DECLSPEC_UUID("b2bed2eb-4080-11d1-a3ac-00c04fb950dc")
NDSTimeStamp;
#endif

#ifndef __INDSPostalAddress_INTERFACE_DEFINED__
#define __INDSPostalAddress_INTERFACE_DEFINED__

/****************************************
 * Generated header for interface: INDSPostalAddress
 * at Thu Oct 30 10:33:16 1997
 * using MIDL 3.02.88
 ****************************************/
/* [object][dual][oleautomation][uuid] */



EXTERN_C const IID IID_INDSPostalAddress;

#if defined(__cplusplus) && !defined(CINTERFACE)

    interface DECLSPEC_UUID("7adecf29-4680-11d1-a3b4-00c04fb950dc")
    INDSPostalAddress : public IDispatch
    {
    public:
        virtual /* [id][propget] */ HRESULT STDMETHODCALLTYPE get_PostalAddress(
            /* [retval][out] */ VARIANT __RPC_FAR *retval) = 0;

        virtual /* [id][propput] */ HRESULT STDMETHODCALLTYPE put_PostalAddress(
            /* [in] */ VARIANT vPostalAddress) = 0;

    };

#else 	/* C style interface */

    typedef struct INDSPostalAddressVtbl
    {
        BEGIN_INTERFACE

        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )(
            INDSPostalAddress __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);

        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )(
            INDSPostalAddress __RPC_FAR * This);

        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )(
            INDSPostalAddress __RPC_FAR * This);

        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )(
            INDSPostalAddress __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);

        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )(
            INDSPostalAddress __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);

        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )(
            INDSPostalAddress __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);

        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )(
            INDSPostalAddress __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);

        /* [id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_PostalAddress )(
            INDSPostalAddress __RPC_FAR * This,
            /* [retval][out] */ VARIANT __RPC_FAR *retval);

        /* [id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_PostalAddress )(
            INDSPostalAddress __RPC_FAR * This,
            /* [in] */ VARIANT vPostalAddress);

        END_INTERFACE
    } INDSPostalAddressVtbl;

    interface INDSPostalAddress
    {
        CONST_VTBL struct INDSPostalAddressVtbl __RPC_FAR *lpVtbl;
    };



#ifdef COBJMACROS


#define INDSPostalAddress_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define INDSPostalAddress_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define INDSPostalAddress_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define INDSPostalAddress_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define INDSPostalAddress_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define INDSPostalAddress_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define INDSPostalAddress_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define INDSPostalAddress_get_PostalAddress(This,retval)	\
    (This)->lpVtbl -> get_PostalAddress(This,retval)

#define INDSPostalAddress_put_PostalAddress(This,vPostalAddress)	\
    (This)->lpVtbl -> put_PostalAddress(This,vPostalAddress)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [id][propget] */ HRESULT STDMETHODCALLTYPE INDSPostalAddress_get_PostalAddress_Proxy(
    INDSPostalAddress __RPC_FAR * This,
    /* [retval][out] */ VARIANT __RPC_FAR *retval);


void __RPC_STUB INDSPostalAddress_get_PostalAddress_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propput] */ HRESULT STDMETHODCALLTYPE INDSPostalAddress_put_PostalAddress_Proxy(
    INDSPostalAddress __RPC_FAR * This,
    /* [in] */ VARIANT vPostalAddress);


void __RPC_STUB INDSPostalAddress_put_PostalAddress_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __INDSPostalAddress_INTERFACE_DEFINED__ */


EXTERN_C const CLSID CLSID_NDSPostalAddress;

#ifdef __cplusplus

class DECLSPEC_UUID("0a75afcd-4680-11d1-a3b4-00c04fb950dc")
NDSPostalAddress;
#endif

#ifndef __INDSBackLink_INTERFACE_DEFINED__
#define __INDSBackLink_INTERFACE_DEFINED__

/****************************************
 * Generated header for interface: INDSBackLink
 * at Thu Oct 30 10:33:16 1997
 * using MIDL 3.02.88
 ****************************************/
/* [object][dual][oleautomation][uuid] */



EXTERN_C const IID IID_INDSBackLink;

#if defined(__cplusplus) && !defined(CINTERFACE)

    interface DECLSPEC_UUID("fd1302bd-4080-11d1-a3ac-00c04fb950dc")
    INDSBackLink : public IDispatch
    {
    public:
        virtual /* [id][propget] */ HRESULT STDMETHODCALLTYPE get_RemoteID(
            /* [retval][out] */ long __RPC_FAR *retval) = 0;

        virtual /* [id][propput] */ HRESULT STDMETHODCALLTYPE put_RemoteID(
            /* [in] */ long lnRemoteID) = 0;

        virtual /* [id][propget] */ HRESULT STDMETHODCALLTYPE get_ObjectName(
            /* [retval][out] */ BSTR __RPC_FAR *retval) = 0;

        virtual /* [id][propput] */ HRESULT STDMETHODCALLTYPE put_ObjectName(
            /* [in] */ BSTR bstrObjectName) = 0;

    };

#else 	/* C style interface */

    typedef struct INDSBackLinkVtbl
    {
        BEGIN_INTERFACE

        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )(
            INDSBackLink __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);

        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )(
            INDSBackLink __RPC_FAR * This);

        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )(
            INDSBackLink __RPC_FAR * This);

        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )(
            INDSBackLink __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);

        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )(
            INDSBackLink __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);

        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )(
            INDSBackLink __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);

        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )(
            INDSBackLink __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);

        /* [id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_RemoteID )(
            INDSBackLink __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *retval);

        /* [id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_RemoteID )(
            INDSBackLink __RPC_FAR * This,
            /* [in] */ long lnRemoteID);

        /* [id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_ObjectName )(
            INDSBackLink __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *retval);

        /* [id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_ObjectName )(
            INDSBackLink __RPC_FAR * This,
            /* [in] */ BSTR bstrObjectName);

        END_INTERFACE
    } INDSBackLinkVtbl;

    interface INDSBackLink
    {
        CONST_VTBL struct INDSBackLinkVtbl __RPC_FAR *lpVtbl;
    };



#ifdef COBJMACROS


#define INDSBackLink_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define INDSBackLink_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define INDSBackLink_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define INDSBackLink_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define INDSBackLink_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define INDSBackLink_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define INDSBackLink_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define INDSBackLink_get_RemoteID(This,retval)	\
    (This)->lpVtbl -> get_RemoteID(This,retval)

#define INDSBackLink_put_RemoteID(This,lnRemoteID)	\
    (This)->lpVtbl -> put_RemoteID(This,lnRemoteID)

#define INDSBackLink_get_ObjectName(This,retval)	\
    (This)->lpVtbl -> get_ObjectName(This,retval)

#define INDSBackLink_put_ObjectName(This,bstrObjectName)	\
    (This)->lpVtbl -> put_ObjectName(This,bstrObjectName)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [id][propget] */ HRESULT STDMETHODCALLTYPE INDSBackLink_get_RemoteID_Proxy(
    INDSBackLink __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *retval);


void __RPC_STUB INDSBackLink_get_RemoteID_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propput] */ HRESULT STDMETHODCALLTYPE INDSBackLink_put_RemoteID_Proxy(
    INDSBackLink __RPC_FAR * This,
    /* [in] */ long lnRemoteID);


void __RPC_STUB INDSBackLink_put_RemoteID_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propget] */ HRESULT STDMETHODCALLTYPE INDSBackLink_get_ObjectName_Proxy(
    INDSBackLink __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *retval);


void __RPC_STUB INDSBackLink_get_ObjectName_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propput] */ HRESULT STDMETHODCALLTYPE INDSBackLink_put_ObjectName_Proxy(
    INDSBackLink __RPC_FAR * This,
    /* [in] */ BSTR bstrObjectName);


void __RPC_STUB INDSBackLink_put_ObjectName_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __INDSBackLink_INTERFACE_DEFINED__ */


EXTERN_C const CLSID CLSID_NDSBackLink;

#ifdef __cplusplus

class DECLSPEC_UUID("fcbf906f-4080-11d1-a3ac-00c04fb950dc")
NDSBackLink;
#endif

#ifndef __INDSTypedName_INTERFACE_DEFINED__
#define __INDSTypedName_INTERFACE_DEFINED__

/****************************************
 * Generated header for interface: INDSTypedName
 * at Thu Oct 30 10:33:16 1997
 * using MIDL 3.02.88
 ****************************************/
/* [object][dual][oleautomation][uuid] */



EXTERN_C const IID IID_INDSTypedName;

#if defined(__cplusplus) && !defined(CINTERFACE)

    interface DECLSPEC_UUID("b371a349-4080-11d1-a3ac-00c04fb950dc")
    INDSTypedName : public IDispatch
    {
    public:
        virtual /* [id][propget] */ HRESULT STDMETHODCALLTYPE get_ObjectName(
            /* [retval][out] */ BSTR __RPC_FAR *retval) = 0;

        virtual /* [id][propput] */ HRESULT STDMETHODCALLTYPE put_ObjectName(
            /* [in] */ BSTR bstrObjectName) = 0;

        virtual /* [id][propget] */ HRESULT STDMETHODCALLTYPE get_Level(
            /* [retval][out] */ long __RPC_FAR *retval) = 0;

        virtual /* [id][propput] */ HRESULT STDMETHODCALLTYPE put_Level(
            /* [in] */ long lnLevel) = 0;

        virtual /* [id][propget] */ HRESULT STDMETHODCALLTYPE get_Interval(
            /* [retval][out] */ long __RPC_FAR *retval) = 0;

        virtual /* [id][propput] */ HRESULT STDMETHODCALLTYPE put_Interval(
            /* [in] */ long lnInterval) = 0;

    };

#else 	/* C style interface */

    typedef struct INDSTypedNameVtbl
    {
        BEGIN_INTERFACE

        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )(
            INDSTypedName __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);

        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )(
            INDSTypedName __RPC_FAR * This);

        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )(
            INDSTypedName __RPC_FAR * This);

        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )(
            INDSTypedName __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);

        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )(
            INDSTypedName __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);

        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )(
            INDSTypedName __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);

        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )(
            INDSTypedName __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);

        /* [id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_ObjectName )(
            INDSTypedName __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *retval);

        /* [id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_ObjectName )(
            INDSTypedName __RPC_FAR * This,
            /* [in] */ BSTR bstrObjectName);

        /* [id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Level )(
            INDSTypedName __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *retval);

        /* [id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Level )(
            INDSTypedName __RPC_FAR * This,
            /* [in] */ long lnLevel);

        /* [id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Interval )(
            INDSTypedName __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *retval);

        /* [id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Interval )(
            INDSTypedName __RPC_FAR * This,
            /* [in] */ long lnInterval);

        END_INTERFACE
    } INDSTypedNameVtbl;

    interface INDSTypedName
    {
        CONST_VTBL struct INDSTypedNameVtbl __RPC_FAR *lpVtbl;
    };



#ifdef COBJMACROS


#define INDSTypedName_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define INDSTypedName_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define INDSTypedName_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define INDSTypedName_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define INDSTypedName_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define INDSTypedName_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define INDSTypedName_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define INDSTypedName_get_ObjectName(This,retval)	\
    (This)->lpVtbl -> get_ObjectName(This,retval)

#define INDSTypedName_put_ObjectName(This,bstrObjectName)	\
    (This)->lpVtbl -> put_ObjectName(This,bstrObjectName)

#define INDSTypedName_get_Level(This,retval)	\
    (This)->lpVtbl -> get_Level(This,retval)

#define INDSTypedName_put_Level(This,lnLevel)	\
    (This)->lpVtbl -> put_Level(This,lnLevel)

#define INDSTypedName_get_Interval(This,retval)	\
    (This)->lpVtbl -> get_Interval(This,retval)

#define INDSTypedName_put_Interval(This,lnInterval)	\
    (This)->lpVtbl -> put_Interval(This,lnInterval)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [id][propget] */ HRESULT STDMETHODCALLTYPE INDSTypedName_get_ObjectName_Proxy(
    INDSTypedName __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *retval);


void __RPC_STUB INDSTypedName_get_ObjectName_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propput] */ HRESULT STDMETHODCALLTYPE INDSTypedName_put_ObjectName_Proxy(
    INDSTypedName __RPC_FAR * This,
    /* [in] */ BSTR bstrObjectName);


void __RPC_STUB INDSTypedName_put_ObjectName_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propget] */ HRESULT STDMETHODCALLTYPE INDSTypedName_get_Level_Proxy(
    INDSTypedName __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *retval);


void __RPC_STUB INDSTypedName_get_Level_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propput] */ HRESULT STDMETHODCALLTYPE INDSTypedName_put_Level_Proxy(
    INDSTypedName __RPC_FAR * This,
    /* [in] */ long lnLevel);


void __RPC_STUB INDSTypedName_put_Level_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propget] */ HRESULT STDMETHODCALLTYPE INDSTypedName_get_Interval_Proxy(
    INDSTypedName __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *retval);


void __RPC_STUB INDSTypedName_get_Interval_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propput] */ HRESULT STDMETHODCALLTYPE INDSTypedName_put_Interval_Proxy(
    INDSTypedName __RPC_FAR * This,
    /* [in] */ long lnInterval);


void __RPC_STUB INDSTypedName_put_Interval_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __INDSTypedName_INTERFACE_DEFINED__ */


EXTERN_C const CLSID CLSID_NDSTypedName;

#ifdef __cplusplus

class DECLSPEC_UUID("b33143cb-4080-11d1-a3ac-00c04fb950dc")
NDSTypedName;
#endif

#ifndef __INDSHold_INTERFACE_DEFINED__
#define __INDSHold_INTERFACE_DEFINED__

/****************************************
 * Generated header for interface: INDSHold
 * at Thu Oct 30 10:33:16 1997
 * using MIDL 3.02.88
 ****************************************/
/* [object][dual][oleautomation][uuid] */



EXTERN_C const IID IID_INDSHold;

#if defined(__cplusplus) && !defined(CINTERFACE)

    interface DECLSPEC_UUID("b3eb3b37-4080-11d1-a3ac-00c04fb950dc")
    INDSHold : public IDispatch
    {
    public:
        virtual /* [id][propget] */ HRESULT STDMETHODCALLTYPE get_ObjectName(
            /* [retval][out] */ BSTR __RPC_FAR *retval) = 0;

        virtual /* [id][propput] */ HRESULT STDMETHODCALLTYPE put_ObjectName(
            /* [in] */ BSTR bstrObjectName) = 0;

        virtual /* [id][propget] */ HRESULT STDMETHODCALLTYPE get_Amount(
            /* [retval][out] */ long __RPC_FAR *retval) = 0;

        virtual /* [id][propput] */ HRESULT STDMETHODCALLTYPE put_Amount(
            /* [in] */ long lnAmount) = 0;

    };

#else 	/* C style interface */

    typedef struct INDSHoldVtbl
    {
        BEGIN_INTERFACE

        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )(
            INDSHold __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);

        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )(
            INDSHold __RPC_FAR * This);

        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )(
            INDSHold __RPC_FAR * This);

        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )(
            INDSHold __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);

        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )(
            INDSHold __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);

        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )(
            INDSHold __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);

        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )(
            INDSHold __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);

        /* [id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_ObjectName )(
            INDSHold __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *retval);

        /* [id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_ObjectName )(
            INDSHold __RPC_FAR * This,
            /* [in] */ BSTR bstrObjectName);

        /* [id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Amount )(
            INDSHold __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *retval);

        /* [id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Amount )(
            INDSHold __RPC_FAR * This,
            /* [in] */ long lnAmount);

        END_INTERFACE
    } INDSHoldVtbl;

    interface INDSHold
    {
        CONST_VTBL struct INDSHoldVtbl __RPC_FAR *lpVtbl;
    };



#ifdef COBJMACROS


#define INDSHold_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define INDSHold_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define INDSHold_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define INDSHold_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define INDSHold_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define INDSHold_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define INDSHold_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define INDSHold_get_ObjectName(This,retval)	\
    (This)->lpVtbl -> get_ObjectName(This,retval)

#define INDSHold_put_ObjectName(This,bstrObjectName)	\
    (This)->lpVtbl -> put_ObjectName(This,bstrObjectName)

#define INDSHold_get_Amount(This,retval)	\
    (This)->lpVtbl -> get_Amount(This,retval)

#define INDSHold_put_Amount(This,lnAmount)	\
    (This)->lpVtbl -> put_Amount(This,lnAmount)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [id][propget] */ HRESULT STDMETHODCALLTYPE INDSHold_get_ObjectName_Proxy(
    INDSHold __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *retval);


void __RPC_STUB INDSHold_get_ObjectName_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propput] */ HRESULT STDMETHODCALLTYPE INDSHold_put_ObjectName_Proxy(
    INDSHold __RPC_FAR * This,
    /* [in] */ BSTR bstrObjectName);


void __RPC_STUB INDSHold_put_ObjectName_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propget] */ HRESULT STDMETHODCALLTYPE INDSHold_get_Amount_Proxy(
    INDSHold __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *retval);


void __RPC_STUB INDSHold_get_Amount_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propput] */ HRESULT STDMETHODCALLTYPE INDSHold_put_Amount_Proxy(
    INDSHold __RPC_FAR * This,
    /* [in] */ long lnAmount);


void __RPC_STUB INDSHold_put_Amount_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __INDSHold_INTERFACE_DEFINED__ */


EXTERN_C const CLSID CLSID_NDSHold;

#ifdef __cplusplus

class DECLSPEC_UUID("b3ad3e13-4080-11d1-a3ac-00c04fb950dc")
NDSHold;
#endif
#endif /* __NDSOle_LIBRARY_DEFINED__ */

/* Additional Prototypes for ALL interfaces */

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif
