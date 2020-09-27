/*  reparse.c - parse a regular expression
 *
 *  cl /c /Zep /AM /NT RE /Gs /G2 /Oa /D LINT_ARGS /Fc reparse.c
 *
 *  Modifications:
 *
 *	22-Jul-1986 mz	Hookable allocator (allow Z to create enough free space)
 *	19-Nov-1986 mz	Add RETranslateLength for Z to determine overflows
 *	18-Aug-1987 mz	Add field width and justification in translations
 *	01-Mar-1988 mz	Add in UNIX-like syntax
 *	14-Jun-1988 mz	Fix file parts allowing backslashes
 *	04-Dec-1989 bp	Let :p accept uppercase drive names
 *	20-Dec-1989 ln	capture trailing periods in :p
 *	23-Jan-1990 ln	Handle escaped characters & invalid trailing \ in
 *			RETranslate.
 *	05-Feb-1991 mz	Merged in KANJI stuff
 *
 */
#include "stdafx.h"
#include <stdio.h>
#include <ctype.h>
#include <malloc.h>
#include <string.h>
#include <tchar.h>

#undef CC_NONE

#include "remi.h"
#include "re.h"


#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

#if _DEBUG
#if 0
static CString g_strBuf;
#define DEBOUT(x) { g_strBuf.Format x; AfxOutputDebugString(g_strBuf); } // gross, but under debug & works
#else
#define DEBOUT(x)
#endif
#else
#define DEBOUT(x)
#endif


/*  regular expression compiler.  A regular expression is compiled into pseudo-
 *  machine code.  The principle is portable to other machines and is outlined
 *  below.  We parse by recursive descent.
 *
 *  The pseudo-code is fairly close to normal assembler and can be easily
 *  converted to be real machine code and has been done for the 80*86
 *  processor family.
 *
 *  The basic regular expressions handled are:
 *
 *	letter	    matches a single letter
 *	[class]     matches a single character in the class
 *	[~class]    matches a single character not in the class
 *	^	    matches the beginning of the line
 *	$	    matches the end of the line
 *	?	    matches any character (except previous two)
 *	\x	    literal x
 *	\n	    matches the previously tagged/matched expression (n digit)
 *
 *  Regular expressions are now build from the above via:
 *
 *	x*	    matches 0 or more x, matching minimal number
 *	x+	    matches 1 or more x, matching minimal number
 *	x@	    matches 0 or more x, matching maximal number
 *	x#	    matches 1 or more x, matching maximal number
 *	(x1!x2!...) matches x1 or x2 or ...
 *	~x	    matches 0 characters but prevents x from occuring
 *	{x}	    identifies an argument
 *
 *  The final expression that is matched by the compiler is:
 *
 *	xy	    matches x then y
 *
 *
 *  The actual grammar used is: 		    Parsing action:
 *
 *	TOP ->	re				    PROLOG .re. EPILOG
 *
 *
 *	re ->	alte re |
 *			empty
 *
 *	alte -> e '|' alte	|
 *			e			|
 *
 *	e ->	
 *		se *		|			    SMSTAR .se. SMSTAR1
 *		se +	    |
 *		se @	    |			    STAR .se. STAR1
 *		se #	    |
 *		se
 *
 *	se ->	( alt )     |
 *		[ ccl ]     |
 *		{ re }	|
 *		?	    |			    ANY
 *		^	    |			    BOL
 *		$	    |			    EOL
 *		~ se	    |			    NOTSIGN .se. NOTSIGN1
 *		:x	    |
 *		\n	    |			    PREV
 *		letter				    LETTER x
 *
 *	alt ->	re ! alt    |			    LEFTOR .re. ORSIGN
 *		re				    LEFTOR .re. ORSIGN RIGHTOR
 *
 *	ccl ->	~ cset	    |			    CCLBEG NOTSIGN .cset. CCLEND
 *		cset				    CCLBEG NULL .cset. CCLEND
 *
 *	cset -> item cset   |
 *		item
 *
 *	item -> letter - letter |		    RANGE x y
 *		letter				    RANGE x x
 *
 *  Abbreviations are introduced by :.
 *
 *	:a	[a-zA-Z0-9]				alphanumeric
 *	:b	[<space><tab>]		single whitespace character
 *	:c	[a-zA-Z]				alphabetic
 *	:d	[0-9]					digit
 *	:f	([~/\\ "\[\]\:<|>+=;,.]#)               file part
 *	:h	([0-9a-fA-F]#)				hex number
 *	:i	([a-zA-Z_$][a-zA-Z0-9_$]@)		identifier
 *	:n	([0-9]#.[0-9]@![0-9]@.[0-9]#![0-9]#)	number
 *	:p	(([A-Za-z]\:!)(\\!)(:f(.:f!)(\\!/))@:f(.:f!.!))	path
 *	:q	("[~"]@"!'[~']@')                       quoted string
 *	:w	([a-zA-Z]#)				word
 *	:z	([0-9]#)				integer
 *
 */

/*  There are several classes of characters:
 *
 *  Closure characters are suffixes that indicate repetition of the previous
 *  RE.
 *
 *  Simple RE chars are characters that indicate a particular type of match
 *
 */

/*  Closure character equates
 */
#define CC_SMPLUS	0		/* plus closure 		     */
#define CC_SMCLOSURE	1		/* star closure 		     */
#define CC_POWER	2		/* n repetitions of previous pattern */
#define CC_CLOSURE	3		/* greedy closure		     */
#define CC_PLUS 	4		/* greedy plus			     */
#define CC_NONE 	5
#define CC_ERROR	-1

/*  Simple RE character equates */
#define SR_BOL		0
#define SR_EOL		1
#define SR_ANY		2
#define SR_CCLBEG	3
#define SR_LEFTOR	4
#define SR_CCLEND	5
#define SR_ABBREV	6
#define SR_RIGHTOR	7
#define SR_ORSIGN	8
#define SR_NOTSIGN	9
#define SR_LEFTARG	10
#define SR_RIGHTARG	11
#define SR_LETTER	12
#define SR_PREV 	13
#define SR_WILD		14
#define SR_XASCII	15
#define SR_NOT_HANDLED	16

int EndAltRE[] =    { SR_ORSIGN, SR_RIGHTOR, -1 };
int EndArg[] =	    { SR_RIGHTARG, -1 };

char *pAbbrev[] = {
	 "a[a-zA-Z0-9]",
//	 "b([ \t]#)",
	 "b[ \t]",
	 "c[a-zA-Z]",
	 "d[0-9]",
	 "f([~/\\\\ \\\"\\[\\]\\:<|>+=;,.]#!..!.)",
	 "h([0-9a-fA-F]#)",
	 "i([a-zA-Z_$][a-zA-Z0-9_$]@)",
	 "n([0-9]#.[0-9]@![0-9]@.[0-9]#![0-9]#)",
	 "p(([A-Za-z]\\:!)(\\\\!/!)(:f(.:f!)(\\\\!/))@:f(.:f!.!))",
	 "q(\"[~\"]@\"!'[~']@')",
	 "w([a-zA-Z]#)",
	 "z([0-9]#)",
	 NULL
};

static char *digits = "0123456789";
static char strEpsTab[] = "<tab>";

static reSyntax g_reSntx = reZibo;

static int cArg;

/*  RECharType - classify a character type
 *
 *  p		character pointer
 *
 *  returns	type of character (SR_xx)
 */
int PASCAL INTERNAL RECharType (char *p)
{
	LONG lArg;

	if (g_reSntx == reZibo) {
	/* Zibo syntax
     */
		switch (*p) {
		case '^':
			 return SR_BOL;
		case '$':
			if (_istdigit ((unsigned char)p[1]))
				return SR_PREV;
			else
				return SR_EOL;
		case '?':
			return SR_ANY;
		case '[':
			return SR_CCLBEG;
		case '(':
			return SR_LEFTOR;
		case ']':
			return SR_CCLEND;
		case ':':
			return SR_ABBREV;
		case ')':
			return SR_RIGHTOR;
		case '!':
			return SR_ORSIGN;
		case '~':
			return SR_NOTSIGN;
		case '{':
			return SR_LEFTARG;
		case '}':
			return SR_RIGHTARG;
		default:
			return SR_LETTER;
		}
	}
	else if (g_reSntx == reUnix) {
	/*  Crappy UNIX syntax
	 */
		switch (*p) {
		case '^':
			return SR_BOL;
		case '$':
			return SR_EOL;
		case '.':
			return SR_ANY;
		case '[':
			return SR_CCLBEG;
		case ']':
			return SR_CCLEND;
		case '\\':
			switch (p[1]) {
			case ':':			    /*	\:C */
				return SR_ABBREV;
			case '(':			    /*	\(  */
				return SR_LEFTARG;
			case ')':			    /*	\)  */
				return SR_RIGHTARG;
			case '~':			    /*	\~  */
				return SR_NOTSIGN;
			case '{':			    /*	\{  */
				return SR_LEFTOR;
			case '}':			    /*	\}  */
				return SR_RIGHTOR;
			case '!':			    /*	\!  */
				return SR_ORSIGN;
			}
			if (_istdigit ((unsigned char)p[1])) 	    /*	\N  */
				return SR_PREV;
		default:
			return SR_LETTER;
		}
	}
	else if (g_reSntx == reBrief) {

		switch (*p) {
		case '{':
			return SR_LEFTARG;
		case '}':
			return SR_RIGHTARG;
		case '<':
		case '%':
			return SR_BOL;
		case '|':
			return SR_ORSIGN;
		case '>':
		case '$':
			return SR_EOL;
		case '[':
			return SR_CCLBEG;
		case ']':
			return SR_CCLEND;
		case '~':
			return SR_NOTSIGN;
		case '?':
			return SR_ANY;
		case '*':
			return SR_WILD;
		case '\\':
			switch (p[1]) {
			case 'x':
				return SR_XASCII;		// hex ascii specifier
			case 'n':
			case 'c':
				return SR_NOT_HANDLED;	// part of the syntax, but we don't compile them
			}
		default:
			return SR_LETTER;
		}
	}
	else {
		ASSERT(g_reSntx == reEpsilon);

		switch (*p) {
		case '|':
			return SR_ORSIGN;
		case '^':
			 return SR_BOL;
		case '.':
			return SR_ANY;
		case '[':
			return SR_CCLBEG;
		case ']':
			return SR_CCLEND;
		case '~':
			return SR_NOTSIGN;
		case '(':
			return SR_LEFTARG;
		case ')':
			return SR_RIGHTARG;
		case '$':
			return SR_EOL;
		case '<':
			if (FEpsASCII(p, &lArg))
				return SR_XASCII;

			// else fall through
		default:
			return SR_LETTER;
		}
	}
}

/*  RECharLen - length of character type
 *
 *  p		character pointer to type
 *
 *  returns	length in chars of type
 */
int PASCAL INTERNAL RECharLen (char *p)
{
	if (g_reSntx == reZibo)
		if (RECharType (p) == SR_PREV)	    /*	$N  */
			return 2;
	else
		if (RECharType (p) == SR_ABBREV)    /*	:N  */
			return 2;
	else
		 return 1;

	else {
		if (*p == '\\')
		switch (p[1]) {
			case '{':
			case '}':
			case '~':
			case '(':
			case ')':
			case '!':
				return 2;		    /*	\C  */
			case ':':			    /*	\:C */
				return 3;
			default:
				if (_istdigit ((unsigned char)p[1]))
					return 2;		/*	\N  */
			else
				return 1;
			}
		return 1;
	}
}

/*  REClosureLen - length of character type
 *
 *  p		character pointer to type
 *
 *  returns	length in chars of type
 */
int PASCAL INTERNAL REClosureLen (char *p)
{
	p;

	return 1;
}

/*  REParseRE - parse a general RE up to but not including the pEnd set
 *  of chars.  Apply a particular action to each node in the parse tree.
 *
 *  pAction	Parse action routine to call at particluar points in the
 *		parse tree.  This routine returns an unsigned quantity that
 *		is expected to be passed on to other action calls within the
 *		same node.
 *  p		character pointer to string being parsed
 *  pEnd	pointer to set of char types that end the current RE.
 *		External callers will typically use NULL for this value.
 *		Internally, however, we need to break on the ALT-terminating
 *		types or on arg-terminating types.
 *
 *  Returns:	pointer to delimited character if successful parse
 *		NULL if unsuccessful parse (syntax error).
 *
 */
char * PASCAL INTERNAL REParseRE (PACT pAction, register char *p, int *pEnd, patType *pat)
{
	int *pe;
	DEBOUT (("REParseRE (%04x, %s)\n", pAction, p));

	while (TRUE)
	{
		/*  If we're at end of input
		 */

		if (*p == '\0')
		{
			/*	If we're not in the midst of an open expression
			 */
			if (pEnd == NULL)
				/*  return the current parse position
				 */
				return p;
			else
			{
				/*  End of input, but expecting more, ERROR
				 */
				DEBOUT (("REParse expecting more, ERROR\n"));
				return NULL;
			}
		}

		/*  If there is an open expression
		 */
		if (pEnd != NULL)
		{
			/*	Find a matching character
			 */
			for (pe = pEnd; *pe != -1; pe++)
				if (RECharType (p) == *pe)
					return p;
		}

		if ((p = REParseAltE (pAction, p, pat)) == NULL)
			return NULL;
	}
}


char * PASCAL INTERNAL REParseAltE (PACT pAction, register char *p, patType *pat)
{
	unsigned int u = 0;

	if (FREAltExpr (p, pat))
	{
		u = (*pAction) (LEFTOR, u, 0, 0, pat);
		// Parse the left expression
		if ((p = REParseE (pAction, p, pat)) == NULL)
			return NULL;

		u = (*pAction) (ORSIGN, u, 0, 0, pat);
		p += RECharLen(p);

		// Parse the right expression
		u = (*pAction) (LEFTOR, u, 0, 0, pat);
		if ((p = REParseAltE (pAction, p, pat)) == NULL)
			return NULL;
		u = (*pAction) (ORSIGN, u, 0, 0, pat);
		(*pAction) (RIGHTOR, u, 0, 0, pat);
	}
	else
		p = REParseE (pAction, p, pat);

	return p;
}


char * PASCAL INTERNAL REParseGroup (PACT pAction, char *p, patType *pat)
{
	unsigned u;

	u = (*pAction) (LEFTARG, 0, 0, 0, pat);
	p += RECharLen(p);
	while (RECharType (p) != SR_RIGHTARG)
	{
		if ((p = REParseRE (pAction, p, EndArg, pat)) == NULL)
			return NULL;
	}
	(*pAction) (RIGHTARG, u, 0, 0, pat);
	cArg++;
	return p + RECharLen (p);
}


/*  REParseE - parse a simple regular expression with potential closures.
 *
 *  pAction	Action to apply at special parse nodes
 *  p		character pointer to spot where parsing occurs
 *
 *  Returns	pointer past parsed text if successful
 *		NULL otherwise (syntax error)
 */
char * PASCAL INTERNAL REParseE (PACT pAction, register char *p, patType *pat)
{
	DEBOUT (("REParseE (%04x, %s)\n", pAction, p));

	switch (REClosureChar (p, pat)) {
	case CC_SMPLUS:
		if (REParseSE (pAction, p, pat) == NULL)
				return NULL;
	case CC_SMCLOSURE:
		return REParseClosure (pAction, p, pat);

	case CC_PLUS:
		if (REParseSE (pAction, p, pat) == NULL)
			return NULL;
	case CC_CLOSURE:
		return REParseGreedy (pAction, p, pat);

	case CC_POWER:
		return REParsePower (pAction, p, pat);

	case CC_NONE:
		return REParseSE (pAction, p, pat);

	default:
		return NULL;
	}
}

/*  REParseSE - parse a simple regular expression
 *
 *  pAction	Action to apply at special parse nodes
 *  p		character pointer to spot where parsing occurs
 *
 *  Returns	pointer past parsed text if successful
 *		NULL otherwise (syntax error)
 */
char * PASCAL INTERNAL REParseSE (register PACT pAction, register char *p, patType *pat)
{
	DEBOUT (("REParseSE (%04x, %s)\n", pAction, p));

	switch (RECharType (p)) {
	case SR_CCLBEG:
		return REParseClass (pAction, p, pat);
	case SR_LEFTARG:
		return REParseGroup (pAction, p, pat);
	case SR_ANY:
		return REParseAny (pAction, p, pat);
	case SR_WILD:
		return REParseWild (pAction, p, pat);
	case SR_XASCII:
		return REParseXAscii(pAction, p, pat);
	case SR_BOL:
		return REParseBOL (pAction, p, pat);
	case SR_EOL:
		return REParseEOL (pAction, p, pat);
	case SR_PREV:
		return REParsePrev (pAction, p, pat);
	case SR_LEFTOR:
		return REParseAlt (pAction, p, pat);
	case SR_NOTSIGN:
		return REParseNot (pAction, p, pat);
	case SR_ABBREV:
		return REParseAbbrev (pAction, p, pat);
	case SR_NOT_HANDLED:	// for syntax we don't handle
		return NULL;
	default:
		return REParseChar (pAction, p, pat);
	}
}

/*  REParseClass - parse a class membership match
 *
 *  pAction	Action to apply at beginning of parse and at each range
 *  p		character pointer to spot where parsing occurs
 *
 *  Returns	pointer past parsed text if successful
 *		NULL otherwise (syntax error)
 */
char * PASCAL INTERNAL REParseClass (PACT pAction, register char *p, patType *pat)
{
	char c;
	char c2, c3, c4;
	unsigned u;

	DEBOUT (("REParseClass (%04x, %s)\n", pAction, p));

	p += RECharLen (p);
	if (((g_reSntx == reZibo) && *p == '~') ||
		((g_reSntx == reBrief) && *p == '~') ||
		((g_reSntx == reUnix || g_reSntx == reEpsilon) && *p == '^'))
	{
		u = (*pAction) (CCLNOT, 0, 0, 0, pat);
		p += RECharLen (p);
	}
	else
		u = (*pAction) (CCLBEG, 0, 0, 0, pat);

	while (RECharType (p) != SR_CCLEND)
	{

		if ((*p == '\\' && g_reSntx != reEpsilon) ||
			(*p == '%' && g_reSntx == reEpsilon))
		{
			p++;
			if (g_reSntx == reBrief || g_reSntx == reEpsilon)
				*p = Escaped(*p);
		}

		if (*p == '\0') {
			DEBOUT (("REParseClass expecting more, ERROR\n"));
			return NULL;
		}

		if (FEpsTab(p))
		{
			c = '\t';
			p += _tcslen(strEpsTab);
		}
		else
			c = *p++;

		if (theApp.m_fOEM_MBCS) {
			if (isleadbyte((unsigned char)c))
				c2 = *p++;
			else {
				c2 = c;
				c = 0;
			}
		}
		if (*p == '-') {
			p++;
			if (*p == '\\')
				p++;
			if (*p == '\0') {
				DEBOUT (("REParseClass expecting more, ERROR\n"));
				return NULL;
			}
			if (theApp.m_fOEM_MBCS) {
				c3 = *p;
				if (isleadbyte(*(unsigned char *)p))
					c4 = *++p;
				else {
					c4 = c3;
					c3 = 0;
				}
				if ( (c == 0 && c3 == 0) || (c != 0 && c3 != 0) ) {
					u = (*pAction) (RANGEJ1, 0, c, c2, pat);
					(*pAction) (RANGEJ2, u, c3, c4, pat);
				}
				else
					return NULL;
			}
			else {
				(*pAction) (RANGE, u, c, *p, pat);
			}
			p++;
		}
		else {
			if (theApp.m_fOEM_MBCS) {
				u = (*pAction) (RANGEJ1, 0, c, c2, pat);
				(*pAction) (RANGEJ2, u, c, c2, pat);
			}
			else {
			(*pAction) (RANGE, u, c, c, pat);
			}
		}
	}
	if (theApp.m_fOEM_MBCS) {
		c = 0;
		u = (*pAction) (RANGEJ1, 0, c, c, pat);
		(*pAction) (RANGEJ2, u, c, c, pat);
	}
	return p + RECharLen (p);
}

/*	REParseWild - parse a match-zero-or-more-any expression
 *
 *  pAction	Action to apply
 *  p		character pointer to spot where parsing occurs
 *
 *  Returns	pointer past parsed text if successful
 *		NULL otherwise (syntax error)
 */
char * PASCAL INTERNAL REParseWild (PACT pAction, char *p, patType *pat)
{
	unsigned u;
	DEBOUT (("REParseWild (%04x, %s)\n", pAction, p));

	// Create this instruction by emulating a '.*'
	u = (*pAction) (SMSTAR, 0, 0, 0, pat);
	(*pAction) (ANY, 0, 0, 0, pat);
	(*pAction) (SMSTAR1, u, 0, 0, pat);

	return p + RECharLen(p);
}

/*	REParseXAscii - parse a hex-specified ascii character
 *
 *	pAction	Action to apply
 *	p		Character pointer to spot where parsing occurs
 *
 *	Returns pointer past parsed text if successful
 *		NULL otherwise (syntax error)
 */
char * PASCAL INTERNAL REParseXAscii (PACT pAction, char *p, patType * pat)
{
	if (g_reSntx == reEpsilon)
	{
		LONG l;
		if (p = FEpsASCII(p, &l))
		{
			(*pAction)(LETTER, 0, (unsigned char)l, 0, pat);
			return p;
		}

		// Wasn't the format we expected!
		ASSERT(FALSE);
		return NULL;
	}
	else
	{
		int i;
		int	cDigits = 0;

		p += 2;

		// Make sure we have at least one valid digit following
		if (!*p || !_istxdigit(*(unsigned char *)p))
			return NULL;
		else
			cDigits++;

		// And check to see if there's a second
		if (*(p + 1) && _istxdigit(*(unsigned char *)(p + 1)))
			cDigits++;

		// Take a look at the two subsequent digits and create an ascii character out of them.
		_stscanf(p, "%2x", &i);

		// Make a character out of what we just read
		(*pAction) (LETTER, 0, (unsigned char)i, 0, pat);

		return p + cDigits;
	}
}

/*  REParseAny - parse a match-any-character expression
 *
 *  pAction	Action to apply
 *  p		character pointer to spot where parsing occurs
 *
 *  Returns	pointer past parsed text if successful
 *		NULL otherwise (syntax error)
 */
char * PASCAL INTERNAL REParseAny (PACT pAction, char *p, patType * pat)
{
	DEBOUT (("REParseAny (%04x, %s)\n", pAction, p));

	(*pAction) (ANY, 0, 0, 0, pat);
	return p + RECharLen (p);
}

/*  REParseBOL - parse a beginning-of-line match
 *
 *  pAction	Action to apply
 *  p		character pointer to spot where parsing occurs
 *
 *  Returns	pointer past parsed text if successful
 *		NULL otherwise (syntax error)
 */
char * PASCAL INTERNAL REParseBOL (PACT pAction, char *p, patType * pat)
{
	DEBOUT (("REParseBOL (%04x, %s)\n", pAction, p));

	(*pAction) (BOL, 0, 0, 0, pat);
	return p + RECharLen (p);
}

/*  REParsePrev - parse a previous-match item
 *
 *  pAction	Action to apply
 *  p		character pointer to spot where parsing occurs
 *
 *  Returns	pointer past parsed text if successful
 *		NULL otherwise (syntax error)
 */
char * PASCAL INTERNAL REParsePrev (PACT pAction, char *p, patType * pat)
{
	unsigned int i = *(p + 1) - '0';

	DEBOUT (("REParsePrev (%04x, %s)\n", pAction, p));

	if (i < 1 || i > (unsigned) cArg) {
		DEBOUT (("REParsePrev invalid previous number, ERROR\n"));
		return NULL;
	}

	(*pAction) (PREV, i, 0, 0, pat);
	return p + RECharLen (p);
}

/*  REParseEOL - parse an end-of-line match
 *
 *  pAction	Action to apply
 *  p		character pointer to spot where parsing occurs
 *
 *  Returns	pointer past parsed text if successful
 *		NULL otherwise (syntax error)
 */
char * PASCAL INTERNAL REParseEOL (PACT pAction, char *p, patType * pat)
{
	DEBOUT (("REParseEOL (%04x, %s)\n", pAction, p));

	(*pAction) (EOL, 0, 0, 0, pat);
	return p + RECharLen (p);
}

/*  REParseAlt - parse a series of alternatives
 *
 *  pAction	Action to apply before and after each alternative
 *  p		character pointer to spot where parsing occurs
 *
 *  Returns	pointer past parsed text if successful
 *		NULL otherwise (syntax error)
 */
char * PASCAL INTERNAL REParseAlt (PACT pAction, register char *p, patType * pat)
{
	unsigned u = 0;

	DEBOUT (("REParseAlt (%04x, %s)\n", pAction, p));

	while (RECharType (p) != SR_RIGHTOR) {
		p += RECharLen (p);
		u = (*pAction) (LEFTOR, u, 0, 0, pat);
		if ((p = REParseRE (pAction, p, EndAltRE, pat)) == NULL)
			return NULL;
		u = (*pAction) (ORSIGN, u, 0, 0, pat);
	}
	(*pAction) (RIGHTOR, u, 0, 0, pat);
	return p + RECharLen (p);
}

/*  REParseNot - parse a guard-against match
 *
 *  pAction	Action to apply
 *  p		character pointer to spot where parsing occurs
 *
 *  Returns	pointer past parsed text if successful
 *		NULL otherwise (syntax error)
 */
char * PASCAL INTERNAL REParseNot (PACT pAction, register char *p, patType * pat)
{
	unsigned u;

	DEBOUT (("REParseNot (%04x, %s)\n", pAction, p));

	p += RECharLen (p);
	if (*p == '\0') {
		DEBOUT (("REParseNot expecting more, ERROR\n"));
		return NULL;
	}
	u = (*pAction) (NOTSIGN, 0, 0, 0, pat);
	p = REParseSE (pAction, p, pat);
	(*pAction) (NOTSIGN1, u, 0, 0, pat);
	return p;
}

/*  REParseAbbrev - parse and expand an abbreviation
 *
 *  Note that since the abbreviations are in Z syntax, we must change syntax
 *  temporarily to Z.  We are careful to do this so that we do not mess up
 *  advancign the pointers.
 *
 *  pAction	Action to apply
 *  p		character pointer to spot where parsing occurs
 *
 *  Returns	pointer past parsed text if successful
 *		NULL otherwise (syntax error)
 */
char * PASCAL INTERNAL REParseAbbrev (PACT pAction, register char *p, patType * pat)
{
	int i;
	reSyntax	reSntxTmp;

	DEBOUT (("REParseAbbrev (%04x, %s)\n", pAction, p));

	p += RECharLen (p);

	reSntxTmp = g_reSntx;
	g_reSntx = reZibo;
	if (p[-1] == '\0') {
		DEBOUT (("REParseAbbrev expecting abbrev char, ERROR\n"));
		g_reSntx = reSntxTmp;
		return NULL;
	}

	for (i = 0; pAbbrev[i]; i++)
		if (p[-1] == *pAbbrev[i])
			if (REParseSE (pAction, pAbbrev[i] + 1, pat) == NULL) {
				g_reSntx = reSntxTmp;
				return NULL;
			}
			else {
				g_reSntx = reSntxTmp;
				return p;
			}
	DEBOUT (("REParseAbbrev found invalid abbrev char %s, ERROR\n", p - 1));
	g_reSntx = reSntxTmp;
	return NULL;
}

/*  REParseChar - parse a single character match
 *
 *  pAction	Action to apply
 *  p		character pointer to spot where parsing occurs
 *
 *  Returns	pointer past parsed text if successful
 *		NULL otherwise (syntax error)
 */
char * PASCAL INTERNAL REParseChar (PACT pAction, register char *p, patType * pat)
{
	char c;
	UINT ib;

	DEBOUT (("REParseChar (%04x, %s)\n", pAction, p));

	if ((*p == '\\' && g_reSntx != reEpsilon) ||
		(*p == '%' && g_reSntx == reEpsilon))
	{
		p++;
		if (g_reSntx == reBrief || g_reSntx == reEpsilon)
			c = Escaped(*p);
		else
			c = *p;
	}
	else
		c = *p;

	if (*p == '\0') {
		DEBOUT (("REParseChar expected more, ERROR\n"));
		return NULL;
	}

	if ((g_reSntx == reEpsilon) && FEpsTab(p))
	{
		c = '\t';
		ib = _tcslen(strEpsTab);
	}
	else
		ib = 1;

	if (theApp.m_fOEM_MBCS) {
		if ( isleadbyte ((unsigned char)*p) ) {
			(*pAction) (LETTER, 0, c, *(p+1), pat);
			return p+2;
		}
		else {
			(*pAction) (LETTER, 0, c, 0, pat);
			return p+ib;
		}
	}
	else {
		(*pAction) (LETTER, 0, c, 0, pat);
		return p+ib;
	}
}

/*  REParseClosure - parse a minimal match closure.  The match occurs by
 *  matching none, then one, ...
 *
 *  pAction	Action to apply
 *  p		character pointer to spot where parsing occurs
 *
 *  Returns	pointer past parsed text if successful
 *		NULL otherwise (syntax error)
 */
char * PASCAL INTERNAL REParseClosure (PACT pAction, register char *p, patType * pat)
{
	unsigned u;

	DEBOUT (("REParseaClosure (%04x, %s)\n", pAction, p));

	u = (*pAction) (SMSTAR, 0, 0, 0, pat);
	if ((p = REParseSE (pAction, p, pat)) == NULL)
		return NULL;
	(*pAction) (SMSTAR1, u, 0, 0, pat);
	return p + REClosureLen (p);
}

/*  REParseGreedy - parse a maximal-match closure.  The match occurs by
 *  matching the maximal number and then backing off as failures occur.
 *
 *  pAction	Action to apply
 *  p		character pointer to spot where parsing occurs
 *
 *  Returns	pointer past parsed text if successful
 *		NULL otherwise (syntax error)
 */
char * PASCAL INTERNAL REParseGreedy (PACT pAction, register char *p, patType * pat)
{
	unsigned u;

	DEBOUT (("REParseGreedy (%04x, %s)\n", pAction, p));

	u = (*pAction) (STAR, 0, 0, 0, pat);
	if ((p = REParseSE (pAction, p, pat)) == NULL)
		return NULL;
	(*pAction) (STAR1, u, 0, 0, pat);
	return p + REClosureLen (p);
}

/*  REParsePower -  parse a power-closure.  This is merely the simple pattern
 *  repeated the number of times specified by the exponent.
 *
 *  pAction	Action to apply
 *  p		character pointer to spot where parsing occurs
 *
 *  Returns	pointer past parsed text if successful
 *		NULL otherwise (syntax error)
 */
char * PASCAL INTERNAL REParsePower (PACT pAction, char *p, patType * pat)
{
	register char *p1;
	int exp;

	DEBOUT (("REParsePower (%04x, %s)\n", pAction, p));

	/*	We have .se. POWER something.  Skip over the .se. and POWER
	 *	to make sure that what follows is a valid number
	 */
	p1 = REParseSE (NullAction, p, pat);

	if (p1 == NULL)
		/*  Parse of .se. failed
		 */
		return NULL;

	/*	skip POWER
	 */
	p1 += REClosureLen (p1);

	if (*p1 == '\0') {
		DEBOUT (("REParsePower expecting more, ERROR\n"));
		return NULL;
	}

	/* try to parse off number */
	if (sscanf (p1, "%d", &exp) != 1) {
		DEBOUT (("REParsePower expecting number, ERROR\n"));
		return NULL;
	}

	p1 += strspn(p1, digits);

	/* iterate the pattern the exponent number of times */
	while (exp--)
		if (REParseSE (pAction, p, pat) == NULL)
			return NULL;
	return p1;
}

/*  NullAction - a do-nothing action.  Used for stubbing out the action
 *  during a parse.
 */
unsigned  INTERNAL NullAction (OPTYPE type, unsigned int u, unsigned char x, unsigned char y, patType * pat)
{
	type; u; x; y;
	return 0;
}

BOOL PASCAL INTERNAL FREAltExpr (char *p, patType * pat)
{
	if (g_reSntx == reBrief || g_reSntx == reEpsilon)
	{
		p = REParseE (NullAction, p, pat);
		return p && (*p == '|');
	}

	return FALSE;
}


// Returns NULL if false, or the pointer to the character past the end
// of the parsed string if true.

char * PASCAL INTERNAL FEpsASCII(char *p, long *plArg)
{
	char *pEnd = NULL;
	*plArg = 0;

	if (*p == '<')
	{
		p++;

		if (*p == '#')
		{
			char	*pStop, *pTmp;
			char	ch;
			int		base = 10;

			// scan for the matching '>' and temporarily set it to NULL
			if (!(pEnd = _tcschr(p, '>')))
				goto L_Return;

			// Remember to restore this before leaving!
			ch = *pEnd;
			*pEnd = '\0';

			p++;	// past the '#'

			// Decide which sort of number we're trying to scan:
			if (p && ((pTmp = _tcsstr(p, "0b")) || (pTmp = _tcsstr(p, "0B"))))
			{
				base = 2;
				p = pTmp + 2;
			}
			else if (p && ((pTmp = _tcsstr(p, "0x")) || (pTmp = _tcsstr(p, "0X"))))
			{
				base = 16;
				p = pTmp + 2;
			}
			else if (p && ((pTmp = _tcsstr(p, "0o")) || (pTmp = _tcsstr(p, "0O"))))
			{
				base = 8;
				p = pTmp + 2;
			}

			// otherwise, it's 10
			if (p)
				*plArg = _tcstol(p, &pStop, base);
			*pEnd = ch;

			if (*plArg > 0 && (pStop == pEnd))
				pEnd++;
			else
				pEnd = NULL;
		}
	}

L_Return:

	return pEnd;
}

BOOL PASCAL INTERNAL FEpsTab(char *p)
{
	return p && !_tcsnicmp(p, strEpsTab, _tcslen(strEpsTab));
}


/*  REClosureChar - return the character that corresponds to the next
 *  closure to be parsed.  We call REParseSE with a null action to merely
 *  advance the character pointer to point just beyond the current simple
 *  regular expression.
 *
 *  p		character pointer to spot where parsing occurs
 *
 *  Returns	closure character if appropriate
 *		CC_NONE if no closure character found.
 */
char  PASCAL INTERNAL REClosureChar (char *p, patType * pat)
{
	p = REParseSE (NullAction, p, pat);
	if (p == NULL)
		return CC_ERROR;

	if (g_reSntx == reZibo)
		/*  Zibo syntax
		 */
		switch (*p) {
		case '^':
			return CC_POWER;
		case '+':
			return CC_SMPLUS;
		case '#':
			return CC_PLUS;
		case '*':
			return CC_SMCLOSURE;
		case '@':
			return CC_CLOSURE;
		default:
			return CC_NONE;
		}
	else if (g_reSntx == reUnix) {
		/*  Crappy UNIX syntax
		 */
		switch (*p) {
		case '+':
			return CC_PLUS;
		case '*':
			return CC_CLOSURE;
		default:
			return CC_NONE;
		}
	}
	else if (g_reSntx == reBrief) {

		switch (*p) {
		case '+':
			return CC_PLUS;
		case '@':
			return CC_SMCLOSURE;
		default:
			return CC_NONE;
		}
	}
	else {

		ASSERT(g_reSntx == reEpsilon);
		switch (*p) {
			case '+':
				return CC_PLUS;
			case '?':			// [kherold]  This actually isn't right, but we can't do "zero or one"
			case '*':
				return CC_CLOSURE;
			default:
				return CC_NONE;
		}
	}
}

/*  RECompile - compile a pattern into the internal machine.  Return a
 *  pointer to the match machine.
 *
 *  p	    character pointer to pattern being compiled
 *
 *  Returns:	pointer to the internal machine if compilation was successful
 *		NULL if syntax error or not enough memory for malloc
 */
patType *RECompile (char *p, flagType fCase, reSyntax reSyntx)
{
	char cLoc ;
	static fXLTabInit = FALSE;
	g_reSntx = reSyntx;
	patType *pat;

	// CUDA #2808 -- compute XLTab correctly for doing case mapping...
	if (!fXLTabInit) {
		AnsiLowerBuff(XLTab, sizeof(XLTab));
		fXLTabInit = TRUE;
	}
	int size;
	if(!REEstimate(p, &size))
		return NULL;

	DEBOUT (("Length is %04x\n", size));

	if ((pat = (struct patType *) malloc(size)) == NULL)
		return NULL;

	memset ((void *) pat, cLoc = (char)(-1), (unsigned int)size);
	memset ((void *) pat->pArgBeg, cLoc = '\0', (unsigned int)sizeof (pat->pArgBeg));
	memset ((void *) pat->pArgEnd, cLoc = '\0', (unsigned int)sizeof (pat->pArgEnd));

	pat->REip = pat->code;
	pat->REArg = 1;
	pat->fCase = fCase;
	pat->reSyntx = reSyntx;

	cArg = 0;

	CompileAction (PROLOG, 0, 0, 0, pat);

	if (REParseRE (CompileAction, p, NULL, pat) == NULL)
		return NULL;

	CompileAction (EPILOG, 0, 0, 0, pat);

#if DEBUG
	REDump (pat);
#endif
	return pat;
}

/*  Escaped - translate an escaped character ala UNIX C conventions.
 *
 *  \t => tab	    \e => ESC char  \h => backspace \g => bell
 *  \n => lf	    \r => cr	    \\ => \
 *
 *  c	    character to be translated
 *
 *  Returns:	character as per above
 */
char  PASCAL INTERNAL Escaped (char c)
{
	switch (c) {
	case 't':
		return '\t';
	case 'e':
		return 0x1B;
	case 'h':
		return 0x08;
	case 'g':
		return 0x07;
#ifdef SUPPORT_NEWLINE_REPLACE
	case 'n':
		return '\n';
	case 'r':
		return '\r';
#endif
	case '\\':
		return '\\';
	default:
		return c;
	}
}

/*  REGetArg - copy argument string out from match.
 *
 *  pat     matched pattern
 *  i	    index of argument to fetch, 0 is entire pattern
 *  p	    destination of argument
 *
 *  Returns:	TRUE if successful, FALSE if i is out of range.
 */
flagType REGetArg (struct patType *pat, int i, char *p)
{
	int l = 0;

	if (i > MAXPATARG)
		return FALSE;
	else
	if (pat->pArgBeg[i] != (char *)-1)
		memmove ((char far *)p, (char far *)pat->pArgBeg[i], l = RELength (pat, i));
	p[l] = '\0';
	return TRUE;
}

/*  RETranslate - translate a pattern string and match structure into an
 *  output string.  During pattern search-and-replace, RETranslate is used
 *  to generate an output string based on an input match pattern and a template
 *  that directs the output.
 *
 *  The input match is any patType returned from RECompile that has been passed
 *  to fREMatch and that causes fREMatch to return TRUE.  The template string
 *  is any set of ascii chars.	The $ character leads in arguments:
 *
 *	$$ is replaced with $
 *	$0 is replaced with the entire match string ** EXCEPT IN BRIEF's SYNTAX! **  where 0 - 9 represent grouped items.
 *	$1-$9 is replaced with the corresponding tagged (by {}) item from
 *	    the match.
 *
 *  An alternative method is to specify the argument as:
 *
 *	$([w,]a) where a is the argument number (0-9) and w is an optional field
 *	    width that will be used in a printf %ws format.
 *
 *  buf     pattern matched
 *  src     template for the match
 *  dst     destination of the translation
 *
 *  Returns:	TRUE if translation was successful, FALSE otherwise
 */
flagType RETranslate (struct patType *buf, register char *src, register char *dst)
{
	int		i, w;
	char *	work;
	ULONG	cbCh;
	char	chArg;
	
	// Decide which symbol denotes the numbered argument
	if ((buf->reSyntx == reUnix) || (buf->reSyntx == reBrief))
		chArg = '\\';
	else if (buf->reSyntx == reEpsilon)
		chArg = '#';
	else
		chArg = '$';

	work = (char *)malloc(MAX_USER_LINE);
	if (work == NULL)
		return FALSE;

	*dst = '\0';

	while (*src != '\0') {
		/*  Process tagged substitutions first */
		if (*src == chArg && (_istdigit ((unsigned char)src[1]) || src[1] == '(')) {
			/*	presume 0-width field */
			w = 0;

			/*	skip $ and char */
			src += 2;

			/*	if we saw $n */
			if (_istdigit ((unsigned char)src[-1]))
				i = src[-1] - '0';
				/*	else we saw $( */
			else {
				/*  get tagged expr number */
				i = atoi (src);

				/*  skip over number */
				if (*src == '-')
					src++;
				src += strspn(src, digits);

				/*  was there a comma? */
				if (*src == ',') {
					/*	We saw field width, parse off expr number */
					w = i;
					i = atoi (++src);
					src += strspn(src, digits);
				}

				/*  We MUST end with a close paren */
				if (*src++ != ')') {
					free((HANDLE)work);
					return FALSE;
				}
			}
			/*	w is field width
			 *	i is selected argument
			 */
			if (g_reSntx == reBrief)
				i++;

			if (!REGetArg (buf, i, work)) {
				free((HANDLE)work);
				return FALSE;
			}

			sprintf (dst, "%*s", w, work);
			dst += _ftcslen (dst);
		}
		else
		/* process escaped characters */
		if ((*src == '\\' && g_reSntx != reEpsilon) ||
			(*src == '%' && g_reSntx == reEpsilon)) {

			src++;
			if (!*src) {
				free((HANDLE)work);
				return FALSE;
				}
			*dst++ = Escaped (*src++);
		}
		else
		/*  chArg quotes itself */
		if (*src == chArg && src[1] == chArg) {
			*dst++ = chArg;
			src += 2;
			}
		else
		{
			// [olympus 2805 : kherold]  Better copy a whole character at a time,
			// or a trail byte of '0x5c' will look like an escaped character!
			cbCh = _tclen(src);
			_tccpy(dst, src);
			dst += cbCh;
			src += cbCh;
		}
	}
	*dst = '\0';
	free((HANDLE)work);
	return TRUE;
}

/*  RETranslateLength - given a matched pattern and a replacement string
 *  return the length of the final replacement
 *
 *  The inputs have the same syntax/semantics as in RETranslate.
 *
 *  buf     pattern matched
 *  src     template for the match
 *
 *  Returns:	number of bytes in total replacement, -1 if error
 */
int RETranslateLength (struct patType *buf, register char *src)
{
	int i, w;
	int length = 0;
	char chArg = (char) (buf->reSyntx == reUnix ? '\\' : '$');

	while (*src != '\0') {
		/*  Process tagged substitutions first
		 */
		if (*src == chArg && (_istdigit ((unsigned char)src[1]) || src[1] == '(')) {
			w = 0;
			src += 2;
			if (_istdigit ((unsigned char)src[-1]))
				i = src[-1] - '0';
			else {
				i = atoi (src);
				if (*src == '-')
					src++;
				src += strspn(src, digits);
				if (*src == ',') {
					w = i;
					i = atoi (++src);
					src += strspn(src, digits);
				}
				if (*src++ != ')')
					return -1;
			}
			/*	w is field width
			 *	i is selected argument
			 */
			i = RELength (buf, i);
			length += max (i, abs(w));
		}
		else
		/* process escaped characters */
		if ((*src == '\\' && g_reSntx != reEpsilon) ||
			(*src == '%' && g_reSntx == reEpsilon)) {
			src += 2;
			length++;
		}
		else
		/*  chArg quotes itself */
		if (*src == chArg && src[1] == chArg) {
			src += 2;
			length++;
		}
		else {
			length++;
			src++;
		}
	}
	return length;
}

/*  RELength - return length of argument in match.
 *
 *  pat     matched pattern
 *  i	    index of argument to examine, 0 is entire pattern
 *
 *  Returns:	length of ith argument, -1 if i is out-of-range.
 */
int RELength (struct patType *pat, int i)
{
	if (i > MAXPATARG)
		return -1;
	else
	if (pat->pArgBeg[i] == (char *)-1)
		return 0;
	else
		return pat->pArgEnd[i] - pat->pArgBeg[i];
}

/*  REStart - return pointer to beginning of match.
 *
 *  ppat    matched pattern
 *
 *  Returns:	character pointer to beginning of match
 */
char *REStart (struct patType *pat)
{
	return pat->pArgBeg[0] == (char *)-1 ? NULL : pat->pArgBeg[0];
}


/*
**	FUNCTION:	FParseExpr
**
**	PURPOSE:	See if the current specified expression can produce a valid parse.
**
**	NOTES:
*/
/*  [fabriced] This should go away, once the edit package does not rely on it any more.
BOOL
FParseExpr(UINT nID, BOOL fMatchCase)
{
	if (pat[nID] != NULL)
	{
		free((HANDLE)(pat[nID]));
		pat[nID] = NULL;
	}

	// TODO: Fix the reSyntax on the following line 
	pat[nID] = RECompile(findReplace.findWhat, fMatchCase, g_reCurrentSyntax, nID);
	return pat[nID] != NULL;
}
*/
char XLTab[ ] = {	// Table for lowercase translation

    '\x00', '\x01', '\x02', '\x03', '\x04', '\x05', '\x06', '\x07',
    '\x08', '\x09', '\x0A', '\x0B', '\x0C', '\x0D', '\x0E', '\x0F',
    '\x10', '\x11', '\x12', '\x13', '\x14', '\x15', '\x16', '\x17',
    '\x18', '\x19', '\x1A', '\x1B', '\x1C', '\x1D', '\x1E', '\x1F',

    ' ', '!', '"', '#', '$', '%', '&', '\x27',
    '(', ')', '*', '+', ',', '-', '.', '/',
    '0', '1', '2', '3', '4', '5', '6', '7',
    '8', '9', ':', ';', '<', '=', '>', '?',
    '@', 'a', 'b', 'c', 'd', 'e', 'f', 'g',
    'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o',
    'p', 'q', 'r', 's', 't', 'u', 'v', 'w',
    'x', 'y', 'z', '[', '\\', ']', '^', '_',
    '`', 'a', 'b', 'c', 'd', 'e', 'f', 'g',
    'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o',
    'p', 'q', 'r', 's', 't', 'u', 'v', 'w',
    'x', 'y', 'z', '{', '|', '}', '~', '\x7F',

    '\x80', '\x81', '\x82', '\x83', '\x84', '\x85', '\x86', '\x87',
    '\x88', '\x89', '\x8A', '\x8B', '\x8C', '\x8D', '\x8E', '\x8F',
    '\x90', '\x91', '\x92', '\x93', '\x94', '\x95', '\x96', '\x97',
    '\x98', '\x99', '\x9A', '\x9B', '\x9C', '\x9D', '\x9E', '\x9F',
    '\xA0', '\xA1', '\xA2', '\xA3', '\xA4', '\xA5', '\xA6', '\xA7',
    '\xA8', '\xA9', '\xAA', '\xAB', '\xAC', '\xAD', '\xAE', '\xAF',
    '\xB0', '\xB1', '\xB2', '\xB3', '\xB4', '\xB5', '\xB6', '\xB7',
    '\xB8', '\xB9', '\xBA', '\xBB', '\xBC', '\xBD', '\xBE', '\xBF',
    '\xC0', '\xC1', '\xC2', '\xC3', '\xC4', '\xC5', '\xC6', '\xC7',
    '\xC8', '\xC9', '\xCA', '\xCB', '\xCC', '\xCD', '\xCE', '\xCF',
    '\xD0', '\xD1', '\xD2', '\xD3', '\xD4', '\xD5', '\xD6', '\xD7',
    '\xD8', '\xD9', '\xDA', '\xDB', '\xDC', '\xDD', '\xDE', '\xDF',
    '\xE0', '\xE1', '\xE2', '\xE3', '\xE4', '\xE5', '\xE6', '\xE7',
    '\xE8', '\xE9', '\xEA', '\xEB', '\xEC', '\xED', '\xEE', '\xEF',
    '\xF0', '\xF1', '\xF2', '\xF3', '\xF4', '\xF5', '\xF6', '\xF7',
    '\xF8', '\xF9', '\xFA', '\xFB', '\xFC', '\xFD', '\xFE', '\xFF'
};
