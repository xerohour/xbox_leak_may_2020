//****************************************************************************
//
// XBox font file generator.  
//
// History:
//
//   07/06/00 [andrewso] - Created
//   08/04/00 [andrewso] - Added compressed glyph bitmaps
//
//****************************************************************************

// NOTE: This is a one-shot tool that isn't too good about releasing its
//  : memory when it is done

#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <limits.h>
#include <xboxverp.h>

#define XFONT_TRUETYPE

#include "xfontformat-pc.h"
#include "xfont-pc.h"

//============================================================================
// Secret export from the xfont lib to give us the character data.
//============================================================================

extern "C"
HRESULT __fastcall XFONT_GetCharacterData(XFONT *pXFont, WCHAR wch, Glyph **ppGlyph, unsigned *pcbGlyphSize);

//============================================================================
// The options.
//============================================================================

static WCHAR *s_szInputFileName;
static WCHAR *s_szOutputFileName;
static WCHAR *s_szFontDirectory;
static WCHAR *s_szCodeName;

static WCHAR s_szFontFileName[MAX_PATH];
static unsigned s_uCellHeight;
static unsigned s_uDefaultCharacter;
static unsigned s_uAntialiasLevel;
static bool     s_fAntialiasLevelSet;
static unsigned s_uRLEWidth;

static unsigned s_uStyle;

static WCHAR *s_szFontFileNameCL;
static unsigned s_uCellHeightCL;
static unsigned s_uDefaultCharacterCL;
static unsigned s_uAntialiasLevelCL;
static bool     s_fAntialiasLevelCLSet;
static unsigned s_uRLEWidthCL;
static unsigned s_uStyleCL;

static bool s_fVerbose;

static unsigned s_uDescent;  // calculated

//============================================================================
// Program data.
//============================================================================

// The list of characters is handy to have around.  Keep and array of
// segments and encoded offsets.
//
static unsigned s_cCharacters;
static unsigned s_rgwCharset[CHAR_SEGMENT_MAX];

// The segment run table at its max size.
static unsigned s_cSegmentRuns;
static SegmentRun s_rgSegmentRuns[CHAR_SEGMENT_MAX / 2];

// The segment table at its maximum size.
static unsigned s_cSegments;
static SegmentDescriptor s_rgSegmentDescriptors[CHAR_SEGMENT_MAX];

// The glyph table.
static unsigned s_cGlyphs;
static Glyph **s_rgpGlyphs;
static DWORD  *s_rgdwGlyphOffsets;

static unsigned s_uMaxBitmapHeight;
static unsigned s_uMaxBitmapWidth;

// We may wind up building the glyphs multiple times to try to find the
// best RLE packet size.  If the user doesn't specify a size then we will
// search 2-5 bits.
//
static Glyph *s_rgpGlyphsTry[2][65536];
static DWORD  s_rgdwGlyphOffsetsTry[2][65536];

//============================================================================
// Displays the options for this tool.
//============================================================================

static void Usage()
{
    fprintf(stderr,
            "\nMakeFont version " VER_PRODUCTVERSION_STR "\n\n"
            "Usage: MakeFont [options] <input file> <output file>\n\n"
            "     -d <directory>      directory from which to load the TrueType font\n"
            "     -f <font file>      TrueType font file to load, -d is ignored\n"
            "     -h <height>         height of the font, in pixels\n"
            "     -e <hex number>     character to use as the default character\n"
            "     -a <alpha>          how much antialiasing information to store?  (0, 2, 4)\n"
            "     -r <bits>           how wide to make each RLE packet?  (0, 2-8)\n"
            "     -s <style>          NORMAL, BOLD, ITALICS or BOLDITALICS\n"
            "     -c <name>           Create a C structure called <name> in the output file\n"
            "     -v                  dump font statistics\n\n"
            "     All of the above options except -d and -c have corresponding commands in\n"
            "     the input file.\n");
}

//============================================================================
// Process the command line.  Returns false if it was invalid 
//============================================================================

static bool ParseCommandLine
(
    int argc,
    WCHAR **argv
)
{
    int i;

    // Loop through the options.
    for (i = 1; i < argc; i++)
    {
        if (argv[i][0] != '-' && argv[i][0] != '/')
        {
            if (!s_szInputFileName)
            {
                s_szInputFileName = argv[i];
            }
            else if (!s_szOutputFileName)
            {
                s_szOutputFileName = argv[i];
            }
            else
            {
                return false;
            }

            continue;
        }

        if (argv[i][2] != 0)
        {
            return false;
        }

        if (argv[i][1] == 'v')
        {
            s_fVerbose = true;
            continue;
        }

        if (argc - i == 1 || argv[i + 1][0] == '-' || argv[i + 1][0] == '/')
        {
            return false;
        }

        switch(argv[i][1])
        {
        case 'd':
            if (s_szFontDirectory)
            {
                return false;
            }

            s_szFontDirectory = argv[i + 1];
            break;

        case 'c':
            if (s_szCodeName)
            {
                return false;
            }

            s_szCodeName = argv[i + 1];
            break;

        case 'f':
            if (s_szFontFileNameCL)
            {
                return false;
            }

            s_szFontFileNameCL = argv[i + 1];
            break;

        case 'h':
            if (s_uCellHeightCL)
            {
                return false;
            }

            s_uCellHeightCL = wcstoul(argv[i + 1], NULL, 10);

            if (!s_uCellHeightCL)
            {
                return false;
            }
            break;

        case 'e':
            if (s_uDefaultCharacterCL)
            {
                return false;
            }

            s_uDefaultCharacterCL = wcstoul(argv[i + 1], NULL, 16);

            if (!s_uDefaultCharacterCL)
            {
                return false;
            }
            break;

        case 'a':
            if (s_fAntialiasLevelCLSet)
            {
                return false;
            }

            s_uAntialiasLevelCL = wcstoul(argv[i + 1], NULL, 10);
            s_fAntialiasLevelCLSet = true;

            if (s_uAntialiasLevelCL != 0 && s_uAntialiasLevelCL != 2 && s_uAntialiasLevelCL != 4)
            {
                return false;
            }                
            break;

        case 'r':
            if (s_uRLEWidthCL)
            {
                return false;
            }

            s_uRLEWidthCL = wcstoul(argv[i + 1], NULL, 10);

            if (s_uRLEWidthCL == 1 || s_uRLEWidthCL > 8)
            {
                return false;
            }                
            break;

        case 's':
            if (s_uStyleCL)
            {
                return false;
            }

            if (!_wcsicmp(argv[i + 1], L"NORMAL"))
            {
                s_uStyleCL = XFONT_NORMAL;
            }
            else if (!_wcsicmp(argv[i + 1], L"BOLD"))
            {
                s_uStyleCL = XFONT_BOLD;
            }
            else if (!_wcsicmp(argv[i + 1], L"ITALICS"))
            {
                s_uStyleCL = XFONT_ITALICS ;
            }
            else if (!_wcsicmp(argv[i + 1], L"BOLDITALICS"))
            {
                s_uStyleCL = XFONT_BOLDITALICS;
            }
            else
            {
                return false;
            }

            break;

        default:
            return false;
        }

        i++;
    }

    // Do we have our defaults?
    if (!s_szInputFileName || !s_szOutputFileName)
    {
        return false;
    }

    return true;
}

//============================================================================
// Get the next word from a string.  This destroys the string as it walks.
//============================================================================

static WCHAR *NextWord(/* in/out */WCHAR **pszString)
{
    // Nothing to walk.
    if (!*pszString)
    {
        return NULL;
    }

    // Find the start of the word.
    WCHAR *szStart = *pszString;

    while (*szStart && iswspace(*szStart))
    {
        szStart++;
    }   

    if (!*szStart)
    {
        *pszString = szStart;
        return NULL;
    }

    // Find its end.
    WCHAR *szEnd = szStart;

    if (*szStart == L'"')
    {
        szStart++;
        szEnd++;

        while (*szEnd && *szEnd != L'"')
        {
            szEnd++;
        }      
    }
    else
    {
        while (*szEnd && !iswspace(*szEnd))
        {
            szEnd++;
        }
    }

    if (!*szEnd)
    {
        *pszString = szEnd;
    }
    else
    {
        *pszString = szEnd + 1;
        *szEnd = 0;
    }

    return szStart;
}

//============================================================================
// Parse the input file.  Returns false if it fails.
//============================================================================

static bool LoadInputFile()
{
    FILE *pFile;
    WCHAR sz[1024], *szWalk, *szWord;
    unsigned l;
    unsigned cLines = 0;

    // The include file stack.
    const unsigned cFileStack = 4;
    unsigned iTopOfStack = 0;

    FILE *rgpFiles[cFileStack];
    unsigned rgcLines[cFileStack];
    WCHAR rgszFileNames[cFileStack][_MAX_PATH];

    WCHAR szCurrentFile[_MAX_PATH];

    // Open the file.
    pFile = _wfopen(s_szInputFileName, L"ra");

    wcscpy(szCurrentFile, s_szInputFileName);

    if (!pFile)
    {
        // UNDONE andrewso: better error message.
        printf("\nUnable to open the file character set file '%S'\n", s_szInputFileName);
        return false;
    }

    while (pFile)
    {
        // Read it in.
        while (!feof(pFile))
        {
            cLines++;

            fgetws(sz, sizeof(sz), pFile);

            szWalk = sz;

            // Get the first word.
            szWord = NextWord(&szWalk);

            // Ignore comments and blank lines.
            if (!szWord || *szWord == '#')
            {
                continue;
            }

            // Process our commands.
            if (!_wcsicmp(szWord, L"!include"))
            {
                szWord = NextWord(&szWalk);

                if (!szWord)
                {
                    printf("%S(%d): ERROR: Syntax error.\n", szCurrentFile, cLines);
                    continue;
                }

                if (iTopOfStack == cFileStack)
                {
                    printf("%S(%d): ERROR: Too many levels of !includes.\n", szCurrentFile, cLines);
                    continue;
                }

                // Push the old file state.
                rgpFiles[iTopOfStack] = pFile;
                rgcLines[iTopOfStack] = cLines;
                wcscpy(rgszFileNames[iTopOfStack], szCurrentFile);

                iTopOfStack++;

                // First try the file as is.
                wcscpy(szCurrentFile, szWord);

                pFile = _wfopen(szCurrentFile, L"ra");

                if (!pFile)
                {
                    WCHAR *pch;

                    // Try in the same directly as the previous file.
                    wcscpy(szCurrentFile, rgszFileNames[iTopOfStack - 1]);

                    pch = wcsrchr(szCurrentFile, L'\\');

                    if (pch)
                    {
                        wcscpy(pch + 1, szWord);

                        pFile = _wfopen(szCurrentFile, L"ra");
                    }
                }

                if (!pFile)
                {
                    printf("%S(%d): ERROR: Unable to open the include file '%s'.\n", szCurrentFile, cLines, szWord);
                    return false;
                }

                cLines = 0;
            }
            else if (!_wcsicmp(szWord, L"FONT"))
            {
                WCHAR *szFileName = NULL;
                unsigned uStyle = 0;

                szWord = NextWord(&szWalk);

                if (!szWord)
                {
                    printf("%S(%d): ERROR: Syntax error.\n", szCurrentFile, cLines);
                    continue;
                }

                if (!_wcsicmp(szWord, L"NORMAL"))
                {
                    uStyle = XFONT_NORMAL;
                }
                else if (!_wcsicmp(szWord, L"BOLD"))
                {
                    uStyle = XFONT_BOLD;
                }
                else if (!_wcsicmp(szWord, L"ITALICS"))
                {
                    uStyle = XFONT_ITALICS ;
                }
                else if (!_wcsicmp(szWord, L"BOLDITALICS"))
                {
                    uStyle = XFONT_BOLDITALICS;
                }

                szFileName = szWord;
                szWord = NextWord(&szWalk);
                    
                // We should be at the end.
                if (szWord)
                {
                    printf("%S(%d): ERROR: Syntax error.\n", szCurrentFile, cLines);
                    continue;
                }

                // Duplicates?
                if (*s_szFontFileName)
                {
                    printf("%S(%d): ERROR: The font file name can only be specified once.\n", szCurrentFile, cLines);
                    continue;
                }

                unsigned iLen = 0;

                if (s_szFontDirectory)
                {
                    iLen = wcslen(s_szFontDirectory);

                    wcscpy(s_szFontFileName, s_szFontDirectory);
                    if (s_szFontDirectory[iLen - 1] != L':' && s_szFontDirectory[iLen - 1] != L'\\')
                    {
                        s_szFontFileName[iLen++] = L'\\';
                    }
                }

                wcscpy(s_szFontFileName + iLen, szFileName);
                s_uStyle = uStyle;
            }

            else if (!_wcsicmp(szWord, L"HEIGHT"))
            {
                szWord = NextWord(&szWalk);

                if (!szWord)
                {
                    printf("%S(%d): ERROR: Syntax error.\n", szCurrentFile, cLines);
                    continue;
                }

                l = wcstoul(szWord, NULL, 10);

                if (l == 0)
                {
                    printf("%S(%d): ERROR: Bad font height.\n", szCurrentFile, cLines);
                    continue;
                }
                else if (s_uCellHeight)
                {
                    printf("%S(%d): ERROR: The font height can only be set once.\n", szCurrentFile, cLines);
                    continue;
                }
                else
                {
                    s_uCellHeight = l;
                }
            }

            else if (!_wcsicmp(szWord, L"DEFAULT"))
            {
                szWord = NextWord(&szWalk);

                if (!szWord)
                {
                    printf("%S(%d): ERROR: Syntax error.\n", szCurrentFile, cLines);
                    continue;
                }

                l = wcstoul(szWord, NULL, 16);

                if (l == 0)
                {
                    printf("%S(%d): ERROR: Bad default character.\n", szCurrentFile, cLines);
                    continue;
                }
                else if (s_uDefaultCharacter)
                {
                    printf("%S(%d): ERROR: The default character can only be set once.\n", szCurrentFile, cLines);
                    continue;
                }
                else
                {
                    s_uDefaultCharacter = l;
                }
            }

            else if (!_wcsicmp(szWord, L"ANTIALIASLEVEL"))
            {
                szWord = NextWord(&szWalk);

                if (!szWord)
                {
                    printf("%S(%d): ERROR: Syntax error.\n", szCurrentFile, cLines);
                    continue;
                }

                l = wcstoul(szWord, NULL, 10);

                if (l != 0 && l != 2 && l != 4)
                {
                    printf("%S(%d): ERROR: Bad antialias level value.\n", szCurrentFile, cLines);
                    continue;
                }
                else if (s_fAntialiasLevelSet)
                {
                    printf("%S(%d): ERROR: The antiailias level can only be set once.\n", szCurrentFile, cLines);
                    continue;
                }

                s_uAntialiasLevel = l;
                s_fAntialiasLevelSet = true;
            }

            else if (!_wcsicmp(szWord, L"RLEWIDTH"))
            {
                szWord = NextWord(&szWalk);

                if (!szWord)
                {
                    printf("%S(%d): ERROR: Syntax error.\n", szCurrentFile, cLines);
                    continue;
                }

                l = wcstoul(szWord, NULL, 10);

                if (l < 2 || l > 8)
                {
                    printf("%S(%d): ERROR: Bad RLE width value.\n", szCurrentFile, cLines);
                    continue;
                }
                else if (s_uRLEWidth)
                {
                    printf("%S(%d): ERROR: The RLE width can only be set once.\n", szCurrentFile, cLines);
                    continue;
                }

                s_uRLEWidth = l;
            }

            // Store the character.
            else
            {
                // Skip a leading 'U+'.
                if (*szWord == 'U' && *(szWord + 1) == '+')
                {
                    szWord += 2;
                }

                l = wcstoul(szWord, NULL, 16);

                if (!l)
                {
                    // See if this is actually the number zero or if we have some other funky
                    // thing.
                    //
                    while (isalnum(*szWord))
                    {
                        if (*szWord != '0')
                        {
                            printf("%S(%d): ERROR: Syntax error.\n", szCurrentFile, cLines);
                            break;
                        }

                        szWord++;
                    }

                    // The above check failed, move to the next line.
                    if (isalnum(*szWord))
                    {
                        continue;
                    }
                }

                s_cCharacters++;
                s_rgwCharset[CHAR_SEGMENT(l)] |= 1 << CHAR_OFFSET(l);
            }
        }

        fclose(pFile);
        pFile = NULL;

        if (iTopOfStack)
        {
            iTopOfStack--;

            pFile = rgpFiles[iTopOfStack];
            cLines = rgcLines[iTopOfStack];
            wcscpy(szCurrentFile, rgszFileNames[iTopOfStack]);
        }
    }

    // Have a file?
    if (s_szFontFileNameCL)
    {
        if (*s_szFontFileName)
        {
            printf("WARNING: The command line is overriding the 'FONT' input command.\n");
        }

        wcscpy(s_szFontFileName, s_szFontFileNameCL);
        s_uStyle = 0;
    }
    else if (!*s_szFontFileName)
    {
        printf("ERROR: No fonts specified in the input file.\n");
        return false;
    }

    // Style?
    if (s_uStyleCL)
    {
        if (s_uStyle)
        {
            printf("WARNING: The command line is overriding the style of the 'FONT' input command.\n");
        }

        s_uStyle = s_uStyleCL;
    }
    else if (!s_uStyle)
    {
        s_uStyle = XFONT_NORMAL;
    }

    // Height set?
    if (s_uCellHeightCL)
    {
        if (s_uCellHeight)
        {
            printf("WARNING: The command line is overriding the 'HEIGHT' input command.\n");
        }

        s_uCellHeight = s_uCellHeightCL;
    }
    else if (!s_uCellHeight)
    {
        printf("ERROR: The font height has not been set.\n");
        return false;
    }

    // Default character?
    if (s_uDefaultCharacterCL)
    {
        if (s_uDefaultCharacter)
        {
            printf("WARNING: The command line is overriding the 'DEFAULT' input command.\n");
        }

        s_uDefaultCharacter = s_uDefaultCharacterCL;
    }
    else if (!s_uDefaultCharacter)
    {
        printf("ERROR: The default character has not been set.\n");
        return false;
    }

    if (!(s_rgwCharset[CHAR_SEGMENT(s_uDefaultCharacter)] & (1 << CHAR_OFFSET(s_uDefaultCharacter))))
    {
        printf("ERROR: The default character must be one of the characters specified in the file.\n");
        return false;
    }

    // Set the default alpha.
    if (s_fAntialiasLevelCLSet)
    {
        if (s_fAntialiasLevelSet)
        {
            printf("WARNING: The command line is overriding the 'ANTIALIASLEVEL' input command.\n");
        }

        s_uAntialiasLevel = s_uAntialiasLevelCL;
    }

    // Set the default alpha.
    if (s_uRLEWidthCL)
    {
        if (s_uRLEWidth)
        {
            printf("WARNING: The command line is overriding the 'RLEWIDTH' input command.\n");
        }

        s_uRLEWidth = s_uRLEWidthCL;
    }
    return true;
}

//============================================================================
// Build the initial tables.  This does pretty much everything except
// produce the glyphs and fill in the offsets.
//============================================================================

static void BuildTables()
{
    unsigned iSegment;
    bool fInRun = false;

    // Walk through the segment table and identify all of the runs.
    for (iSegment = 0; iSegment < CHAR_SEGMENT_MAX; iSegment++)
    {
        // We just started/continued a run.
        if (s_rgwCharset[iSegment])
        {
            if (!fInRun)
            {
                fInRun = true;

                // Remember the start.
                s_rgSegmentRuns[s_cSegmentRuns].wFirstSegment = (WORD)iSegment;

                // Point to the next entry in the SegmentDescriptor table.
                s_rgSegmentRuns[s_cSegmentRuns].iSegmentTable = (WORD)s_cSegments;
            }

            // Build the segment descriptor.
            s_rgSegmentDescriptors[s_cSegments].iGlyph = (WORD)s_cGlyphs;
            s_rgSegmentDescriptors[s_cSegments].wCharMask = (WORD)s_rgwCharset[iSegment];

            // Remember the number of glyphs in this segment.
            unsigned uChars = s_rgwCharset[iSegment];

            while (uChars)
            {
                s_cGlyphs++;

                uChars &= (uChars - 1);
            }

            // We have another segment.
            s_rgSegmentRuns[s_cSegmentRuns].cSegments++;
            s_cSegments++;
        }

        // We just ended a run.
        else if (fInRun)
        {
            fInRun = false;
            s_cSegmentRuns++;
        }
    }

    // Make sure the last run is remembered.
    if (fInRun)
    {
        s_cSegmentRuns++;
    }

}

//============================================================================
// Open the font and generate all of the glyphs.
//============================================================================

static bool BuildGlyphs()
{
    HRESULT hr = NOERROR;

    XFONT *pFont;

    // Process the font for this style.
    hr = XFONT_OpenTrueTypeFont(s_szFontFileName, 0, &pFont);

    if (FAILED(hr))
    {
        printf("ERROR: Unable to initialize TrueType scaler.\n");
        return false;
    }

    // Set the size.
    hr = XFONT_SetTextHeight(pFont, s_uCellHeight);

    if (FAILED(hr))
    {
        printf("ERROR: Unable to initialize TrueType scaler.\n");
        return false;
    }

    // Set the antialiasing level.
    hr = XFONT_SetTextAntialiasLevel(pFont, s_uAntialiasLevel);

    if (FAILED(hr))
    {
        printf("ERROR: Unable to initialize TrueType scaler.\n");
        return false;
    }

    // Set the style.
    hr = XFONT_SetTextStyle(pFont, s_uStyle);

    if (FAILED(hr))
    {
        printf("ERROR: Unable to initialize TrueType scaler.\n");
        return false;
    }

    // Get the descent from the font scaler.  
    XFONT_GetFontMetrics(pFont, NULL, &s_uDescent);

    // Loop through each character and get its ABCs.  We do this three times
    // to find the best compression for this character set.
    //
    {
        unsigned uBitmapHeight;
        unsigned uBitmapWidth;
        unsigned uBitmapPitch;
        unsigned uBitmapSize;
        unsigned uAdvance;
        signed iBearingX;
        signed iBearingY;

        unsigned iCurrentTrySet = 0;
        unsigned iTryMin = 2;
        unsigned iTryMax = 5;

        s_rgpGlyphs = s_rgpGlyphsTry[0];
        s_rgdwGlyphOffsets = s_rgdwGlyphOffsetsTry[0];

        if (s_uRLEWidth)
        {
            iTryMin = s_uRLEWidth;
            iTryMax = s_uRLEWidth;
        }
        else
        {
            s_uRLEWidth = iTryMin;
        }

        for (unsigned iTry = iTryMin; iTry <= iTryMax; iTry++)
        {
            // Get the glyph table.
            Glyph **ppGlyph = s_rgpGlyphsTry[iCurrentTrySet];
            DWORD *pdwGlyphOffset = s_rgdwGlyphOffsetsTry[iCurrentTrySet];
            DWORD dwGlyphOffset = 0;

            // Set the encoding width.
            XFONT_SetRLEWidth(pFont, iTry);

            unsigned iSegment, iOffset;

            for (iSegment = 0; iSegment < CHAR_SEGMENT_MAX; iSegment++)
            {
                for (iOffset = 0; iOffset < CHAR_OFFSET_MAX; iOffset++)
                {
                    if (s_rgwCharset[iSegment] & (1 << iOffset))
                    {
                        WCHAR wch = MAKE_CHAR(iSegment, iOffset);

                        Glyph *pGlyphFromFont, *pGlyph;
                        unsigned cbGlyphSize;

                        // Get the size of the character.
                        hr = XFONT_GetCharacterData(pFont, 
                                                    wch,
                                                    &pGlyphFromFont,
                                                    &cbGlyphSize);

                        if (FAILED(hr))
                        {
                            printf("ERROR: Unable to convert character %04X.\n", wch);
                            return false;
                        }

                        if (wch != s_uDefaultCharacter && hr == S_FALSE)
                        {
                            printf("WARNING: Character %04X is not in the source font.\n", wch);
                        }

                        // Save the bitmap.
                        unsigned cbSize = cbGlyphSize;

                        pGlyph = (Glyph *)malloc(cbSize);

                        if (!pGlyph)
                        {
                            printf("ERROR: Out of memory.\n");
                            return false;
                        }

                        memcpy(pGlyph, pGlyphFromFont, cbSize);

                        if (pGlyph->uBitmapHeight > s_uMaxBitmapHeight)
                        {
                            s_uMaxBitmapHeight = pGlyph->uBitmapHeight;
                        }

                        if (pGlyph->uBitmapWidth > s_uMaxBitmapWidth)
                        {
                            s_uMaxBitmapWidth = pGlyph->uBitmapWidth;
                        }

                        // Set the info.
                        *ppGlyph = pGlyph;
                        *pdwGlyphOffset = dwGlyphOffset;

                        dwGlyphOffset += cbSize;

                        ppGlyph++;                
                        pdwGlyphOffset++;
                    }
                }
            }
            
            // Save the last offset so we can use it to calculate the glyph size
            // when we write it to a file.
            //
            *pdwGlyphOffset = dwGlyphOffset;

            // Is this smaller?
            if (dwGlyphOffset <= s_rgdwGlyphOffsets[s_cGlyphs])
            {
                s_uRLEWidth = iTry;
                s_rgpGlyphs = s_rgpGlyphsTry[iCurrentTrySet];
                s_rgdwGlyphOffsets = s_rgdwGlyphOffsetsTry[iCurrentTrySet];

                iCurrentTrySet = ~iCurrentTrySet & 1;
            }
        }
    }

    return true;
}

//============================================================================
// Writes bytes out to a binary file.
//============================================================================

static DWORD WriteBinary
(
    HANDLE hFile,
    void *pData,
    DWORD Size
)
{
    DWORD BytesWritten;

    if (!WriteFile(hFile, pData, Size, &BytesWritten, NULL))
    {
        return 0;
    }
    else
    {
        return BytesWritten;
    }
}

//============================================================================
// Writes bytes out to a code file.
//============================================================================

static DWORD s_uColumn;

static DWORD WriteCode
(
    HANDLE hFile,
    void *pData,
    DWORD Size
)
{
    char Buffer[16];
    BYTE *pByte = (BYTE *)pData;

    DWORD BytesWritten;
    DWORD TotalBytes = Size;

    while (Size)
    {
        if (s_uColumn % 32 == 0)
        {
            if (!WriteFile(hFile, "\r\n    ", 6, &BytesWritten, NULL))
            {
                return 0;
            }
        }

        sprintf(Buffer, "0x%02X, ", *pByte);

        if (!WriteFile(hFile, Buffer, 6, &BytesWritten, NULL))
        {
            return 0;
        }

        s_uColumn++;
        pByte++;
        Size--;
    }
    
    return TotalBytes;
}

//============================================================================
// Writes the font out to the font file.
//============================================================================

typedef DWORD (WRITEMETHOD)(HANDLE hFile, void *pData, DWORD Size);

static bool WriteFont()
{
    bool retval = false;
    HANDLE hFile;
    DWORD dwBytes;
    DWORD dwTotalBytes = 0;

    WRITEMETHOD *pfnWrite;

    char Buffer[256];

    // Open the file.
    hFile = CreateFile(s_szOutputFileName,
                       GENERIC_WRITE,
                       0,
                       NULL,
                       CREATE_ALWAYS,
                       FILE_ATTRIBUTE_NORMAL,
                       NULL);

    if (hFile == INVALID_HANDLE_VALUE)
    {
        printf("ERROR: Unable to open output file.\n");
        return false;
    }

    if (s_szCodeName)
    {
        pfnWrite = WriteCode;

        sprintf(Buffer, "_declspec(selectany) const BYTE %S[] =\r\n{", s_szCodeName);

        if (!WriteFile(hFile, Buffer, strlen(Buffer), &dwBytes, NULL))
        {
            printf("ERROR: Unable to write to output file.\n");
            goto Error;
        }
    }
    else
    {
        pfnWrite = WriteBinary;
    }

    // Build and write the header.
    FontHeader header;

    header.wSignature = (WORD)'XFNT';
    header.wVersion = FILE_VERSION;
    header.cGlyphs = (WORD)s_cGlyphs + 1;
    header.cSegmentRunTable = (WORD)s_cSegmentRuns;
    header.wDefaultChar = (WORD)s_uDefaultCharacter;
    header.uCellHeight = (BYTE)s_uCellHeight;
    header.uDescent = (BYTE)s_uDescent;
    header.uAntialiasLevel = (BYTE)s_uAntialiasLevel;
    header.uRLEWidth = (BYTE)s_uRLEWidth;
    header.uMaxBitmapHeight = (BYTE)s_uMaxBitmapHeight;
    header.uMaxBitmapWidth = (BYTE)s_uMaxBitmapWidth;

    dwBytes = pfnWrite(hFile, &header, sizeof(FontHeader));

    if (!dwBytes)
    {
        printf("ERROR: Unable to write to output file.\n");
        goto Error;
    }

    if (s_fVerbose)
    {
        dwTotalBytes += dwBytes;

        printf("Header            : %d bytes\n", dwBytes);
    }

    // Fix up the offset table and write it.
    {
        DWORD dwGlyphOffsetBase = sizeof(FontHeader) 
                                  + sizeof(DWORD) * (s_cGlyphs + 1)
                                  + sizeof(SegmentRun) * s_cSegmentRuns 
                                  + sizeof(SegmentDescriptor) * s_cSegments;

        unsigned i, c = s_cGlyphs;

        for (i = 0; i < c + 1; i++)
        {
            s_rgdwGlyphOffsets[i] += dwGlyphOffsetBase;
        }

        dwBytes = pfnWrite(hFile, s_rgdwGlyphOffsets, sizeof(DWORD) * (s_cGlyphs + 1));

        if (!dwBytes)
        {
            printf("ERROR: Unable to write to output file.\n");
            goto Error;
        }

        if (s_fVerbose)
        {
            dwTotalBytes += dwBytes;

            printf("Glyph offset table: %d entries for %d bytes\n", s_cGlyphs + 1, dwBytes);
        }
    }

    // Write the segment run table.
    dwBytes = pfnWrite(hFile, s_rgSegmentRuns, sizeof(SegmentRun) * s_cSegmentRuns);

    if (!dwBytes)
    {
        printf("ERROR: Unable to write to output file.\n");
        goto Error;
    }

    if (s_fVerbose)
    {
        dwTotalBytes += dwBytes;

        printf("Segment run table : %d entries for %d bytes\n", s_cSegmentRuns, dwBytes);
    }

    // Write the segment descriptor table.
    dwBytes = pfnWrite(hFile, s_rgSegmentDescriptors, sizeof(SegmentDescriptor) * s_cSegments);

    if (!dwBytes)
    {
        printf("ERROR: Unable to write to output file.\n");
        goto Error;
    }

    if (s_fVerbose)
    {
        dwTotalBytes += dwBytes;

        printf("Segment table     : %d entries for %d bytes\n", s_cSegments, dwBytes);
    }

    // Write out the glyphs.
    {
        unsigned i, c = s_cGlyphs;
        DWORD dwGlyphBytes = 0;

        for (i = 0; i < c; i++)
        {
            dwBytes = pfnWrite(hFile, s_rgpGlyphs[i], s_rgdwGlyphOffsets[i + 1] - s_rgdwGlyphOffsets[i]);

            if (!dwBytes)
            {
                printf("ERROR: Unable to write to output file.\n");
                goto Error;
            }

            dwGlyphBytes += dwBytes;
        }

        if (s_fVerbose)
        {    
            dwTotalBytes += dwGlyphBytes;

            printf("Glyphs            : %d glyphs for %d bytes using %d bit RLE packets\n\n", s_cGlyphs, dwGlyphBytes, s_uRLEWidth);
            printf("Total font size   : %d bytes\n", dwTotalBytes);
        }
    }

    if (s_szCodeName)
    {
        if (!WriteFile(hFile, "\r\n};\r\n", strlen("\r\n};\r\n"), &dwBytes, NULL))
        {
            printf("ERROR: Unable to write to output file.\n");
            goto Error;
        }
    }

    retval = true;

Error:
    CloseHandle(hFile);
                       
    return retval;
}

//============================================================================
// Main entrypoint.
//============================================================================

int __cdecl wmain(int argc, WCHAR **argv)
{
    // Process the command line.
    if (!ParseCommandLine(argc, argv))
    {
        Usage();
        return 1;
    }

    // Load the input file into memory.
    if (!LoadInputFile())
    {
        return 1;
    }

    // Build the lookup tables.
    BuildTables();

    // Open the font and create the glyphs.
    if (!BuildGlyphs())
    {
        return 1;
    }

    // Write out the font.
    if (!WriteFont())
    {
        return 1;
    }

    // Success!
    return 0;
}

