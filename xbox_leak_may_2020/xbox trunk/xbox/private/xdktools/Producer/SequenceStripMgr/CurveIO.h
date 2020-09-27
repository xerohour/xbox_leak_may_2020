#ifndef __CURVEIO_H_
#define __CURVEIO_H_

#include "EventItem.h"
#include <dmusici.h>

#define MIN_CC_VALUE	0
#define MAX_CC_VALUE	127

#define MIN_PB_VALUE	1
#define MAX_PB_VALUE	16383
#define MIN_PB_DISP_VALUE	-8191
#define MAX_PB_DISP_VALUE	 8191
#define PB_DISP_OFFSET		 8192
#define MIN_MERGE_INDEX	0
#define MAX_MERGE_INDEX 999
#define MAX_RPN_TYPE_NUMBER	16383
#define MIN_RPN_TYPE_NUMBER	0
#define MAX_RPN_VALUE	16383
#define MIN_RPN_VALUE	0

#define INVALID_CC_VALUE	0xFFFFFFFF

class CCurveItem : public CEventItem
{
public:
	CCurveItem();
	CCurveItem( const CCurveItem& );
	CCurveItem( const CCurveItem *pCurveItem );
	CCurveItem( const struct FullSeqEvent* );
	void		Clear();

	void		Copy( const CCurveItem* pCurveItem );

	void		CopyFrom(const DMUS_IO_CURVE_ITEM& item);
	void		CopyFrom(const DMUS_IO_SEQ_ITEM& item);
	void		CopyFrom(const struct FullSeqEvent*);
	
	bool		CopyTo(DMUS_IO_CURVE_ITEM& item);	// fails if item not ET_CCHANGE

	DWORD		ComputeCurve( MUSIC_TIME* pmtIncrement );
	void SetDefaultResetValues( MUSIC_TIME mtLength );

	// curve specific data
    MUSIC_TIME	m_mtResetDuration;
	short		m_nStartValue;
	short		m_nEndValue;
	short		m_nResetValue;
	BYTE		m_bType;	
	BYTE		m_bCurveShape;
	BYTE		m_bCCData;
	BYTE		m_bFlags;
	WORD		m_wParamType;
	WORD		m_wMergeIndex;

	// Draving data
	RECT		m_rectFrame;
	RECT		m_rectSelect;
	MUSIC_TIME	m_mtCurrent;

private:
	CCurveItem& operator=(const CCurveItem&);
};

#endif // __CURVEIO_H_
