#ifndef __$$SAFE_ROOT$$ITEM_H_
#define __$$SAFE_ROOT$$ITEM_H_

#include <dmusici.h>

typedef struct _DMUS_IO_$$SAFE_ROOT$$
{
	MUSIC_TIME	mtTime;		// Time of event
	DWORD		dwMeasure;	// Measure event occurs in
	BYTE		bBeat;		// Beat event occurs in
	BYTE		bPad[3];	// Padding
	DWORD		dwLength;	// Length of text of WCHARS (in # of bytes) that follows the structure
} DMUS_IO_$$SAFE_ROOT$$;

#define UD_DRAGSELECT		0x00000001
#define UD_MULTIPLESELECT	0x00000002

class C$$Safe_root$$Item
{
public:
	C$$Safe_root$$Item();
	C$$Safe_root$$Item(const C$$Safe_root$$Item&);

	void Clear();
	void Copy( const C$$Safe_root$$Item* pItem );

	BOOL After(const C$$Safe_root$$Item& item);
	BOOL Before(const C$$Safe_root$$Item& item);

    long		m_lMeasure;	// What measure this item falls on
    long		m_lBeat;	// What beat this item falls on
	DWORD		m_dwBits;	// Various bits
	BOOL		m_fSelected;// This item is currently selected.
	CString		m_strText;	// Text contained in this event
};

#endif // __$$SAFE_ROOT$$ITEM_H_
