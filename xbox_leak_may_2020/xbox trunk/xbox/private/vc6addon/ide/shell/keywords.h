#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#ifndef KEYWORDS_H
#define KEYWORDS_H

// The index file consists of a header, followed by the index entries.
// The header is made up of 26 longs that contain the position within
// the file of the index entries for the first index begining with an 
// 'a' thru the first index begining with a 'z'. The indices are not
// case-sensitive. Index entries that begin with a symbol immediately
// follow the header.
// 
// When a keyword is checked against the index file, the entire block
// of indices that contain the first letter of the keyword is read in,
// and a newline character is prepended. That block is searched for 
// "\n[Keyword]". So any occurance of an index that begins with the 
// same character sequence as the keyword triggers a hit.
// 
// As a simple optimization, I won't reload a block of indices that
// was loaded on the last search.

class CKeywords
{	
	private:
		char *m_pszFilename;
		long  m_nFileSize;
		char *m_pszIndexBuf;
		long  m_nIndexBufSize;
		long  m_nHeader[26];
	public:
		CKeywords(const char *pszFilename);
		~CKeywords();
		int IsKeyword(const char *pszKeyword);
};

#endif // KEYWORDS_H