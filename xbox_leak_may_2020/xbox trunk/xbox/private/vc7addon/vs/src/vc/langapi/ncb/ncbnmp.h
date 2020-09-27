// NCBNMP.H
//
// NCB package local namemap declaration

#ifndef __NCBNMP_H__
#define __NCBNMP_H__

#include <vcbudefs.h>
#include <bsc.h>

PdbInterface NcbNameMap {
	// Create a new NcbNameMap
	static BOOL Create(BOOL fAllowDuplicates, OUT NcbNameMap **ppncbnmp);

	// Release an NcbNameMap
	virtual BOOL Close() pure;

	// Add a new name along with its attribute and type to the namemap
	virtual BOOL AddItem(const char *sz, TYP typ, ATR32 atr, IINST iinst) pure;

	// Retrieve array of all NIs sorted alphabetically
	virtual BOOL GetSortedNIs(OUT const NI **ppni, OUT ULONG *pcni) pure;

	// Retrieve attribute and type for an item, given its index in the
	// sorted NI array
	virtual BOOL GetPropByIndex(ULONG idx, OUT TYP *ptyp, OUT ATR32 *patr, OUT IINST *piinst) pure;

	// Retrieve an array of iinsts for a given string given a sorted NI array
	// It is the responsibility of the client to release the arrays
	virtual BOOL GetIinstsFrSz(SZ szName, MBF mbfType, OUT IINST **ppiinst, OUT ULONG *pciinst) pure;

	// Get the string corresponding to an NI from the namemap
	virtual SZ szFrNi(NI ni) pure;
};

#endif	// __NCBNMP_H__
