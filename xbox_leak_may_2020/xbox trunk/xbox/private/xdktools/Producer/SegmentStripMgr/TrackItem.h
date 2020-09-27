#ifndef __SEGMENT_TRACKITEM_H_
#define __SEGMENT_TRACKITEM_H_

#include <DMUSProd.h>
#include <dmusici.h>
#include "SegmentDesigner.h"
#include "StyleDesigner.h"

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
#define STP_LOGICAL_NO_ACTION	1
#define STP_LOGICAL_SET_DEFAULT	2
#define STP_LOGICAL_ADJUST		3


// m_wFlags
#define RF_TOP_ITEM			0x0001
#define RF_PENDING_LOAD		0x0002


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

	BOOL After( const CTrackItem& item );
	BOOL Before( const CTrackItem& item );

	HRESULT SetFileReference( IDMUSProdNode* pINewDocRootNode );
	HRESULT SetFileReference( GUID guidFile );
	HRESULT SetListInfo( IDMUSProdFramework* pIFramework );
	void FormatUIText( CString& strText );

	HRESULT	SetTimePhysical( MUSIC_TIME mtTimePhysical, short nAction );
	HRESULT	SetTimeLogical( MUSIC_TIME mtTimeLogical );

public:
	// Fields that are persisted
	DWORD			m_dwFlagsDM;			// DirectMusic DMUS_IO_SEGMENTTRACKF flags
	DWORD			m_dwPlayFlagsDM;		// DirectMusic DMUS_SEGF flags
	MUSIC_TIME		m_mtTimeLogical;		// Time of measure/beat 
	MUSIC_TIME		m_mtTimePhysical;		// Time of measure/beat/tick
	FileRef			m_FileRef;				// Info pertaining to referenced file
	CString			m_strMotif;				// Name of motif

	// Runtime only fields
    long			m_lMeasure;				// What measure this item falls on
    long			m_lBeat;				// What beat this item falls on
    long			m_lTick;				// What tick this item falls on
    long			m_lLogicalMeasure;		// What measure this item belongs to
    long			m_lLogicalBeat;			// What beat this item belongs to
	DWORD			m_dwBitsUI;				// Various bits
	WORD			m_wFlagsUI;				// Various flags
	BOOL			m_fSelected;			// This item is currently selected


private:
	CTrackMgr*		m_pTrackMgr;			// Will be NULL when CTrackItem used for properties
};

#endif // __SEGMENT_TRACKITEM_H_
