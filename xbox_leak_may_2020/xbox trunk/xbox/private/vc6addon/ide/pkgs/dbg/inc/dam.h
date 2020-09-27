class CGoToAddress;

// This value is the maximum length of a symbol.  It should
// probably be in shapi.h since this is determined by the OMF
// the current size of a symbol is an unsigned 8-bit value,
// which is 255 and we add 1 in for a zero terminator
#define	cbSymMax	256

typedef struct _mdf {				// Data structure associated
									// with each opened file
    HSF         hsf;				// The OMF file handle
    long        clnBuf;				// Number of lines in the file
	char FAR *      lszFile;		// Name of file that we opened
	BYTE FAR *      lpbFile;		// Pointer to buffer containing
									// translated file
	long FAR *      mpLineToOff;	// mapping of line numbers to
									// buffer offsets.
	BOOL		bRetry;				// We will search for ths file corresponding to this hsf again if this bit is set.	
} MDF;
typedef MDF FAR *       LPMDF;

typedef struct MFL {
	HSF		hsf;
    WORD	wLine;
} MFL; // MapFileLine information

typedef enum {
    metNone,																	
    metSource,
    metDisasm,
    metLabel,
	metFileName
} MET; // Map Entry Type

typedef struct DML {
    MET 	met;
	ADDR	addr;			// address of line information
	MFL		mfl;			// file/line information (source line)
}DML;       // Diasm/Mixed Line

typedef DML FAR *	PDML;

// structure definition for window info-block  - to be made global

typedef struct dmi {
	BOOL	fAnnotate;		// annotate disassembly with source code
							// (mixed mode)
    BOOL    fDoCodeBytes;	// do code bytes display in disassembly?
    BOOL    fLowerCase;		// display mnemonics && regs in lower case
    BOOL    fDoSymbols;		// display symbols in disassembly
    BOOL    fDontDraw;		// To suppress redundant painting
    BOOL    fNextAddrSet;	// cbGetLine buf set the addr @idmlInvalid
    BOOL    fNewMap;		// Did a new map get set for this window?

    ADDR    addrCSIP;       // The address CS:IP
	ADDR	addrCall;		// Cached address of viewed context for
							// displaying callstack selection

	ADDR	addrSegLim;		// Addr of the current limit info for
							// caching OSDGetObjectLength info

    // Caching Information

    DWORD   lnLastCSIP;     // The last line with the CS:IP bar

    // Mapping Information
    DWORD   lnMapStart;     // The top of the map
    DWORD   lnEnd;          // The end of the (virtual) file.
    int    	idmlInvalid;    // The index of the start of invalid
                                // region in map
	int		cdml;			// Number of DMLs allocated to rgdml
    PDML	rgdml; // The map for the disasm/mixed lines
} DMI;		// Disasm/Mixed Info

// Smallest number of DMLs allowed
#define cdmlMin		50

// Main wndprocs
extern DWORD FAR PASCAL DAMLTextWndProc( HWND, UINT, WPARAM, LPARAM );

// Drag-drop support.
void DAMDropNewAddress(LPSTR lpstr, DWORD cchData);

// New APIs for file handling based on HSFs
BOOL PASCAL DAMFLoadHSFFile( HSF );
void PASCAL DAMUnloadAll( void );
BOOL PASCAL DAMFGetLine( HSF, long, int, char *, UINT ctchTab );
LPMDF   DAMLpmdfFromHSF( HSF hsf );

class CDAMView : public CMultiEdit {
	DECLARE_DYNCREATE(CDAMView)

	private:
		DMI			m_dmi;
		DLA			m_dla;
		UINT		m_WidgetFlags;
		BOOL		m_bAskForFile;

		// WM_xxx message intercepters (DAMAPI.CPP)
		BOOL	FDoScrollChar( WPARAM, LPARAM * );
		BOOL	FDoVScroll( WPARAM, LPARAM );

		// Map Management APIs (DAMMAP.CPP)
		int		FillAsmFromNext( BOOL, PDML, PDML );
		void	FillEntryFromPrev( int );
		void  	FillAsmFromPrev( int );
		void  	FillMixedFromPrev( int );
		int  	FillEntryFromNext( int );
		int  	ShiftMap( int );
		BOOL	SetMap( HSF, WORD, PADDR, BOOL, WORD );
		BOOL	SetMapFromFile( HSF, WORD, WORD );
		BOOL	SetMapFromAddr( PADDR, BOOL );
		MET 	RangeFromPdml( PDML, PADDR, PADDR );
		BOOL 	PreDraw( int, BOOL );
		void 	SetMapToTop( void );

		// Misc utility APIs (DAMUTIL.CPP)
		void	ShiftTopLine( DWORD );
		UOFFSET GetFixedOff( ADDR );
		void	AlignAddrWithSource( PADDR, PADDR );
		void	ResetMap( DWORD );
		BOOL	FSetCursorAtAddr( PADDR );
		void	SetAttributes ( PDML );
		long	SetAddress( PADDR );
		BOOL 	LineToAddr ( HSF, WORD, PADDR, BOOL );
		UOFFSET SegmentLength ( ADDR );
		void	GotoTopOrEnd( BOOL );
		void	GotoTop();
		void	GotoEnd();
		BOOL	SetCsip( PADDR, BOOL bForce = FALSE );
		void	DestroyMap( void );
		GCAF	GcafGetCursorAddr( PGCAINFO );
		void	ResizeMapToWindow( void );
		void	ChangeOpts( void );

	public:
		CDAMView();
		~CDAMView();

		virtual DWORD	GetHelpID() { return HID_WND_DISASSEMBLY; }

		virtual void	ShowContextPopupMenu(CPoint pt);
		virtual BOOL	CanDropText( void ) { return TRUE; }
		virtual BOOL	SupportsDataTips( ) const { return TRUE; }
		virtual BOOL	InsertFromDataObject(COleDataObject * data, const CPoint & point);
		virtual BOOL	FCanUseMarginSelect( void ) { return TRUE; }
		virtual void	GotoLine( void );

		void		UpdateView( PCXF );

		// Edit manager overrides
		WORD 		CbGetLineBuf( DWORD, WORD, char *, PDLA & );
		DWORD		CLinesInBuf( void );
		void 		DrawLineGraphics( CDC *, RECT &, DWORD );

		LRESULT		LTextProc( UINT, WPARAM, LPARAM );
		BOOL		OkToAskForFile() const
		{	return m_bAskForFile; }
			
	private:
        friend class CDebugPackage;
		friend class CGoToAddress;
		// g_cRef is the count of instantiations of CTextView, used to
		// initialize g_pFontInfo which is the pointer to the CFontInfo
		// for this view type
		static UINT g_cRef;
		// Pointer to the font information for this view type
		static CFontInfo *g_pFontInfo;
};
