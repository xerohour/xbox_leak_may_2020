// DlgInsertEffect.cpp : implementation file
//

#include "stdafx.h"
#include "DlgInsertEffect.h"
#include "EffectInfo.h"
#include "AudioPath.h"
#define _SYS_GUID_OPERATORS_
#include "dmoreg.h"
#include "uuids.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgInsertEffect dialog


CDlgInsertEffect::CDlgInsertEffect(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgInsertEffect::IDD, pParent)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	m_pAudioPath = NULL;
	m_fUserChangedName = false;

	//{{AFX_DATA_INIT(CDlgInsertEffect)
	//}}AFX_DATA_INIT
}


void CDlgInsertEffect::DoDataExchange(CDataExchange* pDX)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgInsertEffect)
	DDX_Control(pDX, IDC_COMBO_EFFECT, m_comboEffect);
	DDX_Control(pDX, IDC_EDIT_INSTANCE, m_editInstance);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgInsertEffect, CDialog)
	//{{AFX_MSG_MAP(CDlgInsertEffect)
	ON_EN_KILLFOCUS(IDC_EDIT_INSTANCE, OnKillfocusEditInstance)
	ON_CBN_SELCHANGE(IDC_COMBO_EFFECT, OnSelchangeComboEffect)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgInsertEffect message handlers

BOOL CDlgInsertEffect::OnInitDialog() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	CDialog::OnInitDialog();

	IEnumDMO *pEnumDMO;

	DMO_PARTIAL_MEDIATYPE dmoPMT;
	dmoPMT.type = MEDIATYPE_Audio;
	dmoPMT.subtype = MEDIASUBTYPE_PCM;

	if( SUCCEEDED( DMOEnum( DMOCATEGORY_AUDIO_EFFECT, 0, 1, &dmoPMT, 1, &dmoPMT, &pEnumDMO ) ) )
	{
		pEnumDMO->Reset();
		CLSID clsidItem;
		WCHAR *pwcName;
		DWORD dwItemsFetched;
		while( S_OK == pEnumDMO->Next( 1, &clsidItem, &pwcName, &dwItemsFetched ) )
		{
			CString strName = pwcName;
			EffectInfo *pEffectInfo = new EffectInfo( strName, strName, clsidItem, GUID_NULL );

			m_lstEffects.AddHead( pEffectInfo );

			// Create the name to display
			AfxFormatString2( strName, IDS_INSERT_EFFECT_LIST, pEffectInfo->m_strName, pEffectInfo->m_strInstanceName );

			// Add the string to the combobox and point it's item at the associated EffectInfo
			int nIndex = m_comboEffect.AddString( strName );
			m_comboEffect.SetItemDataPtr( nIndex, pEffectInfo );

			::CoTaskMemFree( pwcName );
		}
		pEnumDMO->Release();
	}

	// Select the first item
	m_comboEffect.SetCurSel( 0 );

	// Set the instance name, based on the first item
	SetInstanceName();

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgInsertEffect::OnOK() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	// Get the index of the current combobox item
	int nIndex = m_comboEffect.GetCurSel();
	if( nIndex != CB_ERR )
	{
		// Get an EffectInfo pointer from the selected item
		m_EffectInfoToCopy.Copy( *static_cast<EffectInfo *>( m_comboEffect.GetItemDataPtr( nIndex ) ) );
	}

	// Get the instance name to use
	m_editInstance.GetWindowText( m_strInstanceName );

	CDialog::OnOK();
}

void CDlgInsertEffect::SetInstanceName( void )
{
	// Check that we have an AudioPath pointer
	if( m_pAudioPath )
	{
		// Get the currently selected index
		int nIndex = m_comboEffect.GetCurSel();
		if( nIndex != CB_ERR )
		{
			// Get a pointer to the effect
			EffectInfo *pEffectInfo = static_cast<EffectInfo *>( m_comboEffect.GetItemDataPtr( nIndex ) );

			// Copy the effect
			EffectInfo newEffectInfo;
			newEffectInfo.Copy( *pEffectInfo );

			// Now, get a unique name for this effect
			m_pAudioPath->GetUniqueEffectInstanceName( &newEffectInfo );

			// Update the display with the effect's instance name
			m_editInstance.SetWindowText( newEffectInfo.m_strInstanceName );

			// Save the instance name for use in OnKillfocusEditInstance
			m_strOldInstanceName = newEffectInfo.m_strInstanceName;
		}
	}
}

void CDlgInsertEffect::OnKillfocusEditInstance() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	// Get the new text for the instance name
	CString strNewInstanceText;
	m_editInstance.GetWindowText( strNewInstanceText );
	strNewInstanceText.TrimLeft();
	strNewInstanceText.TrimRight();

	// Check if the string is empty
	if( strNewInstanceText.IsEmpty() )
	{
		// Reset it to the last instance name used
		m_editInstance.SetWindowText( m_strOldInstanceName );
	}
	else
	{
		// Get the index of the current combobox item
		int nIndex = m_comboEffect.GetCurSel();
		if( nIndex != CB_ERR )
		{
			// Get an EffectInfo pointer from the selected item
			EffectInfo *pEffectInfo = static_cast<EffectInfo *>( m_comboEffect.GetItemDataPtr( nIndex ) );

			// Copy the effect
			EffectInfo newEffectInfo;
			newEffectInfo.Copy( *pEffectInfo );

			// Change the effect's instance name to the one entered by the user
			newEffectInfo.m_strInstanceName = strNewInstanceText;

			// Check if the name is unique
			m_pAudioPath->GetUniqueEffectInstanceName( &newEffectInfo );

			// If name wasn't unique, it was changed
			if( newEffectInfo.m_strInstanceName != strNewInstanceText )
			{
				// Set the display with the new name
				m_editInstance.SetWindowText( newEffectInfo.m_strInstanceName );
			}

			// Check if the name changed
			if( m_strOldInstanceName != newEffectInfo.m_strInstanceName )
			{
				// Save the instance name
				m_strOldInstanceName = newEffectInfo.m_strInstanceName;

				// Flag that the user changed the instance name
				m_fUserChangedName = true;
			}
		}
	}
}

void CDlgInsertEffect::OnSelchangeComboEffect() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	// If the user hasn't modified the instance name
	if( !m_fUserChangedName )
	{
		// Set the instance name, based on the chosen effect
		SetInstanceName();
	}
}
