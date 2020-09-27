//  PARSE.H
//
//  Created 28-Mar-2001 [JonT]

#ifndef _PARSE_H_
#define _PARSE_H_

//  ParseLine
//      Parses a single line of text by splitting the various 'clauses'
//      (separated by commas) in the line. Modifies the original string
//      by inserting zeros where the commas are. Moves the pointer to
//      the end of line. If desired, call SkipRestOfLine to move to next line.
//      Returns TRUE at EOF (*lp == 0)

BOOL ParseLine(LPSTR& lp, LPSTR* lplpParse, DWORD dwcMaxParse, DWORD* dwcActual);

//  SkipWhitespace
//      Skips the current pointer over any whitespace, including comment lines
//      and blank lines

BOOL SkipWhitespace(LPSTR& lp);

//  ZapComma
//      Replaces a comma with a zero character and skips the pointer past it

BOOL ZapComma(LPSTR& lp);

//  SkipRestOfLine
//      Reduces parsing errors by forcing a skip to the next newline

BOOL SkipRestOfLine(LPSTR& lp);

#endif // #ifndef _PARSE_H_

