#ifndef __PERS_REFIO_H_
#define __PERS_REFIO_H_

#include "PropPersRef.h"
#include <dmusici.h>

class CPersRefMgr;

typedef struct _DMUS_IO_PERS_REF
{
	MUSIC_TIME	mtTime;		// Time of personality reference event
	DWORD		dwMeasure;	// Measure event occurs in
	BYTE		bBeat;		// Beat event occurs in
	BYTE		bPad[3];	// Padding
	DWORD		dwProjLength;	// Length of text of WCHARS (in # of bytes) that follows the structure
	DWORD		dwNameLength;	// Length of text of WCHARS (in # of bytes) that follows the structure
	DWORD		dwDescLength;	// Length of text of WCHARS (in # of bytes) that follows the structure
} DMUS_IO_PERS_REF;

class CPersRefItem : public CPropPersRef
{
public:
	CPersRefItem( CPersRefMgr* pPersRefMgr );
	~CPersRefItem();

	BOOL After(const CPersRefItem& PersRef);
	BOOL Before(const CPersRefItem& PersRef);

	CPersRefMgr* m_pPersRefMgr;

//	Used to track the selection of the personality reference in display.
	BOOL		 m_fSelected;		// This personality reference is currently selected.
	BOOL		 m_fRemoveNotify;
};

#endif // __PERS_REFIO_H_
