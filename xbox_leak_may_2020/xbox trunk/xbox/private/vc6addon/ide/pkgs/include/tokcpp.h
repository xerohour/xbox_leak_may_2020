// tokcpp.h - tokens for C++
//
// Include lex.h before including this file.
//

#ifndef __TOKCPP_H__
#define __TOKCPP_H__

enum TOKCPP {	
	tokUNKNOWN = tokclsError,
	tokKEYW_AUTO = tokclsKeyWordMin,
	tokKEYW_BREAK,
	tokKEYW_BOOL,
	tokKEYW_CASE,
	tokKEYW_CATCH,
	tokKEYW_CHAR,
	tokKEYW_CLASS,
	tokKEYW_CONST,
	tokKEYW_CONST_CAST,
	tokKEYW_CONTINUE,
	tokKEYW_DEFAULT,
	tokKEYW_DELETE,
	tokKEYW_DO,
	tokKEYW_DOUBLE,
	tokKEYW_DYNAMIC_CAST,
	tokKEYW_ELSE,
	tokKEYW_ENUM,
	tokKEYW_EXPLICIT,
	tokKEYW_EXTERN,
	tokKEYW_FALSE,
	tokKEYW_FLOAT,
	tokKEYW_FOR,
	tokKEYW_FRIEND,
	tokKEYW_GOTO,
	tokKEYW_IF,
	tokKEYW_INLINE,
	tokKEYW_INT,
	tokKEYW_INTERFACE,
	tokKEYW_LONG,
	tokKEYW_MUTABLE,
	tokKEYW_NAMESPACE,
	tokKEYW_NEW,
	tokKEYW_OPERATOR,
	tokKEYW_PASCAL,
	tokKEYW_PRIVATE,
	tokKEYW_PROTECTED,
	tokKEYW_PUBLIC,
	tokKEYW_REGISTER,
	tokKEYW_REINTERPRET_CAST,
	tokKEYW_RETURN,
	tokKEYW_SHORT,
	tokKEYW_SIGNED,
	tokKEYW_SIZEOF,
	tokKEYW_STATIC,
	tokKEYW_STATIC_CAST,
	tokKEYW_STRUCT,
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
	tokKEYW__ASM,
	tokKEYW__ASSUME,
	tokKEYW__BASED,
	tokKEYW__CDECL,
	tokKEYW__DECLSPEC,
	tokKEYW__EXCEPT,
	tokKEYW__FINALLY,
	tokKEYW__FASTCALL,
	tokKEYW__FORCEINLINE,
	tokKEYW__INLINE,
	tokKEYW__INT16,
	tokKEYW__INT32,
	tokKEYW__INT64,
	tokKEYW__INT8,
	tokKEYW__LEAVE,
	tokKEYW__MULTINHERIT,
	tokKEYW__PASCAL,
	tokKEYW__SINGINHERIT,
	tokKEYW__STDCALL,
	tokKEYW__TRY,
	tokKEYW__UNALIGNED,
	tokKEYW__UUIDOF,
	tokKEYW__VIRTINHERIT,
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
	tokPPKEYW_MAX,			/* max Preprocessor keyword */	

	tokDECLKEYW_ALLOCATE,	/* keywords following _declspec */
	tokDECLKEYW_DLLEXPORT,
	tokDECLKEYW_DLLIMPORT,
	tokDECLKEYW_NAKED,
	tokDECLKEYW_NOTHROW,
	tokDECLKEYW_NOVTABLE,
	tokDECLKEYW_PROPERTY,
	tokDECLKEYW_SELECTANY,
	tokDECLKEYW_THREAD,
	tokDECLKEYW_UUID,
	
	tokASSUMEKEYW_EVAL,		// keywords following __assume
	tokASSUMEKEYW_NOEVAL,

	tokPPIFKEYW_DEFINED, 	/* keyword following #if */

	tokPRAGKEYW_ALLOCTEXT,	/* keywords following #pragma */    
	tokPRAGKEYW_AUTOINLINE,
	tokPRAGKEYW_BESTCASE,
	tokPRAGKEYW_CHKPTR,
	tokPRAGKEYW_CHKSTACK,
	tokPRAGKEYW_CODESEG,
	tokPRAGKEYW_COMMENT,
	tokPRAGKEYW_COMPILER,
	tokPRAGKEYW_DATASEG,
	tokPRAGKEYW_DEFAULT,
	tokPRAGKEYW_DISABLE,
	tokPRAGKEYW_ERROR,
	tokPRAGKEYW_EXESTR,
	tokPRAGKEYW_FULLGENERALITY,
	tokPRAGKEYW_FUNCTION,
	tokPRAGKEYW_HDRSTOP,
	tokPRAGKEYW_INITSEG,
	tokPRAGKEYW_INLINEDEPTH,
	tokPRAGKEYW_INLINERECURSE,
	tokPRAGKEYW_INSTRINSIC,
	tokPRAGKEYW_LIB,
	tokPRAGKEYW_LINESIZE,
	tokPRAGKEYW_LOOPOPT,
	tokPRAGKEYW_MESSAGE,
	tokPRAGKEYW_MULINHERIT,
	tokPRAGKEYW_NATIVECALL,
	tokPRAGKEYW_OFF,
	tokPRAGKEYW_ON,
	tokPRAGKEYW_ONCE,
	tokPRAGKEYW_OPTIMIZE,
	tokPRAGKEYW_PACK,
	tokPRAGKEYW_PAGE,
	tokPRAGKEYW_PAGESIZE,
	tokPRAGKEYW_PTRTOMEMBERS,
	tokPRAGKEYW_POP,
	tokPRAGKEYW_PUSH,
	tokPRAGKEYW_SAMESEG,
	tokPRAGKEYW_SETLOCALE,
	tokPRAGKEYW_SINGINHERIT,
	tokPRAGKEYW_SKIP,
	tokPRAGKEYW_STRPOOL,
	tokPRAGKEYW_SUBTITLE,
	tokPRAGKEYW_TITLE,
	tokPRAGKEYW_USER,
	tokPRAGKEYW_VIRTINHERIT,
	tokPRAGKEYW_VTORDISP,
	tokPRAGKEYW_WARNING,
       
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

#endif // __TOKCPP_H__
