#include "stdafx.h"
#pragma hdrstop

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

static char **	rgpsz = NULL ;
static int		csz;
extern void GrabFocusFromDebuggee( void );

// Utility function to get the font for a listbox
// and set a horizontal scroll bar range based upon
// the font number of characters
void SetListBoxExtent( HWND	hwnd, USHORT cchMax ) {
	HDC			hdc;
	TEXTMETRIC	tm;
	HFONT		hFont;

	hdc = GetDC( hwnd );
	if ( hFont= (HFONT)SendMessage( hwnd, WM_GETFONT, 0, 0L ) ) {
		SelectObject( hdc, hFont );
	}
	GetTextMetrics( hdc, &tm );

	ListBox_SetHorizontalExtent(
		hwnd,
		( cchMax + 1 ) * tm.tmMaxCharWidth
	);
	ReleaseDC( hwnd, hdc );
}

class CAmbSrc : public C3dDialog {
	public:
		CAmbSrc( CWnd* pParent = NULL );	// standard constructor

		//{{AFX_DATA(CAmbSrc)
		enum { IDD = IDD_AMBIG_SRCLINES};
		int	m_iSelect;
		//}}AFX_DATA

		virtual BOOL	OnInitDialog();

		afx_msg void	OnListDoubleClick();

		DECLARE_MESSAGE_MAP();

	protected:
		virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
};

BEGIN_MESSAGE_MAP(CAmbSrc,C3dDialog)
	ON_LBN_DBLCLK( ID_AMB_LIST, OnListDoubleClick )
END_MESSAGE_MAP()

CAmbSrc :: CAmbSrc( CWnd * pParent /*=NULL*/)
	: C3dDialog( CAmbSrc::IDD, pParent ) {
}

void CAmbSrc :: OnListDoubleClick() {
	UpdateData( TRUE );
	EndDialog( IDOK );
}

BOOL CAmbSrc :: OnInitDialog() {
	// Fill in the listbox
	int		isz;

	for( isz = 0; isz < csz; ++isz ) {
		if ( rgpsz[ isz ] ) {
			SendDlgItemMessage(
				ID_AMB_LIST,
				LB_ADDSTRING,
				0,
				(LPARAM)(LPSTR)rgpsz[ isz ]
			);
		}
	}

	// Make the listbox horizontal scrolling
	SetListBoxExtent(
		GetDlgItem( ID_AMB_LIST )->m_hWnd,
		// Const 7 == '{' + ',' + ',' + '}' + 1 space + '.' + zero term
		sizeof( BPCONTEXT ) + sizeof( BPLOCATION ) + 7
	);


	C3dDialog::OnInitDialog();

	return TRUE;
}

void CAmbSrc :: DoDataExchange(CDataExchange* pDX) {
	C3dDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAmbSrc)
	DDX_LBIndex(pDX, ID_AMB_LIST, m_iSelect);
	//}}AFX_DATA_MAP
}

int PASCAL AMBResolveSrc( int cslp, int iline, LPSLP rgslp, LPADDR lpaddr ) {
	LPSLP	lpslp;
	int		islp;
	int		cSelect = 0;
	
	// Build table of strings for display
	rgpsz = (char **)malloc( cslp * sizeof( char * ) );
	if ( rgpsz ) {
		csz = cslp;
		memset( rgpsz, 0, cslp * sizeof( char * ) );
		for( islp = 0, lpslp = rgslp; islp < cslp; ++islp, ++lpslp ) {
			CXT				cxt = {0};
			HSYM			hsym;
			LPCH			lst;
			BREAKPOINTNODE	bpn;
			size_t			cb;

			memset( &bpn, 0, sizeof( bpn ) );

			// Setup the default information for the breakpoint.  If we are going to
			// parse and add it to the list, we will get recursive (infinitely) if the
			// LocType isn't LOCADDREXPR!
			bpnLocType( bpn ) = BPLOCFILELINE;

			// Get the bounding context for compare or fill in the BREAKPOINTNDOE package
			SHSetCxt( &lpslp->addr, &cxt );

			// "Exe/Dll"
			_ftcscpy(
				bpnLocCxtExe( bpn ),
				SHGetExeName( SHHexeFromHmod( SHHMODFrompCXT( &cxt ) ) )
			);

			// "Function"
			SHGetNearestHsym(
				&lpslp->addr,
				SHHMODFrompCXT( &cxt ),
				EECODE,
				&hsym
			);

			FnNameFromHsym( hsym, &cxt, (LPSTR)bpnLocCxtFunction( bpn ) );

			if ( !*bpnLocCxtFunction( bpn ) ) {
				ADDR	addr = lpslp->addr;

				// Can't get a function (probably compiled -Zd), convert to
				// address expression only if the address is loaded.
				if (SYFixupAddr( & addr )) {
					if ( ADDR_IS_FLAT( addr ) )
					{
						ASSERT( ADDR_IS_OFF32( addr ) );
						sprintf(
							bpnAddrExpr( bpn ),
							"0x%08lX",
							GetAddrOff( addr )
						);
					}
					else {
						sprintf(
							bpnAddrExpr( bpn ),
							"0x%04X:0x%0*X",
							(unsigned short)GetAddrSeg( addr ),
							ADDR_IS_OFF32( addr ) ? 8 : 4,
							(unsigned short)GetAddrOff( addr )
						);
					}
					bpnLocType( bpn ) = BPLOCADDREXPR;
					*bpnLocCxtFunction( bpn ) = '\0';
					*bpnLocCxtSource( bpn ) = '\0';
					*bpnLocCxtExe( bpn ) = '\0';

				}
			}

			// Copy the REAL OMF file name (SLNameFromHsf returns a length
			// prefixed non-zero terminated string!)
			lst = SLNameFromHsf( SLHsfFromPcxt( &cxt ) );
			memcpy( bpnFileName( bpn ), lst + 1, (size_t)(UCHAR)*lst );
			*( bpnFileName( bpn ) + (UCHAR)*lst ) = '\0';

			if ( bpnLocType( bpn ) == BPLOCADDREXPR ) {
				cb = strlen( bpnAddrExpr( bpn ) );
			}
			else {
				cb = (
					strlen( bpnLocCxtFunction( bpn ) ) +
					strlen( bpnLocCxtSource( bpn ) ) +
					strlen( bpnLocCxtExe( bpn ) )
				);
			}

			// cb required == text + spare room
			if ( rgpsz[ islp ] = (char *)malloc( cb + 30 ) ) {
				if ( bpnLocType( bpn ) == BPLOCADDREXPR ) {
					strcpy( rgpsz[ islp ], bpnAddrExpr( bpn ) );
				}
				else {
					sprintf(
						rgpsz[ islp ],
						"{%s,%s,%s} .%d",
						bpnLocCxtFunction( bpn ),
						bpnLocCxtSource( bpn ),
						bpnLocCxtExe( bpn ),
						iline
					);
				}
			}
		}
	}


	// Do the dialog and free up allocated memory
	if ( rgpsz ) {
		CAmbSrc	as;

		as.m_iSelect = 0;

		GrabFocusFromDebuggee();
		PreModalWindow();

		if ( as.DoModal() == IDOK && as.m_iSelect >= 0 ) {
			*lpaddr = rgslp[ as.m_iSelect ].addr;
			cSelect = 1;
		}

		PostModalWindow();

		for( islp = 0; islp < cslp; ++islp ) {
			if ( rgpsz[ islp ] ) {
				free( rgpsz[ islp ] );
			}
		}
		free( rgpsz );
	}

	return cSelect;
}
