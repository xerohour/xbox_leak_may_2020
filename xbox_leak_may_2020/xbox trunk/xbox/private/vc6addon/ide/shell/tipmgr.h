// TipMgr.h
// Abstract Tip manager class

#ifndef __TIPMGR_H__
#define __TIPMGR_H__

enum TipStatus
{
	Unknown   = -3, // unknown state
	Invalid   = -2, // tip data is invalid/corrupt
	Exhausted = -1, // all tips have been shown
	Ok = 0 
};

/////////////////////////////////////////////////////////////////////////////
// CTipManager
// pure abstract base class

class CTipManager
{
public:
	virtual BOOL isTipAvailable() = 0;
	virtual BOOL SaveState() = 0;
	virtual TipStatus Status() = 0;
	virtual TipStatus Next() = 0;
	virtual TipStatus Tip( CString & sTip ) = 0;
	virtual void Reset() = 0;
};


/////////////////////////////////////////////////////////////////////////////
// CTipState
// Persistent state for an individual Tip file

typedef signed char PersistTipStatus;
typedef signed char Priority_t;
const Priority_t DefaultPriority = 50;
const Priority_t MAX_PRIORITY = 100;
const Priority_t MIN_PRIORITY = 0;
typedef short Order_t;
const Order_t DEFAULT_ORDER = (Order_t)MIN_PRIORITY-1;
const Order_t ORDER_STATUS_BASE = SHRT_MIN+10; // +10 is room for TipStatus offset

struct CTipState
{
	friend BOOL CheckState(const CTipState & A, const CTipState & B);

	CTipState();
	CTipState( const CFile & f, BOOL bIsFileOpen );
	CTipState( LPCTSTR szName );
	CTipState( const CTipState & ts );

// data
	PersistTipStatus m_Status;
	Priority_t m_nPriority;
	DWORD m_dwCheck;
	DWORD m_dwPos;
	Order_t m_nOrder;

// access
	int Rank() { return m_Status < 0 ? (int)m_Status : (int)m_nPriority; }
	int RankOrder();
	TipStatus Status() { return (TipStatus)m_Status; }

// operations
	TipStatus SetStatus( TipStatus t );
	int SetOrder(int nOrder) { m_nOrder = (Order_t)nOrder; return nOrder; }
	BOOL Read( LPCTSTR szName );
	BOOL Read( const CFile & f, BOOL bIsFileOpen );
	BOOL Write( LPCTSTR szName );
	CTipState & operator=( const CTipState & rhs );
};

inline int CTipState::RankOrder()
{
	if (m_Status < 0)
		return ORDER_STATUS_BASE + (int)m_Status;
	else
	{
		if (m_nOrder == DEFAULT_ORDER)
			return (int)m_nPriority;
		else
			return DEFAULT_ORDER - (int)m_nOrder;
	}
}

inline BOOL CheckState(const CTipState & A, const CTipState & B) { return A.m_dwCheck == B.m_dwCheck; }

inline CTipState::CTipState()
:	m_Status((PersistTipStatus)Unknown), m_nPriority(DefaultPriority),
	m_dwCheck(0), m_dwPos(0), m_nOrder(DEFAULT_ORDER)
{}

inline CTipState::CTipState( const CFile & f, BOOL bIsFileOpen )
:	m_Status((PersistTipStatus)Unknown), m_nPriority(DefaultPriority),
	m_dwCheck(0), m_dwPos(0), m_nOrder(DEFAULT_ORDER)
{
	Read(f,bIsFileOpen);
}

inline CTipState::CTipState( LPCTSTR szName )
:	m_Status((PersistTipStatus)Unknown), m_nPriority(DefaultPriority),
	m_dwCheck(0), m_dwPos(0), m_nOrder(DEFAULT_ORDER)
{
	Read(szName);
}

inline CTipState::CTipState( const CTipState & ts )
{
	memcpy((CTipState*)this, &ts, sizeof CTipState);
}

inline CTipState & CTipState::operator=( const CTipState & ts )
{
	memcpy((CTipState*)this, &ts, sizeof CTipState);
	return *this;
}

inline TipStatus CTipState::SetStatus( TipStatus t )
{
//	if (t==Invalid)
//		m_dwCheck = m_dwPos = 0;
	m_Status = (PersistTipStatus)t;
	return t;
}


/////////////////////////////////////////////////////////////////////////////
// CTipFile

class CTipFile : public CTipManager
{
public:
	~CTipFile() { Close(); }
	CTipFile( LPCTSTR szFile ) : m_bIsFileOpen(FALSE) { Init(szFile); }

// access
	BOOL isTipAvailable();
	int Rank() { return m_State.Rank(); }
	int RankOrder() { return m_State.RankOrder(); }
	int SetOrder(int nOrder) { return m_State.SetOrder(nOrder); }
	TipStatus Status() { return m_State.Status(); }
	Priority_t Priority() { return m_State.m_nPriority; }
	TipStatus Tip( CString & sTip );
	CString GetFilePath() { return m_File.GetFilePath(); }
	CString GetFileName() { return m_File.GetFileName(); }

// operations
	TipStatus Next();
	BOOL SaveState();
	void Reset();
	TipStatus Close();
	TipStatus Init(LPCTSTR szFile);

protected:
	void DoCommand( const CString & sCmd, const CString & sArg );
	CStdioFile m_File;
	BOOL m_bIsFileOpen;
	CString m_sTip;
	CTipState m_State;
};

inline void CTipFile::Reset() { m_State.m_dwPos = 0; m_State.SetStatus(Unknown); Next(); }

/////////////////////////////////////////////////////////////////////////////
// CTipFileQueue

class CTipFileQueue
{
public:
	CTipFileQueue( ) : m_rgptf(0), m_nMax(0), m_nLast(-1), m_bDirty(FALSE) {}
	~CTipFileQueue();

	BOOL Create( int nSize );
	TipStatus Init(CPtrList & lptf);

	int Size() { return m_nMax; }
	BOOL IsEmpty() { return m_nLast < 0; }
	void Add( CTipFile * ptf );
	CTipFile * Get();
	CTipFile * Peek(BOOL bMakeDirty = FALSE);
	CTipFile * operator[](int nIndex);

protected:
	void AddByOrder( CTipFile * ptf );
	CTipFile **m_rgptf;
	int m_nLast;
	int m_nMax;
	BOOL m_bDirty;
};

inline CTipFile * CTipFileQueue::Peek(BOOL bDirty)
{
	ASSERT(m_rgptf != NULL);
	if (bDirty)
		m_bDirty = TRUE;
	return m_rgptf[0];
}

/////////////////////////////////////////////////////////////////////////////
// CTipFileMgr

class CTipFileManager : public CTipManager
{
public:
	CTipFileManager();
	~CTipFileManager();
	TipStatus Status() { return m_Status; }
	isTipAvailable();
	BOOL SaveState();
	TipStatus Next();
	TipStatus Tip( CString & sTip );
	void Reset();
	TipStatus Init();

protected:
	CTipFileQueue m_qptf;
	TipStatus m_Status;
};

inline CTipFileManager::CTipFileManager()
: m_Status(Unknown)
{
}

#endif

