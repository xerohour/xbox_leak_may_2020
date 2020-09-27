//
//	PROJTOOL.CPP
//
///////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#pragma hdrstop

#include "targitem.h"
#include "bldrfile.h"
#include "projtool.h"
#include "schmztl.h"
#include "project.h"

IMPLEMENT_DYNCREATE(CBuildTool, CBldSysCmp)
IMPLEMENT_DYNAMIC(CCustomBuildTool, CBuildTool)
IMPLEMENT_DYNAMIC(CUnknownTool, CBuildTool)
IMPLEMENT_DYNAMIC(CSpecialBuildTool, CBuildTool)

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

BOOL ReplaceMsDevMacros( CString &str );
extern CProjComponentMgr *g_pPrjcompmgr;
extern BOOL ConvertMacros(CString& rstrVal);

///////////////////////////////////////////////////////////////////////////////
// Helpers
///////////////////////////////////////////////////////////////////////////////
BOOL FileNameMatchesExtension(const CPath * pPath, const  TCHAR * pExtensions)
{
	if (pPath == (const CPath *)NULL)	return FALSE;

	const TCHAR * pExtNext = pExtensions;
	const TCHAR * pFileExt = pPath->GetExtension();
	if (*pFileExt) pFileExt++;
		// GetExtension() returns pointer to '.'

	INT_PTR n = _tcslen (pFileExt);

	if (n)
	{
		while (*pExtNext != _T('\0'))

		{
			pExtNext = pExtensions;
			while (*pExtNext != _T(';') && *pExtNext != _T(',') && *pExtNext != _T('\0'))
				pExtNext = _tcsinc ((TCHAR *) pExtNext );

			// skip over the optional '*.'
			if (pExtensions[0] == _T('*') && pExtensions[1] == _T('.'))
				pExtensions += 2;
			else if (pExtensions[0] == _T('.'))
				pExtensions += 1;

			// compare extensions in case-insensitive fashion
			if ((n == (pExtNext - pExtensions)) &&			// check for ext. length equivalence
				_tcsnicmp (pFileExt, pExtensions, n) == 0)	// check for ext. string equivalence
				return TRUE;

			pExtensions = pExtNext + 1;	// skip over the sep.
		}
	}

	return FALSE;
}

enum SpecialBuildToolType
{
	InvalidSpecialBuildTool = -1,
	PreLinkDescId = 0,
	PreLinkCmdsId,
	PostBuildDescId,
	PostBuildCmdsId,
	MaxSpecialBuildToolId
};

const TCHAR* s_ppszSpecialBuildToolMacroName[] = 
{	_T("PreLink_Desc"),
	_T("PreLink_Cmds"),
	_T("PostBuild_Desc"),
	_T("PostBuild_Cmds"),
};

const int s_pSpecialBuildToolProp[] =
{
	P_PreLink_Description,
	P_PreLink_Command,
	P_PostBuild_Description,
	P_PostBuild_Command,
};

static BOOL IsSpecialBuildTool( const CString& strName, int& idtype )
{
	idtype = 0;

	while (idtype < MaxSpecialBuildToolId)
	{
		if ( 0 == strName.CompareNoCase(s_ppszSpecialBuildToolMacroName[idtype]) )
		{
			return TRUE;
		}

		idtype++;
	}
	idtype = InvalidSpecialBuildTool;
	return FALSE;
}

///////////////////////////////////////////////////////////////////////////////
//
// -------------------------------- CBuildTool --------------------------------
//
///////////////////////////////////////////////////////////////////////////////

#define theClass CBuildTool
BEGIN_SLOBPROP_MAP(CBuildTool, CBldSysCmp)
	STR_PROP(ToolPrefix)
	STR_PROP(ToolInput)
END_SLOBPROP_MAP()
#undef theClass

#define theClass CBuildTool
BEGIN_REGISTRY_MAP(CBuildTool, CBldSysCmp)	
	REGISTRY_DATA_ENTRY(IDS_REG_TOOLPREFIX, NO_SUB_KEY, "Macro_Prefix", NO_SUB_KEY, REG_STRING, m_strToolPrefix)
	REGISTRY_DATA_ENTRY(IDS_REG_TOOLINPUT, NO_SUB_KEY, "Input_Spec", NO_SUB_KEY, REG_STRING, m_strToolInput)
END_REGISTRY_MAP()
#undef theClass

CBuildTool::CBuildTool()
{
	m_strName = _TEXT("");
	m_strToolInput = _TEXT("");
	m_strToolPrefix = _TEXT("");
	m_popthdlr = (COptionHandler *)NULL;
	m_fTargetTool = FALSE;
}

CBuildTool::~CBuildTool()
{
}

void CBuildTool::FInit()
{
	// FUTURE: figure this out properly based on input and
	// output file sets, for now use CSchmoozeTool until
	// we delete this class
	m_fTargetTool = IsKindOf(RUNTIME_CLASS(CSchmoozeTool));
}

void CBuildTool::AddAction(CBuildAction* pAction)
{
	// don't care if I set this more than once
	m_BuildActions.SetAt(static_cast<void*>(pAction), static_cast<void*>(NULL));
}

void CBuildTool::RemoveAction(CBuildAction* pAction)
{
	// don't care if I actually removed this or not
	m_BuildActions.RemoveKey(static_cast<void*>(pAction));
}

#if 0
BOOL ReplaceEnvVars(CString &string)
{
	// replace $(ENV_VARIABLES)
	int i,j;
	char value[256];
	CString newString;

	i = string.Find( "$(" );
	while( i != -1 ){
		// copy the characters up to the $( into the new string.
		newString += string.Left( i );

		// skip the $( 
		CString temp;
		temp = string.Right( string.GetLength() - (i + 2) );

		// find the variable for lookup
		j = temp.Find( ")" );
		if( j != -1 ) {
			CString token;
			token = temp.Left( j );
			if( GetEnvironmentVariable( token, value, 255 ) ){
				newString += value;
			}
			// skip past the env variable and search for the next.
			string = string.Right( string.GetLength() - (i+j+3) );
			i = string.Find( "$(" );
		} else {
			// open without close !!! kill the end of the command line
			// this will likely cause a build failure
			string = "";
			return FALSE;
		}
	}
	newString += string;
	string = newString;
	return TRUE;
}
#endif

// default tool options for the tool for the target type component, 'bsc_id', or
// for any target type with attributes
BOOL CBuildTool::GetDefaultToolOptions(DWORD bsc_id, DWORD attrib, DWORD mode, CString & strOption)
{
	return FALSE;	// didn't retrieve default options
}

// Does this tool operate on this file?
BOOL CBuildTool::AttachToFile(const CPath* pPath, CProjItem * pItem)
{
	// is this a target oriented tool? if so then only attach to a target ...
	if (m_fTargetTool)
		return pItem && (pItem->GetTarget() == pItem);

	VSASSERT(pItem, "No item to attach to!");
	if (pItem)
	{
		CProject *pProject = pItem->GetProject();
		CConfigurationRecord *pCurrentConfig = (CConfigurationRecord*)pProject->GetActiveConfig()->m_pBaseRecord;
		CString strAdditionalExt;
		CString strExtensionList = m_strToolInput;

		if(pCurrentConfig->GetExtListFromToolName(GetToolName(), &strAdditionalExt))
			strExtensionList = strAdditionalExt;
		
		// this is a source oriented tool, so only attach to a file if it's in our input set
		return pPath && FileNameMatchesExtension(pPath, strExtensionList /*m_strToolInput*/); 
	}
	return(FALSE);
}

BOOL CBuildTool::ProcessAddSubtractString
(
	CProjItem * pItem,
	const TCHAR * pstr,
	BOOL fSubtract,
	BOOL fBaseBag /* = FALSE */
)
{
	g_pPrjoptengine->SetOptionHandler(GetOptionHandler());
	g_pPrjoptengine->SetPropertyBag ( pItem );

	OptBehaviour optbeh = OBSetDefault | OBNeedEmptyString;
	if (fSubtract)	
		optbeh |= OBAnti;
	if (fBaseBag) 
		optbeh |= OBBasePropBag;

	CString str = pstr;

	return g_pPrjoptengine->ParseString ( str, optbeh );
}

///////////////////////////////////////////////////////////////////////////////
//
// ------------------------------ CCustomBuildTool ----------------------------
//
///////////////////////////////////////////////////////////////////////////////

const TCHAR g_pcBeginCustomBuild[] = _TEXT("Begin Custom Build");
const TCHAR g_pcEndCustomBuild[] = _TEXT("End Custom Build");
const TCHAR g_pcCustomDescSep[] = _TEXT(" - ");

CCustomBuildTool::CCustomBuildTool()
{
	m_fTargetTool = FALSE;
	m_strName = _TEXT("cmd.exe");

	// create ourselves a simple option handler
	SetOptionHandler(new COptHdlrUnknown(this));
}

CCustomBuildTool::~CCustomBuildTool()
{
	delete GetOptionHandler();
}

CCustomBuildTool::eStrPropTypes CCustomBuildTool::HasCustomBuildStrProp(CProjItem * pItem, UINT idProp, CString * pstrProp /* = NULL */, BOOL fDefault /* = TRUE */)
{
	// get the tool string prop, don't use inheritance!
	OptBehaviour optbehOld = pItem->GetOptBehaviour();
	OptBehaviour optbeh;
	if (pItem->IsKindOf(RUNTIME_CLASS(CTargetItem)))
		optbeh = (OptBehaviour) (optbehOld | OBInherit);
	else
		optbeh = (OptBehaviour) (optbehOld & ~OBInherit);

	// use defaults?
	if (!fDefault) optbeh &= ~OBShowDefault;

	(void)pItem->SetOptBehaviour(optbeh);

	CString strNewLine;
	BOOL fRet = pItem->GetStrProp(idProp, strNewLine);

	(void)pItem->SetOptBehaviour(optbehOld);

	if (pstrProp)
		*pstrProp = _T("");
	if (!fRet)
		return eStrPropNone;
	else if (strNewLine.IsEmpty())
		return eStrPropEmpty;
	else
	{
		if (pstrProp)
			*pstrProp = strNewLine;
		return eStrPropHasValue;
	}
}

BOOL CCustomBuildTool::GetCustomBuildStrProp(CProjItem * pItem, UINT idProp, CString & strTab, BOOL fDefault /* = TRUE */)
{
	// get the prop.
	strTab.Empty();  // make sure this starts off empty

	CString strNewLine;
	eStrPropTypes propType = HasCustomBuildStrProp(pItem, idProp, &strNewLine, fDefault);
	if (propType == eStrPropNone)
		return FALSE;

	int i=0;
	int last = strNewLine.GetLength();
	while( i < last ){
		if( strNewLine[i] == _T('\r') ) {
			if( (i+1)<last && strNewLine[i+1] == _T('\n') ){
				strTab += _T('\t');
				i++;
			}
			else {
				strTab += strNewLine[i];
			}
		} else {
			strTab += strNewLine[i];
		}
		i++;
	}

	return TRUE;
}

BOOL CCustomBuildTool::ConvertCustomBuildStrProp(CProjItem * pItem, UINT idProp, CString & strTab, BOOL fDefault /* = TRUE */)
{
	// get the prop.
	strTab.Empty();  // make sure this starts off empty

	CString strNewLine;
	eStrPropTypes propType = HasCustomBuildStrProp(pItem, idProp, &strNewLine, fDefault);
	if (propType == eStrPropNone)
		return FALSE;

	int i=0;
	int last = strNewLine.GetLength();
	while( i < last ){
		if( strNewLine[i] == _T('\r') ) {
			if( (i+1)<last && strNewLine[i+1] == _T('\n') ){
				strTab += _T(';');
				i++;
			}
			else {
				strTab += strNewLine[i];
			}
		} else {
			strTab += strNewLine[i];
		}
		i++;
	}
	::ConvertMacros(strTab);

	return TRUE;
}

BOOL CCustomBuildTool::SetCustomBuildStrProp(CProjItem * pItem, UINT idProp, CString & str)
{
	int i=0;
	CString strNew;
	while( i < str.GetLength() ){
		if( str[i] == _T('\t') ){
			strNew += _T("\r\n");
		}
		else {
			strNew += str[i];
		}
		i++;
	}
	return pItem->SetStrProp(idProp, strNew);
}

BOOL CCustomBuildTool::AttachToFile(const CPath* pPath, CProjItem * pItem)
{
	// attach only if the command and output file string props. are non-empty
	CString str;
	CProject *pProject = pItem->GetProject();
	CConfigurationRecord *pCurrentConfig = (CConfigurationRecord*)pProject->GetActiveConfig()->m_pBaseRecord;
	CString strExtensionList = m_strToolInput;

	if(pItem->IsKindOf(RUNTIME_CLASS(CTimeCustomBuildItem)))
	{
		CString strCustTool = _TEXT("cmd.exe");
		pItem->SetStrProp(P_ItemBuildTool, strCustTool);
	}

	// non-empty command prop?
	if (HasCustomBuildStrProp(pItem, P_CustomCommand) != eStrPropHasValue ||

		// non-empty output prop?
		HasCustomBuildStrProp(pItem, P_CustomOutputSpec) != eStrPropHasValue)

		return FALSE;	// no

	return TRUE;	// yes
}

BOOL CCustomBuildTool::DoReadBuildRule(CProjItem * pItem, TCHAR * pchCustomPrelude)
{
	CObject * pObject = NULL;	// builder file element

	BOOL fRet = FALSE;	// only success if we find custom build end

    // Are we reading for an unknown platform
    BOOL fUnknown = !pItem->GetProject()->GetProjType()->IsSupported();

	// is this a build event
	BOOL fBuildEvent = pItem->IsKindOf(RUNTIME_CLASS(CTimeCustomBuildItem));

	// get our custom description from the prelude text
	// skip text
	pchCustomPrelude += (_tcslen(g_pcBeginCustomBuild) * sizeof(TCHAR));

	// do we have a description?
	if (*pchCustomPrelude != _T('\0'))
	{
		// skip over " - "
		int cSep = lstrlen(g_pcCustomDescSep);
		if (_tcsnicmp(pchCustomPrelude, g_pcCustomDescSep, cSep) == 0)
		{
			CString strPrelude(pchCustomPrelude + cSep * sizeof(TCHAR));
			SetCustomBuildStrProp(pItem, P_CustomDescription, strPrelude);
		}
	}

    CString strUnknownMacros;   // Raw macro strings for unknown platform

	TRY
	{
	   	for(pObject = g_buildfile.m_pmr->GetNextElement(!fBuildEvent);pObject;
			pObject = g_buildfile.m_pmr->GetNextElement(!fBuildEvent))
 	   	{
			// process this builder file element

			// premature EOF?
			if (pObject->IsKindOf(RUNTIME_CLASS(CMakEndOfFile)))
				break;

			// look for the custom build end
			if (pObject->IsKindOf(RUNTIME_CLASS(CMakComment)))
			{
				CMakComment * pMakComment = (CMakComment *)pObject;

				// get the comment and skip leading whitespace
				TCHAR * pch = pMakComment->m_strText.GetBuffer(1);
				SkipWhite(pch);

				if (_tcsnicmp(g_pcEndCustomBuild, pch, _tcslen(g_pcEndCustomBuild)) == 0)
				{
					fRet = TRUE;
					break;
				}
			}
			// is this the description block?
			else if (pObject->IsKindOf(RUNTIME_CLASS(CMakDescBlk)))
			{
				CMakDescBlk * pMakDescBlk = (CMakDescBlk *)pObject;
				
				// read in the output
				CString str;

				GetCustomBuildStrProp(pItem, P_CustomOutputSpec, str);

				// strip quotes?
				if( str.GetLength() > 0 )
					str += _T('\t');

				if (pMakDescBlk->m_strTargets.IsEmpty())
					str.Empty();
				else if (pMakDescBlk->m_strTargets[0] == _T('"'))
			    	str += pMakDescBlk->m_strTargets.Mid(1, pMakDescBlk->m_strTargets.GetLength() - 2);
				else
					str += pMakDescBlk->m_strTargets;
				
				if (!str.IsEmpty())
				{
					// replace bad macros here
					if( ReplaceMsDevMacros( str ) ){
						// Dirty the project object
						pItem->GetProject()->DirtyProject();
					}
				}

				SetCustomBuildStrProp(pItem, P_CustomOutputSpec, str);

				// convert the string list into a '\t' sep'd one
				COptionList optlst(_T('\t'), FALSE,TRUE);
				POSITION pos = pMakDescBlk->m_listCommands.GetHeadPosition();
				while (pos != (POSITION)NULL)
				{
					CString str = pMakDescBlk->m_listCommands.GetNext(pos);

					// replace bad macros here
					if( ReplaceMsDevMacros( str ) ){
						// Dirty the project object
						pItem->GetProject()->DirtyProject();
					}

					TCHAR * pch = (TCHAR *)(const TCHAR *)str;
					TCHAR * pchStart = pch;
					int cch = str.GetLength();

					// skip leading whitespace
					SkipWhite(pch);
					cch -= (int)(pch - pchStart);

					if (_tcsnicmp(pch, _TEXT("$(BuildCmds)"), cch) == 0)
						break;

					// skip leading '@'?
					if (*pch == _T('@'))
					{
						pch++;
						cch -= sizeof(TCHAR);
					}

					optlst.Append(pch, (int)cch);
				}

				// set this property
				optlst.GetString(str);

				// anything?
				if (!str.IsEmpty())
					SetCustomBuildStrProp(pItem, P_CustomCommand, str);
			}
			// is this the macros ... we're going to read in 'BuildCmds=' if there is one
			else if (pObject->IsKindOf(RUNTIME_CLASS(CMakMacro)))
			{
				CMakMacro * pMacro = (CMakMacro *)pObject;

				if (_tcsicmp(pMacro->m_strName, _TEXT("BuildCmds")) == 0)
				{
					// read in the output
					CString str;

					// convert the string list into a '\t' sep'd one
					COptionList optlst(_T('\t'), FALSE,TRUE);

					// possible token start and end
					TCHAR * pBase = pMacro->m_strValue.GetBuffer(1);
					TCHAR * pMakEnd = pBase + pMacro->m_strValue.GetLength();	
					TCHAR * pTokenEnd = max(pBase, (TCHAR *)pMakEnd - 1);
					
					while (pTokenEnd < pMakEnd)
					{
						// skip whitespace
		  				SkipWhite(pBase);

						TCHAR * pTokenStart = pBase;

						// olympus 2020 (briancr)
						// Instead of calling GetMakToken, which will delimit
						// each token at any white space and cause the build
						// commands to be broken at each space, we'll find the
						// end of the token right here. The end of the token
						// is a tab or end of string.
						BOOL bInQuotes = FALSE;
						pTokenEnd = pTokenStart;

						while (*pTokenEnd) {
							// toggle quote flag
							if (*pTokenEnd ==  _T('"')) {
								bInQuotes = ~bInQuotes;
							}
							else if (*pTokenEnd == _T('\t')) {
								if (!bInQuotes) {
									break;
								}
							}
							pTokenEnd = _tcsinc ((char *) pTokenEnd);
						}  
						#ifdef _DEBUG
						if (bInQuotes && !*pTokenEnd) TRACE ("Unmatched quotes while getting token in DoReadBuildRule\n");
						#endif 

						// end olympus 2020 fix

						// skip leading '@'?
						if (*pTokenStart == _T('@'))
						{
							pTokenStart++;
						}

						optlst.Append(pTokenStart, (UINT)(pTokenEnd - pTokenStart));

						// next token
						pBase = _tcsinc(pTokenEnd);
					}

					// set this property
					optlst.GetString(str);

					// anything?
					if (!str.IsEmpty())
						SetCustomBuildStrProp(pItem, P_CustomCommand, str);
				}
				
				if (fUnknown)
				{
                    CString strMacro = pMacro->m_strName + _TEXT("=") + pMacro->m_strValue + _T("\r\n");

                    if (strUnknownMacros.IsEmpty())
                        strUnknownMacros = strMacro;
                    else
                        strUnknownMacros += strMacro;
				}
			}	

	   		delete pObject; pObject = (CObject *)NULL;
		}

        if (fUnknown)
            SetCustomBuildStrProp(pItem, P_CustomMacros, strUnknownMacros);
						 
		if (pObject != (CObject *)NULL)
			delete pObject;
	}
	CATCH (CException, e)
	{
		if (pObject != (CObject *)NULL)
			delete pObject;
	}
	END_CATCH

	return fRet;	// ok
}

// fix for speed bugs #4039 and 4307
// MsDevDir is an environment variable.
// environment variables are case insensitive.
// nmake is case sensitive. therefore our makefiles
// did not work from nmake in 4.0
// This routine touppers the bogus macro
BOOL ReplaceMsDevMacros( CString &str ){
	int nBegin = 0;
	int nEnd;
	int rValue = FALSE;
	CString strMsdev("MSDevDir");
	CString strRemoteTarget("RemoteTargetPath");
	int nLength = str.GetLength();

	// loop until end of string
	for(;;){
		CString strMacro;

		// find the beging of a macro
		while ( nBegin < (nLength-1) ) {
		    if( str[nBegin] == _T('$') && str[nBegin+1] == _T('(') ){
				nBegin+=2;
				break;
			}
			nBegin++;
		}
	
		if( nBegin >= nLength-1 ) return rValue;
	
   		nEnd = nBegin;
		// find the end of a macro
		while ( str[nEnd] != _T(')')  ) {
			strMacro += str[nEnd];
			nEnd++;
			if( nEnd>= nLength )
				return rValue;
		}

		// compare and replace if equal
		if( strMacro == strMsdev ){
			CString strUpper("MSDEVDIR");
			rValue = TRUE;
			int i;
			for( i=0; nBegin < nEnd; i++ ){
				str.SetAt(nBegin,strUpper[i]);
				nBegin++;
			}
		}
		if( strMacro == strRemoteTarget ) {
			CString strUpper("REMOTETARGETPATH");
			rValue = TRUE;
			int i;
			for( i=0; nBegin < nEnd; i++ ){
				str.SetAt(nBegin,strUpper[i]);
				nBegin++;
			}
		}
	}
}

///////////////////////////////////////////////////////////////////////////////
//
// ------------------------------ CUnknownTool ------------------------------
//
///////////////////////////////////////////////////////////////////////////////

UINT g_nIDToolUnknown = PROJ_PER_CONFIG_UNKNOWN;

CUnknownTool::CUnknownTool
(
	const TCHAR * lpszPlatform,
	const TCHAR * lpszPrefix,
	const TCHAR * lpszExeName /* = NULL */
)
{
	m_nIDUnkStr = g_nIDToolUnknown++;
	VSASSERT(g_nIDToolUnknown <= (PROJ_PER_CONFIG_LAST), "g_nIDToolUnknown out of range; reset PROJ_PER_CONFIG_LAST if new value is valid");

	m_strName = _TEXT("unknown.exe");
	m_strToolPrefix = lpszPrefix;

	SetOptionHandler(new COptHdlrUnknown(this));
}

CUnknownTool::~CUnknownTool()
{
	delete GetOptionHandler();
}

///////////////////////////////////////////////////////////////////////////////
BOOL CUnknownTool::AttachToFile(const CPath* pPath, CProjItem * pItem)
{
	// attach done in CProjItem::SuckMacro
	return FALSE;
}

///////////////////////////////////////////////////////////////////////////////
// function called to read a tool builder macro line
// ie. one of the form '<tool prefix>_FOOBAR = '
// for the unknown tool we'll preserve these...
BOOL CUnknownTool::ReadToolMacro(CMakMacro * pMacro, CProjItem * pItem)
{
	// don't inherit while getting prop.
	OptBehaviour optbehOld = pItem->GetOptBehaviour();
	(void)pItem->SetOptBehaviour((OptBehaviour)(optbehOld & ~OBInherit));

	// collect all of our macro's raw-data
	if (pItem->GetStrProp(GetUnknownPropId(), m_strBuffer) != valid)
		m_strBuffer = _TEXT("");

	// append raw-data
	m_strBuffer += pMacro->m_strRawData;

	// set
	pItem->SetStrProp(GetUnknownPropId(), m_strBuffer);
	
	(void)pItem->SetOptBehaviour(optbehOld);

	return TRUE;	// ok
}

///////////////////////////////////////////////////////////////////////////
// Makefile reading and writing for a generic tool
///////////////////////////////////////////////////////////////////////////

const TCHAR g_pcBeginSpecialBuildTool[] = _TEXT("Begin Special Build Tool");
const TCHAR g_pcEndSpecialBuildTool[] = _TEXT("End Special Build Tool");

class COptHdlrSpecialTool : public COptionHandler
{
	DECLARE_DYNAMIC(COptHdlrSpecialTool);

public:
	COptHdlrSpecialTool(CBuildTool * pBuildTool);

	// create our default option map
	void CreateOptDefMap() {/* do nothing*/}

	// retrieve our 'special' option props, ie. the unknown strins and options prop IDs
	void GetSpecialLogicalOptProps(UINT & nIDUnkOpt, UINT & nIDUnkStr, OptBehaviour optbeh = OBNone)
	{
		nIDUnkOpt = (UINT) -1;
		nIDUnkStr = (UINT)0;
	}

	// retrieive our min. and max. option prop IDs
	void GetMinMaxLogicalOptProps(UINT & nIDMinProp, UINT & nIDMaxProp)
	{
		nIDMinProp = P_PreLink_Description;
		nIDMaxProp = P_PostBuild_Command;
	}

	// case sensitive (though this is ignored as we have ignored options'n' strings)
	__inline const BOOL IsCaseSensitive() {return TRUE;}

	// retrieve our option string table
	// (just has two entries, unknown option and unknown string)
	COptStr * GetOptionStringTable(){ return poptstrOptHdlrUknown; }

	// we don't have an option lookup
	__inline COptionLookup * GetOptionLookup()	{return (COptionLookup *)NULL;}

private:
	static COptStr poptstrOptHdlrUknown[];
};

COptStr COptHdlrSpecialTool::poptstrOptHdlrUknown[] = {
	IDOPT_UNKNOWN_STRING,	"",		NO_OPTARGS, single,
	(UINT)-1,				NULL,	NO_OPTARGS, single
};

IMPLEMENT_DYNAMIC(COptHdlrSpecialTool, COptionHandler);

COptHdlrSpecialTool::COptHdlrSpecialTool(CBuildTool * pBuildTool) : COptionHandler(pBuildTool)
{
	// our base
	SetOptPropBase(P_PreLink_Description);

	Initialise();
}	

CSpecialBuildTool::CSpecialBuildTool()
{
	m_fTargetTool = FALSE;
	m_strName = _TEXT("special.exe");

	// create ourselves a simple option handler
	SetOptionHandler(new COptHdlrSpecialTool(this));
}

CSpecialBuildTool::~CSpecialBuildTool()
{
	delete GetOptionHandler();
}

BOOL CSpecialBuildTool::AttachToFile(const CPath* pPath, CProjItem * pItem)
{
	// attach only if the command string props. are non-empty and pItem is a project or target.
	if ( !(pItem->IsKindOf(RUNTIME_CLASS(CProject)) || pItem->IsKindOf(RUNTIME_CLASS(CTargetItem))))
		return FALSE;

	// Allow inheritance.
	OptBehaviour optbehOld = pItem->GetOptBehaviour();
	pItem->SetOptBehaviour((OptBehaviour)(optbehOld | OBInherit));

	CString strPreCmds;
	CString strPostCmds;

	BOOL bAttached = ( pItem->GetStrProp(s_pSpecialBuildToolProp[PreLinkCmdsId], strPreCmds) && !strPreCmds.IsEmpty() )
		|| ( pItem->GetStrProp(s_pSpecialBuildToolProp[PostBuildCmdsId], strPostCmds) && !strPostCmds.IsEmpty() );

	// Restore original behavior.
	pItem->SetOptBehaviour(optbehOld);

	return bAttached;
}

BOOL CSpecialBuildTool::DoReadBuildRule(CProjItem * pItem, TCHAR * pchCustomPrelude, BOOL bAllConfigs)
{
	CObject * pObject = NULL;	// builder file element

	BOOL fRet = FALSE;	// only success if we find custom build end

    // Are we reading for an unknown platform
    BOOL fUnknown = !pItem->GetProject()->GetProjType()->IsSupported();

	// is this a build event?
	BOOL fBuildEvent = pItem->IsKindOf(RUNTIME_CLASS(CTimeCustomBuildItem));

    CString strUnknownMacros;   // Raw macro strings for unknown platform

	TRY
	{
	   	for(pObject = g_buildfile.m_pmr->GetNextElement(!fBuildEvent);pObject;
			pObject = g_buildfile.m_pmr->GetNextElement(!fBuildEvent))
 	   	{
			// process this builder file element

			// premature EOF?
			if (pObject->IsKindOf(RUNTIME_CLASS(CMakEndOfFile)))
				break;

			// look for the custom build end
			if (pObject->IsKindOf(RUNTIME_CLASS(CMakComment)))
			{
				CMakComment * pMakComment = (CMakComment *)pObject;

				// get the comment and skip leading whitespace
				TCHAR * pch = pMakComment->m_strText.GetBuffer(1);
				SkipWhite(pch);

				if (_tcsnicmp(g_pcEndSpecialBuildTool, pch, _tcslen(g_pcEndSpecialBuildTool)) == 0)
				{
					fRet = TRUE;
					break;
				}
			}
			// is this the description block?
			else if (pObject->IsKindOf(RUNTIME_CLASS(CMakDescBlk)))
			{
				//
				// Not sure what to do.
				VSASSERT(0, "Dunno what to do with a CMakDescBlk here");
			}
			// is this the macros ... we're going to read in 'BuildCmds=' if there is one
			else if (pObject->IsKindOf(RUNTIME_CLASS(CMakMacro)))
			{
				CMakMacro * pMacro = (CMakMacro *)pObject;
				int idtype;

				if (IsSpecialBuildTool(pMacro->m_strName, idtype))
				{
					// Process special project build tool.
					CString str = pMacro->m_strValue;
					if (!pMacro->m_strValue.IsEmpty())
					{
						if ( bAllConfigs )
						{
							pItem->CreateAllConfigRecords();
							int	nConfigs = pItem->GetPropBagCount();
							int k;

							for (k=0; k < nConfigs; k++)
							{
								// global across *all configs* so
								// make sure we have all of our configs matching the project

								// Force config. active, set prop, and then restore.
								pItem->ForceConfigActive((CConfigurationRecord *)(*pItem->GetConfigArray())[k]);
								pItem->SetStrProp(s_pSpecialBuildToolProp[idtype], pMacro->m_strValue);
								pItem->ForceConfigActive();
							}
						}
						else
						{
							pItem->SetStrProp(s_pSpecialBuildToolProp[idtype], pMacro->m_strValue);
						}
					}
				}
				
				if (fUnknown)
				{
                    CString strMacro = pMacro->m_strName + _TEXT("=") + pMacro->m_strValue + _T("\r\n");

                    if (strUnknownMacros.IsEmpty())
                        strUnknownMacros = strMacro;
                    else
                        strUnknownMacros += strMacro;
				}
			}	

	   		delete pObject; pObject = (CObject *)NULL;
		}

		if (pObject != (CObject *)NULL)
			delete pObject;
	}
	CATCH (CException, e)
	{
		if (pObject != (CObject *)NULL)
			delete pObject;
	}
	END_CATCH

	return fRet;	// ok
}
