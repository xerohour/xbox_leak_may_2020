// ProjectOptions.h - definition of basic option handling code and tables

#pragma once

/* option table choices and meanings
OPT_ENUM() - enumerated list
OPT_BOOL() - boolean (degenerate enumerated list with two elements)
OPT_BSTR() - string, switch only evaluated if non-empty (will be quoted if it isn't already)
OPT_BSTR_NOQUOTE() - string, switch only evaluated if non-empty (quotes will not be automagically added)
OPT_BSTR_SPECIAL() - string, switch only evaluated if non-empty, derived option handler class responsible
						for generating specific switch
OPT_INT()  - integer element
OPT_INT_NOZERO()	- integer element, but only generate switch if non-zero
OPT_ENUM_SET_COND() - enumerated list; if non-zero, then evaluate conditional property to this one
OPT_BOOL_SET_COND() - boolean; if TRUE, then evaluate conditional properties to this one
OPT_INT_NOZERO_SET_COND() - integer element, only generate switch if non-zero, evaluate conditional
								property if non-zero
OPT_INT_COND()		- only evaluate this integer switch if the property it depends on is TRUE 
						  (or non-zero for an enumerated property).  The '/' is not generated 
						  automagically.
OPT_INT_NOZERO_COND() - only evaluate this integer switch if the property it depends on is TRUE
						  (or non-zero for an enumerated property) AND the property itself is
						  non-zero.  The '/' is not generated automagically.
OPT_BSTR_COND()		- only evaluate this string switch if the property it depends on is TRUE 
						  (or non-zero for an enumerated property).  No switch generated if the
						  storage element is non-empty.  The '/' is not generated automagically.
OPT_INT_NOZERO_COND_CASCADE() - cascading conditional integer switch (i.e., has both a parent property
						and a conditional property).
parser special characters:
- '|' separates options in an enum or boolean
- false condition always comes first in a set
- %<> means format the storage contents into that spot using the specified printf formatting specified by <>
*/

#include <vccolls.h>

const wchar_t* const wszDefaultString = L"";

// forward declarations
class CLookupByDispidMap;
class CLookupByPropertyNameMap;

typedef enum
{
	stdOptEnum,			// option is an enum
	stdOptBool,			// option is a BOOL
	stdOptBstr,			// option is a string (quote it)
	stdOptBstrNoQuote,	// option is a string (don't quote it)
	stdOptInt,			// option is an integer
	noZeroOptInt,		// option is an integer where it must be non-zero to generate a switch
	specOptBstr,		// option is a string with special handling required
	stdOptEnd			// option is the end of the option table
} optionEntryType;

typedef enum
{
	conditionNone,		// doesn't set or have any conditional properties
	conditionSet,		// evaluate the specified conditional property iff non-zero
	conditionProp		// evaluate only if asked directly (i.e., by the prop we're conditional on)
} conditionalOption;

// what type is the option?
typedef enum {single, multiple, multipleNoCase} OptType;

// special option page values
#define NO_HYPERLINK		-1
#define SPECIAL_HYPERLINK	-2

// our definition of an option
struct SOptionEntry
{
	long				idOption;
	const wchar_t *		szOptionName;
	const wchar_t *		szOption;
	int					nStartEnumRange;
	int					nEndEnumRange;
	long				idOptionPage;
	conditionalOption	condType;
	int					nFirstTrue;
	long				idParentOption;
	long				idConditionalOption;
	OptType				type;
	optionEntryType		entryType;
};

#define NoParentProp	0
#define NoNextProp		0

// our tool option tables
#define BEGIN_OPTION_TABLE(tool, szToolString, idSection, fSupportsAdditionalOptions, fCaseSensitive) \
	const BOOL tool::IsCaseSensitive() \
		{ return fCaseSensitive; } \
	const BOOL tool::SupportsAdditionalOptions() \
		{ return fSupportsAdditionalOptions; } \
	const wchar_t * tool::ToolString() \
		{ return szToolString; } \
	const long tool::SectionID() \
		{ return idSection; } \
	BOOL tool::s_bDispidMapInitialized = FALSE; \
	BOOL tool::s_bPropertyNameMapInitialized = FALSE; \
	SOptionEntry tool::m_pOptionEntries[] = {

#define END_OPTION_TABLE() \
	{ 0, L"", L"", 0, 0, 0, conditionNone, 0, NoParentProp, NoNextProp, single, stdOptEnd }};

// declare the option string table
#define DECLARE_OPTION_TABLE() \
public: \
	virtual const BOOL IsCaseSensitive(); \
	virtual const BOOL SupportsAdditionalOptions(); \
	virtual const wchar_t * ToolString(); \
	virtual SOptionEntry* GetOptionTable() { return (SOptionEntry *)m_pOptionEntries; } \
	virtual BOOL PropertyNameLookupIsInitialized() { return s_bPropertyNameMapInitialized; } \
	virtual BOOL DispidLookupIsInitialized() { return s_bDispidMapInitialized; } \
	LPCOLESTR GetSectionString() { if (SectionID() && m_strSectionName.IsEmpty()) m_strSectionName.LoadString(SectionID()); return m_strSectionName; } \
protected: \
	virtual void SetPropertyNameLookupInitialized(BOOL bInit = TRUE) { s_bPropertyNameMapInitialized = bInit; } \
	virtual void SetDispidLookupInitialized(BOOL bInit = TRUE) { s_bDispidMapInitialized = bInit; } \
	virtual const long SectionID(); \
protected: \
	static BOOL s_bDispidMapInitialized; \
	static BOOL s_bPropertyNameMapInitialized; \
	static SOptionEntry m_pOptionEntries[]; \
	CStringW m_strSectionName;

#define OPT_ENUM(prop, propSwitch, startRange, endRange, propName, pageID, idRange) \
	{ idRange##_##prop, propName, propSwitch, startRange, endRange, pageID, conditionNone, 0, NoParentProp, NoNextProp, single, stdOptEnum },
#define OPT_ENUM_SPECIAL(prop, propSwitch, startRange, endRange, propName, pageID, idRange) \
	{ idRange##_##prop, propName, propSwitch, startRange, endRange, pageID, conditionNone, 0, NoParentProp, NoNextProp, single, stdOptEnum },
#define OPT_ENUM_SET_COND(prop, propSwitch, startRange, endRange, firstTrue, propName, nextProp, pageID, idRange) \
	{ idRange##_##prop, propName, propSwitch, startRange, endRange, pageID, conditionSet, firstTrue, NoParentProp, idRange##_##nextProp, single, stdOptEnum },
#define OPT_BOOL(prop, propSwitch, propName, pageID, idRange) \
	{ idRange##_##prop, propName, propSwitch, 0, 0, pageID, conditionNone, 0, NoParentProp, NoNextProp, single, stdOptBool },
#define OPT_BOOL_SET_COND(prop,	propSwitch, propName, nextProp, pageID, idRange) \
	{ idRange##_##prop, propName, propSwitch, 0, 0, pageID, conditionSet, 0, NoParentProp, idRange##_##nextProp, single, stdOptBool },
#define OPT_BSTR(prop, propSwitch, propName, opt_type, pageID, idRange) \
	{ idRange##_##prop, propName, propSwitch, 0, 0, pageID, conditionNone, 0, NoParentProp, NoNextProp, opt_type, stdOptBstr },
#define OPT_BSTR_NOQUOTE(prop, propSwitch, propName, opt_type, pageID, idRange) \
	{ idRange##_##prop, propName, propSwitch, 0, 0, pageID, conditionNone, 0, NoParentProp, NoNextProp, opt_type, stdOptBstrNoQuote },
#define OPT_BSTR_SPECIAL(prop, propSwitch, propName, opt_type, pageID, idRange) \
	{ idRange##_##prop, propName, propSwitch, 0, 0, pageID, conditionNone, 0, NoParentProp, NoNextProp, opt_type, specOptBstr },
#define OPT_BSTR_COND(prop, propSwitch, propName, parentProp, opt_type, pageID, idRange) \
	{ idRange##_##prop, propName, propSwitch, 0, 0, pageID, conditionProp, 0, idRange##_##parentProp, NoNextProp, opt_type, stdOptBstr },
#define OPT_BSTR_COND_NOQUOTE(prop, propSwitch, propName, parentProp, opt_type, pageID, idRange) \
	{ idRange##_##prop, propName, propSwitch, 0, 0, pageID, conditionProp, 0, idRange##_##parentProp, NoNextProp, opt_type, stdOptBstrNoQuote },
#define OPT_INT(prop, propSwitch, propName, pageID, idRange) \
	{ idRange##_##prop, propName, propSwitch, 0, 0, pageID, conditionNone, 0, NoParentProp, NoNextProp, single, stdOptInt },
#define OPT_INT_NOZERO(prop, propSwitch, propName, pageID, idRange) \
	{ idRange##_##prop, propName, propSwitch, 0, 0, pageID, conditionNone, 0, NoParentProp, NoNextProp, single, noZeroOptInt },
#define OPT_INT_NOZERO_SET_COND(prop, propSwitch, propName, nextProp, pageID, idRange) \
	{ idRange##_##prop, propName, propSwitch, 0, 0, pageID, conditionSet, 0, NoParentProp, idRange##_##nextProp, single, noZeroOptInt },
#define OPT_INT_COND(prop, propSwitch, propName, parentProp, pageID, idRange) \
	{ idRange##_##prop, propName, propSwitch, 0, 0, pageID, conditionProp, 0, idRange##_##parentProp, NoNextProp, single, stdOptInt },
#define OPT_INT_NOZERO_COND(prop, propSwitch, propName, parentProp, pageID, idRange) \
	{ idRange##_##prop, propName, propSwitch, 0, 0, pageID, conditionProp, 0, idRange##_##parentProp, NoNextProp, single, noZeroOptInt },
#define OPT_INT_NOZERO_COND_CASCADE(prop, propSwitch, propName, parentProp, nextProp, pageID, idRange) \
	{ idRange##_##prop, propName, propSwitch, 0, 0, pageID, conditionProp, 0, idRange##_##parentProp, idRange##_##nextProp, single, noZeroOptInt },

class CLookupByDispidMap : public CVCMapDWordToPtr
{
public:
	SOptionEntry* FindOptionEntry(DISPID dispidProperty);
};

class CLookupByPropertyNameMap : public CVCMapStringWToPtr
{
public:
	SOptionEntry* FindOptionEntry(BSTR bstrProperty);
};

// pure base class used for generating switches and full command lines
class COptionHandlerBase
{
public:
	COptionHandlerBase() : m_idLastOptionPage(-1) {}
	virtual ~COptionHandlerBase() {}

	CLookupByDispidMap* GetOptionLookupByDispid(BOOL bInitialize = TRUE) 
	{ 
		if (bInitialize)
			InitializeDispidMap();
		return &m_optLookupByDispid; 
	} 
	CLookupByPropertyNameMap* GetOptionLookupByPropertyName(BOOL bInitialize = TRUE) 
	{ 
		if (bInitialize)
			InitializePropertyNameMap();
		return &m_optLookupByPropertyName; 
	} 

	virtual HRESULT GenerateCommandLine(IVCPropertyContainer* pPropContainer, BOOL bForDisplay, commandLineOptionStyle fStyle, 
		CStringW& rstrCmdLine);
	virtual HRESULT ProcessEntry(IVCPropertyContainer* pPropContainer, SOptionEntry* pEntry, CStringW& strOption, 
		commandLineOptionStyle fStyle, CStringW& rstrTrailing, BOOL bIncludeOptionalParts = TRUE, BOOL bIncludeSlash = TRUE, 
		BOOL bForDisplay = FALSE);
	void AdvanceCommandLine(CStringW& rstrCmdLine, CStringW& rstrOption, LPCOLESTR szSep = L" ");
	void GetTrailingCommandLinePart(CStringW& strTrailing) { strTrailing = m_strTrailingPartForDisplay; }
	HRESULT FindSwitch(IVCPropertyContainer* pPropContainer, BSTR bstrProp, DISPID dispidProp, CStringW& rstrSwitch);
	LPCOLESTR PageName(long idOptionPage);
	LPCOLESTR ToolUIName();
	virtual void GetMultiPropSeparator(long id, BSTR* pbstrPreferred, BSTR* pbstrAll);

	virtual void InitializeDispidMap(BOOL bForce = FALSE);
	virtual void InitializePropertyNameMap(BOOL bForce = FALSE);

	// this block of methods is overridden in the declaration/definition of the option table
	virtual const BOOL IsCaseSensitive() PURE;
	virtual const BOOL SupportsAdditionalOptions() PURE;
	virtual SOptionEntry* GetOptionTable() PURE;
	virtual BOOL PropertyNameLookupIsInitialized() PURE;
	virtual BOOL DispidLookupIsInitialized() PURE;
	virtual const wchar_t * ToolString() PURE;
	virtual const long SectionID() PURE;

	// default value handlers
	virtual void GetDefaultValue( long id, BSTR *pVal, IVCPropertyContainer *pPropCnt = NULL ) PURE;
	virtual void GetDefaultValue( long id, long *pVal, IVCPropertyContainer *pPropCnt = NULL ) PURE;
	virtual void GetDefaultValue( long id, VARIANT_BOOL *pVal, IVCPropertyContainer *pPropCnt = NULL ) PURE;

	// default value helpers
	static HRESULT GetValueTrue(VARIANT_BOOL* pbVal)
	{
		*pbVal = VARIANT_TRUE;
		return S_OK;
	}

	static HRESULT GetValueFalse(VARIANT_BOOL* pbVal)
	{
		*pbVal = VARIANT_FALSE;
		return S_OK;
	}

	static HRESULT GetDefaultString(BSTR* pbstrVal)
	{
		CComBSTR bstrRet( wszDefaultString );
		*pbstrVal = bstrRet.Detach();
		return S_OK;
	}

protected:
	void EvaluateEnumAtIndex(int nVal, LPCOLESTR szOption, BOOL bIncludeSlash, CStringW& rstrSwitch);
	void EvaluateString(CStringW& rstrVal, BOOL bIsMultiple, LPCOLESTR szOption, LPCOLESTR szOptionName, long idOption, 
		long idOptionPage, BOOL bIncludeSlash, BOOL bQuoteIt, BOOL bForDisplay, CStringW& rstrSwitch);
	void EvaluateInteger(long nVal, LPCOLESTR szOption, BOOL bIncludeSlash, CStringW& rstrSwitch);
	BOOL GetIntermediateDirectoryForFileCfg(IVCPropertyContainer* pPropContainer, CStringW& rstrIntDir);
	BOOL GetFileNameForFileCfg(IVCPropertyContainer* pPropContainer, CStringW& rstrFileName);
	BOOL GetProjectForFileCfg(IVCPropertyContainer* pPropContainer, VCProject** ppProject);
	BOOL SetBstrInVariant(IVCPropertyContainer* pPropContainer, CComBSTR& bstrVal, CComVariant& rvar);
	virtual void EvaluateSpecialEnumAtIndex(int nVal, LPCOLESTR szOption, long idOption,
		IVCPropertyContainer* pPropContainer, BOOL bIncludeSlash, CStringW& rstrSwitch) 
		{ EvaluateEnumAtIndex(nVal, szOption, bIncludeSlash, rstrSwitch); }
	virtual void EvaluateSpecialString(CStringW& rstrVal, BOOL bIsMultiple, LPCOLESTR szOption, long idOption,
		LPCOLESTR szOptionName, long idOptionPage, IVCPropertyContainer* pPropContainer, BOOL bIncludeSlash, 
		BOOL bForDisplay, commandLineOptionStyle fStyle, CStringW& rstrTrailing, CStringW& rstrSwitch) 
		{ EvaluateString(rstrVal, bIsMultiple, szOption, szOptionName, idOption, idOptionPage, bIncludeSlash, TRUE, bForDisplay, 
			rstrSwitch); }
	virtual BOOL CharIsSeparator(wchar_t ch) { return (ch == L';') || (ch == L','); }
	virtual BOOL SetEvenIfDefault(VARIANT *pvarDefault, long idOption) { return FALSE; }
	virtual BOOL SynthesizeOptionIfNeeded(IVCPropertyContainer* pPropContainer, long idOption)
		{ return FALSE; }
	virtual BOOL SynthesizeOption(IVCPropertyContainer* pPropContainer, long idOption, CComVariant& rvar) 
		{ VSASSERT(FALSE, "Must override base class implementation of SynthesizeOption if SynthesizeOptionIfNeeded is implemented"); return FALSE; }
	virtual BOOL OverrideOptionSet(IVCPropertyContainer* pPropContainer, long idOption)
		{ return FALSE; }
	virtual void FormatScriptItem(const wchar_t* szOptionName, long idOption, long idOptionPage, CStringW& rstrOption);

	// this block of methods is overridden in the declaration/definition of the option table
	virtual void SetPropertyNameLookupInitialized(BOOL bInit = TRUE) PURE;
	virtual void SetDispidLookupInitialized(BOOL bInit = TRUE) PURE;

protected:
	CLookupByDispidMap m_optLookupByDispid;
	CLookupByPropertyNameMap m_optLookupByPropertyName;
	CStringW m_strToolUIName;
	CStringW m_strOptionPage;
	CStringW m_strTrailingPartForDisplay;
	long m_idLastOptionPage;
};
