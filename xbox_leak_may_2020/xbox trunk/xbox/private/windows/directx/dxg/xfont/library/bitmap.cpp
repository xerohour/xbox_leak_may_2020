//****************************************************************************
//
// XBox bitmap font library
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

//****************************************************************************
// Definitions.
//****************************************************************************

// Holds the font information.
struct BP_Font : public Font
{

    // If the font file is smaller than the cache size, then we'll just
    // load it into this memory location and look it up directly.
    //
    void *pMemory;
    FontHeader *pHeader;
    DWORD *rgoGlyphs;
    SegmentRun *pSegmentRunTable;
    SegmentDescriptor *pSegmentTable;
    
    // If the file won't fit into the cache or we've been asked to use
    // minimal memory, then we'll manually munge through the file
    // for every character.
    //
    HANDLE hFile;

    // The location of the tables inside of the file.
    DWORD oGlyphOffsets;
    DWORD oSegmentRunTable;
    DWORD oSegmentTable;

};

// Random forwards.
static void __stdcall BP_UnloadFont(struct BP_Font *);
static HRESULT __stdcall BP_ResetTransform(struct BP_Font *);
static HRESULT __fastcall BP_GetCharacterDataFromMemory(struct BP_Font *, WCHAR, struct _Glyph **, unsigned *);
static HRESULT __fastcall BP_GetCharacterDataFromFile(struct BP_Font *, WCHAR, struct _Glyph **, unsigned *);

//****************************************************************************
// APIs.
//****************************************************************************

//============================================================================
// Open a TrueType font from a block of memory.
//============================================================================

HRESULT __stdcall BP_OpenBitmapFontFromMemory
(
    CONST void *pFontData, 
    unsigned uFontDataSize, 
    struct _Font **ppFont
)
{
    HRESULT hr;
    BP_Font *pFont;
    FontHeader *pHeader;

    HANDLE hFile = INVALID_HANDLE_VALUE;

    // Allocate the memory for the font structure and zero it.
    pFont = (BP_Font *)malloc(sizeof(BP_Font));

    if (!pFont)
    {
        return E_OUTOFMEMORY;
    }

    ZeroMemory(pFont, sizeof(BP_Font));

    pFont->hFile = INVALID_HANDLE_VALUE;

    // Get the header.
    pHeader = (FontHeader *)pFontData;

    // Do a sanity check.
    if (pHeader->wSignature != (WORD)'XFNT')
    {
        hr = E_FAIL;
        goto Error;
    }

    if (pHeader->wVersion != FILE_VERSION)
    {
        hr = E_FAIL;
        goto Error;
    }

    // Set up the tables.
    pFont->rgoGlyphs = (DWORD *)(pHeader + 1);
    pFont->pSegmentRunTable = (SegmentRun *)(pFont->rgoGlyphs + pHeader->cGlyphs);
    pFont->pSegmentTable = (SegmentDescriptor *)(pFont->pSegmentRunTable + pHeader->cSegmentRunTable);

    // Save font info.
    pFont->uCellHeight = pHeader->uCellHeight;
    pFont->uDescent = pHeader->uDescent;
    pFont->uAntialiasLevel = pHeader->uAntialiasLevel;
    pFont->uRLEWidth = pHeader->uRLEWidth;
    pFont->uMaxBitmapHeight = pHeader->uMaxBitmapHeight;
    pFont->uMaxBitmapWidth = pHeader->uMaxBitmapWidth;
    pFont->uReferenceCount = 1;

    // So we'll get the data the right way.
    pFont->pfnGetCharacterData = (CB_GetCharacterData)BP_GetCharacterDataFromMemory;

    // Set up the callbacks.
    pFont->pfnUnloadFont = (CB_UnloadFont)BP_UnloadFont;
    pFont->pfnResetTransform = (CB_ResetTransform)BP_ResetTransform;

    pFont->pHeader = pHeader;

    // Return it.
    *ppFont = (Font *)pFont;

    return NOERROR;

Error:
    BP_UnloadFont(pFont);

    return hr;
}

//============================================================================
// Open a TrueType font.
//============================================================================

HRESULT __stdcall BP_OpenBitmapFont
(
    LPCWSTR wszFileName, 
    unsigned uCacheSize,
    Font **ppFont,
    BOOL *pfUseCache
)
{
    HRESULT hr;
    BP_Font *pFont = NULL;

    HANDLE hFile = INVALID_HANDLE_VALUE;

    char szFile[_MAX_PATH];
    WideCharToMultiByte(CP_ACP, 0, wszFileName, -1, szFile, _MAX_PATH, NULL, NULL);

    // Open the font file.
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

    // Get the file's size.
    BY_HANDLE_FILE_INFORMATION FileInformation;

    if (!GetFileInformationByHandle(hFile, &FileInformation))
    {
        hr = HRESULT_FROM_WIN32(GetLastError());
        goto Error;
    }

    assert(FileInformation.nFileSizeHigh == 0);

    // If the cache will hold the entire file, load it in.
    if (FileInformation.nFileSizeLow <= uCacheSize)
    {
        void *pMemory;

        // Allocate the memory to hold the font file.
        pMemory = malloc(FileInformation.nFileSizeLow);

        if (!pMemory)
        {
            hr = E_OUTOFMEMORY;
            goto Error;
        }

        // Load the font.
        DWORD dwRead;

        if (!ReadFile(hFile, pMemory, FileInformation.nFileSizeLow, &dwRead, NULL))
        {
            hr = HRESULT_FROM_WIN32(GetLastError());
            goto Error;
        }

        hr = BP_OpenBitmapFontFromMemory((CONST void *)pMemory, FileInformation.nFileSizeLow, (Font **)&pFont);

        if (FAILED(hr))
        {
            free(pMemory);
            goto Error;
        }

        pFont->pMemory = pMemory;

        // We don't want to use the real cache.
        *pfUseCache = FALSE;

        CloseHandle(hFile);
    }

    // Otherwise load the header and save the info we need from it.
    else
    {
        FontHeader Header;

        // Allocate the memory for the font structure and zero it.
        pFont = (BP_Font *)malloc(sizeof(BP_Font));

        if (!pFont)
        {
            goto Error;
        }

        ZeroMemory(pFont, sizeof(BP_Font));

        pFont->hFile = INVALID_HANDLE_VALUE;

        // Load the font's header.
        DWORD dwRead;

        if (!ReadFile(hFile, &Header, sizeof(FontHeader), &dwRead, NULL))
        {
            hr = HRESULT_FROM_WIN32(GetLastError());
            goto Error;
        }

        // Do some sanity checks.
        if (Header.wSignature != (WORD)'XFNT')
        {
            hr = E_FAIL;
            goto Error;
        }

        if (Header.wVersion != FILE_VERSION)
        {
            hr = E_FAIL;
            goto Error;
        }

        // Remember where everything is in the file.
        pFont->oGlyphOffsets = sizeof(FontHeader);
        pFont->oSegmentRunTable = pFont->oGlyphOffsets + sizeof(DWORD) * Header.cGlyphs;
        pFont->oSegmentTable = pFont->oSegmentRunTable + sizeof(SegmentRun) * Header.cSegmentRunTable;

        // Save font info.
        pFont->uCellHeight = Header.uCellHeight;
        pFont->uDescent = Header.uDescent;
        pFont->uAntialiasLevel = Header.uAntialiasLevel;
        pFont->uRLEWidth = Header.uRLEWidth;
        pFont->uMaxBitmapHeight = Header.uMaxBitmapHeight;
        pFont->uMaxBitmapWidth = Header.uMaxBitmapWidth;
        pFont->uReferenceCount = 1;

        // So we'll get the data the right way.
        pFont->pfnGetCharacterData = (CB_GetCharacterData)BP_GetCharacterDataFromFile;

        // Save the file handle and tell the xfont package to use its cache.
        pFont->hFile = hFile;
        *pfUseCache = TRUE;

        // Set up the callbacks.
        pFont->pfnUnloadFont = (CB_UnloadFont)BP_UnloadFont;
        pFont->pfnResetTransform = (CB_ResetTransform)BP_ResetTransform;
    }

    // Return it.
    *ppFont = (Font *)pFont;

    return NOERROR;

Error:
    if (pFont)
    {
        BP_UnloadFont(pFont);
    }

    if (hFile != INVALID_HANDLE_VALUE)
    {
        CloseHandle(hFile);
    }

    return hr;
}

//****************************************************************************
// Callbacks to store in the Font structure.
//****************************************************************************

//============================================================================
// Free all memory associated with the font.
//============================================================================

static void __stdcall BP_UnloadFont
(
    struct BP_Font *pFont
)
{
    if (pFont->hFile != INVALID_HANDLE_VALUE)
    {
        CloseHandle(pFont->hFile);
    }

    free(pFont->pMemory);
    free(pFont);
}

//============================================================================
// Reset the size, alpha, etc of the font.
//============================================================================

static HRESULT __stdcall BP_ResetTransform
(
    struct BP_Font *pFont
)
{
    // Can't reset a bitmap font.
    return E_FAIL;
}

//============================================================================
// Get the data and bitmap information for one character.
//============================================================================

static HRESULT __fastcall BP_GetCharacterDataFromMemory
(
    BP_Font *pFont, 
    WCHAR wch, 
    struct _Glyph **ppGlyph,
    unsigned *pcbGlyphSize
)
{
    HRESULT hr;

    SegmentDescriptor *pSegment;
    unsigned iGlyphData;
    unsigned uMask;

    // Break the character up.
    unsigned uSegment = CHAR_SEGMENT(wch);
    unsigned uOffset = CHAR_OFFSET(wch);

    // Find its segment in the segment run table.
    SegmentRun *pRun = pFont->pSegmentRunTable;
    SegmentRun *pRunMax = pRun + pFont->pHeader->cSegmentRunTable;

    for (;;)
    {
        if (pRun == pRunMax)
        {
            goto NotFound;
        }

        if (uSegment >= pRun->wFirstSegment && uSegment < (unsigned)pRun->wFirstSegment + pRun->cSegments)
        {
            break;
        }

        pRun++;
    }

    // Get the segment.
    pSegment = pFont->pSegmentTable + pRun->iSegmentTable + uSegment - pRun->wFirstSegment;

    // Calculate the glyph data index.
    iGlyphData = pSegment->iGlyph;
    uMask = pSegment->wCharMask;

    if (!(uMask & (1 << uOffset)))
    {
        goto NotFound;
    }

    // Mask all the bits under this one.
    uMask &= (1 << uOffset) - 1;

    // Count them.
    while (uMask)
    {
        iGlyphData++;

        uMask &= uMask - 1;
    }

    // We have a winner.
    *ppGlyph = (Glyph *)((BYTE *)pFont->pHeader + pFont->rgoGlyphs[iGlyphData]);
    *pcbGlyphSize = pFont->rgoGlyphs[iGlyphData + 1] - pFont->rgoGlyphs[iGlyphData];

    return NOERROR;

NotFound:
    // Recurse with the default character.
    if (wch != pFont->pHeader->wDefaultChar)
    {
        hr = BP_GetCharacterDataFromMemory(pFont, pFont->pHeader->wDefaultChar, ppGlyph, pcbGlyphSize);

        if (FAILED(hr))
        {
            return hr;
        }
        else
        {
            return S_FALSE;
        }
    }

    return E_FAIL;
}

//============================================================================
// Get the data and bitmap information for one character by munging through
// the file.
//
// This may look slow, but it seems to work just fine on Win2k.  It will
// be interesting to see how well it works on the Xbox, though..
//============================================================================

static HRESULT __fastcall BP_GetCharacterDataFromFile
(
    BP_Font *pFont, 
    WCHAR wch, 
    struct _Glyph **ppGlyph,
    unsigned *pcbGlyphSize
)
{
    HRESULT hr;

    Glyph *pGlyph;

    unsigned iGlyphData;
    unsigned uMask;

    // Break the character up.
    unsigned uSegment = CHAR_SEGMENT(wch);
    unsigned uOffset = CHAR_OFFSET(wch);

    DWORD dwRead, o;
    
    unsigned i, c = (pFont->oSegmentTable - pFont->oSegmentRunTable) / sizeof(SegmentRun);

    // Find its segment in the segment run table.
    SegmentRun Run;
    SegmentDescriptor Segment;
    DWORD rgGlyphOffsets[2];

    // Seek to the right offset.
    if (SetFilePointer(pFont->hFile, pFont->oSegmentRunTable, NULL, FILE_BEGIN) == INVALID_SET_FILE_POINTER)
    {
        hr = HRESULT_FROM_WIN32(GetLastError());
        goto Error;
    }

    // Read segment run table.
    for (i = 0;; i++)
    {
        if (i == c)
        {
            goto NotFound;
        }

        if (!ReadFile(pFont->hFile, &Run, sizeof(Run), &dwRead, NULL))
        {
            hr = HRESULT_FROM_WIN32(GetLastError());
            goto Error;
        }

        if (uSegment >= Run.wFirstSegment && uSegment < (unsigned)Run.wFirstSegment + Run.cSegments)
        {
            break;
        }
    }

    // Get the segment.
    o = pFont->oSegmentTable + (Run.iSegmentTable + uSegment - Run.wFirstSegment) * sizeof(SegmentDescriptor);

    if (SetFilePointer(pFont->hFile, o, NULL, FILE_BEGIN) == INVALID_SET_FILE_POINTER)
    {
        hr = HRESULT_FROM_WIN32(GetLastError());
        goto Error;
    }

    if (!ReadFile(pFont->hFile, &Segment, sizeof(Segment), &dwRead, NULL))
    {
        hr = HRESULT_FROM_WIN32(GetLastError());
        goto Error;
    }

    // Calculate the glyph data index.
    iGlyphData = Segment.iGlyph;
    uMask = Segment.wCharMask;

    if (!(uMask & (1 << uOffset)))
    {
        goto NotFound;
    }

    // Mask all the bits under this one.
    uMask &= (1 << uOffset) - 1;

    // Count them.
    while (uMask)
    {
        iGlyphData++;

        uMask &= uMask - 1;
    }

    // Get the offset of the data.
    o = pFont->oGlyphOffsets + sizeof(DWORD) * iGlyphData;

    if (SetFilePointer(pFont->hFile, o, NULL, FILE_BEGIN) == INVALID_SET_FILE_POINTER)
    {
        hr = HRESULT_FROM_WIN32(GetLastError());
        goto Error;
    }

    if (!ReadFile(pFont->hFile, &rgGlyphOffsets, sizeof(DWORD) * 2, &dwRead, NULL))
    {
        hr = HRESULT_FROM_WIN32(GetLastError());
        goto Error;
    }

    // Allocate memory for the glyph.
    hr = AddToCache(pFont, wch, rgGlyphOffsets[1] - rgGlyphOffsets[0] - sizeof(Glyph), &pGlyph);

    if (FAILED(hr))
    {
        goto Error;
    }

    // We're now ready to read in the glyph.
    if (SetFilePointer(pFont->hFile, rgGlyphOffsets[0], NULL, FILE_BEGIN) == INVALID_SET_FILE_POINTER)
    {
        hr = HRESULT_FROM_WIN32(GetLastError());
        goto Error;
    }

    if (!ReadFile(pFont->hFile, pGlyph, rgGlyphOffsets[1] - rgGlyphOffsets[0], &dwRead, NULL))
    {
        hr = HRESULT_FROM_WIN32(GetLastError());
        goto Error;
    }

    *ppGlyph = pGlyph;
    *pcbGlyphSize = rgGlyphOffsets[1] - rgGlyphOffsets[0];

    return NOERROR;

NotFound:
    // Recurse with the default character.
    if (wch != pFont->pHeader->wDefaultChar)
    {
        hr = BP_GetCharacterDataFromFile(pFont, pFont->pHeader->wDefaultChar, ppGlyph, pcbGlyphSize);

        if (FAILED(hr))
        {
            return hr;
        }
        else
        {
            return S_FALSE;
        }
    }

    hr = E_FAIL;

Error:
    return hr;
}

