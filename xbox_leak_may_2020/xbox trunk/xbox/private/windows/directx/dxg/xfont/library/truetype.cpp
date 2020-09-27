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

// This is a C++ file so we can take advantage of inheritance to extend
// the font data structure.

#ifdef _XBOX

#include <xtl.h>
#include "xfont.h"
#include "xfontformat.h"

#else 

#include <windows.h>
#include "xfont-pc.h"
#include "xfontformat-pc.h"

#endif

#include <assert.h>
#include "font.h"

// Scaler imports
#include "fscdefs.h"
#include "sfnt.h"
#include "sfntaccs.h"
#include "fnt.h"
#include "fsglue.h"
#include "scentry.h"
#include "sbit.h"
#include "fscaler.h"

//****************************************************************************
// Definitions.
//****************************************************************************

// 
// Structures
//

// Holds the font information.
struct TT_Font : public Font
{
    // Handle to our file.
    HANDLE hFile;

	// TrueType specific datastructures.
	fs_GlyphInputType in;
	fs_GlyphInfoType out;

	// The current sizes of each of the alloced chunks of memory.
	unsigned rgcbSizeMemory[MEMORYFRAGMENTS];
};

//
// Global variables.
//

// The identity matrix.
static transMatrix s_IdentityMatrix = 
{
	{
		{ ONEFIX, 0, 0 },
		{ 0, ONEFIX, 0 },
		{ 0, 0, ONEFIX }
	}
};

// The italics matrix.
static transMatrix s_ItalicsMatrix = 
{
	{
		{ ONEFIX, 0,      0      },
		{ 0x5700, ONEFIX, 0      },
		{ 0,      0,      ONEFIX }
	}
};

//
// Forwards.
//

// Error mapping.
#define ErrRet(x) { int __i = (x); if (__i) return MapError(__i); }
#define ErrGo(x) { int __i = (x); if (__i) { hr = MapError(__i); goto Error; } }

static HRESULT MapError(int i);

// Helper to allocate memory in the scaler datastructures.
static HRESULT AllocateScalerMemory(TT_Font *, unsigned, unsigned);

// Random forwards.
static void __stdcall TT_UnloadFont(struct TT_Font *);
static HRESULT __stdcall TT_ResetTransform(struct TT_Font *);
static HRESULT __fastcall TT_GetCharacterData(struct TT_Font *, WCHAR, struct _Glyph **, unsigned *);

static unsigned PackBitmap(unsigned, unsigned, unsigned, unsigned, unsigned, BYTE *, BYTE *);

static void * __cdecl TTI_GetFontData(long, long, long);
static void __cdecl TTI_ReleaseFontData(void *);

//****************************************************************************
// APIs.
//****************************************************************************

extern "C" HRESULT __stdcall GenerateFontMetrics(Font *pFont);
extern "C" HRESULT __stdcall InitializeCache(Font *pFont, unsigned cbCacheSize);

//============================================================================
// Load a TrueType font into the scan converter.  
//
// This call will use more memory than just the the cache because the 
// TrueType scan converter needs a bit of workspace to process the font.  If 
// memory is a concern, use a bitmap font instead.
//
// This automatically sets the opened font as the one that is currently
// active.  The attributes of the font are all set to their defaults.
//============================================================================

extern "C"
HRESULT __stdcall XFONT_OpenTrueTypeFont
(
	LPCWSTR wszFileName,	// [in] The file name to the font
	unsigned uCacheSize,	// [in] The size of the font cache, in bytes.
	XFONT **ppFont          // [out] The font identifier
)
{
	HRESULT hr;
	Font *pFont = NULL;
	
	assert(wszFileName);

	// Initialize the true type package.
	hr = TT_OpenTrueTypeFont(wszFileName, &pFont);

	if (FAILED(hr))
	{
		goto Error;
	}

    hr = InitializeCache(pFont, uCacheSize);
    
    if (FAILED(hr))
    {
        goto Error;
    }

	// Reset the font metrics.
	hr = GenerateFontMetrics(pFont);

	if (FAILED(hr))
	{
		goto Error;
	}

	// Set the font style defaults.
	pFont->uAlignmentMode   = XFONT_TOP | XFONT_LEFT;
	pFont->TextColor		  = 0xFFFFFFFF;
	pFont->BackgroundColor  = 0x00000000;
	pFont->fPaintBackground = FALSE;

	// Return it.
	*ppFont = (XFONT *)pFont;

	return NOERROR;

Error:
    if (pFont)
    {
	    XFONT_Release((XFONT *)pFont);
    }

	return hr;
}

//============================================================================
// Open a TrueType font.
//============================================================================

HRESULT __stdcall TT_OpenTrueTypeFont
(
	LPCWSTR wszFileName, 
	Font **ppFont
)
{
	HRESULT hr;
	TT_Font *pFont;
    HANDLE hFile;

	// Allocate the memory for the font structure and zero it.
	pFont = (TT_Font *)malloc(sizeof(TT_Font));

	if (!pFont)
	{
		return E_OUTOFMEMORY;
	}

	ZeroMemory(pFont, sizeof(TT_Font));

    pFont->hFile = INVALID_HANDLE_VALUE;
    pFont->uReferenceCount = 1;

	// Ask the scaler package how much memory it needs.
	ErrGo(fs_OpenFonts(&pFont->in, &pFont->out));

	// Allocate the memory
	hr = AllocateScalerMemory(pFont, 0, 2);

	if (FAILED(hr))
	{
		goto Error;
	}

	// Do the real initialization.
	ErrGo(fs_Initialize(&pFont->in, &pFont->out));

	// Open the font file.
	hFile = INVALID_HANDLE_VALUE;

    char szFile[_MAX_PATH];
    WideCharToMultiByte(CP_ACP, 0, wszFileName, -1, szFile, _MAX_PATH, NULL, NULL);

	hFile = CreateFileA(szFile, 
			           GENERIC_READ, 
					   FILE_SHARE_READ, 
					   NULL, 
					   OPEN_EXISTING, 
					   FILE_ATTRIBUTE_NORMAL, 
					   NULL);

	if (hFile == INVALID_HANDLE_VALUE)
	{
		hr = HRESULT_FROM_WIN32(GetLastError());
		goto Error;
	}

    pFont->hFile = hFile;

	// Register the font.
	pFont->in.clientID = (ULONG_PTR)hFile;
	pFont->in.param.newsfnt.platformID = 3;		// MS recommended value
	pFont->in.param.newsfnt.specificID = 1;		// UNICODE
	pFont->in.GetSfntFragmentPtr = (GetSFNTFunc)TTI_GetFontData;
	pFont->in.ReleaseSfntFrag = TTI_ReleaseFontData;
	pFont->in.sfntDirectory = NULL;

	ErrGo(fs_NewSfnt(&pFont->in, &pFont->out));

	// Set up its transforms.
	hr = AllocateScalerMemory(pFont, 3, 4);

	if (FAILED(hr))
	{
		goto Error;
	}

	// Default to 16 pixels high.
	pFont->uCellHeight = 16;
	pFont->uAntialiasLevel = 0;
    pFont->uRLEWidth = 2;
    pFont->uMaxBitmapHeight = 255;
    pFont->uMaxBitmapWidth = 255;

	hr = TT_ResetTransform(pFont);

	if (FAILED(hr))
	{
		goto Error;
	}

	// Set up the callbacks.
	pFont->pfnUnloadFont = (CB_UnloadFont)TT_UnloadFont;
	pFont->pfnResetTransform = (CB_ResetTransform)TT_ResetTransform;
	pFont->pfnGetCharacterData = (CB_GetCharacterData)TT_GetCharacterData;

	// Return it.
	*ppFont = (Font *)pFont;

	return NOERROR;

Error:
	TT_UnloadFont(pFont);

	return hr;
}

//****************************************************************************
// Callbacks to store in the Font structure.
//****************************************************************************

//============================================================================
// Free all memory associated with the font.
//============================================================================

static void __stdcall TT_UnloadFont
(
	struct TT_Font *pFont
)
{
	unsigned i;

	for (i = 0; i < MEMORYFRAGMENTS; i++)
	{
		if (pFont->in.memoryBases[i])
		{
			free(pFont->in.memoryBases[i]);
		}
	}

	free(pFont->pvOneGlyph);

    if (pFont->hFile != INVALID_HANDLE_VALUE)
    {
        CloseHandle(pFont->hFile);
    }

	free(pFont);
}

//============================================================================
// Reset the size, alpha, etc of the font.
//============================================================================

static HRESULT __stdcall TT_ResetTransform
(
	struct TT_Font *pFont
)
{
	ClearCache(pFont);  		// The cache is hash.

	ZeroMemory(&pFont->in.param.newtrans, sizeof(pFont->in.param.newtrans));

    unsigned uOverScale = pFont->uAntialiasLevel;

    // We need to set the overscale to 1 when we don't want any antialasing
    // information to avoid creating a true bitmap.  The byte-array format
    // is much more convenient for creating the packed bitmap.  You may want
    // to change this if you're using the truetype package in memory-
    // sensative code.
    //
    if (uOverScale == 0)
    {
        uOverScale = 1;
    }

	// pixels while the other is in 2^n
	pFont->in.param.newtrans.pointSize = pFont->uCellHeight << 16;
	pFont->in.param.newtrans.usOverScale = (USHORT)uOverScale;
	pFont->in.param.newtrans.xResolution = 72;      // 1 pixel = 1 pt
	pFont->in.param.newtrans.yResolution = 72;   
	pFont->in.param.newtrans.pixelDiameter = FIXEDSQRT2;
	pFont->in.param.newtrans.transformMatrix = &s_IdentityMatrix;

    // set up the style.
    if (pFont->uStyle == XFONT_BOLD || pFont->uStyle == XFONT_BOLDITALICS)
    {
		pFont->in.param.newtrans.usEmboldWeightx = 20;
		pFont->in.param.newtrans.usEmboldWeighty = 20;
		pFont->in.param.newtrans.lDescDev = pFont->uDescent;
		pFont->in.param.newtrans.bBitmapEmboldening = TRUE;
    }

    if (pFont->uStyle == XFONT_ITALICS || pFont->uStyle == XFONT_BOLDITALICS)
    {
	    pFont->in.param.newtrans.transformMatrix = &s_ItalicsMatrix;
    }

	return fs_NewTransformation(&pFont->in, &pFont->out);
}

//============================================================================
// Get the data and bitmap information for one character.
//============================================================================

static HRESULT __fastcall TT_GetCharacterData
(
	TT_Font *pFont, 
	WCHAR wch, 
	struct _Glyph **ppGlyph,
    unsigned *pcbGlyphSize
)
{
	HRESULT hr;

	Glyph *pGlyph;

	//
	// Generate the glpyh information.
	//

	// Get the outline for the character.
	ZeroMemory(&pFont->in.param.newglyph, sizeof(pFont->in.param.newglyph));

	pFont->in.param.newglyph.characterCode = wch;

	ErrRet(fs_NewGlyph(&pFont->in, &pFont->out));

	ZeroMemory(&pFont->in.param.gridfit, sizeof(pFont->in.param.gridfit));

	ErrRet(fs_ContourGridFit(&pFont->in, &pFont->out));
	ErrRet(fs_FindBitMapSize(&pFont->in, &pFont->out));

	//
	// Have the rasterizer build the bitmap.
	//

	hr = AllocateScalerMemory(pFont, 5,8);

	if (FAILED(hr))
	{
		return hr;
	}

	// Get the bitmap.
	ZeroMemory(&pFont->in.param.scan, sizeof(pFont->in.param.scan));

	pFont->in.param.scan.bottomClip = pFont->out.bitMapInfo.bounds.top;
	pFont->in.param.scan.topClip = pFont->out.bitMapInfo.bounds.bottom;

	ErrRet(fs_ContourScan(&pFont->in, &pFont->out));

	// 
	// Allocate the memory to store the glyph information and the bitmap, then
	// fill out the glyph data structure.
	//

	const unsigned uBitmapHeight = pFont->out.bitMapInfo.bounds.bottom - pFont->out.bitMapInfo.bounds.top;
	const unsigned uBitmapWidth = pFont->out.bitMapInfo.bounds.right - pFont->out.bitMapInfo.bounds.left;

    // We can't handle very large bitmaps.
    if (uBitmapHeight > 255 || uBitmapWidth > 255)
    {
        return E_FAIL;
    }

	unsigned cbBitmap;
    
    cbBitmap = PackBitmap(uBitmapHeight,
                          uBitmapWidth,
                          pFont->uRLEWidth,
                          pFont->uAntialiasLevel,
                          pFont->out.bitMapInfo.rowBytes,
                          (BYTE *)pFont->out.bitMapInfo.baseAddr,
                          NULL);

	// Allocate the memory
	hr = AddToCache(pFont,
                    wch,
					cbBitmap,
					&pGlyph);

	if (FAILED(hr))
	{
		return hr;
	}

	// Fill in the Glyph structure.
	pGlyph->uBitmapHeight = (BYTE)uBitmapHeight;
	pGlyph->uBitmapWidth = (BYTE)uBitmapWidth;
	pGlyph->uAdvance = (unsigned char)(pFont->out.metricInfo.devAdvanceWidth.x >> 16);
	pGlyph->iBearingX = (char)(pFont->out.metricInfo.devLeftSideBearing.x >> 16);
	pGlyph->iBearingY = (char)(pFont->out.metricInfo.devLeftSideBearing.y >> 16);

	// 
	// 'Pack' the bitmap into its smallest form.
	//
    
    memset((BYTE *)(pGlyph + 1), 0, cbBitmap);

    PackBitmap(uBitmapHeight,
               uBitmapWidth,
               pFont->uRLEWidth,
               pFont->uAntialiasLevel,
               pFont->out.bitMapInfo.rowBytes,
               (BYTE *)pFont->out.bitMapInfo.baseAddr,
               (BYTE *)(pGlyph + 1));

	//
	// Return the results.
	//

	*ppGlyph = pGlyph;
    *pcbGlyphSize = cbBitmap + sizeof(Glyph);
    
    // Return S_FALSE to indicate that we're using the default glyph.
    return pFont->out.glyphIndex ? NOERROR : S_FALSE;
}


//****************************************************************************
// Implementation.
//****************************************************************************

//============================================================================
// Maps an error from the scaler package to an hresult.
//============================================================================

static HRESULT MapError
(	
	int i
)
{
	assert(false);

	return E_FAIL;
}

//============================================================================
// Helper to allocate memory insize of the scaler datastructures.
//============================================================================

static HRESULT AllocateScalerMemory
(
	TT_Font *pFont,         // [in] Font to allocate in.
	unsigned iStart,		// [in] The first fragment to allocate
	unsigned iEnd			// [in] The last fragement to allocate, inclusive
)
{
	unsigned i;

	for (i = iStart; i <= iEnd; i++)
	{
		if (pFont->out.memorySizes[i])
		{
			// Check to see if this slot already has memory that's big enough.
			if (pFont->rgcbSizeMemory[i] >= (unsigned)pFont->out.memorySizes[i])
			{
				continue;
			}

			if (pFont->in.memoryBases[i])
			{
				free(pFont->in.memoryBases[i]);
				pFont->rgcbSizeMemory[i] = 0;
			}

			pFont->in.memoryBases[i] = (char *)malloc(pFont->out.memorySizes[i]);

			if (!pFont->in.memoryBases[i])
			{
				goto Failed;
			}

			pFont->rgcbSizeMemory[i] = pFont->out.memorySizes[i];
		}
	}

	return NOERROR;

Failed:
	return E_OUTOFMEMORY;
}

//============================================================================
// The scaler needs this for some reason.
//============================================================================

extern "C"
void __cdecl TtfdDbgPrint(char *szMessage)
{
    assert(false);
}

//============================================================================
// Helper for pack bitmap
//============================================================================

static unsigned EmitRLEPacket
(
    long cCount,
    unsigned dwValue,
    long lRLEMax,
    unsigned uRLEWidth,
    long lValueMax,
    unsigned uValueWidth,
    BYTE **ppbCurrentPacked,
    unsigned *pcShift
)
{
    BYTE *pbCurrentPacked = *ppbCurrentPacked;
    unsigned cShift = *pcShift;
    unsigned cBits = 0;
    long cValueCount = cCount;
    bool fPartialValue = dwValue && dwValue < (DWORD)lValueMax;

    // We use a slightly different encoding if we're working with aliasing
    // information.  We do 0+<value> to indicate that we're
    // dealing with either a zero count (0 + 0) or a partially lit
    // pixel (0 + value).
    //
    if (uValueWidth && (fPartialValue || cCount == 0))
    {
        if (cCount == 0)
        {
            cCount = 1;
            dwValue = 0;
        }

        while (cCount--)
        {
            unsigned cWriteValue = dwValue;

            // "Write" the zero which is our escape character.
            if (pbCurrentPacked)
            {
                cShift += uRLEWidth;
                pbCurrentPacked += cShift / 8;
                cShift %= 8;
            }

            cBits += uRLEWidth;

            // Shove out the value.
            if (pbCurrentPacked)
            {
                cWriteValue <<= cShift;

                *pbCurrentPacked |= (BYTE)(cWriteValue & 0xFF);

                if (cWriteValue & 0xFF00)
                {
                    *(pbCurrentPacked + 1) = (BYTE)(cWriteValue >> 8);
                }

                cShift += uValueWidth;
                pbCurrentPacked += cShift / 8;
                cShift %= 8;
            }

            cBits += uValueWidth;
        }
    }
    else
    {
        // Write the actual count (may be zero).
        while (cValueCount >= 0)
        {
            if (pbCurrentPacked)
            {
                const unsigned cWriteValue = min(lRLEMax, cValueCount) << cShift;

                *pbCurrentPacked |= (BYTE)(cWriteValue & 0xFF);

                if (cWriteValue & 0xFF00)
                {
                    *(pbCurrentPacked + 1) = (BYTE)(cWriteValue >> 8);
                }

                cShift += uRLEWidth;
                pbCurrentPacked += cShift / 8;
                cShift %= 8;
            }

            cBits += uRLEWidth;
            cValueCount -= lRLEMax;
        }
    }

    *ppbCurrentPacked = pbCurrentPacked;
    *pcShift = cShift;

    return cBits;
}

//============================================================================
// Back a bitmap from the scaler's byte per pixel to its minimum possible
// size.  Returns that size.
//============================================================================

static unsigned PackBitmap
(	
	unsigned uBitmapHeight,     // [in] Height of the bitmap
    unsigned uBitmapWidth,      // [in] Width of the bitmap
    unsigned uRLEWidth,         // [in] How many bits to use in the RLE
    unsigned uAntialiasLevel,   // [in] How much aliasing information to include?
	unsigned uPitch,			// [in] Pitch of the bitmap
	BYTE *pbUnpacked,			// [in] Unpacked bitmap
    BYTE *pbPacked              // [in] Where to put the packed bitmap.
)
{
	assert(uRLEWidth >= 2 && uRLEWidth <= 8);

	// Walk the bitmap and do our thing.
	BYTE *pbCurrentPacked = pbPacked;
	unsigned iRow, cRows = uBitmapHeight;
	unsigned iColumn, cColumns = uBitmapWidth;
	unsigned cUnpackedIncrement = uPitch - cColumns;
    
    long lRLEMax = (1 << uRLEWidth) - 1;
    long lAntialiasMax = (1 << uAntialiasLevel);

    unsigned cBits = 0;
    unsigned cShift = 0;

    BYTE bPixel;

    BYTE bCurrentValue = 0;
    BYTE bLastValue = (BYTE)lAntialiasMax;

    long cValueCount = 0;

	for (iRow = cRows; iRow; iRow--)
	{
		// Copy each each pixel.
		for (iColumn = cColumns; iColumn; iColumn--)
		{
			// Get the pixel.
			bPixel = *pbUnpacked;

            if (bPixel != bCurrentValue)
            {
                cBits += EmitRLEPacket(cValueCount, 
                                       bCurrentValue, 
                                       lRLEMax, 
                                       uRLEWidth, 
                                       lAntialiasMax,
                                       uAntialiasLevel, 
                                       &pbCurrentPacked, 
                                       &cShift);

                // The compaction algorithm always assumes that full off and
                // full on alternate.  If we get on - partial - on then we need
                // to insert a zero to turn it into on - paritial - off - on.
                //
                if (bCurrentValue != 0 && bCurrentValue != lAntialiasMax && bPixel == bLastValue)
                {
                    cBits += EmitRLEPacket(0, 
                                           0,       // doesn't matter
                                           lRLEMax, 
                                           uRLEWidth, 
                                           lAntialiasMax,
                                           uAntialiasLevel, 
                                           &pbCurrentPacked, 
                                           &cShift);
                }

                if (bCurrentValue == 0 || bCurrentValue == lAntialiasMax)
                {
                    bLastValue = bCurrentValue;
                }

                bCurrentValue = bPixel;

                cValueCount = 0;
            }

            cValueCount++;

			// Move on to the next unpacked byte.
			pbUnpacked++;
		}
		pbUnpacked += cUnpackedIncrement;
	}

    // Pack the last value.
    if (cValueCount > 0)
    {
        cBits += EmitRLEPacket(cValueCount, 
                               bCurrentValue, 
                               lRLEMax, 
                               uRLEWidth, 
                               lAntialiasMax,
                               uAntialiasLevel, 
                               &pbCurrentPacked, 
                               &cShift);
    }

    return (cBits + 7) / 8;
}

//============================================================================
// Default implemenation of the font read callback.  This seeks the font's
// file to the right spot and reads it.
//============================================================================

static void * __cdecl TTI_GetFontData
(
	long lCookie, 
	long lOffset, 
	long lSize
)
{
	HANDLE hFile = (HANDLE)lCookie;
	void *pvMemory;
	DWORD dwRead;


	// Allocate the memory.
	pvMemory = malloc(lSize);
	
	if (!pvMemory)
	{
		return NULL;
	}

	// Seek to the right offset.
	if (SetFilePointer(hFile, lOffset, NULL, FILE_BEGIN) == INVALID_SET_FILE_POINTER)
	{
		goto Error;
	}

	// Read the information.
	if (!ReadFile(hFile, pvMemory, lSize, &dwRead, NULL) || dwRead != (DWORD)lSize)
	{
		goto Error;
	}

	return pvMemory;

Error:
	free(pvMemory);
	return NULL;
}

//============================================================================
// Default implementation of the font data release callback.  This just frees
// the memory we allocated.
//============================================================================

static void __cdecl TTI_ReleaseFontData
(
	void *pv
)
{
	free(pv);
}

