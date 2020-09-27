
#pragma warning( disable: 4049 )  /* more than 64k source lines */

/* this ALWAYS GENERATED file contains the proxy stub code */


 /* File created by MIDL compiler version 6.00.0347 */
/* at Sun May 05 19:07:35 2002
 */
/* Compiler settings for oaidl.idl:
    Oicf, W1, Zp8, env=Win32 (32b run)
    protocol : dce , ms_ext, c_ext, robust
    error checks: allocation ref bounds_check enum stub_data 
    VC __declspec() decoration level: 
         __declspec(uuid()), __declspec(selectany), __declspec(novtable)
         DECLSPEC_UUID(), MIDL_INTERFACE()
*/
//@@MIDL_FILE_HEADING(  )

#if !defined(_M_IA64) && !defined(_M_AMD64)
#define USE_STUBLESS_PROXY


/* verify that the <rpcproxy.h> version is high enough to compile this file*/
#ifndef __REDQ_RPCPROXY_H_VERSION__
#define __REQUIRED_RPCPROXY_H_VERSION__ 475
#endif


#include "rpcproxy.h"
#ifndef __RPCPROXY_H_VERSION__
#error this stub requires an updated version of <rpcproxy.h>
#endif // __RPCPROXY_H_VERSION__


#include "oaidl.h"

#define TYPE_FORMAT_STRING_SIZE   2023                              
#define PROC_FORMAT_STRING_SIZE   3151                              
#define TRANSMIT_AS_TABLE_SIZE    0            
#define WIRE_MARSHAL_TABLE_SIZE   3            

typedef struct _MIDL_TYPE_FORMAT_STRING
    {
    short          Pad;
    unsigned char  Format[ TYPE_FORMAT_STRING_SIZE ];
    } MIDL_TYPE_FORMAT_STRING;

typedef struct _MIDL_PROC_FORMAT_STRING
    {
    short          Pad;
    unsigned char  Format[ PROC_FORMAT_STRING_SIZE ];
    } MIDL_PROC_FORMAT_STRING;


static RPC_SYNTAX_IDENTIFIER  _RpcTransferSyntax = 
{{0x8A885D04,0x1CEB,0x11C9,{0x9F,0xE8,0x08,0x00,0x2B,0x10,0x48,0x60}},{2,0}};


extern const MIDL_TYPE_FORMAT_STRING __MIDL_TypeFormatString;
extern const MIDL_PROC_FORMAT_STRING __MIDL_ProcFormatString;


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO IDispatch_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO IDispatch_ProxyInfo;

/* [call_as] */ HRESULT STDMETHODCALLTYPE IDispatch_RemoteInvoke_Proxy( 
    IDispatch * This,
    /* [in] */ DISPID dispIdMember,
    /* [in] */ REFIID riid,
    /* [in] */ LCID lcid,
    /* [in] */ DWORD dwFlags,
    /* [in] */ DISPPARAMS *pDispParams,
    /* [out] */ VARIANT *pVarResult,
    /* [out] */ EXCEPINFO *pExcepInfo,
    /* [out] */ UINT *pArgErr,
    /* [in] */ UINT cVarRef,
    /* [size_is][in] */ UINT *rgVarRefIdx,
    /* [size_is][out][in] */ VARIANTARG *rgVarRef)
{
CLIENT_CALL_RETURN _RetVal;

_RetVal = NdrClientCall2(
                  ( PMIDL_STUB_DESC  )&Object_StubDesc,
                  (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[144],
                  ( unsigned char * )&This);
return ( HRESULT  )_RetVal.Simple;

}

void __RPC_API
IDispatch_RemoteInvoke_Thunk(
    PMIDL_STUB_MESSAGE pStubMsg )
{
    #pragma pack(4)
    struct _PARAM_STRUCT
        {
        IDispatch *This;
        DISPID dispIdMember;
        REFIID riid;
        LCID lcid;
        DWORD dwFlags;
        DISPPARAMS *pDispParams;
        VARIANT *pVarResult;
        EXCEPINFO *pExcepInfo;
        UINT *pArgErr;
        UINT cVarRef;
        UINT *rgVarRefIdx;
        VARIANTARG *rgVarRef;
        HRESULT _RetVal;
        };
    #pragma pack()
    struct _PARAM_STRUCT * pParamStruct;

    pParamStruct = (struct _PARAM_STRUCT *) pStubMsg->StackTop;
    
    /* Call the server */
    
    pParamStruct->_RetVal = IDispatch_Invoke_Stub(
                                (IDispatch *) pParamStruct->This,
                                pParamStruct->dispIdMember,
                                pParamStruct->riid,
                                pParamStruct->lcid,
                                pParamStruct->dwFlags,
                                pParamStruct->pDispParams,
                                pParamStruct->pVarResult,
                                pParamStruct->pExcepInfo,
                                pParamStruct->pArgErr,
                                pParamStruct->cVarRef,
                                pParamStruct->rgVarRefIdx,
                                pParamStruct->rgVarRef);
    
}


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO IEnumVARIANT_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO IEnumVARIANT_ProxyInfo;

/* [call_as] */ HRESULT STDMETHODCALLTYPE IEnumVARIANT_RemoteNext_Proxy( 
    IEnumVARIANT * This,
    /* [in] */ ULONG celt,
    /* [length_is][size_is][out] */ VARIANT *rgVar,
    /* [out] */ ULONG *pCeltFetched)
{
CLIENT_CALL_RETURN _RetVal;

_RetVal = NdrClientCall2(
                  ( PMIDL_STUB_DESC  )&Object_StubDesc,
                  (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[240],
                  ( unsigned char * )&This);
return ( HRESULT  )_RetVal.Simple;

}

void __RPC_API
IEnumVARIANT_RemoteNext_Thunk(
    PMIDL_STUB_MESSAGE pStubMsg )
{
    #pragma pack(4)
    struct _PARAM_STRUCT
        {
        IEnumVARIANT *This;
        ULONG celt;
        VARIANT *rgVar;
        ULONG *pCeltFetched;
        HRESULT _RetVal;
        };
    #pragma pack()
    struct _PARAM_STRUCT * pParamStruct;

    pParamStruct = (struct _PARAM_STRUCT *) pStubMsg->StackTop;
    
    /* Call the server */
    
    pParamStruct->_RetVal = IEnumVARIANT_Next_Stub(
                                 (IEnumVARIANT *) pParamStruct->This,
                                 pParamStruct->celt,
                                 pParamStruct->rgVar,
                                 pParamStruct->pCeltFetched);
    
}


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO ITypeComp_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO ITypeComp_ProxyInfo;

/* [call_as] */ HRESULT STDMETHODCALLTYPE ITypeComp_RemoteBind_Proxy( 
    ITypeComp * This,
    /* [in] */ LPOLESTR szName,
    /* [in] */ ULONG lHashVal,
    /* [in] */ WORD wFlags,
    /* [out] */ ITypeInfo **ppTInfo,
    /* [out] */ DESCKIND *pDescKind,
    /* [out] */ LPFUNCDESC *ppFuncDesc,
    /* [out] */ LPVARDESC *ppVarDesc,
    /* [out] */ ITypeComp **ppTypeComp,
    /* [out] */ CLEANLOCALSTORAGE *pDummy)
{
CLIENT_CALL_RETURN _RetVal;

_RetVal = NdrClientCall2(
                  ( PMIDL_STUB_DESC  )&Object_StubDesc,
                  (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[390],
                  ( unsigned char * )&This);
return ( HRESULT  )_RetVal.Simple;

}

void __RPC_API
ITypeComp_RemoteBind_Thunk(
    PMIDL_STUB_MESSAGE pStubMsg )
{
    #pragma pack(4)
    struct _PARAM_STRUCT
        {
        ITypeComp *This;
        LPOLESTR szName;
        ULONG lHashVal;
        WORD wFlags;
        char Pad0[2];
        ITypeInfo **ppTInfo;
        DESCKIND *pDescKind;
        LPFUNCDESC *ppFuncDesc;
        LPVARDESC *ppVarDesc;
        ITypeComp **ppTypeComp;
        CLEANLOCALSTORAGE *pDummy;
        HRESULT _RetVal;
        };
    #pragma pack()
    struct _PARAM_STRUCT * pParamStruct;

    pParamStruct = (struct _PARAM_STRUCT *) pStubMsg->StackTop;
    
    /* Call the server */
    
    pParamStruct->_RetVal = ITypeComp_Bind_Stub(
                              (ITypeComp *) pParamStruct->This,
                              pParamStruct->szName,
                              pParamStruct->lHashVal,
                              pParamStruct->wFlags,
                              pParamStruct->ppTInfo,
                              pParamStruct->pDescKind,
                              pParamStruct->ppFuncDesc,
                              pParamStruct->ppVarDesc,
                              pParamStruct->ppTypeComp,
                              pParamStruct->pDummy);
    
}

/* [call_as] */ HRESULT STDMETHODCALLTYPE ITypeComp_RemoteBindType_Proxy( 
    ITypeComp * This,
    /* [in] */ LPOLESTR szName,
    /* [in] */ ULONG lHashVal,
    /* [out] */ ITypeInfo **ppTInfo)
{
CLIENT_CALL_RETURN _RetVal;

_RetVal = NdrClientCall2(
                  ( PMIDL_STUB_DESC  )&Object_StubDesc,
                  (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[474],
                  ( unsigned char * )&This);
return ( HRESULT  )_RetVal.Simple;

}

void __RPC_API
ITypeComp_RemoteBindType_Thunk(
    PMIDL_STUB_MESSAGE pStubMsg )
{
    #pragma pack(4)
    struct _PARAM_STRUCT
        {
        ITypeComp *This;
        LPOLESTR szName;
        ULONG lHashVal;
        ITypeInfo **ppTInfo;
        HRESULT _RetVal;
        };
    #pragma pack()
    struct _PARAM_STRUCT * pParamStruct;

    pParamStruct = (struct _PARAM_STRUCT *) pStubMsg->StackTop;
    
    /* Call the server */
    
    pParamStruct->_RetVal = ITypeComp_BindType_Stub(
                                  (ITypeComp *) pParamStruct->This,
                                  pParamStruct->szName,
                                  pParamStruct->lHashVal,
                                  pParamStruct->ppTInfo);
    
}


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO ITypeInfo_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO ITypeInfo_ProxyInfo;

/* [call_as] */ HRESULT STDMETHODCALLTYPE ITypeInfo_RemoteGetTypeAttr_Proxy( 
    ITypeInfo * This,
    /* [out] */ LPTYPEATTR *ppTypeAttr,
    /* [out] */ CLEANLOCALSTORAGE *pDummy)
{
CLIENT_CALL_RETURN _RetVal;

_RetVal = NdrClientCall2(
                  ( PMIDL_STUB_DESC  )&Object_StubDesc,
                  (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[522],
                  ( unsigned char * )&This);
return ( HRESULT  )_RetVal.Simple;

}

void __RPC_API
ITypeInfo_RemoteGetTypeAttr_Thunk(
    PMIDL_STUB_MESSAGE pStubMsg )
{
    #pragma pack(4)
    struct _PARAM_STRUCT
        {
        ITypeInfo *This;
        LPTYPEATTR *ppTypeAttr;
        CLEANLOCALSTORAGE *pDummy;
        HRESULT _RetVal;
        };
    #pragma pack()
    struct _PARAM_STRUCT * pParamStruct;

    pParamStruct = (struct _PARAM_STRUCT *) pStubMsg->StackTop;
    
    /* Call the server */
    
    pParamStruct->_RetVal = ITypeInfo_GetTypeAttr_Stub(
                                     (ITypeInfo *) pParamStruct->This,
                                     pParamStruct->ppTypeAttr,
                                     pParamStruct->pDummy);
    
}

/* [call_as] */ HRESULT STDMETHODCALLTYPE ITypeInfo_RemoteGetFuncDesc_Proxy( 
    ITypeInfo * This,
    /* [in] */ UINT index,
    /* [out] */ LPFUNCDESC *ppFuncDesc,
    /* [out] */ CLEANLOCALSTORAGE *pDummy)
{
CLIENT_CALL_RETURN _RetVal;

_RetVal = NdrClientCall2(
                  ( PMIDL_STUB_DESC  )&Object_StubDesc,
                  (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[600],
                  ( unsigned char * )&This);
return ( HRESULT  )_RetVal.Simple;

}

void __RPC_API
ITypeInfo_RemoteGetFuncDesc_Thunk(
    PMIDL_STUB_MESSAGE pStubMsg )
{
    #pragma pack(4)
    struct _PARAM_STRUCT
        {
        ITypeInfo *This;
        UINT index;
        LPFUNCDESC *ppFuncDesc;
        CLEANLOCALSTORAGE *pDummy;
        HRESULT _RetVal;
        };
    #pragma pack()
    struct _PARAM_STRUCT * pParamStruct;

    pParamStruct = (struct _PARAM_STRUCT *) pStubMsg->StackTop;
    
    /* Call the server */
    
    pParamStruct->_RetVal = ITypeInfo_GetFuncDesc_Stub(
                                     (ITypeInfo *) pParamStruct->This,
                                     pParamStruct->index,
                                     pParamStruct->ppFuncDesc,
                                     pParamStruct->pDummy);
    
}

/* [call_as] */ HRESULT STDMETHODCALLTYPE ITypeInfo_RemoteGetVarDesc_Proxy( 
    ITypeInfo * This,
    /* [in] */ UINT index,
    /* [out] */ LPVARDESC *ppVarDesc,
    /* [out] */ CLEANLOCALSTORAGE *pDummy)
{
CLIENT_CALL_RETURN _RetVal;

_RetVal = NdrClientCall2(
                  ( PMIDL_STUB_DESC  )&Object_StubDesc,
                  (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[648],
                  ( unsigned char * )&This);
return ( HRESULT  )_RetVal.Simple;

}

void __RPC_API
ITypeInfo_RemoteGetVarDesc_Thunk(
    PMIDL_STUB_MESSAGE pStubMsg )
{
    #pragma pack(4)
    struct _PARAM_STRUCT
        {
        ITypeInfo *This;
        UINT index;
        LPVARDESC *ppVarDesc;
        CLEANLOCALSTORAGE *pDummy;
        HRESULT _RetVal;
        };
    #pragma pack()
    struct _PARAM_STRUCT * pParamStruct;

    pParamStruct = (struct _PARAM_STRUCT *) pStubMsg->StackTop;
    
    /* Call the server */
    
    pParamStruct->_RetVal = ITypeInfo_GetVarDesc_Stub(
                                    (ITypeInfo *) pParamStruct->This,
                                    pParamStruct->index,
                                    pParamStruct->ppVarDesc,
                                    pParamStruct->pDummy);
    
}

/* [call_as] */ HRESULT STDMETHODCALLTYPE ITypeInfo_RemoteGetNames_Proxy( 
    ITypeInfo * This,
    /* [in] */ MEMBERID memid,
    /* [length_is][size_is][out] */ BSTR *rgBstrNames,
    /* [in] */ UINT cMaxNames,
    /* [out] */ UINT *pcNames)
{
CLIENT_CALL_RETURN _RetVal;

_RetVal = NdrClientCall2(
                  ( PMIDL_STUB_DESC  )&Object_StubDesc,
                  (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[696],
                  ( unsigned char * )&This);
return ( HRESULT  )_RetVal.Simple;

}

void __RPC_API
ITypeInfo_RemoteGetNames_Thunk(
    PMIDL_STUB_MESSAGE pStubMsg )
{
    #pragma pack(4)
    struct _PARAM_STRUCT
        {
        ITypeInfo *This;
        MEMBERID memid;
        BSTR *rgBstrNames;
        UINT cMaxNames;
        UINT *pcNames;
        HRESULT _RetVal;
        };
    #pragma pack()
    struct _PARAM_STRUCT * pParamStruct;

    pParamStruct = (struct _PARAM_STRUCT *) pStubMsg->StackTop;
    
    /* Call the server */
    
    pParamStruct->_RetVal = ITypeInfo_GetNames_Stub(
                                  (ITypeInfo *) pParamStruct->This,
                                  pParamStruct->memid,
                                  pParamStruct->rgBstrNames,
                                  pParamStruct->cMaxNames,
                                  pParamStruct->pcNames);
    
}

void __RPC_API
ITypeInfo_LocalGetIDsOfNames_Thunk(
    PMIDL_STUB_MESSAGE pStubMsg )
{
    #pragma pack(4)
    struct _PARAM_STRUCT
        {
        ITypeInfo *This;
        HRESULT _RetVal;
        };
    #pragma pack()
    struct _PARAM_STRUCT * pParamStruct;

    pParamStruct = (struct _PARAM_STRUCT *) pStubMsg->StackTop;
    
    /* Call the server */
    
    pParamStruct->_RetVal = ITypeInfo_GetIDsOfNames_Stub((ITypeInfo *) pParamStruct->This);
    
}

void __RPC_API
ITypeInfo_LocalInvoke_Thunk(
    PMIDL_STUB_MESSAGE pStubMsg )
{
    #pragma pack(4)
    struct _PARAM_STRUCT
        {
        ITypeInfo *This;
        HRESULT _RetVal;
        };
    #pragma pack()
    struct _PARAM_STRUCT * pParamStruct;

    pParamStruct = (struct _PARAM_STRUCT *) pStubMsg->StackTop;
    
    /* Call the server */
    
    pParamStruct->_RetVal = ITypeInfo_Invoke_Stub((ITypeInfo *) pParamStruct->This);
    
}

/* [call_as] */ HRESULT STDMETHODCALLTYPE ITypeInfo_RemoteGetDocumentation_Proxy( 
    ITypeInfo * This,
    /* [in] */ MEMBERID memid,
    /* [in] */ DWORD refPtrFlags,
    /* [out] */ BSTR *pBstrName,
    /* [out] */ BSTR *pBstrDocString,
    /* [out] */ DWORD *pdwHelpContext,
    /* [out] */ BSTR *pBstrHelpFile)
{
CLIENT_CALL_RETURN _RetVal;

_RetVal = NdrClientCall2(
                  ( PMIDL_STUB_DESC  )&Object_StubDesc,
                  (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[894],
                  ( unsigned char * )&This);
return ( HRESULT  )_RetVal.Simple;

}

void __RPC_API
ITypeInfo_RemoteGetDocumentation_Thunk(
    PMIDL_STUB_MESSAGE pStubMsg )
{
    #pragma pack(4)
    struct _PARAM_STRUCT
        {
        ITypeInfo *This;
        MEMBERID memid;
        DWORD refPtrFlags;
        BSTR *pBstrName;
        BSTR *pBstrDocString;
        DWORD *pdwHelpContext;
        BSTR *pBstrHelpFile;
        HRESULT _RetVal;
        };
    #pragma pack()
    struct _PARAM_STRUCT * pParamStruct;

    pParamStruct = (struct _PARAM_STRUCT *) pStubMsg->StackTop;
    
    /* Call the server */
    
    pParamStruct->_RetVal = ITypeInfo_GetDocumentation_Stub(
                                          (ITypeInfo *) pParamStruct->This,
                                          pParamStruct->memid,
                                          pParamStruct->refPtrFlags,
                                          pParamStruct->pBstrName,
                                          pParamStruct->pBstrDocString,
                                          pParamStruct->pdwHelpContext,
                                          pParamStruct->pBstrHelpFile);
    
}

/* [call_as] */ HRESULT STDMETHODCALLTYPE ITypeInfo_RemoteGetDllEntry_Proxy( 
    ITypeInfo * This,
    /* [in] */ MEMBERID memid,
    /* [in] */ INVOKEKIND invKind,
    /* [in] */ DWORD refPtrFlags,
    /* [out] */ BSTR *pBstrDllName,
    /* [out] */ BSTR *pBstrName,
    /* [out] */ WORD *pwOrdinal)
{
CLIENT_CALL_RETURN _RetVal;

_RetVal = NdrClientCall2(
                  ( PMIDL_STUB_DESC  )&Object_StubDesc,
                  (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[960],
                  ( unsigned char * )&This);
return ( HRESULT  )_RetVal.Simple;

}

void __RPC_API
ITypeInfo_RemoteGetDllEntry_Thunk(
    PMIDL_STUB_MESSAGE pStubMsg )
{
    #pragma pack(4)
    struct _PARAM_STRUCT
        {
        ITypeInfo *This;
        MEMBERID memid;
        INVOKEKIND invKind;
        DWORD refPtrFlags;
        BSTR *pBstrDllName;
        BSTR *pBstrName;
        WORD *pwOrdinal;
        HRESULT _RetVal;
        };
    #pragma pack()
    struct _PARAM_STRUCT * pParamStruct;

    pParamStruct = (struct _PARAM_STRUCT *) pStubMsg->StackTop;
    
    /* Call the server */
    
    pParamStruct->_RetVal = ITypeInfo_GetDllEntry_Stub(
                                     (ITypeInfo *) pParamStruct->This,
                                     pParamStruct->memid,
                                     pParamStruct->invKind,
                                     pParamStruct->refPtrFlags,
                                     pParamStruct->pBstrDllName,
                                     pParamStruct->pBstrName,
                                     pParamStruct->pwOrdinal);
    
}

void __RPC_API
ITypeInfo_LocalAddressOfMember_Thunk(
    PMIDL_STUB_MESSAGE pStubMsg )
{
    #pragma pack(4)
    struct _PARAM_STRUCT
        {
        ITypeInfo *This;
        HRESULT _RetVal;
        };
    #pragma pack()
    struct _PARAM_STRUCT * pParamStruct;

    pParamStruct = (struct _PARAM_STRUCT *) pStubMsg->StackTop;
    
    /* Call the server */
    
    pParamStruct->_RetVal = ITypeInfo_AddressOfMember_Stub((ITypeInfo *) pParamStruct->This);
    
}

/* [call_as] */ HRESULT STDMETHODCALLTYPE ITypeInfo_RemoteCreateInstance_Proxy( 
    ITypeInfo * This,
    /* [in] */ REFIID riid,
    /* [iid_is][out] */ IUnknown **ppvObj)
{
CLIENT_CALL_RETURN _RetVal;

_RetVal = NdrClientCall2(
                  ( PMIDL_STUB_DESC  )&Object_StubDesc,
                  (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[1098],
                  ( unsigned char * )&This);
return ( HRESULT  )_RetVal.Simple;

}

void __RPC_API
ITypeInfo_RemoteCreateInstance_Thunk(
    PMIDL_STUB_MESSAGE pStubMsg )
{
    #pragma pack(4)
    struct _PARAM_STRUCT
        {
        ITypeInfo *This;
        REFIID riid;
        IUnknown **ppvObj;
        HRESULT _RetVal;
        };
    #pragma pack()
    struct _PARAM_STRUCT * pParamStruct;

    pParamStruct = (struct _PARAM_STRUCT *) pStubMsg->StackTop;
    
    /* Call the server */
    
    pParamStruct->_RetVal = ITypeInfo_CreateInstance_Stub(
                                        (ITypeInfo *) pParamStruct->This,
                                        pParamStruct->riid,
                                        pParamStruct->ppvObj);
    
}

/* [call_as] */ HRESULT STDMETHODCALLTYPE ITypeInfo_RemoteGetContainingTypeLib_Proxy( 
    ITypeInfo * This,
    /* [out] */ ITypeLib **ppTLib,
    /* [out] */ UINT *pIndex)
{
CLIENT_CALL_RETURN _RetVal;

_RetVal = NdrClientCall2(
                  ( PMIDL_STUB_DESC  )&Object_StubDesc,
                  (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[1182],
                  ( unsigned char * )&This);
return ( HRESULT  )_RetVal.Simple;

}

void __RPC_API
ITypeInfo_RemoteGetContainingTypeLib_Thunk(
    PMIDL_STUB_MESSAGE pStubMsg )
{
    #pragma pack(4)
    struct _PARAM_STRUCT
        {
        ITypeInfo *This;
        ITypeLib **ppTLib;
        UINT *pIndex;
        HRESULT _RetVal;
        };
    #pragma pack()
    struct _PARAM_STRUCT * pParamStruct;

    pParamStruct = (struct _PARAM_STRUCT *) pStubMsg->StackTop;
    
    /* Call the server */
    
    pParamStruct->_RetVal = ITypeInfo_GetContainingTypeLib_Stub(
                                              (ITypeInfo *) pParamStruct->This,
                                              pParamStruct->ppTLib,
                                              pParamStruct->pIndex);
    
}

void __RPC_API
ITypeInfo_LocalReleaseTypeAttr_Thunk(
    PMIDL_STUB_MESSAGE pStubMsg )
{
    #pragma pack(4)
    struct _PARAM_STRUCT
        {
        ITypeInfo *This;
        HRESULT _RetVal;
        };
    #pragma pack()
    struct _PARAM_STRUCT * pParamStruct;

    pParamStruct = (struct _PARAM_STRUCT *) pStubMsg->StackTop;
    
    /* Call the server */
    
    pParamStruct->_RetVal = ITypeInfo_ReleaseTypeAttr_Stub((ITypeInfo *) pParamStruct->This);
    
}

void __RPC_API
ITypeInfo_LocalReleaseFuncDesc_Thunk(
    PMIDL_STUB_MESSAGE pStubMsg )
{
    #pragma pack(4)
    struct _PARAM_STRUCT
        {
        ITypeInfo *This;
        HRESULT _RetVal;
        };
    #pragma pack()
    struct _PARAM_STRUCT * pParamStruct;

    pParamStruct = (struct _PARAM_STRUCT *) pStubMsg->StackTop;
    
    /* Call the server */
    
    pParamStruct->_RetVal = ITypeInfo_ReleaseFuncDesc_Stub((ITypeInfo *) pParamStruct->This);
    
}

void __RPC_API
ITypeInfo_LocalReleaseVarDesc_Thunk(
    PMIDL_STUB_MESSAGE pStubMsg )
{
    #pragma pack(4)
    struct _PARAM_STRUCT
        {
        ITypeInfo *This;
        HRESULT _RetVal;
        };
    #pragma pack()
    struct _PARAM_STRUCT * pParamStruct;

    pParamStruct = (struct _PARAM_STRUCT *) pStubMsg->StackTop;
    
    /* Call the server */
    
    pParamStruct->_RetVal = ITypeInfo_ReleaseVarDesc_Stub((ITypeInfo *) pParamStruct->This);
    
}


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO ITypeInfo2_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO ITypeInfo2_ProxyInfo;

/* [call_as] */ HRESULT STDMETHODCALLTYPE ITypeInfo2_RemoteGetDocumentation2_Proxy( 
    ITypeInfo2 * This,
    /* [in] */ MEMBERID memid,
    /* [in] */ LCID lcid,
    /* [in] */ DWORD refPtrFlags,
    /* [out] */ BSTR *pbstrHelpString,
    /* [out] */ DWORD *pdwHelpStringContext,
    /* [out] */ BSTR *pbstrHelpStringDll)
{
CLIENT_CALL_RETURN _RetVal;

_RetVal = NdrClientCall2(
                  ( PMIDL_STUB_DESC  )&Object_StubDesc,
                  (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[1716],
                  ( unsigned char * )&This);
return ( HRESULT  )_RetVal.Simple;

}

void __RPC_API
ITypeInfo2_RemoteGetDocumentation2_Thunk(
    PMIDL_STUB_MESSAGE pStubMsg )
{
    #pragma pack(4)
    struct _PARAM_STRUCT
        {
        ITypeInfo2 *This;
        MEMBERID memid;
        LCID lcid;
        DWORD refPtrFlags;
        BSTR *pbstrHelpString;
        DWORD *pdwHelpStringContext;
        BSTR *pbstrHelpStringDll;
        HRESULT _RetVal;
        };
    #pragma pack()
    struct _PARAM_STRUCT * pParamStruct;

    pParamStruct = (struct _PARAM_STRUCT *) pStubMsg->StackTop;
    
    /* Call the server */
    
    pParamStruct->_RetVal = ITypeInfo2_GetDocumentation2_Stub(
                                            (ITypeInfo2 *) pParamStruct->This,
                                            pParamStruct->memid,
                                            pParamStruct->lcid,
                                            pParamStruct->refPtrFlags,
                                            pParamStruct->pbstrHelpString,
                                            pParamStruct->pdwHelpStringContext,
                                            pParamStruct->pbstrHelpStringDll);
    
}


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO ITypeLib_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO ITypeLib_ProxyInfo;

/* [call_as] */ HRESULT STDMETHODCALLTYPE ITypeLib_RemoteGetTypeInfoCount_Proxy( 
    ITypeLib * This,
    /* [out] */ UINT *pcTInfo)
{
CLIENT_CALL_RETURN _RetVal;

_RetVal = NdrClientCall2(
                  ( PMIDL_STUB_DESC  )&Object_StubDesc,
                  (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[0],
                  ( unsigned char * )&This);
return ( HRESULT  )_RetVal.Simple;

}

void __RPC_API
ITypeLib_RemoteGetTypeInfoCount_Thunk(
    PMIDL_STUB_MESSAGE pStubMsg )
{
    #pragma pack(4)
    struct _PARAM_STRUCT
        {
        ITypeLib *This;
        UINT *pcTInfo;
        HRESULT _RetVal;
        };
    #pragma pack()
    struct _PARAM_STRUCT * pParamStruct;

    pParamStruct = (struct _PARAM_STRUCT *) pStubMsg->StackTop;
    
    /* Call the server */
    
    pParamStruct->_RetVal = ITypeLib_GetTypeInfoCount_Stub((ITypeLib *) pParamStruct->This,pParamStruct->pcTInfo);
    
}

/* [call_as] */ HRESULT STDMETHODCALLTYPE ITypeLib_RemoteGetLibAttr_Proxy( 
    ITypeLib * This,
    /* [out] */ LPTLIBATTR *ppTLibAttr,
    /* [out] */ CLEANLOCALSTORAGE *pDummy)
{
CLIENT_CALL_RETURN _RetVal;

_RetVal = NdrClientCall2(
                  ( PMIDL_STUB_DESC  )&Object_StubDesc,
                  (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[2118],
                  ( unsigned char * )&This);
return ( HRESULT  )_RetVal.Simple;

}

void __RPC_API
ITypeLib_RemoteGetLibAttr_Thunk(
    PMIDL_STUB_MESSAGE pStubMsg )
{
    #pragma pack(4)
    struct _PARAM_STRUCT
        {
        ITypeLib *This;
        LPTLIBATTR *ppTLibAttr;
        CLEANLOCALSTORAGE *pDummy;
        HRESULT _RetVal;
        };
    #pragma pack()
    struct _PARAM_STRUCT * pParamStruct;

    pParamStruct = (struct _PARAM_STRUCT *) pStubMsg->StackTop;
    
    /* Call the server */
    
    pParamStruct->_RetVal = ITypeLib_GetLibAttr_Stub(
                                   (ITypeLib *) pParamStruct->This,
                                   pParamStruct->ppTLibAttr,
                                   pParamStruct->pDummy);
    
}

/* [call_as] */ HRESULT STDMETHODCALLTYPE ITypeLib_RemoteGetDocumentation_Proxy( 
    ITypeLib * This,
    /* [in] */ INT index,
    /* [in] */ DWORD refPtrFlags,
    /* [out] */ BSTR *pBstrName,
    /* [out] */ BSTR *pBstrDocString,
    /* [out] */ DWORD *pdwHelpContext,
    /* [out] */ BSTR *pBstrHelpFile)
{
CLIENT_CALL_RETURN _RetVal;

_RetVal = NdrClientCall2(
                  ( PMIDL_STUB_DESC  )&Object_StubDesc,
                  (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[2196],
                  ( unsigned char * )&This);
return ( HRESULT  )_RetVal.Simple;

}

void __RPC_API
ITypeLib_RemoteGetDocumentation_Thunk(
    PMIDL_STUB_MESSAGE pStubMsg )
{
    #pragma pack(4)
    struct _PARAM_STRUCT
        {
        ITypeLib *This;
        INT index;
        DWORD refPtrFlags;
        BSTR *pBstrName;
        BSTR *pBstrDocString;
        DWORD *pdwHelpContext;
        BSTR *pBstrHelpFile;
        HRESULT _RetVal;
        };
    #pragma pack()
    struct _PARAM_STRUCT * pParamStruct;

    pParamStruct = (struct _PARAM_STRUCT *) pStubMsg->StackTop;
    
    /* Call the server */
    
    pParamStruct->_RetVal = ITypeLib_GetDocumentation_Stub(
                                         (ITypeLib *) pParamStruct->This,
                                         pParamStruct->index,
                                         pParamStruct->refPtrFlags,
                                         pParamStruct->pBstrName,
                                         pParamStruct->pBstrDocString,
                                         pParamStruct->pdwHelpContext,
                                         pParamStruct->pBstrHelpFile);
    
}

/* [call_as] */ HRESULT STDMETHODCALLTYPE ITypeLib_RemoteIsName_Proxy( 
    ITypeLib * This,
    /* [in] */ LPOLESTR szNameBuf,
    /* [in] */ ULONG lHashVal,
    /* [out] */ BOOL *pfName,
    /* [out] */ BSTR *pBstrLibName)
{
CLIENT_CALL_RETURN _RetVal;

_RetVal = NdrClientCall2(
                  ( PMIDL_STUB_DESC  )&Object_StubDesc,
                  (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[2262],
                  ( unsigned char * )&This);
return ( HRESULT  )_RetVal.Simple;

}

void __RPC_API
ITypeLib_RemoteIsName_Thunk(
    PMIDL_STUB_MESSAGE pStubMsg )
{
    #pragma pack(4)
    struct _PARAM_STRUCT
        {
        ITypeLib *This;
        LPOLESTR szNameBuf;
        ULONG lHashVal;
        BOOL *pfName;
        BSTR *pBstrLibName;
        HRESULT _RetVal;
        };
    #pragma pack()
    struct _PARAM_STRUCT * pParamStruct;

    pParamStruct = (struct _PARAM_STRUCT *) pStubMsg->StackTop;
    
    /* Call the server */
    
    pParamStruct->_RetVal = ITypeLib_IsName_Stub(
                               (ITypeLib *) pParamStruct->This,
                               pParamStruct->szNameBuf,
                               pParamStruct->lHashVal,
                               pParamStruct->pfName,
                               pParamStruct->pBstrLibName);
    
}

/* [call_as] */ HRESULT STDMETHODCALLTYPE ITypeLib_RemoteFindName_Proxy( 
    ITypeLib * This,
    /* [in] */ LPOLESTR szNameBuf,
    /* [in] */ ULONG lHashVal,
    /* [length_is][size_is][out] */ ITypeInfo **ppTInfo,
    /* [length_is][size_is][out] */ MEMBERID *rgMemId,
    /* [out][in] */ USHORT *pcFound,
    /* [out] */ BSTR *pBstrLibName)
{
CLIENT_CALL_RETURN _RetVal;

_RetVal = NdrClientCall2(
                  ( PMIDL_STUB_DESC  )&Object_StubDesc,
                  (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[2316],
                  ( unsigned char * )&This);
return ( HRESULT  )_RetVal.Simple;

}

void __RPC_API
ITypeLib_RemoteFindName_Thunk(
    PMIDL_STUB_MESSAGE pStubMsg )
{
    #pragma pack(4)
    struct _PARAM_STRUCT
        {
        ITypeLib *This;
        LPOLESTR szNameBuf;
        ULONG lHashVal;
        ITypeInfo **ppTInfo;
        MEMBERID *rgMemId;
        USHORT *pcFound;
        BSTR *pBstrLibName;
        HRESULT _RetVal;
        };
    #pragma pack()
    struct _PARAM_STRUCT * pParamStruct;

    pParamStruct = (struct _PARAM_STRUCT *) pStubMsg->StackTop;
    
    /* Call the server */
    
    pParamStruct->_RetVal = ITypeLib_FindName_Stub(
                                 (ITypeLib *) pParamStruct->This,
                                 pParamStruct->szNameBuf,
                                 pParamStruct->lHashVal,
                                 pParamStruct->ppTInfo,
                                 pParamStruct->rgMemId,
                                 pParamStruct->pcFound,
                                 pParamStruct->pBstrLibName);
    
}

void __RPC_API
ITypeLib_LocalReleaseTLibAttr_Thunk(
    PMIDL_STUB_MESSAGE pStubMsg )
{
    #pragma pack(4)
    struct _PARAM_STRUCT
        {
        ITypeLib *This;
        HRESULT _RetVal;
        };
    #pragma pack()
    struct _PARAM_STRUCT * pParamStruct;

    pParamStruct = (struct _PARAM_STRUCT *) pStubMsg->StackTop;
    
    /* Call the server */
    
    pParamStruct->_RetVal = ITypeLib_ReleaseTLibAttr_Stub((ITypeLib *) pParamStruct->This);
    
}


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO ITypeLib2_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO ITypeLib2_ProxyInfo;

/* [call_as] */ HRESULT STDMETHODCALLTYPE ITypeLib2_RemoteGetLibStatistics_Proxy( 
    ITypeLib2 * This,
    /* [out] */ ULONG *pcUniqueNames,
    /* [out] */ ULONG *pcchUniqueNames)
{
CLIENT_CALL_RETURN _RetVal;

_RetVal = NdrClientCall2(
                  ( PMIDL_STUB_DESC  )&Object_StubDesc,
                  (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[2454],
                  ( unsigned char * )&This);
return ( HRESULT  )_RetVal.Simple;

}

void __RPC_API
ITypeLib2_RemoteGetLibStatistics_Thunk(
    PMIDL_STUB_MESSAGE pStubMsg )
{
    #pragma pack(4)
    struct _PARAM_STRUCT
        {
        ITypeLib2 *This;
        ULONG *pcUniqueNames;
        ULONG *pcchUniqueNames;
        HRESULT _RetVal;
        };
    #pragma pack()
    struct _PARAM_STRUCT * pParamStruct;

    pParamStruct = (struct _PARAM_STRUCT *) pStubMsg->StackTop;
    
    /* Call the server */
    
    pParamStruct->_RetVal = ITypeLib2_GetLibStatistics_Stub(
                                          (ITypeLib2 *) pParamStruct->This,
                                          pParamStruct->pcUniqueNames,
                                          pParamStruct->pcchUniqueNames);
    
}

/* [call_as] */ HRESULT STDMETHODCALLTYPE ITypeLib2_RemoteGetDocumentation2_Proxy( 
    ITypeLib2 * This,
    /* [in] */ INT index,
    /* [in] */ LCID lcid,
    /* [in] */ DWORD refPtrFlags,
    /* [out] */ BSTR *pbstrHelpString,
    /* [out] */ DWORD *pdwHelpStringContext,
    /* [out] */ BSTR *pbstrHelpStringDll)
{
CLIENT_CALL_RETURN _RetVal;

_RetVal = NdrClientCall2(
                  ( PMIDL_STUB_DESC  )&Object_StubDesc,
                  (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[2496],
                  ( unsigned char * )&This);
return ( HRESULT  )_RetVal.Simple;

}

void __RPC_API
ITypeLib2_RemoteGetDocumentation2_Thunk(
    PMIDL_STUB_MESSAGE pStubMsg )
{
    #pragma pack(4)
    struct _PARAM_STRUCT
        {
        ITypeLib2 *This;
        INT index;
        LCID lcid;
        DWORD refPtrFlags;
        BSTR *pbstrHelpString;
        DWORD *pdwHelpStringContext;
        BSTR *pbstrHelpStringDll;
        HRESULT _RetVal;
        };
    #pragma pack()
    struct _PARAM_STRUCT * pParamStruct;

    pParamStruct = (struct _PARAM_STRUCT *) pStubMsg->StackTop;
    
    /* Call the server */
    
    pParamStruct->_RetVal = ITypeLib2_GetDocumentation2_Stub(
                                           (ITypeLib2 *) pParamStruct->This,
                                           pParamStruct->index,
                                           pParamStruct->lcid,
                                           pParamStruct->refPtrFlags,
                                           pParamStruct->pbstrHelpString,
                                           pParamStruct->pdwHelpStringContext,
                                           pParamStruct->pbstrHelpStringDll);
    
}


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO IErrorInfo_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO IErrorInfo_ProxyInfo;


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO ICreateErrorInfo_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO ICreateErrorInfo_ProxyInfo;


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO ISupportErrorInfo_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO ISupportErrorInfo_ProxyInfo;


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO ITypeFactory_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO ITypeFactory_ProxyInfo;


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO IErrorLog_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO IErrorLog_ProxyInfo;


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO IPropertyBag_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO IPropertyBag_ProxyInfo;

/* [call_as] */ HRESULT STDMETHODCALLTYPE IPropertyBag_RemoteRead_Proxy( 
    IPropertyBag * This,
    /* [in] */ LPCOLESTR pszPropName,
    /* [out] */ VARIANT *pVar,
    /* [in] */ IErrorLog *pErrorLog,
    /* [in] */ DWORD varType,
    /* [in] */ IUnknown *pUnkObj)
{
CLIENT_CALL_RETURN _RetVal;

_RetVal = NdrClientCall2(
                  ( PMIDL_STUB_DESC  )&Object_StubDesc,
                  (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[3048],
                  ( unsigned char * )&This);
return ( HRESULT  )_RetVal.Simple;

}

void __RPC_API
IPropertyBag_RemoteRead_Thunk(
    PMIDL_STUB_MESSAGE pStubMsg )
{
    #pragma pack(4)
    struct _PARAM_STRUCT
        {
        IPropertyBag *This;
        LPCOLESTR pszPropName;
        VARIANT *pVar;
        IErrorLog *pErrorLog;
        DWORD varType;
        IUnknown *pUnkObj;
        HRESULT _RetVal;
        };
    #pragma pack()
    struct _PARAM_STRUCT * pParamStruct;

    pParamStruct = (struct _PARAM_STRUCT *) pStubMsg->StackTop;
    
    /* Call the server */
    
    pParamStruct->_RetVal = IPropertyBag_Read_Stub(
                                 (IPropertyBag *) pParamStruct->This,
                                 pParamStruct->pszPropName,
                                 pParamStruct->pVar,
                                 pParamStruct->pErrorLog,
                                 pParamStruct->varType,
                                 pParamStruct->pUnkObj);
    
}


extern const USER_MARSHAL_ROUTINE_QUADRUPLE UserMarshalRoutines[ WIRE_MARSHAL_TABLE_SIZE ];

#if !defined(__RPC_WIN32__)
#error  Invalid build platform for this stub.
#endif

#if !(TARGET_IS_NT50_OR_LATER)
#error You need a Windows 2000 or later to run this stub because it uses these features:
#error   /robust command line switch.
#error However, your C/C++ compilation flags indicate you intend to run this app on earlier systems.
#error This app will die there with the RPC_X_WRONG_STUB_VERSION error.
#endif


static const MIDL_PROC_FORMAT_STRING __MIDL_ProcFormatString =
    {
        0,
        {

	/* Procedure RemoteGetTypeInfoCount */


	/* Procedure GetTypeInfoCount */

			0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/*  2 */	NdrFcLong( 0x0 ),	/* 0 */
/*  6 */	NdrFcShort( 0x3 ),	/* 3 */
/*  8 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 10 */	NdrFcShort( 0x0 ),	/* 0 */
/* 12 */	NdrFcShort( 0x24 ),	/* 36 */
/* 14 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x2,		/* 2 */
/* 16 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 18 */	NdrFcShort( 0x0 ),	/* 0 */
/* 20 */	NdrFcShort( 0x0 ),	/* 0 */
/* 22 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pcTInfo */


	/* Parameter pctinfo */

/* 24 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 26 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 28 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */


	/* Return value */

/* 30 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 32 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 34 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetTypeInfo */

/* 36 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 38 */	NdrFcLong( 0x0 ),	/* 0 */
/* 42 */	NdrFcShort( 0x4 ),	/* 4 */
/* 44 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 46 */	NdrFcShort( 0x10 ),	/* 16 */
/* 48 */	NdrFcShort( 0x8 ),	/* 8 */
/* 50 */	0x45,		/* Oi2 Flags:  srv must size, has return, has ext, */
			0x4,		/* 4 */
/* 52 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 54 */	NdrFcShort( 0x0 ),	/* 0 */
/* 56 */	NdrFcShort( 0x0 ),	/* 0 */
/* 58 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter iTInfo */

/* 60 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 62 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 64 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter lcid */

/* 66 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 68 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 70 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter ppTInfo */

/* 72 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
/* 74 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 76 */	NdrFcShort( 0x6 ),	/* Type Offset=6 */

	/* Return value */

/* 78 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 80 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 82 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetIDsOfNames */

/* 84 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 86 */	NdrFcLong( 0x0 ),	/* 0 */
/* 90 */	NdrFcShort( 0x5 ),	/* 5 */
/* 92 */	NdrFcShort( 0x1c ),	/* x86 Stack size/offset = 28 */
/* 94 */	NdrFcShort( 0x54 ),	/* 84 */
/* 96 */	NdrFcShort( 0x8 ),	/* 8 */
/* 98 */	0x47,		/* Oi2 Flags:  srv must size, clt must size, has return, has ext, */
			0x6,		/* 6 */
/* 100 */	0x8,		/* 8 */
			0x7,		/* Ext Flags:  new corr desc, clt corr check, srv corr check, */
/* 102 */	NdrFcShort( 0x1 ),	/* 1 */
/* 104 */	NdrFcShort( 0x1 ),	/* 1 */
/* 106 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter riid */

/* 108 */	NdrFcShort( 0x10a ),	/* Flags:  must free, in, simple ref, */
/* 110 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 112 */	NdrFcShort( 0x26 ),	/* Type Offset=38 */

	/* Parameter rgszNames */

/* 114 */	NdrFcShort( 0x10b ),	/* Flags:  must size, must free, in, simple ref, */
/* 116 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 118 */	NdrFcShort( 0x36 ),	/* Type Offset=54 */

	/* Parameter cNames */

/* 120 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 122 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 124 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter lcid */

/* 126 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 128 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 130 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter rgDispId */

/* 132 */	NdrFcShort( 0x113 ),	/* Flags:  must size, must free, out, simple ref, */
/* 134 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 136 */	NdrFcShort( 0x5a ),	/* Type Offset=90 */

	/* Return value */

/* 138 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 140 */	NdrFcShort( 0x18 ),	/* x86 Stack size/offset = 24 */
/* 142 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure RemoteInvoke */

/* 144 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 146 */	NdrFcLong( 0x0 ),	/* 0 */
/* 150 */	NdrFcShort( 0x6 ),	/* 6 */
/* 152 */	NdrFcShort( 0x34 ),	/* x86 Stack size/offset = 52 */
/* 154 */	NdrFcShort( 0x64 ),	/* 100 */
/* 156 */	NdrFcShort( 0x24 ),	/* 36 */
/* 158 */	0x47,		/* Oi2 Flags:  srv must size, clt must size, has return, has ext, */
			0xc,		/* 12 */
/* 160 */	0x8,		/* 8 */
			0x7,		/* Ext Flags:  new corr desc, clt corr check, srv corr check, */
/* 162 */	NdrFcShort( 0x44 ),	/* 68 */
/* 164 */	NdrFcShort( 0x44 ),	/* 68 */
/* 166 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter dispIdMember */

/* 168 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 170 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 172 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter riid */

/* 174 */	NdrFcShort( 0x10a ),	/* Flags:  must free, in, simple ref, */
/* 176 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 178 */	NdrFcShort( 0x26 ),	/* Type Offset=38 */

	/* Parameter lcid */

/* 180 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 182 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 184 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter dwFlags */

/* 186 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 188 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 190 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter pDispParams */

/* 192 */	NdrFcShort( 0x10b ),	/* Flags:  must size, must free, in, simple ref, */
/* 194 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 196 */	NdrFcShort( 0x466 ),	/* Type Offset=1126 */

	/* Parameter pVarResult */

/* 198 */	NdrFcShort( 0x4113 ),	/* Flags:  must size, must free, out, simple ref, srv alloc size=16 */
/* 200 */	NdrFcShort( 0x18 ),	/* x86 Stack size/offset = 24 */
/* 202 */	NdrFcShort( 0x484 ),	/* Type Offset=1156 */

	/* Parameter pExcepInfo */

/* 204 */	NdrFcShort( 0x8113 ),	/* Flags:  must size, must free, out, simple ref, srv alloc size=32 */
/* 206 */	NdrFcShort( 0x1c ),	/* x86 Stack size/offset = 28 */
/* 208 */	NdrFcShort( 0x4a0 ),	/* Type Offset=1184 */

	/* Parameter pArgErr */

/* 210 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 212 */	NdrFcShort( 0x20 ),	/* x86 Stack size/offset = 32 */
/* 214 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter cVarRef */

/* 216 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 218 */	NdrFcShort( 0x24 ),	/* x86 Stack size/offset = 36 */
/* 220 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter rgVarRefIdx */

/* 222 */	NdrFcShort( 0x10b ),	/* Flags:  must size, must free, in, simple ref, */
/* 224 */	NdrFcShort( 0x28 ),	/* x86 Stack size/offset = 40 */
/* 226 */	NdrFcShort( 0x4c0 ),	/* Type Offset=1216 */

	/* Parameter rgVarRef */

/* 228 */	NdrFcShort( 0x11b ),	/* Flags:  must size, must free, in, out, simple ref, */
/* 230 */	NdrFcShort( 0x2c ),	/* x86 Stack size/offset = 44 */
/* 232 */	NdrFcShort( 0x4d0 ),	/* Type Offset=1232 */

	/* Return value */

/* 234 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 236 */	NdrFcShort( 0x30 ),	/* x86 Stack size/offset = 48 */
/* 238 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure RemoteNext */

/* 240 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 242 */	NdrFcLong( 0x0 ),	/* 0 */
/* 246 */	NdrFcShort( 0x3 ),	/* 3 */
/* 248 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 250 */	NdrFcShort( 0x8 ),	/* 8 */
/* 252 */	NdrFcShort( 0x24 ),	/* 36 */
/* 254 */	0x45,		/* Oi2 Flags:  srv must size, has return, has ext, */
			0x4,		/* 4 */
/* 256 */	0x8,		/* 8 */
			0x3,		/* Ext Flags:  new corr desc, clt corr check, */
/* 258 */	NdrFcShort( 0x21 ),	/* 33 */
/* 260 */	NdrFcShort( 0x0 ),	/* 0 */
/* 262 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter celt */

/* 264 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 266 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 268 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter rgVar */

/* 270 */	NdrFcShort( 0x113 ),	/* Flags:  must size, must free, out, simple ref, */
/* 272 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 274 */	NdrFcShort( 0x4ea ),	/* Type Offset=1258 */

	/* Parameter pCeltFetched */

/* 276 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 278 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 280 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 282 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 284 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 286 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure Skip */

/* 288 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 290 */	NdrFcLong( 0x0 ),	/* 0 */
/* 294 */	NdrFcShort( 0x4 ),	/* 4 */
/* 296 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 298 */	NdrFcShort( 0x8 ),	/* 8 */
/* 300 */	NdrFcShort( 0x8 ),	/* 8 */
/* 302 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x2,		/* 2 */
/* 304 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 306 */	NdrFcShort( 0x0 ),	/* 0 */
/* 308 */	NdrFcShort( 0x0 ),	/* 0 */
/* 310 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter celt */

/* 312 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 314 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 316 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 318 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 320 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 322 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure Reset */

/* 324 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 326 */	NdrFcLong( 0x0 ),	/* 0 */
/* 330 */	NdrFcShort( 0x5 ),	/* 5 */
/* 332 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 334 */	NdrFcShort( 0x0 ),	/* 0 */
/* 336 */	NdrFcShort( 0x8 ),	/* 8 */
/* 338 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x1,		/* 1 */
/* 340 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 342 */	NdrFcShort( 0x0 ),	/* 0 */
/* 344 */	NdrFcShort( 0x0 ),	/* 0 */
/* 346 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Return value */

/* 348 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 350 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 352 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure Clone */

/* 354 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 356 */	NdrFcLong( 0x0 ),	/* 0 */
/* 360 */	NdrFcShort( 0x6 ),	/* 6 */
/* 362 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 364 */	NdrFcShort( 0x0 ),	/* 0 */
/* 366 */	NdrFcShort( 0x8 ),	/* 8 */
/* 368 */	0x45,		/* Oi2 Flags:  srv must size, has return, has ext, */
			0x2,		/* 2 */
/* 370 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 372 */	NdrFcShort( 0x0 ),	/* 0 */
/* 374 */	NdrFcShort( 0x0 ),	/* 0 */
/* 376 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter ppEnum */

/* 378 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
/* 380 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 382 */	NdrFcShort( 0x500 ),	/* Type Offset=1280 */

	/* Return value */

/* 384 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 386 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 388 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure RemoteBind */

/* 390 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 392 */	NdrFcLong( 0x0 ),	/* 0 */
/* 396 */	NdrFcShort( 0x3 ),	/* 3 */
/* 398 */	NdrFcShort( 0x2c ),	/* x86 Stack size/offset = 44 */
/* 400 */	NdrFcShort( 0xe ),	/* 14 */
/* 402 */	NdrFcShort( 0x40 ),	/* 64 */
/* 404 */	0x47,		/* Oi2 Flags:  srv must size, clt must size, has return, has ext, */
			0xa,		/* 10 */
/* 406 */	0x8,		/* 8 */
			0x3,		/* Ext Flags:  new corr desc, clt corr check, */
/* 408 */	NdrFcShort( 0x89 ),	/* 137 */
/* 410 */	NdrFcShort( 0x0 ),	/* 0 */
/* 412 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter szName */

/* 414 */	NdrFcShort( 0x10b ),	/* Flags:  must size, must free, in, simple ref, */
/* 416 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 418 */	NdrFcShort( 0x518 ),	/* Type Offset=1304 */

	/* Parameter lHashVal */

/* 420 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 422 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 424 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter wFlags */

/* 426 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 428 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 430 */	0x6,		/* FC_SHORT */
			0x0,		/* 0 */

	/* Parameter ppTInfo */

/* 432 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
/* 434 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 436 */	NdrFcShort( 0x6 ),	/* Type Offset=6 */

	/* Parameter pDescKind */

/* 438 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 440 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 442 */	0xe,		/* FC_ENUM32 */
			0x0,		/* 0 */

	/* Parameter ppFuncDesc */

/* 444 */	NdrFcShort( 0x2013 ),	/* Flags:  must size, must free, out, srv alloc size=8 */
/* 446 */	NdrFcShort( 0x18 ),	/* x86 Stack size/offset = 24 */
/* 448 */	NdrFcShort( 0x51e ),	/* Type Offset=1310 */

	/* Parameter ppVarDesc */

/* 450 */	NdrFcShort( 0x2013 ),	/* Flags:  must size, must free, out, srv alloc size=8 */
/* 452 */	NdrFcShort( 0x1c ),	/* x86 Stack size/offset = 28 */
/* 454 */	NdrFcShort( 0x5fc ),	/* Type Offset=1532 */

	/* Parameter ppTypeComp */

/* 456 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
/* 458 */	NdrFcShort( 0x20 ),	/* x86 Stack size/offset = 32 */
/* 460 */	NdrFcShort( 0x64a ),	/* Type Offset=1610 */

	/* Parameter pDummy */

/* 462 */	NdrFcShort( 0x4112 ),	/* Flags:  must free, out, simple ref, srv alloc size=16 */
/* 464 */	NdrFcShort( 0x24 ),	/* x86 Stack size/offset = 36 */
/* 466 */	NdrFcShort( 0x666 ),	/* Type Offset=1638 */

	/* Return value */

/* 468 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 470 */	NdrFcShort( 0x28 ),	/* x86 Stack size/offset = 40 */
/* 472 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure RemoteBindType */

/* 474 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 476 */	NdrFcLong( 0x0 ),	/* 0 */
/* 480 */	NdrFcShort( 0x4 ),	/* 4 */
/* 482 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 484 */	NdrFcShort( 0x8 ),	/* 8 */
/* 486 */	NdrFcShort( 0x8 ),	/* 8 */
/* 488 */	0x47,		/* Oi2 Flags:  srv must size, clt must size, has return, has ext, */
			0x4,		/* 4 */
/* 490 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 492 */	NdrFcShort( 0x0 ),	/* 0 */
/* 494 */	NdrFcShort( 0x0 ),	/* 0 */
/* 496 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter szName */

/* 498 */	NdrFcShort( 0x10b ),	/* Flags:  must size, must free, in, simple ref, */
/* 500 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 502 */	NdrFcShort( 0x518 ),	/* Type Offset=1304 */

	/* Parameter lHashVal */

/* 504 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 506 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 508 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter ppTInfo */

/* 510 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
/* 512 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 514 */	NdrFcShort( 0x6 ),	/* Type Offset=6 */

	/* Return value */

/* 516 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 518 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 520 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure RemoteGetTypeAttr */

/* 522 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 524 */	NdrFcLong( 0x0 ),	/* 0 */
/* 528 */	NdrFcShort( 0x3 ),	/* 3 */
/* 530 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 532 */	NdrFcShort( 0x0 ),	/* 0 */
/* 534 */	NdrFcShort( 0x24 ),	/* 36 */
/* 536 */	0x45,		/* Oi2 Flags:  srv must size, has return, has ext, */
			0x3,		/* 3 */
/* 538 */	0x8,		/* 8 */
			0x3,		/* Ext Flags:  new corr desc, clt corr check, */
/* 540 */	NdrFcShort( 0x2 ),	/* 2 */
/* 542 */	NdrFcShort( 0x0 ),	/* 0 */
/* 544 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter ppTypeAttr */

/* 546 */	NdrFcShort( 0x2013 ),	/* Flags:  must size, must free, out, srv alloc size=8 */
/* 548 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 550 */	NdrFcShort( 0x670 ),	/* Type Offset=1648 */

	/* Parameter pDummy */

/* 552 */	NdrFcShort( 0x4112 ),	/* Flags:  must free, out, simple ref, srv alloc size=16 */
/* 554 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 556 */	NdrFcShort( 0x6b2 ),	/* Type Offset=1714 */

	/* Return value */

/* 558 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 560 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 562 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetTypeComp */

/* 564 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 566 */	NdrFcLong( 0x0 ),	/* 0 */
/* 570 */	NdrFcShort( 0x4 ),	/* 4 */
/* 572 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 574 */	NdrFcShort( 0x0 ),	/* 0 */
/* 576 */	NdrFcShort( 0x8 ),	/* 8 */
/* 578 */	0x45,		/* Oi2 Flags:  srv must size, has return, has ext, */
			0x2,		/* 2 */
/* 580 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 582 */	NdrFcShort( 0x0 ),	/* 0 */
/* 584 */	NdrFcShort( 0x0 ),	/* 0 */
/* 586 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter ppTComp */

/* 588 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
/* 590 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 592 */	NdrFcShort( 0x64a ),	/* Type Offset=1610 */

	/* Return value */

/* 594 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 596 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 598 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure RemoteGetFuncDesc */

/* 600 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 602 */	NdrFcLong( 0x0 ),	/* 0 */
/* 606 */	NdrFcShort( 0x5 ),	/* 5 */
/* 608 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 610 */	NdrFcShort( 0x8 ),	/* 8 */
/* 612 */	NdrFcShort( 0x24 ),	/* 36 */
/* 614 */	0x45,		/* Oi2 Flags:  srv must size, has return, has ext, */
			0x4,		/* 4 */
/* 616 */	0x8,		/* 8 */
			0x3,		/* Ext Flags:  new corr desc, clt corr check, */
/* 618 */	NdrFcShort( 0x46 ),	/* 70 */
/* 620 */	NdrFcShort( 0x0 ),	/* 0 */
/* 622 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter index */

/* 624 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 626 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 628 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter ppFuncDesc */

/* 630 */	NdrFcShort( 0x2013 ),	/* Flags:  must size, must free, out, srv alloc size=8 */
/* 632 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 634 */	NdrFcShort( 0x51e ),	/* Type Offset=1310 */

	/* Parameter pDummy */

/* 636 */	NdrFcShort( 0x4112 ),	/* Flags:  must free, out, simple ref, srv alloc size=16 */
/* 638 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 640 */	NdrFcShort( 0x6c2 ),	/* Type Offset=1730 */

	/* Return value */

/* 642 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 644 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 646 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure RemoteGetVarDesc */

/* 648 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 650 */	NdrFcLong( 0x0 ),	/* 0 */
/* 654 */	NdrFcShort( 0x6 ),	/* 6 */
/* 656 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 658 */	NdrFcShort( 0x8 ),	/* 8 */
/* 660 */	NdrFcShort( 0x24 ),	/* 36 */
/* 662 */	0x45,		/* Oi2 Flags:  srv must size, has return, has ext, */
			0x4,		/* 4 */
/* 664 */	0x8,		/* 8 */
			0x3,		/* Ext Flags:  new corr desc, clt corr check, */
/* 666 */	NdrFcShort( 0x43 ),	/* 67 */
/* 668 */	NdrFcShort( 0x0 ),	/* 0 */
/* 670 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter index */

/* 672 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 674 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 676 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter ppVarDesc */

/* 678 */	NdrFcShort( 0x2013 ),	/* Flags:  must size, must free, out, srv alloc size=8 */
/* 680 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 682 */	NdrFcShort( 0x5fc ),	/* Type Offset=1532 */

	/* Parameter pDummy */

/* 684 */	NdrFcShort( 0x4112 ),	/* Flags:  must free, out, simple ref, srv alloc size=16 */
/* 686 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 688 */	NdrFcShort( 0x6d2 ),	/* Type Offset=1746 */

	/* Return value */

/* 690 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 692 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 694 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure RemoteGetNames */

/* 696 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 698 */	NdrFcLong( 0x0 ),	/* 0 */
/* 702 */	NdrFcShort( 0x7 ),	/* 7 */
/* 704 */	NdrFcShort( 0x18 ),	/* x86 Stack size/offset = 24 */
/* 706 */	NdrFcShort( 0x10 ),	/* 16 */
/* 708 */	NdrFcShort( 0x24 ),	/* 36 */
/* 710 */	0x45,		/* Oi2 Flags:  srv must size, has return, has ext, */
			0x5,		/* 5 */
/* 712 */	0x8,		/* 8 */
			0x3,		/* Ext Flags:  new corr desc, clt corr check, */
/* 714 */	NdrFcShort( 0x2 ),	/* 2 */
/* 716 */	NdrFcShort( 0x0 ),	/* 0 */
/* 718 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter memid */

/* 720 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 722 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 724 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter rgBstrNames */

/* 726 */	NdrFcShort( 0x113 ),	/* Flags:  must size, must free, out, simple ref, */
/* 728 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 730 */	NdrFcShort( 0x6e0 ),	/* Type Offset=1760 */

	/* Parameter cMaxNames */

/* 732 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 734 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 736 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter pcNames */

/* 738 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 740 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 742 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 744 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 746 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 748 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetRefTypeOfImplType */

/* 750 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 752 */	NdrFcLong( 0x0 ),	/* 0 */
/* 756 */	NdrFcShort( 0x8 ),	/* 8 */
/* 758 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 760 */	NdrFcShort( 0x8 ),	/* 8 */
/* 762 */	NdrFcShort( 0x24 ),	/* 36 */
/* 764 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x3,		/* 3 */
/* 766 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 768 */	NdrFcShort( 0x0 ),	/* 0 */
/* 770 */	NdrFcShort( 0x0 ),	/* 0 */
/* 772 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter index */

/* 774 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 776 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 778 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter pRefType */

/* 780 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 782 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 784 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 786 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 788 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 790 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetImplTypeFlags */

/* 792 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 794 */	NdrFcLong( 0x0 ),	/* 0 */
/* 798 */	NdrFcShort( 0x9 ),	/* 9 */
/* 800 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 802 */	NdrFcShort( 0x8 ),	/* 8 */
/* 804 */	NdrFcShort( 0x24 ),	/* 36 */
/* 806 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x3,		/* 3 */
/* 808 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 810 */	NdrFcShort( 0x0 ),	/* 0 */
/* 812 */	NdrFcShort( 0x0 ),	/* 0 */
/* 814 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter index */

/* 816 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 818 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 820 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter pImplTypeFlags */

/* 822 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 824 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 826 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 828 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 830 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 832 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure LocalGetIDsOfNames */

/* 834 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 836 */	NdrFcLong( 0x0 ),	/* 0 */
/* 840 */	NdrFcShort( 0xa ),	/* 10 */
/* 842 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 844 */	NdrFcShort( 0x0 ),	/* 0 */
/* 846 */	NdrFcShort( 0x8 ),	/* 8 */
/* 848 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x1,		/* 1 */
/* 850 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 852 */	NdrFcShort( 0x0 ),	/* 0 */
/* 854 */	NdrFcShort( 0x0 ),	/* 0 */
/* 856 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Return value */

/* 858 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 860 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 862 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure LocalInvoke */

/* 864 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 866 */	NdrFcLong( 0x0 ),	/* 0 */
/* 870 */	NdrFcShort( 0xb ),	/* 11 */
/* 872 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 874 */	NdrFcShort( 0x0 ),	/* 0 */
/* 876 */	NdrFcShort( 0x8 ),	/* 8 */
/* 878 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x1,		/* 1 */
/* 880 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 882 */	NdrFcShort( 0x0 ),	/* 0 */
/* 884 */	NdrFcShort( 0x0 ),	/* 0 */
/* 886 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Return value */

/* 888 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 890 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 892 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure RemoteGetDocumentation */

/* 894 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 896 */	NdrFcLong( 0x0 ),	/* 0 */
/* 900 */	NdrFcShort( 0xc ),	/* 12 */
/* 902 */	NdrFcShort( 0x20 ),	/* x86 Stack size/offset = 32 */
/* 904 */	NdrFcShort( 0x10 ),	/* 16 */
/* 906 */	NdrFcShort( 0x24 ),	/* 36 */
/* 908 */	0x45,		/* Oi2 Flags:  srv must size, has return, has ext, */
			0x7,		/* 7 */
/* 910 */	0x8,		/* 8 */
			0x3,		/* Ext Flags:  new corr desc, clt corr check, */
/* 912 */	NdrFcShort( 0x3 ),	/* 3 */
/* 914 */	NdrFcShort( 0x0 ),	/* 0 */
/* 916 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter memid */

/* 918 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 920 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 922 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter refPtrFlags */

/* 924 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 926 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 928 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter pBstrName */

/* 930 */	NdrFcShort( 0x2113 ),	/* Flags:  must size, must free, out, simple ref, srv alloc size=8 */
/* 932 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 934 */	NdrFcShort( 0x496 ),	/* Type Offset=1174 */

	/* Parameter pBstrDocString */

/* 936 */	NdrFcShort( 0x2113 ),	/* Flags:  must size, must free, out, simple ref, srv alloc size=8 */
/* 938 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 940 */	NdrFcShort( 0x496 ),	/* Type Offset=1174 */

	/* Parameter pdwHelpContext */

/* 942 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 944 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 946 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter pBstrHelpFile */

/* 948 */	NdrFcShort( 0x2113 ),	/* Flags:  must size, must free, out, simple ref, srv alloc size=8 */
/* 950 */	NdrFcShort( 0x18 ),	/* x86 Stack size/offset = 24 */
/* 952 */	NdrFcShort( 0x496 ),	/* Type Offset=1174 */

	/* Return value */

/* 954 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 956 */	NdrFcShort( 0x1c ),	/* x86 Stack size/offset = 28 */
/* 958 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure RemoteGetDllEntry */

/* 960 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 962 */	NdrFcLong( 0x0 ),	/* 0 */
/* 966 */	NdrFcShort( 0xd ),	/* 13 */
/* 968 */	NdrFcShort( 0x20 ),	/* x86 Stack size/offset = 32 */
/* 970 */	NdrFcShort( 0x18 ),	/* 24 */
/* 972 */	NdrFcShort( 0x22 ),	/* 34 */
/* 974 */	0x45,		/* Oi2 Flags:  srv must size, has return, has ext, */
			0x7,		/* 7 */
/* 976 */	0x8,		/* 8 */
			0x3,		/* Ext Flags:  new corr desc, clt corr check, */
/* 978 */	NdrFcShort( 0x2 ),	/* 2 */
/* 980 */	NdrFcShort( 0x0 ),	/* 0 */
/* 982 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter memid */

/* 984 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 986 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 988 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter invKind */

/* 990 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 992 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 994 */	0xe,		/* FC_ENUM32 */
			0x0,		/* 0 */

	/* Parameter refPtrFlags */

/* 996 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 998 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 1000 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter pBstrDllName */

/* 1002 */	NdrFcShort( 0x2113 ),	/* Flags:  must size, must free, out, simple ref, srv alloc size=8 */
/* 1004 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 1006 */	NdrFcShort( 0x496 ),	/* Type Offset=1174 */

	/* Parameter pBstrName */

/* 1008 */	NdrFcShort( 0x2113 ),	/* Flags:  must size, must free, out, simple ref, srv alloc size=8 */
/* 1010 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 1012 */	NdrFcShort( 0x496 ),	/* Type Offset=1174 */

	/* Parameter pwOrdinal */

/* 1014 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 1016 */	NdrFcShort( 0x18 ),	/* x86 Stack size/offset = 24 */
/* 1018 */	0x6,		/* FC_SHORT */
			0x0,		/* 0 */

	/* Return value */

/* 1020 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1022 */	NdrFcShort( 0x1c ),	/* x86 Stack size/offset = 28 */
/* 1024 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetRefTypeInfo */

/* 1026 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 1028 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1032 */	NdrFcShort( 0xe ),	/* 14 */
/* 1034 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 1036 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1038 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1040 */	0x45,		/* Oi2 Flags:  srv must size, has return, has ext, */
			0x3,		/* 3 */
/* 1042 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 1044 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1046 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1048 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter hRefType */

/* 1050 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 1052 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 1054 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter ppTInfo */

/* 1056 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
/* 1058 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 1060 */	NdrFcShort( 0x6 ),	/* Type Offset=6 */

	/* Return value */

/* 1062 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1064 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 1066 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure LocalAddressOfMember */

/* 1068 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 1070 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1074 */	NdrFcShort( 0xf ),	/* 15 */
/* 1076 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 1078 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1080 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1082 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x1,		/* 1 */
/* 1084 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 1086 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1088 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1090 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Return value */

/* 1092 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1094 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 1096 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure RemoteCreateInstance */

/* 1098 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 1100 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1104 */	NdrFcShort( 0x10 ),	/* 16 */
/* 1106 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 1108 */	NdrFcShort( 0x44 ),	/* 68 */
/* 1110 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1112 */	0x45,		/* Oi2 Flags:  srv must size, has return, has ext, */
			0x3,		/* 3 */
/* 1114 */	0x8,		/* 8 */
			0x3,		/* Ext Flags:  new corr desc, clt corr check, */
/* 1116 */	NdrFcShort( 0x1 ),	/* 1 */
/* 1118 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1120 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter riid */

/* 1122 */	NdrFcShort( 0x10a ),	/* Flags:  must free, in, simple ref, */
/* 1124 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 1126 */	NdrFcShort( 0x26 ),	/* Type Offset=38 */

	/* Parameter ppvObj */

/* 1128 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
/* 1130 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 1132 */	NdrFcShort( 0x6fe ),	/* Type Offset=1790 */

	/* Return value */

/* 1134 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1136 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 1138 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetMops */

/* 1140 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 1142 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1146 */	NdrFcShort( 0x11 ),	/* 17 */
/* 1148 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 1150 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1152 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1154 */	0x45,		/* Oi2 Flags:  srv must size, has return, has ext, */
			0x3,		/* 3 */
/* 1156 */	0x8,		/* 8 */
			0x3,		/* Ext Flags:  new corr desc, clt corr check, */
/* 1158 */	NdrFcShort( 0x1 ),	/* 1 */
/* 1160 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1162 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter memid */

/* 1164 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 1166 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 1168 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter pBstrMops */

/* 1170 */	NdrFcShort( 0x2113 ),	/* Flags:  must size, must free, out, simple ref, srv alloc size=8 */
/* 1172 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 1174 */	NdrFcShort( 0x496 ),	/* Type Offset=1174 */

	/* Return value */

/* 1176 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1178 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 1180 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure RemoteGetContainingTypeLib */

/* 1182 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 1184 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1188 */	NdrFcShort( 0x12 ),	/* 18 */
/* 1190 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 1192 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1194 */	NdrFcShort( 0x24 ),	/* 36 */
/* 1196 */	0x45,		/* Oi2 Flags:  srv must size, has return, has ext, */
			0x3,		/* 3 */
/* 1198 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 1200 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1202 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1204 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter ppTLib */

/* 1206 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
/* 1208 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 1210 */	NdrFcShort( 0x70a ),	/* Type Offset=1802 */

	/* Parameter pIndex */

/* 1212 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 1214 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 1216 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 1218 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1220 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 1222 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure LocalReleaseTypeAttr */

/* 1224 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 1226 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1230 */	NdrFcShort( 0x13 ),	/* 19 */
/* 1232 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 1234 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1236 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1238 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x1,		/* 1 */
/* 1240 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 1242 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1244 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1246 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Return value */

/* 1248 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1250 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 1252 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure LocalReleaseFuncDesc */

/* 1254 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 1256 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1260 */	NdrFcShort( 0x14 ),	/* 20 */
/* 1262 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 1264 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1266 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1268 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x1,		/* 1 */
/* 1270 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 1272 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1274 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1276 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Return value */

/* 1278 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1280 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 1282 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure LocalReleaseVarDesc */

/* 1284 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 1286 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1290 */	NdrFcShort( 0x15 ),	/* 21 */
/* 1292 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 1294 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1296 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1298 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x1,		/* 1 */
/* 1300 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 1302 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1304 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1306 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Return value */

/* 1308 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1310 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 1312 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetTypeKind */

/* 1314 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 1316 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1320 */	NdrFcShort( 0x16 ),	/* 22 */
/* 1322 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 1324 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1326 */	NdrFcShort( 0x24 ),	/* 36 */
/* 1328 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x2,		/* 2 */
/* 1330 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 1332 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1334 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1336 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pTypeKind */

/* 1338 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 1340 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 1342 */	0xe,		/* FC_ENUM32 */
			0x0,		/* 0 */

	/* Return value */

/* 1344 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1346 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 1348 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetTypeFlags */

/* 1350 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 1352 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1356 */	NdrFcShort( 0x17 ),	/* 23 */
/* 1358 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 1360 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1362 */	NdrFcShort( 0x24 ),	/* 36 */
/* 1364 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x2,		/* 2 */
/* 1366 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 1368 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1370 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1372 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pTypeFlags */

/* 1374 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 1376 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 1378 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 1380 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1382 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 1384 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetFuncIndexOfMemId */

/* 1386 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 1388 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1392 */	NdrFcShort( 0x18 ),	/* 24 */
/* 1394 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 1396 */	NdrFcShort( 0x10 ),	/* 16 */
/* 1398 */	NdrFcShort( 0x24 ),	/* 36 */
/* 1400 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x4,		/* 4 */
/* 1402 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 1404 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1406 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1408 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter memid */

/* 1410 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 1412 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 1414 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter invKind */

/* 1416 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 1418 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 1420 */	0xe,		/* FC_ENUM32 */
			0x0,		/* 0 */

	/* Parameter pFuncIndex */

/* 1422 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 1424 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 1426 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 1428 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1430 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 1432 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetVarIndexOfMemId */

/* 1434 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 1436 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1440 */	NdrFcShort( 0x19 ),	/* 25 */
/* 1442 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 1444 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1446 */	NdrFcShort( 0x24 ),	/* 36 */
/* 1448 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x3,		/* 3 */
/* 1450 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 1452 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1454 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1456 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter memid */

/* 1458 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 1460 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 1462 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter pVarIndex */

/* 1464 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 1466 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 1468 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 1470 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1472 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 1474 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetCustData */

/* 1476 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 1478 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1482 */	NdrFcShort( 0x1a ),	/* 26 */
/* 1484 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 1486 */	NdrFcShort( 0x44 ),	/* 68 */
/* 1488 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1490 */	0x45,		/* Oi2 Flags:  srv must size, has return, has ext, */
			0x3,		/* 3 */
/* 1492 */	0x8,		/* 8 */
			0x3,		/* Ext Flags:  new corr desc, clt corr check, */
/* 1494 */	NdrFcShort( 0x20 ),	/* 32 */
/* 1496 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1498 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter guid */

/* 1500 */	NdrFcShort( 0x10a ),	/* Flags:  must free, in, simple ref, */
/* 1502 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 1504 */	NdrFcShort( 0x26 ),	/* Type Offset=38 */

	/* Parameter pVarVal */

/* 1506 */	NdrFcShort( 0x4113 ),	/* Flags:  must size, must free, out, simple ref, srv alloc size=16 */
/* 1508 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 1510 */	NdrFcShort( 0x484 ),	/* Type Offset=1156 */

	/* Return value */

/* 1512 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1514 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 1516 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetFuncCustData */

/* 1518 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 1520 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1524 */	NdrFcShort( 0x1b ),	/* 27 */
/* 1526 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 1528 */	NdrFcShort( 0x4c ),	/* 76 */
/* 1530 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1532 */	0x45,		/* Oi2 Flags:  srv must size, has return, has ext, */
			0x4,		/* 4 */
/* 1534 */	0x8,		/* 8 */
			0x3,		/* Ext Flags:  new corr desc, clt corr check, */
/* 1536 */	NdrFcShort( 0x20 ),	/* 32 */
/* 1538 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1540 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter index */

/* 1542 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 1544 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 1546 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter guid */

/* 1548 */	NdrFcShort( 0x10a ),	/* Flags:  must free, in, simple ref, */
/* 1550 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 1552 */	NdrFcShort( 0x26 ),	/* Type Offset=38 */

	/* Parameter pVarVal */

/* 1554 */	NdrFcShort( 0x4113 ),	/* Flags:  must size, must free, out, simple ref, srv alloc size=16 */
/* 1556 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 1558 */	NdrFcShort( 0x484 ),	/* Type Offset=1156 */

	/* Return value */

/* 1560 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1562 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 1564 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetParamCustData */

/* 1566 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 1568 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1572 */	NdrFcShort( 0x1c ),	/* 28 */
/* 1574 */	NdrFcShort( 0x18 ),	/* x86 Stack size/offset = 24 */
/* 1576 */	NdrFcShort( 0x54 ),	/* 84 */
/* 1578 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1580 */	0x45,		/* Oi2 Flags:  srv must size, has return, has ext, */
			0x5,		/* 5 */
/* 1582 */	0x8,		/* 8 */
			0x3,		/* Ext Flags:  new corr desc, clt corr check, */
/* 1584 */	NdrFcShort( 0x20 ),	/* 32 */
/* 1586 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1588 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter indexFunc */

/* 1590 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 1592 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 1594 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter indexParam */

/* 1596 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 1598 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 1600 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter guid */

/* 1602 */	NdrFcShort( 0x10a ),	/* Flags:  must free, in, simple ref, */
/* 1604 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 1606 */	NdrFcShort( 0x26 ),	/* Type Offset=38 */

	/* Parameter pVarVal */

/* 1608 */	NdrFcShort( 0x4113 ),	/* Flags:  must size, must free, out, simple ref, srv alloc size=16 */
/* 1610 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 1612 */	NdrFcShort( 0x484 ),	/* Type Offset=1156 */

	/* Return value */

/* 1614 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1616 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 1618 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetVarCustData */

/* 1620 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 1622 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1626 */	NdrFcShort( 0x1d ),	/* 29 */
/* 1628 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 1630 */	NdrFcShort( 0x4c ),	/* 76 */
/* 1632 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1634 */	0x45,		/* Oi2 Flags:  srv must size, has return, has ext, */
			0x4,		/* 4 */
/* 1636 */	0x8,		/* 8 */
			0x3,		/* Ext Flags:  new corr desc, clt corr check, */
/* 1638 */	NdrFcShort( 0x20 ),	/* 32 */
/* 1640 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1642 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter index */

/* 1644 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 1646 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 1648 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter guid */

/* 1650 */	NdrFcShort( 0x10a ),	/* Flags:  must free, in, simple ref, */
/* 1652 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 1654 */	NdrFcShort( 0x26 ),	/* Type Offset=38 */

	/* Parameter pVarVal */

/* 1656 */	NdrFcShort( 0x4113 ),	/* Flags:  must size, must free, out, simple ref, srv alloc size=16 */
/* 1658 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 1660 */	NdrFcShort( 0x484 ),	/* Type Offset=1156 */

	/* Return value */

/* 1662 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1664 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 1666 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetImplTypeCustData */

/* 1668 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 1670 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1674 */	NdrFcShort( 0x1e ),	/* 30 */
/* 1676 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 1678 */	NdrFcShort( 0x4c ),	/* 76 */
/* 1680 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1682 */	0x45,		/* Oi2 Flags:  srv must size, has return, has ext, */
			0x4,		/* 4 */
/* 1684 */	0x8,		/* 8 */
			0x3,		/* Ext Flags:  new corr desc, clt corr check, */
/* 1686 */	NdrFcShort( 0x20 ),	/* 32 */
/* 1688 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1690 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter index */

/* 1692 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 1694 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 1696 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter guid */

/* 1698 */	NdrFcShort( 0x10a ),	/* Flags:  must free, in, simple ref, */
/* 1700 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 1702 */	NdrFcShort( 0x26 ),	/* Type Offset=38 */

	/* Parameter pVarVal */

/* 1704 */	NdrFcShort( 0x4113 ),	/* Flags:  must size, must free, out, simple ref, srv alloc size=16 */
/* 1706 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 1708 */	NdrFcShort( 0x484 ),	/* Type Offset=1156 */

	/* Return value */

/* 1710 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1712 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 1714 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure RemoteGetDocumentation2 */

/* 1716 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 1718 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1722 */	NdrFcShort( 0x1f ),	/* 31 */
/* 1724 */	NdrFcShort( 0x20 ),	/* x86 Stack size/offset = 32 */
/* 1726 */	NdrFcShort( 0x18 ),	/* 24 */
/* 1728 */	NdrFcShort( 0x24 ),	/* 36 */
/* 1730 */	0x45,		/* Oi2 Flags:  srv must size, has return, has ext, */
			0x7,		/* 7 */
/* 1732 */	0x8,		/* 8 */
			0x3,		/* Ext Flags:  new corr desc, clt corr check, */
/* 1734 */	NdrFcShort( 0x2 ),	/* 2 */
/* 1736 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1738 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter memid */

/* 1740 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 1742 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 1744 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter lcid */

/* 1746 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 1748 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 1750 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter refPtrFlags */

/* 1752 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 1754 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 1756 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter pbstrHelpString */

/* 1758 */	NdrFcShort( 0x2113 ),	/* Flags:  must size, must free, out, simple ref, srv alloc size=8 */
/* 1760 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 1762 */	NdrFcShort( 0x496 ),	/* Type Offset=1174 */

	/* Parameter pdwHelpStringContext */

/* 1764 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 1766 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 1768 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter pbstrHelpStringDll */

/* 1770 */	NdrFcShort( 0x2113 ),	/* Flags:  must size, must free, out, simple ref, srv alloc size=8 */
/* 1772 */	NdrFcShort( 0x18 ),	/* x86 Stack size/offset = 24 */
/* 1774 */	NdrFcShort( 0x496 ),	/* Type Offset=1174 */

	/* Return value */

/* 1776 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1778 */	NdrFcShort( 0x1c ),	/* x86 Stack size/offset = 28 */
/* 1780 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetAllCustData */

/* 1782 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 1784 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1788 */	NdrFcShort( 0x20 ),	/* 32 */
/* 1790 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 1792 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1794 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1796 */	0x45,		/* Oi2 Flags:  srv must size, has return, has ext, */
			0x2,		/* 2 */
/* 1798 */	0x8,		/* 8 */
			0x3,		/* Ext Flags:  new corr desc, clt corr check, */
/* 1800 */	NdrFcShort( 0x21 ),	/* 33 */
/* 1802 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1804 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pCustData */

/* 1806 */	NdrFcShort( 0x2113 ),	/* Flags:  must size, must free, out, simple ref, srv alloc size=8 */
/* 1808 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 1810 */	NdrFcShort( 0x74c ),	/* Type Offset=1868 */

	/* Return value */

/* 1812 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1814 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 1816 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetAllFuncCustData */

/* 1818 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 1820 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1824 */	NdrFcShort( 0x21 ),	/* 33 */
/* 1826 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 1828 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1830 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1832 */	0x45,		/* Oi2 Flags:  srv must size, has return, has ext, */
			0x3,		/* 3 */
/* 1834 */	0x8,		/* 8 */
			0x3,		/* Ext Flags:  new corr desc, clt corr check, */
/* 1836 */	NdrFcShort( 0x21 ),	/* 33 */
/* 1838 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1840 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter index */

/* 1842 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 1844 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 1846 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter pCustData */

/* 1848 */	NdrFcShort( 0x2113 ),	/* Flags:  must size, must free, out, simple ref, srv alloc size=8 */
/* 1850 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 1852 */	NdrFcShort( 0x74c ),	/* Type Offset=1868 */

	/* Return value */

/* 1854 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1856 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 1858 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetAllParamCustData */

/* 1860 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 1862 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1866 */	NdrFcShort( 0x22 ),	/* 34 */
/* 1868 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 1870 */	NdrFcShort( 0x10 ),	/* 16 */
/* 1872 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1874 */	0x45,		/* Oi2 Flags:  srv must size, has return, has ext, */
			0x4,		/* 4 */
/* 1876 */	0x8,		/* 8 */
			0x3,		/* Ext Flags:  new corr desc, clt corr check, */
/* 1878 */	NdrFcShort( 0x21 ),	/* 33 */
/* 1880 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1882 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter indexFunc */

/* 1884 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 1886 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 1888 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter indexParam */

/* 1890 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 1892 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 1894 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter pCustData */

/* 1896 */	NdrFcShort( 0x2113 ),	/* Flags:  must size, must free, out, simple ref, srv alloc size=8 */
/* 1898 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 1900 */	NdrFcShort( 0x74c ),	/* Type Offset=1868 */

	/* Return value */

/* 1902 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1904 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 1906 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetAllVarCustData */

/* 1908 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 1910 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1914 */	NdrFcShort( 0x23 ),	/* 35 */
/* 1916 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 1918 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1920 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1922 */	0x45,		/* Oi2 Flags:  srv must size, has return, has ext, */
			0x3,		/* 3 */
/* 1924 */	0x8,		/* 8 */
			0x3,		/* Ext Flags:  new corr desc, clt corr check, */
/* 1926 */	NdrFcShort( 0x21 ),	/* 33 */
/* 1928 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1930 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter index */

/* 1932 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 1934 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 1936 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter pCustData */

/* 1938 */	NdrFcShort( 0x2113 ),	/* Flags:  must size, must free, out, simple ref, srv alloc size=8 */
/* 1940 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 1942 */	NdrFcShort( 0x74c ),	/* Type Offset=1868 */

	/* Return value */

/* 1944 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1946 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 1948 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetAllImplTypeCustData */

/* 1950 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 1952 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1956 */	NdrFcShort( 0x24 ),	/* 36 */
/* 1958 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 1960 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1962 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1964 */	0x45,		/* Oi2 Flags:  srv must size, has return, has ext, */
			0x3,		/* 3 */
/* 1966 */	0x8,		/* 8 */
			0x3,		/* Ext Flags:  new corr desc, clt corr check, */
/* 1968 */	NdrFcShort( 0x21 ),	/* 33 */
/* 1970 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1972 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter index */

/* 1974 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 1976 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 1978 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter pCustData */

/* 1980 */	NdrFcShort( 0x2113 ),	/* Flags:  must size, must free, out, simple ref, srv alloc size=8 */
/* 1982 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 1984 */	NdrFcShort( 0x74c ),	/* Type Offset=1868 */

	/* Return value */

/* 1986 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1988 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 1990 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetTypeInfo */

/* 1992 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 1994 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1998 */	NdrFcShort( 0x4 ),	/* 4 */
/* 2000 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 2002 */	NdrFcShort( 0x8 ),	/* 8 */
/* 2004 */	NdrFcShort( 0x8 ),	/* 8 */
/* 2006 */	0x45,		/* Oi2 Flags:  srv must size, has return, has ext, */
			0x3,		/* 3 */
/* 2008 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 2010 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2012 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2014 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter index */

/* 2016 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 2018 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 2020 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter ppTInfo */

/* 2022 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
/* 2024 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 2026 */	NdrFcShort( 0x6 ),	/* Type Offset=6 */

	/* Return value */

/* 2028 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 2030 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 2032 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetTypeInfoType */

/* 2034 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 2036 */	NdrFcLong( 0x0 ),	/* 0 */
/* 2040 */	NdrFcShort( 0x5 ),	/* 5 */
/* 2042 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 2044 */	NdrFcShort( 0x8 ),	/* 8 */
/* 2046 */	NdrFcShort( 0x24 ),	/* 36 */
/* 2048 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x3,		/* 3 */
/* 2050 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 2052 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2054 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2056 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter index */

/* 2058 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 2060 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 2062 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter pTKind */

/* 2064 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 2066 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 2068 */	0xe,		/* FC_ENUM32 */
			0x0,		/* 0 */

	/* Return value */

/* 2070 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 2072 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 2074 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetTypeInfoOfGuid */

/* 2076 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 2078 */	NdrFcLong( 0x0 ),	/* 0 */
/* 2082 */	NdrFcShort( 0x6 ),	/* 6 */
/* 2084 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 2086 */	NdrFcShort( 0x44 ),	/* 68 */
/* 2088 */	NdrFcShort( 0x8 ),	/* 8 */
/* 2090 */	0x45,		/* Oi2 Flags:  srv must size, has return, has ext, */
			0x3,		/* 3 */
/* 2092 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 2094 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2096 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2098 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter guid */

/* 2100 */	NdrFcShort( 0x10a ),	/* Flags:  must free, in, simple ref, */
/* 2102 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 2104 */	NdrFcShort( 0x26 ),	/* Type Offset=38 */

	/* Parameter ppTinfo */

/* 2106 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
/* 2108 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 2110 */	NdrFcShort( 0x6 ),	/* Type Offset=6 */

	/* Return value */

/* 2112 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 2114 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 2116 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure RemoteGetLibAttr */

/* 2118 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 2120 */	NdrFcLong( 0x0 ),	/* 0 */
/* 2124 */	NdrFcShort( 0x7 ),	/* 7 */
/* 2126 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 2128 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2130 */	NdrFcShort( 0x24 ),	/* 36 */
/* 2132 */	0x45,		/* Oi2 Flags:  srv must size, has return, has ext, */
			0x3,		/* 3 */
/* 2134 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 2136 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2138 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2140 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter ppTLibAttr */

/* 2142 */	NdrFcShort( 0x2013 ),	/* Flags:  must size, must free, out, srv alloc size=8 */
/* 2144 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 2146 */	NdrFcShort( 0x75c ),	/* Type Offset=1884 */

	/* Parameter pDummy */

/* 2148 */	NdrFcShort( 0x4112 ),	/* Flags:  must free, out, simple ref, srv alloc size=16 */
/* 2150 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 2152 */	NdrFcShort( 0x77e ),	/* Type Offset=1918 */

	/* Return value */

/* 2154 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 2156 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 2158 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetTypeComp */

/* 2160 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 2162 */	NdrFcLong( 0x0 ),	/* 0 */
/* 2166 */	NdrFcShort( 0x8 ),	/* 8 */
/* 2168 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 2170 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2172 */	NdrFcShort( 0x8 ),	/* 8 */
/* 2174 */	0x45,		/* Oi2 Flags:  srv must size, has return, has ext, */
			0x2,		/* 2 */
/* 2176 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 2178 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2180 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2182 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter ppTComp */

/* 2184 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
/* 2186 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 2188 */	NdrFcShort( 0x64a ),	/* Type Offset=1610 */

	/* Return value */

/* 2190 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 2192 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 2194 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure RemoteGetDocumentation */

/* 2196 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 2198 */	NdrFcLong( 0x0 ),	/* 0 */
/* 2202 */	NdrFcShort( 0x9 ),	/* 9 */
/* 2204 */	NdrFcShort( 0x20 ),	/* x86 Stack size/offset = 32 */
/* 2206 */	NdrFcShort( 0x10 ),	/* 16 */
/* 2208 */	NdrFcShort( 0x24 ),	/* 36 */
/* 2210 */	0x45,		/* Oi2 Flags:  srv must size, has return, has ext, */
			0x7,		/* 7 */
/* 2212 */	0x8,		/* 8 */
			0x3,		/* Ext Flags:  new corr desc, clt corr check, */
/* 2214 */	NdrFcShort( 0x3 ),	/* 3 */
/* 2216 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2218 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter index */

/* 2220 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 2222 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 2224 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter refPtrFlags */

/* 2226 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 2228 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 2230 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter pBstrName */

/* 2232 */	NdrFcShort( 0x2113 ),	/* Flags:  must size, must free, out, simple ref, srv alloc size=8 */
/* 2234 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 2236 */	NdrFcShort( 0x496 ),	/* Type Offset=1174 */

	/* Parameter pBstrDocString */

/* 2238 */	NdrFcShort( 0x2113 ),	/* Flags:  must size, must free, out, simple ref, srv alloc size=8 */
/* 2240 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 2242 */	NdrFcShort( 0x496 ),	/* Type Offset=1174 */

	/* Parameter pdwHelpContext */

/* 2244 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 2246 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 2248 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter pBstrHelpFile */

/* 2250 */	NdrFcShort( 0x2113 ),	/* Flags:  must size, must free, out, simple ref, srv alloc size=8 */
/* 2252 */	NdrFcShort( 0x18 ),	/* x86 Stack size/offset = 24 */
/* 2254 */	NdrFcShort( 0x496 ),	/* Type Offset=1174 */

	/* Return value */

/* 2256 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 2258 */	NdrFcShort( 0x1c ),	/* x86 Stack size/offset = 28 */
/* 2260 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure RemoteIsName */

/* 2262 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 2264 */	NdrFcLong( 0x0 ),	/* 0 */
/* 2268 */	NdrFcShort( 0xa ),	/* 10 */
/* 2270 */	NdrFcShort( 0x18 ),	/* x86 Stack size/offset = 24 */
/* 2272 */	NdrFcShort( 0x8 ),	/* 8 */
/* 2274 */	NdrFcShort( 0x24 ),	/* 36 */
/* 2276 */	0x47,		/* Oi2 Flags:  srv must size, clt must size, has return, has ext, */
			0x5,		/* 5 */
/* 2278 */	0x8,		/* 8 */
			0x3,		/* Ext Flags:  new corr desc, clt corr check, */
/* 2280 */	NdrFcShort( 0x1 ),	/* 1 */
/* 2282 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2284 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter szNameBuf */

/* 2286 */	NdrFcShort( 0x10b ),	/* Flags:  must size, must free, in, simple ref, */
/* 2288 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 2290 */	NdrFcShort( 0x518 ),	/* Type Offset=1304 */

	/* Parameter lHashVal */

/* 2292 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 2294 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 2296 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter pfName */

/* 2298 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 2300 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 2302 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter pBstrLibName */

/* 2304 */	NdrFcShort( 0x2113 ),	/* Flags:  must size, must free, out, simple ref, srv alloc size=8 */
/* 2306 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 2308 */	NdrFcShort( 0x496 ),	/* Type Offset=1174 */

	/* Return value */

/* 2310 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 2312 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 2314 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure RemoteFindName */

/* 2316 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 2318 */	NdrFcLong( 0x0 ),	/* 0 */
/* 2322 */	NdrFcShort( 0xb ),	/* 11 */
/* 2324 */	NdrFcShort( 0x20 ),	/* x86 Stack size/offset = 32 */
/* 2326 */	NdrFcShort( 0x22 ),	/* 34 */
/* 2328 */	NdrFcShort( 0x22 ),	/* 34 */
/* 2330 */	0x47,		/* Oi2 Flags:  srv must size, clt must size, has return, has ext, */
			0x7,		/* 7 */
/* 2332 */	0x8,		/* 8 */
			0x3,		/* Ext Flags:  new corr desc, clt corr check, */
/* 2334 */	NdrFcShort( 0x3 ),	/* 3 */
/* 2336 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2338 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter szNameBuf */

/* 2340 */	NdrFcShort( 0x10b ),	/* Flags:  must size, must free, in, simple ref, */
/* 2342 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 2344 */	NdrFcShort( 0x518 ),	/* Type Offset=1304 */

	/* Parameter lHashVal */

/* 2346 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 2348 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 2350 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter ppTInfo */

/* 2352 */	NdrFcShort( 0x113 ),	/* Flags:  must size, must free, out, simple ref, */
/* 2354 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 2356 */	NdrFcShort( 0x78c ),	/* Type Offset=1932 */

	/* Parameter rgMemId */

/* 2358 */	NdrFcShort( 0x113 ),	/* Flags:  must size, must free, out, simple ref, */
/* 2360 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 2362 */	NdrFcShort( 0x7a6 ),	/* Type Offset=1958 */

	/* Parameter pcFound */

/* 2364 */	NdrFcShort( 0x158 ),	/* Flags:  in, out, base type, simple ref, */
/* 2366 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 2368 */	0x6,		/* FC_SHORT */
			0x0,		/* 0 */

	/* Parameter pBstrLibName */

/* 2370 */	NdrFcShort( 0x2113 ),	/* Flags:  must size, must free, out, simple ref, srv alloc size=8 */
/* 2372 */	NdrFcShort( 0x18 ),	/* x86 Stack size/offset = 24 */
/* 2374 */	NdrFcShort( 0x496 ),	/* Type Offset=1174 */

	/* Return value */

/* 2376 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 2378 */	NdrFcShort( 0x1c ),	/* x86 Stack size/offset = 28 */
/* 2380 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure LocalReleaseTLibAttr */

/* 2382 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 2384 */	NdrFcLong( 0x0 ),	/* 0 */
/* 2388 */	NdrFcShort( 0xc ),	/* 12 */
/* 2390 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 2392 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2394 */	NdrFcShort( 0x8 ),	/* 8 */
/* 2396 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x1,		/* 1 */
/* 2398 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 2400 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2402 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2404 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Return value */

/* 2406 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 2408 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 2410 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetCustData */

/* 2412 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 2414 */	NdrFcLong( 0x0 ),	/* 0 */
/* 2418 */	NdrFcShort( 0xd ),	/* 13 */
/* 2420 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 2422 */	NdrFcShort( 0x44 ),	/* 68 */
/* 2424 */	NdrFcShort( 0x8 ),	/* 8 */
/* 2426 */	0x45,		/* Oi2 Flags:  srv must size, has return, has ext, */
			0x3,		/* 3 */
/* 2428 */	0x8,		/* 8 */
			0x3,		/* Ext Flags:  new corr desc, clt corr check, */
/* 2430 */	NdrFcShort( 0x20 ),	/* 32 */
/* 2432 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2434 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter guid */

/* 2436 */	NdrFcShort( 0x10a ),	/* Flags:  must free, in, simple ref, */
/* 2438 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 2440 */	NdrFcShort( 0x26 ),	/* Type Offset=38 */

	/* Parameter pVarVal */

/* 2442 */	NdrFcShort( 0x4113 ),	/* Flags:  must size, must free, out, simple ref, srv alloc size=16 */
/* 2444 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 2446 */	NdrFcShort( 0x484 ),	/* Type Offset=1156 */

	/* Return value */

/* 2448 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 2450 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 2452 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure RemoteGetLibStatistics */

/* 2454 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 2456 */	NdrFcLong( 0x0 ),	/* 0 */
/* 2460 */	NdrFcShort( 0xe ),	/* 14 */
/* 2462 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 2464 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2466 */	NdrFcShort( 0x40 ),	/* 64 */
/* 2468 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x3,		/* 3 */
/* 2470 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 2472 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2474 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2476 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pcUniqueNames */

/* 2478 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 2480 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 2482 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter pcchUniqueNames */

/* 2484 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 2486 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 2488 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 2490 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 2492 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 2494 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure RemoteGetDocumentation2 */

/* 2496 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 2498 */	NdrFcLong( 0x0 ),	/* 0 */
/* 2502 */	NdrFcShort( 0xf ),	/* 15 */
/* 2504 */	NdrFcShort( 0x20 ),	/* x86 Stack size/offset = 32 */
/* 2506 */	NdrFcShort( 0x18 ),	/* 24 */
/* 2508 */	NdrFcShort( 0x24 ),	/* 36 */
/* 2510 */	0x45,		/* Oi2 Flags:  srv must size, has return, has ext, */
			0x7,		/* 7 */
/* 2512 */	0x8,		/* 8 */
			0x3,		/* Ext Flags:  new corr desc, clt corr check, */
/* 2514 */	NdrFcShort( 0x2 ),	/* 2 */
/* 2516 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2518 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter index */

/* 2520 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 2522 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 2524 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter lcid */

/* 2526 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 2528 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 2530 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter refPtrFlags */

/* 2532 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 2534 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 2536 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter pbstrHelpString */

/* 2538 */	NdrFcShort( 0x2113 ),	/* Flags:  must size, must free, out, simple ref, srv alloc size=8 */
/* 2540 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 2542 */	NdrFcShort( 0x496 ),	/* Type Offset=1174 */

	/* Parameter pdwHelpStringContext */

/* 2544 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 2546 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 2548 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter pbstrHelpStringDll */

/* 2550 */	NdrFcShort( 0x2113 ),	/* Flags:  must size, must free, out, simple ref, srv alloc size=8 */
/* 2552 */	NdrFcShort( 0x18 ),	/* x86 Stack size/offset = 24 */
/* 2554 */	NdrFcShort( 0x496 ),	/* Type Offset=1174 */

	/* Return value */

/* 2556 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 2558 */	NdrFcShort( 0x1c ),	/* x86 Stack size/offset = 28 */
/* 2560 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetAllCustData */

/* 2562 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 2564 */	NdrFcLong( 0x0 ),	/* 0 */
/* 2568 */	NdrFcShort( 0x10 ),	/* 16 */
/* 2570 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 2572 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2574 */	NdrFcShort( 0x8 ),	/* 8 */
/* 2576 */	0x45,		/* Oi2 Flags:  srv must size, has return, has ext, */
			0x2,		/* 2 */
/* 2578 */	0x8,		/* 8 */
			0x3,		/* Ext Flags:  new corr desc, clt corr check, */
/* 2580 */	NdrFcShort( 0x21 ),	/* 33 */
/* 2582 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2584 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pCustData */

/* 2586 */	NdrFcShort( 0x2113 ),	/* Flags:  must size, must free, out, simple ref, srv alloc size=8 */
/* 2588 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 2590 */	NdrFcShort( 0x74c ),	/* Type Offset=1868 */

	/* Return value */

/* 2592 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 2594 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 2596 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetGUID */

/* 2598 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 2600 */	NdrFcLong( 0x0 ),	/* 0 */
/* 2604 */	NdrFcShort( 0x3 ),	/* 3 */
/* 2606 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 2608 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2610 */	NdrFcShort( 0x4c ),	/* 76 */
/* 2612 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x2,		/* 2 */
/* 2614 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 2616 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2618 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2620 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pGUID */

/* 2622 */	NdrFcShort( 0x4112 ),	/* Flags:  must free, out, simple ref, srv alloc size=16 */
/* 2624 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 2626 */	NdrFcShort( 0x26 ),	/* Type Offset=38 */

	/* Return value */

/* 2628 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 2630 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 2632 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetSource */

/* 2634 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 2636 */	NdrFcLong( 0x0 ),	/* 0 */
/* 2640 */	NdrFcShort( 0x4 ),	/* 4 */
/* 2642 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 2644 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2646 */	NdrFcShort( 0x8 ),	/* 8 */
/* 2648 */	0x45,		/* Oi2 Flags:  srv must size, has return, has ext, */
			0x2,		/* 2 */
/* 2650 */	0x8,		/* 8 */
			0x3,		/* Ext Flags:  new corr desc, clt corr check, */
/* 2652 */	NdrFcShort( 0x1 ),	/* 1 */
/* 2654 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2656 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pBstrSource */

/* 2658 */	NdrFcShort( 0x2113 ),	/* Flags:  must size, must free, out, simple ref, srv alloc size=8 */
/* 2660 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 2662 */	NdrFcShort( 0x496 ),	/* Type Offset=1174 */

	/* Return value */

/* 2664 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 2666 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 2668 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetDescription */

/* 2670 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 2672 */	NdrFcLong( 0x0 ),	/* 0 */
/* 2676 */	NdrFcShort( 0x5 ),	/* 5 */
/* 2678 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 2680 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2682 */	NdrFcShort( 0x8 ),	/* 8 */
/* 2684 */	0x45,		/* Oi2 Flags:  srv must size, has return, has ext, */
			0x2,		/* 2 */
/* 2686 */	0x8,		/* 8 */
			0x3,		/* Ext Flags:  new corr desc, clt corr check, */
/* 2688 */	NdrFcShort( 0x1 ),	/* 1 */
/* 2690 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2692 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pBstrDescription */

/* 2694 */	NdrFcShort( 0x2113 ),	/* Flags:  must size, must free, out, simple ref, srv alloc size=8 */
/* 2696 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 2698 */	NdrFcShort( 0x496 ),	/* Type Offset=1174 */

	/* Return value */

/* 2700 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 2702 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 2704 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetHelpFile */

/* 2706 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 2708 */	NdrFcLong( 0x0 ),	/* 0 */
/* 2712 */	NdrFcShort( 0x6 ),	/* 6 */
/* 2714 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 2716 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2718 */	NdrFcShort( 0x8 ),	/* 8 */
/* 2720 */	0x45,		/* Oi2 Flags:  srv must size, has return, has ext, */
			0x2,		/* 2 */
/* 2722 */	0x8,		/* 8 */
			0x3,		/* Ext Flags:  new corr desc, clt corr check, */
/* 2724 */	NdrFcShort( 0x1 ),	/* 1 */
/* 2726 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2728 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pBstrHelpFile */

/* 2730 */	NdrFcShort( 0x2113 ),	/* Flags:  must size, must free, out, simple ref, srv alloc size=8 */
/* 2732 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 2734 */	NdrFcShort( 0x496 ),	/* Type Offset=1174 */

	/* Return value */

/* 2736 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 2738 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 2740 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetHelpContext */

/* 2742 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 2744 */	NdrFcLong( 0x0 ),	/* 0 */
/* 2748 */	NdrFcShort( 0x7 ),	/* 7 */
/* 2750 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 2752 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2754 */	NdrFcShort( 0x24 ),	/* 36 */
/* 2756 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x2,		/* 2 */
/* 2758 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 2760 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2762 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2764 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pdwHelpContext */

/* 2766 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 2768 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 2770 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 2772 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 2774 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 2776 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure InterfaceSupportsErrorInfo */


	/* Procedure SetGUID */

/* 2778 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 2780 */	NdrFcLong( 0x0 ),	/* 0 */
/* 2784 */	NdrFcShort( 0x3 ),	/* 3 */
/* 2786 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 2788 */	NdrFcShort( 0x44 ),	/* 68 */
/* 2790 */	NdrFcShort( 0x8 ),	/* 8 */
/* 2792 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x2,		/* 2 */
/* 2794 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 2796 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2798 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2800 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter riid */


	/* Parameter rguid */

/* 2802 */	NdrFcShort( 0x10a ),	/* Flags:  must free, in, simple ref, */
/* 2804 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 2806 */	NdrFcShort( 0x26 ),	/* Type Offset=38 */

	/* Return value */


	/* Return value */

/* 2808 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 2810 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 2812 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure SetSource */

/* 2814 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 2816 */	NdrFcLong( 0x0 ),	/* 0 */
/* 2820 */	NdrFcShort( 0x4 ),	/* 4 */
/* 2822 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 2824 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2826 */	NdrFcShort( 0x8 ),	/* 8 */
/* 2828 */	0x46,		/* Oi2 Flags:  clt must size, has return, has ext, */
			0x2,		/* 2 */
/* 2830 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 2832 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2834 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2836 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter szSource */

/* 2838 */	NdrFcShort( 0x10b ),	/* Flags:  must size, must free, in, simple ref, */
/* 2840 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 2842 */	NdrFcShort( 0x518 ),	/* Type Offset=1304 */

	/* Return value */

/* 2844 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 2846 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 2848 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure SetDescription */

/* 2850 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 2852 */	NdrFcLong( 0x0 ),	/* 0 */
/* 2856 */	NdrFcShort( 0x5 ),	/* 5 */
/* 2858 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 2860 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2862 */	NdrFcShort( 0x8 ),	/* 8 */
/* 2864 */	0x46,		/* Oi2 Flags:  clt must size, has return, has ext, */
			0x2,		/* 2 */
/* 2866 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 2868 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2870 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2872 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter szDescription */

/* 2874 */	NdrFcShort( 0x10b ),	/* Flags:  must size, must free, in, simple ref, */
/* 2876 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 2878 */	NdrFcShort( 0x518 ),	/* Type Offset=1304 */

	/* Return value */

/* 2880 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 2882 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 2884 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure SetHelpFile */

/* 2886 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 2888 */	NdrFcLong( 0x0 ),	/* 0 */
/* 2892 */	NdrFcShort( 0x6 ),	/* 6 */
/* 2894 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 2896 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2898 */	NdrFcShort( 0x8 ),	/* 8 */
/* 2900 */	0x46,		/* Oi2 Flags:  clt must size, has return, has ext, */
			0x2,		/* 2 */
/* 2902 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 2904 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2906 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2908 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter szHelpFile */

/* 2910 */	NdrFcShort( 0x10b ),	/* Flags:  must size, must free, in, simple ref, */
/* 2912 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 2914 */	NdrFcShort( 0x518 ),	/* Type Offset=1304 */

	/* Return value */

/* 2916 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 2918 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 2920 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure SetHelpContext */

/* 2922 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 2924 */	NdrFcLong( 0x0 ),	/* 0 */
/* 2928 */	NdrFcShort( 0x7 ),	/* 7 */
/* 2930 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 2932 */	NdrFcShort( 0x8 ),	/* 8 */
/* 2934 */	NdrFcShort( 0x8 ),	/* 8 */
/* 2936 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x2,		/* 2 */
/* 2938 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 2940 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2942 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2944 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter dwHelpContext */

/* 2946 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 2948 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 2950 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 2952 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 2954 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 2956 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure CreateFromTypeInfo */

/* 2958 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 2960 */	NdrFcLong( 0x0 ),	/* 0 */
/* 2964 */	NdrFcShort( 0x3 ),	/* 3 */
/* 2966 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 2968 */	NdrFcShort( 0x44 ),	/* 68 */
/* 2970 */	NdrFcShort( 0x8 ),	/* 8 */
/* 2972 */	0x47,		/* Oi2 Flags:  srv must size, clt must size, has return, has ext, */
			0x4,		/* 4 */
/* 2974 */	0x8,		/* 8 */
			0x3,		/* Ext Flags:  new corr desc, clt corr check, */
/* 2976 */	NdrFcShort( 0x1 ),	/* 1 */
/* 2978 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2980 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pTypeInfo */

/* 2982 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 2984 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 2986 */	NdrFcShort( 0xa ),	/* Type Offset=10 */

	/* Parameter riid */

/* 2988 */	NdrFcShort( 0x10a ),	/* Flags:  must free, in, simple ref, */
/* 2990 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 2992 */	NdrFcShort( 0x26 ),	/* Type Offset=38 */

	/* Parameter ppv */

/* 2994 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
/* 2996 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 2998 */	NdrFcShort( 0x7c0 ),	/* Type Offset=1984 */

	/* Return value */

/* 3000 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 3002 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 3004 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure AddError */

/* 3006 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 3008 */	NdrFcLong( 0x0 ),	/* 0 */
/* 3012 */	NdrFcShort( 0x3 ),	/* 3 */
/* 3014 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 3016 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3018 */	NdrFcShort( 0x8 ),	/* 8 */
/* 3020 */	0x46,		/* Oi2 Flags:  clt must size, has return, has ext, */
			0x3,		/* 3 */
/* 3022 */	0x8,		/* 8 */
			0x5,		/* Ext Flags:  new corr desc, srv corr check, */
/* 3024 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3026 */	NdrFcShort( 0x3 ),	/* 3 */
/* 3028 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pszPropName */

/* 3030 */	NdrFcShort( 0x10b ),	/* Flags:  must size, must free, in, simple ref, */
/* 3032 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 3034 */	NdrFcShort( 0x518 ),	/* Type Offset=1304 */

	/* Parameter pExcepInfo */

/* 3036 */	NdrFcShort( 0x10b ),	/* Flags:  must size, must free, in, simple ref, */
/* 3038 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 3040 */	NdrFcShort( 0x4a0 ),	/* Type Offset=1184 */

	/* Return value */

/* 3042 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 3044 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 3046 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure RemoteRead */

/* 3048 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 3050 */	NdrFcLong( 0x0 ),	/* 0 */
/* 3054 */	NdrFcShort( 0x3 ),	/* 3 */
/* 3056 */	NdrFcShort( 0x1c ),	/* x86 Stack size/offset = 28 */
/* 3058 */	NdrFcShort( 0x8 ),	/* 8 */
/* 3060 */	NdrFcShort( 0x8 ),	/* 8 */
/* 3062 */	0x47,		/* Oi2 Flags:  srv must size, clt must size, has return, has ext, */
			0x6,		/* 6 */
/* 3064 */	0x8,		/* 8 */
			0x3,		/* Ext Flags:  new corr desc, clt corr check, */
/* 3066 */	NdrFcShort( 0x20 ),	/* 32 */
/* 3068 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3070 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pszPropName */

/* 3072 */	NdrFcShort( 0x10b ),	/* Flags:  must size, must free, in, simple ref, */
/* 3074 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 3076 */	NdrFcShort( 0x518 ),	/* Type Offset=1304 */

	/* Parameter pVar */

/* 3078 */	NdrFcShort( 0x4113 ),	/* Flags:  must size, must free, out, simple ref, srv alloc size=16 */
/* 3080 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 3082 */	NdrFcShort( 0x484 ),	/* Type Offset=1156 */

	/* Parameter pErrorLog */

/* 3084 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 3086 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 3088 */	NdrFcShort( 0x7d0 ),	/* Type Offset=2000 */

	/* Parameter varType */

/* 3090 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 3092 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 3094 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter pUnkObj */

/* 3096 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 3098 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 3100 */	NdrFcShort( 0x1a0 ),	/* Type Offset=416 */

	/* Return value */

/* 3102 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 3104 */	NdrFcShort( 0x18 ),	/* x86 Stack size/offset = 24 */
/* 3106 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure Write */

/* 3108 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 3110 */	NdrFcLong( 0x0 ),	/* 0 */
/* 3114 */	NdrFcShort( 0x4 ),	/* 4 */
/* 3116 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 3118 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3120 */	NdrFcShort( 0x8 ),	/* 8 */
/* 3122 */	0x46,		/* Oi2 Flags:  clt must size, has return, has ext, */
			0x3,		/* 3 */
/* 3124 */	0x8,		/* 8 */
			0x5,		/* Ext Flags:  new corr desc, srv corr check, */
/* 3126 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3128 */	NdrFcShort( 0x20 ),	/* 32 */
/* 3130 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pszPropName */

/* 3132 */	NdrFcShort( 0x10b ),	/* Flags:  must size, must free, in, simple ref, */
/* 3134 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 3136 */	NdrFcShort( 0x518 ),	/* Type Offset=1304 */

	/* Parameter pVar */

/* 3138 */	NdrFcShort( 0x10b ),	/* Flags:  must size, must free, in, simple ref, */
/* 3140 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 3142 */	NdrFcShort( 0x43a ),	/* Type Offset=1082 */

	/* Return value */

/* 3144 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 3146 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 3148 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

			0x0
        }
    };

static const MIDL_TYPE_FORMAT_STRING __MIDL_TypeFormatString =
    {
        0,
        {
			NdrFcShort( 0x0 ),	/* 0 */
/*  2 */	
			0x11, 0xc,	/* FC_RP [alloced_on_stack] [simple_pointer] */
/*  4 */	0x8,		/* FC_LONG */
			0x5c,		/* FC_PAD */
/*  6 */	
			0x11, 0x10,	/* FC_RP [pointer_deref] */
/*  8 */	NdrFcShort( 0x2 ),	/* Offset= 2 (10) */
/* 10 */	
			0x2f,		/* FC_IP */
			0x5a,		/* FC_CONSTANT_IID */
/* 12 */	NdrFcLong( 0x20401 ),	/* 132097 */
/* 16 */	NdrFcShort( 0x0 ),	/* 0 */
/* 18 */	NdrFcShort( 0x0 ),	/* 0 */
/* 20 */	0xc0,		/* 192 */
			0x0,		/* 0 */
/* 22 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 24 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 26 */	0x0,		/* 0 */
			0x46,		/* 70 */
/* 28 */	
			0x11, 0x0,	/* FC_RP */
/* 30 */	NdrFcShort( 0x8 ),	/* Offset= 8 (38) */
/* 32 */	
			0x1d,		/* FC_SMFARRAY */
			0x0,		/* 0 */
/* 34 */	NdrFcShort( 0x8 ),	/* 8 */
/* 36 */	0x2,		/* FC_CHAR */
			0x5b,		/* FC_END */
/* 38 */	
			0x15,		/* FC_STRUCT */
			0x3,		/* 3 */
/* 40 */	NdrFcShort( 0x10 ),	/* 16 */
/* 42 */	0x8,		/* FC_LONG */
			0x6,		/* FC_SHORT */
/* 44 */	0x6,		/* FC_SHORT */
			0x4c,		/* FC_EMBEDDED_COMPLEX */
/* 46 */	0x0,		/* 0 */
			NdrFcShort( 0xfffffff1 ),	/* Offset= -15 (32) */
			0x5b,		/* FC_END */
/* 50 */	
			0x11, 0x0,	/* FC_RP */
/* 52 */	NdrFcShort( 0x2 ),	/* Offset= 2 (54) */
/* 54 */	
			0x1b,		/* FC_CARRAY */
			0x3,		/* 3 */
/* 56 */	NdrFcShort( 0x4 ),	/* 4 */
/* 58 */	0x29,		/* Corr desc:  parameter, FC_ULONG */
			0x0,		/*  */
/* 60 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 62 */	NdrFcShort( 0x0 ),	/* Corr flags:  */
/* 64 */	
			0x4b,		/* FC_PP */
			0x5c,		/* FC_PAD */
/* 66 */	
			0x48,		/* FC_VARIABLE_REPEAT */
			0x49,		/* FC_FIXED_OFFSET */
/* 68 */	NdrFcShort( 0x4 ),	/* 4 */
/* 70 */	NdrFcShort( 0x0 ),	/* 0 */
/* 72 */	NdrFcShort( 0x1 ),	/* 1 */
/* 74 */	NdrFcShort( 0x0 ),	/* 0 */
/* 76 */	NdrFcShort( 0x0 ),	/* 0 */
/* 78 */	0x12, 0x8,	/* FC_UP [simple_pointer] */
/* 80 */	
			0x25,		/* FC_C_WSTRING */
			0x5c,		/* FC_PAD */
/* 82 */	
			0x5b,		/* FC_END */

			0x8,		/* FC_LONG */
/* 84 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 86 */	
			0x11, 0x0,	/* FC_RP */
/* 88 */	NdrFcShort( 0x2 ),	/* Offset= 2 (90) */
/* 90 */	
			0x1b,		/* FC_CARRAY */
			0x3,		/* 3 */
/* 92 */	NdrFcShort( 0x4 ),	/* 4 */
/* 94 */	0x29,		/* Corr desc:  parameter, FC_ULONG */
			0x0,		/*  */
/* 96 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 98 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 100 */	0x8,		/* FC_LONG */
			0x5b,		/* FC_END */
/* 102 */	
			0x11, 0x0,	/* FC_RP */
/* 104 */	NdrFcShort( 0x3fe ),	/* Offset= 1022 (1126) */
/* 106 */	
			0x12, 0x0,	/* FC_UP */
/* 108 */	NdrFcShort( 0x3ba ),	/* Offset= 954 (1062) */
/* 110 */	
			0x2b,		/* FC_NON_ENCAPSULATED_UNION */
			0x9,		/* FC_ULONG */
/* 112 */	0x7,		/* Corr desc: FC_USHORT */
			0x0,		/*  */
/* 114 */	NdrFcShort( 0xfff8 ),	/* -8 */
/* 116 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 118 */	NdrFcShort( 0x2 ),	/* Offset= 2 (120) */
/* 120 */	NdrFcShort( 0x10 ),	/* 16 */
/* 122 */	NdrFcShort( 0x2b ),	/* 43 */
/* 124 */	NdrFcLong( 0x3 ),	/* 3 */
/* 128 */	NdrFcShort( 0x8008 ),	/* Simple arm type: FC_LONG */
/* 130 */	NdrFcLong( 0x11 ),	/* 17 */
/* 134 */	NdrFcShort( 0x8001 ),	/* Simple arm type: FC_BYTE */
/* 136 */	NdrFcLong( 0x2 ),	/* 2 */
/* 140 */	NdrFcShort( 0x8006 ),	/* Simple arm type: FC_SHORT */
/* 142 */	NdrFcLong( 0x4 ),	/* 4 */
/* 146 */	NdrFcShort( 0x800a ),	/* Simple arm type: FC_FLOAT */
/* 148 */	NdrFcLong( 0x5 ),	/* 5 */
/* 152 */	NdrFcShort( 0x800c ),	/* Simple arm type: FC_DOUBLE */
/* 154 */	NdrFcLong( 0xb ),	/* 11 */
/* 158 */	NdrFcShort( 0x8006 ),	/* Simple arm type: FC_SHORT */
/* 160 */	NdrFcLong( 0xa ),	/* 10 */
/* 164 */	NdrFcShort( 0x8008 ),	/* Simple arm type: FC_LONG */
/* 166 */	NdrFcLong( 0x6 ),	/* 6 */
/* 170 */	NdrFcShort( 0xd6 ),	/* Offset= 214 (384) */
/* 172 */	NdrFcLong( 0x7 ),	/* 7 */
/* 176 */	NdrFcShort( 0x800c ),	/* Simple arm type: FC_DOUBLE */
/* 178 */	NdrFcLong( 0x8 ),	/* 8 */
/* 182 */	NdrFcShort( 0xd0 ),	/* Offset= 208 (390) */
/* 184 */	NdrFcLong( 0xd ),	/* 13 */
/* 188 */	NdrFcShort( 0xe4 ),	/* Offset= 228 (416) */
/* 190 */	NdrFcLong( 0x9 ),	/* 9 */
/* 194 */	NdrFcShort( 0xf0 ),	/* Offset= 240 (434) */
/* 196 */	NdrFcLong( 0x2000 ),	/* 8192 */
/* 200 */	NdrFcShort( 0xfc ),	/* Offset= 252 (452) */
/* 202 */	NdrFcLong( 0x24 ),	/* 36 */
/* 206 */	NdrFcShort( 0x312 ),	/* Offset= 786 (992) */
/* 208 */	NdrFcLong( 0x4024 ),	/* 16420 */
/* 212 */	NdrFcShort( 0x30c ),	/* Offset= 780 (992) */
/* 214 */	NdrFcLong( 0x4011 ),	/* 16401 */
/* 218 */	NdrFcShort( 0x30a ),	/* Offset= 778 (996) */
/* 220 */	NdrFcLong( 0x4002 ),	/* 16386 */
/* 224 */	NdrFcShort( 0x308 ),	/* Offset= 776 (1000) */
/* 226 */	NdrFcLong( 0x4003 ),	/* 16387 */
/* 230 */	NdrFcShort( 0x306 ),	/* Offset= 774 (1004) */
/* 232 */	NdrFcLong( 0x4004 ),	/* 16388 */
/* 236 */	NdrFcShort( 0x304 ),	/* Offset= 772 (1008) */
/* 238 */	NdrFcLong( 0x4005 ),	/* 16389 */
/* 242 */	NdrFcShort( 0x302 ),	/* Offset= 770 (1012) */
/* 244 */	NdrFcLong( 0x400b ),	/* 16395 */
/* 248 */	NdrFcShort( 0x2f0 ),	/* Offset= 752 (1000) */
/* 250 */	NdrFcLong( 0x400a ),	/* 16394 */
/* 254 */	NdrFcShort( 0x2ee ),	/* Offset= 750 (1004) */
/* 256 */	NdrFcLong( 0x4006 ),	/* 16390 */
/* 260 */	NdrFcShort( 0x2f4 ),	/* Offset= 756 (1016) */
/* 262 */	NdrFcLong( 0x4007 ),	/* 16391 */
/* 266 */	NdrFcShort( 0x2ea ),	/* Offset= 746 (1012) */
/* 268 */	NdrFcLong( 0x4008 ),	/* 16392 */
/* 272 */	NdrFcShort( 0x2ec ),	/* Offset= 748 (1020) */
/* 274 */	NdrFcLong( 0x400d ),	/* 16397 */
/* 278 */	NdrFcShort( 0x2ea ),	/* Offset= 746 (1024) */
/* 280 */	NdrFcLong( 0x4009 ),	/* 16393 */
/* 284 */	NdrFcShort( 0x2e8 ),	/* Offset= 744 (1028) */
/* 286 */	NdrFcLong( 0x6000 ),	/* 24576 */
/* 290 */	NdrFcShort( 0x2e6 ),	/* Offset= 742 (1032) */
/* 292 */	NdrFcLong( 0x400c ),	/* 16396 */
/* 296 */	NdrFcShort( 0x2e4 ),	/* Offset= 740 (1036) */
/* 298 */	NdrFcLong( 0x10 ),	/* 16 */
/* 302 */	NdrFcShort( 0x8002 ),	/* Simple arm type: FC_CHAR */
/* 304 */	NdrFcLong( 0x12 ),	/* 18 */
/* 308 */	NdrFcShort( 0x8006 ),	/* Simple arm type: FC_SHORT */
/* 310 */	NdrFcLong( 0x13 ),	/* 19 */
/* 314 */	NdrFcShort( 0x8008 ),	/* Simple arm type: FC_LONG */
/* 316 */	NdrFcLong( 0x16 ),	/* 22 */
/* 320 */	NdrFcShort( 0x8008 ),	/* Simple arm type: FC_LONG */
/* 322 */	NdrFcLong( 0x17 ),	/* 23 */
/* 326 */	NdrFcShort( 0x8008 ),	/* Simple arm type: FC_LONG */
/* 328 */	NdrFcLong( 0xe ),	/* 14 */
/* 332 */	NdrFcShort( 0x2c8 ),	/* Offset= 712 (1044) */
/* 334 */	NdrFcLong( 0x400e ),	/* 16398 */
/* 338 */	NdrFcShort( 0x2cc ),	/* Offset= 716 (1054) */
/* 340 */	NdrFcLong( 0x4010 ),	/* 16400 */
/* 344 */	NdrFcShort( 0x2ca ),	/* Offset= 714 (1058) */
/* 346 */	NdrFcLong( 0x4012 ),	/* 16402 */
/* 350 */	NdrFcShort( 0x28a ),	/* Offset= 650 (1000) */
/* 352 */	NdrFcLong( 0x4013 ),	/* 16403 */
/* 356 */	NdrFcShort( 0x288 ),	/* Offset= 648 (1004) */
/* 358 */	NdrFcLong( 0x4016 ),	/* 16406 */
/* 362 */	NdrFcShort( 0x282 ),	/* Offset= 642 (1004) */
/* 364 */	NdrFcLong( 0x4017 ),	/* 16407 */
/* 368 */	NdrFcShort( 0x27c ),	/* Offset= 636 (1004) */
/* 370 */	NdrFcLong( 0x0 ),	/* 0 */
/* 374 */	NdrFcShort( 0x0 ),	/* Offset= 0 (374) */
/* 376 */	NdrFcLong( 0x1 ),	/* 1 */
/* 380 */	NdrFcShort( 0x0 ),	/* Offset= 0 (380) */
/* 382 */	NdrFcShort( 0xffffffff ),	/* Offset= -1 (381) */
/* 384 */	
			0x15,		/* FC_STRUCT */
			0x7,		/* 7 */
/* 386 */	NdrFcShort( 0x8 ),	/* 8 */
/* 388 */	0xb,		/* FC_HYPER */
			0x5b,		/* FC_END */
/* 390 */	
			0x12, 0x0,	/* FC_UP */
/* 392 */	NdrFcShort( 0xe ),	/* Offset= 14 (406) */
/* 394 */	
			0x1b,		/* FC_CARRAY */
			0x1,		/* 1 */
/* 396 */	NdrFcShort( 0x2 ),	/* 2 */
/* 398 */	0x9,		/* Corr desc: FC_ULONG */
			0x0,		/*  */
/* 400 */	NdrFcShort( 0xfffc ),	/* -4 */
/* 402 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 404 */	0x6,		/* FC_SHORT */
			0x5b,		/* FC_END */
/* 406 */	
			0x17,		/* FC_CSTRUCT */
			0x3,		/* 3 */
/* 408 */	NdrFcShort( 0x8 ),	/* 8 */
/* 410 */	NdrFcShort( 0xfffffff0 ),	/* Offset= -16 (394) */
/* 412 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 414 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 416 */	
			0x2f,		/* FC_IP */
			0x5a,		/* FC_CONSTANT_IID */
/* 418 */	NdrFcLong( 0x0 ),	/* 0 */
/* 422 */	NdrFcShort( 0x0 ),	/* 0 */
/* 424 */	NdrFcShort( 0x0 ),	/* 0 */
/* 426 */	0xc0,		/* 192 */
			0x0,		/* 0 */
/* 428 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 430 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 432 */	0x0,		/* 0 */
			0x46,		/* 70 */
/* 434 */	
			0x2f,		/* FC_IP */
			0x5a,		/* FC_CONSTANT_IID */
/* 436 */	NdrFcLong( 0x20400 ),	/* 132096 */
/* 440 */	NdrFcShort( 0x0 ),	/* 0 */
/* 442 */	NdrFcShort( 0x0 ),	/* 0 */
/* 444 */	0xc0,		/* 192 */
			0x0,		/* 0 */
/* 446 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 448 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 450 */	0x0,		/* 0 */
			0x46,		/* 70 */
/* 452 */	
			0x12, 0x10,	/* FC_UP [pointer_deref] */
/* 454 */	NdrFcShort( 0x2 ),	/* Offset= 2 (456) */
/* 456 */	
			0x12, 0x0,	/* FC_UP */
/* 458 */	NdrFcShort( 0x204 ),	/* Offset= 516 (974) */
/* 460 */	
			0x2a,		/* FC_ENCAPSULATED_UNION */
			0x49,		/* 73 */
/* 462 */	NdrFcShort( 0x18 ),	/* 24 */
/* 464 */	NdrFcShort( 0xa ),	/* 10 */
/* 466 */	NdrFcLong( 0x8 ),	/* 8 */
/* 470 */	NdrFcShort( 0x5a ),	/* Offset= 90 (560) */
/* 472 */	NdrFcLong( 0xd ),	/* 13 */
/* 476 */	NdrFcShort( 0x7e ),	/* Offset= 126 (602) */
/* 478 */	NdrFcLong( 0x9 ),	/* 9 */
/* 482 */	NdrFcShort( 0x9e ),	/* Offset= 158 (640) */
/* 484 */	NdrFcLong( 0xc ),	/* 12 */
/* 488 */	NdrFcShort( 0xc8 ),	/* Offset= 200 (688) */
/* 490 */	NdrFcLong( 0x24 ),	/* 36 */
/* 494 */	NdrFcShort( 0x124 ),	/* Offset= 292 (786) */
/* 496 */	NdrFcLong( 0x800d ),	/* 32781 */
/* 500 */	NdrFcShort( 0x12e ),	/* Offset= 302 (802) */
/* 502 */	NdrFcLong( 0x10 ),	/* 16 */
/* 506 */	NdrFcShort( 0x148 ),	/* Offset= 328 (834) */
/* 508 */	NdrFcLong( 0x2 ),	/* 2 */
/* 512 */	NdrFcShort( 0x162 ),	/* Offset= 354 (866) */
/* 514 */	NdrFcLong( 0x3 ),	/* 3 */
/* 518 */	NdrFcShort( 0x17c ),	/* Offset= 380 (898) */
/* 520 */	NdrFcLong( 0x14 ),	/* 20 */
/* 524 */	NdrFcShort( 0x196 ),	/* Offset= 406 (930) */
/* 526 */	NdrFcShort( 0xffffffff ),	/* Offset= -1 (525) */
/* 528 */	
			0x1b,		/* FC_CARRAY */
			0x3,		/* 3 */
/* 530 */	NdrFcShort( 0x4 ),	/* 4 */
/* 532 */	0x19,		/* Corr desc:  field pointer, FC_ULONG */
			0x0,		/*  */
/* 534 */	NdrFcShort( 0x0 ),	/* 0 */
/* 536 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 538 */	
			0x4b,		/* FC_PP */
			0x5c,		/* FC_PAD */
/* 540 */	
			0x48,		/* FC_VARIABLE_REPEAT */
			0x49,		/* FC_FIXED_OFFSET */
/* 542 */	NdrFcShort( 0x4 ),	/* 4 */
/* 544 */	NdrFcShort( 0x0 ),	/* 0 */
/* 546 */	NdrFcShort( 0x1 ),	/* 1 */
/* 548 */	NdrFcShort( 0x0 ),	/* 0 */
/* 550 */	NdrFcShort( 0x0 ),	/* 0 */
/* 552 */	0x12, 0x0,	/* FC_UP */
/* 554 */	NdrFcShort( 0xffffff6c ),	/* Offset= -148 (406) */
/* 556 */	
			0x5b,		/* FC_END */

			0x8,		/* FC_LONG */
/* 558 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 560 */	
			0x16,		/* FC_PSTRUCT */
			0x3,		/* 3 */
/* 562 */	NdrFcShort( 0x8 ),	/* 8 */
/* 564 */	
			0x4b,		/* FC_PP */
			0x5c,		/* FC_PAD */
/* 566 */	
			0x46,		/* FC_NO_REPEAT */
			0x5c,		/* FC_PAD */
/* 568 */	NdrFcShort( 0x4 ),	/* 4 */
/* 570 */	NdrFcShort( 0x4 ),	/* 4 */
/* 572 */	0x11, 0x0,	/* FC_RP */
/* 574 */	NdrFcShort( 0xffffffd2 ),	/* Offset= -46 (528) */
/* 576 */	
			0x5b,		/* FC_END */

			0x8,		/* FC_LONG */
/* 578 */	0x8,		/* FC_LONG */
			0x5b,		/* FC_END */
/* 580 */	
			0x21,		/* FC_BOGUS_ARRAY */
			0x3,		/* 3 */
/* 582 */	NdrFcShort( 0x0 ),	/* 0 */
/* 584 */	0x19,		/* Corr desc:  field pointer, FC_ULONG */
			0x0,		/*  */
/* 586 */	NdrFcShort( 0x0 ),	/* 0 */
/* 588 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 590 */	NdrFcLong( 0xffffffff ),	/* -1 */
/* 594 */	NdrFcShort( 0x0 ),	/* Corr flags:  */
/* 596 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 598 */	NdrFcShort( 0xffffff4a ),	/* Offset= -182 (416) */
/* 600 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 602 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 604 */	NdrFcShort( 0x8 ),	/* 8 */
/* 606 */	NdrFcShort( 0x0 ),	/* 0 */
/* 608 */	NdrFcShort( 0x6 ),	/* Offset= 6 (614) */
/* 610 */	0x8,		/* FC_LONG */
			0x36,		/* FC_POINTER */
/* 612 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 614 */	
			0x11, 0x0,	/* FC_RP */
/* 616 */	NdrFcShort( 0xffffffdc ),	/* Offset= -36 (580) */
/* 618 */	
			0x21,		/* FC_BOGUS_ARRAY */
			0x3,		/* 3 */
/* 620 */	NdrFcShort( 0x0 ),	/* 0 */
/* 622 */	0x19,		/* Corr desc:  field pointer, FC_ULONG */
			0x0,		/*  */
/* 624 */	NdrFcShort( 0x0 ),	/* 0 */
/* 626 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 628 */	NdrFcLong( 0xffffffff ),	/* -1 */
/* 632 */	NdrFcShort( 0x0 ),	/* Corr flags:  */
/* 634 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 636 */	NdrFcShort( 0xffffff36 ),	/* Offset= -202 (434) */
/* 638 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 640 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 642 */	NdrFcShort( 0x8 ),	/* 8 */
/* 644 */	NdrFcShort( 0x0 ),	/* 0 */
/* 646 */	NdrFcShort( 0x6 ),	/* Offset= 6 (652) */
/* 648 */	0x8,		/* FC_LONG */
			0x36,		/* FC_POINTER */
/* 650 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 652 */	
			0x11, 0x0,	/* FC_RP */
/* 654 */	NdrFcShort( 0xffffffdc ),	/* Offset= -36 (618) */
/* 656 */	
			0x1b,		/* FC_CARRAY */
			0x3,		/* 3 */
/* 658 */	NdrFcShort( 0x4 ),	/* 4 */
/* 660 */	0x19,		/* Corr desc:  field pointer, FC_ULONG */
			0x0,		/*  */
/* 662 */	NdrFcShort( 0x0 ),	/* 0 */
/* 664 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 666 */	
			0x4b,		/* FC_PP */
			0x5c,		/* FC_PAD */
/* 668 */	
			0x48,		/* FC_VARIABLE_REPEAT */
			0x49,		/* FC_FIXED_OFFSET */
/* 670 */	NdrFcShort( 0x4 ),	/* 4 */
/* 672 */	NdrFcShort( 0x0 ),	/* 0 */
/* 674 */	NdrFcShort( 0x1 ),	/* 1 */
/* 676 */	NdrFcShort( 0x0 ),	/* 0 */
/* 678 */	NdrFcShort( 0x0 ),	/* 0 */
/* 680 */	0x12, 0x0,	/* FC_UP */
/* 682 */	NdrFcShort( 0x17c ),	/* Offset= 380 (1062) */
/* 684 */	
			0x5b,		/* FC_END */

			0x8,		/* FC_LONG */
/* 686 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 688 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 690 */	NdrFcShort( 0x8 ),	/* 8 */
/* 692 */	NdrFcShort( 0x0 ),	/* 0 */
/* 694 */	NdrFcShort( 0x6 ),	/* Offset= 6 (700) */
/* 696 */	0x8,		/* FC_LONG */
			0x36,		/* FC_POINTER */
/* 698 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 700 */	
			0x11, 0x0,	/* FC_RP */
/* 702 */	NdrFcShort( 0xffffffd2 ),	/* Offset= -46 (656) */
/* 704 */	
			0x2f,		/* FC_IP */
			0x5a,		/* FC_CONSTANT_IID */
/* 706 */	NdrFcLong( 0x2f ),	/* 47 */
/* 710 */	NdrFcShort( 0x0 ),	/* 0 */
/* 712 */	NdrFcShort( 0x0 ),	/* 0 */
/* 714 */	0xc0,		/* 192 */
			0x0,		/* 0 */
/* 716 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 718 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 720 */	0x0,		/* 0 */
			0x46,		/* 70 */
/* 722 */	
			0x1b,		/* FC_CARRAY */
			0x0,		/* 0 */
/* 724 */	NdrFcShort( 0x1 ),	/* 1 */
/* 726 */	0x19,		/* Corr desc:  field pointer, FC_ULONG */
			0x0,		/*  */
/* 728 */	NdrFcShort( 0x4 ),	/* 4 */
/* 730 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 732 */	0x1,		/* FC_BYTE */
			0x5b,		/* FC_END */
/* 734 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 736 */	NdrFcShort( 0x10 ),	/* 16 */
/* 738 */	NdrFcShort( 0x0 ),	/* 0 */
/* 740 */	NdrFcShort( 0xa ),	/* Offset= 10 (750) */
/* 742 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 744 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 746 */	NdrFcShort( 0xffffffd6 ),	/* Offset= -42 (704) */
/* 748 */	0x36,		/* FC_POINTER */
			0x5b,		/* FC_END */
/* 750 */	
			0x12, 0x0,	/* FC_UP */
/* 752 */	NdrFcShort( 0xffffffe2 ),	/* Offset= -30 (722) */
/* 754 */	
			0x1b,		/* FC_CARRAY */
			0x3,		/* 3 */
/* 756 */	NdrFcShort( 0x4 ),	/* 4 */
/* 758 */	0x19,		/* Corr desc:  field pointer, FC_ULONG */
			0x0,		/*  */
/* 760 */	NdrFcShort( 0x0 ),	/* 0 */
/* 762 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 764 */	
			0x4b,		/* FC_PP */
			0x5c,		/* FC_PAD */
/* 766 */	
			0x48,		/* FC_VARIABLE_REPEAT */
			0x49,		/* FC_FIXED_OFFSET */
/* 768 */	NdrFcShort( 0x4 ),	/* 4 */
/* 770 */	NdrFcShort( 0x0 ),	/* 0 */
/* 772 */	NdrFcShort( 0x1 ),	/* 1 */
/* 774 */	NdrFcShort( 0x0 ),	/* 0 */
/* 776 */	NdrFcShort( 0x0 ),	/* 0 */
/* 778 */	0x12, 0x0,	/* FC_UP */
/* 780 */	NdrFcShort( 0xffffffd2 ),	/* Offset= -46 (734) */
/* 782 */	
			0x5b,		/* FC_END */

			0x8,		/* FC_LONG */
/* 784 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 786 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 788 */	NdrFcShort( 0x8 ),	/* 8 */
/* 790 */	NdrFcShort( 0x0 ),	/* 0 */
/* 792 */	NdrFcShort( 0x6 ),	/* Offset= 6 (798) */
/* 794 */	0x8,		/* FC_LONG */
			0x36,		/* FC_POINTER */
/* 796 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 798 */	
			0x11, 0x0,	/* FC_RP */
/* 800 */	NdrFcShort( 0xffffffd2 ),	/* Offset= -46 (754) */
/* 802 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 804 */	NdrFcShort( 0x18 ),	/* 24 */
/* 806 */	NdrFcShort( 0x0 ),	/* 0 */
/* 808 */	NdrFcShort( 0xa ),	/* Offset= 10 (818) */
/* 810 */	0x8,		/* FC_LONG */
			0x36,		/* FC_POINTER */
/* 812 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 814 */	NdrFcShort( 0xfffffcf8 ),	/* Offset= -776 (38) */
/* 816 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 818 */	
			0x11, 0x0,	/* FC_RP */
/* 820 */	NdrFcShort( 0xffffff10 ),	/* Offset= -240 (580) */
/* 822 */	
			0x1b,		/* FC_CARRAY */
			0x0,		/* 0 */
/* 824 */	NdrFcShort( 0x1 ),	/* 1 */
/* 826 */	0x19,		/* Corr desc:  field pointer, FC_ULONG */
			0x0,		/*  */
/* 828 */	NdrFcShort( 0x0 ),	/* 0 */
/* 830 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 832 */	0x1,		/* FC_BYTE */
			0x5b,		/* FC_END */
/* 834 */	
			0x16,		/* FC_PSTRUCT */
			0x3,		/* 3 */
/* 836 */	NdrFcShort( 0x8 ),	/* 8 */
/* 838 */	
			0x4b,		/* FC_PP */
			0x5c,		/* FC_PAD */
/* 840 */	
			0x46,		/* FC_NO_REPEAT */
			0x5c,		/* FC_PAD */
/* 842 */	NdrFcShort( 0x4 ),	/* 4 */
/* 844 */	NdrFcShort( 0x4 ),	/* 4 */
/* 846 */	0x12, 0x0,	/* FC_UP */
/* 848 */	NdrFcShort( 0xffffffe6 ),	/* Offset= -26 (822) */
/* 850 */	
			0x5b,		/* FC_END */

			0x8,		/* FC_LONG */
/* 852 */	0x8,		/* FC_LONG */
			0x5b,		/* FC_END */
/* 854 */	
			0x1b,		/* FC_CARRAY */
			0x1,		/* 1 */
/* 856 */	NdrFcShort( 0x2 ),	/* 2 */
/* 858 */	0x19,		/* Corr desc:  field pointer, FC_ULONG */
			0x0,		/*  */
/* 860 */	NdrFcShort( 0x0 ),	/* 0 */
/* 862 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 864 */	0x6,		/* FC_SHORT */
			0x5b,		/* FC_END */
/* 866 */	
			0x16,		/* FC_PSTRUCT */
			0x3,		/* 3 */
/* 868 */	NdrFcShort( 0x8 ),	/* 8 */
/* 870 */	
			0x4b,		/* FC_PP */
			0x5c,		/* FC_PAD */
/* 872 */	
			0x46,		/* FC_NO_REPEAT */
			0x5c,		/* FC_PAD */
/* 874 */	NdrFcShort( 0x4 ),	/* 4 */
/* 876 */	NdrFcShort( 0x4 ),	/* 4 */
/* 878 */	0x12, 0x0,	/* FC_UP */
/* 880 */	NdrFcShort( 0xffffffe6 ),	/* Offset= -26 (854) */
/* 882 */	
			0x5b,		/* FC_END */

			0x8,		/* FC_LONG */
/* 884 */	0x8,		/* FC_LONG */
			0x5b,		/* FC_END */
/* 886 */	
			0x1b,		/* FC_CARRAY */
			0x3,		/* 3 */
/* 888 */	NdrFcShort( 0x4 ),	/* 4 */
/* 890 */	0x19,		/* Corr desc:  field pointer, FC_ULONG */
			0x0,		/*  */
/* 892 */	NdrFcShort( 0x0 ),	/* 0 */
/* 894 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 896 */	0x8,		/* FC_LONG */
			0x5b,		/* FC_END */
/* 898 */	
			0x16,		/* FC_PSTRUCT */
			0x3,		/* 3 */
/* 900 */	NdrFcShort( 0x8 ),	/* 8 */
/* 902 */	
			0x4b,		/* FC_PP */
			0x5c,		/* FC_PAD */
/* 904 */	
			0x46,		/* FC_NO_REPEAT */
			0x5c,		/* FC_PAD */
/* 906 */	NdrFcShort( 0x4 ),	/* 4 */
/* 908 */	NdrFcShort( 0x4 ),	/* 4 */
/* 910 */	0x12, 0x0,	/* FC_UP */
/* 912 */	NdrFcShort( 0xffffffe6 ),	/* Offset= -26 (886) */
/* 914 */	
			0x5b,		/* FC_END */

			0x8,		/* FC_LONG */
/* 916 */	0x8,		/* FC_LONG */
			0x5b,		/* FC_END */
/* 918 */	
			0x1b,		/* FC_CARRAY */
			0x7,		/* 7 */
/* 920 */	NdrFcShort( 0x8 ),	/* 8 */
/* 922 */	0x19,		/* Corr desc:  field pointer, FC_ULONG */
			0x0,		/*  */
/* 924 */	NdrFcShort( 0x0 ),	/* 0 */
/* 926 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 928 */	0xb,		/* FC_HYPER */
			0x5b,		/* FC_END */
/* 930 */	
			0x16,		/* FC_PSTRUCT */
			0x3,		/* 3 */
/* 932 */	NdrFcShort( 0x8 ),	/* 8 */
/* 934 */	
			0x4b,		/* FC_PP */
			0x5c,		/* FC_PAD */
/* 936 */	
			0x46,		/* FC_NO_REPEAT */
			0x5c,		/* FC_PAD */
/* 938 */	NdrFcShort( 0x4 ),	/* 4 */
/* 940 */	NdrFcShort( 0x4 ),	/* 4 */
/* 942 */	0x12, 0x0,	/* FC_UP */
/* 944 */	NdrFcShort( 0xffffffe6 ),	/* Offset= -26 (918) */
/* 946 */	
			0x5b,		/* FC_END */

			0x8,		/* FC_LONG */
/* 948 */	0x8,		/* FC_LONG */
			0x5b,		/* FC_END */
/* 950 */	
			0x15,		/* FC_STRUCT */
			0x3,		/* 3 */
/* 952 */	NdrFcShort( 0x8 ),	/* 8 */
/* 954 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 956 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 958 */	
			0x1b,		/* FC_CARRAY */
			0x3,		/* 3 */
/* 960 */	NdrFcShort( 0x8 ),	/* 8 */
/* 962 */	0x7,		/* Corr desc: FC_USHORT */
			0x0,		/*  */
/* 964 */	NdrFcShort( 0xffd8 ),	/* -40 */
/* 966 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 968 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 970 */	NdrFcShort( 0xffffffec ),	/* Offset= -20 (950) */
/* 972 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 974 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 976 */	NdrFcShort( 0x28 ),	/* 40 */
/* 978 */	NdrFcShort( 0xffffffec ),	/* Offset= -20 (958) */
/* 980 */	NdrFcShort( 0x0 ),	/* Offset= 0 (980) */
/* 982 */	0x6,		/* FC_SHORT */
			0x6,		/* FC_SHORT */
/* 984 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 986 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 988 */	NdrFcShort( 0xfffffdf0 ),	/* Offset= -528 (460) */
/* 990 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 992 */	
			0x12, 0x0,	/* FC_UP */
/* 994 */	NdrFcShort( 0xfffffefc ),	/* Offset= -260 (734) */
/* 996 */	
			0x12, 0x8,	/* FC_UP [simple_pointer] */
/* 998 */	0x1,		/* FC_BYTE */
			0x5c,		/* FC_PAD */
/* 1000 */	
			0x12, 0x8,	/* FC_UP [simple_pointer] */
/* 1002 */	0x6,		/* FC_SHORT */
			0x5c,		/* FC_PAD */
/* 1004 */	
			0x12, 0x8,	/* FC_UP [simple_pointer] */
/* 1006 */	0x8,		/* FC_LONG */
			0x5c,		/* FC_PAD */
/* 1008 */	
			0x12, 0x8,	/* FC_UP [simple_pointer] */
/* 1010 */	0xa,		/* FC_FLOAT */
			0x5c,		/* FC_PAD */
/* 1012 */	
			0x12, 0x8,	/* FC_UP [simple_pointer] */
/* 1014 */	0xc,		/* FC_DOUBLE */
			0x5c,		/* FC_PAD */
/* 1016 */	
			0x12, 0x0,	/* FC_UP */
/* 1018 */	NdrFcShort( 0xfffffd86 ),	/* Offset= -634 (384) */
/* 1020 */	
			0x12, 0x10,	/* FC_UP [pointer_deref] */
/* 1022 */	NdrFcShort( 0xfffffd88 ),	/* Offset= -632 (390) */
/* 1024 */	
			0x12, 0x10,	/* FC_UP [pointer_deref] */
/* 1026 */	NdrFcShort( 0xfffffd9e ),	/* Offset= -610 (416) */
/* 1028 */	
			0x12, 0x10,	/* FC_UP [pointer_deref] */
/* 1030 */	NdrFcShort( 0xfffffdac ),	/* Offset= -596 (434) */
/* 1032 */	
			0x12, 0x10,	/* FC_UP [pointer_deref] */
/* 1034 */	NdrFcShort( 0xfffffdba ),	/* Offset= -582 (452) */
/* 1036 */	
			0x12, 0x10,	/* FC_UP [pointer_deref] */
/* 1038 */	NdrFcShort( 0x2 ),	/* Offset= 2 (1040) */
/* 1040 */	
			0x12, 0x0,	/* FC_UP */
/* 1042 */	NdrFcShort( 0x14 ),	/* Offset= 20 (1062) */
/* 1044 */	
			0x15,		/* FC_STRUCT */
			0x7,		/* 7 */
/* 1046 */	NdrFcShort( 0x10 ),	/* 16 */
/* 1048 */	0x6,		/* FC_SHORT */
			0x1,		/* FC_BYTE */
/* 1050 */	0x1,		/* FC_BYTE */
			0x8,		/* FC_LONG */
/* 1052 */	0xb,		/* FC_HYPER */
			0x5b,		/* FC_END */
/* 1054 */	
			0x12, 0x0,	/* FC_UP */
/* 1056 */	NdrFcShort( 0xfffffff4 ),	/* Offset= -12 (1044) */
/* 1058 */	
			0x12, 0x8,	/* FC_UP [simple_pointer] */
/* 1060 */	0x2,		/* FC_CHAR */
			0x5c,		/* FC_PAD */
/* 1062 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x7,		/* 7 */
/* 1064 */	NdrFcShort( 0x20 ),	/* 32 */
/* 1066 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1068 */	NdrFcShort( 0x0 ),	/* Offset= 0 (1068) */
/* 1070 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 1072 */	0x6,		/* FC_SHORT */
			0x6,		/* FC_SHORT */
/* 1074 */	0x6,		/* FC_SHORT */
			0x6,		/* FC_SHORT */
/* 1076 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 1078 */	NdrFcShort( 0xfffffc38 ),	/* Offset= -968 (110) */
/* 1080 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 1082 */	0xb4,		/* FC_USER_MARSHAL */
			0x83,		/* 131 */
/* 1084 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1086 */	NdrFcShort( 0x10 ),	/* 16 */
/* 1088 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1090 */	NdrFcShort( 0xfffffc28 ),	/* Offset= -984 (106) */
/* 1092 */	
			0x21,		/* FC_BOGUS_ARRAY */
			0x3,		/* 3 */
/* 1094 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1096 */	0x19,		/* Corr desc:  field pointer, FC_ULONG */
			0x0,		/*  */
/* 1098 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1100 */	NdrFcShort( 0x0 ),	/* Corr flags:  */
/* 1102 */	NdrFcLong( 0xffffffff ),	/* -1 */
/* 1106 */	NdrFcShort( 0x0 ),	/* Corr flags:  */
/* 1108 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 1110 */	NdrFcShort( 0xffffffe4 ),	/* Offset= -28 (1082) */
/* 1112 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 1114 */	
			0x1b,		/* FC_CARRAY */
			0x3,		/* 3 */
/* 1116 */	NdrFcShort( 0x4 ),	/* 4 */
/* 1118 */	0x19,		/* Corr desc:  field pointer, FC_ULONG */
			0x0,		/*  */
/* 1120 */	NdrFcShort( 0xc ),	/* 12 */
/* 1122 */	NdrFcShort( 0x0 ),	/* Corr flags:  */
/* 1124 */	0x8,		/* FC_LONG */
			0x5b,		/* FC_END */
/* 1126 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 1128 */	NdrFcShort( 0x10 ),	/* 16 */
/* 1130 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1132 */	NdrFcShort( 0x8 ),	/* Offset= 8 (1140) */
/* 1134 */	0x36,		/* FC_POINTER */
			0x36,		/* FC_POINTER */
/* 1136 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 1138 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 1140 */	
			0x12, 0x0,	/* FC_UP */
/* 1142 */	NdrFcShort( 0xffffffce ),	/* Offset= -50 (1092) */
/* 1144 */	
			0x12, 0x0,	/* FC_UP */
/* 1146 */	NdrFcShort( 0xffffffe0 ),	/* Offset= -32 (1114) */
/* 1148 */	
			0x11, 0x4,	/* FC_RP [alloced_on_stack] */
/* 1150 */	NdrFcShort( 0x6 ),	/* Offset= 6 (1156) */
/* 1152 */	
			0x13, 0x0,	/* FC_OP */
/* 1154 */	NdrFcShort( 0xffffffa4 ),	/* Offset= -92 (1062) */
/* 1156 */	0xb4,		/* FC_USER_MARSHAL */
			0x83,		/* 131 */
/* 1158 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1160 */	NdrFcShort( 0x10 ),	/* 16 */
/* 1162 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1164 */	NdrFcShort( 0xfffffff4 ),	/* Offset= -12 (1152) */
/* 1166 */	
			0x11, 0x4,	/* FC_RP [alloced_on_stack] */
/* 1168 */	NdrFcShort( 0x10 ),	/* Offset= 16 (1184) */
/* 1170 */	
			0x13, 0x0,	/* FC_OP */
/* 1172 */	NdrFcShort( 0xfffffd02 ),	/* Offset= -766 (406) */
/* 1174 */	0xb4,		/* FC_USER_MARSHAL */
			0x83,		/* 131 */
/* 1176 */	NdrFcShort( 0x1 ),	/* 1 */
/* 1178 */	NdrFcShort( 0x4 ),	/* 4 */
/* 1180 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1182 */	NdrFcShort( 0xfffffff4 ),	/* Offset= -12 (1170) */
/* 1184 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 1186 */	NdrFcShort( 0x20 ),	/* 32 */
/* 1188 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1190 */	NdrFcShort( 0x0 ),	/* Offset= 0 (1190) */
/* 1192 */	0x6,		/* FC_SHORT */
			0x6,		/* FC_SHORT */
/* 1194 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 1196 */	NdrFcShort( 0xffffffea ),	/* Offset= -22 (1174) */
/* 1198 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 1200 */	NdrFcShort( 0xffffffe6 ),	/* Offset= -26 (1174) */
/* 1202 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 1204 */	NdrFcShort( 0xffffffe2 ),	/* Offset= -30 (1174) */
/* 1206 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 1208 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 1210 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 1212 */	
			0x11, 0x0,	/* FC_RP */
/* 1214 */	NdrFcShort( 0x2 ),	/* Offset= 2 (1216) */
/* 1216 */	
			0x1b,		/* FC_CARRAY */
			0x3,		/* 3 */
/* 1218 */	NdrFcShort( 0x4 ),	/* 4 */
/* 1220 */	0x29,		/* Corr desc:  parameter, FC_ULONG */
			0x0,		/*  */
/* 1222 */	NdrFcShort( 0x24 ),	/* x86 Stack size/offset = 36 */
/* 1224 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 1226 */	0x8,		/* FC_LONG */
			0x5b,		/* FC_END */
/* 1228 */	
			0x11, 0x0,	/* FC_RP */
/* 1230 */	NdrFcShort( 0x2 ),	/* Offset= 2 (1232) */
/* 1232 */	
			0x21,		/* FC_BOGUS_ARRAY */
			0x3,		/* 3 */
/* 1234 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1236 */	0x29,		/* Corr desc:  parameter, FC_ULONG */
			0x0,		/*  */
/* 1238 */	NdrFcShort( 0x24 ),	/* x86 Stack size/offset = 36 */
/* 1240 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 1242 */	NdrFcLong( 0xffffffff ),	/* -1 */
/* 1246 */	NdrFcShort( 0x0 ),	/* Corr flags:  */
/* 1248 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 1250 */	NdrFcShort( 0xffffffa2 ),	/* Offset= -94 (1156) */
/* 1252 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 1254 */	
			0x11, 0x0,	/* FC_RP */
/* 1256 */	NdrFcShort( 0x2 ),	/* Offset= 2 (1258) */
/* 1258 */	
			0x21,		/* FC_BOGUS_ARRAY */
			0x3,		/* 3 */
/* 1260 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1262 */	0x29,		/* Corr desc:  parameter, FC_ULONG */
			0x0,		/*  */
/* 1264 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 1266 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 1268 */	0x29,		/* Corr desc:  parameter, FC_ULONG */
			0x54,		/* FC_DEREFERENCE */
/* 1270 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 1272 */	NdrFcShort( 0x0 ),	/* Corr flags:  */
/* 1274 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 1276 */	NdrFcShort( 0xffffff88 ),	/* Offset= -120 (1156) */
/* 1278 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 1280 */	
			0x11, 0x10,	/* FC_RP [pointer_deref] */
/* 1282 */	NdrFcShort( 0x2 ),	/* Offset= 2 (1284) */
/* 1284 */	
			0x2f,		/* FC_IP */
			0x5a,		/* FC_CONSTANT_IID */
/* 1286 */	NdrFcLong( 0x20404 ),	/* 132100 */
/* 1290 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1292 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1294 */	0xc0,		/* 192 */
			0x0,		/* 0 */
/* 1296 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 1298 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 1300 */	0x0,		/* 0 */
			0x46,		/* 70 */
/* 1302 */	
			0x11, 0x8,	/* FC_RP [simple_pointer] */
/* 1304 */	
			0x25,		/* FC_C_WSTRING */
			0x5c,		/* FC_PAD */
/* 1306 */	
			0x11, 0xc,	/* FC_RP [alloced_on_stack] [simple_pointer] */
/* 1308 */	0xe,		/* FC_ENUM32 */
			0x5c,		/* FC_PAD */
/* 1310 */	
			0x11, 0x14,	/* FC_RP [alloced_on_stack] [pointer_deref] */
/* 1312 */	NdrFcShort( 0x2 ),	/* Offset= 2 (1314) */
/* 1314 */	
			0x13, 0x0,	/* FC_OP */
/* 1316 */	NdrFcShort( 0xb6 ),	/* Offset= 182 (1498) */
/* 1318 */	
			0x2b,		/* FC_NON_ENCAPSULATED_UNION */
			0x7,		/* FC_USHORT */
/* 1320 */	0x7,		/* Corr desc: FC_USHORT */
			0x0,		/*  */
/* 1322 */	NdrFcShort( 0x4 ),	/* 4 */
/* 1324 */	NdrFcShort( 0x0 ),	/* Corr flags:  */
/* 1326 */	NdrFcShort( 0x2 ),	/* Offset= 2 (1328) */
/* 1328 */	NdrFcShort( 0x4 ),	/* 4 */
/* 1330 */	NdrFcShort( 0x4 ),	/* 4 */
/* 1332 */	NdrFcLong( 0x1a ),	/* 26 */
/* 1336 */	NdrFcShort( 0x16 ),	/* Offset= 22 (1358) */
/* 1338 */	NdrFcLong( 0x1b ),	/* 27 */
/* 1342 */	NdrFcShort( 0x10 ),	/* Offset= 16 (1358) */
/* 1344 */	NdrFcLong( 0x1c ),	/* 28 */
/* 1348 */	NdrFcShort( 0xe ),	/* Offset= 14 (1362) */
/* 1350 */	NdrFcLong( 0x1d ),	/* 29 */
/* 1354 */	NdrFcShort( 0x8008 ),	/* Simple arm type: FC_LONG */
/* 1356 */	NdrFcShort( 0x0 ),	/* Offset= 0 (1356) */
/* 1358 */	
			0x13, 0x0,	/* FC_OP */
/* 1360 */	NdrFcShort( 0x26 ),	/* Offset= 38 (1398) */
/* 1362 */	
			0x13, 0x0,	/* FC_OP */
/* 1364 */	NdrFcShort( 0x12 ),	/* Offset= 18 (1382) */
/* 1366 */	
			0x1b,		/* FC_CARRAY */
			0x3,		/* 3 */
/* 1368 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1370 */	0x7,		/* Corr desc: FC_USHORT */
			0x0,		/*  */
/* 1372 */	NdrFcShort( 0xfffc ),	/* -4 */
/* 1374 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 1376 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 1378 */	NdrFcShort( 0xfffffe54 ),	/* Offset= -428 (950) */
/* 1380 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 1382 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 1384 */	NdrFcShort( 0x4 ),	/* 4 */
/* 1386 */	NdrFcShort( 0xffffffec ),	/* Offset= -20 (1366) */
/* 1388 */	NdrFcShort( 0x0 ),	/* Offset= 0 (1388) */
/* 1390 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 1392 */	NdrFcShort( 0x6 ),	/* Offset= 6 (1398) */
/* 1394 */	0x6,		/* FC_SHORT */
			0x3e,		/* FC_STRUCTPAD2 */
/* 1396 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 1398 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 1400 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1402 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1404 */	NdrFcShort( 0x0 ),	/* Offset= 0 (1404) */
/* 1406 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 1408 */	NdrFcShort( 0xffffffa6 ),	/* Offset= -90 (1318) */
/* 1410 */	0x6,		/* FC_SHORT */
			0x3e,		/* FC_STRUCTPAD2 */
/* 1412 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 1414 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 1416 */	NdrFcShort( 0x18 ),	/* 24 */
/* 1418 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1420 */	NdrFcShort( 0x0 ),	/* Offset= 0 (1420) */
/* 1422 */	0x8,		/* FC_LONG */
			0x40,		/* FC_STRUCTPAD4 */
/* 1424 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 1426 */	NdrFcShort( 0xfffffef2 ),	/* Offset= -270 (1156) */
/* 1428 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 1430 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 1432 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1434 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1436 */	NdrFcShort( 0x6 ),	/* Offset= 6 (1442) */
/* 1438 */	0x36,		/* FC_POINTER */
			0x6,		/* FC_SHORT */
/* 1440 */	0x3e,		/* FC_STRUCTPAD2 */
			0x5b,		/* FC_END */
/* 1442 */	
			0x13, 0x0,	/* FC_OP */
/* 1444 */	NdrFcShort( 0xffffffe2 ),	/* Offset= -30 (1414) */
/* 1446 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 1448 */	NdrFcShort( 0x10 ),	/* 16 */
/* 1450 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1452 */	NdrFcShort( 0x0 ),	/* Offset= 0 (1452) */
/* 1454 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 1456 */	NdrFcShort( 0xffffffc6 ),	/* Offset= -58 (1398) */
/* 1458 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 1460 */	NdrFcShort( 0xffffffe2 ),	/* Offset= -30 (1430) */
/* 1462 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 1464 */	
			0x1b,		/* FC_CARRAY */
			0x3,		/* 3 */
/* 1466 */	NdrFcShort( 0x4 ),	/* 4 */
/* 1468 */	0x16,		/* Corr desc:  field pointer, FC_SHORT */
			0x0,		/*  */
/* 1470 */	NdrFcShort( 0x1e ),	/* 30 */
/* 1472 */	NdrFcShort( 0x0 ),	/* Corr flags:  */
/* 1474 */	0x8,		/* FC_LONG */
			0x5b,		/* FC_END */
/* 1476 */	
			0x21,		/* FC_BOGUS_ARRAY */
			0x3,		/* 3 */
/* 1478 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1480 */	0x16,		/* Corr desc:  field pointer, FC_SHORT */
			0x0,		/*  */
/* 1482 */	NdrFcShort( 0x18 ),	/* 24 */
/* 1484 */	NdrFcShort( 0x0 ),	/* Corr flags:  */
/* 1486 */	NdrFcLong( 0xffffffff ),	/* -1 */
/* 1490 */	NdrFcShort( 0x0 ),	/* Corr flags:  */
/* 1492 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 1494 */	NdrFcShort( 0xffffffd0 ),	/* Offset= -48 (1446) */
/* 1496 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 1498 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 1500 */	NdrFcShort( 0x34 ),	/* 52 */
/* 1502 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1504 */	NdrFcShort( 0x14 ),	/* Offset= 20 (1524) */
/* 1506 */	0x8,		/* FC_LONG */
			0x36,		/* FC_POINTER */
/* 1508 */	0x36,		/* FC_POINTER */
			0xe,		/* FC_ENUM32 */
/* 1510 */	0xe,		/* FC_ENUM32 */
			0xe,		/* FC_ENUM32 */
/* 1512 */	0x6,		/* FC_SHORT */
			0x6,		/* FC_SHORT */
/* 1514 */	0x6,		/* FC_SHORT */
			0x6,		/* FC_SHORT */
/* 1516 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 1518 */	NdrFcShort( 0xffffffb8 ),	/* Offset= -72 (1446) */
/* 1520 */	0x6,		/* FC_SHORT */
			0x3e,		/* FC_STRUCTPAD2 */
/* 1522 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 1524 */	
			0x13, 0x0,	/* FC_OP */
/* 1526 */	NdrFcShort( 0xffffffc2 ),	/* Offset= -62 (1464) */
/* 1528 */	
			0x13, 0x0,	/* FC_OP */
/* 1530 */	NdrFcShort( 0xffffffca ),	/* Offset= -54 (1476) */
/* 1532 */	
			0x11, 0x14,	/* FC_RP [alloced_on_stack] [pointer_deref] */
/* 1534 */	NdrFcShort( 0x2 ),	/* Offset= 2 (1536) */
/* 1536 */	
			0x13, 0x0,	/* FC_OP */
/* 1538 */	NdrFcShort( 0x2e ),	/* Offset= 46 (1584) */
/* 1540 */	
			0x2b,		/* FC_NON_ENCAPSULATED_UNION */
			0x8,		/* FC_LONG */
/* 1542 */	0x8,		/* Corr desc: FC_LONG */
			0x0,		/*  */
/* 1544 */	NdrFcShort( 0x18 ),	/* 24 */
/* 1546 */	NdrFcShort( 0x0 ),	/* Corr flags:  */
/* 1548 */	NdrFcShort( 0x2 ),	/* Offset= 2 (1550) */
/* 1550 */	NdrFcShort( 0x4 ),	/* 4 */
/* 1552 */	NdrFcShort( 0x4 ),	/* 4 */
/* 1554 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1558 */	NdrFcShort( 0x8008 ),	/* Simple arm type: FC_LONG */
/* 1560 */	NdrFcLong( 0x3 ),	/* 3 */
/* 1564 */	NdrFcShort( 0x8008 ),	/* Simple arm type: FC_LONG */
/* 1566 */	NdrFcLong( 0x1 ),	/* 1 */
/* 1570 */	NdrFcShort( 0x8008 ),	/* Simple arm type: FC_LONG */
/* 1572 */	NdrFcLong( 0x2 ),	/* 2 */
/* 1576 */	NdrFcShort( 0x4 ),	/* Offset= 4 (1580) */
/* 1578 */	NdrFcShort( 0xffffffff ),	/* Offset= -1 (1577) */
/* 1580 */	
			0x13, 0x0,	/* FC_OP */
/* 1582 */	NdrFcShort( 0xfffffe56 ),	/* Offset= -426 (1156) */
/* 1584 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 1586 */	NdrFcShort( 0x24 ),	/* 36 */
/* 1588 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1590 */	NdrFcShort( 0x10 ),	/* Offset= 16 (1606) */
/* 1592 */	0x8,		/* FC_LONG */
			0x36,		/* FC_POINTER */
/* 1594 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 1596 */	NdrFcShort( 0xffffffc8 ),	/* Offset= -56 (1540) */
/* 1598 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 1600 */	NdrFcShort( 0xffffff66 ),	/* Offset= -154 (1446) */
/* 1602 */	0x6,		/* FC_SHORT */
			0x3e,		/* FC_STRUCTPAD2 */
/* 1604 */	0xe,		/* FC_ENUM32 */
			0x5b,		/* FC_END */
/* 1606 */	
			0x13, 0x8,	/* FC_OP [simple_pointer] */
/* 1608 */	
			0x25,		/* FC_C_WSTRING */
			0x5c,		/* FC_PAD */
/* 1610 */	
			0x11, 0x10,	/* FC_RP [pointer_deref] */
/* 1612 */	NdrFcShort( 0x2 ),	/* Offset= 2 (1614) */
/* 1614 */	
			0x2f,		/* FC_IP */
			0x5a,		/* FC_CONSTANT_IID */
/* 1616 */	NdrFcLong( 0x20403 ),	/* 132099 */
/* 1620 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1622 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1624 */	0xc0,		/* 192 */
			0x0,		/* 0 */
/* 1626 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 1628 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 1630 */	0x0,		/* 0 */
			0x46,		/* 70 */
/* 1632 */	
			0x11, 0x4,	/* FC_RP [alloced_on_stack] */
/* 1634 */	NdrFcShort( 0x4 ),	/* Offset= 4 (1638) */
/* 1636 */	0x8,		/* FC_LONG */
			0x5c,		/* FC_PAD */
/* 1638 */	0xb4,		/* FC_USER_MARSHAL */
			0x3,		/* 3 */
/* 1640 */	NdrFcShort( 0x2 ),	/* 2 */
/* 1642 */	NdrFcShort( 0xc ),	/* 12 */
/* 1644 */	NdrFcShort( 0x4 ),	/* 4 */
/* 1646 */	NdrFcShort( 0xfffffff6 ),	/* Offset= -10 (1636) */
/* 1648 */	
			0x11, 0x14,	/* FC_RP [alloced_on_stack] [pointer_deref] */
/* 1650 */	NdrFcShort( 0x2 ),	/* Offset= 2 (1652) */
/* 1652 */	
			0x13, 0x0,	/* FC_OP */
/* 1654 */	NdrFcShort( 0xe ),	/* Offset= 14 (1668) */
/* 1656 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 1658 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1660 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1662 */	NdrFcShort( 0x0 ),	/* Offset= 0 (1662) */
/* 1664 */	0x8,		/* FC_LONG */
			0x6,		/* FC_SHORT */
/* 1666 */	0x3e,		/* FC_STRUCTPAD2 */
			0x5b,		/* FC_END */
/* 1668 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 1670 */	NdrFcShort( 0x4c ),	/* 76 */
/* 1672 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1674 */	NdrFcShort( 0x1e ),	/* Offset= 30 (1704) */
/* 1676 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 1678 */	NdrFcShort( 0xfffff998 ),	/* Offset= -1640 (38) */
/* 1680 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 1682 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 1684 */	0x36,		/* FC_POINTER */
			0x8,		/* FC_LONG */
/* 1686 */	0xe,		/* FC_ENUM32 */
			0x6,		/* FC_SHORT */
/* 1688 */	0x6,		/* FC_SHORT */
			0x6,		/* FC_SHORT */
/* 1690 */	0x6,		/* FC_SHORT */
			0x6,		/* FC_SHORT */
/* 1692 */	0x6,		/* FC_SHORT */
			0x6,		/* FC_SHORT */
/* 1694 */	0x6,		/* FC_SHORT */
			0x4c,		/* FC_EMBEDDED_COMPLEX */
/* 1696 */	0x0,		/* 0 */
			NdrFcShort( 0xfffffed5 ),	/* Offset= -299 (1398) */
			0x4c,		/* FC_EMBEDDED_COMPLEX */
/* 1700 */	0x0,		/* 0 */
			NdrFcShort( 0xffffffd3 ),	/* Offset= -45 (1656) */
			0x5b,		/* FC_END */
/* 1704 */	
			0x13, 0x8,	/* FC_OP [simple_pointer] */
/* 1706 */	
			0x25,		/* FC_C_WSTRING */
			0x5c,		/* FC_PAD */
/* 1708 */	
			0x11, 0x4,	/* FC_RP [alloced_on_stack] */
/* 1710 */	NdrFcShort( 0x4 ),	/* Offset= 4 (1714) */
/* 1712 */	0x8,		/* FC_LONG */
			0x5c,		/* FC_PAD */
/* 1714 */	0xb4,		/* FC_USER_MARSHAL */
			0x3,		/* 3 */
/* 1716 */	NdrFcShort( 0x2 ),	/* 2 */
/* 1718 */	NdrFcShort( 0xc ),	/* 12 */
/* 1720 */	NdrFcShort( 0x4 ),	/* 4 */
/* 1722 */	NdrFcShort( 0xfffffff6 ),	/* Offset= -10 (1712) */
/* 1724 */	
			0x11, 0x4,	/* FC_RP [alloced_on_stack] */
/* 1726 */	NdrFcShort( 0x4 ),	/* Offset= 4 (1730) */
/* 1728 */	0x8,		/* FC_LONG */
			0x5c,		/* FC_PAD */
/* 1730 */	0xb4,		/* FC_USER_MARSHAL */
			0x3,		/* 3 */
/* 1732 */	NdrFcShort( 0x2 ),	/* 2 */
/* 1734 */	NdrFcShort( 0xc ),	/* 12 */
/* 1736 */	NdrFcShort( 0x4 ),	/* 4 */
/* 1738 */	NdrFcShort( 0xfffffff6 ),	/* Offset= -10 (1728) */
/* 1740 */	
			0x11, 0x4,	/* FC_RP [alloced_on_stack] */
/* 1742 */	NdrFcShort( 0x4 ),	/* Offset= 4 (1746) */
/* 1744 */	0x8,		/* FC_LONG */
			0x5c,		/* FC_PAD */
/* 1746 */	0xb4,		/* FC_USER_MARSHAL */
			0x3,		/* 3 */
/* 1748 */	NdrFcShort( 0x2 ),	/* 2 */
/* 1750 */	NdrFcShort( 0xc ),	/* 12 */
/* 1752 */	NdrFcShort( 0x4 ),	/* 4 */
/* 1754 */	NdrFcShort( 0xfffffff6 ),	/* Offset= -10 (1744) */
/* 1756 */	
			0x11, 0x0,	/* FC_RP */
/* 1758 */	NdrFcShort( 0x2 ),	/* Offset= 2 (1760) */
/* 1760 */	
			0x21,		/* FC_BOGUS_ARRAY */
			0x3,		/* 3 */
/* 1762 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1764 */	0x29,		/* Corr desc:  parameter, FC_ULONG */
			0x0,		/*  */
/* 1766 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 1768 */	NdrFcShort( 0x0 ),	/* Corr flags:  */
/* 1770 */	0x29,		/* Corr desc:  parameter, FC_ULONG */
			0x54,		/* FC_DEREFERENCE */
/* 1772 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 1774 */	NdrFcShort( 0x0 ),	/* Corr flags:  */
/* 1776 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 1778 */	NdrFcShort( 0xfffffda4 ),	/* Offset= -604 (1174) */
/* 1780 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 1782 */	
			0x11, 0x4,	/* FC_RP [alloced_on_stack] */
/* 1784 */	NdrFcShort( 0xfffffd9e ),	/* Offset= -610 (1174) */
/* 1786 */	
			0x11, 0xc,	/* FC_RP [alloced_on_stack] [simple_pointer] */
/* 1788 */	0x6,		/* FC_SHORT */
			0x5c,		/* FC_PAD */
/* 1790 */	
			0x11, 0x10,	/* FC_RP [pointer_deref] */
/* 1792 */	NdrFcShort( 0x2 ),	/* Offset= 2 (1794) */
/* 1794 */	
			0x2f,		/* FC_IP */
			0x5c,		/* FC_PAD */
/* 1796 */	0x28,		/* Corr desc:  parameter, FC_LONG */
			0x0,		/*  */
/* 1798 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 1800 */	NdrFcShort( 0x5 ),	/* Corr flags:  early, iid_is, */
/* 1802 */	
			0x11, 0x10,	/* FC_RP [pointer_deref] */
/* 1804 */	NdrFcShort( 0x2 ),	/* Offset= 2 (1806) */
/* 1806 */	
			0x2f,		/* FC_IP */
			0x5a,		/* FC_CONSTANT_IID */
/* 1808 */	NdrFcLong( 0x20402 ),	/* 132098 */
/* 1812 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1814 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1816 */	0xc0,		/* 192 */
			0x0,		/* 0 */
/* 1818 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 1820 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 1822 */	0x0,		/* 0 */
			0x46,		/* 70 */
/* 1824 */	
			0x11, 0x4,	/* FC_RP [alloced_on_stack] */
/* 1826 */	NdrFcShort( 0x2a ),	/* Offset= 42 (1868) */
/* 1828 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 1830 */	NdrFcShort( 0x20 ),	/* 32 */
/* 1832 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1834 */	NdrFcShort( 0x0 ),	/* Offset= 0 (1834) */
/* 1836 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 1838 */	NdrFcShort( 0xfffff8f8 ),	/* Offset= -1800 (38) */
/* 1840 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 1842 */	NdrFcShort( 0xfffffd52 ),	/* Offset= -686 (1156) */
/* 1844 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 1846 */	
			0x21,		/* FC_BOGUS_ARRAY */
			0x3,		/* 3 */
/* 1848 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1850 */	0x19,		/* Corr desc:  field pointer, FC_ULONG */
			0x0,		/*  */
/* 1852 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1854 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 1856 */	NdrFcLong( 0xffffffff ),	/* -1 */
/* 1860 */	NdrFcShort( 0x0 ),	/* Corr flags:  */
/* 1862 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 1864 */	NdrFcShort( 0xffffffdc ),	/* Offset= -36 (1828) */
/* 1866 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 1868 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 1870 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1872 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1874 */	NdrFcShort( 0x6 ),	/* Offset= 6 (1880) */
/* 1876 */	0x8,		/* FC_LONG */
			0x36,		/* FC_POINTER */
/* 1878 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 1880 */	
			0x13, 0x0,	/* FC_OP */
/* 1882 */	NdrFcShort( 0xffffffdc ),	/* Offset= -36 (1846) */
/* 1884 */	
			0x11, 0x14,	/* FC_RP [alloced_on_stack] [pointer_deref] */
/* 1886 */	NdrFcShort( 0x2 ),	/* Offset= 2 (1888) */
/* 1888 */	
			0x13, 0x0,	/* FC_OP */
/* 1890 */	NdrFcShort( 0x2 ),	/* Offset= 2 (1892) */
/* 1892 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 1894 */	NdrFcShort( 0x20 ),	/* 32 */
/* 1896 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1898 */	NdrFcShort( 0x0 ),	/* Offset= 0 (1898) */
/* 1900 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 1902 */	NdrFcShort( 0xfffff8b8 ),	/* Offset= -1864 (38) */
/* 1904 */	0x8,		/* FC_LONG */
			0xe,		/* FC_ENUM32 */
/* 1906 */	0x6,		/* FC_SHORT */
			0x6,		/* FC_SHORT */
/* 1908 */	0x6,		/* FC_SHORT */
			0x3e,		/* FC_STRUCTPAD2 */
/* 1910 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 1912 */	
			0x11, 0x4,	/* FC_RP [alloced_on_stack] */
/* 1914 */	NdrFcShort( 0x4 ),	/* Offset= 4 (1918) */
/* 1916 */	0x8,		/* FC_LONG */
			0x5c,		/* FC_PAD */
/* 1918 */	0xb4,		/* FC_USER_MARSHAL */
			0x3,		/* 3 */
/* 1920 */	NdrFcShort( 0x2 ),	/* 2 */
/* 1922 */	NdrFcShort( 0xc ),	/* 12 */
/* 1924 */	NdrFcShort( 0x4 ),	/* 4 */
/* 1926 */	NdrFcShort( 0xfffffff6 ),	/* Offset= -10 (1916) */
/* 1928 */	
			0x11, 0x0,	/* FC_RP */
/* 1930 */	NdrFcShort( 0x2 ),	/* Offset= 2 (1932) */
/* 1932 */	
			0x21,		/* FC_BOGUS_ARRAY */
			0x3,		/* 3 */
/* 1934 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1936 */	0x27,		/* Corr desc:  parameter, FC_USHORT */
			0x54,		/* FC_DEREFERENCE */
/* 1938 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 1940 */	NdrFcShort( 0x0 ),	/* Corr flags:  */
/* 1942 */	0x27,		/* Corr desc:  parameter, FC_USHORT */
			0x54,		/* FC_DEREFERENCE */
/* 1944 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 1946 */	NdrFcShort( 0x0 ),	/* Corr flags:  */
/* 1948 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 1950 */	NdrFcShort( 0xfffff86c ),	/* Offset= -1940 (10) */
/* 1952 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 1954 */	
			0x11, 0x0,	/* FC_RP */
/* 1956 */	NdrFcShort( 0x2 ),	/* Offset= 2 (1958) */
/* 1958 */	
			0x1c,		/* FC_CVARRAY */
			0x3,		/* 3 */
/* 1960 */	NdrFcShort( 0x4 ),	/* 4 */
/* 1962 */	0x27,		/* Corr desc:  parameter, FC_USHORT */
			0x54,		/* FC_DEREFERENCE */
/* 1964 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 1966 */	NdrFcShort( 0x0 ),	/* Corr flags:  */
/* 1968 */	0x27,		/* Corr desc:  parameter, FC_USHORT */
			0x54,		/* FC_DEREFERENCE */
/* 1970 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 1972 */	NdrFcShort( 0x0 ),	/* Corr flags:  */
/* 1974 */	0x8,		/* FC_LONG */
			0x5b,		/* FC_END */
/* 1976 */	
			0x11, 0x8,	/* FC_RP [simple_pointer] */
/* 1978 */	0x6,		/* FC_SHORT */
			0x5c,		/* FC_PAD */
/* 1980 */	
			0x11, 0x4,	/* FC_RP [alloced_on_stack] */
/* 1982 */	NdrFcShort( 0xfffff868 ),	/* Offset= -1944 (38) */
/* 1984 */	
			0x11, 0x10,	/* FC_RP [pointer_deref] */
/* 1986 */	NdrFcShort( 0x2 ),	/* Offset= 2 (1988) */
/* 1988 */	
			0x2f,		/* FC_IP */
			0x5c,		/* FC_PAD */
/* 1990 */	0x28,		/* Corr desc:  parameter, FC_LONG */
			0x0,		/*  */
/* 1992 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 1994 */	NdrFcShort( 0x5 ),	/* Corr flags:  early, iid_is, */
/* 1996 */	
			0x11, 0x0,	/* FC_RP */
/* 1998 */	NdrFcShort( 0xfffffcd2 ),	/* Offset= -814 (1184) */
/* 2000 */	
			0x2f,		/* FC_IP */
			0x5a,		/* FC_CONSTANT_IID */
/* 2002 */	NdrFcLong( 0x3127ca40 ),	/* 824691264 */
/* 2006 */	NdrFcShort( 0x446e ),	/* 17518 */
/* 2008 */	NdrFcShort( 0x11ce ),	/* 4558 */
/* 2010 */	0x81,		/* 129 */
			0x35,		/* 53 */
/* 2012 */	0x0,		/* 0 */
			0xaa,		/* 170 */
/* 2014 */	0x0,		/* 0 */
			0x4b,		/* 75 */
/* 2016 */	0xb8,		/* 184 */
			0x51,		/* 81 */
/* 2018 */	0x11, 0x0,	/* FC_RP */
/* 2020 */	NdrFcShort( 0xfffffc56 ),	/* Offset= -938 (1082) */

			0x0
        }
    };

static const USER_MARSHAL_ROUTINE_QUADRUPLE UserMarshalRoutines[ WIRE_MARSHAL_TABLE_SIZE ] = 
        {
            
            {
            VARIANT_UserSize
            ,VARIANT_UserMarshal
            ,VARIANT_UserUnmarshal
            ,VARIANT_UserFree
            },
            {
            BSTR_UserSize
            ,BSTR_UserMarshal
            ,BSTR_UserUnmarshal
            ,BSTR_UserFree
            },
            {
            CLEANLOCALSTORAGE_UserSize
            ,CLEANLOCALSTORAGE_UserMarshal
            ,CLEANLOCALSTORAGE_UserUnmarshal
            ,CLEANLOCALSTORAGE_UserFree
            }

        };



/* Standard interface: __MIDL_itf_oaidl_0000, ver. 0.0,
   GUID={0x00000000,0x0000,0x0000,{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}} */


/* Standard interface: IOleAutomationTypes, ver. 1.0,
   GUID={0x00000000,0x0000,0x0000,{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}} */


/* Object interface: IUnknown, ver. 0.0,
   GUID={0x00000000,0x0000,0x0000,{0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x46}} */


/* Object interface: ICreateTypeInfo, ver. 0.0,
   GUID={0x00020405,0x0000,0x0000,{0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x46}} */


/* Object interface: ICreateTypeInfo2, ver. 0.0,
   GUID={0x0002040E,0x0000,0x0000,{0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x46}} */


/* Object interface: ICreateTypeLib, ver. 0.0,
   GUID={0x00020406,0x0000,0x0000,{0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x46}} */


/* Object interface: ICreateTypeLib2, ver. 0.0,
   GUID={0x0002040F,0x0000,0x0000,{0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x46}} */


/* Object interface: IDispatch, ver. 0.0,
   GUID={0x00020400,0x0000,0x0000,{0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x46}} */

#pragma code_seg(".orpc")
static const unsigned short IDispatch_FormatStringOffsetTable[] =
    {
    0,
    36,
    84,
    144
    };

static const MIDL_STUBLESS_PROXY_INFO IDispatch_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &IDispatch_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const STUB_THUNK IDispatch_StubThunkTable[] = 
    {
    0,
    0,
    0,
    IDispatch_RemoteInvoke_Thunk
    };

static const MIDL_SERVER_INFO IDispatch_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &IDispatch_FormatStringOffsetTable[-3],
    &IDispatch_StubThunkTable[-3],
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(7) _IDispatchProxyVtbl = 
{
    &IDispatch_ProxyInfo,
    &IID_IDispatch,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    (void *) (INT_PTR) -1 /* IDispatch::GetTypeInfoCount */ ,
    (void *) (INT_PTR) -1 /* IDispatch::GetTypeInfo */ ,
    (void *) (INT_PTR) -1 /* IDispatch::GetIDsOfNames */ ,
    IDispatch_Invoke_Proxy
};

const CInterfaceStubVtbl _IDispatchStubVtbl =
{
    &IID_IDispatch,
    &IDispatch_ServerInfo,
    7,
    0, /* pure interpreted */
    CStdStubBuffer_METHODS
};


/* Object interface: IEnumVARIANT, ver. 0.0,
   GUID={0x00020404,0x0000,0x0000,{0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x46}} */

#pragma code_seg(".orpc")
static const unsigned short IEnumVARIANT_FormatStringOffsetTable[] =
    {
    240,
    288,
    324,
    354
    };

static const MIDL_STUBLESS_PROXY_INFO IEnumVARIANT_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &IEnumVARIANT_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const STUB_THUNK IEnumVARIANT_StubThunkTable[] = 
    {
    IEnumVARIANT_RemoteNext_Thunk,
    0,
    0,
    0
    };

static const MIDL_SERVER_INFO IEnumVARIANT_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &IEnumVARIANT_FormatStringOffsetTable[-3],
    &IEnumVARIANT_StubThunkTable[-3],
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(7) _IEnumVARIANTProxyVtbl = 
{
    &IEnumVARIANT_ProxyInfo,
    &IID_IEnumVARIANT,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    IEnumVARIANT_Next_Proxy ,
    (void *) (INT_PTR) -1 /* IEnumVARIANT::Skip */ ,
    (void *) (INT_PTR) -1 /* IEnumVARIANT::Reset */ ,
    (void *) (INT_PTR) -1 /* IEnumVARIANT::Clone */
};

const CInterfaceStubVtbl _IEnumVARIANTStubVtbl =
{
    &IID_IEnumVARIANT,
    &IEnumVARIANT_ServerInfo,
    7,
    0, /* pure interpreted */
    CStdStubBuffer_METHODS
};


/* Object interface: ITypeComp, ver. 0.0,
   GUID={0x00020403,0x0000,0x0000,{0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x46}} */

#pragma code_seg(".orpc")
static const unsigned short ITypeComp_FormatStringOffsetTable[] =
    {
    390,
    474
    };

static const MIDL_STUBLESS_PROXY_INFO ITypeComp_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &ITypeComp_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const STUB_THUNK ITypeComp_StubThunkTable[] = 
    {
    ITypeComp_RemoteBind_Thunk,
    ITypeComp_RemoteBindType_Thunk
    };

static const MIDL_SERVER_INFO ITypeComp_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &ITypeComp_FormatStringOffsetTable[-3],
    &ITypeComp_StubThunkTable[-3],
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(5) _ITypeCompProxyVtbl = 
{
    &ITypeComp_ProxyInfo,
    &IID_ITypeComp,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    ITypeComp_Bind_Proxy ,
    ITypeComp_BindType_Proxy
};

const CInterfaceStubVtbl _ITypeCompStubVtbl =
{
    &IID_ITypeComp,
    &ITypeComp_ServerInfo,
    5,
    0, /* pure interpreted */
    CStdStubBuffer_METHODS
};


/* Object interface: ITypeInfo, ver. 0.0,
   GUID={0x00020401,0x0000,0x0000,{0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x46}} */

#pragma code_seg(".orpc")
static const unsigned short ITypeInfo_FormatStringOffsetTable[] =
    {
    522,
    564,
    600,
    648,
    696,
    750,
    792,
    834,
    864,
    894,
    960,
    1026,
    1068,
    1098,
    1140,
    1182,
    1224,
    1254,
    1284
    };

static const MIDL_STUBLESS_PROXY_INFO ITypeInfo_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &ITypeInfo_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const STUB_THUNK ITypeInfo_StubThunkTable[] = 
    {
    ITypeInfo_RemoteGetTypeAttr_Thunk,
    0,
    ITypeInfo_RemoteGetFuncDesc_Thunk,
    ITypeInfo_RemoteGetVarDesc_Thunk,
    ITypeInfo_RemoteGetNames_Thunk,
    0,
    0,
    ITypeInfo_LocalGetIDsOfNames_Thunk,
    ITypeInfo_LocalInvoke_Thunk,
    ITypeInfo_RemoteGetDocumentation_Thunk,
    ITypeInfo_RemoteGetDllEntry_Thunk,
    0,
    ITypeInfo_LocalAddressOfMember_Thunk,
    ITypeInfo_RemoteCreateInstance_Thunk,
    0,
    ITypeInfo_RemoteGetContainingTypeLib_Thunk,
    ITypeInfo_LocalReleaseTypeAttr_Thunk,
    ITypeInfo_LocalReleaseFuncDesc_Thunk,
    ITypeInfo_LocalReleaseVarDesc_Thunk
    };

static const MIDL_SERVER_INFO ITypeInfo_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &ITypeInfo_FormatStringOffsetTable[-3],
    &ITypeInfo_StubThunkTable[-3],
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(22) _ITypeInfoProxyVtbl = 
{
    &ITypeInfo_ProxyInfo,
    &IID_ITypeInfo,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    ITypeInfo_GetTypeAttr_Proxy ,
    (void *) (INT_PTR) -1 /* ITypeInfo::GetTypeComp */ ,
    ITypeInfo_GetFuncDesc_Proxy ,
    ITypeInfo_GetVarDesc_Proxy ,
    ITypeInfo_GetNames_Proxy ,
    (void *) (INT_PTR) -1 /* ITypeInfo::GetRefTypeOfImplType */ ,
    (void *) (INT_PTR) -1 /* ITypeInfo::GetImplTypeFlags */ ,
    ITypeInfo_GetIDsOfNames_Proxy ,
    ITypeInfo_Invoke_Proxy ,
    ITypeInfo_GetDocumentation_Proxy ,
    ITypeInfo_GetDllEntry_Proxy ,
    (void *) (INT_PTR) -1 /* ITypeInfo::GetRefTypeInfo */ ,
    ITypeInfo_AddressOfMember_Proxy ,
    ITypeInfo_CreateInstance_Proxy ,
    (void *) (INT_PTR) -1 /* ITypeInfo::GetMops */ ,
    ITypeInfo_GetContainingTypeLib_Proxy ,
    ITypeInfo_ReleaseTypeAttr_Proxy ,
    ITypeInfo_ReleaseFuncDesc_Proxy ,
    ITypeInfo_ReleaseVarDesc_Proxy
};

const CInterfaceStubVtbl _ITypeInfoStubVtbl =
{
    &IID_ITypeInfo,
    &ITypeInfo_ServerInfo,
    22,
    0, /* pure interpreted */
    CStdStubBuffer_METHODS
};


/* Object interface: ITypeInfo2, ver. 0.0,
   GUID={0x00020412,0x0000,0x0000,{0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x46}} */

#pragma code_seg(".orpc")
static const unsigned short ITypeInfo2_FormatStringOffsetTable[] =
    {
    522,
    564,
    600,
    648,
    696,
    750,
    792,
    834,
    864,
    894,
    960,
    1026,
    1068,
    1098,
    1140,
    1182,
    1224,
    1254,
    1284,
    1314,
    1350,
    1386,
    1434,
    1476,
    1518,
    1566,
    1620,
    1668,
    1716,
    1782,
    1818,
    1860,
    1908,
    1950
    };

static const MIDL_STUBLESS_PROXY_INFO ITypeInfo2_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &ITypeInfo2_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const STUB_THUNK ITypeInfo2_StubThunkTable[] = 
    {
    ITypeInfo_RemoteGetTypeAttr_Thunk,
    0,
    ITypeInfo_RemoteGetFuncDesc_Thunk,
    ITypeInfo_RemoteGetVarDesc_Thunk,
    ITypeInfo_RemoteGetNames_Thunk,
    0,
    0,
    ITypeInfo_LocalGetIDsOfNames_Thunk,
    ITypeInfo_LocalInvoke_Thunk,
    ITypeInfo_RemoteGetDocumentation_Thunk,
    ITypeInfo_RemoteGetDllEntry_Thunk,
    0,
    ITypeInfo_LocalAddressOfMember_Thunk,
    ITypeInfo_RemoteCreateInstance_Thunk,
    0,
    ITypeInfo_RemoteGetContainingTypeLib_Thunk,
    ITypeInfo_LocalReleaseTypeAttr_Thunk,
    ITypeInfo_LocalReleaseFuncDesc_Thunk,
    ITypeInfo_LocalReleaseVarDesc_Thunk,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    ITypeInfo2_RemoteGetDocumentation2_Thunk,
    0,
    0,
    0,
    0,
    0
    };

static const MIDL_SERVER_INFO ITypeInfo2_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &ITypeInfo2_FormatStringOffsetTable[-3],
    &ITypeInfo2_StubThunkTable[-3],
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(37) _ITypeInfo2ProxyVtbl = 
{
    &ITypeInfo2_ProxyInfo,
    &IID_ITypeInfo2,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    ITypeInfo_GetTypeAttr_Proxy ,
    (void *) (INT_PTR) -1 /* ITypeInfo::GetTypeComp */ ,
    ITypeInfo_GetFuncDesc_Proxy ,
    ITypeInfo_GetVarDesc_Proxy ,
    ITypeInfo_GetNames_Proxy ,
    (void *) (INT_PTR) -1 /* ITypeInfo::GetRefTypeOfImplType */ ,
    (void *) (INT_PTR) -1 /* ITypeInfo::GetImplTypeFlags */ ,
    ITypeInfo_GetIDsOfNames_Proxy ,
    ITypeInfo_Invoke_Proxy ,
    ITypeInfo_GetDocumentation_Proxy ,
    ITypeInfo_GetDllEntry_Proxy ,
    (void *) (INT_PTR) -1 /* ITypeInfo::GetRefTypeInfo */ ,
    ITypeInfo_AddressOfMember_Proxy ,
    ITypeInfo_CreateInstance_Proxy ,
    (void *) (INT_PTR) -1 /* ITypeInfo::GetMops */ ,
    ITypeInfo_GetContainingTypeLib_Proxy ,
    ITypeInfo_ReleaseTypeAttr_Proxy ,
    ITypeInfo_ReleaseFuncDesc_Proxy ,
    ITypeInfo_ReleaseVarDesc_Proxy ,
    (void *) (INT_PTR) -1 /* ITypeInfo2::GetTypeKind */ ,
    (void *) (INT_PTR) -1 /* ITypeInfo2::GetTypeFlags */ ,
    (void *) (INT_PTR) -1 /* ITypeInfo2::GetFuncIndexOfMemId */ ,
    (void *) (INT_PTR) -1 /* ITypeInfo2::GetVarIndexOfMemId */ ,
    (void *) (INT_PTR) -1 /* ITypeInfo2::GetCustData */ ,
    (void *) (INT_PTR) -1 /* ITypeInfo2::GetFuncCustData */ ,
    (void *) (INT_PTR) -1 /* ITypeInfo2::GetParamCustData */ ,
    (void *) (INT_PTR) -1 /* ITypeInfo2::GetVarCustData */ ,
    (void *) (INT_PTR) -1 /* ITypeInfo2::GetImplTypeCustData */ ,
    ITypeInfo2_GetDocumentation2_Proxy ,
    (void *) (INT_PTR) -1 /* ITypeInfo2::GetAllCustData */ ,
    (void *) (INT_PTR) -1 /* ITypeInfo2::GetAllFuncCustData */ ,
    (void *) (INT_PTR) -1 /* ITypeInfo2::GetAllParamCustData */ ,
    (void *) (INT_PTR) -1 /* ITypeInfo2::GetAllVarCustData */ ,
    (void *) (INT_PTR) -1 /* ITypeInfo2::GetAllImplTypeCustData */
};

const CInterfaceStubVtbl _ITypeInfo2StubVtbl =
{
    &IID_ITypeInfo2,
    &ITypeInfo2_ServerInfo,
    37,
    0, /* pure interpreted */
    CStdStubBuffer_METHODS
};


/* Object interface: ITypeLib, ver. 0.0,
   GUID={0x00020402,0x0000,0x0000,{0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x46}} */

#pragma code_seg(".orpc")
static const unsigned short ITypeLib_FormatStringOffsetTable[] =
    {
    0,
    1992,
    2034,
    2076,
    2118,
    2160,
    2196,
    2262,
    2316,
    2382
    };

static const MIDL_STUBLESS_PROXY_INFO ITypeLib_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &ITypeLib_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const STUB_THUNK ITypeLib_StubThunkTable[] = 
    {
    ITypeLib_RemoteGetTypeInfoCount_Thunk,
    0,
    0,
    0,
    ITypeLib_RemoteGetLibAttr_Thunk,
    0,
    ITypeLib_RemoteGetDocumentation_Thunk,
    ITypeLib_RemoteIsName_Thunk,
    ITypeLib_RemoteFindName_Thunk,
    ITypeLib_LocalReleaseTLibAttr_Thunk
    };

static const MIDL_SERVER_INFO ITypeLib_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &ITypeLib_FormatStringOffsetTable[-3],
    &ITypeLib_StubThunkTable[-3],
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(13) _ITypeLibProxyVtbl = 
{
    &ITypeLib_ProxyInfo,
    &IID_ITypeLib,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    ITypeLib_GetTypeInfoCount_Proxy ,
    (void *) (INT_PTR) -1 /* ITypeLib::GetTypeInfo */ ,
    (void *) (INT_PTR) -1 /* ITypeLib::GetTypeInfoType */ ,
    (void *) (INT_PTR) -1 /* ITypeLib::GetTypeInfoOfGuid */ ,
    ITypeLib_GetLibAttr_Proxy ,
    (void *) (INT_PTR) -1 /* ITypeLib::GetTypeComp */ ,
    ITypeLib_GetDocumentation_Proxy ,
    ITypeLib_IsName_Proxy ,
    ITypeLib_FindName_Proxy ,
    ITypeLib_ReleaseTLibAttr_Proxy
};

const CInterfaceStubVtbl _ITypeLibStubVtbl =
{
    &IID_ITypeLib,
    &ITypeLib_ServerInfo,
    13,
    0, /* pure interpreted */
    CStdStubBuffer_METHODS
};


/* Object interface: ITypeLib2, ver. 0.0,
   GUID={0x00020411,0x0000,0x0000,{0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x46}} */

#pragma code_seg(".orpc")
static const unsigned short ITypeLib2_FormatStringOffsetTable[] =
    {
    0,
    1992,
    2034,
    2076,
    2118,
    2160,
    2196,
    2262,
    2316,
    2382,
    2412,
    2454,
    2496,
    2562
    };

static const MIDL_STUBLESS_PROXY_INFO ITypeLib2_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &ITypeLib2_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const STUB_THUNK ITypeLib2_StubThunkTable[] = 
    {
    ITypeLib_RemoteGetTypeInfoCount_Thunk,
    0,
    0,
    0,
    ITypeLib_RemoteGetLibAttr_Thunk,
    0,
    ITypeLib_RemoteGetDocumentation_Thunk,
    ITypeLib_RemoteIsName_Thunk,
    ITypeLib_RemoteFindName_Thunk,
    ITypeLib_LocalReleaseTLibAttr_Thunk,
    0,
    ITypeLib2_RemoteGetLibStatistics_Thunk,
    ITypeLib2_RemoteGetDocumentation2_Thunk,
    0
    };

static const MIDL_SERVER_INFO ITypeLib2_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &ITypeLib2_FormatStringOffsetTable[-3],
    &ITypeLib2_StubThunkTable[-3],
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(17) _ITypeLib2ProxyVtbl = 
{
    &ITypeLib2_ProxyInfo,
    &IID_ITypeLib2,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    ITypeLib_GetTypeInfoCount_Proxy ,
    (void *) (INT_PTR) -1 /* ITypeLib::GetTypeInfo */ ,
    (void *) (INT_PTR) -1 /* ITypeLib::GetTypeInfoType */ ,
    (void *) (INT_PTR) -1 /* ITypeLib::GetTypeInfoOfGuid */ ,
    ITypeLib_GetLibAttr_Proxy ,
    (void *) (INT_PTR) -1 /* ITypeLib::GetTypeComp */ ,
    ITypeLib_GetDocumentation_Proxy ,
    ITypeLib_IsName_Proxy ,
    ITypeLib_FindName_Proxy ,
    ITypeLib_ReleaseTLibAttr_Proxy ,
    (void *) (INT_PTR) -1 /* ITypeLib2::GetCustData */ ,
    ITypeLib2_GetLibStatistics_Proxy ,
    ITypeLib2_GetDocumentation2_Proxy ,
    (void *) (INT_PTR) -1 /* ITypeLib2::GetAllCustData */
};

const CInterfaceStubVtbl _ITypeLib2StubVtbl =
{
    &IID_ITypeLib2,
    &ITypeLib2_ServerInfo,
    17,
    0, /* pure interpreted */
    CStdStubBuffer_METHODS
};


/* Object interface: ITypeChangeEvents, ver. 0.0,
   GUID={0x00020410,0x0000,0x0000,{0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x46}} */


/* Object interface: IErrorInfo, ver. 0.0,
   GUID={0x1CF2B120,0x547D,0x101B,{0x8E,0x65,0x08,0x00,0x2B,0x2B,0xD1,0x19}} */

#pragma code_seg(".orpc")
static const unsigned short IErrorInfo_FormatStringOffsetTable[] =
    {
    2598,
    2634,
    2670,
    2706,
    2742
    };

static const MIDL_STUBLESS_PROXY_INFO IErrorInfo_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &IErrorInfo_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IErrorInfo_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &IErrorInfo_FormatStringOffsetTable[-3],
    0,
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(8) _IErrorInfoProxyVtbl = 
{
    &IErrorInfo_ProxyInfo,
    &IID_IErrorInfo,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    (void *) (INT_PTR) -1 /* IErrorInfo::GetGUID */ ,
    (void *) (INT_PTR) -1 /* IErrorInfo::GetSource */ ,
    (void *) (INT_PTR) -1 /* IErrorInfo::GetDescription */ ,
    (void *) (INT_PTR) -1 /* IErrorInfo::GetHelpFile */ ,
    (void *) (INT_PTR) -1 /* IErrorInfo::GetHelpContext */
};

const CInterfaceStubVtbl _IErrorInfoStubVtbl =
{
    &IID_IErrorInfo,
    &IErrorInfo_ServerInfo,
    8,
    0, /* pure interpreted */
    CStdStubBuffer_METHODS
};


/* Object interface: ICreateErrorInfo, ver. 0.0,
   GUID={0x22F03340,0x547D,0x101B,{0x8E,0x65,0x08,0x00,0x2B,0x2B,0xD1,0x19}} */

#pragma code_seg(".orpc")
static const unsigned short ICreateErrorInfo_FormatStringOffsetTable[] =
    {
    2778,
    2814,
    2850,
    2886,
    2922
    };

static const MIDL_STUBLESS_PROXY_INFO ICreateErrorInfo_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &ICreateErrorInfo_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO ICreateErrorInfo_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &ICreateErrorInfo_FormatStringOffsetTable[-3],
    0,
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(8) _ICreateErrorInfoProxyVtbl = 
{
    &ICreateErrorInfo_ProxyInfo,
    &IID_ICreateErrorInfo,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    (void *) (INT_PTR) -1 /* ICreateErrorInfo::SetGUID */ ,
    (void *) (INT_PTR) -1 /* ICreateErrorInfo::SetSource */ ,
    (void *) (INT_PTR) -1 /* ICreateErrorInfo::SetDescription */ ,
    (void *) (INT_PTR) -1 /* ICreateErrorInfo::SetHelpFile */ ,
    (void *) (INT_PTR) -1 /* ICreateErrorInfo::SetHelpContext */
};

const CInterfaceStubVtbl _ICreateErrorInfoStubVtbl =
{
    &IID_ICreateErrorInfo,
    &ICreateErrorInfo_ServerInfo,
    8,
    0, /* pure interpreted */
    CStdStubBuffer_METHODS
};


/* Object interface: ISupportErrorInfo, ver. 0.0,
   GUID={0xDF0B3D60,0x548F,0x101B,{0x8E,0x65,0x08,0x00,0x2B,0x2B,0xD1,0x19}} */

#pragma code_seg(".orpc")
static const unsigned short ISupportErrorInfo_FormatStringOffsetTable[] =
    {
    2778
    };

static const MIDL_STUBLESS_PROXY_INFO ISupportErrorInfo_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &ISupportErrorInfo_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO ISupportErrorInfo_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &ISupportErrorInfo_FormatStringOffsetTable[-3],
    0,
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(4) _ISupportErrorInfoProxyVtbl = 
{
    &ISupportErrorInfo_ProxyInfo,
    &IID_ISupportErrorInfo,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    (void *) (INT_PTR) -1 /* ISupportErrorInfo::InterfaceSupportsErrorInfo */
};

const CInterfaceStubVtbl _ISupportErrorInfoStubVtbl =
{
    &IID_ISupportErrorInfo,
    &ISupportErrorInfo_ServerInfo,
    4,
    0, /* pure interpreted */
    CStdStubBuffer_METHODS
};


/* Object interface: ITypeFactory, ver. 0.0,
   GUID={0x0000002E,0x0000,0x0000,{0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x46}} */

#pragma code_seg(".orpc")
static const unsigned short ITypeFactory_FormatStringOffsetTable[] =
    {
    2958
    };

static const MIDL_STUBLESS_PROXY_INFO ITypeFactory_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &ITypeFactory_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO ITypeFactory_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &ITypeFactory_FormatStringOffsetTable[-3],
    0,
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(4) _ITypeFactoryProxyVtbl = 
{
    &ITypeFactory_ProxyInfo,
    &IID_ITypeFactory,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    (void *) (INT_PTR) -1 /* ITypeFactory::CreateFromTypeInfo */
};

const CInterfaceStubVtbl _ITypeFactoryStubVtbl =
{
    &IID_ITypeFactory,
    &ITypeFactory_ServerInfo,
    4,
    0, /* pure interpreted */
    CStdStubBuffer_METHODS
};


/* Object interface: ITypeMarshal, ver. 0.0,
   GUID={0x0000002D,0x0000,0x0000,{0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x46}} */


/* Object interface: IRecordInfo, ver. 0.0,
   GUID={0x0000002F,0x0000,0x0000,{0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x46}} */


/* Object interface: IErrorLog, ver. 0.0,
   GUID={0x3127CA40,0x446E,0x11CE,{0x81,0x35,0x00,0xAA,0x00,0x4B,0xB8,0x51}} */

#pragma code_seg(".orpc")
static const unsigned short IErrorLog_FormatStringOffsetTable[] =
    {
    3006
    };

static const MIDL_STUBLESS_PROXY_INFO IErrorLog_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &IErrorLog_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IErrorLog_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &IErrorLog_FormatStringOffsetTable[-3],
    0,
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(4) _IErrorLogProxyVtbl = 
{
    &IErrorLog_ProxyInfo,
    &IID_IErrorLog,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    (void *) (INT_PTR) -1 /* IErrorLog::AddError */
};

const CInterfaceStubVtbl _IErrorLogStubVtbl =
{
    &IID_IErrorLog,
    &IErrorLog_ServerInfo,
    4,
    0, /* pure interpreted */
    CStdStubBuffer_METHODS
};


/* Object interface: IPropertyBag, ver. 0.0,
   GUID={0x55272A00,0x42CB,0x11CE,{0x81,0x35,0x00,0xAA,0x00,0x4B,0xB8,0x51}} */

#pragma code_seg(".orpc")
static const unsigned short IPropertyBag_FormatStringOffsetTable[] =
    {
    3048,
    3108
    };

static const MIDL_STUBLESS_PROXY_INFO IPropertyBag_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &IPropertyBag_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const STUB_THUNK IPropertyBag_StubThunkTable[] = 
    {
    IPropertyBag_RemoteRead_Thunk,
    0
    };

static const MIDL_SERVER_INFO IPropertyBag_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &IPropertyBag_FormatStringOffsetTable[-3],
    &IPropertyBag_StubThunkTable[-3],
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(5) _IPropertyBagProxyVtbl = 
{
    &IPropertyBag_ProxyInfo,
    &IID_IPropertyBag,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    IPropertyBag_Read_Proxy ,
    (void *) (INT_PTR) -1 /* IPropertyBag::Write */
};

const CInterfaceStubVtbl _IPropertyBagStubVtbl =
{
    &IID_IPropertyBag,
    &IPropertyBag_ServerInfo,
    5,
    0, /* pure interpreted */
    CStdStubBuffer_METHODS
};


/* Standard interface: __MIDL_itf_oaidl_0103, ver. 0.0,
   GUID={0x00000000,0x0000,0x0000,{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}} */

static const MIDL_STUB_DESC Object_StubDesc = 
    {
    0,
    NdrOleAllocate,
    NdrOleFree,
    0,
    0,
    0,
    0,
    0,
    __MIDL_TypeFormatString.Format,
    1, /* -error bounds_check flag */
    0x50002, /* Ndr library version */
    0,
    0x600015b, /* MIDL Version 6.0.347 */
    0,
    UserMarshalRoutines,
    0,  /* notify & notify_flag routine table */
    0x1, /* MIDL flag */
    0, /* cs routines */
    0,   /* proxy/server info */
    0   /* Reserved5 */
    };

const CInterfaceProxyVtbl * _oaidl_ProxyVtblList[] = 
{
    ( CInterfaceProxyVtbl *) &_IDispatchProxyVtbl,
    ( CInterfaceProxyVtbl *) &_IPropertyBagProxyVtbl,
    ( CInterfaceProxyVtbl *) &_ITypeInfoProxyVtbl,
    ( CInterfaceProxyVtbl *) &_ITypeLibProxyVtbl,
    ( CInterfaceProxyVtbl *) &_ITypeCompProxyVtbl,
    ( CInterfaceProxyVtbl *) &_IEnumVARIANTProxyVtbl,
    ( CInterfaceProxyVtbl *) &_ITypeLib2ProxyVtbl,
    ( CInterfaceProxyVtbl *) &_ITypeInfo2ProxyVtbl,
    ( CInterfaceProxyVtbl *) &_IErrorInfoProxyVtbl,
    ( CInterfaceProxyVtbl *) &_ITypeFactoryProxyVtbl,
    ( CInterfaceProxyVtbl *) &_ICreateErrorInfoProxyVtbl,
    ( CInterfaceProxyVtbl *) &_IErrorLogProxyVtbl,
    ( CInterfaceProxyVtbl *) &_ISupportErrorInfoProxyVtbl,
    0
};

const CInterfaceStubVtbl * _oaidl_StubVtblList[] = 
{
    ( CInterfaceStubVtbl *) &_IDispatchStubVtbl,
    ( CInterfaceStubVtbl *) &_IPropertyBagStubVtbl,
    ( CInterfaceStubVtbl *) &_ITypeInfoStubVtbl,
    ( CInterfaceStubVtbl *) &_ITypeLibStubVtbl,
    ( CInterfaceStubVtbl *) &_ITypeCompStubVtbl,
    ( CInterfaceStubVtbl *) &_IEnumVARIANTStubVtbl,
    ( CInterfaceStubVtbl *) &_ITypeLib2StubVtbl,
    ( CInterfaceStubVtbl *) &_ITypeInfo2StubVtbl,
    ( CInterfaceStubVtbl *) &_IErrorInfoStubVtbl,
    ( CInterfaceStubVtbl *) &_ITypeFactoryStubVtbl,
    ( CInterfaceStubVtbl *) &_ICreateErrorInfoStubVtbl,
    ( CInterfaceStubVtbl *) &_IErrorLogStubVtbl,
    ( CInterfaceStubVtbl *) &_ISupportErrorInfoStubVtbl,
    0
};

PCInterfaceName const _oaidl_InterfaceNamesList[] = 
{
    "IDispatch",
    "IPropertyBag",
    "ITypeInfo",
    "ITypeLib",
    "ITypeComp",
    "IEnumVARIANT",
    "ITypeLib2",
    "ITypeInfo2",
    "IErrorInfo",
    "ITypeFactory",
    "ICreateErrorInfo",
    "IErrorLog",
    "ISupportErrorInfo",
    0
};


#define _oaidl_CHECK_IID(n)	IID_GENERIC_CHECK_IID( _oaidl, pIID, n)

int __stdcall _oaidl_IID_Lookup( const IID * pIID, int * pIndex )
{
    IID_BS_LOOKUP_SETUP

    IID_BS_LOOKUP_INITIAL_TEST( _oaidl, 13, 8 )
    IID_BS_LOOKUP_NEXT_TEST( _oaidl, 4 )
    IID_BS_LOOKUP_NEXT_TEST( _oaidl, 2 )
    IID_BS_LOOKUP_NEXT_TEST( _oaidl, 1 )
    IID_BS_LOOKUP_RETURN_RESULT( _oaidl, 13, *pIndex )
    
}

const ExtendedProxyFileInfo oaidl_ProxyFileInfo = 
{
    (PCInterfaceProxyVtblList *) & _oaidl_ProxyVtblList,
    (PCInterfaceStubVtblList *) & _oaidl_StubVtblList,
    (const PCInterfaceName * ) & _oaidl_InterfaceNamesList,
    0, // no delegation
    & _oaidl_IID_Lookup, 
    13,
    2,
    0, /* table of [async_uuid] interfaces */
    0, /* Filler1 */
    0, /* Filler2 */
    0  /* Filler3 */
};


#endif /* !defined(_M_IA64) && !defined(_M_AMD64)*/


#pragma warning( disable: 4049 )  /* more than 64k source lines */

/* this ALWAYS GENERATED file contains the proxy stub code */


 /* File created by MIDL compiler version 6.00.0347 */
/* at Sun May 05 19:07:35 2002
 */
/* Compiler settings for oaidl.idl:
    Oicf, W1, Zp8, env=Win64 (32b run,appending)
    protocol : dce , ms_ext, c_ext, robust
    error checks: allocation ref bounds_check enum stub_data 
    VC __declspec() decoration level: 
         __declspec(uuid()), __declspec(selectany), __declspec(novtable)
         DECLSPEC_UUID(), MIDL_INTERFACE()
*/
//@@MIDL_FILE_HEADING(  )

#if defined(_M_IA64) || defined(_M_AMD64)
#define USE_STUBLESS_PROXY


/* verify that the <rpcproxy.h> version is high enough to compile this file*/
#ifndef __REDQ_RPCPROXY_H_VERSION__
#define __REQUIRED_RPCPROXY_H_VERSION__ 475
#endif


#include "rpcproxy.h"
#ifndef __RPCPROXY_H_VERSION__
#error this stub requires an updated version of <rpcproxy.h>
#endif // __RPCPROXY_H_VERSION__


#include "oaidl.h"

#define TYPE_FORMAT_STRING_SIZE   1967                              
#define PROC_FORMAT_STRING_SIZE   3293                              
#define TRANSMIT_AS_TABLE_SIZE    0            
#define WIRE_MARSHAL_TABLE_SIZE   3            

typedef struct _MIDL_TYPE_FORMAT_STRING
    {
    short          Pad;
    unsigned char  Format[ TYPE_FORMAT_STRING_SIZE ];
    } MIDL_TYPE_FORMAT_STRING;

typedef struct _MIDL_PROC_FORMAT_STRING
    {
    short          Pad;
    unsigned char  Format[ PROC_FORMAT_STRING_SIZE ];
    } MIDL_PROC_FORMAT_STRING;


static RPC_SYNTAX_IDENTIFIER  _RpcTransferSyntax = 
{{0x8A885D04,0x1CEB,0x11C9,{0x9F,0xE8,0x08,0x00,0x2B,0x10,0x48,0x60}},{2,0}};


extern const MIDL_TYPE_FORMAT_STRING __MIDL_TypeFormatString;
extern const MIDL_PROC_FORMAT_STRING __MIDL_ProcFormatString;


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO IDispatch_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO IDispatch_ProxyInfo;

/* [call_as] */ HRESULT STDMETHODCALLTYPE IDispatch_RemoteInvoke_Proxy( 
    IDispatch * This,
    /* [in] */ DISPID dispIdMember,
    /* [in] */ REFIID riid,
    /* [in] */ LCID lcid,
    /* [in] */ DWORD dwFlags,
    /* [in] */ DISPPARAMS *pDispParams,
    /* [out] */ VARIANT *pVarResult,
    /* [out] */ EXCEPINFO *pExcepInfo,
    /* [out] */ UINT *pArgErr,
    /* [in] */ UINT cVarRef,
    /* [size_is][in] */ UINT *rgVarRefIdx,
    /* [size_is][out][in] */ VARIANTARG *rgVarRef)
{
CLIENT_CALL_RETURN _RetVal;

_RetVal = NdrClientCall2(
                  ( PMIDL_STUB_DESC  )&Object_StubDesc,
                  (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[150],
                  ( unsigned char * )This,
                  dispIdMember,
                  riid,
                  lcid,
                  dwFlags,
                  pDispParams,
                  pVarResult,
                  pExcepInfo,
                  pArgErr,
                  cVarRef,
                  rgVarRefIdx,
                  rgVarRef);
return ( HRESULT  )_RetVal.Simple;

}

void __RPC_API
IDispatch_RemoteInvoke_Thunk(
    PMIDL_STUB_MESSAGE pStubMsg )
{
    #pragma pack(8)
    struct _PARAM_STRUCT
        {
        IDispatch *This;
        DISPID dispIdMember;
        char Pad0[4];
        REFIID riid;
        LCID lcid;
        char Pad1[4];
        DWORD dwFlags;
        char Pad2[4];
        DISPPARAMS *pDispParams;
        VARIANT *pVarResult;
        EXCEPINFO *pExcepInfo;
        UINT *pArgErr;
        UINT cVarRef;
        char Pad3[4];
        UINT *rgVarRefIdx;
        VARIANTARG *rgVarRef;
        HRESULT _RetVal;
        };
    #pragma pack()
    struct _PARAM_STRUCT * pParamStruct;

    pParamStruct = (struct _PARAM_STRUCT *) pStubMsg->StackTop;
    
    /* Call the server */
    
    pParamStruct->_RetVal = IDispatch_Invoke_Stub(
                                (IDispatch *) pParamStruct->This,
                                pParamStruct->dispIdMember,
                                pParamStruct->riid,
                                pParamStruct->lcid,
                                pParamStruct->dwFlags,
                                pParamStruct->pDispParams,
                                pParamStruct->pVarResult,
                                pParamStruct->pExcepInfo,
                                pParamStruct->pArgErr,
                                pParamStruct->cVarRef,
                                pParamStruct->rgVarRefIdx,
                                pParamStruct->rgVarRef);
    
}


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO IEnumVARIANT_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO IEnumVARIANT_ProxyInfo;

/* [call_as] */ HRESULT STDMETHODCALLTYPE IEnumVARIANT_RemoteNext_Proxy( 
    IEnumVARIANT * This,
    /* [in] */ ULONG celt,
    /* [length_is][size_is][out] */ VARIANT *rgVar,
    /* [out] */ ULONG *pCeltFetched)
{
CLIENT_CALL_RETURN _RetVal;

_RetVal = NdrClientCall2(
                  ( PMIDL_STUB_DESC  )&Object_StubDesc,
                  (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[248],
                  ( unsigned char * )This,
                  celt,
                  rgVar,
                  pCeltFetched);
return ( HRESULT  )_RetVal.Simple;

}

void __RPC_API
IEnumVARIANT_RemoteNext_Thunk(
    PMIDL_STUB_MESSAGE pStubMsg )
{
    #pragma pack(8)
    struct _PARAM_STRUCT
        {
        IEnumVARIANT *This;
        ULONG celt;
        char Pad0[4];
        VARIANT *rgVar;
        ULONG *pCeltFetched;
        HRESULT _RetVal;
        };
    #pragma pack()
    struct _PARAM_STRUCT * pParamStruct;

    pParamStruct = (struct _PARAM_STRUCT *) pStubMsg->StackTop;
    
    /* Call the server */
    
    pParamStruct->_RetVal = IEnumVARIANT_Next_Stub(
                                 (IEnumVARIANT *) pParamStruct->This,
                                 pParamStruct->celt,
                                 pParamStruct->rgVar,
                                 pParamStruct->pCeltFetched);
    
}


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO ITypeComp_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO ITypeComp_ProxyInfo;

/* [call_as] */ HRESULT STDMETHODCALLTYPE ITypeComp_RemoteBind_Proxy( 
    ITypeComp * This,
    /* [in] */ LPOLESTR szName,
    /* [in] */ ULONG lHashVal,
    /* [in] */ WORD wFlags,
    /* [out] */ ITypeInfo **ppTInfo,
    /* [out] */ DESCKIND *pDescKind,
    /* [out] */ LPFUNCDESC *ppFuncDesc,
    /* [out] */ LPVARDESC *ppVarDesc,
    /* [out] */ ITypeComp **ppTypeComp,
    /* [out] */ CLEANLOCALSTORAGE *pDummy)
{
CLIENT_CALL_RETURN _RetVal;

_RetVal = NdrClientCall2(
                  ( PMIDL_STUB_DESC  )&Object_StubDesc,
                  (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[406],
                  ( unsigned char * )This,
                  szName,
                  lHashVal,
                  wFlags,
                  ppTInfo,
                  pDescKind,
                  ppFuncDesc,
                  ppVarDesc,
                  ppTypeComp,
                  pDummy);
return ( HRESULT  )_RetVal.Simple;

}

void __RPC_API
ITypeComp_RemoteBind_Thunk(
    PMIDL_STUB_MESSAGE pStubMsg )
{
    #pragma pack(8)
    struct _PARAM_STRUCT
        {
        ITypeComp *This;
        LPOLESTR szName;
        ULONG lHashVal;
        char Pad0[4];
        WORD wFlags;
        char Pad1[6];
        ITypeInfo **ppTInfo;
        DESCKIND *pDescKind;
        LPFUNCDESC *ppFuncDesc;
        LPVARDESC *ppVarDesc;
        ITypeComp **ppTypeComp;
        CLEANLOCALSTORAGE *pDummy;
        HRESULT _RetVal;
        };
    #pragma pack()
    struct _PARAM_STRUCT * pParamStruct;

    pParamStruct = (struct _PARAM_STRUCT *) pStubMsg->StackTop;
    
    /* Call the server */
    
    pParamStruct->_RetVal = ITypeComp_Bind_Stub(
                              (ITypeComp *) pParamStruct->This,
                              pParamStruct->szName,
                              pParamStruct->lHashVal,
                              pParamStruct->wFlags,
                              pParamStruct->ppTInfo,
                              pParamStruct->pDescKind,
                              pParamStruct->ppFuncDesc,
                              pParamStruct->ppVarDesc,
                              pParamStruct->ppTypeComp,
                              pParamStruct->pDummy);
    
}

/* [call_as] */ HRESULT STDMETHODCALLTYPE ITypeComp_RemoteBindType_Proxy( 
    ITypeComp * This,
    /* [in] */ LPOLESTR szName,
    /* [in] */ ULONG lHashVal,
    /* [out] */ ITypeInfo **ppTInfo)
{
CLIENT_CALL_RETURN _RetVal;

_RetVal = NdrClientCall2(
                  ( PMIDL_STUB_DESC  )&Object_StubDesc,
                  (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[492],
                  ( unsigned char * )This,
                  szName,
                  lHashVal,
                  ppTInfo);
return ( HRESULT  )_RetVal.Simple;

}

void __RPC_API
ITypeComp_RemoteBindType_Thunk(
    PMIDL_STUB_MESSAGE pStubMsg )
{
    #pragma pack(8)
    struct _PARAM_STRUCT
        {
        ITypeComp *This;
        LPOLESTR szName;
        ULONG lHashVal;
        char Pad0[4];
        ITypeInfo **ppTInfo;
        HRESULT _RetVal;
        };
    #pragma pack()
    struct _PARAM_STRUCT * pParamStruct;

    pParamStruct = (struct _PARAM_STRUCT *) pStubMsg->StackTop;
    
    /* Call the server */
    
    pParamStruct->_RetVal = ITypeComp_BindType_Stub(
                                  (ITypeComp *) pParamStruct->This,
                                  pParamStruct->szName,
                                  pParamStruct->lHashVal,
                                  pParamStruct->ppTInfo);
    
}


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO ITypeInfo_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO ITypeInfo_ProxyInfo;

/* [call_as] */ HRESULT STDMETHODCALLTYPE ITypeInfo_RemoteGetTypeAttr_Proxy( 
    ITypeInfo * This,
    /* [out] */ LPTYPEATTR *ppTypeAttr,
    /* [out] */ CLEANLOCALSTORAGE *pDummy)
{
CLIENT_CALL_RETURN _RetVal;

_RetVal = NdrClientCall2(
                  ( PMIDL_STUB_DESC  )&Object_StubDesc,
                  (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[542],
                  ( unsigned char * )This,
                  ppTypeAttr,
                  pDummy);
return ( HRESULT  )_RetVal.Simple;

}

void __RPC_API
ITypeInfo_RemoteGetTypeAttr_Thunk(
    PMIDL_STUB_MESSAGE pStubMsg )
{
    #pragma pack(8)
    struct _PARAM_STRUCT
        {
        ITypeInfo *This;
        LPTYPEATTR *ppTypeAttr;
        CLEANLOCALSTORAGE *pDummy;
        HRESULT _RetVal;
        };
    #pragma pack()
    struct _PARAM_STRUCT * pParamStruct;

    pParamStruct = (struct _PARAM_STRUCT *) pStubMsg->StackTop;
    
    /* Call the server */
    
    pParamStruct->_RetVal = ITypeInfo_GetTypeAttr_Stub(
                                     (ITypeInfo *) pParamStruct->This,
                                     pParamStruct->ppTypeAttr,
                                     pParamStruct->pDummy);
    
}

/* [call_as] */ HRESULT STDMETHODCALLTYPE ITypeInfo_RemoteGetFuncDesc_Proxy( 
    ITypeInfo * This,
    /* [in] */ UINT index,
    /* [out] */ LPFUNCDESC *ppFuncDesc,
    /* [out] */ CLEANLOCALSTORAGE *pDummy)
{
CLIENT_CALL_RETURN _RetVal;

_RetVal = NdrClientCall2(
                  ( PMIDL_STUB_DESC  )&Object_StubDesc,
                  (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[624],
                  ( unsigned char * )This,
                  index,
                  ppFuncDesc,
                  pDummy);
return ( HRESULT  )_RetVal.Simple;

}

void __RPC_API
ITypeInfo_RemoteGetFuncDesc_Thunk(
    PMIDL_STUB_MESSAGE pStubMsg )
{
    #pragma pack(8)
    struct _PARAM_STRUCT
        {
        ITypeInfo *This;
        UINT index;
        char Pad0[4];
        LPFUNCDESC *ppFuncDesc;
        CLEANLOCALSTORAGE *pDummy;
        HRESULT _RetVal;
        };
    #pragma pack()
    struct _PARAM_STRUCT * pParamStruct;

    pParamStruct = (struct _PARAM_STRUCT *) pStubMsg->StackTop;
    
    /* Call the server */
    
    pParamStruct->_RetVal = ITypeInfo_GetFuncDesc_Stub(
                                     (ITypeInfo *) pParamStruct->This,
                                     pParamStruct->index,
                                     pParamStruct->ppFuncDesc,
                                     pParamStruct->pDummy);
    
}

/* [call_as] */ HRESULT STDMETHODCALLTYPE ITypeInfo_RemoteGetVarDesc_Proxy( 
    ITypeInfo * This,
    /* [in] */ UINT index,
    /* [out] */ LPVARDESC *ppVarDesc,
    /* [out] */ CLEANLOCALSTORAGE *pDummy)
{
CLIENT_CALL_RETURN _RetVal;

_RetVal = NdrClientCall2(
                  ( PMIDL_STUB_DESC  )&Object_StubDesc,
                  (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[674],
                  ( unsigned char * )This,
                  index,
                  ppVarDesc,
                  pDummy);
return ( HRESULT  )_RetVal.Simple;

}

void __RPC_API
ITypeInfo_RemoteGetVarDesc_Thunk(
    PMIDL_STUB_MESSAGE pStubMsg )
{
    #pragma pack(8)
    struct _PARAM_STRUCT
        {
        ITypeInfo *This;
        UINT index;
        char Pad0[4];
        LPVARDESC *ppVarDesc;
        CLEANLOCALSTORAGE *pDummy;
        HRESULT _RetVal;
        };
    #pragma pack()
    struct _PARAM_STRUCT * pParamStruct;

    pParamStruct = (struct _PARAM_STRUCT *) pStubMsg->StackTop;
    
    /* Call the server */
    
    pParamStruct->_RetVal = ITypeInfo_GetVarDesc_Stub(
                                    (ITypeInfo *) pParamStruct->This,
                                    pParamStruct->index,
                                    pParamStruct->ppVarDesc,
                                    pParamStruct->pDummy);
    
}

/* [call_as] */ HRESULT STDMETHODCALLTYPE ITypeInfo_RemoteGetNames_Proxy( 
    ITypeInfo * This,
    /* [in] */ MEMBERID memid,
    /* [length_is][size_is][out] */ BSTR *rgBstrNames,
    /* [in] */ UINT cMaxNames,
    /* [out] */ UINT *pcNames)
{
CLIENT_CALL_RETURN _RetVal;

_RetVal = NdrClientCall2(
                  ( PMIDL_STUB_DESC  )&Object_StubDesc,
                  (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[724],
                  ( unsigned char * )This,
                  memid,
                  rgBstrNames,
                  cMaxNames,
                  pcNames);
return ( HRESULT  )_RetVal.Simple;

}

void __RPC_API
ITypeInfo_RemoteGetNames_Thunk(
    PMIDL_STUB_MESSAGE pStubMsg )
{
    #pragma pack(8)
    struct _PARAM_STRUCT
        {
        ITypeInfo *This;
        MEMBERID memid;
        char Pad0[4];
        BSTR *rgBstrNames;
        UINT cMaxNames;
        char Pad1[4];
        UINT *pcNames;
        HRESULT _RetVal;
        };
    #pragma pack()
    struct _PARAM_STRUCT * pParamStruct;

    pParamStruct = (struct _PARAM_STRUCT *) pStubMsg->StackTop;
    
    /* Call the server */
    
    pParamStruct->_RetVal = ITypeInfo_GetNames_Stub(
                                  (ITypeInfo *) pParamStruct->This,
                                  pParamStruct->memid,
                                  pParamStruct->rgBstrNames,
                                  pParamStruct->cMaxNames,
                                  pParamStruct->pcNames);
    
}

void __RPC_API
ITypeInfo_LocalGetIDsOfNames_Thunk(
    PMIDL_STUB_MESSAGE pStubMsg )
{
    #pragma pack(8)
    struct _PARAM_STRUCT
        {
        ITypeInfo *This;
        HRESULT _RetVal;
        };
    #pragma pack()
    struct _PARAM_STRUCT * pParamStruct;

    pParamStruct = (struct _PARAM_STRUCT *) pStubMsg->StackTop;
    
    /* Call the server */
    
    pParamStruct->_RetVal = ITypeInfo_GetIDsOfNames_Stub((ITypeInfo *) pParamStruct->This);
    
}

void __RPC_API
ITypeInfo_LocalInvoke_Thunk(
    PMIDL_STUB_MESSAGE pStubMsg )
{
    #pragma pack(8)
    struct _PARAM_STRUCT
        {
        ITypeInfo *This;
        HRESULT _RetVal;
        };
    #pragma pack()
    struct _PARAM_STRUCT * pParamStruct;

    pParamStruct = (struct _PARAM_STRUCT *) pStubMsg->StackTop;
    
    /* Call the server */
    
    pParamStruct->_RetVal = ITypeInfo_Invoke_Stub((ITypeInfo *) pParamStruct->This);
    
}

/* [call_as] */ HRESULT STDMETHODCALLTYPE ITypeInfo_RemoteGetDocumentation_Proxy( 
    ITypeInfo * This,
    /* [in] */ MEMBERID memid,
    /* [in] */ DWORD refPtrFlags,
    /* [out] */ BSTR *pBstrName,
    /* [out] */ BSTR *pBstrDocString,
    /* [out] */ DWORD *pdwHelpContext,
    /* [out] */ BSTR *pBstrHelpFile)
{
CLIENT_CALL_RETURN _RetVal;

_RetVal = NdrClientCall2(
                  ( PMIDL_STUB_DESC  )&Object_StubDesc,
                  (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[932],
                  ( unsigned char * )This,
                  memid,
                  refPtrFlags,
                  pBstrName,
                  pBstrDocString,
                  pdwHelpContext,
                  pBstrHelpFile);
return ( HRESULT  )_RetVal.Simple;

}

void __RPC_API
ITypeInfo_RemoteGetDocumentation_Thunk(
    PMIDL_STUB_MESSAGE pStubMsg )
{
    #pragma pack(8)
    struct _PARAM_STRUCT
        {
        ITypeInfo *This;
        MEMBERID memid;
        char Pad0[4];
        DWORD refPtrFlags;
        char Pad1[4];
        BSTR *pBstrName;
        BSTR *pBstrDocString;
        DWORD *pdwHelpContext;
        BSTR *pBstrHelpFile;
        HRESULT _RetVal;
        };
    #pragma pack()
    struct _PARAM_STRUCT * pParamStruct;

    pParamStruct = (struct _PARAM_STRUCT *) pStubMsg->StackTop;
    
    /* Call the server */
    
    pParamStruct->_RetVal = ITypeInfo_GetDocumentation_Stub(
                                          (ITypeInfo *) pParamStruct->This,
                                          pParamStruct->memid,
                                          pParamStruct->refPtrFlags,
                                          pParamStruct->pBstrName,
                                          pParamStruct->pBstrDocString,
                                          pParamStruct->pdwHelpContext,
                                          pParamStruct->pBstrHelpFile);
    
}

/* [call_as] */ HRESULT STDMETHODCALLTYPE ITypeInfo_RemoteGetDllEntry_Proxy( 
    ITypeInfo * This,
    /* [in] */ MEMBERID memid,
    /* [in] */ INVOKEKIND invKind,
    /* [in] */ DWORD refPtrFlags,
    /* [out] */ BSTR *pBstrDllName,
    /* [out] */ BSTR *pBstrName,
    /* [out] */ WORD *pwOrdinal)
{
CLIENT_CALL_RETURN _RetVal;

_RetVal = NdrClientCall2(
                  ( PMIDL_STUB_DESC  )&Object_StubDesc,
                  (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[1000],
                  ( unsigned char * )This,
                  memid,
                  invKind,
                  refPtrFlags,
                  pBstrDllName,
                  pBstrName,
                  pwOrdinal);
return ( HRESULT  )_RetVal.Simple;

}

void __RPC_API
ITypeInfo_RemoteGetDllEntry_Thunk(
    PMIDL_STUB_MESSAGE pStubMsg )
{
    #pragma pack(8)
    struct _PARAM_STRUCT
        {
        ITypeInfo *This;
        MEMBERID memid;
        char Pad0[4];
        INVOKEKIND invKind;
        char Pad1[4];
        DWORD refPtrFlags;
        char Pad2[4];
        BSTR *pBstrDllName;
        BSTR *pBstrName;
        WORD *pwOrdinal;
        HRESULT _RetVal;
        };
    #pragma pack()
    struct _PARAM_STRUCT * pParamStruct;

    pParamStruct = (struct _PARAM_STRUCT *) pStubMsg->StackTop;
    
    /* Call the server */
    
    pParamStruct->_RetVal = ITypeInfo_GetDllEntry_Stub(
                                     (ITypeInfo *) pParamStruct->This,
                                     pParamStruct->memid,
                                     pParamStruct->invKind,
                                     pParamStruct->refPtrFlags,
                                     pParamStruct->pBstrDllName,
                                     pParamStruct->pBstrName,
                                     pParamStruct->pwOrdinal);
    
}

void __RPC_API
ITypeInfo_LocalAddressOfMember_Thunk(
    PMIDL_STUB_MESSAGE pStubMsg )
{
    #pragma pack(8)
    struct _PARAM_STRUCT
        {
        ITypeInfo *This;
        HRESULT _RetVal;
        };
    #pragma pack()
    struct _PARAM_STRUCT * pParamStruct;

    pParamStruct = (struct _PARAM_STRUCT *) pStubMsg->StackTop;
    
    /* Call the server */
    
    pParamStruct->_RetVal = ITypeInfo_AddressOfMember_Stub((ITypeInfo *) pParamStruct->This);
    
}

/* [call_as] */ HRESULT STDMETHODCALLTYPE ITypeInfo_RemoteCreateInstance_Proxy( 
    ITypeInfo * This,
    /* [in] */ REFIID riid,
    /* [iid_is][out] */ IUnknown **ppvObj)
{
CLIENT_CALL_RETURN _RetVal;

_RetVal = NdrClientCall2(
                  ( PMIDL_STUB_DESC  )&Object_StubDesc,
                  (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[1144],
                  ( unsigned char * )This,
                  riid,
                  ppvObj);
return ( HRESULT  )_RetVal.Simple;

}

void __RPC_API
ITypeInfo_RemoteCreateInstance_Thunk(
    PMIDL_STUB_MESSAGE pStubMsg )
{
    #pragma pack(8)
    struct _PARAM_STRUCT
        {
        ITypeInfo *This;
        REFIID riid;
        IUnknown **ppvObj;
        HRESULT _RetVal;
        };
    #pragma pack()
    struct _PARAM_STRUCT * pParamStruct;

    pParamStruct = (struct _PARAM_STRUCT *) pStubMsg->StackTop;
    
    /* Call the server */
    
    pParamStruct->_RetVal = ITypeInfo_CreateInstance_Stub(
                                        (ITypeInfo *) pParamStruct->This,
                                        pParamStruct->riid,
                                        pParamStruct->ppvObj);
    
}

/* [call_as] */ HRESULT STDMETHODCALLTYPE ITypeInfo_RemoteGetContainingTypeLib_Proxy( 
    ITypeInfo * This,
    /* [out] */ ITypeLib **ppTLib,
    /* [out] */ UINT *pIndex)
{
CLIENT_CALL_RETURN _RetVal;

_RetVal = NdrClientCall2(
                  ( PMIDL_STUB_DESC  )&Object_StubDesc,
                  (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[1232],
                  ( unsigned char * )This,
                  ppTLib,
                  pIndex);
return ( HRESULT  )_RetVal.Simple;

}

void __RPC_API
ITypeInfo_RemoteGetContainingTypeLib_Thunk(
    PMIDL_STUB_MESSAGE pStubMsg )
{
    #pragma pack(8)
    struct _PARAM_STRUCT
        {
        ITypeInfo *This;
        ITypeLib **ppTLib;
        UINT *pIndex;
        HRESULT _RetVal;
        };
    #pragma pack()
    struct _PARAM_STRUCT * pParamStruct;

    pParamStruct = (struct _PARAM_STRUCT *) pStubMsg->StackTop;
    
    /* Call the server */
    
    pParamStruct->_RetVal = ITypeInfo_GetContainingTypeLib_Stub(
                                              (ITypeInfo *) pParamStruct->This,
                                              pParamStruct->ppTLib,
                                              pParamStruct->pIndex);
    
}

void __RPC_API
ITypeInfo_LocalReleaseTypeAttr_Thunk(
    PMIDL_STUB_MESSAGE pStubMsg )
{
    #pragma pack(8)
    struct _PARAM_STRUCT
        {
        ITypeInfo *This;
        HRESULT _RetVal;
        };
    #pragma pack()
    struct _PARAM_STRUCT * pParamStruct;

    pParamStruct = (struct _PARAM_STRUCT *) pStubMsg->StackTop;
    
    /* Call the server */
    
    pParamStruct->_RetVal = ITypeInfo_ReleaseTypeAttr_Stub((ITypeInfo *) pParamStruct->This);
    
}

void __RPC_API
ITypeInfo_LocalReleaseFuncDesc_Thunk(
    PMIDL_STUB_MESSAGE pStubMsg )
{
    #pragma pack(8)
    struct _PARAM_STRUCT
        {
        ITypeInfo *This;
        HRESULT _RetVal;
        };
    #pragma pack()
    struct _PARAM_STRUCT * pParamStruct;

    pParamStruct = (struct _PARAM_STRUCT *) pStubMsg->StackTop;
    
    /* Call the server */
    
    pParamStruct->_RetVal = ITypeInfo_ReleaseFuncDesc_Stub((ITypeInfo *) pParamStruct->This);
    
}

void __RPC_API
ITypeInfo_LocalReleaseVarDesc_Thunk(
    PMIDL_STUB_MESSAGE pStubMsg )
{
    #pragma pack(8)
    struct _PARAM_STRUCT
        {
        ITypeInfo *This;
        HRESULT _RetVal;
        };
    #pragma pack()
    struct _PARAM_STRUCT * pParamStruct;

    pParamStruct = (struct _PARAM_STRUCT *) pStubMsg->StackTop;
    
    /* Call the server */
    
    pParamStruct->_RetVal = ITypeInfo_ReleaseVarDesc_Stub((ITypeInfo *) pParamStruct->This);
    
}


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO ITypeInfo2_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO ITypeInfo2_ProxyInfo;

/* [call_as] */ HRESULT STDMETHODCALLTYPE ITypeInfo2_RemoteGetDocumentation2_Proxy( 
    ITypeInfo2 * This,
    /* [in] */ MEMBERID memid,
    /* [in] */ LCID lcid,
    /* [in] */ DWORD refPtrFlags,
    /* [out] */ BSTR *pbstrHelpString,
    /* [out] */ DWORD *pdwHelpStringContext,
    /* [out] */ BSTR *pbstrHelpStringDll)
{
CLIENT_CALL_RETURN _RetVal;

_RetVal = NdrClientCall2(
                  ( PMIDL_STUB_DESC  )&Object_StubDesc,
                  (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[1792],
                  ( unsigned char * )This,
                  memid,
                  lcid,
                  refPtrFlags,
                  pbstrHelpString,
                  pdwHelpStringContext,
                  pbstrHelpStringDll);
return ( HRESULT  )_RetVal.Simple;

}

void __RPC_API
ITypeInfo2_RemoteGetDocumentation2_Thunk(
    PMIDL_STUB_MESSAGE pStubMsg )
{
    #pragma pack(8)
    struct _PARAM_STRUCT
        {
        ITypeInfo2 *This;
        MEMBERID memid;
        char Pad0[4];
        LCID lcid;
        char Pad1[4];
        DWORD refPtrFlags;
        char Pad2[4];
        BSTR *pbstrHelpString;
        DWORD *pdwHelpStringContext;
        BSTR *pbstrHelpStringDll;
        HRESULT _RetVal;
        };
    #pragma pack()
    struct _PARAM_STRUCT * pParamStruct;

    pParamStruct = (struct _PARAM_STRUCT *) pStubMsg->StackTop;
    
    /* Call the server */
    
    pParamStruct->_RetVal = ITypeInfo2_GetDocumentation2_Stub(
                                            (ITypeInfo2 *) pParamStruct->This,
                                            pParamStruct->memid,
                                            pParamStruct->lcid,
                                            pParamStruct->refPtrFlags,
                                            pParamStruct->pbstrHelpString,
                                            pParamStruct->pdwHelpStringContext,
                                            pParamStruct->pbstrHelpStringDll);
    
}


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO ITypeLib_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO ITypeLib_ProxyInfo;

/* [call_as] */ HRESULT STDMETHODCALLTYPE ITypeLib_RemoteGetTypeInfoCount_Proxy( 
    ITypeLib * This,
    /* [out] */ UINT *pcTInfo)
{
CLIENT_CALL_RETURN _RetVal;

_RetVal = NdrClientCall2(
                  ( PMIDL_STUB_DESC  )&Object_StubDesc,
                  (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[0],
                  ( unsigned char * )This,
                  pcTInfo);
return ( HRESULT  )_RetVal.Simple;

}

void __RPC_API
ITypeLib_RemoteGetTypeInfoCount_Thunk(
    PMIDL_STUB_MESSAGE pStubMsg )
{
    #pragma pack(8)
    struct _PARAM_STRUCT
        {
        ITypeLib *This;
        UINT *pcTInfo;
        HRESULT _RetVal;
        };
    #pragma pack()
    struct _PARAM_STRUCT * pParamStruct;

    pParamStruct = (struct _PARAM_STRUCT *) pStubMsg->StackTop;
    
    /* Call the server */
    
    pParamStruct->_RetVal = ITypeLib_GetTypeInfoCount_Stub((ITypeLib *) pParamStruct->This,pParamStruct->pcTInfo);
    
}

/* [call_as] */ HRESULT STDMETHODCALLTYPE ITypeLib_RemoteGetLibAttr_Proxy( 
    ITypeLib * This,
    /* [out] */ LPTLIBATTR *ppTLibAttr,
    /* [out] */ CLEANLOCALSTORAGE *pDummy)
{
CLIENT_CALL_RETURN _RetVal;

_RetVal = NdrClientCall2(
                  ( PMIDL_STUB_DESC  )&Object_StubDesc,
                  (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[2212],
                  ( unsigned char * )This,
                  ppTLibAttr,
                  pDummy);
return ( HRESULT  )_RetVal.Simple;

}

void __RPC_API
ITypeLib_RemoteGetLibAttr_Thunk(
    PMIDL_STUB_MESSAGE pStubMsg )
{
    #pragma pack(8)
    struct _PARAM_STRUCT
        {
        ITypeLib *This;
        LPTLIBATTR *ppTLibAttr;
        CLEANLOCALSTORAGE *pDummy;
        HRESULT _RetVal;
        };
    #pragma pack()
    struct _PARAM_STRUCT * pParamStruct;

    pParamStruct = (struct _PARAM_STRUCT *) pStubMsg->StackTop;
    
    /* Call the server */
    
    pParamStruct->_RetVal = ITypeLib_GetLibAttr_Stub(
                                   (ITypeLib *) pParamStruct->This,
                                   pParamStruct->ppTLibAttr,
                                   pParamStruct->pDummy);
    
}

/* [call_as] */ HRESULT STDMETHODCALLTYPE ITypeLib_RemoteGetDocumentation_Proxy( 
    ITypeLib * This,
    /* [in] */ INT index,
    /* [in] */ DWORD refPtrFlags,
    /* [out] */ BSTR *pBstrName,
    /* [out] */ BSTR *pBstrDocString,
    /* [out] */ DWORD *pdwHelpContext,
    /* [out] */ BSTR *pBstrHelpFile)
{
CLIENT_CALL_RETURN _RetVal;

_RetVal = NdrClientCall2(
                  ( PMIDL_STUB_DESC  )&Object_StubDesc,
                  (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[2294],
                  ( unsigned char * )This,
                  index,
                  refPtrFlags,
                  pBstrName,
                  pBstrDocString,
                  pdwHelpContext,
                  pBstrHelpFile);
return ( HRESULT  )_RetVal.Simple;

}

void __RPC_API
ITypeLib_RemoteGetDocumentation_Thunk(
    PMIDL_STUB_MESSAGE pStubMsg )
{
    #pragma pack(8)
    struct _PARAM_STRUCT
        {
        ITypeLib *This;
        INT index;
        char Pad0[4];
        DWORD refPtrFlags;
        char Pad1[4];
        BSTR *pBstrName;
        BSTR *pBstrDocString;
        DWORD *pdwHelpContext;
        BSTR *pBstrHelpFile;
        HRESULT _RetVal;
        };
    #pragma pack()
    struct _PARAM_STRUCT * pParamStruct;

    pParamStruct = (struct _PARAM_STRUCT *) pStubMsg->StackTop;
    
    /* Call the server */
    
    pParamStruct->_RetVal = ITypeLib_GetDocumentation_Stub(
                                         (ITypeLib *) pParamStruct->This,
                                         pParamStruct->index,
                                         pParamStruct->refPtrFlags,
                                         pParamStruct->pBstrName,
                                         pParamStruct->pBstrDocString,
                                         pParamStruct->pdwHelpContext,
                                         pParamStruct->pBstrHelpFile);
    
}

/* [call_as] */ HRESULT STDMETHODCALLTYPE ITypeLib_RemoteIsName_Proxy( 
    ITypeLib * This,
    /* [in] */ LPOLESTR szNameBuf,
    /* [in] */ ULONG lHashVal,
    /* [out] */ BOOL *pfName,
    /* [out] */ BSTR *pBstrLibName)
{
CLIENT_CALL_RETURN _RetVal;

_RetVal = NdrClientCall2(
                  ( PMIDL_STUB_DESC  )&Object_StubDesc,
                  (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[2362],
                  ( unsigned char * )This,
                  szNameBuf,
                  lHashVal,
                  pfName,
                  pBstrLibName);
return ( HRESULT  )_RetVal.Simple;

}

void __RPC_API
ITypeLib_RemoteIsName_Thunk(
    PMIDL_STUB_MESSAGE pStubMsg )
{
    #pragma pack(8)
    struct _PARAM_STRUCT
        {
        ITypeLib *This;
        LPOLESTR szNameBuf;
        ULONG lHashVal;
        char Pad0[4];
        BOOL *pfName;
        BSTR *pBstrLibName;
        HRESULT _RetVal;
        };
    #pragma pack()
    struct _PARAM_STRUCT * pParamStruct;

    pParamStruct = (struct _PARAM_STRUCT *) pStubMsg->StackTop;
    
    /* Call the server */
    
    pParamStruct->_RetVal = ITypeLib_IsName_Stub(
                               (ITypeLib *) pParamStruct->This,
                               pParamStruct->szNameBuf,
                               pParamStruct->lHashVal,
                               pParamStruct->pfName,
                               pParamStruct->pBstrLibName);
    
}

/* [call_as] */ HRESULT STDMETHODCALLTYPE ITypeLib_RemoteFindName_Proxy( 
    ITypeLib * This,
    /* [in] */ LPOLESTR szNameBuf,
    /* [in] */ ULONG lHashVal,
    /* [length_is][size_is][out] */ ITypeInfo **ppTInfo,
    /* [length_is][size_is][out] */ MEMBERID *rgMemId,
    /* [out][in] */ USHORT *pcFound,
    /* [out] */ BSTR *pBstrLibName)
{
CLIENT_CALL_RETURN _RetVal;

_RetVal = NdrClientCall2(
                  ( PMIDL_STUB_DESC  )&Object_StubDesc,
                  (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[2418],
                  ( unsigned char * )This,
                  szNameBuf,
                  lHashVal,
                  ppTInfo,
                  rgMemId,
                  pcFound,
                  pBstrLibName);
return ( HRESULT  )_RetVal.Simple;

}

void __RPC_API
ITypeLib_RemoteFindName_Thunk(
    PMIDL_STUB_MESSAGE pStubMsg )
{
    #pragma pack(8)
    struct _PARAM_STRUCT
        {
        ITypeLib *This;
        LPOLESTR szNameBuf;
        ULONG lHashVal;
        char Pad0[4];
        ITypeInfo **ppTInfo;
        MEMBERID *rgMemId;
        USHORT *pcFound;
        BSTR *pBstrLibName;
        HRESULT _RetVal;
        };
    #pragma pack()
    struct _PARAM_STRUCT * pParamStruct;

    pParamStruct = (struct _PARAM_STRUCT *) pStubMsg->StackTop;
    
    /* Call the server */
    
    pParamStruct->_RetVal = ITypeLib_FindName_Stub(
                                 (ITypeLib *) pParamStruct->This,
                                 pParamStruct->szNameBuf,
                                 pParamStruct->lHashVal,
                                 pParamStruct->ppTInfo,
                                 pParamStruct->rgMemId,
                                 pParamStruct->pcFound,
                                 pParamStruct->pBstrLibName);
    
}

void __RPC_API
ITypeLib_LocalReleaseTLibAttr_Thunk(
    PMIDL_STUB_MESSAGE pStubMsg )
{
    #pragma pack(8)
    struct _PARAM_STRUCT
        {
        ITypeLib *This;
        HRESULT _RetVal;
        };
    #pragma pack()
    struct _PARAM_STRUCT * pParamStruct;

    pParamStruct = (struct _PARAM_STRUCT *) pStubMsg->StackTop;
    
    /* Call the server */
    
    pParamStruct->_RetVal = ITypeLib_ReleaseTLibAttr_Stub((ITypeLib *) pParamStruct->This);
    
}


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO ITypeLib2_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO ITypeLib2_ProxyInfo;

/* [call_as] */ HRESULT STDMETHODCALLTYPE ITypeLib2_RemoteGetLibStatistics_Proxy( 
    ITypeLib2 * This,
    /* [out] */ ULONG *pcUniqueNames,
    /* [out] */ ULONG *pcchUniqueNames)
{
CLIENT_CALL_RETURN _RetVal;

_RetVal = NdrClientCall2(
                  ( PMIDL_STUB_DESC  )&Object_StubDesc,
                  (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[2562],
                  ( unsigned char * )This,
                  pcUniqueNames,
                  pcchUniqueNames);
return ( HRESULT  )_RetVal.Simple;

}

void __RPC_API
ITypeLib2_RemoteGetLibStatistics_Thunk(
    PMIDL_STUB_MESSAGE pStubMsg )
{
    #pragma pack(8)
    struct _PARAM_STRUCT
        {
        ITypeLib2 *This;
        ULONG *pcUniqueNames;
        ULONG *pcchUniqueNames;
        HRESULT _RetVal;
        };
    #pragma pack()
    struct _PARAM_STRUCT * pParamStruct;

    pParamStruct = (struct _PARAM_STRUCT *) pStubMsg->StackTop;
    
    /* Call the server */
    
    pParamStruct->_RetVal = ITypeLib2_GetLibStatistics_Stub(
                                          (ITypeLib2 *) pParamStruct->This,
                                          pParamStruct->pcUniqueNames,
                                          pParamStruct->pcchUniqueNames);
    
}

/* [call_as] */ HRESULT STDMETHODCALLTYPE ITypeLib2_RemoteGetDocumentation2_Proxy( 
    ITypeLib2 * This,
    /* [in] */ INT index,
    /* [in] */ LCID lcid,
    /* [in] */ DWORD refPtrFlags,
    /* [out] */ BSTR *pbstrHelpString,
    /* [out] */ DWORD *pdwHelpStringContext,
    /* [out] */ BSTR *pbstrHelpStringDll)
{
CLIENT_CALL_RETURN _RetVal;

_RetVal = NdrClientCall2(
                  ( PMIDL_STUB_DESC  )&Object_StubDesc,
                  (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[2606],
                  ( unsigned char * )This,
                  index,
                  lcid,
                  refPtrFlags,
                  pbstrHelpString,
                  pdwHelpStringContext,
                  pbstrHelpStringDll);
return ( HRESULT  )_RetVal.Simple;

}

void __RPC_API
ITypeLib2_RemoteGetDocumentation2_Thunk(
    PMIDL_STUB_MESSAGE pStubMsg )
{
    #pragma pack(8)
    struct _PARAM_STRUCT
        {
        ITypeLib2 *This;
        INT index;
        char Pad0[4];
        LCID lcid;
        char Pad1[4];
        DWORD refPtrFlags;
        char Pad2[4];
        BSTR *pbstrHelpString;
        DWORD *pdwHelpStringContext;
        BSTR *pbstrHelpStringDll;
        HRESULT _RetVal;
        };
    #pragma pack()
    struct _PARAM_STRUCT * pParamStruct;

    pParamStruct = (struct _PARAM_STRUCT *) pStubMsg->StackTop;
    
    /* Call the server */
    
    pParamStruct->_RetVal = ITypeLib2_GetDocumentation2_Stub(
                                           (ITypeLib2 *) pParamStruct->This,
                                           pParamStruct->index,
                                           pParamStruct->lcid,
                                           pParamStruct->refPtrFlags,
                                           pParamStruct->pbstrHelpString,
                                           pParamStruct->pdwHelpStringContext,
                                           pParamStruct->pbstrHelpStringDll);
    
}


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO IErrorInfo_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO IErrorInfo_ProxyInfo;


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO ICreateErrorInfo_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO ICreateErrorInfo_ProxyInfo;


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO ISupportErrorInfo_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO ISupportErrorInfo_ProxyInfo;


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO ITypeFactory_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO ITypeFactory_ProxyInfo;


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO IErrorLog_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO IErrorLog_ProxyInfo;


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO IPropertyBag_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO IPropertyBag_ProxyInfo;

/* [call_as] */ HRESULT STDMETHODCALLTYPE IPropertyBag_RemoteRead_Proxy( 
    IPropertyBag * This,
    /* [in] */ LPCOLESTR pszPropName,
    /* [out] */ VARIANT *pVar,
    /* [in] */ IErrorLog *pErrorLog,
    /* [in] */ DWORD varType,
    /* [in] */ IUnknown *pUnkObj)
{
CLIENT_CALL_RETURN _RetVal;

_RetVal = NdrClientCall2(
                  ( PMIDL_STUB_DESC  )&Object_StubDesc,
                  (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[3186],
                  ( unsigned char * )This,
                  pszPropName,
                  pVar,
                  pErrorLog,
                  varType,
                  pUnkObj);
return ( HRESULT  )_RetVal.Simple;

}

void __RPC_API
IPropertyBag_RemoteRead_Thunk(
    PMIDL_STUB_MESSAGE pStubMsg )
{
    #pragma pack(8)
    struct _PARAM_STRUCT
        {
        IPropertyBag *This;
        LPCOLESTR pszPropName;
        VARIANT *pVar;
        IErrorLog *pErrorLog;
        DWORD varType;
        char Pad0[4];
        IUnknown *pUnkObj;
        HRESULT _RetVal;
        };
    #pragma pack()
    struct _PARAM_STRUCT * pParamStruct;

    pParamStruct = (struct _PARAM_STRUCT *) pStubMsg->StackTop;
    
    /* Call the server */
    
    pParamStruct->_RetVal = IPropertyBag_Read_Stub(
                                 (IPropertyBag *) pParamStruct->This,
                                 pParamStruct->pszPropName,
                                 pParamStruct->pVar,
                                 pParamStruct->pErrorLog,
                                 pParamStruct->varType,
                                 pParamStruct->pUnkObj);
    
}


extern const USER_MARSHAL_ROUTINE_QUADRUPLE UserMarshalRoutines[ WIRE_MARSHAL_TABLE_SIZE ];

#if !defined(__RPC_WIN64__)
#error  Invalid build platform for this stub.
#endif

static const MIDL_PROC_FORMAT_STRING __MIDL_ProcFormatString =
    {
        0,
        {

	/* Procedure RemoteGetTypeInfoCount */


	/* Procedure GetTypeInfoCount */

			0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/*  2 */	NdrFcLong( 0x0 ),	/* 0 */
/*  6 */	NdrFcShort( 0x3 ),	/* 3 */
/*  8 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 10 */	NdrFcShort( 0x0 ),	/* 0 */
/* 12 */	NdrFcShort( 0x24 ),	/* 36 */
/* 14 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x2,		/* 2 */
/* 16 */	0xa,		/* 10 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 18 */	NdrFcShort( 0x0 ),	/* 0 */
/* 20 */	NdrFcShort( 0x0 ),	/* 0 */
/* 22 */	NdrFcShort( 0x0 ),	/* 0 */
/* 24 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pcTInfo */


	/* Parameter pctinfo */

/* 26 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 28 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 30 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */


	/* Return value */

/* 32 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 34 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 36 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetTypeInfo */

/* 38 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 40 */	NdrFcLong( 0x0 ),	/* 0 */
/* 44 */	NdrFcShort( 0x4 ),	/* 4 */
/* 46 */	NdrFcShort( 0x28 ),	/* ia64 Stack size/offset = 40 */
/* 48 */	NdrFcShort( 0x10 ),	/* 16 */
/* 50 */	NdrFcShort( 0x8 ),	/* 8 */
/* 52 */	0x45,		/* Oi2 Flags:  srv must size, has return, has ext, */
			0x4,		/* 4 */
/* 54 */	0xa,		/* 10 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 56 */	NdrFcShort( 0x0 ),	/* 0 */
/* 58 */	NdrFcShort( 0x0 ),	/* 0 */
/* 60 */	NdrFcShort( 0x0 ),	/* 0 */
/* 62 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter iTInfo */

/* 64 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 66 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 68 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter lcid */

/* 70 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 72 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 74 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter ppTInfo */

/* 76 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
/* 78 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 80 */	NdrFcShort( 0x6 ),	/* Type Offset=6 */

	/* Return value */

/* 82 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 84 */	NdrFcShort( 0x20 ),	/* ia64 Stack size/offset = 32 */
/* 86 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetIDsOfNames */

/* 88 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 90 */	NdrFcLong( 0x0 ),	/* 0 */
/* 94 */	NdrFcShort( 0x5 ),	/* 5 */
/* 96 */	NdrFcShort( 0x38 ),	/* ia64 Stack size/offset = 56 */
/* 98 */	NdrFcShort( 0x54 ),	/* 84 */
/* 100 */	NdrFcShort( 0x8 ),	/* 8 */
/* 102 */	0x47,		/* Oi2 Flags:  srv must size, clt must size, has return, has ext, */
			0x6,		/* 6 */
/* 104 */	0xa,		/* 10 */
			0x7,		/* Ext Flags:  new corr desc, clt corr check, srv corr check, */
/* 106 */	NdrFcShort( 0x1 ),	/* 1 */
/* 108 */	NdrFcShort( 0x1 ),	/* 1 */
/* 110 */	NdrFcShort( 0x0 ),	/* 0 */
/* 112 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter riid */

/* 114 */	NdrFcShort( 0x10a ),	/* Flags:  must free, in, simple ref, */
/* 116 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 118 */	NdrFcShort( 0x26 ),	/* Type Offset=38 */

	/* Parameter rgszNames */

/* 120 */	NdrFcShort( 0x10b ),	/* Flags:  must size, must free, in, simple ref, */
/* 122 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 124 */	NdrFcShort( 0x36 ),	/* Type Offset=54 */

	/* Parameter cNames */

/* 126 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 128 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 130 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter lcid */

/* 132 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 134 */	NdrFcShort( 0x20 ),	/* ia64 Stack size/offset = 32 */
/* 136 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter rgDispId */

/* 138 */	NdrFcShort( 0x113 ),	/* Flags:  must size, must free, out, simple ref, */
/* 140 */	NdrFcShort( 0x28 ),	/* ia64 Stack size/offset = 40 */
/* 142 */	NdrFcShort( 0x50 ),	/* Type Offset=80 */

	/* Return value */

/* 144 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 146 */	NdrFcShort( 0x30 ),	/* ia64 Stack size/offset = 48 */
/* 148 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure RemoteInvoke */

/* 150 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 152 */	NdrFcLong( 0x0 ),	/* 0 */
/* 156 */	NdrFcShort( 0x6 ),	/* 6 */
/* 158 */	NdrFcShort( 0x68 ),	/* ia64 Stack size/offset = 104 */
/* 160 */	NdrFcShort( 0x64 ),	/* 100 */
/* 162 */	NdrFcShort( 0x24 ),	/* 36 */
/* 164 */	0x47,		/* Oi2 Flags:  srv must size, clt must size, has return, has ext, */
			0xc,		/* 12 */
/* 166 */	0xa,		/* 10 */
			0x7,		/* Ext Flags:  new corr desc, clt corr check, srv corr check, */
/* 168 */	NdrFcShort( 0x44 ),	/* 68 */
/* 170 */	NdrFcShort( 0x44 ),	/* 68 */
/* 172 */	NdrFcShort( 0x0 ),	/* 0 */
/* 174 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter dispIdMember */

/* 176 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 178 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 180 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter riid */

/* 182 */	NdrFcShort( 0x10a ),	/* Flags:  must free, in, simple ref, */
/* 184 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 186 */	NdrFcShort( 0x26 ),	/* Type Offset=38 */

	/* Parameter lcid */

/* 188 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 190 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 192 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter dwFlags */

/* 194 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 196 */	NdrFcShort( 0x20 ),	/* ia64 Stack size/offset = 32 */
/* 198 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter pDispParams */

/* 200 */	NdrFcShort( 0x10b ),	/* Flags:  must size, must free, in, simple ref, */
/* 202 */	NdrFcShort( 0x28 ),	/* ia64 Stack size/offset = 40 */
/* 204 */	NdrFcShort( 0x42a ),	/* Type Offset=1066 */

	/* Parameter pVarResult */

/* 206 */	NdrFcShort( 0x6113 ),	/* Flags:  must size, must free, out, simple ref, srv alloc size=24 */
/* 208 */	NdrFcShort( 0x30 ),	/* ia64 Stack size/offset = 48 */
/* 210 */	NdrFcShort( 0x448 ),	/* Type Offset=1096 */

	/* Parameter pExcepInfo */

/* 212 */	NdrFcShort( 0xc113 ),	/* Flags:  must size, must free, out, simple ref, srv alloc size=48 */
/* 214 */	NdrFcShort( 0x38 ),	/* ia64 Stack size/offset = 56 */
/* 216 */	NdrFcShort( 0x464 ),	/* Type Offset=1124 */

	/* Parameter pArgErr */

/* 218 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 220 */	NdrFcShort( 0x40 ),	/* ia64 Stack size/offset = 64 */
/* 222 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter cVarRef */

/* 224 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 226 */	NdrFcShort( 0x48 ),	/* ia64 Stack size/offset = 72 */
/* 228 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter rgVarRefIdx */

/* 230 */	NdrFcShort( 0x10b ),	/* Flags:  must size, must free, in, simple ref, */
/* 232 */	NdrFcShort( 0x50 ),	/* ia64 Stack size/offset = 80 */
/* 234 */	NdrFcShort( 0x484 ),	/* Type Offset=1156 */

	/* Parameter rgVarRef */

/* 236 */	NdrFcShort( 0x11b ),	/* Flags:  must size, must free, in, out, simple ref, */
/* 238 */	NdrFcShort( 0x58 ),	/* ia64 Stack size/offset = 88 */
/* 240 */	NdrFcShort( 0x494 ),	/* Type Offset=1172 */

	/* Return value */

/* 242 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 244 */	NdrFcShort( 0x60 ),	/* ia64 Stack size/offset = 96 */
/* 246 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure RemoteNext */

/* 248 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 250 */	NdrFcLong( 0x0 ),	/* 0 */
/* 254 */	NdrFcShort( 0x3 ),	/* 3 */
/* 256 */	NdrFcShort( 0x28 ),	/* ia64 Stack size/offset = 40 */
/* 258 */	NdrFcShort( 0x8 ),	/* 8 */
/* 260 */	NdrFcShort( 0x24 ),	/* 36 */
/* 262 */	0x45,		/* Oi2 Flags:  srv must size, has return, has ext, */
			0x4,		/* 4 */
/* 264 */	0xa,		/* 10 */
			0x3,		/* Ext Flags:  new corr desc, clt corr check, */
/* 266 */	NdrFcShort( 0x21 ),	/* 33 */
/* 268 */	NdrFcShort( 0x0 ),	/* 0 */
/* 270 */	NdrFcShort( 0x0 ),	/* 0 */
/* 272 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter celt */

/* 274 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 276 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 278 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter rgVar */

/* 280 */	NdrFcShort( 0x113 ),	/* Flags:  must size, must free, out, simple ref, */
/* 282 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 284 */	NdrFcShort( 0x4ae ),	/* Type Offset=1198 */

	/* Parameter pCeltFetched */

/* 286 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 288 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 290 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 292 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 294 */	NdrFcShort( 0x20 ),	/* ia64 Stack size/offset = 32 */
/* 296 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure Skip */

/* 298 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 300 */	NdrFcLong( 0x0 ),	/* 0 */
/* 304 */	NdrFcShort( 0x4 ),	/* 4 */
/* 306 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 308 */	NdrFcShort( 0x8 ),	/* 8 */
/* 310 */	NdrFcShort( 0x8 ),	/* 8 */
/* 312 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x2,		/* 2 */
/* 314 */	0xa,		/* 10 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 316 */	NdrFcShort( 0x0 ),	/* 0 */
/* 318 */	NdrFcShort( 0x0 ),	/* 0 */
/* 320 */	NdrFcShort( 0x0 ),	/* 0 */
/* 322 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter celt */

/* 324 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 326 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 328 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 330 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 332 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 334 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure Reset */

/* 336 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 338 */	NdrFcLong( 0x0 ),	/* 0 */
/* 342 */	NdrFcShort( 0x5 ),	/* 5 */
/* 344 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 346 */	NdrFcShort( 0x0 ),	/* 0 */
/* 348 */	NdrFcShort( 0x8 ),	/* 8 */
/* 350 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x1,		/* 1 */
/* 352 */	0xa,		/* 10 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 354 */	NdrFcShort( 0x0 ),	/* 0 */
/* 356 */	NdrFcShort( 0x0 ),	/* 0 */
/* 358 */	NdrFcShort( 0x0 ),	/* 0 */
/* 360 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Return value */

/* 362 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 364 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 366 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure Clone */

/* 368 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 370 */	NdrFcLong( 0x0 ),	/* 0 */
/* 374 */	NdrFcShort( 0x6 ),	/* 6 */
/* 376 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 378 */	NdrFcShort( 0x0 ),	/* 0 */
/* 380 */	NdrFcShort( 0x8 ),	/* 8 */
/* 382 */	0x45,		/* Oi2 Flags:  srv must size, has return, has ext, */
			0x2,		/* 2 */
/* 384 */	0xa,		/* 10 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 386 */	NdrFcShort( 0x0 ),	/* 0 */
/* 388 */	NdrFcShort( 0x0 ),	/* 0 */
/* 390 */	NdrFcShort( 0x0 ),	/* 0 */
/* 392 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter ppEnum */

/* 394 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
/* 396 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 398 */	NdrFcShort( 0x4c4 ),	/* Type Offset=1220 */

	/* Return value */

/* 400 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 402 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 404 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure RemoteBind */

/* 406 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 408 */	NdrFcLong( 0x0 ),	/* 0 */
/* 412 */	NdrFcShort( 0x3 ),	/* 3 */
/* 414 */	NdrFcShort( 0x58 ),	/* ia64 Stack size/offset = 88 */
/* 416 */	NdrFcShort( 0xe ),	/* 14 */
/* 418 */	NdrFcShort( 0x40 ),	/* 64 */
/* 420 */	0x47,		/* Oi2 Flags:  srv must size, clt must size, has return, has ext, */
			0xa,		/* 10 */
/* 422 */	0xa,		/* 10 */
			0x3,		/* Ext Flags:  new corr desc, clt corr check, */
/* 424 */	NdrFcShort( 0x89 ),	/* 137 */
/* 426 */	NdrFcShort( 0x0 ),	/* 0 */
/* 428 */	NdrFcShort( 0x0 ),	/* 0 */
/* 430 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter szName */

/* 432 */	NdrFcShort( 0x10b ),	/* Flags:  must size, must free, in, simple ref, */
/* 434 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 436 */	NdrFcShort( 0x4dc ),	/* Type Offset=1244 */

	/* Parameter lHashVal */

/* 438 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 440 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 442 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter wFlags */

/* 444 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 446 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 448 */	0x6,		/* FC_SHORT */
			0x0,		/* 0 */

	/* Parameter ppTInfo */

/* 450 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
/* 452 */	NdrFcShort( 0x20 ),	/* ia64 Stack size/offset = 32 */
/* 454 */	NdrFcShort( 0x6 ),	/* Type Offset=6 */

	/* Parameter pDescKind */

/* 456 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 458 */	NdrFcShort( 0x28 ),	/* ia64 Stack size/offset = 40 */
/* 460 */	0xe,		/* FC_ENUM32 */
			0x0,		/* 0 */

	/* Parameter ppFuncDesc */

/* 462 */	NdrFcShort( 0x2013 ),	/* Flags:  must size, must free, out, srv alloc size=8 */
/* 464 */	NdrFcShort( 0x30 ),	/* ia64 Stack size/offset = 48 */
/* 466 */	NdrFcShort( 0x4e2 ),	/* Type Offset=1250 */

	/* Parameter ppVarDesc */

/* 468 */	NdrFcShort( 0x2013 ),	/* Flags:  must size, must free, out, srv alloc size=8 */
/* 470 */	NdrFcShort( 0x38 ),	/* ia64 Stack size/offset = 56 */
/* 472 */	NdrFcShort( 0x5c2 ),	/* Type Offset=1474 */

	/* Parameter ppTypeComp */

/* 474 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
/* 476 */	NdrFcShort( 0x40 ),	/* ia64 Stack size/offset = 64 */
/* 478 */	NdrFcShort( 0x612 ),	/* Type Offset=1554 */

	/* Parameter pDummy */

/* 480 */	NdrFcShort( 0x6112 ),	/* Flags:  must free, out, simple ref, srv alloc size=24 */
/* 482 */	NdrFcShort( 0x48 ),	/* ia64 Stack size/offset = 72 */
/* 484 */	NdrFcShort( 0x62e ),	/* Type Offset=1582 */

	/* Return value */

/* 486 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 488 */	NdrFcShort( 0x50 ),	/* ia64 Stack size/offset = 80 */
/* 490 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure RemoteBindType */

/* 492 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 494 */	NdrFcLong( 0x0 ),	/* 0 */
/* 498 */	NdrFcShort( 0x4 ),	/* 4 */
/* 500 */	NdrFcShort( 0x28 ),	/* ia64 Stack size/offset = 40 */
/* 502 */	NdrFcShort( 0x8 ),	/* 8 */
/* 504 */	NdrFcShort( 0x8 ),	/* 8 */
/* 506 */	0x47,		/* Oi2 Flags:  srv must size, clt must size, has return, has ext, */
			0x4,		/* 4 */
/* 508 */	0xa,		/* 10 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 510 */	NdrFcShort( 0x0 ),	/* 0 */
/* 512 */	NdrFcShort( 0x0 ),	/* 0 */
/* 514 */	NdrFcShort( 0x0 ),	/* 0 */
/* 516 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter szName */

/* 518 */	NdrFcShort( 0x10b ),	/* Flags:  must size, must free, in, simple ref, */
/* 520 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 522 */	NdrFcShort( 0x4dc ),	/* Type Offset=1244 */

	/* Parameter lHashVal */

/* 524 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 526 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 528 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter ppTInfo */

/* 530 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
/* 532 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 534 */	NdrFcShort( 0x6 ),	/* Type Offset=6 */

	/* Return value */

/* 536 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 538 */	NdrFcShort( 0x20 ),	/* ia64 Stack size/offset = 32 */
/* 540 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure RemoteGetTypeAttr */

/* 542 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 544 */	NdrFcLong( 0x0 ),	/* 0 */
/* 548 */	NdrFcShort( 0x3 ),	/* 3 */
/* 550 */	NdrFcShort( 0x20 ),	/* ia64 Stack size/offset = 32 */
/* 552 */	NdrFcShort( 0x0 ),	/* 0 */
/* 554 */	NdrFcShort( 0x24 ),	/* 36 */
/* 556 */	0x45,		/* Oi2 Flags:  srv must size, has return, has ext, */
			0x3,		/* 3 */
/* 558 */	0xa,		/* 10 */
			0x3,		/* Ext Flags:  new corr desc, clt corr check, */
/* 560 */	NdrFcShort( 0x2 ),	/* 2 */
/* 562 */	NdrFcShort( 0x0 ),	/* 0 */
/* 564 */	NdrFcShort( 0x0 ),	/* 0 */
/* 566 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter ppTypeAttr */

/* 568 */	NdrFcShort( 0x2013 ),	/* Flags:  must size, must free, out, srv alloc size=8 */
/* 570 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 572 */	NdrFcShort( 0x638 ),	/* Type Offset=1592 */

	/* Parameter pDummy */

/* 574 */	NdrFcShort( 0x6112 ),	/* Flags:  must free, out, simple ref, srv alloc size=24 */
/* 576 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 578 */	NdrFcShort( 0x67a ),	/* Type Offset=1658 */

	/* Return value */

/* 580 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 582 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 584 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetTypeComp */

/* 586 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 588 */	NdrFcLong( 0x0 ),	/* 0 */
/* 592 */	NdrFcShort( 0x4 ),	/* 4 */
/* 594 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 596 */	NdrFcShort( 0x0 ),	/* 0 */
/* 598 */	NdrFcShort( 0x8 ),	/* 8 */
/* 600 */	0x45,		/* Oi2 Flags:  srv must size, has return, has ext, */
			0x2,		/* 2 */
/* 602 */	0xa,		/* 10 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 604 */	NdrFcShort( 0x0 ),	/* 0 */
/* 606 */	NdrFcShort( 0x0 ),	/* 0 */
/* 608 */	NdrFcShort( 0x0 ),	/* 0 */
/* 610 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter ppTComp */

/* 612 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
/* 614 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 616 */	NdrFcShort( 0x612 ),	/* Type Offset=1554 */

	/* Return value */

/* 618 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 620 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 622 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure RemoteGetFuncDesc */

/* 624 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 626 */	NdrFcLong( 0x0 ),	/* 0 */
/* 630 */	NdrFcShort( 0x5 ),	/* 5 */
/* 632 */	NdrFcShort( 0x28 ),	/* ia64 Stack size/offset = 40 */
/* 634 */	NdrFcShort( 0x8 ),	/* 8 */
/* 636 */	NdrFcShort( 0x24 ),	/* 36 */
/* 638 */	0x45,		/* Oi2 Flags:  srv must size, has return, has ext, */
			0x4,		/* 4 */
/* 640 */	0xa,		/* 10 */
			0x3,		/* Ext Flags:  new corr desc, clt corr check, */
/* 642 */	NdrFcShort( 0x46 ),	/* 70 */
/* 644 */	NdrFcShort( 0x0 ),	/* 0 */
/* 646 */	NdrFcShort( 0x0 ),	/* 0 */
/* 648 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter index */

/* 650 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 652 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 654 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter ppFuncDesc */

/* 656 */	NdrFcShort( 0x2013 ),	/* Flags:  must size, must free, out, srv alloc size=8 */
/* 658 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 660 */	NdrFcShort( 0x4e2 ),	/* Type Offset=1250 */

	/* Parameter pDummy */

/* 662 */	NdrFcShort( 0x6112 ),	/* Flags:  must free, out, simple ref, srv alloc size=24 */
/* 664 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 666 */	NdrFcShort( 0x68a ),	/* Type Offset=1674 */

	/* Return value */

/* 668 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 670 */	NdrFcShort( 0x20 ),	/* ia64 Stack size/offset = 32 */
/* 672 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure RemoteGetVarDesc */

/* 674 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 676 */	NdrFcLong( 0x0 ),	/* 0 */
/* 680 */	NdrFcShort( 0x6 ),	/* 6 */
/* 682 */	NdrFcShort( 0x28 ),	/* ia64 Stack size/offset = 40 */
/* 684 */	NdrFcShort( 0x8 ),	/* 8 */
/* 686 */	NdrFcShort( 0x24 ),	/* 36 */
/* 688 */	0x45,		/* Oi2 Flags:  srv must size, has return, has ext, */
			0x4,		/* 4 */
/* 690 */	0xa,		/* 10 */
			0x3,		/* Ext Flags:  new corr desc, clt corr check, */
/* 692 */	NdrFcShort( 0x43 ),	/* 67 */
/* 694 */	NdrFcShort( 0x0 ),	/* 0 */
/* 696 */	NdrFcShort( 0x0 ),	/* 0 */
/* 698 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter index */

/* 700 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 702 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 704 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter ppVarDesc */

/* 706 */	NdrFcShort( 0x2013 ),	/* Flags:  must size, must free, out, srv alloc size=8 */
/* 708 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 710 */	NdrFcShort( 0x5c2 ),	/* Type Offset=1474 */

	/* Parameter pDummy */

/* 712 */	NdrFcShort( 0x6112 ),	/* Flags:  must free, out, simple ref, srv alloc size=24 */
/* 714 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 716 */	NdrFcShort( 0x69a ),	/* Type Offset=1690 */

	/* Return value */

/* 718 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 720 */	NdrFcShort( 0x20 ),	/* ia64 Stack size/offset = 32 */
/* 722 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure RemoteGetNames */

/* 724 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 726 */	NdrFcLong( 0x0 ),	/* 0 */
/* 730 */	NdrFcShort( 0x7 ),	/* 7 */
/* 732 */	NdrFcShort( 0x30 ),	/* ia64 Stack size/offset = 48 */
/* 734 */	NdrFcShort( 0x10 ),	/* 16 */
/* 736 */	NdrFcShort( 0x24 ),	/* 36 */
/* 738 */	0x45,		/* Oi2 Flags:  srv must size, has return, has ext, */
			0x5,		/* 5 */
/* 740 */	0xa,		/* 10 */
			0x3,		/* Ext Flags:  new corr desc, clt corr check, */
/* 742 */	NdrFcShort( 0x2 ),	/* 2 */
/* 744 */	NdrFcShort( 0x0 ),	/* 0 */
/* 746 */	NdrFcShort( 0x0 ),	/* 0 */
/* 748 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter memid */

/* 750 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 752 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 754 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter rgBstrNames */

/* 756 */	NdrFcShort( 0x113 ),	/* Flags:  must size, must free, out, simple ref, */
/* 758 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 760 */	NdrFcShort( 0x6a8 ),	/* Type Offset=1704 */

	/* Parameter cMaxNames */

/* 762 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 764 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 766 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter pcNames */

/* 768 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 770 */	NdrFcShort( 0x20 ),	/* ia64 Stack size/offset = 32 */
/* 772 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 774 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 776 */	NdrFcShort( 0x28 ),	/* ia64 Stack size/offset = 40 */
/* 778 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetRefTypeOfImplType */

/* 780 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 782 */	NdrFcLong( 0x0 ),	/* 0 */
/* 786 */	NdrFcShort( 0x8 ),	/* 8 */
/* 788 */	NdrFcShort( 0x20 ),	/* ia64 Stack size/offset = 32 */
/* 790 */	NdrFcShort( 0x8 ),	/* 8 */
/* 792 */	NdrFcShort( 0x24 ),	/* 36 */
/* 794 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x3,		/* 3 */
/* 796 */	0xa,		/* 10 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 798 */	NdrFcShort( 0x0 ),	/* 0 */
/* 800 */	NdrFcShort( 0x0 ),	/* 0 */
/* 802 */	NdrFcShort( 0x0 ),	/* 0 */
/* 804 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter index */

/* 806 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 808 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 810 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter pRefType */

/* 812 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 814 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 816 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 818 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 820 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 822 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetImplTypeFlags */

/* 824 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 826 */	NdrFcLong( 0x0 ),	/* 0 */
/* 830 */	NdrFcShort( 0x9 ),	/* 9 */
/* 832 */	NdrFcShort( 0x20 ),	/* ia64 Stack size/offset = 32 */
/* 834 */	NdrFcShort( 0x8 ),	/* 8 */
/* 836 */	NdrFcShort( 0x24 ),	/* 36 */
/* 838 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x3,		/* 3 */
/* 840 */	0xa,		/* 10 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 842 */	NdrFcShort( 0x0 ),	/* 0 */
/* 844 */	NdrFcShort( 0x0 ),	/* 0 */
/* 846 */	NdrFcShort( 0x0 ),	/* 0 */
/* 848 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter index */

/* 850 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 852 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 854 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter pImplTypeFlags */

/* 856 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 858 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 860 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 862 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 864 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 866 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure LocalGetIDsOfNames */

/* 868 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 870 */	NdrFcLong( 0x0 ),	/* 0 */
/* 874 */	NdrFcShort( 0xa ),	/* 10 */
/* 876 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 878 */	NdrFcShort( 0x0 ),	/* 0 */
/* 880 */	NdrFcShort( 0x8 ),	/* 8 */
/* 882 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x1,		/* 1 */
/* 884 */	0xa,		/* 10 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 886 */	NdrFcShort( 0x0 ),	/* 0 */
/* 888 */	NdrFcShort( 0x0 ),	/* 0 */
/* 890 */	NdrFcShort( 0x0 ),	/* 0 */
/* 892 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Return value */

/* 894 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 896 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 898 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure LocalInvoke */

/* 900 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 902 */	NdrFcLong( 0x0 ),	/* 0 */
/* 906 */	NdrFcShort( 0xb ),	/* 11 */
/* 908 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 910 */	NdrFcShort( 0x0 ),	/* 0 */
/* 912 */	NdrFcShort( 0x8 ),	/* 8 */
/* 914 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x1,		/* 1 */
/* 916 */	0xa,		/* 10 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 918 */	NdrFcShort( 0x0 ),	/* 0 */
/* 920 */	NdrFcShort( 0x0 ),	/* 0 */
/* 922 */	NdrFcShort( 0x0 ),	/* 0 */
/* 924 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Return value */

/* 926 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 928 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 930 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure RemoteGetDocumentation */

/* 932 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 934 */	NdrFcLong( 0x0 ),	/* 0 */
/* 938 */	NdrFcShort( 0xc ),	/* 12 */
/* 940 */	NdrFcShort( 0x40 ),	/* ia64 Stack size/offset = 64 */
/* 942 */	NdrFcShort( 0x10 ),	/* 16 */
/* 944 */	NdrFcShort( 0x24 ),	/* 36 */
/* 946 */	0x45,		/* Oi2 Flags:  srv must size, has return, has ext, */
			0x7,		/* 7 */
/* 948 */	0xa,		/* 10 */
			0x3,		/* Ext Flags:  new corr desc, clt corr check, */
/* 950 */	NdrFcShort( 0x3 ),	/* 3 */
/* 952 */	NdrFcShort( 0x0 ),	/* 0 */
/* 954 */	NdrFcShort( 0x0 ),	/* 0 */
/* 956 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter memid */

/* 958 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 960 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 962 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter refPtrFlags */

/* 964 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 966 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 968 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter pBstrName */

/* 970 */	NdrFcShort( 0x2113 ),	/* Flags:  must size, must free, out, simple ref, srv alloc size=8 */
/* 972 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 974 */	NdrFcShort( 0x45a ),	/* Type Offset=1114 */

	/* Parameter pBstrDocString */

/* 976 */	NdrFcShort( 0x2113 ),	/* Flags:  must size, must free, out, simple ref, srv alloc size=8 */
/* 978 */	NdrFcShort( 0x20 ),	/* ia64 Stack size/offset = 32 */
/* 980 */	NdrFcShort( 0x45a ),	/* Type Offset=1114 */

	/* Parameter pdwHelpContext */

/* 982 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 984 */	NdrFcShort( 0x28 ),	/* ia64 Stack size/offset = 40 */
/* 986 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter pBstrHelpFile */

/* 988 */	NdrFcShort( 0x2113 ),	/* Flags:  must size, must free, out, simple ref, srv alloc size=8 */
/* 990 */	NdrFcShort( 0x30 ),	/* ia64 Stack size/offset = 48 */
/* 992 */	NdrFcShort( 0x45a ),	/* Type Offset=1114 */

	/* Return value */

/* 994 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 996 */	NdrFcShort( 0x38 ),	/* ia64 Stack size/offset = 56 */
/* 998 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure RemoteGetDllEntry */

/* 1000 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 1002 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1006 */	NdrFcShort( 0xd ),	/* 13 */
/* 1008 */	NdrFcShort( 0x40 ),	/* ia64 Stack size/offset = 64 */
/* 1010 */	NdrFcShort( 0x18 ),	/* 24 */
/* 1012 */	NdrFcShort( 0x22 ),	/* 34 */
/* 1014 */	0x45,		/* Oi2 Flags:  srv must size, has return, has ext, */
			0x7,		/* 7 */
/* 1016 */	0xa,		/* 10 */
			0x3,		/* Ext Flags:  new corr desc, clt corr check, */
/* 1018 */	NdrFcShort( 0x2 ),	/* 2 */
/* 1020 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1022 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1024 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter memid */

/* 1026 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 1028 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 1030 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter invKind */

/* 1032 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 1034 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 1036 */	0xe,		/* FC_ENUM32 */
			0x0,		/* 0 */

	/* Parameter refPtrFlags */

/* 1038 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 1040 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 1042 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter pBstrDllName */

/* 1044 */	NdrFcShort( 0x2113 ),	/* Flags:  must size, must free, out, simple ref, srv alloc size=8 */
/* 1046 */	NdrFcShort( 0x20 ),	/* ia64 Stack size/offset = 32 */
/* 1048 */	NdrFcShort( 0x45a ),	/* Type Offset=1114 */

	/* Parameter pBstrName */

/* 1050 */	NdrFcShort( 0x2113 ),	/* Flags:  must size, must free, out, simple ref, srv alloc size=8 */
/* 1052 */	NdrFcShort( 0x28 ),	/* ia64 Stack size/offset = 40 */
/* 1054 */	NdrFcShort( 0x45a ),	/* Type Offset=1114 */

	/* Parameter pwOrdinal */

/* 1056 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 1058 */	NdrFcShort( 0x30 ),	/* ia64 Stack size/offset = 48 */
/* 1060 */	0x6,		/* FC_SHORT */
			0x0,		/* 0 */

	/* Return value */

/* 1062 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1064 */	NdrFcShort( 0x38 ),	/* ia64 Stack size/offset = 56 */
/* 1066 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetRefTypeInfo */

/* 1068 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 1070 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1074 */	NdrFcShort( 0xe ),	/* 14 */
/* 1076 */	NdrFcShort( 0x20 ),	/* ia64 Stack size/offset = 32 */
/* 1078 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1080 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1082 */	0x45,		/* Oi2 Flags:  srv must size, has return, has ext, */
			0x3,		/* 3 */
/* 1084 */	0xa,		/* 10 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 1086 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1088 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1090 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1092 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter hRefType */

/* 1094 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 1096 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 1098 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter ppTInfo */

/* 1100 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
/* 1102 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 1104 */	NdrFcShort( 0x6 ),	/* Type Offset=6 */

	/* Return value */

/* 1106 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1108 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 1110 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure LocalAddressOfMember */

/* 1112 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 1114 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1118 */	NdrFcShort( 0xf ),	/* 15 */
/* 1120 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 1122 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1124 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1126 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x1,		/* 1 */
/* 1128 */	0xa,		/* 10 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 1130 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1132 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1134 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1136 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Return value */

/* 1138 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1140 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 1142 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure RemoteCreateInstance */

/* 1144 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 1146 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1150 */	NdrFcShort( 0x10 ),	/* 16 */
/* 1152 */	NdrFcShort( 0x20 ),	/* ia64 Stack size/offset = 32 */
/* 1154 */	NdrFcShort( 0x44 ),	/* 68 */
/* 1156 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1158 */	0x45,		/* Oi2 Flags:  srv must size, has return, has ext, */
			0x3,		/* 3 */
/* 1160 */	0xa,		/* 10 */
			0x3,		/* Ext Flags:  new corr desc, clt corr check, */
/* 1162 */	NdrFcShort( 0x1 ),	/* 1 */
/* 1164 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1166 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1168 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter riid */

/* 1170 */	NdrFcShort( 0x10a ),	/* Flags:  must free, in, simple ref, */
/* 1172 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 1174 */	NdrFcShort( 0x26 ),	/* Type Offset=38 */

	/* Parameter ppvObj */

/* 1176 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
/* 1178 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 1180 */	NdrFcShort( 0x6c6 ),	/* Type Offset=1734 */

	/* Return value */

/* 1182 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1184 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 1186 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetMops */

/* 1188 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 1190 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1194 */	NdrFcShort( 0x11 ),	/* 17 */
/* 1196 */	NdrFcShort( 0x20 ),	/* ia64 Stack size/offset = 32 */
/* 1198 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1200 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1202 */	0x45,		/* Oi2 Flags:  srv must size, has return, has ext, */
			0x3,		/* 3 */
/* 1204 */	0xa,		/* 10 */
			0x3,		/* Ext Flags:  new corr desc, clt corr check, */
/* 1206 */	NdrFcShort( 0x1 ),	/* 1 */
/* 1208 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1210 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1212 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter memid */

/* 1214 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 1216 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 1218 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter pBstrMops */

/* 1220 */	NdrFcShort( 0x2113 ),	/* Flags:  must size, must free, out, simple ref, srv alloc size=8 */
/* 1222 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 1224 */	NdrFcShort( 0x45a ),	/* Type Offset=1114 */

	/* Return value */

/* 1226 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1228 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 1230 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure RemoteGetContainingTypeLib */

/* 1232 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 1234 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1238 */	NdrFcShort( 0x12 ),	/* 18 */
/* 1240 */	NdrFcShort( 0x20 ),	/* ia64 Stack size/offset = 32 */
/* 1242 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1244 */	NdrFcShort( 0x24 ),	/* 36 */
/* 1246 */	0x45,		/* Oi2 Flags:  srv must size, has return, has ext, */
			0x3,		/* 3 */
/* 1248 */	0xa,		/* 10 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 1250 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1252 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1254 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1256 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter ppTLib */

/* 1258 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
/* 1260 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 1262 */	NdrFcShort( 0x6d2 ),	/* Type Offset=1746 */

	/* Parameter pIndex */

/* 1264 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 1266 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 1268 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 1270 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1272 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 1274 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure LocalReleaseTypeAttr */

/* 1276 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 1278 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1282 */	NdrFcShort( 0x13 ),	/* 19 */
/* 1284 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 1286 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1288 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1290 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x1,		/* 1 */
/* 1292 */	0xa,		/* 10 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 1294 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1296 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1298 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1300 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Return value */

/* 1302 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1304 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 1306 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure LocalReleaseFuncDesc */

/* 1308 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 1310 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1314 */	NdrFcShort( 0x14 ),	/* 20 */
/* 1316 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 1318 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1320 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1322 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x1,		/* 1 */
/* 1324 */	0xa,		/* 10 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 1326 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1328 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1330 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1332 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Return value */

/* 1334 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1336 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 1338 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure LocalReleaseVarDesc */

/* 1340 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 1342 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1346 */	NdrFcShort( 0x15 ),	/* 21 */
/* 1348 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 1350 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1352 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1354 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x1,		/* 1 */
/* 1356 */	0xa,		/* 10 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 1358 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1360 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1362 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1364 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Return value */

/* 1366 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1368 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 1370 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetTypeKind */

/* 1372 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 1374 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1378 */	NdrFcShort( 0x16 ),	/* 22 */
/* 1380 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 1382 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1384 */	NdrFcShort( 0x24 ),	/* 36 */
/* 1386 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x2,		/* 2 */
/* 1388 */	0xa,		/* 10 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 1390 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1392 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1394 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1396 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pTypeKind */

/* 1398 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 1400 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 1402 */	0xe,		/* FC_ENUM32 */
			0x0,		/* 0 */

	/* Return value */

/* 1404 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1406 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 1408 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetTypeFlags */

/* 1410 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 1412 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1416 */	NdrFcShort( 0x17 ),	/* 23 */
/* 1418 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 1420 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1422 */	NdrFcShort( 0x24 ),	/* 36 */
/* 1424 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x2,		/* 2 */
/* 1426 */	0xa,		/* 10 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 1428 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1430 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1432 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1434 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pTypeFlags */

/* 1436 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 1438 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 1440 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 1442 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1444 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 1446 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetFuncIndexOfMemId */

/* 1448 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 1450 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1454 */	NdrFcShort( 0x18 ),	/* 24 */
/* 1456 */	NdrFcShort( 0x28 ),	/* ia64 Stack size/offset = 40 */
/* 1458 */	NdrFcShort( 0x10 ),	/* 16 */
/* 1460 */	NdrFcShort( 0x24 ),	/* 36 */
/* 1462 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x4,		/* 4 */
/* 1464 */	0xa,		/* 10 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 1466 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1468 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1470 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1472 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter memid */

/* 1474 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 1476 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 1478 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter invKind */

/* 1480 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 1482 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 1484 */	0xe,		/* FC_ENUM32 */
			0x0,		/* 0 */

	/* Parameter pFuncIndex */

/* 1486 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 1488 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 1490 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 1492 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1494 */	NdrFcShort( 0x20 ),	/* ia64 Stack size/offset = 32 */
/* 1496 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetVarIndexOfMemId */

/* 1498 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 1500 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1504 */	NdrFcShort( 0x19 ),	/* 25 */
/* 1506 */	NdrFcShort( 0x20 ),	/* ia64 Stack size/offset = 32 */
/* 1508 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1510 */	NdrFcShort( 0x24 ),	/* 36 */
/* 1512 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x3,		/* 3 */
/* 1514 */	0xa,		/* 10 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 1516 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1518 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1520 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1522 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter memid */

/* 1524 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 1526 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 1528 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter pVarIndex */

/* 1530 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 1532 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 1534 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 1536 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1538 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 1540 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetCustData */

/* 1542 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 1544 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1548 */	NdrFcShort( 0x1a ),	/* 26 */
/* 1550 */	NdrFcShort( 0x20 ),	/* ia64 Stack size/offset = 32 */
/* 1552 */	NdrFcShort( 0x44 ),	/* 68 */
/* 1554 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1556 */	0x45,		/* Oi2 Flags:  srv must size, has return, has ext, */
			0x3,		/* 3 */
/* 1558 */	0xa,		/* 10 */
			0x3,		/* Ext Flags:  new corr desc, clt corr check, */
/* 1560 */	NdrFcShort( 0x20 ),	/* 32 */
/* 1562 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1564 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1566 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter guid */

/* 1568 */	NdrFcShort( 0x10a ),	/* Flags:  must free, in, simple ref, */
/* 1570 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 1572 */	NdrFcShort( 0x26 ),	/* Type Offset=38 */

	/* Parameter pVarVal */

/* 1574 */	NdrFcShort( 0x6113 ),	/* Flags:  must size, must free, out, simple ref, srv alloc size=24 */
/* 1576 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 1578 */	NdrFcShort( 0x448 ),	/* Type Offset=1096 */

	/* Return value */

/* 1580 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1582 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 1584 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetFuncCustData */

/* 1586 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 1588 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1592 */	NdrFcShort( 0x1b ),	/* 27 */
/* 1594 */	NdrFcShort( 0x28 ),	/* ia64 Stack size/offset = 40 */
/* 1596 */	NdrFcShort( 0x4c ),	/* 76 */
/* 1598 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1600 */	0x45,		/* Oi2 Flags:  srv must size, has return, has ext, */
			0x4,		/* 4 */
/* 1602 */	0xa,		/* 10 */
			0x3,		/* Ext Flags:  new corr desc, clt corr check, */
/* 1604 */	NdrFcShort( 0x20 ),	/* 32 */
/* 1606 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1608 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1610 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter index */

/* 1612 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 1614 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 1616 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter guid */

/* 1618 */	NdrFcShort( 0x10a ),	/* Flags:  must free, in, simple ref, */
/* 1620 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 1622 */	NdrFcShort( 0x26 ),	/* Type Offset=38 */

	/* Parameter pVarVal */

/* 1624 */	NdrFcShort( 0x6113 ),	/* Flags:  must size, must free, out, simple ref, srv alloc size=24 */
/* 1626 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 1628 */	NdrFcShort( 0x448 ),	/* Type Offset=1096 */

	/* Return value */

/* 1630 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1632 */	NdrFcShort( 0x20 ),	/* ia64 Stack size/offset = 32 */
/* 1634 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetParamCustData */

/* 1636 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 1638 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1642 */	NdrFcShort( 0x1c ),	/* 28 */
/* 1644 */	NdrFcShort( 0x30 ),	/* ia64 Stack size/offset = 48 */
/* 1646 */	NdrFcShort( 0x54 ),	/* 84 */
/* 1648 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1650 */	0x45,		/* Oi2 Flags:  srv must size, has return, has ext, */
			0x5,		/* 5 */
/* 1652 */	0xa,		/* 10 */
			0x3,		/* Ext Flags:  new corr desc, clt corr check, */
/* 1654 */	NdrFcShort( 0x20 ),	/* 32 */
/* 1656 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1658 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1660 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter indexFunc */

/* 1662 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 1664 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 1666 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter indexParam */

/* 1668 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 1670 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 1672 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter guid */

/* 1674 */	NdrFcShort( 0x10a ),	/* Flags:  must free, in, simple ref, */
/* 1676 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 1678 */	NdrFcShort( 0x26 ),	/* Type Offset=38 */

	/* Parameter pVarVal */

/* 1680 */	NdrFcShort( 0x6113 ),	/* Flags:  must size, must free, out, simple ref, srv alloc size=24 */
/* 1682 */	NdrFcShort( 0x20 ),	/* ia64 Stack size/offset = 32 */
/* 1684 */	NdrFcShort( 0x448 ),	/* Type Offset=1096 */

	/* Return value */

/* 1686 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1688 */	NdrFcShort( 0x28 ),	/* ia64 Stack size/offset = 40 */
/* 1690 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetVarCustData */

/* 1692 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 1694 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1698 */	NdrFcShort( 0x1d ),	/* 29 */
/* 1700 */	NdrFcShort( 0x28 ),	/* ia64 Stack size/offset = 40 */
/* 1702 */	NdrFcShort( 0x4c ),	/* 76 */
/* 1704 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1706 */	0x45,		/* Oi2 Flags:  srv must size, has return, has ext, */
			0x4,		/* 4 */
/* 1708 */	0xa,		/* 10 */
			0x3,		/* Ext Flags:  new corr desc, clt corr check, */
/* 1710 */	NdrFcShort( 0x20 ),	/* 32 */
/* 1712 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1714 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1716 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter index */

/* 1718 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 1720 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 1722 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter guid */

/* 1724 */	NdrFcShort( 0x10a ),	/* Flags:  must free, in, simple ref, */
/* 1726 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 1728 */	NdrFcShort( 0x26 ),	/* Type Offset=38 */

	/* Parameter pVarVal */

/* 1730 */	NdrFcShort( 0x6113 ),	/* Flags:  must size, must free, out, simple ref, srv alloc size=24 */
/* 1732 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 1734 */	NdrFcShort( 0x448 ),	/* Type Offset=1096 */

	/* Return value */

/* 1736 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1738 */	NdrFcShort( 0x20 ),	/* ia64 Stack size/offset = 32 */
/* 1740 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetImplTypeCustData */

/* 1742 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 1744 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1748 */	NdrFcShort( 0x1e ),	/* 30 */
/* 1750 */	NdrFcShort( 0x28 ),	/* ia64 Stack size/offset = 40 */
/* 1752 */	NdrFcShort( 0x4c ),	/* 76 */
/* 1754 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1756 */	0x45,		/* Oi2 Flags:  srv must size, has return, has ext, */
			0x4,		/* 4 */
/* 1758 */	0xa,		/* 10 */
			0x3,		/* Ext Flags:  new corr desc, clt corr check, */
/* 1760 */	NdrFcShort( 0x20 ),	/* 32 */
/* 1762 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1764 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1766 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter index */

/* 1768 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 1770 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 1772 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter guid */

/* 1774 */	NdrFcShort( 0x10a ),	/* Flags:  must free, in, simple ref, */
/* 1776 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 1778 */	NdrFcShort( 0x26 ),	/* Type Offset=38 */

	/* Parameter pVarVal */

/* 1780 */	NdrFcShort( 0x6113 ),	/* Flags:  must size, must free, out, simple ref, srv alloc size=24 */
/* 1782 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 1784 */	NdrFcShort( 0x448 ),	/* Type Offset=1096 */

	/* Return value */

/* 1786 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1788 */	NdrFcShort( 0x20 ),	/* ia64 Stack size/offset = 32 */
/* 1790 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure RemoteGetDocumentation2 */

/* 1792 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 1794 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1798 */	NdrFcShort( 0x1f ),	/* 31 */
/* 1800 */	NdrFcShort( 0x40 ),	/* ia64 Stack size/offset = 64 */
/* 1802 */	NdrFcShort( 0x18 ),	/* 24 */
/* 1804 */	NdrFcShort( 0x24 ),	/* 36 */
/* 1806 */	0x45,		/* Oi2 Flags:  srv must size, has return, has ext, */
			0x7,		/* 7 */
/* 1808 */	0xa,		/* 10 */
			0x3,		/* Ext Flags:  new corr desc, clt corr check, */
/* 1810 */	NdrFcShort( 0x2 ),	/* 2 */
/* 1812 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1814 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1816 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter memid */

/* 1818 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 1820 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 1822 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter lcid */

/* 1824 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 1826 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 1828 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter refPtrFlags */

/* 1830 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 1832 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 1834 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter pbstrHelpString */

/* 1836 */	NdrFcShort( 0x2113 ),	/* Flags:  must size, must free, out, simple ref, srv alloc size=8 */
/* 1838 */	NdrFcShort( 0x20 ),	/* ia64 Stack size/offset = 32 */
/* 1840 */	NdrFcShort( 0x45a ),	/* Type Offset=1114 */

	/* Parameter pdwHelpStringContext */

/* 1842 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 1844 */	NdrFcShort( 0x28 ),	/* ia64 Stack size/offset = 40 */
/* 1846 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter pbstrHelpStringDll */

/* 1848 */	NdrFcShort( 0x2113 ),	/* Flags:  must size, must free, out, simple ref, srv alloc size=8 */
/* 1850 */	NdrFcShort( 0x30 ),	/* ia64 Stack size/offset = 48 */
/* 1852 */	NdrFcShort( 0x45a ),	/* Type Offset=1114 */

	/* Return value */

/* 1854 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1856 */	NdrFcShort( 0x38 ),	/* ia64 Stack size/offset = 56 */
/* 1858 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetAllCustData */

/* 1860 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 1862 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1866 */	NdrFcShort( 0x20 ),	/* 32 */
/* 1868 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 1870 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1872 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1874 */	0x45,		/* Oi2 Flags:  srv must size, has return, has ext, */
			0x2,		/* 2 */
/* 1876 */	0xa,		/* 10 */
			0x3,		/* Ext Flags:  new corr desc, clt corr check, */
/* 1878 */	NdrFcShort( 0x21 ),	/* 33 */
/* 1880 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1882 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1884 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pCustData */

/* 1886 */	NdrFcShort( 0x4113 ),	/* Flags:  must size, must free, out, simple ref, srv alloc size=16 */
/* 1888 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 1890 */	NdrFcShort( 0x714 ),	/* Type Offset=1812 */

	/* Return value */

/* 1892 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1894 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 1896 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetAllFuncCustData */

/* 1898 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 1900 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1904 */	NdrFcShort( 0x21 ),	/* 33 */
/* 1906 */	NdrFcShort( 0x20 ),	/* ia64 Stack size/offset = 32 */
/* 1908 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1910 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1912 */	0x45,		/* Oi2 Flags:  srv must size, has return, has ext, */
			0x3,		/* 3 */
/* 1914 */	0xa,		/* 10 */
			0x3,		/* Ext Flags:  new corr desc, clt corr check, */
/* 1916 */	NdrFcShort( 0x21 ),	/* 33 */
/* 1918 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1920 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1922 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter index */

/* 1924 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 1926 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 1928 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter pCustData */

/* 1930 */	NdrFcShort( 0x4113 ),	/* Flags:  must size, must free, out, simple ref, srv alloc size=16 */
/* 1932 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 1934 */	NdrFcShort( 0x714 ),	/* Type Offset=1812 */

	/* Return value */

/* 1936 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1938 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 1940 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetAllParamCustData */

/* 1942 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 1944 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1948 */	NdrFcShort( 0x22 ),	/* 34 */
/* 1950 */	NdrFcShort( 0x28 ),	/* ia64 Stack size/offset = 40 */
/* 1952 */	NdrFcShort( 0x10 ),	/* 16 */
/* 1954 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1956 */	0x45,		/* Oi2 Flags:  srv must size, has return, has ext, */
			0x4,		/* 4 */
/* 1958 */	0xa,		/* 10 */
			0x3,		/* Ext Flags:  new corr desc, clt corr check, */
/* 1960 */	NdrFcShort( 0x21 ),	/* 33 */
/* 1962 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1964 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1966 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter indexFunc */

/* 1968 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 1970 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 1972 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter indexParam */

/* 1974 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 1976 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 1978 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter pCustData */

/* 1980 */	NdrFcShort( 0x4113 ),	/* Flags:  must size, must free, out, simple ref, srv alloc size=16 */
/* 1982 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 1984 */	NdrFcShort( 0x714 ),	/* Type Offset=1812 */

	/* Return value */

/* 1986 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1988 */	NdrFcShort( 0x20 ),	/* ia64 Stack size/offset = 32 */
/* 1990 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetAllVarCustData */

/* 1992 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 1994 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1998 */	NdrFcShort( 0x23 ),	/* 35 */
/* 2000 */	NdrFcShort( 0x20 ),	/* ia64 Stack size/offset = 32 */
/* 2002 */	NdrFcShort( 0x8 ),	/* 8 */
/* 2004 */	NdrFcShort( 0x8 ),	/* 8 */
/* 2006 */	0x45,		/* Oi2 Flags:  srv must size, has return, has ext, */
			0x3,		/* 3 */
/* 2008 */	0xa,		/* 10 */
			0x3,		/* Ext Flags:  new corr desc, clt corr check, */
/* 2010 */	NdrFcShort( 0x21 ),	/* 33 */
/* 2012 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2014 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2016 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter index */

/* 2018 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 2020 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 2022 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter pCustData */

/* 2024 */	NdrFcShort( 0x4113 ),	/* Flags:  must size, must free, out, simple ref, srv alloc size=16 */
/* 2026 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 2028 */	NdrFcShort( 0x714 ),	/* Type Offset=1812 */

	/* Return value */

/* 2030 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 2032 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 2034 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetAllImplTypeCustData */

/* 2036 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 2038 */	NdrFcLong( 0x0 ),	/* 0 */
/* 2042 */	NdrFcShort( 0x24 ),	/* 36 */
/* 2044 */	NdrFcShort( 0x20 ),	/* ia64 Stack size/offset = 32 */
/* 2046 */	NdrFcShort( 0x8 ),	/* 8 */
/* 2048 */	NdrFcShort( 0x8 ),	/* 8 */
/* 2050 */	0x45,		/* Oi2 Flags:  srv must size, has return, has ext, */
			0x3,		/* 3 */
/* 2052 */	0xa,		/* 10 */
			0x3,		/* Ext Flags:  new corr desc, clt corr check, */
/* 2054 */	NdrFcShort( 0x21 ),	/* 33 */
/* 2056 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2058 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2060 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter index */

/* 2062 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 2064 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 2066 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter pCustData */

/* 2068 */	NdrFcShort( 0x4113 ),	/* Flags:  must size, must free, out, simple ref, srv alloc size=16 */
/* 2070 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 2072 */	NdrFcShort( 0x714 ),	/* Type Offset=1812 */

	/* Return value */

/* 2074 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 2076 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 2078 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetTypeInfo */

/* 2080 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 2082 */	NdrFcLong( 0x0 ),	/* 0 */
/* 2086 */	NdrFcShort( 0x4 ),	/* 4 */
/* 2088 */	NdrFcShort( 0x20 ),	/* ia64 Stack size/offset = 32 */
/* 2090 */	NdrFcShort( 0x8 ),	/* 8 */
/* 2092 */	NdrFcShort( 0x8 ),	/* 8 */
/* 2094 */	0x45,		/* Oi2 Flags:  srv must size, has return, has ext, */
			0x3,		/* 3 */
/* 2096 */	0xa,		/* 10 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 2098 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2100 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2102 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2104 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter index */

/* 2106 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 2108 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 2110 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter ppTInfo */

/* 2112 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
/* 2114 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 2116 */	NdrFcShort( 0x6 ),	/* Type Offset=6 */

	/* Return value */

/* 2118 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 2120 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 2122 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetTypeInfoType */

/* 2124 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 2126 */	NdrFcLong( 0x0 ),	/* 0 */
/* 2130 */	NdrFcShort( 0x5 ),	/* 5 */
/* 2132 */	NdrFcShort( 0x20 ),	/* ia64 Stack size/offset = 32 */
/* 2134 */	NdrFcShort( 0x8 ),	/* 8 */
/* 2136 */	NdrFcShort( 0x24 ),	/* 36 */
/* 2138 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x3,		/* 3 */
/* 2140 */	0xa,		/* 10 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 2142 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2144 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2146 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2148 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter index */

/* 2150 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 2152 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 2154 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter pTKind */

/* 2156 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 2158 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 2160 */	0xe,		/* FC_ENUM32 */
			0x0,		/* 0 */

	/* Return value */

/* 2162 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 2164 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 2166 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetTypeInfoOfGuid */

/* 2168 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 2170 */	NdrFcLong( 0x0 ),	/* 0 */
/* 2174 */	NdrFcShort( 0x6 ),	/* 6 */
/* 2176 */	NdrFcShort( 0x20 ),	/* ia64 Stack size/offset = 32 */
/* 2178 */	NdrFcShort( 0x44 ),	/* 68 */
/* 2180 */	NdrFcShort( 0x8 ),	/* 8 */
/* 2182 */	0x45,		/* Oi2 Flags:  srv must size, has return, has ext, */
			0x3,		/* 3 */
/* 2184 */	0xa,		/* 10 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 2186 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2188 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2190 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2192 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter guid */

/* 2194 */	NdrFcShort( 0x10a ),	/* Flags:  must free, in, simple ref, */
/* 2196 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 2198 */	NdrFcShort( 0x26 ),	/* Type Offset=38 */

	/* Parameter ppTinfo */

/* 2200 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
/* 2202 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 2204 */	NdrFcShort( 0x6 ),	/* Type Offset=6 */

	/* Return value */

/* 2206 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 2208 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 2210 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure RemoteGetLibAttr */

/* 2212 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 2214 */	NdrFcLong( 0x0 ),	/* 0 */
/* 2218 */	NdrFcShort( 0x7 ),	/* 7 */
/* 2220 */	NdrFcShort( 0x20 ),	/* ia64 Stack size/offset = 32 */
/* 2222 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2224 */	NdrFcShort( 0x24 ),	/* 36 */
/* 2226 */	0x45,		/* Oi2 Flags:  srv must size, has return, has ext, */
			0x3,		/* 3 */
/* 2228 */	0xa,		/* 10 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 2230 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2232 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2234 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2236 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter ppTLibAttr */

/* 2238 */	NdrFcShort( 0x2013 ),	/* Flags:  must size, must free, out, srv alloc size=8 */
/* 2240 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 2242 */	NdrFcShort( 0x724 ),	/* Type Offset=1828 */

	/* Parameter pDummy */

/* 2244 */	NdrFcShort( 0x6112 ),	/* Flags:  must free, out, simple ref, srv alloc size=24 */
/* 2246 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 2248 */	NdrFcShort( 0x746 ),	/* Type Offset=1862 */

	/* Return value */

/* 2250 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 2252 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 2254 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetTypeComp */

/* 2256 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 2258 */	NdrFcLong( 0x0 ),	/* 0 */
/* 2262 */	NdrFcShort( 0x8 ),	/* 8 */
/* 2264 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 2266 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2268 */	NdrFcShort( 0x8 ),	/* 8 */
/* 2270 */	0x45,		/* Oi2 Flags:  srv must size, has return, has ext, */
			0x2,		/* 2 */
/* 2272 */	0xa,		/* 10 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 2274 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2276 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2278 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2280 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter ppTComp */

/* 2282 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
/* 2284 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 2286 */	NdrFcShort( 0x612 ),	/* Type Offset=1554 */

	/* Return value */

/* 2288 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 2290 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 2292 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure RemoteGetDocumentation */

/* 2294 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 2296 */	NdrFcLong( 0x0 ),	/* 0 */
/* 2300 */	NdrFcShort( 0x9 ),	/* 9 */
/* 2302 */	NdrFcShort( 0x40 ),	/* ia64 Stack size/offset = 64 */
/* 2304 */	NdrFcShort( 0x10 ),	/* 16 */
/* 2306 */	NdrFcShort( 0x24 ),	/* 36 */
/* 2308 */	0x45,		/* Oi2 Flags:  srv must size, has return, has ext, */
			0x7,		/* 7 */
/* 2310 */	0xa,		/* 10 */
			0x3,		/* Ext Flags:  new corr desc, clt corr check, */
/* 2312 */	NdrFcShort( 0x3 ),	/* 3 */
/* 2314 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2316 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2318 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter index */

/* 2320 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 2322 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 2324 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter refPtrFlags */

/* 2326 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 2328 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 2330 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter pBstrName */

/* 2332 */	NdrFcShort( 0x2113 ),	/* Flags:  must size, must free, out, simple ref, srv alloc size=8 */
/* 2334 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 2336 */	NdrFcShort( 0x45a ),	/* Type Offset=1114 */

	/* Parameter pBstrDocString */

/* 2338 */	NdrFcShort( 0x2113 ),	/* Flags:  must size, must free, out, simple ref, srv alloc size=8 */
/* 2340 */	NdrFcShort( 0x20 ),	/* ia64 Stack size/offset = 32 */
/* 2342 */	NdrFcShort( 0x45a ),	/* Type Offset=1114 */

	/* Parameter pdwHelpContext */

/* 2344 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 2346 */	NdrFcShort( 0x28 ),	/* ia64 Stack size/offset = 40 */
/* 2348 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter pBstrHelpFile */

/* 2350 */	NdrFcShort( 0x2113 ),	/* Flags:  must size, must free, out, simple ref, srv alloc size=8 */
/* 2352 */	NdrFcShort( 0x30 ),	/* ia64 Stack size/offset = 48 */
/* 2354 */	NdrFcShort( 0x45a ),	/* Type Offset=1114 */

	/* Return value */

/* 2356 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 2358 */	NdrFcShort( 0x38 ),	/* ia64 Stack size/offset = 56 */
/* 2360 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure RemoteIsName */

/* 2362 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 2364 */	NdrFcLong( 0x0 ),	/* 0 */
/* 2368 */	NdrFcShort( 0xa ),	/* 10 */
/* 2370 */	NdrFcShort( 0x30 ),	/* ia64 Stack size/offset = 48 */
/* 2372 */	NdrFcShort( 0x8 ),	/* 8 */
/* 2374 */	NdrFcShort( 0x24 ),	/* 36 */
/* 2376 */	0x47,		/* Oi2 Flags:  srv must size, clt must size, has return, has ext, */
			0x5,		/* 5 */
/* 2378 */	0xa,		/* 10 */
			0x3,		/* Ext Flags:  new corr desc, clt corr check, */
/* 2380 */	NdrFcShort( 0x1 ),	/* 1 */
/* 2382 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2384 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2386 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter szNameBuf */

/* 2388 */	NdrFcShort( 0x10b ),	/* Flags:  must size, must free, in, simple ref, */
/* 2390 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 2392 */	NdrFcShort( 0x4dc ),	/* Type Offset=1244 */

	/* Parameter lHashVal */

/* 2394 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 2396 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 2398 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter pfName */

/* 2400 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 2402 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 2404 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter pBstrLibName */

/* 2406 */	NdrFcShort( 0x2113 ),	/* Flags:  must size, must free, out, simple ref, srv alloc size=8 */
/* 2408 */	NdrFcShort( 0x20 ),	/* ia64 Stack size/offset = 32 */
/* 2410 */	NdrFcShort( 0x45a ),	/* Type Offset=1114 */

	/* Return value */

/* 2412 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 2414 */	NdrFcShort( 0x28 ),	/* ia64 Stack size/offset = 40 */
/* 2416 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure RemoteFindName */

/* 2418 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 2420 */	NdrFcLong( 0x0 ),	/* 0 */
/* 2424 */	NdrFcShort( 0xb ),	/* 11 */
/* 2426 */	NdrFcShort( 0x40 ),	/* ia64 Stack size/offset = 64 */
/* 2428 */	NdrFcShort( 0x22 ),	/* 34 */
/* 2430 */	NdrFcShort( 0x22 ),	/* 34 */
/* 2432 */	0x47,		/* Oi2 Flags:  srv must size, clt must size, has return, has ext, */
			0x7,		/* 7 */
/* 2434 */	0xa,		/* 10 */
			0x3,		/* Ext Flags:  new corr desc, clt corr check, */
/* 2436 */	NdrFcShort( 0x3 ),	/* 3 */
/* 2438 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2440 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2442 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter szNameBuf */

/* 2444 */	NdrFcShort( 0x10b ),	/* Flags:  must size, must free, in, simple ref, */
/* 2446 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 2448 */	NdrFcShort( 0x4dc ),	/* Type Offset=1244 */

	/* Parameter lHashVal */

/* 2450 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 2452 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 2454 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter ppTInfo */

/* 2456 */	NdrFcShort( 0x113 ),	/* Flags:  must size, must free, out, simple ref, */
/* 2458 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 2460 */	NdrFcShort( 0x754 ),	/* Type Offset=1876 */

	/* Parameter rgMemId */

/* 2462 */	NdrFcShort( 0x113 ),	/* Flags:  must size, must free, out, simple ref, */
/* 2464 */	NdrFcShort( 0x20 ),	/* ia64 Stack size/offset = 32 */
/* 2466 */	NdrFcShort( 0x76e ),	/* Type Offset=1902 */

	/* Parameter pcFound */

/* 2468 */	NdrFcShort( 0x158 ),	/* Flags:  in, out, base type, simple ref, */
/* 2470 */	NdrFcShort( 0x28 ),	/* ia64 Stack size/offset = 40 */
/* 2472 */	0x6,		/* FC_SHORT */
			0x0,		/* 0 */

	/* Parameter pBstrLibName */

/* 2474 */	NdrFcShort( 0x2113 ),	/* Flags:  must size, must free, out, simple ref, srv alloc size=8 */
/* 2476 */	NdrFcShort( 0x30 ),	/* ia64 Stack size/offset = 48 */
/* 2478 */	NdrFcShort( 0x45a ),	/* Type Offset=1114 */

	/* Return value */

/* 2480 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 2482 */	NdrFcShort( 0x38 ),	/* ia64 Stack size/offset = 56 */
/* 2484 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure LocalReleaseTLibAttr */

/* 2486 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 2488 */	NdrFcLong( 0x0 ),	/* 0 */
/* 2492 */	NdrFcShort( 0xc ),	/* 12 */
/* 2494 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 2496 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2498 */	NdrFcShort( 0x8 ),	/* 8 */
/* 2500 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x1,		/* 1 */
/* 2502 */	0xa,		/* 10 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 2504 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2506 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2508 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2510 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Return value */

/* 2512 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 2514 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 2516 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetCustData */

/* 2518 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 2520 */	NdrFcLong( 0x0 ),	/* 0 */
/* 2524 */	NdrFcShort( 0xd ),	/* 13 */
/* 2526 */	NdrFcShort( 0x20 ),	/* ia64 Stack size/offset = 32 */
/* 2528 */	NdrFcShort( 0x44 ),	/* 68 */
/* 2530 */	NdrFcShort( 0x8 ),	/* 8 */
/* 2532 */	0x45,		/* Oi2 Flags:  srv must size, has return, has ext, */
			0x3,		/* 3 */
/* 2534 */	0xa,		/* 10 */
			0x3,		/* Ext Flags:  new corr desc, clt corr check, */
/* 2536 */	NdrFcShort( 0x20 ),	/* 32 */
/* 2538 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2540 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2542 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter guid */

/* 2544 */	NdrFcShort( 0x10a ),	/* Flags:  must free, in, simple ref, */
/* 2546 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 2548 */	NdrFcShort( 0x26 ),	/* Type Offset=38 */

	/* Parameter pVarVal */

/* 2550 */	NdrFcShort( 0x6113 ),	/* Flags:  must size, must free, out, simple ref, srv alloc size=24 */
/* 2552 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 2554 */	NdrFcShort( 0x448 ),	/* Type Offset=1096 */

	/* Return value */

/* 2556 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 2558 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 2560 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure RemoteGetLibStatistics */

/* 2562 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 2564 */	NdrFcLong( 0x0 ),	/* 0 */
/* 2568 */	NdrFcShort( 0xe ),	/* 14 */
/* 2570 */	NdrFcShort( 0x20 ),	/* ia64 Stack size/offset = 32 */
/* 2572 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2574 */	NdrFcShort( 0x40 ),	/* 64 */
/* 2576 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x3,		/* 3 */
/* 2578 */	0xa,		/* 10 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 2580 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2582 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2584 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2586 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pcUniqueNames */

/* 2588 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 2590 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 2592 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter pcchUniqueNames */

/* 2594 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 2596 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 2598 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 2600 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 2602 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 2604 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure RemoteGetDocumentation2 */

/* 2606 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 2608 */	NdrFcLong( 0x0 ),	/* 0 */
/* 2612 */	NdrFcShort( 0xf ),	/* 15 */
/* 2614 */	NdrFcShort( 0x40 ),	/* ia64 Stack size/offset = 64 */
/* 2616 */	NdrFcShort( 0x18 ),	/* 24 */
/* 2618 */	NdrFcShort( 0x24 ),	/* 36 */
/* 2620 */	0x45,		/* Oi2 Flags:  srv must size, has return, has ext, */
			0x7,		/* 7 */
/* 2622 */	0xa,		/* 10 */
			0x3,		/* Ext Flags:  new corr desc, clt corr check, */
/* 2624 */	NdrFcShort( 0x2 ),	/* 2 */
/* 2626 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2628 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2630 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter index */

/* 2632 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 2634 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 2636 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter lcid */

/* 2638 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 2640 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 2642 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter refPtrFlags */

/* 2644 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 2646 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 2648 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter pbstrHelpString */

/* 2650 */	NdrFcShort( 0x2113 ),	/* Flags:  must size, must free, out, simple ref, srv alloc size=8 */
/* 2652 */	NdrFcShort( 0x20 ),	/* ia64 Stack size/offset = 32 */
/* 2654 */	NdrFcShort( 0x45a ),	/* Type Offset=1114 */

	/* Parameter pdwHelpStringContext */

/* 2656 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 2658 */	NdrFcShort( 0x28 ),	/* ia64 Stack size/offset = 40 */
/* 2660 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter pbstrHelpStringDll */

/* 2662 */	NdrFcShort( 0x2113 ),	/* Flags:  must size, must free, out, simple ref, srv alloc size=8 */
/* 2664 */	NdrFcShort( 0x30 ),	/* ia64 Stack size/offset = 48 */
/* 2666 */	NdrFcShort( 0x45a ),	/* Type Offset=1114 */

	/* Return value */

/* 2668 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 2670 */	NdrFcShort( 0x38 ),	/* ia64 Stack size/offset = 56 */
/* 2672 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetAllCustData */

/* 2674 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 2676 */	NdrFcLong( 0x0 ),	/* 0 */
/* 2680 */	NdrFcShort( 0x10 ),	/* 16 */
/* 2682 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 2684 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2686 */	NdrFcShort( 0x8 ),	/* 8 */
/* 2688 */	0x45,		/* Oi2 Flags:  srv must size, has return, has ext, */
			0x2,		/* 2 */
/* 2690 */	0xa,		/* 10 */
			0x3,		/* Ext Flags:  new corr desc, clt corr check, */
/* 2692 */	NdrFcShort( 0x21 ),	/* 33 */
/* 2694 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2696 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2698 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pCustData */

/* 2700 */	NdrFcShort( 0x4113 ),	/* Flags:  must size, must free, out, simple ref, srv alloc size=16 */
/* 2702 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 2704 */	NdrFcShort( 0x714 ),	/* Type Offset=1812 */

	/* Return value */

/* 2706 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 2708 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 2710 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetGUID */

/* 2712 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 2714 */	NdrFcLong( 0x0 ),	/* 0 */
/* 2718 */	NdrFcShort( 0x3 ),	/* 3 */
/* 2720 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 2722 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2724 */	NdrFcShort( 0x4c ),	/* 76 */
/* 2726 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x2,		/* 2 */
/* 2728 */	0xa,		/* 10 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 2730 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2732 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2734 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2736 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pGUID */

/* 2738 */	NdrFcShort( 0x4112 ),	/* Flags:  must free, out, simple ref, srv alloc size=16 */
/* 2740 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 2742 */	NdrFcShort( 0x26 ),	/* Type Offset=38 */

	/* Return value */

/* 2744 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 2746 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 2748 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetSource */

/* 2750 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 2752 */	NdrFcLong( 0x0 ),	/* 0 */
/* 2756 */	NdrFcShort( 0x4 ),	/* 4 */
/* 2758 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 2760 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2762 */	NdrFcShort( 0x8 ),	/* 8 */
/* 2764 */	0x45,		/* Oi2 Flags:  srv must size, has return, has ext, */
			0x2,		/* 2 */
/* 2766 */	0xa,		/* 10 */
			0x3,		/* Ext Flags:  new corr desc, clt corr check, */
/* 2768 */	NdrFcShort( 0x1 ),	/* 1 */
/* 2770 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2772 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2774 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pBstrSource */

/* 2776 */	NdrFcShort( 0x2113 ),	/* Flags:  must size, must free, out, simple ref, srv alloc size=8 */
/* 2778 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 2780 */	NdrFcShort( 0x45a ),	/* Type Offset=1114 */

	/* Return value */

/* 2782 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 2784 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 2786 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetDescription */

/* 2788 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 2790 */	NdrFcLong( 0x0 ),	/* 0 */
/* 2794 */	NdrFcShort( 0x5 ),	/* 5 */
/* 2796 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 2798 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2800 */	NdrFcShort( 0x8 ),	/* 8 */
/* 2802 */	0x45,		/* Oi2 Flags:  srv must size, has return, has ext, */
			0x2,		/* 2 */
/* 2804 */	0xa,		/* 10 */
			0x3,		/* Ext Flags:  new corr desc, clt corr check, */
/* 2806 */	NdrFcShort( 0x1 ),	/* 1 */
/* 2808 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2810 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2812 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pBstrDescription */

/* 2814 */	NdrFcShort( 0x2113 ),	/* Flags:  must size, must free, out, simple ref, srv alloc size=8 */
/* 2816 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 2818 */	NdrFcShort( 0x45a ),	/* Type Offset=1114 */

	/* Return value */

/* 2820 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 2822 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 2824 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetHelpFile */

/* 2826 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 2828 */	NdrFcLong( 0x0 ),	/* 0 */
/* 2832 */	NdrFcShort( 0x6 ),	/* 6 */
/* 2834 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 2836 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2838 */	NdrFcShort( 0x8 ),	/* 8 */
/* 2840 */	0x45,		/* Oi2 Flags:  srv must size, has return, has ext, */
			0x2,		/* 2 */
/* 2842 */	0xa,		/* 10 */
			0x3,		/* Ext Flags:  new corr desc, clt corr check, */
/* 2844 */	NdrFcShort( 0x1 ),	/* 1 */
/* 2846 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2848 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2850 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pBstrHelpFile */

/* 2852 */	NdrFcShort( 0x2113 ),	/* Flags:  must size, must free, out, simple ref, srv alloc size=8 */
/* 2854 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 2856 */	NdrFcShort( 0x45a ),	/* Type Offset=1114 */

	/* Return value */

/* 2858 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 2860 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 2862 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetHelpContext */

/* 2864 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 2866 */	NdrFcLong( 0x0 ),	/* 0 */
/* 2870 */	NdrFcShort( 0x7 ),	/* 7 */
/* 2872 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 2874 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2876 */	NdrFcShort( 0x24 ),	/* 36 */
/* 2878 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x2,		/* 2 */
/* 2880 */	0xa,		/* 10 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 2882 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2884 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2886 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2888 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pdwHelpContext */

/* 2890 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 2892 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 2894 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 2896 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 2898 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 2900 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure InterfaceSupportsErrorInfo */


	/* Procedure SetGUID */

/* 2902 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 2904 */	NdrFcLong( 0x0 ),	/* 0 */
/* 2908 */	NdrFcShort( 0x3 ),	/* 3 */
/* 2910 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 2912 */	NdrFcShort( 0x44 ),	/* 68 */
/* 2914 */	NdrFcShort( 0x8 ),	/* 8 */
/* 2916 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x2,		/* 2 */
/* 2918 */	0xa,		/* 10 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 2920 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2922 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2924 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2926 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter riid */


	/* Parameter rguid */

/* 2928 */	NdrFcShort( 0x10a ),	/* Flags:  must free, in, simple ref, */
/* 2930 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 2932 */	NdrFcShort( 0x26 ),	/* Type Offset=38 */

	/* Return value */


	/* Return value */

/* 2934 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 2936 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 2938 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure SetSource */

/* 2940 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 2942 */	NdrFcLong( 0x0 ),	/* 0 */
/* 2946 */	NdrFcShort( 0x4 ),	/* 4 */
/* 2948 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 2950 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2952 */	NdrFcShort( 0x8 ),	/* 8 */
/* 2954 */	0x46,		/* Oi2 Flags:  clt must size, has return, has ext, */
			0x2,		/* 2 */
/* 2956 */	0xa,		/* 10 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 2958 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2960 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2962 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2964 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter szSource */

/* 2966 */	NdrFcShort( 0x10b ),	/* Flags:  must size, must free, in, simple ref, */
/* 2968 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 2970 */	NdrFcShort( 0x4dc ),	/* Type Offset=1244 */

	/* Return value */

/* 2972 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 2974 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 2976 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure SetDescription */

/* 2978 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 2980 */	NdrFcLong( 0x0 ),	/* 0 */
/* 2984 */	NdrFcShort( 0x5 ),	/* 5 */
/* 2986 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 2988 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2990 */	NdrFcShort( 0x8 ),	/* 8 */
/* 2992 */	0x46,		/* Oi2 Flags:  clt must size, has return, has ext, */
			0x2,		/* 2 */
/* 2994 */	0xa,		/* 10 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 2996 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2998 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3000 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3002 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter szDescription */

/* 3004 */	NdrFcShort( 0x10b ),	/* Flags:  must size, must free, in, simple ref, */
/* 3006 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 3008 */	NdrFcShort( 0x4dc ),	/* Type Offset=1244 */

	/* Return value */

/* 3010 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 3012 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 3014 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure SetHelpFile */

/* 3016 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 3018 */	NdrFcLong( 0x0 ),	/* 0 */
/* 3022 */	NdrFcShort( 0x6 ),	/* 6 */
/* 3024 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 3026 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3028 */	NdrFcShort( 0x8 ),	/* 8 */
/* 3030 */	0x46,		/* Oi2 Flags:  clt must size, has return, has ext, */
			0x2,		/* 2 */
/* 3032 */	0xa,		/* 10 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 3034 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3036 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3038 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3040 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter szHelpFile */

/* 3042 */	NdrFcShort( 0x10b ),	/* Flags:  must size, must free, in, simple ref, */
/* 3044 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 3046 */	NdrFcShort( 0x4dc ),	/* Type Offset=1244 */

	/* Return value */

/* 3048 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 3050 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 3052 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure SetHelpContext */

/* 3054 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 3056 */	NdrFcLong( 0x0 ),	/* 0 */
/* 3060 */	NdrFcShort( 0x7 ),	/* 7 */
/* 3062 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 3064 */	NdrFcShort( 0x8 ),	/* 8 */
/* 3066 */	NdrFcShort( 0x8 ),	/* 8 */
/* 3068 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x2,		/* 2 */
/* 3070 */	0xa,		/* 10 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 3072 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3074 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3076 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3078 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter dwHelpContext */

/* 3080 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 3082 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 3084 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 3086 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 3088 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 3090 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure CreateFromTypeInfo */

/* 3092 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 3094 */	NdrFcLong( 0x0 ),	/* 0 */
/* 3098 */	NdrFcShort( 0x3 ),	/* 3 */
/* 3100 */	NdrFcShort( 0x28 ),	/* ia64 Stack size/offset = 40 */
/* 3102 */	NdrFcShort( 0x44 ),	/* 68 */
/* 3104 */	NdrFcShort( 0x8 ),	/* 8 */
/* 3106 */	0x47,		/* Oi2 Flags:  srv must size, clt must size, has return, has ext, */
			0x4,		/* 4 */
/* 3108 */	0xa,		/* 10 */
			0x3,		/* Ext Flags:  new corr desc, clt corr check, */
/* 3110 */	NdrFcShort( 0x1 ),	/* 1 */
/* 3112 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3114 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3116 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pTypeInfo */

/* 3118 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 3120 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 3122 */	NdrFcShort( 0xa ),	/* Type Offset=10 */

	/* Parameter riid */

/* 3124 */	NdrFcShort( 0x10a ),	/* Flags:  must free, in, simple ref, */
/* 3126 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 3128 */	NdrFcShort( 0x26 ),	/* Type Offset=38 */

	/* Parameter ppv */

/* 3130 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
/* 3132 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 3134 */	NdrFcShort( 0x788 ),	/* Type Offset=1928 */

	/* Return value */

/* 3136 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 3138 */	NdrFcShort( 0x20 ),	/* ia64 Stack size/offset = 32 */
/* 3140 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure AddError */

/* 3142 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 3144 */	NdrFcLong( 0x0 ),	/* 0 */
/* 3148 */	NdrFcShort( 0x3 ),	/* 3 */
/* 3150 */	NdrFcShort( 0x20 ),	/* ia64 Stack size/offset = 32 */
/* 3152 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3154 */	NdrFcShort( 0x8 ),	/* 8 */
/* 3156 */	0x46,		/* Oi2 Flags:  clt must size, has return, has ext, */
			0x3,		/* 3 */
/* 3158 */	0xa,		/* 10 */
			0x5,		/* Ext Flags:  new corr desc, srv corr check, */
/* 3160 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3162 */	NdrFcShort( 0x3 ),	/* 3 */
/* 3164 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3166 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pszPropName */

/* 3168 */	NdrFcShort( 0x10b ),	/* Flags:  must size, must free, in, simple ref, */
/* 3170 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 3172 */	NdrFcShort( 0x4dc ),	/* Type Offset=1244 */

	/* Parameter pExcepInfo */

/* 3174 */	NdrFcShort( 0x10b ),	/* Flags:  must size, must free, in, simple ref, */
/* 3176 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 3178 */	NdrFcShort( 0x464 ),	/* Type Offset=1124 */

	/* Return value */

/* 3180 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 3182 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 3184 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure RemoteRead */

/* 3186 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 3188 */	NdrFcLong( 0x0 ),	/* 0 */
/* 3192 */	NdrFcShort( 0x3 ),	/* 3 */
/* 3194 */	NdrFcShort( 0x38 ),	/* ia64 Stack size/offset = 56 */
/* 3196 */	NdrFcShort( 0x8 ),	/* 8 */
/* 3198 */	NdrFcShort( 0x8 ),	/* 8 */
/* 3200 */	0x47,		/* Oi2 Flags:  srv must size, clt must size, has return, has ext, */
			0x6,		/* 6 */
/* 3202 */	0xa,		/* 10 */
			0x3,		/* Ext Flags:  new corr desc, clt corr check, */
/* 3204 */	NdrFcShort( 0x20 ),	/* 32 */
/* 3206 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3208 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3210 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pszPropName */

/* 3212 */	NdrFcShort( 0x10b ),	/* Flags:  must size, must free, in, simple ref, */
/* 3214 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 3216 */	NdrFcShort( 0x4dc ),	/* Type Offset=1244 */

	/* Parameter pVar */

/* 3218 */	NdrFcShort( 0x6113 ),	/* Flags:  must size, must free, out, simple ref, srv alloc size=24 */
/* 3220 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 3222 */	NdrFcShort( 0x448 ),	/* Type Offset=1096 */

	/* Parameter pErrorLog */

/* 3224 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 3226 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 3228 */	NdrFcShort( 0x798 ),	/* Type Offset=1944 */

	/* Parameter varType */

/* 3230 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 3232 */	NdrFcShort( 0x20 ),	/* ia64 Stack size/offset = 32 */
/* 3234 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter pUnkObj */

/* 3236 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 3238 */	NdrFcShort( 0x28 ),	/* ia64 Stack size/offset = 40 */
/* 3240 */	NdrFcShort( 0x196 ),	/* Type Offset=406 */

	/* Return value */

/* 3242 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 3244 */	NdrFcShort( 0x30 ),	/* ia64 Stack size/offset = 48 */
/* 3246 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure Write */

/* 3248 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 3250 */	NdrFcLong( 0x0 ),	/* 0 */
/* 3254 */	NdrFcShort( 0x4 ),	/* 4 */
/* 3256 */	NdrFcShort( 0x20 ),	/* ia64 Stack size/offset = 32 */
/* 3258 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3260 */	NdrFcShort( 0x8 ),	/* 8 */
/* 3262 */	0x46,		/* Oi2 Flags:  clt must size, has return, has ext, */
			0x3,		/* 3 */
/* 3264 */	0xa,		/* 10 */
			0x5,		/* Ext Flags:  new corr desc, srv corr check, */
/* 3266 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3268 */	NdrFcShort( 0x20 ),	/* 32 */
/* 3270 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3272 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pszPropName */

/* 3274 */	NdrFcShort( 0x10b ),	/* Flags:  must size, must free, in, simple ref, */
/* 3276 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 3278 */	NdrFcShort( 0x4dc ),	/* Type Offset=1244 */

	/* Parameter pVar */

/* 3280 */	NdrFcShort( 0x10b ),	/* Flags:  must size, must free, in, simple ref, */
/* 3282 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 3284 */	NdrFcShort( 0x3fe ),	/* Type Offset=1022 */

	/* Return value */

/* 3286 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 3288 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 3290 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

			0x0
        }
    };

static const MIDL_TYPE_FORMAT_STRING __MIDL_TypeFormatString =
    {
        0,
        {
			NdrFcShort( 0x0 ),	/* 0 */
/*  2 */	
			0x11, 0xc,	/* FC_RP [alloced_on_stack] [simple_pointer] */
/*  4 */	0x8,		/* FC_LONG */
			0x5c,		/* FC_PAD */
/*  6 */	
			0x11, 0x10,	/* FC_RP [pointer_deref] */
/*  8 */	NdrFcShort( 0x2 ),	/* Offset= 2 (10) */
/* 10 */	
			0x2f,		/* FC_IP */
			0x5a,		/* FC_CONSTANT_IID */
/* 12 */	NdrFcLong( 0x20401 ),	/* 132097 */
/* 16 */	NdrFcShort( 0x0 ),	/* 0 */
/* 18 */	NdrFcShort( 0x0 ),	/* 0 */
/* 20 */	0xc0,		/* 192 */
			0x0,		/* 0 */
/* 22 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 24 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 26 */	0x0,		/* 0 */
			0x46,		/* 70 */
/* 28 */	
			0x11, 0x0,	/* FC_RP */
/* 30 */	NdrFcShort( 0x8 ),	/* Offset= 8 (38) */
/* 32 */	
			0x1d,		/* FC_SMFARRAY */
			0x0,		/* 0 */
/* 34 */	NdrFcShort( 0x8 ),	/* 8 */
/* 36 */	0x2,		/* FC_CHAR */
			0x5b,		/* FC_END */
/* 38 */	
			0x15,		/* FC_STRUCT */
			0x3,		/* 3 */
/* 40 */	NdrFcShort( 0x10 ),	/* 16 */
/* 42 */	0x8,		/* FC_LONG */
			0x6,		/* FC_SHORT */
/* 44 */	0x6,		/* FC_SHORT */
			0x4c,		/* FC_EMBEDDED_COMPLEX */
/* 46 */	0x0,		/* 0 */
			NdrFcShort( 0xfffffff1 ),	/* Offset= -15 (32) */
			0x5b,		/* FC_END */
/* 50 */	
			0x11, 0x0,	/* FC_RP */
/* 52 */	NdrFcShort( 0x2 ),	/* Offset= 2 (54) */
/* 54 */	
			0x21,		/* FC_BOGUS_ARRAY */
			0x3,		/* 3 */
/* 56 */	NdrFcShort( 0x0 ),	/* 0 */
/* 58 */	0x29,		/* Corr desc:  parameter, FC_ULONG */
			0x0,		/*  */
/* 60 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 62 */	NdrFcShort( 0x0 ),	/* Corr flags:  */
/* 64 */	NdrFcLong( 0xffffffff ),	/* -1 */
/* 68 */	NdrFcShort( 0x0 ),	/* Corr flags:  */
/* 70 */	
			0x12, 0x8,	/* FC_UP [simple_pointer] */
/* 72 */	
			0x25,		/* FC_C_WSTRING */
			0x5c,		/* FC_PAD */
/* 74 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 76 */	
			0x11, 0x0,	/* FC_RP */
/* 78 */	NdrFcShort( 0x2 ),	/* Offset= 2 (80) */
/* 80 */	
			0x1b,		/* FC_CARRAY */
			0x3,		/* 3 */
/* 82 */	NdrFcShort( 0x4 ),	/* 4 */
/* 84 */	0x29,		/* Corr desc:  parameter, FC_ULONG */
			0x0,		/*  */
/* 86 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 88 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 90 */	0x8,		/* FC_LONG */
			0x5b,		/* FC_END */
/* 92 */	
			0x11, 0x0,	/* FC_RP */
/* 94 */	NdrFcShort( 0x3cc ),	/* Offset= 972 (1066) */
/* 96 */	
			0x12, 0x0,	/* FC_UP */
/* 98 */	NdrFcShort( 0x388 ),	/* Offset= 904 (1002) */
/* 100 */	
			0x2b,		/* FC_NON_ENCAPSULATED_UNION */
			0x9,		/* FC_ULONG */
/* 102 */	0x7,		/* Corr desc: FC_USHORT */
			0x0,		/*  */
/* 104 */	NdrFcShort( 0xfff8 ),	/* -8 */
/* 106 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 108 */	NdrFcShort( 0x2 ),	/* Offset= 2 (110) */
/* 110 */	NdrFcShort( 0x10 ),	/* 16 */
/* 112 */	NdrFcShort( 0x2b ),	/* 43 */
/* 114 */	NdrFcLong( 0x3 ),	/* 3 */
/* 118 */	NdrFcShort( 0x8008 ),	/* Simple arm type: FC_LONG */
/* 120 */	NdrFcLong( 0x11 ),	/* 17 */
/* 124 */	NdrFcShort( 0x8001 ),	/* Simple arm type: FC_BYTE */
/* 126 */	NdrFcLong( 0x2 ),	/* 2 */
/* 130 */	NdrFcShort( 0x8006 ),	/* Simple arm type: FC_SHORT */
/* 132 */	NdrFcLong( 0x4 ),	/* 4 */
/* 136 */	NdrFcShort( 0x800a ),	/* Simple arm type: FC_FLOAT */
/* 138 */	NdrFcLong( 0x5 ),	/* 5 */
/* 142 */	NdrFcShort( 0x800c ),	/* Simple arm type: FC_DOUBLE */
/* 144 */	NdrFcLong( 0xb ),	/* 11 */
/* 148 */	NdrFcShort( 0x8006 ),	/* Simple arm type: FC_SHORT */
/* 150 */	NdrFcLong( 0xa ),	/* 10 */
/* 154 */	NdrFcShort( 0x8008 ),	/* Simple arm type: FC_LONG */
/* 156 */	NdrFcLong( 0x6 ),	/* 6 */
/* 160 */	NdrFcShort( 0xd6 ),	/* Offset= 214 (374) */
/* 162 */	NdrFcLong( 0x7 ),	/* 7 */
/* 166 */	NdrFcShort( 0x800c ),	/* Simple arm type: FC_DOUBLE */
/* 168 */	NdrFcLong( 0x8 ),	/* 8 */
/* 172 */	NdrFcShort( 0xd0 ),	/* Offset= 208 (380) */
/* 174 */	NdrFcLong( 0xd ),	/* 13 */
/* 178 */	NdrFcShort( 0xe4 ),	/* Offset= 228 (406) */
/* 180 */	NdrFcLong( 0x9 ),	/* 9 */
/* 184 */	NdrFcShort( 0xf0 ),	/* Offset= 240 (424) */
/* 186 */	NdrFcLong( 0x2000 ),	/* 8192 */
/* 190 */	NdrFcShort( 0xfc ),	/* Offset= 252 (442) */
/* 192 */	NdrFcLong( 0x24 ),	/* 36 */
/* 196 */	NdrFcShort( 0x2e0 ),	/* Offset= 736 (932) */
/* 198 */	NdrFcLong( 0x4024 ),	/* 16420 */
/* 202 */	NdrFcShort( 0x2da ),	/* Offset= 730 (932) */
/* 204 */	NdrFcLong( 0x4011 ),	/* 16401 */
/* 208 */	NdrFcShort( 0x2d8 ),	/* Offset= 728 (936) */
/* 210 */	NdrFcLong( 0x4002 ),	/* 16386 */
/* 214 */	NdrFcShort( 0x2d6 ),	/* Offset= 726 (940) */
/* 216 */	NdrFcLong( 0x4003 ),	/* 16387 */
/* 220 */	NdrFcShort( 0x2d4 ),	/* Offset= 724 (944) */
/* 222 */	NdrFcLong( 0x4004 ),	/* 16388 */
/* 226 */	NdrFcShort( 0x2d2 ),	/* Offset= 722 (948) */
/* 228 */	NdrFcLong( 0x4005 ),	/* 16389 */
/* 232 */	NdrFcShort( 0x2d0 ),	/* Offset= 720 (952) */
/* 234 */	NdrFcLong( 0x400b ),	/* 16395 */
/* 238 */	NdrFcShort( 0x2be ),	/* Offset= 702 (940) */
/* 240 */	NdrFcLong( 0x400a ),	/* 16394 */
/* 244 */	NdrFcShort( 0x2bc ),	/* Offset= 700 (944) */
/* 246 */	NdrFcLong( 0x4006 ),	/* 16390 */
/* 250 */	NdrFcShort( 0x2c2 ),	/* Offset= 706 (956) */
/* 252 */	NdrFcLong( 0x4007 ),	/* 16391 */
/* 256 */	NdrFcShort( 0x2b8 ),	/* Offset= 696 (952) */
/* 258 */	NdrFcLong( 0x4008 ),	/* 16392 */
/* 262 */	NdrFcShort( 0x2ba ),	/* Offset= 698 (960) */
/* 264 */	NdrFcLong( 0x400d ),	/* 16397 */
/* 268 */	NdrFcShort( 0x2b8 ),	/* Offset= 696 (964) */
/* 270 */	NdrFcLong( 0x4009 ),	/* 16393 */
/* 274 */	NdrFcShort( 0x2b6 ),	/* Offset= 694 (968) */
/* 276 */	NdrFcLong( 0x6000 ),	/* 24576 */
/* 280 */	NdrFcShort( 0x2b4 ),	/* Offset= 692 (972) */
/* 282 */	NdrFcLong( 0x400c ),	/* 16396 */
/* 286 */	NdrFcShort( 0x2b2 ),	/* Offset= 690 (976) */
/* 288 */	NdrFcLong( 0x10 ),	/* 16 */
/* 292 */	NdrFcShort( 0x8002 ),	/* Simple arm type: FC_CHAR */
/* 294 */	NdrFcLong( 0x12 ),	/* 18 */
/* 298 */	NdrFcShort( 0x8006 ),	/* Simple arm type: FC_SHORT */
/* 300 */	NdrFcLong( 0x13 ),	/* 19 */
/* 304 */	NdrFcShort( 0x8008 ),	/* Simple arm type: FC_LONG */
/* 306 */	NdrFcLong( 0x16 ),	/* 22 */
/* 310 */	NdrFcShort( 0x8008 ),	/* Simple arm type: FC_LONG */
/* 312 */	NdrFcLong( 0x17 ),	/* 23 */
/* 316 */	NdrFcShort( 0x8008 ),	/* Simple arm type: FC_LONG */
/* 318 */	NdrFcLong( 0xe ),	/* 14 */
/* 322 */	NdrFcShort( 0x296 ),	/* Offset= 662 (984) */
/* 324 */	NdrFcLong( 0x400e ),	/* 16398 */
/* 328 */	NdrFcShort( 0x29a ),	/* Offset= 666 (994) */
/* 330 */	NdrFcLong( 0x4010 ),	/* 16400 */
/* 334 */	NdrFcShort( 0x298 ),	/* Offset= 664 (998) */
/* 336 */	NdrFcLong( 0x4012 ),	/* 16402 */
/* 340 */	NdrFcShort( 0x258 ),	/* Offset= 600 (940) */
/* 342 */	NdrFcLong( 0x4013 ),	/* 16403 */
/* 346 */	NdrFcShort( 0x256 ),	/* Offset= 598 (944) */
/* 348 */	NdrFcLong( 0x4016 ),	/* 16406 */
/* 352 */	NdrFcShort( 0x250 ),	/* Offset= 592 (944) */
/* 354 */	NdrFcLong( 0x4017 ),	/* 16407 */
/* 358 */	NdrFcShort( 0x24a ),	/* Offset= 586 (944) */
/* 360 */	NdrFcLong( 0x0 ),	/* 0 */
/* 364 */	NdrFcShort( 0x0 ),	/* Offset= 0 (364) */
/* 366 */	NdrFcLong( 0x1 ),	/* 1 */
/* 370 */	NdrFcShort( 0x0 ),	/* Offset= 0 (370) */
/* 372 */	NdrFcShort( 0xffffffff ),	/* Offset= -1 (371) */
/* 374 */	
			0x15,		/* FC_STRUCT */
			0x7,		/* 7 */
/* 376 */	NdrFcShort( 0x8 ),	/* 8 */
/* 378 */	0xb,		/* FC_HYPER */
			0x5b,		/* FC_END */
/* 380 */	
			0x12, 0x0,	/* FC_UP */
/* 382 */	NdrFcShort( 0xe ),	/* Offset= 14 (396) */
/* 384 */	
			0x1b,		/* FC_CARRAY */
			0x1,		/* 1 */
/* 386 */	NdrFcShort( 0x2 ),	/* 2 */
/* 388 */	0x9,		/* Corr desc: FC_ULONG */
			0x0,		/*  */
/* 390 */	NdrFcShort( 0xfffc ),	/* -4 */
/* 392 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 394 */	0x6,		/* FC_SHORT */
			0x5b,		/* FC_END */
/* 396 */	
			0x17,		/* FC_CSTRUCT */
			0x3,		/* 3 */
/* 398 */	NdrFcShort( 0x8 ),	/* 8 */
/* 400 */	NdrFcShort( 0xfffffff0 ),	/* Offset= -16 (384) */
/* 402 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 404 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 406 */	
			0x2f,		/* FC_IP */
			0x5a,		/* FC_CONSTANT_IID */
/* 408 */	NdrFcLong( 0x0 ),	/* 0 */
/* 412 */	NdrFcShort( 0x0 ),	/* 0 */
/* 414 */	NdrFcShort( 0x0 ),	/* 0 */
/* 416 */	0xc0,		/* 192 */
			0x0,		/* 0 */
/* 418 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 420 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 422 */	0x0,		/* 0 */
			0x46,		/* 70 */
/* 424 */	
			0x2f,		/* FC_IP */
			0x5a,		/* FC_CONSTANT_IID */
/* 426 */	NdrFcLong( 0x20400 ),	/* 132096 */
/* 430 */	NdrFcShort( 0x0 ),	/* 0 */
/* 432 */	NdrFcShort( 0x0 ),	/* 0 */
/* 434 */	0xc0,		/* 192 */
			0x0,		/* 0 */
/* 436 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 438 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 440 */	0x0,		/* 0 */
			0x46,		/* 70 */
/* 442 */	
			0x12, 0x10,	/* FC_UP [pointer_deref] */
/* 444 */	NdrFcShort( 0x2 ),	/* Offset= 2 (446) */
/* 446 */	
			0x12, 0x0,	/* FC_UP */
/* 448 */	NdrFcShort( 0x1d2 ),	/* Offset= 466 (914) */
/* 450 */	
			0x2a,		/* FC_ENCAPSULATED_UNION */
			0x89,		/* 137 */
/* 452 */	NdrFcShort( 0x20 ),	/* 32 */
/* 454 */	NdrFcShort( 0xa ),	/* 10 */
/* 456 */	NdrFcLong( 0x8 ),	/* 8 */
/* 460 */	NdrFcShort( 0x50 ),	/* Offset= 80 (540) */
/* 462 */	NdrFcLong( 0xd ),	/* 13 */
/* 466 */	NdrFcShort( 0x70 ),	/* Offset= 112 (578) */
/* 468 */	NdrFcLong( 0x9 ),	/* 9 */
/* 472 */	NdrFcShort( 0x90 ),	/* Offset= 144 (616) */
/* 474 */	NdrFcLong( 0xc ),	/* 12 */
/* 478 */	NdrFcShort( 0xb0 ),	/* Offset= 176 (654) */
/* 480 */	NdrFcLong( 0x24 ),	/* 36 */
/* 484 */	NdrFcShort( 0x102 ),	/* Offset= 258 (742) */
/* 486 */	NdrFcLong( 0x800d ),	/* 32781 */
/* 490 */	NdrFcShort( 0x10c ),	/* Offset= 268 (758) */
/* 492 */	NdrFcLong( 0x10 ),	/* 16 */
/* 496 */	NdrFcShort( 0x126 ),	/* Offset= 294 (790) */
/* 498 */	NdrFcLong( 0x2 ),	/* 2 */
/* 502 */	NdrFcShort( 0x13c ),	/* Offset= 316 (818) */
/* 504 */	NdrFcLong( 0x3 ),	/* 3 */
/* 508 */	NdrFcShort( 0x152 ),	/* Offset= 338 (846) */
/* 510 */	NdrFcLong( 0x14 ),	/* 20 */
/* 514 */	NdrFcShort( 0x168 ),	/* Offset= 360 (874) */
/* 516 */	NdrFcShort( 0xffffffff ),	/* Offset= -1 (515) */
/* 518 */	
			0x21,		/* FC_BOGUS_ARRAY */
			0x3,		/* 3 */
/* 520 */	NdrFcShort( 0x0 ),	/* 0 */
/* 522 */	0x19,		/* Corr desc:  field pointer, FC_ULONG */
			0x0,		/*  */
/* 524 */	NdrFcShort( 0x0 ),	/* 0 */
/* 526 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 528 */	NdrFcLong( 0xffffffff ),	/* -1 */
/* 532 */	NdrFcShort( 0x0 ),	/* Corr flags:  */
/* 534 */	
			0x12, 0x0,	/* FC_UP */
/* 536 */	NdrFcShort( 0xffffff74 ),	/* Offset= -140 (396) */
/* 538 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 540 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 542 */	NdrFcShort( 0x10 ),	/* 16 */
/* 544 */	NdrFcShort( 0x0 ),	/* 0 */
/* 546 */	NdrFcShort( 0x6 ),	/* Offset= 6 (552) */
/* 548 */	0x8,		/* FC_LONG */
			0x40,		/* FC_STRUCTPAD4 */
/* 550 */	0x36,		/* FC_POINTER */
			0x5b,		/* FC_END */
/* 552 */	
			0x11, 0x0,	/* FC_RP */
/* 554 */	NdrFcShort( 0xffffffdc ),	/* Offset= -36 (518) */
/* 556 */	
			0x21,		/* FC_BOGUS_ARRAY */
			0x3,		/* 3 */
/* 558 */	NdrFcShort( 0x0 ),	/* 0 */
/* 560 */	0x19,		/* Corr desc:  field pointer, FC_ULONG */
			0x0,		/*  */
/* 562 */	NdrFcShort( 0x0 ),	/* 0 */
/* 564 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 566 */	NdrFcLong( 0xffffffff ),	/* -1 */
/* 570 */	NdrFcShort( 0x0 ),	/* Corr flags:  */
/* 572 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 574 */	NdrFcShort( 0xffffff58 ),	/* Offset= -168 (406) */
/* 576 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 578 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 580 */	NdrFcShort( 0x10 ),	/* 16 */
/* 582 */	NdrFcShort( 0x0 ),	/* 0 */
/* 584 */	NdrFcShort( 0x6 ),	/* Offset= 6 (590) */
/* 586 */	0x8,		/* FC_LONG */
			0x40,		/* FC_STRUCTPAD4 */
/* 588 */	0x36,		/* FC_POINTER */
			0x5b,		/* FC_END */
/* 590 */	
			0x11, 0x0,	/* FC_RP */
/* 592 */	NdrFcShort( 0xffffffdc ),	/* Offset= -36 (556) */
/* 594 */	
			0x21,		/* FC_BOGUS_ARRAY */
			0x3,		/* 3 */
/* 596 */	NdrFcShort( 0x0 ),	/* 0 */
/* 598 */	0x19,		/* Corr desc:  field pointer, FC_ULONG */
			0x0,		/*  */
/* 600 */	NdrFcShort( 0x0 ),	/* 0 */
/* 602 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 604 */	NdrFcLong( 0xffffffff ),	/* -1 */
/* 608 */	NdrFcShort( 0x0 ),	/* Corr flags:  */
/* 610 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 612 */	NdrFcShort( 0xffffff44 ),	/* Offset= -188 (424) */
/* 614 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 616 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 618 */	NdrFcShort( 0x10 ),	/* 16 */
/* 620 */	NdrFcShort( 0x0 ),	/* 0 */
/* 622 */	NdrFcShort( 0x6 ),	/* Offset= 6 (628) */
/* 624 */	0x8,		/* FC_LONG */
			0x40,		/* FC_STRUCTPAD4 */
/* 626 */	0x36,		/* FC_POINTER */
			0x5b,		/* FC_END */
/* 628 */	
			0x11, 0x0,	/* FC_RP */
/* 630 */	NdrFcShort( 0xffffffdc ),	/* Offset= -36 (594) */
/* 632 */	
			0x21,		/* FC_BOGUS_ARRAY */
			0x3,		/* 3 */
/* 634 */	NdrFcShort( 0x0 ),	/* 0 */
/* 636 */	0x19,		/* Corr desc:  field pointer, FC_ULONG */
			0x0,		/*  */
/* 638 */	NdrFcShort( 0x0 ),	/* 0 */
/* 640 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 642 */	NdrFcLong( 0xffffffff ),	/* -1 */
/* 646 */	NdrFcShort( 0x0 ),	/* Corr flags:  */
/* 648 */	
			0x12, 0x0,	/* FC_UP */
/* 650 */	NdrFcShort( 0x160 ),	/* Offset= 352 (1002) */
/* 652 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 654 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 656 */	NdrFcShort( 0x10 ),	/* 16 */
/* 658 */	NdrFcShort( 0x0 ),	/* 0 */
/* 660 */	NdrFcShort( 0x6 ),	/* Offset= 6 (666) */
/* 662 */	0x8,		/* FC_LONG */
			0x40,		/* FC_STRUCTPAD4 */
/* 664 */	0x36,		/* FC_POINTER */
			0x5b,		/* FC_END */
/* 666 */	
			0x11, 0x0,	/* FC_RP */
/* 668 */	NdrFcShort( 0xffffffdc ),	/* Offset= -36 (632) */
/* 670 */	
			0x2f,		/* FC_IP */
			0x5a,		/* FC_CONSTANT_IID */
/* 672 */	NdrFcLong( 0x2f ),	/* 47 */
/* 676 */	NdrFcShort( 0x0 ),	/* 0 */
/* 678 */	NdrFcShort( 0x0 ),	/* 0 */
/* 680 */	0xc0,		/* 192 */
			0x0,		/* 0 */
/* 682 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 684 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 686 */	0x0,		/* 0 */
			0x46,		/* 70 */
/* 688 */	
			0x1b,		/* FC_CARRAY */
			0x0,		/* 0 */
/* 690 */	NdrFcShort( 0x1 ),	/* 1 */
/* 692 */	0x19,		/* Corr desc:  field pointer, FC_ULONG */
			0x0,		/*  */
/* 694 */	NdrFcShort( 0x4 ),	/* 4 */
/* 696 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 698 */	0x1,		/* FC_BYTE */
			0x5b,		/* FC_END */
/* 700 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 702 */	NdrFcShort( 0x18 ),	/* 24 */
/* 704 */	NdrFcShort( 0x0 ),	/* 0 */
/* 706 */	NdrFcShort( 0xa ),	/* Offset= 10 (716) */
/* 708 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 710 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 712 */	NdrFcShort( 0xffffffd6 ),	/* Offset= -42 (670) */
/* 714 */	0x36,		/* FC_POINTER */
			0x5b,		/* FC_END */
/* 716 */	
			0x12, 0x0,	/* FC_UP */
/* 718 */	NdrFcShort( 0xffffffe2 ),	/* Offset= -30 (688) */
/* 720 */	
			0x21,		/* FC_BOGUS_ARRAY */
			0x3,		/* 3 */
/* 722 */	NdrFcShort( 0x0 ),	/* 0 */
/* 724 */	0x19,		/* Corr desc:  field pointer, FC_ULONG */
			0x0,		/*  */
/* 726 */	NdrFcShort( 0x0 ),	/* 0 */
/* 728 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 730 */	NdrFcLong( 0xffffffff ),	/* -1 */
/* 734 */	NdrFcShort( 0x0 ),	/* Corr flags:  */
/* 736 */	
			0x12, 0x0,	/* FC_UP */
/* 738 */	NdrFcShort( 0xffffffda ),	/* Offset= -38 (700) */
/* 740 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 742 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 744 */	NdrFcShort( 0x10 ),	/* 16 */
/* 746 */	NdrFcShort( 0x0 ),	/* 0 */
/* 748 */	NdrFcShort( 0x6 ),	/* Offset= 6 (754) */
/* 750 */	0x8,		/* FC_LONG */
			0x40,		/* FC_STRUCTPAD4 */
/* 752 */	0x36,		/* FC_POINTER */
			0x5b,		/* FC_END */
/* 754 */	
			0x11, 0x0,	/* FC_RP */
/* 756 */	NdrFcShort( 0xffffffdc ),	/* Offset= -36 (720) */
/* 758 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 760 */	NdrFcShort( 0x20 ),	/* 32 */
/* 762 */	NdrFcShort( 0x0 ),	/* 0 */
/* 764 */	NdrFcShort( 0xa ),	/* Offset= 10 (774) */
/* 766 */	0x8,		/* FC_LONG */
			0x40,		/* FC_STRUCTPAD4 */
/* 768 */	0x36,		/* FC_POINTER */
			0x4c,		/* FC_EMBEDDED_COMPLEX */
/* 770 */	0x0,		/* 0 */
			NdrFcShort( 0xfffffd23 ),	/* Offset= -733 (38) */
			0x5b,		/* FC_END */
/* 774 */	
			0x11, 0x0,	/* FC_RP */
/* 776 */	NdrFcShort( 0xffffff24 ),	/* Offset= -220 (556) */
/* 778 */	
			0x1b,		/* FC_CARRAY */
			0x0,		/* 0 */
/* 780 */	NdrFcShort( 0x1 ),	/* 1 */
/* 782 */	0x19,		/* Corr desc:  field pointer, FC_ULONG */
			0x0,		/*  */
/* 784 */	NdrFcShort( 0x0 ),	/* 0 */
/* 786 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 788 */	0x1,		/* FC_BYTE */
			0x5b,		/* FC_END */
/* 790 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 792 */	NdrFcShort( 0x10 ),	/* 16 */
/* 794 */	NdrFcShort( 0x0 ),	/* 0 */
/* 796 */	NdrFcShort( 0x6 ),	/* Offset= 6 (802) */
/* 798 */	0x8,		/* FC_LONG */
			0x40,		/* FC_STRUCTPAD4 */
/* 800 */	0x36,		/* FC_POINTER */
			0x5b,		/* FC_END */
/* 802 */	
			0x12, 0x0,	/* FC_UP */
/* 804 */	NdrFcShort( 0xffffffe6 ),	/* Offset= -26 (778) */
/* 806 */	
			0x1b,		/* FC_CARRAY */
			0x1,		/* 1 */
/* 808 */	NdrFcShort( 0x2 ),	/* 2 */
/* 810 */	0x19,		/* Corr desc:  field pointer, FC_ULONG */
			0x0,		/*  */
/* 812 */	NdrFcShort( 0x0 ),	/* 0 */
/* 814 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 816 */	0x6,		/* FC_SHORT */
			0x5b,		/* FC_END */
/* 818 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 820 */	NdrFcShort( 0x10 ),	/* 16 */
/* 822 */	NdrFcShort( 0x0 ),	/* 0 */
/* 824 */	NdrFcShort( 0x6 ),	/* Offset= 6 (830) */
/* 826 */	0x8,		/* FC_LONG */
			0x40,		/* FC_STRUCTPAD4 */
/* 828 */	0x36,		/* FC_POINTER */
			0x5b,		/* FC_END */
/* 830 */	
			0x12, 0x0,	/* FC_UP */
/* 832 */	NdrFcShort( 0xffffffe6 ),	/* Offset= -26 (806) */
/* 834 */	
			0x1b,		/* FC_CARRAY */
			0x3,		/* 3 */
/* 836 */	NdrFcShort( 0x4 ),	/* 4 */
/* 838 */	0x19,		/* Corr desc:  field pointer, FC_ULONG */
			0x0,		/*  */
/* 840 */	NdrFcShort( 0x0 ),	/* 0 */
/* 842 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 844 */	0x8,		/* FC_LONG */
			0x5b,		/* FC_END */
/* 846 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 848 */	NdrFcShort( 0x10 ),	/* 16 */
/* 850 */	NdrFcShort( 0x0 ),	/* 0 */
/* 852 */	NdrFcShort( 0x6 ),	/* Offset= 6 (858) */
/* 854 */	0x8,		/* FC_LONG */
			0x40,		/* FC_STRUCTPAD4 */
/* 856 */	0x36,		/* FC_POINTER */
			0x5b,		/* FC_END */
/* 858 */	
			0x12, 0x0,	/* FC_UP */
/* 860 */	NdrFcShort( 0xffffffe6 ),	/* Offset= -26 (834) */
/* 862 */	
			0x1b,		/* FC_CARRAY */
			0x7,		/* 7 */
/* 864 */	NdrFcShort( 0x8 ),	/* 8 */
/* 866 */	0x19,		/* Corr desc:  field pointer, FC_ULONG */
			0x0,		/*  */
/* 868 */	NdrFcShort( 0x0 ),	/* 0 */
/* 870 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 872 */	0xb,		/* FC_HYPER */
			0x5b,		/* FC_END */
/* 874 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 876 */	NdrFcShort( 0x10 ),	/* 16 */
/* 878 */	NdrFcShort( 0x0 ),	/* 0 */
/* 880 */	NdrFcShort( 0x6 ),	/* Offset= 6 (886) */
/* 882 */	0x8,		/* FC_LONG */
			0x40,		/* FC_STRUCTPAD4 */
/* 884 */	0x36,		/* FC_POINTER */
			0x5b,		/* FC_END */
/* 886 */	
			0x12, 0x0,	/* FC_UP */
/* 888 */	NdrFcShort( 0xffffffe6 ),	/* Offset= -26 (862) */
/* 890 */	
			0x15,		/* FC_STRUCT */
			0x3,		/* 3 */
/* 892 */	NdrFcShort( 0x8 ),	/* 8 */
/* 894 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 896 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 898 */	
			0x1b,		/* FC_CARRAY */
			0x3,		/* 3 */
/* 900 */	NdrFcShort( 0x8 ),	/* 8 */
/* 902 */	0x7,		/* Corr desc: FC_USHORT */
			0x0,		/*  */
/* 904 */	NdrFcShort( 0xffc8 ),	/* -56 */
/* 906 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 908 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 910 */	NdrFcShort( 0xffffffec ),	/* Offset= -20 (890) */
/* 912 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 914 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 916 */	NdrFcShort( 0x38 ),	/* 56 */
/* 918 */	NdrFcShort( 0xffffffec ),	/* Offset= -20 (898) */
/* 920 */	NdrFcShort( 0x0 ),	/* Offset= 0 (920) */
/* 922 */	0x6,		/* FC_SHORT */
			0x6,		/* FC_SHORT */
/* 924 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 926 */	0x40,		/* FC_STRUCTPAD4 */
			0x4c,		/* FC_EMBEDDED_COMPLEX */
/* 928 */	0x0,		/* 0 */
			NdrFcShort( 0xfffffe21 ),	/* Offset= -479 (450) */
			0x5b,		/* FC_END */
/* 932 */	
			0x12, 0x0,	/* FC_UP */
/* 934 */	NdrFcShort( 0xffffff16 ),	/* Offset= -234 (700) */
/* 936 */	
			0x12, 0x8,	/* FC_UP [simple_pointer] */
/* 938 */	0x1,		/* FC_BYTE */
			0x5c,		/* FC_PAD */
/* 940 */	
			0x12, 0x8,	/* FC_UP [simple_pointer] */
/* 942 */	0x6,		/* FC_SHORT */
			0x5c,		/* FC_PAD */
/* 944 */	
			0x12, 0x8,	/* FC_UP [simple_pointer] */
/* 946 */	0x8,		/* FC_LONG */
			0x5c,		/* FC_PAD */
/* 948 */	
			0x12, 0x8,	/* FC_UP [simple_pointer] */
/* 950 */	0xa,		/* FC_FLOAT */
			0x5c,		/* FC_PAD */
/* 952 */	
			0x12, 0x8,	/* FC_UP [simple_pointer] */
/* 954 */	0xc,		/* FC_DOUBLE */
			0x5c,		/* FC_PAD */
/* 956 */	
			0x12, 0x0,	/* FC_UP */
/* 958 */	NdrFcShort( 0xfffffdb8 ),	/* Offset= -584 (374) */
/* 960 */	
			0x12, 0x10,	/* FC_UP [pointer_deref] */
/* 962 */	NdrFcShort( 0xfffffdba ),	/* Offset= -582 (380) */
/* 964 */	
			0x12, 0x10,	/* FC_UP [pointer_deref] */
/* 966 */	NdrFcShort( 0xfffffdd0 ),	/* Offset= -560 (406) */
/* 968 */	
			0x12, 0x10,	/* FC_UP [pointer_deref] */
/* 970 */	NdrFcShort( 0xfffffdde ),	/* Offset= -546 (424) */
/* 972 */	
			0x12, 0x10,	/* FC_UP [pointer_deref] */
/* 974 */	NdrFcShort( 0xfffffdec ),	/* Offset= -532 (442) */
/* 976 */	
			0x12, 0x10,	/* FC_UP [pointer_deref] */
/* 978 */	NdrFcShort( 0x2 ),	/* Offset= 2 (980) */
/* 980 */	
			0x12, 0x0,	/* FC_UP */
/* 982 */	NdrFcShort( 0x14 ),	/* Offset= 20 (1002) */
/* 984 */	
			0x15,		/* FC_STRUCT */
			0x7,		/* 7 */
/* 986 */	NdrFcShort( 0x10 ),	/* 16 */
/* 988 */	0x6,		/* FC_SHORT */
			0x1,		/* FC_BYTE */
/* 990 */	0x1,		/* FC_BYTE */
			0x8,		/* FC_LONG */
/* 992 */	0xb,		/* FC_HYPER */
			0x5b,		/* FC_END */
/* 994 */	
			0x12, 0x0,	/* FC_UP */
/* 996 */	NdrFcShort( 0xfffffff4 ),	/* Offset= -12 (984) */
/* 998 */	
			0x12, 0x8,	/* FC_UP [simple_pointer] */
/* 1000 */	0x2,		/* FC_CHAR */
			0x5c,		/* FC_PAD */
/* 1002 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x7,		/* 7 */
/* 1004 */	NdrFcShort( 0x20 ),	/* 32 */
/* 1006 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1008 */	NdrFcShort( 0x0 ),	/* Offset= 0 (1008) */
/* 1010 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 1012 */	0x6,		/* FC_SHORT */
			0x6,		/* FC_SHORT */
/* 1014 */	0x6,		/* FC_SHORT */
			0x6,		/* FC_SHORT */
/* 1016 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 1018 */	NdrFcShort( 0xfffffc6a ),	/* Offset= -918 (100) */
/* 1020 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 1022 */	0xb4,		/* FC_USER_MARSHAL */
			0x83,		/* 131 */
/* 1024 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1026 */	NdrFcShort( 0x18 ),	/* 24 */
/* 1028 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1030 */	NdrFcShort( 0xfffffc5a ),	/* Offset= -934 (96) */
/* 1032 */	
			0x21,		/* FC_BOGUS_ARRAY */
			0x3,		/* 3 */
/* 1034 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1036 */	0x19,		/* Corr desc:  field pointer, FC_ULONG */
			0x0,		/*  */
/* 1038 */	NdrFcShort( 0x10 ),	/* 16 */
/* 1040 */	NdrFcShort( 0x0 ),	/* Corr flags:  */
/* 1042 */	NdrFcLong( 0xffffffff ),	/* -1 */
/* 1046 */	NdrFcShort( 0x0 ),	/* Corr flags:  */
/* 1048 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 1050 */	NdrFcShort( 0xffffffe4 ),	/* Offset= -28 (1022) */
/* 1052 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 1054 */	
			0x1b,		/* FC_CARRAY */
			0x3,		/* 3 */
/* 1056 */	NdrFcShort( 0x4 ),	/* 4 */
/* 1058 */	0x19,		/* Corr desc:  field pointer, FC_ULONG */
			0x0,		/*  */
/* 1060 */	NdrFcShort( 0x14 ),	/* 20 */
/* 1062 */	NdrFcShort( 0x0 ),	/* Corr flags:  */
/* 1064 */	0x8,		/* FC_LONG */
			0x5b,		/* FC_END */
/* 1066 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 1068 */	NdrFcShort( 0x18 ),	/* 24 */
/* 1070 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1072 */	NdrFcShort( 0x8 ),	/* Offset= 8 (1080) */
/* 1074 */	0x36,		/* FC_POINTER */
			0x36,		/* FC_POINTER */
/* 1076 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 1078 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 1080 */	
			0x12, 0x0,	/* FC_UP */
/* 1082 */	NdrFcShort( 0xffffffce ),	/* Offset= -50 (1032) */
/* 1084 */	
			0x12, 0x0,	/* FC_UP */
/* 1086 */	NdrFcShort( 0xffffffe0 ),	/* Offset= -32 (1054) */
/* 1088 */	
			0x11, 0x4,	/* FC_RP [alloced_on_stack] */
/* 1090 */	NdrFcShort( 0x6 ),	/* Offset= 6 (1096) */
/* 1092 */	
			0x13, 0x0,	/* FC_OP */
/* 1094 */	NdrFcShort( 0xffffffa4 ),	/* Offset= -92 (1002) */
/* 1096 */	0xb4,		/* FC_USER_MARSHAL */
			0x83,		/* 131 */
/* 1098 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1100 */	NdrFcShort( 0x18 ),	/* 24 */
/* 1102 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1104 */	NdrFcShort( 0xfffffff4 ),	/* Offset= -12 (1092) */
/* 1106 */	
			0x11, 0x4,	/* FC_RP [alloced_on_stack] */
/* 1108 */	NdrFcShort( 0x10 ),	/* Offset= 16 (1124) */
/* 1110 */	
			0x13, 0x0,	/* FC_OP */
/* 1112 */	NdrFcShort( 0xfffffd34 ),	/* Offset= -716 (396) */
/* 1114 */	0xb4,		/* FC_USER_MARSHAL */
			0x83,		/* 131 */
/* 1116 */	NdrFcShort( 0x1 ),	/* 1 */
/* 1118 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1120 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1122 */	NdrFcShort( 0xfffffff4 ),	/* Offset= -12 (1110) */
/* 1124 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 1126 */	NdrFcShort( 0x30 ),	/* 48 */
/* 1128 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1130 */	NdrFcShort( 0x0 ),	/* Offset= 0 (1130) */
/* 1132 */	0x6,		/* FC_SHORT */
			0x6,		/* FC_SHORT */
/* 1134 */	0x40,		/* FC_STRUCTPAD4 */
			0x4c,		/* FC_EMBEDDED_COMPLEX */
/* 1136 */	0x0,		/* 0 */
			NdrFcShort( 0xffffffe9 ),	/* Offset= -23 (1114) */
			0x4c,		/* FC_EMBEDDED_COMPLEX */
/* 1140 */	0x0,		/* 0 */
			NdrFcShort( 0xffffffe5 ),	/* Offset= -27 (1114) */
			0x4c,		/* FC_EMBEDDED_COMPLEX */
/* 1144 */	0x0,		/* 0 */
			NdrFcShort( 0xffffffe1 ),	/* Offset= -31 (1114) */
			0x8,		/* FC_LONG */
/* 1148 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 1150 */	0x8,		/* FC_LONG */
			0x5b,		/* FC_END */
/* 1152 */	
			0x11, 0x0,	/* FC_RP */
/* 1154 */	NdrFcShort( 0x2 ),	/* Offset= 2 (1156) */
/* 1156 */	
			0x1b,		/* FC_CARRAY */
			0x3,		/* 3 */
/* 1158 */	NdrFcShort( 0x4 ),	/* 4 */
/* 1160 */	0x29,		/* Corr desc:  parameter, FC_ULONG */
			0x0,		/*  */
/* 1162 */	NdrFcShort( 0x48 ),	/* ia64 Stack size/offset = 72 */
/* 1164 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 1166 */	0x8,		/* FC_LONG */
			0x5b,		/* FC_END */
/* 1168 */	
			0x11, 0x0,	/* FC_RP */
/* 1170 */	NdrFcShort( 0x2 ),	/* Offset= 2 (1172) */
/* 1172 */	
			0x21,		/* FC_BOGUS_ARRAY */
			0x3,		/* 3 */
/* 1174 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1176 */	0x29,		/* Corr desc:  parameter, FC_ULONG */
			0x0,		/*  */
/* 1178 */	NdrFcShort( 0x48 ),	/* ia64 Stack size/offset = 72 */
/* 1180 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 1182 */	NdrFcLong( 0xffffffff ),	/* -1 */
/* 1186 */	NdrFcShort( 0x0 ),	/* Corr flags:  */
/* 1188 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 1190 */	NdrFcShort( 0xffffffa2 ),	/* Offset= -94 (1096) */
/* 1192 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 1194 */	
			0x11, 0x0,	/* FC_RP */
/* 1196 */	NdrFcShort( 0x2 ),	/* Offset= 2 (1198) */
/* 1198 */	
			0x21,		/* FC_BOGUS_ARRAY */
			0x3,		/* 3 */
/* 1200 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1202 */	0x29,		/* Corr desc:  parameter, FC_ULONG */
			0x0,		/*  */
/* 1204 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 1206 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 1208 */	0x29,		/* Corr desc:  parameter, FC_ULONG */
			0x54,		/* FC_DEREFERENCE */
/* 1210 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 1212 */	NdrFcShort( 0x0 ),	/* Corr flags:  */
/* 1214 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 1216 */	NdrFcShort( 0xffffff88 ),	/* Offset= -120 (1096) */
/* 1218 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 1220 */	
			0x11, 0x10,	/* FC_RP [pointer_deref] */
/* 1222 */	NdrFcShort( 0x2 ),	/* Offset= 2 (1224) */
/* 1224 */	
			0x2f,		/* FC_IP */
			0x5a,		/* FC_CONSTANT_IID */
/* 1226 */	NdrFcLong( 0x20404 ),	/* 132100 */
/* 1230 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1232 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1234 */	0xc0,		/* 192 */
			0x0,		/* 0 */
/* 1236 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 1238 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 1240 */	0x0,		/* 0 */
			0x46,		/* 70 */
/* 1242 */	
			0x11, 0x8,	/* FC_RP [simple_pointer] */
/* 1244 */	
			0x25,		/* FC_C_WSTRING */
			0x5c,		/* FC_PAD */
/* 1246 */	
			0x11, 0xc,	/* FC_RP [alloced_on_stack] [simple_pointer] */
/* 1248 */	0xe,		/* FC_ENUM32 */
			0x5c,		/* FC_PAD */
/* 1250 */	
			0x11, 0x14,	/* FC_RP [alloced_on_stack] [pointer_deref] */
/* 1252 */	NdrFcShort( 0x2 ),	/* Offset= 2 (1254) */
/* 1254 */	
			0x13, 0x0,	/* FC_OP */
/* 1256 */	NdrFcShort( 0xb6 ),	/* Offset= 182 (1438) */
/* 1258 */	
			0x2b,		/* FC_NON_ENCAPSULATED_UNION */
			0x7,		/* FC_USHORT */
/* 1260 */	0x7,		/* Corr desc: FC_USHORT */
			0x0,		/*  */
/* 1262 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1264 */	NdrFcShort( 0x0 ),	/* Corr flags:  */
/* 1266 */	NdrFcShort( 0x2 ),	/* Offset= 2 (1268) */
/* 1268 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1270 */	NdrFcShort( 0x4 ),	/* 4 */
/* 1272 */	NdrFcLong( 0x1a ),	/* 26 */
/* 1276 */	NdrFcShort( 0x16 ),	/* Offset= 22 (1298) */
/* 1278 */	NdrFcLong( 0x1b ),	/* 27 */
/* 1282 */	NdrFcShort( 0x10 ),	/* Offset= 16 (1298) */
/* 1284 */	NdrFcLong( 0x1c ),	/* 28 */
/* 1288 */	NdrFcShort( 0xe ),	/* Offset= 14 (1302) */
/* 1290 */	NdrFcLong( 0x1d ),	/* 29 */
/* 1294 */	NdrFcShort( 0x8008 ),	/* Simple arm type: FC_LONG */
/* 1296 */	NdrFcShort( 0x0 ),	/* Offset= 0 (1296) */
/* 1298 */	
			0x13, 0x0,	/* FC_OP */
/* 1300 */	NdrFcShort( 0x26 ),	/* Offset= 38 (1338) */
/* 1302 */	
			0x13, 0x0,	/* FC_OP */
/* 1304 */	NdrFcShort( 0x12 ),	/* Offset= 18 (1322) */
/* 1306 */	
			0x1b,		/* FC_CARRAY */
			0x3,		/* 3 */
/* 1308 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1310 */	0x7,		/* Corr desc: FC_USHORT */
			0x0,		/*  */
/* 1312 */	NdrFcShort( 0xfffc ),	/* -4 */
/* 1314 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 1316 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 1318 */	NdrFcShort( 0xfffffe54 ),	/* Offset= -428 (890) */
/* 1320 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 1322 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 1324 */	NdrFcShort( 0x4 ),	/* 4 */
/* 1326 */	NdrFcShort( 0xffffffec ),	/* Offset= -20 (1306) */
/* 1328 */	NdrFcShort( 0x0 ),	/* Offset= 0 (1328) */
/* 1330 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 1332 */	NdrFcShort( 0x6 ),	/* Offset= 6 (1338) */
/* 1334 */	0x6,		/* FC_SHORT */
			0x3e,		/* FC_STRUCTPAD2 */
/* 1336 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 1338 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 1340 */	NdrFcShort( 0x10 ),	/* 16 */
/* 1342 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1344 */	NdrFcShort( 0x0 ),	/* Offset= 0 (1344) */
/* 1346 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 1348 */	NdrFcShort( 0xffffffa6 ),	/* Offset= -90 (1258) */
/* 1350 */	0x6,		/* FC_SHORT */
			0x42,		/* FC_STRUCTPAD6 */
/* 1352 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 1354 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 1356 */	NdrFcShort( 0x20 ),	/* 32 */
/* 1358 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1360 */	NdrFcShort( 0x0 ),	/* Offset= 0 (1360) */
/* 1362 */	0x8,		/* FC_LONG */
			0x40,		/* FC_STRUCTPAD4 */
/* 1364 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 1366 */	NdrFcShort( 0xfffffef2 ),	/* Offset= -270 (1096) */
/* 1368 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 1370 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 1372 */	NdrFcShort( 0x10 ),	/* 16 */
/* 1374 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1376 */	NdrFcShort( 0x6 ),	/* Offset= 6 (1382) */
/* 1378 */	0x36,		/* FC_POINTER */
			0x6,		/* FC_SHORT */
/* 1380 */	0x42,		/* FC_STRUCTPAD6 */
			0x5b,		/* FC_END */
/* 1382 */	
			0x13, 0x0,	/* FC_OP */
/* 1384 */	NdrFcShort( 0xffffffe2 ),	/* Offset= -30 (1354) */
/* 1386 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 1388 */	NdrFcShort( 0x20 ),	/* 32 */
/* 1390 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1392 */	NdrFcShort( 0x0 ),	/* Offset= 0 (1392) */
/* 1394 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 1396 */	NdrFcShort( 0xffffffc6 ),	/* Offset= -58 (1338) */
/* 1398 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 1400 */	NdrFcShort( 0xffffffe2 ),	/* Offset= -30 (1370) */
/* 1402 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 1404 */	
			0x1b,		/* FC_CARRAY */
			0x3,		/* 3 */
/* 1406 */	NdrFcShort( 0x4 ),	/* 4 */
/* 1408 */	0x16,		/* Corr desc:  field pointer, FC_SHORT */
			0x0,		/*  */
/* 1410 */	NdrFcShort( 0x2a ),	/* 42 */
/* 1412 */	NdrFcShort( 0x0 ),	/* Corr flags:  */
/* 1414 */	0x8,		/* FC_LONG */
			0x5b,		/* FC_END */
/* 1416 */	
			0x21,		/* FC_BOGUS_ARRAY */
			0x3,		/* 3 */
/* 1418 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1420 */	0x16,		/* Corr desc:  field pointer, FC_SHORT */
			0x0,		/*  */
/* 1422 */	NdrFcShort( 0x24 ),	/* 36 */
/* 1424 */	NdrFcShort( 0x0 ),	/* Corr flags:  */
/* 1426 */	NdrFcLong( 0xffffffff ),	/* -1 */
/* 1430 */	NdrFcShort( 0x0 ),	/* Corr flags:  */
/* 1432 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 1434 */	NdrFcShort( 0xffffffd0 ),	/* Offset= -48 (1386) */
/* 1436 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 1438 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 1440 */	NdrFcShort( 0x58 ),	/* 88 */
/* 1442 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1444 */	NdrFcShort( 0x16 ),	/* Offset= 22 (1466) */
/* 1446 */	0x8,		/* FC_LONG */
			0x40,		/* FC_STRUCTPAD4 */
/* 1448 */	0x36,		/* FC_POINTER */
			0x36,		/* FC_POINTER */
/* 1450 */	0xe,		/* FC_ENUM32 */
			0xe,		/* FC_ENUM32 */
/* 1452 */	0xe,		/* FC_ENUM32 */
			0x6,		/* FC_SHORT */
/* 1454 */	0x6,		/* FC_SHORT */
			0x6,		/* FC_SHORT */
/* 1456 */	0x6,		/* FC_SHORT */
			0x40,		/* FC_STRUCTPAD4 */
/* 1458 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 1460 */	NdrFcShort( 0xffffffb6 ),	/* Offset= -74 (1386) */
/* 1462 */	0x6,		/* FC_SHORT */
			0x42,		/* FC_STRUCTPAD6 */
/* 1464 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 1466 */	
			0x13, 0x0,	/* FC_OP */
/* 1468 */	NdrFcShort( 0xffffffc0 ),	/* Offset= -64 (1404) */
/* 1470 */	
			0x13, 0x0,	/* FC_OP */
/* 1472 */	NdrFcShort( 0xffffffc8 ),	/* Offset= -56 (1416) */
/* 1474 */	
			0x11, 0x14,	/* FC_RP [alloced_on_stack] [pointer_deref] */
/* 1476 */	NdrFcShort( 0x2 ),	/* Offset= 2 (1478) */
/* 1478 */	
			0x13, 0x0,	/* FC_OP */
/* 1480 */	NdrFcShort( 0x2e ),	/* Offset= 46 (1526) */
/* 1482 */	
			0x2b,		/* FC_NON_ENCAPSULATED_UNION */
			0x8,		/* FC_LONG */
/* 1484 */	0x8,		/* Corr desc: FC_LONG */
			0x0,		/*  */
/* 1486 */	NdrFcShort( 0x2c ),	/* 44 */
/* 1488 */	NdrFcShort( 0x0 ),	/* Corr flags:  */
/* 1490 */	NdrFcShort( 0x2 ),	/* Offset= 2 (1492) */
/* 1492 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1494 */	NdrFcShort( 0x4 ),	/* 4 */
/* 1496 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1500 */	NdrFcShort( 0x8008 ),	/* Simple arm type: FC_LONG */
/* 1502 */	NdrFcLong( 0x3 ),	/* 3 */
/* 1506 */	NdrFcShort( 0x8008 ),	/* Simple arm type: FC_LONG */
/* 1508 */	NdrFcLong( 0x1 ),	/* 1 */
/* 1512 */	NdrFcShort( 0x8008 ),	/* Simple arm type: FC_LONG */
/* 1514 */	NdrFcLong( 0x2 ),	/* 2 */
/* 1518 */	NdrFcShort( 0x4 ),	/* Offset= 4 (1522) */
/* 1520 */	NdrFcShort( 0xffffffff ),	/* Offset= -1 (1519) */
/* 1522 */	
			0x13, 0x0,	/* FC_OP */
/* 1524 */	NdrFcShort( 0xfffffe54 ),	/* Offset= -428 (1096) */
/* 1526 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 1528 */	NdrFcShort( 0x40 ),	/* 64 */
/* 1530 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1532 */	NdrFcShort( 0x12 ),	/* Offset= 18 (1550) */
/* 1534 */	0x8,		/* FC_LONG */
			0x40,		/* FC_STRUCTPAD4 */
/* 1536 */	0x36,		/* FC_POINTER */
			0x4c,		/* FC_EMBEDDED_COMPLEX */
/* 1538 */	0x0,		/* 0 */
			NdrFcShort( 0xffffffc7 ),	/* Offset= -57 (1482) */
			0x4c,		/* FC_EMBEDDED_COMPLEX */
/* 1542 */	0x0,		/* 0 */
			NdrFcShort( 0xffffff63 ),	/* Offset= -157 (1386) */
			0x6,		/* FC_SHORT */
/* 1546 */	0x3e,		/* FC_STRUCTPAD2 */
			0xe,		/* FC_ENUM32 */
/* 1548 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 1550 */	
			0x13, 0x8,	/* FC_OP [simple_pointer] */
/* 1552 */	
			0x25,		/* FC_C_WSTRING */
			0x5c,		/* FC_PAD */
/* 1554 */	
			0x11, 0x10,	/* FC_RP [pointer_deref] */
/* 1556 */	NdrFcShort( 0x2 ),	/* Offset= 2 (1558) */
/* 1558 */	
			0x2f,		/* FC_IP */
			0x5a,		/* FC_CONSTANT_IID */
/* 1560 */	NdrFcLong( 0x20403 ),	/* 132099 */
/* 1564 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1566 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1568 */	0xc0,		/* 192 */
			0x0,		/* 0 */
/* 1570 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 1572 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 1574 */	0x0,		/* 0 */
			0x46,		/* 70 */
/* 1576 */	
			0x11, 0x4,	/* FC_RP [alloced_on_stack] */
/* 1578 */	NdrFcShort( 0x4 ),	/* Offset= 4 (1582) */
/* 1580 */	0x8,		/* FC_LONG */
			0x5c,		/* FC_PAD */
/* 1582 */	0xb4,		/* FC_USER_MARSHAL */
			0x3,		/* 3 */
/* 1584 */	NdrFcShort( 0x2 ),	/* 2 */
/* 1586 */	NdrFcShort( 0x18 ),	/* 24 */
/* 1588 */	NdrFcShort( 0x4 ),	/* 4 */
/* 1590 */	NdrFcShort( 0xfffffff6 ),	/* Offset= -10 (1580) */
/* 1592 */	
			0x11, 0x14,	/* FC_RP [alloced_on_stack] [pointer_deref] */
/* 1594 */	NdrFcShort( 0x2 ),	/* Offset= 2 (1596) */
/* 1596 */	
			0x13, 0x0,	/* FC_OP */
/* 1598 */	NdrFcShort( 0xe ),	/* Offset= 14 (1612) */
/* 1600 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 1602 */	NdrFcShort( 0x10 ),	/* 16 */
/* 1604 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1606 */	NdrFcShort( 0x0 ),	/* Offset= 0 (1606) */
/* 1608 */	0xb9,		/* FC_UINT3264 */
			0x6,		/* FC_SHORT */
/* 1610 */	0x42,		/* FC_STRUCTPAD6 */
			0x5b,		/* FC_END */
/* 1612 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 1614 */	NdrFcShort( 0x60 ),	/* 96 */
/* 1616 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1618 */	NdrFcShort( 0x1e ),	/* Offset= 30 (1648) */
/* 1620 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 1622 */	NdrFcShort( 0xfffff9d0 ),	/* Offset= -1584 (38) */
/* 1624 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 1626 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 1628 */	0x36,		/* FC_POINTER */
			0x8,		/* FC_LONG */
/* 1630 */	0xe,		/* FC_ENUM32 */
			0x6,		/* FC_SHORT */
/* 1632 */	0x6,		/* FC_SHORT */
			0x6,		/* FC_SHORT */
/* 1634 */	0x6,		/* FC_SHORT */
			0x6,		/* FC_SHORT */
/* 1636 */	0x6,		/* FC_SHORT */
			0x6,		/* FC_SHORT */
/* 1638 */	0x6,		/* FC_SHORT */
			0x4c,		/* FC_EMBEDDED_COMPLEX */
/* 1640 */	0x0,		/* 0 */
			NdrFcShort( 0xfffffed1 ),	/* Offset= -303 (1338) */
			0x4c,		/* FC_EMBEDDED_COMPLEX */
/* 1644 */	0x0,		/* 0 */
			NdrFcShort( 0xffffffd3 ),	/* Offset= -45 (1600) */
			0x5b,		/* FC_END */
/* 1648 */	
			0x13, 0x8,	/* FC_OP [simple_pointer] */
/* 1650 */	
			0x25,		/* FC_C_WSTRING */
			0x5c,		/* FC_PAD */
/* 1652 */	
			0x11, 0x4,	/* FC_RP [alloced_on_stack] */
/* 1654 */	NdrFcShort( 0x4 ),	/* Offset= 4 (1658) */
/* 1656 */	0x8,		/* FC_LONG */
			0x5c,		/* FC_PAD */
/* 1658 */	0xb4,		/* FC_USER_MARSHAL */
			0x3,		/* 3 */
/* 1660 */	NdrFcShort( 0x2 ),	/* 2 */
/* 1662 */	NdrFcShort( 0x18 ),	/* 24 */
/* 1664 */	NdrFcShort( 0x4 ),	/* 4 */
/* 1666 */	NdrFcShort( 0xfffffff6 ),	/* Offset= -10 (1656) */
/* 1668 */	
			0x11, 0x4,	/* FC_RP [alloced_on_stack] */
/* 1670 */	NdrFcShort( 0x4 ),	/* Offset= 4 (1674) */
/* 1672 */	0x8,		/* FC_LONG */
			0x5c,		/* FC_PAD */
/* 1674 */	0xb4,		/* FC_USER_MARSHAL */
			0x3,		/* 3 */
/* 1676 */	NdrFcShort( 0x2 ),	/* 2 */
/* 1678 */	NdrFcShort( 0x18 ),	/* 24 */
/* 1680 */	NdrFcShort( 0x4 ),	/* 4 */
/* 1682 */	NdrFcShort( 0xfffffff6 ),	/* Offset= -10 (1672) */
/* 1684 */	
			0x11, 0x4,	/* FC_RP [alloced_on_stack] */
/* 1686 */	NdrFcShort( 0x4 ),	/* Offset= 4 (1690) */
/* 1688 */	0x8,		/* FC_LONG */
			0x5c,		/* FC_PAD */
/* 1690 */	0xb4,		/* FC_USER_MARSHAL */
			0x3,		/* 3 */
/* 1692 */	NdrFcShort( 0x2 ),	/* 2 */
/* 1694 */	NdrFcShort( 0x18 ),	/* 24 */
/* 1696 */	NdrFcShort( 0x4 ),	/* 4 */
/* 1698 */	NdrFcShort( 0xfffffff6 ),	/* Offset= -10 (1688) */
/* 1700 */	
			0x11, 0x0,	/* FC_RP */
/* 1702 */	NdrFcShort( 0x2 ),	/* Offset= 2 (1704) */
/* 1704 */	
			0x21,		/* FC_BOGUS_ARRAY */
			0x3,		/* 3 */
/* 1706 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1708 */	0x29,		/* Corr desc:  parameter, FC_ULONG */
			0x0,		/*  */
/* 1710 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 1712 */	NdrFcShort( 0x0 ),	/* Corr flags:  */
/* 1714 */	0x29,		/* Corr desc:  parameter, FC_ULONG */
			0x54,		/* FC_DEREFERENCE */
/* 1716 */	NdrFcShort( 0x20 ),	/* ia64 Stack size/offset = 32 */
/* 1718 */	NdrFcShort( 0x0 ),	/* Corr flags:  */
/* 1720 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 1722 */	NdrFcShort( 0xfffffda0 ),	/* Offset= -608 (1114) */
/* 1724 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 1726 */	
			0x11, 0x4,	/* FC_RP [alloced_on_stack] */
/* 1728 */	NdrFcShort( 0xfffffd9a ),	/* Offset= -614 (1114) */
/* 1730 */	
			0x11, 0xc,	/* FC_RP [alloced_on_stack] [simple_pointer] */
/* 1732 */	0x6,		/* FC_SHORT */
			0x5c,		/* FC_PAD */
/* 1734 */	
			0x11, 0x10,	/* FC_RP [pointer_deref] */
/* 1736 */	NdrFcShort( 0x2 ),	/* Offset= 2 (1738) */
/* 1738 */	
			0x2f,		/* FC_IP */
			0x5c,		/* FC_PAD */
/* 1740 */	0x2b,		/* Corr desc:  parameter, FC_HYPER */
			0x0,		/*  */
/* 1742 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 1744 */	NdrFcShort( 0x5 ),	/* Corr flags:  early, iid_is, */
/* 1746 */	
			0x11, 0x10,	/* FC_RP [pointer_deref] */
/* 1748 */	NdrFcShort( 0x2 ),	/* Offset= 2 (1750) */
/* 1750 */	
			0x2f,		/* FC_IP */
			0x5a,		/* FC_CONSTANT_IID */
/* 1752 */	NdrFcLong( 0x20402 ),	/* 132098 */
/* 1756 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1758 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1760 */	0xc0,		/* 192 */
			0x0,		/* 0 */
/* 1762 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 1764 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 1766 */	0x0,		/* 0 */
			0x46,		/* 70 */
/* 1768 */	
			0x11, 0x4,	/* FC_RP [alloced_on_stack] */
/* 1770 */	NdrFcShort( 0x2a ),	/* Offset= 42 (1812) */
/* 1772 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 1774 */	NdrFcShort( 0x28 ),	/* 40 */
/* 1776 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1778 */	NdrFcShort( 0x0 ),	/* Offset= 0 (1778) */
/* 1780 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 1782 */	NdrFcShort( 0xfffff930 ),	/* Offset= -1744 (38) */
/* 1784 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 1786 */	NdrFcShort( 0xfffffd4e ),	/* Offset= -690 (1096) */
/* 1788 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 1790 */	
			0x21,		/* FC_BOGUS_ARRAY */
			0x3,		/* 3 */
/* 1792 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1794 */	0x19,		/* Corr desc:  field pointer, FC_ULONG */
			0x0,		/*  */
/* 1796 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1798 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 1800 */	NdrFcLong( 0xffffffff ),	/* -1 */
/* 1804 */	NdrFcShort( 0x0 ),	/* Corr flags:  */
/* 1806 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 1808 */	NdrFcShort( 0xffffffdc ),	/* Offset= -36 (1772) */
/* 1810 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 1812 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 1814 */	NdrFcShort( 0x10 ),	/* 16 */
/* 1816 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1818 */	NdrFcShort( 0x6 ),	/* Offset= 6 (1824) */
/* 1820 */	0x8,		/* FC_LONG */
			0x40,		/* FC_STRUCTPAD4 */
/* 1822 */	0x36,		/* FC_POINTER */
			0x5b,		/* FC_END */
/* 1824 */	
			0x13, 0x0,	/* FC_OP */
/* 1826 */	NdrFcShort( 0xffffffdc ),	/* Offset= -36 (1790) */
/* 1828 */	
			0x11, 0x14,	/* FC_RP [alloced_on_stack] [pointer_deref] */
/* 1830 */	NdrFcShort( 0x2 ),	/* Offset= 2 (1832) */
/* 1832 */	
			0x13, 0x0,	/* FC_OP */
/* 1834 */	NdrFcShort( 0x2 ),	/* Offset= 2 (1836) */
/* 1836 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 1838 */	NdrFcShort( 0x20 ),	/* 32 */
/* 1840 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1842 */	NdrFcShort( 0x0 ),	/* Offset= 0 (1842) */
/* 1844 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 1846 */	NdrFcShort( 0xfffff8f0 ),	/* Offset= -1808 (38) */
/* 1848 */	0x8,		/* FC_LONG */
			0xe,		/* FC_ENUM32 */
/* 1850 */	0x6,		/* FC_SHORT */
			0x6,		/* FC_SHORT */
/* 1852 */	0x6,		/* FC_SHORT */
			0x3e,		/* FC_STRUCTPAD2 */
/* 1854 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 1856 */	
			0x11, 0x4,	/* FC_RP [alloced_on_stack] */
/* 1858 */	NdrFcShort( 0x4 ),	/* Offset= 4 (1862) */
/* 1860 */	0x8,		/* FC_LONG */
			0x5c,		/* FC_PAD */
/* 1862 */	0xb4,		/* FC_USER_MARSHAL */
			0x3,		/* 3 */
/* 1864 */	NdrFcShort( 0x2 ),	/* 2 */
/* 1866 */	NdrFcShort( 0x18 ),	/* 24 */
/* 1868 */	NdrFcShort( 0x4 ),	/* 4 */
/* 1870 */	NdrFcShort( 0xfffffff6 ),	/* Offset= -10 (1860) */
/* 1872 */	
			0x11, 0x0,	/* FC_RP */
/* 1874 */	NdrFcShort( 0x2 ),	/* Offset= 2 (1876) */
/* 1876 */	
			0x21,		/* FC_BOGUS_ARRAY */
			0x3,		/* 3 */
/* 1878 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1880 */	0x27,		/* Corr desc:  parameter, FC_USHORT */
			0x54,		/* FC_DEREFERENCE */
/* 1882 */	NdrFcShort( 0x28 ),	/* ia64 Stack size/offset = 40 */
/* 1884 */	NdrFcShort( 0x0 ),	/* Corr flags:  */
/* 1886 */	0x27,		/* Corr desc:  parameter, FC_USHORT */
			0x54,		/* FC_DEREFERENCE */
/* 1888 */	NdrFcShort( 0x28 ),	/* ia64 Stack size/offset = 40 */
/* 1890 */	NdrFcShort( 0x0 ),	/* Corr flags:  */
/* 1892 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 1894 */	NdrFcShort( 0xfffff8a4 ),	/* Offset= -1884 (10) */
/* 1896 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 1898 */	
			0x11, 0x0,	/* FC_RP */
/* 1900 */	NdrFcShort( 0x2 ),	/* Offset= 2 (1902) */
/* 1902 */	
			0x1c,		/* FC_CVARRAY */
			0x3,		/* 3 */
/* 1904 */	NdrFcShort( 0x4 ),	/* 4 */
/* 1906 */	0x27,		/* Corr desc:  parameter, FC_USHORT */
			0x54,		/* FC_DEREFERENCE */
/* 1908 */	NdrFcShort( 0x28 ),	/* ia64 Stack size/offset = 40 */
/* 1910 */	NdrFcShort( 0x0 ),	/* Corr flags:  */
/* 1912 */	0x27,		/* Corr desc:  parameter, FC_USHORT */
			0x54,		/* FC_DEREFERENCE */
/* 1914 */	NdrFcShort( 0x28 ),	/* ia64 Stack size/offset = 40 */
/* 1916 */	NdrFcShort( 0x0 ),	/* Corr flags:  */
/* 1918 */	0x8,		/* FC_LONG */
			0x5b,		/* FC_END */
/* 1920 */	
			0x11, 0x8,	/* FC_RP [simple_pointer] */
/* 1922 */	0x6,		/* FC_SHORT */
			0x5c,		/* FC_PAD */
/* 1924 */	
			0x11, 0x4,	/* FC_RP [alloced_on_stack] */
/* 1926 */	NdrFcShort( 0xfffff8a0 ),	/* Offset= -1888 (38) */
/* 1928 */	
			0x11, 0x10,	/* FC_RP [pointer_deref] */
/* 1930 */	NdrFcShort( 0x2 ),	/* Offset= 2 (1932) */
/* 1932 */	
			0x2f,		/* FC_IP */
			0x5c,		/* FC_PAD */
/* 1934 */	0x2b,		/* Corr desc:  parameter, FC_HYPER */
			0x0,		/*  */
/* 1936 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 1938 */	NdrFcShort( 0x5 ),	/* Corr flags:  early, iid_is, */
/* 1940 */	
			0x11, 0x0,	/* FC_RP */
/* 1942 */	NdrFcShort( 0xfffffcce ),	/* Offset= -818 (1124) */
/* 1944 */	
			0x2f,		/* FC_IP */
			0x5a,		/* FC_CONSTANT_IID */
/* 1946 */	NdrFcLong( 0x3127ca40 ),	/* 824691264 */
/* 1950 */	NdrFcShort( 0x446e ),	/* 17518 */
/* 1952 */	NdrFcShort( 0x11ce ),	/* 4558 */
/* 1954 */	0x81,		/* 129 */
			0x35,		/* 53 */
/* 1956 */	0x0,		/* 0 */
			0xaa,		/* 170 */
/* 1958 */	0x0,		/* 0 */
			0x4b,		/* 75 */
/* 1960 */	0xb8,		/* 184 */
			0x51,		/* 81 */
/* 1962 */	0x11, 0x0,	/* FC_RP */
/* 1964 */	NdrFcShort( 0xfffffc52 ),	/* Offset= -942 (1022) */

			0x0
        }
    };

static const USER_MARSHAL_ROUTINE_QUADRUPLE UserMarshalRoutines[ WIRE_MARSHAL_TABLE_SIZE ] = 
        {
            
            {
            VARIANT_UserSize
            ,VARIANT_UserMarshal
            ,VARIANT_UserUnmarshal
            ,VARIANT_UserFree
            },
            {
            BSTR_UserSize
            ,BSTR_UserMarshal
            ,BSTR_UserUnmarshal
            ,BSTR_UserFree
            },
            {
            CLEANLOCALSTORAGE_UserSize
            ,CLEANLOCALSTORAGE_UserMarshal
            ,CLEANLOCALSTORAGE_UserUnmarshal
            ,CLEANLOCALSTORAGE_UserFree
            }

        };



/* Standard interface: __MIDL_itf_oaidl_0000, ver. 0.0,
   GUID={0x00000000,0x0000,0x0000,{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}} */


/* Standard interface: IOleAutomationTypes, ver. 1.0,
   GUID={0x00000000,0x0000,0x0000,{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}} */


/* Object interface: IUnknown, ver. 0.0,
   GUID={0x00000000,0x0000,0x0000,{0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x46}} */


/* Object interface: ICreateTypeInfo, ver. 0.0,
   GUID={0x00020405,0x0000,0x0000,{0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x46}} */


/* Object interface: ICreateTypeInfo2, ver. 0.0,
   GUID={0x0002040E,0x0000,0x0000,{0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x46}} */


/* Object interface: ICreateTypeLib, ver. 0.0,
   GUID={0x00020406,0x0000,0x0000,{0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x46}} */


/* Object interface: ICreateTypeLib2, ver. 0.0,
   GUID={0x0002040F,0x0000,0x0000,{0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x46}} */


/* Object interface: IDispatch, ver. 0.0,
   GUID={0x00020400,0x0000,0x0000,{0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x46}} */

#pragma code_seg(".orpc")
static const unsigned short IDispatch_FormatStringOffsetTable[] =
    {
    0,
    38,
    88,
    150
    };

static const MIDL_STUBLESS_PROXY_INFO IDispatch_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &IDispatch_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const STUB_THUNK IDispatch_StubThunkTable[] = 
    {
    0,
    0,
    0,
    IDispatch_RemoteInvoke_Thunk
    };

static const MIDL_SERVER_INFO IDispatch_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &IDispatch_FormatStringOffsetTable[-3],
    &IDispatch_StubThunkTable[-3],
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(7) _IDispatchProxyVtbl = 
{
    &IDispatch_ProxyInfo,
    &IID_IDispatch,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    (void *) (INT_PTR) -1 /* IDispatch::GetTypeInfoCount */ ,
    (void *) (INT_PTR) -1 /* IDispatch::GetTypeInfo */ ,
    (void *) (INT_PTR) -1 /* IDispatch::GetIDsOfNames */ ,
    IDispatch_Invoke_Proxy
};

const CInterfaceStubVtbl _IDispatchStubVtbl =
{
    &IID_IDispatch,
    &IDispatch_ServerInfo,
    7,
    0, /* pure interpreted */
    CStdStubBuffer_METHODS
};


/* Object interface: IEnumVARIANT, ver. 0.0,
   GUID={0x00020404,0x0000,0x0000,{0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x46}} */

#pragma code_seg(".orpc")
static const unsigned short IEnumVARIANT_FormatStringOffsetTable[] =
    {
    248,
    298,
    336,
    368
    };

static const MIDL_STUBLESS_PROXY_INFO IEnumVARIANT_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &IEnumVARIANT_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const STUB_THUNK IEnumVARIANT_StubThunkTable[] = 
    {
    IEnumVARIANT_RemoteNext_Thunk,
    0,
    0,
    0
    };

static const MIDL_SERVER_INFO IEnumVARIANT_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &IEnumVARIANT_FormatStringOffsetTable[-3],
    &IEnumVARIANT_StubThunkTable[-3],
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(7) _IEnumVARIANTProxyVtbl = 
{
    &IEnumVARIANT_ProxyInfo,
    &IID_IEnumVARIANT,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    IEnumVARIANT_Next_Proxy ,
    (void *) (INT_PTR) -1 /* IEnumVARIANT::Skip */ ,
    (void *) (INT_PTR) -1 /* IEnumVARIANT::Reset */ ,
    (void *) (INT_PTR) -1 /* IEnumVARIANT::Clone */
};

const CInterfaceStubVtbl _IEnumVARIANTStubVtbl =
{
    &IID_IEnumVARIANT,
    &IEnumVARIANT_ServerInfo,
    7,
    0, /* pure interpreted */
    CStdStubBuffer_METHODS
};


/* Object interface: ITypeComp, ver. 0.0,
   GUID={0x00020403,0x0000,0x0000,{0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x46}} */

#pragma code_seg(".orpc")
static const unsigned short ITypeComp_FormatStringOffsetTable[] =
    {
    406,
    492
    };

static const MIDL_STUBLESS_PROXY_INFO ITypeComp_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &ITypeComp_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const STUB_THUNK ITypeComp_StubThunkTable[] = 
    {
    ITypeComp_RemoteBind_Thunk,
    ITypeComp_RemoteBindType_Thunk
    };

static const MIDL_SERVER_INFO ITypeComp_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &ITypeComp_FormatStringOffsetTable[-3],
    &ITypeComp_StubThunkTable[-3],
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(5) _ITypeCompProxyVtbl = 
{
    &ITypeComp_ProxyInfo,
    &IID_ITypeComp,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    ITypeComp_Bind_Proxy ,
    ITypeComp_BindType_Proxy
};

const CInterfaceStubVtbl _ITypeCompStubVtbl =
{
    &IID_ITypeComp,
    &ITypeComp_ServerInfo,
    5,
    0, /* pure interpreted */
    CStdStubBuffer_METHODS
};


/* Object interface: ITypeInfo, ver. 0.0,
   GUID={0x00020401,0x0000,0x0000,{0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x46}} */

#pragma code_seg(".orpc")
static const unsigned short ITypeInfo_FormatStringOffsetTable[] =
    {
    542,
    586,
    624,
    674,
    724,
    780,
    824,
    868,
    900,
    932,
    1000,
    1068,
    1112,
    1144,
    1188,
    1232,
    1276,
    1308,
    1340
    };

static const MIDL_STUBLESS_PROXY_INFO ITypeInfo_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &ITypeInfo_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const STUB_THUNK ITypeInfo_StubThunkTable[] = 
    {
    ITypeInfo_RemoteGetTypeAttr_Thunk,
    0,
    ITypeInfo_RemoteGetFuncDesc_Thunk,
    ITypeInfo_RemoteGetVarDesc_Thunk,
    ITypeInfo_RemoteGetNames_Thunk,
    0,
    0,
    ITypeInfo_LocalGetIDsOfNames_Thunk,
    ITypeInfo_LocalInvoke_Thunk,
    ITypeInfo_RemoteGetDocumentation_Thunk,
    ITypeInfo_RemoteGetDllEntry_Thunk,
    0,
    ITypeInfo_LocalAddressOfMember_Thunk,
    ITypeInfo_RemoteCreateInstance_Thunk,
    0,
    ITypeInfo_RemoteGetContainingTypeLib_Thunk,
    ITypeInfo_LocalReleaseTypeAttr_Thunk,
    ITypeInfo_LocalReleaseFuncDesc_Thunk,
    ITypeInfo_LocalReleaseVarDesc_Thunk
    };

static const MIDL_SERVER_INFO ITypeInfo_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &ITypeInfo_FormatStringOffsetTable[-3],
    &ITypeInfo_StubThunkTable[-3],
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(22) _ITypeInfoProxyVtbl = 
{
    &ITypeInfo_ProxyInfo,
    &IID_ITypeInfo,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    ITypeInfo_GetTypeAttr_Proxy ,
    (void *) (INT_PTR) -1 /* ITypeInfo::GetTypeComp */ ,
    ITypeInfo_GetFuncDesc_Proxy ,
    ITypeInfo_GetVarDesc_Proxy ,
    ITypeInfo_GetNames_Proxy ,
    (void *) (INT_PTR) -1 /* ITypeInfo::GetRefTypeOfImplType */ ,
    (void *) (INT_PTR) -1 /* ITypeInfo::GetImplTypeFlags */ ,
    ITypeInfo_GetIDsOfNames_Proxy ,
    ITypeInfo_Invoke_Proxy ,
    ITypeInfo_GetDocumentation_Proxy ,
    ITypeInfo_GetDllEntry_Proxy ,
    (void *) (INT_PTR) -1 /* ITypeInfo::GetRefTypeInfo */ ,
    ITypeInfo_AddressOfMember_Proxy ,
    ITypeInfo_CreateInstance_Proxy ,
    (void *) (INT_PTR) -1 /* ITypeInfo::GetMops */ ,
    ITypeInfo_GetContainingTypeLib_Proxy ,
    ITypeInfo_ReleaseTypeAttr_Proxy ,
    ITypeInfo_ReleaseFuncDesc_Proxy ,
    ITypeInfo_ReleaseVarDesc_Proxy
};

const CInterfaceStubVtbl _ITypeInfoStubVtbl =
{
    &IID_ITypeInfo,
    &ITypeInfo_ServerInfo,
    22,
    0, /* pure interpreted */
    CStdStubBuffer_METHODS
};


/* Object interface: ITypeInfo2, ver. 0.0,
   GUID={0x00020412,0x0000,0x0000,{0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x46}} */

#pragma code_seg(".orpc")
static const unsigned short ITypeInfo2_FormatStringOffsetTable[] =
    {
    542,
    586,
    624,
    674,
    724,
    780,
    824,
    868,
    900,
    932,
    1000,
    1068,
    1112,
    1144,
    1188,
    1232,
    1276,
    1308,
    1340,
    1372,
    1410,
    1448,
    1498,
    1542,
    1586,
    1636,
    1692,
    1742,
    1792,
    1860,
    1898,
    1942,
    1992,
    2036
    };

static const MIDL_STUBLESS_PROXY_INFO ITypeInfo2_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &ITypeInfo2_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const STUB_THUNK ITypeInfo2_StubThunkTable[] = 
    {
    ITypeInfo_RemoteGetTypeAttr_Thunk,
    0,
    ITypeInfo_RemoteGetFuncDesc_Thunk,
    ITypeInfo_RemoteGetVarDesc_Thunk,
    ITypeInfo_RemoteGetNames_Thunk,
    0,
    0,
    ITypeInfo_LocalGetIDsOfNames_Thunk,
    ITypeInfo_LocalInvoke_Thunk,
    ITypeInfo_RemoteGetDocumentation_Thunk,
    ITypeInfo_RemoteGetDllEntry_Thunk,
    0,
    ITypeInfo_LocalAddressOfMember_Thunk,
    ITypeInfo_RemoteCreateInstance_Thunk,
    0,
    ITypeInfo_RemoteGetContainingTypeLib_Thunk,
    ITypeInfo_LocalReleaseTypeAttr_Thunk,
    ITypeInfo_LocalReleaseFuncDesc_Thunk,
    ITypeInfo_LocalReleaseVarDesc_Thunk,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    ITypeInfo2_RemoteGetDocumentation2_Thunk,
    0,
    0,
    0,
    0,
    0
    };

static const MIDL_SERVER_INFO ITypeInfo2_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &ITypeInfo2_FormatStringOffsetTable[-3],
    &ITypeInfo2_StubThunkTable[-3],
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(37) _ITypeInfo2ProxyVtbl = 
{
    &ITypeInfo2_ProxyInfo,
    &IID_ITypeInfo2,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    ITypeInfo_GetTypeAttr_Proxy ,
    (void *) (INT_PTR) -1 /* ITypeInfo::GetTypeComp */ ,
    ITypeInfo_GetFuncDesc_Proxy ,
    ITypeInfo_GetVarDesc_Proxy ,
    ITypeInfo_GetNames_Proxy ,
    (void *) (INT_PTR) -1 /* ITypeInfo::GetRefTypeOfImplType */ ,
    (void *) (INT_PTR) -1 /* ITypeInfo::GetImplTypeFlags */ ,
    ITypeInfo_GetIDsOfNames_Proxy ,
    ITypeInfo_Invoke_Proxy ,
    ITypeInfo_GetDocumentation_Proxy ,
    ITypeInfo_GetDllEntry_Proxy ,
    (void *) (INT_PTR) -1 /* ITypeInfo::GetRefTypeInfo */ ,
    ITypeInfo_AddressOfMember_Proxy ,
    ITypeInfo_CreateInstance_Proxy ,
    (void *) (INT_PTR) -1 /* ITypeInfo::GetMops */ ,
    ITypeInfo_GetContainingTypeLib_Proxy ,
    ITypeInfo_ReleaseTypeAttr_Proxy ,
    ITypeInfo_ReleaseFuncDesc_Proxy ,
    ITypeInfo_ReleaseVarDesc_Proxy ,
    (void *) (INT_PTR) -1 /* ITypeInfo2::GetTypeKind */ ,
    (void *) (INT_PTR) -1 /* ITypeInfo2::GetTypeFlags */ ,
    (void *) (INT_PTR) -1 /* ITypeInfo2::GetFuncIndexOfMemId */ ,
    (void *) (INT_PTR) -1 /* ITypeInfo2::GetVarIndexOfMemId */ ,
    (void *) (INT_PTR) -1 /* ITypeInfo2::GetCustData */ ,
    (void *) (INT_PTR) -1 /* ITypeInfo2::GetFuncCustData */ ,
    (void *) (INT_PTR) -1 /* ITypeInfo2::GetParamCustData */ ,
    (void *) (INT_PTR) -1 /* ITypeInfo2::GetVarCustData */ ,
    (void *) (INT_PTR) -1 /* ITypeInfo2::GetImplTypeCustData */ ,
    ITypeInfo2_GetDocumentation2_Proxy ,
    (void *) (INT_PTR) -1 /* ITypeInfo2::GetAllCustData */ ,
    (void *) (INT_PTR) -1 /* ITypeInfo2::GetAllFuncCustData */ ,
    (void *) (INT_PTR) -1 /* ITypeInfo2::GetAllParamCustData */ ,
    (void *) (INT_PTR) -1 /* ITypeInfo2::GetAllVarCustData */ ,
    (void *) (INT_PTR) -1 /* ITypeInfo2::GetAllImplTypeCustData */
};

const CInterfaceStubVtbl _ITypeInfo2StubVtbl =
{
    &IID_ITypeInfo2,
    &ITypeInfo2_ServerInfo,
    37,
    0, /* pure interpreted */
    CStdStubBuffer_METHODS
};


/* Object interface: ITypeLib, ver. 0.0,
   GUID={0x00020402,0x0000,0x0000,{0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x46}} */

#pragma code_seg(".orpc")
static const unsigned short ITypeLib_FormatStringOffsetTable[] =
    {
    0,
    2080,
    2124,
    2168,
    2212,
    2256,
    2294,
    2362,
    2418,
    2486
    };

static const MIDL_STUBLESS_PROXY_INFO ITypeLib_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &ITypeLib_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const STUB_THUNK ITypeLib_StubThunkTable[] = 
    {
    ITypeLib_RemoteGetTypeInfoCount_Thunk,
    0,
    0,
    0,
    ITypeLib_RemoteGetLibAttr_Thunk,
    0,
    ITypeLib_RemoteGetDocumentation_Thunk,
    ITypeLib_RemoteIsName_Thunk,
    ITypeLib_RemoteFindName_Thunk,
    ITypeLib_LocalReleaseTLibAttr_Thunk
    };

static const MIDL_SERVER_INFO ITypeLib_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &ITypeLib_FormatStringOffsetTable[-3],
    &ITypeLib_StubThunkTable[-3],
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(13) _ITypeLibProxyVtbl = 
{
    &ITypeLib_ProxyInfo,
    &IID_ITypeLib,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    ITypeLib_GetTypeInfoCount_Proxy ,
    (void *) (INT_PTR) -1 /* ITypeLib::GetTypeInfo */ ,
    (void *) (INT_PTR) -1 /* ITypeLib::GetTypeInfoType */ ,
    (void *) (INT_PTR) -1 /* ITypeLib::GetTypeInfoOfGuid */ ,
    ITypeLib_GetLibAttr_Proxy ,
    (void *) (INT_PTR) -1 /* ITypeLib::GetTypeComp */ ,
    ITypeLib_GetDocumentation_Proxy ,
    ITypeLib_IsName_Proxy ,
    ITypeLib_FindName_Proxy ,
    ITypeLib_ReleaseTLibAttr_Proxy
};

const CInterfaceStubVtbl _ITypeLibStubVtbl =
{
    &IID_ITypeLib,
    &ITypeLib_ServerInfo,
    13,
    0, /* pure interpreted */
    CStdStubBuffer_METHODS
};


/* Object interface: ITypeLib2, ver. 0.0,
   GUID={0x00020411,0x0000,0x0000,{0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x46}} */

#pragma code_seg(".orpc")
static const unsigned short ITypeLib2_FormatStringOffsetTable[] =
    {
    0,
    2080,
    2124,
    2168,
    2212,
    2256,
    2294,
    2362,
    2418,
    2486,
    2518,
    2562,
    2606,
    2674
    };

static const MIDL_STUBLESS_PROXY_INFO ITypeLib2_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &ITypeLib2_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const STUB_THUNK ITypeLib2_StubThunkTable[] = 
    {
    ITypeLib_RemoteGetTypeInfoCount_Thunk,
    0,
    0,
    0,
    ITypeLib_RemoteGetLibAttr_Thunk,
    0,
    ITypeLib_RemoteGetDocumentation_Thunk,
    ITypeLib_RemoteIsName_Thunk,
    ITypeLib_RemoteFindName_Thunk,
    ITypeLib_LocalReleaseTLibAttr_Thunk,
    0,
    ITypeLib2_RemoteGetLibStatistics_Thunk,
    ITypeLib2_RemoteGetDocumentation2_Thunk,
    0
    };

static const MIDL_SERVER_INFO ITypeLib2_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &ITypeLib2_FormatStringOffsetTable[-3],
    &ITypeLib2_StubThunkTable[-3],
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(17) _ITypeLib2ProxyVtbl = 
{
    &ITypeLib2_ProxyInfo,
    &IID_ITypeLib2,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    ITypeLib_GetTypeInfoCount_Proxy ,
    (void *) (INT_PTR) -1 /* ITypeLib::GetTypeInfo */ ,
    (void *) (INT_PTR) -1 /* ITypeLib::GetTypeInfoType */ ,
    (void *) (INT_PTR) -1 /* ITypeLib::GetTypeInfoOfGuid */ ,
    ITypeLib_GetLibAttr_Proxy ,
    (void *) (INT_PTR) -1 /* ITypeLib::GetTypeComp */ ,
    ITypeLib_GetDocumentation_Proxy ,
    ITypeLib_IsName_Proxy ,
    ITypeLib_FindName_Proxy ,
    ITypeLib_ReleaseTLibAttr_Proxy ,
    (void *) (INT_PTR) -1 /* ITypeLib2::GetCustData */ ,
    ITypeLib2_GetLibStatistics_Proxy ,
    ITypeLib2_GetDocumentation2_Proxy ,
    (void *) (INT_PTR) -1 /* ITypeLib2::GetAllCustData */
};

const CInterfaceStubVtbl _ITypeLib2StubVtbl =
{
    &IID_ITypeLib2,
    &ITypeLib2_ServerInfo,
    17,
    0, /* pure interpreted */
    CStdStubBuffer_METHODS
};


/* Object interface: ITypeChangeEvents, ver. 0.0,
   GUID={0x00020410,0x0000,0x0000,{0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x46}} */


/* Object interface: IErrorInfo, ver. 0.0,
   GUID={0x1CF2B120,0x547D,0x101B,{0x8E,0x65,0x08,0x00,0x2B,0x2B,0xD1,0x19}} */

#pragma code_seg(".orpc")
static const unsigned short IErrorInfo_FormatStringOffsetTable[] =
    {
    2712,
    2750,
    2788,
    2826,
    2864
    };

static const MIDL_STUBLESS_PROXY_INFO IErrorInfo_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &IErrorInfo_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IErrorInfo_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &IErrorInfo_FormatStringOffsetTable[-3],
    0,
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(8) _IErrorInfoProxyVtbl = 
{
    &IErrorInfo_ProxyInfo,
    &IID_IErrorInfo,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    (void *) (INT_PTR) -1 /* IErrorInfo::GetGUID */ ,
    (void *) (INT_PTR) -1 /* IErrorInfo::GetSource */ ,
    (void *) (INT_PTR) -1 /* IErrorInfo::GetDescription */ ,
    (void *) (INT_PTR) -1 /* IErrorInfo::GetHelpFile */ ,
    (void *) (INT_PTR) -1 /* IErrorInfo::GetHelpContext */
};

const CInterfaceStubVtbl _IErrorInfoStubVtbl =
{
    &IID_IErrorInfo,
    &IErrorInfo_ServerInfo,
    8,
    0, /* pure interpreted */
    CStdStubBuffer_METHODS
};


/* Object interface: ICreateErrorInfo, ver. 0.0,
   GUID={0x22F03340,0x547D,0x101B,{0x8E,0x65,0x08,0x00,0x2B,0x2B,0xD1,0x19}} */

#pragma code_seg(".orpc")
static const unsigned short ICreateErrorInfo_FormatStringOffsetTable[] =
    {
    2902,
    2940,
    2978,
    3016,
    3054
    };

static const MIDL_STUBLESS_PROXY_INFO ICreateErrorInfo_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &ICreateErrorInfo_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO ICreateErrorInfo_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &ICreateErrorInfo_FormatStringOffsetTable[-3],
    0,
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(8) _ICreateErrorInfoProxyVtbl = 
{
    &ICreateErrorInfo_ProxyInfo,
    &IID_ICreateErrorInfo,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    (void *) (INT_PTR) -1 /* ICreateErrorInfo::SetGUID */ ,
    (void *) (INT_PTR) -1 /* ICreateErrorInfo::SetSource */ ,
    (void *) (INT_PTR) -1 /* ICreateErrorInfo::SetDescription */ ,
    (void *) (INT_PTR) -1 /* ICreateErrorInfo::SetHelpFile */ ,
    (void *) (INT_PTR) -1 /* ICreateErrorInfo::SetHelpContext */
};

const CInterfaceStubVtbl _ICreateErrorInfoStubVtbl =
{
    &IID_ICreateErrorInfo,
    &ICreateErrorInfo_ServerInfo,
    8,
    0, /* pure interpreted */
    CStdStubBuffer_METHODS
};


/* Object interface: ISupportErrorInfo, ver. 0.0,
   GUID={0xDF0B3D60,0x548F,0x101B,{0x8E,0x65,0x08,0x00,0x2B,0x2B,0xD1,0x19}} */

#pragma code_seg(".orpc")
static const unsigned short ISupportErrorInfo_FormatStringOffsetTable[] =
    {
    2902
    };

static const MIDL_STUBLESS_PROXY_INFO ISupportErrorInfo_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &ISupportErrorInfo_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO ISupportErrorInfo_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &ISupportErrorInfo_FormatStringOffsetTable[-3],
    0,
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(4) _ISupportErrorInfoProxyVtbl = 
{
    &ISupportErrorInfo_ProxyInfo,
    &IID_ISupportErrorInfo,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    (void *) (INT_PTR) -1 /* ISupportErrorInfo::InterfaceSupportsErrorInfo */
};

const CInterfaceStubVtbl _ISupportErrorInfoStubVtbl =
{
    &IID_ISupportErrorInfo,
    &ISupportErrorInfo_ServerInfo,
    4,
    0, /* pure interpreted */
    CStdStubBuffer_METHODS
};


/* Object interface: ITypeFactory, ver. 0.0,
   GUID={0x0000002E,0x0000,0x0000,{0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x46}} */

#pragma code_seg(".orpc")
static const unsigned short ITypeFactory_FormatStringOffsetTable[] =
    {
    3092
    };

static const MIDL_STUBLESS_PROXY_INFO ITypeFactory_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &ITypeFactory_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO ITypeFactory_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &ITypeFactory_FormatStringOffsetTable[-3],
    0,
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(4) _ITypeFactoryProxyVtbl = 
{
    &ITypeFactory_ProxyInfo,
    &IID_ITypeFactory,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    (void *) (INT_PTR) -1 /* ITypeFactory::CreateFromTypeInfo */
};

const CInterfaceStubVtbl _ITypeFactoryStubVtbl =
{
    &IID_ITypeFactory,
    &ITypeFactory_ServerInfo,
    4,
    0, /* pure interpreted */
    CStdStubBuffer_METHODS
};


/* Object interface: ITypeMarshal, ver. 0.0,
   GUID={0x0000002D,0x0000,0x0000,{0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x46}} */


/* Object interface: IRecordInfo, ver. 0.0,
   GUID={0x0000002F,0x0000,0x0000,{0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x46}} */


/* Object interface: IErrorLog, ver. 0.0,
   GUID={0x3127CA40,0x446E,0x11CE,{0x81,0x35,0x00,0xAA,0x00,0x4B,0xB8,0x51}} */

#pragma code_seg(".orpc")
static const unsigned short IErrorLog_FormatStringOffsetTable[] =
    {
    3142
    };

static const MIDL_STUBLESS_PROXY_INFO IErrorLog_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &IErrorLog_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IErrorLog_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &IErrorLog_FormatStringOffsetTable[-3],
    0,
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(4) _IErrorLogProxyVtbl = 
{
    &IErrorLog_ProxyInfo,
    &IID_IErrorLog,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    (void *) (INT_PTR) -1 /* IErrorLog::AddError */
};

const CInterfaceStubVtbl _IErrorLogStubVtbl =
{
    &IID_IErrorLog,
    &IErrorLog_ServerInfo,
    4,
    0, /* pure interpreted */
    CStdStubBuffer_METHODS
};


/* Object interface: IPropertyBag, ver. 0.0,
   GUID={0x55272A00,0x42CB,0x11CE,{0x81,0x35,0x00,0xAA,0x00,0x4B,0xB8,0x51}} */

#pragma code_seg(".orpc")
static const unsigned short IPropertyBag_FormatStringOffsetTable[] =
    {
    3186,
    3248
    };

static const MIDL_STUBLESS_PROXY_INFO IPropertyBag_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &IPropertyBag_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const STUB_THUNK IPropertyBag_StubThunkTable[] = 
    {
    IPropertyBag_RemoteRead_Thunk,
    0
    };

static const MIDL_SERVER_INFO IPropertyBag_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &IPropertyBag_FormatStringOffsetTable[-3],
    &IPropertyBag_StubThunkTable[-3],
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(5) _IPropertyBagProxyVtbl = 
{
    &IPropertyBag_ProxyInfo,
    &IID_IPropertyBag,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    IPropertyBag_Read_Proxy ,
    (void *) (INT_PTR) -1 /* IPropertyBag::Write */
};

const CInterfaceStubVtbl _IPropertyBagStubVtbl =
{
    &IID_IPropertyBag,
    &IPropertyBag_ServerInfo,
    5,
    0, /* pure interpreted */
    CStdStubBuffer_METHODS
};


/* Standard interface: __MIDL_itf_oaidl_0103, ver. 0.0,
   GUID={0x00000000,0x0000,0x0000,{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}} */

static const MIDL_STUB_DESC Object_StubDesc = 
    {
    0,
    NdrOleAllocate,
    NdrOleFree,
    0,
    0,
    0,
    0,
    0,
    __MIDL_TypeFormatString.Format,
    1, /* -error bounds_check flag */
    0x50002, /* Ndr library version */
    0,
    0x600015b, /* MIDL Version 6.0.347 */
    0,
    UserMarshalRoutines,
    0,  /* notify & notify_flag routine table */
    0x1, /* MIDL flag */
    0, /* cs routines */
    0,   /* proxy/server info */
    0   /* Reserved5 */
    };

const CInterfaceProxyVtbl * _oaidl_ProxyVtblList[] = 
{
    ( CInterfaceProxyVtbl *) &_IDispatchProxyVtbl,
    ( CInterfaceProxyVtbl *) &_IPropertyBagProxyVtbl,
    ( CInterfaceProxyVtbl *) &_ITypeInfoProxyVtbl,
    ( CInterfaceProxyVtbl *) &_ITypeLibProxyVtbl,
    ( CInterfaceProxyVtbl *) &_ITypeCompProxyVtbl,
    ( CInterfaceProxyVtbl *) &_IEnumVARIANTProxyVtbl,
    ( CInterfaceProxyVtbl *) &_ITypeLib2ProxyVtbl,
    ( CInterfaceProxyVtbl *) &_ITypeInfo2ProxyVtbl,
    ( CInterfaceProxyVtbl *) &_IErrorInfoProxyVtbl,
    ( CInterfaceProxyVtbl *) &_ITypeFactoryProxyVtbl,
    ( CInterfaceProxyVtbl *) &_ICreateErrorInfoProxyVtbl,
    ( CInterfaceProxyVtbl *) &_IErrorLogProxyVtbl,
    ( CInterfaceProxyVtbl *) &_ISupportErrorInfoProxyVtbl,
    0
};

const CInterfaceStubVtbl * _oaidl_StubVtblList[] = 
{
    ( CInterfaceStubVtbl *) &_IDispatchStubVtbl,
    ( CInterfaceStubVtbl *) &_IPropertyBagStubVtbl,
    ( CInterfaceStubVtbl *) &_ITypeInfoStubVtbl,
    ( CInterfaceStubVtbl *) &_ITypeLibStubVtbl,
    ( CInterfaceStubVtbl *) &_ITypeCompStubVtbl,
    ( CInterfaceStubVtbl *) &_IEnumVARIANTStubVtbl,
    ( CInterfaceStubVtbl *) &_ITypeLib2StubVtbl,
    ( CInterfaceStubVtbl *) &_ITypeInfo2StubVtbl,
    ( CInterfaceStubVtbl *) &_IErrorInfoStubVtbl,
    ( CInterfaceStubVtbl *) &_ITypeFactoryStubVtbl,
    ( CInterfaceStubVtbl *) &_ICreateErrorInfoStubVtbl,
    ( CInterfaceStubVtbl *) &_IErrorLogStubVtbl,
    ( CInterfaceStubVtbl *) &_ISupportErrorInfoStubVtbl,
    0
};

PCInterfaceName const _oaidl_InterfaceNamesList[] = 
{
    "IDispatch",
    "IPropertyBag",
    "ITypeInfo",
    "ITypeLib",
    "ITypeComp",
    "IEnumVARIANT",
    "ITypeLib2",
    "ITypeInfo2",
    "IErrorInfo",
    "ITypeFactory",
    "ICreateErrorInfo",
    "IErrorLog",
    "ISupportErrorInfo",
    0
};


#define _oaidl_CHECK_IID(n)	IID_GENERIC_CHECK_IID( _oaidl, pIID, n)

int __stdcall _oaidl_IID_Lookup( const IID * pIID, int * pIndex )
{
    IID_BS_LOOKUP_SETUP

    IID_BS_LOOKUP_INITIAL_TEST( _oaidl, 13, 8 )
    IID_BS_LOOKUP_NEXT_TEST( _oaidl, 4 )
    IID_BS_LOOKUP_NEXT_TEST( _oaidl, 2 )
    IID_BS_LOOKUP_NEXT_TEST( _oaidl, 1 )
    IID_BS_LOOKUP_RETURN_RESULT( _oaidl, 13, *pIndex )
    
}

const ExtendedProxyFileInfo oaidl_ProxyFileInfo = 
{
    (PCInterfaceProxyVtblList *) & _oaidl_ProxyVtblList,
    (PCInterfaceStubVtblList *) & _oaidl_StubVtblList,
    (const PCInterfaceName * ) & _oaidl_InterfaceNamesList,
    0, // no delegation
    & _oaidl_IID_Lookup, 
    13,
    2,
    0, /* table of [async_uuid] interfaces */
    0, /* Filler1 */
    0, /* Filler2 */
    0  /* Filler3 */
};


#endif /* defined(_M_IA64) || defined(_M_AMD64)*/

