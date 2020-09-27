//****************************************************************************
//
// XBox true-type font scan converter library
//
// History:
//
//   07/06/00 [andrewso] - Created
//   08/04/00 [andrewso] - Added compressed glyph bitmaps
//
//****************************************************************************

#include "std.h"
#if 0
#ifdef XBOX
#include <xtl.h>
#else 
#include <windows.h>
#endif
#endif

#include <assert.h>
#include <d3d8.h>

#include "xfont.h"
#include "xfontformat.h"
#include "font.h"

//****************************************************************************
// Definitions.
//****************************************************************************

//
// Global variables.
//

// The current font.
Font *s_pFont;

//
// Forwards.
//

HRESULT __stdcall GenerateFontMetrics();

HRESULT __fastcall XFONT_GetCharacterData(WCHAR, Glyph **, unsigned *);

// Cache methods.
HRESULT __stdcall InitializeCache(unsigned cbCacheSize);
static BOOL CheckInCache(WCHAR wch, Glyph **ppGlyph, unsigned *pcbGlyphSize);
static void FreeCacheSpace(unsigned cbSizeNeeded);

//****************************************************************************
// The APIs
//****************************************************************************

//============================================================================
// Loads a bitmap font of the format defined in xfontformat.h.
//
// This method always allocates ~112 bytes for each font to store the
// font's state.  Additional memory will be allocated depending on the
// value of the uCacheSize parameter:
//
//    0                            - an allocation to hold the bitmap for the 
//                                   last drawn glyph.  The glyph information
//                                   is ready directly out of the file. 
//                                   Very slow.

//    uCacheSize < font file size  - a block of memory of size uCacheSize 
//                                   holds the most recently drawn glyphs.  
//                                   The glyph information is ready directly 
//                                   out of the file.  This can perform 
//                                   reasonably if the cache is sized
//                                   correctly as determined by experimentation.
//
//    uCacheSize >= font file size - a block of memory just large enough to 
//                                   hold the contents of the file will be 
//                                   allocated and the entire font file will 
//                                   be loaded into it.  Very fast.
//
// This automatically sets the opened font as the one that is currently
// active.  The attributes of the font are all set to their defaults.
//============================================================================

HRESULT __stdcall XFONT_OpenBitmapFont
(
    LPCWSTR wszFileName,    // [in] The file name of the font
	unsigned uCacheSize,	// [in] The size of the font cache, in bytes.
    XFONT **ppFont          // [out] The font identifier
)
{
	HRESULT hr;
	Font *pOldFont = s_pFont;
	BOOL fUseCache;

	assert(wszFileName);

	// Initialize the true type package.
	hr = BP_OpenBitmapFont(wszFileName, uCacheSize, &s_pFont, &fUseCache);

	if (FAILED(hr))
	{
		goto Error;
	}

    if (fUseCache)
    {
        hr = InitializeCache(uCacheSize);

        if (FAILED(hr))
        {
            goto Error;
        }
    }

	// Reset the font metrics.
	hr = GenerateFontMetrics();

	if (FAILED(hr))
	{
		goto Error;
	}

	// Set the font style defaults.
	s_pFont->uAlignmentMode   = XFONT_TOP | XFONT_LEFT;
	s_pFont->TextColor		  = 0xFFFFFFFF;
	s_pFont->BackgroundColor  = 0x00000000;
	s_pFont->fPaintBackground = FALSE;

	// Return it.
	if (ppFont)
	{
		*ppFont = (XFONT *)s_pFont;
	}

	return NOERROR;

Error:
	XFONT_UnloadFont(s_pFont);
	s_pFont = pOldFont;

	return hr;
}

//============================================================================
// XFONT_OpenTrueTypeFont is defined in 'TrueType.cpp' to make the linker
// happy when "xfont.lib" is consumed without the truetype support.
//============================================================================

//============================================================================
// Change the currently active font.  This does not change any of the
// attributes of the font that were set when it was last active.
//============================================================================

void __stdcall XFONT_SelectFont
(
	XFONT *pFont			// [in] The id of the font
)
{
	s_pFont = (Font *)pFont;
}

//============================================================================
// Unload a font and free all of its memory.  The XFONT identifier will be 
// invalidated.
//============================================================================

void __stdcall XFONT_UnloadFont
(
	XFONT *pXFont		// [in] The id of the font to unload
)
{
    if (pXFont)
    {
	    Font *pFont = (Font *)pXFont;
        BYTE *pbCache = pFont->pbCache;

	    // Let the particular font package clean up.  This will free the
	    // Font structure.
	    //
	    pFont->pfnUnloadFont(pFont);

        // Free the cache.
        free(pbCache);
    }
}

//============================================================================
// Sets the height of the text cell, in pixels, for the current font.
//
// Defaults to 16 pixels high.  This may only be set on a TrueType font.
//============================================================================

HRESULT __stdcall XFONT_SetTextHeight
(
	unsigned uHeight		// [in] The total height of the text cell
)
{
	assert(s_pFont);

	if ((uHeight << 16) != s_pFont->uCellHeight)
	{
		HRESULT hr;

		unsigned uOldCellHeight = s_pFont->uCellHeight;

		// Tell the font engine.
		s_pFont->uCellHeight = uHeight;

		hr = s_pFont->pfnResetTransform(s_pFont);

		if (FAILED(hr))
		{
			s_pFont->uCellHeight = uOldCellHeight;

			return hr;
		}

		// Regenerate the height information.
		hr = GenerateFontMetrics();

		if (FAILED(hr))
		{
			return hr;
		}
	}

	return NOERROR;
}

//============================================================================
// Sets the amount of antialias information to include in the bitmap with
// valid values of 0 (no information), 2, or 4 (lots of information).
//
// The antialiasing works best for larger or bold fonts whose strokes are 
// more the one pixel wide.  
//
// This may only be set on a TrueType font.  Defaults to 0.
//============================================================================

HRESULT __stdcall XFONT_SetTextAntialiasLevel
(
	unsigned uAntialiasLevel
)
{
	assert(s_pFont);

	if (uAntialiasLevel != 0 && uAntialiasLevel != 2 && uAntialiasLevel != 4)
	{
		return E_INVALIDARG;
	}

	if (s_pFont->uAntialiasLevel != uAntialiasLevel)
	{
		HRESULT hr;

		unsigned uOldAntialiasLevel = s_pFont->uAntialiasLevel;

		s_pFont->uAntialiasLevel = uAntialiasLevel;

		hr = s_pFont->pfnResetTransform(s_pFont);

		if (FAILED(hr))
		{
			s_pFont->uAntialiasLevel = uOldAntialiasLevel;
		}

		return hr;
	}
	else
	{
		return NOERROR;
	}
}

//============================================================================
// Sets the size of the RLE packet used to compress the internal bitmaps
// used by the font.  This may effect the number of bitmaps that can fit
// in the internal cache depending on the size and characteristics of your
// font.  May be 2 - 8.
//
// This may only be set on a TrueType font.  Defaults to 2.
//============================================================================

HRESULT __stdcall XFONT_SetRLEWidth
(
	unsigned uRLEWidth
)
{
	assert(s_pFont);

	if (uRLEWidth < 2 || uRLEWidth > 8)
	{
		return E_INVALIDARG;
	}

	if (s_pFont->uRLEWidth != uRLEWidth)
	{
		HRESULT hr;

		unsigned uOldRLEWidth = s_pFont->uRLEWidth;

		s_pFont->uRLEWidth = uRLEWidth;

		hr = s_pFont->pfnResetTransform(s_pFont);

		if (FAILED(hr))
		{
			s_pFont->uRLEWidth = uOldRLEWidth;
		}

		return hr;
	}
	else
	{
		return NOERROR;
	}
}

//============================================================================
// Sets the style for the text, one of NORMAL, BOLD, ITALICS or BOLDITALICS.
// This will cause the TrueType font scaler to simulate these styles when
// generating the bitmaps for the font.  If the font defines the different
// styles in separate TTF files then load each individually instead of 
// using this method.
//
// Defaults to NORMAL.  This may only be set on a TrueType font.
//============================================================================

HRESULT __stdcall XFONT_SetTextStyle
(
	unsigned uStyle
)
{
    assert(s_pFont);

    if (uStyle > XFONT_BOLDITALICS)
    {
        return E_INVALIDARG;
    }

    if (s_pFont->uStyle != uStyle)
    {
        HRESULT hr;

        unsigned uOldStyle = s_pFont->uStyle;

        s_pFont->uStyle = uStyle;

        hr = s_pFont->pfnResetTransform(s_pFont);

        if (FAILED(hr))
        {
            s_pFont->uStyle = uOldStyle;
        }

        return hr;
    }
    else
    {
        return NOERROR;
    }
    }

//============================================================================
// Sets to what part of the text the coordinates passed to XFONT_TextOut refers.
//
// The only parameter must contain one vertical value ORed with one 
// horizontal value.
//
// The vertical alignment value specifies which part of the text is pointed
// to by the y coordinate:
//
//		XFONT_BASELINE  - the baseline of the text
//		XFONT_BOTTOM	- the bottom of the character cell
//      XFONT_TOP       - the top of the character cell
//
// The horizontal alignment value specifies which part of the text is pointed
// to by the x coordinate:
//
//		XFONT_CENTER	- the center of the text
//		XFONT_LEFT 	    - the leftmost edge of the text
//      XFONT_RIGHT	    - the rightmost edge of the text
//
// Defaults to TA_TOP | TA_LEFT.
//============================================================================

void __stdcall XFONT_SetTextAlignment
(
	unsigned uMode
)
{
	assert(s_pFont);

	// UNDONE: validate this parameter.
	s_pFont->uAlignmentMode = uMode;	
}

//============================================================================
// Sets the color for the text.  
//
// Defaults to 0x00FFFFFF (white)
//============================================================================

void __stdcall XFONT_SetTextColor
(
	D3DCOLOR color			// [in] The color of the text
)
{
	assert(s_pFont);

	s_pFont->TextColor = color;
}

//============================================================================
// Sets the color with which to fill the background of the text.  
//
// Defaults to 0x00000000 (black)
//============================================================================

void __stdcall XFONT_SetBkColor
(
	D3DCOLOR color			// [in] The color of the text
)
{
	assert(s_pFont);

	s_pFont->BackgroundColor = color;
}

//============================================================================
// Sets whether to fill the background of the text cell with the background
// color.
//
// Defaults to XFONT_TRANSPARENT because it is more efficient.
//============================================================================

void __stdcall XFONT_SetBkMode
(
	int iBkMode				// [in] Either XFONT_OPAQUE or XFONTTRANSPARENT
)
{
   	assert(s_pFont);

	s_pFont->fPaintBackground = iBkMode == XFONT_OPAQUE;
}

//============================================================================
// Sets the number of additional pixels to put between each character.
//============================================================================

void __stdcall XFONT_SetIntercharacterSpacing
(
	unsigned uSpaces		// [in] The number of extra pixels between each character
)
{
	assert(s_pFont);

	s_pFont->uIntercharacterSpacing = uSpaces;
}

//============================================================================
// Get the vertical metrics for the current font settings.  
//============================================================================

void __stdcall XFONT_GetFontMetrics
(
	unsigned *puCellHeight,		
	unsigned *puDescent
)
{
	assert(s_pFont);

	if (puCellHeight)
	{
		*puCellHeight = s_pFont->uCellHeight;
	}

	if (puDescent)
	{
		*puDescent = s_pFont->uDescent;
	}
}

//============================================================================
// Sets or clears the suface to which to draw the text via XFONT_TextOut.
//============================================================================
/*
void __stdcall XFONT_SetDrawingSurface
(
	IDirect3DSurface8 *pSurface		// [in] The surface to draw the text on or NULL
	                                //      release the reference count on the last
									//      set surface.
)
{
	assert(s_pFont);

	if (s_pFont->pSurface)
	{
		IDirect3DSurface8_Release(s_pFont->pSurface);
		s_pFont->pSurface = NULL;
	}

	s_pFont->pSurface = pSurface;

	if (pSurface)
	{
		IDirect3DSurface8_AddRef(pSurface);

		(void)IDirect3DSurface8_GetDesc(pSurface, &s_pFont->SurfaceDesc);
	}
}
*/
//============================================================================
// Get the width of a string in pixels. This method applies any 
// intercharacter spacing.
//============================================================================

HRESULT __stdcall XFONT_GetTextExtent
(
	LPCWSTR wstr,			// [in] The string
	unsigned cch,			// [in] The length of the string, -1 for a zero terminated string
	unsigned *puWidth		// [out] The width of the string in pixels
)
{
	HRESULT hr;

	const unsigned uIntercharacterSpacing = s_pFont->uIntercharacterSpacing;

	Glyph *pGlyph;
    unsigned cbGlyphSize;

	unsigned ich = 0;
	unsigned uWidth = 0;
    
	assert(s_pFont);

	for (;;)
	{
		hr = XFONT_GetCharacterData(*wstr, &pGlyph, &cbGlyphSize);

		if (FAILED(hr)) 
		{
			return hr;
		}

		uWidth += pGlyph->uAdvance;

		// Increment.
		ich++;
		wstr++;

		// Stop?
		if (ich == cch || !*wstr)
		{
			break;
		}

		// Add the intercharacter space.
		uWidth += uIntercharacterSpacing;
	}

	*puWidth = uWidth;

	return NOERROR;
}

//============================================================================
// Draw the text on a surface.  See XFONT_SetTextAlignment for the meaning of
// the x and y coordinates.
//============================================================================

HRESULT __stdcall XFONT_TextOut2
(
	D3DLOCKED_RECT* pLock,	// [in] The surface info
	D3DFORMAT Format,		// [in] The surface format
	long cxClip,			// [in] The surface width
	long cyClip,			// [in] The surface height
	LPCWSTR wstr,			// [in] The string
	unsigned cch,			// [in] The length of the string, -1 for a zero terminated string
	long x,					// [in] The x coordinate of the string
	long y					// [in] The y coordinate of the string
)
{
	HRESULT hr;

	long lCharacterX;
	long lCharacterY;

	//
	// Figure out where to start drawing from.  The rendering package
	// always wants the coordinates of the baseline of the glyph
	// so figure out how we need to munge the values.
	//

	unsigned uVerticalMask = XFONT_BASELINE | XFONT_BOTTOM | XFONT_TOP;
	unsigned uHorizontalMask = XFONT_LEFT | XFONT_CENTER | XFONT_RIGHT;

	assert(s_pFont);
//	assert(s_pFont->pSurface);

	// Vertical munging.
	switch(s_pFont->uAlignmentMode & uVerticalMask)
	{
	default:
		assert(FALSE);     // How did this get through our checks?

		// fall through
	case XFONT_TOP:
		lCharacterY = y;
		break;

	case XFONT_BASELINE:
		lCharacterY = y - (s_pFont->uCellHeight - s_pFont->uDescent);
		break;

	case XFONT_BOTTOM:
		lCharacterY = y - s_pFont->uCellHeight;
		break;
	}

	// Horizontal munging.
	if ((s_pFont->uAlignmentMode & uHorizontalMask) == XFONT_LEFT)
	{
		lCharacterX = x;
	}
	else
	{
		unsigned uWidth;

		hr = XFONT_GetTextExtent(wstr, cch, &uWidth);

		if (FAILED(hr))
		{
			return hr;
		}

		if ((s_pFont->uAlignmentMode & uHorizontalMask) == XFONT_CENTER)
		{
			lCharacterX = x - uWidth / 2;
		}
		else if ((s_pFont->uAlignmentMode & uHorizontalMask) == XFONT_RIGHT)
		{
			lCharacterX = x - uWidth;
		}
		else
		{
			assert(FALSE);

			lCharacterX = x;
		}
	}

	// Draw the string. Just clip to the surface size for now.
	hr = PaintText(pLock,
                   Format,
                   s_pFont, 
			       wstr, 
				   cch, 
				   lCharacterX, 
				   lCharacterY, 
				   0,
				   0,
				   cxClip/*s_pFont->SurfaceDesc.Width*/,
				   cyClip/*s_pFont->SurfaceDesc.Height*/);

	return hr;
}

//****************************************************************************
// This method isn't really exposed in the header file.  It isn't static 
// because the makefont utility calls it when building the bitmap glyph
// information.
//****************************************************************************

//============================================================================
// Gets the character data from the current font, regardless of whether 
// it is a TrueType or bitmap font.
//
// The information returned by this method is only guarenteed to be valid
// until the next call to the font package.  This stuff might be stored
// in a cache and get flushed with the next call.
//============================================================================

HRESULT __fastcall XFONT_GetCharacterData
(
	WCHAR wch, 
	Glyph **ppGlyph,
    unsigned *pcbGlyphSize
)
{
    assert(ppGlyph);

    // Check the cache first unless we don't have one.
    if (s_pFont->pbCache && CheckInCache(wch, ppGlyph, pcbGlyphSize))
    {
        return NOERROR;
    }

	return s_pFont->pfnGetCharacterData(s_pFont, wch, ppGlyph, pcbGlyphSize);
}

//****************************************************************************
// Implementation
//****************************************************************************

//============================================================================
// Figure out the vertical height information for this font.
//============================================================================

HRESULT __stdcall GenerateFontMetrics()
{
	HRESULT hr;

	Glyph *pGlyph;
    unsigned cbGlyphSize;

	// Figure out the descent, this is defined to be
	// the amount that a lower case 'g' hangs below the baseline.
	// It can be computed by getting the height of the 'g' and
	// subtracting the y component of its bearing.
	//		
	hr = XFONT_GetCharacterData(L'g', &pGlyph, &cbGlyphSize);

	if (FAILED(hr))
	{
		return hr;
	}

	s_pFont->uDescent = pGlyph->uBitmapHeight - pGlyph->iBearingY;

	return NOERROR;
}

//****************************************************************************
// Cache implementation.
//****************************************************************************


//
// The design for the cache errors on the side of simplicitly.  If anybody
// decides to actually use the TrueType package then they will probably
// want to put in something a bit more efficient.
//

//============================================================================
// Initialize the cache.
//============================================================================

HRESULT __stdcall InitializeCache
(
    unsigned cbCacheSize
)
{
    if (cbCacheSize)
    {
        BYTE *pbCache = (BYTE *)malloc(cbCacheSize);

        if (!pbCache)
        {
            return E_OUTOFMEMORY;
        }

	    // Save the cache info.
		s_pFont->pbCache = pbCache;
        s_pFont->pbCacheNext = pbCache;
		s_pFont->pbCacheEnd = pbCache + cbCacheSize;

	    // Initialize the list.
	    s_pFont->CacheHead.pNextEntry = &s_pFont->CacheHead;
	    s_pFont->CacheHead.pPrevEntry = &s_pFont->CacheHead;
    }

    return NOERROR;
}

//============================================================================
// Examines the cache to see if we already have the information for a
// character.
//============================================================================

static BOOL CheckInCache
(
	WCHAR wch, 
	Glyph **ppGlyph,
    unsigned *pcbGlyphSize
)
{
	// This is pretty lame.  We probably have to use a slightly more efficient
	// lookup algorithm.
	//
	CacheEntry *pEntry = s_pFont->CacheHead.pNextEntry;

	while (pEntry->wLength)
	{
		if (pEntry->wch == wch)
		{
			*ppGlyph = (Glyph *)(pEntry + 1);
            *pcbGlyphSize = pEntry->wLength - sizeof(CacheEntry);

			// Move this entry to the front of the list.
			if (s_pFont->CacheHead.pNextEntry != pEntry)
			{
				// Unlink.
				pEntry->pPrevEntry->pNextEntry = pEntry->pNextEntry;
				pEntry->pNextEntry->pPrevEntry = pEntry->pPrevEntry;

				// Relink.
				pEntry->pNextEntry = s_pFont->CacheHead.pNextEntry;
				pEntry->pNextEntry->pPrevEntry = pEntry;
				pEntry->pPrevEntry = &s_pFont->CacheHead;
				s_pFont->CacheHead.pNextEntry = pEntry;
			}

			return TRUE;
		}

		pEntry = pEntry->pNextEntry;
	}

	return FALSE;
}

//============================================================================
// Alloc cache information for a character.
//============================================================================

HRESULT AddToCache
(
	WCHAR wch,
	unsigned cbGlyph,
	Glyph **ppGlyph
)
{
	long cbSizeNeeded = (sizeof(CacheEntry) + sizeof(Glyph) + cbGlyph + 3) & ~3;  // 4 byte aligned
	CacheEntry *pEntry;

	// If the cache isn't big enough, just alloc a block of memory
	// and use that.
	//
	if (cbSizeNeeded > s_pFont->pbCacheEnd - s_pFont->pbCache)
	{
		// We need 1 extra byte for the packer.
		s_pFont->pvOneGlyph = malloc(cbSizeNeeded + 1);

		if (!s_pFont->pvOneGlyph)
		{
			return E_OUTOFMEMORY;
		}

		*ppGlyph = (Glyph *)s_pFont->pvOneGlyph;

		return NOERROR;
	}

	if (cbSizeNeeded > s_pFont->pbCacheEnd - s_pFont->pbCacheNext)
	{
		FreeCacheSpace(cbSizeNeeded);
	}

	// Get the memory.
	pEntry = (CacheEntry *)s_pFont->pbCacheNext;

	// Don't forget about it.
	s_pFont->pbCacheNext += cbSizeNeeded;

	// Link it at the head of the list.
	pEntry->pNextEntry = s_pFont->CacheHead.pNextEntry;
	pEntry->pNextEntry->pPrevEntry = pEntry;
	pEntry->pPrevEntry = &s_pFont->CacheHead;
	s_pFont->CacheHead.pNextEntry = pEntry;

	// Save the cache info.
	pEntry->wch = wch;
	pEntry->wLength = (WORD)cbSizeNeeded;

	// Return it.
	*ppGlyph = (Glyph *)(pEntry + 1);

	return NOERROR;
}

//============================================================================
// Reset the cache back to nothin'.
//============================================================================

void ClearCache()
{
    if (s_pFont)
    {
	    s_pFont->pbCacheNext = s_pFont->pbCache;

	    s_pFont->CacheHead.pNextEntry = &s_pFont->CacheHead;
	    s_pFont->CacheHead.pPrevEntry = &s_pFont->CacheHead;
    }
}

//============================================================================
// Free up enough room in the cache to hold a new entry.
//============================================================================

static void FreeCacheSpace
(
	unsigned cbSizeNeeded
)
{
    unsigned cbSizeFree;
    CacheEntry *pEntry;
    unsigned cbFreed;

    BYTE *pbNext;
	BYTE *pbEnd;

	// Free either the requested size or 1/4 of the cache, whichever is bigger.
	// This won't work well for very small caches with large bitmaps...
	// we'll spend an aweful lot of time freeing.
	//
	cbSizeFree = max(cbSizeNeeded, (unsigned)(s_pFont->pbCacheEnd - s_pFont->pbCache) / 4);

	// We can walk the entries in two ways:
	//
	//  - Through the circularly linked list in either direction.  This list
	//    has the most recently accessed glyph at the front.
	//    
	//  - Through the memory in the order the entries were allocated
	//    via the wLength field.
	//
	// This routine first walks the linked list backwards and "nulls" enough
	// nodes to fit our requirement.  It then walks through memory
	// compressing out those null nodes.  This ruins the whole sense of
	// order in the list...we'll regen that soon enough.
	//
	pEntry = s_pFont->CacheHead.pPrevEntry;
	cbFreed = 0;

	// Free the spaces.
	while (pEntry->wLength && cbFreed < cbSizeFree)
	{
		// Remember the size.
		cbFreed += pEntry->wLength;

		// Mark it as dead by wonking the next field.
		pEntry->pNextEntry = NULL;

		pEntry = pEntry->pPrevEntry;
	}

	// Reset the list.
	s_pFont->CacheHead.pNextEntry = &s_pFont->CacheHead;
	s_pFont->CacheHead.pPrevEntry = &s_pFont->CacheHead;

	// Walk through the memory through the allocations, remove the dead
	// entries and relink the other ones back onto the list.
	//
	pbNext = s_pFont->pbCache;
	pbEnd = s_pFont->pbCacheNext;

	pEntry = (CacheEntry *)pbNext;

	while ((BYTE *)pEntry < pbEnd)
	{
		// Valid entry?
		if (pEntry->pNextEntry)
		{
			CacheEntry *pNewEntry;

			// Move it.
			if ((BYTE *)pEntry != pbNext)
			{
				memmove(pbNext, pEntry, pEntry->wLength);

				pNewEntry = (CacheEntry *)pbNext;
			}
			else
			{
				pNewEntry = pEntry;
			}

			// Relink it onto the end of the list.
			pNewEntry->pPrevEntry = s_pFont->CacheHead.pPrevEntry;
			pNewEntry->pPrevEntry->pNextEntry = pNewEntry;
			pNewEntry->pNextEntry = &s_pFont->CacheHead;
		    s_pFont->CacheHead.pPrevEntry = pNewEntry;

			// The fields in 'pEntry' may have been wonked by the 
			// above move, increment based off the valid field.
			//
			pEntry = (CacheEntry *)((BYTE *)pEntry + pNewEntry->wLength);
			pbNext += pNewEntry->wLength;
		}
		else
		{
			pEntry = (CacheEntry *)((BYTE *)pEntry + pEntry->wLength);
		}
	}

	s_pFont->pbCacheNext = pbNext;
}
