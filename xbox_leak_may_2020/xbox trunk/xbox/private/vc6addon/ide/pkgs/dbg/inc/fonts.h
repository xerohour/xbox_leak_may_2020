#ifndef _fonts_h_
#define _fonts_h_

#define FIRST_CHAR(lb)		(((lb) << 8) + (0x40))
#define LAST_CHAR(lb)		(((lb) << 8) + (0xFC))
#define NORMALIZE_LEAD(lb)	(unsigned short)((lb) - (0x81))
#define NORMALIZE_TRAIL(tb)	(unsigned short)((tb) - (0x40))

// CFontInfo - holds all the information about a font needed by the view classes to draw text,
// position the caret etc.
class CFontInfo
{
	// Data Members
	public:
		// Font Information
		LOGFONT		m_lf;
		HFONT		m_hFont;
		TEXTMETRIC	m_tm;								
		BOOL 		m_fFixedPitchFont;					// Are we displaying a fixed-pitch font?
		int 		m_rgcxSBWidths[NASCII_WIDTHS];		// Array of ASCII widths (-1 indicates lead byte)
		int 		*m_rgrgcxTrailTables[NLEAD_BYTES];	// Table of pointers (indexed by lead byte)
														// to table of widths (indexed by trail byte)
	// Operations
	private:
		int *rgiLoadTrailWidths(const unsigned char chLead);
		void InitializeFontInfo(const LOGFONT &lf);
		void DestroyFontInfo(void);

	public:
		// Constructor
		CFontInfo(const LOGFONT &lf)
			{ InitializeFontInfo(lf); }

		// Destructor
		~CFontInfo()
			{ DestroyFontInfo(); }

		// Replace the information in a CFontInfo with a new font
		void ChangeFontInfo(const LOGFONT &lf)
			{ DestroyFontInfo(); InitializeFontInfo(lf); }

};

#endif // ! _fonts_h_
