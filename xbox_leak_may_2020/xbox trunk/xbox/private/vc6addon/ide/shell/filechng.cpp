//-----------------------------------------------------------------------------
//  filechng.cpp
//
//  Copyright (C) 1993, Microsoft Corporation
//
//  Purpose:    Implement the CFileChange class
//
//  Functions/Methods present:
//
//  Revision History:
//
//  []      14-Jan-1994 Dans    Created
//
//-----------------------------------------------------------------------------

#include "stdafx.h"
#include <process.h>

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

#if defined(_DEBUG)
#define debug(x)	x
#else
#define debug(x)
#endif

//-----------------------------------------------------------------------------
// Road map to file change
//
//  The main features of the CFileChange class are 1) the auxilliary thread
//  that handles the file change events and requests from the other threads and
//  2) the data structures that represent how it looks upon the file change.
//
//  Basically, we have a mapping of CString to a CListPtr using CMapStringToOb,
//  that is implemented in a subclass called CSafeMapStringToOb (it will clean
//  up all the lists on the range side of the mapping, unlike CMapStringToOb).
//  These CStrings in the map are the directory names we are watching.  The
//  Win32 file change api's watch only directories and leave the responsibility
//  of finding the files that actually changed to the programmer.  That is the
//  reason we have this machinery.
//
//  The CPtrList (implemented in CSafePFiledataList) contains all the files
//  in the directory that we are interested in.  A CFileData object is a
//  CString along with some extra data for determining if the file has changed
//  on disk.  This includes a buffer (StatBuf), the ignore count,
//  and some housekeeping data.
//
//  So, what we have is really just another implementation of a directory
//  hierarchy:!!!:
//
//  CSafeMapStringToOb
//      |
//      CString (directory1) -> CSafePFiledataList
//      |                       |
//      |                       CFileData (file1 + aux data)
//      |                       CFileData (file2 + aux data)
//      |
//      CString (directory2) -> CSafePFiledataList
//                              |
//                              CFileData (file3 + aux data)
//
//  Also, we have two arrays that create a reverse mapping from Win32
//  FileChange handles to a directory so that we can map from:
//  HANDLE->Directory->FileList quickly to see if any of the files we have
//  an interest in were the ones that fired the file change event.  This is
//  encapsulated in the HtoStr structure embedded in the CFileChange object.
//  Note that the handle array and the string array are kept discrete because
//  Win32 requires an array of handles in the WaitForMultipleObjects api call.
//
//  The class CSafePFiledataList also has the index of the corresponding
//  handle and string in the HtoStr structure (this is kept in _dwAux) for
//  the purpose of keeping the HtoStr arrays of handles and strings packed as
//  is necessary for WaitForMultipleObjects call.  We have to know which
//  HANDLE and string in the HtoStr arrays to remove when the last file in
//  a directory is removed, so we don't have to watch for changes in that
//  directory anymore.
//
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  Global data, this object is the main CFileChange instance for everyone
//-----------------------------------------------------------------------------
CFileChange	g_filechange;

CFileChange *
GetFileChange() {
	return &g_filechange;
	}
//----------------------------------------------------------------------------
// MakeFileLCase()
// 
//	Purpose:	lowercase the filename only if it is not dbcs
//----------------------------------------------------------------------------
void MakeFileLCase (LPSTR szFileLower, LPCSTR szFileName)
{
	CONST TCHAR * pch1 = szFileName;
	// DevStudio96 #14887 [patbr]: use unsigned chars--otherwise sign
	// extension will mess up the isupper() and tolower() calls below.
	unsigned char * pch2 = (unsigned char *)szFileLower;
	while (*pch1!= '\0')
	{
		if (IsDBCSLeadByte (*pch1))
		{
			*pch2 = *pch1;
			pch1++ ; pch2++;
			*pch2 = *pch1;
			pch1++; pch2++;
		}
		else
		{
			*pch2 = *pch1;
			if (isupper (*pch2))
				*pch2 = _tolower (*pch2);
			pch1++; pch2++;
		}
	}
	*pch2 = '\0';
};

//-----------------------------------------------------------------------------
//  FDirFromFile
//
//  Purpose:    provide the directory for the fullpath filename input
//
//  Input:      str,        CString reference for output
//              szFileName, filename to return directory of
//
//  Output:     str is updated with the directory of the file passed in.
//
//  Returns:    TRUE if successful
//
//  Note:       file must be full path form already.
//
//-----------------------------------------------------------------------------
BOOL    FDirFromFile ( CString & str, LPCTSTR szFileName ) {

	CPath   path;
	CDir    dir;
	LPSTR	szFileLower;
	BOOL	bRet = FALSE;
	szFileLower = new TCHAR [_tcslen(szFileName) + 1];

	::MakeFileLCase (szFileLower, szFileName);
	if ( path.Create ( szFileLower ) && dir.CreateFromPath ( path ) ) {
		str = dir;
		// Make sure we can be found in the file list... all lowercase.
		bRet = fTrue;
		}
	else {
		bRet = fFalse;
		}
	delete []szFileLower;
	return bRet;

	}

//-----------------------------------------------------------------------------
//  ctor/dtor for CFileChange class
//-----------------------------------------------------------------------------
CFileChange::CFileChange() {
	_cIdles = 0;
	_cBoosts = 0;
	_fThreadRunning = fFalse;
	_fDupThreadHandle = fFalse;
	_hThread = 0;
	memset ( &_hts, 0, sizeof(_hts) );
	_hts.rgh[ iHandleITC ] = _eventIn;
	}

CFileChange::~CFileChange() {
	if ( _fThreadRunning ) {
		Terminate();
		}
	}

//-----------------------------------------------------------------------------
//  CFileChange::FInit
//
//  Purpose:    initialize a CFileChange object for use
//
//  Input:      pfn,    callback function to call on each filechange notification
//              dwUser, optional parameter to pass to each invocation of callback
//
//  Returns:    TRUE on success
//
//  Note:       starts up auxilliary thread to handle file change notifications
//
//-----------------------------------------------------------------------------


BOOL    CFileChange::FInit ( DWORD dwUser ) {
	CWinThread * pWinThread = NULL;
	ASSERT ( !_fThreadRunning );
	if ( !_fThreadRunning ) {
		 _fThreadRetCode = fFalse;
		_dwUser = dwUser;
		if ( (pWinThread = AfxBeginThread ( CFileChange::ThreadEntry, this, prtyStandard, 0x10000)) != NULL) {
			if ( !(_fDupThreadHandle = 
					DuplicateHandle ( ::GetCurrentProcess( ),
									pWinThread->m_hThread,
									::GetCurrentProcess( ),
									&_hThread,
									DUPLICATE_SAME_ACCESS,
									TRUE,
									DUPLICATE_SAME_ACCESS
						     		)
						)
				) {
					ASSERT(FALSE);
					_hThread = pWinThread->m_hThread; 
			}

			_eventOut.Wait();
			}
		return _fThreadRetCode;
		}
	return fFalse;
	}

//-----------------------------------------------------------------------------
//  CFileChange::Terminate
//
//  Purpose:    handle the main thread cleanup chores
//
//  Note:       this tells the aux thread to cleanup and exit if it exists,
//              else it cleans up on its own.  generally, the only time the main
//              thread needs to do this is when ExitProcess is called, and we
//              are in a DLL and we have not cleaned up yet--NT kills all threads
//              prior to calling the DllMain code.
//
//-----------------------------------------------------------------------------
void    CFileChange::Terminate() {

	if ( _hThread ) {
		DWORD	dw = ::WaitForSingleObject ( _hThread, 0 );

		if ( dw == WAIT_OBJECT_0 ) {
			// thread handle signalled means thread is dead--probably
			// killed by system during cleanup, we need to do it for it
			if ( _fThreadRunning ) {
				ThreadCleanup();
				}
			}
		else {
			FCItem	fci;
			IDEL	idelDummy;
			
			fci._op = FCItem::opQuit;
			fci._pfnAsyncReturn = CFileChange::SyncCallback;
			fci._pvUser1 = this;
			_queueFCItem.put ( fci, idelDummy );
			_eventIn.Set();

			_eventOut.Wait ( 1000 );

			// We are assuming that 10 seconds is enough time for the 
			// other thread terminate. Might want to revisit this. 
			dw = ::WaitForSingleObject(_hThread, 10000 );
			ASSERT (_fDupThreadHandle == FALSE || dw == WAIT_OBJECT_0);
			}


		if ( _fDupThreadHandle ) {
			CloseHandle(_hThread);
			}

		_hThread = NULL;
		_fThreadRunning = fFalse;
		_fDupThreadHandle = fFalse;

		}
	}

//-----------------------------------------------------------------------------
//  CFileChange::FAddFile
//
//  Purpose:    add a file to the file change list, main thread access
//
//  Input:      szFile, file (fullpath!) to add.
//
//  Returns:    TRUE if successful
//
//-----------------------------------------------------------------------------
BOOL    CFileChange::FAddFile ( LPCSTR szFile, PfnFCCallBack pfn, BOOL fCallAlways /* = fFalse */ ) {
	if ( _fThreadRunning ) {
		// note that szFile must be a full path

		//
		// Don't bother making inter-thread call if file doesn't even exist.
		//
		WIN32_FILE_ATTRIBUTE_DATA find;
		if (Compatible_GetFileAttributesEx( szFile, GetFileExInfoStandard, &find))
			return ThreadCall ( szFile, pfn, FCItem::opAdd, fCallAlways );
		else
			return fFalse;
		}
	return fFalse;
	}

//-----------------------------------------------------------------------------
//  CFileChange::FDelFile
//
//  Purpose:    remove a file from the filechange list, main thread access
//
//  Input:      szFile, file (fullpath!) to remoev
//
//  Returns:    TRUE if file was in the list and was removed.
//
//-----------------------------------------------------------------------------
BOOL    CFileChange::FDelFile ( LPCTSTR szFile, PfnFCCallBack pfn ) {
	if ( _fThreadRunning ) {
		return ThreadCall ( szFile, pfn, FCItem::opDel );
		}
	return fFalse;
	}

//-----------------------------------------------------------------------------
//  CFileChange::FIgnoreFile
//
//  Purpose:    used to temporarily ignore filechange events on a specific file
//              or to restore an ignored file to unignored status.  it is a
//              counting ignore flag.
//
//  Input:      szFile,     file to ignore/unignore
//              fIgnore,    whether to ignore/unignore
//
//  Returns:    TRUE if file is in the list
//
//-----------------------------------------------------------------------------
BOOL    CFileChange::FIgnoreFile ( LPCTSTR szFile, BOOL fIgnore /* = fTrue */,
	PfnFCCallBack pfn /* = NULL */) {
	if ( _fThreadRunning ) {
		return ThreadCall ( szFile, pfn, (fIgnore ? FCItem::opIgnore : FCItem::opUnignore) );
		}
	return fFalse;
	}

//-----------------------------------------------------------------------------
//  CFileChange::FSyncFile
//
//  Purpose:    used to synchronize the filechange data with the file system.
//              typically used after an ignore/unignore phase to make sure that
//              if the file change event comes after the last unignore, we
//              don't have unintended change event notifications.
//
//  Input:      szFile,     file to ignore/unignore
//
//  Returns:    TRUE if file is in the list
//
//-----------------------------------------------------------------------------
BOOL    CFileChange::FSyncFile ( LPCTSTR szFile ) {
	if ( _fThreadRunning ) {
		return ThreadCall ( szFile, NULL, FCItem::opSyncFile );
		}
	return fFalse;
	}

//-----------------------------------------------------------------------------
//  CFileChange::FFilelistEtcFromFilename
//
//  Purpose:    Utility function to do the lookup to find the filelist and
//              filedata associated with the filename, if they exist.
//
//  Input:      szFilename, the filename to search for
//
//  Output:     pFilelist,  reference to the filelist if we find the directory
//              pFiledata,  reference to the filedata if we find the file
//              posRet,     reference to position of found filedata in filelist
//
//  Returns:    TRUE if successful
//
//  Note:       the caller of this fuction must own the critical section.
//
//-----------------------------------------------------------------------------
BOOL    CFileChange::FFilelistEtcFromFilename (
	LPCTSTR                 szFilename,
	CSafePFileDataList * &  pFilelist,
	CFileData * &           pFiledata,
	POSITION &              posRet
	) {

	BOOL    fRet = fFalse;
	CString strPath;

	pFilelist = NULL;
	pFiledata = NULL;
	posRet = 0;
	VERIFY ( ::FDirFromFile ( strPath, szFilename ) );

	CObject *   pObj;
	if ( _mpDirsFilelist.Lookup ( strPath, pObj ) ) {
		// directory exists
		ASSERT ( pObj->IsKindOf(RUNTIME_CLASS(CPtrList)) );
		pFilelist = (CSafePFileDataList *) pObj;

		POSITION    pos = pFilelist->GetHeadPosition();
		POSITION    posCur;

		while ( posCur = pos ) {
			CFileData * pFile = (CFileData *) pFilelist->GetNext ( pos );
			if ( pFile->CompareNoCase ( szFilename ) == 0 ) {
				// found one, return it and leave
				fRet = fTrue;
				posRet = posCur;
				pFiledata = pFile;
				break;
				}
			}
		}
	else {
		// not in the directory list, try the overspill...
		POSITION    pos;
		CFileData * pFile = PFileDataOverspill ( szFilename, pos );
		if ( pFile ) {
			fRet = fTrue;
			posRet = pos;
			pFiledata = pFile;
			pFilelist = NULL;
			}
		}
	return fRet;
	}

//-----------------------------------------------------------------------------
//  CFileChange::ThreadEntry
//
//  Purpose:    static method used for aux thread entry point
//
//  Input:      pv, void pointer that points to the CFileChange instance
//              that the thread is running in.
//
//-----------------------------------------------------------------------------
UINT CFileChange::ThreadEntry ( void * pv ) {
	CFileChange *   pfc = (CFileChange *) pv;
	ASSERT ( pfc );

	pfc->_fThreadRunning = GetCurrentThreadId();

	// success returned to calling thread.
	pfc->ThreadReturn ( fTrue );
	pfc->WaitAndHandleIt();
	pfc->_fThreadRunning = fFalse;
	
	return 0;
	}

//-----------------------------------------------------------------------------
//  CFileChange::SyncCallback
//
//  Purpose:    static method used for calling back to force synchronous
//				behavior using the async calls.
//
//  Input:      pfci, const pointer to an FCItem that was operated on.
//
//-----------------------------------------------------------------------------
void    CFileChange::SyncCallback ( const FCItem * pfci ) {
	ASSERT ( pfci );
	ASSERT ( pfci->_pvUser1 );

	CFileChange *	pfc = (CFileChange*) pfci->_pvUser1;

	pfc->ThreadReturn ( pfci->_fAsyncReturn );
	}

//-----------------------------------------------------------------------------
//  CFileChange::ThreadCall
//
//  Purpose:    handles the common functions for the synchronous calls
//-----------------------------------------------------------------------------
BOOL
CFileChange::ThreadCall (
	LPCTSTR			szFile,
	PfnFCCallBack	pfn,
	FCItem::Op		op,
	BOOL			fCallAlways /* =fTrue */
	) {
	ASSERT (
		op == FCItem::opAdd ||
		op == FCItem::opDel ||
		op == FCItem::opQuit ||
		op == FCItem::opIgnore ||
		op == FCItem::opUnignore ||
		op == FCItem::opSyncFile
		);
	CritSection	cs(_critsecCall);

	ASSERT( 0 < _tcslen(szFile) );
	
	_fcitemThread._op = op;
	_fcitemThread._strFileName = szFile;
	_fcitemThread._pfnFCCallBack = pfn;
	_fcitemThread._fCallAlways = fCallAlways;
	_fcitemThread._pfnAsyncReturn = CFileChange::SyncCallback;
	_fcitemThread._pvUser1 = this;
	
	IDEL	idelDummy;

	BoostPriority();

	_queueFCItem.put ( _fcitemThread, idelDummy );
	_eventIn.Set();
	_eventOut.Wait();

	RestorePriority();

	return _fThreadRetCode;
	}

//-----------------------------------------------------------------------------
//  CFileChange::WaitAndHandleIt
//
//  Purpose:    the main routine for handling the events being signaled for the
//              aux thread to do something, either a file change event or a
//              request from another thread to add, delete, or ignore a file.
//
//-----------------------------------------------------------------------------
void    CFileChange::WaitAndHandleIt() {

	DWORD		cMsecsCumulative = 0;
	DWORD		cMsecs = ::GetTickCount();

	ASSERT ( _mpDirsFilelist.GetCount() == 0 ); // must be 0 at start
	ASSERT ( _hts.rgh[ iHandleITC ] == _eventIn );

	for ( ; ; ) {   // forever
		unsigned    cHandles;
		if ( _cIdles ) {
			cHandles = 1;
			}
		else {
			cHandles = _mpDirsFilelist.GetCount() + 1;
			}

		DWORD   dwWaitCode;
		dwWaitCode = ::WaitForMultipleObjects (
			cHandles,
			_hts.rgh,
			fFalse,
			DwTimeout()
			);

		ASSERT (
			dwWaitCode == WAIT_TIMEOUT ||
			dwWaitCode == WAIT_FAILED ||
			(dwWaitCode >= WAIT_OBJECT_0 && dwWaitCode < WAIT_OBJECT_0 + cHandles)
			);

		DWORD	cMsecsNew = ::GetTickCount();

		if ( _cIdles ) {
			cMsecsCumulative += cMsecsNew - cMsecs;
			}
		else {
			cMsecsCumulative = 0;
			}
		cMsecs = cMsecsNew;

		if ( dwWaitCode == WAIT_FAILED ) {
			TRACE("WaitForMultipleObjects returned WAIT_FAILED; file change notification thread exiting...\n");
			ThreadReturn ( fFalse );
			break;
			}
		else if ( dwWaitCode == WAIT_TIMEOUT ) {
			// we need to do the manual list now and accumulate our idle time
			DoCheckOverspillFiles();
			if ( _cIdles && cMsecsCumulative >= cIdleMsecsCumulative ) {
				// cycle each of the file change handles...
				cMsecsCumulative = 0;
				for (
					unsigned iHandle = 1;
					iHandle <= unsigned(_mpDirsFilelist.GetCount());
					iHandle++
					) {
					if ( WAIT_OBJECT_0 == WaitForSingleObject ( _hts.rgh[ iHandle ], 0 ) ) {
						DoFileChange ( iHandle );
						}
					}
				}
			}
		else {	// only thing left is that an event is signalled.
			unsigned    iHandle = dwWaitCode - WAIT_OBJECT_0;

			if ( iHandleITC == iHandle ) {
				// our inter-thread communication event handle
				if ( !FThreadDoRequest() ) {
					// opQuit came through if FDoRequest returns fFalse
					break;
					}
				}
			else {
				// a file change notification came through--see if we care
				DoFileChange ( iHandle );
				}
			}
		}
	}

//-----------------------------------------------------------------------------
//  CFileChange::FThreadDoRequest
//
//  Purpose:    handles requests from other threads in our aux thread for all
//              the requests (add, delete, ignore/unignore, terminate).
//
//  Returns:    TRUE if not FCItem::opQuit, FALSE if we are quitting.
//
//-----------------------------------------------------------------------------
BOOL    CFileChange::FThreadDoRequest() {

	BOOL    fRet = fTrue;   // this function always returns true unless
							//  the thread is quitting.

	BoostPriority();
	while ( _queueFCItem.size() ) {
		FCItem	fci;
		IDEL	idelDummy;
		_queueFCItem.get ( fci, idelDummy );


		ASSERT ( int(fci._op) < int(FCItem::opFileChanged) );

		switch ( fci._op ) {
			case FCItem::opAdd : {
				ASSERT ( fci._strFileName.GetLength() );
				fci._fAsyncReturn = FThreadAddFile ( fci );
				break;
				}
			case FCItem::opDel : {
				ASSERT ( fci._strFileName.GetLength() );
				fci._fAsyncReturn = FThreadDelFile ( fci );
				break;
				}
			case FCItem::opIgnore :
			case FCItem::opUnignore : {
				ASSERT ( fci._strFileName.GetLength() );
				fci._fAsyncReturn = FThreadIgnoreFile ( fci );
				break;
				}
			case FCItem::opSyncFile : {
				ASSERT ( fci._strFileName.GetLength() );
				fci._fAsyncReturn = FThreadSyncFile ( fci );
				break;
				}
			case FCItem::opNull :
			case FCItem::opFileChanged : {
				// do nothing on these requests--they are meaningless
				fci._fAsyncReturn = fFalse;
				break;
				}
			case FCItem::opQuit : {
				ThreadCleanup();
				fci._fAsyncReturn = fTrue;
				fRet = fFalse;
				break;
				}
			}
		if ( fci._pfnAsyncReturn ) {
			fci._pfnAsyncReturn ( &fci );
			}
		}
	
	RestorePriority();
	return fRet;
	}

//-----------------------------------------------------------------------------
//  CFileChange::FThreadAddFile
//
//  Purpose:    aux thread side of adding a file to the file change list
//
//  Input:      input from fci set up in calling thread.
//
//  Output:     if a valid file, added to the list
//
//  Returns:    TRUE if everything copasetic
//
//-----------------------------------------------------------------------------
BOOL    CFileChange::FThreadAddFile ( FCItem & fci ) {

	CFileData * pFile = new CFileData ( fci._strFileName );
	if ( NULL == pFile ) {
		return fFalse;
		}

	pFile->FAddCallBack ( fci._pfnFCCallBack, fci._fCallAlways );

	StatBuf sb;
 	if ( pFile->FGetCachedStatBuf(sb) && (sb.dwAttrs & StatBuf::fattrDir) ) {
		delete pFile;
		return fFalse;
		}

	fci._statNew = sb;

	BOOL                    fRet = fTrue;
	CSafePFileDataList *    pFileList;
	CFileData *             pFileData = NULL;
	POSITION                posDummy;

	CritSection	cs(_critSection);

	if ( !FFilelistEtcFromFilename ( fci._strFileName, pFileList, pFileData, posDummy ) ) {
		if ( !pFileList ) {
			ASSERT ( pFileData == NULL );
			// directory is new, must add a entry and open a filechange event
			if ( _mpDirsFilelist.GetCount() < cDirMax ) {
				pFileList = new CSafePFileDataList;
				CString strPath;

				VERIFY ( ::FDirFromFile ( strPath, fci._strFileName ) );

				unsigned    i = _mpDirsFilelist.GetCount() + 1; // next index is the count of dirs
				pFileList->_dwAux = i;
				_hts.rgh[ i ] = ::FindFirstChangeNotification (
					strPath,            // watch the directory
					fFalse,             // don't watch subtree
					DwStdFileChange()   // watch for standard file changes
					);
				if ( _hts.rgh[ i ] != INVALID_HANDLE_VALUE ) {
					_mpDirsFilelist.SetAt ( strPath, pFileList );
					_hts.rgpstr[ i ] = new CString ( strPath );
					}
				else {
					// couldn't add it for some reason...add to manual list if the dir
					// exists
					CDir	dir;
					dir.CreateFromString ( strPath );

					if ( dir.ExistsOnDisk() ) {
						if ( !FAddOverspillFile ( pFile ) ) {
							delete pFile;
							}
						}
					else {
						delete pFile;
						fRet = fFalse;
						}
					delete pFileList;
					pFileList = NULL;
					}
				}
			else {
				CString strPath;

				VERIFY ( ::FDirFromFile ( strPath, fci._strFileName ) );
				CDir	dir;
				dir.CreateFromString ( strPath );

				if ( dir.ExistsOnDisk() ) {
					if ( !FAddOverspillFile ( pFile ) ) {
						delete pFile;
						}
					}
				else {
					delete pFile;
					fRet = fFalse;
					}
				}
			}

		// at this point, we must have a file list, else we bail
		if ( pFileList ) {
			ASSERT ( pFile != NULL );
			pFileList->AddTail ( (void *) pFile );
			}
		}

	else {
		// file already exists in our list, delete our filedata
		// So now we'd better add the callback to the filedata
		// structure if it's not already there
		pFileData->FAddCallBack ( fci._pfnFCCallBack, fci._fCallAlways );
		delete pFile;
		}

	return fRet;
	}

//-----------------------------------------------------------------------------
//  CFileChange::DoCheckFile
//
//  Purpose:    check a given file to see if it has been modified and if
//				so send out a notification by calling the callback function
//
//  Input:      pFileData,	a pointer to the CFileData object for the file
//				to check
//
//  Note:       if a file was found to have changed, we call the callback
//              function with a FCItem class filled out.
//
//-----------------------------------------------------------------------------
void CFileChange::DoCheckFile ( CFileData * pFileData ) {
	StatBuf sbPrev, sbCur;
	FCBits  fcbits = {0};
	BOOL    fPrevValid = pFileData->FGetCachedStatBuf ( sbPrev );

	if ( pFileData->FSetCachedStatBuf ( sbCur ) ) {
		fcbits.fTime = (sbPrev.fcft.qwFiletime != sbCur.fcft.qwFiletime);
		fcbits.fSize = (sbPrev.cbFile != sbCur.cbFile);
		if ( sbPrev.dwAttrs != sbCur.dwAttrs ) {
			DWORD	modePrev = sbPrev.dwAttrs;
			DWORD	modeCur = sbCur.dwAttrs;

			ASSERT ( !fPrevValid || !(modePrev & StatBuf::fattrDir) );
			if ( (modeCur & StatBuf::fattrDir) ) {
				// not a file anymore!
				fcbits.fType = fTrue;
				}
			// check for read/write <-> read/only changes
			modePrev &= FattrFileMask();
			modeCur  &= FattrFileMask();
			fcbits.fAttrs = (modeCur != modePrev);
			}
		}
	else if ( fPrevValid ) {
		// not available, file has been deleted?
		fcbits.fDel = TRUE;
		// leave it in the list, may just be unavailable
		}

	// any changes happen?  if so, do the callback
	if ( fcbits.fAll ) {
		FCItem	fci;
		fci._strFileName = *pFileData;
		fci._fcb = fcbits;
		fci._op = FCItem::opFileChanged;
		fci._statNew = sbCur;
		pFileData->DoCallAlways ( &fci, _dwUser );	// Do normal callbacks
		if ( pFileData->CIgnores() == 0 ) {
			pFileData->DoCallIfNotIgnore ( &fci, _dwUser );
			}
		else if( pFileData->FSpecificIgnore() )
		{
			pFileData->DoCallIfNotSpecificIgnore ( &fci, _dwUser );
		}
		if ( fcbits.fDel ) {
			pFileData->Reinit();
			}
		}
	}

//-----------------------------------------------------------------------------
//  CFileChange::DoFileChange
//
//  Purpose:    handles the file change events from WaitAndHandleIt
//
//  Input:      iHandle,    index of the handle in the HtoStr array
//                          that fired the event
//
//  Note:       this iterates over the files in the dir that changed and
//				calls DoCheckFile on each one.
//
//-----------------------------------------------------------------------------
void    CFileChange::DoFileChange ( unsigned iHandle ) {
	HANDLE      h = _hts.rgh [ iHandle ];
	CString *   pstr = _hts.rgpstr [ iHandle ];

	ASSERT ( h );
	ASSERT ( pstr );
	ASSERT ( pstr->GetLength() );

	if ( ::FindNextChangeNotification ( h ) == NULL) {
		// The directory is no longer there (net connection lost?)
		ThreadDeleteDirectory ( iHandle );
		return;
		}

	CObject *               pObj;
	CSafePFileDataList *    pFileList;

	CritSection cs(_critSection);

	// get the list of files to go with the directory
	if ( _mpDirsFilelist.Lookup ( *pstr, pObj ) ) {
		ASSERT ( pObj );
		ASSERT ( pObj->IsKindOf(RUNTIME_CLASS(CPtrList)) );

		pFileList = (CSafePFileDataList *) pObj;
		ASSERT ( pFileList->_dwAux == iHandle );

		POSITION    pos = pFileList->GetHeadPosition();

		while ( pos ) {
			// check each file to see if it was the one that fired off the event
			CFileData * pFileData = (CFileData *) pFileList->GetNext ( pos );
			DoCheckFile ( pFileData );
 			}
		}
	else {
		ASSERT ( fFalse );  // this should never happen!
		}
	}

//-----------------------------------------------------------------------------
//  CFileChange::FThreadDelFile
//
//  Purpose:    delete a file from our list, while possibly removing an entire
//              entry in the directory map if the last file is removed from
//              that directory.
//
//  Input:      all input taken from fci.
//
//  Returns:    TRUE if file was removed, FALSE if file didn't exist.
//
//-----------------------------------------------------------------------------
BOOL    CFileChange::FThreadDelFile ( FCItem & fci ) {

	BOOL                    fRet = fFalse;
	CSafePFileDataList *    pFileList = NULL;
	CFileData *             pFileData = NULL;
	POSITION                posFile = NULL;

	CritSection	cs(_critSection);

	// find all the data associated with the file
	if ( FFilelistEtcFromFilename (
			fci._strFileName,
			pFileList,
			pFileData,
			posFile
			)
		) {
		ASSERT ( pFileData != NULL );
		ASSERT ( posFile != NULL );

		// if we have more than one callback, delete the one we match...
		pFileData->FDelCallBack ( fci._pfnFCCallBack );
		fRet = fTrue;
		if ( pFileData->CCallBacks() == 0 ) {
			if ( pFileList == NULL ) {
				// file came from overspill list...remove it if we match...
				DeleteOverspillFile ( pFileData );
				}
			else {
				// remove filedata entry in filelist and delete the filedata
				pFileList->RemoveAt ( posFile );
				delete pFileData;

				if ( pFileList->IsEmpty() ) {
					// remove the whole list and dir entry
					unsigned    i = pFileList->_dwAux;
					unsigned    iLast = _mpDirsFilelist.GetCount();

					ASSERT ( i > 0 );   // can't be 0, since that is our ITC handle
					ASSERT ( i <= iLast );
					ASSERT ( _hts.rgh[ i ] );
					ASSERT ( _hts.rgpstr[ i ] );

					// close the notification handle
					VERIFY ( ::FindCloseChangeNotification ( _hts.rgh[ i ] ) );

					// remove entry from map
					VERIFY ( _mpDirsFilelist.RemoveKey ( *_hts.rgpstr[ i ] ) );
					delete pFileList;

					// cache the soon-to-be-removed directory string
					CString	*	pstrPath = _hts.rgpstr[ i ];

					// move the arrays so that they are packed
					memmove ( &_hts.rgh[ i ],    &_hts.rgh[ i + 1 ],    (iLast - i) * sizeof(HANDLE) );
					memmove ( &_hts.rgpstr[ i ], &_hts.rgpstr[ i + 1 ], (iLast - i) * sizeof(CString *) );
					_hts.rgh[ iLast ] = 0;
					_hts.rgpstr[ iLast ] = NULL;

					// fix up all the _dwAux records in each file list
					POSITION    pos = _mpDirsFilelist.GetStartPosition();

					while ( pos ) {
						CObject *   pObj;
						_mpDirsFilelist.GetNextAssoc ( pos, *pstrPath, pObj );
						ASSERT ( pObj );
						ASSERT ( pObj->IsKindOf(RUNTIME_CLASS(CPtrList)) );

						pFileList = (CSafePFileDataList *) pObj;
						if ( pFileList->_dwAux > i ) {
							pFileList->_dwAux--;
							}
						}
					delete pstrPath;
					}
				}
			}
		}
	return fRet;
	}

//-----------------------------------------------------------------------------
//  CFileChange::ThreadCleanup
//
//  Purpose:    release all data and handles prior to destruction
//
//  Note:       should be done from aux thread, but in the case where the aux
//              thread is killed by the system due to a call to ExitProcess
//              AND this code resides in a dll, then the main thread will
//              note that the thread is dead and do the cleanup itself.
//
//-----------------------------------------------------------------------------
void    CFileChange::ThreadCleanup () {

	// we either think the thread is running or we know it is--this can be called
	// generally only by the 2nd thread, but can be used by the main thread if the 2nd
	// thread has been killed by the system (like during ExitProcess and we be called
	// by our dtor from the DllMain code.
	ASSERT ( _fThreadRunning );

	_critSection.Enter();
	unsigned    cHandles = _mpDirsFilelist.GetCount();

	// remove all items in dir list (does a deep remove, dtors all called!)
	_mpDirsFilelist.RemoveAllAssoc();

	// remove all file change handles and cached dir strings
	// don't close handle 0: handle 0 is the thread sync handle, not a 
	//  change notification handle
	while ( cHandles >= 1 ) {
		VERIFY ( ::FindCloseChangeNotification ( _hts.rgh[ cHandles ] ) );
		delete _hts.rgpstr[ cHandles ];
		cHandles--;
		}

	_fThreadRunning = fFalse;
	_critSection.Leave();
	}

//-----------------------------------------------------------------------------
//  CFileChange::FThreadIgnoreFile
//
//  Purpose:    to tell the file change handler to ignore or unignore a file.
//              typically used to handle known changes, like File.Save or
//              the ClassWizard.
//
//  Input:      op, an FCItem::Op that is either opIgnore or opUnignore
//              filename comes from fci.
//
//  Returns:    TRUE if successful
//
//  Note:       these are counting hits, so a ignore must be matched with an
//              unignore.
//
//-----------------------------------------------------------------------------
BOOL    CFileChange::FThreadIgnoreFile ( FCItem & fci ) {

	ASSERT ( fci._op == FCItem::opIgnore || fci._op == FCItem::opUnignore );

	BOOL                    fRet = fFalse;
	CSafePFileDataList *    pFileList;
	CFileData *             pFileData;
	POSITION                posDummy;

	CritSection	cs(_critSection);

	if ( FFilelistEtcFromFilename (
			fci._strFileName,
			pFileList,
			pFileData,
			posDummy
			)
		) {
		ASSERT ( pFileData != NULL );
		ASSERT ( posDummy != NULL );
		if ( fci._op == FCItem::opIgnore ) {
			pFileData->FIgnore(fci._pfnFCCallBack);
			}
		else {
			// We are about to unignore the file completely
			// so notify anyone who didn't ignore this file
			// that it has probably changed
			if ( pFileData->CIgnores() == 1 ) {
				DoCheckFile ( pFileData );
				}
			pFileData->FUnignore(fci._pfnFCCallBack);
 			}
		fRet = fTrue;
		}

	return fRet;
	}

//-----------------------------------------------------------------------------
//  CFileChange::FThreadSyncFile
//
//  Purpose:    to sync up the file data we have cached with the file system.
//              typically used after a ignore/unignore pair to make sure
//              file change events don't happen after the unignore.
//
//  Returns:    TRUE if successful
//
//-----------------------------------------------------------------------------
BOOL    CFileChange::FThreadSyncFile ( FCItem & fci ) {

	BOOL                    fRet = fFalse;
	CSafePFileDataList *    pFileList;
	CFileData *             pFileData;
	POSITION                posDummy;

	CritSection	cs(_critSection);

	if ( FFilelistEtcFromFilename (
			fci._strFileName,
			pFileList,
			pFileData,
			posDummy
			)
		) {
		ASSERT ( pFileData != NULL );
		ASSERT ( posDummy != NULL );

		fRet = pFileData->FSetCachedStatBuf();
		}
	return fRet;
	}

//-----------------------------------------------------------------------------
//  CFileChange::ThreadDeleteDirectory
//
//  Purpose:    Remove all the files from a directory entry, and the directory
//              entry itself.
//
//  Input:      iHandle,    index of the handle in the HtoStr of the directory
//
//  Note:       This function handles a failed FindNextFileChangeNotification
//
//-----------------------------------------------------------------------------
void    CFileChange::ThreadDeleteDirectory ( unsigned iHandle ) {
	HANDLE      h = _hts.rgh [ iHandle ];
	CString *   pstr = _hts.rgpstr [ iHandle ];

	ASSERT ( h );
	ASSERT ( pstr );
	ASSERT ( pstr->GetLength() );

	CObject *               pObj;
	CSafePFileDataList *    pFileList;

	CritSection	cs(_critSection);

	// get the list of files to go with the directory
	if ( _mpDirsFilelist.Lookup ( *pstr, pObj ) ) {
		ASSERT ( pObj );
		ASSERT ( pObj->IsKindOf(RUNTIME_CLASS(CPtrList)) );

		pFileList = (CSafePFileDataList *) pObj;
		ASSERT ( pFileList->_dwAux == iHandle );

		FCItem		fci;
		POSITION    pos = pFileList->GetHeadPosition();
		POSITION    posLast;
		
		fci._fcb.fDirDel = fTrue;
		fci._fcb.fDel = fTrue;
		fci._op = FCItem::opFileChanged;

		while ( pos ) {
			// remove each file from the list and tell each client
			posLast = pos;
			CFileData * pFileData = (CFileData *) pFileList->GetNext ( pos );
			pFileList->RemoveAt ( posLast );
			fci._strFileName = *pFileData;
			pFileData->DoCallAlways ( &fci, _dwUser );
			pFileData->DoCallIfNotIgnore ( &fci, _dwUser );
			delete pFileData;
			}
		}

	// remove the whole list and dir entry
	unsigned    i = pFileList->_dwAux;
	unsigned    iLast = _mpDirsFilelist.GetCount();

	ASSERT ( i > 0 );   // can't be 0, since that is our ITC handle
	ASSERT ( i <= iLast );
	ASSERT ( _hts.rgh[ i ] );
	ASSERT ( _hts.rgpstr[ i ] );

	// remove entry from map
	VERIFY ( _mpDirsFilelist.RemoveKey ( *_hts.rgpstr[ i ] ) );
	delete pFileList;

	// cache the soon-to-be-removed directory string
	CString	*	pstrPath = _hts.rgpstr[ i ];

	// move the arrays so that they are packed
	memmove ( &_hts.rgh[ i ],    &_hts.rgh[ i + 1 ],    (iLast - i) * sizeof(HANDLE) );
	memmove ( &_hts.rgpstr[ i ], &_hts.rgpstr[ i + 1 ], (iLast - i) * sizeof(CString *) );
	_hts.rgh[ iLast ] = 0;
	_hts.rgpstr[ iLast ] = NULL;

	// close the change notification handle
	VERIFY ( ::FindCloseChangeNotification ( h ) );

	// fix up all the _dwAux records in each file list
	POSITION    pos = _mpDirsFilelist.GetStartPosition();

	while ( pos ) {
		CObject *   pObj;
		_mpDirsFilelist.GetNextAssoc ( pos, *pstrPath, pObj );
		ASSERT ( pObj );
		ASSERT ( pObj->IsKindOf(RUNTIME_CLASS(CPtrList)) );

		pFileList = (CSafePFileDataList *) pObj;
		if ( pFileList->_dwAux > i ) {
			pFileList->_dwAux--;
			}
		}
	delete pstrPath;
	}

//-----------------------------------------------------------------------------
//  CFileChange::PFileDataOverspill
//
//  Purpose:    find if a filedata object exists in our overspill list and
//				if so, return a pointer to it.
//
//  Input:      szFileName, name of file!
//
//-----------------------------------------------------------------------------
CFileData *
CFileChange::PFileDataOverspill ( LPCTSTR szFileName, POSITION & posCur ) {
	POSITION	pos = _listPFileDataEx.GetHeadPosition();
	while ( posCur = pos ) {
		CFileData *	pfile = (CFileData *) _listPFileDataEx.GetNext ( pos );
		if ( pfile && 0 == pfile->CompareNoCase ( szFileName ) ) {
			return pfile;
			}
		}
	return NULL;
	}

//-----------------------------------------------------------------------------
//  CFileChange::FAddOverspillFile
//
//  Purpose:    Adds a filedata object into the overspill list.  If already in
//				the list, returns fFalse so caller can take appropriate action.
//				The callback is added into an existing one.
//
//  Input:      pFiledata
//
//-----------------------------------------------------------------------------
BOOL
CFileChange::FAddOverspillFile ( CFileData * pFiledata ) {
	debug(::OutputDebugString ( "Using manual overspill area for \"" ));
	debug(::OutputDebugString ( LPCTSTR(*pFiledata) ));
	debug(::OutputDebugString ( "\" change notifications.\n" ));

	POSITION	pos;
	CFileData *	pfile = PFileDataOverspill ( *pFiledata, pos );

	if ( pfile ) {
		pfile->MergeFileData ( pFiledata );
		return fFalse;
		}
	else {
		_listPFileDataEx.AddTail ( pFiledata );
		return fTrue;
		}
	}

//-----------------------------------------------------------------------------
//  CFileChange::DeleteOverspillFile
//
//  Purpose:    Removes a filedata object from the overspill list.
//
//  Input:      pFiledata
//
//-----------------------------------------------------------------------------
void
CFileChange::DeleteOverspillFile ( CFileData * pFiledata ) {
	POSITION	pos = _listPFileDataEx.Find ( pFiledata );
	if ( pos ) {
		_listPFileDataEx.RemoveAt ( pos );
		}
	delete pFiledata;
	}


//-----------------------------------------------------------------------------
//  CFileChange::DoCheckOverspillFiles
//
//  Purpose:    Iterate over the overspill files and check for changes...
//
//-----------------------------------------------------------------------------
void
CFileChange::DoCheckOverspillFiles() {
	POSITION	pos = _listPFileDataEx.GetHeadPosition();
	while ( pos ) {
		CFileData * pFile = (CFileData *) _listPFileDataEx.GetNext ( pos );
		DoCheckFile ( pFile );
		}
	}

//-----------------------------------------------------------------------------
//  CFileChange::FAddFileAsync
//
//  Purpose:    Add a file asynchronously to the file change list,
//				returns true if it is able to queue it up successfully.
//
//-----------------------------------------------------------------------------
BOOL
CFileChange::FAddFileAsync (
	LPCTSTR			szFile,
	PfnFCCallBack	pfnFCCallBack,
	PfnAsyncReturn	pfnAsyncReturn,
	PV				pvUser1 /* = NULL */,
	PV				pvUser2 /* = NULL */,
	BOOL			fCallAlways /*= fFalse */
	) {

	FCItem	fci;

	ASSERT ( szFile );
	ASSERT( 0 < _tcslen(szFile) );
	ASSERT ( pfnFCCallBack );

	fci._strFileName = szFile;
	fci._op = FCItem::opAdd;
	fci._fCallAlways = fCallAlways;
	fci._pfnFCCallBack = pfnFCCallBack;
	fci._pfnAsyncReturn = pfnAsyncReturn;
	fci._pvUser1 = pvUser1;
	fci._pvUser2 = pvUser2;

	IDEL	idelDummy;
	BOOL	fRet = _queueFCItem.put ( fci, idelDummy );
	_eventIn.Set();
	return fRet;
	}

//-----------------------------------------------------------------------------
//  CFileChange::FDelFileAsync
//
//  Purpose:    Delete a file asynchronously to the file change list,
//				returns true if it is able to queue it up successfully.
//
//-----------------------------------------------------------------------------
BOOL
CFileChange::FDelFileAsync (
	LPCTSTR			szFile,
	PfnFCCallBack	pfnFCCallBack,
	PfnAsyncReturn	pfnAsyncReturn,
	PV				pvUser1 /* = NULL */,
	PV				pvUser2 /* = NULL */
	) {

	FCItem	fci;

	ASSERT ( szFile );
	ASSERT ( pfnFCCallBack );

	fci._strFileName = szFile;
	fci._op = FCItem::opDel;
	fci._pfnFCCallBack = pfnFCCallBack;
	fci._pfnAsyncReturn = pfnAsyncReturn;
	fci._pvUser1 = pvUser1;
	fci._pvUser2 = pvUser2;

	IDEL	idelDummy;
	BOOL	fRet = _queueFCItem.put ( fci, idelDummy );
	_eventIn.Set();
	return fRet;
	}

//-----------------------------------------------------------------------------
//  CFileChange::SetIdleMode
//
//  Purpose:    to tell the 2nd thread to raise or lower its priority,
//				generally in response to doing a build so the thread doesn't
//				compete so much with the background processes.
//
//-----------------------------------------------------------------------------
unsigned
CFileChange::SetIdleMode ( BOOL fIdle ) {

	CritSection	cs(_critsecPrty);
	unsigned	cIdlesPrev = _cIdles;

	if ( fIdle ) {
		_cIdles++;
		}
	else {
		_cIdles--;
		}
	
	if ( !cIdlesPrev && _cIdles ) {
		// signal 2nd thread to get it to go into its idle mode, just by doing
		// an FCItem::opNull operation.
		IDEL	idelDummy;
		FCItem	fci;
		
		_queueFCItem.put ( fci, idelDummy );
		_eventIn.Set();

		VERIFY ( ::SetThreadPriority ( _hThread, prtyIdle ) );
		}
	else if ( cIdlesPrev && !_cIdles ) {
		// give a boost to clear stuff out
		BoostPriority();

		// signal 2nd thread to get it to wake up, just by doing an
		// FCItem::opNull operation.
		IDEL	idelDummy;
		FCItem	fci;
		
		_queueFCItem.put ( fci, idelDummy );
		_eventIn.Set();
		::Sleep ( 0 );

		// put us back to normal.
		RestorePriority();
		}
	
	return cIdlesPrev;
	}

//-----------------------------------------------------------------------------
//  CFileChange::BoostPriority
//
//  Purpose:    boost the priority for quick response to clients
//
//-----------------------------------------------------------------------------
void
CFileChange::BoostPriority() {

	CritSection	cs(_critsecPrty);
	
	if ( 0 == _cBoosts++ ) {
		VERIFY ( ::SetThreadPriority ( _hThread, prtyAboveNormal ) );
		}
	}

//-----------------------------------------------------------------------------
//  CFileChange::RestorePriority
//
//  Purpose:    restore the priority to either our normal or idle state
//
//-----------------------------------------------------------------------------
void
CFileChange::RestorePriority() {

	CritSection	cs(_critsecPrty);
		
	if ( 0 == --_cBoosts ) {
		if ( _cIdles ) {
			VERIFY ( ::SetThreadPriority ( _hThread, prtyStandard ) );
			}
		else {
			VERIFY ( ::SetThreadPriority ( _hThread, prtyStandard ) );
			}
		}
	}

//-----------------------------------------------------------------------------
//  FFileInfo
//
//  Purpose:    replace _stat with one that does it via Win32 apis
//
//-----------------------------------------------------------------------------
BOOL
StatBuf::FGetFileInfo ( LPCTSTR szFile ) {
	HANDLE				hfind;
	BOOL				fRet = fFalse;	
	WIN32_FILE_ATTRIBUTE_DATA find;

	fcft.qwFiletime = 0;
	cbFile = 0;
	dwAttrs = 0;

	if (Compatible_GetFileAttributesEx( szFile, GetFileExInfoStandard, &find))
	{
		fcft.filetime = find.ftLastWriteTime;
		cbFile = find.nFileSizeLow + QWORD(find.nFileSizeHigh) * 0x100000000i64;
		dwAttrs = find.dwFileAttributes;
		if ( 0 == dwAttrs ) {
			// HACK: Win95 doesn't set the normal bit if
			//	no other attributes are set.
			dwAttrs = fattrNormal;
			}
		fRet = fTrue;
	}
	return fRet;
	}
