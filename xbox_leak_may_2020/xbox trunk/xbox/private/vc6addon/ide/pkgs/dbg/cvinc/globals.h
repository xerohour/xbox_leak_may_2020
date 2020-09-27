/*** globals.h -- global variable extern header file
*
*   Copyright <C> 1989, Microsoft Corporation
*
*   Purpose:
*
*   Revision History:
*
*   [ 6]    13-Apr-1990 ArthurC Change the reg structure
*   [ 5]    26-Mar-1990 ArthurC Abolish symbol_name
*   [ 4]    03-Apr-1989 dans	Added szCONSOLE, for space savings.
*   [ 3]    01-Apr-1989 dans	Started using C6.00 runtime support for
*								10 byte reals.	Bug 34.  Added printf formats
*								for some floating point output. (externs from
*								isledata.c).
*   [ds]    17-Mar-1989 dans	Redid expression evaluator flags.
*   [ 2]    24-Jan-1989 dans	Added extern of _fDosExt under DOSE switch.
*   [ 1]    10-Jan-1989 dans	Added pfPlayback extern from auxcow.c
*		    19-Mar-1987 brianwi Added help_topics array
*
*************************************************************************/

/* register table and friends */

extern ADDR asmaddr;

extern char *ptxt;

/* text selection */

extern char		*ptst;		    /* ptr to text selection buffer */
extern char		input_text[];
extern char		text_selected;
extern char		text_selection_inserted;
extern char		text_selection[80];

extern int radix;				// radix to use for output
extern int radixInput;			// used to be #defined, but need to change
								// on the fly

extern char		fTargetIs386;	// hpidCurr is on a 386 or higher
extern char		fTry386Mode;	// user specified Options.386 Instructions
extern char		fIn386Mode;		// always == fTargetIs386 && fTry386Mode

extern FILE *	pfPlayback;											// [ 1]

#if defined (DOSE)	// [ 2]
extern unsigned short	_fDosExt;									// [ 2]
#endif	// [ 2]

//  miscellaneous externs from isledata.c

extern CXF cxfIp;

//  defines for targeting the mac

#ifdef MAC

#define cbTargetMax 256

#endif
