#ifndef __NCPARSEEX_H__
#define __NCPARSEEX_H__

#include <ncparse.h>

struct UpdateItem
{
	IINST m_iinstOld;
	IINST m_iinstNew;
	IINST m_iinstParent;
	OPERATION m_op;
};

PdbInterface NcbParseEx : public NcbParse
{
	virtual BOOL setEndLine (IINST iinst, LINE lnEnd) pure;
	virtual BOOL irefEndInfo(IREF iref, OUT SZ *pszModule, OUT LINE *piline) pure;
	virtual void setModLang (IMOD imod, BYTE bLanguage) pure;
	virtual BOOL setIDLAttrib (IINST iinst, SZ szName, SZ szValue, OUT IINST *piinst) pure;
	virtual BOOL setIDLMFCComment (IINST iinst, SZ szType, SZ szValue, OUT IINST *piinst) pure;
#if CC_MULTIPLE_STORES
	// New ones added to support multiple stores
	virtual BOOL addStore(SZ szName, STORE_TYP st) pure;
	virtual BOOL delStore(SZ szName) pure;
	virtual BOOL openNextStore() pure;
	virtual BOOL pageInNextStore() pure;
	virtual BOOL delAllStores() pure;
	virtual BOOL getAllStoresArray(OUT NI** ppni, OUT USHORT* pcStores) pure;
	virtual BOOL openStore(SZ szName, USHORT* pStoreIdx) pure;
	virtual BOOL closeStore(USHORT storeIdx) pure;
	virtual BOOL isExistingStore(SZ szName, OUT STORE_TYP* pst, OUT USHORT* psi) pure;
	virtual SZ szFrStoreIndex(USHORT idx) pure;
#endif	// CC_MULTIPLE_STORES
	virtual BOOL beforeUpdate(IMOD imod) pure;
	virtual BOOL afterUpdate(IMOD imod, UpdateItem **ppUpdates, ULONG *pcUpdates, BOOL *pfIncludesChanged) pure;

	virtual BOOL addReference(SZ szName, IINST iinst) pure;
	virtual BOOL delModuleReferences(IMOD imod) pure;
		// delete all modules not in the project - part of NcbParseEx
    virtual BOOL delAllUnrefMod() pure;

};


class CBscLock
{
private:
    NcbParseEx * m_pDBase;
public:
    CBscLock (NcbParseEx * pDBase)
    {
        m_pDBase = pDBase;
        m_pDBase->lock();
    };

    ~CBscLock()
    {
        m_pDBase->unlock();
        m_pDBase = NULL;
    };
};


#endif __NCPARSEEX__
