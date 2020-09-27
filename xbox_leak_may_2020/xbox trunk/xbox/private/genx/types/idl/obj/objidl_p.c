
#pragma warning( disable: 4049 )  /* more than 64k source lines */

/* this ALWAYS GENERATED file contains the proxy stub code */


 /* File created by MIDL compiler version 6.00.0347 */
/* at Sun May 05 19:07:27 2002
 */
/* Compiler settings for objidl.idl:
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


#include "objidl.h"

#define TYPE_FORMAT_STRING_SIZE   1743                              
#define PROC_FORMAT_STRING_SIZE   6127                              
#define TRANSMIT_AS_TABLE_SIZE    0            
#define WIRE_MARSHAL_TABLE_SIZE   8            

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


extern const MIDL_SERVER_INFO IEnumUnknown_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO IEnumUnknown_ProxyInfo;

/* [call_as] */ HRESULT STDMETHODCALLTYPE IEnumUnknown_RemoteNext_Proxy( 
    IEnumUnknown * This,
    /* [in] */ ULONG celt,
    /* [length_is][size_is][out] */ IUnknown **rgelt,
    /* [out] */ ULONG *pceltFetched)
{
CLIENT_CALL_RETURN _RetVal;

_RetVal = NdrClientCall2(
                  ( PMIDL_STUB_DESC  )&Object_StubDesc,
                  (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[0],
                  ( unsigned char * )&This);
return ( HRESULT  )_RetVal.Simple;

}

void __RPC_API
IEnumUnknown_RemoteNext_Thunk(
    PMIDL_STUB_MESSAGE pStubMsg )
{
    #pragma pack(4)
    struct _PARAM_STRUCT
        {
        IEnumUnknown *This;
        ULONG celt;
        IUnknown **rgelt;
        ULONG *pceltFetched;
        HRESULT _RetVal;
        };
    #pragma pack()
    struct _PARAM_STRUCT * pParamStruct;

    pParamStruct = (struct _PARAM_STRUCT *) pStubMsg->StackTop;
    
    /* Call the server */
    
    pParamStruct->_RetVal = IEnumUnknown_Next_Stub(
                                 (IEnumUnknown *) pParamStruct->This,
                                 pParamStruct->celt,
                                 pParamStruct->rgelt,
                                 pParamStruct->pceltFetched);
    
}


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO IBindCtx_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO IBindCtx_ProxyInfo;

/* [call_as] */ HRESULT STDMETHODCALLTYPE IBindCtx_RemoteSetBindOptions_Proxy( 
    IBindCtx * This,
    /* [in] */ BIND_OPTS2 *pbindopts)
{
CLIENT_CALL_RETURN _RetVal;

_RetVal = NdrClientCall2(
                  ( PMIDL_STUB_DESC  )&Object_StubDesc,
                  (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[222],
                  ( unsigned char * )&This);
return ( HRESULT  )_RetVal.Simple;

}

void __RPC_API
IBindCtx_RemoteSetBindOptions_Thunk(
    PMIDL_STUB_MESSAGE pStubMsg )
{
    #pragma pack(4)
    struct _PARAM_STRUCT
        {
        IBindCtx *This;
        BIND_OPTS2 *pbindopts;
        HRESULT _RetVal;
        };
    #pragma pack()
    struct _PARAM_STRUCT * pParamStruct;

    pParamStruct = (struct _PARAM_STRUCT *) pStubMsg->StackTop;
    
    /* Call the server */
    
    pParamStruct->_RetVal = IBindCtx_SetBindOptions_Stub((IBindCtx *) pParamStruct->This,pParamStruct->pbindopts);
    
}

/* [call_as] */ HRESULT STDMETHODCALLTYPE IBindCtx_RemoteGetBindOptions_Proxy( 
    IBindCtx * This,
    /* [out][in] */ BIND_OPTS2 *pbindopts)
{
CLIENT_CALL_RETURN _RetVal;

_RetVal = NdrClientCall2(
                  ( PMIDL_STUB_DESC  )&Object_StubDesc,
                  (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[258],
                  ( unsigned char * )&This);
return ( HRESULT  )_RetVal.Simple;

}

void __RPC_API
IBindCtx_RemoteGetBindOptions_Thunk(
    PMIDL_STUB_MESSAGE pStubMsg )
{
    #pragma pack(4)
    struct _PARAM_STRUCT
        {
        IBindCtx *This;
        BIND_OPTS2 *pbindopts;
        HRESULT _RetVal;
        };
    #pragma pack()
    struct _PARAM_STRUCT * pParamStruct;

    pParamStruct = (struct _PARAM_STRUCT *) pStubMsg->StackTop;
    
    /* Call the server */
    
    pParamStruct->_RetVal = IBindCtx_GetBindOptions_Stub((IBindCtx *) pParamStruct->This,pParamStruct->pbindopts);
    
}


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO IEnumMoniker_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO IEnumMoniker_ProxyInfo;

/* [call_as] */ HRESULT STDMETHODCALLTYPE IEnumMoniker_RemoteNext_Proxy( 
    IEnumMoniker * This,
    /* [in] */ ULONG celt,
    /* [length_is][size_is][out] */ IMoniker **rgelt,
    /* [out] */ ULONG *pceltFetched)
{
CLIENT_CALL_RETURN _RetVal;

_RetVal = NdrClientCall2(
                  ( PMIDL_STUB_DESC  )&Object_StubDesc,
                  (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[486],
                  ( unsigned char * )&This);
return ( HRESULT  )_RetVal.Simple;

}

void __RPC_API
IEnumMoniker_RemoteNext_Thunk(
    PMIDL_STUB_MESSAGE pStubMsg )
{
    #pragma pack(4)
    struct _PARAM_STRUCT
        {
        IEnumMoniker *This;
        ULONG celt;
        IMoniker **rgelt;
        ULONG *pceltFetched;
        HRESULT _RetVal;
        };
    #pragma pack()
    struct _PARAM_STRUCT * pParamStruct;

    pParamStruct = (struct _PARAM_STRUCT *) pStubMsg->StackTop;
    
    /* Call the server */
    
    pParamStruct->_RetVal = IEnumMoniker_Next_Stub(
                                 (IEnumMoniker *) pParamStruct->This,
                                 pParamStruct->celt,
                                 pParamStruct->rgelt,
                                 pParamStruct->pceltFetched);
    
}


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO IRunnableObject_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO IRunnableObject_ProxyInfo;

/* [call_as] */ HRESULT STDMETHODCALLTYPE IRunnableObject_RemoteIsRunning_Proxy( 
    IRunnableObject * This)
{
CLIENT_CALL_RETURN _RetVal;

_RetVal = NdrClientCall2(
                  ( PMIDL_STUB_DESC  )&Object_StubDesc,
                  (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[84],
                  ( unsigned char * )&This);
return ( HRESULT  )_RetVal.Simple;

}

void __RPC_API
IRunnableObject_RemoteIsRunning_Thunk(
    PMIDL_STUB_MESSAGE pStubMsg )
{
    #pragma pack(4)
    struct _PARAM_STRUCT
        {
        IRunnableObject *This;
        HRESULT _RetVal;
        };
    #pragma pack()
    struct _PARAM_STRUCT * pParamStruct;

    pParamStruct = (struct _PARAM_STRUCT *) pStubMsg->StackTop;
    
    /* Call the server */
    
    pParamStruct->_RetVal = IRunnableObject_IsRunning_Stub((IRunnableObject *) pParamStruct->This);
    
}


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO IRunningObjectTable_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO IRunningObjectTable_ProxyInfo;


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO IPersist_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO IPersist_ProxyInfo;


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO IPersistStream_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO IPersistStream_ProxyInfo;


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO IMoniker_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO IMoniker_ProxyInfo;

/* [call_as] */ HRESULT STDMETHODCALLTYPE IMoniker_RemoteBindToObject_Proxy( 
    IMoniker * This,
    /* [unique][in] */ IBindCtx *pbc,
    /* [unique][in] */ IMoniker *pmkToLeft,
    /* [in] */ REFIID riidResult,
    /* [iid_is][out] */ IUnknown **ppvResult)
{
CLIENT_CALL_RETURN _RetVal;

_RetVal = NdrClientCall2(
                  ( PMIDL_STUB_DESC  )&Object_StubDesc,
                  (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[1116],
                  ( unsigned char * )&This);
return ( HRESULT  )_RetVal.Simple;

}

void __RPC_API
IMoniker_RemoteBindToObject_Thunk(
    PMIDL_STUB_MESSAGE pStubMsg )
{
    #pragma pack(4)
    struct _PARAM_STRUCT
        {
        IMoniker *This;
        IBindCtx *pbc;
        IMoniker *pmkToLeft;
        REFIID riidResult;
        IUnknown **ppvResult;
        HRESULT _RetVal;
        };
    #pragma pack()
    struct _PARAM_STRUCT * pParamStruct;

    pParamStruct = (struct _PARAM_STRUCT *) pStubMsg->StackTop;
    
    /* Call the server */
    
    pParamStruct->_RetVal = IMoniker_BindToObject_Stub(
                                     (IMoniker *) pParamStruct->This,
                                     pParamStruct->pbc,
                                     pParamStruct->pmkToLeft,
                                     pParamStruct->riidResult,
                                     pParamStruct->ppvResult);
    
}

/* [call_as] */ HRESULT STDMETHODCALLTYPE IMoniker_RemoteBindToStorage_Proxy( 
    IMoniker * This,
    /* [unique][in] */ IBindCtx *pbc,
    /* [unique][in] */ IMoniker *pmkToLeft,
    /* [in] */ REFIID riid,
    /* [iid_is][out] */ IUnknown **ppvObj)
{
CLIENT_CALL_RETURN _RetVal;

_RetVal = NdrClientCall2(
                  ( PMIDL_STUB_DESC  )&Object_StubDesc,
                  (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[1170],
                  ( unsigned char * )&This);
return ( HRESULT  )_RetVal.Simple;

}

void __RPC_API
IMoniker_RemoteBindToStorage_Thunk(
    PMIDL_STUB_MESSAGE pStubMsg )
{
    #pragma pack(4)
    struct _PARAM_STRUCT
        {
        IMoniker *This;
        IBindCtx *pbc;
        IMoniker *pmkToLeft;
        REFIID riid;
        IUnknown **ppvObj;
        HRESULT _RetVal;
        };
    #pragma pack()
    struct _PARAM_STRUCT * pParamStruct;

    pParamStruct = (struct _PARAM_STRUCT *) pStubMsg->StackTop;
    
    /* Call the server */
    
    pParamStruct->_RetVal = IMoniker_BindToStorage_Stub(
                                      (IMoniker *) pParamStruct->This,
                                      pParamStruct->pbc,
                                      pParamStruct->pmkToLeft,
                                      pParamStruct->riid,
                                      pParamStruct->ppvObj);
    
}


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO IROTData_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO IROTData_ProxyInfo;


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO IEnumString_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO IEnumString_ProxyInfo;

/* [call_as] */ HRESULT STDMETHODCALLTYPE IEnumString_RemoteNext_Proxy( 
    IEnumString * This,
    /* [in] */ ULONG celt,
    /* [length_is][size_is][out] */ LPOLESTR *rgelt,
    /* [out] */ ULONG *pceltFetched)
{
CLIENT_CALL_RETURN _RetVal;

_RetVal = NdrClientCall2(
                  ( PMIDL_STUB_DESC  )&Object_StubDesc,
                  (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[1848],
                  ( unsigned char * )&This);
return ( HRESULT  )_RetVal.Simple;

}

void __RPC_API
IEnumString_RemoteNext_Thunk(
    PMIDL_STUB_MESSAGE pStubMsg )
{
    #pragma pack(4)
    struct _PARAM_STRUCT
        {
        IEnumString *This;
        ULONG celt;
        LPOLESTR *rgelt;
        ULONG *pceltFetched;
        HRESULT _RetVal;
        };
    #pragma pack()
    struct _PARAM_STRUCT * pParamStruct;

    pParamStruct = (struct _PARAM_STRUCT *) pStubMsg->StackTop;
    
    /* Call the server */
    
    pParamStruct->_RetVal = IEnumString_Next_Stub(
                                (IEnumString *) pParamStruct->This,
                                pParamStruct->celt,
                                pParamStruct->rgelt,
                                pParamStruct->pceltFetched);
    
}


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO ISequentialStream_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO ISequentialStream_ProxyInfo;

/* [call_as] */ HRESULT STDMETHODCALLTYPE ISequentialStream_RemoteRead_Proxy( 
    ISequentialStream * This,
    /* [length_is][size_is][out] */ byte *pv,
    /* [in] */ ULONG cb,
    /* [out] */ ULONG *pcbRead)
{
CLIENT_CALL_RETURN _RetVal;

_RetVal = NdrClientCall2(
                  ( PMIDL_STUB_DESC  )&Object_StubDesc,
                  (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[1932],
                  ( unsigned char * )&This);
return ( HRESULT  )_RetVal.Simple;

}

void __RPC_API
ISequentialStream_RemoteRead_Thunk(
    PMIDL_STUB_MESSAGE pStubMsg )
{
    #pragma pack(4)
    struct _PARAM_STRUCT
        {
        ISequentialStream *This;
        byte *pv;
        ULONG cb;
        ULONG *pcbRead;
        HRESULT _RetVal;
        };
    #pragma pack()
    struct _PARAM_STRUCT * pParamStruct;

    pParamStruct = (struct _PARAM_STRUCT *) pStubMsg->StackTop;
    
    /* Call the server */
    
    pParamStruct->_RetVal = ISequentialStream_Read_Stub(
                                      (ISequentialStream *) pParamStruct->This,
                                      pParamStruct->pv,
                                      pParamStruct->cb,
                                      pParamStruct->pcbRead);
    
}

/* [call_as] */ HRESULT STDMETHODCALLTYPE ISequentialStream_RemoteWrite_Proxy( 
    ISequentialStream * This,
    /* [size_is][in] */ const byte *pv,
    /* [in] */ ULONG cb,
    /* [out] */ ULONG *pcbWritten)
{
CLIENT_CALL_RETURN _RetVal;

_RetVal = NdrClientCall2(
                  ( PMIDL_STUB_DESC  )&Object_StubDesc,
                  (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[1980],
                  ( unsigned char * )&This);
return ( HRESULT  )_RetVal.Simple;

}

void __RPC_API
ISequentialStream_RemoteWrite_Thunk(
    PMIDL_STUB_MESSAGE pStubMsg )
{
    #pragma pack(4)
    struct _PARAM_STRUCT
        {
        ISequentialStream *This;
        const byte *pv;
        ULONG cb;
        ULONG *pcbWritten;
        HRESULT _RetVal;
        };
    #pragma pack()
    struct _PARAM_STRUCT * pParamStruct;

    pParamStruct = (struct _PARAM_STRUCT *) pStubMsg->StackTop;
    
    /* Call the server */
    
    pParamStruct->_RetVal = ISequentialStream_Write_Stub(
                                       (ISequentialStream *) pParamStruct->This,
                                       pParamStruct->pv,
                                       pParamStruct->cb,
                                       pParamStruct->pcbWritten);
    
}


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO IStream_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO IStream_ProxyInfo;

/* [call_as] */ HRESULT STDMETHODCALLTYPE IStream_RemoteSeek_Proxy( 
    IStream * This,
    /* [in] */ LARGE_INTEGER dlibMove,
    /* [in] */ DWORD dwOrigin,
    /* [out] */ ULARGE_INTEGER *plibNewPosition)
{
CLIENT_CALL_RETURN _RetVal;

_RetVal = NdrClientCall2(
                  ( PMIDL_STUB_DESC  )&Object_StubDesc,
                  (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[2028],
                  ( unsigned char * )&This);
return ( HRESULT  )_RetVal.Simple;

}

void __RPC_API
IStream_RemoteSeek_Thunk(
    PMIDL_STUB_MESSAGE pStubMsg )
{
    #pragma pack(4)
    struct _PARAM_STRUCT
        {
        IStream *This;
        LARGE_INTEGER dlibMove;
        DWORD dwOrigin;
        ULARGE_INTEGER *plibNewPosition;
        HRESULT _RetVal;
        };
    #pragma pack()
    struct _PARAM_STRUCT * pParamStruct;

    pParamStruct = (struct _PARAM_STRUCT *) pStubMsg->StackTop;
    
    /* Call the server */
    
    pParamStruct->_RetVal = IStream_Seek_Stub(
                            (IStream *) pParamStruct->This,
                            pParamStruct->dlibMove,
                            pParamStruct->dwOrigin,
                            pParamStruct->plibNewPosition);
    
}

/* [call_as] */ HRESULT STDMETHODCALLTYPE IStream_RemoteCopyTo_Proxy( 
    IStream * This,
    /* [unique][in] */ IStream *pstm,
    /* [in] */ ULARGE_INTEGER cb,
    /* [out] */ ULARGE_INTEGER *pcbRead,
    /* [out] */ ULARGE_INTEGER *pcbWritten)
{
CLIENT_CALL_RETURN _RetVal;

_RetVal = NdrClientCall2(
                  ( PMIDL_STUB_DESC  )&Object_StubDesc,
                  (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[2112],
                  ( unsigned char * )&This);
return ( HRESULT  )_RetVal.Simple;

}

void __RPC_API
IStream_RemoteCopyTo_Thunk(
    PMIDL_STUB_MESSAGE pStubMsg )
{
    #pragma pack(4)
    struct _PARAM_STRUCT
        {
        IStream *This;
        IStream *pstm;
        ULARGE_INTEGER cb;
        ULARGE_INTEGER *pcbRead;
        ULARGE_INTEGER *pcbWritten;
        HRESULT _RetVal;
        };
    #pragma pack()
    struct _PARAM_STRUCT * pParamStruct;

    pParamStruct = (struct _PARAM_STRUCT *) pStubMsg->StackTop;
    
    /* Call the server */
    
    pParamStruct->_RetVal = IStream_CopyTo_Stub(
                              (IStream *) pParamStruct->This,
                              pParamStruct->pstm,
                              pParamStruct->cb,
                              pParamStruct->pcbRead,
                              pParamStruct->pcbWritten);
    
}


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO IEnumSTATSTG_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO IEnumSTATSTG_ProxyInfo;

/* [call_as] */ HRESULT STDMETHODCALLTYPE IEnumSTATSTG_RemoteNext_Proxy( 
    IEnumSTATSTG * This,
    /* [in] */ ULONG celt,
    /* [length_is][size_is][out] */ STATSTG *rgelt,
    /* [out] */ ULONG *pceltFetched)
{
CLIENT_CALL_RETURN _RetVal;

_RetVal = NdrClientCall2(
                  ( PMIDL_STUB_DESC  )&Object_StubDesc,
                  (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[2406],
                  ( unsigned char * )&This);
return ( HRESULT  )_RetVal.Simple;

}

void __RPC_API
IEnumSTATSTG_RemoteNext_Thunk(
    PMIDL_STUB_MESSAGE pStubMsg )
{
    #pragma pack(4)
    struct _PARAM_STRUCT
        {
        IEnumSTATSTG *This;
        ULONG celt;
        STATSTG *rgelt;
        ULONG *pceltFetched;
        HRESULT _RetVal;
        };
    #pragma pack()
    struct _PARAM_STRUCT * pParamStruct;

    pParamStruct = (struct _PARAM_STRUCT *) pStubMsg->StackTop;
    
    /* Call the server */
    
    pParamStruct->_RetVal = IEnumSTATSTG_Next_Stub(
                                 (IEnumSTATSTG *) pParamStruct->This,
                                 pParamStruct->celt,
                                 pParamStruct->rgelt,
                                 pParamStruct->pceltFetched);
    
}


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO IStorage_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO IStorage_ProxyInfo;

/* [call_as] */ HRESULT STDMETHODCALLTYPE IStorage_RemoteOpenStream_Proxy( 
    IStorage * This,
    /* [string][in] */ const OLECHAR *pwcsName,
    /* [in] */ unsigned long cbReserved1,
    /* [size_is][unique][in] */ byte *reserved1,
    /* [in] */ DWORD grfMode,
    /* [in] */ DWORD reserved2,
    /* [out] */ IStream **ppstm)
{
CLIENT_CALL_RETURN _RetVal;

_RetVal = NdrClientCall2(
                  ( PMIDL_STUB_DESC  )&Object_StubDesc,
                  (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[2550],
                  ( unsigned char * )&This);
return ( HRESULT  )_RetVal.Simple;

}

void __RPC_API
IStorage_RemoteOpenStream_Thunk(
    PMIDL_STUB_MESSAGE pStubMsg )
{
    #pragma pack(4)
    struct _PARAM_STRUCT
        {
        IStorage *This;
        const OLECHAR *pwcsName;
        unsigned long cbReserved1;
        byte *reserved1;
        DWORD grfMode;
        DWORD reserved2;
        IStream **ppstm;
        HRESULT _RetVal;
        };
    #pragma pack()
    struct _PARAM_STRUCT * pParamStruct;

    pParamStruct = (struct _PARAM_STRUCT *) pStubMsg->StackTop;
    
    /* Call the server */
    
    pParamStruct->_RetVal = IStorage_OpenStream_Stub(
                                   (IStorage *) pParamStruct->This,
                                   pParamStruct->pwcsName,
                                   pParamStruct->cbReserved1,
                                   pParamStruct->reserved1,
                                   pParamStruct->grfMode,
                                   pParamStruct->reserved2,
                                   pParamStruct->ppstm);
    
}

/* [call_as] */ HRESULT STDMETHODCALLTYPE IStorage_RemoteEnumElements_Proxy( 
    IStorage * This,
    /* [in] */ DWORD reserved1,
    /* [in] */ unsigned long cbReserved2,
    /* [size_is][unique][in] */ byte *reserved2,
    /* [in] */ DWORD reserved3,
    /* [out] */ IEnumSTATSTG **ppenum)
{
CLIENT_CALL_RETURN _RetVal;

_RetVal = NdrClientCall2(
                  ( PMIDL_STUB_DESC  )&Object_StubDesc,
                  (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[2916],
                  ( unsigned char * )&This);
return ( HRESULT  )_RetVal.Simple;

}

void __RPC_API
IStorage_RemoteEnumElements_Thunk(
    PMIDL_STUB_MESSAGE pStubMsg )
{
    #pragma pack(4)
    struct _PARAM_STRUCT
        {
        IStorage *This;
        DWORD reserved1;
        unsigned long cbReserved2;
        byte *reserved2;
        DWORD reserved3;
        IEnumSTATSTG **ppenum;
        HRESULT _RetVal;
        };
    #pragma pack()
    struct _PARAM_STRUCT * pParamStruct;

    pParamStruct = (struct _PARAM_STRUCT *) pStubMsg->StackTop;
    
    /* Call the server */
    
    pParamStruct->_RetVal = IStorage_EnumElements_Stub(
                                     (IStorage *) pParamStruct->This,
                                     pParamStruct->reserved1,
                                     pParamStruct->cbReserved2,
                                     pParamStruct->reserved2,
                                     pParamStruct->reserved3,
                                     pParamStruct->ppenum);
    
}


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO IPersistFile_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO IPersistFile_ProxyInfo;


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO IPersistStorage_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO IPersistStorage_ProxyInfo;


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO ILockBytes_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO ILockBytes_ProxyInfo;

/* [call_as] */ HRESULT __stdcall ILockBytes_RemoteReadAt_Proxy( 
    ILockBytes * This,
    /* [in] */ ULARGE_INTEGER ulOffset,
    /* [length_is][size_is][out] */ byte *pv,
    /* [in] */ ULONG cb,
    /* [out] */ ULONG *pcbRead)
{
CLIENT_CALL_RETURN _RetVal;

_RetVal = NdrClientCall2(
                  ( PMIDL_STUB_DESC  )&Object_StubDesc,
                  (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[3498],
                  ( unsigned char * )&This);
return ( HRESULT  )_RetVal.Simple;

}

void __RPC_API
ILockBytes_RemoteReadAt_Thunk(
    PMIDL_STUB_MESSAGE pStubMsg )
{
    #pragma pack(4)
    struct _PARAM_STRUCT
        {
        ILockBytes *This;
        ULARGE_INTEGER ulOffset;
        byte *pv;
        ULONG cb;
        ULONG *pcbRead;
        HRESULT _RetVal;
        };
    #pragma pack()
    struct _PARAM_STRUCT * pParamStruct;

    pParamStruct = (struct _PARAM_STRUCT *) pStubMsg->StackTop;
    
    /* Call the server */
    
    pParamStruct->_RetVal = ILockBytes_ReadAt_Stub(
                                 (ILockBytes *) pParamStruct->This,
                                 pParamStruct->ulOffset,
                                 pParamStruct->pv,
                                 pParamStruct->cb,
                                 pParamStruct->pcbRead);
    
}

/* [call_as] */ HRESULT STDMETHODCALLTYPE ILockBytes_RemoteWriteAt_Proxy( 
    ILockBytes * This,
    /* [in] */ ULARGE_INTEGER ulOffset,
    /* [size_is][in] */ const byte *pv,
    /* [in] */ ULONG cb,
    /* [out] */ ULONG *pcbWritten)
{
CLIENT_CALL_RETURN _RetVal;

_RetVal = NdrClientCall2(
                  ( PMIDL_STUB_DESC  )&Object_StubDesc,
                  (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[3552],
                  ( unsigned char * )&This);
return ( HRESULT  )_RetVal.Simple;

}

void __RPC_API
ILockBytes_RemoteWriteAt_Thunk(
    PMIDL_STUB_MESSAGE pStubMsg )
{
    #pragma pack(4)
    struct _PARAM_STRUCT
        {
        ILockBytes *This;
        ULARGE_INTEGER ulOffset;
        const byte *pv;
        ULONG cb;
        ULONG *pcbWritten;
        HRESULT _RetVal;
        };
    #pragma pack()
    struct _PARAM_STRUCT * pParamStruct;

    pParamStruct = (struct _PARAM_STRUCT *) pStubMsg->StackTop;
    
    /* Call the server */
    
    pParamStruct->_RetVal = ILockBytes_WriteAt_Stub(
                                  (ILockBytes *) pParamStruct->This,
                                  pParamStruct->ulOffset,
                                  pParamStruct->pv,
                                  pParamStruct->cb,
                                  pParamStruct->pcbWritten);
    
}


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO IEnumFORMATETC_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO IEnumFORMATETC_ProxyInfo;

/* [call_as] */ HRESULT STDMETHODCALLTYPE IEnumFORMATETC_RemoteNext_Proxy( 
    IEnumFORMATETC * This,
    /* [in] */ ULONG celt,
    /* [length_is][size_is][out] */ FORMATETC *rgelt,
    /* [out] */ ULONG *pceltFetched)
{
CLIENT_CALL_RETURN _RetVal;

_RetVal = NdrClientCall2(
                  ( PMIDL_STUB_DESC  )&Object_StubDesc,
                  (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[3744],
                  ( unsigned char * )&This);
return ( HRESULT  )_RetVal.Simple;

}

void __RPC_API
IEnumFORMATETC_RemoteNext_Thunk(
    PMIDL_STUB_MESSAGE pStubMsg )
{
    #pragma pack(4)
    struct _PARAM_STRUCT
        {
        IEnumFORMATETC *This;
        ULONG celt;
        FORMATETC *rgelt;
        ULONG *pceltFetched;
        HRESULT _RetVal;
        };
    #pragma pack()
    struct _PARAM_STRUCT * pParamStruct;

    pParamStruct = (struct _PARAM_STRUCT *) pStubMsg->StackTop;
    
    /* Call the server */
    
    pParamStruct->_RetVal = IEnumFORMATETC_Next_Stub(
                                   (IEnumFORMATETC *) pParamStruct->This,
                                   pParamStruct->celt,
                                   pParamStruct->rgelt,
                                   pParamStruct->pceltFetched);
    
}


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO IEnumSTATDATA_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO IEnumSTATDATA_ProxyInfo;

/* [call_as] */ HRESULT STDMETHODCALLTYPE IEnumSTATDATA_RemoteNext_Proxy( 
    IEnumSTATDATA * This,
    /* [in] */ ULONG celt,
    /* [length_is][size_is][out] */ STATDATA *rgelt,
    /* [out] */ ULONG *pceltFetched)
{
CLIENT_CALL_RETURN _RetVal;

_RetVal = NdrClientCall2(
                  ( PMIDL_STUB_DESC  )&Object_StubDesc,
                  (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[3828],
                  ( unsigned char * )&This);
return ( HRESULT  )_RetVal.Simple;

}

void __RPC_API
IEnumSTATDATA_RemoteNext_Thunk(
    PMIDL_STUB_MESSAGE pStubMsg )
{
    #pragma pack(4)
    struct _PARAM_STRUCT
        {
        IEnumSTATDATA *This;
        ULONG celt;
        STATDATA *rgelt;
        ULONG *pceltFetched;
        HRESULT _RetVal;
        };
    #pragma pack()
    struct _PARAM_STRUCT * pParamStruct;

    pParamStruct = (struct _PARAM_STRUCT *) pStubMsg->StackTop;
    
    /* Call the server */
    
    pParamStruct->_RetVal = IEnumSTATDATA_Next_Stub(
                                  (IEnumSTATDATA *) pParamStruct->This,
                                  pParamStruct->celt,
                                  pParamStruct->rgelt,
                                  pParamStruct->pceltFetched);
    
}


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO IRootStorage_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO IRootStorage_ProxyInfo;


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO IAdviseSink_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO IAdviseSink_ProxyInfo;

/* [call_as] */ HRESULT STDMETHODCALLTYPE IAdviseSink_RemoteOnDataChange_Proxy( 
    IAdviseSink * This,
    /* [unique][in] */ FORMATETC *pFormatetc,
    /* [unique][in] */ ASYNC_STGMEDIUM *pStgmed)
{
CLIENT_CALL_RETURN _RetVal;

_RetVal = NdrClientCall2(
                  ( PMIDL_STUB_DESC  )&Object_StubDesc,
                  (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[3948],
                  ( unsigned char * )&This);
return ( HRESULT  )_RetVal.Simple;

}

void __RPC_API
IAdviseSink_RemoteOnDataChange_Thunk(
    PMIDL_STUB_MESSAGE pStubMsg )
{
    #pragma pack(4)
    struct _PARAM_STRUCT
        {
        IAdviseSink *This;
        FORMATETC *pFormatetc;
        ASYNC_STGMEDIUM *pStgmed;
        HRESULT _RetVal;
        };
    #pragma pack()
    struct _PARAM_STRUCT * pParamStruct;

    pParamStruct = (struct _PARAM_STRUCT *) pStubMsg->StackTop;
    
    /* Call the server */
    
    pParamStruct->_RetVal = IAdviseSink_OnDataChange_Stub(
                                        (IAdviseSink *) pParamStruct->This,
                                        pParamStruct->pFormatetc,
                                        pParamStruct->pStgmed);
    
}

/* [call_as] */ HRESULT STDMETHODCALLTYPE IAdviseSink_RemoteOnViewChange_Proxy( 
    IAdviseSink * This,
    /* [in] */ DWORD dwAspect,
    /* [in] */ LONG lindex)
{
CLIENT_CALL_RETURN _RetVal;

_RetVal = NdrClientCall2(
                  ( PMIDL_STUB_DESC  )&Object_StubDesc,
                  (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[3990],
                  ( unsigned char * )&This);
return ( HRESULT  )_RetVal.Simple;

}

void __RPC_API
IAdviseSink_RemoteOnViewChange_Thunk(
    PMIDL_STUB_MESSAGE pStubMsg )
{
    #pragma pack(4)
    struct _PARAM_STRUCT
        {
        IAdviseSink *This;
        DWORD dwAspect;
        LONG lindex;
        HRESULT _RetVal;
        };
    #pragma pack()
    struct _PARAM_STRUCT * pParamStruct;

    pParamStruct = (struct _PARAM_STRUCT *) pStubMsg->StackTop;
    
    /* Call the server */
    
    pParamStruct->_RetVal = IAdviseSink_OnViewChange_Stub(
                                        (IAdviseSink *) pParamStruct->This,
                                        pParamStruct->dwAspect,
                                        pParamStruct->lindex);
    
}

/* [call_as] */ HRESULT STDMETHODCALLTYPE IAdviseSink_RemoteOnRename_Proxy( 
    IAdviseSink * This,
    /* [in] */ IMoniker *pmk)
{
CLIENT_CALL_RETURN _RetVal;

_RetVal = NdrClientCall2(
                  ( PMIDL_STUB_DESC  )&Object_StubDesc,
                  (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[774],
                  ( unsigned char * )&This);
return ( HRESULT  )_RetVal.Simple;

}

void __RPC_API
IAdviseSink_RemoteOnRename_Thunk(
    PMIDL_STUB_MESSAGE pStubMsg )
{
    #pragma pack(4)
    struct _PARAM_STRUCT
        {
        IAdviseSink *This;
        IMoniker *pmk;
        HRESULT _RetVal;
        };
    #pragma pack()
    struct _PARAM_STRUCT * pParamStruct;

    pParamStruct = (struct _PARAM_STRUCT *) pStubMsg->StackTop;
    
    /* Call the server */
    
    pParamStruct->_RetVal = IAdviseSink_OnRename_Stub((IAdviseSink *) pParamStruct->This,pParamStruct->pmk);
    
}

/* [call_as] */ HRESULT STDMETHODCALLTYPE IAdviseSink_RemoteOnSave_Proxy( 
    IAdviseSink * This)
{
CLIENT_CALL_RETURN _RetVal;

_RetVal = NdrClientCall2(
                  ( PMIDL_STUB_DESC  )&Object_StubDesc,
                  (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[4032],
                  ( unsigned char * )&This);
return ( HRESULT  )_RetVal.Simple;

}

void __RPC_API
IAdviseSink_RemoteOnSave_Thunk(
    PMIDL_STUB_MESSAGE pStubMsg )
{
    #pragma pack(4)
    struct _PARAM_STRUCT
        {
        IAdviseSink *This;
        HRESULT _RetVal;
        };
    #pragma pack()
    struct _PARAM_STRUCT * pParamStruct;

    pParamStruct = (struct _PARAM_STRUCT *) pStubMsg->StackTop;
    
    /* Call the server */
    
    pParamStruct->_RetVal = IAdviseSink_OnSave_Stub((IAdviseSink *) pParamStruct->This);
    
}

/* [call_as] */ HRESULT STDMETHODCALLTYPE IAdviseSink_RemoteOnClose_Proxy( 
    IAdviseSink * This)
{
CLIENT_CALL_RETURN _RetVal;

_RetVal = NdrClientCall2(
                  ( PMIDL_STUB_DESC  )&Object_StubDesc,
                  (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[4062],
                  ( unsigned char * )&This);
return ( HRESULT  )_RetVal.Simple;

}

void __RPC_API
IAdviseSink_RemoteOnClose_Thunk(
    PMIDL_STUB_MESSAGE pStubMsg )
{
    #pragma pack(4)
    struct _PARAM_STRUCT
        {
        IAdviseSink *This;
        HRESULT _RetVal;
        };
    #pragma pack()
    struct _PARAM_STRUCT * pParamStruct;

    pParamStruct = (struct _PARAM_STRUCT *) pStubMsg->StackTop;
    
    /* Call the server */
    
    pParamStruct->_RetVal = IAdviseSink_OnClose_Stub((IAdviseSink *) pParamStruct->This);
    
}


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO AsyncIAdviseSink_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO AsyncIAdviseSink_ProxyInfo;

/* [call_as] */ HRESULT STDMETHODCALLTYPE AsyncIAdviseSink_Begin_RemoteOnDataChange_Proxy( 
    AsyncIAdviseSink * This,
    /* [unique][in] */ FORMATETC *pFormatetc,
    /* [unique][in] */ ASYNC_STGMEDIUM *pStgmed)
{
CLIENT_CALL_RETURN _RetVal;

_RetVal = NdrDcomAsyncClientCall(
                          ( PMIDL_STUB_DESC  )&Object_StubDesc,
                          (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[4092],
                          ( unsigned char * )&This);
return ( HRESULT  )_RetVal.Simple;

}

void __RPC_API
AsyncIAdviseSink_Begin_RemoteOnDataChange_Thunk(
    PMIDL_STUB_MESSAGE pStubMsg )
{
    #pragma pack(4)
    struct _PARAM_STRUCT
        {
        AsyncIAdviseSink *This;
        FORMATETC *pFormatetc;
        ASYNC_STGMEDIUM *pStgmed;
        HRESULT _RetVal;
        };
    #pragma pack()
    struct _PARAM_STRUCT * pParamStruct;

    pParamStruct = (struct _PARAM_STRUCT *) pStubMsg->StackTop;
    
    /* Call the server */
    
    pParamStruct->_RetVal = AsyncIAdviseSink_Begin_OnDataChange_Stub(
                                                   (AsyncIAdviseSink *) pParamStruct->This,
                                                   pParamStruct->pFormatetc,
                                                   pParamStruct->pStgmed);
    
}

/* [call_as] */ HRESULT STDMETHODCALLTYPE AsyncIAdviseSink_Finish_RemoteOnDataChange_Proxy( 
    AsyncIAdviseSink * This)
{
CLIENT_CALL_RETURN _RetVal;

_RetVal = NdrDcomAsyncClientCall(
                          ( PMIDL_STUB_DESC  )&Object_StubDesc,
                          (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[4134],
                          ( unsigned char * )&This);
return ( HRESULT  )_RetVal.Simple;

}

void __RPC_API
AsyncIAdviseSink_Finish_RemoteOnDataChange_Thunk(
    PMIDL_STUB_MESSAGE pStubMsg )
{
    #pragma pack(4)
    struct _PARAM_STRUCT
        {
        AsyncIAdviseSink *This;
        HRESULT _RetVal;
        };
    #pragma pack()
    struct _PARAM_STRUCT * pParamStruct;

    pParamStruct = (struct _PARAM_STRUCT *) pStubMsg->StackTop;
    
    /* Call the server */
    
    pParamStruct->_RetVal = AsyncIAdviseSink_Finish_OnDataChange_Stub((AsyncIAdviseSink *) pParamStruct->This);
    
}

/* [call_as] */ HRESULT STDMETHODCALLTYPE AsyncIAdviseSink_Begin_RemoteOnViewChange_Proxy( 
    AsyncIAdviseSink * This,
    /* [in] */ DWORD dwAspect,
    /* [in] */ LONG lindex)
{
CLIENT_CALL_RETURN _RetVal;

_RetVal = NdrDcomAsyncClientCall(
                          ( PMIDL_STUB_DESC  )&Object_StubDesc,
                          (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[4164],
                          ( unsigned char * )&This);
return ( HRESULT  )_RetVal.Simple;

}

void __RPC_API
AsyncIAdviseSink_Begin_RemoteOnViewChange_Thunk(
    PMIDL_STUB_MESSAGE pStubMsg )
{
    #pragma pack(4)
    struct _PARAM_STRUCT
        {
        AsyncIAdviseSink *This;
        DWORD dwAspect;
        LONG lindex;
        HRESULT _RetVal;
        };
    #pragma pack()
    struct _PARAM_STRUCT * pParamStruct;

    pParamStruct = (struct _PARAM_STRUCT *) pStubMsg->StackTop;
    
    /* Call the server */
    
    pParamStruct->_RetVal = AsyncIAdviseSink_Begin_OnViewChange_Stub(
                                                   (AsyncIAdviseSink *) pParamStruct->This,
                                                   pParamStruct->dwAspect,
                                                   pParamStruct->lindex);
    
}

/* [call_as] */ HRESULT STDMETHODCALLTYPE AsyncIAdviseSink_Finish_RemoteOnViewChange_Proxy( 
    AsyncIAdviseSink * This)
{
CLIENT_CALL_RETURN _RetVal;

_RetVal = NdrDcomAsyncClientCall(
                          ( PMIDL_STUB_DESC  )&Object_StubDesc,
                          (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[4206],
                          ( unsigned char * )&This);
return ( HRESULT  )_RetVal.Simple;

}

void __RPC_API
AsyncIAdviseSink_Finish_RemoteOnViewChange_Thunk(
    PMIDL_STUB_MESSAGE pStubMsg )
{
    #pragma pack(4)
    struct _PARAM_STRUCT
        {
        AsyncIAdviseSink *This;
        HRESULT _RetVal;
        };
    #pragma pack()
    struct _PARAM_STRUCT * pParamStruct;

    pParamStruct = (struct _PARAM_STRUCT *) pStubMsg->StackTop;
    
    /* Call the server */
    
    pParamStruct->_RetVal = AsyncIAdviseSink_Finish_OnViewChange_Stub((AsyncIAdviseSink *) pParamStruct->This);
    
}

/* [call_as] */ HRESULT STDMETHODCALLTYPE AsyncIAdviseSink_Begin_RemoteOnRename_Proxy( 
    AsyncIAdviseSink * This,
    /* [in] */ IMoniker *pmk)
{
CLIENT_CALL_RETURN _RetVal;

_RetVal = NdrDcomAsyncClientCall(
                          ( PMIDL_STUB_DESC  )&Object_StubDesc,
                          (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[4236],
                          ( unsigned char * )&This);
return ( HRESULT  )_RetVal.Simple;

}

void __RPC_API
AsyncIAdviseSink_Begin_RemoteOnRename_Thunk(
    PMIDL_STUB_MESSAGE pStubMsg )
{
    #pragma pack(4)
    struct _PARAM_STRUCT
        {
        AsyncIAdviseSink *This;
        IMoniker *pmk;
        HRESULT _RetVal;
        };
    #pragma pack()
    struct _PARAM_STRUCT * pParamStruct;

    pParamStruct = (struct _PARAM_STRUCT *) pStubMsg->StackTop;
    
    /* Call the server */
    
    pParamStruct->_RetVal = AsyncIAdviseSink_Begin_OnRename_Stub((AsyncIAdviseSink *) pParamStruct->This,pParamStruct->pmk);
    
}

/* [call_as] */ HRESULT STDMETHODCALLTYPE AsyncIAdviseSink_Finish_RemoteOnRename_Proxy( 
    AsyncIAdviseSink * This)
{
CLIENT_CALL_RETURN _RetVal;

_RetVal = NdrDcomAsyncClientCall(
                          ( PMIDL_STUB_DESC  )&Object_StubDesc,
                          (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[4272],
                          ( unsigned char * )&This);
return ( HRESULT  )_RetVal.Simple;

}

void __RPC_API
AsyncIAdviseSink_Finish_RemoteOnRename_Thunk(
    PMIDL_STUB_MESSAGE pStubMsg )
{
    #pragma pack(4)
    struct _PARAM_STRUCT
        {
        AsyncIAdviseSink *This;
        HRESULT _RetVal;
        };
    #pragma pack()
    struct _PARAM_STRUCT * pParamStruct;

    pParamStruct = (struct _PARAM_STRUCT *) pStubMsg->StackTop;
    
    /* Call the server */
    
    pParamStruct->_RetVal = AsyncIAdviseSink_Finish_OnRename_Stub((AsyncIAdviseSink *) pParamStruct->This);
    
}

/* [call_as] */ HRESULT STDMETHODCALLTYPE AsyncIAdviseSink_Begin_RemoteOnSave_Proxy( 
    AsyncIAdviseSink * This)
{
CLIENT_CALL_RETURN _RetVal;

_RetVal = NdrDcomAsyncClientCall(
                          ( PMIDL_STUB_DESC  )&Object_StubDesc,
                          (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[4302],
                          ( unsigned char * )&This);
return ( HRESULT  )_RetVal.Simple;

}

void __RPC_API
AsyncIAdviseSink_Begin_RemoteOnSave_Thunk(
    PMIDL_STUB_MESSAGE pStubMsg )
{
    #pragma pack(4)
    struct _PARAM_STRUCT
        {
        AsyncIAdviseSink *This;
        HRESULT _RetVal;
        };
    #pragma pack()
    struct _PARAM_STRUCT * pParamStruct;

    pParamStruct = (struct _PARAM_STRUCT *) pStubMsg->StackTop;
    
    /* Call the server */
    
    pParamStruct->_RetVal = AsyncIAdviseSink_Begin_OnSave_Stub((AsyncIAdviseSink *) pParamStruct->This);
    
}

/* [call_as] */ HRESULT STDMETHODCALLTYPE AsyncIAdviseSink_Finish_RemoteOnSave_Proxy( 
    AsyncIAdviseSink * This)
{
CLIENT_CALL_RETURN _RetVal;

_RetVal = NdrDcomAsyncClientCall(
                          ( PMIDL_STUB_DESC  )&Object_StubDesc,
                          (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[4332],
                          ( unsigned char * )&This);
return ( HRESULT  )_RetVal.Simple;

}

void __RPC_API
AsyncIAdviseSink_Finish_RemoteOnSave_Thunk(
    PMIDL_STUB_MESSAGE pStubMsg )
{
    #pragma pack(4)
    struct _PARAM_STRUCT
        {
        AsyncIAdviseSink *This;
        HRESULT _RetVal;
        };
    #pragma pack()
    struct _PARAM_STRUCT * pParamStruct;

    pParamStruct = (struct _PARAM_STRUCT *) pStubMsg->StackTop;
    
    /* Call the server */
    
    pParamStruct->_RetVal = AsyncIAdviseSink_Finish_OnSave_Stub((AsyncIAdviseSink *) pParamStruct->This);
    
}

/* [call_as] */ HRESULT STDMETHODCALLTYPE AsyncIAdviseSink_Begin_RemoteOnClose_Proxy( 
    AsyncIAdviseSink * This)
{
CLIENT_CALL_RETURN _RetVal;

_RetVal = NdrDcomAsyncClientCall(
                          ( PMIDL_STUB_DESC  )&Object_StubDesc,
                          (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[4362],
                          ( unsigned char * )&This);
return ( HRESULT  )_RetVal.Simple;

}

void __RPC_API
AsyncIAdviseSink_Begin_RemoteOnClose_Thunk(
    PMIDL_STUB_MESSAGE pStubMsg )
{
    #pragma pack(4)
    struct _PARAM_STRUCT
        {
        AsyncIAdviseSink *This;
        HRESULT _RetVal;
        };
    #pragma pack()
    struct _PARAM_STRUCT * pParamStruct;

    pParamStruct = (struct _PARAM_STRUCT *) pStubMsg->StackTop;
    
    /* Call the server */
    
    pParamStruct->_RetVal = AsyncIAdviseSink_Begin_OnClose_Stub((AsyncIAdviseSink *) pParamStruct->This);
    
}

/* [call_as] */ HRESULT STDMETHODCALLTYPE AsyncIAdviseSink_Finish_RemoteOnClose_Proxy( 
    AsyncIAdviseSink * This)
{
CLIENT_CALL_RETURN _RetVal;

_RetVal = NdrDcomAsyncClientCall(
                          ( PMIDL_STUB_DESC  )&Object_StubDesc,
                          (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[4392],
                          ( unsigned char * )&This);
return ( HRESULT  )_RetVal.Simple;

}

void __RPC_API
AsyncIAdviseSink_Finish_RemoteOnClose_Thunk(
    PMIDL_STUB_MESSAGE pStubMsg )
{
    #pragma pack(4)
    struct _PARAM_STRUCT
        {
        AsyncIAdviseSink *This;
        HRESULT _RetVal;
        };
    #pragma pack()
    struct _PARAM_STRUCT * pParamStruct;

    pParamStruct = (struct _PARAM_STRUCT *) pStubMsg->StackTop;
    
    /* Call the server */
    
    pParamStruct->_RetVal = AsyncIAdviseSink_Finish_OnClose_Stub((AsyncIAdviseSink *) pParamStruct->This);
    
}


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO IAdviseSink2_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO IAdviseSink2_ProxyInfo;

/* [call_as] */ HRESULT STDMETHODCALLTYPE IAdviseSink2_RemoteOnLinkSrcChange_Proxy( 
    IAdviseSink2 * This,
    /* [unique][in] */ IMoniker *pmk)
{
CLIENT_CALL_RETURN _RetVal;

_RetVal = NdrClientCall2(
                  ( PMIDL_STUB_DESC  )&Object_StubDesc,
                  (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[4422],
                  ( unsigned char * )&This);
return ( HRESULT  )_RetVal.Simple;

}

void __RPC_API
IAdviseSink2_RemoteOnLinkSrcChange_Thunk(
    PMIDL_STUB_MESSAGE pStubMsg )
{
    #pragma pack(4)
    struct _PARAM_STRUCT
        {
        IAdviseSink2 *This;
        IMoniker *pmk;
        HRESULT _RetVal;
        };
    #pragma pack()
    struct _PARAM_STRUCT * pParamStruct;

    pParamStruct = (struct _PARAM_STRUCT *) pStubMsg->StackTop;
    
    /* Call the server */
    
    pParamStruct->_RetVal = IAdviseSink2_OnLinkSrcChange_Stub((IAdviseSink2 *) pParamStruct->This,pParamStruct->pmk);
    
}


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO AsyncIAdviseSink2_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO AsyncIAdviseSink2_ProxyInfo;

/* [call_as] */ HRESULT STDMETHODCALLTYPE AsyncIAdviseSink2_Begin_RemoteOnLinkSrcChange_Proxy( 
    AsyncIAdviseSink2 * This,
    /* [unique][in] */ IMoniker *pmk)
{
CLIENT_CALL_RETURN _RetVal;

_RetVal = NdrDcomAsyncClientCall(
                          ( PMIDL_STUB_DESC  )&Object_StubDesc,
                          (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[4458],
                          ( unsigned char * )&This);
return ( HRESULT  )_RetVal.Simple;

}

void __RPC_API
AsyncIAdviseSink2_Begin_RemoteOnLinkSrcChange_Thunk(
    PMIDL_STUB_MESSAGE pStubMsg )
{
    #pragma pack(4)
    struct _PARAM_STRUCT
        {
        AsyncIAdviseSink2 *This;
        IMoniker *pmk;
        HRESULT _RetVal;
        };
    #pragma pack()
    struct _PARAM_STRUCT * pParamStruct;

    pParamStruct = (struct _PARAM_STRUCT *) pStubMsg->StackTop;
    
    /* Call the server */
    
    pParamStruct->_RetVal = AsyncIAdviseSink2_Begin_OnLinkSrcChange_Stub((AsyncIAdviseSink2 *) pParamStruct->This,pParamStruct->pmk);
    
}

/* [call_as] */ HRESULT STDMETHODCALLTYPE AsyncIAdviseSink2_Finish_RemoteOnLinkSrcChange_Proxy( 
    AsyncIAdviseSink2 * This)
{
CLIENT_CALL_RETURN _RetVal;

_RetVal = NdrDcomAsyncClientCall(
                          ( PMIDL_STUB_DESC  )&Object_StubDesc,
                          (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[4494],
                          ( unsigned char * )&This);
return ( HRESULT  )_RetVal.Simple;

}

void __RPC_API
AsyncIAdviseSink2_Finish_RemoteOnLinkSrcChange_Thunk(
    PMIDL_STUB_MESSAGE pStubMsg )
{
    #pragma pack(4)
    struct _PARAM_STRUCT
        {
        AsyncIAdviseSink2 *This;
        HRESULT _RetVal;
        };
    #pragma pack()
    struct _PARAM_STRUCT * pParamStruct;

    pParamStruct = (struct _PARAM_STRUCT *) pStubMsg->StackTop;
    
    /* Call the server */
    
    pParamStruct->_RetVal = AsyncIAdviseSink2_Finish_OnLinkSrcChange_Stub((AsyncIAdviseSink2 *) pParamStruct->This);
    
}


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO IDataObject_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO IDataObject_ProxyInfo;

/* [call_as] */ HRESULT STDMETHODCALLTYPE IDataObject_RemoteGetData_Proxy( 
    IDataObject * This,
    /* [unique][in] */ FORMATETC *pformatetcIn,
    /* [out] */ STGMEDIUM *pRemoteMedium)
{
CLIENT_CALL_RETURN _RetVal;

_RetVal = NdrClientCall2(
                  ( PMIDL_STUB_DESC  )&Object_StubDesc,
                  (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[4524],
                  ( unsigned char * )&This);
return ( HRESULT  )_RetVal.Simple;

}

void __RPC_API
IDataObject_RemoteGetData_Thunk(
    PMIDL_STUB_MESSAGE pStubMsg )
{
    #pragma pack(4)
    struct _PARAM_STRUCT
        {
        IDataObject *This;
        FORMATETC *pformatetcIn;
        STGMEDIUM *pRemoteMedium;
        HRESULT _RetVal;
        };
    #pragma pack()
    struct _PARAM_STRUCT * pParamStruct;

    pParamStruct = (struct _PARAM_STRUCT *) pStubMsg->StackTop;
    
    /* Call the server */
    
    pParamStruct->_RetVal = IDataObject_GetData_Stub(
                                   (IDataObject *) pParamStruct->This,
                                   pParamStruct->pformatetcIn,
                                   pParamStruct->pRemoteMedium);
    
}

/* [call_as] */ HRESULT STDMETHODCALLTYPE IDataObject_RemoteGetDataHere_Proxy( 
    IDataObject * This,
    /* [unique][in] */ FORMATETC *pformatetc,
    /* [out][in] */ STGMEDIUM *pRemoteMedium)
{
CLIENT_CALL_RETURN _RetVal;

_RetVal = NdrClientCall2(
                  ( PMIDL_STUB_DESC  )&Object_StubDesc,
                  (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[4566],
                  ( unsigned char * )&This);
return ( HRESULT  )_RetVal.Simple;

}

void __RPC_API
IDataObject_RemoteGetDataHere_Thunk(
    PMIDL_STUB_MESSAGE pStubMsg )
{
    #pragma pack(4)
    struct _PARAM_STRUCT
        {
        IDataObject *This;
        FORMATETC *pformatetc;
        STGMEDIUM *pRemoteMedium;
        HRESULT _RetVal;
        };
    #pragma pack()
    struct _PARAM_STRUCT * pParamStruct;

    pParamStruct = (struct _PARAM_STRUCT *) pStubMsg->StackTop;
    
    /* Call the server */
    
    pParamStruct->_RetVal = IDataObject_GetDataHere_Stub(
                                       (IDataObject *) pParamStruct->This,
                                       pParamStruct->pformatetc,
                                       pParamStruct->pRemoteMedium);
    
}

/* [call_as] */ HRESULT STDMETHODCALLTYPE IDataObject_RemoteSetData_Proxy( 
    IDataObject * This,
    /* [unique][in] */ FORMATETC *pformatetc,
    /* [unique][in] */ FLAG_STGMEDIUM *pmedium,
    /* [in] */ BOOL fRelease)
{
CLIENT_CALL_RETURN _RetVal;

_RetVal = NdrClientCall2(
                  ( PMIDL_STUB_DESC  )&Object_StubDesc,
                  (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[4686],
                  ( unsigned char * )&This);
return ( HRESULT  )_RetVal.Simple;

}

void __RPC_API
IDataObject_RemoteSetData_Thunk(
    PMIDL_STUB_MESSAGE pStubMsg )
{
    #pragma pack(4)
    struct _PARAM_STRUCT
        {
        IDataObject *This;
        FORMATETC *pformatetc;
        FLAG_STGMEDIUM *pmedium;
        BOOL fRelease;
        HRESULT _RetVal;
        };
    #pragma pack()
    struct _PARAM_STRUCT * pParamStruct;

    pParamStruct = (struct _PARAM_STRUCT *) pStubMsg->StackTop;
    
    /* Call the server */
    
    pParamStruct->_RetVal = IDataObject_SetData_Stub(
                                   (IDataObject *) pParamStruct->This,
                                   pParamStruct->pformatetc,
                                   pParamStruct->pmedium,
                                   pParamStruct->fRelease);
    
}


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO IClassActivator_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO IClassActivator_ProxyInfo;


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO IFillLockBytes_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO IFillLockBytes_ProxyInfo;

/* [call_as] */ HRESULT __stdcall IFillLockBytes_RemoteFillAppend_Proxy( 
    IFillLockBytes * This,
    /* [size_is][in] */ const byte *pv,
    /* [in] */ ULONG cb,
    /* [out] */ ULONG *pcbWritten)
{
CLIENT_CALL_RETURN _RetVal;

_RetVal = NdrClientCall2(
                  ( PMIDL_STUB_DESC  )&Object_StubDesc,
                  (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[4962],
                  ( unsigned char * )&This);
return ( HRESULT  )_RetVal.Simple;

}

void __RPC_API
IFillLockBytes_RemoteFillAppend_Thunk(
    PMIDL_STUB_MESSAGE pStubMsg )
{
    #pragma pack(4)
    struct _PARAM_STRUCT
        {
        IFillLockBytes *This;
        const byte *pv;
        ULONG cb;
        ULONG *pcbWritten;
        HRESULT _RetVal;
        };
    #pragma pack()
    struct _PARAM_STRUCT * pParamStruct;

    pParamStruct = (struct _PARAM_STRUCT *) pStubMsg->StackTop;
    
    /* Call the server */
    
    pParamStruct->_RetVal = IFillLockBytes_FillAppend_Stub(
                                         (IFillLockBytes *) pParamStruct->This,
                                         pParamStruct->pv,
                                         pParamStruct->cb,
                                         pParamStruct->pcbWritten);
    
}

/* [call_as] */ HRESULT __stdcall IFillLockBytes_RemoteFillAt_Proxy( 
    IFillLockBytes * This,
    /* [in] */ ULARGE_INTEGER ulOffset,
    /* [size_is][in] */ const byte *pv,
    /* [in] */ ULONG cb,
    /* [out] */ ULONG *pcbWritten)
{
CLIENT_CALL_RETURN _RetVal;

_RetVal = NdrClientCall2(
                  ( PMIDL_STUB_DESC  )&Object_StubDesc,
                  (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[3552],
                  ( unsigned char * )&This);
return ( HRESULT  )_RetVal.Simple;

}

void __RPC_API
IFillLockBytes_RemoteFillAt_Thunk(
    PMIDL_STUB_MESSAGE pStubMsg )
{
    #pragma pack(4)
    struct _PARAM_STRUCT
        {
        IFillLockBytes *This;
        ULARGE_INTEGER ulOffset;
        const byte *pv;
        ULONG cb;
        ULONG *pcbWritten;
        HRESULT _RetVal;
        };
    #pragma pack()
    struct _PARAM_STRUCT * pParamStruct;

    pParamStruct = (struct _PARAM_STRUCT *) pStubMsg->StackTop;
    
    /* Call the server */
    
    pParamStruct->_RetVal = IFillLockBytes_FillAt_Stub(
                                     (IFillLockBytes *) pParamStruct->This,
                                     pParamStruct->ulOffset,
                                     pParamStruct->pv,
                                     pParamStruct->cb,
                                     pParamStruct->pcbWritten);
    
}


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO IProgressNotify_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO IProgressNotify_ProxyInfo;


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO IBlockingLock_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO IBlockingLock_ProxyInfo;


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO ITimeAndNoticeControl_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO ITimeAndNoticeControl_ProxyInfo;


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO IOplockStorage_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO IOplockStorage_ProxyInfo;


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO ISurrogate_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO ISurrogate_ProxyInfo;


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO IDirectWriterLock_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO IDirectWriterLock_ProxyInfo;


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO ISynchronize_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO ISynchronize_ProxyInfo;


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO IUrlMon_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO IUrlMon_ProxyInfo;


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO IPipeByte_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO IPipeByte_ProxyInfo;


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO AsyncIPipeByte_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO AsyncIPipeByte_ProxyInfo;


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO IPipeLong_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO IPipeLong_ProxyInfo;


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO AsyncIPipeLong_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO AsyncIPipeLong_ProxyInfo;


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO IPipeDouble_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO IPipeDouble_ProxyInfo;


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO AsyncIPipeDouble_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO AsyncIPipeDouble_ProxyInfo;


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO IThumbnailExtractor_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO IThumbnailExtractor_ProxyInfo;


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO IDummyHICONIncluder_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO IDummyHICONIncluder_ProxyInfo;


extern const EXPR_EVAL ExprEvalRoutines[];
extern const USER_MARSHAL_ROUTINE_QUADRUPLE UserMarshalRoutines[ WIRE_MARSHAL_TABLE_SIZE ];

#if !defined(__RPC_WIN32__)
#error  Invalid build platform for this stub.
#endif

#if !(TARGET_IS_NT50_OR_LATER)
#error You need a Windows 2000 or later to run this stub because it uses these features:
#error   /robust command line switch, [async_uuid] attribute.
#error However, your C/C++ compilation flags indicate you intend to run this app on earlier systems.
#error This app will die there with the RPC_X_WRONG_STUB_VERSION error.
#endif


static const MIDL_PROC_FORMAT_STRING __MIDL_ProcFormatString =
    {
        0,
        {

	/* Procedure RemoteNext */

			0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/*  2 */	NdrFcLong( 0x0 ),	/* 0 */
/*  6 */	NdrFcShort( 0x3 ),	/* 3 */
/*  8 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 10 */	NdrFcShort( 0x8 ),	/* 8 */
/* 12 */	NdrFcShort( 0x24 ),	/* 36 */
/* 14 */	0x45,		/* Oi2 Flags:  srv must size, has return, has ext, */
			0x4,		/* 4 */
/* 16 */	0x8,		/* 8 */
			0x3,		/* Ext Flags:  new corr desc, clt corr check, */
/* 18 */	NdrFcShort( 0x1 ),	/* 1 */
/* 20 */	NdrFcShort( 0x0 ),	/* 0 */
/* 22 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter celt */

/* 24 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 26 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 28 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter rgelt */

/* 30 */	NdrFcShort( 0x113 ),	/* Flags:  must size, must free, out, simple ref, */
/* 32 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 34 */	NdrFcShort( 0x18 ),	/* Type Offset=24 */

	/* Parameter pceltFetched */

/* 36 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 38 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 40 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 42 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 44 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 46 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure Skip */


	/* Procedure Skip */


	/* Procedure Skip */


	/* Procedure Skip */


	/* Procedure Revoke */


	/* Procedure Skip */


	/* Procedure Skip */

/* 48 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 50 */	NdrFcLong( 0x0 ),	/* 0 */
/* 54 */	NdrFcShort( 0x4 ),	/* 4 */
/* 56 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 58 */	NdrFcShort( 0x8 ),	/* 8 */
/* 60 */	NdrFcShort( 0x8 ),	/* 8 */
/* 62 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x2,		/* 2 */
/* 64 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 66 */	NdrFcShort( 0x0 ),	/* 0 */
/* 68 */	NdrFcShort( 0x0 ),	/* 0 */
/* 70 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter celt */


	/* Parameter celt */


	/* Parameter celt */


	/* Parameter celt */


	/* Parameter dwRegister */


	/* Parameter celt */


	/* Parameter celt */

/* 72 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 74 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 76 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */


	/* Return value */


	/* Return value */


	/* Return value */


	/* Return value */


	/* Return value */


	/* Return value */

/* 78 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 80 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 82 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure Reset */


	/* Procedure HaveWriteAccess */


	/* Procedure Reset */


	/* Procedure Reset */


	/* Procedure Flush */


	/* Procedure Reset */


	/* Procedure Reset */


	/* Procedure RemoteIsRunning */


	/* Procedure Reset */


	/* Procedure ReleaseBoundObjects */


	/* Procedure Reset */

/* 84 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 86 */	NdrFcLong( 0x0 ),	/* 0 */
/* 90 */	NdrFcShort( 0x5 ),	/* 5 */
/* 92 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 94 */	NdrFcShort( 0x0 ),	/* 0 */
/* 96 */	NdrFcShort( 0x8 ),	/* 8 */
/* 98 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x1,		/* 1 */
/* 100 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 102 */	NdrFcShort( 0x0 ),	/* 0 */
/* 104 */	NdrFcShort( 0x0 ),	/* 0 */
/* 106 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Return value */


	/* Return value */


	/* Return value */


	/* Return value */


	/* Return value */


	/* Return value */


	/* Return value */


	/* Return value */


	/* Return value */


	/* Return value */


	/* Return value */

/* 108 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 110 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 112 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure Clone */

/* 114 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 116 */	NdrFcLong( 0x0 ),	/* 0 */
/* 120 */	NdrFcShort( 0x6 ),	/* 6 */
/* 122 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 124 */	NdrFcShort( 0x0 ),	/* 0 */
/* 126 */	NdrFcShort( 0x8 ),	/* 8 */
/* 128 */	0x45,		/* Oi2 Flags:  srv must size, has return, has ext, */
			0x2,		/* 2 */
/* 130 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 132 */	NdrFcShort( 0x0 ),	/* 0 */
/* 134 */	NdrFcShort( 0x0 ),	/* 0 */
/* 136 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter ppenum */

/* 138 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
/* 140 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 142 */	NdrFcShort( 0x32 ),	/* Type Offset=50 */

	/* Return value */

/* 144 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 146 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 148 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure RegisterObjectBound */

/* 150 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 152 */	NdrFcLong( 0x0 ),	/* 0 */
/* 156 */	NdrFcShort( 0x3 ),	/* 3 */
/* 158 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 160 */	NdrFcShort( 0x0 ),	/* 0 */
/* 162 */	NdrFcShort( 0x8 ),	/* 8 */
/* 164 */	0x46,		/* Oi2 Flags:  clt must size, has return, has ext, */
			0x2,		/* 2 */
/* 166 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 168 */	NdrFcShort( 0x0 ),	/* 0 */
/* 170 */	NdrFcShort( 0x0 ),	/* 0 */
/* 172 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter punk */

/* 174 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 176 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 178 */	NdrFcShort( 0x6 ),	/* Type Offset=6 */

	/* Return value */

/* 180 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 182 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 184 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure RevokeObjectBound */

/* 186 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 188 */	NdrFcLong( 0x0 ),	/* 0 */
/* 192 */	NdrFcShort( 0x4 ),	/* 4 */
/* 194 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 196 */	NdrFcShort( 0x0 ),	/* 0 */
/* 198 */	NdrFcShort( 0x8 ),	/* 8 */
/* 200 */	0x46,		/* Oi2 Flags:  clt must size, has return, has ext, */
			0x2,		/* 2 */
/* 202 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 204 */	NdrFcShort( 0x0 ),	/* 0 */
/* 206 */	NdrFcShort( 0x0 ),	/* 0 */
/* 208 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter punk */

/* 210 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 212 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 214 */	NdrFcShort( 0x6 ),	/* Type Offset=6 */

	/* Return value */

/* 216 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 218 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 220 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure RemoteSetBindOptions */

/* 222 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 224 */	NdrFcLong( 0x0 ),	/* 0 */
/* 228 */	NdrFcShort( 0x6 ),	/* 6 */
/* 230 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 232 */	NdrFcShort( 0x0 ),	/* 0 */
/* 234 */	NdrFcShort( 0x8 ),	/* 8 */
/* 236 */	0x46,		/* Oi2 Flags:  clt must size, has return, has ext, */
			0x2,		/* 2 */
/* 238 */	0x8,		/* 8 */
			0x5,		/* Ext Flags:  new corr desc, srv corr check, */
/* 240 */	NdrFcShort( 0x0 ),	/* 0 */
/* 242 */	NdrFcShort( 0x3 ),	/* 3 */
/* 244 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pbindopts */

/* 246 */	NdrFcShort( 0x10b ),	/* Flags:  must size, must free, in, simple ref, */
/* 248 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 250 */	NdrFcShort( 0xe2 ),	/* Type Offset=226 */

	/* Return value */

/* 252 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 254 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 256 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure RemoteGetBindOptions */

/* 258 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 260 */	NdrFcLong( 0x0 ),	/* 0 */
/* 264 */	NdrFcShort( 0x7 ),	/* 7 */
/* 266 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 268 */	NdrFcShort( 0x0 ),	/* 0 */
/* 270 */	NdrFcShort( 0x8 ),	/* 8 */
/* 272 */	0x47,		/* Oi2 Flags:  srv must size, clt must size, has return, has ext, */
			0x2,		/* 2 */
/* 274 */	0x8,		/* 8 */
			0x7,		/* Ext Flags:  new corr desc, clt corr check, srv corr check, */
/* 276 */	NdrFcShort( 0x3 ),	/* 3 */
/* 278 */	NdrFcShort( 0x3 ),	/* 3 */
/* 280 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pbindopts */

/* 282 */	NdrFcShort( 0x11b ),	/* Flags:  must size, must free, in, out, simple ref, */
/* 284 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 286 */	NdrFcShort( 0xe2 ),	/* Type Offset=226 */

	/* Return value */

/* 288 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 290 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 292 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetRunningObjectTable */

/* 294 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 296 */	NdrFcLong( 0x0 ),	/* 0 */
/* 300 */	NdrFcShort( 0x8 ),	/* 8 */
/* 302 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 304 */	NdrFcShort( 0x0 ),	/* 0 */
/* 306 */	NdrFcShort( 0x8 ),	/* 8 */
/* 308 */	0x45,		/* Oi2 Flags:  srv must size, has return, has ext, */
			0x2,		/* 2 */
/* 310 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 312 */	NdrFcShort( 0x0 ),	/* 0 */
/* 314 */	NdrFcShort( 0x0 ),	/* 0 */
/* 316 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pprot */

/* 318 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
/* 320 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 322 */	NdrFcShort( 0xfc ),	/* Type Offset=252 */

	/* Return value */

/* 324 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 326 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 328 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure RegisterObjectParam */

/* 330 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 332 */	NdrFcLong( 0x0 ),	/* 0 */
/* 336 */	NdrFcShort( 0x9 ),	/* 9 */
/* 338 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 340 */	NdrFcShort( 0x0 ),	/* 0 */
/* 342 */	NdrFcShort( 0x8 ),	/* 8 */
/* 344 */	0x46,		/* Oi2 Flags:  clt must size, has return, has ext, */
			0x3,		/* 3 */
/* 346 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 348 */	NdrFcShort( 0x0 ),	/* 0 */
/* 350 */	NdrFcShort( 0x0 ),	/* 0 */
/* 352 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pszKey */

/* 354 */	NdrFcShort( 0x10b ),	/* Flags:  must size, must free, in, simple ref, */
/* 356 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 358 */	NdrFcShort( 0x114 ),	/* Type Offset=276 */

	/* Parameter punk */

/* 360 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 362 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 364 */	NdrFcShort( 0x6 ),	/* Type Offset=6 */

	/* Return value */

/* 366 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 368 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 370 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetObjectParam */

/* 372 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 374 */	NdrFcLong( 0x0 ),	/* 0 */
/* 378 */	NdrFcShort( 0xa ),	/* 10 */
/* 380 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 382 */	NdrFcShort( 0x0 ),	/* 0 */
/* 384 */	NdrFcShort( 0x8 ),	/* 8 */
/* 386 */	0x47,		/* Oi2 Flags:  srv must size, clt must size, has return, has ext, */
			0x3,		/* 3 */
/* 388 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 390 */	NdrFcShort( 0x0 ),	/* 0 */
/* 392 */	NdrFcShort( 0x0 ),	/* 0 */
/* 394 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pszKey */

/* 396 */	NdrFcShort( 0x10b ),	/* Flags:  must size, must free, in, simple ref, */
/* 398 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 400 */	NdrFcShort( 0x114 ),	/* Type Offset=276 */

	/* Parameter ppunk */

/* 402 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
/* 404 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 406 */	NdrFcShort( 0x116 ),	/* Type Offset=278 */

	/* Return value */

/* 408 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 410 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 412 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure EnumObjectParam */

/* 414 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 416 */	NdrFcLong( 0x0 ),	/* 0 */
/* 420 */	NdrFcShort( 0xb ),	/* 11 */
/* 422 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 424 */	NdrFcShort( 0x0 ),	/* 0 */
/* 426 */	NdrFcShort( 0x8 ),	/* 8 */
/* 428 */	0x45,		/* Oi2 Flags:  srv must size, has return, has ext, */
			0x2,		/* 2 */
/* 430 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 432 */	NdrFcShort( 0x0 ),	/* 0 */
/* 434 */	NdrFcShort( 0x0 ),	/* 0 */
/* 436 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter ppenum */

/* 438 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
/* 440 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 442 */	NdrFcShort( 0x11a ),	/* Type Offset=282 */

	/* Return value */

/* 444 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 446 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 448 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure DestroyElement */


	/* Procedure RevokeObjectParam */

/* 450 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 452 */	NdrFcLong( 0x0 ),	/* 0 */
/* 456 */	NdrFcShort( 0xc ),	/* 12 */
/* 458 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 460 */	NdrFcShort( 0x0 ),	/* 0 */
/* 462 */	NdrFcShort( 0x8 ),	/* 8 */
/* 464 */	0x46,		/* Oi2 Flags:  clt must size, has return, has ext, */
			0x2,		/* 2 */
/* 466 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 468 */	NdrFcShort( 0x0 ),	/* 0 */
/* 470 */	NdrFcShort( 0x0 ),	/* 0 */
/* 472 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pwcsName */


	/* Parameter pszKey */

/* 474 */	NdrFcShort( 0x10b ),	/* Flags:  must size, must free, in, simple ref, */
/* 476 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 478 */	NdrFcShort( 0x114 ),	/* Type Offset=276 */

	/* Return value */


	/* Return value */

/* 480 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 482 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 484 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure RemoteNext */

/* 486 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 488 */	NdrFcLong( 0x0 ),	/* 0 */
/* 492 */	NdrFcShort( 0x3 ),	/* 3 */
/* 494 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 496 */	NdrFcShort( 0x8 ),	/* 8 */
/* 498 */	NdrFcShort( 0x24 ),	/* 36 */
/* 500 */	0x45,		/* Oi2 Flags:  srv must size, has return, has ext, */
			0x4,		/* 4 */
/* 502 */	0x8,		/* 8 */
			0x3,		/* Ext Flags:  new corr desc, clt corr check, */
/* 504 */	NdrFcShort( 0x1 ),	/* 1 */
/* 506 */	NdrFcShort( 0x0 ),	/* 0 */
/* 508 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter celt */

/* 510 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 512 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 514 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter rgelt */

/* 516 */	NdrFcShort( 0x113 ),	/* Flags:  must size, must free, out, simple ref, */
/* 518 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 520 */	NdrFcShort( 0x146 ),	/* Type Offset=326 */

	/* Parameter pceltFetched */

/* 522 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 524 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 526 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 528 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 530 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 532 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure Clone */

/* 534 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 536 */	NdrFcLong( 0x0 ),	/* 0 */
/* 540 */	NdrFcShort( 0x6 ),	/* 6 */
/* 542 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 544 */	NdrFcShort( 0x0 ),	/* 0 */
/* 546 */	NdrFcShort( 0x8 ),	/* 8 */
/* 548 */	0x45,		/* Oi2 Flags:  srv must size, has return, has ext, */
			0x2,		/* 2 */
/* 550 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 552 */	NdrFcShort( 0x0 ),	/* 0 */
/* 554 */	NdrFcShort( 0x0 ),	/* 0 */
/* 556 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter ppenum */

/* 558 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
/* 560 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 562 */	NdrFcShort( 0x15c ),	/* Type Offset=348 */

	/* Return value */

/* 564 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 566 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 568 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetClassID */


	/* Procedure GetRunningClass */

/* 570 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 572 */	NdrFcLong( 0x0 ),	/* 0 */
/* 576 */	NdrFcShort( 0x3 ),	/* 3 */
/* 578 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 580 */	NdrFcShort( 0x0 ),	/* 0 */
/* 582 */	NdrFcShort( 0x4c ),	/* 76 */
/* 584 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x2,		/* 2 */
/* 586 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 588 */	NdrFcShort( 0x0 ),	/* 0 */
/* 590 */	NdrFcShort( 0x0 ),	/* 0 */
/* 592 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pClassID */


	/* Parameter lpClsid */

/* 594 */	NdrFcShort( 0x4112 ),	/* Flags:  must free, out, simple ref, srv alloc size=16 */
/* 596 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 598 */	NdrFcShort( 0x17c ),	/* Type Offset=380 */

	/* Return value */


	/* Return value */

/* 600 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 602 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 604 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure Run */

/* 606 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 608 */	NdrFcLong( 0x0 ),	/* 0 */
/* 612 */	NdrFcShort( 0x4 ),	/* 4 */
/* 614 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 616 */	NdrFcShort( 0x0 ),	/* 0 */
/* 618 */	NdrFcShort( 0x8 ),	/* 8 */
/* 620 */	0x46,		/* Oi2 Flags:  clt must size, has return, has ext, */
			0x2,		/* 2 */
/* 622 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 624 */	NdrFcShort( 0x0 ),	/* 0 */
/* 626 */	NdrFcShort( 0x0 ),	/* 0 */
/* 628 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pbc */

/* 630 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 632 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 634 */	NdrFcShort( 0x188 ),	/* Type Offset=392 */

	/* Return value */

/* 636 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 638 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 640 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure LockRunning */

/* 642 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 644 */	NdrFcLong( 0x0 ),	/* 0 */
/* 648 */	NdrFcShort( 0x6 ),	/* 6 */
/* 650 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 652 */	NdrFcShort( 0x10 ),	/* 16 */
/* 654 */	NdrFcShort( 0x8 ),	/* 8 */
/* 656 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x3,		/* 3 */
/* 658 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 660 */	NdrFcShort( 0x0 ),	/* 0 */
/* 662 */	NdrFcShort( 0x0 ),	/* 0 */
/* 664 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter fLock */

/* 666 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 668 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 670 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter fLastUnlockCloses */

/* 672 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 674 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 676 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 678 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 680 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 682 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure SetContainedObject */

/* 684 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 686 */	NdrFcLong( 0x0 ),	/* 0 */
/* 690 */	NdrFcShort( 0x7 ),	/* 7 */
/* 692 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 694 */	NdrFcShort( 0x8 ),	/* 8 */
/* 696 */	NdrFcShort( 0x8 ),	/* 8 */
/* 698 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x2,		/* 2 */
/* 700 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 702 */	NdrFcShort( 0x0 ),	/* 0 */
/* 704 */	NdrFcShort( 0x0 ),	/* 0 */
/* 706 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter fContained */

/* 708 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 710 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 712 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 714 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 716 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 718 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure Register */

/* 720 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 722 */	NdrFcLong( 0x0 ),	/* 0 */
/* 726 */	NdrFcShort( 0x3 ),	/* 3 */
/* 728 */	NdrFcShort( 0x18 ),	/* x86 Stack size/offset = 24 */
/* 730 */	NdrFcShort( 0x8 ),	/* 8 */
/* 732 */	NdrFcShort( 0x24 ),	/* 36 */
/* 734 */	0x46,		/* Oi2 Flags:  clt must size, has return, has ext, */
			0x5,		/* 5 */
/* 736 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 738 */	NdrFcShort( 0x0 ),	/* 0 */
/* 740 */	NdrFcShort( 0x0 ),	/* 0 */
/* 742 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter grfFlags */

/* 744 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 746 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 748 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter punkObject */

/* 750 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 752 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 754 */	NdrFcShort( 0x6 ),	/* Type Offset=6 */

	/* Parameter pmkObjectName */

/* 756 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 758 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 760 */	NdrFcShort( 0x134 ),	/* Type Offset=308 */

	/* Parameter pdwRegister */

/* 762 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 764 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 766 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 768 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 770 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 772 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure RemoteOnRename */


	/* Procedure IsRunning */

/* 774 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 776 */	NdrFcLong( 0x0 ),	/* 0 */
/* 780 */	NdrFcShort( 0x5 ),	/* 5 */
/* 782 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 784 */	NdrFcShort( 0x0 ),	/* 0 */
/* 786 */	NdrFcShort( 0x8 ),	/* 8 */
/* 788 */	0x46,		/* Oi2 Flags:  clt must size, has return, has ext, */
			0x2,		/* 2 */
/* 790 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 792 */	NdrFcShort( 0x0 ),	/* 0 */
/* 794 */	NdrFcShort( 0x0 ),	/* 0 */
/* 796 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pmk */


	/* Parameter pmkObjectName */

/* 798 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 800 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 802 */	NdrFcShort( 0x134 ),	/* Type Offset=308 */

	/* Return value */


	/* Return value */

/* 804 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 806 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 808 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetObject */

/* 810 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 812 */	NdrFcLong( 0x0 ),	/* 0 */
/* 816 */	NdrFcShort( 0x6 ),	/* 6 */
/* 818 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 820 */	NdrFcShort( 0x0 ),	/* 0 */
/* 822 */	NdrFcShort( 0x8 ),	/* 8 */
/* 824 */	0x47,		/* Oi2 Flags:  srv must size, clt must size, has return, has ext, */
			0x3,		/* 3 */
/* 826 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 828 */	NdrFcShort( 0x0 ),	/* 0 */
/* 830 */	NdrFcShort( 0x0 ),	/* 0 */
/* 832 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pmkObjectName */

/* 834 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 836 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 838 */	NdrFcShort( 0x134 ),	/* Type Offset=308 */

	/* Parameter ppunkObject */

/* 840 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
/* 842 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 844 */	NdrFcShort( 0x116 ),	/* Type Offset=278 */

	/* Return value */

/* 846 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 848 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 850 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure NoteChangeTime */

/* 852 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 854 */	NdrFcLong( 0x0 ),	/* 0 */
/* 858 */	NdrFcShort( 0x7 ),	/* 7 */
/* 860 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 862 */	NdrFcShort( 0x34 ),	/* 52 */
/* 864 */	NdrFcShort( 0x8 ),	/* 8 */
/* 866 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x3,		/* 3 */
/* 868 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 870 */	NdrFcShort( 0x0 ),	/* 0 */
/* 872 */	NdrFcShort( 0x0 ),	/* 0 */
/* 874 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter dwRegister */

/* 876 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 878 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 880 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter pfiletime */

/* 882 */	NdrFcShort( 0x10a ),	/* Flags:  must free, in, simple ref, */
/* 884 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 886 */	NdrFcShort( 0x19e ),	/* Type Offset=414 */

	/* Return value */

/* 888 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 890 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 892 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetTimeOfLastChange */

/* 894 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 896 */	NdrFcLong( 0x0 ),	/* 0 */
/* 900 */	NdrFcShort( 0x8 ),	/* 8 */
/* 902 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 904 */	NdrFcShort( 0x0 ),	/* 0 */
/* 906 */	NdrFcShort( 0x34 ),	/* 52 */
/* 908 */	0x46,		/* Oi2 Flags:  clt must size, has return, has ext, */
			0x3,		/* 3 */
/* 910 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 912 */	NdrFcShort( 0x0 ),	/* 0 */
/* 914 */	NdrFcShort( 0x0 ),	/* 0 */
/* 916 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pmkObjectName */

/* 918 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 920 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 922 */	NdrFcShort( 0x134 ),	/* Type Offset=308 */

	/* Parameter pfiletime */

/* 924 */	NdrFcShort( 0x2112 ),	/* Flags:  must free, out, simple ref, srv alloc size=8 */
/* 926 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 928 */	NdrFcShort( 0x19e ),	/* Type Offset=414 */

	/* Return value */

/* 930 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 932 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 934 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure EnumRunning */

/* 936 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 938 */	NdrFcLong( 0x0 ),	/* 0 */
/* 942 */	NdrFcShort( 0x9 ),	/* 9 */
/* 944 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 946 */	NdrFcShort( 0x0 ),	/* 0 */
/* 948 */	NdrFcShort( 0x8 ),	/* 8 */
/* 950 */	0x45,		/* Oi2 Flags:  srv must size, has return, has ext, */
			0x2,		/* 2 */
/* 952 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 954 */	NdrFcShort( 0x0 ),	/* 0 */
/* 956 */	NdrFcShort( 0x0 ),	/* 0 */
/* 958 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter ppenumMoniker */

/* 960 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
/* 962 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 964 */	NdrFcShort( 0x15c ),	/* Type Offset=348 */

	/* Return value */

/* 966 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 968 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 970 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure Signal */


	/* Procedure ReleaseWriteAccess */


	/* Procedure FreeSurrogate */


	/* Procedure Unlock */


	/* Procedure IsDirty */


	/* Procedure IsDirty */


	/* Procedure IsDirty */

/* 972 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 974 */	NdrFcLong( 0x0 ),	/* 0 */
/* 978 */	NdrFcShort( 0x4 ),	/* 4 */
/* 980 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 982 */	NdrFcShort( 0x0 ),	/* 0 */
/* 984 */	NdrFcShort( 0x8 ),	/* 8 */
/* 986 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x1,		/* 1 */
/* 988 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 990 */	NdrFcShort( 0x0 ),	/* 0 */
/* 992 */	NdrFcShort( 0x0 ),	/* 0 */
/* 994 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Return value */


	/* Return value */


	/* Return value */


	/* Return value */


	/* Return value */


	/* Return value */


	/* Return value */

/* 996 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 998 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 1000 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure Load */

/* 1002 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 1004 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1008 */	NdrFcShort( 0x5 ),	/* 5 */
/* 1010 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 1012 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1014 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1016 */	0x46,		/* Oi2 Flags:  clt must size, has return, has ext, */
			0x2,		/* 2 */
/* 1018 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 1020 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1022 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1024 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pStm */

/* 1026 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 1028 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 1030 */	NdrFcShort( 0x1aa ),	/* Type Offset=426 */

	/* Return value */

/* 1032 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1034 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 1036 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure Save */

/* 1038 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 1040 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1044 */	NdrFcShort( 0x6 ),	/* 6 */
/* 1046 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 1048 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1050 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1052 */	0x46,		/* Oi2 Flags:  clt must size, has return, has ext, */
			0x3,		/* 3 */
/* 1054 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 1056 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1058 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1060 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pStm */

/* 1062 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 1064 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 1066 */	NdrFcShort( 0x1aa ),	/* Type Offset=426 */

	/* Parameter fClearDirty */

/* 1068 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 1070 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 1072 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 1074 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1076 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 1078 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetSizeMax */

/* 1080 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 1082 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1086 */	NdrFcShort( 0x7 ),	/* 7 */
/* 1088 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 1090 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1092 */	NdrFcShort( 0x34 ),	/* 52 */
/* 1094 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x2,		/* 2 */
/* 1096 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 1098 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1100 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1102 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pcbSize */

/* 1104 */	NdrFcShort( 0x2112 ),	/* Flags:  must free, out, simple ref, srv alloc size=8 */
/* 1106 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 1108 */	NdrFcShort( 0x1c0 ),	/* Type Offset=448 */

	/* Return value */

/* 1110 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1112 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 1114 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure RemoteBindToObject */

/* 1116 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 1118 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1122 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1124 */	NdrFcShort( 0x18 ),	/* x86 Stack size/offset = 24 */
/* 1126 */	NdrFcShort( 0x44 ),	/* 68 */
/* 1128 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1130 */	0x47,		/* Oi2 Flags:  srv must size, clt must size, has return, has ext, */
			0x5,		/* 5 */
/* 1132 */	0x8,		/* 8 */
			0x3,		/* Ext Flags:  new corr desc, clt corr check, */
/* 1134 */	NdrFcShort( 0x1 ),	/* 1 */
/* 1136 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1138 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pbc */

/* 1140 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 1142 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 1144 */	NdrFcShort( 0x188 ),	/* Type Offset=392 */

	/* Parameter pmkToLeft */

/* 1146 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 1148 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 1150 */	NdrFcShort( 0x134 ),	/* Type Offset=308 */

	/* Parameter riidResult */

/* 1152 */	NdrFcShort( 0x10a ),	/* Flags:  must free, in, simple ref, */
/* 1154 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 1156 */	NdrFcShort( 0x17c ),	/* Type Offset=380 */

	/* Parameter ppvResult */

/* 1158 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
/* 1160 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 1162 */	NdrFcShort( 0x1ca ),	/* Type Offset=458 */

	/* Return value */

/* 1164 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1166 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 1168 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure RemoteBindToStorage */

/* 1170 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 1172 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1176 */	NdrFcShort( 0x9 ),	/* 9 */
/* 1178 */	NdrFcShort( 0x18 ),	/* x86 Stack size/offset = 24 */
/* 1180 */	NdrFcShort( 0x44 ),	/* 68 */
/* 1182 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1184 */	0x47,		/* Oi2 Flags:  srv must size, clt must size, has return, has ext, */
			0x5,		/* 5 */
/* 1186 */	0x8,		/* 8 */
			0x3,		/* Ext Flags:  new corr desc, clt corr check, */
/* 1188 */	NdrFcShort( 0x1 ),	/* 1 */
/* 1190 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1192 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pbc */

/* 1194 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 1196 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 1198 */	NdrFcShort( 0x188 ),	/* Type Offset=392 */

	/* Parameter pmkToLeft */

/* 1200 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 1202 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 1204 */	NdrFcShort( 0x134 ),	/* Type Offset=308 */

	/* Parameter riid */

/* 1206 */	NdrFcShort( 0x10a ),	/* Flags:  must free, in, simple ref, */
/* 1208 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 1210 */	NdrFcShort( 0x17c ),	/* Type Offset=380 */

	/* Parameter ppvObj */

/* 1212 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
/* 1214 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 1216 */	NdrFcShort( 0x1d6 ),	/* Type Offset=470 */

	/* Return value */

/* 1218 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1220 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 1222 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure Reduce */

/* 1224 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 1226 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1230 */	NdrFcShort( 0xa ),	/* 10 */
/* 1232 */	NdrFcShort( 0x18 ),	/* x86 Stack size/offset = 24 */
/* 1234 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1236 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1238 */	0x47,		/* Oi2 Flags:  srv must size, clt must size, has return, has ext, */
			0x5,		/* 5 */
/* 1240 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 1242 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1244 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1246 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pbc */

/* 1248 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 1250 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 1252 */	NdrFcShort( 0x188 ),	/* Type Offset=392 */

	/* Parameter dwReduceHowFar */

/* 1254 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 1256 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 1258 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter ppmkToLeft */

/* 1260 */	NdrFcShort( 0x201b ),	/* Flags:  must size, must free, in, out, srv alloc size=8 */
/* 1262 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 1264 */	NdrFcShort( 0x1e2 ),	/* Type Offset=482 */

	/* Parameter ppmkReduced */

/* 1266 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
/* 1268 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 1270 */	NdrFcShort( 0x1e6 ),	/* Type Offset=486 */

	/* Return value */

/* 1272 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1274 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 1276 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure ComposeWith */

/* 1278 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 1280 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1284 */	NdrFcShort( 0xb ),	/* 11 */
/* 1286 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 1288 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1290 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1292 */	0x47,		/* Oi2 Flags:  srv must size, clt must size, has return, has ext, */
			0x4,		/* 4 */
/* 1294 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 1296 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1298 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1300 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pmkRight */

/* 1302 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 1304 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 1306 */	NdrFcShort( 0x134 ),	/* Type Offset=308 */

	/* Parameter fOnlyIfNotGeneric */

/* 1308 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 1310 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 1312 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter ppmkComposite */

/* 1314 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
/* 1316 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 1318 */	NdrFcShort( 0x1e6 ),	/* Type Offset=486 */

	/* Return value */

/* 1320 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1322 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 1324 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure Enum */

/* 1326 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 1328 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1332 */	NdrFcShort( 0xc ),	/* 12 */
/* 1334 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 1336 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1338 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1340 */	0x45,		/* Oi2 Flags:  srv must size, has return, has ext, */
			0x3,		/* 3 */
/* 1342 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 1344 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1346 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1348 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter fForward */

/* 1350 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 1352 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 1354 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter ppenumMoniker */

/* 1356 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
/* 1358 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 1360 */	NdrFcShort( 0x15c ),	/* Type Offset=348 */

	/* Return value */

/* 1362 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1364 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 1366 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure IsEqual */

/* 1368 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 1370 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1374 */	NdrFcShort( 0xd ),	/* 13 */
/* 1376 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 1378 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1380 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1382 */	0x46,		/* Oi2 Flags:  clt must size, has return, has ext, */
			0x2,		/* 2 */
/* 1384 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 1386 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1388 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1390 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pmkOtherMoniker */

/* 1392 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 1394 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 1396 */	NdrFcShort( 0x134 ),	/* Type Offset=308 */

	/* Return value */

/* 1398 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1400 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 1402 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure Hash */

/* 1404 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 1406 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1410 */	NdrFcShort( 0xe ),	/* 14 */
/* 1412 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 1414 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1416 */	NdrFcShort( 0x24 ),	/* 36 */
/* 1418 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x2,		/* 2 */
/* 1420 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 1422 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1424 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1426 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pdwHash */

/* 1428 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 1430 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 1432 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 1434 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1436 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 1438 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure IsRunning */

/* 1440 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 1442 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1446 */	NdrFcShort( 0xf ),	/* 15 */
/* 1448 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 1450 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1452 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1454 */	0x46,		/* Oi2 Flags:  clt must size, has return, has ext, */
			0x4,		/* 4 */
/* 1456 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 1458 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1460 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1462 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pbc */

/* 1464 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 1466 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 1468 */	NdrFcShort( 0x188 ),	/* Type Offset=392 */

	/* Parameter pmkToLeft */

/* 1470 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 1472 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 1474 */	NdrFcShort( 0x134 ),	/* Type Offset=308 */

	/* Parameter pmkNewlyRunning */

/* 1476 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 1478 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 1480 */	NdrFcShort( 0x134 ),	/* Type Offset=308 */

	/* Return value */

/* 1482 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1484 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 1486 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetTimeOfLastChange */

/* 1488 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 1490 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1494 */	NdrFcShort( 0x10 ),	/* 16 */
/* 1496 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 1498 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1500 */	NdrFcShort( 0x34 ),	/* 52 */
/* 1502 */	0x46,		/* Oi2 Flags:  clt must size, has return, has ext, */
			0x4,		/* 4 */
/* 1504 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 1506 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1508 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1510 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pbc */

/* 1512 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 1514 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 1516 */	NdrFcShort( 0x188 ),	/* Type Offset=392 */

	/* Parameter pmkToLeft */

/* 1518 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 1520 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 1522 */	NdrFcShort( 0x134 ),	/* Type Offset=308 */

	/* Parameter pFileTime */

/* 1524 */	NdrFcShort( 0x2112 ),	/* Flags:  must free, out, simple ref, srv alloc size=8 */
/* 1526 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 1528 */	NdrFcShort( 0x19e ),	/* Type Offset=414 */

	/* Return value */

/* 1530 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1532 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 1534 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure Inverse */

/* 1536 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 1538 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1542 */	NdrFcShort( 0x11 ),	/* 17 */
/* 1544 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 1546 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1548 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1550 */	0x45,		/* Oi2 Flags:  srv must size, has return, has ext, */
			0x2,		/* 2 */
/* 1552 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 1554 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1556 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1558 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter ppmk */

/* 1560 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
/* 1562 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 1564 */	NdrFcShort( 0x1e6 ),	/* Type Offset=486 */

	/* Return value */

/* 1566 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1568 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 1570 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure CommonPrefixWith */

/* 1572 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 1574 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1578 */	NdrFcShort( 0x12 ),	/* 18 */
/* 1580 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 1582 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1584 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1586 */	0x47,		/* Oi2 Flags:  srv must size, clt must size, has return, has ext, */
			0x3,		/* 3 */
/* 1588 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 1590 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1592 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1594 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pmkOther */

/* 1596 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 1598 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 1600 */	NdrFcShort( 0x134 ),	/* Type Offset=308 */

	/* Parameter ppmkPrefix */

/* 1602 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
/* 1604 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 1606 */	NdrFcShort( 0x1e6 ),	/* Type Offset=486 */

	/* Return value */

/* 1608 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1610 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 1612 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure RelativePathTo */

/* 1614 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 1616 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1620 */	NdrFcShort( 0x13 ),	/* 19 */
/* 1622 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 1624 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1626 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1628 */	0x47,		/* Oi2 Flags:  srv must size, clt must size, has return, has ext, */
			0x3,		/* 3 */
/* 1630 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 1632 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1634 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1636 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pmkOther */

/* 1638 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 1640 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 1642 */	NdrFcShort( 0x134 ),	/* Type Offset=308 */

	/* Parameter ppmkRelPath */

/* 1644 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
/* 1646 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 1648 */	NdrFcShort( 0x1e6 ),	/* Type Offset=486 */

	/* Return value */

/* 1650 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1652 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 1654 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetDisplayName */

/* 1656 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 1658 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1662 */	NdrFcShort( 0x14 ),	/* 20 */
/* 1664 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 1666 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1668 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1670 */	0x47,		/* Oi2 Flags:  srv must size, clt must size, has return, has ext, */
			0x4,		/* 4 */
/* 1672 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 1674 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1676 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1678 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pbc */

/* 1680 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 1682 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 1684 */	NdrFcShort( 0x188 ),	/* Type Offset=392 */

	/* Parameter pmkToLeft */

/* 1686 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 1688 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 1690 */	NdrFcShort( 0x134 ),	/* Type Offset=308 */

	/* Parameter ppszDisplayName */

/* 1692 */	NdrFcShort( 0x2013 ),	/* Flags:  must size, must free, out, srv alloc size=8 */
/* 1694 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 1696 */	NdrFcShort( 0x1ea ),	/* Type Offset=490 */

	/* Return value */

/* 1698 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1700 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 1702 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure ParseDisplayName */

/* 1704 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 1706 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1710 */	NdrFcShort( 0x15 ),	/* 21 */
/* 1712 */	NdrFcShort( 0x1c ),	/* x86 Stack size/offset = 28 */
/* 1714 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1716 */	NdrFcShort( 0x24 ),	/* 36 */
/* 1718 */	0x47,		/* Oi2 Flags:  srv must size, clt must size, has return, has ext, */
			0x6,		/* 6 */
/* 1720 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 1722 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1724 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1726 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pbc */

/* 1728 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 1730 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 1732 */	NdrFcShort( 0x188 ),	/* Type Offset=392 */

	/* Parameter pmkToLeft */

/* 1734 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 1736 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 1738 */	NdrFcShort( 0x134 ),	/* Type Offset=308 */

	/* Parameter pszDisplayName */

/* 1740 */	NdrFcShort( 0x10b ),	/* Flags:  must size, must free, in, simple ref, */
/* 1742 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 1744 */	NdrFcShort( 0x114 ),	/* Type Offset=276 */

	/* Parameter pchEaten */

/* 1746 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 1748 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 1750 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter ppmkOut */

/* 1752 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
/* 1754 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 1756 */	NdrFcShort( 0x1e6 ),	/* Type Offset=486 */

	/* Return value */

/* 1758 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1760 */	NdrFcShort( 0x18 ),	/* x86 Stack size/offset = 24 */
/* 1762 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure IsSystemMoniker */

/* 1764 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 1766 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1770 */	NdrFcShort( 0x16 ),	/* 22 */
/* 1772 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 1774 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1776 */	NdrFcShort( 0x24 ),	/* 36 */
/* 1778 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x2,		/* 2 */
/* 1780 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 1782 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1784 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1786 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pdwMksys */

/* 1788 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 1790 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 1792 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 1794 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1796 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 1798 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetComparisonData */

/* 1800 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 1802 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1806 */	NdrFcShort( 0x3 ),	/* 3 */
/* 1808 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 1810 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1812 */	NdrFcShort( 0x24 ),	/* 36 */
/* 1814 */	0x45,		/* Oi2 Flags:  srv must size, has return, has ext, */
			0x4,		/* 4 */
/* 1816 */	0x8,		/* 8 */
			0x3,		/* Ext Flags:  new corr desc, clt corr check, */
/* 1818 */	NdrFcShort( 0x1 ),	/* 1 */
/* 1820 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1822 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pbData */

/* 1824 */	NdrFcShort( 0x113 ),	/* Flags:  must size, must free, out, simple ref, */
/* 1826 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 1828 */	NdrFcShort( 0x1f6 ),	/* Type Offset=502 */

	/* Parameter cbMax */

/* 1830 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 1832 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 1834 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter pcbData */

/* 1836 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 1838 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 1840 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 1842 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1844 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 1846 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure RemoteNext */

/* 1848 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 1850 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1854 */	NdrFcShort( 0x3 ),	/* 3 */
/* 1856 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 1858 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1860 */	NdrFcShort( 0x24 ),	/* 36 */
/* 1862 */	0x45,		/* Oi2 Flags:  srv must size, has return, has ext, */
			0x4,		/* 4 */
/* 1864 */	0x8,		/* 8 */
			0x3,		/* Ext Flags:  new corr desc, clt corr check, */
/* 1866 */	NdrFcShort( 0x1 ),	/* 1 */
/* 1868 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1870 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter celt */

/* 1872 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 1874 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 1876 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter rgelt */

/* 1878 */	NdrFcShort( 0x113 ),	/* Flags:  must size, must free, out, simple ref, */
/* 1880 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 1882 */	NdrFcShort( 0x206 ),	/* Type Offset=518 */

	/* Parameter pceltFetched */

/* 1884 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 1886 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 1888 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 1890 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1892 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 1894 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure Clone */

/* 1896 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 1898 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1902 */	NdrFcShort( 0x6 ),	/* 6 */
/* 1904 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 1906 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1908 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1910 */	0x45,		/* Oi2 Flags:  srv must size, has return, has ext, */
			0x2,		/* 2 */
/* 1912 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 1914 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1916 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1918 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter ppenum */

/* 1920 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
/* 1922 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 1924 */	NdrFcShort( 0x11a ),	/* Type Offset=282 */

	/* Return value */

/* 1926 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1928 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 1930 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure Pull */


	/* Procedure RemoteRead */

/* 1932 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 1934 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1938 */	NdrFcShort( 0x3 ),	/* 3 */
/* 1940 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 1942 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1944 */	NdrFcShort( 0x24 ),	/* 36 */
/* 1946 */	0x45,		/* Oi2 Flags:  srv must size, has return, has ext, */
			0x4,		/* 4 */
/* 1948 */	0x8,		/* 8 */
			0x3,		/* Ext Flags:  new corr desc, clt corr check, */
/* 1950 */	NdrFcShort( 0x1 ),	/* 1 */
/* 1952 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1954 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter buf */


	/* Parameter pv */

/* 1956 */	NdrFcShort( 0x113 ),	/* Flags:  must size, must free, out, simple ref, */
/* 1958 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 1960 */	NdrFcShort( 0x230 ),	/* Type Offset=560 */

	/* Parameter cRequest */


	/* Parameter cb */

/* 1962 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 1964 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 1966 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter pcReturned */


	/* Parameter pcbRead */

/* 1968 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 1970 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 1972 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */


	/* Return value */

/* 1974 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1976 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 1978 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure RemoteWrite */

/* 1980 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 1982 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1986 */	NdrFcShort( 0x4 ),	/* 4 */
/* 1988 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 1990 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1992 */	NdrFcShort( 0x24 ),	/* 36 */
/* 1994 */	0x46,		/* Oi2 Flags:  clt must size, has return, has ext, */
			0x4,		/* 4 */
/* 1996 */	0x8,		/* 8 */
			0x5,		/* Ext Flags:  new corr desc, srv corr check, */
/* 1998 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2000 */	NdrFcShort( 0x1 ),	/* 1 */
/* 2002 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pv */

/* 2004 */	NdrFcShort( 0x10b ),	/* Flags:  must size, must free, in, simple ref, */
/* 2006 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 2008 */	NdrFcShort( 0x1f6 ),	/* Type Offset=502 */

	/* Parameter cb */

/* 2010 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 2012 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 2014 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter pcbWritten */

/* 2016 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 2018 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 2020 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 2022 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 2024 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 2026 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure RemoteSeek */

/* 2028 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 2030 */	NdrFcLong( 0x0 ),	/* 0 */
/* 2034 */	NdrFcShort( 0x5 ),	/* 5 */
/* 2036 */	NdrFcShort( 0x18 ),	/* x86 Stack size/offset = 24 */
/* 2038 */	NdrFcShort( 0x20 ),	/* 32 */
/* 2040 */	NdrFcShort( 0x34 ),	/* 52 */
/* 2042 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x4,		/* 4 */
/* 2044 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 2046 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2048 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2050 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter dlibMove */

/* 2052 */	NdrFcShort( 0x8a ),	/* Flags:  must free, in, by val, */
/* 2054 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 2056 */	NdrFcShort( 0x1c0 ),	/* Type Offset=448 */

	/* Parameter dwOrigin */

/* 2058 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 2060 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 2062 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter plibNewPosition */

/* 2064 */	NdrFcShort( 0x2112 ),	/* Flags:  must free, out, simple ref, srv alloc size=8 */
/* 2066 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 2068 */	NdrFcShort( 0x1c0 ),	/* Type Offset=448 */

	/* Return value */

/* 2070 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 2072 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 2074 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure SetSize */


	/* Procedure SetSize */

/* 2076 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 2078 */	NdrFcLong( 0x0 ),	/* 0 */
/* 2082 */	NdrFcShort( 0x6 ),	/* 6 */
/* 2084 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 2086 */	NdrFcShort( 0x18 ),	/* 24 */
/* 2088 */	NdrFcShort( 0x8 ),	/* 8 */
/* 2090 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x2,		/* 2 */
/* 2092 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 2094 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2096 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2098 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter cb */


	/* Parameter libNewSize */

/* 2100 */	NdrFcShort( 0x8a ),	/* Flags:  must free, in, by val, */
/* 2102 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 2104 */	NdrFcShort( 0x1c0 ),	/* Type Offset=448 */

	/* Return value */


	/* Return value */

/* 2106 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 2108 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 2110 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure RemoteCopyTo */

/* 2112 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 2114 */	NdrFcLong( 0x0 ),	/* 0 */
/* 2118 */	NdrFcShort( 0x7 ),	/* 7 */
/* 2120 */	NdrFcShort( 0x1c ),	/* x86 Stack size/offset = 28 */
/* 2122 */	NdrFcShort( 0x18 ),	/* 24 */
/* 2124 */	NdrFcShort( 0x60 ),	/* 96 */
/* 2126 */	0x46,		/* Oi2 Flags:  clt must size, has return, has ext, */
			0x5,		/* 5 */
/* 2128 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 2130 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2132 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2134 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pstm */

/* 2136 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 2138 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 2140 */	NdrFcShort( 0x1aa ),	/* Type Offset=426 */

	/* Parameter cb */

/* 2142 */	NdrFcShort( 0x8a ),	/* Flags:  must free, in, by val, */
/* 2144 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 2146 */	NdrFcShort( 0x1c0 ),	/* Type Offset=448 */

	/* Parameter pcbRead */

/* 2148 */	NdrFcShort( 0x2112 ),	/* Flags:  must free, out, simple ref, srv alloc size=8 */
/* 2150 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 2152 */	NdrFcShort( 0x1c0 ),	/* Type Offset=448 */

	/* Parameter pcbWritten */

/* 2154 */	NdrFcShort( 0x2112 ),	/* Flags:  must free, out, simple ref, srv alloc size=8 */
/* 2156 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 2158 */	NdrFcShort( 0x1c0 ),	/* Type Offset=448 */

	/* Return value */

/* 2160 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 2162 */	NdrFcShort( 0x18 ),	/* x86 Stack size/offset = 24 */
/* 2164 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure Commit */

/* 2166 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 2168 */	NdrFcLong( 0x0 ),	/* 0 */
/* 2172 */	NdrFcShort( 0x8 ),	/* 8 */
/* 2174 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 2176 */	NdrFcShort( 0x8 ),	/* 8 */
/* 2178 */	NdrFcShort( 0x8 ),	/* 8 */
/* 2180 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x2,		/* 2 */
/* 2182 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 2184 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2186 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2188 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter grfCommitFlags */

/* 2190 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 2192 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 2194 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 2196 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 2198 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 2200 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure HandsOffStorage */


	/* Procedure Revert */

/* 2202 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 2204 */	NdrFcLong( 0x0 ),	/* 0 */
/* 2208 */	NdrFcShort( 0x9 ),	/* 9 */
/* 2210 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 2212 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2214 */	NdrFcShort( 0x8 ),	/* 8 */
/* 2216 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x1,		/* 1 */
/* 2218 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 2220 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2222 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2224 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Return value */


	/* Return value */

/* 2226 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 2228 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 2230 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure LockRegion */

/* 2232 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 2234 */	NdrFcLong( 0x0 ),	/* 0 */
/* 2238 */	NdrFcShort( 0xa ),	/* 10 */
/* 2240 */	NdrFcShort( 0x1c ),	/* x86 Stack size/offset = 28 */
/* 2242 */	NdrFcShort( 0x38 ),	/* 56 */
/* 2244 */	NdrFcShort( 0x8 ),	/* 8 */
/* 2246 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x4,		/* 4 */
/* 2248 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 2250 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2252 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2254 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter libOffset */

/* 2256 */	NdrFcShort( 0x8a ),	/* Flags:  must free, in, by val, */
/* 2258 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 2260 */	NdrFcShort( 0x1c0 ),	/* Type Offset=448 */

	/* Parameter cb */

/* 2262 */	NdrFcShort( 0x8a ),	/* Flags:  must free, in, by val, */
/* 2264 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 2266 */	NdrFcShort( 0x1c0 ),	/* Type Offset=448 */

	/* Parameter dwLockType */

/* 2268 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 2270 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 2272 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 2274 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 2276 */	NdrFcShort( 0x18 ),	/* x86 Stack size/offset = 24 */
/* 2278 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure UnlockRegion */

/* 2280 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 2282 */	NdrFcLong( 0x0 ),	/* 0 */
/* 2286 */	NdrFcShort( 0xb ),	/* 11 */
/* 2288 */	NdrFcShort( 0x1c ),	/* x86 Stack size/offset = 28 */
/* 2290 */	NdrFcShort( 0x38 ),	/* 56 */
/* 2292 */	NdrFcShort( 0x8 ),	/* 8 */
/* 2294 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x4,		/* 4 */
/* 2296 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 2298 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2300 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2302 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter libOffset */

/* 2304 */	NdrFcShort( 0x8a ),	/* Flags:  must free, in, by val, */
/* 2306 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 2308 */	NdrFcShort( 0x1c0 ),	/* Type Offset=448 */

	/* Parameter cb */

/* 2310 */	NdrFcShort( 0x8a ),	/* Flags:  must free, in, by val, */
/* 2312 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 2314 */	NdrFcShort( 0x1c0 ),	/* Type Offset=448 */

	/* Parameter dwLockType */

/* 2316 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 2318 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 2320 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 2322 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 2324 */	NdrFcShort( 0x18 ),	/* x86 Stack size/offset = 24 */
/* 2326 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure Stat */

/* 2328 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 2330 */	NdrFcLong( 0x0 ),	/* 0 */
/* 2334 */	NdrFcShort( 0xc ),	/* 12 */
/* 2336 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 2338 */	NdrFcShort( 0x8 ),	/* 8 */
/* 2340 */	NdrFcShort( 0x8 ),	/* 8 */
/* 2342 */	0x45,		/* Oi2 Flags:  srv must size, has return, has ext, */
			0x3,		/* 3 */
/* 2344 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 2346 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2348 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2350 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pstatstg */

/* 2352 */	NdrFcShort( 0x113 ),	/* Flags:  must size, must free, out, simple ref, */
/* 2354 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 2356 */	NdrFcShort( 0x246 ),	/* Type Offset=582 */

	/* Parameter grfStatFlag */

/* 2358 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 2360 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 2362 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 2364 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 2366 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 2368 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure Clone */

/* 2370 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 2372 */	NdrFcLong( 0x0 ),	/* 0 */
/* 2376 */	NdrFcShort( 0xd ),	/* 13 */
/* 2378 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 2380 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2382 */	NdrFcShort( 0x8 ),	/* 8 */
/* 2384 */	0x45,		/* Oi2 Flags:  srv must size, has return, has ext, */
			0x2,		/* 2 */
/* 2386 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 2388 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2390 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2392 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter ppstm */

/* 2394 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
/* 2396 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 2398 */	NdrFcShort( 0x272 ),	/* Type Offset=626 */

	/* Return value */

/* 2400 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 2402 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 2404 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure RemoteNext */

/* 2406 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 2408 */	NdrFcLong( 0x0 ),	/* 0 */
/* 2412 */	NdrFcShort( 0x3 ),	/* 3 */
/* 2414 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 2416 */	NdrFcShort( 0x8 ),	/* 8 */
/* 2418 */	NdrFcShort( 0x24 ),	/* 36 */
/* 2420 */	0x45,		/* Oi2 Flags:  srv must size, has return, has ext, */
			0x4,		/* 4 */
/* 2422 */	0x8,		/* 8 */
			0x3,		/* Ext Flags:  new corr desc, clt corr check, */
/* 2424 */	NdrFcShort( 0x1 ),	/* 1 */
/* 2426 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2428 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter celt */

/* 2430 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 2432 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 2434 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter rgelt */

/* 2436 */	NdrFcShort( 0x113 ),	/* Flags:  must size, must free, out, simple ref, */
/* 2438 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 2440 */	NdrFcShort( 0x27a ),	/* Type Offset=634 */

	/* Parameter pceltFetched */

/* 2442 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 2444 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 2446 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 2448 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 2450 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 2452 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure Clone */

/* 2454 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 2456 */	NdrFcLong( 0x0 ),	/* 0 */
/* 2460 */	NdrFcShort( 0x6 ),	/* 6 */
/* 2462 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 2464 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2466 */	NdrFcShort( 0x8 ),	/* 8 */
/* 2468 */	0x45,		/* Oi2 Flags:  srv must size, has return, has ext, */
			0x2,		/* 2 */
/* 2470 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 2472 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2474 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2476 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter ppenum */

/* 2478 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
/* 2480 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 2482 */	NdrFcShort( 0x2a2 ),	/* Type Offset=674 */

	/* Return value */

/* 2484 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 2486 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 2488 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure CreateStream */

/* 2490 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 2492 */	NdrFcLong( 0x0 ),	/* 0 */
/* 2496 */	NdrFcShort( 0x3 ),	/* 3 */
/* 2498 */	NdrFcShort( 0x1c ),	/* x86 Stack size/offset = 28 */
/* 2500 */	NdrFcShort( 0x18 ),	/* 24 */
/* 2502 */	NdrFcShort( 0x8 ),	/* 8 */
/* 2504 */	0x47,		/* Oi2 Flags:  srv must size, clt must size, has return, has ext, */
			0x6,		/* 6 */
/* 2506 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 2508 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2510 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2512 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pwcsName */

/* 2514 */	NdrFcShort( 0x10b ),	/* Flags:  must size, must free, in, simple ref, */
/* 2516 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 2518 */	NdrFcShort( 0x114 ),	/* Type Offset=276 */

	/* Parameter grfMode */

/* 2520 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 2522 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 2524 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter reserved1 */

/* 2526 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 2528 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 2530 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter reserved2 */

/* 2532 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 2534 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 2536 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter ppstm */

/* 2538 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
/* 2540 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 2542 */	NdrFcShort( 0x272 ),	/* Type Offset=626 */

	/* Return value */

/* 2544 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 2546 */	NdrFcShort( 0x18 ),	/* x86 Stack size/offset = 24 */
/* 2548 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure RemoteOpenStream */

/* 2550 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 2552 */	NdrFcLong( 0x0 ),	/* 0 */
/* 2556 */	NdrFcShort( 0x4 ),	/* 4 */
/* 2558 */	NdrFcShort( 0x20 ),	/* x86 Stack size/offset = 32 */
/* 2560 */	NdrFcShort( 0x18 ),	/* 24 */
/* 2562 */	NdrFcShort( 0x8 ),	/* 8 */
/* 2564 */	0x47,		/* Oi2 Flags:  srv must size, clt must size, has return, has ext, */
			0x7,		/* 7 */
/* 2566 */	0x8,		/* 8 */
			0x5,		/* Ext Flags:  new corr desc, srv corr check, */
/* 2568 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2570 */	NdrFcShort( 0x1 ),	/* 1 */
/* 2572 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pwcsName */

/* 2574 */	NdrFcShort( 0x10b ),	/* Flags:  must size, must free, in, simple ref, */
/* 2576 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 2578 */	NdrFcShort( 0x114 ),	/* Type Offset=276 */

	/* Parameter cbReserved1 */

/* 2580 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 2582 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 2584 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter reserved1 */

/* 2586 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 2588 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 2590 */	NdrFcShort( 0x2b8 ),	/* Type Offset=696 */

	/* Parameter grfMode */

/* 2592 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 2594 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 2596 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter reserved2 */

/* 2598 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 2600 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 2602 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter ppstm */

/* 2604 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
/* 2606 */	NdrFcShort( 0x18 ),	/* x86 Stack size/offset = 24 */
/* 2608 */	NdrFcShort( 0x272 ),	/* Type Offset=626 */

	/* Return value */

/* 2610 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 2612 */	NdrFcShort( 0x1c ),	/* x86 Stack size/offset = 28 */
/* 2614 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure CreateStorage */

/* 2616 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 2618 */	NdrFcLong( 0x0 ),	/* 0 */
/* 2622 */	NdrFcShort( 0x5 ),	/* 5 */
/* 2624 */	NdrFcShort( 0x1c ),	/* x86 Stack size/offset = 28 */
/* 2626 */	NdrFcShort( 0x18 ),	/* 24 */
/* 2628 */	NdrFcShort( 0x8 ),	/* 8 */
/* 2630 */	0x47,		/* Oi2 Flags:  srv must size, clt must size, has return, has ext, */
			0x6,		/* 6 */
/* 2632 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 2634 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2636 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2638 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pwcsName */

/* 2640 */	NdrFcShort( 0x10b ),	/* Flags:  must size, must free, in, simple ref, */
/* 2642 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 2644 */	NdrFcShort( 0x114 ),	/* Type Offset=276 */

	/* Parameter grfMode */

/* 2646 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 2648 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 2650 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter reserved1 */

/* 2652 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 2654 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 2656 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter reserved2 */

/* 2658 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 2660 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 2662 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter ppstg */

/* 2664 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
/* 2666 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 2668 */	NdrFcShort( 0x2c8 ),	/* Type Offset=712 */

	/* Return value */

/* 2670 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 2672 */	NdrFcShort( 0x18 ),	/* x86 Stack size/offset = 24 */
/* 2674 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure OpenStorage */

/* 2676 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 2678 */	NdrFcLong( 0x0 ),	/* 0 */
/* 2682 */	NdrFcShort( 0x6 ),	/* 6 */
/* 2684 */	NdrFcShort( 0x20 ),	/* x86 Stack size/offset = 32 */
/* 2686 */	NdrFcShort( 0x10 ),	/* 16 */
/* 2688 */	NdrFcShort( 0x8 ),	/* 8 */
/* 2690 */	0x47,		/* Oi2 Flags:  srv must size, clt must size, has return, has ext, */
			0x7,		/* 7 */
/* 2692 */	0x8,		/* 8 */
			0x5,		/* Ext Flags:  new corr desc, srv corr check, */
/* 2694 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2696 */	NdrFcShort( 0x1 ),	/* 1 */
/* 2698 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pwcsName */

/* 2700 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 2702 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 2704 */	NdrFcShort( 0x2de ),	/* Type Offset=734 */

	/* Parameter pstgPriority */

/* 2706 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 2708 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 2710 */	NdrFcShort( 0x2cc ),	/* Type Offset=716 */

	/* Parameter grfMode */

/* 2712 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 2714 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 2716 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter snbExclude */

/* 2718 */	NdrFcShort( 0x8b ),	/* Flags:  must size, must free, in, by val, */
/* 2720 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 2722 */	NdrFcShort( 0x2fc ),	/* Type Offset=764 */

	/* Parameter reserved */

/* 2724 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 2726 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 2728 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter ppstg */

/* 2730 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
/* 2732 */	NdrFcShort( 0x18 ),	/* x86 Stack size/offset = 24 */
/* 2734 */	NdrFcShort( 0x2c8 ),	/* Type Offset=712 */

	/* Return value */

/* 2736 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 2738 */	NdrFcShort( 0x1c ),	/* x86 Stack size/offset = 28 */
/* 2740 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure CopyTo */

/* 2742 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 2744 */	NdrFcLong( 0x0 ),	/* 0 */
/* 2748 */	NdrFcShort( 0x7 ),	/* 7 */
/* 2750 */	NdrFcShort( 0x18 ),	/* x86 Stack size/offset = 24 */
/* 2752 */	NdrFcShort( 0x8 ),	/* 8 */
/* 2754 */	NdrFcShort( 0x8 ),	/* 8 */
/* 2756 */	0x46,		/* Oi2 Flags:  clt must size, has return, has ext, */
			0x5,		/* 5 */
/* 2758 */	0x8,		/* 8 */
			0x5,		/* Ext Flags:  new corr desc, srv corr check, */
/* 2760 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2762 */	NdrFcShort( 0x2 ),	/* 2 */
/* 2764 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter ciidExclude */

/* 2766 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 2768 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 2770 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter rgiidExclude */

/* 2772 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 2774 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 2776 */	NdrFcShort( 0x306 ),	/* Type Offset=774 */

	/* Parameter snbExclude */

/* 2778 */	NdrFcShort( 0x8b ),	/* Flags:  must size, must free, in, by val, */
/* 2780 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 2782 */	NdrFcShort( 0x2fc ),	/* Type Offset=764 */

	/* Parameter pstgDest */

/* 2784 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 2786 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 2788 */	NdrFcShort( 0x2cc ),	/* Type Offset=716 */

	/* Return value */

/* 2790 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 2792 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 2794 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure MoveElementTo */

/* 2796 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 2798 */	NdrFcLong( 0x0 ),	/* 0 */
/* 2802 */	NdrFcShort( 0x8 ),	/* 8 */
/* 2804 */	NdrFcShort( 0x18 ),	/* x86 Stack size/offset = 24 */
/* 2806 */	NdrFcShort( 0x8 ),	/* 8 */
/* 2808 */	NdrFcShort( 0x8 ),	/* 8 */
/* 2810 */	0x46,		/* Oi2 Flags:  clt must size, has return, has ext, */
			0x5,		/* 5 */
/* 2812 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 2814 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2816 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2818 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pwcsName */

/* 2820 */	NdrFcShort( 0x10b ),	/* Flags:  must size, must free, in, simple ref, */
/* 2822 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 2824 */	NdrFcShort( 0x114 ),	/* Type Offset=276 */

	/* Parameter pstgDest */

/* 2826 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 2828 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 2830 */	NdrFcShort( 0x2cc ),	/* Type Offset=716 */

	/* Parameter pwcsNewName */

/* 2832 */	NdrFcShort( 0x10b ),	/* Flags:  must size, must free, in, simple ref, */
/* 2834 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 2836 */	NdrFcShort( 0x114 ),	/* Type Offset=276 */

	/* Parameter grfFlags */

/* 2838 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 2840 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 2842 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 2844 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 2846 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 2848 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure Commit */

/* 2850 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 2852 */	NdrFcLong( 0x0 ),	/* 0 */
/* 2856 */	NdrFcShort( 0x9 ),	/* 9 */
/* 2858 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 2860 */	NdrFcShort( 0x8 ),	/* 8 */
/* 2862 */	NdrFcShort( 0x8 ),	/* 8 */
/* 2864 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x2,		/* 2 */
/* 2866 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 2868 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2870 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2872 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter grfCommitFlags */

/* 2874 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 2876 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 2878 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 2880 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 2882 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 2884 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure Revert */

/* 2886 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 2888 */	NdrFcLong( 0x0 ),	/* 0 */
/* 2892 */	NdrFcShort( 0xa ),	/* 10 */
/* 2894 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 2896 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2898 */	NdrFcShort( 0x8 ),	/* 8 */
/* 2900 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x1,		/* 1 */
/* 2902 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 2904 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2906 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2908 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Return value */

/* 2910 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 2912 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 2914 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure RemoteEnumElements */

/* 2916 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 2918 */	NdrFcLong( 0x0 ),	/* 0 */
/* 2922 */	NdrFcShort( 0xb ),	/* 11 */
/* 2924 */	NdrFcShort( 0x1c ),	/* x86 Stack size/offset = 28 */
/* 2926 */	NdrFcShort( 0x18 ),	/* 24 */
/* 2928 */	NdrFcShort( 0x8 ),	/* 8 */
/* 2930 */	0x47,		/* Oi2 Flags:  srv must size, clt must size, has return, has ext, */
			0x6,		/* 6 */
/* 2932 */	0x8,		/* 8 */
			0x5,		/* Ext Flags:  new corr desc, srv corr check, */
/* 2934 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2936 */	NdrFcShort( 0x1 ),	/* 1 */
/* 2938 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter reserved1 */

/* 2940 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 2942 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 2944 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter cbReserved2 */

/* 2946 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 2948 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 2950 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter reserved2 */

/* 2952 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 2954 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 2956 */	NdrFcShort( 0x2b8 ),	/* Type Offset=696 */

	/* Parameter reserved3 */

/* 2958 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 2960 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 2962 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter ppenum */

/* 2964 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
/* 2966 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 2968 */	NdrFcShort( 0x2a2 ),	/* Type Offset=674 */

	/* Return value */

/* 2970 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 2972 */	NdrFcShort( 0x18 ),	/* x86 Stack size/offset = 24 */
/* 2974 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure RenameElement */

/* 2976 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 2978 */	NdrFcLong( 0x0 ),	/* 0 */
/* 2982 */	NdrFcShort( 0xd ),	/* 13 */
/* 2984 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 2986 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2988 */	NdrFcShort( 0x8 ),	/* 8 */
/* 2990 */	0x46,		/* Oi2 Flags:  clt must size, has return, has ext, */
			0x3,		/* 3 */
/* 2992 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 2994 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2996 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2998 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pwcsOldName */

/* 3000 */	NdrFcShort( 0x10b ),	/* Flags:  must size, must free, in, simple ref, */
/* 3002 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 3004 */	NdrFcShort( 0x114 ),	/* Type Offset=276 */

	/* Parameter pwcsNewName */

/* 3006 */	NdrFcShort( 0x10b ),	/* Flags:  must size, must free, in, simple ref, */
/* 3008 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 3010 */	NdrFcShort( 0x114 ),	/* Type Offset=276 */

	/* Return value */

/* 3012 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 3014 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 3016 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure SetElementTimes */

/* 3018 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 3020 */	NdrFcLong( 0x0 ),	/* 0 */
/* 3024 */	NdrFcShort( 0xe ),	/* 14 */
/* 3026 */	NdrFcShort( 0x18 ),	/* x86 Stack size/offset = 24 */
/* 3028 */	NdrFcShort( 0x84 ),	/* 132 */
/* 3030 */	NdrFcShort( 0x8 ),	/* 8 */
/* 3032 */	0x46,		/* Oi2 Flags:  clt must size, has return, has ext, */
			0x5,		/* 5 */
/* 3034 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 3036 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3038 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3040 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pwcsName */

/* 3042 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 3044 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 3046 */	NdrFcShort( 0x2de ),	/* Type Offset=734 */

	/* Parameter pctime */

/* 3048 */	NdrFcShort( 0xa ),	/* Flags:  must free, in, */
/* 3050 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 3052 */	NdrFcShort( 0x31a ),	/* Type Offset=794 */

	/* Parameter patime */

/* 3054 */	NdrFcShort( 0xa ),	/* Flags:  must free, in, */
/* 3056 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 3058 */	NdrFcShort( 0x31a ),	/* Type Offset=794 */

	/* Parameter pmtime */

/* 3060 */	NdrFcShort( 0xa ),	/* Flags:  must free, in, */
/* 3062 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 3064 */	NdrFcShort( 0x31a ),	/* Type Offset=794 */

	/* Return value */

/* 3066 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 3068 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 3070 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure SetClass */

/* 3072 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 3074 */	NdrFcLong( 0x0 ),	/* 0 */
/* 3078 */	NdrFcShort( 0xf ),	/* 15 */
/* 3080 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 3082 */	NdrFcShort( 0x44 ),	/* 68 */
/* 3084 */	NdrFcShort( 0x8 ),	/* 8 */
/* 3086 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x2,		/* 2 */
/* 3088 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 3090 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3092 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3094 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter clsid */

/* 3096 */	NdrFcShort( 0x10a ),	/* Flags:  must free, in, simple ref, */
/* 3098 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 3100 */	NdrFcShort( 0x17c ),	/* Type Offset=380 */

	/* Return value */

/* 3102 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 3104 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 3106 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure SetStateBits */

/* 3108 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 3110 */	NdrFcLong( 0x0 ),	/* 0 */
/* 3114 */	NdrFcShort( 0x10 ),	/* 16 */
/* 3116 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 3118 */	NdrFcShort( 0x10 ),	/* 16 */
/* 3120 */	NdrFcShort( 0x8 ),	/* 8 */
/* 3122 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x3,		/* 3 */
/* 3124 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 3126 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3128 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3130 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter grfStateBits */

/* 3132 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 3134 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 3136 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter grfMask */

/* 3138 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 3140 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 3142 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 3144 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 3146 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 3148 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure Stat */

/* 3150 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 3152 */	NdrFcLong( 0x0 ),	/* 0 */
/* 3156 */	NdrFcShort( 0x11 ),	/* 17 */
/* 3158 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 3160 */	NdrFcShort( 0x8 ),	/* 8 */
/* 3162 */	NdrFcShort( 0x8 ),	/* 8 */
/* 3164 */	0x45,		/* Oi2 Flags:  srv must size, has return, has ext, */
			0x3,		/* 3 */
/* 3166 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 3168 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3170 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3172 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pstatstg */

/* 3174 */	NdrFcShort( 0x113 ),	/* Flags:  must size, must free, out, simple ref, */
/* 3176 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 3178 */	NdrFcShort( 0x246 ),	/* Type Offset=582 */

	/* Parameter grfStatFlag */

/* 3180 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 3182 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 3184 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 3186 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 3188 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 3190 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure Load */

/* 3192 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 3194 */	NdrFcLong( 0x0 ),	/* 0 */
/* 3198 */	NdrFcShort( 0x5 ),	/* 5 */
/* 3200 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 3202 */	NdrFcShort( 0x8 ),	/* 8 */
/* 3204 */	NdrFcShort( 0x8 ),	/* 8 */
/* 3206 */	0x46,		/* Oi2 Flags:  clt must size, has return, has ext, */
			0x3,		/* 3 */
/* 3208 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 3210 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3212 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3214 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pszFileName */

/* 3216 */	NdrFcShort( 0x10b ),	/* Flags:  must size, must free, in, simple ref, */
/* 3218 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 3220 */	NdrFcShort( 0x114 ),	/* Type Offset=276 */

	/* Parameter dwMode */

/* 3222 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 3224 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 3226 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 3228 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 3230 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 3232 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure Save */

/* 3234 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 3236 */	NdrFcLong( 0x0 ),	/* 0 */
/* 3240 */	NdrFcShort( 0x6 ),	/* 6 */
/* 3242 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 3244 */	NdrFcShort( 0x8 ),	/* 8 */
/* 3246 */	NdrFcShort( 0x8 ),	/* 8 */
/* 3248 */	0x46,		/* Oi2 Flags:  clt must size, has return, has ext, */
			0x3,		/* 3 */
/* 3250 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 3252 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3254 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3256 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pszFileName */

/* 3258 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 3260 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 3262 */	NdrFcShort( 0x2de ),	/* Type Offset=734 */

	/* Parameter fRemember */

/* 3264 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 3266 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 3268 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 3270 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 3272 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 3274 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure SaveCompleted */

/* 3276 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 3278 */	NdrFcLong( 0x0 ),	/* 0 */
/* 3282 */	NdrFcShort( 0x7 ),	/* 7 */
/* 3284 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 3286 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3288 */	NdrFcShort( 0x8 ),	/* 8 */
/* 3290 */	0x46,		/* Oi2 Flags:  clt must size, has return, has ext, */
			0x2,		/* 2 */
/* 3292 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 3294 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3296 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3298 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pszFileName */

/* 3300 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 3302 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 3304 */	NdrFcShort( 0x2de ),	/* Type Offset=734 */

	/* Return value */

/* 3306 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 3308 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 3310 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetCurFile */

/* 3312 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 3314 */	NdrFcLong( 0x0 ),	/* 0 */
/* 3318 */	NdrFcShort( 0x8 ),	/* 8 */
/* 3320 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 3322 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3324 */	NdrFcShort( 0x8 ),	/* 8 */
/* 3326 */	0x45,		/* Oi2 Flags:  srv must size, has return, has ext, */
			0x2,		/* 2 */
/* 3328 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 3330 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3332 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3334 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter ppszFileName */

/* 3336 */	NdrFcShort( 0x2013 ),	/* Flags:  must size, must free, out, srv alloc size=8 */
/* 3338 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 3340 */	NdrFcShort( 0x1ea ),	/* Type Offset=490 */

	/* Return value */

/* 3342 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 3344 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 3346 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure InitNew */

/* 3348 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 3350 */	NdrFcLong( 0x0 ),	/* 0 */
/* 3354 */	NdrFcShort( 0x5 ),	/* 5 */
/* 3356 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 3358 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3360 */	NdrFcShort( 0x8 ),	/* 8 */
/* 3362 */	0x46,		/* Oi2 Flags:  clt must size, has return, has ext, */
			0x2,		/* 2 */
/* 3364 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 3366 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3368 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3370 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pStg */

/* 3372 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 3374 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 3376 */	NdrFcShort( 0x2cc ),	/* Type Offset=716 */

	/* Return value */

/* 3378 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 3380 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 3382 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure Load */

/* 3384 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 3386 */	NdrFcLong( 0x0 ),	/* 0 */
/* 3390 */	NdrFcShort( 0x6 ),	/* 6 */
/* 3392 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 3394 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3396 */	NdrFcShort( 0x8 ),	/* 8 */
/* 3398 */	0x46,		/* Oi2 Flags:  clt must size, has return, has ext, */
			0x2,		/* 2 */
/* 3400 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 3402 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3404 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3406 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pStg */

/* 3408 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 3410 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 3412 */	NdrFcShort( 0x2cc ),	/* Type Offset=716 */

	/* Return value */

/* 3414 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 3416 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 3418 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure Save */

/* 3420 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 3422 */	NdrFcLong( 0x0 ),	/* 0 */
/* 3426 */	NdrFcShort( 0x7 ),	/* 7 */
/* 3428 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 3430 */	NdrFcShort( 0x8 ),	/* 8 */
/* 3432 */	NdrFcShort( 0x8 ),	/* 8 */
/* 3434 */	0x46,		/* Oi2 Flags:  clt must size, has return, has ext, */
			0x3,		/* 3 */
/* 3436 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 3438 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3440 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3442 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pStgSave */

/* 3444 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 3446 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 3448 */	NdrFcShort( 0x2cc ),	/* Type Offset=716 */

	/* Parameter fSameAsLoad */

/* 3450 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 3452 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 3454 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 3456 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 3458 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 3460 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure SaveCompleted */

/* 3462 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 3464 */	NdrFcLong( 0x0 ),	/* 0 */
/* 3468 */	NdrFcShort( 0x8 ),	/* 8 */
/* 3470 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 3472 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3474 */	NdrFcShort( 0x8 ),	/* 8 */
/* 3476 */	0x46,		/* Oi2 Flags:  clt must size, has return, has ext, */
			0x2,		/* 2 */
/* 3478 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 3480 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3482 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3484 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pStgNew */

/* 3486 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 3488 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 3490 */	NdrFcShort( 0x2cc ),	/* Type Offset=716 */

	/* Return value */

/* 3492 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 3494 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 3496 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure RemoteReadAt */

/* 3498 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 3500 */	NdrFcLong( 0x0 ),	/* 0 */
/* 3504 */	NdrFcShort( 0x3 ),	/* 3 */
/* 3506 */	NdrFcShort( 0x1c ),	/* x86 Stack size/offset = 28 */
/* 3508 */	NdrFcShort( 0x20 ),	/* 32 */
/* 3510 */	NdrFcShort( 0x24 ),	/* 36 */
/* 3512 */	0x45,		/* Oi2 Flags:  srv must size, has return, has ext, */
			0x5,		/* 5 */
/* 3514 */	0x8,		/* 8 */
			0x3,		/* Ext Flags:  new corr desc, clt corr check, */
/* 3516 */	NdrFcShort( 0x1 ),	/* 1 */
/* 3518 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3520 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter ulOffset */

/* 3522 */	NdrFcShort( 0x8a ),	/* Flags:  must free, in, by val, */
/* 3524 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 3526 */	NdrFcShort( 0x1c0 ),	/* Type Offset=448 */

	/* Parameter pv */

/* 3528 */	NdrFcShort( 0x113 ),	/* Flags:  must size, must free, out, simple ref, */
/* 3530 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 3532 */	NdrFcShort( 0x322 ),	/* Type Offset=802 */

	/* Parameter cb */

/* 3534 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 3536 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 3538 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter pcbRead */

/* 3540 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 3542 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 3544 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 3546 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 3548 */	NdrFcShort( 0x18 ),	/* x86 Stack size/offset = 24 */
/* 3550 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure RemoteFillAt */


	/* Procedure RemoteWriteAt */

/* 3552 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 3554 */	NdrFcLong( 0x0 ),	/* 0 */
/* 3558 */	NdrFcShort( 0x4 ),	/* 4 */
/* 3560 */	NdrFcShort( 0x1c ),	/* x86 Stack size/offset = 28 */
/* 3562 */	NdrFcShort( 0x20 ),	/* 32 */
/* 3564 */	NdrFcShort( 0x24 ),	/* 36 */
/* 3566 */	0x46,		/* Oi2 Flags:  clt must size, has return, has ext, */
			0x5,		/* 5 */
/* 3568 */	0x8,		/* 8 */
			0x5,		/* Ext Flags:  new corr desc, srv corr check, */
/* 3570 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3572 */	NdrFcShort( 0x1 ),	/* 1 */
/* 3574 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter ulOffset */


	/* Parameter ulOffset */

/* 3576 */	NdrFcShort( 0x8a ),	/* Flags:  must free, in, by val, */
/* 3578 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 3580 */	NdrFcShort( 0x1c0 ),	/* Type Offset=448 */

	/* Parameter pv */


	/* Parameter pv */

/* 3582 */	NdrFcShort( 0x10b ),	/* Flags:  must size, must free, in, simple ref, */
/* 3584 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 3586 */	NdrFcShort( 0x338 ),	/* Type Offset=824 */

	/* Parameter cb */


	/* Parameter cb */

/* 3588 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 3590 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 3592 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter pcbWritten */


	/* Parameter pcbWritten */

/* 3594 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 3596 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 3598 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */


	/* Return value */

/* 3600 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 3602 */	NdrFcShort( 0x18 ),	/* x86 Stack size/offset = 24 */
/* 3604 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure LockRegion */

/* 3606 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 3608 */	NdrFcLong( 0x0 ),	/* 0 */
/* 3612 */	NdrFcShort( 0x7 ),	/* 7 */
/* 3614 */	NdrFcShort( 0x1c ),	/* x86 Stack size/offset = 28 */
/* 3616 */	NdrFcShort( 0x38 ),	/* 56 */
/* 3618 */	NdrFcShort( 0x8 ),	/* 8 */
/* 3620 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x4,		/* 4 */
/* 3622 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 3624 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3626 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3628 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter libOffset */

/* 3630 */	NdrFcShort( 0x8a ),	/* Flags:  must free, in, by val, */
/* 3632 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 3634 */	NdrFcShort( 0x1c0 ),	/* Type Offset=448 */

	/* Parameter cb */

/* 3636 */	NdrFcShort( 0x8a ),	/* Flags:  must free, in, by val, */
/* 3638 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 3640 */	NdrFcShort( 0x1c0 ),	/* Type Offset=448 */

	/* Parameter dwLockType */

/* 3642 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 3644 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 3646 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 3648 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 3650 */	NdrFcShort( 0x18 ),	/* x86 Stack size/offset = 24 */
/* 3652 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure UnlockRegion */

/* 3654 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 3656 */	NdrFcLong( 0x0 ),	/* 0 */
/* 3660 */	NdrFcShort( 0x8 ),	/* 8 */
/* 3662 */	NdrFcShort( 0x1c ),	/* x86 Stack size/offset = 28 */
/* 3664 */	NdrFcShort( 0x38 ),	/* 56 */
/* 3666 */	NdrFcShort( 0x8 ),	/* 8 */
/* 3668 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x4,		/* 4 */
/* 3670 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 3672 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3674 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3676 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter libOffset */

/* 3678 */	NdrFcShort( 0x8a ),	/* Flags:  must free, in, by val, */
/* 3680 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 3682 */	NdrFcShort( 0x1c0 ),	/* Type Offset=448 */

	/* Parameter cb */

/* 3684 */	NdrFcShort( 0x8a ),	/* Flags:  must free, in, by val, */
/* 3686 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 3688 */	NdrFcShort( 0x1c0 ),	/* Type Offset=448 */

	/* Parameter dwLockType */

/* 3690 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 3692 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 3694 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 3696 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 3698 */	NdrFcShort( 0x18 ),	/* x86 Stack size/offset = 24 */
/* 3700 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure Stat */

/* 3702 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 3704 */	NdrFcLong( 0x0 ),	/* 0 */
/* 3708 */	NdrFcShort( 0x9 ),	/* 9 */
/* 3710 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 3712 */	NdrFcShort( 0x8 ),	/* 8 */
/* 3714 */	NdrFcShort( 0x8 ),	/* 8 */
/* 3716 */	0x45,		/* Oi2 Flags:  srv must size, has return, has ext, */
			0x3,		/* 3 */
/* 3718 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 3720 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3722 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3724 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pstatstg */

/* 3726 */	NdrFcShort( 0x113 ),	/* Flags:  must size, must free, out, simple ref, */
/* 3728 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 3730 */	NdrFcShort( 0x246 ),	/* Type Offset=582 */

	/* Parameter grfStatFlag */

/* 3732 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 3734 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 3736 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 3738 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 3740 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 3742 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure RemoteNext */

/* 3744 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 3746 */	NdrFcLong( 0x0 ),	/* 0 */
/* 3750 */	NdrFcShort( 0x3 ),	/* 3 */
/* 3752 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 3754 */	NdrFcShort( 0x8 ),	/* 8 */
/* 3756 */	NdrFcShort( 0x24 ),	/* 36 */
/* 3758 */	0x45,		/* Oi2 Flags:  srv must size, has return, has ext, */
			0x4,		/* 4 */
/* 3760 */	0x8,		/* 8 */
			0x3,		/* Ext Flags:  new corr desc, clt corr check, */
/* 3762 */	NdrFcShort( 0x3 ),	/* 3 */
/* 3764 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3766 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter celt */

/* 3768 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 3770 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 3772 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter rgelt */

/* 3774 */	NdrFcShort( 0x113 ),	/* Flags:  must size, must free, out, simple ref, */
/* 3776 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 3778 */	NdrFcShort( 0x398 ),	/* Type Offset=920 */

	/* Parameter pceltFetched */

/* 3780 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 3782 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 3784 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 3786 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 3788 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 3790 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure Clone */

/* 3792 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 3794 */	NdrFcLong( 0x0 ),	/* 0 */
/* 3798 */	NdrFcShort( 0x6 ),	/* 6 */
/* 3800 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 3802 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3804 */	NdrFcShort( 0x8 ),	/* 8 */
/* 3806 */	0x45,		/* Oi2 Flags:  srv must size, has return, has ext, */
			0x2,		/* 2 */
/* 3808 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 3810 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3812 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3814 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter ppenum */

/* 3816 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
/* 3818 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 3820 */	NdrFcShort( 0x3ae ),	/* Type Offset=942 */

	/* Return value */

/* 3822 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 3824 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 3826 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure RemoteNext */

/* 3828 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 3830 */	NdrFcLong( 0x0 ),	/* 0 */
/* 3834 */	NdrFcShort( 0x3 ),	/* 3 */
/* 3836 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 3838 */	NdrFcShort( 0x8 ),	/* 8 */
/* 3840 */	NdrFcShort( 0x24 ),	/* 36 */
/* 3842 */	0x45,		/* Oi2 Flags:  srv must size, has return, has ext, */
			0x4,		/* 4 */
/* 3844 */	0x8,		/* 8 */
			0x3,		/* Ext Flags:  new corr desc, clt corr check, */
/* 3846 */	NdrFcShort( 0x3 ),	/* 3 */
/* 3848 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3850 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter celt */

/* 3852 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 3854 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 3856 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter rgelt */

/* 3858 */	NdrFcShort( 0x113 ),	/* Flags:  must size, must free, out, simple ref, */
/* 3860 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 3862 */	NdrFcShort( 0x3ee ),	/* Type Offset=1006 */

	/* Parameter pceltFetched */

/* 3864 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 3866 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 3868 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 3870 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 3872 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 3874 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure Clone */

/* 3876 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 3878 */	NdrFcLong( 0x0 ),	/* 0 */
/* 3882 */	NdrFcShort( 0x6 ),	/* 6 */
/* 3884 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 3886 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3888 */	NdrFcShort( 0x8 ),	/* 8 */
/* 3890 */	0x45,		/* Oi2 Flags:  srv must size, has return, has ext, */
			0x2,		/* 2 */
/* 3892 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 3894 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3896 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3898 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter ppenum */

/* 3900 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
/* 3902 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 3904 */	NdrFcShort( 0x404 ),	/* Type Offset=1028 */

	/* Return value */

/* 3906 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 3908 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 3910 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure SwitchToFile */

/* 3912 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 3914 */	NdrFcLong( 0x0 ),	/* 0 */
/* 3918 */	NdrFcShort( 0x3 ),	/* 3 */
/* 3920 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 3922 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3924 */	NdrFcShort( 0x8 ),	/* 8 */
/* 3926 */	0x46,		/* Oi2 Flags:  clt must size, has return, has ext, */
			0x2,		/* 2 */
/* 3928 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 3930 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3932 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3934 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pszFile */

/* 3936 */	NdrFcShort( 0x10b ),	/* Flags:  must size, must free, in, simple ref, */
/* 3938 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 3940 */	NdrFcShort( 0x114 ),	/* Type Offset=276 */

	/* Return value */

/* 3942 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 3944 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 3946 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure RemoteOnDataChange */

/* 3948 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 3950 */	NdrFcLong( 0x0 ),	/* 0 */
/* 3954 */	NdrFcShort( 0x3 ),	/* 3 */
/* 3956 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 3958 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3960 */	NdrFcShort( 0x8 ),	/* 8 */
/* 3962 */	0x46,		/* Oi2 Flags:  clt must size, has return, has ext, */
			0x3,		/* 3 */
/* 3964 */	0x8,		/* 8 */
			0x5,		/* Ext Flags:  new corr desc, srv corr check, */
/* 3966 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3968 */	NdrFcShort( 0x13 ),	/* 19 */
/* 3970 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pFormatetc */

/* 3972 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 3974 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 3976 */	NdrFcShort( 0x41a ),	/* Type Offset=1050 */

	/* Parameter pStgmed */

/* 3978 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 3980 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 3982 */	NdrFcShort( 0x41e ),	/* Type Offset=1054 */

	/* Return value */

/* 3984 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 3986 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 3988 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure RemoteOnViewChange */

/* 3990 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 3992 */	NdrFcLong( 0x0 ),	/* 0 */
/* 3996 */	NdrFcShort( 0x4 ),	/* 4 */
/* 3998 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 4000 */	NdrFcShort( 0x10 ),	/* 16 */
/* 4002 */	NdrFcShort( 0x8 ),	/* 8 */
/* 4004 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x3,		/* 3 */
/* 4006 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 4008 */	NdrFcShort( 0x0 ),	/* 0 */
/* 4010 */	NdrFcShort( 0x0 ),	/* 0 */
/* 4012 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter dwAspect */

/* 4014 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 4016 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 4018 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter lindex */

/* 4020 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 4022 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 4024 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 4026 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 4028 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 4030 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure RemoteOnSave */

/* 4032 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 4034 */	NdrFcLong( 0x0 ),	/* 0 */
/* 4038 */	NdrFcShort( 0x6 ),	/* 6 */
/* 4040 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 4042 */	NdrFcShort( 0x0 ),	/* 0 */
/* 4044 */	NdrFcShort( 0x8 ),	/* 8 */
/* 4046 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x1,		/* 1 */
/* 4048 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 4050 */	NdrFcShort( 0x0 ),	/* 0 */
/* 4052 */	NdrFcShort( 0x0 ),	/* 0 */
/* 4054 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Return value */

/* 4056 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 4058 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 4060 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure RemoteOnClose */

/* 4062 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 4064 */	NdrFcLong( 0x0 ),	/* 0 */
/* 4068 */	NdrFcShort( 0x7 ),	/* 7 */
/* 4070 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 4072 */	NdrFcShort( 0x0 ),	/* 0 */
/* 4074 */	NdrFcShort( 0x8 ),	/* 8 */
/* 4076 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x1,		/* 1 */
/* 4078 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 4080 */	NdrFcShort( 0x0 ),	/* 0 */
/* 4082 */	NdrFcShort( 0x0 ),	/* 0 */
/* 4084 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Return value */

/* 4086 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 4088 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 4090 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure Begin_RemoteOnDataChange */

/* 4092 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 4094 */	NdrFcLong( 0x0 ),	/* 0 */
/* 4098 */	NdrFcShort( 0x3 ),	/* 3 */
/* 4100 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 4102 */	NdrFcShort( 0x0 ),	/* 0 */
/* 4104 */	NdrFcShort( 0x8 ),	/* 8 */
/* 4106 */	0x66,		/* Oi2 Flags:  clt must size, has return, has async uuid, has ext, */
			0x3,		/* 3 */
/* 4108 */	0x8,		/* 8 */
			0x5,		/* Ext Flags:  new corr desc, srv corr check, */
/* 4110 */	NdrFcShort( 0x0 ),	/* 0 */
/* 4112 */	NdrFcShort( 0x13 ),	/* 19 */
/* 4114 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pFormatetc */

/* 4116 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 4118 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 4120 */	NdrFcShort( 0x41a ),	/* Type Offset=1050 */

	/* Parameter pStgmed */

/* 4122 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 4124 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 4126 */	NdrFcShort( 0x41e ),	/* Type Offset=1054 */

	/* Return value */

/* 4128 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 4130 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 4132 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure Finish_RemoteOnDataChange */

/* 4134 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 4136 */	NdrFcLong( 0x0 ),	/* 0 */
/* 4140 */	NdrFcShort( 0x4 ),	/* 4 */
/* 4142 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 4144 */	NdrFcShort( 0x0 ),	/* 0 */
/* 4146 */	NdrFcShort( 0x8 ),	/* 8 */
/* 4148 */	0x64,		/* Oi2 Flags:  has return, has async uuid, has ext, */
			0x1,		/* 1 */
/* 4150 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 4152 */	NdrFcShort( 0x0 ),	/* 0 */
/* 4154 */	NdrFcShort( 0x0 ),	/* 0 */
/* 4156 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Return value */

/* 4158 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 4160 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 4162 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure Begin_RemoteOnViewChange */

/* 4164 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 4166 */	NdrFcLong( 0x0 ),	/* 0 */
/* 4170 */	NdrFcShort( 0x5 ),	/* 5 */
/* 4172 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 4174 */	NdrFcShort( 0x10 ),	/* 16 */
/* 4176 */	NdrFcShort( 0x8 ),	/* 8 */
/* 4178 */	0x64,		/* Oi2 Flags:  has return, has async uuid, has ext, */
			0x3,		/* 3 */
/* 4180 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 4182 */	NdrFcShort( 0x0 ),	/* 0 */
/* 4184 */	NdrFcShort( 0x0 ),	/* 0 */
/* 4186 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter dwAspect */

/* 4188 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 4190 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 4192 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter lindex */

/* 4194 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 4196 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 4198 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 4200 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 4202 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 4204 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure Finish_Push */


	/* Procedure Finish_Push */


	/* Procedure Finish_Push */


	/* Procedure Finish_RemoteOnViewChange */

/* 4206 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 4208 */	NdrFcLong( 0x0 ),	/* 0 */
/* 4212 */	NdrFcShort( 0x6 ),	/* 6 */
/* 4214 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 4216 */	NdrFcShort( 0x0 ),	/* 0 */
/* 4218 */	NdrFcShort( 0x8 ),	/* 8 */
/* 4220 */	0x64,		/* Oi2 Flags:  has return, has async uuid, has ext, */
			0x1,		/* 1 */
/* 4222 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 4224 */	NdrFcShort( 0x0 ),	/* 0 */
/* 4226 */	NdrFcShort( 0x0 ),	/* 0 */
/* 4228 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Return value */


	/* Return value */


	/* Return value */


	/* Return value */

/* 4230 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 4232 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 4234 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure Begin_RemoteOnRename */

/* 4236 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 4238 */	NdrFcLong( 0x0 ),	/* 0 */
/* 4242 */	NdrFcShort( 0x7 ),	/* 7 */
/* 4244 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 4246 */	NdrFcShort( 0x0 ),	/* 0 */
/* 4248 */	NdrFcShort( 0x8 ),	/* 8 */
/* 4250 */	0x66,		/* Oi2 Flags:  clt must size, has return, has async uuid, has ext, */
			0x2,		/* 2 */
/* 4252 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 4254 */	NdrFcShort( 0x0 ),	/* 0 */
/* 4256 */	NdrFcShort( 0x0 ),	/* 0 */
/* 4258 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pmk */

/* 4260 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 4262 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 4264 */	NdrFcShort( 0x134 ),	/* Type Offset=308 */

	/* Return value */

/* 4266 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 4268 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 4270 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure Finish_RemoteOnRename */

/* 4272 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 4274 */	NdrFcLong( 0x0 ),	/* 0 */
/* 4278 */	NdrFcShort( 0x8 ),	/* 8 */
/* 4280 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 4282 */	NdrFcShort( 0x0 ),	/* 0 */
/* 4284 */	NdrFcShort( 0x8 ),	/* 8 */
/* 4286 */	0x64,		/* Oi2 Flags:  has return, has async uuid, has ext, */
			0x1,		/* 1 */
/* 4288 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 4290 */	NdrFcShort( 0x0 ),	/* 0 */
/* 4292 */	NdrFcShort( 0x0 ),	/* 0 */
/* 4294 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Return value */

/* 4296 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 4298 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 4300 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure Begin_RemoteOnSave */

/* 4302 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 4304 */	NdrFcLong( 0x0 ),	/* 0 */
/* 4308 */	NdrFcShort( 0x9 ),	/* 9 */
/* 4310 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 4312 */	NdrFcShort( 0x0 ),	/* 0 */
/* 4314 */	NdrFcShort( 0x8 ),	/* 8 */
/* 4316 */	0x64,		/* Oi2 Flags:  has return, has async uuid, has ext, */
			0x1,		/* 1 */
/* 4318 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 4320 */	NdrFcShort( 0x0 ),	/* 0 */
/* 4322 */	NdrFcShort( 0x0 ),	/* 0 */
/* 4324 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Return value */

/* 4326 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 4328 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 4330 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure Finish_RemoteOnSave */

/* 4332 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 4334 */	NdrFcLong( 0x0 ),	/* 0 */
/* 4338 */	NdrFcShort( 0xa ),	/* 10 */
/* 4340 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 4342 */	NdrFcShort( 0x0 ),	/* 0 */
/* 4344 */	NdrFcShort( 0x8 ),	/* 8 */
/* 4346 */	0x64,		/* Oi2 Flags:  has return, has async uuid, has ext, */
			0x1,		/* 1 */
/* 4348 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 4350 */	NdrFcShort( 0x0 ),	/* 0 */
/* 4352 */	NdrFcShort( 0x0 ),	/* 0 */
/* 4354 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Return value */

/* 4356 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 4358 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 4360 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure Begin_RemoteOnClose */

/* 4362 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 4364 */	NdrFcLong( 0x0 ),	/* 0 */
/* 4368 */	NdrFcShort( 0xb ),	/* 11 */
/* 4370 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 4372 */	NdrFcShort( 0x0 ),	/* 0 */
/* 4374 */	NdrFcShort( 0x8 ),	/* 8 */
/* 4376 */	0x64,		/* Oi2 Flags:  has return, has async uuid, has ext, */
			0x1,		/* 1 */
/* 4378 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 4380 */	NdrFcShort( 0x0 ),	/* 0 */
/* 4382 */	NdrFcShort( 0x0 ),	/* 0 */
/* 4384 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Return value */

/* 4386 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 4388 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 4390 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure Finish_RemoteOnClose */

/* 4392 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 4394 */	NdrFcLong( 0x0 ),	/* 0 */
/* 4398 */	NdrFcShort( 0xc ),	/* 12 */
/* 4400 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 4402 */	NdrFcShort( 0x0 ),	/* 0 */
/* 4404 */	NdrFcShort( 0x8 ),	/* 8 */
/* 4406 */	0x64,		/* Oi2 Flags:  has return, has async uuid, has ext, */
			0x1,		/* 1 */
/* 4408 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 4410 */	NdrFcShort( 0x0 ),	/* 0 */
/* 4412 */	NdrFcShort( 0x0 ),	/* 0 */
/* 4414 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Return value */

/* 4416 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 4418 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 4420 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure RemoteOnLinkSrcChange */

/* 4422 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 4424 */	NdrFcLong( 0x0 ),	/* 0 */
/* 4428 */	NdrFcShort( 0x8 ),	/* 8 */
/* 4430 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 4432 */	NdrFcShort( 0x0 ),	/* 0 */
/* 4434 */	NdrFcShort( 0x8 ),	/* 8 */
/* 4436 */	0x46,		/* Oi2 Flags:  clt must size, has return, has ext, */
			0x2,		/* 2 */
/* 4438 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 4440 */	NdrFcShort( 0x0 ),	/* 0 */
/* 4442 */	NdrFcShort( 0x0 ),	/* 0 */
/* 4444 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pmk */

/* 4446 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 4448 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 4450 */	NdrFcShort( 0x134 ),	/* Type Offset=308 */

	/* Return value */

/* 4452 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 4454 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 4456 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure Begin_RemoteOnLinkSrcChange */

/* 4458 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 4460 */	NdrFcLong( 0x0 ),	/* 0 */
/* 4464 */	NdrFcShort( 0xd ),	/* 13 */
/* 4466 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 4468 */	NdrFcShort( 0x0 ),	/* 0 */
/* 4470 */	NdrFcShort( 0x8 ),	/* 8 */
/* 4472 */	0x66,		/* Oi2 Flags:  clt must size, has return, has async uuid, has ext, */
			0x2,		/* 2 */
/* 4474 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 4476 */	NdrFcShort( 0x0 ),	/* 0 */
/* 4478 */	NdrFcShort( 0x0 ),	/* 0 */
/* 4480 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pmk */

/* 4482 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 4484 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 4486 */	NdrFcShort( 0x134 ),	/* Type Offset=308 */

	/* Return value */

/* 4488 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 4490 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 4492 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure Finish_RemoteOnLinkSrcChange */

/* 4494 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 4496 */	NdrFcLong( 0x0 ),	/* 0 */
/* 4500 */	NdrFcShort( 0xe ),	/* 14 */
/* 4502 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 4504 */	NdrFcShort( 0x0 ),	/* 0 */
/* 4506 */	NdrFcShort( 0x8 ),	/* 8 */
/* 4508 */	0x64,		/* Oi2 Flags:  has return, has async uuid, has ext, */
			0x1,		/* 1 */
/* 4510 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 4512 */	NdrFcShort( 0x0 ),	/* 0 */
/* 4514 */	NdrFcShort( 0x0 ),	/* 0 */
/* 4516 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Return value */

/* 4518 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 4520 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 4522 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure RemoteGetData */

/* 4524 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 4526 */	NdrFcLong( 0x0 ),	/* 0 */
/* 4530 */	NdrFcShort( 0x3 ),	/* 3 */
/* 4532 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 4534 */	NdrFcShort( 0x0 ),	/* 0 */
/* 4536 */	NdrFcShort( 0x8 ),	/* 8 */
/* 4538 */	0x47,		/* Oi2 Flags:  srv must size, clt must size, has return, has ext, */
			0x3,		/* 3 */
/* 4540 */	0x8,		/* 8 */
			0x7,		/* Ext Flags:  new corr desc, clt corr check, srv corr check, */
/* 4542 */	NdrFcShort( 0x11 ),	/* 17 */
/* 4544 */	NdrFcShort( 0x2 ),	/* 2 */
/* 4546 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pformatetcIn */

/* 4548 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 4550 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 4552 */	NdrFcShort( 0x41a ),	/* Type Offset=1050 */

	/* Parameter pRemoteMedium */

/* 4554 */	NdrFcShort( 0x4113 ),	/* Flags:  must size, must free, out, simple ref, srv alloc size=16 */
/* 4556 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 4558 */	NdrFcShort( 0x5a6 ),	/* Type Offset=1446 */

	/* Return value */

/* 4560 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 4562 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 4564 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure RemoteGetDataHere */

/* 4566 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 4568 */	NdrFcLong( 0x0 ),	/* 0 */
/* 4572 */	NdrFcShort( 0x4 ),	/* 4 */
/* 4574 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 4576 */	NdrFcShort( 0x0 ),	/* 0 */
/* 4578 */	NdrFcShort( 0x8 ),	/* 8 */
/* 4580 */	0x47,		/* Oi2 Flags:  srv must size, clt must size, has return, has ext, */
			0x3,		/* 3 */
/* 4582 */	0x8,		/* 8 */
			0x7,		/* Ext Flags:  new corr desc, clt corr check, srv corr check, */
/* 4584 */	NdrFcShort( 0x11 ),	/* 17 */
/* 4586 */	NdrFcShort( 0x13 ),	/* 19 */
/* 4588 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pformatetc */

/* 4590 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 4592 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 4594 */	NdrFcShort( 0x41a ),	/* Type Offset=1050 */

	/* Parameter pRemoteMedium */

/* 4596 */	NdrFcShort( 0x11b ),	/* Flags:  must size, must free, in, out, simple ref, */
/* 4598 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 4600 */	NdrFcShort( 0x5a6 ),	/* Type Offset=1446 */

	/* Return value */

/* 4602 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 4604 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 4606 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure QueryGetData */

/* 4608 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 4610 */	NdrFcLong( 0x0 ),	/* 0 */
/* 4614 */	NdrFcShort( 0x5 ),	/* 5 */
/* 4616 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 4618 */	NdrFcShort( 0x0 ),	/* 0 */
/* 4620 */	NdrFcShort( 0x8 ),	/* 8 */
/* 4622 */	0x46,		/* Oi2 Flags:  clt must size, has return, has ext, */
			0x2,		/* 2 */
/* 4624 */	0x8,		/* 8 */
			0x5,		/* Ext Flags:  new corr desc, srv corr check, */
/* 4626 */	NdrFcShort( 0x0 ),	/* 0 */
/* 4628 */	NdrFcShort( 0x2 ),	/* 2 */
/* 4630 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pformatetc */

/* 4632 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 4634 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 4636 */	NdrFcShort( 0x41a ),	/* Type Offset=1050 */

	/* Return value */

/* 4638 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 4640 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 4642 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetCanonicalFormatEtc */

/* 4644 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 4646 */	NdrFcLong( 0x0 ),	/* 0 */
/* 4650 */	NdrFcShort( 0x6 ),	/* 6 */
/* 4652 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 4654 */	NdrFcShort( 0x0 ),	/* 0 */
/* 4656 */	NdrFcShort( 0x8 ),	/* 8 */
/* 4658 */	0x47,		/* Oi2 Flags:  srv must size, clt must size, has return, has ext, */
			0x3,		/* 3 */
/* 4660 */	0x8,		/* 8 */
			0x7,		/* Ext Flags:  new corr desc, clt corr check, srv corr check, */
/* 4662 */	NdrFcShort( 0x2 ),	/* 2 */
/* 4664 */	NdrFcShort( 0x2 ),	/* 2 */
/* 4666 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pformatectIn */

/* 4668 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 4670 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 4672 */	NdrFcShort( 0x41a ),	/* Type Offset=1050 */

	/* Parameter pformatetcOut */

/* 4674 */	NdrFcShort( 0x6113 ),	/* Flags:  must size, must free, out, simple ref, srv alloc size=24 */
/* 4676 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 4678 */	NdrFcShort( 0x382 ),	/* Type Offset=898 */

	/* Return value */

/* 4680 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 4682 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 4684 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure RemoteSetData */

/* 4686 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 4688 */	NdrFcLong( 0x0 ),	/* 0 */
/* 4692 */	NdrFcShort( 0x7 ),	/* 7 */
/* 4694 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 4696 */	NdrFcShort( 0x8 ),	/* 8 */
/* 4698 */	NdrFcShort( 0x8 ),	/* 8 */
/* 4700 */	0x46,		/* Oi2 Flags:  clt must size, has return, has ext, */
			0x4,		/* 4 */
/* 4702 */	0x8,		/* 8 */
			0x5,		/* Ext Flags:  new corr desc, srv corr check, */
/* 4704 */	NdrFcShort( 0x0 ),	/* 0 */
/* 4706 */	NdrFcShort( 0x13 ),	/* 19 */
/* 4708 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pformatetc */

/* 4710 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 4712 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 4714 */	NdrFcShort( 0x41a ),	/* Type Offset=1050 */

	/* Parameter pmedium */

/* 4716 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 4718 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 4720 */	NdrFcShort( 0x5b8 ),	/* Type Offset=1464 */

	/* Parameter fRelease */

/* 4722 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 4724 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 4726 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 4728 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 4730 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 4732 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure EnumFormatEtc */

/* 4734 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 4736 */	NdrFcLong( 0x0 ),	/* 0 */
/* 4740 */	NdrFcShort( 0x8 ),	/* 8 */
/* 4742 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 4744 */	NdrFcShort( 0x8 ),	/* 8 */
/* 4746 */	NdrFcShort( 0x8 ),	/* 8 */
/* 4748 */	0x45,		/* Oi2 Flags:  srv must size, has return, has ext, */
			0x3,		/* 3 */
/* 4750 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 4752 */	NdrFcShort( 0x0 ),	/* 0 */
/* 4754 */	NdrFcShort( 0x0 ),	/* 0 */
/* 4756 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter dwDirection */

/* 4758 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 4760 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 4762 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter ppenumFormatEtc */

/* 4764 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
/* 4766 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 4768 */	NdrFcShort( 0x3ae ),	/* Type Offset=942 */

	/* Return value */

/* 4770 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 4772 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 4774 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure DAdvise */

/* 4776 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 4778 */	NdrFcLong( 0x0 ),	/* 0 */
/* 4782 */	NdrFcShort( 0x9 ),	/* 9 */
/* 4784 */	NdrFcShort( 0x18 ),	/* x86 Stack size/offset = 24 */
/* 4786 */	NdrFcShort( 0x8 ),	/* 8 */
/* 4788 */	NdrFcShort( 0x24 ),	/* 36 */
/* 4790 */	0x46,		/* Oi2 Flags:  clt must size, has return, has ext, */
			0x5,		/* 5 */
/* 4792 */	0x8,		/* 8 */
			0x5,		/* Ext Flags:  new corr desc, srv corr check, */
/* 4794 */	NdrFcShort( 0x0 ),	/* 0 */
/* 4796 */	NdrFcShort( 0x2 ),	/* 2 */
/* 4798 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pformatetc */

/* 4800 */	NdrFcShort( 0x10b ),	/* Flags:  must size, must free, in, simple ref, */
/* 4802 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 4804 */	NdrFcShort( 0x382 ),	/* Type Offset=898 */

	/* Parameter advf */

/* 4806 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 4808 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 4810 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter pAdvSink */

/* 4812 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 4814 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 4816 */	NdrFcShort( 0x3c8 ),	/* Type Offset=968 */

	/* Parameter pdwConnection */

/* 4818 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 4820 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 4822 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 4824 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 4826 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 4828 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure DUnadvise */

/* 4830 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 4832 */	NdrFcLong( 0x0 ),	/* 0 */
/* 4836 */	NdrFcShort( 0xa ),	/* 10 */
/* 4838 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 4840 */	NdrFcShort( 0x8 ),	/* 8 */
/* 4842 */	NdrFcShort( 0x8 ),	/* 8 */
/* 4844 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x2,		/* 2 */
/* 4846 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 4848 */	NdrFcShort( 0x0 ),	/* 0 */
/* 4850 */	NdrFcShort( 0x0 ),	/* 0 */
/* 4852 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter dwConnection */

/* 4854 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 4856 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 4858 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 4860 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 4862 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 4864 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure EnumDAdvise */

/* 4866 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 4868 */	NdrFcLong( 0x0 ),	/* 0 */
/* 4872 */	NdrFcShort( 0xb ),	/* 11 */
/* 4874 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 4876 */	NdrFcShort( 0x0 ),	/* 0 */
/* 4878 */	NdrFcShort( 0x8 ),	/* 8 */
/* 4880 */	0x45,		/* Oi2 Flags:  srv must size, has return, has ext, */
			0x2,		/* 2 */
/* 4882 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 4884 */	NdrFcShort( 0x0 ),	/* 0 */
/* 4886 */	NdrFcShort( 0x0 ),	/* 0 */
/* 4888 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter ppenumAdvise */

/* 4890 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
/* 4892 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 4894 */	NdrFcShort( 0x404 ),	/* Type Offset=1028 */

	/* Return value */

/* 4896 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 4898 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 4900 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetClassObject */

/* 4902 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 4904 */	NdrFcLong( 0x0 ),	/* 0 */
/* 4908 */	NdrFcShort( 0x3 ),	/* 3 */
/* 4910 */	NdrFcShort( 0x1c ),	/* x86 Stack size/offset = 28 */
/* 4912 */	NdrFcShort( 0x98 ),	/* 152 */
/* 4914 */	NdrFcShort( 0x8 ),	/* 8 */
/* 4916 */	0x45,		/* Oi2 Flags:  srv must size, has return, has ext, */
			0x6,		/* 6 */
/* 4918 */	0x8,		/* 8 */
			0x3,		/* Ext Flags:  new corr desc, clt corr check, */
/* 4920 */	NdrFcShort( 0x1 ),	/* 1 */
/* 4922 */	NdrFcShort( 0x0 ),	/* 0 */
/* 4924 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter rclsid */

/* 4926 */	NdrFcShort( 0x10a ),	/* Flags:  must free, in, simple ref, */
/* 4928 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 4930 */	NdrFcShort( 0x17c ),	/* Type Offset=380 */

	/* Parameter dwClassContext */

/* 4932 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 4934 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 4936 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter locale */

/* 4938 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 4940 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 4942 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter riid */

/* 4944 */	NdrFcShort( 0x10a ),	/* Flags:  must free, in, simple ref, */
/* 4946 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 4948 */	NdrFcShort( 0x17c ),	/* Type Offset=380 */

	/* Parameter ppv */

/* 4950 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
/* 4952 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 4954 */	NdrFcShort( 0x5de ),	/* Type Offset=1502 */

	/* Return value */

/* 4956 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 4958 */	NdrFcShort( 0x18 ),	/* x86 Stack size/offset = 24 */
/* 4960 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure RemoteFillAppend */

/* 4962 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 4964 */	NdrFcLong( 0x0 ),	/* 0 */
/* 4968 */	NdrFcShort( 0x3 ),	/* 3 */
/* 4970 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 4972 */	NdrFcShort( 0x8 ),	/* 8 */
/* 4974 */	NdrFcShort( 0x24 ),	/* 36 */
/* 4976 */	0x46,		/* Oi2 Flags:  clt must size, has return, has ext, */
			0x4,		/* 4 */
/* 4978 */	0x8,		/* 8 */
			0x5,		/* Ext Flags:  new corr desc, srv corr check, */
/* 4980 */	NdrFcShort( 0x0 ),	/* 0 */
/* 4982 */	NdrFcShort( 0x1 ),	/* 1 */
/* 4984 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pv */

/* 4986 */	NdrFcShort( 0x10b ),	/* Flags:  must size, must free, in, simple ref, */
/* 4988 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 4990 */	NdrFcShort( 0x1f6 ),	/* Type Offset=502 */

	/* Parameter cb */

/* 4992 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 4994 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 4996 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter pcbWritten */

/* 4998 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 5000 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 5002 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 5004 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 5006 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 5008 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure SetFillSize */

/* 5010 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 5012 */	NdrFcLong( 0x0 ),	/* 0 */
/* 5016 */	NdrFcShort( 0x5 ),	/* 5 */
/* 5018 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 5020 */	NdrFcShort( 0x18 ),	/* 24 */
/* 5022 */	NdrFcShort( 0x8 ),	/* 8 */
/* 5024 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x2,		/* 2 */
/* 5026 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 5028 */	NdrFcShort( 0x0 ),	/* 0 */
/* 5030 */	NdrFcShort( 0x0 ),	/* 0 */
/* 5032 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter ulSize */

/* 5034 */	NdrFcShort( 0x8a ),	/* Flags:  must free, in, by val, */
/* 5036 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 5038 */	NdrFcShort( 0x1c0 ),	/* Type Offset=448 */

	/* Return value */

/* 5040 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 5042 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 5044 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure Terminate */

/* 5046 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 5048 */	NdrFcLong( 0x0 ),	/* 0 */
/* 5052 */	NdrFcShort( 0x6 ),	/* 6 */
/* 5054 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 5056 */	NdrFcShort( 0x8 ),	/* 8 */
/* 5058 */	NdrFcShort( 0x8 ),	/* 8 */
/* 5060 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x2,		/* 2 */
/* 5062 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 5064 */	NdrFcShort( 0x0 ),	/* 0 */
/* 5066 */	NdrFcShort( 0x0 ),	/* 0 */
/* 5068 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter bCanceled */

/* 5070 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 5072 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 5074 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 5076 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 5078 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 5080 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure OnProgress */

/* 5082 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 5084 */	NdrFcLong( 0x0 ),	/* 0 */
/* 5088 */	NdrFcShort( 0x3 ),	/* 3 */
/* 5090 */	NdrFcShort( 0x18 ),	/* x86 Stack size/offset = 24 */
/* 5092 */	NdrFcShort( 0x20 ),	/* 32 */
/* 5094 */	NdrFcShort( 0x8 ),	/* 8 */
/* 5096 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x5,		/* 5 */
/* 5098 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 5100 */	NdrFcShort( 0x0 ),	/* 0 */
/* 5102 */	NdrFcShort( 0x0 ),	/* 0 */
/* 5104 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter dwProgressCurrent */

/* 5106 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 5108 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 5110 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter dwProgressMaximum */

/* 5112 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 5114 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 5116 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter fAccurate */

/* 5118 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 5120 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 5122 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter fOwner */

/* 5124 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 5126 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 5128 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 5130 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 5132 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 5134 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure WaitForWriteAccess */


	/* Procedure Lock */

/* 5136 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 5138 */	NdrFcLong( 0x0 ),	/* 0 */
/* 5142 */	NdrFcShort( 0x3 ),	/* 3 */
/* 5144 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 5146 */	NdrFcShort( 0x8 ),	/* 8 */
/* 5148 */	NdrFcShort( 0x8 ),	/* 8 */
/* 5150 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x2,		/* 2 */
/* 5152 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 5154 */	NdrFcShort( 0x0 ),	/* 0 */
/* 5156 */	NdrFcShort( 0x0 ),	/* 0 */
/* 5158 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter dwTimeout */


	/* Parameter dwTimeout */

/* 5160 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 5162 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 5164 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */


	/* Return value */

/* 5166 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 5168 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 5170 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure Wait */


	/* Procedure SuppressChanges */

/* 5172 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 5174 */	NdrFcLong( 0x0 ),	/* 0 */
/* 5178 */	NdrFcShort( 0x3 ),	/* 3 */
/* 5180 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 5182 */	NdrFcShort( 0x10 ),	/* 16 */
/* 5184 */	NdrFcShort( 0x8 ),	/* 8 */
/* 5186 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x3,		/* 3 */
/* 5188 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 5190 */	NdrFcShort( 0x0 ),	/* 0 */
/* 5192 */	NdrFcShort( 0x0 ),	/* 0 */
/* 5194 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter dwFlags */


	/* Parameter res1 */

/* 5196 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 5198 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 5200 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter dwMilliseconds */


	/* Parameter res2 */

/* 5202 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 5204 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 5206 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */


	/* Return value */

/* 5208 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 5210 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 5212 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure CreateStorageEx */

/* 5214 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 5216 */	NdrFcLong( 0x0 ),	/* 0 */
/* 5220 */	NdrFcShort( 0x3 ),	/* 3 */
/* 5222 */	NdrFcShort( 0x20 ),	/* x86 Stack size/offset = 32 */
/* 5224 */	NdrFcShort( 0x5c ),	/* 92 */
/* 5226 */	NdrFcShort( 0x8 ),	/* 8 */
/* 5228 */	0x47,		/* Oi2 Flags:  srv must size, clt must size, has return, has ext, */
			0x7,		/* 7 */
/* 5230 */	0x8,		/* 8 */
			0x3,		/* Ext Flags:  new corr desc, clt corr check, */
/* 5232 */	NdrFcShort( 0x1 ),	/* 1 */
/* 5234 */	NdrFcShort( 0x0 ),	/* 0 */
/* 5236 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pwcsName */

/* 5238 */	NdrFcShort( 0x10b ),	/* Flags:  must size, must free, in, simple ref, */
/* 5240 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 5242 */	NdrFcShort( 0x114 ),	/* Type Offset=276 */

	/* Parameter grfMode */

/* 5244 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 5246 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 5248 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter stgfmt */

/* 5250 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 5252 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 5254 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter grfAttrs */

/* 5256 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 5258 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 5260 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter riid */

/* 5262 */	NdrFcShort( 0x10a ),	/* Flags:  must free, in, simple ref, */
/* 5264 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 5266 */	NdrFcShort( 0x17c ),	/* Type Offset=380 */

	/* Parameter ppstgOpen */

/* 5268 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
/* 5270 */	NdrFcShort( 0x18 ),	/* x86 Stack size/offset = 24 */
/* 5272 */	NdrFcShort( 0x5ea ),	/* Type Offset=1514 */

	/* Return value */

/* 5274 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 5276 */	NdrFcShort( 0x1c ),	/* x86 Stack size/offset = 28 */
/* 5278 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure OpenStorageEx */

/* 5280 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 5282 */	NdrFcLong( 0x0 ),	/* 0 */
/* 5286 */	NdrFcShort( 0x4 ),	/* 4 */
/* 5288 */	NdrFcShort( 0x20 ),	/* x86 Stack size/offset = 32 */
/* 5290 */	NdrFcShort( 0x5c ),	/* 92 */
/* 5292 */	NdrFcShort( 0x8 ),	/* 8 */
/* 5294 */	0x47,		/* Oi2 Flags:  srv must size, clt must size, has return, has ext, */
			0x7,		/* 7 */
/* 5296 */	0x8,		/* 8 */
			0x3,		/* Ext Flags:  new corr desc, clt corr check, */
/* 5298 */	NdrFcShort( 0x1 ),	/* 1 */
/* 5300 */	NdrFcShort( 0x0 ),	/* 0 */
/* 5302 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pwcsName */

/* 5304 */	NdrFcShort( 0x10b ),	/* Flags:  must size, must free, in, simple ref, */
/* 5306 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 5308 */	NdrFcShort( 0x114 ),	/* Type Offset=276 */

	/* Parameter grfMode */

/* 5310 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 5312 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 5314 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter stgfmt */

/* 5316 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 5318 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 5320 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter grfAttrs */

/* 5322 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 5324 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 5326 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter riid */

/* 5328 */	NdrFcShort( 0x10a ),	/* Flags:  must free, in, simple ref, */
/* 5330 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 5332 */	NdrFcShort( 0x17c ),	/* Type Offset=380 */

	/* Parameter ppstgOpen */

/* 5334 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
/* 5336 */	NdrFcShort( 0x18 ),	/* x86 Stack size/offset = 24 */
/* 5338 */	NdrFcShort( 0x5f6 ),	/* Type Offset=1526 */

	/* Return value */

/* 5340 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 5342 */	NdrFcShort( 0x1c ),	/* x86 Stack size/offset = 28 */
/* 5344 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure LoadDllServer */

/* 5346 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 5348 */	NdrFcLong( 0x0 ),	/* 0 */
/* 5352 */	NdrFcShort( 0x3 ),	/* 3 */
/* 5354 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 5356 */	NdrFcShort( 0x44 ),	/* 68 */
/* 5358 */	NdrFcShort( 0x8 ),	/* 8 */
/* 5360 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x2,		/* 2 */
/* 5362 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 5364 */	NdrFcShort( 0x0 ),	/* 0 */
/* 5366 */	NdrFcShort( 0x0 ),	/* 0 */
/* 5368 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter Clsid */

/* 5370 */	NdrFcShort( 0x10a ),	/* Flags:  must free, in, simple ref, */
/* 5372 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 5374 */	NdrFcShort( 0x17c ),	/* Type Offset=380 */

	/* Return value */

/* 5376 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 5378 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 5380 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure AsyncGetClassBits */

/* 5382 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 5384 */	NdrFcLong( 0x0 ),	/* 0 */
/* 5388 */	NdrFcShort( 0x3 ),	/* 3 */
/* 5390 */	NdrFcShort( 0x30 ),	/* x86 Stack size/offset = 48 */
/* 5392 */	NdrFcShort( 0xa8 ),	/* 168 */
/* 5394 */	NdrFcShort( 0x8 ),	/* 8 */
/* 5396 */	0x46,		/* Oi2 Flags:  clt must size, has return, has ext, */
			0xb,		/* 11 */
/* 5398 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 5400 */	NdrFcShort( 0x0 ),	/* 0 */
/* 5402 */	NdrFcShort( 0x0 ),	/* 0 */
/* 5404 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter rclsid */

/* 5406 */	NdrFcShort( 0x10a ),	/* Flags:  must free, in, simple ref, */
/* 5408 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 5410 */	NdrFcShort( 0x17c ),	/* Type Offset=380 */

	/* Parameter pszTYPE */

/* 5412 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 5414 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 5416 */	NdrFcShort( 0x2de ),	/* Type Offset=734 */

	/* Parameter pszExt */

/* 5418 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 5420 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 5422 */	NdrFcShort( 0x2de ),	/* Type Offset=734 */

	/* Parameter dwFileVersionMS */

/* 5424 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 5426 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 5428 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter dwFileVersionLS */

/* 5430 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 5432 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 5434 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter pszCodeBase */

/* 5436 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 5438 */	NdrFcShort( 0x18 ),	/* x86 Stack size/offset = 24 */
/* 5440 */	NdrFcShort( 0x2de ),	/* Type Offset=734 */

	/* Parameter pbc */

/* 5442 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 5444 */	NdrFcShort( 0x1c ),	/* x86 Stack size/offset = 28 */
/* 5446 */	NdrFcShort( 0x188 ),	/* Type Offset=392 */

	/* Parameter dwClassContext */

/* 5448 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 5450 */	NdrFcShort( 0x20 ),	/* x86 Stack size/offset = 32 */
/* 5452 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter riid */

/* 5454 */	NdrFcShort( 0x10a ),	/* Flags:  must free, in, simple ref, */
/* 5456 */	NdrFcShort( 0x24 ),	/* x86 Stack size/offset = 36 */
/* 5458 */	NdrFcShort( 0x17c ),	/* Type Offset=380 */

	/* Parameter flags */

/* 5460 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 5462 */	NdrFcShort( 0x28 ),	/* x86 Stack size/offset = 40 */
/* 5464 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 5466 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 5468 */	NdrFcShort( 0x2c ),	/* x86 Stack size/offset = 44 */
/* 5470 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure Push */

/* 5472 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 5474 */	NdrFcLong( 0x0 ),	/* 0 */
/* 5478 */	NdrFcShort( 0x4 ),	/* 4 */
/* 5480 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 5482 */	NdrFcShort( 0x8 ),	/* 8 */
/* 5484 */	NdrFcShort( 0x8 ),	/* 8 */
/* 5486 */	0x46,		/* Oi2 Flags:  clt must size, has return, has ext, */
			0x3,		/* 3 */
/* 5488 */	0x8,		/* 8 */
			0x5,		/* Ext Flags:  new corr desc, srv corr check, */
/* 5490 */	NdrFcShort( 0x0 ),	/* 0 */
/* 5492 */	NdrFcShort( 0x1 ),	/* 1 */
/* 5494 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter buf */

/* 5496 */	NdrFcShort( 0x10b ),	/* Flags:  must size, must free, in, simple ref, */
/* 5498 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 5500 */	NdrFcShort( 0x1f6 ),	/* Type Offset=502 */

	/* Parameter cSent */

/* 5502 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 5504 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 5506 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 5508 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 5510 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 5512 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure Begin_Pull */


	/* Procedure Begin_Pull */


	/* Procedure Begin_Pull */

/* 5514 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 5516 */	NdrFcLong( 0x0 ),	/* 0 */
/* 5520 */	NdrFcShort( 0x3 ),	/* 3 */
/* 5522 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 5524 */	NdrFcShort( 0x8 ),	/* 8 */
/* 5526 */	NdrFcShort( 0x8 ),	/* 8 */
/* 5528 */	0x64,		/* Oi2 Flags:  has return, has async uuid, has ext, */
			0x2,		/* 2 */
/* 5530 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 5532 */	NdrFcShort( 0x0 ),	/* 0 */
/* 5534 */	NdrFcShort( 0x0 ),	/* 0 */
/* 5536 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter cRequest */


	/* Parameter cRequest */


	/* Parameter cRequest */

/* 5538 */	NdrFcShort( 0x448 ),	/* Flags:  in, base type, split async, */
/* 5540 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 5542 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */


	/* Return value */


	/* Return value */

/* 5544 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 5546 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 5548 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure Finish_Pull */

/* 5550 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 5552 */	NdrFcLong( 0x0 ),	/* 0 */
/* 5556 */	NdrFcShort( 0x4 ),	/* 4 */
/* 5558 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 5560 */	NdrFcShort( 0x0 ),	/* 0 */
/* 5562 */	NdrFcShort( 0x24 ),	/* 36 */
/* 5564 */	0x65,		/* Oi2 Flags:  srv must size, has return, has async uuid, has ext, */
			0x3,		/* 3 */
/* 5566 */	0x8,		/* 8 */
			0x3,		/* Ext Flags:  new corr desc, clt corr check, */
/* 5568 */	NdrFcShort( 0x1 ),	/* 1 */
/* 5570 */	NdrFcShort( 0x0 ),	/* 0 */
/* 5572 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter buf */

/* 5574 */	NdrFcShort( 0x113 ),	/* Flags:  must size, must free, out, simple ref, */
/* 5576 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 5578 */	NdrFcShort( 0x606 ),	/* Type Offset=1542 */

	/* Parameter pcReturned */

/* 5580 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 5582 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 5584 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 5586 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 5588 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 5590 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure Begin_Push */

/* 5592 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 5594 */	NdrFcLong( 0x0 ),	/* 0 */
/* 5598 */	NdrFcShort( 0x5 ),	/* 5 */
/* 5600 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 5602 */	NdrFcShort( 0x8 ),	/* 8 */
/* 5604 */	NdrFcShort( 0x8 ),	/* 8 */
/* 5606 */	0x66,		/* Oi2 Flags:  clt must size, has return, has async uuid, has ext, */
			0x3,		/* 3 */
/* 5608 */	0x8,		/* 8 */
			0x5,		/* Ext Flags:  new corr desc, srv corr check, */
/* 5610 */	NdrFcShort( 0x0 ),	/* 0 */
/* 5612 */	NdrFcShort( 0x1 ),	/* 1 */
/* 5614 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter buf */

/* 5616 */	NdrFcShort( 0x10b ),	/* Flags:  must size, must free, in, simple ref, */
/* 5618 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 5620 */	NdrFcShort( 0x1f6 ),	/* Type Offset=502 */

	/* Parameter cSent */

/* 5622 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 5624 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 5626 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 5628 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 5630 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 5632 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure Pull */

/* 5634 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 5636 */	NdrFcLong( 0x0 ),	/* 0 */
/* 5640 */	NdrFcShort( 0x3 ),	/* 3 */
/* 5642 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 5644 */	NdrFcShort( 0x8 ),	/* 8 */
/* 5646 */	NdrFcShort( 0x24 ),	/* 36 */
/* 5648 */	0x45,		/* Oi2 Flags:  srv must size, has return, has ext, */
			0x4,		/* 4 */
/* 5650 */	0x8,		/* 8 */
			0x3,		/* Ext Flags:  new corr desc, clt corr check, */
/* 5652 */	NdrFcShort( 0x1 ),	/* 1 */
/* 5654 */	NdrFcShort( 0x0 ),	/* 0 */
/* 5656 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter buf */

/* 5658 */	NdrFcShort( 0x113 ),	/* Flags:  must size, must free, out, simple ref, */
/* 5660 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 5662 */	NdrFcShort( 0x61c ),	/* Type Offset=1564 */

	/* Parameter cRequest */

/* 5664 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 5666 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 5668 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter pcReturned */

/* 5670 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 5672 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 5674 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 5676 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 5678 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 5680 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure Push */

/* 5682 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 5684 */	NdrFcLong( 0x0 ),	/* 0 */
/* 5688 */	NdrFcShort( 0x4 ),	/* 4 */
/* 5690 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 5692 */	NdrFcShort( 0x8 ),	/* 8 */
/* 5694 */	NdrFcShort( 0x8 ),	/* 8 */
/* 5696 */	0x46,		/* Oi2 Flags:  clt must size, has return, has ext, */
			0x3,		/* 3 */
/* 5698 */	0x8,		/* 8 */
			0x5,		/* Ext Flags:  new corr desc, srv corr check, */
/* 5700 */	NdrFcShort( 0x0 ),	/* 0 */
/* 5702 */	NdrFcShort( 0x1 ),	/* 1 */
/* 5704 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter buf */

/* 5706 */	NdrFcShort( 0x10b ),	/* Flags:  must size, must free, in, simple ref, */
/* 5708 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 5710 */	NdrFcShort( 0x632 ),	/* Type Offset=1586 */

	/* Parameter cSent */

/* 5712 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 5714 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 5716 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 5718 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 5720 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 5722 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure Finish_Pull */

/* 5724 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 5726 */	NdrFcLong( 0x0 ),	/* 0 */
/* 5730 */	NdrFcShort( 0x4 ),	/* 4 */
/* 5732 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 5734 */	NdrFcShort( 0x0 ),	/* 0 */
/* 5736 */	NdrFcShort( 0x24 ),	/* 36 */
/* 5738 */	0x65,		/* Oi2 Flags:  srv must size, has return, has async uuid, has ext, */
			0x3,		/* 3 */
/* 5740 */	0x8,		/* 8 */
			0x3,		/* Ext Flags:  new corr desc, clt corr check, */
/* 5742 */	NdrFcShort( 0x1 ),	/* 1 */
/* 5744 */	NdrFcShort( 0x0 ),	/* 0 */
/* 5746 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter buf */

/* 5748 */	NdrFcShort( 0x113 ),	/* Flags:  must size, must free, out, simple ref, */
/* 5750 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 5752 */	NdrFcShort( 0x642 ),	/* Type Offset=1602 */

	/* Parameter pcReturned */

/* 5754 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 5756 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 5758 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 5760 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 5762 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 5764 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure Begin_Push */

/* 5766 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 5768 */	NdrFcLong( 0x0 ),	/* 0 */
/* 5772 */	NdrFcShort( 0x5 ),	/* 5 */
/* 5774 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 5776 */	NdrFcShort( 0x8 ),	/* 8 */
/* 5778 */	NdrFcShort( 0x8 ),	/* 8 */
/* 5780 */	0x66,		/* Oi2 Flags:  clt must size, has return, has async uuid, has ext, */
			0x3,		/* 3 */
/* 5782 */	0x8,		/* 8 */
			0x5,		/* Ext Flags:  new corr desc, srv corr check, */
/* 5784 */	NdrFcShort( 0x0 ),	/* 0 */
/* 5786 */	NdrFcShort( 0x1 ),	/* 1 */
/* 5788 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter buf */

/* 5790 */	NdrFcShort( 0x10b ),	/* Flags:  must size, must free, in, simple ref, */
/* 5792 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 5794 */	NdrFcShort( 0x632 ),	/* Type Offset=1586 */

	/* Parameter cSent */

/* 5796 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 5798 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 5800 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 5802 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 5804 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 5806 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure Pull */

/* 5808 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 5810 */	NdrFcLong( 0x0 ),	/* 0 */
/* 5814 */	NdrFcShort( 0x3 ),	/* 3 */
/* 5816 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 5818 */	NdrFcShort( 0x8 ),	/* 8 */
/* 5820 */	NdrFcShort( 0x24 ),	/* 36 */
/* 5822 */	0x45,		/* Oi2 Flags:  srv must size, has return, has ext, */
			0x4,		/* 4 */
/* 5824 */	0x8,		/* 8 */
			0x3,		/* Ext Flags:  new corr desc, clt corr check, */
/* 5826 */	NdrFcShort( 0x1 ),	/* 1 */
/* 5828 */	NdrFcShort( 0x0 ),	/* 0 */
/* 5830 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter buf */

/* 5832 */	NdrFcShort( 0x113 ),	/* Flags:  must size, must free, out, simple ref, */
/* 5834 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 5836 */	NdrFcShort( 0x658 ),	/* Type Offset=1624 */

	/* Parameter cRequest */

/* 5838 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 5840 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 5842 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter pcReturned */

/* 5844 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 5846 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 5848 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 5850 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 5852 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 5854 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure Push */

/* 5856 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 5858 */	NdrFcLong( 0x0 ),	/* 0 */
/* 5862 */	NdrFcShort( 0x4 ),	/* 4 */
/* 5864 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 5866 */	NdrFcShort( 0x8 ),	/* 8 */
/* 5868 */	NdrFcShort( 0x8 ),	/* 8 */
/* 5870 */	0x46,		/* Oi2 Flags:  clt must size, has return, has ext, */
			0x3,		/* 3 */
/* 5872 */	0x8,		/* 8 */
			0x5,		/* Ext Flags:  new corr desc, srv corr check, */
/* 5874 */	NdrFcShort( 0x0 ),	/* 0 */
/* 5876 */	NdrFcShort( 0x1 ),	/* 1 */
/* 5878 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter buf */

/* 5880 */	NdrFcShort( 0x10b ),	/* Flags:  must size, must free, in, simple ref, */
/* 5882 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 5884 */	NdrFcShort( 0x66e ),	/* Type Offset=1646 */

	/* Parameter cSent */

/* 5886 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 5888 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 5890 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 5892 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 5894 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 5896 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure Finish_Pull */

/* 5898 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 5900 */	NdrFcLong( 0x0 ),	/* 0 */
/* 5904 */	NdrFcShort( 0x4 ),	/* 4 */
/* 5906 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 5908 */	NdrFcShort( 0x0 ),	/* 0 */
/* 5910 */	NdrFcShort( 0x24 ),	/* 36 */
/* 5912 */	0x65,		/* Oi2 Flags:  srv must size, has return, has async uuid, has ext, */
			0x3,		/* 3 */
/* 5914 */	0x8,		/* 8 */
			0x3,		/* Ext Flags:  new corr desc, clt corr check, */
/* 5916 */	NdrFcShort( 0x1 ),	/* 1 */
/* 5918 */	NdrFcShort( 0x0 ),	/* 0 */
/* 5920 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter buf */

/* 5922 */	NdrFcShort( 0x113 ),	/* Flags:  must size, must free, out, simple ref, */
/* 5924 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 5926 */	NdrFcShort( 0x67e ),	/* Type Offset=1662 */

	/* Parameter pcReturned */

/* 5928 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 5930 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 5932 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 5934 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 5936 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 5938 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure Begin_Push */

/* 5940 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 5942 */	NdrFcLong( 0x0 ),	/* 0 */
/* 5946 */	NdrFcShort( 0x5 ),	/* 5 */
/* 5948 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 5950 */	NdrFcShort( 0x8 ),	/* 8 */
/* 5952 */	NdrFcShort( 0x8 ),	/* 8 */
/* 5954 */	0x66,		/* Oi2 Flags:  clt must size, has return, has async uuid, has ext, */
			0x3,		/* 3 */
/* 5956 */	0x8,		/* 8 */
			0x5,		/* Ext Flags:  new corr desc, srv corr check, */
/* 5958 */	NdrFcShort( 0x0 ),	/* 0 */
/* 5960 */	NdrFcShort( 0x1 ),	/* 1 */
/* 5962 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter buf */

/* 5964 */	NdrFcShort( 0x10b ),	/* Flags:  must size, must free, in, simple ref, */
/* 5966 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 5968 */	NdrFcShort( 0x66e ),	/* Type Offset=1646 */

	/* Parameter cSent */

/* 5970 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 5972 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 5974 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 5976 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 5978 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 5980 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure ExtractThumbnail */

/* 5982 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 5984 */	NdrFcLong( 0x0 ),	/* 0 */
/* 5988 */	NdrFcShort( 0x3 ),	/* 3 */
/* 5990 */	NdrFcShort( 0x20 ),	/* x86 Stack size/offset = 32 */
/* 5992 */	NdrFcShort( 0x10 ),	/* 16 */
/* 5994 */	NdrFcShort( 0x40 ),	/* 64 */
/* 5996 */	0x47,		/* Oi2 Flags:  srv must size, clt must size, has return, has ext, */
			0x7,		/* 7 */
/* 5998 */	0x8,		/* 8 */
			0x3,		/* Ext Flags:  new corr desc, clt corr check, */
/* 6000 */	NdrFcShort( 0x2 ),	/* 2 */
/* 6002 */	NdrFcShort( 0x0 ),	/* 0 */
/* 6004 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pStg */

/* 6006 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 6008 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 6010 */	NdrFcShort( 0x2cc ),	/* Type Offset=716 */

	/* Parameter ulLength */

/* 6012 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 6014 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 6016 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter ulHeight */

/* 6018 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 6020 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 6022 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter pulOutputLength */

/* 6024 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 6026 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 6028 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter pulOutputHeight */

/* 6030 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 6032 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 6034 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter phOutputBitmap */

/* 6036 */	NdrFcShort( 0x2113 ),	/* Flags:  must size, must free, out, simple ref, srv alloc size=8 */
/* 6038 */	NdrFcShort( 0x18 ),	/* x86 Stack size/offset = 24 */
/* 6040 */	NdrFcShort( 0x698 ),	/* Type Offset=1688 */

	/* Return value */

/* 6042 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 6044 */	NdrFcShort( 0x1c ),	/* x86 Stack size/offset = 28 */
/* 6046 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure OnFileUpdated */

/* 6048 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 6050 */	NdrFcLong( 0x0 ),	/* 0 */
/* 6054 */	NdrFcShort( 0x4 ),	/* 4 */
/* 6056 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 6058 */	NdrFcShort( 0x0 ),	/* 0 */
/* 6060 */	NdrFcShort( 0x8 ),	/* 8 */
/* 6062 */	0x46,		/* Oi2 Flags:  clt must size, has return, has ext, */
			0x2,		/* 2 */
/* 6064 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 6066 */	NdrFcShort( 0x0 ),	/* 0 */
/* 6068 */	NdrFcShort( 0x0 ),	/* 0 */
/* 6070 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pStg */

/* 6072 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 6074 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 6076 */	NdrFcShort( 0x2cc ),	/* Type Offset=716 */

	/* Return value */

/* 6078 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 6080 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 6082 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure Dummy */

/* 6084 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 6086 */	NdrFcLong( 0x0 ),	/* 0 */
/* 6090 */	NdrFcShort( 0x3 ),	/* 3 */
/* 6092 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 6094 */	NdrFcShort( 0x0 ),	/* 0 */
/* 6096 */	NdrFcShort( 0x8 ),	/* 8 */
/* 6098 */	0x46,		/* Oi2 Flags:  clt must size, has return, has ext, */
			0x3,		/* 3 */
/* 6100 */	0x8,		/* 8 */
			0x5,		/* Ext Flags:  new corr desc, srv corr check, */
/* 6102 */	NdrFcShort( 0x0 ),	/* 0 */
/* 6104 */	NdrFcShort( 0x2 ),	/* 2 */
/* 6106 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter h1 */

/* 6108 */	NdrFcShort( 0x8b ),	/* Flags:  must size, must free, in, by val, */
/* 6110 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 6112 */	NdrFcShort( 0x6ba ),	/* Type Offset=1722 */

	/* Parameter h2 */

/* 6114 */	NdrFcShort( 0x8b ),	/* Flags:  must size, must free, in, by val, */
/* 6116 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 6118 */	NdrFcShort( 0x6c4 ),	/* Type Offset=1732 */

	/* Return value */

/* 6120 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 6122 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 6124 */	0x8,		/* FC_LONG */
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
			0x11, 0x0,	/* FC_RP */
/*  4 */	NdrFcShort( 0x14 ),	/* Offset= 20 (24) */
/*  6 */	
			0x2f,		/* FC_IP */
			0x5a,		/* FC_CONSTANT_IID */
/*  8 */	NdrFcLong( 0x0 ),	/* 0 */
/* 12 */	NdrFcShort( 0x0 ),	/* 0 */
/* 14 */	NdrFcShort( 0x0 ),	/* 0 */
/* 16 */	0xc0,		/* 192 */
			0x0,		/* 0 */
/* 18 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 20 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 22 */	0x0,		/* 0 */
			0x46,		/* 70 */
/* 24 */	
			0x21,		/* FC_BOGUS_ARRAY */
			0x3,		/* 3 */
/* 26 */	NdrFcShort( 0x0 ),	/* 0 */
/* 28 */	0x29,		/* Corr desc:  parameter, FC_ULONG */
			0x0,		/*  */
/* 30 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 32 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 34 */	0x29,		/* Corr desc:  parameter, FC_ULONG */
			0x54,		/* FC_DEREFERENCE */
/* 36 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 38 */	NdrFcShort( 0x0 ),	/* Corr flags:  */
/* 40 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 42 */	NdrFcShort( 0xffffffdc ),	/* Offset= -36 (6) */
/* 44 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 46 */	
			0x11, 0xc,	/* FC_RP [alloced_on_stack] [simple_pointer] */
/* 48 */	0x8,		/* FC_LONG */
			0x5c,		/* FC_PAD */
/* 50 */	
			0x11, 0x10,	/* FC_RP [pointer_deref] */
/* 52 */	NdrFcShort( 0x2 ),	/* Offset= 2 (54) */
/* 54 */	
			0x2f,		/* FC_IP */
			0x5a,		/* FC_CONSTANT_IID */
/* 56 */	NdrFcLong( 0x100 ),	/* 256 */
/* 60 */	NdrFcShort( 0x0 ),	/* 0 */
/* 62 */	NdrFcShort( 0x0 ),	/* 0 */
/* 64 */	0xc0,		/* 192 */
			0x0,		/* 0 */
/* 66 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 68 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 70 */	0x0,		/* 0 */
			0x46,		/* 70 */
/* 72 */	
			0x11, 0x0,	/* FC_RP */
/* 74 */	NdrFcShort( 0x98 ),	/* Offset= 152 (226) */
/* 76 */	
			0x1b,		/* FC_CARRAY */
			0x1,		/* 1 */
/* 78 */	NdrFcShort( 0x2 ),	/* 2 */
/* 80 */	0x19,		/* Corr desc:  field pointer, FC_ULONG */
			0x57,		/* FC_ADD_1 */
/* 82 */	NdrFcShort( 0x4 ),	/* 4 */
/* 84 */	NdrFcShort( 0x0 ),	/* Corr flags:  */
/* 86 */	0x6,		/* FC_SHORT */
			0x5b,		/* FC_END */
/* 88 */	
			0x1b,		/* FC_CARRAY */
			0x1,		/* 1 */
/* 90 */	NdrFcShort( 0x2 ),	/* 2 */
/* 92 */	0x19,		/* Corr desc:  field pointer, FC_ULONG */
			0x57,		/* FC_ADD_1 */
/* 94 */	NdrFcShort( 0xc ),	/* 12 */
/* 96 */	NdrFcShort( 0x0 ),	/* Corr flags:  */
/* 98 */	0x6,		/* FC_SHORT */
			0x5b,		/* FC_END */
/* 100 */	
			0x1b,		/* FC_CARRAY */
			0x1,		/* 1 */
/* 102 */	NdrFcShort( 0x2 ),	/* 2 */
/* 104 */	0x19,		/* Corr desc:  field pointer, FC_ULONG */
			0x57,		/* FC_ADD_1 */
/* 106 */	NdrFcShort( 0x14 ),	/* 20 */
/* 108 */	NdrFcShort( 0x0 ),	/* Corr flags:  */
/* 110 */	0x6,		/* FC_SHORT */
			0x5b,		/* FC_END */
/* 112 */	
			0x16,		/* FC_PSTRUCT */
			0x3,		/* 3 */
/* 114 */	NdrFcShort( 0x1c ),	/* 28 */
/* 116 */	
			0x4b,		/* FC_PP */
			0x5c,		/* FC_PAD */
/* 118 */	
			0x46,		/* FC_NO_REPEAT */
			0x5c,		/* FC_PAD */
/* 120 */	NdrFcShort( 0x0 ),	/* 0 */
/* 122 */	NdrFcShort( 0x0 ),	/* 0 */
/* 124 */	0x12, 0x0,	/* FC_UP */
/* 126 */	NdrFcShort( 0xffffffce ),	/* Offset= -50 (76) */
/* 128 */	
			0x46,		/* FC_NO_REPEAT */
			0x5c,		/* FC_PAD */
/* 130 */	NdrFcShort( 0x8 ),	/* 8 */
/* 132 */	NdrFcShort( 0x8 ),	/* 8 */
/* 134 */	0x12, 0x0,	/* FC_UP */
/* 136 */	NdrFcShort( 0xffffffd0 ),	/* Offset= -48 (88) */
/* 138 */	
			0x46,		/* FC_NO_REPEAT */
			0x5c,		/* FC_PAD */
/* 140 */	NdrFcShort( 0x10 ),	/* 16 */
/* 142 */	NdrFcShort( 0x10 ),	/* 16 */
/* 144 */	0x12, 0x0,	/* FC_UP */
/* 146 */	NdrFcShort( 0xffffffd2 ),	/* Offset= -46 (100) */
/* 148 */	
			0x5b,		/* FC_END */

			0x8,		/* FC_LONG */
/* 150 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 152 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 154 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 156 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 158 */	
			0x16,		/* FC_PSTRUCT */
			0x3,		/* 3 */
/* 160 */	NdrFcShort( 0x1c ),	/* 28 */
/* 162 */	
			0x4b,		/* FC_PP */
			0x5c,		/* FC_PAD */
/* 164 */	
			0x46,		/* FC_NO_REPEAT */
			0x5c,		/* FC_PAD */
/* 166 */	NdrFcShort( 0x8 ),	/* 8 */
/* 168 */	NdrFcShort( 0x8 ),	/* 8 */
/* 170 */	0x12, 0x8,	/* FC_UP [simple_pointer] */
/* 172 */	
			0x25,		/* FC_C_WSTRING */
			0x5c,		/* FC_PAD */
/* 174 */	
			0x46,		/* FC_NO_REPEAT */
			0x5c,		/* FC_PAD */
/* 176 */	NdrFcShort( 0x14 ),	/* 20 */
/* 178 */	NdrFcShort( 0x14 ),	/* 20 */
/* 180 */	0x12, 0x0,	/* FC_UP */
/* 182 */	NdrFcShort( 0xffffffba ),	/* Offset= -70 (112) */
/* 184 */	
			0x5b,		/* FC_END */

			0x8,		/* FC_LONG */
/* 186 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 188 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 190 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 192 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 194 */	
			0x16,		/* FC_PSTRUCT */
			0x3,		/* 3 */
/* 196 */	NdrFcShort( 0x10 ),	/* 16 */
/* 198 */	
			0x4b,		/* FC_PP */
			0x5c,		/* FC_PAD */
/* 200 */	
			0x46,		/* FC_NO_REPEAT */
			0x5c,		/* FC_PAD */
/* 202 */	NdrFcShort( 0x4 ),	/* 4 */
/* 204 */	NdrFcShort( 0x4 ),	/* 4 */
/* 206 */	0x12, 0x8,	/* FC_UP [simple_pointer] */
/* 208 */	
			0x25,		/* FC_C_WSTRING */
			0x5c,		/* FC_PAD */
/* 210 */	
			0x46,		/* FC_NO_REPEAT */
			0x5c,		/* FC_PAD */
/* 212 */	NdrFcShort( 0x8 ),	/* 8 */
/* 214 */	NdrFcShort( 0x8 ),	/* 8 */
/* 216 */	0x12, 0x0,	/* FC_UP */
/* 218 */	NdrFcShort( 0xffffffc4 ),	/* Offset= -60 (158) */
/* 220 */	
			0x5b,		/* FC_END */

			0x8,		/* FC_LONG */
/* 222 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 224 */	0x8,		/* FC_LONG */
			0x5b,		/* FC_END */
/* 226 */	
			0x16,		/* FC_PSTRUCT */
			0x3,		/* 3 */
/* 228 */	NdrFcShort( 0x20 ),	/* 32 */
/* 230 */	
			0x4b,		/* FC_PP */
			0x5c,		/* FC_PAD */
/* 232 */	
			0x46,		/* FC_NO_REPEAT */
			0x5c,		/* FC_PAD */
/* 234 */	NdrFcShort( 0x1c ),	/* 28 */
/* 236 */	NdrFcShort( 0x1c ),	/* 28 */
/* 238 */	0x12, 0x0,	/* FC_UP */
/* 240 */	NdrFcShort( 0xffffffd2 ),	/* Offset= -46 (194) */
/* 242 */	
			0x5b,		/* FC_END */

			0x8,		/* FC_LONG */
/* 244 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 246 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 248 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 250 */	0x8,		/* FC_LONG */
			0x5b,		/* FC_END */
/* 252 */	
			0x11, 0x10,	/* FC_RP [pointer_deref] */
/* 254 */	NdrFcShort( 0x2 ),	/* Offset= 2 (256) */
/* 256 */	
			0x2f,		/* FC_IP */
			0x5a,		/* FC_CONSTANT_IID */
/* 258 */	NdrFcLong( 0x10 ),	/* 16 */
/* 262 */	NdrFcShort( 0x0 ),	/* 0 */
/* 264 */	NdrFcShort( 0x0 ),	/* 0 */
/* 266 */	0xc0,		/* 192 */
			0x0,		/* 0 */
/* 268 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 270 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 272 */	0x0,		/* 0 */
			0x46,		/* 70 */
/* 274 */	
			0x11, 0x8,	/* FC_RP [simple_pointer] */
/* 276 */	
			0x25,		/* FC_C_WSTRING */
			0x5c,		/* FC_PAD */
/* 278 */	
			0x11, 0x10,	/* FC_RP [pointer_deref] */
/* 280 */	NdrFcShort( 0xfffffeee ),	/* Offset= -274 (6) */
/* 282 */	
			0x11, 0x10,	/* FC_RP [pointer_deref] */
/* 284 */	NdrFcShort( 0x2 ),	/* Offset= 2 (286) */
/* 286 */	
			0x2f,		/* FC_IP */
			0x5a,		/* FC_CONSTANT_IID */
/* 288 */	NdrFcLong( 0x101 ),	/* 257 */
/* 292 */	NdrFcShort( 0x0 ),	/* 0 */
/* 294 */	NdrFcShort( 0x0 ),	/* 0 */
/* 296 */	0xc0,		/* 192 */
			0x0,		/* 0 */
/* 298 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 300 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 302 */	0x0,		/* 0 */
			0x46,		/* 70 */
/* 304 */	
			0x11, 0x0,	/* FC_RP */
/* 306 */	NdrFcShort( 0x14 ),	/* Offset= 20 (326) */
/* 308 */	
			0x2f,		/* FC_IP */
			0x5a,		/* FC_CONSTANT_IID */
/* 310 */	NdrFcLong( 0xf ),	/* 15 */
/* 314 */	NdrFcShort( 0x0 ),	/* 0 */
/* 316 */	NdrFcShort( 0x0 ),	/* 0 */
/* 318 */	0xc0,		/* 192 */
			0x0,		/* 0 */
/* 320 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 322 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 324 */	0x0,		/* 0 */
			0x46,		/* 70 */
/* 326 */	
			0x21,		/* FC_BOGUS_ARRAY */
			0x3,		/* 3 */
/* 328 */	NdrFcShort( 0x0 ),	/* 0 */
/* 330 */	0x29,		/* Corr desc:  parameter, FC_ULONG */
			0x0,		/*  */
/* 332 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 334 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 336 */	0x29,		/* Corr desc:  parameter, FC_ULONG */
			0x54,		/* FC_DEREFERENCE */
/* 338 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 340 */	NdrFcShort( 0x0 ),	/* Corr flags:  */
/* 342 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 344 */	NdrFcShort( 0xffffffdc ),	/* Offset= -36 (308) */
/* 346 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 348 */	
			0x11, 0x10,	/* FC_RP [pointer_deref] */
/* 350 */	NdrFcShort( 0x2 ),	/* Offset= 2 (352) */
/* 352 */	
			0x2f,		/* FC_IP */
			0x5a,		/* FC_CONSTANT_IID */
/* 354 */	NdrFcLong( 0x102 ),	/* 258 */
/* 358 */	NdrFcShort( 0x0 ),	/* 0 */
/* 360 */	NdrFcShort( 0x0 ),	/* 0 */
/* 362 */	0xc0,		/* 192 */
			0x0,		/* 0 */
/* 364 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 366 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 368 */	0x0,		/* 0 */
			0x46,		/* 70 */
/* 370 */	
			0x11, 0x4,	/* FC_RP [alloced_on_stack] */
/* 372 */	NdrFcShort( 0x8 ),	/* Offset= 8 (380) */
/* 374 */	
			0x1d,		/* FC_SMFARRAY */
			0x0,		/* 0 */
/* 376 */	NdrFcShort( 0x8 ),	/* 8 */
/* 378 */	0x2,		/* FC_CHAR */
			0x5b,		/* FC_END */
/* 380 */	
			0x15,		/* FC_STRUCT */
			0x3,		/* 3 */
/* 382 */	NdrFcShort( 0x10 ),	/* 16 */
/* 384 */	0x8,		/* FC_LONG */
			0x6,		/* FC_SHORT */
/* 386 */	0x6,		/* FC_SHORT */
			0x4c,		/* FC_EMBEDDED_COMPLEX */
/* 388 */	0x0,		/* 0 */
			NdrFcShort( 0xfffffff1 ),	/* Offset= -15 (374) */
			0x5b,		/* FC_END */
/* 392 */	
			0x2f,		/* FC_IP */
			0x5a,		/* FC_CONSTANT_IID */
/* 394 */	NdrFcLong( 0xe ),	/* 14 */
/* 398 */	NdrFcShort( 0x0 ),	/* 0 */
/* 400 */	NdrFcShort( 0x0 ),	/* 0 */
/* 402 */	0xc0,		/* 192 */
			0x0,		/* 0 */
/* 404 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 406 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 408 */	0x0,		/* 0 */
			0x46,		/* 70 */
/* 410 */	
			0x11, 0x0,	/* FC_RP */
/* 412 */	NdrFcShort( 0x2 ),	/* Offset= 2 (414) */
/* 414 */	
			0x15,		/* FC_STRUCT */
			0x3,		/* 3 */
/* 416 */	NdrFcShort( 0x8 ),	/* 8 */
/* 418 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 420 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 422 */	
			0x11, 0x4,	/* FC_RP [alloced_on_stack] */
/* 424 */	NdrFcShort( 0xfffffff6 ),	/* Offset= -10 (414) */
/* 426 */	
			0x2f,		/* FC_IP */
			0x5a,		/* FC_CONSTANT_IID */
/* 428 */	NdrFcLong( 0xc ),	/* 12 */
/* 432 */	NdrFcShort( 0x0 ),	/* 0 */
/* 434 */	NdrFcShort( 0x0 ),	/* 0 */
/* 436 */	0xc0,		/* 192 */
			0x0,		/* 0 */
/* 438 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 440 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 442 */	0x0,		/* 0 */
			0x46,		/* 70 */
/* 444 */	
			0x11, 0x4,	/* FC_RP [alloced_on_stack] */
/* 446 */	NdrFcShort( 0x2 ),	/* Offset= 2 (448) */
/* 448 */	
			0x15,		/* FC_STRUCT */
			0x7,		/* 7 */
/* 450 */	NdrFcShort( 0x8 ),	/* 8 */
/* 452 */	0xb,		/* FC_HYPER */
			0x5b,		/* FC_END */
/* 454 */	
			0x11, 0x0,	/* FC_RP */
/* 456 */	NdrFcShort( 0xffffffb4 ),	/* Offset= -76 (380) */
/* 458 */	
			0x11, 0x10,	/* FC_RP [pointer_deref] */
/* 460 */	NdrFcShort( 0x2 ),	/* Offset= 2 (462) */
/* 462 */	
			0x2f,		/* FC_IP */
			0x5c,		/* FC_PAD */
/* 464 */	0x28,		/* Corr desc:  parameter, FC_LONG */
			0x0,		/*  */
/* 466 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 468 */	NdrFcShort( 0x5 ),	/* Corr flags:  early, iid_is, */
/* 470 */	
			0x11, 0x10,	/* FC_RP [pointer_deref] */
/* 472 */	NdrFcShort( 0x2 ),	/* Offset= 2 (474) */
/* 474 */	
			0x2f,		/* FC_IP */
			0x5c,		/* FC_PAD */
/* 476 */	0x28,		/* Corr desc:  parameter, FC_LONG */
			0x0,		/*  */
/* 478 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 480 */	NdrFcShort( 0x5 ),	/* Corr flags:  early, iid_is, */
/* 482 */	
			0x12, 0x14,	/* FC_UP [alloced_on_stack] [pointer_deref] */
/* 484 */	NdrFcShort( 0xffffff50 ),	/* Offset= -176 (308) */
/* 486 */	
			0x11, 0x10,	/* FC_RP [pointer_deref] */
/* 488 */	NdrFcShort( 0xffffff4c ),	/* Offset= -180 (308) */
/* 490 */	
			0x11, 0x14,	/* FC_RP [alloced_on_stack] [pointer_deref] */
/* 492 */	NdrFcShort( 0x2 ),	/* Offset= 2 (494) */
/* 494 */	
			0x13, 0x8,	/* FC_OP [simple_pointer] */
/* 496 */	
			0x25,		/* FC_C_WSTRING */
			0x5c,		/* FC_PAD */
/* 498 */	
			0x11, 0x0,	/* FC_RP */
/* 500 */	NdrFcShort( 0x2 ),	/* Offset= 2 (502) */
/* 502 */	
			0x1b,		/* FC_CARRAY */
			0x0,		/* 0 */
/* 504 */	NdrFcShort( 0x1 ),	/* 1 */
/* 506 */	0x29,		/* Corr desc:  parameter, FC_ULONG */
			0x0,		/*  */
/* 508 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 510 */	NdrFcShort( 0x0 ),	/* Corr flags:  */
/* 512 */	0x1,		/* FC_BYTE */
			0x5b,		/* FC_END */
/* 514 */	
			0x11, 0x0,	/* FC_RP */
/* 516 */	NdrFcShort( 0x2 ),	/* Offset= 2 (518) */
/* 518 */	
			0x1c,		/* FC_CVARRAY */
			0x3,		/* 3 */
/* 520 */	NdrFcShort( 0x4 ),	/* 4 */
/* 522 */	0x29,		/* Corr desc:  parameter, FC_ULONG */
			0x0,		/*  */
/* 524 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 526 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 528 */	0x29,		/* Corr desc:  parameter, FC_ULONG */
			0x54,		/* FC_DEREFERENCE */
/* 530 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 532 */	NdrFcShort( 0x0 ),	/* Corr flags:  */
/* 534 */	
			0x4b,		/* FC_PP */
			0x5c,		/* FC_PAD */
/* 536 */	
			0x48,		/* FC_VARIABLE_REPEAT */
			0x4a,		/* FC_VARIABLE_OFFSET */
/* 538 */	NdrFcShort( 0x4 ),	/* 4 */
/* 540 */	NdrFcShort( 0x0 ),	/* 0 */
/* 542 */	NdrFcShort( 0x1 ),	/* 1 */
/* 544 */	NdrFcShort( 0x0 ),	/* 0 */
/* 546 */	NdrFcShort( 0x0 ),	/* 0 */
/* 548 */	0x13, 0x8,	/* FC_OP [simple_pointer] */
/* 550 */	
			0x25,		/* FC_C_WSTRING */
			0x5c,		/* FC_PAD */
/* 552 */	
			0x5b,		/* FC_END */

			0x8,		/* FC_LONG */
/* 554 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 556 */	
			0x11, 0x0,	/* FC_RP */
/* 558 */	NdrFcShort( 0x2 ),	/* Offset= 2 (560) */
/* 560 */	
			0x1c,		/* FC_CVARRAY */
			0x0,		/* 0 */
/* 562 */	NdrFcShort( 0x1 ),	/* 1 */
/* 564 */	0x29,		/* Corr desc:  parameter, FC_ULONG */
			0x0,		/*  */
/* 566 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 568 */	NdrFcShort( 0x0 ),	/* Corr flags:  */
/* 570 */	0x29,		/* Corr desc:  parameter, FC_ULONG */
			0x54,		/* FC_DEREFERENCE */
/* 572 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 574 */	NdrFcShort( 0x0 ),	/* Corr flags:  */
/* 576 */	0x1,		/* FC_BYTE */
			0x5b,		/* FC_END */
/* 578 */	
			0x11, 0x0,	/* FC_RP */
/* 580 */	NdrFcShort( 0x2 ),	/* Offset= 2 (582) */
/* 582 */	
			0x16,		/* FC_PSTRUCT */
			0x7,		/* 7 */
/* 584 */	NdrFcShort( 0x48 ),	/* 72 */
/* 586 */	
			0x4b,		/* FC_PP */
			0x5c,		/* FC_PAD */
/* 588 */	
			0x46,		/* FC_NO_REPEAT */
			0x5c,		/* FC_PAD */
/* 590 */	NdrFcShort( 0x0 ),	/* 0 */
/* 592 */	NdrFcShort( 0x0 ),	/* 0 */
/* 594 */	0x13, 0x8,	/* FC_OP [simple_pointer] */
/* 596 */	
			0x25,		/* FC_C_WSTRING */
			0x5c,		/* FC_PAD */
/* 598 */	
			0x5b,		/* FC_END */

			0x8,		/* FC_LONG */
/* 600 */	0x8,		/* FC_LONG */
			0x4c,		/* FC_EMBEDDED_COMPLEX */
/* 602 */	0x0,		/* 0 */
			NdrFcShort( 0xffffff65 ),	/* Offset= -155 (448) */
			0x4c,		/* FC_EMBEDDED_COMPLEX */
/* 606 */	0x0,		/* 0 */
			NdrFcShort( 0xffffff3f ),	/* Offset= -193 (414) */
			0x4c,		/* FC_EMBEDDED_COMPLEX */
/* 610 */	0x0,		/* 0 */
			NdrFcShort( 0xffffff3b ),	/* Offset= -197 (414) */
			0x4c,		/* FC_EMBEDDED_COMPLEX */
/* 614 */	0x0,		/* 0 */
			NdrFcShort( 0xffffff37 ),	/* Offset= -201 (414) */
			0x8,		/* FC_LONG */
/* 618 */	0x8,		/* FC_LONG */
			0x4c,		/* FC_EMBEDDED_COMPLEX */
/* 620 */	0x0,		/* 0 */
			NdrFcShort( 0xffffff0f ),	/* Offset= -241 (380) */
			0x8,		/* FC_LONG */
/* 624 */	0x8,		/* FC_LONG */
			0x5b,		/* FC_END */
/* 626 */	
			0x11, 0x10,	/* FC_RP [pointer_deref] */
/* 628 */	NdrFcShort( 0xffffff36 ),	/* Offset= -202 (426) */
/* 630 */	
			0x11, 0x0,	/* FC_RP */
/* 632 */	NdrFcShort( 0x2 ),	/* Offset= 2 (634) */
/* 634 */	
			0x1c,		/* FC_CVARRAY */
			0x7,		/* 7 */
/* 636 */	NdrFcShort( 0x48 ),	/* 72 */
/* 638 */	0x29,		/* Corr desc:  parameter, FC_ULONG */
			0x0,		/*  */
/* 640 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 642 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 644 */	0x29,		/* Corr desc:  parameter, FC_ULONG */
			0x54,		/* FC_DEREFERENCE */
/* 646 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 648 */	NdrFcShort( 0x0 ),	/* Corr flags:  */
/* 650 */	
			0x4b,		/* FC_PP */
			0x5c,		/* FC_PAD */
/* 652 */	
			0x48,		/* FC_VARIABLE_REPEAT */
			0x4a,		/* FC_VARIABLE_OFFSET */
/* 654 */	NdrFcShort( 0x48 ),	/* 72 */
/* 656 */	NdrFcShort( 0x0 ),	/* 0 */
/* 658 */	NdrFcShort( 0x1 ),	/* 1 */
/* 660 */	NdrFcShort( 0x0 ),	/* 0 */
/* 662 */	NdrFcShort( 0x0 ),	/* 0 */
/* 664 */	0x13, 0x8,	/* FC_OP [simple_pointer] */
/* 666 */	
			0x25,		/* FC_C_WSTRING */
			0x5c,		/* FC_PAD */
/* 668 */	
			0x5b,		/* FC_END */

			0x4c,		/* FC_EMBEDDED_COMPLEX */
/* 670 */	0x0,		/* 0 */
			NdrFcShort( 0xffffffa7 ),	/* Offset= -89 (582) */
			0x5b,		/* FC_END */
/* 674 */	
			0x11, 0x10,	/* FC_RP [pointer_deref] */
/* 676 */	NdrFcShort( 0x2 ),	/* Offset= 2 (678) */
/* 678 */	
			0x2f,		/* FC_IP */
			0x5a,		/* FC_CONSTANT_IID */
/* 680 */	NdrFcLong( 0xd ),	/* 13 */
/* 684 */	NdrFcShort( 0x0 ),	/* 0 */
/* 686 */	NdrFcShort( 0x0 ),	/* 0 */
/* 688 */	0xc0,		/* 192 */
			0x0,		/* 0 */
/* 690 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 692 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 694 */	0x0,		/* 0 */
			0x46,		/* 70 */
/* 696 */	
			0x12, 0x0,	/* FC_UP */
/* 698 */	NdrFcShort( 0x2 ),	/* Offset= 2 (700) */
/* 700 */	
			0x1b,		/* FC_CARRAY */
			0x0,		/* 0 */
/* 702 */	NdrFcShort( 0x1 ),	/* 1 */
/* 704 */	0x29,		/* Corr desc:  parameter, FC_ULONG */
			0x0,		/*  */
/* 706 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 708 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 710 */	0x1,		/* FC_BYTE */
			0x5b,		/* FC_END */
/* 712 */	
			0x11, 0x10,	/* FC_RP [pointer_deref] */
/* 714 */	NdrFcShort( 0x2 ),	/* Offset= 2 (716) */
/* 716 */	
			0x2f,		/* FC_IP */
			0x5a,		/* FC_CONSTANT_IID */
/* 718 */	NdrFcLong( 0xb ),	/* 11 */
/* 722 */	NdrFcShort( 0x0 ),	/* 0 */
/* 724 */	NdrFcShort( 0x0 ),	/* 0 */
/* 726 */	0xc0,		/* 192 */
			0x0,		/* 0 */
/* 728 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 730 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 732 */	0x0,		/* 0 */
			0x46,		/* 70 */
/* 734 */	
			0x12, 0x8,	/* FC_UP [simple_pointer] */
/* 736 */	
			0x25,		/* FC_C_WSTRING */
			0x5c,		/* FC_PAD */
/* 738 */	
			0x12, 0x0,	/* FC_UP */
/* 740 */	NdrFcShort( 0xe ),	/* Offset= 14 (754) */
/* 742 */	
			0x1b,		/* FC_CARRAY */
			0x1,		/* 1 */
/* 744 */	NdrFcShort( 0x2 ),	/* 2 */
/* 746 */	0x9,		/* Corr desc: FC_ULONG */
			0x0,		/*  */
/* 748 */	NdrFcShort( 0xfffc ),	/* -4 */
/* 750 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 752 */	0x5,		/* FC_WCHAR */
			0x5b,		/* FC_END */
/* 754 */	
			0x17,		/* FC_CSTRUCT */
			0x3,		/* 3 */
/* 756 */	NdrFcShort( 0x8 ),	/* 8 */
/* 758 */	NdrFcShort( 0xfffffff0 ),	/* Offset= -16 (742) */
/* 760 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 762 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 764 */	0xb4,		/* FC_USER_MARSHAL */
			0x83,		/* 131 */
/* 766 */	NdrFcShort( 0x0 ),	/* 0 */
/* 768 */	NdrFcShort( 0x4 ),	/* 4 */
/* 770 */	NdrFcShort( 0x0 ),	/* 0 */
/* 772 */	NdrFcShort( 0xffffffde ),	/* Offset= -34 (738) */
/* 774 */	
			0x12, 0x0,	/* FC_UP */
/* 776 */	NdrFcShort( 0x2 ),	/* Offset= 2 (778) */
/* 778 */	
			0x1b,		/* FC_CARRAY */
			0x3,		/* 3 */
/* 780 */	NdrFcShort( 0x10 ),	/* 16 */
/* 782 */	0x29,		/* Corr desc:  parameter, FC_ULONG */
			0x0,		/*  */
/* 784 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 786 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 788 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 790 */	NdrFcShort( 0xfffffe66 ),	/* Offset= -410 (380) */
/* 792 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 794 */	
			0x12, 0x0,	/* FC_UP */
/* 796 */	NdrFcShort( 0xfffffe82 ),	/* Offset= -382 (414) */
/* 798 */	
			0x11, 0x0,	/* FC_RP */
/* 800 */	NdrFcShort( 0x2 ),	/* Offset= 2 (802) */
/* 802 */	
			0x1c,		/* FC_CVARRAY */
			0x0,		/* 0 */
/* 804 */	NdrFcShort( 0x1 ),	/* 1 */
/* 806 */	0x29,		/* Corr desc:  parameter, FC_ULONG */
			0x0,		/*  */
/* 808 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 810 */	NdrFcShort( 0x0 ),	/* Corr flags:  */
/* 812 */	0x29,		/* Corr desc:  parameter, FC_ULONG */
			0x54,		/* FC_DEREFERENCE */
/* 814 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 816 */	NdrFcShort( 0x0 ),	/* Corr flags:  */
/* 818 */	0x1,		/* FC_BYTE */
			0x5b,		/* FC_END */
/* 820 */	
			0x11, 0x0,	/* FC_RP */
/* 822 */	NdrFcShort( 0x2 ),	/* Offset= 2 (824) */
/* 824 */	
			0x1b,		/* FC_CARRAY */
			0x0,		/* 0 */
/* 826 */	NdrFcShort( 0x1 ),	/* 1 */
/* 828 */	0x29,		/* Corr desc:  parameter, FC_ULONG */
			0x0,		/*  */
/* 830 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 832 */	NdrFcShort( 0x0 ),	/* Corr flags:  */
/* 834 */	0x1,		/* FC_BYTE */
			0x5b,		/* FC_END */
/* 836 */	
			0x11, 0x0,	/* FC_RP */
/* 838 */	NdrFcShort( 0x52 ),	/* Offset= 82 (920) */
/* 840 */	
			0x13, 0x0,	/* FC_OP */
/* 842 */	NdrFcShort( 0x2 ),	/* Offset= 2 (844) */
/* 844 */	
			0x2a,		/* FC_ENCAPSULATED_UNION */
			0x48,		/* 72 */
/* 846 */	NdrFcShort( 0x4 ),	/* 4 */
/* 848 */	NdrFcShort( 0x2 ),	/* 2 */
/* 850 */	NdrFcLong( 0x48746457 ),	/* 1215587415 */
/* 854 */	NdrFcShort( 0x8008 ),	/* Simple arm type: FC_LONG */
/* 856 */	NdrFcLong( 0x52746457 ),	/* 1383359575 */
/* 860 */	NdrFcShort( 0xfffffe92 ),	/* Offset= -366 (494) */
/* 862 */	NdrFcShort( 0xffffffff ),	/* Offset= -1 (861) */
/* 864 */	0xb4,		/* FC_USER_MARSHAL */
			0x83,		/* 131 */
/* 866 */	NdrFcShort( 0x1 ),	/* 1 */
/* 868 */	NdrFcShort( 0x2 ),	/* 2 */
/* 870 */	NdrFcShort( 0x0 ),	/* 0 */
/* 872 */	NdrFcShort( 0xffffffe0 ),	/* Offset= -32 (840) */
/* 874 */	
			0x1b,		/* FC_CARRAY */
			0x0,		/* 0 */
/* 876 */	NdrFcShort( 0x1 ),	/* 1 */
/* 878 */	0x0,		/* Corr desc:  */
			0x59,		/* FC_CALLBACK */
/* 880 */	NdrFcShort( 0x0 ),	/* 0 */
/* 882 */	NdrFcShort( 0x0 ),	/* Corr flags:  */
/* 884 */	0x1,		/* FC_BYTE */
			0x5b,		/* FC_END */
/* 886 */	
			0x17,		/* FC_CSTRUCT */
			0x3,		/* 3 */
/* 888 */	NdrFcShort( 0xc ),	/* 12 */
/* 890 */	NdrFcShort( 0xfffffff0 ),	/* Offset= -16 (874) */
/* 892 */	0x8,		/* FC_LONG */
			0x6,		/* FC_SHORT */
/* 894 */	0x6,		/* FC_SHORT */
			0x6,		/* FC_SHORT */
/* 896 */	0x6,		/* FC_SHORT */
			0x5b,		/* FC_END */
/* 898 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 900 */	NdrFcShort( 0x14 ),	/* 20 */
/* 902 */	NdrFcShort( 0x0 ),	/* 0 */
/* 904 */	NdrFcShort( 0xc ),	/* Offset= 12 (916) */
/* 906 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 908 */	NdrFcShort( 0xffffffd4 ),	/* Offset= -44 (864) */
/* 910 */	0x3e,		/* FC_STRUCTPAD2 */
			0x36,		/* FC_POINTER */
/* 912 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 914 */	0x8,		/* FC_LONG */
			0x5b,		/* FC_END */
/* 916 */	
			0x13, 0x0,	/* FC_OP */
/* 918 */	NdrFcShort( 0xffffffe0 ),	/* Offset= -32 (886) */
/* 920 */	
			0x21,		/* FC_BOGUS_ARRAY */
			0x3,		/* 3 */
/* 922 */	NdrFcShort( 0x0 ),	/* 0 */
/* 924 */	0x29,		/* Corr desc:  parameter, FC_ULONG */
			0x0,		/*  */
/* 926 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 928 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 930 */	0x29,		/* Corr desc:  parameter, FC_ULONG */
			0x54,		/* FC_DEREFERENCE */
/* 932 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 934 */	NdrFcShort( 0x0 ),	/* Corr flags:  */
/* 936 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 938 */	NdrFcShort( 0xffffffd8 ),	/* Offset= -40 (898) */
/* 940 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 942 */	
			0x11, 0x10,	/* FC_RP [pointer_deref] */
/* 944 */	NdrFcShort( 0x2 ),	/* Offset= 2 (946) */
/* 946 */	
			0x2f,		/* FC_IP */
			0x5a,		/* FC_CONSTANT_IID */
/* 948 */	NdrFcLong( 0x103 ),	/* 259 */
/* 952 */	NdrFcShort( 0x0 ),	/* 0 */
/* 954 */	NdrFcShort( 0x0 ),	/* 0 */
/* 956 */	0xc0,		/* 192 */
			0x0,		/* 0 */
/* 958 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 960 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 962 */	0x0,		/* 0 */
			0x46,		/* 70 */
/* 964 */	
			0x11, 0x0,	/* FC_RP */
/* 966 */	NdrFcShort( 0x28 ),	/* Offset= 40 (1006) */
/* 968 */	
			0x2f,		/* FC_IP */
			0x5a,		/* FC_CONSTANT_IID */
/* 970 */	NdrFcLong( 0x10f ),	/* 271 */
/* 974 */	NdrFcShort( 0x0 ),	/* 0 */
/* 976 */	NdrFcShort( 0x0 ),	/* 0 */
/* 978 */	0xc0,		/* 192 */
			0x0,		/* 0 */
/* 980 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 982 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 984 */	0x0,		/* 0 */
			0x46,		/* 70 */
/* 986 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 988 */	NdrFcShort( 0x20 ),	/* 32 */
/* 990 */	NdrFcShort( 0x0 ),	/* 0 */
/* 992 */	NdrFcShort( 0xe ),	/* Offset= 14 (1006) */
/* 994 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 996 */	NdrFcShort( 0xffffff9e ),	/* Offset= -98 (898) */
/* 998 */	0x8,		/* FC_LONG */
			0x4c,		/* FC_EMBEDDED_COMPLEX */
/* 1000 */	0x0,		/* 0 */
			NdrFcShort( 0xffffffdf ),	/* Offset= -33 (968) */
			0x8,		/* FC_LONG */
/* 1004 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 1006 */	
			0x21,		/* FC_BOGUS_ARRAY */
			0x3,		/* 3 */
/* 1008 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1010 */	0x29,		/* Corr desc:  parameter, FC_ULONG */
			0x0,		/*  */
/* 1012 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 1014 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 1016 */	0x29,		/* Corr desc:  parameter, FC_ULONG */
			0x54,		/* FC_DEREFERENCE */
/* 1018 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 1020 */	NdrFcShort( 0x0 ),	/* Corr flags:  */
/* 1022 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 1024 */	NdrFcShort( 0xffffffda ),	/* Offset= -38 (986) */
/* 1026 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 1028 */	
			0x11, 0x10,	/* FC_RP [pointer_deref] */
/* 1030 */	NdrFcShort( 0x2 ),	/* Offset= 2 (1032) */
/* 1032 */	
			0x2f,		/* FC_IP */
			0x5a,		/* FC_CONSTANT_IID */
/* 1034 */	NdrFcLong( 0x105 ),	/* 261 */
/* 1038 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1040 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1042 */	0xc0,		/* 192 */
			0x0,		/* 0 */
/* 1044 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 1046 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 1048 */	0x0,		/* 0 */
			0x46,		/* 70 */
/* 1050 */	
			0x12, 0x0,	/* FC_UP */
/* 1052 */	NdrFcShort( 0xffffff66 ),	/* Offset= -154 (898) */
/* 1054 */	
			0x12, 0x0,	/* FC_UP */
/* 1056 */	NdrFcShort( 0x174 ),	/* Offset= 372 (1428) */
/* 1058 */	
			0x12, 0x0,	/* FC_UP */
/* 1060 */	NdrFcShort( 0x15e ),	/* Offset= 350 (1410) */
/* 1062 */	
			0x2a,		/* FC_ENCAPSULATED_UNION */
			0x49,		/* 73 */
/* 1064 */	NdrFcShort( 0x4 ),	/* 4 */
/* 1066 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1068 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1072 */	NdrFcShort( 0x0 ),	/* Offset= 0 (1072) */
/* 1074 */	NdrFcLong( 0x20 ),	/* 32 */
/* 1078 */	NdrFcShort( 0x28 ),	/* Offset= 40 (1118) */
/* 1080 */	NdrFcLong( 0x40 ),	/* 64 */
/* 1084 */	NdrFcShort( 0x80 ),	/* Offset= 128 (1212) */
/* 1086 */	NdrFcLong( 0x10 ),	/* 16 */
/* 1090 */	NdrFcShort( 0x92 ),	/* Offset= 146 (1236) */
/* 1092 */	NdrFcLong( 0x1 ),	/* 1 */
/* 1096 */	NdrFcShort( 0x10e ),	/* Offset= 270 (1366) */
/* 1098 */	NdrFcLong( 0x2 ),	/* 2 */
/* 1102 */	NdrFcShort( 0xfffffe90 ),	/* Offset= -368 (734) */
/* 1104 */	NdrFcLong( 0x4 ),	/* 4 */
/* 1108 */	NdrFcShort( 0x3a ),	/* Offset= 58 (1166) */
/* 1110 */	NdrFcLong( 0x8 ),	/* 8 */
/* 1114 */	NdrFcShort( 0x34 ),	/* Offset= 52 (1166) */
/* 1116 */	NdrFcShort( 0xffffffff ),	/* Offset= -1 (1115) */
/* 1118 */	
			0x12, 0x0,	/* FC_UP */
/* 1120 */	NdrFcShort( 0x2 ),	/* Offset= 2 (1122) */
/* 1122 */	
			0x2a,		/* FC_ENCAPSULATED_UNION */
			0x48,		/* 72 */
/* 1124 */	NdrFcShort( 0x4 ),	/* 4 */
/* 1126 */	NdrFcShort( 0x2 ),	/* 2 */
/* 1128 */	NdrFcLong( 0x48746457 ),	/* 1215587415 */
/* 1132 */	NdrFcShort( 0x8008 ),	/* Simple arm type: FC_LONG */
/* 1134 */	NdrFcLong( 0x52746457 ),	/* 1383359575 */
/* 1138 */	NdrFcShort( 0x4 ),	/* Offset= 4 (1142) */
/* 1140 */	NdrFcShort( 0xffffffff ),	/* Offset= -1 (1139) */
/* 1142 */	
			0x12, 0x0,	/* FC_UP */
/* 1144 */	NdrFcShort( 0x2e ),	/* Offset= 46 (1190) */
/* 1146 */	
			0x2a,		/* FC_ENCAPSULATED_UNION */
			0x48,		/* 72 */
/* 1148 */	NdrFcShort( 0x4 ),	/* 4 */
/* 1150 */	NdrFcShort( 0x2 ),	/* 2 */
/* 1152 */	NdrFcLong( 0x48746457 ),	/* 1215587415 */
/* 1156 */	NdrFcShort( 0x8008 ),	/* Simple arm type: FC_LONG */
/* 1158 */	NdrFcLong( 0x52746457 ),	/* 1383359575 */
/* 1162 */	NdrFcShort( 0x4 ),	/* Offset= 4 (1166) */
/* 1164 */	NdrFcShort( 0xffffffff ),	/* Offset= -1 (1163) */
/* 1166 */	
			0x12, 0x0,	/* FC_UP */
/* 1168 */	NdrFcShort( 0xe ),	/* Offset= 14 (1182) */
/* 1170 */	
			0x1b,		/* FC_CARRAY */
			0x0,		/* 0 */
/* 1172 */	NdrFcShort( 0x1 ),	/* 1 */
/* 1174 */	0x9,		/* Corr desc: FC_ULONG */
			0x0,		/*  */
/* 1176 */	NdrFcShort( 0xfffc ),	/* -4 */
/* 1178 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 1180 */	0x1,		/* FC_BYTE */
			0x5b,		/* FC_END */
/* 1182 */	
			0x17,		/* FC_CSTRUCT */
			0x3,		/* 3 */
/* 1184 */	NdrFcShort( 0x4 ),	/* 4 */
/* 1186 */	NdrFcShort( 0xfffffff0 ),	/* Offset= -16 (1170) */
/* 1188 */	0x8,		/* FC_LONG */
			0x5b,		/* FC_END */
/* 1190 */	
			0x16,		/* FC_PSTRUCT */
			0x3,		/* 3 */
/* 1192 */	NdrFcShort( 0x10 ),	/* 16 */
/* 1194 */	
			0x4b,		/* FC_PP */
			0x5c,		/* FC_PAD */
/* 1196 */	
			0x46,		/* FC_NO_REPEAT */
			0x5c,		/* FC_PAD */
/* 1198 */	NdrFcShort( 0xc ),	/* 12 */
/* 1200 */	NdrFcShort( 0xc ),	/* 12 */
/* 1202 */	0x12, 0x0,	/* FC_UP */
/* 1204 */	NdrFcShort( 0xffffffc6 ),	/* Offset= -58 (1146) */
/* 1206 */	
			0x5b,		/* FC_END */

			0x8,		/* FC_LONG */
/* 1208 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 1210 */	0x8,		/* FC_LONG */
			0x5b,		/* FC_END */
/* 1212 */	
			0x12, 0x0,	/* FC_UP */
/* 1214 */	NdrFcShort( 0x2 ),	/* Offset= 2 (1216) */
/* 1216 */	
			0x2a,		/* FC_ENCAPSULATED_UNION */
			0x48,		/* 72 */
/* 1218 */	NdrFcShort( 0x4 ),	/* 4 */
/* 1220 */	NdrFcShort( 0x2 ),	/* 2 */
/* 1222 */	NdrFcLong( 0x48746457 ),	/* 1215587415 */
/* 1226 */	NdrFcShort( 0x8008 ),	/* Simple arm type: FC_LONG */
/* 1228 */	NdrFcLong( 0x52746457 ),	/* 1383359575 */
/* 1232 */	NdrFcShort( 0xffffffbe ),	/* Offset= -66 (1166) */
/* 1234 */	NdrFcShort( 0xffffffff ),	/* Offset= -1 (1233) */
/* 1236 */	
			0x12, 0x0,	/* FC_UP */
/* 1238 */	NdrFcShort( 0x2 ),	/* Offset= 2 (1240) */
/* 1240 */	
			0x2a,		/* FC_ENCAPSULATED_UNION */
			0x49,		/* 73 */
/* 1242 */	NdrFcShort( 0x4 ),	/* 4 */
/* 1244 */	NdrFcShort( 0x2 ),	/* 2 */
/* 1246 */	NdrFcLong( 0x7 ),	/* 7 */
/* 1250 */	NdrFcShort( 0xa ),	/* Offset= 10 (1260) */
/* 1252 */	NdrFcLong( 0x5 ),	/* 5 */
/* 1256 */	NdrFcShort( 0x2e ),	/* Offset= 46 (1302) */
/* 1258 */	NdrFcShort( 0x6c ),	/* Offset= 108 (1366) */
/* 1260 */	
			0x12, 0x0,	/* FC_UP */
/* 1262 */	NdrFcShort( 0x2 ),	/* Offset= 2 (1264) */
/* 1264 */	
			0x2a,		/* FC_ENCAPSULATED_UNION */
			0x48,		/* 72 */
/* 1266 */	NdrFcShort( 0x4 ),	/* 4 */
/* 1268 */	NdrFcShort( 0x2 ),	/* 2 */
/* 1270 */	NdrFcLong( 0x48746457 ),	/* 1215587415 */
/* 1274 */	NdrFcShort( 0x8008 ),	/* Simple arm type: FC_LONG */
/* 1276 */	NdrFcLong( 0x52746457 ),	/* 1383359575 */
/* 1280 */	NdrFcShort( 0x4 ),	/* Offset= 4 (1284) */
/* 1282 */	NdrFcShort( 0xffffffff ),	/* Offset= -1 (1281) */
/* 1284 */	
			0x12, 0x0,	/* FC_UP */
/* 1286 */	NdrFcShort( 0x2 ),	/* Offset= 2 (1288) */
/* 1288 */	
			0x17,		/* FC_CSTRUCT */
			0x3,		/* 3 */
/* 1290 */	NdrFcShort( 0x18 ),	/* 24 */
/* 1292 */	NdrFcShort( 0xffffff86 ),	/* Offset= -122 (1170) */
/* 1294 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 1296 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 1298 */	0x6,		/* FC_SHORT */
			0x6,		/* FC_SHORT */
/* 1300 */	0x8,		/* FC_LONG */
			0x5b,		/* FC_END */
/* 1302 */	
			0x12, 0x0,	/* FC_UP */
/* 1304 */	NdrFcShort( 0x2 ),	/* Offset= 2 (1306) */
/* 1306 */	
			0x2a,		/* FC_ENCAPSULATED_UNION */
			0x48,		/* 72 */
/* 1308 */	NdrFcShort( 0x4 ),	/* 4 */
/* 1310 */	NdrFcShort( 0x2 ),	/* 2 */
/* 1312 */	NdrFcLong( 0x48746457 ),	/* 1215587415 */
/* 1316 */	NdrFcShort( 0x8008 ),	/* Simple arm type: FC_LONG */
/* 1318 */	NdrFcLong( 0x52746457 ),	/* 1383359575 */
/* 1322 */	NdrFcShort( 0x4 ),	/* Offset= 4 (1326) */
/* 1324 */	NdrFcShort( 0xffffffff ),	/* Offset= -1 (1323) */
/* 1326 */	
			0x12, 0x0,	/* FC_UP */
/* 1328 */	NdrFcShort( 0x1c ),	/* Offset= 28 (1356) */
/* 1330 */	
			0x15,		/* FC_STRUCT */
			0x0,		/* 0 */
/* 1332 */	NdrFcShort( 0x4 ),	/* 4 */
/* 1334 */	0x1,		/* FC_BYTE */
			0x1,		/* FC_BYTE */
/* 1336 */	0x1,		/* FC_BYTE */
			0x1,		/* FC_BYTE */
/* 1338 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 1340 */	
			0x1b,		/* FC_CARRAY */
			0x0,		/* 0 */
/* 1342 */	NdrFcShort( 0x4 ),	/* 4 */
/* 1344 */	0x7,		/* Corr desc: FC_USHORT */
			0x0,		/*  */
/* 1346 */	NdrFcShort( 0xfffe ),	/* -2 */
/* 1348 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 1350 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 1352 */	NdrFcShort( 0xffffffea ),	/* Offset= -22 (1330) */
/* 1354 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 1356 */	
			0x17,		/* FC_CSTRUCT */
			0x1,		/* 1 */
/* 1358 */	NdrFcShort( 0x4 ),	/* 4 */
/* 1360 */	NdrFcShort( 0xffffffec ),	/* Offset= -20 (1340) */
/* 1362 */	0x6,		/* FC_SHORT */
			0x6,		/* FC_SHORT */
/* 1364 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 1366 */	
			0x12, 0x0,	/* FC_UP */
/* 1368 */	NdrFcShort( 0x2 ),	/* Offset= 2 (1370) */
/* 1370 */	
			0x2a,		/* FC_ENCAPSULATED_UNION */
			0x88,		/* 136 */
/* 1372 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1374 */	NdrFcShort( 0x3 ),	/* 3 */
/* 1376 */	NdrFcLong( 0x48746457 ),	/* 1215587415 */
/* 1380 */	NdrFcShort( 0x8008 ),	/* Simple arm type: FC_LONG */
/* 1382 */	NdrFcLong( 0x52746457 ),	/* 1383359575 */
/* 1386 */	NdrFcShort( 0xa ),	/* Offset= 10 (1396) */
/* 1388 */	NdrFcLong( 0x50746457 ),	/* 1349805143 */
/* 1392 */	NdrFcShort( 0x800b ),	/* Simple arm type: FC_HYPER */
/* 1394 */	NdrFcShort( 0xffffffff ),	/* Offset= -1 (1393) */
/* 1396 */	
			0x12, 0x0,	/* FC_UP */
/* 1398 */	NdrFcShort( 0x2 ),	/* Offset= 2 (1400) */
/* 1400 */	
			0x17,		/* FC_CSTRUCT */
			0x3,		/* 3 */
/* 1402 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1404 */	NdrFcShort( 0xffffff16 ),	/* Offset= -234 (1170) */
/* 1406 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 1408 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 1410 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 1412 */	NdrFcShort( 0xc ),	/* 12 */
/* 1414 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1416 */	NdrFcShort( 0xc ),	/* Offset= 12 (1428) */
/* 1418 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 1420 */	NdrFcShort( 0xfffffe9a ),	/* Offset= -358 (1062) */
/* 1422 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 1424 */	NdrFcShort( 0xfffffa76 ),	/* Offset= -1418 (6) */
/* 1426 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 1428 */	0xb4,		/* FC_USER_MARSHAL */
			0x83,		/* 131 */
/* 1430 */	NdrFcShort( 0x2 ),	/* 2 */
/* 1432 */	NdrFcShort( 0xc ),	/* 12 */
/* 1434 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1436 */	NdrFcShort( 0xfffffe86 ),	/* Offset= -378 (1058) */
/* 1438 */	
			0x11, 0x4,	/* FC_RP [alloced_on_stack] */
/* 1440 */	NdrFcShort( 0x6 ),	/* Offset= 6 (1446) */
/* 1442 */	
			0x13, 0x0,	/* FC_OP */
/* 1444 */	NdrFcShort( 0xffffffde ),	/* Offset= -34 (1410) */
/* 1446 */	0xb4,		/* FC_USER_MARSHAL */
			0x83,		/* 131 */
/* 1448 */	NdrFcShort( 0x3 ),	/* 3 */
/* 1450 */	NdrFcShort( 0xc ),	/* 12 */
/* 1452 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1454 */	NdrFcShort( 0xfffffff4 ),	/* Offset= -12 (1442) */
/* 1456 */	
			0x11, 0x0,	/* FC_RP */
/* 1458 */	NdrFcShort( 0xfffffff4 ),	/* Offset= -12 (1446) */
/* 1460 */	
			0x11, 0x4,	/* FC_RP [alloced_on_stack] */
/* 1462 */	NdrFcShort( 0xfffffdcc ),	/* Offset= -564 (898) */
/* 1464 */	
			0x12, 0x0,	/* FC_UP */
/* 1466 */	NdrFcShort( 0x16 ),	/* Offset= 22 (1488) */
/* 1468 */	
			0x12, 0x0,	/* FC_UP */
/* 1470 */	NdrFcShort( 0x2 ),	/* Offset= 2 (1472) */
/* 1472 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 1474 */	NdrFcShort( 0x14 ),	/* 20 */
/* 1476 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1478 */	NdrFcShort( 0x0 ),	/* Offset= 0 (1478) */
/* 1480 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 1482 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 1484 */	NdrFcShort( 0xffffffb6 ),	/* Offset= -74 (1410) */
/* 1486 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 1488 */	0xb4,		/* FC_USER_MARSHAL */
			0x83,		/* 131 */
/* 1490 */	NdrFcShort( 0x4 ),	/* 4 */
/* 1492 */	NdrFcShort( 0x14 ),	/* 20 */
/* 1494 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1496 */	NdrFcShort( 0xffffffe4 ),	/* Offset= -28 (1468) */
/* 1498 */	
			0x11, 0x0,	/* FC_RP */
/* 1500 */	NdrFcShort( 0xfffffda6 ),	/* Offset= -602 (898) */
/* 1502 */	
			0x11, 0x10,	/* FC_RP [pointer_deref] */
/* 1504 */	NdrFcShort( 0x2 ),	/* Offset= 2 (1506) */
/* 1506 */	
			0x2f,		/* FC_IP */
			0x5c,		/* FC_PAD */
/* 1508 */	0x28,		/* Corr desc:  parameter, FC_LONG */
			0x0,		/*  */
/* 1510 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 1512 */	NdrFcShort( 0x5 ),	/* Corr flags:  early, iid_is, */
/* 1514 */	
			0x11, 0x10,	/* FC_RP [pointer_deref] */
/* 1516 */	NdrFcShort( 0x2 ),	/* Offset= 2 (1518) */
/* 1518 */	
			0x2f,		/* FC_IP */
			0x5c,		/* FC_PAD */
/* 1520 */	0x28,		/* Corr desc:  parameter, FC_LONG */
			0x0,		/*  */
/* 1522 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 1524 */	NdrFcShort( 0x5 ),	/* Corr flags:  early, iid_is, */
/* 1526 */	
			0x11, 0x10,	/* FC_RP [pointer_deref] */
/* 1528 */	NdrFcShort( 0x2 ),	/* Offset= 2 (1530) */
/* 1530 */	
			0x2f,		/* FC_IP */
			0x5c,		/* FC_PAD */
/* 1532 */	0x28,		/* Corr desc:  parameter, FC_LONG */
			0x0,		/*  */
/* 1534 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 1536 */	NdrFcShort( 0x5 ),	/* Corr flags:  early, iid_is, */
/* 1538 */	
			0x11, 0x0,	/* FC_RP */
/* 1540 */	NdrFcShort( 0x2 ),	/* Offset= 2 (1542) */
/* 1542 */	
			0x1c,		/* FC_CVARRAY */
			0x0,		/* 0 */
/* 1544 */	NdrFcShort( 0x1 ),	/* 1 */
/* 1546 */	0x29,		/* Corr desc:  parameter, FC_ULONG */
			
			0x20,		/* FC_LGVARRAY or FC_SPLIT */
/* 1548 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 1550 */	NdrFcShort( 0x2 ),	/* Corr flags:  split, */
/* 1552 */	0x29,		/* Corr desc:  parameter, FC_ULONG */
			0x54,		/* FC_DEREFERENCE */
/* 1554 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 1556 */	NdrFcShort( 0x0 ),	/* Corr flags:  */
/* 1558 */	0x1,		/* FC_BYTE */
			0x5b,		/* FC_END */
/* 1560 */	
			0x11, 0x0,	/* FC_RP */
/* 1562 */	NdrFcShort( 0x2 ),	/* Offset= 2 (1564) */
/* 1564 */	
			0x1c,		/* FC_CVARRAY */
			0x3,		/* 3 */
/* 1566 */	NdrFcShort( 0x4 ),	/* 4 */
/* 1568 */	0x29,		/* Corr desc:  parameter, FC_ULONG */
			0x0,		/*  */
/* 1570 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 1572 */	NdrFcShort( 0x0 ),	/* Corr flags:  */
/* 1574 */	0x29,		/* Corr desc:  parameter, FC_ULONG */
			0x54,		/* FC_DEREFERENCE */
/* 1576 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 1578 */	NdrFcShort( 0x0 ),	/* Corr flags:  */
/* 1580 */	0x8,		/* FC_LONG */
			0x5b,		/* FC_END */
/* 1582 */	
			0x11, 0x0,	/* FC_RP */
/* 1584 */	NdrFcShort( 0x2 ),	/* Offset= 2 (1586) */
/* 1586 */	
			0x1b,		/* FC_CARRAY */
			0x3,		/* 3 */
/* 1588 */	NdrFcShort( 0x4 ),	/* 4 */
/* 1590 */	0x29,		/* Corr desc:  parameter, FC_ULONG */
			0x0,		/*  */
/* 1592 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 1594 */	NdrFcShort( 0x0 ),	/* Corr flags:  */
/* 1596 */	0x8,		/* FC_LONG */
			0x5b,		/* FC_END */
/* 1598 */	
			0x11, 0x0,	/* FC_RP */
/* 1600 */	NdrFcShort( 0x2 ),	/* Offset= 2 (1602) */
/* 1602 */	
			0x1c,		/* FC_CVARRAY */
			0x3,		/* 3 */
/* 1604 */	NdrFcShort( 0x4 ),	/* 4 */
/* 1606 */	0x29,		/* Corr desc:  parameter, FC_ULONG */
			
			0x20,		/* FC_LGVARRAY or FC_SPLIT */
/* 1608 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 1610 */	NdrFcShort( 0x2 ),	/* Corr flags:  split, */
/* 1612 */	0x29,		/* Corr desc:  parameter, FC_ULONG */
			0x54,		/* FC_DEREFERENCE */
/* 1614 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 1616 */	NdrFcShort( 0x0 ),	/* Corr flags:  */
/* 1618 */	0x8,		/* FC_LONG */
			0x5b,		/* FC_END */
/* 1620 */	
			0x11, 0x0,	/* FC_RP */
/* 1622 */	NdrFcShort( 0x2 ),	/* Offset= 2 (1624) */
/* 1624 */	
			0x1c,		/* FC_CVARRAY */
			0x7,		/* 7 */
/* 1626 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1628 */	0x29,		/* Corr desc:  parameter, FC_ULONG */
			0x0,		/*  */
/* 1630 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 1632 */	NdrFcShort( 0x0 ),	/* Corr flags:  */
/* 1634 */	0x29,		/* Corr desc:  parameter, FC_ULONG */
			0x54,		/* FC_DEREFERENCE */
/* 1636 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 1638 */	NdrFcShort( 0x0 ),	/* Corr flags:  */
/* 1640 */	0xc,		/* FC_DOUBLE */
			0x5b,		/* FC_END */
/* 1642 */	
			0x11, 0x0,	/* FC_RP */
/* 1644 */	NdrFcShort( 0x2 ),	/* Offset= 2 (1646) */
/* 1646 */	
			0x1b,		/* FC_CARRAY */
			0x7,		/* 7 */
/* 1648 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1650 */	0x29,		/* Corr desc:  parameter, FC_ULONG */
			0x0,		/*  */
/* 1652 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 1654 */	NdrFcShort( 0x0 ),	/* Corr flags:  */
/* 1656 */	0xc,		/* FC_DOUBLE */
			0x5b,		/* FC_END */
/* 1658 */	
			0x11, 0x0,	/* FC_RP */
/* 1660 */	NdrFcShort( 0x2 ),	/* Offset= 2 (1662) */
/* 1662 */	
			0x1c,		/* FC_CVARRAY */
			0x7,		/* 7 */
/* 1664 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1666 */	0x29,		/* Corr desc:  parameter, FC_ULONG */
			
			0x20,		/* FC_LGVARRAY or FC_SPLIT */
/* 1668 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 1670 */	NdrFcShort( 0x2 ),	/* Corr flags:  split, */
/* 1672 */	0x29,		/* Corr desc:  parameter, FC_ULONG */
			0x54,		/* FC_DEREFERENCE */
/* 1674 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 1676 */	NdrFcShort( 0x0 ),	/* Corr flags:  */
/* 1678 */	0xc,		/* FC_DOUBLE */
			0x5b,		/* FC_END */
/* 1680 */	
			0x11, 0x4,	/* FC_RP [alloced_on_stack] */
/* 1682 */	NdrFcShort( 0x6 ),	/* Offset= 6 (1688) */
/* 1684 */	
			0x13, 0x0,	/* FC_OP */
/* 1686 */	NdrFcShort( 0xfffffe5a ),	/* Offset= -422 (1264) */
/* 1688 */	0xb4,		/* FC_USER_MARSHAL */
			0x83,		/* 131 */
/* 1690 */	NdrFcShort( 0x5 ),	/* 5 */
/* 1692 */	NdrFcShort( 0x4 ),	/* 4 */
/* 1694 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1696 */	NdrFcShort( 0xfffffff4 ),	/* Offset= -12 (1684) */
/* 1698 */	
			0x12, 0x0,	/* FC_UP */
/* 1700 */	NdrFcShort( 0x2 ),	/* Offset= 2 (1702) */
/* 1702 */	
			0x2a,		/* FC_ENCAPSULATED_UNION */
			0x48,		/* 72 */
/* 1704 */	NdrFcShort( 0x4 ),	/* 4 */
/* 1706 */	NdrFcShort( 0x2 ),	/* 2 */
/* 1708 */	NdrFcLong( 0x48746457 ),	/* 1215587415 */
/* 1712 */	NdrFcShort( 0x8008 ),	/* Simple arm type: FC_LONG */
/* 1714 */	NdrFcLong( 0x52746457 ),	/* 1383359575 */
/* 1718 */	NdrFcShort( 0x8008 ),	/* Simple arm type: FC_LONG */
/* 1720 */	NdrFcShort( 0xffffffff ),	/* Offset= -1 (1719) */
/* 1722 */	0xb4,		/* FC_USER_MARSHAL */
			0x83,		/* 131 */
/* 1724 */	NdrFcShort( 0x6 ),	/* 6 */
/* 1726 */	NdrFcShort( 0x4 ),	/* 4 */
/* 1728 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1730 */	NdrFcShort( 0xffffffe0 ),	/* Offset= -32 (1698) */
/* 1732 */	0xb4,		/* FC_USER_MARSHAL */
			0x83,		/* 131 */
/* 1734 */	NdrFcShort( 0x7 ),	/* 7 */
/* 1736 */	NdrFcShort( 0x4 ),	/* 4 */
/* 1738 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1740 */	NdrFcShort( 0xffffffd6 ),	/* Offset= -42 (1698) */

			0x0
        }
    };

static const USER_MARSHAL_ROUTINE_QUADRUPLE UserMarshalRoutines[ WIRE_MARSHAL_TABLE_SIZE ] = 
        {
            
            {
            SNB_UserSize
            ,SNB_UserMarshal
            ,SNB_UserUnmarshal
            ,SNB_UserFree
            },
            {
            CLIPFORMAT_UserSize
            ,CLIPFORMAT_UserMarshal
            ,CLIPFORMAT_UserUnmarshal
            ,CLIPFORMAT_UserFree
            },
            {
            ASYNC_STGMEDIUM_UserSize
            ,ASYNC_STGMEDIUM_UserMarshal
            ,ASYNC_STGMEDIUM_UserUnmarshal
            ,ASYNC_STGMEDIUM_UserFree
            },
            {
            STGMEDIUM_UserSize
            ,STGMEDIUM_UserMarshal
            ,STGMEDIUM_UserUnmarshal
            ,STGMEDIUM_UserFree
            },
            {
            FLAG_STGMEDIUM_UserSize
            ,FLAG_STGMEDIUM_UserMarshal
            ,FLAG_STGMEDIUM_UserUnmarshal
            ,FLAG_STGMEDIUM_UserFree
            },
            {
            HBITMAP_UserSize
            ,HBITMAP_UserMarshal
            ,HBITMAP_UserUnmarshal
            ,HBITMAP_UserFree
            },
            {
            HICON_UserSize
            ,HICON_UserMarshal
            ,HICON_UserUnmarshal
            ,HICON_UserFree
            },
            {
            HDC_UserSize
            ,HDC_UserMarshal
            ,HDC_UserUnmarshal
            ,HDC_UserFree
            }

        };


static void __RPC_USER IEnumFORMATETC_DVTARGETDEVICEExprEval_0000( PMIDL_STUB_MESSAGE pStubMsg )
{
    DVTARGETDEVICE *pS	=	( DVTARGETDEVICE * )(pStubMsg->StackTop - 12);
    
    pStubMsg->Offset = 0;
    pStubMsg->MaxCount = ( unsigned long ) ( pS->tdSize - sizeof( DWORD  ) - 4 * sizeof( WORD  ) );
}

static const EXPR_EVAL ExprEvalRoutines[] = 
    {
    IEnumFORMATETC_DVTARGETDEVICEExprEval_0000
    };



/* Standard interface: __MIDL_itf_objidl_0000, ver. 0.0,
   GUID={0x00000000,0x0000,0x0000,{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}} */


/* Object interface: IUnknown, ver. 0.0,
   GUID={0x00000000,0x0000,0x0000,{0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x46}} */


/* Object interface: IMarshal, ver. 0.0,
   GUID={0x00000003,0x0000,0x0000,{0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x46}} */


/* Object interface: IMarshal2, ver. 0.0,
   GUID={0x000001cf,0x0000,0x0000,{0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x46}} */


/* Object interface: IMalloc, ver. 0.0,
   GUID={0x00000002,0x0000,0x0000,{0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x46}} */


/* Object interface: IMallocSpy, ver. 0.0,
   GUID={0x0000001d,0x0000,0x0000,{0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x46}} */


/* Object interface: IStdMarshalInfo, ver. 0.0,
   GUID={0x00000018,0x0000,0x0000,{0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x46}} */


/* Object interface: IExternalConnection, ver. 0.0,
   GUID={0x00000019,0x0000,0x0000,{0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x46}} */


/* Standard interface: __MIDL_itf_objidl_0015, ver. 0.0,
   GUID={0x00000000,0x0000,0x0000,{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}} */


/* Object interface: IMultiQI, ver. 0.0,
   GUID={0x00000020,0x0000,0x0000,{0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x46}} */


/* Object interface: AsyncIMultiQI, ver. 0.0,
   GUID={0x000e0020,0x0000,0x0000,{0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x46}} */


/* Object interface: IInternalUnknown, ver. 0.0,
   GUID={0x00000021,0x0000,0x0000,{0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x46}} */


/* Object interface: IEnumUnknown, ver. 0.0,
   GUID={0x00000100,0x0000,0x0000,{0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x46}} */

#pragma code_seg(".orpc")
static const unsigned short IEnumUnknown_FormatStringOffsetTable[] =
    {
    0,
    48,
    84,
    114
    };

static const MIDL_STUBLESS_PROXY_INFO IEnumUnknown_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &IEnumUnknown_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const STUB_THUNK IEnumUnknown_StubThunkTable[] = 
    {
    IEnumUnknown_RemoteNext_Thunk,
    0,
    0,
    0
    };

static const MIDL_SERVER_INFO IEnumUnknown_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &IEnumUnknown_FormatStringOffsetTable[-3],
    &IEnumUnknown_StubThunkTable[-3],
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(7) _IEnumUnknownProxyVtbl = 
{
    &IEnumUnknown_ProxyInfo,
    &IID_IEnumUnknown,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    IEnumUnknown_Next_Proxy ,
    (void *) (INT_PTR) -1 /* IEnumUnknown::Skip */ ,
    (void *) (INT_PTR) -1 /* IEnumUnknown::Reset */ ,
    (void *) (INT_PTR) -1 /* IEnumUnknown::Clone */
};

const CInterfaceStubVtbl _IEnumUnknownStubVtbl =
{
    &IID_IEnumUnknown,
    &IEnumUnknown_ServerInfo,
    7,
    0, /* pure interpreted */
    CStdStubBuffer_METHODS
};


/* Object interface: IBindCtx, ver. 0.0,
   GUID={0x0000000e,0x0000,0x0000,{0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x46}} */

#pragma code_seg(".orpc")
static const unsigned short IBindCtx_FormatStringOffsetTable[] =
    {
    150,
    186,
    84,
    222,
    258,
    294,
    330,
    372,
    414,
    450
    };

static const MIDL_STUBLESS_PROXY_INFO IBindCtx_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &IBindCtx_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const STUB_THUNK IBindCtx_StubThunkTable[] = 
    {
    0,
    0,
    0,
    IBindCtx_RemoteSetBindOptions_Thunk,
    IBindCtx_RemoteGetBindOptions_Thunk,
    0,
    0,
    0,
    0,
    0
    };

static const MIDL_SERVER_INFO IBindCtx_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &IBindCtx_FormatStringOffsetTable[-3],
    &IBindCtx_StubThunkTable[-3],
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(13) _IBindCtxProxyVtbl = 
{
    &IBindCtx_ProxyInfo,
    &IID_IBindCtx,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    (void *) (INT_PTR) -1 /* IBindCtx::RegisterObjectBound */ ,
    (void *) (INT_PTR) -1 /* IBindCtx::RevokeObjectBound */ ,
    (void *) (INT_PTR) -1 /* IBindCtx::ReleaseBoundObjects */ ,
    IBindCtx_SetBindOptions_Proxy ,
    IBindCtx_GetBindOptions_Proxy ,
    (void *) (INT_PTR) -1 /* IBindCtx::GetRunningObjectTable */ ,
    (void *) (INT_PTR) -1 /* IBindCtx::RegisterObjectParam */ ,
    (void *) (INT_PTR) -1 /* IBindCtx::GetObjectParam */ ,
    (void *) (INT_PTR) -1 /* IBindCtx::EnumObjectParam */ ,
    (void *) (INT_PTR) -1 /* IBindCtx::RevokeObjectParam */
};

const CInterfaceStubVtbl _IBindCtxStubVtbl =
{
    &IID_IBindCtx,
    &IBindCtx_ServerInfo,
    13,
    0, /* pure interpreted */
    CStdStubBuffer_METHODS
};


/* Object interface: IEnumMoniker, ver. 0.0,
   GUID={0x00000102,0x0000,0x0000,{0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x46}} */

#pragma code_seg(".orpc")
static const unsigned short IEnumMoniker_FormatStringOffsetTable[] =
    {
    486,
    48,
    84,
    534
    };

static const MIDL_STUBLESS_PROXY_INFO IEnumMoniker_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &IEnumMoniker_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const STUB_THUNK IEnumMoniker_StubThunkTable[] = 
    {
    IEnumMoniker_RemoteNext_Thunk,
    0,
    0,
    0
    };

static const MIDL_SERVER_INFO IEnumMoniker_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &IEnumMoniker_FormatStringOffsetTable[-3],
    &IEnumMoniker_StubThunkTable[-3],
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(7) _IEnumMonikerProxyVtbl = 
{
    &IEnumMoniker_ProxyInfo,
    &IID_IEnumMoniker,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    IEnumMoniker_Next_Proxy ,
    (void *) (INT_PTR) -1 /* IEnumMoniker::Skip */ ,
    (void *) (INT_PTR) -1 /* IEnumMoniker::Reset */ ,
    (void *) (INT_PTR) -1 /* IEnumMoniker::Clone */
};

const CInterfaceStubVtbl _IEnumMonikerStubVtbl =
{
    &IID_IEnumMoniker,
    &IEnumMoniker_ServerInfo,
    7,
    0, /* pure interpreted */
    CStdStubBuffer_METHODS
};


/* Object interface: IRunnableObject, ver. 0.0,
   GUID={0x00000126,0x0000,0x0000,{0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x46}} */

#pragma code_seg(".orpc")
static const unsigned short IRunnableObject_FormatStringOffsetTable[] =
    {
    570,
    606,
    84,
    642,
    684
    };

static const MIDL_STUBLESS_PROXY_INFO IRunnableObject_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &IRunnableObject_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const STUB_THUNK IRunnableObject_StubThunkTable[] = 
    {
    0,
    0,
    IRunnableObject_RemoteIsRunning_Thunk,
    0,
    0
    };

static const MIDL_SERVER_INFO IRunnableObject_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &IRunnableObject_FormatStringOffsetTable[-3],
    &IRunnableObject_StubThunkTable[-3],
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(8) _IRunnableObjectProxyVtbl = 
{
    &IRunnableObject_ProxyInfo,
    &IID_IRunnableObject,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    (void *) (INT_PTR) -1 /* IRunnableObject::GetRunningClass */ ,
    (void *) (INT_PTR) -1 /* IRunnableObject::Run */ ,
    IRunnableObject_IsRunning_Proxy ,
    (void *) (INT_PTR) -1 /* IRunnableObject::LockRunning */ ,
    (void *) (INT_PTR) -1 /* IRunnableObject::SetContainedObject */
};

const CInterfaceStubVtbl _IRunnableObjectStubVtbl =
{
    &IID_IRunnableObject,
    &IRunnableObject_ServerInfo,
    8,
    0, /* pure interpreted */
    CStdStubBuffer_METHODS
};


/* Object interface: IRunningObjectTable, ver. 0.0,
   GUID={0x00000010,0x0000,0x0000,{0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x46}} */

#pragma code_seg(".orpc")
static const unsigned short IRunningObjectTable_FormatStringOffsetTable[] =
    {
    720,
    48,
    774,
    810,
    852,
    894,
    936
    };

static const MIDL_STUBLESS_PROXY_INFO IRunningObjectTable_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &IRunningObjectTable_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IRunningObjectTable_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &IRunningObjectTable_FormatStringOffsetTable[-3],
    0,
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(10) _IRunningObjectTableProxyVtbl = 
{
    &IRunningObjectTable_ProxyInfo,
    &IID_IRunningObjectTable,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    (void *) (INT_PTR) -1 /* IRunningObjectTable::Register */ ,
    (void *) (INT_PTR) -1 /* IRunningObjectTable::Revoke */ ,
    (void *) (INT_PTR) -1 /* IRunningObjectTable::IsRunning */ ,
    (void *) (INT_PTR) -1 /* IRunningObjectTable::GetObject */ ,
    (void *) (INT_PTR) -1 /* IRunningObjectTable::NoteChangeTime */ ,
    (void *) (INT_PTR) -1 /* IRunningObjectTable::GetTimeOfLastChange */ ,
    (void *) (INT_PTR) -1 /* IRunningObjectTable::EnumRunning */
};

const CInterfaceStubVtbl _IRunningObjectTableStubVtbl =
{
    &IID_IRunningObjectTable,
    &IRunningObjectTable_ServerInfo,
    10,
    0, /* pure interpreted */
    CStdStubBuffer_METHODS
};


/* Object interface: IPersist, ver. 0.0,
   GUID={0x0000010c,0x0000,0x0000,{0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x46}} */

#pragma code_seg(".orpc")
static const unsigned short IPersist_FormatStringOffsetTable[] =
    {
    570
    };

static const MIDL_STUBLESS_PROXY_INFO IPersist_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &IPersist_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IPersist_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &IPersist_FormatStringOffsetTable[-3],
    0,
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(4) _IPersistProxyVtbl = 
{
    &IPersist_ProxyInfo,
    &IID_IPersist,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    (void *) (INT_PTR) -1 /* IPersist::GetClassID */
};

const CInterfaceStubVtbl _IPersistStubVtbl =
{
    &IID_IPersist,
    &IPersist_ServerInfo,
    4,
    0, /* pure interpreted */
    CStdStubBuffer_METHODS
};


/* Object interface: IPersistStream, ver. 0.0,
   GUID={0x00000109,0x0000,0x0000,{0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x46}} */

#pragma code_seg(".orpc")
static const unsigned short IPersistStream_FormatStringOffsetTable[] =
    {
    570,
    972,
    1002,
    1038,
    1080
    };

static const MIDL_STUBLESS_PROXY_INFO IPersistStream_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &IPersistStream_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IPersistStream_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &IPersistStream_FormatStringOffsetTable[-3],
    0,
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(8) _IPersistStreamProxyVtbl = 
{
    &IPersistStream_ProxyInfo,
    &IID_IPersistStream,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    (void *) (INT_PTR) -1 /* IPersist::GetClassID */ ,
    (void *) (INT_PTR) -1 /* IPersistStream::IsDirty */ ,
    (void *) (INT_PTR) -1 /* IPersistStream::Load */ ,
    (void *) (INT_PTR) -1 /* IPersistStream::Save */ ,
    (void *) (INT_PTR) -1 /* IPersistStream::GetSizeMax */
};

const CInterfaceStubVtbl _IPersistStreamStubVtbl =
{
    &IID_IPersistStream,
    &IPersistStream_ServerInfo,
    8,
    0, /* pure interpreted */
    CStdStubBuffer_METHODS
};


/* Object interface: IMoniker, ver. 0.0,
   GUID={0x0000000f,0x0000,0x0000,{0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x46}} */

#pragma code_seg(".orpc")
static const unsigned short IMoniker_FormatStringOffsetTable[] =
    {
    570,
    972,
    1002,
    1038,
    1080,
    1116,
    1170,
    1224,
    1278,
    1326,
    1368,
    1404,
    1440,
    1488,
    1536,
    1572,
    1614,
    1656,
    1704,
    1764
    };

static const MIDL_STUBLESS_PROXY_INFO IMoniker_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &IMoniker_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const STUB_THUNK IMoniker_StubThunkTable[] = 
    {
    0,
    0,
    0,
    0,
    0,
    IMoniker_RemoteBindToObject_Thunk,
    IMoniker_RemoteBindToStorage_Thunk,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0
    };

static const MIDL_SERVER_INFO IMoniker_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &IMoniker_FormatStringOffsetTable[-3],
    &IMoniker_StubThunkTable[-3],
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(23) _IMonikerProxyVtbl = 
{
    &IMoniker_ProxyInfo,
    &IID_IMoniker,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    (void *) (INT_PTR) -1 /* IPersist::GetClassID */ ,
    (void *) (INT_PTR) -1 /* IPersistStream::IsDirty */ ,
    (void *) (INT_PTR) -1 /* IPersistStream::Load */ ,
    (void *) (INT_PTR) -1 /* IPersistStream::Save */ ,
    (void *) (INT_PTR) -1 /* IPersistStream::GetSizeMax */ ,
    IMoniker_BindToObject_Proxy ,
    IMoniker_BindToStorage_Proxy ,
    (void *) (INT_PTR) -1 /* IMoniker::Reduce */ ,
    (void *) (INT_PTR) -1 /* IMoniker::ComposeWith */ ,
    (void *) (INT_PTR) -1 /* IMoniker::Enum */ ,
    (void *) (INT_PTR) -1 /* IMoniker::IsEqual */ ,
    (void *) (INT_PTR) -1 /* IMoniker::Hash */ ,
    (void *) (INT_PTR) -1 /* IMoniker::IsRunning */ ,
    (void *) (INT_PTR) -1 /* IMoniker::GetTimeOfLastChange */ ,
    (void *) (INT_PTR) -1 /* IMoniker::Inverse */ ,
    (void *) (INT_PTR) -1 /* IMoniker::CommonPrefixWith */ ,
    (void *) (INT_PTR) -1 /* IMoniker::RelativePathTo */ ,
    (void *) (INT_PTR) -1 /* IMoniker::GetDisplayName */ ,
    (void *) (INT_PTR) -1 /* IMoniker::ParseDisplayName */ ,
    (void *) (INT_PTR) -1 /* IMoniker::IsSystemMoniker */
};

const CInterfaceStubVtbl _IMonikerStubVtbl =
{
    &IID_IMoniker,
    &IMoniker_ServerInfo,
    23,
    0, /* pure interpreted */
    CStdStubBuffer_METHODS
};


/* Object interface: IROTData, ver. 0.0,
   GUID={0xf29f6bc0,0x5021,0x11ce,{0xaa,0x15,0x00,0x00,0x69,0x01,0x29,0x3f}} */

#pragma code_seg(".orpc")
static const unsigned short IROTData_FormatStringOffsetTable[] =
    {
    1800
    };

static const MIDL_STUBLESS_PROXY_INFO IROTData_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &IROTData_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IROTData_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &IROTData_FormatStringOffsetTable[-3],
    0,
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(4) _IROTDataProxyVtbl = 
{
    &IROTData_ProxyInfo,
    &IID_IROTData,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    (void *) (INT_PTR) -1 /* IROTData::GetComparisonData */
};

const CInterfaceStubVtbl _IROTDataStubVtbl =
{
    &IID_IROTData,
    &IROTData_ServerInfo,
    4,
    0, /* pure interpreted */
    CStdStubBuffer_METHODS
};


/* Object interface: IEnumString, ver. 0.0,
   GUID={0x00000101,0x0000,0x0000,{0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x46}} */

#pragma code_seg(".orpc")
static const unsigned short IEnumString_FormatStringOffsetTable[] =
    {
    1848,
    48,
    84,
    1896
    };

static const MIDL_STUBLESS_PROXY_INFO IEnumString_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &IEnumString_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const STUB_THUNK IEnumString_StubThunkTable[] = 
    {
    IEnumString_RemoteNext_Thunk,
    0,
    0,
    0
    };

static const MIDL_SERVER_INFO IEnumString_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &IEnumString_FormatStringOffsetTable[-3],
    &IEnumString_StubThunkTable[-3],
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(7) _IEnumStringProxyVtbl = 
{
    &IEnumString_ProxyInfo,
    &IID_IEnumString,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    IEnumString_Next_Proxy ,
    (void *) (INT_PTR) -1 /* IEnumString::Skip */ ,
    (void *) (INT_PTR) -1 /* IEnumString::Reset */ ,
    (void *) (INT_PTR) -1 /* IEnumString::Clone */
};

const CInterfaceStubVtbl _IEnumStringStubVtbl =
{
    &IID_IEnumString,
    &IEnumString_ServerInfo,
    7,
    0, /* pure interpreted */
    CStdStubBuffer_METHODS
};


/* Object interface: ISequentialStream, ver. 0.0,
   GUID={0x0c733a30,0x2a1c,0x11ce,{0xad,0xe5,0x00,0xaa,0x00,0x44,0x77,0x3d}} */

#pragma code_seg(".orpc")
static const unsigned short ISequentialStream_FormatStringOffsetTable[] =
    {
    1932,
    1980
    };

static const MIDL_STUBLESS_PROXY_INFO ISequentialStream_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &ISequentialStream_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const STUB_THUNK ISequentialStream_StubThunkTable[] = 
    {
    ISequentialStream_RemoteRead_Thunk,
    ISequentialStream_RemoteWrite_Thunk
    };

static const MIDL_SERVER_INFO ISequentialStream_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &ISequentialStream_FormatStringOffsetTable[-3],
    &ISequentialStream_StubThunkTable[-3],
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(5) _ISequentialStreamProxyVtbl = 
{
    &ISequentialStream_ProxyInfo,
    &IID_ISequentialStream,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    ISequentialStream_Read_Proxy ,
    ISequentialStream_Write_Proxy
};

const CInterfaceStubVtbl _ISequentialStreamStubVtbl =
{
    &IID_ISequentialStream,
    &ISequentialStream_ServerInfo,
    5,
    0, /* pure interpreted */
    CStdStubBuffer_METHODS
};


/* Object interface: IStream, ver. 0.0,
   GUID={0x0000000c,0x0000,0x0000,{0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x46}} */

#pragma code_seg(".orpc")
static const unsigned short IStream_FormatStringOffsetTable[] =
    {
    1932,
    1980,
    2028,
    2076,
    2112,
    2166,
    2202,
    2232,
    2280,
    2328,
    2370
    };

static const MIDL_STUBLESS_PROXY_INFO IStream_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &IStream_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const STUB_THUNK IStream_StubThunkTable[] = 
    {
    ISequentialStream_RemoteRead_Thunk,
    ISequentialStream_RemoteWrite_Thunk,
    IStream_RemoteSeek_Thunk,
    0,
    IStream_RemoteCopyTo_Thunk,
    0,
    0,
    0,
    0,
    0,
    0
    };

static const MIDL_SERVER_INFO IStream_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &IStream_FormatStringOffsetTable[-3],
    &IStream_StubThunkTable[-3],
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(14) _IStreamProxyVtbl = 
{
    &IStream_ProxyInfo,
    &IID_IStream,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    ISequentialStream_Read_Proxy ,
    ISequentialStream_Write_Proxy ,
    IStream_Seek_Proxy ,
    (void *) (INT_PTR) -1 /* IStream::SetSize */ ,
    IStream_CopyTo_Proxy ,
    (void *) (INT_PTR) -1 /* IStream::Commit */ ,
    (void *) (INT_PTR) -1 /* IStream::Revert */ ,
    (void *) (INT_PTR) -1 /* IStream::LockRegion */ ,
    (void *) (INT_PTR) -1 /* IStream::UnlockRegion */ ,
    (void *) (INT_PTR) -1 /* IStream::Stat */ ,
    (void *) (INT_PTR) -1 /* IStream::Clone */
};

const CInterfaceStubVtbl _IStreamStubVtbl =
{
    &IID_IStream,
    &IStream_ServerInfo,
    14,
    0, /* pure interpreted */
    CStdStubBuffer_METHODS
};


/* Object interface: IEnumSTATSTG, ver. 0.0,
   GUID={0x0000000d,0x0000,0x0000,{0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x46}} */

#pragma code_seg(".orpc")
static const unsigned short IEnumSTATSTG_FormatStringOffsetTable[] =
    {
    2406,
    48,
    84,
    2454
    };

static const MIDL_STUBLESS_PROXY_INFO IEnumSTATSTG_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &IEnumSTATSTG_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const STUB_THUNK IEnumSTATSTG_StubThunkTable[] = 
    {
    IEnumSTATSTG_RemoteNext_Thunk,
    0,
    0,
    0
    };

static const MIDL_SERVER_INFO IEnumSTATSTG_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &IEnumSTATSTG_FormatStringOffsetTable[-3],
    &IEnumSTATSTG_StubThunkTable[-3],
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(7) _IEnumSTATSTGProxyVtbl = 
{
    &IEnumSTATSTG_ProxyInfo,
    &IID_IEnumSTATSTG,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    IEnumSTATSTG_Next_Proxy ,
    (void *) (INT_PTR) -1 /* IEnumSTATSTG::Skip */ ,
    (void *) (INT_PTR) -1 /* IEnumSTATSTG::Reset */ ,
    (void *) (INT_PTR) -1 /* IEnumSTATSTG::Clone */
};

const CInterfaceStubVtbl _IEnumSTATSTGStubVtbl =
{
    &IID_IEnumSTATSTG,
    &IEnumSTATSTG_ServerInfo,
    7,
    0, /* pure interpreted */
    CStdStubBuffer_METHODS
};


/* Object interface: IStorage, ver. 0.0,
   GUID={0x0000000b,0x0000,0x0000,{0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x46}} */

#pragma code_seg(".orpc")
static const unsigned short IStorage_FormatStringOffsetTable[] =
    {
    2490,
    2550,
    2616,
    2676,
    2742,
    2796,
    2850,
    2886,
    2916,
    450,
    2976,
    3018,
    3072,
    3108,
    3150
    };

static const MIDL_STUBLESS_PROXY_INFO IStorage_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &IStorage_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const STUB_THUNK IStorage_StubThunkTable[] = 
    {
    0,
    IStorage_RemoteOpenStream_Thunk,
    0,
    0,
    0,
    0,
    0,
    0,
    IStorage_RemoteEnumElements_Thunk,
    0,
    0,
    0,
    0,
    0,
    0
    };

static const MIDL_SERVER_INFO IStorage_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &IStorage_FormatStringOffsetTable[-3],
    &IStorage_StubThunkTable[-3],
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(18) _IStorageProxyVtbl = 
{
    &IStorage_ProxyInfo,
    &IID_IStorage,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    (void *) (INT_PTR) -1 /* IStorage::CreateStream */ ,
    IStorage_OpenStream_Proxy ,
    (void *) (INT_PTR) -1 /* IStorage::CreateStorage */ ,
    (void *) (INT_PTR) -1 /* IStorage::OpenStorage */ ,
    (void *) (INT_PTR) -1 /* IStorage::CopyTo */ ,
    (void *) (INT_PTR) -1 /* IStorage::MoveElementTo */ ,
    (void *) (INT_PTR) -1 /* IStorage::Commit */ ,
    (void *) (INT_PTR) -1 /* IStorage::Revert */ ,
    IStorage_EnumElements_Proxy ,
    (void *) (INT_PTR) -1 /* IStorage::DestroyElement */ ,
    (void *) (INT_PTR) -1 /* IStorage::RenameElement */ ,
    (void *) (INT_PTR) -1 /* IStorage::SetElementTimes */ ,
    (void *) (INT_PTR) -1 /* IStorage::SetClass */ ,
    (void *) (INT_PTR) -1 /* IStorage::SetStateBits */ ,
    (void *) (INT_PTR) -1 /* IStorage::Stat */
};

const CInterfaceStubVtbl _IStorageStubVtbl =
{
    &IID_IStorage,
    &IStorage_ServerInfo,
    18,
    0, /* pure interpreted */
    CStdStubBuffer_METHODS
};


/* Object interface: IPersistFile, ver. 0.0,
   GUID={0x0000010b,0x0000,0x0000,{0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x46}} */

#pragma code_seg(".orpc")
static const unsigned short IPersistFile_FormatStringOffsetTable[] =
    {
    570,
    972,
    3192,
    3234,
    3276,
    3312
    };

static const MIDL_STUBLESS_PROXY_INFO IPersistFile_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &IPersistFile_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IPersistFile_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &IPersistFile_FormatStringOffsetTable[-3],
    0,
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(9) _IPersistFileProxyVtbl = 
{
    &IPersistFile_ProxyInfo,
    &IID_IPersistFile,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    (void *) (INT_PTR) -1 /* IPersist::GetClassID */ ,
    (void *) (INT_PTR) -1 /* IPersistFile::IsDirty */ ,
    (void *) (INT_PTR) -1 /* IPersistFile::Load */ ,
    (void *) (INT_PTR) -1 /* IPersistFile::Save */ ,
    (void *) (INT_PTR) -1 /* IPersistFile::SaveCompleted */ ,
    (void *) (INT_PTR) -1 /* IPersistFile::GetCurFile */
};

const CInterfaceStubVtbl _IPersistFileStubVtbl =
{
    &IID_IPersistFile,
    &IPersistFile_ServerInfo,
    9,
    0, /* pure interpreted */
    CStdStubBuffer_METHODS
};


/* Object interface: IPersistStorage, ver. 0.0,
   GUID={0x0000010a,0x0000,0x0000,{0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x46}} */

#pragma code_seg(".orpc")
static const unsigned short IPersistStorage_FormatStringOffsetTable[] =
    {
    570,
    972,
    3348,
    3384,
    3420,
    3462,
    2202
    };

static const MIDL_STUBLESS_PROXY_INFO IPersistStorage_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &IPersistStorage_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IPersistStorage_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &IPersistStorage_FormatStringOffsetTable[-3],
    0,
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(10) _IPersistStorageProxyVtbl = 
{
    &IPersistStorage_ProxyInfo,
    &IID_IPersistStorage,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    (void *) (INT_PTR) -1 /* IPersist::GetClassID */ ,
    (void *) (INT_PTR) -1 /* IPersistStorage::IsDirty */ ,
    (void *) (INT_PTR) -1 /* IPersistStorage::InitNew */ ,
    (void *) (INT_PTR) -1 /* IPersistStorage::Load */ ,
    (void *) (INT_PTR) -1 /* IPersistStorage::Save */ ,
    (void *) (INT_PTR) -1 /* IPersistStorage::SaveCompleted */ ,
    (void *) (INT_PTR) -1 /* IPersistStorage::HandsOffStorage */
};

const CInterfaceStubVtbl _IPersistStorageStubVtbl =
{
    &IID_IPersistStorage,
    &IPersistStorage_ServerInfo,
    10,
    0, /* pure interpreted */
    CStdStubBuffer_METHODS
};


/* Object interface: ILockBytes, ver. 0.0,
   GUID={0x0000000a,0x0000,0x0000,{0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x46}} */

#pragma code_seg(".orpc")
static const unsigned short ILockBytes_FormatStringOffsetTable[] =
    {
    3498,
    3552,
    84,
    2076,
    3606,
    3654,
    3702
    };

static const MIDL_STUBLESS_PROXY_INFO ILockBytes_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &ILockBytes_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const STUB_THUNK ILockBytes_StubThunkTable[] = 
    {
    ILockBytes_RemoteReadAt_Thunk,
    ILockBytes_RemoteWriteAt_Thunk,
    0,
    0,
    0,
    0,
    0
    };

static const MIDL_SERVER_INFO ILockBytes_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &ILockBytes_FormatStringOffsetTable[-3],
    &ILockBytes_StubThunkTable[-3],
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(10) _ILockBytesProxyVtbl = 
{
    &ILockBytes_ProxyInfo,
    &IID_ILockBytes,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    ILockBytes_ReadAt_Proxy ,
    ILockBytes_WriteAt_Proxy ,
    (void *) (INT_PTR) -1 /* ILockBytes::Flush */ ,
    (void *) (INT_PTR) -1 /* ILockBytes::SetSize */ ,
    (void *) (INT_PTR) -1 /* ILockBytes::LockRegion */ ,
    (void *) (INT_PTR) -1 /* ILockBytes::UnlockRegion */ ,
    (void *) (INT_PTR) -1 /* ILockBytes::Stat */
};

const CInterfaceStubVtbl _ILockBytesStubVtbl =
{
    &IID_ILockBytes,
    &ILockBytes_ServerInfo,
    10,
    0, /* pure interpreted */
    CStdStubBuffer_METHODS
};


/* Object interface: IEnumFORMATETC, ver. 0.0,
   GUID={0x00000103,0x0000,0x0000,{0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x46}} */

#pragma code_seg(".orpc")
static const unsigned short IEnumFORMATETC_FormatStringOffsetTable[] =
    {
    3744,
    48,
    84,
    3792
    };

static const MIDL_STUBLESS_PROXY_INFO IEnumFORMATETC_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &IEnumFORMATETC_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const STUB_THUNK IEnumFORMATETC_StubThunkTable[] = 
    {
    IEnumFORMATETC_RemoteNext_Thunk,
    0,
    0,
    0
    };

static const MIDL_SERVER_INFO IEnumFORMATETC_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &IEnumFORMATETC_FormatStringOffsetTable[-3],
    &IEnumFORMATETC_StubThunkTable[-3],
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(7) _IEnumFORMATETCProxyVtbl = 
{
    &IEnumFORMATETC_ProxyInfo,
    &IID_IEnumFORMATETC,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    IEnumFORMATETC_Next_Proxy ,
    (void *) (INT_PTR) -1 /* IEnumFORMATETC::Skip */ ,
    (void *) (INT_PTR) -1 /* IEnumFORMATETC::Reset */ ,
    (void *) (INT_PTR) -1 /* IEnumFORMATETC::Clone */
};

const CInterfaceStubVtbl _IEnumFORMATETCStubVtbl =
{
    &IID_IEnumFORMATETC,
    &IEnumFORMATETC_ServerInfo,
    7,
    0, /* pure interpreted */
    CStdStubBuffer_METHODS
};


/* Object interface: IEnumSTATDATA, ver. 0.0,
   GUID={0x00000105,0x0000,0x0000,{0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x46}} */

#pragma code_seg(".orpc")
static const unsigned short IEnumSTATDATA_FormatStringOffsetTable[] =
    {
    3828,
    48,
    84,
    3876
    };

static const MIDL_STUBLESS_PROXY_INFO IEnumSTATDATA_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &IEnumSTATDATA_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const STUB_THUNK IEnumSTATDATA_StubThunkTable[] = 
    {
    IEnumSTATDATA_RemoteNext_Thunk,
    0,
    0,
    0
    };

static const MIDL_SERVER_INFO IEnumSTATDATA_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &IEnumSTATDATA_FormatStringOffsetTable[-3],
    &IEnumSTATDATA_StubThunkTable[-3],
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(7) _IEnumSTATDATAProxyVtbl = 
{
    &IEnumSTATDATA_ProxyInfo,
    &IID_IEnumSTATDATA,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    IEnumSTATDATA_Next_Proxy ,
    (void *) (INT_PTR) -1 /* IEnumSTATDATA::Skip */ ,
    (void *) (INT_PTR) -1 /* IEnumSTATDATA::Reset */ ,
    (void *) (INT_PTR) -1 /* IEnumSTATDATA::Clone */
};

const CInterfaceStubVtbl _IEnumSTATDATAStubVtbl =
{
    &IID_IEnumSTATDATA,
    &IEnumSTATDATA_ServerInfo,
    7,
    0, /* pure interpreted */
    CStdStubBuffer_METHODS
};


/* Object interface: IRootStorage, ver. 0.0,
   GUID={0x00000012,0x0000,0x0000,{0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x46}} */

#pragma code_seg(".orpc")
static const unsigned short IRootStorage_FormatStringOffsetTable[] =
    {
    3912
    };

static const MIDL_STUBLESS_PROXY_INFO IRootStorage_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &IRootStorage_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IRootStorage_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &IRootStorage_FormatStringOffsetTable[-3],
    0,
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(4) _IRootStorageProxyVtbl = 
{
    &IRootStorage_ProxyInfo,
    &IID_IRootStorage,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    (void *) (INT_PTR) -1 /* IRootStorage::SwitchToFile */
};

const CInterfaceStubVtbl _IRootStorageStubVtbl =
{
    &IID_IRootStorage,
    &IRootStorage_ServerInfo,
    4,
    0, /* pure interpreted */
    CStdStubBuffer_METHODS
};


/* Object interface: IAdviseSink, ver. 0.0,
   GUID={0x0000010f,0x0000,0x0000,{0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x46}} */

#pragma code_seg(".orpc")
static const unsigned short IAdviseSink_FormatStringOffsetTable[] =
    {
    3948,
    3990,
    774,
    4032,
    4062
    };

static const MIDL_STUBLESS_PROXY_INFO IAdviseSink_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &IAdviseSink_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const STUB_THUNK IAdviseSink_StubThunkTable[] = 
    {
    IAdviseSink_RemoteOnDataChange_Thunk,
    IAdviseSink_RemoteOnViewChange_Thunk,
    IAdviseSink_RemoteOnRename_Thunk,
    IAdviseSink_RemoteOnSave_Thunk,
    IAdviseSink_RemoteOnClose_Thunk
    };

static const MIDL_SERVER_INFO IAdviseSink_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &IAdviseSink_FormatStringOffsetTable[-3],
    &IAdviseSink_StubThunkTable[-3],
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(8) _IAdviseSinkProxyVtbl = 
{
    &IAdviseSink_ProxyInfo,
    &IID_IAdviseSink,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    IAdviseSink_OnDataChange_Proxy ,
    IAdviseSink_OnViewChange_Proxy ,
    IAdviseSink_OnRename_Proxy ,
    IAdviseSink_OnSave_Proxy ,
    IAdviseSink_OnClose_Proxy
};

const CInterfaceStubVtbl _IAdviseSinkStubVtbl =
{
    &IID_IAdviseSink,
    &IAdviseSink_ServerInfo,
    8,
    0, /* pure interpreted */
    CStdStubBuffer_METHODS
};


/* Object interface: AsyncIAdviseSink, ver. 0.0,
   GUID={0x00000150,0x0000,0x0000,{0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x46}} */

#pragma code_seg(".orpc")
static const unsigned short AsyncIAdviseSink_FormatStringOffsetTable[] =
    {
    4092,
    4134,
    4164,
    4206,
    4236,
    4272,
    4302,
    4332,
    4362,
    4392
    };

static const MIDL_STUBLESS_PROXY_INFO AsyncIAdviseSink_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &AsyncIAdviseSink_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const STUB_THUNK AsyncIAdviseSink_StubThunkTable[] = 
    {
    AsyncIAdviseSink_Begin_RemoteOnDataChange_Thunk,
    AsyncIAdviseSink_Finish_RemoteOnDataChange_Thunk,
    AsyncIAdviseSink_Begin_RemoteOnViewChange_Thunk,
    AsyncIAdviseSink_Finish_RemoteOnViewChange_Thunk,
    AsyncIAdviseSink_Begin_RemoteOnRename_Thunk,
    AsyncIAdviseSink_Finish_RemoteOnRename_Thunk,
    AsyncIAdviseSink_Begin_RemoteOnSave_Thunk,
    AsyncIAdviseSink_Finish_RemoteOnSave_Thunk,
    AsyncIAdviseSink_Begin_RemoteOnClose_Thunk,
    AsyncIAdviseSink_Finish_RemoteOnClose_Thunk
    };

static const MIDL_SERVER_INFO AsyncIAdviseSink_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &AsyncIAdviseSink_FormatStringOffsetTable[-3],
    &AsyncIAdviseSink_StubThunkTable[-3],
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(13) _AsyncIAdviseSinkProxyVtbl = 
{
    &AsyncIAdviseSink_ProxyInfo,
    &IID_AsyncIAdviseSink,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    AsyncIAdviseSink_Begin_OnDataChange_Proxy ,
    AsyncIAdviseSink_Finish_OnDataChange_Proxy ,
    AsyncIAdviseSink_Begin_OnViewChange_Proxy ,
    AsyncIAdviseSink_Finish_OnViewChange_Proxy ,
    AsyncIAdviseSink_Begin_OnRename_Proxy ,
    AsyncIAdviseSink_Finish_OnRename_Proxy ,
    AsyncIAdviseSink_Begin_OnSave_Proxy ,
    AsyncIAdviseSink_Finish_OnSave_Proxy ,
    AsyncIAdviseSink_Begin_OnClose_Proxy ,
    AsyncIAdviseSink_Finish_OnClose_Proxy
};

CInterfaceStubVtbl _AsyncIAdviseSinkStubVtbl =
{
    &IID_AsyncIAdviseSink,
    &AsyncIAdviseSink_ServerInfo,
    13,
    0, /* pure interpreted */
    CStdAsyncStubBuffer_METHODS
};


/* Object interface: IAdviseSink2, ver. 0.0,
   GUID={0x00000125,0x0000,0x0000,{0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x46}} */

#pragma code_seg(".orpc")
static const unsigned short IAdviseSink2_FormatStringOffsetTable[] =
    {
    3948,
    3990,
    774,
    4032,
    4062,
    4422
    };

static const MIDL_STUBLESS_PROXY_INFO IAdviseSink2_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &IAdviseSink2_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const STUB_THUNK IAdviseSink2_StubThunkTable[] = 
    {
    IAdviseSink_RemoteOnDataChange_Thunk,
    IAdviseSink_RemoteOnViewChange_Thunk,
    IAdviseSink_RemoteOnRename_Thunk,
    IAdviseSink_RemoteOnSave_Thunk,
    IAdviseSink_RemoteOnClose_Thunk,
    IAdviseSink2_RemoteOnLinkSrcChange_Thunk
    };

static const MIDL_SERVER_INFO IAdviseSink2_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &IAdviseSink2_FormatStringOffsetTable[-3],
    &IAdviseSink2_StubThunkTable[-3],
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(9) _IAdviseSink2ProxyVtbl = 
{
    &IAdviseSink2_ProxyInfo,
    &IID_IAdviseSink2,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    IAdviseSink_OnDataChange_Proxy ,
    IAdviseSink_OnViewChange_Proxy ,
    IAdviseSink_OnRename_Proxy ,
    IAdviseSink_OnSave_Proxy ,
    IAdviseSink_OnClose_Proxy ,
    IAdviseSink2_OnLinkSrcChange_Proxy
};

const CInterfaceStubVtbl _IAdviseSink2StubVtbl =
{
    &IID_IAdviseSink2,
    &IAdviseSink2_ServerInfo,
    9,
    0, /* pure interpreted */
    CStdStubBuffer_METHODS
};


/* Object interface: AsyncIAdviseSink2, ver. 0.0,
   GUID={0x00000151,0x0000,0x0000,{0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x46}} */

#pragma code_seg(".orpc")
static const unsigned short AsyncIAdviseSink2_FormatStringOffsetTable[] =
    {
    4092,
    4134,
    4164,
    4206,
    4236,
    4272,
    4302,
    4332,
    4362,
    4392,
    4458,
    4494
    };

static const MIDL_STUBLESS_PROXY_INFO AsyncIAdviseSink2_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &AsyncIAdviseSink2_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const STUB_THUNK AsyncIAdviseSink2_StubThunkTable[] = 
    {
    AsyncIAdviseSink_Begin_RemoteOnDataChange_Thunk,
    AsyncIAdviseSink_Finish_RemoteOnDataChange_Thunk,
    AsyncIAdviseSink_Begin_RemoteOnViewChange_Thunk,
    AsyncIAdviseSink_Finish_RemoteOnViewChange_Thunk,
    AsyncIAdviseSink_Begin_RemoteOnRename_Thunk,
    AsyncIAdviseSink_Finish_RemoteOnRename_Thunk,
    AsyncIAdviseSink_Begin_RemoteOnSave_Thunk,
    AsyncIAdviseSink_Finish_RemoteOnSave_Thunk,
    AsyncIAdviseSink_Begin_RemoteOnClose_Thunk,
    AsyncIAdviseSink_Finish_RemoteOnClose_Thunk,
    AsyncIAdviseSink2_Begin_RemoteOnLinkSrcChange_Thunk,
    AsyncIAdviseSink2_Finish_RemoteOnLinkSrcChange_Thunk
    };

static const MIDL_SERVER_INFO AsyncIAdviseSink2_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &AsyncIAdviseSink2_FormatStringOffsetTable[-3],
    &AsyncIAdviseSink2_StubThunkTable[-3],
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(15) _AsyncIAdviseSink2ProxyVtbl = 
{
    &AsyncIAdviseSink2_ProxyInfo,
    &IID_AsyncIAdviseSink2,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    AsyncIAdviseSink_Begin_OnDataChange_Proxy ,
    AsyncIAdviseSink_Finish_OnDataChange_Proxy ,
    AsyncIAdviseSink_Begin_OnViewChange_Proxy ,
    AsyncIAdviseSink_Finish_OnViewChange_Proxy ,
    AsyncIAdviseSink_Begin_OnRename_Proxy ,
    AsyncIAdviseSink_Finish_OnRename_Proxy ,
    AsyncIAdviseSink_Begin_OnSave_Proxy ,
    AsyncIAdviseSink_Finish_OnSave_Proxy ,
    AsyncIAdviseSink_Begin_OnClose_Proxy ,
    AsyncIAdviseSink_Finish_OnClose_Proxy ,
    AsyncIAdviseSink2_Begin_OnLinkSrcChange_Proxy ,
    AsyncIAdviseSink2_Finish_OnLinkSrcChange_Proxy
};

CInterfaceStubVtbl _AsyncIAdviseSink2StubVtbl =
{
    &IID_AsyncIAdviseSink2,
    &AsyncIAdviseSink2_ServerInfo,
    15,
    0, /* pure interpreted */
    CStdAsyncStubBuffer_METHODS
};


/* Object interface: IDataObject, ver. 0.0,
   GUID={0x0000010e,0x0000,0x0000,{0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x46}} */

#pragma code_seg(".orpc")
static const unsigned short IDataObject_FormatStringOffsetTable[] =
    {
    4524,
    4566,
    4608,
    4644,
    4686,
    4734,
    4776,
    4830,
    4866
    };

static const MIDL_STUBLESS_PROXY_INFO IDataObject_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &IDataObject_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const STUB_THUNK IDataObject_StubThunkTable[] = 
    {
    IDataObject_RemoteGetData_Thunk,
    IDataObject_RemoteGetDataHere_Thunk,
    0,
    0,
    IDataObject_RemoteSetData_Thunk,
    0,
    0,
    0,
    0
    };

static const MIDL_SERVER_INFO IDataObject_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &IDataObject_FormatStringOffsetTable[-3],
    &IDataObject_StubThunkTable[-3],
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(12) _IDataObjectProxyVtbl = 
{
    &IDataObject_ProxyInfo,
    &IID_IDataObject,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    IDataObject_GetData_Proxy ,
    IDataObject_GetDataHere_Proxy ,
    (void *) (INT_PTR) -1 /* IDataObject::QueryGetData */ ,
    (void *) (INT_PTR) -1 /* IDataObject::GetCanonicalFormatEtc */ ,
    IDataObject_SetData_Proxy ,
    (void *) (INT_PTR) -1 /* IDataObject::EnumFormatEtc */ ,
    (void *) (INT_PTR) -1 /* IDataObject::DAdvise */ ,
    (void *) (INT_PTR) -1 /* IDataObject::DUnadvise */ ,
    (void *) (INT_PTR) -1 /* IDataObject::EnumDAdvise */
};

const CInterfaceStubVtbl _IDataObjectStubVtbl =
{
    &IID_IDataObject,
    &IDataObject_ServerInfo,
    12,
    0, /* pure interpreted */
    CStdStubBuffer_METHODS
};


/* Object interface: IDataAdviseHolder, ver. 0.0,
   GUID={0x00000110,0x0000,0x0000,{0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x46}} */


/* Object interface: IMessageFilter, ver. 0.0,
   GUID={0x00000016,0x0000,0x0000,{0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x46}} */


/* Object interface: IRpcChannelBuffer, ver. 0.0,
   GUID={0xD5F56B60,0x593B,0x101A,{0xB5,0x69,0x08,0x00,0x2B,0x2D,0xBF,0x7A}} */


/* Object interface: IRpcChannelBuffer2, ver. 0.0,
   GUID={0x594f31d0,0x7f19,0x11d0,{0xb1,0x94,0x00,0xa0,0xc9,0x0d,0xc8,0xbf}} */


/* Object interface: IAsyncRpcChannelBuffer, ver. 0.0,
   GUID={0xa5029fb6,0x3c34,0x11d1,{0x9c,0x99,0x00,0xc0,0x4f,0xb9,0x98,0xaa}} */


/* Object interface: IRpcChannelBuffer3, ver. 0.0,
   GUID={0x25B15600,0x0115,0x11d0,{0xBF,0x0D,0x00,0xAA,0x00,0xB8,0xDF,0xD2}} */


/* Object interface: IRpcProxyBuffer, ver. 0.0,
   GUID={0xD5F56A34,0x593B,0x101A,{0xB5,0x69,0x08,0x00,0x2B,0x2D,0xBF,0x7A}} */


/* Object interface: IRpcStubBuffer, ver. 0.0,
   GUID={0xD5F56AFC,0x593B,0x101A,{0xB5,0x69,0x08,0x00,0x2B,0x2D,0xBF,0x7A}} */


/* Object interface: IPSFactoryBuffer, ver. 0.0,
   GUID={0xD5F569D0,0x593B,0x101A,{0xB5,0x69,0x08,0x00,0x2B,0x2D,0xBF,0x7A}} */


/* Standard interface: __MIDL_itf_objidl_0049, ver. 0.0,
   GUID={0x00000000,0x0000,0x0000,{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}} */


/* Object interface: IChannelHook, ver. 0.0,
   GUID={0x1008c4a0,0x7613,0x11cf,{0x9a,0xf1,0x00,0x20,0xaf,0x6e,0x72,0xf4}} */


/* Standard interface: __MIDL_itf_objidl_0050, ver. 0.0,
   GUID={0x00000000,0x0000,0x0000,{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}} */


/* Object interface: IClientSecurity, ver. 0.0,
   GUID={0x0000013D,0x0000,0x0000,{0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x46}} */


/* Object interface: IServerSecurity, ver. 0.0,
   GUID={0x0000013E,0x0000,0x0000,{0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x46}} */


/* Object interface: IClassActivator, ver. 0.0,
   GUID={0x00000140,0x0000,0x0000,{0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x46}} */

#pragma code_seg(".orpc")
static const unsigned short IClassActivator_FormatStringOffsetTable[] =
    {
    4902
    };

static const MIDL_STUBLESS_PROXY_INFO IClassActivator_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &IClassActivator_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IClassActivator_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &IClassActivator_FormatStringOffsetTable[-3],
    0,
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(4) _IClassActivatorProxyVtbl = 
{
    &IClassActivator_ProxyInfo,
    &IID_IClassActivator,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    (void *) (INT_PTR) -1 /* IClassActivator::GetClassObject */
};

const CInterfaceStubVtbl _IClassActivatorStubVtbl =
{
    &IID_IClassActivator,
    &IClassActivator_ServerInfo,
    4,
    0, /* pure interpreted */
    CStdStubBuffer_METHODS
};


/* Object interface: IRpcOptions, ver. 0.0,
   GUID={0x00000144,0x0000,0x0000,{0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x46}} */


/* Standard interface: __MIDL_itf_objidl_0054, ver. 0.0,
   GUID={0x00000000,0x0000,0x0000,{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}} */


/* Object interface: IFillLockBytes, ver. 0.0,
   GUID={0x99caf010,0x415e,0x11cf,{0x88,0x14,0x00,0xaa,0x00,0xb5,0x69,0xf5}} */

#pragma code_seg(".orpc")
static const unsigned short IFillLockBytes_FormatStringOffsetTable[] =
    {
    4962,
    3552,
    5010,
    5046
    };

static const MIDL_STUBLESS_PROXY_INFO IFillLockBytes_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &IFillLockBytes_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const STUB_THUNK IFillLockBytes_StubThunkTable[] = 
    {
    IFillLockBytes_RemoteFillAppend_Thunk,
    IFillLockBytes_RemoteFillAt_Thunk,
    0,
    0
    };

static const MIDL_SERVER_INFO IFillLockBytes_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &IFillLockBytes_FormatStringOffsetTable[-3],
    &IFillLockBytes_StubThunkTable[-3],
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(7) _IFillLockBytesProxyVtbl = 
{
    &IFillLockBytes_ProxyInfo,
    &IID_IFillLockBytes,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    IFillLockBytes_FillAppend_Proxy ,
    IFillLockBytes_FillAt_Proxy ,
    (void *) (INT_PTR) -1 /* IFillLockBytes::SetFillSize */ ,
    (void *) (INT_PTR) -1 /* IFillLockBytes::Terminate */
};

const CInterfaceStubVtbl _IFillLockBytesStubVtbl =
{
    &IID_IFillLockBytes,
    &IFillLockBytes_ServerInfo,
    7,
    0, /* pure interpreted */
    CStdStubBuffer_METHODS
};


/* Object interface: IProgressNotify, ver. 0.0,
   GUID={0xa9d758a0,0x4617,0x11cf,{0x95,0xfc,0x00,0xaa,0x00,0x68,0x0d,0xb4}} */

#pragma code_seg(".orpc")
static const unsigned short IProgressNotify_FormatStringOffsetTable[] =
    {
    5082
    };

static const MIDL_STUBLESS_PROXY_INFO IProgressNotify_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &IProgressNotify_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IProgressNotify_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &IProgressNotify_FormatStringOffsetTable[-3],
    0,
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(4) _IProgressNotifyProxyVtbl = 
{
    &IProgressNotify_ProxyInfo,
    &IID_IProgressNotify,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    (void *) (INT_PTR) -1 /* IProgressNotify::OnProgress */
};

const CInterfaceStubVtbl _IProgressNotifyStubVtbl =
{
    &IID_IProgressNotify,
    &IProgressNotify_ServerInfo,
    4,
    0, /* pure interpreted */
    CStdStubBuffer_METHODS
};


/* Object interface: ILayoutStorage, ver. 0.0,
   GUID={0x0e6d4d90,0x6738,0x11cf,{0x96,0x08,0x00,0xaa,0x00,0x68,0x0d,0xb4}} */


/* Object interface: IBlockingLock, ver. 0.0,
   GUID={0x30f3d47a,0x6447,0x11d1,{0x8e,0x3c,0x00,0xc0,0x4f,0xb9,0x38,0x6d}} */

#pragma code_seg(".orpc")
static const unsigned short IBlockingLock_FormatStringOffsetTable[] =
    {
    5136,
    972
    };

static const MIDL_STUBLESS_PROXY_INFO IBlockingLock_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &IBlockingLock_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IBlockingLock_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &IBlockingLock_FormatStringOffsetTable[-3],
    0,
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(5) _IBlockingLockProxyVtbl = 
{
    &IBlockingLock_ProxyInfo,
    &IID_IBlockingLock,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    (void *) (INT_PTR) -1 /* IBlockingLock::Lock */ ,
    (void *) (INT_PTR) -1 /* IBlockingLock::Unlock */
};

const CInterfaceStubVtbl _IBlockingLockStubVtbl =
{
    &IID_IBlockingLock,
    &IBlockingLock_ServerInfo,
    5,
    0, /* pure interpreted */
    CStdStubBuffer_METHODS
};


/* Object interface: ITimeAndNoticeControl, ver. 0.0,
   GUID={0xbc0bf6ae,0x8878,0x11d1,{0x83,0xe9,0x00,0xc0,0x4f,0xc2,0xc6,0xd4}} */

#pragma code_seg(".orpc")
static const unsigned short ITimeAndNoticeControl_FormatStringOffsetTable[] =
    {
    5172
    };

static const MIDL_STUBLESS_PROXY_INFO ITimeAndNoticeControl_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &ITimeAndNoticeControl_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO ITimeAndNoticeControl_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &ITimeAndNoticeControl_FormatStringOffsetTable[-3],
    0,
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(4) _ITimeAndNoticeControlProxyVtbl = 
{
    &ITimeAndNoticeControl_ProxyInfo,
    &IID_ITimeAndNoticeControl,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    (void *) (INT_PTR) -1 /* ITimeAndNoticeControl::SuppressChanges */
};

const CInterfaceStubVtbl _ITimeAndNoticeControlStubVtbl =
{
    &IID_ITimeAndNoticeControl,
    &ITimeAndNoticeControl_ServerInfo,
    4,
    0, /* pure interpreted */
    CStdStubBuffer_METHODS
};


/* Object interface: IOplockStorage, ver. 0.0,
   GUID={0x8d19c834,0x8879,0x11d1,{0x83,0xe9,0x00,0xc0,0x4f,0xc2,0xc6,0xd4}} */

#pragma code_seg(".orpc")
static const unsigned short IOplockStorage_FormatStringOffsetTable[] =
    {
    5214,
    5280
    };

static const MIDL_STUBLESS_PROXY_INFO IOplockStorage_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &IOplockStorage_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IOplockStorage_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &IOplockStorage_FormatStringOffsetTable[-3],
    0,
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(5) _IOplockStorageProxyVtbl = 
{
    &IOplockStorage_ProxyInfo,
    &IID_IOplockStorage,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    (void *) (INT_PTR) -1 /* IOplockStorage::CreateStorageEx */ ,
    (void *) (INT_PTR) -1 /* IOplockStorage::OpenStorageEx */
};

const CInterfaceStubVtbl _IOplockStorageStubVtbl =
{
    &IID_IOplockStorage,
    &IOplockStorage_ServerInfo,
    5,
    0, /* pure interpreted */
    CStdStubBuffer_METHODS
};


/* Object interface: ISurrogate, ver. 1.0,
   GUID={0x00000022,0x0000,0x0000,{0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x46}} */

#pragma code_seg(".orpc")
static const unsigned short ISurrogate_FormatStringOffsetTable[] =
    {
    5346,
    972
    };

static const MIDL_STUBLESS_PROXY_INFO ISurrogate_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &ISurrogate_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO ISurrogate_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &ISurrogate_FormatStringOffsetTable[-3],
    0,
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(5) _ISurrogateProxyVtbl = 
{
    &ISurrogate_ProxyInfo,
    &IID_ISurrogate,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    (void *) (INT_PTR) -1 /* ISurrogate::LoadDllServer */ ,
    (void *) (INT_PTR) -1 /* ISurrogate::FreeSurrogate */
};

const CInterfaceStubVtbl _ISurrogateStubVtbl =
{
    &IID_ISurrogate,
    &ISurrogate_ServerInfo,
    5,
    0, /* pure interpreted */
    CStdStubBuffer_METHODS
};


/* Object interface: IGlobalInterfaceTable, ver. 0.0,
   GUID={0x00000146,0x0000,0x0000,{0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x46}} */


/* Object interface: IDirectWriterLock, ver. 0.0,
   GUID={0x0e6d4d92,0x6738,0x11cf,{0x96,0x08,0x00,0xaa,0x00,0x68,0x0d,0xb4}} */

#pragma code_seg(".orpc")
static const unsigned short IDirectWriterLock_FormatStringOffsetTable[] =
    {
    5136,
    972,
    84
    };

static const MIDL_STUBLESS_PROXY_INFO IDirectWriterLock_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &IDirectWriterLock_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IDirectWriterLock_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &IDirectWriterLock_FormatStringOffsetTable[-3],
    0,
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(6) _IDirectWriterLockProxyVtbl = 
{
    &IDirectWriterLock_ProxyInfo,
    &IID_IDirectWriterLock,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    (void *) (INT_PTR) -1 /* IDirectWriterLock::WaitForWriteAccess */ ,
    (void *) (INT_PTR) -1 /* IDirectWriterLock::ReleaseWriteAccess */ ,
    (void *) (INT_PTR) -1 /* IDirectWriterLock::HaveWriteAccess */
};

const CInterfaceStubVtbl _IDirectWriterLockStubVtbl =
{
    &IID_IDirectWriterLock,
    &IDirectWriterLock_ServerInfo,
    6,
    0, /* pure interpreted */
    CStdStubBuffer_METHODS
};


/* Object interface: ISynchronize, ver. 0.0,
   GUID={0x00000030,0x0000,0x0000,{0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x46}} */

#pragma code_seg(".orpc")
static const unsigned short ISynchronize_FormatStringOffsetTable[] =
    {
    5172,
    972,
    84
    };

static const MIDL_STUBLESS_PROXY_INFO ISynchronize_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &ISynchronize_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO ISynchronize_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &ISynchronize_FormatStringOffsetTable[-3],
    0,
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(6) _ISynchronizeProxyVtbl = 
{
    &ISynchronize_ProxyInfo,
    &IID_ISynchronize,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    (void *) (INT_PTR) -1 /* ISynchronize::Wait */ ,
    (void *) (INT_PTR) -1 /* ISynchronize::Signal */ ,
    (void *) (INT_PTR) -1 /* ISynchronize::Reset */
};

const CInterfaceStubVtbl _ISynchronizeStubVtbl =
{
    &IID_ISynchronize,
    &ISynchronize_ServerInfo,
    6,
    0, /* pure interpreted */
    CStdStubBuffer_METHODS
};


/* Object interface: ISynchronizeHandle, ver. 0.0,
   GUID={0x00000031,0x0000,0x0000,{0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x46}} */


/* Object interface: ISynchronizeEvent, ver. 0.0,
   GUID={0x00000032,0x0000,0x0000,{0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x46}} */


/* Object interface: ISynchronizeContainer, ver. 0.0,
   GUID={0x00000033,0x0000,0x0000,{0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x46}} */


/* Object interface: ISynchronizeMutex, ver. 0.0,
   GUID={0x00000025,0x0000,0x0000,{0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x46}} */


/* Object interface: ICancelMethodCalls, ver. 0.0,
   GUID={0x00000029,0x0000,0x0000,{0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x46}} */


/* Object interface: IAsyncManager, ver. 0.0,
   GUID={0x0000002A,0x0000,0x0000,{0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x46}} */


/* Object interface: ICallFactory, ver. 0.0,
   GUID={0x1c733a30,0x2a1c,0x11ce,{0xad,0xe5,0x00,0xaa,0x00,0x44,0x77,0x3d}} */


/* Object interface: IRpcHelper, ver. 0.0,
   GUID={0x00000149,0x0000,0x0000,{0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x46}} */


/* Object interface: IReleaseMarshalBuffers, ver. 0.0,
   GUID={0xeb0cb9e8,0x7996,0x11d2,{0x87,0x2e,0x00,0x00,0xf8,0x08,0x08,0x59}} */


/* Object interface: IWaitMultiple, ver. 0.0,
   GUID={0x0000002B,0x0000,0x0000,{0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x46}} */


/* Object interface: IUrlMon, ver. 0.0,
   GUID={0x00000026,0x0000,0x0000,{0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x46}} */

#pragma code_seg(".orpc")
static const unsigned short IUrlMon_FormatStringOffsetTable[] =
    {
    5382
    };

static const MIDL_STUBLESS_PROXY_INFO IUrlMon_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &IUrlMon_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IUrlMon_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &IUrlMon_FormatStringOffsetTable[-3],
    0,
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(4) _IUrlMonProxyVtbl = 
{
    &IUrlMon_ProxyInfo,
    &IID_IUrlMon,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    (void *) (INT_PTR) -1 /* IUrlMon::AsyncGetClassBits */
};

const CInterfaceStubVtbl _IUrlMonStubVtbl =
{
    &IID_IUrlMon,
    &IUrlMon_ServerInfo,
    4,
    0, /* pure interpreted */
    CStdStubBuffer_METHODS
};


/* Object interface: IForegroundTransfer, ver. 0.0,
   GUID={0x00000145,0x0000,0x0000,{0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x46}} */


/* Object interface: IPipeByte, ver. 0.0,
   GUID={0xDB2F3ACA,0x2F86,0x11d1,{0x8E,0x04,0x00,0xC0,0x4F,0xB9,0x98,0x9A}} */

#pragma code_seg(".orpc")
static const unsigned short IPipeByte_FormatStringOffsetTable[] =
    {
    1932,
    5472
    };

static const MIDL_STUBLESS_PROXY_INFO IPipeByte_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &IPipeByte_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IPipeByte_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &IPipeByte_FormatStringOffsetTable[-3],
    0,
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(5) _IPipeByteProxyVtbl = 
{
    &IPipeByte_ProxyInfo,
    &IID_IPipeByte,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    (void *) (INT_PTR) -1 /* IPipeByte::Pull */ ,
    (void *) (INT_PTR) -1 /* IPipeByte::Push */
};

const CInterfaceStubVtbl _IPipeByteStubVtbl =
{
    &IID_IPipeByte,
    &IPipeByte_ServerInfo,
    5,
    0, /* pure interpreted */
    CStdStubBuffer_METHODS
};


/* Object interface: AsyncIPipeByte, ver. 0.0,
   GUID={0xDB2F3ACB,0x2F86,0x11d1,{0x8E,0x04,0x00,0xC0,0x4F,0xB9,0x98,0x9A}} */

#pragma code_seg(".orpc")
static const unsigned short AsyncIPipeByte_FormatStringOffsetTable[] =
    {
    5514,
    5550,
    5592,
    4206
    };

static const MIDL_STUBLESS_PROXY_INFO AsyncIPipeByte_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &AsyncIPipeByte_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO AsyncIPipeByte_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &AsyncIPipeByte_FormatStringOffsetTable[-3],
    0,
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(7) _AsyncIPipeByteProxyVtbl = 
{
    &AsyncIPipeByte_ProxyInfo,
    &IID_AsyncIPipeByte,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    (void *) (INT_PTR) -1 /* AsyncIPipeByte::Begin_Pull */ ,
    (void *) (INT_PTR) -1 /* AsyncIPipeByte::Finish_Pull */ ,
    (void *) (INT_PTR) -1 /* AsyncIPipeByte::Begin_Push */ ,
    (void *) (INT_PTR) -1 /* AsyncIPipeByte::Finish_Push */
};

CInterfaceStubVtbl _AsyncIPipeByteStubVtbl =
{
    &IID_AsyncIPipeByte,
    &AsyncIPipeByte_ServerInfo,
    7,
    0, /* pure interpreted */
    CStdAsyncStubBuffer_METHODS
};


/* Object interface: IPipeLong, ver. 0.0,
   GUID={0xDB2F3ACC,0x2F86,0x11d1,{0x8E,0x04,0x00,0xC0,0x4F,0xB9,0x98,0x9A}} */

#pragma code_seg(".orpc")
static const unsigned short IPipeLong_FormatStringOffsetTable[] =
    {
    5634,
    5682
    };

static const MIDL_STUBLESS_PROXY_INFO IPipeLong_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &IPipeLong_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IPipeLong_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &IPipeLong_FormatStringOffsetTable[-3],
    0,
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(5) _IPipeLongProxyVtbl = 
{
    &IPipeLong_ProxyInfo,
    &IID_IPipeLong,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    (void *) (INT_PTR) -1 /* IPipeLong::Pull */ ,
    (void *) (INT_PTR) -1 /* IPipeLong::Push */
};

const CInterfaceStubVtbl _IPipeLongStubVtbl =
{
    &IID_IPipeLong,
    &IPipeLong_ServerInfo,
    5,
    0, /* pure interpreted */
    CStdStubBuffer_METHODS
};


/* Object interface: AsyncIPipeLong, ver. 0.0,
   GUID={0xDB2F3ACD,0x2F86,0x11d1,{0x8E,0x04,0x00,0xC0,0x4F,0xB9,0x98,0x9A}} */

#pragma code_seg(".orpc")
static const unsigned short AsyncIPipeLong_FormatStringOffsetTable[] =
    {
    5514,
    5724,
    5766,
    4206
    };

static const MIDL_STUBLESS_PROXY_INFO AsyncIPipeLong_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &AsyncIPipeLong_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO AsyncIPipeLong_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &AsyncIPipeLong_FormatStringOffsetTable[-3],
    0,
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(7) _AsyncIPipeLongProxyVtbl = 
{
    &AsyncIPipeLong_ProxyInfo,
    &IID_AsyncIPipeLong,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    (void *) (INT_PTR) -1 /* AsyncIPipeLong::Begin_Pull */ ,
    (void *) (INT_PTR) -1 /* AsyncIPipeLong::Finish_Pull */ ,
    (void *) (INT_PTR) -1 /* AsyncIPipeLong::Begin_Push */ ,
    (void *) (INT_PTR) -1 /* AsyncIPipeLong::Finish_Push */
};

CInterfaceStubVtbl _AsyncIPipeLongStubVtbl =
{
    &IID_AsyncIPipeLong,
    &AsyncIPipeLong_ServerInfo,
    7,
    0, /* pure interpreted */
    CStdAsyncStubBuffer_METHODS
};


/* Object interface: IPipeDouble, ver. 0.0,
   GUID={0xDB2F3ACE,0x2F86,0x11d1,{0x8E,0x04,0x00,0xC0,0x4F,0xB9,0x98,0x9A}} */

#pragma code_seg(".orpc")
static const unsigned short IPipeDouble_FormatStringOffsetTable[] =
    {
    5808,
    5856
    };

static const MIDL_STUBLESS_PROXY_INFO IPipeDouble_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &IPipeDouble_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IPipeDouble_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &IPipeDouble_FormatStringOffsetTable[-3],
    0,
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(5) _IPipeDoubleProxyVtbl = 
{
    &IPipeDouble_ProxyInfo,
    &IID_IPipeDouble,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    (void *) (INT_PTR) -1 /* IPipeDouble::Pull */ ,
    (void *) (INT_PTR) -1 /* IPipeDouble::Push */
};

const CInterfaceStubVtbl _IPipeDoubleStubVtbl =
{
    &IID_IPipeDouble,
    &IPipeDouble_ServerInfo,
    5,
    0, /* pure interpreted */
    CStdStubBuffer_METHODS
};


/* Object interface: AsyncIPipeDouble, ver. 0.0,
   GUID={0xDB2F3ACF,0x2F86,0x11d1,{0x8E,0x04,0x00,0xC0,0x4F,0xB9,0x98,0x9A}} */

#pragma code_seg(".orpc")
static const unsigned short AsyncIPipeDouble_FormatStringOffsetTable[] =
    {
    5514,
    5898,
    5940,
    4206
    };

static const MIDL_STUBLESS_PROXY_INFO AsyncIPipeDouble_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &AsyncIPipeDouble_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO AsyncIPipeDouble_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &AsyncIPipeDouble_FormatStringOffsetTable[-3],
    0,
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(7) _AsyncIPipeDoubleProxyVtbl = 
{
    &AsyncIPipeDouble_ProxyInfo,
    &IID_AsyncIPipeDouble,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    (void *) (INT_PTR) -1 /* AsyncIPipeDouble::Begin_Pull */ ,
    (void *) (INT_PTR) -1 /* AsyncIPipeDouble::Finish_Pull */ ,
    (void *) (INT_PTR) -1 /* AsyncIPipeDouble::Begin_Push */ ,
    (void *) (INT_PTR) -1 /* AsyncIPipeDouble::Finish_Push */
};

CInterfaceStubVtbl _AsyncIPipeDoubleStubVtbl =
{
    &IID_AsyncIPipeDouble,
    &AsyncIPipeDouble_ServerInfo,
    7,
    0, /* pure interpreted */
    CStdAsyncStubBuffer_METHODS
};


/* Object interface: IThumbnailExtractor, ver. 0.0,
   GUID={0x969dc708,0x5c76,0x11d1,{0x8d,0x86,0x00,0x00,0xf8,0x04,0xb0,0x57}} */

#pragma code_seg(".orpc")
static const unsigned short IThumbnailExtractor_FormatStringOffsetTable[] =
    {
    5982,
    6048
    };

static const MIDL_STUBLESS_PROXY_INFO IThumbnailExtractor_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &IThumbnailExtractor_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IThumbnailExtractor_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &IThumbnailExtractor_FormatStringOffsetTable[-3],
    0,
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(5) _IThumbnailExtractorProxyVtbl = 
{
    &IThumbnailExtractor_ProxyInfo,
    &IID_IThumbnailExtractor,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    (void *) (INT_PTR) -1 /* IThumbnailExtractor::ExtractThumbnail */ ,
    (void *) (INT_PTR) -1 /* IThumbnailExtractor::OnFileUpdated */
};

const CInterfaceStubVtbl _IThumbnailExtractorStubVtbl =
{
    &IID_IThumbnailExtractor,
    &IThumbnailExtractor_ServerInfo,
    5,
    0, /* pure interpreted */
    CStdStubBuffer_METHODS
};


/* Object interface: IDummyHICONIncluder, ver. 0.0,
   GUID={0x947990de,0xcc28,0x11d2,{0xa0,0xf7,0x00,0x80,0x5f,0x85,0x8f,0xb1}} */

#pragma code_seg(".orpc")
static const unsigned short IDummyHICONIncluder_FormatStringOffsetTable[] =
    {
    6084
    };

static const MIDL_STUBLESS_PROXY_INFO IDummyHICONIncluder_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &IDummyHICONIncluder_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IDummyHICONIncluder_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &IDummyHICONIncluder_FormatStringOffsetTable[-3],
    0,
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(4) _IDummyHICONIncluderProxyVtbl = 
{
    &IDummyHICONIncluder_ProxyInfo,
    &IID_IDummyHICONIncluder,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    (void *) (INT_PTR) -1 /* IDummyHICONIncluder::Dummy */
};

const CInterfaceStubVtbl _IDummyHICONIncluderStubVtbl =
{
    &IID_IDummyHICONIncluder,
    &IDummyHICONIncluder_ServerInfo,
    4,
    0, /* pure interpreted */
    CStdStubBuffer_METHODS
};


/* Standard interface: __MIDL_itf_objidl_0081, ver. 0.0,
   GUID={0x00000000,0x0000,0x0000,{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}} */

static const MIDL_STUB_DESC Object_StubDesc = 
    {
    0,
    NdrOleAllocate,
    NdrOleFree,
    0,
    0,
    0,
    ExprEvalRoutines,
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

const CInterfaceProxyVtbl * _objidl_ProxyVtblList[] = 
{
    ( CInterfaceProxyVtbl *) &_IEnumUnknownProxyVtbl,
    ( CInterfaceProxyVtbl *) &_IEnumStringProxyVtbl,
    ( CInterfaceProxyVtbl *) &_IEnumMonikerProxyVtbl,
    ( CInterfaceProxyVtbl *) &_IEnumFORMATETCProxyVtbl,
    ( CInterfaceProxyVtbl *) &_IEnumSTATDATAProxyVtbl,
    ( CInterfaceProxyVtbl *) &_IThumbnailExtractorProxyVtbl,
    ( CInterfaceProxyVtbl *) &_IPersistStreamProxyVtbl,
    ( CInterfaceProxyVtbl *) &_ILockBytesProxyVtbl,
    ( CInterfaceProxyVtbl *) &_IPersistStorageProxyVtbl,
    ( CInterfaceProxyVtbl *) &_IStorageProxyVtbl,
    ( CInterfaceProxyVtbl *) &_IPersistFileProxyVtbl,
    ( CInterfaceProxyVtbl *) &_IStreamProxyVtbl,
    ( CInterfaceProxyVtbl *) &_IPersistProxyVtbl,
    ( CInterfaceProxyVtbl *) &_IEnumSTATSTGProxyVtbl,
    ( CInterfaceProxyVtbl *) &_IBindCtxProxyVtbl,
    ( CInterfaceProxyVtbl *) &_IDataObjectProxyVtbl,
    ( CInterfaceProxyVtbl *) &_IMonikerProxyVtbl,
    ( CInterfaceProxyVtbl *) &_IAdviseSinkProxyVtbl,
    ( CInterfaceProxyVtbl *) &_IRunningObjectTableProxyVtbl,
    ( CInterfaceProxyVtbl *) &_IFillLockBytesProxyVtbl,
    ( CInterfaceProxyVtbl *) &_IRootStorageProxyVtbl,
    ( CInterfaceProxyVtbl *) &_ISurrogateProxyVtbl,
    ( CInterfaceProxyVtbl *) &_IAdviseSink2ProxyVtbl,
    ( CInterfaceProxyVtbl *) &_IUrlMonProxyVtbl,
    ( CInterfaceProxyVtbl *) &_IRunnableObjectProxyVtbl,
    ( CInterfaceProxyVtbl *) &_ISynchronizeProxyVtbl,
    ( CInterfaceProxyVtbl *) &_ISequentialStreamProxyVtbl,
    ( CInterfaceProxyVtbl *) &_IOplockStorageProxyVtbl,
    ( CInterfaceProxyVtbl *) &_IClassActivatorProxyVtbl,
    ( CInterfaceProxyVtbl *) &_AsyncIAdviseSinkProxyVtbl,
    ( CInterfaceProxyVtbl *) &_AsyncIAdviseSink2ProxyVtbl,
    ( CInterfaceProxyVtbl *) &_IBlockingLockProxyVtbl,
    ( CInterfaceProxyVtbl *) &_IDirectWriterLockProxyVtbl,
    ( CInterfaceProxyVtbl *) &_IProgressNotifyProxyVtbl,
    ( CInterfaceProxyVtbl *) &_ITimeAndNoticeControlProxyVtbl,
    ( CInterfaceProxyVtbl *) &_IROTDataProxyVtbl,
    ( CInterfaceProxyVtbl *) &_IPipeByteProxyVtbl,
    ( CInterfaceProxyVtbl *) &_AsyncIPipeByteProxyVtbl,
    ( CInterfaceProxyVtbl *) &_IPipeLongProxyVtbl,
    ( CInterfaceProxyVtbl *) &_AsyncIPipeLongProxyVtbl,
    ( CInterfaceProxyVtbl *) &_IPipeDoubleProxyVtbl,
    ( CInterfaceProxyVtbl *) &_AsyncIPipeDoubleProxyVtbl,
    ( CInterfaceProxyVtbl *) &_IDummyHICONIncluderProxyVtbl,
    0
};

const CInterfaceStubVtbl * _objidl_StubVtblList[] = 
{
    ( CInterfaceStubVtbl *) &_IEnumUnknownStubVtbl,
    ( CInterfaceStubVtbl *) &_IEnumStringStubVtbl,
    ( CInterfaceStubVtbl *) &_IEnumMonikerStubVtbl,
    ( CInterfaceStubVtbl *) &_IEnumFORMATETCStubVtbl,
    ( CInterfaceStubVtbl *) &_IEnumSTATDATAStubVtbl,
    ( CInterfaceStubVtbl *) &_IThumbnailExtractorStubVtbl,
    ( CInterfaceStubVtbl *) &_IPersistStreamStubVtbl,
    ( CInterfaceStubVtbl *) &_ILockBytesStubVtbl,
    ( CInterfaceStubVtbl *) &_IPersistStorageStubVtbl,
    ( CInterfaceStubVtbl *) &_IStorageStubVtbl,
    ( CInterfaceStubVtbl *) &_IPersistFileStubVtbl,
    ( CInterfaceStubVtbl *) &_IStreamStubVtbl,
    ( CInterfaceStubVtbl *) &_IPersistStubVtbl,
    ( CInterfaceStubVtbl *) &_IEnumSTATSTGStubVtbl,
    ( CInterfaceStubVtbl *) &_IBindCtxStubVtbl,
    ( CInterfaceStubVtbl *) &_IDataObjectStubVtbl,
    ( CInterfaceStubVtbl *) &_IMonikerStubVtbl,
    ( CInterfaceStubVtbl *) &_IAdviseSinkStubVtbl,
    ( CInterfaceStubVtbl *) &_IRunningObjectTableStubVtbl,
    ( CInterfaceStubVtbl *) &_IFillLockBytesStubVtbl,
    ( CInterfaceStubVtbl *) &_IRootStorageStubVtbl,
    ( CInterfaceStubVtbl *) &_ISurrogateStubVtbl,
    ( CInterfaceStubVtbl *) &_IAdviseSink2StubVtbl,
    ( CInterfaceStubVtbl *) &_IUrlMonStubVtbl,
    ( CInterfaceStubVtbl *) &_IRunnableObjectStubVtbl,
    ( CInterfaceStubVtbl *) &_ISynchronizeStubVtbl,
    ( CInterfaceStubVtbl *) &_ISequentialStreamStubVtbl,
    ( CInterfaceStubVtbl *) &_IOplockStorageStubVtbl,
    ( CInterfaceStubVtbl *) &_IClassActivatorStubVtbl,
    ( CInterfaceStubVtbl *) &_AsyncIAdviseSinkStubVtbl,
    ( CInterfaceStubVtbl *) &_AsyncIAdviseSink2StubVtbl,
    ( CInterfaceStubVtbl *) &_IBlockingLockStubVtbl,
    ( CInterfaceStubVtbl *) &_IDirectWriterLockStubVtbl,
    ( CInterfaceStubVtbl *) &_IProgressNotifyStubVtbl,
    ( CInterfaceStubVtbl *) &_ITimeAndNoticeControlStubVtbl,
    ( CInterfaceStubVtbl *) &_IROTDataStubVtbl,
    ( CInterfaceStubVtbl *) &_IPipeByteStubVtbl,
    ( CInterfaceStubVtbl *) &_AsyncIPipeByteStubVtbl,
    ( CInterfaceStubVtbl *) &_IPipeLongStubVtbl,
    ( CInterfaceStubVtbl *) &_AsyncIPipeLongStubVtbl,
    ( CInterfaceStubVtbl *) &_IPipeDoubleStubVtbl,
    ( CInterfaceStubVtbl *) &_AsyncIPipeDoubleStubVtbl,
    ( CInterfaceStubVtbl *) &_IDummyHICONIncluderStubVtbl,
    0
};

PCInterfaceName const _objidl_InterfaceNamesList[] = 
{
    "IEnumUnknown",
    "IEnumString",
    "IEnumMoniker",
    "IEnumFORMATETC",
    "IEnumSTATDATA",
    "IThumbnailExtractor",
    "IPersistStream",
    "ILockBytes",
    "IPersistStorage",
    "IStorage",
    "IPersistFile",
    "IStream",
    "IPersist",
    "IEnumSTATSTG",
    "IBindCtx",
    "IDataObject",
    "IMoniker",
    "IAdviseSink",
    "IRunningObjectTable",
    "IFillLockBytes",
    "IRootStorage",
    "ISurrogate",
    "IAdviseSink2",
    "IUrlMon",
    "IRunnableObject",
    "ISynchronize",
    "ISequentialStream",
    "IOplockStorage",
    "IClassActivator",
    "AsyncIAdviseSink",
    "AsyncIAdviseSink2",
    "IBlockingLock",
    "IDirectWriterLock",
    "IProgressNotify",
    "ITimeAndNoticeControl",
    "IROTData",
    "IPipeByte",
    "AsyncIPipeByte",
    "IPipeLong",
    "AsyncIPipeLong",
    "IPipeDouble",
    "AsyncIPipeDouble",
    "IDummyHICONIncluder",
    0
};


#define _objidl_CHECK_IID(n)	IID_GENERIC_CHECK_IID( _objidl, pIID, n)

int __stdcall _objidl_IID_Lookup( const IID * pIID, int * pIndex )
{
    IID_BS_LOOKUP_SETUP

    IID_BS_LOOKUP_INITIAL_TEST( _objidl, 43, 32 )
    IID_BS_LOOKUP_NEXT_TEST( _objidl, 16 )
    IID_BS_LOOKUP_NEXT_TEST( _objidl, 8 )
    IID_BS_LOOKUP_NEXT_TEST( _objidl, 4 )
    IID_BS_LOOKUP_NEXT_TEST( _objidl, 2 )
    IID_BS_LOOKUP_NEXT_TEST( _objidl, 1 )
    IID_BS_LOOKUP_RETURN_RESULT( _objidl, 43, *pIndex )
    
}

static const IID * _AsyncInterfaceTable[] = 
{
    (IID*) 0,
    (IID*) 0,
    (IID*) 0,
    (IID*) 0,
    (IID*) 0,
    (IID*) 0,
    (IID*) 0,
    (IID*) 0,
    (IID*) 0,
    (IID*) 0,
    (IID*) 0,
    (IID*) 0,
    (IID*) 0,
    (IID*) 0,
    (IID*) 0,
    (IID*) 0,
    (IID*) 0,
    (IID*) &IID_AsyncIAdviseSink,
    (IID*) 0,
    (IID*) 0,
    (IID*) 0,
    (IID*) 0,
    (IID*) &IID_AsyncIAdviseSink2,
    (IID*) 0,
    (IID*) 0,
    (IID*) 0,
    (IID*) 0,
    (IID*) 0,
    (IID*) 0,
    (IID*) -1,
    (IID*) -1,
    (IID*) 0,
    (IID*) 0,
    (IID*) 0,
    (IID*) 0,
    (IID*) 0,
    (IID*) &IID_AsyncIPipeByte,
    (IID*) -1,
    (IID*) &IID_AsyncIPipeLong,
    (IID*) -1,
    (IID*) &IID_AsyncIPipeDouble,
    (IID*) -1,
    (IID*) 0,
    (IID*) 0
};

const ExtendedProxyFileInfo objidl_ProxyFileInfo = 
{
    (PCInterfaceProxyVtblList *) & _objidl_ProxyVtblList,
    (PCInterfaceStubVtblList *) & _objidl_StubVtblList,
    (const PCInterfaceName * ) & _objidl_InterfaceNamesList,
    0, // no delegation
    & _objidl_IID_Lookup, 
    43,
    6,
    (const IID**) &_AsyncInterfaceTable[0], /* table of [async_uuid] interfaces */
    0, /* Filler1 */
    0, /* Filler2 */
    0  /* Filler3 */
};


#endif /* !defined(_M_IA64) && !defined(_M_AMD64)*/


#pragma warning( disable: 4049 )  /* more than 64k source lines */

/* this ALWAYS GENERATED file contains the proxy stub code */


 /* File created by MIDL compiler version 6.00.0347 */
/* at Sun May 05 19:07:27 2002
 */
/* Compiler settings for objidl.idl:
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


#include "objidl.h"

#define TYPE_FORMAT_STRING_SIZE   1667                              
#define PROC_FORMAT_STRING_SIZE   6415                              
#define TRANSMIT_AS_TABLE_SIZE    0            
#define WIRE_MARSHAL_TABLE_SIZE   8            

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


extern const MIDL_SERVER_INFO IEnumUnknown_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO IEnumUnknown_ProxyInfo;

/* [call_as] */ HRESULT STDMETHODCALLTYPE IEnumUnknown_RemoteNext_Proxy( 
    IEnumUnknown * This,
    /* [in] */ ULONG celt,
    /* [length_is][size_is][out] */ IUnknown **rgelt,
    /* [out] */ ULONG *pceltFetched)
{
CLIENT_CALL_RETURN _RetVal;

_RetVal = NdrClientCall2(
                  ( PMIDL_STUB_DESC  )&Object_StubDesc,
                  (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[0],
                  ( unsigned char * )This,
                  celt,
                  rgelt,
                  pceltFetched);
return ( HRESULT  )_RetVal.Simple;

}

void __RPC_API
IEnumUnknown_RemoteNext_Thunk(
    PMIDL_STUB_MESSAGE pStubMsg )
{
    #pragma pack(8)
    struct _PARAM_STRUCT
        {
        IEnumUnknown *This;
        ULONG celt;
        char Pad0[4];
        IUnknown **rgelt;
        ULONG *pceltFetched;
        HRESULT _RetVal;
        };
    #pragma pack()
    struct _PARAM_STRUCT * pParamStruct;

    pParamStruct = (struct _PARAM_STRUCT *) pStubMsg->StackTop;
    
    /* Call the server */
    
    pParamStruct->_RetVal = IEnumUnknown_Next_Stub(
                                 (IEnumUnknown *) pParamStruct->This,
                                 pParamStruct->celt,
                                 pParamStruct->rgelt,
                                 pParamStruct->pceltFetched);
    
}


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO IBindCtx_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO IBindCtx_ProxyInfo;

/* [call_as] */ HRESULT STDMETHODCALLTYPE IBindCtx_RemoteSetBindOptions_Proxy( 
    IBindCtx * This,
    /* [in] */ BIND_OPTS2 *pbindopts)
{
CLIENT_CALL_RETURN _RetVal;

_RetVal = NdrClientCall2(
                  ( PMIDL_STUB_DESC  )&Object_StubDesc,
                  (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[234],
                  ( unsigned char * )This,
                  pbindopts);
return ( HRESULT  )_RetVal.Simple;

}

void __RPC_API
IBindCtx_RemoteSetBindOptions_Thunk(
    PMIDL_STUB_MESSAGE pStubMsg )
{
    #pragma pack(8)
    struct _PARAM_STRUCT
        {
        IBindCtx *This;
        BIND_OPTS2 *pbindopts;
        HRESULT _RetVal;
        };
    #pragma pack()
    struct _PARAM_STRUCT * pParamStruct;

    pParamStruct = (struct _PARAM_STRUCT *) pStubMsg->StackTop;
    
    /* Call the server */
    
    pParamStruct->_RetVal = IBindCtx_SetBindOptions_Stub((IBindCtx *) pParamStruct->This,pParamStruct->pbindopts);
    
}

/* [call_as] */ HRESULT STDMETHODCALLTYPE IBindCtx_RemoteGetBindOptions_Proxy( 
    IBindCtx * This,
    /* [out][in] */ BIND_OPTS2 *pbindopts)
{
CLIENT_CALL_RETURN _RetVal;

_RetVal = NdrClientCall2(
                  ( PMIDL_STUB_DESC  )&Object_StubDesc,
                  (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[272],
                  ( unsigned char * )This,
                  pbindopts);
return ( HRESULT  )_RetVal.Simple;

}

void __RPC_API
IBindCtx_RemoteGetBindOptions_Thunk(
    PMIDL_STUB_MESSAGE pStubMsg )
{
    #pragma pack(8)
    struct _PARAM_STRUCT
        {
        IBindCtx *This;
        BIND_OPTS2 *pbindopts;
        HRESULT _RetVal;
        };
    #pragma pack()
    struct _PARAM_STRUCT * pParamStruct;

    pParamStruct = (struct _PARAM_STRUCT *) pStubMsg->StackTop;
    
    /* Call the server */
    
    pParamStruct->_RetVal = IBindCtx_GetBindOptions_Stub((IBindCtx *) pParamStruct->This,pParamStruct->pbindopts);
    
}


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO IEnumMoniker_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO IEnumMoniker_ProxyInfo;

/* [call_as] */ HRESULT STDMETHODCALLTYPE IEnumMoniker_RemoteNext_Proxy( 
    IEnumMoniker * This,
    /* [in] */ ULONG celt,
    /* [length_is][size_is][out] */ IMoniker **rgelt,
    /* [out] */ ULONG *pceltFetched)
{
CLIENT_CALL_RETURN _RetVal;

_RetVal = NdrClientCall2(
                  ( PMIDL_STUB_DESC  )&Object_StubDesc,
                  (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[512],
                  ( unsigned char * )This,
                  celt,
                  rgelt,
                  pceltFetched);
return ( HRESULT  )_RetVal.Simple;

}

void __RPC_API
IEnumMoniker_RemoteNext_Thunk(
    PMIDL_STUB_MESSAGE pStubMsg )
{
    #pragma pack(8)
    struct _PARAM_STRUCT
        {
        IEnumMoniker *This;
        ULONG celt;
        char Pad0[4];
        IMoniker **rgelt;
        ULONG *pceltFetched;
        HRESULT _RetVal;
        };
    #pragma pack()
    struct _PARAM_STRUCT * pParamStruct;

    pParamStruct = (struct _PARAM_STRUCT *) pStubMsg->StackTop;
    
    /* Call the server */
    
    pParamStruct->_RetVal = IEnumMoniker_Next_Stub(
                                 (IEnumMoniker *) pParamStruct->This,
                                 pParamStruct->celt,
                                 pParamStruct->rgelt,
                                 pParamStruct->pceltFetched);
    
}


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO IRunnableObject_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO IRunnableObject_ProxyInfo;

/* [call_as] */ HRESULT STDMETHODCALLTYPE IRunnableObject_RemoteIsRunning_Proxy( 
    IRunnableObject * This)
{
CLIENT_CALL_RETURN _RetVal;

_RetVal = NdrClientCall2(
                  ( PMIDL_STUB_DESC  )&Object_StubDesc,
                  (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[88],
                  ( unsigned char * )This);
return ( HRESULT  )_RetVal.Simple;

}

void __RPC_API
IRunnableObject_RemoteIsRunning_Thunk(
    PMIDL_STUB_MESSAGE pStubMsg )
{
    #pragma pack(8)
    struct _PARAM_STRUCT
        {
        IRunnableObject *This;
        HRESULT _RetVal;
        };
    #pragma pack()
    struct _PARAM_STRUCT * pParamStruct;

    pParamStruct = (struct _PARAM_STRUCT *) pStubMsg->StackTop;
    
    /* Call the server */
    
    pParamStruct->_RetVal = IRunnableObject_IsRunning_Stub((IRunnableObject *) pParamStruct->This);
    
}


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO IRunningObjectTable_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO IRunningObjectTable_ProxyInfo;


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO IPersist_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO IPersist_ProxyInfo;


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO IPersistStream_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO IPersistStream_ProxyInfo;


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO IMoniker_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO IMoniker_ProxyInfo;

/* [call_as] */ HRESULT STDMETHODCALLTYPE IMoniker_RemoteBindToObject_Proxy( 
    IMoniker * This,
    /* [unique][in] */ IBindCtx *pbc,
    /* [unique][in] */ IMoniker *pmkToLeft,
    /* [in] */ REFIID riidResult,
    /* [iid_is][out] */ IUnknown **ppvResult)
{
CLIENT_CALL_RETURN _RetVal;

_RetVal = NdrClientCall2(
                  ( PMIDL_STUB_DESC  )&Object_StubDesc,
                  (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[1174],
                  ( unsigned char * )This,
                  pbc,
                  pmkToLeft,
                  riidResult,
                  ppvResult);
return ( HRESULT  )_RetVal.Simple;

}

void __RPC_API
IMoniker_RemoteBindToObject_Thunk(
    PMIDL_STUB_MESSAGE pStubMsg )
{
    #pragma pack(8)
    struct _PARAM_STRUCT
        {
        IMoniker *This;
        IBindCtx *pbc;
        IMoniker *pmkToLeft;
        REFIID riidResult;
        IUnknown **ppvResult;
        HRESULT _RetVal;
        };
    #pragma pack()
    struct _PARAM_STRUCT * pParamStruct;

    pParamStruct = (struct _PARAM_STRUCT *) pStubMsg->StackTop;
    
    /* Call the server */
    
    pParamStruct->_RetVal = IMoniker_BindToObject_Stub(
                                     (IMoniker *) pParamStruct->This,
                                     pParamStruct->pbc,
                                     pParamStruct->pmkToLeft,
                                     pParamStruct->riidResult,
                                     pParamStruct->ppvResult);
    
}

/* [call_as] */ HRESULT STDMETHODCALLTYPE IMoniker_RemoteBindToStorage_Proxy( 
    IMoniker * This,
    /* [unique][in] */ IBindCtx *pbc,
    /* [unique][in] */ IMoniker *pmkToLeft,
    /* [in] */ REFIID riid,
    /* [iid_is][out] */ IUnknown **ppvObj)
{
CLIENT_CALL_RETURN _RetVal;

_RetVal = NdrClientCall2(
                  ( PMIDL_STUB_DESC  )&Object_StubDesc,
                  (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[1230],
                  ( unsigned char * )This,
                  pbc,
                  pmkToLeft,
                  riid,
                  ppvObj);
return ( HRESULT  )_RetVal.Simple;

}

void __RPC_API
IMoniker_RemoteBindToStorage_Thunk(
    PMIDL_STUB_MESSAGE pStubMsg )
{
    #pragma pack(8)
    struct _PARAM_STRUCT
        {
        IMoniker *This;
        IBindCtx *pbc;
        IMoniker *pmkToLeft;
        REFIID riid;
        IUnknown **ppvObj;
        HRESULT _RetVal;
        };
    #pragma pack()
    struct _PARAM_STRUCT * pParamStruct;

    pParamStruct = (struct _PARAM_STRUCT *) pStubMsg->StackTop;
    
    /* Call the server */
    
    pParamStruct->_RetVal = IMoniker_BindToStorage_Stub(
                                      (IMoniker *) pParamStruct->This,
                                      pParamStruct->pbc,
                                      pParamStruct->pmkToLeft,
                                      pParamStruct->riid,
                                      pParamStruct->ppvObj);
    
}


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO IROTData_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO IROTData_ProxyInfo;


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO IEnumString_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO IEnumString_ProxyInfo;

/* [call_as] */ HRESULT STDMETHODCALLTYPE IEnumString_RemoteNext_Proxy( 
    IEnumString * This,
    /* [in] */ ULONG celt,
    /* [length_is][size_is][out] */ LPOLESTR *rgelt,
    /* [out] */ ULONG *pceltFetched)
{
CLIENT_CALL_RETURN _RetVal;

_RetVal = NdrClientCall2(
                  ( PMIDL_STUB_DESC  )&Object_StubDesc,
                  (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[1938],
                  ( unsigned char * )This,
                  celt,
                  rgelt,
                  pceltFetched);
return ( HRESULT  )_RetVal.Simple;

}

void __RPC_API
IEnumString_RemoteNext_Thunk(
    PMIDL_STUB_MESSAGE pStubMsg )
{
    #pragma pack(8)
    struct _PARAM_STRUCT
        {
        IEnumString *This;
        ULONG celt;
        char Pad0[4];
        LPOLESTR *rgelt;
        ULONG *pceltFetched;
        HRESULT _RetVal;
        };
    #pragma pack()
    struct _PARAM_STRUCT * pParamStruct;

    pParamStruct = (struct _PARAM_STRUCT *) pStubMsg->StackTop;
    
    /* Call the server */
    
    pParamStruct->_RetVal = IEnumString_Next_Stub(
                                (IEnumString *) pParamStruct->This,
                                pParamStruct->celt,
                                pParamStruct->rgelt,
                                pParamStruct->pceltFetched);
    
}


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO ISequentialStream_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO ISequentialStream_ProxyInfo;

/* [call_as] */ HRESULT STDMETHODCALLTYPE ISequentialStream_RemoteRead_Proxy( 
    ISequentialStream * This,
    /* [length_is][size_is][out] */ byte *pv,
    /* [in] */ ULONG cb,
    /* [out] */ ULONG *pcbRead)
{
CLIENT_CALL_RETURN _RetVal;

_RetVal = NdrClientCall2(
                  ( PMIDL_STUB_DESC  )&Object_StubDesc,
                  (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[2026],
                  ( unsigned char * )This,
                  pv,
                  cb,
                  pcbRead);
return ( HRESULT  )_RetVal.Simple;

}

void __RPC_API
ISequentialStream_RemoteRead_Thunk(
    PMIDL_STUB_MESSAGE pStubMsg )
{
    #pragma pack(8)
    struct _PARAM_STRUCT
        {
        ISequentialStream *This;
        byte *pv;
        ULONG cb;
        char Pad0[4];
        ULONG *pcbRead;
        HRESULT _RetVal;
        };
    #pragma pack()
    struct _PARAM_STRUCT * pParamStruct;

    pParamStruct = (struct _PARAM_STRUCT *) pStubMsg->StackTop;
    
    /* Call the server */
    
    pParamStruct->_RetVal = ISequentialStream_Read_Stub(
                                      (ISequentialStream *) pParamStruct->This,
                                      pParamStruct->pv,
                                      pParamStruct->cb,
                                      pParamStruct->pcbRead);
    
}

/* [call_as] */ HRESULT STDMETHODCALLTYPE ISequentialStream_RemoteWrite_Proxy( 
    ISequentialStream * This,
    /* [size_is][in] */ const byte *pv,
    /* [in] */ ULONG cb,
    /* [out] */ ULONG *pcbWritten)
{
CLIENT_CALL_RETURN _RetVal;

_RetVal = NdrClientCall2(
                  ( PMIDL_STUB_DESC  )&Object_StubDesc,
                  (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[2076],
                  ( unsigned char * )This,
                  pv,
                  cb,
                  pcbWritten);
return ( HRESULT  )_RetVal.Simple;

}

void __RPC_API
ISequentialStream_RemoteWrite_Thunk(
    PMIDL_STUB_MESSAGE pStubMsg )
{
    #pragma pack(8)
    struct _PARAM_STRUCT
        {
        ISequentialStream *This;
        const byte *pv;
        ULONG cb;
        char Pad0[4];
        ULONG *pcbWritten;
        HRESULT _RetVal;
        };
    #pragma pack()
    struct _PARAM_STRUCT * pParamStruct;

    pParamStruct = (struct _PARAM_STRUCT *) pStubMsg->StackTop;
    
    /* Call the server */
    
    pParamStruct->_RetVal = ISequentialStream_Write_Stub(
                                       (ISequentialStream *) pParamStruct->This,
                                       pParamStruct->pv,
                                       pParamStruct->cb,
                                       pParamStruct->pcbWritten);
    
}


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO IStream_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO IStream_ProxyInfo;

/* [call_as] */ HRESULT STDMETHODCALLTYPE IStream_RemoteSeek_Proxy( 
    IStream * This,
    /* [in] */ LARGE_INTEGER dlibMove,
    /* [in] */ DWORD dwOrigin,
    /* [out] */ ULARGE_INTEGER *plibNewPosition)
{
CLIENT_CALL_RETURN _RetVal;

_RetVal = NdrClientCall2(
                  ( PMIDL_STUB_DESC  )&Object_StubDesc,
                  (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[2126],
                  ( unsigned char * )This,
                  dlibMove,
                  dwOrigin,
                  plibNewPosition);
return ( HRESULT  )_RetVal.Simple;

}

void __RPC_API
IStream_RemoteSeek_Thunk(
    PMIDL_STUB_MESSAGE pStubMsg )
{
    #pragma pack(8)
    struct _PARAM_STRUCT
        {
        IStream *This;
        LARGE_INTEGER dlibMove;
        DWORD dwOrigin;
        char Pad0[4];
        ULARGE_INTEGER *plibNewPosition;
        HRESULT _RetVal;
        };
    #pragma pack()
    struct _PARAM_STRUCT * pParamStruct;

    pParamStruct = (struct _PARAM_STRUCT *) pStubMsg->StackTop;
    
    /* Call the server */
    
    pParamStruct->_RetVal = IStream_Seek_Stub(
                            (IStream *) pParamStruct->This,
                            pParamStruct->dlibMove,
                            pParamStruct->dwOrigin,
                            pParamStruct->plibNewPosition);
    
}

/* [call_as] */ HRESULT STDMETHODCALLTYPE IStream_RemoteCopyTo_Proxy( 
    IStream * This,
    /* [unique][in] */ IStream *pstm,
    /* [in] */ ULARGE_INTEGER cb,
    /* [out] */ ULARGE_INTEGER *pcbRead,
    /* [out] */ ULARGE_INTEGER *pcbWritten)
{
CLIENT_CALL_RETURN _RetVal;

_RetVal = NdrClientCall2(
                  ( PMIDL_STUB_DESC  )&Object_StubDesc,
                  (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[2214],
                  ( unsigned char * )This,
                  pstm,
                  cb,
                  pcbRead,
                  pcbWritten);
return ( HRESULT  )_RetVal.Simple;

}

void __RPC_API
IStream_RemoteCopyTo_Thunk(
    PMIDL_STUB_MESSAGE pStubMsg )
{
    #pragma pack(8)
    struct _PARAM_STRUCT
        {
        IStream *This;
        IStream *pstm;
        ULARGE_INTEGER cb;
        ULARGE_INTEGER *pcbRead;
        ULARGE_INTEGER *pcbWritten;
        HRESULT _RetVal;
        };
    #pragma pack()
    struct _PARAM_STRUCT * pParamStruct;

    pParamStruct = (struct _PARAM_STRUCT *) pStubMsg->StackTop;
    
    /* Call the server */
    
    pParamStruct->_RetVal = IStream_CopyTo_Stub(
                              (IStream *) pParamStruct->This,
                              pParamStruct->pstm,
                              pParamStruct->cb,
                              pParamStruct->pcbRead,
                              pParamStruct->pcbWritten);
    
}


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO IEnumSTATSTG_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO IEnumSTATSTG_ProxyInfo;

/* [call_as] */ HRESULT STDMETHODCALLTYPE IEnumSTATSTG_RemoteNext_Proxy( 
    IEnumSTATSTG * This,
    /* [in] */ ULONG celt,
    /* [length_is][size_is][out] */ STATSTG *rgelt,
    /* [out] */ ULONG *pceltFetched)
{
CLIENT_CALL_RETURN _RetVal;

_RetVal = NdrClientCall2(
                  ( PMIDL_STUB_DESC  )&Object_StubDesc,
                  (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[2522],
                  ( unsigned char * )This,
                  celt,
                  rgelt,
                  pceltFetched);
return ( HRESULT  )_RetVal.Simple;

}

void __RPC_API
IEnumSTATSTG_RemoteNext_Thunk(
    PMIDL_STUB_MESSAGE pStubMsg )
{
    #pragma pack(8)
    struct _PARAM_STRUCT
        {
        IEnumSTATSTG *This;
        ULONG celt;
        char Pad0[4];
        STATSTG *rgelt;
        ULONG *pceltFetched;
        HRESULT _RetVal;
        };
    #pragma pack()
    struct _PARAM_STRUCT * pParamStruct;

    pParamStruct = (struct _PARAM_STRUCT *) pStubMsg->StackTop;
    
    /* Call the server */
    
    pParamStruct->_RetVal = IEnumSTATSTG_Next_Stub(
                                 (IEnumSTATSTG *) pParamStruct->This,
                                 pParamStruct->celt,
                                 pParamStruct->rgelt,
                                 pParamStruct->pceltFetched);
    
}


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO IStorage_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO IStorage_ProxyInfo;

/* [call_as] */ HRESULT STDMETHODCALLTYPE IStorage_RemoteOpenStream_Proxy( 
    IStorage * This,
    /* [string][in] */ const OLECHAR *pwcsName,
    /* [in] */ unsigned long cbReserved1,
    /* [size_is][unique][in] */ byte *reserved1,
    /* [in] */ DWORD grfMode,
    /* [in] */ DWORD reserved2,
    /* [out] */ IStream **ppstm)
{
CLIENT_CALL_RETURN _RetVal;

_RetVal = NdrClientCall2(
                  ( PMIDL_STUB_DESC  )&Object_StubDesc,
                  (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[2672],
                  ( unsigned char * )This,
                  pwcsName,
                  cbReserved1,
                  reserved1,
                  grfMode,
                  reserved2,
                  ppstm);
return ( HRESULT  )_RetVal.Simple;

}

void __RPC_API
IStorage_RemoteOpenStream_Thunk(
    PMIDL_STUB_MESSAGE pStubMsg )
{
    #pragma pack(8)
    struct _PARAM_STRUCT
        {
        IStorage *This;
        const OLECHAR *pwcsName;
        unsigned long cbReserved1;
        char Pad0[4];
        byte *reserved1;
        DWORD grfMode;
        char Pad1[4];
        DWORD reserved2;
        char Pad2[4];
        IStream **ppstm;
        HRESULT _RetVal;
        };
    #pragma pack()
    struct _PARAM_STRUCT * pParamStruct;

    pParamStruct = (struct _PARAM_STRUCT *) pStubMsg->StackTop;
    
    /* Call the server */
    
    pParamStruct->_RetVal = IStorage_OpenStream_Stub(
                                   (IStorage *) pParamStruct->This,
                                   pParamStruct->pwcsName,
                                   pParamStruct->cbReserved1,
                                   pParamStruct->reserved1,
                                   pParamStruct->grfMode,
                                   pParamStruct->reserved2,
                                   pParamStruct->ppstm);
    
}

/* [call_as] */ HRESULT STDMETHODCALLTYPE IStorage_RemoteEnumElements_Proxy( 
    IStorage * This,
    /* [in] */ DWORD reserved1,
    /* [in] */ unsigned long cbReserved2,
    /* [size_is][unique][in] */ byte *reserved2,
    /* [in] */ DWORD reserved3,
    /* [out] */ IEnumSTATSTG **ppenum)
{
CLIENT_CALL_RETURN _RetVal;

_RetVal = NdrClientCall2(
                  ( PMIDL_STUB_DESC  )&Object_StubDesc,
                  (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[3052],
                  ( unsigned char * )This,
                  reserved1,
                  cbReserved2,
                  reserved2,
                  reserved3,
                  ppenum);
return ( HRESULT  )_RetVal.Simple;

}

void __RPC_API
IStorage_RemoteEnumElements_Thunk(
    PMIDL_STUB_MESSAGE pStubMsg )
{
    #pragma pack(8)
    struct _PARAM_STRUCT
        {
        IStorage *This;
        DWORD reserved1;
        char Pad0[4];
        unsigned long cbReserved2;
        char Pad1[4];
        byte *reserved2;
        DWORD reserved3;
        char Pad2[4];
        IEnumSTATSTG **ppenum;
        HRESULT _RetVal;
        };
    #pragma pack()
    struct _PARAM_STRUCT * pParamStruct;

    pParamStruct = (struct _PARAM_STRUCT *) pStubMsg->StackTop;
    
    /* Call the server */
    
    pParamStruct->_RetVal = IStorage_EnumElements_Stub(
                                     (IStorage *) pParamStruct->This,
                                     pParamStruct->reserved1,
                                     pParamStruct->cbReserved2,
                                     pParamStruct->reserved2,
                                     pParamStruct->reserved3,
                                     pParamStruct->ppenum);
    
}


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO IPersistFile_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO IPersistFile_ProxyInfo;


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO IPersistStorage_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO IPersistStorage_ProxyInfo;


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO ILockBytes_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO ILockBytes_ProxyInfo;

/* [call_as] */ HRESULT __stdcall ILockBytes_RemoteReadAt_Proxy( 
    ILockBytes * This,
    /* [in] */ ULARGE_INTEGER ulOffset,
    /* [length_is][size_is][out] */ byte *pv,
    /* [in] */ ULONG cb,
    /* [out] */ ULONG *pcbRead)
{
CLIENT_CALL_RETURN _RetVal;

_RetVal = NdrClientCall2(
                  ( PMIDL_STUB_DESC  )&Object_StubDesc,
                  (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[3662],
                  ( unsigned char * )This,
                  ulOffset,
                  pv,
                  cb,
                  pcbRead);
return ( HRESULT  )_RetVal.Simple;

}

void __RPC_API
ILockBytes_RemoteReadAt_Thunk(
    PMIDL_STUB_MESSAGE pStubMsg )
{
    #pragma pack(8)
    struct _PARAM_STRUCT
        {
        ILockBytes *This;
        ULARGE_INTEGER ulOffset;
        byte *pv;
        ULONG cb;
        char Pad0[4];
        ULONG *pcbRead;
        HRESULT _RetVal;
        };
    #pragma pack()
    struct _PARAM_STRUCT * pParamStruct;

    pParamStruct = (struct _PARAM_STRUCT *) pStubMsg->StackTop;
    
    /* Call the server */
    
    pParamStruct->_RetVal = ILockBytes_ReadAt_Stub(
                                 (ILockBytes *) pParamStruct->This,
                                 pParamStruct->ulOffset,
                                 pParamStruct->pv,
                                 pParamStruct->cb,
                                 pParamStruct->pcbRead);
    
}

/* [call_as] */ HRESULT STDMETHODCALLTYPE ILockBytes_RemoteWriteAt_Proxy( 
    ILockBytes * This,
    /* [in] */ ULARGE_INTEGER ulOffset,
    /* [size_is][in] */ const byte *pv,
    /* [in] */ ULONG cb,
    /* [out] */ ULONG *pcbWritten)
{
CLIENT_CALL_RETURN _RetVal;

_RetVal = NdrClientCall2(
                  ( PMIDL_STUB_DESC  )&Object_StubDesc,
                  (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[3718],
                  ( unsigned char * )This,
                  ulOffset,
                  pv,
                  cb,
                  pcbWritten);
return ( HRESULT  )_RetVal.Simple;

}

void __RPC_API
ILockBytes_RemoteWriteAt_Thunk(
    PMIDL_STUB_MESSAGE pStubMsg )
{
    #pragma pack(8)
    struct _PARAM_STRUCT
        {
        ILockBytes *This;
        ULARGE_INTEGER ulOffset;
        const byte *pv;
        ULONG cb;
        char Pad0[4];
        ULONG *pcbWritten;
        HRESULT _RetVal;
        };
    #pragma pack()
    struct _PARAM_STRUCT * pParamStruct;

    pParamStruct = (struct _PARAM_STRUCT *) pStubMsg->StackTop;
    
    /* Call the server */
    
    pParamStruct->_RetVal = ILockBytes_WriteAt_Stub(
                                  (ILockBytes *) pParamStruct->This,
                                  pParamStruct->ulOffset,
                                  pParamStruct->pv,
                                  pParamStruct->cb,
                                  pParamStruct->pcbWritten);
    
}


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO IEnumFORMATETC_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO IEnumFORMATETC_ProxyInfo;

/* [call_as] */ HRESULT STDMETHODCALLTYPE IEnumFORMATETC_RemoteNext_Proxy( 
    IEnumFORMATETC * This,
    /* [in] */ ULONG celt,
    /* [length_is][size_is][out] */ FORMATETC *rgelt,
    /* [out] */ ULONG *pceltFetched)
{
CLIENT_CALL_RETURN _RetVal;

_RetVal = NdrClientCall2(
                  ( PMIDL_STUB_DESC  )&Object_StubDesc,
                  (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[3918],
                  ( unsigned char * )This,
                  celt,
                  rgelt,
                  pceltFetched);
return ( HRESULT  )_RetVal.Simple;

}

void __RPC_API
IEnumFORMATETC_RemoteNext_Thunk(
    PMIDL_STUB_MESSAGE pStubMsg )
{
    #pragma pack(8)
    struct _PARAM_STRUCT
        {
        IEnumFORMATETC *This;
        ULONG celt;
        char Pad0[4];
        FORMATETC *rgelt;
        ULONG *pceltFetched;
        HRESULT _RetVal;
        };
    #pragma pack()
    struct _PARAM_STRUCT * pParamStruct;

    pParamStruct = (struct _PARAM_STRUCT *) pStubMsg->StackTop;
    
    /* Call the server */
    
    pParamStruct->_RetVal = IEnumFORMATETC_Next_Stub(
                                   (IEnumFORMATETC *) pParamStruct->This,
                                   pParamStruct->celt,
                                   pParamStruct->rgelt,
                                   pParamStruct->pceltFetched);
    
}


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO IEnumSTATDATA_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO IEnumSTATDATA_ProxyInfo;

/* [call_as] */ HRESULT STDMETHODCALLTYPE IEnumSTATDATA_RemoteNext_Proxy( 
    IEnumSTATDATA * This,
    /* [in] */ ULONG celt,
    /* [length_is][size_is][out] */ STATDATA *rgelt,
    /* [out] */ ULONG *pceltFetched)
{
CLIENT_CALL_RETURN _RetVal;

_RetVal = NdrClientCall2(
                  ( PMIDL_STUB_DESC  )&Object_StubDesc,
                  (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[4006],
                  ( unsigned char * )This,
                  celt,
                  rgelt,
                  pceltFetched);
return ( HRESULT  )_RetVal.Simple;

}

void __RPC_API
IEnumSTATDATA_RemoteNext_Thunk(
    PMIDL_STUB_MESSAGE pStubMsg )
{
    #pragma pack(8)
    struct _PARAM_STRUCT
        {
        IEnumSTATDATA *This;
        ULONG celt;
        char Pad0[4];
        STATDATA *rgelt;
        ULONG *pceltFetched;
        HRESULT _RetVal;
        };
    #pragma pack()
    struct _PARAM_STRUCT * pParamStruct;

    pParamStruct = (struct _PARAM_STRUCT *) pStubMsg->StackTop;
    
    /* Call the server */
    
    pParamStruct->_RetVal = IEnumSTATDATA_Next_Stub(
                                  (IEnumSTATDATA *) pParamStruct->This,
                                  pParamStruct->celt,
                                  pParamStruct->rgelt,
                                  pParamStruct->pceltFetched);
    
}


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO IRootStorage_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO IRootStorage_ProxyInfo;


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO IAdviseSink_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO IAdviseSink_ProxyInfo;

/* [call_as] */ HRESULT STDMETHODCALLTYPE IAdviseSink_RemoteOnDataChange_Proxy( 
    IAdviseSink * This,
    /* [unique][in] */ FORMATETC *pFormatetc,
    /* [unique][in] */ ASYNC_STGMEDIUM *pStgmed)
{
CLIENT_CALL_RETURN _RetVal;

_RetVal = NdrClientCall2(
                  ( PMIDL_STUB_DESC  )&Object_StubDesc,
                  (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[4132],
                  ( unsigned char * )This,
                  pFormatetc,
                  pStgmed);
return ( HRESULT  )_RetVal.Simple;

}

void __RPC_API
IAdviseSink_RemoteOnDataChange_Thunk(
    PMIDL_STUB_MESSAGE pStubMsg )
{
    #pragma pack(8)
    struct _PARAM_STRUCT
        {
        IAdviseSink *This;
        FORMATETC *pFormatetc;
        ASYNC_STGMEDIUM *pStgmed;
        HRESULT _RetVal;
        };
    #pragma pack()
    struct _PARAM_STRUCT * pParamStruct;

    pParamStruct = (struct _PARAM_STRUCT *) pStubMsg->StackTop;
    
    /* Call the server */
    
    pParamStruct->_RetVal = IAdviseSink_OnDataChange_Stub(
                                        (IAdviseSink *) pParamStruct->This,
                                        pParamStruct->pFormatetc,
                                        pParamStruct->pStgmed);
    
}

/* [call_as] */ HRESULT STDMETHODCALLTYPE IAdviseSink_RemoteOnViewChange_Proxy( 
    IAdviseSink * This,
    /* [in] */ DWORD dwAspect,
    /* [in] */ LONG lindex)
{
CLIENT_CALL_RETURN _RetVal;

_RetVal = NdrClientCall2(
                  ( PMIDL_STUB_DESC  )&Object_StubDesc,
                  (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[4176],
                  ( unsigned char * )This,
                  dwAspect,
                  lindex);
return ( HRESULT  )_RetVal.Simple;

}

void __RPC_API
IAdviseSink_RemoteOnViewChange_Thunk(
    PMIDL_STUB_MESSAGE pStubMsg )
{
    #pragma pack(8)
    struct _PARAM_STRUCT
        {
        IAdviseSink *This;
        DWORD dwAspect;
        char Pad0[4];
        LONG lindex;
        char Pad1[4];
        HRESULT _RetVal;
        };
    #pragma pack()
    struct _PARAM_STRUCT * pParamStruct;

    pParamStruct = (struct _PARAM_STRUCT *) pStubMsg->StackTop;
    
    /* Call the server */
    
    pParamStruct->_RetVal = IAdviseSink_OnViewChange_Stub(
                                        (IAdviseSink *) pParamStruct->This,
                                        pParamStruct->dwAspect,
                                        pParamStruct->lindex);
    
}

/* [call_as] */ HRESULT STDMETHODCALLTYPE IAdviseSink_RemoteOnRename_Proxy( 
    IAdviseSink * This,
    /* [in] */ IMoniker *pmk)
{
CLIENT_CALL_RETURN _RetVal;

_RetVal = NdrClientCall2(
                  ( PMIDL_STUB_DESC  )&Object_StubDesc,
                  (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[814],
                  ( unsigned char * )This,
                  pmk);
return ( HRESULT  )_RetVal.Simple;

}

void __RPC_API
IAdviseSink_RemoteOnRename_Thunk(
    PMIDL_STUB_MESSAGE pStubMsg )
{
    #pragma pack(8)
    struct _PARAM_STRUCT
        {
        IAdviseSink *This;
        IMoniker *pmk;
        HRESULT _RetVal;
        };
    #pragma pack()
    struct _PARAM_STRUCT * pParamStruct;

    pParamStruct = (struct _PARAM_STRUCT *) pStubMsg->StackTop;
    
    /* Call the server */
    
    pParamStruct->_RetVal = IAdviseSink_OnRename_Stub((IAdviseSink *) pParamStruct->This,pParamStruct->pmk);
    
}

/* [call_as] */ HRESULT STDMETHODCALLTYPE IAdviseSink_RemoteOnSave_Proxy( 
    IAdviseSink * This)
{
CLIENT_CALL_RETURN _RetVal;

_RetVal = NdrClientCall2(
                  ( PMIDL_STUB_DESC  )&Object_StubDesc,
                  (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[4220],
                  ( unsigned char * )This);
return ( HRESULT  )_RetVal.Simple;

}

void __RPC_API
IAdviseSink_RemoteOnSave_Thunk(
    PMIDL_STUB_MESSAGE pStubMsg )
{
    #pragma pack(8)
    struct _PARAM_STRUCT
        {
        IAdviseSink *This;
        HRESULT _RetVal;
        };
    #pragma pack()
    struct _PARAM_STRUCT * pParamStruct;

    pParamStruct = (struct _PARAM_STRUCT *) pStubMsg->StackTop;
    
    /* Call the server */
    
    pParamStruct->_RetVal = IAdviseSink_OnSave_Stub((IAdviseSink *) pParamStruct->This);
    
}

/* [call_as] */ HRESULT STDMETHODCALLTYPE IAdviseSink_RemoteOnClose_Proxy( 
    IAdviseSink * This)
{
CLIENT_CALL_RETURN _RetVal;

_RetVal = NdrClientCall2(
                  ( PMIDL_STUB_DESC  )&Object_StubDesc,
                  (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[4252],
                  ( unsigned char * )This);
return ( HRESULT  )_RetVal.Simple;

}

void __RPC_API
IAdviseSink_RemoteOnClose_Thunk(
    PMIDL_STUB_MESSAGE pStubMsg )
{
    #pragma pack(8)
    struct _PARAM_STRUCT
        {
        IAdviseSink *This;
        HRESULT _RetVal;
        };
    #pragma pack()
    struct _PARAM_STRUCT * pParamStruct;

    pParamStruct = (struct _PARAM_STRUCT *) pStubMsg->StackTop;
    
    /* Call the server */
    
    pParamStruct->_RetVal = IAdviseSink_OnClose_Stub((IAdviseSink *) pParamStruct->This);
    
}


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO AsyncIAdviseSink_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO AsyncIAdviseSink_ProxyInfo;

/* [call_as] */ HRESULT STDMETHODCALLTYPE AsyncIAdviseSink_Begin_RemoteOnDataChange_Proxy( 
    AsyncIAdviseSink * This,
    /* [unique][in] */ FORMATETC *pFormatetc,
    /* [unique][in] */ ASYNC_STGMEDIUM *pStgmed)
{
CLIENT_CALL_RETURN _RetVal;

_RetVal = NdrDcomAsyncClientCall(
                          ( PMIDL_STUB_DESC  )&Object_StubDesc,
                          (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[4284],
                          ( unsigned char * )This,
                          pFormatetc,
                          pStgmed);
return ( HRESULT  )_RetVal.Simple;

}

void __RPC_API
AsyncIAdviseSink_Begin_RemoteOnDataChange_Thunk(
    PMIDL_STUB_MESSAGE pStubMsg )
{
    #pragma pack(8)
    struct _PARAM_STRUCT
        {
        AsyncIAdviseSink *This;
        FORMATETC *pFormatetc;
        ASYNC_STGMEDIUM *pStgmed;
        HRESULT _RetVal;
        };
    #pragma pack()
    struct _PARAM_STRUCT * pParamStruct;

    pParamStruct = (struct _PARAM_STRUCT *) pStubMsg->StackTop;
    
    /* Call the server */
    
    pParamStruct->_RetVal = AsyncIAdviseSink_Begin_OnDataChange_Stub(
                                                   (AsyncIAdviseSink *) pParamStruct->This,
                                                   pParamStruct->pFormatetc,
                                                   pParamStruct->pStgmed);
    
}

/* [call_as] */ HRESULT STDMETHODCALLTYPE AsyncIAdviseSink_Finish_RemoteOnDataChange_Proxy( 
    AsyncIAdviseSink * This)
{
CLIENT_CALL_RETURN _RetVal;

_RetVal = NdrDcomAsyncClientCall(
                          ( PMIDL_STUB_DESC  )&Object_StubDesc,
                          (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[4328],
                          ( unsigned char * )This);
return ( HRESULT  )_RetVal.Simple;

}

void __RPC_API
AsyncIAdviseSink_Finish_RemoteOnDataChange_Thunk(
    PMIDL_STUB_MESSAGE pStubMsg )
{
    #pragma pack(8)
    struct _PARAM_STRUCT
        {
        AsyncIAdviseSink *This;
        HRESULT _RetVal;
        };
    #pragma pack()
    struct _PARAM_STRUCT * pParamStruct;

    pParamStruct = (struct _PARAM_STRUCT *) pStubMsg->StackTop;
    
    /* Call the server */
    
    pParamStruct->_RetVal = AsyncIAdviseSink_Finish_OnDataChange_Stub((AsyncIAdviseSink *) pParamStruct->This);
    
}

/* [call_as] */ HRESULT STDMETHODCALLTYPE AsyncIAdviseSink_Begin_RemoteOnViewChange_Proxy( 
    AsyncIAdviseSink * This,
    /* [in] */ DWORD dwAspect,
    /* [in] */ LONG lindex)
{
CLIENT_CALL_RETURN _RetVal;

_RetVal = NdrDcomAsyncClientCall(
                          ( PMIDL_STUB_DESC  )&Object_StubDesc,
                          (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[4360],
                          ( unsigned char * )This,
                          dwAspect,
                          lindex);
return ( HRESULT  )_RetVal.Simple;

}

void __RPC_API
AsyncIAdviseSink_Begin_RemoteOnViewChange_Thunk(
    PMIDL_STUB_MESSAGE pStubMsg )
{
    #pragma pack(8)
    struct _PARAM_STRUCT
        {
        AsyncIAdviseSink *This;
        DWORD dwAspect;
        char Pad0[4];
        LONG lindex;
        char Pad1[4];
        HRESULT _RetVal;
        };
    #pragma pack()
    struct _PARAM_STRUCT * pParamStruct;

    pParamStruct = (struct _PARAM_STRUCT *) pStubMsg->StackTop;
    
    /* Call the server */
    
    pParamStruct->_RetVal = AsyncIAdviseSink_Begin_OnViewChange_Stub(
                                                   (AsyncIAdviseSink *) pParamStruct->This,
                                                   pParamStruct->dwAspect,
                                                   pParamStruct->lindex);
    
}

/* [call_as] */ HRESULT STDMETHODCALLTYPE AsyncIAdviseSink_Finish_RemoteOnViewChange_Proxy( 
    AsyncIAdviseSink * This)
{
CLIENT_CALL_RETURN _RetVal;

_RetVal = NdrDcomAsyncClientCall(
                          ( PMIDL_STUB_DESC  )&Object_StubDesc,
                          (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[4404],
                          ( unsigned char * )This);
return ( HRESULT  )_RetVal.Simple;

}

void __RPC_API
AsyncIAdviseSink_Finish_RemoteOnViewChange_Thunk(
    PMIDL_STUB_MESSAGE pStubMsg )
{
    #pragma pack(8)
    struct _PARAM_STRUCT
        {
        AsyncIAdviseSink *This;
        HRESULT _RetVal;
        };
    #pragma pack()
    struct _PARAM_STRUCT * pParamStruct;

    pParamStruct = (struct _PARAM_STRUCT *) pStubMsg->StackTop;
    
    /* Call the server */
    
    pParamStruct->_RetVal = AsyncIAdviseSink_Finish_OnViewChange_Stub((AsyncIAdviseSink *) pParamStruct->This);
    
}

/* [call_as] */ HRESULT STDMETHODCALLTYPE AsyncIAdviseSink_Begin_RemoteOnRename_Proxy( 
    AsyncIAdviseSink * This,
    /* [in] */ IMoniker *pmk)
{
CLIENT_CALL_RETURN _RetVal;

_RetVal = NdrDcomAsyncClientCall(
                          ( PMIDL_STUB_DESC  )&Object_StubDesc,
                          (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[4436],
                          ( unsigned char * )This,
                          pmk);
return ( HRESULT  )_RetVal.Simple;

}

void __RPC_API
AsyncIAdviseSink_Begin_RemoteOnRename_Thunk(
    PMIDL_STUB_MESSAGE pStubMsg )
{
    #pragma pack(8)
    struct _PARAM_STRUCT
        {
        AsyncIAdviseSink *This;
        IMoniker *pmk;
        HRESULT _RetVal;
        };
    #pragma pack()
    struct _PARAM_STRUCT * pParamStruct;

    pParamStruct = (struct _PARAM_STRUCT *) pStubMsg->StackTop;
    
    /* Call the server */
    
    pParamStruct->_RetVal = AsyncIAdviseSink_Begin_OnRename_Stub((AsyncIAdviseSink *) pParamStruct->This,pParamStruct->pmk);
    
}

/* [call_as] */ HRESULT STDMETHODCALLTYPE AsyncIAdviseSink_Finish_RemoteOnRename_Proxy( 
    AsyncIAdviseSink * This)
{
CLIENT_CALL_RETURN _RetVal;

_RetVal = NdrDcomAsyncClientCall(
                          ( PMIDL_STUB_DESC  )&Object_StubDesc,
                          (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[4474],
                          ( unsigned char * )This);
return ( HRESULT  )_RetVal.Simple;

}

void __RPC_API
AsyncIAdviseSink_Finish_RemoteOnRename_Thunk(
    PMIDL_STUB_MESSAGE pStubMsg )
{
    #pragma pack(8)
    struct _PARAM_STRUCT
        {
        AsyncIAdviseSink *This;
        HRESULT _RetVal;
        };
    #pragma pack()
    struct _PARAM_STRUCT * pParamStruct;

    pParamStruct = (struct _PARAM_STRUCT *) pStubMsg->StackTop;
    
    /* Call the server */
    
    pParamStruct->_RetVal = AsyncIAdviseSink_Finish_OnRename_Stub((AsyncIAdviseSink *) pParamStruct->This);
    
}

/* [call_as] */ HRESULT STDMETHODCALLTYPE AsyncIAdviseSink_Begin_RemoteOnSave_Proxy( 
    AsyncIAdviseSink * This)
{
CLIENT_CALL_RETURN _RetVal;

_RetVal = NdrDcomAsyncClientCall(
                          ( PMIDL_STUB_DESC  )&Object_StubDesc,
                          (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[4506],
                          ( unsigned char * )This);
return ( HRESULT  )_RetVal.Simple;

}

void __RPC_API
AsyncIAdviseSink_Begin_RemoteOnSave_Thunk(
    PMIDL_STUB_MESSAGE pStubMsg )
{
    #pragma pack(8)
    struct _PARAM_STRUCT
        {
        AsyncIAdviseSink *This;
        HRESULT _RetVal;
        };
    #pragma pack()
    struct _PARAM_STRUCT * pParamStruct;

    pParamStruct = (struct _PARAM_STRUCT *) pStubMsg->StackTop;
    
    /* Call the server */
    
    pParamStruct->_RetVal = AsyncIAdviseSink_Begin_OnSave_Stub((AsyncIAdviseSink *) pParamStruct->This);
    
}

/* [call_as] */ HRESULT STDMETHODCALLTYPE AsyncIAdviseSink_Finish_RemoteOnSave_Proxy( 
    AsyncIAdviseSink * This)
{
CLIENT_CALL_RETURN _RetVal;

_RetVal = NdrDcomAsyncClientCall(
                          ( PMIDL_STUB_DESC  )&Object_StubDesc,
                          (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[4538],
                          ( unsigned char * )This);
return ( HRESULT  )_RetVal.Simple;

}

void __RPC_API
AsyncIAdviseSink_Finish_RemoteOnSave_Thunk(
    PMIDL_STUB_MESSAGE pStubMsg )
{
    #pragma pack(8)
    struct _PARAM_STRUCT
        {
        AsyncIAdviseSink *This;
        HRESULT _RetVal;
        };
    #pragma pack()
    struct _PARAM_STRUCT * pParamStruct;

    pParamStruct = (struct _PARAM_STRUCT *) pStubMsg->StackTop;
    
    /* Call the server */
    
    pParamStruct->_RetVal = AsyncIAdviseSink_Finish_OnSave_Stub((AsyncIAdviseSink *) pParamStruct->This);
    
}

/* [call_as] */ HRESULT STDMETHODCALLTYPE AsyncIAdviseSink_Begin_RemoteOnClose_Proxy( 
    AsyncIAdviseSink * This)
{
CLIENT_CALL_RETURN _RetVal;

_RetVal = NdrDcomAsyncClientCall(
                          ( PMIDL_STUB_DESC  )&Object_StubDesc,
                          (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[4570],
                          ( unsigned char * )This);
return ( HRESULT  )_RetVal.Simple;

}

void __RPC_API
AsyncIAdviseSink_Begin_RemoteOnClose_Thunk(
    PMIDL_STUB_MESSAGE pStubMsg )
{
    #pragma pack(8)
    struct _PARAM_STRUCT
        {
        AsyncIAdviseSink *This;
        HRESULT _RetVal;
        };
    #pragma pack()
    struct _PARAM_STRUCT * pParamStruct;

    pParamStruct = (struct _PARAM_STRUCT *) pStubMsg->StackTop;
    
    /* Call the server */
    
    pParamStruct->_RetVal = AsyncIAdviseSink_Begin_OnClose_Stub((AsyncIAdviseSink *) pParamStruct->This);
    
}

/* [call_as] */ HRESULT STDMETHODCALLTYPE AsyncIAdviseSink_Finish_RemoteOnClose_Proxy( 
    AsyncIAdviseSink * This)
{
CLIENT_CALL_RETURN _RetVal;

_RetVal = NdrDcomAsyncClientCall(
                          ( PMIDL_STUB_DESC  )&Object_StubDesc,
                          (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[4602],
                          ( unsigned char * )This);
return ( HRESULT  )_RetVal.Simple;

}

void __RPC_API
AsyncIAdviseSink_Finish_RemoteOnClose_Thunk(
    PMIDL_STUB_MESSAGE pStubMsg )
{
    #pragma pack(8)
    struct _PARAM_STRUCT
        {
        AsyncIAdviseSink *This;
        HRESULT _RetVal;
        };
    #pragma pack()
    struct _PARAM_STRUCT * pParamStruct;

    pParamStruct = (struct _PARAM_STRUCT *) pStubMsg->StackTop;
    
    /* Call the server */
    
    pParamStruct->_RetVal = AsyncIAdviseSink_Finish_OnClose_Stub((AsyncIAdviseSink *) pParamStruct->This);
    
}


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO IAdviseSink2_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO IAdviseSink2_ProxyInfo;

/* [call_as] */ HRESULT STDMETHODCALLTYPE IAdviseSink2_RemoteOnLinkSrcChange_Proxy( 
    IAdviseSink2 * This,
    /* [unique][in] */ IMoniker *pmk)
{
CLIENT_CALL_RETURN _RetVal;

_RetVal = NdrClientCall2(
                  ( PMIDL_STUB_DESC  )&Object_StubDesc,
                  (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[4634],
                  ( unsigned char * )This,
                  pmk);
return ( HRESULT  )_RetVal.Simple;

}

void __RPC_API
IAdviseSink2_RemoteOnLinkSrcChange_Thunk(
    PMIDL_STUB_MESSAGE pStubMsg )
{
    #pragma pack(8)
    struct _PARAM_STRUCT
        {
        IAdviseSink2 *This;
        IMoniker *pmk;
        HRESULT _RetVal;
        };
    #pragma pack()
    struct _PARAM_STRUCT * pParamStruct;

    pParamStruct = (struct _PARAM_STRUCT *) pStubMsg->StackTop;
    
    /* Call the server */
    
    pParamStruct->_RetVal = IAdviseSink2_OnLinkSrcChange_Stub((IAdviseSink2 *) pParamStruct->This,pParamStruct->pmk);
    
}


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO AsyncIAdviseSink2_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO AsyncIAdviseSink2_ProxyInfo;

/* [call_as] */ HRESULT STDMETHODCALLTYPE AsyncIAdviseSink2_Begin_RemoteOnLinkSrcChange_Proxy( 
    AsyncIAdviseSink2 * This,
    /* [unique][in] */ IMoniker *pmk)
{
CLIENT_CALL_RETURN _RetVal;

_RetVal = NdrDcomAsyncClientCall(
                          ( PMIDL_STUB_DESC  )&Object_StubDesc,
                          (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[4672],
                          ( unsigned char * )This,
                          pmk);
return ( HRESULT  )_RetVal.Simple;

}

void __RPC_API
AsyncIAdviseSink2_Begin_RemoteOnLinkSrcChange_Thunk(
    PMIDL_STUB_MESSAGE pStubMsg )
{
    #pragma pack(8)
    struct _PARAM_STRUCT
        {
        AsyncIAdviseSink2 *This;
        IMoniker *pmk;
        HRESULT _RetVal;
        };
    #pragma pack()
    struct _PARAM_STRUCT * pParamStruct;

    pParamStruct = (struct _PARAM_STRUCT *) pStubMsg->StackTop;
    
    /* Call the server */
    
    pParamStruct->_RetVal = AsyncIAdviseSink2_Begin_OnLinkSrcChange_Stub((AsyncIAdviseSink2 *) pParamStruct->This,pParamStruct->pmk);
    
}

/* [call_as] */ HRESULT STDMETHODCALLTYPE AsyncIAdviseSink2_Finish_RemoteOnLinkSrcChange_Proxy( 
    AsyncIAdviseSink2 * This)
{
CLIENT_CALL_RETURN _RetVal;

_RetVal = NdrDcomAsyncClientCall(
                          ( PMIDL_STUB_DESC  )&Object_StubDesc,
                          (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[4710],
                          ( unsigned char * )This);
return ( HRESULT  )_RetVal.Simple;

}

void __RPC_API
AsyncIAdviseSink2_Finish_RemoteOnLinkSrcChange_Thunk(
    PMIDL_STUB_MESSAGE pStubMsg )
{
    #pragma pack(8)
    struct _PARAM_STRUCT
        {
        AsyncIAdviseSink2 *This;
        HRESULT _RetVal;
        };
    #pragma pack()
    struct _PARAM_STRUCT * pParamStruct;

    pParamStruct = (struct _PARAM_STRUCT *) pStubMsg->StackTop;
    
    /* Call the server */
    
    pParamStruct->_RetVal = AsyncIAdviseSink2_Finish_OnLinkSrcChange_Stub((AsyncIAdviseSink2 *) pParamStruct->This);
    
}


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO IDataObject_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO IDataObject_ProxyInfo;

/* [call_as] */ HRESULT STDMETHODCALLTYPE IDataObject_RemoteGetData_Proxy( 
    IDataObject * This,
    /* [unique][in] */ FORMATETC *pformatetcIn,
    /* [out] */ STGMEDIUM *pRemoteMedium)
{
CLIENT_CALL_RETURN _RetVal;

_RetVal = NdrClientCall2(
                  ( PMIDL_STUB_DESC  )&Object_StubDesc,
                  (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[4742],
                  ( unsigned char * )This,
                  pformatetcIn,
                  pRemoteMedium);
return ( HRESULT  )_RetVal.Simple;

}

void __RPC_API
IDataObject_RemoteGetData_Thunk(
    PMIDL_STUB_MESSAGE pStubMsg )
{
    #pragma pack(8)
    struct _PARAM_STRUCT
        {
        IDataObject *This;
        FORMATETC *pformatetcIn;
        STGMEDIUM *pRemoteMedium;
        HRESULT _RetVal;
        };
    #pragma pack()
    struct _PARAM_STRUCT * pParamStruct;

    pParamStruct = (struct _PARAM_STRUCT *) pStubMsg->StackTop;
    
    /* Call the server */
    
    pParamStruct->_RetVal = IDataObject_GetData_Stub(
                                   (IDataObject *) pParamStruct->This,
                                   pParamStruct->pformatetcIn,
                                   pParamStruct->pRemoteMedium);
    
}

/* [call_as] */ HRESULT STDMETHODCALLTYPE IDataObject_RemoteGetDataHere_Proxy( 
    IDataObject * This,
    /* [unique][in] */ FORMATETC *pformatetc,
    /* [out][in] */ STGMEDIUM *pRemoteMedium)
{
CLIENT_CALL_RETURN _RetVal;

_RetVal = NdrClientCall2(
                  ( PMIDL_STUB_DESC  )&Object_StubDesc,
                  (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[4786],
                  ( unsigned char * )This,
                  pformatetc,
                  pRemoteMedium);
return ( HRESULT  )_RetVal.Simple;

}

void __RPC_API
IDataObject_RemoteGetDataHere_Thunk(
    PMIDL_STUB_MESSAGE pStubMsg )
{
    #pragma pack(8)
    struct _PARAM_STRUCT
        {
        IDataObject *This;
        FORMATETC *pformatetc;
        STGMEDIUM *pRemoteMedium;
        HRESULT _RetVal;
        };
    #pragma pack()
    struct _PARAM_STRUCT * pParamStruct;

    pParamStruct = (struct _PARAM_STRUCT *) pStubMsg->StackTop;
    
    /* Call the server */
    
    pParamStruct->_RetVal = IDataObject_GetDataHere_Stub(
                                       (IDataObject *) pParamStruct->This,
                                       pParamStruct->pformatetc,
                                       pParamStruct->pRemoteMedium);
    
}

/* [call_as] */ HRESULT STDMETHODCALLTYPE IDataObject_RemoteSetData_Proxy( 
    IDataObject * This,
    /* [unique][in] */ FORMATETC *pformatetc,
    /* [unique][in] */ FLAG_STGMEDIUM *pmedium,
    /* [in] */ BOOL fRelease)
{
CLIENT_CALL_RETURN _RetVal;

_RetVal = NdrClientCall2(
                  ( PMIDL_STUB_DESC  )&Object_StubDesc,
                  (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[4912],
                  ( unsigned char * )This,
                  pformatetc,
                  pmedium,
                  fRelease);
return ( HRESULT  )_RetVal.Simple;

}

void __RPC_API
IDataObject_RemoteSetData_Thunk(
    PMIDL_STUB_MESSAGE pStubMsg )
{
    #pragma pack(8)
    struct _PARAM_STRUCT
        {
        IDataObject *This;
        FORMATETC *pformatetc;
        FLAG_STGMEDIUM *pmedium;
        BOOL fRelease;
        char Pad0[4];
        HRESULT _RetVal;
        };
    #pragma pack()
    struct _PARAM_STRUCT * pParamStruct;

    pParamStruct = (struct _PARAM_STRUCT *) pStubMsg->StackTop;
    
    /* Call the server */
    
    pParamStruct->_RetVal = IDataObject_SetData_Stub(
                                   (IDataObject *) pParamStruct->This,
                                   pParamStruct->pformatetc,
                                   pParamStruct->pmedium,
                                   pParamStruct->fRelease);
    
}


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO IClassActivator_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO IClassActivator_ProxyInfo;


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO IFillLockBytes_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO IFillLockBytes_ProxyInfo;

/* [call_as] */ HRESULT __stdcall IFillLockBytes_RemoteFillAppend_Proxy( 
    IFillLockBytes * This,
    /* [size_is][in] */ const byte *pv,
    /* [in] */ ULONG cb,
    /* [out] */ ULONG *pcbWritten)
{
CLIENT_CALL_RETURN _RetVal;

_RetVal = NdrClientCall2(
                  ( PMIDL_STUB_DESC  )&Object_StubDesc,
                  (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[5200],
                  ( unsigned char * )This,
                  pv,
                  cb,
                  pcbWritten);
return ( HRESULT  )_RetVal.Simple;

}

void __RPC_API
IFillLockBytes_RemoteFillAppend_Thunk(
    PMIDL_STUB_MESSAGE pStubMsg )
{
    #pragma pack(8)
    struct _PARAM_STRUCT
        {
        IFillLockBytes *This;
        const byte *pv;
        ULONG cb;
        char Pad0[4];
        ULONG *pcbWritten;
        HRESULT _RetVal;
        };
    #pragma pack()
    struct _PARAM_STRUCT * pParamStruct;

    pParamStruct = (struct _PARAM_STRUCT *) pStubMsg->StackTop;
    
    /* Call the server */
    
    pParamStruct->_RetVal = IFillLockBytes_FillAppend_Stub(
                                         (IFillLockBytes *) pParamStruct->This,
                                         pParamStruct->pv,
                                         pParamStruct->cb,
                                         pParamStruct->pcbWritten);
    
}

/* [call_as] */ HRESULT __stdcall IFillLockBytes_RemoteFillAt_Proxy( 
    IFillLockBytes * This,
    /* [in] */ ULARGE_INTEGER ulOffset,
    /* [size_is][in] */ const byte *pv,
    /* [in] */ ULONG cb,
    /* [out] */ ULONG *pcbWritten)
{
CLIENT_CALL_RETURN _RetVal;

_RetVal = NdrClientCall2(
                  ( PMIDL_STUB_DESC  )&Object_StubDesc,
                  (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[3718],
                  ( unsigned char * )This,
                  ulOffset,
                  pv,
                  cb,
                  pcbWritten);
return ( HRESULT  )_RetVal.Simple;

}

void __RPC_API
IFillLockBytes_RemoteFillAt_Thunk(
    PMIDL_STUB_MESSAGE pStubMsg )
{
    #pragma pack(8)
    struct _PARAM_STRUCT
        {
        IFillLockBytes *This;
        ULARGE_INTEGER ulOffset;
        const byte *pv;
        ULONG cb;
        char Pad0[4];
        ULONG *pcbWritten;
        HRESULT _RetVal;
        };
    #pragma pack()
    struct _PARAM_STRUCT * pParamStruct;

    pParamStruct = (struct _PARAM_STRUCT *) pStubMsg->StackTop;
    
    /* Call the server */
    
    pParamStruct->_RetVal = IFillLockBytes_FillAt_Stub(
                                     (IFillLockBytes *) pParamStruct->This,
                                     pParamStruct->ulOffset,
                                     pParamStruct->pv,
                                     pParamStruct->cb,
                                     pParamStruct->pcbWritten);
    
}


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO IProgressNotify_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO IProgressNotify_ProxyInfo;


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO IBlockingLock_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO IBlockingLock_ProxyInfo;


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO ITimeAndNoticeControl_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO ITimeAndNoticeControl_ProxyInfo;


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO IOplockStorage_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO IOplockStorage_ProxyInfo;


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO ISurrogate_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO ISurrogate_ProxyInfo;


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO IDirectWriterLock_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO IDirectWriterLock_ProxyInfo;


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO ISynchronize_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO ISynchronize_ProxyInfo;


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO IUrlMon_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO IUrlMon_ProxyInfo;


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO IPipeByte_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO IPipeByte_ProxyInfo;


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO AsyncIPipeByte_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO AsyncIPipeByte_ProxyInfo;


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO IPipeLong_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO IPipeLong_ProxyInfo;


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO AsyncIPipeLong_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO AsyncIPipeLong_ProxyInfo;


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO IPipeDouble_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO IPipeDouble_ProxyInfo;


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO AsyncIPipeDouble_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO AsyncIPipeDouble_ProxyInfo;


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO IThumbnailExtractor_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO IThumbnailExtractor_ProxyInfo;


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO IDummyHICONIncluder_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO IDummyHICONIncluder_ProxyInfo;


extern const EXPR_EVAL ExprEvalRoutines[];
extern const USER_MARSHAL_ROUTINE_QUADRUPLE UserMarshalRoutines[ WIRE_MARSHAL_TABLE_SIZE ];

#if !defined(__RPC_WIN64__)
#error  Invalid build platform for this stub.
#endif

static const MIDL_PROC_FORMAT_STRING __MIDL_ProcFormatString =
    {
        0,
        {

	/* Procedure RemoteNext */

			0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/*  2 */	NdrFcLong( 0x0 ),	/* 0 */
/*  6 */	NdrFcShort( 0x3 ),	/* 3 */
/*  8 */	NdrFcShort( 0x28 ),	/* ia64 Stack size/offset = 40 */
/* 10 */	NdrFcShort( 0x8 ),	/* 8 */
/* 12 */	NdrFcShort( 0x24 ),	/* 36 */
/* 14 */	0x45,		/* Oi2 Flags:  srv must size, has return, has ext, */
			0x4,		/* 4 */
/* 16 */	0xa,		/* 10 */
			0x3,		/* Ext Flags:  new corr desc, clt corr check, */
/* 18 */	NdrFcShort( 0x1 ),	/* 1 */
/* 20 */	NdrFcShort( 0x0 ),	/* 0 */
/* 22 */	NdrFcShort( 0x0 ),	/* 0 */
/* 24 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter celt */

/* 26 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 28 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 30 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter rgelt */

/* 32 */	NdrFcShort( 0x113 ),	/* Flags:  must size, must free, out, simple ref, */
/* 34 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 36 */	NdrFcShort( 0x18 ),	/* Type Offset=24 */

	/* Parameter pceltFetched */

/* 38 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 40 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 42 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 44 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 46 */	NdrFcShort( 0x20 ),	/* ia64 Stack size/offset = 32 */
/* 48 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure Skip */


	/* Procedure Skip */


	/* Procedure Skip */


	/* Procedure Skip */


	/* Procedure Revoke */


	/* Procedure Skip */


	/* Procedure Skip */

/* 50 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 52 */	NdrFcLong( 0x0 ),	/* 0 */
/* 56 */	NdrFcShort( 0x4 ),	/* 4 */
/* 58 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 60 */	NdrFcShort( 0x8 ),	/* 8 */
/* 62 */	NdrFcShort( 0x8 ),	/* 8 */
/* 64 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x2,		/* 2 */
/* 66 */	0xa,		/* 10 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 68 */	NdrFcShort( 0x0 ),	/* 0 */
/* 70 */	NdrFcShort( 0x0 ),	/* 0 */
/* 72 */	NdrFcShort( 0x0 ),	/* 0 */
/* 74 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter celt */


	/* Parameter celt */


	/* Parameter celt */


	/* Parameter celt */


	/* Parameter dwRegister */


	/* Parameter celt */


	/* Parameter celt */

/* 76 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 78 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 80 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */


	/* Return value */


	/* Return value */


	/* Return value */


	/* Return value */


	/* Return value */


	/* Return value */

/* 82 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 84 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 86 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure Reset */


	/* Procedure HaveWriteAccess */


	/* Procedure Reset */


	/* Procedure Reset */


	/* Procedure Flush */


	/* Procedure Reset */


	/* Procedure Reset */


	/* Procedure RemoteIsRunning */


	/* Procedure Reset */


	/* Procedure ReleaseBoundObjects */


	/* Procedure Reset */

/* 88 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 90 */	NdrFcLong( 0x0 ),	/* 0 */
/* 94 */	NdrFcShort( 0x5 ),	/* 5 */
/* 96 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 98 */	NdrFcShort( 0x0 ),	/* 0 */
/* 100 */	NdrFcShort( 0x8 ),	/* 8 */
/* 102 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x1,		/* 1 */
/* 104 */	0xa,		/* 10 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 106 */	NdrFcShort( 0x0 ),	/* 0 */
/* 108 */	NdrFcShort( 0x0 ),	/* 0 */
/* 110 */	NdrFcShort( 0x0 ),	/* 0 */
/* 112 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Return value */


	/* Return value */


	/* Return value */


	/* Return value */


	/* Return value */


	/* Return value */


	/* Return value */


	/* Return value */


	/* Return value */


	/* Return value */


	/* Return value */

/* 114 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 116 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 118 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure Clone */

/* 120 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 122 */	NdrFcLong( 0x0 ),	/* 0 */
/* 126 */	NdrFcShort( 0x6 ),	/* 6 */
/* 128 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 130 */	NdrFcShort( 0x0 ),	/* 0 */
/* 132 */	NdrFcShort( 0x8 ),	/* 8 */
/* 134 */	0x45,		/* Oi2 Flags:  srv must size, has return, has ext, */
			0x2,		/* 2 */
/* 136 */	0xa,		/* 10 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 138 */	NdrFcShort( 0x0 ),	/* 0 */
/* 140 */	NdrFcShort( 0x0 ),	/* 0 */
/* 142 */	NdrFcShort( 0x0 ),	/* 0 */
/* 144 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter ppenum */

/* 146 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
/* 148 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 150 */	NdrFcShort( 0x32 ),	/* Type Offset=50 */

	/* Return value */

/* 152 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 154 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 156 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure RegisterObjectBound */

/* 158 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 160 */	NdrFcLong( 0x0 ),	/* 0 */
/* 164 */	NdrFcShort( 0x3 ),	/* 3 */
/* 166 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 168 */	NdrFcShort( 0x0 ),	/* 0 */
/* 170 */	NdrFcShort( 0x8 ),	/* 8 */
/* 172 */	0x46,		/* Oi2 Flags:  clt must size, has return, has ext, */
			0x2,		/* 2 */
/* 174 */	0xa,		/* 10 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 176 */	NdrFcShort( 0x0 ),	/* 0 */
/* 178 */	NdrFcShort( 0x0 ),	/* 0 */
/* 180 */	NdrFcShort( 0x0 ),	/* 0 */
/* 182 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter punk */

/* 184 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 186 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 188 */	NdrFcShort( 0x6 ),	/* Type Offset=6 */

	/* Return value */

/* 190 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 192 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 194 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure RevokeObjectBound */

/* 196 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 198 */	NdrFcLong( 0x0 ),	/* 0 */
/* 202 */	NdrFcShort( 0x4 ),	/* 4 */
/* 204 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 206 */	NdrFcShort( 0x0 ),	/* 0 */
/* 208 */	NdrFcShort( 0x8 ),	/* 8 */
/* 210 */	0x46,		/* Oi2 Flags:  clt must size, has return, has ext, */
			0x2,		/* 2 */
/* 212 */	0xa,		/* 10 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 214 */	NdrFcShort( 0x0 ),	/* 0 */
/* 216 */	NdrFcShort( 0x0 ),	/* 0 */
/* 218 */	NdrFcShort( 0x0 ),	/* 0 */
/* 220 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter punk */

/* 222 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 224 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 226 */	NdrFcShort( 0x6 ),	/* Type Offset=6 */

	/* Return value */

/* 228 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 230 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 232 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure RemoteSetBindOptions */

/* 234 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 236 */	NdrFcLong( 0x0 ),	/* 0 */
/* 240 */	NdrFcShort( 0x6 ),	/* 6 */
/* 242 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 244 */	NdrFcShort( 0x0 ),	/* 0 */
/* 246 */	NdrFcShort( 0x8 ),	/* 8 */
/* 248 */	0x46,		/* Oi2 Flags:  clt must size, has return, has ext, */
			0x2,		/* 2 */
/* 250 */	0xa,		/* 10 */
			0x5,		/* Ext Flags:  new corr desc, srv corr check, */
/* 252 */	NdrFcShort( 0x0 ),	/* 0 */
/* 254 */	NdrFcShort( 0x3 ),	/* 3 */
/* 256 */	NdrFcShort( 0x0 ),	/* 0 */
/* 258 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pbindopts */

/* 260 */	NdrFcShort( 0x10b ),	/* Flags:  must size, must free, in, simple ref, */
/* 262 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 264 */	NdrFcShort( 0xc0 ),	/* Type Offset=192 */

	/* Return value */

/* 266 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 268 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 270 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure RemoteGetBindOptions */

/* 272 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 274 */	NdrFcLong( 0x0 ),	/* 0 */
/* 278 */	NdrFcShort( 0x7 ),	/* 7 */
/* 280 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 282 */	NdrFcShort( 0x0 ),	/* 0 */
/* 284 */	NdrFcShort( 0x8 ),	/* 8 */
/* 286 */	0x47,		/* Oi2 Flags:  srv must size, clt must size, has return, has ext, */
			0x2,		/* 2 */
/* 288 */	0xa,		/* 10 */
			0x7,		/* Ext Flags:  new corr desc, clt corr check, srv corr check, */
/* 290 */	NdrFcShort( 0x3 ),	/* 3 */
/* 292 */	NdrFcShort( 0x3 ),	/* 3 */
/* 294 */	NdrFcShort( 0x0 ),	/* 0 */
/* 296 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pbindopts */

/* 298 */	NdrFcShort( 0x11b ),	/* Flags:  must size, must free, in, out, simple ref, */
/* 300 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 302 */	NdrFcShort( 0xc0 ),	/* Type Offset=192 */

	/* Return value */

/* 304 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 306 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 308 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetRunningObjectTable */

/* 310 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 312 */	NdrFcLong( 0x0 ),	/* 0 */
/* 316 */	NdrFcShort( 0x8 ),	/* 8 */
/* 318 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 320 */	NdrFcShort( 0x0 ),	/* 0 */
/* 322 */	NdrFcShort( 0x8 ),	/* 8 */
/* 324 */	0x45,		/* Oi2 Flags:  srv must size, has return, has ext, */
			0x2,		/* 2 */
/* 326 */	0xa,		/* 10 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 328 */	NdrFcShort( 0x0 ),	/* 0 */
/* 330 */	NdrFcShort( 0x0 ),	/* 0 */
/* 332 */	NdrFcShort( 0x0 ),	/* 0 */
/* 334 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pprot */

/* 336 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
/* 338 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 340 */	NdrFcShort( 0xd6 ),	/* Type Offset=214 */

	/* Return value */

/* 342 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 344 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 346 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure RegisterObjectParam */

/* 348 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 350 */	NdrFcLong( 0x0 ),	/* 0 */
/* 354 */	NdrFcShort( 0x9 ),	/* 9 */
/* 356 */	NdrFcShort( 0x20 ),	/* ia64 Stack size/offset = 32 */
/* 358 */	NdrFcShort( 0x0 ),	/* 0 */
/* 360 */	NdrFcShort( 0x8 ),	/* 8 */
/* 362 */	0x46,		/* Oi2 Flags:  clt must size, has return, has ext, */
			0x3,		/* 3 */
/* 364 */	0xa,		/* 10 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 366 */	NdrFcShort( 0x0 ),	/* 0 */
/* 368 */	NdrFcShort( 0x0 ),	/* 0 */
/* 370 */	NdrFcShort( 0x0 ),	/* 0 */
/* 372 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pszKey */

/* 374 */	NdrFcShort( 0x10b ),	/* Flags:  must size, must free, in, simple ref, */
/* 376 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 378 */	NdrFcShort( 0xee ),	/* Type Offset=238 */

	/* Parameter punk */

/* 380 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 382 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 384 */	NdrFcShort( 0x6 ),	/* Type Offset=6 */

	/* Return value */

/* 386 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 388 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 390 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetObjectParam */

/* 392 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 394 */	NdrFcLong( 0x0 ),	/* 0 */
/* 398 */	NdrFcShort( 0xa ),	/* 10 */
/* 400 */	NdrFcShort( 0x20 ),	/* ia64 Stack size/offset = 32 */
/* 402 */	NdrFcShort( 0x0 ),	/* 0 */
/* 404 */	NdrFcShort( 0x8 ),	/* 8 */
/* 406 */	0x47,		/* Oi2 Flags:  srv must size, clt must size, has return, has ext, */
			0x3,		/* 3 */
/* 408 */	0xa,		/* 10 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 410 */	NdrFcShort( 0x0 ),	/* 0 */
/* 412 */	NdrFcShort( 0x0 ),	/* 0 */
/* 414 */	NdrFcShort( 0x0 ),	/* 0 */
/* 416 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pszKey */

/* 418 */	NdrFcShort( 0x10b ),	/* Flags:  must size, must free, in, simple ref, */
/* 420 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 422 */	NdrFcShort( 0xee ),	/* Type Offset=238 */

	/* Parameter ppunk */

/* 424 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
/* 426 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 428 */	NdrFcShort( 0xf0 ),	/* Type Offset=240 */

	/* Return value */

/* 430 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 432 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 434 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure EnumObjectParam */

/* 436 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 438 */	NdrFcLong( 0x0 ),	/* 0 */
/* 442 */	NdrFcShort( 0xb ),	/* 11 */
/* 444 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 446 */	NdrFcShort( 0x0 ),	/* 0 */
/* 448 */	NdrFcShort( 0x8 ),	/* 8 */
/* 450 */	0x45,		/* Oi2 Flags:  srv must size, has return, has ext, */
			0x2,		/* 2 */
/* 452 */	0xa,		/* 10 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 454 */	NdrFcShort( 0x0 ),	/* 0 */
/* 456 */	NdrFcShort( 0x0 ),	/* 0 */
/* 458 */	NdrFcShort( 0x0 ),	/* 0 */
/* 460 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter ppenum */

/* 462 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
/* 464 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 466 */	NdrFcShort( 0xf4 ),	/* Type Offset=244 */

	/* Return value */

/* 468 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 470 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 472 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure DestroyElement */


	/* Procedure RevokeObjectParam */

/* 474 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 476 */	NdrFcLong( 0x0 ),	/* 0 */
/* 480 */	NdrFcShort( 0xc ),	/* 12 */
/* 482 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 484 */	NdrFcShort( 0x0 ),	/* 0 */
/* 486 */	NdrFcShort( 0x8 ),	/* 8 */
/* 488 */	0x46,		/* Oi2 Flags:  clt must size, has return, has ext, */
			0x2,		/* 2 */
/* 490 */	0xa,		/* 10 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 492 */	NdrFcShort( 0x0 ),	/* 0 */
/* 494 */	NdrFcShort( 0x0 ),	/* 0 */
/* 496 */	NdrFcShort( 0x0 ),	/* 0 */
/* 498 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pwcsName */


	/* Parameter pszKey */

/* 500 */	NdrFcShort( 0x10b ),	/* Flags:  must size, must free, in, simple ref, */
/* 502 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 504 */	NdrFcShort( 0xee ),	/* Type Offset=238 */

	/* Return value */


	/* Return value */

/* 506 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 508 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 510 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure RemoteNext */

/* 512 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 514 */	NdrFcLong( 0x0 ),	/* 0 */
/* 518 */	NdrFcShort( 0x3 ),	/* 3 */
/* 520 */	NdrFcShort( 0x28 ),	/* ia64 Stack size/offset = 40 */
/* 522 */	NdrFcShort( 0x8 ),	/* 8 */
/* 524 */	NdrFcShort( 0x24 ),	/* 36 */
/* 526 */	0x45,		/* Oi2 Flags:  srv must size, has return, has ext, */
			0x4,		/* 4 */
/* 528 */	0xa,		/* 10 */
			0x3,		/* Ext Flags:  new corr desc, clt corr check, */
/* 530 */	NdrFcShort( 0x1 ),	/* 1 */
/* 532 */	NdrFcShort( 0x0 ),	/* 0 */
/* 534 */	NdrFcShort( 0x0 ),	/* 0 */
/* 536 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter celt */

/* 538 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 540 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 542 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter rgelt */

/* 544 */	NdrFcShort( 0x113 ),	/* Flags:  must size, must free, out, simple ref, */
/* 546 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 548 */	NdrFcShort( 0x120 ),	/* Type Offset=288 */

	/* Parameter pceltFetched */

/* 550 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 552 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 554 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 556 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 558 */	NdrFcShort( 0x20 ),	/* ia64 Stack size/offset = 32 */
/* 560 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure Clone */

/* 562 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 564 */	NdrFcLong( 0x0 ),	/* 0 */
/* 568 */	NdrFcShort( 0x6 ),	/* 6 */
/* 570 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 572 */	NdrFcShort( 0x0 ),	/* 0 */
/* 574 */	NdrFcShort( 0x8 ),	/* 8 */
/* 576 */	0x45,		/* Oi2 Flags:  srv must size, has return, has ext, */
			0x2,		/* 2 */
/* 578 */	0xa,		/* 10 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 580 */	NdrFcShort( 0x0 ),	/* 0 */
/* 582 */	NdrFcShort( 0x0 ),	/* 0 */
/* 584 */	NdrFcShort( 0x0 ),	/* 0 */
/* 586 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter ppenum */

/* 588 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
/* 590 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 592 */	NdrFcShort( 0x136 ),	/* Type Offset=310 */

	/* Return value */

/* 594 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 596 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 598 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetClassID */


	/* Procedure GetRunningClass */

/* 600 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 602 */	NdrFcLong( 0x0 ),	/* 0 */
/* 606 */	NdrFcShort( 0x3 ),	/* 3 */
/* 608 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 610 */	NdrFcShort( 0x0 ),	/* 0 */
/* 612 */	NdrFcShort( 0x4c ),	/* 76 */
/* 614 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x2,		/* 2 */
/* 616 */	0xa,		/* 10 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 618 */	NdrFcShort( 0x0 ),	/* 0 */
/* 620 */	NdrFcShort( 0x0 ),	/* 0 */
/* 622 */	NdrFcShort( 0x0 ),	/* 0 */
/* 624 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pClassID */


	/* Parameter lpClsid */

/* 626 */	NdrFcShort( 0x4112 ),	/* Flags:  must free, out, simple ref, srv alloc size=16 */
/* 628 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 630 */	NdrFcShort( 0x156 ),	/* Type Offset=342 */

	/* Return value */


	/* Return value */

/* 632 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 634 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 636 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure Run */

/* 638 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 640 */	NdrFcLong( 0x0 ),	/* 0 */
/* 644 */	NdrFcShort( 0x4 ),	/* 4 */
/* 646 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 648 */	NdrFcShort( 0x0 ),	/* 0 */
/* 650 */	NdrFcShort( 0x8 ),	/* 8 */
/* 652 */	0x46,		/* Oi2 Flags:  clt must size, has return, has ext, */
			0x2,		/* 2 */
/* 654 */	0xa,		/* 10 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 656 */	NdrFcShort( 0x0 ),	/* 0 */
/* 658 */	NdrFcShort( 0x0 ),	/* 0 */
/* 660 */	NdrFcShort( 0x0 ),	/* 0 */
/* 662 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pbc */

/* 664 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 666 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 668 */	NdrFcShort( 0x162 ),	/* Type Offset=354 */

	/* Return value */

/* 670 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 672 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 674 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure LockRunning */

/* 676 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 678 */	NdrFcLong( 0x0 ),	/* 0 */
/* 682 */	NdrFcShort( 0x6 ),	/* 6 */
/* 684 */	NdrFcShort( 0x20 ),	/* ia64 Stack size/offset = 32 */
/* 686 */	NdrFcShort( 0x10 ),	/* 16 */
/* 688 */	NdrFcShort( 0x8 ),	/* 8 */
/* 690 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x3,		/* 3 */
/* 692 */	0xa,		/* 10 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 694 */	NdrFcShort( 0x0 ),	/* 0 */
/* 696 */	NdrFcShort( 0x0 ),	/* 0 */
/* 698 */	NdrFcShort( 0x0 ),	/* 0 */
/* 700 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter fLock */

/* 702 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 704 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 706 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter fLastUnlockCloses */

/* 708 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 710 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 712 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 714 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 716 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 718 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure SetContainedObject */

/* 720 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 722 */	NdrFcLong( 0x0 ),	/* 0 */
/* 726 */	NdrFcShort( 0x7 ),	/* 7 */
/* 728 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 730 */	NdrFcShort( 0x8 ),	/* 8 */
/* 732 */	NdrFcShort( 0x8 ),	/* 8 */
/* 734 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x2,		/* 2 */
/* 736 */	0xa,		/* 10 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 738 */	NdrFcShort( 0x0 ),	/* 0 */
/* 740 */	NdrFcShort( 0x0 ),	/* 0 */
/* 742 */	NdrFcShort( 0x0 ),	/* 0 */
/* 744 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter fContained */

/* 746 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 748 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 750 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 752 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 754 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 756 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure Register */

/* 758 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 760 */	NdrFcLong( 0x0 ),	/* 0 */
/* 764 */	NdrFcShort( 0x3 ),	/* 3 */
/* 766 */	NdrFcShort( 0x30 ),	/* ia64 Stack size/offset = 48 */
/* 768 */	NdrFcShort( 0x8 ),	/* 8 */
/* 770 */	NdrFcShort( 0x24 ),	/* 36 */
/* 772 */	0x46,		/* Oi2 Flags:  clt must size, has return, has ext, */
			0x5,		/* 5 */
/* 774 */	0xa,		/* 10 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 776 */	NdrFcShort( 0x0 ),	/* 0 */
/* 778 */	NdrFcShort( 0x0 ),	/* 0 */
/* 780 */	NdrFcShort( 0x0 ),	/* 0 */
/* 782 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter grfFlags */

/* 784 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 786 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 788 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter punkObject */

/* 790 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 792 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 794 */	NdrFcShort( 0x6 ),	/* Type Offset=6 */

	/* Parameter pmkObjectName */

/* 796 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 798 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 800 */	NdrFcShort( 0x10e ),	/* Type Offset=270 */

	/* Parameter pdwRegister */

/* 802 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 804 */	NdrFcShort( 0x20 ),	/* ia64 Stack size/offset = 32 */
/* 806 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 808 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 810 */	NdrFcShort( 0x28 ),	/* ia64 Stack size/offset = 40 */
/* 812 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure RemoteOnRename */


	/* Procedure IsRunning */

/* 814 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 816 */	NdrFcLong( 0x0 ),	/* 0 */
/* 820 */	NdrFcShort( 0x5 ),	/* 5 */
/* 822 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 824 */	NdrFcShort( 0x0 ),	/* 0 */
/* 826 */	NdrFcShort( 0x8 ),	/* 8 */
/* 828 */	0x46,		/* Oi2 Flags:  clt must size, has return, has ext, */
			0x2,		/* 2 */
/* 830 */	0xa,		/* 10 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 832 */	NdrFcShort( 0x0 ),	/* 0 */
/* 834 */	NdrFcShort( 0x0 ),	/* 0 */
/* 836 */	NdrFcShort( 0x0 ),	/* 0 */
/* 838 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pmk */


	/* Parameter pmkObjectName */

/* 840 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 842 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 844 */	NdrFcShort( 0x10e ),	/* Type Offset=270 */

	/* Return value */


	/* Return value */

/* 846 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 848 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 850 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetObject */

/* 852 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 854 */	NdrFcLong( 0x0 ),	/* 0 */
/* 858 */	NdrFcShort( 0x6 ),	/* 6 */
/* 860 */	NdrFcShort( 0x20 ),	/* ia64 Stack size/offset = 32 */
/* 862 */	NdrFcShort( 0x0 ),	/* 0 */
/* 864 */	NdrFcShort( 0x8 ),	/* 8 */
/* 866 */	0x47,		/* Oi2 Flags:  srv must size, clt must size, has return, has ext, */
			0x3,		/* 3 */
/* 868 */	0xa,		/* 10 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 870 */	NdrFcShort( 0x0 ),	/* 0 */
/* 872 */	NdrFcShort( 0x0 ),	/* 0 */
/* 874 */	NdrFcShort( 0x0 ),	/* 0 */
/* 876 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pmkObjectName */

/* 878 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 880 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 882 */	NdrFcShort( 0x10e ),	/* Type Offset=270 */

	/* Parameter ppunkObject */

/* 884 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
/* 886 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 888 */	NdrFcShort( 0xf0 ),	/* Type Offset=240 */

	/* Return value */

/* 890 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 892 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 894 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure NoteChangeTime */

/* 896 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 898 */	NdrFcLong( 0x0 ),	/* 0 */
/* 902 */	NdrFcShort( 0x7 ),	/* 7 */
/* 904 */	NdrFcShort( 0x20 ),	/* ia64 Stack size/offset = 32 */
/* 906 */	NdrFcShort( 0x34 ),	/* 52 */
/* 908 */	NdrFcShort( 0x8 ),	/* 8 */
/* 910 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x3,		/* 3 */
/* 912 */	0xa,		/* 10 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 914 */	NdrFcShort( 0x0 ),	/* 0 */
/* 916 */	NdrFcShort( 0x0 ),	/* 0 */
/* 918 */	NdrFcShort( 0x0 ),	/* 0 */
/* 920 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter dwRegister */

/* 922 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 924 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 926 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter pfiletime */

/* 928 */	NdrFcShort( 0x10a ),	/* Flags:  must free, in, simple ref, */
/* 930 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 932 */	NdrFcShort( 0x178 ),	/* Type Offset=376 */

	/* Return value */

/* 934 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 936 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 938 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetTimeOfLastChange */

/* 940 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 942 */	NdrFcLong( 0x0 ),	/* 0 */
/* 946 */	NdrFcShort( 0x8 ),	/* 8 */
/* 948 */	NdrFcShort( 0x20 ),	/* ia64 Stack size/offset = 32 */
/* 950 */	NdrFcShort( 0x0 ),	/* 0 */
/* 952 */	NdrFcShort( 0x34 ),	/* 52 */
/* 954 */	0x46,		/* Oi2 Flags:  clt must size, has return, has ext, */
			0x3,		/* 3 */
/* 956 */	0xa,		/* 10 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 958 */	NdrFcShort( 0x0 ),	/* 0 */
/* 960 */	NdrFcShort( 0x0 ),	/* 0 */
/* 962 */	NdrFcShort( 0x0 ),	/* 0 */
/* 964 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pmkObjectName */

/* 966 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 968 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 970 */	NdrFcShort( 0x10e ),	/* Type Offset=270 */

	/* Parameter pfiletime */

/* 972 */	NdrFcShort( 0x2112 ),	/* Flags:  must free, out, simple ref, srv alloc size=8 */
/* 974 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 976 */	NdrFcShort( 0x178 ),	/* Type Offset=376 */

	/* Return value */

/* 978 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 980 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 982 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure EnumRunning */

/* 984 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 986 */	NdrFcLong( 0x0 ),	/* 0 */
/* 990 */	NdrFcShort( 0x9 ),	/* 9 */
/* 992 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 994 */	NdrFcShort( 0x0 ),	/* 0 */
/* 996 */	NdrFcShort( 0x8 ),	/* 8 */
/* 998 */	0x45,		/* Oi2 Flags:  srv must size, has return, has ext, */
			0x2,		/* 2 */
/* 1000 */	0xa,		/* 10 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 1002 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1004 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1006 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1008 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter ppenumMoniker */

/* 1010 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
/* 1012 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 1014 */	NdrFcShort( 0x136 ),	/* Type Offset=310 */

	/* Return value */

/* 1016 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1018 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 1020 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure Signal */


	/* Procedure ReleaseWriteAccess */


	/* Procedure FreeSurrogate */


	/* Procedure Unlock */


	/* Procedure IsDirty */


	/* Procedure IsDirty */


	/* Procedure IsDirty */

/* 1022 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 1024 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1028 */	NdrFcShort( 0x4 ),	/* 4 */
/* 1030 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 1032 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1034 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1036 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x1,		/* 1 */
/* 1038 */	0xa,		/* 10 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 1040 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1042 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1044 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1046 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Return value */


	/* Return value */


	/* Return value */


	/* Return value */


	/* Return value */


	/* Return value */


	/* Return value */

/* 1048 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1050 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 1052 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure Load */

/* 1054 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 1056 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1060 */	NdrFcShort( 0x5 ),	/* 5 */
/* 1062 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 1064 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1066 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1068 */	0x46,		/* Oi2 Flags:  clt must size, has return, has ext, */
			0x2,		/* 2 */
/* 1070 */	0xa,		/* 10 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 1072 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1074 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1076 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1078 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pStm */

/* 1080 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 1082 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 1084 */	NdrFcShort( 0x184 ),	/* Type Offset=388 */

	/* Return value */

/* 1086 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1088 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 1090 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure Save */

/* 1092 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 1094 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1098 */	NdrFcShort( 0x6 ),	/* 6 */
/* 1100 */	NdrFcShort( 0x20 ),	/* ia64 Stack size/offset = 32 */
/* 1102 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1104 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1106 */	0x46,		/* Oi2 Flags:  clt must size, has return, has ext, */
			0x3,		/* 3 */
/* 1108 */	0xa,		/* 10 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 1110 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1112 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1114 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1116 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pStm */

/* 1118 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 1120 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 1122 */	NdrFcShort( 0x184 ),	/* Type Offset=388 */

	/* Parameter fClearDirty */

/* 1124 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 1126 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 1128 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 1130 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1132 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 1134 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetSizeMax */

/* 1136 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 1138 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1142 */	NdrFcShort( 0x7 ),	/* 7 */
/* 1144 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 1146 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1148 */	NdrFcShort( 0x34 ),	/* 52 */
/* 1150 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x2,		/* 2 */
/* 1152 */	0xa,		/* 10 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 1154 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1156 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1158 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1160 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pcbSize */

/* 1162 */	NdrFcShort( 0x2112 ),	/* Flags:  must free, out, simple ref, srv alloc size=8 */
/* 1164 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 1166 */	NdrFcShort( 0x19a ),	/* Type Offset=410 */

	/* Return value */

/* 1168 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1170 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 1172 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure RemoteBindToObject */

/* 1174 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 1176 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1180 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1182 */	NdrFcShort( 0x30 ),	/* ia64 Stack size/offset = 48 */
/* 1184 */	NdrFcShort( 0x44 ),	/* 68 */
/* 1186 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1188 */	0x47,		/* Oi2 Flags:  srv must size, clt must size, has return, has ext, */
			0x5,		/* 5 */
/* 1190 */	0xa,		/* 10 */
			0x3,		/* Ext Flags:  new corr desc, clt corr check, */
/* 1192 */	NdrFcShort( 0x1 ),	/* 1 */
/* 1194 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1196 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1198 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pbc */

/* 1200 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 1202 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 1204 */	NdrFcShort( 0x162 ),	/* Type Offset=354 */

	/* Parameter pmkToLeft */

/* 1206 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 1208 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 1210 */	NdrFcShort( 0x10e ),	/* Type Offset=270 */

	/* Parameter riidResult */

/* 1212 */	NdrFcShort( 0x10a ),	/* Flags:  must free, in, simple ref, */
/* 1214 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 1216 */	NdrFcShort( 0x156 ),	/* Type Offset=342 */

	/* Parameter ppvResult */

/* 1218 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
/* 1220 */	NdrFcShort( 0x20 ),	/* ia64 Stack size/offset = 32 */
/* 1222 */	NdrFcShort( 0x1a4 ),	/* Type Offset=420 */

	/* Return value */

/* 1224 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1226 */	NdrFcShort( 0x28 ),	/* ia64 Stack size/offset = 40 */
/* 1228 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure RemoteBindToStorage */

/* 1230 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 1232 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1236 */	NdrFcShort( 0x9 ),	/* 9 */
/* 1238 */	NdrFcShort( 0x30 ),	/* ia64 Stack size/offset = 48 */
/* 1240 */	NdrFcShort( 0x44 ),	/* 68 */
/* 1242 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1244 */	0x47,		/* Oi2 Flags:  srv must size, clt must size, has return, has ext, */
			0x5,		/* 5 */
/* 1246 */	0xa,		/* 10 */
			0x3,		/* Ext Flags:  new corr desc, clt corr check, */
/* 1248 */	NdrFcShort( 0x1 ),	/* 1 */
/* 1250 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1252 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1254 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pbc */

/* 1256 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 1258 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 1260 */	NdrFcShort( 0x162 ),	/* Type Offset=354 */

	/* Parameter pmkToLeft */

/* 1262 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 1264 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 1266 */	NdrFcShort( 0x10e ),	/* Type Offset=270 */

	/* Parameter riid */

/* 1268 */	NdrFcShort( 0x10a ),	/* Flags:  must free, in, simple ref, */
/* 1270 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 1272 */	NdrFcShort( 0x156 ),	/* Type Offset=342 */

	/* Parameter ppvObj */

/* 1274 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
/* 1276 */	NdrFcShort( 0x20 ),	/* ia64 Stack size/offset = 32 */
/* 1278 */	NdrFcShort( 0x1b0 ),	/* Type Offset=432 */

	/* Return value */

/* 1280 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1282 */	NdrFcShort( 0x28 ),	/* ia64 Stack size/offset = 40 */
/* 1284 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure Reduce */

/* 1286 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 1288 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1292 */	NdrFcShort( 0xa ),	/* 10 */
/* 1294 */	NdrFcShort( 0x30 ),	/* ia64 Stack size/offset = 48 */
/* 1296 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1298 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1300 */	0x47,		/* Oi2 Flags:  srv must size, clt must size, has return, has ext, */
			0x5,		/* 5 */
/* 1302 */	0xa,		/* 10 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 1304 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1306 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1308 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1310 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pbc */

/* 1312 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 1314 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 1316 */	NdrFcShort( 0x162 ),	/* Type Offset=354 */

	/* Parameter dwReduceHowFar */

/* 1318 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 1320 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 1322 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter ppmkToLeft */

/* 1324 */	NdrFcShort( 0x201b ),	/* Flags:  must size, must free, in, out, srv alloc size=8 */
/* 1326 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 1328 */	NdrFcShort( 0x1bc ),	/* Type Offset=444 */

	/* Parameter ppmkReduced */

/* 1330 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
/* 1332 */	NdrFcShort( 0x20 ),	/* ia64 Stack size/offset = 32 */
/* 1334 */	NdrFcShort( 0x1c0 ),	/* Type Offset=448 */

	/* Return value */

/* 1336 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1338 */	NdrFcShort( 0x28 ),	/* ia64 Stack size/offset = 40 */
/* 1340 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure ComposeWith */

/* 1342 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 1344 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1348 */	NdrFcShort( 0xb ),	/* 11 */
/* 1350 */	NdrFcShort( 0x28 ),	/* ia64 Stack size/offset = 40 */
/* 1352 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1354 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1356 */	0x47,		/* Oi2 Flags:  srv must size, clt must size, has return, has ext, */
			0x4,		/* 4 */
/* 1358 */	0xa,		/* 10 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 1360 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1362 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1364 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1366 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pmkRight */

/* 1368 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 1370 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 1372 */	NdrFcShort( 0x10e ),	/* Type Offset=270 */

	/* Parameter fOnlyIfNotGeneric */

/* 1374 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 1376 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 1378 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter ppmkComposite */

/* 1380 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
/* 1382 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 1384 */	NdrFcShort( 0x1c0 ),	/* Type Offset=448 */

	/* Return value */

/* 1386 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1388 */	NdrFcShort( 0x20 ),	/* ia64 Stack size/offset = 32 */
/* 1390 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure Enum */

/* 1392 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 1394 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1398 */	NdrFcShort( 0xc ),	/* 12 */
/* 1400 */	NdrFcShort( 0x20 ),	/* ia64 Stack size/offset = 32 */
/* 1402 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1404 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1406 */	0x45,		/* Oi2 Flags:  srv must size, has return, has ext, */
			0x3,		/* 3 */
/* 1408 */	0xa,		/* 10 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 1410 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1412 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1414 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1416 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter fForward */

/* 1418 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 1420 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 1422 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter ppenumMoniker */

/* 1424 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
/* 1426 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 1428 */	NdrFcShort( 0x136 ),	/* Type Offset=310 */

	/* Return value */

/* 1430 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1432 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 1434 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure IsEqual */

/* 1436 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 1438 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1442 */	NdrFcShort( 0xd ),	/* 13 */
/* 1444 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 1446 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1448 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1450 */	0x46,		/* Oi2 Flags:  clt must size, has return, has ext, */
			0x2,		/* 2 */
/* 1452 */	0xa,		/* 10 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 1454 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1456 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1458 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1460 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pmkOtherMoniker */

/* 1462 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 1464 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 1466 */	NdrFcShort( 0x10e ),	/* Type Offset=270 */

	/* Return value */

/* 1468 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1470 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 1472 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure Hash */

/* 1474 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 1476 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1480 */	NdrFcShort( 0xe ),	/* 14 */
/* 1482 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 1484 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1486 */	NdrFcShort( 0x24 ),	/* 36 */
/* 1488 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x2,		/* 2 */
/* 1490 */	0xa,		/* 10 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 1492 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1494 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1496 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1498 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pdwHash */

/* 1500 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 1502 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 1504 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 1506 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1508 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 1510 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure IsRunning */

/* 1512 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 1514 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1518 */	NdrFcShort( 0xf ),	/* 15 */
/* 1520 */	NdrFcShort( 0x28 ),	/* ia64 Stack size/offset = 40 */
/* 1522 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1524 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1526 */	0x46,		/* Oi2 Flags:  clt must size, has return, has ext, */
			0x4,		/* 4 */
/* 1528 */	0xa,		/* 10 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 1530 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1532 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1534 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1536 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pbc */

/* 1538 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 1540 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 1542 */	NdrFcShort( 0x162 ),	/* Type Offset=354 */

	/* Parameter pmkToLeft */

/* 1544 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 1546 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 1548 */	NdrFcShort( 0x10e ),	/* Type Offset=270 */

	/* Parameter pmkNewlyRunning */

/* 1550 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 1552 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 1554 */	NdrFcShort( 0x10e ),	/* Type Offset=270 */

	/* Return value */

/* 1556 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1558 */	NdrFcShort( 0x20 ),	/* ia64 Stack size/offset = 32 */
/* 1560 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetTimeOfLastChange */

/* 1562 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 1564 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1568 */	NdrFcShort( 0x10 ),	/* 16 */
/* 1570 */	NdrFcShort( 0x28 ),	/* ia64 Stack size/offset = 40 */
/* 1572 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1574 */	NdrFcShort( 0x34 ),	/* 52 */
/* 1576 */	0x46,		/* Oi2 Flags:  clt must size, has return, has ext, */
			0x4,		/* 4 */
/* 1578 */	0xa,		/* 10 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 1580 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1582 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1584 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1586 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pbc */

/* 1588 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 1590 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 1592 */	NdrFcShort( 0x162 ),	/* Type Offset=354 */

	/* Parameter pmkToLeft */

/* 1594 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 1596 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 1598 */	NdrFcShort( 0x10e ),	/* Type Offset=270 */

	/* Parameter pFileTime */

/* 1600 */	NdrFcShort( 0x2112 ),	/* Flags:  must free, out, simple ref, srv alloc size=8 */
/* 1602 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 1604 */	NdrFcShort( 0x178 ),	/* Type Offset=376 */

	/* Return value */

/* 1606 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1608 */	NdrFcShort( 0x20 ),	/* ia64 Stack size/offset = 32 */
/* 1610 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure Inverse */

/* 1612 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 1614 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1618 */	NdrFcShort( 0x11 ),	/* 17 */
/* 1620 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 1622 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1624 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1626 */	0x45,		/* Oi2 Flags:  srv must size, has return, has ext, */
			0x2,		/* 2 */
/* 1628 */	0xa,		/* 10 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 1630 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1632 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1634 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1636 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter ppmk */

/* 1638 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
/* 1640 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 1642 */	NdrFcShort( 0x1c0 ),	/* Type Offset=448 */

	/* Return value */

/* 1644 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1646 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 1648 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure CommonPrefixWith */

/* 1650 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 1652 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1656 */	NdrFcShort( 0x12 ),	/* 18 */
/* 1658 */	NdrFcShort( 0x20 ),	/* ia64 Stack size/offset = 32 */
/* 1660 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1662 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1664 */	0x47,		/* Oi2 Flags:  srv must size, clt must size, has return, has ext, */
			0x3,		/* 3 */
/* 1666 */	0xa,		/* 10 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 1668 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1670 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1672 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1674 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pmkOther */

/* 1676 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 1678 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 1680 */	NdrFcShort( 0x10e ),	/* Type Offset=270 */

	/* Parameter ppmkPrefix */

/* 1682 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
/* 1684 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 1686 */	NdrFcShort( 0x1c0 ),	/* Type Offset=448 */

	/* Return value */

/* 1688 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1690 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 1692 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure RelativePathTo */

/* 1694 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 1696 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1700 */	NdrFcShort( 0x13 ),	/* 19 */
/* 1702 */	NdrFcShort( 0x20 ),	/* ia64 Stack size/offset = 32 */
/* 1704 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1706 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1708 */	0x47,		/* Oi2 Flags:  srv must size, clt must size, has return, has ext, */
			0x3,		/* 3 */
/* 1710 */	0xa,		/* 10 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 1712 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1714 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1716 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1718 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pmkOther */

/* 1720 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 1722 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 1724 */	NdrFcShort( 0x10e ),	/* Type Offset=270 */

	/* Parameter ppmkRelPath */

/* 1726 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
/* 1728 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 1730 */	NdrFcShort( 0x1c0 ),	/* Type Offset=448 */

	/* Return value */

/* 1732 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1734 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 1736 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetDisplayName */

/* 1738 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 1740 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1744 */	NdrFcShort( 0x14 ),	/* 20 */
/* 1746 */	NdrFcShort( 0x28 ),	/* ia64 Stack size/offset = 40 */
/* 1748 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1750 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1752 */	0x47,		/* Oi2 Flags:  srv must size, clt must size, has return, has ext, */
			0x4,		/* 4 */
/* 1754 */	0xa,		/* 10 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 1756 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1758 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1760 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1762 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pbc */

/* 1764 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 1766 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 1768 */	NdrFcShort( 0x162 ),	/* Type Offset=354 */

	/* Parameter pmkToLeft */

/* 1770 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 1772 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 1774 */	NdrFcShort( 0x10e ),	/* Type Offset=270 */

	/* Parameter ppszDisplayName */

/* 1776 */	NdrFcShort( 0x2013 ),	/* Flags:  must size, must free, out, srv alloc size=8 */
/* 1778 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 1780 */	NdrFcShort( 0x1c4 ),	/* Type Offset=452 */

	/* Return value */

/* 1782 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1784 */	NdrFcShort( 0x20 ),	/* ia64 Stack size/offset = 32 */
/* 1786 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure ParseDisplayName */

/* 1788 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 1790 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1794 */	NdrFcShort( 0x15 ),	/* 21 */
/* 1796 */	NdrFcShort( 0x38 ),	/* ia64 Stack size/offset = 56 */
/* 1798 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1800 */	NdrFcShort( 0x24 ),	/* 36 */
/* 1802 */	0x47,		/* Oi2 Flags:  srv must size, clt must size, has return, has ext, */
			0x6,		/* 6 */
/* 1804 */	0xa,		/* 10 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 1806 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1808 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1810 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1812 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pbc */

/* 1814 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 1816 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 1818 */	NdrFcShort( 0x162 ),	/* Type Offset=354 */

	/* Parameter pmkToLeft */

/* 1820 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 1822 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 1824 */	NdrFcShort( 0x10e ),	/* Type Offset=270 */

	/* Parameter pszDisplayName */

/* 1826 */	NdrFcShort( 0x10b ),	/* Flags:  must size, must free, in, simple ref, */
/* 1828 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 1830 */	NdrFcShort( 0xee ),	/* Type Offset=238 */

	/* Parameter pchEaten */

/* 1832 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 1834 */	NdrFcShort( 0x20 ),	/* ia64 Stack size/offset = 32 */
/* 1836 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter ppmkOut */

/* 1838 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
/* 1840 */	NdrFcShort( 0x28 ),	/* ia64 Stack size/offset = 40 */
/* 1842 */	NdrFcShort( 0x1c0 ),	/* Type Offset=448 */

	/* Return value */

/* 1844 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1846 */	NdrFcShort( 0x30 ),	/* ia64 Stack size/offset = 48 */
/* 1848 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure IsSystemMoniker */

/* 1850 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 1852 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1856 */	NdrFcShort( 0x16 ),	/* 22 */
/* 1858 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 1860 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1862 */	NdrFcShort( 0x24 ),	/* 36 */
/* 1864 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x2,		/* 2 */
/* 1866 */	0xa,		/* 10 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 1868 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1870 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1872 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1874 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pdwMksys */

/* 1876 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 1878 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 1880 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 1882 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1884 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 1886 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetComparisonData */

/* 1888 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 1890 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1894 */	NdrFcShort( 0x3 ),	/* 3 */
/* 1896 */	NdrFcShort( 0x28 ),	/* ia64 Stack size/offset = 40 */
/* 1898 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1900 */	NdrFcShort( 0x24 ),	/* 36 */
/* 1902 */	0x45,		/* Oi2 Flags:  srv must size, has return, has ext, */
			0x4,		/* 4 */
/* 1904 */	0xa,		/* 10 */
			0x3,		/* Ext Flags:  new corr desc, clt corr check, */
/* 1906 */	NdrFcShort( 0x1 ),	/* 1 */
/* 1908 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1910 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1912 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pbData */

/* 1914 */	NdrFcShort( 0x113 ),	/* Flags:  must size, must free, out, simple ref, */
/* 1916 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 1918 */	NdrFcShort( 0x1d0 ),	/* Type Offset=464 */

	/* Parameter cbMax */

/* 1920 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 1922 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 1924 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter pcbData */

/* 1926 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 1928 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 1930 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 1932 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1934 */	NdrFcShort( 0x20 ),	/* ia64 Stack size/offset = 32 */
/* 1936 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure RemoteNext */

/* 1938 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 1940 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1944 */	NdrFcShort( 0x3 ),	/* 3 */
/* 1946 */	NdrFcShort( 0x28 ),	/* ia64 Stack size/offset = 40 */
/* 1948 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1950 */	NdrFcShort( 0x24 ),	/* 36 */
/* 1952 */	0x45,		/* Oi2 Flags:  srv must size, has return, has ext, */
			0x4,		/* 4 */
/* 1954 */	0xa,		/* 10 */
			0x3,		/* Ext Flags:  new corr desc, clt corr check, */
/* 1956 */	NdrFcShort( 0x1 ),	/* 1 */
/* 1958 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1960 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1962 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter celt */

/* 1964 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 1966 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 1968 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter rgelt */

/* 1970 */	NdrFcShort( 0x113 ),	/* Flags:  must size, must free, out, simple ref, */
/* 1972 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 1974 */	NdrFcShort( 0x1e0 ),	/* Type Offset=480 */

	/* Parameter pceltFetched */

/* 1976 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 1978 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 1980 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 1982 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1984 */	NdrFcShort( 0x20 ),	/* ia64 Stack size/offset = 32 */
/* 1986 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure Clone */

/* 1988 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 1990 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1994 */	NdrFcShort( 0x6 ),	/* 6 */
/* 1996 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 1998 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2000 */	NdrFcShort( 0x8 ),	/* 8 */
/* 2002 */	0x45,		/* Oi2 Flags:  srv must size, has return, has ext, */
			0x2,		/* 2 */
/* 2004 */	0xa,		/* 10 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 2006 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2008 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2010 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2012 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter ppenum */

/* 2014 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
/* 2016 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 2018 */	NdrFcShort( 0xf4 ),	/* Type Offset=244 */

	/* Return value */

/* 2020 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 2022 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 2024 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure Pull */


	/* Procedure RemoteRead */

/* 2026 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 2028 */	NdrFcLong( 0x0 ),	/* 0 */
/* 2032 */	NdrFcShort( 0x3 ),	/* 3 */
/* 2034 */	NdrFcShort( 0x28 ),	/* ia64 Stack size/offset = 40 */
/* 2036 */	NdrFcShort( 0x8 ),	/* 8 */
/* 2038 */	NdrFcShort( 0x24 ),	/* 36 */
/* 2040 */	0x45,		/* Oi2 Flags:  srv must size, has return, has ext, */
			0x4,		/* 4 */
/* 2042 */	0xa,		/* 10 */
			0x3,		/* Ext Flags:  new corr desc, clt corr check, */
/* 2044 */	NdrFcShort( 0x1 ),	/* 1 */
/* 2046 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2048 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2050 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter buf */


	/* Parameter pv */

/* 2052 */	NdrFcShort( 0x113 ),	/* Flags:  must size, must free, out, simple ref, */
/* 2054 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 2056 */	NdrFcShort( 0x1fa ),	/* Type Offset=506 */

	/* Parameter cRequest */


	/* Parameter cb */

/* 2058 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 2060 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 2062 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter pcReturned */


	/* Parameter pcbRead */

/* 2064 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 2066 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 2068 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */


	/* Return value */

/* 2070 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 2072 */	NdrFcShort( 0x20 ),	/* ia64 Stack size/offset = 32 */
/* 2074 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure RemoteWrite */

/* 2076 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 2078 */	NdrFcLong( 0x0 ),	/* 0 */
/* 2082 */	NdrFcShort( 0x4 ),	/* 4 */
/* 2084 */	NdrFcShort( 0x28 ),	/* ia64 Stack size/offset = 40 */
/* 2086 */	NdrFcShort( 0x8 ),	/* 8 */
/* 2088 */	NdrFcShort( 0x24 ),	/* 36 */
/* 2090 */	0x46,		/* Oi2 Flags:  clt must size, has return, has ext, */
			0x4,		/* 4 */
/* 2092 */	0xa,		/* 10 */
			0x5,		/* Ext Flags:  new corr desc, srv corr check, */
/* 2094 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2096 */	NdrFcShort( 0x1 ),	/* 1 */
/* 2098 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2100 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pv */

/* 2102 */	NdrFcShort( 0x10b ),	/* Flags:  must size, must free, in, simple ref, */
/* 2104 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 2106 */	NdrFcShort( 0x1d0 ),	/* Type Offset=464 */

	/* Parameter cb */

/* 2108 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 2110 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 2112 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter pcbWritten */

/* 2114 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 2116 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 2118 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 2120 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 2122 */	NdrFcShort( 0x20 ),	/* ia64 Stack size/offset = 32 */
/* 2124 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure RemoteSeek */

/* 2126 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 2128 */	NdrFcLong( 0x0 ),	/* 0 */
/* 2132 */	NdrFcShort( 0x5 ),	/* 5 */
/* 2134 */	NdrFcShort( 0x28 ),	/* ia64 Stack size/offset = 40 */
/* 2136 */	NdrFcShort( 0x20 ),	/* 32 */
/* 2138 */	NdrFcShort( 0x34 ),	/* 52 */
/* 2140 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x4,		/* 4 */
/* 2142 */	0xa,		/* 10 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 2144 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2146 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2148 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2150 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter dlibMove */

/* 2152 */	NdrFcShort( 0x8a ),	/* Flags:  must free, in, by val, */
/* 2154 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 2156 */	NdrFcShort( 0x19a ),	/* Type Offset=410 */

	/* Parameter dwOrigin */

/* 2158 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 2160 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 2162 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter plibNewPosition */

/* 2164 */	NdrFcShort( 0x2112 ),	/* Flags:  must free, out, simple ref, srv alloc size=8 */
/* 2166 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 2168 */	NdrFcShort( 0x19a ),	/* Type Offset=410 */

	/* Return value */

/* 2170 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 2172 */	NdrFcShort( 0x20 ),	/* ia64 Stack size/offset = 32 */
/* 2174 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure SetSize */


	/* Procedure SetSize */

/* 2176 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 2178 */	NdrFcLong( 0x0 ),	/* 0 */
/* 2182 */	NdrFcShort( 0x6 ),	/* 6 */
/* 2184 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 2186 */	NdrFcShort( 0x18 ),	/* 24 */
/* 2188 */	NdrFcShort( 0x8 ),	/* 8 */
/* 2190 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x2,		/* 2 */
/* 2192 */	0xa,		/* 10 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 2194 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2196 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2198 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2200 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter cb */


	/* Parameter libNewSize */

/* 2202 */	NdrFcShort( 0x8a ),	/* Flags:  must free, in, by val, */
/* 2204 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 2206 */	NdrFcShort( 0x19a ),	/* Type Offset=410 */

	/* Return value */


	/* Return value */

/* 2208 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 2210 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 2212 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure RemoteCopyTo */

/* 2214 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 2216 */	NdrFcLong( 0x0 ),	/* 0 */
/* 2220 */	NdrFcShort( 0x7 ),	/* 7 */
/* 2222 */	NdrFcShort( 0x30 ),	/* ia64 Stack size/offset = 48 */
/* 2224 */	NdrFcShort( 0x18 ),	/* 24 */
/* 2226 */	NdrFcShort( 0x60 ),	/* 96 */
/* 2228 */	0x46,		/* Oi2 Flags:  clt must size, has return, has ext, */
			0x5,		/* 5 */
/* 2230 */	0xa,		/* 10 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 2232 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2234 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2236 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2238 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pstm */

/* 2240 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 2242 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 2244 */	NdrFcShort( 0x184 ),	/* Type Offset=388 */

	/* Parameter cb */

/* 2246 */	NdrFcShort( 0x8a ),	/* Flags:  must free, in, by val, */
/* 2248 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 2250 */	NdrFcShort( 0x19a ),	/* Type Offset=410 */

	/* Parameter pcbRead */

/* 2252 */	NdrFcShort( 0x2112 ),	/* Flags:  must free, out, simple ref, srv alloc size=8 */
/* 2254 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 2256 */	NdrFcShort( 0x19a ),	/* Type Offset=410 */

	/* Parameter pcbWritten */

/* 2258 */	NdrFcShort( 0x2112 ),	/* Flags:  must free, out, simple ref, srv alloc size=8 */
/* 2260 */	NdrFcShort( 0x20 ),	/* ia64 Stack size/offset = 32 */
/* 2262 */	NdrFcShort( 0x19a ),	/* Type Offset=410 */

	/* Return value */

/* 2264 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 2266 */	NdrFcShort( 0x28 ),	/* ia64 Stack size/offset = 40 */
/* 2268 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure Commit */

/* 2270 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 2272 */	NdrFcLong( 0x0 ),	/* 0 */
/* 2276 */	NdrFcShort( 0x8 ),	/* 8 */
/* 2278 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 2280 */	NdrFcShort( 0x8 ),	/* 8 */
/* 2282 */	NdrFcShort( 0x8 ),	/* 8 */
/* 2284 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x2,		/* 2 */
/* 2286 */	0xa,		/* 10 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 2288 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2290 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2292 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2294 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter grfCommitFlags */

/* 2296 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 2298 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 2300 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 2302 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 2304 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 2306 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure HandsOffStorage */


	/* Procedure Revert */

/* 2308 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 2310 */	NdrFcLong( 0x0 ),	/* 0 */
/* 2314 */	NdrFcShort( 0x9 ),	/* 9 */
/* 2316 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 2318 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2320 */	NdrFcShort( 0x8 ),	/* 8 */
/* 2322 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x1,		/* 1 */
/* 2324 */	0xa,		/* 10 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 2326 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2328 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2330 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2332 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Return value */


	/* Return value */

/* 2334 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 2336 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 2338 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure LockRegion */

/* 2340 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 2342 */	NdrFcLong( 0x0 ),	/* 0 */
/* 2346 */	NdrFcShort( 0xa ),	/* 10 */
/* 2348 */	NdrFcShort( 0x28 ),	/* ia64 Stack size/offset = 40 */
/* 2350 */	NdrFcShort( 0x38 ),	/* 56 */
/* 2352 */	NdrFcShort( 0x8 ),	/* 8 */
/* 2354 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x4,		/* 4 */
/* 2356 */	0xa,		/* 10 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 2358 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2360 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2362 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2364 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter libOffset */

/* 2366 */	NdrFcShort( 0x8a ),	/* Flags:  must free, in, by val, */
/* 2368 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 2370 */	NdrFcShort( 0x19a ),	/* Type Offset=410 */

	/* Parameter cb */

/* 2372 */	NdrFcShort( 0x8a ),	/* Flags:  must free, in, by val, */
/* 2374 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 2376 */	NdrFcShort( 0x19a ),	/* Type Offset=410 */

	/* Parameter dwLockType */

/* 2378 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 2380 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 2382 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 2384 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 2386 */	NdrFcShort( 0x20 ),	/* ia64 Stack size/offset = 32 */
/* 2388 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure UnlockRegion */

/* 2390 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 2392 */	NdrFcLong( 0x0 ),	/* 0 */
/* 2396 */	NdrFcShort( 0xb ),	/* 11 */
/* 2398 */	NdrFcShort( 0x28 ),	/* ia64 Stack size/offset = 40 */
/* 2400 */	NdrFcShort( 0x38 ),	/* 56 */
/* 2402 */	NdrFcShort( 0x8 ),	/* 8 */
/* 2404 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x4,		/* 4 */
/* 2406 */	0xa,		/* 10 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 2408 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2410 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2412 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2414 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter libOffset */

/* 2416 */	NdrFcShort( 0x8a ),	/* Flags:  must free, in, by val, */
/* 2418 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 2420 */	NdrFcShort( 0x19a ),	/* Type Offset=410 */

	/* Parameter cb */

/* 2422 */	NdrFcShort( 0x8a ),	/* Flags:  must free, in, by val, */
/* 2424 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 2426 */	NdrFcShort( 0x19a ),	/* Type Offset=410 */

	/* Parameter dwLockType */

/* 2428 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 2430 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 2432 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 2434 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 2436 */	NdrFcShort( 0x20 ),	/* ia64 Stack size/offset = 32 */
/* 2438 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure Stat */

/* 2440 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 2442 */	NdrFcLong( 0x0 ),	/* 0 */
/* 2446 */	NdrFcShort( 0xc ),	/* 12 */
/* 2448 */	NdrFcShort( 0x20 ),	/* ia64 Stack size/offset = 32 */
/* 2450 */	NdrFcShort( 0x8 ),	/* 8 */
/* 2452 */	NdrFcShort( 0x8 ),	/* 8 */
/* 2454 */	0x45,		/* Oi2 Flags:  srv must size, has return, has ext, */
			0x3,		/* 3 */
/* 2456 */	0xa,		/* 10 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 2458 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2460 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2462 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2464 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pstatstg */

/* 2466 */	NdrFcShort( 0x113 ),	/* Flags:  must size, must free, out, simple ref, */
/* 2468 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 2470 */	NdrFcShort( 0x210 ),	/* Type Offset=528 */

	/* Parameter grfStatFlag */

/* 2472 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 2474 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 2476 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 2478 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 2480 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 2482 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure Clone */

/* 2484 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 2486 */	NdrFcLong( 0x0 ),	/* 0 */
/* 2490 */	NdrFcShort( 0xd ),	/* 13 */
/* 2492 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 2494 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2496 */	NdrFcShort( 0x8 ),	/* 8 */
/* 2498 */	0x45,		/* Oi2 Flags:  srv must size, has return, has ext, */
			0x2,		/* 2 */
/* 2500 */	0xa,		/* 10 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 2502 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2504 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2506 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2508 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter ppstm */

/* 2510 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
/* 2512 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 2514 */	NdrFcShort( 0x238 ),	/* Type Offset=568 */

	/* Return value */

/* 2516 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 2518 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 2520 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure RemoteNext */

/* 2522 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 2524 */	NdrFcLong( 0x0 ),	/* 0 */
/* 2528 */	NdrFcShort( 0x3 ),	/* 3 */
/* 2530 */	NdrFcShort( 0x28 ),	/* ia64 Stack size/offset = 40 */
/* 2532 */	NdrFcShort( 0x8 ),	/* 8 */
/* 2534 */	NdrFcShort( 0x24 ),	/* 36 */
/* 2536 */	0x45,		/* Oi2 Flags:  srv must size, has return, has ext, */
			0x4,		/* 4 */
/* 2538 */	0xa,		/* 10 */
			0x3,		/* Ext Flags:  new corr desc, clt corr check, */
/* 2540 */	NdrFcShort( 0x1 ),	/* 1 */
/* 2542 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2544 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2546 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter celt */

/* 2548 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 2550 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 2552 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter rgelt */

/* 2554 */	NdrFcShort( 0x113 ),	/* Flags:  must size, must free, out, simple ref, */
/* 2556 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 2558 */	NdrFcShort( 0x240 ),	/* Type Offset=576 */

	/* Parameter pceltFetched */

/* 2560 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 2562 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 2564 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 2566 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 2568 */	NdrFcShort( 0x20 ),	/* ia64 Stack size/offset = 32 */
/* 2570 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure Clone */

/* 2572 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 2574 */	NdrFcLong( 0x0 ),	/* 0 */
/* 2578 */	NdrFcShort( 0x6 ),	/* 6 */
/* 2580 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 2582 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2584 */	NdrFcShort( 0x8 ),	/* 8 */
/* 2586 */	0x45,		/* Oi2 Flags:  srv must size, has return, has ext, */
			0x2,		/* 2 */
/* 2588 */	0xa,		/* 10 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 2590 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2592 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2594 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2596 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter ppenum */

/* 2598 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
/* 2600 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 2602 */	NdrFcShort( 0x256 ),	/* Type Offset=598 */

	/* Return value */

/* 2604 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 2606 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 2608 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure CreateStream */

/* 2610 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 2612 */	NdrFcLong( 0x0 ),	/* 0 */
/* 2616 */	NdrFcShort( 0x3 ),	/* 3 */
/* 2618 */	NdrFcShort( 0x38 ),	/* ia64 Stack size/offset = 56 */
/* 2620 */	NdrFcShort( 0x18 ),	/* 24 */
/* 2622 */	NdrFcShort( 0x8 ),	/* 8 */
/* 2624 */	0x47,		/* Oi2 Flags:  srv must size, clt must size, has return, has ext, */
			0x6,		/* 6 */
/* 2626 */	0xa,		/* 10 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 2628 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2630 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2632 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2634 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pwcsName */

/* 2636 */	NdrFcShort( 0x10b ),	/* Flags:  must size, must free, in, simple ref, */
/* 2638 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 2640 */	NdrFcShort( 0xee ),	/* Type Offset=238 */

	/* Parameter grfMode */

/* 2642 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 2644 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 2646 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter reserved1 */

/* 2648 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 2650 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 2652 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter reserved2 */

/* 2654 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 2656 */	NdrFcShort( 0x20 ),	/* ia64 Stack size/offset = 32 */
/* 2658 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter ppstm */

/* 2660 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
/* 2662 */	NdrFcShort( 0x28 ),	/* ia64 Stack size/offset = 40 */
/* 2664 */	NdrFcShort( 0x238 ),	/* Type Offset=568 */

	/* Return value */

/* 2666 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 2668 */	NdrFcShort( 0x30 ),	/* ia64 Stack size/offset = 48 */
/* 2670 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure RemoteOpenStream */

/* 2672 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 2674 */	NdrFcLong( 0x0 ),	/* 0 */
/* 2678 */	NdrFcShort( 0x4 ),	/* 4 */
/* 2680 */	NdrFcShort( 0x40 ),	/* ia64 Stack size/offset = 64 */
/* 2682 */	NdrFcShort( 0x18 ),	/* 24 */
/* 2684 */	NdrFcShort( 0x8 ),	/* 8 */
/* 2686 */	0x47,		/* Oi2 Flags:  srv must size, clt must size, has return, has ext, */
			0x7,		/* 7 */
/* 2688 */	0xa,		/* 10 */
			0x5,		/* Ext Flags:  new corr desc, srv corr check, */
/* 2690 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2692 */	NdrFcShort( 0x1 ),	/* 1 */
/* 2694 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2696 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pwcsName */

/* 2698 */	NdrFcShort( 0x10b ),	/* Flags:  must size, must free, in, simple ref, */
/* 2700 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 2702 */	NdrFcShort( 0xee ),	/* Type Offset=238 */

	/* Parameter cbReserved1 */

/* 2704 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 2706 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 2708 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter reserved1 */

/* 2710 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 2712 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 2714 */	NdrFcShort( 0x26c ),	/* Type Offset=620 */

	/* Parameter grfMode */

/* 2716 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 2718 */	NdrFcShort( 0x20 ),	/* ia64 Stack size/offset = 32 */
/* 2720 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter reserved2 */

/* 2722 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 2724 */	NdrFcShort( 0x28 ),	/* ia64 Stack size/offset = 40 */
/* 2726 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter ppstm */

/* 2728 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
/* 2730 */	NdrFcShort( 0x30 ),	/* ia64 Stack size/offset = 48 */
/* 2732 */	NdrFcShort( 0x238 ),	/* Type Offset=568 */

	/* Return value */

/* 2734 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 2736 */	NdrFcShort( 0x38 ),	/* ia64 Stack size/offset = 56 */
/* 2738 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure CreateStorage */

/* 2740 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 2742 */	NdrFcLong( 0x0 ),	/* 0 */
/* 2746 */	NdrFcShort( 0x5 ),	/* 5 */
/* 2748 */	NdrFcShort( 0x38 ),	/* ia64 Stack size/offset = 56 */
/* 2750 */	NdrFcShort( 0x18 ),	/* 24 */
/* 2752 */	NdrFcShort( 0x8 ),	/* 8 */
/* 2754 */	0x47,		/* Oi2 Flags:  srv must size, clt must size, has return, has ext, */
			0x6,		/* 6 */
/* 2756 */	0xa,		/* 10 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 2758 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2760 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2762 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2764 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pwcsName */

/* 2766 */	NdrFcShort( 0x10b ),	/* Flags:  must size, must free, in, simple ref, */
/* 2768 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 2770 */	NdrFcShort( 0xee ),	/* Type Offset=238 */

	/* Parameter grfMode */

/* 2772 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 2774 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 2776 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter reserved1 */

/* 2778 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 2780 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 2782 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter reserved2 */

/* 2784 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 2786 */	NdrFcShort( 0x20 ),	/* ia64 Stack size/offset = 32 */
/* 2788 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter ppstg */

/* 2790 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
/* 2792 */	NdrFcShort( 0x28 ),	/* ia64 Stack size/offset = 40 */
/* 2794 */	NdrFcShort( 0x27c ),	/* Type Offset=636 */

	/* Return value */

/* 2796 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 2798 */	NdrFcShort( 0x30 ),	/* ia64 Stack size/offset = 48 */
/* 2800 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure OpenStorage */

/* 2802 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 2804 */	NdrFcLong( 0x0 ),	/* 0 */
/* 2808 */	NdrFcShort( 0x6 ),	/* 6 */
/* 2810 */	NdrFcShort( 0x40 ),	/* ia64 Stack size/offset = 64 */
/* 2812 */	NdrFcShort( 0x10 ),	/* 16 */
/* 2814 */	NdrFcShort( 0x8 ),	/* 8 */
/* 2816 */	0x47,		/* Oi2 Flags:  srv must size, clt must size, has return, has ext, */
			0x7,		/* 7 */
/* 2818 */	0xa,		/* 10 */
			0x5,		/* Ext Flags:  new corr desc, srv corr check, */
/* 2820 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2822 */	NdrFcShort( 0x1 ),	/* 1 */
/* 2824 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2826 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pwcsName */

/* 2828 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 2830 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 2832 */	NdrFcShort( 0x292 ),	/* Type Offset=658 */

	/* Parameter pstgPriority */

/* 2834 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 2836 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 2838 */	NdrFcShort( 0x280 ),	/* Type Offset=640 */

	/* Parameter grfMode */

/* 2840 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 2842 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 2844 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter snbExclude */

/* 2846 */	NdrFcShort( 0x8b ),	/* Flags:  must size, must free, in, by val, */
/* 2848 */	NdrFcShort( 0x20 ),	/* ia64 Stack size/offset = 32 */
/* 2850 */	NdrFcShort( 0x2b0 ),	/* Type Offset=688 */

	/* Parameter reserved */

/* 2852 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 2854 */	NdrFcShort( 0x28 ),	/* ia64 Stack size/offset = 40 */
/* 2856 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter ppstg */

/* 2858 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
/* 2860 */	NdrFcShort( 0x30 ),	/* ia64 Stack size/offset = 48 */
/* 2862 */	NdrFcShort( 0x27c ),	/* Type Offset=636 */

	/* Return value */

/* 2864 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 2866 */	NdrFcShort( 0x38 ),	/* ia64 Stack size/offset = 56 */
/* 2868 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure CopyTo */

/* 2870 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 2872 */	NdrFcLong( 0x0 ),	/* 0 */
/* 2876 */	NdrFcShort( 0x7 ),	/* 7 */
/* 2878 */	NdrFcShort( 0x30 ),	/* ia64 Stack size/offset = 48 */
/* 2880 */	NdrFcShort( 0x8 ),	/* 8 */
/* 2882 */	NdrFcShort( 0x8 ),	/* 8 */
/* 2884 */	0x46,		/* Oi2 Flags:  clt must size, has return, has ext, */
			0x5,		/* 5 */
/* 2886 */	0xa,		/* 10 */
			0x5,		/* Ext Flags:  new corr desc, srv corr check, */
/* 2888 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2890 */	NdrFcShort( 0x2 ),	/* 2 */
/* 2892 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2894 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter ciidExclude */

/* 2896 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 2898 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 2900 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter rgiidExclude */

/* 2902 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 2904 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 2906 */	NdrFcShort( 0x2ba ),	/* Type Offset=698 */

	/* Parameter snbExclude */

/* 2908 */	NdrFcShort( 0x8b ),	/* Flags:  must size, must free, in, by val, */
/* 2910 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 2912 */	NdrFcShort( 0x2b0 ),	/* Type Offset=688 */

	/* Parameter pstgDest */

/* 2914 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 2916 */	NdrFcShort( 0x20 ),	/* ia64 Stack size/offset = 32 */
/* 2918 */	NdrFcShort( 0x280 ),	/* Type Offset=640 */

	/* Return value */

/* 2920 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 2922 */	NdrFcShort( 0x28 ),	/* ia64 Stack size/offset = 40 */
/* 2924 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure MoveElementTo */

/* 2926 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 2928 */	NdrFcLong( 0x0 ),	/* 0 */
/* 2932 */	NdrFcShort( 0x8 ),	/* 8 */
/* 2934 */	NdrFcShort( 0x30 ),	/* ia64 Stack size/offset = 48 */
/* 2936 */	NdrFcShort( 0x8 ),	/* 8 */
/* 2938 */	NdrFcShort( 0x8 ),	/* 8 */
/* 2940 */	0x46,		/* Oi2 Flags:  clt must size, has return, has ext, */
			0x5,		/* 5 */
/* 2942 */	0xa,		/* 10 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 2944 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2946 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2948 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2950 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pwcsName */

/* 2952 */	NdrFcShort( 0x10b ),	/* Flags:  must size, must free, in, simple ref, */
/* 2954 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 2956 */	NdrFcShort( 0xee ),	/* Type Offset=238 */

	/* Parameter pstgDest */

/* 2958 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 2960 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 2962 */	NdrFcShort( 0x280 ),	/* Type Offset=640 */

	/* Parameter pwcsNewName */

/* 2964 */	NdrFcShort( 0x10b ),	/* Flags:  must size, must free, in, simple ref, */
/* 2966 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 2968 */	NdrFcShort( 0xee ),	/* Type Offset=238 */

	/* Parameter grfFlags */

/* 2970 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 2972 */	NdrFcShort( 0x20 ),	/* ia64 Stack size/offset = 32 */
/* 2974 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 2976 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 2978 */	NdrFcShort( 0x28 ),	/* ia64 Stack size/offset = 40 */
/* 2980 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure Commit */

/* 2982 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 2984 */	NdrFcLong( 0x0 ),	/* 0 */
/* 2988 */	NdrFcShort( 0x9 ),	/* 9 */
/* 2990 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 2992 */	NdrFcShort( 0x8 ),	/* 8 */
/* 2994 */	NdrFcShort( 0x8 ),	/* 8 */
/* 2996 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x2,		/* 2 */
/* 2998 */	0xa,		/* 10 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 3000 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3002 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3004 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3006 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter grfCommitFlags */

/* 3008 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 3010 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 3012 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 3014 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 3016 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 3018 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure Revert */

/* 3020 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 3022 */	NdrFcLong( 0x0 ),	/* 0 */
/* 3026 */	NdrFcShort( 0xa ),	/* 10 */
/* 3028 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 3030 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3032 */	NdrFcShort( 0x8 ),	/* 8 */
/* 3034 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x1,		/* 1 */
/* 3036 */	0xa,		/* 10 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 3038 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3040 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3042 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3044 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Return value */

/* 3046 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 3048 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 3050 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure RemoteEnumElements */

/* 3052 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 3054 */	NdrFcLong( 0x0 ),	/* 0 */
/* 3058 */	NdrFcShort( 0xb ),	/* 11 */
/* 3060 */	NdrFcShort( 0x38 ),	/* ia64 Stack size/offset = 56 */
/* 3062 */	NdrFcShort( 0x18 ),	/* 24 */
/* 3064 */	NdrFcShort( 0x8 ),	/* 8 */
/* 3066 */	0x47,		/* Oi2 Flags:  srv must size, clt must size, has return, has ext, */
			0x6,		/* 6 */
/* 3068 */	0xa,		/* 10 */
			0x5,		/* Ext Flags:  new corr desc, srv corr check, */
/* 3070 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3072 */	NdrFcShort( 0x1 ),	/* 1 */
/* 3074 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3076 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter reserved1 */

/* 3078 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 3080 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 3082 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter cbReserved2 */

/* 3084 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 3086 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 3088 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter reserved2 */

/* 3090 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 3092 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 3094 */	NdrFcShort( 0x26c ),	/* Type Offset=620 */

	/* Parameter reserved3 */

/* 3096 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 3098 */	NdrFcShort( 0x20 ),	/* ia64 Stack size/offset = 32 */
/* 3100 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter ppenum */

/* 3102 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
/* 3104 */	NdrFcShort( 0x28 ),	/* ia64 Stack size/offset = 40 */
/* 3106 */	NdrFcShort( 0x256 ),	/* Type Offset=598 */

	/* Return value */

/* 3108 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 3110 */	NdrFcShort( 0x30 ),	/* ia64 Stack size/offset = 48 */
/* 3112 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure RenameElement */

/* 3114 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 3116 */	NdrFcLong( 0x0 ),	/* 0 */
/* 3120 */	NdrFcShort( 0xd ),	/* 13 */
/* 3122 */	NdrFcShort( 0x20 ),	/* ia64 Stack size/offset = 32 */
/* 3124 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3126 */	NdrFcShort( 0x8 ),	/* 8 */
/* 3128 */	0x46,		/* Oi2 Flags:  clt must size, has return, has ext, */
			0x3,		/* 3 */
/* 3130 */	0xa,		/* 10 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 3132 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3134 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3136 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3138 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pwcsOldName */

/* 3140 */	NdrFcShort( 0x10b ),	/* Flags:  must size, must free, in, simple ref, */
/* 3142 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 3144 */	NdrFcShort( 0xee ),	/* Type Offset=238 */

	/* Parameter pwcsNewName */

/* 3146 */	NdrFcShort( 0x10b ),	/* Flags:  must size, must free, in, simple ref, */
/* 3148 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 3150 */	NdrFcShort( 0xee ),	/* Type Offset=238 */

	/* Return value */

/* 3152 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 3154 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 3156 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure SetElementTimes */

/* 3158 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 3160 */	NdrFcLong( 0x0 ),	/* 0 */
/* 3164 */	NdrFcShort( 0xe ),	/* 14 */
/* 3166 */	NdrFcShort( 0x30 ),	/* ia64 Stack size/offset = 48 */
/* 3168 */	NdrFcShort( 0x84 ),	/* 132 */
/* 3170 */	NdrFcShort( 0x8 ),	/* 8 */
/* 3172 */	0x46,		/* Oi2 Flags:  clt must size, has return, has ext, */
			0x5,		/* 5 */
/* 3174 */	0xa,		/* 10 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 3176 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3178 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3180 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3182 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pwcsName */

/* 3184 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 3186 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 3188 */	NdrFcShort( 0x292 ),	/* Type Offset=658 */

	/* Parameter pctime */

/* 3190 */	NdrFcShort( 0xa ),	/* Flags:  must free, in, */
/* 3192 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 3194 */	NdrFcShort( 0x2ce ),	/* Type Offset=718 */

	/* Parameter patime */

/* 3196 */	NdrFcShort( 0xa ),	/* Flags:  must free, in, */
/* 3198 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 3200 */	NdrFcShort( 0x2ce ),	/* Type Offset=718 */

	/* Parameter pmtime */

/* 3202 */	NdrFcShort( 0xa ),	/* Flags:  must free, in, */
/* 3204 */	NdrFcShort( 0x20 ),	/* ia64 Stack size/offset = 32 */
/* 3206 */	NdrFcShort( 0x2ce ),	/* Type Offset=718 */

	/* Return value */

/* 3208 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 3210 */	NdrFcShort( 0x28 ),	/* ia64 Stack size/offset = 40 */
/* 3212 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure SetClass */

/* 3214 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 3216 */	NdrFcLong( 0x0 ),	/* 0 */
/* 3220 */	NdrFcShort( 0xf ),	/* 15 */
/* 3222 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 3224 */	NdrFcShort( 0x44 ),	/* 68 */
/* 3226 */	NdrFcShort( 0x8 ),	/* 8 */
/* 3228 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x2,		/* 2 */
/* 3230 */	0xa,		/* 10 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 3232 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3234 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3236 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3238 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter clsid */

/* 3240 */	NdrFcShort( 0x10a ),	/* Flags:  must free, in, simple ref, */
/* 3242 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 3244 */	NdrFcShort( 0x156 ),	/* Type Offset=342 */

	/* Return value */

/* 3246 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 3248 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 3250 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure SetStateBits */

/* 3252 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 3254 */	NdrFcLong( 0x0 ),	/* 0 */
/* 3258 */	NdrFcShort( 0x10 ),	/* 16 */
/* 3260 */	NdrFcShort( 0x20 ),	/* ia64 Stack size/offset = 32 */
/* 3262 */	NdrFcShort( 0x10 ),	/* 16 */
/* 3264 */	NdrFcShort( 0x8 ),	/* 8 */
/* 3266 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x3,		/* 3 */
/* 3268 */	0xa,		/* 10 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 3270 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3272 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3274 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3276 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter grfStateBits */

/* 3278 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 3280 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 3282 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter grfMask */

/* 3284 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 3286 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 3288 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 3290 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 3292 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 3294 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure Stat */

/* 3296 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 3298 */	NdrFcLong( 0x0 ),	/* 0 */
/* 3302 */	NdrFcShort( 0x11 ),	/* 17 */
/* 3304 */	NdrFcShort( 0x20 ),	/* ia64 Stack size/offset = 32 */
/* 3306 */	NdrFcShort( 0x8 ),	/* 8 */
/* 3308 */	NdrFcShort( 0x8 ),	/* 8 */
/* 3310 */	0x45,		/* Oi2 Flags:  srv must size, has return, has ext, */
			0x3,		/* 3 */
/* 3312 */	0xa,		/* 10 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 3314 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3316 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3318 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3320 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pstatstg */

/* 3322 */	NdrFcShort( 0x113 ),	/* Flags:  must size, must free, out, simple ref, */
/* 3324 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 3326 */	NdrFcShort( 0x210 ),	/* Type Offset=528 */

	/* Parameter grfStatFlag */

/* 3328 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 3330 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 3332 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 3334 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 3336 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 3338 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure Load */

/* 3340 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 3342 */	NdrFcLong( 0x0 ),	/* 0 */
/* 3346 */	NdrFcShort( 0x5 ),	/* 5 */
/* 3348 */	NdrFcShort( 0x20 ),	/* ia64 Stack size/offset = 32 */
/* 3350 */	NdrFcShort( 0x8 ),	/* 8 */
/* 3352 */	NdrFcShort( 0x8 ),	/* 8 */
/* 3354 */	0x46,		/* Oi2 Flags:  clt must size, has return, has ext, */
			0x3,		/* 3 */
/* 3356 */	0xa,		/* 10 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 3358 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3360 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3362 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3364 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pszFileName */

/* 3366 */	NdrFcShort( 0x10b ),	/* Flags:  must size, must free, in, simple ref, */
/* 3368 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 3370 */	NdrFcShort( 0xee ),	/* Type Offset=238 */

	/* Parameter dwMode */

/* 3372 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 3374 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 3376 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 3378 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 3380 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 3382 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure Save */

/* 3384 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 3386 */	NdrFcLong( 0x0 ),	/* 0 */
/* 3390 */	NdrFcShort( 0x6 ),	/* 6 */
/* 3392 */	NdrFcShort( 0x20 ),	/* ia64 Stack size/offset = 32 */
/* 3394 */	NdrFcShort( 0x8 ),	/* 8 */
/* 3396 */	NdrFcShort( 0x8 ),	/* 8 */
/* 3398 */	0x46,		/* Oi2 Flags:  clt must size, has return, has ext, */
			0x3,		/* 3 */
/* 3400 */	0xa,		/* 10 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 3402 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3404 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3406 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3408 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pszFileName */

/* 3410 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 3412 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 3414 */	NdrFcShort( 0x292 ),	/* Type Offset=658 */

	/* Parameter fRemember */

/* 3416 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 3418 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 3420 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 3422 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 3424 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 3426 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure SaveCompleted */

/* 3428 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 3430 */	NdrFcLong( 0x0 ),	/* 0 */
/* 3434 */	NdrFcShort( 0x7 ),	/* 7 */
/* 3436 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 3438 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3440 */	NdrFcShort( 0x8 ),	/* 8 */
/* 3442 */	0x46,		/* Oi2 Flags:  clt must size, has return, has ext, */
			0x2,		/* 2 */
/* 3444 */	0xa,		/* 10 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 3446 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3448 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3450 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3452 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pszFileName */

/* 3454 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 3456 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 3458 */	NdrFcShort( 0x292 ),	/* Type Offset=658 */

	/* Return value */

/* 3460 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 3462 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 3464 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetCurFile */

/* 3466 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 3468 */	NdrFcLong( 0x0 ),	/* 0 */
/* 3472 */	NdrFcShort( 0x8 ),	/* 8 */
/* 3474 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 3476 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3478 */	NdrFcShort( 0x8 ),	/* 8 */
/* 3480 */	0x45,		/* Oi2 Flags:  srv must size, has return, has ext, */
			0x2,		/* 2 */
/* 3482 */	0xa,		/* 10 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 3484 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3486 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3488 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3490 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter ppszFileName */

/* 3492 */	NdrFcShort( 0x2013 ),	/* Flags:  must size, must free, out, srv alloc size=8 */
/* 3494 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 3496 */	NdrFcShort( 0x1c4 ),	/* Type Offset=452 */

	/* Return value */

/* 3498 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 3500 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 3502 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure InitNew */

/* 3504 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 3506 */	NdrFcLong( 0x0 ),	/* 0 */
/* 3510 */	NdrFcShort( 0x5 ),	/* 5 */
/* 3512 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 3514 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3516 */	NdrFcShort( 0x8 ),	/* 8 */
/* 3518 */	0x46,		/* Oi2 Flags:  clt must size, has return, has ext, */
			0x2,		/* 2 */
/* 3520 */	0xa,		/* 10 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 3522 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3524 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3526 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3528 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pStg */

/* 3530 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 3532 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 3534 */	NdrFcShort( 0x280 ),	/* Type Offset=640 */

	/* Return value */

/* 3536 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 3538 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 3540 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure Load */

/* 3542 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 3544 */	NdrFcLong( 0x0 ),	/* 0 */
/* 3548 */	NdrFcShort( 0x6 ),	/* 6 */
/* 3550 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 3552 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3554 */	NdrFcShort( 0x8 ),	/* 8 */
/* 3556 */	0x46,		/* Oi2 Flags:  clt must size, has return, has ext, */
			0x2,		/* 2 */
/* 3558 */	0xa,		/* 10 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 3560 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3562 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3564 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3566 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pStg */

/* 3568 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 3570 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 3572 */	NdrFcShort( 0x280 ),	/* Type Offset=640 */

	/* Return value */

/* 3574 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 3576 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 3578 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure Save */

/* 3580 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 3582 */	NdrFcLong( 0x0 ),	/* 0 */
/* 3586 */	NdrFcShort( 0x7 ),	/* 7 */
/* 3588 */	NdrFcShort( 0x20 ),	/* ia64 Stack size/offset = 32 */
/* 3590 */	NdrFcShort( 0x8 ),	/* 8 */
/* 3592 */	NdrFcShort( 0x8 ),	/* 8 */
/* 3594 */	0x46,		/* Oi2 Flags:  clt must size, has return, has ext, */
			0x3,		/* 3 */
/* 3596 */	0xa,		/* 10 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 3598 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3600 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3602 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3604 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pStgSave */

/* 3606 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 3608 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 3610 */	NdrFcShort( 0x280 ),	/* Type Offset=640 */

	/* Parameter fSameAsLoad */

/* 3612 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 3614 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 3616 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 3618 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 3620 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 3622 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure SaveCompleted */

/* 3624 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 3626 */	NdrFcLong( 0x0 ),	/* 0 */
/* 3630 */	NdrFcShort( 0x8 ),	/* 8 */
/* 3632 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 3634 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3636 */	NdrFcShort( 0x8 ),	/* 8 */
/* 3638 */	0x46,		/* Oi2 Flags:  clt must size, has return, has ext, */
			0x2,		/* 2 */
/* 3640 */	0xa,		/* 10 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 3642 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3644 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3646 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3648 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pStgNew */

/* 3650 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 3652 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 3654 */	NdrFcShort( 0x280 ),	/* Type Offset=640 */

	/* Return value */

/* 3656 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 3658 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 3660 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure RemoteReadAt */

/* 3662 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 3664 */	NdrFcLong( 0x0 ),	/* 0 */
/* 3668 */	NdrFcShort( 0x3 ),	/* 3 */
/* 3670 */	NdrFcShort( 0x30 ),	/* ia64 Stack size/offset = 48 */
/* 3672 */	NdrFcShort( 0x20 ),	/* 32 */
/* 3674 */	NdrFcShort( 0x24 ),	/* 36 */
/* 3676 */	0x45,		/* Oi2 Flags:  srv must size, has return, has ext, */
			0x5,		/* 5 */
/* 3678 */	0xa,		/* 10 */
			0x3,		/* Ext Flags:  new corr desc, clt corr check, */
/* 3680 */	NdrFcShort( 0x1 ),	/* 1 */
/* 3682 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3684 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3686 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter ulOffset */

/* 3688 */	NdrFcShort( 0x8a ),	/* Flags:  must free, in, by val, */
/* 3690 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 3692 */	NdrFcShort( 0x19a ),	/* Type Offset=410 */

	/* Parameter pv */

/* 3694 */	NdrFcShort( 0x113 ),	/* Flags:  must size, must free, out, simple ref, */
/* 3696 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 3698 */	NdrFcShort( 0x2d6 ),	/* Type Offset=726 */

	/* Parameter cb */

/* 3700 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 3702 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 3704 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter pcbRead */

/* 3706 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 3708 */	NdrFcShort( 0x20 ),	/* ia64 Stack size/offset = 32 */
/* 3710 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 3712 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 3714 */	NdrFcShort( 0x28 ),	/* ia64 Stack size/offset = 40 */
/* 3716 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure RemoteFillAt */


	/* Procedure RemoteWriteAt */

/* 3718 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 3720 */	NdrFcLong( 0x0 ),	/* 0 */
/* 3724 */	NdrFcShort( 0x4 ),	/* 4 */
/* 3726 */	NdrFcShort( 0x30 ),	/* ia64 Stack size/offset = 48 */
/* 3728 */	NdrFcShort( 0x20 ),	/* 32 */
/* 3730 */	NdrFcShort( 0x24 ),	/* 36 */
/* 3732 */	0x46,		/* Oi2 Flags:  clt must size, has return, has ext, */
			0x5,		/* 5 */
/* 3734 */	0xa,		/* 10 */
			0x5,		/* Ext Flags:  new corr desc, srv corr check, */
/* 3736 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3738 */	NdrFcShort( 0x1 ),	/* 1 */
/* 3740 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3742 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter ulOffset */


	/* Parameter ulOffset */

/* 3744 */	NdrFcShort( 0x8a ),	/* Flags:  must free, in, by val, */
/* 3746 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 3748 */	NdrFcShort( 0x19a ),	/* Type Offset=410 */

	/* Parameter pv */


	/* Parameter pv */

/* 3750 */	NdrFcShort( 0x10b ),	/* Flags:  must size, must free, in, simple ref, */
/* 3752 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 3754 */	NdrFcShort( 0x2ec ),	/* Type Offset=748 */

	/* Parameter cb */


	/* Parameter cb */

/* 3756 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 3758 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 3760 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter pcbWritten */


	/* Parameter pcbWritten */

/* 3762 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 3764 */	NdrFcShort( 0x20 ),	/* ia64 Stack size/offset = 32 */
/* 3766 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */


	/* Return value */

/* 3768 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 3770 */	NdrFcShort( 0x28 ),	/* ia64 Stack size/offset = 40 */
/* 3772 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure LockRegion */

/* 3774 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 3776 */	NdrFcLong( 0x0 ),	/* 0 */
/* 3780 */	NdrFcShort( 0x7 ),	/* 7 */
/* 3782 */	NdrFcShort( 0x28 ),	/* ia64 Stack size/offset = 40 */
/* 3784 */	NdrFcShort( 0x38 ),	/* 56 */
/* 3786 */	NdrFcShort( 0x8 ),	/* 8 */
/* 3788 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x4,		/* 4 */
/* 3790 */	0xa,		/* 10 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 3792 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3794 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3796 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3798 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter libOffset */

/* 3800 */	NdrFcShort( 0x8a ),	/* Flags:  must free, in, by val, */
/* 3802 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 3804 */	NdrFcShort( 0x19a ),	/* Type Offset=410 */

	/* Parameter cb */

/* 3806 */	NdrFcShort( 0x8a ),	/* Flags:  must free, in, by val, */
/* 3808 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 3810 */	NdrFcShort( 0x19a ),	/* Type Offset=410 */

	/* Parameter dwLockType */

/* 3812 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 3814 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 3816 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 3818 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 3820 */	NdrFcShort( 0x20 ),	/* ia64 Stack size/offset = 32 */
/* 3822 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure UnlockRegion */

/* 3824 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 3826 */	NdrFcLong( 0x0 ),	/* 0 */
/* 3830 */	NdrFcShort( 0x8 ),	/* 8 */
/* 3832 */	NdrFcShort( 0x28 ),	/* ia64 Stack size/offset = 40 */
/* 3834 */	NdrFcShort( 0x38 ),	/* 56 */
/* 3836 */	NdrFcShort( 0x8 ),	/* 8 */
/* 3838 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x4,		/* 4 */
/* 3840 */	0xa,		/* 10 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 3842 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3844 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3846 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3848 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter libOffset */

/* 3850 */	NdrFcShort( 0x8a ),	/* Flags:  must free, in, by val, */
/* 3852 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 3854 */	NdrFcShort( 0x19a ),	/* Type Offset=410 */

	/* Parameter cb */

/* 3856 */	NdrFcShort( 0x8a ),	/* Flags:  must free, in, by val, */
/* 3858 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 3860 */	NdrFcShort( 0x19a ),	/* Type Offset=410 */

	/* Parameter dwLockType */

/* 3862 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 3864 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 3866 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 3868 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 3870 */	NdrFcShort( 0x20 ),	/* ia64 Stack size/offset = 32 */
/* 3872 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure Stat */

/* 3874 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 3876 */	NdrFcLong( 0x0 ),	/* 0 */
/* 3880 */	NdrFcShort( 0x9 ),	/* 9 */
/* 3882 */	NdrFcShort( 0x20 ),	/* ia64 Stack size/offset = 32 */
/* 3884 */	NdrFcShort( 0x8 ),	/* 8 */
/* 3886 */	NdrFcShort( 0x8 ),	/* 8 */
/* 3888 */	0x45,		/* Oi2 Flags:  srv must size, has return, has ext, */
			0x3,		/* 3 */
/* 3890 */	0xa,		/* 10 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 3892 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3894 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3896 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3898 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pstatstg */

/* 3900 */	NdrFcShort( 0x113 ),	/* Flags:  must size, must free, out, simple ref, */
/* 3902 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 3904 */	NdrFcShort( 0x210 ),	/* Type Offset=528 */

	/* Parameter grfStatFlag */

/* 3906 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 3908 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 3910 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 3912 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 3914 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 3916 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure RemoteNext */

/* 3918 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 3920 */	NdrFcLong( 0x0 ),	/* 0 */
/* 3924 */	NdrFcShort( 0x3 ),	/* 3 */
/* 3926 */	NdrFcShort( 0x28 ),	/* ia64 Stack size/offset = 40 */
/* 3928 */	NdrFcShort( 0x8 ),	/* 8 */
/* 3930 */	NdrFcShort( 0x24 ),	/* 36 */
/* 3932 */	0x45,		/* Oi2 Flags:  srv must size, has return, has ext, */
			0x4,		/* 4 */
/* 3934 */	0xa,		/* 10 */
			0x3,		/* Ext Flags:  new corr desc, clt corr check, */
/* 3936 */	NdrFcShort( 0x3 ),	/* 3 */
/* 3938 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3940 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3942 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter celt */

/* 3944 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 3946 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 3948 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter rgelt */

/* 3950 */	NdrFcShort( 0x113 ),	/* Flags:  must size, must free, out, simple ref, */
/* 3952 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 3954 */	NdrFcShort( 0x34e ),	/* Type Offset=846 */

	/* Parameter pceltFetched */

/* 3956 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 3958 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 3960 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 3962 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 3964 */	NdrFcShort( 0x20 ),	/* ia64 Stack size/offset = 32 */
/* 3966 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure Clone */

/* 3968 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 3970 */	NdrFcLong( 0x0 ),	/* 0 */
/* 3974 */	NdrFcShort( 0x6 ),	/* 6 */
/* 3976 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 3978 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3980 */	NdrFcShort( 0x8 ),	/* 8 */
/* 3982 */	0x45,		/* Oi2 Flags:  srv must size, has return, has ext, */
			0x2,		/* 2 */
/* 3984 */	0xa,		/* 10 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 3986 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3988 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3990 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3992 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter ppenum */

/* 3994 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
/* 3996 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 3998 */	NdrFcShort( 0x364 ),	/* Type Offset=868 */

	/* Return value */

/* 4000 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 4002 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 4004 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure RemoteNext */

/* 4006 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 4008 */	NdrFcLong( 0x0 ),	/* 0 */
/* 4012 */	NdrFcShort( 0x3 ),	/* 3 */
/* 4014 */	NdrFcShort( 0x28 ),	/* ia64 Stack size/offset = 40 */
/* 4016 */	NdrFcShort( 0x8 ),	/* 8 */
/* 4018 */	NdrFcShort( 0x24 ),	/* 36 */
/* 4020 */	0x45,		/* Oi2 Flags:  srv must size, has return, has ext, */
			0x4,		/* 4 */
/* 4022 */	0xa,		/* 10 */
			0x3,		/* Ext Flags:  new corr desc, clt corr check, */
/* 4024 */	NdrFcShort( 0x3 ),	/* 3 */
/* 4026 */	NdrFcShort( 0x0 ),	/* 0 */
/* 4028 */	NdrFcShort( 0x0 ),	/* 0 */
/* 4030 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter celt */

/* 4032 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 4034 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 4036 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter rgelt */

/* 4038 */	NdrFcShort( 0x113 ),	/* Flags:  must size, must free, out, simple ref, */
/* 4040 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 4042 */	NdrFcShort( 0x3a6 ),	/* Type Offset=934 */

	/* Parameter pceltFetched */

/* 4044 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 4046 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 4048 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 4050 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 4052 */	NdrFcShort( 0x20 ),	/* ia64 Stack size/offset = 32 */
/* 4054 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure Clone */

/* 4056 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 4058 */	NdrFcLong( 0x0 ),	/* 0 */
/* 4062 */	NdrFcShort( 0x6 ),	/* 6 */
/* 4064 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 4066 */	NdrFcShort( 0x0 ),	/* 0 */
/* 4068 */	NdrFcShort( 0x8 ),	/* 8 */
/* 4070 */	0x45,		/* Oi2 Flags:  srv must size, has return, has ext, */
			0x2,		/* 2 */
/* 4072 */	0xa,		/* 10 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 4074 */	NdrFcShort( 0x0 ),	/* 0 */
/* 4076 */	NdrFcShort( 0x0 ),	/* 0 */
/* 4078 */	NdrFcShort( 0x0 ),	/* 0 */
/* 4080 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter ppenum */

/* 4082 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
/* 4084 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 4086 */	NdrFcShort( 0x3bc ),	/* Type Offset=956 */

	/* Return value */

/* 4088 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 4090 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 4092 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure SwitchToFile */

/* 4094 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 4096 */	NdrFcLong( 0x0 ),	/* 0 */
/* 4100 */	NdrFcShort( 0x3 ),	/* 3 */
/* 4102 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 4104 */	NdrFcShort( 0x0 ),	/* 0 */
/* 4106 */	NdrFcShort( 0x8 ),	/* 8 */
/* 4108 */	0x46,		/* Oi2 Flags:  clt must size, has return, has ext, */
			0x2,		/* 2 */
/* 4110 */	0xa,		/* 10 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 4112 */	NdrFcShort( 0x0 ),	/* 0 */
/* 4114 */	NdrFcShort( 0x0 ),	/* 0 */
/* 4116 */	NdrFcShort( 0x0 ),	/* 0 */
/* 4118 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pszFile */

/* 4120 */	NdrFcShort( 0x10b ),	/* Flags:  must size, must free, in, simple ref, */
/* 4122 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 4124 */	NdrFcShort( 0xee ),	/* Type Offset=238 */

	/* Return value */

/* 4126 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 4128 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 4130 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure RemoteOnDataChange */

/* 4132 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 4134 */	NdrFcLong( 0x0 ),	/* 0 */
/* 4138 */	NdrFcShort( 0x3 ),	/* 3 */
/* 4140 */	NdrFcShort( 0x20 ),	/* ia64 Stack size/offset = 32 */
/* 4142 */	NdrFcShort( 0x0 ),	/* 0 */
/* 4144 */	NdrFcShort( 0x8 ),	/* 8 */
/* 4146 */	0x46,		/* Oi2 Flags:  clt must size, has return, has ext, */
			0x3,		/* 3 */
/* 4148 */	0xa,		/* 10 */
			0x5,		/* Ext Flags:  new corr desc, srv corr check, */
/* 4150 */	NdrFcShort( 0x0 ),	/* 0 */
/* 4152 */	NdrFcShort( 0x13 ),	/* 19 */
/* 4154 */	NdrFcShort( 0x0 ),	/* 0 */
/* 4156 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pFormatetc */

/* 4158 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 4160 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 4162 */	NdrFcShort( 0x3d2 ),	/* Type Offset=978 */

	/* Parameter pStgmed */

/* 4164 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 4166 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 4168 */	NdrFcShort( 0x3d6 ),	/* Type Offset=982 */

	/* Return value */

/* 4170 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 4172 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 4174 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure RemoteOnViewChange */

/* 4176 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 4178 */	NdrFcLong( 0x0 ),	/* 0 */
/* 4182 */	NdrFcShort( 0x4 ),	/* 4 */
/* 4184 */	NdrFcShort( 0x20 ),	/* ia64 Stack size/offset = 32 */
/* 4186 */	NdrFcShort( 0x10 ),	/* 16 */
/* 4188 */	NdrFcShort( 0x8 ),	/* 8 */
/* 4190 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x3,		/* 3 */
/* 4192 */	0xa,		/* 10 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 4194 */	NdrFcShort( 0x0 ),	/* 0 */
/* 4196 */	NdrFcShort( 0x0 ),	/* 0 */
/* 4198 */	NdrFcShort( 0x0 ),	/* 0 */
/* 4200 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter dwAspect */

/* 4202 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 4204 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 4206 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter lindex */

/* 4208 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 4210 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 4212 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 4214 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 4216 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 4218 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure RemoteOnSave */

/* 4220 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 4222 */	NdrFcLong( 0x0 ),	/* 0 */
/* 4226 */	NdrFcShort( 0x6 ),	/* 6 */
/* 4228 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 4230 */	NdrFcShort( 0x0 ),	/* 0 */
/* 4232 */	NdrFcShort( 0x8 ),	/* 8 */
/* 4234 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x1,		/* 1 */
/* 4236 */	0xa,		/* 10 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 4238 */	NdrFcShort( 0x0 ),	/* 0 */
/* 4240 */	NdrFcShort( 0x0 ),	/* 0 */
/* 4242 */	NdrFcShort( 0x0 ),	/* 0 */
/* 4244 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Return value */

/* 4246 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 4248 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 4250 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure RemoteOnClose */

/* 4252 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 4254 */	NdrFcLong( 0x0 ),	/* 0 */
/* 4258 */	NdrFcShort( 0x7 ),	/* 7 */
/* 4260 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 4262 */	NdrFcShort( 0x0 ),	/* 0 */
/* 4264 */	NdrFcShort( 0x8 ),	/* 8 */
/* 4266 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x1,		/* 1 */
/* 4268 */	0xa,		/* 10 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 4270 */	NdrFcShort( 0x0 ),	/* 0 */
/* 4272 */	NdrFcShort( 0x0 ),	/* 0 */
/* 4274 */	NdrFcShort( 0x0 ),	/* 0 */
/* 4276 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Return value */

/* 4278 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 4280 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 4282 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure Begin_RemoteOnDataChange */

/* 4284 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 4286 */	NdrFcLong( 0x0 ),	/* 0 */
/* 4290 */	NdrFcShort( 0x3 ),	/* 3 */
/* 4292 */	NdrFcShort( 0x20 ),	/* ia64 Stack size/offset = 32 */
/* 4294 */	NdrFcShort( 0x0 ),	/* 0 */
/* 4296 */	NdrFcShort( 0x8 ),	/* 8 */
/* 4298 */	0x66,		/* Oi2 Flags:  clt must size, has return, has async uuid, has ext, */
			0x3,		/* 3 */
/* 4300 */	0xa,		/* 10 */
			0x5,		/* Ext Flags:  new corr desc, srv corr check, */
/* 4302 */	NdrFcShort( 0x0 ),	/* 0 */
/* 4304 */	NdrFcShort( 0x13 ),	/* 19 */
/* 4306 */	NdrFcShort( 0x0 ),	/* 0 */
/* 4308 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pFormatetc */

/* 4310 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 4312 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 4314 */	NdrFcShort( 0x3d2 ),	/* Type Offset=978 */

	/* Parameter pStgmed */

/* 4316 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 4318 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 4320 */	NdrFcShort( 0x3d6 ),	/* Type Offset=982 */

	/* Return value */

/* 4322 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 4324 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 4326 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure Finish_RemoteOnDataChange */

/* 4328 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 4330 */	NdrFcLong( 0x0 ),	/* 0 */
/* 4334 */	NdrFcShort( 0x4 ),	/* 4 */
/* 4336 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 4338 */	NdrFcShort( 0x0 ),	/* 0 */
/* 4340 */	NdrFcShort( 0x8 ),	/* 8 */
/* 4342 */	0x64,		/* Oi2 Flags:  has return, has async uuid, has ext, */
			0x1,		/* 1 */
/* 4344 */	0xa,		/* 10 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 4346 */	NdrFcShort( 0x0 ),	/* 0 */
/* 4348 */	NdrFcShort( 0x0 ),	/* 0 */
/* 4350 */	NdrFcShort( 0x0 ),	/* 0 */
/* 4352 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Return value */

/* 4354 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 4356 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 4358 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure Begin_RemoteOnViewChange */

/* 4360 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 4362 */	NdrFcLong( 0x0 ),	/* 0 */
/* 4366 */	NdrFcShort( 0x5 ),	/* 5 */
/* 4368 */	NdrFcShort( 0x20 ),	/* ia64 Stack size/offset = 32 */
/* 4370 */	NdrFcShort( 0x10 ),	/* 16 */
/* 4372 */	NdrFcShort( 0x8 ),	/* 8 */
/* 4374 */	0x64,		/* Oi2 Flags:  has return, has async uuid, has ext, */
			0x3,		/* 3 */
/* 4376 */	0xa,		/* 10 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 4378 */	NdrFcShort( 0x0 ),	/* 0 */
/* 4380 */	NdrFcShort( 0x0 ),	/* 0 */
/* 4382 */	NdrFcShort( 0x0 ),	/* 0 */
/* 4384 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter dwAspect */

/* 4386 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 4388 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 4390 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter lindex */

/* 4392 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 4394 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 4396 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 4398 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 4400 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 4402 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure Finish_Push */


	/* Procedure Finish_Push */


	/* Procedure Finish_Push */


	/* Procedure Finish_RemoteOnViewChange */

/* 4404 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 4406 */	NdrFcLong( 0x0 ),	/* 0 */
/* 4410 */	NdrFcShort( 0x6 ),	/* 6 */
/* 4412 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 4414 */	NdrFcShort( 0x0 ),	/* 0 */
/* 4416 */	NdrFcShort( 0x8 ),	/* 8 */
/* 4418 */	0x64,		/* Oi2 Flags:  has return, has async uuid, has ext, */
			0x1,		/* 1 */
/* 4420 */	0xa,		/* 10 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 4422 */	NdrFcShort( 0x0 ),	/* 0 */
/* 4424 */	NdrFcShort( 0x0 ),	/* 0 */
/* 4426 */	NdrFcShort( 0x0 ),	/* 0 */
/* 4428 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Return value */


	/* Return value */


	/* Return value */


	/* Return value */

/* 4430 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 4432 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 4434 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure Begin_RemoteOnRename */

/* 4436 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 4438 */	NdrFcLong( 0x0 ),	/* 0 */
/* 4442 */	NdrFcShort( 0x7 ),	/* 7 */
/* 4444 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 4446 */	NdrFcShort( 0x0 ),	/* 0 */
/* 4448 */	NdrFcShort( 0x8 ),	/* 8 */
/* 4450 */	0x66,		/* Oi2 Flags:  clt must size, has return, has async uuid, has ext, */
			0x2,		/* 2 */
/* 4452 */	0xa,		/* 10 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 4454 */	NdrFcShort( 0x0 ),	/* 0 */
/* 4456 */	NdrFcShort( 0x0 ),	/* 0 */
/* 4458 */	NdrFcShort( 0x0 ),	/* 0 */
/* 4460 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pmk */

/* 4462 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 4464 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 4466 */	NdrFcShort( 0x10e ),	/* Type Offset=270 */

	/* Return value */

/* 4468 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 4470 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 4472 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure Finish_RemoteOnRename */

/* 4474 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 4476 */	NdrFcLong( 0x0 ),	/* 0 */
/* 4480 */	NdrFcShort( 0x8 ),	/* 8 */
/* 4482 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 4484 */	NdrFcShort( 0x0 ),	/* 0 */
/* 4486 */	NdrFcShort( 0x8 ),	/* 8 */
/* 4488 */	0x64,		/* Oi2 Flags:  has return, has async uuid, has ext, */
			0x1,		/* 1 */
/* 4490 */	0xa,		/* 10 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 4492 */	NdrFcShort( 0x0 ),	/* 0 */
/* 4494 */	NdrFcShort( 0x0 ),	/* 0 */
/* 4496 */	NdrFcShort( 0x0 ),	/* 0 */
/* 4498 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Return value */

/* 4500 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 4502 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 4504 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure Begin_RemoteOnSave */

/* 4506 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 4508 */	NdrFcLong( 0x0 ),	/* 0 */
/* 4512 */	NdrFcShort( 0x9 ),	/* 9 */
/* 4514 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 4516 */	NdrFcShort( 0x0 ),	/* 0 */
/* 4518 */	NdrFcShort( 0x8 ),	/* 8 */
/* 4520 */	0x64,		/* Oi2 Flags:  has return, has async uuid, has ext, */
			0x1,		/* 1 */
/* 4522 */	0xa,		/* 10 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 4524 */	NdrFcShort( 0x0 ),	/* 0 */
/* 4526 */	NdrFcShort( 0x0 ),	/* 0 */
/* 4528 */	NdrFcShort( 0x0 ),	/* 0 */
/* 4530 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Return value */

/* 4532 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 4534 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 4536 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure Finish_RemoteOnSave */

/* 4538 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 4540 */	NdrFcLong( 0x0 ),	/* 0 */
/* 4544 */	NdrFcShort( 0xa ),	/* 10 */
/* 4546 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 4548 */	NdrFcShort( 0x0 ),	/* 0 */
/* 4550 */	NdrFcShort( 0x8 ),	/* 8 */
/* 4552 */	0x64,		/* Oi2 Flags:  has return, has async uuid, has ext, */
			0x1,		/* 1 */
/* 4554 */	0xa,		/* 10 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 4556 */	NdrFcShort( 0x0 ),	/* 0 */
/* 4558 */	NdrFcShort( 0x0 ),	/* 0 */
/* 4560 */	NdrFcShort( 0x0 ),	/* 0 */
/* 4562 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Return value */

/* 4564 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 4566 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 4568 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure Begin_RemoteOnClose */

/* 4570 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 4572 */	NdrFcLong( 0x0 ),	/* 0 */
/* 4576 */	NdrFcShort( 0xb ),	/* 11 */
/* 4578 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 4580 */	NdrFcShort( 0x0 ),	/* 0 */
/* 4582 */	NdrFcShort( 0x8 ),	/* 8 */
/* 4584 */	0x64,		/* Oi2 Flags:  has return, has async uuid, has ext, */
			0x1,		/* 1 */
/* 4586 */	0xa,		/* 10 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 4588 */	NdrFcShort( 0x0 ),	/* 0 */
/* 4590 */	NdrFcShort( 0x0 ),	/* 0 */
/* 4592 */	NdrFcShort( 0x0 ),	/* 0 */
/* 4594 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Return value */

/* 4596 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 4598 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 4600 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure Finish_RemoteOnClose */

/* 4602 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 4604 */	NdrFcLong( 0x0 ),	/* 0 */
/* 4608 */	NdrFcShort( 0xc ),	/* 12 */
/* 4610 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 4612 */	NdrFcShort( 0x0 ),	/* 0 */
/* 4614 */	NdrFcShort( 0x8 ),	/* 8 */
/* 4616 */	0x64,		/* Oi2 Flags:  has return, has async uuid, has ext, */
			0x1,		/* 1 */
/* 4618 */	0xa,		/* 10 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 4620 */	NdrFcShort( 0x0 ),	/* 0 */
/* 4622 */	NdrFcShort( 0x0 ),	/* 0 */
/* 4624 */	NdrFcShort( 0x0 ),	/* 0 */
/* 4626 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Return value */

/* 4628 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 4630 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 4632 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure RemoteOnLinkSrcChange */

/* 4634 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 4636 */	NdrFcLong( 0x0 ),	/* 0 */
/* 4640 */	NdrFcShort( 0x8 ),	/* 8 */
/* 4642 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 4644 */	NdrFcShort( 0x0 ),	/* 0 */
/* 4646 */	NdrFcShort( 0x8 ),	/* 8 */
/* 4648 */	0x46,		/* Oi2 Flags:  clt must size, has return, has ext, */
			0x2,		/* 2 */
/* 4650 */	0xa,		/* 10 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 4652 */	NdrFcShort( 0x0 ),	/* 0 */
/* 4654 */	NdrFcShort( 0x0 ),	/* 0 */
/* 4656 */	NdrFcShort( 0x0 ),	/* 0 */
/* 4658 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pmk */

/* 4660 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 4662 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 4664 */	NdrFcShort( 0x10e ),	/* Type Offset=270 */

	/* Return value */

/* 4666 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 4668 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 4670 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure Begin_RemoteOnLinkSrcChange */

/* 4672 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 4674 */	NdrFcLong( 0x0 ),	/* 0 */
/* 4678 */	NdrFcShort( 0xd ),	/* 13 */
/* 4680 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 4682 */	NdrFcShort( 0x0 ),	/* 0 */
/* 4684 */	NdrFcShort( 0x8 ),	/* 8 */
/* 4686 */	0x66,		/* Oi2 Flags:  clt must size, has return, has async uuid, has ext, */
			0x2,		/* 2 */
/* 4688 */	0xa,		/* 10 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 4690 */	NdrFcShort( 0x0 ),	/* 0 */
/* 4692 */	NdrFcShort( 0x0 ),	/* 0 */
/* 4694 */	NdrFcShort( 0x0 ),	/* 0 */
/* 4696 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pmk */

/* 4698 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 4700 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 4702 */	NdrFcShort( 0x10e ),	/* Type Offset=270 */

	/* Return value */

/* 4704 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 4706 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 4708 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure Finish_RemoteOnLinkSrcChange */

/* 4710 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 4712 */	NdrFcLong( 0x0 ),	/* 0 */
/* 4716 */	NdrFcShort( 0xe ),	/* 14 */
/* 4718 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 4720 */	NdrFcShort( 0x0 ),	/* 0 */
/* 4722 */	NdrFcShort( 0x8 ),	/* 8 */
/* 4724 */	0x64,		/* Oi2 Flags:  has return, has async uuid, has ext, */
			0x1,		/* 1 */
/* 4726 */	0xa,		/* 10 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 4728 */	NdrFcShort( 0x0 ),	/* 0 */
/* 4730 */	NdrFcShort( 0x0 ),	/* 0 */
/* 4732 */	NdrFcShort( 0x0 ),	/* 0 */
/* 4734 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Return value */

/* 4736 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 4738 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 4740 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure RemoteGetData */

/* 4742 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 4744 */	NdrFcLong( 0x0 ),	/* 0 */
/* 4748 */	NdrFcShort( 0x3 ),	/* 3 */
/* 4750 */	NdrFcShort( 0x20 ),	/* ia64 Stack size/offset = 32 */
/* 4752 */	NdrFcShort( 0x0 ),	/* 0 */
/* 4754 */	NdrFcShort( 0x8 ),	/* 8 */
/* 4756 */	0x47,		/* Oi2 Flags:  srv must size, clt must size, has return, has ext, */
			0x3,		/* 3 */
/* 4758 */	0xa,		/* 10 */
			0x7,		/* Ext Flags:  new corr desc, clt corr check, srv corr check, */
/* 4760 */	NdrFcShort( 0x11 ),	/* 17 */
/* 4762 */	NdrFcShort( 0x2 ),	/* 2 */
/* 4764 */	NdrFcShort( 0x0 ),	/* 0 */
/* 4766 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pformatetcIn */

/* 4768 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 4770 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 4772 */	NdrFcShort( 0x3d2 ),	/* Type Offset=978 */

	/* Parameter pRemoteMedium */

/* 4774 */	NdrFcShort( 0x6113 ),	/* Flags:  must size, must free, out, simple ref, srv alloc size=24 */
/* 4776 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 4778 */	NdrFcShort( 0x55a ),	/* Type Offset=1370 */

	/* Return value */

/* 4780 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 4782 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 4784 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure RemoteGetDataHere */

/* 4786 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 4788 */	NdrFcLong( 0x0 ),	/* 0 */
/* 4792 */	NdrFcShort( 0x4 ),	/* 4 */
/* 4794 */	NdrFcShort( 0x20 ),	/* ia64 Stack size/offset = 32 */
/* 4796 */	NdrFcShort( 0x0 ),	/* 0 */
/* 4798 */	NdrFcShort( 0x8 ),	/* 8 */
/* 4800 */	0x47,		/* Oi2 Flags:  srv must size, clt must size, has return, has ext, */
			0x3,		/* 3 */
/* 4802 */	0xa,		/* 10 */
			0x7,		/* Ext Flags:  new corr desc, clt corr check, srv corr check, */
/* 4804 */	NdrFcShort( 0x11 ),	/* 17 */
/* 4806 */	NdrFcShort( 0x13 ),	/* 19 */
/* 4808 */	NdrFcShort( 0x0 ),	/* 0 */
/* 4810 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pformatetc */

/* 4812 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 4814 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 4816 */	NdrFcShort( 0x3d2 ),	/* Type Offset=978 */

	/* Parameter pRemoteMedium */

/* 4818 */	NdrFcShort( 0x11b ),	/* Flags:  must size, must free, in, out, simple ref, */
/* 4820 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 4822 */	NdrFcShort( 0x55a ),	/* Type Offset=1370 */

	/* Return value */

/* 4824 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 4826 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 4828 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure QueryGetData */

/* 4830 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 4832 */	NdrFcLong( 0x0 ),	/* 0 */
/* 4836 */	NdrFcShort( 0x5 ),	/* 5 */
/* 4838 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 4840 */	NdrFcShort( 0x0 ),	/* 0 */
/* 4842 */	NdrFcShort( 0x8 ),	/* 8 */
/* 4844 */	0x46,		/* Oi2 Flags:  clt must size, has return, has ext, */
			0x2,		/* 2 */
/* 4846 */	0xa,		/* 10 */
			0x5,		/* Ext Flags:  new corr desc, srv corr check, */
/* 4848 */	NdrFcShort( 0x0 ),	/* 0 */
/* 4850 */	NdrFcShort( 0x2 ),	/* 2 */
/* 4852 */	NdrFcShort( 0x0 ),	/* 0 */
/* 4854 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pformatetc */

/* 4856 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 4858 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 4860 */	NdrFcShort( 0x3d2 ),	/* Type Offset=978 */

	/* Return value */

/* 4862 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 4864 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 4866 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetCanonicalFormatEtc */

/* 4868 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 4870 */	NdrFcLong( 0x0 ),	/* 0 */
/* 4874 */	NdrFcShort( 0x6 ),	/* 6 */
/* 4876 */	NdrFcShort( 0x20 ),	/* ia64 Stack size/offset = 32 */
/* 4878 */	NdrFcShort( 0x0 ),	/* 0 */
/* 4880 */	NdrFcShort( 0x8 ),	/* 8 */
/* 4882 */	0x47,		/* Oi2 Flags:  srv must size, clt must size, has return, has ext, */
			0x3,		/* 3 */
/* 4884 */	0xa,		/* 10 */
			0x7,		/* Ext Flags:  new corr desc, clt corr check, srv corr check, */
/* 4886 */	NdrFcShort( 0x2 ),	/* 2 */
/* 4888 */	NdrFcShort( 0x2 ),	/* 2 */
/* 4890 */	NdrFcShort( 0x0 ),	/* 0 */
/* 4892 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pformatectIn */

/* 4894 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 4896 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 4898 */	NdrFcShort( 0x3d2 ),	/* Type Offset=978 */

	/* Parameter pformatetcOut */

/* 4900 */	NdrFcShort( 0x8113 ),	/* Flags:  must size, must free, out, simple ref, srv alloc size=32 */
/* 4902 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 4904 */	NdrFcShort( 0x336 ),	/* Type Offset=822 */

	/* Return value */

/* 4906 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 4908 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 4910 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure RemoteSetData */

/* 4912 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 4914 */	NdrFcLong( 0x0 ),	/* 0 */
/* 4918 */	NdrFcShort( 0x7 ),	/* 7 */
/* 4920 */	NdrFcShort( 0x28 ),	/* ia64 Stack size/offset = 40 */
/* 4922 */	NdrFcShort( 0x8 ),	/* 8 */
/* 4924 */	NdrFcShort( 0x8 ),	/* 8 */
/* 4926 */	0x46,		/* Oi2 Flags:  clt must size, has return, has ext, */
			0x4,		/* 4 */
/* 4928 */	0xa,		/* 10 */
			0x5,		/* Ext Flags:  new corr desc, srv corr check, */
/* 4930 */	NdrFcShort( 0x0 ),	/* 0 */
/* 4932 */	NdrFcShort( 0x13 ),	/* 19 */
/* 4934 */	NdrFcShort( 0x0 ),	/* 0 */
/* 4936 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pformatetc */

/* 4938 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 4940 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 4942 */	NdrFcShort( 0x3d2 ),	/* Type Offset=978 */

	/* Parameter pmedium */

/* 4944 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 4946 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 4948 */	NdrFcShort( 0x56c ),	/* Type Offset=1388 */

	/* Parameter fRelease */

/* 4950 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 4952 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 4954 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 4956 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 4958 */	NdrFcShort( 0x20 ),	/* ia64 Stack size/offset = 32 */
/* 4960 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure EnumFormatEtc */

/* 4962 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 4964 */	NdrFcLong( 0x0 ),	/* 0 */
/* 4968 */	NdrFcShort( 0x8 ),	/* 8 */
/* 4970 */	NdrFcShort( 0x20 ),	/* ia64 Stack size/offset = 32 */
/* 4972 */	NdrFcShort( 0x8 ),	/* 8 */
/* 4974 */	NdrFcShort( 0x8 ),	/* 8 */
/* 4976 */	0x45,		/* Oi2 Flags:  srv must size, has return, has ext, */
			0x3,		/* 3 */
/* 4978 */	0xa,		/* 10 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 4980 */	NdrFcShort( 0x0 ),	/* 0 */
/* 4982 */	NdrFcShort( 0x0 ),	/* 0 */
/* 4984 */	NdrFcShort( 0x0 ),	/* 0 */
/* 4986 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter dwDirection */

/* 4988 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 4990 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 4992 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter ppenumFormatEtc */

/* 4994 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
/* 4996 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 4998 */	NdrFcShort( 0x364 ),	/* Type Offset=868 */

	/* Return value */

/* 5000 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 5002 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 5004 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure DAdvise */

/* 5006 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 5008 */	NdrFcLong( 0x0 ),	/* 0 */
/* 5012 */	NdrFcShort( 0x9 ),	/* 9 */
/* 5014 */	NdrFcShort( 0x30 ),	/* ia64 Stack size/offset = 48 */
/* 5016 */	NdrFcShort( 0x8 ),	/* 8 */
/* 5018 */	NdrFcShort( 0x24 ),	/* 36 */
/* 5020 */	0x46,		/* Oi2 Flags:  clt must size, has return, has ext, */
			0x5,		/* 5 */
/* 5022 */	0xa,		/* 10 */
			0x5,		/* Ext Flags:  new corr desc, srv corr check, */
/* 5024 */	NdrFcShort( 0x0 ),	/* 0 */
/* 5026 */	NdrFcShort( 0x2 ),	/* 2 */
/* 5028 */	NdrFcShort( 0x0 ),	/* 0 */
/* 5030 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pformatetc */

/* 5032 */	NdrFcShort( 0x10b ),	/* Flags:  must size, must free, in, simple ref, */
/* 5034 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 5036 */	NdrFcShort( 0x336 ),	/* Type Offset=822 */

	/* Parameter advf */

/* 5038 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 5040 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 5042 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter pAdvSink */

/* 5044 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 5046 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 5048 */	NdrFcShort( 0x37e ),	/* Type Offset=894 */

	/* Parameter pdwConnection */

/* 5050 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 5052 */	NdrFcShort( 0x20 ),	/* ia64 Stack size/offset = 32 */
/* 5054 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 5056 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 5058 */	NdrFcShort( 0x28 ),	/* ia64 Stack size/offset = 40 */
/* 5060 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure DUnadvise */

/* 5062 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 5064 */	NdrFcLong( 0x0 ),	/* 0 */
/* 5068 */	NdrFcShort( 0xa ),	/* 10 */
/* 5070 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 5072 */	NdrFcShort( 0x8 ),	/* 8 */
/* 5074 */	NdrFcShort( 0x8 ),	/* 8 */
/* 5076 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x2,		/* 2 */
/* 5078 */	0xa,		/* 10 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 5080 */	NdrFcShort( 0x0 ),	/* 0 */
/* 5082 */	NdrFcShort( 0x0 ),	/* 0 */
/* 5084 */	NdrFcShort( 0x0 ),	/* 0 */
/* 5086 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter dwConnection */

/* 5088 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 5090 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 5092 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 5094 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 5096 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 5098 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure EnumDAdvise */

/* 5100 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 5102 */	NdrFcLong( 0x0 ),	/* 0 */
/* 5106 */	NdrFcShort( 0xb ),	/* 11 */
/* 5108 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 5110 */	NdrFcShort( 0x0 ),	/* 0 */
/* 5112 */	NdrFcShort( 0x8 ),	/* 8 */
/* 5114 */	0x45,		/* Oi2 Flags:  srv must size, has return, has ext, */
			0x2,		/* 2 */
/* 5116 */	0xa,		/* 10 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 5118 */	NdrFcShort( 0x0 ),	/* 0 */
/* 5120 */	NdrFcShort( 0x0 ),	/* 0 */
/* 5122 */	NdrFcShort( 0x0 ),	/* 0 */
/* 5124 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter ppenumAdvise */

/* 5126 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
/* 5128 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 5130 */	NdrFcShort( 0x3bc ),	/* Type Offset=956 */

	/* Return value */

/* 5132 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 5134 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 5136 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetClassObject */

/* 5138 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 5140 */	NdrFcLong( 0x0 ),	/* 0 */
/* 5144 */	NdrFcShort( 0x3 ),	/* 3 */
/* 5146 */	NdrFcShort( 0x38 ),	/* ia64 Stack size/offset = 56 */
/* 5148 */	NdrFcShort( 0x98 ),	/* 152 */
/* 5150 */	NdrFcShort( 0x8 ),	/* 8 */
/* 5152 */	0x45,		/* Oi2 Flags:  srv must size, has return, has ext, */
			0x6,		/* 6 */
/* 5154 */	0xa,		/* 10 */
			0x3,		/* Ext Flags:  new corr desc, clt corr check, */
/* 5156 */	NdrFcShort( 0x1 ),	/* 1 */
/* 5158 */	NdrFcShort( 0x0 ),	/* 0 */
/* 5160 */	NdrFcShort( 0x0 ),	/* 0 */
/* 5162 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter rclsid */

/* 5164 */	NdrFcShort( 0x10a ),	/* Flags:  must free, in, simple ref, */
/* 5166 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 5168 */	NdrFcShort( 0x156 ),	/* Type Offset=342 */

	/* Parameter dwClassContext */

/* 5170 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 5172 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 5174 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter locale */

/* 5176 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 5178 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 5180 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter riid */

/* 5182 */	NdrFcShort( 0x10a ),	/* Flags:  must free, in, simple ref, */
/* 5184 */	NdrFcShort( 0x20 ),	/* ia64 Stack size/offset = 32 */
/* 5186 */	NdrFcShort( 0x156 ),	/* Type Offset=342 */

	/* Parameter ppv */

/* 5188 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
/* 5190 */	NdrFcShort( 0x28 ),	/* ia64 Stack size/offset = 40 */
/* 5192 */	NdrFcShort( 0x592 ),	/* Type Offset=1426 */

	/* Return value */

/* 5194 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 5196 */	NdrFcShort( 0x30 ),	/* ia64 Stack size/offset = 48 */
/* 5198 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure RemoteFillAppend */

/* 5200 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 5202 */	NdrFcLong( 0x0 ),	/* 0 */
/* 5206 */	NdrFcShort( 0x3 ),	/* 3 */
/* 5208 */	NdrFcShort( 0x28 ),	/* ia64 Stack size/offset = 40 */
/* 5210 */	NdrFcShort( 0x8 ),	/* 8 */
/* 5212 */	NdrFcShort( 0x24 ),	/* 36 */
/* 5214 */	0x46,		/* Oi2 Flags:  clt must size, has return, has ext, */
			0x4,		/* 4 */
/* 5216 */	0xa,		/* 10 */
			0x5,		/* Ext Flags:  new corr desc, srv corr check, */
/* 5218 */	NdrFcShort( 0x0 ),	/* 0 */
/* 5220 */	NdrFcShort( 0x1 ),	/* 1 */
/* 5222 */	NdrFcShort( 0x0 ),	/* 0 */
/* 5224 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pv */

/* 5226 */	NdrFcShort( 0x10b ),	/* Flags:  must size, must free, in, simple ref, */
/* 5228 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 5230 */	NdrFcShort( 0x1d0 ),	/* Type Offset=464 */

	/* Parameter cb */

/* 5232 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 5234 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 5236 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter pcbWritten */

/* 5238 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 5240 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 5242 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 5244 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 5246 */	NdrFcShort( 0x20 ),	/* ia64 Stack size/offset = 32 */
/* 5248 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure SetFillSize */

/* 5250 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 5252 */	NdrFcLong( 0x0 ),	/* 0 */
/* 5256 */	NdrFcShort( 0x5 ),	/* 5 */
/* 5258 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 5260 */	NdrFcShort( 0x18 ),	/* 24 */
/* 5262 */	NdrFcShort( 0x8 ),	/* 8 */
/* 5264 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x2,		/* 2 */
/* 5266 */	0xa,		/* 10 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 5268 */	NdrFcShort( 0x0 ),	/* 0 */
/* 5270 */	NdrFcShort( 0x0 ),	/* 0 */
/* 5272 */	NdrFcShort( 0x0 ),	/* 0 */
/* 5274 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter ulSize */

/* 5276 */	NdrFcShort( 0x8a ),	/* Flags:  must free, in, by val, */
/* 5278 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 5280 */	NdrFcShort( 0x19a ),	/* Type Offset=410 */

	/* Return value */

/* 5282 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 5284 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 5286 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure Terminate */

/* 5288 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 5290 */	NdrFcLong( 0x0 ),	/* 0 */
/* 5294 */	NdrFcShort( 0x6 ),	/* 6 */
/* 5296 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 5298 */	NdrFcShort( 0x8 ),	/* 8 */
/* 5300 */	NdrFcShort( 0x8 ),	/* 8 */
/* 5302 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x2,		/* 2 */
/* 5304 */	0xa,		/* 10 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 5306 */	NdrFcShort( 0x0 ),	/* 0 */
/* 5308 */	NdrFcShort( 0x0 ),	/* 0 */
/* 5310 */	NdrFcShort( 0x0 ),	/* 0 */
/* 5312 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter bCanceled */

/* 5314 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 5316 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 5318 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 5320 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 5322 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 5324 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure OnProgress */

/* 5326 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 5328 */	NdrFcLong( 0x0 ),	/* 0 */
/* 5332 */	NdrFcShort( 0x3 ),	/* 3 */
/* 5334 */	NdrFcShort( 0x30 ),	/* ia64 Stack size/offset = 48 */
/* 5336 */	NdrFcShort( 0x20 ),	/* 32 */
/* 5338 */	NdrFcShort( 0x8 ),	/* 8 */
/* 5340 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x5,		/* 5 */
/* 5342 */	0xa,		/* 10 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 5344 */	NdrFcShort( 0x0 ),	/* 0 */
/* 5346 */	NdrFcShort( 0x0 ),	/* 0 */
/* 5348 */	NdrFcShort( 0x0 ),	/* 0 */
/* 5350 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter dwProgressCurrent */

/* 5352 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 5354 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 5356 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter dwProgressMaximum */

/* 5358 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 5360 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 5362 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter fAccurate */

/* 5364 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 5366 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 5368 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter fOwner */

/* 5370 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 5372 */	NdrFcShort( 0x20 ),	/* ia64 Stack size/offset = 32 */
/* 5374 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 5376 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 5378 */	NdrFcShort( 0x28 ),	/* ia64 Stack size/offset = 40 */
/* 5380 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure WaitForWriteAccess */


	/* Procedure Lock */

/* 5382 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 5384 */	NdrFcLong( 0x0 ),	/* 0 */
/* 5388 */	NdrFcShort( 0x3 ),	/* 3 */
/* 5390 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 5392 */	NdrFcShort( 0x8 ),	/* 8 */
/* 5394 */	NdrFcShort( 0x8 ),	/* 8 */
/* 5396 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x2,		/* 2 */
/* 5398 */	0xa,		/* 10 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 5400 */	NdrFcShort( 0x0 ),	/* 0 */
/* 5402 */	NdrFcShort( 0x0 ),	/* 0 */
/* 5404 */	NdrFcShort( 0x0 ),	/* 0 */
/* 5406 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter dwTimeout */


	/* Parameter dwTimeout */

/* 5408 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 5410 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 5412 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */


	/* Return value */

/* 5414 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 5416 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 5418 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure Wait */


	/* Procedure SuppressChanges */

/* 5420 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 5422 */	NdrFcLong( 0x0 ),	/* 0 */
/* 5426 */	NdrFcShort( 0x3 ),	/* 3 */
/* 5428 */	NdrFcShort( 0x20 ),	/* ia64 Stack size/offset = 32 */
/* 5430 */	NdrFcShort( 0x10 ),	/* 16 */
/* 5432 */	NdrFcShort( 0x8 ),	/* 8 */
/* 5434 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x3,		/* 3 */
/* 5436 */	0xa,		/* 10 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 5438 */	NdrFcShort( 0x0 ),	/* 0 */
/* 5440 */	NdrFcShort( 0x0 ),	/* 0 */
/* 5442 */	NdrFcShort( 0x0 ),	/* 0 */
/* 5444 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter dwFlags */


	/* Parameter res1 */

/* 5446 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 5448 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 5450 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter dwMilliseconds */


	/* Parameter res2 */

/* 5452 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 5454 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 5456 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */


	/* Return value */

/* 5458 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 5460 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 5462 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure CreateStorageEx */

/* 5464 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 5466 */	NdrFcLong( 0x0 ),	/* 0 */
/* 5470 */	NdrFcShort( 0x3 ),	/* 3 */
/* 5472 */	NdrFcShort( 0x40 ),	/* ia64 Stack size/offset = 64 */
/* 5474 */	NdrFcShort( 0x5c ),	/* 92 */
/* 5476 */	NdrFcShort( 0x8 ),	/* 8 */
/* 5478 */	0x47,		/* Oi2 Flags:  srv must size, clt must size, has return, has ext, */
			0x7,		/* 7 */
/* 5480 */	0xa,		/* 10 */
			0x3,		/* Ext Flags:  new corr desc, clt corr check, */
/* 5482 */	NdrFcShort( 0x1 ),	/* 1 */
/* 5484 */	NdrFcShort( 0x0 ),	/* 0 */
/* 5486 */	NdrFcShort( 0x0 ),	/* 0 */
/* 5488 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pwcsName */

/* 5490 */	NdrFcShort( 0x10b ),	/* Flags:  must size, must free, in, simple ref, */
/* 5492 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 5494 */	NdrFcShort( 0xee ),	/* Type Offset=238 */

	/* Parameter grfMode */

/* 5496 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 5498 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 5500 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter stgfmt */

/* 5502 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 5504 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 5506 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter grfAttrs */

/* 5508 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 5510 */	NdrFcShort( 0x20 ),	/* ia64 Stack size/offset = 32 */
/* 5512 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter riid */

/* 5514 */	NdrFcShort( 0x10a ),	/* Flags:  must free, in, simple ref, */
/* 5516 */	NdrFcShort( 0x28 ),	/* ia64 Stack size/offset = 40 */
/* 5518 */	NdrFcShort( 0x156 ),	/* Type Offset=342 */

	/* Parameter ppstgOpen */

/* 5520 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
/* 5522 */	NdrFcShort( 0x30 ),	/* ia64 Stack size/offset = 48 */
/* 5524 */	NdrFcShort( 0x59e ),	/* Type Offset=1438 */

	/* Return value */

/* 5526 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 5528 */	NdrFcShort( 0x38 ),	/* ia64 Stack size/offset = 56 */
/* 5530 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure OpenStorageEx */

/* 5532 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 5534 */	NdrFcLong( 0x0 ),	/* 0 */
/* 5538 */	NdrFcShort( 0x4 ),	/* 4 */
/* 5540 */	NdrFcShort( 0x40 ),	/* ia64 Stack size/offset = 64 */
/* 5542 */	NdrFcShort( 0x5c ),	/* 92 */
/* 5544 */	NdrFcShort( 0x8 ),	/* 8 */
/* 5546 */	0x47,		/* Oi2 Flags:  srv must size, clt must size, has return, has ext, */
			0x7,		/* 7 */
/* 5548 */	0xa,		/* 10 */
			0x3,		/* Ext Flags:  new corr desc, clt corr check, */
/* 5550 */	NdrFcShort( 0x1 ),	/* 1 */
/* 5552 */	NdrFcShort( 0x0 ),	/* 0 */
/* 5554 */	NdrFcShort( 0x0 ),	/* 0 */
/* 5556 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pwcsName */

/* 5558 */	NdrFcShort( 0x10b ),	/* Flags:  must size, must free, in, simple ref, */
/* 5560 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 5562 */	NdrFcShort( 0xee ),	/* Type Offset=238 */

	/* Parameter grfMode */

/* 5564 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 5566 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 5568 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter stgfmt */

/* 5570 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 5572 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 5574 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter grfAttrs */

/* 5576 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 5578 */	NdrFcShort( 0x20 ),	/* ia64 Stack size/offset = 32 */
/* 5580 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter riid */

/* 5582 */	NdrFcShort( 0x10a ),	/* Flags:  must free, in, simple ref, */
/* 5584 */	NdrFcShort( 0x28 ),	/* ia64 Stack size/offset = 40 */
/* 5586 */	NdrFcShort( 0x156 ),	/* Type Offset=342 */

	/* Parameter ppstgOpen */

/* 5588 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
/* 5590 */	NdrFcShort( 0x30 ),	/* ia64 Stack size/offset = 48 */
/* 5592 */	NdrFcShort( 0x5aa ),	/* Type Offset=1450 */

	/* Return value */

/* 5594 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 5596 */	NdrFcShort( 0x38 ),	/* ia64 Stack size/offset = 56 */
/* 5598 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure LoadDllServer */

/* 5600 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 5602 */	NdrFcLong( 0x0 ),	/* 0 */
/* 5606 */	NdrFcShort( 0x3 ),	/* 3 */
/* 5608 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 5610 */	NdrFcShort( 0x44 ),	/* 68 */
/* 5612 */	NdrFcShort( 0x8 ),	/* 8 */
/* 5614 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x2,		/* 2 */
/* 5616 */	0xa,		/* 10 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 5618 */	NdrFcShort( 0x0 ),	/* 0 */
/* 5620 */	NdrFcShort( 0x0 ),	/* 0 */
/* 5622 */	NdrFcShort( 0x0 ),	/* 0 */
/* 5624 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter Clsid */

/* 5626 */	NdrFcShort( 0x10a ),	/* Flags:  must free, in, simple ref, */
/* 5628 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 5630 */	NdrFcShort( 0x156 ),	/* Type Offset=342 */

	/* Return value */

/* 5632 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 5634 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 5636 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure AsyncGetClassBits */

/* 5638 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 5640 */	NdrFcLong( 0x0 ),	/* 0 */
/* 5644 */	NdrFcShort( 0x3 ),	/* 3 */
/* 5646 */	NdrFcShort( 0x60 ),	/* ia64 Stack size/offset = 96 */
/* 5648 */	NdrFcShort( 0xa8 ),	/* 168 */
/* 5650 */	NdrFcShort( 0x8 ),	/* 8 */
/* 5652 */	0x46,		/* Oi2 Flags:  clt must size, has return, has ext, */
			0xb,		/* 11 */
/* 5654 */	0xa,		/* 10 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 5656 */	NdrFcShort( 0x0 ),	/* 0 */
/* 5658 */	NdrFcShort( 0x0 ),	/* 0 */
/* 5660 */	NdrFcShort( 0x0 ),	/* 0 */
/* 5662 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter rclsid */

/* 5664 */	NdrFcShort( 0x10a ),	/* Flags:  must free, in, simple ref, */
/* 5666 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 5668 */	NdrFcShort( 0x156 ),	/* Type Offset=342 */

	/* Parameter pszTYPE */

/* 5670 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 5672 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 5674 */	NdrFcShort( 0x292 ),	/* Type Offset=658 */

	/* Parameter pszExt */

/* 5676 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 5678 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 5680 */	NdrFcShort( 0x292 ),	/* Type Offset=658 */

	/* Parameter dwFileVersionMS */

/* 5682 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 5684 */	NdrFcShort( 0x20 ),	/* ia64 Stack size/offset = 32 */
/* 5686 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter dwFileVersionLS */

/* 5688 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 5690 */	NdrFcShort( 0x28 ),	/* ia64 Stack size/offset = 40 */
/* 5692 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter pszCodeBase */

/* 5694 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 5696 */	NdrFcShort( 0x30 ),	/* ia64 Stack size/offset = 48 */
/* 5698 */	NdrFcShort( 0x292 ),	/* Type Offset=658 */

	/* Parameter pbc */

/* 5700 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 5702 */	NdrFcShort( 0x38 ),	/* ia64 Stack size/offset = 56 */
/* 5704 */	NdrFcShort( 0x162 ),	/* Type Offset=354 */

	/* Parameter dwClassContext */

/* 5706 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 5708 */	NdrFcShort( 0x40 ),	/* ia64 Stack size/offset = 64 */
/* 5710 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter riid */

/* 5712 */	NdrFcShort( 0x10a ),	/* Flags:  must free, in, simple ref, */
/* 5714 */	NdrFcShort( 0x48 ),	/* ia64 Stack size/offset = 72 */
/* 5716 */	NdrFcShort( 0x156 ),	/* Type Offset=342 */

	/* Parameter flags */

/* 5718 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 5720 */	NdrFcShort( 0x50 ),	/* ia64 Stack size/offset = 80 */
/* 5722 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 5724 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 5726 */	NdrFcShort( 0x58 ),	/* ia64 Stack size/offset = 88 */
/* 5728 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure Push */

/* 5730 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 5732 */	NdrFcLong( 0x0 ),	/* 0 */
/* 5736 */	NdrFcShort( 0x4 ),	/* 4 */
/* 5738 */	NdrFcShort( 0x20 ),	/* ia64 Stack size/offset = 32 */
/* 5740 */	NdrFcShort( 0x8 ),	/* 8 */
/* 5742 */	NdrFcShort( 0x8 ),	/* 8 */
/* 5744 */	0x46,		/* Oi2 Flags:  clt must size, has return, has ext, */
			0x3,		/* 3 */
/* 5746 */	0xa,		/* 10 */
			0x5,		/* Ext Flags:  new corr desc, srv corr check, */
/* 5748 */	NdrFcShort( 0x0 ),	/* 0 */
/* 5750 */	NdrFcShort( 0x1 ),	/* 1 */
/* 5752 */	NdrFcShort( 0x0 ),	/* 0 */
/* 5754 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter buf */

/* 5756 */	NdrFcShort( 0x10b ),	/* Flags:  must size, must free, in, simple ref, */
/* 5758 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 5760 */	NdrFcShort( 0x1d0 ),	/* Type Offset=464 */

	/* Parameter cSent */

/* 5762 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 5764 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 5766 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 5768 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 5770 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 5772 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure Begin_Pull */


	/* Procedure Begin_Pull */


	/* Procedure Begin_Pull */

/* 5774 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 5776 */	NdrFcLong( 0x0 ),	/* 0 */
/* 5780 */	NdrFcShort( 0x3 ),	/* 3 */
/* 5782 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 5784 */	NdrFcShort( 0x8 ),	/* 8 */
/* 5786 */	NdrFcShort( 0x8 ),	/* 8 */
/* 5788 */	0x64,		/* Oi2 Flags:  has return, has async uuid, has ext, */
			0x2,		/* 2 */
/* 5790 */	0xa,		/* 10 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 5792 */	NdrFcShort( 0x0 ),	/* 0 */
/* 5794 */	NdrFcShort( 0x0 ),	/* 0 */
/* 5796 */	NdrFcShort( 0x0 ),	/* 0 */
/* 5798 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter cRequest */


	/* Parameter cRequest */


	/* Parameter cRequest */

/* 5800 */	NdrFcShort( 0x448 ),	/* Flags:  in, base type, split async, */
/* 5802 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 5804 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */


	/* Return value */


	/* Return value */

/* 5806 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 5808 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 5810 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure Finish_Pull */

/* 5812 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 5814 */	NdrFcLong( 0x0 ),	/* 0 */
/* 5818 */	NdrFcShort( 0x4 ),	/* 4 */
/* 5820 */	NdrFcShort( 0x20 ),	/* ia64 Stack size/offset = 32 */
/* 5822 */	NdrFcShort( 0x0 ),	/* 0 */
/* 5824 */	NdrFcShort( 0x24 ),	/* 36 */
/* 5826 */	0x65,		/* Oi2 Flags:  srv must size, has return, has async uuid, has ext, */
			0x3,		/* 3 */
/* 5828 */	0xa,		/* 10 */
			0x3,		/* Ext Flags:  new corr desc, clt corr check, */
/* 5830 */	NdrFcShort( 0x1 ),	/* 1 */
/* 5832 */	NdrFcShort( 0x0 ),	/* 0 */
/* 5834 */	NdrFcShort( 0x0 ),	/* 0 */
/* 5836 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter buf */

/* 5838 */	NdrFcShort( 0x113 ),	/* Flags:  must size, must free, out, simple ref, */
/* 5840 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 5842 */	NdrFcShort( 0x5ba ),	/* Type Offset=1466 */

	/* Parameter pcReturned */

/* 5844 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 5846 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 5848 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 5850 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 5852 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 5854 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure Begin_Push */

/* 5856 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 5858 */	NdrFcLong( 0x0 ),	/* 0 */
/* 5862 */	NdrFcShort( 0x5 ),	/* 5 */
/* 5864 */	NdrFcShort( 0x20 ),	/* ia64 Stack size/offset = 32 */
/* 5866 */	NdrFcShort( 0x8 ),	/* 8 */
/* 5868 */	NdrFcShort( 0x8 ),	/* 8 */
/* 5870 */	0x66,		/* Oi2 Flags:  clt must size, has return, has async uuid, has ext, */
			0x3,		/* 3 */
/* 5872 */	0xa,		/* 10 */
			0x5,		/* Ext Flags:  new corr desc, srv corr check, */
/* 5874 */	NdrFcShort( 0x0 ),	/* 0 */
/* 5876 */	NdrFcShort( 0x1 ),	/* 1 */
/* 5878 */	NdrFcShort( 0x0 ),	/* 0 */
/* 5880 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter buf */

/* 5882 */	NdrFcShort( 0x10b ),	/* Flags:  must size, must free, in, simple ref, */
/* 5884 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 5886 */	NdrFcShort( 0x1d0 ),	/* Type Offset=464 */

	/* Parameter cSent */

/* 5888 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 5890 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 5892 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 5894 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 5896 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 5898 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure Pull */

/* 5900 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 5902 */	NdrFcLong( 0x0 ),	/* 0 */
/* 5906 */	NdrFcShort( 0x3 ),	/* 3 */
/* 5908 */	NdrFcShort( 0x28 ),	/* ia64 Stack size/offset = 40 */
/* 5910 */	NdrFcShort( 0x8 ),	/* 8 */
/* 5912 */	NdrFcShort( 0x24 ),	/* 36 */
/* 5914 */	0x45,		/* Oi2 Flags:  srv must size, has return, has ext, */
			0x4,		/* 4 */
/* 5916 */	0xa,		/* 10 */
			0x3,		/* Ext Flags:  new corr desc, clt corr check, */
/* 5918 */	NdrFcShort( 0x1 ),	/* 1 */
/* 5920 */	NdrFcShort( 0x0 ),	/* 0 */
/* 5922 */	NdrFcShort( 0x0 ),	/* 0 */
/* 5924 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter buf */

/* 5926 */	NdrFcShort( 0x113 ),	/* Flags:  must size, must free, out, simple ref, */
/* 5928 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 5930 */	NdrFcShort( 0x5d0 ),	/* Type Offset=1488 */

	/* Parameter cRequest */

/* 5932 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 5934 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 5936 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter pcReturned */

/* 5938 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 5940 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 5942 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 5944 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 5946 */	NdrFcShort( 0x20 ),	/* ia64 Stack size/offset = 32 */
/* 5948 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure Push */

/* 5950 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 5952 */	NdrFcLong( 0x0 ),	/* 0 */
/* 5956 */	NdrFcShort( 0x4 ),	/* 4 */
/* 5958 */	NdrFcShort( 0x20 ),	/* ia64 Stack size/offset = 32 */
/* 5960 */	NdrFcShort( 0x8 ),	/* 8 */
/* 5962 */	NdrFcShort( 0x8 ),	/* 8 */
/* 5964 */	0x46,		/* Oi2 Flags:  clt must size, has return, has ext, */
			0x3,		/* 3 */
/* 5966 */	0xa,		/* 10 */
			0x5,		/* Ext Flags:  new corr desc, srv corr check, */
/* 5968 */	NdrFcShort( 0x0 ),	/* 0 */
/* 5970 */	NdrFcShort( 0x1 ),	/* 1 */
/* 5972 */	NdrFcShort( 0x0 ),	/* 0 */
/* 5974 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter buf */

/* 5976 */	NdrFcShort( 0x10b ),	/* Flags:  must size, must free, in, simple ref, */
/* 5978 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 5980 */	NdrFcShort( 0x5e6 ),	/* Type Offset=1510 */

	/* Parameter cSent */

/* 5982 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 5984 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 5986 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 5988 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 5990 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 5992 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure Finish_Pull */

/* 5994 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 5996 */	NdrFcLong( 0x0 ),	/* 0 */
/* 6000 */	NdrFcShort( 0x4 ),	/* 4 */
/* 6002 */	NdrFcShort( 0x20 ),	/* ia64 Stack size/offset = 32 */
/* 6004 */	NdrFcShort( 0x0 ),	/* 0 */
/* 6006 */	NdrFcShort( 0x24 ),	/* 36 */
/* 6008 */	0x65,		/* Oi2 Flags:  srv must size, has return, has async uuid, has ext, */
			0x3,		/* 3 */
/* 6010 */	0xa,		/* 10 */
			0x3,		/* Ext Flags:  new corr desc, clt corr check, */
/* 6012 */	NdrFcShort( 0x1 ),	/* 1 */
/* 6014 */	NdrFcShort( 0x0 ),	/* 0 */
/* 6016 */	NdrFcShort( 0x0 ),	/* 0 */
/* 6018 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter buf */

/* 6020 */	NdrFcShort( 0x113 ),	/* Flags:  must size, must free, out, simple ref, */
/* 6022 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 6024 */	NdrFcShort( 0x5f6 ),	/* Type Offset=1526 */

	/* Parameter pcReturned */

/* 6026 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 6028 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 6030 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 6032 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 6034 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 6036 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure Begin_Push */

/* 6038 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 6040 */	NdrFcLong( 0x0 ),	/* 0 */
/* 6044 */	NdrFcShort( 0x5 ),	/* 5 */
/* 6046 */	NdrFcShort( 0x20 ),	/* ia64 Stack size/offset = 32 */
/* 6048 */	NdrFcShort( 0x8 ),	/* 8 */
/* 6050 */	NdrFcShort( 0x8 ),	/* 8 */
/* 6052 */	0x66,		/* Oi2 Flags:  clt must size, has return, has async uuid, has ext, */
			0x3,		/* 3 */
/* 6054 */	0xa,		/* 10 */
			0x5,		/* Ext Flags:  new corr desc, srv corr check, */
/* 6056 */	NdrFcShort( 0x0 ),	/* 0 */
/* 6058 */	NdrFcShort( 0x1 ),	/* 1 */
/* 6060 */	NdrFcShort( 0x0 ),	/* 0 */
/* 6062 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter buf */

/* 6064 */	NdrFcShort( 0x10b ),	/* Flags:  must size, must free, in, simple ref, */
/* 6066 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 6068 */	NdrFcShort( 0x5e6 ),	/* Type Offset=1510 */

	/* Parameter cSent */

/* 6070 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 6072 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 6074 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 6076 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 6078 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 6080 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure Pull */

/* 6082 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 6084 */	NdrFcLong( 0x0 ),	/* 0 */
/* 6088 */	NdrFcShort( 0x3 ),	/* 3 */
/* 6090 */	NdrFcShort( 0x28 ),	/* ia64 Stack size/offset = 40 */
/* 6092 */	NdrFcShort( 0x8 ),	/* 8 */
/* 6094 */	NdrFcShort( 0x24 ),	/* 36 */
/* 6096 */	0x45,		/* Oi2 Flags:  srv must size, has return, has ext, */
			0x4,		/* 4 */
/* 6098 */	0xa,		/* 10 */
			0x3,		/* Ext Flags:  new corr desc, clt corr check, */
/* 6100 */	NdrFcShort( 0x1 ),	/* 1 */
/* 6102 */	NdrFcShort( 0x0 ),	/* 0 */
/* 6104 */	NdrFcShort( 0x0 ),	/* 0 */
/* 6106 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter buf */

/* 6108 */	NdrFcShort( 0x113 ),	/* Flags:  must size, must free, out, simple ref, */
/* 6110 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 6112 */	NdrFcShort( 0x60c ),	/* Type Offset=1548 */

	/* Parameter cRequest */

/* 6114 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 6116 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 6118 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter pcReturned */

/* 6120 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 6122 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 6124 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 6126 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 6128 */	NdrFcShort( 0x20 ),	/* ia64 Stack size/offset = 32 */
/* 6130 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure Push */

/* 6132 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 6134 */	NdrFcLong( 0x0 ),	/* 0 */
/* 6138 */	NdrFcShort( 0x4 ),	/* 4 */
/* 6140 */	NdrFcShort( 0x20 ),	/* ia64 Stack size/offset = 32 */
/* 6142 */	NdrFcShort( 0x8 ),	/* 8 */
/* 6144 */	NdrFcShort( 0x8 ),	/* 8 */
/* 6146 */	0x46,		/* Oi2 Flags:  clt must size, has return, has ext, */
			0x3,		/* 3 */
/* 6148 */	0xa,		/* 10 */
			0x5,		/* Ext Flags:  new corr desc, srv corr check, */
/* 6150 */	NdrFcShort( 0x0 ),	/* 0 */
/* 6152 */	NdrFcShort( 0x1 ),	/* 1 */
/* 6154 */	NdrFcShort( 0x0 ),	/* 0 */
/* 6156 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter buf */

/* 6158 */	NdrFcShort( 0x10b ),	/* Flags:  must size, must free, in, simple ref, */
/* 6160 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 6162 */	NdrFcShort( 0x622 ),	/* Type Offset=1570 */

	/* Parameter cSent */

/* 6164 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 6166 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 6168 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 6170 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 6172 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 6174 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure Finish_Pull */

/* 6176 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 6178 */	NdrFcLong( 0x0 ),	/* 0 */
/* 6182 */	NdrFcShort( 0x4 ),	/* 4 */
/* 6184 */	NdrFcShort( 0x20 ),	/* ia64 Stack size/offset = 32 */
/* 6186 */	NdrFcShort( 0x0 ),	/* 0 */
/* 6188 */	NdrFcShort( 0x24 ),	/* 36 */
/* 6190 */	0x65,		/* Oi2 Flags:  srv must size, has return, has async uuid, has ext, */
			0x3,		/* 3 */
/* 6192 */	0xa,		/* 10 */
			0x3,		/* Ext Flags:  new corr desc, clt corr check, */
/* 6194 */	NdrFcShort( 0x1 ),	/* 1 */
/* 6196 */	NdrFcShort( 0x0 ),	/* 0 */
/* 6198 */	NdrFcShort( 0x0 ),	/* 0 */
/* 6200 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter buf */

/* 6202 */	NdrFcShort( 0x113 ),	/* Flags:  must size, must free, out, simple ref, */
/* 6204 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 6206 */	NdrFcShort( 0x632 ),	/* Type Offset=1586 */

	/* Parameter pcReturned */

/* 6208 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 6210 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 6212 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 6214 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 6216 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 6218 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure Begin_Push */

/* 6220 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 6222 */	NdrFcLong( 0x0 ),	/* 0 */
/* 6226 */	NdrFcShort( 0x5 ),	/* 5 */
/* 6228 */	NdrFcShort( 0x20 ),	/* ia64 Stack size/offset = 32 */
/* 6230 */	NdrFcShort( 0x8 ),	/* 8 */
/* 6232 */	NdrFcShort( 0x8 ),	/* 8 */
/* 6234 */	0x66,		/* Oi2 Flags:  clt must size, has return, has async uuid, has ext, */
			0x3,		/* 3 */
/* 6236 */	0xa,		/* 10 */
			0x5,		/* Ext Flags:  new corr desc, srv corr check, */
/* 6238 */	NdrFcShort( 0x0 ),	/* 0 */
/* 6240 */	NdrFcShort( 0x1 ),	/* 1 */
/* 6242 */	NdrFcShort( 0x0 ),	/* 0 */
/* 6244 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter buf */

/* 6246 */	NdrFcShort( 0x10b ),	/* Flags:  must size, must free, in, simple ref, */
/* 6248 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 6250 */	NdrFcShort( 0x622 ),	/* Type Offset=1570 */

	/* Parameter cSent */

/* 6252 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 6254 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 6256 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 6258 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 6260 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 6262 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure ExtractThumbnail */

/* 6264 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 6266 */	NdrFcLong( 0x0 ),	/* 0 */
/* 6270 */	NdrFcShort( 0x3 ),	/* 3 */
/* 6272 */	NdrFcShort( 0x40 ),	/* ia64 Stack size/offset = 64 */
/* 6274 */	NdrFcShort( 0x10 ),	/* 16 */
/* 6276 */	NdrFcShort( 0x40 ),	/* 64 */
/* 6278 */	0x47,		/* Oi2 Flags:  srv must size, clt must size, has return, has ext, */
			0x7,		/* 7 */
/* 6280 */	0xa,		/* 10 */
			0x3,		/* Ext Flags:  new corr desc, clt corr check, */
/* 6282 */	NdrFcShort( 0x2 ),	/* 2 */
/* 6284 */	NdrFcShort( 0x0 ),	/* 0 */
/* 6286 */	NdrFcShort( 0x0 ),	/* 0 */
/* 6288 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pStg */

/* 6290 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 6292 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 6294 */	NdrFcShort( 0x280 ),	/* Type Offset=640 */

	/* Parameter ulLength */

/* 6296 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 6298 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 6300 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter ulHeight */

/* 6302 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 6304 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 6306 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter pulOutputLength */

/* 6308 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 6310 */	NdrFcShort( 0x20 ),	/* ia64 Stack size/offset = 32 */
/* 6312 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter pulOutputHeight */

/* 6314 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 6316 */	NdrFcShort( 0x28 ),	/* ia64 Stack size/offset = 40 */
/* 6318 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter phOutputBitmap */

/* 6320 */	NdrFcShort( 0x2113 ),	/* Flags:  must size, must free, out, simple ref, srv alloc size=8 */
/* 6322 */	NdrFcShort( 0x30 ),	/* ia64 Stack size/offset = 48 */
/* 6324 */	NdrFcShort( 0x64c ),	/* Type Offset=1612 */

	/* Return value */

/* 6326 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 6328 */	NdrFcShort( 0x38 ),	/* ia64 Stack size/offset = 56 */
/* 6330 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure OnFileUpdated */

/* 6332 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 6334 */	NdrFcLong( 0x0 ),	/* 0 */
/* 6338 */	NdrFcShort( 0x4 ),	/* 4 */
/* 6340 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 6342 */	NdrFcShort( 0x0 ),	/* 0 */
/* 6344 */	NdrFcShort( 0x8 ),	/* 8 */
/* 6346 */	0x46,		/* Oi2 Flags:  clt must size, has return, has ext, */
			0x2,		/* 2 */
/* 6348 */	0xa,		/* 10 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 6350 */	NdrFcShort( 0x0 ),	/* 0 */
/* 6352 */	NdrFcShort( 0x0 ),	/* 0 */
/* 6354 */	NdrFcShort( 0x0 ),	/* 0 */
/* 6356 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pStg */

/* 6358 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 6360 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 6362 */	NdrFcShort( 0x280 ),	/* Type Offset=640 */

	/* Return value */

/* 6364 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 6366 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 6368 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure Dummy */

/* 6370 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 6372 */	NdrFcLong( 0x0 ),	/* 0 */
/* 6376 */	NdrFcShort( 0x3 ),	/* 3 */
/* 6378 */	NdrFcShort( 0x20 ),	/* ia64 Stack size/offset = 32 */
/* 6380 */	NdrFcShort( 0x0 ),	/* 0 */
/* 6382 */	NdrFcShort( 0x8 ),	/* 8 */
/* 6384 */	0x46,		/* Oi2 Flags:  clt must size, has return, has ext, */
			0x3,		/* 3 */
/* 6386 */	0xa,		/* 10 */
			0x5,		/* Ext Flags:  new corr desc, srv corr check, */
/* 6388 */	NdrFcShort( 0x0 ),	/* 0 */
/* 6390 */	NdrFcShort( 0x2 ),	/* 2 */
/* 6392 */	NdrFcShort( 0x0 ),	/* 0 */
/* 6394 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter h1 */

/* 6396 */	NdrFcShort( 0x8b ),	/* Flags:  must size, must free, in, by val, */
/* 6398 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 6400 */	NdrFcShort( 0x66e ),	/* Type Offset=1646 */

	/* Parameter h2 */

/* 6402 */	NdrFcShort( 0x8b ),	/* Flags:  must size, must free, in, by val, */
/* 6404 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 6406 */	NdrFcShort( 0x678 ),	/* Type Offset=1656 */

	/* Return value */

/* 6408 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 6410 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 6412 */	0x8,		/* FC_LONG */
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
			0x11, 0x0,	/* FC_RP */
/*  4 */	NdrFcShort( 0x14 ),	/* Offset= 20 (24) */
/*  6 */	
			0x2f,		/* FC_IP */
			0x5a,		/* FC_CONSTANT_IID */
/*  8 */	NdrFcLong( 0x0 ),	/* 0 */
/* 12 */	NdrFcShort( 0x0 ),	/* 0 */
/* 14 */	NdrFcShort( 0x0 ),	/* 0 */
/* 16 */	0xc0,		/* 192 */
			0x0,		/* 0 */
/* 18 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 20 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 22 */	0x0,		/* 0 */
			0x46,		/* 70 */
/* 24 */	
			0x21,		/* FC_BOGUS_ARRAY */
			0x3,		/* 3 */
/* 26 */	NdrFcShort( 0x0 ),	/* 0 */
/* 28 */	0x29,		/* Corr desc:  parameter, FC_ULONG */
			0x0,		/*  */
/* 30 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 32 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 34 */	0x29,		/* Corr desc:  parameter, FC_ULONG */
			0x54,		/* FC_DEREFERENCE */
/* 36 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 38 */	NdrFcShort( 0x0 ),	/* Corr flags:  */
/* 40 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 42 */	NdrFcShort( 0xffffffdc ),	/* Offset= -36 (6) */
/* 44 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 46 */	
			0x11, 0xc,	/* FC_RP [alloced_on_stack] [simple_pointer] */
/* 48 */	0x8,		/* FC_LONG */
			0x5c,		/* FC_PAD */
/* 50 */	
			0x11, 0x10,	/* FC_RP [pointer_deref] */
/* 52 */	NdrFcShort( 0x2 ),	/* Offset= 2 (54) */
/* 54 */	
			0x2f,		/* FC_IP */
			0x5a,		/* FC_CONSTANT_IID */
/* 56 */	NdrFcLong( 0x100 ),	/* 256 */
/* 60 */	NdrFcShort( 0x0 ),	/* 0 */
/* 62 */	NdrFcShort( 0x0 ),	/* 0 */
/* 64 */	0xc0,		/* 192 */
			0x0,		/* 0 */
/* 66 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 68 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 70 */	0x0,		/* 0 */
			0x46,		/* 70 */
/* 72 */	
			0x11, 0x0,	/* FC_RP */
/* 74 */	NdrFcShort( 0x76 ),	/* Offset= 118 (192) */
/* 76 */	
			0x1b,		/* FC_CARRAY */
			0x1,		/* 1 */
/* 78 */	NdrFcShort( 0x2 ),	/* 2 */
/* 80 */	0x19,		/* Corr desc:  field pointer, FC_ULONG */
			0x57,		/* FC_ADD_1 */
/* 82 */	NdrFcShort( 0x8 ),	/* 8 */
/* 84 */	NdrFcShort( 0x0 ),	/* Corr flags:  */
/* 86 */	0x6,		/* FC_SHORT */
			0x5b,		/* FC_END */
/* 88 */	
			0x1b,		/* FC_CARRAY */
			0x1,		/* 1 */
/* 90 */	NdrFcShort( 0x2 ),	/* 2 */
/* 92 */	0x19,		/* Corr desc:  field pointer, FC_ULONG */
			0x57,		/* FC_ADD_1 */
/* 94 */	NdrFcShort( 0x18 ),	/* 24 */
/* 96 */	NdrFcShort( 0x0 ),	/* Corr flags:  */
/* 98 */	0x6,		/* FC_SHORT */
			0x5b,		/* FC_END */
/* 100 */	
			0x1b,		/* FC_CARRAY */
			0x1,		/* 1 */
/* 102 */	NdrFcShort( 0x2 ),	/* 2 */
/* 104 */	0x19,		/* Corr desc:  field pointer, FC_ULONG */
			0x57,		/* FC_ADD_1 */
/* 106 */	NdrFcShort( 0x28 ),	/* 40 */
/* 108 */	NdrFcShort( 0x0 ),	/* Corr flags:  */
/* 110 */	0x6,		/* FC_SHORT */
			0x5b,		/* FC_END */
/* 112 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 114 */	NdrFcShort( 0x30 ),	/* 48 */
/* 116 */	NdrFcShort( 0x0 ),	/* 0 */
/* 118 */	NdrFcShort( 0xc ),	/* Offset= 12 (130) */
/* 120 */	0x36,		/* FC_POINTER */
			0x8,		/* FC_LONG */
/* 122 */	0x40,		/* FC_STRUCTPAD4 */
			0x36,		/* FC_POINTER */
/* 124 */	0x8,		/* FC_LONG */
			0x40,		/* FC_STRUCTPAD4 */
/* 126 */	0x36,		/* FC_POINTER */
			0x8,		/* FC_LONG */
/* 128 */	0x8,		/* FC_LONG */
			0x5b,		/* FC_END */
/* 130 */	
			0x12, 0x0,	/* FC_UP */
/* 132 */	NdrFcShort( 0xffffffc8 ),	/* Offset= -56 (76) */
/* 134 */	
			0x12, 0x0,	/* FC_UP */
/* 136 */	NdrFcShort( 0xffffffd0 ),	/* Offset= -48 (88) */
/* 138 */	
			0x12, 0x0,	/* FC_UP */
/* 140 */	NdrFcShort( 0xffffffd8 ),	/* Offset= -40 (100) */
/* 142 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 144 */	NdrFcShort( 0x28 ),	/* 40 */
/* 146 */	NdrFcShort( 0x0 ),	/* 0 */
/* 148 */	NdrFcShort( 0xc ),	/* Offset= 12 (160) */
/* 150 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 152 */	0x36,		/* FC_POINTER */
			0x8,		/* FC_LONG */
/* 154 */	0x8,		/* FC_LONG */
			0x36,		/* FC_POINTER */
/* 156 */	0x8,		/* FC_LONG */
			0x40,		/* FC_STRUCTPAD4 */
/* 158 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 160 */	
			0x12, 0x8,	/* FC_UP [simple_pointer] */
/* 162 */	
			0x25,		/* FC_C_WSTRING */
			0x5c,		/* FC_PAD */
/* 164 */	
			0x12, 0x0,	/* FC_UP */
/* 166 */	NdrFcShort( 0xffffffca ),	/* Offset= -54 (112) */
/* 168 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 170 */	NdrFcShort( 0x20 ),	/* 32 */
/* 172 */	NdrFcShort( 0x0 ),	/* 0 */
/* 174 */	NdrFcShort( 0xa ),	/* Offset= 10 (184) */
/* 176 */	0x8,		/* FC_LONG */
			0x40,		/* FC_STRUCTPAD4 */
/* 178 */	0x36,		/* FC_POINTER */
			0x36,		/* FC_POINTER */
/* 180 */	0x8,		/* FC_LONG */
			0x40,		/* FC_STRUCTPAD4 */
/* 182 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 184 */	
			0x12, 0x8,	/* FC_UP [simple_pointer] */
/* 186 */	
			0x25,		/* FC_C_WSTRING */
			0x5c,		/* FC_PAD */
/* 188 */	
			0x12, 0x0,	/* FC_UP */
/* 190 */	NdrFcShort( 0xffffffd0 ),	/* Offset= -48 (142) */
/* 192 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 194 */	NdrFcShort( 0x28 ),	/* 40 */
/* 196 */	NdrFcShort( 0x0 ),	/* 0 */
/* 198 */	NdrFcShort( 0xc ),	/* Offset= 12 (210) */
/* 200 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 202 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 204 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 206 */	0x8,		/* FC_LONG */
			0x40,		/* FC_STRUCTPAD4 */
/* 208 */	0x36,		/* FC_POINTER */
			0x5b,		/* FC_END */
/* 210 */	
			0x12, 0x0,	/* FC_UP */
/* 212 */	NdrFcShort( 0xffffffd4 ),	/* Offset= -44 (168) */
/* 214 */	
			0x11, 0x10,	/* FC_RP [pointer_deref] */
/* 216 */	NdrFcShort( 0x2 ),	/* Offset= 2 (218) */
/* 218 */	
			0x2f,		/* FC_IP */
			0x5a,		/* FC_CONSTANT_IID */
/* 220 */	NdrFcLong( 0x10 ),	/* 16 */
/* 224 */	NdrFcShort( 0x0 ),	/* 0 */
/* 226 */	NdrFcShort( 0x0 ),	/* 0 */
/* 228 */	0xc0,		/* 192 */
			0x0,		/* 0 */
/* 230 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 232 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 234 */	0x0,		/* 0 */
			0x46,		/* 70 */
/* 236 */	
			0x11, 0x8,	/* FC_RP [simple_pointer] */
/* 238 */	
			0x25,		/* FC_C_WSTRING */
			0x5c,		/* FC_PAD */
/* 240 */	
			0x11, 0x10,	/* FC_RP [pointer_deref] */
/* 242 */	NdrFcShort( 0xffffff14 ),	/* Offset= -236 (6) */
/* 244 */	
			0x11, 0x10,	/* FC_RP [pointer_deref] */
/* 246 */	NdrFcShort( 0x2 ),	/* Offset= 2 (248) */
/* 248 */	
			0x2f,		/* FC_IP */
			0x5a,		/* FC_CONSTANT_IID */
/* 250 */	NdrFcLong( 0x101 ),	/* 257 */
/* 254 */	NdrFcShort( 0x0 ),	/* 0 */
/* 256 */	NdrFcShort( 0x0 ),	/* 0 */
/* 258 */	0xc0,		/* 192 */
			0x0,		/* 0 */
/* 260 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 262 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 264 */	0x0,		/* 0 */
			0x46,		/* 70 */
/* 266 */	
			0x11, 0x0,	/* FC_RP */
/* 268 */	NdrFcShort( 0x14 ),	/* Offset= 20 (288) */
/* 270 */	
			0x2f,		/* FC_IP */
			0x5a,		/* FC_CONSTANT_IID */
/* 272 */	NdrFcLong( 0xf ),	/* 15 */
/* 276 */	NdrFcShort( 0x0 ),	/* 0 */
/* 278 */	NdrFcShort( 0x0 ),	/* 0 */
/* 280 */	0xc0,		/* 192 */
			0x0,		/* 0 */
/* 282 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 284 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 286 */	0x0,		/* 0 */
			0x46,		/* 70 */
/* 288 */	
			0x21,		/* FC_BOGUS_ARRAY */
			0x3,		/* 3 */
/* 290 */	NdrFcShort( 0x0 ),	/* 0 */
/* 292 */	0x29,		/* Corr desc:  parameter, FC_ULONG */
			0x0,		/*  */
/* 294 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 296 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 298 */	0x29,		/* Corr desc:  parameter, FC_ULONG */
			0x54,		/* FC_DEREFERENCE */
/* 300 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 302 */	NdrFcShort( 0x0 ),	/* Corr flags:  */
/* 304 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 306 */	NdrFcShort( 0xffffffdc ),	/* Offset= -36 (270) */
/* 308 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 310 */	
			0x11, 0x10,	/* FC_RP [pointer_deref] */
/* 312 */	NdrFcShort( 0x2 ),	/* Offset= 2 (314) */
/* 314 */	
			0x2f,		/* FC_IP */
			0x5a,		/* FC_CONSTANT_IID */
/* 316 */	NdrFcLong( 0x102 ),	/* 258 */
/* 320 */	NdrFcShort( 0x0 ),	/* 0 */
/* 322 */	NdrFcShort( 0x0 ),	/* 0 */
/* 324 */	0xc0,		/* 192 */
			0x0,		/* 0 */
/* 326 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 328 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 330 */	0x0,		/* 0 */
			0x46,		/* 70 */
/* 332 */	
			0x11, 0x4,	/* FC_RP [alloced_on_stack] */
/* 334 */	NdrFcShort( 0x8 ),	/* Offset= 8 (342) */
/* 336 */	
			0x1d,		/* FC_SMFARRAY */
			0x0,		/* 0 */
/* 338 */	NdrFcShort( 0x8 ),	/* 8 */
/* 340 */	0x2,		/* FC_CHAR */
			0x5b,		/* FC_END */
/* 342 */	
			0x15,		/* FC_STRUCT */
			0x3,		/* 3 */
/* 344 */	NdrFcShort( 0x10 ),	/* 16 */
/* 346 */	0x8,		/* FC_LONG */
			0x6,		/* FC_SHORT */
/* 348 */	0x6,		/* FC_SHORT */
			0x4c,		/* FC_EMBEDDED_COMPLEX */
/* 350 */	0x0,		/* 0 */
			NdrFcShort( 0xfffffff1 ),	/* Offset= -15 (336) */
			0x5b,		/* FC_END */
/* 354 */	
			0x2f,		/* FC_IP */
			0x5a,		/* FC_CONSTANT_IID */
/* 356 */	NdrFcLong( 0xe ),	/* 14 */
/* 360 */	NdrFcShort( 0x0 ),	/* 0 */
/* 362 */	NdrFcShort( 0x0 ),	/* 0 */
/* 364 */	0xc0,		/* 192 */
			0x0,		/* 0 */
/* 366 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 368 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 370 */	0x0,		/* 0 */
			0x46,		/* 70 */
/* 372 */	
			0x11, 0x0,	/* FC_RP */
/* 374 */	NdrFcShort( 0x2 ),	/* Offset= 2 (376) */
/* 376 */	
			0x15,		/* FC_STRUCT */
			0x3,		/* 3 */
/* 378 */	NdrFcShort( 0x8 ),	/* 8 */
/* 380 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 382 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 384 */	
			0x11, 0x4,	/* FC_RP [alloced_on_stack] */
/* 386 */	NdrFcShort( 0xfffffff6 ),	/* Offset= -10 (376) */
/* 388 */	
			0x2f,		/* FC_IP */
			0x5a,		/* FC_CONSTANT_IID */
/* 390 */	NdrFcLong( 0xc ),	/* 12 */
/* 394 */	NdrFcShort( 0x0 ),	/* 0 */
/* 396 */	NdrFcShort( 0x0 ),	/* 0 */
/* 398 */	0xc0,		/* 192 */
			0x0,		/* 0 */
/* 400 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 402 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 404 */	0x0,		/* 0 */
			0x46,		/* 70 */
/* 406 */	
			0x11, 0x4,	/* FC_RP [alloced_on_stack] */
/* 408 */	NdrFcShort( 0x2 ),	/* Offset= 2 (410) */
/* 410 */	
			0x15,		/* FC_STRUCT */
			0x7,		/* 7 */
/* 412 */	NdrFcShort( 0x8 ),	/* 8 */
/* 414 */	0xb,		/* FC_HYPER */
			0x5b,		/* FC_END */
/* 416 */	
			0x11, 0x0,	/* FC_RP */
/* 418 */	NdrFcShort( 0xffffffb4 ),	/* Offset= -76 (342) */
/* 420 */	
			0x11, 0x10,	/* FC_RP [pointer_deref] */
/* 422 */	NdrFcShort( 0x2 ),	/* Offset= 2 (424) */
/* 424 */	
			0x2f,		/* FC_IP */
			0x5c,		/* FC_PAD */
/* 426 */	0x2b,		/* Corr desc:  parameter, FC_HYPER */
			0x0,		/*  */
/* 428 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 430 */	NdrFcShort( 0x5 ),	/* Corr flags:  early, iid_is, */
/* 432 */	
			0x11, 0x10,	/* FC_RP [pointer_deref] */
/* 434 */	NdrFcShort( 0x2 ),	/* Offset= 2 (436) */
/* 436 */	
			0x2f,		/* FC_IP */
			0x5c,		/* FC_PAD */
/* 438 */	0x2b,		/* Corr desc:  parameter, FC_HYPER */
			0x0,		/*  */
/* 440 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 442 */	NdrFcShort( 0x5 ),	/* Corr flags:  early, iid_is, */
/* 444 */	
			0x12, 0x14,	/* FC_UP [alloced_on_stack] [pointer_deref] */
/* 446 */	NdrFcShort( 0xffffff50 ),	/* Offset= -176 (270) */
/* 448 */	
			0x11, 0x10,	/* FC_RP [pointer_deref] */
/* 450 */	NdrFcShort( 0xffffff4c ),	/* Offset= -180 (270) */
/* 452 */	
			0x11, 0x14,	/* FC_RP [alloced_on_stack] [pointer_deref] */
/* 454 */	NdrFcShort( 0x2 ),	/* Offset= 2 (456) */
/* 456 */	
			0x13, 0x8,	/* FC_OP [simple_pointer] */
/* 458 */	
			0x25,		/* FC_C_WSTRING */
			0x5c,		/* FC_PAD */
/* 460 */	
			0x11, 0x0,	/* FC_RP */
/* 462 */	NdrFcShort( 0x2 ),	/* Offset= 2 (464) */
/* 464 */	
			0x1b,		/* FC_CARRAY */
			0x0,		/* 0 */
/* 466 */	NdrFcShort( 0x1 ),	/* 1 */
/* 468 */	0x29,		/* Corr desc:  parameter, FC_ULONG */
			0x0,		/*  */
/* 470 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 472 */	NdrFcShort( 0x0 ),	/* Corr flags:  */
/* 474 */	0x1,		/* FC_BYTE */
			0x5b,		/* FC_END */
/* 476 */	
			0x11, 0x0,	/* FC_RP */
/* 478 */	NdrFcShort( 0x2 ),	/* Offset= 2 (480) */
/* 480 */	
			0x21,		/* FC_BOGUS_ARRAY */
			0x3,		/* 3 */
/* 482 */	NdrFcShort( 0x0 ),	/* 0 */
/* 484 */	0x29,		/* Corr desc:  parameter, FC_ULONG */
			0x0,		/*  */
/* 486 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 488 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 490 */	0x29,		/* Corr desc:  parameter, FC_ULONG */
			0x54,		/* FC_DEREFERENCE */
/* 492 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 494 */	NdrFcShort( 0x0 ),	/* Corr flags:  */
/* 496 */	
			0x13, 0x8,	/* FC_OP [simple_pointer] */
/* 498 */	
			0x25,		/* FC_C_WSTRING */
			0x5c,		/* FC_PAD */
/* 500 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 502 */	
			0x11, 0x0,	/* FC_RP */
/* 504 */	NdrFcShort( 0x2 ),	/* Offset= 2 (506) */
/* 506 */	
			0x1c,		/* FC_CVARRAY */
			0x0,		/* 0 */
/* 508 */	NdrFcShort( 0x1 ),	/* 1 */
/* 510 */	0x29,		/* Corr desc:  parameter, FC_ULONG */
			0x0,		/*  */
/* 512 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 514 */	NdrFcShort( 0x0 ),	/* Corr flags:  */
/* 516 */	0x29,		/* Corr desc:  parameter, FC_ULONG */
			0x54,		/* FC_DEREFERENCE */
/* 518 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 520 */	NdrFcShort( 0x0 ),	/* Corr flags:  */
/* 522 */	0x1,		/* FC_BYTE */
			0x5b,		/* FC_END */
/* 524 */	
			0x11, 0x0,	/* FC_RP */
/* 526 */	NdrFcShort( 0x2 ),	/* Offset= 2 (528) */
/* 528 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x7,		/* 7 */
/* 530 */	NdrFcShort( 0x50 ),	/* 80 */
/* 532 */	NdrFcShort( 0x0 ),	/* 0 */
/* 534 */	NdrFcShort( 0x1e ),	/* Offset= 30 (564) */
/* 536 */	0x36,		/* FC_POINTER */
			0x8,		/* FC_LONG */
/* 538 */	0x40,		/* FC_STRUCTPAD4 */
			0x4c,		/* FC_EMBEDDED_COMPLEX */
/* 540 */	0x0,		/* 0 */
			NdrFcShort( 0xffffff7d ),	/* Offset= -131 (410) */
			0x4c,		/* FC_EMBEDDED_COMPLEX */
/* 544 */	0x0,		/* 0 */
			NdrFcShort( 0xffffff57 ),	/* Offset= -169 (376) */
			0x4c,		/* FC_EMBEDDED_COMPLEX */
/* 548 */	0x0,		/* 0 */
			NdrFcShort( 0xffffff53 ),	/* Offset= -173 (376) */
			0x4c,		/* FC_EMBEDDED_COMPLEX */
/* 552 */	0x0,		/* 0 */
			NdrFcShort( 0xffffff4f ),	/* Offset= -177 (376) */
			0x8,		/* FC_LONG */
/* 556 */	0x8,		/* FC_LONG */
			0x4c,		/* FC_EMBEDDED_COMPLEX */
/* 558 */	0x0,		/* 0 */
			NdrFcShort( 0xffffff27 ),	/* Offset= -217 (342) */
			0x8,		/* FC_LONG */
/* 562 */	0x8,		/* FC_LONG */
			0x5b,		/* FC_END */
/* 564 */	
			0x13, 0x8,	/* FC_OP [simple_pointer] */
/* 566 */	
			0x25,		/* FC_C_WSTRING */
			0x5c,		/* FC_PAD */
/* 568 */	
			0x11, 0x10,	/* FC_RP [pointer_deref] */
/* 570 */	NdrFcShort( 0xffffff4a ),	/* Offset= -182 (388) */
/* 572 */	
			0x11, 0x0,	/* FC_RP */
/* 574 */	NdrFcShort( 0x2 ),	/* Offset= 2 (576) */
/* 576 */	
			0x21,		/* FC_BOGUS_ARRAY */
			0x7,		/* 7 */
/* 578 */	NdrFcShort( 0x0 ),	/* 0 */
/* 580 */	0x29,		/* Corr desc:  parameter, FC_ULONG */
			0x0,		/*  */
/* 582 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 584 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 586 */	0x29,		/* Corr desc:  parameter, FC_ULONG */
			0x54,		/* FC_DEREFERENCE */
/* 588 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 590 */	NdrFcShort( 0x0 ),	/* Corr flags:  */
/* 592 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 594 */	NdrFcShort( 0xffffffbe ),	/* Offset= -66 (528) */
/* 596 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 598 */	
			0x11, 0x10,	/* FC_RP [pointer_deref] */
/* 600 */	NdrFcShort( 0x2 ),	/* Offset= 2 (602) */
/* 602 */	
			0x2f,		/* FC_IP */
			0x5a,		/* FC_CONSTANT_IID */
/* 604 */	NdrFcLong( 0xd ),	/* 13 */
/* 608 */	NdrFcShort( 0x0 ),	/* 0 */
/* 610 */	NdrFcShort( 0x0 ),	/* 0 */
/* 612 */	0xc0,		/* 192 */
			0x0,		/* 0 */
/* 614 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 616 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 618 */	0x0,		/* 0 */
			0x46,		/* 70 */
/* 620 */	
			0x12, 0x0,	/* FC_UP */
/* 622 */	NdrFcShort( 0x2 ),	/* Offset= 2 (624) */
/* 624 */	
			0x1b,		/* FC_CARRAY */
			0x0,		/* 0 */
/* 626 */	NdrFcShort( 0x1 ),	/* 1 */
/* 628 */	0x29,		/* Corr desc:  parameter, FC_ULONG */
			0x0,		/*  */
/* 630 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 632 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 634 */	0x1,		/* FC_BYTE */
			0x5b,		/* FC_END */
/* 636 */	
			0x11, 0x10,	/* FC_RP [pointer_deref] */
/* 638 */	NdrFcShort( 0x2 ),	/* Offset= 2 (640) */
/* 640 */	
			0x2f,		/* FC_IP */
			0x5a,		/* FC_CONSTANT_IID */
/* 642 */	NdrFcLong( 0xb ),	/* 11 */
/* 646 */	NdrFcShort( 0x0 ),	/* 0 */
/* 648 */	NdrFcShort( 0x0 ),	/* 0 */
/* 650 */	0xc0,		/* 192 */
			0x0,		/* 0 */
/* 652 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 654 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 656 */	0x0,		/* 0 */
			0x46,		/* 70 */
/* 658 */	
			0x12, 0x8,	/* FC_UP [simple_pointer] */
/* 660 */	
			0x25,		/* FC_C_WSTRING */
			0x5c,		/* FC_PAD */
/* 662 */	
			0x12, 0x0,	/* FC_UP */
/* 664 */	NdrFcShort( 0xe ),	/* Offset= 14 (678) */
/* 666 */	
			0x1b,		/* FC_CARRAY */
			0x1,		/* 1 */
/* 668 */	NdrFcShort( 0x2 ),	/* 2 */
/* 670 */	0x9,		/* Corr desc: FC_ULONG */
			0x0,		/*  */
/* 672 */	NdrFcShort( 0xfffc ),	/* -4 */
/* 674 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 676 */	0x5,		/* FC_WCHAR */
			0x5b,		/* FC_END */
/* 678 */	
			0x17,		/* FC_CSTRUCT */
			0x3,		/* 3 */
/* 680 */	NdrFcShort( 0x8 ),	/* 8 */
/* 682 */	NdrFcShort( 0xfffffff0 ),	/* Offset= -16 (666) */
/* 684 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 686 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 688 */	0xb4,		/* FC_USER_MARSHAL */
			0x83,		/* 131 */
/* 690 */	NdrFcShort( 0x0 ),	/* 0 */
/* 692 */	NdrFcShort( 0x8 ),	/* 8 */
/* 694 */	NdrFcShort( 0x0 ),	/* 0 */
/* 696 */	NdrFcShort( 0xffffffde ),	/* Offset= -34 (662) */
/* 698 */	
			0x12, 0x0,	/* FC_UP */
/* 700 */	NdrFcShort( 0x2 ),	/* Offset= 2 (702) */
/* 702 */	
			0x1b,		/* FC_CARRAY */
			0x3,		/* 3 */
/* 704 */	NdrFcShort( 0x10 ),	/* 16 */
/* 706 */	0x29,		/* Corr desc:  parameter, FC_ULONG */
			0x0,		/*  */
/* 708 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 710 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 712 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 714 */	NdrFcShort( 0xfffffe8c ),	/* Offset= -372 (342) */
/* 716 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 718 */	
			0x12, 0x0,	/* FC_UP */
/* 720 */	NdrFcShort( 0xfffffea8 ),	/* Offset= -344 (376) */
/* 722 */	
			0x11, 0x0,	/* FC_RP */
/* 724 */	NdrFcShort( 0x2 ),	/* Offset= 2 (726) */
/* 726 */	
			0x1c,		/* FC_CVARRAY */
			0x0,		/* 0 */
/* 728 */	NdrFcShort( 0x1 ),	/* 1 */
/* 730 */	0x29,		/* Corr desc:  parameter, FC_ULONG */
			0x0,		/*  */
/* 732 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 734 */	NdrFcShort( 0x0 ),	/* Corr flags:  */
/* 736 */	0x29,		/* Corr desc:  parameter, FC_ULONG */
			0x54,		/* FC_DEREFERENCE */
/* 738 */	NdrFcShort( 0x20 ),	/* ia64 Stack size/offset = 32 */
/* 740 */	NdrFcShort( 0x0 ),	/* Corr flags:  */
/* 742 */	0x1,		/* FC_BYTE */
			0x5b,		/* FC_END */
/* 744 */	
			0x11, 0x0,	/* FC_RP */
/* 746 */	NdrFcShort( 0x2 ),	/* Offset= 2 (748) */
/* 748 */	
			0x1b,		/* FC_CARRAY */
			0x0,		/* 0 */
/* 750 */	NdrFcShort( 0x1 ),	/* 1 */
/* 752 */	0x29,		/* Corr desc:  parameter, FC_ULONG */
			0x0,		/*  */
/* 754 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 756 */	NdrFcShort( 0x0 ),	/* Corr flags:  */
/* 758 */	0x1,		/* FC_BYTE */
			0x5b,		/* FC_END */
/* 760 */	
			0x11, 0x0,	/* FC_RP */
/* 762 */	NdrFcShort( 0x54 ),	/* Offset= 84 (846) */
/* 764 */	
			0x13, 0x0,	/* FC_OP */
/* 766 */	NdrFcShort( 0x2 ),	/* Offset= 2 (768) */
/* 768 */	
			0x2a,		/* FC_ENCAPSULATED_UNION */
			0x88,		/* 136 */
/* 770 */	NdrFcShort( 0x8 ),	/* 8 */
/* 772 */	NdrFcShort( 0x2 ),	/* 2 */
/* 774 */	NdrFcLong( 0x48746457 ),	/* 1215587415 */
/* 778 */	NdrFcShort( 0x8008 ),	/* Simple arm type: FC_LONG */
/* 780 */	NdrFcLong( 0x52746457 ),	/* 1383359575 */
/* 784 */	NdrFcShort( 0xfffffeb8 ),	/* Offset= -328 (456) */
/* 786 */	NdrFcShort( 0xffffffff ),	/* Offset= -1 (785) */
/* 788 */	0xb4,		/* FC_USER_MARSHAL */
			0x83,		/* 131 */
/* 790 */	NdrFcShort( 0x1 ),	/* 1 */
/* 792 */	NdrFcShort( 0x2 ),	/* 2 */
/* 794 */	NdrFcShort( 0x0 ),	/* 0 */
/* 796 */	NdrFcShort( 0xffffffe0 ),	/* Offset= -32 (764) */
/* 798 */	
			0x1b,		/* FC_CARRAY */
			0x0,		/* 0 */
/* 800 */	NdrFcShort( 0x1 ),	/* 1 */
/* 802 */	0x0,		/* Corr desc:  */
			0x59,		/* FC_CALLBACK */
/* 804 */	NdrFcShort( 0x0 ),	/* 0 */
/* 806 */	NdrFcShort( 0x0 ),	/* Corr flags:  */
/* 808 */	0x1,		/* FC_BYTE */
			0x5b,		/* FC_END */
/* 810 */	
			0x17,		/* FC_CSTRUCT */
			0x3,		/* 3 */
/* 812 */	NdrFcShort( 0xc ),	/* 12 */
/* 814 */	NdrFcShort( 0xfffffff0 ),	/* Offset= -16 (798) */
/* 816 */	0x8,		/* FC_LONG */
			0x6,		/* FC_SHORT */
/* 818 */	0x6,		/* FC_SHORT */
			0x6,		/* FC_SHORT */
/* 820 */	0x6,		/* FC_SHORT */
			0x5b,		/* FC_END */
/* 822 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 824 */	NdrFcShort( 0x20 ),	/* 32 */
/* 826 */	NdrFcShort( 0x0 ),	/* 0 */
/* 828 */	NdrFcShort( 0xe ),	/* Offset= 14 (842) */
/* 830 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 832 */	NdrFcShort( 0xffffffd4 ),	/* Offset= -44 (788) */
/* 834 */	0x42,		/* FC_STRUCTPAD6 */
			0x36,		/* FC_POINTER */
/* 836 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 838 */	0x8,		/* FC_LONG */
			0x40,		/* FC_STRUCTPAD4 */
/* 840 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 842 */	
			0x13, 0x0,	/* FC_OP */
/* 844 */	NdrFcShort( 0xffffffde ),	/* Offset= -34 (810) */
/* 846 */	
			0x21,		/* FC_BOGUS_ARRAY */
			0x3,		/* 3 */
/* 848 */	NdrFcShort( 0x0 ),	/* 0 */
/* 850 */	0x29,		/* Corr desc:  parameter, FC_ULONG */
			0x0,		/*  */
/* 852 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 854 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 856 */	0x29,		/* Corr desc:  parameter, FC_ULONG */
			0x54,		/* FC_DEREFERENCE */
/* 858 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 860 */	NdrFcShort( 0x0 ),	/* Corr flags:  */
/* 862 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 864 */	NdrFcShort( 0xffffffd6 ),	/* Offset= -42 (822) */
/* 866 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 868 */	
			0x11, 0x10,	/* FC_RP [pointer_deref] */
/* 870 */	NdrFcShort( 0x2 ),	/* Offset= 2 (872) */
/* 872 */	
			0x2f,		/* FC_IP */
			0x5a,		/* FC_CONSTANT_IID */
/* 874 */	NdrFcLong( 0x103 ),	/* 259 */
/* 878 */	NdrFcShort( 0x0 ),	/* 0 */
/* 880 */	NdrFcShort( 0x0 ),	/* 0 */
/* 882 */	0xc0,		/* 192 */
			0x0,		/* 0 */
/* 884 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 886 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 888 */	0x0,		/* 0 */
			0x46,		/* 70 */
/* 890 */	
			0x11, 0x0,	/* FC_RP */
/* 892 */	NdrFcShort( 0x2a ),	/* Offset= 42 (934) */
/* 894 */	
			0x2f,		/* FC_IP */
			0x5a,		/* FC_CONSTANT_IID */
/* 896 */	NdrFcLong( 0x10f ),	/* 271 */
/* 900 */	NdrFcShort( 0x0 ),	/* 0 */
/* 902 */	NdrFcShort( 0x0 ),	/* 0 */
/* 904 */	0xc0,		/* 192 */
			0x0,		/* 0 */
/* 906 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 908 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 910 */	0x0,		/* 0 */
			0x46,		/* 70 */
/* 912 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 914 */	NdrFcShort( 0x38 ),	/* 56 */
/* 916 */	NdrFcShort( 0x0 ),	/* 0 */
/* 918 */	NdrFcShort( 0x10 ),	/* Offset= 16 (934) */
/* 920 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 922 */	NdrFcShort( 0xffffff9c ),	/* Offset= -100 (822) */
/* 924 */	0x8,		/* FC_LONG */
			0x40,		/* FC_STRUCTPAD4 */
/* 926 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 928 */	NdrFcShort( 0xffffffde ),	/* Offset= -34 (894) */
/* 930 */	0x8,		/* FC_LONG */
			0x40,		/* FC_STRUCTPAD4 */
/* 932 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 934 */	
			0x21,		/* FC_BOGUS_ARRAY */
			0x3,		/* 3 */
/* 936 */	NdrFcShort( 0x0 ),	/* 0 */
/* 938 */	0x29,		/* Corr desc:  parameter, FC_ULONG */
			0x0,		/*  */
/* 940 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 942 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 944 */	0x29,		/* Corr desc:  parameter, FC_ULONG */
			0x54,		/* FC_DEREFERENCE */
/* 946 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 948 */	NdrFcShort( 0x0 ),	/* Corr flags:  */
/* 950 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 952 */	NdrFcShort( 0xffffffd8 ),	/* Offset= -40 (912) */
/* 954 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 956 */	
			0x11, 0x10,	/* FC_RP [pointer_deref] */
/* 958 */	NdrFcShort( 0x2 ),	/* Offset= 2 (960) */
/* 960 */	
			0x2f,		/* FC_IP */
			0x5a,		/* FC_CONSTANT_IID */
/* 962 */	NdrFcLong( 0x105 ),	/* 261 */
/* 966 */	NdrFcShort( 0x0 ),	/* 0 */
/* 968 */	NdrFcShort( 0x0 ),	/* 0 */
/* 970 */	0xc0,		/* 192 */
			0x0,		/* 0 */
/* 972 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 974 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 976 */	0x0,		/* 0 */
			0x46,		/* 70 */
/* 978 */	
			0x12, 0x0,	/* FC_UP */
/* 980 */	NdrFcShort( 0xffffff62 ),	/* Offset= -158 (822) */
/* 982 */	
			0x12, 0x0,	/* FC_UP */
/* 984 */	NdrFcShort( 0x170 ),	/* Offset= 368 (1352) */
/* 986 */	
			0x12, 0x0,	/* FC_UP */
/* 988 */	NdrFcShort( 0x15a ),	/* Offset= 346 (1334) */
/* 990 */	
			0x2a,		/* FC_ENCAPSULATED_UNION */
			0x89,		/* 137 */
/* 992 */	NdrFcShort( 0x8 ),	/* 8 */
/* 994 */	NdrFcShort( 0x8 ),	/* 8 */
/* 996 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1000 */	NdrFcShort( 0x0 ),	/* Offset= 0 (1000) */
/* 1002 */	NdrFcLong( 0x20 ),	/* 32 */
/* 1006 */	NdrFcShort( 0x28 ),	/* Offset= 40 (1046) */
/* 1008 */	NdrFcLong( 0x40 ),	/* 64 */
/* 1012 */	NdrFcShort( 0x7c ),	/* Offset= 124 (1136) */
/* 1014 */	NdrFcLong( 0x10 ),	/* 16 */
/* 1018 */	NdrFcShort( 0x8e ),	/* Offset= 142 (1160) */
/* 1020 */	NdrFcLong( 0x1 ),	/* 1 */
/* 1024 */	NdrFcShort( 0x10a ),	/* Offset= 266 (1290) */
/* 1026 */	NdrFcLong( 0x2 ),	/* 2 */
/* 1030 */	NdrFcShort( 0xfffffe8c ),	/* Offset= -372 (658) */
/* 1032 */	NdrFcLong( 0x4 ),	/* 4 */
/* 1036 */	NdrFcShort( 0x3a ),	/* Offset= 58 (1094) */
/* 1038 */	NdrFcLong( 0x8 ),	/* 8 */
/* 1042 */	NdrFcShort( 0x34 ),	/* Offset= 52 (1094) */
/* 1044 */	NdrFcShort( 0xffffffff ),	/* Offset= -1 (1043) */
/* 1046 */	
			0x12, 0x0,	/* FC_UP */
/* 1048 */	NdrFcShort( 0x2 ),	/* Offset= 2 (1050) */
/* 1050 */	
			0x2a,		/* FC_ENCAPSULATED_UNION */
			0x88,		/* 136 */
/* 1052 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1054 */	NdrFcShort( 0x2 ),	/* 2 */
/* 1056 */	NdrFcLong( 0x48746457 ),	/* 1215587415 */
/* 1060 */	NdrFcShort( 0x8008 ),	/* Simple arm type: FC_LONG */
/* 1062 */	NdrFcLong( 0x52746457 ),	/* 1383359575 */
/* 1066 */	NdrFcShort( 0x4 ),	/* Offset= 4 (1070) */
/* 1068 */	NdrFcShort( 0xffffffff ),	/* Offset= -1 (1067) */
/* 1070 */	
			0x12, 0x0,	/* FC_UP */
/* 1072 */	NdrFcShort( 0x2e ),	/* Offset= 46 (1118) */
/* 1074 */	
			0x2a,		/* FC_ENCAPSULATED_UNION */
			0x88,		/* 136 */
/* 1076 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1078 */	NdrFcShort( 0x2 ),	/* 2 */
/* 1080 */	NdrFcLong( 0x48746457 ),	/* 1215587415 */
/* 1084 */	NdrFcShort( 0x8008 ),	/* Simple arm type: FC_LONG */
/* 1086 */	NdrFcLong( 0x52746457 ),	/* 1383359575 */
/* 1090 */	NdrFcShort( 0x4 ),	/* Offset= 4 (1094) */
/* 1092 */	NdrFcShort( 0xffffffff ),	/* Offset= -1 (1091) */
/* 1094 */	
			0x12, 0x0,	/* FC_UP */
/* 1096 */	NdrFcShort( 0xe ),	/* Offset= 14 (1110) */
/* 1098 */	
			0x1b,		/* FC_CARRAY */
			0x0,		/* 0 */
/* 1100 */	NdrFcShort( 0x1 ),	/* 1 */
/* 1102 */	0x9,		/* Corr desc: FC_ULONG */
			0x0,		/*  */
/* 1104 */	NdrFcShort( 0xfffc ),	/* -4 */
/* 1106 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 1108 */	0x1,		/* FC_BYTE */
			0x5b,		/* FC_END */
/* 1110 */	
			0x17,		/* FC_CSTRUCT */
			0x3,		/* 3 */
/* 1112 */	NdrFcShort( 0x4 ),	/* 4 */
/* 1114 */	NdrFcShort( 0xfffffff0 ),	/* Offset= -16 (1098) */
/* 1116 */	0x8,		/* FC_LONG */
			0x5b,		/* FC_END */
/* 1118 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 1120 */	NdrFcShort( 0x18 ),	/* 24 */
/* 1122 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1124 */	NdrFcShort( 0x8 ),	/* Offset= 8 (1132) */
/* 1126 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 1128 */	0x8,		/* FC_LONG */
			0x40,		/* FC_STRUCTPAD4 */
/* 1130 */	0x36,		/* FC_POINTER */
			0x5b,		/* FC_END */
/* 1132 */	
			0x12, 0x0,	/* FC_UP */
/* 1134 */	NdrFcShort( 0xffffffc4 ),	/* Offset= -60 (1074) */
/* 1136 */	
			0x12, 0x0,	/* FC_UP */
/* 1138 */	NdrFcShort( 0x2 ),	/* Offset= 2 (1140) */
/* 1140 */	
			0x2a,		/* FC_ENCAPSULATED_UNION */
			0x88,		/* 136 */
/* 1142 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1144 */	NdrFcShort( 0x2 ),	/* 2 */
/* 1146 */	NdrFcLong( 0x48746457 ),	/* 1215587415 */
/* 1150 */	NdrFcShort( 0x8008 ),	/* Simple arm type: FC_LONG */
/* 1152 */	NdrFcLong( 0x52746457 ),	/* 1383359575 */
/* 1156 */	NdrFcShort( 0xffffffc2 ),	/* Offset= -62 (1094) */
/* 1158 */	NdrFcShort( 0xffffffff ),	/* Offset= -1 (1157) */
/* 1160 */	
			0x12, 0x0,	/* FC_UP */
/* 1162 */	NdrFcShort( 0x2 ),	/* Offset= 2 (1164) */
/* 1164 */	
			0x2a,		/* FC_ENCAPSULATED_UNION */
			0x89,		/* 137 */
/* 1166 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1168 */	NdrFcShort( 0x2 ),	/* 2 */
/* 1170 */	NdrFcLong( 0x7 ),	/* 7 */
/* 1174 */	NdrFcShort( 0xa ),	/* Offset= 10 (1184) */
/* 1176 */	NdrFcLong( 0x5 ),	/* 5 */
/* 1180 */	NdrFcShort( 0x2e ),	/* Offset= 46 (1226) */
/* 1182 */	NdrFcShort( 0x6c ),	/* Offset= 108 (1290) */
/* 1184 */	
			0x12, 0x0,	/* FC_UP */
/* 1186 */	NdrFcShort( 0x2 ),	/* Offset= 2 (1188) */
/* 1188 */	
			0x2a,		/* FC_ENCAPSULATED_UNION */
			0x88,		/* 136 */
/* 1190 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1192 */	NdrFcShort( 0x2 ),	/* 2 */
/* 1194 */	NdrFcLong( 0x48746457 ),	/* 1215587415 */
/* 1198 */	NdrFcShort( 0x8008 ),	/* Simple arm type: FC_LONG */
/* 1200 */	NdrFcLong( 0x52746457 ),	/* 1383359575 */
/* 1204 */	NdrFcShort( 0x4 ),	/* Offset= 4 (1208) */
/* 1206 */	NdrFcShort( 0xffffffff ),	/* Offset= -1 (1205) */
/* 1208 */	
			0x12, 0x0,	/* FC_UP */
/* 1210 */	NdrFcShort( 0x2 ),	/* Offset= 2 (1212) */
/* 1212 */	
			0x17,		/* FC_CSTRUCT */
			0x3,		/* 3 */
/* 1214 */	NdrFcShort( 0x18 ),	/* 24 */
/* 1216 */	NdrFcShort( 0xffffff8a ),	/* Offset= -118 (1098) */
/* 1218 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 1220 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 1222 */	0x6,		/* FC_SHORT */
			0x6,		/* FC_SHORT */
/* 1224 */	0x8,		/* FC_LONG */
			0x5b,		/* FC_END */
/* 1226 */	
			0x12, 0x0,	/* FC_UP */
/* 1228 */	NdrFcShort( 0x2 ),	/* Offset= 2 (1230) */
/* 1230 */	
			0x2a,		/* FC_ENCAPSULATED_UNION */
			0x88,		/* 136 */
/* 1232 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1234 */	NdrFcShort( 0x2 ),	/* 2 */
/* 1236 */	NdrFcLong( 0x48746457 ),	/* 1215587415 */
/* 1240 */	NdrFcShort( 0x8008 ),	/* Simple arm type: FC_LONG */
/* 1242 */	NdrFcLong( 0x52746457 ),	/* 1383359575 */
/* 1246 */	NdrFcShort( 0x4 ),	/* Offset= 4 (1250) */
/* 1248 */	NdrFcShort( 0xffffffff ),	/* Offset= -1 (1247) */
/* 1250 */	
			0x12, 0x0,	/* FC_UP */
/* 1252 */	NdrFcShort( 0x1c ),	/* Offset= 28 (1280) */
/* 1254 */	
			0x15,		/* FC_STRUCT */
			0x0,		/* 0 */
/* 1256 */	NdrFcShort( 0x4 ),	/* 4 */
/* 1258 */	0x1,		/* FC_BYTE */
			0x1,		/* FC_BYTE */
/* 1260 */	0x1,		/* FC_BYTE */
			0x1,		/* FC_BYTE */
/* 1262 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 1264 */	
			0x1b,		/* FC_CARRAY */
			0x0,		/* 0 */
/* 1266 */	NdrFcShort( 0x4 ),	/* 4 */
/* 1268 */	0x7,		/* Corr desc: FC_USHORT */
			0x0,		/*  */
/* 1270 */	NdrFcShort( 0xfffe ),	/* -2 */
/* 1272 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 1274 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 1276 */	NdrFcShort( 0xffffffea ),	/* Offset= -22 (1254) */
/* 1278 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 1280 */	
			0x17,		/* FC_CSTRUCT */
			0x1,		/* 1 */
/* 1282 */	NdrFcShort( 0x4 ),	/* 4 */
/* 1284 */	NdrFcShort( 0xffffffec ),	/* Offset= -20 (1264) */
/* 1286 */	0x6,		/* FC_SHORT */
			0x6,		/* FC_SHORT */
/* 1288 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 1290 */	
			0x12, 0x0,	/* FC_UP */
/* 1292 */	NdrFcShort( 0x2 ),	/* Offset= 2 (1294) */
/* 1294 */	
			0x2a,		/* FC_ENCAPSULATED_UNION */
			0x88,		/* 136 */
/* 1296 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1298 */	NdrFcShort( 0x3 ),	/* 3 */
/* 1300 */	NdrFcLong( 0x48746457 ),	/* 1215587415 */
/* 1304 */	NdrFcShort( 0x8008 ),	/* Simple arm type: FC_LONG */
/* 1306 */	NdrFcLong( 0x52746457 ),	/* 1383359575 */
/* 1310 */	NdrFcShort( 0xa ),	/* Offset= 10 (1320) */
/* 1312 */	NdrFcLong( 0x50746457 ),	/* 1349805143 */
/* 1316 */	NdrFcShort( 0x800b ),	/* Simple arm type: FC_HYPER */
/* 1318 */	NdrFcShort( 0xffffffff ),	/* Offset= -1 (1317) */
/* 1320 */	
			0x12, 0x0,	/* FC_UP */
/* 1322 */	NdrFcShort( 0x2 ),	/* Offset= 2 (1324) */
/* 1324 */	
			0x17,		/* FC_CSTRUCT */
			0x3,		/* 3 */
/* 1326 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1328 */	NdrFcShort( 0xffffff1a ),	/* Offset= -230 (1098) */
/* 1330 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 1332 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 1334 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 1336 */	NdrFcShort( 0x18 ),	/* 24 */
/* 1338 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1340 */	NdrFcShort( 0xc ),	/* Offset= 12 (1352) */
/* 1342 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 1344 */	NdrFcShort( 0xfffffe9e ),	/* Offset= -354 (990) */
/* 1346 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 1348 */	NdrFcShort( 0xfffffac2 ),	/* Offset= -1342 (6) */
/* 1350 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 1352 */	0xb4,		/* FC_USER_MARSHAL */
			0x83,		/* 131 */
/* 1354 */	NdrFcShort( 0x2 ),	/* 2 */
/* 1356 */	NdrFcShort( 0x18 ),	/* 24 */
/* 1358 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1360 */	NdrFcShort( 0xfffffe8a ),	/* Offset= -374 (986) */
/* 1362 */	
			0x11, 0x4,	/* FC_RP [alloced_on_stack] */
/* 1364 */	NdrFcShort( 0x6 ),	/* Offset= 6 (1370) */
/* 1366 */	
			0x13, 0x0,	/* FC_OP */
/* 1368 */	NdrFcShort( 0xffffffde ),	/* Offset= -34 (1334) */
/* 1370 */	0xb4,		/* FC_USER_MARSHAL */
			0x83,		/* 131 */
/* 1372 */	NdrFcShort( 0x3 ),	/* 3 */
/* 1374 */	NdrFcShort( 0x18 ),	/* 24 */
/* 1376 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1378 */	NdrFcShort( 0xfffffff4 ),	/* Offset= -12 (1366) */
/* 1380 */	
			0x11, 0x0,	/* FC_RP */
/* 1382 */	NdrFcShort( 0xfffffff4 ),	/* Offset= -12 (1370) */
/* 1384 */	
			0x11, 0x4,	/* FC_RP [alloced_on_stack] */
/* 1386 */	NdrFcShort( 0xfffffdcc ),	/* Offset= -564 (822) */
/* 1388 */	
			0x12, 0x0,	/* FC_UP */
/* 1390 */	NdrFcShort( 0x16 ),	/* Offset= 22 (1412) */
/* 1392 */	
			0x12, 0x0,	/* FC_UP */
/* 1394 */	NdrFcShort( 0x2 ),	/* Offset= 2 (1396) */
/* 1396 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 1398 */	NdrFcShort( 0x20 ),	/* 32 */
/* 1400 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1402 */	NdrFcShort( 0x0 ),	/* Offset= 0 (1402) */
/* 1404 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 1406 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 1408 */	NdrFcShort( 0xffffffb6 ),	/* Offset= -74 (1334) */
/* 1410 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 1412 */	0xb4,		/* FC_USER_MARSHAL */
			0x83,		/* 131 */
/* 1414 */	NdrFcShort( 0x4 ),	/* 4 */
/* 1416 */	NdrFcShort( 0x20 ),	/* 32 */
/* 1418 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1420 */	NdrFcShort( 0xffffffe4 ),	/* Offset= -28 (1392) */
/* 1422 */	
			0x11, 0x0,	/* FC_RP */
/* 1424 */	NdrFcShort( 0xfffffda6 ),	/* Offset= -602 (822) */
/* 1426 */	
			0x11, 0x10,	/* FC_RP [pointer_deref] */
/* 1428 */	NdrFcShort( 0x2 ),	/* Offset= 2 (1430) */
/* 1430 */	
			0x2f,		/* FC_IP */
			0x5c,		/* FC_PAD */
/* 1432 */	0x2b,		/* Corr desc:  parameter, FC_HYPER */
			0x0,		/*  */
/* 1434 */	NdrFcShort( 0x20 ),	/* ia64 Stack size/offset = 32 */
/* 1436 */	NdrFcShort( 0x5 ),	/* Corr flags:  early, iid_is, */
/* 1438 */	
			0x11, 0x10,	/* FC_RP [pointer_deref] */
/* 1440 */	NdrFcShort( 0x2 ),	/* Offset= 2 (1442) */
/* 1442 */	
			0x2f,		/* FC_IP */
			0x5c,		/* FC_PAD */
/* 1444 */	0x2b,		/* Corr desc:  parameter, FC_HYPER */
			0x0,		/*  */
/* 1446 */	NdrFcShort( 0x28 ),	/* ia64 Stack size/offset = 40 */
/* 1448 */	NdrFcShort( 0x5 ),	/* Corr flags:  early, iid_is, */
/* 1450 */	
			0x11, 0x10,	/* FC_RP [pointer_deref] */
/* 1452 */	NdrFcShort( 0x2 ),	/* Offset= 2 (1454) */
/* 1454 */	
			0x2f,		/* FC_IP */
			0x5c,		/* FC_PAD */
/* 1456 */	0x2b,		/* Corr desc:  parameter, FC_HYPER */
			0x0,		/*  */
/* 1458 */	NdrFcShort( 0x28 ),	/* ia64 Stack size/offset = 40 */
/* 1460 */	NdrFcShort( 0x5 ),	/* Corr flags:  early, iid_is, */
/* 1462 */	
			0x11, 0x0,	/* FC_RP */
/* 1464 */	NdrFcShort( 0x2 ),	/* Offset= 2 (1466) */
/* 1466 */	
			0x1c,		/* FC_CVARRAY */
			0x0,		/* 0 */
/* 1468 */	NdrFcShort( 0x1 ),	/* 1 */
/* 1470 */	0x29,		/* Corr desc:  parameter, FC_ULONG */
			
			0x20,		/* FC_LGVARRAY or FC_SPLIT */
/* 1472 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 1474 */	NdrFcShort( 0x2 ),	/* Corr flags:  split, */
/* 1476 */	0x29,		/* Corr desc:  parameter, FC_ULONG */
			0x54,		/* FC_DEREFERENCE */
/* 1478 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 1480 */	NdrFcShort( 0x0 ),	/* Corr flags:  */
/* 1482 */	0x1,		/* FC_BYTE */
			0x5b,		/* FC_END */
/* 1484 */	
			0x11, 0x0,	/* FC_RP */
/* 1486 */	NdrFcShort( 0x2 ),	/* Offset= 2 (1488) */
/* 1488 */	
			0x1c,		/* FC_CVARRAY */
			0x3,		/* 3 */
/* 1490 */	NdrFcShort( 0x4 ),	/* 4 */
/* 1492 */	0x29,		/* Corr desc:  parameter, FC_ULONG */
			0x0,		/*  */
/* 1494 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 1496 */	NdrFcShort( 0x0 ),	/* Corr flags:  */
/* 1498 */	0x29,		/* Corr desc:  parameter, FC_ULONG */
			0x54,		/* FC_DEREFERENCE */
/* 1500 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 1502 */	NdrFcShort( 0x0 ),	/* Corr flags:  */
/* 1504 */	0x8,		/* FC_LONG */
			0x5b,		/* FC_END */
/* 1506 */	
			0x11, 0x0,	/* FC_RP */
/* 1508 */	NdrFcShort( 0x2 ),	/* Offset= 2 (1510) */
/* 1510 */	
			0x1b,		/* FC_CARRAY */
			0x3,		/* 3 */
/* 1512 */	NdrFcShort( 0x4 ),	/* 4 */
/* 1514 */	0x29,		/* Corr desc:  parameter, FC_ULONG */
			0x0,		/*  */
/* 1516 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 1518 */	NdrFcShort( 0x0 ),	/* Corr flags:  */
/* 1520 */	0x8,		/* FC_LONG */
			0x5b,		/* FC_END */
/* 1522 */	
			0x11, 0x0,	/* FC_RP */
/* 1524 */	NdrFcShort( 0x2 ),	/* Offset= 2 (1526) */
/* 1526 */	
			0x1c,		/* FC_CVARRAY */
			0x3,		/* 3 */
/* 1528 */	NdrFcShort( 0x4 ),	/* 4 */
/* 1530 */	0x29,		/* Corr desc:  parameter, FC_ULONG */
			
			0x20,		/* FC_LGVARRAY or FC_SPLIT */
/* 1532 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 1534 */	NdrFcShort( 0x2 ),	/* Corr flags:  split, */
/* 1536 */	0x29,		/* Corr desc:  parameter, FC_ULONG */
			0x54,		/* FC_DEREFERENCE */
/* 1538 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 1540 */	NdrFcShort( 0x0 ),	/* Corr flags:  */
/* 1542 */	0x8,		/* FC_LONG */
			0x5b,		/* FC_END */
/* 1544 */	
			0x11, 0x0,	/* FC_RP */
/* 1546 */	NdrFcShort( 0x2 ),	/* Offset= 2 (1548) */
/* 1548 */	
			0x1c,		/* FC_CVARRAY */
			0x7,		/* 7 */
/* 1550 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1552 */	0x29,		/* Corr desc:  parameter, FC_ULONG */
			0x0,		/*  */
/* 1554 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 1556 */	NdrFcShort( 0x0 ),	/* Corr flags:  */
/* 1558 */	0x29,		/* Corr desc:  parameter, FC_ULONG */
			0x54,		/* FC_DEREFERENCE */
/* 1560 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 1562 */	NdrFcShort( 0x0 ),	/* Corr flags:  */
/* 1564 */	0xc,		/* FC_DOUBLE */
			0x5b,		/* FC_END */
/* 1566 */	
			0x11, 0x0,	/* FC_RP */
/* 1568 */	NdrFcShort( 0x2 ),	/* Offset= 2 (1570) */
/* 1570 */	
			0x1b,		/* FC_CARRAY */
			0x7,		/* 7 */
/* 1572 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1574 */	0x29,		/* Corr desc:  parameter, FC_ULONG */
			0x0,		/*  */
/* 1576 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 1578 */	NdrFcShort( 0x0 ),	/* Corr flags:  */
/* 1580 */	0xc,		/* FC_DOUBLE */
			0x5b,		/* FC_END */
/* 1582 */	
			0x11, 0x0,	/* FC_RP */
/* 1584 */	NdrFcShort( 0x2 ),	/* Offset= 2 (1586) */
/* 1586 */	
			0x1c,		/* FC_CVARRAY */
			0x7,		/* 7 */
/* 1588 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1590 */	0x29,		/* Corr desc:  parameter, FC_ULONG */
			
			0x20,		/* FC_LGVARRAY or FC_SPLIT */
/* 1592 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 1594 */	NdrFcShort( 0x2 ),	/* Corr flags:  split, */
/* 1596 */	0x29,		/* Corr desc:  parameter, FC_ULONG */
			0x54,		/* FC_DEREFERENCE */
/* 1598 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 1600 */	NdrFcShort( 0x0 ),	/* Corr flags:  */
/* 1602 */	0xc,		/* FC_DOUBLE */
			0x5b,		/* FC_END */
/* 1604 */	
			0x11, 0x4,	/* FC_RP [alloced_on_stack] */
/* 1606 */	NdrFcShort( 0x6 ),	/* Offset= 6 (1612) */
/* 1608 */	
			0x13, 0x0,	/* FC_OP */
/* 1610 */	NdrFcShort( 0xfffffe5a ),	/* Offset= -422 (1188) */
/* 1612 */	0xb4,		/* FC_USER_MARSHAL */
			0x83,		/* 131 */
/* 1614 */	NdrFcShort( 0x5 ),	/* 5 */
/* 1616 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1618 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1620 */	NdrFcShort( 0xfffffff4 ),	/* Offset= -12 (1608) */
/* 1622 */	
			0x12, 0x0,	/* FC_UP */
/* 1624 */	NdrFcShort( 0x2 ),	/* Offset= 2 (1626) */
/* 1626 */	
			0x2a,		/* FC_ENCAPSULATED_UNION */
			0x48,		/* 72 */
/* 1628 */	NdrFcShort( 0x4 ),	/* 4 */
/* 1630 */	NdrFcShort( 0x2 ),	/* 2 */
/* 1632 */	NdrFcLong( 0x48746457 ),	/* 1215587415 */
/* 1636 */	NdrFcShort( 0x8008 ),	/* Simple arm type: FC_LONG */
/* 1638 */	NdrFcLong( 0x52746457 ),	/* 1383359575 */
/* 1642 */	NdrFcShort( 0x8008 ),	/* Simple arm type: FC_LONG */
/* 1644 */	NdrFcShort( 0xffffffff ),	/* Offset= -1 (1643) */
/* 1646 */	0xb4,		/* FC_USER_MARSHAL */
			0x83,		/* 131 */
/* 1648 */	NdrFcShort( 0x6 ),	/* 6 */
/* 1650 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1652 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1654 */	NdrFcShort( 0xffffffe0 ),	/* Offset= -32 (1622) */
/* 1656 */	0xb4,		/* FC_USER_MARSHAL */
			0x83,		/* 131 */
/* 1658 */	NdrFcShort( 0x7 ),	/* 7 */
/* 1660 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1662 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1664 */	NdrFcShort( 0xffffffd6 ),	/* Offset= -42 (1622) */

			0x0
        }
    };

static const USER_MARSHAL_ROUTINE_QUADRUPLE UserMarshalRoutines[ WIRE_MARSHAL_TABLE_SIZE ] = 
        {
            
            {
            SNB_UserSize
            ,SNB_UserMarshal
            ,SNB_UserUnmarshal
            ,SNB_UserFree
            },
            {
            CLIPFORMAT_UserSize
            ,CLIPFORMAT_UserMarshal
            ,CLIPFORMAT_UserUnmarshal
            ,CLIPFORMAT_UserFree
            },
            {
            ASYNC_STGMEDIUM_UserSize
            ,ASYNC_STGMEDIUM_UserMarshal
            ,ASYNC_STGMEDIUM_UserUnmarshal
            ,ASYNC_STGMEDIUM_UserFree
            },
            {
            STGMEDIUM_UserSize
            ,STGMEDIUM_UserMarshal
            ,STGMEDIUM_UserUnmarshal
            ,STGMEDIUM_UserFree
            },
            {
            FLAG_STGMEDIUM_UserSize
            ,FLAG_STGMEDIUM_UserMarshal
            ,FLAG_STGMEDIUM_UserUnmarshal
            ,FLAG_STGMEDIUM_UserFree
            },
            {
            HBITMAP_UserSize
            ,HBITMAP_UserMarshal
            ,HBITMAP_UserUnmarshal
            ,HBITMAP_UserFree
            },
            {
            HICON_UserSize
            ,HICON_UserMarshal
            ,HICON_UserUnmarshal
            ,HICON_UserFree
            },
            {
            HDC_UserSize
            ,HDC_UserMarshal
            ,HDC_UserUnmarshal
            ,HDC_UserFree
            }

        };


static void __RPC_USER IEnumFORMATETC_DVTARGETDEVICEExprEval_0000( PMIDL_STUB_MESSAGE pStubMsg )
{
    DVTARGETDEVICE *pS	=	( DVTARGETDEVICE * )(pStubMsg->StackTop - 12);
    
    pStubMsg->Offset = 0;
    pStubMsg->MaxCount = (ULONG_PTR) ( pS->tdSize - sizeof( DWORD  ) - 4 * sizeof( WORD  ) );
}

static const EXPR_EVAL ExprEvalRoutines[] = 
    {
    IEnumFORMATETC_DVTARGETDEVICEExprEval_0000
    };



/* Standard interface: __MIDL_itf_objidl_0000, ver. 0.0,
   GUID={0x00000000,0x0000,0x0000,{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}} */


/* Object interface: IUnknown, ver. 0.0,
   GUID={0x00000000,0x0000,0x0000,{0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x46}} */


/* Object interface: IMarshal, ver. 0.0,
   GUID={0x00000003,0x0000,0x0000,{0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x46}} */


/* Object interface: IMarshal2, ver. 0.0,
   GUID={0x000001cf,0x0000,0x0000,{0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x46}} */


/* Object interface: IMalloc, ver. 0.0,
   GUID={0x00000002,0x0000,0x0000,{0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x46}} */


/* Object interface: IMallocSpy, ver. 0.0,
   GUID={0x0000001d,0x0000,0x0000,{0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x46}} */


/* Object interface: IStdMarshalInfo, ver. 0.0,
   GUID={0x00000018,0x0000,0x0000,{0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x46}} */


/* Object interface: IExternalConnection, ver. 0.0,
   GUID={0x00000019,0x0000,0x0000,{0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x46}} */


/* Standard interface: __MIDL_itf_objidl_0015, ver. 0.0,
   GUID={0x00000000,0x0000,0x0000,{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}} */


/* Object interface: IMultiQI, ver. 0.0,
   GUID={0x00000020,0x0000,0x0000,{0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x46}} */


/* Object interface: AsyncIMultiQI, ver. 0.0,
   GUID={0x000e0020,0x0000,0x0000,{0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x46}} */


/* Object interface: IInternalUnknown, ver. 0.0,
   GUID={0x00000021,0x0000,0x0000,{0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x46}} */


/* Object interface: IEnumUnknown, ver. 0.0,
   GUID={0x00000100,0x0000,0x0000,{0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x46}} */

#pragma code_seg(".orpc")
static const unsigned short IEnumUnknown_FormatStringOffsetTable[] =
    {
    0,
    50,
    88,
    120
    };

static const MIDL_STUBLESS_PROXY_INFO IEnumUnknown_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &IEnumUnknown_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const STUB_THUNK IEnumUnknown_StubThunkTable[] = 
    {
    IEnumUnknown_RemoteNext_Thunk,
    0,
    0,
    0
    };

static const MIDL_SERVER_INFO IEnumUnknown_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &IEnumUnknown_FormatStringOffsetTable[-3],
    &IEnumUnknown_StubThunkTable[-3],
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(7) _IEnumUnknownProxyVtbl = 
{
    &IEnumUnknown_ProxyInfo,
    &IID_IEnumUnknown,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    IEnumUnknown_Next_Proxy ,
    (void *) (INT_PTR) -1 /* IEnumUnknown::Skip */ ,
    (void *) (INT_PTR) -1 /* IEnumUnknown::Reset */ ,
    (void *) (INT_PTR) -1 /* IEnumUnknown::Clone */
};

const CInterfaceStubVtbl _IEnumUnknownStubVtbl =
{
    &IID_IEnumUnknown,
    &IEnumUnknown_ServerInfo,
    7,
    0, /* pure interpreted */
    CStdStubBuffer_METHODS
};


/* Object interface: IBindCtx, ver. 0.0,
   GUID={0x0000000e,0x0000,0x0000,{0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x46}} */

#pragma code_seg(".orpc")
static const unsigned short IBindCtx_FormatStringOffsetTable[] =
    {
    158,
    196,
    88,
    234,
    272,
    310,
    348,
    392,
    436,
    474
    };

static const MIDL_STUBLESS_PROXY_INFO IBindCtx_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &IBindCtx_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const STUB_THUNK IBindCtx_StubThunkTable[] = 
    {
    0,
    0,
    0,
    IBindCtx_RemoteSetBindOptions_Thunk,
    IBindCtx_RemoteGetBindOptions_Thunk,
    0,
    0,
    0,
    0,
    0
    };

static const MIDL_SERVER_INFO IBindCtx_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &IBindCtx_FormatStringOffsetTable[-3],
    &IBindCtx_StubThunkTable[-3],
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(13) _IBindCtxProxyVtbl = 
{
    &IBindCtx_ProxyInfo,
    &IID_IBindCtx,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    (void *) (INT_PTR) -1 /* IBindCtx::RegisterObjectBound */ ,
    (void *) (INT_PTR) -1 /* IBindCtx::RevokeObjectBound */ ,
    (void *) (INT_PTR) -1 /* IBindCtx::ReleaseBoundObjects */ ,
    IBindCtx_SetBindOptions_Proxy ,
    IBindCtx_GetBindOptions_Proxy ,
    (void *) (INT_PTR) -1 /* IBindCtx::GetRunningObjectTable */ ,
    (void *) (INT_PTR) -1 /* IBindCtx::RegisterObjectParam */ ,
    (void *) (INT_PTR) -1 /* IBindCtx::GetObjectParam */ ,
    (void *) (INT_PTR) -1 /* IBindCtx::EnumObjectParam */ ,
    (void *) (INT_PTR) -1 /* IBindCtx::RevokeObjectParam */
};

const CInterfaceStubVtbl _IBindCtxStubVtbl =
{
    &IID_IBindCtx,
    &IBindCtx_ServerInfo,
    13,
    0, /* pure interpreted */
    CStdStubBuffer_METHODS
};


/* Object interface: IEnumMoniker, ver. 0.0,
   GUID={0x00000102,0x0000,0x0000,{0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x46}} */

#pragma code_seg(".orpc")
static const unsigned short IEnumMoniker_FormatStringOffsetTable[] =
    {
    512,
    50,
    88,
    562
    };

static const MIDL_STUBLESS_PROXY_INFO IEnumMoniker_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &IEnumMoniker_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const STUB_THUNK IEnumMoniker_StubThunkTable[] = 
    {
    IEnumMoniker_RemoteNext_Thunk,
    0,
    0,
    0
    };

static const MIDL_SERVER_INFO IEnumMoniker_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &IEnumMoniker_FormatStringOffsetTable[-3],
    &IEnumMoniker_StubThunkTable[-3],
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(7) _IEnumMonikerProxyVtbl = 
{
    &IEnumMoniker_ProxyInfo,
    &IID_IEnumMoniker,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    IEnumMoniker_Next_Proxy ,
    (void *) (INT_PTR) -1 /* IEnumMoniker::Skip */ ,
    (void *) (INT_PTR) -1 /* IEnumMoniker::Reset */ ,
    (void *) (INT_PTR) -1 /* IEnumMoniker::Clone */
};

const CInterfaceStubVtbl _IEnumMonikerStubVtbl =
{
    &IID_IEnumMoniker,
    &IEnumMoniker_ServerInfo,
    7,
    0, /* pure interpreted */
    CStdStubBuffer_METHODS
};


/* Object interface: IRunnableObject, ver. 0.0,
   GUID={0x00000126,0x0000,0x0000,{0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x46}} */

#pragma code_seg(".orpc")
static const unsigned short IRunnableObject_FormatStringOffsetTable[] =
    {
    600,
    638,
    88,
    676,
    720
    };

static const MIDL_STUBLESS_PROXY_INFO IRunnableObject_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &IRunnableObject_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const STUB_THUNK IRunnableObject_StubThunkTable[] = 
    {
    0,
    0,
    IRunnableObject_RemoteIsRunning_Thunk,
    0,
    0
    };

static const MIDL_SERVER_INFO IRunnableObject_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &IRunnableObject_FormatStringOffsetTable[-3],
    &IRunnableObject_StubThunkTable[-3],
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(8) _IRunnableObjectProxyVtbl = 
{
    &IRunnableObject_ProxyInfo,
    &IID_IRunnableObject,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    (void *) (INT_PTR) -1 /* IRunnableObject::GetRunningClass */ ,
    (void *) (INT_PTR) -1 /* IRunnableObject::Run */ ,
    IRunnableObject_IsRunning_Proxy ,
    (void *) (INT_PTR) -1 /* IRunnableObject::LockRunning */ ,
    (void *) (INT_PTR) -1 /* IRunnableObject::SetContainedObject */
};

const CInterfaceStubVtbl _IRunnableObjectStubVtbl =
{
    &IID_IRunnableObject,
    &IRunnableObject_ServerInfo,
    8,
    0, /* pure interpreted */
    CStdStubBuffer_METHODS
};


/* Object interface: IRunningObjectTable, ver. 0.0,
   GUID={0x00000010,0x0000,0x0000,{0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x46}} */

#pragma code_seg(".orpc")
static const unsigned short IRunningObjectTable_FormatStringOffsetTable[] =
    {
    758,
    50,
    814,
    852,
    896,
    940,
    984
    };

static const MIDL_STUBLESS_PROXY_INFO IRunningObjectTable_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &IRunningObjectTable_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IRunningObjectTable_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &IRunningObjectTable_FormatStringOffsetTable[-3],
    0,
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(10) _IRunningObjectTableProxyVtbl = 
{
    &IRunningObjectTable_ProxyInfo,
    &IID_IRunningObjectTable,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    (void *) (INT_PTR) -1 /* IRunningObjectTable::Register */ ,
    (void *) (INT_PTR) -1 /* IRunningObjectTable::Revoke */ ,
    (void *) (INT_PTR) -1 /* IRunningObjectTable::IsRunning */ ,
    (void *) (INT_PTR) -1 /* IRunningObjectTable::GetObject */ ,
    (void *) (INT_PTR) -1 /* IRunningObjectTable::NoteChangeTime */ ,
    (void *) (INT_PTR) -1 /* IRunningObjectTable::GetTimeOfLastChange */ ,
    (void *) (INT_PTR) -1 /* IRunningObjectTable::EnumRunning */
};

const CInterfaceStubVtbl _IRunningObjectTableStubVtbl =
{
    &IID_IRunningObjectTable,
    &IRunningObjectTable_ServerInfo,
    10,
    0, /* pure interpreted */
    CStdStubBuffer_METHODS
};


/* Object interface: IPersist, ver. 0.0,
   GUID={0x0000010c,0x0000,0x0000,{0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x46}} */

#pragma code_seg(".orpc")
static const unsigned short IPersist_FormatStringOffsetTable[] =
    {
    600
    };

static const MIDL_STUBLESS_PROXY_INFO IPersist_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &IPersist_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IPersist_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &IPersist_FormatStringOffsetTable[-3],
    0,
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(4) _IPersistProxyVtbl = 
{
    &IPersist_ProxyInfo,
    &IID_IPersist,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    (void *) (INT_PTR) -1 /* IPersist::GetClassID */
};

const CInterfaceStubVtbl _IPersistStubVtbl =
{
    &IID_IPersist,
    &IPersist_ServerInfo,
    4,
    0, /* pure interpreted */
    CStdStubBuffer_METHODS
};


/* Object interface: IPersistStream, ver. 0.0,
   GUID={0x00000109,0x0000,0x0000,{0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x46}} */

#pragma code_seg(".orpc")
static const unsigned short IPersistStream_FormatStringOffsetTable[] =
    {
    600,
    1022,
    1054,
    1092,
    1136
    };

static const MIDL_STUBLESS_PROXY_INFO IPersistStream_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &IPersistStream_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IPersistStream_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &IPersistStream_FormatStringOffsetTable[-3],
    0,
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(8) _IPersistStreamProxyVtbl = 
{
    &IPersistStream_ProxyInfo,
    &IID_IPersistStream,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    (void *) (INT_PTR) -1 /* IPersist::GetClassID */ ,
    (void *) (INT_PTR) -1 /* IPersistStream::IsDirty */ ,
    (void *) (INT_PTR) -1 /* IPersistStream::Load */ ,
    (void *) (INT_PTR) -1 /* IPersistStream::Save */ ,
    (void *) (INT_PTR) -1 /* IPersistStream::GetSizeMax */
};

const CInterfaceStubVtbl _IPersistStreamStubVtbl =
{
    &IID_IPersistStream,
    &IPersistStream_ServerInfo,
    8,
    0, /* pure interpreted */
    CStdStubBuffer_METHODS
};


/* Object interface: IMoniker, ver. 0.0,
   GUID={0x0000000f,0x0000,0x0000,{0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x46}} */

#pragma code_seg(".orpc")
static const unsigned short IMoniker_FormatStringOffsetTable[] =
    {
    600,
    1022,
    1054,
    1092,
    1136,
    1174,
    1230,
    1286,
    1342,
    1392,
    1436,
    1474,
    1512,
    1562,
    1612,
    1650,
    1694,
    1738,
    1788,
    1850
    };

static const MIDL_STUBLESS_PROXY_INFO IMoniker_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &IMoniker_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const STUB_THUNK IMoniker_StubThunkTable[] = 
    {
    0,
    0,
    0,
    0,
    0,
    IMoniker_RemoteBindToObject_Thunk,
    IMoniker_RemoteBindToStorage_Thunk,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0
    };

static const MIDL_SERVER_INFO IMoniker_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &IMoniker_FormatStringOffsetTable[-3],
    &IMoniker_StubThunkTable[-3],
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(23) _IMonikerProxyVtbl = 
{
    &IMoniker_ProxyInfo,
    &IID_IMoniker,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    (void *) (INT_PTR) -1 /* IPersist::GetClassID */ ,
    (void *) (INT_PTR) -1 /* IPersistStream::IsDirty */ ,
    (void *) (INT_PTR) -1 /* IPersistStream::Load */ ,
    (void *) (INT_PTR) -1 /* IPersistStream::Save */ ,
    (void *) (INT_PTR) -1 /* IPersistStream::GetSizeMax */ ,
    IMoniker_BindToObject_Proxy ,
    IMoniker_BindToStorage_Proxy ,
    (void *) (INT_PTR) -1 /* IMoniker::Reduce */ ,
    (void *) (INT_PTR) -1 /* IMoniker::ComposeWith */ ,
    (void *) (INT_PTR) -1 /* IMoniker::Enum */ ,
    (void *) (INT_PTR) -1 /* IMoniker::IsEqual */ ,
    (void *) (INT_PTR) -1 /* IMoniker::Hash */ ,
    (void *) (INT_PTR) -1 /* IMoniker::IsRunning */ ,
    (void *) (INT_PTR) -1 /* IMoniker::GetTimeOfLastChange */ ,
    (void *) (INT_PTR) -1 /* IMoniker::Inverse */ ,
    (void *) (INT_PTR) -1 /* IMoniker::CommonPrefixWith */ ,
    (void *) (INT_PTR) -1 /* IMoniker::RelativePathTo */ ,
    (void *) (INT_PTR) -1 /* IMoniker::GetDisplayName */ ,
    (void *) (INT_PTR) -1 /* IMoniker::ParseDisplayName */ ,
    (void *) (INT_PTR) -1 /* IMoniker::IsSystemMoniker */
};

const CInterfaceStubVtbl _IMonikerStubVtbl =
{
    &IID_IMoniker,
    &IMoniker_ServerInfo,
    23,
    0, /* pure interpreted */
    CStdStubBuffer_METHODS
};


/* Object interface: IROTData, ver. 0.0,
   GUID={0xf29f6bc0,0x5021,0x11ce,{0xaa,0x15,0x00,0x00,0x69,0x01,0x29,0x3f}} */

#pragma code_seg(".orpc")
static const unsigned short IROTData_FormatStringOffsetTable[] =
    {
    1888
    };

static const MIDL_STUBLESS_PROXY_INFO IROTData_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &IROTData_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IROTData_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &IROTData_FormatStringOffsetTable[-3],
    0,
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(4) _IROTDataProxyVtbl = 
{
    &IROTData_ProxyInfo,
    &IID_IROTData,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    (void *) (INT_PTR) -1 /* IROTData::GetComparisonData */
};

const CInterfaceStubVtbl _IROTDataStubVtbl =
{
    &IID_IROTData,
    &IROTData_ServerInfo,
    4,
    0, /* pure interpreted */
    CStdStubBuffer_METHODS
};


/* Object interface: IEnumString, ver. 0.0,
   GUID={0x00000101,0x0000,0x0000,{0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x46}} */

#pragma code_seg(".orpc")
static const unsigned short IEnumString_FormatStringOffsetTable[] =
    {
    1938,
    50,
    88,
    1988
    };

static const MIDL_STUBLESS_PROXY_INFO IEnumString_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &IEnumString_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const STUB_THUNK IEnumString_StubThunkTable[] = 
    {
    IEnumString_RemoteNext_Thunk,
    0,
    0,
    0
    };

static const MIDL_SERVER_INFO IEnumString_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &IEnumString_FormatStringOffsetTable[-3],
    &IEnumString_StubThunkTable[-3],
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(7) _IEnumStringProxyVtbl = 
{
    &IEnumString_ProxyInfo,
    &IID_IEnumString,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    IEnumString_Next_Proxy ,
    (void *) (INT_PTR) -1 /* IEnumString::Skip */ ,
    (void *) (INT_PTR) -1 /* IEnumString::Reset */ ,
    (void *) (INT_PTR) -1 /* IEnumString::Clone */
};

const CInterfaceStubVtbl _IEnumStringStubVtbl =
{
    &IID_IEnumString,
    &IEnumString_ServerInfo,
    7,
    0, /* pure interpreted */
    CStdStubBuffer_METHODS
};


/* Object interface: ISequentialStream, ver. 0.0,
   GUID={0x0c733a30,0x2a1c,0x11ce,{0xad,0xe5,0x00,0xaa,0x00,0x44,0x77,0x3d}} */

#pragma code_seg(".orpc")
static const unsigned short ISequentialStream_FormatStringOffsetTable[] =
    {
    2026,
    2076
    };

static const MIDL_STUBLESS_PROXY_INFO ISequentialStream_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &ISequentialStream_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const STUB_THUNK ISequentialStream_StubThunkTable[] = 
    {
    ISequentialStream_RemoteRead_Thunk,
    ISequentialStream_RemoteWrite_Thunk
    };

static const MIDL_SERVER_INFO ISequentialStream_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &ISequentialStream_FormatStringOffsetTable[-3],
    &ISequentialStream_StubThunkTable[-3],
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(5) _ISequentialStreamProxyVtbl = 
{
    &ISequentialStream_ProxyInfo,
    &IID_ISequentialStream,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    ISequentialStream_Read_Proxy ,
    ISequentialStream_Write_Proxy
};

const CInterfaceStubVtbl _ISequentialStreamStubVtbl =
{
    &IID_ISequentialStream,
    &ISequentialStream_ServerInfo,
    5,
    0, /* pure interpreted */
    CStdStubBuffer_METHODS
};


/* Object interface: IStream, ver. 0.0,
   GUID={0x0000000c,0x0000,0x0000,{0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x46}} */

#pragma code_seg(".orpc")
static const unsigned short IStream_FormatStringOffsetTable[] =
    {
    2026,
    2076,
    2126,
    2176,
    2214,
    2270,
    2308,
    2340,
    2390,
    2440,
    2484
    };

static const MIDL_STUBLESS_PROXY_INFO IStream_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &IStream_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const STUB_THUNK IStream_StubThunkTable[] = 
    {
    ISequentialStream_RemoteRead_Thunk,
    ISequentialStream_RemoteWrite_Thunk,
    IStream_RemoteSeek_Thunk,
    0,
    IStream_RemoteCopyTo_Thunk,
    0,
    0,
    0,
    0,
    0,
    0
    };

static const MIDL_SERVER_INFO IStream_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &IStream_FormatStringOffsetTable[-3],
    &IStream_StubThunkTable[-3],
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(14) _IStreamProxyVtbl = 
{
    &IStream_ProxyInfo,
    &IID_IStream,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    ISequentialStream_Read_Proxy ,
    ISequentialStream_Write_Proxy ,
    IStream_Seek_Proxy ,
    (void *) (INT_PTR) -1 /* IStream::SetSize */ ,
    IStream_CopyTo_Proxy ,
    (void *) (INT_PTR) -1 /* IStream::Commit */ ,
    (void *) (INT_PTR) -1 /* IStream::Revert */ ,
    (void *) (INT_PTR) -1 /* IStream::LockRegion */ ,
    (void *) (INT_PTR) -1 /* IStream::UnlockRegion */ ,
    (void *) (INT_PTR) -1 /* IStream::Stat */ ,
    (void *) (INT_PTR) -1 /* IStream::Clone */
};

const CInterfaceStubVtbl _IStreamStubVtbl =
{
    &IID_IStream,
    &IStream_ServerInfo,
    14,
    0, /* pure interpreted */
    CStdStubBuffer_METHODS
};


/* Object interface: IEnumSTATSTG, ver. 0.0,
   GUID={0x0000000d,0x0000,0x0000,{0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x46}} */

#pragma code_seg(".orpc")
static const unsigned short IEnumSTATSTG_FormatStringOffsetTable[] =
    {
    2522,
    50,
    88,
    2572
    };

static const MIDL_STUBLESS_PROXY_INFO IEnumSTATSTG_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &IEnumSTATSTG_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const STUB_THUNK IEnumSTATSTG_StubThunkTable[] = 
    {
    IEnumSTATSTG_RemoteNext_Thunk,
    0,
    0,
    0
    };

static const MIDL_SERVER_INFO IEnumSTATSTG_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &IEnumSTATSTG_FormatStringOffsetTable[-3],
    &IEnumSTATSTG_StubThunkTable[-3],
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(7) _IEnumSTATSTGProxyVtbl = 
{
    &IEnumSTATSTG_ProxyInfo,
    &IID_IEnumSTATSTG,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    IEnumSTATSTG_Next_Proxy ,
    (void *) (INT_PTR) -1 /* IEnumSTATSTG::Skip */ ,
    (void *) (INT_PTR) -1 /* IEnumSTATSTG::Reset */ ,
    (void *) (INT_PTR) -1 /* IEnumSTATSTG::Clone */
};

const CInterfaceStubVtbl _IEnumSTATSTGStubVtbl =
{
    &IID_IEnumSTATSTG,
    &IEnumSTATSTG_ServerInfo,
    7,
    0, /* pure interpreted */
    CStdStubBuffer_METHODS
};


/* Object interface: IStorage, ver. 0.0,
   GUID={0x0000000b,0x0000,0x0000,{0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x46}} */

#pragma code_seg(".orpc")
static const unsigned short IStorage_FormatStringOffsetTable[] =
    {
    2610,
    2672,
    2740,
    2802,
    2870,
    2926,
    2982,
    3020,
    3052,
    474,
    3114,
    3158,
    3214,
    3252,
    3296
    };

static const MIDL_STUBLESS_PROXY_INFO IStorage_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &IStorage_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const STUB_THUNK IStorage_StubThunkTable[] = 
    {
    0,
    IStorage_RemoteOpenStream_Thunk,
    0,
    0,
    0,
    0,
    0,
    0,
    IStorage_RemoteEnumElements_Thunk,
    0,
    0,
    0,
    0,
    0,
    0
    };

static const MIDL_SERVER_INFO IStorage_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &IStorage_FormatStringOffsetTable[-3],
    &IStorage_StubThunkTable[-3],
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(18) _IStorageProxyVtbl = 
{
    &IStorage_ProxyInfo,
    &IID_IStorage,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    (void *) (INT_PTR) -1 /* IStorage::CreateStream */ ,
    IStorage_OpenStream_Proxy ,
    (void *) (INT_PTR) -1 /* IStorage::CreateStorage */ ,
    (void *) (INT_PTR) -1 /* IStorage::OpenStorage */ ,
    (void *) (INT_PTR) -1 /* IStorage::CopyTo */ ,
    (void *) (INT_PTR) -1 /* IStorage::MoveElementTo */ ,
    (void *) (INT_PTR) -1 /* IStorage::Commit */ ,
    (void *) (INT_PTR) -1 /* IStorage::Revert */ ,
    IStorage_EnumElements_Proxy ,
    (void *) (INT_PTR) -1 /* IStorage::DestroyElement */ ,
    (void *) (INT_PTR) -1 /* IStorage::RenameElement */ ,
    (void *) (INT_PTR) -1 /* IStorage::SetElementTimes */ ,
    (void *) (INT_PTR) -1 /* IStorage::SetClass */ ,
    (void *) (INT_PTR) -1 /* IStorage::SetStateBits */ ,
    (void *) (INT_PTR) -1 /* IStorage::Stat */
};

const CInterfaceStubVtbl _IStorageStubVtbl =
{
    &IID_IStorage,
    &IStorage_ServerInfo,
    18,
    0, /* pure interpreted */
    CStdStubBuffer_METHODS
};


/* Object interface: IPersistFile, ver. 0.0,
   GUID={0x0000010b,0x0000,0x0000,{0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x46}} */

#pragma code_seg(".orpc")
static const unsigned short IPersistFile_FormatStringOffsetTable[] =
    {
    600,
    1022,
    3340,
    3384,
    3428,
    3466
    };

static const MIDL_STUBLESS_PROXY_INFO IPersistFile_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &IPersistFile_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IPersistFile_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &IPersistFile_FormatStringOffsetTable[-3],
    0,
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(9) _IPersistFileProxyVtbl = 
{
    &IPersistFile_ProxyInfo,
    &IID_IPersistFile,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    (void *) (INT_PTR) -1 /* IPersist::GetClassID */ ,
    (void *) (INT_PTR) -1 /* IPersistFile::IsDirty */ ,
    (void *) (INT_PTR) -1 /* IPersistFile::Load */ ,
    (void *) (INT_PTR) -1 /* IPersistFile::Save */ ,
    (void *) (INT_PTR) -1 /* IPersistFile::SaveCompleted */ ,
    (void *) (INT_PTR) -1 /* IPersistFile::GetCurFile */
};

const CInterfaceStubVtbl _IPersistFileStubVtbl =
{
    &IID_IPersistFile,
    &IPersistFile_ServerInfo,
    9,
    0, /* pure interpreted */
    CStdStubBuffer_METHODS
};


/* Object interface: IPersistStorage, ver. 0.0,
   GUID={0x0000010a,0x0000,0x0000,{0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x46}} */

#pragma code_seg(".orpc")
static const unsigned short IPersistStorage_FormatStringOffsetTable[] =
    {
    600,
    1022,
    3504,
    3542,
    3580,
    3624,
    2308
    };

static const MIDL_STUBLESS_PROXY_INFO IPersistStorage_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &IPersistStorage_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IPersistStorage_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &IPersistStorage_FormatStringOffsetTable[-3],
    0,
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(10) _IPersistStorageProxyVtbl = 
{
    &IPersistStorage_ProxyInfo,
    &IID_IPersistStorage,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    (void *) (INT_PTR) -1 /* IPersist::GetClassID */ ,
    (void *) (INT_PTR) -1 /* IPersistStorage::IsDirty */ ,
    (void *) (INT_PTR) -1 /* IPersistStorage::InitNew */ ,
    (void *) (INT_PTR) -1 /* IPersistStorage::Load */ ,
    (void *) (INT_PTR) -1 /* IPersistStorage::Save */ ,
    (void *) (INT_PTR) -1 /* IPersistStorage::SaveCompleted */ ,
    (void *) (INT_PTR) -1 /* IPersistStorage::HandsOffStorage */
};

const CInterfaceStubVtbl _IPersistStorageStubVtbl =
{
    &IID_IPersistStorage,
    &IPersistStorage_ServerInfo,
    10,
    0, /* pure interpreted */
    CStdStubBuffer_METHODS
};


/* Object interface: ILockBytes, ver. 0.0,
   GUID={0x0000000a,0x0000,0x0000,{0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x46}} */

#pragma code_seg(".orpc")
static const unsigned short ILockBytes_FormatStringOffsetTable[] =
    {
    3662,
    3718,
    88,
    2176,
    3774,
    3824,
    3874
    };

static const MIDL_STUBLESS_PROXY_INFO ILockBytes_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &ILockBytes_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const STUB_THUNK ILockBytes_StubThunkTable[] = 
    {
    ILockBytes_RemoteReadAt_Thunk,
    ILockBytes_RemoteWriteAt_Thunk,
    0,
    0,
    0,
    0,
    0
    };

static const MIDL_SERVER_INFO ILockBytes_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &ILockBytes_FormatStringOffsetTable[-3],
    &ILockBytes_StubThunkTable[-3],
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(10) _ILockBytesProxyVtbl = 
{
    &ILockBytes_ProxyInfo,
    &IID_ILockBytes,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    ILockBytes_ReadAt_Proxy ,
    ILockBytes_WriteAt_Proxy ,
    (void *) (INT_PTR) -1 /* ILockBytes::Flush */ ,
    (void *) (INT_PTR) -1 /* ILockBytes::SetSize */ ,
    (void *) (INT_PTR) -1 /* ILockBytes::LockRegion */ ,
    (void *) (INT_PTR) -1 /* ILockBytes::UnlockRegion */ ,
    (void *) (INT_PTR) -1 /* ILockBytes::Stat */
};

const CInterfaceStubVtbl _ILockBytesStubVtbl =
{
    &IID_ILockBytes,
    &ILockBytes_ServerInfo,
    10,
    0, /* pure interpreted */
    CStdStubBuffer_METHODS
};


/* Object interface: IEnumFORMATETC, ver. 0.0,
   GUID={0x00000103,0x0000,0x0000,{0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x46}} */

#pragma code_seg(".orpc")
static const unsigned short IEnumFORMATETC_FormatStringOffsetTable[] =
    {
    3918,
    50,
    88,
    3968
    };

static const MIDL_STUBLESS_PROXY_INFO IEnumFORMATETC_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &IEnumFORMATETC_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const STUB_THUNK IEnumFORMATETC_StubThunkTable[] = 
    {
    IEnumFORMATETC_RemoteNext_Thunk,
    0,
    0,
    0
    };

static const MIDL_SERVER_INFO IEnumFORMATETC_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &IEnumFORMATETC_FormatStringOffsetTable[-3],
    &IEnumFORMATETC_StubThunkTable[-3],
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(7) _IEnumFORMATETCProxyVtbl = 
{
    &IEnumFORMATETC_ProxyInfo,
    &IID_IEnumFORMATETC,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    IEnumFORMATETC_Next_Proxy ,
    (void *) (INT_PTR) -1 /* IEnumFORMATETC::Skip */ ,
    (void *) (INT_PTR) -1 /* IEnumFORMATETC::Reset */ ,
    (void *) (INT_PTR) -1 /* IEnumFORMATETC::Clone */
};

const CInterfaceStubVtbl _IEnumFORMATETCStubVtbl =
{
    &IID_IEnumFORMATETC,
    &IEnumFORMATETC_ServerInfo,
    7,
    0, /* pure interpreted */
    CStdStubBuffer_METHODS
};


/* Object interface: IEnumSTATDATA, ver. 0.0,
   GUID={0x00000105,0x0000,0x0000,{0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x46}} */

#pragma code_seg(".orpc")
static const unsigned short IEnumSTATDATA_FormatStringOffsetTable[] =
    {
    4006,
    50,
    88,
    4056
    };

static const MIDL_STUBLESS_PROXY_INFO IEnumSTATDATA_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &IEnumSTATDATA_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const STUB_THUNK IEnumSTATDATA_StubThunkTable[] = 
    {
    IEnumSTATDATA_RemoteNext_Thunk,
    0,
    0,
    0
    };

static const MIDL_SERVER_INFO IEnumSTATDATA_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &IEnumSTATDATA_FormatStringOffsetTable[-3],
    &IEnumSTATDATA_StubThunkTable[-3],
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(7) _IEnumSTATDATAProxyVtbl = 
{
    &IEnumSTATDATA_ProxyInfo,
    &IID_IEnumSTATDATA,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    IEnumSTATDATA_Next_Proxy ,
    (void *) (INT_PTR) -1 /* IEnumSTATDATA::Skip */ ,
    (void *) (INT_PTR) -1 /* IEnumSTATDATA::Reset */ ,
    (void *) (INT_PTR) -1 /* IEnumSTATDATA::Clone */
};

const CInterfaceStubVtbl _IEnumSTATDATAStubVtbl =
{
    &IID_IEnumSTATDATA,
    &IEnumSTATDATA_ServerInfo,
    7,
    0, /* pure interpreted */
    CStdStubBuffer_METHODS
};


/* Object interface: IRootStorage, ver. 0.0,
   GUID={0x00000012,0x0000,0x0000,{0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x46}} */

#pragma code_seg(".orpc")
static const unsigned short IRootStorage_FormatStringOffsetTable[] =
    {
    4094
    };

static const MIDL_STUBLESS_PROXY_INFO IRootStorage_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &IRootStorage_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IRootStorage_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &IRootStorage_FormatStringOffsetTable[-3],
    0,
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(4) _IRootStorageProxyVtbl = 
{
    &IRootStorage_ProxyInfo,
    &IID_IRootStorage,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    (void *) (INT_PTR) -1 /* IRootStorage::SwitchToFile */
};

const CInterfaceStubVtbl _IRootStorageStubVtbl =
{
    &IID_IRootStorage,
    &IRootStorage_ServerInfo,
    4,
    0, /* pure interpreted */
    CStdStubBuffer_METHODS
};


/* Object interface: IAdviseSink, ver. 0.0,
   GUID={0x0000010f,0x0000,0x0000,{0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x46}} */

#pragma code_seg(".orpc")
static const unsigned short IAdviseSink_FormatStringOffsetTable[] =
    {
    4132,
    4176,
    814,
    4220,
    4252
    };

static const MIDL_STUBLESS_PROXY_INFO IAdviseSink_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &IAdviseSink_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const STUB_THUNK IAdviseSink_StubThunkTable[] = 
    {
    IAdviseSink_RemoteOnDataChange_Thunk,
    IAdviseSink_RemoteOnViewChange_Thunk,
    IAdviseSink_RemoteOnRename_Thunk,
    IAdviseSink_RemoteOnSave_Thunk,
    IAdviseSink_RemoteOnClose_Thunk
    };

static const MIDL_SERVER_INFO IAdviseSink_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &IAdviseSink_FormatStringOffsetTable[-3],
    &IAdviseSink_StubThunkTable[-3],
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(8) _IAdviseSinkProxyVtbl = 
{
    &IAdviseSink_ProxyInfo,
    &IID_IAdviseSink,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    IAdviseSink_OnDataChange_Proxy ,
    IAdviseSink_OnViewChange_Proxy ,
    IAdviseSink_OnRename_Proxy ,
    IAdviseSink_OnSave_Proxy ,
    IAdviseSink_OnClose_Proxy
};

const CInterfaceStubVtbl _IAdviseSinkStubVtbl =
{
    &IID_IAdviseSink,
    &IAdviseSink_ServerInfo,
    8,
    0, /* pure interpreted */
    CStdStubBuffer_METHODS
};


/* Object interface: AsyncIAdviseSink, ver. 0.0,
   GUID={0x00000150,0x0000,0x0000,{0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x46}} */

#pragma code_seg(".orpc")
static const unsigned short AsyncIAdviseSink_FormatStringOffsetTable[] =
    {
    4284,
    4328,
    4360,
    4404,
    4436,
    4474,
    4506,
    4538,
    4570,
    4602
    };

static const MIDL_STUBLESS_PROXY_INFO AsyncIAdviseSink_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &AsyncIAdviseSink_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const STUB_THUNK AsyncIAdviseSink_StubThunkTable[] = 
    {
    AsyncIAdviseSink_Begin_RemoteOnDataChange_Thunk,
    AsyncIAdviseSink_Finish_RemoteOnDataChange_Thunk,
    AsyncIAdviseSink_Begin_RemoteOnViewChange_Thunk,
    AsyncIAdviseSink_Finish_RemoteOnViewChange_Thunk,
    AsyncIAdviseSink_Begin_RemoteOnRename_Thunk,
    AsyncIAdviseSink_Finish_RemoteOnRename_Thunk,
    AsyncIAdviseSink_Begin_RemoteOnSave_Thunk,
    AsyncIAdviseSink_Finish_RemoteOnSave_Thunk,
    AsyncIAdviseSink_Begin_RemoteOnClose_Thunk,
    AsyncIAdviseSink_Finish_RemoteOnClose_Thunk
    };

static const MIDL_SERVER_INFO AsyncIAdviseSink_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &AsyncIAdviseSink_FormatStringOffsetTable[-3],
    &AsyncIAdviseSink_StubThunkTable[-3],
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(13) _AsyncIAdviseSinkProxyVtbl = 
{
    &AsyncIAdviseSink_ProxyInfo,
    &IID_AsyncIAdviseSink,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    AsyncIAdviseSink_Begin_OnDataChange_Proxy ,
    AsyncIAdviseSink_Finish_OnDataChange_Proxy ,
    AsyncIAdviseSink_Begin_OnViewChange_Proxy ,
    AsyncIAdviseSink_Finish_OnViewChange_Proxy ,
    AsyncIAdviseSink_Begin_OnRename_Proxy ,
    AsyncIAdviseSink_Finish_OnRename_Proxy ,
    AsyncIAdviseSink_Begin_OnSave_Proxy ,
    AsyncIAdviseSink_Finish_OnSave_Proxy ,
    AsyncIAdviseSink_Begin_OnClose_Proxy ,
    AsyncIAdviseSink_Finish_OnClose_Proxy
};

CInterfaceStubVtbl _AsyncIAdviseSinkStubVtbl =
{
    &IID_AsyncIAdviseSink,
    &AsyncIAdviseSink_ServerInfo,
    13,
    0, /* pure interpreted */
    CStdAsyncStubBuffer_METHODS
};


/* Object interface: IAdviseSink2, ver. 0.0,
   GUID={0x00000125,0x0000,0x0000,{0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x46}} */

#pragma code_seg(".orpc")
static const unsigned short IAdviseSink2_FormatStringOffsetTable[] =
    {
    4132,
    4176,
    814,
    4220,
    4252,
    4634
    };

static const MIDL_STUBLESS_PROXY_INFO IAdviseSink2_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &IAdviseSink2_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const STUB_THUNK IAdviseSink2_StubThunkTable[] = 
    {
    IAdviseSink_RemoteOnDataChange_Thunk,
    IAdviseSink_RemoteOnViewChange_Thunk,
    IAdviseSink_RemoteOnRename_Thunk,
    IAdviseSink_RemoteOnSave_Thunk,
    IAdviseSink_RemoteOnClose_Thunk,
    IAdviseSink2_RemoteOnLinkSrcChange_Thunk
    };

static const MIDL_SERVER_INFO IAdviseSink2_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &IAdviseSink2_FormatStringOffsetTable[-3],
    &IAdviseSink2_StubThunkTable[-3],
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(9) _IAdviseSink2ProxyVtbl = 
{
    &IAdviseSink2_ProxyInfo,
    &IID_IAdviseSink2,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    IAdviseSink_OnDataChange_Proxy ,
    IAdviseSink_OnViewChange_Proxy ,
    IAdviseSink_OnRename_Proxy ,
    IAdviseSink_OnSave_Proxy ,
    IAdviseSink_OnClose_Proxy ,
    IAdviseSink2_OnLinkSrcChange_Proxy
};

const CInterfaceStubVtbl _IAdviseSink2StubVtbl =
{
    &IID_IAdviseSink2,
    &IAdviseSink2_ServerInfo,
    9,
    0, /* pure interpreted */
    CStdStubBuffer_METHODS
};


/* Object interface: AsyncIAdviseSink2, ver. 0.0,
   GUID={0x00000151,0x0000,0x0000,{0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x46}} */

#pragma code_seg(".orpc")
static const unsigned short AsyncIAdviseSink2_FormatStringOffsetTable[] =
    {
    4284,
    4328,
    4360,
    4404,
    4436,
    4474,
    4506,
    4538,
    4570,
    4602,
    4672,
    4710
    };

static const MIDL_STUBLESS_PROXY_INFO AsyncIAdviseSink2_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &AsyncIAdviseSink2_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const STUB_THUNK AsyncIAdviseSink2_StubThunkTable[] = 
    {
    AsyncIAdviseSink_Begin_RemoteOnDataChange_Thunk,
    AsyncIAdviseSink_Finish_RemoteOnDataChange_Thunk,
    AsyncIAdviseSink_Begin_RemoteOnViewChange_Thunk,
    AsyncIAdviseSink_Finish_RemoteOnViewChange_Thunk,
    AsyncIAdviseSink_Begin_RemoteOnRename_Thunk,
    AsyncIAdviseSink_Finish_RemoteOnRename_Thunk,
    AsyncIAdviseSink_Begin_RemoteOnSave_Thunk,
    AsyncIAdviseSink_Finish_RemoteOnSave_Thunk,
    AsyncIAdviseSink_Begin_RemoteOnClose_Thunk,
    AsyncIAdviseSink_Finish_RemoteOnClose_Thunk,
    AsyncIAdviseSink2_Begin_RemoteOnLinkSrcChange_Thunk,
    AsyncIAdviseSink2_Finish_RemoteOnLinkSrcChange_Thunk
    };

static const MIDL_SERVER_INFO AsyncIAdviseSink2_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &AsyncIAdviseSink2_FormatStringOffsetTable[-3],
    &AsyncIAdviseSink2_StubThunkTable[-3],
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(15) _AsyncIAdviseSink2ProxyVtbl = 
{
    &AsyncIAdviseSink2_ProxyInfo,
    &IID_AsyncIAdviseSink2,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    AsyncIAdviseSink_Begin_OnDataChange_Proxy ,
    AsyncIAdviseSink_Finish_OnDataChange_Proxy ,
    AsyncIAdviseSink_Begin_OnViewChange_Proxy ,
    AsyncIAdviseSink_Finish_OnViewChange_Proxy ,
    AsyncIAdviseSink_Begin_OnRename_Proxy ,
    AsyncIAdviseSink_Finish_OnRename_Proxy ,
    AsyncIAdviseSink_Begin_OnSave_Proxy ,
    AsyncIAdviseSink_Finish_OnSave_Proxy ,
    AsyncIAdviseSink_Begin_OnClose_Proxy ,
    AsyncIAdviseSink_Finish_OnClose_Proxy ,
    AsyncIAdviseSink2_Begin_OnLinkSrcChange_Proxy ,
    AsyncIAdviseSink2_Finish_OnLinkSrcChange_Proxy
};

CInterfaceStubVtbl _AsyncIAdviseSink2StubVtbl =
{
    &IID_AsyncIAdviseSink2,
    &AsyncIAdviseSink2_ServerInfo,
    15,
    0, /* pure interpreted */
    CStdAsyncStubBuffer_METHODS
};


/* Object interface: IDataObject, ver. 0.0,
   GUID={0x0000010e,0x0000,0x0000,{0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x46}} */

#pragma code_seg(".orpc")
static const unsigned short IDataObject_FormatStringOffsetTable[] =
    {
    4742,
    4786,
    4830,
    4868,
    4912,
    4962,
    5006,
    5062,
    5100
    };

static const MIDL_STUBLESS_PROXY_INFO IDataObject_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &IDataObject_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const STUB_THUNK IDataObject_StubThunkTable[] = 
    {
    IDataObject_RemoteGetData_Thunk,
    IDataObject_RemoteGetDataHere_Thunk,
    0,
    0,
    IDataObject_RemoteSetData_Thunk,
    0,
    0,
    0,
    0
    };

static const MIDL_SERVER_INFO IDataObject_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &IDataObject_FormatStringOffsetTable[-3],
    &IDataObject_StubThunkTable[-3],
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(12) _IDataObjectProxyVtbl = 
{
    &IDataObject_ProxyInfo,
    &IID_IDataObject,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    IDataObject_GetData_Proxy ,
    IDataObject_GetDataHere_Proxy ,
    (void *) (INT_PTR) -1 /* IDataObject::QueryGetData */ ,
    (void *) (INT_PTR) -1 /* IDataObject::GetCanonicalFormatEtc */ ,
    IDataObject_SetData_Proxy ,
    (void *) (INT_PTR) -1 /* IDataObject::EnumFormatEtc */ ,
    (void *) (INT_PTR) -1 /* IDataObject::DAdvise */ ,
    (void *) (INT_PTR) -1 /* IDataObject::DUnadvise */ ,
    (void *) (INT_PTR) -1 /* IDataObject::EnumDAdvise */
};

const CInterfaceStubVtbl _IDataObjectStubVtbl =
{
    &IID_IDataObject,
    &IDataObject_ServerInfo,
    12,
    0, /* pure interpreted */
    CStdStubBuffer_METHODS
};


/* Object interface: IDataAdviseHolder, ver. 0.0,
   GUID={0x00000110,0x0000,0x0000,{0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x46}} */


/* Object interface: IMessageFilter, ver. 0.0,
   GUID={0x00000016,0x0000,0x0000,{0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x46}} */


/* Object interface: IRpcChannelBuffer, ver. 0.0,
   GUID={0xD5F56B60,0x593B,0x101A,{0xB5,0x69,0x08,0x00,0x2B,0x2D,0xBF,0x7A}} */


/* Object interface: IRpcChannelBuffer2, ver. 0.0,
   GUID={0x594f31d0,0x7f19,0x11d0,{0xb1,0x94,0x00,0xa0,0xc9,0x0d,0xc8,0xbf}} */


/* Object interface: IAsyncRpcChannelBuffer, ver. 0.0,
   GUID={0xa5029fb6,0x3c34,0x11d1,{0x9c,0x99,0x00,0xc0,0x4f,0xb9,0x98,0xaa}} */


/* Object interface: IRpcChannelBuffer3, ver. 0.0,
   GUID={0x25B15600,0x0115,0x11d0,{0xBF,0x0D,0x00,0xAA,0x00,0xB8,0xDF,0xD2}} */


/* Object interface: IRpcProxyBuffer, ver. 0.0,
   GUID={0xD5F56A34,0x593B,0x101A,{0xB5,0x69,0x08,0x00,0x2B,0x2D,0xBF,0x7A}} */


/* Object interface: IRpcStubBuffer, ver. 0.0,
   GUID={0xD5F56AFC,0x593B,0x101A,{0xB5,0x69,0x08,0x00,0x2B,0x2D,0xBF,0x7A}} */


/* Object interface: IPSFactoryBuffer, ver. 0.0,
   GUID={0xD5F569D0,0x593B,0x101A,{0xB5,0x69,0x08,0x00,0x2B,0x2D,0xBF,0x7A}} */


/* Standard interface: __MIDL_itf_objidl_0049, ver. 0.0,
   GUID={0x00000000,0x0000,0x0000,{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}} */


/* Object interface: IChannelHook, ver. 0.0,
   GUID={0x1008c4a0,0x7613,0x11cf,{0x9a,0xf1,0x00,0x20,0xaf,0x6e,0x72,0xf4}} */


/* Standard interface: __MIDL_itf_objidl_0050, ver. 0.0,
   GUID={0x00000000,0x0000,0x0000,{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}} */


/* Object interface: IClientSecurity, ver. 0.0,
   GUID={0x0000013D,0x0000,0x0000,{0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x46}} */


/* Object interface: IServerSecurity, ver. 0.0,
   GUID={0x0000013E,0x0000,0x0000,{0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x46}} */


/* Object interface: IClassActivator, ver. 0.0,
   GUID={0x00000140,0x0000,0x0000,{0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x46}} */

#pragma code_seg(".orpc")
static const unsigned short IClassActivator_FormatStringOffsetTable[] =
    {
    5138
    };

static const MIDL_STUBLESS_PROXY_INFO IClassActivator_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &IClassActivator_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IClassActivator_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &IClassActivator_FormatStringOffsetTable[-3],
    0,
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(4) _IClassActivatorProxyVtbl = 
{
    &IClassActivator_ProxyInfo,
    &IID_IClassActivator,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    (void *) (INT_PTR) -1 /* IClassActivator::GetClassObject */
};

const CInterfaceStubVtbl _IClassActivatorStubVtbl =
{
    &IID_IClassActivator,
    &IClassActivator_ServerInfo,
    4,
    0, /* pure interpreted */
    CStdStubBuffer_METHODS
};


/* Object interface: IRpcOptions, ver. 0.0,
   GUID={0x00000144,0x0000,0x0000,{0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x46}} */


/* Standard interface: __MIDL_itf_objidl_0054, ver. 0.0,
   GUID={0x00000000,0x0000,0x0000,{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}} */


/* Object interface: IFillLockBytes, ver. 0.0,
   GUID={0x99caf010,0x415e,0x11cf,{0x88,0x14,0x00,0xaa,0x00,0xb5,0x69,0xf5}} */

#pragma code_seg(".orpc")
static const unsigned short IFillLockBytes_FormatStringOffsetTable[] =
    {
    5200,
    3718,
    5250,
    5288
    };

static const MIDL_STUBLESS_PROXY_INFO IFillLockBytes_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &IFillLockBytes_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const STUB_THUNK IFillLockBytes_StubThunkTable[] = 
    {
    IFillLockBytes_RemoteFillAppend_Thunk,
    IFillLockBytes_RemoteFillAt_Thunk,
    0,
    0
    };

static const MIDL_SERVER_INFO IFillLockBytes_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &IFillLockBytes_FormatStringOffsetTable[-3],
    &IFillLockBytes_StubThunkTable[-3],
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(7) _IFillLockBytesProxyVtbl = 
{
    &IFillLockBytes_ProxyInfo,
    &IID_IFillLockBytes,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    IFillLockBytes_FillAppend_Proxy ,
    IFillLockBytes_FillAt_Proxy ,
    (void *) (INT_PTR) -1 /* IFillLockBytes::SetFillSize */ ,
    (void *) (INT_PTR) -1 /* IFillLockBytes::Terminate */
};

const CInterfaceStubVtbl _IFillLockBytesStubVtbl =
{
    &IID_IFillLockBytes,
    &IFillLockBytes_ServerInfo,
    7,
    0, /* pure interpreted */
    CStdStubBuffer_METHODS
};


/* Object interface: IProgressNotify, ver. 0.0,
   GUID={0xa9d758a0,0x4617,0x11cf,{0x95,0xfc,0x00,0xaa,0x00,0x68,0x0d,0xb4}} */

#pragma code_seg(".orpc")
static const unsigned short IProgressNotify_FormatStringOffsetTable[] =
    {
    5326
    };

static const MIDL_STUBLESS_PROXY_INFO IProgressNotify_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &IProgressNotify_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IProgressNotify_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &IProgressNotify_FormatStringOffsetTable[-3],
    0,
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(4) _IProgressNotifyProxyVtbl = 
{
    &IProgressNotify_ProxyInfo,
    &IID_IProgressNotify,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    (void *) (INT_PTR) -1 /* IProgressNotify::OnProgress */
};

const CInterfaceStubVtbl _IProgressNotifyStubVtbl =
{
    &IID_IProgressNotify,
    &IProgressNotify_ServerInfo,
    4,
    0, /* pure interpreted */
    CStdStubBuffer_METHODS
};


/* Object interface: ILayoutStorage, ver. 0.0,
   GUID={0x0e6d4d90,0x6738,0x11cf,{0x96,0x08,0x00,0xaa,0x00,0x68,0x0d,0xb4}} */


/* Object interface: IBlockingLock, ver. 0.0,
   GUID={0x30f3d47a,0x6447,0x11d1,{0x8e,0x3c,0x00,0xc0,0x4f,0xb9,0x38,0x6d}} */

#pragma code_seg(".orpc")
static const unsigned short IBlockingLock_FormatStringOffsetTable[] =
    {
    5382,
    1022
    };

static const MIDL_STUBLESS_PROXY_INFO IBlockingLock_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &IBlockingLock_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IBlockingLock_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &IBlockingLock_FormatStringOffsetTable[-3],
    0,
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(5) _IBlockingLockProxyVtbl = 
{
    &IBlockingLock_ProxyInfo,
    &IID_IBlockingLock,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    (void *) (INT_PTR) -1 /* IBlockingLock::Lock */ ,
    (void *) (INT_PTR) -1 /* IBlockingLock::Unlock */
};

const CInterfaceStubVtbl _IBlockingLockStubVtbl =
{
    &IID_IBlockingLock,
    &IBlockingLock_ServerInfo,
    5,
    0, /* pure interpreted */
    CStdStubBuffer_METHODS
};


/* Object interface: ITimeAndNoticeControl, ver. 0.0,
   GUID={0xbc0bf6ae,0x8878,0x11d1,{0x83,0xe9,0x00,0xc0,0x4f,0xc2,0xc6,0xd4}} */

#pragma code_seg(".orpc")
static const unsigned short ITimeAndNoticeControl_FormatStringOffsetTable[] =
    {
    5420
    };

static const MIDL_STUBLESS_PROXY_INFO ITimeAndNoticeControl_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &ITimeAndNoticeControl_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO ITimeAndNoticeControl_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &ITimeAndNoticeControl_FormatStringOffsetTable[-3],
    0,
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(4) _ITimeAndNoticeControlProxyVtbl = 
{
    &ITimeAndNoticeControl_ProxyInfo,
    &IID_ITimeAndNoticeControl,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    (void *) (INT_PTR) -1 /* ITimeAndNoticeControl::SuppressChanges */
};

const CInterfaceStubVtbl _ITimeAndNoticeControlStubVtbl =
{
    &IID_ITimeAndNoticeControl,
    &ITimeAndNoticeControl_ServerInfo,
    4,
    0, /* pure interpreted */
    CStdStubBuffer_METHODS
};


/* Object interface: IOplockStorage, ver. 0.0,
   GUID={0x8d19c834,0x8879,0x11d1,{0x83,0xe9,0x00,0xc0,0x4f,0xc2,0xc6,0xd4}} */

#pragma code_seg(".orpc")
static const unsigned short IOplockStorage_FormatStringOffsetTable[] =
    {
    5464,
    5532
    };

static const MIDL_STUBLESS_PROXY_INFO IOplockStorage_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &IOplockStorage_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IOplockStorage_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &IOplockStorage_FormatStringOffsetTable[-3],
    0,
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(5) _IOplockStorageProxyVtbl = 
{
    &IOplockStorage_ProxyInfo,
    &IID_IOplockStorage,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    (void *) (INT_PTR) -1 /* IOplockStorage::CreateStorageEx */ ,
    (void *) (INT_PTR) -1 /* IOplockStorage::OpenStorageEx */
};

const CInterfaceStubVtbl _IOplockStorageStubVtbl =
{
    &IID_IOplockStorage,
    &IOplockStorage_ServerInfo,
    5,
    0, /* pure interpreted */
    CStdStubBuffer_METHODS
};


/* Object interface: ISurrogate, ver. 1.0,
   GUID={0x00000022,0x0000,0x0000,{0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x46}} */

#pragma code_seg(".orpc")
static const unsigned short ISurrogate_FormatStringOffsetTable[] =
    {
    5600,
    1022
    };

static const MIDL_STUBLESS_PROXY_INFO ISurrogate_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &ISurrogate_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO ISurrogate_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &ISurrogate_FormatStringOffsetTable[-3],
    0,
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(5) _ISurrogateProxyVtbl = 
{
    &ISurrogate_ProxyInfo,
    &IID_ISurrogate,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    (void *) (INT_PTR) -1 /* ISurrogate::LoadDllServer */ ,
    (void *) (INT_PTR) -1 /* ISurrogate::FreeSurrogate */
};

const CInterfaceStubVtbl _ISurrogateStubVtbl =
{
    &IID_ISurrogate,
    &ISurrogate_ServerInfo,
    5,
    0, /* pure interpreted */
    CStdStubBuffer_METHODS
};


/* Object interface: IGlobalInterfaceTable, ver. 0.0,
   GUID={0x00000146,0x0000,0x0000,{0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x46}} */


/* Object interface: IDirectWriterLock, ver. 0.0,
   GUID={0x0e6d4d92,0x6738,0x11cf,{0x96,0x08,0x00,0xaa,0x00,0x68,0x0d,0xb4}} */

#pragma code_seg(".orpc")
static const unsigned short IDirectWriterLock_FormatStringOffsetTable[] =
    {
    5382,
    1022,
    88
    };

static const MIDL_STUBLESS_PROXY_INFO IDirectWriterLock_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &IDirectWriterLock_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IDirectWriterLock_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &IDirectWriterLock_FormatStringOffsetTable[-3],
    0,
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(6) _IDirectWriterLockProxyVtbl = 
{
    &IDirectWriterLock_ProxyInfo,
    &IID_IDirectWriterLock,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    (void *) (INT_PTR) -1 /* IDirectWriterLock::WaitForWriteAccess */ ,
    (void *) (INT_PTR) -1 /* IDirectWriterLock::ReleaseWriteAccess */ ,
    (void *) (INT_PTR) -1 /* IDirectWriterLock::HaveWriteAccess */
};

const CInterfaceStubVtbl _IDirectWriterLockStubVtbl =
{
    &IID_IDirectWriterLock,
    &IDirectWriterLock_ServerInfo,
    6,
    0, /* pure interpreted */
    CStdStubBuffer_METHODS
};


/* Object interface: ISynchronize, ver. 0.0,
   GUID={0x00000030,0x0000,0x0000,{0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x46}} */

#pragma code_seg(".orpc")
static const unsigned short ISynchronize_FormatStringOffsetTable[] =
    {
    5420,
    1022,
    88
    };

static const MIDL_STUBLESS_PROXY_INFO ISynchronize_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &ISynchronize_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO ISynchronize_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &ISynchronize_FormatStringOffsetTable[-3],
    0,
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(6) _ISynchronizeProxyVtbl = 
{
    &ISynchronize_ProxyInfo,
    &IID_ISynchronize,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    (void *) (INT_PTR) -1 /* ISynchronize::Wait */ ,
    (void *) (INT_PTR) -1 /* ISynchronize::Signal */ ,
    (void *) (INT_PTR) -1 /* ISynchronize::Reset */
};

const CInterfaceStubVtbl _ISynchronizeStubVtbl =
{
    &IID_ISynchronize,
    &ISynchronize_ServerInfo,
    6,
    0, /* pure interpreted */
    CStdStubBuffer_METHODS
};


/* Object interface: ISynchronizeHandle, ver. 0.0,
   GUID={0x00000031,0x0000,0x0000,{0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x46}} */


/* Object interface: ISynchronizeEvent, ver. 0.0,
   GUID={0x00000032,0x0000,0x0000,{0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x46}} */


/* Object interface: ISynchronizeContainer, ver. 0.0,
   GUID={0x00000033,0x0000,0x0000,{0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x46}} */


/* Object interface: ISynchronizeMutex, ver. 0.0,
   GUID={0x00000025,0x0000,0x0000,{0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x46}} */


/* Object interface: ICancelMethodCalls, ver. 0.0,
   GUID={0x00000029,0x0000,0x0000,{0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x46}} */


/* Object interface: IAsyncManager, ver. 0.0,
   GUID={0x0000002A,0x0000,0x0000,{0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x46}} */


/* Object interface: ICallFactory, ver. 0.0,
   GUID={0x1c733a30,0x2a1c,0x11ce,{0xad,0xe5,0x00,0xaa,0x00,0x44,0x77,0x3d}} */


/* Object interface: IRpcHelper, ver. 0.0,
   GUID={0x00000149,0x0000,0x0000,{0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x46}} */


/* Object interface: IReleaseMarshalBuffers, ver. 0.0,
   GUID={0xeb0cb9e8,0x7996,0x11d2,{0x87,0x2e,0x00,0x00,0xf8,0x08,0x08,0x59}} */


/* Object interface: IWaitMultiple, ver. 0.0,
   GUID={0x0000002B,0x0000,0x0000,{0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x46}} */


/* Object interface: IUrlMon, ver. 0.0,
   GUID={0x00000026,0x0000,0x0000,{0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x46}} */

#pragma code_seg(".orpc")
static const unsigned short IUrlMon_FormatStringOffsetTable[] =
    {
    5638
    };

static const MIDL_STUBLESS_PROXY_INFO IUrlMon_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &IUrlMon_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IUrlMon_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &IUrlMon_FormatStringOffsetTable[-3],
    0,
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(4) _IUrlMonProxyVtbl = 
{
    &IUrlMon_ProxyInfo,
    &IID_IUrlMon,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    (void *) (INT_PTR) -1 /* IUrlMon::AsyncGetClassBits */
};

const CInterfaceStubVtbl _IUrlMonStubVtbl =
{
    &IID_IUrlMon,
    &IUrlMon_ServerInfo,
    4,
    0, /* pure interpreted */
    CStdStubBuffer_METHODS
};


/* Object interface: IForegroundTransfer, ver. 0.0,
   GUID={0x00000145,0x0000,0x0000,{0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x46}} */


/* Object interface: IPipeByte, ver. 0.0,
   GUID={0xDB2F3ACA,0x2F86,0x11d1,{0x8E,0x04,0x00,0xC0,0x4F,0xB9,0x98,0x9A}} */

#pragma code_seg(".orpc")
static const unsigned short IPipeByte_FormatStringOffsetTable[] =
    {
    2026,
    5730
    };

static const MIDL_STUBLESS_PROXY_INFO IPipeByte_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &IPipeByte_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IPipeByte_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &IPipeByte_FormatStringOffsetTable[-3],
    0,
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(5) _IPipeByteProxyVtbl = 
{
    &IPipeByte_ProxyInfo,
    &IID_IPipeByte,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    (void *) (INT_PTR) -1 /* IPipeByte::Pull */ ,
    (void *) (INT_PTR) -1 /* IPipeByte::Push */
};

const CInterfaceStubVtbl _IPipeByteStubVtbl =
{
    &IID_IPipeByte,
    &IPipeByte_ServerInfo,
    5,
    0, /* pure interpreted */
    CStdStubBuffer_METHODS
};


/* Object interface: AsyncIPipeByte, ver. 0.0,
   GUID={0xDB2F3ACB,0x2F86,0x11d1,{0x8E,0x04,0x00,0xC0,0x4F,0xB9,0x98,0x9A}} */

#pragma code_seg(".orpc")
static const unsigned short AsyncIPipeByte_FormatStringOffsetTable[] =
    {
    5774,
    5812,
    5856,
    4404
    };

static const MIDL_STUBLESS_PROXY_INFO AsyncIPipeByte_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &AsyncIPipeByte_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO AsyncIPipeByte_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &AsyncIPipeByte_FormatStringOffsetTable[-3],
    0,
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(7) _AsyncIPipeByteProxyVtbl = 
{
    &AsyncIPipeByte_ProxyInfo,
    &IID_AsyncIPipeByte,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    (void *) (INT_PTR) -1 /* AsyncIPipeByte::Begin_Pull */ ,
    (void *) (INT_PTR) -1 /* AsyncIPipeByte::Finish_Pull */ ,
    (void *) (INT_PTR) -1 /* AsyncIPipeByte::Begin_Push */ ,
    (void *) (INT_PTR) -1 /* AsyncIPipeByte::Finish_Push */
};

CInterfaceStubVtbl _AsyncIPipeByteStubVtbl =
{
    &IID_AsyncIPipeByte,
    &AsyncIPipeByte_ServerInfo,
    7,
    0, /* pure interpreted */
    CStdAsyncStubBuffer_METHODS
};


/* Object interface: IPipeLong, ver. 0.0,
   GUID={0xDB2F3ACC,0x2F86,0x11d1,{0x8E,0x04,0x00,0xC0,0x4F,0xB9,0x98,0x9A}} */

#pragma code_seg(".orpc")
static const unsigned short IPipeLong_FormatStringOffsetTable[] =
    {
    5900,
    5950
    };

static const MIDL_STUBLESS_PROXY_INFO IPipeLong_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &IPipeLong_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IPipeLong_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &IPipeLong_FormatStringOffsetTable[-3],
    0,
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(5) _IPipeLongProxyVtbl = 
{
    &IPipeLong_ProxyInfo,
    &IID_IPipeLong,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    (void *) (INT_PTR) -1 /* IPipeLong::Pull */ ,
    (void *) (INT_PTR) -1 /* IPipeLong::Push */
};

const CInterfaceStubVtbl _IPipeLongStubVtbl =
{
    &IID_IPipeLong,
    &IPipeLong_ServerInfo,
    5,
    0, /* pure interpreted */
    CStdStubBuffer_METHODS
};


/* Object interface: AsyncIPipeLong, ver. 0.0,
   GUID={0xDB2F3ACD,0x2F86,0x11d1,{0x8E,0x04,0x00,0xC0,0x4F,0xB9,0x98,0x9A}} */

#pragma code_seg(".orpc")
static const unsigned short AsyncIPipeLong_FormatStringOffsetTable[] =
    {
    5774,
    5994,
    6038,
    4404
    };

static const MIDL_STUBLESS_PROXY_INFO AsyncIPipeLong_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &AsyncIPipeLong_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO AsyncIPipeLong_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &AsyncIPipeLong_FormatStringOffsetTable[-3],
    0,
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(7) _AsyncIPipeLongProxyVtbl = 
{
    &AsyncIPipeLong_ProxyInfo,
    &IID_AsyncIPipeLong,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    (void *) (INT_PTR) -1 /* AsyncIPipeLong::Begin_Pull */ ,
    (void *) (INT_PTR) -1 /* AsyncIPipeLong::Finish_Pull */ ,
    (void *) (INT_PTR) -1 /* AsyncIPipeLong::Begin_Push */ ,
    (void *) (INT_PTR) -1 /* AsyncIPipeLong::Finish_Push */
};

CInterfaceStubVtbl _AsyncIPipeLongStubVtbl =
{
    &IID_AsyncIPipeLong,
    &AsyncIPipeLong_ServerInfo,
    7,
    0, /* pure interpreted */
    CStdAsyncStubBuffer_METHODS
};


/* Object interface: IPipeDouble, ver. 0.0,
   GUID={0xDB2F3ACE,0x2F86,0x11d1,{0x8E,0x04,0x00,0xC0,0x4F,0xB9,0x98,0x9A}} */

#pragma code_seg(".orpc")
static const unsigned short IPipeDouble_FormatStringOffsetTable[] =
    {
    6082,
    6132
    };

static const MIDL_STUBLESS_PROXY_INFO IPipeDouble_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &IPipeDouble_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IPipeDouble_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &IPipeDouble_FormatStringOffsetTable[-3],
    0,
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(5) _IPipeDoubleProxyVtbl = 
{
    &IPipeDouble_ProxyInfo,
    &IID_IPipeDouble,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    (void *) (INT_PTR) -1 /* IPipeDouble::Pull */ ,
    (void *) (INT_PTR) -1 /* IPipeDouble::Push */
};

const CInterfaceStubVtbl _IPipeDoubleStubVtbl =
{
    &IID_IPipeDouble,
    &IPipeDouble_ServerInfo,
    5,
    0, /* pure interpreted */
    CStdStubBuffer_METHODS
};


/* Object interface: AsyncIPipeDouble, ver. 0.0,
   GUID={0xDB2F3ACF,0x2F86,0x11d1,{0x8E,0x04,0x00,0xC0,0x4F,0xB9,0x98,0x9A}} */

#pragma code_seg(".orpc")
static const unsigned short AsyncIPipeDouble_FormatStringOffsetTable[] =
    {
    5774,
    6176,
    6220,
    4404
    };

static const MIDL_STUBLESS_PROXY_INFO AsyncIPipeDouble_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &AsyncIPipeDouble_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO AsyncIPipeDouble_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &AsyncIPipeDouble_FormatStringOffsetTable[-3],
    0,
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(7) _AsyncIPipeDoubleProxyVtbl = 
{
    &AsyncIPipeDouble_ProxyInfo,
    &IID_AsyncIPipeDouble,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    (void *) (INT_PTR) -1 /* AsyncIPipeDouble::Begin_Pull */ ,
    (void *) (INT_PTR) -1 /* AsyncIPipeDouble::Finish_Pull */ ,
    (void *) (INT_PTR) -1 /* AsyncIPipeDouble::Begin_Push */ ,
    (void *) (INT_PTR) -1 /* AsyncIPipeDouble::Finish_Push */
};

CInterfaceStubVtbl _AsyncIPipeDoubleStubVtbl =
{
    &IID_AsyncIPipeDouble,
    &AsyncIPipeDouble_ServerInfo,
    7,
    0, /* pure interpreted */
    CStdAsyncStubBuffer_METHODS
};


/* Object interface: IThumbnailExtractor, ver. 0.0,
   GUID={0x969dc708,0x5c76,0x11d1,{0x8d,0x86,0x00,0x00,0xf8,0x04,0xb0,0x57}} */

#pragma code_seg(".orpc")
static const unsigned short IThumbnailExtractor_FormatStringOffsetTable[] =
    {
    6264,
    6332
    };

static const MIDL_STUBLESS_PROXY_INFO IThumbnailExtractor_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &IThumbnailExtractor_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IThumbnailExtractor_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &IThumbnailExtractor_FormatStringOffsetTable[-3],
    0,
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(5) _IThumbnailExtractorProxyVtbl = 
{
    &IThumbnailExtractor_ProxyInfo,
    &IID_IThumbnailExtractor,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    (void *) (INT_PTR) -1 /* IThumbnailExtractor::ExtractThumbnail */ ,
    (void *) (INT_PTR) -1 /* IThumbnailExtractor::OnFileUpdated */
};

const CInterfaceStubVtbl _IThumbnailExtractorStubVtbl =
{
    &IID_IThumbnailExtractor,
    &IThumbnailExtractor_ServerInfo,
    5,
    0, /* pure interpreted */
    CStdStubBuffer_METHODS
};


/* Object interface: IDummyHICONIncluder, ver. 0.0,
   GUID={0x947990de,0xcc28,0x11d2,{0xa0,0xf7,0x00,0x80,0x5f,0x85,0x8f,0xb1}} */

#pragma code_seg(".orpc")
static const unsigned short IDummyHICONIncluder_FormatStringOffsetTable[] =
    {
    6370
    };

static const MIDL_STUBLESS_PROXY_INFO IDummyHICONIncluder_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &IDummyHICONIncluder_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IDummyHICONIncluder_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &IDummyHICONIncluder_FormatStringOffsetTable[-3],
    0,
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(4) _IDummyHICONIncluderProxyVtbl = 
{
    &IDummyHICONIncluder_ProxyInfo,
    &IID_IDummyHICONIncluder,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    (void *) (INT_PTR) -1 /* IDummyHICONIncluder::Dummy */
};

const CInterfaceStubVtbl _IDummyHICONIncluderStubVtbl =
{
    &IID_IDummyHICONIncluder,
    &IDummyHICONIncluder_ServerInfo,
    4,
    0, /* pure interpreted */
    CStdStubBuffer_METHODS
};


/* Standard interface: __MIDL_itf_objidl_0081, ver. 0.0,
   GUID={0x00000000,0x0000,0x0000,{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}} */

static const MIDL_STUB_DESC Object_StubDesc = 
    {
    0,
    NdrOleAllocate,
    NdrOleFree,
    0,
    0,
    0,
    ExprEvalRoutines,
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

const CInterfaceProxyVtbl * _objidl_ProxyVtblList[] = 
{
    ( CInterfaceProxyVtbl *) &_IEnumUnknownProxyVtbl,
    ( CInterfaceProxyVtbl *) &_IEnumStringProxyVtbl,
    ( CInterfaceProxyVtbl *) &_IEnumMonikerProxyVtbl,
    ( CInterfaceProxyVtbl *) &_IEnumFORMATETCProxyVtbl,
    ( CInterfaceProxyVtbl *) &_IEnumSTATDATAProxyVtbl,
    ( CInterfaceProxyVtbl *) &_IThumbnailExtractorProxyVtbl,
    ( CInterfaceProxyVtbl *) &_IPersistStreamProxyVtbl,
    ( CInterfaceProxyVtbl *) &_ILockBytesProxyVtbl,
    ( CInterfaceProxyVtbl *) &_IPersistStorageProxyVtbl,
    ( CInterfaceProxyVtbl *) &_IStorageProxyVtbl,
    ( CInterfaceProxyVtbl *) &_IPersistFileProxyVtbl,
    ( CInterfaceProxyVtbl *) &_IStreamProxyVtbl,
    ( CInterfaceProxyVtbl *) &_IPersistProxyVtbl,
    ( CInterfaceProxyVtbl *) &_IEnumSTATSTGProxyVtbl,
    ( CInterfaceProxyVtbl *) &_IBindCtxProxyVtbl,
    ( CInterfaceProxyVtbl *) &_IDataObjectProxyVtbl,
    ( CInterfaceProxyVtbl *) &_IMonikerProxyVtbl,
    ( CInterfaceProxyVtbl *) &_IAdviseSinkProxyVtbl,
    ( CInterfaceProxyVtbl *) &_IRunningObjectTableProxyVtbl,
    ( CInterfaceProxyVtbl *) &_IFillLockBytesProxyVtbl,
    ( CInterfaceProxyVtbl *) &_IRootStorageProxyVtbl,
    ( CInterfaceProxyVtbl *) &_ISurrogateProxyVtbl,
    ( CInterfaceProxyVtbl *) &_IAdviseSink2ProxyVtbl,
    ( CInterfaceProxyVtbl *) &_IUrlMonProxyVtbl,
    ( CInterfaceProxyVtbl *) &_IRunnableObjectProxyVtbl,
    ( CInterfaceProxyVtbl *) &_ISynchronizeProxyVtbl,
    ( CInterfaceProxyVtbl *) &_ISequentialStreamProxyVtbl,
    ( CInterfaceProxyVtbl *) &_IOplockStorageProxyVtbl,
    ( CInterfaceProxyVtbl *) &_IClassActivatorProxyVtbl,
    ( CInterfaceProxyVtbl *) &_AsyncIAdviseSinkProxyVtbl,
    ( CInterfaceProxyVtbl *) &_AsyncIAdviseSink2ProxyVtbl,
    ( CInterfaceProxyVtbl *) &_IBlockingLockProxyVtbl,
    ( CInterfaceProxyVtbl *) &_IDirectWriterLockProxyVtbl,
    ( CInterfaceProxyVtbl *) &_IProgressNotifyProxyVtbl,
    ( CInterfaceProxyVtbl *) &_ITimeAndNoticeControlProxyVtbl,
    ( CInterfaceProxyVtbl *) &_IROTDataProxyVtbl,
    ( CInterfaceProxyVtbl *) &_IPipeByteProxyVtbl,
    ( CInterfaceProxyVtbl *) &_AsyncIPipeByteProxyVtbl,
    ( CInterfaceProxyVtbl *) &_IPipeLongProxyVtbl,
    ( CInterfaceProxyVtbl *) &_AsyncIPipeLongProxyVtbl,
    ( CInterfaceProxyVtbl *) &_IPipeDoubleProxyVtbl,
    ( CInterfaceProxyVtbl *) &_AsyncIPipeDoubleProxyVtbl,
    ( CInterfaceProxyVtbl *) &_IDummyHICONIncluderProxyVtbl,
    0
};

const CInterfaceStubVtbl * _objidl_StubVtblList[] = 
{
    ( CInterfaceStubVtbl *) &_IEnumUnknownStubVtbl,
    ( CInterfaceStubVtbl *) &_IEnumStringStubVtbl,
    ( CInterfaceStubVtbl *) &_IEnumMonikerStubVtbl,
    ( CInterfaceStubVtbl *) &_IEnumFORMATETCStubVtbl,
    ( CInterfaceStubVtbl *) &_IEnumSTATDATAStubVtbl,
    ( CInterfaceStubVtbl *) &_IThumbnailExtractorStubVtbl,
    ( CInterfaceStubVtbl *) &_IPersistStreamStubVtbl,
    ( CInterfaceStubVtbl *) &_ILockBytesStubVtbl,
    ( CInterfaceStubVtbl *) &_IPersistStorageStubVtbl,
    ( CInterfaceStubVtbl *) &_IStorageStubVtbl,
    ( CInterfaceStubVtbl *) &_IPersistFileStubVtbl,
    ( CInterfaceStubVtbl *) &_IStreamStubVtbl,
    ( CInterfaceStubVtbl *) &_IPersistStubVtbl,
    ( CInterfaceStubVtbl *) &_IEnumSTATSTGStubVtbl,
    ( CInterfaceStubVtbl *) &_IBindCtxStubVtbl,
    ( CInterfaceStubVtbl *) &_IDataObjectStubVtbl,
    ( CInterfaceStubVtbl *) &_IMonikerStubVtbl,
    ( CInterfaceStubVtbl *) &_IAdviseSinkStubVtbl,
    ( CInterfaceStubVtbl *) &_IRunningObjectTableStubVtbl,
    ( CInterfaceStubVtbl *) &_IFillLockBytesStubVtbl,
    ( CInterfaceStubVtbl *) &_IRootStorageStubVtbl,
    ( CInterfaceStubVtbl *) &_ISurrogateStubVtbl,
    ( CInterfaceStubVtbl *) &_IAdviseSink2StubVtbl,
    ( CInterfaceStubVtbl *) &_IUrlMonStubVtbl,
    ( CInterfaceStubVtbl *) &_IRunnableObjectStubVtbl,
    ( CInterfaceStubVtbl *) &_ISynchronizeStubVtbl,
    ( CInterfaceStubVtbl *) &_ISequentialStreamStubVtbl,
    ( CInterfaceStubVtbl *) &_IOplockStorageStubVtbl,
    ( CInterfaceStubVtbl *) &_IClassActivatorStubVtbl,
    ( CInterfaceStubVtbl *) &_AsyncIAdviseSinkStubVtbl,
    ( CInterfaceStubVtbl *) &_AsyncIAdviseSink2StubVtbl,
    ( CInterfaceStubVtbl *) &_IBlockingLockStubVtbl,
    ( CInterfaceStubVtbl *) &_IDirectWriterLockStubVtbl,
    ( CInterfaceStubVtbl *) &_IProgressNotifyStubVtbl,
    ( CInterfaceStubVtbl *) &_ITimeAndNoticeControlStubVtbl,
    ( CInterfaceStubVtbl *) &_IROTDataStubVtbl,
    ( CInterfaceStubVtbl *) &_IPipeByteStubVtbl,
    ( CInterfaceStubVtbl *) &_AsyncIPipeByteStubVtbl,
    ( CInterfaceStubVtbl *) &_IPipeLongStubVtbl,
    ( CInterfaceStubVtbl *) &_AsyncIPipeLongStubVtbl,
    ( CInterfaceStubVtbl *) &_IPipeDoubleStubVtbl,
    ( CInterfaceStubVtbl *) &_AsyncIPipeDoubleStubVtbl,
    ( CInterfaceStubVtbl *) &_IDummyHICONIncluderStubVtbl,
    0
};

PCInterfaceName const _objidl_InterfaceNamesList[] = 
{
    "IEnumUnknown",
    "IEnumString",
    "IEnumMoniker",
    "IEnumFORMATETC",
    "IEnumSTATDATA",
    "IThumbnailExtractor",
    "IPersistStream",
    "ILockBytes",
    "IPersistStorage",
    "IStorage",
    "IPersistFile",
    "IStream",
    "IPersist",
    "IEnumSTATSTG",
    "IBindCtx",
    "IDataObject",
    "IMoniker",
    "IAdviseSink",
    "IRunningObjectTable",
    "IFillLockBytes",
    "IRootStorage",
    "ISurrogate",
    "IAdviseSink2",
    "IUrlMon",
    "IRunnableObject",
    "ISynchronize",
    "ISequentialStream",
    "IOplockStorage",
    "IClassActivator",
    "AsyncIAdviseSink",
    "AsyncIAdviseSink2",
    "IBlockingLock",
    "IDirectWriterLock",
    "IProgressNotify",
    "ITimeAndNoticeControl",
    "IROTData",
    "IPipeByte",
    "AsyncIPipeByte",
    "IPipeLong",
    "AsyncIPipeLong",
    "IPipeDouble",
    "AsyncIPipeDouble",
    "IDummyHICONIncluder",
    0
};


#define _objidl_CHECK_IID(n)	IID_GENERIC_CHECK_IID( _objidl, pIID, n)

int __stdcall _objidl_IID_Lookup( const IID * pIID, int * pIndex )
{
    IID_BS_LOOKUP_SETUP

    IID_BS_LOOKUP_INITIAL_TEST( _objidl, 43, 32 )
    IID_BS_LOOKUP_NEXT_TEST( _objidl, 16 )
    IID_BS_LOOKUP_NEXT_TEST( _objidl, 8 )
    IID_BS_LOOKUP_NEXT_TEST( _objidl, 4 )
    IID_BS_LOOKUP_NEXT_TEST( _objidl, 2 )
    IID_BS_LOOKUP_NEXT_TEST( _objidl, 1 )
    IID_BS_LOOKUP_RETURN_RESULT( _objidl, 43, *pIndex )
    
}

static const IID * _AsyncInterfaceTable[] = 
{
    (IID*) 0,
    (IID*) 0,
    (IID*) 0,
    (IID*) 0,
    (IID*) 0,
    (IID*) 0,
    (IID*) 0,
    (IID*) 0,
    (IID*) 0,
    (IID*) 0,
    (IID*) 0,
    (IID*) 0,
    (IID*) 0,
    (IID*) 0,
    (IID*) 0,
    (IID*) 0,
    (IID*) 0,
    (IID*) &IID_AsyncIAdviseSink,
    (IID*) 0,
    (IID*) 0,
    (IID*) 0,
    (IID*) 0,
    (IID*) &IID_AsyncIAdviseSink2,
    (IID*) 0,
    (IID*) 0,
    (IID*) 0,
    (IID*) 0,
    (IID*) 0,
    (IID*) 0,
    (IID*) -1,
    (IID*) -1,
    (IID*) 0,
    (IID*) 0,
    (IID*) 0,
    (IID*) 0,
    (IID*) 0,
    (IID*) &IID_AsyncIPipeByte,
    (IID*) -1,
    (IID*) &IID_AsyncIPipeLong,
    (IID*) -1,
    (IID*) &IID_AsyncIPipeDouble,
    (IID*) -1,
    (IID*) 0,
    (IID*) 0
};

const ExtendedProxyFileInfo objidl_ProxyFileInfo = 
{
    (PCInterfaceProxyVtblList *) & _objidl_ProxyVtblList,
    (PCInterfaceStubVtblList *) & _objidl_StubVtblList,
    (const PCInterfaceName * ) & _objidl_InterfaceNamesList,
    0, // no delegation
    & _objidl_IID_Lookup, 
    43,
    6,
    (const IID**) &_AsyncInterfaceTable[0], /* table of [async_uuid] interfaces */
    0, /* Filler1 */
    0, /* Filler2 */
    0  /* Filler3 */
};


#endif /* defined(_M_IA64) || defined(_M_AMD64)*/

