/**** EMDISASM.C - EM Lego disassembler interface                          *
 *                                                                         *
 *                                                                         *
 *  Copyright <C> 1995, Microsoft Corp                                     *
 *                                                                         *
 *  Created: September 18, 1995 by RafaelL                                 *
 *                                                                         *
 *  Revision History:                                                      *
 *                                                                         *
 *                                                                         *
 *                                                                         *
 ***************************************************************************/

#include "emdp.h"

#include <simpldis.h>
#define _CRTBLD
#include "undname.h"
#undef _CRTBLD

#define MAXL     20
#define CCHMAX   256

// these are defines taken from the compiler front-end
#define	LIMIT_DNAME_WSTRING_LENGTH	20
#define	LIMIT_DNAME_STRING_LENGTH	32

// offset is masm syntax, which means zero-prefixed if starts with A-F,
// h postfixed if =>10

LPSTR MasmOffset( DWORD dwOffset, LPSTR szResult )
{
	if (dwOffset==0)
	{
		*szResult = 0;
	}
	else
	{
		CHAR szNumber[10];
		_ultoa( dwOffset, szNumber, 16 );
		_strupr( szNumber );
		sprintf( szResult, "+%s%s%c", (szNumber[0]>'9') ? "0" : "", szNumber, (dwOffset>9) ? 'h' : 0 );
	}
	return szResult;
}

LOCAL int EncodedChar( const char *&p )
{
	int c = (int)(unsigned char)*p++;

	if (c!='?')
		return c;

	c = *p++;
	if ( (c>='0') && (c<='9') )
		return ",/\\:. \n\t'-"[ c-'0' ];

	if (c=='$')
	{
		// the masking is required due to a bug in V5 and pre-M3 V6 which was related to sign
		// extension and would output characters less than 'A', e.g.
		// 0x80 was output as "?$9A" instead of "?$IA"
		// mangling sources are in \\lang5\v5x.pro\src\cxxfe\sl\p1\c\outdname.c in DN_StringChar
		c = (unsigned char)((*p++) - 'A') & 0xF;
		c <<= 4;
		c |= (unsigned char)((*p++) - 'A') & 0xF;
		return c;
	}

	return c | 0x80;
}

// returns NULL if didn't like it
// length of result is:
// <string ""> 9
// Unicode worse case: L + 20 * "\n1234" = 121
// Ansi worse case: 32 * "?$XX" = 128
// Worse case = 138 (including terminator) which is well within limits (256) so no checks

LOCAL pchar_t ExtractString( LPTSTR pNewName, LPCTSTR pOldName, size_t /* nSize */, const ADDR& addr )
{
	assert( memcmp(pOldName, "\?\?_C@_",6)==0 );		// marker prefix

	const char *p = pOldName+6;
	bool bUnicode;
	switch (*p++)
	{
		case '0':
			bUnicode = false;
			break;
		case '1':
			bUnicode = true;
			break;
		default:
			assert(!"Strange unmangled string type");
			return NULL;
			break;
	}

	size_t nLength;
	if ( (p[1]!='@') && (p[1]<='9') )
	{
		// short length, single digit, no @ sign
		nLength = p[0]-'0';
		p++;
	}
	else
	{
		if (p[0]=='A')
		{
			nLength = 0;
			p+=2;
		}
		else if (p[0]<='9')
		{
			nLength = p[0]-'0'+1;
			p+=2;
		}
		else
		{
			nLength = 0;
			do
			{
				nLength = (nLength<<4) | (p[0]-'A');
				p++;
			}
			while (p[0]!='@');
			p++;
		}
	}

	// skip the checksum (whose value we don't care about)
	// if the checksum was ever 1-9 there would be no @ sign, cope with paranoid case
	if (*p>'9')
		while (*p!='@')
		{
			if (*p==0)
			{
				assert( !"Strange unmangled string" );
				return NULL;
			}
			p++;
		}
	p++;

	char *pOut = pNewName;
	strcpy( pOut, bUnicode ? "string L\"" : "string \"" );
	pOut += strlen(pOut);

	size_t i;
	bool bMore = false;

	// get displayable length, smaller of actual-terminator or limit
	if (bUnicode)
	{
		i = min( (nLength-1)/2, LIMIT_DNAME_WSTRING_LENGTH );
		bMore = (nLength>LIMIT_DNAME_WSTRING_LENGTH*2) ? true : false;
	}
	else
	{
		i = min( nLength-1, LIMIT_DNAME_STRING_LENGTH );
		bMore = (nLength>LIMIT_DNAME_STRING_LENGTH) ? true : false;
	}

#ifdef UNICODE
#error improve this code for Unicode
#endif

	while (i--)
	{
		int c = EncodedChar( p );

		if (bUnicode)
		{
			int lowbyte = EncodedChar( p );
			if ( (c!=0) || (lowbyte>0x7f) )
			{
				pOut += sprintf( pOut, "\\x%04x", (c<<8)|lowbyte );
				continue;
			}
			// simple conversion so do so
			c = lowbyte;
		}
		
		switch (c)
		{
			case 0:
				pOut += sprintf( pOut, "\\0" );
				break;
			case '\r':
				pOut += sprintf( pOut, "\\r" );
				break;
			case '\n':
				pOut += sprintf( pOut, "\\n" );
				break;
			case '\\':
				pOut += sprintf( pOut, "\\\\" );
				break;
			case '\t':
				pOut += sprintf( pOut, "\\t" );
				break;
			default:
				if ( (c<' ') || (c>0x7f) )
					pOut += sprintf( pOut, "\\x%02x", c );
				else
					*pOut++ = (char)c;
				break;
		}
	}

	strcpy( pOut, bMore ? "\"..." : "\"" );

	return pNewName;
}

LPSTR
_SHGetSymbol(
	LPADDR	addr1,
	SOP		sop,
	LPADDR	addr2,
	LPSTR	szName,
	LPDWORD	Delta,
	BOOL	bAddOffset
	)
{
	ODR		odr;
	LPSTR	lpstr;
	
	odr.lszName = szName;
	lpstr = SHGetSymbol (addr1, addr2, sop, &odr);

	if (Delta)
		*Delta = odr.dwDeltaOff;

	if (lpstr)
	{
		CHAR newname[512];		// same length as clients' buffers
		pchar_t pResult = NULL;

		// try and undecorate the symbol using (undocumented) CRT routine
		// Note you *cannot* pass NULL as the allocator, contrary to the 'docs'
		if (lpstr[0]=='?')
		{
			if (memcmp(lpstr+1,"\?_C@_",5)==0)
				pResult = ExtractString( newname, lpstr, sizeof(newname), *addr1 );
			else
				pResult = __unDName( newname, lpstr, sizeof(newname), malloc, free, UNDNAME_NAME_ONLY );
		}
		else if ((lpstr[0]=='@') && (lpstr[1]=='I') && (lpstr[2]=='L'))
		{
			// ILT symbols get generated on the fly by MSPDB, so turn
			// @ITL+0x1234(?Foothing) into an undecorated version
			LPSTR pOpen = _tcschr( lpstr, '(' );
			if (pOpen)
			{
				LPSTR pClose = _tcschr( pOpen+1, ')' );
				if (pClose)
				{
					CHAR noBrackets[512];
					size_t len = pClose-pOpen-1;
					memcpy( newname, pOpen+1, len );
					newname[len]=0;
					pResult = __unDName( noBrackets, newname, sizeof(noBrackets), malloc, free, UNDNAME_NAME_ONLY );
					if (pResult)
					{
						// put the unmangled version back together
						memcpy( szName, lpstr, pOpen-lpstr+1 );				// "@ILT+4("
						memcpy( szName + (pOpen-lpstr+1), noBrackets, _tcslen(noBrackets) );	// unmangled name
						_tcscpy( szName + (pOpen-lpstr+1)+_tcslen(noBrackets), ")" );
						pResult = NULL;				// prevent redundant copy below
					}
				}
			}
		}
		if (pResult)
			_tcscpy( szName, pResult );
	}
	
	if (lpstr && bAddOffset)
	{
		// we currently have "SymFoo", and an offset
		// we need to turn this into "SymFoo+masmoffset(0xrealaddr)"
		// and return an offset of zero to stop the disassembler from adding
		// spurious "+nnn" digits
		CHAR extra[25];
		CHAR szOffset[20];
		sprintf( extra, "%s (%08lx)", MasmOffset(*Delta, szOffset), GetAddrOff( *addr1 ) );
		_tcscat( lpstr, extra );
		*Delta = 0;
	}

	return lpstr;
}
	

XOSD Assemble (
    HPID hpid,
    HTID htid,
    LPADDR lpaddr,
    LPTSTR lszInput
    )
{
    return xosdUnsupported;
}


int
CvRegFromSimpleReg(
    MPT     mpt,
    int     regInstr
    )
{
    switch (mpt) {
    case mptix86:
        switch(regInstr) {
            case SimpleRegEax: return CV_REG_EAX;
            case SimpleRegEcx: return CV_REG_ECX;
            case SimpleRegEdx: return CV_REG_EDX;
            case SimpleRegEbx: return CV_REG_EBX;
            case SimpleRegEsp: return CV_REG_ESP;
            case SimpleRegEbp: return CV_REG_EBP;
            case SimpleRegEsi: return CV_REG_ESI;
            case SimpleRegEdi: return CV_REG_EDI;
        }
        break;

    case mptmips:
        return (regInstr + CV_M4_IntZERO);

    case mptdaxp:
        return (regInstr + CV_ALPHA_IntV0);

    case mptmppc:
        return (regInstr + CV_PPC_GPR0);

    }
    return (0);
}

int
SimpleArchFromMPT(
    MPT mpt
    )
{
    switch (mpt) {
        case mptix86:
            return Simple_Arch_X86;

        case mptmips:
            return Simple_Arch_Mips;

        case mptdaxp:
            return Simple_Arch_AlphaAxp;

        case mptmppc:
            return Simple_Arch_PowerPc;

        default:
            return -1;
    }
}



DWORDLONG
QwGetreg(
    PVOID   pv,
    int     regInstr
    )
{
    HTHD hthd = (HTHD)pv;

    XOSD        xosd;
    DWORDLONG   retVal;
    HPID        hpid = HpidFromHthd(hthd);

    xosd = GetRegValue(hpid,
                       HtidFromHthd(hthd),
                       CvRegFromSimpleReg(MPTFromHthd(hthd), regInstr),
                       &retVal
                       );

    return (xosd == xosdNone) ? retVal : 0;
}


size_t
WINAPI
CchRegrel(
    PVOID       pv,
    DWORD       ipaddr,
    int         reg,
    DWORD       offset,
    PCHAR       symbol,
    size_t      symsize,
    PDWORD      pDisp
    )
{
	HTHD hthd = (HTHD)pv;
 	
	CHAR    string[512];
    DWORD   dw;
    ADDR    AddrIP;
    ADDR    AddrData;
	MPT     mpt = MPTFromHthd(hthd);

	// ALPHA and PowerPC data access through registers other than SP
	int     cvReg = CvRegFromSimpleReg(mpt, reg);

	// Theoretically, you can get the value for the ALPHA GP register, or
	// for other non-SP registers when the current PC matches the 
	// instruction PC, and in some other cases,
	// but for now just fail for non-SP cases
	
	if ((mpt == mptdaxp && cvReg != CV_ALPHA_IntSP) || (mpt == mptmppc && cvReg != CV_PPC_GPR1))
	{
		return 0;
	}

	// Lock the thread only after we've interogated the thread
	// for any of the ALPHA or PPC cases above

    LPTHD   lpthd = (LPTHD)LLLock(hthd);
    AddrInit(&AddrIP,
             NULL,
             (SEGMENT)(lpthd->fFlat? 0 : (ipaddr >> 16)), //seg
             (lpthd->fFlat? ipaddr : (ipaddr & 0xffff)),
             lpthd->fFlat,
             lpthd->fOff32,
             0, // li
             lpthd->fReal);

    AddrInit(&AddrData,
             NULL,
             (SEGMENT)(lpthd->fFlat? 0 : (offset >> 16)), //seg
             (lpthd->fFlat? offset : (offset & 0xffff)),
             lpthd->fFlat,
             lpthd->fOff32,
             0,
             (USHORT)lpthd->fReal);

    LLUnlock(hthd);

 	// For x86 architecture SHGetSymbol assumes offsets are relative to ebp	
 	if ((!((SimpleArchFromMPT(mpt) == Simple_Arch_X86) && 
 		(CvRegFromSimpleReg(mpt,reg) != CV_REG_EBP)))
 		&& _SHGetSymbol (&AddrData, sopStack, &AddrIP, string,  &dw, FALSE )) {
    		_tcsncpy( symbol, string, symsize );
    		*pDisp = dw;
    	} else {
    		*pDisp = 0;
    		symbol[0] = 0;
    	} 
			
    return _tcslen(symbol);
}



unsigned int
CchAddr(
    PVOID       pv,
    ULONG       offset,
    char        *symbol,
    size_t      symsize,
    DWORD       *pDisp
    )
{
	HTHD hthd = (HTHD)pv;
	
    CHAR    string[512];
    DWORD   dw;
    ADDR    AddrIP;
    ADDR    AddrData;
    LPTHD   lpthd = (LPTHD)LLLock(hthd);

    AddrInit(&AddrData,
             NULL,
             (SEGMENT)(lpthd->fFlat? 0 : (offset >> 16)), //seg
             (lpthd->fFlat? offset : (offset & 0xffff)),
             lpthd->fFlat,
             lpthd->fOff32,
             0, // li
             (USHORT)lpthd->fReal);

    LLUnlock(hthd);

    AddrIP = AddrData;


       	if (_SHGetSymbol (&AddrData, sopNone, &AddrIP, string,  &dw, TRUE )) {
    		_tcsncpy( symbol, string, symsize );
    		*pDisp = dw;
    	} else {
    		*pDisp = 0;
    		symbol[0] = 0;
    	}
    	
    return _tcslen(symbol);
}


unsigned int
CchFixup(
    PVOID       pv,
    DWORD       ipaddr,
    ULONG       offset,
    size_t      size,
    PCHAR       symbol,
    size_t      symsize,
    DWORD       *pDisp
    )
{
	HTHD hthd = (HTHD)pv;
	
    CHAR    string[512];
    DWORD   dw;
    ADDR    AddrIP;
    ADDR    AddrData;
    LPTHD   lpthd = (LPTHD)LLLock(hthd);
    MPT     mpt = MPTFromHthd(hthd);

	// 16 bit displacement values cannot be fixed up on ALPHA or PowerPC
	// 16 bit register offsets are picked up in CchRegrel
	if (size < sizeof(DWORD) && (mpt == mptdaxp || mpt == mptmppc))
	{
		return 0;
	}


    AddrInit(&AddrIP,
             NULL,
             (SEGMENT)(lpthd->fFlat? 0 : (ipaddr >> 16)), //seg
             (lpthd->fFlat? ipaddr : (ipaddr & 0xffff)),
             lpthd->fFlat,
             lpthd->fOff32,
             0, // li
             (USHORT)lpthd->fReal);

    AddrInit(&AddrData,
             NULL,
             (SEGMENT)(lpthd->fFlat? 0 : (offset >> 16)), //seg
             (lpthd->fFlat? offset : (offset & 0xffff)),
             lpthd->fFlat,
             lpthd->fOff32,
             0,
             (USHORT)lpthd->fReal);

    LLUnlock(hthd);

	if (size==sizeof(DWORD))
	{
		// go get the DWORD at address 'offset'
		DWORD dw;
		DWORD res;
		XOSD xosd = ReadBuffer( HpidFromHthd(hthd), HtidFromHthd(hthd), &AddrData, size, (LPBYTE)&dw, &res );
		if (xosd==xosdNone)
			SetAddrOff( &AddrData, dw );
		else
		{
			// couldn't read the memory
			*pDisp = 0;
			symbol[0] = 0;
			return 0;
		}
	}

    	if (_SHGetSymbol (&AddrData, sopNone, &AddrIP, string,  &dw, TRUE )) {
    		_tcsncpy( symbol, string, symsize );
    		*pDisp = dw;
    	} else {
    		*pDisp = 0;
    		symbol[0] = 0;
    	}
    	
    return _tcslen(symbol);
}

int
AddString(
    LPSTR *ppchOut,
    LPINT  pichCur,
    LPINT  pcchMax,
    LPSTR  string
    )

/*++

Routine Description:

    Add a string to a packed list of strings.

Arguments:

    ppchOut - Supplies a pointer to a pointer to the position of the next
        entry in the string.  Returns the new position after this string
        is added.

    pichCur - Supplies a pointer to the current index in the string.  Returns
        the new index.

    pcchMax - Supplies a pointer to the number of characters available in the
        string.  Returns the new value of same.

    string - Supplies the string to add to the list.

Return Value:

    Index to beginning of string (original ichCur).  Returns -1 if the string
    could not be added.

--*/
{
    int r = -1;
    int l = _tcslen(string);
    if (l >= *pcchMax) {
        l = *pcchMax - 1;
    }
    if (l >= 0) {
        _tcsncpy(*ppchOut, string, l);
        (*ppchOut)[l] = 0;
        *pcchMax -= (l+1);
        r = *pichCur;
        *pichCur += (l+1);
        *ppchOut += (l+1);
    }
    return r;

}

XOSD
Disasm (
    HPID   hpid,
    HTID   htid,
    LPSDI  lpsdi
    )
{
    XOSD        xosd      = xosdNone;
    DWORD       dop       = lpsdi->dop;
    DWORD       cb;
    int         cbUsed    = 0;
    ADDR        addr;
    BYTE        rgb[MAXL];
    PBYTE       prgb;
    int         Bytes;
    DWORD       dwTgtMem;
    SIMPLEDIS   Sdis;
    HPRC        hprc = ValidHprcFromHpid(hpid);
    HTHD        hthd = HthdFromHtid( hprc, htid );
    MPT         mpt = MPTFromHthd(hthd);

    static char String[CCHMAX];
    LPSTR       lpchOut;
    int         ichCur;
    int         cchMax;




    lpsdi->ichAddr      = -1;
    lpsdi->ichBytes     = -1;
    lpsdi->ichOpcode    = -1;
    lpsdi->ichOperands  = -1;
    lpsdi->ichComment   = -1;
    lpsdi->ichEA0       = -1;
    lpsdi->ichEA1       = -1;
    lpsdi->ichEA2       = -1;

    lpsdi->cbEA0        =  0;
    lpsdi->cbEA1        =  0;
    lpsdi->cbEA2        =  0;

    lpsdi->fAssocNext   =  0;

    lpsdi->lpch         = String;



    //ADDR_IS_FLAT( addrStart ) = TRUE;

    //if (!Memory) {
        xosd = ReadBuffer(hpid, htid, &lpsdi->addr, MAXL, rgb, (unsigned long *) &cb);
        if (xosd != xosdNone) {
            cb = 0;
        }
        prgb = rgb;
    //}
    //else {
        //prgb = (BYTE *) Memory;
        //cb  = (DWORD)MemorySize;
    //}

    if ( cb == 0 ) {

        //
        // Even if we can't read memory we still need to send back the formatted address
        // so the shell can display it.  
        // HACK HACK: We make up the address string here from the passed in address. We need to 
        // come up with a better solution so we don't have to keep the address formatting 
        // between the disassembler and the EM in sync. [sanjays]

        if ((dop & dopAddr) || (dop & dopFlatAddr)) {

            //
            // address of instruction
            //
            sprintf(String,"%08X", GetAddrOff(lpsdi->addr)); 
            lpsdi->ichAddr = 0;
        }

        //cbUsed = 0;
        cbUsed = 1;
        xosd = xosdGeneral;

    } else {

		if (dop & dopSym) {
        Bytes = SimplyDisassemble(
            prgb,                         // code ptr
            cb,                           // bytes
            GetAddrOff(lpsdi->addr),
            SimpleArchFromMPT(mpt),
            &Sdis,
            CchAddr,
            CchFixup,
            CchRegrel,
            QwGetreg,
            (PVOID)hthd
            );
		} else {
		Bytes = SimplyDisassemble(
            prgb,                         // code ptr
            cb,                           // bytes
            GetAddrOff(lpsdi->addr),
            SimpleArchFromMPT(mpt),
            &Sdis,
			NULL,
            NULL,
            NULL,
            QwGetreg,
            (PVOID)hthd
            );
		}


        if (Bytes < 0) {
            cbUsed = -Bytes;
            //xosd = xosdGeneral;
        } else {
            cbUsed = Bytes;
        }

        //
        // unpack Sdis
        //


        //
        // fill in addresses, whether asked for or not
        //

        lpsdi->cbEA0 = Sdis.cbEA0;
        lpsdi->cbEA1 = Sdis.cbEA1;
        lpsdi->cbEA2 = Sdis.cbEA2;

        if (lpsdi->cbEA0) {
            AddrInit( &lpsdi->addrEA0,
                      NULL,
                      0,    // SEG
                      Sdis.dwEA0,
                      1,    // flat
                      1,    // off32
                      0,    // LI
                      0     // real
                      );
        }

        if (lpsdi->cbEA1) {
            AddrInit( &lpsdi->addrEA1,
                      NULL,
                      0,    // SEG
                      Sdis.dwEA1,
                      1,    // flat
                      1,    // off32
                      0,    // LI
                      0     // real
                      );
        }

        if (lpsdi->cbEA2) {
            AddrInit( &lpsdi->addrEA2,
                      NULL,
                      0,    // SEG
                      Sdis.dwEA2,
                      1,    // flat
                      1,    // off32
                      0,    // LI
                      0     // real
                      );
        }


        //
        // initialize packed string
        //
        lpchOut   = String;
        ichCur    = 0;
        cchMax    = CCHMAX;

        if ((dop & dopAddr) || (dop & dopFlatAddr)) {

            //
            // address of instruction
            //

            lpsdi->ichAddr = AddString(&lpchOut, &ichCur, &cchMax, Sdis.szAddress);

        }

        if (dop & dopRaw) {

            //
            // Raw bytes
            //

            lpsdi->ichBytes = AddString(&lpchOut, &ichCur, &cchMax, Sdis.szRaw);

        }

        if (dop & dopOpcode) {

            //
            // opcode...
            //

            lpsdi->ichOpcode = AddString(&lpchOut, &ichCur, &cchMax, Sdis.szOpcode);

        }

        if (dop & dopOperands) {

            //
            // operands...
            //

            lpsdi->ichOperands = AddString(&lpchOut, &ichCur, &cchMax, Sdis.szOperands);

        }

        {

            //
            // comment
            //

            lpsdi->ichComment   = -1;

        }

        if (dop & dopEA) {

            //
            // show EA(s)
            //

            if (lpsdi->cbEA0) {
                lpsdi->ichEA0 = AddString(&lpchOut, &ichCur, &cchMax, Sdis.szEA0);
            }

            if (lpsdi->cbEA1) {
                lpsdi->ichEA1 = AddString(&lpchOut, &ichCur, &cchMax, Sdis.szEA1);
            }

            if (lpsdi->cbEA2) {
                lpsdi->ichEA2 = AddString(&lpchOut, &ichCur, &cchMax, Sdis.szEA2);
            }
        }

    }

    GetAddrOff ( lpsdi->addr ) += cbUsed;
 	lpsdi->addr.emi = 0;

    return xosd;
}



XOSD
BackDisasm(
    HPID hpid,
    HTID htid,
    LPGPIS lpgpis
    )

/*++

Routine Description:

    This will find the instruction which ends nearest to the supplied address
    without consuming the supplied address.

    On machines with fixed instruction size and alignment, this is trivially
    accomplished with arithmetic.

    On machines with variable instruction size, this is done by disassembling
    instructions until a closest fit is found.

    This implementation will not consume the supplied address, but will accept
    either a) the first (longest) match which consumes the byte before the
    address, or b) the match which ends closest to the address.

Arguments:

    hpid -

    htid -

    lpgpis -


Return Value:


--*/
{
    HPRC        hprc = ValidHprcFromHpid(hpid);
    HTHD        hthd = HthdFromHtid( hprc, htid );
    MPT         mpt = MPTFromHthd(hthd);
    SIMPLEDIS   Sdis;

    if (GetAddrOff(*lpgpis->lpaddr) == 0) {

        return xosdBadAddress;

	} else if (mpt != mptix86) {
        //
		// all but X86 can assume DWORD alignment opcodes
        //

        SetAddrOff((lpgpis->lpaddr), GetAddrOff(*(lpgpis->lpaddr))-4);
        *(lpgpis->lpuoffset) = GetAddrOff(*(lpgpis->lpaddr)) & 3;
        SetAddrOff((lpgpis->lpaddr),
                 GetAddrOff(*(lpgpis->lpaddr)) - *(lpgpis->lpuoffset));

        return xosdNone; // Hack for MIPS&Alpha doesn't check page r/w
    } else {

        //
		// x86 is more painful
		// we start 20 bytes before and disassemble forwards until we hit it
		// if we miss it, we start again from the next byte	down
        //

        const int arch = SimpleArchFromMPT(mpt);
		const int X86_BACK_MAX = 20;
		UOFF32 endOffset = GetAddrOff(*lpgpis->lpaddr);		// where we want to end up
		ADDR startAddr = *lpgpis->lpaddr;
        DWORD cbTry;
		BYTE rgb[X86_BACK_MAX];

		// we start X86_BACK_MAX bytes before (checking that we're not too early)
		if (GetAddrOff(startAddr) < X86_BACK_MAX) {
			GetAddrOff(startAddr) = 0;
		} else {
			GetAddrOff(startAddr) -= X86_BACK_MAX;
        }

        XOSD xosd = ReadBuffer(hpid, htid, &startAddr, X86_BACK_MAX, rgb, &cbTry);

        if (xosd != xosdNone) {
            //
            // nothing there.  just decrement 1 and return.
            //
            SetAddrOff( lpgpis->lpaddr, endOffset - 1 );
            return xosd;
        }



        LPBYTE pbTry = rgb;
		UOFF32 tryOffset = GetAddrOff(startAddr);

		while (cbTry > 0) {

            LPBYTE pbCurr = pbTry;
            DWORD cbCurr = cbTry;
			UOFF32 currOffset = tryOffset;

			while (1) {

                int Bytes = SimplyDisassemble(pbCurr,
                                              cbCurr,
                                              currOffset,
                                              arch,
                                              &Sdis,
                                              CchAddr,
                                              CchFixup,
                                              CchRegrel,
                                              QwGetreg,
                                              (PVOID)hthd
                                              );
                if (Bytes < 0) {

                    //
                    // no instruction found - slide ahead one
                    // byte and start again.
                    //

                    pbTry += 1;
                    cbTry -= 1;
                    tryOffset += 1;
                    break;
                }

                if (Bytes == 0) {

                    //
                    // this is not supposed to happen
                    //

                    assert(!"SimplyDisassemble returned 0!");
                    SetAddrOff( lpgpis->lpaddr, endOffset - 1 );
                    return xosdGeneral;

                }

                if (Bytes > (int)cbCurr) {

                    //
                    // this is not supposed to happen
                    //

                    assert(!"SimplyDisassemble consumed too many bytes!");
                    SetAddrOff( lpgpis->lpaddr, endOffset - 1 );
                    return xosdGeneral;

                }

                if (Bytes < (int)cbCurr) {

                    //
                    // so far, so good.
                    //

                    cbCurr -= Bytes;
                    pbCurr += Bytes;
                    currOffset += Bytes;
                    continue;
                }


                if (Bytes == (int)cbCurr) {

                    //
                    // perfect fit
                    //

                    // currOffset is the one we want.

                    SetAddrOff(lpgpis->lpaddr, currOffset);
                    return xosdNone;
                }
				
			}

        }

        //
		// didn't find anything at all, so bail out and
        // pretend it's a one byte op-code
        //

		SetAddrOff( lpgpis->lpaddr, endOffset - 1 );
		return xosdNone;
	}

}

