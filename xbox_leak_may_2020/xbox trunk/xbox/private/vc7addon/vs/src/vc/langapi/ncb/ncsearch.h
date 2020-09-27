// NCSEARCH.H
//
// This keeps a collection of sorted NI Arrays and aids in searching
// and retrieving an appropriate index
//

#ifndef __NCSEARCH_H__
#define __NCSEARCH_H__

class Ncb; // forward declaration

struct NIArrayInfo {
    const NI    *pNIArray;
    ULONG       ulSize; // size of the array
    ULONG       ulIndex;    // index into the array
    Ncb         *pNcb;
    NcbNameMap  *pnmp;
    USHORT      store;
};


class NISearchArrays {
public:
    NISearchArrays();
    ~NISearchArrays();
    // This is for keeping a collection of NI search arrays
    ULONG       InitializeNIArrays();
    void        ReleaseNIArrays();
    BOOL        GetInfoOnIndexIntoNIArrays 
                    (ULONG uIndex, OUT SZ *psz, OUT TYP *ptyp, OUT ATR32 *patr, OUT IINST *piinst);
    BOOL        GetBestMatch(SZ_CONST szSoFar, long iLength, long *piIndex, SZ *pszUnambiguousMatch);
    ULONG       m_cNIArrays;
    NIArrayInfo **m_ppNIArrayInfo;
    BOOL        m_bGlobals;

private:
    int     m_iAnchorNI; // this is the NI array info that has the correct rank
    ULONG   m_ulAnchorRank; // This is the rank of the m_ppNIArrayInfo[m_iAnchorNI]->ulIndex in the merged list
    ULONG   m_ulTotal;  // Total count of the number of items in all the NI Search arrays
    BOOL    BinSearch(NIArrayInfo *ptr, SZ_CONST strName, ULONG num, OUT ULONG *puIndex);
    void    ForwardSequentialSearch(ULONG uIndex);
    void    ReverseSequentialSearch(ULONG uIndex);
    void    GetIinstInfoAtAnchor(OUT SZ *psz, OUT TYP *ptyp, OUT ATR32 *patr, OUT IINST *piinst);
};


#endif // !__NCSEARCH_H__
