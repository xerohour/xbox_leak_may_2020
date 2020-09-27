// NCBROWSE.H
//
// implementation of no compile browser API for one source information
//
// REVIEWS:
// 1) need some locking mechanism for thread safe operation?
// 2) need some other interface for writing info from the parser thread?
// 

#ifndef __NCBROWSE_H__
#define __NCBROWSE_H__

// No compile browser will have the same interface as Bsc
#include "ncarray.h"
#include "ncparex.h"
#include "ncbdef.h"
#include "ncbmod.h"
#include "ncbiinst.h"
#include "ncsearch.h"
#include "simparray.h"
#include "nmt_t.h"
#include <tchar.h>

// Ncb = No Compile Browser
// derived from Bsc to provide common interface

// module content buffer size
// size of array defined to store module content in memory
// this is the number of modules we store in memory.
#define NCB_MOD_BUF_SIZE		12

// different flags for module status
// module content can be loaded for read or write
// or unloaded. (default is unloaded)
#define NCB_STATMOD_EMPTY		0x00
#define NCB_STATMOD_UNLOADED	0x01
#define NCB_STATMOD_LOAD_READ	0x02
#define NCB_STATMOD_LOAD_WRITE	0x04
#define NCB_STATMOD_DIRTY		0x08  // set to dirty when file content is modified
									  // so we have to write it out to the stream
									  // when we save.

// Currently supported maximum number of stores. Here's how the number came up ...
//
// We want the IINST to identify a store for the sake of speed. The IINST is
// made up of Imod (hi 16-bits) and IProp (low 16-bits). Currently the PDB format
// allows no more than 4K streams ... we use a stream per module. This means
// that we have never supported more than 4K modules. So we can use the top 4
// bits of the Imod to have a store index. The value 0x0 is reserved for the main
// store as this will be the most in use. The value 0xF is unavailable because
// we have previously used reserved values like 0xFFFFFFFE to designate
// IINST_UNDEFINED and 0xFFFF0001 to designate IINST_GLOBALS. Thus, it is safe to
// have as many as 15 stores open at the same time (including the main store).
// [We could in theory use a table to keep track of things and allow more than 4K
// mods per store ... this simply makes the code pretty complex. The above method
// is faster and less invasive]
// 
// To allow for future expansion and sharing of pre-built stores across projects
// we will do another optimization. The store index will not be saved in the physical
// store. We keep track of the store index and the IINST actually returned to users
// of the store will have the store index encoded in. -Sundeep-
#define NCB_MAXSTORES			15			// used commonly in FOR_ALL_STORES

class Ncb;

// string class for use in m_mapFilenameImod
class NcbString : public SimpleString
{
public:
	NcbString( unsigned len = 256 ) : SimpleString(len) {}
	// shall we move these to SimpleString itself?
	NcbString( const char *sz ) 
	{
		Set( sz, strlen(sz) + 1 );
	}
	NcbString& operator=( NcbString& str ) {
		Set( str.Base(), strlen( str.Base() ) );
		return *this;
	}
	void MakeLower()  { _tcslwr(Base()); }
};

class CNcbStringHasher
{
public:
	inline LHASH __fastcall operator()(NcbString &str)
	{
		return hashSz(str.Base());
	}
};

///////////////////////////////////////////////////////////
// needed to keep track the list of no compile browser object
///////////////////////////////////////////////////////////
struct NcbInfo
{
	Ncb *	m_pNcb;					// pointer to the real Ncb
	char 	m_szName[_MAX_PATH];	// filename
	PDB	*	m_pPdb;					// pointer to the pdb file
	int		m_count;				// the number of reference
	BOOL	m_bIOwnPdb;				// if I own the pdb
	Ncb *	m_pParentNcb;			// if opened by openStore()
	USHORT  m_storeIndex;			// store index in parent
};

////////////////////////////////////////////////
// data structures for notifications
////////////////////////////////////////////////

struct NcbNotifyQ
{
	HTARGET	m_hTarget;
	BYTE	m_bLanguage;
	BOOL	m_bDel;
	Array<NiQ> m_rgQ;
};

typedef enum { initMismatchVer = -1, initError, initOK} InitRetVal;

class Ncb: public BscEx, public NcbParseEx
{
private:
	static Array<NcbInfo>	*ms_prgNcbInfo;
	static BOOL OpenThePdb(SZ szName, PDB** ppPdb, BOOL bWrite);
public:
	static BOOL OpenNcb (SZ szName, BOOL bWrite, Ncb ** ppNcb);
	static BOOL OpenNcb (PDB * ppdb, Ncb ** ppNcb);
	static void DeleteNcbInfo()
	{
		//delete Ncb::ms_prgNcbInfo;
	}
	static NameMap *	m_pnmStatic; // Name map, our hash table
									 // used by CmpStrFrIProp
									 // as of a global for qsort()

public:
//////////////////////////////////////////////////////////////////////////////
// BEGIN of Bsc interface
	// open by name or by .pdb 
	virtual BOOL close();
	// primitives for getting the information that underlies a handle
	virtual BOOL iinstInfo(HTARGET hTarget, BYTE bLanguage, IINST iinst, OUT SZ *psz, OUT TYP *ptyp, OUT ATR32 *patr);
	virtual BOOL iinstInfo(IINST iinst, OUT SZ *psz, OUT TYP *ptyp, OUT ATR *patr); // no longer supported
	virtual BOOL iinstInfo2(IINST iinst, OUT SZ *psz, OUT TYP *ptyp, OUT ATR32 *patr);
	virtual BOOL irefInfo(IREF iref, OUT SZ *pszModule, OUT LINE *piline);
	virtual BOOL idefInfo(HTARGET hTarget, BYTE bLanguage, IDEF idef, OUT SZ *pszModule, OUT LINE *piline);
	virtual BOOL idefInfo(IDEF idef, OUT SZ *pszModule, OUT LINE *piline);
	virtual BOOL imodInfo(IMOD imod, OUT SZ *pszModule);
	virtual SZ   szFrTyp(TYP typ);
	virtual SZ   szFrAtr(ATR atr);								// no longer supported
	virtual SZ   szFrAtr2(ATR32 atr);


	// primitives for managing object instances (iinst)
	virtual BOOL getIinstByvalue(HTARGET hTarget, BYTE bLanguage, SZ sz, TYP typ, ATR32 atr, OUT IINST *piinst);	
	virtual BOOL getIinstByvalue(SZ sz, TYP typ, ATR atr, OUT IINST *piinst);	// no longer supported
	virtual BOOL getIinstByvalue2(SZ sz, TYP typ, ATR32 atr, OUT IINST *piinst);
	virtual BOOL getIinstByvalue3(SZ sz, TYP typ, ATR32 atr, OUT IINST** ppinst, OUT ULONG* pciinst);
	virtual BOOL getIinstByvalue3(HTARGET hTarget, BYTE bLanguage, SZ sz, TYP typ, ATR32 atr, OUT IINST** ppinst, OUT ULONG* pcinst);
	virtual BOOL getOverloadArray(HTARGET hTarget, BYTE bLanguage, SZ sz, MBF mbf, OUT IINST **ppiinst, OUT ULONG *pciinst);
	virtual BOOL getOverloadArray(SZ sz, MBF mbf, OUT IINST **ppiinst, OUT ULONG *pciinst);
	virtual BOOL getUsesArray(IINST iinst, MBF mbf, OUT IINST **ppiinst, OUT ULONG *pciinst);
	virtual BOOL getUsedByArray(IINST iinst, MBF mbf, OUT IINST **ppiinst, OUT ULONG *pciinst);
	virtual BOOL getBaseArray(HTARGET hTarget, BYTE bLanguage, IINST iinst, OUT IINST **ppiinst, OUT ULONG *pciinst);
	virtual BOOL getBaseArray(IINST iinst, OUT IINST **ppiinst, OUT ULONG *pciinst);
	virtual BOOL getDervArray(HTARGET hTarget, BYTE bLanguage, IINST iinst, OUT IINST **ppiinst, OUT ULONG *pciinst);
	virtual BOOL getDervArray(IINST iinst, OUT IINST **ppiinst, OUT ULONG *pciinst);
	virtual BOOL getMembersArray(HTARGET hTarget, BYTE bLanguage, IINST iinst, MBF mbf, OUT IINST **ppiinst, OUT ULONG *pciinst);
	virtual BOOL getMembersArray(IINST iinst, MBF mbf, OUT IINST **ppiinst, OUT ULONG *pciinst);

	// primitives for getting definition and reference information
	virtual BOOL getDefArray(HTARGET hTarget, BYTE bLanguage, IINST iinst, OUT IDEF **ppidef, OUT ULONG *pciidef);
	virtual BOOL getDefArray(IINST iinst, OUT IDEF **ppidef, OUT ULONG *pciidef);
	virtual BOOL getRefArray(IINST iinst, OUT IREF **ppiref, OUT ULONG *pciiref);

	// primitives for managing source module contents
	virtual BOOL getModuleContents(IMOD imod, MBF mbf, OUT IINST **ppiinst, OUT ULONG *pciinst);
	virtual BOOL getModuleContents(HTARGET hTarget, BYTE bLanguage, IMOD imod, MBF mbf, BOOL bGlobalOnly, OUT IINST **ppiinst, OUT ULONG *pciinst);
	virtual BOOL getModuleByName(SZ sz, OUT IMOD *pimod);
	virtual BOOL getAllModulesArray(HTARGET hTarget, BYTE bLanguage, OUT IMOD **ppimod, OUT ULONG *pcimod);
	virtual BOOL getAllModulesArray(OUT IMOD **ppimod, OUT ULONG *pcimod);
	
	// call this when a computed array is no longer required
	virtual void disposeArray(void *pAnyArray);
	
	// call this to get a pretty form of a decorated name	
	virtual SZ  formatDname(SZ szDecor);

	// call this to do category testing on instances
	virtual BOOL fInstFilter(IINST iinst, MBF mbf);

	// primitives for converting index types
	virtual IINST iinstFrIref(IREF);
	virtual IINST iinstFrIdef(IDEF);
	virtual IINST iinstContextIref(IREF);

	// general size information
	virtual BOOL getStatistics(BSC_STAT *);
	virtual BOOL getModuleStatistics(IMOD, BSC_STAT *);

	// needed for no compile browser
	virtual SZ     getParams (IINST iinst);
	virtual USHORT getNumParam (IINST iinst);
	virtual SZ     getParam (IINST iinst, USHORT index);

	// get return type/variable type
	virtual SZ  getType (IINST iinst);
	virtual SZ	getTypeWithName(IINST iinst, SZ szName);
        virtual SZ  getMacroDefinition(IINST iinst);

	// get global information
	virtual BOOL fCaseSensitive ();
	virtual BOOL setCaseSensitivity (BOOL);
	virtual BOOL getAllGlobalsArray (MBF mbf, OUT IINST ** ppiinst, OUT ULONG * pciinst);
	virtual BOOL getAllGlobalsArray (HTARGET hTarget, BYTE bLanguage, MBF mbf, OUT IINST ** ppiinst, OUT ULONG * pciinst);
	virtual BOOL getAllGlobalsArray (MBF mbf, OUT IinstInfo ** ppiinstinfo, OUT ULONG * pciinst);
	virtual BOOL getAllGlobalsArray (HTARGET hTarget, BYTE bLanguage, MBF mbf, OUT IinstInfo ** ppiinstinfo, OUT ULONG * pciinst);
	virtual BOOL regNotify (pfnNotifyChange pNotify);
	virtual BOOL regNotify (HTARGET hTarget, pfnNotifyChange pNotify);
	// register to make sure that NCB will create change queue
	virtual BOOL regNotify ();
	virtual BOOL regNotify (HTARGET hTarget, OUT ULONG * pindex);
	virtual BOOL getQ (OUT NiQ ** ppQ, OUT ULONG * pcQ);
	virtual BOOL getQ (ULONG index, HTARGET hTarget, OUT NiQ ** ppQ, OUT ULONG * pcQ);
	virtual BOOL checkParams (IINST iinst, SZ * pszParam, ULONG cParam);
	virtual BOOL fHasMembers (IINST iinst, MBF mbf);
	virtual BOOL fHasMembers (HTARGET hTarget, BYTE bLanguage, IINST iinst, MBF mbf);
	// needed for class view for optimization
	virtual SZ szFrNi (NI ni);
	virtual BOOL niFrIinst (IINST iinst, NI *ni);
// END of Bsc interface
// BEGIN of BscEx interface
	virtual BOOL irefEndInfo(IREF iref, OUT SZ *pszModule, OUT LINE *piline);
	virtual BOOL idefEndInfo(IDEF idef, OUT SZ *pszModule, OUT LINE *piline);
	virtual BOOL idefEndInfo(HTARGET hTarget, BYTE bLanguage, IDEF idef, OUT SZ *pszModule, OUT LINE *piline);
	virtual BOOL getGlobalsFrImod (IMOD imod, MBF mbf, OUT IINST **ppiinst, OUT ULONG *pciinst);
	virtual BOOL getGlobalsFrImod (HTARGET hTarget, BYTE bLanguage, IMOD imod, MBF mbf, OUT IINST **ppiinst, OUT ULONG *pciinst);
	virtual void setLanguage (BYTE bLanguage) {}; // default to nothing (used only by ncwrap)
	virtual void getLanguage (BYTE * pbLanguage){};
	virtual BOOL isModInLang (BYTE bLanguage, IMOD imod);
	virtual BOOL getIDLAttrib (IINST iinst, OUT IINST **ppiiAttr, OUT ULONG *pciinst);
	virtual BOOL getIDLAttribVal (IINST iiAttr, OUT SZ *pszValue);
	virtual BOOL isIDLAttrib (IINST iinst, SZ szAttrib, OUT IINST *piiAttr, OUT SZ *pszValue);
	virtual BOOL filterInTypeArray (IINST * piinst, ULONG ciinst, TYP type, 
									OUT IINST ** ppiinstOut, OUT ULONG * pciinstOut) ;
	virtual BOOL filterOutTypeArray (IINST * piinst, ULONG ciinst, TYP type, 
									OUT IINST ** ppiinstOut, OUT ULONG * pciinstOut) ;
	// get the line number for the declaration
	virtual BOOL ideclInfo(IINST iinst, OUT SZ *pszModule, OUT LINE *piline);
	virtual BOOL ideclInfo(IINST iinst, OUT SZ *pszModule, OUT LINE *piline, OUT LINE *piEndline);
	virtual BOOL ideclInfo(HTARGET hTarget, BYTE bLanguage, IINST iinst, OUT SZ *pszModule, OUT LINE *piline, OUT LINE *piEndline = NULL);

	virtual BOOL getIDLMFCComment (IINST iinst, OUT IINST **ppiiComment, OUT ULONG * pciinst) ;
	virtual BOOL getIDLMFCCommentClass (IINST iiComment, OUT SZ * pszClass) ;
	virtual BOOL isIDLMFCComment (IINST iinst, SZ szType, OUT IINST * piiComment, OUT SZ * pszValue) ;
	virtual BOOL getMapIinst (IINST iiClass, SZ szMapType, OUT IINST **ppIinst, OUT ULONG *pciinst);
	virtual BOOL getMapIinst (HTARGET hTarget, BYTE bLanguage, IINST iiClass, SZ szMapType, OUT IINST **ppIinst, OUT ULONG *pciinst);
	virtual BOOL getAllArray (MBF mbf, OUT IINST ** ppiinst, OUT ULONG * pciinst);
	virtual BOOL getAllArray (HTARGET hTarget, BYTE bLanguage, MBF mbf, OUT IINST ** ppiinst, OUT ULONG * pciinst);
	virtual BOOL isLangInProject (BYTE bLanguage);
	virtual BOOL isLangInProject (HTARGET hTarget, BYTE bLanguage);
#if CC_MULTIPLE_STORES
	// store related
	virtual BOOL isExistingStore(SZ szName, OUT STORE_TYP* pst, OUT USHORT* psi);
	virtual SZ   szFrStoreTyp(STORE_TYP st);
	virtual BOOL getAllStoresArray(OUT NI** ppni, OUT USHORT* pcStores);
	virtual SZ   szFrStoreIndex(USHORT idx);
#endif	// CC_MULTIPLE_STORES
	virtual BOOL compareIinst (IINST iinst1, IINST iinst2, DWORD * pdwFlags);
	virtual BOOL compareIinst (HTARGET hTarget, IINST iinst1, IINST iinst2, DWORD * pdwFlags);

	// get attributes ...
	virtual BOOL getAttributeArray(IINST iinst, OUT IINST ** ppiinst, OUT ULONG * pciinst);
	virtual BOOL getAttributeArray(HTARGET hTarget, BYTE bLanguage, IINST iinst, OUT IINST **ppiinst, OUT ULONG *pciinst);
	virtual BOOL getGlobalAttributeArray(OUT IINST ** ppiinst, OUT ULONG * pciinst);
	virtual BOOL getGlobalAttributeArray(HTARGET hTarget, BYTE bLanguage, OUT IINST ** ppiinst, OUT ULONG * pciinst);

	virtual BOOL getParentIinst(IINST iinstChild, IINST *piinstParent);
	virtual BOOL isIinstInProject(IINST iinst);
	virtual BOOL isIinstInProject(HTARGET hTarget, IINST iinst);
	virtual BOOL isUserIinst(IINST iinst);

	// returns an mbf with all bits set for which TypFilter(typ, mbfXXX) == true
	virtual MBF TypToMBF(TYP typ);

	// returns whether the given IINST is in the given language
	virtual BOOL isIinstInLang(BYTE bLanguage, IINST iinst);

	// get the project a particular IINST is part of
	virtual BOOL getProjectForIinst(IINST iinst, OUT HTARGET* phTarget);
	virtual USHORT IModFrIinst(IINST iinst);

	//get the list of ref info for this string
	virtual BOOL getRefInfo(SZ szName, IINST **ppiinst, ULONG *pciinst);
	virtual BOOL getNIsWithPattern(SZ szPattern, SZ **ppNIs, ULONG *pcNIs);
	virtual IINST getModuleIinst(IMOD imod);
	
	// get the name for this iinst including any parent namespaces
	virtual BOOL iinstNameWithNamespace(IINST iinst, SZ szOut, ULONG_PTR cchOut, ULONG_PTR *pcchNeeded);

	virtual BOOL getDeclForDefn(SZ sz, TYP typ, ATR32 atr, IINST defnIinst, OUT IINST *piinst);
	virtual BOOL getOverloadArrayWithWildcard(SZ sz, MBF mbf, BSCEX_SEARCHOPT eSrchOpts, OUT IINST **ppiinst, OUT ULONG *pciinst);
	
	virtual SZ getAttributeParam(IINST iinst, USHORT ind, BOOL *pfisDefault);
	// END of BscEx interface

//////////////////////////////////////////////////////////////////////////////
// BEGIN of NcbParse interface
	// open by name or by .pdb
	// INTERFACE FOR MODULE LEVEL
	virtual BOOL openMod (SZ szMod, BOOL bCreate, OUT IMOD * pimod);
	virtual BOOL closeMod (IMOD imod, BOOL bSave);
	virtual BOOL clearModContent (IMOD imod);
	virtual BOOL setModTime (IMOD imod, time_t tStamp);
	virtual BOOL getModTime (IMOD imod, time_t *ptStamp);
	virtual BOOL calculateCRC (IMOD imod, USHORT mask, SIG *pSig);

	// set module attributes
	virtual BOOL setModAtr (IMOD imod, BYTE bAtr);
	virtual BOOL getModAtr (IMOD imod, BYTE * pbAtr);

	// check if module is member of a specific target
	virtual BOOL isModInTarget (HTARGET hTarget, IMOD imod);
	virtual BOOL setModAsSrc (HTARGET hTarget, IMOD imod, BOOL bProjSrc);
	// primitives for adding a target to a module
	virtual BOOL addModToTarget (HTARGET hTarget, IMOD imod, BOOL bProjSrc);

	virtual BOOL isModTargetSource (HTARGET hTarget, IMOD imod);
	// primitives for adding an include file
	virtual BOOL addInclToMod (IMOD inclimod, HTARGET hTarget, IMOD imod); 
	virtual BOOL isInclInMod (IMOD inclimod, HTARGET hTarget, IMOD imod);
	// primitives for deleting an include file
	virtual BOOL delInclFrMod (IMOD inclimod, HTARGET hTarget, IMOD imod);

	// primitives for deleting all include files
	virtual BOOL delAllInclFrMod (HTARGET hTarget, IMOD imod);

	// primitives for deleting target from the database
	virtual BOOL delTarget (HTARGET hTarget);
	// primitives for adding a target to the database
	virtual BOOL addTarget (HTARGET hTarget);
	// primitives for deleting file from target
	virtual BOOL delModFrTarget (IMOD imod, HTARGET hTarget);

	virtual BOOL mapTargetToSz (HTARGET hTarget, SZ szTarget);
	virtual BOOL mapSzToTarget (SZ szTarget, HTARGET hTarget);

	virtual BOOL getAllInclMod (HTARGET hTarget, IMOD imod, OUT IMOD ** ppimod, OUT ULONG * pcmod);
	virtual BOOL getAllTarget (IMOD imod, OUT HTARGET ** ppTarget, OUT ULONG * pcTarget);
	virtual BOOL getAllFlattenDeps (HTARGET hTarget, IMOD imod, OUT IMOD ** ppimod, OUT ULONG * pcmod, BOOL &bNotifyBuild);

	// INTERFACE FOR OBJECT LEVEL
		// primitives for adding an info
		// IINST is used for containment
	virtual BOOL addProp (SZ szName, TYP typ, ATR32 atr, IMOD imod, OUT IINST * pinst);
	virtual BOOL setKind (IINST iinst, IINST iinstP, BYTE kind);
	virtual BOOL setLine (IINST iinst, LINE lnStart);
	virtual BOOL setDefn (IINST iinst);
	virtual BOOL delProp (IINST iinst);
	// For function, the 1st param is always return type followed by real params.
	// For variable, the 1st param is always type.
	virtual BOOL addParam (IINST iinst, SZ szName);
	// Locking mechanism:
	virtual BOOL lock();
	virtual BOOL unlock();
	virtual BOOL notify(); // flush out notification queue!!
	virtual BOOL suspendNotify ();
	virtual BOOL resumeNotify();
	virtual void graphBuilt();
	virtual BOOL delUnreachable(HTARGET hTarget); 
	virtual BOOL isInit (HTARGET hTarget, IMOD imod);
	virtual BOOL setInit (HTARGET hTarget, IMOD imod, BOOL bInit);
	virtual BOOL notifyImod (OPERATION op, IMOD imod, HTARGET hTarget);
	virtual BOOL notifyIinst (NiQ qItem, HTARGET hTarget, BYTE bLanguage);
	virtual BOOL getBsc (HTARGET hTarget, SZ szName, Bsc ** ppBsc);
	virtual BOOL delUninitTarget ();
    virtual BOOL imodFrSz (SZ szName, OUT IMOD *pimod);
	virtual BOOL getGlobalsArray (MBF mbf, IMOD imod, OUT IinstInfo ** ppiinstinfo, OUT ULONG * pciinst);

	virtual BOOL targetFiles (HTARGET hTarget, BOOL bSrcProjOnly, OUT IMOD ** ppimod, OUT ULONG * pcimod);
	virtual BOOL setAllInit (HTARGET hTarget, BOOL bInit);
	virtual void setNotifyBuild (IMOD imod, BOOL bNotifyBuild);
	virtual BOOL isNotifyBuild (IMOD imod);
	virtual BOOL notifyArrIinst (NiQ * pArrQ, ULONG uSize, HTARGET hTarget, BYTE bLanguage);
// END of NcbParse Interface
// BEGIN of NcbParseEx Interface
	// REVIEW: should be merged as ParseNcb Interface after we have 5.0 langapi project
	virtual BOOL setEndLine (IINST iinst, LINE lnEnd);
	virtual void setModLang (IMOD imod, BYTE bLanguage);
	virtual BOOL setIDLAttrib (IINST iinst, SZ szName, SZ szValue, OUT IINST *piinst);
	virtual BOOL setIDLMFCComment (IINST iinst, SZ szType, SZ szValue, OUT IINST *piinst);

	// This is for keeping a collection of NI search arrays
	virtual ULONG InitializeNIArrays(NcbNameMap *pncbnmpCurClass, NcbNameMap *pncbnmpCurFunc, BOOL fAddPrebuiltStore);
	virtual void ReleaseNIArrays();
	virtual BOOL GetInfoOnIndexIntoNIArrays (ULONG uIndex, OUT SZ *psz, OUT TYP *ptyp, OUT ATR32 *patr, OUT IINST *piinst);
	virtual BOOL GetBestMatch(SZ_CONST szSoFar, long iLength, long *piIndex, SZ *pszUnambiguousMatch);
	virtual BOOL GetEmptyNcbNameMap(BOOL fAllowDuplicates, OUT NcbNameMap ** ppncbnmp);
	virtual BOOL GetClassMembersNameMap(IINST iinst, SZ szTemplateArgs, BOOL fAllowDuplicates, BOOL fOnlyBases, OUT NcbNameMap ** ppncbnmp, BOOL fConstructors);
	virtual BOOL GetNameSpaceNameMap(SZ szNS, OUT NcbNameMap ** ppncbnmp);

#if CC_MULTIPLE_STORES
	// New ones added to support multiple stores
	virtual BOOL addStore(SZ szName, STORE_TYP st = ST_USER);
	virtual BOOL delStore(SZ szName);
	virtual BOOL openNextStore();
	virtual BOOL pageInNextStore();
	virtual BOOL delAllStores();
	virtual BOOL openStore(SZ szName, USHORT* pStoreIdx);
	virtual BOOL closeStore(USHORT storeIdx);
#endif // CC_MULTIPLE_STORES
	virtual BOOL beforeUpdate(IMOD imod);
	virtual BOOL afterUpdate(IMOD imod, UpdateItem **ppUpdates, ULONG *pcUpdates, BOOL *pfIncludesChanged);

	virtual BOOL addReference(SZ szName, IINST iinst);
	virtual BOOL delModuleReferences(IMOD imod);
	// delete all modules not in the project - part of NcbParseEx
    virtual BOOL delAllUnrefMod();

	
	    

// END of NcbParseEx Interface
///////////////////////////////////////////////////////////////////////////////
	// these members can be accessed by anyone who can see 
	// the class definition
	Ncb(BOOL bWrite);
	~Ncb();
	InitRetVal init (PDB * pdb, BOOL bIgnoreVersion);
	BOOL unregNotify (pfnNotifyChange pNotify);
	BOOL unregNotify (ULONG index, HTARGET hTarget);
	void setLanguage (BYTE bLanguage, ULONG index);
	BOOL getOverloadArrayWithWildcard(HTARGET hTarget, BYTE bLanguage, SZ sz, MBF mbf, BSCEX_SEARCHOPT eSrchOpts, OUT IINST **ppiinst, OUT ULONG *pciinst);
	BOOL setCaseSensitivity (HTARGET hTarget, BOOL bCase);
   

private:
	// private functions:
	BOOL LoadVersion();
	BOOL SaveVersionToStream();
	BOOL LoadTargetsToMem();
	BOOL SaveTargetsToStream();
	BOOL LoadModHdrsToMem();
	BOOL SaveModHdrsToStream();
	BOOL LoadStoreInfo();
	BOOL SaveStoreInfoToStream();
	BOOL LoadMapToMem(char *szMapName, MapNiToIInstArray *pMap);
	BOOL SaveMapToStream(char *szMapName, MapNiToIInstArray *pMap);
	BOOL CompressTarget (NCB_TARGETINFO * ti, Array<NCB_MODINFO> &rgModInfo);
	BOOL DelUnreachable (NCB_TARGETINFO * ti);
	BOOL DelUnreachable (NCB_TARGETINFO * ti, USHORT i);
	BOOL CompressModHdr();
	BOOL LoadModForRead(USHORT iModHdr, OUT USHORT * pindex);
	BOOL SaveReadModToStream(USHORT iBuf);
	BOOL LoadFrReadToWrite (USHORT iBuf, OUT USHORT * piBuf);
	BOOL LoadFrWriteToRead (USHORT iBuf, OUT USHORT * piBuf);
	BOOL FindWriteIndex (USHORT iModHdr, OUT USHORT *pindex);
	BOOL SetLine (IINST iinst, LINE ln, BOOL bStartLine);
	// conversion from Iinst to imod or iprop
	USHORT IPropFrIinst (IINST iinst);

	// Helper functions for bsc interface:
	BOOL GetIDef (HTARGET hTarget, BYTE bLanguage, IINST iinst, OUT IDEF * piDef, OUT USHORT * piBuf);
	BOOL GetIDef (HTARGET hTarget, BYTE bLanguage, IINST iinst, NI ni, TYP typ, NI niBase, NI * rgParam, USHORT cParam, OUT IDEF * piDef, OUT USHORT * piBuf);
        BOOL GetIDefs (HTARGET hTarget, BYTE bLanguage, IINST iinst, OUT IDEF ** ppiDef, OUT USHORT * piBuf, ULONG *puCnt);
	BOOL GetIDefs (HTARGET hTarget, BYTE bLanguage, IINST iinst, NI ni, TYP typ, NI niBase, NI * rgParam, USHORT cParam, OUT IDEF ** ppiDef, OUT USHORT * piBuf, ULONG *puCnt);	
	BOOL GetIDecl (HTARGET hTarget, BYTE bLanguage, IINST iinst, OUT IINST * piinstDecl);
	void UpdateBuffer (USHORT index);
	BOOL CheckParam (BYTE bLanguage, NI * rg1, USHORT c1, NI * rg2, USHORT c2);
	BOOL CompareParam (IINST iinst1, IINST iinst2);
	void GetClassFrMember (SZ szName, SZ * pszClass);

	// get a list of iProp given the modhdr
	BOOL GetIPropFrMod (NI ni, USHORT iModHdr, OUT USHORT * pindex, OUT USHORT ** prgProp, OUT ULONG * pcProp);
	void GetIPropFrMod (NI ni, USHORT cProp, NCB_PROP * rgProp, USHORT disp, OUT Array<USHORT> * prgProp); 
	BOOL GetIPropFrMod (SZ sz, USHORT iModHdr, OUT USHORT * pindex, OUT USHORT ** prgProp, OUT ULONG * pcProp);
	void GetIPropFrMod (SZ sz, USHORT cProp, NCB_PROP * rgProp, USHORT disp, OUT Array<USHORT> * prgProp); 
	BOOL FindFirstNi (SZ sz, USHORT cProp, NCB_PROP * rgProp, OUT USHORT * piFirst);
	BOOL getBaseArray (OUT IINST ** ppiinst, OUT ULONG * pciinst, USHORT iModHdr, NCB_USE * rgUse, USHORT count);
	BOOL getAttributeArray(HTARGET hTarget, BYTE bLanguage, IINST iinst, Array<IINST> * prgiinst);
	BOOL getAttributeArray(Array<IINST> * prgiinst, USHORT iModHdr, NCB_USE * rgUse, USHORT count);
	BOOL getParams (IINST iinst, OUT NI ** prgParam, OUT ULONG * pcParam);
	BOOL EnFrIinst (IINST iinst, NCB_ENTITY * pEn);
	BOOL EnFrIinst (IINST iinst, NCB_ENTITY * pEn, HTARGET hTarget);
	BOOL GetGlobalClass (IMOD imod, Array<IINST> * prgiinst, BOOL bGlobalOnly);
	BOOL GetGlobalClass (HTARGET hTarget, BYTE bLanguage, Array<IINST> * prgiinst, BOOL bGlobalOnly);
	BOOL GetGlobalOther(IMOD imod, MBF mbf, Array<IINST> * prgiinst, BOOL bGlobalOnly);
	BOOL GetGlobalOther (HTARGET hTarget, BYTE bLanguage, MBF mbf, Array<IINST> * prgiinst, BOOL bGlobalOnly);

	BOOL GetGlobalClass (IMOD imod, Array<IinstInfo> * prgiinst);
	BOOL GetGlobalClass (HTARGET hTarget, BYTE bLanguage, Array<IinstInfo> * prgiinst);
	BOOL GetGlobalOther(IMOD imod, MBF mbf, Array<IinstInfo> * prgiinst);
	BOOL GetGlobalOther (HTARGET hTarget, BYTE bLanguage, MBF mbf, Array<IinstInfo> * prgiinst);
	
	BOOL IrefInfo (IREF iref, BOOL bStart, OUT SZ *pszModule, OUT LINE *piline);
	BOOL IdefInfo (HTARGET hTarget, BYTE bLanguage, IDEF idef, BOOL bStart, OUT SZ *pszModule, OUT LINE *piline);
	BOOL IdeclInfo (HTARGET hTarget, BYTE bLanguage, IINST iinst, OUT SZ *pszModule, OUT LINE *piline, OUT LINE *piEndline = NULL);
	BOOL IsGlobalName (SZ_CONST sz);
	BOOL ImodFrNi (NI ni, OUT IMOD * pimod);
	BOOL CreateImod (NI ni, OUT IMOD * pimod);
	BOOL FindITarget (HTARGET hTarget, USHORT * piTarget);
	BOOL FindImodInfo (USHORT iTarget, IMOD imod, USHORT * piModInfo);
	BOOL IsIModInfoInIncl (USHORT iTarget, USHORT iModInfo, USHORT iInclModInfo, USHORT * piIncl);
	BOOL delAllInclFrMod (USHORT iTarget, USHORT iModInfo);
	BOOL isInTarget (IMOD imod, USHORT index);
	BOOL getAllFlattenDeps (HTARGET hTarget, IMOD imod, Array<IMOD> &rgMod, BOOL &bNotifyBuild);
	BOOL imodInArray (IMOD imod, Array<IMOD> &rgMod);
	BOOL fHasGlobals (HTARGET hTarget, BYTE bLanguage, MBF mbf);
	BOOL IsClassType (TYP typ);
	void AddToQ (NiQ * pqItem, UINT uSize, HTARGET hTarget, BYTE bLanguage);
	// used by OpenNcb:
	BOOL IsSzInTable (SZ szName, USHORT* pindex);
	BOOL IsPdbInTable (PDB * pdb, USHORT* pindex);
	// add/delete/change queue
	BOOL IsInQ (Array<NiQ> & rgiinst, IINST iinst, USHORT * pindex);
	BOOL delUnreachable();
	// helper functions
	BOOL getOverloadArray(HTARGET hTarget, BYTE bLanguage, SZ sz, MBF mbf, Array<IINST>* prgiinst);
	BOOL getOverloadArrayWildcardFromMap(HTARGET hTarget, BSCEX_SEARCHOPT eSrchOpts, SZ sz, MBF mbf, Array<IINST>* prgiinst, MapNiToIInstArray *map);
	BOOL StrMatchWildcard(SZ szPattern, SZ szCandidate, BOOL fCaseSensitive=TRUE);
	BOOL getAllModulesArray(HTARGET hTarget, BYTE bLanguage, Array<IMOD>* prgimod);
	SZ   getTypeHelper(IINST iinst);
	BOOL GetClassMembersNameMapHelper(IINST iinst, Array<SZ>* prgTArgs, Array<SZ>* prgTFormals, BOOL fOnlyBases, OUT NcbNameMap ** ppncbnmp, BOOL fConstructors);
	void StripNSQname(SZ szFullQname, SZ szPrevNS, SZ *pszName, SZ *pszPrevNS);
	BOOL getDeclForDefn(HTARGET hTarget, BYTE bLanguage, SZ sz, TYP typ, ATR32 atr, IINST defnIinst, OUT IINST *piinst);
	BOOL checkNamespaceParents(IINST iinst, SZ sz);
	BOOL getIinstByvalue3(HTARGET hTarget, BYTE bLang, SZ sz, TYP typ, ATR32 atr, Array<IINST>* rgiinst);


#if CC_MULTIPLE_STORES
	// store related
	BOOL addAStore(USHORT index, NI ni, STORE_TYP st);
	BOOL delAStore(USHORT index);
	BOOL IsChildNcb() const;					// child Ncb's are opened by openStore()
	Ncb* NcbForStore(USHORT index) const;
	// multiple store helper functions
	IMOD LogicalizeImod(IMOD imod, USHORT store) const;
	IMOD PhysicalizeImod(IMOD imod) const;
	IINST LogicalizeIinst(IINST iinst, USHORT store) const;
	IINST PhysicalizeIinst(IINST iinst) const;
	NI LogicalizeNi(NI ni, USHORT store) const;
	NI PhysicalizeNi(NI ni) const;
	USHORT StoreForImod(IMOD imod) const;
	USHORT StoreForIinst(IINST iinst) const;
	USHORT StoreForNi(NI ni) const;
	Ncb* GetNcbForStore(USHORT store);			// opens the store if needed
#endif	// CC_MULTIPLE_STORES



	// data members
	PDB	*		m_pdb;							// our PDB file
	NameMap *	m_pnm;							// Name map, our hash table
	USHORT		m_cTargets;
#if CC_MULTIPLE_STORES
	USHORT		m_cStores;						// used most commonly with FOR_ALL_EXISTING_STORES
#endif	// CC_MULTIPLE_STORES
	BOOL		m_fIOwnThePdb;
	BOOL		m_bGraphBuilt;
	BOOL		m_bWrite;
    BOOL        m_fCaseSensitiveOn;
	
	// target info headers
	NCArray<NCB_TARGETINFO>	m_rgTargetInfo;		// array of target information
												// each element represent target information
	Array<NCB_MODHDR>		m_rgModHdr;			// array of module headers
	Map<NcbString, IMOD, CNcbStringHasher> m_mapFilenameImod; // map of name to imod
    Array<NcbString>        m_rgFilenames;      // map of imod to filename
#if CC_MULTIPLE_STORES
	Array<NCB_STORE_INMEM>	m_rgStoreInfo;		// array of store information
#endif	// CC_MULTIPLE_STORES
	Array<BYTE>				m_rgStat;			// flags to check if the mods are loaded for read/write
												// size should be the same as m_rgModHdr
	Array<BYTE>				m_rgNotifyBuild;	// size should be the same as m_rgModHdr
												// REVIEW: should be merged as a structure
												// with m_rgStat.
												// Used to check if we need to notify the build
												// system when we init the file
												// Default is FALSE
	NCB_CONTENT m_rgContBuf[NCB_MOD_BUF_SIZE];	// content buffer. This buffer is used to load the
												// module into the memory for READ ONLY
												// We need another structure that is less restrictive
												// for WRITING
    void *m_rgEventBuffer,*m_rgAfterEventBuffer;// buffers for the beforeUpdate/afterUpdate arrays
	NCArray<NCB_CONTENT_INMEM> m_rgRWBuf;		// content buffer in a writable form.
	MapNiToIInstArray		m_mapNiDefs;        // map NIs to their defining IINSTs
	NCArray<NcbNotifyQ>		m_rgNotifyQ;
	BOOL					m_bNotifyNow;		// set the notification 
	CRITICAL_SECTION		m_cs;				// critical section for NCB access
	HANDLE					m_hMutex;			// mutex for notification queue
	int						m_nSearchRecurse;	// recursion level in Ncb::GetClassMembersNameMapHelper

	NISearchArrays			m_NISearchArrays;	// This is for keeping a collection of NI search arrays
	MapNiToIInstArray		m_mapRefInfo;		// map NIs to the enclosing IINSTs in which they appear
    BOOL                    m_fIModLimitExceeded;

	// friends and family:
	friend class EnumNi;	
	friend class NcWrap;						// wrapper class for Ncb



};


#if CC_MULTIPLE_STORES
// Use to iterate over all existing stores including current one
#define FOR_ALL_EXISTING_STORES_INCLUDING_PROJECT(x)	for (USHORT x=0; x <= m_cStores; x++)
// Use to iterate over all existing stores
#define FOR_ALL_EXISTING_STORES(x)	for (USHORT x=1; x <= m_cStores; x++)
// Use to iterate over the entire possible range of stores
#define FOR_ALL_STORES(x)	for (USHORT x=1; x < NCB_MAXSTORES; x++)
#endif	// CC_MULTIPLE_STORES

// helper function to match mbf with TYP defined in ncbsc.cpp
BOOL TypFilter (TYP typ, MBF mbf);

//////////////////////////////////
// class to enumerate all the object
// sharing the same Ni
//////////////////////////////////
class EnumNi 
{
public:
	IMOD	m_iModHdr;			// module header
	USHORT  m_index;			// index either to RWBuf or ContBuf
	BYTE	m_BufType;			// either in RWBuf or ContBuf
	USHORT	m_iProp;			// prop index
private:
	NI		m_ni;				// ni to compare: or
	SZ		m_sz;				// sz to compare:
	Ncb	*	m_pncb;				// pointer to Ncb
	USHORT	*m_rgProp;			// prop array
	ULONG	m_cProp;			// size of prop array
	USHORT	m_i;				// index to the m_rgProp;
	HTARGET m_hTarget;			// target specifier
	BYTE	m_bLanguage;		// language specifier
	BOOL	*m_rgbModVisit;		// a bool whether we have visited the mod or not 
public:
	EnumNi (HTARGET hTarget, BYTE bLanguage, NI ni, Ncb * pncb);
	EnumNi (HTARGET hTarget, BYTE bLanguage, SZ sz, Ncb * pncb);
	~EnumNi();
	BOOL GetNext();
	void SkipNi();
};



//////////////////////////////////
// Interface to get reference info
//	associated with each IINST.
//////////////////////////////////
PdbInterface RefBlock
{
	virtual BOOL getAllIinsts(IINST **ppiinst, ULONG *pciinst) pure;
	virtual BOOL getLineNumsForIinst(IINST iinst, ULONG **ppline, ULONG *pcline) pure;
	virtual void disposeArray(void *pAnyArray) pure;
	virtual BOOL close() pure;
};


#ifdef __NCB_INLINES__
#include "ncbrowse.inl"
#endif

#endif
