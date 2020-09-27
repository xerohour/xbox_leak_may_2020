#ifndef __EVENTITEM_H_
#define __EVENTITEM_H_

#include <dmusici.h>
#include <dmusicf.h>

#define MIDI_NOTEOFF    0x80
#define MIDI_NOTEON     0x90
#define MIDI_PTOUCH     0xA0
#define MIDI_CCHANGE    0xB0
#define MIDI_PCHANGE    0xC0
#define MIDI_MTOUCH     0xD0
#define MIDI_PBEND      0xE0
#define MIDI_SYSX       0xF0
#define MIDI_MTC        0xF1
#define MIDI_SONGPP     0xF2
#define MIDI_SONGS      0xF3
#define MIDI_EOX        0xF7
#define MIDI_CLOCK      0xF8
#define MIDI_START      0xFA
#define MIDI_CONTINUE   0xFB
#define MIDI_STOP       0xFC
#define MIDI_SENSE      0xFE

class CEventItem 
{
public:
			CEventItem( );
	void	Clear();
	BOOL	After(const CEventItem& Item) const;
	BOOL	Before(const CEventItem& Item) const;

	inline MUSIC_TIME	AbsTime() const
	{
		return m_mtTime + m_nOffset;
	}
	inline MUSIC_TIME& MusicTime()
	{
		return m_mtTime;
	}
	inline short& Offset()
	{
		return m_nOffset;
	}

	MUSIC_TIME	m_mtTime;		// The time of this item
    MUSIC_TIME	m_mtDuration;
	short		m_nOffset;

	DWORD		m_dwBits;	// Various bits
//	Used to track the selection of the item in display.
	BOOL		m_fSelected;	// This item is currently selected.

protected:
	void		Copy( const CEventItem* pEventItem );
	void		CopyTo(DMUS_IO_SEQ_ITEM& item);
	BOOL		IsEqual( const CEventItem *pEventItem ) const;
};

#endif // __EVENTITEM_H_
