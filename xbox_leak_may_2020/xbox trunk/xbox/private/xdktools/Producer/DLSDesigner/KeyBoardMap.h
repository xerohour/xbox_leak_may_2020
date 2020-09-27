//////////////////////////////////////////////////////////////////////
// KeyBoardMap.h 
//

#ifndef KEYBOARDMAP_H
#define KEYBOARDMAP_H

#include "objbase.h"

class RegionNote;

class KeyBoardMap : public IUnknown
{
public:

	KeyBoardMap() : m_pRegionMap(NULL), m_dwRef(0) { AddRef();}
	~KeyBoardMap() {}

    // IUnknown functions
    HRESULT STDMETHODCALLTYPE QueryInterface( REFIID riid, LPVOID *ppv );
    ULONG STDMETHODCALLTYPE AddRef();
    ULONG STDMETHODCALLTYPE Release();

	void SetRegionMap(RegionNote* pRegionMap) {ASSERT(pRegionMap); 
											   m_pRegionMap = pRegionMap;}

	RegionNote* GetRegionMap() {return m_pRegionMap;}
	
private:
	RegionNote* m_pRegionMap;
	DWORD		m_dwRef;
};

#endif