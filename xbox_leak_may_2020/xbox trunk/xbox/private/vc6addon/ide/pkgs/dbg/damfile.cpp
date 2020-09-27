#include "stdafx.h"
#pragma hdrstop

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

#define CMDFMAX         10
MDF             		rgmdf[ CMDFMAX ];

// Index to the next invalid entry
static int      imdfMax = 0;

// If we try to locate a file, we will
// get a callback to save the information
char    szAliasFileName[ _MAX_PATH ];
HSF     hsfAlias;

// When we want to open a file, this is where the file name and
// hsf go when codemgr and askpath try to do the search!
void DAMSaveDebugFile( LPCSTR sz, HSF hsf ) {
	_ftcsncpy( szAliasFileName, sz, sizeof( szAliasFileName ) );
	szAliasFileName[ sizeof( szAliasFileName ) - 1 ] = '\0';
	hsfAlias = hsf;
}

HSF     PASCAL DAMHsfFromFile( const char FAR * lsz ) {
	HSF     hsf = (HSF)NULL;
	int     imdf = 0;

	if (lsz[0]==0)
		return hsf;						// if no name (eg SQL), don't find it

	while( !hsf && imdf < imdfMax ) {
		if ( rgmdf[ imdf ].lszFile &&
			!_ftcsicmp( rgmdf[ imdf ].lszFile, lsz )
		) {
			hsf = rgmdf[ imdf ].hsf;
		}
		++imdf;
	}
	return hsf;
}

// Helper function for SearchFileFromModule.  If the
// hsf is in our list, then we don't want the SFFM()
// to ask the user for the path since they've already
// pressed cancel the first time they saw the file
BOOL PASCAL DAMFOkToAskForPath( HSF hsf ) {
	char            sz[ _MAX_PATH ];
	char FAR *      lpst;
	size_t          cb;
	CDAMView *pView = (CDAMView *)pViewDisassy;

	if ( pView && !pView->OkToAskForFile() )
		return FALSE;
	
	// Build sz name from hsf name
	lpst = SLNameFromHsf( hsf );
	cb = (size_t)*(unsigned char FAR *)lpst;
	memcpy( sz, lpst + 1, cb );
	*( sz + cb ) = '\0';

	// If the hsf is already in the dam window's list
	// then we don't want to ask for the path
	return DAMHsfFromFile( (const char FAR *)sz ) == (HSF)NULL;
}

// Unload an mdf and free up memory associated with it.
void PASCAL DAMUnloadMdf( LPMDF lpmdf ) {
	if ( lpmdf->lszFile ) {
		free( lpmdf->lszFile );
	}

	if ( lpmdf->mpLineToOff ) {
		free( lpmdf->mpLineToOff );
	}

	if ( lpmdf->lpbFile ) {
		free( lpmdf->lpbFile );
	}

	// Zero out ALL of the fields to guarantee a clean slot
	memset( lpmdf, 0, sizeof( MDF ) );
}

void PASCAL DAMCountLinesInFile( LPMDF lpmdf, unsigned long lcb ) {

	// Allocate a buffer which should be more than big enough to handle all
	// of the lfo's for this file.  The only case where this will fail is
	// where there is a file which has more completely blank lines (no spaces,
	// no nothing!).  Once we're done, we will reallocate the buffer to the
	// correct size.  This will free up memory not needed.
	if ( lpmdf->mpLineToOff = (long FAR *)malloc( lcb * sizeof( long ) / 2 ) ) {

		BYTE FAR *      lpb = lpmdf->lpbFile;
		BYTE FAR *      lpbMax = lpb + lcb;
		unsigned long   cln = 0;
		long *          pl = lpmdf->mpLineToOff;
		char *          prev;

		while( lpb && lpb < lpbMax ) {
			*pl++ = (long)( lpb - lpmdf->lpbFile );

			// DBCS enable this!!!
			lpb = (BYTE FAR *)memchr( lpb, '\n', (size_t)( lpbMax - lpb ) );
			if ( lpb ) {
				if ( lpmdf->lpbFile < lpb &&
					*(prev = (char *)_ftcsdec( (char FAR *)lpmdf->lpbFile, (char FAR *)lpb) ) == '\r'
				) {
					*( prev ) = '\0';
				}
				*lpb = '\0';
				lpb = (BYTE FAR *)_ftcsinc( (char FAR *)lpb );
			}
		}
		lpmdf->clnBuf = pl - lpmdf->mpLineToOff;

		// Free up what's not really needed!
		lpmdf->mpLineToOff =
			(long FAR *)realloc(
				lpmdf->mpLineToOff,
				lpmdf->clnBuf * sizeof( long )
			);
	}
}

void PASCAL DAMSetFileFromHSF( LPMDF lpmdf, HSF hsf, BOOL bForce ) {
	char FAR *      lpst;
	size_t          cb;
	char            szFile[ _MAX_PATH ];
	FILE *          pfile;
	BOOL            fCxt;
	CDocument       *pDoc;

	// Zero out the struct
	memset( lpmdf, 0, sizeof( MDF ) );

	// HSF
	lpmdf->hsf = hsf;

	// char FAR *
	lpst = SLNameFromHsf( hsf );
	cb = *(unsigned char FAR *)lpst;
	memcpy( szFile, lpst + 1, cb );
	*( szFile + cb ) = '\0';

	// Read a file into memory.  Assumed that if the file needs
	// to be aliased it has ALREADY been done!

	lpmdf->clnBuf = 0;
	lpmdf->hsf = hsf;

	if ( bForce )
        RetrySearchFile(); // blow any caches so we don't just get last times value.

	if ( !SearchFileFromModule( szFile, &pDoc, hsf, &fCxt, DISASSY_WIN) )
    {
		ASSERT( hsf == hsfAlias );
		_ftcscpy( szFile, szAliasFileName );

		if ( lpmdf->lszFile = (char FAR *)malloc( _ftcslen( szFile ) + 1 ) )
			_ftcscpy( lpmdf->lszFile, szFile );

		// Attempt to open the file (see if it's there!
		if ( szFile[0] && (pfile = fopen( szFile, "r" )) ) {
			unsigned long   lcb;

			// Find out the file length
			fseek( pfile, 0, SEEK_END );
			lcb = ftell( pfile );
			fclose( pfile );

			// Now allocate memory for the buffer
			if ( lpmdf->lpbFile = (BYTE FAR *)malloc( lcb + 1 ) ) {
				int     ioBuf = _open( szFile, _O_RDONLY );

				// should never fail since we just opened/closed a buffered file!
				ASSERT( ioBuf != -1 );

				// Read the entire file into memory and process
				// If there is an EOF marker before the real end of
				// file, _read will report a smaller size, use that!
				lcb = _read( ioBuf, lpmdf->lpbFile, lcb );
				_close( ioBuf );

				// This is added so when we grab the last line, if there's no
				// '\n' at the end, we don't walk off the end of the buffer
				*( lpmdf->lpbFile + lcb ) = '\0';

				DAMCountLinesInFile( lpmdf, lcb );
			}
		}
	}
    else
    {
		CDAMView * pView = (CDAMView *)pViewDisassy;

    	// File couldn't be found and user said so, keep the file name
    	// so we can be smart about multiple HSFs for a specific file
        if ( pView && pView->OkToAskForFile( ) )	// Only if we explicitly asked for the file should we do this.
		{			
			if ( lpmdf->lszFile = (char FAR *)malloc( _ftcslen( szFile ) + 1 ) ) {
				
				_ftcscpy( lpmdf->lszFile, szFile );
			}
		}
		else {
			lpmdf->bRetry = TRUE;
		}
	}
}

LPMDF   DAMLpmdfFromHSF( HSF hsf ) {
	LPMDF   lpmdf = (LPMDF)NULL;

	// Make sure that we're not being asked for a NULL hsf
	if ( hsf ) {
		int                     imdf = 0;

		// Search all entries for the requested item.  It
		// may already be in memory.
		while( imdf < imdfMax && rgmdf[ imdf ].hsf != hsf ) {
			++imdf;
		}

		// If the indicies don't match, we already have the
		// hsf in memory
		if ( imdf != imdfMax ) {
			// If there's no filename, we are currently trying
			// to locate it and probably repainting underneath
			// the dialog!
			if ( !rgmdf[ 0 ].lszFile ) {
				lpmdf = &rgmdf[ imdf ];

				if ( lpmdf->bRetry && ((CDAMView *)pViewDisassy)->OkToAskForFile() )
				{
					DAMUnloadMdf(lpmdf);
					DAMSetFileFromHSF(lpmdf, hsf, TRUE);
				}
			}
			else {
				// Put the current selection at the top of the MRU list
				if ( imdf ) {
					MDF     mdf;

					mdf = rgmdf[ imdf ];
					memmove( &rgmdf[ 1 ], &rgmdf[ 0 ], imdf * sizeof( MDF ) );
					rgmdf[ 0 ] = mdf;
				}
				lpmdf = &rgmdf[ 0 ];
			}
		}

		// Not yet in memory try to process the file
		else {
			MDF             mdf;
			BOOL            fDeleteMdf = FALSE;
			int             imdf;
			BOOL            fFound;
			char            szFile[ _MAX_PATH ];
			char FAR *      lpst;
			size_t          cb;

			// If there are no more slots available,
			// decrement imdfMax so the memmove below
			// will not GPF.  It will get incremented
			// before we exit, so all will be fine.  When
			// this condition is TRUE, we are dropping the
			// LRU off of the list
			if ( imdfMax == CMDFMAX ) {
				mdf = rgmdf[ --imdfMax ];
				fDeleteMdf = TRUE;
			}

			// If imdfMax is non-zero, then we are adding
			// to a non-empty list.  Move all of the entries
			// down a slot before adding new item to the top
			// of the list.  This will give us a MRU list with
			// item [0] being the MRU and item imdfMax-1 being
			// the LRU

			if ( imdfMax ) {
				memmove( &rgmdf[ 1 ], &rgmdf[ 0 ], imdfMax * sizeof( MDF ) );
			}
			++imdfMax;

			// Scan other entries in the list by name (we already searched
			// by hsf).  It's possible that the file had more than 1 hsf,
			// and if there's a duplicate, duplicate the node (NOTE: this
			// should only happen, when the file can't be found!
			imdf = 1;
			fFound = FALSE;

			// char FAR *
			lpst = SLNameFromHsf( hsf );
			cb = *(unsigned char FAR *)lpst;
			memcpy( szFile, lpst + 1, cb );
			*( szFile + cb ) = '\0';

			while( imdf < imdfMax && !fFound ) {
				if ( rgmdf[ imdf ].lszFile &&
					!_ftcsicmp( rgmdf[ imdf ].lszFile, szFile )
				) {
					fFound = TRUE;
				}
				else {
					++imdf;
				}
			}

			// Only to this if there is no file data.
			if ( fFound && !rgmdf[ imdf ].lpbFile ) {
				// Save return value
				lpmdf = &rgmdf[ 0 ];

				// Initialize everything to null and save only the hsf and file name!
				memset( lpmdf, 0, sizeof( MDF ) );
				lpmdf->hsf = hsf;
				if ( lpmdf->lszFile = (char FAR *)malloc( _ftcslen( szFile ) + 1 ) ) {
					_ftcscpy( lpmdf->lszFile, szFile );
				}
			}
			else {
				// Never fail.  If we can't find the file or OOM, keep track
				// of the hsf so we don't ask for it over and over
				// Insert the new item into the MRU slot (==0)
				DAMSetFileFromHSF( lpmdf = &rgmdf[ 0 ], hsf, FALSE );
			}

			// If we need to throw out a node, do it here!
			if ( fDeleteMdf ) {
				DAMUnloadMdf( &mdf );
			}
		}
	}
	return lpmdf;
}

// Called to load an HSF into a buffered slot.
// This will fail if:
//              1) The item is NOT cached AND
//              2) There is an error loading the file (no memory, empty file)
//
BOOL PASCAL DAMFLoadHSFFile( HSF hsf ) {
	LPMDF   lpmdf = DAMLpmdfFromHSF( hsf );

	return lpmdf != (LPMDF)NULL && lpmdf->clnBuf;
}

// Get a textual line from the HSF
BOOL PASCAL DAMFGetLine( HSF hsf, long iln, int cbMax, char * sz, UINT ctchTab ) {
	LPMDF   lpmdf;

	// Make sure that the HSF is in a current slot.  If not try to load it
	if ( !( lpmdf = DAMLpmdfFromHSF( hsf ) ) ) {
		return FALSE;
	}

	// Ensure that the requested line is contained within this version of the
	// file.  it is possible that between the time that the file is loaded
	// and the time it is compiled that the file has had lines deleted.  So
	// we want to make sure that we actually have the line which is in the cv
	// info
	if ( iln - 1 < lpmdf->clnBuf ) {
		char    szBuf[ axMax + 2 ];
		char *  psz = szBuf;
		char *  pszMax = sz + cbMax - 1;
		int     cb;
		int		column = 0;

        // Weird hack to workaround bug #20891
        if ( iln < 1 && pDebugCurr->IsJava() )
            iln = 1;
        // End hack!!!

		memset( szBuf, 0, sizeof( szBuf ) );
		_ftcsncpy(
			szBuf,
			(char *)( lpmdf->lpbFile + lpmdf->mpLineToOff[ iln - 1 ] ),
			sizeof( szBuf ) - 1
		);

		// FUTURE:Make this DBCS aware!!!!
		while( *psz && (cbMax-1) ) {
			if ( *psz == '\t' ) {
				if (cbMax) {
					do {
						*sz++ = ' ';
					} while (++column % ctchTab && cbMax--);
				}

				psz = _ftcsinc( psz );
			}
			else {
				if (cbMax - (cb = _ftclen( psz )) > 0) {
					// The whole character will fit
					_ftccpy( sz, psz );
					sz += cb;
					psz += cb;
					cbMax -= cb;
					++column;
				}
				else {
					break;
				}
			}
		}

		if ( cbMax > 0 ) {
			pszMax = sz;
		}
		*pszMax = '\0';

		return TRUE;
	}
	return FALSE;
}

// Loop through all slots and free up memory/buffers
void PASCAL DAMUnloadAll() {
	while( imdfMax ) {
		DAMUnloadMdf( &rgmdf[ --imdfMax ] );
	}
	hsfAlias = (HSF)NULL;
	szAliasFileName[ 0 ] = '\0';
}
