/*** island.h - global include file
*
*   Copyright <C> 1989, Microsoft Corporation
*
*   Purpose:
*
*   this file is portable, and contains include information for
*   the portable code in ISLAND.
*
*   Revision History:
*
*
*************************************************************************/

/* ISLAND definitions */

#define emsptr(x) (x)
#define ptrems(x) (x)
#define ems_alloc(size) ((char FAR *)_fmalloc(size))

#ifdef DOS5
void FAR * _fmalloc ( size_t );
#define IsSegEqual(a,b) (((a) & ~0x3) == ((b) & ~0x3))

#ifndef DOSBOX
#define SetExeType(a)	NOERROR
#endif

#define IsEmsPageLoaded(x) (TRUE)

#if (defined (EMSDEBUG) || defined (EMSSWAP)) && defined (DOS5)
// for debug do a 2's complement so NULL is still NULL
//#define emsptr(x) ((char FAR *) ((~((unsigned long) (x))) + 1) )
//
// ems debugging
//

#if (defined (EMSDEBUG) || defined (EMSSWAP)) && defined (DOS5)
char FAR * FAR emsptr ( void FAR * );
#endif

#define ptrems(x) emsptr(x)
#else
#define emsptr(x) (x)
#define ptrems(x) (x)
#endif

#define ems_alloc(size) ptrems(((char FAR *)_fmalloc(size)))
#else
#define IsSegEqual(a,b) ((a) == (b))
#endif

#define IsEmiEqual(a,b) ((a) == (b))

typedef uchar FAR *emstype; //* emsptr cast type

#define EMSPTR(a,b) ((a) emsptr((uchar FAR *) b))
#define PTREMS(a,b) ((a) ptrems((uchar FAR *) b))

extern	int IsEmiLoaded(ushort);

#define MAXINPLINE 256		/* longest user input line */
#define MAXLINE    256		/* longest source line */
#define MAXSTRING  128		/* longest string */

#ifndef SetExeType
extern int SetExeType( int );	// If not "defined", then need prototype
#endif	// !SetExeType

/* additional numbers for errno.h */

#define NOSYMBOLS		 101
#define NOTPACKED		 102	// file must be packed
#define MUSTRELINK		 103	// file must be relinked
#define FUTURESYMBOLS	 104	// file has OMF that is newer than this CV
#define PDBNOTFOUND      105    // pdb cannot be found
#define PDBBADSIGNATURE  106	// no symbolic information in pdb
#define PDBINVALIDAGE    107    // pdb is older than exe
#define PDBOLDFORMAT     108	// pdb is not current format
#define CV_NOERROR		   0	// no error condition
#define BADBKPTCMD		1001	// bad breakpoint command
#define SYNTAXERROR 	1017	// syntax error
#define NOCODE			1023	// no code at this line number
#define NOSOURCEHERE	1031	// address with no source line
#define MODLOADERROR	1042	// library module not loaded
#define NOTLVALUE		1050	// tp addr not lvalue

// Lang independent expression evaluator errors
#define CATASTROPHICTM	1250	// catastrophic TM error
#define EXPRERROR		1252	// to print the expression evaluators error strings!!
#define GEXPRERR		1253	// use the Expr Eval error saved by the message routines
#define BADADDR         1254    // not a valid address expression
#define NODATAMEMBERS   1255    // expanded a class with no data members

#define MODLOADED		2207	// loading OMF info for dll
#define NOADDRGIVEN     2212    // didn't specify an address
#define TOOMANYADDR     2213    // tried to specify more than one addr exp
#define MODNOMATCH      2214    // Mod name already loaded doesn't match
#define SYMSCONVERTED   2215
#define	EXPORTSCONVERTED 2216

#define EMMINSUFMEM 	3007	// Insufficent EMS memory

#define BADTYPECAST		1008	// bad type cast
#define NOROOM			3608	// out of memory

#define CVMSGMIN		(5000)

#define NOWATCHFUNC     (CVMSGMIN + 14) // Warning: Function Execution not Support in Watch Window
#define CLSTKCHGD       (CVMSGMIN + 15) // User changed memory/reg which
                                        // altered the call stack while
                                        // viewing function in walkback

/* Please note: This codes also enable the dependent functions to insure
   correct results. If a code is disabled, make sure all functions depending
   on it are disabled! That is if user_pc is disabled, so must ctxtIp and
   stack. */


typedef union fEnvir
	{
	unsigned short	 fAll;
	struct	fs
	    {
	    unsigned int     user_pc :1;	  /* indicates if user_pc is valid */
	    unsigned int     ctxtIp  :1;	  /* current context is valid */
	    unsigned int     stack   :1;	  /* stack frame is set up */
	    unsigned int     asmaddr :1;	  /* assembly addres is valid */
	    unsigned int	     	:12;	  /* reserved, must be zero */
	    } fs;
	} fEnvir;


/* enabling masks */

#define mUserPc     0x0001
#define mCtxtIp     0x0003		/* ctxtIp depends on user pc */
#define mStack	    0x0007		/* depends on ctxtIp and user pc */
#define mAsmaddr    0x0008

/* disabling masks */

#define mdUserPc     0xFFF8
#define mdCtxtIp     0xFFF9
#define mdStack      0xFFFB
#define mdAsmaddr    0xFFF7

/*
** Define for max length prefixed strings converted to sz style
*/
#define CCHMAXPREFIXTOSZ    (256)
