/*++

Copyright (c) 2001 Microsoft Corporation

Module Name:

	EffectNameDialog.cpp

Abstract:

	Dialog to handle effect names

Author:

	Robert Heitkamp (robheit) 12-Feb-2002

Revision History:

	12-Feb-2002 robheit
		Initial Version

--*/

//------------------------------------------------------------------------------
//	Includes:
//------------------------------------------------------------------------------
#include <math.h>
#include "resource.h"
#include "EffectNameDialog.h"

//------------------------------------------------------------------------------
//	Message Map
//------------------------------------------------------------------------------
BEGIN_MESSAGE_MAP(CEffectNameDialog,CDialog)
	//{{AFX_MSG_MAP(CEffectNameDialog)
	ON_COMMAND(IDDEFAULT, OnDefault)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

//------------------------------------------------------------------------------
//	CEffectNameDialog::CEffectNameDialog
//------------------------------------------------------------------------------
CEffectNameDialog::CEffectNameDialog(
								     IN LPCTSTR	pEffect,
									 IN LPCTSTR	pName
								     ) : CDialog(IDD_EFFECT_NAME_DIALOG)
/*++

Routine Description:

	Constructor

Arguments:

	IN pEffect -	True name of effect
	IN pName -		User name of effect

Return Value:

	None

--*/
{
	m_effect	= pEffect;
	m_name		= pName;
}

//------------------------------------------------------------------------------
//	CEffectNameDialog::~CEffectNameDialog
//------------------------------------------------------------------------------
CEffectNameDialog::~CEffectNameDialog(void)
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
//	CEffectNameDialog::OnInitDialog
//------------------------------------------------------------------------------
BOOL
CEffectNameDialog::OnInitDialog(void)
/*++

Routine Description:

	Initializes the dialog

Arguments:

	None

Return Value:

	None

--*/
{
	if(CDialog::OnInitDialog())
	{
		SetDlgItemText(IDC_EFFECT_NAME_EDIT, m_name);
		return TRUE;
	}
	return FALSE;
}

//------------------------------------------------------------------------------
//	CEffectNameDialog::OnOK
//------------------------------------------------------------------------------
void
CEffectNameDialog::OnOK(void)
/*++

Routine Description:

	Handles the Ok button

Arguments:

	None

Return Value:

	None

--*/
{
	GetDlgItemText(IDC_EFFECT_NAME_EDIT, m_name); 
	CDialog::OnOK();
}

//------------------------------------------------------------------------------
//	CEffectNameDialog::OnDefault
//------------------------------------------------------------------------------
void
CEffectNameDialog::OnDefault(void)
/*++

Routine Description:

	Resets the knobs to their original (last applied) settings

Arguments:

	None

Return Value:

	None

--*/
{
	m_name = m_effect;
	SetDlgItemText(IDC_EFFECT_NAME_EDIT, m_name);
}
