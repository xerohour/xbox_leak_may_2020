/*
 *  History:
 *
 *	??-???-???? ?? Created
 *	15-Sep-1988 bw Added REM_*, changed REMatch parameters.
*/

#ifndef __REMI_H__
#define __REMI_H__

#define MAXPATARG   10			/* 0 is entire 1-9 are valid	     */

/* Return codes from REMatch */

#define REM_MATCH   0		    /* A match was found		    */
#define REM_NOMATCH 1		    /* No match was found		    */
#define REM_UNDEF   2		    /* An undefined Op-code was encountered */
#define REM_STKOVR  3		    /* The stack overflowed		    */
#define REM_INVALID 4		    /* A parameter was invalid		    */


#define MAX_USER_LINE		2048	//Max inside length of editor line

extern char XLTab[256];
typedef char flagType;
typedef unsigned char RE_OPCODE;

/* structure of compiled pattern */

struct patType {
    flagType	fCase;				/* TRUE => case is significant	     */
    reSyntax	reSyntx;			/* unix, brief, epsilon or zibo      */
	int			RESize;				/* estimated size */
	RE_OPCODE *	REip;				/* Instruction pointer */
	int			REArg;
    char *		pArgBeg[MAXPATARG];	/* beginning of tagged strings	     */
    char *		pArgEnd[MAXPATARG];	/* end of tagged strings	     */
    RE_OPCODE	code[1];			/* pseudo-code instructions	     */
};

#define RE_LOCAL	0				// Indices for global vars above:  Only two simultaneous searches allowed,
#define RE_GREP		1				// one from grep and one local to a file. dolphin 7522: [kherold]


int		REMatch(struct patType  *,LPCTSTR,LPCTSTR,RE_OPCODE *[], unsigned, char, /* out */ BOOL *pbLineMatch);
char	REGetArg(struct patType  *,int ,char  *);
char	RETranslate(struct patType  *,char  *,char  *);
int		RELength(struct patType  *,int );
char	*REStart(struct patType  *);

#endif	// __REMI_H__
