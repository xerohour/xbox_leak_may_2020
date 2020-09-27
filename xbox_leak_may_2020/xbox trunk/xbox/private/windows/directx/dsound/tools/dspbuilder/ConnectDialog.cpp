/*++

Copyright (c) 2001 Microsoft Corporation

Module Name:

	ConnectDialog.cpp

Abstract:

	Dialog to handle connect to Xbox

Author:

	Robert Heitkamp (robheit) 13-Jun-2001

Revision History:

	13-Jun-2001 robheit
		Initial Version

--*/

//------------------------------------------------------------------------------
//	Includes:
//------------------------------------------------------------------------------
#include "ConnectDialog.h"
#include "resource.h"

//------------------------------------------------------------------------------
//	CConnectDialog::CConnectDialog
//------------------------------------------------------------------------------
CConnectDialog::CConnectDialog(
							   IN LPCTSTR pName
							   ) : 
CDialog(IDD_XBOX_CONNECT)
/*++

Routine Description:

	Constructor

Arguments:

	None

Return Value:

	None

--*/
{
	SetName(pName);
}

//------------------------------------------------------------------------------
//	CConnectDialog::~CConnectDialog
//------------------------------------------------------------------------------
CConnectDialog::~CConnectDialog(void)
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
//	CConnectDialog::OnInitDialog
//------------------------------------------------------------------------------
BOOL
CConnectDialog::OnInitDialog(void)
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
		SetDlgItemText(IDC_XBOX_NAME, m_name);
		return TRUE;
	}
	return FALSE;
}

//------------------------------------------------------------------------------
//	CConnectDialog::OnOK
//------------------------------------------------------------------------------
void
CConnectDialog::OnOK(void)
/*++

Routine Description:

	Handles the Ok button

Arguments:

	None

Return Value:

	None

--*/
{
	GetDlgItemText(IDC_XBOX_NAME, m_name);
	CDialog::OnOK();
}
