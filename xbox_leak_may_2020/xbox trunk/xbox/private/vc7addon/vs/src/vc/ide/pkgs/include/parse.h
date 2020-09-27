#pragma once

#include "lex.h"
#include "ncparex.h"
#include "vccolls.h"

#define PARSE_NOERROR				0
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
#define E_PARSE_CTOR_RET_TYPE       11
#define E_PARSE_DUP_ACCESS_SPEC     12
#define E_PARSE_INVALID_MODIFIER    13
#define E_PARSE_MACROERROR			14
#define E_PARSE_PARAM_COUNT_MISMATCH 15

#define PARSE_TYP_FUNC			0x00000001
#define PARSE_TYP_VAR			0x00000002
#define PARSE_TYP_PURE			0x00000004
#define PARSE_TYP_STATIC		0x00000008
#define PARSE_TYP_VIRTUAL		0x00000010
#define PARSE_TYP_EXTERN		0x00000020
#define PARSE_TYP_INLINE		0x00000040
#define PARSE_TYP_CONSTFUNC		0x00000080
#define PARSE_TYP_MAPENTRY      0x00000100

//////////////////////////////////////////////////////////////
enum parsetype 
{
	functionType = 0x0001,
	classType = 0x0002,
	varType = 0x0004,
	templateType = 0x0008,
	typedefType = 0x0010,
	unionType = 0x0020,
	enumType = 0x0040,
	structType = 0x0080,
	fileType = 0x0100,
	msgmapType = 0x0200,
	mapitemType = 0x0400,
	unknownType = 0x0800,
};

enum decldefn
{
	decl,
	defn
};

enum Action
{
	noOperation = -1, //REVIEW: to be compatible with old enum
	initFile,	
	addFile,		
	updateFile,
	delFile,
	initDone,
	reparseTarget,
	updateFileFromBuffer,
	forceReparse,
	squigglyInfo,
	includeFileParse,
    refParse,
    addOpenStore,
    delUnrefMods,
    delTarget
};

class CParser
{
private :

	////////////////////////////////////////////////////////////////////////////////////		
	// this struct is used to set the ordering of include files based on line number
	//
	struct CLineInfo
	{
		USHORT m_index;
		LINE m_lineno;
	};

protected :

	////////////////////////////////////////////////////////////////////////////////////
	// used by qsort to sort CLineInfo
	////////////////////////////////////////////////////////////////////////////////////
	static int CompareLineNo(const void * elem1, const void * elem2);
	void rgIinstToStrList (IINST * rgIinst, ULONG cinst, CVCStringList & slIncl, NcbParseEx * pDBase);


public:	 
	CParser() {};
	~CParser() {};
	// initialize parser w/ specific filename
	// or should it be a pointer to a Doc ?
	virtual BOOL Init (LPCSTR szFilename, void * pTextDocs) PURE;
	virtual BOOL CanAutoComplete() PURE;
	// parsing the file we just opened
	virtual BOOL Parse (LPCSTR szFilename, void * pTextDoc, Action action, HTARGET hTarget, CLex * lexer, BOOL fComPlus, const WCHAR *wszIncDirsAddtl, NcbParse * pDBase, 
		BOOL bParseIncludeFiles, BOOL bPrjSource, BOOL & bSuspendThread) PURE;
	virtual BOOL ParseForAutoComplete (void *pACInfo, BscEx* pDBase) PURE;
	virtual BOOL ParseFunction (CLex * pLex, LPCSTR ptchLine, UINT cbLine, PLXS plxs, CVCStringList & slFunc) PURE;
	virtual BOOL ParseFunction (CLex * pLex, LPCSTR ptchLine, UINT cbLine, PLXS plxs, CVCStringList & slFunc, BOOL & bFunction) PURE;
	virtual UINT ParseFunction (CLex * pLex, LPCSTR ptchLine, UINT cbLine, PLXS plxs, CVCStringList & slFunc, DWORD & dwFlags, UINT & iPos) PURE;
	// closing the file
	virtual void Close() PURE;
	// loading the macros
	// virtual LoadMacros (LPCSTR pszFile) PURE;
	virtual CParser * Duplicate() PURE;
	virtual void Release() PURE;


protected:
	BOOL BeforeUpdate(IMOD imod, NcbParseEx *pDBase);
	BOOL AfterUpdate(IMOD imod, NcbParseEx *pDBase, BOOL *pfIncludesChanged);
};

typedef DWORD	PARSETYPE;

struct ParseInfo
{
public:
	ATR32		m_atr;
	TYP			m_typ;
	BOOL		m_bIsDefinition;
	CString		m_strName	;
	CString		m_strType	;
	CString		m_strParam	;
	int			m_iNumParam ;
	int			m_iLineStart;
	int			m_iLineEnd	;
	CString		m_strFilename;
};


struct TokenInfo
{
	UINT		m_tok;
	CString		m_strToken;
};
