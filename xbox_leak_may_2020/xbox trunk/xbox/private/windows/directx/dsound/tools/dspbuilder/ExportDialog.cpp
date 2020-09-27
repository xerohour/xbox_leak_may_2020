/*++

Copyright (c) 2001 Microsoft Corporation

Module Name:

	ExportDialog.cpp

Abstract:

	Export options dialog

Author:

	Robert Heitkamp (robheit) 14-Jun-2001

Revision History:

	14-Jun-2001 robheit
		Initial Version

--*/

//------------------------------------------------------------------------------
//	Includes:
//------------------------------------------------------------------------------
#include "ExportDialog.h"
#include "resource.h"

//------------------------------------------------------------------------------
//	CExportDialog::CExportDialog
//------------------------------------------------------------------------------
CExportDialog::CExportDialog(void)
/*++

Routine Description:

	Constructor

Arguments:

	None

Return Value:

	None

--*/
{
	m_bPatchXtalkToLFE	= FALSE;
	m_bIncludeXtalk		= TRUE;
}

//------------------------------------------------------------------------------
//	CExportDialog::CExportDialog
//------------------------------------------------------------------------------
CExportDialog::CExportDialog(
							 IN const CStringArray&	configs
							 ) : CDialog(IDD_EXPORT_DIALOG)
/*++

Routine Description:

	Constructor

Arguments:

	IN configs - Reverb configurations

Return Value:

	None

--*/
{
	SetConfigs(configs);
	m_bPatchXtalkToLFE	= FALSE;
	m_bIncludeXtalk		= TRUE;
}

//------------------------------------------------------------------------------
//	CExportDialog::~CExportDialog
//------------------------------------------------------------------------------
CExportDialog::~CExportDialog(void)
/*++

Routine Description:

	Destructor

Arguments:

	None

Return Value:

	None

--*/
{
}

//------------------------------------------------------------------------------
//	CExportDialog::OnOK
//------------------------------------------------------------------------------
void
CExportDialog::OnOK(void)
{
	CComboBox*	pComboBox;

	m_bPatchXtalkToLFE	= IsDlgButtonChecked(IDC_PATCH_XTALK_TO_LFE) ? TRUE : FALSE;
	pComboBox			= (CComboBox*)GetDlgItem(IDC_I3DL2_COMBO);
	m_bIncludeXtalk		= IsDlgButtonChecked(IDC_EXPORT_OPTIONS_XTALK) ? TRUE : FALSE;

	if(pComboBox->GetCurSel() == 0)
		m_i3dl2 = _T("");
	else
		m_i3dl2 = m_configs[pComboBox->GetCurSel()-1];

	CDialog::OnOK();
}

//------------------------------------------------------------------------------
//	CExportDialog::OnInitDialog
//------------------------------------------------------------------------------
BOOL
CExportDialog::OnInitDialog(void)
/*++

Routine Description:

	Handles the initialization of the dialog by adding the configs to the
	combo box

Arguments:

	None

Return Value:

	None

--*/
{
	CComboBox*	pComboBox;
	CRect		rect;
	CRect		dRect;
	int			i;

	if(CDialog::OnInitDialog())
	{
		// Setup the list
		pComboBox = (CComboBox*)GetDlgItem(IDC_I3DL2_COMBO);
		pComboBox->ResetContent();
		pComboBox->AddString(_T("<None>"));
		for(i=0; i<m_configs.GetSize(); ++i)
			pComboBox->AddString(m_configs[i]);

		// Select a reverb
		if(m_i3dl2.IsEmpty())
			pComboBox->SetCurSel(0);
		else
		{
			for(i=0; i<m_configs.GetSize(); ++i)
			{
				if(m_i3dl2 == m_configs[i])
				{
					pComboBox->SetCurSel(i+1);
					break;
				}
			}
			if(i == m_configs.GetSize())
			{
				pComboBox->SetCurSel(0);
				m_i3dl2.Empty();
			}
		}

		// Adjust the dropdown height
		GetClientRect(dRect);
		ClientToScreen(dRect);
		pComboBox->GetClientRect(rect);
		pComboBox->ClientToScreen(rect);
		rect.left	-= dRect.left;
		rect.right	-= dRect.left;
		rect.top	-= dRect.top;
		rect.bottom	= rect.top + 100;
		pComboBox->MoveWindow(rect);

		// Check the patch box
		CheckDlgButton(IDC_PATCH_XTALK_TO_LFE, (unsigned int)m_bPatchXtalkToLFE);

		// Include Xtalk?
		CheckDlgButton(IDC_EXPORT_OPTIONS_XTALK, (unsigned int)m_bIncludeXtalk);
		return TRUE;
	}
	return FALSE;
}

