/* this ALWAYS GENERATED file contains the definitions for the interfaces */


/* File created by MIDL compiler version 3.01.75 */
/* at Wed Apr 22 19:48:30 1998
 */
/* Compiler settings for brokcfg.idl:
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

#ifndef __brokcfg_h__
#define __brokcfg_h__

#ifdef __cplusplus
extern "C"{
#endif 

/* Forward Declarations */ 

#ifndef __IBrokConfig_FWD_DEFINED__
#define __IBrokConfig_FWD_DEFINED__
typedef interface IBrokConfig IBrokConfig;
#endif 	/* __IBrokConfig_FWD_DEFINED__ */


#ifndef __IBrokServers_FWD_DEFINED__
#define __IBrokServers_FWD_DEFINED__
typedef interface IBrokServers IBrokServers;
#endif 	/* __IBrokServers_FWD_DEFINED__ */


#ifndef __BrokConfig_FWD_DEFINED__
#define __BrokConfig_FWD_DEFINED__

#ifdef __cplusplus
typedef class BrokConfig BrokConfig;
#else
typedef struct BrokConfig BrokConfig;
#endif /* __cplusplus */

#endif 	/* __BrokConfig_FWD_DEFINED__ */


#ifndef __BrokServers_FWD_DEFINED__
#define __BrokServers_FWD_DEFINED__

#ifdef __cplusplus
typedef class BrokServers BrokServers;
#else
typedef struct BrokServers BrokServers;
#endif /* __cplusplus */

#endif 	/* __BrokServers_FWD_DEFINED__ */


/* header files for imported files */
#include "oaidl.h"
#include "ocidl.h"

void __RPC_FAR * __RPC_USER MIDL_user_allocate(size_t);
void __RPC_USER MIDL_user_free( void __RPC_FAR * ); 

/****************************************
 * Generated header for interface: __MIDL_itf_brokcfg_0000
 * at Wed Apr 22 19:48:30 1998
 * using MIDL 3.01.75
 ****************************************/
/* [local] */ 


#define	BROK_AUTH_TYPE_ANON	( 0x1 )

#define	BROK_AUTH_TYPE_PWDCOOKIE	( 0x2 )

#define	BROK_AUTH_TYPE_BASIC	( 0x4 )

#define	BROK_AUTH_TYPE_DPA	( 0x8 )

#define	IIS_AUTH_TYPE_ALLOW_ANON	( 0x10 )

#define	BROK_ALWAYS_WRITE	( 0x20 )

#define	BROK_IGNORE_ALLOW_ANON	( 0x40 )



extern RPC_IF_HANDLE __MIDL_itf_brokcfg_0000_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_brokcfg_0000_v0_0_s_ifspec;

#ifndef __IBrokConfig_INTERFACE_DEFINED__
#define __IBrokConfig_INTERFACE_DEFINED__

/****************************************
 * Generated header for interface: IBrokConfig
 * at Wed Apr 22 19:48:30 1998
 * using MIDL 3.01.75
 ****************************************/
/* [unique][helpstring][dual][uuid][object] */ 



EXTERN_C const IID IID_IBrokConfig;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    interface DECLSPEC_UUID("19edab12-c4a4-11d0-bbda-00c04fb615e5")
    IBrokConfig : public IDispatch
    {
    public:
        virtual /* [hidden] */ HRESULT STDMETHODCALLTYPE LoadDefaults( 
            LONG lVirtServId) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetConfig( 
            LONG lVirtServId) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetConfig( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE CheckAcct( 
            BSTR bszDomain,
            BSTR bszName,
            BSTR bszPassword) = 0;
        
        virtual /* [id][propget] */ HRESULT STDMETHODCALLTYPE get_lVirtServId( 
            /* [retval][out] */ LONG __RPC_FAR *pRetVal) = 0;
        
        virtual /* [id][propput] */ HRESULT STDMETHODCALLTYPE put_lVirtServId( 
            /* [in] */ LONG NewVal) = 0;
        
        virtual /* [id][propget] */ HRESULT STDMETHODCALLTYPE get_bLocal( 
            /* [retval][out] */ BOOL __RPC_FAR *pRetVal) = 0;
        
        virtual /* [id][propput] */ HRESULT STDMETHODCALLTYPE put_bLocal( 
            /* [in] */ BOOL NewVal) = 0;
        
        virtual /* [id][propget] */ HRESULT STDMETHODCALLTYPE get_bszServerName( 
            /* [retval][out] */ BSTR __RPC_FAR *pRetVal) = 0;
        
        virtual /* [id][propput] */ HRESULT STDMETHODCALLTYPE put_bszServerName( 
            /* [in] */ BSTR NewVal) = 0;
        
        virtual /* [id][propget] */ HRESULT STDMETHODCALLTYPE get_lPort( 
            /* [retval][out] */ LONG __RPC_FAR *pRetVal) = 0;
        
        virtual /* [id][propput] */ HRESULT STDMETHODCALLTYPE put_lPort( 
            /* [in] */ LONG NewVal) = 0;
        
        virtual /* [id][propget] */ HRESULT STDMETHODCALLTYPE get_bSecure( 
            /* [retval][out] */ BOOL __RPC_FAR *pRetVal) = 0;
        
        virtual /* [id][propput] */ HRESULT STDMETHODCALLTYPE put_bSecure( 
            /* [in] */ BOOL NewVal) = 0;
        
        virtual /* [id][propget] */ HRESULT STDMETHODCALLTYPE get_lSecurePort( 
            /* [retval][out] */ LONG __RPC_FAR *pRetVal) = 0;
        
        virtual /* [id][propput] */ HRESULT STDMETHODCALLTYPE put_lSecurePort( 
            /* [in] */ LONG NewVal) = 0;
        
        virtual /* [id][propget] */ HRESULT STDMETHODCALLTYPE get_lTimeLimit( 
            /* [retval][out] */ LONG __RPC_FAR *pRetVal) = 0;
        
        virtual /* [id][propput] */ HRESULT STDMETHODCALLTYPE put_lTimeLimit( 
            /* [in] */ LONG NewVal) = 0;
        
        virtual /* [id][propget] */ HRESULT STDMETHODCALLTYPE get_lSizeLimit( 
            /* [retval][out] */ LONG __RPC_FAR *pRetVal) = 0;
        
        virtual /* [id][propput] */ HRESULT STDMETHODCALLTYPE put_lSizeLimit( 
            /* [in] */ LONG NewVal) = 0;
        
        virtual /* [id][propget] */ HRESULT STDMETHODCALLTYPE get_bszBaseDN( 
            /* [retval][out] */ BSTR __RPC_FAR *pRetVal) = 0;
        
        virtual /* [id][propput] */ HRESULT STDMETHODCALLTYPE put_bszBaseDN( 
            /* [in] */ BSTR NewVal) = 0;
        
        virtual /* [id][propget] */ HRESULT STDMETHODCALLTYPE get_lCacheTimeout( 
            /* [retval][out] */ LONG __RPC_FAR *pRetVal) = 0;
        
        virtual /* [id][propput] */ HRESULT STDMETHODCALLTYPE put_lCacheTimeout( 
            /* [in] */ LONG NewVal) = 0;
        
        virtual /* [id][propget] */ HRESULT STDMETHODCALLTYPE get_bszGroupPrefix( 
            /* [retval][out] */ BSTR __RPC_FAR *pRetVal) = 0;
        
        virtual /* [id][propput] */ HRESULT STDMETHODCALLTYPE put_bszGroupPrefix( 
            /* [in] */ BSTR NewVal) = 0;
        
        virtual /* [id][propget] */ HRESULT STDMETHODCALLTYPE get_bCreateGroups( 
            /* [retval][out] */ BOOL __RPC_FAR *pRetVal) = 0;
        
        virtual /* [id][propput] */ HRESULT STDMETHODCALLTYPE put_bCreateGroups( 
            /* [in] */ BOOL NewVal) = 0;
        
        virtual /* [id][propget] */ HRESULT STDMETHODCALLTYPE get_bszDomain( 
            /* [retval][out] */ BSTR __RPC_FAR *pRetVal) = 0;
        
        virtual /* [id][propput] */ HRESULT STDMETHODCALLTYPE put_bszDomain( 
            /* [in] */ BSTR NewVal) = 0;
        
        virtual /* [id][propget] */ HRESULT STDMETHODCALLTYPE get_bszDsName( 
            /* [retval][out] */ BSTR __RPC_FAR *pRetVal) = 0;
        
        virtual /* [id][propput] */ HRESULT STDMETHODCALLTYPE put_bszDsName( 
            /* [in] */ BSTR NewVal) = 0;
        
        virtual /* [id][propget] */ HRESULT STDMETHODCALLTYPE get_bszDsPwd( 
            /* [retval][out] */ BSTR __RPC_FAR *pRetVal) = 0;
        
        virtual /* [id][propput] */ HRESULT STDMETHODCALLTYPE put_bszDsPwd( 
            /* [in] */ BSTR NewVal) = 0;
        
        virtual /* [id][propget] */ HRESULT STDMETHODCALLTYPE get_bszProxyName( 
            /* [retval][out] */ BSTR __RPC_FAR *pRetVal) = 0;
        
        virtual /* [id][propput] */ HRESULT STDMETHODCALLTYPE put_bszProxyName( 
            /* [in] */ BSTR NewVal) = 0;
        
        virtual /* [id][propget] */ HRESULT STDMETHODCALLTYPE get_bszProxyPwd( 
            /* [retval][out] */ BSTR __RPC_FAR *pRetVal) = 0;
        
        virtual /* [id][propput] */ HRESULT STDMETHODCALLTYPE put_bszProxyPwd( 
            /* [in] */ BSTR NewVal) = 0;
        
        virtual /* [id][propget] */ HRESULT STDMETHODCALLTYPE get_bszProxyDomain( 
            /* [retval][out] */ BSTR __RPC_FAR *pRetVal) = 0;
        
        virtual /* [id][propput] */ HRESULT STDMETHODCALLTYPE put_bszProxyDomain( 
            /* [in] */ BSTR NewVal) = 0;
        
        virtual /* [id][propget] */ HRESULT STDMETHODCALLTYPE get_bUseTrackCookie( 
            /* [retval][out] */ BOOL __RPC_FAR *pRetVal) = 0;
        
        virtual /* [id][propput] */ HRESULT STDMETHODCALLTYPE put_bUseTrackCookie( 
            /* [in] */ BOOL NewVal) = 0;
        
        virtual /* [id][propget] */ HRESULT STDMETHODCALLTYPE get_lPwdCookieTimeout( 
            /* [retval][out] */ LONG __RPC_FAR *pRetVal) = 0;
        
        virtual /* [id][propput] */ HRESULT STDMETHODCALLTYPE put_lPwdCookieTimeout( 
            /* [in] */ LONG NewVal) = 0;
        
        virtual /* [id][propget] */ HRESULT STDMETHODCALLTYPE get_bEnabled( 
            /* [retval][out] */ BOOL __RPC_FAR *pRetVal) = 0;
        
        virtual /* [id][propput] */ HRESULT STDMETHODCALLTYPE put_bEnabled( 
            /* [in] */ BOOL NewVal) = 0;
        
        virtual /* [id][propget] */ HRESULT STDMETHODCALLTYPE get_bszComment( 
            /* [retval][out] */ BSTR __RPC_FAR *pRetVal) = 0;
        
        virtual /* [id][propput] */ HRESULT STDMETHODCALLTYPE put_bszComment( 
            /* [in] */ BSTR NewVal) = 0;
        
        virtual /* [id][propget] */ HRESULT STDMETHODCALLTYPE get_bPwdCookiePersist( 
            /* [retval][out] */ BOOL __RPC_FAR *pRetVal) = 0;
        
        virtual /* [id][propput] */ HRESULT STDMETHODCALLTYPE put_bPwdCookiePersist( 
            /* [in] */ BOOL NewVal) = 0;
        
        virtual /* [id][propget] */ HRESULT STDMETHODCALLTYPE get_bszRealm( 
            /* [retval][out] */ BSTR __RPC_FAR *pRetVal) = 0;
        
        virtual /* [id][propput] */ HRESULT STDMETHODCALLTYPE put_bszRealm( 
            /* [in] */ BSTR NewVal) = 0;
        
        virtual /* [id][propget] */ HRESULT STDMETHODCALLTYPE get_lTokenCacheTimeout( 
            /* [retval][out] */ LONG __RPC_FAR *pRetVal) = 0;
        
        virtual /* [id][propput] */ HRESULT STDMETHODCALLTYPE put_lTokenCacheTimeout( 
            /* [in] */ LONG NewVal) = 0;
        
        virtual /* [id][propget] */ HRESULT STDMETHODCALLTYPE get_bszTokenCreatorDll( 
            /* [retval][out] */ BSTR __RPC_FAR *pRetVal) = 0;
        
        virtual /* [id][propput] */ HRESULT STDMETHODCALLTYPE put_bszTokenCreatorDll( 
            /* [in] */ BSTR NewVal) = 0;
        
        virtual /* [id][propget] */ HRESULT STDMETHODCALLTYPE get_bDirty( 
            /* [retval][out] */ BOOL __RPC_FAR *pRetVal) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IBrokConfigVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IBrokConfig __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IBrokConfig __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IBrokConfig __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            IBrokConfig __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            IBrokConfig __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            IBrokConfig __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            IBrokConfig __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        /* [hidden] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *LoadDefaults )( 
            IBrokConfig __RPC_FAR * This,
            LONG lVirtServId);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetConfig )( 
            IBrokConfig __RPC_FAR * This,
            LONG lVirtServId);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *SetConfig )( 
            IBrokConfig __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *CheckAcct )( 
            IBrokConfig __RPC_FAR * This,
            BSTR bszDomain,
            BSTR bszName,
            BSTR bszPassword);
        
        /* [id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_lVirtServId )( 
            IBrokConfig __RPC_FAR * This,
            /* [retval][out] */ LONG __RPC_FAR *pRetVal);
        
        /* [id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_lVirtServId )( 
            IBrokConfig __RPC_FAR * This,
            /* [in] */ LONG NewVal);
        
        /* [id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_bLocal )( 
            IBrokConfig __RPC_FAR * This,
            /* [retval][out] */ BOOL __RPC_FAR *pRetVal);
        
        /* [id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_bLocal )( 
            IBrokConfig __RPC_FAR * This,
            /* [in] */ BOOL NewVal);
        
        /* [id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_bszServerName )( 
            IBrokConfig __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pRetVal);
        
        /* [id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_bszServerName )( 
            IBrokConfig __RPC_FAR * This,
            /* [in] */ BSTR NewVal);
        
        /* [id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_lPort )( 
            IBrokConfig __RPC_FAR * This,
            /* [retval][out] */ LONG __RPC_FAR *pRetVal);
        
        /* [id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_lPort )( 
            IBrokConfig __RPC_FAR * This,
            /* [in] */ LONG NewVal);
        
        /* [id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_bSecure )( 
            IBrokConfig __RPC_FAR * This,
            /* [retval][out] */ BOOL __RPC_FAR *pRetVal);
        
        /* [id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_bSecure )( 
            IBrokConfig __RPC_FAR * This,
            /* [in] */ BOOL NewVal);
        
        /* [id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_lSecurePort )( 
            IBrokConfig __RPC_FAR * This,
            /* [retval][out] */ LONG __RPC_FAR *pRetVal);
        
        /* [id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_lSecurePort )( 
            IBrokConfig __RPC_FAR * This,
            /* [in] */ LONG NewVal);
        
        /* [id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_lTimeLimit )( 
            IBrokConfig __RPC_FAR * This,
            /* [retval][out] */ LONG __RPC_FAR *pRetVal);
        
        /* [id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_lTimeLimit )( 
            IBrokConfig __RPC_FAR * This,
            /* [in] */ LONG NewVal);
        
        /* [id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_lSizeLimit )( 
            IBrokConfig __RPC_FAR * This,
            /* [retval][out] */ LONG __RPC_FAR *pRetVal);
        
        /* [id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_lSizeLimit )( 
            IBrokConfig __RPC_FAR * This,
            /* [in] */ LONG NewVal);
        
        /* [id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_bszBaseDN )( 
            IBrokConfig __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pRetVal);
        
        /* [id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_bszBaseDN )( 
            IBrokConfig __RPC_FAR * This,
            /* [in] */ BSTR NewVal);
        
        /* [id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_lCacheTimeout )( 
            IBrokConfig __RPC_FAR * This,
            /* [retval][out] */ LONG __RPC_FAR *pRetVal);
        
        /* [id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_lCacheTimeout )( 
            IBrokConfig __RPC_FAR * This,
            /* [in] */ LONG NewVal);
        
        /* [id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_bszGroupPrefix )( 
            IBrokConfig __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pRetVal);
        
        /* [id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_bszGroupPrefix )( 
            IBrokConfig __RPC_FAR * This,
            /* [in] */ BSTR NewVal);
        
        /* [id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_bCreateGroups )( 
            IBrokConfig __RPC_FAR * This,
            /* [retval][out] */ BOOL __RPC_FAR *pRetVal);
        
        /* [id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_bCreateGroups )( 
            IBrokConfig __RPC_FAR * This,
            /* [in] */ BOOL NewVal);
        
        /* [id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_bszDomain )( 
            IBrokConfig __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pRetVal);
        
        /* [id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_bszDomain )( 
            IBrokConfig __RPC_FAR * This,
            /* [in] */ BSTR NewVal);
        
        /* [id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_bszDsName )( 
            IBrokConfig __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pRetVal);
        
        /* [id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_bszDsName )( 
            IBrokConfig __RPC_FAR * This,
            /* [in] */ BSTR NewVal);
        
        /* [id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_bszDsPwd )( 
            IBrokConfig __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pRetVal);
        
        /* [id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_bszDsPwd )( 
            IBrokConfig __RPC_FAR * This,
            /* [in] */ BSTR NewVal);
        
        /* [id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_bszProxyName )( 
            IBrokConfig __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pRetVal);
        
        /* [id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_bszProxyName )( 
            IBrokConfig __RPC_FAR * This,
            /* [in] */ BSTR NewVal);
        
        /* [id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_bszProxyPwd )( 
            IBrokConfig __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pRetVal);
        
        /* [id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_bszProxyPwd )( 
            IBrokConfig __RPC_FAR * This,
            /* [in] */ BSTR NewVal);
        
        /* [id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_bszProxyDomain )( 
            IBrokConfig __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pRetVal);
        
        /* [id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_bszProxyDomain )( 
            IBrokConfig __RPC_FAR * This,
            /* [in] */ BSTR NewVal);
        
        /* [id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_bUseTrackCookie )( 
            IBrokConfig __RPC_FAR * This,
            /* [retval][out] */ BOOL __RPC_FAR *pRetVal);
        
        /* [id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_bUseTrackCookie )( 
            IBrokConfig __RPC_FAR * This,
            /* [in] */ BOOL NewVal);
        
        /* [id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_lPwdCookieTimeout )( 
            IBrokConfig __RPC_FAR * This,
            /* [retval][out] */ LONG __RPC_FAR *pRetVal);
        
        /* [id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_lPwdCookieTimeout )( 
            IBrokConfig __RPC_FAR * This,
            /* [in] */ LONG NewVal);
        
        /* [id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_bEnabled )( 
            IBrokConfig __RPC_FAR * This,
            /* [retval][out] */ BOOL __RPC_FAR *pRetVal);
        
        /* [id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_bEnabled )( 
            IBrokConfig __RPC_FAR * This,
            /* [in] */ BOOL NewVal);
        
        /* [id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_bszComment )( 
            IBrokConfig __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pRetVal);
        
        /* [id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_bszComment )( 
            IBrokConfig __RPC_FAR * This,
            /* [in] */ BSTR NewVal);
        
        /* [id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_bPwdCookiePersist )( 
            IBrokConfig __RPC_FAR * This,
            /* [retval][out] */ BOOL __RPC_FAR *pRetVal);
        
        /* [id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_bPwdCookiePersist )( 
            IBrokConfig __RPC_FAR * This,
            /* [in] */ BOOL NewVal);
        
        /* [id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_bszRealm )( 
            IBrokConfig __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pRetVal);
        
        /* [id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_bszRealm )( 
            IBrokConfig __RPC_FAR * This,
            /* [in] */ BSTR NewVal);
        
        /* [id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_lTokenCacheTimeout )( 
            IBrokConfig __RPC_FAR * This,
            /* [retval][out] */ LONG __RPC_FAR *pRetVal);
        
        /* [id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_lTokenCacheTimeout )( 
            IBrokConfig __RPC_FAR * This,
            /* [in] */ LONG NewVal);
        
        /* [id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_bszTokenCreatorDll )( 
            IBrokConfig __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pRetVal);
        
        /* [id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_bszTokenCreatorDll )( 
            IBrokConfig __RPC_FAR * This,
            /* [in] */ BSTR NewVal);
        
        /* [id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_bDirty )( 
            IBrokConfig __RPC_FAR * This,
            /* [retval][out] */ BOOL __RPC_FAR *pRetVal);
        
        END_INTERFACE
    } IBrokConfigVtbl;

    interface IBrokConfig
    {
        CONST_VTBL struct IBrokConfigVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IBrokConfig_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IBrokConfig_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IBrokConfig_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IBrokConfig_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IBrokConfig_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IBrokConfig_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IBrokConfig_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IBrokConfig_LoadDefaults(This,lVirtServId)	\
    (This)->lpVtbl -> LoadDefaults(This,lVirtServId)

#define IBrokConfig_GetConfig(This,lVirtServId)	\
    (This)->lpVtbl -> GetConfig(This,lVirtServId)

#define IBrokConfig_SetConfig(This)	\
    (This)->lpVtbl -> SetConfig(This)

#define IBrokConfig_CheckAcct(This,bszDomain,bszName,bszPassword)	\
    (This)->lpVtbl -> CheckAcct(This,bszDomain,bszName,bszPassword)

#define IBrokConfig_get_lVirtServId(This,pRetVal)	\
    (This)->lpVtbl -> get_lVirtServId(This,pRetVal)

#define IBrokConfig_put_lVirtServId(This,NewVal)	\
    (This)->lpVtbl -> put_lVirtServId(This,NewVal)

#define IBrokConfig_get_bLocal(This,pRetVal)	\
    (This)->lpVtbl -> get_bLocal(This,pRetVal)

#define IBrokConfig_put_bLocal(This,NewVal)	\
    (This)->lpVtbl -> put_bLocal(This,NewVal)

#define IBrokConfig_get_bszServerName(This,pRetVal)	\
    (This)->lpVtbl -> get_bszServerName(This,pRetVal)

#define IBrokConfig_put_bszServerName(This,NewVal)	\
    (This)->lpVtbl -> put_bszServerName(This,NewVal)

#define IBrokConfig_get_lPort(This,pRetVal)	\
    (This)->lpVtbl -> get_lPort(This,pRetVal)

#define IBrokConfig_put_lPort(This,NewVal)	\
    (This)->lpVtbl -> put_lPort(This,NewVal)

#define IBrokConfig_get_bSecure(This,pRetVal)	\
    (This)->lpVtbl -> get_bSecure(This,pRetVal)

#define IBrokConfig_put_bSecure(This,NewVal)	\
    (This)->lpVtbl -> put_bSecure(This,NewVal)

#define IBrokConfig_get_lSecurePort(This,pRetVal)	\
    (This)->lpVtbl -> get_lSecurePort(This,pRetVal)

#define IBrokConfig_put_lSecurePort(This,NewVal)	\
    (This)->lpVtbl -> put_lSecurePort(This,NewVal)

#define IBrokConfig_get_lTimeLimit(This,pRetVal)	\
    (This)->lpVtbl -> get_lTimeLimit(This,pRetVal)

#define IBrokConfig_put_lTimeLimit(This,NewVal)	\
    (This)->lpVtbl -> put_lTimeLimit(This,NewVal)

#define IBrokConfig_get_lSizeLimit(This,pRetVal)	\
    (This)->lpVtbl -> get_lSizeLimit(This,pRetVal)

#define IBrokConfig_put_lSizeLimit(This,NewVal)	\
    (This)->lpVtbl -> put_lSizeLimit(This,NewVal)

#define IBrokConfig_get_bszBaseDN(This,pRetVal)	\
    (This)->lpVtbl -> get_bszBaseDN(This,pRetVal)

#define IBrokConfig_put_bszBaseDN(This,NewVal)	\
    (This)->lpVtbl -> put_bszBaseDN(This,NewVal)

#define IBrokConfig_get_lCacheTimeout(This,pRetVal)	\
    (This)->lpVtbl -> get_lCacheTimeout(This,pRetVal)

#define IBrokConfig_put_lCacheTimeout(This,NewVal)	\
    (This)->lpVtbl -> put_lCacheTimeout(This,NewVal)

#define IBrokConfig_get_bszGroupPrefix(This,pRetVal)	\
    (This)->lpVtbl -> get_bszGroupPrefix(This,pRetVal)

#define IBrokConfig_put_bszGroupPrefix(This,NewVal)	\
    (This)->lpVtbl -> put_bszGroupPrefix(This,NewVal)

#define IBrokConfig_get_bCreateGroups(This,pRetVal)	\
    (This)->lpVtbl -> get_bCreateGroups(This,pRetVal)

#define IBrokConfig_put_bCreateGroups(This,NewVal)	\
    (This)->lpVtbl -> put_bCreateGroups(This,NewVal)

#define IBrokConfig_get_bszDomain(This,pRetVal)	\
    (This)->lpVtbl -> get_bszDomain(This,pRetVal)

#define IBrokConfig_put_bszDomain(This,NewVal)	\
    (This)->lpVtbl -> put_bszDomain(This,NewVal)

#define IBrokConfig_get_bszDsName(This,pRetVal)	\
    (This)->lpVtbl -> get_bszDsName(This,pRetVal)

#define IBrokConfig_put_bszDsName(This,NewVal)	\
    (This)->lpVtbl -> put_bszDsName(This,NewVal)

#define IBrokConfig_get_bszDsPwd(This,pRetVal)	\
    (This)->lpVtbl -> get_bszDsPwd(This,pRetVal)

#define IBrokConfig_put_bszDsPwd(This,NewVal)	\
    (This)->lpVtbl -> put_bszDsPwd(This,NewVal)

#define IBrokConfig_get_bszProxyName(This,pRetVal)	\
    (This)->lpVtbl -> get_bszProxyName(This,pRetVal)

#define IBrokConfig_put_bszProxyName(This,NewVal)	\
    (This)->lpVtbl -> put_bszProxyName(This,NewVal)

#define IBrokConfig_get_bszProxyPwd(This,pRetVal)	\
    (This)->lpVtbl -> get_bszProxyPwd(This,pRetVal)

#define IBrokConfig_put_bszProxyPwd(This,NewVal)	\
    (This)->lpVtbl -> put_bszProxyPwd(This,NewVal)

#define IBrokConfig_get_bszProxyDomain(This,pRetVal)	\
    (This)->lpVtbl -> get_bszProxyDomain(This,pRetVal)

#define IBrokConfig_put_bszProxyDomain(This,NewVal)	\
    (This)->lpVtbl -> put_bszProxyDomain(This,NewVal)

#define IBrokConfig_get_bUseTrackCookie(This,pRetVal)	\
    (This)->lpVtbl -> get_bUseTrackCookie(This,pRetVal)

#define IBrokConfig_put_bUseTrackCookie(This,NewVal)	\
    (This)->lpVtbl -> put_bUseTrackCookie(This,NewVal)

#define IBrokConfig_get_lPwdCookieTimeout(This,pRetVal)	\
    (This)->lpVtbl -> get_lPwdCookieTimeout(This,pRetVal)

#define IBrokConfig_put_lPwdCookieTimeout(This,NewVal)	\
    (This)->lpVtbl -> put_lPwdCookieTimeout(This,NewVal)

#define IBrokConfig_get_bEnabled(This,pRetVal)	\
    (This)->lpVtbl -> get_bEnabled(This,pRetVal)

#define IBrokConfig_put_bEnabled(This,NewVal)	\
    (This)->lpVtbl -> put_bEnabled(This,NewVal)

#define IBrokConfig_get_bszComment(This,pRetVal)	\
    (This)->lpVtbl -> get_bszComment(This,pRetVal)

#define IBrokConfig_put_bszComment(This,NewVal)	\
    (This)->lpVtbl -> put_bszComment(This,NewVal)

#define IBrokConfig_get_bPwdCookiePersist(This,pRetVal)	\
    (This)->lpVtbl -> get_bPwdCookiePersist(This,pRetVal)

#define IBrokConfig_put_bPwdCookiePersist(This,NewVal)	\
    (This)->lpVtbl -> put_bPwdCookiePersist(This,NewVal)

#define IBrokConfig_get_bszRealm(This,pRetVal)	\
    (This)->lpVtbl -> get_bszRealm(This,pRetVal)

#define IBrokConfig_put_bszRealm(This,NewVal)	\
    (This)->lpVtbl -> put_bszRealm(This,NewVal)

#define IBrokConfig_get_lTokenCacheTimeout(This,pRetVal)	\
    (This)->lpVtbl -> get_lTokenCacheTimeout(This,pRetVal)

#define IBrokConfig_put_lTokenCacheTimeout(This,NewVal)	\
    (This)->lpVtbl -> put_lTokenCacheTimeout(This,NewVal)

#define IBrokConfig_get_bszTokenCreatorDll(This,pRetVal)	\
    (This)->lpVtbl -> get_bszTokenCreatorDll(This,pRetVal)

#define IBrokConfig_put_bszTokenCreatorDll(This,NewVal)	\
    (This)->lpVtbl -> put_bszTokenCreatorDll(This,NewVal)

#define IBrokConfig_get_bDirty(This,pRetVal)	\
    (This)->lpVtbl -> get_bDirty(This,pRetVal)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [hidden] */ HRESULT STDMETHODCALLTYPE IBrokConfig_LoadDefaults_Proxy( 
    IBrokConfig __RPC_FAR * This,
    LONG lVirtServId);


void __RPC_STUB IBrokConfig_LoadDefaults_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IBrokConfig_GetConfig_Proxy( 
    IBrokConfig __RPC_FAR * This,
    LONG lVirtServId);


void __RPC_STUB IBrokConfig_GetConfig_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IBrokConfig_SetConfig_Proxy( 
    IBrokConfig __RPC_FAR * This);


void __RPC_STUB IBrokConfig_SetConfig_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IBrokConfig_CheckAcct_Proxy( 
    IBrokConfig __RPC_FAR * This,
    BSTR bszDomain,
    BSTR bszName,
    BSTR bszPassword);


void __RPC_STUB IBrokConfig_CheckAcct_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propget] */ HRESULT STDMETHODCALLTYPE IBrokConfig_get_lVirtServId_Proxy( 
    IBrokConfig __RPC_FAR * This,
    /* [retval][out] */ LONG __RPC_FAR *pRetVal);


void __RPC_STUB IBrokConfig_get_lVirtServId_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propput] */ HRESULT STDMETHODCALLTYPE IBrokConfig_put_lVirtServId_Proxy( 
    IBrokConfig __RPC_FAR * This,
    /* [in] */ LONG NewVal);


void __RPC_STUB IBrokConfig_put_lVirtServId_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propget] */ HRESULT STDMETHODCALLTYPE IBrokConfig_get_bLocal_Proxy( 
    IBrokConfig __RPC_FAR * This,
    /* [retval][out] */ BOOL __RPC_FAR *pRetVal);


void __RPC_STUB IBrokConfig_get_bLocal_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propput] */ HRESULT STDMETHODCALLTYPE IBrokConfig_put_bLocal_Proxy( 
    IBrokConfig __RPC_FAR * This,
    /* [in] */ BOOL NewVal);


void __RPC_STUB IBrokConfig_put_bLocal_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propget] */ HRESULT STDMETHODCALLTYPE IBrokConfig_get_bszServerName_Proxy( 
    IBrokConfig __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pRetVal);


void __RPC_STUB IBrokConfig_get_bszServerName_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propput] */ HRESULT STDMETHODCALLTYPE IBrokConfig_put_bszServerName_Proxy( 
    IBrokConfig __RPC_FAR * This,
    /* [in] */ BSTR NewVal);


void __RPC_STUB IBrokConfig_put_bszServerName_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propget] */ HRESULT STDMETHODCALLTYPE IBrokConfig_get_lPort_Proxy( 
    IBrokConfig __RPC_FAR * This,
    /* [retval][out] */ LONG __RPC_FAR *pRetVal);


void __RPC_STUB IBrokConfig_get_lPort_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propput] */ HRESULT STDMETHODCALLTYPE IBrokConfig_put_lPort_Proxy( 
    IBrokConfig __RPC_FAR * This,
    /* [in] */ LONG NewVal);


void __RPC_STUB IBrokConfig_put_lPort_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propget] */ HRESULT STDMETHODCALLTYPE IBrokConfig_get_bSecure_Proxy( 
    IBrokConfig __RPC_FAR * This,
    /* [retval][out] */ BOOL __RPC_FAR *pRetVal);


void __RPC_STUB IBrokConfig_get_bSecure_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propput] */ HRESULT STDMETHODCALLTYPE IBrokConfig_put_bSecure_Proxy( 
    IBrokConfig __RPC_FAR * This,
    /* [in] */ BOOL NewVal);


void __RPC_STUB IBrokConfig_put_bSecure_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propget] */ HRESULT STDMETHODCALLTYPE IBrokConfig_get_lSecurePort_Proxy( 
    IBrokConfig __RPC_FAR * This,
    /* [retval][out] */ LONG __RPC_FAR *pRetVal);


void __RPC_STUB IBrokConfig_get_lSecurePort_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propput] */ HRESULT STDMETHODCALLTYPE IBrokConfig_put_lSecurePort_Proxy( 
    IBrokConfig __RPC_FAR * This,
    /* [in] */ LONG NewVal);


void __RPC_STUB IBrokConfig_put_lSecurePort_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propget] */ HRESULT STDMETHODCALLTYPE IBrokConfig_get_lTimeLimit_Proxy( 
    IBrokConfig __RPC_FAR * This,
    /* [retval][out] */ LONG __RPC_FAR *pRetVal);


void __RPC_STUB IBrokConfig_get_lTimeLimit_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propput] */ HRESULT STDMETHODCALLTYPE IBrokConfig_put_lTimeLimit_Proxy( 
    IBrokConfig __RPC_FAR * This,
    /* [in] */ LONG NewVal);


void __RPC_STUB IBrokConfig_put_lTimeLimit_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propget] */ HRESULT STDMETHODCALLTYPE IBrokConfig_get_lSizeLimit_Proxy( 
    IBrokConfig __RPC_FAR * This,
    /* [retval][out] */ LONG __RPC_FAR *pRetVal);


void __RPC_STUB IBrokConfig_get_lSizeLimit_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propput] */ HRESULT STDMETHODCALLTYPE IBrokConfig_put_lSizeLimit_Proxy( 
    IBrokConfig __RPC_FAR * This,
    /* [in] */ LONG NewVal);


void __RPC_STUB IBrokConfig_put_lSizeLimit_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propget] */ HRESULT STDMETHODCALLTYPE IBrokConfig_get_bszBaseDN_Proxy( 
    IBrokConfig __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pRetVal);


void __RPC_STUB IBrokConfig_get_bszBaseDN_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propput] */ HRESULT STDMETHODCALLTYPE IBrokConfig_put_bszBaseDN_Proxy( 
    IBrokConfig __RPC_FAR * This,
    /* [in] */ BSTR NewVal);


void __RPC_STUB IBrokConfig_put_bszBaseDN_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propget] */ HRESULT STDMETHODCALLTYPE IBrokConfig_get_lCacheTimeout_Proxy( 
    IBrokConfig __RPC_FAR * This,
    /* [retval][out] */ LONG __RPC_FAR *pRetVal);


void __RPC_STUB IBrokConfig_get_lCacheTimeout_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propput] */ HRESULT STDMETHODCALLTYPE IBrokConfig_put_lCacheTimeout_Proxy( 
    IBrokConfig __RPC_FAR * This,
    /* [in] */ LONG NewVal);


void __RPC_STUB IBrokConfig_put_lCacheTimeout_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propget] */ HRESULT STDMETHODCALLTYPE IBrokConfig_get_bszGroupPrefix_Proxy( 
    IBrokConfig __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pRetVal);


void __RPC_STUB IBrokConfig_get_bszGroupPrefix_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propput] */ HRESULT STDMETHODCALLTYPE IBrokConfig_put_bszGroupPrefix_Proxy( 
    IBrokConfig __RPC_FAR * This,
    /* [in] */ BSTR NewVal);


void __RPC_STUB IBrokConfig_put_bszGroupPrefix_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propget] */ HRESULT STDMETHODCALLTYPE IBrokConfig_get_bCreateGroups_Proxy( 
    IBrokConfig __RPC_FAR * This,
    /* [retval][out] */ BOOL __RPC_FAR *pRetVal);


void __RPC_STUB IBrokConfig_get_bCreateGroups_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propput] */ HRESULT STDMETHODCALLTYPE IBrokConfig_put_bCreateGroups_Proxy( 
    IBrokConfig __RPC_FAR * This,
    /* [in] */ BOOL NewVal);


void __RPC_STUB IBrokConfig_put_bCreateGroups_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propget] */ HRESULT STDMETHODCALLTYPE IBrokConfig_get_bszDomain_Proxy( 
    IBrokConfig __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pRetVal);


void __RPC_STUB IBrokConfig_get_bszDomain_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propput] */ HRESULT STDMETHODCALLTYPE IBrokConfig_put_bszDomain_Proxy( 
    IBrokConfig __RPC_FAR * This,
    /* [in] */ BSTR NewVal);


void __RPC_STUB IBrokConfig_put_bszDomain_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propget] */ HRESULT STDMETHODCALLTYPE IBrokConfig_get_bszDsName_Proxy( 
    IBrokConfig __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pRetVal);


void __RPC_STUB IBrokConfig_get_bszDsName_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propput] */ HRESULT STDMETHODCALLTYPE IBrokConfig_put_bszDsName_Proxy( 
    IBrokConfig __RPC_FAR * This,
    /* [in] */ BSTR NewVal);


void __RPC_STUB IBrokConfig_put_bszDsName_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propget] */ HRESULT STDMETHODCALLTYPE IBrokConfig_get_bszDsPwd_Proxy( 
    IBrokConfig __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pRetVal);


void __RPC_STUB IBrokConfig_get_bszDsPwd_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propput] */ HRESULT STDMETHODCALLTYPE IBrokConfig_put_bszDsPwd_Proxy( 
    IBrokConfig __RPC_FAR * This,
    /* [in] */ BSTR NewVal);


void __RPC_STUB IBrokConfig_put_bszDsPwd_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propget] */ HRESULT STDMETHODCALLTYPE IBrokConfig_get_bszProxyName_Proxy( 
    IBrokConfig __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pRetVal);


void __RPC_STUB IBrokConfig_get_bszProxyName_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propput] */ HRESULT STDMETHODCALLTYPE IBrokConfig_put_bszProxyName_Proxy( 
    IBrokConfig __RPC_FAR * This,
    /* [in] */ BSTR NewVal);


void __RPC_STUB IBrokConfig_put_bszProxyName_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propget] */ HRESULT STDMETHODCALLTYPE IBrokConfig_get_bszProxyPwd_Proxy( 
    IBrokConfig __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pRetVal);


void __RPC_STUB IBrokConfig_get_bszProxyPwd_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propput] */ HRESULT STDMETHODCALLTYPE IBrokConfig_put_bszProxyPwd_Proxy( 
    IBrokConfig __RPC_FAR * This,
    /* [in] */ BSTR NewVal);


void __RPC_STUB IBrokConfig_put_bszProxyPwd_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propget] */ HRESULT STDMETHODCALLTYPE IBrokConfig_get_bszProxyDomain_Proxy( 
    IBrokConfig __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pRetVal);


void __RPC_STUB IBrokConfig_get_bszProxyDomain_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propput] */ HRESULT STDMETHODCALLTYPE IBrokConfig_put_bszProxyDomain_Proxy( 
    IBrokConfig __RPC_FAR * This,
    /* [in] */ BSTR NewVal);


void __RPC_STUB IBrokConfig_put_bszProxyDomain_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propget] */ HRESULT STDMETHODCALLTYPE IBrokConfig_get_bUseTrackCookie_Proxy( 
    IBrokConfig __RPC_FAR * This,
    /* [retval][out] */ BOOL __RPC_FAR *pRetVal);


void __RPC_STUB IBrokConfig_get_bUseTrackCookie_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propput] */ HRESULT STDMETHODCALLTYPE IBrokConfig_put_bUseTrackCookie_Proxy( 
    IBrokConfig __RPC_FAR * This,
    /* [in] */ BOOL NewVal);


void __RPC_STUB IBrokConfig_put_bUseTrackCookie_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propget] */ HRESULT STDMETHODCALLTYPE IBrokConfig_get_lPwdCookieTimeout_Proxy( 
    IBrokConfig __RPC_FAR * This,
    /* [retval][out] */ LONG __RPC_FAR *pRetVal);


void __RPC_STUB IBrokConfig_get_lPwdCookieTimeout_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propput] */ HRESULT STDMETHODCALLTYPE IBrokConfig_put_lPwdCookieTimeout_Proxy( 
    IBrokConfig __RPC_FAR * This,
    /* [in] */ LONG NewVal);


void __RPC_STUB IBrokConfig_put_lPwdCookieTimeout_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propget] */ HRESULT STDMETHODCALLTYPE IBrokConfig_get_bEnabled_Proxy( 
    IBrokConfig __RPC_FAR * This,
    /* [retval][out] */ BOOL __RPC_FAR *pRetVal);


void __RPC_STUB IBrokConfig_get_bEnabled_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propput] */ HRESULT STDMETHODCALLTYPE IBrokConfig_put_bEnabled_Proxy( 
    IBrokConfig __RPC_FAR * This,
    /* [in] */ BOOL NewVal);


void __RPC_STUB IBrokConfig_put_bEnabled_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propget] */ HRESULT STDMETHODCALLTYPE IBrokConfig_get_bszComment_Proxy( 
    IBrokConfig __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pRetVal);


void __RPC_STUB IBrokConfig_get_bszComment_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propput] */ HRESULT STDMETHODCALLTYPE IBrokConfig_put_bszComment_Proxy( 
    IBrokConfig __RPC_FAR * This,
    /* [in] */ BSTR NewVal);


void __RPC_STUB IBrokConfig_put_bszComment_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propget] */ HRESULT STDMETHODCALLTYPE IBrokConfig_get_bPwdCookiePersist_Proxy( 
    IBrokConfig __RPC_FAR * This,
    /* [retval][out] */ BOOL __RPC_FAR *pRetVal);


void __RPC_STUB IBrokConfig_get_bPwdCookiePersist_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propput] */ HRESULT STDMETHODCALLTYPE IBrokConfig_put_bPwdCookiePersist_Proxy( 
    IBrokConfig __RPC_FAR * This,
    /* [in] */ BOOL NewVal);


void __RPC_STUB IBrokConfig_put_bPwdCookiePersist_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propget] */ HRESULT STDMETHODCALLTYPE IBrokConfig_get_bszRealm_Proxy( 
    IBrokConfig __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pRetVal);


void __RPC_STUB IBrokConfig_get_bszRealm_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propput] */ HRESULT STDMETHODCALLTYPE IBrokConfig_put_bszRealm_Proxy( 
    IBrokConfig __RPC_FAR * This,
    /* [in] */ BSTR NewVal);


void __RPC_STUB IBrokConfig_put_bszRealm_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propget] */ HRESULT STDMETHODCALLTYPE IBrokConfig_get_lTokenCacheTimeout_Proxy( 
    IBrokConfig __RPC_FAR * This,
    /* [retval][out] */ LONG __RPC_FAR *pRetVal);


void __RPC_STUB IBrokConfig_get_lTokenCacheTimeout_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propput] */ HRESULT STDMETHODCALLTYPE IBrokConfig_put_lTokenCacheTimeout_Proxy( 
    IBrokConfig __RPC_FAR * This,
    /* [in] */ LONG NewVal);


void __RPC_STUB IBrokConfig_put_lTokenCacheTimeout_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propget] */ HRESULT STDMETHODCALLTYPE IBrokConfig_get_bszTokenCreatorDll_Proxy( 
    IBrokConfig __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pRetVal);


void __RPC_STUB IBrokConfig_get_bszTokenCreatorDll_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propput] */ HRESULT STDMETHODCALLTYPE IBrokConfig_put_bszTokenCreatorDll_Proxy( 
    IBrokConfig __RPC_FAR * This,
    /* [in] */ BSTR NewVal);


void __RPC_STUB IBrokConfig_put_bszTokenCreatorDll_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propget] */ HRESULT STDMETHODCALLTYPE IBrokConfig_get_bDirty_Proxy( 
    IBrokConfig __RPC_FAR * This,
    /* [retval][out] */ BOOL __RPC_FAR *pRetVal);


void __RPC_STUB IBrokConfig_get_bDirty_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IBrokConfig_INTERFACE_DEFINED__ */


#ifndef __IBrokServers_INTERFACE_DEFINED__
#define __IBrokServers_INTERFACE_DEFINED__

/****************************************
 * Generated header for interface: IBrokServers
 * at Wed Apr 22 19:48:30 1998
 * using MIDL 3.01.75
 ****************************************/
/* [unique][helpstring][dual][uuid][object] */ 



EXTERN_C const IID IID_IBrokServers;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    interface DECLSPEC_UUID("099226a0-c4a7-11d0-bbda-00c04fb615e5")
    IBrokServers : public IDispatch
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE Init( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE HasWritePrivilege( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE HasNTAdminPrivilege( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE CreateServer( 
            /* [out] */ VARIANT __RPC_FAR *plVirtServId) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE DeleteServer( 
            LONG lVirtServId) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE StartServer( 
            LONG lVirtServId) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE StopServer( 
            LONG lVirtServId) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetServers( 
            /* [out][in] */ VARIANT __RPC_FAR *plVirtServIds,
            /* [out][in] */ VARIANT __RPC_FAR *pComments) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE MapToBroker( 
            BSTR bszServiceName,
            LONG lVirtServId,
            LONG lVirtBrokId) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE ClearMapping( 
            BSTR bszServiceName,
            LONG lVirtServId) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE MappedTo( 
            BSTR bszServiceName,
            LONG lVirtServId,
            /* [out][in] */ VARIANT __RPC_FAR *plVirtBrokId,
            /* [out][in] */ VARIANT __RPC_FAR *pbszComment) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetAuthTypes( 
            BSTR bszPath,
            LONG lTypes) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetAuthTypes( 
            BSTR bszPath,
            /* [retval][out] */ LONG __RPC_FAR *plTypes) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetSecurityMode( 
            BSTR bszServiceName,
            LONG lVirtServId,
            /* [retval][out] */ BOOL __RPC_FAR *pbNTSecurity) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IBrokServersVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IBrokServers __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IBrokServers __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IBrokServers __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            IBrokServers __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            IBrokServers __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            IBrokServers __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            IBrokServers __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Init )( 
            IBrokServers __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *HasWritePrivilege )( 
            IBrokServers __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *HasNTAdminPrivilege )( 
            IBrokServers __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *CreateServer )( 
            IBrokServers __RPC_FAR * This,
            /* [out] */ VARIANT __RPC_FAR *plVirtServId);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *DeleteServer )( 
            IBrokServers __RPC_FAR * This,
            LONG lVirtServId);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *StartServer )( 
            IBrokServers __RPC_FAR * This,
            LONG lVirtServId);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *StopServer )( 
            IBrokServers __RPC_FAR * This,
            LONG lVirtServId);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetServers )( 
            IBrokServers __RPC_FAR * This,
            /* [out][in] */ VARIANT __RPC_FAR *plVirtServIds,
            /* [out][in] */ VARIANT __RPC_FAR *pComments);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *MapToBroker )( 
            IBrokServers __RPC_FAR * This,
            BSTR bszServiceName,
            LONG lVirtServId,
            LONG lVirtBrokId);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *ClearMapping )( 
            IBrokServers __RPC_FAR * This,
            BSTR bszServiceName,
            LONG lVirtServId);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *MappedTo )( 
            IBrokServers __RPC_FAR * This,
            BSTR bszServiceName,
            LONG lVirtServId,
            /* [out][in] */ VARIANT __RPC_FAR *plVirtBrokId,
            /* [out][in] */ VARIANT __RPC_FAR *pbszComment);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *SetAuthTypes )( 
            IBrokServers __RPC_FAR * This,
            BSTR bszPath,
            LONG lTypes);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetAuthTypes )( 
            IBrokServers __RPC_FAR * This,
            BSTR bszPath,
            /* [retval][out] */ LONG __RPC_FAR *plTypes);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetSecurityMode )( 
            IBrokServers __RPC_FAR * This,
            BSTR bszServiceName,
            LONG lVirtServId,
            /* [retval][out] */ BOOL __RPC_FAR *pbNTSecurity);
        
        END_INTERFACE
    } IBrokServersVtbl;

    interface IBrokServers
    {
        CONST_VTBL struct IBrokServersVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IBrokServers_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IBrokServers_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IBrokServers_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IBrokServers_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IBrokServers_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IBrokServers_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IBrokServers_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IBrokServers_Init(This)	\
    (This)->lpVtbl -> Init(This)

#define IBrokServers_HasWritePrivilege(This)	\
    (This)->lpVtbl -> HasWritePrivilege(This)

#define IBrokServers_HasNTAdminPrivilege(This)	\
    (This)->lpVtbl -> HasNTAdminPrivilege(This)

#define IBrokServers_CreateServer(This,plVirtServId)	\
    (This)->lpVtbl -> CreateServer(This,plVirtServId)

#define IBrokServers_DeleteServer(This,lVirtServId)	\
    (This)->lpVtbl -> DeleteServer(This,lVirtServId)

#define IBrokServers_StartServer(This,lVirtServId)	\
    (This)->lpVtbl -> StartServer(This,lVirtServId)

#define IBrokServers_StopServer(This,lVirtServId)	\
    (This)->lpVtbl -> StopServer(This,lVirtServId)

#define IBrokServers_GetServers(This,plVirtServIds,pComments)	\
    (This)->lpVtbl -> GetServers(This,plVirtServIds,pComments)

#define IBrokServers_MapToBroker(This,bszServiceName,lVirtServId,lVirtBrokId)	\
    (This)->lpVtbl -> MapToBroker(This,bszServiceName,lVirtServId,lVirtBrokId)

#define IBrokServers_ClearMapping(This,bszServiceName,lVirtServId)	\
    (This)->lpVtbl -> ClearMapping(This,bszServiceName,lVirtServId)

#define IBrokServers_MappedTo(This,bszServiceName,lVirtServId,plVirtBrokId,pbszComment)	\
    (This)->lpVtbl -> MappedTo(This,bszServiceName,lVirtServId,plVirtBrokId,pbszComment)

#define IBrokServers_SetAuthTypes(This,bszPath,lTypes)	\
    (This)->lpVtbl -> SetAuthTypes(This,bszPath,lTypes)

#define IBrokServers_GetAuthTypes(This,bszPath,plTypes)	\
    (This)->lpVtbl -> GetAuthTypes(This,bszPath,plTypes)

#define IBrokServers_GetSecurityMode(This,bszServiceName,lVirtServId,pbNTSecurity)	\
    (This)->lpVtbl -> GetSecurityMode(This,bszServiceName,lVirtServId,pbNTSecurity)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IBrokServers_Init_Proxy( 
    IBrokServers __RPC_FAR * This);


void __RPC_STUB IBrokServers_Init_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IBrokServers_HasWritePrivilege_Proxy( 
    IBrokServers __RPC_FAR * This);


void __RPC_STUB IBrokServers_HasWritePrivilege_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IBrokServers_HasNTAdminPrivilege_Proxy( 
    IBrokServers __RPC_FAR * This);


void __RPC_STUB IBrokServers_HasNTAdminPrivilege_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IBrokServers_CreateServer_Proxy( 
    IBrokServers __RPC_FAR * This,
    /* [out] */ VARIANT __RPC_FAR *plVirtServId);


void __RPC_STUB IBrokServers_CreateServer_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IBrokServers_DeleteServer_Proxy( 
    IBrokServers __RPC_FAR * This,
    LONG lVirtServId);


void __RPC_STUB IBrokServers_DeleteServer_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IBrokServers_StartServer_Proxy( 
    IBrokServers __RPC_FAR * This,
    LONG lVirtServId);


void __RPC_STUB IBrokServers_StartServer_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IBrokServers_StopServer_Proxy( 
    IBrokServers __RPC_FAR * This,
    LONG lVirtServId);


void __RPC_STUB IBrokServers_StopServer_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IBrokServers_GetServers_Proxy( 
    IBrokServers __RPC_FAR * This,
    /* [out][in] */ VARIANT __RPC_FAR *plVirtServIds,
    /* [out][in] */ VARIANT __RPC_FAR *pComments);


void __RPC_STUB IBrokServers_GetServers_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IBrokServers_MapToBroker_Proxy( 
    IBrokServers __RPC_FAR * This,
    BSTR bszServiceName,
    LONG lVirtServId,
    LONG lVirtBrokId);


void __RPC_STUB IBrokServers_MapToBroker_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IBrokServers_ClearMapping_Proxy( 
    IBrokServers __RPC_FAR * This,
    BSTR bszServiceName,
    LONG lVirtServId);


void __RPC_STUB IBrokServers_ClearMapping_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IBrokServers_MappedTo_Proxy( 
    IBrokServers __RPC_FAR * This,
    BSTR bszServiceName,
    LONG lVirtServId,
    /* [out][in] */ VARIANT __RPC_FAR *plVirtBrokId,
    /* [out][in] */ VARIANT __RPC_FAR *pbszComment);


void __RPC_STUB IBrokServers_MappedTo_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IBrokServers_SetAuthTypes_Proxy( 
    IBrokServers __RPC_FAR * This,
    BSTR bszPath,
    LONG lTypes);


void __RPC_STUB IBrokServers_SetAuthTypes_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IBrokServers_GetAuthTypes_Proxy( 
    IBrokServers __RPC_FAR * This,
    BSTR bszPath,
    /* [retval][out] */ LONG __RPC_FAR *plTypes);


void __RPC_STUB IBrokServers_GetAuthTypes_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IBrokServers_GetSecurityMode_Proxy( 
    IBrokServers __RPC_FAR * This,
    BSTR bszServiceName,
    LONG lVirtServId,
    /* [retval][out] */ BOOL __RPC_FAR *pbNTSecurity);


void __RPC_STUB IBrokServers_GetSecurityMode_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IBrokServers_INTERFACE_DEFINED__ */



#ifndef __BrokConfigLib_LIBRARY_DEFINED__
#define __BrokConfigLib_LIBRARY_DEFINED__

/****************************************
 * Generated header for library: BrokConfigLib
 * at Wed Apr 22 19:48:30 1998
 * using MIDL 3.01.75
 ****************************************/
/* [helpstring][version][uuid] */ 



EXTERN_C const IID LIBID_BrokConfigLib;

#ifdef __cplusplus
EXTERN_C const CLSID CLSID_BrokConfig;

class DECLSPEC_UUID("c78fa6e6-c4ac-11d0-bbda-00c04fb615e5")
BrokConfig;
#endif

#ifdef __cplusplus
EXTERN_C const CLSID CLSID_BrokServers;

class DECLSPEC_UUID("cd8f114e-c4ac-11d0-bbda-00c04fb615e5")
BrokServers;
#endif
#endif /* __BrokConfigLib_LIBRARY_DEFINED__ */

/****************************************
 * Generated header for interface: __MIDL_itf_brokcfg_0137
 * at Wed Apr 22 19:48:30 1998
 * using MIDL 3.01.75
 ****************************************/
/* [local] */ 


DEFINE_GUID(LIBID_BrokConfigLib, 0xa0341532,0xc4ac,0x11d0,0xbb,0xda,0x00,0xc0,0x4f,0xb6,0x15,0xe5);
DEFINE_GUID(IID_IBrokConfig, 0x19edab12,0xc4a4,0x11d0,0xbb,0xda,0x00,0xc0,0x4f,0xb6,0x15,0xe5);
DEFINE_GUID(IID_IBrokServers, 0x099226a0,0xc4a7,0x11d0,0xbb,0xda,0x00,0xc0,0x4f,0xb6,0x15,0xe5);
DEFINE_GUID(CLSID_BrokConfig, 0xc78fa6e6,0xc4ac,0x11d0,0xbb,0xda,0x00,0xc0,0x4f,0xb6,0x15,0xe5);
DEFINE_GUID(CLSID_BrokServers, 0xcd8f114e,0xc4ac,0x11d0,0xbb,0xda,0x00,0xc0,0x4f,0xb6,0x15,0xe5);


extern RPC_IF_HANDLE __MIDL_itf_brokcfg_0137_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_brokcfg_0137_v0_0_s_ifspec;

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
