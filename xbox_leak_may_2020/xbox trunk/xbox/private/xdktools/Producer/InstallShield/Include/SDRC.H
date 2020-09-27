
 /*=======================================================================*/
 /*                                                                       */
 /*           (c) InstallShield Software Corporation (1996-1997)          */
 /*            (c)  InstallShield Corporation  (1990-1996)                */
 /*                       Schaumburg, Illinois 60173                      */
 /*                          All Rights Reserved                          */
 /*                           InstallShield (R)                           */
 /*                                                                       */
 /*   File    : sdrc.h                                                    */
 /*                                                                       */
 /*   Purpose : This file contains the resource file constants for        */
 /*             compiling the dialog resource and the dialog script code. */
 /*                                                                       */
 /*=======================================================================*/


 //
 // Pre-defined script dialog constants
 //

        // Dialog id
#define SD_NDLG_REGISTERUSER            12001
#define SD_NDLG_REGISTERUSEREX          12002
#define SD_NDLG_CONFIRMREGISTRATION     12003
#define SD_NDLG_CONFIRMNEWDIR           12004
#define SD_NDLG_ASKDESTPATH             12005
#define SD_NDLG_WELCOME                 12006
#define SD_NDLG_SHOWINFOLIST            12007
#define SD_NDLG_SELECTFOLDER            12008
#define SD_NDLG_SETUPTYPE               12009
#define SD_NDLG_SHOWANYDIALOG           12010
#define SD_NDLG_DISPLAYTOPICS           12011
#define SD_NDLG_SHOWMSG                 12012
#define SD_NDLG_ASKOPTIONSLIST1         12013
#define SD_NDLG_ASKOPTIONSLIST2         12014
#define SD_NDLG_CHANGEDIR               12015
#define SD_NDLG_SHOWFILEMODS            12016
#define SD_NDLG_SHOWDLGEDIT1            12017
#define SD_NDLG_SHOWDLGEDIT2            12018
#define SD_NDLG_SHOWDLGEDIT3            12019
#define SD_NDLG_ASKOPTIONS              12020
#define SD_NDLG_COMPONENTDIALOG         12021
#define SD_NDLG_COMPONENTDIALOGADV      12022
#define SD_NDLG_DISKSPACE               12023
#define SD_NDLG_COMPONENTDIALOG2        12024
#define SD_NDLG_COMPONENTMULT           12025
#define SD_NDLG_OPTIONSBUTTONS          12026
#define SD_NDLG_BITMAP                  12027
#define SD_NDLG_COMPONENTDIALOGEX       12028
#define SD_NDLG_LICENSE                 12029
#define SD_NDLG_STARTCOPY               12030
#define SD_NDLG_FINISHREBOOT            12031
#define SD_NDLG_FINISH                  12032
#define SD_NDLG_SETUPTYPEEX             12033

#define SD_STR_NOTENOUGHSPACE           11600
#define SD_STR_SETUP_COMPLETE_RUN       11601
#define SD_STR_SETUP_COMPLETE           11602
#define SD_STR_INSTALLING               11603
#define SD_STR_INSUFFICIENTSPACE        11604
#define SD_STR_FREESPACE                11605
#define SD_STR_RESOLUTION               11606
#define SD_STR_READMENOW                11607
#define SD_STR_WRITE_ERR                11608
#define SD_STR_COPYFILE                 11609
#define SD_STR_GROUP_ICONS              11610
#define SD_STR_MODIFYFILE               11611
#define SD_STR_CREATEDIR                11612
#define SD_STR_WRITE_ACCESS             11613
#define SD_STR_LOCATEFILE               11614
#define SD_STR_LOCATION                 11615
#define SD_STR_INSTALLATION             11616
#define SD_STR_OLDVERSION               11617
#define SD_STR_RUNSETUP_LATER           11618
#define SD_STR_QUITNOW                  11619
#define SD_STR_CONTINUE                 11620
#define SD_STR_OVERWRITE                11621
#define SD_STR_LOADFILE                 11622
#define SD_STR_FILE_MODIFIED            11623
#define SD_STR_APPINUSE                 11624
#define SD_STR_CLOSEALLAPP              11625
#define SD_STR_INSTALLATION_DIR         11626
#define SD_STR_INSTALLATION_TYPE        11627
#define SD_STR_CLOSEALL_RESTART         11628
#define SD_STR_RELATEDFILE              11629
#define SD_STR_ERRORNO                  11630
#define SD_STR_FILESAVE_ERR             11631
#define SD_STR_INVALID_FOLDER           11632
#define SD_STR_INVALID_FOLDER_TITLE     11633

        // ---------------- General Constants -------------

                // pushbuttons
#define SD_PBUT_OK              1
#define SD_PBUT_CONTINUE        1
#define SD_PBUT_CANCEL          2
#define SD_PBUT_ABORT           3
#define SD_PBUT_RETRY           4
#define SD_PBUT_IGNORE          5
#define SD_PBUT_YES             6
#define SD_PBUT_NO              7
#define SD_PBUT_HELP            8
#define SD_PBUT_EXITSETUP       9
#define SD_PBUT_BACK            12

#define SD_PBUT_DISKSPACE       195
#define SD_PBUT_CHANGEDIR       196
#define SD_PBUT_CHANGE          197

        // Edit
#define SD_MULTEDIT_FIELD1      301

        // Static
#define SD_STA_MSG              710
#define SD_STA_MSG1             711
#define SD_STA_MSG2             712
#define SD_STA_DESTDIR          715
#define SD_SPACEREQ             716
#define SD_SPACEAVI             717
#define SD_STA_CHANGEDIRMSG     718
#define SD_STA_FIELD1           719
#define SD_STA_FIELD2           720
#define SD_STA_FIELD3           721

        // Id for bitmap and cut-in line image
#define SD_STA_IMAGE_1          1200
#define SD_STA_IMAGE_2          1300

#define SD_BMP_IMAGE            550

        // ---------------- Function Specific Constants -------------

                // SdConfirmRegistration
#define SD_STA_SERIALTITLE      701
#define SD_STA_SERIAL           702
#define SD_STA_COMPANY          703
#define SD_STA_NAME             704

               // SdRegisterUesr & SdRegisterUserEx
#define SD_EDIT_NAME            301
#define SD_EDIT_COMPANY         302
#define SD_EDIT_SERIAL          303

               // SdShowInformation
#define SD_EDIT_INFO            301

               // SdShowMsg
#define SD_ICO_SHMSG            1001

               // SdWelcome
#define SD_STA_WELCOME          701

               // SdSelectGroup
#define SD_EDIT_PROGGRP         301
#define SD_LIST_EXISTGRP        401

               // SdNewDir
#define SD_STA_NEWDIR           701

               // SdAskDestPath

              // SdStartCopy
#define SD_STA_SETTINGS         701

              // SdSetupType
#define SD_TYPICAL_MSG          701
#define SD_COMPACT_MSG          702
#define SD_CUSTOM_MSG           703
#define SD_TYPICAL_TITLE        704
#define SD_COMPACT_TITLE        705
#define SD_CUSTOM_TITLE         706
#define SD_RADIO_TYPICAL        501
#define SD_RADIO_COMPACT        502
#define SD_RADIO_CUSTOM         503

              // SdShowAnyDialog
#define SD_ICO_SADLG            1002
#define SD_RBUT_NOCHANGE        502
#define SD_RBUT_SAVE            503
#define SD_RBUT_CHANGE          501
#define SD_LIST_BATCH           401


             //SdShowDlgEdit1
#define SD_EDIT1                301
#define SD_ICO_SSEDT            1003

             //SdShowDlgEdit2
#define SD_EDIT2                302

             //SdShowDlgEdit3
#define SD_EDIT3                303

             //SdAskOptions
#define SD_CHECK1               501
#define SD_CHECK2               502
#define SD_CHECK3               503
#define SD_CHECK4               504

             //SdComponentDialogAdv
#define SD_SELECT1              500
#define SD_SELECT2              501
#define SD_SELECT3              502
#define SD_SELECT4              503
#define SD_SELECT5              504
#define SD_SELECT6              505
#define SD_SELECTSIZE1          800
#define SD_SELECTSIZE2          801
#define SD_SELECTSIZE3          802
#define SD_SELECTSIZE4          803
#define SD_SELECTSIZE5          804
#define SD_SELECTSIZE6          805
#define SD_COMPONENT_MSG        711

             //SdMutlipleSelect
#define SD_PBUT_SELECTALL       100
#define SD_PBUT_DESELECTALL     101
#define SD_LISTBOX              401
#define SD_LISTBOX2             402

             //SdDiaplayTopics
#define SD_STA_TOPIC            702
#define SD_STA_TOPIC1           703
#define SD_STA_TOPIC2           704
#define SD_STA_DETAIL           752
#define SD_STA_DETAIL1          753
#define SD_STA_DETAIL2          754

             //SdDComponentDialogAdv
#define SD_STA_DLGMSG           701

             //SdDComponentDialog

             // Disk Space Dialog
#define SD_DISKAVI              701
#define SD_DISKREQ              702
#define SD_COMBO_DRIVE          601

             // SdLicense
#define SD_ICO_LICENSE          1003

             // SdFinishReboot
#define SD_RBUT_RESTARTWINDOWS  501
#define SD_RBUT_RESTARTMACHINE  502
#define SD_RBUT_NONE            503

             // SdFinishReboot
#define SD_RBUT_LAUNCHREADME    501
#define SD_RBUT_LAUNCHAPP       502

             // SdOptionsButtons
#define SD_STA_STARTMSG1        701
#define SD_STA_STARTMSG2        702
#define SD_STA_STARTMSG3        703
#define SD_STA_STARTMSG4        704
#define SD_PBUT_STARTBUTTON1    101
#define SD_PBUT_STARTBUTTON2    102
#define SD_PBUT_STARTBUTTON3    103
#define SD_PBUT_STARTBUTTON4    104


#define SD_BMP_BUTTONOPTION1    12001
#define SD_BMP_BUTTONOPTION2    12002
#define SD_BMP_BUTTONOPTION3    12003
#define SD_BMP_BUTTONOPTION4    12004


