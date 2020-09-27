/* this ALWAYS GENERATED file contains the definitions for the interfaces */


/* File created by MIDL compiler version 5.01.0158 */
/* at Thu Jun 25 16:10:02 1998
 */
/* Compiler settings for dbgoledb.idl:
    Os (OptLev=s), W1, Zp8, env=Win32, ms_ext, c_ext
    error checks: allocation ref bounds_check enum stub_data 
*/
//@@MIDL_FILE_HEADING(  )


/* verify that the <rpcndr.h> version is high enough to compile this file*/
#ifndef __REQUIRED_RPCNDR_H_VERSION__
#define __REQUIRED_RPCNDR_H_VERSION__ 440
#endif

#include "rpc.h"
#include "rpcndr.h"

#ifndef __dbgoledb_h__
#define __dbgoledb_h__

#ifdef __cplusplus
extern "C"{
#endif 

/* Forward Declarations */ 

#ifndef __DbgOleDB_FWD_DEFINED__
#define __DbgOleDB_FWD_DEFINED__

#ifdef __cplusplus
typedef class DbgOleDB DbgOleDB;
#else
typedef struct DbgOleDB DbgOleDB;
#endif /* __cplusplus */

#endif 	/* __DbgOleDB_FWD_DEFINED__ */


/* header files for imported files */
#include "oaidl.h"
#include "ocidl.h"

void __RPC_FAR * __RPC_USER MIDL_user_allocate(size_t);
void __RPC_USER MIDL_user_free( void __RPC_FAR * ); 

/* interface __MIDL_itf_dbgoledb_0000 */
/* [local] */ 

			/* size is 2 */

enum DBPROP_SESS
    {	DBPROP_INIT_EXE_TIMESTAMP	= 0x100L,
	DBPROP_INIT_SEARCHPATH	= DBPROP_INIT_EXE_TIMESTAMP + 1,
	DBPROP_SESS_LOADADDRESS	= DBPROP_INIT_SEARCHPATH + 1
    };
			/* size is 2 */

enum __MIDL___MIDL_itf_dbgoledb_0000_0001
    {	E_PDB_OK	= ( HRESULT  )(( unsigned long  )1 << 31 | ( unsigned long  )( LONG  )0x6d << 16 | ( unsigned long  )1),
	E_PDB_USAGE	= E_PDB_OK + 1,
	E_PDB_FILE_SYSTEM	= E_PDB_USAGE + 1,
	E_PDB_NOT_FOUND	= E_PDB_FILE_SYSTEM + 1,
	E_PDB_INVALID_SIG	= E_PDB_NOT_FOUND + 1,
	E_PDB_INVALID_AGE	= E_PDB_INVALID_SIG + 1,
	E_PDB_PRECOMP_REQUIRED	= E_PDB_INVALID_AGE + 1,
	E_PDB_OUT_OF_TI	= E_PDB_PRECOMP_REQUIRED + 1,
	E_PDB_V1_PDB	= E_PDB_OUT_OF_TI + 1,
	E_PDB_FORMAT	= E_PDB_V1_PDB + 1,
	E_PDB_LIMIT	= E_PDB_FORMAT + 1,
	E_PDB_CORRUPT	= E_PDB_LIMIT + 1,
	E_PDB_TI16	= E_PDB_CORRUPT + 1,
	E_PDB_ILLEGAL_TYPE_EDIT	= E_PDB_TI16 + 1,
	E_PDB_INVALID_EXECUTABLE	= E_PDB_ILLEGAL_TYPE_EDIT + 1,
	E_PDB_DBG_NOT_FOUND	= E_PDB_INVALID_EXECUTABLE + 1,
	E_PDB_NO_DEBUG_INFO	= E_PDB_DBG_NOT_FOUND + 1,
	E_PDB_INVALID_EXE_TIMESTAMP	= E_PDB_NO_DEBUG_INFO + 1,
	E_PDB_DEBUG_INFO_NOT_IN_PDB	= E_PDB_INVALID_EXE_TIMESTAMP + 1
    };
			/* size is 2 */

enum SymTagEnum
    {	SymTagNull	= 0,
	SymTagExe	= SymTagNull + 1,
	SymTagCompiland	= SymTagExe + 1,
	SymTagFunctionSymbol	= SymTagCompiland + 1,
	SymTagBlockBegin	= SymTagFunctionSymbol + 1,
	SymTagBlockEnd	= SymTagBlockBegin + 1,
	SymTagData	= SymTagBlockEnd + 1,
	SymTagConstant	= SymTagData + 1,
	SymTagLabel	= SymTagConstant + 1,
	SymTagPublicSymbol	= SymTagLabel + 1,
	SymTagUDT	= SymTagPublicSymbol + 1,
	SymTagEnum	= SymTagUDT + 1,
	SymTagFunctionType	= SymTagEnum + 1,
	SymTagPointerType	= SymTagFunctionType + 1,
	SymTagArrayType	= SymTagPointerType + 1,
	SymTagBaseType	= SymTagArrayType + 1,
	SymTagTypedef	= SymTagBaseType + 1,
	SymTagFunctionArgType	= SymTagTypedef + 1,
	SymTagFuncDebugStart	= SymTagFunctionArgType + 1,
	SymTagFuncDebugEnd	= SymTagFuncDebugStart + 1,
	SymTagCustom	= SymTagFuncDebugEnd + 1,
	SymTagMax	= SymTagCustom + 1
    };
			/* size is 2 */

enum LocationType
    {	LocIsStatic	= 0,
	LocIsTLS	= LocIsStatic + 1,
	LocIsRegRel	= LocIsTLS + 1,
	LocIsThisRel	= LocIsRegRel + 1,
	LocIsEnregistered	= LocIsThisRel + 1,
	LocIsBitField	= LocIsEnregistered + 1,
	LocTypeMax	= LocIsBitField + 1
    };
			/* size is 2 */

enum AccessSpecifier
    {	ASPublic	= 0,
	ASPrivate	= ASPublic + 1,
	ASProtect	= ASPrivate + 1
    };
			/* size is 2 */

enum FuncLanguageEnum
    {	FuncLanguageC	= 0,
	FuncLanguageCPlus	= FuncLanguageC + 1,
	FuncLanguageBasic	= FuncLanguageCPlus + 1,
	FuncLanguageFortran	= FuncLanguageBasic + 1,
	FuncLanguagePascal	= FuncLanguageFortran + 1,
	FuncLanguageJava	= FuncLanguagePascal + 1
    };
			/* size is 2 */

enum FuncThunkOrdinal
    {	FuncTONotAThunk	= 0,
	FuncTONoType	= FuncTONotAThunk + 1,
	FuncTOAdjustor	= FuncTONoType + 1,
	FuncTOVCall	= FuncTOAdjustor + 1,
	FuncTOPcode	= FuncTOVCall + 1,
	FuncTODelayLoad	= FuncTOPcode + 1
    };
			/* size is 2 */

enum BasicType
    {	btNoType	= 0,
	btVoid	= btNoType + 1,
	btChar	= btVoid + 1,
	btWChar	= btChar + 1,
	btSChar	= btWChar + 1,
	btUChar	= btSChar + 1,
	btInt	= btUChar + 1,
	btUInt	= btInt + 1,
	btFloat	= btUInt + 1,
	btBCD	= btFloat + 1,
	btBool	= btBCD + 1,
	btShort	= btBool + 1,
	btUShort	= btShort + 1,
	btLong	= btUShort + 1,
	btULong	= btLong + 1,
	btInt8	= btULong + 1,
	btInt16	= btInt8 + 1,
	btInt32	= btInt16 + 1,
	btInt64	= btInt32 + 1,
	btInt128	= btInt64 + 1,
	btUInt8	= btInt128 + 1,
	btUInt16	= btUInt8 + 1,
	btUInt32	= btUInt16 + 1,
	btUInt64	= btUInt32 + 1,
	btUInt128	= btUInt64 + 1,
	btCurrency	= btUInt128 + 1,
	btDate	= btCurrency + 1,
	btVariant	= btDate + 1,
	btComplex	= btVariant + 1,
	btBit	= btComplex + 1,
	btBSTR	= btBit + 1
    };
			/* size is 2 */

enum FunctionCallingConvention
    {	fccCdecl	= 0,
	fccStdcall	= fccCdecl + 1,
	fccFastcall	= fccStdcall + 1,
	fccThiscall	= fccFastcall + 1,
	fccPascal	= fccThiscall + 1,
	fccSyscall	= fccPascal + 1,
	fccCorcall	= fccSyscall + 1,
	fccMipsCall	= fccCorcall + 1,
	fccGeneric	= fccMipsCall + 1,
	fccAlpha	= fccGeneric + 1,
	fccPPC	= fccAlpha + 1,
	fccSHcall	= fccPPC + 1,
	fccArmcall	= fccSHcall + 1,
	fccUnknown	= fccArmcall + 1
    };


extern RPC_IF_HANDLE __MIDL_itf_dbgoledb_0000_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_dbgoledb_0000_v0_0_s_ifspec;


#ifndef __DBGOLEDBLib_LIBRARY_DEFINED__
#define __DBGOLEDBLib_LIBRARY_DEFINED__

/* library DBGOLEDBLib */
/* [helpstring][version][uuid] */ 


EXTERN_C const IID LIBID_DBGOLEDBLib;

EXTERN_C const CLSID CLSID_DbgOleDB;

#ifdef __cplusplus

class DECLSPEC_UUID("4072ED30-BEA4-11D1-AC7E-00A0C90D2884")
DbgOleDB;
#endif
#endif /* __DBGOLEDBLib_LIBRARY_DEFINED__ */

/* Additional Prototypes for ALL interfaces */

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif
