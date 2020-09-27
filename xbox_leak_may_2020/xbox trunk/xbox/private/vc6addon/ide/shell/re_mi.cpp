/*  re_mi.c - machine independent regular expression compiler
 *  cl /c /Zep /AM /NT RE /Gs /G2 /Oa /D LINT_ARGS /Fc re_mi.c
 *
 *  Modifications:
 *	09-Mar-1988 mz	Add check in fREMtch for pat == NULL
 *	15-Sep-1988 bw	Change fREMatch to REMatch.  New parameters and
 *			return type.
 *	23-Nov-1989 bp	Use relative adresses: OFST and PNTR macros
 *	05-Feb-1991 mz	Merge in KANJI changes
 *
 */

#include "stdafx.h"
#include <tchar.h>

#include "util.h"

#include <ctype.h>
#include <malloc.h>
#include <string.h>

#include "remi.h"
#include "re.h"
#include "msgboxes.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

typedef unsigned short int			DBCS;
typedef UNALIGNED RE_OPCODE * UNALIGNED *		PPREOP;
typedef unsigned char * UNALIGNED *	PPUCH;
typedef UNALIGNED unsigned *		PU;
typedef UNALIGNED unsigned short *	PUS;
typedef UNALIGNED DBCS *			PDBCS;
typedef const char *		PCC;

unsigned char *MovePBackwards (unsigned char *bos, unsigned char *P);


/*
	class CRegExStack -- this class exists simply to handle the stack
	allocation management for the RESearch fn below, which had a one-
	time leak
*/

class CRegExStack
{
protected:
	RE_OPCODE **m_stack; // Allocation for RE stack
	int maxREStack; // stack size

	void AllocateStack(void);
public:
	CRegExStack(void);
	~CRegExStack(void);
	void GrowStack(void);
	RE_OPCODE **GetData(void) { return m_stack; }
	int GetMax(void) { return maxREStack; }
};

void CRegExStack::AllocateStack(void)
{
	ASSERT(!m_stack); // cleared under _DEBUG
	m_stack = static_cast<RE_OPCODE **>(malloc(maxREStack * sizeof(*m_stack))); // presently 1024 total
}

void CRegExStack::GrowStack(void)
{
	ASSERT(m_stack);
	free(m_stack);
#ifdef _DEBUG
	m_stack = 0;
#endif
	maxREStack += 256;
	AllocateStack();
}

CRegExStack::CRegExStack(void)
{
	maxREStack = 256;
#ifdef _DEBUG
	m_stack = 0;
#endif
	AllocateStack();
}

CRegExStack::~CRegExStack(void)
{
	ASSERT(m_stack);
	free(m_stack);
}

BOOL RESearch(
	LPCTSTR line,
	ULONG *ichStart,	// byte index into line
	ULONG *xEnd,
	patType *pat,
	LPCTSTR szString,	// pointer to reg expression
	/* out */ BOOL *pbLineMatch)
{
//	static int maxREStack = 256;
	BOOL done;
//	static RE_OPCODE **stack = NULL; //Allocation for RE stack
	static CRegExStack theStack;
	flagType f = '\1' ;
	ULONG		cbSrchLine;

	//Allocates the stack for the regular expression
//	if (stack == NULL)
//		stack = (RE_OPCODE **)malloc(maxREStack * sizeof(*stack));

	//We may redo it if we need more stack
	do {

		done = TRUE;

		//Do not search if start is out of string
		cbSrchLine = (ULONG)_ftcslen(line);
		if(*ichStart > cbSrchLine) // DevStd #295: can be equal in case of empty line
			break;

		//Try the match
		switch (REMatch(pat, line, line + *ichStart, theStack.GetData(), theStack.GetMax(), f, pbLineMatch))
		{

			case REM_MATCH: {
				int len = RELength(pat, 0);
				if (len >= 0) {

					int newichStart = (REStart (pat) - line);

					// Returning a byte index into the document line
					*ichStart = newichStart;
					*xEnd = newichStart + len;

					return TRUE;
				}
				//else {
				//   if (len == 0) {
				//      *xEnd = *ichStart + 1;
				//      VERIFY(LocalFree((HANDLE)stack) == NULL);
				//      return TRUE;
				//   }
				//}
				break;
			}

			case REM_NOMATCH:
				break;

			case REM_STKOVR:

				//The RE engine had a stack overflow. Add 256 bytes
				//and try again

				theStack.GrowStack();

				done = FALSE;
				break;

			case REM_INVALID:
//				ErrorBox(IDS_REGEXPR_INVALID, szString);
//				frMem.hadError = TRUE;
				break;

			case REM_UNDEF:
//				ErrorBox(IDS_REGEXPR_UNDEF, szString);
//				frMem.hadError = TRUE;
				break;

			default:
				break;

		}

	} while (!done);

	return FALSE;
}

/*  The following are dependent on the low-level representation of the compiled
 *  machine.  The cases that have been implemented are:
 *
 *  Simple interpreted machine
 */

/* pseudo-instruction definitions */

#define I_CALL	    0
#define I_RETURN    1
#define I_LETTER    2
#define I_ANY	    3
#define I_EOL	    4
#define I_BOL	    5
#define I_CCL	    6
#define I_NCCL	    7
#define I_MATCH     8
#define I_JMP	    9
#define I_SPTOM     10
#define I_PTOM	    11
#define I_MTOP	    12
#define I_MTOSP     13
#define I_FAIL	    14
#define I_PUSHP     15
#define I_PUSHM     16
#define I_POPP	    17
#define I_POPM	    18
#define I_PNEQM     19
#define I_ITOM	    20
#define I_PREV	    21

/* instruction templates and lengths */

//#if defined(KANJI)
#define LLETTER     3
//#else
//#define LLETTER     2
//#endif
#define LANY	    1
#define LBOL	    1
#define LEOL	    1

/* address part of instruction */

#define ADDR(ip)    (*(PPREOP)(ip+sizeof(RE_OPCODE)))

/* conversion macros for adresses */

#define OFST(p)     ((RE_OPCODE *) (((char *) p) - ((char *) pat)))
#define PNTR(p)     ((RE_OPCODE *) (((char *) pat) + ((unsigned) p)))

#define IMM(ip)     (*(PPREOP)(ip+sizeof(RE_OPCODE)+sizeof(RE_OPCODE *)))

//#if defined(KANJI)
#define LRANGE	    (sizeof(RE_OPCODE)+sizeof(RE_OPCODE)+sizeof(RE_OPCODE)+sizeof(RE_OPCODE))
//#define LCCL	    (sizeof(RE_OPCODE))
//#define LNCCL	    (sizeof(RE_OPCODE))
//#else
#define LCCL	    (sizeof(RE_OPCODE)+(256/8))
#define LNCCL	    (sizeof(RE_OPCODE)+(256/8))
//#endif

#define LOFFSET     sizeof(RE_OPCODE *)
#define LCALL	    (sizeof(RE_OPCODE)+LOFFSET)
#define LJMP	    (sizeof(RE_OPCODE)+LOFFSET)
#define LSPTOM	    (sizeof(RE_OPCODE)+LOFFSET)
#define LPTOM	    (sizeof(RE_OPCODE)+LOFFSET)
#define LMTOP	    (sizeof(RE_OPCODE)+LOFFSET)
#define LMTOSP	    (sizeof(RE_OPCODE)+LOFFSET)
#define LRETURN     sizeof(RE_OPCODE)
#define LMATCH	    sizeof(RE_OPCODE)
#define LFAIL	    sizeof(RE_OPCODE)
#define LPUSHM	    (sizeof(RE_OPCODE)+LOFFSET)
#define LPOPM	    (sizeof(RE_OPCODE)+LOFFSET)
#define LPUSHP	    sizeof(RE_OPCODE)
#define LPOPP	    sizeof(RE_OPCODE)
#define LPNEQM	    (sizeof(RE_OPCODE)+LOFFSET)
#define LITOM	    (sizeof(RE_OPCODE)+LOFFSET+LOFFSET)
#define LPREV	    (sizeof(RE_OPCODE)+sizeof(RE_OPCODE))

/* action templates */

typedef struct {
	 RE_OPCODE	i1[LCALL];		/*	CALL	pattern 	  */
	 RE_OPCODE	i2[LFAIL];		/*	FAIL			      */
	 } T_PROLOG; 				/* pattern:			      */

typedef struct {
	 RE_OPCODE	i1[LPTOM];		/*	PTOM	ArgBeg[cArg]	*/
	 RE_OPCODE	i2[LCALL];		/*	CALL	x				*/
	 RE_OPCODE	i3[LITOM];		/*	ITOM	ArgBeg[cArg],-1 */
	 RE_OPCODE	i4[LRETURN];	/*	RETURN			      	*/
	 } T_LEFTARG;				/* x:				      	*/

typedef struct {
	 RE_OPCODE	i1[LPTOM];		/*	PTOM	ArgEnd[cArg]	*/
	 } T_RIGHTARG;

typedef struct {
	 RE_OPCODE	i1[LPUSHM];		/*	PUSHM	tmp		      */
	 RE_OPCODE	i2[LCALL];		/*	CALL	l1		      */
	 RE_OPCODE	i3[LPOPM];		/*	POPM	tmp		      */
	 RE_OPCODE	i4[LRETURN];	/*	RETURN			      */
	 RE_OPCODE	tmp[LOFFSET];	/* tmp	DW			      */
	 RE_OPCODE	i6[LPUSHP];		/* l1:	PUSHP			  */
	 RE_OPCODE	i7[LCALL];		/*	CALL	y		      */
	 RE_OPCODE	i8[LPOPP];		/*	POPP			      */
	 RE_OPCODE	i9[LPTOM];		/*	PTOM	tmp		      */
	 } T_SMSTAR; 				/* x:	...			      */

typedef struct {
	 RE_OPCODE	i1[LPNEQM];		/*	PNEQM	tmp				*/
	 RE_OPCODE	i2[LJMP];		/*	JMP	l1					*/
	 } T_SMSTAR1;				/* y:	...					*/

typedef struct {
	 RE_OPCODE	i1[LPUSHM];		/* l1:	PUSHM	tmp		  */
	 RE_OPCODE	i2[LPTOM];		/*	PTOM	tmp		      */
	 RE_OPCODE	i3[LPUSHP];		/*	PUSHP			      */
	 RE_OPCODE	i4[LCALL];		/*	CALL	x		      */
	 RE_OPCODE	i5[LPOPP];		/*	POPP			      */
	 RE_OPCODE	i6[LPOPM];		/*	POPM	tmp		      */
	 RE_OPCODE	i7[LJMP];		/*	JMP	y		      	  */
	 RE_OPCODE	tmp[LOFFSET];	/* tmp	DW			      */
	 } T_STAR;					/* x:	...			      */

typedef struct {
	 RE_OPCODE	i1[LPNEQM];		/*	PNEQM	tmp		      */
	 RE_OPCODE	i2[LPTOM];		/*	PTOM	tmp		      */
	 RE_OPCODE	i3[LJMP];		/*	JMP	l1		      	  */
	 } T_STAR1;					/* y:	...			      */

typedef struct {
	 RE_OPCODE	i1[LANY];		/*	ANY			      */
	 } T_ANY;

typedef struct {
	 RE_OPCODE	i1[LBOL];		/*	BOL			      */
	 } T_BOL;

typedef struct {
	 RE_OPCODE	i1[LEOL];		/*	EOL			      */
	 } T_EOL;

typedef struct {
	 RE_OPCODE	i1[LSPTOM];		/*	SPTOM	tmp		      */
	 RE_OPCODE	i2[LPTOM];		/*	PTOM	tmp1		  */
	 RE_OPCODE	i3[LCALL];		/*	CALL	x		      */
	 RE_OPCODE	i4[LMTOP];		/*	MTOP	tmp1		  */
	 RE_OPCODE	i5[LJMP];		/*	JMP	y		          */
	 RE_OPCODE	tmp[LOFFSET];	/* tmp	DW			      */
	 RE_OPCODE	tmp1[LOFFSET];	/* tmp1 DW			      */
	 } T_NOTSIGN;				/* x:	...			      */

typedef struct {
	 RE_OPCODE	i1[LMTOSP];		/*	MTOSP	tmp		      */
	 RE_OPCODE	i2[LMTOP];		/*	MTOP	tmp1		  */
	 RE_OPCODE	i3[LRETURN];	/*	RETURN			      */
	 } T_NOTSIGN1;				/* y:	...			      */

typedef struct {
	 RE_OPCODE	i1[LLETTER];	/*	LETTER	c		      */
	 } T_LETTER;

typedef struct {
	 RE_OPCODE	i1[LPUSHP];		/* ln:	PUSHP			  */
	 RE_OPCODE	i2[LCALL];		/*	CALL	cn		      */
	 RE_OPCODE	i3[LPOPP];		/*	POPP			      */
	 RE_OPCODE	i4[LJMP];		/*	JMP	ln+1		      */
	 } T_LEFTOR; 				/* cn:	...			      */

typedef struct {
	 RE_OPCODE	i1[LJMP];		/*	JMP	y		      */
	 } T_ORSIGN;

typedef struct {
	 RE_OPCODE	i1[LRETURN];	/* cn+1:RETURN			  */
	 } T_RIGHTOR;				/* y:	...			      */

typedef struct {
	 RE_OPCODE	i1[LCCL];		/*	CCL <bits>		      */
	 } T_CCL;

typedef struct {
	 RE_OPCODE	i1[LMATCH];		/*	MATCH			      */
	 } T_EPILOG;

typedef struct {
	 RE_OPCODE	i1[LPREV];		/*	PREV	n		      */
	 } T_PREV;

//#if defined(KANJI)
typedef struct {
	 RE_OPCODE	i1[LRANGE];		/*	x1:x2 - y1:y2		  */
	 } T_RANGE;
//#endif

typedef union {
	 T_PROLOG	U_PROLOG;
	 T_LEFTARG	U_LEFTARG;
	 T_RIGHTARG	U_RIGHTARG;
	 T_SMSTAR	U_SMSTAR;
	 T_SMSTAR1	U_SMSTAR1;
	 T_STAR		U_STAR;
	 T_STAR1	U_STAR1;
	 T_ANY		U_ANY;
	 T_BOL		U_BOL;
	 T_EOL		U_EOL;
	 T_NOTSIGN	U_NOTSIGN;
	 T_NOTSIGN1	U_NOTSIGN1;
	 T_LETTER	U_LETTER;
	 T_LEFTOR	U_LEFTOR;
	 T_ORSIGN	U_ORSIGN;
	 T_RIGHTOR	U_RIGHTOR;
	 T_CCL		U_CCL;
	 T_EPILOG	U_EPILOG;
	 T_PREV		U_PREV;
//#if defined(KANJI)
	 T_RANGE	U_RANGE;
//#endif
	 } re_template ;

/* size of each compiled action */

const int cbIns[] =  {
/* PROLOG      0    */	sizeof (T_PROLOG      ),
/* LEFTARG     1    */	sizeof (T_LEFTARG     ),
/* RIGHTARG    2    */	sizeof (T_RIGHTARG    ),
/* SMSTAR      3    */	sizeof (T_SMSTAR      ),
/* SMSTAR1     4    */	sizeof (T_SMSTAR1     ),
/* STAR        5    */	sizeof (T_STAR	      ),
/* STAR1       6    */	sizeof (T_STAR1       ),
/* ANY	       7    */	sizeof (T_ANY	      ),
/* BOL	       8    */	sizeof (T_BOL	      ),
/* EOL	       9    */	sizeof (T_EOL	      ),
/* NOTSIGN     10   */	sizeof (T_NOTSIGN     ),
/* NOTSIGN1    11   */	sizeof (T_NOTSIGN1    ),
/* LETTER      12   */	sizeof (T_LETTER      ),
/* LEFTOR      13   */	sizeof (T_LEFTOR      ),
/* ORSIGN      14   */	sizeof (T_ORSIGN      ),
/* RIGHTOR     15   */	sizeof (T_RIGHTOR     ),
/* CCLBEG      16   */	sizeof (T_CCL	      ),
/* CCLNOT      17   */	sizeof (T_CCL	      ),
/* RANGE       18   */	0,
/* EPILOG      19   */	sizeof (T_EPILOG      ),
/* PREV        20   */	sizeof (T_PREV	      ),
//#if defined(KANJI)
/* RANGEJ1     21   */	sizeof (T_RANGE       ),
/* RANGEJ2     22   */	0,
//#endif
			0
			};

#if DEBUG
#if 0
static CString g_strBuf;
#define DEBOUT(x) { g_strBuf.Format x; AfxOutputDebugString(g_strBuf); } // gross, but under debug & works
#else
#define DEBOUT(x)
#endif
#else
#define DEBOUT(x)
#endif



//#if defined (KANJI)
/* MovePBackwards - Move P backwards
 *
 *
 */
unsigned char *MovePBackwards (unsigned char *bos, unsigned char *P)
{
	unsigned char *P1, *P2;

	P1 = P2 = bos;
	while (P1 < P) {
		P2 = P1;
		if (isleadbyte(*P1) )
			P1++;
		P1++;
	}

	return P2;
}

//#endif


/*  REMatch - enumerate all matches of a pattern onto a string
 *
 *	pat	compiled pattern (gotten from RECompile)
 *	bos	pointer to beginning of string to scan
 *	str	pointer to into bos of place to begin scan
 *	fFor	direction to move on unsuccessful compares (for <msearch> in Z)
 *
 *  REMatch returns 0 if a match was found.  Otherwise it returns a non-zero
 *  error code.
 *
 *  REMatch interprets the compiled patching machine in the pattern.
 */
int REMatch (struct patType *pat, LPCTSTR bos, LPCTSTR str, RE_OPCODE *Stack[],
						 unsigned MaxREStack, flagType fFor, /* out */ BOOL *pbLineMatch)
{
	char cLoc ;
	PPREOP	SP;					/* top of stack 		             */
	register RE_OPCODE *IP;		/* current instruction to execute    */
	register unsigned char *P;	/* pointer to next char to match     */
	RE_OPCODE	  C;
//#if defined(KANJI)
	unsigned short U;
	int fMatched;
//#endif
	int i, n;
	PPREOP StackEnd = (PPREOP) & Stack[MaxREStack-sizeof(Stack[0])];
	typedef int __cdecl fnCompType (const char *, const char *, size_t);
	fnCompType *pfncomp;

/*	for performance reasons, we cache theApp.m_fOEM_MBCS on the frame, since otherwise we pay a
    hit both in locality and in TLS overhead; however, note that this can't be static since
	theApp.m_fOEM_MBCS is set in the ctor for theApp! */
	BOOL bIsMBCS = theApp.m_fOEM_MBCS; // cache this on the stack instead of going through TLS hell repeatedly


	if ( pat == NULL)
		return REM_INVALID;

	if (pbLineMatch)
		*pbLineMatch = FALSE; // we'll assume false unless we find otherwise

	pfncomp = pat->fCase ? _tcsncmp : _tcsnicmp;

	/* initialize the machine */
	memset ((void *) pat->pArgBeg,cLoc = (char)(-1),(unsigned int) sizeof (pat->pArgBeg));
	P = (unsigned char *)str;
	pat->pArgBeg[0] = (char *)P;

	/* begin this instance of the machine */
	SP = (PPREOP) &Stack[-1];
	IP = pat->code;

	while (TRUE) {
		DEBOUT (("%04x/%04x/%04x ", IP, SP-&Stack[0], P));
		/* execute instruction */
		switch (*IP) {
			/* call a subroutine */
			case I_CALL:
				if (SP >= StackEnd)
					return REM_STKOVR;
				*++SP = IP + LCALL;
				IP = PNTR (ADDR (IP));
				DEBOUT (("CALL %04x\n", IP));
				break;

			/* return from a subroutine */
			case I_RETURN:
				DEBOUT (("RETURN\n"));
				IP = *SP--;
				break;

			/* match a character, fail if no match */
			case I_LETTER:
				if (bIsMBCS && isleadbyte(*P)) {
					// [olympus 2332 : kherold] Pay attention to case-sensitivity!
					if ((!pat->fCase && !_tcsnicmp((PCC)P, (PCC)(IP + 1), 2)) ||
							(pat->fCase && (*(PDBCS)P == *(PDBCS)(IP+1))))
					{
						 IP += LLETTER;
					}
					else
						 IP = *SP--;
					P += 2;
				}
				else {
					C = pat->fCase ? *P++ : XLTab[*P++];
					DEBOUT (("LETTER %c\n", IP[1]));
					if (C == IP[1])
						IP += LLETTER;
					else
						IP = *SP--;
				}
				break;

			/* match any character, fail if no match */
			case I_ANY:
				DEBOUT (("ANY\n"));
				if (bIsMBCS) {
					if (*P != '\0') {
						if (isleadbyte(*P))
							P++;
						IP += LANY;
					}
					else
						IP = *SP--;
				 	P++;
				}
				else {
					if (*P++ != '\0')
						IP += LANY;
					else
						IP = *SP--;
				}
				break;

			/* match end of line, fail if no match */
			case I_EOL:
				DEBOUT (("EOL\n"));
				if (*P == '\0')
				{
					IP += LEOL;
					if (pbLineMatch)
						*pbLineMatch = TRUE;
				}
				else
					IP = *SP--;
				break;

			/* match beginning of line, fail if no match */
			case I_BOL:
				DEBOUT (("BOL\n"));
				if (P == (unsigned char *)bos)
				{
					IP += LBOL;
					if (pbLineMatch)
						*pbLineMatch = TRUE;
				}
				else
					IP = *SP--;
				break;

			/* handle character class, fail if no match */
			case I_CCL:
				if (bIsMBCS) {
					IP += LCCL;
					fMatched = FALSE;
					U = pat->fCase ? *P++ : (unsigned char)XLTab[*P++];
					if (isleadbyte(U))
						U = (U << 8) + *P++;
					if (U == '\0') {
						IP = *SP--;
						break;
					}
#define USIP	((PUS)IP)
#define CLOW	(USIP[0])
#define CHIGH	(USIP[1])
					while (CLOW != 0) {
						fMatched |= (CLOW <= U) && (U <= CHIGH);
						IP += LRANGE;
					}
					IP += LRANGE;
#undef USIP
#undef CLOW
#undef CHIGH
					if (!fMatched)
						IP = *SP--;
				}
				else {
					C = pat->fCase ? *P++ : XLTab[*P++];
					DEBOUT (("CCL '%c', %d\n", C, C));
					if (C != '\0' && (IP[1 + (C >> 3)] & (1 << (C & 7))) != 0)
						IP += LCCL;
					else
						IP = *SP--;
				}
				break;

			/* handle not character class, fail if match */
			case I_NCCL:
				if (bIsMBCS) {
					IP += LCCL;
					fMatched = FALSE;
					U = pat->fCase ? *P++ : (unsigned char)XLTab[*P++];
					if (isleadbyte(U))
						U = (U << 8) + *P++;
					if (U == '\0') {
						IP = *SP--;
						break;
					}
#define USIP	((PUS)IP)
#define CLOW	(USIP[0])
#define CHIGH	(USIP[1])
					while (CLOW != 0) {
						fMatched |= (CLOW <= U) && (U <= CHIGH);
						IP += LRANGE;
					}
					IP += LRANGE;
#undef USIP
#undef CLOW
#undef CHIGH
					if (fMatched)
						IP = *SP--;
				}
				else {
					DEBOUT (("NCCL %c\n", C));
					C = pat->fCase ? *P++ : XLTab[*P++];
					if (C != '\0' && (IP[1 + (C >> 3)] & (1 << (C & 7))) == 0)
						IP += LNCCL;
					else
						IP = *SP--;
				}
				break;

			/* signal a match */
			case I_MATCH:
				DEBOUT (("MATCH\n"));
				pat->pArgEnd[0] = (char *)P;
				return REM_MATCH;

			/* jump to an instruction */
			case I_JMP:
				IP = PNTR (ADDR (IP));
				DEBOUT (("JMP %04x\n", IP));
				break;

			/* save the character pointer in a memory location */
			case I_PTOM:
				DEBOUT (("PTOM %04x\n", PNTR (ADDR(IP))));
				* ((PPUCH) PNTR (ADDR (IP))) = P;
				IP += LPTOM;
				break;

			/* restore the character pointer from a memory location */
			case I_MTOP:
				DEBOUT (("MTOP %04x\n", PNTR (ADDR(IP))));
				P = * ((PPUCH) PNTR (ADDR (IP)));
				IP += LMTOP;
				break;

			/* save the stack pointer in a memory location */
			case I_SPTOM:
				DEBOUT (("SPTOM %04x\n", PNTR (ADDR(IP))));
				* ((PPREOP*) PNTR (ADDR (IP))) = SP;
				IP += LSPTOM;
				break;

			/* restore the stack pointer from a memory location */
			case I_MTOSP:
				DEBOUT (("MTOSP %04x\n", PNTR (ADDR (IP))));
				SP = * ((PPREOP*) PNTR (ADDR (IP)));
				IP += LMTOSP;
				break;

			/* push the char pointer */
			case I_PUSHP:
				DEBOUT (("PUSHP\n"));
				if (SP >= StackEnd)
					return REM_STKOVR;
				*++SP = (RE_OPCODE *) P;
				IP++;
				break;

			/* pop the char pointer */
			case I_POPP:
				DEBOUT (("POPP\n"));
				P = (unsigned char *) (*SP--);
				IP ++;
				break;

			/* push memory */
			case I_PUSHM:
				DEBOUT (("PUSHM %04x\n", PNTR (ADDR (IP))));
				if (SP >= StackEnd)
					return REM_STKOVR;
				*++SP = * ((PPREOP) PNTR (ADDR (IP)));
				IP += LPUSHM;
				break;

			/* pop memory */
			case I_POPM:
				DEBOUT (("POPM %04x\n", PNTR (ADDR (IP))));
				* ((PPREOP) PNTR (ADDR (IP))) = *SP--;
				IP += LPOPM;
				break;

			/* make sure that the char pointer P is != memory, fail if necessary */
			case I_PNEQM:
				DEBOUT (("PNEQM %04x\n", PNTR (ADDR (IP))));
				if (P != * ((PPUCH) PNTR (ADDR (IP))))
					IP += LPNEQM;
				else
					IP = *SP--;
				break;

			/* move an immediate value to memory */
			case I_ITOM:
				DEBOUT (("ITOM %04x,%04x\n", PNTR (ADDR (IP)), IMM(IP)));
				* ((PPREOP) PNTR (ADDR (IP))) = IMM (IP);
				IP += LITOM;
				break;

			/* indicate a fail on the total match */
			case I_FAIL:
				DEBOUT (("FAIL\n"));
				P = (unsigned char *)pat->pArgBeg[0];
				if (bIsMBCS) {
					if (fFor) {
						if (isleadbyte(*P))
					 		P++;
						if (*P++ == '\0')
					 		return REM_NOMATCH;
						else
					 		;
					}
					else
						if (P == (unsigned char *)bos)
							return REM_NOMATCH;
						else
							P = MovePBackwards ((unsigned char *)bos, (unsigned char *)P);
				}
				else {
					if (fFor)
						if (*P++ == '\0')
					 		return REM_NOMATCH;
						else
					 		;
				 	else
						if (P-- == (unsigned char *)bos)
							return REM_NOMATCH;
				}

				pat->pArgBeg[0] = (char *)P;
				SP = (PPREOP) &Stack[-1];
				IP = pat->code;
				break;

			/* perform a match with a previously matched item */
			case I_PREV:
				i = IP[1];
				n = pat->pArgEnd[i] - pat->pArgBeg[i];
				DEBOUT (("PREV %04x\n", i));
				if (pat->pArgBeg[i] == (char *) -1)
					IP = *SP--;
				else
					if ((*pfncomp) (pat->pArgBeg[i], (const char *)P, n))
						IP = *SP--;
					else {
						IP += LPREV;
						P += n;
					}
				break;
			default:
				return REM_UNDEF;

		 }
	}
}

void PASCAL INTERNAL REStackOverflow ()
{
//	MsgBox(InternalError, SYS_RegExpr_StackOverflow); TODO: Fix MsgBox
}

/*  CompileAction - drop in the compilation template at a particular node
 *  in the tree.  Continuation appropriate to a node occurs by relying on
 *  passed input and past input (yuk, yuk).
 *
 *  type	type of action being performed
 *  u		previous return value.	Typically points to a previous
 *		template that needs to be linked together.
 *  x		low byte of a range
 *  y		high range of a range.
 *
 *  Returns	variable depending on action required.
 *
 */
unsigned INTERNAL CompileAction (OPTYPE type, register unsigned int u, unsigned char x, unsigned char y, 
								 patType *pat)
{
	register re_template *t = (re_template *) pat->REip;
	unsigned u1, u2, u3;

/*	for performance reasons, we cache theApp.m_fOEM_MBCS on the frame, since otherwise we pay a
    hit both in locality and in TLS overhead; however, note that this can't be static since
	theApp.m_fOEM_MBCS is set in the ctor for theApp! */
	BOOL bIsMBCS = theApp.m_fOEM_MBCS;

	DEBOUT (("%04x CompileAction %04x\n", pat->REip, type));

	pat->REip += cbIns[type];

	switch (type) {

		case PROLOG:
#define ip  (&(t->U_PROLOG))
			ip->i1[0] = I_CALL;	ADDR(ip->i1) = OFST (pat->REip);
			ip->i2[0] = I_FAIL;
			return (unsigned) NULL;
#undef	ip
			break;

		case LEFTARG:
#define ip  (&(t->U_LEFTARG))
			ip->i1[0] = I_PTOM;
			ADDR(ip->i1) = OFST ((RE_OPCODE *) &(pat->pArgBeg[pat->REArg]));
			ip->i2[0] = I_CALL;		ADDR(ip->i2) = OFST (pat->REip);
			ip->i3[0] = I_ITOM;
			ADDR(ip->i3) = OFST ((RE_OPCODE *) &(pat->pArgBeg[pat->REArg]));
			IMM(ip->i3) = (RE_OPCODE *) -1;
			ip->i4[0] = I_RETURN;
			return (unsigned) (pat->REArg)++;
#undef	ip
			break;

		case RIGHTARG:
#define ip  (&(t->U_RIGHTARG))
			ip->i1[0] = I_PTOM;
			ADDR(ip->i1) = OFST ((RE_OPCODE *) &(pat->pArgEnd[u]));
			return (unsigned) NULL;
#undef	ip
			break;

		case SMSTAR:
#define ip  (&(t->U_SMSTAR))
			return (unsigned) ip;
#undef	ip
			break;

		case SMSTAR1:
#define ip  ((T_SMSTAR *)u)
#define ip2 (&(t->U_SMSTAR1))
			ip->i1[0] = I_PUSHM;	ADDR(ip->i1) = OFST (ip->tmp);
			ip->i2[0] = I_CALL;		ADDR(ip->i2) = OFST (ip->i6);
			ip->i3[0] = I_POPM;		ADDR(ip->i3) = OFST (ip->tmp);
			ip->i4[0] = I_RETURN;
			/* DW */
			ip->i6[0] = I_PUSHP;
			ip->i7[0] = I_CALL;		ADDR(ip->i7) = OFST (pat->REip);
			ip->i8[0] = I_POPP;
			ip->i9[0] = I_PTOM;		ADDR(ip->i9) = OFST (ip->tmp);

			ip2->i1[0] = I_PNEQM;	ADDR(ip2->i1) = OFST (ip->tmp);
			ip2->i2[0] = I_JMP;		ADDR(ip2->i2) = OFST (ip->i6);
			return (unsigned) NULL;
#undef	ip
#undef	ip2
			break;

		case STAR:
#define ip  (&(t->U_STAR))
			return (unsigned) ip;
#undef	ip
			break;

		case STAR1:
#define ip  ((T_STAR *)u)
#define ip2 (&(t->U_STAR1))
			ip->i1[0] = I_PUSHM;	ADDR(ip->i1) = OFST (ip->tmp);
			ip->i2[0] = I_PTOM;		ADDR(ip->i2) = OFST (ip->tmp);
			ip->i3[0] = I_PUSHP;
			ip->i4[0] = I_CALL;		ADDR(ip->i4) = OFST (((RE_OPCODE *)ip) + sizeof (*ip));
			ip->i5[0] = I_POPP;
			ip->i6[0] = I_POPM;		ADDR(ip->i6) = OFST (ip->tmp);
			ip->i7[0] = I_JMP;		ADDR(ip->i7) = OFST (pat->REip);

			ip2->i1[0] = I_PNEQM;	ADDR(ip2->i1) = OFST (ip->tmp);
			ip2->i2[0] = I_PTOM;	ADDR(ip2->i2) = OFST (ip->tmp);
			ip2->i3[0] = I_JMP;		ADDR(ip2->i3) = OFST (ip->i1);
			return (unsigned) NULL;
#undef	ip
#undef	ip2
			break;

		case ANY:
#define ip  (&(t->U_ANY))
			ip->i1[0] = I_ANY;
			return (unsigned) NULL;
#undef	ip
			break;

		case BOL:
#define ip  (&(t->U_BOL))
			ip->i1[0] = I_BOL;
			return (unsigned) NULL;
#undef	ip
			break;

		case EOL:
#define ip  (&(t->U_EOL))
			ip->i1[0] = I_EOL;
			return (unsigned) NULL;
#undef	ip
			break;

		case NOTSIGN:
#define ip  (&(t->U_NOTSIGN))
			return (unsigned) ip;
#undef	ip
			break;

		case NOTSIGN1:
#define ip  ((T_NOTSIGN *)u)
#define ip2 (&(t->U_NOTSIGN1))
			ip->i1[0] = I_SPTOM;	ADDR(ip->i1) = OFST (ip->tmp);
			ip->i2[0] = I_PTOM;		ADDR(ip->i2) = OFST (ip->tmp1);
			ip->i3[0] = I_CALL;		ADDR(ip->i3) = OFST (((RE_OPCODE *)ip) + sizeof (*ip));
			ip->i4[0] = I_MTOP;		ADDR(ip->i4) = OFST (ip->tmp1);
			ip->i5[0] = I_JMP;		ADDR(ip->i5) = OFST (pat->REip);

			ip2->i1[0] = I_MTOSP;	ADDR(ip2->i1) = OFST (ip->tmp);
			ip2->i2[0] = I_MTOP;	ADDR(ip2->i2) = OFST (ip->tmp1);
			ip2->i3[0] = I_RETURN;
			return (unsigned) NULL;
#undef	ip
#undef	ip2
			break;

		case LETTER:
#define ip  (&(t->U_LETTER))
			if (!pat->fCase)
				x = XLTab[x];
			ip->i1[0] = I_LETTER;	ip->i1[1] = (RE_OPCODE) x;
			if (bIsMBCS)
				ip->i1[2] = (RE_OPCODE) y;
			return (unsigned) NULL;
#undef	ip
			break;

		case LEFTOR:
#define ip  (&(t->U_LEFTOR))
			* (PU) ip = u;
			return (unsigned) ip;
#undef	ip
			break;

		case ORSIGN:
#define ip  (&(t->U_ORSIGN))
			* (PU) ip = u;
			return (unsigned) ip;
#undef	ip
			break;

		case RIGHTOR:
			u1 = u;
			u2 = (unsigned int) t;
			while (u1 != (unsigned) NULL) {
				u  = * (PU)u1;
				u3 = * (PU) u;
				/*	u   points to leftor
				 *	u1  points to orsign
				 *	u2  points to next leftor
				 *	u3  points to previous orsign
				 */
#define ip  (&(((re_template *)u)->U_LEFTOR))
				ip->i1[0] = I_PUSHP;
				ip->i2[0] = I_CALL;	ADDR (ip->i2) = OFST (((RE_OPCODE *)ip) + sizeof (*ip));
				ip->i3[0] = I_POPP;
				ip->i4[0] = I_JMP;	ADDR (ip->i4) = OFST ((RE_OPCODE *) u2);
#undef	ip
#define ip  (&(((re_template *)u1)->U_ORSIGN))
				ip->i1[0] = I_JMP;	ADDR (ip->i1) = OFST (pat->REip);
#undef	ip
				u2 = u;
				u1 = u3;
			}
#define ip  (&(t->U_RIGHTOR))
			ip->i1[0] = I_RETURN;
#undef	ip
			return (unsigned) NULL;
			break;

		case CCLBEG:
#define ip  (&(t->U_CCL))
			if (bIsMBCS) {
				ip->i1[0] = I_CCL;
				return (unsigned) NULL;
			}
			else {
				char cLoc ;
				memset ((void far *) ip->i1, cLoc = '\0',(unsigned int) sizeof (ip->i1));
				ip->i1[0] = I_CCL;
				return (unsigned) ip;
			}
#undef	ip
			break;


		case CCLNOT:
#define ip  (&(t->U_CCL))
			if (bIsMBCS) {
				ip->i1[0] = I_NCCL;
				return (unsigned) ip;
			}
			else {
				char cLoc ;
				memset ((void far *) ip->i1, cLoc = '\0',(unsigned int) sizeof (ip->i1));
				ip->i1[0] = I_NCCL;
				return (unsigned) ip;
			}
#undef	ip
			break;

		case RANGEJ1:
#define ip  (&(t->U_RANGE))
			ip->i1[0] = (pat->fCase || x) ? y : XLTab[y];
			ip->i1[1] = x;
			return (unsigned) ip;
#undef	ip
			break;

		case RANGEJ2:
#define ip  ((T_RANGE *)u)
			ip->i1[2] = (pat->fCase || x) ? y : XLTab[y];
			ip->i1[3] = x;
			return (unsigned) NULL;
#undef	ip
			break;

		case RANGE:
#define ip  ((T_CCL *)u)
			{
			unsigned int ch;

			for (ch = x; ch <= y; ch++) {
				 x = pat->fCase ? (unsigned char) ch : XLTab[ch];
				 ip->i1[1 + (x >> 3)] |= 1 << (x & 7);
				 }
			return (unsigned) NULL;
			}
#undef	ip
			break;


		case EPILOG:
#define ip  (&(t->U_EPILOG))
			ip->i1[0] = I_MATCH;
			return (unsigned) NULL;
#undef	ip
			break;

		case PREV:
#define ip (&(t->U_PREV))
			ip->i1[0] = I_PREV;
			ip->i1[1] = (RE_OPCODE) u;
			return (unsigned) NULL;
#undef ip

		default:
//			MsgBox(InternalError, SYS_RegExpr_CompileAction);  TODO: fix that [fabriced]
			return (unsigned) NULL;
	}
}

#if DEBUG
void INTERNAL REDump ( struct patType *p)
{
	RE_OPCODE *REip = p->code;
	DEBOUT(("=> Start REDump <=\n"));
	while (TRUE) {
		DEBOUT (("%04x ", REip));
		switch (*REip) {
			case I_CALL:
				DEBOUT (("CALL    %04x\n", ADDR(REip)));
				REip += LCALL;
				break;
			case I_RETURN:
				DEBOUT ( ("RETURN\n"));
				REip += LRETURN;
				break;
			case I_LETTER:
				DEBOUT ( ("LETTER  %c\n", REip[1]));
				REip += LLETTER;
				break;
			case I_ANY:
				DEBOUT ( ("ANY\n"));
				REip += LANY;
				break;
			case I_EOL:
				DEBOUT ( ("EOL\n"));
				REip += LEOL;
				break;
			case I_BOL:
				DEBOUT ( ("BOL\n"));
				REip += LBOL;
				break;
			case I_CCL:
				DEBOUT ( ("CCL\n"));
				REip += LCCL;
				break;
			case I_NCCL:
				DEBOUT ( ("NCCL\n"));
				REip += LNCCL;
				break;
			case I_MATCH:
				DEBOUT(("=> End REDump <=\n"));
				DEBOUT ( ("MATCH\n"));
				return;
				break;
			case I_JMP:
				DEBOUT ( ("JMP     %04x\n", ADDR(REip)));
				REip += LJMP;
				break;
			case I_SPTOM:
				DEBOUT ( ("SPTOM   %04x\n", ADDR(REip)));
				REip += LSPTOM;
				break;
			case I_PTOM:
				DEBOUT ( ("PTOM    %04x\n", ADDR(REip)));
				REip += LPTOM;
				break;
			case I_MTOP:
				DEBOUT ( ("MTOP    %04x\n", ADDR(REip)));
				REip += LMTOP;
				break;
			case I_MTOSP:
				DEBOUT ( ("MTOSP   %04x\n", ADDR(REip)));
				REip += LMTOSP;
				break;
			case I_FAIL:
				DEBOUT ( ("FAIL\n"));
				REip += LFAIL;
				break;
			case I_PUSHP:
				DEBOUT ( ("PUSHP\n"));
				REip += LPUSHP;
				break;
			case I_PUSHM:
				DEBOUT ( ("PUSHM   %04x\n", ADDR(REip)));
				REip += LPUSHM;
				break;
			case I_POPP:
				DEBOUT ( ("POPP\n"));
				REip += LPOPP;
				break;
			case I_POPM:
				DEBOUT ( ("POPM    %04x\n", ADDR(REip)));
				REip += LPOPM;
				break;
			case I_PNEQM:
				DEBOUT ( ("PNEQM   %04x\n", ADDR(REip)));
				REip += LPNEQM;
				break;
			case I_ITOM:
				DEBOUT ( ("ITOM    %04x,%04x\n", ADDR(REip), IMM(REip)));
				REip += LITOM;
				break;
			default:
				DEBOUT ( ("%04x ???\n", *REip));
				REip += LOFFSET;
				DEBOUT(("=> End REDump <=\n"));
				return;
		}
	}
}
#endif

/*  EstimateAction - sum up the number of bytes required by each individual
 *  parsing action in the tree.  Take the input action and add it up to the
 *  running total.
 *
 *  type	type of action being performed
 *  u		dummy parm
 *  x		dummy parm
 *  y		dummy parm
 *
 *  Returns	0 always
 *
 */
unsigned INTERNAL EstimateAction (OPTYPE type, unsigned int u, unsigned char x, unsigned char y, patType *pat)
{
	u; x; y;

//	DEBOUT (("%04x EstimateAction %04x\n", RESize, type));

//	if (type < ACTIONMIN || type > ACTIONMAX)
//		MsgBox(InternalError, SYS_RegExpr_EstimateAction);  TODO: Fix MsgBox
	pat->RESize += cbIns[type];
	return (unsigned) 0;
}

/*  REEstimate - estimates the number of bytes required to
 *  compile a specified pattern.
 *
 *  REEstimate returns the number of bytes required to compile
 *  a pattern.	If there is a syntax error in the pattern, the function
 *  returns FALSE.
 *
 *  p		character pointer to pattern that will be compiled
 */
BOOL REEstimate (char *p, int *pSize)
{
	patType pat;
	ASSERT(pSize != NULL);
	pat.RESize = sizeof (struct patType) - 1;
	pat.REArg = 1;

	EstimateAction (PROLOG, (unsigned int) 0, '\0', '\0', &pat);

	if (REParseRE (EstimateAction, p, NULL, &pat) == NULL || pat.REArg > MAXPATARG)
		return FALSE;
	else
		EstimateAction (EPILOG, (unsigned int) 0, '\0', '\0', &pat);
	*pSize = pat.RESize;
	return TRUE;
}
