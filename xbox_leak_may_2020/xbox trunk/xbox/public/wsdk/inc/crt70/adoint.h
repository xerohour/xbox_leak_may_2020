//--------------------------------------------------------------------
// Microsoft ADO
//
// (c) 1996-1998 Microsoft Corporation.  All Rights Reserved.
//
// @doc
//
// @module	adoint.h | ADO Interface header
//
// @devnote None
//--------------------------------------------------------------------
#ifndef _ADOINT_H_
#define _ADOINT_H_

#ifndef _INC_TCHAR
#include <tchar.h>
#endif

#if _MSC_VER >= 1100
#define DECLSPEC_UUID(x)    __declspec(uuid(x))
#else
#define DECLSPEC_UUID(x)
#endif


#pragma warning( disable: 4049 )  /* more than 64k source lines */
/* this ALWAYS GENERATED file contains the definitions for the interfaces */
 /* File created by MIDL compiler version 5.03.0279 */
/* at Fri Apr 07 05:37:21 2000
 */
/* Compiler settings for m_bobj.odl:
    Os (OptLev=s), W4, Zp8, env=Win32 (32b run), ms_ext, c_ext
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
#ifndef __m_bobj_h__
#define __m_bobj_h__
/* Forward Declarations */ 
#ifndef ___ADOCollection_FWD_DEFINED__
#define ___ADOCollection_FWD_DEFINED__
typedef interface _ADOCollection _ADOCollection;
#endif 	/* ___ADOCollection_FWD_DEFINED__ */
#ifndef ___ADODynaCollection_FWD_DEFINED__
#define ___ADODynaCollection_FWD_DEFINED__
typedef interface _ADODynaCollection _ADODynaCollection;
#endif 	/* ___ADODynaCollection_FWD_DEFINED__ */
#ifndef ___ADO_FWD_DEFINED__
#define ___ADO_FWD_DEFINED__
typedef interface _ADO _ADO;
#endif 	/* ___ADO_FWD_DEFINED__ */
#ifndef __Error_FWD_DEFINED__
#define __Error_FWD_DEFINED__
typedef interface ADOError Error;
#endif 	/* __Error_FWD_DEFINED__ */
#ifndef __Errors_FWD_DEFINED__
#define __Errors_FWD_DEFINED__
typedef interface ADOErrors Errors;
#endif 	/* __Errors_FWD_DEFINED__ */
#ifndef __Command15_FWD_DEFINED__
#define __Command15_FWD_DEFINED__
typedef interface Command15 Command15;
#endif 	/* __Command15_FWD_DEFINED__ */
#ifndef __Command25_FWD_DEFINED__
#define __Command25_FWD_DEFINED__
typedef interface Command25 Command25;
#endif 	/* __Command25_FWD_DEFINED__ */
#ifndef ___Command_FWD_DEFINED__
#define ___Command_FWD_DEFINED__
typedef interface _ADOCommand _Command;
#endif 	/* ___Command_FWD_DEFINED__ */
#ifndef __ConnectionEventsVt_FWD_DEFINED__
#define __ConnectionEventsVt_FWD_DEFINED__
typedef interface ConnectionEventsVt ConnectionEventsVt;
#endif 	/* __ConnectionEventsVt_FWD_DEFINED__ */
#ifndef __RecordsetEventsVt_FWD_DEFINED__
#define __RecordsetEventsVt_FWD_DEFINED__
typedef interface RecordsetEventsVt RecordsetEventsVt;
#endif 	/* __RecordsetEventsVt_FWD_DEFINED__ */
#ifndef __ConnectionEvents_FWD_DEFINED__
#define __ConnectionEvents_FWD_DEFINED__
typedef interface ConnectionEvents ConnectionEvents;
#endif 	/* __ConnectionEvents_FWD_DEFINED__ */
#ifndef __RecordsetEvents_FWD_DEFINED__
#define __RecordsetEvents_FWD_DEFINED__
typedef interface RecordsetEvents RecordsetEvents;
#endif 	/* __RecordsetEvents_FWD_DEFINED__ */
#ifndef __Connection15_FWD_DEFINED__
#define __Connection15_FWD_DEFINED__
typedef interface Connection15 Connection15;
#endif 	/* __Connection15_FWD_DEFINED__ */
#ifndef ___Connection_FWD_DEFINED__
#define ___Connection_FWD_DEFINED__
typedef interface _ADOConnection _Connection;
#endif 	/* ___Connection_FWD_DEFINED__ */
#ifndef __ADOConnectionConstruction15_FWD_DEFINED__
#define __ADOConnectionConstruction15_FWD_DEFINED__
typedef interface ADOConnectionConstruction15 ADOConnectionConstruction15;
#endif 	/* __ADOConnectionConstruction15_FWD_DEFINED__ */
#ifndef __ADOConnectionConstruction_FWD_DEFINED__
#define __ADOConnectionConstruction_FWD_DEFINED__
typedef interface ADOConnectionConstruction ADOConnectionConstruction;
#endif 	/* __ADOConnectionConstruction_FWD_DEFINED__ */
#ifndef __Connection_FWD_DEFINED__
#define __Connection_FWD_DEFINED__
#ifdef __cplusplus
typedef class ADOConnection Connection;
#else
typedef struct ADOConnection Connection;
#endif /* __cplusplus */
#endif 	/* __Connection_FWD_DEFINED__ */
#ifndef ___Record_FWD_DEFINED__
#define ___Record_FWD_DEFINED__
typedef interface _ADORecord _Record;
#endif 	/* ___Record_FWD_DEFINED__ */
#ifndef __Record_FWD_DEFINED__
#define __Record_FWD_DEFINED__
#ifdef __cplusplus
typedef class ADORecord Record;
#else
typedef struct ADORecord Record;
#endif /* __cplusplus */
#endif 	/* __Record_FWD_DEFINED__ */
#ifndef ___Stream_FWD_DEFINED__
#define ___Stream_FWD_DEFINED__
typedef interface _ADOStream _Stream;
#endif 	/* ___Stream_FWD_DEFINED__ */
#ifndef __Stream_FWD_DEFINED__
#define __Stream_FWD_DEFINED__
#ifdef __cplusplus
typedef class ADOStream Stream;
#else
typedef struct ADOStream Stream;
#endif /* __cplusplus */
#endif 	/* __Stream_FWD_DEFINED__ */
#ifndef __ADORecordConstruction_FWD_DEFINED__
#define __ADORecordConstruction_FWD_DEFINED__
typedef interface ADORecordConstruction ADORecordConstruction;
#endif 	/* __ADORecordConstruction_FWD_DEFINED__ */
#ifndef __ADOStreamConstruction_FWD_DEFINED__
#define __ADOStreamConstruction_FWD_DEFINED__
typedef interface ADOStreamConstruction ADOStreamConstruction;
#endif 	/* __ADOStreamConstruction_FWD_DEFINED__ */
#ifndef __ADOCommandConstruction_FWD_DEFINED__
#define __ADOCommandConstruction_FWD_DEFINED__
typedef interface ADOCommandConstruction ADOCommandConstruction;
#endif 	/* __ADOCommandConstruction_FWD_DEFINED__ */
#ifndef __Command_FWD_DEFINED__
#define __Command_FWD_DEFINED__
#ifdef __cplusplus
typedef class ADOCommand Command;
#else
typedef struct ADOCommand Command;
#endif /* __cplusplus */
#endif 	/* __Command_FWD_DEFINED__ */
#ifndef __Recordset_FWD_DEFINED__
#define __Recordset_FWD_DEFINED__
#ifdef __cplusplus
typedef class ADORecordset Recordset;
#else
typedef struct ADORecordset Recordset;
#endif /* __cplusplus */
#endif 	/* __Recordset_FWD_DEFINED__ */
#ifndef __Recordset15_FWD_DEFINED__
#define __Recordset15_FWD_DEFINED__
typedef interface Recordset15 Recordset15;
#endif 	/* __Recordset15_FWD_DEFINED__ */
#ifndef __Recordset20_FWD_DEFINED__
#define __Recordset20_FWD_DEFINED__
typedef interface Recordset20 Recordset20;
#endif 	/* __Recordset20_FWD_DEFINED__ */
#ifndef __Recordset21_FWD_DEFINED__
#define __Recordset21_FWD_DEFINED__
typedef interface Recordset21 Recordset21;
#endif 	/* __Recordset21_FWD_DEFINED__ */
#ifndef ___Recordset_FWD_DEFINED__
#define ___Recordset_FWD_DEFINED__
typedef interface _ADORecordset _Recordset;
#endif 	/* ___Recordset_FWD_DEFINED__ */
#ifndef __ADORecordsetConstruction_FWD_DEFINED__
#define __ADORecordsetConstruction_FWD_DEFINED__
typedef interface ADORecordsetConstruction ADORecordsetConstruction;
#endif 	/* __ADORecordsetConstruction_FWD_DEFINED__ */
#ifndef __Field15_FWD_DEFINED__
#define __Field15_FWD_DEFINED__
typedef interface Field15 Field15;
#endif 	/* __Field15_FWD_DEFINED__ */
#ifndef __Field20_FWD_DEFINED__
#define __Field20_FWD_DEFINED__
typedef interface Field20 Field20;
#endif 	/* __Field20_FWD_DEFINED__ */
#ifndef __Field_FWD_DEFINED__
#define __Field_FWD_DEFINED__
typedef interface ADOField Field;
#endif 	/* __Field_FWD_DEFINED__ */
#ifndef __Fields15_FWD_DEFINED__
#define __Fields15_FWD_DEFINED__
typedef interface Fields15 Fields15;
#endif 	/* __Fields15_FWD_DEFINED__ */
#ifndef __Fields20_FWD_DEFINED__
#define __Fields20_FWD_DEFINED__
typedef interface Fields20 Fields20;
#endif 	/* __Fields20_FWD_DEFINED__ */
#ifndef __Fields_FWD_DEFINED__
#define __Fields_FWD_DEFINED__
typedef interface ADOFields Fields;
#endif 	/* __Fields_FWD_DEFINED__ */
#ifndef ___Parameter_FWD_DEFINED__
#define ___Parameter_FWD_DEFINED__
typedef interface _ADOParameter _Parameter;
#endif 	/* ___Parameter_FWD_DEFINED__ */
#ifndef __Parameter_FWD_DEFINED__
#define __Parameter_FWD_DEFINED__
#ifdef __cplusplus
typedef class ADOParameter Parameter;
#else
typedef struct ADOParameter Parameter;
#endif /* __cplusplus */
#endif 	/* __Parameter_FWD_DEFINED__ */
#ifndef __Parameters_FWD_DEFINED__
#define __Parameters_FWD_DEFINED__
typedef interface ADOParameters Parameters;
#endif 	/* __Parameters_FWD_DEFINED__ */
#ifndef __Property_FWD_DEFINED__
#define __Property_FWD_DEFINED__
typedef interface ADOProperty Property;
#endif 	/* __Property_FWD_DEFINED__ */
#ifndef __Properties_FWD_DEFINED__
#define __Properties_FWD_DEFINED__
typedef interface ADOProperties Properties;
#endif 	/* __Properties_FWD_DEFINED__ */
#ifdef __cplusplus
extern "C"{
#endif 
void __RPC_FAR * __RPC_USER MIDL_user_allocate(size_t);
void __RPC_USER MIDL_user_free( void __RPC_FAR * ); 
/* interface __MIDL_itf_m_bobj_0000 */
/* [local] */ 
#ifdef _WIN64
// Number of rows
typedef LONGLONG				ADO_DBLONGPTR;
#else
// Number of rows
typedef long ADO_DBLONGPTR;
#endif	// _WIN64
extern RPC_IF_HANDLE __MIDL_itf_m_bobj_0000_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_m_bobj_0000_v0_0_s_ifspec;
#ifndef __ADODB_LIBRARY_DEFINED__
#define __ADODB_LIBRARY_DEFINED__
/* library ADODB */
/* [helpstring][helpfile][version][uuid] */ 
typedef /* [uuid][helpcontext][public] */  DECLSPEC_UUID("0000051B-0000-0010-8000-00AA006D2EA4") 
enum CursorTypeEnum
    {	adOpenUnspecified	= -1,
	adOpenForwardOnly	= 0,
	adOpenKeyset	= 1,
	adOpenDynamic	= 2,
	adOpenStatic	= 3
    }	CursorTypeEnum;
typedef /* [uuid][helpcontext] */  DECLSPEC_UUID("0000051C-0000-0010-8000-00AA006D2EA4") 
enum CursorOptionEnum
    {	adHoldRecords	= 0x100,
	adMovePrevious	= 0x200,
	adAddNew	= 0x1000400,
	adDelete	= 0x1000800,
	adUpdate	= 0x1008000,
	adBookmark	= 0x2000,
	adApproxPosition	= 0x4000,
	adUpdateBatch	= 0x10000,
	adResync	= 0x20000,
	adNotify	= 0x40000,
	adFind	= 0x80000,
	adSeek	= 0x400000,
	adIndex	= 0x800000
    }	CursorOptionEnum;
typedef /* [uuid][helpcontext] */  DECLSPEC_UUID("0000051D-0000-0010-8000-00AA006D2EA4") 
enum LockTypeEnum
    {	adLockUnspecified	= -1,
	adLockReadOnly	= 1,
	adLockPessimistic	= 2,
	adLockOptimistic	= 3,
	adLockBatchOptimistic	= 4
    }	LockTypeEnum;
typedef /* [uuid][helpcontext] */  DECLSPEC_UUID("0000051E-0000-0010-8000-00AA006D2EA4") 
enum ExecuteOptionEnum
    {	adOptionUnspecified	= -1,
	adAsyncExecute	= 0x10,
	adAsyncFetch	= 0x20,
	adAsyncFetchNonBlocking	= 0x40,
	adExecuteNoRecords	= 0x80,
	adExecuteStream	= 0x400,
	adExecuteRecord	= 0x800
    }	ExecuteOptionEnum;
typedef /* [helpcontext][uuid] */  DECLSPEC_UUID("00000541-0000-0010-8000-00AA006D2EA4") 
enum ConnectOptionEnum
    {	adConnectUnspecified	= -1,
	adAsyncConnect	= 0x10
    }	ConnectOptionEnum;
typedef /* [helpcontext][uuid] */  DECLSPEC_UUID("00000532-0000-0010-8000-00AA006D2EA4") 
enum ObjectStateEnum
    {	adStateClosed	= 0,
	adStateOpen	= 0x1,
	adStateConnecting	= 0x2,
	adStateExecuting	= 0x4,
	adStateFetching	= 0x8
    }	ObjectStateEnum;
typedef /* [helpcontext][uuid] */  DECLSPEC_UUID("0000052F-0000-0010-8000-00AA006D2EA4") 
enum CursorLocationEnum
    {	adUseNone	= 1,
	adUseServer	= 2,
	adUseClient	= 3,
	adUseClientBatch	= 3
    }	CursorLocationEnum;
typedef /* [helpcontext][uuid] */  DECLSPEC_UUID("0000051F-0000-0010-8000-00AA006D2EA4") 
enum DataTypeEnum
    {	adEmpty	= 0,
	adTinyInt	= 16,
	adSmallInt	= 2,
	adInteger	= 3,
	adBigInt	= 20,
	adUnsignedTinyInt	= 17,
	adUnsignedSmallInt	= 18,
	adUnsignedInt	= 19,
	adUnsignedBigInt	= 21,
	adSingle	= 4,
	adDouble	= 5,
	adCurrency	= 6,
	adDecimal	= 14,
	adNumeric	= 131,
	adBoolean	= 11,
	adError	= 10,
	adUserDefined	= 132,
	adVariant	= 12,
	adIDispatch	= 9,
	adIUnknown	= 13,
	adGUID	= 72,
	adDate	= 7,
	adDBDate	= 133,
	adDBTime	= 134,
	adDBTimeStamp	= 135,
	adBSTR	= 8,
	adChar	= 129,
	adVarChar	= 200,
	adLongVarChar	= 201,
	adWChar	= 130,
	adVarWChar	= 202,
	adLongVarWChar	= 203,
	adBinary	= 128,
	adVarBinary	= 204,
	adLongVarBinary	= 205,
	adChapter	= 136,
	adFileTime	= 64,
	adPropVariant	= 138,
	adVarNumeric	= 139,
	adArray	= 0x2000
    }	DataTypeEnum;
typedef /* [helpcontext][uuid] */  DECLSPEC_UUID("00000525-0000-0010-8000-00AA006D2EA4") 
enum FieldAttributeEnum
    {	adFldUnspecified	= -1,
	adFldMayDefer	= 0x2,
	adFldUpdatable	= 0x4,
	adFldUnknownUpdatable	= 0x8,
	adFldFixed	= 0x10,
	adFldIsNullable	= 0x20,
	adFldMayBeNull	= 0x40,
	adFldLong	= 0x80,
	adFldRowID	= 0x100,
	adFldRowVersion	= 0x200,
	adFldCacheDeferred	= 0x1000,
	adFldIsChapter	= 0x2000,
	adFldNegativeScale	= 0x4000,
	adFldKeyColumn	= 0x8000,
	adFldIsRowURL	= 0x10000,
	adFldIsDefaultStream	= 0x20000,
	adFldIsCollection	= 0x40000
    }	FieldAttributeEnum;
typedef /* [helpcontext][uuid] */  DECLSPEC_UUID("00000526-0000-0010-8000-00AA006D2EA4") 
enum EditModeEnum
    {	adEditNone	= 0,
	adEditInProgress	= 0x1,
	adEditAdd	= 0x2,
	adEditDelete	= 0x4
    }	EditModeEnum;
typedef /* [helpcontext][uuid] */  DECLSPEC_UUID("00000527-0000-0010-8000-00AA006D2EA4") 
enum RecordStatusEnum
    {	adRecOK	= 0,
	adRecNew	= 0x1,
	adRecModified	= 0x2,
	adRecDeleted	= 0x4,
	adRecUnmodified	= 0x8,
	adRecInvalid	= 0x10,
	adRecMultipleChanges	= 0x40,
	adRecPendingChanges	= 0x80,
	adRecCanceled	= 0x100,
	adRecCantRelease	= 0x400,
	adRecConcurrencyViolation	= 0x800,
	adRecIntegrityViolation	= 0x1000,
	adRecMaxChangesExceeded	= 0x2000,
	adRecObjectOpen	= 0x4000,
	adRecOutOfMemory	= 0x8000,
	adRecPermissionDenied	= 0x10000,
	adRecSchemaViolation	= 0x20000,
	adRecDBDeleted	= 0x40000
    }	RecordStatusEnum;
typedef /* [helpcontext][uuid] */  DECLSPEC_UUID("00000542-0000-0010-8000-00AA006D2EA4") 
enum GetRowsOptionEnum
    {	adGetRowsRest	= -1
    }	GetRowsOptionEnum;
typedef /* [helpcontext][uuid] */  DECLSPEC_UUID("00000528-0000-0010-8000-00AA006D2EA4") 
enum PositionEnum
    {	adPosUnknown	= -1,
	adPosBOF	= -2,
	adPosEOF	= -3
    }	PositionEnum;
typedef /* [helpcontext] */ 
enum BookmarkEnum
    {	adBookmarkCurrent	= 0,
	adBookmarkFirst	= 1,
	adBookmarkLast	= 2
    }	BookmarkEnum;
typedef /* [helpcontext][uuid] */  DECLSPEC_UUID("00000540-0000-0010-8000-00AA006D2EA4") 
enum MarshalOptionsEnum
    {	adMarshalAll	= 0,
	adMarshalModifiedOnly	= 1
    }	MarshalOptionsEnum;
typedef /* [helpcontext][uuid] */  DECLSPEC_UUID("00000543-0000-0010-8000-00AA006D2EA4") 
enum AffectEnum
    {	adAffectCurrent	= 1,
	adAffectGroup	= 2,
	adAffectAll	= 3,
	adAffectAllChapters	= 4
    }	AffectEnum;
typedef /* [helpcontext][uuid] */  DECLSPEC_UUID("00000544-0000-0010-8000-00AA006D2EA4") 
enum ResyncEnum
    {	adResyncUnderlyingValues	= 1,
	adResyncAllValues	= 2
    }	ResyncEnum;
typedef /* [helpcontext][uuid] */  DECLSPEC_UUID("00000545-0000-0010-8000-00AA006D2EA4") 
enum CompareEnum
    {	adCompareLessThan	= 0,
	adCompareEqual	= 1,
	adCompareGreaterThan	= 2,
	adCompareNotEqual	= 3,
	adCompareNotComparable	= 4
    }	CompareEnum;
typedef /* [helpcontext][uuid] */  DECLSPEC_UUID("00000546-0000-0010-8000-00AA006D2EA4") 
enum FilterGroupEnum
    {	adFilterNone	= 0,
	adFilterPendingRecords	= 1,
	adFilterAffectedRecords	= 2,
	adFilterFetchedRecords	= 3,
	adFilterPredicate	= 4,
	adFilterConflictingRecords	= 5
    }	FilterGroupEnum;
typedef /* [helpcontext][uuid] */  DECLSPEC_UUID("00000547-0000-0010-8000-00AA006D2EA4") 
enum SearchDirectionEnum
    {	adSearchForward	= 1,
	adSearchBackward	= -1
    }	SearchDirectionEnum;
typedef /* [hidden] */ SearchDirectionEnum SearchDirection;
typedef /* [helpcontext][uuid] */  DECLSPEC_UUID("00000548-0000-0010-8000-00AA006D2EA4") 
enum PersistFormatEnum
    {	adPersistADTG	= 0,
	adPersistXML	= 1
    }	PersistFormatEnum;
typedef /* [helpcontext][uuid] */  DECLSPEC_UUID("00000549-0000-0010-8000-00AA006D2EA4") 
enum StringFormatEnum
    {	adClipString	= 2
    }	StringFormatEnum;
typedef /* [helpcontext][uuid] */  DECLSPEC_UUID("00000520-0000-0010-8000-00AA006D2EA4") 
enum ConnectPromptEnum
    {	adPromptAlways	= 1,
	adPromptComplete	= 2,
	adPromptCompleteRequired	= 3,
	adPromptNever	= 4
    }	ConnectPromptEnum;
typedef /* [helpcontext][uuid] */  DECLSPEC_UUID("00000521-0000-0010-8000-00AA006D2EA4") 
enum ConnectModeEnum
    {	adModeUnknown	= 0,
	adModeRead	= 1,
	adModeWrite	= 2,
	adModeReadWrite	= 3,
	adModeShareDenyRead	= 4,
	adModeShareDenyWrite	= 8,
	adModeShareExclusive	= 0xc,
	adModeShareDenyNone	= 0x10,
	adModeRecursive	= 0x400000
    }	ConnectModeEnum;
typedef /* [helpcontext][uuid] */  DECLSPEC_UUID("00000570-0000-0010-8000-00AA006D2EA4") 
enum RecordCreateOptionsEnum
    {	adCreateCollection	= 0x2000,
	adCreateStructDoc	= 0x80000000,
	adCreateNonCollection	= 0,
	adOpenIfExists	= 0x2000000,
	adCreateOverwrite	= 0x4000000,
	adFailIfNotExists	= -1
    }	RecordCreateOptionsEnum;
typedef /* [helpcontext][uuid] */  DECLSPEC_UUID("00000571-0000-0010-8000-00AA006D2EA4") 
enum RecordOpenOptionsEnum
    {	adOpenRecordUnspecified	= -1,
	adOpenSource	= 0x800000,
	adOpenOutput	= 0x800000,
	adOpenAsync	= 0x1000,
	adDelayFetchStream	= 0x4000,
	adDelayFetchFields	= 0x8000,
	adOpenExecuteCommand	= 0x10000
    }	RecordOpenOptionsEnum;
typedef /* [helpcontext][uuid] */  DECLSPEC_UUID("00000523-0000-0010-8000-00AA006D2EA4") 
enum IsolationLevelEnum
    {	adXactUnspecified	= 0xffffffff,
	adXactChaos	= 0x10,
	adXactReadUncommitted	= 0x100,
	adXactBrowse	= 0x100,
	adXactCursorStability	= 0x1000,
	adXactReadCommitted	= 0x1000,
	adXactRepeatableRead	= 0x10000,
	adXactSerializable	= 0x100000,
	adXactIsolated	= 0x100000
    }	IsolationLevelEnum;
typedef /* [helpcontext][uuid] */  DECLSPEC_UUID("00000524-0000-0010-8000-00AA006D2EA4") 
enum XactAttributeEnum
    {	adXactCommitRetaining	= 0x20000,
	adXactAbortRetaining	= 0x40000,
	adXactAsyncPhaseOne	= 0x80000,
	adXactSyncPhaseOne	= 0x100000
    }	XactAttributeEnum;
typedef /* [helpcontext][uuid] */  DECLSPEC_UUID("00000529-0000-0010-8000-00AA006D2EA4") 
enum PropertyAttributesEnum
    {	adPropNotSupported	= 0,
	adPropRequired	= 0x1,
	adPropOptional	= 0x2,
	adPropRead	= 0x200,
	adPropWrite	= 0x400
    }	PropertyAttributesEnum;
typedef /* [helpcontext][uuid] */  DECLSPEC_UUID("0000052A-0000-0010-8000-00AA006D2EA4") 
enum ErrorValueEnum
    {	adErrProviderFailed	=     MAKE_HRESULT(SEVERITY_ERROR, FACILITY_CONTROL, 0xbb8),
	adErrInvalidArgument	=     MAKE_HRESULT(SEVERITY_ERROR, FACILITY_CONTROL, 0xbb9),
	adErrOpeningFile	= 0xbba,
	adErrReadFile	=     MAKE_HRESULT(SEVERITY_ERROR, FACILITY_CONTROL, 0xbbb),
	adErrWriteFile	=     MAKE_HRESULT(SEVERITY_ERROR, FACILITY_CONTROL, 0xbbc),
	adErrNoCurrentRecord	=     MAKE_HRESULT(SEVERITY_ERROR, FACILITY_CONTROL, 0xbcd),
	adErrIllegalOperation	=     MAKE_HRESULT(SEVERITY_ERROR, FACILITY_CONTROL, 0xc93),
	adErrCantChangeProvider	=     MAKE_HRESULT(SEVERITY_ERROR, FACILITY_CONTROL, 0xc94),
	adErrInTransaction	=     MAKE_HRESULT(SEVERITY_ERROR, FACILITY_CONTROL, 0xcae),
	adErrFeatureNotAvailable	=     MAKE_HRESULT(SEVERITY_ERROR, FACILITY_CONTROL, 0xcb3),
	adErrItemNotFound	=     MAKE_HRESULT(SEVERITY_ERROR, FACILITY_CONTROL, 0xcc1),
	adErrObjectInCollection	=     MAKE_HRESULT(SEVERITY_ERROR, FACILITY_CONTROL, 0xd27),
	adErrObjectNotSet	=     MAKE_HRESULT(SEVERITY_ERROR, FACILITY_CONTROL, 0xd5c),
	adErrDataConversion	=     MAKE_HRESULT(SEVERITY_ERROR, FACILITY_CONTROL, 0xd5d),
	adErrObjectClosed	=     MAKE_HRESULT(SEVERITY_ERROR, FACILITY_CONTROL, 0xe78),
	adErrObjectOpen	=     MAKE_HRESULT(SEVERITY_ERROR, FACILITY_CONTROL, 0xe79),
	adErrProviderNotFound	=     MAKE_HRESULT(SEVERITY_ERROR, FACILITY_CONTROL, 0xe7a),
	adErrBoundToCommand	=     MAKE_HRESULT(SEVERITY_ERROR, FACILITY_CONTROL, 0xe7b),
	adErrInvalidParamInfo	=     MAKE_HRESULT(SEVERITY_ERROR, FACILITY_CONTROL, 0xe7c),
	adErrInvalidConnection	=     MAKE_HRESULT(SEVERITY_ERROR, FACILITY_CONTROL, 0xe7d),
	adErrNotReentrant	=     MAKE_HRESULT(SEVERITY_ERROR, FACILITY_CONTROL, 0xe7e),
	adErrStillExecuting	=     MAKE_HRESULT(SEVERITY_ERROR, FACILITY_CONTROL, 0xe7f),
	adErrOperationCancelled	=     MAKE_HRESULT(SEVERITY_ERROR, FACILITY_CONTROL, 0xe80),
	adErrStillConnecting	=     MAKE_HRESULT(SEVERITY_ERROR, FACILITY_CONTROL, 0xe81),
	adErrInvalidTransaction	=     MAKE_HRESULT(SEVERITY_ERROR, FACILITY_CONTROL, 0xe82),
	adErrNotExecuting	=     MAKE_HRESULT(SEVERITY_ERROR, FACILITY_CONTROL, 0xe83),
	adErrUnsafeOperation	=     MAKE_HRESULT(SEVERITY_ERROR, FACILITY_CONTROL, 0xe84),
	adwrnSecurityDialog	=     MAKE_HRESULT(SEVERITY_ERROR, FACILITY_CONTROL, 0xe85),
	adwrnSecurityDialogHeader	=     MAKE_HRESULT(SEVERITY_ERROR, FACILITY_CONTROL, 0xe86),
	adErrIntegrityViolation	=     MAKE_HRESULT(SEVERITY_ERROR, FACILITY_CONTROL, 0xe87),
	adErrPermissionDenied	=     MAKE_HRESULT(SEVERITY_ERROR, FACILITY_CONTROL, 0xe88),
	adErrDataOverflow	=     MAKE_HRESULT(SEVERITY_ERROR, FACILITY_CONTROL, 0xe89),
	adErrSchemaViolation	=     MAKE_HRESULT(SEVERITY_ERROR, FACILITY_CONTROL, 0xe8a),
	adErrSignMismatch	=     MAKE_HRESULT(SEVERITY_ERROR, FACILITY_CONTROL, 0xe8b),
	adErrCantConvertvalue	=     MAKE_HRESULT(SEVERITY_ERROR, FACILITY_CONTROL, 0xe8c),
	adErrCantCreate	= 0xe8d,
	adErrColumnNotOnThisRow	=     MAKE_HRESULT(SEVERITY_ERROR, FACILITY_CONTROL, 0xe8e),
	adErrURLDoesNotExist	=     MAKE_HRESULT(SEVERITY_ERROR, FACILITY_CONTROL, 0xe8f),
	adErrTreePermissionDenied	=     MAKE_HRESULT(SEVERITY_ERROR, FACILITY_CONTROL, 0xe90),
	adErrInvalidURL	=     MAKE_HRESULT(SEVERITY_ERROR, FACILITY_CONTROL, 0xe91),
	adErrResourceLocked	=     MAKE_HRESULT(SEVERITY_ERROR, FACILITY_CONTROL, 0xe92),
	adErrResourceExists	=     MAKE_HRESULT(SEVERITY_ERROR, FACILITY_CONTROL, 0xe93),
	adErrCannotComplete	=     MAKE_HRESULT(SEVERITY_ERROR, FACILITY_CONTROL, 0xe94),
	adErrVolumeNotFound	=     MAKE_HRESULT(SEVERITY_ERROR, FACILITY_CONTROL, 0xe95),
	adErrOutOfSpace	=     MAKE_HRESULT(SEVERITY_ERROR, FACILITY_CONTROL, 0xe96),
	adErrResourceOutOfScope	=     MAKE_HRESULT(SEVERITY_ERROR, FACILITY_CONTROL, 0xe97),
	adErrUnavailable	=     MAKE_HRESULT(SEVERITY_ERROR, FACILITY_CONTROL, 0xe98),
	adErrURLNamedRowDoesNotExist	=     MAKE_HRESULT(SEVERITY_ERROR, FACILITY_CONTROL, 0xe99),
	adErrDelResOutOfScope	=     MAKE_HRESULT(SEVERITY_ERROR, FACILITY_CONTROL, 0xe9a),
	adErrPropInvalidColumn	=     MAKE_HRESULT(SEVERITY_ERROR, FACILITY_CONTROL, 0xe9b),
	adErrPropInvalidOption	=     MAKE_HRESULT(SEVERITY_ERROR, FACILITY_CONTROL, 0xe9c),
	adErrPropInvalidValue	=     MAKE_HRESULT(SEVERITY_ERROR, FACILITY_CONTROL, 0xe9d),
	adErrPropConflicting	=     MAKE_HRESULT(SEVERITY_ERROR, FACILITY_CONTROL, 0xe9e),
	adErrPropNotAllSettable	=     MAKE_HRESULT(SEVERITY_ERROR, FACILITY_CONTROL, 0xe9f),
	adErrPropNotSet	=     MAKE_HRESULT(SEVERITY_ERROR, FACILITY_CONTROL, 0xea0),
	adErrPropNotSettable	=     MAKE_HRESULT(SEVERITY_ERROR, FACILITY_CONTROL, 0xea1),
	adErrPropNotSupported	=     MAKE_HRESULT(SEVERITY_ERROR, FACILITY_CONTROL, 0xea2),
	adErrCatalogNotSet	=     MAKE_HRESULT(SEVERITY_ERROR, FACILITY_CONTROL, 0xea3),
	adErrCantChangeConnection	=     MAKE_HRESULT(SEVERITY_ERROR, FACILITY_CONTROL, 0xea4),
	adErrFieldsUpdateFailed	=     MAKE_HRESULT(SEVERITY_ERROR, FACILITY_CONTROL, 0xea5),
	adErrDenyNotSupported	=     MAKE_HRESULT(SEVERITY_ERROR, FACILITY_CONTROL, 0xea6),
	adErrDenyTypeNotSupported	=     MAKE_HRESULT(SEVERITY_ERROR, FACILITY_CONTROL, 0xea7),
    }	ErrorValueEnum;
typedef /* [helpcontext][uuid] */  DECLSPEC_UUID("0000052B-0000-0010-8000-00AA006D2EA4") 
enum ParameterAttributesEnum
    {	adParamSigned	= 0x10,
	adParamNullable	= 0x40,
	adParamLong	= 0x80
    }	ParameterAttributesEnum;
typedef /* [helpcontext][uuid] */  DECLSPEC_UUID("0000052C-0000-0010-8000-00AA006D2EA4") 
enum ParameterDirectionEnum
    {	adParamUnknown	= 0,
	adParamInput	= 0x1,
	adParamOutput	= 0x2,
	adParamInputOutput	= 0x3,
	adParamReturnValue	= 0x4
    }	ParameterDirectionEnum;
typedef /* [helpcontext][uuid] */  DECLSPEC_UUID("0000052E-0000-0010-8000-00AA006D2EA4") 
enum CommandTypeEnum
    {	adCmdUnspecified	= -1,
	adCmdUnknown	= 0x8,
	adCmdText	= 0x1,
	adCmdTable	= 0x2,
	adCmdStoredProc	= 0x4,
	adCmdFile	= 0x100,
	adCmdTableDirect	= 0x200
    }	CommandTypeEnum;
typedef /* [helpcontext][uuid] */  DECLSPEC_UUID("00000530-0000-0010-8000-00AA006D2EA4") 
enum EventStatusEnum
    {	adStatusOK	= 0x1,
	adStatusErrorsOccurred	= 0x2,
	adStatusCantDeny	= 0x3,
	adStatusCancel	= 0x4,
	adStatusUnwantedEvent	= 0x5
    }	EventStatusEnum;
typedef /* [helpcontext][uuid] */  DECLSPEC_UUID("00000531-0000-0010-8000-00AA006D2EA4") 
enum EventReasonEnum
    {	adRsnAddNew	= 1,
	adRsnDelete	= 2,
	adRsnUpdate	= 3,
	adRsnUndoUpdate	= 4,
	adRsnUndoAddNew	= 5,
	adRsnUndoDelete	= 6,
	adRsnRequery	= 7,
	adRsnResynch	= 8,
	adRsnClose	= 9,
	adRsnMove	= 10,
	adRsnFirstChange	= 11,
	adRsnMoveFirst	= 12,
	adRsnMoveNext	= 13,
	adRsnMovePrevious	= 14,
	adRsnMoveLast	= 15
    }	EventReasonEnum;
typedef /* [helpcontext][uuid] */  DECLSPEC_UUID("00000533-0000-0010-8000-00AA006D2EA4") 
enum SchemaEnum
    {	adSchemaProviderSpecific	= -1,
	adSchemaAsserts	= 0,
	adSchemaCatalogs	= 1,
	adSchemaCharacterSets	= 2,
	adSchemaCollations	= 3,
	adSchemaColumns	= 4,
	adSchemaCheckConstraints	= 5,
	adSchemaConstraintColumnUsage	= 6,
	adSchemaConstraintTableUsage	= 7,
	adSchemaKeyColumnUsage	= 8,
	adSchemaReferentialContraints	= 9,
	adSchemaReferentialConstraints	= 9,
	adSchemaTableConstraints	= 10,
	adSchemaColumnsDomainUsage	= 11,
	adSchemaIndexes	= 12,
	adSchemaColumnPrivileges	= 13,
	adSchemaTablePrivileges	= 14,
	adSchemaUsagePrivileges	= 15,
	adSchemaProcedures	= 16,
	adSchemaSchemata	= 17,
	adSchemaSQLLanguages	= 18,
	adSchemaStatistics	= 19,
	adSchemaTables	= 20,
	adSchemaTranslations	= 21,
	adSchemaProviderTypes	= 22,
	adSchemaViews	= 23,
	adSchemaViewColumnUsage	= 24,
	adSchemaViewTableUsage	= 25,
	adSchemaProcedureParameters	= 26,
	adSchemaForeignKeys	= 27,
	adSchemaPrimaryKeys	= 28,
	adSchemaProcedureColumns	= 29,
	adSchemaDBInfoKeywords	= 30,
	adSchemaDBInfoLiterals	= 31,
	adSchemaCubes	= 32,
	adSchemaDimensions	= 33,
	adSchemaHierarchies	= 34,
	adSchemaLevels	= 35,
	adSchemaMeasures	= 36,
	adSchemaProperties	= 37,
	adSchemaMembers	= 38,
	adSchemaTrustees	= 39
    }	SchemaEnum;
typedef /* [helpcontext][uuid] */  DECLSPEC_UUID("0000057E-0000-0010-8000-00AA006D2EA4") 
enum FieldStatusEnum
    {	adFieldOK	= 0,
	adFieldCantConvertValue	= 2,
	adFieldIsNull	= 3,
	adFieldTruncated	= 4,
	adFieldSignMismatch	= 5,
	adFieldDataOverflow	= 6,
	adFieldCantCreate	= 7,
	adFieldUnavailable	= 8,
	adFieldPermissionDenied	= 9,
	adFieldIntegrityViolation	= 10,
	adFieldSchemaViolation	= 11,
	adFieldBadStatus	= 12,
	adFieldDefault	= 13,
	adFieldIgnore	= 15,
	adFieldDoesNotExist	= 16,
	adFieldInvalidURL	= 17,
	adFieldResourceLocked	= 18,
	adFieldResourceExists	= 19,
	adFieldCannotComplete	= 20,
	adFieldVolumeNotFound	= 21,
	adFieldOutOfSpace	= 22,
	adFieldCannotDeleteSource	= 23,
	adFieldReadOnly	= 24,
	adFieldResourceOutOfScope	= 25,
	adFieldAlreadyExists	= 26,
	adFieldPendingInsert	= 0x10000,
	adFieldPendingDelete	= 0x20000,
	adFieldPendingChange	= 0x40000,
	adFieldPendingUnknown	= 0x80000,
	adFieldPendingUnknownDelete	= 0x100000
    }	FieldStatusEnum;
typedef /* [helpcontext][uuid] */  DECLSPEC_UUID("00000552-0000-0010-8000-00AA006D2EA4") 
enum SeekEnum
    {	adSeekFirstEQ	= 0x1,
	adSeekLastEQ	= 0x2,
	adSeekAfterEQ	= 0x4,
	adSeekAfter	= 0x8,
	adSeekBeforeEQ	= 0x10,
	adSeekBefore	= 0x20
    }	SeekEnum;
#ifndef _COMMON_ADC_AND_ADO_PROPS_
#define _COMMON_ADC_AND_ADO_PROPS_
typedef /* [helpcontext][uuid] */  DECLSPEC_UUID("0000054A-0000-0010-8000-00AA006D2EA4") 
enum ADCPROP_UPDATECRITERIA_ENUM
    {	adCriteriaKey	= 0,
	adCriteriaAllCols	= 1,
	adCriteriaUpdCols	= 2,
	adCriteriaTimeStamp	= 3
    }	ADCPROP_UPDATECRITERIA_ENUM;
typedef /* [helpcontext][uuid] */  DECLSPEC_UUID("0000054B-0000-0010-8000-00AA006D2EA4") 
enum ADCPROP_ASYNCTHREADPRIORITY_ENUM
    {	adPriorityLowest	= 1,
	adPriorityBelowNormal	= 2,
	adPriorityNormal	= 3,
	adPriorityAboveNormal	= 4,
	adPriorityHighest	= 5
    }	ADCPROP_ASYNCTHREADPRIORITY_ENUM;
typedef /* [helpcontext][uuid] */  DECLSPEC_UUID("00000554-0000-0010-8000-00AA006D2EA4") 
enum ADCPROP_AUTORECALC_ENUM
    {	adRecalcUpFront	= 0,
	adRecalcAlways	= 1
    }	ADCPROP_AUTORECALC_ENUM;
typedef /* [helpcontext][uuid] */  DECLSPEC_UUID("00000553-0000-0010-8000-00AA006D2EA4") 
enum ADCPROP_UPDATERESYNC_ENUM
    {	adResyncNone	= 0,
	adResyncAutoIncrement	= 1,
	adResyncConflicts	= 2,
	adResyncUpdates	= 4,
	adResyncInserts	= 8,
	adResyncAll	= 15
    }	ADCPROP_UPDATERESYNC_ENUM;
#endif	/* _COMMON_ADC_AND_ADO_PROPS_ */
typedef ADCPROP_UPDATERESYNC_ENUM CEResyncEnum;
typedef /* [helpcontext][uuid] */  DECLSPEC_UUID("00000573-0000-0010-8000-00AA006D2EA4") 
enum MoveRecordOptionsEnum
    {	adMoveUnspecified	= -1,
	adMoveOverWrite	= 1,
	adMoveDontUpdateLinks	= 2,
	adMoveAllowEmulation	= 4
    }	MoveRecordOptionsEnum;
typedef /* [helpcontext][uuid] */  DECLSPEC_UUID("00000574-0000-0010-8000-00AA006D2EA4") 
enum CopyRecordOptionsEnum
    {	adCopyUnspecified	= -1,
	adCopyOverWrite	= 1,
	adCopyAllowEmulation	= 4,
	adCopyNonRecursive	= 2
    }	CopyRecordOptionsEnum;
typedef /* [helpcontext][uuid] */  DECLSPEC_UUID("00000576-0000-0010-8000-00AA006D2EA4") 
enum StreamTypeEnum
    {	adTypeBinary	= 1,
	adTypeText	= 2
    }	StreamTypeEnum;
typedef /* [helpcontext][uuid] */  DECLSPEC_UUID("00000577-0000-0010-8000-00AA006D2EA4") 
enum LineSeparatorEnum
    {	adLF	= 10,
	adCR	= 13,
	adCRLF	= -1
    }	LineSeparatorEnum;
typedef /* [helpcontext][uuid] */  DECLSPEC_UUID("0000057A-0000-0010-8000-00AA006D2EA4") 
enum StreamOpenOptionsEnum
    {	adOpenStreamUnspecified	= -1,
	adOpenStreamAsync	= 1,
	adOpenStreamFromRecord	= 4
    }	StreamOpenOptionsEnum;
typedef /* [helpcontext][uuid] */  DECLSPEC_UUID("0000057B-0000-0010-8000-00AA006D2EA4") 
enum StreamWriteEnum
    {	adWriteChar	= 0,
	adWriteLine	= 1,
	stWriteChar	= 0,
	stWriteLine	= 1
    }	StreamWriteEnum;
typedef /* [helpcontext][uuid] */  DECLSPEC_UUID("0000057C-0000-0010-8000-00AA006D2EA4") 
enum SaveOptionsEnum
    {	adSaveCreateNotExist	= 1,
	adSaveCreateOverWrite	= 2
    }	SaveOptionsEnum;
typedef /* [helpcontext] */ 
enum FieldEnum
    {	adDefaultStream	= -1,
	adRecordURL	= -2
    }	FieldEnum;
typedef /* [helpcontext] */ 
enum StreamReadEnum
    {	adReadAll	= -1,
	adReadLine	= -2
    }	StreamReadEnum;
typedef /* [helpcontext][uuid] */  DECLSPEC_UUID("0000057D-0000-0010-8000-00AA006D2EA4") 
enum RecordTypeEnum
    {	adSimpleRecord	= 0,
	adCollectionRecord	= 1,
	adStructDoc	= 2
    }	RecordTypeEnum;
EXTERN_C const IID LIBID_ADODB;
#ifndef ___ADOCollection_INTERFACE_DEFINED__
#define ___ADOCollection_INTERFACE_DEFINED__
/* interface _ADOCollection */
/* [object][uuid][nonextensible][dual] */ 
EXTERN_C const IID IID__ADOCollection;
#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("00000512-0000-0010-8000-00AA006D2EA4")
    _ADOCollection : public IDispatch
    {
    public:
        virtual /* [id][helpcontext][propget] */ HRESULT STDMETHODCALLTYPE get_Count( 
            /* [retval][out] */ long __RPC_FAR *c) = 0;
        
        virtual /* [id][restricted] */ HRESULT STDMETHODCALLTYPE _NewEnum( 
            /* [retval][out] */ IUnknown __RPC_FAR *__RPC_FAR *ppvObject) = 0;
        
        virtual /* [id][helpcontext] */ HRESULT STDMETHODCALLTYPE Refresh( void) = 0;
        
    };
    
#else 	/* C style interface */
    typedef struct _ADOCollectionVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            _ADOCollection __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            _ADOCollection __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            _ADOCollection __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            _ADOCollection __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            _ADOCollection __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            _ADOCollection __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            _ADOCollection __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        /* [id][helpcontext][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Count )( 
            _ADOCollection __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *c);
        
        /* [id][restricted] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *_NewEnum )( 
            _ADOCollection __RPC_FAR * This,
            /* [retval][out] */ IUnknown __RPC_FAR *__RPC_FAR *ppvObject);
        
        /* [id][helpcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Refresh )( 
            _ADOCollection __RPC_FAR * This);
        
        END_INTERFACE
    } _ADOCollectionVtbl;
    interface _ADOCollection
    {
        CONST_VTBL struct _ADOCollectionVtbl __RPC_FAR *lpVtbl;
    };
    
#ifdef COBJMACROS
#define _ADOCollection_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)
#define _ADOCollection_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)
#define _ADOCollection_Release(This)	\
    (This)->lpVtbl -> Release(This)
#define _ADOCollection_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)
#define _ADOCollection_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)
#define _ADOCollection_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)
#define _ADOCollection_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)
#define _Collection_get_Count(This,c)	\
    (This)->lpVtbl -> get_Count(This,c)
#define _ADOCollection__NewEnum(This,ppvObject)	\
    (This)->lpVtbl -> _NewEnum(This,ppvObject)
#define _ADOCollection_Refresh(This)	\
    (This)->lpVtbl -> Refresh(This)
#endif /* COBJMACROS */
#endif 	/* C style interface */
/* [id][helpcontext][propget] */ HRESULT STDMETHODCALLTYPE _Collection_get_Count_Proxy( 
    _ADOCollection __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *c);
void __RPC_STUB _Collection_get_Count_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);
/* [id][restricted] */ HRESULT STDMETHODCALLTYPE _ADOCollection__NewEnum_Proxy( 
    _ADOCollection __RPC_FAR * This,
    /* [retval][out] */ IUnknown __RPC_FAR *__RPC_FAR *ppvObject);
void __RPC_STUB _ADOCollection__NewEnum_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);
/* [id][helpcontext] */ HRESULT STDMETHODCALLTYPE _ADOCollection_Refresh_Proxy( 
    _ADOCollection __RPC_FAR * This);
void __RPC_STUB _ADOCollection_Refresh_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);
#endif 	/* ___ADOCollection_INTERFACE_DEFINED__ */
#ifndef ___ADODynaCollection_INTERFACE_DEFINED__
#define ___ADODynaCollection_INTERFACE_DEFINED__
/* interface _ADODynaCollection */
/* [object][uuid][nonextensible][dual] */ 
EXTERN_C const IID IID__ADODynaCollection;
#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("00000513-0000-0010-8000-00AA006D2EA4")
_ADODynaCollection : public _ADOCollection
    {
    public:
        virtual /* [helpcontext] */ HRESULT STDMETHODCALLTYPE Append( 
            /* [in] */ IDispatch __RPC_FAR *Object) = 0;
        
        virtual /* [helpcontext] */ HRESULT STDMETHODCALLTYPE Delete( 
            /* [in] */ VARIANT Index) = 0;
        
    };
    
#else 	/* C style interface */
    typedef struct _ADODynaCollectionVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            _ADODynaCollection __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            _ADODynaCollection __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            _ADODynaCollection __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            _ADODynaCollection __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            _ADODynaCollection __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            _ADODynaCollection __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            _ADODynaCollection __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        /* [id][helpcontext][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Count )( 
            _ADODynaCollection __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *c);
        
        /* [id][restricted] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *_NewEnum )( 
            _ADODynaCollection __RPC_FAR * This,
            /* [retval][out] */ IUnknown __RPC_FAR *__RPC_FAR *ppvObject);
        
        /* [id][helpcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Refresh )( 
            _ADODynaCollection __RPC_FAR * This);
        
        /* [helpcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Append )( 
            _ADODynaCollection __RPC_FAR * This,
            /* [in] */ IDispatch __RPC_FAR *Object);
        
        /* [helpcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Delete )( 
            _ADODynaCollection __RPC_FAR * This,
            /* [in] */ VARIANT Index);
        
        END_INTERFACE
    } _ADODynaCollectionVtbl;
    interface _ADODynaCollection
    {
        CONST_VTBL struct _ADODynaCollectionVtbl __RPC_FAR *lpVtbl;
    };
    
#ifdef COBJMACROS
#define _ADODynaCollection_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)
#define _ADODynaCollection_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)
#define _ADODynaCollection_Release(This)	\
    (This)->lpVtbl -> Release(This)
#define _ADODynaCollection_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)
#define _ADODynaCollection_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)
#define _ADODynaCollection_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)
#define _ADODynaCollection_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)
#define _DynaCollection_get_Count(This,c)	\
    (This)->lpVtbl -> get_Count(This,c)
#define _ADODynaCollection__NewEnum(This,ppvObject)	\
    (This)->lpVtbl -> _NewEnum(This,ppvObject)
#define _ADODynaCollection_Refresh(This)	\
    (This)->lpVtbl -> Refresh(This)
#define _ADODynaCollection_Append(This,Object)	\
    (This)->lpVtbl -> Append(This,Object)
#define _ADODynaCollection_Delete(This,Index)	\
    (This)->lpVtbl -> Delete(This,Index)
#endif /* COBJMACROS */
#endif 	/* C style interface */
/* [helpcontext] */ HRESULT STDMETHODCALLTYPE _ADODynaCollection_Append_Proxy( 
    _ADODynaCollection __RPC_FAR * This,
    /* [in] */ IDispatch __RPC_FAR *Object);
void __RPC_STUB _ADODynaCollection_Append_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);
/* [helpcontext] */ HRESULT STDMETHODCALLTYPE _ADODynaCollection_Delete_Proxy( 
    _ADODynaCollection __RPC_FAR * This,
    /* [in] */ VARIANT Index);
void __RPC_STUB _ADODynaCollection_Delete_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);
#endif 	/* ___ADODynaCollection_INTERFACE_DEFINED__ */
#ifndef ___ADO_INTERFACE_DEFINED__
#define ___ADO_INTERFACE_DEFINED__
/* interface _ADO */
/* [object][uuid][nonextensible][dual] */ 
EXTERN_C const IID IID__ADO;
#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("00000534-0000-0010-8000-00AA006D2EA4")
    _ADO : public IDispatch
    {
    public:
        virtual /* [helpcontext][id][propget] */ HRESULT STDMETHODCALLTYPE get_Properties( 
            /* [retval][out] */ ADOProperties __RPC_FAR *__RPC_FAR *ppvObject) = 0;
        
    };
    
#else 	/* C style interface */
    typedef struct _ADOVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            _ADO __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            _ADO __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            _ADO __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            _ADO __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            _ADO __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            _ADO __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            _ADO __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        /* [helpcontext][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Properties )( 
            _ADO __RPC_FAR * This,
            /* [retval][out] */ ADOProperties __RPC_FAR *__RPC_FAR *ppvObject);
        
        END_INTERFACE
    } _ADOVtbl;
    interface _ADO
    {
        CONST_VTBL struct _ADOVtbl __RPC_FAR *lpVtbl;
    };
    
#ifdef COBJMACROS
#define _ADO_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)
#define _ADO_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)
#define _ADO_Release(This)	\
    (This)->lpVtbl -> Release(This)
#define _ADO_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)
#define _ADO_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)
#define _ADO_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)
#define _ADO_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)
#define _ADO_get_Properties(This,ppvObject)	\
    (This)->lpVtbl -> get_Properties(This,ppvObject)
#endif /* COBJMACROS */
#endif 	/* C style interface */
/* [helpcontext][id][propget] */ HRESULT STDMETHODCALLTYPE _ADO_get_Properties_Proxy( 
    _ADO __RPC_FAR * This,
    /* [retval][out] */ ADOProperties __RPC_FAR *__RPC_FAR *ppvObject);
void __RPC_STUB _ADO_get_Properties_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);
#endif 	/* ___ADO_INTERFACE_DEFINED__ */
#ifndef __Error_INTERFACE_DEFINED__
#define __Error_INTERFACE_DEFINED__
/* interface ADOError */
/* [object][helpcontext][uuid][nonextensible][dual] */ 
EXTERN_C const IID IID_Error;
#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("00000500-0000-0010-8000-00AA006D2EA4")
    ADOError : public IDispatch
    {
    public:
        virtual /* [helpcontext][propget][id] */ HRESULT STDMETHODCALLTYPE get_Number( 
            /* [retval][out] */ long __RPC_FAR *pl) = 0;
        
        virtual /* [helpcontext][propget][id] */ HRESULT STDMETHODCALLTYPE get_Source( 
            /* [retval][out] */ BSTR __RPC_FAR *pbstr) = 0;
        
        virtual /* [helpcontext][propget][id] */ HRESULT STDMETHODCALLTYPE get_Description( 
            /* [retval][out] */ BSTR __RPC_FAR *pbstr) = 0;
        
        virtual /* [helpcontext][propget][id] */ HRESULT STDMETHODCALLTYPE get_HelpFile( 
            /* [retval][out] */ BSTR __RPC_FAR *pbstr) = 0;
        
        virtual /* [helpcontext][propget][id] */ HRESULT STDMETHODCALLTYPE get_HelpContext( 
            /* [retval][out] */ long __RPC_FAR *pl) = 0;
        
        virtual /* [helpcontext][propget][id] */ HRESULT STDMETHODCALLTYPE get_SQLState( 
            /* [retval][out] */ BSTR __RPC_FAR *pbstr) = 0;
        
        virtual /* [helpcontext][propget][id] */ HRESULT STDMETHODCALLTYPE get_NativeError( 
            /* [retval][out] */ long __RPC_FAR *pl) = 0;
        
    };
    
#else 	/* C style interface */
    typedef struct ErrorVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            ADOError __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            ADOError __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            ADOError __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            ADOError __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            ADOError __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            ADOError __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            ADOError __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        /* [helpcontext][propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Number )( 
            ADOError __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *pl);
        
        /* [helpcontext][propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Source )( 
            ADOError __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pbstr);
        
        /* [helpcontext][propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Description )( 
            ADOError __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pbstr);
        
        /* [helpcontext][propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_HelpFile )( 
            ADOError __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pbstr);
        
        /* [helpcontext][propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_HelpContext )( 
            ADOError __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *pl);
        
        /* [helpcontext][propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_SQLState )( 
            ADOError __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pbstr);
        
        /* [helpcontext][propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_NativeError )( 
            ADOError __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *pl);
        
        END_INTERFACE
    } ErrorVtbl;
    interface Error
    {
        CONST_VTBL struct ErrorVtbl __RPC_FAR *lpVtbl;
    };
    
#ifdef COBJMACROS
#define Error_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)
#define Error_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)
#define Error_Release(This)	\
    (This)->lpVtbl -> Release(This)
#define Error_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)
#define Error_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)
#define Error_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)
#define Error_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)
#define Error_get_Number(This,pl)	\
    (This)->lpVtbl -> get_Number(This,pl)
#define Error_get_Source(This,pbstr)	\
    (This)->lpVtbl -> get_Source(This,pbstr)
#define Error_get_Description(This,pbstr)	\
    (This)->lpVtbl -> get_Description(This,pbstr)
#define Error_get_HelpFile(This,pbstr)	\
    (This)->lpVtbl -> get_HelpFile(This,pbstr)
#define Error_get_HelpContext(This,pl)	\
    (This)->lpVtbl -> get_HelpContext(This,pl)
#define Error_get_SQLState(This,pbstr)	\
    (This)->lpVtbl -> get_SQLState(This,pbstr)
#define Error_get_NativeError(This,pl)	\
    (This)->lpVtbl -> get_NativeError(This,pl)
#endif /* COBJMACROS */
#endif 	/* C style interface */
/* [helpcontext][propget][id] */ HRESULT STDMETHODCALLTYPE Error_get_Number_Proxy( 
    ADOError __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *pl);
void __RPC_STUB Error_get_Number_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);
/* [helpcontext][propget][id] */ HRESULT STDMETHODCALLTYPE Error_get_Source_Proxy( 
    ADOError __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pbstr);
void __RPC_STUB Error_get_Source_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);
/* [helpcontext][propget][id] */ HRESULT STDMETHODCALLTYPE Error_get_Description_Proxy( 
    ADOError __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pbstr);
void __RPC_STUB Error_get_Description_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);
/* [helpcontext][propget][id] */ HRESULT STDMETHODCALLTYPE Error_get_HelpFile_Proxy( 
    ADOError __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pbstr);
void __RPC_STUB Error_get_HelpFile_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);
/* [helpcontext][propget][id] */ HRESULT STDMETHODCALLTYPE Error_get_HelpContext_Proxy( 
    ADOError __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *pl);
void __RPC_STUB Error_get_HelpContext_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);
/* [helpcontext][propget][id] */ HRESULT STDMETHODCALLTYPE Error_get_SQLState_Proxy( 
    ADOError __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pbstr);
void __RPC_STUB Error_get_SQLState_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);
/* [helpcontext][propget][id] */ HRESULT STDMETHODCALLTYPE Error_get_NativeError_Proxy( 
    ADOError __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *pl);
void __RPC_STUB Error_get_NativeError_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);
#endif 	/* __Error_INTERFACE_DEFINED__ */
#ifndef __Errors_INTERFACE_DEFINED__
#define __Errors_INTERFACE_DEFINED__
/* interface ADOErrors */
/* [object][helpcontext][uuid][nonextensible][dual] */ 
EXTERN_C const IID IID_Errors;
#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("00000501-0000-0010-8000-00AA006D2EA4")
    ADOErrors : public _ADOCollection
    {
    public:
        virtual /* [helpcontext][id][propget] */ HRESULT STDMETHODCALLTYPE get_Item( 
            /* [in] */ VARIANT Index,
            /* [retval][out] */ ADOError __RPC_FAR *__RPC_FAR *ppvObject) = 0;
        
        virtual /* [helpcontext] */ HRESULT STDMETHODCALLTYPE Clear( void) = 0;
        
    };
    
#else 	/* C style interface */
    typedef struct ErrorsVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            ADOErrors __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            ADOErrors __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            ADOErrors __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            ADOErrors __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            ADOErrors __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            ADOErrors __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            ADOErrors __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        /* [id][helpcontext][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Count )( 
            ADOErrors __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *c);
        
        /* [id][restricted] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *_NewEnum )( 
            ADOErrors __RPC_FAR * This,
            /* [retval][out] */ IUnknown __RPC_FAR *__RPC_FAR *ppvObject);
        
        /* [id][helpcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Refresh )( 
            ADOErrors __RPC_FAR * This);
        
        /* [helpcontext][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Item )( 
            ADOErrors __RPC_FAR * This,
            /* [in] */ VARIANT Index,
            /* [retval][out] */ ADOError __RPC_FAR *__RPC_FAR *ppvObject);
        
        /* [helpcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Clear )( 
            ADOErrors __RPC_FAR * This);
        
        END_INTERFACE
    } ErrorsVtbl;
    interface Errors
    {
        CONST_VTBL struct ErrorsVtbl __RPC_FAR *lpVtbl;
    };
    
#ifdef COBJMACROS
#define Errors_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)
#define Errors_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)
#define Errors_Release(This)	\
    (This)->lpVtbl -> Release(This)
#define Errors_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)
#define Errors_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)
#define Errors_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)
#define Errors_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)
#define Errors_get_Count(This,c)	\
    (This)->lpVtbl -> get_Count(This,c)
#define Errors__NewEnum(This,ppvObject)	\
    (This)->lpVtbl -> _NewEnum(This,ppvObject)
#define Errors_Refresh(This)	\
    (This)->lpVtbl -> Refresh(This)
#define Errors_get_Item(This,Index,ppvObject)	\
    (This)->lpVtbl -> get_Item(This,Index,ppvObject)
#define Errors_Clear(This)	\
    (This)->lpVtbl -> Clear(This)
#endif /* COBJMACROS */
#endif 	/* C style interface */
/* [helpcontext][id][propget] */ HRESULT STDMETHODCALLTYPE Errors_get_Item_Proxy( 
    ADOErrors __RPC_FAR * This,
    /* [in] */ VARIANT Index,
    /* [retval][out] */ ADOError __RPC_FAR *__RPC_FAR *ppvObject);
void __RPC_STUB Errors_get_Item_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);
/* [helpcontext] */ HRESULT STDMETHODCALLTYPE Errors_Clear_Proxy( 
    ADOErrors __RPC_FAR * This);
void __RPC_STUB Errors_Clear_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);
#endif 	/* __Errors_INTERFACE_DEFINED__ */
#ifndef __Command15_INTERFACE_DEFINED__
#define __Command15_INTERFACE_DEFINED__
/* interface Command15 */
/* [object][helpcontext][uuid][hidden][nonextensible][dual] */ 
EXTERN_C const IID IID_Command15;
#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("00000508-0000-0010-8000-00AA006D2EA4")
    Command15 : public _ADO
    {
    public:
        virtual /* [helpcontext][propget][id] */ HRESULT STDMETHODCALLTYPE get_ActiveConnection( 
            /* [retval][out] */ _ADOConnection __RPC_FAR *__RPC_FAR *ppvObject) = 0;
        
        virtual /* [helpcontext][propputref][id] */ HRESULT STDMETHODCALLTYPE putref_ActiveConnection( 
            /* [in] */ _ADOConnection __RPC_FAR *pCon) = 0;
        
        virtual /* [helpcontext][propput][id] */ HRESULT STDMETHODCALLTYPE put_ActiveConnection( 
            /* [in] */ VARIANT vConn) = 0;
        
        virtual /* [helpcontext][propget][id] */ HRESULT STDMETHODCALLTYPE get_CommandText( 
            /* [retval][out] */ BSTR __RPC_FAR *pbstr) = 0;
        
        virtual /* [helpcontext][propput][id] */ HRESULT STDMETHODCALLTYPE put_CommandText( 
            /* [in] */ BSTR bstr) = 0;
        
        virtual /* [helpcontext][propget][id] */ HRESULT STDMETHODCALLTYPE get_CommandTimeout( 
            /* [retval][out] */ LONG __RPC_FAR *pl) = 0;
        
        virtual /* [helpcontext][propput][id] */ HRESULT STDMETHODCALLTYPE put_CommandTimeout( 
            /* [in] */ LONG Timeout) = 0;
        
        virtual /* [helpcontext][propget][id] */ HRESULT STDMETHODCALLTYPE get_Prepared( 
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pfPrepared) = 0;
        
        virtual /* [helpcontext][propput][id] */ HRESULT STDMETHODCALLTYPE put_Prepared( 
            /* [in] */ VARIANT_BOOL fPrepared) = 0;
        
        virtual /* [helpcontext][id] */ HRESULT STDMETHODCALLTYPE Execute( 
            /* [optional][out] */ VARIANT __RPC_FAR *RecordsAffected,
            /* [optional][in] */ VARIANT __RPC_FAR *Parameters,
            /* [defaultvalue][in] */ long Options,
            /* [retval][out] */ _ADORecordset __RPC_FAR *__RPC_FAR *ppirs) = 0;
        
        virtual /* [helpcontext][id] */ HRESULT STDMETHODCALLTYPE CreateParameter( 
            /* [defaultvalue][in] */ BSTR Name,
            /* [defaultvalue][in] */ DataTypeEnum Type,
            /* [defaultvalue][in] */ ParameterDirectionEnum Direction,
            /* [defaultvalue][in] */ long Size,
            /* [optional][in] */ VARIANT Value,
            /* [retval][out] */ _ADOParameter __RPC_FAR *__RPC_FAR *ppiprm) = 0;
        
        virtual /* [helpcontext][propget][id] */ HRESULT STDMETHODCALLTYPE get_Parameters( 
            /* [retval][out] */ ADOParameters __RPC_FAR *__RPC_FAR *ppvObject) = 0;
        
        virtual /* [helpcontext][propput][id] */ HRESULT STDMETHODCALLTYPE put_CommandType( 
            /* [in] */ CommandTypeEnum lCmdType) = 0;
        
        virtual /* [helpcontext][propget][id] */ HRESULT STDMETHODCALLTYPE get_CommandType( 
            /* [retval][out] */ CommandTypeEnum __RPC_FAR *plCmdType) = 0;
        
        virtual /* [helpcontext][propget][id] */ HRESULT STDMETHODCALLTYPE get_Name( 
            /* [retval][out] */ BSTR __RPC_FAR *pbstrName) = 0;
        
        virtual /* [helpcontext][propput][id] */ HRESULT STDMETHODCALLTYPE put_Name( 
            /* [in] */ BSTR bstrName) = 0;
        
    };
    
#else 	/* C style interface */
    typedef struct Command15Vtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            Command15 __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            Command15 __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            Command15 __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            Command15 __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            Command15 __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            Command15 __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            Command15 __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        /* [helpcontext][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Properties )( 
            Command15 __RPC_FAR * This,
            /* [retval][out] */ ADOProperties __RPC_FAR *__RPC_FAR *ppvObject);
        
        /* [helpcontext][propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_ActiveConnection )( 
            Command15 __RPC_FAR * This,
            /* [retval][out] */ _ADOConnection __RPC_FAR *__RPC_FAR *ppvObject);
        
        /* [helpcontext][propputref][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *putref_ActiveADOConnection )( 
            Command15 __RPC_FAR * This,
            /* [in] */ _ADOConnection __RPC_FAR *pCon);
        
        /* [helpcontext][propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_ActiveConnection )( 
            Command15 __RPC_FAR * This,
            /* [in] */ VARIANT vConn);
        
        /* [helpcontext][propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_CommandText )( 
            Command15 __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pbstr);
        
        /* [helpcontext][propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_CommandText )( 
            Command15 __RPC_FAR * This,
            /* [in] */ BSTR bstr);
        
        /* [helpcontext][propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_CommandTimeout )( 
            Command15 __RPC_FAR * This,
            /* [retval][out] */ LONG __RPC_FAR *pl);
        
        /* [helpcontext][propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_CommandTimeout )( 
            Command15 __RPC_FAR * This,
            /* [in] */ LONG Timeout);
        
        /* [helpcontext][propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Prepared )( 
            Command15 __RPC_FAR * This,
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pfPrepared);
        
        /* [helpcontext][propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Prepared )( 
            Command15 __RPC_FAR * This,
            /* [in] */ VARIANT_BOOL fPrepared);
        
        /* [helpcontext][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Execute )( 
            Command15 __RPC_FAR * This,
            /* [optional][out] */ VARIANT __RPC_FAR *RecordsAffected,
            /* [optional][in] */ VARIANT __RPC_FAR *Parameters,
            /* [defaultvalue][in] */ long Options,
            /* [retval][out] */ _ADORecordset __RPC_FAR *__RPC_FAR *ppirs);
        
        /* [helpcontext][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *CreateParameter )( 
            Command15 __RPC_FAR * This,
            /* [defaultvalue][in] */ BSTR Name,
            /* [defaultvalue][in] */ DataTypeEnum Type,
            /* [defaultvalue][in] */ ParameterDirectionEnum Direction,
            /* [defaultvalue][in] */ long Size,
            /* [optional][in] */ VARIANT Value,
            /* [retval][out] */ _ADOParameter __RPC_FAR *__RPC_FAR *ppiprm);
        
        /* [helpcontext][propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Parameters )( 
            Command15 __RPC_FAR * This,
            /* [retval][out] */ ADOParameters __RPC_FAR *__RPC_FAR *ppvObject);
        
        /* [helpcontext][propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_CommandType )( 
            Command15 __RPC_FAR * This,
            /* [in] */ CommandTypeEnum lCmdType);
        
        /* [helpcontext][propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_CommandType )( 
            Command15 __RPC_FAR * This,
            /* [retval][out] */ CommandTypeEnum __RPC_FAR *plCmdType);
        
        /* [helpcontext][propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Name )( 
            Command15 __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pbstrName);
        
        /* [helpcontext][propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Name )( 
            Command15 __RPC_FAR * This,
            /* [in] */ BSTR bstrName);
        
        END_INTERFACE
    } Command15Vtbl;
    interface Command15
    {
        CONST_VTBL struct Command15Vtbl __RPC_FAR *lpVtbl;
    };
    
#ifdef COBJMACROS
#define Command15_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)
#define Command15_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)
#define Command15_Release(This)	\
    (This)->lpVtbl -> Release(This)
#define Command15_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)
#define Command15_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)
#define Command15_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)
#define Command15_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)
#define Command15_get_Properties(This,ppvObject)	\
    (This)->lpVtbl -> get_Properties(This,ppvObject)
#define Command15_get_ActiveConnection(This,ppvObject)	\
    (This)->lpVtbl -> get_ActiveConnection(This,ppvObject)
#define Command15_putref_ActiveConnection(This,pCon)	\
    (This)->lpVtbl -> putref_ActiveConnection(This,pCon)
#define Command15_put_ActiveConnection(This,vConn)	\
    (This)->lpVtbl -> put_ActiveConnection(This,vConn)
#define Command15_get_CommandText(This,pbstr)	\
    (This)->lpVtbl -> get_CommandText(This,pbstr)
#define Command15_put_CommandText(This,bstr)	\
    (This)->lpVtbl -> put_CommandText(This,bstr)
#define Command15_get_CommandTimeout(This,pl)	\
    (This)->lpVtbl -> get_CommandTimeout(This,pl)
#define Command15_put_CommandTimeout(This,Timeout)	\
    (This)->lpVtbl -> put_CommandTimeout(This,Timeout)
#define Command15_get_Prepared(This,pfPrepared)	\
    (This)->lpVtbl -> get_Prepared(This,pfPrepared)
#define Command15_put_Prepared(This,fPrepared)	\
    (This)->lpVtbl -> put_Prepared(This,fPrepared)
#define Command15_Execute(This,RecordsAffected,Parameters,Options,ppirs)	\
    (This)->lpVtbl -> Execute(This,RecordsAffected,Parameters,Options,ppirs)
#define Command15_CreateParameter(This,Name,Type,Direction,Size,Value,ppiprm)	\
    (This)->lpVtbl -> CreateParameter(This,Name,Type,Direction,Size,Value,ppiprm)
#define Command15_get_Parameters(This,ppvObject)	\
    (This)->lpVtbl -> get_Parameters(This,ppvObject)
#define Command15_put_CommandType(This,lCmdType)	\
    (This)->lpVtbl -> put_CommandType(This,lCmdType)
#define Command15_get_CommandType(This,plCmdType)	\
    (This)->lpVtbl -> get_CommandType(This,plCmdType)
#define Command15_get_Name(This,pbstrName)	\
    (This)->lpVtbl -> get_Name(This,pbstrName)
#define Command15_put_Name(This,bstrName)	\
    (This)->lpVtbl -> put_Name(This,bstrName)
#endif /* COBJMACROS */
#endif 	/* C style interface */
/* [helpcontext][propget][id] */ HRESULT STDMETHODCALLTYPE Command15_get_ActiveConnection_Proxy( 
    Command15 __RPC_FAR * This,
    /* [retval][out] */ _ADOConnection __RPC_FAR *__RPC_FAR *ppvObject);
void __RPC_STUB Command15_get_ActiveConnection_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);
/* [helpcontext][propputref][id] */ HRESULT STDMETHODCALLTYPE Command15_putref_ActiveConnection_Proxy( 
    Command15 __RPC_FAR * This,
    /* [in] */ _ADOConnection __RPC_FAR *pCon);
void __RPC_STUB Command15_putref_ActiveConnection_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);
/* [helpcontext][propput][id] */ HRESULT STDMETHODCALLTYPE Command15_put_ActiveConnection_Proxy( 
    Command15 __RPC_FAR * This,
    /* [in] */ VARIANT vConn);
void __RPC_STUB Command15_put_ActiveConnection_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);
/* [helpcontext][propget][id] */ HRESULT STDMETHODCALLTYPE Command15_get_CommandText_Proxy( 
    Command15 __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pbstr);
void __RPC_STUB Command15_get_CommandText_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);
/* [helpcontext][propput][id] */ HRESULT STDMETHODCALLTYPE Command15_put_CommandText_Proxy( 
    Command15 __RPC_FAR * This,
    /* [in] */ BSTR bstr);
void __RPC_STUB Command15_put_CommandText_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);
/* [helpcontext][propget][id] */ HRESULT STDMETHODCALLTYPE Command15_get_CommandTimeout_Proxy( 
    Command15 __RPC_FAR * This,
    /* [retval][out] */ LONG __RPC_FAR *pl);
void __RPC_STUB Command15_get_CommandTimeout_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);
/* [helpcontext][propput][id] */ HRESULT STDMETHODCALLTYPE Command15_put_CommandTimeout_Proxy( 
    Command15 __RPC_FAR * This,
    /* [in] */ LONG Timeout);
void __RPC_STUB Command15_put_CommandTimeout_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);
/* [helpcontext][propget][id] */ HRESULT STDMETHODCALLTYPE Command15_get_Prepared_Proxy( 
    Command15 __RPC_FAR * This,
    /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pfPrepared);
void __RPC_STUB Command15_get_Prepared_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);
/* [helpcontext][propput][id] */ HRESULT STDMETHODCALLTYPE Command15_put_Prepared_Proxy( 
    Command15 __RPC_FAR * This,
    /* [in] */ VARIANT_BOOL fPrepared);
void __RPC_STUB Command15_put_Prepared_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);
/* [helpcontext][id] */ HRESULT STDMETHODCALLTYPE Command15_Execute_Proxy( 
    Command15 __RPC_FAR * This,
    /* [optional][out] */ VARIANT __RPC_FAR *RecordsAffected,
    /* [optional][in] */ VARIANT __RPC_FAR *Parameters,
    /* [defaultvalue][in] */ long Options,
    /* [retval][out] */ _ADORecordset __RPC_FAR *__RPC_FAR *ppirs);
void __RPC_STUB Command15_Execute_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);
/* [helpcontext][id] */ HRESULT STDMETHODCALLTYPE Command15_CreateParameter_Proxy( 
    Command15 __RPC_FAR * This,
    /* [defaultvalue][in] */ BSTR Name,
    /* [defaultvalue][in] */ DataTypeEnum Type,
    /* [defaultvalue][in] */ ParameterDirectionEnum Direction,
    /* [defaultvalue][in] */ long Size,
    /* [optional][in] */ VARIANT Value,
    /* [retval][out] */ _ADOParameter __RPC_FAR *__RPC_FAR *ppiprm);
void __RPC_STUB Command15_CreateParameter_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);
/* [helpcontext][propget][id] */ HRESULT STDMETHODCALLTYPE Command15_get_Parameters_Proxy( 
    Command15 __RPC_FAR * This,
    /* [retval][out] */ ADOParameters __RPC_FAR *__RPC_FAR *ppvObject);
void __RPC_STUB Command15_get_Parameters_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);
/* [helpcontext][propput][id] */ HRESULT STDMETHODCALLTYPE Command15_put_CommandType_Proxy( 
    Command15 __RPC_FAR * This,
    /* [in] */ CommandTypeEnum lCmdType);
void __RPC_STUB Command15_put_CommandType_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);
/* [helpcontext][propget][id] */ HRESULT STDMETHODCALLTYPE Command15_get_CommandType_Proxy( 
    Command15 __RPC_FAR * This,
    /* [retval][out] */ CommandTypeEnum __RPC_FAR *plCmdType);
void __RPC_STUB Command15_get_CommandType_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);
/* [helpcontext][propget][id] */ HRESULT STDMETHODCALLTYPE Command15_get_Name_Proxy( 
    Command15 __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pbstrName);
void __RPC_STUB Command15_get_Name_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);
/* [helpcontext][propput][id] */ HRESULT STDMETHODCALLTYPE Command15_put_Name_Proxy( 
    Command15 __RPC_FAR * This,
    /* [in] */ BSTR bstrName);
void __RPC_STUB Command15_put_Name_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);
#endif 	/* __Command15_INTERFACE_DEFINED__ */
#ifndef __Command25_INTERFACE_DEFINED__
#define __Command25_INTERFACE_DEFINED__
/* interface Command25 */
/* [object][helpcontext][uuid][hidden][nonextensible][dual] */ 
EXTERN_C const IID IID_Command25;
#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("0000054E-0000-0010-8000-00AA006D2EA4")
    Command25 : public Command15
    {
    public:
        virtual /* [helpcontext][propget][id] */ HRESULT STDMETHODCALLTYPE get_State( 
            /* [retval][out] */ LONG __RPC_FAR *plObjState) = 0;
        
        virtual /* [helpcontext][id] */ HRESULT STDMETHODCALLTYPE Cancel( void) = 0;
        
    };
    
#else 	/* C style interface */
    typedef struct Command25Vtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            Command25 __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            Command25 __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            Command25 __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            Command25 __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            Command25 __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            Command25 __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            Command25 __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        /* [helpcontext][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Properties )( 
            Command25 __RPC_FAR * This,
            /* [retval][out] */ ADOProperties __RPC_FAR *__RPC_FAR *ppvObject);
        
        /* [helpcontext][propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_ActiveConnection )( 
            Command25 __RPC_FAR * This,
            /* [retval][out] */ _ADOConnection __RPC_FAR *__RPC_FAR *ppvObject);
        
        /* [helpcontext][propputref][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *putref_ActiveADOConnection )( 
            Command25 __RPC_FAR * This,
            /* [in] */ _ADOConnection __RPC_FAR *pCon);
        
        /* [helpcontext][propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_ActiveConnection )( 
            Command25 __RPC_FAR * This,
            /* [in] */ VARIANT vConn);
        
        /* [helpcontext][propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_CommandText )( 
            Command25 __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pbstr);
        
        /* [helpcontext][propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_CommandText )( 
            Command25 __RPC_FAR * This,
            /* [in] */ BSTR bstr);
        
        /* [helpcontext][propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_CommandTimeout )( 
            Command25 __RPC_FAR * This,
            /* [retval][out] */ LONG __RPC_FAR *pl);
        
        /* [helpcontext][propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_CommandTimeout )( 
            Command25 __RPC_FAR * This,
            /* [in] */ LONG Timeout);
        
        /* [helpcontext][propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Prepared )( 
            Command25 __RPC_FAR * This,
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pfPrepared);
        
        /* [helpcontext][propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Prepared )( 
            Command25 __RPC_FAR * This,
            /* [in] */ VARIANT_BOOL fPrepared);
        
        /* [helpcontext][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Execute )( 
            Command25 __RPC_FAR * This,
            /* [optional][out] */ VARIANT __RPC_FAR *RecordsAffected,
            /* [optional][in] */ VARIANT __RPC_FAR *Parameters,
            /* [defaultvalue][in] */ long Options,
            /* [retval][out] */ _ADORecordset __RPC_FAR *__RPC_FAR *ppirs);
        
        /* [helpcontext][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *CreateParameter )( 
            Command25 __RPC_FAR * This,
            /* [defaultvalue][in] */ BSTR Name,
            /* [defaultvalue][in] */ DataTypeEnum Type,
            /* [defaultvalue][in] */ ParameterDirectionEnum Direction,
            /* [defaultvalue][in] */ long Size,
            /* [optional][in] */ VARIANT Value,
            /* [retval][out] */ _ADOParameter __RPC_FAR *__RPC_FAR *ppiprm);
        
        /* [helpcontext][propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Parameters )( 
            Command25 __RPC_FAR * This,
            /* [retval][out] */ ADOParameters __RPC_FAR *__RPC_FAR *ppvObject);
        
        /* [helpcontext][propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_CommandType )( 
            Command25 __RPC_FAR * This,
            /* [in] */ CommandTypeEnum lCmdType);
        
        /* [helpcontext][propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_CommandType )( 
            Command25 __RPC_FAR * This,
            /* [retval][out] */ CommandTypeEnum __RPC_FAR *plCmdType);
        
        /* [helpcontext][propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Name )( 
            Command25 __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pbstrName);
        
        /* [helpcontext][propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Name )( 
            Command25 __RPC_FAR * This,
            /* [in] */ BSTR bstrName);
        
        /* [helpcontext][propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_State )( 
            Command25 __RPC_FAR * This,
            /* [retval][out] */ LONG __RPC_FAR *plObjState);
        
        /* [helpcontext][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Cancel )( 
            Command25 __RPC_FAR * This);
        
        END_INTERFACE
    } Command25Vtbl;
    interface Command25
    {
        CONST_VTBL struct Command25Vtbl __RPC_FAR *lpVtbl;
    };
    
#ifdef COBJMACROS
#define Command25_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)
#define Command25_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)
#define Command25_Release(This)	\
    (This)->lpVtbl -> Release(This)
#define Command25_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)
#define Command25_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)
#define Command25_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)
#define Command25_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)
#define Command25_get_Properties(This,ppvObject)	\
    (This)->lpVtbl -> get_Properties(This,ppvObject)
#define Command25_get_ActiveConnection(This,ppvObject)	\
    (This)->lpVtbl -> get_ActiveConnection(This,ppvObject)
#define Command25_putref_ActiveConnection(This,pCon)	\
    (This)->lpVtbl -> putref_ActiveConnection(This,pCon)
#define Command25_put_ActiveConnection(This,vConn)	\
    (This)->lpVtbl -> put_ActiveConnection(This,vConn)
#define Command25_get_CommandText(This,pbstr)	\
    (This)->lpVtbl -> get_CommandText(This,pbstr)
#define Command25_put_CommandText(This,bstr)	\
    (This)->lpVtbl -> put_CommandText(This,bstr)
#define Command25_get_CommandTimeout(This,pl)	\
    (This)->lpVtbl -> get_CommandTimeout(This,pl)
#define Command25_put_CommandTimeout(This,Timeout)	\
    (This)->lpVtbl -> put_CommandTimeout(This,Timeout)
#define Command25_get_Prepared(This,pfPrepared)	\
    (This)->lpVtbl -> get_Prepared(This,pfPrepared)
#define Command25_put_Prepared(This,fPrepared)	\
    (This)->lpVtbl -> put_Prepared(This,fPrepared)
#define Command25_Execute(This,RecordsAffected,Parameters,Options,ppirs)	\
    (This)->lpVtbl -> Execute(This,RecordsAffected,Parameters,Options,ppirs)
#define Command25_CreateParameter(This,Name,Type,Direction,Size,Value,ppiprm)	\
    (This)->lpVtbl -> CreateParameter(This,Name,Type,Direction,Size,Value,ppiprm)
#define Command25_get_Parameters(This,ppvObject)	\
    (This)->lpVtbl -> get_Parameters(This,ppvObject)
#define Command25_put_CommandType(This,lCmdType)	\
    (This)->lpVtbl -> put_CommandType(This,lCmdType)
#define Command25_get_CommandType(This,plCmdType)	\
    (This)->lpVtbl -> get_CommandType(This,plCmdType)
#define Command25_get_Name(This,pbstrName)	\
    (This)->lpVtbl -> get_Name(This,pbstrName)
#define Command25_put_Name(This,bstrName)	\
    (This)->lpVtbl -> put_Name(This,bstrName)
#define Command25_get_State(This,plObjState)	\
    (This)->lpVtbl -> get_State(This,plObjState)
#define Command25_Cancel(This)	\
    (This)->lpVtbl -> Cancel(This)
#endif /* COBJMACROS */
#endif 	/* C style interface */
/* [helpcontext][propget][id] */ HRESULT STDMETHODCALLTYPE Command25_get_State_Proxy( 
    Command25 __RPC_FAR * This,
    /* [retval][out] */ LONG __RPC_FAR *plObjState);
void __RPC_STUB Command25_get_State_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);
/* [helpcontext][id] */ HRESULT STDMETHODCALLTYPE Command25_Cancel_Proxy( 
    Command25 __RPC_FAR * This);
void __RPC_STUB Command25_Cancel_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);
#endif 	/* __Command25_INTERFACE_DEFINED__ */
#ifndef ___Command_INTERFACE_DEFINED__
#define ___Command_INTERFACE_DEFINED__
/* interface _ADOCommand */
/* [object][helpcontext][uuid][nonextensible][dual] */ 
EXTERN_C const IID IID__Command;
#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("B08400BD-F9D1-4D02-B856-71D5DBA123E9")
    _ADOCommand : public Command25
    {
    public:
        virtual /* [helpcontext][propputref][id] */ HRESULT __stdcall putref_CommandStream( 
            /* [in] */ IUnknown __RPC_FAR *pStream) = 0;
        
        virtual /* [helpcontext][propget][id] */ HRESULT __stdcall get_CommandStream( 
            /* [retval][out] */ VARIANT __RPC_FAR *pvStream) = 0;
        
        virtual /* [helpcontext][propput][id] */ HRESULT __stdcall put_Dialect( 
            /* [in] */ BSTR bstrDialect) = 0;
        
        virtual /* [helpcontext][propget][id] */ HRESULT __stdcall get_Dialect( 
            /* [retval][out] */ BSTR __RPC_FAR *pbstrDialect) = 0;
        
        virtual /* [helpcontext][propput][id] */ HRESULT __stdcall put_NamedParameters( 
            /* [in] */ VARIANT_BOOL fNamedParameters) = 0;
        
        virtual /* [helpcontext][propget][id] */ HRESULT __stdcall get_NamedParameters( 
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pfNamedParameters) = 0;
        
    };
    
#else 	/* C style interface */
    typedef struct _CommandVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            _ADOCommand __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            _ADOCommand __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            _ADOCommand __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            _ADOCommand __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            _ADOCommand __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            _ADOCommand __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            _ADOCommand __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        /* [helpcontext][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Properties )( 
            _ADOCommand __RPC_FAR * This,
            /* [retval][out] */ ADOProperties __RPC_FAR *__RPC_FAR *ppvObject);
        
        /* [helpcontext][propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_ActiveConnection )( 
            _ADOCommand __RPC_FAR * This,
            /* [retval][out] */ _ADOConnection __RPC_FAR *__RPC_FAR *ppvObject);
        
        /* [helpcontext][propputref][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *putref_ActiveADOConnection )( 
            _ADOCommand __RPC_FAR * This,
            /* [in] */ _ADOConnection __RPC_FAR *pCon);
        
        /* [helpcontext][propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_ActiveConnection )( 
            _ADOCommand __RPC_FAR * This,
            /* [in] */ VARIANT vConn);
        
        /* [helpcontext][propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_CommandText )( 
            _ADOCommand __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pbstr);
        
        /* [helpcontext][propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_CommandText )( 
            _ADOCommand __RPC_FAR * This,
            /* [in] */ BSTR bstr);
        
        /* [helpcontext][propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_CommandTimeout )( 
            _ADOCommand __RPC_FAR * This,
            /* [retval][out] */ LONG __RPC_FAR *pl);
        
        /* [helpcontext][propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_CommandTimeout )( 
            _ADOCommand __RPC_FAR * This,
            /* [in] */ LONG Timeout);
        
        /* [helpcontext][propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Prepared )( 
            _ADOCommand __RPC_FAR * This,
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pfPrepared);
        
        /* [helpcontext][propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Prepared )( 
            _ADOCommand __RPC_FAR * This,
            /* [in] */ VARIANT_BOOL fPrepared);
        
        /* [helpcontext][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Execute )( 
            _ADOCommand __RPC_FAR * This,
            /* [optional][out] */ VARIANT __RPC_FAR *RecordsAffected,
            /* [optional][in] */ VARIANT __RPC_FAR *Parameters,
            /* [defaultvalue][in] */ long Options,
            /* [retval][out] */ _ADORecordset __RPC_FAR *__RPC_FAR *ppirs);
        
        /* [helpcontext][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *CreateParameter )( 
            _ADOCommand __RPC_FAR * This,
            /* [defaultvalue][in] */ BSTR Name,
            /* [defaultvalue][in] */ DataTypeEnum Type,
            /* [defaultvalue][in] */ ParameterDirectionEnum Direction,
            /* [defaultvalue][in] */ long Size,
            /* [optional][in] */ VARIANT Value,
            /* [retval][out] */ _ADOParameter __RPC_FAR *__RPC_FAR *ppiprm);
        
        /* [helpcontext][propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Parameters )( 
            _ADOCommand __RPC_FAR * This,
            /* [retval][out] */ ADOParameters __RPC_FAR *__RPC_FAR *ppvObject);
        
        /* [helpcontext][propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_CommandType )( 
            _ADOCommand __RPC_FAR * This,
            /* [in] */ CommandTypeEnum lCmdType);
        
        /* [helpcontext][propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_CommandType )( 
            _ADOCommand __RPC_FAR * This,
            /* [retval][out] */ CommandTypeEnum __RPC_FAR *plCmdType);
        
        /* [helpcontext][propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Name )( 
            _ADOCommand __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pbstrName);
        
        /* [helpcontext][propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Name )( 
            _ADOCommand __RPC_FAR * This,
            /* [in] */ BSTR bstrName);
        
        /* [helpcontext][propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_State )( 
            _ADOCommand __RPC_FAR * This,
            /* [retval][out] */ LONG __RPC_FAR *plObjState);
        
        /* [helpcontext][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Cancel )( 
            _ADOCommand __RPC_FAR * This);
        
        /* [helpcontext][propputref][id] */ HRESULT ( __stdcall __RPC_FAR *putref_CommandADOStream )( 
            _ADOCommand __RPC_FAR * This,
            /* [in] */ IUnknown __RPC_FAR *pStream);
        
        /* [helpcontext][propget][id] */ HRESULT ( __stdcall __RPC_FAR *get_CommandStream )( 
            _ADOCommand __RPC_FAR * This,
            /* [retval][out] */ VARIANT __RPC_FAR *pvStream);
        
        /* [helpcontext][propput][id] */ HRESULT ( __stdcall __RPC_FAR *put_Dialect )( 
            _ADOCommand __RPC_FAR * This,
            /* [in] */ BSTR bstrDialect);
        
        /* [helpcontext][propget][id] */ HRESULT ( __stdcall __RPC_FAR *get_Dialect )( 
            _ADOCommand __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pbstrDialect);
        
        /* [helpcontext][propput][id] */ HRESULT ( __stdcall __RPC_FAR *put_NamedParameters )( 
            _ADOCommand __RPC_FAR * This,
            /* [in] */ VARIANT_BOOL fNamedParameters);
        
        /* [helpcontext][propget][id] */ HRESULT ( __stdcall __RPC_FAR *get_NamedParameters )( 
            _ADOCommand __RPC_FAR * This,
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pfNamedParameters);
        
        END_INTERFACE
    } _CommandVtbl;
    interface _Command
    {
        CONST_VTBL struct _CommandVtbl __RPC_FAR *lpVtbl;
    };
    
#ifdef COBJMACROS
#define _Command_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)
#define _Command_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)
#define _Command_Release(This)	\
    (This)->lpVtbl -> Release(This)
#define _Command_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)
#define _Command_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)
#define _Command_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)
#define _Command_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)
#define _Command_get_Properties(This,ppvObject)	\
    (This)->lpVtbl -> get_Properties(This,ppvObject)
#define _Command_get_ActiveConnection(This,ppvObject)	\
    (This)->lpVtbl -> get_ActiveConnection(This,ppvObject)
#define _Command_putref_ActiveConnection(This,pCon)	\
    (This)->lpVtbl -> putref_ActiveConnection(This,pCon)
#define _Command_put_ActiveConnection(This,vConn)	\
    (This)->lpVtbl -> put_ActiveConnection(This,vConn)
#define _Command_get_CommandText(This,pbstr)	\
    (This)->lpVtbl -> get_CommandText(This,pbstr)
#define _Command_put_CommandText(This,bstr)	\
    (This)->lpVtbl -> put_CommandText(This,bstr)
#define _Command_get_CommandTimeout(This,pl)	\
    (This)->lpVtbl -> get_CommandTimeout(This,pl)
#define _Command_put_CommandTimeout(This,Timeout)	\
    (This)->lpVtbl -> put_CommandTimeout(This,Timeout)
#define _Command_get_Prepared(This,pfPrepared)	\
    (This)->lpVtbl -> get_Prepared(This,pfPrepared)
#define _Command_put_Prepared(This,fPrepared)	\
    (This)->lpVtbl -> put_Prepared(This,fPrepared)
#define _Command_Execute(This,RecordsAffected,Parameters,Options,ppirs)	\
    (This)->lpVtbl -> Execute(This,RecordsAffected,Parameters,Options,ppirs)
#define _Command_CreateParameter(This,Name,Type,Direction,Size,Value,ppiprm)	\
    (This)->lpVtbl -> CreateParameter(This,Name,Type,Direction,Size,Value,ppiprm)
#define _Command_get_Parameters(This,ppvObject)	\
    (This)->lpVtbl -> get_Parameters(This,ppvObject)
#define _Command_put_CommandType(This,lCmdType)	\
    (This)->lpVtbl -> put_CommandType(This,lCmdType)
#define _Command_get_CommandType(This,plCmdType)	\
    (This)->lpVtbl -> get_CommandType(This,plCmdType)
#define _Command_get_Name(This,pbstrName)	\
    (This)->lpVtbl -> get_Name(This,pbstrName)
#define _Command_put_Name(This,bstrName)	\
    (This)->lpVtbl -> put_Name(This,bstrName)
#define _Command_get_State(This,plObjState)	\
    (This)->lpVtbl -> get_State(This,plObjState)
#define _Command_Cancel(This)	\
    (This)->lpVtbl -> Cancel(This)
#define _Command_putref_CommandStream(This,pStream)	\
    (This)->lpVtbl -> putref_CommandStream(This,pStream)
#define _Command_get_CommandStream(This,pvStream)	\
    (This)->lpVtbl -> get_CommandStream(This,pvStream)
#define _Command_put_Dialect(This,bstrDialect)	\
    (This)->lpVtbl -> put_Dialect(This,bstrDialect)
#define _Command_get_Dialect(This,pbstrDialect)	\
    (This)->lpVtbl -> get_Dialect(This,pbstrDialect)
#define _Command_put_NamedParameters(This,fNamedParameters)	\
    (This)->lpVtbl -> put_NamedParameters(This,fNamedParameters)
#define _Command_get_NamedParameters(This,pfNamedParameters)	\
    (This)->lpVtbl -> get_NamedParameters(This,pfNamedParameters)
#endif /* COBJMACROS */
#endif 	/* C style interface */
/* [helpcontext][propputref][id] */ HRESULT __stdcall _Command_putref_CommandStream_Proxy( 
    _ADOCommand __RPC_FAR * This,
    /* [in] */ IUnknown __RPC_FAR *pStream);
void __RPC_STUB _Command_putref_CommandStream_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);
/* [helpcontext][propget][id] */ HRESULT __stdcall _Command_get_CommandStream_Proxy( 
    _ADOCommand __RPC_FAR * This,
    /* [retval][out] */ VARIANT __RPC_FAR *pvStream);
void __RPC_STUB _Command_get_CommandStream_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);
/* [helpcontext][propput][id] */ HRESULT __stdcall _Command_put_Dialect_Proxy( 
    _ADOCommand __RPC_FAR * This,
    /* [in] */ BSTR bstrDialect);
void __RPC_STUB _Command_put_Dialect_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);
/* [helpcontext][propget][id] */ HRESULT __stdcall _Command_get_Dialect_Proxy( 
    _ADOCommand __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pbstrDialect);
void __RPC_STUB _Command_get_Dialect_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);
/* [helpcontext][propput][id] */ HRESULT __stdcall _Command_put_NamedParameters_Proxy( 
    _ADOCommand __RPC_FAR * This,
    /* [in] */ VARIANT_BOOL fNamedParameters);
void __RPC_STUB _Command_put_NamedParameters_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);
/* [helpcontext][propget][id] */ HRESULT __stdcall _Command_get_NamedParameters_Proxy( 
    _ADOCommand __RPC_FAR * This,
    /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pfNamedParameters);
void __RPC_STUB _Command_get_NamedParameters_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);
#endif 	/* ___Command_INTERFACE_DEFINED__ */
#ifndef __ConnectionEventsVt_INTERFACE_DEFINED__
#define __ConnectionEventsVt_INTERFACE_DEFINED__
/* interface ConnectionEventsVt */
/* [object][uuid][hidden] */ 
EXTERN_C const IID IID_ConnectionEventsVt;
#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("00000402-0000-0010-8000-00AA006D2EA4")
    ConnectionEventsVt : public IUnknown
    {
    public:
        virtual /* [helpcontext][id] */ HRESULT STDMETHODCALLTYPE InfoMessage( 
            /* [in] */ ADOError __RPC_FAR *pError,
            /* [out][in] */ EventStatusEnum __RPC_FAR *adStatus,
            /* [in] */ _ADOConnection __RPC_FAR *pConnection) = 0;
        
        virtual /* [helpcontext][id] */ HRESULT STDMETHODCALLTYPE BeginTransComplete( 
            /* [in] */ LONG TransactionLevel,
            /* [in] */ ADOError __RPC_FAR *pError,
            /* [out][in] */ EventStatusEnum __RPC_FAR *adStatus,
            /* [in] */ _ADOConnection __RPC_FAR *pConnection) = 0;
        
        virtual /* [helpcontext][id] */ HRESULT STDMETHODCALLTYPE CommitTransComplete( 
            /* [in] */ ADOError __RPC_FAR *pError,
            /* [out][in] */ EventStatusEnum __RPC_FAR *adStatus,
            /* [in] */ _ADOConnection __RPC_FAR *pConnection) = 0;
        
        virtual /* [helpcontext][id] */ HRESULT STDMETHODCALLTYPE RollbackTransComplete( 
            /* [in] */ ADOError __RPC_FAR *pError,
            /* [out][in] */ EventStatusEnum __RPC_FAR *adStatus,
            /* [in] */ _ADOConnection __RPC_FAR *pConnection) = 0;
        
        virtual /* [helpcontext][id] */ HRESULT STDMETHODCALLTYPE WillExecute( 
            /* [out][in] */ BSTR __RPC_FAR *Source,
            /* [out][in] */ CursorTypeEnum __RPC_FAR *CursorType,
            /* [out][in] */ LockTypeEnum __RPC_FAR *LockType,
            /* [out][in] */ long __RPC_FAR *Options,
            /* [out][in] */ EventStatusEnum __RPC_FAR *adStatus,
            /* [in] */ _ADOCommand __RPC_FAR *pCommand,
            /* [in] */ _ADORecordset __RPC_FAR *pRecordset,
            /* [in] */ _ADOConnection __RPC_FAR *pConnection) = 0;
        
        virtual /* [helpcontext][id] */ HRESULT STDMETHODCALLTYPE ExecuteComplete( 
            /* [in] */ LONG RecordsAffected,
            /* [in] */ ADOError __RPC_FAR *pError,
            /* [out][in] */ EventStatusEnum __RPC_FAR *adStatus,
            /* [in] */ _ADOCommand __RPC_FAR *pCommand,
            /* [in] */ _ADORecordset __RPC_FAR *pRecordset,
            /* [in] */ _ADOConnection __RPC_FAR *pConnection) = 0;
        
        virtual /* [helpcontext][id] */ HRESULT STDMETHODCALLTYPE WillConnect( 
            /* [out][in] */ BSTR __RPC_FAR *ConnectionString,
            /* [out][in] */ BSTR __RPC_FAR *UserID,
            /* [out][in] */ BSTR __RPC_FAR *Password,
            /* [out][in] */ long __RPC_FAR *Options,
            /* [out][in] */ EventStatusEnum __RPC_FAR *adStatus,
            /* [in] */ _ADOConnection __RPC_FAR *pConnection) = 0;
        
        virtual /* [helpcontext][id] */ HRESULT STDMETHODCALLTYPE ConnectComplete( 
            /* [in] */ ADOError __RPC_FAR *pError,
            /* [out][in] */ EventStatusEnum __RPC_FAR *adStatus,
            /* [in] */ _ADOConnection __RPC_FAR *pConnection) = 0;
        
        virtual /* [helpcontext][id] */ HRESULT STDMETHODCALLTYPE Disconnect( 
            /* [out][in] */ EventStatusEnum __RPC_FAR *adStatus,
            /* [in] */ _ADOConnection __RPC_FAR *pConnection) = 0;
        
    };
    
#else 	/* C style interface */
    typedef struct ConnectionEventsVtVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            ConnectionEventsVt __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            ConnectionEventsVt __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            ConnectionEventsVt __RPC_FAR * This);
        
        /* [helpcontext][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *InfoMessage )( 
            ConnectionEventsVt __RPC_FAR * This,
            /* [in] */ ADOError __RPC_FAR *pError,
            /* [out][in] */ EventStatusEnum __RPC_FAR *adStatus,
            /* [in] */ _ADOConnection __RPC_FAR *pConnection);
        
        /* [helpcontext][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *BeginTransComplete )( 
            ConnectionEventsVt __RPC_FAR * This,
            /* [in] */ LONG TransactionLevel,
            /* [in] */ ADOError __RPC_FAR *pError,
            /* [out][in] */ EventStatusEnum __RPC_FAR *adStatus,
            /* [in] */ _ADOConnection __RPC_FAR *pConnection);
        
        /* [helpcontext][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *CommitTransComplete )( 
            ConnectionEventsVt __RPC_FAR * This,
            /* [in] */ ADOError __RPC_FAR *pError,
            /* [out][in] */ EventStatusEnum __RPC_FAR *adStatus,
            /* [in] */ _ADOConnection __RPC_FAR *pConnection);
        
        /* [helpcontext][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *RollbackTransComplete )( 
            ConnectionEventsVt __RPC_FAR * This,
            /* [in] */ ADOError __RPC_FAR *pError,
            /* [out][in] */ EventStatusEnum __RPC_FAR *adStatus,
            /* [in] */ _ADOConnection __RPC_FAR *pConnection);
        
        /* [helpcontext][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *WillExecute )( 
            ConnectionEventsVt __RPC_FAR * This,
            /* [out][in] */ BSTR __RPC_FAR *Source,
            /* [out][in] */ CursorTypeEnum __RPC_FAR *CursorType,
            /* [out][in] */ LockTypeEnum __RPC_FAR *LockType,
            /* [out][in] */ long __RPC_FAR *Options,
            /* [out][in] */ EventStatusEnum __RPC_FAR *adStatus,
            /* [in] */ _ADOCommand __RPC_FAR *pCommand,
            /* [in] */ _ADORecordset __RPC_FAR *pRecordset,
            /* [in] */ _ADOConnection __RPC_FAR *pConnection);
        
        /* [helpcontext][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *ExecuteComplete )( 
            ConnectionEventsVt __RPC_FAR * This,
            /* [in] */ LONG RecordsAffected,
            /* [in] */ ADOError __RPC_FAR *pError,
            /* [out][in] */ EventStatusEnum __RPC_FAR *adStatus,
            /* [in] */ _ADOCommand __RPC_FAR *pCommand,
            /* [in] */ _ADORecordset __RPC_FAR *pRecordset,
            /* [in] */ _ADOConnection __RPC_FAR *pConnection);
        
        /* [helpcontext][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *WillConnect )( 
            ConnectionEventsVt __RPC_FAR * This,
            /* [out][in] */ BSTR __RPC_FAR *ConnectionString,
            /* [out][in] */ BSTR __RPC_FAR *UserID,
            /* [out][in] */ BSTR __RPC_FAR *Password,
            /* [out][in] */ long __RPC_FAR *Options,
            /* [out][in] */ EventStatusEnum __RPC_FAR *adStatus,
            /* [in] */ _ADOConnection __RPC_FAR *pConnection);
        
        /* [helpcontext][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *ConnectComplete )( 
            ConnectionEventsVt __RPC_FAR * This,
            /* [in] */ ADOError __RPC_FAR *pError,
            /* [out][in] */ EventStatusEnum __RPC_FAR *adStatus,
            /* [in] */ _ADOConnection __RPC_FAR *pConnection);
        
        /* [helpcontext][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Disconnect )( 
            ConnectionEventsVt __RPC_FAR * This,
            /* [out][in] */ EventStatusEnum __RPC_FAR *adStatus,
            /* [in] */ _ADOConnection __RPC_FAR *pConnection);
        
        END_INTERFACE
    } ConnectionEventsVtVtbl;
    interface ConnectionEventsVt
    {
        CONST_VTBL struct ConnectionEventsVtVtbl __RPC_FAR *lpVtbl;
    };
    
#ifdef COBJMACROS
#define ConnectionEventsVt_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)
#define ConnectionEventsVt_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)
#define ConnectionEventsVt_Release(This)	\
    (This)->lpVtbl -> Release(This)
#define ConnectionEventsVt_InfoMessage(This,pError,adStatus,pConnection)	\
    (This)->lpVtbl -> InfoMessage(This,pError,adStatus,pConnection)
#define ConnectionEventsVt_BeginTransComplete(This,TransactionLevel,pError,adStatus,pConnection)	\
    (This)->lpVtbl -> BeginTransComplete(This,TransactionLevel,pError,adStatus,pConnection)
#define ConnectionEventsVt_CommitTransComplete(This,pError,adStatus,pConnection)	\
    (This)->lpVtbl -> CommitTransComplete(This,pError,adStatus,pConnection)
#define ConnectionEventsVt_RollbackTransComplete(This,pError,adStatus,pConnection)	\
    (This)->lpVtbl -> RollbackTransComplete(This,pError,adStatus,pConnection)
#define ConnectionEventsVt_WillExecute(This,Source,CursorType,LockType,Options,adStatus,pCommand,pRecordset,pConnection)	\
    (This)->lpVtbl -> WillExecute(This,Source,CursorType,LockType,Options,adStatus,pCommand,pRecordset,pConnection)
#define ConnectionEventsVt_ExecuteComplete(This,RecordsAffected,pError,adStatus,pCommand,pRecordset,pConnection)	\
    (This)->lpVtbl -> ExecuteComplete(This,RecordsAffected,pError,adStatus,pCommand,pRecordset,pConnection)
#define ConnectionEventsVt_WillConnect(This,ConnectionString,UserID,Password,Options,adStatus,pConnection)	\
    (This)->lpVtbl -> WillConnect(This,ConnectionString,UserID,Password,Options,adStatus,pConnection)
#define ConnectionEventsVt_ConnectComplete(This,pError,adStatus,pConnection)	\
    (This)->lpVtbl -> ConnectComplete(This,pError,adStatus,pConnection)
#define ConnectionEventsVt_Disconnect(This,adStatus,pConnection)	\
    (This)->lpVtbl -> Disconnect(This,adStatus,pConnection)
#endif /* COBJMACROS */
#endif 	/* C style interface */
/* [helpcontext][id] */ HRESULT STDMETHODCALLTYPE ConnectionEventsVt_InfoMessage_Proxy( 
    ConnectionEventsVt __RPC_FAR * This,
    /* [in] */ ADOError __RPC_FAR *pError,
    /* [out][in] */ EventStatusEnum __RPC_FAR *adStatus,
    /* [in] */ _ADOConnection __RPC_FAR *pConnection);
void __RPC_STUB ConnectionEventsVt_InfoMessage_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);
/* [helpcontext][id] */ HRESULT STDMETHODCALLTYPE ConnectionEventsVt_BeginTransComplete_Proxy( 
    ConnectionEventsVt __RPC_FAR * This,
    /* [in] */ LONG TransactionLevel,
    /* [in] */ ADOError __RPC_FAR *pError,
    /* [out][in] */ EventStatusEnum __RPC_FAR *adStatus,
    /* [in] */ _ADOConnection __RPC_FAR *pConnection);
void __RPC_STUB ConnectionEventsVt_BeginTransComplete_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);
/* [helpcontext][id] */ HRESULT STDMETHODCALLTYPE ConnectionEventsVt_CommitTransComplete_Proxy( 
    ConnectionEventsVt __RPC_FAR * This,
    /* [in] */ ADOError __RPC_FAR *pError,
    /* [out][in] */ EventStatusEnum __RPC_FAR *adStatus,
    /* [in] */ _ADOConnection __RPC_FAR *pConnection);
void __RPC_STUB ConnectionEventsVt_CommitTransComplete_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);
/* [helpcontext][id] */ HRESULT STDMETHODCALLTYPE ConnectionEventsVt_RollbackTransComplete_Proxy( 
    ConnectionEventsVt __RPC_FAR * This,
    /* [in] */ ADOError __RPC_FAR *pError,
    /* [out][in] */ EventStatusEnum __RPC_FAR *adStatus,
    /* [in] */ _ADOConnection __RPC_FAR *pConnection);
void __RPC_STUB ConnectionEventsVt_RollbackTransComplete_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);
/* [helpcontext][id] */ HRESULT STDMETHODCALLTYPE ConnectionEventsVt_WillExecute_Proxy( 
    ConnectionEventsVt __RPC_FAR * This,
    /* [out][in] */ BSTR __RPC_FAR *Source,
    /* [out][in] */ CursorTypeEnum __RPC_FAR *CursorType,
    /* [out][in] */ LockTypeEnum __RPC_FAR *LockType,
    /* [out][in] */ long __RPC_FAR *Options,
    /* [out][in] */ EventStatusEnum __RPC_FAR *adStatus,
    /* [in] */ _ADOCommand __RPC_FAR *pCommand,
    /* [in] */ _ADORecordset __RPC_FAR *pRecordset,
    /* [in] */ _ADOConnection __RPC_FAR *pConnection);
void __RPC_STUB ConnectionEventsVt_WillExecute_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);
/* [helpcontext][id] */ HRESULT STDMETHODCALLTYPE ConnectionEventsVt_ExecuteComplete_Proxy( 
    ConnectionEventsVt __RPC_FAR * This,
    /* [in] */ LONG RecordsAffected,
    /* [in] */ ADOError __RPC_FAR *pError,
    /* [out][in] */ EventStatusEnum __RPC_FAR *adStatus,
    /* [in] */ _ADOCommand __RPC_FAR *pCommand,
    /* [in] */ _ADORecordset __RPC_FAR *pRecordset,
    /* [in] */ _ADOConnection __RPC_FAR *pConnection);
void __RPC_STUB ConnectionEventsVt_ExecuteComplete_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);
/* [helpcontext][id] */ HRESULT STDMETHODCALLTYPE ConnectionEventsVt_WillConnect_Proxy( 
    ConnectionEventsVt __RPC_FAR * This,
    /* [out][in] */ BSTR __RPC_FAR *ConnectionString,
    /* [out][in] */ BSTR __RPC_FAR *UserID,
    /* [out][in] */ BSTR __RPC_FAR *Password,
    /* [out][in] */ long __RPC_FAR *Options,
    /* [out][in] */ EventStatusEnum __RPC_FAR *adStatus,
    /* [in] */ _ADOConnection __RPC_FAR *pConnection);
void __RPC_STUB ConnectionEventsVt_WillConnect_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);
/* [helpcontext][id] */ HRESULT STDMETHODCALLTYPE ConnectionEventsVt_ConnectComplete_Proxy( 
    ConnectionEventsVt __RPC_FAR * This,
    /* [in] */ ADOError __RPC_FAR *pError,
    /* [out][in] */ EventStatusEnum __RPC_FAR *adStatus,
    /* [in] */ _ADOConnection __RPC_FAR *pConnection);
void __RPC_STUB ConnectionEventsVt_ConnectComplete_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);
/* [helpcontext][id] */ HRESULT STDMETHODCALLTYPE ConnectionEventsVt_Disconnect_Proxy( 
    ConnectionEventsVt __RPC_FAR * This,
    /* [out][in] */ EventStatusEnum __RPC_FAR *adStatus,
    /* [in] */ _ADOConnection __RPC_FAR *pConnection);
void __RPC_STUB ConnectionEventsVt_Disconnect_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);
#endif 	/* __ConnectionEventsVt_INTERFACE_DEFINED__ */
#ifndef __RecordsetEventsVt_INTERFACE_DEFINED__
#define __RecordsetEventsVt_INTERFACE_DEFINED__
/* interface RecordsetEventsVt */
/* [object][uuid][hidden] */ 
EXTERN_C const IID IID_RecordsetEventsVt;
#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("00000403-0000-0010-8000-00AA006D2EA4")
    RecordsetEventsVt : public IUnknown
    {
    public:
        virtual /* [helpcontext][id] */ HRESULT STDMETHODCALLTYPE WillChangeField( 
            /* [in] */ LONG cFields,
            /* [in] */ VARIANT Fields,
            /* [out][in] */ EventStatusEnum __RPC_FAR *adStatus,
            /* [in] */ _ADORecordset __RPC_FAR *pRecordset) = 0;
        
        virtual /* [helpcontext][id] */ HRESULT STDMETHODCALLTYPE FieldChangeComplete( 
            /* [in] */ LONG cFields,
            /* [in] */ VARIANT Fields,
            /* [in] */ ADOError __RPC_FAR *pError,
            /* [out][in] */ EventStatusEnum __RPC_FAR *adStatus,
            /* [in] */ _ADORecordset __RPC_FAR *pRecordset) = 0;
        
        virtual /* [helpcontext][id] */ HRESULT STDMETHODCALLTYPE WillChangeRecord( 
            /* [in] */ EventReasonEnum adReason,
            /* [in] */ LONG cRecords,
            /* [out][in] */ EventStatusEnum __RPC_FAR *adStatus,
            /* [in] */ _ADORecordset __RPC_FAR *pRecordset) = 0;
        
        virtual /* [helpcontext][id] */ HRESULT STDMETHODCALLTYPE RecordChangeComplete( 
            /* [in] */ EventReasonEnum adReason,
            /* [in] */ LONG cRecords,
            /* [in] */ ADOError __RPC_FAR *pError,
            /* [out][in] */ EventStatusEnum __RPC_FAR *adStatus,
            /* [in] */ _ADORecordset __RPC_FAR *pRecordset) = 0;
        
        virtual /* [helpcontext][id] */ HRESULT STDMETHODCALLTYPE WillChangeRecordset( 
            /* [in] */ EventReasonEnum adReason,
            /* [out][in] */ EventStatusEnum __RPC_FAR *adStatus,
            /* [in] */ _ADORecordset __RPC_FAR *pRecordset) = 0;
        
        virtual /* [helpcontext][id] */ HRESULT STDMETHODCALLTYPE RecordsetChangeComplete( 
            /* [in] */ EventReasonEnum adReason,
            /* [in] */ ADOError __RPC_FAR *pError,
            /* [out][in] */ EventStatusEnum __RPC_FAR *adStatus,
            /* [in] */ _ADORecordset __RPC_FAR *pRecordset) = 0;
        
        virtual /* [helpcontext][id] */ HRESULT STDMETHODCALLTYPE WillMove( 
            /* [in] */ EventReasonEnum adReason,
            /* [out][in] */ EventStatusEnum __RPC_FAR *adStatus,
            /* [in] */ _ADORecordset __RPC_FAR *pRecordset) = 0;
        
        virtual /* [helpcontext][id] */ HRESULT STDMETHODCALLTYPE MoveComplete( 
            /* [in] */ EventReasonEnum adReason,
            /* [in] */ ADOError __RPC_FAR *pError,
            /* [out][in] */ EventStatusEnum __RPC_FAR *adStatus,
            /* [in] */ _ADORecordset __RPC_FAR *pRecordset) = 0;
        
        virtual /* [helpcontext][id] */ HRESULT STDMETHODCALLTYPE EndOfRecordset( 
            /* [out][in] */ VARIANT_BOOL __RPC_FAR *fMoreData,
            /* [out][in] */ EventStatusEnum __RPC_FAR *adStatus,
            /* [in] */ _ADORecordset __RPC_FAR *pRecordset) = 0;
        
        virtual /* [helpcontext][id] */ HRESULT STDMETHODCALLTYPE FetchProgress( 
            /* [in] */ long Progress,
            /* [in] */ long MaxProgress,
            /* [out][in] */ EventStatusEnum __RPC_FAR *adStatus,
            /* [in] */ _ADORecordset __RPC_FAR *pRecordset) = 0;
        
        virtual /* [helpcontext][id] */ HRESULT STDMETHODCALLTYPE FetchComplete( 
            /* [in] */ ADOError __RPC_FAR *pError,
            /* [out][in] */ EventStatusEnum __RPC_FAR *adStatus,
            /* [in] */ _ADORecordset __RPC_FAR *pRecordset) = 0;
        
    };
    
#else 	/* C style interface */
    typedef struct RecordsetEventsVtVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            RecordsetEventsVt __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            RecordsetEventsVt __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            RecordsetEventsVt __RPC_FAR * This);
        
        /* [helpcontext][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *WillChangeADOField )( 
            RecordsetEventsVt __RPC_FAR * This,
            /* [in] */ LONG cFields,
            /* [in] */ VARIANT Fields,
            /* [out][in] */ EventStatusEnum __RPC_FAR *adStatus,
            /* [in] */ _ADORecordset __RPC_FAR *pRecordset);
        
        /* [helpcontext][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *FieldChangeComplete )( 
            RecordsetEventsVt __RPC_FAR * This,
            /* [in] */ LONG cFields,
            /* [in] */ VARIANT Fields,
            /* [in] */ ADOError __RPC_FAR *pError,
            /* [out][in] */ EventStatusEnum __RPC_FAR *adStatus,
            /* [in] */ _ADORecordset __RPC_FAR *pRecordset);
        
        /* [helpcontext][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *WillChangeADORecord )( 
            RecordsetEventsVt __RPC_FAR * This,
            /* [in] */ EventReasonEnum adReason,
            /* [in] */ LONG cRecords,
            /* [out][in] */ EventStatusEnum __RPC_FAR *adStatus,
            /* [in] */ _ADORecordset __RPC_FAR *pRecordset);
        
        /* [helpcontext][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *RecordChangeComplete )( 
            RecordsetEventsVt __RPC_FAR * This,
            /* [in] */ EventReasonEnum adReason,
            /* [in] */ LONG cRecords,
            /* [in] */ ADOError __RPC_FAR *pError,
            /* [out][in] */ EventStatusEnum __RPC_FAR *adStatus,
            /* [in] */ _ADORecordset __RPC_FAR *pRecordset);
        
        /* [helpcontext][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *WillChangeADORecordset )( 
            RecordsetEventsVt __RPC_FAR * This,
            /* [in] */ EventReasonEnum adReason,
            /* [out][in] */ EventStatusEnum __RPC_FAR *adStatus,
            /* [in] */ _ADORecordset __RPC_FAR *pRecordset);
        
        /* [helpcontext][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *RecordsetChangeComplete )( 
            RecordsetEventsVt __RPC_FAR * This,
            /* [in] */ EventReasonEnum adReason,
            /* [in] */ ADOError __RPC_FAR *pError,
            /* [out][in] */ EventStatusEnum __RPC_FAR *adStatus,
            /* [in] */ _ADORecordset __RPC_FAR *pRecordset);
        
        /* [helpcontext][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *WillMove )( 
            RecordsetEventsVt __RPC_FAR * This,
            /* [in] */ EventReasonEnum adReason,
            /* [out][in] */ EventStatusEnum __RPC_FAR *adStatus,
            /* [in] */ _ADORecordset __RPC_FAR *pRecordset);
        
        /* [helpcontext][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *MoveComplete )( 
            RecordsetEventsVt __RPC_FAR * This,
            /* [in] */ EventReasonEnum adReason,
            /* [in] */ ADOError __RPC_FAR *pError,
            /* [out][in] */ EventStatusEnum __RPC_FAR *adStatus,
            /* [in] */ _ADORecordset __RPC_FAR *pRecordset);
        
        /* [helpcontext][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *EndOfADORecordset )( 
            RecordsetEventsVt __RPC_FAR * This,
            /* [out][in] */ VARIANT_BOOL __RPC_FAR *fMoreData,
            /* [out][in] */ EventStatusEnum __RPC_FAR *adStatus,
            /* [in] */ _ADORecordset __RPC_FAR *pRecordset);
        
        /* [helpcontext][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *FetchProgress )( 
            RecordsetEventsVt __RPC_FAR * This,
            /* [in] */ long Progress,
            /* [in] */ long MaxProgress,
            /* [out][in] */ EventStatusEnum __RPC_FAR *adStatus,
            /* [in] */ _ADORecordset __RPC_FAR *pRecordset);
        
        /* [helpcontext][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *FetchComplete )( 
            RecordsetEventsVt __RPC_FAR * This,
            /* [in] */ ADOError __RPC_FAR *pError,
            /* [out][in] */ EventStatusEnum __RPC_FAR *adStatus,
            /* [in] */ _ADORecordset __RPC_FAR *pRecordset);
        
        END_INTERFACE
    } RecordsetEventsVtVtbl;
    interface RecordsetEventsVt
    {
        CONST_VTBL struct RecordsetEventsVtVtbl __RPC_FAR *lpVtbl;
    };
    
#ifdef COBJMACROS
#define RecordsetEventsVt_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)
#define RecordsetEventsVt_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)
#define RecordsetEventsVt_Release(This)	\
    (This)->lpVtbl -> Release(This)
#define RecordsetEventsVt_WillChangeField(This,cFields,Fields,adStatus,pRecordset)	\
    (This)->lpVtbl -> WillChangeField(This,cFields,Fields,adStatus,pRecordset)
#define RecordsetEventsVt_FieldChangeComplete(This,cFields,Fields,pError,adStatus,pRecordset)	\
    (This)->lpVtbl -> FieldChangeComplete(This,cFields,Fields,pError,adStatus,pRecordset)
#define RecordsetEventsVt_WillChangeRecord(This,adReason,cRecords,adStatus,pRecordset)	\
    (This)->lpVtbl -> WillChangeRecord(This,adReason,cRecords,adStatus,pRecordset)
#define RecordsetEventsVt_RecordChangeComplete(This,adReason,cRecords,pError,adStatus,pRecordset)	\
    (This)->lpVtbl -> RecordChangeComplete(This,adReason,cRecords,pError,adStatus,pRecordset)
#define RecordsetEventsVt_WillChangeRecordset(This,adReason,adStatus,pRecordset)	\
    (This)->lpVtbl -> WillChangeRecordset(This,adReason,adStatus,pRecordset)
#define RecordsetEventsVt_RecordsetChangeComplete(This,adReason,pError,adStatus,pRecordset)	\
    (This)->lpVtbl -> RecordsetChangeComplete(This,adReason,pError,adStatus,pRecordset)
#define RecordsetEventsVt_WillMove(This,adReason,adStatus,pRecordset)	\
    (This)->lpVtbl -> WillMove(This,adReason,adStatus,pRecordset)
#define RecordsetEventsVt_MoveComplete(This,adReason,pError,adStatus,pRecordset)	\
    (This)->lpVtbl -> MoveComplete(This,adReason,pError,adStatus,pRecordset)
#define RecordsetEventsVt_EndOfRecordset(This,fMoreData,adStatus,pRecordset)	\
    (This)->lpVtbl -> EndOfRecordset(This,fMoreData,adStatus,pRecordset)
#define RecordsetEventsVt_FetchProgress(This,Progress,MaxProgress,adStatus,pRecordset)	\
    (This)->lpVtbl -> FetchProgress(This,Progress,MaxProgress,adStatus,pRecordset)
#define RecordsetEventsVt_FetchComplete(This,pError,adStatus,pRecordset)	\
    (This)->lpVtbl -> FetchComplete(This,pError,adStatus,pRecordset)
#endif /* COBJMACROS */
#endif 	/* C style interface */
/* [helpcontext][id] */ HRESULT STDMETHODCALLTYPE RecordsetEventsVt_WillChangeField_Proxy( 
    RecordsetEventsVt __RPC_FAR * This,
    /* [in] */ LONG cFields,
    /* [in] */ VARIANT Fields,
    /* [out][in] */ EventStatusEnum __RPC_FAR *adStatus,
    /* [in] */ _ADORecordset __RPC_FAR *pRecordset);
void __RPC_STUB RecordsetEventsVt_WillChangeField_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);
/* [helpcontext][id] */ HRESULT STDMETHODCALLTYPE RecordsetEventsVt_FieldChangeComplete_Proxy( 
    RecordsetEventsVt __RPC_FAR * This,
    /* [in] */ LONG cFields,
    /* [in] */ VARIANT Fields,
    /* [in] */ ADOError __RPC_FAR *pError,
    /* [out][in] */ EventStatusEnum __RPC_FAR *adStatus,
    /* [in] */ _ADORecordset __RPC_FAR *pRecordset);
void __RPC_STUB RecordsetEventsVt_FieldChangeComplete_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);
/* [helpcontext][id] */ HRESULT STDMETHODCALLTYPE RecordsetEventsVt_WillChangeRecord_Proxy( 
    RecordsetEventsVt __RPC_FAR * This,
    /* [in] */ EventReasonEnum adReason,
    /* [in] */ LONG cRecords,
    /* [out][in] */ EventStatusEnum __RPC_FAR *adStatus,
    /* [in] */ _ADORecordset __RPC_FAR *pRecordset);
void __RPC_STUB RecordsetEventsVt_WillChangeRecord_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);
/* [helpcontext][id] */ HRESULT STDMETHODCALLTYPE RecordsetEventsVt_RecordChangeComplete_Proxy( 
    RecordsetEventsVt __RPC_FAR * This,
    /* [in] */ EventReasonEnum adReason,
    /* [in] */ LONG cRecords,
    /* [in] */ ADOError __RPC_FAR *pError,
    /* [out][in] */ EventStatusEnum __RPC_FAR *adStatus,
    /* [in] */ _ADORecordset __RPC_FAR *pRecordset);
void __RPC_STUB RecordsetEventsVt_RecordChangeComplete_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);
/* [helpcontext][id] */ HRESULT STDMETHODCALLTYPE RecordsetEventsVt_WillChangeRecordset_Proxy( 
    RecordsetEventsVt __RPC_FAR * This,
    /* [in] */ EventReasonEnum adReason,
    /* [out][in] */ EventStatusEnum __RPC_FAR *adStatus,
    /* [in] */ _ADORecordset __RPC_FAR *pRecordset);
void __RPC_STUB RecordsetEventsVt_WillChangeRecordset_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);
/* [helpcontext][id] */ HRESULT STDMETHODCALLTYPE RecordsetEventsVt_RecordsetChangeComplete_Proxy( 
    RecordsetEventsVt __RPC_FAR * This,
    /* [in] */ EventReasonEnum adReason,
    /* [in] */ ADOError __RPC_FAR *pError,
    /* [out][in] */ EventStatusEnum __RPC_FAR *adStatus,
    /* [in] */ _ADORecordset __RPC_FAR *pRecordset);
void __RPC_STUB RecordsetEventsVt_RecordsetChangeComplete_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);
/* [helpcontext][id] */ HRESULT STDMETHODCALLTYPE RecordsetEventsVt_WillMove_Proxy( 
    RecordsetEventsVt __RPC_FAR * This,
    /* [in] */ EventReasonEnum adReason,
    /* [out][in] */ EventStatusEnum __RPC_FAR *adStatus,
    /* [in] */ _ADORecordset __RPC_FAR *pRecordset);
void __RPC_STUB RecordsetEventsVt_WillMove_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);
/* [helpcontext][id] */ HRESULT STDMETHODCALLTYPE RecordsetEventsVt_MoveComplete_Proxy( 
    RecordsetEventsVt __RPC_FAR * This,
    /* [in] */ EventReasonEnum adReason,
    /* [in] */ ADOError __RPC_FAR *pError,
    /* [out][in] */ EventStatusEnum __RPC_FAR *adStatus,
    /* [in] */ _ADORecordset __RPC_FAR *pRecordset);
void __RPC_STUB RecordsetEventsVt_MoveComplete_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);
/* [helpcontext][id] */ HRESULT STDMETHODCALLTYPE RecordsetEventsVt_EndOfRecordset_Proxy( 
    RecordsetEventsVt __RPC_FAR * This,
    /* [out][in] */ VARIANT_BOOL __RPC_FAR *fMoreData,
    /* [out][in] */ EventStatusEnum __RPC_FAR *adStatus,
    /* [in] */ _ADORecordset __RPC_FAR *pRecordset);
void __RPC_STUB RecordsetEventsVt_EndOfRecordset_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);
/* [helpcontext][id] */ HRESULT STDMETHODCALLTYPE RecordsetEventsVt_FetchProgress_Proxy( 
    RecordsetEventsVt __RPC_FAR * This,
    /* [in] */ long Progress,
    /* [in] */ long MaxProgress,
    /* [out][in] */ EventStatusEnum __RPC_FAR *adStatus,
    /* [in] */ _ADORecordset __RPC_FAR *pRecordset);
void __RPC_STUB RecordsetEventsVt_FetchProgress_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);
/* [helpcontext][id] */ HRESULT STDMETHODCALLTYPE RecordsetEventsVt_FetchComplete_Proxy( 
    RecordsetEventsVt __RPC_FAR * This,
    /* [in] */ ADOError __RPC_FAR *pError,
    /* [out][in] */ EventStatusEnum __RPC_FAR *adStatus,
    /* [in] */ _ADORecordset __RPC_FAR *pRecordset);
void __RPC_STUB RecordsetEventsVt_FetchComplete_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);
#endif 	/* __RecordsetEventsVt_INTERFACE_DEFINED__ */
#ifndef __ConnectionEvents_DISPINTERFACE_DEFINED__
#define __ConnectionEvents_DISPINTERFACE_DEFINED__
/* dispinterface ConnectionEvents */
/* [uuid] */ 
EXTERN_C const IID DIID_ConnectionEvents;
#if defined(__cplusplus) && !defined(CINTERFACE)
    MIDL_INTERFACE("00000400-0000-0010-8000-00AA006D2EA4")
    ConnectionEvents : public IDispatch
    {
    };
    
#else 	/* C style interface */
    typedef struct ConnectionEventsVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            ConnectionEvents __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            ConnectionEvents __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            ConnectionEvents __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            ConnectionEvents __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            ConnectionEvents __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            ConnectionEvents __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            ConnectionEvents __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        END_INTERFACE
    } ConnectionEventsVtbl;
    interface ConnectionEvents
    {
        CONST_VTBL struct ConnectionEventsVtbl __RPC_FAR *lpVtbl;
    };
    
#ifdef COBJMACROS
#define ConnectionEvents_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)
#define ConnectionEvents_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)
#define ConnectionEvents_Release(This)	\
    (This)->lpVtbl -> Release(This)
#define ConnectionEvents_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)
#define ConnectionEvents_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)
#define ConnectionEvents_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)
#define ConnectionEvents_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)
#endif /* COBJMACROS */
#endif 	/* C style interface */
#endif 	/* __ConnectionEvents_DISPINTERFACE_DEFINED__ */
#ifndef __RecordsetEvents_DISPINTERFACE_DEFINED__
#define __RecordsetEvents_DISPINTERFACE_DEFINED__
/* dispinterface RecordsetEvents */
/* [uuid] */ 
EXTERN_C const IID DIID_RecordsetEvents;
#if defined(__cplusplus) && !defined(CINTERFACE)
    MIDL_INTERFACE("00000266-0000-0010-8000-00AA006D2EA4")
    RecordsetEvents : public IDispatch
    {
    };
    
#else 	/* C style interface */
    typedef struct RecordsetEventsVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            RecordsetEvents __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            RecordsetEvents __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            RecordsetEvents __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            RecordsetEvents __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            RecordsetEvents __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            RecordsetEvents __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            RecordsetEvents __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        END_INTERFACE
    } RecordsetEventsVtbl;
    interface RecordsetEvents
    {
        CONST_VTBL struct RecordsetEventsVtbl __RPC_FAR *lpVtbl;
    };
    
#ifdef COBJMACROS
#define RecordsetEvents_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)
#define RecordsetEvents_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)
#define RecordsetEvents_Release(This)	\
    (This)->lpVtbl -> Release(This)
#define RecordsetEvents_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)
#define RecordsetEvents_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)
#define RecordsetEvents_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)
#define RecordsetEvents_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)
#endif /* COBJMACROS */
#endif 	/* C style interface */
#endif 	/* __RecordsetEvents_DISPINTERFACE_DEFINED__ */
#ifndef __Connection15_INTERFACE_DEFINED__
#define __Connection15_INTERFACE_DEFINED__
/* interface Connection15 */
/* [object][helpcontext][uuid][hidden][dual] */ 
EXTERN_C const IID IID_Connection15;
#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("00000515-0000-0010-8000-00AA006D2EA4")
    Connection15 : public _ADO
    {
    public:
        virtual /* [helpcontext][id][propget] */ HRESULT STDMETHODCALLTYPE get_ConnectionString( 
            /* [retval][out] */ BSTR __RPC_FAR *pbstr) = 0;
        
        virtual /* [helpcontext][id][propput] */ HRESULT STDMETHODCALLTYPE put_ConnectionString( 
            /* [in] */ BSTR bstr) = 0;
        
        virtual /* [helpcontext][id][propget] */ HRESULT STDMETHODCALLTYPE get_CommandTimeout( 
            /* [retval][out] */ LONG __RPC_FAR *plTimeout) = 0;
        
        virtual /* [helpcontext][id][propput] */ HRESULT STDMETHODCALLTYPE put_CommandTimeout( 
            /* [in] */ LONG lTimeout) = 0;
        
        virtual /* [helpcontext][id][propget] */ HRESULT STDMETHODCALLTYPE get_ConnectionTimeout( 
            /* [retval][out] */ LONG __RPC_FAR *plTimeout) = 0;
        
        virtual /* [helpcontext][id][propput] */ HRESULT STDMETHODCALLTYPE put_ConnectionTimeout( 
            /* [in] */ LONG lTimeout) = 0;
        
        virtual /* [helpcontext][id][propget] */ HRESULT STDMETHODCALLTYPE get_Version( 
            /* [retval][out] */ BSTR __RPC_FAR *pbstr) = 0;
        
        virtual /* [helpcontext][id] */ HRESULT STDMETHODCALLTYPE Close( void) = 0;
        
        virtual /* [helpcontext][id] */ HRESULT STDMETHODCALLTYPE Execute( 
            /* [in] */ BSTR CommandText,
            /* [optional][out] */ VARIANT __RPC_FAR *RecordsAffected,
            /* [defaultvalue][in] */ long Options,
            /* [retval][out] */ _ADORecordset __RPC_FAR *__RPC_FAR *ppiRset) = 0;
        
        virtual /* [helpcontext][id] */ HRESULT STDMETHODCALLTYPE BeginTrans( 
            /* [retval][out] */ long __RPC_FAR *TransactionLevel) = 0;
        
        virtual /* [helpcontext][id] */ HRESULT STDMETHODCALLTYPE CommitTrans( void) = 0;
        
        virtual /* [helpcontext][id] */ HRESULT STDMETHODCALLTYPE RollbackTrans( void) = 0;
        
        virtual /* [helpcontext][id] */ HRESULT STDMETHODCALLTYPE Open( 
            /* [defaultvalue][in] */ BSTR ConnectionString = L"",
            /* [defaultvalue][in] */ BSTR UserID = L"",
            /* [defaultvalue][in] */ BSTR Password = L"",
            /* [defaultvalue][in] */ long Options = adOptionUnspecified) = 0;
        
        virtual /* [helpcontext][id][propget] */ HRESULT STDMETHODCALLTYPE get_Errors( 
            /* [retval][out] */ ADOErrors __RPC_FAR *__RPC_FAR *ppvObject) = 0;
        
        virtual /* [helpcontext][id][propget] */ HRESULT STDMETHODCALLTYPE get_DefaultDatabase( 
            /* [retval][out] */ BSTR __RPC_FAR *pbstr) = 0;
        
        virtual /* [helpcontext][id][propput] */ HRESULT STDMETHODCALLTYPE put_DefaultDatabase( 
            /* [in] */ BSTR bstr) = 0;
        
        virtual /* [helpcontext][id][propget] */ HRESULT STDMETHODCALLTYPE get_IsolationLevel( 
            /* [retval][out] */ IsolationLevelEnum __RPC_FAR *Level) = 0;
        
        virtual /* [helpcontext][id][propput] */ HRESULT STDMETHODCALLTYPE put_IsolationLevel( 
            /* [in] */ IsolationLevelEnum Level) = 0;
        
        virtual /* [helpcontext][id][propget] */ HRESULT STDMETHODCALLTYPE get_Attributes( 
            /* [retval][out] */ long __RPC_FAR *plAttr) = 0;
        
        virtual /* [helpcontext][id][propput] */ HRESULT STDMETHODCALLTYPE put_Attributes( 
            /* [in] */ long lAttr) = 0;
        
        virtual /* [helpcontext][propget][id] */ HRESULT STDMETHODCALLTYPE get_CursorLocation( 
            /* [retval][out] */ CursorLocationEnum __RPC_FAR *plCursorLoc) = 0;
        
        virtual /* [helpcontext][propput][id] */ HRESULT STDMETHODCALLTYPE put_CursorLocation( 
            /* [in] */ CursorLocationEnum lCursorLoc) = 0;
        
        virtual /* [helpcontext][id][propget] */ HRESULT STDMETHODCALLTYPE get_Mode( 
            /* [retval][out] */ ConnectModeEnum __RPC_FAR *plMode) = 0;
        
        virtual /* [helpcontext][id][propput] */ HRESULT STDMETHODCALLTYPE put_Mode( 
            /* [in] */ ConnectModeEnum lMode) = 0;
        
        virtual /* [helpcontext][id][propget] */ HRESULT STDMETHODCALLTYPE get_Provider( 
            /* [retval][out] */ BSTR __RPC_FAR *pbstr) = 0;
        
        virtual /* [helpcontext][id][propput] */ HRESULT STDMETHODCALLTYPE put_Provider( 
            /* [in] */ BSTR Provider) = 0;
        
        virtual /* [helpcontext][id][propget] */ HRESULT STDMETHODCALLTYPE get_State( 
            /* [retval][out] */ LONG __RPC_FAR *plObjState) = 0;
        
        virtual /* [helpcontext][id] */ HRESULT STDMETHODCALLTYPE OpenSchema( 
            /* [in] */ SchemaEnum Schema,
            /* [optional][in] */ VARIANT Restrictions,
            /* [optional][in] */ VARIANT SchemaID,
            /* [retval][out] */ _ADORecordset __RPC_FAR *__RPC_FAR *pprset) = 0;
        
    };
    
#else 	/* C style interface */
    typedef struct Connection15Vtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            Connection15 __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            Connection15 __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            Connection15 __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            Connection15 __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            Connection15 __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            Connection15 __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            Connection15 __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        /* [helpcontext][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Properties )( 
            Connection15 __RPC_FAR * This,
            /* [retval][out] */ ADOProperties __RPC_FAR *__RPC_FAR *ppvObject);
        
        /* [helpcontext][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_ConnectionString )( 
            Connection15 __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pbstr);
        
        /* [helpcontext][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_ConnectionString )( 
            Connection15 __RPC_FAR * This,
            /* [in] */ BSTR bstr);
        
        /* [helpcontext][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_CommandTimeout )( 
            Connection15 __RPC_FAR * This,
            /* [retval][out] */ LONG __RPC_FAR *plTimeout);
        
        /* [helpcontext][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_CommandTimeout )( 
            Connection15 __RPC_FAR * This,
            /* [in] */ LONG lTimeout);
        
        /* [helpcontext][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_ConnectionTimeout )( 
            Connection15 __RPC_FAR * This,
            /* [retval][out] */ LONG __RPC_FAR *plTimeout);
        
        /* [helpcontext][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_ConnectionTimeout )( 
            Connection15 __RPC_FAR * This,
            /* [in] */ LONG lTimeout);
        
        /* [helpcontext][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Version )( 
            Connection15 __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pbstr);
        
        /* [helpcontext][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Close )( 
            Connection15 __RPC_FAR * This);
        
        /* [helpcontext][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Execute )( 
            Connection15 __RPC_FAR * This,
            /* [in] */ BSTR CommandText,
            /* [optional][out] */ VARIANT __RPC_FAR *RecordsAffected,
            /* [defaultvalue][in] */ long Options,
            /* [retval][out] */ _ADORecordset __RPC_FAR *__RPC_FAR *ppiRset);
        
        /* [helpcontext][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *BeginTrans )( 
            Connection15 __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *TransactionLevel);
        
        /* [helpcontext][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *CommitTrans )( 
            Connection15 __RPC_FAR * This);
        
        /* [helpcontext][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *RollbackTrans )( 
            Connection15 __RPC_FAR * This);
        
        /* [helpcontext][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Open )( 
            Connection15 __RPC_FAR * This,
            /* [defaultvalue][in] */ BSTR ConnectionString,
            /* [defaultvalue][in] */ BSTR UserID,
            /* [defaultvalue][in] */ BSTR Password,
            /* [defaultvalue][in] */ long Options);
        
        /* [helpcontext][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Errors )( 
            Connection15 __RPC_FAR * This,
            /* [retval][out] */ ADOErrors __RPC_FAR *__RPC_FAR *ppvObject);
        
        /* [helpcontext][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_DefaultDatabase )( 
            Connection15 __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pbstr);
        
        /* [helpcontext][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_DefaultDatabase )( 
            Connection15 __RPC_FAR * This,
            /* [in] */ BSTR bstr);
        
        /* [helpcontext][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_IsolationLevel )( 
            Connection15 __RPC_FAR * This,
            /* [retval][out] */ IsolationLevelEnum __RPC_FAR *Level);
        
        /* [helpcontext][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_IsolationLevel )( 
            Connection15 __RPC_FAR * This,
            /* [in] */ IsolationLevelEnum Level);
        
        /* [helpcontext][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Attributes )( 
            Connection15 __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *plAttr);
        
        /* [helpcontext][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Attributes )( 
            Connection15 __RPC_FAR * This,
            /* [in] */ long lAttr);
        
        /* [helpcontext][propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_CursorLocation )( 
            Connection15 __RPC_FAR * This,
            /* [retval][out] */ CursorLocationEnum __RPC_FAR *plCursorLoc);
        
        /* [helpcontext][propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_CursorLocation )( 
            Connection15 __RPC_FAR * This,
            /* [in] */ CursorLocationEnum lCursorLoc);
        
        /* [helpcontext][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Mode )( 
            Connection15 __RPC_FAR * This,
            /* [retval][out] */ ConnectModeEnum __RPC_FAR *plMode);
        
        /* [helpcontext][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Mode )( 
            Connection15 __RPC_FAR * This,
            /* [in] */ ConnectModeEnum lMode);
        
        /* [helpcontext][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Provider )( 
            Connection15 __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pbstr);
        
        /* [helpcontext][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Provider )( 
            Connection15 __RPC_FAR * This,
            /* [in] */ BSTR Provider);
        
        /* [helpcontext][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_State )( 
            Connection15 __RPC_FAR * This,
            /* [retval][out] */ LONG __RPC_FAR *plObjState);
        
        /* [helpcontext][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *OpenSchema )( 
            Connection15 __RPC_FAR * This,
            /* [in] */ SchemaEnum Schema,
            /* [optional][in] */ VARIANT Restrictions,
            /* [optional][in] */ VARIANT SchemaID,
            /* [retval][out] */ _ADORecordset __RPC_FAR *__RPC_FAR *pprset);
        
        END_INTERFACE
    } Connection15Vtbl;
    interface Connection15
    {
        CONST_VTBL struct Connection15Vtbl __RPC_FAR *lpVtbl;
    };
    
#ifdef COBJMACROS
#define Connection15_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)
#define Connection15_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)
#define Connection15_Release(This)	\
    (This)->lpVtbl -> Release(This)
#define Connection15_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)
#define Connection15_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)
#define Connection15_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)
#define Connection15_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)
#define Connection15_get_Properties(This,ppvObject)	\
    (This)->lpVtbl -> get_Properties(This,ppvObject)
#define Connection15_get_ConnectionString(This,pbstr)	\
    (This)->lpVtbl -> get_ConnectionString(This,pbstr)
#define Connection15_put_ConnectionString(This,bstr)	\
    (This)->lpVtbl -> put_ConnectionString(This,bstr)
#define Connection15_get_CommandTimeout(This,plTimeout)	\
    (This)->lpVtbl -> get_CommandTimeout(This,plTimeout)
#define Connection15_put_CommandTimeout(This,lTimeout)	\
    (This)->lpVtbl -> put_CommandTimeout(This,lTimeout)
#define Connection15_get_ConnectionTimeout(This,plTimeout)	\
    (This)->lpVtbl -> get_ConnectionTimeout(This,plTimeout)
#define Connection15_put_ConnectionTimeout(This,lTimeout)	\
    (This)->lpVtbl -> put_ConnectionTimeout(This,lTimeout)
#define Connection15_get_Version(This,pbstr)	\
    (This)->lpVtbl -> get_Version(This,pbstr)
#define Connection15_Close(This)	\
    (This)->lpVtbl -> Close(This)
#define Connection15_Execute(This,CommandText,RecordsAffected,Options,ppiRset)	\
    (This)->lpVtbl -> Execute(This,CommandText,RecordsAffected,Options,ppiRset)
#define Connection15_BeginTrans(This,TransactionLevel)	\
    (This)->lpVtbl -> BeginTrans(This,TransactionLevel)
#define Connection15_CommitTrans(This)	\
    (This)->lpVtbl -> CommitTrans(This)
#define Connection15_RollbackTrans(This)	\
    (This)->lpVtbl -> RollbackTrans(This)
#define Connection15_Open(This,ConnectionString,UserID,Password,Options)	\
    (This)->lpVtbl -> Open(This,ConnectionString,UserID,Password,Options)
#define Connection15_get_Errors(This,ppvObject)	\
    (This)->lpVtbl -> get_Errors(This,ppvObject)
#define Connection15_get_DefaultDatabase(This,pbstr)	\
    (This)->lpVtbl -> get_DefaultDatabase(This,pbstr)
#define Connection15_put_DefaultDatabase(This,bstr)	\
    (This)->lpVtbl -> put_DefaultDatabase(This,bstr)
#define Connection15_get_IsolationLevel(This,Level)	\
    (This)->lpVtbl -> get_IsolationLevel(This,Level)
#define Connection15_put_IsolationLevel(This,Level)	\
    (This)->lpVtbl -> put_IsolationLevel(This,Level)
#define Connection15_get_Attributes(This,plAttr)	\
    (This)->lpVtbl -> get_Attributes(This,plAttr)
#define Connection15_put_Attributes(This,lAttr)	\
    (This)->lpVtbl -> put_Attributes(This,lAttr)
#define Connection15_get_CursorLocation(This,plCursorLoc)	\
    (This)->lpVtbl -> get_CursorLocation(This,plCursorLoc)
#define Connection15_put_CursorLocation(This,lCursorLoc)	\
    (This)->lpVtbl -> put_CursorLocation(This,lCursorLoc)
#define Connection15_get_Mode(This,plMode)	\
    (This)->lpVtbl -> get_Mode(This,plMode)
#define Connection15_put_Mode(This,lMode)	\
    (This)->lpVtbl -> put_Mode(This,lMode)
#define Connection15_get_Provider(This,pbstr)	\
    (This)->lpVtbl -> get_Provider(This,pbstr)
#define Connection15_put_Provider(This,Provider)	\
    (This)->lpVtbl -> put_Provider(This,Provider)
#define Connection15_get_State(This,plObjState)	\
    (This)->lpVtbl -> get_State(This,plObjState)
#define Connection15_OpenSchema(This,Schema,Restrictions,SchemaID,pprset)	\
    (This)->lpVtbl -> OpenSchema(This,Schema,Restrictions,SchemaID,pprset)
#endif /* COBJMACROS */
#endif 	/* C style interface */
/* [helpcontext][id][propget] */ HRESULT STDMETHODCALLTYPE Connection15_get_ConnectionString_Proxy( 
    Connection15 __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pbstr);
void __RPC_STUB Connection15_get_ConnectionString_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);
/* [helpcontext][id][propput] */ HRESULT STDMETHODCALLTYPE Connection15_put_ConnectionString_Proxy( 
    Connection15 __RPC_FAR * This,
    /* [in] */ BSTR bstr);
void __RPC_STUB Connection15_put_ConnectionString_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);
/* [helpcontext][id][propget] */ HRESULT STDMETHODCALLTYPE Connection15_get_CommandTimeout_Proxy( 
    Connection15 __RPC_FAR * This,
    /* [retval][out] */ LONG __RPC_FAR *plTimeout);
void __RPC_STUB Connection15_get_CommandTimeout_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);
/* [helpcontext][id][propput] */ HRESULT STDMETHODCALLTYPE Connection15_put_CommandTimeout_Proxy( 
    Connection15 __RPC_FAR * This,
    /* [in] */ LONG lTimeout);
void __RPC_STUB Connection15_put_CommandTimeout_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);
/* [helpcontext][id][propget] */ HRESULT STDMETHODCALLTYPE Connection15_get_ConnectionTimeout_Proxy( 
    Connection15 __RPC_FAR * This,
    /* [retval][out] */ LONG __RPC_FAR *plTimeout);
void __RPC_STUB Connection15_get_ConnectionTimeout_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);
/* [helpcontext][id][propput] */ HRESULT STDMETHODCALLTYPE Connection15_put_ConnectionTimeout_Proxy( 
    Connection15 __RPC_FAR * This,
    /* [in] */ LONG lTimeout);
void __RPC_STUB Connection15_put_ConnectionTimeout_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);
/* [helpcontext][id][propget] */ HRESULT STDMETHODCALLTYPE Connection15_get_Version_Proxy( 
    Connection15 __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pbstr);
void __RPC_STUB Connection15_get_Version_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);
/* [helpcontext][id] */ HRESULT STDMETHODCALLTYPE Connection15_Close_Proxy( 
    Connection15 __RPC_FAR * This);
void __RPC_STUB Connection15_Close_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);
/* [helpcontext][id] */ HRESULT STDMETHODCALLTYPE Connection15_Execute_Proxy( 
    Connection15 __RPC_FAR * This,
    /* [in] */ BSTR CommandText,
    /* [optional][out] */ VARIANT __RPC_FAR *RecordsAffected,
    /* [defaultvalue][in] */ long Options,
    /* [retval][out] */ _ADORecordset __RPC_FAR *__RPC_FAR *ppiRset);
void __RPC_STUB Connection15_Execute_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);
/* [helpcontext][id] */ HRESULT STDMETHODCALLTYPE Connection15_BeginTrans_Proxy( 
    Connection15 __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *TransactionLevel);
void __RPC_STUB Connection15_BeginTrans_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);
/* [helpcontext][id] */ HRESULT STDMETHODCALLTYPE Connection15_CommitTrans_Proxy( 
    Connection15 __RPC_FAR * This);
void __RPC_STUB Connection15_CommitTrans_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);
/* [helpcontext][id] */ HRESULT STDMETHODCALLTYPE Connection15_RollbackTrans_Proxy( 
    Connection15 __RPC_FAR * This);
void __RPC_STUB Connection15_RollbackTrans_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);
/* [helpcontext][id] */ HRESULT STDMETHODCALLTYPE Connection15_Open_Proxy( 
    Connection15 __RPC_FAR * This,
    /* [defaultvalue][in] */ BSTR ConnectionString,
    /* [defaultvalue][in] */ BSTR UserID,
    /* [defaultvalue][in] */ BSTR Password,
    /* [defaultvalue][in] */ long Options);
void __RPC_STUB Connection15_Open_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);
/* [helpcontext][id][propget] */ HRESULT STDMETHODCALLTYPE Connection15_get_Errors_Proxy( 
    Connection15 __RPC_FAR * This,
    /* [retval][out] */ ADOErrors __RPC_FAR *__RPC_FAR *ppvObject);
void __RPC_STUB Connection15_get_Errors_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);
/* [helpcontext][id][propget] */ HRESULT STDMETHODCALLTYPE Connection15_get_DefaultDatabase_Proxy( 
    Connection15 __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pbstr);
void __RPC_STUB Connection15_get_DefaultDatabase_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);
/* [helpcontext][id][propput] */ HRESULT STDMETHODCALLTYPE Connection15_put_DefaultDatabase_Proxy( 
    Connection15 __RPC_FAR * This,
    /* [in] */ BSTR bstr);
void __RPC_STUB Connection15_put_DefaultDatabase_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);
/* [helpcontext][id][propget] */ HRESULT STDMETHODCALLTYPE Connection15_get_IsolationLevel_Proxy( 
    Connection15 __RPC_FAR * This,
    /* [retval][out] */ IsolationLevelEnum __RPC_FAR *Level);
void __RPC_STUB Connection15_get_IsolationLevel_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);
/* [helpcontext][id][propput] */ HRESULT STDMETHODCALLTYPE Connection15_put_IsolationLevel_Proxy( 
    Connection15 __RPC_FAR * This,
    /* [in] */ IsolationLevelEnum Level);
void __RPC_STUB Connection15_put_IsolationLevel_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);
/* [helpcontext][id][propget] */ HRESULT STDMETHODCALLTYPE Connection15_get_Attributes_Proxy( 
    Connection15 __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *plAttr);
void __RPC_STUB Connection15_get_Attributes_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);
/* [helpcontext][id][propput] */ HRESULT STDMETHODCALLTYPE Connection15_put_Attributes_Proxy( 
    Connection15 __RPC_FAR * This,
    /* [in] */ long lAttr);
void __RPC_STUB Connection15_put_Attributes_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);
/* [helpcontext][propget][id] */ HRESULT STDMETHODCALLTYPE Connection15_get_CursorLocation_Proxy( 
    Connection15 __RPC_FAR * This,
    /* [retval][out] */ CursorLocationEnum __RPC_FAR *plCursorLoc);
void __RPC_STUB Connection15_get_CursorLocation_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);
/* [helpcontext][propput][id] */ HRESULT STDMETHODCALLTYPE Connection15_put_CursorLocation_Proxy( 
    Connection15 __RPC_FAR * This,
    /* [in] */ CursorLocationEnum lCursorLoc);
void __RPC_STUB Connection15_put_CursorLocation_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);
/* [helpcontext][id][propget] */ HRESULT STDMETHODCALLTYPE Connection15_get_Mode_Proxy( 
    Connection15 __RPC_FAR * This,
    /* [retval][out] */ ConnectModeEnum __RPC_FAR *plMode);
void __RPC_STUB Connection15_get_Mode_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);
/* [helpcontext][id][propput] */ HRESULT STDMETHODCALLTYPE Connection15_put_Mode_Proxy( 
    Connection15 __RPC_FAR * This,
    /* [in] */ ConnectModeEnum lMode);
void __RPC_STUB Connection15_put_Mode_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);
/* [helpcontext][id][propget] */ HRESULT STDMETHODCALLTYPE Connection15_get_Provider_Proxy( 
    Connection15 __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pbstr);
void __RPC_STUB Connection15_get_Provider_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);
/* [helpcontext][id][propput] */ HRESULT STDMETHODCALLTYPE Connection15_put_Provider_Proxy( 
    Connection15 __RPC_FAR * This,
    /* [in] */ BSTR Provider);
void __RPC_STUB Connection15_put_Provider_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);
/* [helpcontext][id][propget] */ HRESULT STDMETHODCALLTYPE Connection15_get_State_Proxy( 
    Connection15 __RPC_FAR * This,
    /* [retval][out] */ LONG __RPC_FAR *plObjState);
void __RPC_STUB Connection15_get_State_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);
/* [helpcontext][id] */ HRESULT STDMETHODCALLTYPE Connection15_OpenSchema_Proxy( 
    Connection15 __RPC_FAR * This,
    /* [in] */ SchemaEnum Schema,
    /* [optional][in] */ VARIANT Restrictions,
    /* [optional][in] */ VARIANT SchemaID,
    /* [retval][out] */ _ADORecordset __RPC_FAR *__RPC_FAR *pprset);
void __RPC_STUB Connection15_OpenSchema_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);
#endif 	/* __Connection15_INTERFACE_DEFINED__ */
#ifndef ___Connection_INTERFACE_DEFINED__
#define ___Connection_INTERFACE_DEFINED__
/* interface _ADOConnection */
/* [object][helpcontext][uuid][dual] */ 
EXTERN_C const IID IID__Connection;
#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("00000550-0000-0010-8000-00AA006D2EA4")
    _ADOConnection : public Connection15
    {
    public:
        virtual /* [helpcontext][id] */ HRESULT STDMETHODCALLTYPE Cancel( void) = 0;
        
    };
    
#else 	/* C style interface */
    typedef struct _ConnectionVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            _ADOConnection __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            _ADOConnection __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            _ADOConnection __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            _ADOConnection __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            _ADOConnection __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            _ADOConnection __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            _ADOConnection __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        /* [helpcontext][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Properties )( 
            _ADOConnection __RPC_FAR * This,
            /* [retval][out] */ ADOProperties __RPC_FAR *__RPC_FAR *ppvObject);
        
        /* [helpcontext][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_ConnectionString )( 
            _ADOConnection __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pbstr);
        
        /* [helpcontext][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_ConnectionString )( 
            _ADOConnection __RPC_FAR * This,
            /* [in] */ BSTR bstr);
        
        /* [helpcontext][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_CommandTimeout )( 
            _ADOConnection __RPC_FAR * This,
            /* [retval][out] */ LONG __RPC_FAR *plTimeout);
        
        /* [helpcontext][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_CommandTimeout )( 
            _ADOConnection __RPC_FAR * This,
            /* [in] */ LONG lTimeout);
        
        /* [helpcontext][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_ConnectionTimeout )( 
            _ADOConnection __RPC_FAR * This,
            /* [retval][out] */ LONG __RPC_FAR *plTimeout);
        
        /* [helpcontext][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_ConnectionTimeout )( 
            _ADOConnection __RPC_FAR * This,
            /* [in] */ LONG lTimeout);
        
        /* [helpcontext][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Version )( 
            _ADOConnection __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pbstr);
        
        /* [helpcontext][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Close )( 
            _ADOConnection __RPC_FAR * This);
        
        /* [helpcontext][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Execute )( 
            _ADOConnection __RPC_FAR * This,
            /* [in] */ BSTR CommandText,
            /* [optional][out] */ VARIANT __RPC_FAR *RecordsAffected,
            /* [defaultvalue][in] */ long Options,
            /* [retval][out] */ _ADORecordset __RPC_FAR *__RPC_FAR *ppiRset);
        
        /* [helpcontext][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *BeginTrans )( 
            _ADOConnection __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *TransactionLevel);
        
        /* [helpcontext][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *CommitTrans )( 
            _ADOConnection __RPC_FAR * This);
        
        /* [helpcontext][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *RollbackTrans )( 
            _ADOConnection __RPC_FAR * This);
        
        /* [helpcontext][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Open )( 
            _ADOConnection __RPC_FAR * This,
            /* [defaultvalue][in] */ BSTR ConnectionString,
            /* [defaultvalue][in] */ BSTR UserID,
            /* [defaultvalue][in] */ BSTR Password,
            /* [defaultvalue][in] */ long Options);
        
        /* [helpcontext][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Errors )( 
            _ADOConnection __RPC_FAR * This,
            /* [retval][out] */ ADOErrors __RPC_FAR *__RPC_FAR *ppvObject);
        
        /* [helpcontext][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_DefaultDatabase )( 
            _ADOConnection __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pbstr);
        
        /* [helpcontext][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_DefaultDatabase )( 
            _ADOConnection __RPC_FAR * This,
            /* [in] */ BSTR bstr);
        
        /* [helpcontext][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_IsolationLevel )( 
            _ADOConnection __RPC_FAR * This,
            /* [retval][out] */ IsolationLevelEnum __RPC_FAR *Level);
        
        /* [helpcontext][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_IsolationLevel )( 
            _ADOConnection __RPC_FAR * This,
            /* [in] */ IsolationLevelEnum Level);
        
        /* [helpcontext][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Attributes )( 
            _ADOConnection __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *plAttr);
        
        /* [helpcontext][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Attributes )( 
            _ADOConnection __RPC_FAR * This,
            /* [in] */ long lAttr);
        
        /* [helpcontext][propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_CursorLocation )( 
            _ADOConnection __RPC_FAR * This,
            /* [retval][out] */ CursorLocationEnum __RPC_FAR *plCursorLoc);
        
        /* [helpcontext][propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_CursorLocation )( 
            _ADOConnection __RPC_FAR * This,
            /* [in] */ CursorLocationEnum lCursorLoc);
        
        /* [helpcontext][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Mode )( 
            _ADOConnection __RPC_FAR * This,
            /* [retval][out] */ ConnectModeEnum __RPC_FAR *plMode);
        
        /* [helpcontext][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Mode )( 
            _ADOConnection __RPC_FAR * This,
            /* [in] */ ConnectModeEnum lMode);
        
        /* [helpcontext][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Provider )( 
            _ADOConnection __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pbstr);
        
        /* [helpcontext][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Provider )( 
            _ADOConnection __RPC_FAR * This,
            /* [in] */ BSTR Provider);
        
        /* [helpcontext][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_State )( 
            _ADOConnection __RPC_FAR * This,
            /* [retval][out] */ LONG __RPC_FAR *plObjState);
        
        /* [helpcontext][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *OpenSchema )( 
            _ADOConnection __RPC_FAR * This,
            /* [in] */ SchemaEnum Schema,
            /* [optional][in] */ VARIANT Restrictions,
            /* [optional][in] */ VARIANT SchemaID,
            /* [retval][out] */ _ADORecordset __RPC_FAR *__RPC_FAR *pprset);
        
        /* [helpcontext][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Cancel )( 
            _ADOConnection __RPC_FAR * This);
        
        END_INTERFACE
    } _ConnectionVtbl;
    interface _Connection
    {
        CONST_VTBL struct _ConnectionVtbl __RPC_FAR *lpVtbl;
    };
    
#ifdef COBJMACROS
#define _Connection_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)
#define _Connection_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)
#define _Connection_Release(This)	\
    (This)->lpVtbl -> Release(This)
#define _Connection_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)
#define _Connection_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)
#define _Connection_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)
#define _Connection_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)
#define _Connection_get_Properties(This,ppvObject)	\
    (This)->lpVtbl -> get_Properties(This,ppvObject)
#define _Connection_get_ConnectionString(This,pbstr)	\
    (This)->lpVtbl -> get_ConnectionString(This,pbstr)
#define _Connection_put_ConnectionString(This,bstr)	\
    (This)->lpVtbl -> put_ConnectionString(This,bstr)
#define _Connection_get_CommandTimeout(This,plTimeout)	\
    (This)->lpVtbl -> get_CommandTimeout(This,plTimeout)
#define _Connection_put_CommandTimeout(This,lTimeout)	\
    (This)->lpVtbl -> put_CommandTimeout(This,lTimeout)
#define _Connection_get_ConnectionTimeout(This,plTimeout)	\
    (This)->lpVtbl -> get_ConnectionTimeout(This,plTimeout)
#define _Connection_put_ConnectionTimeout(This,lTimeout)	\
    (This)->lpVtbl -> put_ConnectionTimeout(This,lTimeout)
#define _Connection_get_Version(This,pbstr)	\
    (This)->lpVtbl -> get_Version(This,pbstr)
#define _Connection_Close(This)	\
    (This)->lpVtbl -> Close(This)
#define _Connection_Execute(This,CommandText,RecordsAffected,Options,ppiRset)	\
    (This)->lpVtbl -> Execute(This,CommandText,RecordsAffected,Options,ppiRset)
#define _Connection_BeginTrans(This,TransactionLevel)	\
    (This)->lpVtbl -> BeginTrans(This,TransactionLevel)
#define _Connection_CommitTrans(This)	\
    (This)->lpVtbl -> CommitTrans(This)
#define _Connection_RollbackTrans(This)	\
    (This)->lpVtbl -> RollbackTrans(This)
#define _Connection_Open(This,ConnectionString,UserID,Password,Options)	\
    (This)->lpVtbl -> Open(This,ConnectionString,UserID,Password,Options)
#define _Connection_get_Errors(This,ppvObject)	\
    (This)->lpVtbl -> get_Errors(This,ppvObject)
#define _Connection_get_DefaultDatabase(This,pbstr)	\
    (This)->lpVtbl -> get_DefaultDatabase(This,pbstr)
#define _Connection_put_DefaultDatabase(This,bstr)	\
    (This)->lpVtbl -> put_DefaultDatabase(This,bstr)
#define _Connection_get_IsolationLevel(This,Level)	\
    (This)->lpVtbl -> get_IsolationLevel(This,Level)
#define _Connection_put_IsolationLevel(This,Level)	\
    (This)->lpVtbl -> put_IsolationLevel(This,Level)
#define _Connection_get_Attributes(This,plAttr)	\
    (This)->lpVtbl -> get_Attributes(This,plAttr)
#define _Connection_put_Attributes(This,lAttr)	\
    (This)->lpVtbl -> put_Attributes(This,lAttr)
#define _Connection_get_CursorLocation(This,plCursorLoc)	\
    (This)->lpVtbl -> get_CursorLocation(This,plCursorLoc)
#define _Connection_put_CursorLocation(This,lCursorLoc)	\
    (This)->lpVtbl -> put_CursorLocation(This,lCursorLoc)
#define _Connection_get_Mode(This,plMode)	\
    (This)->lpVtbl -> get_Mode(This,plMode)
#define _Connection_put_Mode(This,lMode)	\
    (This)->lpVtbl -> put_Mode(This,lMode)
#define _Connection_get_Provider(This,pbstr)	\
    (This)->lpVtbl -> get_Provider(This,pbstr)
#define _Connection_put_Provider(This,Provider)	\
    (This)->lpVtbl -> put_Provider(This,Provider)
#define _Connection_get_State(This,plObjState)	\
    (This)->lpVtbl -> get_State(This,plObjState)
#define _Connection_OpenSchema(This,Schema,Restrictions,SchemaID,pprset)	\
    (This)->lpVtbl -> OpenSchema(This,Schema,Restrictions,SchemaID,pprset)
#define _Connection_Cancel(This)	\
    (This)->lpVtbl -> Cancel(This)
#endif /* COBJMACROS */
#endif 	/* C style interface */
/* [helpcontext][id] */ HRESULT STDMETHODCALLTYPE _Connection_Cancel_Proxy( 
    _ADOConnection __RPC_FAR * This);
void __RPC_STUB _Connection_Cancel_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);
#endif 	/* ___Connection_INTERFACE_DEFINED__ */
#ifndef __ADOConnectionConstruction15_INTERFACE_DEFINED__
#define __ADOConnectionConstruction15_INTERFACE_DEFINED__
/* interface ADOConnectionConstruction15 */
/* [object][uuid][restricted] */ 
EXTERN_C const IID IID_ADOConnectionConstruction15;
#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("00000516-0000-0010-8000-00AA006D2EA4")
    ADOConnectionConstruction15 : public IUnknown
    {
    public:
        virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_DSO( 
            /* [retval][out] */ IUnknown __RPC_FAR *__RPC_FAR *ppDSO) = 0;
        
        virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_Session( 
            /* [retval][out] */ IUnknown __RPC_FAR *__RPC_FAR *ppSession) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE WrapDSOandSession( 
            /* [in] */ IUnknown __RPC_FAR *pDSO,
            /* [in] */ IUnknown __RPC_FAR *pSession) = 0;
        
    };
    
#else 	/* C style interface */
    typedef struct ADOConnectionConstruction15Vtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            ADOConnectionConstruction15 __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            ADOConnectionConstruction15 __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            ADOConnectionConstruction15 __RPC_FAR * This);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_DSO )( 
            ADOConnectionConstruction15 __RPC_FAR * This,
            /* [retval][out] */ IUnknown __RPC_FAR *__RPC_FAR *ppDSO);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Session )( 
            ADOConnectionConstruction15 __RPC_FAR * This,
            /* [retval][out] */ IUnknown __RPC_FAR *__RPC_FAR *ppSession);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *WrapDSOandSession )( 
            ADOConnectionConstruction15 __RPC_FAR * This,
            /* [in] */ IUnknown __RPC_FAR *pDSO,
            /* [in] */ IUnknown __RPC_FAR *pSession);
        
        END_INTERFACE
    } ADOConnectionConstruction15Vtbl;
    interface ADOConnectionConstruction15
    {
        CONST_VTBL struct ADOConnectionConstruction15Vtbl __RPC_FAR *lpVtbl;
    };
    
#ifdef COBJMACROS
#define ADOConnectionConstruction15_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)
#define ADOConnectionConstruction15_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)
#define ADOConnectionConstruction15_Release(This)	\
    (This)->lpVtbl -> Release(This)
#define ADOConnectionConstruction15_get_DSO(This,ppDSO)	\
    (This)->lpVtbl -> get_DSO(This,ppDSO)
#define ADOConnectionConstruction15_get_Session(This,ppSession)	\
    (This)->lpVtbl -> get_Session(This,ppSession)
#define ADOConnectionConstruction15_WrapDSOandSession(This,pDSO,pSession)	\
    (This)->lpVtbl -> WrapDSOandSession(This,pDSO,pSession)
#endif /* COBJMACROS */
#endif 	/* C style interface */
/* [propget] */ HRESULT STDMETHODCALLTYPE ADOConnectionConstruction15_get_DSO_Proxy( 
    ADOConnectionConstruction15 __RPC_FAR * This,
    /* [retval][out] */ IUnknown __RPC_FAR *__RPC_FAR *ppDSO);
void __RPC_STUB ADOConnectionConstruction15_get_DSO_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);
/* [propget] */ HRESULT STDMETHODCALLTYPE ADOConnectionConstruction15_get_Session_Proxy( 
    ADOConnectionConstruction15 __RPC_FAR * This,
    /* [retval][out] */ IUnknown __RPC_FAR *__RPC_FAR *ppSession);
void __RPC_STUB ADOConnectionConstruction15_get_Session_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);
HRESULT STDMETHODCALLTYPE ADOConnectionConstruction15_WrapDSOandSession_Proxy( 
    ADOConnectionConstruction15 __RPC_FAR * This,
    /* [in] */ IUnknown __RPC_FAR *pDSO,
    /* [in] */ IUnknown __RPC_FAR *pSession);
void __RPC_STUB ADOConnectionConstruction15_WrapDSOandSession_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);
#endif 	/* __ADOConnectionConstruction15_INTERFACE_DEFINED__ */
#ifndef __ADOConnectionConstruction_INTERFACE_DEFINED__
#define __ADOConnectionConstruction_INTERFACE_DEFINED__
/* interface ADOConnectionConstruction */
/* [object][uuid][restricted] */ 
EXTERN_C const IID IID_ADOConnectionConstruction;
#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("00000551-0000-0010-8000-00AA006D2EA4")
    ADOConnectionConstruction : public ADOConnectionConstruction15
    {
    public:
    };
    
#else 	/* C style interface */
    typedef struct ADOConnectionConstructionVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            ADOConnectionConstruction __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            ADOConnectionConstruction __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            ADOConnectionConstruction __RPC_FAR * This);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_DSO )( 
            ADOConnectionConstruction __RPC_FAR * This,
            /* [retval][out] */ IUnknown __RPC_FAR *__RPC_FAR *ppDSO);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Session )( 
            ADOConnectionConstruction __RPC_FAR * This,
            /* [retval][out] */ IUnknown __RPC_FAR *__RPC_FAR *ppSession);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *WrapDSOandSession )( 
            ADOConnectionConstruction __RPC_FAR * This,
            /* [in] */ IUnknown __RPC_FAR *pDSO,
            /* [in] */ IUnknown __RPC_FAR *pSession);
        
        END_INTERFACE
    } ADOConnectionConstructionVtbl;
    interface ADOConnectionConstruction
    {
        CONST_VTBL struct ADOConnectionConstructionVtbl __RPC_FAR *lpVtbl;
    };
    
#ifdef COBJMACROS
#define ADOConnectionConstruction_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)
#define ADOConnectionConstruction_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)
#define ADOConnectionConstruction_Release(This)	\
    (This)->lpVtbl -> Release(This)
#define ADOConnectionConstruction_get_DSO(This,ppDSO)	\
    (This)->lpVtbl -> get_DSO(This,ppDSO)
#define ADOConnectionConstruction_get_Session(This,ppSession)	\
    (This)->lpVtbl -> get_Session(This,ppSession)
#define ADOConnectionConstruction_WrapDSOandSession(This,pDSO,pSession)	\
    (This)->lpVtbl -> WrapDSOandSession(This,pDSO,pSession)
#endif /* COBJMACROS */
#endif 	/* C style interface */
#endif 	/* __ADOConnectionConstruction_INTERFACE_DEFINED__ */
EXTERN_C const CLSID CLSID_Connection;
#ifdef __cplusplus
Connection;
#endif
#ifndef ___Record_INTERFACE_DEFINED__
#define ___Record_INTERFACE_DEFINED__
/* interface _ADORecord */
/* [object][uuid][helpcontext][hidden][dual] */ 
EXTERN_C const IID IID__Record;
#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("00000562-0000-0010-8000-00AA006D2EA4")
    _ADORecord : public _ADO
    {
    public:
        virtual /* [helpcontext][propget][id] */ HRESULT STDMETHODCALLTYPE get_ActiveConnection( 
            /* [retval][out] */ VARIANT __RPC_FAR *pvar) = 0;
        
        virtual /* [helpcontext][propput][id] */ HRESULT STDMETHODCALLTYPE put_ActiveConnection( 
            /* [in] */ BSTR bstrConn) = 0;
        
        virtual /* [helpcontext][propputref][id] */ HRESULT STDMETHODCALLTYPE putref_ActiveConnection( 
            /* [in] */ _ADOConnection __RPC_FAR *Con) = 0;
        
        virtual /* [helpcontext][propget][id] */ HRESULT STDMETHODCALLTYPE get_State( 
            /* [retval][out] */ ObjectStateEnum __RPC_FAR *pState) = 0;
        
        virtual /* [helpcontext][propget][id] */ HRESULT STDMETHODCALLTYPE get_Source( 
            /* [retval][out] */ VARIANT __RPC_FAR *pvar) = 0;
        
        virtual /* [helpcontext][propput][id] */ HRESULT STDMETHODCALLTYPE put_Source( 
            /* [in] */ BSTR Source) = 0;
        
        virtual /* [helpcontext][propputref][id] */ HRESULT STDMETHODCALLTYPE putref_Source( 
            /* [in] */ IDispatch __RPC_FAR *Source) = 0;
        
        virtual /* [helpcontext][propget][id] */ HRESULT STDMETHODCALLTYPE get_Mode( 
            /* [retval][out] */ ConnectModeEnum __RPC_FAR *pMode) = 0;
        
        virtual /* [helpcontext][propput][id] */ HRESULT STDMETHODCALLTYPE put_Mode( 
            /* [in] */ ConnectModeEnum Mode) = 0;
        
        virtual /* [helpcontext][propget][id] */ HRESULT STDMETHODCALLTYPE get_ParentURL( 
            /* [retval][out] */ BSTR __RPC_FAR *pbstrParentURL) = 0;
        
        virtual /* [helpcontext][id] */ HRESULT STDMETHODCALLTYPE MoveRecord( 
            /* [defaultvalue][in] */ BSTR Source,
            /* [defaultvalue][in] */ BSTR Destination,
            /* [optional][in] */ BSTR UserName,
            /* [optional][in] */ BSTR Password,
            /* [defaultvalue][in] */ MoveRecordOptionsEnum Options,
            /* [optional][in] */ VARIANT_BOOL Async,
            /* [retval][out] */ BSTR __RPC_FAR *pbstrNewURL) = 0;
        
        virtual /* [helpcontext][id] */ HRESULT STDMETHODCALLTYPE CopyRecord( 
            /* [defaultvalue][in] */ BSTR Source,
            /* [defaultvalue][in] */ BSTR Destination,
            /* [optional][in] */ BSTR UserName,
            /* [optional][in] */ BSTR Password,
            /* [defaultvalue][in] */ CopyRecordOptionsEnum Options,
            /* [optional][in] */ VARIANT_BOOL Async,
            /* [retval][out] */ BSTR __RPC_FAR *pbstrNewURL) = 0;
        
        virtual /* [helpcontext][id] */ HRESULT STDMETHODCALLTYPE DeleteRecord( 
            /* [optional][in] */ BSTR Source,
            /* [optional][in] */ VARIANT_BOOL Async) = 0;
        
        virtual /* [helpcontext][id] */ HRESULT STDMETHODCALLTYPE Open( 
            /* [optional][in] */ VARIANT Source,
            /* [optional][in] */ VARIANT ActiveConnection,
            /* [optional][in] */ ConnectModeEnum Mode,
            /* [defaultvalue][in] */ RecordCreateOptionsEnum CreateOptions,
            /* [defaultvalue][in] */ RecordOpenOptionsEnum Options,
            /* [optional][in] */ BSTR UserName,
            /* [optional][in] */ BSTR Password) = 0;
        
        virtual /* [helpcontext][id] */ HRESULT STDMETHODCALLTYPE Close( void) = 0;
        
        virtual /* [helpcontext][propget][id] */ HRESULT STDMETHODCALLTYPE get_Fields( 
            /* [retval][out] */ ADOFields __RPC_FAR *__RPC_FAR *ppFlds) = 0;
        
        virtual /* [helpcontext][propget][id] */ HRESULT STDMETHODCALLTYPE get_RecordType( 
            /* [retval][out] */ RecordTypeEnum __RPC_FAR *pType) = 0;
        
        virtual /* [helpcontext][id] */ HRESULT STDMETHODCALLTYPE GetChildren( 
            /* [retval][out] */ _ADORecordset __RPC_FAR *__RPC_FAR *ppRSet) = 0;
        
        virtual /* [helpcontext][id] */ HRESULT STDMETHODCALLTYPE Cancel( void) = 0;
        
    };
    
#else 	/* C style interface */
    typedef struct _RecordVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            _ADORecord __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            _ADORecord __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            _ADORecord __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            _ADORecord __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            _ADORecord __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            _ADORecord __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            _ADORecord __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        /* [helpcontext][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Properties )( 
            _ADORecord __RPC_FAR * This,
            /* [retval][out] */ ADOProperties __RPC_FAR *__RPC_FAR *ppvObject);
        
        /* [helpcontext][propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_ActiveConnection )( 
            _ADORecord __RPC_FAR * This,
            /* [retval][out] */ VARIANT __RPC_FAR *pvar);
        
        /* [helpcontext][propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_ActiveConnection )( 
            _ADORecord __RPC_FAR * This,
            /* [in] */ BSTR bstrConn);
        
        /* [helpcontext][propputref][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *putref_ActiveADOConnection )( 
            _ADORecord __RPC_FAR * This,
            /* [in] */ _ADOConnection __RPC_FAR *Con);
        
        /* [helpcontext][propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_State )( 
            _ADORecord __RPC_FAR * This,
            /* [retval][out] */ ObjectStateEnum __RPC_FAR *pState);
        
        /* [helpcontext][propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Source )( 
            _ADORecord __RPC_FAR * This,
            /* [retval][out] */ VARIANT __RPC_FAR *pvar);
        
        /* [helpcontext][propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Source )( 
            _ADORecord __RPC_FAR * This,
            /* [in] */ BSTR Source);
        
        /* [helpcontext][propputref][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *putref_Source )( 
            _ADORecord __RPC_FAR * This,
            /* [in] */ IDispatch __RPC_FAR *Source);
        
        /* [helpcontext][propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Mode )( 
            _ADORecord __RPC_FAR * This,
            /* [retval][out] */ ConnectModeEnum __RPC_FAR *pMode);
        
        /* [helpcontext][propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Mode )( 
            _ADORecord __RPC_FAR * This,
            /* [in] */ ConnectModeEnum Mode);
        
        /* [helpcontext][propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_ParentURL )( 
            _ADORecord __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pbstrParentURL);
        
        /* [helpcontext][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *MoveADORecord )( 
            _ADORecord __RPC_FAR * This,
            /* [defaultvalue][in] */ BSTR Source,
            /* [defaultvalue][in] */ BSTR Destination,
            /* [optional][in] */ BSTR UserName,
            /* [optional][in] */ BSTR Password,
            /* [defaultvalue][in] */ MoveRecordOptionsEnum Options,
            /* [optional][in] */ VARIANT_BOOL Async,
            /* [retval][out] */ BSTR __RPC_FAR *pbstrNewURL);
        
        /* [helpcontext][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *CopyADORecord )( 
            _ADORecord __RPC_FAR * This,
            /* [defaultvalue][in] */ BSTR Source,
            /* [defaultvalue][in] */ BSTR Destination,
            /* [optional][in] */ BSTR UserName,
            /* [optional][in] */ BSTR Password,
            /* [defaultvalue][in] */ CopyRecordOptionsEnum Options,
            /* [optional][in] */ VARIANT_BOOL Async,
            /* [retval][out] */ BSTR __RPC_FAR *pbstrNewURL);
        
        /* [helpcontext][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *DeleteADORecord )( 
            _ADORecord __RPC_FAR * This,
            /* [optional][in] */ BSTR Source,
            /* [optional][in] */ VARIANT_BOOL Async);
        
        /* [helpcontext][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Open )( 
            _ADORecord __RPC_FAR * This,
            /* [optional][in] */ VARIANT Source,
            /* [optional][in] */ VARIANT ActiveConnection,
            /* [optional][in] */ ConnectModeEnum Mode,
            /* [defaultvalue][in] */ RecordCreateOptionsEnum CreateOptions,
            /* [defaultvalue][in] */ RecordOpenOptionsEnum Options,
            /* [optional][in] */ BSTR UserName,
            /* [optional][in] */ BSTR Password);
        
        /* [helpcontext][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Close )( 
            _ADORecord __RPC_FAR * This);
        
        /* [helpcontext][propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Fields )( 
            _ADORecord __RPC_FAR * This,
            /* [retval][out] */ ADOFields __RPC_FAR *__RPC_FAR *ppFlds);
        
        /* [helpcontext][propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_RecordType )( 
            _ADORecord __RPC_FAR * This,
            /* [retval][out] */ RecordTypeEnum __RPC_FAR *pType);
        
        /* [helpcontext][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetChildren )( 
            _ADORecord __RPC_FAR * This,
            /* [retval][out] */ _ADORecordset __RPC_FAR *__RPC_FAR *ppRSet);
        
        /* [helpcontext][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Cancel )( 
            _ADORecord __RPC_FAR * This);
        
        END_INTERFACE
    } _RecordVtbl;
    interface _Record
    {
        CONST_VTBL struct _RecordVtbl __RPC_FAR *lpVtbl;
    };
    
#ifdef COBJMACROS
#define _Record_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)
#define _Record_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)
#define _Record_Release(This)	\
    (This)->lpVtbl -> Release(This)
#define _Record_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)
#define _Record_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)
#define _Record_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)
#define _Record_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)
#define _Record_get_Properties(This,ppvObject)	\
    (This)->lpVtbl -> get_Properties(This,ppvObject)
#define _Record_get_ActiveConnection(This,pvar)	\
    (This)->lpVtbl -> get_ActiveConnection(This,pvar)
#define _Record_put_ActiveConnection(This,bstrConn)	\
    (This)->lpVtbl -> put_ActiveConnection(This,bstrConn)
#define _Record_putref_ActiveConnection(This,Con)	\
    (This)->lpVtbl -> putref_ActiveConnection(This,Con)
#define _Record_get_State(This,pState)	\
    (This)->lpVtbl -> get_State(This,pState)
#define _Record_get_Source(This,pvar)	\
    (This)->lpVtbl -> get_Source(This,pvar)
#define _Record_put_Source(This,Source)	\
    (This)->lpVtbl -> put_Source(This,Source)
#define _Record_putref_Source(This,Source)	\
    (This)->lpVtbl -> putref_Source(This,Source)
#define _Record_get_Mode(This,pMode)	\
    (This)->lpVtbl -> get_Mode(This,pMode)
#define _Record_put_Mode(This,Mode)	\
    (This)->lpVtbl -> put_Mode(This,Mode)
#define _Record_get_ParentURL(This,pbstrParentURL)	\
    (This)->lpVtbl -> get_ParentURL(This,pbstrParentURL)
#define _Record_MoveRecord(This,Source,Destination,UserName,Password,Options,Async,pbstrNewURL)	\
    (This)->lpVtbl -> MoveRecord(This,Source,Destination,UserName,Password,Options,Async,pbstrNewURL)
#define _Record_CopyRecord(This,Source,Destination,UserName,Password,Options,Async,pbstrNewURL)	\
    (This)->lpVtbl -> CopyRecord(This,Source,Destination,UserName,Password,Options,Async,pbstrNewURL)
#define _Record_DeleteRecord(This,Source,Async)	\
    (This)->lpVtbl -> DeleteRecord(This,Source,Async)
#define _Record_Open(This,Source,ActiveConnection,Mode,CreateOptions,Options,UserName,Password)	\
    (This)->lpVtbl -> Open(This,Source,ActiveConnection,Mode,CreateOptions,Options,UserName,Password)
#define _Record_Close(This)	\
    (This)->lpVtbl -> Close(This)
#define _Record_get_Fields(This,ppFlds)	\
    (This)->lpVtbl -> get_Fields(This,ppFlds)
#define _Record_get_RecordType(This,pType)	\
    (This)->lpVtbl -> get_RecordType(This,pType)
#define _Record_GetChildren(This,ppRSet)	\
    (This)->lpVtbl -> GetChildren(This,ppRSet)
#define _Record_Cancel(This)	\
    (This)->lpVtbl -> Cancel(This)
#endif /* COBJMACROS */
#endif 	/* C style interface */
/* [helpcontext][propget][id] */ HRESULT STDMETHODCALLTYPE _Record_get_ActiveConnection_Proxy( 
    _ADORecord __RPC_FAR * This,
    /* [retval][out] */ VARIANT __RPC_FAR *pvar);
void __RPC_STUB _Record_get_ActiveConnection_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);
/* [helpcontext][propput][id] */ HRESULT STDMETHODCALLTYPE _Record_put_ActiveConnection_Proxy( 
    _ADORecord __RPC_FAR * This,
    /* [in] */ BSTR bstrConn);
void __RPC_STUB _Record_put_ActiveConnection_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);
/* [helpcontext][propputref][id] */ HRESULT STDMETHODCALLTYPE _Record_putref_ActiveConnection_Proxy( 
    _ADORecord __RPC_FAR * This,
    /* [in] */ _ADOConnection __RPC_FAR *Con);
void __RPC_STUB _Record_putref_ActiveConnection_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);
/* [helpcontext][propget][id] */ HRESULT STDMETHODCALLTYPE _Record_get_State_Proxy( 
    _ADORecord __RPC_FAR * This,
    /* [retval][out] */ ObjectStateEnum __RPC_FAR *pState);
void __RPC_STUB _Record_get_State_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);
/* [helpcontext][propget][id] */ HRESULT STDMETHODCALLTYPE _Record_get_Source_Proxy( 
    _ADORecord __RPC_FAR * This,
    /* [retval][out] */ VARIANT __RPC_FAR *pvar);
void __RPC_STUB _Record_get_Source_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);
/* [helpcontext][propput][id] */ HRESULT STDMETHODCALLTYPE _Record_put_Source_Proxy( 
    _ADORecord __RPC_FAR * This,
    /* [in] */ BSTR Source);
void __RPC_STUB _Record_put_Source_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);
/* [helpcontext][propputref][id] */ HRESULT STDMETHODCALLTYPE _Record_putref_Source_Proxy( 
    _ADORecord __RPC_FAR * This,
    /* [in] */ IDispatch __RPC_FAR *Source);
void __RPC_STUB _Record_putref_Source_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);
/* [helpcontext][propget][id] */ HRESULT STDMETHODCALLTYPE _Record_get_Mode_Proxy( 
    _ADORecord __RPC_FAR * This,
    /* [retval][out] */ ConnectModeEnum __RPC_FAR *pMode);
void __RPC_STUB _Record_get_Mode_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);
/* [helpcontext][propput][id] */ HRESULT STDMETHODCALLTYPE _Record_put_Mode_Proxy( 
    _ADORecord __RPC_FAR * This,
    /* [in] */ ConnectModeEnum Mode);
void __RPC_STUB _Record_put_Mode_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);
/* [helpcontext][propget][id] */ HRESULT STDMETHODCALLTYPE _Record_get_ParentURL_Proxy( 
    _ADORecord __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pbstrParentURL);
void __RPC_STUB _Record_get_ParentURL_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);
/* [helpcontext][id] */ HRESULT STDMETHODCALLTYPE _Record_MoveRecord_Proxy( 
    _ADORecord __RPC_FAR * This,
    /* [defaultvalue][in] */ BSTR Source,
    /* [defaultvalue][in] */ BSTR Destination,
    /* [optional][in] */ BSTR UserName,
    /* [optional][in] */ BSTR Password,
    /* [defaultvalue][in] */ MoveRecordOptionsEnum Options,
    /* [optional][in] */ VARIANT_BOOL Async,
    /* [retval][out] */ BSTR __RPC_FAR *pbstrNewURL);
void __RPC_STUB _Record_MoveRecord_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);
/* [helpcontext][id] */ HRESULT STDMETHODCALLTYPE _Record_CopyRecord_Proxy( 
    _ADORecord __RPC_FAR * This,
    /* [defaultvalue][in] */ BSTR Source,
    /* [defaultvalue][in] */ BSTR Destination,
    /* [optional][in] */ BSTR UserName,
    /* [optional][in] */ BSTR Password,
    /* [defaultvalue][in] */ CopyRecordOptionsEnum Options,
    /* [optional][in] */ VARIANT_BOOL Async,
    /* [retval][out] */ BSTR __RPC_FAR *pbstrNewURL);
void __RPC_STUB _Record_CopyRecord_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);
/* [helpcontext][id] */ HRESULT STDMETHODCALLTYPE _Record_DeleteRecord_Proxy( 
    _ADORecord __RPC_FAR * This,
    /* [optional][in] */ BSTR Source,
    /* [optional][in] */ VARIANT_BOOL Async);
void __RPC_STUB _Record_DeleteRecord_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);
/* [helpcontext][id] */ HRESULT STDMETHODCALLTYPE _Record_Open_Proxy( 
    _ADORecord __RPC_FAR * This,
    /* [optional][in] */ VARIANT Source,
    /* [optional][in] */ VARIANT ActiveConnection,
    /* [optional][in] */ ConnectModeEnum Mode,
    /* [defaultvalue][in] */ RecordCreateOptionsEnum CreateOptions,
    /* [defaultvalue][in] */ RecordOpenOptionsEnum Options,
    /* [optional][in] */ BSTR UserName,
    /* [optional][in] */ BSTR Password);
void __RPC_STUB _Record_Open_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);
/* [helpcontext][id] */ HRESULT STDMETHODCALLTYPE _Record_Close_Proxy( 
    _ADORecord __RPC_FAR * This);
void __RPC_STUB _Record_Close_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);
/* [helpcontext][propget][id] */ HRESULT STDMETHODCALLTYPE _Record_get_Fields_Proxy( 
    _ADORecord __RPC_FAR * This,
    /* [retval][out] */ ADOFields __RPC_FAR *__RPC_FAR *ppFlds);
void __RPC_STUB _Record_get_Fields_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);
/* [helpcontext][propget][id] */ HRESULT STDMETHODCALLTYPE _Record_get_RecordType_Proxy( 
    _ADORecord __RPC_FAR * This,
    /* [retval][out] */ RecordTypeEnum __RPC_FAR *pType);
void __RPC_STUB _Record_get_RecordType_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);
/* [helpcontext][id] */ HRESULT STDMETHODCALLTYPE _Record_GetChildren_Proxy( 
    _ADORecord __RPC_FAR * This,
    /* [retval][out] */ _ADORecordset __RPC_FAR *__RPC_FAR *ppRSet);
void __RPC_STUB _Record_GetChildren_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);
/* [helpcontext][id] */ HRESULT STDMETHODCALLTYPE _Record_Cancel_Proxy( 
    _ADORecord __RPC_FAR * This);
void __RPC_STUB _Record_Cancel_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);
#endif 	/* ___Record_INTERFACE_DEFINED__ */
EXTERN_C const CLSID CLSID_Record;
#ifdef __cplusplus
Record;
#endif
#ifndef ___Stream_INTERFACE_DEFINED__
#define ___Stream_INTERFACE_DEFINED__
/* interface _ADOStream */
/* [object][helpcontext][uuid][hidden][dual] */ 
EXTERN_C const IID IID__Stream;
#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("00000565-0000-0010-8000-00AA006D2EA4")
    _ADOStream : public IDispatch
    {
    public:
        virtual /* [helpcontext][id][propget] */ HRESULT STDMETHODCALLTYPE get_Size( 
            /* [retval][out] */ LONG __RPC_FAR *pSize) = 0;
        
        virtual /* [helpcontext][id][propget] */ HRESULT STDMETHODCALLTYPE get_EOS( 
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pEOS) = 0;
        
        virtual /* [helpcontext][id][propget] */ HRESULT STDMETHODCALLTYPE get_Position( 
            /* [retval][out] */ LONG __RPC_FAR *pPos) = 0;
        
        virtual /* [helpcontext][id][propput] */ HRESULT STDMETHODCALLTYPE put_Position( 
            /* [in] */ LONG Position) = 0;
        
        virtual /* [helpcontext][id][propget] */ HRESULT STDMETHODCALLTYPE get_Type( 
            /* [retval][out] */ StreamTypeEnum __RPC_FAR *pType) = 0;
        
        virtual /* [helpcontext][id][propput] */ HRESULT STDMETHODCALLTYPE put_Type( 
            /* [in] */ StreamTypeEnum Type) = 0;
        
        virtual /* [helpcontext][id][propget] */ HRESULT STDMETHODCALLTYPE get_LineSeparator( 
            /* [retval][out] */ LineSeparatorEnum __RPC_FAR *pLS) = 0;
        
        virtual /* [helpcontext][id][propput] */ HRESULT STDMETHODCALLTYPE put_LineSeparator( 
            /* [in] */ LineSeparatorEnum LineSeparator) = 0;
        
        virtual /* [helpcontext][id][propget] */ HRESULT STDMETHODCALLTYPE get_State( 
            /* [retval][out] */ ObjectStateEnum __RPC_FAR *pState) = 0;
        
        virtual /* [helpcontext][id][propget] */ HRESULT STDMETHODCALLTYPE get_Mode( 
            /* [retval][out] */ ConnectModeEnum __RPC_FAR *pMode) = 0;
        
        virtual /* [helpcontext][id][propput] */ HRESULT STDMETHODCALLTYPE put_Mode( 
            /* [in] */ ConnectModeEnum Mode) = 0;
        
        virtual /* [helpcontext][id][propget] */ HRESULT STDMETHODCALLTYPE get_Charset( 
            /* [retval][out] */ BSTR __RPC_FAR *pbstrCharset) = 0;
        
        virtual /* [helpcontext][id][propput] */ HRESULT STDMETHODCALLTYPE put_Charset( 
            /* [in] */ BSTR Charset) = 0;
        
        virtual /* [helpcontext][id] */ HRESULT STDMETHODCALLTYPE Read( 
            /* [defaultvalue][in] */ long NumBytes,
            /* [retval][out] */ VARIANT __RPC_FAR *pVal) = 0;
        
        virtual /* [helpcontext][id] */ HRESULT STDMETHODCALLTYPE Open( 
            /* [optional][in] */ VARIANT Source,
            /* [defaultvalue][in] */ ConnectModeEnum Mode,
            /* [defaultvalue][in] */ StreamOpenOptionsEnum Options,
            /* [optional][in] */ BSTR UserName,
            /* [optional][in] */ BSTR Password) = 0;
        
        virtual /* [helpcontext][id] */ HRESULT STDMETHODCALLTYPE Close( void) = 0;
        
        virtual /* [helpcontext][id] */ HRESULT STDMETHODCALLTYPE SkipLine( void) = 0;
        
        virtual /* [helpcontext][id] */ HRESULT STDMETHODCALLTYPE Write( 
            /* [in] */ VARIANT Buffer) = 0;
        
        virtual /* [helpcontext][id] */ HRESULT STDMETHODCALLTYPE SetEOS( void) = 0;
        
        virtual /* [helpcontext][id] */ HRESULT STDMETHODCALLTYPE CopyTo( 
            /* [in] */ _ADOStream __RPC_FAR *DestStream,
            /* [defaultvalue][in] */ LONG CharNumber = -1) = 0;
        
        virtual /* [helpcontext][id] */ HRESULT STDMETHODCALLTYPE Flush( void) = 0;
        
        virtual /* [helpcontext][id] */ HRESULT STDMETHODCALLTYPE SaveToFile( 
            /* [in] */ BSTR FileName,
            /* [defaultvalue][in] */ SaveOptionsEnum Options = adSaveCreateNotExist) = 0;
        
        virtual /* [helpcontext][id] */ HRESULT STDMETHODCALLTYPE LoadFromFile( 
            /* [in] */ BSTR FileName) = 0;
        
        virtual /* [helpcontext][id] */ HRESULT STDMETHODCALLTYPE ReadText( 
            /* [defaultvalue][in] */ long NumChars,
            /* [retval][out] */ BSTR __RPC_FAR *pbstr) = 0;
        
        virtual /* [helpcontext][id] */ HRESULT STDMETHODCALLTYPE WriteText( 
            /* [in] */ BSTR Data,
            /* [defaultvalue][in] */ StreamWriteEnum Options = adWriteChar) = 0;
        
        virtual /* [helpcontext][id] */ HRESULT STDMETHODCALLTYPE Cancel( void) = 0;
        
    };
    
#else 	/* C style interface */
    typedef struct _StreamVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            _ADOStream __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            _ADOStream __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            _ADOStream __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            _ADOStream __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            _ADOStream __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            _ADOStream __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            _ADOStream __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        /* [helpcontext][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Size )( 
            _ADOStream __RPC_FAR * This,
            /* [retval][out] */ LONG __RPC_FAR *pSize);
        
        /* [helpcontext][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_EOS )( 
            _ADOStream __RPC_FAR * This,
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pEOS);
        
        /* [helpcontext][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Position )( 
            _ADOStream __RPC_FAR * This,
            /* [retval][out] */ LONG __RPC_FAR *pPos);
        
        /* [helpcontext][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Position )( 
            _ADOStream __RPC_FAR * This,
            /* [in] */ LONG Position);
        
        /* [helpcontext][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Type )( 
            _ADOStream __RPC_FAR * This,
            /* [retval][out] */ StreamTypeEnum __RPC_FAR *pType);
        
        /* [helpcontext][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Type )( 
            _ADOStream __RPC_FAR * This,
            /* [in] */ StreamTypeEnum Type);
        
        /* [helpcontext][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_LineSeparator )( 
            _ADOStream __RPC_FAR * This,
            /* [retval][out] */ LineSeparatorEnum __RPC_FAR *pLS);
        
        /* [helpcontext][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_LineSeparator )( 
            _ADOStream __RPC_FAR * This,
            /* [in] */ LineSeparatorEnum LineSeparator);
        
        /* [helpcontext][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_State )( 
            _ADOStream __RPC_FAR * This,
            /* [retval][out] */ ObjectStateEnum __RPC_FAR *pState);
        
        /* [helpcontext][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Mode )( 
            _ADOStream __RPC_FAR * This,
            /* [retval][out] */ ConnectModeEnum __RPC_FAR *pMode);
        
        /* [helpcontext][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Mode )( 
            _ADOStream __RPC_FAR * This,
            /* [in] */ ConnectModeEnum Mode);
        
        /* [helpcontext][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Charset )( 
            _ADOStream __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pbstrCharset);
        
        /* [helpcontext][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Charset )( 
            _ADOStream __RPC_FAR * This,
            /* [in] */ BSTR Charset);
        
        /* [helpcontext][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Read )( 
            _ADOStream __RPC_FAR * This,
            /* [defaultvalue][in] */ long NumBytes,
            /* [retval][out] */ VARIANT __RPC_FAR *pVal);
        
        /* [helpcontext][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Open )( 
            _ADOStream __RPC_FAR * This,
            /* [optional][in] */ VARIANT Source,
            /* [defaultvalue][in] */ ConnectModeEnum Mode,
            /* [defaultvalue][in] */ StreamOpenOptionsEnum Options,
            /* [optional][in] */ BSTR UserName,
            /* [optional][in] */ BSTR Password);
        
        /* [helpcontext][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Close )( 
            _ADOStream __RPC_FAR * This);
        
        /* [helpcontext][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *SkipLine )( 
            _ADOStream __RPC_FAR * This);
        
        /* [helpcontext][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Write )( 
            _ADOStream __RPC_FAR * This,
            /* [in] */ VARIANT Buffer);
        
        /* [helpcontext][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *SetEOS )( 
            _ADOStream __RPC_FAR * This);
        
        /* [helpcontext][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *CopyTo )( 
            _ADOStream __RPC_FAR * This,
            /* [in] */ _ADOStream __RPC_FAR *DestStream,
            /* [defaultvalue][in] */ LONG CharNumber);
        
        /* [helpcontext][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Flush )( 
            _ADOStream __RPC_FAR * This);
        
        /* [helpcontext][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *SaveToFile )( 
            _ADOStream __RPC_FAR * This,
            /* [in] */ BSTR FileName,
            /* [defaultvalue][in] */ SaveOptionsEnum Options);
        
        /* [helpcontext][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *LoadFromFile )( 
            _ADOStream __RPC_FAR * This,
            /* [in] */ BSTR FileName);
        
        /* [helpcontext][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *ReadText )( 
            _ADOStream __RPC_FAR * This,
            /* [defaultvalue][in] */ long NumChars,
            /* [retval][out] */ BSTR __RPC_FAR *pbstr);
        
        /* [helpcontext][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *WriteText )( 
            _ADOStream __RPC_FAR * This,
            /* [in] */ BSTR Data,
            /* [defaultvalue][in] */ StreamWriteEnum Options);
        
        /* [helpcontext][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Cancel )( 
            _ADOStream __RPC_FAR * This);
        
        END_INTERFACE
    } _StreamVtbl;
    interface _Stream
    {
        CONST_VTBL struct _StreamVtbl __RPC_FAR *lpVtbl;
    };
    
#ifdef COBJMACROS
#define _Stream_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)
#define _Stream_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)
#define _Stream_Release(This)	\
    (This)->lpVtbl -> Release(This)
#define _Stream_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)
#define _Stream_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)
#define _Stream_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)
#define _Stream_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)
#define _Stream_get_Size(This,pSize)	\
    (This)->lpVtbl -> get_Size(This,pSize)
#define _Stream_get_EOS(This,pEOS)	\
    (This)->lpVtbl -> get_EOS(This,pEOS)
#define _Stream_get_Position(This,pPos)	\
    (This)->lpVtbl -> get_Position(This,pPos)
#define _Stream_put_Position(This,Position)	\
    (This)->lpVtbl -> put_Position(This,Position)
#define _Stream_get_Type(This,pType)	\
    (This)->lpVtbl -> get_Type(This,pType)
#define _Stream_put_Type(This,Type)	\
    (This)->lpVtbl -> put_Type(This,Type)
#define _Stream_get_LineSeparator(This,pLS)	\
    (This)->lpVtbl -> get_LineSeparator(This,pLS)
#define _Stream_put_LineSeparator(This,LineSeparator)	\
    (This)->lpVtbl -> put_LineSeparator(This,LineSeparator)
#define _Stream_get_State(This,pState)	\
    (This)->lpVtbl -> get_State(This,pState)
#define _Stream_get_Mode(This,pMode)	\
    (This)->lpVtbl -> get_Mode(This,pMode)
#define _Stream_put_Mode(This,Mode)	\
    (This)->lpVtbl -> put_Mode(This,Mode)
#define _Stream_get_Charset(This,pbstrCharset)	\
    (This)->lpVtbl -> get_Charset(This,pbstrCharset)
#define _Stream_put_Charset(This,Charset)	\
    (This)->lpVtbl -> put_Charset(This,Charset)
#define _Stream_Read(This,NumBytes,pVal)	\
    (This)->lpVtbl -> Read(This,NumBytes,pVal)
#define _Stream_Open(This,Source,Mode,Options,UserName,Password)	\
    (This)->lpVtbl -> Open(This,Source,Mode,Options,UserName,Password)
#define _Stream_Close(This)	\
    (This)->lpVtbl -> Close(This)
#define _Stream_SkipLine(This)	\
    (This)->lpVtbl -> SkipLine(This)
#define _Stream_Write(This,Buffer)	\
    (This)->lpVtbl -> Write(This,Buffer)
#define _Stream_SetEOS(This)	\
    (This)->lpVtbl -> SetEOS(This)
#define _Stream_CopyTo(This,DestStream,CharNumber)	\
    (This)->lpVtbl -> CopyTo(This,DestStream,CharNumber)
#define _Stream_Flush(This)	\
    (This)->lpVtbl -> Flush(This)
#define _Stream_SaveToFile(This,FileName,Options)	\
    (This)->lpVtbl -> SaveToFile(This,FileName,Options)
#define _Stream_LoadFromFile(This,FileName)	\
    (This)->lpVtbl -> LoadFromFile(This,FileName)
#define _Stream_ReadText(This,NumChars,pbstr)	\
    (This)->lpVtbl -> ReadText(This,NumChars,pbstr)
#define _Stream_WriteText(This,Data,Options)	\
    (This)->lpVtbl -> WriteText(This,Data,Options)
#define _Stream_Cancel(This)	\
    (This)->lpVtbl -> Cancel(This)
#endif /* COBJMACROS */
#endif 	/* C style interface */
/* [helpcontext][id][propget] */ HRESULT STDMETHODCALLTYPE _Stream_get_Size_Proxy( 
    _ADOStream __RPC_FAR * This,
    /* [retval][out] */ LONG __RPC_FAR *pSize);
void __RPC_STUB _Stream_get_Size_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);
/* [helpcontext][id][propget] */ HRESULT STDMETHODCALLTYPE _Stream_get_EOS_Proxy( 
    _ADOStream __RPC_FAR * This,
    /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pEOS);
void __RPC_STUB _Stream_get_EOS_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);
/* [helpcontext][id][propget] */ HRESULT STDMETHODCALLTYPE _Stream_get_Position_Proxy( 
    _ADOStream __RPC_FAR * This,
    /* [retval][out] */ LONG __RPC_FAR *pPos);
void __RPC_STUB _Stream_get_Position_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);
/* [helpcontext][id][propput] */ HRESULT STDMETHODCALLTYPE _Stream_put_Position_Proxy( 
    _ADOStream __RPC_FAR * This,
    /* [in] */ LONG Position);
void __RPC_STUB _Stream_put_Position_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);
/* [helpcontext][id][propget] */ HRESULT STDMETHODCALLTYPE _Stream_get_Type_Proxy( 
    _ADOStream __RPC_FAR * This,
    /* [retval][out] */ StreamTypeEnum __RPC_FAR *pType);
void __RPC_STUB _Stream_get_Type_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);
/* [helpcontext][id][propput] */ HRESULT STDMETHODCALLTYPE _Stream_put_Type_Proxy( 
    _ADOStream __RPC_FAR * This,
    /* [in] */ StreamTypeEnum Type);
void __RPC_STUB _Stream_put_Type_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);
/* [helpcontext][id][propget] */ HRESULT STDMETHODCALLTYPE _Stream_get_LineSeparator_Proxy( 
    _ADOStream __RPC_FAR * This,
    /* [retval][out] */ LineSeparatorEnum __RPC_FAR *pLS);
void __RPC_STUB _Stream_get_LineSeparator_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);
/* [helpcontext][id][propput] */ HRESULT STDMETHODCALLTYPE _Stream_put_LineSeparator_Proxy( 
    _ADOStream __RPC_FAR * This,
    /* [in] */ LineSeparatorEnum LineSeparator);
void __RPC_STUB _Stream_put_LineSeparator_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);
/* [helpcontext][id][propget] */ HRESULT STDMETHODCALLTYPE _Stream_get_State_Proxy( 
    _ADOStream __RPC_FAR * This,
    /* [retval][out] */ ObjectStateEnum __RPC_FAR *pState);
void __RPC_STUB _Stream_get_State_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);
/* [helpcontext][id][propget] */ HRESULT STDMETHODCALLTYPE _Stream_get_Mode_Proxy( 
    _ADOStream __RPC_FAR * This,
    /* [retval][out] */ ConnectModeEnum __RPC_FAR *pMode);
void __RPC_STUB _Stream_get_Mode_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);
/* [helpcontext][id][propput] */ HRESULT STDMETHODCALLTYPE _Stream_put_Mode_Proxy( 
    _ADOStream __RPC_FAR * This,
    /* [in] */ ConnectModeEnum Mode);
void __RPC_STUB _Stream_put_Mode_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);
/* [helpcontext][id][propget] */ HRESULT STDMETHODCALLTYPE _Stream_get_Charset_Proxy( 
    _ADOStream __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pbstrCharset);
void __RPC_STUB _Stream_get_Charset_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);
/* [helpcontext][id][propput] */ HRESULT STDMETHODCALLTYPE _Stream_put_Charset_Proxy( 
    _ADOStream __RPC_FAR * This,
    /* [in] */ BSTR Charset);
void __RPC_STUB _Stream_put_Charset_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);
/* [helpcontext][id] */ HRESULT STDMETHODCALLTYPE _Stream_Read_Proxy( 
    _ADOStream __RPC_FAR * This,
    /* [defaultvalue][in] */ long NumBytes,
    /* [retval][out] */ VARIANT __RPC_FAR *pVal);
void __RPC_STUB _Stream_Read_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);
/* [helpcontext][id] */ HRESULT STDMETHODCALLTYPE _Stream_Open_Proxy( 
    _ADOStream __RPC_FAR * This,
    /* [optional][in] */ VARIANT Source,
    /* [defaultvalue][in] */ ConnectModeEnum Mode,
    /* [defaultvalue][in] */ StreamOpenOptionsEnum Options,
    /* [optional][in] */ BSTR UserName,
    /* [optional][in] */ BSTR Password);
void __RPC_STUB _Stream_Open_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);
/* [helpcontext][id] */ HRESULT STDMETHODCALLTYPE _Stream_Close_Proxy( 
    _ADOStream __RPC_FAR * This);
void __RPC_STUB _Stream_Close_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);
/* [helpcontext][id] */ HRESULT STDMETHODCALLTYPE _Stream_SkipLine_Proxy( 
    _ADOStream __RPC_FAR * This);
void __RPC_STUB _Stream_SkipLine_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);
/* [helpcontext][id] */ HRESULT STDMETHODCALLTYPE _Stream_Write_Proxy( 
    _ADOStream __RPC_FAR * This,
    /* [in] */ VARIANT Buffer);
void __RPC_STUB _Stream_Write_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);
/* [helpcontext][id] */ HRESULT STDMETHODCALLTYPE _Stream_SetEOS_Proxy( 
    _ADOStream __RPC_FAR * This);
void __RPC_STUB _Stream_SetEOS_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);
/* [helpcontext][id] */ HRESULT STDMETHODCALLTYPE _Stream_CopyTo_Proxy( 
    _ADOStream __RPC_FAR * This,
    /* [in] */ _ADOStream __RPC_FAR *DestStream,
    /* [defaultvalue][in] */ LONG CharNumber);
void __RPC_STUB _Stream_CopyTo_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);
/* [helpcontext][id] */ HRESULT STDMETHODCALLTYPE _Stream_Flush_Proxy( 
    _ADOStream __RPC_FAR * This);
void __RPC_STUB _Stream_Flush_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);
/* [helpcontext][id] */ HRESULT STDMETHODCALLTYPE _Stream_SaveToFile_Proxy( 
    _ADOStream __RPC_FAR * This,
    /* [in] */ BSTR FileName,
    /* [defaultvalue][in] */ SaveOptionsEnum Options);
void __RPC_STUB _Stream_SaveToFile_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);
/* [helpcontext][id] */ HRESULT STDMETHODCALLTYPE _Stream_LoadFromFile_Proxy( 
    _ADOStream __RPC_FAR * This,
    /* [in] */ BSTR FileName);
void __RPC_STUB _Stream_LoadFromFile_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);
/* [helpcontext][id] */ HRESULT STDMETHODCALLTYPE _Stream_ReadText_Proxy( 
    _ADOStream __RPC_FAR * This,
    /* [defaultvalue][in] */ long NumChars,
    /* [retval][out] */ BSTR __RPC_FAR *pbstr);
void __RPC_STUB _Stream_ReadText_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);
/* [helpcontext][id] */ HRESULT STDMETHODCALLTYPE _Stream_WriteText_Proxy( 
    _ADOStream __RPC_FAR * This,
    /* [in] */ BSTR Data,
    /* [defaultvalue][in] */ StreamWriteEnum Options);
void __RPC_STUB _Stream_WriteText_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);
/* [helpcontext][id] */ HRESULT STDMETHODCALLTYPE _Stream_Cancel_Proxy( 
    _ADOStream __RPC_FAR * This);
void __RPC_STUB _Stream_Cancel_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);
#endif 	/* ___Stream_INTERFACE_DEFINED__ */
EXTERN_C const CLSID CLSID_Stream;
#ifdef __cplusplus
Stream;
#endif
#ifndef __ADORecordConstruction_INTERFACE_DEFINED__
#define __ADORecordConstruction_INTERFACE_DEFINED__
/* interface ADORecordConstruction */
/* [object][uuid][restricted] */ 
EXTERN_C const IID IID_ADORecordConstruction;
#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("00000567-0000-0010-8000-00AA006D2EA4")
    ADORecordConstruction : public IDispatch
    {
    public:
        virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_Row( 
            /* [retval][out] */ IUnknown __RPC_FAR *__RPC_FAR *ppRow) = 0;
        
        virtual /* [propput] */ HRESULT STDMETHODCALLTYPE put_Row( 
            /* [in] */ IUnknown __RPC_FAR *pRow) = 0;
        
        virtual /* [propput] */ HRESULT STDMETHODCALLTYPE put_ParentRow( 
            /* [in] */ IUnknown __RPC_FAR *pRow) = 0;
        
    };
    
#else 	/* C style interface */
    typedef struct ADORecordConstructionVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            ADORecordConstruction __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            ADORecordConstruction __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            ADORecordConstruction __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            ADORecordConstruction __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            ADORecordConstruction __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            ADORecordConstruction __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            ADORecordConstruction __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Row )( 
            ADORecordConstruction __RPC_FAR * This,
            /* [retval][out] */ IUnknown __RPC_FAR *__RPC_FAR *ppRow);
        
        /* [propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Row )( 
            ADORecordConstruction __RPC_FAR * This,
            /* [in] */ IUnknown __RPC_FAR *pRow);
        
        /* [propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_ParentRow )( 
            ADORecordConstruction __RPC_FAR * This,
            /* [in] */ IUnknown __RPC_FAR *pRow);
        
        END_INTERFACE
    } ADORecordConstructionVtbl;
    interface ADORecordConstruction
    {
        CONST_VTBL struct ADORecordConstructionVtbl __RPC_FAR *lpVtbl;
    };
    
#ifdef COBJMACROS
#define ADORecordConstruction_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)
#define ADORecordConstruction_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)
#define ADORecordConstruction_Release(This)	\
    (This)->lpVtbl -> Release(This)
#define ADORecordConstruction_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)
#define ADORecordConstruction_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)
#define ADORecordConstruction_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)
#define ADORecordConstruction_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)
#define ADORecordConstruction_get_Row(This,ppRow)	\
    (This)->lpVtbl -> get_Row(This,ppRow)
#define ADORecordConstruction_put_Row(This,pRow)	\
    (This)->lpVtbl -> put_Row(This,pRow)
#define ADORecordConstruction_put_ParentRow(This,pRow)	\
    (This)->lpVtbl -> put_ParentRow(This,pRow)
#endif /* COBJMACROS */
#endif 	/* C style interface */
/* [propget] */ HRESULT STDMETHODCALLTYPE ADORecordConstruction_get_Row_Proxy( 
    ADORecordConstruction __RPC_FAR * This,
    /* [retval][out] */ IUnknown __RPC_FAR *__RPC_FAR *ppRow);
void __RPC_STUB ADORecordConstruction_get_Row_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);
/* [propput] */ HRESULT STDMETHODCALLTYPE ADORecordConstruction_put_Row_Proxy( 
    ADORecordConstruction __RPC_FAR * This,
    /* [in] */ IUnknown __RPC_FAR *pRow);
void __RPC_STUB ADORecordConstruction_put_Row_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);
/* [propput] */ HRESULT STDMETHODCALLTYPE ADORecordConstruction_put_ParentRow_Proxy( 
    ADORecordConstruction __RPC_FAR * This,
    /* [in] */ IUnknown __RPC_FAR *pRow);
void __RPC_STUB ADORecordConstruction_put_ParentRow_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);
#endif 	/* __ADORecordConstruction_INTERFACE_DEFINED__ */
#ifndef __ADOStreamConstruction_INTERFACE_DEFINED__
#define __ADOStreamConstruction_INTERFACE_DEFINED__
/* interface ADOStreamConstruction */
/* [object][uuid][restricted] */ 
EXTERN_C const IID IID_ADOStreamConstruction;
#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("00000568-0000-0010-8000-00AA006D2EA4")
    ADOStreamConstruction : public IDispatch
    {
    public:
        virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_Stream( 
            /* [retval][out] */ IUnknown __RPC_FAR *__RPC_FAR *ppStm) = 0;
        
        virtual /* [propput] */ HRESULT STDMETHODCALLTYPE put_Stream( 
            /* [in] */ IUnknown __RPC_FAR *pStm) = 0;
        
    };
    
#else 	/* C style interface */
    typedef struct ADOStreamConstructionVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            ADOStreamConstruction __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            ADOStreamConstruction __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            ADOStreamConstruction __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            ADOStreamConstruction __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            ADOStreamConstruction __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            ADOStreamConstruction __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            ADOStreamConstruction __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Stream )( 
            ADOStreamConstruction __RPC_FAR * This,
            /* [retval][out] */ IUnknown __RPC_FAR *__RPC_FAR *ppStm);
        
        /* [propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Stream )( 
            ADOStreamConstruction __RPC_FAR * This,
            /* [in] */ IUnknown __RPC_FAR *pStm);
        
        END_INTERFACE
    } ADOStreamConstructionVtbl;
    interface ADOStreamConstruction
    {
        CONST_VTBL struct ADOStreamConstructionVtbl __RPC_FAR *lpVtbl;
    };
    
#ifdef COBJMACROS
#define ADOStreamConstruction_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)
#define ADOStreamConstruction_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)
#define ADOStreamConstruction_Release(This)	\
    (This)->lpVtbl -> Release(This)
#define ADOStreamConstruction_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)
#define ADOStreamConstruction_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)
#define ADOStreamConstruction_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)
#define ADOStreamConstruction_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)
#define ADOStreamConstruction_get_Stream(This,ppStm)	\
    (This)->lpVtbl -> get_Stream(This,ppStm)
#define ADOStreamConstruction_put_Stream(This,pStm)	\
    (This)->lpVtbl -> put_Stream(This,pStm)
#endif /* COBJMACROS */
#endif 	/* C style interface */
/* [propget] */ HRESULT STDMETHODCALLTYPE ADOStreamConstruction_get_Stream_Proxy( 
    ADOStreamConstruction __RPC_FAR * This,
    /* [retval][out] */ IUnknown __RPC_FAR *__RPC_FAR *ppStm);
void __RPC_STUB ADOStreamConstruction_get_Stream_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);
/* [propput] */ HRESULT STDMETHODCALLTYPE ADOStreamConstruction_put_Stream_Proxy( 
    ADOStreamConstruction __RPC_FAR * This,
    /* [in] */ IUnknown __RPC_FAR *pStm);
void __RPC_STUB ADOStreamConstruction_put_Stream_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);
#endif 	/* __ADOStreamConstruction_INTERFACE_DEFINED__ */
#ifndef __ADOCommandConstruction_INTERFACE_DEFINED__
#define __ADOCommandConstruction_INTERFACE_DEFINED__
/* interface ADOCommandConstruction */
/* [object][uuid][restricted] */ 
EXTERN_C const IID IID_ADOCommandConstruction;
#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("00000517-0000-0010-8000-00AA006D2EA4")
    ADOCommandConstruction : public IUnknown
    {
    public:
        virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_OLEDBCommand( 
            /* [retval][out] */ IUnknown __RPC_FAR *__RPC_FAR *ppOLEDBCommand) = 0;
        
        virtual /* [propput] */ HRESULT STDMETHODCALLTYPE put_OLEDBCommand( 
            /* [in] */ IUnknown __RPC_FAR *pOLEDBCommand) = 0;
        
    };
    
#else 	/* C style interface */
    typedef struct ADOCommandConstructionVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            ADOCommandConstruction __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            ADOCommandConstruction __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            ADOCommandConstruction __RPC_FAR * This);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_OLEDBCommand )( 
            ADOCommandConstruction __RPC_FAR * This,
            /* [retval][out] */ IUnknown __RPC_FAR *__RPC_FAR *ppOLEDBCommand);
        
        /* [propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_OLEDBCommand )( 
            ADOCommandConstruction __RPC_FAR * This,
            /* [in] */ IUnknown __RPC_FAR *pOLEDBCommand);
        
        END_INTERFACE
    } ADOCommandConstructionVtbl;
    interface ADOCommandConstruction
    {
        CONST_VTBL struct ADOCommandConstructionVtbl __RPC_FAR *lpVtbl;
    };
    
#ifdef COBJMACROS
#define ADOCommandConstruction_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)
#define ADOCommandConstruction_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)
#define ADOCommandConstruction_Release(This)	\
    (This)->lpVtbl -> Release(This)
#define ADOCommandConstruction_get_OLEDBCommand(This,ppOLEDBCommand)	\
    (This)->lpVtbl -> get_OLEDBCommand(This,ppOLEDBCommand)
#define ADOCommandConstruction_put_OLEDBCommand(This,pOLEDBCommand)	\
    (This)->lpVtbl -> put_OLEDBCommand(This,pOLEDBCommand)
#endif /* COBJMACROS */
#endif 	/* C style interface */
/* [propget] */ HRESULT STDMETHODCALLTYPE ADOCommandConstruction_get_OLEDBCommand_Proxy( 
    ADOCommandConstruction __RPC_FAR * This,
    /* [retval][out] */ IUnknown __RPC_FAR *__RPC_FAR *ppOLEDBCommand);
void __RPC_STUB ADOCommandConstruction_get_OLEDBCommand_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);
/* [propput] */ HRESULT STDMETHODCALLTYPE ADOCommandConstruction_put_OLEDBCommand_Proxy( 
    ADOCommandConstruction __RPC_FAR * This,
    /* [in] */ IUnknown __RPC_FAR *pOLEDBCommand);
void __RPC_STUB ADOCommandConstruction_put_OLEDBCommand_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);
#endif 	/* __ADOCommandConstruction_INTERFACE_DEFINED__ */
EXTERN_C const CLSID CLSID_Command;
#ifdef __cplusplus
Command;
#endif
EXTERN_C const CLSID CLSID_Recordset;
#ifdef __cplusplus
Recordset;
#endif
#ifndef __Recordset15_INTERFACE_DEFINED__
#define __Recordset15_INTERFACE_DEFINED__
/* interface Recordset15 */
/* [object][helpcontext][uuid][nonextensible][hidden][dual] */ 
EXTERN_C const IID IID_Recordset15;
#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("0000050E-0000-0010-8000-00AA006D2EA4")
    Recordset15 : public _ADO
    {
    public:
        virtual /* [helpcontext][propget][id] */ HRESULT STDMETHODCALLTYPE get_AbsolutePosition( 
            /* [retval][out] */ PositionEnum __RPC_FAR *pl) = 0;
        
        virtual /* [helpcontext][propput][id] */ HRESULT STDMETHODCALLTYPE put_AbsolutePosition( 
            /* [in] */ PositionEnum Position) = 0;
        
        virtual /* [helpcontext][propputref][id] */ HRESULT STDMETHODCALLTYPE putref_ActiveConnection( 
            /* [in] */ IDispatch __RPC_FAR *pconn) = 0;
        
        virtual /* [helpcontext][propput][id] */ HRESULT STDMETHODCALLTYPE put_ActiveConnection( 
            /* [in] */ VARIANT vConn) = 0;
        
        virtual /* [helpcontext][propget][id] */ HRESULT STDMETHODCALLTYPE get_ActiveConnection( 
            /* [retval][out] */ VARIANT __RPC_FAR *pvar) = 0;
        
        virtual /* [helpcontext][propget][id] */ HRESULT STDMETHODCALLTYPE get_BOF( 
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pb) = 0;
        
        virtual /* [helpcontext][propget][id] */ HRESULT STDMETHODCALLTYPE get_Bookmark( 
            /* [retval][out] */ VARIANT __RPC_FAR *pvBookmark) = 0;
        
        virtual /* [helpcontext][propput][id] */ HRESULT STDMETHODCALLTYPE put_Bookmark( 
            /* [in] */ VARIANT vBookmark) = 0;
        
        virtual /* [helpcontext][propget][id] */ HRESULT STDMETHODCALLTYPE get_CacheSize( 
            /* [retval][out] */ long __RPC_FAR *pl) = 0;
        
        virtual /* [helpcontext][propput][id] */ HRESULT STDMETHODCALLTYPE put_CacheSize( 
            /* [in] */ long CacheSize) = 0;
        
        virtual /* [helpcontext][propget][id] */ HRESULT STDMETHODCALLTYPE get_CursorType( 
            /* [retval][out] */ CursorTypeEnum __RPC_FAR *plCursorType) = 0;
        
        virtual /* [helpcontext][propput][id] */ HRESULT STDMETHODCALLTYPE put_CursorType( 
            /* [in] */ CursorTypeEnum lCursorType) = 0;
        
        virtual /* [helpcontext][propget][id] */ HRESULT STDMETHODCALLTYPE get_EOF( 
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pb) = 0;
        
        virtual /* [helpcontext][propget][id] */ HRESULT STDMETHODCALLTYPE get_Fields( 
            /* [retval][out] */ ADOFields __RPC_FAR *__RPC_FAR *ppvObject) = 0;
        
        virtual /* [helpcontext][propget][id] */ HRESULT STDMETHODCALLTYPE get_LockType( 
            /* [retval][out] */ LockTypeEnum __RPC_FAR *plLockType) = 0;
        
        virtual /* [helpcontext][propput][id] */ HRESULT STDMETHODCALLTYPE put_LockType( 
            /* [in] */ LockTypeEnum lLockType) = 0;
        
        virtual /* [helpcontext][propget][id] */ HRESULT STDMETHODCALLTYPE get_MaxRecords( 
            /* [retval][out] */ long __RPC_FAR *plMaxRecords) = 0;
        
        virtual /* [helpcontext][propput][id] */ HRESULT STDMETHODCALLTYPE put_MaxRecords( 
            /* [in] */ long lMaxRecords) = 0;
        
        virtual /* [helpcontext][propget][id] */ HRESULT STDMETHODCALLTYPE get_RecordCount( 
            /* [retval][out] */ long __RPC_FAR *pl) = 0;
        
        virtual /* [helpcontext][propputref][id] */ HRESULT STDMETHODCALLTYPE putref_Source( 
            /* [in] */ IDispatch __RPC_FAR *pcmd) = 0;
        
        virtual /* [helpcontext][propput][id] */ HRESULT STDMETHODCALLTYPE put_Source( 
            /* [in] */ BSTR bstrConn) = 0;
        
        virtual /* [helpcontext][propget][id] */ HRESULT STDMETHODCALLTYPE get_Source( 
            /* [retval][out] */ VARIANT __RPC_FAR *pvSource) = 0;
        
        virtual /* [helpcontext][id] */ HRESULT STDMETHODCALLTYPE AddNew( 
            /* [optional][in] */ VARIANT FieldList,
            /* [optional][in] */ VARIANT Values) = 0;
        
        virtual /* [helpcontext][id] */ HRESULT STDMETHODCALLTYPE CancelUpdate( void) = 0;
        
        virtual /* [helpcontext][id] */ HRESULT STDMETHODCALLTYPE Close( void) = 0;
        
        virtual /* [helpcontext][id] */ HRESULT STDMETHODCALLTYPE Delete( 
            /* [defaultvalue][in] */ AffectEnum AffectRecords = adAffectCurrent) = 0;
        
        virtual /* [helpcontext][id] */ HRESULT STDMETHODCALLTYPE GetRows( 
            /* [defaultvalue][in] */ long Rows,
            /* [optional][in] */ VARIANT Start,
            /* [optional][in] */ VARIANT Fields,
            /* [retval][out] */ VARIANT __RPC_FAR *pvar) = 0;
        
        virtual /* [helpcontext][id] */ HRESULT STDMETHODCALLTYPE Move( 
            /* [in] */ long NumRecords,
            /* [optional][in] */ VARIANT Start) = 0;
        
        virtual /* [helpcontext][id] */ HRESULT STDMETHODCALLTYPE MoveNext( void) = 0;
        
        virtual /* [helpcontext][id] */ HRESULT STDMETHODCALLTYPE MovePrevious( void) = 0;
        
        virtual /* [helpcontext][id] */ HRESULT STDMETHODCALLTYPE MoveFirst( void) = 0;
        
        virtual /* [helpcontext][id] */ HRESULT STDMETHODCALLTYPE MoveLast( void) = 0;
        
        virtual /* [helpcontext][id] */ HRESULT STDMETHODCALLTYPE Open( 
            /* [optional][in] */ VARIANT Source,
            /* [optional][in] */ VARIANT ActiveConnection,
            /* [defaultvalue][in] */ CursorTypeEnum CursorType = adOpenUnspecified,
            /* [defaultvalue][in] */ LockTypeEnum LockType = adLockUnspecified,
            /* [defaultvalue][in] */ LONG Options = adCmdUnspecified) = 0;
        
        virtual /* [helpcontext][id] */ HRESULT STDMETHODCALLTYPE Requery( 
            /* [defaultvalue][in] */ LONG Options = adOptionUnspecified) = 0;
        
        virtual /* [hidden] */ HRESULT STDMETHODCALLTYPE _xResync( 
            /* [defaultvalue][in] */ AffectEnum AffectRecords = adAffectAll) = 0;
        
        virtual /* [helpcontext][id] */ HRESULT STDMETHODCALLTYPE Update( 
            /* [optional][in] */ VARIANT Fields,
            /* [optional][in] */ VARIANT Values) = 0;
        
        virtual /* [helpcontext][propget][id] */ HRESULT STDMETHODCALLTYPE get_AbsolutePage( 
            /* [retval][out] */ PositionEnum __RPC_FAR *pl) = 0;
        
        virtual /* [helpcontext][propput][id] */ HRESULT STDMETHODCALLTYPE put_AbsolutePage( 
            /* [in] */ PositionEnum Page) = 0;
        
        virtual /* [helpcontext][propget][id] */ HRESULT STDMETHODCALLTYPE get_EditMode( 
            /* [retval][out] */ EditModeEnum __RPC_FAR *pl) = 0;
        
        virtual /* [helpcontext][propget][id] */ HRESULT STDMETHODCALLTYPE get_Filter( 
            /* [retval][out] */ VARIANT __RPC_FAR *Criteria) = 0;
        
        virtual /* [helpcontext][propput][id] */ HRESULT STDMETHODCALLTYPE put_Filter( 
            /* [in] */ VARIANT Criteria) = 0;
        
        virtual /* [helpcontext][propget][id] */ HRESULT STDMETHODCALLTYPE get_PageCount( 
            /* [retval][out] */ long __RPC_FAR *pl) = 0;
        
        virtual /* [helpcontext][propget][id] */ HRESULT STDMETHODCALLTYPE get_PageSize( 
            /* [retval][out] */ long __RPC_FAR *pl) = 0;
        
        virtual /* [helpcontext][propput][id] */ HRESULT STDMETHODCALLTYPE put_PageSize( 
            /* [in] */ long PageSize) = 0;
        
        virtual /* [helpcontext][propget][id] */ HRESULT STDMETHODCALLTYPE get_Sort( 
            /* [retval][out] */ BSTR __RPC_FAR *Criteria) = 0;
        
        virtual /* [helpcontext][propput][id] */ HRESULT STDMETHODCALLTYPE put_Sort( 
            /* [in] */ BSTR Criteria) = 0;
        
        virtual /* [helpcontext][propget][id] */ HRESULT STDMETHODCALLTYPE get_Status( 
            /* [retval][out] */ long __RPC_FAR *pl) = 0;
        
        virtual /* [helpcontext][propget][id] */ HRESULT STDMETHODCALLTYPE get_State( 
            /* [retval][out] */ LONG __RPC_FAR *plObjState) = 0;
        
        virtual /* [hidden] */ HRESULT STDMETHODCALLTYPE _xClone( 
            /* [retval][out] */ _ADORecordset __RPC_FAR *__RPC_FAR *ppvObject) = 0;
        
        virtual /* [helpcontext][id] */ HRESULT STDMETHODCALLTYPE UpdateBatch( 
            /* [defaultvalue][in] */ AffectEnum AffectRecords = adAffectAll) = 0;
        
        virtual /* [helpcontext][id] */ HRESULT STDMETHODCALLTYPE CancelBatch( 
            /* [defaultvalue][in] */ AffectEnum AffectRecords = adAffectAll) = 0;
        
        virtual /* [helpcontext][propget][id] */ HRESULT STDMETHODCALLTYPE get_CursorLocation( 
            /* [retval][out] */ CursorLocationEnum __RPC_FAR *plCursorLoc) = 0;
        
        virtual /* [helpcontext][propput][id] */ HRESULT STDMETHODCALLTYPE put_CursorLocation( 
            /* [in] */ CursorLocationEnum lCursorLoc) = 0;
        
        virtual /* [helpcontext][id] */ HRESULT STDMETHODCALLTYPE NextRecordset( 
            /* [optional][out] */ VARIANT __RPC_FAR *RecordsAffected,
            /* [retval][out] */ _ADORecordset __RPC_FAR *__RPC_FAR *ppiRs) = 0;
        
        virtual /* [helpcontext][id] */ HRESULT STDMETHODCALLTYPE Supports( 
            /* [in] */ CursorOptionEnum CursorOptions,
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pb) = 0;
        
        virtual /* [hidden][id][propget] */ HRESULT STDMETHODCALLTYPE get_Collect( 
            /* [in] */ VARIANT Index,
            /* [retval][out] */ VARIANT __RPC_FAR *pvar) = 0;
        
        virtual /* [hidden][id][propput] */ HRESULT STDMETHODCALLTYPE put_Collect( 
            /* [in] */ VARIANT Index,
            /* [in] */ VARIANT value) = 0;
        
        virtual /* [helpcontext][propget][id] */ HRESULT STDMETHODCALLTYPE get_MarshalOptions( 
            /* [retval][out] */ MarshalOptionsEnum __RPC_FAR *peMarshal) = 0;
        
        virtual /* [helpcontext][propput][id] */ HRESULT STDMETHODCALLTYPE put_MarshalOptions( 
            /* [in] */ MarshalOptionsEnum eMarshal) = 0;
        
        virtual /* [helpcontext][id] */ HRESULT STDMETHODCALLTYPE Find( 
            /* [in] */ BSTR Criteria,
            /* [defaultvalue][in] */ LONG SkipRecords,
            /* [defaultvalue][in] */ SearchDirectionEnum SearchDirection,
            /* [optional][in] */ VARIANT Start) = 0;
        
    };
    
#else 	/* C style interface */
    typedef struct Recordset15Vtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            Recordset15 __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            Recordset15 __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            Recordset15 __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            Recordset15 __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            Recordset15 __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            Recordset15 __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            Recordset15 __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        /* [helpcontext][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Properties )( 
            Recordset15 __RPC_FAR * This,
            /* [retval][out] */ ADOProperties __RPC_FAR *__RPC_FAR *ppvObject);
        
        /* [helpcontext][propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_AbsolutePosition )( 
            Recordset15 __RPC_FAR * This,
            /* [retval][out] */ PositionEnum __RPC_FAR *pl);
        
        /* [helpcontext][propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_AbsolutePosition )( 
            Recordset15 __RPC_FAR * This,
            /* [in] */ PositionEnum Position);
        
        /* [helpcontext][propputref][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *putref_ActiveADOConnection )( 
            Recordset15 __RPC_FAR * This,
            /* [in] */ IDispatch __RPC_FAR *pconn);
        
        /* [helpcontext][propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_ActiveConnection )( 
            Recordset15 __RPC_FAR * This,
            /* [in] */ VARIANT vConn);
        
        /* [helpcontext][propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_ActiveConnection )( 
            Recordset15 __RPC_FAR * This,
            /* [retval][out] */ VARIANT __RPC_FAR *pvar);
        
        /* [helpcontext][propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_BOF )( 
            Recordset15 __RPC_FAR * This,
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pb);
        
        /* [helpcontext][propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Bookmark )( 
            Recordset15 __RPC_FAR * This,
            /* [retval][out] */ VARIANT __RPC_FAR *pvBookmark);
        
        /* [helpcontext][propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Bookmark )( 
            Recordset15 __RPC_FAR * This,
            /* [in] */ VARIANT vBookmark);
        
        /* [helpcontext][propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_CacheSize )( 
            Recordset15 __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *pl);
        
        /* [helpcontext][propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_CacheSize )( 
            Recordset15 __RPC_FAR * This,
            /* [in] */ long CacheSize);
        
        /* [helpcontext][propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_CursorType )( 
            Recordset15 __RPC_FAR * This,
            /* [retval][out] */ CursorTypeEnum __RPC_FAR *plCursorType);
        
        /* [helpcontext][propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_CursorType )( 
            Recordset15 __RPC_FAR * This,
            /* [in] */ CursorTypeEnum lCursorType);
        
        /* [helpcontext][propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_EOF )( 
            Recordset15 __RPC_FAR * This,
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pb);
        
        /* [helpcontext][propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Fields )( 
            Recordset15 __RPC_FAR * This,
            /* [retval][out] */ ADOFields __RPC_FAR *__RPC_FAR *ppvObject);
        
        /* [helpcontext][propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_LockType )( 
            Recordset15 __RPC_FAR * This,
            /* [retval][out] */ LockTypeEnum __RPC_FAR *plLockType);
        
        /* [helpcontext][propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_LockType )( 
            Recordset15 __RPC_FAR * This,
            /* [in] */ LockTypeEnum lLockType);
        
        /* [helpcontext][propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_MaxRecords )( 
            Recordset15 __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *plMaxRecords);
        
        /* [helpcontext][propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_MaxRecords )( 
            Recordset15 __RPC_FAR * This,
            /* [in] */ long lMaxRecords);
        
        /* [helpcontext][propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_RecordCount )( 
            Recordset15 __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *pl);
        
        /* [helpcontext][propputref][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *putref_Source )( 
            Recordset15 __RPC_FAR * This,
            /* [in] */ IDispatch __RPC_FAR *pcmd);
        
        /* [helpcontext][propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Source )( 
            Recordset15 __RPC_FAR * This,
            /* [in] */ BSTR bstrConn);
        
        /* [helpcontext][propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Source )( 
            Recordset15 __RPC_FAR * This,
            /* [retval][out] */ VARIANT __RPC_FAR *pvSource);
        
        /* [helpcontext][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *AddNew )( 
            Recordset15 __RPC_FAR * This,
            /* [optional][in] */ VARIANT FieldList,
            /* [optional][in] */ VARIANT Values);
        
        /* [helpcontext][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *CancelUpdate )( 
            Recordset15 __RPC_FAR * This);
        
        /* [helpcontext][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Close )( 
            Recordset15 __RPC_FAR * This);
        
        /* [helpcontext][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Delete )( 
            Recordset15 __RPC_FAR * This,
            /* [defaultvalue][in] */ AffectEnum AffectRecords);
        
        /* [helpcontext][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetRows )( 
            Recordset15 __RPC_FAR * This,
            /* [defaultvalue][in] */ long Rows,
            /* [optional][in] */ VARIANT Start,
            /* [optional][in] */ VARIANT Fields,
            /* [retval][out] */ VARIANT __RPC_FAR *pvar);
        
        /* [helpcontext][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Move )( 
            Recordset15 __RPC_FAR * This,
            /* [in] */ long NumRecords,
            /* [optional][in] */ VARIANT Start);
        
        /* [helpcontext][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *MoveNext )( 
            Recordset15 __RPC_FAR * This);
        
        /* [helpcontext][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *MovePrevious )( 
            Recordset15 __RPC_FAR * This);
        
        /* [helpcontext][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *MoveFirst )( 
            Recordset15 __RPC_FAR * This);
        
        /* [helpcontext][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *MoveLast )( 
            Recordset15 __RPC_FAR * This);
        
        /* [helpcontext][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Open )( 
            Recordset15 __RPC_FAR * This,
            /* [optional][in] */ VARIANT Source,
            /* [optional][in] */ VARIANT ActiveConnection,
            /* [defaultvalue][in] */ CursorTypeEnum CursorType,
            /* [defaultvalue][in] */ LockTypeEnum LockType,
            /* [defaultvalue][in] */ LONG Options);
        
        /* [helpcontext][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Requery )( 
            Recordset15 __RPC_FAR * This,
            /* [defaultvalue][in] */ LONG Options);
        
        /* [hidden] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *_xResync )( 
            Recordset15 __RPC_FAR * This,
            /* [defaultvalue][in] */ AffectEnum AffectRecords);
        
        /* [helpcontext][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Update )( 
            Recordset15 __RPC_FAR * This,
            /* [optional][in] */ VARIANT Fields,
            /* [optional][in] */ VARIANT Values);
        
        /* [helpcontext][propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_AbsolutePage )( 
            Recordset15 __RPC_FAR * This,
            /* [retval][out] */ PositionEnum __RPC_FAR *pl);
        
        /* [helpcontext][propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_AbsolutePage )( 
            Recordset15 __RPC_FAR * This,
            /* [in] */ PositionEnum Page);
        
        /* [helpcontext][propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_EditMode )( 
            Recordset15 __RPC_FAR * This,
            /* [retval][out] */ EditModeEnum __RPC_FAR *pl);
        
        /* [helpcontext][propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Filter )( 
            Recordset15 __RPC_FAR * This,
            /* [retval][out] */ VARIANT __RPC_FAR *Criteria);
        
        /* [helpcontext][propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Filter )( 
            Recordset15 __RPC_FAR * This,
            /* [in] */ VARIANT Criteria);
        
        /* [helpcontext][propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_PageCount )( 
            Recordset15 __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *pl);
        
        /* [helpcontext][propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_PageSize )( 
            Recordset15 __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *pl);
        
        /* [helpcontext][propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_PageSize )( 
            Recordset15 __RPC_FAR * This,
            /* [in] */ long PageSize);
        
        /* [helpcontext][propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Sort )( 
            Recordset15 __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *Criteria);
        
        /* [helpcontext][propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Sort )( 
            Recordset15 __RPC_FAR * This,
            /* [in] */ BSTR Criteria);
        
        /* [helpcontext][propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Status )( 
            Recordset15 __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *pl);
        
        /* [helpcontext][propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_State )( 
            Recordset15 __RPC_FAR * This,
            /* [retval][out] */ LONG __RPC_FAR *plObjState);
        
        /* [hidden] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *_xClone )( 
            Recordset15 __RPC_FAR * This,
            /* [retval][out] */ _ADORecordset __RPC_FAR *__RPC_FAR *ppvObject);
        
        /* [helpcontext][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *UpdateBatch )( 
            Recordset15 __RPC_FAR * This,
            /* [defaultvalue][in] */ AffectEnum AffectRecords);
        
        /* [helpcontext][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *CancelBatch )( 
            Recordset15 __RPC_FAR * This,
            /* [defaultvalue][in] */ AffectEnum AffectRecords);
        
        /* [helpcontext][propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_CursorLocation )( 
            Recordset15 __RPC_FAR * This,
            /* [retval][out] */ CursorLocationEnum __RPC_FAR *plCursorLoc);
        
        /* [helpcontext][propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_CursorLocation )( 
            Recordset15 __RPC_FAR * This,
            /* [in] */ CursorLocationEnum lCursorLoc);
        
        /* [helpcontext][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *NextADORecordset )( 
            Recordset15 __RPC_FAR * This,
            /* [optional][out] */ VARIANT __RPC_FAR *RecordsAffected,
            /* [retval][out] */ _ADORecordset __RPC_FAR *__RPC_FAR *ppiRs);
        
        /* [helpcontext][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Supports )( 
            Recordset15 __RPC_FAR * This,
            /* [in] */ CursorOptionEnum CursorOptions,
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pb);
        
        /* [hidden][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Collect )( 
            Recordset15 __RPC_FAR * This,
            /* [in] */ VARIANT Index,
            /* [retval][out] */ VARIANT __RPC_FAR *pvar);
        
        /* [hidden][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Collect )( 
            Recordset15 __RPC_FAR * This,
            /* [in] */ VARIANT Index,
            /* [in] */ VARIANT value);
        
        /* [helpcontext][propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_MarshalOptions )( 
            Recordset15 __RPC_FAR * This,
            /* [retval][out] */ MarshalOptionsEnum __RPC_FAR *peMarshal);
        
        /* [helpcontext][propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_MarshalOptions )( 
            Recordset15 __RPC_FAR * This,
            /* [in] */ MarshalOptionsEnum eMarshal);
        
        /* [helpcontext][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Find )( 
            Recordset15 __RPC_FAR * This,
            /* [in] */ BSTR Criteria,
            /* [defaultvalue][in] */ LONG SkipRecords,
            /* [defaultvalue][in] */ SearchDirectionEnum SearchDirection,
            /* [optional][in] */ VARIANT Start);
        
        END_INTERFACE
    } Recordset15Vtbl;
    interface Recordset15
    {
        CONST_VTBL struct Recordset15Vtbl __RPC_FAR *lpVtbl;
    };
    
#ifdef COBJMACROS
#define Recordset15_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)
#define Recordset15_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)
#define Recordset15_Release(This)	\
    (This)->lpVtbl -> Release(This)
#define Recordset15_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)
#define Recordset15_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)
#define Recordset15_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)
#define Recordset15_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)
#define Recordset15_get_Properties(This,ppvObject)	\
    (This)->lpVtbl -> get_Properties(This,ppvObject)
#define Recordset15_get_AbsolutePosition(This,pl)	\
    (This)->lpVtbl -> get_AbsolutePosition(This,pl)
#define Recordset15_put_AbsolutePosition(This,Position)	\
    (This)->lpVtbl -> put_AbsolutePosition(This,Position)
#define Recordset15_putref_ActiveConnection(This,pconn)	\
    (This)->lpVtbl -> putref_ActiveConnection(This,pconn)
#define Recordset15_put_ActiveConnection(This,vConn)	\
    (This)->lpVtbl -> put_ActiveConnection(This,vConn)
#define Recordset15_get_ActiveConnection(This,pvar)	\
    (This)->lpVtbl -> get_ActiveConnection(This,pvar)
#define Recordset15_get_BOF(This,pb)	\
    (This)->lpVtbl -> get_BOF(This,pb)
#define Recordset15_get_Bookmark(This,pvBookmark)	\
    (This)->lpVtbl -> get_Bookmark(This,pvBookmark)
#define Recordset15_put_Bookmark(This,vBookmark)	\
    (This)->lpVtbl -> put_Bookmark(This,vBookmark)
#define Recordset15_get_CacheSize(This,pl)	\
    (This)->lpVtbl -> get_CacheSize(This,pl)
#define Recordset15_put_CacheSize(This,CacheSize)	\
    (This)->lpVtbl -> put_CacheSize(This,CacheSize)
#define Recordset15_get_CursorType(This,plCursorType)	\
    (This)->lpVtbl -> get_CursorType(This,plCursorType)
#define Recordset15_put_CursorType(This,lCursorType)	\
    (This)->lpVtbl -> put_CursorType(This,lCursorType)
#define Recordset15_get_EOF(This,pb)	\
    (This)->lpVtbl -> get_EOF(This,pb)
#define Recordset15_get_Fields(This,ppvObject)	\
    (This)->lpVtbl -> get_Fields(This,ppvObject)
#define Recordset15_get_LockType(This,plLockType)	\
    (This)->lpVtbl -> get_LockType(This,plLockType)
#define Recordset15_put_LockType(This,lLockType)	\
    (This)->lpVtbl -> put_LockType(This,lLockType)
#define Recordset15_get_MaxRecords(This,plMaxRecords)	\
    (This)->lpVtbl -> get_MaxRecords(This,plMaxRecords)
#define Recordset15_put_MaxRecords(This,lMaxRecords)	\
    (This)->lpVtbl -> put_MaxRecords(This,lMaxRecords)
#define Recordset15_get_RecordCount(This,pl)	\
    (This)->lpVtbl -> get_RecordCount(This,pl)
#define Recordset15_putref_Source(This,pcmd)	\
    (This)->lpVtbl -> putref_Source(This,pcmd)
#define Recordset15_put_Source(This,bstrConn)	\
    (This)->lpVtbl -> put_Source(This,bstrConn)
#define Recordset15_get_Source(This,pvSource)	\
    (This)->lpVtbl -> get_Source(This,pvSource)
#define Recordset15_AddNew(This,FieldList,Values)	\
    (This)->lpVtbl -> AddNew(This,FieldList,Values)
#define Recordset15_CancelUpdate(This)	\
    (This)->lpVtbl -> CancelUpdate(This)
#define Recordset15_Close(This)	\
    (This)->lpVtbl -> Close(This)
#define Recordset15_Delete(This,AffectRecords)	\
    (This)->lpVtbl -> Delete(This,AffectRecords)
#define Recordset15_GetRows(This,Rows,Start,Fields,pvar)	\
    (This)->lpVtbl -> GetRows(This,Rows,Start,Fields,pvar)
#define Recordset15_Move(This,NumRecords,Start)	\
    (This)->lpVtbl -> Move(This,NumRecords,Start)
#define Recordset15_MoveNext(This)	\
    (This)->lpVtbl -> MoveNext(This)
#define Recordset15_MovePrevious(This)	\
    (This)->lpVtbl -> MovePrevious(This)
#define Recordset15_MoveFirst(This)	\
    (This)->lpVtbl -> MoveFirst(This)
#define Recordset15_MoveLast(This)	\
    (This)->lpVtbl -> MoveLast(This)
#define Recordset15_Open(This,Source,ActiveConnection,CursorType,LockType,Options)	\
    (This)->lpVtbl -> Open(This,Source,ActiveConnection,CursorType,LockType,Options)
#define Recordset15_Requery(This,Options)	\
    (This)->lpVtbl -> Requery(This,Options)
#define Recordset15__xResync(This,AffectRecords)	\
    (This)->lpVtbl -> _xResync(This,AffectRecords)
#define Recordset15_Update(This,Fields,Values)	\
    (This)->lpVtbl -> Update(This,Fields,Values)
#define Recordset15_get_AbsolutePage(This,pl)	\
    (This)->lpVtbl -> get_AbsolutePage(This,pl)
#define Recordset15_put_AbsolutePage(This,Page)	\
    (This)->lpVtbl -> put_AbsolutePage(This,Page)
#define Recordset15_get_EditMode(This,pl)	\
    (This)->lpVtbl -> get_EditMode(This,pl)
#define Recordset15_get_Filter(This,Criteria)	\
    (This)->lpVtbl -> get_Filter(This,Criteria)
#define Recordset15_put_Filter(This,Criteria)	\
    (This)->lpVtbl -> put_Filter(This,Criteria)
#define Recordset15_get_PageCount(This,pl)	\
    (This)->lpVtbl -> get_PageCount(This,pl)
#define Recordset15_get_PageSize(This,pl)	\
    (This)->lpVtbl -> get_PageSize(This,pl)
#define Recordset15_put_PageSize(This,PageSize)	\
    (This)->lpVtbl -> put_PageSize(This,PageSize)
#define Recordset15_get_Sort(This,Criteria)	\
    (This)->lpVtbl -> get_Sort(This,Criteria)
#define Recordset15_put_Sort(This,Criteria)	\
    (This)->lpVtbl -> put_Sort(This,Criteria)
#define Recordset15_get_Status(This,pl)	\
    (This)->lpVtbl -> get_Status(This,pl)
#define Recordset15_get_State(This,plObjState)	\
    (This)->lpVtbl -> get_State(This,plObjState)
#define Recordset15__xClone(This,ppvObject)	\
    (This)->lpVtbl -> _xClone(This,ppvObject)
#define Recordset15_UpdateBatch(This,AffectRecords)	\
    (This)->lpVtbl -> UpdateBatch(This,AffectRecords)
#define Recordset15_CancelBatch(This,AffectRecords)	\
    (This)->lpVtbl -> CancelBatch(This,AffectRecords)
#define Recordset15_get_CursorLocation(This,plCursorLoc)	\
    (This)->lpVtbl -> get_CursorLocation(This,plCursorLoc)
#define Recordset15_put_CursorLocation(This,lCursorLoc)	\
    (This)->lpVtbl -> put_CursorLocation(This,lCursorLoc)
#define Recordset15_NextRecordset(This,RecordsAffected,ppiRs)	\
    (This)->lpVtbl -> NextRecordset(This,RecordsAffected,ppiRs)
#define Recordset15_Supports(This,CursorOptions,pb)	\
    (This)->lpVtbl -> Supports(This,CursorOptions,pb)
#define Recordset15_get_Collect(This,Index,pvar)	\
    (This)->lpVtbl -> get_Collect(This,Index,pvar)
#define Recordset15_put_Collect(This,Index,value)	\
    (This)->lpVtbl -> put_Collect(This,Index,value)
#define Recordset15_get_MarshalOptions(This,peMarshal)	\
    (This)->lpVtbl -> get_MarshalOptions(This,peMarshal)
#define Recordset15_put_MarshalOptions(This,eMarshal)	\
    (This)->lpVtbl -> put_MarshalOptions(This,eMarshal)
#define Recordset15_Find(This,Criteria,SkipRecords,SearchDirection,Start)	\
    (This)->lpVtbl -> Find(This,Criteria,SkipRecords,SearchDirection,Start)
#endif /* COBJMACROS */
#endif 	/* C style interface */
/* [helpcontext][propget][id] */ HRESULT STDMETHODCALLTYPE Recordset15_get_AbsolutePosition_Proxy( 
    Recordset15 __RPC_FAR * This,
    /* [retval][out] */ PositionEnum __RPC_FAR *pl);
void __RPC_STUB Recordset15_get_AbsolutePosition_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);
/* [helpcontext][propput][id] */ HRESULT STDMETHODCALLTYPE Recordset15_put_AbsolutePosition_Proxy( 
    Recordset15 __RPC_FAR * This,
    /* [in] */ PositionEnum Position);
void __RPC_STUB Recordset15_put_AbsolutePosition_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);
/* [helpcontext][propputref][id] */ HRESULT STDMETHODCALLTYPE Recordset15_putref_ActiveConnection_Proxy( 
    Recordset15 __RPC_FAR * This,
    /* [in] */ IDispatch __RPC_FAR *pconn);
void __RPC_STUB Recordset15_putref_ActiveConnection_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);
/* [helpcontext][propput][id] */ HRESULT STDMETHODCALLTYPE Recordset15_put_ActiveConnection_Proxy( 
    Recordset15 __RPC_FAR * This,
    /* [in] */ VARIANT vConn);
void __RPC_STUB Recordset15_put_ActiveConnection_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);
/* [helpcontext][propget][id] */ HRESULT STDMETHODCALLTYPE Recordset15_get_ActiveConnection_Proxy( 
    Recordset15 __RPC_FAR * This,
    /* [retval][out] */ VARIANT __RPC_FAR *pvar);
void __RPC_STUB Recordset15_get_ActiveConnection_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);
/* [helpcontext][propget][id] */ HRESULT STDMETHODCALLTYPE Recordset15_get_BOF_Proxy( 
    Recordset15 __RPC_FAR * This,
    /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pb);
void __RPC_STUB Recordset15_get_BOF_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);
/* [helpcontext][propget][id] */ HRESULT STDMETHODCALLTYPE Recordset15_get_Bookmark_Proxy( 
    Recordset15 __RPC_FAR * This,
    /* [retval][out] */ VARIANT __RPC_FAR *pvBookmark);
void __RPC_STUB Recordset15_get_Bookmark_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);
/* [helpcontext][propput][id] */ HRESULT STDMETHODCALLTYPE Recordset15_put_Bookmark_Proxy( 
    Recordset15 __RPC_FAR * This,
    /* [in] */ VARIANT vBookmark);
void __RPC_STUB Recordset15_put_Bookmark_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);
/* [helpcontext][propget][id] */ HRESULT STDMETHODCALLTYPE Recordset15_get_CacheSize_Proxy( 
    Recordset15 __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *pl);
void __RPC_STUB Recordset15_get_CacheSize_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);
/* [helpcontext][propput][id] */ HRESULT STDMETHODCALLTYPE Recordset15_put_CacheSize_Proxy( 
    Recordset15 __RPC_FAR * This,
    /* [in] */ long CacheSize);
void __RPC_STUB Recordset15_put_CacheSize_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);
/* [helpcontext][propget][id] */ HRESULT STDMETHODCALLTYPE Recordset15_get_CursorType_Proxy( 
    Recordset15 __RPC_FAR * This,
    /* [retval][out] */ CursorTypeEnum __RPC_FAR *plCursorType);
void __RPC_STUB Recordset15_get_CursorType_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);
/* [helpcontext][propput][id] */ HRESULT STDMETHODCALLTYPE Recordset15_put_CursorType_Proxy( 
    Recordset15 __RPC_FAR * This,
    /* [in] */ CursorTypeEnum lCursorType);
void __RPC_STUB Recordset15_put_CursorType_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);
/* [helpcontext][propget][id] */ HRESULT STDMETHODCALLTYPE Recordset15_get_EOF_Proxy( 
    Recordset15 __RPC_FAR * This,
    /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pb);
void __RPC_STUB Recordset15_get_EOF_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);
/* [helpcontext][propget][id] */ HRESULT STDMETHODCALLTYPE Recordset15_get_Fields_Proxy( 
    Recordset15 __RPC_FAR * This,
    /* [retval][out] */ ADOFields __RPC_FAR *__RPC_FAR *ppvObject);
void __RPC_STUB Recordset15_get_Fields_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);
/* [helpcontext][propget][id] */ HRESULT STDMETHODCALLTYPE Recordset15_get_LockType_Proxy( 
    Recordset15 __RPC_FAR * This,
    /* [retval][out] */ LockTypeEnum __RPC_FAR *plLockType);
void __RPC_STUB Recordset15_get_LockType_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);
/* [helpcontext][propput][id] */ HRESULT STDMETHODCALLTYPE Recordset15_put_LockType_Proxy( 
    Recordset15 __RPC_FAR * This,
    /* [in] */ LockTypeEnum lLockType);
void __RPC_STUB Recordset15_put_LockType_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);
/* [helpcontext][propget][id] */ HRESULT STDMETHODCALLTYPE Recordset15_get_MaxRecords_Proxy( 
    Recordset15 __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *plMaxRecords);
void __RPC_STUB Recordset15_get_MaxRecords_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);
/* [helpcontext][propput][id] */ HRESULT STDMETHODCALLTYPE Recordset15_put_MaxRecords_Proxy( 
    Recordset15 __RPC_FAR * This,
    /* [in] */ long lMaxRecords);
void __RPC_STUB Recordset15_put_MaxRecords_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);
/* [helpcontext][propget][id] */ HRESULT STDMETHODCALLTYPE Recordset15_get_RecordCount_Proxy( 
    Recordset15 __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *pl);
void __RPC_STUB Recordset15_get_RecordCount_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);
/* [helpcontext][propputref][id] */ HRESULT STDMETHODCALLTYPE Recordset15_putref_Source_Proxy( 
    Recordset15 __RPC_FAR * This,
    /* [in] */ IDispatch __RPC_FAR *pcmd);
void __RPC_STUB Recordset15_putref_Source_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);
/* [helpcontext][propput][id] */ HRESULT STDMETHODCALLTYPE Recordset15_put_Source_Proxy( 
    Recordset15 __RPC_FAR * This,
    /* [in] */ BSTR bstrConn);
void __RPC_STUB Recordset15_put_Source_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);
/* [helpcontext][propget][id] */ HRESULT STDMETHODCALLTYPE Recordset15_get_Source_Proxy( 
    Recordset15 __RPC_FAR * This,
    /* [retval][out] */ VARIANT __RPC_FAR *pvSource);
void __RPC_STUB Recordset15_get_Source_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);
/* [helpcontext][id] */ HRESULT STDMETHODCALLTYPE Recordset15_AddNew_Proxy( 
    Recordset15 __RPC_FAR * This,
    /* [optional][in] */ VARIANT FieldList,
    /* [optional][in] */ VARIANT Values);
void __RPC_STUB Recordset15_AddNew_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);
/* [helpcontext][id] */ HRESULT STDMETHODCALLTYPE Recordset15_CancelUpdate_Proxy( 
    Recordset15 __RPC_FAR * This);
void __RPC_STUB Recordset15_CancelUpdate_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);
/* [helpcontext][id] */ HRESULT STDMETHODCALLTYPE Recordset15_Close_Proxy( 
    Recordset15 __RPC_FAR * This);
void __RPC_STUB Recordset15_Close_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);
/* [helpcontext][id] */ HRESULT STDMETHODCALLTYPE Recordset15_Delete_Proxy( 
    Recordset15 __RPC_FAR * This,
    /* [defaultvalue][in] */ AffectEnum AffectRecords);
void __RPC_STUB Recordset15_Delete_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);
/* [helpcontext][id] */ HRESULT STDMETHODCALLTYPE Recordset15_GetRows_Proxy( 
    Recordset15 __RPC_FAR * This,
    /* [defaultvalue][in] */ long Rows,
    /* [optional][in] */ VARIANT Start,
    /* [optional][in] */ VARIANT Fields,
    /* [retval][out] */ VARIANT __RPC_FAR *pvar);
void __RPC_STUB Recordset15_GetRows_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);
/* [helpcontext][id] */ HRESULT STDMETHODCALLTYPE Recordset15_Move_Proxy( 
    Recordset15 __RPC_FAR * This,
    /* [in] */ long NumRecords,
    /* [optional][in] */ VARIANT Start);
void __RPC_STUB Recordset15_Move_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);
/* [helpcontext][id] */ HRESULT STDMETHODCALLTYPE Recordset15_MoveNext_Proxy( 
    Recordset15 __RPC_FAR * This);
void __RPC_STUB Recordset15_MoveNext_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);
/* [helpcontext][id] */ HRESULT STDMETHODCALLTYPE Recordset15_MovePrevious_Proxy( 
    Recordset15 __RPC_FAR * This);
void __RPC_STUB Recordset15_MovePrevious_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);
/* [helpcontext][id] */ HRESULT STDMETHODCALLTYPE Recordset15_MoveFirst_Proxy( 
    Recordset15 __RPC_FAR * This);
void __RPC_STUB Recordset15_MoveFirst_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);
/* [helpcontext][id] */ HRESULT STDMETHODCALLTYPE Recordset15_MoveLast_Proxy( 
    Recordset15 __RPC_FAR * This);
void __RPC_STUB Recordset15_MoveLast_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);
/* [helpcontext][id] */ HRESULT STDMETHODCALLTYPE Recordset15_Open_Proxy( 
    Recordset15 __RPC_FAR * This,
    /* [optional][in] */ VARIANT Source,
    /* [optional][in] */ VARIANT ActiveConnection,
    /* [defaultvalue][in] */ CursorTypeEnum CursorType,
    /* [defaultvalue][in] */ LockTypeEnum LockType,
    /* [defaultvalue][in] */ LONG Options);
void __RPC_STUB Recordset15_Open_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);
/* [helpcontext][id] */ HRESULT STDMETHODCALLTYPE Recordset15_Requery_Proxy( 
    Recordset15 __RPC_FAR * This,
    /* [defaultvalue][in] */ LONG Options);
void __RPC_STUB Recordset15_Requery_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);
/* [hidden] */ HRESULT STDMETHODCALLTYPE Recordset15__xResync_Proxy( 
    Recordset15 __RPC_FAR * This,
    /* [defaultvalue][in] */ AffectEnum AffectRecords);
void __RPC_STUB Recordset15__xResync_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);
/* [helpcontext][id] */ HRESULT STDMETHODCALLTYPE Recordset15_Update_Proxy( 
    Recordset15 __RPC_FAR * This,
    /* [optional][in] */ VARIANT Fields,
    /* [optional][in] */ VARIANT Values);
void __RPC_STUB Recordset15_Update_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);
/* [helpcontext][propget][id] */ HRESULT STDMETHODCALLTYPE Recordset15_get_AbsolutePage_Proxy( 
    Recordset15 __RPC_FAR * This,
    /* [retval][out] */ PositionEnum __RPC_FAR *pl);
void __RPC_STUB Recordset15_get_AbsolutePage_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);
/* [helpcontext][propput][id] */ HRESULT STDMETHODCALLTYPE Recordset15_put_AbsolutePage_Proxy( 
    Recordset15 __RPC_FAR * This,
    /* [in] */ PositionEnum Page);
void __RPC_STUB Recordset15_put_AbsolutePage_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);
/* [helpcontext][propget][id] */ HRESULT STDMETHODCALLTYPE Recordset15_get_EditMode_Proxy( 
    Recordset15 __RPC_FAR * This,
    /* [retval][out] */ EditModeEnum __RPC_FAR *pl);
void __RPC_STUB Recordset15_get_EditMode_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);
/* [helpcontext][propget][id] */ HRESULT STDMETHODCALLTYPE Recordset15_get_Filter_Proxy( 
    Recordset15 __RPC_FAR * This,
    /* [retval][out] */ VARIANT __RPC_FAR *Criteria);
void __RPC_STUB Recordset15_get_Filter_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);
/* [helpcontext][propput][id] */ HRESULT STDMETHODCALLTYPE Recordset15_put_Filter_Proxy( 
    Recordset15 __RPC_FAR * This,
    /* [in] */ VARIANT Criteria);
void __RPC_STUB Recordset15_put_Filter_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);
/* [helpcontext][propget][id] */ HRESULT STDMETHODCALLTYPE Recordset15_get_PageCount_Proxy( 
    Recordset15 __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *pl);
void __RPC_STUB Recordset15_get_PageCount_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);
/* [helpcontext][propget][id] */ HRESULT STDMETHODCALLTYPE Recordset15_get_PageSize_Proxy( 
    Recordset15 __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *pl);
void __RPC_STUB Recordset15_get_PageSize_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);
/* [helpcontext][propput][id] */ HRESULT STDMETHODCALLTYPE Recordset15_put_PageSize_Proxy( 
    Recordset15 __RPC_FAR * This,
    /* [in] */ long PageSize);
void __RPC_STUB Recordset15_put_PageSize_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);
/* [helpcontext][propget][id] */ HRESULT STDMETHODCALLTYPE Recordset15_get_Sort_Proxy( 
    Recordset15 __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *Criteria);
void __RPC_STUB Recordset15_get_Sort_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);
/* [helpcontext][propput][id] */ HRESULT STDMETHODCALLTYPE Recordset15_put_Sort_Proxy( 
    Recordset15 __RPC_FAR * This,
    /* [in] */ BSTR Criteria);
void __RPC_STUB Recordset15_put_Sort_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);
/* [helpcontext][propget][id] */ HRESULT STDMETHODCALLTYPE Recordset15_get_Status_Proxy( 
    Recordset15 __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *pl);
void __RPC_STUB Recordset15_get_Status_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);
/* [helpcontext][propget][id] */ HRESULT STDMETHODCALLTYPE Recordset15_get_State_Proxy( 
    Recordset15 __RPC_FAR * This,
    /* [retval][out] */ LONG __RPC_FAR *plObjState);
void __RPC_STUB Recordset15_get_State_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);
/* [hidden] */ HRESULT STDMETHODCALLTYPE Recordset15__xClone_Proxy( 
    Recordset15 __RPC_FAR * This,
    /* [retval][out] */ _ADORecordset __RPC_FAR *__RPC_FAR *ppvObject);
void __RPC_STUB Recordset15__xClone_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);
/* [helpcontext][id] */ HRESULT STDMETHODCALLTYPE Recordset15_UpdateBatch_Proxy( 
    Recordset15 __RPC_FAR * This,
    /* [defaultvalue][in] */ AffectEnum AffectRecords);
void __RPC_STUB Recordset15_UpdateBatch_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);
/* [helpcontext][id] */ HRESULT STDMETHODCALLTYPE Recordset15_CancelBatch_Proxy( 
    Recordset15 __RPC_FAR * This,
    /* [defaultvalue][in] */ AffectEnum AffectRecords);
void __RPC_STUB Recordset15_CancelBatch_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);
/* [helpcontext][propget][id] */ HRESULT STDMETHODCALLTYPE Recordset15_get_CursorLocation_Proxy( 
    Recordset15 __RPC_FAR * This,
    /* [retval][out] */ CursorLocationEnum __RPC_FAR *plCursorLoc);
void __RPC_STUB Recordset15_get_CursorLocation_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);
/* [helpcontext][propput][id] */ HRESULT STDMETHODCALLTYPE Recordset15_put_CursorLocation_Proxy( 
    Recordset15 __RPC_FAR * This,
    /* [in] */ CursorLocationEnum lCursorLoc);
void __RPC_STUB Recordset15_put_CursorLocation_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);
/* [helpcontext][id] */ HRESULT STDMETHODCALLTYPE Recordset15_NextRecordset_Proxy( 
    Recordset15 __RPC_FAR * This,
    /* [optional][out] */ VARIANT __RPC_FAR *RecordsAffected,
    /* [retval][out] */ _ADORecordset __RPC_FAR *__RPC_FAR *ppiRs);
void __RPC_STUB Recordset15_NextRecordset_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);
/* [helpcontext][id] */ HRESULT STDMETHODCALLTYPE Recordset15_Supports_Proxy( 
    Recordset15 __RPC_FAR * This,
    /* [in] */ CursorOptionEnum CursorOptions,
    /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pb);
void __RPC_STUB Recordset15_Supports_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);
/* [hidden][id][propget] */ HRESULT STDMETHODCALLTYPE Recordset15_get_Collect_Proxy( 
    Recordset15 __RPC_FAR * This,
    /* [in] */ VARIANT Index,
    /* [retval][out] */ VARIANT __RPC_FAR *pvar);
void __RPC_STUB Recordset15_get_Collect_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);
/* [hidden][id][propput] */ HRESULT STDMETHODCALLTYPE Recordset15_put_Collect_Proxy( 
    Recordset15 __RPC_FAR * This,
    /* [in] */ VARIANT Index,
    /* [in] */ VARIANT value);
void __RPC_STUB Recordset15_put_Collect_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);
/* [helpcontext][propget][id] */ HRESULT STDMETHODCALLTYPE Recordset15_get_MarshalOptions_Proxy( 
    Recordset15 __RPC_FAR * This,
    /* [retval][out] */ MarshalOptionsEnum __RPC_FAR *peMarshal);
void __RPC_STUB Recordset15_get_MarshalOptions_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);
/* [helpcontext][propput][id] */ HRESULT STDMETHODCALLTYPE Recordset15_put_MarshalOptions_Proxy( 
    Recordset15 __RPC_FAR * This,
    /* [in] */ MarshalOptionsEnum eMarshal);
void __RPC_STUB Recordset15_put_MarshalOptions_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);
/* [helpcontext][id] */ HRESULT STDMETHODCALLTYPE Recordset15_Find_Proxy( 
    Recordset15 __RPC_FAR * This,
    /* [in] */ BSTR Criteria,
    /* [defaultvalue][in] */ LONG SkipRecords,
    /* [defaultvalue][in] */ SearchDirectionEnum SearchDirection,
    /* [optional][in] */ VARIANT Start);
void __RPC_STUB Recordset15_Find_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);
#endif 	/* __Recordset15_INTERFACE_DEFINED__ */
#ifndef __Recordset20_INTERFACE_DEFINED__
#define __Recordset20_INTERFACE_DEFINED__
/* interface Recordset20 */
/* [object][helpcontext][uuid][nonextensible][hidden][dual] */ 
EXTERN_C const IID IID_Recordset20;
#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("0000054F-0000-0010-8000-00AA006D2EA4")
    Recordset20 : public Recordset15
    {
    public:
        virtual /* [helpcontext][id] */ HRESULT STDMETHODCALLTYPE Cancel( void) = 0;
        
        virtual /* [helpcontext][propget][id] */ HRESULT STDMETHODCALLTYPE get_DataSource( 
            /* [retval][out] */ IUnknown __RPC_FAR *__RPC_FAR *ppunkDataSource) = 0;
        
        virtual /* [helpcontext][propputref][id] */ HRESULT STDMETHODCALLTYPE putref_DataSource( 
            /* [in] */ IUnknown __RPC_FAR *punkDataSource) = 0;
        
        virtual /* [hidden] */ HRESULT STDMETHODCALLTYPE _xSave( 
            /* [optional][in] */ BSTR FileName,
            /* [defaultvalue][in] */ PersistFormatEnum PersistFormat = adPersistADTG) = 0;
        
        virtual /* [helpcontext][propget][id] */ HRESULT STDMETHODCALLTYPE get_ActiveCommand( 
            /* [retval][out] */ IDispatch __RPC_FAR *__RPC_FAR *ppCmd) = 0;
        
        virtual /* [helpcontext][propput][id] */ HRESULT STDMETHODCALLTYPE put_StayInSync( 
            /* [in] */ VARIANT_BOOL bStayInSync) = 0;
        
        virtual /* [helpcontext][propget][id] */ HRESULT STDMETHODCALLTYPE get_StayInSync( 
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pbStayInSync) = 0;
        
        virtual /* [helpcontext][id] */ HRESULT STDMETHODCALLTYPE GetString( 
            /* [defaultvalue][in] */ StringFormatEnum StringFormat,
            /* [defaultvalue][in] */ long NumRows,
            /* [optional][in] */ BSTR ColumnDelimeter,
            /* [optional][in] */ BSTR RowDelimeter,
            /* [optional][in] */ BSTR NullExpr,
            /* [retval][out] */ BSTR __RPC_FAR *pRetString) = 0;
        
        virtual /* [helpcontext][propget][id] */ HRESULT STDMETHODCALLTYPE get_DataMember( 
            /* [retval][out] */ BSTR __RPC_FAR *pbstrDataMember) = 0;
        
        virtual /* [helpcontext][propput][id] */ HRESULT STDMETHODCALLTYPE put_DataMember( 
            /* [in] */ BSTR bstrDataMember) = 0;
        
        virtual /* [helpcontext][id] */ HRESULT STDMETHODCALLTYPE CompareBookmarks( 
            /* [in] */ VARIANT Bookmark1,
            /* [in] */ VARIANT Bookmark2,
            /* [retval][out] */ CompareEnum __RPC_FAR *pCompare) = 0;
        
        virtual /* [helpcontext][id] */ HRESULT STDMETHODCALLTYPE Clone( 
            /* [defaultvalue][in] */ LockTypeEnum LockType,
            /* [retval][out] */ _ADORecordset __RPC_FAR *__RPC_FAR *ppvObject) = 0;
        
        virtual /* [helpcontext][id] */ HRESULT STDMETHODCALLTYPE Resync( 
            /* [defaultvalue][in] */ AffectEnum AffectRecords = adAffectAll,
            /* [defaultvalue][in] */ ResyncEnum ResyncValues = adResyncAllValues) = 0;
        
    };
    
#else 	/* C style interface */
    typedef struct Recordset20Vtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            Recordset20 __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            Recordset20 __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            Recordset20 __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            Recordset20 __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            Recordset20 __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            Recordset20 __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            Recordset20 __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        /* [helpcontext][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Properties )( 
            Recordset20 __RPC_FAR * This,
            /* [retval][out] */ ADOProperties __RPC_FAR *__RPC_FAR *ppvObject);
        
        /* [helpcontext][propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_AbsolutePosition )( 
            Recordset20 __RPC_FAR * This,
            /* [retval][out] */ PositionEnum __RPC_FAR *pl);
        
        /* [helpcontext][propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_AbsolutePosition )( 
            Recordset20 __RPC_FAR * This,
            /* [in] */ PositionEnum Position);
        
        /* [helpcontext][propputref][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *putref_ActiveADOConnection )( 
            Recordset20 __RPC_FAR * This,
            /* [in] */ IDispatch __RPC_FAR *pconn);
        
        /* [helpcontext][propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_ActiveConnection )( 
            Recordset20 __RPC_FAR * This,
            /* [in] */ VARIANT vConn);
        
        /* [helpcontext][propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_ActiveConnection )( 
            Recordset20 __RPC_FAR * This,
            /* [retval][out] */ VARIANT __RPC_FAR *pvar);
        
        /* [helpcontext][propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_BOF )( 
            Recordset20 __RPC_FAR * This,
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pb);
        
        /* [helpcontext][propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Bookmark )( 
            Recordset20 __RPC_FAR * This,
            /* [retval][out] */ VARIANT __RPC_FAR *pvBookmark);
        
        /* [helpcontext][propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Bookmark )( 
            Recordset20 __RPC_FAR * This,
            /* [in] */ VARIANT vBookmark);
        
        /* [helpcontext][propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_CacheSize )( 
            Recordset20 __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *pl);
        
        /* [helpcontext][propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_CacheSize )( 
            Recordset20 __RPC_FAR * This,
            /* [in] */ long CacheSize);
        
        /* [helpcontext][propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_CursorType )( 
            Recordset20 __RPC_FAR * This,
            /* [retval][out] */ CursorTypeEnum __RPC_FAR *plCursorType);
        
        /* [helpcontext][propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_CursorType )( 
            Recordset20 __RPC_FAR * This,
            /* [in] */ CursorTypeEnum lCursorType);
        
        /* [helpcontext][propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_EOF )( 
            Recordset20 __RPC_FAR * This,
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pb);
        
        /* [helpcontext][propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Fields )( 
            Recordset20 __RPC_FAR * This,
            /* [retval][out] */ ADOFields __RPC_FAR *__RPC_FAR *ppvObject);
        
        /* [helpcontext][propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_LockType )( 
            Recordset20 __RPC_FAR * This,
            /* [retval][out] */ LockTypeEnum __RPC_FAR *plLockType);
        
        /* [helpcontext][propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_LockType )( 
            Recordset20 __RPC_FAR * This,
            /* [in] */ LockTypeEnum lLockType);
        
        /* [helpcontext][propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_MaxRecords )( 
            Recordset20 __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *plMaxRecords);
        
        /* [helpcontext][propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_MaxRecords )( 
            Recordset20 __RPC_FAR * This,
            /* [in] */ long lMaxRecords);
        
        /* [helpcontext][propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_RecordCount )( 
            Recordset20 __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *pl);
        
        /* [helpcontext][propputref][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *putref_Source )( 
            Recordset20 __RPC_FAR * This,
            /* [in] */ IDispatch __RPC_FAR *pcmd);
        
        /* [helpcontext][propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Source )( 
            Recordset20 __RPC_FAR * This,
            /* [in] */ BSTR bstrConn);
        
        /* [helpcontext][propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Source )( 
            Recordset20 __RPC_FAR * This,
            /* [retval][out] */ VARIANT __RPC_FAR *pvSource);
        
        /* [helpcontext][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *AddNew )( 
            Recordset20 __RPC_FAR * This,
            /* [optional][in] */ VARIANT FieldList,
            /* [optional][in] */ VARIANT Values);
        
        /* [helpcontext][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *CancelUpdate )( 
            Recordset20 __RPC_FAR * This);
        
        /* [helpcontext][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Close )( 
            Recordset20 __RPC_FAR * This);
        
        /* [helpcontext][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Delete )( 
            Recordset20 __RPC_FAR * This,
            /* [defaultvalue][in] */ AffectEnum AffectRecords);
        
        /* [helpcontext][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetRows )( 
            Recordset20 __RPC_FAR * This,
            /* [defaultvalue][in] */ long Rows,
            /* [optional][in] */ VARIANT Start,
            /* [optional][in] */ VARIANT Fields,
            /* [retval][out] */ VARIANT __RPC_FAR *pvar);
        
        /* [helpcontext][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Move )( 
            Recordset20 __RPC_FAR * This,
            /* [in] */ long NumRecords,
            /* [optional][in] */ VARIANT Start);
        
        /* [helpcontext][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *MoveNext )( 
            Recordset20 __RPC_FAR * This);
        
        /* [helpcontext][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *MovePrevious )( 
            Recordset20 __RPC_FAR * This);
        
        /* [helpcontext][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *MoveFirst )( 
            Recordset20 __RPC_FAR * This);
        
        /* [helpcontext][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *MoveLast )( 
            Recordset20 __RPC_FAR * This);
        
        /* [helpcontext][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Open )( 
            Recordset20 __RPC_FAR * This,
            /* [optional][in] */ VARIANT Source,
            /* [optional][in] */ VARIANT ActiveConnection,
            /* [defaultvalue][in] */ CursorTypeEnum CursorType,
            /* [defaultvalue][in] */ LockTypeEnum LockType,
            /* [defaultvalue][in] */ LONG Options);
        
        /* [helpcontext][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Requery )( 
            Recordset20 __RPC_FAR * This,
            /* [defaultvalue][in] */ LONG Options);
        
        /* [hidden] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *_xResync )( 
            Recordset20 __RPC_FAR * This,
            /* [defaultvalue][in] */ AffectEnum AffectRecords);
        
        /* [helpcontext][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Update )( 
            Recordset20 __RPC_FAR * This,
            /* [optional][in] */ VARIANT Fields,
            /* [optional][in] */ VARIANT Values);
        
        /* [helpcontext][propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_AbsolutePage )( 
            Recordset20 __RPC_FAR * This,
            /* [retval][out] */ PositionEnum __RPC_FAR *pl);
        
        /* [helpcontext][propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_AbsolutePage )( 
            Recordset20 __RPC_FAR * This,
            /* [in] */ PositionEnum Page);
        
        /* [helpcontext][propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_EditMode )( 
            Recordset20 __RPC_FAR * This,
            /* [retval][out] */ EditModeEnum __RPC_FAR *pl);
        
        /* [helpcontext][propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Filter )( 
            Recordset20 __RPC_FAR * This,
            /* [retval][out] */ VARIANT __RPC_FAR *Criteria);
        
        /* [helpcontext][propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Filter )( 
            Recordset20 __RPC_FAR * This,
            /* [in] */ VARIANT Criteria);
        
        /* [helpcontext][propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_PageCount )( 
            Recordset20 __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *pl);
        
        /* [helpcontext][propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_PageSize )( 
            Recordset20 __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *pl);
        
        /* [helpcontext][propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_PageSize )( 
            Recordset20 __RPC_FAR * This,
            /* [in] */ long PageSize);
        
        /* [helpcontext][propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Sort )( 
            Recordset20 __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *Criteria);
        
        /* [helpcontext][propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Sort )( 
            Recordset20 __RPC_FAR * This,
            /* [in] */ BSTR Criteria);
        
        /* [helpcontext][propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Status )( 
            Recordset20 __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *pl);
        
        /* [helpcontext][propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_State )( 
            Recordset20 __RPC_FAR * This,
            /* [retval][out] */ LONG __RPC_FAR *plObjState);
        
        /* [hidden] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *_xClone )( 
            Recordset20 __RPC_FAR * This,
            /* [retval][out] */ _ADORecordset __RPC_FAR *__RPC_FAR *ppvObject);
        
        /* [helpcontext][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *UpdateBatch )( 
            Recordset20 __RPC_FAR * This,
            /* [defaultvalue][in] */ AffectEnum AffectRecords);
        
        /* [helpcontext][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *CancelBatch )( 
            Recordset20 __RPC_FAR * This,
            /* [defaultvalue][in] */ AffectEnum AffectRecords);
        
        /* [helpcontext][propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_CursorLocation )( 
            Recordset20 __RPC_FAR * This,
            /* [retval][out] */ CursorLocationEnum __RPC_FAR *plCursorLoc);
        
        /* [helpcontext][propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_CursorLocation )( 
            Recordset20 __RPC_FAR * This,
            /* [in] */ CursorLocationEnum lCursorLoc);
        
        /* [helpcontext][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *NextADORecordset )( 
            Recordset20 __RPC_FAR * This,
            /* [optional][out] */ VARIANT __RPC_FAR *RecordsAffected,
            /* [retval][out] */ _ADORecordset __RPC_FAR *__RPC_FAR *ppiRs);
        
        /* [helpcontext][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Supports )( 
            Recordset20 __RPC_FAR * This,
            /* [in] */ CursorOptionEnum CursorOptions,
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pb);
        
        /* [hidden][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Collect )( 
            Recordset20 __RPC_FAR * This,
            /* [in] */ VARIANT Index,
            /* [retval][out] */ VARIANT __RPC_FAR *pvar);
        
        /* [hidden][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Collect )( 
            Recordset20 __RPC_FAR * This,
            /* [in] */ VARIANT Index,
            /* [in] */ VARIANT value);
        
        /* [helpcontext][propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_MarshalOptions )( 
            Recordset20 __RPC_FAR * This,
            /* [retval][out] */ MarshalOptionsEnum __RPC_FAR *peMarshal);
        
        /* [helpcontext][propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_MarshalOptions )( 
            Recordset20 __RPC_FAR * This,
            /* [in] */ MarshalOptionsEnum eMarshal);
        
        /* [helpcontext][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Find )( 
            Recordset20 __RPC_FAR * This,
            /* [in] */ BSTR Criteria,
            /* [defaultvalue][in] */ LONG SkipRecords,
            /* [defaultvalue][in] */ SearchDirectionEnum SearchDirection,
            /* [optional][in] */ VARIANT Start);
        
        /* [helpcontext][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Cancel )( 
            Recordset20 __RPC_FAR * This);
        
        /* [helpcontext][propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_DataSource )( 
            Recordset20 __RPC_FAR * This,
            /* [retval][out] */ IUnknown __RPC_FAR *__RPC_FAR *ppunkDataSource);
        
        /* [helpcontext][propputref][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *putref_DataSource )( 
            Recordset20 __RPC_FAR * This,
            /* [in] */ IUnknown __RPC_FAR *punkDataSource);
        
        /* [hidden] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *_xSave )( 
            Recordset20 __RPC_FAR * This,
            /* [optional][in] */ BSTR FileName,
            /* [defaultvalue][in] */ PersistFormatEnum PersistFormat);
        
        /* [helpcontext][propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_ActiveCommand )( 
            Recordset20 __RPC_FAR * This,
            /* [retval][out] */ IDispatch __RPC_FAR *__RPC_FAR *ppCmd);
        
        /* [helpcontext][propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_StayInSync )( 
            Recordset20 __RPC_FAR * This,
            /* [in] */ VARIANT_BOOL bStayInSync);
        
        /* [helpcontext][propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_StayInSync )( 
            Recordset20 __RPC_FAR * This,
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pbStayInSync);
        
        /* [helpcontext][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetString )( 
            Recordset20 __RPC_FAR * This,
            /* [defaultvalue][in] */ StringFormatEnum StringFormat,
            /* [defaultvalue][in] */ long NumRows,
            /* [optional][in] */ BSTR ColumnDelimeter,
            /* [optional][in] */ BSTR RowDelimeter,
            /* [optional][in] */ BSTR NullExpr,
            /* [retval][out] */ BSTR __RPC_FAR *pRetString);
        
        /* [helpcontext][propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_DataMember )( 
            Recordset20 __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pbstrDataMember);
        
        /* [helpcontext][propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_DataMember )( 
            Recordset20 __RPC_FAR * This,
            /* [in] */ BSTR bstrDataMember);
        
        /* [helpcontext][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *CompareBookmarks )( 
            Recordset20 __RPC_FAR * This,
            /* [in] */ VARIANT Bookmark1,
            /* [in] */ VARIANT Bookmark2,
            /* [retval][out] */ CompareEnum __RPC_FAR *pCompare);
        
        /* [helpcontext][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Clone )( 
            Recordset20 __RPC_FAR * This,
            /* [defaultvalue][in] */ LockTypeEnum LockType,
            /* [retval][out] */ _ADORecordset __RPC_FAR *__RPC_FAR *ppvObject);
        
        /* [helpcontext][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Resync )( 
            Recordset20 __RPC_FAR * This,
            /* [defaultvalue][in] */ AffectEnum AffectRecords,
            /* [defaultvalue][in] */ ResyncEnum ResyncValues);
        
        END_INTERFACE
    } Recordset20Vtbl;
    interface Recordset20
    {
        CONST_VTBL struct Recordset20Vtbl __RPC_FAR *lpVtbl;
    };
    
#ifdef COBJMACROS
#define Recordset20_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)
#define Recordset20_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)
#define Recordset20_Release(This)	\
    (This)->lpVtbl -> Release(This)
#define Recordset20_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)
#define Recordset20_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)
#define Recordset20_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)
#define Recordset20_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)
#define Recordset20_get_Properties(This,ppvObject)	\
    (This)->lpVtbl -> get_Properties(This,ppvObject)
#define Recordset20_get_AbsolutePosition(This,pl)	\
    (This)->lpVtbl -> get_AbsolutePosition(This,pl)
#define Recordset20_put_AbsolutePosition(This,Position)	\
    (This)->lpVtbl -> put_AbsolutePosition(This,Position)
#define Recordset20_putref_ActiveConnection(This,pconn)	\
    (This)->lpVtbl -> putref_ActiveConnection(This,pconn)
#define Recordset20_put_ActiveConnection(This,vConn)	\
    (This)->lpVtbl -> put_ActiveConnection(This,vConn)
#define Recordset20_get_ActiveConnection(This,pvar)	\
    (This)->lpVtbl -> get_ActiveConnection(This,pvar)
#define Recordset20_get_BOF(This,pb)	\
    (This)->lpVtbl -> get_BOF(This,pb)
#define Recordset20_get_Bookmark(This,pvBookmark)	\
    (This)->lpVtbl -> get_Bookmark(This,pvBookmark)
#define Recordset20_put_Bookmark(This,vBookmark)	\
    (This)->lpVtbl -> put_Bookmark(This,vBookmark)
#define Recordset20_get_CacheSize(This,pl)	\
    (This)->lpVtbl -> get_CacheSize(This,pl)
#define Recordset20_put_CacheSize(This,CacheSize)	\
    (This)->lpVtbl -> put_CacheSize(This,CacheSize)
#define Recordset20_get_CursorType(This,plCursorType)	\
    (This)->lpVtbl -> get_CursorType(This,plCursorType)
#define Recordset20_put_CursorType(This,lCursorType)	\
    (This)->lpVtbl -> put_CursorType(This,lCursorType)
#define Recordset20_get_EOF(This,pb)	\
    (This)->lpVtbl -> get_EOF(This,pb)
#define Recordset20_get_Fields(This,ppvObject)	\
    (This)->lpVtbl -> get_Fields(This,ppvObject)
#define Recordset20_get_LockType(This,plLockType)	\
    (This)->lpVtbl -> get_LockType(This,plLockType)
#define Recordset20_put_LockType(This,lLockType)	\
    (This)->lpVtbl -> put_LockType(This,lLockType)
#define Recordset20_get_MaxRecords(This,plMaxRecords)	\
    (This)->lpVtbl -> get_MaxRecords(This,plMaxRecords)
#define Recordset20_put_MaxRecords(This,lMaxRecords)	\
    (This)->lpVtbl -> put_MaxRecords(This,lMaxRecords)
#define Recordset20_get_RecordCount(This,pl)	\
    (This)->lpVtbl -> get_RecordCount(This,pl)
#define Recordset20_putref_Source(This,pcmd)	\
    (This)->lpVtbl -> putref_Source(This,pcmd)
#define Recordset20_put_Source(This,bstrConn)	\
    (This)->lpVtbl -> put_Source(This,bstrConn)
#define Recordset20_get_Source(This,pvSource)	\
    (This)->lpVtbl -> get_Source(This,pvSource)
#define Recordset20_AddNew(This,FieldList,Values)	\
    (This)->lpVtbl -> AddNew(This,FieldList,Values)
#define Recordset20_CancelUpdate(This)	\
    (This)->lpVtbl -> CancelUpdate(This)
#define Recordset20_Close(This)	\
    (This)->lpVtbl -> Close(This)
#define Recordset20_Delete(This,AffectRecords)	\
    (This)->lpVtbl -> Delete(This,AffectRecords)
#define Recordset20_GetRows(This,Rows,Start,Fields,pvar)	\
    (This)->lpVtbl -> GetRows(This,Rows,Start,Fields,pvar)
#define Recordset20_Move(This,NumRecords,Start)	\
    (This)->lpVtbl -> Move(This,NumRecords,Start)
#define Recordset20_MoveNext(This)	\
    (This)->lpVtbl -> MoveNext(This)
#define Recordset20_MovePrevious(This)	\
    (This)->lpVtbl -> MovePrevious(This)
#define Recordset20_MoveFirst(This)	\
    (This)->lpVtbl -> MoveFirst(This)
#define Recordset20_MoveLast(This)	\
    (This)->lpVtbl -> MoveLast(This)
#define Recordset20_Open(This,Source,ActiveConnection,CursorType,LockType,Options)	\
    (This)->lpVtbl -> Open(This,Source,ActiveConnection,CursorType,LockType,Options)
#define Recordset20_Requery(This,Options)	\
    (This)->lpVtbl -> Requery(This,Options)
#define Recordset20__xResync(This,AffectRecords)	\
    (This)->lpVtbl -> _xResync(This,AffectRecords)
#define Recordset20_Update(This,Fields,Values)	\
    (This)->lpVtbl -> Update(This,Fields,Values)
#define Recordset20_get_AbsolutePage(This,pl)	\
    (This)->lpVtbl -> get_AbsolutePage(This,pl)
#define Recordset20_put_AbsolutePage(This,Page)	\
    (This)->lpVtbl -> put_AbsolutePage(This,Page)
#define Recordset20_get_EditMode(This,pl)	\
    (This)->lpVtbl -> get_EditMode(This,pl)
#define Recordset20_get_Filter(This,Criteria)	\
    (This)->lpVtbl -> get_Filter(This,Criteria)
#define Recordset20_put_Filter(This,Criteria)	\
    (This)->lpVtbl -> put_Filter(This,Criteria)
#define Recordset20_get_PageCount(This,pl)	\
    (This)->lpVtbl -> get_PageCount(This,pl)
#define Recordset20_get_PageSize(This,pl)	\
    (This)->lpVtbl -> get_PageSize(This,pl)
#define Recordset20_put_PageSize(This,PageSize)	\
    (This)->lpVtbl -> put_PageSize(This,PageSize)
#define Recordset20_get_Sort(This,Criteria)	\
    (This)->lpVtbl -> get_Sort(This,Criteria)
#define Recordset20_put_Sort(This,Criteria)	\
    (This)->lpVtbl -> put_Sort(This,Criteria)
#define Recordset20_get_Status(This,pl)	\
    (This)->lpVtbl -> get_Status(This,pl)
#define Recordset20_get_State(This,plObjState)	\
    (This)->lpVtbl -> get_State(This,plObjState)
#define Recordset20__xClone(This,ppvObject)	\
    (This)->lpVtbl -> _xClone(This,ppvObject)
#define Recordset20_UpdateBatch(This,AffectRecords)	\
    (This)->lpVtbl -> UpdateBatch(This,AffectRecords)
#define Recordset20_CancelBatch(This,AffectRecords)	\
    (This)->lpVtbl -> CancelBatch(This,AffectRecords)
#define Recordset20_get_CursorLocation(This,plCursorLoc)	\
    (This)->lpVtbl -> get_CursorLocation(This,plCursorLoc)
#define Recordset20_put_CursorLocation(This,lCursorLoc)	\
    (This)->lpVtbl -> put_CursorLocation(This,lCursorLoc)
#define Recordset20_NextRecordset(This,RecordsAffected,ppiRs)	\
    (This)->lpVtbl -> NextRecordset(This,RecordsAffected,ppiRs)
#define Recordset20_Supports(This,CursorOptions,pb)	\
    (This)->lpVtbl -> Supports(This,CursorOptions,pb)
#define Recordset20_get_Collect(This,Index,pvar)	\
    (This)->lpVtbl -> get_Collect(This,Index,pvar)
#define Recordset20_put_Collect(This,Index,value)	\
    (This)->lpVtbl -> put_Collect(This,Index,value)
#define Recordset20_get_MarshalOptions(This,peMarshal)	\
    (This)->lpVtbl -> get_MarshalOptions(This,peMarshal)
#define Recordset20_put_MarshalOptions(This,eMarshal)	\
    (This)->lpVtbl -> put_MarshalOptions(This,eMarshal)
#define Recordset20_Find(This,Criteria,SkipRecords,SearchDirection,Start)	\
    (This)->lpVtbl -> Find(This,Criteria,SkipRecords,SearchDirection,Start)
#define Recordset20_Cancel(This)	\
    (This)->lpVtbl -> Cancel(This)
#define Recordset20_get_DataSource(This,ppunkDataSource)	\
    (This)->lpVtbl -> get_DataSource(This,ppunkDataSource)
#define Recordset20_putref_DataSource(This,punkDataSource)	\
    (This)->lpVtbl -> putref_DataSource(This,punkDataSource)
#define Recordset20__xSave(This,FileName,PersistFormat)	\
    (This)->lpVtbl -> _xSave(This,FileName,PersistFormat)
#define Recordset20_get_ActiveCommand(This,ppCmd)	\
    (This)->lpVtbl -> get_ActiveCommand(This,ppCmd)
#define Recordset20_put_StayInSync(This,bStayInSync)	\
    (This)->lpVtbl -> put_StayInSync(This,bStayInSync)
#define Recordset20_get_StayInSync(This,pbStayInSync)	\
    (This)->lpVtbl -> get_StayInSync(This,pbStayInSync)
#define Recordset20_GetString(This,StringFormat,NumRows,ColumnDelimeter,RowDelimeter,NullExpr,pRetString)	\
    (This)->lpVtbl -> GetString(This,StringFormat,NumRows,ColumnDelimeter,RowDelimeter,NullExpr,pRetString)
#define Recordset20_get_DataMember(This,pbstrDataMember)	\
    (This)->lpVtbl -> get_DataMember(This,pbstrDataMember)
#define Recordset20_put_DataMember(This,bstrDataMember)	\
    (This)->lpVtbl -> put_DataMember(This,bstrDataMember)
#define Recordset20_CompareBookmarks(This,Bookmark1,Bookmark2,pCompare)	\
    (This)->lpVtbl -> CompareBookmarks(This,Bookmark1,Bookmark2,pCompare)
#define Recordset20_Clone(This,LockType,ppvObject)	\
    (This)->lpVtbl -> Clone(This,LockType,ppvObject)
#define Recordset20_Resync(This,AffectRecords,ResyncValues)	\
    (This)->lpVtbl -> Resync(This,AffectRecords,ResyncValues)
#endif /* COBJMACROS */
#endif 	/* C style interface */
/* [helpcontext][id] */ HRESULT STDMETHODCALLTYPE Recordset20_Cancel_Proxy( 
    Recordset20 __RPC_FAR * This);
void __RPC_STUB Recordset20_Cancel_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);
/* [helpcontext][propget][id] */ HRESULT STDMETHODCALLTYPE Recordset20_get_DataSource_Proxy( 
    Recordset20 __RPC_FAR * This,
    /* [retval][out] */ IUnknown __RPC_FAR *__RPC_FAR *ppunkDataSource);
void __RPC_STUB Recordset20_get_DataSource_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);
/* [helpcontext][propputref][id] */ HRESULT STDMETHODCALLTYPE Recordset20_putref_DataSource_Proxy( 
    Recordset20 __RPC_FAR * This,
    /* [in] */ IUnknown __RPC_FAR *punkDataSource);
void __RPC_STUB Recordset20_putref_DataSource_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);
/* [hidden] */ HRESULT STDMETHODCALLTYPE Recordset20__xSave_Proxy( 
    Recordset20 __RPC_FAR * This,
    /* [optional][in] */ BSTR FileName,
    /* [defaultvalue][in] */ PersistFormatEnum PersistFormat);
void __RPC_STUB Recordset20__xSave_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);
/* [helpcontext][propget][id] */ HRESULT STDMETHODCALLTYPE Recordset20_get_ActiveCommand_Proxy( 
    Recordset20 __RPC_FAR * This,
    /* [retval][out] */ IDispatch __RPC_FAR *__RPC_FAR *ppCmd);
void __RPC_STUB Recordset20_get_ActiveCommand_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);
/* [helpcontext][propput][id] */ HRESULT STDMETHODCALLTYPE Recordset20_put_StayInSync_Proxy( 
    Recordset20 __RPC_FAR * This,
    /* [in] */ VARIANT_BOOL bStayInSync);
void __RPC_STUB Recordset20_put_StayInSync_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);
/* [helpcontext][propget][id] */ HRESULT STDMETHODCALLTYPE Recordset20_get_StayInSync_Proxy( 
    Recordset20 __RPC_FAR * This,
    /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pbStayInSync);
void __RPC_STUB Recordset20_get_StayInSync_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);
/* [helpcontext][id] */ HRESULT STDMETHODCALLTYPE Recordset20_GetString_Proxy( 
    Recordset20 __RPC_FAR * This,
    /* [defaultvalue][in] */ StringFormatEnum StringFormat,
    /* [defaultvalue][in] */ long NumRows,
    /* [optional][in] */ BSTR ColumnDelimeter,
    /* [optional][in] */ BSTR RowDelimeter,
    /* [optional][in] */ BSTR NullExpr,
    /* [retval][out] */ BSTR __RPC_FAR *pRetString);
void __RPC_STUB Recordset20_GetString_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);
/* [helpcontext][propget][id] */ HRESULT STDMETHODCALLTYPE Recordset20_get_DataMember_Proxy( 
    Recordset20 __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pbstrDataMember);
void __RPC_STUB Recordset20_get_DataMember_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);
/* [helpcontext][propput][id] */ HRESULT STDMETHODCALLTYPE Recordset20_put_DataMember_Proxy( 
    Recordset20 __RPC_FAR * This,
    /* [in] */ BSTR bstrDataMember);
void __RPC_STUB Recordset20_put_DataMember_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);
/* [helpcontext][id] */ HRESULT STDMETHODCALLTYPE Recordset20_CompareBookmarks_Proxy( 
    Recordset20 __RPC_FAR * This,
    /* [in] */ VARIANT Bookmark1,
    /* [in] */ VARIANT Bookmark2,
    /* [retval][out] */ CompareEnum __RPC_FAR *pCompare);
void __RPC_STUB Recordset20_CompareBookmarks_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);
/* [helpcontext][id] */ HRESULT STDMETHODCALLTYPE Recordset20_Clone_Proxy( 
    Recordset20 __RPC_FAR * This,
    /* [defaultvalue][in] */ LockTypeEnum LockType,
    /* [retval][out] */ _ADORecordset __RPC_FAR *__RPC_FAR *ppvObject);
void __RPC_STUB Recordset20_Clone_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);
/* [helpcontext][id] */ HRESULT STDMETHODCALLTYPE Recordset20_Resync_Proxy( 
    Recordset20 __RPC_FAR * This,
    /* [defaultvalue][in] */ AffectEnum AffectRecords,
    /* [defaultvalue][in] */ ResyncEnum ResyncValues);
void __RPC_STUB Recordset20_Resync_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);
#endif 	/* __Recordset20_INTERFACE_DEFINED__ */
#ifndef __Recordset21_INTERFACE_DEFINED__
#define __Recordset21_INTERFACE_DEFINED__
/* interface Recordset21 */
/* [object][helpcontext][uuid][nonextensible][hidden][dual] */ 
EXTERN_C const IID IID_Recordset21;
#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("00000555-0000-0010-8000-00AA006D2EA4")
    Recordset21 : public Recordset20
    {
    public:
        virtual /* [helpcontext][id] */ HRESULT STDMETHODCALLTYPE Seek( 
            /* [in] */ VARIANT KeyValues,
            /* [defaultvalue][in] */ SeekEnum SeekOption = adSeekFirstEQ) = 0;
        
        virtual /* [helpcontext][propput][id] */ HRESULT STDMETHODCALLTYPE put_Index( 
            /* [in] */ BSTR Index) = 0;
        
        virtual /* [helpcontext][propget][id] */ HRESULT STDMETHODCALLTYPE get_Index( 
            /* [retval][out] */ BSTR __RPC_FAR *pbstrIndex) = 0;
        
    };
    
#else 	/* C style interface */
    typedef struct Recordset21Vtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            Recordset21 __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            Recordset21 __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            Recordset21 __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            Recordset21 __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            Recordset21 __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            Recordset21 __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            Recordset21 __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        /* [helpcontext][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Properties )( 
            Recordset21 __RPC_FAR * This,
            /* [retval][out] */ ADOProperties __RPC_FAR *__RPC_FAR *ppvObject);
        
        /* [helpcontext][propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_AbsolutePosition )( 
            Recordset21 __RPC_FAR * This,
            /* [retval][out] */ PositionEnum __RPC_FAR *pl);
        
        /* [helpcontext][propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_AbsolutePosition )( 
            Recordset21 __RPC_FAR * This,
            /* [in] */ PositionEnum Position);
        
        /* [helpcontext][propputref][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *putref_ActiveADOConnection )( 
            Recordset21 __RPC_FAR * This,
            /* [in] */ IDispatch __RPC_FAR *pconn);
        
        /* [helpcontext][propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_ActiveConnection )( 
            Recordset21 __RPC_FAR * This,
            /* [in] */ VARIANT vConn);
        
        /* [helpcontext][propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_ActiveConnection )( 
            Recordset21 __RPC_FAR * This,
            /* [retval][out] */ VARIANT __RPC_FAR *pvar);
        
        /* [helpcontext][propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_BOF )( 
            Recordset21 __RPC_FAR * This,
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pb);
        
        /* [helpcontext][propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Bookmark )( 
            Recordset21 __RPC_FAR * This,
            /* [retval][out] */ VARIANT __RPC_FAR *pvBookmark);
        
        /* [helpcontext][propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Bookmark )( 
            Recordset21 __RPC_FAR * This,
            /* [in] */ VARIANT vBookmark);
        
        /* [helpcontext][propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_CacheSize )( 
            Recordset21 __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *pl);
        
        /* [helpcontext][propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_CacheSize )( 
            Recordset21 __RPC_FAR * This,
            /* [in] */ long CacheSize);
        
        /* [helpcontext][propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_CursorType )( 
            Recordset21 __RPC_FAR * This,
            /* [retval][out] */ CursorTypeEnum __RPC_FAR *plCursorType);
        
        /* [helpcontext][propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_CursorType )( 
            Recordset21 __RPC_FAR * This,
            /* [in] */ CursorTypeEnum lCursorType);
        
        /* [helpcontext][propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_EOF )( 
            Recordset21 __RPC_FAR * This,
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pb);
        
        /* [helpcontext][propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Fields )( 
            Recordset21 __RPC_FAR * This,
            /* [retval][out] */ ADOFields __RPC_FAR *__RPC_FAR *ppvObject);
        
        /* [helpcontext][propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_LockType )( 
            Recordset21 __RPC_FAR * This,
            /* [retval][out] */ LockTypeEnum __RPC_FAR *plLockType);
        
        /* [helpcontext][propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_LockType )( 
            Recordset21 __RPC_FAR * This,
            /* [in] */ LockTypeEnum lLockType);
        
        /* [helpcontext][propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_MaxRecords )( 
            Recordset21 __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *plMaxRecords);
        
        /* [helpcontext][propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_MaxRecords )( 
            Recordset21 __RPC_FAR * This,
            /* [in] */ long lMaxRecords);
        
        /* [helpcontext][propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_RecordCount )( 
            Recordset21 __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *pl);
        
        /* [helpcontext][propputref][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *putref_Source )( 
            Recordset21 __RPC_FAR * This,
            /* [in] */ IDispatch __RPC_FAR *pcmd);
        
        /* [helpcontext][propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Source )( 
            Recordset21 __RPC_FAR * This,
            /* [in] */ BSTR bstrConn);
        
        /* [helpcontext][propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Source )( 
            Recordset21 __RPC_FAR * This,
            /* [retval][out] */ VARIANT __RPC_FAR *pvSource);
        
        /* [helpcontext][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *AddNew )( 
            Recordset21 __RPC_FAR * This,
            /* [optional][in] */ VARIANT FieldList,
            /* [optional][in] */ VARIANT Values);
        
        /* [helpcontext][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *CancelUpdate )( 
            Recordset21 __RPC_FAR * This);
        
        /* [helpcontext][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Close )( 
            Recordset21 __RPC_FAR * This);
        
        /* [helpcontext][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Delete )( 
            Recordset21 __RPC_FAR * This,
            /* [defaultvalue][in] */ AffectEnum AffectRecords);
        
        /* [helpcontext][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetRows )( 
            Recordset21 __RPC_FAR * This,
            /* [defaultvalue][in] */ long Rows,
            /* [optional][in] */ VARIANT Start,
            /* [optional][in] */ VARIANT Fields,
            /* [retval][out] */ VARIANT __RPC_FAR *pvar);
        
        /* [helpcontext][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Move )( 
            Recordset21 __RPC_FAR * This,
            /* [in] */ long NumRecords,
            /* [optional][in] */ VARIANT Start);
        
        /* [helpcontext][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *MoveNext )( 
            Recordset21 __RPC_FAR * This);
        
        /* [helpcontext][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *MovePrevious )( 
            Recordset21 __RPC_FAR * This);
        
        /* [helpcontext][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *MoveFirst )( 
            Recordset21 __RPC_FAR * This);
        
        /* [helpcontext][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *MoveLast )( 
            Recordset21 __RPC_FAR * This);
        
        /* [helpcontext][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Open )( 
            Recordset21 __RPC_FAR * This,
            /* [optional][in] */ VARIANT Source,
            /* [optional][in] */ VARIANT ActiveConnection,
            /* [defaultvalue][in] */ CursorTypeEnum CursorType,
            /* [defaultvalue][in] */ LockTypeEnum LockType,
            /* [defaultvalue][in] */ LONG Options);
        
        /* [helpcontext][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Requery )( 
            Recordset21 __RPC_FAR * This,
            /* [defaultvalue][in] */ LONG Options);
        
        /* [hidden] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *_xResync )( 
            Recordset21 __RPC_FAR * This,
            /* [defaultvalue][in] */ AffectEnum AffectRecords);
        
        /* [helpcontext][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Update )( 
            Recordset21 __RPC_FAR * This,
            /* [optional][in] */ VARIANT Fields,
            /* [optional][in] */ VARIANT Values);
        
        /* [helpcontext][propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_AbsolutePage )( 
            Recordset21 __RPC_FAR * This,
            /* [retval][out] */ PositionEnum __RPC_FAR *pl);
        
        /* [helpcontext][propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_AbsolutePage )( 
            Recordset21 __RPC_FAR * This,
            /* [in] */ PositionEnum Page);
        
        /* [helpcontext][propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_EditMode )( 
            Recordset21 __RPC_FAR * This,
            /* [retval][out] */ EditModeEnum __RPC_FAR *pl);
        
        /* [helpcontext][propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Filter )( 
            Recordset21 __RPC_FAR * This,
            /* [retval][out] */ VARIANT __RPC_FAR *Criteria);
        
        /* [helpcontext][propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Filter )( 
            Recordset21 __RPC_FAR * This,
            /* [in] */ VARIANT Criteria);
        
        /* [helpcontext][propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_PageCount )( 
            Recordset21 __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *pl);
        
        /* [helpcontext][propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_PageSize )( 
            Recordset21 __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *pl);
        
        /* [helpcontext][propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_PageSize )( 
            Recordset21 __RPC_FAR * This,
            /* [in] */ long PageSize);
        
        /* [helpcontext][propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Sort )( 
            Recordset21 __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *Criteria);
        
        /* [helpcontext][propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Sort )( 
            Recordset21 __RPC_FAR * This,
            /* [in] */ BSTR Criteria);
        
        /* [helpcontext][propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Status )( 
            Recordset21 __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *pl);
        
        /* [helpcontext][propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_State )( 
            Recordset21 __RPC_FAR * This,
            /* [retval][out] */ LONG __RPC_FAR *plObjState);
        
        /* [hidden] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *_xClone )( 
            Recordset21 __RPC_FAR * This,
            /* [retval][out] */ _ADORecordset __RPC_FAR *__RPC_FAR *ppvObject);
        
        /* [helpcontext][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *UpdateBatch )( 
            Recordset21 __RPC_FAR * This,
            /* [defaultvalue][in] */ AffectEnum AffectRecords);
        
        /* [helpcontext][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *CancelBatch )( 
            Recordset21 __RPC_FAR * This,
            /* [defaultvalue][in] */ AffectEnum AffectRecords);
        
        /* [helpcontext][propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_CursorLocation )( 
            Recordset21 __RPC_FAR * This,
            /* [retval][out] */ CursorLocationEnum __RPC_FAR *plCursorLoc);
        
        /* [helpcontext][propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_CursorLocation )( 
            Recordset21 __RPC_FAR * This,
            /* [in] */ CursorLocationEnum lCursorLoc);
        
        /* [helpcontext][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *NextADORecordset )( 
            Recordset21 __RPC_FAR * This,
            /* [optional][out] */ VARIANT __RPC_FAR *RecordsAffected,
            /* [retval][out] */ _ADORecordset __RPC_FAR *__RPC_FAR *ppiRs);
        
        /* [helpcontext][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Supports )( 
            Recordset21 __RPC_FAR * This,
            /* [in] */ CursorOptionEnum CursorOptions,
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pb);
        
        /* [hidden][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Collect )( 
            Recordset21 __RPC_FAR * This,
            /* [in] */ VARIANT Index,
            /* [retval][out] */ VARIANT __RPC_FAR *pvar);
        
        /* [hidden][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Collect )( 
            Recordset21 __RPC_FAR * This,
            /* [in] */ VARIANT Index,
            /* [in] */ VARIANT value);
        
        /* [helpcontext][propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_MarshalOptions )( 
            Recordset21 __RPC_FAR * This,
            /* [retval][out] */ MarshalOptionsEnum __RPC_FAR *peMarshal);
        
        /* [helpcontext][propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_MarshalOptions )( 
            Recordset21 __RPC_FAR * This,
            /* [in] */ MarshalOptionsEnum eMarshal);
        
        /* [helpcontext][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Find )( 
            Recordset21 __RPC_FAR * This,
            /* [in] */ BSTR Criteria,
            /* [defaultvalue][in] */ LONG SkipRecords,
            /* [defaultvalue][in] */ SearchDirectionEnum SearchDirection,
            /* [optional][in] */ VARIANT Start);
        
        /* [helpcontext][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Cancel )( 
            Recordset21 __RPC_FAR * This);
        
        /* [helpcontext][propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_DataSource )( 
            Recordset21 __RPC_FAR * This,
            /* [retval][out] */ IUnknown __RPC_FAR *__RPC_FAR *ppunkDataSource);
        
        /* [helpcontext][propputref][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *putref_DataSource )( 
            Recordset21 __RPC_FAR * This,
            /* [in] */ IUnknown __RPC_FAR *punkDataSource);
        
        /* [hidden] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *_xSave )( 
            Recordset21 __RPC_FAR * This,
            /* [optional][in] */ BSTR FileName,
            /* [defaultvalue][in] */ PersistFormatEnum PersistFormat);
        
        /* [helpcontext][propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_ActiveCommand )( 
            Recordset21 __RPC_FAR * This,
            /* [retval][out] */ IDispatch __RPC_FAR *__RPC_FAR *ppCmd);
        
        /* [helpcontext][propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_StayInSync )( 
            Recordset21 __RPC_FAR * This,
            /* [in] */ VARIANT_BOOL bStayInSync);
        
        /* [helpcontext][propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_StayInSync )( 
            Recordset21 __RPC_FAR * This,
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pbStayInSync);
        
        /* [helpcontext][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetString )( 
            Recordset21 __RPC_FAR * This,
            /* [defaultvalue][in] */ StringFormatEnum StringFormat,
            /* [defaultvalue][in] */ long NumRows,
            /* [optional][in] */ BSTR ColumnDelimeter,
            /* [optional][in] */ BSTR RowDelimeter,
            /* [optional][in] */ BSTR NullExpr,
            /* [retval][out] */ BSTR __RPC_FAR *pRetString);
        
        /* [helpcontext][propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_DataMember )( 
            Recordset21 __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pbstrDataMember);
        
        /* [helpcontext][propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_DataMember )( 
            Recordset21 __RPC_FAR * This,
            /* [in] */ BSTR bstrDataMember);
        
        /* [helpcontext][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *CompareBookmarks )( 
            Recordset21 __RPC_FAR * This,
            /* [in] */ VARIANT Bookmark1,
            /* [in] */ VARIANT Bookmark2,
            /* [retval][out] */ CompareEnum __RPC_FAR *pCompare);
        
        /* [helpcontext][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Clone )( 
            Recordset21 __RPC_FAR * This,
            /* [defaultvalue][in] */ LockTypeEnum LockType,
            /* [retval][out] */ _ADORecordset __RPC_FAR *__RPC_FAR *ppvObject);
        
        /* [helpcontext][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Resync )( 
            Recordset21 __RPC_FAR * This,
            /* [defaultvalue][in] */ AffectEnum AffectRecords,
            /* [defaultvalue][in] */ ResyncEnum ResyncValues);
        
        /* [helpcontext][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Seek )( 
            Recordset21 __RPC_FAR * This,
            /* [in] */ VARIANT KeyValues,
            /* [defaultvalue][in] */ SeekEnum SeekOption);
        
        /* [helpcontext][propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Index )( 
            Recordset21 __RPC_FAR * This,
            /* [in] */ BSTR Index);
        
        /* [helpcontext][propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Index )( 
            Recordset21 __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pbstrIndex);
        
        END_INTERFACE
    } Recordset21Vtbl;
    interface Recordset21
    {
        CONST_VTBL struct Recordset21Vtbl __RPC_FAR *lpVtbl;
    };
    
#ifdef COBJMACROS
#define Recordset21_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)
#define Recordset21_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)
#define Recordset21_Release(This)	\
    (This)->lpVtbl -> Release(This)
#define Recordset21_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)
#define Recordset21_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)
#define Recordset21_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)
#define Recordset21_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)
#define Recordset21_get_Properties(This,ppvObject)	\
    (This)->lpVtbl -> get_Properties(This,ppvObject)
#define Recordset21_get_AbsolutePosition(This,pl)	\
    (This)->lpVtbl -> get_AbsolutePosition(This,pl)
#define Recordset21_put_AbsolutePosition(This,Position)	\
    (This)->lpVtbl -> put_AbsolutePosition(This,Position)
#define Recordset21_putref_ActiveConnection(This,pconn)	\
    (This)->lpVtbl -> putref_ActiveConnection(This,pconn)
#define Recordset21_put_ActiveConnection(This,vConn)	\
    (This)->lpVtbl -> put_ActiveConnection(This,vConn)
#define Recordset21_get_ActiveConnection(This,pvar)	\
    (This)->lpVtbl -> get_ActiveConnection(This,pvar)
#define Recordset21_get_BOF(This,pb)	\
    (This)->lpVtbl -> get_BOF(This,pb)
#define Recordset21_get_Bookmark(This,pvBookmark)	\
    (This)->lpVtbl -> get_Bookmark(This,pvBookmark)
#define Recordset21_put_Bookmark(This,vBookmark)	\
    (This)->lpVtbl -> put_Bookmark(This,vBookmark)
#define Recordset21_get_CacheSize(This,pl)	\
    (This)->lpVtbl -> get_CacheSize(This,pl)
#define Recordset21_put_CacheSize(This,CacheSize)	\
    (This)->lpVtbl -> put_CacheSize(This,CacheSize)
#define Recordset21_get_CursorType(This,plCursorType)	\
    (This)->lpVtbl -> get_CursorType(This,plCursorType)
#define Recordset21_put_CursorType(This,lCursorType)	\
    (This)->lpVtbl -> put_CursorType(This,lCursorType)
#define Recordset21_get_EOF(This,pb)	\
    (This)->lpVtbl -> get_EOF(This,pb)
#define Recordset21_get_Fields(This,ppvObject)	\
    (This)->lpVtbl -> get_Fields(This,ppvObject)
#define Recordset21_get_LockType(This,plLockType)	\
    (This)->lpVtbl -> get_LockType(This,plLockType)
#define Recordset21_put_LockType(This,lLockType)	\
    (This)->lpVtbl -> put_LockType(This,lLockType)
#define Recordset21_get_MaxRecords(This,plMaxRecords)	\
    (This)->lpVtbl -> get_MaxRecords(This,plMaxRecords)
#define Recordset21_put_MaxRecords(This,lMaxRecords)	\
    (This)->lpVtbl -> put_MaxRecords(This,lMaxRecords)
#define Recordset21_get_RecordCount(This,pl)	\
    (This)->lpVtbl -> get_RecordCount(This,pl)
#define Recordset21_putref_Source(This,pcmd)	\
    (This)->lpVtbl -> putref_Source(This,pcmd)
#define Recordset21_put_Source(This,bstrConn)	\
    (This)->lpVtbl -> put_Source(This,bstrConn)
#define Recordset21_get_Source(This,pvSource)	\
    (This)->lpVtbl -> get_Source(This,pvSource)
#define Recordset21_AddNew(This,FieldList,Values)	\
    (This)->lpVtbl -> AddNew(This,FieldList,Values)
#define Recordset21_CancelUpdate(This)	\
    (This)->lpVtbl -> CancelUpdate(This)
#define Recordset21_Close(This)	\
    (This)->lpVtbl -> Close(This)
#define Recordset21_Delete(This,AffectRecords)	\
    (This)->lpVtbl -> Delete(This,AffectRecords)
#define Recordset21_GetRows(This,Rows,Start,Fields,pvar)	\
    (This)->lpVtbl -> GetRows(This,Rows,Start,Fields,pvar)
#define Recordset21_Move(This,NumRecords,Start)	\
    (This)->lpVtbl -> Move(This,NumRecords,Start)
#define Recordset21_MoveNext(This)	\
    (This)->lpVtbl -> MoveNext(This)
#define Recordset21_MovePrevious(This)	\
    (This)->lpVtbl -> MovePrevious(This)
#define Recordset21_MoveFirst(This)	\
    (This)->lpVtbl -> MoveFirst(This)
#define Recordset21_MoveLast(This)	\
    (This)->lpVtbl -> MoveLast(This)
#define Recordset21_Open(This,Source,ActiveConnection,CursorType,LockType,Options)	\
    (This)->lpVtbl -> Open(This,Source,ActiveConnection,CursorType,LockType,Options)
#define Recordset21_Requery(This,Options)	\
    (This)->lpVtbl -> Requery(This,Options)
#define Recordset21__xResync(This,AffectRecords)	\
    (This)->lpVtbl -> _xResync(This,AffectRecords)
#define Recordset21_Update(This,Fields,Values)	\
    (This)->lpVtbl -> Update(This,Fields,Values)
#define Recordset21_get_AbsolutePage(This,pl)	\
    (This)->lpVtbl -> get_AbsolutePage(This,pl)
#define Recordset21_put_AbsolutePage(This,Page)	\
    (This)->lpVtbl -> put_AbsolutePage(This,Page)
#define Recordset21_get_EditMode(This,pl)	\
    (This)->lpVtbl -> get_EditMode(This,pl)
#define Recordset21_get_Filter(This,Criteria)	\
    (This)->lpVtbl -> get_Filter(This,Criteria)
#define Recordset21_put_Filter(This,Criteria)	\
    (This)->lpVtbl -> put_Filter(This,Criteria)
#define Recordset21_get_PageCount(This,pl)	\
    (This)->lpVtbl -> get_PageCount(This,pl)
#define Recordset21_get_PageSize(This,pl)	\
    (This)->lpVtbl -> get_PageSize(This,pl)
#define Recordset21_put_PageSize(This,PageSize)	\
    (This)->lpVtbl -> put_PageSize(This,PageSize)
#define Recordset21_get_Sort(This,Criteria)	\
    (This)->lpVtbl -> get_Sort(This,Criteria)
#define Recordset21_put_Sort(This,Criteria)	\
    (This)->lpVtbl -> put_Sort(This,Criteria)
#define Recordset21_get_Status(This,pl)	\
    (This)->lpVtbl -> get_Status(This,pl)
#define Recordset21_get_State(This,plObjState)	\
    (This)->lpVtbl -> get_State(This,plObjState)
#define Recordset21__xClone(This,ppvObject)	\
    (This)->lpVtbl -> _xClone(This,ppvObject)
#define Recordset21_UpdateBatch(This,AffectRecords)	\
    (This)->lpVtbl -> UpdateBatch(This,AffectRecords)
#define Recordset21_CancelBatch(This,AffectRecords)	\
    (This)->lpVtbl -> CancelBatch(This,AffectRecords)
#define Recordset21_get_CursorLocation(This,plCursorLoc)	\
    (This)->lpVtbl -> get_CursorLocation(This,plCursorLoc)
#define Recordset21_put_CursorLocation(This,lCursorLoc)	\
    (This)->lpVtbl -> put_CursorLocation(This,lCursorLoc)
#define Recordset21_NextRecordset(This,RecordsAffected,ppiRs)	\
    (This)->lpVtbl -> NextRecordset(This,RecordsAffected,ppiRs)
#define Recordset21_Supports(This,CursorOptions,pb)	\
    (This)->lpVtbl -> Supports(This,CursorOptions,pb)
#define Recordset21_get_Collect(This,Index,pvar)	\
    (This)->lpVtbl -> get_Collect(This,Index,pvar)
#define Recordset21_put_Collect(This,Index,value)	\
    (This)->lpVtbl -> put_Collect(This,Index,value)
#define Recordset21_get_MarshalOptions(This,peMarshal)	\
    (This)->lpVtbl -> get_MarshalOptions(This,peMarshal)
#define Recordset21_put_MarshalOptions(This,eMarshal)	\
    (This)->lpVtbl -> put_MarshalOptions(This,eMarshal)
#define Recordset21_Find(This,Criteria,SkipRecords,SearchDirection,Start)	\
    (This)->lpVtbl -> Find(This,Criteria,SkipRecords,SearchDirection,Start)
#define Recordset21_Cancel(This)	\
    (This)->lpVtbl -> Cancel(This)
#define Recordset21_get_DataSource(This,ppunkDataSource)	\
    (This)->lpVtbl -> get_DataSource(This,ppunkDataSource)
#define Recordset21_putref_DataSource(This,punkDataSource)	\
    (This)->lpVtbl -> putref_DataSource(This,punkDataSource)
#define Recordset21__xSave(This,FileName,PersistFormat)	\
    (This)->lpVtbl -> _xSave(This,FileName,PersistFormat)
#define Recordset21_get_ActiveCommand(This,ppCmd)	\
    (This)->lpVtbl -> get_ActiveCommand(This,ppCmd)
#define Recordset21_put_StayInSync(This,bStayInSync)	\
    (This)->lpVtbl -> put_StayInSync(This,bStayInSync)
#define Recordset21_get_StayInSync(This,pbStayInSync)	\
    (This)->lpVtbl -> get_StayInSync(This,pbStayInSync)
#define Recordset21_GetString(This,StringFormat,NumRows,ColumnDelimeter,RowDelimeter,NullExpr,pRetString)	\
    (This)->lpVtbl -> GetString(This,StringFormat,NumRows,ColumnDelimeter,RowDelimeter,NullExpr,pRetString)
#define Recordset21_get_DataMember(This,pbstrDataMember)	\
    (This)->lpVtbl -> get_DataMember(This,pbstrDataMember)
#define Recordset21_put_DataMember(This,bstrDataMember)	\
    (This)->lpVtbl -> put_DataMember(This,bstrDataMember)
#define Recordset21_CompareBookmarks(This,Bookmark1,Bookmark2,pCompare)	\
    (This)->lpVtbl -> CompareBookmarks(This,Bookmark1,Bookmark2,pCompare)
#define Recordset21_Clone(This,LockType,ppvObject)	\
    (This)->lpVtbl -> Clone(This,LockType,ppvObject)
#define Recordset21_Resync(This,AffectRecords,ResyncValues)	\
    (This)->lpVtbl -> Resync(This,AffectRecords,ResyncValues)
#define Recordset21_Seek(This,KeyValues,SeekOption)	\
    (This)->lpVtbl -> Seek(This,KeyValues,SeekOption)
#define Recordset21_put_Index(This,Index)	\
    (This)->lpVtbl -> put_Index(This,Index)
#define Recordset21_get_Index(This,pbstrIndex)	\
    (This)->lpVtbl -> get_Index(This,pbstrIndex)
#endif /* COBJMACROS */
#endif 	/* C style interface */
/* [helpcontext][id] */ HRESULT STDMETHODCALLTYPE Recordset21_Seek_Proxy( 
    Recordset21 __RPC_FAR * This,
    /* [in] */ VARIANT KeyValues,
    /* [defaultvalue][in] */ SeekEnum SeekOption);
void __RPC_STUB Recordset21_Seek_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);
/* [helpcontext][propput][id] */ HRESULT STDMETHODCALLTYPE Recordset21_put_Index_Proxy( 
    Recordset21 __RPC_FAR * This,
    /* [in] */ BSTR Index);
void __RPC_STUB Recordset21_put_Index_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);
/* [helpcontext][propget][id] */ HRESULT STDMETHODCALLTYPE Recordset21_get_Index_Proxy( 
    Recordset21 __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pbstrIndex);
void __RPC_STUB Recordset21_get_Index_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);
#endif 	/* __Recordset21_INTERFACE_DEFINED__ */
#ifndef ___Recordset_INTERFACE_DEFINED__
#define ___Recordset_INTERFACE_DEFINED__
/* interface _ADORecordset */
/* [object][helpcontext][uuid][nonextensible][dual] */ 
EXTERN_C const IID IID__Recordset;
#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("00000556-0000-0010-8000-00AA006D2EA4")
    _ADORecordset : public Recordset21
    {
    public:
        virtual /* [helpcontext][id] */ HRESULT STDMETHODCALLTYPE Save( 
            /* [optional][in] */ VARIANT Destination,
            /* [defaultvalue][in] */ PersistFormatEnum PersistFormat = adPersistADTG) = 0;
        
    };
    
#else 	/* C style interface */
    typedef struct _RecordsetVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            _ADORecordset __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            _ADORecordset __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            _ADORecordset __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            _ADORecordset __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            _ADORecordset __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            _ADORecordset __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            _ADORecordset __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        /* [helpcontext][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Properties )( 
            _ADORecordset __RPC_FAR * This,
            /* [retval][out] */ ADOProperties __RPC_FAR *__RPC_FAR *ppvObject);
        
        /* [helpcontext][propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_AbsolutePosition )( 
            _ADORecordset __RPC_FAR * This,
            /* [retval][out] */ PositionEnum __RPC_FAR *pl);
        
        /* [helpcontext][propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_AbsolutePosition )( 
            _ADORecordset __RPC_FAR * This,
            /* [in] */ PositionEnum Position);
        
        /* [helpcontext][propputref][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *putref_ActiveADOConnection )( 
            _ADORecordset __RPC_FAR * This,
            /* [in] */ IDispatch __RPC_FAR *pconn);
        
        /* [helpcontext][propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_ActiveConnection )( 
            _ADORecordset __RPC_FAR * This,
            /* [in] */ VARIANT vConn);
        
        /* [helpcontext][propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_ActiveConnection )( 
            _ADORecordset __RPC_FAR * This,
            /* [retval][out] */ VARIANT __RPC_FAR *pvar);
        
        /* [helpcontext][propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_BOF )( 
            _ADORecordset __RPC_FAR * This,
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pb);
        
        /* [helpcontext][propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Bookmark )( 
            _ADORecordset __RPC_FAR * This,
            /* [retval][out] */ VARIANT __RPC_FAR *pvBookmark);
        
        /* [helpcontext][propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Bookmark )( 
            _ADORecordset __RPC_FAR * This,
            /* [in] */ VARIANT vBookmark);
        
        /* [helpcontext][propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_CacheSize )( 
            _ADORecordset __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *pl);
        
        /* [helpcontext][propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_CacheSize )( 
            _ADORecordset __RPC_FAR * This,
            /* [in] */ long CacheSize);
        
        /* [helpcontext][propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_CursorType )( 
            _ADORecordset __RPC_FAR * This,
            /* [retval][out] */ CursorTypeEnum __RPC_FAR *plCursorType);
        
        /* [helpcontext][propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_CursorType )( 
            _ADORecordset __RPC_FAR * This,
            /* [in] */ CursorTypeEnum lCursorType);
        
        /* [helpcontext][propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_EOF )( 
            _ADORecordset __RPC_FAR * This,
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pb);
        
        /* [helpcontext][propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Fields )( 
            _ADORecordset __RPC_FAR * This,
            /* [retval][out] */ ADOFields __RPC_FAR *__RPC_FAR *ppvObject);
        
        /* [helpcontext][propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_LockType )( 
            _ADORecordset __RPC_FAR * This,
            /* [retval][out] */ LockTypeEnum __RPC_FAR *plLockType);
        
        /* [helpcontext][propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_LockType )( 
            _ADORecordset __RPC_FAR * This,
            /* [in] */ LockTypeEnum lLockType);
        
        /* [helpcontext][propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_MaxRecords )( 
            _ADORecordset __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *plMaxRecords);
        
        /* [helpcontext][propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_MaxRecords )( 
            _ADORecordset __RPC_FAR * This,
            /* [in] */ long lMaxRecords);
        
        /* [helpcontext][propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_RecordCount )( 
            _ADORecordset __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *pl);
        
        /* [helpcontext][propputref][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *putref_Source )( 
            _ADORecordset __RPC_FAR * This,
            /* [in] */ IDispatch __RPC_FAR *pcmd);
        
        /* [helpcontext][propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Source )( 
            _ADORecordset __RPC_FAR * This,
            /* [in] */ BSTR bstrConn);
        
        /* [helpcontext][propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Source )( 
            _ADORecordset __RPC_FAR * This,
            /* [retval][out] */ VARIANT __RPC_FAR *pvSource);
        
        /* [helpcontext][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *AddNew )( 
            _ADORecordset __RPC_FAR * This,
            /* [optional][in] */ VARIANT FieldList,
            /* [optional][in] */ VARIANT Values);
        
        /* [helpcontext][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *CancelUpdate )( 
            _ADORecordset __RPC_FAR * This);
        
        /* [helpcontext][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Close )( 
            _ADORecordset __RPC_FAR * This);
        
        /* [helpcontext][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Delete )( 
            _ADORecordset __RPC_FAR * This,
            /* [defaultvalue][in] */ AffectEnum AffectRecords);
        
        /* [helpcontext][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetRows )( 
            _ADORecordset __RPC_FAR * This,
            /* [defaultvalue][in] */ long Rows,
            /* [optional][in] */ VARIANT Start,
            /* [optional][in] */ VARIANT Fields,
            /* [retval][out] */ VARIANT __RPC_FAR *pvar);
        
        /* [helpcontext][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Move )( 
            _ADORecordset __RPC_FAR * This,
            /* [in] */ long NumRecords,
            /* [optional][in] */ VARIANT Start);
        
        /* [helpcontext][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *MoveNext )( 
            _ADORecordset __RPC_FAR * This);
        
        /* [helpcontext][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *MovePrevious )( 
            _ADORecordset __RPC_FAR * This);
        
        /* [helpcontext][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *MoveFirst )( 
            _ADORecordset __RPC_FAR * This);
        
        /* [helpcontext][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *MoveLast )( 
            _ADORecordset __RPC_FAR * This);
        
        /* [helpcontext][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Open )( 
            _ADORecordset __RPC_FAR * This,
            /* [optional][in] */ VARIANT Source,
            /* [optional][in] */ VARIANT ActiveConnection,
            /* [defaultvalue][in] */ CursorTypeEnum CursorType,
            /* [defaultvalue][in] */ LockTypeEnum LockType,
            /* [defaultvalue][in] */ LONG Options);
        
        /* [helpcontext][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Requery )( 
            _ADORecordset __RPC_FAR * This,
            /* [defaultvalue][in] */ LONG Options);
        
        /* [hidden] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *_xResync )( 
            _ADORecordset __RPC_FAR * This,
            /* [defaultvalue][in] */ AffectEnum AffectRecords);
        
        /* [helpcontext][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Update )( 
            _ADORecordset __RPC_FAR * This,
            /* [optional][in] */ VARIANT Fields,
            /* [optional][in] */ VARIANT Values);
        
        /* [helpcontext][propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_AbsolutePage )( 
            _ADORecordset __RPC_FAR * This,
            /* [retval][out] */ PositionEnum __RPC_FAR *pl);
        
        /* [helpcontext][propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_AbsolutePage )( 
            _ADORecordset __RPC_FAR * This,
            /* [in] */ PositionEnum Page);
        
        /* [helpcontext][propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_EditMode )( 
            _ADORecordset __RPC_FAR * This,
            /* [retval][out] */ EditModeEnum __RPC_FAR *pl);
        
        /* [helpcontext][propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Filter )( 
            _ADORecordset __RPC_FAR * This,
            /* [retval][out] */ VARIANT __RPC_FAR *Criteria);
        
        /* [helpcontext][propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Filter )( 
            _ADORecordset __RPC_FAR * This,
            /* [in] */ VARIANT Criteria);
        
        /* [helpcontext][propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_PageCount )( 
            _ADORecordset __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *pl);
        
        /* [helpcontext][propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_PageSize )( 
            _ADORecordset __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *pl);
        
        /* [helpcontext][propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_PageSize )( 
            _ADORecordset __RPC_FAR * This,
            /* [in] */ long PageSize);
        
        /* [helpcontext][propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Sort )( 
            _ADORecordset __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *Criteria);
        
        /* [helpcontext][propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Sort )( 
            _ADORecordset __RPC_FAR * This,
            /* [in] */ BSTR Criteria);
        
        /* [helpcontext][propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Status )( 
            _ADORecordset __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *pl);
        
        /* [helpcontext][propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_State )( 
            _ADORecordset __RPC_FAR * This,
            /* [retval][out] */ LONG __RPC_FAR *plObjState);
        
        /* [hidden] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *_xClone )( 
            _ADORecordset __RPC_FAR * This,
            /* [retval][out] */ _ADORecordset __RPC_FAR *__RPC_FAR *ppvObject);
        
        /* [helpcontext][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *UpdateBatch )( 
            _ADORecordset __RPC_FAR * This,
            /* [defaultvalue][in] */ AffectEnum AffectRecords);
        
        /* [helpcontext][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *CancelBatch )( 
            _ADORecordset __RPC_FAR * This,
            /* [defaultvalue][in] */ AffectEnum AffectRecords);
        
        /* [helpcontext][propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_CursorLocation )( 
            _ADORecordset __RPC_FAR * This,
            /* [retval][out] */ CursorLocationEnum __RPC_FAR *plCursorLoc);
        
        /* [helpcontext][propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_CursorLocation )( 
            _ADORecordset __RPC_FAR * This,
            /* [in] */ CursorLocationEnum lCursorLoc);
        
        /* [helpcontext][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *NextADORecordset )( 
            _ADORecordset __RPC_FAR * This,
            /* [optional][out] */ VARIANT __RPC_FAR *RecordsAffected,
            /* [retval][out] */ _ADORecordset __RPC_FAR *__RPC_FAR *ppiRs);
        
        /* [helpcontext][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Supports )( 
            _ADORecordset __RPC_FAR * This,
            /* [in] */ CursorOptionEnum CursorOptions,
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pb);
        
        /* [hidden][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Collect )( 
            _ADORecordset __RPC_FAR * This,
            /* [in] */ VARIANT Index,
            /* [retval][out] */ VARIANT __RPC_FAR *pvar);
        
        /* [hidden][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Collect )( 
            _ADORecordset __RPC_FAR * This,
            /* [in] */ VARIANT Index,
            /* [in] */ VARIANT value);
        
        /* [helpcontext][propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_MarshalOptions )( 
            _ADORecordset __RPC_FAR * This,
            /* [retval][out] */ MarshalOptionsEnum __RPC_FAR *peMarshal);
        
        /* [helpcontext][propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_MarshalOptions )( 
            _ADORecordset __RPC_FAR * This,
            /* [in] */ MarshalOptionsEnum eMarshal);
        
        /* [helpcontext][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Find )( 
            _ADORecordset __RPC_FAR * This,
            /* [in] */ BSTR Criteria,
            /* [defaultvalue][in] */ LONG SkipRecords,
            /* [defaultvalue][in] */ SearchDirectionEnum SearchDirection,
            /* [optional][in] */ VARIANT Start);
        
        /* [helpcontext][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Cancel )( 
            _ADORecordset __RPC_FAR * This);
        
        /* [helpcontext][propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_DataSource )( 
            _ADORecordset __RPC_FAR * This,
            /* [retval][out] */ IUnknown __RPC_FAR *__RPC_FAR *ppunkDataSource);
        
        /* [helpcontext][propputref][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *putref_DataSource )( 
            _ADORecordset __RPC_FAR * This,
            /* [in] */ IUnknown __RPC_FAR *punkDataSource);
        
        /* [hidden] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *_xSave )( 
            _ADORecordset __RPC_FAR * This,
            /* [optional][in] */ BSTR FileName,
            /* [defaultvalue][in] */ PersistFormatEnum PersistFormat);
        
        /* [helpcontext][propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_ActiveCommand )( 
            _ADORecordset __RPC_FAR * This,
            /* [retval][out] */ IDispatch __RPC_FAR *__RPC_FAR *ppCmd);
        
        /* [helpcontext][propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_StayInSync )( 
            _ADORecordset __RPC_FAR * This,
            /* [in] */ VARIANT_BOOL bStayInSync);
        
        /* [helpcontext][propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_StayInSync )( 
            _ADORecordset __RPC_FAR * This,
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pbStayInSync);
        
        /* [helpcontext][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetString )( 
            _ADORecordset __RPC_FAR * This,
            /* [defaultvalue][in] */ StringFormatEnum StringFormat,
            /* [defaultvalue][in] */ long NumRows,
            /* [optional][in] */ BSTR ColumnDelimeter,
            /* [optional][in] */ BSTR RowDelimeter,
            /* [optional][in] */ BSTR NullExpr,
            /* [retval][out] */ BSTR __RPC_FAR *pRetString);
        
        /* [helpcontext][propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_DataMember )( 
            _ADORecordset __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pbstrDataMember);
        
        /* [helpcontext][propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_DataMember )( 
            _ADORecordset __RPC_FAR * This,
            /* [in] */ BSTR bstrDataMember);
        
        /* [helpcontext][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *CompareBookmarks )( 
            _ADORecordset __RPC_FAR * This,
            /* [in] */ VARIANT Bookmark1,
            /* [in] */ VARIANT Bookmark2,
            /* [retval][out] */ CompareEnum __RPC_FAR *pCompare);
        
        /* [helpcontext][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Clone )( 
            _ADORecordset __RPC_FAR * This,
            /* [defaultvalue][in] */ LockTypeEnum LockType,
            /* [retval][out] */ _ADORecordset __RPC_FAR *__RPC_FAR *ppvObject);
        
        /* [helpcontext][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Resync )( 
            _ADORecordset __RPC_FAR * This,
            /* [defaultvalue][in] */ AffectEnum AffectRecords,
            /* [defaultvalue][in] */ ResyncEnum ResyncValues);
        
        /* [helpcontext][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Seek )( 
            _ADORecordset __RPC_FAR * This,
            /* [in] */ VARIANT KeyValues,
            /* [defaultvalue][in] */ SeekEnum SeekOption);
        
        /* [helpcontext][propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Index )( 
            _ADORecordset __RPC_FAR * This,
            /* [in] */ BSTR Index);
        
        /* [helpcontext][propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Index )( 
            _ADORecordset __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pbstrIndex);
        
        /* [helpcontext][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Save )( 
            _ADORecordset __RPC_FAR * This,
            /* [optional][in] */ VARIANT Destination,
            /* [defaultvalue][in] */ PersistFormatEnum PersistFormat);
        
        END_INTERFACE
    } _RecordsetVtbl;
    interface _Recordset
    {
        CONST_VTBL struct _RecordsetVtbl __RPC_FAR *lpVtbl;
    };
    
#ifdef COBJMACROS
#define _Recordset_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)
#define _Recordset_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)
#define _Recordset_Release(This)	\
    (This)->lpVtbl -> Release(This)
#define _Recordset_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)
#define _Recordset_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)
#define _Recordset_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)
#define _Recordset_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)
#define _Recordset_get_Properties(This,ppvObject)	\
    (This)->lpVtbl -> get_Properties(This,ppvObject)
#define _Recordset_get_AbsolutePosition(This,pl)	\
    (This)->lpVtbl -> get_AbsolutePosition(This,pl)
#define _Recordset_put_AbsolutePosition(This,Position)	\
    (This)->lpVtbl -> put_AbsolutePosition(This,Position)
#define _Recordset_putref_ActiveConnection(This,pconn)	\
    (This)->lpVtbl -> putref_ActiveConnection(This,pconn)
#define _Recordset_put_ActiveConnection(This,vConn)	\
    (This)->lpVtbl -> put_ActiveConnection(This,vConn)
#define _Recordset_get_ActiveConnection(This,pvar)	\
    (This)->lpVtbl -> get_ActiveConnection(This,pvar)
#define _Recordset_get_BOF(This,pb)	\
    (This)->lpVtbl -> get_BOF(This,pb)
#define _Recordset_get_Bookmark(This,pvBookmark)	\
    (This)->lpVtbl -> get_Bookmark(This,pvBookmark)
#define _Recordset_put_Bookmark(This,vBookmark)	\
    (This)->lpVtbl -> put_Bookmark(This,vBookmark)
#define _Recordset_get_CacheSize(This,pl)	\
    (This)->lpVtbl -> get_CacheSize(This,pl)
#define _Recordset_put_CacheSize(This,CacheSize)	\
    (This)->lpVtbl -> put_CacheSize(This,CacheSize)
#define _Recordset_get_CursorType(This,plCursorType)	\
    (This)->lpVtbl -> get_CursorType(This,plCursorType)
#define _Recordset_put_CursorType(This,lCursorType)	\
    (This)->lpVtbl -> put_CursorType(This,lCursorType)
#define _Recordset_get_EOF(This,pb)	\
    (This)->lpVtbl -> get_EOF(This,pb)
#define _Recordset_get_Fields(This,ppvObject)	\
    (This)->lpVtbl -> get_Fields(This,ppvObject)
#define _Recordset_get_LockType(This,plLockType)	\
    (This)->lpVtbl -> get_LockType(This,plLockType)
#define _Recordset_put_LockType(This,lLockType)	\
    (This)->lpVtbl -> put_LockType(This,lLockType)
#define _Recordset_get_MaxRecords(This,plMaxRecords)	\
    (This)->lpVtbl -> get_MaxRecords(This,plMaxRecords)
#define _Recordset_put_MaxRecords(This,lMaxRecords)	\
    (This)->lpVtbl -> put_MaxRecords(This,lMaxRecords)
#define _Recordset_get_RecordCount(This,pl)	\
    (This)->lpVtbl -> get_RecordCount(This,pl)
#define _Recordset_putref_Source(This,pcmd)	\
    (This)->lpVtbl -> putref_Source(This,pcmd)
#define _Recordset_put_Source(This,bstrConn)	\
    (This)->lpVtbl -> put_Source(This,bstrConn)
#define _Recordset_get_Source(This,pvSource)	\
    (This)->lpVtbl -> get_Source(This,pvSource)
#define _Recordset_AddNew(This,FieldList,Values)	\
    (This)->lpVtbl -> AddNew(This,FieldList,Values)
#define _Recordset_CancelUpdate(This)	\
    (This)->lpVtbl -> CancelUpdate(This)
#define _Recordset_Close(This)	\
    (This)->lpVtbl -> Close(This)
#define _Recordset_Delete(This,AffectRecords)	\
    (This)->lpVtbl -> Delete(This,AffectRecords)
#define _Recordset_GetRows(This,Rows,Start,Fields,pvar)	\
    (This)->lpVtbl -> GetRows(This,Rows,Start,Fields,pvar)
#define _Recordset_Move(This,NumRecords,Start)	\
    (This)->lpVtbl -> Move(This,NumRecords,Start)
#define _Recordset_MoveNext(This)	\
    (This)->lpVtbl -> MoveNext(This)
#define _Recordset_MovePrevious(This)	\
    (This)->lpVtbl -> MovePrevious(This)
#define _Recordset_MoveFirst(This)	\
    (This)->lpVtbl -> MoveFirst(This)
#define _Recordset_MoveLast(This)	\
    (This)->lpVtbl -> MoveLast(This)
#define _Recordset_Open(This,Source,ActiveConnection,CursorType,LockType,Options)	\
    (This)->lpVtbl -> Open(This,Source,ActiveConnection,CursorType,LockType,Options)
#define _Recordset_Requery(This,Options)	\
    (This)->lpVtbl -> Requery(This,Options)
#define _Recordset__xResync(This,AffectRecords)	\
    (This)->lpVtbl -> _xResync(This,AffectRecords)
#define _Recordset_Update(This,Fields,Values)	\
    (This)->lpVtbl -> Update(This,Fields,Values)
#define _Recordset_get_AbsolutePage(This,pl)	\
    (This)->lpVtbl -> get_AbsolutePage(This,pl)
#define _Recordset_put_AbsolutePage(This,Page)	\
    (This)->lpVtbl -> put_AbsolutePage(This,Page)
#define _Recordset_get_EditMode(This,pl)	\
    (This)->lpVtbl -> get_EditMode(This,pl)
#define _Recordset_get_Filter(This,Criteria)	\
    (This)->lpVtbl -> get_Filter(This,Criteria)
#define _Recordset_put_Filter(This,Criteria)	\
    (This)->lpVtbl -> put_Filter(This,Criteria)
#define _Recordset_get_PageCount(This,pl)	\
    (This)->lpVtbl -> get_PageCount(This,pl)
#define _Recordset_get_PageSize(This,pl)	\
    (This)->lpVtbl -> get_PageSize(This,pl)
#define _Recordset_put_PageSize(This,PageSize)	\
    (This)->lpVtbl -> put_PageSize(This,PageSize)
#define _Recordset_get_Sort(This,Criteria)	\
    (This)->lpVtbl -> get_Sort(This,Criteria)
#define _Recordset_put_Sort(This,Criteria)	\
    (This)->lpVtbl -> put_Sort(This,Criteria)
#define _Recordset_get_Status(This,pl)	\
    (This)->lpVtbl -> get_Status(This,pl)
#define _Recordset_get_State(This,plObjState)	\
    (This)->lpVtbl -> get_State(This,plObjState)
#define _Recordset__xClone(This,ppvObject)	\
    (This)->lpVtbl -> _xClone(This,ppvObject)
#define _Recordset_UpdateBatch(This,AffectRecords)	\
    (This)->lpVtbl -> UpdateBatch(This,AffectRecords)
#define _Recordset_CancelBatch(This,AffectRecords)	\
    (This)->lpVtbl -> CancelBatch(This,AffectRecords)
#define _Recordset_get_CursorLocation(This,plCursorLoc)	\
    (This)->lpVtbl -> get_CursorLocation(This,plCursorLoc)
#define _Recordset_put_CursorLocation(This,lCursorLoc)	\
    (This)->lpVtbl -> put_CursorLocation(This,lCursorLoc)
#define _Recordset_NextRecordset(This,RecordsAffected,ppiRs)	\
    (This)->lpVtbl -> NextRecordset(This,RecordsAffected,ppiRs)
#define _Recordset_Supports(This,CursorOptions,pb)	\
    (This)->lpVtbl -> Supports(This,CursorOptions,pb)
#define _Recordset_get_Collect(This,Index,pvar)	\
    (This)->lpVtbl -> get_Collect(This,Index,pvar)
#define _Recordset_put_Collect(This,Index,value)	\
    (This)->lpVtbl -> put_Collect(This,Index,value)
#define _Recordset_get_MarshalOptions(This,peMarshal)	\
    (This)->lpVtbl -> get_MarshalOptions(This,peMarshal)
#define _Recordset_put_MarshalOptions(This,eMarshal)	\
    (This)->lpVtbl -> put_MarshalOptions(This,eMarshal)
#define _Recordset_Find(This,Criteria,SkipRecords,SearchDirection,Start)	\
    (This)->lpVtbl -> Find(This,Criteria,SkipRecords,SearchDirection,Start)
#define _Recordset_Cancel(This)	\
    (This)->lpVtbl -> Cancel(This)
#define _Recordset_get_DataSource(This,ppunkDataSource)	\
    (This)->lpVtbl -> get_DataSource(This,ppunkDataSource)
#define _Recordset_putref_DataSource(This,punkDataSource)	\
    (This)->lpVtbl -> putref_DataSource(This,punkDataSource)
#define _Recordset__xSave(This,FileName,PersistFormat)	\
    (This)->lpVtbl -> _xSave(This,FileName,PersistFormat)
#define _Recordset_get_ActiveCommand(This,ppCmd)	\
    (This)->lpVtbl -> get_ActiveCommand(This,ppCmd)
#define _Recordset_put_StayInSync(This,bStayInSync)	\
    (This)->lpVtbl -> put_StayInSync(This,bStayInSync)
#define _Recordset_get_StayInSync(This,pbStayInSync)	\
    (This)->lpVtbl -> get_StayInSync(This,pbStayInSync)
#define _Recordset_GetString(This,StringFormat,NumRows,ColumnDelimeter,RowDelimeter,NullExpr,pRetString)	\
    (This)->lpVtbl -> GetString(This,StringFormat,NumRows,ColumnDelimeter,RowDelimeter,NullExpr,pRetString)
#define _Recordset_get_DataMember(This,pbstrDataMember)	\
    (This)->lpVtbl -> get_DataMember(This,pbstrDataMember)
#define _Recordset_put_DataMember(This,bstrDataMember)	\
    (This)->lpVtbl -> put_DataMember(This,bstrDataMember)
#define _Recordset_CompareBookmarks(This,Bookmark1,Bookmark2,pCompare)	\
    (This)->lpVtbl -> CompareBookmarks(This,Bookmark1,Bookmark2,pCompare)
#define _Recordset_Clone(This,LockType,ppvObject)	\
    (This)->lpVtbl -> Clone(This,LockType,ppvObject)
#define _Recordset_Resync(This,AffectRecords,ResyncValues)	\
    (This)->lpVtbl -> Resync(This,AffectRecords,ResyncValues)
#define _Recordset_Seek(This,KeyValues,SeekOption)	\
    (This)->lpVtbl -> Seek(This,KeyValues,SeekOption)
#define _Recordset_put_Index(This,Index)	\
    (This)->lpVtbl -> put_Index(This,Index)
#define _Recordset_get_Index(This,pbstrIndex)	\
    (This)->lpVtbl -> get_Index(This,pbstrIndex)
#define _Recordset_Save(This,Destination,PersistFormat)	\
    (This)->lpVtbl -> Save(This,Destination,PersistFormat)
#endif /* COBJMACROS */
#endif 	/* C style interface */
/* [helpcontext][id] */ HRESULT STDMETHODCALLTYPE _Recordset_Save_Proxy( 
    _ADORecordset __RPC_FAR * This,
    /* [optional][in] */ VARIANT Destination,
    /* [defaultvalue][in] */ PersistFormatEnum PersistFormat);
void __RPC_STUB _Recordset_Save_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);
#endif 	/* ___Recordset_INTERFACE_DEFINED__ */
#ifndef __ADORecordsetConstruction_INTERFACE_DEFINED__
#define __ADORecordsetConstruction_INTERFACE_DEFINED__
/* interface ADORecordsetConstruction */
/* [object][uuid][restricted] */ 
EXTERN_C const IID IID_ADORecordsetConstruction;
#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("00000283-0000-0010-8000-00AA006D2EA4")
    ADORecordsetConstruction : public IDispatch
    {
    public:
        virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_Rowset( 
            /* [retval][out] */ IUnknown __RPC_FAR *__RPC_FAR *ppRowset) = 0;
        
        virtual /* [propput] */ HRESULT STDMETHODCALLTYPE put_Rowset( 
            /* [in] */ IUnknown __RPC_FAR *pRowset) = 0;
        
        virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_Chapter( 
            /* [retval][out] */ ADO_DBLONGPTR __RPC_FAR *plChapter) = 0;
        
        virtual /* [propput] */ HRESULT STDMETHODCALLTYPE put_Chapter( 
            /* [in] */ ADO_DBLONGPTR lChapter) = 0;
        
        virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_RowPosition( 
            /* [retval][out] */ IUnknown __RPC_FAR *__RPC_FAR *ppRowPos) = 0;
        
        virtual /* [propput] */ HRESULT STDMETHODCALLTYPE put_RowPosition( 
            /* [in] */ IUnknown __RPC_FAR *pRowPos) = 0;
        
    };
    
#else 	/* C style interface */
    typedef struct ADORecordsetConstructionVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            ADORecordsetConstruction __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            ADORecordsetConstruction __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            ADORecordsetConstruction __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            ADORecordsetConstruction __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            ADORecordsetConstruction __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            ADORecordsetConstruction __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            ADORecordsetConstruction __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Rowset )( 
            ADORecordsetConstruction __RPC_FAR * This,
            /* [retval][out] */ IUnknown __RPC_FAR *__RPC_FAR *ppRowset);
        
        /* [propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Rowset )( 
            ADORecordsetConstruction __RPC_FAR * This,
            /* [in] */ IUnknown __RPC_FAR *pRowset);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Chapter )( 
            ADORecordsetConstruction __RPC_FAR * This,
            /* [retval][out] */ ADO_DBLONGPTR __RPC_FAR *plChapter);
        
        /* [propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Chapter )( 
            ADORecordsetConstruction __RPC_FAR * This,
            /* [in] */ ADO_DBLONGPTR lChapter);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_RowPosition )( 
            ADORecordsetConstruction __RPC_FAR * This,
            /* [retval][out] */ IUnknown __RPC_FAR *__RPC_FAR *ppRowPos);
        
        /* [propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_RowPosition )( 
            ADORecordsetConstruction __RPC_FAR * This,
            /* [in] */ IUnknown __RPC_FAR *pRowPos);
        
        END_INTERFACE
    } ADORecordsetConstructionVtbl;
    interface ADORecordsetConstruction
    {
        CONST_VTBL struct ADORecordsetConstructionVtbl __RPC_FAR *lpVtbl;
    };
    
#ifdef COBJMACROS
#define ADORecordsetConstruction_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)
#define ADORecordsetConstruction_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)
#define ADORecordsetConstruction_Release(This)	\
    (This)->lpVtbl -> Release(This)
#define ADORecordsetConstruction_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)
#define ADORecordsetConstruction_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)
#define ADORecordsetConstruction_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)
#define ADORecordsetConstruction_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)
#define ADORecordsetConstruction_get_Rowset(This,ppRowset)	\
    (This)->lpVtbl -> get_Rowset(This,ppRowset)
#define ADORecordsetConstruction_put_Rowset(This,pRowset)	\
    (This)->lpVtbl -> put_Rowset(This,pRowset)
#define ADORecordsetConstruction_get_Chapter(This,plChapter)	\
    (This)->lpVtbl -> get_Chapter(This,plChapter)
#define ADORecordsetConstruction_put_Chapter(This,lChapter)	\
    (This)->lpVtbl -> put_Chapter(This,lChapter)
#define ADORecordsetConstruction_get_RowPosition(This,ppRowPos)	\
    (This)->lpVtbl -> get_RowPosition(This,ppRowPos)
#define ADORecordsetConstruction_put_RowPosition(This,pRowPos)	\
    (This)->lpVtbl -> put_RowPosition(This,pRowPos)
#endif /* COBJMACROS */
#endif 	/* C style interface */
/* [propget] */ HRESULT STDMETHODCALLTYPE ADORecordsetConstruction_get_Rowset_Proxy( 
    ADORecordsetConstruction __RPC_FAR * This,
    /* [retval][out] */ IUnknown __RPC_FAR *__RPC_FAR *ppRowset);
void __RPC_STUB ADORecordsetConstruction_get_Rowset_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);
/* [propput] */ HRESULT STDMETHODCALLTYPE ADORecordsetConstruction_put_Rowset_Proxy( 
    ADORecordsetConstruction __RPC_FAR * This,
    /* [in] */ IUnknown __RPC_FAR *pRowset);
void __RPC_STUB ADORecordsetConstruction_put_Rowset_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);
/* [propget] */ HRESULT STDMETHODCALLTYPE ADORecordsetConstruction_get_Chapter_Proxy( 
    ADORecordsetConstruction __RPC_FAR * This,
    /* [retval][out] */ ADO_DBLONGPTR __RPC_FAR *plChapter);
void __RPC_STUB ADORecordsetConstruction_get_Chapter_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);
/* [propput] */ HRESULT STDMETHODCALLTYPE ADORecordsetConstruction_put_Chapter_Proxy( 
    ADORecordsetConstruction __RPC_FAR * This,
    /* [in] */ ADO_DBLONGPTR lChapter);
void __RPC_STUB ADORecordsetConstruction_put_Chapter_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);
/* [propget] */ HRESULT STDMETHODCALLTYPE ADORecordsetConstruction_get_RowPosition_Proxy( 
    ADORecordsetConstruction __RPC_FAR * This,
    /* [retval][out] */ IUnknown __RPC_FAR *__RPC_FAR *ppRowPos);
void __RPC_STUB ADORecordsetConstruction_get_RowPosition_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);
/* [propput] */ HRESULT STDMETHODCALLTYPE ADORecordsetConstruction_put_RowPosition_Proxy( 
    ADORecordsetConstruction __RPC_FAR * This,
    /* [in] */ IUnknown __RPC_FAR *pRowPos);
void __RPC_STUB ADORecordsetConstruction_put_RowPosition_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);
#endif 	/* __ADORecordsetConstruction_INTERFACE_DEFINED__ */
#ifndef __Field15_INTERFACE_DEFINED__
#define __Field15_INTERFACE_DEFINED__
/* interface Field15 */
/* [object][helpcontext][uuid][hidden][nonextensible][dual] */ 
EXTERN_C const IID IID_Field15;
#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("00000505-0000-0010-8000-00AA006D2EA4")
    Field15 : public _ADO
    {
    public:
        virtual /* [helpcontext][propget][id] */ HRESULT STDMETHODCALLTYPE get_ActualSize( 
            /* [retval][out] */ long __RPC_FAR *pl) = 0;
        
        virtual /* [helpcontext][propget][id] */ HRESULT STDMETHODCALLTYPE get_Attributes( 
            /* [retval][out] */ long __RPC_FAR *pl) = 0;
        
        virtual /* [helpcontext][propget][id] */ HRESULT STDMETHODCALLTYPE get_DefinedSize( 
            /* [retval][out] */ long __RPC_FAR *pl) = 0;
        
        virtual /* [helpcontext][propget][id] */ HRESULT STDMETHODCALLTYPE get_Name( 
            /* [retval][out] */ BSTR __RPC_FAR *pbstr) = 0;
        
        virtual /* [helpcontext][propget][id] */ HRESULT STDMETHODCALLTYPE get_Type( 
            /* [retval][out] */ DataTypeEnum __RPC_FAR *pDataType) = 0;
        
        virtual /* [helpcontext][propget][id] */ HRESULT STDMETHODCALLTYPE get_Value( 
            /* [retval][out] */ VARIANT __RPC_FAR *pvar) = 0;
        
        virtual /* [helpcontext][propput][id] */ HRESULT STDMETHODCALLTYPE put_Value( 
            /* [in] */ VARIANT Val) = 0;
        
        virtual /* [helpcontext][propget][id] */ HRESULT STDMETHODCALLTYPE get_Precision( 
            /* [retval][out] */ BYTE __RPC_FAR *pbPrecision) = 0;
        
        virtual /* [helpcontext][propget][id] */ HRESULT STDMETHODCALLTYPE get_NumericScale( 
            /* [retval][out] */ BYTE __RPC_FAR *pbNumericScale) = 0;
        
        virtual /* [helpcontext][id] */ HRESULT STDMETHODCALLTYPE AppendChunk( 
            /* [in] */ VARIANT Data) = 0;
        
        virtual /* [helpcontext][id] */ HRESULT STDMETHODCALLTYPE GetChunk( 
            /* [in] */ long Length,
            /* [retval][out] */ VARIANT __RPC_FAR *pvar) = 0;
        
        virtual /* [helpcontext][propget][id] */ HRESULT STDMETHODCALLTYPE get_OriginalValue( 
            /* [retval][out] */ VARIANT __RPC_FAR *pvar) = 0;
        
        virtual /* [helpcontext][propget][id] */ HRESULT STDMETHODCALLTYPE get_UnderlyingValue( 
            /* [retval][out] */ VARIANT __RPC_FAR *pvar) = 0;
        
    };
    
#else 	/* C style interface */
    typedef struct Field15Vtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            Field15 __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            Field15 __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            Field15 __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            Field15 __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            Field15 __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            Field15 __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            Field15 __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        /* [helpcontext][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Properties )( 
            Field15 __RPC_FAR * This,
            /* [retval][out] */ ADOProperties __RPC_FAR *__RPC_FAR *ppvObject);
        
        /* [helpcontext][propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_ActualSize )( 
            Field15 __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *pl);
        
        /* [helpcontext][propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Attributes )( 
            Field15 __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *pl);
        
        /* [helpcontext][propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_DefinedSize )( 
            Field15 __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *pl);
        
        /* [helpcontext][propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Name )( 
            Field15 __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pbstr);
        
        /* [helpcontext][propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Type )( 
            Field15 __RPC_FAR * This,
            /* [retval][out] */ DataTypeEnum __RPC_FAR *pDataType);
        
        /* [helpcontext][propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Value )( 
            Field15 __RPC_FAR * This,
            /* [retval][out] */ VARIANT __RPC_FAR *pvar);
        
        /* [helpcontext][propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Value )( 
            Field15 __RPC_FAR * This,
            /* [in] */ VARIANT Val);
        
        /* [helpcontext][propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Precision )( 
            Field15 __RPC_FAR * This,
            /* [retval][out] */ BYTE __RPC_FAR *pbPrecision);
        
        /* [helpcontext][propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_NumericScale )( 
            Field15 __RPC_FAR * This,
            /* [retval][out] */ BYTE __RPC_FAR *pbNumericScale);
        
        /* [helpcontext][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *AppendChunk )( 
            Field15 __RPC_FAR * This,
            /* [in] */ VARIANT Data);
        
        /* [helpcontext][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetChunk )( 
            Field15 __RPC_FAR * This,
            /* [in] */ long Length,
            /* [retval][out] */ VARIANT __RPC_FAR *pvar);
        
        /* [helpcontext][propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_OriginalValue )( 
            Field15 __RPC_FAR * This,
            /* [retval][out] */ VARIANT __RPC_FAR *pvar);
        
        /* [helpcontext][propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_UnderlyingValue )( 
            Field15 __RPC_FAR * This,
            /* [retval][out] */ VARIANT __RPC_FAR *pvar);
        
        END_INTERFACE
    } Field15Vtbl;
    interface Field15
    {
        CONST_VTBL struct Field15Vtbl __RPC_FAR *lpVtbl;
    };
    
#ifdef COBJMACROS
#define Field15_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)
#define Field15_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)
#define Field15_Release(This)	\
    (This)->lpVtbl -> Release(This)
#define Field15_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)
#define Field15_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)
#define Field15_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)
#define Field15_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)
#define Field15_get_Properties(This,ppvObject)	\
    (This)->lpVtbl -> get_Properties(This,ppvObject)
#define Field15_get_ActualSize(This,pl)	\
    (This)->lpVtbl -> get_ActualSize(This,pl)
#define Field15_get_Attributes(This,pl)	\
    (This)->lpVtbl -> get_Attributes(This,pl)
#define Field15_get_DefinedSize(This,pl)	\
    (This)->lpVtbl -> get_DefinedSize(This,pl)
#define Field15_get_Name(This,pbstr)	\
    (This)->lpVtbl -> get_Name(This,pbstr)
#define Field15_get_Type(This,pDataType)	\
    (This)->lpVtbl -> get_Type(This,pDataType)
#define Field15_get_Value(This,pvar)	\
    (This)->lpVtbl -> get_Value(This,pvar)
#define Field15_put_Value(This,Val)	\
    (This)->lpVtbl -> put_Value(This,Val)
#define Field15_get_Precision(This,pbPrecision)	\
    (This)->lpVtbl -> get_Precision(This,pbPrecision)
#define Field15_get_NumericScale(This,pbNumericScale)	\
    (This)->lpVtbl -> get_NumericScale(This,pbNumericScale)
#define Field15_AppendChunk(This,Data)	\
    (This)->lpVtbl -> AppendChunk(This,Data)
#define Field15_GetChunk(This,Length,pvar)	\
    (This)->lpVtbl -> GetChunk(This,Length,pvar)
#define Field15_get_OriginalValue(This,pvar)	\
    (This)->lpVtbl -> get_OriginalValue(This,pvar)
#define Field15_get_UnderlyingValue(This,pvar)	\
    (This)->lpVtbl -> get_UnderlyingValue(This,pvar)
#endif /* COBJMACROS */
#endif 	/* C style interface */
/* [helpcontext][propget][id] */ HRESULT STDMETHODCALLTYPE Field15_get_ActualSize_Proxy( 
    Field15 __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *pl);
void __RPC_STUB Field15_get_ActualSize_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);
/* [helpcontext][propget][id] */ HRESULT STDMETHODCALLTYPE Field15_get_Attributes_Proxy( 
    Field15 __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *pl);
void __RPC_STUB Field15_get_Attributes_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);
/* [helpcontext][propget][id] */ HRESULT STDMETHODCALLTYPE Field15_get_DefinedSize_Proxy( 
    Field15 __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *pl);
void __RPC_STUB Field15_get_DefinedSize_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);
/* [helpcontext][propget][id] */ HRESULT STDMETHODCALLTYPE Field15_get_Name_Proxy( 
    Field15 __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pbstr);
void __RPC_STUB Field15_get_Name_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);
/* [helpcontext][propget][id] */ HRESULT STDMETHODCALLTYPE Field15_get_Type_Proxy( 
    Field15 __RPC_FAR * This,
    /* [retval][out] */ DataTypeEnum __RPC_FAR *pDataType);
void __RPC_STUB Field15_get_Type_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);
/* [helpcontext][propget][id] */ HRESULT STDMETHODCALLTYPE Field15_get_Value_Proxy( 
    Field15 __RPC_FAR * This,
    /* [retval][out] */ VARIANT __RPC_FAR *pvar);
void __RPC_STUB Field15_get_Value_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);
/* [helpcontext][propput][id] */ HRESULT STDMETHODCALLTYPE Field15_put_Value_Proxy( 
    Field15 __RPC_FAR * This,
    /* [in] */ VARIANT Val);
void __RPC_STUB Field15_put_Value_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);
/* [helpcontext][propget][id] */ HRESULT STDMETHODCALLTYPE Field15_get_Precision_Proxy( 
    Field15 __RPC_FAR * This,
    /* [retval][out] */ BYTE __RPC_FAR *pbPrecision);
void __RPC_STUB Field15_get_Precision_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);
/* [helpcontext][propget][id] */ HRESULT STDMETHODCALLTYPE Field15_get_NumericScale_Proxy( 
    Field15 __RPC_FAR * This,
    /* [retval][out] */ BYTE __RPC_FAR *pbNumericScale);
void __RPC_STUB Field15_get_NumericScale_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);
/* [helpcontext][id] */ HRESULT STDMETHODCALLTYPE Field15_AppendChunk_Proxy( 
    Field15 __RPC_FAR * This,
    /* [in] */ VARIANT Data);
void __RPC_STUB Field15_AppendChunk_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);
/* [helpcontext][id] */ HRESULT STDMETHODCALLTYPE Field15_GetChunk_Proxy( 
    Field15 __RPC_FAR * This,
    /* [in] */ long Length,
    /* [retval][out] */ VARIANT __RPC_FAR *pvar);
void __RPC_STUB Field15_GetChunk_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);
/* [helpcontext][propget][id] */ HRESULT STDMETHODCALLTYPE Field15_get_OriginalValue_Proxy( 
    Field15 __RPC_FAR * This,
    /* [retval][out] */ VARIANT __RPC_FAR *pvar);
void __RPC_STUB Field15_get_OriginalValue_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);
/* [helpcontext][propget][id] */ HRESULT STDMETHODCALLTYPE Field15_get_UnderlyingValue_Proxy( 
    Field15 __RPC_FAR * This,
    /* [retval][out] */ VARIANT __RPC_FAR *pvar);
void __RPC_STUB Field15_get_UnderlyingValue_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);
#endif 	/* __Field15_INTERFACE_DEFINED__ */
#ifndef __Field20_INTERFACE_DEFINED__
#define __Field20_INTERFACE_DEFINED__
/* interface Field20 */
/* [object][helpcontext][uuid][hidden][nonextensible][dual] */ 
EXTERN_C const IID IID_Field20;
#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("0000054C-0000-0010-8000-00AA006D2EA4")
    Field20 : public _ADO
    {
    public:
        virtual /* [helpcontext][propget][id] */ HRESULT STDMETHODCALLTYPE get_ActualSize( 
            /* [retval][out] */ long __RPC_FAR *pl) = 0;
        
        virtual /* [helpcontext][propget][id] */ HRESULT STDMETHODCALLTYPE get_Attributes( 
            /* [retval][out] */ long __RPC_FAR *pl) = 0;
        
        virtual /* [helpcontext][propget][id] */ HRESULT STDMETHODCALLTYPE get_DefinedSize( 
            /* [retval][out] */ long __RPC_FAR *pl) = 0;
        
        virtual /* [helpcontext][propget][id] */ HRESULT STDMETHODCALLTYPE get_Name( 
            /* [retval][out] */ BSTR __RPC_FAR *pbstr) = 0;
        
        virtual /* [helpcontext][propget][id] */ HRESULT STDMETHODCALLTYPE get_Type( 
            /* [retval][out] */ DataTypeEnum __RPC_FAR *pDataType) = 0;
        
        virtual /* [helpcontext][propget][id] */ HRESULT STDMETHODCALLTYPE get_Value( 
            /* [retval][out] */ VARIANT __RPC_FAR *pvar) = 0;
        
        virtual /* [helpcontext][propput][id] */ HRESULT STDMETHODCALLTYPE put_Value( 
            /* [in] */ VARIANT Val) = 0;
        
        virtual /* [helpcontext][propget][id] */ HRESULT STDMETHODCALLTYPE get_Precision( 
            /* [retval][out] */ BYTE __RPC_FAR *pbPrecision) = 0;
        
        virtual /* [helpcontext][propget][id] */ HRESULT STDMETHODCALLTYPE get_NumericScale( 
            /* [retval][out] */ BYTE __RPC_FAR *pbNumericScale) = 0;
        
        virtual /* [helpcontext][id] */ HRESULT STDMETHODCALLTYPE AppendChunk( 
            /* [in] */ VARIANT Data) = 0;
        
        virtual /* [helpcontext][id] */ HRESULT STDMETHODCALLTYPE GetChunk( 
            /* [in] */ long Length,
            /* [retval][out] */ VARIANT __RPC_FAR *pvar) = 0;
        
        virtual /* [helpcontext][propget][id] */ HRESULT STDMETHODCALLTYPE get_OriginalValue( 
            /* [retval][out] */ VARIANT __RPC_FAR *pvar) = 0;
        
        virtual /* [helpcontext][propget][id] */ HRESULT STDMETHODCALLTYPE get_UnderlyingValue( 
            /* [retval][out] */ VARIANT __RPC_FAR *pvar) = 0;
        
        virtual /* [propget][id] */ HRESULT STDMETHODCALLTYPE get_DataFormat( 
            /* [retval][out] */ IUnknown __RPC_FAR *__RPC_FAR *ppiDF) = 0;
        
        virtual /* [propputref][id] */ HRESULT STDMETHODCALLTYPE putref_DataFormat( 
            /* [in] */ IUnknown __RPC_FAR *piDF) = 0;
        
        virtual /* [helpcontext][propput][id] */ HRESULT STDMETHODCALLTYPE put_Precision( 
            /* [in] */ BYTE bPrecision) = 0;
        
        virtual /* [helpcontext][propput][id] */ HRESULT STDMETHODCALLTYPE put_NumericScale( 
            /* [in] */ BYTE bScale) = 0;
        
        virtual /* [helpcontext][propput][id] */ HRESULT STDMETHODCALLTYPE put_Type( 
            /* [in] */ DataTypeEnum DataType) = 0;
        
        virtual /* [helpcontext][propput][id] */ HRESULT STDMETHODCALLTYPE put_DefinedSize( 
            /* [in] */ long lSize) = 0;
        
        virtual /* [helpcontext][propput][id] */ HRESULT STDMETHODCALLTYPE put_Attributes( 
            /* [in] */ long lAttributes) = 0;
        
    };
    
#else 	/* C style interface */
    typedef struct Field20Vtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            Field20 __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            Field20 __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            Field20 __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            Field20 __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            Field20 __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            Field20 __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            Field20 __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        /* [helpcontext][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Properties )( 
            Field20 __RPC_FAR * This,
            /* [retval][out] */ ADOProperties __RPC_FAR *__RPC_FAR *ppvObject);
        
        /* [helpcontext][propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_ActualSize )( 
            Field20 __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *pl);
        
        /* [helpcontext][propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Attributes )( 
            Field20 __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *pl);
        
        /* [helpcontext][propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_DefinedSize )( 
            Field20 __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *pl);
        
        /* [helpcontext][propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Name )( 
            Field20 __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pbstr);
        
        /* [helpcontext][propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Type )( 
            Field20 __RPC_FAR * This,
            /* [retval][out] */ DataTypeEnum __RPC_FAR *pDataType);
        
        /* [helpcontext][propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Value )( 
            Field20 __RPC_FAR * This,
            /* [retval][out] */ VARIANT __RPC_FAR *pvar);
        
        /* [helpcontext][propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Value )( 
            Field20 __RPC_FAR * This,
            /* [in] */ VARIANT Val);
        
        /* [helpcontext][propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Precision )( 
            Field20 __RPC_FAR * This,
            /* [retval][out] */ BYTE __RPC_FAR *pbPrecision);
        
        /* [helpcontext][propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_NumericScale )( 
            Field20 __RPC_FAR * This,
            /* [retval][out] */ BYTE __RPC_FAR *pbNumericScale);
        
        /* [helpcontext][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *AppendChunk )( 
            Field20 __RPC_FAR * This,
            /* [in] */ VARIANT Data);
        
        /* [helpcontext][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetChunk )( 
            Field20 __RPC_FAR * This,
            /* [in] */ long Length,
            /* [retval][out] */ VARIANT __RPC_FAR *pvar);
        
        /* [helpcontext][propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_OriginalValue )( 
            Field20 __RPC_FAR * This,
            /* [retval][out] */ VARIANT __RPC_FAR *pvar);
        
        /* [helpcontext][propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_UnderlyingValue )( 
            Field20 __RPC_FAR * This,
            /* [retval][out] */ VARIANT __RPC_FAR *pvar);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_DataFormat )( 
            Field20 __RPC_FAR * This,
            /* [retval][out] */ IUnknown __RPC_FAR *__RPC_FAR *ppiDF);
        
        /* [propputref][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *putref_DataFormat )( 
            Field20 __RPC_FAR * This,
            /* [in] */ IUnknown __RPC_FAR *piDF);
        
        /* [helpcontext][propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Precision )( 
            Field20 __RPC_FAR * This,
            /* [in] */ BYTE bPrecision);
        
        /* [helpcontext][propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_NumericScale )( 
            Field20 __RPC_FAR * This,
            /* [in] */ BYTE bScale);
        
        /* [helpcontext][propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Type )( 
            Field20 __RPC_FAR * This,
            /* [in] */ DataTypeEnum DataType);
        
        /* [helpcontext][propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_DefinedSize )( 
            Field20 __RPC_FAR * This,
            /* [in] */ long lSize);
        
        /* [helpcontext][propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Attributes )( 
            Field20 __RPC_FAR * This,
            /* [in] */ long lAttributes);
        
        END_INTERFACE
    } Field20Vtbl;
    interface Field20
    {
        CONST_VTBL struct Field20Vtbl __RPC_FAR *lpVtbl;
    };
    
#ifdef COBJMACROS
#define Field20_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)
#define Field20_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)
#define Field20_Release(This)	\
    (This)->lpVtbl -> Release(This)
#define Field20_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)
#define Field20_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)
#define Field20_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)
#define Field20_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)
#define Field20_get_Properties(This,ppvObject)	\
    (This)->lpVtbl -> get_Properties(This,ppvObject)
#define Field20_get_ActualSize(This,pl)	\
    (This)->lpVtbl -> get_ActualSize(This,pl)
#define Field20_get_Attributes(This,pl)	\
    (This)->lpVtbl -> get_Attributes(This,pl)
#define Field20_get_DefinedSize(This,pl)	\
    (This)->lpVtbl -> get_DefinedSize(This,pl)
#define Field20_get_Name(This,pbstr)	\
    (This)->lpVtbl -> get_Name(This,pbstr)
#define Field20_get_Type(This,pDataType)	\
    (This)->lpVtbl -> get_Type(This,pDataType)
#define Field20_get_Value(This,pvar)	\
    (This)->lpVtbl -> get_Value(This,pvar)
#define Field20_put_Value(This,Val)	\
    (This)->lpVtbl -> put_Value(This,Val)
#define Field20_get_Precision(This,pbPrecision)	\
    (This)->lpVtbl -> get_Precision(This,pbPrecision)
#define Field20_get_NumericScale(This,pbNumericScale)	\
    (This)->lpVtbl -> get_NumericScale(This,pbNumericScale)
#define Field20_AppendChunk(This,Data)	\
    (This)->lpVtbl -> AppendChunk(This,Data)
#define Field20_GetChunk(This,Length,pvar)	\
    (This)->lpVtbl -> GetChunk(This,Length,pvar)
#define Field20_get_OriginalValue(This,pvar)	\
    (This)->lpVtbl -> get_OriginalValue(This,pvar)
#define Field20_get_UnderlyingValue(This,pvar)	\
    (This)->lpVtbl -> get_UnderlyingValue(This,pvar)
#define Field20_get_DataFormat(This,ppiDF)	\
    (This)->lpVtbl -> get_DataFormat(This,ppiDF)
#define Field20_putref_DataFormat(This,piDF)	\
    (This)->lpVtbl -> putref_DataFormat(This,piDF)
#define Field20_put_Precision(This,bPrecision)	\
    (This)->lpVtbl -> put_Precision(This,bPrecision)
#define Field20_put_NumericScale(This,bScale)	\
    (This)->lpVtbl -> put_NumericScale(This,bScale)
#define Field20_put_Type(This,DataType)	\
    (This)->lpVtbl -> put_Type(This,DataType)
#define Field20_put_DefinedSize(This,lSize)	\
    (This)->lpVtbl -> put_DefinedSize(This,lSize)
#define Field20_put_Attributes(This,lAttributes)	\
    (This)->lpVtbl -> put_Attributes(This,lAttributes)
#endif /* COBJMACROS */
#endif 	/* C style interface */
/* [helpcontext][propget][id] */ HRESULT STDMETHODCALLTYPE Field20_get_ActualSize_Proxy( 
    Field20 __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *pl);
void __RPC_STUB Field20_get_ActualSize_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);
/* [helpcontext][propget][id] */ HRESULT STDMETHODCALLTYPE Field20_get_Attributes_Proxy( 
    Field20 __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *pl);
void __RPC_STUB Field20_get_Attributes_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);
/* [helpcontext][propget][id] */ HRESULT STDMETHODCALLTYPE Field20_get_DefinedSize_Proxy( 
    Field20 __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *pl);
void __RPC_STUB Field20_get_DefinedSize_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);
/* [helpcontext][propget][id] */ HRESULT STDMETHODCALLTYPE Field20_get_Name_Proxy( 
    Field20 __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pbstr);
void __RPC_STUB Field20_get_Name_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);
/* [helpcontext][propget][id] */ HRESULT STDMETHODCALLTYPE Field20_get_Type_Proxy( 
    Field20 __RPC_FAR * This,
    /* [retval][out] */ DataTypeEnum __RPC_FAR *pDataType);
void __RPC_STUB Field20_get_Type_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);
/* [helpcontext][propget][id] */ HRESULT STDMETHODCALLTYPE Field20_get_Value_Proxy( 
    Field20 __RPC_FAR * This,
    /* [retval][out] */ VARIANT __RPC_FAR *pvar);
void __RPC_STUB Field20_get_Value_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);
/* [helpcontext][propput][id] */ HRESULT STDMETHODCALLTYPE Field20_put_Value_Proxy( 
    Field20 __RPC_FAR * This,
    /* [in] */ VARIANT Val);
void __RPC_STUB Field20_put_Value_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);
/* [helpcontext][propget][id] */ HRESULT STDMETHODCALLTYPE Field20_get_Precision_Proxy( 
    Field20 __RPC_FAR * This,
    /* [retval][out] */ BYTE __RPC_FAR *pbPrecision);
void __RPC_STUB Field20_get_Precision_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);
/* [helpcontext][propget][id] */ HRESULT STDMETHODCALLTYPE Field20_get_NumericScale_Proxy( 
    Field20 __RPC_FAR * This,
    /* [retval][out] */ BYTE __RPC_FAR *pbNumericScale);
void __RPC_STUB Field20_get_NumericScale_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);
/* [helpcontext][id] */ HRESULT STDMETHODCALLTYPE Field20_AppendChunk_Proxy( 
    Field20 __RPC_FAR * This,
    /* [in] */ VARIANT Data);
void __RPC_STUB Field20_AppendChunk_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);
/* [helpcontext][id] */ HRESULT STDMETHODCALLTYPE Field20_GetChunk_Proxy( 
    Field20 __RPC_FAR * This,
    /* [in] */ long Length,
    /* [retval][out] */ VARIANT __RPC_FAR *pvar);
void __RPC_STUB Field20_GetChunk_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);
/* [helpcontext][propget][id] */ HRESULT STDMETHODCALLTYPE Field20_get_OriginalValue_Proxy( 
    Field20 __RPC_FAR * This,
    /* [retval][out] */ VARIANT __RPC_FAR *pvar);
void __RPC_STUB Field20_get_OriginalValue_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);
/* [helpcontext][propget][id] */ HRESULT STDMETHODCALLTYPE Field20_get_UnderlyingValue_Proxy( 
    Field20 __RPC_FAR * This,
    /* [retval][out] */ VARIANT __RPC_FAR *pvar);
void __RPC_STUB Field20_get_UnderlyingValue_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);
/* [propget][id] */ HRESULT STDMETHODCALLTYPE Field20_get_DataFormat_Proxy( 
    Field20 __RPC_FAR * This,
    /* [retval][out] */ IUnknown __RPC_FAR *__RPC_FAR *ppiDF);
void __RPC_STUB Field20_get_DataFormat_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);
/* [propputref][id] */ HRESULT STDMETHODCALLTYPE Field20_putref_DataFormat_Proxy( 
    Field20 __RPC_FAR * This,
    /* [in] */ IUnknown __RPC_FAR *piDF);
void __RPC_STUB Field20_putref_DataFormat_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);
/* [helpcontext][propput][id] */ HRESULT STDMETHODCALLTYPE Field20_put_Precision_Proxy( 
    Field20 __RPC_FAR * This,
    /* [in] */ BYTE bPrecision);
void __RPC_STUB Field20_put_Precision_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);
/* [helpcontext][propput][id] */ HRESULT STDMETHODCALLTYPE Field20_put_NumericScale_Proxy( 
    Field20 __RPC_FAR * This,
    /* [in] */ BYTE bScale);
void __RPC_STUB Field20_put_NumericScale_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);
/* [helpcontext][propput][id] */ HRESULT STDMETHODCALLTYPE Field20_put_Type_Proxy( 
    Field20 __RPC_FAR * This,
    /* [in] */ DataTypeEnum DataType);
void __RPC_STUB Field20_put_Type_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);
/* [helpcontext][propput][id] */ HRESULT STDMETHODCALLTYPE Field20_put_DefinedSize_Proxy( 
    Field20 __RPC_FAR * This,
    /* [in] */ long lSize);
void __RPC_STUB Field20_put_DefinedSize_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);
/* [helpcontext][propput][id] */ HRESULT STDMETHODCALLTYPE Field20_put_Attributes_Proxy( 
    Field20 __RPC_FAR * This,
    /* [in] */ long lAttributes);
void __RPC_STUB Field20_put_Attributes_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);
#endif 	/* __Field20_INTERFACE_DEFINED__ */
#ifndef __Field_INTERFACE_DEFINED__
#define __Field_INTERFACE_DEFINED__
/* interface ADOField */
/* [object][helpcontext][uuid][nonextensible][dual] */ 
EXTERN_C const IID IID_Field;
#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("00000569-0000-0010-8000-00AA006D2EA4")
    ADOField : public Field20
    {
    public:
        virtual /* [helpcontext][propget][id] */ HRESULT STDMETHODCALLTYPE get_Status( 
            /* [retval][out] */ long __RPC_FAR *pFStatus) = 0;
        
    };
    
#else 	/* C style interface */
    typedef struct FieldVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            ADOField __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            ADOField __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            ADOField __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            ADOField __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            ADOField __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            ADOField __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            ADOField __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        /* [helpcontext][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Properties )( 
            ADOField __RPC_FAR * This,
            /* [retval][out] */ ADOProperties __RPC_FAR *__RPC_FAR *ppvObject);
        
        /* [helpcontext][propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_ActualSize )( 
            ADOField __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *pl);
        
        /* [helpcontext][propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Attributes )( 
            ADOField __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *pl);
        
        /* [helpcontext][propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_DefinedSize )( 
            ADOField __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *pl);
        
        /* [helpcontext][propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Name )( 
            ADOField __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pbstr);
        
        /* [helpcontext][propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Type )( 
            ADOField __RPC_FAR * This,
            /* [retval][out] */ DataTypeEnum __RPC_FAR *pDataType);
        
        /* [helpcontext][propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Value )( 
            ADOField __RPC_FAR * This,
            /* [retval][out] */ VARIANT __RPC_FAR *pvar);
        
        /* [helpcontext][propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Value )( 
            ADOField __RPC_FAR * This,
            /* [in] */ VARIANT Val);
        
        /* [helpcontext][propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Precision )( 
            ADOField __RPC_FAR * This,
            /* [retval][out] */ BYTE __RPC_FAR *pbPrecision);
        
        /* [helpcontext][propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_NumericScale )( 
            ADOField __RPC_FAR * This,
            /* [retval][out] */ BYTE __RPC_FAR *pbNumericScale);
        
        /* [helpcontext][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *AppendChunk )( 
            ADOField __RPC_FAR * This,
            /* [in] */ VARIANT Data);
        
        /* [helpcontext][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetChunk )( 
            ADOField __RPC_FAR * This,
            /* [in] */ long Length,
            /* [retval][out] */ VARIANT __RPC_FAR *pvar);
        
        /* [helpcontext][propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_OriginalValue )( 
            ADOField __RPC_FAR * This,
            /* [retval][out] */ VARIANT __RPC_FAR *pvar);
        
        /* [helpcontext][propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_UnderlyingValue )( 
            ADOField __RPC_FAR * This,
            /* [retval][out] */ VARIANT __RPC_FAR *pvar);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_DataFormat )( 
            ADOField __RPC_FAR * This,
            /* [retval][out] */ IUnknown __RPC_FAR *__RPC_FAR *ppiDF);
        
        /* [propputref][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *putref_DataFormat )( 
            ADOField __RPC_FAR * This,
            /* [in] */ IUnknown __RPC_FAR *piDF);
        
        /* [helpcontext][propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Precision )( 
            ADOField __RPC_FAR * This,
            /* [in] */ BYTE bPrecision);
        
        /* [helpcontext][propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_NumericScale )( 
            ADOField __RPC_FAR * This,
            /* [in] */ BYTE bScale);
        
        /* [helpcontext][propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Type )( 
            ADOField __RPC_FAR * This,
            /* [in] */ DataTypeEnum DataType);
        
        /* [helpcontext][propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_DefinedSize )( 
            ADOField __RPC_FAR * This,
            /* [in] */ long lSize);
        
        /* [helpcontext][propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Attributes )( 
            ADOField __RPC_FAR * This,
            /* [in] */ long lAttributes);
        
        /* [helpcontext][propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Status )( 
            ADOField __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *pFStatus);
        
        END_INTERFACE
    } FieldVtbl;
    interface Field
    {
        CONST_VTBL struct FieldVtbl __RPC_FAR *lpVtbl;
    };
    
#ifdef COBJMACROS
#define Field_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)
#define Field_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)
#define Field_Release(This)	\
    (This)->lpVtbl -> Release(This)
#define Field_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)
#define Field_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)
#define Field_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)
#define Field_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)
#define Field_get_Properties(This,ppvObject)	\
    (This)->lpVtbl -> get_Properties(This,ppvObject)
#define Field_get_ActualSize(This,pl)	\
    (This)->lpVtbl -> get_ActualSize(This,pl)
#define Field_get_Attributes(This,pl)	\
    (This)->lpVtbl -> get_Attributes(This,pl)
#define Field_get_DefinedSize(This,pl)	\
    (This)->lpVtbl -> get_DefinedSize(This,pl)
#define Field_get_Name(This,pbstr)	\
    (This)->lpVtbl -> get_Name(This,pbstr)
#define Field_get_Type(This,pDataType)	\
    (This)->lpVtbl -> get_Type(This,pDataType)
#define Field_get_Value(This,pvar)	\
    (This)->lpVtbl -> get_Value(This,pvar)
#define Field_put_Value(This,Val)	\
    (This)->lpVtbl -> put_Value(This,Val)
#define Field_get_Precision(This,pbPrecision)	\
    (This)->lpVtbl -> get_Precision(This,pbPrecision)
#define Field_get_NumericScale(This,pbNumericScale)	\
    (This)->lpVtbl -> get_NumericScale(This,pbNumericScale)
#define Field_AppendChunk(This,Data)	\
    (This)->lpVtbl -> AppendChunk(This,Data)
#define Field_GetChunk(This,Length,pvar)	\
    (This)->lpVtbl -> GetChunk(This,Length,pvar)
#define Field_get_OriginalValue(This,pvar)	\
    (This)->lpVtbl -> get_OriginalValue(This,pvar)
#define Field_get_UnderlyingValue(This,pvar)	\
    (This)->lpVtbl -> get_UnderlyingValue(This,pvar)
#define Field_get_DataFormat(This,ppiDF)	\
    (This)->lpVtbl -> get_DataFormat(This,ppiDF)
#define Field_putref_DataFormat(This,piDF)	\
    (This)->lpVtbl -> putref_DataFormat(This,piDF)
#define Field_put_Precision(This,bPrecision)	\
    (This)->lpVtbl -> put_Precision(This,bPrecision)
#define Field_put_NumericScale(This,bScale)	\
    (This)->lpVtbl -> put_NumericScale(This,bScale)
#define Field_put_Type(This,DataType)	\
    (This)->lpVtbl -> put_Type(This,DataType)
#define Field_put_DefinedSize(This,lSize)	\
    (This)->lpVtbl -> put_DefinedSize(This,lSize)
#define Field_put_Attributes(This,lAttributes)	\
    (This)->lpVtbl -> put_Attributes(This,lAttributes)
#define Field_get_Status(This,pFStatus)	\
    (This)->lpVtbl -> get_Status(This,pFStatus)
#endif /* COBJMACROS */
#endif 	/* C style interface */
/* [helpcontext][propget][id] */ HRESULT STDMETHODCALLTYPE Field_get_Status_Proxy( 
    ADOField __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *pFStatus);
void __RPC_STUB Field_get_Status_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);
#endif 	/* __Field_INTERFACE_DEFINED__ */
#ifndef __Fields15_INTERFACE_DEFINED__
#define __Fields15_INTERFACE_DEFINED__
/* interface Fields15 */
/* [object][helpcontext][uuid][hidden][nonextensible][dual] */ 
EXTERN_C const IID IID_Fields15;
#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("00000506-0000-0010-8000-00AA006D2EA4")
    Fields15 : public _ADOCollection
    {
    public:
        virtual /* [helpcontext][propget][id] */ HRESULT STDMETHODCALLTYPE get_Item( 
            /* [in] */ VARIANT Index,
            /* [retval][out] */ ADOField __RPC_FAR *__RPC_FAR *ppvObject) = 0;
        
    };
    
#else 	/* C style interface */
    typedef struct Fields15Vtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            Fields15 __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            Fields15 __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            Fields15 __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            Fields15 __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            Fields15 __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            Fields15 __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            Fields15 __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        /* [id][helpcontext][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Count )( 
            Fields15 __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *c);
        
        /* [id][restricted] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *_NewEnum )( 
            Fields15 __RPC_FAR * This,
            /* [retval][out] */ IUnknown __RPC_FAR *__RPC_FAR *ppvObject);
        
        /* [id][helpcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Refresh )( 
            Fields15 __RPC_FAR * This);
        
        /* [helpcontext][propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Item )( 
            Fields15 __RPC_FAR * This,
            /* [in] */ VARIANT Index,
            /* [retval][out] */ ADOField __RPC_FAR *__RPC_FAR *ppvObject);
        
        END_INTERFACE
    } Fields15Vtbl;
    interface Fields15
    {
        CONST_VTBL struct Fields15Vtbl __RPC_FAR *lpVtbl;
    };
    
#ifdef COBJMACROS
#define Fields15_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)
#define Fields15_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)
#define Fields15_Release(This)	\
    (This)->lpVtbl -> Release(This)
#define Fields15_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)
#define Fields15_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)
#define Fields15_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)
#define Fields15_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)
#define Fields15_get_Count(This,c)	\
    (This)->lpVtbl -> get_Count(This,c)
#define Fields15__NewEnum(This,ppvObject)	\
    (This)->lpVtbl -> _NewEnum(This,ppvObject)
#define Fields15_Refresh(This)	\
    (This)->lpVtbl -> Refresh(This)
#define Fields15_get_Item(This,Index,ppvObject)	\
    (This)->lpVtbl -> get_Item(This,Index,ppvObject)
#endif /* COBJMACROS */
#endif 	/* C style interface */
/* [helpcontext][propget][id] */ HRESULT STDMETHODCALLTYPE Fields15_get_Item_Proxy( 
    Fields15 __RPC_FAR * This,
    /* [in] */ VARIANT Index,
    /* [retval][out] */ ADOField __RPC_FAR *__RPC_FAR *ppvObject);
void __RPC_STUB Fields15_get_Item_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);
#endif 	/* __Fields15_INTERFACE_DEFINED__ */
#ifndef __Fields20_INTERFACE_DEFINED__
#define __Fields20_INTERFACE_DEFINED__
/* interface Fields20 */
/* [object][helpcontext][uuid][hidden][nonextensible][dual] */ 
EXTERN_C const IID IID_Fields20;
#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("0000054D-0000-0010-8000-00AA006D2EA4")
    Fields20 : public Fields15
    {
    public:
        virtual /* [hidden] */ HRESULT STDMETHODCALLTYPE _Append( 
            /* [in] */ BSTR Name,
            /* [in] */ DataTypeEnum Type,
            /* [defaultvalue][in] */ long DefinedSize = 0,
            /* [defaultvalue][in] */ FieldAttributeEnum Attrib = adFldUnspecified) = 0;
        
        virtual /* [helpcontext][id] */ HRESULT STDMETHODCALLTYPE Delete( 
            /* [in] */ VARIANT Index) = 0;
        
    };
    
#else 	/* C style interface */
    typedef struct Fields20Vtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            Fields20 __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            Fields20 __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            Fields20 __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            Fields20 __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            Fields20 __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            Fields20 __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            Fields20 __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        /* [id][helpcontext][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Count )( 
            Fields20 __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *c);
        
        /* [id][restricted] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *_NewEnum )( 
            Fields20 __RPC_FAR * This,
            /* [retval][out] */ IUnknown __RPC_FAR *__RPC_FAR *ppvObject);
        
        /* [id][helpcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Refresh )( 
            Fields20 __RPC_FAR * This);
        
        /* [helpcontext][propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Item )( 
            Fields20 __RPC_FAR * This,
            /* [in] */ VARIANT Index,
            /* [retval][out] */ ADOField __RPC_FAR *__RPC_FAR *ppvObject);
        
        /* [hidden] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *_Append )( 
            Fields20 __RPC_FAR * This,
            /* [in] */ BSTR Name,
            /* [in] */ DataTypeEnum Type,
            /* [defaultvalue][in] */ long DefinedSize,
            /* [defaultvalue][in] */ FieldAttributeEnum Attrib);
        
        /* [helpcontext][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Delete )( 
            Fields20 __RPC_FAR * This,
            /* [in] */ VARIANT Index);
        
        END_INTERFACE
    } Fields20Vtbl;
    interface Fields20
    {
        CONST_VTBL struct Fields20Vtbl __RPC_FAR *lpVtbl;
    };
    
#ifdef COBJMACROS
#define Fields20_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)
#define Fields20_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)
#define Fields20_Release(This)	\
    (This)->lpVtbl -> Release(This)
#define Fields20_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)
#define Fields20_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)
#define Fields20_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)
#define Fields20_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)
#define Fields20_get_Count(This,c)	\
    (This)->lpVtbl -> get_Count(This,c)
#define Fields20__NewEnum(This,ppvObject)	\
    (This)->lpVtbl -> _NewEnum(This,ppvObject)
#define Fields20_Refresh(This)	\
    (This)->lpVtbl -> Refresh(This)
#define Fields20_get_Item(This,Index,ppvObject)	\
    (This)->lpVtbl -> get_Item(This,Index,ppvObject)
#define Fields20__Append(This,Name,Type,DefinedSize,Attrib)	\
    (This)->lpVtbl -> _Append(This,Name,Type,DefinedSize,Attrib)
#define Fields20_Delete(This,Index)	\
    (This)->lpVtbl -> Delete(This,Index)
#endif /* COBJMACROS */
#endif 	/* C style interface */
/* [hidden] */ HRESULT STDMETHODCALLTYPE Fields20__Append_Proxy( 
    Fields20 __RPC_FAR * This,
    /* [in] */ BSTR Name,
    /* [in] */ DataTypeEnum Type,
    /* [defaultvalue][in] */ long DefinedSize,
    /* [defaultvalue][in] */ FieldAttributeEnum Attrib);
void __RPC_STUB Fields20__Append_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);
/* [helpcontext][id] */ HRESULT STDMETHODCALLTYPE Fields20_Delete_Proxy( 
    Fields20 __RPC_FAR * This,
    /* [in] */ VARIANT Index);
void __RPC_STUB Fields20_Delete_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);
#endif 	/* __Fields20_INTERFACE_DEFINED__ */
#ifndef __Fields_INTERFACE_DEFINED__
#define __Fields_INTERFACE_DEFINED__
/* interface ADOFields */
/* [object][helpcontext][uuid][nonextensible][dual] */ 
EXTERN_C const IID IID_Fields;
#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("00000564-0000-0010-8000-00AA006D2EA4")
    ADOFields : public Fields20
    {
    public:
        virtual /* [helpcontext][id] */ HRESULT STDMETHODCALLTYPE Append( 
            /* [in] */ BSTR Name,
            /* [in] */ DataTypeEnum Type,
            /* [defaultvalue][in] */ long DefinedSize,
            /* [defaultvalue][in] */ FieldAttributeEnum Attrib,
            /* [optional][in] */ VARIANT FieldValue) = 0;
        
        virtual /* [helpcontext][id] */ HRESULT STDMETHODCALLTYPE Update( void) = 0;
        
        virtual /* [helpcontext][id] */ HRESULT STDMETHODCALLTYPE Resync( 
            /* [defaultvalue][in] */ ResyncEnum ResyncValues = adResyncAllValues) = 0;
        
        virtual /* [helpcontext][id] */ HRESULT STDMETHODCALLTYPE CancelUpdate( void) = 0;
        
    };
    
#else 	/* C style interface */
    typedef struct FieldsVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            ADOFields __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            ADOFields __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            ADOFields __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            ADOFields __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            ADOFields __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            ADOFields __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            ADOFields __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        /* [id][helpcontext][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Count )( 
            ADOFields __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *c);
        
        /* [id][restricted] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *_NewEnum )( 
            ADOFields __RPC_FAR * This,
            /* [retval][out] */ IUnknown __RPC_FAR *__RPC_FAR *ppvObject);
        
        /* [id][helpcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Refresh )( 
            ADOFields __RPC_FAR * This);
        
        /* [helpcontext][propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Item )( 
            ADOFields __RPC_FAR * This,
            /* [in] */ VARIANT Index,
            /* [retval][out] */ ADOField __RPC_FAR *__RPC_FAR *ppvObject);
        
        /* [hidden] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *_Append )( 
            ADOFields __RPC_FAR * This,
            /* [in] */ BSTR Name,
            /* [in] */ DataTypeEnum Type,
            /* [defaultvalue][in] */ long DefinedSize,
            /* [defaultvalue][in] */ FieldAttributeEnum Attrib);
        
        /* [helpcontext][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Delete )( 
            ADOFields __RPC_FAR * This,
            /* [in] */ VARIANT Index);
        
        /* [helpcontext][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Append )( 
            ADOFields __RPC_FAR * This,
            /* [in] */ BSTR Name,
            /* [in] */ DataTypeEnum Type,
            /* [defaultvalue][in] */ long DefinedSize,
            /* [defaultvalue][in] */ FieldAttributeEnum Attrib,
            /* [optional][in] */ VARIANT FieldValue);
        
        /* [helpcontext][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Update )( 
            ADOFields __RPC_FAR * This);
        
        /* [helpcontext][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Resync )( 
            ADOFields __RPC_FAR * This,
            /* [defaultvalue][in] */ ResyncEnum ResyncValues);
        
        /* [helpcontext][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *CancelUpdate )( 
            ADOFields __RPC_FAR * This);
        
        END_INTERFACE
    } FieldsVtbl;
    interface Fields
    {
        CONST_VTBL struct FieldsVtbl __RPC_FAR *lpVtbl;
    };
    
#ifdef COBJMACROS
#define Fields_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)
#define Fields_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)
#define Fields_Release(This)	\
    (This)->lpVtbl -> Release(This)
#define Fields_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)
#define Fields_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)
#define Fields_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)
#define Fields_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)
#define Fields_get_Count(This,c)	\
    (This)->lpVtbl -> get_Count(This,c)
#define Fields__NewEnum(This,ppvObject)	\
    (This)->lpVtbl -> _NewEnum(This,ppvObject)
#define Fields_Refresh(This)	\
    (This)->lpVtbl -> Refresh(This)
#define Fields_get_Item(This,Index,ppvObject)	\
    (This)->lpVtbl -> get_Item(This,Index,ppvObject)
#define Fields__Append(This,Name,Type,DefinedSize,Attrib)	\
    (This)->lpVtbl -> _Append(This,Name,Type,DefinedSize,Attrib)
#define Fields_Delete(This,Index)	\
    (This)->lpVtbl -> Delete(This,Index)
#define Fields_Append(This,Name,Type,DefinedSize,Attrib,FieldValue)	\
    (This)->lpVtbl -> Append(This,Name,Type,DefinedSize,Attrib,FieldValue)
#define Fields_Update(This)	\
    (This)->lpVtbl -> Update(This)
#define Fields_Resync(This,ResyncValues)	\
    (This)->lpVtbl -> Resync(This,ResyncValues)
#define Fields_CancelUpdate(This)	\
    (This)->lpVtbl -> CancelUpdate(This)
#endif /* COBJMACROS */
#endif 	/* C style interface */
/* [helpcontext][id] */ HRESULT STDMETHODCALLTYPE Fields_Append_Proxy( 
    ADOFields __RPC_FAR * This,
    /* [in] */ BSTR Name,
    /* [in] */ DataTypeEnum Type,
    /* [defaultvalue][in] */ long DefinedSize,
    /* [defaultvalue][in] */ FieldAttributeEnum Attrib,
    /* [optional][in] */ VARIANT FieldValue);
void __RPC_STUB Fields_Append_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);
/* [helpcontext][id] */ HRESULT STDMETHODCALLTYPE Fields_Update_Proxy( 
    ADOFields __RPC_FAR * This);
void __RPC_STUB Fields_Update_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);
/* [helpcontext][id] */ HRESULT STDMETHODCALLTYPE Fields_Resync_Proxy( 
    ADOFields __RPC_FAR * This,
    /* [defaultvalue][in] */ ResyncEnum ResyncValues);
void __RPC_STUB Fields_Resync_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);
/* [helpcontext][id] */ HRESULT STDMETHODCALLTYPE Fields_CancelUpdate_Proxy( 
    ADOFields __RPC_FAR * This);
void __RPC_STUB Fields_CancelUpdate_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);
#endif 	/* __Fields_INTERFACE_DEFINED__ */
#ifndef ___Parameter_INTERFACE_DEFINED__
#define ___Parameter_INTERFACE_DEFINED__
/* interface _ADOParameter */
/* [object][helpcontext][uuid][nonextensible][dual] */ 
EXTERN_C const IID IID__Parameter;
#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("0000050C-0000-0010-8000-00AA006D2EA4")
    _ADOParameter : public _ADO
    {
    public:
        virtual /* [helpcontext][propget][id] */ HRESULT STDMETHODCALLTYPE get_Name( 
            /* [retval][out] */ BSTR __RPC_FAR *pbstr) = 0;
        
        virtual /* [helpcontext][propput][id] */ HRESULT STDMETHODCALLTYPE put_Name( 
            /* [in] */ BSTR bstr) = 0;
        
        virtual /* [helpcontext][propget][id] */ HRESULT STDMETHODCALLTYPE get_Value( 
            /* [retval][out] */ VARIANT __RPC_FAR *pvar) = 0;
        
        virtual /* [helpcontext][propput][id] */ HRESULT STDMETHODCALLTYPE put_Value( 
            /* [in] */ VARIANT val) = 0;
        
        virtual /* [helpcontext][propget][id] */ HRESULT STDMETHODCALLTYPE get_Type( 
            /* [retval][out] */ DataTypeEnum __RPC_FAR *psDataType) = 0;
        
        virtual /* [helpcontext][propput][id] */ HRESULT STDMETHODCALLTYPE put_Type( 
            /* [in] */ DataTypeEnum sDataType) = 0;
        
        virtual /* [helpcontext][propput][id] */ HRESULT STDMETHODCALLTYPE put_Direction( 
            /* [in] */ ParameterDirectionEnum lParmDirection) = 0;
        
        virtual /* [helpcontext][propget][id] */ HRESULT STDMETHODCALLTYPE get_Direction( 
            /* [retval][out] */ ParameterDirectionEnum __RPC_FAR *plParmDirection) = 0;
        
        virtual /* [helpcontext][propput][id] */ HRESULT STDMETHODCALLTYPE put_Precision( 
            /* [in] */ BYTE bPrecision) = 0;
        
        virtual /* [helpcontext][propget][id] */ HRESULT STDMETHODCALLTYPE get_Precision( 
            /* [retval][out] */ BYTE __RPC_FAR *pbPrecision) = 0;
        
        virtual /* [helpcontext][propput][id] */ HRESULT STDMETHODCALLTYPE put_NumericScale( 
            /* [in] */ BYTE bScale) = 0;
        
        virtual /* [helpcontext][propget][id] */ HRESULT STDMETHODCALLTYPE get_NumericScale( 
            /* [retval][out] */ BYTE __RPC_FAR *pbScale) = 0;
        
        virtual /* [helpcontext][propput][id] */ HRESULT STDMETHODCALLTYPE put_Size( 
            /* [in] */ long l) = 0;
        
        virtual /* [helpcontext][propget][id] */ HRESULT STDMETHODCALLTYPE get_Size( 
            /* [retval][out] */ long __RPC_FAR *pl) = 0;
        
        virtual /* [helpcontext][id] */ HRESULT STDMETHODCALLTYPE AppendChunk( 
            /* [in] */ VARIANT Val) = 0;
        
        virtual /* [helpcontext][propget][id] */ HRESULT STDMETHODCALLTYPE get_Attributes( 
            /* [retval][out] */ LONG __RPC_FAR *plParmAttribs) = 0;
        
        virtual /* [helpcontext][propput][id] */ HRESULT STDMETHODCALLTYPE put_Attributes( 
            /* [in] */ LONG lParmAttribs) = 0;
        
    };
    
#else 	/* C style interface */
    typedef struct _ParameterVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            _ADOParameter __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            _ADOParameter __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            _ADOParameter __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            _ADOParameter __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            _ADOParameter __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            _ADOParameter __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            _ADOParameter __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        /* [helpcontext][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Properties )( 
            _ADOParameter __RPC_FAR * This,
            /* [retval][out] */ ADOProperties __RPC_FAR *__RPC_FAR *ppvObject);
        
        /* [helpcontext][propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Name )( 
            _ADOParameter __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pbstr);
        
        /* [helpcontext][propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Name )( 
            _ADOParameter __RPC_FAR * This,
            /* [in] */ BSTR bstr);
        
        /* [helpcontext][propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Value )( 
            _ADOParameter __RPC_FAR * This,
            /* [retval][out] */ VARIANT __RPC_FAR *pvar);
        
        /* [helpcontext][propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Value )( 
            _ADOParameter __RPC_FAR * This,
            /* [in] */ VARIANT val);
        
        /* [helpcontext][propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Type )( 
            _ADOParameter __RPC_FAR * This,
            /* [retval][out] */ DataTypeEnum __RPC_FAR *psDataType);
        
        /* [helpcontext][propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Type )( 
            _ADOParameter __RPC_FAR * This,
            /* [in] */ DataTypeEnum sDataType);
        
        /* [helpcontext][propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Direction )( 
            _ADOParameter __RPC_FAR * This,
            /* [in] */ ParameterDirectionEnum lParmDirection);
        
        /* [helpcontext][propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Direction )( 
            _ADOParameter __RPC_FAR * This,
            /* [retval][out] */ ParameterDirectionEnum __RPC_FAR *plParmDirection);
        
        /* [helpcontext][propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Precision )( 
            _ADOParameter __RPC_FAR * This,
            /* [in] */ BYTE bPrecision);
        
        /* [helpcontext][propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Precision )( 
            _ADOParameter __RPC_FAR * This,
            /* [retval][out] */ BYTE __RPC_FAR *pbPrecision);
        
        /* [helpcontext][propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_NumericScale )( 
            _ADOParameter __RPC_FAR * This,
            /* [in] */ BYTE bScale);
        
        /* [helpcontext][propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_NumericScale )( 
            _ADOParameter __RPC_FAR * This,
            /* [retval][out] */ BYTE __RPC_FAR *pbScale);
        
        /* [helpcontext][propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Size )( 
            _ADOParameter __RPC_FAR * This,
            /* [in] */ long l);
        
        /* [helpcontext][propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Size )( 
            _ADOParameter __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *pl);
        
        /* [helpcontext][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *AppendChunk )( 
            _ADOParameter __RPC_FAR * This,
            /* [in] */ VARIANT Val);
        
        /* [helpcontext][propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Attributes )( 
            _ADOParameter __RPC_FAR * This,
            /* [retval][out] */ LONG __RPC_FAR *plParmAttribs);
        
        /* [helpcontext][propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Attributes )( 
            _ADOParameter __RPC_FAR * This,
            /* [in] */ LONG lParmAttribs);
        
        END_INTERFACE
    } _ParameterVtbl;
    interface _Parameter
    {
        CONST_VTBL struct _ParameterVtbl __RPC_FAR *lpVtbl;
    };
    
#ifdef COBJMACROS
#define _Parameter_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)
#define _Parameter_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)
#define _Parameter_Release(This)	\
    (This)->lpVtbl -> Release(This)
#define _Parameter_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)
#define _Parameter_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)
#define _Parameter_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)
#define _Parameter_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)
#define _Parameter_get_Properties(This,ppvObject)	\
    (This)->lpVtbl -> get_Properties(This,ppvObject)
#define _Parameter_get_Name(This,pbstr)	\
    (This)->lpVtbl -> get_Name(This,pbstr)
#define _Parameter_put_Name(This,bstr)	\
    (This)->lpVtbl -> put_Name(This,bstr)
#define _Parameter_get_Value(This,pvar)	\
    (This)->lpVtbl -> get_Value(This,pvar)
#define _Parameter_put_Value(This,val)	\
    (This)->lpVtbl -> put_Value(This,val)
#define _Parameter_get_Type(This,psDataType)	\
    (This)->lpVtbl -> get_Type(This,psDataType)
#define _Parameter_put_Type(This,sDataType)	\
    (This)->lpVtbl -> put_Type(This,sDataType)
#define _Parameter_put_Direction(This,lParmDirection)	\
    (This)->lpVtbl -> put_Direction(This,lParmDirection)
#define _Parameter_get_Direction(This,plParmDirection)	\
    (This)->lpVtbl -> get_Direction(This,plParmDirection)
#define _Parameter_put_Precision(This,bPrecision)	\
    (This)->lpVtbl -> put_Precision(This,bPrecision)
#define _Parameter_get_Precision(This,pbPrecision)	\
    (This)->lpVtbl -> get_Precision(This,pbPrecision)
#define _Parameter_put_NumericScale(This,bScale)	\
    (This)->lpVtbl -> put_NumericScale(This,bScale)
#define _Parameter_get_NumericScale(This,pbScale)	\
    (This)->lpVtbl -> get_NumericScale(This,pbScale)
#define _Parameter_put_Size(This,l)	\
    (This)->lpVtbl -> put_Size(This,l)
#define _Parameter_get_Size(This,pl)	\
    (This)->lpVtbl -> get_Size(This,pl)
#define _Parameter_AppendChunk(This,Val)	\
    (This)->lpVtbl -> AppendChunk(This,Val)
#define _Parameter_get_Attributes(This,plParmAttribs)	\
    (This)->lpVtbl -> get_Attributes(This,plParmAttribs)
#define _Parameter_put_Attributes(This,lParmAttribs)	\
    (This)->lpVtbl -> put_Attributes(This,lParmAttribs)
#endif /* COBJMACROS */
#endif 	/* C style interface */
/* [helpcontext][propget][id] */ HRESULT STDMETHODCALLTYPE _Parameter_get_Name_Proxy( 
    _ADOParameter __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pbstr);
void __RPC_STUB _Parameter_get_Name_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);
/* [helpcontext][propput][id] */ HRESULT STDMETHODCALLTYPE _Parameter_put_Name_Proxy( 
    _ADOParameter __RPC_FAR * This,
    /* [in] */ BSTR bstr);
void __RPC_STUB _Parameter_put_Name_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);
/* [helpcontext][propget][id] */ HRESULT STDMETHODCALLTYPE _Parameter_get_Value_Proxy( 
    _ADOParameter __RPC_FAR * This,
    /* [retval][out] */ VARIANT __RPC_FAR *pvar);
void __RPC_STUB _Parameter_get_Value_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);
/* [helpcontext][propput][id] */ HRESULT STDMETHODCALLTYPE _Parameter_put_Value_Proxy( 
    _ADOParameter __RPC_FAR * This,
    /* [in] */ VARIANT val);
void __RPC_STUB _Parameter_put_Value_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);
/* [helpcontext][propget][id] */ HRESULT STDMETHODCALLTYPE _Parameter_get_Type_Proxy( 
    _ADOParameter __RPC_FAR * This,
    /* [retval][out] */ DataTypeEnum __RPC_FAR *psDataType);
void __RPC_STUB _Parameter_get_Type_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);
/* [helpcontext][propput][id] */ HRESULT STDMETHODCALLTYPE _Parameter_put_Type_Proxy( 
    _ADOParameter __RPC_FAR * This,
    /* [in] */ DataTypeEnum sDataType);
void __RPC_STUB _Parameter_put_Type_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);
/* [helpcontext][propput][id] */ HRESULT STDMETHODCALLTYPE _Parameter_put_Direction_Proxy( 
    _ADOParameter __RPC_FAR * This,
    /* [in] */ ParameterDirectionEnum lParmDirection);
void __RPC_STUB _Parameter_put_Direction_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);
/* [helpcontext][propget][id] */ HRESULT STDMETHODCALLTYPE _Parameter_get_Direction_Proxy( 
    _ADOParameter __RPC_FAR * This,
    /* [retval][out] */ ParameterDirectionEnum __RPC_FAR *plParmDirection);
void __RPC_STUB _Parameter_get_Direction_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);
/* [helpcontext][propput][id] */ HRESULT STDMETHODCALLTYPE _Parameter_put_Precision_Proxy( 
    _ADOParameter __RPC_FAR * This,
    /* [in] */ BYTE bPrecision);
void __RPC_STUB _Parameter_put_Precision_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);
/* [helpcontext][propget][id] */ HRESULT STDMETHODCALLTYPE _Parameter_get_Precision_Proxy( 
    _ADOParameter __RPC_FAR * This,
    /* [retval][out] */ BYTE __RPC_FAR *pbPrecision);
void __RPC_STUB _Parameter_get_Precision_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);
/* [helpcontext][propput][id] */ HRESULT STDMETHODCALLTYPE _Parameter_put_NumericScale_Proxy( 
    _ADOParameter __RPC_FAR * This,
    /* [in] */ BYTE bScale);
void __RPC_STUB _Parameter_put_NumericScale_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);
/* [helpcontext][propget][id] */ HRESULT STDMETHODCALLTYPE _Parameter_get_NumericScale_Proxy( 
    _ADOParameter __RPC_FAR * This,
    /* [retval][out] */ BYTE __RPC_FAR *pbScale);
void __RPC_STUB _Parameter_get_NumericScale_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);
/* [helpcontext][propput][id] */ HRESULT STDMETHODCALLTYPE _Parameter_put_Size_Proxy( 
    _ADOParameter __RPC_FAR * This,
    /* [in] */ long l);
void __RPC_STUB _Parameter_put_Size_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);
/* [helpcontext][propget][id] */ HRESULT STDMETHODCALLTYPE _Parameter_get_Size_Proxy( 
    _ADOParameter __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *pl);
void __RPC_STUB _Parameter_get_Size_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);
/* [helpcontext][id] */ HRESULT STDMETHODCALLTYPE _Parameter_AppendChunk_Proxy( 
    _ADOParameter __RPC_FAR * This,
    /* [in] */ VARIANT Val);
void __RPC_STUB _Parameter_AppendChunk_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);
/* [helpcontext][propget][id] */ HRESULT STDMETHODCALLTYPE _Parameter_get_Attributes_Proxy( 
    _ADOParameter __RPC_FAR * This,
    /* [retval][out] */ LONG __RPC_FAR *plParmAttribs);
void __RPC_STUB _Parameter_get_Attributes_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);
/* [helpcontext][propput][id] */ HRESULT STDMETHODCALLTYPE _Parameter_put_Attributes_Proxy( 
    _ADOParameter __RPC_FAR * This,
    /* [in] */ LONG lParmAttribs);
void __RPC_STUB _Parameter_put_Attributes_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);
#endif 	/* ___Parameter_INTERFACE_DEFINED__ */
EXTERN_C const CLSID CLSID_Parameter;
#ifdef __cplusplus
Parameter;
#endif
#ifndef __Parameters_INTERFACE_DEFINED__
#define __Parameters_INTERFACE_DEFINED__
/* interface ADOParameters */
/* [object][helpcontext][uuid][nonextensible][dual] */ 
EXTERN_C const IID IID_Parameters;
#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("0000050D-0000-0010-8000-00AA006D2EA4")
    ADOParameters : public _ADODynaCollection
    {
    public:
        virtual /* [helpcontext][propget][id] */ HRESULT STDMETHODCALLTYPE get_Item( 
            /* [in] */ VARIANT Index,
            /* [retval][out] */ _ADOParameter __RPC_FAR *__RPC_FAR *ppvObject) = 0;
        
    };
    
#else 	/* C style interface */
    typedef struct ParametersVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            ADOParameters __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            ADOParameters __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            ADOParameters __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            ADOParameters __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            ADOParameters __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            ADOParameters __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            ADOParameters __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        /* [id][helpcontext][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Count )( 
            ADOParameters __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *c);
        
        /* [id][restricted] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *_NewEnum )( 
            ADOParameters __RPC_FAR * This,
            /* [retval][out] */ IUnknown __RPC_FAR *__RPC_FAR *ppvObject);
        
        /* [id][helpcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Refresh )( 
            ADOParameters __RPC_FAR * This);
        
        /* [helpcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Append )( 
            ADOParameters __RPC_FAR * This,
            /* [in] */ IDispatch __RPC_FAR *Object);
        
        /* [helpcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Delete )( 
            ADOParameters __RPC_FAR * This,
            /* [in] */ VARIANT Index);
        
        /* [helpcontext][propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Item )( 
            ADOParameters __RPC_FAR * This,
            /* [in] */ VARIANT Index,
            /* [retval][out] */ _ADOParameter __RPC_FAR *__RPC_FAR *ppvObject);
        
        END_INTERFACE
    } ParametersVtbl;
    interface Parameters
    {
        CONST_VTBL struct ParametersVtbl __RPC_FAR *lpVtbl;
    };
    
#ifdef COBJMACROS
#define Parameters_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)
#define Parameters_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)
#define Parameters_Release(This)	\
    (This)->lpVtbl -> Release(This)
#define Parameters_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)
#define Parameters_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)
#define Parameters_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)
#define Parameters_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)
#define Parameters_get_Count(This,c)	\
    (This)->lpVtbl -> get_Count(This,c)
#define Parameters__NewEnum(This,ppvObject)	\
    (This)->lpVtbl -> _NewEnum(This,ppvObject)
#define Parameters_Refresh(This)	\
    (This)->lpVtbl -> Refresh(This)
#define Parameters_Append(This,Object)	\
    (This)->lpVtbl -> Append(This,Object)
#define Parameters_Delete(This,Index)	\
    (This)->lpVtbl -> Delete(This,Index)
#define Parameters_get_Item(This,Index,ppvObject)	\
    (This)->lpVtbl -> get_Item(This,Index,ppvObject)
#endif /* COBJMACROS */
#endif 	/* C style interface */
/* [helpcontext][propget][id] */ HRESULT STDMETHODCALLTYPE Parameters_get_Item_Proxy( 
    ADOParameters __RPC_FAR * This,
    /* [in] */ VARIANT Index,
    /* [retval][out] */ _ADOParameter __RPC_FAR *__RPC_FAR *ppvObject);
void __RPC_STUB Parameters_get_Item_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);
#endif 	/* __Parameters_INTERFACE_DEFINED__ */
#ifndef __Property_INTERFACE_DEFINED__
#define __Property_INTERFACE_DEFINED__
/* interface ADOProperty */
/* [object][helpcontext][uuid][nonextensible][dual] */ 
EXTERN_C const IID IID_Property;
#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("00000503-0000-0010-8000-00AA006D2EA4")
    ADOProperty : public IDispatch
    {
    public:
        virtual /* [helpcontext][id][propget] */ HRESULT STDMETHODCALLTYPE get_Value( 
            /* [retval][out] */ VARIANT __RPC_FAR *pval) = 0;
        
        virtual /* [helpcontext][id][propput] */ HRESULT STDMETHODCALLTYPE put_Value( 
            /* [in] */ VARIANT val) = 0;
        
        virtual /* [helpcontext][propget] */ HRESULT STDMETHODCALLTYPE get_Name( 
            /* [retval][out] */ BSTR __RPC_FAR *pbstr) = 0;
        
        virtual /* [helpcontext][propget] */ HRESULT STDMETHODCALLTYPE get_Type( 
            /* [retval][out] */ DataTypeEnum __RPC_FAR *ptype) = 0;
        
        virtual /* [helpcontext][propget] */ HRESULT STDMETHODCALLTYPE get_Attributes( 
            /* [retval][out] */ long __RPC_FAR *plAttributes) = 0;
        
        virtual /* [helpcontext][propput] */ HRESULT STDMETHODCALLTYPE put_Attributes( 
            /* [in] */ long lAttributes) = 0;
        
    };
    
#else 	/* C style interface */
    typedef struct PropertyVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            ADOProperty __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            ADOProperty __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            ADOProperty __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            ADOProperty __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            ADOProperty __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            ADOProperty __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            ADOProperty __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        /* [helpcontext][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Value )( 
            ADOProperty __RPC_FAR * This,
            /* [retval][out] */ VARIANT __RPC_FAR *pval);
        
        /* [helpcontext][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Value )( 
            ADOProperty __RPC_FAR * This,
            /* [in] */ VARIANT val);
        
        /* [helpcontext][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Name )( 
            ADOProperty __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pbstr);
        
        /* [helpcontext][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Type )( 
            ADOProperty __RPC_FAR * This,
            /* [retval][out] */ DataTypeEnum __RPC_FAR *ptype);
        
        /* [helpcontext][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Attributes )( 
            ADOProperty __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *plAttributes);
        
        /* [helpcontext][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Attributes )( 
            ADOProperty __RPC_FAR * This,
            /* [in] */ long lAttributes);
        
        END_INTERFACE
    } PropertyVtbl;
    interface Property
    {
        CONST_VTBL struct PropertyVtbl __RPC_FAR *lpVtbl;
    };
    
#ifdef COBJMACROS
#define Property_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)
#define Property_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)
#define Property_Release(This)	\
    (This)->lpVtbl -> Release(This)
#define Property_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)
#define Property_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)
#define Property_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)
#define Property_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)
#define Property_get_Value(This,pval)	\
    (This)->lpVtbl -> get_Value(This,pval)
#define Property_put_Value(This,val)	\
    (This)->lpVtbl -> put_Value(This,val)
#define Property_get_Name(This,pbstr)	\
    (This)->lpVtbl -> get_Name(This,pbstr)
#define Property_get_Type(This,ptype)	\
    (This)->lpVtbl -> get_Type(This,ptype)
#define Property_get_Attributes(This,plAttributes)	\
    (This)->lpVtbl -> get_Attributes(This,plAttributes)
#define Property_put_Attributes(This,lAttributes)	\
    (This)->lpVtbl -> put_Attributes(This,lAttributes)
#endif /* COBJMACROS */
#endif 	/* C style interface */
/* [helpcontext][id][propget] */ HRESULT STDMETHODCALLTYPE Property_get_Value_Proxy( 
    ADOProperty __RPC_FAR * This,
    /* [retval][out] */ VARIANT __RPC_FAR *pval);
void __RPC_STUB Property_get_Value_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);
/* [helpcontext][id][propput] */ HRESULT STDMETHODCALLTYPE Property_put_Value_Proxy( 
    ADOProperty __RPC_FAR * This,
    /* [in] */ VARIANT val);
void __RPC_STUB Property_put_Value_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);
/* [helpcontext][propget] */ HRESULT STDMETHODCALLTYPE Property_get_Name_Proxy( 
    ADOProperty __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pbstr);
void __RPC_STUB Property_get_Name_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);
/* [helpcontext][propget] */ HRESULT STDMETHODCALLTYPE Property_get_Type_Proxy( 
    ADOProperty __RPC_FAR * This,
    /* [retval][out] */ DataTypeEnum __RPC_FAR *ptype);
void __RPC_STUB Property_get_Type_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);
/* [helpcontext][propget] */ HRESULT STDMETHODCALLTYPE Property_get_Attributes_Proxy( 
    ADOProperty __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *plAttributes);
void __RPC_STUB Property_get_Attributes_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);
/* [helpcontext][propput] */ HRESULT STDMETHODCALLTYPE Property_put_Attributes_Proxy( 
    ADOProperty __RPC_FAR * This,
    /* [in] */ long lAttributes);
void __RPC_STUB Property_put_Attributes_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);
#endif 	/* __Property_INTERFACE_DEFINED__ */
#ifndef __Properties_INTERFACE_DEFINED__
#define __Properties_INTERFACE_DEFINED__
/* interface ADOProperties */
/* [object][helpcontext][uuid][nonextensible][dual] */ 
EXTERN_C const IID IID_Properties;
#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("00000504-0000-0010-8000-00AA006D2EA4")
    ADOProperties : public _ADOCollection
    {
    public:
        virtual /* [helpcontext][id][propget] */ HRESULT STDMETHODCALLTYPE get_Item( 
            /* [in] */ VARIANT Index,
            /* [retval][out] */ ADOProperty __RPC_FAR *__RPC_FAR *ppvObject) = 0;
        
    };
    
#else 	/* C style interface */
    typedef struct PropertiesVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            ADOProperties __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            ADOProperties __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            ADOProperties __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            ADOProperties __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            ADOProperties __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            ADOProperties __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            ADOProperties __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        /* [id][helpcontext][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Count )( 
            ADOProperties __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *c);
        
        /* [id][restricted] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *_NewEnum )( 
            ADOProperties __RPC_FAR * This,
            /* [retval][out] */ IUnknown __RPC_FAR *__RPC_FAR *ppvObject);
        
        /* [id][helpcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Refresh )( 
            ADOProperties __RPC_FAR * This);
        
        /* [helpcontext][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Item )( 
            ADOProperties __RPC_FAR * This,
            /* [in] */ VARIANT Index,
            /* [retval][out] */ ADOProperty __RPC_FAR *__RPC_FAR *ppvObject);
        
        END_INTERFACE
    } PropertiesVtbl;
    interface Properties
    {
        CONST_VTBL struct PropertiesVtbl __RPC_FAR *lpVtbl;
    };
    
#ifdef COBJMACROS
#define Properties_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)
#define Properties_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)
#define Properties_Release(This)	\
    (This)->lpVtbl -> Release(This)
#define Properties_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)
#define Properties_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)
#define Properties_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)
#define Properties_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)
#define Properties_get_Count(This,c)	\
    (This)->lpVtbl -> get_Count(This,c)
#define Properties__NewEnum(This,ppvObject)	\
    (This)->lpVtbl -> _NewEnum(This,ppvObject)
#define Properties_Refresh(This)	\
    (This)->lpVtbl -> Refresh(This)
#define Properties_get_Item(This,Index,ppvObject)	\
    (This)->lpVtbl -> get_Item(This,Index,ppvObject)
#endif /* COBJMACROS */
#endif 	/* C style interface */
/* [helpcontext][id][propget] */ HRESULT STDMETHODCALLTYPE Properties_get_Item_Proxy( 
    ADOProperties __RPC_FAR * This,
    /* [in] */ VARIANT Index,
    /* [retval][out] */ ADOProperty __RPC_FAR *__RPC_FAR *ppvObject);
void __RPC_STUB Properties_get_Item_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);
#endif 	/* __Properties_INTERFACE_DEFINED__ */
#endif /* __ADODB_LIBRARY_DEFINED__ */
/* Additional Prototypes for ALL interfaces */
/* end of Additional Prototypes */
#ifdef __cplusplus
}
#endif
#endif
#define ADOCommand _ADOCommand
#define ADORecordset _ADORecordset
#define ADOTransaction _ADOTransaction
#define ADOParameter _ADOParameter
#define ADOConnection _ADOConnection
#define ADOCollection _ADOCollection
#define ADODynaCollection _ADODynaCollection
#define ADORecord _ADORecord
#define ADORecField _ADORecField
#define ADOStream _ADOStream


#endif // _ADOINT_H_
