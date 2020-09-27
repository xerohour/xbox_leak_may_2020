/*** cvexpr.h - header file for codeview expression evaluator stuff
*
*   Copyright <C> 1989, Microsoft Corporation
*
*   Purpose:
*
*   Revision History:
*
*   [05]    17-Aug-1990 Glennn	changed ESSet_eval to char far *
*   [04]    11-Jul-1990 Glennn	changed fIsLoaded to fIsFound
*   [03]    06-Jul-1990 Glennn	Added def of hDLL field and fIsloaded
*   [2]     18-May-1989 ArthurC Installed Windows Message tracing
*   [ds]    24-Apr-1989 dans	Added BPDEADTHREAD.
*
*************************************************************************/

#define clanMax 10

typedef struct _lan {			// language
    char *		szFile;	    	// full path to file name after ESFindExpr call
    char *		szName;	    	// name displayed to user
    char *		szTitle;
    char *		szExt;	    	// ext by default it will be .dll
	char FAR *	lszIsIdChar;	// chars that are legal in identifiers
	char FAR *	AssignOp; 		// assignment operator for language
	PEXF		pexf; 			// point to struct that holds ee initialization stuff
    int			Radix;	    	// current
    char		fCaseSensitive; // current
    ushort		hDLL;    		// handle to dll DOS3
    int			fIsFound;       // [03][04]whether the file has been found
    } LAN;

extern LAN     rglan[];
extern int     clan;
extern int     fAuto;
extern PEXF    pEXF;

extern long		PASCAL CVGetNbr(char FAR *, int, int,
					PCXF, char FAR *, int FAR *);
extern int      PASCAL CVGetCastNbr(char FAR *, CV_typ_t, int, int,
					PCXF, char FAR *, char FAR *);
extern int		PASCAL CVGetList(char *, int, int, PCXF,
					int, char [], int *);
extern int		PASCAL CVEvaluate(char FAR *, int, int, PCXF, char *);

extern void PASCAL  ESInitExpr( char *szEval );
extern void PASCAL  ESDefaultExpr( void );
extern void PASCAL  ESSet_language(void);
extern void PASCAL  ESLoadExpr(int, unsigned int, LPFNEEINIT);
extern void	PASCAL  ESUnLoadAllExpr (void);
extern int 	PASCAL  ESilan (void);
extern int 	PASCAL  ESSetFromIndex (int);
extern void	PASCAL  ESSet_eval (char  FAR *);
extern void	PASCAL  ESSet_auto (int);
extern void	PASCAL  ESShow_language (void);
extern int  PASCAL  ESFindEEIndex ( char *);
