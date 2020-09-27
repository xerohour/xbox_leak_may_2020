#ifndef _MECLASS	// WHOLE FILE
#define _MECLASS

/*
	This file contains macros and functions to have CodeView COW oriented
	functions work under real windows.
*/

/*************************************************************************
	[begin] MultiEdit utility functions, types, and defines
 ************************************************************************/

typedef struct dla {
	FMT_ELEMENT	*pElement;
	WORD		cb;
} DLA;

typedef DLA *	PDLA;

/*** emsc - edit message sel char */
typedef struct emsc {
    WORD    x1;     /* old hiword( lParam ) */
    WORD    x2;     /* old loword( lParam ) */
    DWORD   y;      /* old wparam */
} EMSC;
typedef EMSC FAR *	LPEMSC;

/*** ld - line buffer */
typedef struct ld {
	WORD	flags;
	WORD	cb;		/* count of bytes in buffer */
	WORD	cbMax;	/* max buffer length */
	char *	prgch;	/* pointer to buffer */
} LD;

/*** ip - insert point */
typedef struct ip {	/* insert point */
	WORD 	ob;	    /* offset to current column position */
	DWORD 	oln;	/* offset to current line position */
} IP;

/*** pd - program descripter */
typedef struct pd {
	DWORD	olntop;	/* offset within progrm display at top of screen */
	WORD 	obleft;	/* offset within list buffer of left most character */
	DWORD	oln;	/* current ld line offset within program */
} PD;

extern BOOL fPasteOk;
extern BYTE fInsertMode;
extern LD  ldEMScratch;

extern WORD emFlags;
#define EMF_IN_EDITMGR 0x01

/* styles */
#define ES_NOREDRAW	0x0002
#define ES_NOSELECT	0x0004

/* control messages: */
#define EM_SELCHARS		(WM_USER+700+0)
#define EM_GETWORD		(WM_USER+700+2)
#define EM_GETLINESEL	(WM_USER+700+3)
#define EM_MOVECURSOR	(WM_USER+700+16)
#define WM_PAINTLINE	(WM_USER+700+17)
#define WM_GETPASTELINE (WM_USER+700+18)
#define WM_REDO			(WM_USER+700+19)
#define WM_PAINTSCREEN	(WM_USER+700+20)
#define WM_FLUSHFOCUS	(WM_USER+700+22)
#define EM_GETTEXT    	(WM_USER+700+23)
#define EM_PTINSELECTION (WM_USER+700+24)

/* ldCur.flags values */
#define LD_fDirty			0x0001

typedef short    RX;
typedef short    RY;
typedef short    AX;
typedef short    AY;

typedef struct _rrc {
   RX  rxLeft;
   RX  rxRight;
   RY  ryTop;
   RY  ryBottom;
} RRC;

typedef RRC *  PRRC;

#define olnMax      ((DWORD)-1L)
#define obMax       ((WORD)-1)

#define KK_SHIFT    (0x8000)
#define KK_CONTROL  (0x4000)

class CDragInfo; //forward decl

class CMultiEdit : public CDockablePartView
{
    friend void ClearMultiEdit          ( void );
    friend void AddThisToCMEList        ( CMultiEdit * );
    friend void RemoveThisFromCMEList   ( CMultiEdit * );
    friend HWND GetDebugWindowHandle    ( UINT );
	friend void UpdateAllMultiEditScroll();

    DECLARE_DYNCREATE(CMultiEdit)

protected:
    /* from CTextView */
    CFontInfo   *m_pFontInfo;
    /* end from CTextView */

private:

    static CMultiEdit   *m_pList[NUM_ME_WINDOWS];   // there are 6 possible multi-edit windows

private:
	// Edit manager data

	// File/view position of top left corner
	PD	    m_pdCur;

	// Location of cursor within the view (always visible)
	IP	    m_ipCur;

	// Location of the anchor during selections
	IP	    m_ipAnchor;

	// No-longer used? for copy/paste operations (COW version)
	LD	*   m_pldCur;

	// Display vertical and horizontal scroll bars?
   	BOOL	m_fScrollV;
   	BOOL	m_fScrollH;

	// Indicates if window is redraw or not
	BYTE	m_Style;

	// Is there a selection?
	BYTE	m_fSelection;

	// Multiplier for very large scroll ranges.  This will
	// let us have 32-bit line numbers in a 16-bit space
	int		m_dScrollVal;

	// Cached value for determining vertical scrollbar
	// ranges
	DWORD	m_olnMacPrev;

	// Cursor and anchor positions for intelligent
	// painting of selection range changes
	IP		m_ipCurOld;
	IP		m_ipAnchorOld;

	// Direction to adjust cursor when cursor placed on a
	// trail byte.  This will be toggled between 1 and -1
	// for adjustments on vertical realignment
	int		m_iAdjustDbcs;

	// Is view in a state where CMultiEdit should start
	// processing things?  This will be set to true after
	// the window has received a WU_INITDEBUG and cleared
	// when the window receives the WU_CLEARDEBUG.  Only
	// used in CMultiEdit::WindowProc!
	BOOL	m_fUseMultiEdit;

	// Dbcs characters are passed in byte by byte, so we
	// receive 2 wm_char messages.  CMultiEdit::WindowProc
	// will cache the 1st byte and when the 2nd is received
	// it will create a single 2-byte char which will get
	// passed on to the TextProc.  Note, the 1st byte's
	// WM_CHAR message will NOT be reveived by the TextProc!

	// Is dbcs cache empty?
	BOOL	m_fAcceptDbcs;

	// Only valid when m_fAcceptDbcs == FALSE!  When valid, it
	// contains the lead byte which is to be combined with the
	// next WM_CHAR value to form a DBCS character
	BYTE	m_bDbcsLead;

	// TRUE if doing column selections, FALSE otherwise (default)
	BOOL	m_fColumnSelect;

	// Internal edit manager workings
	void    ResizeScroll( void );
	long    LGetShiftState( void );
	BOOL    FGetSelection( DWORD, WORD *, WORD * );
	void    RedisplaySelection( void );
	void    PasteSelection( void );
	HANDLE  CopySelection( char FAR *, size_t );
	void    DeleteSelection( void );
	long    LcbGetText( WORD,char FAR * );
	long    LcbGetWord( UINT, char FAR *,WORD *,WORD * );
	void    UpdateAnchor( long );
	WORD    CbGetCachedLine( DWORD, WORD, char * );
	WORD    CbGetLineIntoCache( DWORD );
	void    FlushCache( void );
	void    GetEditRrc( BOOL, PRRC );
	void    DrawAttrText(CDC *, BOOL, DWORD, char *, WORD, WORD, PDLA, RY, WORD );
	void    UpdateCursorPos( void );
	BOOL    FMoveWindowToCursor( BOOL );
	void    ValidateCursor( void );
	DWORD   Paint( PRRC, CDC * );
	void    VScroll( UINT, long );
	void    HScroll( UINT, long );
	void    DeleteChar( BOOL );
	void    InsertChar( UINT, long );
	void    PreviousWord( long );
	void    NextWord( long );
	void    DoEnterKey( void );
	void    DoChar( UINT, long );
	void    PaintLine( DWORD );
	void    DoMouse( UINT, UINT, long );
	BOOL    FMouseHitSel( BOOL, LPARAM );
	int     ScrollFromOln( DWORD );
	void	AdjustDbcsCursor( int );
    BOOL    GetHelpWord(CString&);
    BOOL    FWhitespaceOnly(PCSTR, _CB);

protected:

	// Undo/Redo stuff
	// States of engine
	enum UNDO_STATE {

		usNormal,		// Edits should record.
		usSuspendedError		// Edits should not record -- suspended due to error

	}; 

	enum UNDO_BUFFER {

		ubUndo,			// current recording buffer
		ubRedo

	};

	INT		m_mePlayCount;
	UNDO_BUFFER  m_undoBuffer;	 // current recording buffer.
	UNDO_STATE  m_undoState;	 // state of recording process.


	virtual	LRESULT	WindowProc(UINT, WPARAM, LPARAM);
				// The WndProc for this window.
	int     OnCreate(LPCREATESTRUCT lpCreateStruct);
    void    OnEditCopy();
    void    OnEditUndo();
    void    OnEditRedo();

    void    OnUpdateGotoLine(CCmdUI *);
    void    OnUpdateEditUndo(CCmdUI *);
    void    OnUpdateEditRedo(CCmdUI *);

    void    OnGotoLine();
	void	SelectWordCur();

    DECLARE_MESSAGE_MAP()

public:

    DOCTYPE     m_dt;

    /* from CTextView */
    INT         m_cxSelectMargin;
    UINT        m_cbTab;          // Cached tab from doc
    /* end from CTextView */

public:
	// Constructors, destructors

	CMultiEdit();
	CMultiEdit(INT);
	virtual			~CMultiEdit() { RemoveThisFromCMEList(this); }

	// Don't let message mapping occur for keys handled by edit manager
	virtual BOOL 	PreTranslateMessage(MSG* pMsg);

	// Selection management
	virtual BOOL	GetCurrentText(BOOL *lookAround, LPSTR pText, _CB  maxSize,	ICH * xLeft, ICH * xRight);
	virtual void 	OnFindNext(BOOL* pbSuccess = NULL); // called for a find

	// Drawing methods
	virtual	VOID	OnDraw(CDC *); 
	void            DrawMarginGlyphs(HDC hDC, RECT& rcMargin, UINT uStatus);

	// Edit manager overridables/callbacks
	// NOTE: The following functions should be pure virtuals, but
	// IMPLEMENT_DYNCREATE will choke on the class since it is
	// an abstract class.  For this reason, the functions have
	// bodies here which will assert!
	virtual DWORD		CLinesInBuf( void ) { ASSERT( FALSE ); return NULL; }
	virtual WORD 		CbGetLineBuf( DWORD, WORD, char *, PDLA & ) { ASSERT( FALSE ); return NULL; }
	virtual BOOL		FReplaceLineBuf( DWORD, WORD, char * );
	virtual BOOL		FReadOnlyBuf( void );
	virtual BOOL		FIsWordChar( TCHAR );
	virtual WORD		ObMaxBuf( void );
	virtual void 		DrawLineGraphics( CDC *, RECT &, DWORD ) {};
	virtual LRESULT		LTextProc( UINT, WPARAM, LPARAM );	
    virtual void        ShowContextPopupMenu( CPoint );
    // GoToLine should be over-ridden if the view supports it.
    virtual void        GotoLine( void ) { ASSERT(FALSE); ::MessageBeep(0); }

	// Override if user can edit this window.
	virtual BOOL		IsEditable( ) { return FALSE; }
	void	GetClientRrc( PRRC );
	void	InvalidateRrc( PRRC );
	void	DrawLine( DWORD );
	void	DrawWindow( void );
	DWORD	WinHeight( void );
	WORD	WinWidth( void );
	WORD	ObGetCurPos( void );
	void	SetCurObPos( WORD );
	DWORD	OlnGetCurPos( void );
	void	SetCurOlnPos( DWORD );
	DWORD	OlnGetTop( void );
	WORD	ObGetLeft( void );
	void	SetCursorAt( WORD, DWORD );
	void	SetWinTopLineCursor( DWORD, WORD, DWORD );
	void	SetWinTopLine( DWORD );
	void	SetWinUpperLeft( WORD, DWORD );
	DWORD	GetEditSelection( WORD, char FAR * );
	DWORD	GetEditWord( WORD, char FAR * );
	WORD	FInsertMode( void );
	DWORD	OlnFromScrollPos( int );
	BOOL	InitEditMgr();
	BOOL	GetWordAtPosition(WORD, DWORD, LPSTR pBuf, UINT maxSize, WORD * obMic,
									WORD *obMac);

	void	FlipBytes( BYTE FAR *, int );
	BOOL		FCanUndo( );	
	BOOL		FCanRedo( );
	
	virtual BOOL SupportsDataTips( ) const   { return FALSE; }
	void ArmDataTip();

	// OLE Drag/Drop
public:
	virtual BOOL	CanDropText( void ) { return FALSE; }
	virtual BOOL	CanMoveText( void ) { return FALSE; }
	virtual DROPEFFECT	OnDropEx( COleDataObject* pDataObject, DROPEFFECT dropDefault, DROPEFFECT dropList, CPoint point );
	virtual DROPEFFECT	OnDragEnter( COleDataObject* pDataObject, DWORD grfKeyState, CPoint point );
	virtual DROPEFFECT	OnDragOver( COleDataObject* pDataObject, DWORD grfKeyState, CPoint point );
	virtual void		OnDragLeave();
	// utilities
	virtual BOOL		DoDrag(CPoint point);
	virtual void        OnDragEnterFeedback(DWORD grfKeyState, CPoint point) { HideCaret(); }
	virtual void        OnDragOverFeedback(DWORD grfKeyState, CPoint point) { ; }
	virtual void        OnDragLeaveFeedback() { ; }
	virtual BOOL		InitDataSource(COleDataSource& data, const CPoint & point);
	virtual BOOL		InsertFromDataObject(COleDataObject * data, const CPoint & point);
	virtual BOOL		IsDragActive() { return FALSE; }
	// Drag & Drop implementation
private:
	COleDropTarget	m_DropTarget;
	CDragInfo *		m_pDragInfo;

public:
	virtual void	PosFromPoint( int &, int &, CPoint );
	virtual BOOL	FCanUseMarginSelect( void ) { return FALSE; }
	inline BOOL	FDbcsEnabled( void ) { return (BOOL)( m_pFontInfo->m_tm.tmAveCharWidth != m_pFontInfo->m_tm.tmMaxCharWidth ); }
	virtual BOOL Create(LPCSTR lpszClassName,
		LPCSTR lpszWindowName, DWORD dwStyle,
		const RECT& rect,
		CWnd* pParentWnd, UINT nID,
		CCreateContext* pContext = NULL);
};

#endif	// !_MECLASS

