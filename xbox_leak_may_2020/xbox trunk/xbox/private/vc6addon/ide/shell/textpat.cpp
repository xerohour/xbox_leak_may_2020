// textpat.cpp
//
// Created June 13, 1996 by paulde - Paul Chase Dempsey
//
// Text pattern recognition routines
//

#include "stdafx.h"

//////////////////////////////////////////////////////////////////////////
// Find regular expression
//
BOOL FREMatch( LPCTSTR sz, LPCTSTR szPat, ULONG & ibStart, ULONG & ibEnd )
{
    struct patType * ppat = RECompile((LPSTR)szPat, FALSE, reUnix);
    ASSERT(ppat);
    ibStart = ibEnd = 0;
    BOOL bLineMatch = FALSE;
    BOOL fFound = FALSE;

    fFound = RESearch(sz, &ibStart, &ibEnd, ppat, szPat, &bLineMatch);
    free(ppat);
    return fFound;
}


//////////////////////////////////////////////////////////////////////////

struct cbsz {
	int _cb;
	LPTSTR _sz;
	bool Match(LPCTSTR sz, int cb)
	{
		return (0 == _tcsnicmp(sz, _sz, _cb));
	}
};


// FUTURE: use registry, so protocol list can be customized/expanded
// INVESTIGATE: 
// Maybe there's a way to ask the system for a list of protocols
// It appears you could enumerate the keys in HKEY_CLASSES_ROOT 
// and collect the names of all keys with "URL Protocol" values.
//
// Another approach is to parse a candidate protocol, then query the registry
// for the existence of an "HKCR/<protocol>/URL Protocol" value.
//
// I found most of these in 
//   http://www.acl.lanl.gov/URI/archive/uri-94q3.messages/150.html
// which gives BNF for the content part of an URL.
//

cbsz rgzProtocols[] =
{
	// sorted in ascending order of length
	{ 3, _T("mk:")       }, // MoniKer

	{ 4, _T("ftp:")      }, // File Transfer Protocol
	{ 4, _T("mid:")      }, // Message ID (mail)
	{ 4, _T("cid:")      }, // Content ID (for MIME mail parts)
	{ 4, _T("afs:")      }, // Andrew File System

	{ 5, _T("file:")     },
	{ 5, _T("http:")     }, // HyperText Transfer Protocol
	{ 5, _T("nntp:")     }, // Network News Transfer Protocol
	{ 5, _T("news:")     }, // News
	{ 5, _T("wais:")     }, // Wide Area Information Search

	{ 6, _T("shell:")    }, // e.g. shell:desktop invokes the desktop docobject
	{ 6, _T("https:")    }, // HTTP Secure
	{ 6, _T("shttp:")    }, // Secure HTTP

	{ 7, _T("mailto:")   },
	{ 7, _T("gopher:")   },
	{ 7, _T("telnet:")   },
	{ 7, _T("tn3270:")   }, // This appears in the registry w/ IE 3 installed - terminal emulation session

	{ 8, _T("outlook:")  }, // Outlook (REN) -- Office's PIM/lightweight mail client

	{ 9, _T("prospero:") },

	{ 0, 0 }
};

// index of range by length
int rgiProtocols[] =
{
//	 1	 2	 3	 4	 5	 6	 7	 8	 9
	-1, -1,  0,  1,  5, 10, 13, 17, 18
};

#define CEL(rg) (sizeof rg/sizeof rg[0])

cbsz* IsProtocol(LPCTSTR rgb, int cb)
{
	if (cb < rgzProtocols[0]._cb ||
		cb > rgzProtocols[CEL(rgzProtocols) - 2]._cb)
		return NULL;

	int i = rgiProtocols[cb-1];
	if (i < 0)
		return NULL;
	for (; cb == rgzProtocols[i]._cb; i++)
	{
		if (rgzProtocols[i].Match(rgb, cb))
			return &rgzProtocols[i];
	}
	return NULL;
}

#ifdef _DEBUG
bool IsValidProtocolTable()
{
	bool bValid = true;
	// check rgzProtocols
	int cbLast = 0;
	int cbThis;
	for (int i = 0; cbThis = rgzProtocols[i]._cb; i++)
	{
		if (cbThis != cbLast)
		{
			if (rgiProtocols[cbThis-1] != i)
			{
				TRACE("IsValidProtocolTable: invalid index [%i]\n", cbThis-1);
				bValid = false;
			}
		}
		cbLast = cbThis;
		int cbActual = _tcsnbcnt(rgzProtocols[i]._sz, INT_MAX);
		if (cbThis != cbActual)
		{
			TRACE("IsValidProtocolTable: invalid entry [%i] <_cb %i, _sz '%s'> actual: %i\n",
				i, cbThis, rgzProtocols[i]._sz, cbActual );
			bValid = false;
		}
		if ((0 != rgzProtocols[i+1]._cb) && (cbThis > rgzProtocols[i+1]._cb))
		{
			TRACE("IsValidProtocolTable: entry [%i] out of order\n", i);
			bValid = false;
		}
	}
	return bValid;
}
#endif

//////////////////////////////////////////////////////////////////////////

BOOL FFindURL(LPCTSTR sz, ULONG ibAt, ULONG & ibStart, ULONG & ibEndProtocol, ULONG & ibEnd)
{
	ASSERT(IsValidProtocolTable());
	if (!*sz) // empty line
		return FALSE;
	ASSERT(ibStart <= _tcsnbcnt(sz, INT_MAX));
	ASSERT(ibAt == NO_HITTEST || ibAt < _tcsnbcnt(sz, INT_MAX));

    LPCTSTR szOpen  = _T("\"'<([{");
	LPCTSTR szClose = _T("\"'>)]}");
	_TCHAR chClose;

	LPCTSTR pchHit = (ibAt == NO_HITTEST) ? NULL : sz + ibAt;
	LPCTSTR pchStart;
	LPCTSTR pchScan;
	LPCTSTR pchColon;

	pchStart = sz + ibStart;

	for (;*pchStart;)
	{
		// starting past hit?
		if (pchHit && pchHit < pchStart)
			return FALSE;

		// find protocol part of URL
		for (;;)
		{
			chClose = 0;

			// Urls must contain a colon
			if (NULL == (pchColon = _tcschr(pchStart, _T(':'))))
				return FALSE;

			// ASSUME: smallest protocol name is 2 bytes
			if (pchColon - pchStart < 2)
				return FALSE;

			// Find possible start of protocol.
			// Scan left from colon for first nonprotocol char
			// According to the RFC, protocols never include multibyte 
			// chars, so it's safe to decrement by one.
			for (pchScan = pchColon -1; pchScan >= pchStart; pchScan--)
			{
				// valid protocol chars: a-z + - .
				if (!(_istalnum(*pchScan) || *pchScan == _T('-') || *pchScan == _T('.') || *pchScan == _T('+')))
				{
					// If the delimiting char is in our open quote set,
					//   set the matching close character.
					LPCTSTR pchOpen;
					if ((NULL != (pchOpen = _tcschr(szOpen, *pchScan))) &&
						(pchScan == pchStart || !_ismbblead(*(pchStart-1)))
						)
						chClose = szClose[pchOpen - szOpen];
					break;
				}
			}
			pchScan++; // beginning of protocol
			if (IsProtocol(pchScan, pchColon - pchScan + sizeof _T(':')))
				break; // found protocol!

			// look for next colon
			pchStart = pchColon+1;
		}

		// check if we're past the hit
		if (pchHit && (pchHit < pchScan))
			return FALSE;

		// set start of URL
		pchStart = pchScan;
		if (chClose)
			pchStart--; // include quote char in scanned string

		// scan remaining portion from the colon
		pchScan = pchColon + 1;
		if (chClose) // we're quoted
		{
			// just look for the closing char
			pchScan = _tcschr(pchScan, chClose);
			if (pchScan == NULL)
			{
				// didn't find a close quote -- try for an unquoted URL
				// reset scan pointer
				pchScan = pchColon + 1;
				// fall through to unquoted scan below
			}
			else
			{
				// found an URL!
				if ((NULL == pchHit) || (pchStart <= pchHit && pchHit <= pchScan))
				{
					// return extents of unquoted part
					ibStart = pchStart - sz + 1;
					ibEnd = pchScan - sz;
					ibEndProtocol = pchColon - sz + 1;
					return TRUE;
				}
				else
				{
					// keep looking
					pchStart = pchColon + 1;
					continue;
				}
			}
		}

		// not quoted. scan a token delimited by whitespace
		while (*pchScan && !_istspace(*pchScan))
			pchScan++;

		pchScan--;
		
		// trim trailing punctuation (interior punctuation ok)
		while (pchScan > pchColon && _istpunct(*pchScan))
		{
			pchScan = _tcsdec(pchColon, pchScan);
		}
		pchScan = _tcsinc(pchScan);

		// need at least one char past colon for a valid URL
		if (!(pchScan > pchColon + 1))
		{
			pchStart = pchColon + 1;
			continue;
		}

		// hittest if necessary
		if ((NULL == pchHit) || (pchStart <= pchHit && pchHit <= pchScan))
		{
			ibStart = pchStart - sz;
			ibEnd = pchScan - sz;
			ibEndProtocol = pchColon - sz + 1;
			return TRUE;
		}

		// not found -- restart scan at one char past the colon
		pchStart = pchColon + 1;
	}

	return FALSE;
}


