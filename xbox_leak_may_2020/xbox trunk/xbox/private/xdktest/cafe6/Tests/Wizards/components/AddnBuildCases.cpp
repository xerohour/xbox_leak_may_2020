///////////////////////////////////////////////////////////////////////////////
//      AddnBuildCases.CPP
//
//      Description :
//              Implementation of CAddnBuildCases

#include "stdafx.h"
#include "AddnBuildCases.h"

#define new DEBUG_NEW

#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;

IMPLEMENT_TEST(CAddnBuildCases, CTest, "AddnBuild", -1, CComponentsSuite)


////////////////////////////////////////////////////////////////////////
// each function creates a different component. 
// add new functions to the end of the list.

COComponent* CreateActiveXControlContainment(void)
{
	return new(ActiveXControlContainmentComponent);
}

COComponent* CreateClipboardAssistant(void)
{
	return new(ClipboardAssistantComponent);
}

COComponent* CreateDialogBar(void)
{
	return new(DialogBarComponent);
}

COComponent* CreateDocumentRegistration(void)
{
	return new(DocumentRegistrationComponent);
}

COComponent* CreateIdleTimeProcessing(void)
{
	return new(IdleTimeProcessingComponent);
}

COComponent* CreateMAPI(void)
{
	return new(MAPIComponent);
}

COComponent* CreateOwnerDrawControls(void)
{
	return new(OwnerDrawControlsComponent);
}

COComponent* CreatePalette(void)
{
	return new(PaletteComponent);
}

COComponent* CreatePopUpMenu(void)
{
	return new(PopUpMenuComponent);
}

COComponent* CreateProgressDialog(void)
{
	return new(ProgressDialogComponent);
}

COComponent* CreateSplashScreen(void)
{
	return new(SplashScreenComponent);
}

COComponent* CreateSplitBars(void)
{
	return new(SplitBarsComponent);
}

COComponent* CreateStatusBar(void)
{
	return new(StatusBarComponent);
}

COComponent* CreateSystemInfo(void)
{
	return new(SystemInfoComponent);
}

COComponent* CreateTipOfTheDay(void)
{
	return new(TipOfTheDayComponent);
}

COComponent* CreateToolTips(void)
{
	return new(ToolTipsComponent);
}

COComponent* CreateWindowsMultiMedia(void)
{
	return new(WindowsMultiMediaComponent);
}

COComponent* CreateWindowsSockets(void)
{
	return new(WindowsSocketsComponent);
}


// each function that inserts a component has this prototype.
typedef COComponent* (*PTR_CREATE_COMPONENT_FUNC)(void); 

// holds the info we need to add each component and log the result.
struct ComponentInfo
{
	LPCSTR						szName;
	PTR_CREATE_COMPONENT_FUNC	pCreateFunc;
};

// we randomly pick out of the following list of components and add them.
// insert information about new components to the end of the list.
ComponentInfo components[] = {
	"ActiveX Control Containment",	CreateActiveXControlContainment,
	"Clipboard Assistant",			CreateClipboardAssistant,
	"Dialog Bar",					CreateDialogBar,
	"Document Registration",		CreateDocumentRegistration,
	"Idle Time Processing",			CreateIdleTimeProcessing,
	"MAPI",							CreateMAPI,
	"Owner Draw Controls",			CreateOwnerDrawControls,
	"Palette",						CreatePalette,
	"Pop-up Menu",					CreatePopUpMenu,
	"Progress Dialog",				CreateProgressDialog,
	"Splash Screen",				CreateSplashScreen,
	"Split Bars",					CreateSplitBars,
	"Status Bar",					CreateStatusBar,
	"System Info",					CreateSystemInfo,
	"Tip of the Day",				CreateTipOfTheDay,
	"ToolTips",						CreateToolTips,
	"Windows MultiMedia",			CreateWindowsMultiMedia,
	"Windows Sockets",				CreateWindowsSockets

/* TODO(michma): still need to support:

1.	ATL Proxy Generator		requires ATL project
2.	Custom Appwizard Tools	requires custom appwizard project
3.	GUID Generator			requires post-addition source editing
4.	Property Sheets			waiting on fix for vc98:2506

#1 and #2 should probably each be in separate testsets.

#3 could be added to this testset, it would just require special actions
(editing source) prior to building.

#4 can be added as soon as the bug is fixed.

*/
};


void CAddnBuildCases::Run(void)
	
{
	// access to the IDE's project system.
	COProject prj;
	// the name of the project to which we will add components.
	CString strProjectName;
	
	// we use a project name specific to the language of the os we are running on.
	switch(GetSystem())
	{
	case SYSTEM_JAPAN:
#pragma warning (disable : 4129)
		strProjectName = "ƒ[ƒ\ƒ]ƒ^ƒ{ƒ|ƒ}ƒAƒa‚‚`Ÿ@Ÿ~Ÿ€Ÿü";
#pragma warning (default : 4129)
		break;
	default:
		strProjectName = "MFC AppWiz Project";
	}
	
	// can't create a new mfc appwiz project over an old one.
	KillAllFiles(strProjectName);
	// create the default appwiz project to which we will add components.
	prj.NewAppWiz(strProjectName, GetCWD(), GetUserTargetPlatforms());
	
	// enable adding of components to the project (this is just a shortcut so we don't have to keep
	// closing and re-opening the same initial windows before adding each component).
	LOG->RecordInfo("");
	LOG->RecordCompare(prj.EnableComponents(), "*** Enabling components. ***");
					   
	// seed the random number generator (components are added in random order).
	srand(time(NULL));
	// we only want to randomly add each component once.
	RandomNumberCheckList rncl(sizeof(components) / (sizeof(PTR_CREATE_COMPONENT_FUNC) + sizeof(LPCSTR)));
	// indexes into the array containing information about the components we are trying to add.
	int i;
	// points to the actual component object that we are trying to add.
	COComponent *pComponent;
	
	// randomly add each component once.
	while((i = rncl.GetNext()) != -1)
	
	{
		// log a standard, spaced, header.
		LOG->RecordInfo("");
		LOG->RecordInfo("*** Adding %s component. ***", components[i].szName);
		// create the next component that we want to add.
		pComponent = (*components[i].pCreateFunc)();

		// add the component to the project.
		LOG->RecordCompare(prj.AddComponent(pComponent), "Adding %s component.", components[i].szName);
		
		// delete the component so we can use the same pointer for the next one.
		delete pComponent;
	}

	// puts ide back into state it was in before prj.EnableComponents was called.
	LOG->RecordInfo("");
	LOG->RecordCompare(prj.DisableComponents(), "*** Disabling components. ***");

	// build the project.
	LOG->RecordInfo("");
	LOG->RecordCompare(prj.Build() == ERROR_SUCCESS, "*** Building. ***");

	// verify no errors or warnings were generated.
	int iErr, iWarn;
	LOG->RecordInfo("");
	LOG->RecordCompare(prj.VerifyBuild(TRUE, &iErr, &iWarn, TRUE) == ERROR_SUCCESS, "*** Verifying build. ***");

	// even though cafe will do this, it's safer to clean up after yourself.
	prj.Close();
}
