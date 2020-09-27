// helper.h
// header for helper.cpp
//	contains helper functions shared between bsc and ncb
////////////////////////////////////////////////////////
#ifndef __HELPER_H__
#define __HELPER_H__

#include <array_t.h>

void MaskFrNi (NI ni, NI niMax, const USHORT cBytes, int *pib, BYTE *pbMask);
SZ szFrNi (NameMap * pnm, NI ni);

//////////////////////////////////////////////////////////
//	MaskFrNi()
// Create a masking and byte index given NI and NI max
//////////////////////////////////////////////////////////
inline void MaskFrNi(NI ni, NI niMax, const USHORT cBytes, int *pib, BYTE *pbMask) 
{
	// find scaling factor for this module...
	int iDiv = 1 + niMax / (cBytes * 8);
	ni /= iDiv;

	*pbMask = 1<<(ni%8);	// bottom 3 bits to form the mask
	ni /= 8;				// now remove the 3 position bits
	ni %= cBytes;		// then modulo the remaining bits
	*pib = ni;
};

///////////////////////////////////////
// szFrNi ()
// get the name sz from given name map
///////////////////////////////////////
inline SZ szFrNi(NameMap * pnm, NI ni)
{
	SZ_CONST sz;
	verify(pnm->getName(ni, &sz));	// REVIEW: error?
	return (SZ)sz;
}

// copy from Array to regular array
template<class T> 
BOOL DupArray (T **ppNew, ULONG *pc, Array<T>& ar)
{
	*pc = ar.size();
	
	if (!*pc) {if (ppNew) *ppNew = NULL; return TRUE;	}
	
	CB cbAr = ar.size()*sizeof(T);
	if (*pc > 0)
	{
		//*ppNew =  new T[*pc];
        *ppNew = (T *) malloc (cbAr);
		if (!*ppNew) return FALSE;

		memcpy(*ppNew, &ar[0], cbAr);
	}
	return TRUE;
};

// copy from regular array to Array:
template<class T>
BOOL DupArray (Array<T>& arNew, ULONG cSize, T  *rg)
{
	if (!arNew.setSize (cSize))
		return FALSE;
	if (cSize > 0)
	{
		CB cbAr = cSize * sizeof (T);
		memcpy (&arNew[0], rg, cbAr);
	}
	return TRUE;
};

extern "C"
{
	inline void ncfailAssertion(SZ_CONST szFile, int line)
	{
		_ASSERT (FALSE);
	}
}

/*
BOOL DupArray(ULONG **ppNew, ULONG *pc, Array<ULONG>& ar)
{
	*pc = ar.size();
	
	if (!*pc) {if (ppNew) *ppNew = NULL; return TRUE;	}
	
	CB cbAr = ar.size()*sizeof(ULONG);

	*ppNew = (ULONG*)malloc(cbAr);
	if (!*ppNew) return FALSE;

	memcpy(*ppNew, &ar[0], cbAr);
	return TRUE;
}
*/
#endif
