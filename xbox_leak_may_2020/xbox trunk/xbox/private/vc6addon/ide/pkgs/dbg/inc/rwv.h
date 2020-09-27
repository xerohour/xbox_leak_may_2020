// Start at 0x1000 to ensure that we do not collide with
// !OSDEBUG4s dwGrp values
typedef enum _rwf {
    rwfNone = 0x1000,		// Nothing in location
    rwfHexReg,				// Hex displayed value
    rwfFlag,				// Flag (bit) value
	rwfFloat,				// floating point displayed value

	// Text.  Used when DBL_CLICKING to toggle a flag
	// this will effectively "widen" the DBL_CLICK region
	rwfText,

	// rwfEffAddr and effData will BOTH be present if there's
	// an EA and the entire display doesn't fit on 1 line.  If
	// there's enough room, then rwfEffAddr will be present.

    rwfEffAddr,				// EA combination (addr & value)
    rwfEffAddrOnly,			// EA address only, not editable
    rwfEffData				// EA data only, editable
} RWF;

#define cbRegMax    64
typedef union _ur {
    BYTE    rgb[ cbRegMax ];
    ULONG   ul;
} UR;
typedef UR FAR * LPUR;

typedef union _ut {
	RT	rt;
	FT	ft;
} UT;
typedef UT FAR * LPUT;

typedef struct _rui {
    RWF		rwf;
	UT		ut;
    DWORD	hReg;
    WORD    iFlag;
	WORD	cbEA;			// Number of bytes for EAs only
	ADDR	addrEA;			// Address for EAs only
    UR		ur;
} RUI;
typedef RUI FAR *   LPRUI;
typedef HLLE    	HRUI;

typedef HLLE    	HRWI;
#define hrwiNull    ((HRWI)hlleNull)

typedef struct _rwi {
    RUI     rui;            // This MUST be the first item in this struct
    LSZ     lsz;            // since RWIs and RUIs are BOTH passed to
							// RWSetReg!!!
    UINT    cbit;
    WORD    obSzMin;		// char index for beginning of label
    WORD    obValMin;		// char index for beginning of value
    WORD    obValMac;		// char index for end+1 of value
    DWORD   oln;			// display line for item
	DWORD	dwGrp;			// display group (determines breaks)
    HRWI    hrwiPrev;		// Previous pointer (for non-dbl linked)
	UR		urDiff;			// data from previous execution
} RWI;
typedef RWI FAR *    LPRWI;

class CCpuView : public CMultiEdit {
	DECLARE_DYNCREATE(CCpuView)

	private:
		/* List of edits for undo/redo */
		HLLI    	m_hlliRuiUndo;
		HLLI    	m_hlliRuiRedo;

		/* List of registers, values, and cooridnates */
		HLLI    	m_hlliRwi;

		/* Item which needs to be saved */
		HRWI		m_hrwiEdit;

		/* Display attributes */
		BOOL		m_fShowCpu;
		BOOL		m_fShowFlags;
		BOOL		m_fShowEA;
		BOOL		m_fShowFPU;

		/* Update parameters */
		WORD		m_wEM;
		WORD		m_wModel;
		BOOL		m_f386Mode;

		/* Index to here the EA item should be placed */
		DWORD		m_ihrwiEA;
		int			m_cEA;

		/* max display width (calculated) */
		WORD		m_obMax;
		WORD		m_obMacPrev;

#if defined (_X86_)
		/* display width of long double */
		size_t		m_cchLongDouble;
#endif	// _X86X

		/*
		*  axMax == max chars per line
		*  4 == (smallest display entry == flag)
		* 		1 + 	space
		* 		1 + 	single char flag name
		* 		1 +		'='
		* 		1 +		value ( 0 || 1 )
		*
		* The following will allow us to display axMax * 4 - 1
		* flags on a SINGLE line.  This is overkill, but here
		* we will not have to worry about dynamic allocations
		* and memory failures.
		*/
		DLA 	m_rgdla[ axMax ];

		/* member functions */
		BOOL 	FIsVisible( HRWI );
		void 	UpdateDocInfo();
		void 	SaveForUndo( LPRUI );
		void	WriteReg( LPRUI, LPUR );
		void 	FlushEdit( BOOL );
		void 	NextField( void );
		void 	PrevField( void );
		void 	SetReg( LPUR, HRUI, BOOL );
		RWF 	RwfFieldType( HRWI * );
		void 	ToggleFlag( HRWI );
		void 	RwiUpdateEA( void );
		void	AddEANode( DWORD, int, LPCH, LPADDR, int );
		void 	UpdateRwi( void );
		void 	UpdateRegs( void );
		void 	UndoValue( BOOL );
		BOOL 	FDoKeyDown( WPARAM, LPARAM );
		BOOL 	FDoChar( WPARAM, LPARAM );
		void 	Freeze( void );
		void 	RecalcCoords( BOOL );
		void 	UpdateDisplay( BOOL );
		int		InitWndMgr( void );
		void 	DestroyInfo( void );
		void	InitializeMembers( void );
		size_t	CchFloatFromRwi( LPRWI );

	protected:
		BOOL	PreTranslateMessage( MSG * pMsg );

	public:
		/* default constructor/destructor */
		CCpuView();
		virtual ~CCpuView();

		virtual DWORD GetHelpID() { return HID_WND_REGISTERS; }
		virtual BOOL  IsEditable( ) { return TRUE; }

		virtual void ShowContextPopupMenu(CPoint pt);

		// No textual help available for this window!
		virtual BOOL GetHelpWord( CString& strHelp ) { return FALSE; }

		WORD 		CbGetLineBuf( DWORD, WORD, char *, PDLA & );
		WORD		ObMaxBuf( void );
		DWORD		CLinesInBuf( void );

		LRESULT	LTextProc( UINT, WPARAM, LPARAM );

	private:
		friend class CDebugPackage;
		// g_cRef is the count of instantiations of CTextView, used to
		// initialize g_pFontInfo which is the pointer to the CFontInfo
		// for this view type
		static UINT g_cRef;
		// Pointer to the font information for this view type
		static CFontInfo *g_pFontInfo;
};
