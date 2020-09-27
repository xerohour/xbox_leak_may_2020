#ifndef __FELOAD_H__
#define __FELOAD_H__

#include "dllmap.h"
#include "IDE_ParseInfo.h"

BEGIN_DLLNAME_MAP(CFELoader)
	FUNCTION_ITEM("_DoRealParse@24")
	FUNCTION_ITEM("_DoRealParseFromBuffer@32")
	FUNCTION_ITEM("_ParseFunction@44")
	FUNCTION_ITEM("_SignatureMatch@24")
	FUNCTION_ITEM("_GetTypeFromBuffer@48")
	FUNCTION_ITEM("_GetIdentTypeFromBuffer@48")
	FUNCTION_ITEM("_DoParseForErrors@32") 
	FUNCTION_ITEM("_DoParseTree@36")
	FUNCTION_ITEM("_DoListIncludesFromBuffer@28")
	FUNCTION_ITEM("_DoListIncludes@12")
	FUNCTION_ITEM("_DoReferenceParse@24")
	FUNCTION_ITEM("_DoReferenceParseFromBuffer@32")
	FUNCTION_ITEM("_GetLocalsInScopeFromBuffer@36")
END_DLLNAME_MAP()

class CFELoader : CDllWrapper {
	INIT_WRAPPER (CFELoader);

public:
	
	// Typedef definitions
//	typedef int (FAR WINAPI *ENTRYPROC) (NcbParse *, IMOD, HTARGET, void *, int, char **);
	
//	typedef int (FAR WINAPI *ENTRYBUFFERPROC) (NcbParse *, IMOD, HTARGET, void *, int, char **, LPCCH, ULONG);

	typedef int (FAR WINAPI *tdefDoRealParse) (NcbParse *, IMOD, HTARGET, void *, int, char **);

	typedef int (FAR WINAPI *tdefDoRealParseFromBuffer) (NcbParse *, IMOD, HTARGET, void *, int, char **, void *, ULONG);

	typedef int (FAR WINAPI *tdefParseFunction) (int, char **, void *, ULONG, BscEx *, PFNALLOC, char ***, ULONG *, IINST *, DWORD *, DWORD);

	typedef int (FAR WINAPI *tdefGetTypeFromBuffer) (int argc, char **, void *, ULONG, ULONG, ULONG, ULONG, BscEx *, char *, int, USHORT, ULONG);

	typedef int (FAR WINAPI *tdefSignatureMatch) (int, char **, BscEx *, IINST, IINST, DWORD *);	

	typedef int (FAR WINAPI *tdefGetIdentTypeFromBuffer) (int, char **, void *, ULONG, ULONG, ULONG, ULONG, BscEx *, char *, int, USHORT, ULONG);

	typedef int (FAR WINAPI *tdefDoParseForErrors) (int, char **, void *, ULONG, int, int, BscEx *, PFNREPORTERROR);
	
	typedef int (FAR WINAPI *tdefDoParseTree) (int, char **, void *, ULONG, int, int, BscEx *, PFNALLOC, IDE_Decl_t **);
	
	typedef int (FAR WINAPI *tdefDoListIncludesFromBuffer) (int, char **, void *, ULONG, int, int, PFNREPORTINC);
	
	typedef int (FAR WINAPI *tdefDoListIncludes) (int, char **, PFNREPORTINC);
	
	typedef int (FAR WINAPI *tdefDoReferenceParse) (NcbParse *, BscEx *, IMOD, void *, int, char **);
	
	typedef int (FAR WINAPI *tdefDoReferenceParseFromBuffer) (NcbParse *, BscEx *, IMOD, void *, int, char **, void *, ULONG);

	typedef int (FAR WINAPI *tdefGetLocalsInScopeFromBuffer)(int, char **, void *, ULONG, ULONG, int, int, BscEx *, NcbNameMap **);



	
	// Member Functions
	int DoRealParse (NcbParse *pNCBDB, IMOD imod, HTARGET hTarget, PYCBCALLBACK pfnIncludeInProject, int argc, char **argv)
	{
		FUNC_PROLOGUE();
		return tdefDoRealParse(FuncMap(0)) (pNCBDB, imod, hTarget, pfnIncludeInProject, argc, argv);
	};

	
	int DoRealParseFromBuffer (NcbParse *pNCBDB, IMOD imod, HTARGET hTarget, PYCBCALLBACK pfnIncludeInProject, int argc, char **argv, void *szBuf, ULONG cbBuf) 
	{
		FUNC_PROLOGUE();
		return tdefDoRealParseFromBuffer (FuncMap(1)) (pNCBDB, imod, hTarget, pfnIncludeInProject, argc, argv, szBuf, cbBuf);
	};

	
	int ParseFunction (
		int				argc,
		char**			argv,
		void*			pFileBuf,		// [in] ptr to buffer of source to compile
		ULONG			cbFileBuf,		// [in] length of source buffer
		BscEx*			pNCBDB,			// [in] NCB store to query for type info
		PFNALLOC		pfnalloc,		// [in] memory allocator
		char***			prgArray,		// [out] array with (retval, funcname, [params...])
		ULONG*			puSize,			// [out] size of array
		IINST*			piinst,			// [in/out] IINST for the resolved symbol
		DWORD*			pdwFlags,		// [out] E_PARSE_*
		DWORD			dwParseFlags)	// [in] PF_*
	{
		FUNC_PROLOGUE();
		return tdefParseFunction (FuncMap(2)) (argc, argv, pFileBuf, cbFileBuf, pNCBDB, pfnalloc, prgArray, puSize, piinst, pdwFlags, dwParseFlags);
	};

	
	int SignatureMatch(
		unsigned argc, 
		char ** argv, 
		BscEx* pNCB, 
		IINST iinst1, 
		IINST iinst2, 
		DWORD* pdwFlags)
	{
		FUNC_PROLOGUE();
		return tdefSignatureMatch (FuncMap(3)) (argc, argv, pNCB, iinst1, iinst2, pdwFlags);
	};

	
	int GetTypeFromBuffer(
		int				argc,
		char **			argv,
		void *			pFileBuf,		// [in] ptr to buffer of source to compile
		ULONG			cbFileBuf,		// [in] length of source buffer
		ULONG			ulCaretLoc,		// [in] offset of cursor position in source buffer
		ULONG 			ulFuncStart,	// [in] line # of start of source buffer
		ULONG			ulAnchorLine,	// [in] line # of cursor position in source buffer
		BscEx*			pNcbStore,		// [in] already-open NCB store
		char*			szBuffer,
		int				cbBuffer,
		USHORT			targetImod,		// [in] the file where we are trying QI or AC
		ULONG			parentCl)		// [in] the class whose inline member function we are in
	{
		FUNC_PROLOGUE();
		return tdefGetTypeFromBuffer (FuncMap(4)) (argc, argv, pFileBuf, cbFileBuf, ulCaretLoc, ulFuncStart, ulAnchorLine, pNcbStore, szBuffer, cbBuffer, targetImod, parentCl);
	};

	
	int GetIdentTypeFromBuffer(
		int				argc,
		char**			argv,
		void *			pFileBuf,		// [in] ptr to buffer of source to compile
		ULONG			cbFileBuf,		// [in] length of source buffer
		ULONG			ulCaretLoc,		// [in] offset of cursor position in source buffer
		ULONG 			ulFuncStart,	// [in] line # of start of source buffer
		ULONG			ulAnchorLine,	// [in] line # of cursor position in source buffer
		BscEx*			pNcbStore,		// [in] already-open NCB store
		char*			szBuffer,
		int				cbBuffer,
		USHORT			targetImod,		// [in] the file where we are trying QI or AC
		ULONG			parentCl)		// [in] the class whose inline member function we are in
	{
		FUNC_PROLOGUE();
		return tdefGetIdentTypeFromBuffer (FuncMap(5)) (argc, argv, pFileBuf, cbFileBuf, ulCaretLoc, ulFuncStart, ulAnchorLine, pNcbStore, szBuffer, cbBuffer, targetImod, parentCl);
	};

	
	int DoParseForErrors (
		int				argc,
		char**			argv,
		void *			pFileBuf,		// [in] ptr to buffer of source to parse for errors
		ULONG			cbFileBuf,		// [in] length of source buffer
		int 			nStartLine,		// [in] line # of start of source buffer
		int 			nStopLine,		// [in] line # of end of source buffer
		BscEx*			pNCBDB,			// [in] NCB store to query for type info
		PFNREPORTERROR pfnReportError)	// [in] pfn to report errors
	{

		FUNC_PROLOGUE();
		return tdefDoParseForErrors (FuncMap(6)) (argc, argv, pFileBuf, cbFileBuf, nStartLine, nStopLine, pNCBDB, pfnReportError);
	};

	
	int DoParseTree(
		int				argc,
		char**			argv,
		void*			pFileBuf,		// [in] ptr to buffer of source to parse
		ULONG			cbFileBuf,		// [in] length of source buffer
		int 			nStartLine,		// [in] line # of start of source buffer
		int 			nStopLine,		// [in] line # of end of source buffer
		BscEx*			pNCBDB,			// [in] NCB store to query for type info
		PFNALLOC		pfnAlloc,		// [in] pfn to allocator
		IDE_Decl_t**	ppDeclTree)		// [out] ptr to decl tree
	{

		FUNC_PROLOGUE();
		return tdefDoParseTree (FuncMap(7)) (argc, argv, pFileBuf, cbFileBuf, nStartLine, nStopLine, pNCBDB, pfnAlloc, ppDeclTree);
	};
	
	
	int DoListIncludesFromBuffer(
		int				argc,
		char**			argv,
		void *			pFileBuf,		// [in] ptr to buffer of source to parse
		ULONG			cbFileBuf,		// [in] length of source buffer
		int 			nStartLine,		// [in] line # of start of source buffer
		int 			nStopLine,		// [in] line # of end of source buffer
		PFNREPORTINC	pfnReportInc)	// [in] pfn to report includes
	{
		FUNC_PROLOGUE();
		return tdefDoListIncludesFromBuffer (FuncMap(8)) (argc, argv, pFileBuf, cbFileBuf, nStartLine, nStopLine, pfnReportInc);
	};

	
	int DoListIncludes(
		int				argc,
		char**			argv,
		PFNREPORTINC	pfnReportInc)	// [in] pfn to report includes
	{
		FUNC_PROLOGUE();
		return tdefDoListIncludes (FuncMap(9)) (argc, argv, pfnReportInc);
	};


	int DoReferenceParse (NcbParse *pNCBDB, BscEx *pBscEx, IMOD imod, PYCBCALLBACK pfnIncludeInProject, int argc, char **argv)
	{
		FUNC_PROLOGUE();
		return tdefDoReferenceParse(FuncMap(10)) (pNCBDB, pBscEx, imod, pfnIncludeInProject, argc, argv);
	};


	int DoReferenceParseFromBuffer (NcbParse *pNCBDB, BscEx *pBscEx, IMOD imod, PYCBCALLBACK pfnIncludeInProject, int argc, char **argv, void *szBuf, ULONG cbBuf)
	{
		FUNC_PROLOGUE();
		return tdefDoReferenceParseFromBuffer (FuncMap(11)) (pNCBDB, pBscEx, imod, pfnIncludeInProject, argc, argv, szBuf, cbBuf);
	};

	int GetLocalsInScopeFromBuffer(int argc, char ** argv, void * pFileBuf, ULONG cbFileBuf, ULONG ulCaretLoc, int nStartLine, int nStopLine, BscEx * pNCBDB, NcbNameMap **	ppNcbNmp)
	{
		FUNC_PROLOGUE();
		return tdefGetLocalsInScopeFromBuffer (FuncMap(12)) (argc, argv, pFileBuf, cbFileBuf, ulCaretLoc, nStartLine, nStopLine, pNCBDB, ppNcbNmp);
	};
};

#endif
