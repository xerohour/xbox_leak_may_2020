
//
// This file contains the code that wraps the interface to the EE
//

#include "stdafx.h"
#pragma  hdrstop

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

// CTM - abstract base class which provides the interface to a TM.

IMPLEMENT_DYNAMIC(CTM,CObject);


CTM::~CTM( ) {}

// CRootTM

IMPLEMENT_DYNAMIC(CRootTM, CTM);

CRootTM::CRootTM (LPCSTR lpszName, BOOL bNoFuncEval /* = FALSE */)
{
	// The user typed in a name.
	// create the basic TM corresponding to it.
	// Note that the check to see if the name points to a valid
	// data item happens before we call the constructor.

	ASSERT(lpszName != NULL);

	BOOL fCase = TRUE;
	USHORT iEnd = 0;

	// initialize fields with neutral values in case of failure...

	m_htm = htmNil;
	m_fExpandedTM = FALSE;
	m_bNameTM	= TRUE ;

	CommonInit();
	InvalidateCaches();

	char *szExpr = (char *)lpszName;	// cast away const...

	// save away the expression, we'll need it in case
	// we can't parse & bind right now...

	m_cstrNameCache = szExpr;
	m_fNameCacheValid = TRUE;

	m_bNoFuncEval = bNoFuncEval;

	// parse expression, if can't be parsed we have no useable TM
	if ( (pEXF==NULL) || (EEParse(szExpr, ::radix, fCase, &m_htm, &iEnd) != EENOERROR) )
	{
		m_htm = htmNil;
		return;
	}

	// now try to bind and evaluate, if these fail the item is likely out
	// of scope, save the TM as it may come into scope later...

	if (RebindAndEvaluate() != EENOERROR)
	{
		return;
	}

	// if we get here, all is well, m_htm has been assigned with a bound
	// and evaluated TM and we're ready to go...
}

// Constructor is called with a handle to a codeview object,
// and the second argument indicates the type of the handle.

CRootTM::CRootTM (HIND h , HANDLE_TYPE hType  )
{
	m_fNameCacheValid = FALSE;
	
	CommonInit( );
	InvalidateCaches();

	m_bNameTM = FALSE ; // TM's not created by passing in a string value.

	switch ( hType )
	{
		case IS_HTM:
		{
			// this constructor is given a TM that was already parsed, bound,
			// and evaluated successfully. There is sadly no way to assert this
			// because the EE has no such API's...
			
            HTM htm = HTM(h) ;
			m_htm = htm;
			m_fExpandedTM = TRUE;
			break;
		}
		case IS_HSYM:
		{
			HSYM hSym = h ;
			// this constructor is given an HSYM and needs to convert that
			// to a TM.
			CXF curCxf ( ::CxfWatch( ) );
			PCXF pCxf = &curCxf ;

			USHORT iEnd = 0;

			m_htm = htmNil;
			m_fExpandedTM = FALSE;

			if ( (m_eestatus = EEGetTMFromHSYM( hSym, SHpCXTFrompCXF(pCxf), &m_htm, &iEnd, TRUE, FALSE )) != EENOERROR )
			{
				m_htm = htmNil ;
				return ;
			}
	
			if ( (m_eestatus = EEvaluateTM( &m_htm, SHpFrameFrompCXF(pCxf) , EEVERTICAL)) != EENOERROR )
			{
				m_htmSaved = m_htm ;
				m_htm = htmNil ;
				return ;
			}
			break;
		}
		default:
			ASSERT(FALSE);
	}
}


		
CRootTM::~CRootTM()
{

	// at most one of these may be valid, otherwise we don't
	// know which item to free...

	ASSERT(!m_htmSaved || !m_htm);

	// release the htm we have saved (if any)

	if (m_htm)
	{
		EEFreeTM(&m_htm);
		m_htm = htmNil;
	}

	if (m_htmSaved)
	{
		EEFreeTM(&m_htmSaved);
		m_htmSaved = htmNil;
	}
}

#if _DEBUG
void CRootTM::Dump(CDumpContext& dc) const
{
	CTM::Dump(dc);

	dc << "Name TM=" << (m_bNameTM ? "TRUE" : "FALSE") ;
	dc << "Expanded TM=" << (m_fExpandedTM ? "TRUE" : "FALSE") ;
	dc <<  "Cached Name was " << m_cstrNameCache ;
}
#endif

BOOL CRootTM::IsExpandable ( ) const
{
	BOOL bRet = FALSE ;

	// check for nil TM
	if (!m_htm) return FALSE;

	// must copy the member to preserve const-ness of this function
	HTM htm = m_htm;
	
	switch (EEIsExpandable(&htm))
	{
		case EENOTEXP:			// not expandable
		case EETYPE:			// expandable as type (not handled yet)
		case EETYPENOTEXP:		// not expandable type
		case EETYPEPTR:			// pointer type? -- not handled yet
			bRet = FALSE ;
			break ;

		case EEAGGREGATE:		// a struct, array, union, class, etc.
		case EEPOINTER:			// a pointer that needs to be dereferenced
			bRet =  TRUE;
			break;
	}

	if ( bRet )
	{
		LONG lcChild ;
		SHFLAG shf ;

		// We need to additionally ensure that the TM has a non-zero
		// number of children because the previous check ( EEIsExpandable)
		// does not take care of zero-sized arrays and structures with no members.
		if  (EEcChildrenTM (&htm, &lcChild , &shf) == EENOERROR)
			bRet = (lcChild == 0) ? FALSE : TRUE ;
		else
			bRet = FALSE ;
	}
	return bRet ;
}

BOOL	CRootTM::IsNameEditable (  ) const
{	
	return m_bNameTM ;
}

BOOL  CRootTM::IsSynthChildNode( ) const
{
	// Only valid/expanded TM's could be synthesized children.
	if ( m_htm == htmNil || !m_fExpandedTM )
		return FALSE;

	RTMI rtmi = { 0 }  ;
	HTI hTI ;
	HTM htm = m_htm;
	
	// Request information from the EE.
	rtmi.fSynthChild = TRUE ;	

	if ( ( EEInfoFromTM(&htm, &rtmi, &hTI) != EENOERROR ) )
	{
		return FALSE ;
	}
	ASSERT( hTI != ( HTI ) NULL );

	// Get the TM Info structure.
	PTI pTI = ( PTI ) BMLock(hTI);
	ASSERT(pTI != NULL);

	BOOL retVal = FALSE;
	if (pTI->fResponse.fSynthChild)
	{
		retVal = pTI->fSynthChild;
	}	

	// Free up the resources.
	BMUnlock(hTI);
	EEFreeTI(&hTI);
	
	// FUTURE: Maybe we want to cache this value as well
	// to avoid querying the EE too often.
	return retVal ;
}
	
BOOL  CRootTM::HasSynthChild( )	const
{
	if ( m_htm == htmNil)
		return FALSE;

	// Cast away constness.
	HTM htm = m_htm;
	USHORT iEnd;
	BOOL retval = FALSE;
	BOOL bOldState = EEEnableAutoClassCast(TRUE);

	if (EEIsExpandable(&htm) == EEPOINTER)
	{
		HTM htmDeref;
		if ( EEDereferenceTM(&htm, &htmDeref, &iEnd, TRUE) == EENOERROR)
		{
			if ( EEIsExpandable(&htmDeref) == EEAGGREGATE )
			{
				long cChildren;
				if (EEcSynthChildTM(&htmDeref, &cChildren) == EENOERROR)
				{
					ASSERT(cChildren <= 1);

					if (cChildren != 0 )
						retval = TRUE;
				}
			}
			EEFreeTM(&htmDeref);
		}
	}	
	
	EEEnableAutoClassCast(bOldState);
	return retval;
}

BOOL CRootTM::IsValueValid() const
{
	if (!m_htm)
		return FALSE;
	else
		return TRUE;
}		

BOOL CRootTM::CouldParseName( ) const
{
	ASSERT(m_bNameTM);

	if (m_bNameTM)
	{
		if (m_htm == htmNil && m_htmSaved == htmNil)
			return FALSE;
	}
	return TRUE;
}

BOOL CRootTM::IsClass() const
{
	if (!m_htm && !m_htmSaved)  return FALSE;
	
	HTM htm = (m_htm != NULL) ? m_htm : m_htmSaved ;

	ETI eti;
	if (EEGetExtendedTypeInfo(&htm, &eti) == EENOERROR)
	{
		if ( eti == ETICLASS)
			return TRUE;
	}
	return FALSE;
}
	

BOOL CRootTM::IsDataTM() const
{
	// Does this TM correspond to a data value.
	// Things like 'CWnd' which is a type and function names, labels are excluded.
	
	if (!m_htm && !m_htmSaved) return FALSE;	

	HTM htm = (m_htm != NULL) ? m_htm : m_htmSaved ;

	EEPDTYP eetyp = EEIsExpandable(&htm);

	switch (eetyp)
	{
		case EETYPE:
		case EETYPENOTEXP:
		case EETYPEPTR:
			return FALSE;
	}

	// Check for function names
	ETI eti;
	if (EEGetExtendedTypeInfo(&htm, &eti) == EENOERROR)
	{
		if ( eti == ETIFUNCTION )
			return FALSE;
	}

	RTMI rtmi = { 0 };
	HTI hTI;

	// Request information from the EE.
	rtmi.fLabel = TRUE;

	if ( ( EEInfoFromTM(&htm, &rtmi, &hTI) != EENOERROR ) )
	{
		return FALSE ;
	}
	ASSERT( hTI != ( HTI ) NULL );

	// Get the TM Info structure.
	PTI pTI = ( PTI ) BMLock(hTI);
	ASSERT(pTI != NULL);

	BOOL retVal = TRUE;

	if (pTI->fResponse.fLabel)
	{
		retVal = ! pTI->fLabel;	 // Should not be a label.
	}

	// Free up the resources.
	BMUnlock(hTI);
	EEFreeTI(&hTI);

	return retVal;
}
				
		
BOOL CRootTM::IsTypeIdentical(CTM *pTM)
{
	return (GetType( ) == pTM->GetType());
}

BOOL CRootTM::HadSideEffects( ) const
{
	return m_bHadSideEffects;
}

CTM * CRootTM::GetTMChild(int nChild)
{
	// The TM must be an expandable one...
	ASSERT(IsExpandable());

	HTM htm = m_htm;

	USHORT iEnd;
	BOOL fFreeTM = FALSE;
	CXF curCxf( ::CxfWatch( ) );
	PCXF pCxf = &curCxf;
	CRootTM * pChildTM = NULL;
	BOOL bOldState = EEEnableAutoClassCast(TRUE);

	// If we have a pointer to an aggregate we want to dereference
	// before getting the children.

	switch(EEIsExpandable(&m_htm))
	{
		case EEPOINTER:
			if (EEDereferenceTM(&m_htm, &htm, &iEnd, TRUE) != EENOERROR)
				break;

			if ( EEIsExpandable(&htm) != EEAGGREGATE )
			{

				// Plain pointer. If we were asked for the first
				// child go ahead and pass it back now.
				if ( nChild == 0 )
				{
					// This eval could fail if we have NULL pointer etc. We
					// will intentionally ignore the failure.
					EEvaluateTM(&htm, SHpFrameFrompCXF( pCxf ), EEVERTICAL);

					pChildTM = new CRootTM(HIND(htm), IS_HTM );
					break;
				}
				else
				{
					fFreeTM = TRUE;
					break;
				}

			}

			fFreeTM = TRUE;
			// Fall through.
		
		case EEAGGREGATE:
		{	
			HTM htmNew;
			
			if (EEGetChildTM(&htm, nChild, &htmNew, &iEnd, 0, TRUE) == EENOERROR)
			{
				// It is possible and okay if this evaluate fails, because the memory
				// pointed to is not valid etc.
				EEvaluateTM(&htmNew, SHpFrameFrompCXF( pCxf ), EEVERTICAL);
						
				pChildTM = new CRootTM(HIND(htmNew), IS_HTM );
			}
			break;
		}
	}

	if (fFreeTM) EEFreeTM(&htm);

	EEEnableAutoClassCast(bOldState);

	return pChildTM;
}

CTM * CRootTM::GetSynthChildTM( )
{
	if ( !HasSynthChild( ))
		return NULL;
		
	return GetTMChild(0);
}

DWORD  CRootTM::GetChildCount( )
{
	LONG numChild = 0;
	SHFLAG fVarChildren;

	if (IsExpandable()) 
	{
		if (EEcChildrenTM(&m_htm, &numChild, &fVarChildren) != EENOERROR)
			numChild = 0;
	}
	
	return numChild;
}


BOOL  CRootTM::GetTMChildren(CTMArray& childTMArray)
{
	// the TM must be an expandable one...
	ASSERT (IsExpandable());
	
	ASSERT( childTMArray.GetSize() == 0);

	// we'll either use a computed htm from a dereference pointer
	// or else the member htm if that turns out to be an aggregate
	// setup for the initial operation in either case by copying
	// m_htm...
	HTM htm = m_htm;

	
	SHFLAG fVarChildren;
	USHORT iEnd;
	BOOL fFreeHTM = FALSE;
	CXF curCxf ( ::CxfWatch( ) );
	PCXF pCxf = &curCxf ;
	BOOL bOldState = EEEnableAutoClassCast(TRUE);

	switch (EEIsExpandable(&m_htm))
	{
	// if we're a pointer, we de-reference first then expand...
	case EEPOINTER:
		{
		if (EEDereferenceTM(&m_htm, &htm, &iEnd, TRUE) != EENOERROR)
			break;

		// if this is not an aggregate, then return just the one TM
		// in the array...
		if (EEIsExpandable(&htm) != EEAGGREGATE)
			{
			// evaluate the TM (so far it is only bound)
			// even if the TM cannot be succesfully evaluated,
			// add it to the TM array as we might be able to evaluate
			// it later ( after the debuggee state changes).
			EEvaluateTM(&htm, SHpFrameFrompCXF( pCxf ), EEVERTICAL);
			
			// now add the TM to the array of child TM's

            CRootTM* pChildTM = new CRootTM(HIND(htm), IS_HTM );
			childTMArray.Add(pChildTM);
			break;
			}
		}

	// fall through

	// at this point we know that we're not going to want
	// the htm we've created above, so we're going to throw it away
	// when we're done expanding

	fFreeHTM = TRUE;

	case EEAGGREGATE:
		{
		LONG numChildren;

		// get the number of children and iterate through them

		if (EEcChildrenTM(&htm, &numChildren, &fVarChildren) != EENOERROR)
			break;

		for (LONG i = 0 ; i < numChildren ; i++ )
			{
			HTM htmNew;

			// extract and evaluate the child TM (if possible)

			// If we could not get the child TM,  continue looking  further
			if (EEGetChildTM(&htm, i, &htmNew, &iEnd, 0, TRUE) != EENOERROR )
				continue;

			// It is possible that this evaluate fails, because the memory pointed
			// to is not valid etc. However we still add this TM to the child TM array,
			// as we might be able to evaluate it later ( after the debuggee state changes)

			EEvaluateTM(&htmNew,SHpFrameFrompCXF( pCxf ),EEVERTICAL);

            CRootTM* pChildTM = new CRootTM(HIND(htmNew), IS_HTM);
			childTMArray.Add(pChildTM);
			}

		// we would like to assert that the size of the array is
		// indeed the same as the size of the array but we can't
		// do so because the EE might run out of memory or some
		// such causing the API's to fail and hence not all of the
		// members to be present
		}
		break;
	}

	// if htm is a temporary TM (so far that can only occur if we
	// are dereferencing a pointer to a structure) then we must free it now
	if (fFreeHTM) EEFreeTM(&htm);

	EEEnableAutoClassCast(bOldState);

	if ( childTMArray.GetSize( ) != 0 )
		return TRUE ;
	else
		return FALSE;
}


CString CRootTM::GetName()
{	
	// use the cache if it is valid, a special case of that is
	// if this is a root TM then we saved the name the user typed explicitly...

	if (!m_fNameCacheValid)
	{
		// this will hold the string for the name...
		EEHSTR hstrName;
		HTM htm = (m_htm) ? m_htm : m_htmSaved;

		if (!htm || EEGetNameFromTM(&htm, &hstrName) != EENOERROR)
		{
			// error condition...		
			m_cstrNameCache.Empty();
		}
		else
		{
			// fetch the string and return a good pointer...
			char *buf = (char *)BMLock(hstrName);

			if (buf == NULL)
				m_cstrNameCache.Empty();	
			else
				m_cstrNameCache = buf;

			BMUnlock (hstrName);
			EEFreeStr (hstrName);
		}

		m_fNameCacheValid = TRUE;
	}

	ASSERT(m_fNameCacheValid);

	return m_cstrNameCache;
}

CString CRootTM::GetExpr()
{	
	EEHSTR hstrExpr;
	EERADIX radixT;
	USHORT strIndex;
	CString strExpr;
	HTM htm = (m_htm) ? m_htm : m_htmSaved ;

	if (htm && EEGetExprFromTM(&htm, &radixT, &hstrExpr, &strIndex) == EENOERROR)
	{
		// fetch the string and return a good pointer...
		char *buf = (char *)BMLock(hstrExpr);

		if (buf != NULL)
		{
			strExpr = buf;
		}
				
		BMUnlock (hstrExpr);
		EEFreeStr (hstrExpr);
	}

	return strExpr;
}

// Return the expr without any format qualifiers, so for example if you have
// an expression foo,x this would return just foo.

CString CRootTM::GetExprSansFormat()
{	
	EEHSTR hstrExpr;
	EERADIX radixT;
	USHORT strIndex;
	CString strExpr;
	HTM htm = (m_htm) ? m_htm : m_htmSaved ;

	if (htm && EEGetExprFromTM(&htm, &radixT, &hstrExpr, &strIndex) == EENOERROR)
	{
		// fetch the string and return a good pointer...
		char *buf = (char *)BMLock(hstrExpr);

		if (buf != NULL)
		{
			strExpr = buf;
			strExpr = strExpr.Left(strIndex);
		}
				
		BMUnlock (hstrExpr);
		EEFreeStr (hstrExpr);
	}

	return strExpr;
}

CString CRootTM::GetValue( )
{
	EEHSTR hstrValue;
	
	if (!m_fValueCacheValid)
	{
		EEHSTR hstrErr = NULL;
		LPSTR lpError;
		EESTATUS eestatus;

		m_radix = ::radix ;	// Remember the radix that we last used to get the value.

		if (!m_htm )
		{
			// If m_htm is NULL, the last TM we tried to Bind and evaluate is in
			// m_htmSaved. Use that TM to get the error string. (see RebindAndEvaluate)
			if ( (htmNil != m_htmSaved) &&
				 (EEGetError(&m_htmSaved, m_eestatus, &hstrErr) == EENOERROR) &&
				 ((lpError = (char *)BMLock(hstrErr)) != NULL)
				)
			{
				m_cstrValueCache = lpError;
				BMUnlock(hstrErr);
				EEFreeStr (hstrErr);
			}
			else
			{
				m_cstrValueCache.LoadString(ERR_Unknown_Vars_Error);
			}
		}										  					
		else if ((eestatus = EEGetValueFromTM(&m_htm, ::radix, NULL, &hstrValue)) != EENOERROR)
		{
			if ( (EEGetError(&m_htm, eestatus, &hstrErr) == EENOERROR) &&
				 ((lpError = (char *)BMLock(hstrErr)) != NULL)
				)
			{
				m_cstrValueCache = lpError;
				
				BMUnlock(hstrErr);
				EEFreeStr (hstrErr);
			}
			else
			{
				m_cstrValueCache.LoadString(ERR_Unknown_Vars_Error);
			}

			m_htmSaved = m_htm;
			m_htm = htmNil;

			
		}
		else
		{
			char *buf = (char *)BMLock(hstrValue);

			if (!buf || !buf[0]) {
				m_cstrValueCache.LoadString(ERR_Unknown_Vars_Error);
			    m_htmSaved = m_htm;
			    m_htm = htmNil;

            }
			else {
				m_cstrValueCache = buf;
            }

			BMUnlock (hstrValue);
			EEFreeStr (hstrValue);
		}

		m_fValueCacheValid = TRUE;
	}

	ASSERT(m_fValueCacheValid);

	return m_cstrValueCache;
}

CString CRootTM::GetValueInRadix (int iRadix)
{
	EEHSTR hstrValue;
	EESTATUS eestatus;
	CString strValue;

	// If the TM is in error or we can't get the value for some reason, the radix
	// shouldn't matter, just return the value using the default radix.
	if (!m_htm || (eestatus = EEGetValueFromTM(&m_htm, iRadix, NULL, &hstrValue)) != EENOERROR )
		return GetValue();

	char *buf = (char *)BMLock(hstrValue);
	if (!buf || !buf[0]) {
		strValue.LoadString(ERR_Unknown_Vars_Error);
    }
	else {
		strValue = buf;
    }

	BMUnlock (hstrValue);
	EEFreeStr (hstrValue);

	return strValue;
}


		
CString CRootTM::GetEditableValue( )
{
	CString displayValue(GetValue( ));
	// Extract the sub-string till the first blank.
	int firstBlankIndex = displayValue.Find( _T(' '));
	ASSERT( firstBlankIndex != 0);

	if ( firstBlankIndex == -1 )
	{
		// No blanks => whole string is editable.
		return (displayValue);
	}
	else
	{
		const TCHAR rgchQuote[] = "\'\"";

		int  firstQuoteIndex = displayValue.FindOneOf(rgchQuote);
		
		if ( firstQuoteIndex == -1 || firstBlankIndex < firstQuoteIndex )	
		{
			return ( displayValue.Left(firstBlankIndex));
		}
		else
		{
			// if we get here then we are probably a fortran string
			// that contains spaces
			// eg
			// "'abcd efg'"
			return displayValue;
		}
	}	
}	
	
BOOL CRootTM::IsValueEditable( )
{
	if (m_fValEditableCacheValid)
		return m_fValEditableCache;

	m_fValEditableCache = FALSE;
	m_fValEditableCacheValid  = TRUE ;

	// Can't edit the TM if we don't have a valid TM.
	if ( m_htm == htmNil )
		return FALSE;

	RTMI rtmi = { 0 }  ;
	HTI hTI ;

	// Request information about "lvalue"ness from the EE.
	rtmi.fLvalue = TRUE ;	

	if ( ( EEInfoFromTM(&m_htm, &rtmi, &hTI) != EENOERROR ) )
	{
		return FALSE ;
	}
	ASSERT( hTI != ( HTI ) NULL );

	// Get the TM Info structure.
	PTI pTI = ( PTI ) BMLock(hTI);
	ASSERT(pTI != NULL);

	BOOL retVal = pTI->fResponse.fLvalue  ;

	// Free up the resources.
	BMUnlock(hTI);
	EEFreeTI(&hTI);
	
	return m_fValEditableCache = retVal;		// cache value
}



CString CRootTM::GetType( )
{
	EEHSTR hstrValue;

	if ( !m_fTypeCacheValid )
	{
		HTM htm = m_htm;

		if (!htm || EENOERROR != EEGetTypeFromTM(&htm, NULL, &hstrValue, 0))
		{
			// error condition...
			m_cstrTypeCache.Empty();	// no type, error displayed in the value column.
		}
		else
		{
			char *buf = (char *)BMLock(hstrValue);
	
			if ( buf == NULL )
			{
				// error condition
				m_cstrTypeCache.Empty();
				return m_cstrTypeCache ;
			}

			// At the beginining of the type string is a HDR_TYPE structure followed
			// by the actual string. Increment pointer to get to the string.

			buf += sizeof(HDR_TYPE);

			m_cstrTypeCache = buf;

			BMUnlock (hstrValue);
			EEFreeStr (hstrValue);
		 }

		 m_fTypeCacheValid = TRUE;
	}

	ASSERT(m_fTypeCacheValid);

	return m_cstrTypeCache;
}

BOOL CRootTM::GetBCIA(PHBCIA phBCIA)
{
	BOOL retval = FALSE;
	HTM htm = m_htm;
	BOOL fFreeTM = FALSE;
	BOOL bOldState = EEEnableAutoClassCast(TRUE);
	USHORT iEnd;

	if (!m_htm)
		return retval;

	switch(EEIsExpandable(&m_htm))
	{
		case EEPOINTER:
			if (EEDereferenceTM(&m_htm, &htm, &iEnd, TRUE) != EENOERROR)
				break;

			fFreeTM = TRUE;
			if ( EEIsExpandable(&htm) != EEAGGREGATE )
			{
				// this is a plain pointer, no base classes here.
				break;
			}
			// Fall through
		case EEAGGREGATE:
			if ( EEGetBCIA(&htm, phBCIA) == EENOERROR )
			{
				retval = TRUE;
			}
			break;
		}

	if (fFreeTM)  EEFreeTM(&htm);

	EEEnableAutoClassCast(bOldState);

	return retval;
}

void CRootTM::FreeBCIA(PHBCIA phBCIA)
{
	EEFreeBCIA(phBCIA);
}

CTM::RES_REFRESH
CRootTM::Refresh()
{
	if ( m_bNoFurtherEval )
	{
		return SAME_TYPE; // Don't try and re-evaluate this expression.
	}
		
	// the debugger has stepped or memory has changed...
	// some event requires us to re-evaluate our expression...

	// We need to remember the type string at this point because
	// we are going to override and change back to the last bound
	// TM in the next step.

	CString oldTypeString;
	CString oldValueString;
	int oldRadix;

	BOOL oldValueValid = IsValueValid(); // Was the value valid before.

	if ( oldValueValid)
	{
		oldValueString = GetValue();
		oldRadix = m_radix;
	}

	if ( !m_fExpandedTM )
	{
		oldTypeString = GetType();
	}
	
	// m_fNameCacheValid is preserved through a refresh, but the value other
	// caches must be invalidated as they are no longer meaningful...
	InvalidateCaches();
	

	// if there is a saved TM then try to use that, this means
	// that we couldn't bind the TM in the original context that
	// was given but we might be able to do so now...

		
	if (m_htmSaved)
	{
		ASSERT(m_htm == htmNil);
		m_htm = m_htmSaved;
		m_htmSaved = htmNil;
	}

	// nil HTM cannot be rebound...
	if (!m_htm)
	{
		// check if we have a name that we couldn't parse...
		// if we don't have a name, the status of this TM is unchanged, so return SAME_TYPE.
		if (!m_fNameCacheValid)
			return SAME_TYPE  ;

		// if so, try to parse it now...
		char *szExpr = (char *)(const char *)m_cstrNameCache;

		BOOL fCase = TRUE;
		USHORT iEnd = 0;

		// if this fails, we can't refresh..
		// The parse failed before and still fails so we return SAME_TYPE
		// as the type status is still unknown.
		if (EEParse(szExpr, ::radix, fCase, &m_htm, &iEnd) != EENOERROR)
			return SAME_TYPE ;
	}

	// we now have a TM that we can try to bind and evaluate...

	ASSERT(m_htm != htmNil);

	// optimization .. assume that expanded types will not change because
    // if so the root type would have changed and we wouldn't be
	// re-evaluating this node at all...
	
	RES_REFRESH retVal;

	if (m_fExpandedTM)
	{
		RebindAndEvaluate();
		retVal = SAME_TYPE;
	}
	else
	{
		RebindAndEvaluate();

		// compare types and notify if change...
		CString newTypeString(GetType( ));
		retVal =  (oldTypeString == newTypeString) ? SAME_TYPE : TYPE_CHANGED;
	}

	// Do the neccessary checks to determine if the value changed.
	// The conditions are
	// a) The value should be valid both before and after the refresh.
	// b) the value should have changed.
	// c) we can't do simple string compares of the value string before and after because
	//    only the radix might have changed. So we need to check for that and then get the
	//    value in some canonical radix.
	
	if (oldValueValid && IsValueValid() && (oldValueString != GetValue( )))
	{
		// Check if this might be because the radix changed.
		if ( oldRadix != m_radix )
		{
			CString strValueInOldRadix = GetValueInRadix(oldRadix);
			m_bValueChanged = (oldValueString != strValueInOldRadix);
		}
		else
			m_bValueChanged = TRUE;
	}
	else
	{
		m_bValueChanged = FALSE;
	}

	return retVal;
}


// Update the value of the TM with a given string.
BOOL CRootTM::ModifyValue ( const CString& strValue )
{
	CXF curCxf ( ::CxfWatch( ) );
	PCXF pCxf = &curCxf   ;

	// If we don't have a valid TM we can't change the value.
	if ( m_htm == htmNil )
	{
		return FALSE ;
	}

		
	int radixExpr = 10;

	USHORT strIndex ;

	CString strAssignExpr = GetExprSansFormat();

	if (strAssignExpr.IsEmpty())
	{
		return ( FALSE );
	}

	// Create an assignment  expression of the form
	//  LHS = RHS .

	// Append the assignment operator to the string.
	strAssignExpr += rglan[ESilan()].AssignOp+1 ;

	// Finally append the RHS value
	strAssignExpr += strValue ;

	BOOL retVal = TRUE ;
	EEHSTR hTMAssignExpr  = htmNil ;

	// Cast away the constness so we can pass the string to the EE.
	LPSTR szAssignExpr = (char *)(const char *)strAssignExpr ;


	// Parse, bind and evaluate the expression to cause the value to change.
	if ( (EEParse( szAssignExpr, ::radix, FALSE,  &hTMAssignExpr , &strIndex) != EENOERROR) ||
		 (EEBindTM(&hTMAssignExpr, SHpCXTFrompCXF(pCxf), TRUE,FALSE) != EENOERROR) ||
		 (EEvaluateTM(&hTMAssignExpr , SHpFrameFrompCXF(pCxf), EEHORIZONTAL) != EENOERROR))
	{
		retVal = FALSE;		
	}

	if ( hTMAssignExpr  != htmNil )
	{
		EEFreeTM(&hTMAssignExpr);
	}
	
	return ( retVal );
}
					

EESTATUS
CRootTM::RebindAndEvaluate()
{
	EESTATUS eestatus;
	CXF curCxf( ::CxfWatch( ) ) ;	
	PCXF pCxf = &curCxf ;
	// It is very hard to get the EE/DHExecProc combination to restore
	// the callstack to the right frame. So we remember the frame number
	// before evaluating a function, and restore it after.
	int iCallsPrev = iCallsCxf;
	int chfme = CLChfmeOnStack();

	if ((eestatus = EEBindTM(&m_htm, SHpCXTFrompCXF(pCxf ),TRUE, FALSE)) == EENOERROR )
	{
		// If we succesfully bound it determin if it has a fnction eval.
		// The assumption is that if a succesful bind has a function eval every subsequent
		// bind will also have a function eval.
		if ( !m_bCheckedForFunc )
		{
			m_bCheckedForFunc = TRUE;

			RTMI rtmi = { 0 }  ;
			HTI hTI ;
			HTM htm = m_htm;
			
			// Request information from the EE.
			if ( ( EEInfoFromTM(&m_htm, &rtmi, &hTI) != EENOERROR ) )
			{
				return EEGENERAL ;
			}
			ASSERT( hTI != ( HTI ) NULL );

			// Get the TM Info structure.
			PTI pTI = ( PTI ) BMLock(hTI);
			ASSERT(pTI != NULL);

			BOOL retVal = FALSE;
			if ( pTI->fFunction )
			{
				m_bHasFunctionCall = TRUE;
			}
			
			
			if (m_bHasFunctionCall && m_bNoFuncEval)
			{
				m_bNoFurtherEval = TRUE;
				m_htmSaved = m_htm;
				m_htm = htmNil;
				return EENOERROR;
			}				

			// Free up the resources.
			BMUnlock(hTI);
			EEFreeTI(&hTI);
		}
		
		is_assign = FALSE;
		m_bHadSideEffects = FALSE;

		if (m_bHasFunctionCall && !lpprcCurr->IsBeyondEntryPoint())
		{
			// don't allow function evaluation until we have got to our entry point
			eestatus = EEBADADDR;
		}
		else
		{
			if ((eestatus = EEvaluateTM(&m_htm, SHpFrameFrompCXF(pCxf), EEVERTICAL)) != EENOERROR)
			{
				// Eval failed. If this expression has a function don't evaluate it from now on.
				// Might have an infinite loop or something and could cause bad things to happen.
				m_bNoFurtherEval =  m_bHasFunctionCall;
			}
			else {
				m_bHadSideEffects = is_assign;
			}
		}		
	}
	// restore the call-stack after the function call is done.
	if ( m_bHasFunctionCall && chfme != 0 )
	{
		CLGetWalkbackStack(hpidCurr, htidCurr, chfme);
		iCallsCxf = iCallsPrev;
	}

	if ( eestatus != EENOERROR )
	{
		// we (still) can't bind the TM we have, save it away, maybe later...
		m_htmSaved = m_htm;
		m_htm = htmNil;
	}

	m_eestatus = eestatus;

	// HACK: sometimes the EE will indicate that an evaluation has succeeded, when it
	// has the address to get to the value. The EE will not notice the problem (invalid addr)
	// till we actually try to fetch the value. So we fetch the value here and this should give
	// us the final, correct state of the TM. Note that the value string is cached and will
	// be re-used later.
	GetValue();
	
	
	return eestatus;
}


VOID
CRootTM::Restart()
{
	// the debuggee is restarting, the EE resources have been
	// freed right out from under us... mark them all as invalid
	// if we are not a root TM we will soon be destructed...

	// we DO NOT call EEFreeTM() on these TM's, the EE's memory allocator
	// has been reset completely, these TM's are completely useless...

	m_htm = htmNil;
	m_htmSaved = htmNil;
	m_bCheckedForFunc = FALSE;
	m_bHasFunctionCall = FALSE;
	m_bNoFurtherEval = FALSE;

	m_eestatus = EEGENERAL;
	// m_fNameCacheValid is preserved even through a restart, but the rest of the
	// caches are no longer meaningful...
	InvalidateCaches();
	
}


BOOL DebuggeeAlive(void);
PCXF PcxfLocal(void);
extern char szPath[_MAX_PATH];
BOOL PASCAL GetDebugTarget(PSTR szTarget, UINT cch);

/****************************************************************************

	FUNCTION:	CxfWatch()

	PURPOSE:	Returns a struct (not a pointer to struct) which is the
				context to use for Watch and QuickWatch expressions.
				[CUDA #3038 4/19/93 mikemo]

****************************************************************************/

CXF CxfWatch(void)
{
	CXF		cxf;
	HEXE	hexe;

	// Start with the CXF for the currently active stack frame.
	cxf = *PcxfLocal();

	// If we're in some nasty location that doesn't have source associated
	// with it, then modify our context to use the first module of the
	// program being debugged.

	if (cxf.cxt.hMod == 0 && DebuggeeAlive() &&
		GetDebugTarget(szPath, sizeof(szPath)))
	{
		hexe = SHGethExeFromName(szPath);
		cxf.cxt.hMod = SHGetNextMod(hexe, 0);
	}

	return cxf;
}

////////////////////////////////////////////////////////////////////////////////
// CReturnTM

IMPLEMENT_DYNAMIC(CReturnTM, CTM);

CReturnTM::CReturnTM (LPCSTR szName, LPCSTR szValue, LPCSTR szType)
{
	m_strName = szName;

	CString strReturned;
	strReturned.LoadString (IDS_RETURNED);

	m_strName += strReturned;

	m_strValue = szValue;
	m_strType = szType;

	m_fValueValid = TRUE;
}

CReturnTM::CReturnTM(const CReturnTM& rTM)
{
	m_strName = rTM.m_strName;
	m_strValue = rTM.m_strValue;
	m_strType = rTM.m_strType;
	m_fValueValid = rTM.m_fValueValid;
}
