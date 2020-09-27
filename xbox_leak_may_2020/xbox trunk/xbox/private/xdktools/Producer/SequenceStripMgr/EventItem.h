#ifndef __EVENTITEM_H_
#define __EVENTITEM_H_

#include <dmusici.h>
#include <dmusicf.h>

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
	bool		CopyTo(DMUS_IO_SEQ_ITEM& item);
	BOOL		IsEqual( const CEventItem *pEventItem ) const;
};

#endif // __EVENTITEM_H_
