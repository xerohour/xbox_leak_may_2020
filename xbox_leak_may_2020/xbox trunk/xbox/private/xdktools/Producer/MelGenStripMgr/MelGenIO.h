#ifndef __MELGENIO_H_
#define __MELGENIO_H_

#include "PropMelGen.h"
#include <dmusici.h>

class CMelGenMgr;

class CMelGenItem : public CPropMelGen
{
public:
	CMelGenItem( CMelGenMgr* pMelGenMgr );
	~CMelGenItem();

	BOOL After(const CMelGenItem& MelGen);
	BOOL Before(const CMelGenItem& MelGen);
	void SetSelectFlag( BOOL fSelected );

	CMelGenMgr*	m_pMelGenMgr;

//	Used to track the selection of the style reference in display.
	BOOL		 m_fSelected;		// This style reference is currently selected.
	BOOL		 m_fRemoveNotify;
};

#endif // __MELGENIO_H_
