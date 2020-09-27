/****************************************************************************
 Aug 1993 Split codemgr.c into dbg.h, dbgosd.c, and dbgmisc.c
 [MarkBro]

 Revision J.M. Dec 1991
 This file is derived from CV400
 SYSTEM   layer ( SYSTEM.C LOAD.C CONFIG.C )

 CODEMGR  code management
 OSDEBUG  CallBack
 CONFIG   Dll configuration
 LOAD     OSDebug loading

 ****************************************************************************/
#include "stdafx.h"
#pragma hdrstop

#include <dbgver.h>
#include "dbgosdp.h"
#include "autobp.h"
#include "autodbg.h"
#include "dbgenc.h"
#define _XBDM_
#define XbeGetLocalModulePathA HrXbeGetLocalModulePath
#include <xboxdbg.h>

#include "fbrdbg.h"
#ifdef _DEBUG

	#define new DEBUG_NEW
	#undef THIS_FILE
	static char BASED_CODE THIS_FILE[] = __FILE__;

	extern "C" DEBUG_VERSION ('V', 'C', "Visual C++ Code Editor, Browser, Debugger, Builder");

	#include "osdstr.h"
	
#else

	extern "C" RELEASE_VERSION ('V', 'C', "Visual C++ Code Editor, Browser, Debugger, Builder");

#endif


//
//	External Function Prototypes
//

extern CXF CxfWatch (void);
extern BOOL CheckAssertionFailed ( LPADDR );
HTHD HthdLive (HPRC);

CExecutionExts	g_ExecutionExts;

#define DECL_DBC(name, fRequest, dbct) dbct,
const DBCT mpdbcdbct[] =
{
	#include <dbc.h>
};
#undef DECL_DBC


//
// 	Global Variables
//


static CDbgEvent evtOSDDone;
static CDbgEvent evtOSDHurry;
static CDbgEvent evtQueueEmpty;


fEnvir 	fEnvirGbl;
CVF*	lpcvf 		= NULL;			// Pointer to CVF entry functions
SHF*	lpshf = NULL;				// Pointer to SHF entry structure
CXF 	cxfIp;						// Global context

HLLI 	qCallBack;					// Our internal notifications linked list
HLLI 	qCallBackReturns;			// Return codes from processed notifications
BOOL 	fDbgTimerSet = FALSE;
CRITICAL_SECTION	csDbgTimerSet;	// guards fDbgTimerSet



ULONG	pidDebuggee =  NULL;
BOOL 	fInFindLocalDlg = FALSE;



#ifdef _DEBUG
BOOL fHackFlipScreen = FALSE;	// flip screen on F8/F10?
#endif

VOID
WINAPIV
DebugPrint(
    LPTSTR szFormat,
    ...
    );

static BOOL g_fCrashDump = FALSE;

BOOL
IsCrashDump(
	)
{
	return g_fCrashDump;
}

BOOL
SetCrashDumpDebugging(
	BOOL	fCrashDump
	)
{
	BOOL	temp = g_fCrashDump;

	g_fCrashDump = fCrashDump;
	return temp;
}


// since MFC dialogs pump messages, we want to make sure we
// don't go recursive in DoAllCallbacks and DoAllCallbacksAndQueueReturn.
// That would be bad. [piersh]
// This class provides a mechanism for preventing this recursion.
class CBlockRecursion
{
public:
	CBlockRecursion () { s_ulcRecurse++; }
	~CBlockRecursion () { s_ulcRecurse--; }

	BOOL HasRecursed () const { return (s_ulcRecurse > 1); }

protected:
	static ULONG s_ulcRecurse;
};

ULONG CBlockRecursion::s_ulcRecurse = 0;





//
// End Global Variables
//

void PrimeDbgMessagePump (void)
{
	evtOSDDone.Reset ();
	evtOSDHurry.Reset ();
}

void DbgMessagePump (BOOL fPump)
{
	HANDLE rgh[2];

	/* FUTURE: we don't properly deal with WM_QUIT */

	// CUDA #4360
	// give the debuggee some time to run with us blocked before
	// starting the pump.  This gives us a chance to run
	// synchronously for short debuggee bursts which in turn
	// helps us to avoid painting problems and keystroke loss
	// because the debuggee would be 'running' when those messages
	// are processed and that might prevent us from doing the
	// correct painting/command processing... [rm]
	//
	// evtOSDHurry will be set by the run thread if it is waiting
	// for the main thread to finish up processing of a callback.
	// For example, if a DLL is loaded, the run thread will block
	// until the main thread is done processing the dbcModLoad;
	// without evtOSDHurry, every time you stepped over a LoadLibrary
	// in the user's source there would be an unnecessary half-second
	// delay.

	rgh[0] = evtOSDDone.m_h;
	rgh[1] = evtOSDHurry.m_h;

	if (WaitForMultipleObjects (2, rgh, FALSE, 500) != WAIT_OBJECT_0)
	{
		if (fPump)
		{
			theApp.IdleAndPump (evtOSDDone.m_h);
		}
		else
		{
			rgh[1] = evtOSDDone.m_h;
			rgh[0] = evtOSDHurry.m_h;
			for(;;)
			{
				DWORD dwResult = WaitForMultipleObjects (2, rgh, FALSE, lpprcCurr->funcExecTimerID ? 1000 : INFINITE);
				if (dwResult == WAIT_TIMEOUT)
				{
					// if we have a timer event, we need to dispatch it so that the timeout
					// on function evaluation gets done
					MSG msg;
					if (::PeekMessage( &msg, NULL, WM_TIMER, WM_TIMER, PM_REMOVE) &&
						(lpprcCurr->funcExecTimerID==msg.wParam) )
						DispatchMessage( &msg );
				}
				else if (dwResult == WAIT_OBJECT_0)
				{
					evtOSDHurry.Reset ();
					/* The NT em/dm pair really doesn't want you to call
					   ** OSDGetThreadStatus on a dbcCreateThread cause it won't be
					   ** setup yet and will hang waiting for its own thread create
					   ** semaphore, so you must call this from a different thread.
					 */
					DoCallBackAndQueueReturn ();
					rgh[1] = evtOSDDone.m_h;
					rgh[0] = evtOSDHurry.m_h;
				}
				else
					break;
			}
		}
	}

	// reset evtOSDDone immediately in case of nested pumps
	evtOSDDone.Reset ();
}

void ExitDbgMessagePump (void)
{
	evtOSDDone.Set ();

	// make sure the message pump exits GetMessage
	PostMessage (hwndFrame, WM_NULL, 0, 0);
}

#ifdef _DEBUG					// {

/*********************************************************************
 ToggleHackFlipScreen

 Hack for testing: if pressing Ctrl-Shift-Alt-F toggles a screen
 flip on F8 and F10 in addition to the usual screen flip on F5.
 ALSO, it changes the way it flips: instead of giving focus to
 the debuggee's top-level window, it gives focus to whatever window
 had it when the debuggee last stopped.
 *********************************************************************/

VOID ToggleHackFlipScreen (VOID)
{
	char sz[100];

	fHackFlipScreen = !fHackFlipScreen;
	if (DebuggeeAlive ())
	{
		PrimeDbgMessagePump ();
		if (xosdNone == OSDSystemService (hpidCurr, htidCurr, ssvcHackFlipScreen, &fHackFlipScreen, 1, NULL))
		{
			DbgMessagePump ();
		}
	}
	// Note, we do NOT have to stick this string in the resources,
	// because this feature is just an internal testing thing which
	// is only enabled in the debug version
	//sprintf (sz, "Flipscreen hack turned %s.", fHackFlipScreen ? "ON" : "OFF");
	//gpISrc->StatusText(SYS_My_String, STATUS_INFOTEXT, FALSE, sz);
}

#endif // } _DEBUG

/*********************************************************************
 FContinueThreadTerm

 Purpose  : When receiving a dbcThreadTerm, we should stop running
 the debuggee if we were trying to step that thread.
 This routines checks what we were trying to do and
 returns TRUE if we should continue running the thread,
 FALSE otherwise.
 Arguments: hpid = process, htid = thread that died
 Returns  : TRUE if we should continue running, FALSE otherwise
 *********************************************************************/
BOOL FContinueThreadTerm (HPID hpid, HTID htid)
{
	BOOL fContinue = TRUE;

	HPRC hprc;
	LPPRC lpprc;
	BOOL fSqlInproc = FALSE ;

	//
	// If we're doing SQL inproc debugging, we *always* want to
	// return TRUE from this function
	//

	hprc = HprcFromHpid (hpid);
	if (hprc != NULL)
	{
		lpprc = (LPPRC) LLLpvFromHlle (hprc);

		if (lpprc)
		{
			fSqlInproc = lpprc->fSqlInproc ;
		}

		UnlockHlle (hprc);
	}

	if (fSqlInproc)
    {
		return TRUE ;
	}

	if ((hpid == hpidCurr) &&
		(htid == lpprcCurr->htidExecuting) &&
        (HthdLive(hprcCurr)) &&
		(lpprcCurr->GetDebuggeeStateEx () != edsKilling)
		)
	{
		switch (lpprcCurr->exp)
		{
		case expStopped:
		case expGo:

#if 0
			// Step Out is sometimes implemented by setting a breakpoint
			// past the function return and doing a Go, so we need to
			// check for this case.  If we're doing a Step Out and the
			// thread we're stepping died, stop.
			if (fDoingStepOut)
				fContinue = FALSE;
#endif

			break;

		case expStep:
		case expRange:
		case expAfterRet:
			// thread that was stepping has died
			fContinue = FALSE;
			break;

		default:
			ASSERT (FALSE);		// should be one of the above

			break;
		}
	}

	return fContinue;
}

// [CAVIAR #7412 #7397 01/25/93 v-natjm]
/*********************************************************************
 Implementation   : J.M. Jan 1993
 SYCheckContinue  :

 Purpose  : Checks if we can continue or not
 arguments: handle on process
 Returns  : TRUE if ok
 *********************************************************************/
BOOL SYCheckContinue (HPRC hprc)
{
	LPPRC lpprc = (LPPRC) LLLpvFromHlle (hprc);
	HPID hpid = lpprc->hpid;
	BOOL fRet = TRUE;
	ADDR addrCSIP;
	HTHD hthd;
	LPTHD lpthd;
	BOOL fThreadOk = FALSE;

	// [cuda#4433 5/17/93 mikemo]  If the thread that was running has died,
	// then we should stop any Step commands, but Go commands can keep running.
	hthd = LLHlleFindLpv (lpprc->llthd, 0, &lpprc->htidExecuting, cmpHtid);
	if (hthd != 0)
	{							// does thread still exist?

		lpthd = (LPTHD) LLLpvFromHlle (hthd);
		fThreadOk = (lpthd->tdf != tdfDead);	// is thread still alive?

		UnlockHlle (hthd);
	}
	if (!fThreadOk)
	{
		// The thread that was executing has died.
		fRet = FContinueThreadTerm (hpid, lpprc->htidExecuting);
		if (fRet)
		{
			// The thread that was executing a Go command has died.
			// We should find some other thread to make the current one.
			hthd = 0;
			while ((hthd = LLHlleFindNext (lpprc->llthd, 0)) != 0)
			{
				lpthd = (LPTHD) LLLpvFromHlle (hthd);
				if (lpthd->tdf != tdfDead)	// is thread still alive?

					break;		// yes, use this one

				UnlockHlle (hthd);
			}

			if (hthd == 0)
			{
				// couldn't find a good thread
				fRet = FALSE;
			}
			else
			{
				// found a thread to use
				lpprc->htidExecuting = lpthd->htid;
				UnlockHlle (hthd);
			}
		}
	}

	if (fRet && lpprc->exp == expRange)
	{
		ADDR addrStart = lpprc->addrStart;
		ADDR addrEnd = lpprc->addrEnd;

		// [cuda#4805 5/25/93 mikemo]  I don't see how this code ever worked
		// -- it used to compare fixed up addresses to un-fixed up ones
		SYUnFixupAddr (&addrStart);
		SYUnFixupAddr (&addrEnd);

		// Get the current CS:IP
		SYGetAddr (hpid, lpprc->htidExecuting, adrPC, &addrCSIP);

		// Check if the address is out of range or what...
		if ((SYCmpAddr (&addrCSIP, &addrStart) < 0) ||
			(SYCmpAddr (&addrCSIP, &addrEnd) > 0))
		{
			fRet = FALSE;
		}
	}

	UnlockHlle (hprc);
	return fRet;
}

/*********************************************************************
 Implementation   : J.M. Fev 1992
 ProcessInfoAvail : CODEMGR.C / SYSTEM.C

 Purpose  : Process info avail in real time in the callback
 itself because the debuggee restarts right after
 exiting CallBack...
 arguments: handle on a CBP
 Returns  : None, we just continue
 *********************************************************************/
void ProcessInfoAvail (HLLE hcbp)
{
	char *pchCur;
	LPCBP lpcbp = (LPCBP) LLLpvFromHlle (hcbp);

#if !(defined (OSDEBUG4))
	INF FAR *lpinf = (INF FAR *) (lpcbp->lParam);

#else
	LPINFOAVAIL lpinf = (LPINFOAVAIL) (lpcbp->lParam);

#endif
	LPB lpb = (LPB) lpinf->buffer;

	// get system service type
#if !(defined (OSDEBUG4))
	WORD wFunction = lpinf->wFunction;

#else
	WORD wFunction = ssvcNull;

#endif
	BYTE buf[1024];
	WORD i = 0;
	UINT nDebugCommandID = OwinGetDebugCommandID ();

	switch (wFunction)
	{
	case ssvcNull:
	case ssvcDumpLocalHeap:
	case ssvcDumpGlobalHeap:
	case ssvcDumpModuleList:
		// copy FAR string to near buffer
		_ftcsncpy ((LPSTR) buf, (const char *) lpb, sizeof (buf) - 1);
		buf[sizeof (buf) - 1] = '\0';
		break;

	case ssvcCrackLocalHmem:
	case ssvcCrackGlobalHmem:
		{
			address_t FAR *lpaddr_t = (address_t FAR *) lpinf->buffer;

			_snprintf ((char *) buf, sizeof (buf), "\n%x:%lx", lpaddr_t->seg, lpaddr_t->off);
			break;
		}

	case ssvcFreeLibrary:
		break;

	case ssvcInput:
		_snprintf ((char *) buf, sizeof (buf), "%02x\n", *lpb);
		break;

	default:
		ASSERT (FALSE);
		break;
	}

	UnlockHlle (hcbp);

	// Strip out all embedded "\r"s.  This catches both the "\r\n"
	// and the "\n\r" cases.
	{
		UCHAR *pchSrc;
		UCHAR *pchDst;
		int cb;

		for (pchSrc = pchDst = buf; *pchSrc; pchSrc = (UCHAR *) _ftcsinc ((char *) pchSrc))
		{
			if (*pchSrc != '\r')
			{
				cb = _ftclen ((char *) pchSrc);
				_ftccpy ((char *) pchDst, (char *) pchSrc);
				pchDst += cb;
			}
		}
		*pchDst = '\0';
	}

	// Here goes the OutputDebugString...
	// Display this info in the error window...

	// Call OutputWindowQueueHit() multiple times if there are
	// embedded '\n's.
	//
	// [CAVIAR 4799: 12/2/92 mattg]

	pchCur = (char *) buf;

	while (*pchCur != '\0')
	{
		int nchEndLine;
		char *pchLF;
		char *pchNewline;
		char chChopped;

		pchLF = _ftcschr (pchCur, _T ('\n'));

		if (pchLF)
		{
			nchEndLine = 1;
			pchNewline = pchLF;
		}
		else
		{
			UINT nchBuf = _ftcslen (pchCur);

			// If we have exceeded the max line length
			if (nchBuf > MAX_USER_LINE)
			{
				pchNewline = pchCur + nchBuf;
			}
			else
			{
				break;
			}
		}

		// Cannot insert more than the user_line limit (so chop it up)
		// Subsequent lines will have spaces prepended
		if ((pchNewline - pchCur) > MAX_USER_LINE)
		{
			pchNewline = &pchCur[MAX_USER_LINE];
			// Save current character
			chChopped = *pchNewline;
			// We will back up one character to insert a space
			nchEndLine = -1;
		}

		*pchNewline = '\0';
		OutputWindowQueueHit (nDebugCommandID, (LPSTR) pchCur, TRUE, TRUE);

		// If we split a line
		if (nchEndLine < 0)
		{
			// replace the chopped character & prepend a space
			*pchNewline = chChopped;
			pchCur = pchNewline + nchEndLine;
			*pchCur = ' ';
		}
		else
		{
			pchCur = pchNewline + nchEndLine;
		}
	}

	if (*pchCur != '\0')
		OutputWindowQueueHit (nDebugCommandID, (LPSTR) pchCur, TRUE, FALSE);

#if defined (OSDEBUG4)
//	OSDInfoReply (hpidCurr, htidCurr, 0, 0);
#endif
}

/*********************************************************************
 Implementation     : J.M. Fev 1992
 ProcessInfoRequest : CODEMGR.C / SYSTEM.C

 Purpose  : Process info requested in real time in the callback
 itself because the debuggee restarts right after
 exiting CallBack... and WINDOWS KERNEL is
 waiting for something like a Fatal error answer !
 arguments: handle on a CBP
 Returns  : None, we just continue
 *********************************************************************/
void ProcessInfoRequest (HLLE hcbp)
{
	char c;
	LPCBP lpcbp = (LPCBP) LLLpvFromHlle (hcbp);
	CBP cbp = *lpcbp;
	CString str;

	UnlockHlle (hcbp);

	if (DebuggerMsgBox (Error, MsgText (str, DBG_Windows_Fatal_Error),
						MB_OKCANCEL) == IDOK)
	{
		c = 'i';
	}
	else
	{
		c = 'a';
	}
	if (OSDInfoReply (cbp.hpid, cbp.htid, (LPV) & c, sizeof (c)) != xosdNone)
	{
		ASSERT (FALSE);
	}
}

/***************************************************************************
 Revision        : J.M. Dec 1991
 UpdateUserEnvir : ( CV1.C )

 Purpose         : setup the context
 Arguments       : setup flags (ISLAND.H)
 mUserPc | mAsmaddr | mCtxtIp
 ***************************************************************************/
void PASCAL UpdateUserEnvir (unsigned short fReq)
{
	// Free up the old walkback stack, just to make sure
	// that we don't have any old stackwalking info!
	// This used to be done in makeeng.c, but it would
	// screw up the call stack window while the debuggee
	// is running
	CLFreeWalkbackStack (TRUE);

	// Don't try to update anything if the child is dead !
	if (!DebuggeeAlive ())
	{
		// we don't have a context anymore in 32-bit (aka separate address
		// spaces) so nuke our one
		// [matthewt] CUDA bug fix #3084
		memset (&cxfIp, 0, sizeof (CXF));
		UpdateAllDebugWindows (&cxfIp);
		return;
	}

	if (((((fReq & mUserPc) == mUserPc) && !fEnvirGbl.fs.user_pc) ||
		 (((fReq & mCtxtIp) == mCtxtIp) && !fEnvirGbl.fs.ctxtIp)) &&
		hpidCurr != 0)
	{
		ADDR addr;

		SYGetAddr (hpidCurr, htidCurr, adrPC, (LPADDR) & addr);
		if (!SHSetCxt (&addr, SHpCXTFrompCXF (&cxfIp)))
		{
			*SHpADDRFrompCXT (SHpCXTFrompCXF (&cxfIp)) = addr;
		}
		SYSetFrame (SHpFrameFrompCXF (&cxfIp));
		fEnvirGbl.fs.user_pc = TRUE;
		fEnvirGbl.fs.ctxtIp = TRUE;
	}
	if ((hpidCurr != 0) && (fReq & mAsmaddr) && (!fEnvirGbl.fs.asmaddr))
		SYGetAddr (hpidCurr, htidCurr, adrPC, (LPADDR) & asmaddr);
}

/*********************************************************************
 Revision    : J.M.  Mar 1992
 Go          : CV0.C

 Purpose     : runs the child
 *********************************************************************/
void PASCAL Go (HTHD hthd, BOOL fGoException, BOOL fFlipScreen)
{
	EXOP exop = {0};

	exop.fSingleThread = hthd != 0;
	exop.fPassException = fGoException;

	// MWFreeze();
	if (!DebuggeeAlive ())
	{
		errno = ESRCH;
		return;
	}

	// Set the debuggee Focus and Active window
	exop.fSetFocus = fFlipScreen;

	//AuxPrintf1("No emul.") ;
	if (!SYGo (&exop, hpidCurr, htidCurr))
	{
		//can't assert this - the remote connection could be broken
		//ASSERT(FALSE);
	}
	else if (fBlocking)
	{
		nsWaitForDebugEvent ();
	}
}

/*********************************************************************
 Revision :           J.M.  Mar 1992
 nsWaitForDebugEvent :  SYSTEM.C

 Purpose  : Wait for any callback message considered as an
 interesting event
 Arguments: none
 return   : none
 *********************************************************************/
void nsWaitForDebugEvent (void)
{
	// [CAVIAR #5487 11/17/92 v-natjm]
	if (lpprcCurr && lpprcCurr->exp != expStopped)
	{
		DoCallBacksUntil ();
	}
}

/*********************************************************************
 Revision :         J.M.  Sept 1992
 EventLeft:

 Purpose  : Check if any event left in the stack
 Arguments: none
 *********************************************************************/
BOOL PASCAL EventLeft (void)
{
	return (LLChlleInLl (qCallBack) > 0);
}

/*********************************************************************
 Revision :         J.M.  Mar 1992
 CheckCallBack :    SYSTEM.C

 Purpose  : Check if any  callback messages in our message linked list
 Arguments: bDequeueReturnCodes : if TRUE, this function will first
 see if there are any return codes from previously
 processed notifications, and if there are, then it will
 just dequeue the first of these return codes instead of
 processing a new callback message.
 return   : xosdQueueEmpty / xosdContinue / 2
 *********************************************************************/
XOSD PASCAL CheckCallBack (BOOL bDequeueReturnCodes)
{
	XOSD xosd = xosdQueueEmpty;
	HLLE hret;
	XOSD FAR *lpxosd;

	// Before processing any messages that have not yet been dequeued,
	// we may want to return the return codes for messages that were
	// dequeued earlier.
	if (bDequeueReturnCodes)
	{
		hret = LLHlleFindNext (qCallBackReturns, 0);

		// any return codes to return?
		if (hret != NULL)
		{
			lpxosd = (XOSD FAR *) LLLpvFromHlle (hret);

			xosd = *lpxosd;

			UnlockHlle (hret);
			lpxosd = NULL;
			(void) LLFDeleteHlleFromLl (qCallBackReturns, hret);

			return xosd;
		}
	}

	LLAcquire (qCallBack);
	if (LLChlleInLl (qCallBack) > 0)
	{
		HLLE hcbp = LLHlleFindNext (qCallBack, 0);
		LPCBP lpcbp = (LPCBP) LLLpvFromHlle (hcbp);
		CBP cbp = *lpcbp;

		UnlockHlle (hcbp);
		lpcbp = NULL;			// what this pointed to is no longer valid

		(void) LLFDeleteHlleFromLl (qCallBack, hcbp);
		LLRelease (qCallBack);
		xosd = DoCallBack (&cbp);

		// 'destruct' the CBP: free any allocations that may have
		// been done in CallBack()
		if (cbp.wMessage == dbcModLoad ||
			cbp.wMessage == dbcError ||
			cbp.wMessage == dbcMsgBpt ||
			cbp.wMessage == dbcException ||
			cbp.wMessage == dbcModFree)
		{
			delete[](BYTE *) cbp.lParam;
		}
	}
	else
	{
		LLRelease (qCallBack);
		evtQueueEmpty.Set ();
	}

	return xosd;
}

/*********************************************************************
 DoCallBackAndQueueReturn

 Purpose  : Process callback messages, and queue the return
 codes from the processing.  These return codes will
 later be returned to whoever calls CheckCallBack.

 Here's what's going on: when an OSDebug callback is
 received, after the callback is enqueued on
 qCallBack, the DBGCALLBACKTIMERID timer is turned
 on.  When the frame window sees a WM_TIMER message
 with this ID, it calls this function.  The reason
 for doing this is so that OSDebug callbacks are
 processed as soon as possible rather than not being
 processed until OSDebug has returned from whatever
 you've asked it to do (OSDebug is usually blocking).

 One problem is that the processing of an OSDebug
 callback results in a return code which is of interest,
 but which is meaningless at WM_TIMER time.  So
 we stick the return code on a queue.  This way when
 someone later is repeatedly calling CheckCallBack,
 he will first get the return codes from all these
 notifications that have already been processed, and
 then, if there are any real notifications left, they
 will get processed and their return codes returned.
 Arguments: none
 return   : none
 *********************************************************************/



void
DoCallBackAndQueueReturn(
	)
{
	// Don't allow recursion in this function (constructor and destructor
	// do all the work).
	CBlockRecursion block;
	if (!block.HasRecursed ())
	{
		XOSD xosd;
		HLLE hret;
		XOSD FAR *lpxosd;

		while ((xosd = CheckCallBack (FALSE)) != xosdQueueEmpty)
		{
			// Add a new element to our callback parameters list
			hret = LLHlleCreate (qCallBack);
			// Create pointer from element
			lpxosd = (XOSD FAR *) LLLpvFromHlle (hret);
			*lpxosd = xosd;
			UnlockHlle (hret);
			LLAddHlleToLl (qCallBackReturns, hret);
		}

		// [cuda#3652 5/4/93 mikemo]  Now that we're done dequeuing messages,
		// turn off the timer that allowed us to dequeue them.
		QCEnterCritSection (&csDbgTimerSet);
		if (fDbgTimerSet)
		{
			ASSERT(IsWindow(hwndFrame));

			KillTimer(hwndFrame, DBGCALLBACKTIMERID);

			fDbgTimerSet = FALSE;
		}
		QCLeaveCritSection (&csDbgTimerSet);
	}
}

/*********************************************************************
 Revision :            J.M.  Mar 1992
 DoCallBacksUntil :  SYSTEM.C

 Purpose  : Process the callback messages until an interesting
 event...
 Arguments: none
 return   : TRUE if an event occurred
 *********************************************************************/
VOID DoCallBacksUntil (void)
{
#if 1 //!(defined (OSDEBUG4))
	// This routine used to keep looping if it saw xosdQueueEmpty.
	// That was necessary because various OSDebug operations like
	// Step and Go used to be asynchronous.  Now, those routines are
	// all synchronous, so the logic becomes much simpler: it's just
	// the same thing as DoAllCallBacks().

	DoAllCallBacks ();

#else
	XOSD xosd = xosdContinue;
	BOOL fContinue;

	do
	{
		fContinue = TRUE;
		do
		{
			xosd = CheckCallBack (TRUE);
			// Break this if any interesting debug event...
			if (!(xosd == xosdContinue ||
				  xosd == xosdNone ||
				  xosd == xosdQueueEmpty))
			{
				return;
			}

			// If we had to leave while processing all, remember it
			if (xosd == xosdNone)
				fContinue = FALSE;
		}
		while (xosd != xosdQueueEmpty);
		if (!fContinue)
			xosd = xosdNone;
	}
	while (xosd == xosdContinue || xosd == xosdQueueEmpty || fContinue);
#endif
}


/****************************************************************************

 FUNCTION:  DebugMessage()

 PURPOSE:

 RETURNS:   Nothing.

 ****************************************************************************/

char FAR DebugMessage[256];
void FAR PASCAL DebuggerMessage (MsgBoxTypes MsgType, UINT wErrorId, LPSTR Msg, int ShowType)
{
	char OutPut[MAX_VAR_MSG_TXT];
	char FAR *lpsOut;

	// Strip leading \n's
	while (*Msg == '\n')
		Msg++;
	lpsOut = Msg;

	*DebugMessage = '\0';
	*OutPut = '\0';
	if (wErrorId != 0)
	{
		LoadString (hInst, wErrorId, szTmp, sizeof (szTmp));
		wsprintf ((LPSTR) OutPut, (LPSTR) szTmp, (LPSTR) Msg);
		lpsOut = (char FAR *) OutPut;
	}

	if (AutoTest)
	{
		char szBuffer[_MAX_PATH];
		PSTR szAutoTest = "msgbox.log";
		PSTR pszBuffer;

		HANDLE fd = INVALID_HANDLE_VALUE; //BUILD FIX: Init var

		*szBuffer = '\0';
		GetModuleFileName (hInst, szBuffer, sizeof (szBuffer));
		// Replace exe name with szAutoTest
		pszBuffer = _ftcsrchr (szBuffer, _T ('\\'));
		if (pszBuffer == NULL)
		{
			pszBuffer = szBuffer;
		}
		else
		{
			pszBuffer++;
		}
		_ftcscpy (pszBuffer, szAutoTest);

		fd = CreateFile(szBuffer, GENERIC_WRITE, FILE_SHARE_READ, 0, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
		if (fd != INVALID_HANDLE_VALUE)
		{
			DWORD dwBytesWritten = 0;

			// seek to the end of file
			SetFilePointer (fd, 0, NULL, FILE_END);
			// write the text
			WriteFile (fd, lpsOut, (DWORD) _ftcslen (lpsOut), &dwBytesWritten, NULL);
			WriteFile (fd, CrLf, 2, &dwBytesWritten, NULL);
			// close the file
			CloseHandle (fd);
		}
	}
	else
	{
		switch (ShowType)
		{
		case SHOW_IMMEDIATE:
			MsgBox (MsgType, Msg);
			break;
		case SHOW_ASYNCHRONOUS:
			_ftcsncpy (DebugMessage, Msg, sizeof (DebugMessage) - 1);
			DebugMessage[sizeof (DebugMessage) - 1] = '\0';
			break;
		case SHOW_REPORT:
		default:
			{
				//DBGOutputDebugString((LPSTR)Msg,FALSE) ;
				OutputWindowQueueHit (OwinGetDebugCommandID (),
									  (LPSTR) Msg, FALSE, TRUE);

				break;
			}
		}
	}
}

/*************************************************************************

 Performs source or disassembly single-step, over or thru calls.

 Entry conditions:
 fOverCalls: TRUE skips over calls/ints, FALSE traces through them
 sto: describes type of step to perform

 Exit conditions:
 single_step:
 TRUE if successful
 errno:
 set to error if single_step == FALSE

 Merged functionality of Step and StepN
 ***************************************************************************/

int PASCAL Step (BOOL fOverCalls, STO sto)
{
	// go to main (entry point)
	if (lpprcCurr->stp == stpNotStarted)
	{
		CXT cxt;

		if ((sto & stoQueryStep) && get_initial_context (&cxt))
		{
			ADDR addr;

			// we need to continue after the dbcEntryPoint
			lpprcCurr->fBpInitialCxt = TRUE;

			// The code fixes the bug that occurs if the LoadComplete address == the
			// InitialContext address: we'll set the breakpoint at the
			// initial context and do a Go; since we're already at the
			// bp, we'll actually run to completion.

			// check to see if we're already at the initial context
			SYGetAddr (hpidCurr, htidCurr, adrPC, &addr);
			if (FAddrsEq(addr, cxt.addr))
				lpprcCurr->fBpInitialCxt = FALSE;

			// set a BP at, and run to, the initial context
// v-vadimp this will skip the main routine's prologue by moving the breakpoint
// location beyond it
#if defined(_M_ALPHA)
//            ADDR BPaddr = *SHpADDRFrompCXT(&cxt);
//			SetAddrOff(&BPaddr, SHGetDebugStart (SHHPROCFrompCXT (&cxt)));
			SetAddrOff(SHpADDRFrompCXT(&cxt), SHGetDebugStart (SHHPROCFrompCXT (&cxt)));
//			GoUntil (&BPaddr, 0, BPTMP, FALSE, TRUE);
#endif           
			GoUntil (SHpADDRFrompCXT (&cxt), 0, BPTMP, FALSE, TRUE);

            if (lpprcCurr == NULL)
                return STEPN_FAILED;

			// make sure we could set all our BPs correctly

			if (!lpprcCurr->IsBPsBound ())
			{
				return STEPN_BPFAILED;
			}

			// if we stopped before the initial context, or
			// the user put her own BP here, don't continue
			SYGetAddr (hpidCurr, htidCurr, adrPC, &addr);
			if (SYCmpAddr (&addr, &cxt.addr) ||
				BPBrkExec (&addr, htidCurr) != BPHARDCODEDBP )
			{
				return STEPN_BREAK;
			}
		}
		else
		{
			// we want to stop when we get the dbcEntryPoint
			lpprcCurr->fBpInitialCxt = FALSE;

			//
			//	This will stop at the program's entry point
			
			Go ((HTHD) NULL, sto & stoPassEx, TRUE);

            if (lpprcCurr == NULL)
                return STEPN_FAILED;

			//
			// If this is JIT, not attach to active, then we are at a break.
			
			if (theApp.m_jit.GetActive () && theApp.m_jit.GetEvent ())
			{
				return STEPN_BREAK;
			}
			
			if (!lpprcCurr->IsBPsBound () || !lpprcCurr->IsBeyondEntryPoint ())
			{
				return STEPN_BREAK;
			}
		}
	}
	else
	{
		ADDR addr;
		BOOL fSuccess;

		WORD wLn;
		SHOFF cbLn;
		SHOFF dbLn;

		sto = (STO) (sto | stoInitialBP);

		if (!DebuggeeAlive ())
		{
			errno = ESRCH;
			return STEPN_BREAK;
		}

		// this is set in SYStop
		lpprcCurr->fHitBP = FALSE;

		// if we're source stepping and we can, then range step
		if ((sto & stoQueryStep) &&
			SYGetAddr (hpidCurr, htidCurr, adrPC, &addr) == xosdNone &&
			SLLineFromAddr (&addr, &wLn, &cbLn, &dbLn))
		{
			ADDR addrEnd = addr;

			// cbLn is always the bytes count -1 !!!
			SetAddrOff (&addrEnd, (GetAddrOff (addr) + cbLn - dbLn));
			AuxAddr (&addr);
			AuxAddr (&addrEnd);
			fSuccess = SYRangeStep (fOverCalls, sto, addr, addrEnd,
									hpidCurr, htidCurr);
		}
		else
		{
			// otherwise, single step
			fSuccess = SYSingleStep (fOverCalls, sto, hpidCurr, htidCurr);
		}
		

		//
		//	NOTE NOTE NOTE NOTE
		//
		// It is possible that when we go down in a step, by the time we get
		// back we will have already stopped debugging; hence lpprcCurr==NULL.
		// The REAL PROBLEM, as far as I'm concerned, is that stepping waits
		// in the stupid message loop.  Fix this next time around.
		//
		
		if (!fSuccess || lpprcCurr == NULL)
		{
			return STEPN_NOERROR;
		}

		// Wait for the step...
		DoAllCallBacks ();

		// if we hit a BP...

		if (lpprcCurr == NULL) {
			return STEPN_NOERROR;
		}
		
		if (lpprcCurr->fHitBP)
		{
			return STEPN_BREAK;
		}
	}

	return STEPN_NOERROR;
}



/***************************************************************************
 LoadJITProject

 Purpose  : Load the JIT exe as a project
 Arguments: None
 Returns  : Nothing
 ***************************************************************************/
void LoadJITProject (void)
{
	// We should have gotten a dbcModLoad for the exe.
	if (theApp.m_jit.FPathIsReal ())
	{
		CPath pathMakefile;

		// When VPROJ calls us to query whether it's okay to open
		// a project, we need to know that it is in fact okay since
		// we're opening a project for JIT.
		fLoadingJitProject = TRUE;

#ifdef DOLPHIN_V3				// [dolphin #4538 3/10/94 mikemo] consider this next time
		if (!pProjectDocTemplate->FFindMakefileForTarget (
										 theApp.m_jit.GetPath (), pathMakefile))
#endif
		{
			pathMakefile = theApp.m_jit.GetPath ();
		}

		if (gpIProjectWorkspace)
		{
			CDocument *pDoc ;
			
			if ( gpIProjectWorkspace->OpenWorkspace(&pDoc, pathMakefile, TRUE) != S_OK )
				// Can't create the makefile - don't try to create it again
				// unless the path is updated
				theApp.m_jit.FSetPath(pathMakefile, FALSE);
		}

		fLoadingJitProject = FALSE;
	}
}


BOOL
VerifyImagesInSync(
	HPID	hpid,
	LPCTSTR	LocalImage,
	LPCTSTR	RemoteImage
	)
/*++

Routine Description:

	Check if the files LocalImage and RemoteImage are the same file.  This
	is done by looking at the Time Stamp and Check Sum in the header of both
	of the image files.  If they do not match, give an error.

Comments:

	Since this function does UI, in can only be called in the main thread
	(not the DmPollThread).

--*/
{
	ULONG	RemoteCheckSum;
	ULONG	RemoteTimeStamp;
	ULONG	LocalCheckSum;
	ULONG	LocalTimeStamp;
	SHE		she;
	XOSD	xosd;
	BOOL	fCheckTimeStamp;
	BOOL	fCheckCheckSum;
	BOOL	fRet;
	
	xosd = OSDGetTimeStamp (hpid,
							NULL,
							(LPTSTR) RemoteImage,
							&RemoteTimeStamp,
							&RemoteCheckSum
							);

	switch (xosd)
	{
		case xosdNone:
			// no error
			break;

		case xosdFileNotFound:
			ErrorBox (ERR_Remote_Debuggee_Missing);
			return FALSE;

		case xosdBadFormat:
			ErrorBox (ERR_Remote_Exe_Invalid, RemoteImage);
			return FALSE;

		default:
			ErrorBox (ERR_RemoteExe_Cannot_Open);
			return FALSE;
	}


	she = SHGetExeTimeStamp ((LPSTR) LocalImage, &LocalTimeStamp,
							 &LocalCheckSum);

	switch (she)
	{
		case sheNone:
			// no error
		break;

		case sheCorruptOmf:
			ErrorBox (ERR_Corrupt_Omf, LocalImage);
			return FALSE;

		case sheFileOpen:
		default:
				
			ErrorBox (ERR_File_Open, LocalImage);
			return FALSE;
	}


	fCheckTimeStamp = TRUE;
	fCheckCheckSum = TRUE;
	
	if (LocalTimeStamp == (ULONG) -1 || RemoteTimeStamp == (ULONG) -1)
		fCheckTimeStamp = FALSE;	

	if (LocalCheckSum == (ULONG) -1 || RemoteCheckSum == (ULONG) -1)
		fCheckCheckSum = FALSE;

	fRet = TRUE;

	if (fCheckTimeStamp && fRet)
		fRet = (LocalTimeStamp == RemoteTimeStamp);

	if (fCheckCheckSum && fRet)
		fRet = (LocalCheckSum == RemoteCheckSum);


	if (!fRet)
	{
		if (QuestionBox (ERR_App_Exe_Mismatch, MB_YESNO) == IDYES)
			fRet = TRUE;
	}

	return fRet;
}


BOOL
RemoteDebugging(
	)
{
	if (pDebugCurr && pDebugCurr->MtrcRemoteTL ())
		return TRUE;

	return FALSE;
}



BOOL
LoadDebuggee(
	LPTSTR	lszLocalExe,
	LPTSTR 	lszRemoteExe,
	LPTSTR 	lszDebuggeeArgs,
	BOOL	fDebugActive,
	ULONG	DebugActivePid,
	HANDLE	DebugActiveEvent,
	BOOL	fRestart,
	BOOL	fCrashDump
	)
/*++

Routine Description:

	Process all the loading including the OMF attached to our debugging
	session.

Arguments:

	lszLocalExe - Local EXE name.

	lszRemoteExe - Remote EXE name.

	lszDebuggeeArgs - Arguments to the program.

	fDebugActive - 	Boolean whether we are debugging an active process or not.
				   	In the case that this is true, we may be doing either
					JIT debugging or Attach To Active debugging.  Also, when
					this is true, lszLocalExe will not be the correct image
					name.

	DebugActivePid - Pid of active proces to debug. Unused otherwise.

	DebugActiveEvent - Event of active process to debug.  Unused otherwise.

	fRestart - TRUE if we are loading this debuggee as a result of a restart.


Return Value:

	Bool: true for successfully loaded/executed the process; false otherwise.


Comments:

	This routine uses tons of global state information (global variables).  I
	have tried to comment most of them with the hopes that we can remove
	them, but be forewarned.


			DON'T YOU DARE PUT ANOTHER GLOBAL VARIBLE IN HERE

--*/
{
	BOOL 		fRet = TRUE;
	ULONG		pid = 0;
	XOSD 		xosd;
	TCHAR 		szOemLocalExe[_MAX_PATH];
	HCURSOR 	hSaveCursor;
	CString 	strDebuggerTitle;
	BOOL		fFailure = FALSE;

	
	ASSERT ( !(fCrashDump && fDebugActive));

	_ftcscpy (szOemLocalExe, lszLocalExe);
	_ftcscpy (szTmp, lszDebuggeeArgs);

	if (!SYCreateHprc (szOemLocalExe, szTmp, 0, 0))
		return FALSE;

	ASSERT (lpprcCurr);
	
	if (fRestart)
		lpprcCurr->SetStepMode (STEP_MODE_FROM_WINDOW);		

	if (pDebugCurr)
		pDebugCurr->OnDebugStart (hpidCurr);

	if (RemoteDebugging ())
	{
		if (!VerifyImagesInSync (hpidCurr, szOemLocalExe, lszRemoteExe))
		{
			OSDDestroyHpid (hpidCurr);
			lpprcCurr->hpid = NULL;
			return FALSE;
		}
	}
	
	if (!fDebugActive) {
		if (!CheckEXEForDebug (szOemLocalExe, FALSE, FALSE)) {
			fFailure = TRUE;
		}
	}

	if (!fFailure && !LoadAdditionalDLLs (NULL, TRUE)) {
		fFailure = TRUE;
	}


	if (fFailure) {
		OSDDestroyHpid (hpidCurr);
		lpprcCurr->hpid = hpidNull;

        SHUnloadSymbolHandler( TRUE );
		return FALSE;
	}
		
		
    if (!fDebugActive && !fCrashDump) {

		//
	    // Load all the OMF associated to the EXE and additional DLLs
		//
		
	    if (!SYLoadDebugInfo ())
	    {
			OSDDestroyHpid (hpidCurr);
			lpprcCurr->hpid = hpidNull;
			return FALSE;
	    }
    }

	// set global context to null
	memset (&cxfIp, 0, sizeof (CXF));		// HACK: global state variable

	if (IsCrashDump ()) {
		SetModeName (TBR_Mode_CrashDump);
	} else {
		SetModeName (TBR_Mode_Run);
	}

	VERIFY (strDebuggerTitle.LoadString (AFX_IDS_APP_TITLE));

	DBMI dbmi = {0};
	dbmi.hWndFrame = hwndFrame;
	dbmi.hWndMDIClient = NULL; // FUTURE: Nuke this parameter!
	
	OSDSetDebugMode (hpidCurr,
					 dbmSoftMode,
					 &dbmi, sizeof (dbmi));

	hSaveCursor = SetCursor (LoadCursor (NULL, IDC_WAIT));

	// Update Status bar with Load Symbols
	gpISrc->StatusText (DBG_Load_Symbols, STATUS_INFOTEXT, FALSE);

	PrimeDbgMessagePump ();


	if (fDebugActive) // doing just-in-time debugging?
	{
		struct DAP
		{
			DWORD 	dwProcessID;		// ID of process to attach to
			HANDLE 	hEventGo;			// Event handle to SetEvent when ready to
		};
		
		DAP dap = {0};

		dap.dwProcessID = DebugActivePid; 		// theApp.m_jit.GetPid ();
		dap.hEventGo = DebugActiveEvent;		// theApp.m_jit.GetEvent ();
		xosd = OSDDebugActive (hpidCurr, &dap, sizeof (dap));
	}
	else
	{
		CString strWorkingDir;
		gpIBldSys->GetWorkingDirectory(ACTIVE_BUILDER, strWorkingDir);

		// If no working dir specified, and project is local, use project dir
		if (strWorkingDir.IsEmpty () && !pDebugCurr->MtrcRemoteTL ())
		{
			CDir dir;
			LPCSTR pszPath;

			gpIBldSys->GetBuilderFile(ACTIVE_BUILDER, &pszPath);
			if (pszPath[0])
				dir.CreateFromPath(pszPath);

			strWorkingDir = dir;
		}

		// Must be on in case we're redirecting IO.
		ulChildFlags |= ulfInheritHandles;

		xosd = OSDProgramLoad (hpidCurr,
							  lszRemoteExe,
							  lpprcCurr->szArgs ? lpprcCurr->szArgs : "",
							  (LSZ) (const char *) strWorkingDir,
							  (LSZ) (const char *) strDebuggerTitle,
							  ulChildFlags);
	}

	if (xosd == xosdNone) {

		//
		// Wait for dbcLoadComplete.  Do NOT pump msgs.
		//
		
		DbgMessagePump (FALSE);
	}

	fRet = (xosd == xosdNone);

	if (fRet) {
		xosd = DoAllCallBacks ();
	}

	//
	//	There are two error cases:
	//
	// 	(1)	OSDProgramLoad/OSDDebugActive failed (returned an xosd error code).
	//  	In this case, we need to display an appropriate error.
	// 	(2) OSDProgramLoad/OSDDebugActive succeeded, but then later we
	//     	received a dbcError, in which case lpprcCurr->stp was left
	//     	as stpNotStarted instead of being changed to stpStopped.  In
	//     	this case, an error has already been displayed (in dbcError
	//    	processing), and we just need to clean up and exit.
	//

	if (!fRet || lpprcCurr->stp == stpNotStarted || lpprcCurr->stp == stpDead)
	{
		// NOTE: the callee should be doing this!!!
		// turn off just-in-time debugging
		theApp.m_jit.SetActive (FALSE);

		// If load failed, we must make sure no one thinks it's still around.
		// Most importantly, calling TerminateDebuggee() will set hthdCurr to 0

		// If OSDProgramLoad was successful, we only want to call OSDProgramFree
		// if we're not stpDead. If we're marked stpDead, OSDProgramFree has
		// already been called by TerminateDebuggee. For now, this fix is only
		// for Java, but can (probably) be applied to all platforms.

		if (fRet &&
			(!pDebugCurr ||
			 (pDebugCurr && !pDebugCurr->IsJava()) ||
			(pDebugCurr && pDebugCurr->IsJava() && hpidCurr != hpidNull)))
		{
			ClearDebuggee(FALSE);
		}
		else if (hpidCurr != hpidNull)
		{
			VERIFY (OSDDestroyHpid (hpidCurr) == xosdNone);

			// NOTE: for attaching to an active process, this seems not to
			// be true: OSDEBUG4 uses dbcError event for case (1) above

//			fRet = TRUE;
		}

		lpprcCurr->hpid = hpidNull;
		hthdCurr = NULL;

		// Reinitialize the symbol handler.  This is also so the symbol handler
		// unlocks/releases any PDB or EXE which may have been loaded during
		// the initial load process.

		SHUnloadSymbolHandler (TRUE);


		gpISrc->StatusText (SYS_StatusClear, STATUS_INFOTEXT, FALSE);

		if (!fRet)
			MsgBox (Error, IDS_CouldNotExecuteProgram);

		SetCursor (hSaveCursor);

		return FALSE;
	}

	fRet = fRet && (xosd == xosdNone);

	// We should have received either dbcError or dbcLoadComplete.
	// If we got dbcLoadComplete, then our process status (stp) is
	// now stpStopped instead of stpNotStarted.  If it's still
	// stpNotStarted, then an error occurred and the process was
	// not actually loaded.

	ASSERT (lpprcCurr->stp == stpStopped || lpprcCurr->stp == stpNotStarted);

	// For Java, load additional classes after dbcLoadComplete.
	// We also want to load any classes referenced by breakpoints.
	if (pDebugCurr && pDebugCurr->IsJava()) {
		LoadAdditionalDLLs(NULL, TRUE);
		LoadBPReferencedClasses();
	}

	gpISrc->StatusText (SYS_StatusClear, STATUS_INFOTEXT, FALSE);

	SetCursor (hSaveCursor);

	if (!fCrashDump) {
		lpprcCurr->stp = stpNotStarted;
	}

	PST pst;
	VERIFY (OSDGetProcessStatus (hpidCurr, &pst) == xosdNone);
	pid = pst.dwProcessID;

	pidDebuggee = pid;		// HACK: global state variable

	//
	//	Set the debugger mode after loading

	lpprcCurr->pid = pid;
	set_addrs ();
	SYGetAddr (hpidCurr, htidCurr, adrData, &dump_start_address);
	SYGetAddr (hpidCurr, htidCurr, adrPC, &asmaddr);

	// Initialize OLE RPC stepping
	if (pDebugCurr->MtrcOleRpc ())
	{
		OSDSystemService (hpidCurr,
						  htidCurr,
						  ssvcOleRpc,
						  &runDebugParams.fOleRpc,
						  1,
						  NULL
						  );
	}

	if(runDebugParams.fFiberDebugging) {
		OFBRS ofbrs;
		ofbrs.op = OFBR_ENABLE_FBRS;
		OSDSystemService (hpidCurr,
					  htidCurr,
					  ssvcFiberDebug,
					  (LPVOID) &ofbrs,
					  sizeof(OFBRS),
					  NULL
					  );
	}

	if (!fRet)
		return fRet;

	//
	//	If we're doing just-in-time debugging, and there's no project
	//	loaded, load the EXE as a pseudo-project.  (Note, there may
	//	already be a project loaded, if the user launched the debuggee
	// 	with Project.Execute, and now we've been told to debug it).

	if (fDebugActive &&
		((gpIBldSys == NULL) || (gpIBldSys->IsActiveBuilderValid() != S_OK)))
		LoadJITProject ();

	InitProcessExceptions ();

	// Bind our list of breakpoints
	lpprcCurr->SetBPsBound (BHBindBPList ());

	fEnvirGbl.fAll = 0;
	fEnvirGbl.fs.user_pc = FALSE;
	fEnvirGbl.fs.ctxtIp = FALSE;
	//UpdateUserEnvir(mCtxtIp | mAsmaddr) ;


	//
	//	At this point we are sure the Load had succeeded. Also LoadDebuggee
	//	gets called for normal, JIT and Active process debugging. Therefore
	//	we do the state transition here.

	if (fRet)
    {
		if ( !IS_STATE_DEBUG(DkGetDockState()) )
		{
			DkSetDockState(STATE_DEBUG);
		}
		else
        {
			// We must be doing a restart since we were already in debug state.
			
 			if( pViewCpu )
 				pViewCpu->SendMessage( WU_INITDEBUGWIN, 0, 0 );

			if ( pViewMemory )
 				pViewMemory->SendMessage( WU_INITDEBUGWIN, 0, 0 );

 			if ( pViewCalls )
 				pViewCalls->SendMessage( WU_INITDEBUGWIN, 0, 0 );

 			if ( pViewDisassy )
 				pViewDisassy->SendMessage( WU_INITDEBUGWIN, 0, 0 );
		}
	}

	return fRet;
}

/***************************************************************************
 Spawn a non-debuggee process (Build.Execute)
 ***************************************************************************/

BOOL FSpawnOrphan (HPID hpid, LPCTSTR lszRemoteExe, LPCTSTR lszCmdLine, LPCTSTR lszRemoteDir, PID * lppid)
{
	BOOL fOK = FALSE;
	BOOL fFreeHpid = FALSE;
	SPAWNORPHAN so;

	// The passed-in HPID may be null, in which case we're supposed to
	// create a new HPID.

	if (hpid == hpidNull)
	{
		DEBUGDestroy ();
		if (!DEBUGInitialize ())
			return FALSE;

		ASSERT (hemCurr);
		ASSERT (htlCurr);

		if (SYUtilCreateHpid (CallBack, hemCurr, htlCurr, &hpid) != xosdNone)
		{
			return FALSE;
		}

		fFreeHpid = TRUE;
	}

	// Try to execute it now ...
	if (OSDSpawnOrphan (hpid, lszRemoteExe, lszCmdLine, lszRemoteDir, &so, ulfInheritHandles) != xosdNone)
	{
		MsgBox (Error, IDS_CouldNotExecuteProgram);
	}
	else if (so.rgchErr[0])
	{
		MsgBox (Error, so.rgchErr);
	}
	else
	{
		fOK = TRUE;
		if (lppid)
			*lppid = (PID) so.dwPid;
	}

	if (fFreeHpid)
		OSDDestroyHpid (hpid);

	return fOK;
} // FSpawnOrphan


BOOL
RestartDebuggee(
	LPTSTR	lszLocalExe,
	LPTSTR 	lszRemoteExe,
	LPTSTR 	lszDebuggeeArgs,
	BOOL	fCrashDump
	)
/*++

Routine Description:

	Restart or start depending on Gee status.

--*/
{
	BOOL	fRestart = FALSE;
	BOOL	fSuccess = FALSE;


	//
	// Free up the previous debugging session

	if (!DebuggeeAlive ())
	{
		pDebugCurr->SetQuickRestart (FALSE);
	}
	else
	{
		ASSERT (lpprcCurr);

		fRestart = TRUE;

		// Disable Quick Restart if Edit and Continue occurred

		if (pDebugCurr->IsEditedByENC())
			pDebugCurr->SetQuickRestart(FALSE);

		// if supported, go into Quick Restart mode
		
		else if (pDebugCurr->SupportsQuickRestart ())
			pDebugCurr->SetQuickRestart (TRUE);

		// We want it to be quiet on restart

		fSuccess = KillDebuggee (krRestart | krQuiet) &&
			!pDebugCurr->IsEditedByENC();
		
		if (!fSuccess)
		{
			pDebugCurr->SetQuickRestart (FALSE);
			return FALSE;
		}


	}

	// call DEBUGDestroy only in the case of not quick restart
	
	if (pDebugCurr->IsQuickRestart ())
	{
		fSuccess = DEBUGInitialize (TRUE);
	}
	else
	{
		DEBUGDestroy ();
		fSuccess = DEBUGInitialize ();
	}
	

	if (!fSuccess)
	{
		return FALSE;
	}

	
	pDebugCurr->SetQuickRestart (FALSE);

	// The use of global theApp is rather unfortunate here
		
	fSuccess = LoadDebuggee (lszLocalExe,
							 lszRemoteExe,
							 lszDebuggeeArgs,
							 theApp.m_jit.GetActive (),
							 theApp.m_jit.GetPid (),
							 theApp.m_jit.GetEvent (),
							 fRestart,
							 fCrashDump
							 );
								
	if (!fSuccess) {
		SetModeName ( (UINT) 0);
		return FALSE;
	}
	

	ASSERT (lpprcCurr);
			
	fEnvirGbl.fAll = FALSE;
	lpprcCurr->SetOmfLoaded (TRUE);

	return TRUE;
}


BOOL
TerminateDebuggee(
	)
/*++

Routine Description:

	This routine terminates the debugging session.  It can fail but still
	achieve the desired effect (of resetting our state to a clean state).
	This will happen, for example, when the transport layer is dead.  This
	function will fail because we cannot get communicate with the DM, but
	the net result will be success.

Return Value:

	Boolean.

--*/
{

	BOOL	fSuccess;

	//
	//	We should not call OSDProgramFree unless we succesfully loaded the
	//	debuggee.

	ASSERT (lpprcCurr->stp != stpNotStarted);

	if (hpidCurr != hpidNull)
	{
		fSuccess = OSDProgramFree (hpidCurr);
	}

	lpprcCurr->hpid = hpidNull;
	lpprcCurr->fLoaded = FALSE;

	hthdCurr = NULL;

	return fSuccess;
}


BOOL PASCAL
DebuggeeRunning(
	)
/*++

Routine Description:

	This routine returns true if the debuggee is in the run state (stpRunning)
	and	false otherwise (e.g. stpNotStarted, stpStopped).

--*/
{
	BOOL	fRunning;
	
	if (lpprcCurr && lpprcCurr->stp == stpRunning)
		fRunning = TRUE;
	else
		fRunning = FALSE;
		
	return fRunning;
}

BOOL PASCAL
DebuggeeNotStarted(
	)
/*++

Routine Description:

	This routine returns true if the debuggee is not started (stpNotStarted)
	and	false otherwise (e.g. stpRunning, stpStopped).

--*/
{
	return (lpprcCurr && lpprcCurr->stp == stpNotStarted);
}

BOOL PASCAL
FakeAsyncKillDebuggee(
	ULONG	Reason
	)
/*++

Routine Description:

	Kills the debuggee; doesn't really communicate with the remote side, so
	it's safe to use in error conditions, e.g., when the transport dies.

--*/
{
	CBP cbp;


	if (lpprcCurr && lpprcCurr->stp != stpDead) {
	
		lpprcCurr->SetDebuggeeStateEx (edsKilling);
		lpprcCurr->SetKillReason (Reason);

		OSDDisconnect (hpidCurr, htidCurr);

		cbp.wMessage = dbcProcTerm;
		cbp.hpid = hpidCurr;
		cbp.htid = htidCurr;
		cbp.dwParam = 0;
		cbp.lParam = 0;

		DoCallBack (&cbp);

		cbp.wMessage = dbcDeleteProc;

		DoCallBack (&cbp);

		UpdateDebuggerState (UPDATE_DEBUGGEE);
	}

	return TRUE;
}



BOOL PASCAL
AsyncKillDebuggee(
	ULONG	Reason
	)
/*++

Routine Description:

	Kills asynchronously the debuggee (while running).  By the time this
	routine returns, the debuggee is dead.
	
Comments:

	DebuggeeRunning() is not valid inside this scope.

--*/
{
	UINT	cTry = 0;
	XOSD	xosd = xosdNone;

	ASSERT (lpprcCurr);

	lpprcCurr->SetDebuggeeStateEx (edsKilling);
	lpprcCurr->SetKillReason (Reason);

	xosd = OSDProgramFree (hpidCurr);

	//
	// If we failed to kill the debuggee, don't wait around, just cleanup
	// right now.  This happens, for example, when the transport fails and
	// the user trys to stop debugging.
	
	if (xosd != xosdNone) {
		FakeAsyncKillDebuggee (Reason);
	}

	// Block until the debuggee is actually dead.  Since OSDTerminate
	// is asynchronous, we have to repeatedly call DoAllCallBacks until the
	// debuggee is actually dead.  If the process still isn't dead after
	// about 10 seconds, give up.  (NOTE: 10 seconds is a long time, but
	// unfortunately there's no good way to know for sure exactly when we
	// should give up.  NT's "End Task" dialog comes up after 5 seconds,
	// but that's not long enough a delay for us when running under our
	// test suites.)

	DoAllCallBacks ();
	while (WaitForSingleObject (evtOSDDone.m_h, 0) == WAIT_TIMEOUT &&
		   cTry++ < 100)		// 100 * 1/10th second = 10 seconds

	{
		MSG msg;

		// If we've already waited at least a tenth of a second,
		// put up an hourglass
		if (cTry == 2)
			AfxGetApp ()->DoWaitCursor (1);

		Sleep (1000 / 10);		// 1/10th second


		//
		// If a whole ton of OutputDebugStrings are coming back from
		// the debuggee, then our Output window code is going to
		// set a timer in order to have the main thread dequeue some
		// of the strings, and then it's going to block.  This
		// produces a deadlock situation.  To hack around this, while
		// we're waiting we dequeue all WM_TIMER messages.
		//

		while (PeekMessage (&msg, 0, WM_TIMER, WM_TIMER, PM_REMOVE)) {
		
			if (!theApp.PreTranslateMessage (&msg))
				DispatchMessage (&msg);
		}

		DoAllCallBacks ();
	}

	//
	// Remove hourglass, if any
	//
	
	if (cTry >= 2) {
		AfxGetApp ()->DoWaitCursor (-1);
	}


	//
	// For some reason failed to kill the debuggee.  Hammer the DM.
	//
	
	if (DebuggeeAlive ()) {
		FakeAsyncKillDebuggee (Reason);
		return TRUE;
	}

	UpdateDebuggerState (UPDATE_DEBUGGEE);
	return TRUE;
}

/***************************************************************************
 Implementation   : J.M. Dec 1992
 KillDebuggee     : CODEMGR.C

 Purpose  : Kills synchronously the debuggee ( when stopped )
 Arguments: Boolean to prevent a termination message box
 Returns  : TRUE if everything is OK...
 Note     : DebuggeeRunning() is not valid inside this scope
 ***************************************************************************/
BOOL PASCAL
KillDebuggee(
	ULONG	Reason
	)
{
	XOSD xosd = xosdNone;
	BOOL fAlreadyDying;

	if (!lpprcCurr || !hpidCurr)
	{
		return TRUE;
	}
	fAlreadyDying = (lpprcCurr->stp == stpDead);

	lpprcCurr->SetDebuggeeStateEx (edsKilling);
	lpprcCurr->SetKillReason (Reason);

	// If we want to stop debugging and the process is not
	// the current one, we'd better pretend this one is our
	// main debuggee

	pidDebuggee = lpprcCurr->pid;

	lpprcCurr->stp = stpRunning;	// This flag will be set on ProcTerm

	PrimeDbgMessagePump ();
	OSDProgramFree (hpidCurr);

	DbgMessagePump ();			// wait for dbcProcTerm

	if (!fAlreadyDying)
	{
		DoCallBacksUntil ();
	}

	UpdateDebuggerState (UPDATE_ALLSTATES);
	return (lpprcCurr->stp == stpDead);
}

/***************************************************************************
 Revision       : J.M. Dec 1992
 ClearDebuggee  : CODEMGR.C

 Purpose  : Calls the kill routine that matches the gee status
 Arguments: Boolean to prevent a termination message box
 Returns  : TRUE for success, FALSE for failure.  Failure is unlikely,
 but CAN occur!  If FALSE is returned, an error message has
 already been displayed to the user telling him we were
 unable to terminate the debuggee.
 ***************************************************************************/
BOOL PASCAL
ClearDebuggee(
	ULONG	Reason
	)
{
	// Reset 1st chance status
	// BUGBUG - shouldn't have to check this.
	if (lpprcCurr)
		lpprcCurr->SetFirstChance (FALSE);

	// We should never be calling ClearDebuggee from ExecDebuggee
	// since DebuggeeRunning would be always TRUE.
	// This routine is intended for a general kill purpose.

	// Java: when running the Java debugger, we're pumping messages during program load.
	// This leads to the case where the debuggee may not have completely started, but the
	// user is doing a kill debuggee. In this case, we need to do AsyncKillDebuggee since
	// we already have a DbgMessagePump pumping during the debuggee load.
	if (DebuggeeRunning () || (pDebugCurr && pDebugCurr->IsJava() && DebuggeeNotStarted()))
	{
		if (!(pDebugCurr->MtrcAsync () & asyncKill))
		{
			return FakeAsyncKillDebuggee (Reason);
		}
		else
		{
			return AsyncKillDebuggee (Reason);
		}
	}
	else
	{
		return KillDebuggee (Reason);
	}
}



/***************************************************************************
 TryingToAsyncStop

 Purpose  : Determine whether the debugger has already sent a dbcStop
 command to the DM and is waiting for it to finish.
 Arguments: None
 Returns  : TRUE if we're currently waiting for a dbcAsyncStop
 ***************************************************************************/
BOOL TryingToAsyncStop (VOID)
{

	ASSERT (lpprcCurr);

	if (lpprcCurr)
		return (lpprcCurr->GetDebuggeeStateEx () == edsAsyncStop);

	return FALSE;
}



/***************************************************************************
 AsyncStopDebuggee

 Purpose  : Stops the debuggee while it is running
 Arguments: None
 Returns  : None
 ***************************************************************************/
void
AsyncStopDebuggee(
	)
{
	XOSD xosd;
	if (!lpprcCurr || !DebuggeeRunning () ||
		(lpprcCurr && lpprcCurr->GetDebuggeeStateEx () == edsAsyncStop))	
	{
		MessageBeep (0);
	}
	else
	{
		ASSERT (lpprcCurr);

		lpprcCurr->SetDebuggeeStateEx (edsAsyncStop);
		
		xosd = OSDAsyncStop (hpidCurr, TRUE);
		if(xosd != xosdNone){
			MsgBox(Error, IDS_CANNOTBREAK);	
		}
	}
}

 /****************************************************************************
 
  FUNCTION:  IsDebugging()
 
  PURPOSE:   Exported version so packages can check if the debugger is active
 
  RETURNS:   TRUE if debugging
 
  ****************************************************************************/
 
 IDE_EXPORT BOOL
 IsDebugging(
 	)
 {
 	return DebuggeeAlive ();
 }
 
 

 // NOTE: this function is perhaps more accurately described as debuggee not
// dead (there are other states: stpNotStarted and stpGone, for example)

BOOL
DebuggeeAlive(
	)
{
	
	if (lpprcCurr && lpprcCurr->hpid)
	{
		return (lpprcCurr->stp != stpDead);
	}

	return FALSE;
}

BOOL PASCAL DebuggeeInCallBack (void)
{
	return TRUE;
}

/****************************************************************************

 FUNCTION:  MoveIPToCursor()

 PURPOSE:   Attempts to change the value of the [E]IP register to that
 of the current line

 RETURNS:   TRUE if successful

 ****************************************************************************/
BOOL PASCAL MoveIPToCursor (int iLine /*= -1*/)
{
	char szCurLine[20];
	DIS_RET disRet = DIS_NOERROR;
	HSF hsf = (HSF) NULL;
	ADDR addr, addrOld;
	BOOL fGotAddr = FALSE;

	if (DebuggeeAlive () && !DebuggeeRunning ())
	{
		// if ribbon edit box is active, get string from there & parse it
		if (IsFindComboActive ())
		{
			fGotAddr = FAddrFromRibbonEdit (&addr);
			if (fGotAddr)
			{
				// restore focus to whatever window had it before
				SetFocus (((CMDIFrameWnd *) theApp.m_pMainWnd)->MDIGetActive ()->
						  GetSafeHwnd ());
			}
		}
		else
		{
			CView   *pCurTextView = GetCurView();

			if (pCurTextView != NULL)
			{
                DOCTYPE dt;

                if ( gpISrc->ViewBelongsToSrc(pCurTextView) )
                    dt = DOC_WIN;
                else if ( pCurTextView->IsKindOf(RUNTIME_CLASS(CMultiEdit)) )
                    dt = ((CMultiEdit *)pCurTextView)->m_dt;
                else
                    dt = MEMORY_WIN;

				switch ( dt )
				{
				case DOC_WIN:
					// Make a current line BP node
					*szCurLine = BP_LINELEADER;
					if (iLine == -1) iLine = gpISrc->GetCurrentLine(pCurTextView) + 1;
					_itoa (iLine, szCurLine + 1, 10);
					fGotAddr = FAddrFromSz (szCurLine, &addr);
					break;

				case DISASSY_WIN:
				case CALLS_WIN:
					{
						GCAINFO gcai = {0};

						fGotAddr = (BOOL) pCurTextView->SendMessage (WU_GETCURSORADDR,0,(DWORD)&gcai);
						addr = gcai.addr;
						break;
					}
				}
			}
		}

		if (fGotAddr)
		{
			BOOL fCxtOld, fCxtNew;
			CXT cxtOld = {0}, cxtNew = {0};

			SYGetAddr (hpidCurr, htidCurr, adrPC, &addrOld);
			fCxtOld = (SHSetCxt (&addrOld, &cxtOld) != NULL);
			fCxtNew = (SHSetCxt (&addr, &cxtNew) != NULL);
			// If both of the SHSetCxt calls failed, then neither the old
			// location nor the new one has a context, so there's no way
			// we can warn the user about the danger of this operation.
			// However, if either SHSetCxt succeeded, then we can do
			// something useful.
			if (fCxtOld || fCxtNew)
			{
				// If we've gotten this far, then at least one of the
				// SHSetCxt calls succeeded.  If the other one failed,
				// or if the two contexts are different, issue a
				// warning about the danger of moving the PC out of
				// the current proc/mod.
				if ((!fCxtOld || !fCxtNew) ||
					cxtOld.hMod != cxtNew.hMod ||
					cxtOld.hProc != cxtNew.hProc
//                  Too agressive of a check (quite in your face)
				//                  || cxtOld.hBlk  != cxtNew.hBlk
					)
				{
					if (pDebugCurr && pDebugCurr->IsJava()) {
					    MsgBox (Error, IDS_CantMoveToNewProc, MB_OK);
						return TRUE;
					}
					else {
						if (MsgBox (Question, IDS_WillMoveToNewProc,
									MB_OKCANCEL) == IDCANCEL)
						{
							// return TRUE, not FALSE, because we don't want
							// caller to beep
							return TRUE;
						}
					}
				}
			}
			SYSetAddr (hpidCurr, htidCurr, adrPC, &addr);
			set_addrs ();
			UpdateDebuggerState (UPDATE_ALLDBGWIN | UPDATE_SOURCE);
			return TRUE;
		}
	}
	return FALSE;
}

/****************************************************************************

 FUNCTION:   ContinueToCursor()

 PURPOSE:    Attempts to do a GoUntil to the address that corresponds
 to the source line at the current cursor position in
 the editor

 RETURNS:    STEPCUR_NOERROR
 STEPCUR_NOCODE
 STEPCUR_NOMOVE
 STEPCUR_NOBPSET

 ****************************************************************************/
WORD PASCAL ContinueToCursor (BOOL fPassException, TCHAR * ptchGoto)
{
	ADDR addr;
	BOOL fGotAddr = FALSE;
	CString strName;
	LSZ lszName;
	ILINE wLine = (ILINE) - 1;

	// if ribbon edit box is active, get string from there & parse it
	if (ptchGoto)
	{
		fGotAddr = FAddrFromSz (ptchGoto, &addr);
		if (!fGotAddr)
		{
			return STEPCUR_NOBPSET;
		}
	}
	else if (IsFindComboActive ())
	{
		fGotAddr = FAddrFromRibbonEdit (&addr);
		if (!fGotAddr)
			return STEPCUR_NOBPSET;
	}
	else
	{
		CView   *pCurTextView = GetCurView();

		if (pCurTextView != NULL)
		{
            DOCTYPE dt;

            if ( gpISrc->ViewBelongsToSrc(pCurTextView) )
                dt = DOC_WIN;
            else if ( pCurTextView->IsKindOf(RUNTIME_CLASS(CMultiEdit)) )
                dt = ((CMultiEdit *)pCurTextView)->m_dt;
            else
                dt = MEMORY_WIN; // Just to get through the switch block.

			switch ( dt )
			{
			case DOC_WIN:
				fGotAddr = TRUE;
				wLine = gpISrc->GetCurrentLine(pCurTextView) + 1;
				strName = GetDebugPathName(pCurTextView->GetDocument());
				lszName = (LPTSTR) (LPCTSTR) strName;
				break;

			case CALLS_WIN:
			case DISASSY_WIN:
				{
					GCAINFO gcai = {0};

					fGotAddr = (BOOL) pCurTextView->SendMessage (WU_GETCURSORADDR,0,(DWORD) &gcai);
					addr = gcai.addr;
					break;
				}
			}
		}
	}
	
	if (fGotAddr)
	{
		// We can set a temporary breakpoint on the current line
		if (wLine != (ILINE) - 1)
		{
			CXT cxt = {0};

			if (get_initial_context (&cxt))
			{
				char szCurLine[20];

				// Make a current line BP node
				*szCurLine = BP_LINELEADER;
				_itoa (wLine, szCurLine + 1, 10);

				// Parse the address (must succeed)
				if (fGotAddr = FAddrFromSz (szCurLine, &addr))
				{
					// If the address matches the initial context's
					// address, then assume that the user started
					// debugging by going to the '{' for [Win]main
					// Dolphin: 5403
					if (FAddrsEq (addr, *SHpADDRFrompCXT (&cxt)))
					{
						return STEPCUR_NOERROR;
					}

					VERIFY (SYUnFixupAddr (&addr));
					if (!FSetUpTempBp (&addr, (HTHD) NULL, BPTMP))
					{
						return STEPCUR_NOBPSET;
					}
				}
				else
				{
					return STEPCUR_NOCODE;
				}
			}
			else if (!FSetUpTempSourceLineBp (lszName, wLine, (HTHD) NULL))
			{
				return STEPCUR_NOBPSET;
			}
		}
		else
		{
			VERIFY (SYUnFixupAddr (&addr));
			if (!FSetUpTempBp (&addr, (HTHD) NULL, BPTMP))
			{
				return STEPCUR_NOBPSET;
			}
		}

		Go ((HTHD) NULL, fPassException, TRUE);
		return STEPCUR_NOERROR;
	}

	return STEPCUR_NOCODE;
}

/****************************************************************************

 FUNCTION:   StepToReturn()

 PURPOSE:    Step out of current frame (use OSDebug)

 RETURNS:    STEPOUT_NOERROR        (Success)
 STEPOUT_NOCALLER   (Failure - nothing to get out of)
 STEPOUT_NOSOURCE   (Failure - OSDebug couldn't comply)

 ****************************************************************************/
WORD PASCAL StepToReturn (BOOL fPassException, int iStepMode)
{
	WORD wRet = STEPOUT_NOERROR;
	STO sto = (STO) ((fPassException ? stoPassEx : stoNone) |
					 (iStepMode == SRCSTEPPING ? stoQueryStep : stoNone));

	// Initialize the call stack chain
	CLGetWalkbackStack (hpidCurr, htidCurr, (UINT) NULL);

	// Any caller to this function ?
	if (CLChfmeOnStack () < 2)
	{
		wRet = STEPOUT_NOCALLER;
	}
	else
	{
		// Did OSDebug failed to perform the AfterReturn?
		if (!SYAfterReturn (sto, hpidCurr, htidCurr))
		{
			// Is this a valid return code?
			wRet = STEPOUT_NOSOURCE;
		}

		// If blocking, wait for the debug event to happen
		// everything went fine!
		else if (fBlocking)
		{
			nsWaitForDebugEvent ();
		}
	}

	return wRet;
}

/****************************************************************************
 CALLBACK handling routines
 ****************************************************************************/

void PASCAL InitCallBack (void)
{
	qCallBack = LLHlliInit (sizeof (CBP), llfNull, NULL, NULL);
	qCallBackReturns = LLHlliInit (sizeof (XOSD), llfNull, NULL, NULL);
}

/****************************************************************************
 LtszRemoteFileFromPath

 Given a remote path, returns a pointer to the filename portion.  E.g.
 if the remote path is "c:\foo\bar.exe", returns "bar.exe".  If the
 remote path is "My Macintosh disk:Subdirectory:Executable name",
 returns "Executable name".

 Note, the word "remote" in the name of this function does not imply
 that you can only call it when doing remote debugging.  In this case,
 "remote" just means "target-side", which may be the same machine.
 ****************************************************************************/

LPTSTR LtszRemoteFileFromPath (LPCTSTR ltszRemotePath)
{
	const TCHAR rgtchSepMac[] = ":";	// Mac path component separator

	const TCHAR rgtchSepDos[] = ":/\\";	// DOS (etc.) path component separators

	LPCTSTR ptchSep;
	LPCTSTR ptchFile;
	LPCTSTR ptch;

	// Figure out which list of path separators to use, depending on
	// the current target.
	//
	// Also, we only use the Macintosh separators if a project is open.
	// This is because when no project is open, any action such as
	// building or trying to run will automatically create a LOCAL
	// project.
	if (gpIBldSys && gpIBldSys->IsActiveBuilderValid() == S_OK &&
		pDebugCurr && pDebugCurr->IsMacintosh ())
	{
		ptchSep = rgtchSepMac;
	}
	else
	{
		ptchSep = rgtchSepDos;
	}

	// Find the filename component of the remote path which we're mapping.
	for (ptchFile = ptch = ltszRemotePath; *ptch; ptch = _tcsinc (ptch))
	{
		if (_ftcschr (ptchSep, (_TUCHAR) * ptch))
			ptchFile = _tcsinc (ptch);
	}

	return (LPTSTR) ptchFile;
}

/****************************************************************************
 MapRemoteFileToLocal

 Attempts to find a local module name given a remote module name.  If a
 local name cannot be found, returns FALSE so the main thread can prompt
 the user for a local name.

 For the case of local debugging, just copies the remote name to the
 local name.
 ****************************************************************************/

BOOL MapRemoteFileToLocal (LPCTSTR ltszRemote, CString & strLocal)
{
	LPCTSTR ptchModFile;
	LPCTSTR ptchExeFile;
	CPath pathLocal;

	ASSERT (pDebugCurr);

	// The symbol handler maintains its filenames in upper case.
	// We have to make the string upper case so that we can find
	// the corresponding remote filename later.

	// Local debugging - no mapping necessary
	if (!pDebugCurr->MtrcRemoteTL ())
	{
		strLocal = ltszRemote;
		return TRUE;
	}

	// look up the remote name in DLLInfo (if we have a project) and registry
	if (FGetLocalDLLFromRemoteDLL (strLocal, ltszRemote))
	{
		return TRUE;
	}

	// For JIT let's just not try to find anything for now.  The user probably
	// won't be able to locate the local file before all the modloads get
	// processed anyway.
	if (theApp.m_jit.GetActive ())
	{
		return FALSE;
	}

	// If we're debugging xbox, see if we can pluck the mapping out of the
	// xbe
	if(pDebugCurr->GetPlatform() == xbox && lpprcCurr) {
		char szPath[MAX_PATH + 1];

		HRESULT hr = HrXbeGetLocalModulePath(lpprcCurr->szName, ltszRemote,
			szPath, sizeof szPath);
		if(SUCCEEDED(hr)) {
			strLocal = szPath;
			return TRUE;
		}
	}

	ASSERT (gpIBldSys && gpIBldSys->IsActiveBuilderValid() == S_OK);

	CString strRemoteExe;

	gpIBldSys->GetRemoteTargetFileName(ACTIVE_BUILDER, strRemoteExe);

	// Find the filename component of the remote path which we're mapping,
	// and of the original remote exe name from our dialog.
	ptchModFile = LtszRemoteFileFromPath (ltszRemote);
	ptchExeFile = LtszRemoteFileFromPath (strRemoteExe);

	// If the remote name which we're mapping is the same as the exe
	// name which we sent over, then return the local name of the
	// exe.
	// Also check if we have an executable path
	if (_ftcsicmp (ptchModFile, ptchExeFile) == 0
		&& *ExecutablePath)
	{
		VERIFY (pathLocal.Create (ExecutablePath));
		strLocal = pathLocal;
		return TRUE;
	}

	// The remote name doesn't match the local name.  Look for
	// the remote name on our path (if it's a Macintosh name,
	// this is almost certain to fail, but oh well.)

	CDir dirProj;
    LPCSTR pszPath;

    if (gpIBldSys == NULL ||
		FAILED(gpIBldSys->GetBuilderFile(ACTIVE_BUILDER, &pszPath)) ||
        pszPath == NULL ||
		!dirProj.CreateFromPath(pszPath))
	{
		VERIFY (dirProj.CreateFromCurrent ());
	}

	if (pathLocal.CreateFromDirAndFilename (dirProj, ptchModFile)
		&& pathLocal.ExistsOnDisk ()
		&& CheckEXEForDebug (pathLocal, TRUE, TRUE))
	{
        strLocal = pathLocal;
        HBLDTARGET hTarget;

        gpIBldSys->GetActiveTarget(ACTIVE_BUILDER, &hTarget);
        UpdateDLLInfoRec (strLocal, ltszRemote, TRUE, (UINT)hTarget, TRUE);

        return( TRUE );
	}

	//
	//	We could check here if the file is on the users path, but we may find
	//	incorrect versions of things (consider remote debugging W95 on NT and
	//	vice versa: we will find the wrong versions of Kernel32, User32, &c.
	//
	
	// Couldn't find a matching local file.  Give up and prompt the user from
	// the main thread.
	return( FALSE );
}



BOOL
CheckTarget(
	LPCTSTR	LocalTarget,
	ULONG	TimeStamp,
	ULONG	CheckSum
	)
/*++

Routine Description:

	Check whether the given target has the specified TimeStamp and CheckSum
	pair.

	TimeStamp = -1 means don't care.
	CheckSum  = -1 means don't care.

Return Value:

	Return TRUE if the target file exists and is an image and if the
	TimeStamp and CheckSum match Target's TimeStamp and CheckSum.  Values of
	-1 for TimeStamp and CheckSum mean don't check this field.

--*/
{

	SHE		she;
	BOOL	fCheckCheckSum  = TRUE;
	BOOL	fCheckTimeStamp = TRUE;
	BOOL	fRet;
	ULONG	TargetTimeStamp;
	ULONG	TargetCheckSum;

	she = SHGetExeTimeStamp ( (LPSTR) LocalTarget, &TargetTimeStamp, &TargetCheckSum);

	if (she != sheNone)
		return FALSE;

	if (TimeStamp == (ULONG) -1)
		fCheckTimeStamp = FALSE;

	if (CheckSum == (ULONG) -1)
		fCheckCheckSum = FALSE;

	fRet = TRUE;
	
	if (fCheckTimeStamp && fRet)
		fRet = (TimeStamp ==  TargetTimeStamp);

	if (fCheckCheckSum && fRet)
		fRet = (CheckSum == TargetCheckSum);

	return fRet;
}



BOOL
CheckTargetInSync(
	LPCTSTR	LocalFile,
	ULONG	RemoteTimeStamp,
	ULONG	RemoteCheckSum
	)
/*++

Routine Description:

	For checking whether a target is in sync or not.

Comments:

	LocalFile must be a non-osdebug4 string.

--*/
{
	BOOL		fSuccess;
	CString*	strMessage = NULL;
	CString		strT;


	ASSERT (*LocalFile != '|');
	
	if (!RemoteDebugging ())
		return TRUE;
	
	fSuccess = CheckTarget (LocalFile, RemoteTimeStamp, RemoteCheckSum);

	if (!fSuccess)
	{
		strMessage = new CString;
		strT.LoadString (ERR_Proj_Target_Mismatch);

		strMessage->GetBuffer (1024);
		strMessage->Format (strT, LocalFile);
		strMessage->ReleaseBuffer ();

		PostMessage (hwndFrame, WU_MESSAGEBOX, Error, (LPARAM) strMessage);

	}

	return fSuccess;
}


void
SetDbgTimer(
	)
{
	QCEnterCritSection (&csDbgTimerSet);
	
	if (!fDbgTimerSet) {
		fDbgTimerSet = TRUE;
		VERIFY (SetTimer(hwndFrame, DBGCALLBACKTIMERID, 1, NULL));
	}
	
	QCLeaveCritSection (&csDbgTimerSet);
}



BOOL
UnMungeString(
	IN	CString*	strOD4Name,
	OUT CString*	strBaseName
	)
/*++

Routine Description:

	Extract basename from module load/unload strings that is given in the
	OSDebug4 format.  This function takes care of the DBCS case where a '|'
	could be a trail/lead byte.

Arguments:
	
	strOD4Name --  string in OSDebug4 format.
	
	strBaseName -- module basename string.

Return Value:

	Retuns TRUE is basename string is successfully generated.

--*/
{
	int nIndex = -1;

	if(strOD4Name->GetLength() <= 0)
		return FALSE;

	ASSERT(strOD4Name->Find(_T('|')) == 0);

	*strBaseName = strOD4Name->Mid(1);
	nIndex = strBaseName->Find(_T('|'));
	*strOD4Name = strOD4Name->Mid(nIndex + 1);
	*strBaseName = strBaseName->Mid(0, nIndex);
	return (strBaseName->GetLength() > 0)? TRUE: FALSE;

}


BOOL
UnMungeModuleName(
	IN	LPCTSTR		OD4Name,
	OUT	CString*	BaseName,
	OUT	ULONG*		TimeStamp,	OPTIONAL
	OUT	ULONG*		CheckSum,	OPTIONAL
	OUT	HANDLE*		FileHandle,	OPTIONAL
	OUT	ULONG*		BaseAddr	OPTIONAL
	)
/*++

Routine Description:
	
	Converts module load/unload strings from the munged OSDebug4 format to
	it's elements.

	The format of the OSDebug 4 buffer is like:

	"|"
    Path to file; if path is unknown, it's "#:\filename"
    "|"
    "0xNNNNNNNN", time and date stamp of exe
    "|"
    "0xNNNNNNNN", exe's checksum
    "|"
    "0xNNNNNNNN", handle to file (0xFFFFFFFF if DM is remote)
    "|"
    "0xNNNNNNNN", base load address of the exe/dll
    "|"

Return Value:

	True if the conversion was successful; False otherwise.

--*/
{

	ULONG	Time;
	ULONG	Check;
	ULONG	File;
	ULONG	Base;
	INT		ret;
	CString strOD4Name = OD4Name;

	if (*OD4Name != '|')
		return FALSE;

	// to take care of the DBCS case where we could have a '|' as a trail/lead byte
	// use UnMungeString()
	if(!UnMungeString(&strOD4Name, BaseName))
		return FALSE;

	ret = _stscanf ((const char*)strOD4Name,
					"|%x|%x|%x|%x|",
					&Time,
					&Check,
					&File,
					&Base);

	if (ret != 4)
		return FALSE;

	if (TimeStamp)
		*TimeStamp = Time;

	if (CheckSum)
		*CheckSum = Check;

	if (FileHandle)
		*FileHandle = (HANDLE) File;

	if (BaseAddr)
		*BaseAddr = Base;

	return TRUE;
}


BOOL
MungeModuleName(
	IN	LPCTSTR		BaseName,
	OUT	CString*	LocalName,
	IN	ULONG		TimeStamp,
	IN	ULONG		CheckSum,
	IN	HANDLE		hFile,
	IN	ULONG		BaseAddr
	)
{
	//
	// CString::Format apparently doesn't work on data longer than 128 bytes
	// unless you call GetBuffer with the size first.  Neat.
	//
	
	LocalName->GetBuffer (_MAX_PATH + 50);
	LocalName->Format ("|%s|0x%08lX|0x%08lX|0x%08lX|0x%08lX|", BaseName,
					    TimeStamp, CheckSum, hFile, BaseAddr);

	LocalName->ReleaseBuffer ();
	return TRUE;
	
}


XOSD
CallbackOnDbcModFree(
	IN	HPID	hpid,
	IN	HEXE	hexe,
	OUT	LPTSTR*	ModuleOut,
	OUT	BOOL*	MustWait
	)
/*++

Routine Description:

	This is where ModFree lands from Callback.

--*/
{
	LPCTSTR	Module;

	if (!hexe)
		return xosdNone;

	*MustWait = TRUE;
	
	//
	// SHGetExeName gives the local name
	
	Module = SHGetExeName (hexe);
	
	if (!Module || *Module == '|') {
		ASSERT (FALSE);
		return xosdNone;
	}

	*ModuleOut = new TCHAR [ _tcslen (Module) + 1];
	_ftcscpy (*ModuleOut, Module);

	SYInstallVirtual (hpid, *ModuleOut, FALSE);

	return xosdNone;
}


XOSD
CallbackOnDbcModLoad(
	IN	HPID	hpid,
	IN	LPCSTR	Module,
	OUT	LPTSTR*	ModuleOut,
	OUT BOOL*	MustWait,
	OUT	BOOL*	FoundLocal
	)
/*++

Routine Description:

	This is where dbcModLoad lands from Callback.

Comments:

	NOTE: this routine allocates memory for the new module's name.

--*/
{
	TCHAR*	pch;
	CString	BaseRemoteName;
	CString	FullLocalName;
	CString	BaseLocalName;
	BOOL	fMungedName = FALSE;
	HANDLE	hFile;
	ULONG	TimeStamp;
	ULONG	CheckSum;
	ULONG	BaseAddr;
	

	*MustWait = TRUE;

	ASSERT (Module);

	if (*Module == '|')
	{
		VERIFY (UnMungeModuleName (Module, &BaseRemoteName, &TimeStamp,
								   &CheckSum, &hFile, &BaseAddr));
		fMungedName = TRUE;
	}
	else
	{
		BaseRemoteName = Module;
	}
		
	if (BaseRemoteName [0] == '#' && BaseRemoteName [1] == ':' &&
		BaseRemoteName [2] == '\\')
	{
		LPCSTR pch = ((LPCSTR) BaseRemoteName) + 3;
		
		*FoundLocal = MapRemoteFileToLocal (pch, BaseLocalName);
		BaseRemoteName.ReleaseBuffer ();
	}
	else
	{
		*FoundLocal = MapRemoteFileToLocal (BaseRemoteName, BaseLocalName);
	}

#if 0
	//
	// Do not do this or NULL entries will force a prompt.  If a user has
	// hit cancel, there will be a null entry => we should not prompt.
	//
	
	if (*FoundLocal && BaseLocalName.IsEmpty ()) {
		*FoundLocal = FALSE;
	}
#endif
	
	//
	// if we can't find a local name, FindLocal is FALSE and set ModuleOut
	// to the un-altered file name (what was passed in).  The DoCallback
	// thread will prompt the user for the local file name.
	//

	if (!*FoundLocal || BaseLocalName.IsEmpty ())
	{
		*ModuleOut = new TCHAR [ _tcslen (Module) + 1];
		_ftcscpy (*ModuleOut, Module);
	}
	else
	{
		CheckTargetInSync (BaseLocalName, TimeStamp, -1);

		if (fMungedName)
		{
			VERIFY (MungeModuleName (BaseLocalName, &FullLocalName, TimeStamp,
								 CheckSum, hFile, BaseAddr));
		}
		else
		{
			FullLocalName = BaseLocalName;
		}

		*ModuleOut = new TCHAR [ FullLocalName.GetLength () + 1];
		_ftcscpy (*ModuleOut, FullLocalName);

		if (*FoundLocal)
		{
			SYInstallVirtual (hpid, (TCHAR*) Module, TRUE);
		}
	}

	return xosdNone;
}



//
// The lpaddr is the in parameter, the CanStep is the out parameter
//

struct CanStepPacket {
	union {
		LPADDR	lpAddr;
		CANSTEP	CanStep;
	} u;
};

		
void
GetCanStep(
	HPID		hpid,
	HTID		htid,
	ADDR		addr,
	CANSTEP*	CanStep
	)
{
	HSYM			hSym = NULL;
	CXT				Cxt = {0};
	UOFFSET			offset = 0;
	THUNK_ORDINAL	ordinal;
	ULONG			length = 0;
	CHAR			buffer [256];
	PROCSYM32*		proc = NULL;
	
	memset (CanStep, 0, sizeof (*CanStep));

	SYUnFixupAddr (&addr);
	SHSetCxt (&addr, &Cxt);
	offset = SHGetNearestHsym (&addr, Cxt.hMod, EECODE, &hSym);

	if (hSym) {

		SHGetSymName (hSym, buffer);

		proc = (PROCSYM32*) MHOmfLock (hSym);

		if (proc->rectyp == S_LPROC32 || proc->rectyp == S_GPROC32) {

			ADDR	addrLo = {0};
			ADDR	addrHi = {0};
		
			SYUnFixupAddr (&addr);

			AddrInit (&addrLo, NULL, proc->seg, proc->off, TRUE, TRUE, FALSE, FALSE);
			AddrInit (&addrHi, NULL, proc->seg, proc->off + proc->len, TRUE, TRUE, FALSE, FALSE);

			SYUnFixupAddr (&addrLo);
			SYUnFixupAddr (&addrHi);

			if (GetAddrSeg (addrLo) == GetAddrSeg (addr) &&
				GetAddrOff (addrLo) <= GetAddrOff (addr) &&
				GetAddrOff (addr) < GetAddrOff (addrHi)) {

				if (g_ExecutionExts.IsNoStepInto (buffer)) {
					CanStep->Flags = CANSTEP_NO;
					return;
				}
			}
		}
	}

		

	if (!SYFHasSource (hpid, &addr)) {

		CanStep->Flags = CANSTEP_NO;

		if (hSym) {

#if 0

//
// A temporary hack for delay-load thunks.
//

#define IMP_THUNK_SYM ("__imp_load_")
#define TAILMERGE_THUNK_SYM ("__tailMerge_")


			if (_ftcsncmp (IMP_THUNK_SYM, buffer, sizeof (IMP_THUNK_SYM) - 1) == 0) {

				//
				// The __imp_load thunk can be emulated over, because it makes
				// no calls.
				//

				CanStep->Flags = CANSTEP_THUNK;
/*MATHHACK*/	CanStep->ThunkSize = 1;

			} else if (_ftcsncmp (TAILMERGE_THUNK_SYM, buffer, sizeof (TAILMERGE_THUNK_SYM) - 1) == 00) {

/*MATHHACK*/
				CanStep->Flags = CANSTEP_THUNK_OVER;
				CanStep->ThunkSize = 14 - offset;

			} else

#endif
			if (SHIsThunk (hSym, &ordinal, &length)) {

				if (ordinal == THUNK_ORDINAL_LOAD) {

					CanStep->Flags = CANSTEP_THUNK_OVER;
					CanStep->ThunkSize = length - offset;

				} else {
					
					CanStep->Flags = CANSTEP_THUNK;
					CanStep->ThunkSize = length - offset;
				}
			}
		}

	} else {

		CanStep->Flags = CANSTEP_YES;

		offset = 0;

		if (SHIsInProlog (&Cxt)) {
			offset = SHGetDebugStart (Cxt.hProc);

			if (offset < GetAddrOff (Cxt.addr)) {

				//
				// Dont slide from epilog to prolog
				//
				
				offset = 0;

			} else {

				offset -= GetAddrOff (Cxt.addr);
			}
		}

		CanStep->PrologOffset = offset;
	}


	//
	// Step-into specific support.
	//
	
	if (CanStep->Flags == CANSTEP_YES && (lpprcCurr->sto & stoTraceFunction)) {

		ASSERT (g_pTraceFunction != NULL);

		SYUnFixupAddr (&addr);
		SetAddrOff (&addr, GetAddrOff (addr) + CanStep->PrologOffset);

		SHSetCxt (&addr, &Cxt);

		if (!g_pTraceFunction->CompareCxt (&Cxt)) {
			CanStep->Flags = CANSTEP_NO;
		}
	}
}
			
	

/****************************************************************************
 Revision      : J.M. Dec 1991
 CallBack   : ( SYSTEM.C )

 Purpose       : Handle all events from OSDebug
 Arguments     : wMsg     : message dbc
 hpid     : process ID for the message
 htid     : thread ID for the message
 wParam   : data about the message
 lParam   : data about the message
 Returns       : xosdNone

 To let the child process work correctly, we store information
 and let the Main window procedure take care of this asynchronously
 calling DoCallBack...
 ****************************************************************************/
XOSD PASCAL LOADDS
CallBack(
	DBC dbc,
	HPID hpid,
	HTID htid,
	LPARAM wParam,
	LPARAM lParam
	)
{
	LPCBP 	lpcbp;					// Callback parameters
	HLLE 	hlle;
	BOOL 	fMustWait = FALSE;		// must wait for DoCallBack to finish?
	BOOL	fLocalName = FALSE;		// whether we are dealing with a local name
	CString strLocal;

	static DWORD	tidMain = 0;	// id of main thread

	if (dbc == dbcRemoteQuit)
	{
		PostMessage(hwndFrame, WU_REMOTEQUIT, 0, 0L);
		return xosdNone;
	}

	// Should we set focus to the debuggee there ?
	if (dbc == dbcFlipScreen)
	{
		return (xosdNone);
	}

	if (dbc == dbcLastAddr)
	{
		//
		//  We will return:
		//
		//  If SRC mode and have src for line - Last addr in line
		//  If SRC mode and no src for line   - Zero
		//  if ASM mode                       - Same addr
		//
		
		LPADDR	lpaddr = (LPADDR) lParam;
		WORD	wLn;
		SHOFF	cbLn;
		SHOFF	dbLn;
		HPRC	hprc = HprcFromHpid (hpid);
		LPPRC	lpprc;
		int		StepMode = SRCSTEPPING;

		lpprc = (LPPRC) LLLpvFromHlle (hprc);
		
		if (lpprc)
			StepMode = lpprc->GetStepMode ();

		if (StepMode == SRCSTEPPING)
		{
			if (SLLineFromAddr (lpaddr, &wLn, &cbLn, &dbLn))
			{

				ASSERT (cbLn >= dbLn);
				if (cbLn >= dbLn)
				{
					lpaddr->addr.off += cbLn - dbLn;
				}
			}
			else
			{
				memset (lpaddr, 0, sizeof (ADDR));
			}
		}
		
		UnlockHlle (hprc);
		return (xosdNone);
	}
	
	if (dbc == dbcCheckWatchPoint || dbc == dbcCheckBpt)
	{
		BOOL fReply;
		ADDR addrT;

		SYGetAddr (hpid, htid, adrPC, &addrT);
		fReply = BPCheckBpt (&addrT, NULL, htid);

		return ((XOSD) fReply);
	}



	if (dbc == dbcCheckMsgBpt) {
	
		LPMSGI lpmsgi = (LPMSGI) lParam;

		return (XOSD) BPCheckBpt (&lpmsgi->addr, lpmsgi, htid);
	}

	
	if (dbc == dbcCanStep) {
		GetCanStep (hpid, htid, *(LPADDR)lParam, (CANSTEP*) lParam);
		return xosdNone;
	}

	/*
	** If we are not to stop on a thread term return TRUE.  Mark the
	** thread as dead.
	*/
	
	if (dbc == dbcThreadTerm)
	{
		HPRC hprc = HprcFromHpid (hpid);
		HTHD hthd;
		LPTHD lpthd;

		hthd = HthdFromHtid (hprc, htid);
		ASSERT (hthd);
		lpthd = (LPTHD) LLLpvFromHlle (hthd);
		lpthd->tdf = tdfDead;

		if (1) // !fQuietKill)
		{
			CString strThreadTerm;
			OutputWindowQueueHit (OwinGetDebugCommandID (),
								  MsgText (strThreadTerm, DBG_Thread_Term,
										   lpthd->tid, lParam, lParam),
								  FALSE,
								  TRUE);
		}

		UnlockHlle (hthd);

		ASSERT (wParam == 0);
		if (FContinueThreadTerm (hpid, htid))
		{
			if (mpdbcdbct[dbcThreadTerm] == dbctStop)
			{
                // REVIEW: HACK!
                // V5 - Bug #22302.  Java can't deal with stopping thread terms yet
                if (pDebugCurr && pDebugCurr->IsJava()) {
				    return (XOSD) TRUE;
                }
				wParam = TRUE;
			}
			else
			{
				return (XOSD) TRUE;
			}
		}
	}

	if (dbc == dbcExitedFunction)
	{
		BOOL GetSymbolFromAddr (PADDR pAddr, CString & strSymbolName);

		LPADDR lpaddr = (LPADDR) lParam;
		ADDR addr = *lpaddr;
		CXT cxt = {0};
		HPROC hProc;
		USHORT iStr = 0;
		HTM hTMProc;
		EEHSTR hszName;

#if 0
#if defined(_MIPS_) || defined(_M_ALPHA) || defined(_M_PPC)
#if defined(_M_PPC)
#pragma message(__FILE__ "REVIEW v-ibmms: merge ppcnt; need code review")
#endif
		REGISTERS regs;
		UOFFSET target = 0;
		if (ADDR_IS_LI(addr)) {
			SYFixupAddr(&addr);
		}
		int ThunkLimit = 8;
		while (ThunkTarget(hpid, htid, &addr, target, regs) && --ThunkLimit) {
			SetAddrOff(&addr, target);
			target = 0;
		}
#endif
#endif
		if (SYUnFixupAddr (&addr) &&
			SHSetCxt (&addr, &cxt) != NULL &&
			(hProc = SHHPROCFrompCXT (&cxt)) != NULL &&
			EEGetTMFromHSYM (hProc, &cxt, &hTMProc, &iStr, TRUE, FALSE) == EENOERROR &&
            EEGetNameFromTM (&hTMProc, &hszName) == EENOERROR)
		{
			LPCSTR szName = (LPCSTR) BMLock (hszName);
			CXF cxfCur (::CxfWatch ());
			HTM hTM;
			EEHSTR hszValue;

			ASSERT (szName != NULL);

			iStr = 0;

			// Parse, bind and evaluate the expression to cause the value to
			// change.
			if (EEParse ("$ReturnValue", ::radix, FALSE, &hTM, &iStr) == EENOERROR &&
				EEBindTM (&hTM, &cxt, TRUE, FALSE) == EENOERROR &&
				EEvaluateTM (&hTM, SHpFrameFrompCXF (&cxfCur), EEHORIZONTAL) == EENOERROR &&
				EEGetValueFromTM (&hTM, radix, NULL, &hszValue) == EENOERROR)
			{
				EEHSTR hszType = NULL;
				LPCSTR szType = _T ("");
				LPCSTR szValue;

				szValue = (LPCSTR) BMLock (hszValue);
				ASSERT (szValue != NULL);

				if (EEGetTypeFromTM(&hTM, NULL, &hszType, 0) == EENOERROR)
				{
					szType = (LPCSTR) BMLock (hszType);
					ASSERT (szType != NULL);
					szType += sizeof (HDR_TYPE);
				}

				g_retVals.AddTM (szName, szValue, szType);

				if (hszType != NULL)
				{
					BMUnlock (hszType);
					EEFreeStr (hszType);
				}

				BMUnlock (hszValue);
				EEFreeStr (hszValue);
			}

			BMUnlock (hszName);
			EEFreeStr (hszName);
		}

		return xosdNone;
	}

	// Add a new element to our callback parameters list
	hlle = LLHlleCreate (qCallBack);
	if (hlle == 0)				// out of memory?

		goto exit;				// yes, punt
	// Create pointer from element

	lpcbp = (LPCBP) LLLpvFromHlle (hlle);
	// Copy over the information
	lpcbp->wMessage = dbc;
	lpcbp->hpid = hpid;
	lpcbp->htid = htid;
	lpcbp->dwParam = wParam;
	lpcbp->lParam = lParam;

	// Store info if necessary !
	switch (lpcbp->wMessage)
	{
	case dbcException:
		{
			// Currently lParam == LPEPR, a ptr to a structure; but
			// we're going to repackage this information into:
			//      dwParam = exception number
			//      lParam  = EXSTUFF*
			LPEPR lpepr = (LPEPR) lParam;
			BOOL fStopping = TRUE;
			HTID hvtid = lpcbp->htid;
			DWORD cFrame;
			EXSTUFF *pexstuff = (EXSTUFF *) new BYTE[sizeof (EXSTUFF)];

			if (!pexstuff)		// no memory available?

				return xosdNone;	// bail

			memset (pexstuff, 0, sizeof (EXSTUFF));

			lpcbp->dwParam = lpepr->ExceptionCode;
			lpcbp->lParam = (LPARAM) pexstuff;

			if (lpepr->dwFirstChance)
			{
				pexstuff->fFirstChance = TRUE;

				// since it's a first-chance exception, DM may be
				// continuing execution, depending on what the
				// user said in the Debug.Exceptions dialog
				if (GetExceptionAction (lpepr->ExceptionCode) ==
					EX_ACTION_2NDCHANCE)
				{
					fStopping = FALSE;
				}
			}

			if (!fStopping)
			{
				pexstuff->fContinuing = TRUE;
			}


#if 0
			//	THIS CODE IS BROKEN.  In the DM if this was an ignore or
			//	a first-chance we want to ignore, we will already be continued
			//	in the DM.  Therefore, a stack dump doesn't really make sense,
			//	and, ends is killing us.
			
			
			// If the exception is the special one which indicates a
			// throw(), we want to look two elements up the stack.
			// This is because the stack looks like this:
			//      RaiseException
			//      C runtime
			//      User's code
#if defined (OSDEBUG4)			// Don't unwind 9 times
			cFrame = 1;
#else
			cFrame = 3;
#endif
			if (lpcbp->dwParam == (0xE0000000 | 'msc')
				&&
				OSDGetFrame (lpcbp->hpid, hvtid, cFrame, &hvtid) == xosdNone
				&&
				OSDGetFrame (lpcbp->hpid, hvtid, cFrame, &hvtid) == xosdNone
				&&
				OSDGetFrame (lpcbp->hpid, hvtid, cFrame, &hvtid) == xosdNone)
			{
				// nothing
			}
#endif

			// this will get either the current PC, or the PC for
			// the third element on the stack, as appropriate
			OSDGetAddr (lpcbp->hpid, hvtid, adrPC, &pexstuff->addrPC);
		}
		break;

	case dbcMsgBpt:
		// need to copy the contents of the lParam Buffer to our own buffer
		lpcbp->lParam = (LONG) new BYTE[wParam];

		_fmemcpy ((char FAR *) lpcbp->lParam, (char FAR *) lParam, wParam);

		break;

	case dbcInfoAvail:
		// Process it just right now ... Why ?
		// it could be any information related to a KERNEL
		// Fatal error and should be displayed in the error window !
		// ProcessInfoAvail destroy the element in our list ...
		UnlockHlle (hlle);
		ProcessInfoAvail (hlle);
		BMFree (hlle);
		return (xosdNone);

	case dbcInfoReq:
		// WE HAVE TO PROCESS the dbcInfoReq at this point !
		// if not, the debuggee will restart without any answer
		// to WINDOWS kernel request...
		UnlockHlle (hlle);

		ProcessInfoRequest (hlle);

		BMFree (hlle);
		return (xosdNone);

	case dbcCreateThread:
		break;

	case dbcModFree:

		CallbackOnDbcModFree (hpid,
							  (HEXE) lParam,
							  (LPTSTR*) &lpcbp->lParam,
							  &fMustWait
							  );
		fMustWait = TRUE;
		OSDContinue (hpid, htid);
		break;

	case dbcModLoad:

		CallbackOnDbcModLoad (hpid,
							  (LPCTSTR) lParam,
							  (LPTSTR*) &lpcbp->lParam,
							  &fMustWait,
							  (BOOL*) &lpcbp->dwParam
							  );

		OSDContinue (hpid, htid);
		break;
		
	case dbcError:
		lpcbp->lParam = (LONG) new BYTE[_ftcslen ((const char *) lParam) + 1];

		_ftcscpy ((char *) lpcbp->lParam, (const char *) lParam);
		// AuxPrintf2 ("--[ %s ]--", (LPSTR) lParam);
		break;

	default:
		break;
	}

	// Code in the switch statement above may have modified the
	// dwParam and lParam in the CBP structure; we want to copy
	// these changes into the passed-in wParam and lParam arguments
	// in case the code below accesses those variables.
	
	wParam = lpcbp->dwParam;
	lParam = lpcbp->lParam;


	UnlockHlle (hlle);
	LLAddHlleToLl (qCallBack, hlle);
	evtQueueEmpty.Reset ();

	// Get ID of main thread, if we haven't gotten it yet
	if (tidMain == 0)
		tidMain = GetWindowThreadProcessId (GetShellWindow(), NULL);

	// [cuda#3652 5/4/93 mikemo]  This thread (the debugging thread)
	// may be enqueueing messages into qCallBack faster than the
	// main thread is capable of dequeueing them.  This is bad news,
	// so if the queue grows too big, this thread will just sleep
	// for a while until the queue gets smaller.
	//
	// Also, some messages must be processed by DoCallBack before
	// we return control to OSDebug, so wait if fMustWait is set.
	//
	// Finally, don't wait if CallBack was called from the main thread
	// (the owner of hwndFrame).

	if ((fMustWait || LLChlleInLl (qCallBack) > 10) &&
		tidMain != GetCurrentThreadId ())
	{
		// [cuda#5252 6/25/93 mikemo]
		// OSDProgramLoad is synchronous: the main thread is completely
		// blocked until the load is done.  In that case, we must empty
		// the queue ourselves rather than waiting for the main thread
		// to do so.
		//
		// 'stp' will be stpNotStarted during OSDProgramLoad, but
		// stpRunning during any other OSDebug call.
		if (lpprcCurr->stp == stpRunning)	// are we in OSDProgramLoad?

		{						// no
			// Set the 'hurry' event so that the main thread will immediately
			// break out of its initial half-second delay

			evtOSDHurry.Set ();

			if (fMustWait)
			{
				// If fMustWait is set, this is some message for which we
				// must wait until DoCallBack has processed it.  Calling
				// SendMessage() directly, rather than waiting for the
				// system to send a WM_TIMER event, will force the message
				// to be processed as quickly as possible.

				if (pDebugCurr && pDebugCurr->IsJava())
				{
					// for Java, we don't want to block waiting for the
					// main thread here, because there is the potential for deadlock:
					// - the main thread is handling some command (say stop debugging)
					// - the DM thread comes up here on a class load event
					//   (and takes the critical section in EMCallBackDB) and
					//   blocks doing this SendMessage
					// - the main thread then attempts to enter EMCallBackDB and
					//   blocks on the critical section
					// Result: deadlock
					
					PostMessage(hwndFrame, WM_TIMER, DBGCALLBACKTIMERID, 0);
				}
				else
				{
					// Force immediate flushing of the queue by the main thread
					SendMessage (hwndFrame, WM_TIMER, DBGCALLBACKTIMERID, 0);

					// Make sure the SendMessage did the trick
					VERIFY (WaitForSingleObject (evtQueueEmpty.m_h, 0) == 0);
				}
			}
			else
			{
				// fMustWait is not set, so, in order to prevent the IDE
				// from bogging down, we're going to patiently wait until
				// the system decides to send a WM_TIMER to the main
				// thread.  That way the IDE's UI won't be blocked much
				// if a debuggee is causing lots of messages to be sent.

				SetDbgTimer ();

				// billjoy - don't do this.  We need to be able to prompt user for
				// a local name.  By waiting until the queue is empty, the TL
				// will timeout

				//WaitForSingleObject(evtQueueEmpty.m_h, INFINITE);
			}
		}
		else					// we're in OSDProgramLoad

		{
			// We're in OSDProgramLoad, so the main thread is blocked.
			// We'll tell the main thread to process messages by
			// signalling evtOSDHurry.

			evtOSDHurry.Set ();

			// billjoy - don't do this.  We need to be able to prompt user for
			// a local name.  By waiting until the queue is empty, the TL
			// will timeout

			// Wait until main thread has processed messages
			//WaitForSingleObject(evtQueueEmpty.m_h, INFINITE);
		}
	}
	else
	{
#if defined (OSDEBUG4)

		// fMustWait is not set for dbcCreateThread under OSDEBUG4
		// debug tid must be running to allow parent to call OSDThreadStatus
		// w/o deadlock but we still might be waiting for dbcLoadComplete
		// so let the pump trickle; otherwise set the timer

		if (dbc == dbcCreateThread || dbc == dbcThreadTerm)
		{
			if (lpprcCurr->stp == stpRunning)
			{
				SetDbgTimer ();
			}
			else
			{
				evtOSDHurry.Set ();
			}
		}
		else
#endif
		{
			SetDbgTimer ();
		}
	}

  exit:

	// If this dbc has caused the debuggee to stop running, then it's
	// time to set the evtOSDDone.

	ASSERT (dbc < CELEM_ARRAY (mpdbcdbct));

	switch (mpdbcdbct[dbc])
	{
	case dbctStop:
#if defined (OSDEBUG4 )     // HACK!!!
		if ((dbc == dbcCreateThread) ||
			((dbc == dbcThreadTerm) && (wParam == TRUE)))
		{
			break;
		}
#endif
		if (dbc == dbcEntryPoint)	// HACK!!!
			break;

		if (dbc == dbcProcTerm)		// Yet another HACK
			break;
		
        if (dbc == dbcMsgBpt)
            break;

		ExitDbgMessagePump ();
		break;

	case dbctContinue:
		break;

	case dbctMaybeContinue:
		switch (dbc)
		{
		case dbcException:
			if (!((EXSTUFF *) lParam)->fContinuing)
				ExitDbgMessagePump ();

			break;

#if !defined (OSDEBUG4)	// mpdbcdbct[dbcThreadTerm] == dbctStop!
		case dbcThreadTerm:
			// In the case where we're going to continue running
			// after getting a dbcThreadTerm, we've already
			// exited from this function up above, so we can
			// unconditionally call ExitDbgMessagePump() here.
			if (mpdbcdbct[dbc] == dbctStop)
				break;

			ExitDbgMessagePump ();
			break;
#endif

		default:
			// currently the only dbctMaybeContinue dbc's are
			// dbcException and dbcThreadTerm
			ASSERT (FALSE);
		}
		break;

	default:
		ASSERT (FALSE);
	}

#if !defined (OSDEBUG4)	// mpdbcdbct[dbcThreadTerm] == dbctStop!
	/* dbcThreadTerm is only enqueued if we are going to stop.  We need to
	** return a boolean indicating whether the DM should continue running
	** rather than stop.
	**
	** [cuda#4811 5/25/93 mikemo]  If the thread that just died was one that
	** we're stepping, then tell the DM to stop.
	*/
	if (dbc == dbcThreadTerm && mpdbcdbct[dbc] == dbctMaybeContinue)
		return (XOSD) FALSE;
#endif

    if (pDebugCurr && pDebugCurr->IsJava()) {
        // REVIEW: HACK!
        // V5 - Bug #22302.  Java can't deal with stopping thread terms yet
    	if (dbc == dbcThreadTerm)
    		return (XOSD) FALSE;
    }

	return (xosdNone);
}

XOSD PASCAL DoAllCallBacks (void)
{
	XOSD xosd = xosdNone;

	do
	{
		xosd = CheckCallBack (TRUE);
	}
	while (xosd == xosdNone || xosd == xosdContinue);

	return xosd == xosdQueueEmpty ? xosdNone : xosd;
}

/************************************************************************
 HthdLive()

 Return any live thread of the process.
 ************************************************************************/
HTHD HthdLive (HPRC hprc)
{
	HTHD hthd;
	LPTHD lpthd;
	HLLI llthd = LlthdFromHprc (hprc);

	hthd = 0;
	while ((hthd = LLHlleFindNext (llthd, hthd)) != 0)
	{
		lpthd = (LPTHD) LLLpvFromHlle (hthd);
		if (!(lpthd->tdf & tdfDead))
			break;
		UnlockHlle (hthd);
	}
	UnlockHlle (hthd);
	return hthd;
}

void SetInfoFromJITExe (LPCSTR szJITExe)
{
	// save new exe name
	LPCSTR lsz = szJITExe;
	LSZ lszT;

	if (lsz[0] == '|')
	{
		lsz++;
		if (lsz[0] == '#' && lsz[1] == ':' && lsz[2] == '\\')
		{
			lsz += 3;
		}
		_ftcsncpy (ExecutablePath, lsz, sizeof (ExecutablePath));
		ExecutablePath[sizeof (ExecutablePath) - 1] = '\0';
		lszT = _tcschr (ExecutablePath, _T ('|'));
		if (lszT)
		{
			*lszT = '\0';
		}
	}
	else
	{							// into global too

		_ftcsncpy (ExecutablePath, szJITExe, sizeof (ExecutablePath));
		ExecutablePath[sizeof (ExecutablePath) - 1] = '\0';
	}
	VERIFY (theApp.m_jit.FSetPath (ExecutablePath, TRUE));

	// Reset c:\path\UNKNOWN.EXE to the real name
	if (lpprcCurr->szName)
	{
		free (lpprcCurr->szName);
	}
	lpprcCurr->szName = _ftcsdup (ExecutablePath);

	// set current dir to that of the exe
	CPath path;
	CDir dir;

	path.Create ((LPCTSTR) ExecutablePath);
	if (dir.CreateFromPath (path))
		(void) dir.MakeCurrent ();	// ignore return code

}

static void SYStopAndSet (HPRC hprc)
{
	SYStop (hprc);
	ExitDbgMessagePump ();
}


BOOL
IsRemote(
	)
{
	ASSERT (pDebugCurr);
	return pDebugCurr->MtrcRemoteTL ();
}

BOOL
IsJit(
	)
{
	return (theApp.m_jit.GetActive () && !theApp.m_jit.FPathIsReal ());
}


HRESULT
PromptForLocalFile(
	LPCTSTR		szRemoteFile,
	CString*	pstrLocalFile,
	BOOL		*bPrompt
	)
/*++

Routine Description:

	When remote debugging and a remote module is loaded, prompt for a local
	module.

	If a local file was not found, pstrLocalFile will be emtpy.

Return Values:

	S_OK if the local file was found; S_FALSE if not.

--*/
{
	int					ret;
	HRESULT				hr;
	HBLDTARGET			hTarget = NULL;
	CMapRemoteDllDlg	dlg (szRemoteFile);

	ASSERT (IsRemote ());

	fInFindLocalDlg = TRUE;		// ??

	ret = dlg.DoModal ();

	if (ret == IDOK && !dlg.m_strLocal.IsEmpty ()) {

		ASSERT (!IsJit ());
		ASSERT (gpIBldSys);

		if (!dlg.m_strLocal.IsEmpty ()) {

			gpIBldSys->GetActiveTarget (ACTIVE_BUILDER, &hTarget);
			AddRecToDLLInfo (dlg.m_strLocal,
							szRemoteFile,
							TRUE,
							(ULONG) hTarget);

			hr = S_OK;
			*pstrLocalFile = dlg.m_strLocal;
		} else {
			hr = S_FALSE;
		}

		if (gpIBldSys && gpIBldSys->IsActiveBuilderValid () == S_OK) {
			gpIBldSys->SetPromptForDlls (ACTIVE_BUILDER, dlg.m_fPrompt);
		}
	} else {
		hr = S_FALSE;
		pstrLocalFile->Empty ();
	}
		
	*bPrompt = dlg.m_fPrompt;
	fInFindLocalDlg = FALSE;

	return hr;
}
	

	
	
VOID
OnDoCallback_ModLoad(
	HPID	hpid,
	LPCSTR	szRemoteFile,
	bool	bFoundLocal
	)
/*++

Routine Description:

	Callback on the UI thread when a MOD is loaded to prompt for the local
	MOD.

Arguments:

	lpszFile - The remote file name.

--*/
{
	CString	strBaseRemoteName;
	CString	strLocal;
	DWORD	dwTimeStamp;
	DWORD	dwCheckSum;
	HANDLE	hFile;
	DWORD	dwBaseAddr;
	BOOL	succ;
	HRESULT	hr;

	if (IsRemote ()) {

		if (bFoundLocal)
			return;

		if (*szRemoteFile == '|') {
			succ = UnMungeModuleName (szRemoteFile,
									  &strBaseRemoteName,
									  &dwTimeStamp,
									  &dwCheckSum,
									  &hFile,
									  &dwBaseAddr);
			ASSERT (succ);
		}


		ASSERT (!(strBaseRemoteName [0] == '#' &&
				  strBaseRemoteName [1] == ':' &&
				  strBaseRemoteName [2] == '\\'));

		if (gpIBldSys && (gpIBldSys->GetPromptForDlls(ACTIVE_BUILDER) == S_OK) )
		{
			BOOL bPrompt;
			hr = PromptForLocalFile (strBaseRemoteName, &strLocal, &bPrompt);
			if (!bPrompt)
				gpIBldSys->SetPromptForDlls(ACTIVE_BUILDER, FALSE);
		}
		else
			hr = E_FAIL;

		if (hr == S_OK) {

			CheckTargetInSync (strLocal, dwTimeStamp, -1);
			SYInstallVirtual (hpid, (LSZ) szRemoteFile, TRUE);
		}

	} else {

		if (IsJit ()) {
			SetInfoFromJITExe (szRemoteFile);
		}
	}
}


				


/************************************************************************
 We really process the callbacks in this function
 ************************************************************************/
XOSD PASCAL DoCallBack (LPCBP lpcbp)
{
	CBlockRecursion block;	// this will cause us not to handle
							// callbacks recursively!

	XOSD xosd = xosdNone;
	static HPID hpidNew = 0;
	static HTID htidNew = 0;
	static HPRC hprcParent = 0;
	static BOOL fReallyNew;
	CBP cbp = *lpcbp;
	BPI *pbpi = 0;

	if (hpidNew == 0 &&
		cbp.wMessage != dbcDeleteProc &&
		cbp.wMessage != dbcDeleteThread)
	{
		HPRC hprc = HprcFromHpid (cbp.hpid);

		if (hprc != NULL)
		{
			HPRC hthd = HthdFromHtid (hprc, cbp.htid);

			SYSetContext (hprc, hthd, FALSE);
		}
	}

	ASSERT (cbp.wMessage < CELEM_ARRAY (mpdbcdbct));
	if (mpdbcdbct[cbp.wMessage] == dbctContinue)
		xosd = xosdContinue;

	switch (cbp.wMessage)
	{
	case dbcServiceDone:
		ASSERT (mpdbcdbct[cbp.wMessage] == dbctStop);
		break;

		// A new module has been loaded
	case dbcModLoad:

		{
			//
			// If dwParam == 0 then we did not find this file locally; else
			// we did find it locally.
			//
			
			OnDoCallback_ModLoad (cbp.hpid, (LSZ) cbp.lParam, cbp.dwParam ? true : false);
		
			// Notify the Edit & Continue File Set about the new dll
			HEXE hexe = SHGethExeFromName( (LSZ) cbp.lParam );

			if (hexe) {
				ENCAddDll(hexe);
			}

			lpprcCurr->fLoaded = TRUE;

			if (hpidNew != 0) {
				ProgLoad (hprcParent,
						  hpidNew,
						  htidNew,
						  (LSZ) cbp.lParam,
						  fReallyNew);
					  
				hpidNew = 0;
				htidNew = 0;
			}
		}
		
		break;

	case dbcError:
		{
			TST tst;
			HTHD hthd = NULL;
			LPTHD lpthd;
			BOOL fDead;
			HLLI llthd;
			CString str;

			// AuxPrintf2 ("%Fs", (LPV) cbp.lParam);
			xosd = (XOSD) cbp.dwParam;
			// If the process is not yet started, then don't call
			// SYStop.  This is sort of a hack -- calling
			// SYStop would assert, and it's not really
			// necessary here.

			// VC5.0 Bug #20708 - Needed to add the Java Check below.  There is a window of opportunity where
			// the stp == stpRunning and DoCallBack can be called prior to load complete.  When a dialog is up
			// we can call DoCallback prior to returning from DbgMessagePump.  We end up setting the stp to stpStopped
			// in this case inside of SYStopAndSet.  We DO NOT want to do this since it will cause LoadDebuggee to not
			// recognize the load failure ( Search for 20708 above ).  It is likely that other platforms need this fix
			// as well but since I can't repro on another platform the fixed will be confined to Java - JimSa
			if (lpprcCurr && lpprcCurr->stp != stpNotStarted && !pDebugCurr->IsJava() )
			{
				SYStopAndSet (HprcFromHpid (cbp.hpid));

				// If the process hasn't started we don't have threads
				// yet so we should not be querying them.

				// Determine if a thread is in first-chance state
				llthd = LlthdFromHprc (HprcFromHpid (cbp.hpid));
				while ((hthd = LLHlleFindNext (llthd, hthd)) != NULL)
				{
					lpthd = (LPTHD) LLLpvFromHlle (hthd);
					fDead = !!(lpthd->tdf & tdfDead);
					UnlockHlle (hthd);

					if (!fDead)
					{
						VERIFY (OSDGetThreadStatus (cbp.hpid,
									HtidFromHthd (hthd), &tst) == xosdNone);
						if ((tst.dwState & tstExceptionMask) == tstExcept1st)
						{
							lpprcCurr->SetFirstChance (TRUE);
							break;
						}
					}
				}
			}
			// Message box is safe here...
			DebuggerMsgBox (
							   Error,
						  MsgText (str, SYS_Debug_Error, (LPSTR) cbp.lParam)
				);
		}
		break;

		// A breakpoint on a Windows message was hit
	case dbcMsgBpt:

		if (!BPMsgBpt (TRUE, (LPMSGI) lpcbp->lParam))
		{
			EXOP	exop = {0};

			exop.fSingleThread = TRUE;
			OSDGo (cbp.hpid, cbp.htid, &exop);
			xosd = xosdContinue;
		}
		else 
		{
			SYStopAndSet(HprcFromHpid (lpcbp->hpid));
		}
		
		ASSERT (mpdbcdbct[cbp.wMessage] == dbctStop);
		break;

		// A breakpoint was hit
	case dbcWatchPoint:
	case dbcBpt:
		{
			ADDR addrT;
			int bpt;

			SYGetAddr (hpidCurr, htidCurr, adrPC, &addrT);
			bpt = BPBrkExec (&addrT, htidCurr, &pbpi);

			BreakStatus (bpt);
			SYStopAndSet (HprcFromHpid (lpcbp->hpid));

			if (bpt == BPHARDCODEDBP) {

                UpdateDebuggerState (UPDATE_ALLSTATES);
                SYFixupAddr (&addrT);

                if ( !CheckAssertionFailed ( &addrT ) ) {
                    char    arch[ CCHMAXPREFIXTOSZ + 11 ];
                    char	rgchT[ CCHMAXPREFIXTOSZ + 11 ];

                    LoadString (
                        hInst,
                        IDS_HARDCODEDBP,
                        (LPSTR) arch,
                        sizeof( arch )
                    );

                    _stprintf (
                        rgchT,
                        _T("%s 0x%lx "),
                        arch,
                        (LONG)GetAddrOff ( addrT ) );

                    CVMessage ( INFOMSG, (MSGID) rgchT, CMDWINDOW );
                    DispPrompt();
                }
			}

			ASSERT (mpdbcdbct[cbp.wMessage] == dbctStop);
		}
		
		BREAKPOINTNODE *pbpnT;
		if (pAutoDebugger && pbpi && (pbpnT = BHFirstBPNode()))
		{
			while(pbpnT && (LPBPI)LLLpvFromHlle(pbpnBPhbpi(pbpnT)) != pbpi)
				pbpnT = pbpnNext(pbpnT);

			if (pbpnT)
			{
				CAutoBreakpoint *pBP = pbpnT->pAutoBP;
				IDispatch *pDispatch;
				bool bHadBP = FALSE;

				if (!pBP) pBP = new CAutoBreakpoint(pbpnT);
				else bHadBP = TRUE;
				
				ASSERT(pBP);
				// If we already had a breakpoint, we force a new reference
				// for the parameter.  In either case, we decrement the refcount
				// when we are done with it.
				pDispatch = pBP->GetIDispatch(bHadBP);
				pAutoDebugger->FireBreakpointHit(pBP);
				pDispatch->Release();
			}
		}
		break;

	case dbcAsyncStop:
		SYStopAndSet (HprcFromHpid (cbp.hpid));
		ASSERT (mpdbcdbct[cbp.wMessage] == dbctStop);
		break;

	case dbcSignal:
		ASSERT (FALSE);
#if 0
		abortflag = TRUE;
		VERIFY (OSDAsyncStop (hpidCurr, TRUE) == xosdNone);
		SYStopAndSet (HprcFromHpid (cbp.hpid));
		ASSERT (mpdbcdbct[cbp.wMessage] == dbctStop);
#endif
		break;

		// a single or range step finished !
	case dbcStep:
		SYStopAndSet (HprcFromHpid (cbp.hpid));
		lpprcCurr->fHitBP = FALSE;

		ASSERT (mpdbcdbct[cbp.wMessage] == dbctStop);
		break;

	case dbcCreateThread:

		if (hpidNew == 0) {
			AddThread (cbp.hpid, cbp.htid);
		} else {
			htidNew = cbp.htid;
		}
			
		if (mpdbcdbct[cbp.wMessage] == dbctStop) {

			EXOP exop = {0};

			exop.fSingleThread = TRUE;
			OSDGo (cbp.hpid, cbp.htid, &exop);
			xosd = xosdContinue;
		} else {
			ASSERT (mpdbcdbct[cbp.wMessage] == dbctContinue);
		}
		break;

	case dbcThreadTerm:
		{
			HPRC hprc = HprcFromHpid (cbp.hpid);
			HTHD hthd;
			LPTHD lpthd;
			CString str;

#if defined (OSDEBUG4)
			// OSDebug 3 will send dbcProcTerm on EXIT_PROCESS
			// OSDebug 4 will send dbcThreadTerm and wait for GO to send dbcDeleteThread
			// dbcProcTerm dbcDeleteProc
			if (cbp.dwParam == FALSE)
			{
				PST pst;

				if (OSDGetProcessStatus (cbp.hpid, &pst) == xosdNone)
				{
					cbp.dwParam = (pst.dwProcessState == pstDead);
				}
			}
#endif
			if ((mpdbcdbct[cbp.wMessage] == dbctStop) && (cbp.dwParam == TRUE))
			{
				EXOP exop = {0};

				exop.fSingleThread = TRUE;
				OSDGo (cbp.hpid, cbp.htid, &exop);
				xosd = xosdContinue;
				break;
			}
			lpthd = (LPTHD) LLLpvFromHlle (hthd = HthdFromHtid (hprc, cbp.htid));
			UnlockHlle (hthd);

			// Make sure some live thread is in hthdCurr
			if (hthdCurr == hthd)
			{
				SYSetContext (hprc, HthdLive (hprc), FALSE);
			}

			SYStopAndSet (HprcFromHpid (cbp.hpid));
			DebuggerMsgBox (Information, MsgText (
													 str,
													 DBG_Thread_Term,
													 lpthd->tid,
													 cbp.lParam,
													 cbp.lParam
							));
		}
		break;


	case dbcDeleteThread:
		{
			HPRC	hprc = NULL;
			HLLI	llthd = NULL;

			OSDDestroyHtid (cbp.hpid, cbp.htid);
			hprc = HprcFromHpid (cbp.hpid);

			if (hprc == NULL) {
				break;			// Hack until 8662 failure better understood
			}
			
			llthd = LlthdFromHprc (hprc);

			LLFDeleteHlleFromLl (llthd, HthdFromHtid (hprc, cbp.htid));
			hthdCurr = LLHlleFindNext (llthd, (HTHD) NULL);
			ASSERT (mpdbcdbct[cbp.wMessage] == dbctContinue);
		}
		break;

	case dbcNewProc:
		hpidNew = (HPID) cbp.dwParam;
		fReallyNew = (BOOL) cbp.lParam;
		//fReallyNew = TRUE ;
		//AuxPrintf2("- NEW PROC - %d -",cbp.lParam) ;
		//AuxPrintf2("  parent : 0x%04X",cbp.hpid) ;
		//AuxPrintf2("  child  : 0x%04X",cbp.wParam) ;
		hprcParent = HprcFromHpid (cbp.hpid);
		ASSERT (mpdbcdbct[cbp.wMessage] == dbctContinue);
		break;

	case dbcProcTerm:
		{
			HPRC hprc = HprcFromHpid (cbp.hpid);

			if (hprc == NULL)
			{
				break;
			}

			LPPRC lpprc = (LPPRC) LLLpvFromHlle (hprc);

			// [CAVIAR #3794 11/04/92 v-natjm]
			// We want to stop debugging even if this process
			// is not the one we started first. Let's pretend
			// in this case it was the first one.
			if (lpprc->pid == pidDebuggee)
			{
				lpprc->fReallyNew = TRUE;
			}
			if (lpprc->fReallyNew == FALSE)
			{
				// When we do a restart ?
				lpprc->stp = stpDead;
				lpprc->exp = expStopped;
				// We don't want to continue if there is any event
				// pending... This probably means we are
				// processing this notification asynchronously !!!
				// Typical situation is Stepping over an MFC app
				// object ->run() method. This is going to return
				// control to WINDOWS through the message loop
				if (!EventLeft ())
				{
					HPRC hprcOrg = (HPRC) NULL;

					SYContinue (hprcCurr);
					// We don't want to sit on this context if it's
					// a dying process, switch back to our debuggee
					// Every call to the BH has to be made with
					// a valid Hprc
					hprcOrg = HprcFromHpid (hpidCurr);
					SYSetContext (hprcOrg, 0, FALSE);
				}
				xosd = xosdContinue;
			}
			else
			{
				if (lpprc->stp != stpDead)
				{
					STP stpT = lpprc->stp;


                    CString strFileName;
                    BOOL bGotExeName = FALSE;
                    LPTSTR lpstr = strFileName.GetBuffer(_MAX_PATH);
                    if (GetDebugTarget(lpstr, _MAX_PATH))
                    {
                        bGotExeName = TRUE;
                    }
                    strFileName.ReleaseBuffer( );

					lpprc->stp = stpDead;
					lpprc->exp = expStopped;
					if ((lpprc->hprcParent == 0) ||
						(lpprc->pid == pidDebuggee))
					{
						SYStop (hprcCurr);

						if (lpprc->GetDebuggeeStateEx () == edsKilling &&
							lpprc->GetKillReason () & krRestart)
						{
							// if we are doing a quick restart.

							ASSERT (pDebugCurr);
							
							if (pDebugCurr->IsQuickRestart ())
							{
								CheckBPsAtRestart ();
							}
						}
						else
						{
							//
							//	This is not a restart, so reset normal toolbars
							//	and	palettes
							//
							
							DkSetDockState (STATE_EDIT);	
						}

						// With remote JIT, we may not have a project
						if (gpIBldSys && gpIBldSys->IsActiveBuilderValid() == S_OK) 						// was fQuietKill &&
						{
                            CString str;

							if (pDebugCurr && pDebugCurr->IsJava()) {
								gpIBldSys->GetJavaClassName(ACTIVE_BUILDER, strFileName);
							}
                            else if (!bGotExeName || strFileName.IsEmpty())
                            {
                                gpIBldSys->GetRemoteTargetFileName(ACTIVE_BUILDER, strFileName);
                                if (strFileName.IsEmpty())
                                {
                                    LPTSTR pszPath = NULL;
                                    gpIBldSys->GetTargetFileName(ACTIVE_BUILDER, &pszPath);
                                    if (pszPath)
                                    {
                                        strFileName = pszPath;
                                        delete [] pszPath;
                                    }
                                }
                            }

							// we're not sure which one we want, complete
							// path and exe name or just the exe name
							// ptchExeFile = LtszRemoteFileFromPath(strRemoteExe);

							// Put "process terminated" message in output wnd
							MsgText (str, SYS_Proc_Term, (LPCSTR) strFileName, cbp.lParam, cbp.lParam);

							OutputWindowQueueHit (OwinGetDebugCommandID (), str, FALSE, TRUE);
						}

						// Exe is locked by the Symbol handler
						// and will cause us some problems...

						ASSERT (lpprcCurr);
 						if (lpprcCurr->IsOmfLoaded ())
						{
							SYUnFixupOmf ();

							// this is prob unnecessary since we are about
							// to destroy the lpprc
							lpprcCurr->SetOmfLoaded (FALSE);
						}

						// If we were doing JIT without a project, clear the DLL info
						// (since there will be one grid and its hTarget will be NO_TARGET
                        if (gpIBldSys == NULL || (gpIBldSys->IsActiveBuilderValid() != S_OK))
							ClearDLLInfo();

						// Remove all DLL mappings marked for deletion
						PurgeDLLInfo ();

						// [CAVIAR #5878 11/27/92 v-natjm]
						// Reset all the BP nodes on termination
						BFHResetShowAllBPNode ();
						// For any breakpoints which are in DLLs,
						// add those DLLs to the Additional DLLs
						// line so that when the program restarts
						// we can find the breakpoints
						BHRememberBPDLLs ();
						// BUG #1997 02/19/93 v-natjm
						// We'd better "Unbind" from CV BPHandler
						// On proc termination. This ensures that
						// the code is not going to be found in
						// multiple modules...
						BHUnBindBPList ();
						// just-in-time debugging is now off
						theApp.m_jit.SetActive (FALSE);

						{
							extern void ClearMultiEdit (void);
							ClearMultiEdit ();
						}

						// This is the case for when a remote monitor is exited
						// and attempts to kill all debuggees.  If the debuggee
						// is stopped, then we aren't really expecting this so
						// we need to clear out the debuggee here.

						if (lpprcCurr->GetDebuggeeStateEx () == edsKilling &&
							stpT == stpStopped)
						{
							UpdateDebuggerState (UPDATE_DEBUGGEE);
						}

					}
					else
					{
						SYSetContext (lpprc->hprcParent, 0, TRUE);
					}
				}
			}
			UnlockHlle (hprc);

		}
		break;

	case dbcDeleteProc:

		//	lpprcCurr can be NULL here when we are processing a Restart ()

		if (lpprcCurr && cbp.hpid == lpprcCurr->hpid)
		{
			OSDDestroyHpid (cbp.hpid);
			lpprcCurr->hpid = hpidNull;
		}

		ExitDbgMessagePump ();
		break;

	case dbcException:
		// NOTE: in the CallBack function, before putting the
		// dbcException into the queue, it changed its wParam
		// and lParam in order to avoid having to allocate
		// memory for a copy of the structure which got passed
		// from OSDebug, since we only need a couple of small
		// pieces of information from that structure.
		//
		// Now, we have:
		//      wParam = exception number
		//      lParam = EXSTUFF*
		{
			CString strExceptDesc;
			CString strExeDll;
			LPCTSTR lszExe, lszDll;
			CPath pathExe, pathDll;
			CString strChance;
			EXSTUFF *pexstuff = (EXSTUFF *) cbp.lParam;

			// The exception description may have been changed by
			// the user in the Exceptions dialogbox, so, get this one
			GetExceptionDescription (lpcbp->dwParam, strExceptDesc);
			if (strExceptDesc.IsEmpty ())
				VERIFY (strExceptDesc.LoadString (DBG_Exception_NoName));

			// Get name of EXE and name of module (EXE/DLL) in which exception
			// occurred
			VERIFY (pathExe.Create (ExecutablePath));
			lszExe = pathExe.GetFileName ();

			if ((lszDll = SHGetExeName ((HEXE) emiAddr (pexstuff->addrPC)))
				!= NULL)
			{
				VERIFY (pathDll.Create (lszDll));
				lszDll = pathDll.GetFileName ();
			}
			// if exception occurred in the EXE itself or in no known module,
			// display "FOO.EXE"; otherwise, display "FOO.EXE (BAR.DLL)"
			if (lszDll != NULL && _tcsicmp (lszExe, lszDll) != 0)
			{
				lszExe = MsgText (strExeDll, DBG_Exception_ExeDll, lszExe,
								  lszDll);
			}

			// Is OSDebug going to keep running the debuggee, or stop?
			if (pexstuff->fContinuing)
			{
				// We're continuing

				CString strExc;

				// Send a line to the output window
				ASSERT (pexstuff->fFirstChance);
				MsgText (strExc, DBG_Exception_Msg,
						 MsgText (strChance, DBG_First_Chance), lszExe,
						 lpcbp->dwParam, (LPCSTR) strExceptDesc);
				OutputWindowQueueHit (OwinGetDebugCommandID (), strExc,
									  FALSE, TRUE);

//Speed:3364 removed this code [apennell]
#ifdef OSDEBUG4_NOT_ANY_MORE
				// Continue executing the debuggee.
				EXOP exop = {0};

				exop.fSingleThread = FALSE;
				exop.fPassException = TRUE;
				OSDGo (cbp.hpid, cbp.htid, &exop);
#endif
				xosd = xosdContinue;
			}
			else
			{
				// We're stopping

				BOOL fHasFirstChanceExceptions;
				UINT ids;		// string id

				CString str;

				SYStopAndSet (HprcFromHpid (cbp.hpid));
				lpprcCurr->SetFirstChance (pexstuff->fFirstChance);
				VERIFY (OSDGetDebugMetric (cbp.hpid, cbp.htid,
										   mtrcExceptionHandling, &fHasFirstChanceExceptions) == xosdNone);

				if (fHasFirstChanceExceptions)
				{
					ids = lpprcCurr->IsFirstChance ()
						? DBG_First_Chance : DBG_Last_Chance;
				}
				else
				{
					ids = DBG_Exception;
				}

				// Display a message there !
				DebuggerMsgBox (Information, MsgText (
				   str, DBG_Exception_Msg, MsgText (strChance, ids), lszExe,
								   lpcbp->dwParam, (LPCSTR) strExceptDesc));
			}
		}
		break;

	case dbcModFree:
		// Breakpoints should become virtual at this point
		// until module reloaded...
		ASSERT (mpdbcdbct[cbp.wMessage] == dbctContinue);
		break;

// #if defined(TARGMAC68K)
#pragma message("OSDEBUG4: dbcMacCodeLoad removed! FUTURE")
#if !(defined (OSDEBUG4))
	case dbcMacCodeLoad:		

		ASSERT (mpdbcdbct[cbp.wMessage] == dbctContinue);
		break;
#endif
// #endif

	case dbcLoadComplete:
		// Java: since we pump messages during the load, the process may actually be
		// dead by the time we get load complete. If this is the case, we want
		// the process to remain dead.
		if (!pDebugCurr || (pDebugCurr && !pDebugCurr->IsJava()) || (pDebugCurr && pDebugCurr->IsJava() && lpprcCurr->stp != stpDead))
		{
            // lpprcCurr->stp is set to stpRunning before we enter the message pump (to properly handle
            // shutting down the IDE while loading the debuggee).
			ASSERT (lpprcCurr->stp == stpNotStarted || lpprcCurr->stp == stpRunning);
			lpprcCurr->stp = stpStopped;
		}

		// For Java, we can get dbcLoadComplete, after the process is dead (marked stpDead).
		// We don't want to make the OSDNativeOnly call, in this case.
		if  ( !lpprcCurr->fPcodePresent && lpprcCurr->stp != stpDead ) {
			OSDNativeOnly ( cbp.hpid, cbp.htid, !lpprcCurr->fPcodePresent );
		}

		//
		// If we are JIT debugging or attached to a running process, we are
		// already past the entry point.
		//

		if (theApp.m_jit.GetActive() || IsCrashDump ()) {
		
			lpprcCurr->SetBeyondEntryPoint (TRUE);
		}

		ASSERT (mpdbcdbct[cbp.wMessage] == dbctStop);
		break;

	case dbcEntryPoint:
		{
			ADDR addrT;
			int bpt;

			ASSERT (mpdbcdbct[cbp.wMessage] == dbctStop);
			ASSERT (lpprcCurr);

			// If the user has set a bp at the entry point, we don't get a separate
			// notification. Therefore we need to check bps here to see if we need to
			// stop.

			SYGetAddr(hpidCurr, htidCurr, adrPC, &addrT);
			bpt = BPBrkExec(&addrT, htidCurr, &pbpi);

			lpprcCurr->SetBeyondEntryPoint (TRUE);
			
			// We've reached the entry point of the debuggee.  If we have a
			// breakpoint at WinMain, then we'll just go again; otherwise,
			// we'll stop now.

			// HACK: The hard-coded bp check here is due to a hack in
			// BPBrkExec that needs to be cleaned up.
			if (lpprcCurr->fBpInitialCxt && (bpt == BPCONTINUE || bpt == BPHARDCODEDBP))
			{
				EXOP exop = {0};

				exop.fSingleThread = TRUE;
				OSDGo (cbp.hpid, cbp.htid, &exop);
				xosd = xosdContinue;
			}
			else
			{
				SYStopAndSet (HprcFromHpid (cbp.hpid));
			}
			break;
		}
	}

	// Has the debuggee stopped running?
	if (xosd != xosdContinue)
	{
		HPRC hprc;
		LPPRC lpprc;

		// If the debuggee has stopped running, record what dbc made it stop.
		// NOTE, this may not actually be the dbc which makes us stop, so for
		// a short period of time lpprc->dbcLastStop may have an incorrect
		// value, but as long as this function gets called again when the
		// actual stopping dbc DOES arrive, everything will be okay.

		hprc = HprcFromHpid (cbp.hpid);
		if (hprc != NULL)
		{
			lpprc = (LPPRC) LLLpvFromHlle (hprc);

			// The process may have been completely wiped out

			if (lpprc)
			{

	//?!? NOTE: this seems like a strange place to reset this flag  -- ???

				if (lpprc->GetDebuggeeStateEx () == edsAsyncStop)
					lpprc->SetDebuggeeStateEx (edsNone);


				lpprc->dbcLastStop = (DBC) (cbp.wMessage);
			}

			UnlockHlle (hprc);
		}
	}

	return (xosd);
}

void ActivateRemoteDebugger (void)
{
	HCURSOR hSaveCursor = SetCursor (LoadCursor (NULL, IDC_WAIT));

	evtOSDDone.Reset ();

	ASSERT (pDebugCurr && pDebugCurr->MtrcNativeDebugger ());
	
	if (OSDSystemService (hpidCurr, htidCurr, ssvcNativeDebugger, NULL, 0, NULL)
		== xosdNone)
	{
		while (WaitForSingleObject (evtOSDDone.m_h, 0) == WAIT_TIMEOUT)
		{
			DoAllCallBacks ();
		}
		evtOSDDone.Reset ();
	}
	SetCursor (hSaveCursor);
}

// this is an external interface for packages to get to the debugger

IDE_EXPORT XOSD DebuggerSystemService (
    HPID        hpid,
    HTID        htid,
    SSVC        ssvc,
    LPV            lpvData,
    DWORD        cbData,
    LPDWORD        lpcbReturned )
{
	if (hpid==NULL)
		hpid = hpidCurr;
	XOSD xosd =	OSDSystemService( hpid, htid, ssvc, lpvData, cbData, lpcbReturned );
	if (xosdNone==xosd)
	{
#ifndef OSDEBUG4
		DbgMessagePump();
#endif
	}
	return xosd;
}

// this is an external interface for debugger components to get to theApp.FindInterface

IDE_EXPORT HRESULT DbgFindInterface ( REFIID refid, LPVOID * lpv )
{
	return theApp.FindInterface( refid, lpv );
}



ULONG
THashedMapOf <CString, DWORD>::Hash(
	const CString&	str
	)
{
	return HashPjw (str);
}

int
THashedMapOf <CString, DWORD>::Compare(
	const CString&	str1, const CString& str2
	)
{
	return str1.Compare (str2);
}


#ifndef _tsizeof
#define _tsizeof(_x) (sizeof (_x) / sizeof (TCHAR))
#endif
	
	
VOID
WINAPIV
DebugPrint(
    LPTSTR szFormat,
    ...
    )
{
    va_list		marker;
    int			n;
	TCHAR		rgchDebug [1024];

    va_start (marker, szFormat);
    n = _vsnprintf (rgchDebug, _tsizeof (rgchDebug), szFormat, marker );
    va_end (marker);

    if (n == -1) {
        rgchDebug [_tsizeof (rgchDebug) - 1] = 0;
    }

    OutputDebugString (rgchDebug);
}
