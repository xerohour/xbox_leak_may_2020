//
// ParseErr.h
//


//
// Flags used by ParseFunction() ...
//
#define PF_PARSE_SYMBOL_DECL	0x00000001		// break decl down into components
#define PF_PARAM_NAMES			0x00000002		// include func param names
#define PF_PARAM_DEFAULTS		0x00000004		// include func param default
#define PF_LOOKUP_IINST_DEFN	0x00000008		// Lookup the iinst of the defn in the NCB
#define PF_LOOKUP_IINST_DECL	0x00000010		// Lookup the iinst of the decl in the NCB
#define PF_COMPARE_IINST		0x00000020		// Compare with given iinst in the NCB



//
// Error codes returned by SignatureMatch and ParseFunction in FEACP
//
// Note: The original values come from "ide\pkgs\include\parse.h"
//
#define PARSE_NOERROR				0			// matched perfectly
#define E_PARSE_MISMATCH_PARENT		1
#define E_PARSE_NO_TEMPLATE_DEFN	2
#define E_PARSE_NO_DEFN				3
#define E_PARSE_MULTI_LINE			4
#define E_PARSE_INVALID_TOKEN		5
#define E_PARSE_NO_LEXER			6
#define E_PARSE_EMPTY_STREAM		7
#define E_PARSE_MISMATCH_BRACKET	8
#define E_PARSE_UNEXPECTED_EOL		9
#define E_PARSE_EXTRA_TOKEN			10
#define E_PARSE_CTOR_RET_TYPE		11
#define E_PARSE_DUP_ACCESS_SPEC		12
#define E_PARSE_INVALID_MODIFIER	13
#define E_PARSE_MACROERROR			14
// except for the following new ones ...
#define E_PARSE_NOMATCH				20			// symbol name's don't match up
#define E_PARSE_LOOKUP_FAILED		21			// No symbol's with that name in the NCB
#define E_PARSE_NUM_MISMATCH		22			// mismatched number of parameters
#define E_PARSE_PARAM_MISMATCH		23			// parameter mismatched
#define E_PARSE_RET_TYPE_CONFLICT	24			// Return type incorrect
#define E_PARSE_BAD_IINST1			25			// bad iinst1 parameter to SignatureMatch(<other-args>, iinst1, iinst2)
#define E_PARSE_BAD_IINST2			26			// bad iinst2 parameter to SignatureMatch(<other-args>, iinst1, iinst2)
#define E_PARSE_NOT_IMPLEMENTED		27			// Signature Matching encountered symbol type not yet implemented
#define E_PARSE_NO_DECL_FOUND		28			// ParseFunction could not find a declaration
#define E_PARSE_NO_DEFN_FOUND		29			// ParseFunction could not find a declaration
#define E_PARSE_THIS_MISMATCH		30			// this pointer modifiers mismatched (e.g. const)
#define E_PARSE_ALLOC_FAILED		31			// ParseFunction could not allocate space for array
#define E_PARSE_REPEATED_PARAM		32			// ParseFunction found a repeated param name
#define E_PARSE_SYNTAX_ERROR		33			// ParseFunction found a syntax error


//
// Error codes returned by GetTypeFromBuffer (./-> completion)
// and GetIdentTypeFromBuffer (QuickInfo)
//
#define	ACP_NOERROR					0
#define	E_ACP_FAULT					1			// Hit an access violation
#define	E_ACP_STACK_OVERFLOW		2			// Hit a stack overflow
#define	E_ACP_EOF_ABORT				3			// Hit an unexpected EOF we couldn't recover from
#define	E_ACP_FATAL_OVERFLOW		4			// Hit too many fatal error messages
#define	E_ACP_ERROR_OVERFLOW		5			// Hit too many error messages
#define	E_ACP_PARSER_OVERFLOW		6			// Hit a yacc parser stack overflow
#define	E_ACP_CHKBUF_OVERFLOW		7			// Failed to expand buffer in chkbuf
#define E_ACP_NCB_CALL_FAILED		8			// NCB call failed
#define E_ACP_OUT_OF_HEAP_MEMORY	9			// Call to obtain heap memory failed

// GetTypeFromBuffer specific errors:
#define	E_ACP_DA_NOTREE				10			// LHS of ./-> is in error (no tree)
#define	E_ACP_DA_NOTYPE				11			// LHS of ./-> doesn't have a type
#define	E_ACP_DA_INDIRECTION		12			// LHS of ./-> has inappropriate level of indirection
#define	E_ACP_DA_NOTCSU				13			// LHS of ./-> doesn't name a class/struct/union
#define E_ACP_DA_NO_DOT_ARROW		14			// Call to autocomplete entrypoint without ./-> as EOB
#define E_ACP_DA_TENTATIVE			15			// e.g.   <!>void foo() { Ty *pT; pT-><!> }
												// We know that the type is Ty but Ty was not found in NCB
												// and therefore is just a guess.
#define E_ACP_DA_TEMPLATE_PARAMETER	16			// Resolving type of expression on LHS of ./-> depends on an
												// uninstanciated template parameter.

// GetIdentTypeFromBuffer specific errors:
#define	E_ACP_QI_NOTFOUND			20			// Identifier not found in symbol table or NCB
#define	E_ACP_QI_NOTYPE				21			// Identifier isn't something with a type
#define	E_ACP_QI_TENTATIVE			22			// Identifier is an artificial tentative tag
#define	E_ACP_QI_ERRSYM				23			// Identifier is an error symbol
#define E_ACP_QI_KEYWORD			24			// Identifier was a C/C++ keyword
#define E_ACP_QI_NOT_IDENT			25			// Cursor was not on an identifier but on a hard token

// DoParseForErrors specific errors
#define E_ACP_PE_ABORT				30			// Aborting due to errors from injected text from NCB's

// More general acd errors continued after 9 ...
#define E_ACP_NO_TARGET_FOR_IMOD	40			// The specified imod did not exist in NCB / did not
												// have any target associated with it
#define E_ACP_DECORATED_NAME		41			// Return string was a decorated name

// DoListIncludes specific errors
#define E_ACP_LI_ABORT				50			// Fatal error during LI

//
// Base error count on returning normally from feacp.  If feacp doesn't detect
// the cursor and just compiles to the end of the buffer, it will return the
// current error count, which is actually E_ACP_EXIT_ERRORS + # of real errors
//
#define	E_ACP_EXIT_ERRORS			100

//
// Flags returned by ReportError() which is callback from DoParseForErrors()
//
//
#define RE_FATAL_ERROR		0x00000001
#define RE_ERROR			0x00000002
#define RE_ERROR_3			0x00000003
#define RE_WARNING			0x00000004
#define RE_RESERVED_5		0x00000005
#define RE_NOTE				0x00000006
#define RE_RESERVED_7		0x00000007
#define RE_RESERVED_8		0x00000008
#define RE_RESERVED_9		0x00000009
