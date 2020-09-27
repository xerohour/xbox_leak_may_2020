//---------------------------------------------------------------------------
// Thunder context sensitive help topic areas.  No strings are created but
// we use these constants to share data with User Ed.
//---------------------------------------------------------------------------




// ************* NOTE *****************
//
// The constants for UI stuff are now found in IDS.ID.  From this the file
// IDSID.H is automatically created by the build process, and that file is
// included here.  All ID's get prepended by HID_.
//
//   EXAMPLE:
//
//   IDS.ID
//
//     idsrgn_PropertiesBar 200000 ""
//
// turns into:
//
//   IDSID.H
//
//     #define HID_idsrgn_PropertiesBar 200000
//
// and you should use the HID_vb_idsrgnPropertiesBar constant in your code.
//
// ************* NOTE *****************




// Here is where the real constants are (this file is built from IDS.ID)
#include "IDSID.H"







// This is the VB.HLP table of contents dummy context id
// It's used only internally, so WinHelp should never see it.  It should therefore
//   NOT appear in HELPCTX.TXT.  Therefore it's defined here instead of in ids.id.
#define HELP_TOC_CONTEXT 2008002




// ************* NOTE *****************
// The following is for backwards compatibility only.  
//
// You should *NOT* define new constants here, but simply use the 
// new HID_* versions (add HID_ to the beginning of the name that
// you add to IDS.ID).
// ************************************

//CONSIDER,stephwe,6/96: These should be removed sometime.

#define BACKWARDSCOMPAT(idsrgn_id) idsrgn_id

//#define idsrgnPropertiesBar              BACKWARDSCOMPAT(HID_idsrgn_PropertiesBar)
//#define idsrgnToolbox                    BACKWARDSCOMPAT(HID_idsrgn_Toolbox)
#define idsrgnProjectWin                 BACKWARDSCOMPAT(HID_idsrgnProjectWin)
#define idsrgnFormWin                    BACKWARDSCOMPAT(HID_idsrgnFormWin)
#define idsrgnColorPalette               BACKWARDSCOMPAT(HID_idsrgnColorPalette)
//#define idsrgnCodeWin                    BACKWARDSCOMPAT(HID_idsrgnCodeWin)
//#define idsrgnImmediateWin               BACKWARDSCOMPAT(HID_idsrgnImmediateWin)
#define idsrgnMenuDesignWin              BACKWARDSCOMPAT(HID_idsrgnMenuDesignWin)
#define idsrgnAboutDlgTopic              BACKWARDSCOMPAT(HID_idsrgnAboutDlgTopic)
#define idsrgnPrintDlgTopic              BACKWARDSCOMPAT(HID_idsrgnPrintDlgTopic)
#define idsrgnFileOpenDlgTopic           BACKWARDSCOMPAT(HID_idsrgnFileOpenDlgTopic)
#define idsrgnFileSaveDlgTopic           BACKWARDSCOMPAT(HID_idsrgnFileSaveDlgTopic)
#define idsrgnMakeExeDlgTopic            BACKWARDSCOMPAT(HID_idsrgnMakeExeDlgTopic)
#define idsrgnNewProcDlgTopic            BACKWARDSCOMPAT(HID_idsrgnNewProcDlgTopic)
#define idsrgnSearchDlgTopic             BACKWARDSCOMPAT(HID_idsrgnSearchDlgTopic)
#define idsrgnReplaceDlgTopic            BACKWARDSCOMPAT(HID_idsrgnReplaceDlgTopic)
#define idsrgnFilePrintDlgTopic          BACKWARDSCOMPAT(HID_idsrgnFilePrintDlgTopic)
#define idsrgnAddFileDlgTopic            BACKWARDSCOMPAT(HID_idsrgnAddFileDlgTopic)
#define idsrgnSaveProjDlgTopic           BACKWARDSCOMPAT(HID_idsrgnSaveProjDlgTopic)
#define idsrgnOptionsProjectDlgTopic     BACKWARDSCOMPAT(HID_idsrgnOptionsProjectDlgTopic)
#define idsrgnMainMenu                   BACKWARDSCOMPAT(HID_idsrgnMainMenu)
//#define idsrgnProdSupport                BACKWARDSCOMPAT(HID_idsrgnProdSupport)
#define idsrgnOptionsEnvironmentDlgTopic BACKWARDSCOMPAT(HID_idsrgnOptionsEnvironmentDlgTopic)
#define idsrgnOpenPictureDlgTopic        BACKWARDSCOMPAT(HID_idsrgnOpenPictureDlgTopic)
#define idsrgnOpenIconDlgTopic           BACKWARDSCOMPAT(HID_idsrgnOpenIconDlgTopic)
//#define idsrgnViewProcDlgTopic           BACKWARDSCOMPAT(HID_idsrgnViewProcDlgTopic)
#define idsrgnAddWatchExpDlgTopic        BACKWARDSCOMPAT(HID_idsrgnAddWatchExpDlgTopic)
#define idsrgnEditWatchDlgTopic          BACKWARDSCOMPAT(HID_idsrgnEditWatchDlgTopic)
#define idsrgnInstantWatchDlgTopic       BACKWARDSCOMPAT(HID_idsrgnInstantWatchDlgTopic)
#define idsrgnCallsDlgTopic              BACKWARDSCOMPAT(HID_idsrgnCallsDlgTopic)
#define idsrgnOptionsFormatDlgTopic      BACKWARDSCOMPAT(HID_idsrgnOptionsFormatDlgTopic)
#define idsrgnOptionsEditorDlgTopic      BACKWARDSCOMPAT(HID_idsrgnOptionsEditorDlgTopic)
//#define idsrgnVersionDlgTopic            BACKWARDSCOMPAT(HID_idsrgnVersionDlgTopic)
#define idsrgnAddInMgrDlgTopic           BACKWARDSCOMPAT(HID_idsrgnAddInMgrDlgTopic)
#define idsrgnAddCtrlDlgTopic            BACKWARDSCOMPAT(HID_idsrgnAddCtrlDlgTopic)
#define idsrgnAddRefDlgTopic             BACKWARDSCOMPAT(HID_idsrgnAddRefDlgTopic)
#define idsrgnObjBrwDlgTopic             BACKWARDSCOMPAT(HID_idsrgnObjBrwDlgTopic)
#define idsrgnProcOptionDlgTopic         BACKWARDSCOMPAT(HID_idsrgnProcOptionDlgTopic)
#define idsrgnMorphDlgTopic              BACKWARDSCOMPAT(HID_idsrgnMorphDlgTopic)
#define idsrgnMakeExeDlgOptionsTopic     BACKWARDSCOMPAT(HID_idsrgnMakeExeDlgOptionsTopic)
#define idsrgnToolBox                    BACKWARDSCOMPAT(HID_idsrgnToolBox)
#define idsrgnInsertProcedure            BACKWARDSCOMPAT(HID_idsrgnInsertProcedure)
#define idsrgnInsertFile                 BACKWARDSCOMPAT(HID_idsrgnInsertFile)
#define idsrgnFontObjectDialog           BACKWARDSCOMPAT(HID_idsrgnFontObjectDialog)
#define idsrgnPrintSetupDlgTopic         BACKWARDSCOMPAT(HID_idsrgnPrintSetupDlgTopic)
#define idsrgnOptionsAdvancedDlgTopic    BACKWARDSCOMPAT(HID_idsrgnOptionsAdvancedDlgTopic)
#define idsrgnPropPagesDlgTopic          BACKWARDSCOMPAT(HID_idsrgnPropPagesDlgTopic)
#define idsrgnFileNewDlgTopic		 BACKWARDSCOMPAT(HID_idsrgnFileNewDlgTopic)
//#define idsrgnFileNewOpenDlgTopic        BACKWARDSCOMPAT(HID_idsrgnFileNewOpenDlgTopic)
#define idsrgnDockingViewDlgTopic	 BACKWARDSCOMPAT(HID_idsrgnDockingViewDlgTopic)
#define idsrgnPreviewWindow		 BACKWARDSCOMPAT(HID_idsrgnPreviewWindow)



// MISCELLANEOUS
#define HELP_CUSTOM_PROPERTY		 BACKWARDSCOMPAT(HID_HELP_CUSTOM_PROPERTY)

