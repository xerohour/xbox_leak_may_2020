
 /*=======================================================================*/
 /*                                                                       */
 /*           (c) InstallShield Software Corporation (1996-1997)          */
 /*            (c)  InstallShield Corporation  (1990-1996)                */
 /*                       Schaumburg, Illinois 60173                      */
 /*                          All Rights Reserved                          */
 /*                           InstallShield (R)                           */
 /*                                                                       */
 /*   File    : sdint.h                                                   */
 /*                                                                       */
 /*   Purpose : This file contains the internal declarations necessary    */
 /*             for operations under script dialogs.                      */
 /*                                                                       */
 /*=======================================================================*/


 //
 // Pre-defined script dialog constants
 //

        // Dialog names and customization interface keys
#define SD_DLG_REGISTERUSEREX           "SdRegisterUserEx"
#define SD_DLG_REGISTERUSER             "SdRegisterUser"
#define SD_DLG_CONFIRMREGISTRATION      "SdConfirmRegistration"
#define SD_DLG_CONFIRMNEWDIR            "SdConfirmNewDir"
#define SD_DLG_ASKDESTPATH              "SdAskDestPath"
#define SD_DLG_WELCOME                  "SdWelcome"
#define SD_DLG_SHOWINFOLIST             "SdShowInfoList"
#define SD_DLG_SELECTFOLDER             "SdSelectFolder"
#define SD_DLG_SETUPTYPE                "SdSetupType"
#define SD_DLG_SETUPTYPEEX              "SdSetupTypeEx"
#define SD_DLG_SHOWANYDIALOG            "SdShowAnyDialog"
#define SD_DLG_DISPLAYTOPICS            "SdDisplayTopics"
#define SD_DLG_SHOWMSG                  "SdShowMsg"
#define SD_DLG_ASKOPTIONSLIST           "SdAskOptionsList"
#define SD_DLG_OPTIONSBUTTONS           "SdOptionsButtons"
#define SD_DLG_BITMAP                   "SdBitmap"
#define SD_DLG_SHOWFILEMODS             "SdShowFileMods"
#define SD_DLG_SHOWDLGEDIT1             "SdShowDlgEdit1"
#define SD_DLG_SHOWDLGEDIT2             "SdShowDlgEdit2"
#define SD_DLG_SHOWDLGEDIT3             "SdShowDlgEdit3"
#define SD_DLG_ASKOPTIONS               "SdAskOptions"
#define SD_DLG_COMPONENTDIALOG          "SdComponentDialog"
#define SD_DLG_COMPONENTDIALOG2         "SdComponentDialog2"
#define SD_DLG_COMPONENTMULT            "SdComponentMult"
#define SD_DLG_COMPONENTDIALOGADV       "SdComponentDialogAdv"
#define SD_DLG_PRODUCTNAME              "SdProductName"
#define SD_DLG_LICENSE                  "SdLicense"
#define SD_DLG_STARTCOPY                "SdStartCopy"
#define SD_DLG_FINISHREBOOT             "SdFinishReboot"
#define SD_DLG_FINISH                   "SdFinish"

        // internal functions
#define SD_DLG_COMPONENTDIALOGEX        "SdComponentDialogEx"

 //
 // Script dialog global variables
 //

 BOOL   bSdInit;           // indicates if the sd dialogs are initialized
 STRING szSdProduct;       // global name of product
 STRING szAppKey[200];     // name created to record/read data during silent mode
 STRING szSdStr_NotEnoughSpace[ _MAX_STRING ]; // error message
 BOOL   bInstall16;        // InstallShield executing is 16 bits
 BOOL   bSdShowMsgUsed;    // Is set to TRUE if the SdShowMsg is enabled

        // counters for unique recording names
 NUMBER nSdAskDestPath;
 NUMBER nSdRegisterUserEx;
 NUMBER nSdRegisterUser;
 NUMBER nSdConfirmRegistration;
 NUMBER nSdConfirmNewDir;
 NUMBER nSdShowDlgEdit1;
 NUMBER nSdShowDlgEdit2;
 NUMBER nSdShowDlgEdit3;
 NUMBER nSdShowAnyDialog;
 NUMBER nSdDisplayTopics;
 NUMBER nSdWelcome;
 NUMBER nSdShowInfoList;
 NUMBER nSdSelectFolder;
 NUMBER nSdSetupType;
 NUMBER nSdSetupTypeEx;
 NUMBER nSdAskOptionsList;
 NUMBER nSdShowFileMods;
 NUMBER nSdAskOptions;
 NUMBER nSdComponentDialog;
 NUMBER nSdComponentDialog2;
 NUMBER nSdComponentMult;
 NUMBER nSdBitmap;
 NUMBER nSdOptionsButtons;
 NUMBER nSdComponentDialogAdv;
 NUMBER nSdLicense;
 NUMBER nSdStartCopy;
 NUMBER nSdFinishReboot;
 NUMBER nSdFinish;

 typedef  _sdRECT
   begin
     INT left;
     INT top;
     INT right;
     INT bottom;
   end;

 typedef  _sdSIZE
   begin
     INT cx;
     INT cy;
   end;

        // constants for Windows interaction
#define GWW_ID                 -12
#define WM_SETSTYLE            1028
#define WM_SETSTYLE_32         0x00F4
#define WM_GETFONT             0x0031
#define WM_SHOW                5
#define WM_HIDE                0
#define BS_AUTORADIOBUTTON     9
#define LB_GETCARETINDEX       1056
#define LB_GETCARETINDEX_32    0x019F
#define LB_GETTEXT             1034
#define LB_GETTEXT_32          0x0189
#define LB_SETSEL              1030
#define LB_SETSEL_32           0x0185

 //
 // Supported script dialog functions
 //

 // Global generic prototypes (sdint.rul)
 prototype SdIsStdButton( NUMBER );
 prototype SdDoStdButton( NUMBER );
 prototype SdGeneralInit( STRING, NUMBER, NUMBER, STRING );
 prototype SdInit();
 prototype SdUnInit();
 prototype SdError( INT, STRING );
 prototype SdCloseDlg( INT, BYREF INT, BYREF BOOL );
 prototype SdDiskSpace( BYREF STRING, BYREF STRING, STRING, BYREF BOOL );
 prototype SdSetStatic( STRING, INT, STRING );
 prototype SdGetTextExtent( INT, STRING, INT );
 prototype SdPlugInProductName( STRING, NUMBER, STRING, NUMBER, NUMBER );
 prototype SdEnablement( HWND );
 prototype SdMakeName( BYREF STRING, STRING, STRING, BYREF NUMBER );

 // Local function prototypes
 prototype SdRegEnableButton( INT, INT, BYREF STRING, BYREF STRING );
 prototype SdRegExEnableButton( INT, INT, BYREF STRING, BYREF STRING, BYREF STRING );
 prototype SdOptionSetState( INT, INT );
 prototype SdOptionInit( STRING, INT, STRING, INT, INT );
 prototype SdComponentAdvInit( STRING, INT, STRING, INT );
 prototype SdComponentAdvUpdateSpace( STRING, STRING, STRING, INT );
 prototype SdComponentAdvCheckSpace( STRING, STRING, STRING );
 prototype SdComponentDlgCheckSpace( STRING, STRING, STRING );
 prototype SdUpdateComponentSelection( STRING, STRING, NUMBER );
 prototype SdComponentDialogEx( STRING, NUMBER, NUMBER, STRING, STRING );
 prototype SdGetItemName( BYREF STRING );
 prototype SdOptionsButtonsInit( STRING, NUMBER, NUMBER, NUMBER );
 prototype SdSetSequentialItems( STRING, NUMBER, NUMBER, NUMBER );
 prototype SdFinishInit32( STRING, NUMBER, NUMBER );
 prototype SdRemoveEndSpace( BYREF STRING );
 prototype SdGetUserCompanyInfo( BYREF STRING, BYREF STRING );
 prototype SdCreateComponentView( NUMBER, NUMBER, STRING, STRING );
 prototype SdSetupTypeFromDisplayName( BYREF STRING, STRING, LIST, LIST );
 prototype SdDisplayNameFromSetupType( BYREF STRING, STRING );
 prototype BOOL SdIsShellExplorer();

#ifndef _WIN_PROTOTYPES
#define _WIN_PROTOTYPES 1

 // Necessary Windows API prototypes

  prototype  HWND GDI.SelectObject( HWND, HWND );
  prototype  BOOL GDI.GetTextExtentPoint( HWND, STRING, INT, POINTER );
  prototype  BOOL USER.EnableWindow( HWND, SHORT );
  prototype   INT USER.GetClassName( HWND, POINTER, INT );
  prototype  HWND USER.GetDC( HWND );
  prototype  HWND USER.GetDlgItem( HWND, INT );
  prototype  HWND USER.GetFocus();
  prototype  LONG USER.GetWindowLong( HWND, INT );
  prototype       USER.GetWindowRect( HWND, POINTER );
  prototype SHORT USER.GetWindowWord( HWND, INT );
  prototype  BOOL USER.IsIconic( HWND );
  prototype  BOOL USER.IsWindow( HWND );
  prototype  BOOL USER.IsWindowEnabled( HWND );
  prototype   INT USER.MoveWindow( HWND, INT, INT, INT, INT, INT );
  prototype   INT USER.ReleaseDC( HWND, HWND );
  prototype  HWND USER.SetFocus( HWND );
  prototype       USER.SetWindowText( HWND, STRING );
  prototype   INT USER.ShowWindow( HWND, SHORT );

  prototype  INT  USER.LoadString( INT, INT, STRING, INT );
  prototype  HWND KERNEL.GetModuleHandle( STRING );
  prototype       USER.GetClientRect( HWND, POINTER );
  prototype   INT USER.SetWindowPos( HWND, INT, INT, INT, INT, INT, INT );
  prototype  BOOL USER.PostMessage( HWND, SHORT, SHORT, LONG );
  prototype   INT USER.ShowCursor( SHORT );
  prototype  BOOL USER.SystemParametersInfo( HWND, INT, POINTER , INT );

#endif
