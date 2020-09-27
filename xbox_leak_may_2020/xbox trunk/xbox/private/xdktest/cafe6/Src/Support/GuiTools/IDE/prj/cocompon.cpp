///////////////////////////////////////////////////////////////////////////////
//  COCOMPON.CPP
//
//  Created by :            Date :
//      MichMa					5/7/97
//
//  Description :
//      Implementation of the COComponent base class and component-specific derivatives.
//
#include "stdafx.h"
#include "cocompon.h"
#include "mstwrap.h"
#include "..\shl\wbutil.h"
#include "guiv1.h"

#define new DEBUG_NEW

#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;


///////////////////////////////////////////////////////////////////////////////
// ActiveXControlContainmentComponent class


// BEGIN_HELP_COMMENT
// Function: ActiveXControlContainmentComponent::ActiveXControlContainmentComponent(void)
// Description: Constructs a ActiveXControlContainmentComponent object.
// Return: none.
// END_HELP_COMMENT
ActiveXControlContainmentComponent::ActiveXControlContainmentComponent(void)
{
	// these settings are constant.
	m_strGalleryName	= GetLocString(IDSS_COMP_ACTX_CONT_GAL_NAME);
	m_strFileName		= GetLocString(IDSS_COMP_ACTX_CONT_FILE_NAME);
	m_strDescription	= GetLocString(IDSS_COMP_ACTX_CONT_DESC);
	m_strWizardDlgTitle = GetLocString(IDSS_COMP_ACTX_CONT_WIZ_TITLE);
}


// BEGIN_HELP_COMMENT
// Function: BOOL ActiveXControlContainmentComponent::WizardFunc(void)
// Description: Manipulates the component's wizard, using values stored by the ActiveXControlContainmentComponent object.
// Return: TRUE if the wizard was successful, FALSE if not.
// END_HELP_COMMENT
BOOL ActiveXControlContainmentComponent::WizardFunc(void)

{
	// wait up to 3 seconds for the wizard dlg to become active.
	if(!MST.WFndWndWait(m_strWizardDlgTitle, FW_PART, 3))
	{
		LOG->RecordInfo("ERROR! ActiveXControlContainmentComponent::WizardFunc - wizard not active");
		return FALSE;
	}

	// finish the wizard.
	MST.DoKeys("{ENTER}");

 	// wait up to 5 seconds for the wizard dlg to go away. 
	// the changes this wizard makes don't take much time.
	if(!MST.WFndWndWait(m_strWizardDlgTitle, FW_NOEXIST | FW_PART, 5))
	{
		LOG->RecordInfo("ERROR! ActiveXControlContainmentComponent::WizardFunc - wizard still active");
		return FALSE;
	}

	return TRUE;
}


///////////////////////////////////////////////////////////////////////////////
// ClipboardAssistantComponent class


// BEGIN_HELP_COMMENT
// Function: ClipboardAssistantComponent::ClipboardAssistantComponent(void)
// Description: Constructs a ClipboardAssistantComponent object.
// Return: none.
// END_HELP_COMMENT
ClipboardAssistantComponent::ClipboardAssistantComponent(void)
{
	// these settings are constant.
	m_strGalleryName	= GetLocString(IDSS_COMP_CLIP_ASST_GAL_NAME);
	m_strFileName		= GetLocString(IDSS_COMP_CLIP_ASST_FILE_NAME);
	m_strDescription	= GetLocString(IDSS_COMP_CLIP_ASST_DESC);
	m_strWizardDlgTitle = GetLocString(IDSS_COMP_CLIP_ASST_WIZ_TITLE);
	// TODO(michma): add support for initializing the wizard options to their defaults.
}


// BEGIN_HELP_COMMENT
// Function: BOOL ClipboardAssistantComponent::WizardFunc(void)
// Description: Manipulates the component's wizard, using values stored by the ClipboardAssistantComponent object.
// Return: TRUE if the wizard was successful, FALSE if not.
// END_HELP_COMMENT
BOOL ClipboardAssistantComponent::WizardFunc(void)

{
	// wait up to 3 seconds for the wizard dlg to become active.
	if(!MST.WFndWndWait(m_strWizardDlgTitle, FW_PART, 3))
	{
		LOG->RecordInfo("ERROR! ClipboardAssistantComponent::WizardFunc - wizard not active");
		return FALSE;
	}

	// continue past the 3 dlgs in the wizard.
	// TODO(michma): add support for changing the wizard options.
	for(int i = 0; i < 3; i++)
		MST.DoKeys("{ENTER}");

 	// wait up to 5 seconds for the wizard dlg to go away. 
	// the changes this wizard makes don't take much time.
	if(!MST.WFndWndWait(m_strWizardDlgTitle, FW_NOEXIST | FW_PART, 5))
	{
		LOG->RecordInfo("ERROR! ClipboardAssistantComponent::WizardFunc - wizard still active");
		return FALSE;
	}
	
	return TRUE;
}


///////////////////////////////////////////////////////////////////////////////
// DialogBarComponent class


// BEGIN_HELP_COMMENT
// Function: DialogBarComponent::DialogBarComponent(void)
// Description: Constructs a DialogBarComponent object.
// Return: none.
// END_HELP_COMMENT
DialogBarComponent::DialogBarComponent(void)
{
	// these settings are constant.
	m_strGalleryName	= GetLocString(IDSS_COMP_DLG_BAR_GAL_NAME);
	m_strFileName		= GetLocString(IDSS_COMP_DLG_BAR_FILE_NAME);
	m_strDescription	= GetLocString(IDSS_COMP_DLG_BAR_DESC);	
	m_strWizardDlgTitle = GetLocString(IDSS_COMP_DLG_BAR_WIZ_TITLE);
	// these are wizard defaults.
	m_strName		= "My Dialog Bar";
	m_strMemberVar	= "m_wndMyDialogBar";
	m_strFrame		= "CMainFrame";
	m_etDocking		= EDGE_TOP;
	m_bVisible		= TRUE;
	m_bDockable		= TRUE;
}


// BEGIN_HELP_COMMENT
// Function: BOOL DialogBarComponent::WizardFunc(void)
// Description: Manipulates the component's wizard, using values stored by the DialogBarComponent object.
// Return: TRUE if the wizard was successful, FALSE if not.
// END_HELP_COMMENT
BOOL DialogBarComponent::WizardFunc(void)

{
	// wait up to 3 seconds for the wizard dlg to become active.
	if(!MST.WFndWndWait(m_strWizardDlgTitle, FW_DEFAULT, 3))
	{
		LOG->RecordInfo("ERROR! DialogBarComponent::WizardFunc - wizard not active");
		return FALSE;
	}

	// set the wizard options.
	MST.WEditSetText("@1", m_strName);
	MST.WEditSetText("@2", m_strMemberVar);
	MST.WComboItemClk("@1", m_strFrame);

	char szDocking[4];
	sprintf(szDocking, "@%d", m_etDocking);
	MST.WOptionSelect(szDocking);
	
	if(m_bVisible)
		MST.WCheckCheck("@1");
	else
		MST.WCheckUnCheck("@1");

	if(m_bDockable)
		MST.WCheckCheck("@2");
	else
		MST.WCheckUnCheck("@2");

	// finish the wizard.
	MST.DoKeys("{ENTER}");

	// wait up to 3 seconds for the wizard dlg to go away.
	if(!MST.WFndWndWait(m_strWizardDlgTitle, FW_NOEXIST, 3))
	{
		LOG->RecordInfo("ERROR! DialogBarComponent::WizardFunc - wizard dlg still active");
		return FALSE;
	}

	// wait up to 15 seconds for the confirmation dlg to become active (title is superset of wizard dlg).
	// during that time, the wizard will be modifying project files.
	if(!MST.WFndWndWait(m_strWizardDlgTitle, FW_PART, 15))
	{
		LOG->RecordInfo("ERROR! DialogBarComponent::WizardFunc - confirmation dlg not active");
		return FALSE;
	}

	// confirm.
	MST.DoKeys("{ENTER}");

	// wait up to 3 seconds for the confirmation dlg to go away.
	if(!MST.WFndWndWait(m_strWizardDlgTitle, FW_PART | FW_NOEXIST, 3))
	{
		LOG->RecordInfo("ERROR! DialogBarComponent::WizardFunc - confirmation still active");
		return FALSE;
	}

	return TRUE;
}


///////////////////////////////////////////////////////////////////////////////
// DocumentRegistrationComponent class


// BEGIN_HELP_COMMENT
// Function: DocumentRegistrationComponent::DocumentRegistrationComponent(void)
// Description: Constructs a DocumentRegistrationComponent object.
// Return: none.
// END_HELP_COMMENT
DocumentRegistrationComponent::DocumentRegistrationComponent(void)
{
	// these settings are constant.
	m_strGalleryName	= GetLocString(IDSS_COMP_DOC_REG_GAL_NAME);
	m_strFileName		= GetLocString(IDSS_COMP_DOC_REG_FILE_NAME);
	m_strDescription	= GetLocString(IDSS_COMP_DOC_REG_DESC);	
	m_strWizardDlgTitle = GetLocString(IDSS_COMP_DOC_REG_WIZ_TITLE);
	// this is the only wizard option. note that the default is blank but it must be filled in for
	// the wizard to complete, so we set a usable default here.
	m_strExtension = "tst";
}


// BEGIN_HELP_COMMENT
// Function: BOOL DocumentRegistrationComponent::WizardFunc(void)
// Description: Manipulates the component's wizard, using values stored by the DocumentRegistrationComponent object.
// Return: TRUE if the wizard was successful, FALSE if not.
// END_HELP_COMMENT
BOOL DocumentRegistrationComponent::WizardFunc(void)

{
	// wait up to 3 seconds for the wizard dlg to become active.
	if(!MST.WFndWndWait(m_strWizardDlgTitle, FW_PART, 3))
	{
		LOG->RecordInfo("ERROR! DocumentRegistrationComponent::WizardFunc - wizard not active");
		return FALSE;
	}

	// set the extension as specified by the user, and finish the wizard.
	MST.DoKeys(m_strExtension);
	MST.DoKeys("{ENTER}");

	// this wizard has two dlgs, both with the same title. so to keep track of where we are we need
	// to look for a button. the first dlg has a "cancel" and "ok" button, the second one just has "ok".
	// wait for the wizard dlg to go away.
	while(MST.WButtonExists(GetLocString(IDSS_CANCEL)));

	// wait up to 15 seconds for the confirmation dlg to become active.
	// some files are munged during this time.
	if(!MST.WFndWndWait(m_strWizardDlgTitle, FW_PART, 15))
	{
		LOG->RecordInfo("ERROR! DocumentRegistrationComponent::WizardFunc - confirmation dlg not active");
		return FALSE;
	}

	// continue past the confirmation dlg.
	MST.DoKeys("{ENTER}");
	
	// wait up to 3 seconds for cancellation dlg to go away.
	if(!MST.WFndWndWait(m_strWizardDlgTitle, FW_NOEXIST | FW_PART, 3))
	{
		LOG->RecordInfo("ERROR! DocumentRegistrationComponent::WizardFunc - confirmation dlg still active");
		return FALSE;
	}

	return TRUE;
}


///////////////////////////////////////////////////////////////////////////////
// IdleTimeProcessingComponent class


// BEGIN_HELP_COMMENT
// Function: IdleTimeProcessingComponent::IdleTimeProcessingComponent(void)
// Description: Constructs a IdleTimeProcessingComponent object.
// Return: none.
// END_HELP_COMMENT
IdleTimeProcessingComponent::IdleTimeProcessingComponent(void)
{
	// these settings are constant.
	m_strGalleryName	= GetLocString(IDSS_COMP_IDLE_TIME_PROC_GAL_NAME);
	m_strFileName		= GetLocString(IDSS_COMP_IDLE_TIME_PROC_FILE_NAME);
	m_strDescription	= GetLocString(IDSS_COMP_IDLE_TIME_PROC_DESC);	
	m_strWizardDlgTitle = GetLocString(IDSS_COMP_IDLE_TIME_PROC_WIZ_TITLE);
}


// BEGIN_HELP_COMMENT
// Function: BOOL IdleTimeProcessingComponent::WizardFunc(void)
// Description: Manipulates the component's wizard, using values stored by the IdleTimeProcessingComponent object.
// Return: TRUE if the wizard was successful, FALSE if not.
// END_HELP_COMMENT
BOOL IdleTimeProcessingComponent::WizardFunc(void)

{
	// wait up to 3 seconds for the wizard dlg to become active.
	if(!MST.WFndWndWait(m_strWizardDlgTitle, FW_PART, 3))
	{
		LOG->RecordInfo("ERROR! IdleTimeProcessingComponent::WizardFunc - wizard not active");
		return FALSE;
	}

	// finish the wizard.
	MST.DoKeys("{ENTER}");

	// wait up to 3 seconds for the wizard dlg to go away (no need to wait for file munging, etc.)
	if(!MST.WFndWndWait(m_strWizardDlgTitle, FW_NOEXIST | FW_PART, 3))
	{
		LOG->RecordInfo("ERROR! IdleTimeProcessingComponent::WizardFunc - wizard still active");
		return FALSE;
	}

	return TRUE;
}


///////////////////////////////////////////////////////////////////////////////
// MAPIComponent class


// BEGIN_HELP_COMMENT
// Function: MAPIComponent::MAPIComponent(void)
// Description: Constructs a MAPIComponent object.
// Return: none.
// END_HELP_COMMENT
MAPIComponent::MAPIComponent(void)
{
	// these settings are constant.
	m_strGalleryName	= GetLocString(IDSS_COMP_MAPI_GAL_NAME);
	m_strFileName		= GetLocString(IDSS_COMP_MAPI_FILE_NAME);
	m_strDescription	= GetLocString(IDSS_COMP_MAPI_DESC);	
	m_strWizardDlgTitle = GetLocString(IDSS_COMP_MAPI_WIZ_TITLE);
}


// BEGIN_HELP_COMMENT
// Function: BOOL MAPIComponent::WizardFunc(void)
// Description: Manipulates the component's wizard, using values stored by the MAPIComponent object.
// Return: TRUE if the wizard was successful, FALSE if not.
// END_HELP_COMMENT
BOOL MAPIComponent::WizardFunc(void)

{
	// wait up to 3 seconds for the wizard dlg to become active.
	if(!MST.WFndWndWait(m_strWizardDlgTitle, FW_DEFAULT, 3))
	{
		LOG->RecordInfo("ERROR! MAPIComponent::WizardFunc - wizard not active");
		return FALSE;
	}

	// finish the wizard.
	MST.DoKeys("{ENTER}");

	// wait up to 3 seconds for the wizard dlg to go away.
	if(!MST.WFndWndWait(m_strWizardDlgTitle, FW_NOEXIST, 3))
	{
		LOG->RecordInfo("ERROR! MAPIComponent::WizardFunc - wizard dlg still active");
		return FALSE;
	}

	// wait up to 3 seconds for the confirmation dlg to become active (title is superset of wizard dlg).
	if(!MST.WFndWndWait(m_strWizardDlgTitle, FW_PART, 15))
	{
		LOG->RecordInfo("ERROR! MAPIComponent::WizardFunc - confirmation dlg not active");
		return FALSE;
	}

	// confirm.
	MST.DoKeys("{ENTER}");

	// wait up to 3 seconds for the confirmation dlg to go away.
	if(!MST.WFndWndWait(m_strWizardDlgTitle, FW_PART | FW_NOEXIST, 3))
	{
		LOG->RecordInfo("ERROR! MAPIComponent::WizardFunc - confirmation still active");
		return FALSE;
	}

	return TRUE;
}


///////////////////////////////////////////////////////////////////////////////
// OwnerDrawControlsComponent class


// BEGIN_HELP_COMMENT
// Function: OwnerDrawControlsComponent::OwnerDrawControlsComponent(void)
// Description: Constructs a OwnerDrawControlsComponent object.
// Return: none.
// END_HELP_COMMENT
OwnerDrawControlsComponent::OwnerDrawControlsComponent(void)
{
	// these settings are constant.
	m_strGalleryName	= GetLocString(IDSS_COMP_OWNER_DRAW_CTRLS_GAL_NAME);
	m_strFileName		= GetLocString(IDSS_COMP_OWNER_DRAW_CTRLS_FILE_NAME);
	m_strDescription	= GetLocString(IDSS_COMP_OWNER_DRAW_CTRLS_DESC);	
	m_strWizardDlgTitle = GetLocString(IDSS_COMP_OWNER_DRAW_CTRLS_WIZ_TITLE);
}


// BEGIN_HELP_COMMENT
// Function: BOOL OwnerDrawControlsComponent::WizardFunc(void)
// Description: Manipulates the component's wizard, using values stored by the OwnerDrawControlsComponent object.
// Return: TRUE if the wizard was successful, FALSE if not.
// END_HELP_COMMENT
BOOL OwnerDrawControlsComponent::WizardFunc(void)

{
	// wait up to 3 seconds for the wizard dlg to become active.
	if(!MST.WFndWndWait(m_strWizardDlgTitle, FW_PART, 3))
	{
		LOG->RecordInfo("ERROR! OwnerDrawControlsComponent::WizardFunc - wizard not active");
		return FALSE;
	}

	// finish the wizard.
	MST.DoKeys("{ENTER}");

	// wait up to 3 seconds for the wizard dlg to go away.
	if(!MST.WFndWndWait(m_strWizardDlgTitle, FW_NOEXIST | FW_PART, 3))
	{
		LOG->RecordInfo("ERROR! OwnerDrawControlsComponent::WizardFunc - wizard still active");
		return FALSE;
	}

	return TRUE;
}


///////////////////////////////////////////////////////////////////////////////
// PaletteComponent class


// BEGIN_HELP_COMMENT
// Function: PaletteComponent::PaletteComponent(void)
// Description: Constructs a PaletteComponent object.
// Return: none.
// END_HELP_COMMENT
PaletteComponent::PaletteComponent(void)
{
	// these settings are constant.
	m_strGalleryName	= GetLocString(IDSS_COMP_PALETTE_GAL_NAME);
	m_strFileName		= GetLocString(IDSS_COMP_PALETTE_FILE_NAME);
	m_strDescription	= GetLocString(IDSS_COMP_PALETTE_DESC);	
	m_strWizardDlgTitle = GetLocString(IDSS_COMP_PALETTE_WIZ_TITLE);
}


// BEGIN_HELP_COMMENT
// Function: BOOL PaletteComponent::WizardFunc(void)
// Description: Manipulates the component's wizard, using values stored by the PaletteComponent object.
// Return: TRUE if the wizard was successful, FALSE if not.
// END_HELP_COMMENT
BOOL PaletteComponent::WizardFunc(void)

{
	// wait up to 3 seconds for the wizard dlg to become active.
	if(!MST.WFndWndWait(m_strWizardDlgTitle, FW_PART, 3))
	{
		LOG->RecordInfo("ERROR! PaletteComponent::WizardFunc - wizard not active");
		return FALSE;
	}

	// finish the wizard.
	MST.DoKeys("{ENTER}");

	// wait up to 3 seconds for the wizard dlg to go away.
	if(!MST.WFndWndWait(m_strWizardDlgTitle, FW_NOEXIST | FW_PART, 3))
	{
		LOG->RecordInfo("ERROR! PaletteComponent::WizardFunc - wizard still active");
		return FALSE;
	}

	return TRUE;
}


///////////////////////////////////////////////////////////////////////////////
// PopUpMenuComponent class


// BEGIN_HELP_COMMENT
// Function: PopUpMenuComponent::PopUpMenuComponent(void)
// Description: Constructs a PopUpMenuComponent object.
// Return: none.
// END_HELP_COMMENT
PopUpMenuComponent::PopUpMenuComponent(void)
{
	// these settings are constant.
	m_strGalleryName	= GetLocString(IDSS_COMP_POPUP_MENU_GAL_NAME);
	m_strFileName		= GetLocString(IDSS_COMP_POPUP_MENU_FILE_NAME);
	m_strDescription	= GetLocString(IDSS_COMP_POPUP_MENU_DESC);	
	m_strWizardDlgTitle = GetLocString(IDSS_COMP_POPUP_MENU_WIZ_TITLE);
}


// BEGIN_HELP_COMMENT
// Function: BOOL PopUpMenuComponent::WizardFunc(void)
// Description: Manipulates the component's wizard, using values stored by the PopUpMenuComponent object.
// Return: TRUE if the wizard was successful, FALSE if not.
// END_HELP_COMMENT
BOOL PopUpMenuComponent::WizardFunc(void)

{
	// wait up to 3 seconds for the wizard dlg to become active.
	if(!MST.WFndWndWait(m_strWizardDlgTitle, FW_PART, 3))
	{
		LOG->RecordInfo("ERROR! PopUpMenuComponent::WizardFunc - wizard not active");
		return FALSE;
	}

	// finish the wizard.
	MST.DoKeys("{ENTER}");

	// wait up to 3 seconds for the wizard dlg to go away.
	if(!MST.WFndWndWait(m_strWizardDlgTitle, FW_NOEXIST | FW_PART, 3))
	{
		LOG->RecordInfo("ERROR! PopUpMenuComponent::WizardFunc - wizard still active");
		return FALSE;
	}

	return TRUE;
}


///////////////////////////////////////////////////////////////////////////////
// ProgressDialogComponent class


// BEGIN_HELP_COMMENT
// Function: ProgressDialogComponent::ProgressDialogComponent(void)
// Description: Constructs a ProgressDialogComponent object.
// Return: none.
// END_HELP_COMMENT
ProgressDialogComponent::ProgressDialogComponent(void)
{
	// these settings are constant.
	m_strGalleryName	= GetLocString(IDSS_COMP_PROGRESS_DLG_GAL_NAME);
	m_strFileName		= GetLocString(IDSS_COMP_PROGRESS_DLG_FILE_NAME);
	m_strDescription	= GetLocString(IDSS_COMP_PROGRESS_DLG_DESC);
	m_strWizardDlgTitle = GetLocString(IDSS_COMP_PROGRESS_DLG_WIZ_TITLE);
}


// BEGIN_HELP_COMMENT
// Function: BOOL ProgressDialogComponent::WizardFunc(void)
// Description: Manipulates the component's wizard, using values stored by the ProgressDialogComponent object.
// Return: TRUE if the wizard was successful, FALSE if not.
// END_HELP_COMMENT
BOOL ProgressDialogComponent::WizardFunc(void)

{
	// wait up to 3 seconds for the wizard dlg to become active.
	if(!MST.WFndWndWait(m_strWizardDlgTitle, FW_PART, 3))
	{
		LOG->RecordInfo("ERROR! ProgressDialogComponent::WizardFunc - wizard not active");
		return FALSE;
	}

	// finish the wizard.
	MST.DoKeys("{ENTER}");

	// wait up to 3 seconds for the wizard dlg to go away.
	if(!MST.WFndWndWait(m_strWizardDlgTitle, FW_NOEXIST | FW_PART, 3))
	{
		LOG->RecordInfo("ERROR! ProgressDialogComponent::WizardFunc - wizard still active");
		return FALSE;
	}

	return TRUE;
}


///////////////////////////////////////////////////////////////////////////////
// SplashScreenComponent class


// BEGIN_HELP_COMMENT
// Function: SplashScreenComponent::SplashScreenComponent(void)
// Description: Constructs a SplashScreenComponent object.
// Return: none.
// END_HELP_COMMENT
SplashScreenComponent::SplashScreenComponent(void)
{
	// these settings are constant.
	m_strGalleryName	= GetLocString(IDSS_COMP_SPLASH_SCREEN_GAL_NAME);
	m_strFileName		= GetLocString(IDSS_COMP_SPLASH_SCREEN_FILE_NAME);
	m_strDescription	= GetLocString(IDSS_COMP_SPLASH_SCREEN_DESC);
	m_strWizardDlgTitle = GetLocString(IDSS_COMP_SPLASH_SCREEN_WIZ_TITLE);
}


// BEGIN_HELP_COMMENT
// Function: BOOL SplashScreenComponent::WizardFunc(void)
// Description: Manipulates the component's wizard, using values stored by the SplashScreenComponent object.
// Return: TRUE if the wizard was successful, FALSE if not.
// END_HELP_COMMENT
BOOL SplashScreenComponent::WizardFunc(void)

{
	// wait up to 3 seconds for the wizard dlg to become active.
	if(!MST.WFndWndWait(m_strWizardDlgTitle, FW_PART, 3))
	{
		LOG->RecordInfo("ERROR! SplashScreenComponent::WizardFunc - wizard not active");
		return FALSE;
	}

	// finish the wizard.
	MST.DoKeys("{ENTER}");

	// wait up to 3 seconds for the wizard dlg to go away.
	if(!MST.WFndWndWait(m_strWizardDlgTitle, FW_NOEXIST | FW_PART, 3))
	{
		LOG->RecordInfo("ERROR! SplashScreenComponent::WizardFunc - wizard still active");
		return FALSE;
	}

	return TRUE;
}


///////////////////////////////////////////////////////////////////////////////
// SplitBarsComponent class


// BEGIN_HELP_COMMENT
// Function: SplitBarsComponent::SplitBarsComponent(void)
// Description: Constructs a SplitBarsComponent object.
// Return: none.
// END_HELP_COMMENT
SplitBarsComponent::SplitBarsComponent(void)
{
	// these settings are constant.
	m_strGalleryName	= GetLocString(IDSS_COMP_SPLIT_BARS_GAL_NAME);
	m_strFileName		= GetLocString(IDSS_COMP_SPLIT_BARS_FILE_NAME);
	m_strDescription	= GetLocString(IDSS_COMP_SPLIT_BARS_DESC);
	m_strWizardDlgTitle = GetLocString(IDSS_COMP_SPLIT_BARS_WIZ_TITLE);
}


// BEGIN_HELP_COMMENT
// Function: BOOL SplitBarsComponent::WizardFunc(void)
// Description: Manipulates the component's wizard, using values stored by the SplitBarsComponent object.
// Return: TRUE if the wizard was successful, FALSE if not.
// END_HELP_COMMENT
BOOL SplitBarsComponent::WizardFunc(void)

{
	// wait up to 3 seconds for the wizard dlg to become active.
	if(!MST.WFndWndWait(m_strWizardDlgTitle, FW_PART, 3))
	{
		LOG->RecordInfo("ERROR! SplitBarsComponent::WizardFunc - wizard not active");
		return FALSE;
	}

	// finish the wizard.
	MST.DoKeys("{ENTER}");

	// wait up to 3 seconds for the wizard dlg to go away.
	if(!MST.WFndWndWait(m_strWizardDlgTitle, FW_NOEXIST | FW_PART, 3))
	{
		LOG->RecordInfo("ERROR! SplitBarsComponent::WizardFunc - wizard still active");
		return FALSE;
	}

	return TRUE;
}


///////////////////////////////////////////////////////////////////////////////
// StatusBarComponent class


// BEGIN_HELP_COMMENT
// Function: StatusBarComponent::StatusBarComponent(void)
// Description: Constructs a StatusBarComponent object.
// Return: none.
// END_HELP_COMMENT
StatusBarComponent::StatusBarComponent(void)
{
	// these settings are constant.
	m_strGalleryName	= GetLocString(IDSS_COMP_STATUS_BAR_GAL_NAME);
	m_strFileName		= GetLocString(IDSS_COMP_STATUS_BAR_FILE_NAME);
	m_strDescription	= GetLocString(IDSS_COMP_STATUS_BAR_DESC);
	m_strWizardDlgTitle = GetLocString(IDSS_COMP_STATUS_BAR_WIZ_TITLE);
	// these are wizard options.
	// note that we must tell the wizard to display either the date or time for the wizard to complete.
	m_sdtDate = STATUS_DISPLAY_DEFAULT;
	m_sdtTime = STATUS_DISPLAY_DEFAULT;
}


// BEGIN_HELP_COMMENT
// Function: BOOL StatusBarComponent::WizardFunc(void)
// Description: Manipulates the component's wizard, using values stored by the StatusBarComponent object.
// Return: TRUE if the wizard was successful, FALSE if not.
// END_HELP_COMMENT
BOOL StatusBarComponent::WizardFunc(void)

{
	// wait up to 3 seconds for the wizard dlg to become active.
	if(!MST.WFndWndWait(m_strWizardDlgTitle, FW_PART, 3))
	{
		LOG->RecordInfo("ERROR! StatusBarComponent::WizardFunc - wizard not active");
		return FALSE;
	}

	// set the date display option.
	char szDisplay[4];
	sprintf(szDisplay, "@%d", m_sdtDate);
	MST.WOptionSelect(szDisplay);

	// advance to the next page of the wizard.
	MST.DoKeys("{ENTER}");

	// we know the next page is up when a certain option button appears.
	if(!MST.WFndWndWaitC(GetLocString(IDSS_DONT_DISPLAY_TIME), "Button", FW_DEFAULT, 3))
	{
		LOG->RecordInfo("ERROR! StatusBarComponent::WizardFunc - 2nd page of wizard not active");
		return FALSE;
	}

	// set the time display option.
	sprintf(szDisplay, "@%d", m_sdtTime);
	MST.WOptionSelect(szDisplay);

	// finish the wizard.
	MST.DoKeys("{ENTER}");
	MST.DoKeys("{ENTER}");

	// wait up to 3 seconds for the wizard dlg to go away.
	if(!MST.WFndWndWait(m_strWizardDlgTitle, FW_NOEXIST | FW_PART, 3))
	{
		LOG->RecordInfo("ERROR! StatusBarComponent::WizardFunc - wizard still active");
		return FALSE;
	}

	return TRUE;
}


///////////////////////////////////////////////////////////////////////////////
// SystemInfoComponent class


// BEGIN_HELP_COMMENT
// Function: SystemInfoComponent::SystemInfoComponent(void)
// Description: Constructs a SystemInfoComponent object.
// Return: none.
// END_HELP_COMMENT
SystemInfoComponent::SystemInfoComponent(void)
{
	// these settings are constant.
	m_strGalleryName	= GetLocString(IDSS_COMP_SYS_INFO_GAL_NAME);
	m_strFileName		= GetLocString(IDSS_COMP_SYS_INFO_FILE_NAME);
	m_strDescription	= GetLocString(IDSS_COMP_SYS_INFO_DESC);
	m_strWizardDlgTitle = GetLocString(IDSS_COMP_SYS_INFO_WIZ_TITLE);
}


// BEGIN_HELP_COMMENT
// Function: BOOL SystemInfoComponent::WizardFunc(void)
// Description: Manipulates the component's wizard, using values stored by the SystemInfoComponent object.
// Return: TRUE if the wizard was successful, FALSE if not.
// END_HELP_COMMENT
BOOL SystemInfoComponent::WizardFunc(void)

{
	// wait up to 3 seconds for the wizard dlg to become active.
	if(!MST.WFndWndWait(m_strWizardDlgTitle, FW_PART, 3))
	{
		LOG->RecordInfo("ERROR! SystemInfoComponent::WizardFunc - wizard not active");
		return FALSE;
	}

	// finish the wizard.
	MST.DoKeys("{ENTER}");

	// wait up to 3 seconds for the wizard dlg to go away.
	if(!MST.WFndWndWait(m_strWizardDlgTitle, FW_NOEXIST | FW_PART, 3))
	{
		LOG->RecordInfo("ERROR! SystemInfoComponent::WizardFunc - wizard still active");
		return FALSE;
	}

	return TRUE;
}


///////////////////////////////////////////////////////////////////////////////
// TipOfTheDayComponent class


// BEGIN_HELP_COMMENT
// Function: TipOfTheDayComponent::TipOfTheDayComponent(void)
// Description: Constructs a TipOfTheDayComponent object.
// Return: none.
// END_HELP_COMMENT
TipOfTheDayComponent::TipOfTheDayComponent(void)
{
	// these settings are constant.
	m_strGalleryName	= GetLocString(IDSS_COMP_TIP_OF_DAY_GAL_NAME);
	m_strFileName		= GetLocString(IDSS_COMP_TIP_OF_DAY_FILE_NAME);
	m_strDescription	= GetLocString(IDSS_COMP_TIP_OF_DAY_DESC);
	m_strWizardDlgTitle = GetLocString(IDSS_COMP_TIP_OF_DAY_WIZ_TITLE);
}


// BEGIN_HELP_COMMENT
// Function: BOOL TipOfTheDayComponent::WizardFunc(void)
// Description: Manipulates the component's wizard, using values stored by the TipOfTheDayComponent object.
// Return: TRUE if the wizard was successful, FALSE if not.
// END_HELP_COMMENT
BOOL TipOfTheDayComponent::WizardFunc(void)

{
	// wait up to 3 seconds for the wizard dlg to become active.
	if(!MST.WFndWndWait(m_strWizardDlgTitle, FW_PART, 3))
	{
		LOG->RecordInfo("ERROR! TipOfTheDayComponent::WizardFunc - wizard not active");
		return FALSE;
	}

	// finish the wizard.
	MST.DoKeys("{ENTER}");

	// wait up to 3 seconds for the wizard dlg to go away.
	if(!MST.WFndWndWait(m_strWizardDlgTitle, FW_NOEXIST | FW_PART, 3))
	{
		LOG->RecordInfo("ERROR! TipOfTheDayComponent::WizardFunc - wizard still active");
		return FALSE;
	}

	return TRUE;
}


///////////////////////////////////////////////////////////////////////////////
// ToolTipsComponent class


// BEGIN_HELP_COMMENT
// Function: ToolTipsComponent::ToolTipsComponent(void)
// Description: Constructs a ToolTipsComponent object.
// Return: none.
// END_HELP_COMMENT
ToolTipsComponent::ToolTipsComponent(void)
{
	// these settings are constant.
	m_strGalleryName	= GetLocString(IDSS_COMP_TOOLTIPS_GAL_NAME);
	m_strFileName		= GetLocString(IDSS_COMP_TOOLTIPS_FILE_NAME);
	m_strDescription	= GetLocString(IDSS_COMP_TOOLTIPS_DESC);
	m_strWizardDlgTitle = GetLocString(IDSS_COMP_TOOLTIPS_WIZ_TITLE);
}


// BEGIN_HELP_COMMENT
// Function: BOOL ToolTipsComponent::WizardFunc(void)
// Description: Manipulates the component's wizard, using values stored by the ToolTipsComponent object.
// Return: TRUE if the wizard was successful, FALSE if not.
// END_HELP_COMMENT
BOOL ToolTipsComponent::WizardFunc(void)

{
	// wait up to 3 seconds for the wizard dlg to become active.
	if(!MST.WFndWndWait(m_strWizardDlgTitle, FW_PART, 3))
	{
		LOG->RecordInfo("ERROR! ToolTipsComponent::WizardFunc - wizard not active");
		return FALSE;
	}

	// finish the wizard.
	MST.DoKeys("{ENTER}");

	// wait up to 3 seconds for the wizard dlg to go away.
	if(!MST.WFndWndWait(m_strWizardDlgTitle, FW_NOEXIST | FW_PART, 3))
	{
		LOG->RecordInfo("ERROR! ToolTipsComponent::WizardFunc - wizard still active");
		return FALSE;
	}

	return TRUE;
}


///////////////////////////////////////////////////////////////////////////////
// WindowsMultiMediaComponent class


// BEGIN_HELP_COMMENT
// Function: WindowsMultiMediaComponent::WindowsMultiMediaComponent(void)
// Description: Constructs a WindowsMultiMediaComponent object.
// Return: none.
// END_HELP_COMMENT
WindowsMultiMediaComponent::WindowsMultiMediaComponent(void)
{
	// these settings are constant.
	m_strGalleryName	= GetLocString(IDSS_COMP_WIN_MM_GAL_NAME);
	m_strFileName		= GetLocString(IDSS_COMP_WIN_MM_FILE_NAME);
	m_strDescription	= GetLocString(IDSS_COMP_WIN_MM_DESC);
	m_strWizardDlgTitle = GetLocString(IDSS_COMP_WIN_MM_WIZ_TITLE);
}


// BEGIN_HELP_COMMENT
// Function: BOOL WindowsMultiMediaComponent::WizardFunc(void)
// Description: Manipulates the component's wizard, using values stored by the WindowsMultiMediaComponent object.
// Return: TRUE if the wizard was successful, FALSE if not.
// END_HELP_COMMENT
BOOL WindowsMultiMediaComponent::WizardFunc(void)

{
	// wait up to 3 seconds for the wizard dlg to become active.
	if(!MST.WFndWndWait(m_strWizardDlgTitle, FW_PART, 3))
	{
		LOG->RecordInfo("ERROR! WindowsMultiMediaComponent::WizardFunc - wizard not active");
		return FALSE;
	}

	// finish the wizard.
	MST.DoKeys("{ENTER}");

	// wait up to 3 seconds for the wizard dlg to go away.
	if(!MST.WFndWndWait(m_strWizardDlgTitle, FW_NOEXIST | FW_PART, 3))
	{
		LOG->RecordInfo("ERROR! WindowsMultiMediaComponent::WizardFunc - wizard still active");
		return FALSE;
	}

	return TRUE;
}


///////////////////////////////////////////////////////////////////////////////
// WindowsSocketsComponent class


// BEGIN_HELP_COMMENT
// Function: WindowsSocketsComponent::WindowsSocketsComponent(void)
// Description: Constructs a WindowsSocketsComponent object.
// Return: none.
// END_HELP_COMMENT
WindowsSocketsComponent::WindowsSocketsComponent(void)
{
	// these settings are constant.
	m_strGalleryName	= GetLocString(IDSS_COMP_WIN_SOCKETS_GAL_NAME);
	m_strFileName		= GetLocString(IDSS_COMP_WIN_SOCKETS_FILE_NAME);
	m_strDescription	= GetLocString(IDSS_COMP_WIN_SOCKETS_DESC);
	m_strWizardDlgTitle = GetLocString(IDSS_COMP_WIN_SOCKETS_WIZ_TITLE);
}


// BEGIN_HELP_COMMENT
// Function: BOOL WindowsSocketsComponent::WizardFunc(void)
// Description: Manipulates the component's wizard, using values stored by the WindowsSocketsComponent object.
// Return: TRUE if the wizard was successful, FALSE if not.
// END_HELP_COMMENT
BOOL WindowsSocketsComponent::WizardFunc(void)

{
	// wait up to 3 seconds for the wizard dlg to become active.
	if(!MST.WFndWndWait(m_strWizardDlgTitle, FW_PART, 3))
	{
		LOG->RecordInfo("ERROR! WindowsSocketsComponent::WizardFunc - wizard not active");
		return FALSE;
	}

	// finish the wizard.
	MST.DoKeys("{ENTER}");

	// wait up to 3 seconds for the wizard dlg to go away.
	if(!MST.WFndWndWait(m_strWizardDlgTitle, FW_NOEXIST | FW_PART, 3))
	{
		LOG->RecordInfo("ERROR! WindowsSocketsComponent::WizardFunc - wizard still active");
		return FALSE;
	}

	return TRUE;
}



///////////////////////////////////////////////////////////////////////////////
// WindowsSocketsComponent class


// BEGIN_HELP_COMMENT
// Function: CActiveXControl::CActiveXControl(void)
// Description: Constructs a CActiveXControl object.
// Return: none.
// END_HELP_COMMENT
CActiveXControl::CActiveXControl(LPCSTR lpCtrlName, LPCSTR lpCtrlDesc /*=NULL*/)
{
	// these settings are constant.
	m_strGalleryName	= lpCtrlName;
	m_strFileName		= lpCtrlName;
	m_strDescription	= lpCtrlDesc;
	m_strWizardDlgTitle = "Confirm Classes"; //GetLocString(???);
}


// BEGIN_HELP_COMMENT
// Function: BOOL CActiveXControl::WizardFunc(void)
// Description: Manipulates the component's wizard, using values stored by the CActiveXControl object.
// Return: TRUE if the wizard was successful, FALSE if not.
// END_HELP_COMMENT
BOOL CActiveXControl::WizardFunc(void)

{
	// wait up to 3 seconds for the wizard dlg to become active.
	if(!MST.WFndWndWait(m_strWizardDlgTitle, FW_PART, 3))
	{
		LOG->RecordInfo("ERROR! CActiveXControl::WizardFunc - wizard not active");
		return FALSE;
	}

	// finish the wizard.
	MST.DoKeys("{ENTER}");

	// wait up to 3 seconds for the wizard dlg to go away.
	if(!MST.WFndWndWait(m_strWizardDlgTitle, FW_NOEXIST | FW_PART, 3))
	{
		LOG->RecordInfo("ERROR! CActiveXControl::WizardFunc - wizard still active");
		return FALSE;
	}

	return TRUE;
}














