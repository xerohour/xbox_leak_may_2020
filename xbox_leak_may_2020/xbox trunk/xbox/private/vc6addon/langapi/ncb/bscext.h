// bscext.h
//	Extension for the bsc interface
#ifndef __BSCEXT_H__
#define __BSCEXT_H__

#include <bsc.h>

#if CC_MULTIPLE_STORES
#undef IINST_GLOBALS
#undef isTheGlobalIinst

#define IINST_GLOBALS   0x0FFF0001
#define isTheGlobalIinst(iinst)    (((iinst) & 0x0FFFFFFF) == IINST_GLOBALS)

// store type
enum STORE_TYP {
	ST_WIN32, ST_MFC, ST_CRT, ST_CXXRT, 
	// ST_USER must be the last one
	ST_USER
};
#else
#pragma message("Not Using Multiple Stores ...")
#endif	// CC_MULTIPLE_STORES

PdbInterface NcbNameMap;

// NCB language attributes:
#define NCB_LANGUAGE_ALL		0xff
#define NCB_LANGUAGE_CPP		0x01
#define NCB_LANGUAGE_JAVA		0x02
#define NCB_LANGUAGE_ODL		0x04
#define NCB_LANGUAGE_FORTRAN	0x08
#define NCB_LANGUAGE_HTML		0x10

PdbInterface BscEx : public Bsc
{
	virtual BOOL irefEndInfo(IREF iref, OUT SZ *pszModule, OUT LINE *piline) pure;
	virtual BOOL idefEndInfo(IDEF idef, OUT SZ *pszModule, OUT LINE *piline) pure;
	virtual BOOL getGlobalsFrImod (IMOD imod, MBF mbf, OUT IINST **ppiinst, OUT ULONG *pciinst) pure;
	virtual void setLanguage (BYTE bLanguage) pure;
	virtual void getLanguage (BYTE * pbLanguage) pure;
	virtual BOOL isModInLang (BYTE bLanguage, IMOD imod) pure;

	// IDL interfaces
	virtual BOOL getIDLAttrib (IINST iinst, OUT IINST **ppiiAttr, OUT ULONG *pciinst) pure;
	virtual BOOL getIDLAttribVal (IINST iiAttr, OUT SZ *pszValue) pure;
	virtual BOOL isIDLAttrib (IINST iinst, SZ szAttrib, OUT IINST *piiAttr, OUT SZ *pszValue) pure;
	virtual BOOL filterInTypeArray (IINST * piinst, ULONG ciinst, TYP type, 
									OUT IINST ** ppiinstOut, OUT ULONG * pciinstOut) pure;
	virtual BOOL filterOutTypeArray (IINST * piinst, ULONG ciinst, TYP type, 
									OUT IINST ** ppiinstOut, OUT ULONG * pciinstOut) pure;
	virtual BOOL getIDLMFCComment (IINST iinst, OUT IINST **ppiiComment, OUT ULONG * pciinst) pure;
	virtual BOOL getIDLMFCCommentClass (IINST iiComment, OUT SZ * pszClass) pure;
	virtual BOOL isIDLMFCComment (IINST iinst, SZ szType, OUT IINST * piiComment, OUT SZ * pszValue) pure;
	// back to general interface

	// get the line number for the declaration
	virtual BOOL ideclInfo(IINST iinst, OUT SZ *pszModule, OUT LINE *piline) pure;
	virtual BOOL getMapIinst (IINST iiClass, SZ szMapType, OUT IINST **ppIinst, OUT ULONG * pciinst) pure;
	virtual BOOL getAllArray (MBF mbf, OUT IINST ** ppiinst, OUT ULONG * pciinst) pure;
	virtual BOOL isLangInProject (BYTE bLanguage) pure;

#if CC_MULTIPLE_STORES
	// store specific API
	virtual BOOL isExistingStore(SZ szName, OUT STORE_TYP* pst, OUT USHORT* psi) pure;
	virtual SZ   szFrStoreTyp(STORE_TYP st) pure; 
	virtual BOOL getAllStoresArray(OUT NI** ppni, OUT USHORT* pcStores) pure;
#endif	// CC_MULTIPLE_STORES

	// get type with name inserted in proper position
	virtual SZ   getTypeWithName(IINST iinst, SZ szName) pure;

	// Support for searching global array of NIs
	virtual ULONG InitializeNIArrays(NcbNameMap *pncbnmpCurClass, NcbNameMap *pncbnmpCurFunc, BOOL fAddPrebuiltStore) pure;
	virtual void ReleaseNIArrays() pure;
	virtual	BOOL GetInfoOnIndexIntoNIArrays (ULONG uIndex, OUT SZ *psz, OUT TYP *ptyp, OUT ATR *patr, OUT IINST *piinst) pure;
	virtual BOOL GetBestMatch(const SZ szSoFar, long iLength, long *piIndex, SZ *pszUnambiguousMatch) pure;
	virtual BOOL GetEmptyNcbNameMap(BOOL fAllowDuplicates, OUT NcbNameMap ** ppncbnmp) pure;
	virtual BOOL GetClassMembersNameMap(IINST iinst, SZ szTemplateArgs, BOOL fAllowDuplicates, OUT NcbNameMap ** ppncbnmp) pure;
	virtual BOOL GetNameSpaceNameMap(SZ szNS, OUT NcbNameMap ** ppncbnmp) pure;
	
	// compare two IINST if they have the same signature
	virtual BOOL compareIinst (IINST iinst1, IINST iinst2, DWORD * pdwFlags) pure;
};

#endif __BSCEXT_H__
