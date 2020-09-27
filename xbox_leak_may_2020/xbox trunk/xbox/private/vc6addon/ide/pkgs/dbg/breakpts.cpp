  
/**********************************************************************

	FILE:			BREAKPTS.C [ BREAKPOINT HANDLER ]

	PURPOSE:		Routines for the QCQP persistent breakpoint handler.

**********************************************************************/
#include "stdafx.h"
#pragma hdrstop

#include <oleref.h>
#include <bldapi.h>
#include <bldguid.h>
#include "autobp.h"
#include "autobps.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

extern CXF cxfIp ;

// From SYSTEM.C
extern HEXE PASCAL SYHEXEFromADDR(PADDR) ;

// Global pointers to the start and end of the Breakpoint node list
static PBREAKPOINTNODE _far FirstBreakpointNode = (PBREAKPOINTNODE)NULL ;
static PBREAKPOINTNODE _far LastBreakpointNode = (PBREAKPOINTNODE)NULL ;
static PBREAKPOINTNODE _far LastBPCreated = (PBREAKPOINTNODE)NULL ;


// Number of lines we will scan down to find a line that has source
#define clinesScanForSource 15

#define CHECK_DISABLED_BREAKPOINTS  TRUE

int CCanInstantiateSourceBP( PBREAKPOINTNODE, LPPBP );
BOOL FInstantiateSourceBP( PBREAKPOINTNODE );

// For identifying instantiated source line breakponts
static int	iGroupMax = 0;

/**********************************************************************

	FUNCTION:	BFHFindDoc

	PURPOSE:	Enhanced version of FindDoc.  We will first call
				find doc.  If that fails, then try to find the doc
				on disk (a specified), if that too fails, try to
				find the doc as a base name.ext.

	RETURNS:	TRUE if doc found and *piDoc is updated to the iDoc
				found.

**********************************************************************/
BOOL PASCAL BFHFindDoc(	LPCSTR lpstrName, CDocument **ppTextDoc )
{
	BOOL	fFound;
	CPath	path;

	if ( !(fFound = gpISrc->FindDocument(lpstrName, ppTextDoc, TRUE)) )
	{
	if ( path.Create( (const TCHAR *)lpstrName ) )
	{
		// First try to find the fully qualified file name as
		// an open document
		if ( !(fFound = gpISrc->FindDocument((LPSTR)path.GetFullPath(), ppTextDoc, TRUE)) )
		{
			// If not found, we want to stop the search if the
			// file actually exists on disk or if the fullpath name
			// matches the passed in name.
			if ( !path.ExistsOnDisk() || _ftcsicmp( path.GetFullPath(), lpstrName ) )
				fFound = gpISrc->ThingyForBFHFindDoc(ppTextDoc, path);
		}
	}
	else
	{
		// See if we've got what's being looked for if CPath object
		// can't be created as a final try
		fFound = gpISrc->FindDocument(lpstrName, ppTextDoc, TRUE );
	}
	}
//	if (fFound)
//		gpISrc->SetCallback(*ppTextDoc, (FARPROC)TextDocCallback);
	return( fFound );
}

/**********************************************************************

	FUNCTION:	ParseQC25Location

	PURPOSE:	Takes the passed location specfier and attempts
				to fill the appropriate fields in the passed
				breakpoint node accordingly.

	RETURNS:	TRUE if location specfier is valid, FALSE otherwise.

	NOTE:		The following location formats are expected:

				C:\ABC\XYZ.C!.nnnnn
				.nnnnn

**********************************************************************/
BOOL PASCAL ParseQC25Location(
	LPSTR Location,
	PBREAKPOINTNODE pbpnFill)
{
	LPSTR StartPos, CurPos;
	CPath SrcName;
	long aLong;
	WORD NumToCopy;
	char TempName[_MAX_PATH];

	StartPos = Location;
	while (whitespace(*StartPos)) 
		StartPos = _ftcsinc(StartPos);

	// Nothing there
	if (*StartPos == '\0') return FALSE;

	// .nnnnn
	if (*StartPos == '.')
	{
		CView *pCurIDEView = GetCurView();

		if ( pCurIDEView == NULL )
			return FALSE;

		// Trying to specfiy a line number in current editor file
		const CString &strDocFileName = pCurIDEView->GetDocument()->GetPathName();

		if ((gpISrc->ViewBelongsToSrc(pCurIDEView) && !(strDocFileName.IsEmpty())))
		{
			if (fScanAnyLong(StartPos+1, &aLong, 0x1L, cLineBPLim))
			{
				// Ok we'll buy it.
				pbpnLocType(pbpnFill) = BPLOCFILELINE;

				NumToCopy = min(sizeof(pbpnLocCxtSource(pbpnFill))-1,
									 strDocFileName.GetLength());
				_fmemcpy(pbpnLocCxtSource(pbpnFill),
							strDocFileName,
							NumToCopy);
				pbpnLocCxtSource(pbpnFill)[NumToCopy] = '\0';

				// Zero out the rest of the context
				*pbpnLocCxtFunction(pbpnFill) = '\0';
				*pbpnLocCxtExe(pbpnFill) = '\0';

				// If the source file has been modified, set the initialial line number
				// to -1 so that the breakpoint is deleted if the file is closed
				// but not saved; else, we set it to the current line.
				pbpnCurLine(pbpnFill) = (int)aLong;
				if( pCurIDEView->GetDocument()->IsModified() )
					pbpnInitLine(pbpnFill) = -1;
				else
					pbpnInitLine(pbpnFill) = (int)aLong;

				return TRUE;
			}
		}

		return FALSE;
	}

	if ((CurPos = FindNonQuotedChar(StartPos, _T('!'))) != NULL)
	{
		// We should have a file name on the left hand side
		// and a .nnnnn line number specifier on the right.

		// Check .nnnnn first - it's cheaper
		if ((*(CurPos+1) == '.') && (fScanAnyLong(CurPos+2, &aLong, 0x1L, cLineBPLim)))
		{
			// Ok, so now the filename

			// Copy the filename
			// Null terminate temporarily
			*CurPos = '\0';

			// Do we need to remove quotes?
			if (*StartPos == _T('\"')) {

				LPSTR pEndQuote = _ftcschr(_ftcsinc(StartPos), _T('\"'));

				// Make sure there is a close quote
				if (pEndQuote == NULL) {
					return FALSE;
				}
				
				NumToCopy = pEndQuote - _ftcsinc(StartPos);
				_fmemcpy(TempName, _ftcsinc(StartPos), NumToCopy);
			}
			else
			{
				NumToCopy = min(sizeof(TempName)-1, CurPos-StartPos);
				_fmemcpy(TempName, StartPos, NumToCopy);
			}

			TempName[NumToCopy] = '\0';
			// Repair damage
			*CurPos = '!';

			if (ValidFilename(TempName, FALSE))
			{
				// Build this filename as relative to any current
				// makefile, or the current directory otherwise.
				CDir dir;

				LPCSTR pszPath;

				if (!gpIBldSys || (gpIBldSys->IsActiveBuilderValid() != S_OK) || FAILED(gpIBldSys->GetBuilderFile(ACTIVE_BUILDER, &pszPath)) || !dir.CreateFromPath(pszPath))
					VERIFY(dir.CreateFromCurrent());

				VERIFY(SrcName.CreateFromDirAndFilename(dir, TempName));

				// BUG #6792 01/10/93 v-natjm
				// Removed the OemToAnsi call since we are dealing with ANSI
				// names only...

				// We're happy
				pbpnLocType(pbpnFill) = BPLOCFILELINE;

				NumToCopy = min(sizeof(pbpnLocCxtSource(pbpnFill))-1,
									 _ftcslen(SrcName));
				_fmemcpy(pbpnLocCxtSource(pbpnFill),
							SrcName,
							NumToCopy);
				pbpnLocCxtSource(pbpnFill)[NumToCopy] = '\0';

				// Zero out the rest of the context
				*pbpnLocCxtFunction(pbpnFill) = '\0';
				*pbpnLocCxtExe(pbpnFill) = '\0';

				// by default, make the breakpoint non restorable
				// see if the file has been loaded and set the init line accordingly.
				pbpnCurLine(pbpnFill) = (int)aLong;

				CDocument   *pDoc;

				if( BFHFindDoc( SrcName, &pDoc ) )
				{
					if( pDoc->IsModified() )
						pbpnInitLine(pbpnFill) = -1;
					else
						pbpnInitLine(pbpnFill) = (int)aLong;
				}
				else
				{
					// since doc isn't loaded, make the breakpoint restorable
					pbpnInitLine(pbpnFill) = (int)aLong;
				}

				return TRUE;
			}
		}

		return FALSE;
	}

	// It's not a QC25 location specifier
	return FALSE;
}


/**********************************************************************

	FUNCTION:	FindNonQuotedChar

	PURPOSE:	Takes a pointer to a potential CV400 context string
				(or any string for that matter) and search for the given
				character, skipping anything inside quotes.

	RETURNS:	A pointer to the first occurrence of the char, or
				NULL if the character is not found outside quotes.

**********************************************************************/
LPSTR FindNonQuotedChar(LPSTR sz, UINT ch)
{
	LPSTR pStartQuote;
	LPSTR pch;
	LPSTR pCur = sz;

	while (pCur)
	{
		pch = _ftcschr(pCur, ch);
		if (pch == NULL) return NULL;

		pStartQuote = _ftcschr(pCur, _TCHAR('\"'));

		if (pStartQuote == NULL || pch < pStartQuote) return pch;
		
		// At this point, we've found our character, but it's somewhere after
		// the opening quote.  Set current position to just after the closing
		// quote, and re-search for the char.

		pCur = _ftcschr(_ftcsinc(pStartQuote), _TCHAR('\"'));
		if (pCur)
		{
			pCur = _ftcsinc(pCur);
		}
	}

	// No closing quote found
	return NULL;
}


BOOL FFilenameNeedsQuotes(LPCSTR szFile)
{
	UINT nLen;

	ASSERT(szFile);

	if (szFile[0] == '\0')
	{
		return FALSE;
	}

	// Is the filename already quoted?
	nLen = _ftcslen(szFile);
	if (szFile[0] == '\"' && szFile[nLen-1] == '\"')
	{
		return FALSE;
	}

	// Does the filename contain funny chars that would require quoting?
	return (_ftcscspn(szFile, " ,;{}+=") != nLen);
}

/**********************************************************************

	FUNCTION:	ExtractCV400Context

	PURPOSE:	Takes a pointer to a potential CV400 context string
				and attempts to split it into its component
				function, source, exe parts placing them in the
				passed BPCONTEXT structure.

	RETURNS:	TRUE if context successfully split up, FALSE otherwise.

**********************************************************************/
BOOL PASCAL ExtractCV400Context(
	LPCTSTR StartContext,
	PBPCONTEXT Context)
{
	LPCTSTR EndContext, CurPos, EndPos;
	LPCTSTR pStartQuote, pEndQuote;
	LPCTSTR pEndOfToken;
	WORD NumToCopy;
	CHAR szBuf[_MAX_PATH];

	while (whitespace(*StartContext)) 
		StartContext = _ftcsinc (StartContext);

	if ((*StartContext != OPENCXT) ||
		 ((EndContext = _ftcsrchr(StartContext, CLOSECXT)) == NULL))
	{
		return FALSE;
	}

	//
	// Function context
	//

	CurPos = _ftcsinc (StartContext);

	// Make sure that a () pair in the context is valid!

	// We need to calculate where the end of the function string
	// should be.  This can be a little tricky because the function part of
	// the context may or may not contain parens and commas (it could be
	// simply "foo" or it could be "foo(int, int, int)" and the filenames
	// may now contain the same chars.

	// find the first comma that is not within <>s or ()s
	int TemplateCount = 0;
	int BracketCount = 0;
	EndPos = CurPos;
	int c = 1;
	while ( (*EndPos) && (EndPos <= EndContext) )
	{
		c = *EndPos;

		switch (c)
		{
			case '<':
				TemplateCount++;
				break;
			case '>':
				TemplateCount--;
				break;
			case '(':
				BracketCount++;
				break;
			case ')':
				BracketCount--;
				break;
			case',':
				if ( (TemplateCount==0) && (BracketCount==0) )
					c = 0;					// found it, so exit
				break;
		}
		if (c==0)
			break;
		EndPos = _tcsinc( EndPos );
	}
	if ( (*EndPos!=',') || (EndPos > EndContext) )
		EndPos = NULL;
	LPCTSTR pFunc = CurPos;

	while (whitespace(*pFunc)) 
		pFunc = _ftcsinc(pFunc);

	if (*pFunc == _T(','))
	{
		if (pFunc > EndContext)
			return FALSE;
		// no function context
		EndPos = pFunc;
	}

	if (EndPos == NULL)
	{
		NumToCopy = min(sizeof(Context->Function)-1,
							EndContext-CurPos);
		_fmemcpy(Context->Function,
			CurPos,
			NumToCopy);
		Context->Function[NumToCopy] = '\0';
	}
	else
	{
		if ( EndPos-CurPos > sizeof( Context->Function ) - 3 )
		{
			// FUTURE: This stuff does not work correctly, especially if
			// we have templated type names as part of the type.
			LPTCH	lpch = _ftcschr( CurPos, _T('(') );
			LPTCH	lpchLastArg;
			int		cParen = 0;

			if ( !lpch )
			{
				return FALSE;
			}

			lpchLastArg = lpch = _ftcsinc (lpch);
			while( lpch-CurPos < sizeof( Context->Function ) - 3 )
			{
				if ( *lpch == _T('(') )
				{
					++cParen;
				}
				else if ( *lpch == _T(')') )
				{
					--cParen;
				}
				else if (cParen == 0 && *lpch == _T(',') )
				{
					lpchLastArg = lpch;
				}
				lpch = _ftcsinc( lpch );

				// Since the string is too big, we should NEVER
				// walk of the end!
				ASSERT( lpch || *lpch );
			}

			NumToCopy = lpchLastArg - CurPos;
			_fmemcpy(Context->Function,
				CurPos,
				NumToCopy);
			Context->Function[NumToCopy] = _T('\0');

			// Don't append anything if cParen is negative.  This
			// means that the end of the string had spaces padding
			// it beyond the end of valid space, just ignore it!
			if ( cParen >= 0 )
			{
				_fstrcat( Context->Function, "...)" );
			}
		}
		else
		{
			NumToCopy = EndPos-CurPos;
			_fmemcpy(Context->Function,
				CurPos,
				NumToCopy);
			Context->Function[NumToCopy] = _T('\0');
		}
	}

	if (EndPos == NULL) 
		goto FinishedContext;
	
	//
	// Source context
	//

	CurPos = _ftcsinc(EndPos);

	EndPos = _ftcschr(CurPos, _T(','));
	pStartQuote = _ftcschr(CurPos, _T('\"'));

	// Is the comma we found between quotes?
	if (pStartQuote != NULL &&
		(EndPos == NULL || pStartQuote < EndPos) )
	{
		CurPos = pStartQuote;

		// Find the end quote (a quote is not a legal file char so the
		// next quote must be the end quote)
		pEndQuote = _ftcschr(_ftcsinc(pStartQuote), _T('\"'));
		if (pEndQuote == NULL) return FALSE;
	
		EndPos = _ftcschr(pEndQuote, _T(','));
	}
	else
	{
		pStartQuote = NULL;
	}

	if (EndPos > EndContext)
		return FALSE;

	pEndOfToken = (EndPos) ? EndPos : EndContext;

	// _ftcsdec() not neccessary because we know it's a } or a ,
	NumToCopy = min(sizeof(Context->Source) - sizeof (TCHAR), pEndOfToken-CurPos);

	_fmemcpy(Context->Source, CurPos, NumToCopy);
	Context->Source[NumToCopy] = _T('\0');

	if (pStartQuote)
	{
		// Copy the filename (minus the quotes) into the tmp buffer
		// _ftcsint() not neccessary because we know it's a " and a \0
		_fmemcpy(szBuf, _ftcsinc(Context->Source), NumToCopy-(2*sizeof(TCHAR)));
		szBuf[NumToCopy-(2*sizeof(TCHAR))] = _T('\0');
	}
	else
	{
		// Copy the filename (plus da terminator) into the tmp buffer
		_fmemcpy(szBuf, Context->Source, NumToCopy+1);
	}

	if ((*(Context->Source) != _T('\0')) && !ValidFilename(szBuf, FALSE))
	{
		return FALSE;
	}

	if (EndPos == NULL) 
		goto FinishedContext;

	//
	// Exe context
	//

	CurPos = _ftcsinc (EndPos);

	if (CurPos > EndContext)
		return FALSE;
	
	EndPos = _ftcschr(CurPos, _T(','));

	if (EndPos > EndContext)
		EndPos = NULL;

	pStartQuote = _ftcschr(CurPos, _T('\"'));

	if (pStartQuote > EndContext)
		pStartQuote = NULL;

	// Make sure we take the next comma NOT between quotes
	if (pStartQuote != NULL &&
		(EndPos == NULL || pStartQuote < EndPos) )

	{
		CurPos = pStartQuote;

		// Find the end quote (a quote is not a legal file char so the
		// next quote must be the end quote)
		pEndQuote = _ftcschr(_ftcsinc(pStartQuote), _T('\"'));
		if (pEndQuote == NULL) return FALSE;
	
		EndPos = _ftcschr(pEndQuote, _T(','));
	}
	else
	{
		pStartQuote = NULL;
	}

	pEndOfToken = (EndPos) ? EndPos : EndContext;
	// _ftcsdec() not neccessary because we know it's a } or a ,
	NumToCopy = min(sizeof(Context->Exe)-sizeof (TCHAR), pEndOfToken-CurPos);

	_fmemcpy(Context->Exe, CurPos, NumToCopy);
	Context->Exe[NumToCopy] = _T('\0');

	if (pStartQuote)
	{
		// Copy the filename (minus the quotes) into the tmp buffer
		// _ftcsdec() not neccessary because we know it's a " and a \0
		_fmemcpy(szBuf, _ftcsinc(Context->Exe), NumToCopy-(2*sizeof(TCHAR)));
		szBuf[NumToCopy-(2*sizeof(TCHAR))] = _T('\0');
	}
	else
	{
		// Copy the filename (plus da terminator) into the tmp buffer
		// _ftcsinc() not neccessary because we know it's a \0
		_fmemcpy(szBuf, Context->Exe, NumToCopy + sizeof (_TCHAR));
	}

	if ((*(Context->Exe) != _T('\0')) && !ValidFilename(szBuf, FALSE))
	{
		return FALSE;
	}

FinishedContext:
	return TRUE;
}

/**********************************************************************

	FUNCTION:	FNNameFromHsym

	PURPOSE:	Given an hsym and pcxt, get the name of the symbol
				which matches the hsym and the argument list (types
				only).  Used in parsing/displaying overloaded source
				line breakpoints.

			INPUT:
				hsym		Symbol to get name of
				pcxt		pcxt containing hsym ref

			OUTPUT:
				lpstrName	string (cbMax = FUNC_NAME_LEN + 1)

	RETURNS:	nothing

**********************************************************************/
void PASCAL FnNameFromHsym(
	HSYM	hsym,
	PCXT	pcxt,
	LPSTR	lpstrName)
{
	CXT		cxt = *pcxt;
	EEHSTR	hstr;

	// If the proc doesn't have a symbol, try to get one!
	if ( !SHHPROCFrompCXT( &cxt ) ) {
		SHHPROCFrompCXT( &cxt ) = (HPROC)hsym;
	}

	// Just in case of a failure
	*lpstrName = '\0';

	// If hsym is NULL, then nothing to output!
	if ( EENOERROR == EEFormatCXTFromPCXT( &cxt, &hstr ) )
	{
		BPCONTEXT		bpc = {0};
		LPSTR			lpstr = (LPSTR)BMLock( hstr );

		// Get the function out of the EE formatted string
		// and copy into the destination buffer!
		if ( ExtractCV400Context( lpstr, &bpc ) )
		{
			_tcscpy( lpstrName, bpc.Function );
		}

		BMUnlock( hstr );
		EEFreeStr( hstr );
	}
}


/**********************************************************************

	FUNCTION:	ParseAllCV400Location

	PURPOSE:	Takes the passed location specfier and attempts
				to fill the appropriate fields in the passed
				breakpoint node accordingly.

	RETURNS:	TRUE if location specfier is valid, FALSE otherwise.

	NOTE:		The following location formats are expected:

				.nnnnn
				{,C:\ABC\XYZ.C,} .nnnnn
				FuncName
				{function,source,exe} FuncName

				If IsWndProc is TRUE then line number breakpoints are
				disallowed and the sizeof the expression part of
				the location (hopefully a WndProc name) is limited to the
				WndProc size.

**********************************************************************/
BOOL PASCAL ParseAllCV400Location(
LPCTSTR			Location,
PBREAKPOINTNODE pbpnFill,
BOOL 			IsWndProc)
{
	LPCTSTR 	StartPos;
	LPCTSTR	EndContext;
	LPCTSTR	CurPos;
	long 	aLong;
	WORD 	NumToCopy;

	StartPos = Location;

	while (whitespace(*StartPos)) 
		StartPos = _ftcsinc(StartPos);

	// Nothing there
	if (*StartPos == '\0') return FALSE;

	// The first thing we can do is pass the location through
	// the expression evaluator.  If it says it's not valid there
	// is no point continuing.
	if (!CheckExpression(StartPos, radixInput, TRUE))
	{
		return FALSE;
	}

	// Initialise the context
	*pbpnLocCxtFunction(pbpnFill) = '\0';
	*pbpnLocCxtSource(pbpnFill) = '\0';
	*pbpnLocCxtExe(pbpnFill) = '\0';

	// First set up any context:
	if ((EndContext = _ftcsrchr(StartPos, CLOSECXT)) != NULL)
	{
		if (!ExtractCV400Context(StartPos, &pbpnLocCxt(pbpnFill)))
		{
			return FALSE;
		}

		// _ftcsinc() not neccessary because we know it's a }
		CurPos = EndContext + sizeof (_TCHAR); 
		while (whitespace(*CurPos)) 
			CurPos = _ftcsinc(CurPos);
	}
	else
	{
		CurPos = StartPos;
	}

	// Right, the location specifier is either a .nnnnn or an expression
	// (NB either must be valid to have got through the pre-parse done
	// by CheckExpression() above) but we do our checking any way.
	if ((*CurPos == BP_LINELEADER) ||
		(*CurPos == BP_LINELEADER_OTHER)
	   )
	{
		if (IsWndProc)
		{
			// we're expecting a WndProc name
			return FALSE;
		}


		// Line number.  We must either have a file name in the context
		// or an active edit window from which to grab a file name:
		if ( !*pbpnLocCxtSource( pbpnFill ) )
		{
			CView   *pCurIDEView = GetCurView();

			// Is there a current view which is a DOC_WIN?  No, bail.
			if ( pCurIDEView == NULL || !gpISrc->ViewBelongsToSrc(pCurIDEView) )
				return FALSE;

			const CString &	strFileName = GetDebugPathName(pCurIDEView->GetDocument());

			// Ensure that there's a file name
			if ( strFileName.IsEmpty() ) {
				return FALSE;
			}

			// Valid filename in current view, copy to buffer for filename
			NumToCopy = min(sizeof( pbpnLocCxtSource( pbpnFill ) ) - 1,strFileName.GetLength());

			_fmemcpy(pbpnLocCxtSource(pbpnFill), strFileName, NumToCopy);

			pbpnLocCxtSource(pbpnFill)[NumToCopy] = '\0';

		}

		// Extract a line number, if there isn't one, fail
		if (fScanAnyLong(CurPos+1, &aLong, 0x1L, cLineBPLim))
		{
			// Finally, set up the type and fill in the line numbers.
			pbpnLocType(pbpnFill) = BPLOCFILELINE;

			// If the source file has been modified, set the initialial line number
			// to -1 so that the breakpoint is deleted if the file is closed
			// but not saved; else, we set it to the current line.
			pbpnCurLine(pbpnFill) = (int)aLong;

			CView   *pCurIDEView = GetCurView();

			if( pCurIDEView != NULL && pCurIDEView->GetDocument() && pCurIDEView->GetDocument()->IsModified() )
				pbpnInitLine(pbpnFill) = -1;
			else
				pbpnInitLine(pbpnFill) = (int)aLong;


			if (!*pbpnLocCxtExe (pbpnFill) &&
				pDebugCurr && pDebugCurr->IsJava ())
			{
				HBUILDER			hBld;
				VERIFY(SUCCEEDED(gpIBldSys->GetActiveBuilder(&hBld)));

				HBLDTARGET			hTarget;
				VERIFY(SUCCEEDED(gpIBldSys->GetActiveTarget(hBld, &hTarget)));

				LPPARSERDATABASE 	pParserDataBase;
				VERIFY(SUCCEEDED(theApp.FindInterface(IID_IParserDataBase, (LPVOID *) &pParserDataBase)));

				if (pParserDataBase != NULL)
				{
					int					grbv;
					BscEx				*pBsc;
					VERIFY(SUCCEEDED(pParserDataBase->GetBsc(&grbv, (UINT)hTarget, (HBSC *)&pBsc)));

					if (pBsc != NULL)
					{
						IMOD		imod;
						if (pBsc->getModuleByName(pbpnLocCxtSource (pbpnFill), &imod))
						{
							IINST		*rgiinstFunc;
							ULONG		ciinstFunc;
							if (pBsc->getModuleContents(imod, mbfFuncs, &rgiinstFunc, &ciinstFunc))
							{
								for (ULONG iinst = 0; iinst < ciinstFunc; iinst ++)
								{
									IDEF		*rgidef;
									ULONG		cDefs;

									pBsc->getDefArray(rgiinstFunc[iinst], &rgidef, &cDefs);
									for (ULONG iDef = 0; iDef < cDefs; iDef ++)
									{
										USHORT		iStartLine;
										LPTSTR		szName;

										pBsc->idefInfo(rgidef[iDef], &szName, &iStartLine);
										if (pbpnCurLine (pbpnFill) >= iStartLine)
										{
											USHORT		iEndLine;

											pBsc->irefEndInfo(rgidef[iDef], &szName, &iEndLine);
											if (pbpnCurLine (pbpnFill) <= iEndLine)
											{
												TYP			type;
												ATR			atr;

												pBsc->iinstInfo(rgiinstFunc [iinst], &szName, &type, &atr);
												LPTSTR pchDot;
												pchDot = _tcsrchr (szName, '.');
												if (pchDot != NULL)
												{
													ULONG cb = min (pchDot - szName, sizeof (pbpnLocCxtExe (pbpnFill)) - 1);
													memcpy (pbpnLocCxtExe(pbpnFill), szName, cb);
													pbpnLocCxtExe(pbpnFill)[cb] = '\0';
												}

												// exit the loops
												iDef = cDefs;
												iinst = ciinstFunc;
											}
										}
									}
								}
							}
							pBsc->disposeArray(rgiinstFunc);
						}
					}
				}

				pParserDataBase->Release();
			}
		}
		else
			return FALSE;
	}
	else
	{
		// Just to make sure we've not got very confused:
		if (!CheckExpression(CurPos, radixInput, TRUE))
			return FALSE;

		// Ok, we take whatever context was specified and
		// set the type and Address expression accordingly
		pbpnLocType(pbpnFill) = BPLOCADDREXPR;
		if (IsWndProc)
		{
			NumToCopy = min(sizeof(pbpnWndProc(pbpnFill))-1,
								 _ftcslen(CurPos));
			_fmemcpy(pbpnWndProc(pbpnFill),CurPos,NumToCopy);
			pbpnWndProc(pbpnFill)[NumToCopy] = '\0';
		}
		else
		{
			NumToCopy = min(sizeof(pbpnAddrExpr(pbpnFill))-1,_ftcslen(CurPos));
			_fmemcpy(pbpnAddrExpr(pbpnFill),CurPos,NumToCopy);
			pbpnAddrExpr(pbpnFill)[NumToCopy] = '\0';
		}
	}

	// If we get to here we're ok.  (In fact it's a miracle)
	return TRUE;
}

/**********************************************************************

	FUNCTION:	ParseCV400Location

	PURPOSE:	Takes the passed location specfier and attempts
				to fill the appropriate fields in the passed
				breakpoint node accordingly.

	RETURNS:	TRUE if location specfier is valid, FALSE otherwise.

	NOTE:		The following location formats are expected:

				.nnnnn
				{,C:\ABC\XYZ.C,} .nnnnn
				FuncName
				{function,source,exe} FuncName

**********************************************************************/
BOOL PASCAL ParseCV400Location(
	LPSTR Location,
	PBREAKPOINTNODE pbpnFill)
{
	return ParseAllCV400Location(Location, pbpnFill, FALSE);
}


/**********************************************************************

	FUNCTION:	ParseExpression

	PURPOSE:	Takes the passed expression and attempts
				to fill the appropriate fields in the passed
				breakpoint node accordingly.

	RETURNS:	TRUE if expression is valid, FALSE otherwise.

**********************************************************************/
BOOL PASCAL ParseExpression(
	LPCTSTR Expression,
	PBREAKPOINTNODE pbpnFill)
{
	LPCTSTR StartPos;
	WORD NumToCopy;

	StartPos = Expression;
	while (whitespace(*StartPos)) 
		StartPos = _ftcsinc(StartPos);

	// Nothing there
	if (*StartPos == '\0') return FALSE;

	// First, check that it is parses
	if (!CheckExpression(StartPos, radixInput, TRUE))
	{
		return FALSE;
	}

	// Initialise context
	*pbpnExprCxtFunction(pbpnFill) = '\0';
	*pbpnExprCxtSource(pbpnFill) = '\0';
	*pbpnExprCxtExe(pbpnFill) = '\0';

	// Fill in the expression field in pbpnFill
	NumToCopy = min(sizeof(pbpnExpression(pbpnFill))-1,
						 _ftcslen(StartPos));
	_fmemcpy(pbpnExpression(pbpnFill),
				StartPos,
				NumToCopy);
	pbpnExpression(pbpnFill)[NumToCopy] = '\0';

	return TRUE;
}


/**********************************************************************

	FUNCTION:	ParseWndProc

	PURPOSE:	Takes the passed WndProc specifier and attempts
				to fill the appropriate fields in the passed
				breakpoint node accordingly.

	RETURNS:	TRUE if WndProc is valid, FALSE otherwise.

**********************************************************************/
BOOL PASCAL ParseWndProc(
	LPSTR WndProc,
	PBREAKPOINTNODE pbpnFill)
{
	return ParseAllCV400Location(WndProc, pbpnFill, TRUE);
}


/**********************************************************************

	FUNCTION: 	EnsureExprContext

	PURPOSE: 	Called after a CV400 bp has been committed.  Ensures
				that any context assumptions made by the CV400 bp
				handler are reflected in the context structure in
				our bp nodes.

	NOTE:		This only takes action if no context has already been
				specified for the expression.

**********************************************************************/
void PASCAL NEAR EnsureExprContext(
	PBREAKPOINTNODE pbpn)
{
	LPBPI lpbpi = (LPBPI)NULL ;
	HCXTL hCXTL = (HCXTL)NULL ;
	PCXTL pCXTL = (PCXTL)NULL ;
	EEHSTR hCxtStr = (EEHSTR)NULL;
	LPSTR pCxtStr = (LPSTR)NULL;

	if(pbpnBPhbpi(pbpn) == 0) return ;

	switch (pbpnType(pbpn))
	{
		case BPLOCEXPRTRUE:
		case BPLOCEXPRCHGD:
		case BPEXPRTRUE:
		case BPEXPRCHGD:
			if (*pbpnExprCxtFunction(pbpn) ||
				*pbpnExprCxtSource(pbpn) ||
				*pbpnExprCxtExe(pbpn))
			{
				// Already have a context
				break;
			}

			//////////////////////////////////////////////////////////////////
			//
			// I don't believe that this is remotely correct.  When a complex
			// expression is passed in, there may be more than 1 context for
			// the expression and this will only get one of them.
			//
			// MarkBro 10/26/93
			//
			//////////////////////////////////////////////////////////////////

			// Ask the EE for the (ascii) context of the expression
			lpbpi = (LPBPI)LLLpvFromHlle(pbpnBPhbpi(pbpn));
			if (lpbpi ) {
				if ( EEGetCXTLFromTM ( ( PHTM )( &(lpbpi->lpDpi->hTM) ),
					( PHCXTL )&hCXTL ) == EENOERROR ) {
					if ( pCXTL = (PCXTL)BMLock(hCXTL) ) {
						if ( EEFormatCXTFromPCXT(&(pCXTL->CXT), &hCxtStr) == 0 ) {
							pCxtStr = (char *)BMLock(hCxtStr);
						}
					}
				}
			}
			if (pCxtStr)
			{
				ExtractCV400Context(pCxtStr, &pbpnExprCxt(pbpn));
				//!_ftcslwr(pbpnExprCxtSource(pbpn));
				//!_ftcslwr(pbpnExprCxtExe(pbpn));

				BMUnlock(hCxtStr);
				EEFreeStr(hCxtStr);
			}
			if (pCXTL)
			{
				BMUnlock(hCXTL);
				EEFreeCXTL(&hCXTL);
			}
			if (lpbpi)
			{
				UnlockHlle(pbpnBPhbpi(pbpn));
			}
			break;

		default:
			break;
	}
}

/****************************************************************************

	FUNCTION:   GetCV400BPPacket()

	PURPOSE:    Takes a CV400 format command and returns the corresponding
				CV400 BP packet before commital.

	RETURNS:	The return code from BPParse

****************************************************************************/
int PASCAL NEAR GetCV400BPPacket(PSTR CV400BPCmd, LPPBP pBPPacket, PCXF pCXF)
{
	// Set up the breakpoint packet to parse and set if valid
	_fmemset((void FAR *)pBPPacket, 0, sizeof(PBP));
	pBPPacket->hthd = 0;
	pBPPacket->BPType = BPBREAKPT;
	pBPPacket->lszCmd = CV400BPCmd;
	pBPPacket->BPSegType = EECODE;
	pBPPacket->iErr = -1;

	if(pCXF)
	{
		pBPPacket->pCXF = pCXF;
	}
	else
	{
		pBPPacket->pCXF = &cxfIp;
	}

	//!return BPParse(pBPPacket)    ;
	{
		int ret;

		ret = BPParse(pBPPacket);
		return ret;
	}
}


/****************************************************************************

	FUNCTION:   GetBpAtNode()

	PURPOSE:    Check if a BP can be set
				If it is a line BP, search forward to find a valid line

	RETURNS:	TRUE if BP can be set, FALSE if not

****************************************************************************/
BOOL PASCAL NEAR GetBpAtNode(
	PBREAKPOINTNODE pbpn,
	LPPBP pBPPacket)
{
	char	CV400BPCmd[cbBpCmdMax]; // Breakpoint description

	if (pbpnFileLineNode(pbpn) )
	{
		// If the breakpoint is a line breakpoint, we will search forward
		// for a valid line to put the breakpoint if we can't put it on
		// the current line.  We decide that if a valid BP line is more
		// than clinesScanForSource lines away, we can't set the breakpoint

		return CCanInstantiateSourceBP( pbpn, pBPPacket );
	}

	// Create a CV BP descriptor string from our descriptor
	*CV400BPCmd = '\0';
	MakeCV400BPCmd(pbpn, CV400BPCmd, sizeof(CV400BPCmd));

	// Try to set the BP at CurLine+increment
	if (GetCV400BPPacket(CV400BPCmd, pBPPacket, NULL) == EENOERROR)
	{
		//AuxPrintf("###PMF: Breakpoint set");
		// BP set
		return TRUE;
	}

	// AuxPrintf2("##PMF: Can't set breakpoint %s",(LPSTR)CV400BPCmd);
	return FALSE;
}

/****************************************************************************

	FUNCTION:	LoopExeGetCV400BPPacket

	PURPOSE:	Loop through the executables (exe, dll...) of the debuggee
				to check if the BP can be set.

	RETURNS:	TRUE if the breakpoint can be set, FALSE if not
				The LPPBP CV400 breakpoint descriptor is filled by the function

****************************************************************************/
BOOL PASCAL LoopExeGetCV400BPPacket(
	PBREAKPOINTNODE pbpn,
	LPPBP pBPPacket)
{
	return (GetBpAtNode(pbpn, pBPPacket));
}


/****************************************************************************

	FUNCTION:   SetCV400BP()

	PURPOSE:    Takes a CV400 format command and attempts to commit it
				to the current list of breakpoints.

	RETURNS:	TRUE if successful, FALSE otherwise.  CV400 internal
				error returned in passed buffer.

****************************************************************************/
HBPI PASCAL SetCV400BP(PSTR CV400BPCmd, int *pBPRet, LPINT lpiErr)
{
	PBP 	BPPacket;
	CXT		cxt;
	CXF		cxf = {0};
	PCXF	pCXF;
	CXF curCxf ( CxfWatch( ) );

	if (lpprcCurr->stp == stpNotStarted && get_initial_context (&cxt))
	{
		// This is to construct a valid frame for
		// error checking later on.
		// Load the frame with the IP frame
		pCXF = &cxf;
		*SHpFrameFrompCXF(pCXF) = *SHpFrameFrompCXF(&cxfIp);
		*SHpCXTFrompCXF ( pCXF ) = cxt;
	}
	else
	{
		pCXF = &curCxf;
	}

	*pBPRet = GetCV400BPPacket(CV400BPCmd, (LPPBP)&BPPacket, pCXF);
	if(lpiErr != (LPINT)NULL)
		*lpiErr = BPPacket.iErr ;
	if (*pBPRet == EENOERROR)
	{
		return AsyncBPCommitBP((LPPBP)&BPPacket);
	}

	return NULL;
}

/**********************************************************************

	Implementation     : J.M. Nov 1992
	BHSetBPExtInfo()   : BREAKPTS.C

	Purpose    : Set flag to indicate the need of extended info on display
	Input      : A breakpoint node pointer
	Returns    : None

**********************************************************************/
void NEAR PASCAL BHSetBPExtInfo(PBREAKPOINTNODE pbpn)
{
LPBPI lpbpi ;

	if(pbpnBPVirtual(pbpn)) {
		pbpnBPExtInfo(pbpn) = FALSE ;
	}
	else {
		// Check if ambig flag set at CV400 level and set ours as a reminder !
		lpbpi = (LPBPI)LLLpvFromHlle(pbpnBPhbpi(pbpn));
		if(lpbpi->bpf.f.fAmbig == abptNotAmbig)
			pbpnBPExtInfo(pbpn) = FALSE ;
		else
			pbpnBPExtInfo(pbpn) = TRUE ;
		UnlockHlle(pbpnBPhbpi(pbpn));
	}
}


/****************************************************************************

	FUNCTION:   SetBpAtNode()

	PURPOSE:    Try to set the BP described in the pbpn struct.
				If it is a line BP, search forward to find a valid line

	RETURNS:	TRUE if BP has been set, FALSE if not

****************************************************************************/
BOOL PASCAL NEAR SetBpAtNode(
	PBREAKPOINTNODE pbpn,
	int *pBPRet,
	LPINT lpiErr)
{
	char	sz[cbBpCmdMax]; 	// Breakpoint description
	int 	iErr = 0;			// Error return from BPParse
	int 	BPRet = 0;			// Another error return from BPParse (sigh)

	// Line breakpoint...
	if (pbpnFileLineNode(pbpn))
	{
		pbpnEbpt( pbpn ) = BPLINE;
		// Attempt to instantiate the source line.  If zero, then try to
		// let CV's breakpoint hander doit.  Otherwise return success
		if ( FInstantiateSourceBP( pbpn ) )
		{
			return TRUE;
		}
	}

	// Create a CV BP descriptor string from our descriptor
	*sz = '\0';
	MakeCV400BPCmd(pbpn, sz, sizeof(sz));

	// Try to set the BP at CurLine+increment
	if ( pbpnBPhbpi(pbpn) = SetCV400BP(sz, &BPRet, &iErr) )
	{
//#ifdef SYMBOLIZE_ADDR
		EBPT	ebpt = BPLINE;

		LPBPI	lpbpi = (LPBPI) LLLpvFromHlle(pbpnBPhbpi(pbpn));

		if (pDebugCurr && pDebugCurr->IsJava())
		{
			// For Java, we want to store the EXE part of the context in the BPN
			// so we can load symbols for that class on start up the next time around.

			// Get the HEXE from the BPI's CodeAddr and get the name of the
			// EXE (class) from the SH.
			HEXE hexeClass = emiAddr(lpbpi->CodeAddr);
			LPTSTR pszClass = SHGetExeName(hexeClass);
			if (pszClass != NULL)
			{
				switch (pbpnType(pbpn)) {
					case BPLOC:
					case BPLOCEXPRTRUE:
					case BPLOCEXPRCHGD: {
						_tcscpy(pbpnLocCxtExe(pbpn), pszClass);
						break;
					}
					case BPEXPRTRUE:
					case BPEXPRCHGD: {
						_tcscpy(pbpnExprCxtExe(pbpn), pszClass);
						break;
					}
				}
			}
		}

		// if this is a SQL Bp then set the flag
		// all this checking is probably not necessary
		// but I thought I had better be safe.
		if (lpbpi && 
			(pbpnType(pbpn) == BPLOC) &&
			(pbpnEbpt(pbpn) == BPLINE) &&
			(lpbpi->CodeAddr.mode.fSql))
		{
			pbpnSqlBp(pbpn) = TRUE;
		}
		else
		{
			ASSERT(!pbpnSqlBp(pbpn));
		}

		// Only convert if not tried and it's not a line number
		// breakpoint.
		if ( !pbpnSymbolized( pbpn ) && !pbpnFileLineNode( pbpn ) )
		{
			char *	pchT = sz;

			while( whitespace( *pchT ) )
			{
				pchT = _tcsinc( pchT );
			}

			// Skip over leading quote (if there's one)
			if ( *pchT == _T('\"') ) {
				pchT = _ftcsinc (pchT);
			}

			while( whitespace( *pchT ) )
			{
				pchT = _ftcsinc( pchT );
			}

			ebpt = BPUNKNOWN;

			// Only do this if there's a constant address
			if ( _istdigit( (_TUCHAR)*pchT ) &&
				BPGetBpCmd( pbpnBPhbpi( pbpn ), EECODE, &ebpt, sz, TRUE ) &&
				ebpt == BPLINE )
			{
				if ( strlen( sz ) < MAX_EXPRESS_SIZE )
				{
					// Remove trailing quote
					char *	pch = _tcschr( sz + 1, _T('\"') );
					char *	pchExpr = _tcschr( sz, CLOSECXT );
					long	lT;

					ASSERT( pch );
					*pch = '\0';

					*pbpnLocCxtFunction( pbpn ) = '\0';
					*pbpnLocCxtSource( pbpn ) = '\0';
					*pbpnLocCxtExe( pbpn ) = '\0';

					// Skip over enclosing quote
					if ( pchExpr ) {
						// Skip past end cxt char
						pchExpr = _ftcsinc (pchExpr);
						ExtractCV400Context( sz + 1, &pbpnLocCxt( pbpn ) );
					}
					else {
						// copy the whole string (except leading quote!)
						ASSERT( *sz == '\"' );
						pchExpr = _ftcsinc (sz);
					}

					while( whitespace( *pchExpr ) )
					{
						pchExpr = _ftcsinc(pchExpr);
					}

					fScanAnyLong(pchExpr + 1,&lT,1L,cLineBPLim);

					pbpnInitLine( pbpn ) = pbpnCurLine( pbpn ) = (int)lT;
					pbpnLocType( pbpn ) = BPLOCFILELINE;
				}
			}

			pbpnEbpt( pbpn ) = ebpt;
		}
		pbpnSymbolized( pbpn ) = TRUE;
// #endif // SYMBOLIZE_ADDR
		BPRet = iErr = 0;
	}

	// Save return values if requested
	if (pBPRet != NULL)
	{
		*pBPRet = BPRet;
	}

	if (lpiErr != NULL)
	{
		*lpiErr = iErr;
	}

	return (BOOL)( iErr == 0 );
}

/****************************************************************************

	FUNCTION:	LoopExeSetCV400BP

	PURPOSE:	Loop through the executables (exe, dll...) of the debuggee
				and set the BP.

	RETURNS:	TRUE if the breakpoint can be set, FALSE if not

****************************************************************************/
BOOL NEAR PASCAL LoopExeSetCV400BP(
PBREAKPOINTNODE pbpn,
int *			pBPRet,
LPINT 			lpiErr )
{
	// CAVIAR 6473 : must check return value of SetCV400BP as this operation
	// sometimes requires large amounts of extra memory (watch on large
	// memory area) [rm]
	if (SetBpAtNode(pbpn, pBPRet, lpiErr))
	{
		BHSetBPExtInfo(pbpn) ;
		return !pbpnBPVirtual(pbpn) || pbpnBPGroup( pbpn );
	}

	return FALSE;
}

/**********************************************************************

	Implementation     : J.M. Nov 1992
	BHSetLastBPNode()  : BREAKPTS.C

	Purpose    : Keep track of the last BP node we work on
	Input      : bpNode       : an IDE BP node
	Returns    : None

**********************************************************************/
VOID PASCAL BHSetLastBPNode(
PBREAKPOINTNODE 	pbpn )
{
	// When the CV breakpoint handler calls our disambiguation
	// dialog box, we want to access our BP node data to avoid
	// disambiguation when we know which TM we want to use.
	LastBPCreated = pbpn ;
}



/**********************************************************************

	Implementation     : J.M. Nov 1992
	BHCreateBPHandle() : BREAKPTS.C

	Purpose    : Bind a virtual IDE BP to a valid hbpi
	Input      : pbpn       : an IDE BP node
				 SetAtStartup : if set at debuggee startup
				 fQuiet       : if we notify on failure or not
				 lpiErr       : what component of expr failed
	Returns    : TRUE / FALSE if we could bind it or not

**********************************************************************/
BOOL PASCAL BHCreateBPHandle(
PBREAKPOINTNODE	pbpn,
BOOL 			SetAtStartUp,
BOOL 			fQuiet,
LPINT 			lpiErr )
{
	char 	szBPCmd[cbBpCmdMax];
	int 	BPRet;
	UINT 	ErrorId;

	if(!BHFTypeSupported(pbpnType(pbpn)))
	{
		return TRUE;
	}

	// Check if Breakpoint was already bound
	if(!pbpnBPVirtual(pbpn))
	{
		return TRUE ;
	}

	// Remember the current breakpoint
	BHSetLastBPNode(pbpn) ;

SetTheBP:
	// Try and set the bp until all our options are exhausted - we
	// loop through exes here for location bps.
	BPRet = 0;

	// location breakpoints on hard addresses are not to be restored
	if ( (SetAtStartUp && pbpnEbpt( pbpn ) == BPADDR ) ||
		!LoopExeSetCV400BP(pbpn, &BPRet,lpiErr))
	{
		if (!pbpnExprUseCxt(pbpn) && pbpnExpressionNode(pbpn))
		{
			// Failed, but haven't tried the expression context yet
			pbpnExprUseCxt(pbpn) = TRUE;
			goto SetTheBP;
		}

		// Make sure we reset the hbpi
		pbpnBPhbpi(pbpn) = (HBPI)NULL ;

		// Set the disable flag at low level...
		pbpnEnabled (pbpn) = FALSE ;

		if(!fQuiet) {
			if ( SetAtStartUp && pbpnEbpt( pbpn ) == BPADDR ) {
				ErrorId = ERR_Const_Address_Breakpoint;
			}
			else if ( BPRet == NOCODE ) {
				ErrorId = ERR_No_Code_For_Line;
			}
			else {
				ErrorId = ERR_Couldnt_Set_Breakpoint;
			}

			FormatDisplayBP(pbpn, szBPCmd, sizeof(szBPCmd));

			// [BUG #3397 10/11/1992 v-natjm]
			if (AutoTest && SetAtStartUp)
			{
				DebuggerMessage(Information, ErrorId, (LPSTR)szBPCmd, SHOW_IMMEDIATE) ;
				pbpnInitError( pbpn ) = ErrorId;
			}
			else
			{
				InformationBox(ErrorId, (LPSTR)szBPCmd);
			}
		}
		return FALSE;
	}

	// Set the context for entered expression - only do this
	// if not being set at start up as we know that no global
	// context is necessary
	if (!SetAtStartUp)
	{
		EnsureExprContext(pbpn);
	}
	return TRUE;
}

/****************************************************************************

	FUNCTION:	FBreakpointFileExists

	PURPOSE:	Callback function used to by the file picker dialog to
				determine if the indicated file meets some secondary
				criteria (in this case we insist that the file exists
				on disk)

****************************************************************************/
static BOOL FBreakpointFileExists(LPCSTR lszPath, LPCSTR lszFilename, LONG)
{
	CDir dir;
	CPath path;

	return dir.CreateFromString(lszPath) &&
		   path.CreateFromDirAndFilename(dir, lszFilename) &&
		   path.ExistsOnDisk();
}

/****************************************************************************

	FUNCTION:	BHGotoBP

	PURPOSE:	View source (or disassembly if no source) for a breakpoint

****************************************************************************/
void PASCAL BHGotoBP(PBREAKPOINTNODE pBpNode)
{
	CPath	path;
	CHAR	szNewFile[_MAX_PATH];

	// caller should have called BHCanGotoBP before calling BHGotoBP
	ASSERT(BHCanGotoBP(pBpNode));

	// first check for an actual nonvirtual address, because that's the
	// preferred way to find the breakpoint location (will get it
	// exactly right)
	if (DebuggeeAlive() && !pbpnBPVirtual(pBpNode))
	{
		LPBPI	lpbpi = (LPBPI) LLLpvFromHlle(pbpnBPhbpi(pBpNode));

		ASSERT(lpbpi->bpf.f.fBpCode);
		ASSERT(!lpbpi->bpf.f.fVirtual);
		MoveEditorToAddr(&lpbpi->CodeAddr);

		UnlockHlle(pbpnBPhbpi(pBpNode));
	}
	// else it's a file/line breakpoint
	else
	{
		ASSERT(pbpnFileLineNode(pBpNode));
		ASSERT(pbpnFileName(pBpNode)[0]);
		ASSERT(pbpnCurLine(pBpNode));

		// let non-disk docs get a chance before converting to path
		CDocument *pDoc;
		if (LoadNonDiskDocument( pbpnFileName(pBpNode), &pDoc ))
		{
			// If another view is on top let's get this one
			CView *pView = gpISrc->FindTextDocActiveView(pDoc);
			ASSERT(pView);
			// Make sure window active
			theApp.SetActiveView(pView->GetSafeHwnd());
			gpISrc->Select(pView, pbpnCurLine(pBpNode) - 1, 0, FALSE);
			return;
		}

		// ActivateTextDoc requires canonical name
		if (!path.Create(pbpnFileName(pBpNode)))
			MessageBeep(0);
		else if (!gpISrc->ActivateTextDoc(path, pbpnCurLine(pBpNode) - 1))
		{
			CDir dir;
			CString strPrompt, strTitle;
			DIALOGPARAM dlgParam;
			CHAR szOldFile[_MAX_PATH];	// not just _MAX_FILE
			CHAR szOldExt[_MAX_EXT];

			dir.CreateFromCurrent();
			_tcscpy(szNewFile, (LPCTSTR)dir);

			LPCSTR pszPath;

			if ((gpIBldSys == NULL) ||
				(gpIBldSys->IsActiveBuilderValid() != S_OK) ||
				FAILED(gpIBldSys->GetBuilderFile(ACTIVE_BUILDER, &pszPath)) ||
				pszPath == NULL || 
				!dir.CreateFromPath(pszPath))
			{
				// There's no project, but we still want to set dir to
				// something useful for the call to FMapPath() below.  So we
				// set it to the current directory.
				VERIFY(dir.CreateFromCurrent());
			}

			_splitpath(path, NULL, NULL, szOldFile, szOldExt);
			_tcscat(szOldFile, szOldExt);	// szOldFile contains file base+ext

			// Before putting up the FindSource dlg, check the Source Dirs
			LPSTR lpszFullPath = NULL;

			COleRef<IBuildDirManager> srpBDM;
			if (SUCCEEDED(theApp.FindInterface(IID_IBuildDirManager,
					(LPVOID*) &srpBDM)) &&
				srpBDM->FindFileOnPath(PLATFORM_CURRENT, DIRLIST_SOURCE,
					szOldFile, &lpszFullPath) == S_OK)
			{
				if (!path.Create(lpszFullPath) ||
					!gpISrc->ActivateTextDoc(path, pbpnCurLine(pBpNode) - 1))
				{
					MessageBeep(0);
				}
			}
			else
			{
				MsgText(strPrompt, IDS_ASKPATH_GETPATH, szOldFile, "");
				VERIFY(strTitle.LoadString(IDS_ASKPATH_CAPTION_SOURCE));
				dlgParam.lpParam = (LPSTR) (const char *) strPrompt;
				dlgParam.lpCaption = (LPSTR) (const char *) strTitle;

				if (FMapPath(dir,szNewFile,szOldFile,&dlgParam,FBreakpointFileExists,0))
				{
					if (!dir.CreateFromString(szNewFile) ||
						!path.CreateFromDirAndFilename(dir, szOldFile) ||
						!gpISrc->ActivateTextDoc(path, pbpnCurLine(pBpNode) - 1))
					{
						MessageBeep(0);
					}
				}
			}
			if (NULL != lpszFullPath)
				AfxFreeTaskMem(lpszFullPath);
		}
	}
}

/****************************************************************************

	FUNCTION:	BHCanGotoBP

	PURPOSE:	Return TRUE if it's possible to view the indicated
				breakpoint, FALSE if not.  It's only possible to view
				breakpoints which are either (a) nonvirtual or
				(b) file/line breakpoints.

****************************************************************************/

BOOL PASCAL BHCanGotoBP(PBREAKPOINTNODE pBpNode)
{
	BOOL	fCanGoto = FALSE;

	if (DebuggeeAlive() && !pbpnBPVirtual(pBpNode))
	{
		LPBPI	lpbpi = (LPBPI) LLLpvFromHlle(pbpnBPhbpi(pBpNode));

		fCanGoto = lpbpi->bpf.f.fBpCode && !lpbpi->bpf.f.fVirtual;
		UnlockHlle(pbpnBPhbpi(pBpNode));
	}
	else
	{
		fCanGoto = pbpnFileLineNode(pBpNode);
	}

	return fCanGoto;
}

// [CAVIAR #5459 11/27/92 v-natjm]
/**********************************************************************

	FUNCTION:	AddBreakpointNode

	PURPOSE:	Adds a breakpoint node to the breakpoint node list.
	INPUT  :    fReset : resets the ambiguous resolve status
				fQuiet : does not bring up a message box on failure
				fRestoring : we're restoring this BP from the status file
				lpiErr : the error to be updated

	RETURNS:	Pointer to added node if successful, NULL otherwise.

	NOTE:		Copies en masse the passed structure so caller
				must ensure all appropriate fields NULLed out.
				If a debuggee exists it also tries to set the cv400
				equivalent bp.  In this case the node is only added
				if this is successful too.

**********************************************************************/
PBREAKPOINTNODE PASCAL AddBreakpointNode(PBREAKPOINTNODE pbpnOriginal, BOOL fReset, BOOL fQuiet, BOOL fRestoring, LPINT lpiErr)
{
	PBREAKPOINTNODE pbpnNew;
	BOOL 			OkToAdd;

	pbpnNew = (PBREAKPOINTNODE)malloc(sizeof(BREAKPOINTNODE));

	if ( pbpnNew != NULL )
	{
		// Copy the data across
		*pbpnNew = *pbpnOriginal;

		// [CAVIAR #5459 11/25/92 v-natjm]
		// Make sure we initialize the ambiguous information but only
		// if it doesn't come from the Status File loading...
		if( fReset )
		{
			pbpnAmbigBP(pbpnNew) = FALSE ;
			pbpnBPTMindex(pbpnNew) = AMB_STS_UNRESOLVED ;
		}

		// Until VC5.0-6256 there was code here to upper-case filenames
		// pbpnLocCxtSource(pbpnNew), pbpnLocCxtExe(pbpnNew) and 
		// pbpnExprCxtExe(pbpnNew) (except for Java which skipped it).
		// Now we leave the case alone always. This is critical for SQL
		// debugging support [apennell]

		// If user wants to add a breakpoint which is of a type that is not
		// supported by the current platform, don't add it
		if ( !fRestoring &&pbpnEnabled(pbpnNew) && !BHFTypeSupported(pbpnType(pbpnNew)) )
		{
			OkToAdd = FALSE;

			if ( !fQuiet )
				MsgBox(Error, IDS_UnsupportedBpType);
		}
		else if ( DebuggeeAlive() && pbpnEnabled(pbpnNew) )
			OkToAdd = BHCreateBPHandle(pbpnNew,FALSE,fQuiet,lpiErr);
		else
			OkToAdd = TRUE;

		// If the group of the new node is non-zero, then the BP
		// has been instantiated by FInstantiateSourceBP
		if ( OkToAdd && pbpnBPGroup( pbpnNew ) == 0 )
		{
#ifdef DEBUGGER_AUTOMATION

			// If we have a valid collection, add this breakpoint to it
			if (pAutoBPS) 
			{
				pbpnNew->pAutoBP = new CAutoBreakpoint(pbpnNew);
			}
#endif			
			
			if ( LastBreakpointNode == NULL )
				FirstBreakpointNode = LastBreakpointNode = pbpnNew;
			else
			{
				pbpnNext(pbpnNew) = NULL;
				pbpnNext(LastBreakpointNode) = pbpnNew;
				LastBreakpointNode = pbpnNew;
			}
		}
		else
		{
			int	iGroup = pbpnBPGroup( pbpnNew );

			free((LPSTR)pbpnNew);

			pbpnNew = NULL;

			// If OkToAdd, search for added node as successful return
			// value!
			if ( OkToAdd )
			{
				PBREAKPOINTNODE	pbpnT = FirstBreakpointNode;

				ASSERT( FirstBreakpointNode );

				while(pbpnT && pbpnBPGroup(pbpnT) != iGroup )
					pbpnT = pbpnNext(pbpnT);

				ASSERT( pbpnT );
				pbpnNew = pbpnT;
			}
		}
	}

	return( pbpnNew );
}


/**********************************************************************

	FUNCTION: 	DeleteBreakpointNode (PBREAKPOINTNODE, PBREAKPOINTNODE)

	PURPOSE: 	Does actual work of deleting breakpoint, given node to
				delete and previous node in list.  (This was common code
				in the two versions of DBPN that follow, and it didn't
				make sense to keep modifying the two copies.)

	RETURNS:	TRUE

	NOTE:		This takes out any attached CV400 breakpoint too.

**********************************************************************/
BOOL PASCAL DeleteBreakpointNode(PBREAKPOINTNODE pPrev, PBREAKPOINTNODE pCur)
{
	// Extract pbpnCur
	if (pPrev == NULL)
	{
		// Deleting the first node
		FirstBreakpointNode = pbpnNext(pCur);
	}
	else
	{
		pbpnNext(pPrev) = pbpnNext(pCur);
	}

	if (pCur == LastBreakpointNode)
	{
		LastBreakpointNode = pPrev;
	}

	// [CAVIAR #5713 11/19/92 v-natjm]
	// Force the enable flag to make it disappear
	pbpnEnabled ( pCur ) = FALSE ;
	pbpnMarkDel ( pCur ) = TRUE;
	BFHShowBPNode ( pCur );

	// Is there a CV400 breakpoint attached?
	if (pbpnBPhbpi(pCur) != NULL)
	{
		AsyncBPDelete(pbpnBPhbpi(pCur));
	}

#ifdef DEBUGGER_AUTOMATION
	// Is there a debug automation object attached?
	// If so, mark the Auto obj as invalid
	if (pCur->pAutoBP) 
	{
		pCur->pAutoBP->BPBeGone();
	}
#endif

	free((LPSTR)pCur);

	return TRUE;
}


/**********************************************************************

	FUNCTION: 	DeleteBreakpointNode (PBREAKPOINTNODE)

	PURPOSE: 	Delete the breakpoint node from the breakpoint
				node list.

	RETURNS:	TRUE if deleted, FALSE otherwise.

	NOTE:		This takes out any attached CV400 breakpoint too.

**********************************************************************/
BOOL PASCAL DeleteBreakpointNode(PBREAKPOINTNODE pbpn)
{
	PBREAKPOINTNODE pbpnCur, PrevBreakpointNode;

	pbpnCur = BHFirstBPNode() ;
	PrevBreakpointNode = NULL;

	// Look for the breakpoint node:
	while (pbpnCur != NULL && pbpnCur != pbpn)
	{
		PrevBreakpointNode = pbpnCur;
		pbpnCur = pbpnNext(pbpnCur);
	}

	if (pbpnCur == NULL)
	{
		// Didn't find it
		return FALSE;
	}

	ASSERT (pbpnCur == pbpn);

	return DeleteBreakpointNode(PrevBreakpointNode, pbpnCur);
}


/**********************************************************************

	FUNCTION: 	DeleteBreakpointNode

	PURPOSE: 	Delete the nth breakpoint node from the breakpoint
				node list.

	RETURNS:	TRUE if deleted, FALSE otherwise.

	NOTE:		This takes out any attached CV400 breakpoint too.

**********************************************************************/
BOOL PASCAL DeleteBreakpointNode(int BreakpointNodeIndex)
{
	PBREAKPOINTNODE pbpnCur, PrevBreakpointNode;
	int CurIndex;

	CurIndex = 0;
	pbpnCur = BHFirstBPNode() ;
	PrevBreakpointNode = NULL;

	// Look for the breakpoint node:
	while ((CurIndex < BreakpointNodeIndex) && (pbpnCur != NULL))
	{
		CurIndex++;
		PrevBreakpointNode = pbpnCur;
		pbpnCur = pbpnNext(pbpnCur);
	}

	if ((pbpnCur == NULL) || (CurIndex != BreakpointNodeIndex))
	{
		if(pbpnCur == NULL)
		if(CurIndex != BreakpointNodeIndex) {
		   ;//AuxPrintf1("Index differ...") ;
		}
		// Didn't find it
		return FALSE;
	}

	return DeleteBreakpointNode(PrevBreakpointNode, pbpnCur);
}


/**********************************************************************

	FUNCTION: 	ClearCV400Breakpoints

	PURPOSE: 	Free the CV400 breakpoints attached to the current
				breakpoint node list.

	RETURNS:	Number of breakpoints deleted.

	NOTE:		For safety we call the CV400 clear routine after
				we have done our deleting.  NB there shouldn't be
				anything left to delete after we have deleted
				those CV400 bps attached to our list.

**********************************************************************/
int PASCAL ClearCV400Breakpoints(void)
{
	PBREAKPOINTNODE pbpnCur;
	int DeleteCount;

	pbpnCur = BHFirstBPNode() ;
	DeleteCount = 0;
	while (pbpnCur != NULL)
	{
		if (pbpnBPhbpi(pbpnCur) != NULL)
		{
			AsyncBPDelete(pbpnBPhbpi(pbpnCur));
			pbpnBPhbpi(pbpnCur) = NULL;
			pbpnBPGroup(pbpnCur) = 0;
			DeleteCount++;
		}

		pbpnCur = pbpnNext(pbpnCur);
	}

	iGroupMax = 0;

	//! This should become an ASSERT(BPFirstBPI()==NULL);
	{
		HBPI hbpiCur, hbpiThis;

		hbpiCur = BPFirstBPI();
		while (hbpiCur != NULL)
		{
			hbpiThis = hbpiCur;
			hbpiCur = LLHlleFindNext(llbpi, hbpiCur);

			AsyncBPDelete(hbpiThis);
		}
	}

	// The list is empty, destroy the list
	if ( llbpi )
	{
		LLChlleDestroyLl( llbpi );
		llbpi = (HLLI)NULL;
	}

	return DeleteCount;
}


/**********************************************************************

	FUNCTION: 	ClearBreakpointNodeList

	PURPOSE: 	Free the breakpoint node list, resetting global
				pointers.

**********************************************************************/
int PASCAL ClearBreakpointNodeList(void)
{
	PBREAKPOINTNODE	pbpn;
	int				cDeleted;

	cDeleted = 0;

	while( pbpn = FirstBreakpointNode )
	{
		// We shouldn't be freeing these when we still
		// have CV400 breakpoints allocated
		ASSERT( pbpnBPhbpi( pbpn ) == NULL );

		// [CAVIAR #5713 11/19/92 v-natjm]
		// Force the enable flag to make it disappear
		pbpnEnabled ( pbpn ) = FALSE ;
		pbpnMarkDel ( pbpn ) = TRUE ;
		BFHShowBPNode ( pbpn );

		FirstBreakpointNode = pbpnNext( pbpn );
		
#ifdef DEBUGGER_AUTOMATION
		// Is there a debug automation object attached?
		// If so, mark the Auto obj as invalid
		if (pbpn->pAutoBP) 
		{
			pbpn->pAutoBP->BPBeGone();
			// ASSERT(!pbpn->pAutoBP);
		}
#endif
		free((LPSTR)pbpn);
		++cDeleted;
	}

	ASSERT( FirstBreakpointNode == NULL );
	LastBreakpointNode = NULL;

	iGroupMax = 0;

	return cDeleted;
}

/**********************************************************************

	Implementation     : J.M. Nov 1992
	BHUnBindBPList()   : BREAKPTS.C

	Purpose    : Unbind our list of BP - delete all the hbpi
	Input      : fPerformingENC (true if Unbinding for Edit and Continue)				
	Returns    : None

**********************************************************************/
VOID PASCAL BHUnBindBPList(BOOL fPerformingENC)
{

	ASSERT (pDebugCurr);
	
	if ( pDebugCurr->IsQuickRestart ())
	{
		// we get no ModFrees for SQL, so we have to mark them as
		// virtual manually, else they won't get reinstantiated on a Restart
		PBREAKPOINTNODE pbpnCur = BHFirstBPNode();
		while (pbpnCur != NULL)
		{
			if (pbpnSqlBp(pbpnCur) && !pbpnBPVirtual(pbpnCur))
			{
				// Delete the OSDEBUG BP
				BPDelete(pbpnBPhbpi(pbpnCur));
				// Make it virtual now
				pbpnBPhbpi(pbpnCur) = (HBPI)NULL ;
				// Reset the group ID
				pbpnBPGroup(pbpnCur) = 0;
			}
			pbpnCur = pbpnNext(pbpnCur) ;
		}
		BPResetAllPassCnt();
	}
	else
	{
		PBREAKPOINTNODE pbpnCur;
		LPBPI lpbpi = (LPBPI)NULL ;
		HBPI hbpi = (HBPI)NULL ;
		HBPI hbpiDel = (HBPI)NULL ;

		pbpnCur = BHFirstBPNode() ;
		while (pbpnCur != NULL) {
			// Make sure there is an existing Bound hbpi ( hbpi != NULL )
			if(!pbpnBPVirtual(pbpnCur)) {
				// Extract handle form our structure
				hbpi = pbpnBPhbpi(pbpnCur) ;
				lpbpi = (LPBPI)LLLpvFromHlle(hbpi) ;
				if(lpbpi != (LPBPI)NULL) {
					// Don't try to replace the 0xCC by the opcode if
					// debugging is terminated... Disconnect cleans up
					// all the INT 3 in Windebug Emulator
					if(!DebuggeeAlive()) {
						lpbpi->bpf.f.fActive = FALSE ;
					}
				}
				// Unlock the handle
				UnlockHlle(hbpi);
				// Delete the OSDEBUG BP
				BPDelete(hbpi);
				// Make it virtual now
				pbpnBPhbpi(pbpnCur) = (HBPI)NULL ;

				// Reset the group ID
				pbpnBPGroup(pbpnCur) = 0;
			}
			pbpnCur = pbpnNext(pbpnCur) ;
		}

		// Reset the group id list
		iGroupMax = 0;

		// This code ensures no existing BP in the OSDEBUG layer
		// This is just my paranoia since we cannot have more hbpi than
		// BP ( always equal or less in case of Virtual BP's )
		// It should be something like ASSERT ( BPFirstBPI() == NULL )

		hbpi = BPFirstBPI();
		while(hbpi != (HBPI)NULL) {
			hbpiDel = hbpi ;
			hbpi = (HBPI)LLHlleFindNext(llbpi,hbpi) ;
			lpbpi = (LPBPI)LLLpvFromHlle(hbpiDel) ;

			// vc60 note: If unbinding BPs for Edit and Continue, then we
			// may have existing BPTMPENC BPs in the OSDEBUG layer. We
			// do not want to remove such BPs while the debuggee is alive. 
			if (fPerformingENC && 
				DebuggeeAlive() &&
				lpbpi && 
				lpbpi->bpf.f.BpType == BPTMPENC) {
				UnlockHlle(hbpiDel) ;
				continue;
			}

			if(lpbpi != (LPBPI)NULL) {
				if(!DebuggeeAlive()) {
					lpbpi->bpf.f.fActive = FALSE ;
				}
			}
			UnlockHlle(hbpiDel) ;
			BPDelete(hbpiDel) ;
		}
	}
}

/**********************************************************************

	Implementation     : M.M. Jan 1994
	BHUnBindBPList()   : BREAKPTS.C

	Purpose    : Remember the names of all DLLs on which the user
				 currently has breakpoints which are enabled.  This
				 is so that when the user restarts, he won't get
				 error messages about our being unable to set those
				 breakpoints.

				 All we're going to do is add these DLLs to the
				 project's "Additional DLLs" field, so that symbolic
				 information will be loaded for these DLLs as soon
				 as the EXE itself is loaded.

	Input      : None
	Returns    : None

**********************************************************************/
VOID PASCAL BHRememberBPDLLs(VOID)
{
	PBREAKPOINTNODE pbpn;

	for (pbpn = BHFirstBPNode(); pbpn; pbpn = pbpnNext(pbpn))
	{
		// NYI
	}
}

// [CAVIAR #5522 11/17/92 v-natjm]
/**********************************************************************

	Implementation     : J.M. Nov 1992
	BHBindBPList()     : BREAKPTS.C

	Purpose    : Bind our list of BP to some valid hbpi
	Input      : None
	Returns    : TRUE if we could set all the BP's

**********************************************************************/
BOOL PASCAL BHBindBPList(VOID)
{
	PBREAKPOINTNODE pbpnCur;
	int 			iRet;
	BOOL 			fSet = TRUE;
	BOOL 			fEnable;
	int 			CurLine;	// BP line for a LINE BP
	BOOL 			fBPMoved = FALSE;
	BOOL			fBPUnsupported = FALSE;
	BOOL            fBPNotSet = FALSE;
//	BOOL 			fEnabled;
	PBREAKPOINTNODE	pbpnLast = LastBreakpointNode;
	PBREAKPOINTNODE pbpnPrev = (PBREAKPOINTNODE)NULL;
	int				iNode = 0;

	pbpnCur = BHFirstBPNode();
	while( pbpnCur != NULL )
	{
		BOOL            fSetBreakpoint = TRUE;

		// If it is a Line BP, clear the line status
		// because the line number can change when the CV400 BP will be set
		if (pbpnFileLineNode(pbpnCur))
		{
			CurLine = pbpnCurLine(pbpnCur);
			// Temporarily pretend the breakpoint is disabled, so that we
			// can un-highlight the line.
			// fEnabled = pbpnEnabled(pbpnCur);
			// pbpnEnabled(pbpnCur) = FALSE;
			pbpnMarkDel(pbpnCur) = TRUE;
			BFHShowBPNode(pbpnCur);
			// pbpnEnabled(pbpnCur) = fEnabled;
			pbpnMarkDel(pbpnCur) = FALSE;

			// Scan the source lines to see if the
			// source line number needs to be adjusted
			if ( CCanInstantiateSourceBP( pbpnCur, NULL ) &&
				CurLine != pbpnCurLine( pbpnCur ) )
			{
				// The line have changed, set a flag
				fBPMoved = TRUE;
			}
		}

		// [CAVIAR #6390 01/04/93 v-natjm]
		fEnable = pbpnEnabled(pbpnCur);

		if (BHFTypeSupported(pbpnType(pbpnCur)))
		{
			if (FIsSqlInproc())
			{
				// if it is not a new or SQL breakpoint
				// then don't try to instantiate it when doing inproc
				// debugging.
				if (!pbpnSqlBp(pbpnCur))
				{
					if (pbpnEbpt(pbpnCur) != BPUNKNOWN ||
					   (pbpnType(pbpnCur) == BPLOC &&
						pbpnLocType(pbpnCur) == BPLOCADDREXPR))
					{
						fSetBreakpoint = FALSE;
					}
				}
			}

			// Attempt to create the breakpoint
			if (fSetBreakpoint && !BHCreateBPHandle(pbpnCur, TRUE, TRUE, &iRet))
			{
				if (fEnable)
				{
					fSet = FALSE;
					fBPNotSet = TRUE;
				}
			}

			if (fSetBreakpoint && !pbpnBPGroup(pbpnCur))
			{
				// Check for bound breakpoint only...
				if(!pbpnBPVirtual(pbpnCur))
				{
					// If BP is supposed to be disabled do it now
					if(!pbpnEnabled(pbpnCur))
					{
						BPDisableFar(pbpnBPhbpi(pbpnCur)) ;
					}
					// else BP is already physically existing
				}
				else
				{
					// Type is supported but we couldn't set bp, force disable
					pbpnEnabled(pbpnCur) = FALSE ;
				}

				// Highlight now depending on MarkEnable
				BFHShowBPNode(pbpnCur);
			}
		}
		else
		{
			// If we haven't told them before, set up flags to indiate
			// that we will tell them
			if ( fEnable && !fBPUnsupported && !pbpnWarnedUnsupported(pbpnCur) )
			{
				fSet = FALSE;
				fBPUnsupported = TRUE;
			}

			// Always set the state to warned.  If we've already warned
			// them about this breakpoint, then this should already be
			// set.  If not, we will be warning them about this one and
			// we don't want to tell them again
			pbpnWarnedUnsupported( pbpnCur ) = TRUE;
		}

		pbpnPrev = pbpnCur;
		pbpnCur = pbpnNext(pbpnCur);

		if ( pbpnMarkDel( pbpnPrev ) )
		{
			DeleteBreakpointNode( pbpnPrev );
		}

		++iNode;
	}

	// Update the UI BPs might have been changed
//    PBREAKPOINTNODE pbpn = BHFirstBPNode ();
//    while (pbpn != NULL)
//    {
//        BFHShowBPNode (pbpn);
//        pbpn = pbpnNext (pbpn);
//    }

	// If some BP have been moved, warn the user
	if (fBPMoved)
	{
		ErrorBox(ERR_Breakpoint_Moved);
	}

	// If some BPs' haven't been set warn the user.
	if (fBPNotSet)
		ErrorBox((lpprcCurr && lpprcCurr->stp == stpStopped) ? 
			ERR_Multiple_BP_ENC : ERR_Multiple_Breakpoints);

	// If some BPs' types are unsupported, warn the user
	if (fBPUnsupported)
		ErrorBox(IDS_UnsupportedBpTypes);

	return fSet;
}

/**********************************************************************

	Implementation     : J.M. Nov 1992
	BHFirstBPNode()    : BREAKPTS.C

	Purpose    : Returns first BP in our list
	Input      : None
	Returns    : Pointer or NULL

**********************************************************************/
PBREAKPOINTNODE PASCAL BHFirstBPNode(VOID)
{
	return FirstBreakpointNode ;
}

/**********************************************************************

	Implementation     : J.M. Nov 1992
	BHLastBPNode()     : BREAKPTS.C

	Purpose    : Returns Last BP in our list
	Input      : None
	Returns    : Pointer or NULL

**********************************************************************/
PBREAKPOINTNODE PASCAL BHLastBPNode(VOID)
{
	return LastBreakpointNode ;
}

// [CAVIAR #5459 11/27/92 v-natjm]
/**********************************************************************

	Implementation     : J.M. Nov 1992
	BHLastBPCreated()  : BREAKPTS.C

	Purpose    : Returns Last BP currently created ( AMBIG.C)
	Input      : None
	Returns    : Pointer on BP node or NULL

**********************************************************************/
PBREAKPOINTNODE PASCAL BHLastBPCreated(VOID)
{
	return LastBPCreated ;
}

/**********************************************************************

	Implementation       : J.M. Nov 1992
	BFHGetLineBPIndex() : BREAKPTS.C

	Purpose    : Get a BP index in list for specified file and line
				if fCheckDisabled, in addtion return the WidgetFlag status
	Input      : LPSTR File name
				 INT Line number
				 LPINT the node index to be updated
	Returns    : The count of BP's for this line

**********************************************************************/
INT PASCAL BFHGetLineBPIndex(
	LPSTR File,
	INT Line,
	LPINT pNodeIndex,
	BOOL  fCheckDisabled,
	UINT  *pWidgetFlag)
{
	PBREAKPOINTNODE pbpnCur;
	int idx;
	int nbBP;
	CDocument   *pTextDoc = NULL;

	BOOL fDisable = FALSE;
	BOOL fEnable = FALSE;
	BFHFindDoc( File, &pTextDoc );

	pbpnCur = BHFirstBPNode() ;
	idx = 0;
	nbBP = 0;

	// Skip over to the nth item
	while( pbpnCur != NULL && *pNodeIndex > idx )
	{
		--*pNodeIndex;
		++idx;
		pbpnCur = pbpnNext( pbpnCur );
	}

	*pNodeIndex = 0;
	while (pbpnCur != NULL)
	{
		if (pbpnFileLineNode(pbpnCur) )
		{
			if (pbpnCurLine(pbpnCur) == Line)
			{
				CDocument   *pDocBP = NULL;

				// Match file name OR if document of BP matches
				// document of file being searched
				if (_ftcsicmp(pbpnFileName(pbpnCur), File) == 0 ||
					( BFHFindDoc( pbpnFileName( pbpnCur ), &pDocBP ) &&
						pDocBP == pTextDoc))
				{
					// Found one
					if ( fCheckDisabled )
					{
						if ( pbpnEnabled( pbpnCur ) )
						{
							fEnable = TRUE;
							*pWidgetFlag = BRKPOINT_LINE;
						}
						else
						{
							fDisable = TRUE;
							*pWidgetFlag = DISABLED_BRKPOINT_LINE;
						}
						if ( fEnable && fDisable )
							*pWidgetFlag = MIXED_BRKPOINT_LINE;

					}

					if(++nbBP == 1) {
						*pNodeIndex = idx;
					}
				}
			}
		}
		pbpnCur = pbpnNext(pbpnCur);
		idx++;
	}

	return nbBP ;
}


/****************************************************************************

	FUNCTION: 	AdjustBreakpointLines

	PURPOSE: 	Updates source/line breakpoint nodes when lines are
				added/deleted to a file in the editor.  If Added is
				TRUE the lines have been added otherwise they've been
				deleted.

	NOTE:		This is called from the editor every time a block is
				added or deleted.
				Insertions are always performed BEFORE the StartLine.
				Deletions are always performed INCLUDING the StartLine.
				StartLine is passed 0 relative.

****************************************************************************/
void PASCAL AdjustBreakpointLines(const CString &strDocFileName, int StartLine, int NumberLines, BOOL fAdded, BOOL fLineBreak)
{
	PBREAKPOINTNODE CurBreakpoint;
	int bpIndex;
	BOOL bAdjustDisplay = FALSE;

	if (strDocFileName.IsEmpty()) // not a real doc
		return;

	CDocument * pTextDoc;
	BFHFindDoc(strDocFileName, &pTextDoc);

	StartLine++; //adjust from 0-based editor line #s to 1-based bp line #s

	CurBreakpoint = BHFirstBPNode() ;
	bpIndex = 0;
	while (CurBreakpoint != NULL)
	{
		if (pbpnFileLineNode(CurBreakpoint))
		{
			// Firstly, we are only affected if the passed StartLine
			// is less than or equal to the breakpoints line number
			if (StartLine <= pbpnCurLine(CurBreakpoint))
			{
				// Is the passed file the one for this breakpoint
				if (strDocFileName.CompareNoCase(pbpnFileName(CurBreakpoint)) == 0)
				{
					// DevStudio96 2396 [paulde] patch glyphs in editor
					// clear line status on original line
					gpISrc->SetLineStatus(pTextDoc, StartLine, HAS_BRKPOINT, LINESTATUS_OFF, FALSE, FALSE);
					// set flag so we know to reset bp line status at the end of this loop
					bAdjustDisplay = TRUE;

					// Adjust the current line accordingly
					if (fAdded)
					{
						// If the added lines are before the bp line, or
						//   at the bp line and we split the bp line, then
						//   add NumberLines
						if (StartLine < pbpnCurLine(CurBreakpoint) || fLineBreak)
							pbpnCurLine(CurBreakpoint) += NumberLines;
					}
					else // Deleted
					{
						// If we're beyond the deletion block then move the breakpoint.
						if ((pbpnCurLine(CurBreakpoint) > (StartLine + NumberLines - 1)))
						{
							// Subtract NumberLines
							pbpnCurLine(CurBreakpoint) -= NumberLines;
						}
						// we're inside the deletion block, or we're deleting the ENTIRE bp line
						else if (!(pbpnCurLine(CurBreakpoint) == StartLine && !fLineBreak))
						{
							// The breakpoint line itself is being deleted, so we 
							// lose the breakpoint.
							// Increment here, as we lose our place straight after
							CurBreakpoint = pbpnNext(CurBreakpoint);
							VERIFY(DeleteBreakpointNode(bpIndex));
							// We have already incremented CurBreakpoint
							// and we don't want bpIndex to be incremented
							// so...
							continue;
						}
					}
					ASSERT(pbpnCurLine(CurBreakpoint) > 0);
				}
			}
		}

		CurBreakpoint = pbpnNext(CurBreakpoint);
		bpIndex++;
	}

	// DevStudio96 2396 [paulde] patch glyphs in editor
	if (bAdjustDisplay)
	{
		SetDebugLines(pTextDoc, TRUE, TRUE);
	}
}

// As a replacement for RestoreBreakpointLinesInDoc
// we use the following:
/****************************************************************************

	FUNCTION: 	DeleteBreakpointLinesInDoc

	PURPOSE: 	Runs through the breakpoint node list and deletes
				breakpoints that can't be restored (those marked with a -1
				initial line number).  This should
				be called when a document has been updated, the	user
				closes it but doesn't save it.

	NOTE:		This means that source/line breakpoints are lost if
				you put a breakpoint on a modified file, then close the
				file and refuse to save it.

****************************************************************************/
void PASCAL DeleteBreakpointLinesInDoc(const CString &strDocFileName)
{
	PBREAKPOINTNODE CurBreakpoint;
	int bpIndex;

	CurBreakpoint = BHFirstBPNode() ;
	bpIndex = 0;
	while (CurBreakpoint != NULL)
	{
		if (pbpnFileLineNode(CurBreakpoint))
		{
			// This is a source error - is it for us?
			if (strDocFileName.CompareNoCase(pbpnFileName(CurBreakpoint)) == 0)
			{
				if( pbpnInitLine(CurBreakpoint) != -1 )
				{
					pbpnCurLine(CurBreakpoint) = pbpnInitLine(CurBreakpoint);
				}
				else
				{
					// can't restore original line - delete it
					CurBreakpoint = pbpnNext(CurBreakpoint);
					VERIFY(DeleteBreakpointNode(bpIndex));

					// We have already incremented CurBreakpoint
					// and we don't want bpIndex to be incremented
					// so...
					continue;
				}
			}
		}

		CurBreakpoint = pbpnNext(CurBreakpoint);
		bpIndex++;
	}
}

/****************************************************************************

	FUNCTION: 	CementBreakpointLinesInDoc

	PURPOSE: 	Runs through the breakpoint node list and marks all
				non-restorable breakpoints (those with initial line set to
				-1) as restorable (by setting initial line = current line).
				This should be called when a file is saved.

****************************************************************************/
void PASCAL CementBreakpointLinesInDoc(const CString &strDocFileName)
{
	PBREAKPOINTNODE CurBreakpoint;
	int bpIndex;

	CurBreakpoint = BHFirstBPNode() ;
	bpIndex = 0;
	while (CurBreakpoint != NULL)
	{
		if (pbpnFileLineNode(CurBreakpoint))
		{
			// This is a source error - is it for us?
			if (strDocFileName.CompareNoCase(pbpnFileName(CurBreakpoint)) == 0)
			{
				pbpnInitLine(CurBreakpoint) = pbpnCurLine(CurBreakpoint);
			}
		}

		CurBreakpoint = pbpnNext(CurBreakpoint);
		bpIndex++;
	}
}

/**********************************************************************

	FUNCTION:	CheckExpression

	PURPOSE:	Takes an expression in string form and validates
				it by running it through the EE.

	RETURNS:	TRUE if expression parses successfully, FALSE
				otherwise.

**********************************************************************/
BOOL PASCAL CheckExpression(LPCTSTR Expr, int Radix, int Case)
{
	HTM hTM = (HTM)NULL;
	ushort status;
	ushort strIndex;

	// The disassembly window sets the input radix to hex and resets to
	// decimal.  Fix for DOLPHIN:2954

#ifdef ONLY_DECIMAL
	// We've decided the radix will always be decimal for input, but I'm going
	// to leave the argument to this function in case we change our minds later.
	ASSERT(Radix == 10);
	Radix = 10;
#endif	// ONLY_DECIMAL

	status = pEXF ? EEParse(Expr, Radix, Case, (PHTM)&hTM, &strIndex) : EENOERROR;
	if (hTM)
	{
		EEFreeTM (&hTM);
	}


	return (status == EENOERROR);
}


/****************************************************************************

	FUNCTION:   BuildCV400Context()

	PURPOSE:    Builds a CV400 BP context from the passed BREAKPOINTNODE.

	NOTE:		ToOem excludes the possibility of ToLower to avoid
				screw ups with toupper/tolower/toupper not yielding
				the same result.

****************************************************************************/
void NEAR PASCAL BuildCV400Context(
	PBPCONTEXT pContext,
	PSTR Cxt, UINT Len,
	BOOL FullPaths, BOOL ToLower, BOOL ToOem)
{
	int Len1, Len2, Len3;
	char szSource[_MAX_PATH], szExe[_MAX_PATH];
	BOOL fSourceNeedsQuotes, fExeNeedsQuotes;
	CString strFormat;

	if ((*(pContext->Function) =='\0') &&
		 (*(pContext->Source) =='\0') &&
		 (*(pContext->Exe) =='\0'))
	{
		// Don't build null contexts
		*Cxt = '\0';
		return;
	}

	fSourceNeedsQuotes = FFilenameNeedsQuotes(pContext->Source);
	fExeNeedsQuotes = FFilenameNeedsQuotes(pContext->Exe);

	if (FullPaths)
	{
		_ftcscpy(szSource, pContext->Source);
		_ftcscpy(szExe, pContext->Exe);
	}
	else
	{
		if (*(pContext->Source) == '\0')
		{
			szSource[0] = '\0';
		}
		else
		{
			_ftcscpy(szPath, pContext->Source);
			*szFName = '\0';
			*szExt = '\0';
			_splitpath(szPath, szDrive, szDir, szFName, szExt);
			_makepath(szSource, szNull, szNull, szFName, szExt);
		}

		if (*(pContext->Exe) == '\0')
		{
			szExe[0] = '\0';
		}
		else
		{
			_ftcscpy(szPath, pContext->Exe);
			*szFName = '\0';
			*szExt = '\0';
			_splitpath(szPath, szDrive, szDir, szFName, szExt);
			_makepath(szExe, szNull, szNull, szFName, szExt);
		}
	}

	// Calculate the lengths for the component parts
	Len1 = (int)Len-4;
	Len2 = max((int)(Len1-_ftcslen(pContext->Function)), 0);
	Len3 = max((int)(Len2-_ftcslen(szSource)), 0);

	if (ToOem)
	{
	}
	else
	{
		if (ToLower)
		{
			// leave the case alone always (important for SQL) so commented
			// this out [apennell]
//			AnsiLower(szSource);
//			// For Java, don't change the case of the EXE (class) name
//			if (!pDebugCurr || (pDebugCurr && !pDebugCurr->IsJava()))
//			{
//				AnsiLower(szExe);
//			}
		}
	}

	// Here is the format string we are building, with the possible addition of
	// quotes around the source and/or exe names: "{%.*Fs,%.*s,%.*s}"
	strFormat = "{%.*Fs,";

	if (fSourceNeedsQuotes)
	{
		strFormat += "\"%.*s\",";
	}
	else
	{
		strFormat += "%.*s,";
	}

	if (fExeNeedsQuotes)
	{
		strFormat += "\"%.*s\"}";
	}
	else
	{
		strFormat += "%.*s}";
	}

	sprintf(Cxt, strFormat,
		Len1, pContext->Function,
		Len2, szSource,
		Len3, szExe);
}


/****************************************************************************

	FUNCTION:   BuildCV400Location()

	PURPOSE:    Builds a CV400 BP at location specifier

****************************************************************************/
void PASCAL BuildCV400Location(
	PBREAKPOINTNODE pBreakpoint,
	PSTR LocSpec, UINT Len,
	BOOL FullPaths, BOOL ToLower, BOOL ToOem,
	BOOL Quote)
{
	char szBuffer[cbBpCmdMax];
	LPSTR LocPtr;
	char szLine[20];
	UINT Len1, Len2;

	// First, put the context together:
	BuildCV400Context(&pbpnLocCxt(pBreakpoint),
		szBuffer, sizeof(szBuffer)-1, FullPaths, ToLower, ToOem);

	// Append a space at the end for convenience
	Len1 = _ftcslen(szBuffer);
	if (Len1 > 0)
	{
		szBuffer[Len1] = ' ';
		szBuffer[Len1+1] = '\0';
	}

	// Now, what sort of location have we got
	switch (pbpnType(pBreakpoint))
	{
		case BPLOC:
		case BPLOCEXPRTRUE:
		case BPLOCEXPRCHGD:
			if (pbpnLocType(pBreakpoint) == BPLOCADDREXPR)
			{
				// We're adding the address expression to the context
				LocPtr = pbpnAddrExpr(pBreakpoint);
			}
			else
			{
				ASSERT(	pbpnLocType(pBreakpoint) == BPLOCFILELINE );

				// We're adding a .nnnn to the context
				if ( IsNonLocalDecimalPoint () )
				{
					// the dot in the line header for a breakpoint
					// doesn't work when a non-localized decimal point
					// such as a comma is being used.
					*szLine = BP_LINELEADER_OTHER;
				}
				else
					*szLine = BP_LINELEADER;

				_itoa(pbpnCurLine(pBreakpoint), szLine+1, 10);
				LocPtr = szLine;
			}
			break;

		case BPWNDPROCMSGRCVD:
			// We're adding the WndProc to the context
			LocPtr = pbpnWndProc(pBreakpoint);
			break;

		default:
			ASSERT(FALSE);
	}

	// Now, append the location to the context (the space is for
	// appending expressions and only appears if Quote is TRUE)

	if (Quote)
	{
		Len1 = (int)Len-4;
		Len2 = max((int)(Len1-_ftcslen(LocPtr)), 0);

		sprintf(LocSpec, "\"%.*s%.*Fs\" ",
			Len1, szBuffer,
			Len2, LocPtr);
	}
	else
	{
		Len1 = (int)Len-1;
		Len2 = max((int)(Len1-_ftcslen(LocPtr)), 0);

		sprintf(LocSpec, "%.*s%.*Fs",
			Len1, szBuffer,
			Len2, LocPtr);
	}
}

/****************************************************************************

	FUNCTION:   BuildCV400LocationMessage()

	PURPOSE:    Builds a CV400 BP at location, (WndProc), when message
				received specifier.

****************************************************************************/
void NEAR PASCAL BuildCV400LocationMessage(
	PBREAKPOINTNODE pbpn,
	PSTR LocSpec, UINT Len,
	BOOL FullPaths, BOOL ToLower, BOOL ToOem)
{
	char szBuffer[100], * pBuf = LocSpec;
	PSTR psz;

	ASSERT(sizeof(szBuffer) <= Len);
	ASSERT(pbpnType(pbpn) == BPWNDPROCMSGRCVD);

	// Leave space for 30 chars for the message specs
	BuildCV400Location(pbpn, LocSpec, Len-30,
		FullPaths, ToLower, ToOem,
		TRUE);

	// Add the message switch at the end
	_ftcscat(LocSpec, "/M");
	if (pbpnMessageClass(pbpn) == msgSingleOnly)
	{
		// Load the string form of the message
		VERIFY(LoadString(hInst, DBG_Msgs_Start+pbpnMessageNum(pbpn),
			szBuffer, sizeof(szBuffer)));
	}
	else
	{
		// This is VERY TIED to BPParse (brkpt0.c).  In there
		// is an rgchClass[] which ASSumes that the message
		// "classes" are in the order defined in qcqp.h.  If
		// you change any of those values or add a new one,
		// you MUST change BPParse to recognize the class
		psz = szBuffer;
		if (pbpnMessageClass(pbpn) & msgMaskMouse)
		{
			*psz = 'm';
			psz++;
		}
		if (pbpnMessageClass(pbpn) & msgMaskWin)
		{
			*psz = 'w';
			psz++;
		}
		if (pbpnMessageClass(pbpn) & msgMaskInput)
		{
			*psz = 'n';
			psz++;
		}
		if (pbpnMessageClass(pbpn) & msgMaskSys)
		{
			*psz = 's';
			psz++;
		}
		if (pbpnMessageClass(pbpn) & msgMaskInit)
		{
			*psz = 'i';
			psz++;
		}
		if (pbpnMessageClass(pbpn) & msgMaskClip)
		{
			*psz = 'c';
			psz++;
		}
		if (pbpnMessageClass(pbpn) & msgMaskDDE)
		{
			*psz = 'd';
			psz++;
		}
		if (pbpnMessageClass(pbpn) & msgMaskNC)
		{
			*psz = 'z';
			psz++;
		}
		*psz = '\0';
	}

	_ftcscat(LocSpec, szBuffer);
}


/****************************************************************************

	FUNCTION:   BuildCV400Expression()

	PURPOSE:    Builds a complete expression string from the context
				and the expression.

****************************************************************************/
void PASCAL BuildCV400Expression(
	PBREAKPOINTNODE pbpn,
	PSTR ExprSpec, UINT Len,
	BOOL FullPaths, BOOL ToLower, BOOL ToOem)
{
	char szCxt[cbBpCmdMax];
	char szExpr[MAX_EXPRESS_SIZE];
	LPSTR lpszExpr;
	UINT Len1, Len2;

	*szCxt = '\0';
	if (pbpnExprUseCxt(pbpn))
	{
		// First, put the context together:
		BuildCV400Context(&pbpnExprCxt(pbpn),
			szCxt, sizeof(szCxt)-1, FullPaths, ToLower, ToOem);
	}

	Len1 = _ftcslen(szCxt);
	if (Len1 > 0)
	{
		// If we have a context we format like {,,}(expr) so
		// bracket the expression here
		szExpr[0] = '(';
		_ftcsncpy(szExpr+1, pbpnExpression(pbpn), sizeof(szExpr)-3);
		szExpr[sizeof(szExpr)-3] = '\0';
		Len1 = _ftcslen(szExpr);
		szExpr[Len1] = ')';
		szExpr[Len1+1] = '\0';
		lpszExpr = (LPSTR)szExpr;

	}
	else
	{
		// ...otherwise we just use the expression
		lpszExpr = pbpnExpression(pbpn);
	}

	// Append expression to context specifier
	Len1 = (int)Len-1;
	Len2 = max((int)(Len1-_ftcslen(lpszExpr)), 0);

	sprintf(ExprSpec, "%.*s%.*Fs", Len1, szCxt, Len2, lpszExpr);
}


/****************************************************************************

	FUNCTION:   BuildCV400ExpressionTrue()

	PURPOSE:    Builds a CV400 BP when expression true specifier

****************************************************************************/
void NEAR PASCAL BuildCV400ExpressionTrue(
	PBREAKPOINTNODE pbpn,
	PSTR ExprSpec, UINT Len,
	BOOL FullPaths, BOOL ToLower, BOOL ToOem)
{
	char szBuffer[cbBpCmdMax];

	// Build the complete expression
	BuildCV400Expression(pbpn, szBuffer, Len,
		FullPaths, ToLower, ToOem);

	// sprintf this into a specifier
	sprintf(ExprSpec, "?\"%.*s\"", Len-4, szBuffer);
}


/****************************************************************************

	FUNCTION:   BuildCV400ExpressionChgd()

	PURPOSE:    Builds a CV400 BP when expression changed specifier

****************************************************************************/
void NEAR PASCAL BuildCV400ExpressionChgd(
	PBREAKPOINTNODE pbpn,
	PSTR ExprSpec, UINT Len,
	BOOL FullPaths, BOOL ToLower, BOOL ToOem)
{
	char szBuffer[cbBpCmdMax];

	// Build the complete expression
	BuildCV400Expression(pbpn, szBuffer, Len,
		FullPaths, ToLower, ToOem);

	// sprintf this into a specifier
	sprintf(ExprSpec, "=\"%.*s\" /R%u",
		Len-12, szBuffer, pbpnExprLen(pbpn));
}


/****************************************************************************

	FUNCTION:   MakeCV400BPCmd()

	PURPOSE:    Takes a QCQP breakpoint node and builds a
				corresponding CV400 BP command string.

****************************************************************************/
void PASCAL MakeCV400BPCmd(
	PBREAKPOINTNODE pbpn,
	PSTR CV400BPCmd, UINT CmdLen)
{
	char szBuffer[cbBpCmdMax];

	ASSERT(sizeof(szBuffer) <= CmdLen);

	switch (pbpnType(pbpn))
	{
		case BPLOC:
			BuildCV400Location(pbpn, CV400BPCmd, CmdLen,
				TRUE, FALSE, TRUE, TRUE);
			break;

		case BPLOCEXPRTRUE:
			BuildCV400Location(pbpn, CV400BPCmd, CmdLen,
				TRUE, FALSE, TRUE, TRUE);
			BuildCV400ExpressionTrue(pbpn, szBuffer, sizeof(szBuffer),
				TRUE, FALSE, TRUE);

			// Now concatenate them
			_ftcsncat(CV400BPCmd, szBuffer, CmdLen-_ftcslen(szBuffer)-1);
			szBuffer[CmdLen-1] = '\0';
			break;

		case BPLOCEXPRCHGD:
			BuildCV400Location(pbpn, CV400BPCmd, CmdLen,
				TRUE, FALSE, TRUE, TRUE);
			BuildCV400ExpressionChgd(pbpn, szBuffer, sizeof(szBuffer),
				TRUE, FALSE, TRUE);

			// Now concatenate them
			_ftcsncat(CV400BPCmd, szBuffer, CmdLen-_ftcslen(szBuffer)-1);
			szBuffer[CmdLen-1] = '\0';
			break;

		case BPEXPRTRUE:
			BuildCV400ExpressionTrue(pbpn, CV400BPCmd, CmdLen,
				TRUE, FALSE, TRUE);
			break;

		case BPEXPRCHGD:
			BuildCV400ExpressionChgd(pbpn, CV400BPCmd, CmdLen,
				TRUE, FALSE, TRUE);
			break;

		case BPWNDPROCMSGRCVD:
			BuildCV400LocationMessage(pbpn,	CV400BPCmd, CmdLen,
				TRUE, FALSE, TRUE);
			break;
	}

	// Pass count
	if ( pbpnPassCount( pbpn ) )
	{
		sprintf( szBuffer, " /P%u", pbpnPassCount( pbpn ) );
		_ftcsncat( CV400BPCmd, szBuffer, CmdLen - _ftcslen( szBuffer ) - 1 );
	}
}


#define MAX_TMNAME	256
// [CAVIAR #5459 11/27/92 v-natjm]
/****************************************************************************

	Implementation     : J.M. Nov 1992
	BHFormatAmbigBP()  : BREAKPTS.C

	Purpose    : Formats a BP that requires extended info ( ambiguous BP )
	Input      : Display string and size
	Returns    : None
****************************************************************************/
VOID PASCAL BHFormatAmbigBP(
	PBREAKPOINTNODE pbpn,
	PSTR DisplayBP,
	UINT DisplayLen)
{
	EBPT BpSym = BPUNKNOWN;
	HBPI hbpi ;

	if(hbpi = pbpnBPhbpi(pbpn)) {
		BPGetBpCmd( hbpi, EECODE, &BpSym, DisplayBP , FALSE );
	}
}

/****************************************************************************

	FUNCTION:   FormatDisplayBP()

	PURPOSE:    Takes a QCQP breakpoint node and formats a
				string corresponding to the node.

	NOTE:       Currently just does the same as MakeCV400BPCmd
				but asks for filenames only in the context.

****************************************************************************/
void PASCAL FormatDisplayBP(
	PBREAKPOINTNODE pbpn,
	PSTR DisplayBP, UINT DisplayLen)
{
	char	sz[cbBpCmdMax];
	char	szLoc[cbBpCmdMax];
	char	szExpr[cbBpCmdMax];
	char	szPass[ 50 ];
	TCHAR *	ptch;
	CString	str;

	// Initialize to empty strings
	*szLoc = *szExpr = *szPass = '\0';

	// Location type breakpoint, get the location text
	if ( pbpnLocationNode( pbpn ) || pbpnType( pbpn ) == BPWNDPROCMSGRCVD )
	{
		if( pbpnBPExtInfo(pbpn) && pbpnBPhbpi(pbpn))
		{
			BHFormatAmbigBP( pbpn, sz, sizeof( sz ) );
		}
		else
		{
			BuildCV400Location(
				pbpn,
				sz,
				sizeof( sz ),
				FALSE,
				TRUE,
				FALSE,
				FALSE
			);
		}

		str.LoadString( BPFMT_LOCATION );
		sprintf( szLoc, (TCHAR const *)str, sz );
	}

	// Data type breakpoint, get the expression text
	if ( pbpnExpressionNode( pbpn ) )
	{
		BuildCV400Expression(
			pbpn,
			sz,
			sizeof( sz ),
			FALSE,
			TRUE,
			FALSE
		);

		// Break when loc changes, include "changes" in the text
		if ( pbpnType( pbpn ) == BPEXPRCHGD || pbpnType( pbpn ) == BPLOCEXPRCHGD )
		{
			char bufLen[20];
			str.LoadString( BPFMT_WHENCHANGES );
			sprintf( bufLen, "%d", pbpnExprLen( pbpn ) );
			sprintf( szExpr, str, sz, bufLen );
		}
		else
		{
			str.LoadString( BPFMT_WHENTRUE );
			sprintf( szExpr, (TCHAR const *)str, sz );
		}
	}

	// WndProc breakpoint, include the message/class to break on
	else if ( pbpnType( pbpn ) == BPWNDPROCMSGRCVD )
	{
		BuildCV400LocationMessage(
			pbpn,	
			sz,
			sizeof( sz ),
			FALSE,
			TRUE,
			FALSE
		);

		// BuildCV400LocationMessage formats text as WndProc /MWM_FOO, only
		// care about the WM_FOO at this point, scan for the /M and skip over
		ptch = _tcschr( sz, '/' );
		ASSERT( ptch );
		ASSERT( *( ptch + 1 ) == 'M' );

		str.LoadString( BPFMT_ONMESSAGE );
		sprintf( szExpr, (TCHAR const *)str, ptch + 2 );
	}

	// If this node has pass counts, display ", skip XXX time(s) (YYY remaining)"
	if ( pbpnPassCount( pbpn ) )
	{
		if ( DebuggeeAlive() )
		{
			USHORT	cPassCur;

			if ( pbpnBPhbpi( pbpn ) )
			{
				HBPI	hbpi = pbpnBPhbpi( pbpn );
				LPBPI	lpbpi = (LPBPI)LLLpvFromHlle( hbpi );

				cPassCur = lpbpi->cPassCur;

				UnlockHlle( hbpi );
			}
			else
			{
				 cPassCur = pbpnPassCount( pbpn );
			}

			char bufPass[20];
			char bufRem[20];
			sprintf( bufPass, "%d", pbpnPassCount( pbpn ) );
			sprintf( bufRem, "%d", cPassCur );
			str.LoadString( BPFMT_PASSCOUNTREMAIN );
			sprintf( szPass, str, bufPass, bufRem);
		}
		else
		{
			char bufPass[20];
			sprintf( bufPass, "%d", pbpnPassCount( pbpn ) );
			str.LoadString( BPFMT_PASSCOUNT );
			sprintf(szPass, str, bufPass );
		}
	}

	// Put the pieces together
	_tcsncpy( DisplayBP, szLoc, DisplayLen );
	_tcsncat( DisplayBP, szExpr, DisplayLen - _tcslen( DisplayBP ) );

	// For pass counts, remove the trailing spaces so the pass count
	// expression (with leading comma) will look good
	ptch = _tcsdec( DisplayBP, DisplayBP + _tcslen( DisplayBP ) );

	while( _istspace( *ptch ) )
	{
		*ptch = _TCHAR( '\0' );
		ptch = _tcsdec( DisplayBP, ptch );
	}

	// Now append the pass count text
	_tcsncat( DisplayBP, szPass, DisplayLen - _tcslen( DisplayBP ) );

}

/****************************************************************************

	Implementation     : J.M. Nov 1992
	BFHShowCodeBP      : BREAKPTS.C

	Purpose    : Shows a BP in source if it has a code address
	Input      : pbpn       : an IDE BP node
	Returns    : TRUE / FALSE if set or not

****************************************************************************/
BOOL PASCAL BFHShowCodeBP(PBREAKPOINTNODE pbpn)
{
	char	    szFile[_MAX_PATH];
	ILINE	    wLine;
	CDocument   *pTextDoc;
	HSF 	    hsf;
	LPBPI 	    lpbpi;
	ADDR 	    addr;

	// Make a few checkings...
	if( !pbpn || pbpnBPVirtual( pbpn ) )
	{
		return FALSE ;
	}

	lpbpi = (LPBPI)LLLpvFromHlle( pbpnBPhbpi( pbpn ) );
	addr = lpbpi->CodeAddr ;
	UnlockHlle( pbpnBPhbpi( pbpn ) );

	// Get the file name from the CV info
	if ( GetSourceFrompADDR(&addr,szFile,sizeof(szFile),(LONG *)&wLine,NULL,&hsf ) )
	{
		// Extended document search
		if ( BFHFindDoc( szFile, &pTextDoc ) )
		{

			LINESTATUSACTION action = LINESTATUS_OFF;
			UINT			 state	= 0;

			gpISrc->SetHSF(pTextDoc, hsf);

			if ( pbpnMarkDel( pbpn ) )
			{
				// Breakpoint was Removed

				state = BRKPOINT_LINE;
				action = LINESTATUS_OFF;
			}
			else
			{
				LPSLP	lpslp;
				LPSLP	rgslp;
				int 	cslp;
				int 	islp;

				// See how many source lines there are from this address
				cslp = SLCAddrFromLine((HEXE)NULL,(HMOD)NULL,szFile,(USHORT)wLine,&rgslp);

				// If any of the source lines are enabled, then
				// show the breakpoint
				for(islp = 0, lpslp = rgslp;islp < cslp;++islp, ++lpslp )
				{
					HBPI	hbpi;
					UINT	uiWidgetFlag;

#if defined (_MIPS_) || defined (_ALPHA_)
					if (!MassageBPAddr(&lpslp->addr, &lpslp->cb))
						continue;
#endif
					// If there is a breakpoint at this location AND it's
					// not this bp (it may be disabled in the BREAKPOINTNODE
					// struct, but not in the BPI struct
					if ( hbpi = BPIsAddr(
									&lpslp->addr,
									lpslp->cb,
									0,
									TRUE,
									CHECK_DISABLED_BREAKPOINTS,
									&uiWidgetFlag ) )
					{
						state |= uiWidgetFlag;
						action = LINESTATUS_ON;
					}
				}

				if ( rgslp )
				{
					ASSERT( rgslp );
					SHFree( (LPV)rgslp );
				}
			}

			// Cleanup the previous line state
			switch ( state )
			{
				case DISABLED_BRKPOINT_LINE:
					gpISrc->SetLineStatus(pTextDoc,wLine,MIXED_BRKPOINT_LINE,LINESTATUS_OFF,FALSE,TRUE);
					gpISrc->SetLineStatus(pTextDoc,wLine,BRKPOINT_LINE,LINESTATUS_OFF,FALSE,TRUE);
					break;
				case BRKPOINT_LINE:
					gpISrc->SetLineStatus(pTextDoc,wLine,MIXED_BRKPOINT_LINE,LINESTATUS_OFF,FALSE,TRUE);
					gpISrc->SetLineStatus(pTextDoc,wLine,DISABLED_BRKPOINT_LINE,LINESTATUS_OFF,FALSE,TRUE);
					break;
				default:
					state = MIXED_BRKPOINT_LINE;
					gpISrc->SetLineStatus(pTextDoc,wLine,DISABLED_BRKPOINT_LINE,LINESTATUS_OFF,FALSE,TRUE);
					gpISrc->SetLineStatus(pTextDoc,wLine,BRKPOINT_LINE,LINESTATUS_OFF,FALSE,TRUE);
					break;
			}

			// Set the line status to be on or off!
			if (pbpnLocType(pbpn)==BPLOCFILELINE)			// if set by line, use that
				wLine = pbpnCurLine(pbpn);					// else use that calculated above

			gpISrc->SetLineStatus(pTextDoc,wLine,state,action,FALSE,TRUE);

			return TRUE;
		}
	}

	return FALSE;
}

/****************************************************************************

	Implementation     : J.M. Nov 1992
	BFHShowBPNode()    : BREAKPTS.C

	Purpose    : Shows a BP from our list if displayable
	Input      : pbpn       : an IDE BP node
	Returns    : TRUE / FALSE if we could show it or not

****************************************************************************/
BOOL PASCAL BFHShowBPNode(PBREAKPOINTNODE pbpn )
{
	if ( !pbpnBPVirtual( pbpn ) && DebuggeeAlive() )
	{
		if ( pbpnLocationNode( pbpn ) )
		{
			return BFHShowCodeBP( pbpn );
		}
	}
	else
	{
	// Even without a debuggee we can (de)highlight file/line location bps
		if ( pbpnFileLineNode( pbpn ) )
		{
			CDocument           *pTextDoc;
			LINESTATUSACTION    action = LINESTATUS_OFF;
			UINT                state  = 0;

			if (BFHFindDoc(pbpnFileName(pbpn), &pTextDoc))
			{
				// Loop through all breakpoints and see if ANY are enabled
				PBREAKPOINTNODE	pbpnT = FirstBreakpointNode;

				if ( pbpnMarkDel( pbpn ) )
				{
					state = BRKPOINT_LINE;
				}
				else
				{
					while( pbpnT )
					{
						CDocument   *pDocBP = NULL;

						// Match file name OR if document of BP matches
						// document of file being searched
						if ( ( ( _ftcsicmp( pbpnFileName( pbpnT ),
									pbpnFileName ( pbpn )  ) == 0  ) ||
							( BFHFindDoc( pbpnFileName( pbpnT ),
										&pDocBP  ) &&
							pDocBP == pTextDoc ) ) &&
							( pbpnCurLine( pbpn ) == pbpnCurLine( pbpnT ) ) )
						{

							if ( pbpnEnabled( pbpnT ) )
							{
								if ( pbpnFileLineNode( pbpnT ) )
								{
									state |= BRKPOINT_LINE;
									action = LINESTATUS_ON;
								}
							}
							else
							{
								// See if node is a source line node
								if ( pbpnFileLineNode( pbpnT ) )
								{
									state |= DISABLED_BRKPOINT_LINE;
									action = LINESTATUS_ON;
								}
							}
						}

						// Next node!
						pbpnT = pbpnNext( pbpnT );
					}
				}

				switch ( state )
				{
					case DISABLED_BRKPOINT_LINE:
						gpISrc->SetLineStatus(pTextDoc,pbpnCurLine(pbpn),MIXED_BRKPOINT_LINE,LINESTATUS_OFF,FALSE,TRUE);
						gpISrc->SetLineStatus(pTextDoc,pbpnCurLine(pbpn),BRKPOINT_LINE,LINESTATUS_OFF,FALSE,TRUE);
						break;
					case BRKPOINT_LINE:
						gpISrc->SetLineStatus(pTextDoc,pbpnCurLine(pbpn),MIXED_BRKPOINT_LINE,LINESTATUS_OFF,FALSE,TRUE);
						gpISrc->SetLineStatus(pTextDoc,pbpnCurLine(pbpn),DISABLED_BRKPOINT_LINE,LINESTATUS_OFF,FALSE,TRUE);
						break;
					default:
						state = MIXED_BRKPOINT_LINE;
						gpISrc->SetLineStatus(pTextDoc,pbpnCurLine(pbpn),BRKPOINT_LINE,LINESTATUS_OFF,FALSE,TRUE);
						gpISrc->SetLineStatus(pTextDoc,pbpnCurLine(pbpn),DISABLED_BRKPOINT_LINE,LINESTATUS_OFF,FALSE,TRUE);
						break;
				}

				if ( action == LINESTATUS_OFF ) {
					gpISrc->SetLineStatus(pTextDoc,pbpnCurLine(pbpn),MIXED_BRKPOINT_LINE,LINESTATUS_OFF,FALSE,TRUE);
					gpISrc->SetLineStatus(pTextDoc,pbpnCurLine(pbpn),DISABLED_BRKPOINT_LINE,LINESTATUS_OFF,FALSE,TRUE);
					state = BRKPOINT_LINE;
				}

				// Finally, set the status of the line!
				gpISrc->SetLineStatus(pTextDoc,pbpnCurLine(pbpn),state,action,FALSE,TRUE);
				return TRUE;
			}
		}
	}

	return FALSE;
}


/****************************************************************************

	Implementation     : J.M. Nov 1992
	BFHShowAllBPNode() : BREAKPTS.C

	Purpose    : Shows all BP's from our list if displayable
	Input      : None
	Returns    : None

****************************************************************************/
VOID PASCAL BFHShowAllBPNode(VOID)
{
	PBREAKPOINTNODE pbpn;

	pbpn = BHFirstBPNode();
	while (pbpn != NULL)
	{
		BFHShowBPNode(pbpn);
		pbpn = pbpnNext(pbpn);
	}

	if ( DebuggeeAlive() )
	{
		// Should change this to SendMessage(,WU_BREAKPTCHANGE,).  Current
		// implementation determines the hiliting so forcing the repaint is
		// sufficient to update the display (at least for DISASSY and CALLS
		// _WINs)
		if ( pViewCalls )
			pViewCalls->InvalidateRect((LPRECT)NULL,FALSE);

		if ( pViewDisassy )
			pViewDisassy->InvalidateRect((LPRECT)NULL,FALSE);
	}
}

/****************************************************************************

	Implementation          : J.M. Nov 1992
	BFHResetShowAllBPNode() : BREAKPTS.C

	Purpose    : When debugging terminates, we only want to show the
				 breakpoints that are referenced to a file and line
				 and dehighlight all the others ( Code addr BP's )
	Input      : None
	Returns    : None

****************************************************************************/
VOID PASCAL BFHResetShowAllBPNode(VOID)
{
	// We don't keep the BP type information for each line
	// We could be more clever by having a fast link between each line
	// and the attached BP...
	gpISrc->ClearAllDocStatus(BRKPOINT_LINE|DISABLED_BRKPOINT_LINE|MIXED_BRKPOINT_LINE);

	// ReHighlight now all the BP nodes that are referenced to a file
	// and line. We don't do that for BP code addr ( Sym + Abs address )
	BFHShowAllBPNode();
}


/****************************************************************************

	FUNCTION:   SetEditorLocBP()

	PURPOSE:    Changes the breakpoint at the current editor line.

	ARGUMENTS:  BPOpt specifies one of set, clear or toggle.
				ppbpn if non NULL has the pointer to the first bpn added.
				line specifies where to place the breakpoint (one base, -1 
				for current line)

	RETURNS:	TRUE if successful, FALSE otherwise.

****************************************************************************/
BOOL PASCAL SetEditorLocBP (BPSetOpt bpOpt, PBREAKPOINTNODE *ppbpn /*= NULL*/, 
		int iLine /*= -1 */)
{
	BREAKPOINTNODE 	bpnCur;
	PBREAKPOINTNODE pbpnNew;
	char 			szCurLine[20];
	int 			NodeIndex;
	int 			nbBP ;
	HBPI			hbpi;


	CView   *pCurTextView = gpISrc->GetCurTextView();
	if (pCurTextView == NULL)
		return FALSE;

	CDocument   *pCurTextDoc = (CDocument *)pCurTextView->GetDocument();

	const CString &strCurViewFileName = GetDebugPathName(pCurTextDoc);

	// Set up a breakpoint node and a command string for the
	// current line
	if (strCurViewFileName.IsEmpty() )
	{
		return FALSE;
	}
	
	// Make a current line BP command
	if ( IsNonLocalDecimalPoint () )
	{
		// the dot in the line header for a breakpoint
		// doesn't work when a non-localized decimal point
		// such as a comma is being used.
		*szCurLine = BP_LINELEADER_OTHER;
	}
	else
		*szCurLine = BP_LINELEADER;

	if (iLine == -1) 
	{
		iLine = gpISrc->GetCurrentLine(pCurTextView) + 1;
	}
	
	_itoa(iLine, szCurLine+1, 10);
	memset(&bpnCur, 0, sizeof(bpnCur));
	if (ParseCV400Location(szCurLine, &bpnCur))
	{
		// We've got a good node so set the type...
		bpnType(bpnCur) = BPLOC;
	}
	else
	{
		return FALSE;
	}

	if ( DebuggeeAlive() )
	{
		int 	cslp;
		int		islp;
		LPSLP	rgslp;
		LPSLP	lpslp;
		BOOL	fFound = FALSE;

		// If the initline is -1, then the file has been
		// edited, don't modify the curline
		if ( bpnInitLine( bpnCur ) != -1 ) {
			bpnCurLine( bpnCur ) = bpnInitLine( bpnCur );
		}

		// Make sure that there is code!
		if ( !CCanInstantiateSourceBP( &bpnCur, NULL ) )
		{
			return FALSE;
		}

		cslp = SLCAddrFromLine((HEXE)NULL,(HMOD)NULL,(char *)(const char *)strCurViewFileName,bpnCurLine( bpnCur ),&rgslp);

		// Pass zero -- fixup for MIPS and Alpha
#if defined (_MIPS_) || defined (_ALPHA_)
		for( islp = 0, lpslp = rgslp; islp < cslp; ++islp, ++lpslp ) {
			if (!MassageBPAddr(&lpslp->addr, &lpslp->cb)) {
				cslp--;
				if (islp != cslp) {
					_fmemmove(lpslp, lpslp+1, (cslp-islp)*sizeof(rgslp[0]));
					islp--;
					lpslp--;
				}
			}
		}
#endif
		// Pass one, see if the address is in the list
		for( islp = 0, lpslp = rgslp; !fFound && islp < cslp; ++islp, ++lpslp )
		{
			hbpi = BPIsAddr( &lpslp->addr, lpslp->cb, 0, TRUE );
			fFound = !!hbpi;
		}

		// Pass two, if found, loop through and remove all matches
		lpslp = rgslp;

		// return first breakpoint at this location
		if (bpOpt == bplfind) 
		{
			PBREAKPOINTNODE pbpn;

			if (!fFound)
				return FALSE;

			pbpn = BHFirstBPNode();
			while( pbpn )
			{
				if ( pbpnBPhbpi( pbpn ) == hbpi )
				{
					if (ppbpn) 
						*ppbpn = pbpn;
					return TRUE;
				}
				pbpn = pbpnNext( pbpn );
			}
			ASSERT(FALSE);
			return FALSE;
		}
		// make sure that this is not a mixed BP for the new behavior. 
		if ( (fFound && 
            ((bpOpt == bpltoggle || bpOpt == bplclear))) && (!(GetBPLineState() & MIXED_BRKPOINT_LINE)))
		{
			for( islp = 0, lpslp = rgslp; islp < cslp; ++islp, ++lpslp )
			{

				// If there's a breakpoint at this location, we need
				// to free it.  Go through the list and removed ALL
				// instances of this breakpoint.  We probably shouldn't
				// include break at location when XXX in this code.
				if ( hbpi = BPIsAddr( &lpslp->addr, lpslp->cb, 0, TRUE ) )
				{
					do
					{
						int 			iNode = 0;
						PBREAKPOINTNODE pbpn;


						pbpn = BHFirstBPNode();
						while( pbpn )
						{
							if ( pbpnBPhbpi( pbpn ) == hbpi )
							{
								pbpnEnabled( pbpn ) = FALSE;
								BFHShowBPNode( pbpn );
								DeleteBreakpointNode( iNode );
								break;
							}
							pbpn = pbpnNext( pbpn );
							++iNode;
						}
					} while( hbpi = BPIsAddr( &lpslp->addr, 1, 0, TRUE ) );
				}
			}
			return TRUE;
		}
		else if (!fFound && bpOpt == bplclear)
        {
            // Clear bps when none exist, everything is fine.
            return TRUE;
        }
		else if ( (((!fFound && bpOpt == bpltoggle) || bpOpt == bplset)) ||
					(fFound && ((bpOpt == bpltoggle || bpOpt == bplclear))) && (GetBPLineState() & MIXED_BRKPOINT_LINE) )

		{
			BOOL fFoundLOCBP = FALSE;

			for( islp = 0, lpslp = rgslp; islp < cslp; ++islp, ++lpslp )
			{
			
				PBREAKPOINTNODE pbpn;
				
				pbpn = BHFirstBPNode();
				while( pbpn )
				{
					if ( pbpnLocationNode(pbpn) && !pbpnEnabled( pbpn ) && pbpnBPhbpi(pbpn) )
					{
						LPBPI lpbpiCur = (LPBPI)LLLpvFromHlle(pbpnBPhbpi(pbpn));
						if( ( GetAddrSeg ( lpbpiCur->CodeAddr ) == GetAddrSeg ( lpslp->addr ) )  &&
							// check the offset range
							GetAddrOff( lpslp->addr ) <= GetAddrOff( lpbpiCur->CodeAddr ) &&
							GetAddrOff( lpbpiCur->CodeAddr ) <  (GetAddrOff (lpslp->addr) + lpslp->cb)    ) 
						{
							BPEnableFar ( pbpnBPhbpi( pbpn ) );
							pbpnEnabled ( pbpn ) = TRUE ;
							fFoundLOCBP = TRUE;
							if (ppbpn) 
								*ppbpn = pbpn;
						}
					}
					pbpn = pbpnNext( pbpn );
				}
			}
		
			if (!fFoundLOCBP)
			{
				int iRet ;

				bpnEnabled ( bpnCur ) = TRUE;

				if ((pbpnNew=AddBreakpointNode(&bpnCur,TRUE,TRUE,FALSE,&iRet)) != NULL)
				{
					gpISrc->SetLineStatus(pCurTextDoc, bpnCurLine( bpnCur ), BRKPOINT_LINE, LINESTATUS_ON,FALSE,TRUE);

					if (ppbpn != NULL)
						*ppbpn = pbpnNew;

					return TRUE;
				}
			}
			else
				return TRUE;
		}
        else {
			// We should have covered all the cases in the previous 
			// if.. else if clauses
			ASSERT(FALSE);
		}

		if ( rgslp )
			SHFree( (LPV)rgslp );
	}
	else
	{
		// Wow! Watch out for the next line.  We really can't cast a CString any
		// further than LPCTSTR.  Getting a real LPTSTR requires calling GetBuffer...
		CString str = GetDebugPathName(pCurTextView->GetDocument());
		LPTSTR	lptstr = (LPTSTR)(LPCTSTR)str;
		UINT    uiState;

		// No current debuggee so we search the
		// breakpoint node list for a bp node set for the
		// current line in the current file.
		if ( (( nbBP = BFHGetLineBPIndex(
						lptstr,
						iLine,
						&( NodeIndex = 0 ),
						CHECK_DISABLED_BREAKPOINTS,
						&uiState ) ) > 0 ) &&
				bpOpt != bplset)
		{
			// We don't have a debuggee active, we do have a breakpoint on this 
			// line, and we aren't trying to set another breakpoint.
			if (bpOpt == bplfind) 
			{
				PBREAKPOINTNODE pbpn;

				pbpn = BHFirstBPNode();
				while( pbpn )
				{
					if (!(NodeIndex--))
					{
						if (ppbpn) 
							*ppbpn = pbpn;
						return TRUE;
					}
					pbpn = pbpnNext( pbpn );
				}
				// We wouldn't enter this case unless we had found a breakpoint here
				ASSERT(FALSE);
				return FALSE;
			}
			if ( uiState & BRKPOINT_LINE || bpOpt == bplclear)
			{
				// All breakpoints on the line are enabled
				//  delete them all
				do
				{
					DeleteBreakpointNode(NodeIndex);
				} while( ( nbBP = BFHGetLineBPIndex(lptstr,iLine,&( NodeIndex = 0 ) ) ) > 0);
			}
			else
			if ( (uiState & DISABLED_BRKPOINT_LINE) || (uiState & MIXED_BRKPOINT_LINE) )
			{
				// Enable all BPLOC breakpoints.

				PBREAKPOINTNODE pbpn;
				CDocument       *pDocBP = NULL;

				pbpn = BHFirstBPNode();
				while( pbpn )
				{
					if ( pbpnCurLine( pbpn ) == bpnCurLine ( bpnCur ) &&
						BFHFindDoc( pbpnFileName( pbpn ), &pDocBP ) &&
						pDocBP == pCurTextDoc )
                    {
                        if ( !pbpnEnabled( pbpn ) )
                        {
                            bpnEnabled ( *pbpn ) = TRUE;
	                    }
					}
					pbpn = pbpnNext( pbpn );
				}
		   }

			return TRUE;
		} 
		else
		{
			int iRet ;

			if (bpOpt == bplfind) 
			{
				return FALSE;
			}
			
			bpnEnabled ( bpnCur ) = TRUE;
			if ((pbpnNew=AddBreakpointNode(&bpnCur, TRUE, TRUE, FALSE, &iRet))!= NULL)
			{
				gpISrc->SetLineStatus(pCurTextDoc,iLine,BRKPOINT_LINE,LINESTATUS_ON,FALSE,TRUE);

				if (ppbpn != NULL)
					*ppbpn = pbpnNew;

				return TRUE;
			}
		}
	}

	return FALSE;
}

/****************************************************************************
	FUNCTION:	BHFIsValidWindowForBP(void)

	PURPOSE:	Returns TRUE if the current view is one which can have
				breakpoints set.
				Currently: DOC_WIN, CALLS_WIN, DISASSY_WIN, and BREAKPT_WIN
				Provided for button disabling [marklam]

	RETURNS:	TRUE if successful, FALSE otherwise.

****************************************************************************/
BOOL BHFIsValidWindowForBP(void)
{
	int  iAttrs;

	if (FIsActiveProjectDebuggable( NULL, NULL, NULL, NULL ))
	{
		BOOL  fRet = IsFindComboActive();

		CView   *pCurIDEView = GetCurView();

		if ( !fRet && pCurIDEView )
		{
			DOCTYPE     dt;

			if ( gpISrc->ViewBelongsToSrc(pCurIDEView) )
				dt = DOC_WIN;
			else if ( pCurIDEView->IsKindOf(RUNTIME_CLASS(CMultiEdit)) )
				dt = ((CMultiEdit *)pCurIDEView)->m_dt;
			else
				dt = MEMORY_WIN; // Just to get around switch statement.

			switch ( dt )
			{
				case DOC_WIN:
				case CALLS_WIN:
				case DISASSY_WIN:
				case BREAKPT_WIN:
				{
					CWnd * pWndActive = CWnd::GetActiveWindow();

					fRet = !( pWndActive && pWndActive->IsKindOf(RUNTIME_CLASS(CFloatingFrameWnd)) );
					break;
				}

				default:
				   break;
			}
		}

		if( pDebugCurr && !(pDebugCurr->MtrcAsync() & asyncBP) )
		{
			if(DebuggeeAlive() && DebuggeeRunning())
			{
				// ErrorBox(ERR_No_Async_BPs);
				return( FALSE );
			}
		}

		return fRet;
	}

	return( FALSE );
}

/****************************************************************************

	FUNCTION:   ToggleLocBP()

	PURPOSE:	If the ribbon edit box or class view is active, sets (that's
				SETS, not toggles!) a breakpoint at the location specified in
				the edit box.  Otherwise, toggles the breakpoint at the current
				line.  Only valid for DOC, DISASSY, BREAKPT, and CALLSTACK
				windows

	RETURNS:	TRUE if successful, FALSE otherwise.

****************************************************************************/
BOOL PASCAL ToggleLocBP ( void )
{
	BOOL	    fRet = FALSE;
	LPCLASSVIEW pInterface;

	// FUTURE: this is kind of a hack to solve the problem that UpdateUI can't
	// take the time to call into class view to see if it has the focus on a
	// valid item, so class view always enables the toggle bp msg and here
	// we are.
	if (DebuggeeAlive() && DebuggeeRunning())
	{
		ASSERT(pDebugCurr);

		if (pDebugCurr == NULL || !(pDebugCurr->MtrcAsync() & asyncBP))
			return FALSE;
	}

	CView   *pCurIDEView;
	BOOL	bHandled = TRUE;

	// If the ribbon edit box is active, set a breakpoint at the specified
	// location.
	if (IsFindComboActive())
	{
		GCAINFO     gcai = {0};

		SendMessage(GetFocus(), WM_GETTEXT, sizeof(gcai.szExpr), (LPARAM)gcai.szExpr);

		extern BOOL PASCAL SetSmartBreakpoint( LPSTR );

		fRet = SetSmartBreakpoint(gcai.szExpr);

		if (fRet)
		{
			LONG	lResult;

			// restore focus to whatever window had it before
			// FUTURE: BUG??: this doesn't do it right if a dockable window
			// had focus!
			DkPreHandleMessage(GetFocus(), WM_KEYDOWN, VK_ESCAPE, 0, &lResult);
		}
	}
	else if ((pCurIDEView = GetCurView()) != NULL )
	{
		DOCTYPE     dt;

		if ( gpISrc->ViewBelongsToSrc(pCurIDEView) )
			dt = DOC_WIN;
		else if ( pCurIDEView->IsKindOf(RUNTIME_CLASS(CMultiEdit)) )
			dt = ((CMultiEdit *)pCurIDEView)->m_dt;
		else
			dt = MEMORY_WIN; // Just to get around switch statement.

		// Support toggle for specific windows only
		//		Currently: DOC_WIN, CALLS_WIN, DISASSY_WIN, and BREAKPT_WIN
		//		See also fIsValidWindowForBP
		switch( dt )
		{
			case DOC_WIN:
				fRet = SetEditorLocBP(bpltoggle);
				break;

			case CALLS_WIN:
			case DISASSY_WIN:
			case BREAKPT_WIN:
			{
				GCAINFO	gcai = {0};

				// Get the address for where the cursor is.  The window
				// must be sensitive to this message.  A non-zero return
				// value indicates that the addr packet has been filled in.
				// it is GROSSLY ASSUMED that it IS valid!
				if ( (LRESULT)gcafAddress == pCurIDEView->SendMessage(WU_GETCURSORADDR, 0, (DWORD)&gcai ) )
				{
#if defined (_MIPS_) || defined (_ALPHA_)	// Dolphin 13040
					MassageBPAddr(&gcai.addr, 0);
#endif

					HBPI	hbpi = BPIsAddr( &gcai.addr, 1, 0, TRUE );

					// If there's a breakpoint at this location, we need
					// to free it.  Go through the list and removed ALL
					// instances of this breakpoint.  We probably shouldn't
					// include break at location when XXX in this code.
					if ( hbpi )
					{
						int 			iNode = 0;
						PBREAKPOINTNODE pbpn;

						do
						{
							pbpn = BHFirstBPNode();

							// Reset iNode since we are starting
							// back at the first node in the list
							iNode = 0;

							while( pbpn )
							{
								if ( pbpnBPhbpi( pbpn ) == hbpi )
								{
									pbpnEnabled( pbpn ) = FALSE;
									BFHShowBPNode( pbpn );
									DeleteBreakpointNode( iNode );
									fRet = TRUE;
									break;
								}

								pbpn = pbpnNext( pbpn );
								++iNode;
							}
						} while( hbpi = BPIsAddr( &gcai.addr, 1, 0, TRUE ) );
					}
					else
					{
						// No breakpoint at the specified location.  Create one!
						char			sz[1023];
						BREAKPOINTNODE	bpn;

						// Fix up the address, format it into something
						// that the EE/BP parser can handle and set the
						// breakpoint
						memset( &bpn, 0, sizeof( bpn ) );
						SYFixupAddr( &gcai.addr );

						if (pDebugCurr && pDebugCurr->IsJava()) {

							*sz = _T('\0');

							// Java: offset-based bps are set using the following
							// format: class name.method name + offset

							ASSERT(modeAddr(gcai.addr).fJava == 1);

							// get the nearest HSYM (method)
							HSYM hsymMethod = NULL;
							UOFFSET offAddr = SHGetNearestHsym(&(gcai.addr), emiAddr(gcai.addr), EECODE, &hsymMethod);
							ASSERT(offAddr < CV_MAXOFFSET && hsymMethod != NULL);
							if (offAddr < CV_MAXOFFSET && hsymMethod != NULL) {

								CXT cxtMethod;
								SHSetCxt(&(gcai.addr), &cxtMethod);
								// get the name from the HSYM
								TCHAR szMethod[1023];
								FnNameFromHsym(hsymMethod, &cxtMethod, szMethod);

								_stprintf(sz, _T("%s + 0x%08lx"), szMethod, offAddr);
							}
						}
						else {
							// get the EE to convert to an address
							// C++ just dumps a hex value, SQL does "{,spname,}.line"
							EEFormatAddress( &gcai.addr, sz );
						}

						if ( ParseCV400Location ( sz, &bpn ) )
						{
							// We've got a good node so set the type...
							bpnType(bpn) = BPLOC;
							bpnEnabled( bpn ) = TRUE;
							if (AddBreakpointNode(&bpn, TRUE, TRUE, FALSE, NULL))
								fRet = TRUE;
						}
					}
				}
			}
				break;

			default:
				bHandled = FALSE;
		}
	}
	else
		bHandled = FALSE;
	
	if (!bHandled)
	{
		// Is class view active?
		if ((SUCCEEDED(theApp.FindInterface(IID_IClassView, (LPVOID FAR *)&pInterface))) &&
			pInterface != NULL)
		{
			SZ sz;
			TYP typ;
			ATR attr;

			fRet = (pInterface->GetCurrentSelInfo(&sz, &typ, &attr) == S_OK);
			pInterface->Release();

			if (fRet)
			{
				extern BOOL PASCAL SetSmartBreakpoint( LPSTR );
				fRet = SetSmartBreakpoint( sz );
			}
		}
	}

	// If we were successful in toggling the breakpoint, make sure that ALL windows
	// which display code get updated accordingly
	if ( fRet )
		BFHShowAllBPNode();

	return( fRet );
}

/****************************************************************************

	FUNCTION:   GetBPLineState

	PURPOSE:    Return the line state for the current line

	RETURNS:	UINT line state

****************************************************************************/
UINT GetBPLineState(void)
{
	CView       *pCurView = GetCurView();
	CDocument   *pDoc = NULL;
	UINT        uiState = 0;

	if ( pCurView )
		pDoc = pCurView->GetDocument();

	DOCTYPE     dt;

	if ( gpISrc->ViewBelongsToSrc(pCurView) )
		dt = DOC_WIN;
	else if ( pCurView->IsKindOf(RUNTIME_CLASS(CMultiEdit)) )
		dt = ((CMultiEdit *)pCurView)->m_dt;
	else
		dt = MEMORY_WIN; // Just to get around switch statement.

	switch ( dt )
	{
		case DOC_WIN:
		{
			INT  iLine = gpISrc->GetCurrentLine(pCurView);

			ASSERT(pDoc != NULL);
			// check if there is a breakpoint on the line
			uiState =  gpISrc->GetLStat(pDoc, iLine);
		}
		break;

		case CALLS_WIN:
		case DISASSY_WIN:
		case BREAKPT_WIN:
		{
			GCAINFO	gcai = {0};

			// Get the address for where the cursor is.  The window
			// must be sensitive to this message.  A non-zero return
			// value indicates that the addr packet has been filled in.
			// it is GROSSLY ASSUMED that it IS valid!
			if ( (LRESULT)gcafAddress == pCurView->SendMessage(WU_GETCURSORADDR,0, (DWORD)&gcai ) )
			{
#if defined (_MIPS_) || defined (_ALPHA_)	// Dolphin 13040
				MassageBPAddr(&gcai.addr, 0);
#endif

				HBPI	hbpi = BPIsAddr(
								&gcai.addr,
								1,
								0,
								TRUE,
								CHECK_DISABLED_BREAKPOINTS,
								&uiState );

			}
		}
		break;
	}

	return ( uiState );
}

/****************************************************************************

	FUNCTION:   DisableAllBreakpoints()

	PURPOSE:    Disables All Breakpoints

	RETURNS:	

****************************************************************************/
VOID PASCAL DisableAllBreakpoints ( VOID ) {

	PBREAKPOINTNODE pbpn = BHFirstBPNode();

	while( pbpn )
	{
		if ( pbpnBPhbpi( pbpn ) != NULL )
		{
			BPDisableFar ( pbpnBPhbpi( pbpn ) );
		}
		pbpnEnabled ( pbpn ) = FALSE ;
		pbpn = pbpnNext( pbpn );
	}

	// change the glyphs
	pbpn = BHFirstBPNode ();
	while (pbpn != NULL)
	{
		BFHShowBPNode (pbpn);
		pbpn = pbpnNext (pbpn);
	}

#if 0
	HBPI hbpiCur, hbpiThis;

	hbpiCur = BPFirstBPI();
	while ( hbpiCur != NULL )
	{
		hbpiThis = hbpiCur;
		hbpiCur = LLHlleFindNext( llbpi, hbpiCur );

		BPDisableFar ( hbpiThis );
	}
#endif
}

/****************************************************************************
	FUNCTION:   EnableDisableEditorLocBP()

	PURPOSE:    Enables All Breakpoints disabled  at the current line
				when fEnable is TRUE, otherwise Disables all Breakpoints
				at the current line.

	RETURNS:	

****************************************************************************/
VOID PASCAL EnableDisableEditorLocBP ( BOOL fEnable )
{
	BREAKPOINTNODE 	bpnCur;
	char 			szCurLine[20];


	CView   *pCurTextView = GetCurView();

	ASSERT(pCurTextView != NULL);

	CDocument   *pCurTextDoc = pCurTextView->GetDocument();

	const CString &strCurViewFileName = GetDebugPathName(pCurTextDoc);

	// Set up a breakpoint node and a command string for the
	// current line
	if ((gpISrc->ViewBelongsToSrc(pCurTextView) ) && !strCurViewFileName.IsEmpty())
	{
		// Make a current line BP command
		if ( IsNonLocalDecimalPoint () )
		{
			// the dot in the line header for a breakpoint
			// doesn't work when a non-localized decimal point
			// such as a comma is being used.
			*szCurLine = BP_LINELEADER_OTHER;
		}
		else
			*szCurLine = BP_LINELEADER;

		_itoa(gpISrc->GetCurrentLine(pCurTextView)+1, szCurLine+1, 10);

		memset(&bpnCur, 0, sizeof(bpnCur));

		if (ParseCV400Location(szCurLine, &bpnCur))
		{
			// We've got a good node so set the type...
			bpnType(bpnCur) = BPLOC;
		}
		else
			return;
	}
	else
		return;

	if ( DebuggeeAlive() )
	{
		// If the initline is -1, then the file has been
		// edited, don't modify the curline
		if ( bpnInitLine( bpnCur ) != -1 ) {
			bpnCurLine( bpnCur ) = bpnInitLine( bpnCur );
		}

		// Make sure that there is code!
		if ( !CCanInstantiateSourceBP( &bpnCur, NULL ) )
			return;

		char	*pszFile;

		if ( pszFile = _ftcsrchr(bpnLocCxtSource( bpnCur ),'\\' ) )
			pszFile++;
		else
			pszFile = bpnLocCxtSource( bpnCur );

		PBREAKPOINTNODE pbpn = BHFirstBPNode();
		int iRet;
		while( pbpn )
		{
			// if the hbpi is NULL, try to set the handler for this node one more time.
			if ( !pbpnBPhbpi( pbpn ) )
			{
				BHCreateBPHandle(pbpn, TRUE, TRUE, &iRet);
				if (!pbpnBPGroup(pbpn))
				{
					// Check for bound breakpoint only...
					if(!pbpnBPVirtual(pbpn))
					{
						// If BP is supposed to be disabled do it now
						if(!pbpnEnabled(pbpn))
						{
							BPDisableFar(pbpnBPhbpi(pbpn)) ;
						}
						// else BP is already physically existing
					}
					else
					{
						// Type is supported but we couldn't set bp, force disable
						pbpnEnabled(pbpn) = FALSE ;
					}
				}
			}

			LPBPI 	lpbpi = (LPBPI)LLLpvFromHlle( pbpnBPhbpi( pbpn ) );

			if ( lpbpi )
			{
				ADDR 	addr  = lpbpi->CodeAddr ;
				char	szFile[_MAX_PATH];
				HSF 	hsf;
				int     iLine;

				UnlockHlle( pbpnBPhbpi( pbpn ) );

				// Get the line number
				GetSourceFrompADDR(&addr,szFile,sizeof(szFile),(LONG *)&iLine,NULL,&hsf);

				char	*pszCurBPFile;

				if ( pszCurBPFile = _ftcsrchr(szFile,'\\' ) )
					pszCurBPFile++;
				else
					pszCurBPFile = szFile;

				if ( bpnCurLine( bpnCur ) == iLine &&!_ftcsicmp(pszCurBPFile,pszFile ) && pbpnLocationNode( pbpn ) )
				{
					if ( fEnable )
					{
						BPDisableFar ( pbpnBPhbpi( pbpn ) );
						pbpnEnabled ( pbpn ) = FALSE ;
					}
					else
					{
						BPEnableFar ( pbpnBPhbpi( pbpn ) );
						pbpnEnabled ( pbpn ) = TRUE ;
					}
				}
			}

			pbpn = pbpnNext( pbpn );
		}
	}
	else
	{
		char	*pszFile;

		if ( pszFile = _ftcsrchr(bpnLocCxtSource( bpnCur ),'\\' ) )
			pszFile++;
		else
			pszFile = bpnLocCxtSource( bpnCur );

		PBREAKPOINTNODE pbpn = BHFirstBPNode();

		while( pbpn )
		{
			char	*pszCurBPFile;

			if ( pszCurBPFile = _ftcsrchr(pbpnLocCxtSource( pbpn ),'\\' ) )
				pszCurBPFile++;
			else
				pszCurBPFile = pbpnLocCxtSource( pbpn );

			if ( bpnCurLine( bpnCur ) == pbpnCurLine( pbpn ) &&!_ftcsicmp(pszCurBPFile,pszFile ) )
			{
				if ( fEnable )
					pbpnEnabled ( pbpn ) = FALSE ;
				else
					pbpnEnabled ( pbpn ) = TRUE ;
			}

			pbpn = pbpnNext( pbpn );
		}
	}

	// Update the UI after the list has been updated!
	PBREAKPOINTNODE pbpn = BHFirstBPNode ();

	while (pbpn != NULL)
	{
		BFHShowBPNode (pbpn);
		pbpn = pbpnNext (pbpn);
	}

	UpdateBPInViews();
}

/****************************************************************************

	FUNCTION:   EnableDisableLocBP()

	PURPOSE:    Disable all breakpoint on the same line if at least one
				is enabled.  Enable all breakpoints if all breakpoints
				on the same line are disabled.
				Only valid for DOC, DISASSY, BREAKPT, and CALLSTACK windows

	RETURNS:	

****************************************************************************/
VOID PASCAL EnableDisableLocBP ( BOOL fEnable )
{
	CView   *pCurIDEView;

	if ( (pCurIDEView = GetCurView()) != NULL )
	{
		DOCTYPE dt;

		if ( gpISrc->ViewBelongsToSrc(pCurIDEView) )
			dt = DOC_WIN;
		else if ( pCurIDEView->IsKindOf(RUNTIME_CLASS(CMultiEdit)) )
			dt = ((CMultiEdit *)pCurIDEView)->m_dt;
		else
			dt = MEMORY_WIN; // Just to get past the switch

		// Support toggle for specific windows only
		//		Currently: DOC_WIN, CALLS_WIN, DISASSY_WIN, and BREAKPT_WIN
		//		See also fIsValidWindowForBP
		switch( dt )
		{
			case DOC_WIN:
				EnableDisableEditorLocBP ( fEnable );
				break;

			case CALLS_WIN:
			case DISASSY_WIN:
			case BREAKPT_WIN:
			{
				GCAINFO	gcai = {0};

				// Get the address for where the cursor is.  The window
				// must be sensitive to this message.  A non-zero return
				// value indicates that the addr packet has been filled in.
				// it is GROSSLY ASSUMED that it IS valid!
				if ( (LRESULT)gcafAddress == pCurIDEView->SendMessage(WU_GETCURSORADDR,0, (DWORD)&gcai ))
				{
#if defined (_MIPS_) || defined (_ALPHA_)	// Dolphin 13040
					MassageBPAddr(&gcai.addr, 0);
#endif

					UINT    uiWidgetFlag;
					HBPI	hbpi = BPIsAddr(
									&gcai.addr,
									1,
									0,
									TRUE,
									CHECK_DISABLED_BREAKPOINTS,
									&uiWidgetFlag );

					if ( hbpi ) {
						PBREAKPOINTNODE pbpn = BHFirstBPNode();
						while( pbpn )
						{
							LPBPI 	lpbpi =
										(LPBPI)LLLpvFromHlle(pbpnBPhbpi(pbpn));
							if ( lpbpi )
							{
								ADDR 	addr  = lpbpi->CodeAddr ;

								UnlockHlle( pbpnBPhbpi( pbpn ) );

								if ( GetAddrOff(gcai.addr) == GetAddrOff(addr) &&
									GetAddrSeg(gcai.addr) == GetAddrSeg(addr) &&
									emiAddr(gcai.addr) == emiAddr(addr) )
								{
									if ( fEnable )
									{
										BPDisableFar ( pbpnBPhbpi( pbpn ) );
										pbpnEnabled ( pbpn ) = FALSE ;
									}
									else
									{
										BPEnableFar ( pbpnBPhbpi( pbpn ) );
										pbpnEnabled ( pbpn ) = TRUE ;
									}
								}
							}
							pbpn = pbpnNext( pbpn );
						}
					}

					// Update the UI after the list has been updated!
					PBREAKPOINTNODE pbpn = BHFirstBPNode ();
					while (pbpn != NULL)
					{
						BFHShowBPNode (pbpn);
						pbpn = pbpnNext (pbpn);
					}

					UpdateBPInViews();
				}
			}
		}
	}
	// Is class view active?
	else
	{
		// class view stuff not supported yet
		return;
	}
}

#ifndef UNUSED // [paulde] 
/****************************************************************************

	FUNCTION:   FindEditorLocBP()

	PURPOSE:    Returns TRUE when a breakpointis  at the current editor line.

	INPUT:      fEnabled  flag if at least one BP is enabled
	RETURNS:	TRUE if successful, FALSE otherwise.

****************************************************************************/
BOOL PASCAL FindEditorLocBP ( BOOL * pfEnabled )
{
	BREAKPOINTNODE 	bpnCur;
	char 			szCurLine[20];
	int 			NodeIndex;
	int 			nbBP ;


	*pfEnabled = FALSE;

	CTextView *pCurTextView = GetCurTextView();
	ASSERT(pCurTextView != NULL);

	CTextDoc *pCurTextDoc = (CTextDoc *)pCurTextView->GetDocument();

	// Make sure these casts were kosher
	ASSERT(((CView *)pCurTextView)->IsKindOf(RUNTIME_CLASS(CTextView)));
	ASSERT(((CDocument *)pCurTextDoc)->IsKindOf(RUNTIME_CLASS(CTextDoc)));

	const CString &strCurViewFileName = GetDebugPathName(pCurTextDoc);

	// Set up a breakpoint node and a command string for the
	// current line
	if ((pCurTextView->GetDocType() == DOC_WIN) &&
		 !strCurViewFileName.IsEmpty())
	{
		// Make a current line BP command
		if ( FNonLocalDecimalPoint () ) {
			// the dot in the line header for a breakpoint
			// doesn't work when a non-localized decimal point
			// such as a comma is being used.
			*szCurLine = BP_LINELEADER_OTHER;
		}
		else {
			*szCurLine = BP_LINELEADER;
		}
		_itoa(pCurTextView->GetCurrentLine()+1, szCurLine+1, 10);
		memset(&bpnCur, 0, sizeof(bpnCur));
		if (ParseCV400Location(szCurLine, &bpnCur))
		{
			// We've got a good node so set the type...
			bpnType(bpnCur) = BPLOC;
		}
		else
		{
			return FALSE;
		}
	}
	else
	{
		return FALSE;
	}

	if ( DebuggeeAlive() )
	{
		int 	cslp;
		int		islp;
		LPSLP	rgslp;
		LPSLP	lpslp;
		BOOL	fFound = FALSE;
		UINT    uiWidgetFlag;

		// If the initline is -1, then the file has been
		// edited, don't modify the curline
		if ( bpnInitLine( bpnCur ) != -1 ) {
			bpnCurLine( bpnCur ) = bpnInitLine( bpnCur );
		}

		// Make sure that there is code!
		if ( !CCanInstantiateSourceBP( &bpnCur, NULL ) )
		{
			return FALSE;
		}

		cslp = SLCAddrFromLine(
			(HEXE)NULL,
			(HMOD)NULL,
			(char *)(const char *)strCurViewFileName,
			bpnCurLine( bpnCur ),
			&rgslp
		);
		// Pass zero -- fixup for MIPS and Alpha
#if defined (_MIPS_) || defined (_ALPHA_)
		for( islp = 0, lpslp = rgslp; islp < cslp; ++islp, ++lpslp ) {
			if (!MassageBPAddr(&lpslp->addr, &lpslp->cb)) {
				cslp--;
				if (islp != cslp) {
					_fmemmove(lpslp, lpslp+1, (cslp-islp)*sizeof(rgslp[0]));
					islp--;
					lpslp--;
				}
			}
		}
#endif
		// Pass one, see if the address is in the list
		for( islp = 0, lpslp = rgslp; !fFound && islp < cslp; ++islp, ++lpslp )
		{
			fFound = !!BPIsAddr(
						&lpslp->addr,
						lpslp->cb,
						0,
						TRUE,
						CHECK_DISABLED_BREAKPOINTS,
						&uiWidgetFlag );
		}

		if ( fFound )
		{
			switch ( uiWidgetFlag )
			{
				case DISABLED_BRKPOINT_LINE:
					*pfEnabled = FALSE;
					break;
				case MIXED_BRKPOINT_LINE:
					*pfEnabled = TRUE;
					break;
				default:
					*pfEnabled = TRUE;
					break;
			}
			return ( TRUE );
		}
	}
	else
	{
		// Wow.. see other wow comment above.. [cbrown]
		CString str = GetDebugPathName(pCurTextView->GetDocument());
		LPTSTR	lptstr = (LPTSTR)(LPCTSTR)str;
		int		iLine = pCurTextView->GetCurrentLine() + 1;
		UINT    uiWidgetFlag;

		// No current debuggee so we search the
		// breakpoint node list for a bp node set for the
		// current line in the current file.
		if ( ( nbBP = BFHGetLineBPIndex(
						lptstr,
						iLine,
						&( NodeIndex = 0 ),
						CHECK_DISABLED_BREAKPOINTS,
						&uiWidgetFlag ) ) > 0 )
		{
			switch ( uiWidgetFlag )
			{
				case DISABLED_BRKPOINT_LINE:
					*pfEnabled = FALSE;
					break;
				case MIXED_BRKPOINT_LINE:
					*pfEnabled = TRUE;
					break;
				default:
					*pfEnabled = TRUE;
					break;
			}
			return ( TRUE );

		}
	}

	return FALSE;
}
#endif // UNUSED

/****************************************************************************

	FUNCTION: CCanInstantiateSourceBP

	PURPOSE: Given a source line breakpoint, roughly determine if the number
			of instances of the line in all of the loaded modules (unless
			an hexe is specified)

	RETURNS: Approximate number of instances of the source line

****************************************************************************/
int CCanInstantiateSourceBP( PBREAKPOINTNODE pbpn, LPPBP lppbp ) {
	int 	cslp;
	LPSLP	rgslp = (LPSLP)NULL;
	int		clinesSrch = clinesScanForSource;
	int		ilnCur = pbpnCurLine( pbpn );


	// Search (max clinesScanForSource) for a matching source line!
	do {
		cslp = SLCAddrFromLine(
			SHGethExeFromName( pbpnLocCxtExe( pbpn ) ),
			(HMOD)NULL,
			pbpnFileName( pbpn ),
			ilnCur++,
			&rgslp
		);
#if defined (_MIPS_) || defined (_ALPHA_)
		for (int islp = 0; islp<cslp; islp++) {
			if (!MassageBPAddr(&rgslp[islp].addr, &rgslp[islp].cb)) {
				cslp--;
				if (islp != cslp) {
					_fmemmove(rgslp+islp, rgslp+islp+1,
							  (cslp-islp)*sizeof(rgslp[0]));
					islp--;
				}
			}
		}
#endif
	} while( !cslp && clinesSrch-- );

	if ( cslp ) {
		// Save matching line
		pbpnCurLine( pbpn ) = ilnCur - 1;

		if ( lppbp ) {
			memset( lppbp, 0, sizeof( PBP ) );
			if ( cslp == 1 ) {
				lppbp->Addr = rgslp[ 0 ].addr;
			}
			else {
				extern int PASCAL AMBResolveSrc( int, int, LPSLP, LPADDR );

				// Returns 1 for selected item or zero for cancel
				cslp = AMBResolveSrc( cslp, ilnCur - 1, rgslp, &lppbp->Addr );
			}
		}
	}

	if ( rgslp ) {
		SHFree( (LPV)rgslp );
	}

	return cslp;
}

/****************************************************************************

	FUNCTION: FInstantiateSourceBP

	PURPOSE: Given a source line breakpoint, determine exactly how many matches
			there are and create them.  NOTE:  If a generic "{,foo.c,}.4" is
			specified and there is only 1, it will NOT be instantiated here!

	RETURNS: TRUE if one or more breakpoints have been instantiated

****************************************************************************/
BOOL FInstantiateSourceBP( PBREAKPOINTNODE pbpn ) {
	int		cInst = 0;
	int 	cslp;
	int 	islp;
	LPSLP	lpslp;
	LPSLP	rgslp = (LPSLP)NULL;
	int		iGroup = ++iGroupMax;
	HEXE	hexe;
	ADDR	addr;

	cslp = SLCAddrFromLine(
		hexe = SHGethExeFromName( pbpnLocCxtExe( pbpn ) ),
		(HMOD)NULL,
		pbpnFileName( pbpn ),
		pbpnCurLine( pbpn ),
		&rgslp
	);
#if defined (_MIPS_) || defined (_ALPHA_)
	for (islp=0; islp<cslp; islp++) {
		if (!MassageBPAddr(&rgslp[islp].addr, &rgslp[islp].cb)) {
			cslp--;
			if (islp != cslp) {
				_fmemmove(rgslp+islp, rgslp+islp+1,
						  (cslp-islp)*sizeof(rgslp[0]));
				islp--;
			}
		}
	}
#endif
	// if cslp == 1 and either a function or exe is specified just return
	//  and let caller add the breakpoint with the matching source text
	// info which can be parsed by BPParse!
	if ( cslp != 1 &&
		( !*pbpnLocCxtExe( pbpn ) || !*pbpnLocCxtFunction( pbpn ) )
	) {
		// Loop through all of the occurrences
		for( islp = 0, lpslp = rgslp; islp < cslp; ++islp, ++lpslp ) {
			PBREAKPOINTNODE	pbpnT;
			CXT				cxt = {0};
			HSYM			hsym;
			LPCH			lst;
			BREAKPOINTNODE	bpn = *pbpn;

			// Setup the default information for the breakpoint.  If we are
			// going to parse and add it to the list, we will get recursive
			// (infinitely) if the LocType isn't LOCADDREXPR!
			bpnBPGroup( bpn ) = 0;
			bpnNext( bpn ) = NULL;
			bpnEnabled( bpn ) = TRUE;
			bpnLocType( bpn ) = BPLOCFILELINE;

			// Get the bounding context for compare or fill in the
			// BREAKPOINTNDOE package
			SHSetCxt( &lpslp->addr, &cxt );

			// "Exe/Dll"  If one wasn't specified, fill it in
			if ( !*pbpnLocCxtExe( pbpn ) ) {

				LPTSTR pExeName = SHGetExeName( SHHexeFromHmod( SHHMODFrompCXT( &cxt ) ) );
				if (pExeName)
					_tcscpy(
						bpnLocCxtExe( bpn ),
						pExeName
					);
			}

			// "Function"
			SHGetNearestHsym(
				&lpslp->addr,
				SHHMODFrompCXT( &cxt ),
				EECODE,
				&hsym
			);

			FnNameFromHsym( hsym, &cxt, (LPSTR)bpnLocCxtFunction( bpn ) );

			if ( *bpnLocCxtFunction( bpn ) ) {
				// If there was a Function specified and it doesn't match the
				// address being processed, don't add to the list
				if ( *pbpnLocCxtFunction( pbpn ) &&
					_ftcscmp( pbpnLocCxtFunction( pbpn ), bpnLocCxtFunction( bpn ) )
				) {
					continue;
				}
			}

			// Can't get a function (probably compiled -Zd), convert to address
			// expression only if the address is loaded.
			else if ( addr = lpslp->addr, SYFixupAddr ( &addr ) ) {

				if ( ADDR_IS_FLAT( addr ) ) {
					ASSERT( ADDR_IS_OFF32( addr ) );
					sprintf(
						bpnAddrExpr( bpn ),
						"0x%08lX",
						GetAddrOff( addr )
					);
				}
				else {
					sprintf(
						bpnAddrExpr( bpn ),
						"0x%04X:0x%0*X",
						(unsigned short)GetAddrSeg( addr ),
						ADDR_IS_OFF32( addr ) ? 8 : 4,
						(unsigned short)GetAddrOff( addr )
					);
				}
				bpnLocType( bpn ) = BPLOCADDREXPR;
				*bpnLocCxtFunction( bpn ) = '\0';
				*bpnLocCxtSource( bpn ) = '\0';
				*bpnLocCxtExe( bpn ) = '\0';

			}

			// Unable to fixup the address since the module isn't loaded.  Skip
			// this one
			else {
				continue;
			}

			// Copy the REAL OMF file name (SLNameFromHsf returns a length
			// prefixed non-zero terminated string!)
			lst = SLNameFromHsf( SLHsfFromPcxt( &cxt ) );
			memcpy( bpnFileName( bpn ), lst + 1, (size_t)(UCHAR)*lst );
			*( bpnFileName( bpn ) + (UCHAR)*lst ) = '\0';

			if ( pbpnT = AddBreakpointNode( &bpn, TRUE, TRUE, FALSE, NULL ) ) {

				// Set the iGroup (info that it's been instantiated!)
				pbpnBPGroup( pbpnT ) = iGroup;

				// Set true enable status
				pbpnEnabled( pbpnT ) = pbpnEnabled( pbpn );

				// Increment the number added
				++cInst;

				// Highlight
				BFHShowBPNode( pbpnT );
			}
		}
	}

	if ( rgslp ) {
		SHFree( (LPV)rgslp );
	}

	// If we failed, don't set the group so everything else can fail
	if ( !cInst ) {
		--iGroupMax;
		pbpnBPGroup( pbpn ) = 0;
	}

	// success.  save the iGroup in the original (used sparingly to
	// delete the now instantiated breakpoint node.
	else {
		pbpnBPGroup( pbpn ) = iGroup;
		pbpnMarkDel ( pbpn ) = TRUE;
	}

	return cInst != 0;
}

/****************************************************************************

	FUNCTION:   UpdateBPInViews()

	PURPOSE:	Delete any BPs marked for deletion, and reset the BP
				editing flags.  Update the editor screen to reflect any
				changes.

****************************************************************************/
void UpdateBPInViews ( void )
{
	if ( DebuggeeAlive() )
	{
		// Should change this to SendMessage(,WU_BREAKPTCHANGE,)
		if ( pViewCalls )
			pViewCalls->InvalidateRect((LPRECT)NULL, FALSE);

		if ( pViewDisassy )
			pViewDisassy->InvalidateRect((LPRECT)NULL, FALSE);
	}
}

BOOL BHFTypeSupported( WORD iType )
{
	BOOL fSupported = TRUE;

	if ( pDebugCurr ) {
		static const BPTS mpTypeBpts[] = {
			bptsExec,					// BPLOC
			bptsExec,					// BPLOCAEXPRTRUE
			bptsExec,					// BPLOCEXPRCHGD

			bptsDataC | bptsRegC,		// BPEXPRTRUE
			bptsDataC | bptsRegC,		// BPEXPRCHGD

			bptsMessage | bptsMClass	// BPWNDPROCMSGRCVD
		};

		ASSERT( iType < sizeof( mpTypeBpts ) / sizeof( BPTS ) );
		
		fSupported = !!( pDebugCurr->MtrcBreakPoints() & mpTypeBpts[ iType ] );
	}

	return fSupported;
}


BOOL BHFTypeEnabled( WORD iType )
{
	BOOL fEnabled = BHFTypeSupported( iType );

	// If the debuggee isn't running, then there had better be
	// a pDebuggCurr.  If not, this is a fatal error
	ASSERT( !DebuggeeRunning() || pDebugCurr );

	// Extra check for BPs/WPs while child is running (async)
	if ( fEnabled && DebuggeeRunning() ) {

		static const ASYNC mpTypeAsync[] = {
			asyncBP,	// BPLOC
			asyncBP,	// BPLOCAEXPRTRUE
			asyncBP,	// BPLOCEXPRCHGD

			asyncWP,	// BPEXPRTRUE
			asyncWP,	// BPEXPRCHGD

			asyncBP		// BPWNDPROCMSGRCVD
		};

		ASSERT( iType < sizeof( mpTypeAsync ) / sizeof( ASYNC ) );

		fEnabled = !!( pDebugCurr->MtrcAsync() & mpTypeAsync[ iType ] );
	}

	return fEnabled;
}

void SetLastBreakpointNode(PBREAKPOINTNODE pbpn)
{
	LastBreakpointNode = pbpn;
}


// Used when we are doing a quick restart. When doing a quick restart we don't
// re-instantiate the whole bp list. However it is possible that some BPs on hard
// addresses cannot be reset correctly. We warn about these BP's here.

VOID CheckBPsAtRestart()
{
	PBREAKPOINTNODE pbpn;
	char 	szBPCmd[cbBpCmdMax];
	
	for ( pbpn = BHFirstBPNode(); pbpn != NULL ; pbpn = pbpnNext(pbpn) )	
	{
		if ( pbpnEnabled(pbpn) && pbpnEbpt(pbpn) == BPADDR )
		{
			
			BPDisableFar(pbpnBPhbpi(pbpn));
			pbpnEnabled( pbpn ) = FALSE;

			FormatDisplayBP(pbpn, szBPCmd, sizeof(szBPCmd));
			InformationBox(ERR_Const_Address_Breakpoint, (LPSTR)szBPCmd);
		}
	}
}			

// For Java, we want to load any classes referenced by BPs. We'll go through the
// list of BPNs and load any classes referenced in the EXE part of the context.
BOOL LoadBPReferencedClasses(void)
{
	// Iterate through the list of BPs.
	PBREAKPOINTNODE pbpnCur = BHFirstBPNode();
	while (pbpnCur != NULL) {

		// Get the class from the context.
		LPTSTR pszClass = NULL;
		switch (pbpnType(pbpnCur)) {
			case BPLOC:
			case BPLOCEXPRTRUE:
			case BPLOCEXPRCHGD: {
				pszClass = pbpnLocCxtExe(pbpnCur);
				break;
			}
			case BPEXPRTRUE:
			case BPEXPRCHGD: {
				pszClass = pbpnExprCxtExe(pbpnCur);
				break;
			}
		}

		// If a class name is specified, load the class.
		if (pszClass != NULL && *pszClass != _T('\0')) {

			SHE she = SHAddDll(pszClass, TRUE);
		}

		pbpnCur = pbpnNext(pbpnCur);
	}

	return TRUE;
}
