#ifndef __SHRDRES_H__
#define SHRDRES___SHRDRES_H__

/////////////////////////////////////////////////////////////////////////////
// ID ranges assigned to packages for resources and string IDs
//

//  Sample Package
#define SHRDRES_MIN_SAMPLE_RESOURCE_ID	0
#define SHRDRES_MAX_SAMPLE_RESOURCE_ID	499

//
// *External Packages
//
//	Espresso
#define SHRDRES_MIN_ESPRESSO_RESOURCE_ID 12000
#define SHRDRES_MAX_ESPRESSO_RESOURCE_ID 12999
//  Test
#define SHRDRES_MIN_TEST_RESOURCE_ID	13000
#define SHRDRES_MAX_TEST_RESOURCE_ID	13999
//  Fortran
#define SHRDRES_MIN_FORTRAN_RESOURCE_ID	14000
#define SHRDRES_MAX_FORTRAN_RESOURCE_ID	14999

//
// Internal Packages
//
//  Build package
#define SHRDRES_MIN_BUILD_RESOURCE_ID	16000
#define SHRDRES_MAX_BUILD_RESOURCE_ID	16999
// App wizard
#define SHRDRES_MIN_APP_WIZARD_ID		17000
#define SHRDRES_MAX_APP_WIZARD_ID		17499
// Class wizard
#define SHRDRES_MIN_CLASS_WIZARD_ID		17500
#define SHRDRES_MAX_CLASS_WIZARD_ID		17999

//  Shell
#define SHRDRES_MIN_SHARED_RESOURCE_ID	20000
#define SHRDRES_MAX_SHARED_RESOURCE_ID	20199
#define SHRDRES_MIN_EXE_RESOURCE_ID		20200
#define SHRDRES_MAX_EXE_RESOURCE_ID		20299
#define SHRDRES_MIN_SHELL_RESOURCE_ID	20300
#define SHRDRES_MAX_SHELL_RESOURCE_ID	21999
//  Object Gallery
#define SHRDRES_MIN_GALLERY_RESOURCE_ID	22000
#define SHRDRES_MAX_GALLERY_RESOURCE_ID	22499

//  VBA
#define SHRDRES_MIN_VBA_RESOURCE_ID 	23000
#define SHRDRES_MAX_VBA_RESOURCE_ID 	23999
//  Source
#define SHRDRES_MIN_SOURCE_RESOURCE_ID	24000
#define SHRDRES_MAX_SOURCE_RESOURCE_ID	25951
//  CPP LangPackage
#define SHRDRES_MIN_CPP_RESOURCE_ID		25952
#define SHRDRES_MAX_CPP_RESOURCE_ID		25999
//  Resource package
#define SHRDRES_MIN_RES_RESOURCE_ID 	26000
#define SHRDRES_MAX_RES_RESOURCE_ID 	27999
//  MSIN
#define SHRDRES_MIN_MSIN_RESOURCE_ID	29000
#define SHRDRES_MAX_MSIN_RESOURCE_ID	29999
//  Proj
#define SHRDRES_MIN_PROJECT_RESOURCE_ID	31000
#define SHRDRES_MAX_PROJECT_RESOURCE_ID	31999
//  ClsView
#define SHRDRES_MIN_CLSVIEW_RESOURCE_ID	32000
#define SHRDRES_MAX_CLSVIEW_RESOURCE_ID	32100

/////////////////////////////////////////////////////////////////////////////

//
// IDs for shared resources.
//

// Dialog controls
#define SHRDRES_IDC_CAPTION                     102
#define SHRDRES_IDC_RIGHT_ARROW                 103
#define SHRDRES_IDC_SIZE_BAR_HORZ               104
#define SHRDRES_IDC_ID_CAP                      221
#define SHRDRES_IDC_ID                          222
#define SHRDRES_IDC_SCC_STATUS                  16830
#define SHRDRES_IDC_SCC_STATUS_TEXT             16831

// Resources
#define SHRDRES_IDR_MAINFRAME                   20000

#define SHRDRES_IDB_SPLASH                      20001
#define SHRDRES_IDB_SPLASH16                    20002
#define SHRDRES_IDB_ABOUT                       20003

#define SHRDRES_IDD_ABOUTBOX                    20001
#define SHRDRES_IDD_GOTODIALOG                  20002
#define SHRDRES_IDD_PUSHPIN_DIALOGBAR           20003
#define SHRDRES_IDD_GOTODLG						20004

// Strings
#define SHRDRES_IDS_DEBUG_CONFIG				20001
#define SHRDRES_IDS_RELEASE_CONFIG				20002
#define SHRDRES_IDS_UNDO_PROPEDIT               20003
#define SHRDRES_IDS_UNDO_MOVE                   20004
#define SHRDRES_IDS_UNDO_COPY                   20005
#define SHRDRES_IDS_UNDO_PASTE                  20006
#define SHRDRES_IDS_UNDO_CUT                    20007
#define SHRDRES_IDS_UNDO_DELETE                 20008
#define SHRDRES_IDS_ABOUTTITLE                  20009
#define SHRDRES_IDS_COPYRIGHT                   20010
#define SHRDRES_IDS_PRODUCTLICENSEDTO           20011
#define SHRDRES_IDS_SERIALNUMBER                20012
#define SHRDRES_IDS_ADDITIONS                   20013
#define SHRDRES_IDS_MAC_ADDON                   20014
#define SHRDRES_IDS_WARNING1                    20015
#define SHRDRES_IDS_WARNING2                    20016
#define SHRDRES_IDS_WARNING3                    20017
#define SHRDRES_IDS_WARNING4                    20018
#define SHRDRES_IDS_WARNING5                    20019
#define SHRDRES_IDS_WARNING6                    20020
#define SHRDRES_IDS_WARNING7                    20021
#define SHRDRES_IDS_NOMATHCHIP                  20022
#define SHRDRES_IDS_HASMATHCHIP                 20023
#define SHRDRES_IDS_FMTFREESPACE                20024
#define SHRDRES_IDS_UNAVAILABLE                 20025
#define SHRDRES_IDS_FMTAVAILMEM                 20026
#define SHRDRES_IDS_CANT_LOAD_PACKAGE           20027
#define SHRDRES_IDS_CANT_LOAD_MAC               20028
#define SHRDRES_IDS_LICENSEDTO                  20029
#define SHRDRES_IDS_GENERAL                     20030
#define SHRDRES_IDS_STYLES						20031
#define SHRDRES_IDS_EXSTYLES					20032

// DocTemplateID's (strings and icons)
#define SHRDRES_IDR_RC_TEMPLATE                 20048
#define SHRDRES_IDR_RCT_TEMPLATE                20049
#define SHRDRES_IDR_RES_TEMPLATE                20050
#define SHRDRES_IDR_EXE_TEMPLATE                20051
#define SHRDRES_IDR_PE_TEMPLATE                 20052
#define SHRDRES_IDR_BIN_TEMPLATE                20053
#define SHRDRES_IDR_BMP_TEMPLATE                20054
#define SHRDRES_IDR_CUR_TEMPLATE                20055
#define SHRDRES_IDR_ICO_TEMPLATE                20056
#define SHRDRES_IDR_TEXT_TEMPLATE               20057
#define SHRDRES_IDR_BSC_TEMPLATE                20058
#define SHRDRES_IDR_PRJ_TEMPLATE                20059
#define SHRDRES_IDR_PRJWKS_TEMPLATE             20060
#define SHRDRES_IDR_MSIN_TEMPLATE               20061

#endif //__SHRDRES_H__
