#ifndef CL_INCLUDED
#define CL_INCLUDED

typedef enum {
    cltNone,
    cltPub,
    cltProc,
    cltBlk,
} CLT;

typedef struct _FME {
    char FAR *symbol;
    char	clt;
	BOOL	fSetProc;	 /* has CLSetProcAddr been called? */
	LSZ		lsz;		 /* cached display string */
    ADDR	addrProc;    /* start of procedure, or block */
    ADDR	addrRet;
    ADDR	addrCSIP;    /* current return location */
    FRAME	Frame;
    HMOD	module;      /* pointer to module */

} FME; // FraMe Entry
typedef FME FAR *   LPFME;

extern int	PASCAL CLLookupAddress( ADDR );
extern void PASCAL CLGetWalkbackStack (  HPID, HTID, UINT );
extern void PASCAL CLFreeWalkbackStack( BOOL );
extern char *  PASCAL CLGetProcName(int, char *,int,BOOL);
extern BOOL CLDispCallInfo( int iCall );

void PASCAL CLDumpStackItem ( int );
void PASCAL CLDumpStackTrace ( void );

PCXF PcxfLocal ( void );

typedef HLLE        HFME;

extern HLLI hlliFME;
extern int iCallsCxf;

int  PASCAL CLChfmeOnStack( void );
HFME PASCAL CLHfmeGetNth( int );

#endif	// !CL_INCLUDED
