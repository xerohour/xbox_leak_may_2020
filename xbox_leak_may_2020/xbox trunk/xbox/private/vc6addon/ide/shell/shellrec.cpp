/*

	Copyright 1996, Microsoft Corp.

	SHELLREC.CPP
		This header contains the implementation of the CShellRecorder object,
		which manages macro recording for the shell.

	HISTORY

		08-26-96	CFlaat	Adapted from pkgs\edit\editrec.cpp.
		08-27-96	CFlaat	Added some finishing touches.

*/


#include "stdafx.h"  // PCH
#include <utilauto.h>
#include "shellrec.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif


#ifdef _DEBUG
#include "typeinfo.h" // for debug checks
#endif // _DEBUG



// implementation of CGeneralRecorder

IMacroRecorder *CShellRecorder::m_pMacroRecorder;
//IScriptGenerator *CGeneralRecorder::s_pScrGen; // will be only VBS to start with

// implementation of CShellRecorder

CShellRecorder g_theShellRecorder;


const TCHAR * const CShellRecorder::szPkgName = "devshl";  /* REVIEW(CFlaat): isn't this stored someplace else?? */ 



void CShellRecorder::EmitCurrentAction(void)
{
#ifdef _DEBUG
	// this is a reentrancy check that's probably not needed, but just here for sanity
	static BOOL bEntered = FALSE;
	ASSERT(!bEntered);
	bEntered = TRUE;
#endif

	if (!m_pCurrentShellAction)
	{
		ASSERT(FALSE); // hey!  need to set up the current action before it can be emitted!
		return;
	}

	ASSERT(ShouldEmitCode()); // the caller should have verified this already!!

	CString str;

	m_pCurrentShellAction->GetText(str); // NOTE: this can't be factored above, since coalescing changes its value
	GetMacRec()->RecordText(str, GetPkgName());

	delete m_pCurrentShellAction;
	m_pCurrentShellAction = 0;

#ifdef _DEBUG
	bEntered = FALSE;
#endif
}


void CShellRecorder::EmitMethodCall(ShellElement eMethod)
{
  if (ShouldEmitCode())
	{
		ASSERT(!m_pCurrentShellAction);
		m_pCurrentShellAction = new CShellAction(eMethod, CShellAction::acMethod);
		EmitCurrentAction();
		ASSERT(!m_pCurrentShellAction);
	}
}

void CShellRecorder::EmitMethodCall(ShellElement eMethod, CRecArg &arg1)
{
  if (ShouldEmitCode())
	{
		ASSERT(!m_pCurrentShellAction);
		m_pCurrentShellAction = new CShellAction(eMethod, CShellAction::acMethod, arg1);
		EmitCurrentAction();
		ASSERT(!m_pCurrentShellAction);
	}
}


void CShellRecorder::EmitPropertyAssignment(ShellElement eProperty, CRecArg &arg)
{
  if (ShouldEmitCode())
	{
		ASSERT(!m_pCurrentShellAction);
		m_pCurrentShellAction = new CShellAction(eProperty, CShellAction::acProperty, arg);
		EmitCurrentAction();
		ASSERT(!m_pCurrentShellAction);
	}
}

void CShellRecorder::EmitParameterizedPropertyAssignment(ShellElement eProperty, CRecArg &param, CRecArg &value)
{
  if (ShouldEmitCode())
	{
		ASSERT(!m_pCurrentShellAction);
		m_pCurrentShellAction = new CShellAction(eProperty, CShellAction::acProperty, param, value);
		EmitCurrentAction();
		ASSERT(!m_pCurrentShellAction);
	}
}


CShellRecorder::CShellRecorder(void)
{
	m_pCurrentShellAction = 0;
}

CShellRecorder::~CShellRecorder(void)
{
	ASSERT(!m_pCurrentShellAction); // should have been deleted in EmitCurrentAction
}


// CShellAction implementation


CShellAction::CShellAction(ShellElement ee, ActionCat ac)
{
	m_eElement = ee;
	m_eCategory = ac;

	m_cArgs = 0;

#ifdef _DEBUG
	VerifyCorrectness();
#endif
}

CShellAction::CShellAction(ShellElement ee, ActionCat ac, CRecArg &arg1)
{
	m_eElement = ee;
	m_eCategory = ac;

	m_cArgs = 1;

	m_aArgs[0] = arg1.Clone(); // sort of a virtual copy ctor

#ifdef _DEBUG
	VerifyCorrectness();
#endif
}

CShellAction::CShellAction(ShellElement ee, ActionCat ac, CRecArg &arg1, CRecArg &arg2)
{
	m_eElement = ee;
	m_eCategory = ac;

	m_cArgs = 2;

	m_aArgs[0] = arg1.Clone(); // sort of a virtual copy ctor
	m_aArgs[1] = arg2.Clone(); // sort of a virtual copy ctor

#ifdef _DEBUG
	VerifyCorrectness();
#endif
}

#ifdef _DEBUG

/*
	Don't get scared by the functions below!  They simply verify (in debug builds
	only) that the element specified by m_eElement has the right corresponding
	arguments.  The funky	typeid comparisons are how we make sure that our
	generic CRecArg pointer actually corresponds to the right argument type.
	Because CRegArg objects are polymorphic (i.e. they have a vtable), we can
	use the RTTI typeid operator to determine actual the underlying type.

	Note: "ZB" here stands for "zero-based", just as a reminder.
*/

void CShellAction::CheckZBArgType(int nIndex, const type_info & Type)
{
	ASSERT(nIndex >= 0);
	ASSERT (nIndex < MaxArgCount);

	try
	{
		ASSERT(typeid(*(m_aArgs[nIndex])) == Type);
	}
	catch(bad_typeid &bt)
	{
		ASSERT(FALSE); // bad type given to typeid above!
	}
	catch(...)
	{
		ASSERT(FALSE); // some other horrible error!
	}
}

void CShellAction::CheckArgCount(int nIndex)
{
	ASSERT(nIndex >= 0); 
	ASSERT (nIndex <= MaxArgCount); 
	ASSERT(m_cArgs == nIndex);
}

void CShellAction::CheckArgCountRange(int nLow, int nHigh)
{
	ASSERT(nLow >= 0);
	ASSERT(nLow < nHigh);  // note: this call is only for ranges!
	ASSERT (nHigh <= MaxArgCount);
	ASSERT(nLow <= m_cArgs); ASSERT(m_cArgs <= nHigh);
}

void CShellAction::VerifyCorrectness(void)
{
#ifndef _DEBUG
#error Hey!  CShellAction::VerifyCorrectness is a debug-only integrity-check!
#endif

	try
	{
		switch (m_eElement)
		{
			default:
				{
					ASSERT(FALSE); // the input value is not in the list -- what gives?!?
					break;
				}


			case seCloseWin: // one optional const arg
			case seSaveAll:
				{
					ASSERT(m_eCategory == acMethod);

					CheckArgCountRange(0, 1); // can be 0 or 1 args
					if (m_cArgs == 1)
						CheckZBArgType(0, typeid(CShellConstArg));
					break; 
				}


			// these elements have zero parameters

			case seNewWin: // always takes zero args
				{
					ASSERT(m_eCategory == acMethod);

					CheckArgCount(0); // must have zero args
					break; 
				}

				// TODO(CFlaat): add cases here for each element

		}
	}
	catch(...)
	{
		ASSERT(FALSE); // some terrible error has occurred!!!!
	}

}

#endif

/*
		The global static functions Get/SetArg exist as a workaround to the compiler's current
	inability to handle member function templates.  Once member fn templates are available,
	we can use those.
*/
#if 0

template <class ValType, class ArgContainerType>
ValType GetArg(CShellAction *pEA, unsigned iArg, ValType vDefault, BOOL bCreate)
{
	ASSERT(iArg < CShellAction::MaxArgCount);

	if (iArg < pEA->m_cArgs)
		ASSERT( dynamic_cast<ArgContainerType*>(pEA->m_aArgs[iArg]) );

	if (iArg >= pEA->m_cArgs) // if the requested argument doesn't exist
	{
		if (bCreate)
		{
#ifdef _DEBUG
			{
				for (int i = 0; i < iArg; i++)
				{
					ASSERT(i < pEA->m_cArgs);
					ASSERT(i < CShellAction::MaxArgCount);
					ASSERT(pEA->m_aArgs[i]); // if we're creating an arg, no previous arg should be NULL
				}
			}
#endif // _DEBUG

			pEA->m_aArgs[iArg] = new ArgContainerType(vDefault);
			pEA->m_cArgs++;
		}
		return vDefault;
	}
	else
		return static_cast<ArgContainerType*>(pEA->m_aArgs[iArg])->GetValue();
}

template <class ValType, class ArgContainerType>
void SetArg(CShellAction *pEA, unsigned iArg, ValType vData)
{
	ASSERT(iArg < CShellAction::MaxArgCount);
	ASSERT(iArg <= pEA->m_cArgs);

	ASSERT(pEA->m_aArgs[iArg]);
	ASSERT(dynamic_cast<ArgContainerType*>(pEA->m_aArgs[iArg]));

	static_cast<ArgContainerType*>(pEA->m_aArgs[iArg])->SetValue(vData);
}

template ShellConst GetArg<ShellConst, CShellConstArg>(CShellAction *, unsigned, ShellConst, BOOL);
template int GetArg<int, CIntArg>(CShellAction *, unsigned, int, BOOL);
template void SetArg<int, CIntArg>(CShellAction *, unsigned int, int);


ShellConst CShellAction::GetConstArg(unsigned iArg, ShellConst ecDefault, BOOL bCreate /* =FALSE */)
{
	return GetArg<ShellConst, CShellConstArg>(this, iArg, ecDefault, bCreate);
}

int CShellAction::GetIntArg(unsigned iArg, int nDefault, BOOL bCreate /* =FALSE */)
{
	return GetArg<int, CIntArg>(this, iArg, nDefault, bCreate);
}

void CShellAction::SetIntArg(unsigned iArg, int nValue)
{
	SetArg<int, CIntArg>(this, iArg, nValue);
}

LPCTSTR CShellAction::GetQuotedStringArg(unsigned iArg, LPCTSTR szDefault, BOOL bCreate /* =FALSE */)
{
	return GetArg<LPCTSTR, CQuotedStringArg>(this, iArg, szDefault, bCreate);
}

void CShellAction::SetQuotedStringArg(unsigned iArg, LPCTSTR szValue)
{
	SetArg<LPCTSTR, CQuotedStringArg>(this, iArg, szValue);
}
#endif


void CShellAction::GetText(CString &str) // gets the language-specific representation
{

	if (m_eCategory == acProperty) // if a property
	{
		// properties should either have a single RHS arg or include a parameterization
		ASSERT((m_cArgs == 1) || (m_cArgs == 2));

		if (m_cArgs == 1)
			GetElementText(str);
		else
		{
			CString strParam;
			m_aArgs[0]->GetText(strParam);

			GetElementText(str, strParam);
		}

		CString strRHS;

		// if 1 arg, RHS is zero-based index 0; if 2 args, RHS is zero-based index 1
		m_aArgs[m_cArgs == 1 ? 0 : 1]->GetText(strRHS);

		str += _T(" = ");
		str += strRHS;

//		pScrGen->CreatePropertyAssignment(strLHS, strRHS);
//		pScrGen->Finish(str);
	}
	else // it's a method call
	{
		GetElementText(str);

		ASSERT(m_eCategory == acMethod);

		ASSERT(m_cArgs <= MaxArgCount);

		CString strArg;

		if (m_cArgs)
		{
			str += _T(' ');
			m_aArgs[0]->GetText(strArg); // special-case the first param
			str += strArg;

			for (int i = 1; i < m_cArgs; i++) // now take care of the rest, starting with #1
			{
				m_aArgs[i]->GetText(strArg);
				str += _T(", ");
				str += strArg;
			}
		}


//		for (int i = 0; i < m_cArgs; i++)
//			m_aArgs[i]->GetText(astrParams[i], pScrGen);
//		pScrGen->CreateMethodCall(strLHS, m_cArgs, astrParams);
//		pScrGen->Finish(str);
	}
}

void CShellAction::GetElementText(CString &str, LPCTSTR szParam /* =0 */ /*, IScriptGenerator *pScrGen*/) const
{
// in the editor we use a table for this, but since we have so few calls here presently that we'll be lazy

//	theIdentifierTable.GetTextFromElement(str, m_eElement, szParam);

	switch (m_eElement)
	{
	case seSaveAll:
		{
			str = "Documents.SaveAll";
			break;
		}

	case seCloseWin:
		{
			str = "ActiveWindow.Close";
			break;
		}
	case seNewWin:
		{
			str = "ActiveDocument.NewWindow";
			break;
		}
	}

	ASSERT(!str.IsEmpty());
}


CShellAction::~CShellAction(void)
{
  int i;

  for (i = 0; i < m_cArgs; i++)
		delete m_aArgs[i];
}


void CShellConstArg::GetText(CString &str)
{
	switch (GetValue())
	{
	case scTrue:
			{
				str = "True";
				break;
			}
	case scSaveChangesPrompt:
			{
				str = "dsSaveChangesPrompt"; /* "3" */
				break;
			}
	}
}

