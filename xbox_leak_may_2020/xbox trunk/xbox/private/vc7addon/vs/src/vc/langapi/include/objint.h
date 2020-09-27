
#pragma warning( disable: 4049 )  /* more than 64k source lines */

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


 /* File created by MIDL compiler version 6.00.0342 */
/* at Wed Mar 14 14:21:50 2001
 */
/* Compiler settings for objint.idl:
    Os, W1, Zp8, env=Win32 (32b run)
    protocol : dce , ms_ext, c_ext
    error checks: allocation ref bounds_check enum stub_data 
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

#ifndef __objint_h__
#define __objint_h__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

/* Forward Declarations */ 

#ifndef __ILinkDataRO_FWD_DEFINED__
#define __ILinkDataRO_FWD_DEFINED__
typedef interface ILinkDataRO ILinkDataRO;
#endif 	/* __ILinkDataRO_FWD_DEFINED__ */


#ifndef __ILinkDataRW_FWD_DEFINED__
#define __ILinkDataRW_FWD_DEFINED__
typedef interface ILinkDataRW ILinkDataRW;
#endif 	/* __ILinkDataRW_FWD_DEFINED__ */


#ifndef __IPublicSym_FWD_DEFINED__
#define __IPublicSym_FWD_DEFINED__
typedef interface IPublicSym IPublicSym;
#endif 	/* __IPublicSym_FWD_DEFINED__ */


#ifndef __IEnumPublics_FWD_DEFINED__
#define __IEnumPublics_FWD_DEFINED__
typedef interface IEnumPublics IEnumPublics;
#endif 	/* __IEnumPublics_FWD_DEFINED__ */


#ifndef __IObjFile_FWD_DEFINED__
#define __IObjFile_FWD_DEFINED__
typedef interface IObjFile IObjFile;
#endif 	/* __IObjFile_FWD_DEFINED__ */


#ifndef __IObjHandler_FWD_DEFINED__
#define __IObjHandler_FWD_DEFINED__
typedef interface IObjHandler IObjHandler;
#endif 	/* __IObjHandler_FWD_DEFINED__ */


#ifndef __IObjectContrib_FWD_DEFINED__
#define __IObjectContrib_FWD_DEFINED__
typedef interface IObjectContrib IObjectContrib;
#endif 	/* __IObjectContrib_FWD_DEFINED__ */


#ifndef __IEnumContrib_FWD_DEFINED__
#define __IEnumContrib_FWD_DEFINED__
typedef interface IEnumContrib IEnumContrib;
#endif 	/* __IEnumContrib_FWD_DEFINED__ */


#ifndef __ICOFFAuxSym_FWD_DEFINED__
#define __ICOFFAuxSym_FWD_DEFINED__
typedef interface ICOFFAuxSym ICOFFAuxSym;
#endif 	/* __ICOFFAuxSym_FWD_DEFINED__ */


#ifndef __IEnumAuxSym_FWD_DEFINED__
#define __IEnumAuxSym_FWD_DEFINED__
typedef interface IEnumAuxSym IEnumAuxSym;
#endif 	/* __IEnumAuxSym_FWD_DEFINED__ */


#ifndef __ICOFFSymRO_FWD_DEFINED__
#define __ICOFFSymRO_FWD_DEFINED__
typedef interface ICOFFSymRO ICOFFSymRO;
#endif 	/* __ICOFFSymRO_FWD_DEFINED__ */


#ifndef __ICOFFSymRW_FWD_DEFINED__
#define __ICOFFSymRW_FWD_DEFINED__
typedef interface ICOFFSymRW ICOFFSymRW;
#endif 	/* __ICOFFSymRW_FWD_DEFINED__ */


#ifndef __IEnumCOFFSymRO_FWD_DEFINED__
#define __IEnumCOFFSymRO_FWD_DEFINED__
typedef interface IEnumCOFFSymRO IEnumCOFFSymRO;
#endif 	/* __IEnumCOFFSymRO_FWD_DEFINED__ */


#ifndef __IEnumCOFFSymRW_FWD_DEFINED__
#define __IEnumCOFFSymRW_FWD_DEFINED__
typedef interface IEnumCOFFSymRW IEnumCOFFSymRW;
#endif 	/* __IEnumCOFFSymRW_FWD_DEFINED__ */


#ifndef __ICOFFSymTabRO_FWD_DEFINED__
#define __ICOFFSymTabRO_FWD_DEFINED__
typedef interface ICOFFSymTabRO ICOFFSymTabRO;
#endif 	/* __ICOFFSymTabRO_FWD_DEFINED__ */


#ifndef __ICOFFSymTabRW_FWD_DEFINED__
#define __ICOFFSymTabRW_FWD_DEFINED__
typedef interface ICOFFSymTabRW ICOFFSymTabRW;
#endif 	/* __ICOFFSymTabRW_FWD_DEFINED__ */


#ifndef __ICOFFObj_FWD_DEFINED__
#define __ICOFFObj_FWD_DEFINED__
typedef interface ICOFFObj ICOFFObj;
#endif 	/* __ICOFFObj_FWD_DEFINED__ */


#ifndef __ILinkError_FWD_DEFINED__
#define __ILinkError_FWD_DEFINED__
typedef interface ILinkError ILinkError;
#endif 	/* __ILinkError_FWD_DEFINED__ */


#ifndef __ILinkNotify_FWD_DEFINED__
#define __ILinkNotify_FWD_DEFINED__
typedef interface ILinkNotify ILinkNotify;
#endif 	/* __ILinkNotify_FWD_DEFINED__ */


/* header files for imported files */
#include "objidl.h"

#ifdef __cplusplus
extern "C"{
#endif 

void * __RPC_USER MIDL_user_allocate(size_t);
void __RPC_USER MIDL_user_free( void * ); 

/* interface __MIDL_itf_objint_0000 */
/* [local] */ 

typedef /* [public][public] */ 
enum __MIDL___MIDL_itf_objint_0000_0001
    {	psymtNil	= 0,
	psymtRef	= psymtNil + 1,
	psymtDef	= psymtRef + 1,
	psymtAbs	= psymtDef + 1,
	psymtCom	= psymtAbs + 1,
	psymtAlias	= psymtCom + 1,
	psymtWeak	= psymtAlias + 1,
	psymtLazy	= psymtWeak + 1
    } 	PSYMT;

typedef /* [public][public] */ 
enum __MIDL___MIDL_itf_objint_0000_0002
    {	oftCIL	= 0,
	oftCOFF	= oftCIL + 1,
	oftMSILPE	= oftCOFF + 1
    } 	OFT;

typedef DWORDLONG OBJTK;

typedef /* [public] */ 
enum __MIDL___MIDL_itf_objint_0000_0003
    {	objtktNil	= 0
    } 	OBJTKT;

typedef DWORDLONG ISYM;

typedef DWORD ICOFFSYM;



extern RPC_IF_HANDLE __MIDL_itf_objint_0000_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_objint_0000_v0_0_s_ifspec;

#ifndef __ILinkDataRO_INTERFACE_DEFINED__
#define __ILinkDataRO_INTERFACE_DEFINED__

/* interface ILinkDataRO */
/* [unique][uuid][object] */ 

typedef /* [unique] */ ILinkDataRO *PLinkDataRO;


EXTERN_C const IID IID_ILinkDataRO;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("0CE9B320-AE0F-11d1-A719-0060083E8C78")
    ILinkDataRO : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE GetDataPointer( 
            /* [out] */ BYTE **__MIDL_0004,
            /* [out] */ int *__MIDL_0005) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct ILinkDataROVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            ILinkDataRO * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            ILinkDataRO * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            ILinkDataRO * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetDataPointer )( 
            ILinkDataRO * This,
            /* [out] */ BYTE **__MIDL_0004,
            /* [out] */ int *__MIDL_0005);
        
        END_INTERFACE
    } ILinkDataROVtbl;

    interface ILinkDataRO
    {
        CONST_VTBL struct ILinkDataROVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define ILinkDataRO_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define ILinkDataRO_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define ILinkDataRO_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define ILinkDataRO_GetDataPointer(This,__MIDL_0004,__MIDL_0005)	\
    (This)->lpVtbl -> GetDataPointer(This,__MIDL_0004,__MIDL_0005)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE ILinkDataRO_GetDataPointer_Proxy( 
    ILinkDataRO * This,
    /* [out] */ BYTE **__MIDL_0004,
    /* [out] */ int *__MIDL_0005);


void __RPC_STUB ILinkDataRO_GetDataPointer_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __ILinkDataRO_INTERFACE_DEFINED__ */


#ifndef __ILinkDataRW_INTERFACE_DEFINED__
#define __ILinkDataRW_INTERFACE_DEFINED__

/* interface ILinkDataRW */
/* [unique][uuid][object] */ 

typedef /* [unique] */ ILinkDataRW *PLinkDataRW;


EXTERN_C const IID IID_ILinkDataRW;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("DB97C491-AE41-11d1-A719-0060083E8C78")
    ILinkDataRW : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE GetDataPointer( 
            /* [out] */ BYTE **__MIDL_0006,
            /* [out] */ int *__MIDL_0007) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct ILinkDataRWVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            ILinkDataRW * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            ILinkDataRW * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            ILinkDataRW * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetDataPointer )( 
            ILinkDataRW * This,
            /* [out] */ BYTE **__MIDL_0006,
            /* [out] */ int *__MIDL_0007);
        
        END_INTERFACE
    } ILinkDataRWVtbl;

    interface ILinkDataRW
    {
        CONST_VTBL struct ILinkDataRWVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define ILinkDataRW_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define ILinkDataRW_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define ILinkDataRW_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define ILinkDataRW_GetDataPointer(This,__MIDL_0006,__MIDL_0007)	\
    (This)->lpVtbl -> GetDataPointer(This,__MIDL_0006,__MIDL_0007)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE ILinkDataRW_GetDataPointer_Proxy( 
    ILinkDataRW * This,
    /* [out] */ BYTE **__MIDL_0006,
    /* [out] */ int *__MIDL_0007);


void __RPC_STUB ILinkDataRW_GetDataPointer_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __ILinkDataRW_INTERFACE_DEFINED__ */


#ifndef __IPublicSym_INTERFACE_DEFINED__
#define __IPublicSym_INTERFACE_DEFINED__

/* interface IPublicSym */
/* [unique][uuid][object] */ 

typedef /* [unique] */ IPublicSym *LPPubSym;


EXTERN_C const IID IID_IPublicSym;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("18851AD0-910F-11d1-A712-0060083E8C78")
    IPublicSym : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE FComdat( 
            /* [out] */ BOOL *__MIDL_0008,
            /* [out] */ BYTE *__MIDL_0009) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE ISym( 
            /* [out] */ ISYM *__MIDL_0010) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE TkFile( 
            /* [out] */ OBJTK *__MIDL_0011) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SzName( 
            /* [out] */ PLinkDataRO *__MIDL_0012) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SymType( 
            /* [out] */ PSYMT *__MIDL_0013) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SzNameDefaultSym( 
            /* [out] */ PLinkDataRO *__MIDL_0014) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IPublicSymVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IPublicSym * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IPublicSym * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IPublicSym * This);
        
        HRESULT ( STDMETHODCALLTYPE *FComdat )( 
            IPublicSym * This,
            /* [out] */ BOOL *__MIDL_0008,
            /* [out] */ BYTE *__MIDL_0009);
        
        HRESULT ( STDMETHODCALLTYPE *ISym )( 
            IPublicSym * This,
            /* [out] */ ISYM *__MIDL_0010);
        
        HRESULT ( STDMETHODCALLTYPE *TkFile )( 
            IPublicSym * This,
            /* [out] */ OBJTK *__MIDL_0011);
        
        HRESULT ( STDMETHODCALLTYPE *SzName )( 
            IPublicSym * This,
            /* [out] */ PLinkDataRO *__MIDL_0012);
        
        HRESULT ( STDMETHODCALLTYPE *SymType )( 
            IPublicSym * This,
            /* [out] */ PSYMT *__MIDL_0013);
        
        HRESULT ( STDMETHODCALLTYPE *SzNameDefaultSym )( 
            IPublicSym * This,
            /* [out] */ PLinkDataRO *__MIDL_0014);
        
        END_INTERFACE
    } IPublicSymVtbl;

    interface IPublicSym
    {
        CONST_VTBL struct IPublicSymVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IPublicSym_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IPublicSym_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IPublicSym_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IPublicSym_FComdat(This,__MIDL_0008,__MIDL_0009)	\
    (This)->lpVtbl -> FComdat(This,__MIDL_0008,__MIDL_0009)

#define IPublicSym_ISym(This,__MIDL_0010)	\
    (This)->lpVtbl -> ISym(This,__MIDL_0010)

#define IPublicSym_TkFile(This,__MIDL_0011)	\
    (This)->lpVtbl -> TkFile(This,__MIDL_0011)

#define IPublicSym_SzName(This,__MIDL_0012)	\
    (This)->lpVtbl -> SzName(This,__MIDL_0012)

#define IPublicSym_SymType(This,__MIDL_0013)	\
    (This)->lpVtbl -> SymType(This,__MIDL_0013)

#define IPublicSym_SzNameDefaultSym(This,__MIDL_0014)	\
    (This)->lpVtbl -> SzNameDefaultSym(This,__MIDL_0014)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IPublicSym_FComdat_Proxy( 
    IPublicSym * This,
    /* [out] */ BOOL *__MIDL_0008,
    /* [out] */ BYTE *__MIDL_0009);


void __RPC_STUB IPublicSym_FComdat_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IPublicSym_ISym_Proxy( 
    IPublicSym * This,
    /* [out] */ ISYM *__MIDL_0010);


void __RPC_STUB IPublicSym_ISym_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IPublicSym_TkFile_Proxy( 
    IPublicSym * This,
    /* [out] */ OBJTK *__MIDL_0011);


void __RPC_STUB IPublicSym_TkFile_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IPublicSym_SzName_Proxy( 
    IPublicSym * This,
    /* [out] */ PLinkDataRO *__MIDL_0012);


void __RPC_STUB IPublicSym_SzName_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IPublicSym_SymType_Proxy( 
    IPublicSym * This,
    /* [out] */ PSYMT *__MIDL_0013);


void __RPC_STUB IPublicSym_SymType_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IPublicSym_SzNameDefaultSym_Proxy( 
    IPublicSym * This,
    /* [out] */ PLinkDataRO *__MIDL_0014);


void __RPC_STUB IPublicSym_SzNameDefaultSym_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IPublicSym_INTERFACE_DEFINED__ */


#ifndef __IEnumPublics_INTERFACE_DEFINED__
#define __IEnumPublics_INTERFACE_DEFINED__

/* interface IEnumPublics */
/* [unique][uuid][object] */ 

typedef /* [unique] */ IEnumPublics *LPEnumPub;


EXTERN_C const IID IID_IEnumPublics;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("3BFA6690-9113-11d1-A712-0060083E8C78")
    IEnumPublics : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE Next( 
            /* [in] */ ULONG __MIDL_0015,
            /* [out] */ IPublicSym **__MIDL_0016,
            /* [out] */ ULONG *__MIDL_0017) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Skip( 
            /* [in] */ ULONG __MIDL_0018) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Reset( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Clone( 
            /* [out] */ IEnumPublics **__MIDL_0019) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IEnumPublicsVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IEnumPublics * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IEnumPublics * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IEnumPublics * This);
        
        HRESULT ( STDMETHODCALLTYPE *Next )( 
            IEnumPublics * This,
            /* [in] */ ULONG __MIDL_0015,
            /* [out] */ IPublicSym **__MIDL_0016,
            /* [out] */ ULONG *__MIDL_0017);
        
        HRESULT ( STDMETHODCALLTYPE *Skip )( 
            IEnumPublics * This,
            /* [in] */ ULONG __MIDL_0018);
        
        HRESULT ( STDMETHODCALLTYPE *Reset )( 
            IEnumPublics * This);
        
        HRESULT ( STDMETHODCALLTYPE *Clone )( 
            IEnumPublics * This,
            /* [out] */ IEnumPublics **__MIDL_0019);
        
        END_INTERFACE
    } IEnumPublicsVtbl;

    interface IEnumPublics
    {
        CONST_VTBL struct IEnumPublicsVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IEnumPublics_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IEnumPublics_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IEnumPublics_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IEnumPublics_Next(This,__MIDL_0015,__MIDL_0016,__MIDL_0017)	\
    (This)->lpVtbl -> Next(This,__MIDL_0015,__MIDL_0016,__MIDL_0017)

#define IEnumPublics_Skip(This,__MIDL_0018)	\
    (This)->lpVtbl -> Skip(This,__MIDL_0018)

#define IEnumPublics_Reset(This)	\
    (This)->lpVtbl -> Reset(This)

#define IEnumPublics_Clone(This,__MIDL_0019)	\
    (This)->lpVtbl -> Clone(This,__MIDL_0019)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IEnumPublics_Next_Proxy( 
    IEnumPublics * This,
    /* [in] */ ULONG __MIDL_0015,
    /* [out] */ IPublicSym **__MIDL_0016,
    /* [out] */ ULONG *__MIDL_0017);


void __RPC_STUB IEnumPublics_Next_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IEnumPublics_Skip_Proxy( 
    IEnumPublics * This,
    /* [in] */ ULONG __MIDL_0018);


void __RPC_STUB IEnumPublics_Skip_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IEnumPublics_Reset_Proxy( 
    IEnumPublics * This);


void __RPC_STUB IEnumPublics_Reset_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IEnumPublics_Clone_Proxy( 
    IEnumPublics * This,
    /* [out] */ IEnumPublics **__MIDL_0019);


void __RPC_STUB IEnumPublics_Clone_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IEnumPublics_INTERFACE_DEFINED__ */


#ifndef __IObjFile_INTERFACE_DEFINED__
#define __IObjFile_INTERFACE_DEFINED__

/* interface IObjFile */
/* [unique][uuid][object] */ 


EXTERN_C const IID IID_IObjFile;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("5AEA3330-911D-11d1-A712-0060083E8C78")
    IObjFile : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE Characteristics( 
            /* [out] */ DWORD *__MIDL_0020) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE CloseCache( 
            /* [in] */ BOOL __MIDL_0021) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE EnumPublics( 
            /* [out] */ IEnumPublics **__MIDL_0022,
            /* [in] */ OBJTK __MIDL_0023) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE FNative( 
            BOOL *__MIDL_0024) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Machine( 
            /* [out] */ WORD *__MIDL_0025) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE PbDirectives( 
            /* [out] */ PLinkDataRO *__MIDL_0026) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE RgISymForeignRefs( 
            /* [out] */ PLinkDataRO *__MIDL_0027) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE PbExternalFilenames( 
            /* [out] */ PLinkDataRO *__MIDL_0028) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IObjFileVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IObjFile * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IObjFile * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IObjFile * This);
        
        HRESULT ( STDMETHODCALLTYPE *Characteristics )( 
            IObjFile * This,
            /* [out] */ DWORD *__MIDL_0020);
        
        HRESULT ( STDMETHODCALLTYPE *CloseCache )( 
            IObjFile * This,
            /* [in] */ BOOL __MIDL_0021);
        
        HRESULT ( STDMETHODCALLTYPE *EnumPublics )( 
            IObjFile * This,
            /* [out] */ IEnumPublics **__MIDL_0022,
            /* [in] */ OBJTK __MIDL_0023);
        
        HRESULT ( STDMETHODCALLTYPE *FNative )( 
            IObjFile * This,
            BOOL *__MIDL_0024);
        
        HRESULT ( STDMETHODCALLTYPE *Machine )( 
            IObjFile * This,
            /* [out] */ WORD *__MIDL_0025);
        
        HRESULT ( STDMETHODCALLTYPE *PbDirectives )( 
            IObjFile * This,
            /* [out] */ PLinkDataRO *__MIDL_0026);
        
        HRESULT ( STDMETHODCALLTYPE *RgISymForeignRefs )( 
            IObjFile * This,
            /* [out] */ PLinkDataRO *__MIDL_0027);
        
        HRESULT ( STDMETHODCALLTYPE *PbExternalFilenames )( 
            IObjFile * This,
            /* [out] */ PLinkDataRO *__MIDL_0028);
        
        END_INTERFACE
    } IObjFileVtbl;

    interface IObjFile
    {
        CONST_VTBL struct IObjFileVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IObjFile_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IObjFile_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IObjFile_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IObjFile_Characteristics(This,__MIDL_0020)	\
    (This)->lpVtbl -> Characteristics(This,__MIDL_0020)

#define IObjFile_CloseCache(This,__MIDL_0021)	\
    (This)->lpVtbl -> CloseCache(This,__MIDL_0021)

#define IObjFile_EnumPublics(This,__MIDL_0022,__MIDL_0023)	\
    (This)->lpVtbl -> EnumPublics(This,__MIDL_0022,__MIDL_0023)

#define IObjFile_FNative(This,__MIDL_0024)	\
    (This)->lpVtbl -> FNative(This,__MIDL_0024)

#define IObjFile_Machine(This,__MIDL_0025)	\
    (This)->lpVtbl -> Machine(This,__MIDL_0025)

#define IObjFile_PbDirectives(This,__MIDL_0026)	\
    (This)->lpVtbl -> PbDirectives(This,__MIDL_0026)

#define IObjFile_RgISymForeignRefs(This,__MIDL_0027)	\
    (This)->lpVtbl -> RgISymForeignRefs(This,__MIDL_0027)

#define IObjFile_PbExternalFilenames(This,__MIDL_0028)	\
    (This)->lpVtbl -> PbExternalFilenames(This,__MIDL_0028)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IObjFile_Characteristics_Proxy( 
    IObjFile * This,
    /* [out] */ DWORD *__MIDL_0020);


void __RPC_STUB IObjFile_Characteristics_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IObjFile_CloseCache_Proxy( 
    IObjFile * This,
    /* [in] */ BOOL __MIDL_0021);


void __RPC_STUB IObjFile_CloseCache_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IObjFile_EnumPublics_Proxy( 
    IObjFile * This,
    /* [out] */ IEnumPublics **__MIDL_0022,
    /* [in] */ OBJTK __MIDL_0023);


void __RPC_STUB IObjFile_EnumPublics_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IObjFile_FNative_Proxy( 
    IObjFile * This,
    BOOL *__MIDL_0024);


void __RPC_STUB IObjFile_FNative_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IObjFile_Machine_Proxy( 
    IObjFile * This,
    /* [out] */ WORD *__MIDL_0025);


void __RPC_STUB IObjFile_Machine_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IObjFile_PbDirectives_Proxy( 
    IObjFile * This,
    /* [out] */ PLinkDataRO *__MIDL_0026);


void __RPC_STUB IObjFile_PbDirectives_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IObjFile_RgISymForeignRefs_Proxy( 
    IObjFile * This,
    /* [out] */ PLinkDataRO *__MIDL_0027);


void __RPC_STUB IObjFile_RgISymForeignRefs_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IObjFile_PbExternalFilenames_Proxy( 
    IObjFile * This,
    /* [out] */ PLinkDataRO *__MIDL_0028);


void __RPC_STUB IObjFile_PbExternalFilenames_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IObjFile_INTERFACE_DEFINED__ */


#ifndef __IObjHandler_INTERFACE_DEFINED__
#define __IObjHandler_INTERFACE_DEFINED__

/* interface IObjHandler */
/* [unique][uuid][object] */ 

typedef /* [unique] */ IObjHandler *PObjHandler;


EXTERN_C const IID IID_IObjHandler;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("54222448-B583-4cf1-89CA-1737C9F4613B")
    IObjHandler : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE CodeGen( 
            /* [in] */ int __MIDL_0029,
            /* [in] */ unsigned char **__MIDL_0030,
            /* [in] */ PLinkDataRO __MIDL_0031,
            /* [out] */ PLinkDataRO *__MIDL_0032) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Init( 
            /* [in] */ IUnknown *__MIDL_0033,
            /* [in] */ BOOL __MIDL_0034,
            /* [in] */ int __MIDL_0035,
            /* [in] */ unsigned char **__MIDL_0036) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE IObjFileForFile( 
            /* [in] */ BSTR __MIDL_0037,
            /* [in] */ WORD __MIDL_0038,
            /* [in] */ DWORD __MIDL_0039,
            /* [in] */ DWORD __MIDL_0040,
            /* [in] */ OBJTK __MIDL_0041,
            /* [in] */ OFT __MIDL_0042,
            /* [out] */ IObjFile **__MIDL_0043) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE OnCtrlC( void) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IObjHandlerVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IObjHandler * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IObjHandler * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IObjHandler * This);
        
        HRESULT ( STDMETHODCALLTYPE *CodeGen )( 
            IObjHandler * This,
            /* [in] */ int __MIDL_0029,
            /* [in] */ unsigned char **__MIDL_0030,
            /* [in] */ PLinkDataRO __MIDL_0031,
            /* [out] */ PLinkDataRO *__MIDL_0032);
        
        HRESULT ( STDMETHODCALLTYPE *Init )( 
            IObjHandler * This,
            /* [in] */ IUnknown *__MIDL_0033,
            /* [in] */ BOOL __MIDL_0034,
            /* [in] */ int __MIDL_0035,
            /* [in] */ unsigned char **__MIDL_0036);
        
        HRESULT ( STDMETHODCALLTYPE *IObjFileForFile )( 
            IObjHandler * This,
            /* [in] */ BSTR __MIDL_0037,
            /* [in] */ WORD __MIDL_0038,
            /* [in] */ DWORD __MIDL_0039,
            /* [in] */ DWORD __MIDL_0040,
            /* [in] */ OBJTK __MIDL_0041,
            /* [in] */ OFT __MIDL_0042,
            /* [out] */ IObjFile **__MIDL_0043);
        
        HRESULT ( STDMETHODCALLTYPE *OnCtrlC )( 
            IObjHandler * This);
        
        END_INTERFACE
    } IObjHandlerVtbl;

    interface IObjHandler
    {
        CONST_VTBL struct IObjHandlerVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IObjHandler_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IObjHandler_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IObjHandler_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IObjHandler_CodeGen(This,__MIDL_0029,__MIDL_0030,__MIDL_0031,__MIDL_0032)	\
    (This)->lpVtbl -> CodeGen(This,__MIDL_0029,__MIDL_0030,__MIDL_0031,__MIDL_0032)

#define IObjHandler_Init(This,__MIDL_0033,__MIDL_0034,__MIDL_0035,__MIDL_0036)	\
    (This)->lpVtbl -> Init(This,__MIDL_0033,__MIDL_0034,__MIDL_0035,__MIDL_0036)

#define IObjHandler_IObjFileForFile(This,__MIDL_0037,__MIDL_0038,__MIDL_0039,__MIDL_0040,__MIDL_0041,__MIDL_0042,__MIDL_0043)	\
    (This)->lpVtbl -> IObjFileForFile(This,__MIDL_0037,__MIDL_0038,__MIDL_0039,__MIDL_0040,__MIDL_0041,__MIDL_0042,__MIDL_0043)

#define IObjHandler_OnCtrlC(This)	\
    (This)->lpVtbl -> OnCtrlC(This)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IObjHandler_CodeGen_Proxy( 
    IObjHandler * This,
    /* [in] */ int __MIDL_0029,
    /* [in] */ unsigned char **__MIDL_0030,
    /* [in] */ PLinkDataRO __MIDL_0031,
    /* [out] */ PLinkDataRO *__MIDL_0032);


void __RPC_STUB IObjHandler_CodeGen_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IObjHandler_Init_Proxy( 
    IObjHandler * This,
    /* [in] */ IUnknown *__MIDL_0033,
    /* [in] */ BOOL __MIDL_0034,
    /* [in] */ int __MIDL_0035,
    /* [in] */ unsigned char **__MIDL_0036);


void __RPC_STUB IObjHandler_Init_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IObjHandler_IObjFileForFile_Proxy( 
    IObjHandler * This,
    /* [in] */ BSTR __MIDL_0037,
    /* [in] */ WORD __MIDL_0038,
    /* [in] */ DWORD __MIDL_0039,
    /* [in] */ DWORD __MIDL_0040,
    /* [in] */ OBJTK __MIDL_0041,
    /* [in] */ OFT __MIDL_0042,
    /* [out] */ IObjFile **__MIDL_0043);


void __RPC_STUB IObjHandler_IObjFileForFile_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IObjHandler_OnCtrlC_Proxy( 
    IObjHandler * This);


void __RPC_STUB IObjHandler_OnCtrlC_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IObjHandler_INTERFACE_DEFINED__ */


#ifndef __IObjectContrib_INTERFACE_DEFINED__
#define __IObjectContrib_INTERFACE_DEFINED__

/* interface IObjectContrib */
/* [unique][uuid][object] */ 

typedef /* [unique] */ IObjectContrib *PObjContrib;


EXTERN_C const IID IID_IObjectContrib;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("A461E470-911E-11d1-A712-0060083E8C78")
    IObjectContrib : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE CbContents( 
            /* [out] */ DWORD *__MIDL_0044) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Characteristics( 
            /* [out] */ DWORD *__MIDL_0045) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE CLineNo( 
            /* [out] */ DWORD *__MIDL_0046) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE CRelocs( 
            /* [out] */ DWORD *__MIDL_0047) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE ISection( 
            /* [out] */ DWORD *__MIDL_0048) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE PbContents( 
            /* [out] */ BYTE *__MIDL_0049,
            /* [in] */ DWORD __MIDL_0050,
            /* [out] */ DWORD *__MIDL_0051) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE PbContentsRO( 
            /* [out] */ PLinkDataRO *__MIDL_0052) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE PbContentsRW( 
            /* [out] */ PLinkDataRW *__MIDL_0053) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE RgLineNo( 
            /* [out] */ PLinkDataRO *__MIDL_0054) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE RgRelocs( 
            /* [out] */ PLinkDataRO *__MIDL_0055) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE RgRelocsEx( 
            /* [out] */ BYTE *__MIDL_0056,
            /* [in] */ DWORD __MIDL_0057) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SzSectionName( 
            /* [out] */ PLinkDataRO *__MIDL_0058) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE TkFile( 
            /* [out] */ OBJTK *__MIDL_0059) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE VirtualAddress( 
            /* [out] */ DWORD *__MIDL_0060) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IObjectContribVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IObjectContrib * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IObjectContrib * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IObjectContrib * This);
        
        HRESULT ( STDMETHODCALLTYPE *CbContents )( 
            IObjectContrib * This,
            /* [out] */ DWORD *__MIDL_0044);
        
        HRESULT ( STDMETHODCALLTYPE *Characteristics )( 
            IObjectContrib * This,
            /* [out] */ DWORD *__MIDL_0045);
        
        HRESULT ( STDMETHODCALLTYPE *CLineNo )( 
            IObjectContrib * This,
            /* [out] */ DWORD *__MIDL_0046);
        
        HRESULT ( STDMETHODCALLTYPE *CRelocs )( 
            IObjectContrib * This,
            /* [out] */ DWORD *__MIDL_0047);
        
        HRESULT ( STDMETHODCALLTYPE *ISection )( 
            IObjectContrib * This,
            /* [out] */ DWORD *__MIDL_0048);
        
        HRESULT ( STDMETHODCALLTYPE *PbContents )( 
            IObjectContrib * This,
            /* [out] */ BYTE *__MIDL_0049,
            /* [in] */ DWORD __MIDL_0050,
            /* [out] */ DWORD *__MIDL_0051);
        
        HRESULT ( STDMETHODCALLTYPE *PbContentsRO )( 
            IObjectContrib * This,
            /* [out] */ PLinkDataRO *__MIDL_0052);
        
        HRESULT ( STDMETHODCALLTYPE *PbContentsRW )( 
            IObjectContrib * This,
            /* [out] */ PLinkDataRW *__MIDL_0053);
        
        HRESULT ( STDMETHODCALLTYPE *RgLineNo )( 
            IObjectContrib * This,
            /* [out] */ PLinkDataRO *__MIDL_0054);
        
        HRESULT ( STDMETHODCALLTYPE *RgRelocs )( 
            IObjectContrib * This,
            /* [out] */ PLinkDataRO *__MIDL_0055);
        
        HRESULT ( STDMETHODCALLTYPE *RgRelocsEx )( 
            IObjectContrib * This,
            /* [out] */ BYTE *__MIDL_0056,
            /* [in] */ DWORD __MIDL_0057);
        
        HRESULT ( STDMETHODCALLTYPE *SzSectionName )( 
            IObjectContrib * This,
            /* [out] */ PLinkDataRO *__MIDL_0058);
        
        HRESULT ( STDMETHODCALLTYPE *TkFile )( 
            IObjectContrib * This,
            /* [out] */ OBJTK *__MIDL_0059);
        
        HRESULT ( STDMETHODCALLTYPE *VirtualAddress )( 
            IObjectContrib * This,
            /* [out] */ DWORD *__MIDL_0060);
        
        END_INTERFACE
    } IObjectContribVtbl;

    interface IObjectContrib
    {
        CONST_VTBL struct IObjectContribVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IObjectContrib_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IObjectContrib_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IObjectContrib_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IObjectContrib_CbContents(This,__MIDL_0044)	\
    (This)->lpVtbl -> CbContents(This,__MIDL_0044)

#define IObjectContrib_Characteristics(This,__MIDL_0045)	\
    (This)->lpVtbl -> Characteristics(This,__MIDL_0045)

#define IObjectContrib_CLineNo(This,__MIDL_0046)	\
    (This)->lpVtbl -> CLineNo(This,__MIDL_0046)

#define IObjectContrib_CRelocs(This,__MIDL_0047)	\
    (This)->lpVtbl -> CRelocs(This,__MIDL_0047)

#define IObjectContrib_ISection(This,__MIDL_0048)	\
    (This)->lpVtbl -> ISection(This,__MIDL_0048)

#define IObjectContrib_PbContents(This,__MIDL_0049,__MIDL_0050,__MIDL_0051)	\
    (This)->lpVtbl -> PbContents(This,__MIDL_0049,__MIDL_0050,__MIDL_0051)

#define IObjectContrib_PbContentsRO(This,__MIDL_0052)	\
    (This)->lpVtbl -> PbContentsRO(This,__MIDL_0052)

#define IObjectContrib_PbContentsRW(This,__MIDL_0053)	\
    (This)->lpVtbl -> PbContentsRW(This,__MIDL_0053)

#define IObjectContrib_RgLineNo(This,__MIDL_0054)	\
    (This)->lpVtbl -> RgLineNo(This,__MIDL_0054)

#define IObjectContrib_RgRelocs(This,__MIDL_0055)	\
    (This)->lpVtbl -> RgRelocs(This,__MIDL_0055)

#define IObjectContrib_RgRelocsEx(This,__MIDL_0056,__MIDL_0057)	\
    (This)->lpVtbl -> RgRelocsEx(This,__MIDL_0056,__MIDL_0057)

#define IObjectContrib_SzSectionName(This,__MIDL_0058)	\
    (This)->lpVtbl -> SzSectionName(This,__MIDL_0058)

#define IObjectContrib_TkFile(This,__MIDL_0059)	\
    (This)->lpVtbl -> TkFile(This,__MIDL_0059)

#define IObjectContrib_VirtualAddress(This,__MIDL_0060)	\
    (This)->lpVtbl -> VirtualAddress(This,__MIDL_0060)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IObjectContrib_CbContents_Proxy( 
    IObjectContrib * This,
    /* [out] */ DWORD *__MIDL_0044);


void __RPC_STUB IObjectContrib_CbContents_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IObjectContrib_Characteristics_Proxy( 
    IObjectContrib * This,
    /* [out] */ DWORD *__MIDL_0045);


void __RPC_STUB IObjectContrib_Characteristics_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IObjectContrib_CLineNo_Proxy( 
    IObjectContrib * This,
    /* [out] */ DWORD *__MIDL_0046);


void __RPC_STUB IObjectContrib_CLineNo_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IObjectContrib_CRelocs_Proxy( 
    IObjectContrib * This,
    /* [out] */ DWORD *__MIDL_0047);


void __RPC_STUB IObjectContrib_CRelocs_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IObjectContrib_ISection_Proxy( 
    IObjectContrib * This,
    /* [out] */ DWORD *__MIDL_0048);


void __RPC_STUB IObjectContrib_ISection_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IObjectContrib_PbContents_Proxy( 
    IObjectContrib * This,
    /* [out] */ BYTE *__MIDL_0049,
    /* [in] */ DWORD __MIDL_0050,
    /* [out] */ DWORD *__MIDL_0051);


void __RPC_STUB IObjectContrib_PbContents_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IObjectContrib_PbContentsRO_Proxy( 
    IObjectContrib * This,
    /* [out] */ PLinkDataRO *__MIDL_0052);


void __RPC_STUB IObjectContrib_PbContentsRO_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IObjectContrib_PbContentsRW_Proxy( 
    IObjectContrib * This,
    /* [out] */ PLinkDataRW *__MIDL_0053);


void __RPC_STUB IObjectContrib_PbContentsRW_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IObjectContrib_RgLineNo_Proxy( 
    IObjectContrib * This,
    /* [out] */ PLinkDataRO *__MIDL_0054);


void __RPC_STUB IObjectContrib_RgLineNo_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IObjectContrib_RgRelocs_Proxy( 
    IObjectContrib * This,
    /* [out] */ PLinkDataRO *__MIDL_0055);


void __RPC_STUB IObjectContrib_RgRelocs_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IObjectContrib_RgRelocsEx_Proxy( 
    IObjectContrib * This,
    /* [out] */ BYTE *__MIDL_0056,
    /* [in] */ DWORD __MIDL_0057);


void __RPC_STUB IObjectContrib_RgRelocsEx_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IObjectContrib_SzSectionName_Proxy( 
    IObjectContrib * This,
    /* [out] */ PLinkDataRO *__MIDL_0058);


void __RPC_STUB IObjectContrib_SzSectionName_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IObjectContrib_TkFile_Proxy( 
    IObjectContrib * This,
    /* [out] */ OBJTK *__MIDL_0059);


void __RPC_STUB IObjectContrib_TkFile_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IObjectContrib_VirtualAddress_Proxy( 
    IObjectContrib * This,
    /* [out] */ DWORD *__MIDL_0060);


void __RPC_STUB IObjectContrib_VirtualAddress_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IObjectContrib_INTERFACE_DEFINED__ */


#ifndef __IEnumContrib_INTERFACE_DEFINED__
#define __IEnumContrib_INTERFACE_DEFINED__

/* interface IEnumContrib */
/* [unique][uuid][object] */ 

typedef /* [unique] */ IEnumContrib *PEnumContrib;


EXTERN_C const IID IID_IEnumContrib;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("6EC1C970-911F-11d1-A712-0060083E8C78")
    IEnumContrib : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE Next( 
            /* [in] */ ULONG __MIDL_0061,
            /* [out] */ IObjectContrib **__MIDL_0062,
            /* [out] */ ULONG *__MIDL_0063) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Skip( 
            /* [in] */ ULONG __MIDL_0064) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Reset( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Clone( 
            /* [out] */ PEnumContrib *__MIDL_0065) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IEnumContribVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IEnumContrib * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IEnumContrib * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IEnumContrib * This);
        
        HRESULT ( STDMETHODCALLTYPE *Next )( 
            IEnumContrib * This,
            /* [in] */ ULONG __MIDL_0061,
            /* [out] */ IObjectContrib **__MIDL_0062,
            /* [out] */ ULONG *__MIDL_0063);
        
        HRESULT ( STDMETHODCALLTYPE *Skip )( 
            IEnumContrib * This,
            /* [in] */ ULONG __MIDL_0064);
        
        HRESULT ( STDMETHODCALLTYPE *Reset )( 
            IEnumContrib * This);
        
        HRESULT ( STDMETHODCALLTYPE *Clone )( 
            IEnumContrib * This,
            /* [out] */ PEnumContrib *__MIDL_0065);
        
        END_INTERFACE
    } IEnumContribVtbl;

    interface IEnumContrib
    {
        CONST_VTBL struct IEnumContribVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IEnumContrib_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IEnumContrib_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IEnumContrib_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IEnumContrib_Next(This,__MIDL_0061,__MIDL_0062,__MIDL_0063)	\
    (This)->lpVtbl -> Next(This,__MIDL_0061,__MIDL_0062,__MIDL_0063)

#define IEnumContrib_Skip(This,__MIDL_0064)	\
    (This)->lpVtbl -> Skip(This,__MIDL_0064)

#define IEnumContrib_Reset(This)	\
    (This)->lpVtbl -> Reset(This)

#define IEnumContrib_Clone(This,__MIDL_0065)	\
    (This)->lpVtbl -> Clone(This,__MIDL_0065)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IEnumContrib_Next_Proxy( 
    IEnumContrib * This,
    /* [in] */ ULONG __MIDL_0061,
    /* [out] */ IObjectContrib **__MIDL_0062,
    /* [out] */ ULONG *__MIDL_0063);


void __RPC_STUB IEnumContrib_Next_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IEnumContrib_Skip_Proxy( 
    IEnumContrib * This,
    /* [in] */ ULONG __MIDL_0064);


void __RPC_STUB IEnumContrib_Skip_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IEnumContrib_Reset_Proxy( 
    IEnumContrib * This);


void __RPC_STUB IEnumContrib_Reset_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IEnumContrib_Clone_Proxy( 
    IEnumContrib * This,
    /* [out] */ PEnumContrib *__MIDL_0065);


void __RPC_STUB IEnumContrib_Clone_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IEnumContrib_INTERFACE_DEFINED__ */


#ifndef __ICOFFAuxSym_INTERFACE_DEFINED__
#define __ICOFFAuxSym_INTERFACE_DEFINED__

/* interface ICOFFAuxSym */
/* [unique][uuid][object] */ 

typedef /* [unique] */ ICOFFAuxSym *PCOFFAuxSym;


EXTERN_C const IID IID_ICOFFAuxSym;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("63C3815F-9FFA-48CD-8B2B-472E795FBA94")
    ICOFFAuxSym : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE TkFile( 
            /* [out] */ OBJTK *__MIDL_0066) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE ImgAuxSym( 
            /* [in] */ BYTE *__MIDL_0067) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Length( 
            /* [out] */ DWORD *__MIDL_0068) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE CRelocs( 
            /* [out] */ DWORD *__MIDL_0069) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE CLineNo( 
            /* [out] */ DWORD *__MIDL_0070) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE CheckSum( 
            /* [out] */ DWORD *__MIDL_0071) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE ISection( 
            /* [out] */ int *__MIDL_0072) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SelType( 
            /* [out] */ BYTE *__MIDL_0073) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE TagIndex( 
            /* [out] */ DWORD *__MIDL_0074) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE MiscLineNo( 
            /* [out] */ DWORD *__MIDL_0075) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE MiscSize( 
            /* [out] */ DWORD *__MIDL_0076) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE MiscTotalSize( 
            /* [out] */ DWORD *__MIDL_0077) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE FcnPLineNo( 
            /* [out] */ DWORD *__MIDL_0078) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE FcnPNextFcn( 
            /* [out] */ DWORD *__MIDL_0079) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE AryDim( 
            /* [in] */ DWORD *__MIDL_0080,
            /* [in] */ DWORD __MIDL_0081) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE NewSymCrc( 
            /* [out] */ DWORD *__MIDL_0082) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE AuxType( 
            /* [out] */ BYTE *__MIDL_0083) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Tocid( 
            /* [out] */ BYTE *__MIDL_0084) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE TokenDef( 
            /* [out] */ DWORD *__MIDL_0085) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct ICOFFAuxSymVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            ICOFFAuxSym * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            ICOFFAuxSym * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            ICOFFAuxSym * This);
        
        HRESULT ( STDMETHODCALLTYPE *TkFile )( 
            ICOFFAuxSym * This,
            /* [out] */ OBJTK *__MIDL_0066);
        
        HRESULT ( STDMETHODCALLTYPE *ImgAuxSym )( 
            ICOFFAuxSym * This,
            /* [in] */ BYTE *__MIDL_0067);
        
        HRESULT ( STDMETHODCALLTYPE *Length )( 
            ICOFFAuxSym * This,
            /* [out] */ DWORD *__MIDL_0068);
        
        HRESULT ( STDMETHODCALLTYPE *CRelocs )( 
            ICOFFAuxSym * This,
            /* [out] */ DWORD *__MIDL_0069);
        
        HRESULT ( STDMETHODCALLTYPE *CLineNo )( 
            ICOFFAuxSym * This,
            /* [out] */ DWORD *__MIDL_0070);
        
        HRESULT ( STDMETHODCALLTYPE *CheckSum )( 
            ICOFFAuxSym * This,
            /* [out] */ DWORD *__MIDL_0071);
        
        HRESULT ( STDMETHODCALLTYPE *ISection )( 
            ICOFFAuxSym * This,
            /* [out] */ int *__MIDL_0072);
        
        HRESULT ( STDMETHODCALLTYPE *SelType )( 
            ICOFFAuxSym * This,
            /* [out] */ BYTE *__MIDL_0073);
        
        HRESULT ( STDMETHODCALLTYPE *TagIndex )( 
            ICOFFAuxSym * This,
            /* [out] */ DWORD *__MIDL_0074);
        
        HRESULT ( STDMETHODCALLTYPE *MiscLineNo )( 
            ICOFFAuxSym * This,
            /* [out] */ DWORD *__MIDL_0075);
        
        HRESULT ( STDMETHODCALLTYPE *MiscSize )( 
            ICOFFAuxSym * This,
            /* [out] */ DWORD *__MIDL_0076);
        
        HRESULT ( STDMETHODCALLTYPE *MiscTotalSize )( 
            ICOFFAuxSym * This,
            /* [out] */ DWORD *__MIDL_0077);
        
        HRESULT ( STDMETHODCALLTYPE *FcnPLineNo )( 
            ICOFFAuxSym * This,
            /* [out] */ DWORD *__MIDL_0078);
        
        HRESULT ( STDMETHODCALLTYPE *FcnPNextFcn )( 
            ICOFFAuxSym * This,
            /* [out] */ DWORD *__MIDL_0079);
        
        HRESULT ( STDMETHODCALLTYPE *AryDim )( 
            ICOFFAuxSym * This,
            /* [in] */ DWORD *__MIDL_0080,
            /* [in] */ DWORD __MIDL_0081);
        
        HRESULT ( STDMETHODCALLTYPE *NewSymCrc )( 
            ICOFFAuxSym * This,
            /* [out] */ DWORD *__MIDL_0082);
        
        HRESULT ( STDMETHODCALLTYPE *AuxType )( 
            ICOFFAuxSym * This,
            /* [out] */ BYTE *__MIDL_0083);
        
        HRESULT ( STDMETHODCALLTYPE *Tocid )( 
            ICOFFAuxSym * This,
            /* [out] */ BYTE *__MIDL_0084);
        
        HRESULT ( STDMETHODCALLTYPE *TokenDef )( 
            ICOFFAuxSym * This,
            /* [out] */ DWORD *__MIDL_0085);
        
        END_INTERFACE
    } ICOFFAuxSymVtbl;

    interface ICOFFAuxSym
    {
        CONST_VTBL struct ICOFFAuxSymVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define ICOFFAuxSym_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define ICOFFAuxSym_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define ICOFFAuxSym_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define ICOFFAuxSym_TkFile(This,__MIDL_0066)	\
    (This)->lpVtbl -> TkFile(This,__MIDL_0066)

#define ICOFFAuxSym_ImgAuxSym(This,__MIDL_0067)	\
    (This)->lpVtbl -> ImgAuxSym(This,__MIDL_0067)

#define ICOFFAuxSym_Length(This,__MIDL_0068)	\
    (This)->lpVtbl -> Length(This,__MIDL_0068)

#define ICOFFAuxSym_CRelocs(This,__MIDL_0069)	\
    (This)->lpVtbl -> CRelocs(This,__MIDL_0069)

#define ICOFFAuxSym_CLineNo(This,__MIDL_0070)	\
    (This)->lpVtbl -> CLineNo(This,__MIDL_0070)

#define ICOFFAuxSym_CheckSum(This,__MIDL_0071)	\
    (This)->lpVtbl -> CheckSum(This,__MIDL_0071)

#define ICOFFAuxSym_ISection(This,__MIDL_0072)	\
    (This)->lpVtbl -> ISection(This,__MIDL_0072)

#define ICOFFAuxSym_SelType(This,__MIDL_0073)	\
    (This)->lpVtbl -> SelType(This,__MIDL_0073)

#define ICOFFAuxSym_TagIndex(This,__MIDL_0074)	\
    (This)->lpVtbl -> TagIndex(This,__MIDL_0074)

#define ICOFFAuxSym_MiscLineNo(This,__MIDL_0075)	\
    (This)->lpVtbl -> MiscLineNo(This,__MIDL_0075)

#define ICOFFAuxSym_MiscSize(This,__MIDL_0076)	\
    (This)->lpVtbl -> MiscSize(This,__MIDL_0076)

#define ICOFFAuxSym_MiscTotalSize(This,__MIDL_0077)	\
    (This)->lpVtbl -> MiscTotalSize(This,__MIDL_0077)

#define ICOFFAuxSym_FcnPLineNo(This,__MIDL_0078)	\
    (This)->lpVtbl -> FcnPLineNo(This,__MIDL_0078)

#define ICOFFAuxSym_FcnPNextFcn(This,__MIDL_0079)	\
    (This)->lpVtbl -> FcnPNextFcn(This,__MIDL_0079)

#define ICOFFAuxSym_AryDim(This,__MIDL_0080,__MIDL_0081)	\
    (This)->lpVtbl -> AryDim(This,__MIDL_0080,__MIDL_0081)

#define ICOFFAuxSym_NewSymCrc(This,__MIDL_0082)	\
    (This)->lpVtbl -> NewSymCrc(This,__MIDL_0082)

#define ICOFFAuxSym_AuxType(This,__MIDL_0083)	\
    (This)->lpVtbl -> AuxType(This,__MIDL_0083)

#define ICOFFAuxSym_Tocid(This,__MIDL_0084)	\
    (This)->lpVtbl -> Tocid(This,__MIDL_0084)

#define ICOFFAuxSym_TokenDef(This,__MIDL_0085)	\
    (This)->lpVtbl -> TokenDef(This,__MIDL_0085)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE ICOFFAuxSym_TkFile_Proxy( 
    ICOFFAuxSym * This,
    /* [out] */ OBJTK *__MIDL_0066);


void __RPC_STUB ICOFFAuxSym_TkFile_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ICOFFAuxSym_ImgAuxSym_Proxy( 
    ICOFFAuxSym * This,
    /* [in] */ BYTE *__MIDL_0067);


void __RPC_STUB ICOFFAuxSym_ImgAuxSym_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ICOFFAuxSym_Length_Proxy( 
    ICOFFAuxSym * This,
    /* [out] */ DWORD *__MIDL_0068);


void __RPC_STUB ICOFFAuxSym_Length_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ICOFFAuxSym_CRelocs_Proxy( 
    ICOFFAuxSym * This,
    /* [out] */ DWORD *__MIDL_0069);


void __RPC_STUB ICOFFAuxSym_CRelocs_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ICOFFAuxSym_CLineNo_Proxy( 
    ICOFFAuxSym * This,
    /* [out] */ DWORD *__MIDL_0070);


void __RPC_STUB ICOFFAuxSym_CLineNo_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ICOFFAuxSym_CheckSum_Proxy( 
    ICOFFAuxSym * This,
    /* [out] */ DWORD *__MIDL_0071);


void __RPC_STUB ICOFFAuxSym_CheckSum_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ICOFFAuxSym_ISection_Proxy( 
    ICOFFAuxSym * This,
    /* [out] */ int *__MIDL_0072);


void __RPC_STUB ICOFFAuxSym_ISection_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ICOFFAuxSym_SelType_Proxy( 
    ICOFFAuxSym * This,
    /* [out] */ BYTE *__MIDL_0073);


void __RPC_STUB ICOFFAuxSym_SelType_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ICOFFAuxSym_TagIndex_Proxy( 
    ICOFFAuxSym * This,
    /* [out] */ DWORD *__MIDL_0074);


void __RPC_STUB ICOFFAuxSym_TagIndex_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ICOFFAuxSym_MiscLineNo_Proxy( 
    ICOFFAuxSym * This,
    /* [out] */ DWORD *__MIDL_0075);


void __RPC_STUB ICOFFAuxSym_MiscLineNo_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ICOFFAuxSym_MiscSize_Proxy( 
    ICOFFAuxSym * This,
    /* [out] */ DWORD *__MIDL_0076);


void __RPC_STUB ICOFFAuxSym_MiscSize_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ICOFFAuxSym_MiscTotalSize_Proxy( 
    ICOFFAuxSym * This,
    /* [out] */ DWORD *__MIDL_0077);


void __RPC_STUB ICOFFAuxSym_MiscTotalSize_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ICOFFAuxSym_FcnPLineNo_Proxy( 
    ICOFFAuxSym * This,
    /* [out] */ DWORD *__MIDL_0078);


void __RPC_STUB ICOFFAuxSym_FcnPLineNo_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ICOFFAuxSym_FcnPNextFcn_Proxy( 
    ICOFFAuxSym * This,
    /* [out] */ DWORD *__MIDL_0079);


void __RPC_STUB ICOFFAuxSym_FcnPNextFcn_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ICOFFAuxSym_AryDim_Proxy( 
    ICOFFAuxSym * This,
    /* [in] */ DWORD *__MIDL_0080,
    /* [in] */ DWORD __MIDL_0081);


void __RPC_STUB ICOFFAuxSym_AryDim_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ICOFFAuxSym_NewSymCrc_Proxy( 
    ICOFFAuxSym * This,
    /* [out] */ DWORD *__MIDL_0082);


void __RPC_STUB ICOFFAuxSym_NewSymCrc_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ICOFFAuxSym_AuxType_Proxy( 
    ICOFFAuxSym * This,
    /* [out] */ BYTE *__MIDL_0083);


void __RPC_STUB ICOFFAuxSym_AuxType_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ICOFFAuxSym_Tocid_Proxy( 
    ICOFFAuxSym * This,
    /* [out] */ BYTE *__MIDL_0084);


void __RPC_STUB ICOFFAuxSym_Tocid_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ICOFFAuxSym_TokenDef_Proxy( 
    ICOFFAuxSym * This,
    /* [out] */ DWORD *__MIDL_0085);


void __RPC_STUB ICOFFAuxSym_TokenDef_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __ICOFFAuxSym_INTERFACE_DEFINED__ */


#ifndef __IEnumAuxSym_INTERFACE_DEFINED__
#define __IEnumAuxSym_INTERFACE_DEFINED__

/* interface IEnumAuxSym */
/* [unique][uuid][object] */ 

typedef /* [unique] */ IEnumAuxSym *PEnumAuxSym;


EXTERN_C const IID IID_IEnumAuxSym;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("083DE150-B461-11d1-A71B-0060083E8C78")
    IEnumAuxSym : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE Next( 
            /* [in] */ ULONG __MIDL_0086,
            /* [out] */ ICOFFAuxSym **__MIDL_0087,
            /* [out] */ ULONG *__MIDL_0088) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Skip( 
            /* [in] */ ULONG __MIDL_0089) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Reset( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Clone( 
            /* [out] */ IEnumAuxSym **__MIDL_0090) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IEnumAuxSymVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IEnumAuxSym * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IEnumAuxSym * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IEnumAuxSym * This);
        
        HRESULT ( STDMETHODCALLTYPE *Next )( 
            IEnumAuxSym * This,
            /* [in] */ ULONG __MIDL_0086,
            /* [out] */ ICOFFAuxSym **__MIDL_0087,
            /* [out] */ ULONG *__MIDL_0088);
        
        HRESULT ( STDMETHODCALLTYPE *Skip )( 
            IEnumAuxSym * This,
            /* [in] */ ULONG __MIDL_0089);
        
        HRESULT ( STDMETHODCALLTYPE *Reset )( 
            IEnumAuxSym * This);
        
        HRESULT ( STDMETHODCALLTYPE *Clone )( 
            IEnumAuxSym * This,
            /* [out] */ IEnumAuxSym **__MIDL_0090);
        
        END_INTERFACE
    } IEnumAuxSymVtbl;

    interface IEnumAuxSym
    {
        CONST_VTBL struct IEnumAuxSymVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IEnumAuxSym_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IEnumAuxSym_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IEnumAuxSym_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IEnumAuxSym_Next(This,__MIDL_0086,__MIDL_0087,__MIDL_0088)	\
    (This)->lpVtbl -> Next(This,__MIDL_0086,__MIDL_0087,__MIDL_0088)

#define IEnumAuxSym_Skip(This,__MIDL_0089)	\
    (This)->lpVtbl -> Skip(This,__MIDL_0089)

#define IEnumAuxSym_Reset(This)	\
    (This)->lpVtbl -> Reset(This)

#define IEnumAuxSym_Clone(This,__MIDL_0090)	\
    (This)->lpVtbl -> Clone(This,__MIDL_0090)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IEnumAuxSym_Next_Proxy( 
    IEnumAuxSym * This,
    /* [in] */ ULONG __MIDL_0086,
    /* [out] */ ICOFFAuxSym **__MIDL_0087,
    /* [out] */ ULONG *__MIDL_0088);


void __RPC_STUB IEnumAuxSym_Next_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IEnumAuxSym_Skip_Proxy( 
    IEnumAuxSym * This,
    /* [in] */ ULONG __MIDL_0089);


void __RPC_STUB IEnumAuxSym_Skip_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IEnumAuxSym_Reset_Proxy( 
    IEnumAuxSym * This);


void __RPC_STUB IEnumAuxSym_Reset_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IEnumAuxSym_Clone_Proxy( 
    IEnumAuxSym * This,
    /* [out] */ IEnumAuxSym **__MIDL_0090);


void __RPC_STUB IEnumAuxSym_Clone_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IEnumAuxSym_INTERFACE_DEFINED__ */


#ifndef __ICOFFSymRO_INTERFACE_DEFINED__
#define __ICOFFSymRO_INTERFACE_DEFINED__

/* interface ICOFFSymRO */
/* [unique][uuid][object] */ 

typedef /* [unique] */ ICOFFSymRO *PCOFFSymRO;


EXTERN_C const IID IID_ICOFFSymRO;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("CBDFD0D1-B21F-11d1-A71B-0060083E8C78")
    ICOFFSymRO : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE CAuxSym( 
            /* [out] */ BYTE *__MIDL_0091) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE ISection( 
            /* [out] */ int *__MIDL_0092) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE ISym( 
            /* [out] */ DWORD *__MIDL_0093) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE StorageClass( 
            /* [out] */ BYTE *__MIDL_0094) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SzFile( 
            /* [out] */ PLinkDataRO *__MIDL_0095) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SzName( 
            /* [out] */ PLinkDataRO *__MIDL_0096) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE TkFile( 
            /* [out] */ OBJTK *__MIDL_0097) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Type( 
            /* [out] */ WORD *__MIDL_0098) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Value( 
            /* [out] */ DWORD *__MIDL_0099) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE EnumAuxSym( 
            /* [out] */ IEnumAuxSym **__MIDL_0100) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct ICOFFSymROVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            ICOFFSymRO * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            ICOFFSymRO * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            ICOFFSymRO * This);
        
        HRESULT ( STDMETHODCALLTYPE *CAuxSym )( 
            ICOFFSymRO * This,
            /* [out] */ BYTE *__MIDL_0091);
        
        HRESULT ( STDMETHODCALLTYPE *ISection )( 
            ICOFFSymRO * This,
            /* [out] */ int *__MIDL_0092);
        
        HRESULT ( STDMETHODCALLTYPE *ISym )( 
            ICOFFSymRO * This,
            /* [out] */ DWORD *__MIDL_0093);
        
        HRESULT ( STDMETHODCALLTYPE *StorageClass )( 
            ICOFFSymRO * This,
            /* [out] */ BYTE *__MIDL_0094);
        
        HRESULT ( STDMETHODCALLTYPE *SzFile )( 
            ICOFFSymRO * This,
            /* [out] */ PLinkDataRO *__MIDL_0095);
        
        HRESULT ( STDMETHODCALLTYPE *SzName )( 
            ICOFFSymRO * This,
            /* [out] */ PLinkDataRO *__MIDL_0096);
        
        HRESULT ( STDMETHODCALLTYPE *TkFile )( 
            ICOFFSymRO * This,
            /* [out] */ OBJTK *__MIDL_0097);
        
        HRESULT ( STDMETHODCALLTYPE *Type )( 
            ICOFFSymRO * This,
            /* [out] */ WORD *__MIDL_0098);
        
        HRESULT ( STDMETHODCALLTYPE *Value )( 
            ICOFFSymRO * This,
            /* [out] */ DWORD *__MIDL_0099);
        
        HRESULT ( STDMETHODCALLTYPE *EnumAuxSym )( 
            ICOFFSymRO * This,
            /* [out] */ IEnumAuxSym **__MIDL_0100);
        
        END_INTERFACE
    } ICOFFSymROVtbl;

    interface ICOFFSymRO
    {
        CONST_VTBL struct ICOFFSymROVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define ICOFFSymRO_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define ICOFFSymRO_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define ICOFFSymRO_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define ICOFFSymRO_CAuxSym(This,__MIDL_0091)	\
    (This)->lpVtbl -> CAuxSym(This,__MIDL_0091)

#define ICOFFSymRO_ISection(This,__MIDL_0092)	\
    (This)->lpVtbl -> ISection(This,__MIDL_0092)

#define ICOFFSymRO_ISym(This,__MIDL_0093)	\
    (This)->lpVtbl -> ISym(This,__MIDL_0093)

#define ICOFFSymRO_StorageClass(This,__MIDL_0094)	\
    (This)->lpVtbl -> StorageClass(This,__MIDL_0094)

#define ICOFFSymRO_SzFile(This,__MIDL_0095)	\
    (This)->lpVtbl -> SzFile(This,__MIDL_0095)

#define ICOFFSymRO_SzName(This,__MIDL_0096)	\
    (This)->lpVtbl -> SzName(This,__MIDL_0096)

#define ICOFFSymRO_TkFile(This,__MIDL_0097)	\
    (This)->lpVtbl -> TkFile(This,__MIDL_0097)

#define ICOFFSymRO_Type(This,__MIDL_0098)	\
    (This)->lpVtbl -> Type(This,__MIDL_0098)

#define ICOFFSymRO_Value(This,__MIDL_0099)	\
    (This)->lpVtbl -> Value(This,__MIDL_0099)

#define ICOFFSymRO_EnumAuxSym(This,__MIDL_0100)	\
    (This)->lpVtbl -> EnumAuxSym(This,__MIDL_0100)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE ICOFFSymRO_CAuxSym_Proxy( 
    ICOFFSymRO * This,
    /* [out] */ BYTE *__MIDL_0091);


void __RPC_STUB ICOFFSymRO_CAuxSym_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ICOFFSymRO_ISection_Proxy( 
    ICOFFSymRO * This,
    /* [out] */ int *__MIDL_0092);


void __RPC_STUB ICOFFSymRO_ISection_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ICOFFSymRO_ISym_Proxy( 
    ICOFFSymRO * This,
    /* [out] */ DWORD *__MIDL_0093);


void __RPC_STUB ICOFFSymRO_ISym_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ICOFFSymRO_StorageClass_Proxy( 
    ICOFFSymRO * This,
    /* [out] */ BYTE *__MIDL_0094);


void __RPC_STUB ICOFFSymRO_StorageClass_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ICOFFSymRO_SzFile_Proxy( 
    ICOFFSymRO * This,
    /* [out] */ PLinkDataRO *__MIDL_0095);


void __RPC_STUB ICOFFSymRO_SzFile_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ICOFFSymRO_SzName_Proxy( 
    ICOFFSymRO * This,
    /* [out] */ PLinkDataRO *__MIDL_0096);


void __RPC_STUB ICOFFSymRO_SzName_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ICOFFSymRO_TkFile_Proxy( 
    ICOFFSymRO * This,
    /* [out] */ OBJTK *__MIDL_0097);


void __RPC_STUB ICOFFSymRO_TkFile_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ICOFFSymRO_Type_Proxy( 
    ICOFFSymRO * This,
    /* [out] */ WORD *__MIDL_0098);


void __RPC_STUB ICOFFSymRO_Type_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ICOFFSymRO_Value_Proxy( 
    ICOFFSymRO * This,
    /* [out] */ DWORD *__MIDL_0099);


void __RPC_STUB ICOFFSymRO_Value_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ICOFFSymRO_EnumAuxSym_Proxy( 
    ICOFFSymRO * This,
    /* [out] */ IEnumAuxSym **__MIDL_0100);


void __RPC_STUB ICOFFSymRO_EnumAuxSym_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __ICOFFSymRO_INTERFACE_DEFINED__ */


#ifndef __ICOFFSymRW_INTERFACE_DEFINED__
#define __ICOFFSymRW_INTERFACE_DEFINED__

/* interface ICOFFSymRW */
/* [unique][uuid][object] */ 

typedef /* [unique] */ ICOFFSymRW *PCOFFSymRW;


EXTERN_C const IID IID_ICOFFSymRW;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("B966F266-47C9-42EE-BF73-E8B006231883")
    ICOFFSymRW : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE CAuxSym( 
            /* [out] */ BYTE *__MIDL_0101) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE ISection( 
            /* [out] */ int *__MIDL_0102) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE ISym( 
            /* [out] */ DWORD *__MIDL_0103) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE StorageClass( 
            /* [out] */ BYTE *__MIDL_0104) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SzFile( 
            /* [out] */ PLinkDataRO *__MIDL_0105) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SzName( 
            /* [out] */ PLinkDataRO *__MIDL_0106) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE TkFile( 
            /* [out] */ OBJTK *__MIDL_0107) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Type( 
            /* [out] */ WORD *__MIDL_0108) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Value( 
            /* [out] */ DWORD *__MIDL_0109) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE EnumAuxSym( 
            /* [out] */ IEnumAuxSym **__MIDL_0110) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetISection( 
            /* [in] */ int __MIDL_0111) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetType( 
            /* [in] */ WORD __MIDL_0112) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetValue( 
            /* [in] */ DWORD __MIDL_0113) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct ICOFFSymRWVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            ICOFFSymRW * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            ICOFFSymRW * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            ICOFFSymRW * This);
        
        HRESULT ( STDMETHODCALLTYPE *CAuxSym )( 
            ICOFFSymRW * This,
            /* [out] */ BYTE *__MIDL_0101);
        
        HRESULT ( STDMETHODCALLTYPE *ISection )( 
            ICOFFSymRW * This,
            /* [out] */ int *__MIDL_0102);
        
        HRESULT ( STDMETHODCALLTYPE *ISym )( 
            ICOFFSymRW * This,
            /* [out] */ DWORD *__MIDL_0103);
        
        HRESULT ( STDMETHODCALLTYPE *StorageClass )( 
            ICOFFSymRW * This,
            /* [out] */ BYTE *__MIDL_0104);
        
        HRESULT ( STDMETHODCALLTYPE *SzFile )( 
            ICOFFSymRW * This,
            /* [out] */ PLinkDataRO *__MIDL_0105);
        
        HRESULT ( STDMETHODCALLTYPE *SzName )( 
            ICOFFSymRW * This,
            /* [out] */ PLinkDataRO *__MIDL_0106);
        
        HRESULT ( STDMETHODCALLTYPE *TkFile )( 
            ICOFFSymRW * This,
            /* [out] */ OBJTK *__MIDL_0107);
        
        HRESULT ( STDMETHODCALLTYPE *Type )( 
            ICOFFSymRW * This,
            /* [out] */ WORD *__MIDL_0108);
        
        HRESULT ( STDMETHODCALLTYPE *Value )( 
            ICOFFSymRW * This,
            /* [out] */ DWORD *__MIDL_0109);
        
        HRESULT ( STDMETHODCALLTYPE *EnumAuxSym )( 
            ICOFFSymRW * This,
            /* [out] */ IEnumAuxSym **__MIDL_0110);
        
        HRESULT ( STDMETHODCALLTYPE *SetISection )( 
            ICOFFSymRW * This,
            /* [in] */ int __MIDL_0111);
        
        HRESULT ( STDMETHODCALLTYPE *SetType )( 
            ICOFFSymRW * This,
            /* [in] */ WORD __MIDL_0112);
        
        HRESULT ( STDMETHODCALLTYPE *SetValue )( 
            ICOFFSymRW * This,
            /* [in] */ DWORD __MIDL_0113);
        
        END_INTERFACE
    } ICOFFSymRWVtbl;

    interface ICOFFSymRW
    {
        CONST_VTBL struct ICOFFSymRWVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define ICOFFSymRW_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define ICOFFSymRW_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define ICOFFSymRW_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define ICOFFSymRW_CAuxSym(This,__MIDL_0101)	\
    (This)->lpVtbl -> CAuxSym(This,__MIDL_0101)

#define ICOFFSymRW_ISection(This,__MIDL_0102)	\
    (This)->lpVtbl -> ISection(This,__MIDL_0102)

#define ICOFFSymRW_ISym(This,__MIDL_0103)	\
    (This)->lpVtbl -> ISym(This,__MIDL_0103)

#define ICOFFSymRW_StorageClass(This,__MIDL_0104)	\
    (This)->lpVtbl -> StorageClass(This,__MIDL_0104)

#define ICOFFSymRW_SzFile(This,__MIDL_0105)	\
    (This)->lpVtbl -> SzFile(This,__MIDL_0105)

#define ICOFFSymRW_SzName(This,__MIDL_0106)	\
    (This)->lpVtbl -> SzName(This,__MIDL_0106)

#define ICOFFSymRW_TkFile(This,__MIDL_0107)	\
    (This)->lpVtbl -> TkFile(This,__MIDL_0107)

#define ICOFFSymRW_Type(This,__MIDL_0108)	\
    (This)->lpVtbl -> Type(This,__MIDL_0108)

#define ICOFFSymRW_Value(This,__MIDL_0109)	\
    (This)->lpVtbl -> Value(This,__MIDL_0109)

#define ICOFFSymRW_EnumAuxSym(This,__MIDL_0110)	\
    (This)->lpVtbl -> EnumAuxSym(This,__MIDL_0110)

#define ICOFFSymRW_SetISection(This,__MIDL_0111)	\
    (This)->lpVtbl -> SetISection(This,__MIDL_0111)

#define ICOFFSymRW_SetType(This,__MIDL_0112)	\
    (This)->lpVtbl -> SetType(This,__MIDL_0112)

#define ICOFFSymRW_SetValue(This,__MIDL_0113)	\
    (This)->lpVtbl -> SetValue(This,__MIDL_0113)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE ICOFFSymRW_CAuxSym_Proxy( 
    ICOFFSymRW * This,
    /* [out] */ BYTE *__MIDL_0101);


void __RPC_STUB ICOFFSymRW_CAuxSym_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ICOFFSymRW_ISection_Proxy( 
    ICOFFSymRW * This,
    /* [out] */ int *__MIDL_0102);


void __RPC_STUB ICOFFSymRW_ISection_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ICOFFSymRW_ISym_Proxy( 
    ICOFFSymRW * This,
    /* [out] */ DWORD *__MIDL_0103);


void __RPC_STUB ICOFFSymRW_ISym_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ICOFFSymRW_StorageClass_Proxy( 
    ICOFFSymRW * This,
    /* [out] */ BYTE *__MIDL_0104);


void __RPC_STUB ICOFFSymRW_StorageClass_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ICOFFSymRW_SzFile_Proxy( 
    ICOFFSymRW * This,
    /* [out] */ PLinkDataRO *__MIDL_0105);


void __RPC_STUB ICOFFSymRW_SzFile_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ICOFFSymRW_SzName_Proxy( 
    ICOFFSymRW * This,
    /* [out] */ PLinkDataRO *__MIDL_0106);


void __RPC_STUB ICOFFSymRW_SzName_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ICOFFSymRW_TkFile_Proxy( 
    ICOFFSymRW * This,
    /* [out] */ OBJTK *__MIDL_0107);


void __RPC_STUB ICOFFSymRW_TkFile_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ICOFFSymRW_Type_Proxy( 
    ICOFFSymRW * This,
    /* [out] */ WORD *__MIDL_0108);


void __RPC_STUB ICOFFSymRW_Type_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ICOFFSymRW_Value_Proxy( 
    ICOFFSymRW * This,
    /* [out] */ DWORD *__MIDL_0109);


void __RPC_STUB ICOFFSymRW_Value_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ICOFFSymRW_EnumAuxSym_Proxy( 
    ICOFFSymRW * This,
    /* [out] */ IEnumAuxSym **__MIDL_0110);


void __RPC_STUB ICOFFSymRW_EnumAuxSym_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ICOFFSymRW_SetISection_Proxy( 
    ICOFFSymRW * This,
    /* [in] */ int __MIDL_0111);


void __RPC_STUB ICOFFSymRW_SetISection_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ICOFFSymRW_SetType_Proxy( 
    ICOFFSymRW * This,
    /* [in] */ WORD __MIDL_0112);


void __RPC_STUB ICOFFSymRW_SetType_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ICOFFSymRW_SetValue_Proxy( 
    ICOFFSymRW * This,
    /* [in] */ DWORD __MIDL_0113);


void __RPC_STUB ICOFFSymRW_SetValue_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __ICOFFSymRW_INTERFACE_DEFINED__ */


#ifndef __IEnumCOFFSymRO_INTERFACE_DEFINED__
#define __IEnumCOFFSymRO_INTERFACE_DEFINED__

/* interface IEnumCOFFSymRO */
/* [unique][uuid][object] */ 

typedef /* [unique] */ IEnumCOFFSymRO *PEnumCOFFSymRO;


EXTERN_C const IID IID_IEnumCOFFSymRO;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("33640AD0-B2C6-11d1-A71B-0060083E8C78")
    IEnumCOFFSymRO : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE Next( 
            /* [in] */ ULONG __MIDL_0114,
            /* [out] */ ICOFFSymRO **__MIDL_0115,
            /* [out] */ ULONG *__MIDL_0116) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Skip( 
            /* [in] */ ULONG __MIDL_0117) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Reset( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Clone( 
            /* [out] */ IEnumCOFFSymRO **__MIDL_0118) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IEnumCOFFSymROVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IEnumCOFFSymRO * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IEnumCOFFSymRO * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IEnumCOFFSymRO * This);
        
        HRESULT ( STDMETHODCALLTYPE *Next )( 
            IEnumCOFFSymRO * This,
            /* [in] */ ULONG __MIDL_0114,
            /* [out] */ ICOFFSymRO **__MIDL_0115,
            /* [out] */ ULONG *__MIDL_0116);
        
        HRESULT ( STDMETHODCALLTYPE *Skip )( 
            IEnumCOFFSymRO * This,
            /* [in] */ ULONG __MIDL_0117);
        
        HRESULT ( STDMETHODCALLTYPE *Reset )( 
            IEnumCOFFSymRO * This);
        
        HRESULT ( STDMETHODCALLTYPE *Clone )( 
            IEnumCOFFSymRO * This,
            /* [out] */ IEnumCOFFSymRO **__MIDL_0118);
        
        END_INTERFACE
    } IEnumCOFFSymROVtbl;

    interface IEnumCOFFSymRO
    {
        CONST_VTBL struct IEnumCOFFSymROVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IEnumCOFFSymRO_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IEnumCOFFSymRO_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IEnumCOFFSymRO_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IEnumCOFFSymRO_Next(This,__MIDL_0114,__MIDL_0115,__MIDL_0116)	\
    (This)->lpVtbl -> Next(This,__MIDL_0114,__MIDL_0115,__MIDL_0116)

#define IEnumCOFFSymRO_Skip(This,__MIDL_0117)	\
    (This)->lpVtbl -> Skip(This,__MIDL_0117)

#define IEnumCOFFSymRO_Reset(This)	\
    (This)->lpVtbl -> Reset(This)

#define IEnumCOFFSymRO_Clone(This,__MIDL_0118)	\
    (This)->lpVtbl -> Clone(This,__MIDL_0118)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IEnumCOFFSymRO_Next_Proxy( 
    IEnumCOFFSymRO * This,
    /* [in] */ ULONG __MIDL_0114,
    /* [out] */ ICOFFSymRO **__MIDL_0115,
    /* [out] */ ULONG *__MIDL_0116);


void __RPC_STUB IEnumCOFFSymRO_Next_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IEnumCOFFSymRO_Skip_Proxy( 
    IEnumCOFFSymRO * This,
    /* [in] */ ULONG __MIDL_0117);


void __RPC_STUB IEnumCOFFSymRO_Skip_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IEnumCOFFSymRO_Reset_Proxy( 
    IEnumCOFFSymRO * This);


void __RPC_STUB IEnumCOFFSymRO_Reset_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IEnumCOFFSymRO_Clone_Proxy( 
    IEnumCOFFSymRO * This,
    /* [out] */ IEnumCOFFSymRO **__MIDL_0118);


void __RPC_STUB IEnumCOFFSymRO_Clone_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IEnumCOFFSymRO_INTERFACE_DEFINED__ */


#ifndef __IEnumCOFFSymRW_INTERFACE_DEFINED__
#define __IEnumCOFFSymRW_INTERFACE_DEFINED__

/* interface IEnumCOFFSymRW */
/* [unique][uuid][object] */ 

typedef /* [unique] */ IEnumCOFFSymRW *PEnumCOFFSymRW;


EXTERN_C const IID IID_IEnumCOFFSymRW;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("01B3C322-B2FC-11d1-A71B-0060083E8C78")
    IEnumCOFFSymRW : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE Next( 
            /* [in] */ ULONG __MIDL_0119,
            /* [out] */ ICOFFSymRW **__MIDL_0120,
            /* [out] */ ULONG *__MIDL_0121) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Skip( 
            /* [in] */ ULONG __MIDL_0122) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Reset( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Clone( 
            /* [out] */ IEnumCOFFSymRW **__MIDL_0123) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IEnumCOFFSymRWVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IEnumCOFFSymRW * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IEnumCOFFSymRW * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IEnumCOFFSymRW * This);
        
        HRESULT ( STDMETHODCALLTYPE *Next )( 
            IEnumCOFFSymRW * This,
            /* [in] */ ULONG __MIDL_0119,
            /* [out] */ ICOFFSymRW **__MIDL_0120,
            /* [out] */ ULONG *__MIDL_0121);
        
        HRESULT ( STDMETHODCALLTYPE *Skip )( 
            IEnumCOFFSymRW * This,
            /* [in] */ ULONG __MIDL_0122);
        
        HRESULT ( STDMETHODCALLTYPE *Reset )( 
            IEnumCOFFSymRW * This);
        
        HRESULT ( STDMETHODCALLTYPE *Clone )( 
            IEnumCOFFSymRW * This,
            /* [out] */ IEnumCOFFSymRW **__MIDL_0123);
        
        END_INTERFACE
    } IEnumCOFFSymRWVtbl;

    interface IEnumCOFFSymRW
    {
        CONST_VTBL struct IEnumCOFFSymRWVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IEnumCOFFSymRW_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IEnumCOFFSymRW_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IEnumCOFFSymRW_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IEnumCOFFSymRW_Next(This,__MIDL_0119,__MIDL_0120,__MIDL_0121)	\
    (This)->lpVtbl -> Next(This,__MIDL_0119,__MIDL_0120,__MIDL_0121)

#define IEnumCOFFSymRW_Skip(This,__MIDL_0122)	\
    (This)->lpVtbl -> Skip(This,__MIDL_0122)

#define IEnumCOFFSymRW_Reset(This)	\
    (This)->lpVtbl -> Reset(This)

#define IEnumCOFFSymRW_Clone(This,__MIDL_0123)	\
    (This)->lpVtbl -> Clone(This,__MIDL_0123)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IEnumCOFFSymRW_Next_Proxy( 
    IEnumCOFFSymRW * This,
    /* [in] */ ULONG __MIDL_0119,
    /* [out] */ ICOFFSymRW **__MIDL_0120,
    /* [out] */ ULONG *__MIDL_0121);


void __RPC_STUB IEnumCOFFSymRW_Next_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IEnumCOFFSymRW_Skip_Proxy( 
    IEnumCOFFSymRW * This,
    /* [in] */ ULONG __MIDL_0122);


void __RPC_STUB IEnumCOFFSymRW_Skip_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IEnumCOFFSymRW_Reset_Proxy( 
    IEnumCOFFSymRW * This);


void __RPC_STUB IEnumCOFFSymRW_Reset_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IEnumCOFFSymRW_Clone_Proxy( 
    IEnumCOFFSymRW * This,
    /* [out] */ IEnumCOFFSymRW **__MIDL_0123);


void __RPC_STUB IEnumCOFFSymRW_Clone_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IEnumCOFFSymRW_INTERFACE_DEFINED__ */


#ifndef __ICOFFSymTabRO_INTERFACE_DEFINED__
#define __ICOFFSymTabRO_INTERFACE_DEFINED__

/* interface ICOFFSymTabRO */
/* [unique][uuid][object] */ 

typedef /* [unique] */ ICOFFSymTabRO *PCOFFSymTabRO;


EXTERN_C const IID IID_ICOFFSymTabRO;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("CBDFD0D0-B21F-11d1-A71B-0060083E8C78")
    ICOFFSymTabRO : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE CSymbols( 
            /* [out] */ DWORD *__MIDL_0124) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetCOFFSymRO( 
            /* [in] */ DWORD __MIDL_0125,
            /* [out] */ ICOFFSymRO **__MIDL_0126) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE ISYMNextCOFFSymRO( 
            /* [in] */ ICOFFSymRO *__MIDL_0127,
            /* [out] */ DWORD *__MIDL_0128) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE EmitCOFFSymbolTable( 
            /* [out] */ BYTE *__MIDL_0129,
            /* [in] */ DWORD __MIDL_0130) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE EnumCOFFSymRO( 
            /* [out] */ IEnumCOFFSymRO **__MIDL_0131) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct ICOFFSymTabROVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            ICOFFSymTabRO * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            ICOFFSymTabRO * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            ICOFFSymTabRO * This);
        
        HRESULT ( STDMETHODCALLTYPE *CSymbols )( 
            ICOFFSymTabRO * This,
            /* [out] */ DWORD *__MIDL_0124);
        
        HRESULT ( STDMETHODCALLTYPE *GetCOFFSymRO )( 
            ICOFFSymTabRO * This,
            /* [in] */ DWORD __MIDL_0125,
            /* [out] */ ICOFFSymRO **__MIDL_0126);
        
        HRESULT ( STDMETHODCALLTYPE *ISYMNextCOFFSymRO )( 
            ICOFFSymTabRO * This,
            /* [in] */ ICOFFSymRO *__MIDL_0127,
            /* [out] */ DWORD *__MIDL_0128);
        
        HRESULT ( STDMETHODCALLTYPE *EmitCOFFSymbolTable )( 
            ICOFFSymTabRO * This,
            /* [out] */ BYTE *__MIDL_0129,
            /* [in] */ DWORD __MIDL_0130);
        
        HRESULT ( STDMETHODCALLTYPE *EnumCOFFSymRO )( 
            ICOFFSymTabRO * This,
            /* [out] */ IEnumCOFFSymRO **__MIDL_0131);
        
        END_INTERFACE
    } ICOFFSymTabROVtbl;

    interface ICOFFSymTabRO
    {
        CONST_VTBL struct ICOFFSymTabROVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define ICOFFSymTabRO_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define ICOFFSymTabRO_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define ICOFFSymTabRO_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define ICOFFSymTabRO_CSymbols(This,__MIDL_0124)	\
    (This)->lpVtbl -> CSymbols(This,__MIDL_0124)

#define ICOFFSymTabRO_GetCOFFSymRO(This,__MIDL_0125,__MIDL_0126)	\
    (This)->lpVtbl -> GetCOFFSymRO(This,__MIDL_0125,__MIDL_0126)

#define ICOFFSymTabRO_ISYMNextCOFFSymRO(This,__MIDL_0127,__MIDL_0128)	\
    (This)->lpVtbl -> ISYMNextCOFFSymRO(This,__MIDL_0127,__MIDL_0128)

#define ICOFFSymTabRO_EmitCOFFSymbolTable(This,__MIDL_0129,__MIDL_0130)	\
    (This)->lpVtbl -> EmitCOFFSymbolTable(This,__MIDL_0129,__MIDL_0130)

#define ICOFFSymTabRO_EnumCOFFSymRO(This,__MIDL_0131)	\
    (This)->lpVtbl -> EnumCOFFSymRO(This,__MIDL_0131)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE ICOFFSymTabRO_CSymbols_Proxy( 
    ICOFFSymTabRO * This,
    /* [out] */ DWORD *__MIDL_0124);


void __RPC_STUB ICOFFSymTabRO_CSymbols_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ICOFFSymTabRO_GetCOFFSymRO_Proxy( 
    ICOFFSymTabRO * This,
    /* [in] */ DWORD __MIDL_0125,
    /* [out] */ ICOFFSymRO **__MIDL_0126);


void __RPC_STUB ICOFFSymTabRO_GetCOFFSymRO_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ICOFFSymTabRO_ISYMNextCOFFSymRO_Proxy( 
    ICOFFSymTabRO * This,
    /* [in] */ ICOFFSymRO *__MIDL_0127,
    /* [out] */ DWORD *__MIDL_0128);


void __RPC_STUB ICOFFSymTabRO_ISYMNextCOFFSymRO_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ICOFFSymTabRO_EmitCOFFSymbolTable_Proxy( 
    ICOFFSymTabRO * This,
    /* [out] */ BYTE *__MIDL_0129,
    /* [in] */ DWORD __MIDL_0130);


void __RPC_STUB ICOFFSymTabRO_EmitCOFFSymbolTable_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ICOFFSymTabRO_EnumCOFFSymRO_Proxy( 
    ICOFFSymTabRO * This,
    /* [out] */ IEnumCOFFSymRO **__MIDL_0131);


void __RPC_STUB ICOFFSymTabRO_EnumCOFFSymRO_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __ICOFFSymTabRO_INTERFACE_DEFINED__ */


#ifndef __ICOFFSymTabRW_INTERFACE_DEFINED__
#define __ICOFFSymTabRW_INTERFACE_DEFINED__

/* interface ICOFFSymTabRW */
/* [unique][uuid][object] */ 

typedef /* [unique] */ ICOFFSymTabRW *PCOFFSymTabRW;


EXTERN_C const IID IID_ICOFFSymTabRW;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("6D2C4950-B788-11d1-A71B-0060083E8C78")
    ICOFFSymTabRW : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE CSymbols( 
            /* [out] */ DWORD *__MIDL_0132) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetCOFFSymRW( 
            /* [in] */ DWORD __MIDL_0133,
            /* [out] */ ICOFFSymRW **__MIDL_0134) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE ISYMNextCOFFSymRW( 
            /* [in] */ ICOFFSymRW *__MIDL_0135,
            /* [out] */ DWORD *__MIDL_0136) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE EmitCOFFSymbolTable( 
            /* [out] */ BYTE *__MIDL_0137,
            /* [in] */ DWORD __MIDL_0138) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE EnumCOFFSymRW( 
            /* [out] */ IEnumCOFFSymRW **__MIDL_0139) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct ICOFFSymTabRWVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            ICOFFSymTabRW * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            ICOFFSymTabRW * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            ICOFFSymTabRW * This);
        
        HRESULT ( STDMETHODCALLTYPE *CSymbols )( 
            ICOFFSymTabRW * This,
            /* [out] */ DWORD *__MIDL_0132);
        
        HRESULT ( STDMETHODCALLTYPE *GetCOFFSymRW )( 
            ICOFFSymTabRW * This,
            /* [in] */ DWORD __MIDL_0133,
            /* [out] */ ICOFFSymRW **__MIDL_0134);
        
        HRESULT ( STDMETHODCALLTYPE *ISYMNextCOFFSymRW )( 
            ICOFFSymTabRW * This,
            /* [in] */ ICOFFSymRW *__MIDL_0135,
            /* [out] */ DWORD *__MIDL_0136);
        
        HRESULT ( STDMETHODCALLTYPE *EmitCOFFSymbolTable )( 
            ICOFFSymTabRW * This,
            /* [out] */ BYTE *__MIDL_0137,
            /* [in] */ DWORD __MIDL_0138);
        
        HRESULT ( STDMETHODCALLTYPE *EnumCOFFSymRW )( 
            ICOFFSymTabRW * This,
            /* [out] */ IEnumCOFFSymRW **__MIDL_0139);
        
        END_INTERFACE
    } ICOFFSymTabRWVtbl;

    interface ICOFFSymTabRW
    {
        CONST_VTBL struct ICOFFSymTabRWVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define ICOFFSymTabRW_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define ICOFFSymTabRW_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define ICOFFSymTabRW_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define ICOFFSymTabRW_CSymbols(This,__MIDL_0132)	\
    (This)->lpVtbl -> CSymbols(This,__MIDL_0132)

#define ICOFFSymTabRW_GetCOFFSymRW(This,__MIDL_0133,__MIDL_0134)	\
    (This)->lpVtbl -> GetCOFFSymRW(This,__MIDL_0133,__MIDL_0134)

#define ICOFFSymTabRW_ISYMNextCOFFSymRW(This,__MIDL_0135,__MIDL_0136)	\
    (This)->lpVtbl -> ISYMNextCOFFSymRW(This,__MIDL_0135,__MIDL_0136)

#define ICOFFSymTabRW_EmitCOFFSymbolTable(This,__MIDL_0137,__MIDL_0138)	\
    (This)->lpVtbl -> EmitCOFFSymbolTable(This,__MIDL_0137,__MIDL_0138)

#define ICOFFSymTabRW_EnumCOFFSymRW(This,__MIDL_0139)	\
    (This)->lpVtbl -> EnumCOFFSymRW(This,__MIDL_0139)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE ICOFFSymTabRW_CSymbols_Proxy( 
    ICOFFSymTabRW * This,
    /* [out] */ DWORD *__MIDL_0132);


void __RPC_STUB ICOFFSymTabRW_CSymbols_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ICOFFSymTabRW_GetCOFFSymRW_Proxy( 
    ICOFFSymTabRW * This,
    /* [in] */ DWORD __MIDL_0133,
    /* [out] */ ICOFFSymRW **__MIDL_0134);


void __RPC_STUB ICOFFSymTabRW_GetCOFFSymRW_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ICOFFSymTabRW_ISYMNextCOFFSymRW_Proxy( 
    ICOFFSymTabRW * This,
    /* [in] */ ICOFFSymRW *__MIDL_0135,
    /* [out] */ DWORD *__MIDL_0136);


void __RPC_STUB ICOFFSymTabRW_ISYMNextCOFFSymRW_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ICOFFSymTabRW_EmitCOFFSymbolTable_Proxy( 
    ICOFFSymTabRW * This,
    /* [out] */ BYTE *__MIDL_0137,
    /* [in] */ DWORD __MIDL_0138);


void __RPC_STUB ICOFFSymTabRW_EmitCOFFSymbolTable_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ICOFFSymTabRW_EnumCOFFSymRW_Proxy( 
    ICOFFSymTabRW * This,
    /* [out] */ IEnumCOFFSymRW **__MIDL_0139);


void __RPC_STUB ICOFFSymTabRW_EnumCOFFSymRW_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __ICOFFSymTabRW_INTERFACE_DEFINED__ */


#ifndef __ICOFFObj_INTERFACE_DEFINED__
#define __ICOFFObj_INTERFACE_DEFINED__

/* interface ICOFFObj */
/* [unique][uuid][object] */ 

typedef /* [unique] */ ICOFFObj *PCOFFObj;


EXTERN_C const IID IID_ICOFFObj;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("A9470BB0-911F-11d1-A712-0060083E8C78")
    ICOFFObj : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE BeginOptCaching( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE CContrib( 
            /* [out] */ DWORD *__MIDL_0140) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Close( 
            /* [in] */ BOOL __MIDL_0141) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE COFFTimeStamp( 
            /* [out] */ DWORD *__MIDL_0142) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE CSymbols( 
            /* [out] */ DWORD *__MIDL_0143) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE EndOptCaching( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE EnumContrib( 
            /* [out] */ IEnumContrib **__MIDL_0144) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetContribForISec( 
            /* [in] */ DWORD __MIDL_0145,
            /* [out] */ IObjectContrib **__MIDL_0146) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE FImportObj( 
            /* [out] */ BOOL *__MIDL_0147) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetStringTable( 
            /* [out] */ PLinkDataRO *__MIDL_0148) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetSymbolTableRO( 
            /* [out] */ ICOFFSymTabRO **__MIDL_0149) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetSymbolTableRW( 
            /* [out] */ ICOFFSymTabRW **__MIDL_0150) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE ObjtkFromSzSymbol( 
            /* [in] */ PLinkDataRO __MIDL_0151,
            /* [out] */ OBJTK *__MIDL_0152) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE TimeStamp( 
            /* [out] */ DWORD *__MIDL_0153) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SzRefForIsym( 
            /* [in] */ ISYM __MIDL_0154,
            /* [out] */ PLinkDataRO *__MIDL_0155) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct ICOFFObjVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            ICOFFObj * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            ICOFFObj * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            ICOFFObj * This);
        
        HRESULT ( STDMETHODCALLTYPE *BeginOptCaching )( 
            ICOFFObj * This);
        
        HRESULT ( STDMETHODCALLTYPE *CContrib )( 
            ICOFFObj * This,
            /* [out] */ DWORD *__MIDL_0140);
        
        HRESULT ( STDMETHODCALLTYPE *Close )( 
            ICOFFObj * This,
            /* [in] */ BOOL __MIDL_0141);
        
        HRESULT ( STDMETHODCALLTYPE *COFFTimeStamp )( 
            ICOFFObj * This,
            /* [out] */ DWORD *__MIDL_0142);
        
        HRESULT ( STDMETHODCALLTYPE *CSymbols )( 
            ICOFFObj * This,
            /* [out] */ DWORD *__MIDL_0143);
        
        HRESULT ( STDMETHODCALLTYPE *EndOptCaching )( 
            ICOFFObj * This);
        
        HRESULT ( STDMETHODCALLTYPE *EnumContrib )( 
            ICOFFObj * This,
            /* [out] */ IEnumContrib **__MIDL_0144);
        
        HRESULT ( STDMETHODCALLTYPE *GetContribForISec )( 
            ICOFFObj * This,
            /* [in] */ DWORD __MIDL_0145,
            /* [out] */ IObjectContrib **__MIDL_0146);
        
        HRESULT ( STDMETHODCALLTYPE *FImportObj )( 
            ICOFFObj * This,
            /* [out] */ BOOL *__MIDL_0147);
        
        HRESULT ( STDMETHODCALLTYPE *GetStringTable )( 
            ICOFFObj * This,
            /* [out] */ PLinkDataRO *__MIDL_0148);
        
        HRESULT ( STDMETHODCALLTYPE *GetSymbolTableRO )( 
            ICOFFObj * This,
            /* [out] */ ICOFFSymTabRO **__MIDL_0149);
        
        HRESULT ( STDMETHODCALLTYPE *GetSymbolTableRW )( 
            ICOFFObj * This,
            /* [out] */ ICOFFSymTabRW **__MIDL_0150);
        
        HRESULT ( STDMETHODCALLTYPE *ObjtkFromSzSymbol )( 
            ICOFFObj * This,
            /* [in] */ PLinkDataRO __MIDL_0151,
            /* [out] */ OBJTK *__MIDL_0152);
        
        HRESULT ( STDMETHODCALLTYPE *TimeStamp )( 
            ICOFFObj * This,
            /* [out] */ DWORD *__MIDL_0153);
        
        HRESULT ( STDMETHODCALLTYPE *SzRefForIsym )( 
            ICOFFObj * This,
            /* [in] */ ISYM __MIDL_0154,
            /* [out] */ PLinkDataRO *__MIDL_0155);
        
        END_INTERFACE
    } ICOFFObjVtbl;

    interface ICOFFObj
    {
        CONST_VTBL struct ICOFFObjVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define ICOFFObj_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define ICOFFObj_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define ICOFFObj_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define ICOFFObj_BeginOptCaching(This)	\
    (This)->lpVtbl -> BeginOptCaching(This)

#define ICOFFObj_CContrib(This,__MIDL_0140)	\
    (This)->lpVtbl -> CContrib(This,__MIDL_0140)

#define ICOFFObj_Close(This,__MIDL_0141)	\
    (This)->lpVtbl -> Close(This,__MIDL_0141)

#define ICOFFObj_COFFTimeStamp(This,__MIDL_0142)	\
    (This)->lpVtbl -> COFFTimeStamp(This,__MIDL_0142)

#define ICOFFObj_CSymbols(This,__MIDL_0143)	\
    (This)->lpVtbl -> CSymbols(This,__MIDL_0143)

#define ICOFFObj_EndOptCaching(This)	\
    (This)->lpVtbl -> EndOptCaching(This)

#define ICOFFObj_EnumContrib(This,__MIDL_0144)	\
    (This)->lpVtbl -> EnumContrib(This,__MIDL_0144)

#define ICOFFObj_GetContribForISec(This,__MIDL_0145,__MIDL_0146)	\
    (This)->lpVtbl -> GetContribForISec(This,__MIDL_0145,__MIDL_0146)

#define ICOFFObj_FImportObj(This,__MIDL_0147)	\
    (This)->lpVtbl -> FImportObj(This,__MIDL_0147)

#define ICOFFObj_GetStringTable(This,__MIDL_0148)	\
    (This)->lpVtbl -> GetStringTable(This,__MIDL_0148)

#define ICOFFObj_GetSymbolTableRO(This,__MIDL_0149)	\
    (This)->lpVtbl -> GetSymbolTableRO(This,__MIDL_0149)

#define ICOFFObj_GetSymbolTableRW(This,__MIDL_0150)	\
    (This)->lpVtbl -> GetSymbolTableRW(This,__MIDL_0150)

#define ICOFFObj_ObjtkFromSzSymbol(This,__MIDL_0151,__MIDL_0152)	\
    (This)->lpVtbl -> ObjtkFromSzSymbol(This,__MIDL_0151,__MIDL_0152)

#define ICOFFObj_TimeStamp(This,__MIDL_0153)	\
    (This)->lpVtbl -> TimeStamp(This,__MIDL_0153)

#define ICOFFObj_SzRefForIsym(This,__MIDL_0154,__MIDL_0155)	\
    (This)->lpVtbl -> SzRefForIsym(This,__MIDL_0154,__MIDL_0155)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE ICOFFObj_BeginOptCaching_Proxy( 
    ICOFFObj * This);


void __RPC_STUB ICOFFObj_BeginOptCaching_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ICOFFObj_CContrib_Proxy( 
    ICOFFObj * This,
    /* [out] */ DWORD *__MIDL_0140);


void __RPC_STUB ICOFFObj_CContrib_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ICOFFObj_Close_Proxy( 
    ICOFFObj * This,
    /* [in] */ BOOL __MIDL_0141);


void __RPC_STUB ICOFFObj_Close_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ICOFFObj_COFFTimeStamp_Proxy( 
    ICOFFObj * This,
    /* [out] */ DWORD *__MIDL_0142);


void __RPC_STUB ICOFFObj_COFFTimeStamp_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ICOFFObj_CSymbols_Proxy( 
    ICOFFObj * This,
    /* [out] */ DWORD *__MIDL_0143);


void __RPC_STUB ICOFFObj_CSymbols_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ICOFFObj_EndOptCaching_Proxy( 
    ICOFFObj * This);


void __RPC_STUB ICOFFObj_EndOptCaching_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ICOFFObj_EnumContrib_Proxy( 
    ICOFFObj * This,
    /* [out] */ IEnumContrib **__MIDL_0144);


void __RPC_STUB ICOFFObj_EnumContrib_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ICOFFObj_GetContribForISec_Proxy( 
    ICOFFObj * This,
    /* [in] */ DWORD __MIDL_0145,
    /* [out] */ IObjectContrib **__MIDL_0146);


void __RPC_STUB ICOFFObj_GetContribForISec_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ICOFFObj_FImportObj_Proxy( 
    ICOFFObj * This,
    /* [out] */ BOOL *__MIDL_0147);


void __RPC_STUB ICOFFObj_FImportObj_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ICOFFObj_GetStringTable_Proxy( 
    ICOFFObj * This,
    /* [out] */ PLinkDataRO *__MIDL_0148);


void __RPC_STUB ICOFFObj_GetStringTable_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ICOFFObj_GetSymbolTableRO_Proxy( 
    ICOFFObj * This,
    /* [out] */ ICOFFSymTabRO **__MIDL_0149);


void __RPC_STUB ICOFFObj_GetSymbolTableRO_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ICOFFObj_GetSymbolTableRW_Proxy( 
    ICOFFObj * This,
    /* [out] */ ICOFFSymTabRW **__MIDL_0150);


void __RPC_STUB ICOFFObj_GetSymbolTableRW_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ICOFFObj_ObjtkFromSzSymbol_Proxy( 
    ICOFFObj * This,
    /* [in] */ PLinkDataRO __MIDL_0151,
    /* [out] */ OBJTK *__MIDL_0152);


void __RPC_STUB ICOFFObj_ObjtkFromSzSymbol_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ICOFFObj_TimeStamp_Proxy( 
    ICOFFObj * This,
    /* [out] */ DWORD *__MIDL_0153);


void __RPC_STUB ICOFFObj_TimeStamp_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ICOFFObj_SzRefForIsym_Proxy( 
    ICOFFObj * This,
    /* [in] */ ISYM __MIDL_0154,
    /* [out] */ PLinkDataRO *__MIDL_0155);


void __RPC_STUB ICOFFObj_SzRefForIsym_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __ICOFFObj_INTERFACE_DEFINED__ */


/* interface __MIDL_itf_objint_0102 */
/* [local] */ 

typedef /* [public][public] */ 
enum __MIDL___MIDL_itf_objint_0102_0001
    {	tleMultDef	= 0,
	tleSecAttrib	= tleMultDef + 1,
	tleMessage	= tleSecAttrib + 1,
	tleWarning	= tleMessage + 1,
	tleError	= tleWarning + 1,
	tleFatal	= tleError + 1
    } 	TLE;



extern RPC_IF_HANDLE __MIDL_itf_objint_0102_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_objint_0102_v0_0_s_ifspec;

#ifndef __ILinkError_INTERFACE_DEFINED__
#define __ILinkError_INTERFACE_DEFINED__

/* interface ILinkError */
/* [unique][uuid][object] */ 


EXTERN_C const IID IID_ILinkError;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("2DEBF730-F5C3-11d1-A73F-0060083E8C78")
    ILinkError : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE ErrorType( 
            /* [out] */ TLE *__MIDL_0156) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Objtk1( 
            /* [out] */ OBJTK *__MIDL_0157) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Objtk2( 
            /* [out] */ OBJTK *__MIDL_0158) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Sz( 
            /* [out] */ PLinkDataRO *__MIDL_0159) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE DwInfo( 
            /* [out] */ DWORD *__MIDL_0160) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct ILinkErrorVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            ILinkError * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            ILinkError * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            ILinkError * This);
        
        HRESULT ( STDMETHODCALLTYPE *ErrorType )( 
            ILinkError * This,
            /* [out] */ TLE *__MIDL_0156);
        
        HRESULT ( STDMETHODCALLTYPE *Objtk1 )( 
            ILinkError * This,
            /* [out] */ OBJTK *__MIDL_0157);
        
        HRESULT ( STDMETHODCALLTYPE *Objtk2 )( 
            ILinkError * This,
            /* [out] */ OBJTK *__MIDL_0158);
        
        HRESULT ( STDMETHODCALLTYPE *Sz )( 
            ILinkError * This,
            /* [out] */ PLinkDataRO *__MIDL_0159);
        
        HRESULT ( STDMETHODCALLTYPE *DwInfo )( 
            ILinkError * This,
            /* [out] */ DWORD *__MIDL_0160);
        
        END_INTERFACE
    } ILinkErrorVtbl;

    interface ILinkError
    {
        CONST_VTBL struct ILinkErrorVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define ILinkError_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define ILinkError_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define ILinkError_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define ILinkError_ErrorType(This,__MIDL_0156)	\
    (This)->lpVtbl -> ErrorType(This,__MIDL_0156)

#define ILinkError_Objtk1(This,__MIDL_0157)	\
    (This)->lpVtbl -> Objtk1(This,__MIDL_0157)

#define ILinkError_Objtk2(This,__MIDL_0158)	\
    (This)->lpVtbl -> Objtk2(This,__MIDL_0158)

#define ILinkError_Sz(This,__MIDL_0159)	\
    (This)->lpVtbl -> Sz(This,__MIDL_0159)

#define ILinkError_DwInfo(This,__MIDL_0160)	\
    (This)->lpVtbl -> DwInfo(This,__MIDL_0160)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE ILinkError_ErrorType_Proxy( 
    ILinkError * This,
    /* [out] */ TLE *__MIDL_0156);


void __RPC_STUB ILinkError_ErrorType_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ILinkError_Objtk1_Proxy( 
    ILinkError * This,
    /* [out] */ OBJTK *__MIDL_0157);


void __RPC_STUB ILinkError_Objtk1_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ILinkError_Objtk2_Proxy( 
    ILinkError * This,
    /* [out] */ OBJTK *__MIDL_0158);


void __RPC_STUB ILinkError_Objtk2_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ILinkError_Sz_Proxy( 
    ILinkError * This,
    /* [out] */ PLinkDataRO *__MIDL_0159);


void __RPC_STUB ILinkError_Sz_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ILinkError_DwInfo_Proxy( 
    ILinkError * This,
    /* [out] */ DWORD *__MIDL_0160);


void __RPC_STUB ILinkError_DwInfo_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __ILinkError_INTERFACE_DEFINED__ */


#ifndef __ILinkNotify_INTERFACE_DEFINED__
#define __ILinkNotify_INTERFACE_DEFINED__

/* interface ILinkNotify */
/* [unique][uuid][object] */ 

typedef /* [unique] */ ILinkNotify *PLinkNotify;


EXTERN_C const IID IID_ILinkNotify;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("9D7E97A0-F5B6-11d1-A73F-0060083E8C78")
    ILinkNotify : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE OnError( 
            /* [in] */ ILinkError *__MIDL_0161) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE OnStatus( 
            /* [in] */ DWORD __MIDL_0162,
            /* [in] */ DWORD __MIDL_0163) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct ILinkNotifyVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            ILinkNotify * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            ILinkNotify * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            ILinkNotify * This);
        
        HRESULT ( STDMETHODCALLTYPE *OnError )( 
            ILinkNotify * This,
            /* [in] */ ILinkError *__MIDL_0161);
        
        HRESULT ( STDMETHODCALLTYPE *OnStatus )( 
            ILinkNotify * This,
            /* [in] */ DWORD __MIDL_0162,
            /* [in] */ DWORD __MIDL_0163);
        
        END_INTERFACE
    } ILinkNotifyVtbl;

    interface ILinkNotify
    {
        CONST_VTBL struct ILinkNotifyVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define ILinkNotify_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define ILinkNotify_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define ILinkNotify_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define ILinkNotify_OnError(This,__MIDL_0161)	\
    (This)->lpVtbl -> OnError(This,__MIDL_0161)

#define ILinkNotify_OnStatus(This,__MIDL_0162,__MIDL_0163)	\
    (This)->lpVtbl -> OnStatus(This,__MIDL_0162,__MIDL_0163)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE ILinkNotify_OnError_Proxy( 
    ILinkNotify * This,
    /* [in] */ ILinkError *__MIDL_0161);


void __RPC_STUB ILinkNotify_OnError_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ILinkNotify_OnStatus_Proxy( 
    ILinkNotify * This,
    /* [in] */ DWORD __MIDL_0162,
    /* [in] */ DWORD __MIDL_0163);


void __RPC_STUB ILinkNotify_OnStatus_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __ILinkNotify_INTERFACE_DEFINED__ */


/* Additional Prototypes for ALL interfaces */

unsigned long             __RPC_USER  BSTR_UserSize(     unsigned long *, unsigned long            , BSTR * ); 
unsigned char * __RPC_USER  BSTR_UserMarshal(  unsigned long *, unsigned char *, BSTR * ); 
unsigned char * __RPC_USER  BSTR_UserUnmarshal(unsigned long *, unsigned char *, BSTR * ); 
void                      __RPC_USER  BSTR_UserFree(     unsigned long *, BSTR * ); 

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif


