// STSFILE.C
//		Status file handling
//
//
#include "stdafx.h"
#pragma hdrstop

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

// [CAVIAR #5459 11/27/92 v-natjm]
// RebuildBreakpoint
//
static BOOL NEAR PASCAL RebuildBreakpoint (
	PBREAKPOINTNODE pBPDest,
	BREAKPOINTTYPES wType,
	LPSTR lpszLocation,
	LPSTR lpszExpression,
	int iLength,
	LPSTR lpszWndProc,
	UINT uMessageClass,
	UINT uMessageNum,
	BOOL bEnabled,
	BOOL bAmbig,
	int  TMindex,
	BOOL bWarnedUnsupported,
	long cPass,
	BOOL fSqlBp )
{

	// Clear all fields in the BREAKPOINTNODE destination
	_fmemset ( pBPDest, 0, sizeof ( BREAKPOINTNODE ) );

	// Action - breakpointtype
	pbpnType ( pBPDest ) = wType;
	pbpnPassCount( pBPDest ) = (USHORT)cPass;
	pbpnSqlBp( pBPDest ) = fSqlBp;

	// Location
	if ( ( wType == BPLOC ) ||
		 ( wType == BPLOCEXPRTRUE ) ||
		 ( wType == BPLOCEXPRCHGD ) ) {

		if  ( ! ParseCV400Location ( lpszLocation, pBPDest ) &&
			 ! ParseQC25Location ( lpszLocation, pBPDest ) ) {
			return FALSE;
		}
	}

	// Wnd Proc
	if ( wType == BPWNDPROCMSGRCVD ) {
		// Must have a wndproc and a message class/message
		if ( ! ParseWndProc ( lpszWndProc, pBPDest ) ||
			uMessageClass == msgNone
		) {
			return FALSE;
		}

		// Messages
		pbpnMessageClass ( pBPDest ) = uMessageClass;
		pbpnMessageNum ( pBPDest ) = uMessageNum;
	}

	// Expression
	if ( ( wType == BPLOCEXPRTRUE ) ||
		 ( wType == BPLOCEXPRCHGD ) ||
		 ( wType == BPEXPRTRUE) ||
		 ( wType == BPEXPRCHGD ) ) {
		if ( ! ParseExpression ( lpszExpression, pBPDest ) ) {
			return FALSE;
		}
	}

	// Length
	if ( ( wType == BPLOCEXPRCHGD ) ||
		 ( wType == BPEXPRCHGD ) ) {
		pbpnExprLen(pBPDest) = iLength;
	}

	// Set the enabled/disabled flag
	pbpnEnabled ( pBPDest ) = bEnabled;
	pbpnAmbigBP ( pBPDest ) = bAmbig ;
	pbpnBPTMindex ( pBPDest ) = TMindex ;
	pbpnWarnedUnsupported ( pBPDest ) = bWarnedUnsupported;

	// if "line type" bp, set the line numbers to indicate that breakpoint
	// is "restorable"
	// (i.e. set non- -1 initial line number
	if (pbpnFileLineNode(pBPDest))
		pbpnInitLine(pBPDest) = pbpnCurLine(pBPDest);

	// If get to here the Breakpoint action has necessary data
	return TRUE;
}

UINT nVersionNumber = 0x0003000A;

#define ENDOFRECORD ((DWORD)0x47414D00)

static const char BASED_CODE szOPTInfo[] = "Debugger";

BOOL LoadFromOPTFile(CStateSaver &stateSave)
{
	BOOL bRetval = TRUE;
	CInitFile fileInit;

	ASSERT(stateSave.IsLoading());

	if (!stateSave.OpenStream(fileInit, szOPTInfo))
		return FALSE;

	CArchive ar(&fileInit, CArchive::bNoFlushOnDelete | CArchive::load);
	CString str;

	try
	{
		long nFormatVersion;

		ar >> nFormatVersion;

		// For the OPT files, the formats must match exactly.
		if ((UINT&)nFormatVersion != nVersionNumber )
			AfxThrowArchiveException(CArchiveException::badSchema);

		//
		// Serialize breakpoint list
		//
		char szGeneralBuffer[cbBpCmdMax];
		WORD wLoop;
		WORD wBPCount;
		UINT uMsgNum, uMsgClass;
		long iTmp;
		long lBrkptType;
		BREAKPOINTTYPES wBrkptType;
		int iRet;
		LPSTR pc1, pc2;
		long bEnable;
		long bAmbig;
		long TMindex;
		long bWarnedUnsupported;
		BREAKPOINTNODE BpNode;
		long cPass;
		long fSqlBp;

	 	// read the number of breakpoints
		ar >> wBPCount;

		if (wBPCount)
		{
			// we have breakpoints to set. read them one by one
			// if an info is missing for a breakpoint, just skip it.
			// load the generic entry names
			// [CAVIAR # 11/26/92 v-natjm]
			// Save all necessary flags to restore Ambiguous BP

			for (wLoop = 0; wLoop < wBPCount; wLoop++)
			{
				ar >> lBrkptType;
				wBrkptType = (BREAKPOINTTYPES)lBrkptType;
				ar >> bEnable;
				ar >> bAmbig;
				ar >> TMindex;
				ar >> bWarnedUnsupported;
				ar >> cPass;
				ar >> fSqlBp;

				switch ( wBrkptType )
				{
					case BPLOC:
						// breakpoint at location
						ar >> str;
						_tcscpy(szGeneralBuffer, str);
						pc1 = szGeneralBuffer + _fstrlen(szGeneralBuffer);	// use strlen, not _tcslen, to get bytes not chars
						if (RebuildBreakpoint(&BpNode, wBrkptType,
								szGeneralBuffer, pc1, 0, pc1, 0, 0, bEnable,
								bAmbig, TMindex, bWarnedUnsupported, cPass, fSqlBp))
							AddBreakpointNode(&BpNode, FALSE, TRUE, TRUE, &iRet); // iRet unused here
						break;

					case BPLOCEXPRTRUE:
						// breakpoint at location when expression is true
						ar >> str;
						_tcscpy(szGeneralBuffer, str);
						pc1 = szGeneralBuffer + _fstrlen(szGeneralBuffer) + 1;	// use strlen, not _tcslen, to get bytes not chars
						ar >> str;
						_tcscpy(pc1, str);
						pc2 = pc1 + _fstrlen(pc1);
						if (RebuildBreakpoint(&BpNode, wBrkptType,
								szGeneralBuffer, pc1, 0, pc2, 0, 0, bEnable,
								bAmbig, TMindex, bWarnedUnsupported, cPass, fSqlBp))
							AddBreakpointNode(&BpNode, FALSE, TRUE, TRUE, &iRet);
						break;

					case BPLOCEXPRCHGD:
						// breakpoint at location when expression has changed
						ar >> str;
						_tcscpy(szGeneralBuffer, str);
						pc1 = szGeneralBuffer + _fstrlen(szGeneralBuffer) + 1;	// use strlen, not _tcslen, to get bytes not chars
						ar >> str;
						_tcscpy(pc1, str);
						pc2 = pc1 + _fstrlen(pc1);
						ar >> iTmp;
						if (RebuildBreakpoint(&BpNode, wBrkptType,
								szGeneralBuffer, pc1, iTmp, pc2, 0, 0,
								bEnable, bAmbig, TMindex, bWarnedUnsupported,
								cPass, fSqlBp))
							AddBreakpointNode(&BpNode, FALSE, TRUE, TRUE, &iRet);
						break;

					case BPEXPRTRUE:
						// breakpoint when expression is true
						ar >> str;
						_tcscpy(szGeneralBuffer, str);
						pc1 = szGeneralBuffer + _fstrlen(szGeneralBuffer);	// use strlen, not _tcslen, to get bytes not chars
						if (RebuildBreakpoint(&BpNode, wBrkptType, pc1,
								szGeneralBuffer, 0, pc1, 0, 0, bEnable,
								bAmbig, TMindex, bWarnedUnsupported, cPass, fSqlBp))
							AddBreakpointNode(&BpNode, FALSE, TRUE, TRUE, &iRet);
						break;

					case BPEXPRCHGD:
						// breakpoint when expression has changed
						ar >> str;
						_tcscpy(szGeneralBuffer, str);
						pc1 = szGeneralBuffer + _fstrlen(szGeneralBuffer);	// use strlen, not _tcslen, to get bytes not chars
						ar >> iTmp;
						if (RebuildBreakpoint(&BpNode, wBrkptType, pc1,
								szGeneralBuffer, iTmp, pc1, 0, 0, bEnable,
								bAmbig, TMindex, bWarnedUnsupported, cPass, fSqlBp))
							AddBreakpointNode(&BpNode, FALSE, TRUE, TRUE, &iRet);
						break;

					case BPWNDPROCMSGRCVD:
						// breakpoint at window procedure
						ar >> str;
						_tcscpy(szGeneralBuffer, str);
						pc1 = szGeneralBuffer + _fstrlen(szGeneralBuffer);	// use strlen, not _tcslen, to get bytes not chars
						ar >> iTmp;
						uMsgNum = (UINT)iTmp;
						ar >> iTmp;
						uMsgClass = (UINT)iTmp;
						if (RebuildBreakpoint(&BpNode, wBrkptType, pc1, pc1,
								0, szGeneralBuffer, uMsgClass, uMsgNum,
								bEnable, bAmbig, TMindex, bWarnedUnsupported,
								cPass, fSqlBp))
							AddBreakpointNode(&BpNode, FALSE, TRUE, TRUE, &iRet);
						break;
				}
			}
		}

		// free mem from old list
		ClearDLLInfo();

		// Restore the additional DLL grid information
		long cTargets;

		ar >> cTargets;
		for (int iTarg=0; iTarg < cTargets; iTarg++)
		{
			long cDLLRecs;
			CString strTargetName;
			HBLDTARGET hTarget;

			ar >> strTargetName;
			gpIBldSys->GetTarget(strTargetName, ACTIVE_BUILDER, &hTarget);
			ar >> cDLLRecs;
			for (int iRecs=0; iRecs < cDLLRecs; iRecs++)
			{
				long fPreload;
				CString strLocalName;
				CString strRemoteName;

				ar >> fPreload;  
				ar >> strLocalName;
				ar >> strRemoteName;

				// if target doesn't exist, don't add to dll info
				if (hTarget != NO_TARGET)
				{
					AddRecToDLLInfo(strLocalName, strRemoteName, (BOOL) fPreload, (HTARGET)hTarget);
				}
			}
		}


		//
		// Serialize the exception list
		//
		long Count,i;
		EXCEPTION_OBJECT Object;
		DWORD dwExceptionCode;
		LONG iAction;
		LONG iPlatformId;

		// Get # of exception in the file
		ar >> Count;

		// Reset current list
		InitList(DLG_EXCEP_LIST);
		EmptyList(DLG_EXCEP_LIST);

		// Check if list isn't empty
	    if (Count == -1)
			fExcepListInitialized = FALSE;
		else
		{
			fExcepListInitialized = TRUE;

			// Get each exception, and build a string
			for (i = 0; i < Count; i++)
			{
				ar >> dwExceptionCode;
				ar >> str;
				ar >> iAction;
				ar >> iPlatformId;

				MakeExceptionObject(&Object, dwExceptionCode, str,
					(int) iAction, (UINT) iPlatformId);

	            ListAddObject(DLG_EXCEP_LIST, (LPSTR)&Object,
					sizeof(EXCEPTION_OBJECT));
			}
		}


		//
		// Serialize the debugger file alias list
		//
		int cRestore;
		ar >> (long &)cRestore;
		while( cRestore-- )
		{
			CString	strFrom;
			CString	strTo;

			ar >> strFrom;
			ar >> strTo;

			// No hocus pocus, just call the internal routine.
			AddMapping(
				(LPSTR)(const char FAR *)strFrom,
				(LPSTR)(const char FAR *)strTo
			);
		}


		//
		// Serialize the watch window state.
		//
		g_persistWatch.Serialize(ar);
		g_persistVars.Serialize(ar);

		ar >> ((DWORD&) g_fPromptNoSymbolInfo);
		
		ar.Close();
		fileInit.Close();
	}
	catch (CArchiveException *e)
	{
		e->Delete();
		ar.Close();
		fileInit.Abort(); // will not throw an exception
		g_persistWatch.InitDefault();
		g_persistVars.InitDefault();
		g_fPromptNoSymbolInfo = TRUE;
		bRetval = FALSE;
	}

	// Note: At this point, if bRetval is false, then the OPT file is only
	// partially loaded and we may be in some sort of weird state

	if (bRetval)
	{
	}

	return bRetval;
}

BOOL SaveIntoOPTFile(CStateSaver &stateSave)
{
	BOOL bRetval = TRUE;
	CInitFile fileInit;

	ASSERT(stateSave.IsStoring());

	if (!stateSave.OpenStream(fileInit, szOPTInfo))
	{
		SetFileError(CFileException::generic);
		return FALSE;
	}

	CArchive ar(&fileInit, CArchive::bNoFlushOnDelete | CArchive::store);
	CString str;

	TRY
	{
		// Save format version:
		ar << ((long)nVersionNumber);

		//
		// Serialize breakpoint list
		//
		char szGeneralBuffer[cbBpCmdMax];
		WORD wBPCount = 0;
		BREAKPOINTTYPES wBrkptType;
		PBREAKPOINTNODE	pBpNode;
		long wLoop;

		// do a pass on the eventually existing breakpoints,
		// eliminating the breakpoints on assembly
		pBpNode = BHFirstBPNode();
		while (pBpNode != NULL)
		{
			// [CAVIAR #5878 11/27/92 v-natjm]
			++wBPCount;
			pBpNode = pbpnNext(pBpNode);
		}

		// write out the number of breakpoints to be saved
		ar << wBPCount;
		// write out the breakpoints
		pBpNode = BHFirstBPNode();
		for (wLoop = 0; wLoop < wBPCount; wLoop++)
		{
			// [CAVIAR #5878 11/27/92 v-natjm]
			// [CAVIAR #5459 11/27/92 v-natjm]
	 		wBrkptType = pbpnType(pBpNode);
			ar << (long)wBrkptType;
			ar << (long)pbpnEnabled(pBpNode);
			ar << (long)pbpnAmbigBP(pBpNode);
			ar << (long)pbpnBPTMindex(pBpNode);
			ar << (long)pbpnWarnedUnsupported(pBpNode);
			ar << (long)pbpnPassCount(pBpNode);
			ar << (long)pbpnSqlBp(pBpNode);

	 		// build the expression depending on each breakpoint type
			switch (wBrkptType)
			{
				case BPLOC:
					// breakpoint at location
					BuildCV400Location(pBpNode, szGeneralBuffer,
						sizeof(szGeneralBuffer), TRUE, TRUE, FALSE, FALSE);
					ar << (CString) szGeneralBuffer;
					break;

				case BPLOCEXPRTRUE:
					// breakpoint at location when expression is true
					BuildCV400Location(pBpNode, szGeneralBuffer,
						sizeof(szGeneralBuffer), TRUE, TRUE, FALSE, FALSE);
					ar << (CString) szGeneralBuffer;
					BuildCV400Expression(pBpNode, szGeneralBuffer,
						sizeof(szGeneralBuffer), TRUE, TRUE, FALSE);
					ar << (CString) szGeneralBuffer;
					break;

				case BPLOCEXPRCHGD:
					// breakpoint at location when expression has changed
					BuildCV400Location(pBpNode, szGeneralBuffer,
						sizeof(szGeneralBuffer), TRUE, TRUE, FALSE, FALSE);
					ar << (CString) szGeneralBuffer;
					BuildCV400Expression(pBpNode, szGeneralBuffer,
						sizeof(szGeneralBuffer), TRUE, TRUE, FALSE);
					ar << (CString) szGeneralBuffer;
					ar << (long)pbpnExprLen(pBpNode);
					break;

				case BPEXPRTRUE:
					// breakpoint when expression is true
					BuildCV400Expression(pBpNode, szGeneralBuffer,
						sizeof(szGeneralBuffer), TRUE, TRUE, FALSE);
					ar << (CString) szGeneralBuffer;
					break;

				case BPEXPRCHGD:
					// breakpoint when expression has changed
					BuildCV400Expression(pBpNode, szGeneralBuffer,
						sizeof(szGeneralBuffer), TRUE, TRUE, FALSE);
					ar << (CString) szGeneralBuffer;
					ar << (long)pbpnExprLen(pBpNode);
					break;

				case BPWNDPROCMSGRCVD:
					// breakpoint at window procedure
					BuildCV400Location(pBpNode, szGeneralBuffer,
						sizeof(szGeneralBuffer), TRUE, TRUE, FALSE, FALSE);
					ar << (CString) szGeneralBuffer;
					ar << (long)pbpnMessageNum(pBpNode);
					ar << (long)pbpnMessageClass(pBpNode);
					break;
			}

			pBpNode = pbpnNext(pBpNode);
		}



		//
		// Serialize DLL grid information
		//

		POSITION posGridList;
		POSITION posDLLRecList;
		GRIDINFO *pGridInfo;
		CPtrList *pDLLRecList;
		DLLREC *pDLLRec;
		CString strTargetName;

		long count = (long)DLLGridInfoList.GetCount();

		if (!DLLGridInfoList.IsEmpty())	{
			posGridList = DLLGridInfoList.GetHeadPosition();
		} else {
			posGridList = NULL;
		}

		while (posGridList != NULL)
		{
			pGridInfo = (GRIDINFO *)DLLGridInfoList.GetNext(posGridList);
			if( !(gpIBldSys->GetTargetName((HBLDTARGET)pGridInfo->hTarget, strTargetName, ACTIVE_BUILDER) == S_OK) ){
				count--;
			}
		}

		ar << count;

		if (!DLLGridInfoList.IsEmpty())	{
			posGridList = DLLGridInfoList.GetHeadPosition();
		} else {
			posGridList = NULL;
		}

		while (posGridList != NULL)
		{
			pGridInfo = (GRIDINFO *)DLLGridInfoList.GetNext(posGridList);
			pDLLRecList = &pGridInfo->DLLRecList;

			BOOL bTest;

			bTest = gpIBldSys->GetTargetName((HBLDTARGET)pGridInfo->hTarget, strTargetName, ACTIVE_BUILDER) == S_OK;

			if( bTest ){
				ar << strTargetName;
				ar << (long)pDLLRecList->GetCount();
		
				posDLLRecList = pDLLRecList->GetHeadPosition();
				while (posDLLRecList != NULL) {
					pDLLRec = (DLLREC *)pDLLRecList->GetNext(posDLLRecList);

					ar << (long)pDLLRec->fPreload;
					ar << pDLLRec->strLocalName;
					ar << pDLLRec->strRemoteName;
				}
			}
		}

		
		//
		// Serialize exception list
		//
		int Count,i;
		EXCEPTION_OBJECT Object;

		// Get # of exception in our list
		// and write it in the sts file
		if (fExcepListInitialized)
		{
			Count = (int)ListGetCount(DLG_EXCEP_LIST);
			ar << (long)Count;
		}
		else
		{
			Count = 0;
			ar << (long)-1;
		}

		// Get each exception, and build a string
		for (i = 0; i < Count; i++)
		{
			if (!ListGetObject(i, DLG_EXCEP_LIST, (LPSTR)&Object))
			{
				ASSERT(FALSE);
			}
			else
			{
			ar << (DWORD) Object.dwExceptionCode;
			ar << (CString) Object.Name;
			ar << (LONG) Object.iAction;
			ar << (LONG) Object.iPlatformId;
			}
		}


		//
		// Serialize the debugger file alias list
		//
		ar << (long)cmappath;
		for(i = 0; i < cmappath; i++)
		{
			ASSERT( rgmappath[i].lszFrom );
			ASSERT( rgmappath[i].lszTo );

			ar << (CString) rgmappath[i].lszFrom;
			ar << (CString) rgmappath[i].lszTo;
		}


		//
		// Serialize the watch window state.
		//
		g_persistWatch.Serialize(ar);
		g_persistVars.Serialize(ar);

		ar << (DWORD) g_fPromptNoSymbolInfo ;
		
		ar.Close();
		fileInit.Close();
	}
	CATCH_ALL (e)
	{
#ifdef _DEBUG
		AfxDump(e);
		TRACE("\n");
#endif	// _DEBUG

		if (e->IsKindOf(RUNTIME_CLASS(CFileException)))
			SetFileError(((CFileException*) e)->m_cause);
		else
			SetFileError(CFileException::generic);
		ar.Close();
		fileInit.Abort();
		g_fPromptNoSymbolInfo = TRUE;
		bRetval = FALSE;
	}
	END_CATCH_ALL

	return bRetval;
}
