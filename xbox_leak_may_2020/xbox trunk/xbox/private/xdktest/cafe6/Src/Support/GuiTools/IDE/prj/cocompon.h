///////////////////////////////////////////////////////////////////////////////
//  COCOMPON.H
//
//  Created by :            Date :
//      MichMa               5/7/97
//															
//  Description :
//      Declaration of the COComponent base class and component-specific derivatives.
//

#ifndef __COCOMPON_H__
#define __COCOMPON_H__

#include "prjxprt.h"


///////////////////////////////////////////////////////////////////////////////
// COComponent class
//
// all component classes should inherit from this base class, adding component-specifc
// wizard options, and overriding WizardFunc to handle the component-specific wizard.
//
// BEGIN_CLASS_HELP
// ClassName: COComponent
// BaseClass: None
// Category: Project
// END_CLASS_HELP
class PRJ_CLASS COComponent

{
public:
	CString m_strGalleryName;		// name used in various gallery dlgs, help
	CString m_strFileName;			// name used in the gallery list and filename field
	CString m_strDescription;		// description displayed in gallery dlg

	virtual BOOL WizardFunc(void) = 0;		// function to handle the component-specific wizard.
};


///////////////////////////////////////////////////////////////////////////////
// ActiveXControlContainmentComponent class
//
// BEGIN_CLASS_HELP
// ClassName: ActiveXControlContainmentComponent
// BaseClass: COComponent
// Category: Project
// END_CLASS_HELP
class PRJ_CLASS ActiveXControlContainmentComponent : public COComponent
{
public:
	// constructor initializes inherited COComponent and other members.
	ActiveXControlContainmentComponent(void);
	// title of the wizard dlg.
	CString m_strWizardDlgTitle;
	// handles the component's wizard.
	virtual BOOL WizardFunc(void);
};


///////////////////////////////////////////////////////////////////////////////
// ClipboardAssistantComponent class
//
// BEGIN_CLASS_HELP
// ClassName: ClipboardAssistantComponent
// BaseClass: COComponent
// Category: Project
// END_CLASS_HELP
class PRJ_CLASS ClipboardAssistantComponent : public COComponent
{
public:
	// constructor initializes inherited COComponent and other members.
	ClipboardAssistantComponent(void);
	// title of the wizard dlg.
	CString m_strWizardDlgTitle;
	// TODO(michma): add member variables to represent the changeable options in the wizard.
	// handles the component's wizard.
	virtual BOOL WizardFunc(void);
};


///////////////////////////////////////////////////////////////////////////////
// DialogBarComponent class
//
// BEGIN_CLASS_HELP
// ClassName: DialogBarComponent
// BaseClass: COComponent
// Category: Project
// END_CLASS_HELP
class PRJ_CLASS DialogBarComponent : public COComponent

{
public:
	
	// for "Default docking edge" option (m_etDocking, below).
	typedef enum {EDGE_TOP = 1, EDGE_BOTTOM, EDGE_LEFT, EDGE_RIGHT} EDGE_TYPE;

	// constructor initializes inherited COComponent and other members.
	DialogBarComponent(void);

	// title of the first (and only) wizard dlg.
	CString m_strWizardDlgTitle;

	// these correspond to options in the Dialog Bar component wizard.
	CString m_strName;
	CString m_strMemberVar;
	CString m_strFrame;
	EDGE_TYPE m_etDocking;
	BOOL m_bVisible;
	BOOL m_bDockable;

	// handles the Dialog Bar component wizard.
	virtual BOOL WizardFunc(void);
};


///////////////////////////////////////////////////////////////////////////////
// DocumentRegistrationComponent class
//
// BEGIN_CLASS_HELP
// ClassName: DocumentRegistrationComponent
// BaseClass: COComponent
// Category: Project
// END_CLASS_HELP
class PRJ_CLASS DocumentRegistrationComponent : public COComponent
{
public:
	// constructor initializes inherited COComponent and other members.
	DocumentRegistrationComponent(void);
	// title of the wizard dlg.
	CString m_strWizardDlgTitle;
	// these correspond to options in the component's wizard.
	CString m_strExtension;
	// handles the component's wizard.
	virtual BOOL WizardFunc(void);
};


///////////////////////////////////////////////////////////////////////////////
// IdleTimeProcessingComponent class
//
// BEGIN_CLASS_HELP
// ClassName: IdleTimeProcessingComponent
// BaseClass: COComponent
// Category: Project
// END_CLASS_HELP
class PRJ_CLASS IdleTimeProcessingComponent : public COComponent
{
public:
	
	// constructor initializes inherited COComponent and other members.
	IdleTimeProcessingComponent(void);
	// title of the wizard dlg.
	CString m_strWizardDlgTitle;
	// these correspond to options in the component's wizard.
	// TODO: add support for setting wizard options to non-defaults.
	// handles the component's wizard.
	virtual BOOL WizardFunc(void);
};


///////////////////////////////////////////////////////////////////////////////
// MAPIComponent class
//
// BEGIN_CLASS_HELP
// ClassName: MAPIComponent
// BaseClass: COComponent
// Category: Project
// END_CLASS_HELP
class PRJ_CLASS MAPIComponent : public COComponent

{
public:
	
	// constructor initializes inherited COComponent and other members.
	MAPIComponent(void);

	// title of the wizard dlg.
	CString m_strWizardDlgTitle;

	// these correspond to options in the component's wizard.
	// TODO: add support for setting wizard options.

	// handles the component's wizard.
	virtual BOOL WizardFunc(void);
};


///////////////////////////////////////////////////////////////////////////////
// OwnerDrawControlsComponent class
//
// BEGIN_CLASS_HELP
// ClassName: OwnerDrawControlsComponent
// BaseClass: COComponent
// Category: Project
// END_CLASS_HELP
class PRJ_CLASS OwnerDrawControlsComponent : public COComponent

{
public:
	
	// constructor initializes inherited COComponent and other members.
	OwnerDrawControlsComponent(void);

	// title of the wizard dlg.
	CString m_strWizardDlgTitle;

	// these correspond to options in the component's wizard.
	// TODO: add support for setting wizard options.
	
	// handles the component's wizard.
	virtual BOOL WizardFunc(void);
};


///////////////////////////////////////////////////////////////////////////////
// PaletteComponent class
//
// BEGIN_CLASS_HELP
// ClassName: PaletteComponent
// BaseClass: COComponent
// Category: Project
// END_CLASS_HELP
class PRJ_CLASS PaletteComponent : public COComponent

{
public:
	
	// constructor initializes inherited COComponent and other members.
	PaletteComponent(void);

	// title of the wizard dlg.
	CString m_strWizardDlgTitle;

	// these correspond to options in the component's wizard.
	// TODO: add support for setting wizard options.

	// handles the component's wizard.
	virtual BOOL WizardFunc(void);
};


///////////////////////////////////////////////////////////////////////////////
// PopUpMenuComponent class
//
// BEGIN_CLASS_HELP
// ClassName: PopUpMenuComponent
// BaseClass: COComponent
// Category: Project
// END_CLASS_HELP
class PRJ_CLASS PopUpMenuComponent : public COComponent

{
public:
	
	// constructor initializes inherited COComponent and other members.
	PopUpMenuComponent(void);

	// title of the wizard dlg.
	CString m_strWizardDlgTitle;

	// these correspond to options in the component's wizard.
	// TODO: add support for setting wizard options.

	// handles the component's wizard.
	virtual BOOL WizardFunc(void);
};


///////////////////////////////////////////////////////////////////////////////
// ProgressDialogComponent class
//
// BEGIN_CLASS_HELP
// ClassName: ProgressDialogComponent
// BaseClass: COComponent
// Category: Project
// END_CLASS_HELP
class PRJ_CLASS ProgressDialogComponent : public COComponent

{
public:
	
	// constructor initializes inherited COComponent and other members.
	ProgressDialogComponent(void);

	// title of the wizard dlg.
	CString m_strWizardDlgTitle;

	// these correspond to options in the component's wizard.
	// TODO: add support for setting wizard options.

	// handles the component's wizard.
	virtual BOOL WizardFunc(void);
};


///////////////////////////////////////////////////////////////////////////////
// SplashScreenComponent class
//
// BEGIN_CLASS_HELP
// ClassName: SplashScreenComponent
// BaseClass: COComponent
// Category: Project
// END_CLASS_HELP
class PRJ_CLASS SplashScreenComponent : public COComponent

{
public:
	
	// constructor initializes inherited COComponent and other members.
	SplashScreenComponent(void);

	// title of the wizard dlg.
	CString m_strWizardDlgTitle;

	// these correspond to options in the component's wizard.
	// TODO: add support for setting wizard options.

	// handles the component's wizard.
	virtual BOOL WizardFunc(void);
};


///////////////////////////////////////////////////////////////////////////////
// SplitBarsComponent class
//
// BEGIN_CLASS_HELP
// ClassName: SplitBarsComponent
// BaseClass: COComponent
// Category: Project
// END_CLASS_HELP
class PRJ_CLASS SplitBarsComponent : public COComponent

{
public:
	
	// constructor initializes inherited COComponent and other members.
	SplitBarsComponent(void);

	// title of the wizard dlg.
	CString m_strWizardDlgTitle;

	// these correspond to options in the component's wizard.
	// TODO: add support for setting wizard options.

	// handles the component's wizard.
	virtual BOOL WizardFunc(void);
};


///////////////////////////////////////////////////////////////////////////////
// StatusBarComponent class
//
// BEGIN_CLASS_HELP
// ClassName: StatusBarComponent
// BaseClass: COComponent
// Category: Project
// END_CLASS_HELP
class PRJ_CLASS StatusBarComponent : public COComponent

{
public:
	
	// for date/time display options in the wizard (sdtDate, sdtTime, below).
	typedef enum {STATUS_DISPLAY_NONE = 1, STATUS_DISPLAY_DEFAULT, STATUS_DISPLAY_CUSTOM} STATUS_DISPLAY_TYPE;

	// constructor initializes inherited COComponent and other members.
	StatusBarComponent(void);

	// title of the wizard dlg.
	CString m_strWizardDlgTitle;

	// these correspond to options in the component's wizard.
	// TODO: add support for the rest of the wizard options.
	STATUS_DISPLAY_TYPE m_sdtDate;
	STATUS_DISPLAY_TYPE m_sdtTime;

	// handles the component's wizard.
	virtual BOOL WizardFunc(void);
};


///////////////////////////////////////////////////////////////////////////////
// SystemInfoComponent class
//
// BEGIN_CLASS_HELP
// ClassName: SystemInfoComponent
// BaseClass: COComponent
// Category: Project
// END_CLASS_HELP
class PRJ_CLASS SystemInfoComponent : public COComponent
{
public:
	// constructor initializes inherited COComponent and other members.
	SystemInfoComponent(void);
	// title of the wizard dlg.
	CString m_strWizardDlgTitle;
	// handles the component's wizard.
	virtual BOOL WizardFunc(void);
};


///////////////////////////////////////////////////////////////////////////////
// TipOfTheDayComponent class
//
// BEGIN_CLASS_HELP
// ClassName: TipOfTheDayComponent
// BaseClass: COComponent
// Category: Project
// END_CLASS_HELP
class PRJ_CLASS TipOfTheDayComponent : public COComponent

{
public:
	
	// constructor initializes inherited COComponent and other members.
	TipOfTheDayComponent(void);

	// title of the wizard dlg.
	CString m_strWizardDlgTitle;

	// these correspond to options in the component's wizard.
	// TODO: add support for setting wizard options.

	// handles the component's wizard.
	virtual BOOL WizardFunc(void);
};


///////////////////////////////////////////////////////////////////////////////
// ToolTipsComponent class
//
// BEGIN_CLASS_HELP
// ClassName: ToolTipsComponent
// BaseClass: COComponent
// Category: Project
// END_CLASS_HELP
class PRJ_CLASS ToolTipsComponent : public COComponent

{
public:
	
	// constructor initializes inherited COComponent and other members.
	ToolTipsComponent(void);

	// title of the wizard dlg.
	CString m_strWizardDlgTitle;

	// these correspond to options in the component's wizard.
	// TODO: add support for setting wizard options.

	// handles the component's wizard.
	virtual BOOL WizardFunc(void);
};


///////////////////////////////////////////////////////////////////////////////
// WindowsMultiMediaComponent class
//
// BEGIN_CLASS_HELP
// ClassName: WindowsMultiMediaComponent
// BaseClass: COComponent
// Category: Project
// END_CLASS_HELP
class PRJ_CLASS WindowsMultiMediaComponent : public COComponent
{
public:
	// constructor initializes inherited COComponent and other members.
	WindowsMultiMediaComponent(void);
	// title of the wizard dlg.
	CString m_strWizardDlgTitle;
	// handles the component's wizard.
	virtual BOOL WizardFunc(void);
};


///////////////////////////////////////////////////////////////////////////////
// WindowsSocketsComponent class
//
// BEGIN_CLASS_HELP
// ClassName: WindowsSocketsComponent
// BaseClass: COComponent
// Category: Project
// END_CLASS_HELP
class PRJ_CLASS WindowsSocketsComponent : public COComponent
{
public:
	// constructor initializes inherited COComponent and other members.
	WindowsSocketsComponent(void);
	// title of the wizard dlg.
	CString m_strWizardDlgTitle;
	// handles the component's wizard.
	virtual BOOL WizardFunc(void);
};


///////////////////////////////////////////////////////////////////////////////
// WindowsSocketsComponent class
//
// BEGIN_CLASS_HELP
// ClassName: CActiveXControl
// BaseClass: COComponent
// Category: Project
// END_CLASS_HELP
class PRJ_CLASS CActiveXControl : public COComponent
{
public:
	// constructor initializes inherited COComponent and other members.
	CActiveXControl(LPCSTR lpCtrlName, LPCSTR lpCtrlDesc=NULL);
	// title of the wizard dlg.
	CString m_strWizardDlgTitle;
	// handles the component's wizard.
	virtual BOOL WizardFunc(void);
};


#endif //__COCOMPON_H__
