
 /*=======================================================================*/
 /*                                                                       */
 /*           (c) InstallShield Software Corporation (1996-1997)          */
 /*            (c)  InstallShield Corporation  (1990-1996)                */
 /*                       Schaumburg, Illinois 60173                      */
 /*                          All Rights Reserved                          */
 /*                           InstallShield (R)                           */
 /*                                                                       */
 /*   File    : sddialog.h                                                */
 /*                                                                       */
 /*   Purpose : This file contains the declarations necessary for access  */
 /*             to the script based dialogs in InstallShield.             */
 /*             These dialogs provide a set of customizable dialogs       */
 /*             for use in InstallShield.                                 */
 /*                                                                       */
 /*=======================================================================*/


 //
 // Script dialog global variables
 //

 //
 // Supported script dialog functions
 //
 prototype SdRegisterUserEx(STRING, STRING, BYREF STRING, BYREF STRING, BYREF STRING);
 prototype SdRegisterUser(STRING, STRING, BYREF STRING, BYREF STRING);
 prototype SdConfirmRegistration( STRING, STRING, STRING, STRING, NUMBER );
 prototype SdConfirmNewDir(STRING, STRING, NUMBER);
 prototype SdAskDestPath(STRING, STRING, BYREF STRING, NUMBER);
 prototype SdWelcome(STRING, STRING);
 prototype SdShowInfoList(STRING, STRING, LIST);
 prototype SdSelectFolder(STRING, STRING, BYREF STRING);
 prototype SdSetupType(STRING, STRING, BYREF STRING, NUMBER);
 prototype SdSetupTypeEx(STRING, STRING, STRING, BYREF STRING, NUMBER);
 prototype SdShowAnyDialog(STRING, STRING, NUMBER, NUMBER);
 prototype SdDisplayTopics(STRING, STRING, LIST, LIST, NUMBER);
 prototype SdShowMsg(STRING, BOOL);
 prototype SdAskOptionsList(STRING, STRING, STRING, NUMBER );
 prototype SdShowFileMods(STRING, STRING, STRING, STRING, LIST, BYREF NUMBER);
 prototype SdShowDlgEdit1(STRING, STRING, STRING, BYREF STRING);
 prototype SdShowDlgEdit2(STRING, STRING, STRING, STRING, BYREF STRING, BYREF STRING);
 prototype SdShowDlgEdit3(STRING, STRING, STRING, STRING, STRING, BYREF STRING, BYREF STRING, BYREF STRING);
 prototype SdAskOptions(STRING, STRING, STRING, STRING, STRING, NUMBER);
 prototype SdComponentDialogAdv(STRING, STRING, BYREF STRING, STRING);
 prototype SdComponentDialog(STRING, STRING, BYREF STRING, STRING);
 prototype SdComponentDialog2(STRING, STRING, STRING, STRING);
 prototype SdComponentMult(STRING, STRING, STRING, STRING);
 prototype SdBitmap(STRING, STRING, STRING);
 prototype SdOptionsButtons( STRING, STRING, NUMBER, NUMBER );
 prototype SdProductName( STRING );
 prototype SdLicense( STRING, STRING, STRING, STRING );
 prototype SdStartCopy( STRING, STRING, LIST );
 prototype SdFinishReboot( STRING, STRING, NUMBER, STRING, NUMBER );
 prototype SdFinish( STRING, STRING, STRING, STRING, STRING, BYREF NUMBER, BYREF NUMBER );

        number  nSdDialog;      // indicates which dialog is in process

        // -- internal prototypes declares --
#include "sdrc.h"
#include "sdint.h"

