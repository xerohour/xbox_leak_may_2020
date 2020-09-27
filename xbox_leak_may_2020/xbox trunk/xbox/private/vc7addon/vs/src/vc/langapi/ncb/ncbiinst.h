// NCBIINST.H
//
// Define map from NIs to defining IINSTs within a single NCB.
// Used to greatly speed up Ncb::getOverloadArray
//

#ifndef __NCBIINST_H__
#define __NCBIINST_H__

#include <new.h>
#include <pdb.h>
#include <vcbudefs.h>
#include <bsc.h>
#include <map_t.h>

//
// IInstArray - Array of IINSTs suitable as the range of a Map<>.
// If count of IINSTs is 2 or more, this points to a heap-allocated list of
// IINSTs, otherwise the single IINST is local to this object.  Reduces memory
// requirements for the common case of a single IINST, while still maintaining
// a fixed-length object as needed by Map<>.
//

class IInstArray {
public:
    IInstArray(IINST iinst = iinstNil);
    IInstArray(const IInstArray &that);
    IInstArray(PB *ppb);
    ~IInstArray();
    IInstArray &operator =(const IInstArray &that);
    bool	operator ==(const IInstArray &that) const;
	DWORD	Count() const;
    BOOL	Find(IINST iinst) const;
    BOOL	Add(IINST iinst);
    BOOL	Remove(IINST iinst);
    DWORD	RemoveModule(IMOD imod);
    BOOL	Save(Buffer *pbuf) const;
	void	Replace(IINST *piinst);
    void	GetIInstArray(Array<IINST> *prgiinst) const;
private:
    DWORD m_cIInst;         // Count of IINSTs in this array
    union {
        IINST m_iinst;      // m_cIInst == 1 - the single IINST
        IINST *m_prgIInst;  // m_cIInst != 1 - pointer to the array of IINSTs
    };
};

inline
IInstArray::IInstArray(IINST iinst) : m_cIInst(1), m_iinst(iinst)
{
}

inline
IInstArray::~IInstArray()
{
    if (m_cIInst > 1) {
        delete [] m_prgIInst;
    }
}

inline DWORD
IInstArray::Count() const
{
	return m_cIInst;
}

//
// MapNiToIInstArray - wrapper for the Map<NI,IInstArray> used to map an NI to
// a list of IINSTs.  Used by the Ncb implementation to map NIs to all the
// IINSTs which define them.
//
// The NIs present in the map are also kept in an array, sorted alphabetically
// by the strings each represents.  This is the order in which the NIs are
// serialized to the NCB stream.  This sorted array can be queried to find all
// defined names within the NCB which start with a given prefix (used by
// identifier completion).
//

class MapNiToIInstArray {
public:
	// Initialize the object
    void Reset(NameMap *pnm);

	// Move from in memory <-> NCB stream
    BOOL Reload(PB *ppb);
    BOOL Save(Buffer *pbuf);

	// Add new mappings
    BOOL Add(NI ni, IINST iinst);

	// Remove mappings
    BOOL Remove(NI ni, IINST iinst);
    BOOL RemoveModule(IMOD imod);

	// Retrieve all mappings for a single NI
    BOOL GetIInstArray(NI ni, Array<IINST> *prgiinst) const;

	// Retrieve the sorted list of global NIs
	BOOL GetSortedGlobalNIs(const NI **ppni, ULONG *pcni) const;

private:
	// Update sorted NI array for RemoveModule
	void RemoveModuleHelper(Array<NI> &rgniRemoved);

	// Check if name is one we want to save in sorted NI list
	bool IsGlobalName(NI ni);

	// Do binary search for an NI in the sorted array
	bool SortedSearch(NI ni, unsigned *pidx) const;

	// Update the sorted NI array with recently added NIs
	void UpdateSortedNIs() const;

#ifdef _DEBUG
	// Check the validity of this object
	bool Validate() const;
#endif

private:
    Map<NI,IInstArray,LHcNi> m_map;
	NameMap *				m_pnm;
	mutable Array<NI>		m_rgniSorted;
	mutable Array<NI>		m_rgniAdded;
};

// Helper function to compare two strings derived from NIs.  The comparison is case-insensitive
// with a fall-back to case-sensitive if the first compare shows a match.
inline int StrCmpNIs(SZ_CONST sz1, SZ_CONST sz2)
{
	int iRes = _stricmp(sz1, sz2);
	if (iRes == 0) {
		iRes = strcmp(sz1, sz2);
	}
	return iRes;
}

#endif // !__NCBIINST_H__
