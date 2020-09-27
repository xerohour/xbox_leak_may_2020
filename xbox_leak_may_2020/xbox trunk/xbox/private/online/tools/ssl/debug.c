/*-----------------------------------------------------------------------------
* Copyright (C) Microsoft Corporation, 1995 - 1996.
* All rights reserved.
*
* This file is part of the Microsoft Private Communication Technology
* reference implementation, version 1.0
*
* The Private Communication Technology reference implementation, version 1.0
* ("PCTRef"), is being provided by Microsoft to encourage the development and
* enhancement of an open standard for secure general-purpose business and
* personal communications on open networks.  Microsoft is distributing PCTRef
* at no charge irrespective of whether you use PCTRef for non-commercial or
* commercial use.
*
* Microsoft expressly disclaims any warranty for PCTRef and all derivatives of
* it.  PCTRef and any related documentation is provided "as is" without
* warranty of any kind, either express or implied, including, without
* limitation, the implied warranties or merchantability, fitness for a
* particular purpose, or noninfringement.  Microsoft shall have no obligation
* to provide maintenance, support, upgrades or new releases to you or to anyone
* receiving from you PCTRef or your modifications.  The entire risk arising out
* of use or performance of PCTRef remains with you.
*
* Please see the file LICENSE.txt,
* or http://pct.microsoft.com/pct/pctlicen.txt
* for more information on licensing.
*
* Please see http://pct.microsoft.com/pct/pct.htm for The Private
* Communication Technology Specification version 1.0 ("PCT Specification")
*
* 1/23/96
*----------------------------------------------------------------------------*/


/*
 *  For ease of debugging the SPMgr, all the debug support functions have
 *  been stuck here.  Basically, we read info from win.ini, since that allows
 *  us to configure the debug level via a text file (and DOS, for example).
 *
 *  Format is:
 *
 *  win.ini
 *
 *  [Schannel]
 *      DebugFlags=<Flag>[<,Flag>]*
 *
 *  WHERE:
 *      Flag is one of the following:
 *          Error, Warning, Trace
 */


#include <spbase.h>

#if DBG         /* NOTE:  This file not compiled for retail builds */

DWORD  g_dwEventLogging=0;
HANDLE g_hfLogFile = NULL;

#include <stdio.h>
#include <stdarg.h>

#ifdef HEAPDUMP
//#include <winbasep.h>
#endif

#define WINDEBUG

#ifndef min
#define min(x,y) ((x)<(y)?(x):(y))
#endif

DWORD   g_dwInfoLevel  = 0;
DWORD   g_dwDebugBreak = 0;
DWORD   PctTraceIndent = 0;


#define MAX_DEBUG_BUFFER 2048


// This function simply outputs information to the debugging log file handle.

void
SPDebugOutput(char *szOutString)
{
    DWORD dwWritten;

    if (NULL != g_hfLogFile)
    {
        WriteFile(
		g_hfLogFile,
		szOutString,
		strlen(szOutString),
		&dwWritten,
		NULL);
    }
    OutputDebugStringA(szOutString);
}


void
InitDebugSupport(void)
{

}


void
DbgDumpHexString(const unsigned char *String, DWORD cbString)
{
#ifndef XBOX
    unsigned int i;

    for (i = 0; i < cbString; i++)
    {
	int j;
	char *pch;
	char ach[9];

	pch = &ach[sprintf(ach,  "%2.2x", String[i])];
	SP_ASSERT(pch - ach <= sizeof(ach) - 4);


	if ((i & 1) == 1)
	{
	    *pch++ = ' ';
	}
	if ((i & 7) == 7)
	{
	    *pch++ = ' ';
	}
	if ((i & 15) == 15)
	{
	    *pch++ = '\n';
	}
	*pch = '\0';
	SPDebugOutput(ach);
    }
#endif
}


char *aszSPDebugLevel[] = {
    "Error  ",
    "Warning",
    "Trace  ",
    "Mem    ",
    "Result "
};


void
SPDebugLog(long Mask, const char *Format, ...)
{
    va_list ArgList;
    int     Level = 0;
    int     PrefixSize = 0;
    int     iOut;
    char    szOutString[MAX_DEBUG_BUFFER];
    long    OriginalMask = Mask;

    if (Mask & g_dwInfoLevel)
    {
	while (!(Mask & 1))
	{
	    Level++;
	    Mask >>= 1;
	}
	if (Level >= sizeof(aszSPDebugLevel) / sizeof(char *))
	{
	    Level = sizeof(aszSPDebugLevel) / sizeof(char *) - 1;
	}
	// Make the prefix first:  "Process.Thread> GINA-XXX"

	iOut = sprintf(
			szOutString,
			"%3d.%3d> %s: ",
			333,
			GetCurrentThreadId(),
			aszSPDebugLevel[Level]);

	PrefixSize = min(60, PctTraceIndent * 3);
	FillMemory(szOutString+iOut, PrefixSize, ' ');
	PrefixSize += iOut;
	szOutString[PrefixSize] = '\0';

	va_start(ArgList, Format);

	if (vsprintf(&szOutString[PrefixSize], Format, ArgList) < 0)
	{
	    static char szOverFlow[] = "\n<256 byte OVERFLOW!>\n";

	    // Less than zero indicates that the string would not fit into the
	    // buffer.  Output a special message indicating overflow.

	    strcpy(
		&szOutString[sizeof(szOutString) - sizeof(szOverFlow)],
		szOverFlow);
	}
	va_end(ArgList);
	SPDebugOutput(szOutString);
    }
}


long
SPLogErrorCode(
    long err,
    const char *szFile,
    long lLine)
{
    char *szName = "Unknown";

    switch(err)
    {
    case PCT_ERR_OK: szName = "PCT_ERR_OK"; break;
    case PCT_ERR_BAD_CERTIFICATE: szName = "PCT_ERR_BAD_CERTIFICATE"; break;
    case PCT_ERR_CLIENT_AUTH_FAILED: szName = "PCT_ERR_CLIENT_AUTH_FAILED"; break;
    case PCT_ERR_ILLEGAL_MESSAGE: szName = "PCT_ERR_ILLEGAL_MESSAGE"; break;
    case PCT_ERR_INTEGRITY_CHECK_FAILED: szName = "PCT_ERR_INTEGRITY_CHECK_FAILED"; break;
    case PCT_ERR_SERVER_AUTH_FAILED: szName = "PCT_ERR_SERVER_AUTH_FAILED"; break;
    case PCT_ERR_SPECS_MISMATCH: szName = "PCT_ERR_SPECS_MISMATCH"; break;
    case PCT_ERR_SSL_STYLE_MSG: szName = "PCT_ERR_SSL_STYLE_MSG"; break;

    case PCT_INT_BUFF_TOO_SMALL: szName = "PCT_INT_BUFF_TOO_SMALL"; break;
    case PCT_INT_INCOMPLETE_MSG: szName = "PCT_INT_INCOMPLETE_MSG"; break;
    case PCT_INT_DROP_CONNECTION: szName = "PCT_INT_DROP_CONNECTION"; break;
    case PCT_INT_BAD_CERT: szName = "PCT_INT_BAD_CERT"; break;
    case PCT_INT_CLI_AUTH: szName = "PCT_INT_CLI_AUTH"; break;
    case PCT_INT_ILLEGAL_MSG: szName = "PCT_INT_ILLEGAL_MSG"; break;
    case PCT_INT_MSG_ALTERED: szName = "PCT_INT_MSG_ALTERED"; break;
    case PCT_INT_INTERNAL_ERROR: szName = "PCT_INT_INTERNAL_ERROR"; break;
    case PCT_INT_OUT_OF_MEMORY: szName = "PCT_INT_OUT_OF_MEMORY"; break;
    case PCT_INT_DATA_OVERFLOW: szName = "PCT_INT_DATA_OVERFLOW"; break;
    case PCT_INT_SPECS_MISMATCH: szName = "PCT_INT_SPECS_MISMATCH"; break;

    case WSAEINTR:               szName = "WSAEINTR";               break;
    case WSAEBADF:               szName = "WSAEBADF";               break;
    case WSAEACCES:              szName = "WSAEACCES";              break;
    case WSAEFAULT:              szName = "WSAEFAULT";              break;
    case WSAEINVAL:              szName = "WSAEINVAL";              break;
    case WSAEMFILE:              szName = "WSAEMFILE";              break;

    case WSAEWOULDBLOCK:         szName = "WSAEWOULDBLOCK";         break;
    case WSAEINPROGRESS:         szName = "WSAEINPROGRESS";         break;
    case WSAEALREADY:            szName = "WSAEALREADY";            break;
    case WSAENOTSOCK:            szName = "WSAENOTSOCK";            break;
    case WSAEDESTADDRREQ:        szName = "WSAEDESTADDRREQ";        break;
    case WSAEMSGSIZE:            szName = "WSAEMSGSIZE";            break;
    case WSAEPROTOTYPE:          szName = "WSAEPROTOTYPE";          break;
    case WSAENOPROTOOPT:         szName = "WSAENOPROTOOPT";         break;
    case WSAEPROTONOSUPPORT:     szName = "WSAEPROTONOSUPPORT";     break;
    case WSAESOCKTNOSUPPORT:     szName = "WSAESOCKTNOSUPPORT";     break;
    case WSAEOPNOTSUPP:          szName = "WSAEOPNOTSUPP";          break;
    case WSAEPFNOSUPPORT:        szName = "WSAEPFNOSUPPORT";        break;
    case WSAEAFNOSUPPORT:        szName = "WSAEAFNOSUPPORT";        break;
    case WSAEADDRINUSE:          szName = "WSAEADDRINUSE";          break;
    case WSAEADDRNOTAVAIL:       szName = "WSAEADDRNOTAVAIL";       break;
    case WSAENETDOWN:            szName = "WSAENETDOWN";            break;
    case WSAENETUNREACH:         szName = "WSAENETUNREACH";         break;
    case WSAENETRESET:           szName = "WSAENETRESET";           break;
    case WSAECONNABORTED:        szName = "WSAECONNABORTED";        break;
    case WSAECONNRESET:          szName = "WSAECONNRESET";          break;
    case WSAENOBUFS:             szName = "WSAENOBUFS";             break;
    case WSAEISCONN:             szName = "WSAEISCONN";             break;
    case WSAENOTCONN:            szName = "WSAENOTCONN";            break;
    case WSAESHUTDOWN:           szName = "WSAESHUTDOWN";           break;
    case WSAETOOMANYREFS:        szName = "WSAETOOMANYREFS";        break;
    case WSAETIMEDOUT:           szName = "WSAETIMEDOUT";           break;
    case WSAECONNREFUSED:        szName = "WSAECONNREFUSED";        break;
    case WSAELOOP:               szName = "WSAELOOP";               break;
    case WSAENAMETOOLONG:        szName = "WSAENAMETOOLONG";        break;
    case WSAEHOSTDOWN:           szName = "WSAEHOSTDOWN";           break;
    case WSAEHOSTUNREACH:        szName = "WSAEHOSTUNREACH";        break;
    case WSAENOTEMPTY:           szName = "WSAENOTEMPTY";           break;
    case WSAEPROCLIM:            szName = "WSAEPROCLIM";            break;
    case WSAEUSERS:              szName = "WSAEUSERS";              break;
    case WSAEDQUOT:              szName = "WSAEDQUOT";              break;
    case WSAESTALE:              szName = "WSAESTALE";              break;
    case WSAEREMOTE:             szName = "WSAEREMOTE";             break;

    case WSASYSNOTREADY:         szName = "WSASYSNOTREADY";         break;
    case WSAVERNOTSUPPORTED:     szName = "WSAVERNOTSUPPORTED";     break;
    case WSANOTINITIALISED:      szName = "WSANOTINITIALISED";      break;
    case WSAEDISCON:             szName = "WSAEDISCON";             break;
    case WSAENOMORE:             szName = "WSAENOMORE";             break;
    case WSAECANCELLED:          szName = "WSAECANCELLED";          break;
    case WSAEINVALIDPROCTABLE:   szName = "WSAEINVALIDPROCTABLE";   break;
    case WSAEINVALIDPROVIDER:    szName = "WSAEINVALIDPROVIDER";    break;
    case WSAEPROVIDERFAILEDINIT: szName = "WSAEPROVIDERFAILEDINIT"; break;
    case WSASYSCALLFAILURE:      szName = "WSASYSCALLFAILURE";      break;
    case WSASERVICE_NOT_FOUND:   szName = "WSASERVICE_NOT_FOUND";   break;
    case WSATYPE_NOT_FOUND:      szName = "WSATYPE_NOT_FOUND";      break;
    case WSA_E_NO_MORE:          szName = "WSA_E_NO_MORE";          break;
    case WSA_E_CANCELLED:        szName = "WSA_E_CANCELLED";        break;
    case WSAEREFUSED:            szName = "WSAEREFUSED";            break;

    }

    SPDebugLog(SP_LOG_RES, "Result: %s (0x%lx) - %s, Line %d\n", szName, err, szFile, lLine);

    return err;
}

#pragma warning(disable:4206)   /* Disable the empty translation unit */
				/* warning/error */

void
SPAssert(
    void * FailedAssertion,
    void * FileName,
    unsigned long LineNumber,
    char * Message)
{

    SPDebugLog(SP_LOG_ERROR,
               "Assertion FAILED, %s, %s : %d\n",
               FailedAssertion,
		       FileName,
		       LineNumber);

}

#ifdef HEAPDUMP

int HeapFlags = 0;

VOID
DumpHeapState(char *pszMsg)
{
    DWORD dwWritten;
    if (HeapFlags & 2)
    {
	HANDLE BaseHeap = RtlProcessHeap();
	HEAP_SUMMARY HeapSum;
	char szOutString[256];

	wsprintf(szOutString, "%s: HeapSummary(%x): ", pszMsg, BaseHeap);
	HeapSum.cb = sizeof(HeapSum);
	if (!HeapSummary(BaseHeap, 0, &HeapSum))
	{
	    wsprintf(
		&szOutString[lstrlen(szOutString)],
		"FAILED: %u\n",
		GetLastError());
	}
	else
	{
	    wsprintf(
		&szOutString[lstrlen(szOutString)],
		"Allocated=%x  Committed=%x\n",
		HeapSum.cbAllocated,
		HeapSum.cbCommitted);
	}
	SPDebugOutput(szOutString);
    }
}


VOID
CaptureStackBackTrace(
    EXCEPTION_POINTERS *pep,
    ULONG cSkip,
    ULONG cFrames,
    ULONG *aeip)
{
    FillMemory(aeip, cFrames * sizeof(aeip[0]), 0);

#if i386 == 1
    {
	ULONG ieip, *pebp;
	ULONG *pebpMax;
	ULONG *pebpMin;

	if (pep == NULL)
	{
	    ieip = 0;
	    cSkip++;                    // always skip current frame
	    pebp = ((ULONG *) &pep) - 2;
	}
	else
	{
	    ieip = 1;
	    SP_ASSERT(cSkip == 0);
	    aeip[0] = pep->ContextRecord->Eip;
	    pebp = (ULONG *) pep->ContextRecord->Ebp;
	}

	pebpMin = (ULONG *) &pep - 3;   // start below current frame
	pebpMax = pebpMin + 16*1024/sizeof(ULONG);
	if (pebp >= pebpMin && pebp < pebpMax)
	{
//          __try
	    {
		for ( ; ieip < cSkip + cFrames; ieip++)
		{
		    ULONG *pebpNext;

		    if (ieip >= cSkip)
		    {
			aeip[ieip - cSkip] = *(pebp + 1);  // save an eip
		    }

		    pebpNext = (ULONG *) *pebp;
		    if (pebpNext < pebp + 2 || pebpNext >= pebpMax - 1)
		    {
			break;
		    }
		    pebp = pebpNext;
		}
	    }
//          __except(EXCEPTION_EXECUTE_HANDLER)
	    {
	    }
	}
    }
#endif // i386 == 1
}


#define CMEMMAX 500
#define CEIP    6

int cMemAlloc;
int cMemAlloc2;
ULONG cbMemTotal;
ULONG cbMemTotal2;

typedef struct _MEM {
    VOID *pv;
    ULONG cb;
    ULONG aeip[CEIP];
} MEM;

MEM amem[CMEMMAX];

#endif // ifdef HEAPDUMP


VOID *
SPExternalAlloc(DWORD cb)
{
    VOID *pv;
#ifdef HEAPDUMP
    int i;
    char szOutString[256];
    int iRecorded = -1;
    MEM mem;

    DumpHeapState("Before Alloc");

    if (HeapFlags & 2)
    {
	CaptureStackBackTrace(NULL, 1, CEIP, mem.aeip);
	wsprintf(szOutString, "AllocMem(%x bytes): ", cb);
	cMemAlloc2++;
	cbMemTotal2 += cb;
	mem.cb = cb;
    }
#endif // ifdef HEAPDUMP
    pv = LocalAlloc(LMEM_FIXED | LMEM_ZEROINIT, cb);
#ifdef HEAPDUMP
    if (HeapFlags & 2)
    {
	mem.pv = pv;
	if (pv != NULL)
	{
	    for (i = 0; i < CMEMMAX; i++)
	    {
		if (amem[i].pv == pv)
		{
		    char szOutString2[256];

		    wsprintf(
			szOutString2,
			"FreeDup(%x) (%x bytes) from %x %x %x %x %x %x (%d)\n",
			pv,
			amem[i].cb,
			amem[i].aeip[0],
			amem[i].aeip[1],
			amem[i].aeip[2],
			amem[i].aeip[3],
			amem[i].aeip[4],
			amem[i].aeip[5],
			i);
		    SPDebugOutput(szOutString2);
		    cMemAlloc--;
		    cbMemTotal -= amem[i].cb;
		    amem[i].pv = NULL;
		}
		if (amem[i].pv == NULL && iRecorded == -1)
		{
		    amem[i] = mem;
		    cMemAlloc++;
		    cbMemTotal += cb;
		    iRecorded = i;
		}
	    }
	}
	else
	{
	    lstrcat(szOutString, "FAILED: ");
	}
	wsprintf(
	    &szOutString[lstrlen(szOutString)],
	    "%x from %x %x %x %x %x %x (%d)\n",
	    mem.pv,
	    mem.aeip[0],
	    mem.aeip[1],
	    mem.aeip[2],
	    mem.aeip[3],
	    mem.aeip[4],
	    mem.aeip[5],
	    iRecorded);
	SPDebugOutput(szOutString);
    }
    DumpHeapState("After Alloc");
#endif // ifdef HEAPDUMP
    return(pv);
}


VOID
SPExternalFree(VOID *pv)
{
#ifdef HEAPDUMP
    int i;
    char szOutString[256];

    DumpHeapState("Before Free");
    if (HeapFlags & 2)
    {
	cMemAlloc2--;
	wsprintf(szOutString, "FreeMem(%x)", pv);
	for (i = 0; i < CMEMMAX; i++)
	{
	    if (pv == amem[i].pv)
	    {
		cMemAlloc--;
		cbMemTotal -= amem[i].cb;
		amem[i].pv = NULL;
		wsprintf(
		    &szOutString[lstrlen(szOutString)],
		    " (%x bytes) from %x %x %x %x %x %x (%d)",
		    amem[i].cb,
		    amem[i].aeip[0],
		    amem[i].aeip[1],
		    amem[i].aeip[2],
		    amem[i].aeip[3],
		    amem[i].aeip[4],
		    amem[i].aeip[5],
		    i);
		break;
	    }
	}
    }
#endif // ifdef HEAPDUMP
    LocalFree(pv);
#ifdef HEAPDUMP
    if (HeapFlags & 2)
    {
	    lstrcat(szOutString, "\n");
	    SPDebugOutput(szOutString);
    }
    DumpHeapState("After Free");
#endif // ifdef HEAPDUMP
}


#endif /* DBG */ /* NOTE:  This file not compiled for retail builds */

