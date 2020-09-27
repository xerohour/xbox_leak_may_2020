/*
 * dbgtrace.h
 *    Definitions for async tracing routines for Host Integration Server/SNA Server
 *
 * Copyright (c) Microsoft Corporation.  All rights reserved.
 * All Rights Reserved.
 */


#ifdef __cplusplus
extern "C" {
#endif

#if !defined(_DBGTRACE_H_)
#define _DBGTRACE_H_



/* The following definitions are used to manipulate the handle coding  */
#define FILETYPES		0x0000000f
#define APIFILE			0x00000001
#define MESSAGEFILE		0x00000002
#define INTFILE			0x00000004

#define APPC_FLAG		0x00000010
#define CPIC_FLAG		0x00000020
#define LUA_FLAG		0x00000040
#define CSV_FLAG		0x00000080
#define API_FLAGS		0x000000f0

#define IsAPIFile(x)	  ( ((x & FILETYPES) & APIFILE) != 0 )
#define IsMessageFile(x)  ( ((x & FILETYPES) & MESSAGEFILE) != 0 )
#define IsAPPCTrace(x)    ( ((x & API_FLAGS) & APPC_FLAG) != 0 )
#define IsCPICTrace(x)    ( ((x & API_FLAGS) & CPIC_FLAG) != 0 )
#define IsLUATrace(x)    ( ((x & API_FLAGS) & LUA_FLAG) != 0 )
#define IsCSVTrace(x)    ( ((x & API_FLAGS) & CSV_FLAG) != 0 )


#define SetAPIFile(x)	    x  = (ULONG)APIFILE
#define SetMessageFile(x)   x  = (ULONG)MESSAGEFILE
#define SetAPPCTrace(x)     x  = (ULONG)(APIFILE|APPC_FLAG)
#define SetCPICTrace(x)     x  = (ULONG)(APIFILE|CPIC_FLAG)
#define SetLUATrace(x)      x  = (ULONG)(APIFILE|LUA_FLAG)
#define SetCSVTrace(x)      x  = (ULONG)(APIFILE|CSV_FLAG)

#define INTERNAL_FLAG		0x000000010
#define SNA3270_FLAG		0x000000020
#define LU62_FLAG			0x000000040
#define DLC_FLAG			0x000000080
#define LEVEL2_FLAG			0x000000100
#define SNA_FLAG			0x000000200
#define DPL_HEADER_FLAGS	0x000000400
#define BRIEF_LU62_FLAGS	0x000000800
#define CONNECT_INFO_FLAG   0x000001000
#define NETWORK_DS_FLAG		0x000002000
#define MESSAGE_FLAGS		0x000003ff0

#define IsInternalTrace(x)    ( ((x & MESSAGE_FLAGS) & INTERNAL_FLAG) != 0 )
#define Is3270Trace(x)		  ( ((x & MESSAGE_FLAGS) & SNA3270_FLAG) != 0 )
#define IsLU62Trace(x)		  ( ((x & MESSAGE_FLAGS) & LU62_FLAG) != 0 )
#define IsDLCTrace(x)		  ( ((x & MESSAGE_FLAGS) & DLC_FLAG) != 0 )
#define IsLevel2Trace(x)      ( ((x & MESSAGE_FLAGS) & LEVEL2_FLAG) != 0 )
#define IsSNATrace(x)		  ( ((x & MESSAGE_FLAGS) & SNA_FLAG) != 0 )
#define IsDPLHeaderTrace(x)   ( ((x & MESSAGE_FLAGS) & DPL_HEADER_FLAGS) != 0)
#define IsBriefLU62Trace(x)   ( ((x & MESSAGE_FLAGS) & BRIEF_LU62_FLAGS) != 0)
#define IsConnectInfoTrace(x) ( ((x & MESSAGE_FLAGS) & CONNECT_INFO_FLAG) != 0)
#define IsNetworkDSTrace(x)   ( ((x & MESSAGE_FLAGS) & NETWORK_DS_FLAG) != 0)


#define SetInternalTrace(x)   x  =	(ULONG)(MESSAGEFILE|INTERNAL_FLAG)
#define Set3270Trace(x)		  x  =	(ULONG)(MESSAGEFILE|SNA3270_FLAG)
#define SetLU62Trace(x)		  x  =	(ULONG)(MESSAGEFILE|LU62_FLAG)
#define SetDLCTrace(x)		  x  =	(ULONG)(MESSAGEFILE|DLC_FLAG)
#define SetLevel2Trace(x)     x  =	(ULONG)(MESSAGEFILE|LEVEL2_FLAG)
#define SetSNATrace(x)		  x  =	(ULONG)(MESSAGEFILE|SNA_FLAG)
#define SetMessageTrace(x)    x  =  (ULONG)(MESSAGEFILE|MESSAGE_FLAGS)
#define SetDPLHeaderTrace(x)  x  =  (ULONG)(MESSAGEFILE|DPL_HEADER_FLAGS)
#define SetBriefLU62Trace(x)  x  =  (ULONG)(MESSAGEFILE|BRIEF_LU62_FLAGS)
#define SetConnectInfoTrace(x)  x  =  (ULONG)(MESSAGEFILE|CONNECT_INFO_FLAG)
#define SetNetworkDSTrace(x)  x  =  (ULONG)(MESSAGEFILE|NETWORK_DS_FLAG)


/* end of the handle encoding definitions */


#if defined( DISABLE_TRACING )

#define	FLUSHASYNCTRACE			/* for _ASSERT below  */





#define FatalTrace  1 ? (void)0 : PreAsyncTrace
#define ErrorTrace  1 ? (void)0 : PreAsyncTrace
#define DebugTrace  1 ? (void)0 : PreAsyncTrace
#define StateTrace  1 ? (void)0 : PreAsyncTrace
#define FunctTrace  1 ? (void)0 : PreAsyncTrace
#define ErrorTraceX 1 ? (void)0 : PreAsyncTrace
#define DebugTraceX 1 ? (void)0 : PreAsyncTrace
#define CustomTrace	1 ? (void)0 : PreAsyncTrace
#define CMTIInternalPlanEnter	1?(void)0:PreAsyncTrace
#define CMTIInternalPlanExit	1?(void)0:PreAsyncTrace
	
#define FatalTraceEx	1 ? (void)0 : PreAsyncTraceEx
#define ErrorTraceEx	1 ? (void)0 : PreAsyncTraceEx
#define DebugTraceEx	1 ? (void)0 : PreAsyncTraceEx
#define StateTraceEx	1 ? (void)0 : PreAsyncTraceEx
#define FunctTraceEx	1 ? (void)0 : PreAsyncTraceEx
#define CustomTraceEx	1 ? (void)0 : PreAsyncTraceEx

#define	InternalMessageTrace	1 ? (void)0 : PreMessageTrace
#define	SNA3270MessageTrace		1 ? (void)0 : PreMessageTrace
#define	LU62MessageTrace		1 ? (void)0 : PreMessageTrace
#define	DLCMessageTrace			1 ? (void)0 : PreMessageTrace
#define	LEVEL2MessageTrace		1 ? (void)0 : PreMessageTrace
#define	SNAMessageTrace			1 ? (void)0 : PreMessageTrace
#define DPLHeaderMessageTrace   1 ? (void)0 : PreMessageTrace
#define BriefLU62MessageTrace   1 ? (void)0 : PreMessageTrace
#define ConnectInfoMessageTrace 1 ? (void)0 : PreMessageTrace
#define NetworkDSMessageTrace   1 ? (void)0 : PreMessageTrace
#define AllMessageTrace			1 ? (void)0 : PreMessageTrace

/* COMTI API Trace  */
#define CMTIAPIComtiProxy		1 ? (void)0 : PreAPITrace
#define CMTIAPIPipeline			1 ? (void)0 : PreAPITrace
#define CMTIAPIBlackBoard		1 ? (void)0 : PreAPITrace
#define CMTIAPIGeneralServices	1 ? (void)0 : PreAPITrace
#define CMTIAPIRepository		1 ? (void)0 : PreAPITrace
#define CMTIAPIDataTransit		1 ? (void)0 : PreAPITrace
#define CMTIAPIConvert			1 ? (void)0 : PreAPITrace
#define CMTIAPITransport		1 ? (void)0 : PreAPITrace
#define CMTIAPIRegistrar		1 ? (void)0 : PreAPITrace
#define CMTIAPIScripting		1 ? (void)0 : PreAPITrace
#define CMTIAPISessMgr			1 ? (void)0 : PreAPITrace
#define CMTIAPIDataLayout		1 ? (void)0 : PreAPITrace
#define CMTIAPITerminalService	1 ? (void)0 : PreAPITrace


#define	APPCAPITrace		1 ? (void)0 : PreAPITrace
#define	CPICAPITrace		1 ? (void)0 : PreAPITrace
#define	LUAAPITrace			1 ? (void)0 : PreAPITrace
#define	CSVAPITrace			1 ? (void)0 : PreAPITrace


#define OLEDBAPITrace		1 ? (void)0 : PreAPITrace
#define ODBCAPITrace		1 ? (void)0 : PreAPITrace
#define NetworkAPITrace		1 ? (void)0 : PreAPITrace


#define MessageTrace( lParam, pbData, cbData )
#define BinaryTrace( lParam, pbData, cbData )
#define UserTrace( lParam, dwUserType, pbData, cbData )

#define BinaryInternalTrace( lParam, pbData, cbData )
#define Binary3270Trace( lParam, pbData, cbData )
#define BinaryLU62Trace( lParam, pbData, cbData )
#define BinaryDLCTrace( lParam, pbData, cbData )
#define BinaryLevel2Trace( lParam, pbData, cbData )
#define BinarySNATrace( lParam, pbData, cbData )
#define BinaryAllMsgTrace(lParam, pbData, cbData )

#define BinaryAPPCTrace( lParam, pbData, cbData )
#define BinaryCPICTrace( lParam, pbData, cbData )
#define BinaryLUATrace( lParam, pbData, cbData )
#define BinaryCSVTrace( lParam, pbData, cbData )



#define TraceFunctEnter( sz )
#define TraceFunctLeave()

#define InitAsyncTraceEx(X)

__inline int PreAsyncTrace( LONG_PTR lParam, LPCSTR szFormat, ... )
{
        return( 1);
}

__inline int PreAsyncTraceEx( LPCSTR szFormat, ... )
{
        return( 1);
}


#define TraceQuietEnter( sz )

#else /* NOTRACE */

#define	FLUSHASYNCTRACE		FlushAsyncTrace(), 	/* for _ASSERT below */

#define FatalTrace  !(__dwEnabledTraces & FATAL_TRACE_MASK) ?   \
                    (void)0 :                                   \
                    SetAsyncTraceParams( THIS_FILE, __LINE__, ___pszFunctionName, FATAL_TRACE_MASK ) &&     \
                    PreAsyncTrace

#define ErrorTrace  !(__dwEnabledTraces & ERROR_TRACE_MASK) ?   \
                    (void)0 :                                   \
                    SetAsyncTraceParams( THIS_FILE, __LINE__, ___pszFunctionName, ERROR_TRACE_MASK ) &&     \
                    PreAsyncTrace

#define DebugTrace  !(__dwEnabledTraces & DEBUG_TRACE_MASK) ?   \
                    (void)0 :                                   \
                    SetAsyncTraceParams( THIS_FILE, __LINE__, ___pszFunctionName, DEBUG_TRACE_MASK ) &&     \
                    PreAsyncTrace

#define StateTrace  !(__dwEnabledTraces & STATE_TRACE_MASK) ?   \
                    (void)0 :                                   \
                    SetAsyncTraceParams( THIS_FILE, __LINE__, ___pszFunctionName, STATE_TRACE_MASK ) &&     \
                    PreAsyncTrace

#define FunctTrace  !(__dwEnabledTraces & FUNCT_TRACE_MASK) ?   \
                    (void)0 :                                   \
                    SetAsyncTraceParams( THIS_FILE, __LINE__, ___pszFunctionName, FUNCT_TRACE_MASK ) &&     \
                    PreAsyncTrace
#define CustomTrace  !(__dwEnabledTraces & CUSTOM_TRACE_MASK) ?   \
                    (void)0 :                                   \
                    SetAsyncTraceParams( THIS_FILE, __LINE__, ___pszFunctionName, CUSTOM_TRACE_MASK ) &&     \
                    PreAsyncTrace

#define CMTIInternalPlanEnter !(__dwEnabledTraces & PLAN_ENTER_EXIT_MASK) ?   \
	                (void)0 :                                   \
                    SetAsyncTraceParams( THIS_FILE, __LINE__, ___pszFunctionName, CUSTOM_TRACE_MASK ) &&     \
                    PreAsyncTrace

#define CMTIInternalPlanExit !(__dwEnabledTraces & PLAN_ENTER_EXIT_MASK) ?   \
	                (void)0 :                                   \
                    SetAsyncTraceParams( THIS_FILE, __LINE__, ___pszFunctionName, CUSTOM_TRACE_MASK ) &&     \
                    PreAsyncTrace

/*
 * Support for unspecified function names
 */

#define ErrorTraceX  !(__dwEnabledTraces & ERROR_TRACE_MASK) ?   \
                    (void)0 :                                   \
                    SetAsyncTraceParams( THIS_FILE, __LINE__, "Fn", ERROR_TRACE_MASK ) &&     \
                    PreAsyncTrace

#define DebugTraceX  !(__dwEnabledTraces & DEBUG_TRACE_MASK) ?   \
                    (void)0 :                                   \
                    SetAsyncTraceParams( THIS_FILE, __LINE__, "Fn", DEBUG_TRACE_MASK ) &&     \
                    PreAsyncTrace


/* Trace Macros that do not require the first parameter, i.e user data  */

#define FatalTraceEx		!(__dwEnabledTraces & FATAL_TRACE_MASK) ?   \
							(void)0 :                                   \
							SetAsyncTraceParams( THIS_FILE, __LINE__, ___pszFunctionName, FATAL_TRACE_MASK ) &&     \
							PreAsyncTraceEx

#define ErrorTraceEx		!(__dwEnabledTraces & ERROR_TRACE_MASK) ?   \
							(void)0 :                                   \
							SetAsyncTraceParams( THIS_FILE, __LINE__, ___pszFunctionName, ERROR_TRACE_MASK ) &&     \
							PreAsyncTraceEx

#define DebugTraceEx		!(__dwEnabledTraces & DEBUG_TRACE_MASK) ?   \
							(void)0 :                                   \
							SetAsyncTraceParams( THIS_FILE, __LINE__, ___pszFunctionName, DEBUG_TRACE_MASK ) &&     \
							PreAsyncTraceEx

#define StateTraceEx		!(__dwEnabledTraces & STATE_TRACE_MASK) ?   \
							(void)0 :                                   \
							SetAsyncTraceParams( THIS_FILE, __LINE__, ___pszFunctionName, STATE_TRACE_MASK ) &&     \
							PreAsyncTraceEx

#define FunctTraceEx		!(__dwEnabledTraces & FUNCT_TRACE_MASK) ?   \
							(void)0 :                                   \
							SetAsyncTraceParams( THIS_FILE, __LINE__, ___pszFunctionName, FUNCT_TRACE_MASK ) &&     \
							PreAsyncTraceEx
#define CustomTraceEx		!(__dwEnabledTraces & CUSTOM_TRACE_MASK) ?   \
							(void)0 :                                   \
							SetAsyncTraceParams( THIS_FILE, __LINE__, ___pszFunctionName, CUSTOM_TRACE_MASK ) &&     \
							PreAsyncTraceEx



/*
 *	Support for SNA Server Message Trace 
 */



#define InternalMessageTrace	!(__dwMessageTraces & INTERNAL_MESSAGE_MASK) ?   \
								(void)0 :                                   \
								SetMessageTraceParams( THIS_FILE, __LINE__, ___pszFunctionName, INTERNAL_MESSAGE_MASK ) &&     \
								PreMessageTrace

#define SNA3270MessageTrace		!(__dwMessageTraces & SNA3270_MESSAGE_MASK) ?   \
								(void)0 :                                   \
								SetMessageTraceParams( THIS_FILE, __LINE__, ___pszFunctionName, SNA3270_MESSAGE_MASK ) &&     \
								PreMessageTrace

#define LU62MessageTrace		!(__dwMessageTraces & LU62_MESSAGE_MASK) ?   \
								(void)0 :                                   \
								SetMessageTraceParams( THIS_FILE, __LINE__, ___pszFunctionName, LU62_MESSAGE_MASK ) &&     \
								PreMessageTrace



#define DLCMessageTrace		    !(__dwMessageTraces & DLC_MESSAGE_MASK) ?   \
			                    (void)0 :                                   \
						        SetMessageTraceParams( THIS_FILE, __LINE__, ___pszFunctionName, DLC_MESSAGE_MASK ) &&     \
								PreMessageTrace

#define LEVEL2MessageTrace	    !(__dwMessageTraces & LEVEL2_MESSAGE_MASK) ?   \
								(void)0 :                                   \
								SetMessageTraceParams( THIS_FILE, __LINE__, ___pszFunctionName, LEVEL2_MESSAGE_MASK ) &&     \
								PreMessageTrace

#define SNAMessageTrace			!(__dwMessageTraces & SNA_MESSAGE_MASK) ?   \
								(void)0 :                                   \
								SetMessageTraceParams( THIS_FILE, __LINE__, ___pszFunctionName, SNA_MESSAGE_MASK ) &&     \
								PreMessageTrace

#define DPLHeaderMessageTrace   !(__dwMessageTraces & DPL_HEADER_MESSAGE_MASK) ? \
								(void)0:										 \
								SetMessageTraceParams( THIS_FILE, __LINE__, ___pszFunctionName, DPL_HEADER_MESSAGE_MASK) && \
								PreMessageTrace

#define BriefLU62MessageTrace   !(__dwMessageTraces & BRIEF_LU62_MESSAGE_MASK)? \
								(void)0:										\
								SetMessageTraceParams( THIS_FILE, __LINE__, ___pszFunctionName, BRIEF_LU62_MESSAGE_MASK) && \
								PreMessageTrace

#define ConnectInfoMessageTrace !(__dwMessageTraces & CONNECTION_INFO_MASK)? \
								(void)0:										\
								SetMessageTraceParams( THIS_FILE, __LINE__, ___pszFunctionName, CONNECTION_INFO_MASK) && \
								PreMessageTrace

#define NetworkDSMessageTrace   !(__dwMessageTraces & NETWORK_DS_MASK)? \
								(void)0:										\
								SetMessageTraceParams( THIS_FILE, __LINE__, ___pszFunctionName, NETWORK_DS_MASK) && \
								PreMessageTrace





#define AllMessageTrace		SetMessageTraceParams( THIS_FILE, __LINE__, ___pszFunctionName, SNA_MESSAGE_MASK ) &&     \
								PreMessageTrace


#define APPCAPITrace		!(__dwAPITraces & APPC_MESSAGE_MASK) ?   \
								(void)0 :                                   \
								SetAPITraceParams( THIS_FILE, __LINE__, ___pszFunctionName, APPC_MESSAGE_MASK ) &&     \
								PreAPITrace

#define CPICAPITrace		!(__dwAPITraces & CPIC_MESSAGE_MASK) ?   \
								(void)0 :                                   \
								SetAPITraceParams( THIS_FILE, __LINE__, ___pszFunctionName, CPIC_MESSAGE_MASK ) &&     \
								PreAPITrace

#define LUAAPITrace		    !(__dwAPITraces & LUA_MESSAGE_MASK) ?   \
								(void)0 :                                   \
								SetAPITraceParams( THIS_FILE, __LINE__, ___pszFunctionName, LUA_MESSAGE_MASK ) &&     \
								PreAPITrace

#define CSVAPITrace			!(__dwAPITraces & CSV_MESSAGE_MASK) ?   \
								(void)0 :                                   \
								SetAPITraceParams( THIS_FILE, __LINE__, ___pszFunctionName, CSV_MESSAGE_MASK ) &&     \
								PreAPITrace


/* COMTI API Trace Macros */


#define CMTIAPIComtiProxy		!(__dwAPITraces & COMTI_PROXY_MASK) ?   \
								(void)0 :                                   \
								SetAPITraceParams( THIS_FILE, __LINE__, ___pszFunctionName, APPC_MESSAGE_MASK ) &&     \
								PreAPITrace


#define CMTIAPIPipeline			!(__dwAPITraces & COMTI_PIPELINE_MASK) ?   \
								(void)0 :                                   \
								SetAPITraceParams( THIS_FILE, __LINE__, ___pszFunctionName, APPC_MESSAGE_MASK ) &&     \
								PreAPITrace


#define CMTIAPIBlackBoard		!(__dwAPITraces & COMTI_BB_MASK) ?   \
								(void)0 :                                   \
								SetAPITraceParams( THIS_FILE, __LINE__, ___pszFunctionName, APPC_MESSAGE_MASK ) &&     \
								PreAPITrace


#define CMTIAPIGeneralServices	!(__dwAPITraces & COMTI_SVC_MASK) ?   \
								(void)0 :                                   \
								SetAPITraceParams( THIS_FILE, __LINE__, ___pszFunctionName, APPC_MESSAGE_MASK ) &&     \
								PreAPITrace


#define CMTIAPIRepository		!(__dwAPITraces & COMTI_REPO_MASK) ?   \
								(void)0 :                                   \
								SetAPITraceParams( THIS_FILE, __LINE__, ___pszFunctionName, APPC_MESSAGE_MASK ) &&     \
								PreAPITrace


#define CMTIAPIDataTransit		!(__dwAPITraces & COMTI_DT_MASK) ?   \
								(void)0 :                                   \
								SetAPITraceParams( THIS_FILE, __LINE__, ___pszFunctionName, APPC_MESSAGE_MASK ) &&     \
								PreAPITrace


#define CMTIAPIConvert			!(__dwAPITraces & COMTI_CONV_MASK) ?   \
								(void)0 :                                   \
								SetAPITraceParams( THIS_FILE, __LINE__, ___pszFunctionName, APPC_MESSAGE_MASK ) &&     \
								PreAPITrace


#define CMTIAPITransport		!(__dwAPITraces & COMTI_TRANS_MASK) ?   \
								(void)0 :                                   \
								SetAPITraceParams( THIS_FILE, __LINE__, ___pszFunctionName, APPC_MESSAGE_MASK ) &&     \
								PreAPITrace

		
		

#define CMTIAPIRegistrar		!(__dwAPITraces & COMTI_REGISTRAR_MASK) ?   \
								(void)0 :                                   \
								SetAPITraceParams( THIS_FILE, __LINE__, ___pszFunctionName, APPC_MESSAGE_MASK ) &&     \
								PreAPITrace


#define CMTIAPIScripting		!(__dwAPITraces & COMTI_SCRIPTING_MASK) ?   \
								(void)0 :                                   \
								SetAPITraceParams( THIS_FILE, __LINE__, ___pszFunctionName, APPC_MESSAGE_MASK ) &&     \
								PreAPITrace


#define CMTIAPISessMgr			!(__dwAPITraces & COMTI_SESSMGR_MASK) ?   \
								(void)0 :                                   \
								SetAPITraceParams( THIS_FILE, __LINE__, ___pszFunctionName, APPC_MESSAGE_MASK ) &&     \
								PreAPITrace


#define CMTIAPIDataLayout		!(__dwAPITraces & COMTI_DL_MASK) ?   \
								(void)0 :                                   \
								SetAPITraceParams( THIS_FILE, __LINE__, ___pszFunctionName, APPC_MESSAGE_MASK ) &&     \
								PreAPITrace


#define CMTIAPITerminalService	!(__dwAPITraces & COMTI_TERMSERV_MASK) ?   \
								(void)0 :                                   \
								SetAPITraceParams( THIS_FILE, __LINE__, ___pszFunctionName, APPC_MESSAGE_MASK ) &&     \
								PreAPITrace

#define OLEDBAPITrace			!(__dwAPITraces & OLEDB_API_MASK) ?   \
								(void)0 :                                   \
								SetAPITraceParams( THIS_FILE, __LINE__, ___pszFunctionName, OLEDB_API_MASK ) &&     \
								PreAPITrace

#define ODBCAPITrace			!(__dwAPITraces & OLEDB_API_MASK) ?   \
								(void)0 :                                   \
								SetAPITraceParams( THIS_FILE, __LINE__, ___pszFunctionName, ODBC_API_MASK ) &&     \
								PreAPITrace

#define NetworkAPITrace			!(__dwAPITraces & NETWORK_API_MASK) ?   \
								(void)0 :                                   \
								SetAPITraceParams( THIS_FILE, __LINE__, ___pszFunctionName, NETWORK_API_MASK ) &&     \
								PreAPITrace








/* Check the Message Trace Flag */

#define IsInternalMessageTraceOn 	( (__dwMessageTraces & INTERNAL_MESSAGE_MASK) != 0 )

#define IsSNA3270MessageTraceOn		(  (__dwMessageTraces & SNA3270_MESSAGE_MASK) != 0 )

#define IsLU62MessageTraceOn		( (__dwMessageTraces & LU62_MESSAGE_MASK) != 0 )

#define IsDLCMessageTraceOn		    ( (__dwMessageTraces & DLC_MESSAGE_MASK) != 0 )

#define IsLEVEL2MessageTraceOn	    ( (__dwMessageTraces & LEVEL2_MESSAGE_MASK) != 0 )

#define IsSNAMessageTraceOn			( (__dwMessageTraces & SNA_MESSAGE_MASK) != 0 )

#define GetMessageFlags				(__dwMessageTraces | __dwAPITraces)

#define IsAPPCAPITraceOn			( (__dwAPITraces & APPC_MESSAGE_MASK) != 0 )

#define IsCPICAPITraceOn		    ( (__dwAPITraces & CPIC_MESSAGE_MASK) != 0 )

#define IsLUAAPITraceOn				( (__dwAPITraces & LUA_MESSAGE_MASK) != 0 )

#define IsCSVAPITraceOn				( (__dwAPITraces & CSV_MESSAGE_MASK) != 0 )
#define IsAPITraceOn				( __dwAPITraces != 0 )

#define IsInternalTraceOn           ((__dwEnabledTraces & INTERNAL_TRACE_MASK) != 0)



/*  Macro to check for COMTI API Trace state */
#define IsCOMTIAPITraceOn					( (__dwAPITraces & COMTI_ALL_API_MASK) != 0 )
#define IsCOMTIProxyTraceOn					( (__dwAPITraces & COMTI_PROXY_MASK) != 0 )
#define IsCOMTIPipelineTraceOn				( (__dwAPITraces & COMTI_PIPELINE_MASK) != 0 )
#define IsCOMTIBlackBoardTraceOn			( (__dwAPITraces & COMTI_BB_MASK) != 0 )
#define IsCOMTIGeneralServicesTraceOn		( (__dwAPITraces & COMTI_SVC_MASK) != 0 )
#define IsCOMTIRepositoryTraceOn			( (__dwAPITraces & COMTI_REPO_MASK) != 0 )
#define IsCOMTIDataTransitTraceOn			( (__dwAPITraces & COMTI_DT_MASK) != 0 )
#define IsCOMTIConvertTraceOn				( (__dwAPITraces & COMTI_DL_MASK) != 0 )
#define IsCOMTITransportTraceOn				( (__dwAPITraces & COMTI_TRANS_MASK) != 0 )
#define IsCOMTIRegistrarTraceOn				( (__dwAPITraces & COMTI_REGISTRAR_MASK) != 0 )
#define IsCOMTIScriptingTraceOn				( (__dwAPITraces & COMTI_SCRIPTING_MASK) != 0 )
#define IsCOMTISessMgrTraceOn				( (__dwAPITraces & COMTI_SESSMGR_MASK) != 0 )
#define IsCOMTIDataLayoutTraceOn			( (__dwAPITraces & COMTI_DL_MASK) != 0 )
#define IsCOMTITerminalServiceTraceOn		( (__dwAPITraces & COMTI_TERMSERV_MASK) != 0 )


/* Macro to check for COMTI Message Trace state */
#define IsCOMTIMsgTraceOn					( (__dwMessageTraces & COMTI_ALL_MSG_MASK) != 0 )
#define IsCOMTIInternalTraceOn				( (__dwMessageTraces & INTERNAL_MESSAGE_MASK) != 0 )
#define IsCOMTIDPLHeaderTraceOn				( (__dwMessageTraces & DPL_HEADER_MESSAGE_MASK) != 0 )
#define IsCOMTIFullLU62TraceOn				( (__dwMessageTraces & LU62_MESSAGE_MASK) != 0 )
#define IsCOMTIBriefLU62TraceOn				( (__dwMessageTraces & BRIEF_LU62_MESSAGE_MASK) != 0 )





#define BinaryInternalTrace( lParam, pbData, cbData )		\
								!(__dwMessageTraces & INTERNAL_MESSAGE_MASK) ?   \
								(void)0 :                                   \
								SetMessageTraceParams( THIS_FILE, __LINE__, ___pszFunctionName, INTERNAL_MESSAGE_MASK ) &&     \
								MessageBinaryTrace( lParam, TRACE_BINARY, pbData, cbData )

#define Binary3270Trace( lParam, pbData, cbData )		\
								!(__dwMessageTraces & SNA3270_MESSAGE_MASK) ?   \
								(void)0 :                                   \
								SetMessageTraceParams( THIS_FILE, __LINE__, ___pszFunctionName, SNA3270_MESSAGE_MASK ) &&     \
								MessageBinaryTrace( lParam, TRACE_BINARY, pbData, cbData )


#define BinaryLU62Trace( lParam, pbData, cbData )				\
								!(__dwMessageTraces & LU62_MESSAGE_MASK) ?   \
								(void)0 :                                   \
								SetMessageTraceParams( THIS_FILE, __LINE__, ___pszFunctionName, LU62_MESSAGE_MASK ) &&     \
								MessageBinaryTrace( lParam, TRACE_BINARY, pbData, cbData )




#define BinaryDLCTrace( lParam, pbData, cbData )	\
								!(__dwMessageTraces & DLC_MESSAGE_MASK) ?   \
			                    (void)0 :                                   \
						        SetMessageTraceParams( THIS_FILE, __LINE__, ___pszFunctionName, DLC_MESSAGE_MASK ) &&     \
								MessageBinaryTrace( lParam, TRACE_BINARY, pbData, cbData )


#define BinaryLevel2Trace( lParam, pbData, cbData )	\
								!(__dwMessageTraces & LEVEL2_MESSAGE_MASK) ?   \
								(void)0 :                                   \
								SetMessageTraceParams( THIS_FILE, __LINE__, ___pszFunctionName, LEVEL2_MESSAGE_MASK ) &&     \
							    MessageBinaryTrace( lParam, TRACE_BINARY, pbData, cbData )


#define BinarySNATrace( lParam, pbData, cbData )	\
								!(__dwMessageTraces & SNA_MESSAGE_MASK) ?   \
								(void)0 :                                   \
								SetMessageTraceParams( THIS_FILE, __LINE__, ___pszFunctionName, SNA_MESSAGE_MASK ) &&     \
								MessageBinaryTrace( lParam, TRACE_BINARY, pbData, cbData )

#define BinaryAllMsgTrace( lParam, pbData, cbData )	\
								SetMessageTraceParams( THIS_FILE, __LINE__, ___pszFunctionName, SNA_MESSAGE_MASK ) &&     \
								MessageBinaryTrace( lParam, TRACE_BINARY, pbData, cbData )


#define BinaryAPPCTrace( lParam, pbData, cbData )													\
        !(__dwAPITraces & APPC_MESSAGE_MASK) ?														\
        (void)0 :																					\
        SetAsyncTraceParams( THIS_FILE, __LINE__, ___pszFunctionName, MESSAGE_TRACE_MASK ) &&       \
        APIBinaryTrace( lParam, TRACE_BINARY, pbData, cbData )

#define BinaryCPICTrace( lParam, pbData, cbData )													\
        !(__dwAPITraces & CPIC_MESSAGE_MASK) ?														\
        (void)0 :																					\
        SetAsyncTraceParams( THIS_FILE, __LINE__, ___pszFunctionName, MESSAGE_TRACE_MASK ) &&       \
        APIBinaryTrace( lParam, TRACE_BINARY, pbData, cbData )

#define BinaryLUATrace( lParam, pbData, cbData )                   \
        !(__dwAPITraces & LUA_MESSAGE_MASK) ?             \
        (void)0 :                                               \
        SetAsyncTraceParams( THIS_FILE, __LINE__, ___pszFunctionName, MESSAGE_TRACE_MASK ) &&       \
        APIBinaryTrace( lParam, TRACE_BINARY, pbData, cbData )

#define BinaryCSVTrace( lParam, pbData, cbData )                   \
        !(__dwAPITraces & CSV_MESSAGE_MASK) ?             \
        (void)0 :                                               \
        SetAsyncTraceParams( THIS_FILE, __LINE__, ___pszFunctionName, MESSAGE_TRACE_MASK ) &&       \
        APIBinaryTrace( lParam, TRACE_BINARY, pbData, cbData )




#define TraceFunctEnter( sz )                   \
        char    *___pszFunctionName = sz;   \
        FunctTrace( 0, "Entering %s", sz )

#define TraceFunctLeave()                       \
        FunctTrace( 0, "Leaving %s", ___pszFunctionName )

#define TraceFunctEnterEx( lParam, sz )                   \
        char    *___pszFunctionName = sz;   \
        FunctTrace( lParam, "Entering %s", sz )

#define TraceFunctLeaveEx( lParam )                       \
        FunctTrace( lParam, "Leaving %s", ___pszFunctionName )

#define TraceQuietEnter( sz )                   \
        char    *___pszFunctionName = sz

/*
 * fixed number of parameters for Binary trace macros
 */
#define MessageTrace( lParam, pbData, cbData )                  \
        !(__dwEnabledTraces & MESSAGE_TRACE_MASK) ?             \
        (void)0 :                                               \
        SetAsyncTraceParams( THIS_FILE, __LINE__, ___pszFunctionName, MESSAGE_TRACE_MASK ) &&       \
        AsyncBinaryTrace( lParam, TRACE_MESSAGE, pbData, cbData )

#define BinaryTrace( lParam, pbData, cbData )                   \
        !(__dwEnabledTraces & MESSAGE_TRACE_MASK) ?             \
        (void)0 :                                               \
        SetAsyncTraceParams( THIS_FILE, __LINE__, ___pszFunctionName, MESSAGE_TRACE_MASK ) &&       \
        AsyncBinaryTrace( lParam, TRACE_BINARY, pbData, cbData )

#define UserTrace( lParam, dwUserType, pbData, cbData )         \
        !(__dwEnabledTraces & MESSAGE_TRACE_MASK) ?             \
        (void)0 :                                               \
        SetAsyncTraceParams( THIS_FILE, __LINE__, ___pszFunctionName, MESSAGE_TRACE_MASK ) &&       \
        AsyncBinaryTrace( lParam, dwUserType, pbData, cbData )


#ifndef THIS_FILE
#define THIS_FILE   __FILE__
#endif

#if !defined(DllExport)
    #define DllExport __declspec( dllexport )
#endif
#if !defined(DllImport)
    #define DllImport __declspec( dllimport )
#endif
#if !defined(_DBGTRACE_DLL_DEFINED)
    #define _DBGTRACE_DLL_DEFINED
    #if defined(WIN32)
        #if defined(_DBGTRACE_DLL_IMPLEMENTATION)
		    #define DbgTraceDLL DllExport
        #else
		    #define DbgTraceDLL DllImport
        #endif
    #else
        #define DbgTraceDLL
    #endif
#endif

/*
 * imported trace flag used by trace macros to determine if the trace
 * statement should be executed
 */
extern DWORD DbgTraceDLL    __dwEnabledTraces;
extern DWORD DbgTraceDLL	__dwMessageTraces;
extern DWORD DbgTraceDLL	__dwAPITraces;




/*
 * import functions from DBGTRACE.DLL
 */
extern DbgTraceDLL BOOL WINAPI InitAsyncTrace( void );
extern DbgTraceDLL BOOL WINAPI GetTraceCtrlBlock(VOID **pptrc);
extern DbgTraceDLL BOOL WINAPI InitAsyncTraceEx( LPSTR pszName );
/*  *************************** WARNING *********************************
 *
 * Don't call TermAsyncTrace at DLL_PROCESS_DETACH, it will hang the process when it waits for the thread to terminate
 *
 *  *************************** WARNING *********************************
 */
extern DbgTraceDLL BOOL WINAPI TermAsyncTrace( void );
extern DbgTraceDLL BOOL	WINAPI HardTermAsyncTrace(void);
extern DbgTraceDLL BOOL WINAPI FlushAsyncTrace( void );

extern DbgTraceDLL int WINAPI AsyncStringTrace( LONG_PTR  lParam,
                                                LPCSTR  szFormat,
                                                va_list marker );

extern DbgTraceDLL int WINAPI AsyncBinaryTrace( LONG_PTR  lParam,
                                                DWORD   dwBinaryType,
                                                LPBYTE  pbData,
                                                DWORD   cbData );

extern DbgTraceDLL int WINAPI APIBinaryTrace( LONG_PTR  lParam,
                                                DWORD   dwBinaryType,
                                                LPBYTE  pbData,
                                                DWORD   cbData );

extern DbgTraceDLL int WINAPI MessageBinaryTrace( LONG_PTR  lParam,
                                                DWORD   dwBinaryType,
                                                LPBYTE  pbData,
                                                DWORD   cbData );

extern DbgTraceDLL int WINAPI SetAsyncTraceParams(  LPSTR   pszFile,
                                                    int     iLine,
                                                    LPSTR   szFunction,
                                                    DWORD   dwTraceMask );

extern DbgTraceDLL int WINAPI SetMessageTraceParams(  LPSTR   pszFile,
                                                    int     iLine,
                                                    LPSTR   szFunction,
                                                    DWORD   dwTraceMask );
extern DbgTraceDLL int WINAPI SetAPITraceParams(  LPSTR   pszFile,
                                                    int     iLine,
                                                    LPSTR   szFunction,
                                                    DWORD   dwTraceMask );



extern DbgTraceDLL int WINAPI AsyncMessageTrace( LPARAM		lParam,
												 LPCSTR		szFormat,
												 va_list	marker );

extern DbgTraceDLL int WINAPI AsyncAPITrace( LPARAM		lParam,
												 LPCSTR		szFormat,
												 va_list	marker );


/*
 * Trace flag constants
 */
#define FATAL_TRACE_MASK			0x00000001
#define ERROR_TRACE_MASK			0x00000002
#define DEBUG_TRACE_MASK			0x00000004
#define STATE_TRACE_MASK			0x00000008
#define FUNCT_TRACE_MASK			0x00000010
#define MESSAGE_TRACE_MASK			0x00000020
#define CUSTOM_TRACE_MASK			0x00000040
#define	PLAN_ENTER_EXIT_MASK		0x00000080

#define INTERNAL_TRACE_MASK         0x000000DF
#define ALL_TRACE_MASK				0xFFFFFFFF
#define NUM_TRACE_TYPES     		8

/*
 *  SNA Server Message Trace Flags, same as define in comw32.h
 */
#define DLC_MESSAGE_MASK			0x00000001
#define LU62_MESSAGE_MASK			0x00000002
#define SNA3270_MESSAGE_MASK		0x00000004
#define INTERNAL_MESSAGE_MASK		0x00000008
#define SNA_MESSAGE_MASK			0x00000010
#define DPL_HEADER_MESSAGE_MASK		0x00000020
#define BRIEF_LU62_MESSAGE_MASK		0x00000040
#define LEVEL2_MESSAGE_MASK			0x00001000
#define COMTI_ALL_MSG_MASK			0x00000069
/* DDM and DB2 message trace */
#define CONNECTION_INFO_MASK		0x00002000
#define NETWORK_DS_MASK				0x00004000
#define ALL_MSG_MASK				0x0000707f

#define NUM_MESSAGE_TRACE_TYPES		8
/*
 * API Message Trace Flags
 */
#define APPC_MESSAGE_MASK			0x00000800
#define CPIC_MESSAGE_MASK			0x00000040
#define LUA_MESSAGE_MASK			0x00000080
#define CSV_MESSAGE_MASK			0x00000400


/* COMTI message trace */
#define	COMTI_PROXY_MASK			0x00001000
#define COMTI_PIPELINE_MASK			0x00002000
#define COMTI_BB_MASK				0x00004000
#define COMTI_SVC_MASK				0x00008000
#define COMTI_REPO_MASK				0x00010000
#define COMTI_DT_MASK				0x00020000
#define COMTI_DL_MASK				0x00040000
#define COMTI_CONV_MASK				0x00080000
#define COMTI_TRANS_MASK			0x00100000
#define COMTI_REGISTRAR_MASK		0x00200000
#define COMTI_SCRIPTING_MASK		0x00400000
#define COMTI_SESSMGR_MASK			0x00800000
#define COMTI_TERMSERV_MASK			0x01000000
#define COMTI_ALL_API_MASK			0x01fff000


/* DDM and DB2 API Trace */
#define OLEDB_API_MASK				0x02000000
#define ODBC_API_MASK				0x04000000
#define NETWORK_API_MASK			0x08000000










#define NUM_API_TRACE_TYPES			4
/*
 * Output trace types. used by tools to modify the
 * registry to change the output target
 */
enum tagTraceOutputTypes {
    TRACE_OUTPUT_DISABLED = 0,
    TRACE_OUTPUT_FILE = 1,
    TRACE_OUTPUT_DEBUG = 2,
    TRACE_OUTPUT_DISCARD = 4        /* used to find race windows */
};

#define TRACE_OUTPUT_INVALID    \
        ~(TRACE_OUTPUT_FILE|TRACE_OUTPUT_DEBUG|TRACE_OUTPUT_DISCARD)


#define IsTraceFile(x)      ((x) & TRACE_OUTPUT_FILE)
#define IsTraceDebug(x)     ((x) & TRACE_OUTPUT_DEBUG)
#define IsTraceDiscard(x)   ((x) & TRACE_OUTPUT_DISCARD)


/*
 * predefined types of binary trace types.  User defined
 * types must be greater than 0x8000
 */
enum tagBinaryTypes {
    TRACE_STRING = 0,
    TRACE_BINARY,
    TRACE_MESSAGE,
    TRACE_USER = 0x8000
};

#include <stdarg.h>

/*
 * use __inline to ensure grab __LINE__ and __FILE__
 */
__inline int WINAPIV PreAsyncTrace( LONG_PTR lParam, LPCSTR szFormat, ... )
{
    va_list marker;
    int     iLength;

    va_start( marker, szFormat );
    iLength = AsyncStringTrace( lParam, szFormat, marker );
    va_end( marker );

    return  iLength;
}

/*
 *	extended from PreAsyncTrace, but eliminate the first argument
 */
__inline int WINAPIV PreAsyncTraceEx( LPCSTR szFormat, ... )
{
    va_list marker;
    int     iLength;

    va_start( marker, szFormat );
    iLength = AsyncStringTrace( (LONG_PTR)NULL, szFormat, marker );
    va_end( marker );

    return  iLength;
}

/*
 * use __inline to ensure grab __LINE__ and __FILE__
 */
__inline int WINAPIV PreMessageTrace( LONG_PTR lParam, LPCSTR szFormat, ... )
{
    va_list marker;
    int     iLength;
    va_start( marker, szFormat );
    iLength = AsyncMessageTrace( lParam, szFormat, marker );
    va_end( marker );

    return  iLength;
}

/*
 * use __inline to ensure grab __LINE__ and __FILE__
 */
__inline int WINAPIV PreAPITrace( LONG_PTR lParam, LPCSTR szFormat, ... )
{
    va_list marker;
    int     iLength;

    va_start( marker, szFormat );
    iLength = AsyncAPITrace( lParam, szFormat, marker );
    va_end( marker );

    return  iLength;
}
#endif /* !defined(NOTRACE)  */

/* Asserts are independent of tracing
 * (with the exception of flushing the trace buffer).
 *
 * For now enable ASSERT defines only if debugging is enabled
 */
#ifdef  _DEBUG
 /*
  * Macros added for doing asserts and verifies.  basic clones
  * of the MFC macros with a prepended _ symbol
  */
 #define ASYNC_ASSERT(f)		!(f) ? FLUSHASYNCTRACE DebugBreak() : ((void)0)
 #define ASYNC_VERIFY(f)         ASYNC_ASSERT(f)

#else

 #define ASYNC_ASSERT(f)		((void)0)
 #define ASYNC_VERIFY(f)        ((void)(f))

#endif

#endif /* !defined(_DBGTRACE_H_) */

#ifdef __cplusplus
}
#endif

