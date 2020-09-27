#ifndef __MUTELIST_H__
#define __MUTELIST_H__ 1
#include <afxtempl.h>
#pragma warning( push )
#pragma warning( disable : 4201 )
#include <dmusici.h>
#pragma warning( pop )
#include <dmusicf.h>

class CMuteList {
public:
    CMuteList();
	~CMuteList();
	HRESULT IMA_AddMute( IStream* pIStream, long lRecSize );
	HRESULT CreateMuteTrack( class CTrack** ppTrack );
	interface IDMUSProdFramework*	m_pIFramework;

private:
	HRESULT DM_SaveMuteList( interface IDMUSProdRIFFStream* pIRIFFStream );
	void RemoveAll();

	CTypedPtrList<CPtrList, DMUS_IO_MUTE*>	m_lstDMMutes;
};

#endif //__MUTELIST_H__
