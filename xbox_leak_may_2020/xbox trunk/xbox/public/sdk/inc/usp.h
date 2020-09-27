/*++

   Copyright (c) 1998-1999 Microsoft Corporation.

*/

#ifndef __usp__
#define __usp__
#if _MSC_VER > 1000
#pragma once
#endif

#include <windows.h>
#ifdef __cplusplus
extern "C" {
#endif


/////   USP - Unicode Complex Script processor
//
//      Copyright (c) 1996-8, Microsoft Corporation. All rights reserved.






/////   LANG
//
//      The following defines are temporary - they will be removed once they
//      have been added to the standard NLS header files.



#ifndef LANG_BURMESE
#define LANG_BURMESE     0x55       // Burma
#endif
#ifndef LANG_KHMER
#define LANG_KHMER       0x53       // Cambodia
#endif
#ifndef LANG_LAO
#define LANG_LAO         0x54       // Lao
#endif
#ifndef LANG_MONGOLIAN
#define LANG_MONGOLIAN   0x50       // Mongolia
#endif
#ifndef LANG_TIBETAN
#define LANG_TIBETAN     0x51       // Tibet
#endif
#ifndef LANG_URDU
#define LANG_URDU        0x20       // India / Pakistan
#endif






/////   SCRIPT
//
//      The SCRIPT enum is an opaque type used internally to identify
//      which shaping engine functions are used to process a given run.
//
//
#define SCRIPT_UNDEFINED  0
#define SCRIPT_MAX       47
//
//p     SCRIPT_UNDEFINED: This is the only public script ordinal. May be
//      forced into the eScript field of a SCRIPT_ANALYSIS to disable shaping.
//      SCRIPT_UNDEFINED is supported by all fonts - ScriptShape will display
//      whatever glyph is defined in the font CMAP table, or, if none, the
//      missing glyph.







/////   USP Status Codes
//
#define USP_E_SCRIPT_NOT_IN_FONT   \
        MAKE_HRESULT(SEVERITY_ERROR,FACILITY_ITF,0x200)    // Script doesn't exist in font






/////   SCRIPT_CACHE
//
//      Many script APIs take a combination of HDC and SCRIPT_CACHE parameter.
//
//      A SCRIPT_CACHE is an opaque pointer to a Uniscribe font metric cache
//      structure.


typedef void *SCRIPT_CACHE;


//      The client must allocate and retain one SCRIPT_CACHE variable for each
//      character style used. It must be initialised by the client to NULL.
//
//      APIs are passed an HDC and the address of a SCRIPT_CACHE variable.
//      Uniscribe will first attempt to access font data via the SCRIPT_CACHE
//      and will only inspect the HDC if the required data is not already
//      cached.
//
//      The HDC may be passed as NULL. If data required by Uniscribe is
//      already cached, the HDC won't be accessed and operation continues
//      normally.
//
//      If the HDC is passed as NULL, and Uniscribe needs to access it for
//      any reason, Uniscribe will return E_PENDING.
//
//      E_PENDING is returned quickly, allowing the client to avoid time
//      consuming SelectObject calls. The following example applies to all
//      APIs that take a SCRIPT_CACHE and an optional HDC.
//
//c     hr = ScriptShape(NULL, &sc, ..);
//c     if (hr == E_PENDING) {
//c         ... select font into hdc ...
//c         hr = ScriptShape(hdc, &sc, ...);
//c     }






/////   ScriptFreeCache
//
//      The client may free a SCRIPT_CACHE at any time. Uniscribe maintains
//      reference counts in it's font and shaper caches, and frees font data
//      only when all sizes of the font are free, and shaper data only when
//      all fonts it supports are freed.
//
//      The client should free the SCRIPT_CACHE for a style when it discards
//      that style.
//
//      ScriptFreeCache always sets it's parameter to NULL to help avoid
//      mis-referencing.


HRESULT WINAPI ScriptFreeCache(
    SCRIPT_CACHE   *psc);       //InOut  Cache handle






/////   SCRIPT_CONTROL
//
//      The SCRIPT_CONTROL structure provides itemization control flags to the
//      ScriptItemize function.
//
//
typedef struct tag_SCRIPT_CONTROL {
    DWORD   uDefaultLanguage    :16; // For NADS, also default for context
    DWORD   fContextDigits      :1;  // Means use previous script instead of uPrimaryLanguage

    // The following flags provide legacy support for GetCharacterPlacement features
    DWORD   fInvertPreBoundDir  :1;  // Reading order of virtual item immediately prior to string
    DWORD   fInvertPostBoundDir :1;  // Reading order of virtual item immediately following string
    DWORD   fLinkStringBefore   :1;  // Equivalent to presence of ZWJ before string
    DWORD   fLinkStringAfter    :1;  // Equivalent to presence of ZWJ after string
    DWORD   fNeutralOverride    :1;  // Causes all neutrals to be strong in the current embedding direction
    DWORD   fNumericOverride    :1;  // Causes all numerals to be strong in the current embedding direction
    DWORD   fReserved           :9;
} SCRIPT_CONTROL;
//
//
//p     uDefaultLanguage: Language to use when Unicode values are ambiguous.
//              Used by numeric processing to select digit shape when
//              fDigitShape (see SCRIPT_STATE) is in force.
//
//p     fContextDigits: Specifies that national digits are chosen according to
//              the nearest previous strong text, rather than using
//              uPrimaryLanguage.
//
//p     fInvertPreBoundDir: By default text at the start of the string is
//              laid out as if it follows strong text of the same direction
//              as the base embedding level. Set fInvertPreBoundDir to change
//              the initial context to the opposite of the base embedding
//              level. This flag is for GetCharacterPlacement legacy support.
//
//p     fInvertPostBoundDir: By default text at the end of the string is
//              laid out as if it preceeds strong text of the same direction
//              as the base embedding level. Set fInvertPreBoundDir to change
//              the final context to the opposite of the base embedding
//              level. This flag is for GetCharacterPlacement legacy support.
//
//p     fLinkStringBefore: Causes the first character of the string to be
//              shaped as if were joined to a previous character.
//
//p     fLinkStringAfter: Causes the last character of the string to be
//              shaped as if were joined to a following character.
//
//p     fNeutralOverride: Causes all neutral characters in the string to be
//              treated as if they were strong characters of their enclosing
//              embedding level. This effectively locks them in place,
//              reordering occuring only between neutrals.
//
//p     fNumericOverride: Causes all numeric characters in the string to be
//              treated as if they were strong characters of their enclosing
//              embedding level. This effectively locks them in place,
//              reordering occuring only between numerics.
//
//p     fReserved: Reserved. Always initialise to 0.






/////   SCRIPT_STATE
//
//      The SCRIPT_STATE structure is used both to initialise the unicode
//      algorithm state as an input parameter to ScriptItemize, and is also
//      a component of the analysis returned by ScriptItemize.
//
//
typedef struct tag_SCRIPT_STATE {
    WORD    uBidiLevel         :5;  // Unicode Bidi algorithm embedding level (0-16)
    WORD    fOverrideDirection :1;  // Set when in LRO/RLO embedding
    WORD    fInhibitSymSwap    :1;  // Set by U+206A (ISS), cleared by U+206B (ASS)
    WORD    fCharShape         :1;  // Set by U+206D (AAFS), cleared by U+206C (IAFS)
    WORD    fDigitSubstitute   :1;  // Set by U+206E (NADS), cleared by U+206F (NODS)
    WORD    fInhibitLigate     :1;  // Equiv !GCP_Ligate, no Unicode control chars yet
    WORD    fDisplayZWG        :1;  // Equiv GCP_DisplayZWG, no Unicode control characters yet
    WORD    fArabicNumContext  :1;  // For EN->AN Unicode rule
    WORD    fReserved          :2;
    WORD    fEngineReserved    :2;  // For use by shaping engine
} SCRIPT_STATE;
//
//
//p     uBidiLevel: The embedding level associated with all characters in this
//              run according to the Unicode bidi algorithm. When passed to
//              ScriptItemize, should be initialised to 0 for an LTR base
//              embedding level, or 1 for RTL.
//
//p     fOverrideDirection: TRUE if this level is an override level (LRO/RLO).
//              In an override level, characters are layed out purely
//              left to right, or right to left. No reordering of digits
//              or strong characters of opposing direction takes place.
//
//p     fInhibitSymSwap: TRUE if the shaping engine is to bypass mirroring of
//              Unicode Mirrored glyphs such as brackets. Set by Unicode
//              character ISS, cleared by ASS.
//
//p     fCharShape: TRUE if character codes in the Arabic Presentation Forms
//              areas of Unicode should be shaped. (Not implemented).
//
//p     fDigitSubstitute: TRUE if character codes U+0030 through U+0039
//              (European digits) are to be substituted by national digits.
//              Set by Unicode NADS, Cleared by NODS.
//
//p     fInhibitLigate: TRUE if ligatures ate not to be used in the shaping
//              of Arabic or Hebrew characters.
//
//p     fDisplayZWG: TRUE if control characters are to be shaped as
//              representational glyphs. (Normally, control characters are
//              shaped to the blank glyph and given a width of zero).
//
//p     fArabicNumContext: TRUE indicates prior strong characters were Arabic
//              for the purposes of rule P0 on page 3-19 of 'The Unicode
//              Standard, version 2.0'. Should normally be set TRUE before
//              itemizing an RTL paragraph in an Arabic language, FALSE
//              otherwise.
//
//p     fReserved: Reserved. Always initialise to 0.
//
//p     fEngineReserved: Reserved. Always initialise to 0.






/////   SCRIPT_ANALYSIS
//
//      Each analysed item is described by a SCRIPT_ANALYSIS structure.
//      It also includes a copy of the Unicode algorithm state (SCRIPT_STATE).
//
//
typedef struct tag_SCRIPT_ANALYSIS {
    WORD    eScript         :10;    // Shaping engine
    WORD    fRTL            :1;     // Rendering direction
    WORD    fLayoutRTL      :1;     // Set for GCP classes ARABIC/HEBREW and LOCALNUMBER
    WORD    fLinkBefore     :1;     // Implies there was a ZWJ before this item
    WORD    fLinkAfter      :1;     // Implies there is a ZWJ following this item.
    WORD    fLogicalOrder   :1;     // Set by client as input to ScriptShape/Place
    WORD    fNoGlyphIndex   :1;     // Generated by ScriptShape/Place - this item does not use glyph indices
    SCRIPT_STATE s;
} SCRIPT_ANALYSIS;
//
//
//p     eScript: Opaque value identifying which engine Uniscribe will use to
//              Shape, Place and TextOut this item. The value of eScript is
//              undefined, and will change in future realses, but attributes
//              of eScript may be obtained by calling ScriptGetProperties.
//
//p     fRTL: Rendering direction. Normally identical to the parity of the
//              Unicode embedding level, but may differ if overridden by
//              GetCharacterPlacement legacy support.
//
//p     fLinkBefore: If set, the shaping engine will shape the first character
//              of this item as if it were joining with a previous character.
//              Set by ScriptItemize, may be overriden before calling ScriptShape.
//
//p     fLinkAfter: If set, the shaping engine will shape the last character
//              of this item as if it were joining with a subsequient character.
//              Set by ScriptItemize, may be overriden before calling ScriptShape.
//
//p     fLogicalOrder: If set, the shaping engine will generate all glyph
//              related arrays in logical order. By default glyph related
//              arrays are in visual order, the first array entry corresponding
//              to the leftmost glyph.
//              Set to FALSE by ScriptItemize, may be overriden before calling
//              ScriptShape.
//
//p     fNoGlyphIndex: Usually FALSE. Set TRUE for bitmap, vector and device
//              fonts, and for FE scripts.
/////   SCRIPT_ITEM
//
//      The SCRIPT_ITEM structure includes a SCRIPT_ANALYSIS with the string
//      ofset of the first character of the item.
//
//
typedef struct tag_SCRIPT_ITEM {
    int              iCharPos;      // Logical offset to first character in this item
    SCRIPT_ANALYSIS  a;
} SCRIPT_ITEM;
//
//






/////   ScriptItemize - break text into items
//
//      Breaks a run of unicode into individually shapeable items.
//      Items are delimited by
//
//      o Change of shaping engine
//      o Change of direction
//
//      The client may create multiple runs from each item returned by
//      ScriptItemize, but should not combine multiple items into a single run.
//
//      Later the client will call ScriptShape for each run (when measuring or
//      rendering), and must pass the SCRIPT_ANALYSIS, that ScriptItemize
//      returned.


HRESULT WINAPI ScriptItemize(
    const WCHAR           *pwcInChars,  // In   Unicode string to be itemized
    int                    cInChars,    // In   Character count to itemize
    int                    cMaxItems,   // In   Max length of itemization array
    const SCRIPT_CONTROL  *psControl,   // In   Analysis control (optional)
    const SCRIPT_STATE    *psState,     // In   Initial bidi algorithm state (optional)
    SCRIPT_ITEM           *pItems,      // Out  Array to receive itemization
    int                   *pcItems);    // Out  Count of items processed (optional)






/////
//
//      If psControl, psState and pbBidiStack are NULL on entry, ScriptItemize
//      breaks the unicode string purely by character code.  If they are all
//      non-null, it performs a full Unicode bidi analysis.
//
//      ScriptItemize always adds a terminal item to the item analysis array
//      (pItems) such that the length of an item at pItem is always available as:
//
//c     pItem[1].iCharPos - pItem[0].iCharPos
//
//      For this reason, it is invalid to call ScriptItemize with a buffer
//      of less than two SCRIPT_ANALYSIS items.
//
//      To perform a correct Unicode Bidi analysis, the SCRIPT_STATE should
//      be initialised according to the reading order at paragraph start, and
//      ScriptItemize should be passed the whole paragraph.
//
//      The Bidi stack is not large - just 16 bytes. It should be shared
//      between calls.
//
//      fRTL and fNumeric together provide the same classification as
//      the lpClass output from GetCharacterPlacement.
//
//      If shaping is disabled (fDisableGlyphShape in SCRIPT_STATE), complex
//      scripts are substituted by SCRIPT_SIMPLE (the rendering direction is
//      still set appropriately)
//
//      European digits U+0030 through U+0039 may be rendered as national
//      digits as follows:
//
//t     fDigitSubstitute | FContextDigits | Digit shapes displayed for Unicode U+0030 through U+0039
//t     ---------------- | -------------- | ------------------------------------
//t     False            | Any            | Western (European / American) digits
//t     True             | False          | As specified in SCRIPT_CONTROL.uPrimaryLanguage
//t     True             | True           | As prior strong text, defaulting to SCRIPT_CONTROL.uPrimaryLanguage
//
//      Effect of Unicode control characters on SCRIPT_STATE:
//
//t     SCRIPT_STATE flag | Set by | Cleared by
//t     ----------------- | ------   ----------
//t     fDigitSubstitute  |  NADS  |   NODS
//t     fInhibitSymSwap   |  ISS   |   ASS
//t     fCharShape        |  AAFS  |   IAFS
//
//      SCRIPT_STATE.fArabicNumContext controls the Unicode EN->AN rule. At
//      the beginning of paragraph it should normally be initialised to TRUE
//      for an Arabic locale, FALSE for any other. ScritpItemize will update
//      it as it processes strong text.
/////   ScriptLayout
//
//      The ScriptLayout function converts an array of run embedding levels to
//      a map of visual to logical position, and/or logical to visual position.
//
//      pbLevel must contain the embedding levels for all runs on the line,
//      ordered logically.
//
//      On output, piVisualToLogical[0] is the logical index of the run to
//      display at the far left. Subsequent entries should be displayed
//      progressing from left to right.
//
//      piLogicalToVisual[0] is the relative visual position where the first
//      logical run should be displayed - the leftmost display position being zero.
//
//      The caller may request either piLogicalToVisual or piVisualToLogical
//      or both.
//
//      Note: No other input is required since the embedding levels give all
//      necessary information for layout.


HRESULT WINAPI ScriptLayout(
    int           cRuns,              // In   Number of runs to process
    const BYTE   *pbLevel,            // In   Array of run embedding levels
    int          *piVisualToLogical,  // Out  List of run indices in visual order
    int          *piLogicalToVisual); // Out  List of visual run positions






/////   SCRIPT_JUSTIFY
//
//      The script justification enumeration provides the client with the
//      glyph characteristic information it needs to implement justification.


typedef enum tag_SCRIPT_JUSTIFY {
    SCRIPT_JUSTIFY_NONE           = 0,   // Justification can't be applied at this glyph
    SCRIPT_JUSTIFY_ARABIC_BLANK   = 1,   // This glyph represents a blank in an Arabic run
    SCRIPT_JUSTIFY_CHARACTER      = 2,   // Inter-character justification point follows this glyph
    SCRIPT_JUSTIFY_RESERVED1      = 3,   // Reserved #1
    SCRIPT_JUSTIFY_BLANK          = 4,   // This glyph represents a blank outside an Arabic run
    SCRIPT_JUSTIFY_RESERVED2      = 5,   // Reserved #2
    SCRIPT_JUSTIFY_RESERVED3      = 6,   // Reserved #3
    SCRIPT_JUSTIFY_ARABIC_NORMAL  = 7,   // Normal Middle-Of-Word glyph that connects to the right (begin)
    SCRIPT_JUSTIFY_ARABIC_KASHIDA = 8,   // Kashida(U+640) in middle of word
    SCRIPT_JUSTIFY_ARABIC_ALEF    = 9,   // Final form of Alef-like (U+627, U+625, U+623, U+632)
    SCRIPT_JUSTIFY_ARABIC_HA      = 10,  // Final form of Ha (U+647)
    SCRIPT_JUSTIFY_ARABIC_RA      = 11,  // Final form of Ra (U+631)
    SCRIPT_JUSTIFY_ARABIC_BA      = 12,  // Final form of Ba (U+628)
    SCRIPT_JUSTIFY_ARABIC_BARA    = 13,  // Ligature of alike (U+628,U+631)
    SCRIPT_JUSTIFY_ARABIC_SEEN    = 14,  // Highest priority: Initial shape of Seen(U+633) (end)
    SCRIPT_JUSTIFY_RESERVED4      = 15,  // Reserved #4
} SCRIPT_JUSTIFY;



/////   SCRIPT_VISATTR
//
//      The visual (glyph) attribute buffer generated by ScriptShape
//      identifies clusters and justification points:


typedef struct tag_SCRIPT_VISATTR {
    WORD           uJustification   :4;  // Justification class
    WORD           fClusterStart    :1;  // First glyph of representation of cluster
    WORD           fDiacritic       :1;  // Diacritic
    WORD           fReserved        :4;  // General reserved
    WORD           fShapeReserved   :6;  // Reserved for use by shaping engines
} SCRIPT_VISATTR;
//
//
//p     uJustification: Justification class for this glyph. See SCRIPT_JUSTIFY.
//
//p     fClusterStart: Set for the logically first glyph in every cluster,
//          even for clusters containing just one glyph.
//
//p     fDicritic: Set for glyphs that combine with base characters.


/////   ScriptShape
//
//      The ScriptShape function takes a Unicode run and generates glyphs and
//      visual attributes.


HRESULT WINAPI ScriptShape(
    HDC                 hdc,            // In    Optional (see under caching)
    SCRIPT_CACHE       *psc,            // InOut Cache handle
    const WCHAR        *pwcChars,       // In    Logical unicode run
    int                 cChars,         // In    Length of unicode run
    int                 cMaxGlyphs,     // In    Max glyphs to generate
    SCRIPT_ANALYSIS    *psa,            // InOut Result of ScriptItemize
    WORD               *pwOutGlyphs,    // Out   Output glyph buffer
    WORD               *pwLogClust,     // Out   Logical clusters
    SCRIPT_VISATTR     *psva,           // Out   Visual glyph attributes (Optional)
    int                *pcGlyphs);      // Out   Count of glyphs generated






/////
//
//      Clusters are sequenced uniformly within the run, as are glyphs within
//      the cluster - the fRTL item flag (from ScriptItemize) identifies
//      whether left to right, or right to left.
//
//      ScriptShape updates the flags fFallbackFont and fGlyphIndex in psa.
//
//      If fLogicalOrder is requested in psa, glyphs will be always be
//      generated in the same order as the original Unicode characters.
//      If fLogicalOrder is not set, right to left items are generated in
//      reverse order, so ScriptTextOut does not need to reverse them before
//      calling ExtTextOut.
/////   ScriptPlace
//
//      The ScriptPlace function takes the output of a ScriptShape call and
//      generates glyph advance width and 2D offset information.
//
//      The composite ABC width for the whole item identifies how much the
//      glyphs overhang to the left of the start position and to the right of
//      the length implied by the sum of the advance widths.
//
//      The total advance width of the line is exactly abcA + abcB + abcC.
//
//      abcA and abcC are maintained as proportions of the cell height
//      represented in 8 bits and are thus roughly +/- 1%.
//
//      All arrays are in visual order unless the fLogicalOrder flag is set
//      in psa.


#ifndef LSDEFS_DEFINED
typedef struct tagGOFFSET {
    LONG  du;
    LONG  dv;
} GOFFSET;
#endif


HRESULT WINAPI ScriptPlace(
    HDC                     hdc,        // In    Optional (see under caching)
    SCRIPT_CACHE           *psc,        // InOut Cache handle
    const WORD             *pwGlyphs,   // In    Glyph buffer from prior ScriptShape call
    int                     cGlyphs,    // In    Number of glyphs
    const SCRIPT_VISATTR   *psva,       // In    Visual glyph attributes (Optional)
    const SCRIPT_ANALYSIS  *psa,        // In    Result of ScriptItemize
    int                    *piAdvance,  // Out   Advance wdiths
    GOFFSET                *pGoffset,   // Out   x,y offset for combining glyph
    ABC                    *pABC);      // Out   Composite ABC for the whole run (Optional)






/////   ScriptTextOut
//
//      The ScriptTextOut function takes the output of both ScriptShape and
//      ScriptPlace calls and calls the operating system ExtTextOut function
//      appropriately.
//
//      All arrays are in visual order unless the fLogicalOrder flag is set in
//      psa.


HRESULT WINAPI ScriptTextOut(
    const HDC               hdc,        // In     OS handle to device context (required)
    SCRIPT_CACHE           *psc,        // InOut  Cache handle
    int                     x,          // In     x,y position for first glyph
    int                     y,          // In
    UINT                    fuOptions,  // In     ExtTextOut options
    const RECT             *lprc,       // In     optional clipping/opaquing rectangle
    const SCRIPT_ANALYSIS  *psa,        // In     Result of ScriptItemize
    const WCHAR            *pwcInChars, // In     Required only for metafile DCs
    int                     cChars,     // In     Required only for metafile DCs
    const WORD             *pwGlyphs,   // In     Glyph buffer from prior ScriptShape call
    int                     cGlyphs,    // In     Number of glyphs
    const int              *piAdvance,  // In     Advance widths from ScriptPlace
    const int              *piJustify,  // In     Justified advance widths (optional)
    const GOFFSET          *pGoffset);  // In     x,y offset for combining glyph






/////
//
//      The caller should normally use SetTextAlign(hdc, TA_RIGHT) before
//      calling ScriptTextOut with an RTL item inlogical order.
//
//      The piJustify array provides requested cell widths for each glyph.
//      When the piJustify width of a glyph differs from the unjustified
//      width (in PiAdvance), space is added to or removed from the glyph
//      cell at it's trailing edge. The glyph is always aligned with the
//      leading edge of it's cell. (This rule applies even in visual order.)
//
//      When a glyph cell is extended the extra space is uaually made up by
//      the addition of white space, however for Arabic scripts, the extra
//      space is made up by one or more kashida glyphs, unless the extra space
//      is insufficient for the shortest kashida glyph in the font. (The
//      width of the shortest kashida is available by calling
//      ScriptGetFontProperties.)
//
//      piJustify should only be passed if re-justification of the string is
//      required. Normally pass NULL to this parameter.
//
//      pwcinChars and cChars are only required if output is to a metafile DC.
//      If hdc is not a metafile, they may be passed as NULL and zero.
//
//      fuOptions may contain ETO_CLIPPED or ETO_OPAQUE (or neither or both).
/////   ScriptJustify
//
//      Sophisticated text formatters may prefer to generate their own delta
//      dx array by combining their own features with the information returned
//      by ScriptShape in the SCRIPT_VISATTR array.
//
//      ScriptJustify establishes how much adjustment to make at each glyph
//      position on the line. It interprets the SCRIPT_VISATTR array generated
//      by a call to ScriptShape, and gives top priority to kashida, then uses
//      inter word spacing if there's no kashida points, then uses
//      intercharacter spacing if there are no inter-word points.
//
//      The justified advance widths generated in ScriptJustify should be
//      passed to ScriptTextOut in the piJustify paramter.
//
//      NOTE : Kashida insertion happens at the right of the glyph -visually-.
//      Word and PPT uses this concept as a result, ScriptJustify now generates
//      'piJustify' which indicates kashida insertion to the right (visually) of
//      the glyph-to-be-justified. Any change in the algorithm of where to insert
//      kashida should accompany a change in the corresponding ScriptTextOut handler
//      for the particular script (e.g. ArabicTextOut justification handler if kashida
//      placement algorithm had changed).

HRESULT WINAPI ScriptJustify(
    const SCRIPT_VISATTR  *psva,        // In   Collected visual attributes for entire line
    const int             *piAdvance,   // In   Advance widths from ScriptPlace
    int                    cGlyphs,     // In   Size of all arrays
    int                    iDx,         // In   Desired new line width
    int                    iMinKashida, // In   Minimum length of continuous kashida glyph to generate
    int                   *piJustify);  // Out  Updated advance widths to pass to ScriptTextOut






/////   SCRIPT_LOGATTR
//
//      The SCRIPT_LOGATTR structure describes attributes of logical
//      characters useful when editing and formatting text.
//
//
typedef struct tag_SCRIPT_LOGATTR {
    BYTE    fSoftBreak      :1;     // Potential linebreak point
    BYTE    fWhiteSpace     :1;     // A unicode whitespace character, except NBSP, ZWNBSP
    BYTE    fCharStop       :1;     // Valid cursor position (for left/right arrow)
    BYTE    fWordStop       :1;     // Valid cursor position (for ctrl + left/right arrow)
    BYTE    fInvalid       :1;      // Mark invalid characters in sequence
    BYTE    fReserved       :3;
} SCRIPT_LOGATTR;
//
//
//p     fSoftBreak: It would be valid to break the line in front of this
//              character. This flag is set on the first character of
//              South-East Asian words.
//
//p     fWhiteSpace: This character is one of the many Unicode character
//              that are classified as breakable whitespace.
//
//p     fCharStop: Valid cursor position. Set on most characters, but not
//              on codepoints inside Indian and South East Asian character
//              clusters.
//
//p     fWordStop: Valid position following word advance/retire commonly
//              implemented at ctrl/left-arrow and ctrl/right-arrow.






/////   ScriptBreak
//
//      The ScriptBreak function returns cursor movement and formatting break
//      positions for an item. To support mixed formatting within a single
//      word correctly, ScriptBreak should be passed whole items as returned
//      by ScriptItemize, and not the finer formatting runs.
//
//      ScriptBreak does not perform shaping.
//
//      The logical (unicode) attribute buffer identifies valid cursor
//      positions and linebreaks.


HRESULT WINAPI ScriptBreak(
    const WCHAR            *pwcChars,  // In   Logical unicode item
    int                     cChars,    // In   Length of unicode item
    const SCRIPT_ANALYSIS  *psa,       // In   Result of earlier ScriptItemize call
    SCRIPT_LOGATTR         *psla);     // Out  Logical character attributes






/////   ScriptCPtoX
//
//      The ScriptCPtoX function returns the x offset from the left end
//      (!fLogical) or leading edge (fLogical) of a run to either the leading
//      or the trailing edge of a logical character cluster.
//
//      iCP is the offset of any logical character in the cluster.
//
//      For scripts where the caret may conventionally be placed into the
//      middle of clusters (e.g. Arabic, Hebrew), the returned X may be
//      an interpolated position for any codepoint in the line.
//
//      For scripts where the caret is conventionally snapped to the boundaries
//      of clusters, (e.g. Thai, Indian), the resulting X position will be
//      snapped to the requested edge of the cluster containing CP.


HRESULT WINAPI ScriptCPtoX(
    int                     iCP,        // In   Logical character position in run
    BOOL                    fTrailing,  // In   Which edge (default - leading)
    int                     cChars,     // In   Count of logical codepoints in run
    int                     cGlyphs,    // In   Count of glyphs in run
    const PWORD             pwLogClust, // In   Logical clusters
    const SCRIPT_VISATTR   *psva,       // In   Visual glyph attributes array
    const int              *piAdvance,  // In   Advance widths
    const SCRIPT_ANALYSIS  *psa,        // In   Script analysis from item attributes
    int                    *piX);       // Out  Resulting X position






/////   ScriptXtoCP
//
//      The ScriptXtoCP function converts an x offset from the left end
//      (!fLogical) or leading edge (fLogical) of a run to a logical
//      character position and a flag that indicates whether the X position
//      fell in the leading or the trailing half of the character.
//
//      For scripts where the cursor may conventionally be placed into the
//      middle of clusters (e.g. Arabic, Hebrew), the returned CP may be
//      for any codepoint in the line, and fTrailing will be either zero
//      or one.
//
//      For scripts where the cursor is conventionally snapped to the
//      boundaries of a cluster, the returned CP is always the position of
//      the logically first codepoint in a cluster, and fTrailing is either
//      zero, or the number of codepoints in the cluster.
//
//      Thus the appropriate cursor position for a mouse hit is always the
//      returned CP plus the value of fTrailing.
//
//      If the X positition passed is not in the item at all, the resulting
//      position will be the trailing edge of character -1 (for X positions
//      before the item), or the leading edge of character 'cChars' (for
//      X positions following the item).


HRESULT WINAPI ScriptXtoCP(
    int                     iX,             // In   X offset from left of run
    int                     cChars,         // In   Count of logical codepoints in run
    int                     cGlyphs,        // In   Count of glyphs in run
    const WORD             *pwLogClust,     // In   Logical clusters
    const SCRIPT_VISATTR   *psva,           // In   Visual glyph attributes
    const int              *piAdvance,      // In   Advance widths
    const SCRIPT_ANALYSIS  *psa,            // In   Script analysis from item attributes
    int                    *piCP,           // Out  Resulting character position
    int                    *piTrailing);    // Out  Leading or trailing half flag






/////   Notes on ScriptXtoCP and ScriptCPtoX
//
//      Both functions work only within runs and require the results of a
//      previous ScriptShape call.
//
//      The client must establish which run a given cursor offset or x
//      position is within before passing it to ScriptCPtoX or ScriptXtoCP.
//
//      Cluster information in the logical cluster array is used to share
//      the width of a cluster of glyphs equally among the logical characters
//      they represent.
//
//      For example, the lam alif glyph is divided into four areas: the
//      leading half of the lam, the trailing half of the lam, the leading
//      half of the alif and the trailing half of the alif.
//
//      ScriptXtoCP Understands the caet position conventions of each script.
//      For Indian and Thai, caret positions are snapped to cluster boundaries,
//      for Arabic and Hebrew, caret positions are interpolated with clusters.
//
//
/////   Translating mouse hit 'x' offset to caret position
//
//      Conventionally, caret position 'cp' may be selected by clicking either
//      on the trailing half of character 'cp-1' or on the leading half of
//      character 'cp'. This may easily be implemented as follows:
//
//c     int iCharPos;
//c     int iCaretPos
//c     int fTrailing;
//
//c     ScriptXtoCP(iMouseX, ..., &iCharPos, &fTrailing);
//c     iCaretPos = iCharPos + fTrailing;
//
//      For scripts that snap the caret to cluster boundaries, ScriptXtoCP
//      returns ftrailing set to either 0, or the width of the cluster in
//      codepoints.
//
//
/////   Displaying the caret in bidi strings
//
//      In unidirectional text, the leading edge of a character is at the same
//      place as the trailing edge of the previous character, so there is no
//      ambiguity in placing the caret between characters.
//
//      In bidirectional text, the caret position between runs of opposing
//      direction may be ambiguous.
//
//      For example in the left to right paragraph 'helloMAALAS', the last
//      letter of 'hello' immediately preceeds the first letter of 'salaam'.
//      The best position to display the caret depends on whether it is
//      considered to follow the 'o' of 'hello', or to preceed the 's' of
//      'salaam'.
//
//      Cursor positioning conventions (with thanks to Michael Jochimsen)
//
//t     Situation       | Visual caret placement
//t     ---------       | -------------------------------------------
//t     Typing          | Trailing edge of last character typed
//t     Pasting         | Trailing edge of last character pasted
//t     Caret advancing | Trailing edge of last character passed over
//t     Caret retiring  | Leading edge of last character passed over
//t     Home            | Leading edge of line
//t     End             | Trailing edge of line
//
//      The caret may be positioned as follows:
//
//c     if (advancing) {
//c         ScriptCPtoX(iCharPos-1, TRUE, ..., &iCaretX);
//c     } else {
//c         ScriptCPtoX(iCharPos, FALSE, ..., &iCaretX);
//c     }
//
//      Or, more simply, given an fAdvancing BOOL restricted to TRUE or FALSE:
//
//c     ScriptCPtoX(iCharPos-fAdvancing, fAdvancing, ..., &iCaretX);
//
//      ScriptCPtoX handles out of range positions logically: it returns the
//      leading edge of the run for iCharPos <0, and the trailing edge of the
//      run for iCharPos >=length.
/////   ScriptApplyLogicalWidth
//
//      Accepts an array of advance widths in logical order, corresponding
//      one to one with codepoints, and generates an array of glyph widths
//      suitable for passing to the piJustify parameter of ScriptTextOut.


HRESULT WINAPI ScriptApplyLogicalWidth(
    const int              *piDx,        // In     Logical dx array to apply
    int                     cChars,      // In     Count of logical codepoints in run
    int                     cGlyphs,     // In     Glyph count
    const WORD             *pwLogClust,  // In     Logical clusters
    const SCRIPT_VISATTR   *psva,        // In     Visual attributes from ScriptShape/Place
    const int              *piAdvance,   // In     Glyph advance widths from ScriptPlace
    const SCRIPT_ANALYSIS  *psa,         // In     Script analysis from item attributes
    ABC                    *pABC,        // InOut  Updated item ABC width
    int                    *piJustify);  // Out    Resulting glyph advance widths for ScriptTextOut






/////   ScriptGetCMap
//
//      Return glyph indices of Unicode characters according to Truetype
//      Cmap table, or standard Cmap implemented for old style fonts.


#define SGCM_RTL  0x00000001      // Return mirrored glyph for mirrorable Unicode codepoints


HRESULT WINAPI ScriptGetCMap(
    HDC             hdc,            // In    Optional (see notes on caching)
    SCRIPT_CACHE   *psc,            // InOut Address of Cache handle
    const WCHAR    *pwcInChars,     // In    Unicode codepoint(s) to look up
    int             cChars,         // In    Number of characters
    DWORD           dwFlags,        // In    Flags such as SGCM_RTL
    WORD           *pwOutGlyphs);   // Out   Array of glyphs, one per input character






/////   SCRIPT_PROPERTIES
//
typedef struct {
    DWORD   langid              :16;  // Primary and sublanguage associated with script
    DWORD   fNumeric            :1;
    DWORD   fComplex            :1;
    DWORD   fNeedsWordBreaking  :1;   // Requires ScriptBreak for word breaking information
    DWORD   fNeedsCaretInfo     :1;   // Requires ScriptBreak for caret placement information
    DWORD   bCharSet            :8;   // Charset to use when creating font
    DWORD   fControl            :1;   // Control characters
    DWORD   fPrivateUseArea     :1;   // This item is from the Unicode range U+E000 through U+F8FF
    DWORD   iReserved           :2;
} SCRIPT_PROPERTIES;


/////   ScriptGetProperties
//
//      ScriptGetProperties returns the address of a table that maps a
//      script in a SCRIPT_ANALYSIS uScript field to properties including
//      the primary language associated with that script, whether it's
//      numeric and whether it's complex.


HRESULT WINAPI ScriptGetProperties(
    const SCRIPT_PROPERTIES ***ppSp,             // Out  Receives pointer to table of pointers to properties indexed by script
    int                       *piNumScripts);    // Out  Receives number of scripts (valid values are 0 through NumScripts-1)






/////   SCRIPT_FONTPROPERTIES
//
typedef struct {
    int     cBytes;         // Structure length
    WORD    wgBlank;        // Blank glyph
    WORD    wgDefault;      // Glyph used for Unicode values not present in the font
    WORD    wgInvalid;      // Glyph used for invalid character combinations (especially in Thai)
    WORD    wgKashida;      // Shortest continuous kashida glyph in the font, -1 if doesn't exist
    int     iKashidaWidth;  // Widths of shortest continuous kashida glyph in the font
} SCRIPT_FONTPROPERTIES;


/////   ScriptGetFontProperties
//
//      Returns information from the font cache


HRESULT WINAPI ScriptGetFontProperties(
    HDC                     hdc,    // In    Optional (see notes on caching)
    SCRIPT_CACHE           *psc,    // InOut Address of Cache handle
    SCRIPT_FONTPROPERTIES  *sfp);   // Out   Receives properties for this font






/////   ScriptCacheGetHeight
//
//


HRESULT WINAPI ScriptCacheGetHeight(
    HDC            hdc,         // In    Optional (see notes on caching)
    SCRIPT_CACHE  *psc,         // InOut Address of Cache handle
    long          *tmHeight);   // Out   Receives properties for this font






/////   SCRIPT_STRING_ANALYSIS
//
//      This structure provides all parameters required for script analysis.
//
//
#define MAX_FONT 10                 // Max fallback fonts including users font (Cannot exceed 31)


typedef struct tag_SCRIPT_STRING_ANALYSIS {

// Input variables - Initialised by the caller

    HDC             hdc;            // Only required for shaping (GCP_Ligate && lpOrder or lpGlyphs arrays specified)

    DWORD           dwFlags;        // See ScriptStringAnalyse

    // Input buffers

    WCHAR          *pwInChars;      // Unicode input string
    int             cInChars;       // String length

    int             iMaxExtent;     // Required maximum pixel width (used if clipping or fitting)
    const int      *piDx;           // Logical advance width array

    SCRIPT_CONTROL  sControl;
    SCRIPT_STATE    sState;

    int             cMaxItems;      // Number of entries in pItems
    SCRIPT_ITEM    *pItems;

    // Low cost analysis output buffers
    // No shaping required when fLigate=FALSE
    // Must be at least as long as the input string

    BYTE           *pbLevel;        // Array of item level
    int            *piVisToLog;     // Visual to Logical mapping
    WORD           *pwLeftGlyph;    // Leftmost glyph of each logical item
    WORD           *pwcGlyphs;      // Count of glyphs in each logical item

    SCRIPT_LOGATTR *pLogAttr;       // Cursor points, word and line breaking (indexed in logical order)

    // High cost analysis output buffers
    // Require hDC to be set
    // Must be at least nGlyphs long.

    int             cMaxGlyphs;     // Max glyphs to create
    WORD           *pwGlyphs;       // Output glyph array
    WORD           *pwLogClust;     // logical to visual mapping
    SCRIPT_VISATTR *pVisAttr;       // Justification insertion points (visual order) and other flags
    int            *piAdvance;      // Advance widths
    int            *piJustify;      // Justified advance widths
    GOFFSET        *pGoffset;       // x,y combining character offsets


    // Font fallback

    DWORD           dwFallbacksUsed;// Bitmap of fallback fonts used
    int            *piFallback;     // Fallback font indices (0 if script doesnt use a fallback font)
    SCRIPT_CACHE   *psc[MAX_FONT];  // Script cache for each fallback, [0] is users font
    HFONT           hf[MAX_FONT];   // Handles to fallback fonts, [0] is users font
    int             iCurFont;       // 0 For users font

// Output variables


    // Item analysis

    int             cItems;        // Number of items analysed == Index of terminal (sentinel) item in pItem


    // Generated glyphs and character measurements
    // Note that
    //  1) nOutGlyphs may be more or less than nInChars.
    //  2) nOutChars may be less than nInChars if fClip was requested.

    int             cOutGlyphs;     // Number of glyphs generated
    int             cOutChars;      // Number of characters generated
    ABC             abc;
    SIZE            size;           // Size of whole line (pixel width and height)

    // For client use

    void           *pvClient;

} SCRIPT_STRING_ANALYSIS;
//
//
//
//
//
#define SSA_PASSWORD    0x00000001  // Input string contains a single character to be duplicated iLength times
#define SSA_TAB         0x00000002  // Expand tabs
#define SSA_CLIP        0x00000004  // Clip string at iReqWidth
#define SSA_FIT         0x00000008  // Justify string to iReqWidth
#define SSA_DZWG        0x00000010  // Provide representation glyphs for control characters
#define SSA_FALLBACK    0x00000020  // Use fallback fonts
#define SSA_BREAK       0x00000040  // Return break flags (character and word stops)
#define SSA_GLYPHS      0x00000080  // Generate glyphs, positions and attributes
#define SSA_RTL         0x00000100  // Base embedding level 1
#define SSA_GCP         0x00000200  // Return missing glyphs with GetCharacterPlacement conventions

#define SSA_PIDX        0x10000000  // Internal
#define SSA_LAYOUTRTL   0x20000000  // Internal
#define SSA_KERN        0x40000000  // Not implemented
#define SSA_NOKASHIDA   0x80000000  // Not implemented
//
//      Implemented separately:
//
//      o  Overriding logical input widths
//      o  Generation of legacy GetCharacterPlacement output classes
//      o  Generation of reordered unicode string
//      o  Generation of indices of logical ch positions in visual output array
//         (Applies to pwOutGlyphs, or pwOutStr if pwOutGlyphs not present)
//      o  Caret position array


HRESULT WINAPI ScriptStringAnalyse(
    HDC                      hdc,       //In  Device context (required)
    const void              *pString,   //In  String in 8 or 16 bit characters
    int                      cString,   //In  Length in characters
    int                      cGlyphs,   //In  Required glyph buffer size (default cString*3/2 + 1)
    int                      iCharset,  //In  Charset if an ANSI string, -1 for a Unicode string
    DWORD                    dwFlags,   //In  Analysis required
    int                      iReqWidth, //In  Required width for fit and/or clip
    SCRIPT_CONTROL          *psControl, //In  Analysis control (optional)
    SCRIPT_STATE            *psState,   //In  Analysis initial state (optional)
    const int               *piDx,      //In  Requested logical dx array
    const int               *piTabstop, //In  Tab positions (optional)
    const BYTE              *pbInClass, //In  Legacy GetCharacterPlacement character classifications (deprecated)

    SCRIPT_STRING_ANALYSIS **ppsa);     //Out Analysis of string






/////   ScriptStringFree - free a string analysis
//
//


HRESULT WINAPI ScriptStringFree(
    SCRIPT_STRING_ANALYSIS **ppsa);  //InOut Address of pointer to analysis






/////   ScriptStringGetLogicalWidths
//
//      Converts visual withs in psa->piAdvance into logical widths,
//      one per original character, in logical order.
//
//      Requires SSA_GLYPHS requested in original ScriptStringAnalyse call.


HRESULT WINAPI ScriptStringGetLogicalWidths(
    SCRIPT_STRING_ANALYSIS  *psa,
    int                     *piDx);






/////   ScriptStringCPtoX
//
//      Return x coordinate for leading or trailing edge of character icp.


HRESULT WINAPI ScriptStringCPtoX(
    SCRIPT_STRING_ANALYSIS *psa,        //In  String analysis
    int                     icp,        //In  Caret character position
    BOOL                    fTrailing,  //In  Which edge of icp
    int                    *pX);        //Out Corresponding x offset






/////   ScriptStringXtoCP
//
//


HRESULT WINAPI ScriptStringXtoCP(
    SCRIPT_STRING_ANALYSIS *psa,
    int      iX,
    int     *piCh,
    int     *piTrailing);






/////   ScriptStringOut
//
//      Displays a string with InternalStringOut, then adds
//      highlighting corresponding to a logical selection.


HRESULT WINAPI ScriptStringOut(
    SCRIPT_STRING_ANALYSIS *psa,    // Analysis with glyphs
    int           iX,
    int           iY,
    UINT          uOptions,         // ExtTextOut options
    const RECT   *prc,              // Clipping rectangle (iff ETO_CLIPPED)
    int           iMinSel,          // Logical selection. Set iMinSel>=iMaxSel for no selection
    int           iMaxSel,
    BOOL          fDisabled);       // If disabled, only the background is highlighted.






/////   UspAllocMem
//
//      Provided mainly for used by the NT5 language pack DLL


HRESULT WINAPI UspAllocMem(
    int     iSize,              // In   required size in bytes
    void  **ppv);               // Out  Allocated address






/////   UspFreeMem
//
//      Provided mainly for used by the NT5 language pack DLL


HRESULT WINAPI UspFreeMem(
    void  *pv);                 // In   memory to be freed






#ifdef __cplusplus
}
#endif
#endif
