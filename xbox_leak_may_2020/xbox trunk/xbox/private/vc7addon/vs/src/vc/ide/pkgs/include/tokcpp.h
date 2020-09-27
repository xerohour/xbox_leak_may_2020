// tokcpp.h - tokens for C++
//
// Include lex.h before including this file.
//

#pragma once

#include <cppsvc.h>

enum TOKCPP {	
	tokUNKNOWN = tokclsError,
	tokKEYW_AUTO = tokclsKeyWordMin,
	tokKEYW_BREAK,
	tokKEYW_BOOL,
	tokKEYW_BOOLEAN,//ODL
	tokKEYW_BYTE,//ODL
	tokKEYW_CASE,
	tokKEYW_CATCH,
	tokKEYW_CHAR,
	tokKEYW_CLASS,
	tokKEYW_COCLASS,//ODL
	tokKEYW_CONST,
	tokKEYW_CONST_CAST,
	tokKEYW_CONTINUE,
	tokKEYW_CPP_QUOTE,//ODL
	tokKEYW_DISPINTERFACE,//ODL
	tokKEYW_DEFAULT,
	tokKEYW_DELETE,
	tokKEYW_DO,
	tokKEYW_DOUBLE,
	tokKEYW_DYNAMIC_CAST,
	tokKEYW_ELSE,
	tokKEYW_ENUM,
	tokKEYW_ERROR_STATUS_T,//ODL
	tokKEYW_EXPLICIT,
	tokKEYW_EXTERN,
	tokKEYW_FALSE,
	tokKEYW_FLOAT,
	tokKEYW_HANDLE_T,//ODL
	tokKEYW_HYPER,//ODL
	tokKEYW_FOR,
	tokKEYW_FRIEND,
	tokKEYW_GOTO,
	tokKEYW_IF,
	tokKEYW_IMPORT,//ODL
	tokKEYW_IMPORTLIB,//ODL
	tokKEYW_INCLUDE,//ODL
	tokKEYW_INLINE,
	tokKEYW_INT,
	tokKEYW_INTERFACE,
	tokKEYW_LIBRARY,//ODL
	tokKEYW_LONG,
	tokKEYW_MAIN,
	tokKEYW_METHODS,//ODL
	tokKEYW_MODULE,//ODL
	tokKEYW_MUTABLE,
	tokKEYW_NAMESPACE,
	tokKEYW_NEW,
	tokKEYW_OPERATOR,
	tokKEYW_PASCAL,
	tokKEYW_PRIVATE,
	tokKEYW_PROPERTIES,//ODL
	tokKEYW_PROTECTED,
	tokKEYW_PUBLIC,
	tokKEYW_REGISTER,
	tokKEYW_REINTERPRET_CAST,
	tokKEYW_RETURN,
	tokKEYW_SHORT,
	tokKEYW_SIGNED,
	tokKEYW_SIZEOF,
	tokKEYW_SMALL,//ODL
	tokKEYW_STATIC,
	tokKEYW_STATIC_CAST,
	tokKEYW_STRUCT,
	tokKEYW_SUPER,
	tokKEYW_SWITCH,
	tokKEYW_TEMPLATE,
	tokKEYW_THIS,
	tokKEYW_THROW,
	tokKEYW_TRUE,
	tokKEYW_TRY,
	tokKEYW_TYPEDEF,
	tokKEYW_TYPEID,
	tokKEYW_TYPENAME,
	tokKEYW_UNION,
	tokKEYW_UNSIGNED,
	tokKEYW_USING,
	tokKEYW_VIRTUAL,
	tokKEYW_VOID,
	tokKEYW_VOLATILE,
	tokKEYW_WHILE,
	tokKEYW_WMAIN,
	tokKEYW_XALLOC,
	tokKEYW_WCHAR_T,

	tokKEYW__ABSTRACT,//MC++
	tokKEYW__ALIGNOF,
	tokKEYW__ASM,
	tokKEYW__ASSUME,
	tokKEYW__BASED,
	tokKEYW__BOX,//MC++
	tokKEYW__CDECL,
	tokKEYW__DECLSPEC,
	tokKEYW__DELEGATE,//MC++
	tokKEYW__EVENT,//MC++
	tokKEYW__EXCEPT,
	tokKEYW__FASTCALL,
	tokKEYW__FINALLY,
	tokKEYW__FORCEINLINE,
	tokKEYW__GC,//MC++
    tokKEYW__HOOK,//MC++
	tokKEYW__IDENTIFIER,//MC++
	tokKEYW__IF_EXISTS,
	tokKEYW__IF_NEXISTS,
	tokKEYW__INLINE,
	tokKEYW__INT16,
	tokKEYW__INT32,
	tokKEYW__INT64,
	tokKEYW__INT8,
	tokKEYW__INTERFACE,//MC++
	tokKEYW__LEAVE,
    tokKEYW__M128,//MC++
    tokKEYW__M128D,//MC++
    tokKEYW__M128I,//MC++
    tokKEYW__M64,//MC++
	tokKEYW__MULTINHERIT,//MC++
	tokKEYW__NOGC,//MC++
	tokKEYW__NOOP,
	tokKEYW__PASCAL,
	tokKEYW__PIN,//MC++
	tokKEYW__PRAGMA,
	tokKEYW__PROPERTY,//MC++
	tokKEYW__PTR64,
	tokKEYW__RAISE,//MC++
	tokKEYW__SEALED,//MC++
	tokKEYW__SERIALIZABLE,//MC++
	tokKEYW__SINGINHERIT,
	tokKEYW__STDCALL,
	tokKEYW__SUPER,
	tokKEYW__TRANSIENT,//MC++
	tokKEYW__TRY,
	tokKEYW__TRY_CAST,//MC++
	tokKEYW__TYPEOF,//MC++
	tokKEYW__UNALIGNED,
	tokKEYW__UNHOOK,//MC++
	tokKEYW__UUIDOF,
	tokKEYW__VALUE,//MC++
	tokKEYW__VIRTINHERIT,
	tokKEYW__WCHAR_T,

	tokATTR_MIN,//ODL
	tokATTR_APPOBJECT,//ODL
    tokATTR_ASYNC_UUID, //ODL
	tokATTR_AUTO_HANDLE,//ODL
	tokATTR_BASE_TYPES,//ODL
	tokATTR_BINDABLE,//ODL
	tokATTR_BROADCAST,//ODL
	tokATTR_BYTE_COUNT,//ODL
	tokATTR_CALL_AS,//ODL
	tokATTR_CALLBACK,//ODL
	tokATTR_CODE,//ODL
	tokATTR_COMM_STATUS,//ODL
	tokATTR_CONTEXT_HANDLE,//ODL
	tokATTR_CONTROL,//ODL
	tokATTR_CUSTOM,//ODL
	tokATTR_DECODE,//ODL
	tokATTR_DEFAULT,//ODL
	tokATTR_DEFAULTBIND,//ODL
    tokATTR_DEFAULTCOLLELEM,//ODL
    tokATTR_DEFAULTVALUE, //ODL
	tokATTR_DISPLAYBIND,//ODL
	tokATTR_DLLNAME,//ODL
	tokATTR_DUAL,//ODL
	tokATTR_ENABLE_ALLOCATE,//ODL
	tokATTR_ENCODE,//ODL
	tokATTR_ENDPOINT,//ODL
	tokATTR_ENTRY,//ODL
	tokATTR_EXPLICIT_HANDLE,//ODL
	tokATTR_FAULT_STATUS,//ODL
	tokATTR_FIRST_IS,//ODL
	tokATTR_HANDLE,//ODL
	tokATTR_HELPCONTEXT,//ODL
	tokATTR_HELPFILE,//ODL
	tokATTR_HELPSTRING,//ODL
	tokATTR_HELPSTRINGCONTEXT,//ODL
	tokATTR_HIDDEN,//ODL
	tokATTR_ID,//ODL
	tokATTR_IGNORE,//ODL
	tokATTR_IID_IS,//ODL
    tokATTR_IMMEDIATEBIND,//ODL
	tokATTR_IMPLICIT_HANDLE,//ODL
	tokATTR_IN,//ODL
	tokATTR_LAST_IS,//ODL
	tokATTR_LCID,//ODL
	tokATTR_LENGTH_IS,//ODL
	tokATTR_LICENSED,//ODL
	tokATTR_LOCAL,//ODL
	tokATTR_MAX_IS,//ODL
	tokATTR_MAYBE,//ODL
	tokATTR_MS_UNION,//ODL
	tokATTR_NOCODE,//ODL
	tokATTR_NONBROWSABLE,//ODL
	tokATTR_NONEXTENSIBLE,//ODL
	tokATTR_NOTIFY,//ODL
	tokATTR_OBJECT,//ODL
	tokATTR_ODL,//ODL
	tokATTR_OLEAUTOMATION,//ODL
	tokATTR_OPTIMIZE,//ODL
	tokATTR_OPTIONAL,//ODL
	tokATTR_OUT,//ODL
	tokATTR_POINTER_DEFAULT,//ODL
	tokATTR_PROPGET,//ODL
	tokATTR_PROPPUT,//ODL
	tokATTR_PROPPUTREF,//ODL
	tokATTR_PTR,//ODL
	tokATTR_PUBLIC,//ODL
	tokATTR_READONLY,//ODL
	tokATTR_REF,//ODL
	tokATTR_REPRESENT_AS,//ODL
	tokATTR_REQUESTEDIT,//ODL
	tokATTR_RESTRICTED,//ODL
	tokATTR_RETVAL,//ODL
	tokATTR_SIZE_IS,//ODL
	tokATTR_SOURCE,//ODL
	tokATTR_STRING,//ODL
	tokATTR_SWITCH_IS,//ODL
	tokATTR_SWITCH_TYPE,//ODL
	tokATTR_TRANSMIT_AS,//ODL
	tokATTR_UNIQUE,//ODL
	tokATTR_UUID,//ODL
	tokATTR_V1_ENUM,//ODL
	tokATTR_VARARG,//ODL
	tokATTR_VERSION,//ODL
	tokATTR_MAX,//ODL			// last attribute key


	tokKEYW__MAX,			/* last keyword */

	tokPPKEYW_MIN,			/* min Preprocessor keyword */
	tokPPKEYW_DEFINE,
	tokPPKEYW_ELIF,
	tokPPKEYW_ELSE,
	tokPPKEYW_ENDIF,
	tokPPKEYW_ERROR,
	tokPPKEYW_IF,
	tokPPKEYW_IFDEF,
	tokPPKEYW_IFNDEF,
	tokPPKEYW_IMPORT,
	tokPPKEYW_INCLUDE,
	tokPPKEYW_LINE,
	tokPPKEYW_PRAGMA,
	tokPPKEYW_UNDEF,
	tokPPKEYW_USING,//MC++
	tokPPKEYW_MAX,			/* max Preprocessor keyword */	

	tokDECLKEYW_ALIGN,	/* keywords following _declspec */
	tokDECLKEYW_ALLOCATE,
	tokDECLKEYW_DEPRECATED,
	tokDECLKEYW_DLLEXPORT,
	tokDECLKEYW_DLLIMPORT,
	tokDECLKEYW_NAKED,
	tokDECLKEYW_NOINLINE,
	tokDECLKEYW_NORETURN,
	tokDECLKEYW_NOTHROW,
	tokDECLKEYW_NOVTABLE,
	tokDECLKEYW_PROPERTY,
	tokDECLKEYW_SEALED,
	tokDECLKEYW_SELECTANY,
	tokDECLKEYW_THREAD,
	tokDECLKEYW_UUID,
	
	tokASSUMEKEYW_EVAL,		// keywords following __assume
	tokASSUMEKEYW_NOEVAL,

	tokPPIFKEYW_DEFINED, 	/* keyword following #if */

	tokPRAGKEYW_ALLOCTEXT,	/* keywords following #pragma */    
	tokPRAGKEYW_AUTOINLINE,
	tokPRAGKEYW_BESTCASE,
	tokPRAGKEYW_BSS_SEG,
	tokPRAGKEYW_CHKSTACK,
	tokPRAGKEYW_CODESEG,
	tokPRAGKEYW_COMMENT,
	tokPRAGKEYW_COMPILER,
	tokPRAGKEYW_COMPONENT,
	tokPRAGKEYW_CONFORM,
	tokPRAGKEYW_CONST_SEG,
	tokPRAGKEYW_DATASEG,
	tokPRAGKEYW_DEFAULT,
	tokPRAGKEYW_DEPRACATED,
	tokPRAGKEYW_DISABLE,
	tokPRAGKEYW_ERROR,
	tokPRAGKEYW_EXESTR,
	tokPRAGKEYW_FULLGENERALITY,
	tokPRAGKEYW_FUNCTION,
	tokPRAGKEYW_HDRSTOP,
	tokPRAGKEYW_INCLUDE_ALIAS,
	tokPRAGKEYW_INITSEG,
	tokPRAGKEYW_INLINEDEPTH,
	tokPRAGKEYW_INLINERECURSE,
	tokPRAGKEYW_INSTRINSIC,
	tokPRAGKEYW_KEYWORD,
	tokPRAGKEYW_LIB,
	tokPRAGKEYW_MANAGED,
	tokPRAGKEYW_MESSAGE,
	tokPRAGKEYW_MIDL_ECHO,//ODL
	tokPRAGKEYW_MULINHERIT,
	tokPRAGKEYW_OFF,
	tokPRAGKEYW_ON,
	tokPRAGKEYW_ONCE,
	tokPRAGKEYW_OPTIMIZE,
	tokPRAGKEYW_PACK,
	tokPRAGKEYW_PTRTOMEMBERS,
	tokPRAGKEYW_POP,
	tokPRAGKEYW_POPMACRO,
	tokPRAGKEYW_PUSH,
	tokPRAGKEYW_PUSHMACRO,
	tokPRAGKEYW_RESTORE,
	tokPRAGKEYW_RUNTIME_CHECKS,
	tokPRAGKEYW_SECTION,
	tokPRAGKEYW_SETLOCALE,
	tokPRAGKEYW_SINGINHERIT,
	tokPRAGKEYW_UNMANAGED,
	tokPRAGKEYW_USER,
	tokPRAGKEYW_VIRTINHERIT,
	tokPRAGKEYW_VTORDISP,
	tokPRAGKEYW_WARNING,
//#if tokPRAGKEYW_WARNING >= tokclsOpMin
//#error To many keywords, adjust tokclsOpMin value
//#endif
	tokDELI_LPAREN = tokclsOpMin,			// ( 
	tokDELI_RPAREN,							// ) 
	tokDELI_LBKT,							// [ 
	tokDELI_RBKT,							// ] 

	tokOP_COMA,								// ,    
	tokOP_DOT,								// .    
	tokOP_DOTSTAR,							// .*    
	tokOP_SCOLON,							// ;    
	tokOP_TILDE	,							// ~    
	tokOP_QUEST,							// ?    

	tokOP_NOT,								// !    
	tokOP_NOTEQ,							// !=   
	tokOP_MOD,								// %    
	tokOP_MODEQ,							// %=   
	tokOP_AND,								// &   
	tokOP_ANDAND,							// &&  
	tokOP_ANDEQ,							// &=  
	tokOP_MUL,								// *   
	tokOP_MULEQ,							// *=  
	tokOP_ENDCOMMENT,						// */  
	tokOP_PLUS,								// +   
	tokOP_INC,								// ++  
	tokOP_PLUSEQ,							// +=  
	tokOP_MINUS,							// -   
	tokOP_DECR,								// --  
	tokOP_MINUSEQ,							// -=  
	tokOP_DIV,								// /   
	tokOP_DIVEQ,							// /=  
	tokOP_BEGINCOMMENT,						// /*  
	tokOP_LT,								// <   
	tokOP_LSHIFT,							// <<  
	tokOP_LEQ,								// <=  
	tokOP_ASGN,								// =   
	tokOP_EQ,								// ==  
	tokOP_GT,								// >   
	tokOP_RSHIFT,							// >>  
	tokOP_GEQ,								// >=  
	tokOP_XOR,								// ^   
	tokOP_XOREQ,							// ^=  
	tokOP_OR,								// |   
	tokOP_OROR,								// ||  
	tokOP_OREQ,								// |=  
	tokOP_COLON,							// :
	tokOP_DCOLON,							// ::    
	tokOP_ARROW,							// ->  
	tokOP_ARROWSTAR,						// ->* 
	tokOP_RSHIFTEQ,							// >>= 
	tokOP_LSHIFTEQ,							// <<= 
	tokDELI_LCBKT = tokclsOpSpecOpenBlock,	// { 
	tokDELI_RCBKT = tokclsOpSpecCloseBlock,	// } 
	tokOP_MAX,								// token op MAX

	tokOP_LINECMT =  tokclsOpSpecLineCmt,	// line comment

	tokIDENTIFIER = tokclsIdentMin,			// identifier
	tokNUMBER = tokclsConstInteger,			// number const
	tokREAL = tokclsConstReal,
	tokSTRING = tokclsConstString,			// string
	tokBEGINSTRING,
	tokCHAR,
	tokSTRINGERR,

	tokCOMMENT = tokclsCommentMin,			// comment

	tokWIZ = tokclsUserMin,					//return this token  if we are in the wizard modified code,
	tokNEWLINE,
	tokFILENAME,							// <filename> after #define
	tokUSERDEFKW,
	tokEOF
};


#define BASED_HINT		0x9000

// token hints
#define ERR			BASED_HINT + 0	// error
#define HWS			BASED_HINT + 1	// white space
#define HST			BASED_HINT + 2	// string
#define HPP			BASED_HINT + 3	// preprocessor
#define HCH			BASED_HINT + 4	// character (')
#define HNU			BASED_HINT + 5	// number
#define HKW			BASED_HINT + 6	// keyword


#define OLP	tokDELI_LPAREN	
#define ORP tokDELI_RPAREN	
#define OLB tokDELI_LBKT		
#define ORB tokDELI_RBKT		
#define OLC tokDELI_LCBKT	
#define ORC tokDELI_RCBKT	
                
#define OCO tokOP_COMA		
#define ODO tokOP_DOT		
#define OSC tokOP_SCOLON		
#define OUN tokOP_TILDE		
#define OQU tokOP_QUEST
#define ONL tokNEWLINE
#define EOS tokEOF

#define BASED_MULOP		0
#define M00		BASED_MULOP + 0
#define M01		BASED_MULOP + 1	
#define M02		BASED_MULOP + 2
#define M03		BASED_MULOP + 3	
#define M04		BASED_MULOP + 4
#define M05		BASED_MULOP + 5	
#define M06		BASED_MULOP + 6
#define M07		BASED_MULOP + 7	
#define M08		BASED_MULOP + 8
#define M09		BASED_MULOP + 9	
#define M0A		BASED_MULOP + 10
#define M0B		BASED_MULOP + 11
#define M0C		BASED_MULOP + 12	
#define M0D		BASED_MULOP + 13
#define BASED_MULOP_MAX	BASED_MULOP + 14			


// the state of lexical analyser 
// ie:
// inComment : we're inside a comment before getting the next token
//				so we need to find end of comment code (*/)	and if
//				we can't find a matching comment then put everything
//				inside the comment.
// inString	 : we're inside a string before getting the next token
//				so we need to find end of string (") or continuation (\)
//				otherwise we need to return an error.
// inCode	 : should parse a regular code
// inWizard	 : should parse wizard modified code
//
// The following states are used only internally (ie: inside lexer)
// since we want to do differently after we encounter a
// inPPIf	 : after the preprocessor #if is encountered but before eol
// inPPPragma: after #pragma is encountered but before eol
// inPPInclude: after #include is encountered but before eol
// 
enum lexstate
{
	inComment       = 0x0001,
	inString        = 0x0002,
	inCode          = 0x0004,
	inPPIf          = 0x0008,
	inPPPragma      = 0x0010,
	inPPInclude     = 0x0020,
	inWizard        = 0x0040,
	inDeclspec      = 0x0080,
	inPreProcessor  = 0x0100,
	inSingleComment = 0x0200,
	inAssume		= 0x0400,
	inPPError       = 0x0800,
	inUuid			= 0x1000,//ODL
};

