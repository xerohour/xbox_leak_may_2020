#include "stdafx.h"
#include "mfcappwz.h"
#include "codegen.h"
#include "myaw.h"
#include "lang.h"
#include "symbols.h"
#ifdef VS_PACKAGE
#include <direct.h>
#endif


/////////////////////////////////////////////////////////////////////////////
// This points to the one CProjGen instance available.  If there isn't one,
//  this is NULL.  This is used in CProjGen's GetCurrent & SetCurrent
static CProjGen* s_pProjGen = NULL;

// Keeps track of all the files currently on the include stack.  This exists
//  solely to check for recursive includes that would cause a stack overflow.
IncludeStack g_IncludeStack;

// Is set if the filename has a prepended '*' in newproj.inf, indicating
//  that we should bypass the extension and load the template directly
//  from appwiz.
BOOL g_bLoadFromAppwiz = FALSE;

/////////////////////////////////////////////////////////////////////////////
// Random helper C functions

void CreateDirectory(const char* szDir, BOOL bChangeTo)
{
    struct _stat st;
    if (_stat(szDir, &st) == 0)
    {
        // something already exists
        if (!(st.st_mode & _S_IFDIR))
            ReportAndThrow(IDP_DIRECT_IS_FILE, szDir);
    }
    else
    {
		CDir dir;
		dir.CreateFromString(szDir);
		if (!dir.CreateOnDisk())
			ReportAndThrow(IDP_DIRECT_CREATE, szDir);
    }

    if (bChangeTo)
    {
        // change to that directory
        if (_chdir(szDir) != 0)
            ReportAndThrow(IDP_DIRECT_CHANGE, szDir);
    }
}

const char* RemoveDot(const char* szFilename)
{
	if (szFilename[0] == '.' && szFilename[1] == '\\')
		return szFilename+2;
	else
		return szFilename;
}

inline void AddDotIfNecessary(CString& strFileName)
{
	if (strFileName.Find('\\') == -1)
		// Name doesn't have a path, so prepend ".\". This works around some
		//  weird Novell NetWare bug which causes the filename to be searched
		//  along some other environment path rather than searching cwd first
		strFileName = ".\\" + strFileName;
}


void MyMessageBox(LPCTSTR szPrompt)
{
	ASSERT (theDLL.m_pMainDlg != NULL);
	ASSERT (theDLL.m_pMainDlg->m_pConfirmDlg != NULL);
	theDLL.m_pMainDlg->m_pConfirmDlg->EnableWindow(FALSE);
	AfxMessageBox(szPrompt);
	theDLL.m_pMainDlg->m_pConfirmDlg->EnableWindow(TRUE);
}

// Note that the following function is duplicated in...
//
//	ide\pkgs\cpp\addclass.cpp
//	ide\wizards\appwiz\codegen.cpp
//	ide\wizards\clswiz\srcio.cpp
//
//	If you make a change to it in one place, please modify it in all places.
//	Yes, this just should be in the shell, but it's too late to get a change
//	like that in place.  -mgrier 12/4/96
static CString ConstructFileNameInclusionSymbol(const CString &rstrFileName)
{
	// Construct a symbol name corresponding to the current file name.  Each
	// character could yield up to 6 characters in the output.
	char rgchFileNameSymbol[(MAX_PATH * 6) + (sizeof(GUID) * 3) + 1];
	const char *pchInput = rstrFileName;
	char *pchOutput = rgchFileNameSymbol;
	char ch;

	*pchOutput++ = 'A';
	*pchOutput++ = 'F';
	*pchOutput++ = 'X';
	*pchOutput++ = '_';

	// A lot of filenames are expressed ".\foo.h", so skip past the .\ part.
	if (*pchInput == '.' && *(pchInput + 1) == '\\')
	{
		pchInput += 2;
	}

	// Keep track of whether any alphanumeric characters at all were found. If
	// there aren't any, we just skip the whole filename thing.  (per discussions
	// with anitag.)  -mgrier 12/3/96

	BOOL fAnyAlphanumerics = FALSE;

	while ((ch = *pchInput++) != '\0')
	{
		static char rgchHexChars[] = "0123456789ABCDEF";

		if (((ch >= 'A') && (ch <= 'Z')) ||
			((ch >= 'a') && (ch <= 'z')) ||
			((ch >= '0') && (ch <= '9')))
		{
			*pchOutput++ = (char)toupper(ch);
			fAnyAlphanumerics = TRUE;
		}
#if !defined(DONT_USE_GUID_IN_INCLUSION_SYMBOL_NAME)
//
// If the GUID is being used for uniqueness, we don't have to mangle
// the name as much; just pass letters and numbers through, and turn
// everything else into underscores.  -mgrier 9/30/96

		else if (isleadbyte(ch))
		{
			// We don't output anything for multibyte characters.
			if (*pchInput != '\0')
				pchInput++;
		}
		else if ((ch == '.') || (ch == '_'))
			*pchOutput++ = '_';
#else
		else if (isleadbyte(ch))
		{
			*pchOutput++ = '_';
			*pchOutput++ = rgchHexChars[(ch >> 4) & 0xf];
			*pchOutput++ = rgchHexChars[ch & 0xf];

			if ((ch = *pchInput) != '\0')
			{
				*pchOutput++ = rgchHexChars[(ch >> 4) & 0xf];
				*pchOutput++ = rgchHexChars[ch & 0xf];
				pchInput++;
			}

			*pchOutput++ = '_';
		}
		else if ((ch == '_') || (ch == '.'))
		{
			*pchOutput++ = '_';
			*pchOutput++ = '_';
		}
		else
		{
			// Tradeoff: we can generate a mapping which is intuitive but
			//				not one to one (e.g. "file 1.h" => "file_1_h")
			//				or one which is one to one but not intuitive
			//				(e.g. "file 1.h" => "file_41__04_h" assuming
			//				that space maps to _41_ and . to _04_ -- note
			//				the numbers in this example are just totally made
			//				up -mgrier 9/18/96).
			//
			//				I'm picking the non-intuitive one because while
			//				people might complain about the ugly preprocessor
			//				symbols, at least they won't get errors from
			//				naively named symbols.  -mgrier 9/18/96
			*pchOutput++ = '_';
			*pchOutput++ = rgchHexChars[(ch >> 4) & 0xf];
			*pchOutput++ = rgchHexChars[ch & 0xf];
			*pchOutput++ = '_';
		}
#endif // !defined(DONT_USE_GUID_IN_INCLUSION_SYMBOL_NAME)
	}


#if !defined(DONT_USE_GUID_IN_INCLUSION_SYMBOL_NAME)
	if (!fAnyAlphanumerics)
	{
		// There wasn't anything in the name which just passed through.  We'll start
		// over from just after the AFX_
		pchOutput = &rgchFileNameSymbol[4];
	}

	// If you want to get rid of the adding a GUID thing, just remove the lines
	// from here to the *pchOutput = '\0'; line.  -mgrier 9/24/96
	GUID guidUniqueness;

	HRESULT hr = ::CoCreateGuid(&guidUniqueness);
	if (SUCCEEDED(hr))
	{
		OLECHAR rgochGuid[256];
		CHAR rgchGuid[256];

		::StringFromGUID2(guidUniqueness, rgochGuid, sizeof(rgochGuid) / sizeof(rgochGuid[0]));

		::WideCharToMultiByte(CP_ACP, 0, rgochGuid, -1, rgchGuid, sizeof(rgchGuid), NULL, NULL);

		*pchOutput++ = '_';

		pchInput = rgchGuid;

		while ((ch = *pchInput++) != '\0')
		{
			if (isalnum(ch))
				*pchOutput++ = (char)toupper(ch);
			else
				*pchOutput++ = '_';
		}
	}
#endif // !defined(DONT_USE_GUID_IN_INCLUSION_SYMBOL_NAME)

	*pchOutput = '\0';

	return CString(rgchFileNameSymbol);
}

/////////////////////////////////////////////////////////////////////////////
// The State class.

State::State()
{
    m_iCurState = 0;
    m_pCurState = &m_states[m_iCurState];
    m_pCurState->m_bOutput = TRUE;
    m_pCurState->m_bOneOutput = TRUE;
    m_pCurState->m_mode = idle;
	m_pCodeGen = NULL;
}

static BOOL Match(LPCTSTR &lpch, const TCHAR* szMatch)
{
    int nLen = _tcslen(szMatch);
    if (_tcsncmp(lpch, szMatch, nLen) != 0)
        return FALSE;
    lpch += nLen;
    return TRUE;
}

// Special processing for $$IF, $$ELIF, $$ELSE, $$ENDIF
BOOL State::Process(LPCTSTR lpch, LPCTSTR *plpchNext)
{
	ASSERT(m_pCodeGen != NULL);
	ASSERT(m_pCodeGen->m_pSymbols != NULL);

	CMapStringToString* pSymbols = m_pCodeGen->m_pSymbols;
    if (*lpch != '$' || !Match(lpch, "$$"))
        return FALSE;

    if (Match(lpch, "//"))
    {
        // comment - ignore it
    }
    else if (Match(lpch, "IF("))
    {
        if (m_pCurState->m_mode != idle)
        {
            // allow nested IF
            if (++m_iCurState >= MAXSTATE)
            {
				m_pCodeGen->ReportAndThrow(IDS_CG_IF_TOO_DEEP);
            }
            m_states[m_iCurState] = m_states[m_iCurState-1];    // copy state
            m_pCurState = &m_states[m_iCurState];
            // cur set to new values below
            if (!m_pCurState->m_bOutput)
            {
                // ignore till next endif
                m_pCurState->m_bOneOutput = TRUE;       // pretend we did
                m_pCurState->m_bOutput = FALSE;
                m_pCurState->m_mode = inIf;
     			m_pCurState->m_nLine = m_pCodeGen->m_nLine;
                return TRUE;
            }
        }
Lif:
        // find the symbol
        LPCTSTR lpchEnd = _tcschr(lpch, ')');
        if (lpchEnd == NULL)
        {
			m_pCodeGen->ReportAndThrow(IDS_CG_BAD_IF);
        }

        CString value;
        BOOL finalAnswer = FALSE;
        CString flag(lpch, lpchEnd - lpch);
        int ncurr=0;
        CString currFlag;
        while (ncurr < lpchEnd - lpch)
        {
			BOOL bNegated = FALSE;
            currFlag = "";
            // Get next flag... first, is it negated?
			if (flag[ncurr] == '!')
			{
				bNegated = TRUE;
				ncurr++;
			}
			// ...get flag name
            for (; ncurr < lpchEnd-lpch
                && flag[ncurr] != '|' && flag[ncurr] != ' '; ncurr++)
			{
                currFlag += flag[ncurr];
			}
			ASSERT(pSymbols);
            if ((bNegated == FALSE && m_pCodeGen->DoesSymbolExist(currFlag))
				|| (bNegated == TRUE && !m_pCodeGen->DoesSymbolExist(currFlag)))
            {
                finalAnswer = TRUE;
                break;
            }
            // Reset ncurr to beginning of next flag
            for (; ncurr < lpchEnd-lpch &&
                (flag[ncurr] == '|' || flag[ncurr] == ' '); ncurr++);
        }
        m_pCurState->m_bOneOutput = m_pCurState->m_bOutput = finalAnswer;
        m_pCurState->m_mode = inIf;
        m_pCurState->m_nLine = m_pCodeGen->m_nLine;
    }
    else if (Match(lpch, "ELIF("))
    {
        if (m_pCurState->m_mode != inIf)
        {
			m_pCodeGen->ReportAndThrow(IDS_CG_BAD_ELIF);
        }
        if (m_pCurState->m_bOneOutput)
        {
            // already had output (ignore this until ENDIF)
            m_pCurState->m_bOutput = FALSE;
        }
        else
        {
            // still looking for TRUE conditional
            goto Lif;
        }
    }
    else if (Match(lpch, "ELSE"))
    {
        if (m_pCurState->m_mode != inIf)
        {
			m_pCodeGen->ReportAndThrow(IDS_CG_BAD_ELSE);
        }
        m_pCurState->m_mode = inElse;
        m_pCurState->m_bOneOutput = m_pCurState->m_bOutput = !m_pCurState->m_bOneOutput;
    }
    else if (Match(lpch, "ENDIF"))
    {
        if (m_pCurState->m_mode != inIf && m_pCurState->m_mode != inElse)
        {
			m_pCodeGen->ReportAndThrow(IDS_CG_BAD_ENDIF);
        }

        if (m_iCurState > 0)
        {
            // restore previous state
            m_pCurState = &m_states[--m_iCurState];
        }
        else
        {
            ASSERT(m_pCurState == &m_states[0]);
            m_pCurState->m_mode = idle;
            m_pCurState->m_bOutput = TRUE;
        }
    }
    else
    {
        // symbol at the start of a line - not special
        return FALSE;
    }

    return TRUE; // processed - ignore the rest of this line
}

/////////////////////////////////////////////////////////////////////////////
// The CCodeGen class-- handles one unit (e.g., file) of a project's
//   generation.


// Construction: If there's currently a CProjGen, have it initialize this
//  codegen with the current "loop state".  If CProjGen is parsing through
//  a loop in newproj.inf, the codegen must be aware of this, so it looks
//  up all its variables correctly, and so it can balk an attempt to further
//  loop within codegen's template.
CCodeGen::CCodeGen()
{
	m_pSymbols = NULL;
	m_pOutput = NULL;
	m_bLoopInProjGen = FALSE;
	m_lpszBeginLoop = NULL;
	m_nBeginLoop = -1;
	m_nIteration = -1;
	m_nTotalIterations = 0;
	m_bGeneratingWorklist = FALSE;
	m_nLine = 0;
	CProjGen::InitCodeGen(this);
	ASSERT(!m_strTemplateName.IsEmpty());
}

// This ties together the codegen class.  It coordinates the master parse through
//  the template and calls to process the directives & symbols.
void CCodeGen::Go(LPCTSTR lpszInput, DWORD dwSize, OutputStream* pOutput, CMapStringToString* pSymbols)
{
	if (lpszInput == NULL)
	{
		m_nLine = 0;
		ReportAndThrow(IDP_NULL_TEMPLATE);
		return;
	}

	// Initialize CCodeGen's member variables
	m_pSymbols = pSymbols;
	m_pOutput = pOutput;
	m_nLine = 1;
	m_state.m_pCodeGen = this;

    // parsed textual output
	LPCTSTR lpszBeginning = lpszInput;	// Remember beginning to keep track of size

    // output a line at a time (zero terminated)
    while ((DWORD) (lpszInput-lpszBeginning+1) <= dwSize && *lpszInput != '\0')
    {
        // find the end of the line
        LPCTSTR lpchEnd = _tcschr(lpszInput, '\n');
        if (lpchEnd == NULL)
        {
			m_pSymbols = NULL;
			ReportAndThrow(IDS_CG_MISSING_EOL);
        }
        int nLen = lpchEnd - lpszInput;
        ASSERT(*lpchEnd == '\n');
        lpchEnd = _tcsinc(lpchEnd);  // skip ending '\n'

        if (!m_state.Process(lpszInput, &lpchEnd))
        {
            CString strBuff = "";

            if (m_state.IsOutput())
            {
				// First, deal with looping.  If we loop back up, lpszInput will
				//  update to reflect this, and ProcessControl returns TRUE.
				if (ProcessControl(lpszInput, lpchEnd))
					continue;

                Translate(strBuff, lpszInput);
                m_pOutput->WriteLine(strBuff);
            }
        }
        lpszInput = lpchEnd;
		m_nLine++;
    }

	// Check for EOF type of errors.  First, did we hit a NULL before EOF?
	if (IsUserAWX() && *lpszInput == '\0' && (DWORD) (lpszInput-lpszBeginning+1) <= dwSize)
	{
		ReportAndThrow(IDS_CG_STRAY_NULL);
	}

	// Second: EOF before $$ENDIF
    if (m_state.m_iCurState != 0 || m_state.m_pCurState->m_mode != State::idle)
    {
		m_nLine = m_state.m_pCurState->m_nLine;
		ReportAndThrow(IDS_CG_NO_ENDIF);
	}

	// Third: EOF before $$ENDLOOP
	if (!m_bLoopInProjGen &&
		(m_nBeginLoop != -1 || m_lpszBeginLoop != NULL 
			|| m_nIteration != -1 || m_nTotalIterations != 0))
	{
			// Test our own sanity... if any of these are set, they should all be set.
			ASSERT(m_nBeginLoop != -1 && m_lpszBeginLoop != NULL 
				&& m_nIteration != -1 && m_nTotalIterations != 0);
			ReportAndThrow(IDS_CG_EOF_IN_LOOP);
	}

	// De-initialize member variables
	m_pSymbols = NULL;
	m_pOutput = NULL;
	m_state.m_pCodeGen = NULL;
}

// Seeks lpch to the beginning of the line after the next $$ENDLOOP.  Used
//  in $$BEGINLOOP(0).
void CCodeGen::ScanToAfterEndloop(LPCTSTR& lpch)
{
	while (*lpch != NULL && !Match(lpch, "$$ENDLOOP"))
	{
		lpch = _tcschr(lpch, '\n') + 1;
		m_nLine++;
	}

	if (*lpch == NULL)
		return;

	lpch = _tcschr(lpch, '\n') + 1;
	m_nLine++;
}
	

// This checks to see if there's a control directive ($$BEGINLOOP, $$ENDLOOP, $$INCLUDE, $$SET_DEFAULT_LANG)
//  to process, and if so, it processes it.
BOOL CCodeGen::ProcessControl(LPCTSTR& lpch, LPCTSTR lpchEnd)
{
	LPCTSTR lpchOrig = lpch;

    if (*lpch != '$' || !Match(lpch, "$$"))
        return FALSE;

	BOOL bReturn = TRUE;
	if (Match(lpch, "BEGINLOOP("))
	{
		if (m_nIteration != -1 || m_nBeginLoop != -1
			|| m_nTotalIterations != 0 || m_lpszBeginLoop != NULL)
		{
			// Check our own sanity: if any of these are set, they
			//  must all be set
			ASSERT (m_nIteration != -1 && m_nBeginLoop != -1
				&& m_nTotalIterations != 0 && m_lpszBeginLoop != NULL);
			ReportAndThrow(IDS_CG_LOOP_IN_LOOP);
		}
        LPCTSTR lpchEndVar = _tcschr(lpch, ')');
        if (lpchEndVar == NULL || lpchEndVar - lpch == 0)
        {
			ReportAndThrow(IDS_CG_BAD_BEGINLOOP);
        }

        CString strIterationsKey(lpch, lpchEndVar - lpch);
		CString strIterationsVal;
		if (!GetSymbol(strIterationsKey, strIterationsVal))
		{
			ReportAndThrow(IDS_CG_UNDEF_IN_BEGINLOOP);
		}
		m_nTotalIterations = atoi(strIterationsVal);
		if (m_nTotalIterations < 0)
		{
			ReportAndThrow(IDS_CG_BAD_ITERATIONS);
		}
		if (m_nTotalIterations == 0)
		{
			// A $$BEGINLOOP(0) should iterate zero times.  Thus, scan
			//  for the matching $$ENDLOOP and ignore everything before it
			lpch = lpchEnd;
			m_nLine++;
			ScanToAfterEndloop(lpch);
		}
		else
		{
			// Here we have a $$BEGINLOOP(positive integer)
			m_nIteration = 0;
			lpch = m_lpszBeginLoop = lpchEnd;
			m_nBeginLoop = ++m_nLine;
			// If we're generating the worklist, *retain* the beginloop & endloop directives
			//  while unfolding the loop, so that during GenerateFromWorklist, CProjGen
			//  will know at what points to up its iteration count.
			if (m_bGeneratingWorklist)
				m_pOutput->WriteLine("$$BEGINLOOP");
		}
	}
	else if (Match(lpch, "ENDLOOP"))
	{
		m_nIteration++;
		if (m_nIteration == m_nTotalIterations)
		{
			// Here, we're done iterating
			m_nIteration = -1;
			m_nTotalIterations = 0;
			m_lpszBeginLoop = NULL;
			m_nBeginLoop = -1;
			lpch = lpchEnd;
			m_nLine++;
			// If we're generating the worklist, *retain* the beginloop & endloop directives
			//  while unfolding the loop, so that during GenerateFromWorklist, CProjGen
			//  will know at what points to up its iteration count.
			if (m_bGeneratingWorklist)
				m_pOutput->WriteLine("$$ENDLOOP");
		}
		else
		{
			if (m_lpszBeginLoop == NULL || m_nBeginLoop == -1
				|| m_nIteration == -1 || m_nTotalIterations == 0)
			{
				// If one is unset, all must be unset
				ASSERT (m_lpszBeginLoop == NULL && m_nBeginLoop == -1
					&& m_nIteration == -1 && m_nTotalIterations == 0);
				ReportAndThrow(IDS_CG_EL_NO_BL);
			}
			lpch = m_lpszBeginLoop;
			m_nLine = m_nBeginLoop;
			if (m_bGeneratingWorklist)
				m_pOutput->WriteLine("$$BEGINLOOP");
		}
	}
	else if (Match(lpch, "INCLUDE("))
	{
        // Bad include syntax?
		LPCTSTR lpchEndVar = _tcschr(lpch, ')');
        if (lpchEndVar == NULL || lpchEndVar - lpch == 0)
        {
			ReportAndThrow(IDS_CG_BAD_INCLUDE);
        }

        // Bad include arg?
		CString strIncludeKey(lpch, lpchEndVar - lpch);
		CString strIncludeVal;
		if (!GetSymbol(strIncludeKey, strIncludeVal))
		{
			ReportAndThrow(IDS_CG_UNDEF_IN_INCLUDE);
		}

		// Recursive include call?
		if (g_IncludeStack.Lookup(strIncludeVal))
		{
			g_IncludeStack.Clear();
			ReportAndThrow(IDS_CG_RECURSIVE_INCLUDE, strIncludeVal);
		}

		// Are we inside a loop?  If so, transfer the insideness to the next
		//  codegen that gets created
		CProjGen* pProjGen = CProjGen::GetCurrent();
		if (m_nIteration != -1)
			pProjGen->SetCurrentIteration(m_nIteration);

		LoadAndProcessTemplate(strIncludeVal, m_pOutput);

		// If we're inside a loop, reset the projgen
		if (m_nIteration != -1)
			pProjGen->ResetCurrentIteration();

		lpch = lpchEnd;
	}
	else if (Match(lpch, "SET_DEFAULT_LANG("))
	{
		LPCTSTR lpchEndVar = _tcschr(lpch, ')');
		if (lpchEndVar == NULL || lpchEndVar - lpch == 0)
		{
			ReportAndThrow(IDS_CG_BAD_SETDEFLANG);
		}
		CString strLangKey(lpch, lpchEndVar - lpch);
		CString strLangVal;
		if (!GetSymbol(strLangKey, strLangVal))
		{
			ReportAndThrow(IDS_CG_UNDEF_IN_SETDEFLANG);
		}
		// Inform the langDlls class that the default language has changed.  Only
		//  show error if this is not an extension (since extensions can get
		//  localized templates independent of the appwz*.dll's).
		if (!langDlls.SetDefaultLang(strLangVal) && !IsUserAWX())
			ReportAndThrow(IDS_CG_BADLANG_IN_SETDEFLANG, strLangVal);

		lpch = lpchEnd;

		// If we're generating the worklist, *retain* the set_default_lang directives
		//  while unfolding the loop, so that during GenerateFromWorklist, CProjGen
		//  will know at what points to set the default lang
		if (m_bGeneratingWorklist)
		{
			m_pOutput->WriteLine("$$SET_DEFAULT_LANG" + strLangVal);
		}
	}
    else if (Match(lpch, "NEWPROJ("))
    {
        LPCTSTR lpchEndVar = _tcschr(lpch, ')');
        if (lpchEndVar == NULL || lpchEndVar - lpch == 0)
        {
            ReportAndThrow(IDS_CG_BAD_NEWPROJ);
        }
        
        // Expand the macro, if any
        CString strNewProjKey(lpch, lpchEndVar - lpch);
        CString strNewProjVal;
        if (!GetSymbol(strNewProjKey, strNewProjVal))
        {
            ReportAndThrow(IDS_CG_UNDEF_IN_NEWPROJ);
        }

        // Are we inside a loop?  If so, throw a tantrum
        if (m_nIteration != -1)
            ReportAndThrow(IDS_CG_LOOP_IN_NEWPROJ);

        // We have to create the new AppWizTermStatus
        ASSERT (pTermStatus->nextAppWizTermStatus == NULL);
        pTermStatus->nextAppWizTermStatus = new AppWizTermStatus;
        pTermStatus = pTermStatus->nextAppWizTermStatus;
        
        // Initialize the variables, otherwise they might contain junk
        pTermStatus->nextAppWizTermStatus = NULL;
        memset(pTermStatus->szFullName, '\0', MAX_PATH);

        // Copy the new name. We can add the full path later on
        ASSERT (_MAX_PATH > strNewProjVal.GetLength());
        strncpy(pTermStatus->szFullName, LPCTSTR(strNewProjVal), strNewProjVal.GetLength());

        
        // If we are generating a worklist, we have to put the
        // NewProj directive back in, so that we can start generating
        // the project properly. Remember we are not sending in the name though
        if (m_bGeneratingWorklist)
            m_pOutput->WriteLine("$$NEWPROJ");

        lpch = lpchEnd;
    }
	else
	{
		lpch = lpchOrig;
		bReturn = FALSE;
	}
	
	return bReturn;
}


#define BUFFER_INC_SIZE 1000   // Amount to increment buffer size

// This helper function is used in Translate to increase the buffer
//  size just before it's about to be exceeded.
void readjustBuf(
    char*& sBuf,        // Points to beginning of buffer
    int& iCurr,         // Index where next character may be inserted into buffer
    CString& pchOut,    // The CString the buffer is created from
    int& iMax)          // Maximum allowed index into buffer
{
    pchOut.ReleaseBuffer(iCurr);
    iMax += BUFFER_INC_SIZE;
    sBuf = pchOut.GetBuffer(iMax + 1);
}

// After all directives have been handled, this takes an ordinary template
//  line, and takes care of the rest (expanding all symbols).
void CCodeGen::Translate(CString& pchOut, LPCTSTR lpch)
{
    char* sBuf = pchOut.GetBuffer(BUFFER_INC_SIZE);
    int iCurr = 0;           // Current index into pchOut.
    int iMax = BUFFER_INC_SIZE - 1;  // Maximum allowed index into buffer
    while (*lpch != '\n' && *lpch != '\0')
    {
        if (lpch[0] == '$' && Match(lpch, "$$"))
        {
            // process it
            LPCTSTR lpchEnd = _tcschr(lpch, '$');
            if (lpchEnd == NULL || lpchEnd[1] != '$')
            {
				ReportAndThrow(IDS_CG_MISSING_DLR);
            }
			
            CString szKey(lpch, lpchEnd - lpch);
            if (szKey.IsEmpty())
            {
                // $$$$ => $$
                if (iCurr + 2 > iMax)
                    readjustBuf(sBuf, iCurr, pchOut, iMax);
                *(sBuf + iCurr++) = '$';
                *(sBuf + iCurr++) = '$';
            }
            else
            {
                CString value;
                if (!GetSymbol(szKey, value))
                {
					ReportAndThrow(IDS_CG_UNDEFINED_SYMBOL, szKey);
                }
                else
                {
                    int nLength = value.GetLength();
                    if (iCurr + nLength + 1 > iMax)     // +1 cuz strcpy copies null
                    {
                        readjustBuf(sBuf, iCurr, pchOut, iMax);
						
                        // We better make sure that "value" isn't so huge that one buffer-
                        //  increase still isn't enough space to allocate for it.
                        if (iCurr + nLength + 1 > iMax)
                        {
                            // Here, a buffer-increase was not enough, so revert to
                            //  the old CString +=.  This should RARELY happen.
                            pchOut.ReleaseBuffer(iCurr);
                            pchOut += value;
                            iCurr += nLength;
							
                            // Now, readjust the buffer by another BUFFER_INC_SIZE
                            iMax = pchOut.GetLength() + BUFFER_INC_SIZE;
                            sBuf = pchOut.GetBuffer(iMax + 1);
                        }
                        else
                        {
                            // Here, one buffer-increase was enough.
                            _tcscpy(sBuf+iCurr, (const TCHAR*) value);
                            iCurr += nLength;
                        }
						
                    }
                    else
                    {
                        // Here, we did not even need a buffer-increase.
                        _tcscpy(sBuf+iCurr, (const TCHAR*) value);
                        iCurr += nLength;
                    }
                }
            }
            lpch = lpchEnd + 2; // skip past second "$$"
        }
		else
        {
            // just a normal character
            // Store in buffer
			if (*lpch != '\r')
			{
				// Don't copy over '\r's
				int nCLength = _tclen(lpch);
            	if (iCurr + nCLength > iMax)
	                readjustBuf(sBuf, iCurr, pchOut, iMax);
            	_tccpy(sBuf + iCurr, lpch);
				iCurr += nCLength;
			}
			lpch = _tcsinc(lpch);
        }
    }
    pchOut.ReleaseBuffer(iCurr);
}

// Returns whether we're currently inside a $$BEGINLOOP, and asserts
//  that the loop-variables aren't contradictary.
BOOL CCodeGen::InLoop()
{
	if (m_nIteration == -1)
	{
		ASSERT(m_nTotalIterations == 0);
		ASSERT(m_lpszBeginLoop == NULL);
		return FALSE;
	}
	else
	{
		ASSERT(m_nTotalIterations != 0);
		ASSERT(m_lpszBeginLoop != NULL);
		return TRUE;
	}
}


// Generates a key name based on the current loop status.
// Example:  If we're in iteration #4,
//				KEY -> KEY_4
//			 If we're not in a loop,
//				KEY -> <empty string>
void CCodeGen::GetLoopKey(LPCTSTR lpszKey, CString& rStrLoopKey)
{
	if (!InLoop())
		rStrLoopKey.Empty();
	else
		rStrLoopKey.Format("%s_%d", lpszKey, m_nIteration);
}

// Look for non-empty string keyed by loop key or verbatim key.
BOOL CCodeGen::DoesSymbolExist(LPCTSTR lpszKey)
{
	ASSERT(m_pSymbols != NULL);
	ASSERT_VALID(m_pSymbols);

	CString strLoopKey;
	GetLoopKey(lpszKey, strLoopKey);

	CString tmp;

	// If we're in a loop and the loop key is there, return TRUE
	if (!strLoopKey.IsEmpty() && m_pSymbols->Lookup(strLoopKey, tmp) && !tmp.IsEmpty())
		return TRUE;

	// Otherwise, just check the key verbatim
	return (m_pSymbols->Lookup(lpszKey, tmp) && !tmp.IsEmpty());
}

// Find first occurrence of value keyed by loop key or verbatim key, even if that
//  first occurrence is an empty string.
BOOL CCodeGen::GetSymbol(LPCTSTR lpszKey, CString& rStrValue)
{
	ASSERT(m_pSymbols != NULL);
	ASSERT_VALID(m_pSymbols);

	CString strLoopKey;
	GetLoopKey(lpszKey, strLoopKey);

	// If we're in a loop and the loop key is there, return TRUE
	if (!strLoopKey.IsEmpty() && m_pSymbols->Lookup(strLoopKey, rStrValue))
		return TRUE;

	// Otherwise, just check the key verbatim
	return (m_pSymbols->Lookup(lpszKey, rStrValue));
}


/////////////////////////////////////////////////////////////////////////////
// Error reporting

// Reports error during code generation, and throws an exception.
//  This reports EXTENSION ERRORS
void CCodeGen::ReportAndThrow(UINT nIDP, LPCTSTR lpszArg)
{
	ASSERT(!m_strTemplateName.IsEmpty());

	// strFormat contains the format of the entire error string,
	//  including context information
	CString strFormat;
	strFormat.LoadString(IDP_GENERAL_ERROR);

	// strMessage is the specific error, including one optional argument
	CString strMessage;
	if (lpszArg)
		AfxFormatString1(strMessage, nIDP, lpszArg);
	else
		strMessage.LoadString(nIDP);

	// Now generate the actual error string.
	CString strPrompt;
	strPrompt.Format(strFormat, (LPCTSTR) m_strTemplateName, m_nLine, (LPCTSTR) strMessage);

    MyMessageBox(strPrompt);

    AfxThrowUserException();
}

// Reports generic EXTENSION ERRORS
void ReportErrorExt(UINT nIDP, LPCTSTR lpszArg /* = NULL */)
{
	// strMessage is the specific error, including one optional argument
	CString strMessage;
	if (lpszArg)
		AfxFormatString1(strMessage, nIDP, lpszArg);
	else
		strMessage.LoadString(nIDP);

	// Now generate the actual error string
	CString strPrompt;
	AfxFormatString1(strPrompt, IDP_EXTENSION_ERROR, (LPCTSTR) strMessage);
	
	MyMessageBox(strPrompt);
}

// Reports generic EXTENSION ERRORS and throws a user exception
void ReportAndThrowExt(UINT nIDP, LPCTSTR lpszArg /* = NULL */)
{
	ReportErrorExt(nIDP, lpszArg);
    AfxThrowUserException();
}

// Reports any generic error and throws a user exception
void ReportAndThrow(UINT nIDP, LPCTSTR lpszArg)
{
    CString strPrompt;
    //_tcsupr(lpszArg);     // cheat - force reported names to upper case
    AfxFormatString1(strPrompt, nIDP, lpszArg);
    MyMessageBox(strPrompt);

    AfxThrowUserException();
}

/////////////////////////////////////////////////////////////////////////////
// The CProjGen class-- handles the whole of project generation.

// Helper used in CProjGen's Go below.  Takes a worklist and makes sure
//  the files we're about to generate don't already exist.
void CheckExistingFromWorkList(CStringList& list)
{
    pTermStatus = &g_TermStatus;

    // first check we can create the files and directories
    POSITION pos = list.GetHeadPosition();
    while (pos)
    {
        CString const& s = list.GetNext(pos);

        if (s.IsEmpty() || s == "$$BEGINLOOP" || s == "$$ENDLOOP"
			|| s.Left(18) == "$$SET_DEFAULT_LANG")
            continue;   // just a comment or control directive

        if (s == "$$NEWPROJ") 
        {
            pTermStatus = pTermStatus->nextAppWizTermStatus;
            struct _stat st;
            if (_stat(pTermStatus->szFullName, &st) == 0 && !(st.st_mode & _S_IFDIR))
            {
                // it exists, but is not a directory
                ReportAndThrow(IDP_DIRECT_IS_FILE, pTermStatus->szFullName);
            }
            // TODO: ShankarV: SHOULD WE NOT CHANGE DIR AT THIS POINT???
            // so that the rest of the directory creation will be 
            // part of the new project directory.
            continue;
        }
            
        if (s[0] == '/')
        {
            // if the name exists it must be a directory
            CString dirName = s.Mid(1);
            struct _stat st;
            if (_stat(dirName, &st) == 0 && !(st.st_mode & _S_IFDIR))
            {
                // it exists, but is not a directory
                ReportAndThrow(IDP_DIRECT_IS_FILE, dirName);
            }
            continue;
        }
        int iSplit = s.Find('\t');
        if (iSplit == -1)
        {
            TRACE("illegal work list item '%s'\n", (const char*)s);
			ReportAndThrowExt(IDS_CG_BAD_WL_ITEM, s);
        }
        CString fileName = s.Mid(iSplit+1);
        struct _stat st;
        if (_stat(fileName, &st) == 0)
        {
            // it already exists
            UINT nIDP = (st.st_mode & _S_IFDIR) ?
                IDP_FILE_IS_DIRECT : IDP_FILE_EXISTS;
            ReportAndThrow(nIDP, fileName);
        }
    }

    // Reinitialize pTermStatus to the beginning of the list
    pTermStatus = &g_TermStatus;
}

// Helper used in CProjGen's Go below.  Takes a worklist and makes sure
//  there are no duplicate filenames
void CheckDuplicateInWorkList(CStringList& list)
{
	
    // TODO: ShankarV: Allow dup filnames in muliple projects
    // First make list of just the to-be-generated filenames
	CStringList listJustFiles;
	POSITION pos = list.GetHeadPosition();
    while (pos != NULL)
    {
        CString const& s = list.GetNext(pos);

        if (s.IsEmpty() || s == "$$BEGINLOOP" || s == "$$ENDLOOP" || s =="$$NEWPROJ"
			|| s.Left(18) == "$$SET_DEFAULT_LANG")
            continue;   // just a comment or control directive

        if (s[0] == '/')
			// Skip directory names
            continue;

        int iSplit = s.Find('\t');
		// This should have been caught in CheckExistingFromWorklist
		ASSERT (iSplit != -1);
        CString fileName = s.Mid(iSplit+1);
		listJustFiles.AddTail(fileName);
	}

    // TODO: ShankarV: Move this piece of code into a separate file so that
    // this may be called everytime we generate you new project
    pos = listJustFiles.GetHeadPosition();
	while (pos != NULL)
	{
		// Get next filename to check for duplicates
		CString strFile = listJustFiles.GetNext(pos);

		// Scan all filenames following strFile in the list, and make sure
		//  none of them duplicate strFile.
		POSITION pos2 = pos;
		while (pos2 != NULL)
		{
			CString strFile2 = listJustFiles.GetNext(pos2);
			if (!strFile.CompareNoCase(strFile2))
	            ReportAndThrow(IDP_DUPLICATE_FILEGEN, strFile);
		}

    }

}

// This coordinates the entire project generation process: It gets a worklist,
//  parses through it, and creates CCodeGens to generate each file.
void CProjGen::Go(CWnd* pProjStatus, CWnd* pFileStatus)
{
	ASSERT(pProjStatus != NULL);
	ASSERT(pFileStatus != NULL);

	m_pProjStatus = pProjStatus;
	m_pFileStatus = pFileStatus;

    // first get a work-list
	ListStream workList;

	m_bGeneratingWorklist = TRUE;
	LoadAndProcessTemplate("newproj.inf", &workList);
	m_bGeneratingWorklist = FALSE;
	m_strTemplateName.Empty();

    CheckExistingFromWorkList(workList.list);
	CheckDuplicateInWorkList(workList.list);
   	GenerateFromWorkList(workList.list);
}

// Takes a codegen and fills it with the current "loop" state, and the
//  current template name.  This is called from the codegen's constructor.
void CProjGen::InitCodeGen(CCodeGen* pCodeGen)
{
	ASSERT(!pCodeGen->InLoop());

	CProjGen* pProjGen = GetCurrent();
	if (pProjGen != NULL)
	{
		pCodeGen->m_bGeneratingWorklist = pProjGen->m_bGeneratingWorklist;
		if(pProjGen->InLoop())
		{
			pCodeGen->m_lpszBeginLoop = "";
			pCodeGen->m_nIteration = pProjGen->m_nIteration;
			pCodeGen->m_nTotalIterations = 1;
			pCodeGen->m_bLoopInProjGen = TRUE;
		}
		pCodeGen->m_strTemplateName = pProjGen->m_strTemplateName;
		ASSERT(!pCodeGen->m_strTemplateName.IsEmpty());
	}
}

// There is at most one project generator at a given time.  GetCurrent() and
//  SetCurrent() manipulate this pointer.

CProjGen* CProjGen::GetCurrent()
{
	return s_pProjGen;
}

void CProjGen::SetCurrent(CProjGen* pProjGen)
{
	ASSERT(pProjGen == NULL || s_pProjGen == NULL);		// Can have only one CProjGen
	s_pProjGen = pProjGen;
}

// This reads the flags to the left of a filename in newproj.inf, and
//  returns an int with the corresponding bits set.
// Examples:
//  =root.ico		Use CopyTemplate rather than ProcessTemplate
//  *confirm.inf	Load directly from appwiz's resources-- don't try extension's
//	+root.cpp		Add the file root.cpp generates to the project
#define FF_BINARY				0x01
#define FF_LOAD_FROM_APPWIZ		0x02
#define FF_ADD_TO_PROJECT		0x04
#define FF_NOT_IN_PROJECT		0x08
#define FF_EXCLUDE_FROM_BUILD  0x10
#define FF_HELP_FILE		   0x20
#define FF_RESOURCE_FILE	   0x40
#define FF_TEMPLATE_FILE	   0x80
int GetFileFlags(CString& strFileName)
{
	int nFlags = 0;
	int i = 0;
	BOOL bLoop = TRUE;
	do
	{
		char c = strFileName[i++];
		switch ( c )
		{
			case '=' :
				nFlags |= FF_BINARY;
				break;
			case '*' :
				nFlags |= FF_LOAD_FROM_APPWIZ;
				break;
			case '+' :
				nFlags |= FF_ADD_TO_PROJECT;
				break;
			case '-' : // REVIEW
				nFlags |= FF_NOT_IN_PROJECT;
				break;
			case '!' : // REVIEW: make sure not valid first char
				nFlags |= FF_EXCLUDE_FROM_BUILD;
				break;
			case '?' :
				nFlags |= FF_HELP_FILE;
				break;
			case '#' :
				nFlags |= FF_TEMPLATE_FILE;
				break;
			case ':' :
				nFlags |= FF_RESOURCE_FILE;
				break;
			default:
				bLoop = FALSE;	// done
				break;
		}
	} while (bLoop);

	strFileName = strFileName.Mid(i-1);
	return nFlags;
}


// Takes a generated worklist, and generates the project files from it.
void CProjGen::GenerateFromWorkList(CStringList& list)
{
    // save original directory
    char szOrigDir[_MAX_DIR];
    if (_getcwd(szOrigDir, _MAX_DIR) == NULL)
        szOrigDir[0] = '\0';

    // Initialize AppWizTermStatus
    pTermStatus = &g_TermStatus;

    if (m_pProjStatus != NULL)
        m_pProjStatus->SetWindowText(projOptions.m_strProj);
    // now for each item in the work list, do it
	pTermStatus->strDefFile.Empty();
	//pTermStatus->strClwFile.Empty();
    POSITION pos = list.GetHeadPosition();
    while (pos)
    {
        CString const& s = list.GetNext(pos);

        if (s.IsEmpty())
            continue;   // just a comment

		if (s == "$$BEGINLOOP")
		{
			m_nIteration++;
			continue;
		}

		if (s == "$$ENDLOOP")
		{
			m_nIteration = -1;
			continue;
		}

        if (s == "$$NEWPROJ")
        {
            pTermStatus = pTermStatus->nextAppWizTermStatus;
            pTermStatus->strDefFile.Empty();
            // pTermStatus->strClwFile.Empty();

            // restore to original directory in case a $$NEWPROJ directive 
            // was already used
            if (szOrigDir[0] != '\0')
                _chdir(szOrigDir);
			
            // Create Directory and change to it
            CreateDirectory(pTermStatus->szFullName, TRUE);
            continue;
        }

		if (s.Left(18) == "$$SET_DEFAULT_LANG")
		{
			// Shift over strlen("$$SET_DEFAULT_LANG") characters
			//  to get to language suffix
			LPCTSTR szLangVal = ((LPCTSTR) s) + 18;

			// Now make that be the default language.  Since we made sure
			//  this worked while processing newproj.inf to make this worklist,
			//  the language string should be valid.
			VERIFY (langDlls.SetDefaultLang(szLangVal));
			continue;
		}

        if (s[0] == '/')
        {
            // create a new sub-directory (don't change to it)
            CreateDirectory(s.Mid(1), FALSE);
            continue;
        }
        // work list format is : resname\tfilename

        int iSplit = s.Find('\t');
        if (iSplit == -1)
        {
			ReportAndThrowExt(IDS_CG_BAD_WL_ITEM, (const TCHAR*) s);
        }
        CString resName = s.Left(iSplit);

		// Set flags on this file
		int nFileFlags = GetFileFlags(resName);
		BOOL bBinary = ((nFileFlags & FF_BINARY) != 0);
		BOOL bExclude = ((nFileFlags & FF_EXCLUDE_FROM_BUILD) != 0);
		BOOL bHelpFile = ((nFileFlags & FF_HELP_FILE) != 0);
		BOOL bResFile = ((nFileFlags & FF_RESOURCE_FILE) != 0);
		BOOL bTemplFile = ((nFileFlags & FF_TEMPLATE_FILE) != 0);
		BOOL bNonProject = ((nFileFlags & FF_NOT_IN_PROJECT) != 0);
		g_bLoadFromAppwiz = (nFileFlags & FF_LOAD_FROM_APPWIZ);

        // Get filename, and remember it for IDE project creation
        CString fileName = s.Mid(iSplit+1);
		AddDotIfNecessary(fileName);
		LPCTSTR szFileNameNoDot = RemoveDot(fileName);
		LPCTSTR szExt;

		// Add this as a dependency file if it's not the project .clw file: i.e.,
		//  1) It's not in the CWD, or
		//  2) It has no extension, or
		//  3) It's extension is NOT .clw
		if (_tcschr(szFileNameNoDot, '\\')
			|| ((szExt = _tcsrchr(szFileNameNoDot, '.')) == NULL)
			|| _tcsicmp(szExt, ".clw"))
		{
			// n.b.  Order is important here:
			if (bNonProject)
				pTermStatus->strlNonPrjFiles.AddTail(szFileNameNoDot);
			else if (bTemplFile)
				pTermStatus->strlTemplFiles.AddTail(szFileNameNoDot);
			else if (bExclude)
				pTermStatus->strlNonBldFiles.AddTail(szFileNameNoDot);
			else if (bHelpFile)
				pTermStatus->strlHelpFiles.AddTail(szFileNameNoDot);
			else if (bResFile)
				pTermStatus->strlResFiles.AddTail(szFileNameNoDot);
			else // default
			{
				// Now, ALL files are added to the project.  Source
				//  files are automatically detected as such by the
				//  build system, so we lump them in with all the
				//  dependency files in this string list
				pTermStatus->strlDepFiles.AddTail(szFileNameNoDot);

				// Remember whether it's a .def file
				if (((szExt = _tcsrchr(szFileNameNoDot, '.')) != NULL)
					&& !_tcsicmp(szExt, ".def"))
				{
					pTermStatus->strDefFile = szFileNameNoDot;
				}
			}
		}

        if (m_pFileStatus != NULL)
            m_pFileStatus->SetWindowText(szFileNameNoDot);

        FILE* pfile = fopen(fileName, bBinary ? "wb" : "wt");
        if (pfile == NULL)
            ReportAndThrow(IDP_FILE_CREATE, fileName);

        BOOL bOK = TRUE;
        // BLOCK: generate file
        {
			SetSymbol(_T("FILE_NAME_SYMBOL"), ConstructFileNameInclusionSymbol(fileName));

            FileStream fileGen(pfile);
			g_IncludeStack.Clear();
			LoadAndProcessTemplate(resName, &fileGen, bBinary);
			m_strTemplateName.Empty();

            if (ferror(pfile) != 0)
                bOK = FALSE;
            if (fclose(pfile) != 0)
                bOK = FALSE;
			fileGen.pOpenFile = NULL;  // We closed the file, so the destructor shouldn't
        }

        if (!bOK)
            ReportAndThrow(IDP_FILE_WRITE, fileName);

    }

    // Finally set pTermStatus to its head
    pTermStatus = &g_TermStatus;
    // restore to original directory in case a $$NEWPROJ directive was used
    if (szOrigDir[0] != '\0')
        _chdir(szOrigDir);
}

// Returns whether the project generator is currently in a loop (i.e., a
//  loop in the worklist).
BOOL CProjGen::InLoop()
{
	return (m_nIteration != -1);
}

void LoadAndProcessTemplate(LPCTSTR szTemplateName, OutputStream* pOutput, BOOL bBinary /* = FALSE */)
{
	CProjGen* pProjGen = CProjGen::GetCurrent();

	// Set projgen to name of new template
	if (pProjGen != NULL)
		pProjGen->SetTemplateName(szTemplateName);

	// Load
	DWORD dwSize;
	LPCTSTR szTemplate = LoadTemplateWrapper(szTemplateName, dwSize);
	ASSERT(szTemplate != NULL);	// LoadTemplateWrapper will report & throw if error loading
	
	// Process
	g_IncludeStack.Add(szTemplateName);
	if (bBinary)
		GetAWX()->CopyTemplate(szTemplate, dwSize, pOutput);
	else
		GetAWX()->ProcessTemplate(szTemplate, dwSize, pOutput);
	GetAWX()->PostProcessTemplate(szTemplate);
	VERIFY(g_IncludeStack.Remove(szTemplateName));
}

