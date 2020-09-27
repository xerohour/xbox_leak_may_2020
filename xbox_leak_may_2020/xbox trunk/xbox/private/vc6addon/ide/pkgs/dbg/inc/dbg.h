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
#ifndef __DBG_H__
#define __DBG_H__

/*
#include "ansistub.h"
#include "util.h"
#include "makeeng.h"
#include "mk_make.h"
#include "outwin.h"
#include "excep.h"
#include "main.h"
#include "sushutil.h"

#include "..\..\include\project.h"

#include "tlui.h"

// Special low-level functions
#ifndef _WIN32
#include "toolhelp.h"
#endif

#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <malloc.h>


#include "cvexefmt.h"

#include "cmgrhigh.h"
#include "brkpt.h"
#include "breakpts.h"
#include "cmgrlow.h"
#include "askpath.h"
#include "outwin.h"

// Prototypes for SYSTEM.C Functions
#include "sy.hmd"

#include "codemgr.h"
#include "extern.h"

// NEW EXE header information
#include "newexe.h"

//prototypes for CL.C functions
#define MENU void
#define MENUITEM void
#include "cl.h"
*/

// Properties
#define P_VarExpr                   128
#define P_VarType                   129
#define P_VarValue                  130

void PASCAL ResetCV400Globals(void);
void PASCAL UpdateUserEnvir(unsigned short fReq);

// 4th caviar merge : 2 following constants
#define GRANUL                  0x1000  // 4K block size as a default...
#define MAX_BLOCK               0x0008  // 8 blocks of 4K memory

typedef void (LOADDS PASCAL *QUIT)(UINT);
typedef HDEP (LPFNSYM MMALLOCMEM) (UINT);
typedef VOID (LPFNSYM LPFNFREEHUGE) (LPV);

void ProgLoad(HPRC,HPID,HTID,LSZ,BOOL) ;

#define cmpHtid   0
#define cmpTid    1
#define cmpLpthd  2

#define cmpHpid   0
#define cmpPid    1
#define cmpTitle  2
#define cmpName   3

#define cbMaxTitle (8)

#define SF_NOERROR      0
#define SF_NOFILE       1
#define SF_NODOCAVAIL   2
#define SF_DONTOPENFILE 3

#define SHpADDRFrompCXF(pCXF)  SHpADDRFrompCXT(SHpCXTFrompCXF(pCXF))

// Prototypes
VOID PASCAL UnInitOSD(void) ;
BOOL PASCAL InitOSDDlls(void) ;
BOOL PASCAL DllLoad(int DLLtype) ;
XOSD PASCAL DoCallBack (LPCBP lpcbp);
void PASCAL StopChild(void) ;
void PASCAL InitCallBack(void);

DIS_RET PASCAL GetHsfFromDoc(CDocument *,HSF FAR *) ;
int PASCAL SearchFileFromModule(LPSTR, CDocument **, HSF, BOOL *, UINT);
BOOL PASCAL LoadFileFromModule(LPSTR,LPCSTR,CDocument **,HSF,BOOL *,UINT) ;
void TextDocCallback(CDocument *, UINT);

void PASCAL RetrySearchFile();	// Forces search file to forget cache information.
BOOL GetCurrentSource(LPSTR,WORD,ILINE *,SHOFF *,HSF FAR *) ;
void nsWaitForDebugEvent(void) ;
LOCAL int PASCAL near StartExec(void) ;
BOOL FSetUpTempBp(PADDR paddr,HTHD hthd,int BpType) ;
BOOL FSetUpTempSourceLineBp(LSZ, ILINE, HTHD);
void GoUntil(PADDR paddr, HTHD hthd, int BpType, BOOL fGoException,
	BOOL fFlipScreen) ;
BOOL PASCAL KillDebuggee(ULONG Reason);
BOOL PASCAL FakeAsyncKillDebuggee(ULONG Reason);
BOOL PASCAL AsyncKillDebuggee(ULONG Reason);

void AuxAddr(PADDR);
BOOL FAddrFromRibbonEdit(ADDR*);
BOOL FAddrFromSz(char *, ADDR*);
void BreakStatus(int);
BOOL GetSourceFrompADDR( LPADDR, LPSTR, WORD, LONG FAR *, UOFFSET FAR *, HSF FAR *);
int PASCAL BPNbrAtAddr(LPADDR,UOFFSET,BOOL fCheckDisabled = FALSE, UINT *pWidgetFlag = 0 );
BOOL PASCAL InitOSD(void) ;
BOOL PASCAL DebuggeeInCallBack(void);
CString GetDebugPathName(CDocument* pDoc);
BOOL LoadNonDiskDocument(LPCTSTR szName, CDocument** ppDoc);
BOOL FIsSqlInproc();
BOOL FIsActiveProjectDebuggable( BOOL *pUnknownTarget, BOOL *pIsExternal, BOOL *pTryToBuild, uniq_platform *pPlat );

#ifdef __cplusplus

template <class TYPE> class Protect
/*++

Abstract:

	A little template-class to make sure resources (handles) are properly
	deallocated.

	Use as:

	{
		HANDLE				hBla;
		Protect <HANDLE>	protect (hBla);

		.
		.
		.

		// Protect <HANDLE>::~Protect  automatically called to destruct hBla
	}

	Protect <HANDLE>::~Protect(
		)
	{
		if (m_h)
		{
			CloseHandle (m_h);
			m_h = NULL;
		}
	}

--*/
{
	TYPE&	m_h;
	
  public:
  
	Protect(
		TYPE&	h
		) : m_h (h)
	{
	}

	~Protect(
		);
};

#endif // __cplusplus


#if defined (TARGMACPPC)

typedef void (*SF)(TLUI *);

extern void QCEMSetup(HWND hwndOwner);
extern BOOL FMacQuit(void);
extern BOOL FMacDeleteEM(void);



#endif

#endif // __DBG_H__
