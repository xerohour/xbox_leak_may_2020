/*++

Copyright (c) 2000 Microsoft Corporation


Module Name:

    didbg.h

Abstract:

  Macros for printing debug messages.

Environment:

  XAPI

Notes:

Revision History:

    05-11-00 created by Mitchell Dernis (mitchd)

--*/

#ifndef	__DIDBG_H__
#define	__DIDBG_H__

// Hack! DbgPrint is much nicer than OutputDebugString so JonT exported it from xapi.lib
// (as DebugPrint) but the prototype isn't in the public headers...
extern "C"
{
	ULONG
	DebugPrint(
		PCHAR Format,
		...
		);
}

//
//	DEBUG_MODULE_NAME is declared as a variable for the debug level of the module.
//	DEBUG_TRACE_NAME is a name prepended to all tracouts.
//
//	example:
//		#define DEBUG_MODULE_NAME FooCpp
//		#define DEBUG_TRACE_NAME "Foo"
//		#include <dinputi.h>
//
#ifndef DEBUG_MODULE_NAME
#error "You need to #define DEBUG_MODULE_NAME before including didbg.h"
#endif

#ifndef DEBUG_TRACE_NAME
#error "You need to #define DEBUG_TRACE_NAME before including didbg.h"
#endif

//---------------------------------------------------------------------------
//			Definitions
//---------------------------------------------------------------------------

#define DEBUG_LEVEL_VERBOSE	0
#define DEBUG_LEVEL_WARN	1
#define DEBUG_LEVEL_ERROR	2

#ifndef DEBUG_LEVEL_DEFAULT
#define DEBUG_LEVEL_DEFAULT DEBUG_LEVEL_ERROR
#endif

/*
**	Definitions for DEBUG BUILDS
**
**
*/
#if	DBG

int DEBUG_MODULE_NAME = DEBUG_LEVEL_DEFAULT;

#define TRACE_VERBOSE(__x__)\
	if(DEBUG_MODULE_NAME >= DEBUG_LEVEL_VERBOSE)\
	{\
		DebugPrint( "%s: ", DEBUG_TRACE_NAME);\
		DebugPrint __x__;\
	}

#define TRACE_WARN(__x__)\
	if(DEBUG_MODULE_NAME >= DEBUG_LEVEL_WARN)\
	{\
		DebugPrint( "%s: ", DEBUG_TRACE_NAME);\
		DebugPrint __x__;\
	}

#define TRACE_ERROR(__x__)\
	if(DEBUG_MODULE_NAME >= DEBUG_LEVEL_ERROR)\
	{\
		DebugPrint( "%s: ", DEBUG_TRACE_NAME);\
		DebugPrint __x__;\
	}

#define TRACE(__x__)\
	DebugPrint( "%s: ", DEBUG_TRACE_NAME);\
	DebugPrint __x__;


#define	ASSERT_MSG(_cond_, _msg_)\
	if(!(_cond_))\
	{\
		DebugPrint("Assertion failed: %s(%d)\n", __FILE__,__LINE__);\
		DebugPrint(#_cond_);\
		DebugPrint("\n");\
		DebugPrint _msg_;\
		DebugBreak();\
	}

#define	ASSERT(_cond_)\
	if(!(_cond_))\
	{\
		DebugPrint("Assertion failed: %s(%d)\n", __FILE__,__LINE__);\
		DebugPrint(#_cond_);\
		DebugPrint("\n");\
		DebugBreak();\
	}

#define DEBUG_BREAK() DebugBreak()

#define CHECK_STRUCT_SIZE(_SizeExpression_, _StructName_, _ApiName_)\
    if( (_SizeExpression_) != sizeof(_StructName_))\
    {\
        DebugPrint("Call to %s, %s is invalid.\n",#_ApiName_, #_SizeExpression_);\
        DebugPrint("sizeof(%s) = %d\n", #_StructName_, sizeof(_StructName_));\
        DebugPrint("%s = %d\n", #_SizeExpression_, (_SizeExpression_));\
        DebugBreak();\
        return DIERR_INVALIDPARAM;\
    }

#define CHECK_PARAMETER_NOT_NULL(_API_NAME_, _PARAMETER_)\
	if(NULL == _PARAMETER_)\
	{\
		DebugPrint("ERROR: %s was NULL in call to %s\n",  #_PARAMETER_, #_API_NAME_);\
		DebugBreak();\
        return DIERR_INVALIDPARAM;\
    }

#define CHECK_PARAMETER_VALID_INSTANCE_GUID(_API_NAME_, _guidInstance_)\
	if(!IS_VALID_GUID_INSTANCE(_guidInstance_))\
	{\
		DebugPrint("ERROR: %s invalid in call to %s\n",  #_guidInstance_, #_API_NAME_);\
		DebugBreak();\
		return DIERR_INVALIDPARAM;\
	}

/*
**	Definitions for RELEASE builds
**
**
*/

#else		// DBG=0

#define	TRACE_VERBOSE(__x__)
#define	TRACE_WARN(__x__)
#define	TRACE_ERROR(__x__)
#define TRACE(__x__)

#define	ASSERT_MSG(_cond_, _msg_)
#define	ASSERT(_cond_)

#define DEBUG_BREAK()

#define CHECK_STRUCT_SIZE(_SizeExpression_, _StructName_, _ApiName_)\
    if((_SizeExpression_) != sizeof(_StructName_)) return DIERR_INVALIDPARAM;

#define CHECK_PARAMETER_NOT_NULL(_API_NAME_, _PARAMETER_)\
	if(NULL == _PARAMETER_) return DIERR_INVALIDPARAM;

#define CHECK_PARAMETER_VALID_INSTANCE_GUID(_API_NAME_, _guidInstance_)\
	if(!IS_VALID_GUID_INSTANCE(_guidInstance_))	return DIERR_INVALIDPARAM;

#endif	// DBG=?


//===========================================================================
//			End
//===========================================================================

#endif	// __DIDBG_H__





