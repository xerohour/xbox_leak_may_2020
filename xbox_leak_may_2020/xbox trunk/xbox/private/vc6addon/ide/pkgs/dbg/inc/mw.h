class CGoToAddress;

#define	MWNINCH	(0xffff)

enum _autotypes
{
 MW_AUTO_HEX = -1,
 MW_NO_AUTO_TYPE,
 MW_AUTO
};

enum _expropts
{
 MW_DEAD = 0,
 MW_LIVE,
};

enum _movmnt
{
 MW_REV = -1,
 MW_CUR,
 MW_FWD
};

enum _celltype
{
 MW_NONE = 0,
 MW_ADDR,
 MW_FMT,
 MW_RAW,
 MW_ASCIIDAT
};

enum _mpiimwd
{
 MW_ASCII = 0,
#ifdef _WIN32
 MW_WCHAR,
#endif	// _WIN32
 MW_BYTE,
 MW_INT,
 MW_INT_HEX,
 MW_INT_UNSIGNED,
 MW_LONG,
 MW_LONG_HEX,
 MW_LONG_UNSIGNED,
 MW_REAL,
 MW_REAL_LONG,
#ifndef _WIN32
 MW_REAL_TEN,
#endif	// !_WIN32
 MW_cDisplayTypes,
#if _INTEGRAL_MAX_BITS >= 64
 MW_INT64 = 		MW_cDisplayTypes,
 MW_INT64_HEX,
 MW_INT64_UNSIGNED,
 MW_cDisplayTypesExt
#else	//	_INTEGRAL_MAX_BITS >= 64
 MW_cDisplayTypesExt = MW_cDisplayTypes
#endif	//	_INTEGRAL_MAX_BITS >= 64
};

#ifndef _WIN32
#define IsCharFormat(imwd)  (imwd==MW_ASCII)
#else	// !_WIN32
#define IsCharFormat(imwd)  (imwd==MW_ASCII || imwd==MW_WCHAR)
#endif	// _WIN32

#ifdef _WIN32
typedef char *BSZ;
typedef char BCH;
#else
typedef char _based(_segname("_CODE")) *	BSZ;
typedef char _based(_segname("_CODE")) 	BCH;
#endif

typedef struct _mwd
{
	USHORT	cbType;
	USHORT	cchFormatted;	 // Number of chars occupied by formatted
	char	bszFmtIn[ 6 ];	 // sscanf format for input
							 // data type.
} MWD;		// Memory Window Display information
#ifdef _WIN32
typedef MWD *PMWD;
#else
typedef MWD _based(_segname("MW_CODE")) *	PMWD;
#endif

#define cbDataMax		10
#define cchAddr32		8
#define cchAddr16		9
#define cchAddrMax		cchAddr16
#define cchPad			2
#define cchRaw			3
#define cchFixedMax		126
#define chBsp			((char)0x08)
#define chCtrlE			((char)0x05)
#define chNull			((char)0x00)
#define chLineFeed		((char)0x0a)
#define cchBufMax		(axMax+2)
#ifdef MAC
#define szDefaultExpr	"0x00000000"
#else	// MAC
#define szDefaultExpr	"DS:0"
#endif	// MAC
#define wNoType		    ((WORD)0x8000)
#define wFmtHiMin		((WORD)0x8001)
#define wFmtLoMin		((WORD)0x0001)
#define wFmtMask		((WORD)0x8000)
#define wAsciiMask		((WORD)0x7fff)

#ifdef _WIN32
extern MWD const rgmwd[ MW_cDisplayTypesExt ];
#else
extern MWD const _based(_segname("MW_CODE")) rgmwd[ MW_cDisplayTypesExt ];
#endif
extern char const rgchType[ MW_cDisplayTypesExt ];

// mpbHex is for converting bytes to ascii hex.  The lowercase and
// zero terminator are for quick scans for editing addresses.

extern char const mpbHex[];

typedef struct _mwi
{
    const MWD * pmwd;         // display information for type
    char *      szExpr;       // character version of expression
    ADDR        addr;         // evaluated expression - or scrolled current
    USHORT      fLiveExpr;    // Is expression live or dead?
    BOOL        fShowRaw;     // Show raw data?
    USHORT      cFmtVals;     // Number of values/line to display
    DWORD       cLines;       // Number of lines in window
	HLLI		hlliUndo;	  // linked list of undo data
	HLLI		hlliRedo;	  // linked list of redo data
    WORD FAR *  rgwLineInfo;  // line cell information
    WORD        cwLineInfo;   // number of cells in line info
    BOOL FAR *  rgf;          // Value has changed array
    USHORT      cFixedFmt;    // Fixed # of display values.
    USHORT      cchAddr;      // Display width of address
    BOOL        fNoUpdate;    // This is a value to let us
                              // update if its the first time
                              // we've set focus to this window
	BOOL		fRetry;		  // If this is true the first time
							  // we tried updating it was to early.
							  // so we will retry setting the memory
							  // window to the original expression.
} MWI; // Memory Window Information - per window data
typedef MWI *	PMWI;

typedef struct _mub
{
	USHORT		ucls;		// class for edit MW_RAW, MW_FORMAT, MW_ASCIIDAT
	const MWD * pmwd;		// display format when created
	ADDR		addr;
	size_t		cb;
	BYTE		rgb[ cbDataMax ];
} MUB;		// Memory window Undo Buffer
typedef MUB FAR *	LPMUB;

typedef struct _mwc
{
    ADDR    addrBegin;	// beginning address of data
    WORD    cbReq;      // cb requested for read at addrBegin
    WORD    cbRead;     // Successful cb read at addrBegin
    LPB     lpb;        // pointer to alloc'd buffer where addrBegin is
} MWC;      // Memory Window Cache
typedef MWC FAR *   LPMWC;

// We may want this to come from OSDebug later
#define cbPageMax	(4096)

#define MW_LAMAX	256

extern BOOL	fDumping;

#define CBMWCacheMax    (80*50)

extern void		PASCAL MWAddrFromIln( PMWI, DWORD, PADDR );
extern DWORD	PASCAL MWIlnFromAddr( PMWI, PADDR );
extern USHORT	PASCAL MWSetAddrFromExpr( PMWI, int );
extern void		PASCAL MWSaveSettings( char * );
extern void     PASCAL MWScanType( short );
extern void	MWDropNewAddress(LPSTR, DWORD, POINT * lppt);
extern EESTATUS PASCAL CVParseBind ( PHTM, PCXT, int, char, char, char FAR * );
extern char		fCaseSensitive;

class CMemoryView : public CMultiEdit
{
	friend class CGoToAddress;

	DECLARE_DYNCREATE(CMemoryView)

	private:
		DLA	 		m_rgdla[ MW_LAMAX ];

		HLLI		m_hlliCur;
		BYTE FAR *	m_rgbCacheCur;

		HLLI		m_hlliPrev;
		BYTE FAR *	m_rgbCachePrev;

		BOOL		FUpdateInfo( BOOL );
		int		    CchFormatData( USHORT, BOOL, BYTE *, char *, char *, BOOL * );
		void		FormatLine( DWORD, char * );
		void		ChangeTitle( void );
		void		NextField( short, BOOL );
		void		NextLikeField( char * );
		USHORT	    CchGetCellInfo( BOOL, UOFFSET *, USHORT *, WORD * );
		BOOL		FDoChar( UINT, DWORD );
		BOOL		FChangeMemory( BOOL, int, char * );
		void        ChangeDebuggeeData( void );
		void		SaveDebuggeeData( void );
		void		UpdateExprFromMwiAddr( void );
		BOOL		FCmpUserMemory( PADDR, USHORT, BYTE * );
		WORD FAR *  LpwFindNotEq( WORD FAR * );
		void		BuildRgla( void );
		void		MakeWindowActive( void );
		void		OOMError( void );
		void		DoEditField( void );
		void		DoDelChar( BOOL );
		void		UpdateWorld( PADDR, USHORT );
		BOOL		FIsAddrInRange( PADDR, PADDR, PADDR );
		LPB         LpbFromCache( HLLI, PADDR, size_t, size_t * pcbRead = NULL );
		void		CacheDebuggeeData( HLLI *, LPB * );
		void        DestroyCache( HLLI *, LPB * );
		void		UncacheData( void );
		void        UncacheAll( void );
		USHORT	    CbGetDebuggeeBytes( PADDR, size_t, BYTE FAR * );
		BOOL		FPutDebuggeeBytes( USHORT, PADDR, size_t, LPB, HLLI, HLLI );
		void		UndoValue( BOOL );
		void		DestroyUndoList( void );
		void		UpdateView( void );

	protected:
		virtual BOOL Create(LPCSTR lpszClassName,
			LPCSTR lpszWindowName, DWORD dwStyle,
			const RECT& rect,
			CWnd* pParentWnd, UINT nID,
			CCreateContext* pContext = NULL);

	public:
		MWI			m_mwi;
		
		/* default constructor/destructor */
		CMemoryView();
		virtual ~CMemoryView();

		virtual DWORD 	GetHelpID() { return HID_WND_MEMORY; }

		virtual void 	ShowContextPopupMenu(CPoint pt);

		virtual BOOL	CanDropText( void ) { return TRUE; }
		virtual BOOL	InsertFromDataObject(COleDataObject * data, const CPoint & point);
		virtual void    OnDragEnterFeedback(DWORD grfKeyState, CPoint point);
		virtual void    OnDragOverFeedback(DWORD grfKeyState, CPoint point);
		virtual void    OnDragLeaveFeedback();
		virtual void	GotoLine( void );
		virtual BOOL	IsEditable() { return TRUE; }

		// No textual help available for this window!
		virtual BOOL GetHelpWord( CString& strHelp ) { return FALSE; }
		

		WORD 		CbGetLineBuf( DWORD, WORD, char *, PDLA & );
		WORD		ObMaxBuf( void );
		DWORD		CLinesInBuf( void );

		LRESULT		LTextProc( UINT, WPARAM, LPARAM );

		BOOL		SetAttributes( WORD, char *, USHORT, WORD, USHORT );

	private:
		friend class CDebugPackage;
		// g_cRef is the count of instantiations of CTextView, used to
		// initialize g_pFontInfo which is the pointer to the CFontInfo
		// for this view type
		static UINT g_cRef;
		// Pointer to the font information for this view type
		static CFontInfo *g_pFontInfo;
};

inline PMWI MWPmwiOfPwnd(PWND pWnd)
{
	CMemoryView *	pMemoryView = (CMemoryView *)CWnd::FromHandle(pWnd);

	ASSERT(pMemoryView->IsKindOf(RUNTIME_CLASS(CMemoryView)));
	return(&pMemoryView->m_mwi);
}

/////////
// CMemoryEditItem
//
// edit item in the Memory class...
// needs subclass so that it can handler ENTER to add a new Memory...
//

class CMemoryEditItem: public CDockEdit
{
public:
	virtual BOOL PreTranslateMessage(MSG *pMsg);
};

////////
// CMemoryDialogBar
//
// the dialog bar wrapper class for the Memory window
//

class CMemoryDialogBar : public C3dDialogBar
{
public:
	CMemoryEditItem	m_edit;			// the edit field

	virtual void OnUpdateCmdUI(CFrameWnd* pTarget, BOOL bDisableIfNoHndler);
	virtual BOOL OnInitDialog();				// dialog is coming up, init buttons

protected:
	//{{AFX_MSG(CMemoryDialogBar)
	afx_msg void OnDestroy();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
// CMemoryParent
//
// This window serves as a parent for the Memory window
// it holds the check boxes and the edit field widget for the Memory window
//

class CMemoryParent : public CWnd
{
	DECLARE_DYNAMIC(CMemoryParent)
	friend class CMemoryView;
	
public:
	CMemoryParent();
	void RecalcLayout();	

	CMemoryDialogBar	m_toolbar;			// the toolbar for this class
	
protected:
	virtual void PostNcDestroy();
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	virtual BOOL PreTranslateMessage(MSG *pMsg);
	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
	
	CMemoryView*  m_pView;

protected:
	//{{AFX_MSG(CMemoryParent)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	//}}AFX_MSG
	
	DECLARE_MESSAGE_MAP()
};
