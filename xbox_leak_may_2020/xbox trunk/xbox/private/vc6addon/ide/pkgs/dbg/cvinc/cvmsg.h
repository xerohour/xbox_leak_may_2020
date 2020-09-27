/*** cvmsg.h - miscellaneous defines for cvmsg.c and anybody that needs it.
*
*   Copyright <C> 1989, Microsoft Corporation
*
*   Purpose:
*
*   Revision History:
*
*   [ds]    31-Aug-1989 dans	Changed prototype of CVMessage, added MESSAGEMSG
*   [xx]    08-Feb-1989 Dans	Created
*
*************************************************************************/

#define MAXERRMSG	256

enum _MSG_TYPE {
	ERRORMSG = 1,	// output="CVnnnn Error: %s", msgid=errno
	WARNMSG,		// output="CVnnnn Warning: %s", msgid=errno
	INFOMSG,		// output="%s", msgid=nsz
	MESSAGEMSG,		// output="CVnnnn Message: %s", msgid=errno
	FMESSAGEMSG,	// output="CVnnnn Message: %s", msgid=errno referring to
					//   string which has a "%s" in it, 1st vararg=lsz
	EXPREVALMSG		// output="CVnnnn Error: %s", 1st vararg=lsz
	};

enum _MSG_WHERE {
	CMDWINDOW = 1,	// if command window is open, display there, otherwise
					//   message box
	MSGBOX,			// message box
	STATLINE,		// status line
	MSGSTRING,		// 1st vararg=lsz where message is written
    MSGGERRSTR,     // save to global err string, to be output later when
                    //   MSGID==GEXPRERR
    CVMSGSTRING
	};

typedef int				MSGTYPE;
typedef unsigned int	MSGID;		// must not be short--VCNT needs 4 bytes
typedef int				MSGWHERE;

//
//  api call
//
extern "C" void		CVMessage ( MSGTYPE, MSGID, MSGWHERE, ... );
extern void PASCAL	CVExprErr ( EESTATUS, MSGWHERE, PHTM, char FAR * );
