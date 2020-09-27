//-----------------------------------------------------------------------------
// File: xparse.h
//
// Copyright (c) 2000-2001 Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#ifndef __PARSE_H
#define __PARSE_H

#ifdef __cplusplus
extern "C" {
#endif

#define MAX_TOKENS 32
#define MAX_TOKENSIZE 40
#define TOK_ERR -1

typedef struct sParseBlk
{
	char str[32];
	int val;
} ParseBlock;

typedef struct sToken
{
	char str[MAX_TOKENSIZE];	// token string
	int val;					// token value
} Token;

extern Token tokens[MAX_TOKENS];

int	Parse(char *string, struct sParseBlk *pb);
int ParseTok(char *string, struct sParseBlk *pb);

#ifdef __cplusplus
}
#endif

#endif