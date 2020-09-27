#ifndef __PROPCURVE_H_
#define __PROPCURVE_H_

#include "CurveIO.h"
#include "PropItem.h"

class CPropCurve : public CPropItem
{
public:
	CPropCurve( );
	CPropCurve( CSequenceMgr *pSequenceMgr );
	CPropCurve(const CCurveItem&, CSequenceMgr* pSequenceMgr);
	CPropCurve(const CCurveItem*, CSequenceMgr* pSequenceMgr);
	void	Copy( const CPropCurve* pPropItem );
	void	Import( const CCurveItem* pCurveItem );
	void	Clear();
	CPropCurve operator+=(const CPropCurve &propcurve);
	DWORD	ApplyToCurve( CCurveItem* pCurveItem ) const;
	DWORD	ApplyPropCurve( const CPropCurve* pPropItem );
	void	CalcResetBarBeatGridTick( void );
	void	CalcResetDuration( void );

	// curve specific data
	MUSIC_TIME	m_mtResetDuration;
	short		m_nStartValue;
	short		m_nEndValue;
	short		m_nResetValue;
	BYTE		m_bEventType;	
	BYTE		m_bCurveShape;
	BYTE		m_bCCData;
	BYTE		m_bFlags;
	WORD		m_wParamType;
	WORD		m_wMergeIndex;

	long		m_lResetBar;
	long		m_lResetBeat;
	long		m_lResetGrid;
	long		m_lResetTick;

	DWORD		m_dwUndetermined2;
	DWORD		m_dwChanged2;
};

#define UNDT_STARTVAL		(1 << 18)
#define UNDT_ENDVAL			(1 << 19)
#define UNDT_SHAPE			(1 << 20)
#define UNDT_FLIPVERT		(1 << 21)
#define UNDT_FLIPHORZ		(1 << 22)

#define UNDT_RESETENABLE	(1 << 23)
#define UNDT_RESETVAL		(1 << 24)
#define UNDT_RESETBAR		(1 << 25)
#define UNDT_RESETBEAT		(1 << 26)
#define UNDT_RESETGRID		(1 << 27)
#define UNDT_RESETTICK		(1 << 28)
#define UNDT_RESETDURATION	(1 << 29)

#define UNDT2_MERGEINDEX	(DWORD(1 << 0))
#define UNDT2_STARTCURRENT	(DWORD(1 << 1))

#define	CHGD_STARTVAL		UNDT_STARTVAL
#define	CHGD_ENDVAL			UNDT_ENDVAL
#define	CHGD_SHAPE			UNDT_SHAPE
#define	CHGD_FLIPVERT		UNDT_FLIPVERT
#define	CHGD_FLIPHORZ		UNDT_FLIPHORZ

#define	CHGD_RESETENABLE	UNDT_RESETENABLE
#define	CHGD_RESETVAL		UNDT_RESETVAL
#define	CHGD_RESETBAR		UNDT_RESETBAR
#define	CHGD_RESETBEAT		UNDT_RESETBEAT
#define	CHGD_RESETGRID		UNDT_RESETGRID
#define	CHGD_RESETTICK		UNDT_RESETTICK

#define	CHGD2_MERGEINDEX	UNDT2_MERGEINDEX
#define	CHGD2_STARTCURRENT	UNDT2_STARTCURRENT

#define UNDT_RESET_BARBEATGRIDTICK (UNDT_RESETBAR | UNDT_RESETBEAT | UNDT_RESETGRID | UNDT_RESETTICK)

#define CHGD_RESET_BARBEATGRIDTICK UNDT_RESET_BARBEATGRIDTICK

#endif // __PROPCURVE_H_
