// ===========================================================
// UnknownDLSChunk.h
// ===========================================================
#ifndef __UNKDLSCHUNK_H__
#define __UNKDLSCHUNK_H__

#include "RiffStrm.h"
#include "alist.h"

//NOTE: Keeping 2 classes to follow the model for AList and AListItem.
//  as in CCollectionInstruments and CInstruments
class CUnknownChunkItem;

class CUnknownChunk: public AList
{
public:
    CUnknownChunk();
	~CUnknownChunk();

    // IUnknown functions
    HRESULT STDMETHODCALLTYPE QueryInterface( REFIID riid, LPVOID *ppv );
    ULONG STDMETHODCALLTYPE AddRef();
    ULONG STDMETHODCALLTYPE Release();

    //	AList overrides
    CUnknownChunkItem *GetHead();
    CUnknownChunkItem *RemoveHead();

	void Remove(CUnknownChunkItem *pChunk);
	void AddTail(CUnknownChunkItem *pChunk);

    // Additional functions
	HRESULT Load(IDMUSProdRIFFStream* pIRiffStream, MMCKINFO* pckMain, BOOL bIsListType);
    HRESULT Save(IDMUSProdRIFFStream* pIRiffStream);
    
// member variables
private:
    DWORD	  m_dwRef;
};


class CUnknownChunkItem : public AListItem
{
friend class CUnknownChunk;
public:
	
	DWORD Size();
    CUnknownChunkItem();
	~CUnknownChunkItem();
    
	// IUnknown functions
    HRESULT STDMETHODCALLTYPE QueryInterface( REFIID riid, LPVOID *ppv );
    ULONG STDMETHODCALLTYPE AddRef();
    ULONG STDMETHODCALLTYPE Release();

    // Additional methods
    HRESULT Load( IDMUSProdRIFFStream* pIRiffStream, MMCKINFO* pckMain );
    HRESULT Save(IDMUSProdRIFFStream* pIRiffStream);

	HRESULT Write(void* pv, DWORD* pdwCurOffset, DWORD dwIndexNextExtChk);

	
private:
	DWORD GetChunkSize(IDMUSProdRIFFStream* pIRiffStream, MMCKINFO* pckMain); // Descends through the subchunks and reports the size

// member variables
private:
    DWORD       m_dwRef;
    MMCKINFO    m_ck;
    BYTE *      m_pData;
    int         m_ListCount; 
    BOOL        m_bIsListItem;  // if true this is a list entry 

};

#endif