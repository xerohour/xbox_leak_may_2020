#include "std.h"
#include "xapp.h"
#include "Parser.h"
#include "Node.h"
#include "Runner.h"

#include "DefUse.h"

//#define LOCALTRACE TRACE
#define LOCALTRACE 1 ? (void)0 : ::Trace


////////////////////////////////////////////////////////////////////////////

CCompiler::CCompiler()
{
	m_nop = 0;
	m_opsSize = 0;
	m_ops = NULL;
	m_bError = false;
}

CCompiler::~CCompiler()
{
	delete [] m_ops;
}

void CCompiler::GrowTo(int nNewSize)
{
	int cb = ((nNewSize + 4095) & ~0xfff);
	BYTE* ops = new BYTE [cb];
	CopyMemory(ops, m_ops, m_nop);
	delete [] m_ops;
	m_ops = ops;
	m_opsSize = cb;
}

extern int g_nLine;
extern const TCHAR* g_szFileName;
extern bool g_bParseError;

void CCompiler::SyntaxError(const TCHAR* szMsg, ...)
{
	va_list args;
	va_start(args, szMsg);

	TCHAR szBuffer [512];
	_vsntprintf(szBuffer, countof (szBuffer), szMsg, args);

	TCHAR szMessage [1024];
	_stprintf(szMessage, _T("Syntax Error\n\nFile: %s\nLine: %d\n\n%s"), g_szFileName, g_nLine, szBuffer);

#ifdef _WINDOWS
	if (!m_bError)
		XAppMessageBox(szMessage); // Only one message box!
	
	Trace(_T("\001%s\n"), szMessage);
#else
	Trace(_T("\007%s\n"), szMessage);
#endif

	va_end(args);

	m_bError = true;
	g_bParseError = true;
}

CFunction* CCompiler::CreateFunction()
{
#pragma push_macro("new")
#undef new
	CFunction* pFunction = new(m_nop) CFunction;
#pragma pop_macro("new")
	pFunction->m_cbop = m_nop;
	CopyMemory(pFunction->m_rgop, m_ops, m_nop);

	return pFunction;
}


CFunctionCompiler::CFunctionCompiler()
{
	m_nFrameSize = 0;
	m_pBreak = NULL;
	m_nBreakables = 0;
	m_nopTopOfLoop = 0;
	m_bBehavior = false;
}



////////////////////////////////////////////////////////////////////////////


const TCHAR* CFunctionCompiler::ParseTerm(const TCHAR* pch)
{
	bool bNeg = false;
	bool bInvert = false;

	for (;;)
	{
		pch = SkipWhite(pch);

		if (*pch == '-')
			bNeg = !bNeg;
		else if (*pch == '+')
			; // just skip it
		else if (*pch == '~')
			bInvert = !bInvert;
		else
			break;

		pch += 1;
	}

//	// TODO: Deal with unary operators here...
//	if (*pch == '-' || *pch == '+' || *pch == '~')
//		SyntaxError("Can't handle unary operators yet!");

	if (bInvert)
		SyntaxError(_T("Sorry, the ~ operator has not been implemented yet!"));

	if (*pch == '(')
	{
		pch += 1;
		pch = ParseExpression(pch);
		pch = SkipWhite(pch);
		if (*pch != ')')
		{
			SyntaxError(_T("Missing ')'"));
			return pch;
		}

		pch += 1;
	}
	else
	{
		bool bQuoted = *pch == '"' || *pch == '\'';
		const TCHAR* pchToken;
		int cchToken;
		pch = Token(pch, pchToken, cchToken);

		if (cchToken == 0 && !bQuoted)
			return pch;

		if (bQuoted)
		{
			// string...
//			LOCALTRACE(_T("%d: opStr: %d \n"), m_nop, cchToken);

			Write(opStr);

			TCHAR rgchString [1024];
			int nStringLen = ExpandCString(rgchString, countof(rgchString), pchToken, cchToken);
			if (nStringLen < 0)
				return _T("");
			WriteString(rgchString, nStringLen);
		}
		else if (*pchToken == '.' || *pchToken == '-' || *pchToken >= '0' && *pchToken <= '9')
		{
			// number...
			TCHAR szBuf [256]; // copy the token because _tcstod stupidly copies the while string
			ASSERT(cchToken < countof(szBuf));
			CopyChars(szBuf, pchToken, cchToken);
			szBuf[cchToken] = 0;
			float n = (float)_tcstod(szBuf, NULL);
			if (bNeg)
			{
				bNeg = false;
				n = -n;
			}

//			LOCALTRACE(_T("%d: opNum: %f\n"), m_nop, n);
			Write(opNum);
			WriteNumber(n);
		}
		else if (cchToken == 4 && _tcsncmp(pchToken, _T("true"), 4) == 0)
		{
//			LOCALTRACE(_T("%d: opNum: 1.0\n"), m_nop);
			Write(opNum);
			WriteNumber(1.0f);
		}
		else if (cchToken == 5 && _tcsncmp(pchToken, _T("false"), 5) == 0)
		{
//			LOCALTRACE(_T("%d: opNum: 0.0\n"), m_nop);
			Write(opNum);
			WriteNumber(0.0f);
		}
		else if (cchToken == 4 && _tcsncmp(pchToken, _T("this"), 4) == 0)
		{
//			LOCALTRACE(_T("%d: opThis:\n"), m_nop);
			Write(opThis);
		}
		else if (cchToken == 4 && _tcsncmp(pchToken, _T("null"), 4) == 0)
		{
//			LOCALTRACE(_T("%d: opNull:\n"), m_nop);
			Write(opNull);
		}
		else if (cchToken == 3 && _tcsncmp(pchToken, _T("new"), 3) == 0)
		{
			pch = Token(pch, pchToken, cchToken);
			
#ifdef _DEBUG0
			{
				TCHAR chSav = pchToken[cchToken];
				((TCHAR*)pchToken)[cchToken] = '\0';
				LOCALTRACE(_T("new: %s\n"), pchToken);
				((TCHAR*)pchToken)[cchToken] = chSav;
			}
#endif

			int nParam = 0;

			pch = SkipWhite(pch);
			if (*pch == '(')
			{
				pch += 1;

				if (*pch == ')')
				{
					pch += 1;
				}
				else
				{
					for (;;)
					{
						pch = ParseExpression(pch);
						nParam += 1;

						pch = SkipWhite(pch);
						if (*pch == ')')
						{
							pch += 1;
							break;
						}

						if (*pch != ',')
						{
							SyntaxError(_T("Missing ','"));
							return pch;
						}

						pch += 1;
					}
				}
			}

//			LOCALTRACE(_T("%d: opNew:\n"), m_nop);
			Write(opNew);
			WriteInteger(nParam);
			WriteString(pchToken, cchToken);
		}
		else
		{
			// symbol...

			// Frame variables...

			// REVIEW: Should only so this if the last operator was not a dot!
			for (int i = 0; i < m_nFrameSize; i += 1)
			{
				if (cchToken == m_rgstLocal[i].cchName && _tcsncmp(pchToken, m_rgstLocal[i].pchName, cchToken) == 0)
				{
					// found a frame variable reference!
					Write(opLocal);
					WriteInteger(i);
					break;
				}
			}

			if (i == m_nFrameSize)
			{
				// Not a parameter, so assume late bound variable...
#ifdef _DEBUG0
				{
					TCHAR chSav = pchToken[cchToken];
					((TCHAR*)pchToken)[cchToken] = '\0';
					LOCALTRACE(_T("%d: opVar: %s\n"), m_nop, pchToken);
					((TCHAR*)pchToken)[cchToken] = chSav;
				}
#endif
				Write(opVar);
				WriteString(pchToken, cchToken);
			}
		}
	}

	if (bNeg)
		Write(opNeg);

	return pch;
}

static const DOPER rgdoper [] =
{
	{ '.',	0,		0,		1, opDot },

	{ '*',	0,		0,		2, opMul },
	{ '/',	0,		0,		2, opDiv },
//	{ '%',	0,		0,		5, opMod },

	{ '+',	0,		0,		3, opAdd },
	{ '-',	0,		0,		3, opSub },

	{ '<',	'<',	0,		4, opSHL },
//	{ '>',	'>',	'>',	4, opSHRU },
	{ '>',	'>',	0,		4, opSHR },

	{ '<',	'=',	0,		5, opLE },
	{ '>',	'=',	0,		5, opGE },
	{ '<',	0,		0,		5, opLT },
	{ '>',	0,		0,		5, opGT },

	{ '=',	'=',	0,		6, opEQ },
	{ '!',	'=',	0,		6, opNE },

  	{ '&',	0,		0,		7, opAnd },
  	{ '^',	0,		0,		8, opXor },
  	{ '|',	0,		0,		9, opOr },

//	{ '&',	'&',	0,		10, opLAnd }, // TODO: Need to implement this!
//	{ '|',	'|',	0,		11, opLOr }, // TODO: Need to implement this!

//	{ '?',	0,		0,		12, opQuest }, // TODO: Need to implement this!

	{ '=',	0,		0,		13, opAssign },
	{ '+',	'=',	0,		13, opAddAssign },
	{ '-',	'=',	0,		13, opSubAssign },
	{ '*',	'=',	0,		13, opMulAssign },
	{ '/',	'=',	0,		13, opDivAssign },
	{ '%',	'=',	0,		13, opModAssign },
//	{ '&',	'=',	0,		13, opAndAssign },
//	{ '|',	'=',	0,		13, opOrAssign },
//	{ '^',	'=',	0,		13, opXorAssign },
//	{ '<',	'<',	'=',	13, opSHLAssign },
//	{ '>',	'>',	'=',	13, opSHRAssign },
};

#define LAST_PRI	14

const TCHAR* CFunctionCompiler::ParseOperator(const TCHAR* pch, const DOPER*& pdoper)
{
	pdoper = NULL;

	pch = SkipWhite(pch);
	if (*pch == '\0')
		return pch;

	for (int nOper = 0; nOper < sizeof (rgdoper) / sizeof (DOPER); nOper += 1)
	{
		if (rgdoper[nOper].m_ch1 == *pch && (rgdoper[nOper].m_ch2 == 0 || rgdoper[nOper].m_ch2 == *(pch + 1)))
		{
			pch += 1;
			if (rgdoper[nOper].m_ch2 != 0)
				pch += 1;
			pdoper = &rgdoper[nOper];
			break;
		}
	}

	return pch;
}

const TCHAR* CFunctionCompiler::ParseArray(const TCHAR* pch)
{
	ASSERT(*pch == '[');
	pch += 1;

	pch = ParseExpression(pch);
	pch = SkipWhite(pch);

	if (*pch != ']')
	{
		SyntaxError(_T("Missing ']'"));
		return pch;
	}

	pch += 1;

//	LOCALTRACE(_T("%d: opArray:\n"), m_nop);
	Write(opArray);

	return pch;
}

const TCHAR* CFunctionCompiler::ParseCall(const TCHAR* pch)
{
	// we have evaluated the reference to the function already and are looking at the '('

	int nArgs = 0;

	ASSERT(*pch == '(');
	pch += 1;

	pch = SkipWhite(pch);

	if (*pch == ')')
	{
		pch += 1;
	}
	else
	{
		for (;;)
		{
			pch = ParseExpression(pch);
			nArgs += 1;

			pch = SkipWhite(pch);

			if (*pch == ')')
			{
				pch += 1;
				break;
			}

			if (*pch != ',')
			{
				SyntaxError(_T("Missing ,"));
				return pch;
			}

			pch += 1;
		}
	}

	// we have now evaluated (nArgs) arguments

//	LOCALTRACE(_T("%d: opCall: nArgs=%d\n"), m_nop, nArgs);
	Write(opCall);
	Write((BYTE)nArgs);

	return pch;
}

const TCHAR* CFunctionCompiler::ParseExp(const TCHAR* pch, int nPrio)
{
	if (nPrio == 0)
		return ParseTerm(pch);

	pch = ParseExp(pch, nPrio - 1);

	for (;;)
	{
		pch = SkipWhite(pch);

		if (nPrio == 1)
		{
			if (*pch == '(')
				pch = ParseCall(pch);
			else if (*pch == '[')
				pch = ParseArray(pch);
		}

		const TCHAR* pchOper = pch;
		const DOPER* pdoper;
		pch = ParseOperator(pch, pdoper);
		if (pdoper == NULL || pdoper->m_pri != nPrio)
			return pchOper;

		pch = ParseExp(pch, nPrio - 1);

//		LOCALTRACE(_T("%d: op %c%c\n"), m_nop, pdoper->m_ch1, pdoper->m_ch2 == 0 ? ' ' : pdoper->m_ch2);
		Write(pdoper->m_op);
	}
}

const TCHAR* CFunctionCompiler::ParseExpression(const TCHAR* pch)
{
	return ParseExp(pch, LAST_PRI);
}

const TCHAR* CFunctionCompiler::ParseIF(const TCHAR* pch)
{
	pch = SkipWhite(pch);
	if (*pch != '(')
	{
		SyntaxError(_T("expected '('"));
		return pch;
	}

	pch += 1;

	pch = ParseExpression(pch);

	pch = SkipWhite(pch);
	if (*pch != ')')
	{
		SyntaxError(_T("expected ')'"));
		return pch;
	}

	pch += 1;

//	LOCALTRACE(_T("%d opCond: \n"), m_nop);
	Write(opCond);
	UINT nop = GetAddress();
	WriteInteger(0);

	pch = ParseStatement(pch);

	// Check for else clause...

	const TCHAR* pchToken;
	int cchToken;
	pch = Token(pch, pchToken, cchToken);

	if (cchToken == 4 && _tcsncmp(pchToken, _T("else"), cchToken) == 0)
	{
//		LOCALTRACE(_T("%d: opJump: \n"), m_nop);
		Write(opJump);
		UINT nopElse = GetAddress();
		WriteInteger(0);

		// fixup cond
		Fixup(nop, GetAddress());

		pch = ParseStatement(pch);

		// fixup jump
		Fixup(nopElse, GetAddress());
	}
	else
	{
		// Backup...
		pch = pchToken;

		// fixup cond
		Fixup(nop, GetAddress());
	}

	return pch;
}

const TCHAR* CFunctionCompiler::ParseWHILE(const TCHAR* pch)
{
	BREAK* pOldBreaks = m_pBreak;
	m_pBreak = NULL;
	m_nBreakables += 1;

	pch = SkipWhite(pch);
	if (*pch != '(')
	{
		SyntaxError(_T("expected '('"));
		return _T("");
	}

	pch += 1;

	int nopOldTopOfLoop = m_nopTopOfLoop;
	m_nopTopOfLoop = GetAddress();
	pch = ParseExpression(pch);

	pch = SkipWhite(pch);
	if (*pch != ')')
	{
		SyntaxError(_T("expected ')'"));
		return _T("");
	}

	pch += 1;

//	LOCALTRACE(_T("%d opCond: \n"), m_nop);
	Write(opCond);
	UINT nopFixup = GetAddress();
	WriteInteger(0);

	pch = ParseStatement(pch);

//	LOCALTRACE(_T("%d: opJump: \n"), m_nop);
	Write(opJump);
	WriteInteger(m_nopTopOfLoop);

	// Fixup jump out of loop
	Fixup(nopFixup, GetAddress());

	FixupBreaks();

	m_pBreak = pOldBreaks;
	m_nBreakables -= 1;
	m_nopTopOfLoop = nopOldTopOfLoop;

	return pch;
}

const TCHAR* CFunctionCompiler::ParseDO(const TCHAR* pch)
{
	BREAK* pOldBreaks = m_pBreak;
	m_pBreak = NULL;
	m_nBreakables += 1;

	pch += 1;

	int nopOldTopOfLoop = m_nopTopOfLoop;
	m_nopTopOfLoop = GetAddress();

	pch = ParseStatement(pch);

	const TCHAR* pchToken;
	int cchToken;
	pch = Token(pch, pchToken, cchToken);
	if (cchToken != 5 || _tcsncmp(pchToken, _T("while"), 5) != 0)
	{
		SyntaxError(_T("expected 'while'"));
		return _T("");
	}

	pch = SkipWhite(pch);
	if (*pch != '(')
	{
		SyntaxError(_T("expected '('"));
		return _T("");
	}

	pch += 1;

	pch = ParseExpression(pch);

	pch = SkipWhite(pch);
	if (*pch != ')')
	{
		SyntaxError(_T("expected ')'"));
		return _T("");
	}

	pch += 1;

	pch = SkipWhite(pch);
	if (*pch != ';')
	{
		SyntaxError(_T("expected ';'"));
		return _T("");
	}

	pch += 1;

	// REVIEW: This is inefficient; should have opNotCond or something...
//	LOCALTRACE(_T("%d opCond: \n"), m_nop);
	Write(opCond);
	UINT nopFixup = GetAddress();
	WriteInteger(0);

//	LOCALTRACE(_T("%d: opJump: \n"), m_nop);
	Write(opJump);
	WriteInteger(m_nopTopOfLoop);

	// Fixup jump out of loop
	Fixup(nopFixup, GetAddress());

	FixupBreaks();

	m_pBreak = pOldBreaks;
	m_nBreakables -= 1;
	m_nopTopOfLoop = nopOldTopOfLoop;

	return pch;
}

const TCHAR* CFunctionCompiler::ParseFOR(const TCHAR* pch)
{
	BREAK* pOldBreaks = m_pBreak;
	m_pBreak = NULL;
	m_nBreakables += 1;

	pch = SkipWhite(pch);
	if (*pch != '(')
	{
		SyntaxError(_T("expected '('"));
		return _T("");
	}

	pch += 1;

	// Initial expression...
	const TCHAR* pchToken;
	int cchToken;
	Token(pch, pchToken, cchToken);
	if (cchToken == 3 && _tcsncmp(pchToken, _T("var"), cchToken) == 0)
	{
		pch += 3;
		pch = ParseLocalVar(pch);
	}
	else
	{
		pch = ParseExpression(pch);
		Write(opDrop);

		pch = SkipWhite(pch);
		if (*pch != ';')
		{
			SyntaxError(_T("expected ';'"));
			return _T("");
		}

		pch += 1;
	}



	// Condition...
	int nopOldTopOfLoop = m_nopTopOfLoop;
	m_nopTopOfLoop = GetAddress();
	pch = ParseExpression(pch);

	pch = SkipWhite(pch);
	if (*pch != ';')
	{
		SyntaxError(_T("expected ';'"));
		return _T("");
	}

	pch += 1;

	int nopExp3 = GetAddress();
	const TCHAR* pchExp3 = pch;
	pch = ParseExpression(pch);
	Write(opDrop);
	m_nop = nopExp3; // backup to start of expression

	pch = SkipWhite(pch);
	if (*pch != ')')
	{
		SyntaxError(_T("expected ')'"));
		return _T("");
	}

	pch += 1;



//	LOCALTRACE(_T("%d opCond: \n"), m_nop);
	Write(opCond);
	UINT nopFixup = GetAddress();
	WriteInteger(0);

	pch = ParseStatement(pch);

	// Generate code for the third expression...
	ParseExpression(pchExp3);
	Write(opDrop);

//	LOCALTRACE(_T("%d: opJump: \n"), m_nop);
	Write(opJump);
	WriteInteger(m_nopTopOfLoop);

	// Fixup jump out of loop
	Fixup(nopFixup, GetAddress());

	FixupBreaks();

	m_pBreak = pOldBreaks;
	m_nBreakables -= 1;
	m_nopTopOfLoop = nopOldTopOfLoop;

	return pch;
}


void CFunctionCompiler::FixupBreaks()
{
	for (BREAK* pBreak = m_pBreak; pBreak != NULL; )
	{
		BREAK* pNextBreak = pBreak->m_pNext;
		Fixup(pBreak->m_nop, GetAddress());
		delete pBreak;
		pBreak = pNextBreak;
	}
}

const TCHAR* CFunctionCompiler::ParseBREAK(const TCHAR* pch)
{
	pch = SkipWhite(pch);
	if (*pch != ';')
	{
		SyntaxError(_T("Missing ';'"));
		return _T("");
	}

	pch += 1;

	if (m_nBreakables == 0)
	{
		SyntaxError(_T("unexpected 'break'"));
		return _T("");
	}

	// Jump out of the loop or switch!
//	LOCALTRACE(_T("%d: opJump: \n"), m_nop);
	Write(opJump);
	BREAK* pBreak = new BREAK;
	pBreak->m_pNext = m_pBreak;
	m_pBreak = pBreak;
	pBreak->m_nop = GetAddress();
	WriteInteger(0);

	return pch;
}

const TCHAR* CFunctionCompiler::ParseCONTINUE(const TCHAR* pch)
{
	pch = SkipWhite(pch);
	if (*pch != ';')
	{
		SyntaxError(_T("Missing ';'"));
		return _T("");
	}

	pch += 1;

	if (m_nBreakables == 0)
	{
		SyntaxError(_T("unexpected 'continue'"));
		return _T("");
	}

//	LOCALTRACE(_T("%d: opJump: \n"), m_nop);
	Write(opJump);
	WriteInteger(m_nopTopOfLoop);

	return pch;
}

const TCHAR* CFunctionCompiler::ParseRETURN(const TCHAR* pch)
{
	pch = SkipWhite(pch);
	if (*pch == ';')
	{
		Write(opNull);
	}
	else
	{
		pch = ParseExpression(pch);

		pch = SkipWhite(pch);
		if (*pch != ';')
		{
			SyntaxError(_T("Missing ';'"));
			return _T("");
		}
	}

	pch += 1;

//	LOCALTRACE(_T("%d: opRet\n"), m_nop);
	Write(opRet);

	return pch;
}

const TCHAR* CFunctionCompiler::ParseSLEEP(const TCHAR* pch)
{
	pch = SkipWhite(pch);
	if (*pch == ';')
	{
		Write(opNull);
	}
	else
	{
		pch = ParseExpression(pch);

		pch = SkipWhite(pch);
		if (*pch != ';')
		{
			SyntaxError(_T("Missing ';'"));
			return _T("");
		}
	}

	pch += 1;

//	LOCALTRACE(_T("%d: opSleep\n"), m_nop);
	Write(opSleep);

	return pch;
}

const TCHAR* CFunctionCompiler::ParseBlock(const TCHAR* pch)
{
	bool bBrace = (*pch == '{');
	if (bBrace)
		pch += 1;

	int nFrameSize = m_nFrameSize;

	Write(opFrame);
	UINT nopFrameFixup = GetAddress();
	WriteInteger(0);

	while (*pch != '\0' && (!bBrace || *pch != '}'))
		pch = ParseStatement(pch);

	if (bBrace)
	{
		if (*pch != '}')
		{
			SyntaxError(_T("expected '}'"));
			return pch;
		}

		pch += 1;
	}

	// Fixup frame...
//	LOCALTRACE(_T("Fixing frame for %d locals\n"), m_nFrameSize - nFrameSize);
	Fixup(nopFrameFixup, m_nFrameSize - nFrameSize);
	Write(opEndFrame);
	WriteInteger(m_nFrameSize - nFrameSize);

	m_nFrameSize = nFrameSize; // nuke the block locals...

	return pch;
}

const TCHAR* CFunctionCompiler::ParseStatement(const TCHAR* pch)
{
	pch = SkipWhite(pch);

	if (*pch == '}')
		return pch;

	if (*pch == '{')
	{
		return ParseBlock(pch);
	}

	const TCHAR* pchToken;
	int cchToken;
	pch = Token(pch, pchToken, cchToken);

//	LOCALTRACE(_T("%d: opStatement: line %d\n"), GetAddress(), g_nLine);
	Write(opStatement);
	WriteInteger(g_nLine);

	if (cchToken == 3 && _tcsncmp(pchToken, _T("var"), cchToken) == 0)
		return ParseLocalVar(pch);

	if (cchToken == 2 && _tcsncmp(pchToken, _T("if"), cchToken) == 0)
		return ParseIF(pch);

	if (cchToken == 5 && _tcsncmp(pchToken, _T("while"), cchToken) == 0)
		return ParseWHILE(pch);

	if (cchToken == 3 && _tcsncmp(pchToken, _T("for"), cchToken) == 0)
		return ParseFOR(pch);

	if (cchToken == 2 && _tcsncmp(pchToken, _T("do"), cchToken) == 0)
		return ParseDO(pch);

	if (cchToken == 5 && _tcsncmp(pchToken, _T("break"), cchToken) == 0)
		return ParseBREAK(pch);

	if (cchToken == 8 && _tcsncmp(pchToken, _T("continue"), cchToken) == 0)
		return ParseCONTINUE(pch);

	if (cchToken == 6 && _tcsncmp(pchToken, _T("return"), cchToken) == 0)
		return ParseRETURN(pch);

	if (m_bBehavior && cchToken == 5 && _tcsncmp(pchToken, _T("sleep"), cchToken) == 0)
		return ParseSLEEP(pch);

	// TODO: switch statements

	pch = ParseExpression(pchToken);
	Write(opDrop);

	if (*pch != 0)
	{
		if (*pch != ';')
		{
			SyntaxError(_T("expected ';'"));
			return _T("");
		}

		pch += 1;
	}

	return pch;
}


// Add local variables to the current function...
const TCHAR* CFunctionCompiler::ParseLocalVar(const TCHAR* pch)
{
	for (;;)
	{
		const TCHAR* pchToken;
		int cchToken;
		pch = Token(pch, pchToken, cchToken);

#ifdef _DEBUG0
		{
			TCHAR chSav = pchToken[cchToken];
			((TCHAR*)pchToken)[cchToken] = '\0';
			LOCALTRACE(_T("new member variable \"%s\"\n"), pchToken);
			((TCHAR*)pchToken)[cchToken] = chSav;
		}
#endif
		m_rgstLocal[m_nFrameSize].pchName = pchToken;
		m_rgstLocal[m_nFrameSize].cchName = (short)cchToken;
		m_nFrameSize += 1;

		pch = SkipWhite(pch);

		if (*pch == '=')
		{
			pch += 1;

//			LOCALTRACE(_T("%d: opLocal: %d\n"), m_nop, m_nFrameSize - 1);
			Write(opLocal);
			WriteInteger(m_nFrameSize - 1);

			pch = ParseExpression(pch);

//			LOCALTRACE(_T("%d: opAssign\n"), m_nop);
			Write(opAssign);
			Write(opDrop);

			pch = SkipWhite(pch);
		}

		if (*pch != ',')
			break;

		pch += 1;
	}

	if (*pch != ';')
		SyntaxError(_T("Expected a ';'"));
	else
		pch += 1;

	return pch;
}
