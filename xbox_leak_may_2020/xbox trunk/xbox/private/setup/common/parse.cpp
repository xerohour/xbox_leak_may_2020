//  PARSE.CPP
//
//  Created 28-Mar-2001 [JonT]

#include "common.h"
#include "parse.h"

//  ParseLine
//      Parses a single line of text by splitting the various 'clauses'
//      (separated by commas) in the line. Modifies the original string
//      by inserting zeros where the commas are. Returns a pointer past
//      the line and pointing to the first character of the next line.
//      If the pointer points to a zero, it encountered the zero while
//      parsing, generally meaning end of file.
//      Returns TRUE only at EOF.

BOOL ParseLine(
    LPSTR& lp,
    LPSTR* lplpParse,
    DWORD dwcMaxParse,
    DWORD* dwcActualParsed
    )
{
    DWORD i;

    // Clear the incoming buffer
    ZeroMemory(lplpParse, dwcMaxParse * sizeof (LPSTR*));
    if (dwcActualParsed != NULL)
        *dwcActualParsed = 0;

    // Skip any leading whitespace. If at EOF, bail.
    if (SkipWhitespace(lp))
        return TRUE;

    // Zap the proper number of commas.
    // If we reach EOF or EOL mid-loop,
    // we can return, but we need to first
    // point the remaining pointers to 
    // the termination.
    for (i = 0 ; i < dwcMaxParse ; ++i)
    {
        *lplpParse++ = lp;
        if (dwcActualParsed != NULL)
            ++*dwcActualParsed;
        if (ZapComma(lp))
        {
            //EOL then lp points to the beginning of the next
            //we need to terminate with the previous count.
            //EOF we just terminate by pointing to the final NULL.
            LPSTR lpTerminate = *lp ? (lp-1) : lp;
            while(++i < dwcMaxParse) *lplpParse++ = lpTerminate;
            return TRUE;
        }
    }

    // Since we got here, that means that there is still line left, but we've
    // exhausted the available pointers. Now just skip to end of line and zero
    // that
    while (*lp != '\n' && *lp != '\r')
    {
        if (*lp == 0)
            return TRUE;
        else
            ++lp;
    }
    *lp++ = 0;

    // Skip any additional whitespace
    return (*lp == 0);
}


//  ZapComma
//      Moves the passed in pointer to the next comma and zaps it.
//      Returns TRUE at end of line or end of file.
//      At EOF, *lp == 0.

BOOL
ZapComma(
    LPSTR& lp
    )
{
    // Look for the next comma
    while (*lp != ',')
    {
        // If we hit a zero, stop and flag end of file
        if (*lp == 0)
            return TRUE;

        // If we land on CR/LF, zap it and return pointing to that
        if (*lp == '\n' || *lp == '\r')
        {
            *lp++ = 0;
            return TRUE;
        }
        else
            ++lp;
    }

    // Nuke the comma
    *lp++ = 0;
    return FALSE;
}


//  SkipWhitespace
//      Skips past whitespace including comment lines.
//      Returns TRUE at EOF.

BOOL
SkipWhitespace(
    LPSTR& lp
    )
{
    // In case we get called at end of file, bail
    if (*lp == 0)
        return TRUE;

    // Loop until we have a good starting character
    while (TRUE)
    {
        // Skip any amount of whitespace or blank lines
        while (*lp == ' ' || *lp == '\t' || *lp == '\n' || *lp == '\r')
                ++lp;
    
        // Did we find a comment line? If so, skip the entire line
        if (*lp == ';')
        {
            while (*lp != '\n' && *lp != '\r' && *lp != '\0')
                ++lp;
            // Keep looping as there might be additional whitespace/comments
        }

        // Are we at EOF?
        else if (*lp == 0)
            return TRUE;

        // Otherwise, we have a good starting character
        else
            return FALSE;
    }
}

//  SkipRestOfLine
//      Reduces parsing errors by forcing a skip to the next newline.
//      Returns TRUE on EOF.

BOOL
SkipRestOfLine(
    LPSTR& lp
    )
{
    if (*lp == 0)
        return TRUE;

    // Skip to next newline
    while (*lp != '\n' && *lp != '\r')
    {
        if (*lp == 0)
            return TRUE;
        else
            ++lp;
    }

    // Now it's safe to skip whitespace since we know we are at EOL
    return SkipWhitespace(lp);
}

