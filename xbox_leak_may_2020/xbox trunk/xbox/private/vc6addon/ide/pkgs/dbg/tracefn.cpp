////////////////////////////////////////////////////////////////////////////////
// Trace into specific function

#include "stdafx.h"
#pragma hdrstop
#include "tokens.h"

CTraceFunction *g_pTraceFunction = NULL;

CTraceFunction::CTraceFunction ()
{
	m_fTmlInitialized = FALSE;
}

CTraceFunction::~CTraceFunction ()
{
	if (m_fTmlInitialized)
	{
		EEFreeTML (&m_tml);
	}
}

BOOL CTraceFunction::SetupStep()
{
	BOOL bRet = FALSE;
	unsigned long ilineCurrent;
	_CB ibCurrent;
	LPSOURCEEDIT piSrcEdit = NULL; 
	CString strCall;
	LPSLP rgslp;
	int cslp;
	int islp;

	// get ISourceEdit
	ASSERT(gpISrcQuery);
	if (!SUCCEEDED(gpISrcQuery->CreateSourceEditForActiveDoc(&piSrcEdit,FALSE)))
	{
		return FALSE;
	}
	ASSERT(piSrcEdit);

	// Get current line and column
	if ((S_OK == piSrcEdit->FHasSelection()) &&
		SUCCEEDED(piSrcEdit->GetSelection(NULL, NULL, &ilineCurrent, &ibCurrent, NULL))
		)
	{
		ibCurrent--;
	}
	else 
	{
		piSrcEdit->GetCaretLine(&ilineCurrent);
		piSrcEdit->GetCaretIndex(&ibCurrent);
	}

	// get the address of the IP
	if (SYGetAddr (hpidCurr, htidCurr, adrPC, &m_addrPC) != xosdNone ||
		!SYUnFixupAddr (&m_addrPC))
	{
		goto L_Exit;
	}

	m_wLineEnd = ilineCurrent + 1;

	// Block: get a list of all addresses at this line
	{
		CString strPathName;
		piSrcEdit->GetPath(strPathName);
		// for some reason the EE complains about blank filenames
		if (strPathName == "")
			goto L_Exit;

		cslp = SLCAddrFromLine(
			(HEXE)NULL,
			(HMOD)NULL,
			(LPSTR) (LPCSTR) strPathName,
			m_wLineEnd,
			&rgslp
		);
	}

	// see if the address is in the list
	for (islp = 0; islp < cslp; islp++)
	{
		const LPSLP lpslp = &rgslp [islp];

		ASSERT (ADDR_IS_LI (lpslp->addr));

		if (GetAddrSeg (m_addrPC) == GetAddrSeg (lpslp->addr) &&
			GetAddrOff (m_addrPC) >= GetAddrOff (lpslp->addr) &&
			GetAddrOff (m_addrPC) <  GetAddrOff (lpslp->addr) + lpslp->cb)
		{
			break;
		}
	}

	// free up the RGSLP if necessary
	if (rgslp)
		SHFree( (LPV)rgslp );

	// quit if we didn't find any matches
	if (islp >= cslp)
		goto L_Exit;

	// take a guess at where to start parsing
	if (m_wLineEnd > 10)
		m_wLineStart = m_wLineEnd - 10;
	else
		m_wLineStart = 1;

	{ // block: parse text
		CTokenStream tokStream(piSrcEdit, m_wLineStart - 1, m_wLineEnd - 1);
		CTokenStreamPos posFunc = tokStream.GetTailPosition();
		while (posFunc.IsValid() && !posFunc.IsAtStartOfLine())
		{
			TXTB *ptxtb = tokStream.GetAt (posFunc);
			if (ptxtb->ibTokMin <= ibCurrent)
				break;
			tokStream.GetPrev (posFunc);
		}

		if (!posFunc.IsValid() || tokStream.GetAt(posFunc)->ibTokMin > ibCurrent)
			goto L_Exit;

		ASSERT(posFunc.IsValid());

		CTokenStreamPos posStart = posFunc;
		BOOL fContinue = TRUE;
		BOOL fLastTokenWasID = FALSE;
		do
		{
			CToken token(tokStream.GetAt(posStart)->tok);
			if (token.IsCxxIdent())
			{
				if (fLastTokenWasID)
					// don't allow two IDs in a row
					goto L_Exit;
				fContinue = TRUE;
				fLastTokenWasID = TRUE;
			}
			else
			{
				fLastTokenWasID = FALSE;
				if (token.IsOperator())
				{
					switch (token)
					{
					case tokOP_ARROW:
					case tokOP_DOT:
					case tokOP_DCOLON:
						fContinue = TRUE;
						break;

					default:
						fContinue = FALSE;
					}
				}
				else
				{
					fContinue = FALSE;
				}
			}

			if (fContinue)
			{
				tokStream.GetPrev (posStart);
			}
		} while (fContinue && posStart.IsValid());

		if (posStart == posFunc)
			// the cursor is not on a relevant token type
			goto L_Exit;

		// move posStart to point to the first token in our call
		if (posStart.IsValid ())
			tokStream.GetNext (posStart);
		else
			posStart = tokStream.GetHeadPosition ();

		CTokenStreamPos posTmp = posStart;

		while (posTmp != posFunc)
		{
			strCall += tokStream.GetStringAt (posTmp);
			tokStream.GetNext (posTmp);
		}
		strCall += tokStream.GetStringAt (posFunc);
	}

	// block: EEParse call
	{
		CXF curCxf (::CxfWatch());
		PCXF pCXF = &curCxf;
		USHORT strIndex;
		EESTATUS eestatus;

		eestatus = EEParseBP(
			(LSZ)(const char *)strCall,
			0x10,	//radixInput,
			TRUE,	//fCaseSensitive,
			pCXF,
			&m_tml,
			0L,
			&strIndex,
			FALSE
			);

		if (eestatus == EEGENERAL)
		{
			// not in current context
			// FUTURE: should check in each DLL??
			goto L_Exit;
		}

		if (eestatus == EEGENERAL)
		{
			HEXE hexe = (HEXE) NULL;

			// We couldn't find the symbol in the current context
			//  and the user didn't specify a context so we'll
			//  try all the other dlls

			// first, we save away the error msg
			EEFreeTML(&m_tml);

			while ((hexe = SHGetNextExe(hexe)) != 0 && eestatus == EEGENERAL)
			{
				CString strBP;

				strBP.Format ("%c,,%s%c%s", chBeginCxtOp, SHGetExeName (hexe), chEndCxtOp, strCall);
				eestatus = EEParseBP (
					(LSZ) (const char *) strBP,
					0x10,		// radixInput
					TRUE,		// fCaseSensitive
					pCXF,
					&m_tml,
					0L,
					&strIndex,
					FALSE
				);

				// If we failed, just delete the TM list
				if ( eestatus != EENOERROR )
				{
					EEFreeTML(&m_tml);
				}
			}
		}

		if (eestatus != NOERROR)  // failed for some other reason
			goto L_Exit;
	}
	m_fTmlInitialized = TRUE;
	bRet = TRUE;

L_Exit:
	if (piSrcEdit)
		piSrcEdit->Release();
	return bRet;
}

BOOL CTraceFunction::GetString(CString & str)
{
	EESTATUS eestatus;
	BOOL fSuccess = FALSE;

	PHTM rgHTM = (PHTM)BMLock(m_tml.hTMList);

	HCXTL hCXTL;
	HTM &htm = rgHTM[0];

	eestatus = EEGetCXTLFromTM(&htm, &hCXTL);
	if (eestatus == EENOERROR)
	{
		// Check for function names
		ETI eti;
		RTMI rtmi = { 0 };
		HTI hTI ;

		// Request information about "lvalue"ness from the EE.
		rtmi.fLvalue = TRUE;	

		if (EEGetExtendedTypeInfo(&htm, &eti) == EENOERROR &&
			eti == ETIFUNCTION &&
			EEInfoFromTM(&htm, &rtmi, &hTI) == EENOERROR)
		{
			ASSERT( hTI != ( HTI ) NULL );

			// Get the TM Info structure.
			PTI pTI = ( PTI ) BMLock(hTI);
			ASSERT(pTI != NULL);

			// can't do anything with function calls
			if (!pTI->fFunction)
			{
				PCXTL pCXTL = (PCXTL) BMLock (hCXTL);

				ASSERT (pCXTL->cHCS >= 1);

				char rgch[257];
				HSYM hSym = pCXTL->rgHCS[pCXTL->cHCS - 1].hSym;

				if (hSym != NULL && SHGetSymName (hSym, rgch))
				{
					str = rgch;
					fSuccess = TRUE;
				}

				BMUnlock (hCXTL);
				EEFreeCXTL (&hCXTL);
			}

			// Free up the resources.
			BMUnlock(hTI);
			EEFreeTI(&hTI);
		}
	}

	BMUnlock (m_tml.hTMList);

	return fSuccess;
}

BOOL CTraceFunction::CompareCxt(PCXT pCXT)
{
	EESTATUS eestatus;
	BOOL fFound = FALSE;

	PHTM rgHTM = (PHTM) BMLock(m_tml.hTMList);

	HSYM hSym;
	UOFF32 uoff = SHGetNearestHsym (
		SHpADDRFrompCXT (pCXT),
		SHHMODFrompCXT (pCXT),
		EECODE, &hSym );

	for (UINT iCXT = 0; !fFound && iCXT < m_tml.cTMListAct; iCXT++)
	{
		HCXTL hCXTL;

		eestatus = EEGetCXTLFromTM (&rgHTM[iCXT], &hCXTL);
		if (eestatus == EENOERROR)
		{
			CXT cxt;
			ADDR addrT = {0};
			ushort usSegType = EEANYSEG;

			// Get the BP address from TM...
			BPADDRFromTM (&rgHTM[iCXT], &usSegType, &addrT);

			// And then the context from the addr...
			if (!SHSetCxt (&addrT, &cxt))
			{
				// Build the cxt
				cxt = *(PCXT) BMLock (hCXTL);
				BMUnlock (hCXTL);
			}
			// Free the allocated space from the EE
			EEFreeCXTL (&hCXTL);

			if (pCXT->hProc == cxt.hProc)
			{
				fFound = TRUE;
			}
		}
	}

	BMUnlock (m_tml.hTMList);

	return fFound;
}
