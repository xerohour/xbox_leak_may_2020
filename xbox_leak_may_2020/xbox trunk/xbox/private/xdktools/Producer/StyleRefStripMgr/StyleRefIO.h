#ifndef __STYLE_REFIO_H_
#define __STYLE_REFIO_H_

#include "PropStyleRef.h"
#include <dmusici.h>

class CStyleRefMgr;

#define DMUS_FOURCC_STYLE_REF_DESIGN_CHUNK mmioFOURCC('p','s','r','d')

typedef struct _DMUS_IO_STYLE_REF_DESIGN
{
	DWORD		dwVariationSeed;
	BOOL		fVariationSeedActive;
} DMUS_IO_STYLE_REF_DESIGN;

class CStyleRefItem : public CPropStyleRef
{
public:
	CStyleRefItem( CStyleRefMgr* pStyleRefMgr );
	~CStyleRefItem();

	BOOL After(const CStyleRefItem& StyleRef);
	BOOL Before(const CStyleRefItem& StyleRef);
	void SetSelectFlag( BOOL fSelected );

	CStyleRefMgr*	m_pStyleRefMgr;

//	Used to track the selection of the style reference in display.
	BOOL		 m_fSelected;		// This style reference is currently selected.
	BOOL		 m_fRemoveNotify;
};

#endif // __STYLE_REFIO_H_
