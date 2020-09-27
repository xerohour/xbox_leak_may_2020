//
// COptionTable
//
// Option parsing and re-generation using property bag values.
//
// [matthewt]
//				   

#ifndef _INCLUDE_OPTIONTBL_H										
#define _INCLUDE_OPTIONTBL_H   

class COptionHandler;

// highly efficient manipulation of option string lists
// o uses map for quick check to see if exists
// o uses low-level mem. copy and compare.
// o CProjItem string list prop. aware

// are we the add or subtract component of a pseudo string list prop?
#define ListStr_AddPart(prop) (prop + 1)
#define ListStr_SubPart(prop) (prop + 2)

// NOTE: no duplicates are allowed in the option list

typedef struct tagOptEl {
	BYTE cref;		// ref. count
	int cch;		// # of chars. in pch (incl. '\0')
	TCHAR pch[1];	// space for single terminator
} OptEl;

class  COptionList : public CObject
{
public:
	// ConstrucTORs
	COptionList(TCHAR chJoin = _T(','), BOOL fUseDefJoins = TRUE, BOOL fAllowDupes = FALSE)
		{m_cchStr = 0;m_bAllowDuplicates = fAllowDupes; SetJoinUsage(chJoin, fUseDefJoins);}

	COptionList(const TCHAR * pch, TCHAR chJoin = _T(','), BOOL fUseDefJoins = TRUE, BOOL fAllowDupes = FALSE)
		{m_cchStr = 0;m_bAllowDuplicates = fAllowDupes; SetJoinUsage(chJoin, fUseDefJoins); SetString(pch);}

	__inline void SetJoinUsage(TCHAR chJoin = _T(','), BOOL fUseDefJoins = TRUE)
		{m_strJoin = (CString)chJoin + (fUseDefJoins ? _TEXT(",;\t") : _TEXT(""));}

	// DestrucTOR
	virtual ~COptionList()
		{Empty();}

	void Empty();										// empty
	void GetString(CString & str);						// conversion
	void SetString(const TCHAR * pch);					// conversion
	__inline const COptionList & operator=(COptionList & optlst)
		{Empty(); Append(optlst); return *this;}		// assignment
	void Append(const TCHAR * pch, int cch);			// element append
	void Append(COptionList & optlst);					// list append
	BOOL Subtract(const TCHAR * pch);					// element subtraction (returns FALSE if it didn't exist)
	BOOL Subtract(COptionList & optlst);				// list subtraction (returns FALSE if none existed)
	void Common(COptionList & optlst);					// retrieve common part
	__inline BOOL Exists(const TCHAR * pch)				// existance?
		{VCPOSITION posOurWord; return m_bAllowDuplicates? FALSE : m_mapStrToPtr.Lookup(pch, (void *&)posOurWord);}

	// enumeration
	__inline VCPOSITION GetHeadPosition()
		{return m_lstStr.GetHeadPosition();}
	__inline TCHAR * GetNext(VCPOSITION & pos)
		{return ((OptEl *)m_lstStr.GetNext(pos))->pch;}
	__inline int GetCount()
		{return (int)m_lstStr.GetCount();}

	// form the component parts from 'pch' based on 'this'.
	void Components(const TCHAR * pch, COptionList & optlstAdd, COptionList &optlstSub);

protected:
	// addition/deletion of elements using ref. counting
	void Add(OptEl * poptel);
	BOOL Delete(OptEl * poptel, VCPOSITION posToDelete = (VCPOSITION)NULL);	// if VCPOSITION known pls. use

	CString m_strJoin;					// our join chars
	CMapStringToPtr	m_mapStrToPtr;		// our hash lookup
	CVCPtrList m_lstStr;				// our actual list
	int m_cchStr;						// total length of string (bytes, incl. join char. seps.)
	BOOL m_bAllowDuplicates;
};

// some static buffer sizes
#define MAX_OPT_ARGS	3		// max. of two args. per option

// special IDOPTs that refer to the collection of unknown options
// and unknown strings repsectively, these may be placed in the
// option table 
// these are reserved, start your own IDOPTs from IDOPT_BASE
#define IDOPT_UNKNOWN_OPTION		(UINT)0
#define IDOPT_UNKNOWN_STRING		(UINT)1
#define IDOPT_DERIVED_OPTHDLR		(UINT)2
#define IDOPT_BASE					(UINT)3

// our tool option tables
#define OLD_OPTION_HANDLER(tool) COptHdlr##tool

// I'd like to allocate chunks of memory to CString's internal buffer so that
// a character append will be inexpensive (the generation of an option string does
// this a lot!).
#define OPTSTR_ALLOC_CHUNK	64	// 64 bytes at a time (N.B. must be a power of 2)
#define OPTSTR_ALLOC_EXPON	6	// 64 == 2 ^ 6

__inline void AdjustBuffer(CString & str)
{
	register unsigned int cch = (unsigned int)str.GetLength(); // # chars (will be zero, for initial alloc.)
	
	// are we at a length boundary?
	if ((cch & (OPTSTR_ALLOC_CHUNK-1)) == 0)	// check for no remainder .. can do logical AND if power of 2
	{
		// allocate a new chunk
		(void) str.GetBuffer((int)OPTSTR_ALLOC_CHUNK * ((cch >> OPTSTR_ALLOC_EXPON) + 1));	// set buffer length
		(void) str.ReleaseBuffer(cch);				// set data length, and terminate
	}
}
 
// what type is the option?
typedef enum {single, multiple} OptType;

// what sub-type is the option prop?
typedef enum {boolST, intST, hexST, octST, strST, pathST, dirST} SUBTYPE;

class CDefOptionProp : public CObject
{
public:
	CDefOptionProp() { m_pprop = NULL; }
	~CDefOptionProp() { if (m_pprop != NULL) CProp::DeleteCProp(m_pprop); }

	CDefOptionProp(WORD idProp, int n, SUBTYPE st = intST, OptType ot = single);

	CDefOptionProp(WORD idProp, const char * sz, SUBTYPE st = strST, OptType ot = single);

	CProp *	m_pprop;
	OptType m_opttype;
	SUBTYPE subtypeprop;
};

// our def'n of an option
struct COptStr
{
	UINT			idOption;
	const char *	szOption;
	UINT			rgidArg[MAX_OPT_ARGS];
	OptType			type;
};

// definition of the COptionLookup class
class  COptionLookup : public CObject
{
public:
	~COptionLookup();

	// perform init. now
	BOOL FInit(COptStr *);

	// lookup the option based on ID
	__inline COptStr * Lookup(UINT nID);

	// lookup the option based on first char.
	__inline COptStr * LookupFirst(unsigned char ch, BOOL fCaseSensitive = TRUE);
	__inline COptStr * LookupNext(INT_PTR cchLead);

private:
	// used for lookup 'based on ID'
	UINT		m_idLowest;
	CVCPtrArray	m_ptrIDArray;

	// used for lookup 'based on first char.
	unsigned char	m_chLookup;
	UINT			m_ioptstr;
	CVCPtrArray		m_ptrChArray;
	CVCMapWordToPtr	m_mapChToPtr;

};

// used to optimize lookups for parsing and generation

typedef struct {
	COptStr *	poptstr;
	UINT		ioptstr;
} CLookupEntry;

// I'd like GetAt to return the zero terminator if the index
// is >= GetLength()!
#define NewGetAt(str, ich) ((int)(ich) >= str.GetLength() ? '\0' : str.GetAt((int)(ich)))

// convertion of depth based ids
#define OHStripDepth(x) ((x) & 0x0fff)
#define OHAddDepth(x, d) ((x) | (0x1000 * (d)))
#define OHGetDepth(x) (((x) & 0xf000) / 0x1000)

#define _MAX_SLOB_STACK	5

class  COptionHandler : public CBldSysCmp
{
	DECLARE_DYNAMIC(COptionHandler);

public:
	COptionHandler(CBuildTool * pBuildTool, COptionHandler * popthdlrBase = (COptionHandler *)NULL);
	COptionHandler(const TCHAR * szPkg, WORD id,
				   const TCHAR * szPkgTool, WORD idTool,
				   const TCHAR * szPkgBase, WORD idBase);
	~COptionHandler();

	// called by ctor, dtors of derived classes
	virtual BOOL Initialise();

	// create our default option map
	virtual void CreateOptDefMap() {}

	// retrieve our 'special' option props, ie. the unknown strins and options prop IDs (logical)
	virtual void GetSpecialLogicalOptProps(UINT & nIDUnkOpt, UINT & nIDUnkStr, OptBehaviour optbeh = OBNone) {VSASSERT(FALSE, "GetSpecialLogicalOptProps must be overridden");}

	// retrieive our min. and max. option prop IDs (logical)
	virtual void GetMinMaxLogicalOptProps(UINT & nIDMinProp, UINT & nIDMaxProp) {VSASSERT(FALSE, "GetMinMaxLogicalOptProps must be overridden");}

	// retrieve our 'special' option props, ie. the unknown strins and options prop IDs (actual)
	void GetSpecialOptProps(UINT & nIDUnkOpt, UINT & nIDUnkStr, OptBehaviour optbeh = OBNone);

	// case sensitive option string table?
	virtual const BOOL IsCaseSensitive() {VSASSERT(FALSE, "IsCaseSensitive must be overridden"); return TRUE;}

	// what CSlob do we (and all of our base option handlers) refer to ?
 	__inline void SetSlob(CSlob * pSlob, BOOL fRemember = TRUE)
	{
		COptionHandler * popthdlr = GetBaseOptionHandler();
		if (popthdlr != NULL)
			popthdlr->SetSlob(pSlob, fRemember);

		// fRemember if TRUE pushes old onto stack
		if (fRemember)
			m_stkOldSlobs[m_sStk++] = m_pSlob;

		VSASSERT(m_sStk <= 5, "stack size too big");
		m_pSlob = pSlob;
	}

	__inline CSlob* GetSlob() { return m_pSlob; }
	__inline void ResetSlob()
	{
		COptionHandler * popthdlr = GetBaseOptionHandler();
		if (popthdlr != NULL)
			popthdlr->ResetSlob();

		// make sure the SetSlob & ResetSlob calls are matched
		VSASSERT(m_sStk > 0, "Stack size not big enough");
		m_pSlob = m_stkOldSlobs[--m_sStk];
	}
	
	// retrieve our base option handler (if we have one, by default we don't)
	__inline COptionHandler * GetBaseOptionHandler()	{return m_popthdlrBase;}

	// retrieve our root option handler
	__inline COptionHandler * GetRootOptionHandler()
	{
		COptionHandler * popthdlr = GetBaseOptionHandler();
		if (popthdlr == (COptionHandler *)NULL)
			return this;	// we are the root!
		return popthdlr->GetRootOptionHandler();
	}

	// retrieve our option string table
	virtual COptStr * GetOptionStringTable()	{VSASSERT(FALSE, "GetOptionStringTable must be overridden");return (COptStr *)NULL;}

	// get our option lookup
	virtual COptionLookup * GetOptionLookup()	{VSASSERT(FALSE, "GetOptionLookup must be overridden");return (COptionLookup *)NULL;}

	// set out 'base' prop value
	__inline void SetOptPropBase(UINT nPropBase) {m_nPropBase = nPropBase;}

	// get out 'actual' minimum and maximum props
	__inline void GetMinMaxOptProps(UINT & nIDMinProp, UINT & nIDMaxProp)
	{
		nIDMinProp = m_nPropBase; nIDMaxProp = m_nPropBase + GetOptPropRange() - 1;
	}


	// get our 'range' of props required
	__inline const UINT GetOptPropRange()
	{
		UINT nPropMin, nPropMax; GetMinMaxLogicalOptProps(nPropMin, nPropMax);
		return nPropMax - nPropMin + 1;
	}

	// get our 'actual' prop from logical prop
	__inline UINT MapLogical(UINT nOptProp)
	{
		UINT nPropMin, nPropMax; GetMinMaxLogicalOptProps(nPropMin, nPropMax);
		return m_nPropBase + OHStripDepth(nOptProp - nPropMin);
	}

	// get our 'logical' prop from actual prop
 	__inline UINT MapActual(UINT nOptProp)
	{
		UINT nPropMin, nPropMax; GetMinMaxLogicalOptProps(nPropMin, nPropMax);
		return OHAddDepth(nOptProp + nPropMin - m_nPropBase, m_cDepth);
	}

	__inline UINT GetDepth() {return m_cDepth;}

	__inline UINT FirstOptProp()
	{
		UINT nPropMin, nPropMax; GetMinMaxLogicalOptProps(nPropMin, nPropMax);
		return m_nPropBase + OHStripDepth(nPropMin);
	}

	__inline UINT LastOptProp()
	{
		UINT nPropMin, nPropMax; GetMinMaxLogicalOptProps(nPropMin, nPropMax);
		return m_nPropBase + OHStripDepth(nPropMax);
	}

	//
	// add default tool option properties to our map
	//
	void AddDefStrProp(UINT idProp, const char * sz, OptType ot = single);

	void AddDefPathProp(UINT idProp, const char * sz, OptType ot = single);

	void AddDefDirProp(UINT idProp, const char * sz, OptType ot = single);

	void AddDefIntProp(UINT idProp, int n, OptType ot = single);

	void AddDefBoolProp(UINT idProp, BOOL b, OptType ot = single);
																			
	void AddDefHexProp(UINT idProp, BOOL b, OptType ot = single);

	//
	// is this tool option property a default?
	//
	virtual BOOL IsDefaultIntProp(UINT idProp, int & nVal);
	virtual BOOL IsDefaultStringProp(UINT idProp, CString & strVal);
#define IsDefaultBoolProp(idProp, bVal)	IsDefaultIntProp((idProp), (int &)(bVal))
	 
	__inline virtual BOOL AlwaysShowDefault(UINT idProp)	{return FALSE;	/* never always show defaults*/}
	__inline virtual BOOL IsFakeProp(UINT idProp)			{return FALSE;	/* default is no */}

	// retrieve the main-type of this property
	__inline PROP_TYPE GetDefOptionType(UINT idProp)
	{
		CDefOptionProp * pdefopt = NULL;
		m_mapDefOptions.Lookup((WORD)idProp, (void *&)pdefopt);
		if (pdefopt)
			return pdefopt->m_pprop->m_nType;
		return null;
	}

	// retrieve the sub-type of this property
	// eg. subtypes for ints are bool, hex, int, oct
	__inline SUBTYPE GetDefOptionSubType(UINT idProp)
	{
		CDefOptionProp * pdefopt = NULL;
		m_mapDefOptions.Lookup((WORD)idProp, (void *&)pdefopt);
		if (pdefopt)
			return pdefopt->subtypeprop;
		return boolST;
	}

	// retrieve our default tool options from the map
	virtual GPT GetDefIntProp(UINT idProp, int & nVal);
	virtual GPT GetDefStrProp(UINT idProp, CString & strVal);

	// get the output directory string id, == -1 if none, that is used to fake (prepend to)
	// this property 'idProp'
	virtual UINT GetFakePathDirProp(UINT idProp) {return (UINT)-1;}

	// an option property in the table has changed
	virtual void OnOptionStrPropChange(UINT idProp, const CString & strVal) { /* do nothing */ }
	virtual void OnOptionIntPropChange(UINT idProp, int nVal) { /* do nothing */ }

	// conversion of int, oct,hex values into a text form
	// FUTURE (matthewt): I'd like to put this elsewhere, but where?
	BOOL ConvertToStr(UINT idProp, int nVal, CString & strVal);
	BOOL ConvertFromStr(UINT idProp, const TCHAR * pchVal, int & nVal);

	__inline BOOL NeedsSubtypeConversion(UINT idProp)
	{
		CDefOptionProp * pdefopt;
		if (!m_mapDefOptions.Lookup((WORD)idProp, (void *&)pdefopt))	return FALSE;
		return (pdefopt->m_pprop->m_nType == integer);
	}

	__inline BOOL IsListStrProp(UINT idProp)
	{
		CDefOptionProp * pdefopt;
		if (!m_mapDefOptions.Lookup((WORD)idProp, (void *&)pdefopt))	return FALSE;
		return (pdefopt->m_opttype == multiple);
	}

	// joins prop strings together using the chJoin char
	// (only the current property bag will be searched and not the any of the container's (parent's))
	GPT GetListStrProp
	(
		CSlob * pSlob,
		UINT nIDProp,
		CString & strVal,
		BOOL fInherit = TRUE, BOOL fAnti = FALSE,
		char chJoin = ','
	);

 	// add or remove the string from the list
	void MungeListStrProp(UINT nIDProp, const CString & strVal, BOOL fAppend, char chJoin = ' ');

	void SetListStrProp
	(
		CSlob * pSlob,
		UINT nIDExtraProp,
		CString & strVal,
		BOOL fInherit = TRUE, BOOL fAnti = FALSE,
		char chJoin = ','
	);

	__inline void DirtyArg(UINT nAdjProp)
		{m_pdirtyProps->SetAt(nAdjProp, (BYTE)TRUE);}

	// is this property id valid with respect to its deps?
	__inline virtual BOOL CheckDepOK(UINT idProp) {return TRUE; /* always valid */}
	
	// Query whether this slob's faked property value may be different from what the
	// "default" would be for a random slob in the same container.
	// Option handlers can safely return TRUE for all fake properties.  Returning FALSE
	// is an optimization hint to the build engine, which may save time by avoiding
	// queries of the fake property value.
	__inline virtual BOOL SetsFakePropValue(UINT idProp)	{return TRUE;}
		
	// our associated CBuildTool
	CBuildTool * m_pAssociatedBuildTool;

	// our 'current' and 'old' CSlobs stack
	CSlob *	m_pSlob;
	CSlob * m_stkOldSlobs[_MAX_SLOB_STACK];
	unsigned short m_sStk;

protected:
	CVCMapWordToPtr	m_mapDefOptions;	// our default option value map							  
	UINT			m_nPropBase;		// base of our option prop range, determined at run-time
	UINT			m_cDepth;			// our option handler 'depth' == number of 'base' handlers 

private:
	COptionHandler *	m_popthdlrBase;		// our base option handler (if we have one)
	CVCByteArray *		m_pdirtyProps;		// dirty properties
	UINT *				m_pStrListPropIds;	// cache of string list props IDs 
	UINT				m_cStrListPropIds;	// # of entries in this cache
};

// our 'Unknown' option handler
// this will substitute for any other option handler and still
// parse and generate option strings
class COptHdlrUnknown : public COptionHandler
{
	DECLARE_DYNAMIC(COptHdlrUnknown);

public:
	COptHdlrUnknown(CBuildTool * pBuildTool);

	// create our default option map
	void CreateOptDefMap() {/* do nothing*/}

	// retrieve our 'special' option props, ie. the unknown strins and options prop IDs
	void GetSpecialLogicalOptProps(UINT & nIDUnkOpt, UINT & nIDUnkStr, OptBehaviour = OBNone);

	// retrieive our min. and max. option prop IDs
	void GetMinMaxLogicalOptProps(UINT & nIDMinProp, UINT & nIDMaxProp);

	// case sensitive (though this is ignored as we have ignored options'n' strings)
	__inline const BOOL IsCaseSensitive() {return TRUE;}

	// retrieve our option string table
	// (just has two entries, unknown option and unknown string)
	COptStr * GetOptionStringTable();

	// we don't have an option lookup
	__inline COptionLookup * GetOptionLookup()	{return (COptionLookup *)NULL;}
};

// definition of the COptHdlrMapper class
class  COptHdlrMapper
{
public:
	COptHdlrMapper();
	virtual ~COptHdlrMapper();

	//	Return the COptionHandler for this tool
	__inline COptionHandler * GetOptionHandler() {return m_popthdlr;}

	//  Set the COptionHandler for this tool
	__inline void SetOptionHandler(COptionHandler * popthdlr)
		{m_popthdlr = m_popthdlrMapper = popthdlr; m_cDepth = popthdlr->GetDepth();}

	void SetMapperOptionHandler(UINT cDepth);

	// mapping of logical->actual option handler props
	__inline UINT MapLogical(UINT idProp)
	{
		if (OHGetDepth(idProp) != m_cDepth)	SetMapperOptionHandler(OHGetDepth(idProp));
		return m_popthdlrMapper->MapLogical(idProp);
	}

protected:
	COptionHandler * m_popthdlr;		// the tool's option handler

private:
	COptionHandler * m_popthdlrMapper;	// our option handler used to map the props
	UINT m_cDepth;						// our current depth handler
};

// definition of the COptionTable class
class  COptionTable: public CObject
{
public:
	// initialise our option table with
	COptionTable();
	~COptionTable();

	// get/set the COptionHandler that we'll use
	__inline COptionHandler * GetOptionHandler() {return m_popthdlr;}
	__inline void SetOptionHandler(COptionHandler * popthdlr)
	{
 		m_popthdlr = popthdlr; if (m_pSlob && m_popthdlr) m_popthdlr->SetSlob(m_pSlob, FALSE);
	}

	// set the option handler given a package name and component pair
	COptionHandler * SetOptionHandler(const TCHAR * szPkg, WORD id);

	// get/set the CSlob that contains the property bag that contains the tool option values
	// the CSlob must be a CProjItem to support inheritance
	// (but this is not required if inheritance is not wanted)
	__inline CSlob * GetPropertyBag() {return m_pSlob;} 
	__inline void SetPropertyBag(CSlob * pSlob)
	{
 		VSASSERT(pSlob, "Cannot set property bag on NULL slob"); m_pSlob = pSlob; if (m_popthdlr) m_popthdlr->SetSlob(m_pSlob, FALSE);
	}

	// parse a string using the option string table and set the approp.
	// properties in the current CSlob's property bag, returns FALSE if unable
	BOOL ParseString(CString &, OptBehaviour optbeh = OBClear);

private:
	// option 'arg expression' helper & option 'optional part' extraction
	void ExtractOptionArg(char * & pchExpr, size_t & cchExpr);
	void ExtractPart(char * & pchExpr, size_t & cchExpr, char * & pchPart, size_t & cchPart);

	// parsing option string helper functions
	BOOL MatchChoice(size_t &, char *, UINT *);
	BOOL MatchExpression(const char * pchExpr, size_t cchExpr, UINT * pidArg);	// match an expression
	BOOL MatchOption(COptStr * poptstr);	// match an option with the source

	// skip whitespace in the source
	__inline void SkipWhiteSpace()
	{
		while (isspace((unsigned char)NewGetAt(strSrc, ichSrc + cchAdvSrc)))
			cchAdvSrc++;
	}

	// collect characters from the source into the output buffer according to a set of char. filters
	BOOL CollectOptionPart
	(
		CString & strOut,				// the collected option part
		BOOL	  fAlphaNumOnly = FALSE,// allow only alpha-numeric
		BOOL	  fAllowPunct = FALSE,	// don't allow punct.
		BOOL	  fAllowKanji = TRUE,	// allow Kanji characters (required)
		BOOL      fKeepQuotes = FALSE,	// keep the quotes?
		CString   strDelim = ""			// any delimiters (could be empty)
	);

private:
	// our option handler and current CSlob (property bag) 
	COptionHandler * m_popthdlr;
	CSlob * m_pSlob;

	// how should the parsing, generation behave?
	// ie. are we using defaults, showing defaults, setting defaults etc. etc.?
	OptBehaviour m_optbeh;

	//
	// property arg list
	//
	struct {
		UINT	idArg;
		BOOL	fChanged;
		CProp *	pProp;
		OptType type;
	} m_rgProps[MAX_OPT_ARGS];	// our properties to set for each option

	// clear our property arg list
	void ClearPropValList(BOOL fOnlyThoseChanged = FALSE);

	// add a property to our list,
	// set the properties in the list in the current property bag (also for multiple options)
	__inline void AddToPropList(UINT iArg, UINT idArg, CProp * pprop)
	{
		// first delete any existing arg (likely due to matching an optional part)
		if (m_rgProps[iArg].idArg != (UINT)-1 && m_rgProps[iArg].pProp)
			CProp::DeleteCProp(m_rgProps[iArg].pProp);

		m_rgProps[iArg].idArg = idArg;	// our arg id to set
		m_rgProps[iArg].pProp = pprop;	// our property
		m_rgProps[iArg].fChanged = TRUE;	// we changed this arg. recently
	}

	void SetPropValListInPropBag(UINT * poptarg);

	// reset our property list changed flags so we can spot which ones
	// we recently set when we go to back-out
	__inline void NoChangedPropValList()
	{
		for (WORD cArgs = 0; cArgs < MAX_OPT_ARGS ; cArgs++)
			m_rgProps[cArgs].fChanged = FALSE;
	}

	// allowable flag prefixes
	CString			strPrefixes;

	// option string table 'cached' props, and 'cached' case sensitive bool
	UINT	m_nPropMin, m_nPropMax;
	BOOL	m_fCaseSensitive;

	// source to parse/generate 
	CString strSrc;			// our source string
	INT_PTR	ichSrc;				// our current position
	INT_PTR	cchAdvSrc;			// count chars advanced
	INT_PTR	cchAdvSrcToArg;		// count chars advanced to first arg.

	// multiple option data
	BOOL m_fMultipleGen;		// single or multiple?

	// cached data for our multiple option generation
	CPropBag * m_pBagCache;
	UINT m_idArgPropCache;
	OptBehaviour m_optbehCache;

	COptionList m_optlstCache;
	VCPOSITION m_posCache;

	// FUTURE: move this to the stack as locals
	TCHAR * pchArgValue;		// option arg expr. value
	UINT cchArgValue; 			// size of this dynamic buffer
	enum {none, booln, choice, absolute} patArgValue;	// option arg type
};

extern  COptionTable * g_pPrjoptengine;

extern UINT g_nIDOptHdlrUnknown;

#define NO_OPTARGS		{UINT(-1),UINT(-1),UINT(-1)}
#define OPTARGS1(a) 	{a,UINT(-1),UINT(-1)}
#define OPTARGS2(a,b)	{a, b,UINT(-1)}
#define OPTARGS3(a,b,c)	{a, b, c}

// declare the option string table
#define DECL_OPTSTR_TABLE() \
public: \
	void GetSpecialLogicalOptProps(UINT & nIDUnkOpt, UINT & nIDUnkStr, OptBehaviour = OBNone); \
	void GetMinMaxLogicalOptProps(UINT & nIDMinProp, UINT & nIDMaxProp); \
	const BOOL IsCaseSensitive(); \
	COptStr * GetOptionStringTable(); \
	COptionLookup * GetOptionLookup(); \
protected: \
	COptionLookup m_optlookup; \
	static COptStr m_poptstr[]; \

// declare the option default map
#define DECL_OPTDEF_MAP() \
private: \
	virtual void CreateOptDefMap();

// define the option string table
// need to provide:-
// o props used for the unknown option/string props (-1 if one doesn't exist)
// o min. and max. props that exist in the option string table
// o whether the parsing of the option string should be case sensitive
#define BEGIN_OPTSTR_TABLE(tool, nIDArg1, nIDArg2, nIDArg3, nIDArg4, fCaseSensitive) \
	void OLD_OPTION_HANDLER(tool)::GetSpecialLogicalOptProps(UINT & nIDUnknownOption, UINT & nIDUnknownString, OptBehaviour optbeh) \
		{ \
			ASSERT_VALID(m_pSlob); \
			nIDUnknownOption = (optbeh & OBAnti) ? (UINT)-1 : (nIDArg1); \
			nIDUnknownString = (optbeh & OBAnti) ? (UINT)-1 : (nIDArg2); \
		} \
	void OLD_OPTION_HANDLER(tool)::GetMinMaxLogicalOptProps(UINT & nIDMinProp, UINT & nIDMaxProp) \
		{nIDMinProp = (nIDArg3); nIDMaxProp = (nIDArg4); VSASSERT(nIDMinProp != (UINT)-1 && nIDMaxProp != (UINT)-1, "Min or max prop not set properly");} \
	const BOOL OLD_OPTION_HANDLER(tool)::IsCaseSensitive() \
		{return fCaseSensitive;} \
	COptStr * OLD_OPTION_HANDLER(tool)::GetOptionStringTable() \
		{return (COptStr *)m_poptstr;} \
	COptionLookup * OLD_OPTION_HANDLER(tool)::GetOptionLookup() \
		{return &m_optlookup;} \
	COptStr OLD_OPTION_HANDLER(tool)::m_poptstr[] = {

#define END_OPTSTRTBL() \
	{(UINT)-1, NULL, NO_OPTARGS, single}};

// declare the option handler
#define DEFN_OPTHDLR_HEADER(tool, szPkg, id, szPkgTool, idTool, szPkgBase, idBase)\
class OLD_OPTION_HANDLER(tool) : public COptionHandler { \
public: \
	OLD_OPTION_HANDLER(tool)() : COptionHandler(szPkg, id, szPkgTool, idTool, szPkgBase, idBase) {} \

#define DEFN_OPTHDLR_COMMON(tool, szPkg, id, szPkgTool, idTool) \
	DEFN_OPTHDLR_HEADER(tool, szPkg, id, szPkgTool, idTool, (const TCHAR *)NULL, 0)
#define DEFN_OPTHDLR_PLATFORM(tool, szPkg, id, szPkgTool, idTool, szPkgBase, idBase) \
	DEFN_OPTHDLR_HEADER(tool, szPkg, id, szPkgTool, idTool, szPkgBase, idBase)

#define END_OPTHDLR() };

// this defines the ctor that will create our default option map for the handler
#define BEGIN_OPTDEF_MAP(tool) void OLD_OPTION_HANDLER(tool)::CreateOptDefMap() {

#define OPTDEF_LIST(name, sz) AddDefStrProp(MapLogical(P_##name), sz, multiple);
#define OPTDEF_DIR_LIST(name, sz) AddDefDirProp(MapLogical(P_##name), sz, multiple);

#define OPTDEF_BOOL(name, b) AddDefBoolProp(MapLogical(P_##name), b);
#define OPTDEF_STRING(name, sz) AddDefStrProp(MapLogical(P_##name), sz);
#define OPTDEF_PATH(name, sz) AddDefPathProp(MapLogical(P_##name), sz);
#define OPTDEF_INT(name, n) AddDefIntProp(MapLogical(P_##name), n);
#define OPTDEF_HEX(name, h) AddDefHexProp(MapLogical(P_##name), h);

#define END_OPTDEF_MAP() }

#endif // _INCLUDE_OPTIONTBL_H
