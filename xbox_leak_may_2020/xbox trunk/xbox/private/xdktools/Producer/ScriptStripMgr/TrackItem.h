#ifndef __SCRIPT_TRACKITEM_H_
#define __SCRIPT_TRACKITEM_H_

#include <DMUSProd.h>
#include <dmusici.h>
#include "ScriptDesigner.h"

#pragma pack(2)

typedef struct FileListInfo
{
	FileListInfo()
	{
		pIProject = NULL;
		memset( &guidFile, 0, sizeof(GUID) );
	}

	IDMUSProdProject*	pIProject;
	CString				strProjectName;
	CString				strName;
	CString				strDescriptor;
	GUID				guidFile;
} FileListInfo;

typedef struct FileRef
{
	FileRef()
	{
		pIDocRootNode = NULL;
		fRemoveNotify = FALSE;
	}

	IDMUSProdNode*		pIDocRootNode;	// Pointer to referenced file's DocRoot node
	BOOL				fRemoveNotify;
	FileListInfo		li;
} FileRef;

#pragma pack()


class CTrackMgr;

// m_dwBits
#define UD_DRAGSELECT		0x00000001
#define UD_MULTIPLESELECT	0x00000002


// Flags for SetTimePhysical()
#define STP_LOGICAL_NO_ACTION				1
#define STP_LOGICAL_SET_DEFAULT				2
#define STP_LOGICAL_ADJUST					3
#define STP_LOGICAL_RECALC_MEASURE_BEAT		4
#define STP_LOGICAL_FROM_BEAT_OFFSET		5


// m_wFlags
#define RF_TOP_ITEM		0x0001
#define RF_PENDING_LOAD	0x0002


class CTrackItem
{
public:
	CTrackItem();
	CTrackItem( CTrackMgr* pTrackMgr );
	CTrackItem( CTrackMgr* pTrackMgr, const CTrackItem& );
	virtual ~CTrackItem();

	void Clear();
	void ClearListInfo();
	void Copy( const CTrackItem* pItem );

	HRESULT SetFileReference( IDMUSProdNode* pINewDocRootNode );
	HRESULT SetFileReference( GUID guidFile );
	HRESULT SetListInfo( IDMUSProdFramework* pIFramework );
	void FormatUIText( CString& strText );
	void SwitchTimeBase();
	HRESULT SetTimePhysical( REFERENCE_TIME rtTimePhysical, short nAction );
	HRESULT SetTimeLogical( REFERENCE_TIME rtTimeLogical );

public:
	// Fields that are persisted
	DWORD			m_dwFlagsDM;		// DirectMusic DMUS_IO_SCRIPTTRACKF flags
	REFERENCE_TIME	m_rtTimeLogical;	// Belongs To
	REFERENCE_TIME	m_rtTimePhysical;	// Time of event (REFERENCE_TIME or MUSIC_TIME)
	FileRef			m_FileRef;			// Info pertaining to referenced file
	CString			m_strRoutine;		// Name of script routine

	// Runtime only fields
    long			m_lMeasure;			// What measure (or minute) this item falls on
    long			m_lBeat;			// What beat (or second) this item falls on
    long			m_lTick;			// What tick (or ms) this item falls on
    long			m_lLogicalMeasure;	// What measure this item belongs to
    long			m_lLogicalBeat;		// What beat this item belongs to
	DWORD			m_dwBitsUI;			// Various bits
	WORD			m_wFlagsUI;			// Various flags
	BOOL			m_fSelected;		// This item is currently selected


private:
	CTrackMgr*		m_pTrackMgr;		// Will be NULL when CTrackItem used for properties
};

#endif // __SCRIPT_TRACKITEM_H_
