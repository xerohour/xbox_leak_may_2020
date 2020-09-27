// TrackObject.h : Declaration of the CTrackMgr

#ifndef __PARAM_OBJECT_H_
#define __PARAM_OBJECT_H_

#include <dmusicf.h>
#include "MedParam.h"
#include "RiffStrm.h"
#include "newparameterdialog.h"

/////////////////////////////////////////////////////////////////////////////
// CTrackObject
class CTrackObject  
{
friend class CTrackMgr;
friend class CParamStrip;

public:
	CTrackObject( CTrackMgr* pTrackMgr );
	~CTrackObject();

	BOOL	IsEmptyTrackObject();
	HRESULT AddStrip(StripInfo* pStripInfo);
	CString GetMinimizedStripText();
	void	RefreshAllStrips();
	CString GetName();
	static	HRESULT GetParamEnumTypeText(GUID guidObject, DWORD dwParamIndex, WCHAR** ppwchParamEnums);
	static	HRESULT ParseEnumText(WCHAR* pwchParamText, CString& sParamName, CString& sUnitLabel, CList<CString, CString&>* plstEnumTypes, DWORD* pdwCount);
	GUID	GetCLSID();
	void	GetObjectHeader(DMUS_IO_PARAMCONTROLTRACK_OBJECTHEADER& oObjectHeader);
	void	RefreshObjectProperties();	
	BOOL	IsEqualObject(StripInfo* pStripInfo);
    BOOL    IsEqualObject(GUID guidInstance);
	BOOL	IsDisabled();

protected:
	HRESULT LoadObject( IDMUSProdRIFFStream* pIRiffStream, MMCKINFO* pckParent );
	HRESULT SaveObject( IDMUSProdRIFFStream* pIRiffStream );
	void InsertByAscendingParameter( CParamStrip *pParamStrip );
    HRESULT SaveObjectDesignData( IDMUSProdRIFFStream* pIRiffStream );


	// Change of view on param strips
	void OnChangeParamStripView(STRIPVIEW svNewStripView);

	// Performs action on all CParamStrips
	void	AllStrips_OnUpdate( REFGUID rguidType, DWORD dwGroupBits, void* pData );
	void	AllStrips_Invalidate();
	bool	AllStrips_RecomputeTimes(); // Returns true if anything changed
	void	AllStrips_RecomputeMeasureBeats();
	void	AllStrips_AddToTimeline();
	void	AllStrips_RemoveFromTimeline();
	BOOL	AllStrips_SetNbrGridsPerSecond( short nNbrGridsPerSecond );
	void	AllStrips_SwitchTimeBase( void );


	HRESULT	RemoveStripFromTimeline(CParamStrip* pParamStrip);
	HRESULT AddStripToTimeline(CParamStrip* pParamStrip, int nPosition);
	void	DeleteStrip(CParamStrip* pParamStrip);
	HRESULT DeleteAllStrips();
	HRESULT DeleteAllSelectedStrips();
	
	CDMOInfo* CheckListForEqualObject(CPtrList& lstObjects, CTrackObject* pTrackObject);

	// Helpers
	DWORD			GetNumberOfStrips();
	CDMToolInfo*	GetToolInfo(DWORD dwStage, GUID clsidObject);
	CDMOInfo*		GetDMOInfo(CTrackObject* pTrackObject);
	CString			GetToolStageName(DWORD dwStage);
	CString			GetDMOStageName(DWORD dwStage);
	CString			GetPChannelText();
	CString			GetObjectName();


protected:
	// Fields used for UI 
	CTrackMgr*		m_pTrackMgr;

	// Fields that are persisted (DirectMusic parameter control track data)
	CString			m_sObjectName;			// The Object Name
	DWORD			m_dwPChannel;			// PChannel number
	DWORD			m_dwBuffer;				// Buffer index
	DWORD			m_dwEffectIndex;		// EffectIndex
	CString			m_sPChannelText;		// PChannel text for the tool/DMO
	CString			m_sStageName;			// Stage Name
	DWORD			m_dwStage;				// Where object is located
	GUID			m_guidObject;			// GUID of object
    GUID            m_guidInstance;         // GUID for the instance of this DMO
	GUID			m_guidTimeFormat;		// GUID for the timeformat
	DWORD			m_dwObjectIndex;		// index of object
	DWORD			m_dwObjectFlagsDM;		// Various flags pertaining to object
	CParamStrip*	m_pMinimizedStrip;		// minimized strip
	BOOL			m_bDisabled;			// Object doesn't affect the playback


	STRIPVIEW		m_ParamStripView;
	CTypedPtrList<CPtrList, CParamStrip*> m_lstParamStrips;
};


#endif //__PARAM_OBJECT_H_
