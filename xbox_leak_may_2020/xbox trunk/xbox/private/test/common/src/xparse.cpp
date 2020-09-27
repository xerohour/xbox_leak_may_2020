//-----------------------------------------------------------------------------
// File: parse.cpp
//
// Desc: Routines to parse a string into distinct tokens.
//       Should probably be called tokenize...
//
// Copyright (c) 2000-2001 Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#include <string.h>
#include "xparse.h"

Token tokens[MAX_TOKENS];

//-----------------------------------------------------------------------------
// Name: ParseTok
// Desc: Parse a string into its relevant token from the Parse Block
//-----------------------------------------------------------------------------
int ParseTok(char *string, struct sParseBlk *pb)
{
	int idx;

	idx = 0;

	while(pb[idx].val!=TOK_ERR)
	{
		if(!_stricmp(string, pb[idx].str))
			return pb[idx].val;

		idx++;
	}

	return TOK_ERR;
}

//-----------------------------------------------------------------------------
// Name: Parse
// Desc: Generate distinct tokens from a string
//-----------------------------------------------------------------------------
int	Parse(char *string, struct sParseBlk *pb)
{
	int	i, ntok;
	char *sp, *tp;
	char parsestr[256];

	strncpy(parsestr, string, 256);
	_strupr(parsestr);
	sp = parsestr;

	ntok = 0;

	while ((*sp!=0) && (ntok<MAX_TOKENS) && (*sp!=';') && (*sp!='\n')) 
	{
		while(*sp==' ' || *sp=='\t')
			sp++;

		tp = tokens[ntok].str;
		while(  (*sp!=0) && (*sp!=' ') && (*sp!='\t') && 
				(*sp!=',') && (*sp!='\n') && (*sp!='(') &&
				(*sp!=')') && (*sp!=13) )
			*tp++ = *sp++;

		*tp = 0;
		ntok++;

		if(*sp)
			sp++;
	}

	// parse all the tokens
	if(pb)
		for(i=0; i<ntok; i++)
			tokens[i].val = ParseTok(tokens[i].str, pb);

	return ntok;
}

