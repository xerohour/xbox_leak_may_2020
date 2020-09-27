//	REVIEW(DavidGa):  This needs to be drastically simplified and moved
//                    into shlsrvc.h.
//-----------------------------------------------------------------------------
//  filechng.h
//
//  Copyright (C) 1993, Microsoft Corporation
//
//  Purpose:    define the various supporting classes and the class CFileChange
//              for use in watching when files are touched or otherwise
//              modified.
//
//  Revision History:
//
//  []      14-Jan-1994 Dans    Created
//
//-----------------------------------------------------------------------------
#if !defined(_filechng_h) /* { */
#define _filechng_h

#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>

typedef unsigned __int64	QWORD;
const unsigned				cIdleMsecs = 5000;
const unsigned				cIdleMsecsCumulative = 120 * 1000;

union FCFT {	// FileChange FileTime
	FILETIME	filetime;
	QWORD		qwFiletime;
	};

struct StatBuf {
	enum {	// file attributes, mimic long names from Win32 headers.
		fattrDir = FILE_ATTRIBUTE_DIRECTORY,
		fattrNormal = FILE_ATTRIBUTE_NORMAL,
		fattrReadOnly = FILE_ATTRIBUTE_READONLY,
		fattrSystem = FILE_ATTRIBUTE_SYSTEM,
		fattrHidden = FILE_ATTRIBUTE_HIDDEN,
		fattrArchive = FILE_ATTRIBUTE_ARCHIVE
		};

	void
	clear() {
		fcft.qwFiletime = cbFile = dwAttrs = 0;
		}

	StatBuf() {
		clear();
		}

	BOOL
	FGetFileInfo ( LPCTSTR szFile );

	FCFT	fcft;
	QWORD	cbFile;
	DWORD	dwAttrs;
	};

inline DWORD
FattrFileMask() {
	return 
		StatBuf::fattrNormal |
		StatBuf::fattrReadOnly |
		StatBuf::fattrSystem |
		StatBuf::fattrHidden |
		StatBuf::fattrArchive;

	}

// depends upon windows.h, afx.h, and afxcoll.h to be included

#if !defined (fTrue)
	#define fTrue   TRUE
#endif
#if !defined (fFalse)
	#define fFalse  FALSE
#endif

inline DWORD DwStdFileChange() {
	return
		FILE_NOTIFY_CHANGE_FILE_NAME |
		FILE_NOTIFY_CHANGE_ATTRIBUTES |
		FILE_NOTIFY_CHANGE_SIZE |
		FILE_NOTIFY_CHANGE_LAST_WRITE
		;
	}

union FCBits {  // File Change Bits, what really changed
	UINT        fAll;
	struct {
		UINT    fAttrs:1;
		UINT    fTime:1;
		UINT    fSize:1;
		UINT    fDel:1;
		UINT    fType:1;
		UINT	fDirDel:1;
		};
	};

//
// Callback function, called from secondary thread, cannot do SendMessage, must
//  use PostMessage or another form of IPC.
//
class FCItem;
typedef void (*PfnFCCallBack)( const FCItem *, DWORD dwUser );  // call back function
typedef void (*PfnAsyncReturn)( const FCItem * );
typedef void *		PV;

class FCItem {  // File Change Item
public:
	enum Op {   // File Change Operation
		opNull,
		opAdd,          // in, add file
		opDel,          // in, delete file
		opIgnore,       // in, ignore file (counting!)
		opUnignore,     // in, unignore file (counting!)
		opSyncFile,     // in, force file data to be synced to file system
		opQuit,         // in, tell thread to quit
		opFileChanged   // out, a file changed
		};

	CString     _strFileName;       // name of affected file
	Op          _op;                // what this all means
	BOOL        _fSilent;           // whether to update silently or not (changed by us!)
	StatBuf     _statNew;           // keeps the new info around
	FCBits      _fcb;
	PfnFCCallBack
				_pfnFCCallBack;		// The callback function
	BOOL		_fCallAlways;       // whether this callback function wants calling even when we
									// are ignoring the file.

	BOOL		_fAsyncReturn;		// async return code
	PfnAsyncReturn
				_pfnAsyncReturn;	// async callback with result
	PV			_pvUser1;			// async user info
	PV			_pvUser2;			// async user info

				FCItem() {
					_op = opNull;
					_fSilent = fFalse;
					_fcb.fAll = fFalse;
					_pfnFCCallBack = 0;
					_fCallAlways = fFalse;
					_fAsyncReturn = fFalse;
					_pfnAsyncReturn = 0;
					_pvUser1 = _pvUser2 = 0;
					}
	};

#pragma warning(disable:4097)
class   CSection : public CRITICAL_SECTION {
public:
	CSection() {
		::InitializeCriticalSection ( this );
		}
	~CSection() {
		::DeleteCriticalSection ( this );
		}
	void Enter() {
		::EnterCriticalSection ( this );
		}
	void Leave() {
		::LeaveCriticalSection ( this );
		}
	};
#pragma warning(default:4097)

// CTestableSection
//
// This is like a CSection but it supports an extra method FOwned(), which tests
// whether or not the current thread has entered (and not left) the critical section.
//
// This is a debugging facility only, so calls to FOwned() must always be in an
// ASSERT or equivalent.  In the ship version, a CTestableSection is the same as a
// CSection.
#ifndef _DEBUG

class CTestableSection : public CSection
{
};

#else	// _DEBUG

class   CTestableSection
{
private:
	CSection m_csBase;			// the "real" c.s. we are wrapping
	LONG m_cLocks;				// conceptually "inside" m_csBase

	DWORD m_idThread;			// thread identifier outside the base CSection
	CSection m_csIdThread;		// guards access to m_idThread
public:
	CTestableSection()
	{
		m_cLocks = 0;
		m_idThread = 0;
	}
	void Enter()
	{
		m_csBase.Enter();
		if (m_cLocks++ == 0)
		{
			// initialize m_idThread
			DWORD idThread = GetCurrentThreadId();

			m_csIdThread.Enter();
			m_idThread = idThread;
			m_csIdThread.Leave();
		}
	}
	void Leave() {
		ASSERT(m_cLocks > 0);	// underflow test
		if (--m_cLocks == 0)
		{
			// clear m_idThread
			m_csIdThread.Enter();
			m_idThread = 0;
			m_csIdThread.Leave();
		}
		m_csBase.Leave();
	}
	BOOL FOwned()
	{
		DWORD idThread = GetCurrentThreadId();

		m_csIdThread.Enter();
		BOOL fOwned = idThread == m_idThread;
		m_csIdThread.Leave();
		
		return fOwned;
	}
};

#endif	// _DEBUG

// ease-of-use of a critical section...
class CritSection {
	CSection *	pcs;
public:
	CritSection ( CSection & cs ) {
		cs.Enter();
		pcs = &cs;
		}
	~CritSection() {
		pcs->Leave();
		}
	};

// same as CritSection but for the "testable" variety
class CritSectionT {
	CTestableSection *	pcs;
public:
	CritSectionT ( CTestableSection & cs ) {
		cs.Enter();
		pcs = &cs;
		}
	~CritSectionT() {
		pcs->Leave();
		}
	};


typedef SECURITY_ATTRIBUTES     SA;
typedef SA *                    PSA;

class CW32Event {
	HANDLE  _hEvent;
public:
	CW32Event (
		BOOL    fInitialState = FALSE,  // not signalled
		BOOL    fManualReset = FALSE,   // auto reset on
		LPCTSTR szName = NULL,          // no name
		PSA     psa = NULL ) {          // std security

		_hEvent = CreateEvent ( psa, fManualReset, fInitialState, szName );
		}

	~CW32Event () {
		::CloseHandle ( _hEvent );
		}
	operator HANDLE() {
		return _hEvent;
		}

	BOOL    Set() {
		return ::SetEvent ( _hEvent );
		}
	BOOL    Pulse() {
		return ::PulseEvent ( _hEvent );
		}
	BOOL    Reset() {
		return ::ResetEvent ( _hEvent );
		}

	DWORD   Wait ( DWORD dwWait = INFINITE) {
		return ::WaitForSingleObject ( _hEvent, dwWait );
		}
	};

class CSafeMapStringToOb : public CMapStringToOb {  // &*#$% safe map, dtors called!
public:
	void RemoveAllAssoc() {
		POSITION    pos = GetStartPosition();
		CObject *   pObj;
		CString     str;

		while ( pos ) {
			GetNextAssoc ( pos, str, pObj );
			RemoveKey ( str );
			delete pObj;
			}
		}

	~CSafeMapStringToOb() {
		RemoveAllAssoc();
		}

	};

class CFileData : public CString {      // keep extra info about a file
	StatBuf     	_statBuf;
	unsigned        _cIgnore;
	BOOL    	    _fValid;
	CPtrList        _listPfnAlwaysCall;		// List of callbacks that are always called.
	CPtrList        _listPfnCallNotIgnore;	// List of callbacks only called if file changes
											// are not being ignored
	PfnFCCallBack _pfnSpecificIgnore;		// major HACK to tide us over for now (davidga)

public:
				CFileData ( const char * szFile ) : CString ( szFile ) {
					_cIgnore = 0;
					_pfnSpecificIgnore = NULL;
					FSetCachedStatBuf();
					}

	BOOL        FValid() { return _fValid; }

	void        Reinit() {
					_statBuf.clear();
					_fValid = FALSE;
					}

				CFileData() {
					_cIgnore = 0;
					Reinit();
					}

	BOOL        FGetCachedStatBuf ( StatBuf & statbuf ) {
					statbuf = _statBuf;
					return FValid();
					}

	BOOL        FSetCachedStatBuf () {
					_fValid = _statBuf.FGetFileInfo ( *this );
					return FValid();
					}

	BOOL        FSetCachedStatBuf ( StatBuf & statBuf) {
					_fValid = _statBuf.FGetFileInfo ( *this );
					statBuf = _statBuf;
					return FValid();
					}

	BOOL        FIgnore ( PfnFCCallBack pfn = NULL ) {
					ASSERT ( _cIgnore + 1 );  // must not wrap
					if( (_pfnSpecificIgnore != NULL) && (_pfnSpecificIgnore != pfn) )
					{
						ASSERT( pfn == NULL );		// can't specifically ignore two callbacks
						_pfnSpecificIgnore = NULL;
					}
					else
						_pfnSpecificIgnore = pfn;
					return (_cIgnore = max ( _cIgnore, _cIgnore + 1 ) );
					}

	BOOL        FUnignore( PfnFCCallBack pfn = NULL ) {
					ASSERT ( _cIgnore );      // must not wrap
					ASSERT( (pfn == NULL) || (pfn == _pfnSpecificIgnore) );
					if( (_cIgnore = min ( _cIgnore, _cIgnore - 1 )) == 0 )
						_pfnSpecificIgnore = NULL;
					return _cIgnore;
					}
	unsigned    CIgnores() {
					return _cIgnore;
					}

	BOOL		FSpecificIgnore() {
					// should not have specific ignore if cIgnore is 0
					ASSERT( (_cIgnore != 0) || (_pfnSpecificIgnore == NULL) );
					return _pfnSpecificIgnore != NULL;
					}

	void        DoCallAlways ( const FCItem * fc, DWORD dwUser ) {
					POSITION pos = _listPfnAlwaysCall.GetHeadPosition();
					while ( pos != NULL ) {
						PfnFCCallBack pfn = PfnFCCallBack(_listPfnAlwaysCall.GetNext(pos));
						(pfn)(fc, dwUser);
						}
					}

	void        DoCallIfNotIgnore ( const FCItem* fc, DWORD dwUser ) {
					POSITION pos = _listPfnCallNotIgnore.GetHeadPosition();
					while ( pos != NULL ) {
						PfnFCCallBack pfn = PfnFCCallBack(_listPfnCallNotIgnore.GetNext(pos));
						(pfn)(fc, dwUser);
						}
					}

	void        DoCallIfNotSpecificIgnore ( const FCItem* fc, DWORD dwUser ) {
					POSITION pos = _listPfnCallNotIgnore.GetHeadPosition();
					while ( pos != NULL ) {
						PfnFCCallBack pfn = PfnFCCallBack(_listPfnCallNotIgnore.GetNext(pos));
						if( pfn == _pfnSpecificIgnore )
							continue;
						(pfn)(fc, dwUser);
						}
					}

	void		MergeFileData ( CFileData * pfile ) {
					POSITION	pos = pfile->_listPfnAlwaysCall.GetHeadPosition();
					while ( pos ) {
						PfnFCCallBack pfn = PfnFCCallBack(pfile->_listPfnAlwaysCall.GetNext ( pos ) );
						if ( !_listPfnAlwaysCall.Find ( pfn ) ) {
							_listPfnAlwaysCall.AddTail ( pfn );
							}
						}

					pos = pfile->_listPfnCallNotIgnore.GetHeadPosition();
					while ( pos ) {
						PfnFCCallBack pfn = PfnFCCallBack(pfile->_listPfnCallNotIgnore.GetNext ( pos ) );
						if ( !_listPfnCallNotIgnore.Find ( pfn ) ) {
							_listPfnCallNotIgnore.AddTail ( pfn );
							}
						}
					}
					

	BOOL        FAddCallBack ( PfnFCCallBack pfn, BOOL fCallAlways = fFalse ) {
					CPtrList *	pList;

					if ( fCallAlways )
						pList = &_listPfnAlwaysCall;
					else
						pList = &_listPfnCallNotIgnore;

					if ( !pList->Find ( pfn ) ) {
						pList->AddTail ( pfn );
						return fTrue;
						}
					return fFalse;
					}

	BOOL        FDelCallBack ( PfnFCCallBack pfn ) {
					POSITION pos = _listPfnAlwaysCall.Find ( pfn );
					if ( pos ) {
						_listPfnAlwaysCall.RemoveAt ( pos );
						return fTrue;
						}
					pos = _listPfnCallNotIgnore.Find ( pfn );
					if ( pos ) {
						_listPfnCallNotIgnore.RemoveAt ( pos );
						return fTrue;
						}
					return fFalse;
					}

	UINT		CCallBacks() {
					return _listPfnAlwaysCall.GetCount() + _listPfnCallNotIgnore.GetCount();
					}

	BOOL        FLastCallBack() {
					return 1 == CCallBacks();
					}
	};

class CSafePFileDataList : public CPtrList {    // *&)*(*% safe list, dtors called!
public:
	DWORD   _dwAux;                 // some extra info (currently the index in the HtoStr)

	CSafePFileDataList() {
		_dwAux = 0;
		}
	~CSafePFileDataList() {
		while ( !IsEmpty() ) {
			CFileData * pstr = (CFileData *) GetHead();
			delete pstr;
			RemoveHead();
			}
		}
	};

//
// message to send has a FCItem * in lparam
//
#define WU_FILECHANGE   (WM_USER + 0x2112)

#define iHandleITC  0       // index of event that is our inter-thread comm event.
#define iHandleMax  MAXIMUM_WAIT_OBJECTS     // allows us watch iHandleMax - 1 directories
#define cDirMax     (iHandleMax - 1)

struct HtoStr {
	HANDLE      rgh[ iHandleMax ];
	CString *   rgpstr[ iHandleMax ];
	};

//
// templated queue for asynchronous calls
//
typedef unsigned	IDEL;

template <class T, class ARG_T>
class CQueue {

	CSection	_critSec;
	unsigned	_ielFront;
	unsigned	_ielBack;
	unsigned	_celAlloc;
	unsigned	_celQueued;
	IDEL		_idEl;
	T *			_rgel;

protected:
	
	unsigned
	celGrowTo() {
		// grow by 50% (multiply by 1.5)
		return _celAlloc + _celAlloc / 2 + 1;
		}
			
	BOOL
	grow();

	void
	advance ( unsigned & iel ) {
		iel = (iel + 1) % _celAlloc;
		}

	void
	init ( unsigned cInitial );
	

public:
	enum { cInitialDefault = 64 };

	CQueue() {
		init ( cInitialDefault );
		}
	CQueue ( unsigned cInitial ) {
		init ( cInitial ? cInitial : cInitialDefault );
		}
	~CQueue() {
		if ( _rgel ) {
			delete [] _rgel;
			}
		_rgel = 0;
		_celAlloc = _celQueued = _ielFront = _ielBack = 0;
		}

	BOOL
	get ( T &, IDEL & );

	BOOL
	put ( ARG_T, IDEL & );

	unsigned
	size() {
		CritSection	cs(_critSec);
		return _celQueued;
		}
	};



template <class T, class ARG_T>
BOOL CQueue<T, ARG_T>::get ( T & el, IDEL & idel ) {
	CritSection	cs(_critSec);
	BOOL		fRet = fFalse;

	if ( _celQueued ) {
		fRet = fTrue;
		el = _rgel[ _ielBack ];
		advance ( _ielBack );
		idel = _idEl - _celQueued;
		_celQueued--;
		}
	return fRet;
	}

template <class T, class ARG_T>
BOOL CQueue<T, ARG_T>::put ( ARG_T el, IDEL & idel ) {
	CritSection	cs(_critSec);
	BOOL		fRet = fFalse;

	if ( grow() ) {
		fRet = fTrue;
		_rgel[ _ielFront ] = el;
		advance ( _ielFront );
		_celQueued++;
		idel = ++_idEl;
		}
	return fRet;
	}

template <class T, class ARG_T>
BOOL CQueue<T, ARG_T>::grow() {
	// note that this routine does NOT gain the crit sec.  it relies
	// upon the caller to do so.
	BOOL	fRet = _celQueued < _celAlloc;

	if ( !fRet ) {
		// grow the array and remap the indices
		unsigned	celNew = celGrowTo();
		T *			rgelNew = new T[ celNew ];

		if ( rgelNew ) {
			fRet = fTrue;
			// copy over elements
			unsigned	iel;
			unsigned	ielNew = 0;
			unsigned	cel;
			for (
				iel = _ielBack, cel = _celQueued;
				cel != 0;
				advance(iel), cel--
				) {
				rgelNew[ ielNew++ ] = _rgel[ iel ];
				}
			delete [] _rgel;
			_rgel = rgelNew;
			_celAlloc = celNew;
			_ielBack = 0;
			_ielFront = _celQueued;
			}
		}
	return fRet;
	}

template <class T, class ARG_T>
void CQueue<T, ARG_T>::init ( unsigned cel ) {
	_celAlloc = _celQueued = _ielFront = _ielBack = _idEl = 0;
	_rgel = new T[ cel ];
	if ( _rgel ) {
		_celAlloc = cel;
		}
	}


//
// main work horse of the file change system
//

class CFileChange {

public:
	CFileChange();
	~CFileChange();
	
	BOOL
	FInit ( DWORD dwUser = 0 );

	void
	Terminate();                                        // close things down (still
														//  can be re-initialized)
	// add a file to be watched
	BOOL
	FAddFile ( LPCTSTR szFile, PfnFCCallBack pfn, BOOL fCallAlways = fFalse );
	
	// remove a file from the list
	BOOL
	FDelFile ( LPCTSTR szFile, PfnFCCallBack pfn );
	
	// sync data to file system
	BOOL
	FSyncFile ( LPCTSTR szFile );
	
	// ignore changes to this file
	BOOL FIgnoreFile ( LPCTSTR szFile, BOOL fIgnore = fTrue, 
		PfnFCCallBack pfn = NULL );

	// async add file, returns true if queued successfully
	BOOL
	FAddFileAsync (
		LPCTSTR	szFile,
		PfnFCCallBack,
		PfnAsyncReturn,
		PV pvUser1 = NULL,
		PV pvUser2 = NULL,
		BOOL fCallAlways = fFalse
		);

	// async del file, returns true if queued successfully
	BOOL
	FDelFileAsync (
		LPCTSTR szFile,
		PfnFCCallBack,
		PfnAsyncReturn,
		PV pvUser1 = NULL,
		PV pvUser2 = NULL
		);

	// set the priority of the thread to normal or idle, returns
	// the count of idle requests
	unsigned
	SetIdleMode ( BOOL fIdle );

protected:
	// 2nd thread entry point, always pass in a CFileChange*
	static UINT
	ThreadEntry ( void * );

	// our call back for making the synchronous calls out of an async api
	static void
	SyncCallback ( const FCItem * );

	// where the 2nd thread spends most of its time
	void
	WaitAndHandleIt();

	// handle the requests in 2nd thread
	BOOL
	FThreadDoRequest();
	
	// handle add file in 2nd thread
	BOOL
	FThreadAddFile ( FCItem & );

	// handle del file in 2nd thread
	BOOL
	FThreadDelFile ( FCItem & );
	
	// handle ignore file in 2nd thread
	BOOL
	FThreadIgnoreFile ( FCItem & );
	
	// handle sync file in 2nd thread
	BOOL
	FThreadSyncFile ( FCItem & );
	
	// check a file for a change and send notification
	void
	DoCheckFile ( CFileData * pFileData );
	
	// handle the file change event in 2nd thread
	void
	DoFileChange ( unsigned iHandle );
	
	// delete a whole directory entry
	void
	ThreadDeleteDirectory ( unsigned iHandle );

	void
	ThreadCleanup();

	// used by 2nd thread to return vals
	void
	ThreadReturn ( BOOL f ) {
		_fThreadRetCode = f;
		_eventOut.Set();
		}

	// used by 1st thread to call 2nd thread to do something
	BOOL
	ThreadCall ( LPCTSTR szFile, PfnFCCallBack pfn, FCItem::Op op, BOOL fCallAlways = TRUE );

	// utility function to search for filelists and filedata
	//  returns partial info
	BOOL
	FFilelistEtcFromFilename (
		LPCTSTR                 szFilename,
		CSafePFileDataList * &  pFilelist,
		CFileData * &           pFiledata,
		POSITION &              pos
		);

	// function to tell us how much time to wait in WaitForMultipleObjects...
	DWORD
	DwTimeout() {
		if ( !_cIdles && _listPFileDataEx.IsEmpty() ) {
			return INFINITE;
			}
		else {
			// we timeout every 5 seconds for the manual detect
			// cases on the overflow files or while idling.
			return cIdleMsecs;
			}
		}

	// check all the files in the overspill list
	void
	DoCheckOverspillFiles();

	// add file to overspill list returns fFalse if file already in list
	BOOL
	FAddOverspillFile ( CFileData * );

	// check to see if a file is already in the list, returning the CFileData *
	CFileData *
	PFileDataOverspill ( LPCTSTR szFile, POSITION & pos );

	// delete a file from the overspill list
	void
	DeleteOverspillFile ( CFileData * );

	void
	BoostPriority();

	void
	RestorePriority();

private:
	DWORD               _fThreadRunning;    // used as a flag, but contains thread id
	HANDLE              _hThread;           // handle to the thread we create.
	FCItem				_fcitemThread;		// for our synchronous calls
	CW32Event           _eventIn;           // our inter-thread event
	CW32Event           _eventOut;          // our inter-thread event for sync calls
	BOOL                _fThreadRetCode;    // thread return code
	CSection            _critSection;       // critical section for accessing lists
	CSafeMapStringToOb  _mpDirsFilelist;    // map the directories to file lists
	DWORD               _dwUser;
	HtoStr              _hts;               // array of handles to events to wait on
	CSafePFileDataList	_listPFileDataEx;	// handle the overspill when/if we run out
											// of directory handles that we can watch
											// (currently WaitForMultipleObjects has
											// the limit of 64) or on Chicago where
											// ABSOLUTELY NONE of the file change
											// notifications work.

	CSection			_critsecCall;		// used to serialize calls to the filechange thread
	CQueue<FCItem, FCItem&>
						_queueFCItem;		// queue for async calls
	unsigned			_cIdles;			// count of idle requests
	unsigned			_cBoosts;			// count of priority boosts
	CSection			_critsecPrty;		// serialize access to the idle/priority setting
	BOOL				_fDupThreadHandle;	// Do we have a duplicated thread handle.

	enum {
		prtyIdle = THREAD_PRIORITY_IDLE,
		prtyLowest = THREAD_PRIORITY_LOWEST,
		prtyBelowNormal = THREAD_PRIORITY_BELOW_NORMAL,
		prtyNormal = THREAD_PRIORITY_NORMAL,
		prtyAboveNormal = THREAD_PRIORITY_ABOVE_NORMAL,
		prtyStandard = prtyBelowNormal		// priority we normally run at, one tick below main thread
		};
	};

CFileChange * GetFileChange();

// CIgnoreFile...used as an object to automatically ignore changes to
// a file while doing something with it.  Automatically unignores
// when object is destroyed.
//
class CIgnoreFile {
	CFileChange *	_pfc;
	_TCHAR			_szFile[ _MAX_PATH ];
	BOOL			_fIgnoreSuccess;
	PfnFCCallBack	_pfnSpecificIgnore;
	
	// private default ctor--must use parametered version
	CIgnoreFile();

public:
	CIgnoreFile ( CFileChange * pfc, LPCTSTR sz, PfnFCCallBack pfn = NULL ) {
		_tcsncpy ( _szFile, sz, _MAX_PATH );
		_szFile[ _MAX_PATH - 1 ] = 0;
		_pfc = pfc;
		_pfnSpecificIgnore = pfn;
		_fIgnoreSuccess = pfc->FIgnoreFile ( _szFile, fTrue, _pfnSpecificIgnore );
		}
	~CIgnoreFile() {
		if ( _fIgnoreSuccess )
			_pfc->FIgnoreFile ( _szFile, fFalse, _pfnSpecificIgnore );
		}
	};

#endif /* } */
