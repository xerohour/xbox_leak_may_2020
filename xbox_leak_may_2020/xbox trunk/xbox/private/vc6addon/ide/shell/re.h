/*  re.h - common include files for regular expression compilers
 */

#define INTERNAL    near

/* defined actions for parsing */

typedef  int OPTYPE ;

#define ACTIONMIN   ((OPTYPE) 0)

#define PROLOG	    ((OPTYPE) 0)
#define LEFTARG     ((OPTYPE) 1)
#define RIGHTARG    ((OPTYPE) 2)
#define SMSTAR	    ((OPTYPE) 3)
#define SMSTAR1     ((OPTYPE) 4)
#define STAR	    ((OPTYPE) 5)
#define STAR1	    ((OPTYPE) 6)
#define ANY	    ((OPTYPE) 7)
#define BOL	    ((OPTYPE) 8)
#define EOL	    ((OPTYPE) 9)
#define NOTSIGN     ((OPTYPE) 10)
#define NOTSIGN1    ((OPTYPE) 11)
#define LETTER	    ((OPTYPE) 12)
#define LEFTOR	    ((OPTYPE) 13)
#define ORSIGN	    ((OPTYPE) 14)
#define RIGHTOR     ((OPTYPE) 15)
#define CCLBEG	    ((OPTYPE) 16)
#define CCLNOT	    ((OPTYPE) 17)
#define RANGE	    ((OPTYPE) 18)
#define EPILOG	    ((OPTYPE) 19)
#define PREV	    ((OPTYPE) 20)

// International (Japan only)
#define RANGEJ1     ((OPTYPE) 21)
#define RANGEJ2     ((OPTYPE) 22)
#define ACTIONMAX	((OPTYPE) 22)

/*	query */
BOOL	FParseExpr(UINT, BOOL);		// Check to see if an expression is valid

/*  function forward declarations */

char			 fREMatch (struct patType *,char *,char *,char );
char			 REGetArg (struct patType *,int ,char *);
int			 RELength (struct patType *,int );
char *			 REStart (struct patType *);

typedef unsigned INTERNAL ACT (OPTYPE, unsigned int,
			       unsigned char, unsigned char, patType *);

typedef ACT *PACT;

ACT			 CompileAction;
ACT			 EstimateAction;
ACT			 NullAction;

int	PASCAL	INTERNAL RECharType (char *);
int	PASCAL	INTERNAL RECharLen (char *);
int	PASCAL	INTERNAL REClosureLen (char *);
BOOL PASCAL INTERNAL FREAltExpr(char *, patType *);
char *	PASCAL INTERNAL FEpsASCII(char *, long *);
BOOL PASCAL INTERNAL FEpsTab(char *);
char *	PASCAL	INTERNAL REParseRE (PACT, char *,int *, patType *);
char *	PASCAL	INTERNAL REParseE (PACT,char *, patType *);
char *	PASCAL	INTERNAL REParseGroup (PACT,char *, patType *);
char *	PASCAL	INTERNAL REParseSE (PACT,char *, patType *);
char *	PASCAL	INTERNAL REParseAltE (PACT, register char *, patType *);
char *	PASCAL	INTERNAL REParseClass (PACT,char *, patType *);
char *	PASCAL	INTERNAL REParseAny (PACT,char *, patType *);
char *	PASCAL	INTERNAL REParseXAscii(PACT, char *, patType *);
char *	PASCAL	INTERNAL REParseCaret(PACT, char *, patType *);
char *	PASCAL	INTERNAL REParseWild (PACT,char *, patType *);
char *	PASCAL	INTERNAL REParseBOL (PACT,char *, patType *);
char *	PASCAL	INTERNAL REParsePrev (PACT, char *, patType *);
char *	PASCAL	INTERNAL REParseEOL (PACT,char *, patType *);
char *	PASCAL	INTERNAL REParseAlt (PACT,char *, patType *);
char *	PASCAL	INTERNAL REParseNot (PACT,char *, patType *);
char *	PASCAL	INTERNAL REParseAbbrev (PACT,char *, patType *);
char *	PASCAL	INTERNAL REParseChar (PACT,char *, patType *);
char *	PASCAL	INTERNAL REParseClosure (PACT,char *, patType *);
char *	PASCAL	INTERNAL REParseGreedy (PACT,char *, patType *);
char *	PASCAL	INTERNAL REParsePower (PACT,char *, patType *);
char	PASCAL	INTERNAL REClosureChar (char *, patType *);
char	PASCAL	INTERNAL Escaped (char );

void	PASCAL	INTERNAL REStackOverflow (void);
BOOL	REEstimate (char *, int *);

#ifdef _DEBUG
void INTERNAL REDump (struct patType *p);
#endif

