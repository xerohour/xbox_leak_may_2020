//
//	PROJTOOL.H
//
//		Definition of classes for tracking buidld tools.

#ifndef _PROJTOOL_H_
#define _PROJTOOL_H_

//#include "bldapi.h"

// Note: NMAKE /A currently doesn't properly handle multiple targets on the
// LHS of a dependency/build rule (eg. A B C : deps).  Remove this if fixed.
//#define _NMAKE_LHS_BUG 1

#include "projitem.h"
#include "prjoptn.h"
#include "engine.h"

class CBuildTool;

class CNameMunger;

// Helper functions used by tools (in PROJTOOL.CPP):
BOOL  FileNameMatchesExtension(const CPath *pPath, const TCHAR *pExtensions);

#if 0
BOOL ReplaceEnvVars(CString &string);
#endif

///////////////////////////////////////////////////////////////////////////////
//
//	Build tool classes:
//
//	We divide build tools into two types: "Source Tools" which take a single 
//	input and produce intermediate files as output (e.g. C compiler)  and 
//  "Schmooze tools" which take several intermediate files and produce a 
//  the final targets.

class  CBuildTool : public CBldSysCmp, public COptHdlrMapper
{
	DECLARE_DYNCREATE(CBuildTool)
	DECLARE_SLOBPROP_MAP()
	DECLARE_REGISTRY_MAP()

public:
	CBuildTool();
	virtual ~CBuildTool();

	// Init.
	void FInit();

	// default tool options for the tool for the target type component, 'bsc_id', or
	// for any target type with attributes
	virtual BOOL GetDefaultToolOptions(DWORD bsc_id, DWORD attrib, DWORD mode, CString & strOption);

	//
	// \/ Build system engine interface API 
	//

	// Interaction with per-action 'tool-supplied' data and build system
	// events
	virtual void AddAction(CBuildAction* pAction);
	virtual void RemoveAction(CBuildAction* pAction);

protected:
	CVCMapPtrToPtr m_BuildActions;

public:
	// Does this tool operate on this file?
	virtual BOOL AttachToFile(const CPath* pPath, CProjItem * pItem);

	virtual CString GetToolName(){return m_strName;}

	virtual CString GetDefaultExtList(CString *strExt)
	{
		*strExt = m_strToolInput;
		return m_strToolInput;
	}

	CString m_strToolPrefix;	// our nmake macro prefix
	CString m_strToolInput;		// our input spec.
	CString m_strName;			// our too name

	// Is this target a oriented tool?
	BOOL m_fTargetTool;

	//
	// /\ Build system engine interface API 
	//

	virtual BOOL ProcessAddSubtractString
	( 
		CProjItem * pItem,
		const TCHAR * pstr,
		BOOL fSubtract,
		BOOL fBaseBag = FALSE
	);

	// Ensure that the tool will build an the project item (debug or release config. mode)
	// Likely to entail doing such things as munging tool options, eg. the libs for linker
	// FUTURE: Use wizards to do this in v4.0?
	virtual BOOL PerformSettingsWizard(CProjItem *, BOOL fDebug, int iUseMFC) {return FALSE;}

	// builder file reading
	//
	// function called to read a tool builder macro line
	// ie. one of the form '<tool prefix>_FOOBAR = '
	// return FALSE if an error occurred, this will abort builder reading
	virtual BOOL ReadToolMacro(CMakMacro * pMacro, CProjItem * pItem) {return TRUE; /* ok */}
};

extern UINT g_nIDToolUnknown;

// 
// Custom build tool (derived from the generic CBuildTool)
//

extern const TCHAR g_pcBeginCustomBuild[];
extern const TCHAR g_pcEndCustomBuild[];

typedef struct tagMACROINFO {
	CString strName;
	CString strValue;
} MACROINFO;

class CCustomBuildTool : public CBuildTool
{
	DECLARE_DYNAMIC(CCustomBuildTool)

public:
	CCustomBuildTool();
	virtual ~CCustomBuildTool();

	// -> from CBuildTool
	BOOL AttachToFile(const CPath* pPath, CProjItem * pItem);

	virtual BOOL GetDefaultToolOptions(DWORD bsc_id, DWORD attrib, DWORD mode, CString & strOption) { return TRUE; } // nothing interesting to do for custom build
	// <- from CBuildTool

	// read our custom build step from the builder file
	static BOOL DoReadBuildRule(CProjItem * pItem, TCHAR * pchCustomPrelude);

	// get and set custom build properties
	enum eStrPropTypes 
	{ 
		eStrPropNone,		// strProp not found at all
		eStrPropEmpty,		// strProp present, but empty
		eStrPropHasValue	// strProp present and non-empty
	};
	static BOOL SetCustomBuildStrProp(CProjItem * pItem, UINT idProp, CString & str);
	static BOOL GetCustomBuildStrProp(CProjItem * pItem, UINT idProp, CString & str, BOOL fDefault = TRUE);
	static BOOL ConvertCustomBuildStrProp(CProjItem * pItem, UINT idProp, CString & str, BOOL fDefault = TRUE);
	static eStrPropTypes HasCustomBuildStrProp(CProjItem * pItem, UINT idProp, CString * pstrProp = NULL, BOOL fDefault = TRUE);
};

// 
// Unknown tools (derived from the generic CBuildTool)
//
class CUnknownTool : public CBuildTool
{
	DECLARE_DYNAMIC(CUnknownTool)

public:

	CUnknownTool() {};
	CUnknownTool(const TCHAR * lpszPlatform, const TCHAR * lpszPrefix, const TCHAR * lpszExeName = NULL);
	virtual ~CUnknownTool();

	// from -> CBldSysCmp
	BOOL IsSupported() const { return FALSE; }
	// <- CBldSysCmp

	// -> from CBuildTool
 	BOOL AttachToFile(const CPath* pPath, CProjItem * pItem);
	// <- from CBuildTool

	__inline UINT GetUnknownPropId() const {return m_nIDUnkStr;}

	// builder file reading
	//
	// function called to read a tool builder macro line
	// ie. one of the form '<tool prefix>_FOOBAR = '
	// for the unknown tool we'll preserve these...
	BOOL ReadToolMacro(CMakMacro * pMacro, CProjItem * pItem);

private:
	CString m_strBuffer;
	UINT m_nIDUnkStr;
};


// 
// Special build tool (derived from the generic CBuildTool)
//

extern const TCHAR g_pcBeginSpecialBuildTool[];
extern const TCHAR g_pcEndSpecialBuildTool[];

class CSpecialBuildTool : public CBuildTool
{
	DECLARE_DYNAMIC(CSpecialBuildTool)

public:
	CSpecialBuildTool();
	virtual ~CSpecialBuildTool();

	// Similar to custom build tool.
	BOOL AttachToFile(const CPath* pPath, CProjItem * pItem);

	virtual BOOL GetDefaultToolOptions(DWORD bsc_id, DWORD attrib, DWORD mode, CString & strOption) { return TRUE; } // nothing interesting to do for special build
	// <- from CBuildTool

	// read our custom build step from the builder file
	static BOOL DoReadBuildRule(CProjItem * pItem, TCHAR * pchCustomPrelude, BOOL bAllConfigs = FALSE);
};


///////////////////////////////////////////////////////////////////////////////
#endif // _PROJTOOL_H_
