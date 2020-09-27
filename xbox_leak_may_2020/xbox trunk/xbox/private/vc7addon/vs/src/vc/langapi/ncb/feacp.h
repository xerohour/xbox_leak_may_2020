#pragma once

#include "IDE_ParseInfo.h"

// NOTE: These prototypes are also defined in cxxfe\sl\p1\c\main.c
// If you change them here, change them there
extern "C" {

//
// FEACP dll entrypoint to parse a source file and populate the NCB with the
// results.  This is called for a file on disk, generally by the IDE when
// creating a new NCB or after a file has been saved in the editor.
//
	int WINAPI DoRealParse(
			NcbParseEx *	pNCBDB,
			IMOD			imod,
			HTARGET			hTarget,
			PYCBCALLBACK	pCallBack,
			int				argc,
			char **			argv
	);


//
// FEACP dll entrypoint to parse a source file and populate the NCB with the
// results.  This is called for a file in memory, generally by the IDE during
// dynamic parsing.
//
	int WINAPI DoRealParseFromBuffer(
			NcbParseEx *	pNCBDB,
			IMOD			imod,
			HTARGET			hTarget,
			PYCBCALLBACK	pCallBack,
			int				argc,
			char **			argv,
			void *			pFileBuf,
			ULONG			cbFileBuf
	);


//
// FEACP dll entrypoint to parse a prototype for a function and then do
// signature matching on that prototype against the NCB, looking for the
// matching IINST, which is returned.
//
	int WINAPI ParseFunction(
			int argc,
			char **			argv,
			void *			pFileBuf,			// [in] ptr to buffer of source 
												//      to compile
			ULONG			cbFileBuf,			// [in] length of source buffer
			BscEx *			pNCBDB,				// [in] NCB store to query for 
												//      type info
			PFNALLOC		pfnalloc,			// [in] memory allocator
			char ***		prgArray,			// [out] array with (retval, 
												//       funcname, [params...])
			ULONG *			puSize,				// [out] size of array
			IINST *			piinst,				// [in/out] IINST for the 
												//          resolved symbol
			DWORD *			pdwFlags,			// [out] E_PARSE_*
			DWORD			dwParseFlags		// [in] PF_*
	);


//
// FEACP dll entrypoint to signature matching on two prototypes from the 
// NCB given the IINST's for the two functions.
//
	int WINAPI SignatureMatch(
			int				argc,
			char **			argv,
			BscEx *			pNCBDB,				// [in] NCB store to query for 
												//      type info
			IINST			iinst1,				// [in] IINST for the first 
												//      symbol
			IINST			iinst2,				// [in] IINST for the second 
												//      symbol
			DWORD *			pdwFlags			// [out] E_PARSE_*
	);

//
// FEACP dll entrypoint to assist autocompletion on the right hand side of a
// -> or . operator.  We are passed the text of the current function.  Parse
// until the points-to expression, then return the type of the left-hand side
// (without pointer) as a string, e.g. given
//
//		void foo(void) {
//			CFoo *pfoo;
//			pfoo-><!>; // cursor position at the <!>
//		}
//
// this will return the string "CFoo".
//
	int WINAPI GetTypeFromBuffer(
			int				argc,
			char **			argv,
			void *			pFileBuf,			// [in] ptr to buffer of source 
												//      to compile
			ULONG			cbFileBuf,			// [in] length of source buffer
			ULONG			ulCaretLoc,			// [in] offset of cursor 
												//      position 
												//      in source buffer
			int				nStartLine,			// [in] line # of start of 
												//      source buffer
			int				nStopLine,			// [in] line # of cursor 
												//      position in source 
												//      buffer
			BscEx *			pNCBDB,				// [in] NCB store to query for 
												//      type info
			char *			szTypeBuf,			// [out] buffer for returning 
												//       type name
			int				cbTypeBuf,			// [in] length of buffer for 
												//      returning type 
			USHORT			targetImod,			// [in] the file where we are trying QI or AC
			ULONG			parentCl			// [in] the class whose inline member function we are in
	);


//
// FEACP dll entrypoint to perform "identifier info" dynamic parsing.  We are
// passed the text of the current function, with the cursor position on some
// identifier.  Parse up to that identifier, then return a string giving the
// type and name (e.g. "CFoo *pfoo").
//
	int WINAPI GetIdentTypeFromBuffer(
			int				argc,
			char **			argv,
			void *			pFileBuf,			// [in] ptr to buffer of source 
												//      to compile
			ULONG			cbFileBuf,			// [in] length of source buffer
			ULONG			ulCaretLoc,			// [in] offset of cursor 
												//      position in source 
												//      buffer
			int 			nStartLine,			// [in] line # of start of 
												//      source buffer
			int 			nStopLine,			// [in] line # of cursor 
												//      position in source 
												//      buffer
			BscEx *			pNCBDB,				// [in] NCB store to query for 
												//      type info
			char *			szTypeBuf,			// [out] buffer for returning 
												//       type name
			int				cbTypeBuf,			// [in] length of buffer for 
												//      returning type 
			USHORT			targetImod,			// [in] the file where we are trying QI or AC
			ULONG			parentCl			// [in] the class whose inline member function we are in
	);


//
// FEACP dll entrypoint to parse a block of text and report errors in there
//
	int WINAPI DoParseForErrors(
			int				argc,
			char **			argv,
			void *			pFileBuf,			// [in] ptr to buffer of source 
												//      to parse for errors
			ULONG			cbFileBuf,			// [in] length of source buffer
			int				nStartLine,			// [in] line # of start of 
												//      source buffer
			int				nStopLine,			// [in] line # of end of source 
												//      buffer
			BscEx *			pNCBDB,				// [in] NCB store to query for 
												//      type info
			PFNREPORTERROR	pfnReportError		// [in] pfn to report errors
	);


//
// FEACP dll entrypoint to parse a block of text and report decl line# info
//
	int WINAPI DoParseTree(
			int				argc,
			char **			argv,
			void *			pFileBuf,			// [in] ptr to buffer of source 
												//      to parse for errors
			ULONG			cbFileBuf,			// [in] length of source buffer
			int				nStartLine,			// [in] line # of start of 
												//      source buffer
			int				nStopLine,			// [in] line # of end of source 
												//      buffer
			BscEx *			pNCBDB,				// [in] NCB store to query for 
												//      type info
			PFNALLOC		pfnAlloc,			// [in] pfn to allocator
			IDE_Decl_t **	ppDeclTree			// [out] ptr to decl tree
	);


//
// FEACP dll entrypoint to report include files in a buffer
//
	int WINAPI DoListIncludesFromBuffer(
			int				argc,
			char **			argv,
			void *			pFileBuf,			// [in] ptr to buffer of source 
												//      to list includes from
			ULONG			cbFileBuf,			// [in] length of source buffer
			int				nStartLine,			// [in] line # of start of 
												//      source buffer
			int				nStopLine,			// [in] line # of end of source 
												//      buffer
			PFNREPORTINC	pfnReportInclude	// [in] pfn to report an 
												//      include file
	);


//
// FEACP dll entrypoint to report include files for current file
//
	int WINAPI DoListIncludes(
			int				argc,
			char **			argv,
			PFNREPORTINC	pfnReportInclude	// [in] pfn to report an 
												//      include file
	);


//
// FEACP entry point to parse for reference information in a file.
//
	int WINAPI DoReferenceParse(
			NcbParseEx *	pNcbParseEx,
			BscEx *			pBscEx,
			IMOD 			imod,
			PYCBCALLBACK 	pCallBack,
			int 			argc,
			char **			argv
	);


//
// FEACP entry point to parse for reference information from a buffer.
//
	int WINAPI DoReferenceParseFromBuffer (
			NcbParseEx *	pNcbParseEx,
			BscEx *			pBscEx,
			IMOD			imod,
			PYCBCALLBACK	pCallBack,
			int				argc,
			char **			argv,
			void *			pFileBuf,			// buffer of source to parse
			ULONG			cbFileBuf			// length of source buffer
	);


//
// FEACP dll entrypoint to assist autocompletion when not in a points-to
// expression.  We are passed the text of the current function.  Parse up to
// the cursor position, then return a list of all in-scope local variables and
// types collected in an NcbNameMap.
//
	int WINAPI GetLocalsInScopeFromBuffer(
			int				argc,
			char **			argv,
			void *			pFileBuf,		// [in] ptr to buffer of source to 
											//      compile
			ULONG			cbFileBuf,		// [in] length of source buffer
			ULONG			ulCaretLoc,		// [in] offset of cursor position 
											//      in source buffer
			int				nStartLine,		// [in] line # of start of source 
											//      buffer
			int				nStopLine,		// [in] line # of cursor position 
											//      in source buffer
			BscEx *			pNCBDB,			// [in] NCB store to query for type 
											//      info
			NcbNameMap **	ppNcbNmp		// [out] NameMap for returning all 
											//       locals in scope
	);

};


