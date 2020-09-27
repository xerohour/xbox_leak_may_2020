#include "stdafx.h"
#pragma hdrstop

#include "tokens.h"
#include "autownd.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif


/////////////////////////////////////////////////////////////////////////////
// CAutoWnd

CAutoWnd::CAutoWnd(CBaseVarsView *pView)
	: CBaseClass( "",  pView)
{
	m_strTabName.LoadString(SYS_AutoWin_Title);;
	m_bLineInfoValid = FALSE;
	m_hsf =  NULL;
	m_curLineNo = 0;	// 1 based so 0 is an invalid value.
}

CAutoWnd::~CAutoWnd()
{
}

void CAutoWnd::UpdateEntries( )
{
	CXF curCxf( ::CxfWatch( ));
	BOOL bChangeSelect = TRUE;

	if ( ::DebuggeeAlive( ))
	{
		if ( ( curCxf.cxt.hMod && curCxf.cxt.hProc ) )
		{
			// Figure out the new HSF, lineNo where the IP is currently.
			LONG lineNo;
			HSF hsf;
			_TCHAR szFileName[_MAX_PATH];

			if ( GetSourceFrompADDR(SHpADDRFrompCXT(SHpCXTFrompCXF(&curCxf)),
							szFileName, _MAX_PATH, &lineNo, NULL, &hsf))
			{
			
				// Don't reparse if we are the same line as last time.
				if ( m_bLineInfoValid && hsf == m_hsf && lineNo == m_curLineNo && GetRowCount() != 0 )
				{
					// try to maintain the selection if it is in a row
					// we know will be there after the Update is done.
					CTreeGridRow *pRow = GetCurSel();
					if (pRow != NULL)
					{
						ASSERT(!pRow->IsNewRow());
						if (!((CWatchRow *)pRow)->IsRetVal())
							bChangeSelect = FALSE;
					}

					// Even if we are on the same line, the return values
					// could have changed completely.
					DeleteRetValRows();

					CBaseClass::OnUpdateDebug(0,0);
				}
				else
				{
					m_bLineInfoValid = TRUE;
					m_hsf = hsf;
					m_curLineNo = lineNo;
					m_strFileName = szFileName;

					CWRInfoArray rgWRInfo;

					// Remember the currently expanded entries so we
					// can re-expand then after we are done.
					RememberPrevState(rgWRInfo);
					ClearCurEntries();

					AddExprsFromSourceLines(m_strFileName, m_hsf, m_curLineNo);

					RefreshUsingPrevState(rgWRInfo);

				}
			}
			else
			{
				// Couldn't get the source line info nothing we can do here.
				m_bLineInfoValid = FALSE;
				ClearCurEntries( );
			}
		}
		else
		{
			// No source line info here.
			m_bLineInfoValid = FALSE;
			ClearCurEntries( );
		}

		AddExprsFromReturnValues ();
	}
	else
	{
		m_bLineInfoValid = FALSE;
		// No source line in this context.
		ClearCurEntries( );
	}

	// make the grid refresh itself
	InvalidateGrid();

	if (bChangeSelect && GetRowCount() != 0)
		Select(GetRow(0));
}
										

	
// Given a source line this function adds the appropriate auto-watch items from the line.

void CAutoWnd::AddExprsFromSourceLines(const CString& strFileName, HSF m_hsf, LONG curLineNo)
{
	LPSOURCEEDIT pISourceEdit = NULL;
	if (!SUCCEEDED(gpISrc->CreateSourceEditForHSF( m_hsf, &pISourceEdit)))
	{
		gpISrcQuery->CreateSourceEditForOpenDoc(strFileName, &pISourceEdit, FALSE);
		if (NULL == pISourceEdit)
		{
			TRACE("Dbg:Couldn't get iface for '%s'\n", strFileName);
			return;
		}
	}
	ASSERT(pISourceEdit);

	// The incoming line-no is the last line of the current C/C++ statement.
	WORD rgln[2];
	
	LONG begLine = curLineNo;
	LONG endLine = curLineNo;
	LONG prevLine = curLineNo - 1;

	// Remember the address of the current line
	ADDR addrCurr;
	SLFLineToAddr( m_hsf, (WORD)curLineNo, &addrCurr, NULL, NULL);

	// if the previous line doesn't have CVINFO associated with it,
	// then go back to the last line that does and start from the next line.
	if ( !SLFLineToAddr( m_hsf, (WORD)(curLineNo - 1), NULL, NULL, rgln))
	{
		prevLine = rgln[0];
		begLine = prevLine + 1;
	}
	

	if ( begLine + maxLinesScanned < endLine )
		begLine = endLine - maxLinesScanned;

	CTokenStream tokStream(pISourceEdit, begLine - 1, endLine - 1);

	CStrArray rgStr;
	CTMArray  rgtm;

	// This will parse the current line.
	Parse(tokStream, rgtm, rgStr);

	// Now figure out where the previous line starts and decide if we want to parse that.

	begLine = prevLine;
	endLine = prevLine;

	ADDR addrPrevLine;

	// If the prev line doesn't have any address associated with it, don't bother.
	if ( SLFLineToAddr( m_hsf, (WORD)prevLine, &addrPrevLine, NULL, NULL) )
	{
		// Figure out the true extent of the previous line.
		if ( !SLFLineToAddr( m_hsf, (WORD)(prevLine - 1), NULL, NULL, rgln))
		{
			begLine = rgln[0] + 1;
		}

		// Use Previous line only if the Function corresponding to the previous line is the
		CXT cxtCurr = {0};
		CXT cxtPrevLine = {0};

		if ( SHSetCxt(&addrCurr, &cxtCurr) != NULL &&
			 SHSetCxt(&addrPrevLine, &cxtPrevLine) != NULL &&
			 SHHPROCFrompCXT(&cxtCurr) == SHHPROCFrompCXT(&cxtPrevLine)
		   )
		{
			if ( begLine + maxLinesScanned < endLine )
				begLine = endLine - maxLinesScanned;
			
			CTokenStream tokStreamPrev(pISourceEdit, begLine - 1, endLine - 1);
			Parse(tokStreamPrev, rgtm, rgStr);
		}
	}
	pISourceEdit->Release();

	// If we haven't added 'this' yet, and there is a valid 'this' i.e we are in
	// a C++ member function, add it since it is likely an interesting thing to watch.
	CString strThis("this");
	int nIndex;


	if ( !IsStrInArray(rgStr, strThis) )
	{
		// We haven't include 'this' already.
		CRootTM * ptmthis = new CRootTM(strThis, TRUE);		
		
		if (IsInterestingAutoTM(ptmthis) )
		{
			rgStr.Add(strThis);
			rgtm.Add(ptmthis);
		}
		else
		{
			delete ptmthis;
		}
	}

	// Finally add all the TM's we have collected so far into he
	// auto watch window.
	for ( nIndex = 0 ; nIndex < rgtm.GetSize() ; nIndex++ )
	{
		CWatchRow *pRow = new CWatchRow(this, rgtm[nIndex]);
		
		VERIFY(InsertTopLevelRow(pRow, FALSE));
	}

	// Hack for non-autosort
	m_nSortColumn = COL_NAME;
	ReSort();
	m_nSortColumn = -1;

}	

VOID CAutoWnd::AddExprsFromReturnValues ()
{
	CPtrList &lst = g_retVals.GetList ();

	POSITION pos = lst.GetHeadPosition();
	while (pos)
	{
		POSITION posTmp = pos;

		CReturnTM *pTM =  (CReturnTM *)lst.GetNext(pos);
		ASSERT(pTM->IsKindOf(RUNTIME_CLASS(CReturnTM)));
		
		CReturnTM *pAddTM = new CReturnTM(*pTM);

		CWatchRow *pRow = new CWatchRow(this, pAddTM);
		
		VERIFY(InsertTopLevelRow(pRow, FALSE));
	}
}

VOID CAutoWnd::DeleteRetValRows( )
{
	POSITION pos = GetHeadRowPosition();
	
	while ( pos != NULL )
	{
		CTreeGridRow *pRow = (CTreeGridRow *)GetNextRow(pos);
		if (!pRow->IsNewRow() && ((CWatchRow *)pRow)->IsRetVal( ))
		{
			ASSERT(!IsExpanded(pRow));

			RemoveRow(pRow, FALSE);
			delete pRow;
		}
	}
}
						 		
BOOL CAutoWnd::IsInterestingAutoTM(CTM *pTM)
{
	return pTM->IsValueValid() && pTM->IsDataTM();
}


	
// Given a stream of tokens, figures out the TM's that should be added for this token stream.
// Appends the TM's to the CTMArray passed in.
// Adds TM's only if the corresponding strings are not in rgStr, appends new strings to rgStr.

VOID CAutoWnd::Parse(CTokenStream& tokStream, CTMArray& rgtm, CStrArray& rgStr)
{
	BOOL bEndOfStream = FALSE;

	CTokenStreamPos pos = tokStream.GetHeadPosition ();
	CTokenStreamPos posSave;
	BOOL bRestoreTokStream = FALSE;

	if (!pos.IsValid()) return;
		
	while (	!bEndOfStream )
	{
		if ( bRestoreTokStream )
		{
			pos = posSave;
			bRestoreTokStream = FALSE;
		}

		if ( !MoveToStartToken(tokStream, pos) ) // Nothing more to parse.
			break;

		CString strCurr;
		CList<CString, CString&> strCandidates;

		CToken tokenInit(tokStream.GetAt (pos)->tok);

		if (tokenInit.IsOperator())
		{
			switch (tokenInit)
			{
				case tokOP_AND:
				case tokOP_MUL:
				case tokOP_DCOLON:
					bEndOfStream = !GetInitialOperators(tokStream, pos, strCurr);
					break;
				default:
					ASSERT(FALSE);
			}
		}
		else {
			ASSERT(tokenInit.IsCxxIdent());
		}

		if (bEndOfStream || !SkipComments(tokStream, pos))
		{
			ASSERT(!tokenInit.IsCxxIdent());
			bEndOfStream = TRUE;
			break;		// reached the end of stream.
		}

		BOOL bExpectIdent = TRUE;
		BOOL bContinue = FALSE;
				
		do
		{
			if (bExpectIdent)
			{
				CToken token(tokStream.GetAt (pos)->tok);

				if (token.IsCxxIdent())
				{
					strCurr += tokStream.GetStringAt (pos);			
					// Add the string we have seen so far to our list of candidates.
					strCandidates.AddHead(strCurr);
					if (tokStream.GetNext (pos), !pos.IsValid ())
					{
						bEndOfStream = TRUE;
						break;
					}
				}
				else {
					break;
				}
					
			}

			if (!SkipComments(tokStream, pos))	
			{
				bEndOfStream = TRUE;
				break;
			}

			CToken token(tokStream.GetAt (pos)->tok);
			
			switch (token)
			{
				case tokOP_DOT:
				case tokOP_ARROW:
				case tokOP_ARROWSTAR:
				case tokOP_DOTSTAR:
				case tokOP_DCOLON:
					strCurr += tokStream.GetStringAt (pos);
					bExpectIdent = TRUE;
					tokStream.GetNext (pos);
					if (!pos.IsValid ())
						bEndOfStream = TRUE;
					else
						bContinue = TRUE;
					break;

				case tokDELI_LBKT:	
				{
					// rememeber the position so we will re-parse for things inside the array index.
					bRestoreTokStream = TRUE;
					posSave = pos;

					CString strArray = strCurr;

					if (CollectIndexStr(tokStream, pos, strArray))
					{
						strCandidates.AddHead(strArray);
						strCurr = strArray;
						bExpectIdent = FALSE;
						bContinue = TRUE;
					}
					else
					{
						bContinue = FALSE;
					}
					break;
				}
				default:
					bContinue = FALSE;

			} /* end switch */
		}
		while (bContinue);
				
		// At this point strCandidates has the list of candidates to be added to the auto watch window.
		// We go through this list and try and see if any of them are interesting entries to add to the
		// auto watch window. Also if the string already exists, then we ju

		BOOL bAdded = FALSE;
		CString strBestChoice;

		if (!strCandidates.IsEmpty())
			strBestChoice = strCandidates.GetHead();

		while (!strCandidates.IsEmpty() )
		{
			CString strWatch(strCandidates.RemoveHead());

			if (IsStrInArray(rgStr, strWatch))
				break;	// We already tried to put in the best candidate to this point into the window.


			// We will record that we tried this string even if we are unsuccesful at adding it,
			// that way we don't fail on the same thing again and again.
			rgStr.Add(strWatch);
			
			CRootTM * pTM = new CRootTM(strWatch, TRUE);

			if ( IsInterestingAutoTM(pTM) )
			{
				// If we have an expression such as *ptr where ptr is a ptr to a class,
				// we just show ptr. This is because ptr shows every bit of information that
				// *ptr shows and we are likely to get less number of duplicate entries.
				if (tokenInit == tokOP_MUL && pTM->IsClass())	
				{
					CString strPtr = strWatch.Mid(1);
					if (IsStrInArray(rgStr, strPtr))
						break;	// Already there, no problemo.

					rgStr.Add(strPtr);
						
					CRootTM * pAltTM = new CRootTM(strPtr, TRUE);
					if ( IsInterestingAutoTM(pAltTM) )
					{
						delete pTM;
						pTM = pAltTM;
					}
					else
						delete pAltTM;
				}

				rgtm.Add(pTM);
				bAdded = TRUE;
				break;	
			}
			else
			{
				delete pTM;
			}
		} /* end while */

		// No luck so far, let us see if our address oiperator s putting us in
		// trouble.
		if (!bAdded && (tokenInit == tokOP_MUL || tokenInit == tokOP_AND) && !strBestChoice.IsEmpty())
		{
			// As a last ditch effort try the first candidate without the * or &
			// Note this is to make up for bad guesses in IsAddressOperator.
			CString strSansOp = strBestChoice.Mid(1);
			
			if (!IsStrInArray(rgStr, strSansOp))
			{
				rgStr.Add(strSansOp);

				CRootTM *pTM = new CRootTM(strSansOp, TRUE);
				if (IsInterestingAutoTM(pTM))
				{
					rgtm.Add(pTM);
					bAdded = TRUE;
					break;	
				}
				else
					delete pTM;
			}
		}

	} /* end while */			

}

// Starts collecting tokens starting from the begining of an array expression.
// Returns FALSE if it notices function calls/side effects in the index expression.
// If it returns TRUE, the token stream will be at the first token past the closing
// ]
// If it returns FALSE, the token Stream could be at any arbitrary point.
// IT IS NOT RESTORED. The passed in string could also contain an ill-formed
// expression in this case.

BOOL CAutoWnd::CollectIndexStr(CTokenStream& tokStream, CTokenStreamPos &pos, CString& str)
{
#if _DEBUG
	CToken tokCheck(tokStream.GetAt (pos)->tok);
	ASSERT(tokCheck == tokDELI_LBKT);
#endif

	str += tokStream.GetStringAt (pos);
	tokStream.GetNext (pos);

	
	while (SkipComments(tokStream, pos))
	{
		CToken token(tokStream.GetAt (pos)->tok);
		
		switch (token)
		{
			case tokDELI_LBKT:
				// Recursive call to this function.
				if (!CollectIndexStr(tokStream, pos, str) )
					return FALSE;
				break;

			case tokOP_INC:
			case tokOP_DECR:
				// Skip the ++ and --'s to make sure we have not side effects in the resulting watch.
				tokStream.GetNext (pos);
				break;

			case tokDELI_LPAREN:
				// Check if it is a function call.
				{
					CTokenStreamPos posTmp = pos;

					ASSERT((tokStream.GetPrev (posTmp), posTmp.IsValid ()));
					ASSERT(SkipComments(tokStream, posTmp, FALSE));

					CToken tokenPrev(tokStream.GetAt (pos)->tok);

					if (tokenPrev.IsUserIdent() || tokenPrev == tokDELI_RPAREN)
						return FALSE;		// must be a function call.
				}

				str += tokStream.GetStringAt (pos);
				tokStream.GetNext (pos);
				break;										
				
			case tokDELI_RBKT:
				// We are done,
				str += tokStream.GetStringAt (pos);
				tokStream.GetNext (pos);
				return TRUE;										
							
			default:
				if (token.IsAssignOp())
					return FALSE;
				else
				{
					str += tokStream.GetStringAt (pos);
					tokStream.GetNext (pos);
					break;
				}
		} /* end switch */
	} /* end while */

	return FALSE;
}																		
		

// Skips over comments. Moves the position ptr of the token to the first
// significant token. We don't need to check for white space because the
// lexer does not put any tokens corresponding to white space.
// returns TRUE if such a token was found, false otherwise.

BOOL CAutoWnd::SkipComments(CTokenStream& tokStream, CTokenStreamPos &pos, BOOL bForward /* = TRUE */)
{
	while (pos.IsValid ())
	{
		CToken token(tokStream.GetAt (pos)->tok);
		
		if ( token.GetTokClass() != tokclsCommentMin && token != tokOP_LINECMT )
			return TRUE;

		if (bForward)
			tokStream.GetNext (pos);
		else
			tokStream.GetPrev (pos);
	}

	return FALSE;
}								

// gets over initial strings such as
// **** *:: etc. Some of these are invalid language constructs
// but we will include them anyway. Moves the tokStream position to the first
// token past the operators.

BOOL CAutoWnd::GetInitialOperators(CTokenStream& tokStream, CTokenStreamPos &pos, CString& str)
{
	BOOL fFirstTime = TRUE;
	CString strOp;
		
	ASSERT(str.IsEmpty());

	do
	{
		SkipComments(tokStream, pos);
		CToken token(tokStream.GetAt (pos)->tok);	
		
		switch (token)
		{
			case tokOP_AND:
			case tokOP_MUL:
			case tokOP_DCOLON:
				str += tokStream.GetStringAt (pos);
				break;
			case tokOP_INC:
			case tokOP_DECR:
				// Skip '++' in *++a for example.
				ASSERT(!fFirstTime);
				break;
			default:
				ASSERT(!fFirstTime);
				return TRUE;
		}

		fFirstTime = FALSE;
	}
	while (tokStream.GetNext (pos), pos.IsValid ());

	return FALSE;
}
							
// There are a list of tokens which can start a new auto watch.
// This function moves the tokStream position to the first such token.
BOOL CAutoWnd::MoveToStartToken(CTokenStream& tokStream, CTokenStreamPos& pos)
{
	BOOL fContinue = TRUE;

	do {
		CToken token(tokStream.GetAt (pos)->tok);
		
		if (token.IsCxxIdent())
			return TRUE;

		if (token.IsOperator())
		{
			switch(token)
			{
				case tokOP_AND :
				case tokOP_MUL:
					if ( IsAddressOperator(tokStream, pos))
					{
						fContinue = FALSE;
					}
					break;

				case tokOP_DCOLON:
					fContinue = FALSE;
					break;

				case tokDELI_LPAREN:
					// FUTURE: need to handle expressions starting with parens.				
					break;
			}	/* end switch */
		}
	}
	while ( fContinue && (tokStream.GetNext (pos), pos.IsValid ()) );
	
	if (fContinue)
	{
		ASSERT(!(tokStream.GetNext (pos), pos.IsValid ()));
		return FALSE;
	}
	else
		return TRUE;
}

// Heuristically tries to determine if a '*' or '&' is actually an address operator instead
// of a multiplication/'bitwise and' operator.	Will not get it right in some situations.
// If it can't determine the status, it just returns TRUE.
// If we want to soup this up we could start using EE calls.

BOOL CAutoWnd::IsAddressOperator(CTokenStream& tokStream, CTokenStreamPos& pos)
{
	CToken tokenPrev;
	CString strPrev;

	CToken tokenOp = tokStream.GetAt (pos)->tok;			
				
	ASSERT(tokenOp.IsIndirectionOp());

	CTokenStreamPos posTmp = pos;
	if ( tokStream.GetPrev (posTmp), posTmp.IsValid () )
	{
		if (!SkipComments(tokStream, posTmp, FALSE)) return TRUE;	// We don't know any better.

		tokenPrev = tokStream.GetAt (posTmp)->tok;
		
		if ( tokenPrev.IsKeyword() || tokenPrev.IsInError() )
			return FALSE;

		if ( tokenPrev.IsConstant( ))
			return FALSE;

		if (tokenPrev.IsOperator())
		{
			switch (tokenPrev)
			{
				case tokDELI_RBKT:
				case tokOP_INC:
				case tokOP_DECR:
					return FALSE;
				default:
					// Note that we are intentionally returning TRUE for tokDELI_RPAREN,
					// even though it could be one or the other.
					// This is because for the * case we are likely to catch the correct
					// expression in the last ditch attempt in the Parse function
					//  For & address is simply more
					// common than bitwise & so return TRUE.
					return TRUE;
			}
		}
	}
	else {
		return TRUE;
	}

	ASSERT(tokenPrev.IsUserIdent());

	posTmp = pos;

	if (tokStream.GetNext (posTmp), posTmp.IsValid ())
	{
		if (!SkipComments(tokStream, posTmp)) return FALSE;	// no hope of finding anything to indirect the op.

		CToken tokenNext = tokStream.GetAt (posTmp)->tok;
		
		// We cannot continue trying to use this & or * as an address operation so return FALSE.
		if ( tokenNext.IsKeyword() || tokenPrev.IsInError() || tokenNext.IsConstant() )	
			return FALSE;		

		if (tokenNext.IsOperator())
		{
			return TRUE;	// might not be true for example - *++ptr
		}
		
		// Finally here both tokenNext and tokenPrev are user Identifiers.
		// We might want to do some checks using the EE for some of the common cases.

		return FALSE;
	}
	else {
		return FALSE;			
	}
}
																			 					
BEGIN_MESSAGE_MAP(CAutoWnd, CAutoWnd::CBaseClass)
	//{{AFX_MSG_MAP(CAutoWnd)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CAutoWnd message handlers

LRESULT CAutoWnd::OnUpdateDebug(WPARAM wParam, LPARAM lParam)
{
	UpdateEntries();
	return 0;
}	

LRESULT CAutoWnd::OnClearDebug(WPARAM wParam, LPARAM lParam)
{
	m_bLineInfoValid = FALSE;
	ClearCurEntries();
	return 0;
}	
