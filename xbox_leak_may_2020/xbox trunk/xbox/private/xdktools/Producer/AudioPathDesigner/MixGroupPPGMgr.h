#if !defined(AFX_MIXGROUPPPGMGR_H__3AAC415E_7096_49B6_AAE4_7FA2DAFD39D6__INCLUDED_)
#define AFX_MIXGROUPPPGMGR_H__3AAC415E_7096_49B6_AAE4_7FA2DAFD39D6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// MixGroupPPGMgr.h : header file
//

#include <DMUSProd.h>
#include "DllBasePropPageManager.h"
#include "PPGItemBase.h"
#include "ItemInfo.h"

// {1D3748D4-80CC-45e8-AACB-BC6BC9142869}
static const GUID GUID_MixGroupPPGMgr = 
{ 0x1d3748d4, 0x80cc, 0x45e8, { 0xaa, 0xcb, 0xbc, 0x6b, 0xc9, 0x14, 0x28, 0x69 } };

#define CH_MIXGROUP_NAME			0x00000001
#define CH_MIXGROUP_SYNTH			0x00000002
#define CH_MIXGROUP_SYNTH_SETTINGS	0x00000004

#define MGI_HAS_SYNTH				0x00000001
#define MGI_CAN_EDIT_SYNTH			0x00000002
#define MGI_DONT_EDIT_NAME			0x00000004

struct MixGroupInfoForPPG : public PPGItemBase
{
	MixGroupInfoForPPG() : PPGItemBase()
	{
		m_ppgIndex = PPG_MIX_GROUP;
		m_dwFlags = 0;
	}
	void Copy( const MixGroupInfoForPPG &mixGroupInfoForPPG )
	{
		PPGItemBase::Copy( mixGroupInfoForPPG );
		m_strMixGroupName = mixGroupInfoForPPG.m_strMixGroupName;
		m_PortOptions.Copy( &mixGroupInfoForPPG.m_PortOptions );
		m_dwFlags = mixGroupInfoForPPG.m_dwFlags;
	}
	CString		m_strMixGroupName;
	PortOptions	m_PortOptions;
	DWORD		m_dwFlags;

};

/////////////////////////////////////////////////////////////////////////////
// CMixGroupPPGMgr

class CMixGroupPPGMgr : public CDllBasePropPageManager
{
friend class CTabMixGroup;

public:
	CMixGroupPPGMgr();
	virtual ~CMixGroupPPGMgr();

    // IDMUSProdPropPageManager functions
    HRESULT STDMETHODCALLTYPE SetObject( IDMUSProdPropPageObject* pINewPropPageObject );
	HRESULT STDMETHODCALLTYPE RemoveObject( IDMUSProdPropPageObject* pIPropPageObject );
    HRESULT STDMETHODCALLTYPE GetPropertySheetTitle( BSTR* pbstrTitle, BOOL* pfAddPropertiesText );
    HRESULT STDMETHODCALLTYPE GetPropertySheetPages( IDMUSProdPropSheet* pIPropSheet, LONG* hPropSheetPage[], short* pnNbrPages );
	HRESULT STDMETHODCALLTYPE OnRemoveFromPropertySheet();
    HRESULT STDMETHODCALLTYPE RefreshData();

	// Member variables
private:
	CTabMixGroup	*m_pTabMixGroup;

public:
	static short	sm_nActiveTab;
};

#endif // !defined(AFX_MIXGROUPPPGMGR_H__3AAC415E_7096_49B6_AAE4_7FA2DAFD39D6__INCLUDED_)
