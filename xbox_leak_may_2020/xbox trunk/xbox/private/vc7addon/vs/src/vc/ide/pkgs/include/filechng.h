//-----------------------------------------------------------------------------
//  filechng.h
//
//  Copyright (C) 1993, Microsoft Corporation
//
//  Purpose:    A dummy class to accomadate filechange operations for the
//				build system. When the filechange notification is enabled
//				in the new shell, we can call the appropriate methods.
//				Untill then, we'll use these dummy methods.
//              modified.
//
//  Revision History:
//
//  []      10-Mar-1997 WinslowF    Created
//
//-----------------------------------------------------------------------------
#ifndef _FILECHNG_H
#define _FILECHNG_H

class FCItem;
typedef void (*PfnFCCallBack)( const FCItem *, DWORD dwUser );  // call back function
typedef void (*PfnAsyncReturn)( const FCItem * );
typedef void *		PV;

class FCItem {  // File Change Item
public:

};

class   CSection : public CRITICAL_SECTION 
{
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
		_ASSERT(m_cLocks > 0);	// underflow test
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



// same as CritSection but for the "testable" variety
class CritSectionT 
{
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


class CFileChange {

public:
	CFileChange(){};
	~CFileChange(){};

	// add a file to be watched
	BOOL FAddFile ( LPCTSTR szFile, PfnFCCallBack pfn, BOOL fCallAlways = FALSE )
    {
        return TRUE;
    };
	
	// remove a file from the list
	BOOL FDelFile ( LPCTSTR szFile, PfnFCCallBack pfn )
    {
        return TRUE;
    };

   	// async add file, returns true if queued successfully
	BOOL FAddFileAsync (LPCTSTR	szFile,	PfnFCCallBack, PfnAsyncReturn,
		PV pvUser1 = NULL, PV pvUser2 = NULL, BOOL fCallAlways = FALSE)
    {
        return TRUE;
    };

	// async del file, returns true if queued successfully
	BOOL FDelFileAsync (LPCTSTR szFile,	PfnFCCallBack, PfnAsyncReturn,
		PV pvUser1 = NULL, PV pvUser2 = NULL)
    {
        return TRUE;
    };

};


extern CFileChange	g_filechange;

inline CFileChange * GetFileChange()
{
    return &g_filechange;
}


// CIgnoreFile...used as an object to automatically ignore changes to
// a file while doing something with it.  Automatically unignores
// when object is destroyed.
//
class CIgnoreFile 
{
	// private default ctor--must use parametered version
	CIgnoreFile();

public:
	CIgnoreFile ( CFileChange * pfc, LPCTSTR sz, PfnFCCallBack pfn = NULL ) 
    {
	}

    ~CIgnoreFile() 
    {
	}

};


#endif // _FILECHNG_H
