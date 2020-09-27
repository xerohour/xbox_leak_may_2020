//
// Resource IDs
//

//
//  Self Registration Script
//
#define IDR_XBOXFOLDER                  101


// Icons
//
//  ***MUST READ*** IF YOU ADD OR ALTER ICONS!!!!!
//
//  Bug 10152 - The shell ruthlessly caches icons, and there is no reliable way to force it to refresh certain images in the
//              cache or the whole cache.  Consequently, any new icon we access by telling the shell its location (i.e. through the
//              registry or IExtractIcon::GetIconLoaction) must occupy a new index that we have never told the shell had an icon at it
//              before.  Prior to this fix we had used temporary icons, or we had rearranged the icon index order.  Now that we have
//              final icons, we need to fix this bug.  Indices 0-12 (Resources IDs 201-213) previously had some icon at them.  Some we
//              may have previously told the shell about, others maybe not.  Since I don't know for sure, we will assume that on some
//              machine somewhere the shell has icon an icon at each of these locations.  It is OK to use these indices for Icons we
//              load ourselves with LoadIcon (for dialogs, property pages, etc), but not icons we tell the shell about.  In the future
//              following the below instructions for use and documentation should avoid any problems.  - mitchd (1/16/2002)
//
//              INSTRUCTIONS FOR MODIFYING ICONS: Document the use of icon indices.  There are four possibilites
//               a) SHELL       -  this icon is loaded by the shell given the location.  If you update this icon or it becomes obsolete replace
//                                 it with a placeholder.  Place the updated icon at a new index.
//
//               b) INTERNAL(DO NOT REPLACE WITH SHELL) - this icon is loaded internally only.  It was previously used for SHELL
//                                                        icon and can never be used for a SHELL icon again.
//
//               c) INTERNAL    - this icon is loaded internally.  It was never used for a shell icon, so you could put a new
//                                shell icon in its place.
//               d) PLACEHOLDER - this icon is currently not in use.  It was previously used for a shell icon, so you may not put another SHELL icon here.
//

#define IDI_BASE                        201
#define IDI_REPLACE                     201   //INTERNAL  (DO NOT REPLACE WITH SHELL)
#define IDI_DELETE                      202   //INTERNAL  (DO NOT REPLACE WITH SHELL)
#define IDI_MOVE                        203   //INTERNAL  (DO NOT REPLACE WITH SHELL)
#define IDI_MULDOC                      204   //INTERNAL  (DO NOT REPLACE WITH SHELL)
#define IDI_CAMERA                      205   //INTERNAL  (DO NOT REPLACE WITH SHELL)
#define IDI_REBOOT                      206   //INTERNAL  (DO NOT REPLACE WITH SHELL)
#define IDI_RESERVED_207                207   //PLACEHOLDER
#define IDI_RESERVED_208                208   //PLACEHOLDER
#define IDI_RESERVED_209                209   //PLACEHOLDER
#define IDI_RESERVED_210                210   //PLACEHOLDER
#define IDI_RESERVED_211                211   //PLACEHOLDER
#define IDI_RESERVED_212                212   //PLACEHOLDER
#define IDI_RESERVED_213                213   //PLACEHOLDER
#define IDI_MAIN                        214   //SHELL  - Referenced by the XboxFolder.rgs script, it tells the
                                              //         shell the IDI_MAIN is the icon for Xbox Neighborhood.
                                              //         note that the index for the registry ICON_INDEX(IDI_MAIN) =
                                              //         IDI_MAIN-IDI_BASE = 13
#define IDI_ADD_CONSOLE                 215   //SHELL
#define IDI_CONSOLE                     216   //SHELL
#define IDI_CONSOLE_DEFAULT             217   //SHELL
#define IDI_VOLUME                      218   //SHELL
#define IDI_FOLDER                      219   //SHELL
#define IDI_XBE                         220   //SHELL
#define ICON_INDEX(rsrcId) (rsrcId-IDI_BASE)

//
//  Column headers names for various detail views
//
#define IDS_COLUMN_HEADER_NAME              301
#define IDS_COLUMN_HEADER_IPADDR            302
#define IDS_COLUMN_HEADER_TYPE              303
#define IDS_COLUMN_HEADER_FREE_CAPACITY     304
#define IDS_COLUMN_HEADER_TOTAL_CAPACITY    305
#define IDS_COLUMN_HEADER_SIZE              306
#define IDS_COLUMN_HEADER_MODIFIED          307
#define IDS_COLUMN_HEADER_CREATED           308  //Not Currently Used
#define IDS_COLUMN_HEADER_ATTRIBUTES        309

//
//  Resource IDs for the Menu Strings
//
#define IDS_CM_NEW                319
#define IDS_CM_REBOOT             320
#define IDS_CM_LAUNCH             321
#define IDS_CM_OPEN               322
#define IDS_CM_EXPLORE            323
#define IDS_CM_REBOOT_WARM        324
#define IDS_CM_REBOOT_SAME_TITLE  325
#define IDS_CM_REBOOT_COLD        326
#define IDS_CM_CAPTURE            327
#define IDS_CM_SETDEFAULT         328
#define IDS_CM_SECURITY           329
#define IDS_CM_CUT                330
#define IDS_CM_COPY               331
#define IDS_CM_PASTE              332
#define IDS_CM_DELETE             333
#define IDS_CM_RENAME             334
#define IDS_CM_NEW_FOLDER         335
#define IDS_CM_NEW_CONSOLE        336
#define IDS_CM_PROPERTIES         337

#define HELP_ID_FROM_COMMAND_ID(_X_) ((_X_)+(IDS_CMH_LAUNCH-IDS_CM_LAUNCH))

#define IDS_CMH_LAUNCH             338
#define IDS_CMH_OPEN               339
#define IDS_CMH_EXPLORE            340
#define IDS_CMH_REBOOT_WARM        341
#define IDS_CMH_REBOOT_SAME_TITLE  342
#define IDS_CMH_REBOOT_COLD        343
#define IDS_CMH_CAPTURE            344
#define IDS_CMH_SETDEFAULT         345
#define IDS_CMH_SECURITY           346
#define IDS_CMH_CUT                347
#define IDS_CMH_COPY               348
#define IDS_CMH_PASTE              349
#define IDS_CMH_DELETE             350
#define IDS_CMH_RENAME             351
#define IDS_CMH_NEW_FOLDER         352
#define IDS_CMH_NEW_CONSOLE        353
#define IDS_CMH_PROPERTIES         354

//
//  Progress Strings
//
#define IDS_FILE_COPY_PROGRESS_TITLE        360
#define IDS_FILE_MOVE_PROGRESS_TITLE        361
#define IDS_FILE_COPY_PROGRESS_PREPARING    363
#define IDS_FILE_MOVE_PROGRESS_PREPARING    364
#define IDS_FILE_PROGRESS_TARGET            365
#define IDS_FILE_PROGRESS_CANCEL            366
#define IDS_FILE_DELETE_PROGRESS_TITLE      367
#define IDS_FILE_DELETE_PROGRESS_STATUS     368

//
//  Drive Types
//

#define IDS_DRIVETYPE_MEMORY_UNIT           375
#define IDS_DRIVETYPE_MAIN_ROOT             376
#define IDS_DRIVETYPE_BOOT                  377
#define IDS_DRIVETYPE_DEVELOPMENT           378
#define IDS_DRIVETYPE_TITLE_ROOT            379
#define IDS_DRIVETYPE_TITLE_CURRENT         380
#define IDS_DRIVETYPE_SAVED_CURRENT         381
#define IDS_DRIVETYPE_SAVED_ROOT            382
#define IDS_DRIVETYPE_SCRATCH               383
#define IDS_DRIVETYPE_DASH                  384
#define IDS_DRIVETYPE_UNKNOWN               385

//
// Confirmation Strings
//

#define IDS_CONFIRM_REMOVE_CONSOLE_CAPTION   400
#define IDS_CONFIRM_REMOVE_CONSOLE           401
#define IDS_CONFIRM_REMOVE_MULTIPLE_CONSOLES 402
#define IDS_CONFIRM_REMOVE_DEFAULT_CONSOLE   403
#define IDS_CONFIRM_RO_FILE_MOVE_CAPTION     404
#define IDS_CONFIRM_RO_FILE_MOVE             405
#define IDS_CONFIRM_RO_FOLDER_MOVE_CAPTION   406
#define IDS_CONFIRM_RO_FOLDER_MOVE           407
#define IDS_CONFIRM_FOLDER_REPLACE_CAPTION   408
#define IDS_CONFIRM_FOLDER_REPLACE           409
#define IDS_CONFIRM_DELETE_CAPTION           410
#define IDS_CONFIRM_DELETE                   411
#define IDS_CONFIRM_DELETE_RO                412
#define IDS_CONFIRM_DELETE_FOLDER_CAPTION    413
#define IDS_CONFIRM_DELETE_FOLDER            414
#define IDS_CONFIRM_DELETE_RO_FOLDER         415
#define IDS_CONFIRM_DELETE_MULTIPLE_CAPTION  416
#define IDS_CONFIRM_DELETE_MULTIPLE          417
#define IDS_CONFIRM_RENAME_CAPTION           418
#define IDS_CONFIRM_RENAME_RO_FILE           419
#define IDS_CONFIRM_DESKTOP_SHORTCUT_CAPTION 420
#define IDS_CONFIRM_DESKTOP_SHORTCUT         421

//
// helper strings for confirmation dialogs
//

#define IDS_THISFOLDER                       450
#define IDS_THESELECTEDITEMS                 451
#define IDS_READONLY                         452
#define IDS_NOTREADONLY                      453
#define IDS_HIDE                             454
#define IDS_UNHIDE                           455


//
//  Error Strings and Captions for Error Boxes
//

#define IDS_GENERIC_CAPTION                  500
#define IDS_CANNOT_DELETE_ROOT               501
#define IDS_CANNOT_DELETE_ADD_XBOX           502
#define IDS_CONSOLE_NOT_FOUND                503
#define IDS_CONSOLE_NOT_AVAILABLE            504
#define IDS_SECURE_CONNECTION_FAILED         505
#define IDS_COULDNT_UNLOCK_CONSOLE           506
#define IDS_COULDNT_LOCK_CONSOLE             507
#define IDS_CANNOT_REMOVE_LAST_MANAGER       508
#define IDS_COULDNT_APPLY_SECURITY_CHANGES   509
#define IDS_PASSWORD_SET_CAPTION             510
#define IDS_PASSWORD_SET                     511
#define IDS_COULDNT_SET_PASSWORD             512
#define IDS_COULDNT_REBOOT                   513
#define IDS_COULDNT_GET_SCREEN_SHOT          514
#define IDS_RENAME_ERROR_CAPTION             515
#define IDS_RENAME_ERROR_ILLEGAL_CHARACTERS  516
#define IDS_RENAME_XBDM_ERROR                517
#define IDS_TRANSFER_FAILED                  518
#define IDS_TRANSFER_FAILED_CAPTION          519
#define IDS_ERROR_LOW_MEMORY                 520
#define IDS_COULDNT_REPLACE_DIR_W_FILE       521
#define IDS_COULDNT_REPLACE_DIR_W_FILE_MULTI 522
#define IDS_COULDNT_REPLACE_FILE             523
#define IDS_COULDNT_CREATE_TARGET_DIR        524
#define IDS_CREATE_TEMP_FILE_FAILED          525
#define IDS_COULDNT_CONNECT_TO_XBOX          526
#define IDS_COULDNT_MOVE_FILE                527
#define IDS_COULDNT_READ_SOURCE_FILE         528
#define IDS_TRANSFER_ILLEGAL_FILENAME        529
#define IDS_TRANSFER_ILLEGAL_FILENAME_W      530
#define IDS_ERROR_PASTE_CAPTION              531
#define IDS_NOTHING_TO_PASTE                 532
#define IDS_ERROR_CREATE_FOLDER_CAPTION      533
#define IDS_ERROR_CREATE_FOLDER              534
#define IDS_ERROR_OPENNING_FOLDER_CAPTION    535
#define IDS_ERROR_OPENNING_FOLDER            536
#define IDS_ERROR_VOLUME_FULL                537

//
//  General Page for File\Folder
//
#define IDS_CONTAINS_FORMAT                  700
#define IDS_FILETYPE_VARIOUS                 701

//
//  General Page for Console
//
#define IDS_DEFAULT_TITLE                    710
#define IDS_TITLE_NOT_AVAILABLE              711

//
//  Security Page for Console
//

#define IDS_SECURITY_PERMISSIONS_FOR         720
#define IDS_SECURITY_UNLOCK_WARNING          721
#define IDS_SECURITY_UNLOCK_WARNING_CAPTION  722
#define IDS_SECURITY_LOCK_WARNING            723
#define IDS_SECURITY_LOCK_WARNING_CAPTION    724

//
//  Add Console Wizard Strings
//

#define IDS_ADDCONSOLE_GETNAME_TITLE         750
#define IDS_ADDCONSOLE_GETNAME_SUBTITLE      751
#define IDS_ADDCONSOLE_ACCESSDENIED_TITLE    752
#define IDS_ADDCONSOLE_ACCESSDENIED_SUBTITLE 753
#define IDS_ADDCONSOLE_MAKEDEFAULT_TITLE     754
#define IDS_ADDCONSOLE_MAKEDEFAULT_SUBTITLE  755
#define IDS_ADDCONSOLE_MAKEDEFAULT_QUESTION  756

//
//  Strings for Status Bar
//

#define IDS_STATUS_OBJECT_COUNT_FORMAT       780
#define IDS_STATUS_SELOBJECT_COUNT_FORMAT    781
#define IDS_STATUS_VOLUME                    782
#define IDS_STATUS_FILE                      783

//
//  Access Permission Names, the order of the Ids
//  determine the order in the security page listview.
//  
#define IDS_PERMISSION_READ                  800
#define IDS_PERMISSION_WRITE                 801
#define IDS_PERMISSION_CONFIGURE             802
#define IDS_PERMISSION_CONTROL               803
#define IDS_PERMISSION_MANAGE                804
#define PERMISSION_LISTVIEW_INDEX(_x_) (_x_ - IDS_PERMISSION_READ)

//
//  Misc Strings
//

#define IDS_ROOT_SHORT_CUT_NAME              850
#define IDS_ROOT_SHORT_CUT_DESCRIPTION       851
#define IDS_LITERAL_YES                      853
#define IDS_LITERAL_NO                       854
#define IDS_LITERAL_THIS_COMPUTER            855
#define IDS_NEW_FOLDER                       856
#define IDS_NEW_FOLDER_TEMPLATE              857
#define IDS_NORMAL_NAME_FORMAT               858

//
//  Preloaded strings (frequently loaded strings
//  that we keep one lazy loaded copy of in global
//  memory)
//
#define IDS_FIRST_PRELOADED_STRING              950

#define IDS_PRELOAD_FOLDER_TYPE_NAME            950
#define IDS_PRELOAD_FILESIZE_FORMAT_BYTES       951
#define IDS_PRELOAD_FILESIZE_FORMAT_KB          952
#define IDS_PRELOAD_FILESIZE_FORMAT_MB          953
#define IDS_PRELOAD_FILESIZE_FORMAT_GB          954
#define IDS_PRELOAD_FILEBYTESIZE_FORMAT1        955
#define IDS_PRELOAD_FILEBYTESIZE_FORMAT2        956
#define IDS_PRELOAD_XBOX_ZONE_NAME              957
#define IDS_PRELOAD_STATUS_ADD_CONSOLE_FORMAT   958
#define IDS_NEXT_PRELOADED_STRING               959

//
//  Error Strings that correspond with errors codes returned from xboxdbg.lib
//

#define IDC_XBDM_NOERRORSTRING  0x8200
#define IDC_E_UNEXPECTED        0x8201
#define IDC_E_INVALIDARG        0x8202

//
// These IDs are programmatically generated from error codes.  The exact values
// do matter.
//

#define IDC_XBDM_UNDEFINED        0x8000
#define IDC_XBDM_MAXCONNECT       0x8001
#define IDC_XBDM_NOSUCHFILE       0x8002
#define IDC_XBDM_NOMODULE         0x8003
#define IDC_XBDM_MEMUNMAPPED      0x8004
#define IDC_XBDM_NOTHREAD         0x8005
#define IDC_XBDM_INVALIDCMD       0x8007
#define IDC_XBDM_NOTSTOPPED       0x8008
#define IDC_XBDM_MUSTCOPY         0x8009
#define IDC_XBDM_ALREADYEXISTS    0x800A
#define IDC_XBDM_DIRNOTEMPTY      0x800B
#define IDC_XBDM_BADFILENAME      0x800C
#define IDC_XBDM_CANNOTCREATE     0x800D
#define IDC_XBDM_CANNOTACCESS     0x800E
#define IDC_XBDM_DEVICEFULL       0x800F
#define IDC_XBDM_NOTDEBUGGABLE    0x8010
#define IDC_XBDM_BADCOUNTTYPE     0x8011
#define IDC_XBDM_COUNTUNAVAILABLE 0x8012

#define IDC_XBDM_CANNOTCONNECT    0x8100
#define IDC_XBDM_CONNECTIONLOST   0x8101
#define IDC_XBDM_FILEERROR        0x8103
#define IDC_XBDM_NOTXBEFILE       0x8106
#define IDC_XBDM_MEMSETINCOMPLETE 0x8107
#define IDC_XBDM_NOXBOXNAME       0x8108

//
//  Dialogs
//

#define   IDD_CONFIRM_REPLACE         1201
#define   IDD_CONFIRM_FOLDER_REPLACE  1202
#define   IDD_CONFIRM_DELETE          1203
#define   IDD_CONFIRM_MOVE            1204
#define   IDD_CONFIRM_REMOVEDIR       1205
#define   IDD_DRV_GENERAL             1206
#define   IDD_FILEPROP                1207
#define   IDD_FOLDERPROP              1208
#define   IDD_FILEMULTPROP            1209
#define   IDD_ATTRIBS_RECURSIVE       1210
#define   IDD_CONSOLE_GENERAL         1211
#define   IDD_CONSOLE_SETTING         1212
#define   IDD_CONSOLE_SECURITY        1213
#define   IDD_CONSOLE_ADVANCED        1214
#define   IDD_ADDCONSOLE_WELCOME      1215
#define   IDD_ADDCONSOLE_GETNAME      1216
#define   IDD_ADDCONSOLE_ACCESSDENIED 1217
#define   IDD_ADDCONSOLE_MAKEDEFAULT  1218
#define   IDD_ADDCONSOLE_FINISH       1219
#define   IDD_USERNAME_PROMPT         1220
#define   IDD_PASSWORD_PROMPT         1221


//
//  Command ID used by multiple dialogs
//  (put XB in the name to avoid conflict
//  with standard IDC names)
//
#define   IDC_XB_YES          1301
#define   IDC_XB_YESTOALL     1302
#define   IDC_XB_NO           1303
#define   IDC_XB_NOTOALL      1304
#define   IDC_XB_CANCEL       1305
#define   IDC_XB_FILENAME     1306 
#define   IDC_XB_FILESIZE     1307
#define   IDC_XB_FILETIME     1308
#define   IDC_XB_FILEICON     1309
#define   IDC_XB_FILESIZE2    1310
#define   IDC_XB_FILETIME2    1311
#define   IDC_XB_FILEICON2    1312
#define   IDC_XB_TEXT         1313
#define   IDC_XB_TITLE        1314

//
//  Command IDs Used in the driver property
//  pages.
//
#define IDC_DRV_ICON            1321
#define IDC_DRV_LETTER1         1222
#define IDC_DRV_TYPE_TXT        1323
#define IDC_DRV_TYPE            1324
#define IDC_DRV_USEDCOLOR       1325
#define IDC_DRV_USEDBYTES_TXT   1326
#define IDC_DRV_USEDBYTES       1327
#define IDC_DRV_USEDMB          1328
#define IDC_DRV_FREECOLOR       1329
#define IDC_DRV_FREEBYTES_TXT   1330
#define IDC_DRV_FREEBYTES       1331
#define IDC_DRV_FREEMB          1332
#define IDC_DRV_TOTSEP          1333
#define IDC_DRV_TOTBYTES_TXT    1334
#define IDC_DRV_TOTBYTES        1335
#define IDC_DRV_TOTMB           1336
#define IDC_DRV_PIE             1337
#define IDC_DRV_TOTSEP2         1338

//
//  Command IDs for the file,
//  folder, and multiple file
//  property pages
//  
#define IDC_ITEMICON          1351
#define IDC_NAMEEDIT          1352
#define IDC_FILETYPE          1353
#define IDC_LOCATION          1354
#define IDC_FILESIZE          1355
#define IDC_CREATED           1356
#define IDC_LASTMODIFIED      1357
#define IDC_READONLY          1358
#define IDC_HIDDEN            1359
#define IDC_CONTAINS          1360

//
//  Command ID's for the Console property pages
//
#define IDC_IPADDRESS         1371
#define IDC_ALTIPADDRESS      1372
#define IDC_ALTIPADDRESS_TEXT 1373
#define IDC_RUNNINGTITLE      1374
#define IDC_SYSTEMTIME        1375
#define IDC_WARMBOOT          1376
#define IDC_COLDBOOT          1377
#define IDC_REBOOT            1378
#define IDC_CAPTURE           1379


//
//  Controls on Confirmation Dialogs
//
#define IDC_ATTRIBSTOAPPLY    1401
#define IDC_RECURSIVE         1402
#define IDC_NOTRECURSIVE      1403
#define IDC_RECURSIVE_TXT     1404

//
//  Controls used by the Add Xbox Wizard
//
#define IDC_ADDCONSOLE_NAME                1451
#define IDC_ADDCONSOLE_PASSWORD            1452
#define IDC_ADDCONSOLE_YES                 1453
#define IDC_ADDCONSOLE_NO                  1454
#define IDC_ADDCONSOLE_MAKEDEFAULT         1455
#define IDC_ADDCONSOLE_PERMISSIONS_CAPTION 1456 
#define IDC_ADDCONSOLE_PERMISSIONS         1457
#define IDC_ADDCONSOLE_COMPLETE            1458
#define IDC_ADDCONSOLE_READ                1459
#define IDC_ADDCONSOLE_WRITE               1460
#define IDC_ADDCONSOLE_CONFIGURE           1461
#define IDC_ADDCONSOLE_CONTROL             1462
#define IDC_ADDCONSOLE_MANAGE              1463

//
//  Controls on the Console Security Page
//
#define IDC_SECURITY_MACHINES              1470
#define IDC_SECURITY_USERLIST              1471
#define IDC_SECURITY_ADD                   1472
#define IDC_SECURITY_REMOVE                1473
#define IDC_SECURITY_ACCESS                1474
#define IDC_SECURITY_ACCESS_TEXT           1475
#define IDC_SECURITY_UNLOCK                1476
#define IDC_SECURITY_CHANGE_PASSWORD       1477
#define IDC_SECURITY_MANAGE_TEXT           1478
#define IDC_SECURITY_PASSWORD_TEXT         1479
#define IDC_SECURITY_PASSWORD_EDIT         1480
#define IDC_SECURITY_MANAGE_BUTTON         1481
#define IDC_SECURITY_UNLOCKED_TEXT         1482
#define IDC_SECURITY_LOCK_BUTTON           1483
#define IDC_SECURITY_CONFIRM_PASSWORD      1484
#define IDC_SECURITY_PASSWORD_MISMATCH     1485

//
//  Resource ID of the Drop Popup Context Menu
//  (a.k.a. Copy\Move\Cancel)
//
#define IDM_CONTEXT_MENU_DROP   1551

//           
//  These definitions get sucked in by windows.h
//  but not for resources.
//
#ifdef RC_INVOKED
#define	DROPEFFECT_NONE	0
#define	DROPEFFECT_COPY	1
#define	DROPEFFECT_MOVE	2
#endif 

//
//  Animation
//
#define IDA_COPY    1601
#define IDA_DELETE  1602

//
//  Bitmaps
//
#define IDB_WATERMARK 1701
#define IDB_HEADER    1702
