/////////////////////////////
// NCBAPI.H
//		NCB Package interface declaration
//

#ifndef __NCBAPI_H__
#define __NCBAPI_H__

PdbInterface INCB;

typedef INCB	* LPNCB;

///////////////////////////////////
// INCB
//
#include <ncparex.h>

// CLSID_NCB 
//
// {99EDEFAF-9B02-11D0-A51B-00A0C91F381B}
//

extern "C" const __declspec(selectany) CLSID CLSID_NCB = { 0x99EDEFAF, 0x9B02, 0x11D0, { 0xA5, 0x1B, 0x00, 0xA0, 0xC9, 0x1F, 0x38, 0x1B } };

// INCB
//

__interface __declspec(uuid("{9ADE0EB0-6273-11cf-BF4F-00AA006C2891}")) INCB : public IUnknown
{
	// INCB methods
	STDMETHOD(OpenNcbForReadFrName)(SZ szName, HTARGET hTarget, SZ szTarget, BOOL bWrite, Bsc ** ppBsc);
	STDMETHOD(OpenNcbForReadFrPdb)(PDB * ppdb, HTARGET hTarget, SZ szTarget, Bsc ** ppBsc);
	STDMETHOD(OpenNcbForWriteFrName)(SZ szName, BOOL bWrite, NcbParseEx ** ppNcParse);
	STDMETHOD(OpenNcbForWriteFrPdb)(PDB * ppdb, NcbParseEx **ppNcParse);

};

#if !defined(_NCB_COM_)

// This is so that the ncb package can be loaded with a LoadLib from the ycb\driv project
//

HRESULT OpenNcbForReadFrName(SZ szName, HTARGET hTarget, SZ szTarget, BOOL bWrite, Bsc ** ppBsc);
HRESULT OpenNcbForReadFrPdb(PDB * ppdb, HTARGET hTarget, SZ szTarget, Bsc ** ppBsc);
HRESULT OpenNcbForWriteFrName(SZ szName, BOOL bWrite, NcbParseEx ** ppNcParse);
HRESULT OpenNcbForWriteFrPdb(PDB * ppdb, NcbParseEx **ppNcParse);

#endif // _NCB_COM_

#endif // __NCBAPI_H__
