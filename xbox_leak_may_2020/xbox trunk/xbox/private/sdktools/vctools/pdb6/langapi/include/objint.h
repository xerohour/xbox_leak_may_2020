/* this ALWAYS GENERATED file contains the definitions for the interfaces */


/* File created by MIDL compiler version 3.03.0110 */
/* at Wed Jun 10 17:42:56 1998
 */
/* Compiler settings for ..\idl\objint.idl:
    Os (OptLev=s), W1, Zp8, env=Win32, ms_ext, c_ext
    error checks: none
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

#ifdef __cplusplus
extern "C"{
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

void __RPC_FAR * __RPC_USER MIDL_user_allocate(size_t);
void __RPC_USER MIDL_user_free( void __RPC_FAR * ); 

/****************************************
 * Generated header for interface: __MIDL_itf_objint_0000
 * at Wed Jun 10 17:42:56 1998
 * using MIDL 3.03.0110
 ****************************************/
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
    }	PSYMT;

typedef DWORDLONG OBJTK;

typedef /* [public] */ 
enum __MIDL___MIDL_itf_objint_0000_0002
    {	objtktNil	= 0
    }	OBJTKT;

typedef DWORD ISYM;



extern RPC_IF_HANDLE __MIDL_itf_objint_0000_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_objint_0000_v0_0_s_ifspec;

#ifndef __ILinkDataRO_INTERFACE_DEFINED__
#define __ILinkDataRO_INTERFACE_DEFINED__

/****************************************
 * Generated header for interface: ILinkDataRO
 * at Wed Jun 10 17:42:56 1998
 * using MIDL 3.03.0110
 ****************************************/
/* [unique][uuid][object] */ 


typedef /* [unique] */ ILinkDataRO __RPC_FAR *PLinkDataRO;


EXTERN_C const IID IID_ILinkDataRO;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("0CE9B320-AE0F-11d1-A719-0060083E8C78")
    ILinkDataRO : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE GetDataPointer( 
            /* [out] */ BYTE __RPC_FAR *__RPC_FAR *__MIDL_0008,
            /* [out] */ int __RPC_FAR *__MIDL_0009) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct ILinkDataROVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            ILinkDataRO __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            ILinkDataRO __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            ILinkDataRO __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetDataPointer )( 
            ILinkDataRO __RPC_FAR * This,
            /* [out] */ BYTE __RPC_FAR *__RPC_FAR *__MIDL_0008,
            /* [out] */ int __RPC_FAR *__MIDL_0009);
        
        END_INTERFACE
    } ILinkDataROVtbl;

    interface ILinkDataRO
    {
        CONST_VTBL struct ILinkDataROVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define ILinkDataRO_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define ILinkDataRO_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define ILinkDataRO_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define ILinkDataRO_GetDataPointer(This,__MIDL_0008,__MIDL_0009)	\
    (This)->lpVtbl -> GetDataPointer(This,__MIDL_0008,__MIDL_0009)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE ILinkDataRO_GetDataPointer_Proxy( 
    ILinkDataRO __RPC_FAR * This,
    /* [out] */ BYTE __RPC_FAR *__RPC_FAR *__MIDL_0008,
    /* [out] */ int __RPC_FAR *__MIDL_0009);


void __RPC_STUB ILinkDataRO_GetDataPointer_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __ILinkDataRO_INTERFACE_DEFINED__ */


#ifndef __ILinkDataRW_INTERFACE_DEFINED__
#define __ILinkDataRW_INTERFACE_DEFINED__

/****************************************
 * Generated header for interface: ILinkDataRW
 * at Wed Jun 10 17:42:56 1998
 * using MIDL 3.03.0110
 ****************************************/
/* [unique][uuid][object] */ 


typedef /* [unique] */ ILinkDataRW __RPC_FAR *PLinkDataRW;


EXTERN_C const IID IID_ILinkDataRW;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("DB97C491-AE41-11d1-A719-0060083E8C78")
    ILinkDataRW : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE GetDataPointer( 
            /* [out] */ BYTE __RPC_FAR *__RPC_FAR *__MIDL_0010,
            /* [out] */ int __RPC_FAR *__MIDL_0011) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct ILinkDataRWVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            ILinkDataRW __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            ILinkDataRW __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            ILinkDataRW __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetDataPointer )( 
            ILinkDataRW __RPC_FAR * This,
            /* [out] */ BYTE __RPC_FAR *__RPC_FAR *__MIDL_0010,
            /* [out] */ int __RPC_FAR *__MIDL_0011);
        
        END_INTERFACE
    } ILinkDataRWVtbl;

    interface ILinkDataRW
    {
        CONST_VTBL struct ILinkDataRWVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define ILinkDataRW_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define ILinkDataRW_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define ILinkDataRW_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define ILinkDataRW_GetDataPointer(This,__MIDL_0010,__MIDL_0011)	\
    (This)->lpVtbl -> GetDataPointer(This,__MIDL_0010,__MIDL_0011)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE ILinkDataRW_GetDataPointer_Proxy( 
    ILinkDataRW __RPC_FAR * This,
    /* [out] */ BYTE __RPC_FAR *__RPC_FAR *__MIDL_0010,
    /* [out] */ int __RPC_FAR *__MIDL_0011);


void __RPC_STUB ILinkDataRW_GetDataPointer_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __ILinkDataRW_INTERFACE_DEFINED__ */


#ifndef __IPublicSym_INTERFACE_DEFINED__
#define __IPublicSym_INTERFACE_DEFINED__

/****************************************
 * Generated header for interface: IPublicSym
 * at Wed Jun 10 17:42:56 1998
 * using MIDL 3.03.0110
 ****************************************/
/* [unique][uuid][object] */ 


typedef /* [unique] */ IPublicSym __RPC_FAR *LPPubSym;


EXTERN_C const IID IID_IPublicSym;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("18851AD0-910F-11d1-A712-0060083E8C78")
    IPublicSym : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE FComdat( 
            /* [out] */ BOOL __RPC_FAR *__MIDL_0012,
            /* [out] */ BYTE __RPC_FAR *__MIDL_0013) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE ISym( 
            /* [out] */ ISYM __RPC_FAR *__MIDL_0014) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE TkFile( 
            /* [out] */ OBJTK __RPC_FAR *__MIDL_0015) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SzName( 
            /* [out] */ PLinkDataRO __RPC_FAR *__MIDL_0016) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SymType( 
            /* [out] */ PSYMT __RPC_FAR *__MIDL_0017) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SzNameDefaultSym( 
            /* [out] */ PLinkDataRO __RPC_FAR *__MIDL_0018) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IPublicSymVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IPublicSym __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IPublicSym __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IPublicSym __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *FComdat )( 
            IPublicSym __RPC_FAR * This,
            /* [out] */ BOOL __RPC_FAR *__MIDL_0012,
            /* [out] */ BYTE __RPC_FAR *__MIDL_0013);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *ISym )( 
            IPublicSym __RPC_FAR * This,
            /* [out] */ ISYM __RPC_FAR *__MIDL_0014);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *TkFile )( 
            IPublicSym __RPC_FAR * This,
            /* [out] */ OBJTK __RPC_FAR *__MIDL_0015);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *SzName )( 
            IPublicSym __RPC_FAR * This,
            /* [out] */ PLinkDataRO __RPC_FAR *__MIDL_0016);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *SymType )( 
            IPublicSym __RPC_FAR * This,
            /* [out] */ PSYMT __RPC_FAR *__MIDL_0017);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *SzNameDefaultSym )( 
            IPublicSym __RPC_FAR * This,
            /* [out] */ PLinkDataRO __RPC_FAR *__MIDL_0018);
        
        END_INTERFACE
    } IPublicSymVtbl;

    interface IPublicSym
    {
        CONST_VTBL struct IPublicSymVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IPublicSym_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IPublicSym_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IPublicSym_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IPublicSym_FComdat(This,__MIDL_0012,__MIDL_0013)	\
    (This)->lpVtbl -> FComdat(This,__MIDL_0012,__MIDL_0013)

#define IPublicSym_ISym(This,__MIDL_0014)	\
    (This)->lpVtbl -> ISym(This,__MIDL_0014)

#define IPublicSym_TkFile(This,__MIDL_0015)	\
    (This)->lpVtbl -> TkFile(This,__MIDL_0015)

#define IPublicSym_SzName(This,__MIDL_0016)	\
    (This)->lpVtbl -> SzName(This,__MIDL_0016)

#define IPublicSym_SymType(This,__MIDL_0017)	\
    (This)->lpVtbl -> SymType(This,__MIDL_0017)

#define IPublicSym_SzNameDefaultSym(This,__MIDL_0018)	\
    (This)->lpVtbl -> SzNameDefaultSym(This,__MIDL_0018)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IPublicSym_FComdat_Proxy( 
    IPublicSym __RPC_FAR * This,
    /* [out] */ BOOL __RPC_FAR *__MIDL_0012,
    /* [out] */ BYTE __RPC_FAR *__MIDL_0013);


void __RPC_STUB IPublicSym_FComdat_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IPublicSym_ISym_Proxy( 
    IPublicSym __RPC_FAR * This,
    /* [out] */ ISYM __RPC_FAR *__MIDL_0014);


void __RPC_STUB IPublicSym_ISym_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IPublicSym_TkFile_Proxy( 
    IPublicSym __RPC_FAR * This,
    /* [out] */ OBJTK __RPC_FAR *__MIDL_0015);


void __RPC_STUB IPublicSym_TkFile_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IPublicSym_SzName_Proxy( 
    IPublicSym __RPC_FAR * This,
    /* [out] */ PLinkDataRO __RPC_FAR *__MIDL_0016);


void __RPC_STUB IPublicSym_SzName_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IPublicSym_SymType_Proxy( 
    IPublicSym __RPC_FAR * This,
    /* [out] */ PSYMT __RPC_FAR *__MIDL_0017);


void __RPC_STUB IPublicSym_SymType_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IPublicSym_SzNameDefaultSym_Proxy( 
    IPublicSym __RPC_FAR * This,
    /* [out] */ PLinkDataRO __RPC_FAR *__MIDL_0018);


void __RPC_STUB IPublicSym_SzNameDefaultSym_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IPublicSym_INTERFACE_DEFINED__ */


#ifndef __IEnumPublics_INTERFACE_DEFINED__
#define __IEnumPublics_INTERFACE_DEFINED__

/****************************************
 * Generated header for interface: IEnumPublics
 * at Wed Jun 10 17:42:56 1998
 * using MIDL 3.03.0110
 ****************************************/
/* [unique][uuid][object] */ 


typedef /* [unique] */ IEnumPublics __RPC_FAR *LPEnumPub;


EXTERN_C const IID IID_IEnumPublics;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("3BFA6690-9113-11d1-A712-0060083E8C78")
    IEnumPublics : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE Next( 
            /* [in] */ ULONG __MIDL_0019,
            /* [out] */ IPublicSym __RPC_FAR *__RPC_FAR *__MIDL_0020,
            /* [out] */ ULONG __RPC_FAR *__MIDL_0021) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Skip( 
            /* [in] */ ULONG __MIDL_0022) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Reset( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Clone( 
            /* [out] */ IEnumPublics __RPC_FAR *__RPC_FAR *__MIDL_0023) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IEnumPublicsVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IEnumPublics __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IEnumPublics __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IEnumPublics __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Next )( 
            IEnumPublics __RPC_FAR * This,
            /* [in] */ ULONG __MIDL_0019,
            /* [out] */ IPublicSym __RPC_FAR *__RPC_FAR *__MIDL_0020,
            /* [out] */ ULONG __RPC_FAR *__MIDL_0021);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Skip )( 
            IEnumPublics __RPC_FAR * This,
            /* [in] */ ULONG __MIDL_0022);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Reset )( 
            IEnumPublics __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Clone )( 
            IEnumPublics __RPC_FAR * This,
            /* [out] */ IEnumPublics __RPC_FAR *__RPC_FAR *__MIDL_0023);
        
        END_INTERFACE
    } IEnumPublicsVtbl;

    interface IEnumPublics
    {
        CONST_VTBL struct IEnumPublicsVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IEnumPublics_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IEnumPublics_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IEnumPublics_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IEnumPublics_Next(This,__MIDL_0019,__MIDL_0020,__MIDL_0021)	\
    (This)->lpVtbl -> Next(This,__MIDL_0019,__MIDL_0020,__MIDL_0021)

#define IEnumPublics_Skip(This,__MIDL_0022)	\
    (This)->lpVtbl -> Skip(This,__MIDL_0022)

#define IEnumPublics_Reset(This)	\
    (This)->lpVtbl -> Reset(This)

#define IEnumPublics_Clone(This,__MIDL_0023)	\
    (This)->lpVtbl -> Clone(This,__MIDL_0023)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IEnumPublics_Next_Proxy( 
    IEnumPublics __RPC_FAR * This,
    /* [in] */ ULONG __MIDL_0019,
    /* [out] */ IPublicSym __RPC_FAR *__RPC_FAR *__MIDL_0020,
    /* [out] */ ULONG __RPC_FAR *__MIDL_0021);


void __RPC_STUB IEnumPublics_Next_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IEnumPublics_Skip_Proxy( 
    IEnumPublics __RPC_FAR * This,
    /* [in] */ ULONG __MIDL_0022);


void __RPC_STUB IEnumPublics_Skip_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IEnumPublics_Reset_Proxy( 
    IEnumPublics __RPC_FAR * This);


void __RPC_STUB IEnumPublics_Reset_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IEnumPublics_Clone_Proxy( 
    IEnumPublics __RPC_FAR * This,
    /* [out] */ IEnumPublics __RPC_FAR *__RPC_FAR *__MIDL_0023);


void __RPC_STUB IEnumPublics_Clone_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IEnumPublics_INTERFACE_DEFINED__ */


#ifndef __IObjFile_INTERFACE_DEFINED__
#define __IObjFile_INTERFACE_DEFINED__

/****************************************
 * Generated header for interface: IObjFile
 * at Wed Jun 10 17:42:56 1998
 * using MIDL 3.03.0110
 ****************************************/
/* [unique][uuid][object] */ 



EXTERN_C const IID IID_IObjFile;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("5AEA3330-911D-11d1-A712-0060083E8C78")
    IObjFile : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE Characteristics( 
            /* [out] */ DWORD __RPC_FAR *__MIDL_0024) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE CloseCache( 
            /* [in] */ BOOL __MIDL_0025) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE EnumPublics( 
            /* [out] */ IEnumPublics __RPC_FAR *__RPC_FAR *__MIDL_0026,
            /* [in] */ OBJTK __MIDL_0027) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE FNative( 
            BOOL __RPC_FAR *__MIDL_0028) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Machine( 
            /* [out] */ WORD __RPC_FAR *__MIDL_0029) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE PbDirectives( 
            /* [out] */ PLinkDataRO __RPC_FAR *__MIDL_0030) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE RgISymForeignRefs( 
            /* [out] */ PLinkDataRO __RPC_FAR *__MIDL_0031) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IObjFileVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IObjFile __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IObjFile __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IObjFile __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Characteristics )( 
            IObjFile __RPC_FAR * This,
            /* [out] */ DWORD __RPC_FAR *__MIDL_0024);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *CloseCache )( 
            IObjFile __RPC_FAR * This,
            /* [in] */ BOOL __MIDL_0025);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *EnumPublics )( 
            IObjFile __RPC_FAR * This,
            /* [out] */ IEnumPublics __RPC_FAR *__RPC_FAR *__MIDL_0026,
            /* [in] */ OBJTK __MIDL_0027);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *FNative )( 
            IObjFile __RPC_FAR * This,
            BOOL __RPC_FAR *__MIDL_0028);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Machine )( 
            IObjFile __RPC_FAR * This,
            /* [out] */ WORD __RPC_FAR *__MIDL_0029);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *PbDirectives )( 
            IObjFile __RPC_FAR * This,
            /* [out] */ PLinkDataRO __RPC_FAR *__MIDL_0030);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *RgISymForeignRefs )( 
            IObjFile __RPC_FAR * This,
            /* [out] */ PLinkDataRO __RPC_FAR *__MIDL_0031);
        
        END_INTERFACE
    } IObjFileVtbl;

    interface IObjFile
    {
        CONST_VTBL struct IObjFileVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IObjFile_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IObjFile_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IObjFile_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IObjFile_Characteristics(This,__MIDL_0024)	\
    (This)->lpVtbl -> Characteristics(This,__MIDL_0024)

#define IObjFile_CloseCache(This,__MIDL_0025)	\
    (This)->lpVtbl -> CloseCache(This,__MIDL_0025)

#define IObjFile_EnumPublics(This,__MIDL_0026,__MIDL_0027)	\
    (This)->lpVtbl -> EnumPublics(This,__MIDL_0026,__MIDL_0027)

#define IObjFile_FNative(This,__MIDL_0028)	\
    (This)->lpVtbl -> FNative(This,__MIDL_0028)

#define IObjFile_Machine(This,__MIDL_0029)	\
    (This)->lpVtbl -> Machine(This,__MIDL_0029)

#define IObjFile_PbDirectives(This,__MIDL_0030)	\
    (This)->lpVtbl -> PbDirectives(This,__MIDL_0030)

#define IObjFile_RgISymForeignRefs(This,__MIDL_0031)	\
    (This)->lpVtbl -> RgISymForeignRefs(This,__MIDL_0031)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IObjFile_Characteristics_Proxy( 
    IObjFile __RPC_FAR * This,
    /* [out] */ DWORD __RPC_FAR *__MIDL_0024);


void __RPC_STUB IObjFile_Characteristics_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IObjFile_CloseCache_Proxy( 
    IObjFile __RPC_FAR * This,
    /* [in] */ BOOL __MIDL_0025);


void __RPC_STUB IObjFile_CloseCache_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IObjFile_EnumPublics_Proxy( 
    IObjFile __RPC_FAR * This,
    /* [out] */ IEnumPublics __RPC_FAR *__RPC_FAR *__MIDL_0026,
    /* [in] */ OBJTK __MIDL_0027);


void __RPC_STUB IObjFile_EnumPublics_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IObjFile_FNative_Proxy( 
    IObjFile __RPC_FAR * This,
    BOOL __RPC_FAR *__MIDL_0028);


void __RPC_STUB IObjFile_FNative_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IObjFile_Machine_Proxy( 
    IObjFile __RPC_FAR * This,
    /* [out] */ WORD __RPC_FAR *__MIDL_0029);


void __RPC_STUB IObjFile_Machine_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IObjFile_PbDirectives_Proxy( 
    IObjFile __RPC_FAR * This,
    /* [out] */ PLinkDataRO __RPC_FAR *__MIDL_0030);


void __RPC_STUB IObjFile_PbDirectives_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IObjFile_RgISymForeignRefs_Proxy( 
    IObjFile __RPC_FAR * This,
    /* [out] */ PLinkDataRO __RPC_FAR *__MIDL_0031);


void __RPC_STUB IObjFile_RgISymForeignRefs_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IObjFile_INTERFACE_DEFINED__ */


#ifndef __IObjHandler_INTERFACE_DEFINED__
#define __IObjHandler_INTERFACE_DEFINED__

/****************************************
 * Generated header for interface: IObjHandler
 * at Wed Jun 10 17:42:56 1998
 * using MIDL 3.03.0110
 ****************************************/
/* [unique][uuid][object] */ 


typedef /* [unique] */ IObjHandler __RPC_FAR *PObjHandler;


EXTERN_C const IID IID_IObjHandler;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("5CDF16C0-FFC6-11d1-A740-0060083E8C78")
    IObjHandler : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE CodeGen( 
            /* [in] */ int __MIDL_0032,
            /* [in] */ unsigned char __RPC_FAR *__RPC_FAR *__MIDL_0033,
            /* [in] */ PLinkDataRO __MIDL_0034) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Init( 
            /* [in] */ IUnknown __RPC_FAR *__MIDL_0035) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE IObjFileForFile( 
            /* [in] */ BSTR __MIDL_0036,
            /* [in] */ WORD __MIDL_0037,
            /* [in] */ DWORD __MIDL_0038,
            /* [in] */ DWORD __MIDL_0039,
            /* [in] */ OBJTK __MIDL_0040,
            /* [out] */ IObjFile __RPC_FAR *__RPC_FAR *__MIDL_0041) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE OnCtrlC( void) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IObjHandlerVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IObjHandler __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IObjHandler __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IObjHandler __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *CodeGen )( 
            IObjHandler __RPC_FAR * This,
            /* [in] */ int __MIDL_0032,
            /* [in] */ unsigned char __RPC_FAR *__RPC_FAR *__MIDL_0033,
            /* [in] */ PLinkDataRO __MIDL_0034);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Init )( 
            IObjHandler __RPC_FAR * This,
            /* [in] */ IUnknown __RPC_FAR *__MIDL_0035);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *IObjFileForFile )( 
            IObjHandler __RPC_FAR * This,
            /* [in] */ BSTR __MIDL_0036,
            /* [in] */ WORD __MIDL_0037,
            /* [in] */ DWORD __MIDL_0038,
            /* [in] */ DWORD __MIDL_0039,
            /* [in] */ OBJTK __MIDL_0040,
            /* [out] */ IObjFile __RPC_FAR *__RPC_FAR *__MIDL_0041);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *OnCtrlC )( 
            IObjHandler __RPC_FAR * This);
        
        END_INTERFACE
    } IObjHandlerVtbl;

    interface IObjHandler
    {
        CONST_VTBL struct IObjHandlerVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IObjHandler_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IObjHandler_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IObjHandler_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IObjHandler_CodeGen(This,__MIDL_0032,__MIDL_0033,__MIDL_0034)	\
    (This)->lpVtbl -> CodeGen(This,__MIDL_0032,__MIDL_0033,__MIDL_0034)

#define IObjHandler_Init(This,__MIDL_0035)	\
    (This)->lpVtbl -> Init(This,__MIDL_0035)

#define IObjHandler_IObjFileForFile(This,__MIDL_0036,__MIDL_0037,__MIDL_0038,__MIDL_0039,__MIDL_0040,__MIDL_0041)	\
    (This)->lpVtbl -> IObjFileForFile(This,__MIDL_0036,__MIDL_0037,__MIDL_0038,__MIDL_0039,__MIDL_0040,__MIDL_0041)

#define IObjHandler_OnCtrlC(This)	\
    (This)->lpVtbl -> OnCtrlC(This)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IObjHandler_CodeGen_Proxy( 
    IObjHandler __RPC_FAR * This,
    /* [in] */ int __MIDL_0032,
    /* [in] */ unsigned char __RPC_FAR *__RPC_FAR *__MIDL_0033,
    /* [in] */ PLinkDataRO __MIDL_0034);


void __RPC_STUB IObjHandler_CodeGen_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IObjHandler_Init_Proxy( 
    IObjHandler __RPC_FAR * This,
    /* [in] */ IUnknown __RPC_FAR *__MIDL_0035);


void __RPC_STUB IObjHandler_Init_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IObjHandler_IObjFileForFile_Proxy( 
    IObjHandler __RPC_FAR * This,
    /* [in] */ BSTR __MIDL_0036,
    /* [in] */ WORD __MIDL_0037,
    /* [in] */ DWORD __MIDL_0038,
    /* [in] */ DWORD __MIDL_0039,
    /* [in] */ OBJTK __MIDL_0040,
    /* [out] */ IObjFile __RPC_FAR *__RPC_FAR *__MIDL_0041);


void __RPC_STUB IObjHandler_IObjFileForFile_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IObjHandler_OnCtrlC_Proxy( 
    IObjHandler __RPC_FAR * This);


void __RPC_STUB IObjHandler_OnCtrlC_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IObjHandler_INTERFACE_DEFINED__ */


#ifndef __IObjectContrib_INTERFACE_DEFINED__
#define __IObjectContrib_INTERFACE_DEFINED__

/****************************************
 * Generated header for interface: IObjectContrib
 * at Wed Jun 10 17:42:56 1998
 * using MIDL 3.03.0110
 ****************************************/
/* [unique][uuid][object] */ 


typedef /* [unique] */ IObjectContrib __RPC_FAR *PObjContrib;


EXTERN_C const IID IID_IObjectContrib;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("A461E470-911E-11d1-A712-0060083E8C78")
    IObjectContrib : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE CbContents( 
            /* [out] */ DWORD __RPC_FAR *__MIDL_0042) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Characteristics( 
            /* [out] */ DWORD __RPC_FAR *__MIDL_0043) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE CLineNo( 
            /* [out] */ DWORD __RPC_FAR *__MIDL_0044) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE CRelocs( 
            /* [out] */ DWORD __RPC_FAR *__MIDL_0045) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE ISection( 
            /* [out] */ DWORD __RPC_FAR *__MIDL_0046) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE PbContents( 
            /* [out] */ BYTE __RPC_FAR *__MIDL_0047,
            /* [in] */ DWORD __MIDL_0048,
            /* [out] */ DWORD __RPC_FAR *__MIDL_0049) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE PbContentsRO( 
            /* [out] */ PLinkDataRO __RPC_FAR *__MIDL_0050) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE PbContentsRW( 
            /* [out] */ PLinkDataRW __RPC_FAR *__MIDL_0051) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE RgLineNo( 
            /* [out] */ PLinkDataRO __RPC_FAR *__MIDL_0052) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE RgRelocs( 
            /* [out] */ PLinkDataRO __RPC_FAR *__MIDL_0053) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE RgRelocsEx( 
            /* [out] */ BYTE __RPC_FAR *__MIDL_0054,
            /* [in] */ DWORD __MIDL_0055) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SzSectionName( 
            /* [out] */ PLinkDataRO __RPC_FAR *__MIDL_0056) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE TkFile( 
            /* [out] */ OBJTK __RPC_FAR *__MIDL_0057) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE VirtualAddress( 
            /* [out] */ DWORD __RPC_FAR *__MIDL_0058) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IObjectContribVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IObjectContrib __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IObjectContrib __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IObjectContrib __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *CbContents )( 
            IObjectContrib __RPC_FAR * This,
            /* [out] */ DWORD __RPC_FAR *__MIDL_0042);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Characteristics )( 
            IObjectContrib __RPC_FAR * This,
            /* [out] */ DWORD __RPC_FAR *__MIDL_0043);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *CLineNo )( 
            IObjectContrib __RPC_FAR * This,
            /* [out] */ DWORD __RPC_FAR *__MIDL_0044);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *CRelocs )( 
            IObjectContrib __RPC_FAR * This,
            /* [out] */ DWORD __RPC_FAR *__MIDL_0045);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *ISection )( 
            IObjectContrib __RPC_FAR * This,
            /* [out] */ DWORD __RPC_FAR *__MIDL_0046);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *PbContents )( 
            IObjectContrib __RPC_FAR * This,
            /* [out] */ BYTE __RPC_FAR *__MIDL_0047,
            /* [in] */ DWORD __MIDL_0048,
            /* [out] */ DWORD __RPC_FAR *__MIDL_0049);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *PbContentsRO )( 
            IObjectContrib __RPC_FAR * This,
            /* [out] */ PLinkDataRO __RPC_FAR *__MIDL_0050);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *PbContentsRW )( 
            IObjectContrib __RPC_FAR * This,
            /* [out] */ PLinkDataRW __RPC_FAR *__MIDL_0051);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *RgLineNo )( 
            IObjectContrib __RPC_FAR * This,
            /* [out] */ PLinkDataRO __RPC_FAR *__MIDL_0052);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *RgRelocs )( 
            IObjectContrib __RPC_FAR * This,
            /* [out] */ PLinkDataRO __RPC_FAR *__MIDL_0053);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *RgRelocsEx )( 
            IObjectContrib __RPC_FAR * This,
            /* [out] */ BYTE __RPC_FAR *__MIDL_0054,
            /* [in] */ DWORD __MIDL_0055);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *SzSectionName )( 
            IObjectContrib __RPC_FAR * This,
            /* [out] */ PLinkDataRO __RPC_FAR *__MIDL_0056);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *TkFile )( 
            IObjectContrib __RPC_FAR * This,
            /* [out] */ OBJTK __RPC_FAR *__MIDL_0057);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *VirtualAddress )( 
            IObjectContrib __RPC_FAR * This,
            /* [out] */ DWORD __RPC_FAR *__MIDL_0058);
        
        END_INTERFACE
    } IObjectContribVtbl;

    interface IObjectContrib
    {
        CONST_VTBL struct IObjectContribVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IObjectContrib_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IObjectContrib_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IObjectContrib_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IObjectContrib_CbContents(This,__MIDL_0042)	\
    (This)->lpVtbl -> CbContents(This,__MIDL_0042)

#define IObjectContrib_Characteristics(This,__MIDL_0043)	\
    (This)->lpVtbl -> Characteristics(This,__MIDL_0043)

#define IObjectContrib_CLineNo(This,__MIDL_0044)	\
    (This)->lpVtbl -> CLineNo(This,__MIDL_0044)

#define IObjectContrib_CRelocs(This,__MIDL_0045)	\
    (This)->lpVtbl -> CRelocs(This,__MIDL_0045)

#define IObjectContrib_ISection(This,__MIDL_0046)	\
    (This)->lpVtbl -> ISection(This,__MIDL_0046)

#define IObjectContrib_PbContents(This,__MIDL_0047,__MIDL_0048,__MIDL_0049)	\
    (This)->lpVtbl -> PbContents(This,__MIDL_0047,__MIDL_0048,__MIDL_0049)

#define IObjectContrib_PbContentsRO(This,__MIDL_0050)	\
    (This)->lpVtbl -> PbContentsRO(This,__MIDL_0050)

#define IObjectContrib_PbContentsRW(This,__MIDL_0051)	\
    (This)->lpVtbl -> PbContentsRW(This,__MIDL_0051)

#define IObjectContrib_RgLineNo(This,__MIDL_0052)	\
    (This)->lpVtbl -> RgLineNo(This,__MIDL_0052)

#define IObjectContrib_RgRelocs(This,__MIDL_0053)	\
    (This)->lpVtbl -> RgRelocs(This,__MIDL_0053)

#define IObjectContrib_RgRelocsEx(This,__MIDL_0054,__MIDL_0055)	\
    (This)->lpVtbl -> RgRelocsEx(This,__MIDL_0054,__MIDL_0055)

#define IObjectContrib_SzSectionName(This,__MIDL_0056)	\
    (This)->lpVtbl -> SzSectionName(This,__MIDL_0056)

#define IObjectContrib_TkFile(This,__MIDL_0057)	\
    (This)->lpVtbl -> TkFile(This,__MIDL_0057)

#define IObjectContrib_VirtualAddress(This,__MIDL_0058)	\
    (This)->lpVtbl -> VirtualAddress(This,__MIDL_0058)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IObjectContrib_CbContents_Proxy( 
    IObjectContrib __RPC_FAR * This,
    /* [out] */ DWORD __RPC_FAR *__MIDL_0042);


void __RPC_STUB IObjectContrib_CbContents_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IObjectContrib_Characteristics_Proxy( 
    IObjectContrib __RPC_FAR * This,
    /* [out] */ DWORD __RPC_FAR *__MIDL_0043);


void __RPC_STUB IObjectContrib_Characteristics_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IObjectContrib_CLineNo_Proxy( 
    IObjectContrib __RPC_FAR * This,
    /* [out] */ DWORD __RPC_FAR *__MIDL_0044);


void __RPC_STUB IObjectContrib_CLineNo_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IObjectContrib_CRelocs_Proxy( 
    IObjectContrib __RPC_FAR * This,
    /* [out] */ DWORD __RPC_FAR *__MIDL_0045);


void __RPC_STUB IObjectContrib_CRelocs_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IObjectContrib_ISection_Proxy( 
    IObjectContrib __RPC_FAR * This,
    /* [out] */ DWORD __RPC_FAR *__MIDL_0046);


void __RPC_STUB IObjectContrib_ISection_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IObjectContrib_PbContents_Proxy( 
    IObjectContrib __RPC_FAR * This,
    /* [out] */ BYTE __RPC_FAR *__MIDL_0047,
    /* [in] */ DWORD __MIDL_0048,
    /* [out] */ DWORD __RPC_FAR *__MIDL_0049);


void __RPC_STUB IObjectContrib_PbContents_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IObjectContrib_PbContentsRO_Proxy( 
    IObjectContrib __RPC_FAR * This,
    /* [out] */ PLinkDataRO __RPC_FAR *__MIDL_0050);


void __RPC_STUB IObjectContrib_PbContentsRO_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IObjectContrib_PbContentsRW_Proxy( 
    IObjectContrib __RPC_FAR * This,
    /* [out] */ PLinkDataRW __RPC_FAR *__MIDL_0051);


void __RPC_STUB IObjectContrib_PbContentsRW_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IObjectContrib_RgLineNo_Proxy( 
    IObjectContrib __RPC_FAR * This,
    /* [out] */ PLinkDataRO __RPC_FAR *__MIDL_0052);


void __RPC_STUB IObjectContrib_RgLineNo_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IObjectContrib_RgRelocs_Proxy( 
    IObjectContrib __RPC_FAR * This,
    /* [out] */ PLinkDataRO __RPC_FAR *__MIDL_0053);


void __RPC_STUB IObjectContrib_RgRelocs_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IObjectContrib_RgRelocsEx_Proxy( 
    IObjectContrib __RPC_FAR * This,
    /* [out] */ BYTE __RPC_FAR *__MIDL_0054,
    /* [in] */ DWORD __MIDL_0055);


void __RPC_STUB IObjectContrib_RgRelocsEx_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IObjectContrib_SzSectionName_Proxy( 
    IObjectContrib __RPC_FAR * This,
    /* [out] */ PLinkDataRO __RPC_FAR *__MIDL_0056);


void __RPC_STUB IObjectContrib_SzSectionName_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IObjectContrib_TkFile_Proxy( 
    IObjectContrib __RPC_FAR * This,
    /* [out] */ OBJTK __RPC_FAR *__MIDL_0057);


void __RPC_STUB IObjectContrib_TkFile_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IObjectContrib_VirtualAddress_Proxy( 
    IObjectContrib __RPC_FAR * This,
    /* [out] */ DWORD __RPC_FAR *__MIDL_0058);


void __RPC_STUB IObjectContrib_VirtualAddress_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IObjectContrib_INTERFACE_DEFINED__ */


#ifndef __IEnumContrib_INTERFACE_DEFINED__
#define __IEnumContrib_INTERFACE_DEFINED__

/****************************************
 * Generated header for interface: IEnumContrib
 * at Wed Jun 10 17:42:56 1998
 * using MIDL 3.03.0110
 ****************************************/
/* [unique][uuid][object] */ 


typedef /* [unique] */ IEnumContrib __RPC_FAR *PEnumContrib;


EXTERN_C const IID IID_IEnumContrib;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("6EC1C970-911F-11d1-A712-0060083E8C78")
    IEnumContrib : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE Next( 
            /* [in] */ ULONG __MIDL_0059,
            /* [out] */ IObjectContrib __RPC_FAR *__RPC_FAR *__MIDL_0060,
            /* [out] */ ULONG __RPC_FAR *__MIDL_0061) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Skip( 
            /* [in] */ ULONG __MIDL_0062) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Reset( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Clone( 
            /* [out] */ PEnumContrib __RPC_FAR *__MIDL_0063) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IEnumContribVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IEnumContrib __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IEnumContrib __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IEnumContrib __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Next )( 
            IEnumContrib __RPC_FAR * This,
            /* [in] */ ULONG __MIDL_0059,
            /* [out] */ IObjectContrib __RPC_FAR *__RPC_FAR *__MIDL_0060,
            /* [out] */ ULONG __RPC_FAR *__MIDL_0061);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Skip )( 
            IEnumContrib __RPC_FAR * This,
            /* [in] */ ULONG __MIDL_0062);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Reset )( 
            IEnumContrib __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Clone )( 
            IEnumContrib __RPC_FAR * This,
            /* [out] */ PEnumContrib __RPC_FAR *__MIDL_0063);
        
        END_INTERFACE
    } IEnumContribVtbl;

    interface IEnumContrib
    {
        CONST_VTBL struct IEnumContribVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IEnumContrib_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IEnumContrib_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IEnumContrib_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IEnumContrib_Next(This,__MIDL_0059,__MIDL_0060,__MIDL_0061)	\
    (This)->lpVtbl -> Next(This,__MIDL_0059,__MIDL_0060,__MIDL_0061)

#define IEnumContrib_Skip(This,__MIDL_0062)	\
    (This)->lpVtbl -> Skip(This,__MIDL_0062)

#define IEnumContrib_Reset(This)	\
    (This)->lpVtbl -> Reset(This)

#define IEnumContrib_Clone(This,__MIDL_0063)	\
    (This)->lpVtbl -> Clone(This,__MIDL_0063)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IEnumContrib_Next_Proxy( 
    IEnumContrib __RPC_FAR * This,
    /* [in] */ ULONG __MIDL_0059,
    /* [out] */ IObjectContrib __RPC_FAR *__RPC_FAR *__MIDL_0060,
    /* [out] */ ULONG __RPC_FAR *__MIDL_0061);


void __RPC_STUB IEnumContrib_Next_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IEnumContrib_Skip_Proxy( 
    IEnumContrib __RPC_FAR * This,
    /* [in] */ ULONG __MIDL_0062);


void __RPC_STUB IEnumContrib_Skip_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IEnumContrib_Reset_Proxy( 
    IEnumContrib __RPC_FAR * This);


void __RPC_STUB IEnumContrib_Reset_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IEnumContrib_Clone_Proxy( 
    IEnumContrib __RPC_FAR * This,
    /* [out] */ PEnumContrib __RPC_FAR *__MIDL_0063);


void __RPC_STUB IEnumContrib_Clone_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IEnumContrib_INTERFACE_DEFINED__ */


#ifndef __ICOFFAuxSym_INTERFACE_DEFINED__
#define __ICOFFAuxSym_INTERFACE_DEFINED__

/****************************************
 * Generated header for interface: ICOFFAuxSym
 * at Wed Jun 10 17:42:56 1998
 * using MIDL 3.03.0110
 ****************************************/
/* [unique][uuid][object] */ 


typedef /* [unique] */ ICOFFAuxSym __RPC_FAR *PCOFFAuxSym;


EXTERN_C const IID IID_ICOFFAuxSym;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("01B3C321-B2FC-11d1-A71B-0060083E8C78")
    ICOFFAuxSym : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE TkFile( 
            /* [out] */ OBJTK __RPC_FAR *__MIDL_0064) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE ImgAuxSym( 
            /* [in] */ BYTE __RPC_FAR *__MIDL_0065) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Length( 
            /* [out] */ DWORD __RPC_FAR *__MIDL_0066) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE CRelocs( 
            /* [out] */ DWORD __RPC_FAR *__MIDL_0067) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE CLineNo( 
            /* [out] */ DWORD __RPC_FAR *__MIDL_0068) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE CheckSum( 
            /* [out] */ DWORD __RPC_FAR *__MIDL_0069) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE ISection( 
            /* [out] */ int __RPC_FAR *__MIDL_0070) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SelType( 
            /* [out] */ BYTE __RPC_FAR *__MIDL_0071) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE TagIndex( 
            /* [out] */ DWORD __RPC_FAR *__MIDL_0072) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE MiscLineNo( 
            /* [out] */ DWORD __RPC_FAR *__MIDL_0073) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE MiscSize( 
            /* [out] */ DWORD __RPC_FAR *__MIDL_0074) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE MiscTotalSize( 
            /* [out] */ DWORD __RPC_FAR *__MIDL_0075) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE FcnPLineNo( 
            /* [out] */ DWORD __RPC_FAR *__MIDL_0076) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE FcnPNextFcn( 
            /* [out] */ DWORD __RPC_FAR *__MIDL_0077) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE AryDim( 
            /* [in] */ DWORD __RPC_FAR *__MIDL_0078,
            /* [in] */ DWORD __MIDL_0079) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE NewSymCrc( 
            /* [out] */ DWORD __RPC_FAR *__MIDL_0080) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct ICOFFAuxSymVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            ICOFFAuxSym __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            ICOFFAuxSym __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            ICOFFAuxSym __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *TkFile )( 
            ICOFFAuxSym __RPC_FAR * This,
            /* [out] */ OBJTK __RPC_FAR *__MIDL_0064);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *ImgAuxSym )( 
            ICOFFAuxSym __RPC_FAR * This,
            /* [in] */ BYTE __RPC_FAR *__MIDL_0065);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Length )( 
            ICOFFAuxSym __RPC_FAR * This,
            /* [out] */ DWORD __RPC_FAR *__MIDL_0066);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *CRelocs )( 
            ICOFFAuxSym __RPC_FAR * This,
            /* [out] */ DWORD __RPC_FAR *__MIDL_0067);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *CLineNo )( 
            ICOFFAuxSym __RPC_FAR * This,
            /* [out] */ DWORD __RPC_FAR *__MIDL_0068);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *CheckSum )( 
            ICOFFAuxSym __RPC_FAR * This,
            /* [out] */ DWORD __RPC_FAR *__MIDL_0069);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *ISection )( 
            ICOFFAuxSym __RPC_FAR * This,
            /* [out] */ int __RPC_FAR *__MIDL_0070);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *SelType )( 
            ICOFFAuxSym __RPC_FAR * This,
            /* [out] */ BYTE __RPC_FAR *__MIDL_0071);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *TagIndex )( 
            ICOFFAuxSym __RPC_FAR * This,
            /* [out] */ DWORD __RPC_FAR *__MIDL_0072);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *MiscLineNo )( 
            ICOFFAuxSym __RPC_FAR * This,
            /* [out] */ DWORD __RPC_FAR *__MIDL_0073);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *MiscSize )( 
            ICOFFAuxSym __RPC_FAR * This,
            /* [out] */ DWORD __RPC_FAR *__MIDL_0074);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *MiscTotalSize )( 
            ICOFFAuxSym __RPC_FAR * This,
            /* [out] */ DWORD __RPC_FAR *__MIDL_0075);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *FcnPLineNo )( 
            ICOFFAuxSym __RPC_FAR * This,
            /* [out] */ DWORD __RPC_FAR *__MIDL_0076);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *FcnPNextFcn )( 
            ICOFFAuxSym __RPC_FAR * This,
            /* [out] */ DWORD __RPC_FAR *__MIDL_0077);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *AryDim )( 
            ICOFFAuxSym __RPC_FAR * This,
            /* [in] */ DWORD __RPC_FAR *__MIDL_0078,
            /* [in] */ DWORD __MIDL_0079);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *NewSymCrc )( 
            ICOFFAuxSym __RPC_FAR * This,
            /* [out] */ DWORD __RPC_FAR *__MIDL_0080);
        
        END_INTERFACE
    } ICOFFAuxSymVtbl;

    interface ICOFFAuxSym
    {
        CONST_VTBL struct ICOFFAuxSymVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define ICOFFAuxSym_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define ICOFFAuxSym_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define ICOFFAuxSym_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define ICOFFAuxSym_TkFile(This,__MIDL_0064)	\
    (This)->lpVtbl -> TkFile(This,__MIDL_0064)

#define ICOFFAuxSym_ImgAuxSym(This,__MIDL_0065)	\
    (This)->lpVtbl -> ImgAuxSym(This,__MIDL_0065)

#define ICOFFAuxSym_Length(This,__MIDL_0066)	\
    (This)->lpVtbl -> Length(This,__MIDL_0066)

#define ICOFFAuxSym_CRelocs(This,__MIDL_0067)	\
    (This)->lpVtbl -> CRelocs(This,__MIDL_0067)

#define ICOFFAuxSym_CLineNo(This,__MIDL_0068)	\
    (This)->lpVtbl -> CLineNo(This,__MIDL_0068)

#define ICOFFAuxSym_CheckSum(This,__MIDL_0069)	\
    (This)->lpVtbl -> CheckSum(This,__MIDL_0069)

#define ICOFFAuxSym_ISection(This,__MIDL_0070)	\
    (This)->lpVtbl -> ISection(This,__MIDL_0070)

#define ICOFFAuxSym_SelType(This,__MIDL_0071)	\
    (This)->lpVtbl -> SelType(This,__MIDL_0071)

#define ICOFFAuxSym_TagIndex(This,__MIDL_0072)	\
    (This)->lpVtbl -> TagIndex(This,__MIDL_0072)

#define ICOFFAuxSym_MiscLineNo(This,__MIDL_0073)	\
    (This)->lpVtbl -> MiscLineNo(This,__MIDL_0073)

#define ICOFFAuxSym_MiscSize(This,__MIDL_0074)	\
    (This)->lpVtbl -> MiscSize(This,__MIDL_0074)

#define ICOFFAuxSym_MiscTotalSize(This,__MIDL_0075)	\
    (This)->lpVtbl -> MiscTotalSize(This,__MIDL_0075)

#define ICOFFAuxSym_FcnPLineNo(This,__MIDL_0076)	\
    (This)->lpVtbl -> FcnPLineNo(This,__MIDL_0076)

#define ICOFFAuxSym_FcnPNextFcn(This,__MIDL_0077)	\
    (This)->lpVtbl -> FcnPNextFcn(This,__MIDL_0077)

#define ICOFFAuxSym_AryDim(This,__MIDL_0078,__MIDL_0079)	\
    (This)->lpVtbl -> AryDim(This,__MIDL_0078,__MIDL_0079)

#define ICOFFAuxSym_NewSymCrc(This,__MIDL_0080)	\
    (This)->lpVtbl -> NewSymCrc(This,__MIDL_0080)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE ICOFFAuxSym_TkFile_Proxy( 
    ICOFFAuxSym __RPC_FAR * This,
    /* [out] */ OBJTK __RPC_FAR *__MIDL_0064);


void __RPC_STUB ICOFFAuxSym_TkFile_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ICOFFAuxSym_ImgAuxSym_Proxy( 
    ICOFFAuxSym __RPC_FAR * This,
    /* [in] */ BYTE __RPC_FAR *__MIDL_0065);


void __RPC_STUB ICOFFAuxSym_ImgAuxSym_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ICOFFAuxSym_Length_Proxy( 
    ICOFFAuxSym __RPC_FAR * This,
    /* [out] */ DWORD __RPC_FAR *__MIDL_0066);


void __RPC_STUB ICOFFAuxSym_Length_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ICOFFAuxSym_CRelocs_Proxy( 
    ICOFFAuxSym __RPC_FAR * This,
    /* [out] */ DWORD __RPC_FAR *__MIDL_0067);


void __RPC_STUB ICOFFAuxSym_CRelocs_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ICOFFAuxSym_CLineNo_Proxy( 
    ICOFFAuxSym __RPC_FAR * This,
    /* [out] */ DWORD __RPC_FAR *__MIDL_0068);


void __RPC_STUB ICOFFAuxSym_CLineNo_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ICOFFAuxSym_CheckSum_Proxy( 
    ICOFFAuxSym __RPC_FAR * This,
    /* [out] */ DWORD __RPC_FAR *__MIDL_0069);


void __RPC_STUB ICOFFAuxSym_CheckSum_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ICOFFAuxSym_ISection_Proxy( 
    ICOFFAuxSym __RPC_FAR * This,
    /* [out] */ int __RPC_FAR *__MIDL_0070);


void __RPC_STUB ICOFFAuxSym_ISection_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ICOFFAuxSym_SelType_Proxy( 
    ICOFFAuxSym __RPC_FAR * This,
    /* [out] */ BYTE __RPC_FAR *__MIDL_0071);


void __RPC_STUB ICOFFAuxSym_SelType_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ICOFFAuxSym_TagIndex_Proxy( 
    ICOFFAuxSym __RPC_FAR * This,
    /* [out] */ DWORD __RPC_FAR *__MIDL_0072);


void __RPC_STUB ICOFFAuxSym_TagIndex_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ICOFFAuxSym_MiscLineNo_Proxy( 
    ICOFFAuxSym __RPC_FAR * This,
    /* [out] */ DWORD __RPC_FAR *__MIDL_0073);


void __RPC_STUB ICOFFAuxSym_MiscLineNo_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ICOFFAuxSym_MiscSize_Proxy( 
    ICOFFAuxSym __RPC_FAR * This,
    /* [out] */ DWORD __RPC_FAR *__MIDL_0074);


void __RPC_STUB ICOFFAuxSym_MiscSize_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ICOFFAuxSym_MiscTotalSize_Proxy( 
    ICOFFAuxSym __RPC_FAR * This,
    /* [out] */ DWORD __RPC_FAR *__MIDL_0075);


void __RPC_STUB ICOFFAuxSym_MiscTotalSize_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ICOFFAuxSym_FcnPLineNo_Proxy( 
    ICOFFAuxSym __RPC_FAR * This,
    /* [out] */ DWORD __RPC_FAR *__MIDL_0076);


void __RPC_STUB ICOFFAuxSym_FcnPLineNo_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ICOFFAuxSym_FcnPNextFcn_Proxy( 
    ICOFFAuxSym __RPC_FAR * This,
    /* [out] */ DWORD __RPC_FAR *__MIDL_0077);


void __RPC_STUB ICOFFAuxSym_FcnPNextFcn_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ICOFFAuxSym_AryDim_Proxy( 
    ICOFFAuxSym __RPC_FAR * This,
    /* [in] */ DWORD __RPC_FAR *__MIDL_0078,
    /* [in] */ DWORD __MIDL_0079);


void __RPC_STUB ICOFFAuxSym_AryDim_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ICOFFAuxSym_NewSymCrc_Proxy( 
    ICOFFAuxSym __RPC_FAR * This,
    /* [out] */ DWORD __RPC_FAR *__MIDL_0080);


void __RPC_STUB ICOFFAuxSym_NewSymCrc_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __ICOFFAuxSym_INTERFACE_DEFINED__ */


#ifndef __IEnumAuxSym_INTERFACE_DEFINED__
#define __IEnumAuxSym_INTERFACE_DEFINED__

/****************************************
 * Generated header for interface: IEnumAuxSym
 * at Wed Jun 10 17:42:56 1998
 * using MIDL 3.03.0110
 ****************************************/
/* [unique][uuid][object] */ 


typedef /* [unique] */ IEnumAuxSym __RPC_FAR *PEnumAuxSym;


EXTERN_C const IID IID_IEnumAuxSym;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("083DE150-B461-11d1-A71B-0060083E8C78")
    IEnumAuxSym : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE Next( 
            /* [in] */ ULONG __MIDL_0081,
            /* [out] */ ICOFFAuxSym __RPC_FAR *__RPC_FAR *__MIDL_0082,
            /* [out] */ ULONG __RPC_FAR *__MIDL_0083) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Skip( 
            /* [in] */ ULONG __MIDL_0084) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Reset( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Clone( 
            /* [out] */ IEnumAuxSym __RPC_FAR *__RPC_FAR *__MIDL_0085) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IEnumAuxSymVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IEnumAuxSym __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IEnumAuxSym __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IEnumAuxSym __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Next )( 
            IEnumAuxSym __RPC_FAR * This,
            /* [in] */ ULONG __MIDL_0081,
            /* [out] */ ICOFFAuxSym __RPC_FAR *__RPC_FAR *__MIDL_0082,
            /* [out] */ ULONG __RPC_FAR *__MIDL_0083);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Skip )( 
            IEnumAuxSym __RPC_FAR * This,
            /* [in] */ ULONG __MIDL_0084);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Reset )( 
            IEnumAuxSym __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Clone )( 
            IEnumAuxSym __RPC_FAR * This,
            /* [out] */ IEnumAuxSym __RPC_FAR *__RPC_FAR *__MIDL_0085);
        
        END_INTERFACE
    } IEnumAuxSymVtbl;

    interface IEnumAuxSym
    {
        CONST_VTBL struct IEnumAuxSymVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IEnumAuxSym_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IEnumAuxSym_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IEnumAuxSym_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IEnumAuxSym_Next(This,__MIDL_0081,__MIDL_0082,__MIDL_0083)	\
    (This)->lpVtbl -> Next(This,__MIDL_0081,__MIDL_0082,__MIDL_0083)

#define IEnumAuxSym_Skip(This,__MIDL_0084)	\
    (This)->lpVtbl -> Skip(This,__MIDL_0084)

#define IEnumAuxSym_Reset(This)	\
    (This)->lpVtbl -> Reset(This)

#define IEnumAuxSym_Clone(This,__MIDL_0085)	\
    (This)->lpVtbl -> Clone(This,__MIDL_0085)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IEnumAuxSym_Next_Proxy( 
    IEnumAuxSym __RPC_FAR * This,
    /* [in] */ ULONG __MIDL_0081,
    /* [out] */ ICOFFAuxSym __RPC_FAR *__RPC_FAR *__MIDL_0082,
    /* [out] */ ULONG __RPC_FAR *__MIDL_0083);


void __RPC_STUB IEnumAuxSym_Next_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IEnumAuxSym_Skip_Proxy( 
    IEnumAuxSym __RPC_FAR * This,
    /* [in] */ ULONG __MIDL_0084);


void __RPC_STUB IEnumAuxSym_Skip_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IEnumAuxSym_Reset_Proxy( 
    IEnumAuxSym __RPC_FAR * This);


void __RPC_STUB IEnumAuxSym_Reset_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IEnumAuxSym_Clone_Proxy( 
    IEnumAuxSym __RPC_FAR * This,
    /* [out] */ IEnumAuxSym __RPC_FAR *__RPC_FAR *__MIDL_0085);


void __RPC_STUB IEnumAuxSym_Clone_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IEnumAuxSym_INTERFACE_DEFINED__ */


#ifndef __ICOFFSymRO_INTERFACE_DEFINED__
#define __ICOFFSymRO_INTERFACE_DEFINED__

/****************************************
 * Generated header for interface: ICOFFSymRO
 * at Wed Jun 10 17:42:56 1998
 * using MIDL 3.03.0110
 ****************************************/
/* [unique][uuid][object] */ 


typedef /* [unique] */ ICOFFSymRO __RPC_FAR *PCOFFSymRO;


EXTERN_C const IID IID_ICOFFSymRO;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("CBDFD0D1-B21F-11d1-A71B-0060083E8C78")
    ICOFFSymRO : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE CAuxSym( 
            /* [out] */ BYTE __RPC_FAR *__MIDL_0086) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE ISection( 
            /* [out] */ int __RPC_FAR *__MIDL_0087) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE ISym( 
            /* [out] */ ISYM __RPC_FAR *__MIDL_0088) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE StorageClass( 
            /* [out] */ BYTE __RPC_FAR *__MIDL_0089) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SzFile( 
            /* [out] */ PLinkDataRO __RPC_FAR *__MIDL_0090) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SzName( 
            /* [out] */ PLinkDataRO __RPC_FAR *__MIDL_0091) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE TkFile( 
            /* [out] */ OBJTK __RPC_FAR *__MIDL_0092) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Type( 
            /* [out] */ WORD __RPC_FAR *__MIDL_0093) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Value( 
            /* [out] */ DWORD __RPC_FAR *__MIDL_0094) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE EnumAuxSym( 
            /* [out] */ IEnumAuxSym __RPC_FAR *__RPC_FAR *__MIDL_0095) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct ICOFFSymROVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            ICOFFSymRO __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            ICOFFSymRO __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            ICOFFSymRO __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *CAuxSym )( 
            ICOFFSymRO __RPC_FAR * This,
            /* [out] */ BYTE __RPC_FAR *__MIDL_0086);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *ISection )( 
            ICOFFSymRO __RPC_FAR * This,
            /* [out] */ int __RPC_FAR *__MIDL_0087);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *ISym )( 
            ICOFFSymRO __RPC_FAR * This,
            /* [out] */ ISYM __RPC_FAR *__MIDL_0088);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *StorageClass )( 
            ICOFFSymRO __RPC_FAR * This,
            /* [out] */ BYTE __RPC_FAR *__MIDL_0089);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *SzFile )( 
            ICOFFSymRO __RPC_FAR * This,
            /* [out] */ PLinkDataRO __RPC_FAR *__MIDL_0090);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *SzName )( 
            ICOFFSymRO __RPC_FAR * This,
            /* [out] */ PLinkDataRO __RPC_FAR *__MIDL_0091);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *TkFile )( 
            ICOFFSymRO __RPC_FAR * This,
            /* [out] */ OBJTK __RPC_FAR *__MIDL_0092);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Type )( 
            ICOFFSymRO __RPC_FAR * This,
            /* [out] */ WORD __RPC_FAR *__MIDL_0093);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Value )( 
            ICOFFSymRO __RPC_FAR * This,
            /* [out] */ DWORD __RPC_FAR *__MIDL_0094);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *EnumAuxSym )( 
            ICOFFSymRO __RPC_FAR * This,
            /* [out] */ IEnumAuxSym __RPC_FAR *__RPC_FAR *__MIDL_0095);
        
        END_INTERFACE
    } ICOFFSymROVtbl;

    interface ICOFFSymRO
    {
        CONST_VTBL struct ICOFFSymROVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define ICOFFSymRO_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define ICOFFSymRO_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define ICOFFSymRO_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define ICOFFSymRO_CAuxSym(This,__MIDL_0086)	\
    (This)->lpVtbl -> CAuxSym(This,__MIDL_0086)

#define ICOFFSymRO_ISection(This,__MIDL_0087)	\
    (This)->lpVtbl -> ISection(This,__MIDL_0087)

#define ICOFFSymRO_ISym(This,__MIDL_0088)	\
    (This)->lpVtbl -> ISym(This,__MIDL_0088)

#define ICOFFSymRO_StorageClass(This,__MIDL_0089)	\
    (This)->lpVtbl -> StorageClass(This,__MIDL_0089)

#define ICOFFSymRO_SzFile(This,__MIDL_0090)	\
    (This)->lpVtbl -> SzFile(This,__MIDL_0090)

#define ICOFFSymRO_SzName(This,__MIDL_0091)	\
    (This)->lpVtbl -> SzName(This,__MIDL_0091)

#define ICOFFSymRO_TkFile(This,__MIDL_0092)	\
    (This)->lpVtbl -> TkFile(This,__MIDL_0092)

#define ICOFFSymRO_Type(This,__MIDL_0093)	\
    (This)->lpVtbl -> Type(This,__MIDL_0093)

#define ICOFFSymRO_Value(This,__MIDL_0094)	\
    (This)->lpVtbl -> Value(This,__MIDL_0094)

#define ICOFFSymRO_EnumAuxSym(This,__MIDL_0095)	\
    (This)->lpVtbl -> EnumAuxSym(This,__MIDL_0095)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE ICOFFSymRO_CAuxSym_Proxy( 
    ICOFFSymRO __RPC_FAR * This,
    /* [out] */ BYTE __RPC_FAR *__MIDL_0086);


void __RPC_STUB ICOFFSymRO_CAuxSym_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ICOFFSymRO_ISection_Proxy( 
    ICOFFSymRO __RPC_FAR * This,
    /* [out] */ int __RPC_FAR *__MIDL_0087);


void __RPC_STUB ICOFFSymRO_ISection_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ICOFFSymRO_ISym_Proxy( 
    ICOFFSymRO __RPC_FAR * This,
    /* [out] */ ISYM __RPC_FAR *__MIDL_0088);


void __RPC_STUB ICOFFSymRO_ISym_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ICOFFSymRO_StorageClass_Proxy( 
    ICOFFSymRO __RPC_FAR * This,
    /* [out] */ BYTE __RPC_FAR *__MIDL_0089);


void __RPC_STUB ICOFFSymRO_StorageClass_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ICOFFSymRO_SzFile_Proxy( 
    ICOFFSymRO __RPC_FAR * This,
    /* [out] */ PLinkDataRO __RPC_FAR *__MIDL_0090);


void __RPC_STUB ICOFFSymRO_SzFile_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ICOFFSymRO_SzName_Proxy( 
    ICOFFSymRO __RPC_FAR * This,
    /* [out] */ PLinkDataRO __RPC_FAR *__MIDL_0091);


void __RPC_STUB ICOFFSymRO_SzName_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ICOFFSymRO_TkFile_Proxy( 
    ICOFFSymRO __RPC_FAR * This,
    /* [out] */ OBJTK __RPC_FAR *__MIDL_0092);


void __RPC_STUB ICOFFSymRO_TkFile_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ICOFFSymRO_Type_Proxy( 
    ICOFFSymRO __RPC_FAR * This,
    /* [out] */ WORD __RPC_FAR *__MIDL_0093);


void __RPC_STUB ICOFFSymRO_Type_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ICOFFSymRO_Value_Proxy( 
    ICOFFSymRO __RPC_FAR * This,
    /* [out] */ DWORD __RPC_FAR *__MIDL_0094);


void __RPC_STUB ICOFFSymRO_Value_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ICOFFSymRO_EnumAuxSym_Proxy( 
    ICOFFSymRO __RPC_FAR * This,
    /* [out] */ IEnumAuxSym __RPC_FAR *__RPC_FAR *__MIDL_0095);


void __RPC_STUB ICOFFSymRO_EnumAuxSym_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __ICOFFSymRO_INTERFACE_DEFINED__ */


#ifndef __ICOFFSymRW_INTERFACE_DEFINED__
#define __ICOFFSymRW_INTERFACE_DEFINED__

/****************************************
 * Generated header for interface: ICOFFSymRW
 * at Wed Jun 10 17:42:56 1998
 * using MIDL 3.03.0110
 ****************************************/
/* [unique][uuid][object] */ 


typedef /* [unique] */ ICOFFSymRW __RPC_FAR *PCOFFSymRW;


EXTERN_C const IID IID_ICOFFSymRW;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("01B3C320-B2FC-11d1-A71B-0060083E8C78")
    ICOFFSymRW : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE CAuxSym( 
            /* [out] */ BYTE __RPC_FAR *__MIDL_0096) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE ISection( 
            /* [out] */ int __RPC_FAR *__MIDL_0097) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE ISym( 
            /* [out] */ DWORD __RPC_FAR *__MIDL_0098) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE StorageClass( 
            /* [out] */ BYTE __RPC_FAR *__MIDL_0099) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SzFile( 
            /* [out] */ PLinkDataRO __RPC_FAR *__MIDL_0100) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SzName( 
            /* [out] */ PLinkDataRO __RPC_FAR *__MIDL_0101) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE TkFile( 
            /* [out] */ OBJTK __RPC_FAR *__MIDL_0102) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Type( 
            /* [out] */ WORD __RPC_FAR *__MIDL_0103) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Value( 
            /* [out] */ DWORD __RPC_FAR *__MIDL_0104) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE EnumAuxSym( 
            /* [out] */ IEnumAuxSym __RPC_FAR *__RPC_FAR *__MIDL_0105) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetISection( 
            /* [in] */ int __MIDL_0106) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetValue( 
            /* [in] */ DWORD __MIDL_0107) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct ICOFFSymRWVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            ICOFFSymRW __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            ICOFFSymRW __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            ICOFFSymRW __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *CAuxSym )( 
            ICOFFSymRW __RPC_FAR * This,
            /* [out] */ BYTE __RPC_FAR *__MIDL_0096);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *ISection )( 
            ICOFFSymRW __RPC_FAR * This,
            /* [out] */ int __RPC_FAR *__MIDL_0097);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *ISym )( 
            ICOFFSymRW __RPC_FAR * This,
            /* [out] */ DWORD __RPC_FAR *__MIDL_0098);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *StorageClass )( 
            ICOFFSymRW __RPC_FAR * This,
            /* [out] */ BYTE __RPC_FAR *__MIDL_0099);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *SzFile )( 
            ICOFFSymRW __RPC_FAR * This,
            /* [out] */ PLinkDataRO __RPC_FAR *__MIDL_0100);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *SzName )( 
            ICOFFSymRW __RPC_FAR * This,
            /* [out] */ PLinkDataRO __RPC_FAR *__MIDL_0101);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *TkFile )( 
            ICOFFSymRW __RPC_FAR * This,
            /* [out] */ OBJTK __RPC_FAR *__MIDL_0102);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Type )( 
            ICOFFSymRW __RPC_FAR * This,
            /* [out] */ WORD __RPC_FAR *__MIDL_0103);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Value )( 
            ICOFFSymRW __RPC_FAR * This,
            /* [out] */ DWORD __RPC_FAR *__MIDL_0104);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *EnumAuxSym )( 
            ICOFFSymRW __RPC_FAR * This,
            /* [out] */ IEnumAuxSym __RPC_FAR *__RPC_FAR *__MIDL_0105);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *SetISection )( 
            ICOFFSymRW __RPC_FAR * This,
            /* [in] */ int __MIDL_0106);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *SetValue )( 
            ICOFFSymRW __RPC_FAR * This,
            /* [in] */ DWORD __MIDL_0107);
        
        END_INTERFACE
    } ICOFFSymRWVtbl;

    interface ICOFFSymRW
    {
        CONST_VTBL struct ICOFFSymRWVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define ICOFFSymRW_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define ICOFFSymRW_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define ICOFFSymRW_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define ICOFFSymRW_CAuxSym(This,__MIDL_0096)	\
    (This)->lpVtbl -> CAuxSym(This,__MIDL_0096)

#define ICOFFSymRW_ISection(This,__MIDL_0097)	\
    (This)->lpVtbl -> ISection(This,__MIDL_0097)

#define ICOFFSymRW_ISym(This,__MIDL_0098)	\
    (This)->lpVtbl -> ISym(This,__MIDL_0098)

#define ICOFFSymRW_StorageClass(This,__MIDL_0099)	\
    (This)->lpVtbl -> StorageClass(This,__MIDL_0099)

#define ICOFFSymRW_SzFile(This,__MIDL_0100)	\
    (This)->lpVtbl -> SzFile(This,__MIDL_0100)

#define ICOFFSymRW_SzName(This,__MIDL_0101)	\
    (This)->lpVtbl -> SzName(This,__MIDL_0101)

#define ICOFFSymRW_TkFile(This,__MIDL_0102)	\
    (This)->lpVtbl -> TkFile(This,__MIDL_0102)

#define ICOFFSymRW_Type(This,__MIDL_0103)	\
    (This)->lpVtbl -> Type(This,__MIDL_0103)

#define ICOFFSymRW_Value(This,__MIDL_0104)	\
    (This)->lpVtbl -> Value(This,__MIDL_0104)

#define ICOFFSymRW_EnumAuxSym(This,__MIDL_0105)	\
    (This)->lpVtbl -> EnumAuxSym(This,__MIDL_0105)

#define ICOFFSymRW_SetISection(This,__MIDL_0106)	\
    (This)->lpVtbl -> SetISection(This,__MIDL_0106)

#define ICOFFSymRW_SetValue(This,__MIDL_0107)	\
    (This)->lpVtbl -> SetValue(This,__MIDL_0107)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE ICOFFSymRW_CAuxSym_Proxy( 
    ICOFFSymRW __RPC_FAR * This,
    /* [out] */ BYTE __RPC_FAR *__MIDL_0096);


void __RPC_STUB ICOFFSymRW_CAuxSym_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ICOFFSymRW_ISection_Proxy( 
    ICOFFSymRW __RPC_FAR * This,
    /* [out] */ int __RPC_FAR *__MIDL_0097);


void __RPC_STUB ICOFFSymRW_ISection_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ICOFFSymRW_ISym_Proxy( 
    ICOFFSymRW __RPC_FAR * This,
    /* [out] */ DWORD __RPC_FAR *__MIDL_0098);


void __RPC_STUB ICOFFSymRW_ISym_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ICOFFSymRW_StorageClass_Proxy( 
    ICOFFSymRW __RPC_FAR * This,
    /* [out] */ BYTE __RPC_FAR *__MIDL_0099);


void __RPC_STUB ICOFFSymRW_StorageClass_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ICOFFSymRW_SzFile_Proxy( 
    ICOFFSymRW __RPC_FAR * This,
    /* [out] */ PLinkDataRO __RPC_FAR *__MIDL_0100);


void __RPC_STUB ICOFFSymRW_SzFile_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ICOFFSymRW_SzName_Proxy( 
    ICOFFSymRW __RPC_FAR * This,
    /* [out] */ PLinkDataRO __RPC_FAR *__MIDL_0101);


void __RPC_STUB ICOFFSymRW_SzName_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ICOFFSymRW_TkFile_Proxy( 
    ICOFFSymRW __RPC_FAR * This,
    /* [out] */ OBJTK __RPC_FAR *__MIDL_0102);


void __RPC_STUB ICOFFSymRW_TkFile_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ICOFFSymRW_Type_Proxy( 
    ICOFFSymRW __RPC_FAR * This,
    /* [out] */ WORD __RPC_FAR *__MIDL_0103);


void __RPC_STUB ICOFFSymRW_Type_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ICOFFSymRW_Value_Proxy( 
    ICOFFSymRW __RPC_FAR * This,
    /* [out] */ DWORD __RPC_FAR *__MIDL_0104);


void __RPC_STUB ICOFFSymRW_Value_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ICOFFSymRW_EnumAuxSym_Proxy( 
    ICOFFSymRW __RPC_FAR * This,
    /* [out] */ IEnumAuxSym __RPC_FAR *__RPC_FAR *__MIDL_0105);


void __RPC_STUB ICOFFSymRW_EnumAuxSym_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ICOFFSymRW_SetISection_Proxy( 
    ICOFFSymRW __RPC_FAR * This,
    /* [in] */ int __MIDL_0106);


void __RPC_STUB ICOFFSymRW_SetISection_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ICOFFSymRW_SetValue_Proxy( 
    ICOFFSymRW __RPC_FAR * This,
    /* [in] */ DWORD __MIDL_0107);


void __RPC_STUB ICOFFSymRW_SetValue_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __ICOFFSymRW_INTERFACE_DEFINED__ */


#ifndef __IEnumCOFFSymRO_INTERFACE_DEFINED__
#define __IEnumCOFFSymRO_INTERFACE_DEFINED__

/****************************************
 * Generated header for interface: IEnumCOFFSymRO
 * at Wed Jun 10 17:42:56 1998
 * using MIDL 3.03.0110
 ****************************************/
/* [unique][uuid][object] */ 


typedef /* [unique] */ IEnumCOFFSymRO __RPC_FAR *PEnumCOFFSymRO;


EXTERN_C const IID IID_IEnumCOFFSymRO;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("33640AD0-B2C6-11d1-A71B-0060083E8C78")
    IEnumCOFFSymRO : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE Next( 
            /* [in] */ ULONG __MIDL_0108,
            /* [out] */ ICOFFSymRO __RPC_FAR *__RPC_FAR *__MIDL_0109,
            /* [out] */ ULONG __RPC_FAR *__MIDL_0110) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Skip( 
            /* [in] */ ULONG __MIDL_0111) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Reset( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Clone( 
            /* [out] */ IEnumCOFFSymRO __RPC_FAR *__RPC_FAR *__MIDL_0112) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IEnumCOFFSymROVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IEnumCOFFSymRO __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IEnumCOFFSymRO __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IEnumCOFFSymRO __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Next )( 
            IEnumCOFFSymRO __RPC_FAR * This,
            /* [in] */ ULONG __MIDL_0108,
            /* [out] */ ICOFFSymRO __RPC_FAR *__RPC_FAR *__MIDL_0109,
            /* [out] */ ULONG __RPC_FAR *__MIDL_0110);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Skip )( 
            IEnumCOFFSymRO __RPC_FAR * This,
            /* [in] */ ULONG __MIDL_0111);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Reset )( 
            IEnumCOFFSymRO __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Clone )( 
            IEnumCOFFSymRO __RPC_FAR * This,
            /* [out] */ IEnumCOFFSymRO __RPC_FAR *__RPC_FAR *__MIDL_0112);
        
        END_INTERFACE
    } IEnumCOFFSymROVtbl;

    interface IEnumCOFFSymRO
    {
        CONST_VTBL struct IEnumCOFFSymROVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IEnumCOFFSymRO_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IEnumCOFFSymRO_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IEnumCOFFSymRO_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IEnumCOFFSymRO_Next(This,__MIDL_0108,__MIDL_0109,__MIDL_0110)	\
    (This)->lpVtbl -> Next(This,__MIDL_0108,__MIDL_0109,__MIDL_0110)

#define IEnumCOFFSymRO_Skip(This,__MIDL_0111)	\
    (This)->lpVtbl -> Skip(This,__MIDL_0111)

#define IEnumCOFFSymRO_Reset(This)	\
    (This)->lpVtbl -> Reset(This)

#define IEnumCOFFSymRO_Clone(This,__MIDL_0112)	\
    (This)->lpVtbl -> Clone(This,__MIDL_0112)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IEnumCOFFSymRO_Next_Proxy( 
    IEnumCOFFSymRO __RPC_FAR * This,
    /* [in] */ ULONG __MIDL_0108,
    /* [out] */ ICOFFSymRO __RPC_FAR *__RPC_FAR *__MIDL_0109,
    /* [out] */ ULONG __RPC_FAR *__MIDL_0110);


void __RPC_STUB IEnumCOFFSymRO_Next_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IEnumCOFFSymRO_Skip_Proxy( 
    IEnumCOFFSymRO __RPC_FAR * This,
    /* [in] */ ULONG __MIDL_0111);


void __RPC_STUB IEnumCOFFSymRO_Skip_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IEnumCOFFSymRO_Reset_Proxy( 
    IEnumCOFFSymRO __RPC_FAR * This);


void __RPC_STUB IEnumCOFFSymRO_Reset_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IEnumCOFFSymRO_Clone_Proxy( 
    IEnumCOFFSymRO __RPC_FAR * This,
    /* [out] */ IEnumCOFFSymRO __RPC_FAR *__RPC_FAR *__MIDL_0112);


void __RPC_STUB IEnumCOFFSymRO_Clone_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IEnumCOFFSymRO_INTERFACE_DEFINED__ */


#ifndef __IEnumCOFFSymRW_INTERFACE_DEFINED__
#define __IEnumCOFFSymRW_INTERFACE_DEFINED__

/****************************************
 * Generated header for interface: IEnumCOFFSymRW
 * at Wed Jun 10 17:42:56 1998
 * using MIDL 3.03.0110
 ****************************************/
/* [unique][uuid][object] */ 


typedef /* [unique] */ IEnumCOFFSymRW __RPC_FAR *PEnumCOFFSymRW;


EXTERN_C const IID IID_IEnumCOFFSymRW;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("01B3C322-B2FC-11d1-A71B-0060083E8C78")
    IEnumCOFFSymRW : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE Next( 
            /* [in] */ ULONG __MIDL_0113,
            /* [out] */ ICOFFSymRW __RPC_FAR *__RPC_FAR *__MIDL_0114,
            /* [out] */ ULONG __RPC_FAR *__MIDL_0115) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Skip( 
            /* [in] */ ULONG __MIDL_0116) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Reset( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Clone( 
            /* [out] */ IEnumCOFFSymRW __RPC_FAR *__RPC_FAR *__MIDL_0117) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IEnumCOFFSymRWVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IEnumCOFFSymRW __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IEnumCOFFSymRW __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IEnumCOFFSymRW __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Next )( 
            IEnumCOFFSymRW __RPC_FAR * This,
            /* [in] */ ULONG __MIDL_0113,
            /* [out] */ ICOFFSymRW __RPC_FAR *__RPC_FAR *__MIDL_0114,
            /* [out] */ ULONG __RPC_FAR *__MIDL_0115);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Skip )( 
            IEnumCOFFSymRW __RPC_FAR * This,
            /* [in] */ ULONG __MIDL_0116);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Reset )( 
            IEnumCOFFSymRW __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Clone )( 
            IEnumCOFFSymRW __RPC_FAR * This,
            /* [out] */ IEnumCOFFSymRW __RPC_FAR *__RPC_FAR *__MIDL_0117);
        
        END_INTERFACE
    } IEnumCOFFSymRWVtbl;

    interface IEnumCOFFSymRW
    {
        CONST_VTBL struct IEnumCOFFSymRWVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IEnumCOFFSymRW_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IEnumCOFFSymRW_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IEnumCOFFSymRW_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IEnumCOFFSymRW_Next(This,__MIDL_0113,__MIDL_0114,__MIDL_0115)	\
    (This)->lpVtbl -> Next(This,__MIDL_0113,__MIDL_0114,__MIDL_0115)

#define IEnumCOFFSymRW_Skip(This,__MIDL_0116)	\
    (This)->lpVtbl -> Skip(This,__MIDL_0116)

#define IEnumCOFFSymRW_Reset(This)	\
    (This)->lpVtbl -> Reset(This)

#define IEnumCOFFSymRW_Clone(This,__MIDL_0117)	\
    (This)->lpVtbl -> Clone(This,__MIDL_0117)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IEnumCOFFSymRW_Next_Proxy( 
    IEnumCOFFSymRW __RPC_FAR * This,
    /* [in] */ ULONG __MIDL_0113,
    /* [out] */ ICOFFSymRW __RPC_FAR *__RPC_FAR *__MIDL_0114,
    /* [out] */ ULONG __RPC_FAR *__MIDL_0115);


void __RPC_STUB IEnumCOFFSymRW_Next_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IEnumCOFFSymRW_Skip_Proxy( 
    IEnumCOFFSymRW __RPC_FAR * This,
    /* [in] */ ULONG __MIDL_0116);


void __RPC_STUB IEnumCOFFSymRW_Skip_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IEnumCOFFSymRW_Reset_Proxy( 
    IEnumCOFFSymRW __RPC_FAR * This);


void __RPC_STUB IEnumCOFFSymRW_Reset_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IEnumCOFFSymRW_Clone_Proxy( 
    IEnumCOFFSymRW __RPC_FAR * This,
    /* [out] */ IEnumCOFFSymRW __RPC_FAR *__RPC_FAR *__MIDL_0117);


void __RPC_STUB IEnumCOFFSymRW_Clone_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IEnumCOFFSymRW_INTERFACE_DEFINED__ */


#ifndef __ICOFFSymTabRO_INTERFACE_DEFINED__
#define __ICOFFSymTabRO_INTERFACE_DEFINED__

/****************************************
 * Generated header for interface: ICOFFSymTabRO
 * at Wed Jun 10 17:42:56 1998
 * using MIDL 3.03.0110
 ****************************************/
/* [unique][uuid][object] */ 


typedef /* [unique] */ ICOFFSymTabRO __RPC_FAR *PCOFFSymTabRO;


EXTERN_C const IID IID_ICOFFSymTabRO;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("CBDFD0D0-B21F-11d1-A71B-0060083E8C78")
    ICOFFSymTabRO : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE CSymbols( 
            /* [out] */ DWORD __RPC_FAR *__MIDL_0118) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetCOFFSymRO( 
            /* [in] */ DWORD __MIDL_0119,
            /* [out] */ ICOFFSymRO __RPC_FAR *__RPC_FAR *__MIDL_0120) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE ISYMNextCOFFSymRO( 
            /* [in] */ ICOFFSymRO __RPC_FAR *__MIDL_0121,
            /* [out] */ ISYM __RPC_FAR *__MIDL_0122) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE EmitCOFFSymbolTable( 
            /* [out] */ BYTE __RPC_FAR *__MIDL_0123,
            /* [in] */ DWORD __MIDL_0124) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE EnumCOFFSymRO( 
            /* [out] */ IEnumCOFFSymRO __RPC_FAR *__RPC_FAR *__MIDL_0125) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct ICOFFSymTabROVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            ICOFFSymTabRO __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            ICOFFSymTabRO __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            ICOFFSymTabRO __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *CSymbols )( 
            ICOFFSymTabRO __RPC_FAR * This,
            /* [out] */ DWORD __RPC_FAR *__MIDL_0118);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetCOFFSymRO )( 
            ICOFFSymTabRO __RPC_FAR * This,
            /* [in] */ DWORD __MIDL_0119,
            /* [out] */ ICOFFSymRO __RPC_FAR *__RPC_FAR *__MIDL_0120);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *ISYMNextCOFFSymRO )( 
            ICOFFSymTabRO __RPC_FAR * This,
            /* [in] */ ICOFFSymRO __RPC_FAR *__MIDL_0121,
            /* [out] */ ISYM __RPC_FAR *__MIDL_0122);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *EmitCOFFSymbolTable )( 
            ICOFFSymTabRO __RPC_FAR * This,
            /* [out] */ BYTE __RPC_FAR *__MIDL_0123,
            /* [in] */ DWORD __MIDL_0124);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *EnumCOFFSymRO )( 
            ICOFFSymTabRO __RPC_FAR * This,
            /* [out] */ IEnumCOFFSymRO __RPC_FAR *__RPC_FAR *__MIDL_0125);
        
        END_INTERFACE
    } ICOFFSymTabROVtbl;

    interface ICOFFSymTabRO
    {
        CONST_VTBL struct ICOFFSymTabROVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define ICOFFSymTabRO_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define ICOFFSymTabRO_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define ICOFFSymTabRO_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define ICOFFSymTabRO_CSymbols(This,__MIDL_0118)	\
    (This)->lpVtbl -> CSymbols(This,__MIDL_0118)

#define ICOFFSymTabRO_GetCOFFSymRO(This,__MIDL_0119,__MIDL_0120)	\
    (This)->lpVtbl -> GetCOFFSymRO(This,__MIDL_0119,__MIDL_0120)

#define ICOFFSymTabRO_ISYMNextCOFFSymRO(This,__MIDL_0121,__MIDL_0122)	\
    (This)->lpVtbl -> ISYMNextCOFFSymRO(This,__MIDL_0121,__MIDL_0122)

#define ICOFFSymTabRO_EmitCOFFSymbolTable(This,__MIDL_0123,__MIDL_0124)	\
    (This)->lpVtbl -> EmitCOFFSymbolTable(This,__MIDL_0123,__MIDL_0124)

#define ICOFFSymTabRO_EnumCOFFSymRO(This,__MIDL_0125)	\
    (This)->lpVtbl -> EnumCOFFSymRO(This,__MIDL_0125)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE ICOFFSymTabRO_CSymbols_Proxy( 
    ICOFFSymTabRO __RPC_FAR * This,
    /* [out] */ DWORD __RPC_FAR *__MIDL_0118);


void __RPC_STUB ICOFFSymTabRO_CSymbols_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ICOFFSymTabRO_GetCOFFSymRO_Proxy( 
    ICOFFSymTabRO __RPC_FAR * This,
    /* [in] */ DWORD __MIDL_0119,
    /* [out] */ ICOFFSymRO __RPC_FAR *__RPC_FAR *__MIDL_0120);


void __RPC_STUB ICOFFSymTabRO_GetCOFFSymRO_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ICOFFSymTabRO_ISYMNextCOFFSymRO_Proxy( 
    ICOFFSymTabRO __RPC_FAR * This,
    /* [in] */ ICOFFSymRO __RPC_FAR *__MIDL_0121,
    /* [out] */ ISYM __RPC_FAR *__MIDL_0122);


void __RPC_STUB ICOFFSymTabRO_ISYMNextCOFFSymRO_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ICOFFSymTabRO_EmitCOFFSymbolTable_Proxy( 
    ICOFFSymTabRO __RPC_FAR * This,
    /* [out] */ BYTE __RPC_FAR *__MIDL_0123,
    /* [in] */ DWORD __MIDL_0124);


void __RPC_STUB ICOFFSymTabRO_EmitCOFFSymbolTable_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ICOFFSymTabRO_EnumCOFFSymRO_Proxy( 
    ICOFFSymTabRO __RPC_FAR * This,
    /* [out] */ IEnumCOFFSymRO __RPC_FAR *__RPC_FAR *__MIDL_0125);


void __RPC_STUB ICOFFSymTabRO_EnumCOFFSymRO_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __ICOFFSymTabRO_INTERFACE_DEFINED__ */


#ifndef __ICOFFSymTabRW_INTERFACE_DEFINED__
#define __ICOFFSymTabRW_INTERFACE_DEFINED__

/****************************************
 * Generated header for interface: ICOFFSymTabRW
 * at Wed Jun 10 17:42:56 1998
 * using MIDL 3.03.0110
 ****************************************/
/* [unique][uuid][object] */ 


typedef /* [unique] */ ICOFFSymTabRW __RPC_FAR *PCOFFSymTabRW;


EXTERN_C const IID IID_ICOFFSymTabRW;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("6D2C4950-B788-11d1-A71B-0060083E8C78")
    ICOFFSymTabRW : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE CSymbols( 
            /* [out] */ DWORD __RPC_FAR *__MIDL_0126) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetCOFFSymRW( 
            /* [in] */ DWORD __MIDL_0127,
            /* [out] */ ICOFFSymRW __RPC_FAR *__RPC_FAR *__MIDL_0128) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE ISYMNextCOFFSymRW( 
            /* [in] */ ICOFFSymRW __RPC_FAR *__MIDL_0129,
            /* [out] */ ISYM __RPC_FAR *__MIDL_0130) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE EmitCOFFSymbolTable( 
            /* [out] */ BYTE __RPC_FAR *__MIDL_0131,
            /* [in] */ DWORD __MIDL_0132) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE EnumCOFFSymRW( 
            /* [out] */ IEnumCOFFSymRW __RPC_FAR *__RPC_FAR *__MIDL_0133) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct ICOFFSymTabRWVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            ICOFFSymTabRW __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            ICOFFSymTabRW __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            ICOFFSymTabRW __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *CSymbols )( 
            ICOFFSymTabRW __RPC_FAR * This,
            /* [out] */ DWORD __RPC_FAR *__MIDL_0126);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetCOFFSymRW )( 
            ICOFFSymTabRW __RPC_FAR * This,
            /* [in] */ DWORD __MIDL_0127,
            /* [out] */ ICOFFSymRW __RPC_FAR *__RPC_FAR *__MIDL_0128);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *ISYMNextCOFFSymRW )( 
            ICOFFSymTabRW __RPC_FAR * This,
            /* [in] */ ICOFFSymRW __RPC_FAR *__MIDL_0129,
            /* [out] */ ISYM __RPC_FAR *__MIDL_0130);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *EmitCOFFSymbolTable )( 
            ICOFFSymTabRW __RPC_FAR * This,
            /* [out] */ BYTE __RPC_FAR *__MIDL_0131,
            /* [in] */ DWORD __MIDL_0132);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *EnumCOFFSymRW )( 
            ICOFFSymTabRW __RPC_FAR * This,
            /* [out] */ IEnumCOFFSymRW __RPC_FAR *__RPC_FAR *__MIDL_0133);
        
        END_INTERFACE
    } ICOFFSymTabRWVtbl;

    interface ICOFFSymTabRW
    {
        CONST_VTBL struct ICOFFSymTabRWVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define ICOFFSymTabRW_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define ICOFFSymTabRW_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define ICOFFSymTabRW_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define ICOFFSymTabRW_CSymbols(This,__MIDL_0126)	\
    (This)->lpVtbl -> CSymbols(This,__MIDL_0126)

#define ICOFFSymTabRW_GetCOFFSymRW(This,__MIDL_0127,__MIDL_0128)	\
    (This)->lpVtbl -> GetCOFFSymRW(This,__MIDL_0127,__MIDL_0128)

#define ICOFFSymTabRW_ISYMNextCOFFSymRW(This,__MIDL_0129,__MIDL_0130)	\
    (This)->lpVtbl -> ISYMNextCOFFSymRW(This,__MIDL_0129,__MIDL_0130)

#define ICOFFSymTabRW_EmitCOFFSymbolTable(This,__MIDL_0131,__MIDL_0132)	\
    (This)->lpVtbl -> EmitCOFFSymbolTable(This,__MIDL_0131,__MIDL_0132)

#define ICOFFSymTabRW_EnumCOFFSymRW(This,__MIDL_0133)	\
    (This)->lpVtbl -> EnumCOFFSymRW(This,__MIDL_0133)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE ICOFFSymTabRW_CSymbols_Proxy( 
    ICOFFSymTabRW __RPC_FAR * This,
    /* [out] */ DWORD __RPC_FAR *__MIDL_0126);


void __RPC_STUB ICOFFSymTabRW_CSymbols_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ICOFFSymTabRW_GetCOFFSymRW_Proxy( 
    ICOFFSymTabRW __RPC_FAR * This,
    /* [in] */ DWORD __MIDL_0127,
    /* [out] */ ICOFFSymRW __RPC_FAR *__RPC_FAR *__MIDL_0128);


void __RPC_STUB ICOFFSymTabRW_GetCOFFSymRW_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ICOFFSymTabRW_ISYMNextCOFFSymRW_Proxy( 
    ICOFFSymTabRW __RPC_FAR * This,
    /* [in] */ ICOFFSymRW __RPC_FAR *__MIDL_0129,
    /* [out] */ ISYM __RPC_FAR *__MIDL_0130);


void __RPC_STUB ICOFFSymTabRW_ISYMNextCOFFSymRW_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ICOFFSymTabRW_EmitCOFFSymbolTable_Proxy( 
    ICOFFSymTabRW __RPC_FAR * This,
    /* [out] */ BYTE __RPC_FAR *__MIDL_0131,
    /* [in] */ DWORD __MIDL_0132);


void __RPC_STUB ICOFFSymTabRW_EmitCOFFSymbolTable_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ICOFFSymTabRW_EnumCOFFSymRW_Proxy( 
    ICOFFSymTabRW __RPC_FAR * This,
    /* [out] */ IEnumCOFFSymRW __RPC_FAR *__RPC_FAR *__MIDL_0133);


void __RPC_STUB ICOFFSymTabRW_EnumCOFFSymRW_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __ICOFFSymTabRW_INTERFACE_DEFINED__ */


#ifndef __ICOFFObj_INTERFACE_DEFINED__
#define __ICOFFObj_INTERFACE_DEFINED__

/****************************************
 * Generated header for interface: ICOFFObj
 * at Wed Jun 10 17:42:56 1998
 * using MIDL 3.03.0110
 ****************************************/
/* [unique][uuid][object] */ 


typedef /* [unique] */ ICOFFObj __RPC_FAR *PCOFFObj;


EXTERN_C const IID IID_ICOFFObj;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("A9470BB0-911F-11d1-A712-0060083E8C78")
    ICOFFObj : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE BeginOptCaching( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE CContrib( 
            /* [out] */ DWORD __RPC_FAR *__MIDL_0134) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Close( 
            /* [in] */ BOOL __MIDL_0135) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE COFFTimeStamp( 
            /* [out] */ DWORD __RPC_FAR *__MIDL_0136) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE CSymbols( 
            /* [out] */ DWORD __RPC_FAR *__MIDL_0137) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE EndOptCaching( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE EnumContrib( 
            /* [out] */ IEnumContrib __RPC_FAR *__RPC_FAR *__MIDL_0138) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE FImportObj( 
            /* [out] */ BOOL __RPC_FAR *__MIDL_0139) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetStringTable( 
            /* [out] */ PLinkDataRO __RPC_FAR *__MIDL_0140) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetSymbolTableRO( 
            /* [out] */ ICOFFSymTabRO __RPC_FAR *__RPC_FAR *__MIDL_0141) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetSymbolTableRW( 
            /* [out] */ ICOFFSymTabRW __RPC_FAR *__RPC_FAR *__MIDL_0142) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE ObjtkFromSzSymbol( 
            /* [in] */ PLinkDataRO __MIDL_0143,
            /* [out] */ OBJTK __RPC_FAR *__MIDL_0144) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE TimeStamp( 
            /* [out] */ DWORD __RPC_FAR *__MIDL_0145) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct ICOFFObjVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            ICOFFObj __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            ICOFFObj __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            ICOFFObj __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *BeginOptCaching )( 
            ICOFFObj __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *CContrib )( 
            ICOFFObj __RPC_FAR * This,
            /* [out] */ DWORD __RPC_FAR *__MIDL_0134);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Close )( 
            ICOFFObj __RPC_FAR * This,
            /* [in] */ BOOL __MIDL_0135);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *COFFTimeStamp )( 
            ICOFFObj __RPC_FAR * This,
            /* [out] */ DWORD __RPC_FAR *__MIDL_0136);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *CSymbols )( 
            ICOFFObj __RPC_FAR * This,
            /* [out] */ DWORD __RPC_FAR *__MIDL_0137);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *EndOptCaching )( 
            ICOFFObj __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *EnumContrib )( 
            ICOFFObj __RPC_FAR * This,
            /* [out] */ IEnumContrib __RPC_FAR *__RPC_FAR *__MIDL_0138);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *FImportObj )( 
            ICOFFObj __RPC_FAR * This,
            /* [out] */ BOOL __RPC_FAR *__MIDL_0139);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetStringTable )( 
            ICOFFObj __RPC_FAR * This,
            /* [out] */ PLinkDataRO __RPC_FAR *__MIDL_0140);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetSymbolTableRO )( 
            ICOFFObj __RPC_FAR * This,
            /* [out] */ ICOFFSymTabRO __RPC_FAR *__RPC_FAR *__MIDL_0141);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetSymbolTableRW )( 
            ICOFFObj __RPC_FAR * This,
            /* [out] */ ICOFFSymTabRW __RPC_FAR *__RPC_FAR *__MIDL_0142);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *ObjtkFromSzSymbol )( 
            ICOFFObj __RPC_FAR * This,
            /* [in] */ PLinkDataRO __MIDL_0143,
            /* [out] */ OBJTK __RPC_FAR *__MIDL_0144);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *TimeStamp )( 
            ICOFFObj __RPC_FAR * This,
            /* [out] */ DWORD __RPC_FAR *__MIDL_0145);
        
        END_INTERFACE
    } ICOFFObjVtbl;

    interface ICOFFObj
    {
        CONST_VTBL struct ICOFFObjVtbl __RPC_FAR *lpVtbl;
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

#define ICOFFObj_CContrib(This,__MIDL_0134)	\
    (This)->lpVtbl -> CContrib(This,__MIDL_0134)

#define ICOFFObj_Close(This,__MIDL_0135)	\
    (This)->lpVtbl -> Close(This,__MIDL_0135)

#define ICOFFObj_COFFTimeStamp(This,__MIDL_0136)	\
    (This)->lpVtbl -> COFFTimeStamp(This,__MIDL_0136)

#define ICOFFObj_CSymbols(This,__MIDL_0137)	\
    (This)->lpVtbl -> CSymbols(This,__MIDL_0137)

#define ICOFFObj_EndOptCaching(This)	\
    (This)->lpVtbl -> EndOptCaching(This)

#define ICOFFObj_EnumContrib(This,__MIDL_0138)	\
    (This)->lpVtbl -> EnumContrib(This,__MIDL_0138)

#define ICOFFObj_FImportObj(This,__MIDL_0139)	\
    (This)->lpVtbl -> FImportObj(This,__MIDL_0139)

#define ICOFFObj_GetStringTable(This,__MIDL_0140)	\
    (This)->lpVtbl -> GetStringTable(This,__MIDL_0140)

#define ICOFFObj_GetSymbolTableRO(This,__MIDL_0141)	\
    (This)->lpVtbl -> GetSymbolTableRO(This,__MIDL_0141)

#define ICOFFObj_GetSymbolTableRW(This,__MIDL_0142)	\
    (This)->lpVtbl -> GetSymbolTableRW(This,__MIDL_0142)

#define ICOFFObj_ObjtkFromSzSymbol(This,__MIDL_0143,__MIDL_0144)	\
    (This)->lpVtbl -> ObjtkFromSzSymbol(This,__MIDL_0143,__MIDL_0144)

#define ICOFFObj_TimeStamp(This,__MIDL_0145)	\
    (This)->lpVtbl -> TimeStamp(This,__MIDL_0145)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE ICOFFObj_BeginOptCaching_Proxy( 
    ICOFFObj __RPC_FAR * This);


void __RPC_STUB ICOFFObj_BeginOptCaching_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ICOFFObj_CContrib_Proxy( 
    ICOFFObj __RPC_FAR * This,
    /* [out] */ DWORD __RPC_FAR *__MIDL_0134);


void __RPC_STUB ICOFFObj_CContrib_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ICOFFObj_Close_Proxy( 
    ICOFFObj __RPC_FAR * This,
    /* [in] */ BOOL __MIDL_0135);


void __RPC_STUB ICOFFObj_Close_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ICOFFObj_COFFTimeStamp_Proxy( 
    ICOFFObj __RPC_FAR * This,
    /* [out] */ DWORD __RPC_FAR *__MIDL_0136);


void __RPC_STUB ICOFFObj_COFFTimeStamp_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ICOFFObj_CSymbols_Proxy( 
    ICOFFObj __RPC_FAR * This,
    /* [out] */ DWORD __RPC_FAR *__MIDL_0137);


void __RPC_STUB ICOFFObj_CSymbols_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ICOFFObj_EndOptCaching_Proxy( 
    ICOFFObj __RPC_FAR * This);


void __RPC_STUB ICOFFObj_EndOptCaching_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ICOFFObj_EnumContrib_Proxy( 
    ICOFFObj __RPC_FAR * This,
    /* [out] */ IEnumContrib __RPC_FAR *__RPC_FAR *__MIDL_0138);


void __RPC_STUB ICOFFObj_EnumContrib_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ICOFFObj_FImportObj_Proxy( 
    ICOFFObj __RPC_FAR * This,
    /* [out] */ BOOL __RPC_FAR *__MIDL_0139);


void __RPC_STUB ICOFFObj_FImportObj_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ICOFFObj_GetStringTable_Proxy( 
    ICOFFObj __RPC_FAR * This,
    /* [out] */ PLinkDataRO __RPC_FAR *__MIDL_0140);


void __RPC_STUB ICOFFObj_GetStringTable_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ICOFFObj_GetSymbolTableRO_Proxy( 
    ICOFFObj __RPC_FAR * This,
    /* [out] */ ICOFFSymTabRO __RPC_FAR *__RPC_FAR *__MIDL_0141);


void __RPC_STUB ICOFFObj_GetSymbolTableRO_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ICOFFObj_GetSymbolTableRW_Proxy( 
    ICOFFObj __RPC_FAR * This,
    /* [out] */ ICOFFSymTabRW __RPC_FAR *__RPC_FAR *__MIDL_0142);


void __RPC_STUB ICOFFObj_GetSymbolTableRW_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ICOFFObj_ObjtkFromSzSymbol_Proxy( 
    ICOFFObj __RPC_FAR * This,
    /* [in] */ PLinkDataRO __MIDL_0143,
    /* [out] */ OBJTK __RPC_FAR *__MIDL_0144);


void __RPC_STUB ICOFFObj_ObjtkFromSzSymbol_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ICOFFObj_TimeStamp_Proxy( 
    ICOFFObj __RPC_FAR * This,
    /* [out] */ DWORD __RPC_FAR *__MIDL_0145);


void __RPC_STUB ICOFFObj_TimeStamp_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __ICOFFObj_INTERFACE_DEFINED__ */


/****************************************
 * Generated header for interface: __MIDL_itf_objint_0090
 * at Wed Jun 10 17:42:56 1998
 * using MIDL 3.03.0110
 ****************************************/
/* [local] */ 


typedef /* [public][public] */ 
enum __MIDL___MIDL_itf_objint_0090_0001
    {	tleMultDef	= 0,
	tleSecAttrib	= tleMultDef + 1,
	tleUnknown	= tleSecAttrib + 1
    }	TLE;



extern RPC_IF_HANDLE __MIDL_itf_objint_0090_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_objint_0090_v0_0_s_ifspec;

#ifndef __ILinkError_INTERFACE_DEFINED__
#define __ILinkError_INTERFACE_DEFINED__

/****************************************
 * Generated header for interface: ILinkError
 * at Wed Jun 10 17:42:56 1998
 * using MIDL 3.03.0110
 ****************************************/
/* [unique][uuid][object] */ 



EXTERN_C const IID IID_ILinkError;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("2DEBF730-F5C3-11d1-A73F-0060083E8C78")
    ILinkError : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE ErrorType( 
            /* [out] */ TLE __RPC_FAR *__MIDL_0146) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Objtk1( 
            /* [out] */ OBJTK __RPC_FAR *__MIDL_0147) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Objtk2( 
            /* [out] */ OBJTK __RPC_FAR *__MIDL_0148) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Sz( 
            /* [out] */ PLinkDataRO __RPC_FAR *__MIDL_0149) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE DwInfo( 
            /* [out] */ DWORD __RPC_FAR *__MIDL_0150) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct ILinkErrorVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            ILinkError __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            ILinkError __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            ILinkError __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *ErrorType )( 
            ILinkError __RPC_FAR * This,
            /* [out] */ TLE __RPC_FAR *__MIDL_0146);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Objtk1 )( 
            ILinkError __RPC_FAR * This,
            /* [out] */ OBJTK __RPC_FAR *__MIDL_0147);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Objtk2 )( 
            ILinkError __RPC_FAR * This,
            /* [out] */ OBJTK __RPC_FAR *__MIDL_0148);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Sz )( 
            ILinkError __RPC_FAR * This,
            /* [out] */ PLinkDataRO __RPC_FAR *__MIDL_0149);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *DwInfo )( 
            ILinkError __RPC_FAR * This,
            /* [out] */ DWORD __RPC_FAR *__MIDL_0150);
        
        END_INTERFACE
    } ILinkErrorVtbl;

    interface ILinkError
    {
        CONST_VTBL struct ILinkErrorVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define ILinkError_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define ILinkError_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define ILinkError_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define ILinkError_ErrorType(This,__MIDL_0146)	\
    (This)->lpVtbl -> ErrorType(This,__MIDL_0146)

#define ILinkError_Objtk1(This,__MIDL_0147)	\
    (This)->lpVtbl -> Objtk1(This,__MIDL_0147)

#define ILinkError_Objtk2(This,__MIDL_0148)	\
    (This)->lpVtbl -> Objtk2(This,__MIDL_0148)

#define ILinkError_Sz(This,__MIDL_0149)	\
    (This)->lpVtbl -> Sz(This,__MIDL_0149)

#define ILinkError_DwInfo(This,__MIDL_0150)	\
    (This)->lpVtbl -> DwInfo(This,__MIDL_0150)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE ILinkError_ErrorType_Proxy( 
    ILinkError __RPC_FAR * This,
    /* [out] */ TLE __RPC_FAR *__MIDL_0146);


void __RPC_STUB ILinkError_ErrorType_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ILinkError_Objtk1_Proxy( 
    ILinkError __RPC_FAR * This,
    /* [out] */ OBJTK __RPC_FAR *__MIDL_0147);


void __RPC_STUB ILinkError_Objtk1_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ILinkError_Objtk2_Proxy( 
    ILinkError __RPC_FAR * This,
    /* [out] */ OBJTK __RPC_FAR *__MIDL_0148);


void __RPC_STUB ILinkError_Objtk2_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ILinkError_Sz_Proxy( 
    ILinkError __RPC_FAR * This,
    /* [out] */ PLinkDataRO __RPC_FAR *__MIDL_0149);


void __RPC_STUB ILinkError_Sz_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ILinkError_DwInfo_Proxy( 
    ILinkError __RPC_FAR * This,
    /* [out] */ DWORD __RPC_FAR *__MIDL_0150);


void __RPC_STUB ILinkError_DwInfo_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __ILinkError_INTERFACE_DEFINED__ */


#ifndef __ILinkNotify_INTERFACE_DEFINED__
#define __ILinkNotify_INTERFACE_DEFINED__

/****************************************
 * Generated header for interface: ILinkNotify
 * at Wed Jun 10 17:42:56 1998
 * using MIDL 3.03.0110
 ****************************************/
/* [unique][uuid][object] */ 


typedef /* [unique] */ ILinkNotify __RPC_FAR *PLinkNotify;


EXTERN_C const IID IID_ILinkNotify;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("9D7E97A0-F5B6-11d1-A73F-0060083E8C78")
    ILinkNotify : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE OnError( 
            /* [in] */ ILinkError __RPC_FAR *__MIDL_0151) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE OnStatus( 
            /* [in] */ DWORD __MIDL_0152,
            /* [in] */ DWORD __MIDL_0153) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct ILinkNotifyVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            ILinkNotify __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            ILinkNotify __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            ILinkNotify __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *OnError )( 
            ILinkNotify __RPC_FAR * This,
            /* [in] */ ILinkError __RPC_FAR *__MIDL_0151);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *OnStatus )( 
            ILinkNotify __RPC_FAR * This,
            /* [in] */ DWORD __MIDL_0152,
            /* [in] */ DWORD __MIDL_0153);
        
        END_INTERFACE
    } ILinkNotifyVtbl;

    interface ILinkNotify
    {
        CONST_VTBL struct ILinkNotifyVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define ILinkNotify_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define ILinkNotify_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define ILinkNotify_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define ILinkNotify_OnError(This,__MIDL_0151)	\
    (This)->lpVtbl -> OnError(This,__MIDL_0151)

#define ILinkNotify_OnStatus(This,__MIDL_0152,__MIDL_0153)	\
    (This)->lpVtbl -> OnStatus(This,__MIDL_0152,__MIDL_0153)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE ILinkNotify_OnError_Proxy( 
    ILinkNotify __RPC_FAR * This,
    /* [in] */ ILinkError __RPC_FAR *__MIDL_0151);


void __RPC_STUB ILinkNotify_OnError_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ILinkNotify_OnStatus_Proxy( 
    ILinkNotify __RPC_FAR * This,
    /* [in] */ DWORD __MIDL_0152,
    /* [in] */ DWORD __MIDL_0153);


void __RPC_STUB ILinkNotify_OnStatus_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __ILinkNotify_INTERFACE_DEFINED__ */


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
