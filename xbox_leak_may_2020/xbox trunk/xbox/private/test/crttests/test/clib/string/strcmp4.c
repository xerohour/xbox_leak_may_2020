
/***************************************************************************

		   Copyright (c) 1994 Microsoft Corporation

Test:       strcmp

Abstract:   Verify strcmp()with strings that contain control characters.

===========================================================================
Syntax

===========================================================================
Priority:               1
Products:               WIN NT


Revision History:

    Date        emailname   description
----------------------------------------------------------------------------
    11-May-94   a-timke     created
----------------------------------------------------------------------------

EndHeader:

****************************************************************************/
#include <string.h>
#include <test.h>

static char    test[] = __FILE__;
static int     perfect=0;

typedef struct TSTRINGTEST
{
  char *String1;
  char *String2;
  int   ExpectedResult;
} tStringTest;

//
// Tests for strcmp().
//
int strcmp4Entry ( void ){
  static tStringTest rgStringTest[] =
  {    
    {"Strings that are the same.\n",     "Strings that are the same.\n", 0},
    {"Strings that are not the same.\n", "Strings THAT are not same.\n", 1},
    {"Strings THAT are not the same.\n", "Strings that are not same.\n", -1},

    {"\n",       "\n",          0},
    {"\n",       "",            1},
    {"",         "\n",         -1},

    {"\a",       "\a",          0},
    {"\a",       "",            1},
    {"",         "\a",         -1},

    {"\a\b\a\b", "\a\b\a\b",    0},
    {"\a\b\a\b", "",            1},
    {"",         "\a\b\a\b",   -1},

    {"a\a\bb",   "a\a\bb",      0},
    {"ab",       "a\a\bb",      1},
    {"a\a\bb",   "ab",         -1},

    {"\n\v\n\v", "\n\v\n\v",    0},
    {"\n\v\n\v", "",            1},
    {"",         "\n\v\n\v",   -1},

    {"\a\n\vb", "\a\n\vb",      0},
    {"ab",      "\a\n\vb",      1},
    {"\a\n\vb", "ab",          -1},

    {"\r\n\n\r\n", "\r\n\n\r\n", 0},
    {"\r\n\n\r\n", "\r\n",       1},
    {"\r\n",       "\r\n\n\r\n", -1},
  };
  int i;
  int iReturn, iReturnSave;
  startest ();

  for (i=0; i<sizeof(rgStringTest)/sizeof(tStringTest); i++)
  {
    tStringTest *pTest = &rgStringTest[i];
    iReturn = iReturnSave = strcmp(pTest->String1, pTest->String2);

    if (iReturn < -1)
      iReturn = -1;
    else if (iReturn > 1)
      iReturn = 1;

    if (iReturn != pTest->ExpectedResult)
    {
      fail(i);
      printf("Failure: strcmp(\"%s\", \"%s\") returned=%d, expected=%d\n",
         pTest->String1, pTest->String2, iReturnSave, pTest->ExpectedResult);
    }
  }

  finish ();
}
