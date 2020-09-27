//
// COptionTable
//

#include "stdafx.h"		// our standard AFX include
#pragma hdrstop

#include "prjoptn.h"
#include "targitem.h"
#include "project.h"

IMPLEMENT_DYNAMIC(COptionHandler, CBldSysCmp);
IMPLEMENT_DYNAMIC(COptHdlrUnknown, COptionHandler);

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

extern CProjComponentMgr *g_pPrjcompmgr;

//////////////////////////////////////////////////////////////////////////////
// implementation of the COptionList class
#pragma intrinsic(memcpy)

void COptionList::SetString(const TCHAR * pch)
{
	VSASSERT(pch != (const TCHAR *)NULL, "SetString cannot be called with NULL");

	// empty the old one
	Empty();

	const TCHAR * rgchJoin = (const TCHAR *)m_strJoin;

	//
	// Use rgchJoinAndSpace to eliminate spaces surrounding separators.  This is a quick
	// fix to prevent the usability problem of leading and trailing spaces in tokens. (Speed:49).
	//
	// NOTE: This fix will prevent the obscure but legal use of path names with leading spaces.
	//

	// allocate enough to copy grchJoin and append space.
	TCHAR * rgchJoinAndSpace = new TCHAR[_tcslen(rgchJoin)+2];
	_tcscpy(rgchJoinAndSpace,rgchJoin);
	_tcscat(rgchJoinAndSpace,_T(" "));

	// skip leading join chars and spaces.
	while (*pch != _T('\0') && _tcschr(rgchJoinAndSpace, (unsigned int)(int)*pch) != (TCHAR *)NULL)	
		pch++;	// join chars assumed to be SBC

	// anything to do?
	if (*pch == _T('\0'))
	{
		delete [] rgchJoinAndSpace;
		return; // do nothing
	}

	TCHAR chVal, chJoin = m_strJoin[0];
	VSASSERT(chJoin != _T('\0'), "Join character cannot be \0");

	BOOL fQuoted = FALSE;
	const TCHAR * pchWord = pch;
	size_t cch, cchWord = 0;
	do
	{
		chVal = *pch;

		// are we quoted?
		if (chVal == _T('"'))
			fQuoted = !fQuoted;

		// look for a non-quoted join char, or terminator
		if (chVal == _T('\0') || (_tcschr(rgchJoin, chVal) != NULL && (chVal == _T('\t') || !fQuoted)))
		{
			// skip any extra join chars (and spaces) with this one, ie. ' , '
			if (chVal != _T('\0'))
			{
				chVal = *(++pch);	// join chars assumed to be SBC
				while (chVal != _T('\0') && _tcschr(rgchJoinAndSpace, chVal) != NULL)
				{												
					pch++; chVal = *pch;	// join chars assumed to be SBC
				}
			}

			// Remove trailing blanks from pchWord.
			while (0 != cchWord && 0 == _tccmp( _tcsdec(pchWord,pchWord+cchWord),_T(" ")) )
				--cchWord;

			// do we have any word characters?
			if (cchWord != 0)	Append(pchWord, (UINT) cchWord);

			cchWord = 0;	// ready for start of next word
			pchWord = pch;
		}
		else
		{
			// skip non-join char
			cch = _tclen(pch);
			cchWord += cch; pch += cch;
		}
	}
	while (chVal != _T('\0'));

	delete [] rgchJoinAndSpace;
	return;
}

void COptionList::GetString(CString & str)
{
	TCHAR chJoin = m_strJoin[0];
	VSASSERT(chJoin != _T('\0'), "Join character cannot be \0");

	// allocate our buffer
	TCHAR * pch;
	if (!m_cchStr ||	// empty string?
		(pch = new TCHAR[m_cchStr]) == (TCHAR *)NULL
	   )
	{
		str = _TEXT(""); // in case of error
		return;
	}

	TCHAR * pchWord = pch;
	VCPOSITION pos = m_lstStr.GetHeadPosition();
	VSASSERT(pos != (VCPOSITION)NULL, "No options in option list!");
	for (;;)	// we'll break (more efficient!)
	{
		OptEl * poptel = (OptEl *)m_lstStr.GetNext(pos);

		int cch = poptel->cch;
		if( cch > 0 )
		{
		    memcpy(pchWord, poptel->pch, cch);
    
		    if (pos != (VCPOSITION)NULL)
			    pchWord[cch-1] = chJoin;	// join *not* terminate
		    else
			    break;	// terminate!
    
		    pchWord += cch;
		}
	}

	// return string and then delete our local buffer
	str = pch;
	delete [] pch;
}

void COptionList::Empty()
{
	// free-up our string memory
	VCPOSITION pos = m_lstStr.GetHeadPosition();
	while (pos != (VCPOSITION)NULL)
	{
		OptEl * poptel = (OptEl *)m_lstStr.GetNext(pos);
		poptel->cref--;	// decr. the ref. count
		if (poptel->cref == 0)	// if zero, then free-up the element
			delete [] (BYTE *)poptel;
	}
	m_lstStr.RemoveAll();
	m_mapStrToPtr.RemoveAll();
}

void COptionList::Add(OptEl * poptel)
{
	// put this into our list
	poptel->cref++;	// bump. ref. count.
	VCPOSITION posOurWord = m_lstStr.AddTail((void *)poptel);
	m_cchStr += poptel->cch;

	// remember this for quick 'does string exist in list?' query
	// (in the case of dupes, we might nuke the old duplicate but that
	//  doesn't matter, it'll still solve its purpose)
	m_mapStrToPtr.SetAt(poptel->pch, (void *)posOurWord);
}

void COptionList::Append(const TCHAR * pch, int cch)
{
	OptEl * poptel = (OptEl *)new BYTE[sizeof(OptEl) + cch];
	if (poptel == (OptEl *)NULL)	return;

	// initialise with a ref. count of 1
	memcpy(poptel->pch, pch, cch);
	poptel->pch[cch] = _T('\0');
	poptel->cch = cch + 1;
	poptel->cref = 0;

	// can we add, ie. 
	// do we want to check for duplicates?
	if (!Exists(poptel->pch))
		Add(poptel);	// put this into our list
	else
		delete [] (BYTE *)poptel;	// duplicate
}

void COptionList::Append(COptionList & optlst)
{
	VCPOSITION pos = optlst.m_lstStr.GetHeadPosition();
	while (pos != NULL)
	{
		OptEl * poptel = (OptEl *)optlst.m_lstStr.GetNext(pos);
		if (!Exists(poptel->pch))
			Add(poptel);
	}
}

void COptionList::Common(COptionList & optlst)
{
	BOOL bOK = TRUE;
	VCPOSITION pos = m_lstStr.GetHeadPosition(), posCurrent;
	while (pos != (VCPOSITION)NULL)
	{
	 	posCurrent = pos;
		OptEl * poptel = (OptEl *)m_lstStr.GetNext(pos);
		if (!optlst.Exists(poptel->pch))
		{
			bOK = Delete(poptel, posCurrent);
			VSASSERT(bOK, "Failed to delete option!");
		}
	}
}

BOOL COptionList::Delete(OptEl * poptel, VCPOSITION posOurWord)
{
	// pre-loaded position to delete, or found this OptEl?
	if (!posOurWord && !m_mapStrToPtr.Lookup(poptel->pch, (void *&)posOurWord))
		return FALSE;	// no

	// retrieve our optel for this (might be the same)
	poptel = (OptEl *)m_lstStr.GetAt(posOurWord);

	m_lstStr.RemoveAt(posOurWord);
	(void) m_mapStrToPtr.RemoveKey(poptel->pch);

	m_cchStr -= poptel->cch;
	poptel->cref--;	// decr. the ref. count
	if (poptel->cref == 0)	// if zero, then free-up the element
		delete [] (BYTE *)poptel;

	return TRUE;
}

BOOL COptionList::Subtract(const TCHAR * pch)
{
	// does this exist?
	VCPOSITION posOurWord;
	if (!m_mapStrToPtr.Lookup(pch, (void *&)posOurWord))	return FALSE;	// no

	// remove this from our list
	BOOL bOK = Delete((OptEl *)m_lstStr.GetAt(posOurWord), posOurWord);
	VSASSERT(bOK, "Failed to remove item from option list!");

	return TRUE; // ok
}

BOOL COptionList::Subtract(COptionList & optlst)
{
	BOOL fFound = FALSE;

	VCPOSITION pos = optlst.m_lstStr.GetHeadPosition();
	while (pos != (VCPOSITION)NULL)
	{
		OptEl * poptel = (OptEl *)optlst.m_lstStr.GetNext(pos);
		// removed this from our list?
		if (Delete(poptel))
			fFound = TRUE;
	}

	return fFound;
}

void COptionList::Components(const TCHAR * pch, COptionList & optlstAdd, COptionList & optlstSub)
{
	COptionList optlst(_T(' '), FALSE);

	// set the option string to find components for
	optlst.m_strJoin = m_strJoin;
	optlst.SetString(pch);

	VCPOSITION pos = m_lstStr.GetHeadPosition();
	while (pos != (VCPOSITION)NULL)
	{
		OptEl * poptel = (OptEl *)m_lstStr.GetNext(pos);
		// exist in us?
		if (!optlst.Delete(poptel))		// can do an addition comp.?
			optlstSub.Add(poptel);		// must be a subtract comp.
	}

	// return
	optlstAdd = optlst;
}

CDefOptionProp::CDefOptionProp(WORD idProp, int n, SUBTYPE st, OptType ot)
{
	m_pprop = (st == boolST) ? (CProp *) new CBoolProp(n) : (CProp *) new CIntProp(n);
	subtypeprop = st;
	m_opttype = ot;
}

CDefOptionProp::CDefOptionProp(WORD idProp, const char * sz, SUBTYPE st, OptType ot)
{
	m_pprop = (CProp *) new CStringProp(sz);
	subtypeprop = st;
	m_opttype = ot;
} 

#pragma function(memcpy)

//////////////////////////////////////////////////////////////////////////////
// implementation of the COptionHandler class
COptionHandler::COptionHandler(CBuildTool * pBuildTool, COptionHandler * popthdlrBase)
{
	m_pAssociatedBuildTool = pBuildTool;
	m_popthdlrBase = (COptionHandler *)popthdlrBase;	// no base option handler

	// make sure we got a valid build tool
	ASSERT_VALID(m_pAssociatedBuildTool);

	// Initialize our hierachy depth
	m_cDepth = 0;

	// Init. our slob stack
	m_sStk = 0;

	// assoc. ourselves with the build tool
	m_pAssociatedBuildTool->SetOptionHandler(this);

	// our associated CSlobs
	m_pSlob = (CSlob *)NULL;
}

COptionHandler::COptionHandler
(
	const TCHAR * szPkg, WORD id,
	const TCHAR * szPkgTool, WORD idTool,
	const TCHAR * szPkgBase, WORD idBase
)
{
	// set our id.
	SetId(GenerateComponentId(g_pPrjcompmgr->GenerateBldSysCompPackageId(szPkg), id));

	// Initialize our hierachy depth
	m_cDepth = 0;

	// Init. our slob stack
	m_sStk = 0;

	// our associated build tool (if we have one)
	m_pAssociatedBuildTool = (CBuildTool *)NULL;
	if (idTool != 0)
	{
		if (!g_pPrjcompmgr->LookupBldSysComp(GenerateComponentId(g_pPrjcompmgr->GenerateBldSysCompPackageId(szPkgTool), idTool),
										   (CBldSysCmp *&)m_pAssociatedBuildTool))
			VSASSERT(FALSE, "Failed to find associated build tool");	// failed

		// make sure we got a valid build tool
		ASSERT_VALID(m_pAssociatedBuildTool);
		VSASSERT(m_pAssociatedBuildTool->IsKindOf(RUNTIME_CLASS(CBuildTool)), "Associated build tool must be a CBuildTool");

		// assoc. ourselves with the build tool
		if(m_pAssociatedBuildTool)
		{
			m_pAssociatedBuildTool->SetOptionHandler(this);
		}
	}

	// our base option handler (if we have one)
	m_popthdlrBase = (COptionHandler *)NULL;
	if (idBase != 0)
	{
		if (!g_pPrjcompmgr->LookupBldSysComp(GenerateComponentId(g_pPrjcompmgr->GenerateBldSysCompPackageId(szPkgBase), idBase),
										   (CBldSysCmp *&)m_popthdlrBase))
			VSASSERT(FALSE, "Failed to find option handler");	// failed

		// make sure we got a valid base option handler
		ASSERT_VALID(m_popthdlrBase);
		VSASSERT(m_popthdlrBase->IsKindOf(RUNTIME_CLASS(COptionHandler)), "Option handler must a COptionHandler");

		// calc. our depth
		COptionHandler * popthdlr = m_popthdlrBase;
		while (popthdlr)
		{
			m_cDepth++;	// inc. depth (we have at least 1)
			popthdlr = popthdlr->GetBaseOptionHandler();
		}

		// If we have a tool then reset the option handler
		// as our depth has now changed. The depth must be
		// in ssync with the option handler in the tool.
		if (idTool != 0)
			m_pAssociatedBuildTool->SetOptionHandler(this);
	}


	// our associated CSlobs
	m_pSlob = (CSlob *)NULL;
}

COptionHandler::~COptionHandler()
{
	WORD key;
	CDefOptionProp * pdefopt;

	// destroy all of our default props for the map
	for (VCPOSITION pos = m_mapDefOptions.GetStartPosition() ; pos != NULL;)
	{
		m_mapDefOptions.GetNextAssoc(pos, key, (void *&)pdefopt);
		delete pdefopt;
	}

	m_mapDefOptions.RemoveAll();

	VSASSERT(m_pdirtyProps, "Dirty props cannot be NULL");
	delete m_pdirtyProps;
	delete [] m_pStrListPropIds;
}

BOOL COptionHandler::Initialise()
{
	// convert all table logical props to actual props
	COptStr	* poptstr = GetOptionStringTable();
	while (poptstr->idOption != (UINT)-1)
	{
		for (WORD cArgs = 0; cArgs < MAX_OPT_ARGS; cArgs++)
		{
			UINT * pidArg = poptstr->rgidArg + cArgs;
			if (*pidArg != (UINT)-1)
				*pidArg = MapLogical(*pidArg);
		}

		poptstr++;	// next
	}

	// create our default option map
	UINT nRange = GetOptPropRange();

	COptionLookup * poptlookup = GetOptionLookup();
	if ((poptlookup == (COptionLookup *)NULL) || GetOptionLookup()->FInit(GetOptionStringTable()))
	{
		// make the map hash size close to a prime no. matching the # of props.
		UINT nHash = (UINT)-1;
		if (nHash > 90)
			nHash = 89;
		else if (nHash > 60)
			nHash = 59;
		else if (nHash > 30)
			nHash = 29;
		if (nHash != (UINT)-1)
			m_mapDefOptions.InitHashTable(nHash);

		// fill the default option map
		CreateOptDefMap();
	}
	
	// create our dirty prop array
	m_pdirtyProps = new CVCByteArray; m_pdirtyProps->SetSize(nRange);
	m_pStrListPropIds = new UINT[nRange];	// max. required

	// create our cache of string list props
	m_cStrListPropIds = 0;
	UINT idPropMin, idPropMax; GetMinMaxOptProps(idPropMin, idPropMax);
	for (UINT idProp = idPropMin; idProp <= idPropMax; idProp++)
		if (IsListStrProp(idProp))
			m_pStrListPropIds[m_cStrListPropIds++] = idProp;

	return TRUE;	// ok
}

void COptionHandler::GetSpecialOptProps(UINT & nIDUnkOpt, UINT & nIDUnkStr, OptBehaviour optbeh)
{
	UINT nIDUnkOptB, nIDUnkStrB;
	GetSpecialLogicalOptProps(nIDUnkOpt, nIDUnkStr, optbeh);

	COptionHandler * popthdlr = GetBaseOptionHandler();

	if (nIDUnkOpt != (UINT)-1)
	{
		if (nIDUnkOpt == (UINT)NULL && popthdlr != (COptionHandler *)NULL)
			popthdlr->GetSpecialOptProps(nIDUnkOpt, nIDUnkStrB);
		else
			nIDUnkOpt = MapLogical(nIDUnkOpt);
	}
		
	if (nIDUnkStr != (UINT)-1)
	{
		if (nIDUnkStr == (UINT)NULL && popthdlr != (COptionHandler *)NULL)
			popthdlr->GetSpecialOptProps(nIDUnkOptB, nIDUnkStr);
		else
			nIDUnkStr = MapLogical(nIDUnkStr);
	}
}

//
// add default tool option properties to our map
//
void COptionHandler::AddDefStrProp(UINT idProp, const char * sz, OptType ot)
{
	m_mapDefOptions.SetAt((WORD)idProp, new CDefOptionProp((WORD)idProp, sz, strST, ot));
}

void COptionHandler::AddDefPathProp(UINT idProp, const char * sz, OptType ot)
{
	m_mapDefOptions.SetAt((WORD)idProp, new CDefOptionProp((WORD)idProp, sz, pathST, ot));
}

void COptionHandler::AddDefDirProp(UINT idProp, const char * sz, OptType ot)
{
	m_mapDefOptions.SetAt((WORD)idProp, new CDefOptionProp((WORD)idProp, sz, dirST, ot));
}

void COptionHandler::AddDefIntProp(UINT idProp, int n, OptType ot)
{
	m_mapDefOptions.SetAt((WORD)idProp, new CDefOptionProp((WORD)idProp, n, intST, ot));
}

void COptionHandler::AddDefBoolProp(UINT idProp, BOOL b, OptType ot)
{
	m_mapDefOptions.SetAt((WORD)idProp, new CDefOptionProp((WORD)idProp, b, boolST, ot));
}
																	
void COptionHandler::AddDefHexProp(UINT idProp, BOOL b, OptType ot)
{
	m_mapDefOptions.SetAt((WORD)idProp, new CDefOptionProp((WORD)idProp, b, hexST, ot));
}

GPT COptionHandler::GetDefIntProp(UINT idProp, int & val)
{
	CDefOptionProp * pdefopt;
	if (!m_mapDefOptions.Lookup((WORD)idProp, (void *&)pdefopt)) return invalid;

	if (pdefopt->m_pprop->m_nType == string)
	{
		if (!ConvertFromStr(idProp, ((CStringProp *)pdefopt->m_pprop)->m_strVal, val))
			return invalid;
	}
	else
	{
		VSASSERT(pdefopt->m_pprop->m_nType == booln || pdefopt->m_pprop->m_nType == integer, "Can only call GetDefIntProp with bool or int props!");
		val = (pdefopt->m_pprop->m_nType == booln) ?
			   ((CBoolProp *)pdefopt->m_pprop)->m_bVal : ((CIntProp *)pdefopt->m_pprop)->m_nVal;
	}

	return valid;
}

GPT COptionHandler::GetDefStrProp(UINT idProp, CString & val)
{
	CDefOptionProp * pdefopt;
	if (!m_mapDefOptions.Lookup((WORD)idProp, (void *&)pdefopt)) return invalid;

	if (pdefopt->m_pprop->m_nType == integer)
	{
		if (!ConvertToStr(idProp, ((CIntProp *)pdefopt->m_pprop)->m_nVal, val))
			return invalid;
	}
	else
	{
		VSASSERT(pdefopt->m_pprop->m_nType == string, "Can only call GetDefStrProp with string props!");
		val = ((CStringProp *)pdefopt->m_pprop)->m_strVal;
	}

	return valid;
}

BOOL COptionHandler::IsDefaultIntProp(UINT idProp, int & nVal)
{
	int nOurVal;
	return (GetDefIntProp(idProp, nOurVal) == valid) &&
		   (nOurVal == nVal);
}

BOOL COptionHandler::IsDefaultStringProp(UINT idProp, CString & strVal)
{
	CString strOurVal;
	if (GetDefStrProp(idProp, strOurVal) != valid)
		return FALSE;

   TCHAR * pch1 = (TCHAR *)(const TCHAR *)strOurVal;
   TCHAR * pch2 = (TCHAR *)(const TCHAR *)strVal;
   BOOL fEscaped = FALSE;
   TCHAR ch1, ch2;
   while ((ch1 = *pch1) != _T('\0') && (ch2 = *pch2) != _T('\0'))
   {
		// case insensitive compare (most likely for tool options)
   		if (_totupper(ch1) != _totupper(ch2))
   		{
			// treat a '\' and an '/' as equivalent if not escaped '\'
			// this is the case for common option args which are filenames
			if (fEscaped ||
				(ch1 != _T('\\') && ch1 != _T('/')) ||
				(ch2 != _T('\\') && ch2 != _T('/'))
			   )
	   			return FALSE;	// failed
   		}

		size_t cch = _tclen(pch1);
		pch1 += cch; pch2 += cch;

   		fEscaped = !fEscaped && (ch1 == _T('\\'));
   }

   return *pch1 == _T('\0') && *pch2 == _T('\0');
}

// conversion of int, oct,hex values into a text form
// FUTURE: I'd like to put this elsewhere, but where?
BOOL COptionHandler::ConvertToStr(UINT idProp, int nVal, CString & strVal)
{
	CDefOptionProp * pdefopt;
	if (!m_mapDefOptions.Lookup((WORD)idProp, (void *&)pdefopt)) return FALSE;

	SUBTYPE subtype = pdefopt->subtypeprop;
	int nBase;
	if (subtype == hexST)
	{
		nBase = 16; strVal = "0x";
	}
	else if (subtype == octST)
	{
		nBase = 8; strVal = "0";
	}
	else
	{
		VSASSERT(subtype == intST, "Invalid subtype");
		nBase = 10; strVal = "";
	}

	char	szVal[34];	// max. of 33 chars

	// we use unsigned longs, no negative ints allowed
	strVal += _ultoa((unsigned long)(unsigned int)nVal, szVal, nBase);

	return TRUE;
}

// what terminators do we have?
// o zero terminator ('\0') for regular number strings
// o '>' terminator for option table number strings
#define IsStrTerminator(ch) (ch == _T('\0') || ch == _T('>'))

BOOL COptionHandler::ConvertFromStr(UINT idProp, const TCHAR * pchVal, int & nVal)
{
	CDefOptionProp * pdefopt;
	if (!m_mapDefOptions.Lookup((WORD)idProp, (void *&)pdefopt)) return FALSE;

	SUBTYPE subtype = pdefopt->subtypeprop;
	TCHAR * pch = (TCHAR *)pchVal;

	// strip the prefix
	if (subtype == hexST)
	{
		// we'll allow a leading 'x' (a hack BTW)
		if (*pchVal == _T('x') || *pchVal == _T('X'))
			pch++;
		else if (*pchVal == _T('0') && (*(pchVal+1) == _T('x') || *(pchVal+1) == _T('X')))
			pch += 2;
		else
			subtype = intST;	// no leading hex prefix, we'll assume decimal
	}
	else if (subtype == octST)
	{
		// we'll allow a leading '0' (a hack BTW)
		if (*pchVal == _T('0'))
			pch++;
	}
	else
	{
		VSASSERT(subtype == intST, "Invalid subtype");
		if (*pchVal == _T('+'))
			pch++;	// '+' is a single byte DBC
	}

	UINT iOut = 0;

	// init. the value
	nVal = 0;

	if (subtype == hexST)
	{
		for (; !IsStrTerminator(pch[iOut]); iOut++)
		{
			// check for well-formed hex number
			if (iOut > 7 || !isxdigit((unsigned char)pch[iOut]) || nVal & 0xf0000000)
			{
				// not a hex-digit or 
				// too many hex-digits (only 32-bit, ie. 8 digit hex-numbers supported) or
				// next shift would cause overflow!
				return FALSE;
			}

			nVal <<= 4;
			nVal += (toupper((unsigned char)pch[iOut]) -
					 (isdigit(toupper((unsigned char)pch[iOut])) ? '0' : 'A' - 10));
		}
	}
	else if (subtype == octST)
	{
		for (; !IsStrTerminator(pch[iOut]); iOut++)
		{
			// check for well-formed hex number
			if (iOut > 10 || !isdigit((unsigned char)pch[iOut]) ||
				pch[iOut] == _T('8') || pch[iOut] == _T('9') || nVal & 0xe0000000)
			{
				// we found '8' or '9'
				// too may oct-digits (only 32-bit, ie. 11 digit oct-numbers supported)
				// next shift would cause overflow!
				return FALSE;
			}

			nVal <<= 3;
			nVal += (toupper((unsigned char)pch[iOut]) - '0');
		}
	}
	else
	{
		VSASSERT(subtype == intST, "Invalid subtype");

		for (; !IsStrTerminator(pch[iOut]); iOut++)
		{
			// check for well-formed dec. number
			if (iOut > 9 || !isdigit((unsigned char)pch[iOut]))
			{
				// only want positive numbers
				// too may decimal digits (only 32-bit, ie. 10 digit dec-numbers supported)
				return FALSE;
			}

  			int nDigit = (toupper((unsigned char)pch[iOut]) - '0');
			
			// possible overflow?
			if ((nVal >= 429496729 && nDigit > 5) || (nVal > 429496730))
			{
				// number too big (only 32-bit, max. is 4,294,967,295)
				return FALSE;
			}

			nVal *= 10;
			nVal += nDigit;
		}
	}

	return TRUE;
}

void COptionHandler::MungeListStrProp
(
	UINT nIDProp,
	const CString & strVal,
	BOOL fAppend,
	char chJoin
)
{
	// get our comma-sep list
	CString str;
	m_pSlob->GetStrProp(nIDProp, str);
	COptionList optlst(str, chJoin);

	// perform munge
	optlst.Subtract((const TCHAR *)strVal);
	if (fAppend)
		optlst.Append((const TCHAR *)strVal, (int)strVal.GetLength());

	// set our 'munged' list
	optlst.GetString(str);
	m_pSlob->SetStrProp(nIDProp, str);
}

void COptionHandler::SetListStrProp
(
	CSlob * pSlob,
	UINT nIDExtraProp,
	CString & strVal,
	BOOL fInherit,
	BOOL fAnti,
	TCHAR chJoin
)
{
	// do nothing
	if (pSlob == (CSlob *)NULL)	return;

	CPropBag * pBag = pSlob->GetPropBag();

	// it is easy to set an anti prop
	if (fAnti)
	{
		pBag->SetStrProp(pSlob, ListStr_SubPart(nIDExtraProp), strVal);
	}
	else
	{
		// get the currently existing option list for our parent
		CString strParent; strParent = "";
		CSlob * pSlobParent = ((CProjItem *)pSlob)->GetContainerInSameConfig();
		if (pSlobParent != (CSlob *)NULL)
		{
			(void) GetListStrProp(pSlobParent, nIDExtraProp, strParent, fInherit, fAnti, chJoin);
			((CProjItem *)pSlobParent)->ResetContainerConfig();
		}

		// now figure out what we have subtracted or added
		COptionList optlstParent(strParent, chJoin);
		COptionList optlstAdd(chJoin), optlstSub(chJoin);

		// break into the component parts
		optlstParent.Components(strVal, optlstAdd, optlstSub);

		// we now have our add and subtract, convert back into strings
		CString strAddPart, strSubPart;
		optlstAdd.GetString(strAddPart);
		optlstSub.GetString(strSubPart);

		// set the properties
		pBag->SetStrProp(pSlob, ListStr_AddPart(nIDExtraProp), strAddPart);
		pBag->SetStrProp(pSlob, ListStr_SubPart(nIDExtraProp), strSubPart);
	}

	// inform people of a change to pseudo prop. nIDExtraProp
	pSlob->InformDependants(nIDExtraProp);
}

GPT COptionHandler::GetListStrProp
(
	CSlob * pOrigSlob,
	UINT nIDExtraProp,
	CString & strVal,
	BOOL fInherit,
	BOOL fAnti,
	char chJoin
)
{
	// do nothing?
	if (pOrigSlob == (CSlob *)NULL)	return invalid;

	// clear the buffer ready for concatenation
	strVal = "";
	
	// won't allow inheritance plus anti for string lists
	if (fAnti)	fInherit = FALSE;

	CSlob * pSlob = pOrigSlob;
	CObList obList;
	if (fInherit)
	{
		// we are a CProjItem and we are inheriting!
		// get the prop for each of our ancestor's in elder first order
		while (pSlob != (CSlob *)NULL)
		{
			obList.AddHead(pSlob);
			pSlob = ((CProjItem *)pSlob)->GetContainerInSameConfig();	// get our container
		}
	}
	else
	{
		// we can do a no-inheritance thing!
		obList.AddHead(pSlob);
	}

	// if we are a CProjItem and we are inheriting then do our own thing
	// ie. go through each CSlob in the container hierarchy and append each part to 
	// our options

	COptionList optlstVal(chJoin);
	POSITION pos = obList.GetHeadPosition();
	while (pos != (POSITION)NULL)
	{
		pSlob = (CSlob *)obList.GetNext(pos);

		// CProjGroups don't support tool options!
		if (!pSlob->IsKindOf(RUNTIME_CLASS(CProjGroup)))
		{
			CPropBag * pBag = pSlob->GetPropBag();
			CStringProp * pStrProp;

			// disable inheriting so we can do it!
			BOOL fPropInheritOld = ((CProjItem *)pSlob)->EnablePropInherit(FALSE);

			// using string lists
			COptionList optlstSub(chJoin);
			if ((pStrProp = (CStringProp *)pBag->FindProp(ListStr_SubPart(nIDExtraProp))) != (CStringProp *)NULL)
				optlstSub.SetString(pStrProp->m_strVal);

			// if *not* anti,
			// - append our 'add part'
			// - subtract our 'subtract part'
			// else
			// - add our 'subtract part'
			if (fAnti) /* anti-option */
			{
				CString strParent;
				CSlob * pSlobParent = ((CProjItem *)pSlob)->GetContainerInSameConfig();
				if (pSlobParent != (CSlob *)NULL)
				{
					(void) GetListStrProp(pSlobParent, nIDExtraProp, strParent, TRUE, FALSE);
					((CProjItem *)pSlobParent)->ResetContainerConfig();
				}

				// remove from the subtract option anything that
				// doesn't exist in our parent's list
				COptionList optlstParent(strParent, chJoin);
				optlstSub.Common(optlstParent);
				optlstVal.Append(optlstSub);
			}
			else /* not anti-option */
			{
				COptionList optlstAdd(chJoin);
				if ((pStrProp = (CStringProp *)pBag->FindProp(ListStr_AddPart(nIDExtraProp))) != (CStringProp *)NULL)
					optlstAdd.SetString(pStrProp->m_strVal);

				if (fInherit)	optlstVal.Subtract(optlstSub);	// no subtract, if no inherit
				optlstVal.Append(optlstAdd);
			}

			// re-set inheriting to old value
			((CProjItem *)pSlob)->EnablePropInherit(fPropInheritOld);
		}

		// only need to do this if we we'rere a got container
		// (ie. we're not the first node in this list)
		if (pSlob != pOrigSlob)
			((CProjItem *)pSlob)->ResetContainerConfig();
	}
	
	if (!fAnti && IsFakeProp(nIDExtraProp))
	{
		CString strAdd;

		SetSlob (pOrigSlob);
		GetDefStrProp (nIDExtraProp, strAdd);
		COptionList optlstAdd (strAdd);
		ResetSlob();

		optlstVal.Append (optlstAdd);
	} 
	// we need to convert our string list into a usable string
	// this will ensure all our join chars are chJoin
	optlstVal.GetString(strVal);
	
	return valid;
}

BOOL CompareProps(const CProp *, const CProp *); // Rvw: defined in projitem.cpp

//////////////////////////////////////////////////////////////////////////////
// implementation of the COptHdlrUnknown class
// mechanism to allow for unknown option handlers

// we use this to retrieve a unique option property value each
// time an 'Uknown' option handler is CTORed
UINT g_nIDOptHdlrUnknown = 0;

// small option string table used to parse and generate uknown option lists
static COptStr g_poptstrOptHdlrUknown[] = {
	IDOPT_UNKNOWN_STRING,	"",		NO_OPTARGS, single,
	(UINT)-1,				NULL,	NO_OPTARGS, single
};

#define P_UnkHdkr_UnkStr_AddL	0
#define P_UnkHdlr_UnkStr_SubL	1

COptHdlrUnknown::COptHdlrUnknown(CBuildTool * pBuildTool) : COptionHandler(pBuildTool)
{
	// our base
	SetOptPropBase(PROJ_OPT_UNKNOWN + g_nIDOptHdlrUnknown);

	// we'll be using 2 off from this base
	// logical prop 0 is unknown string (add component)	== P_UnkHdkr_UnkStr_AddL
	// logical prop 1 is unknown string (subtract component) == P_UnkHdlr_UnkStr_SubL
	g_nIDOptHdlrUnknown += 2;

	VSASSERT(g_nIDOptHdlrUnknown <= 100, "Option handler list too long; bump up if it really needs to be bigger"); // 100 should be plenty (allow for 50 unknown tools)

	Initialise();
}	

void COptHdlrUnknown::GetSpecialLogicalOptProps(UINT & nIDUnkOpt, UINT & nIDUnkStr, OptBehaviour optbeh)
{
	ASSERT_VALID(m_pSlob);

	nIDUnkOpt = (UINT) -1;
	nIDUnkStr = (optbeh & OBAnti) ? P_UnkHdlr_UnkStr_SubL : P_UnkHdkr_UnkStr_AddL;
}

// retrieive our min. and max. option prop IDs
void COptHdlrUnknown::GetMinMaxLogicalOptProps(UINT & nIDMinProp, UINT & nIDMaxProp)
{
	nIDMinProp = P_UnkHdkr_UnkStr_AddL; nIDMaxProp = P_UnkHdlr_UnkStr_SubL;
}

COptStr * COptHdlrUnknown::GetOptionStringTable()
{
	return (COptStr *)g_poptstrOptHdlrUknown;
}

//////////////////////////////////////////////////////////////////////////////
// implementation of the COptHdlrMapper class
COptHdlrMapper::COptHdlrMapper()
{
}

COptHdlrMapper::~COptHdlrMapper()
{
}

// get the right mapper option handler (used by COptHdlrMapper::MapLogical())
void COptHdlrMapper::SetMapperOptionHandler(UINT cDepth)
{
	UINT cStep = m_popthdlr->GetDepth() - cDepth;  
	m_popthdlrMapper = m_popthdlr;
	while (cStep--)
		m_popthdlrMapper = m_popthdlrMapper->GetBaseOptionHandler();

	m_cDepth = cDepth;
}

//////////////////////////////////////////////////////////////////////////////
// implementation of the COptionLookup class
// optimized lookup for the options
COptionLookup::~COptionLookup()
{
	WORD		keyDummy;
	CLookupEntry *	pentry;
	VCPOSITION pos = m_mapChToPtr.GetStartPosition();

	while (pos != (VCPOSITION)NULL)
	{
		 m_mapChToPtr.GetNextAssoc(pos, keyDummy, (void *&)pentry);
		 delete pentry;
	}
}

BOOL COptionLookup::FInit(COptStr * poptstr)
{
	COptStr	* poptstrOur = poptstr;

	//
	// create our lookup struct. for 'based on ID'
	//

	// what is the lowest ID we have in this table?
	m_idLowest = UINT_MAX;	// this is the highest
	while (poptstrOur->idOption != (UINT)-1)
	{
		// no need to do these!
		if (poptstrOur->idOption != IDOPT_UNKNOWN_OPTION &&
	    	poptstrOur->idOption != IDOPT_UNKNOWN_STRING)				
		{
			if (poptstrOur->idOption < m_idLowest)	m_idLowest = poptstrOur->idOption;
		}
		poptstrOur++;
	}

	// now using this lowest ID as the base create our
	// pointer array
	poptstrOur = poptstr;
	while (poptstrOur->idOption != (UINT)-1)
	{
		// no need to do these
		if (poptstrOur->idOption != IDOPT_UNKNOWN_OPTION &&
	    	poptstrOur->idOption != IDOPT_UNKNOWN_STRING)				
		{
			m_ptrIDArray.SetAtGrow(poptstrOur->idOption - m_idLowest, (void *)poptstrOur);
		}
		poptstrOur++;
	}

	// free up any extra
	m_ptrIDArray.FreeExtra();

	//
	// create our lookup struct. for 'based on char'
	//

	// init. our lookup char.
	m_chLookup = _T('\0');

	// now use an insertion sort to alphabetically sort our options (on first letter only)

	int iChArray = 0;
	unsigned char ch;

	poptstrOur = poptstr;

	while (poptstrOur->idOption != (UINT)-1)
	{
		// don't do this for our special ones!
		if (poptstrOur->idOption == IDOPT_UNKNOWN_OPTION ||
		    poptstrOur->idOption == IDOPT_UNKNOWN_STRING)
		{
			poptstrOur++;
			continue;
		}

		COptStr * poptstr;

		ch = (unsigned char)*(poptstrOur->szOption);

		iChArray = 0;
		while (iChArray <= m_ptrChArray.GetUpperBound())
		{
			poptstr = (COptStr *)m_ptrChArray[iChArray];

			// UNICODE ALERT:             
 			if ((unsigned char)*(poptstr->szOption) > ch)
				break; // break-out here

			iChArray++;
		}

		// insert at this position
		m_ptrChArray.InsertAt(iChArray, (void *)poptstrOur);
		poptstrOur++;
	}

	// free up any extra
	m_ptrChArray.FreeExtra();

	// now go through our list once more and for each first char.
	// put it's index into our map

	CLookupEntry * pentry = (CLookupEntry *)NULL;
	iChArray = 0; ch = _T('\0');	// last char.

	m_mapChToPtr.InitHashTable(53);	// prime no. >= 26 * 2 (alpha letters upper+lower)
	while (iChArray <= m_ptrChArray.GetUpperBound())
	{
		// don't do this for our special ones!
		if (poptstrOur->idOption == IDOPT_UNKNOWN_OPTION ||
		    poptstrOur->idOption == IDOPT_UNKNOWN_STRING)
		{
			iChArray++;
			continue;
		}

		COptStr * poptstr;

		poptstr = (COptStr *)m_ptrChArray[iChArray];

		// UNICODE ALERT:             
		// N.B. guaranteed to hit on first iteration
		if ((unsigned char)*(poptstr->szOption) != ch)
		{
			// fill in 'first char'
			pentry = new CLookupEntry;
			pentry->poptstr = poptstr;
			pentry->ioptstr = (UINT)-1;	// by default next is not available

			ch = (unsigned char)*(poptstr->szOption);
			m_mapChToPtr.SetAt((WORD)ch, (void *)pentry);
		}
		else if (pentry != (CLookupEntry *)NULL)
		{
			// fill in 'next char'
			pentry->ioptstr = iChArray;
			pentry = (CLookupEntry *)NULL;
		}

 		iChArray++;
	}

	return TRUE;
}

inline COptStr * COptionLookup::Lookup(UINT nID)
{
	// no need to do these!
	if (nID == IDOPT_UNKNOWN_OPTION ||
    	nID == IDOPT_UNKNOWN_STRING)
	{
		VSASSERT(FALSE, "Should not be doing lookup for unknown option or unknown string via Lookup!");
		return (COptStr *)NULL;
	}
	
	// must be within our bounds
	if ((nID >= m_idLowest) && (m_ptrIDArray.GetUpperBound() >= (int)(nID - m_idLowest)))
		return (COptStr *)m_ptrIDArray[nID - m_idLowest];

	return (COptStr *)NULL;	 // not found
}

inline COptStr * COptionLookup::LookupFirst(unsigned char ch, BOOL fCaseSensitive)
{
	CLookupEntry * pentry;

	if (m_mapChToPtr.Lookup((WORD)ch, (void *&)pentry) ||
		(!fCaseSensitive &&
		 m_mapChToPtr.Lookup((WORD)(isupper(ch) ? tolower(ch) : toupper(ch)), (void *&)pentry)))
	{
		// remember our char. plus first position
		m_chLookup = ch; m_ioptstr = pentry->ioptstr;
		return pentry->poptstr;
	}

	return (COptStr *)NULL;
}

inline COptStr * COptionLookup::LookupNext(INT_PTR cchLead)
{
	// no lookup entry, or no next one to lookup
	if (m_ioptstr == (UINT)-1)	return (COptStr *)NULL;

	COptStr * poptstr = (COptStr *)m_ptrChArray[m_ioptstr];

	// is this next one valid?
	TCHAR * pch = (TCHAR *)poptstr->szOption;
	TCHAR ch = *pch;
	if (ch == m_chLookup ||
		((isupper(ch) ? tolower(ch) : toupper(ch)) == m_chLookup))
	{
		if ((int)++m_ioptstr > m_ptrChArray.GetUpperBound())
			m_ioptstr = (UINT)-1;
	}
	else
		m_ioptstr = (UINT)-1;	// no longer available

	// check the lead char. minimum
	if (poptstr)
	{
		int cchThisLead = 0;
		while (*pch != _T('\0') && *pch != _T('%'))
		{
			cchThisLead++;
			pch++;	// N.B. no Kanjii in tool option specifications
		}

		if (cchLead > cchThisLead)
			poptstr = (COptStr *)NULL;
	}
                          
	return poptstr;
}

// implementation of the COptionTable class

COptionTable::COptionTable()
{
	// init. our arg property list index
	for (UINT i = 0 ; i < MAX_OPT_ARGS ; i++)
		m_rgProps[i].idArg = (UINT)-1;

	// our list string cache
	m_pBagCache = (CPropBag *)NULL;

	// our default option prefixes
	strPrefixes = "/-";

	// our arg. value allocated to be 64
	pchArgValue = new char [cchArgValue = 64];

	// set up our default option behaviour
	m_optbeh = OBNone;
}

COptionTable::~COptionTable()
{
	// delete our arg property list
	ClearPropValList();

	// delete our arg. value if we had one
	if (pchArgValue != (TCHAR *)NULL)
		delete [] pchArgValue;
}


// set the option handler given a package name and component pair
COptionHandler * COptionTable::SetOptionHandler(const TCHAR * szPkg, WORD idOptHdlr)
{
	// generate this option handler's component id
	DWORD bsc_idOptHdlr = GenerateComponentId(g_pPrjcompmgr->GenerateBldSysCompPackageId(szPkg), idOptHdlr);
	
	// look it up
	COptionHandler * popthdlr;
	if (!g_pPrjcompmgr->LookupBldSysComp(bsc_idOptHdlr, (CBldSysCmp * &)popthdlr))
		popthdlr = (COptionHandler *)NULL;

	VSASSERT(popthdlr->IsKindOf(RUNTIME_CLASS(COptionHandler)), "Option handler must be a COptionHandler");

	// set this option handler?
	if (popthdlr != (COptionHandler *)NULL)
		SetOptionHandler(popthdlr);

	// return it
	return popthdlr;
}

BOOL COptionTable::ParseString(CString & strOption, OptBehaviour optbeh)
{
	VSASSERT(m_popthdlr, "Option handler NULL!");

	// set our option flags
	m_optbeh = optbeh;

	// can't have set defaults for root CSlob's
	if (m_pSlob->GetContainer() == (CSlob *)NULL)
		m_optbeh &= ~OBSetDefault;

	// don't clear props for anti-options!
	if (m_optbeh & OBAnti)
		m_optbeh &= ~OBClear;

 	// set the prop. bag usage
	int idOldBag = ((CProjItem *)m_pSlob)->UsePropertyBag((m_optbeh & OBBasePropBag) ? BaseBag : CurrBag);

	// create our unknown option/string buffers
	CString	strUnknownOption, strUnknownString, strPart;
	UINT nIDUnknownOption, nIDUnknownString;
	m_popthdlr->GetSpecialOptProps(nIDUnknownOption, nIDUnknownString, m_optbeh);

	// We want to append to any previously-read options/strings for this projitem
	// N.B. AppWizard depends on this behavior!

	// append any previous unknown strings/options if we are not clearing
	// ie. this might be multiple Add/Subtract lines in the makefile
	// (AppWizard generates .MAKs like this)
	if (!(m_optbeh & OBClear))
	{
		// don't want inheritance enabled for this
		OptBehaviour optbehOld = ((CProjItem *)m_pSlob)->GetOptBehaviour();
		((CProjItem *)m_pSlob)->SetOptBehaviour((OptBehaviour)(optbehOld & ~OBInherit));

		if (nIDUnknownOption != (UINT)-1 &&
			m_pSlob->GetStrProp(nIDUnknownOption, strUnknownOption) == valid && !strUnknownOption.IsEmpty()
		   )
			strUnknownOption += _T(' ');

		if (nIDUnknownString != (UINT)-1 &&
			m_pSlob->GetStrProp(nIDUnknownString, strUnknownString) == valid && !strUnknownString.IsEmpty()
		   )
			strUnknownString += _T(' ');

		((CProjItem *)m_pSlob)->SetOptBehaviour(optbehOld);
	}

	TCHAR chSrc, chPrefix = _T(' ');
	BOOL fReturn = TRUE;	// default is to pass

	// reset for this new option string
	strSrc = strOption; ichSrc = 0;

	// break out on parse completion, ie. found terminator (chSrc == _T('\0'))
	for (;;)
	{
		// skip any leading whitespace
		cchAdvSrc = 0; SkipWhiteSpace(); ichSrc += cchAdvSrc;

		// reset our advancement pointers
		cchAdvSrc = cchAdvSrcToArg = 0;

		chSrc = NewGetAt(strSrc, ichSrc);

		// parsed all of the source?
		if (chSrc == _T('\0'))	break;

		BOOL fStringTrash = TRUE;	// default is 'it is trash!'

		// are we on a option prefix?
		BOOL fHasPrefix = strPrefixes.Find(chSrc) != -1;
		if (fHasPrefix)
		{
			// remember this prefix
			chPrefix = chSrc;

			// yes, this is an option
			BOOL		fFoundMatch = FALSE;

			ichSrc++;	// move over prefix

			// our option to try and parse with
			COptStr * poptstr;

			// our current fast-lookup object (one for each option handler)
			COptionLookup * poptlookup;

			// for each of our option handler lookup objects do this
			COptionHandler * popthdlrOld = m_popthdlr;	// start off at ourselves

			while (!fFoundMatch && m_popthdlr != (COptionHandler *)NULL)
			{
				// cache some values from this option handler
				m_popthdlr->GetMinMaxOptProps(m_nPropMin, m_nPropMax);
				m_fCaseSensitive = m_popthdlr->IsCaseSensitive();

				// get our fast-lookup object
				poptlookup = m_popthdlr->GetOptionLookup();

				// yes, so try to match each of our options
				poptstr = poptlookup ?
						  poptlookup->LookupFirst(NewGetAt(strSrc, ichSrc), m_fCaseSensitive) : (COptStr *)NULL;

				// we share the lookup list with our base + derived option tables
				// is this a valid option string?
				INT_PTR cchMinLead = 0;
				while (!fFoundMatch && poptstr != (COptStr *)NULL)
				{
					fFoundMatch = MatchOption(poptstr);

					// did we find a match?
					if (fFoundMatch)
					{
						// yes, advance our source pointer
						ichSrc += cchAdvSrc;
					}
					else
					{
						if (cchAdvSrcToArg > cchMinLead)	cchMinLead = cchAdvSrcToArg;

						// no, get next option to try to match
						// (only get those options with *more* leading chars
						// we could match, ie. not /F% if we match /FI%)
						poptstr = poptlookup->LookupNext(cchMinLead);
					}

					cchAdvSrc = cchAdvSrcToArg = 0;	// reset our advancement pointers
				}

				m_popthdlr = m_popthdlr->GetBaseOptionHandler(); // get our base-handler
			}

			m_popthdlr = popthdlrOld;

			// did we find a match? ie. is this option trash?
			fStringTrash = !fFoundMatch;

			if (fStringTrash && nIDUnknownOption != (UINT)-1)
			{
				// collect together the unknown option
				strPart = ""; CollectOptionPart(strPart, FALSE, TRUE, FALSE, TRUE);

				// append part, terminate with a space
				AdjustBuffer(strUnknownOption);
				strUnknownOption += chPrefix;
				strUnknownOption += strPart;
				strUnknownOption += _T(' ');

				ichSrc += cchAdvSrc;	// advance our source pointer
				fStringTrash = FALSE;	// processed as 'option' string trash
			}
		}
		
		// is this just string trash?
		if (fStringTrash)
		{
			// collect together the trash (note: any collection of characters)
			strPart = ""; CollectOptionPart(strPart, FALSE, TRUE, TRUE, TRUE);
			ichSrc += cchAdvSrc;	// advance our source pointer

			// must have at least an unknown string unless we're Anti
			VSASSERT((nIDUnknownString != (UINT)- 1) || (m_optbeh & OBAnti), "Must have at least an unknown string unless we're Anti");
			if (nIDUnknownString != (UINT)-1)
			{
				// append part, terminate with a space
				AdjustBuffer(strUnknownString);
				if (fHasPrefix)	strUnknownString += chPrefix;
			  	strUnknownString += strPart;
				strUnknownString += _T(' ');
			}
		}
	}

	// set the unknown option property, and
	// set the unknown string property
	if (nIDUnknownOption != (UINT)-1)
		m_pSlob->SetStrProp(nIDUnknownOption, strUnknownOption);

	if (nIDUnknownString != (UINT)-1)
		m_pSlob->SetStrProp(nIDUnknownString, strUnknownString);

   	// reset the prop. bag usage
 	(void) ((CProjItem *)m_pSlob)->UsePropertyBag(idOldBag);

	return fReturn;	// success or failure?
}

BOOL COptionTable::CollectOptionPart
(
	CString &	strOut,			// the collected option part
	BOOL		fAlphaNumOnly,	// allow only alpha-numeric
	BOOL		fAllowPunct,	// allow punctuation
	BOOL		fAllowKanji,	// allow Kanji characters
	BOOL		fKeepQuotes,	// keep the quotes?
	CString		strDelim		// any delimiters (could be empty)
)
{
	BOOL			fQuoted, fEscaped;
	INT_PTR			ichFirst, cchAdvSrcOrig;
	TCHAR			* pchSrc, ch;

	// figure first char.
	ichFirst = ichSrc + cchAdvSrc; cchAdvSrcOrig = cchAdvSrc;

CollectOptionPart:

	fEscaped = FALSE;
	pchSrc = (TCHAR *)(const TCHAR *)strSrc + ichSrc + cchAdvSrc; 

	// move over the first quote if we have one
	fQuoted = (*pchSrc == _T('"'));
	if (fQuoted)
	{	
		// we're a quoted string
		pchSrc++; cchAdvSrc++;
		if (!fKeepQuotes)
		{
			ichFirst++; cchAdvSrcOrig++;
		}
	}

	// check for '/OPTION" /' where we are parsing
	// a partial typed string, we'll assume that whitespace
	// followed by an option delimiter is not a valid arg.
	INT_PTR ichLeadWhiteSpace = cchAdvSrc;	// assume whitespace for quoted

	// the end of the option part is
	// o whitespace (if not quoted) or null terminator
	// o a double-quote (if not escaped)
	// o a non alpha-numeric if fAlphaNumOnly = TRUE
	// o a punctuation character ~= '=' and ~= ',' not a flag prefix
	// o we hit a delimiter
	while ((ch = *pchSrc) != _T('\0'))
	{
		int cch = (int)_tclen(pchSrc); pchSrc += cch;

		// allow only alpha-numeric
		if (fAlphaNumOnly)
		{
			if (!isalnum(ch) && ch != _T('-') && ch != _T('+'))
				break;
		}
		// allow all alpha
		else if (isalpha(ch))
		{
			ichLeadWhiteSpace = 0;
		}
		else
		{
	#ifdef _MBCS
			if (!fAllowKanji && (_ismbbkana(ch) || _ismbblead(ch)))
				break;
	#endif

			// currently parsing a quoted string?
			if (fQuoted)
			{
				// yes
				// FUTURE: do we allow escaped 'quotes' in tool options?
				if (/*!fEscaped &&*/ ch == _T('"'))
					break;

				if (ichLeadWhiteSpace)
				{
					if (!_istspace(ch))
					{
						if (strPrefixes.Find(ch) != -1)
						{
							// back-up to the quote before the whitespace
							cchAdvSrc = ichLeadWhiteSpace;
							break;	// break here, we found '/OPTION"   /'
						}

				 		ichLeadWhiteSpace = 0;
					}
				}
			}
			else
			{
				//no
				// allow punctuation inside a quoted string!
				if (!fAllowPunct && ispunct(ch))
				{
					// FUTURE (matthewt): when we process '=' for defines correctly
					// then maybe we want to split the args, for now treat
					// as one arg.
					if (ch == _T('=') || ch == _T(','))
						break;

					if (strPrefixes.Find(ch) != -1)
						break;
				}

				if (_istspace(ch) || strDelim.Find(ch) != -1)
					break;	// break here, we found option delimiter (incls. whitespace)
			}
		}

		cchAdvSrc += cch;

		// do we have '\\' at the end of our string?
		if (fEscaped && fQuoted && ch == _T('\\') && *pchSrc == _T('"'))
			// skip back over the last one (ie. ignore it)
			cchAdvSrcOrig++;

		fEscaped = !fEscaped && ch == _T('\\');
	}

	INT_PTR cchOut = cchAdvSrc - cchAdvSrcOrig;

 	// eat the final quote if we have one
	if (fQuoted && ch == _T('"'))
	{
		// we're a quoted string
		cchAdvSrc++;
		if (fKeepQuotes)	cchOut++;
	}

	// do it for the 2nd half of an arg., eg. /D foo="bar"
	if (!fQuoted && ch == _T('='))
	{
		// we're an equals sign
		cchAdvSrc++;

		// keep the quotes (if they exist) for second half of assignment
		// (ignore return of FALSE - we don't care if empty)
		fKeepQuotes = TRUE;
		goto CollectOptionPart;	// next half
	}

	// have we actually found any characters?
	if (!cchOut)
	{
		// do we need to have a quoted empty string
		// in order to succeed?
		if ((m_optbeh & OBNeedEmptyString) && !fQuoted)
			return FALSE;

		strOut = "";
	}
	else
	{
		VSASSERT(ichFirst >= 0 && cchOut > 0, "CollectOptionPart parse error");
		strOut += strSrc.Mid((int)ichFirst, (int)cchOut);
	}

	return TRUE;	// got a match
}

void COptionTable::ExtractOptionArg(char * & pchExpr, size_t & cchExpr)
{
	char	chExpr = *pchExpr;
	TCHAR *	pch = NULL;
	size_t	cchArg = 0;

	// argument value pattern?
	//
	patArgValue = none;

	if (chExpr == _T('T') || chExpr == _T('F') ||
		chExpr == _T('t') || chExpr == _T('f'))
	{
		// booln arg
		pch = pchExpr++; cchArg++; // move over booln arg.
		patArgValue = booln;
	}
	else
	if (chExpr == _T('{') ||
		chExpr == _T('<'))
	{
		// what is our terminator char?
		TCHAR chTerm = chExpr == _T('{') ? _T('}') : _T('>');

		// choice or absolute arg
		pch = ++pchExpr; cchExpr--; // skip over '{' or '<'
		patArgValue = chExpr == _T('{') ? choice : absolute;

		char	chExpr;
		char	* pch2 = NULL;
									  
		// extract our '<...>' or '{...}' chunk
		chExpr = *pchExpr; pch2 = pchExpr;
		while (chExpr != chTerm)
		{
			chExpr = *pchExpr;	// we want' to stop just after the '}' or '>'!
			pchExpr = _tcsinc(pchExpr);
		}
		cchArg = pchExpr - pch2;
	}

	cchExpr -= cchArg;

	// ensure that the arg buffer is big enough
	// for debug purposes only, the option table arg
	// 'chunks' are static
	VSASSERT(cchArg < cchArgValue, "arg buffer not big enough");
	_tcsncpy(pchArgValue, pch, cchArg); 
	pchArgValue[cchArg] = _T('\0');	// terminate
}

void COptionTable::ExtractPart(char * & pchExpr, size_t & cchExpr, char * & pchPart, size_t & cchPart)
{
	UINT	cBrackets = 1;		// used to count brackets
	char	chExpr;

	cchPart = 0;	// init. the count of chars in the part
	pchPart = ++pchExpr; cchExpr--;	// skip the '[', and remember the position

	// search for the corresponding close bracket
	while (TRUE)
	{
		chExpr = *pchExpr;
		VSASSERT(chExpr != _T('\0'), "Must be able to find the part we want!");	// must be able to find one!

		if (chExpr == _T('['))	cBrackets++;	// bump up bracket count
		else
		if (chExpr == _T(']'))	cBrackets--;	// bump down bracket count

		// have we found the match?
		if (cBrackets == 0)
			break;	// yes, so break-out of here

		size_t	cch = _tclen(pchExpr);
		cchPart += cch; pchExpr += cch; cchExpr -= cch;
	}

	pchExpr++; cchExpr--;	// terminate on the ']'
}

BOOL COptionTable::MatchChoice(size_t &nChoiceMatch, char * pchChoice, UINT * pidArg)
{
	INT_PTR cchChoice;
	INT_PTR cchAdvSrcKeep;
	char * pchStart;

	// signal any kind of match
	nChoiceMatch = 0;	// no match

	for (size_t nChoice = 1 ; /* we'll break out! */ ; nChoice++ /* next choice */)
	{
		pchStart = pchChoice;

		// find our choice expression
		while (*pchChoice != _T('|') && *pchChoice != _T('}'))
			pchChoice = _tcsinc(pchChoice);

		// do we have a choice to match?
		cchChoice = (int)(pchChoice - pchStart);
		if (cchChoice)
		{
			cchAdvSrcKeep = cchAdvSrc;
			if (MatchExpression(pchStart, cchChoice, pidArg))
			{
				nChoiceMatch = nChoice;		// matched, or last resort match (cchAdvSrc == cchAdvSrcKeep)

				if (cchAdvSrc != cchAdvSrcKeep)	return TRUE;	// matched option

				// this choice matched no chars, ie. was purely arg matching
				// we will use this nChoice if no other option matches
			}
			cchAdvSrc = cchAdvSrcKeep; 	// no match
		}
		// this is an empty option
		// we will use this nChoice if no other option matches
		else
			nChoiceMatch = nChoice;	// last resort match	

		// skip over the '|'
		if (*pchChoice++ == _T('}'))
			return (nChoiceMatch != 0);	// no match possible?
										// ran out of choices, maybe last resort choice?
	}

	return TRUE;
}

BOOL COptionTable::MatchExpression(const char * pchExprConst, size_t cchExpr, UINT * pidArg)
{
	size_t cch;
	UINT cArgs = 0;
	char * pchExpr = (char *)pchExprConst;
	unsigned char chExpr; // expression character

	CString	strOut; // output string
    
    char * pchSrc = (char *)(const char *)strSrc + ichSrc + cchAdvSrc;	// our source                                        
                               
	// have we finished this pattern? we will break out on failure (return)
	while (cchExpr != 0)
	{
  		chExpr = *pchExpr;

 		// do we have an argument specified?
		if (chExpr == _T('%'))
		{
			// increment the number of args. found
			// first?
			if (++cArgs == 1)
				cchAdvSrcToArg = cchAdvSrc;	// remember the adv. src. to arg.

			cch = _tclen(pchExpr);
			pchExpr += cch; cchExpr -= cch; // move over arg. spec.
			chExpr = *pchExpr;

			// argument value?
			ExtractOptionArg(pchExpr, cchExpr);

			// argument index
			//
			UINT	iArg;		// our arg. index
			UINT	idArgProp;	// our arg. property
								  
			chExpr = *pchExpr;
			VSASSERT(isdigit(chExpr), "Must be an arg. index!");	// must be an arg. index!
			iArg = chExpr - '1';
			idArgProp = pidArg[iArg];	// retrieve this arg's property #
			cch = _tclen(pchExpr);
 			pchExpr += cch; cchExpr -= cch;	// move over arg. index

			BOOL fFake = m_popthdlr->IsFakeProp(idArgProp);
			BOOL fAlwaysShowDefault = m_popthdlr->AlwaysShowDefault(idArgProp);

			// what type of property is this?
			// we want to match in the source the property value
			switch (m_popthdlr->GetDefOptionType(idArgProp) /* property type */)
			{
				case ::integer:
				{
					int nVal;

					// do we want to match an choice?
					if (patArgValue == choice)
					{
						size_t uVal;

						// match a choice in the string
						if (!MatchChoice(uVal, pchArgValue, pidArg))
						{
							// for 0th choices we might have used subtract
							if (!(m_optbeh & OBAnti) || _tcsncmp(pchSrc, _TEXT("<none>"), 6))
								return FALSE;	// no match, failed to find choice

							cchAdvSrc += 6;	// skip over

							// consume this source
							pchSrc = (char *)(const char *)strSrc + ichSrc + cchAdvSrc;	// our 'new' source                                        

							uVal = 0;	// 0th
						}

						nVal = (int)uVal;
					}
					// do we want to set an absolute?
					else if (patArgValue == absolute)
					{
						// convert this absolute into a numeric to suit
						// the subtype of this integer 
					   	BOOL bOK = m_popthdlr->ConvertFromStr(idArgProp, pchArgValue, nVal);
						VSASSERT(bOK, "Failed to convert prop from option string!");
					}
					else
					{
						// just match an integer in the string
						if (!CollectOptionPart(strOut, TRUE) || strOut.IsEmpty())
							return FALSE;	// no match, failed to collect

						// got a match, consume this source
						pchSrc = (char *)(const char *)strSrc + ichSrc + cchAdvSrc;	// our 'new' source                                        

						// do the conversion from a string into an integer
						// based on the sub-type
					   	if (!m_popthdlr->ConvertFromStr(idArgProp, strOut, nVal))
						{
							// couldn't convert due to bad numeric format
							// use existing value if one exists!
							CProp * pProp = m_pSlob->GetPropBag()->FindProp(idArgProp);
							if (pProp == (CProp *)NULL)
							{
								AddToPropList(iArg, idArgProp, NULL);
								break;	// doesn't exist, ignore ie. no pass, no fail
							}
							nVal = ((CIntProp *)pProp)->m_nVal;
						}
					}

					// don't set defaults, or fake props
					BOOL fSet;
					if (fFake)
						fSet = fAlwaysShowDefault;
					else
						fSet = fAlwaysShowDefault || (m_optbeh & OBSetDefault);

					if (!fSet && m_popthdlr->IsDefaultIntProp(idArgProp, nVal))
						break;

					CIntProp *	ppropInt = new CIntProp(nVal);
					AddToPropList(iArg, idArgProp, ppropInt);
					break;
				}
				
				case ::booln:
				{
					BOOL	bVal;
					BOOL	fSoftBool = FALSE;

					// do we want to match an choice?
					if (patArgValue == choice)
					{	
						size_t	nVal;

						// match a choice in the string
						if (!MatchChoice(nVal, pchArgValue, pidArg))
							return FALSE;	// no match, failed to find choice

						// got a match, consume this source
						pchSrc = (char *)(const char *)strSrc + ichSrc + cchAdvSrc;	// our 'new' source 
						                                       
						// match an choice in the string
						bVal = (nVal == 1) ? FALSE : TRUE;	// convert to a booln
					}
					else
					{
						VSASSERT(patArgValue == booln, "Illegal arg value");
						// just set the property based in the booln arg value
						bVal = *pchArgValue == _T('T');
						if( !bVal )
						{
							fSoftBool = (*pchArgValue == _T('t'));
							bVal = fSoftBool;
						}
					}

					// don't set defaults, or fake props
					BOOL fSet;
					if (fFake)
						fSet = fAlwaysShowDefault;
					else
						fSet = fAlwaysShowDefault || (m_optbeh & OBSetDefault);

					if (!fSet && m_popthdlr->IsDefaultBoolProp(idArgProp, bVal))
						break;

					// do we need to invert the integer value?
					// FUTURE: don't do this for softbool values (remove when per-prop remove in .PRJ)
					if ((m_optbeh & OBAnti) && !fSoftBool) 
						bVal = !bVal;	// invert

					CBoolProp *	ppropBool = new CBoolProp(bVal);
					AddToPropList(iArg, idArgProp, ppropBool);
					break;
				}

				case ::string:
				{
					if (!CollectOptionPart(strOut))
						return FALSE;	// no match, failed to collect

					// got a match, consume this source
					pchSrc = (char *)(const char *)strSrc + ichSrc + cchAdvSrc;	// our 'new' source                                        

					// don't set defaults, or fake props
					BOOL fSet;
					if (fFake)
						fSet = fAlwaysShowDefault;
					else
						fSet = fAlwaysShowDefault || (m_optbeh & OBSetDefault);

					if (!fSet && m_popthdlr->IsDefaultStringProp(idArgProp, strOut))
						break;

					CStringProp * ppropString = new CStringProp(strOut);
					AddToPropList(iArg, idArgProp, ppropString);
					break;
				}
					 
				case ::longint:	// not yet needed!
				case ::number:
					break;

				default:
					VSASSERT(FALSE, "Unhandled expression char");
					break;
			}
		}
		// do we have an open bracket (optional part)?
		else if (chExpr == _T('['))
		{
			char * pchPart;
			size_t cchPart;

			// extract the optional part
			ExtractPart(pchExpr, cchExpr, pchPart, cchPart);

			// don't bother matching empty optional parts
			if (cchPart != 0)
			{
				NoChangedPropValList();	// reset, so we recognise args to back-out
				if (MatchExpression(pchPart, cchPart, pidArg))
					// got a match, consume this source
				    pchSrc = (char *)(const char *)strSrc + ichSrc + cchAdvSrc;	// our 'new' source                                        
				else
					// back-out args set in no match optional part
					ClearPropValList(TRUE);
			}

			// continue, this is matching an optional part
		}
		// is this a 'just' whitespace candidate?
		// only do this for 'single whitespace expressions'
		else if (cchExpr == 1 && chExpr == _T(' '))
		{
			size_t cchAdvSrcOrig = cchAdvSrc;

			SkipWhiteSpace();	// skip any  whitespace
			pchExpr++; cchExpr--;	// skip over this space in the expression
			pchSrc += (cchAdvSrc - cchAdvSrcOrig);	// advance our source
		}
		// else should just try to match this
		else
		{
			if (_tccmp(pchExpr, pchSrc) != 0)	// not a char. match
			{
				unsigned char chSrc = *pchSrc;

				// did this not match 'cos of only case insensitivity
				if (m_fCaseSensitive || (toupper(chExpr) != toupper(chSrc)))
					return FALSE;
			}

			cch = _tclen(pchExpr);
			pchExpr += cch; cchExpr -= cch;

			cch = _tclen(pchSrc);
			pchSrc += cch; cchAdvSrc += cch;
		}

		// continue trying to match source and expression
	}
	return TRUE;	// successful match
}

BOOL COptionTable::MatchOption(COptStr * poptstr)
{
	BOOL	fMatched;

	// what type of option it this? single or multiple?
 	m_fMultipleGen = (poptstr->type == multiple);

	// can we match this expression?
	fMatched = MatchExpression(poptstr->szOption, _tcslen(poptstr->szOption), poptstr->rgidArg);

	// must terminate in a whitespace or EOS
	unsigned char	chTerm = NewGetAt(strSrc, ichSrc + cchAdvSrc);

	fMatched = fMatched && (chTerm == _T('\0') || _istspace(chTerm));
	
	if (fMatched)	
		// yes, so set up these properties in our bag
		SetPropValListInPropBag(poptstr->rgidArg);

	// clear our property-value list (may be empty)
	ClearPropValList();

	return fMatched;
}

void COptionTable::ClearPropValList(BOOL fOnlyThoseChanged)
{
	for (WORD cArgs = 0 ; cArgs < MAX_OPT_ARGS ; cArgs++)
		if (m_rgProps[cArgs].idArg != (UINT)-1 && 
		    (!fOnlyThoseChanged || m_rgProps[cArgs].fChanged)
		   )
		{
			if (m_rgProps[cArgs].pProp)
				CProp::DeleteCProp(m_rgProps[cArgs].pProp);
			m_rgProps[cArgs].idArg = (UINT)-1;
		}
}

BOOL RemoveMultiOptionFromString(CString *strOptList, CString strOpt)
{
	CString strSub, strNew = _T("");
	CString str = *strOptList;
	int	 pos = -1;
	BOOL bFound = FALSE;

	while(!str.IsEmpty())
	{
		pos = str.Find(",");
		if(pos)
		{
			strSub = str.Left(pos);
			str = str.Right(str.GetLength() - pos - 1);
		}
		else
		{
			strSub = str;
			str = _T("");
		}
		if(strSub != strOpt)
		{
			if(!strNew.IsEmpty())
				strNew += _T(',');
			strNew += strSub;
		}
		else
			bFound = TRUE;
	}

	if(bFound)
		*strOptList = strNew;
	return bFound;
}

void COptionTable::SetPropValListInPropBag(UINT * poptarg)
{
	CPropBag * 	ppropbag = m_pSlob->GetPropBag();
	CProp * ppropExisting; 

	for (WORD cArgs = 0; cArgs < MAX_OPT_ARGS ; cArgs++, poptarg++)
	{
		UINT	idArg = m_rgProps[cArgs].idArg;
		BOOL	fExists = (ppropExisting = ppropbag->FindProp(*poptarg)) != (CProp *)NULL;

		// has this arg. been set?
		if (idArg == (UINT)-1)
		{
			UINT	idOptArg = *poptarg;

			// should we clear the arg? we need to do this for args that exist for the option
			// but which have not been set
			if (idOptArg != (UINT)-1)
			{
				// yes, clear it, but for multiple options just append an empty prop. value
				if (m_fMultipleGen)
				{
					// get the prop value
					CString	strVal;

					// set to retrieve subtract if anti-option
					BOOL fPropAntiOld = ((CProjItem *)m_pSlob)->EnableAntiProps(m_optbeh & OBAnti);

					// get the existing comma sep. list of args if *we* have one
					// ie. we only want to do this for *our* propbag (not the parents)
					if (ppropbag->FindProp(ListStr_AddPart(idOptArg)) ||
						ppropbag->FindProp(ListStr_SubPart(idOptArg)))
					{
						(void) m_pSlob->GetStrProp(idOptArg, strVal);

						// append a sep
						strVal += _T(',');
					}

					// append a comma sep
					m_pSlob->SetStrProp(idOptArg, strVal); 

					(void) ((CProjItem *)m_pSlob)->EnableAntiProps(fPropAntiOld);
				}
				// remove this prop if we have one
				else if (fExists)
				{
					// delete this from the current property bag (it exists!)
					ppropbag->RemovePropAndInform(idOptArg, m_pSlob);
				}
			}

			continue;
		}

		CProp *	pprop = m_rgProps[cArgs].pProp;

		// if we have NULL here then ignore
		// we want to use the previous existing value (if one exists)
		if (pprop == (CProp *)NULL)
			continue;

		// dirty this prop we are about to set

		// for string list props we need to delete the components
		// *not* the pseudo prop
		if (m_fMultipleGen && m_popthdlr->IsListStrProp(idArg))
		{
			m_popthdlr->DirtyArg(ListStr_AddPart(idArg) - m_nPropMin);
			m_popthdlr->DirtyArg(ListStr_SubPart(idArg) - m_nPropMin);
		}
		else
		{
			m_popthdlr->DirtyArg(idArg - m_nPropMin);
		}

		// what is the property type?
		// don't set property values if current property has this value!
		switch (pprop->m_nType /* property type */)
		{	
			case ::integer:
				VSASSERT(!m_fMultipleGen, "Integer can only handle single");	// only for single options
				if (!fExists ||
					(((CIntProp *)pprop)->m_nVal != ((CIntProp *)ppropExisting)->m_nVal)
				   )
				{
					m_pSlob->SetIntProp(idArg, ((CIntProp *)pprop)->m_nVal); 
				}
				break;

			case ::booln:
				VSASSERT(!m_fMultipleGen, "Booln can only handle single");	// only for single options
				if (!fExists ||
					(((CBoolProp *)pprop)->m_bVal != ((CBoolProp *)ppropExisting)->m_bVal)
				   )
				{
					m_pSlob->SetIntProp(idArg, ((CBoolProp *)pprop)->m_bVal); 
				}
				break;

			case ::string:
			{
				// if we are a multiple option, append using comma seps.
				if (m_fMultipleGen)
				{	
					// the option sep. list
					CString	strVal;

					// set to retrieve subtract if anti-option
//					BOOL fPropAntiOld = ((CProjItem *)m_pSlob)->EnableAntiProps(m_optbeh & OBAnti);

					// get the existing comma sep. list of args if *we* have one
					// and append to that
					if (m_pSlob->GetStrProp(idArg, strVal) == valid)
					{
						if (!strVal.IsEmpty())
							strVal += _T(',');
						if (!(m_optbeh & OBAnti))
							strVal += ((CStringProp *)pprop)->m_strVal;
						else
						{	//remove from m_strVal
						
							if(RemoveMultiOptionFromString(&strVal, ((CStringProp *)pprop)->m_strVal))
							{	
							// 1) Find prop in	m_strVal
							// if (found)
							//		remove from m_strVal
				
							}
							else
							{
								BOOL fPropAntiOld = ((CProjItem *)m_pSlob)->EnableAntiProps(m_optbeh & OBAnti);
							//	set antiprop
							//  do oldway
							//  set back anti
								if (m_pSlob->GetStrProp(idArg, strVal) == valid)
								{
									if (!strVal.IsEmpty())
										strVal += _T(',');
									strVal += ((CStringProp *)pprop)->m_strVal;
									m_pSlob->SetStrProp(idArg, strVal); 
									(void) ((CProjItem *)m_pSlob)->EnableAntiProps(fPropAntiOld);
									break;
								}
							}
						}

						m_pSlob->SetStrProp(idArg, strVal); 
					}

//					(void) ((CProjItem *)m_pSlob)->EnableAntiProps(fPropAntiOld);
				}
				else
				{
					// anti-option only for multiple string lists
					if (m_optbeh & OBAnti)
						break;

					if (!fExists ||
						(((CStringProp *)pprop)->m_strVal != ((CStringProp *)ppropExisting)->m_strVal)
					   )
					{
						m_pSlob->SetStrProp(idArg, ((CStringProp *)pprop)->m_strVal); 
					}
				}

				break;
			}

			case ::longint:	// not yet needed!
			case ::number:
				break;

			default:
				VSASSERT(FALSE, "Unhandled option type");
				break;
		} 
	}
}
