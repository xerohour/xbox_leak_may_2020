/*++

Copyright (c) 2001 Microsoft Corporation

Module Name:

	ParameterDialog.cpp

Abstract:

	Dialog to handle effect parameters

Author:

	Robert Heitkamp (robheit) 30-Nov-2001

Revision History:

	30-Nov-2001 robheit
		Initial Version

--*/

//------------------------------------------------------------------------------
//	Includes:
//------------------------------------------------------------------------------
#include <math.h>
#include "resource.h"
#include "ParameterDialog.h"
#include "Knob.h"
#include "Module.h"

//------------------------------------------------------------------------------
//	Message Map
//------------------------------------------------------------------------------
BEGIN_MESSAGE_MAP(CParameterDialog,CDialog)
	//{{AFX_MSG_MAP(CParameterDialog)
	ON_COMMAND(IDRESET, OnReset)
	ON_COMMAND(IDAPPLY, OnApply)
	ON_COMMAND(IDDEFAULT, OnDefault)
	ON_BN_CLICKED(IDC_REALTIME, OnRealtime)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

//------------------------------------------------------------------------------
//	CParameterDialog::CParameterDialog
//------------------------------------------------------------------------------
CParameterDialog::CParameterDialog(
								   IN CModule*	pModule,
								   IN LPCTSTR	pName,
								   IN int		width
								   ) : CDialog(IDD_PARAMETER_DIALOG)
/*++

Routine Description:

	Constructor

Arguments:

	IN pModule -	Module associated with this
	IN pName -		Name of dialog
	IN width -		Number of parameter knobs per row (max)

Return Value:

	None

--*/
{
	m_pModule			= pModule;
	m_name				= pName;
	m_width				= width;
	m_bRealtimeEnabled	= FALSE;
	m_bRealtime			= FALSE;
}

//------------------------------------------------------------------------------
//	CParameterDialog::~CParameterDialog
//------------------------------------------------------------------------------
CParameterDialog::~CParameterDialog(void)
/*++

Routine Description:

	Destructor

Arguments:

	None

Return Value:

	None

--*/
{
	int	i;
	for(i=0; i<m_knobs.GetSize(); ++i)
		delete m_knobs[i];
}

//------------------------------------------------------------------------------
//	CParameterDialog::AddKnob
//------------------------------------------------------------------------------
void 
CParameterDialog::AddKnob(
						  IN CKnob*	pKnob
						  )
/*++

Routine Description:

	Adds a knob. The memory for this object is managed internally and will
	be destroyed with the dialog exits. This method cannot be called after
	the dialog has been displayed.

Arguments:

	IN pKnob -	Knob to add

Return Value:

	None

--*/
{
	m_knobs.Add(pKnob);
}

//------------------------------------------------------------------------------
//	CParameterDialog::UpdateKnobs
//------------------------------------------------------------------------------
void
CParameterDialog::UpdateKnobs(void)
/*++

Routine Description:

	Updates the dialog for the new knobs

Arguments:

	None

Return Value:

	None

--*/
{
	int		i;
	CWnd*	pWnd;
	CRect	rect;
	CRect	winRect;
	int		x;
	int		y;
	int		rows;
	int		smallWidth;
	int		height		= 0;
	int		width		= 0;

	GetClientRect(winRect);
	
	// Create all the knobs and calculate the width and height
	for(m_bRealtimeEnabled=TRUE, i=0; i<m_knobs.GetSize(); ++i)
	{
		m_knobs[i]->Create(NULL, WS_CHILD | WS_VISIBLE, CRect(0,0,1,1), this);
		m_knobs[i]->GetClientRect(rect);
		if(rect.Width() > width)
			width = rect.Width();
		if(rect.Height() > height)
			height = rect.Height();
		if(!m_knobs[i]->GetAtomic())
			m_bRealtimeEnabled = FALSE;
	}
	
	// Adjust for spacing
	width	+= 10;
	height	+= 20;

	rows = (m_knobs.GetSize() / m_width) + ((m_knobs.GetSize() % m_width) ? 1 : 0);

	// Place the knobs
	for(i=0, y=0; y<rows; ++y)
	{
		for(x=0; x<m_width && i<m_knobs.GetSize(); ++x, ++i)
		{
			m_knobs[i]->GetClientRect(rect);
			m_knobs[i]->MoveWindow(x*width+10 + (((width-10) - rect.Width()) / 2), y*height+10, 
								   rect.Width(), rect.Height());
		}
	}
	if(m_width <= m_knobs.GetSize())
		x = m_width;
	else
		x = m_knobs.GetSize();

	// Adjust the location of the ok/apply/cancel/reset buttons
	pWnd = (CWnd*)GetDlgItem(IDRESET);
	pWnd->GetClientRect(rect);
	smallWidth = rect.Width();
	pWnd->MoveWindow(x*width+20, 20, smallWidth, rect.Height());

	pWnd = (CWnd*)GetDlgItem(IDDEFAULT);
	pWnd->MoveWindow(x*width+20, rect.Height()+25, smallWidth, rect.Height());

	pWnd = (CWnd*)GetDlgItem(IDC_REALTIME);
	pWnd->MoveWindow(x*width+20, 2*rect.Height()+30, smallWidth, rect.Height());
	pWnd->EnableWindow(m_bRealtimeEnabled);
	m_bRealtime = m_bRealtime && m_bRealtimeEnabled;
	
	pWnd = (CWnd*)GetDlgItem(IDOK);
	pWnd->MoveWindow(x*width+smallWidth+35, 20, rect.Width(), rect.Height());

	pWnd = (CWnd*)GetDlgItem(IDAPPLY);
	pWnd->MoveWindow(x*width+smallWidth+35, rect.Height()+25, rect.Width(), rect.Height());

	pWnd = (CWnd*)GetDlgItem(IDCANCEL);
	pWnd->MoveWindow(x*width+smallWidth+35, 2*rect.Height()+30, rect.Width(), rect.Height());

	pWnd->GetClientRect(rect);
	pWnd = (CWnd*)GetDlgItem(IDC_GROUP);
	pWnd->MoveWindow(x*width+smallWidth+27, 5, rect.Width()+17, 3*rect.Height()+35);

	width = (x*width) + 20 + smallWidth + (rect.Width()) + 30;
	if((4*rect.Height()+35) > (rows*height+20))
		height = 4*rect.Height()+35;
	else
		height = rows*height+20;

	// Adjust the window dimensions of this
	GetWindowRect(&winRect);
	ScreenToClient(&winRect);
	GetClientRect(rect);
	width	+= winRect.Width() - rect.Width();
	height	+= winRect.Height() - rect.Height();
	SetWindowPos(NULL, 0, 0, width, height, SWP_NOMOVE | SWP_NOZORDER);
}

//------------------------------------------------------------------------------
//	CParameterDialog::SetRealtime
//------------------------------------------------------------------------------
void 
CParameterDialog::SetRealtime(
							  IN BOOL bFlag
							  ) 
/*++

Routine Description:

	Sets realtime control

Arguments:

	IN bFlag -	Realtime flag

Return Value:

	None

--*/
{ 
	int	i;

	m_bRealtime = bFlag && m_bRealtimeEnabled; 
	CheckDlgButton(IDC_REALTIME, m_bRealtime);
	for(i=0; i<m_knobs.GetSize(); ++i)
		m_knobs[i]->EnableRealtime(m_bRealtime);
}

//------------------------------------------------------------------------------
//	CParameterDialog::OnInitDialog
//------------------------------------------------------------------------------
BOOL
CParameterDialog::OnInitDialog(void)
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
		SetWindowText(m_name);
		return TRUE;
	}
	return FALSE;
}

//------------------------------------------------------------------------------
//	CParameterDialog::OnOK
//------------------------------------------------------------------------------
void
CParameterDialog::OnOK(void)
/*++

Routine Description:

	Handles the Ok button

Arguments:

	None

Return Value:

	None

--*/
{
	OnApply();
	OnCancel();
}

//------------------------------------------------------------------------------
//	CParameterDialog::OnReset
//------------------------------------------------------------------------------
void
CParameterDialog::OnReset(void)
/*++

Routine Description:

	Resets the knobs to their original (last applied) settings

Arguments:

	None

Return Value:

	None

--*/
{
	int	i;
	for(i=0; i<m_knobs.GetSize(); ++i)
		m_knobs[i]->Reset();
}

//------------------------------------------------------------------------------
//	CParameterDialog::OnApply
//------------------------------------------------------------------------------
void
CParameterDialog::OnApply(void)
/*++

Routine Description:

	Applys any changes made

Arguments:

	None

Return Value:

	None

--*/
{
	int		i;

	// Any changes?
	for(i=m_knobs.GetSize()-1; i>=0; --i)
	{
		m_pModule->ChangeParameter(m_knobs[i], FALSE);
		m_knobs[i]->SetLastPos(m_knobs[i]->GetPos());
	}
	m_pModule->ChangeParameter(NULL, TRUE);
}

//------------------------------------------------------------------------------
//	CParameterDialog::OnDefault
//------------------------------------------------------------------------------
void
CParameterDialog::OnDefault(void)
/*++

Routine Description:

	Resets the knobs to their default settings

Arguments:

	None

Return Value:

	None

--*/
{
	int	i;
	for(i=0; i<m_knobs.GetSize(); ++i)
		m_knobs[i]->Default();
}

//------------------------------------------------------------------------------
//	CParameterDialog::OnCancel
//------------------------------------------------------------------------------
void
CParameterDialog::OnCancel(void)
/*++

Routine Description:

	Destroys the window when the cancel button is pressed

Arguments:

	None

Return Value:

	None

--*/
{
	DestroyWindow();
}

//------------------------------------------------------------------------------
//	CParameterDialog::PostNcDestroy
//------------------------------------------------------------------------------
void
CParameterDialog::PostNcDestroy(void)
/*++

Routine Description:

	Handles the destruction of the window

Arguments:

	None

Return Value:

	None

--*/
{
	CDialog::PostNcDestroy();
	m_pModule->ParameterDialogDestroyed();
	delete this;
}

//------------------------------------------------------------------------------
//	CParameterDialog::OnRealtime
//------------------------------------------------------------------------------
void 
CParameterDialog::OnRealtime(void) 
/*++

Routine Description:

	Toggles the realtime feature

Arguments:

	None

Return Value:

	None

--*/
{
	int	i;

	m_bRealtime = !m_bRealtime;
	for(i=0; i<m_knobs.GetSize(); ++i)
		m_knobs[i]->EnableRealtime(m_bRealtime);
	m_pModule->SetRealtime(m_bRealtime);
}

