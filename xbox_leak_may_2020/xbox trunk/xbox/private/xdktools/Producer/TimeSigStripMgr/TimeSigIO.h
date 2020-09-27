#ifndef __TIMESIGIO_H_
#define __TIMESIGIO_H_

#include "PropTimeSig.h"
#include <dmusici.h>

class CTimeSigMgr;

class CTimeSigItem : public CPropTimeSig
{
public:
	CTimeSigItem( CTimeSigMgr* pTimeSigMgr );
	~CTimeSigItem();

	BOOL After(const CTimeSigItem& TimeSig);
	BOOL Before(const CTimeSigItem& TimeSig);
	void SetSelectFlag( BOOL fSelected );

	CTimeSigMgr*	m_pTimeSigMgr;

//	Used to track the selection of the style reference in display.
	BOOL		 m_fSelected;		// This style reference is currently selected.
	BOOL		 m_fRemoveNotify;
};

#endif // __TIMESIGIO_H_
