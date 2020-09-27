//{{NO_DEPENDENCIES}}
//
// Command IDs for the SHELL and all packages
//
//Reserved Ranges:  (All commands > 0x7FFF 
//                                      -- All non-command strings < 0x8000)
//

/////////////////////////////////////////////////////////////////////////////
// *All VC IDE Packages    0x8000-0x9FFF
//
//  Shell
#define MIN_SHELL_COMMAND_ID    0x8000
#define MAX_SHELL_COMMAND_ID    0x83FF
//  Build package
#define MIN_BUILD_COMMAND_ID    0x8400
#define MAX_BUILD_COMMAND_ID    0x85FF
//  Res package
#define MIN_RES_COMMAND_ID      0x8600
#define MAX_RES_COMMAND_ID      0x86FF
//  Src
#define MIN_SOURCE_COMMAND_ID   0x8700
#define MAX_SOURCE_COMMAND_ID   0x88FF
//  Wizards
#define MIN_WIZARD_COMMAND_ID   0x8900
#define MAX_WIZARD_COMMAND_ID   0x89FF
//  MSIN
#define MIN_MSIN_COMMAND_ID     0x8A00
#define MAX_MSIN_COMMAND_ID     0x8BFF
//  VBA                         (BUG: Also using 0x8800-0x880F)
#define MIN_VBA_COMMAND_ID      0x8C00
#define MAX_VBA_COMMAND_ID      0x8EFF
//  Proj
#define MIN_PROJECT_COMMAND_ID  0x8F00
#define MAX_PROJECT_COMMAND_ID  0x8FFF
// IV
// MarcI: Added commands for IV5
#define MIN_IV_COMMAND_ID   0x90A0
#define MAX_IV_COMMAND_ID   0x9FFF
//
// *External Packages      0xA000-0xDFFF
//
//  Fortran
#define MIN_FORTRAN_COMMAND_ID  0xA000
#define MAX_FORTRAN_COMMAND_ID  0xA1FF
//  Test
#define MIN_TEST_COMMAND_ID     0xA200
#define MAX_TEST_COMMAND_ID     0xA3FF
//  Espresso
#define MIN_ESPRESSO_COMMAND_ID 0xA400
#define MAX_ESPRESSO_COMMAND_ID 0xA5FF
//  Sample Package
#define MIN_SAMPLE_COMMAND_ID 0xDF00
#define MAX_SAMPLE_COMMAND_ID 0xDFFF
//
// *MFC
#define MIN_MFC_COMMAND_ID      0xE000
#define MAX_MFC_COMMAND_ID      0xFFFF
/////////////////////////////////////////////////////////////////////////////

// Major group ID's
#define IDG_FILE                                1
#define IDG_EDIT                                2
#define IDG_SEARCH                      3
#define IDG_PROJECT_MAJOR       4
#define IDG_BUILD_MAJOR                 5
#define IDG_RESOURCE                    6
#define IDG_IMAGE                               7
#define IDG_LAYOUT                      8
#define IDG_DEBUG                               9
#define IDG_TOOLS                               10
#define IDG_WINDOW                      11
#define IDG_HELP                                12
#define IDG_MACRO                               13
#define IDG_VIEW                        14
#define IDG_RUN                                 15
#define IDG_INSERT              16
#define IDG_ISEARCH             17

#define IDG_TEST                                50      // MSTest package menu.

// Minor Command groups
//
#define IDG_FILEOPS             1
#define IDG_FILESAVE            2
#define IDG_PRINT               3
#define IDG_MRU                 4
#define IDG_EXIT                5
#define IDG_MAIL                        6
#define IDG_PROJ_MRU        7
#define IDG_FILE_FIND       8
#define IDG_MSINOPEN        9

#define IDG_UNDO                        10
#define IDG_CLIPBOARD           11
#define IDG_FIND                        12
#define IDG_FIND_COMBO          13
#define IDG_GOTO                        14
#define IDG_BOOKMARK            15
#define IDG_EDITPROPS           16
#define IDG_OLE                         17
#define IDG_BREAKPOINTS                 18

#define IDG_PROJECTOPS          20
#define IDG_BUILD                       21
#define IDG_ERROR                       22
#define IDG_DEPENDENCIES        23
#define IDG_PROJECTCLOSE        24
#define IDG_PROJECTOPS_STD      25
#define IDG_PROJECT_ADD         26
#define IDG_PROJECTOPS_ADD      27
#define IDG_PROJECTOPS_SCAN     28
#define IDG_PROJECTOPS_SCHM     29
#define IDG_ERROR                       22
#define IDG_PROJECTOPS_ADD      27
#define IDG_PROJECTOPS_SCAN     28
#define IDG_PROJECTOPS_SCHM     29

#define IDG_RESOURCENEW         30
#define IDG_RESOURCEIMPORT      31
#define IDG_SYMBOLS                     32
#define IDG_VIEW_AS_POPUP       33
#define IDG_NEWRES                      34
#define IDG_CLASSWIZARD     35
#define IDG_LAYOUTOPT     36

#define IDG_DEBUGRUN            40
#define IDG_DEBUGSTEP           41
#define IDG_DEBUGSHOW           42
#define IDG_DEBUGTHREADS        43
#define IDG_CODECOVER           46
#define IDG_COVERITEMS          47
#define IDG_DEBUGOPTS       48
#define IDG_DEBUGACTIVE     49

#define IDG_TOOLOPS                     50
#define IDG_OPTIONS                     52
#define IDG_TOOLS_SEARCH                                53
#define IDG_USERTOOLS           54

#define IDG_WINDOWOPS           60
#define IDG_WINDOWALL           61
#define IDG_WINDOW_WINDOWS      62
#define IDG_WINDOWLIST          63

#define IDG_HELPGENERAL         70
#define IDG_HELPDOCS            71
#define IDG_HELPPSS             72
#define IDG_HELPINSTALLED       73
#define IDG_HELP_SUBSETCOMBO    74
#define IDG_HELPABOUT           75
#define IDG_HELP_OPENCOMBO      76
#define IDG_CONTENTS                    77
#define IDG_HELP_CONTENTS               78
#define IDG_HELP_BROWSE         79

#define IDG_IMAGEOPS            80
#define IDG_IMAGECOLORS         81
#define IDG_IMAGEGRID           82
#define IDG_IMAGETOOLS      83
#define IDG_IMAGE_DEVICE    84

#define IDG_LAYOUTOPS           90
#define IDG_LAYOUTGRID          91
#define IDG_LAYOUTTAB           92
#define IDG_RULER_CONTEXT       93

#define IDG_ALIGNEDGE           94
#define IDG_ALIGNCENTER         95
#define IDG_SPACEEVENLY         96
#define IDG_CENTERDIALOG        97
#define IDG_ARRANGEBUTTONS      98
#define IDG_SAMESIZE            99

#define IDG_BROWSE                      100
#define IDG_KEY                         101

#define IDG_MACRORUN            110
#define IDG_MACROBROWSE         111

#define IDG_PROJECT                     130
#define IDG_PARSER                      134
#define IDG_PRJSYS_OPS      135
#define IDG_BLDSYS_TOOL         136
#define IDG_TARGET_COMBO    137
#define IDG_BUILD_1         138
#define IDG_BUILD_2         139

#define IDG_VIEW_WINDOWS        141
#define IDG_VIEW_MSIN1                  142
#define IDG_VIEW_MSIN2                  143
#define IDG_VIEW_MSIN3                  144
#define IDG_MSIN_PRINT                  145

#define IDG_SCC_POPUP           150
#define IDG_SCC_OPS1            151
#define IDG_SCC_OPS2            152
#define IDG_SCC_OPS3            153
#define IDG_SCC_RUN             154

#define IDG_BROWSE_OPS          160
#define IDG_BROWSE_QUERY        161

#define IDG_VIEW_VBA            170

#define IDG_VIEW_FULLSCREEN 190
#define IDG_INSERT_1        191
#define IDG_INSERT_2        192
#define IDG_INSERT_3        193
#define IDG_INSERT_4        194
#define IDG_BUILD_DEBUG     196
#define IDG_BUILD_SETTINGS  197
#define IDG_VIEW_TOOLBARS   198
#define IDG_TOOLS_REMOTE    199

#define IDG_DEBUG_1         300
#define IDG_DEBUG_2         301
#define IDG_DEBUG_3         302
#define IDG_DEBUG_4         303
#define IDG_DEBUG_5         304
#define IDG_DEBUG_6         305
#define IDG_DEBUG_7         306
#define IDG_DEBUG_8         307

#define IDG_TEST_DEBUG          500
#define IDG_TEST_RECORD         501
#define IDG_TEST_SCREENS        502
#define IDG_TEST_DIALOGS        503
#define IDG_TEST_DRIVER         504

/////////////////////////////////////////////////////////////////////
// SUSHI Frame Commands
//
#define IDM_TOOLBAR_EDIT                0x8000
#define IDM_CUSTOMIZE                   0x8001
#define ID_WINDOW_CLOSE_ALL             0x8002
#define IDM_OPTIONS                             0x8003
#define IDM_TOGGLE_MDI                  0x8004
#define IDM_TOOLBAR_CUSTOMIZE   0x8005
#define ID_WINDOW_FULLSCREEN    0x8006
#define ID_FILE_SAVE_ALL_EXIT   0x8007

#define IDM_DOCKSHOW_BASE               0x8200
#define IDM_DOCKSHOW_LAST               0x8218  //24 possible show commands for all dockable windows.

#define IDM_ACTIVATE_PROJECT               0x8280
#define IDM_TOGGLE_PROJECT                 0x8281

#define IDMX_IDPROP                     0x8290
#define IDMX_PROPGENERAL                0x8291
#define IDMX_PROPSTYLES                 0x8292
#define ID_FASTHELP                     0x8298

#define IDM_USERTOOLS_BASE              0x82A0
#define IDM_USERTOOLS_0                 (IDM_USERTOOLS_BASE + 0)
#define IDM_USERTOOLS_1                 (IDM_USERTOOLS_BASE + 1)
#define IDM_USERTOOLS_2                 (IDM_USERTOOLS_BASE + 2)
#define IDM_USERTOOLS_3                 (IDM_USERTOOLS_BASE + 3)
#define IDM_USERTOOLS_4                 (IDM_USERTOOLS_BASE + 4)
#define IDM_USERTOOLS_5                 (IDM_USERTOOLS_BASE + 5)
#define IDM_USERTOOLS_6                 (IDM_USERTOOLS_BASE + 6)
#define IDM_USERTOOLS_7                 (IDM_USERTOOLS_BASE + 7)
#define IDM_USERTOOLS_8                 (IDM_USERTOOLS_BASE + 8)
#define IDM_USERTOOLS_9                 (IDM_USERTOOLS_BASE + 9)
#define IDM_USERTOOLS_10                (IDM_USERTOOLS_BASE + 10)
#define IDM_USERTOOLS_11                (IDM_USERTOOLS_BASE + 11)
#define IDM_USERTOOLS_12                (IDM_USERTOOLS_BASE + 12)
#define IDM_USERTOOLS_13                (IDM_USERTOOLS_BASE + 13)
#define IDM_USERTOOLS_14                (IDM_USERTOOLS_BASE + 14)
#define IDM_USERTOOLS_15                (IDM_USERTOOLS_BASE + 15)
#define IDM_USERTOOLS_LAST              0x82AF  //16 user tools on Tools menu

#define ID_PROJ_MRU_FIRST                               0x8C10
#define ID_PROJ_MRU_FILE1               (ID_PROJ_MRU_FIRST)
#define ID_PROJ_MRU_FILE2               (ID_PROJ_MRU_FIRST + 1)
#define ID_PROJ_MRU_FILE3               (ID_PROJ_MRU_FIRST + 2)
#define ID_PROJ_MRU_FILE4               (ID_PROJ_MRU_FIRST + 3)
#define ID_PROJ_MRU_FILE5               (ID_PROJ_MRU_FIRST + 4)
#define ID_PROJ_MRU_FILE6               (ID_PROJ_MRU_FIRST + 5)
#define ID_PROJ_MRU_FILE7               (ID_PROJ_MRU_FIRST + 6)
#define ID_PROJ_MRU_FILE8               (ID_PROJ_MRU_FIRST + 7)
#define ID_PROJ_MRU_FILE9               (ID_PROJ_MRU_FIRST + 8)
#define ID_PROJ_MRU_FILE10              (ID_PROJ_MRU_FIRST + 9)
#define ID_PROJ_MRU_FILE11              (ID_PROJ_MRU_FIRST + 10)
#define ID_PROJ_MRU_FILE12              (ID_PROJ_MRU_FIRST + 11)
#define ID_PROJ_MRU_FILE13              (ID_PROJ_MRU_FIRST + 12)
#define ID_PROJ_MRU_FILE14              (ID_PROJ_MRU_FIRST + 13)
#define ID_PROJ_MRU_FILE15              (ID_PROJ_MRU_FIRST + 14)
#define ID_PROJ_MRU_FILE16              (ID_PROJ_MRU_FIRST + 15)
#define ID_PROJ_MRU_LAST                                0x8C1F

#ifdef _DEBUG
#define IDM_OPENLAYOUT                  0x8300
#endif

#define ID_TOOLS_RECORD                 0x8302
#define ID_TOOLS_PLAYBACK               0x8303
#define ID_FILE_NEW_SOURCE              0x8304
#define ID_FILE_NEW_RC                  0x8305
#define IDM_WINDOW_SHOWPROPERTIES       0x8306
#define IDM_WINDOW_HSPROPERTIES         0x8307
#define ID_WINDOW_HIDE                          0x8308
#define ID_WINDOW_POPUP_HIDE            0x8309
#define ID_FILE_NEW_BITMAP                      0x830A
#define IDM_TOOLS_RUNMENU						0x830B
#define IDM_QUERY_GROUPBYMENU					0x830C
#define IDM_QUERY_VERIFYMENU					0x830D

#define ID_PROP_PUSHPIN                         0x8320
#define MIN_ID_REPEAT_CMD                                               0x8321
#define ID_REPEAT_CMD                           0x8321
#define ID_REPEAT_CMD0                                                  0x8322
#define ID_REPEAT_CMD1                                                  0x8323
#define ID_REPEAT_CMD2                                                  0x8324
#define ID_REPEAT_CMD3                                                  0x8325
#define ID_REPEAT_CMD4                                                  0x8326
#define ID_REPEAT_CMD5                                                  0x8327
#define ID_REPEAT_CMD6                                                  0x8328
#define ID_REPEAT_CMD7                                                  0x8329
#define ID_REPEAT_CMD8                                                  0x832A
#define ID_REPEAT_CMD9                                                  0x832B
#define MAX_ID_REPEAT_CMD                                               0x832B

#define ID_DRAG_MOVE                            0x8330
#define ID_DRAG_COPY                            0x8331
#define ID_DRAG_CANCEL                          0x8332

#define ID_CANCEL                                               0x8333
#define ID_MDI_CYCLE_FORWARD                                    0x8334

/////////////////////////////////////////////////////////////////////
// VPROJ Commands
//
#define IDM_PROJITEM_FIRST              0x8400
#define IDM_PROJITEM_NEW                0x8400
#define IDM_PROJITEM_MOVE               0x8401
#define IDM_PROJITEM_COPY               0x8402
#define IDM_PROJITEM_DELETE             0x8403
#define IDM_PROJITEM_OPEN               0x8404
#define IDM_PROJ_ADD_DATASOURCE         0x8405
#define IDM_PROJITEM_ADD_FILES          0x8406
#define IDM_PROJITEM_ADD_RC             0x8416
#define IDM_PROJITEM_ADD_GROUP          0x8407
#define IDM_PROJITEM_ADD_SUBPROJ        0x8408
#define IDM_PROJITEM_SCAN               0x8409
#define IDM_PROJECT_SCAN_ALL            0x840a
#define IDM_PROJITEM_BUILD              0x840b
#define IDM_PROJITEM_COMPILE            0x840c
#define IDM_PROJITEM_PRINT              0x840d
#define IDM_PROJITEM_REBUILD_ALL        0x840e
#define IDM_PROJECT_EXECUTE             0x840f
#define IDM_PROJITEM_BATCH_BUILD        0x8410
#define IDM_PROJITEM_CBUILD             0x841c
#define IDM_PROJITEM_CCOMPILE           0x841f
#define IDM_PROJECT_SET_AS_DEFAULT      0x8420
#define IDM_PROJITEM_LAST               0x84ff
#define IDM_PROJ_FIRST                  0x8500
#define IDM_PROJECT_SETTINGS            0x8500
#define IDM_PROJECT_STOP_BUILD          0x8501
#define IDM_PROJECT_CLOSEBSC            0x8502
#define IDM_PROJECT_TOOL_MECR           0x8503
#define IDM_PROJECT_CONFIGURATIONS      0x8504
#define IDM_PROJITEM_EXPORT_MAKEFILE	0x8505
#define IDM_PROJECT_TOOL_CMD_FIRST      0x8510
#define IDM_PROJECT_TOOL_CMD0           0x8510
#define IDM_PROJECT_TOOL_CMD1           0x8511
#define IDM_PROJECT_TOOL_CMD2           0x8512
#define IDM_PROJECT_TOOL_CMD3           0x8513
#define IDM_PROJECT_TOOL_CMD4           0x8514
#define IDM_PROJECT_TOOL_CMD5           0x8515
#define IDM_PROJECT_TOOL_CMD6           0x8516
#define IDM_PROJECT_TOOL_CMD_LAST       0x8516
#define IDM_PROJECT_TARGET              0x8517
#define IDM_PROJECT_TOOLS               0x8519
#define IDM_PROJECT_SETTINGS_POP        0x851a
#define IDM_PROJECT_BUILDGRAPH          0x851b
#define IDM_PROJECT_CLEAN               0x851c
#define IDM_PROJECT_EXPORT_MAKEFILE		0x8522
#define IDM_PROJ_ACTIVEPROJMENU			0x8523
// adding user-defined messages for PostMessage()
// this is to avoid buttons being depressed during duration of the cmd.
#define ID_PROJITEM_COMPILE             0x851d
#define ID_PROJITEM_BUILD               0x851e 
#define ID_PROJITEM_REBUILD_ALL         0x851f
#define ID_PROJITEM_BATCH_BUILD         0x8520

#define ID_TARGET_COMBO                 0x8521

// -> custom build tool output specification macros ->
#define IDM_BLDTOOL_DIRMACRO1           0x8550
#define IDM_BLDTOOL_DIRMACRO2           0x8551
#define IDM_BLDTOOL_DIRMACRO3           0x8552
#define IDM_BLDTOOL_DIRMACRO4           0x8553
#define IDM_BLDTOOL_DIRMACRO5           0x8554
#define IDM_BLDTOOL_DIRMACRO6           0x8555
#define IDM_BLDTOOL_DIRMACRO7           0x8556
#define IDM_BLDTOOL_DIRMACRO8           0x8557
#define IDM_BLDTOOL_FILEMACRO1          0x8558
#define IDM_BLDTOOL_FILEMACRO2          0x8559
#define IDM_BLDTOOL_FILEMACRO3          0x855a
#define IDM_BLDTOOL_FILEMACRO4          0x855b
#define IDM_BLDTOOL_FILEMACRO5          0x855c
#define IDM_BLDTOOL_FILEMACRO6          0x855d

#define IDM_BLDTOOL_FIRSTMACRO          IDM_BLDTOOL_DIRMACRO1
#define IDM_BLDTOOL_LASTMACRO           IDM_BLDTOOL_FILEMACRO6
// <- custom build tool output specification macros <-

#define IDM_PROJECT_DEFAULT_PROJ        0x852c

#define IDM_CTL_PROJ_TARGET                     0x85F0
#define IDM_PROJ_LAST                           0x85FF

/////////////////////////////////////////////////////////////////////
// VRES Commands
//
#define IDM_FILE_SETINCLUDES            0x8600
#define IDM_FILE_INSTALLCONTROLS        0x8601
#define IDM_EDIT_SYMBOLS                        0x8602
#define IDM_RESOURCE_NEW            0x8603
#define IDM_RESOURCE_OPEN                       0x8604
#define IDM_RESOURCE_OPENBINARY         0x8605
#define IDM_RESOURCE_IMPORT                     0x8606
#define IDM_RESOURCE_EXPORT                     0x8607
#define IDM_TOOLBAR_WIZARD                      0x8608
#define IDMY_CLASSWIZARD            0x8609
#define IDMB_NEW_DIALOG             0x860A
#define IDMB_NEW_CURSOR             0x860B
#define IDMB_NEW_ICON               0x860C
#define IDMB_NEW_BITMAP             0x860D
#define IDMB_NEW_MENUBAR            0x860E
#define IDMB_NEW_STRINGTABLE        0x860F
#define IDMB_NEW_ACCTABLE           0x8610
#define IDMB_NEWRC                  0x8611
#define IDMB_NEW_VERSION            0x8612
#define ID_CANCEL_EDIT              0x8613
#define IDM_RESOURCE_ADD            0x8614
#define IDM_RESOURCE_NEWCOPY        0x8615
#define IDMB_NEW_TOOLBAR                        0x8616

// special commands for specific VRES resource types
#define IDM_FIND_STRING             0x8650
#define IDM_FIND_NEXT               0x8651
#define IDM_NEW_STRING              0x8652
#define IDM_NEW_ACCELERATOR         0x8653
#define IDM_NEW_STRINGBLOCK         0x8654
#define IDM_DELETE_STRINGBLOCK      0x8655
#define IDMY_TEST                   0x8656
#define IDM_SETTABORDER             0x8657
#define IDMY_ALIGNLEFT              0x8658
#define IDMY_ALIGNRIGHT             0x8659
#define IDMY_ALIGNTOP               0x865A
#define IDMY_ALIGNBOTTOM            0x865B
#define IDMY_SPACEACROSS            0x865C
#define IDMY_SPACEDOWN              0x865D
#define IDM_BUTTONRIGHT             0x865E
#define IDM_BUTTONBOTTOM            0x865F
#define IDMY_MAKESAMEWIDTH          0x8660
#define IDMY_MAKESAMEHEIGHT         0x8661
#define IDMY_MAKESAMEBOTH           0x8662
#define IDM_SIZETOTEXT              0x8663
#define IDM_GRIDSETTINGS            0x8664
#define IDMB_SHOWGRID               0x8665
#define IDM_VIEWASPOPUP             0x8666
#define IDMY_PICKCOLOR              0x8667
#define IDM_FLIPBSHH                0x8668
#define IDM_FLIPBSHV                0x8669
#define IDM_MAGNIFY                 0x866A
#define IDM_NEWDEVIMG               0x866B
#define IDM_OPENDEVIMG              0x866C
#define IDM_DELDEVIMG               0x866D
#define IDM_INVERTCOLORS            0x866E
#define IDM_TGLOPAQUE               0x866F
#define IDM_EDITCOLORS              0x8670
#define IDM_LOADCOLORS              0x8671
#define IDM_SAVECOLORS              0x8672
#define IDM_SETHOTSPOT              0x8673
#define IDM_ALIGNCVERT              0x8674
#define IDM_ALIGNCHORZ              0x8675
#define IDMY_CENTERVERT             0x8676
#define IDMY_CENTERHORZ             0x8677
#define IDM_INSTALLCONTROLS         0x8678
#define IDMY_CHECKHOTKEYS           0x8679
#define IDMZ_IMGSEPARATOR           0x8680
#define IDMX_ZOOMIN                 0x8681
#define IDMX_ZOOMOUT                0x8682
#define IDMX_SMALLERBRUSH           0x8683
#define IDMX_LARGERBRUSH            0x8684
#define IDMB_RECTTOOL               0x8685
#define IDMB_FRECTTOOL              0x8686
#define IDMB_ELLIPSETOOL            0x8687
#define IDMB_FELLIPSETOOL           0x8688
#define IDMZ_BRUSHTOOL              0x8689
#define IDMB_FILLTOOL               0x868A
#define IDMB_LINETOOL               0x868B
#define IDMB_PICKTOOL               0x868C
#define IDMX_PREVCOLOR              0x868D
#define IDMX_NEXTCOLOR              0x868E
#define IDMX_PREVECOLOR             0x868F
#define IDMB_3DRECTTOOL             0x8690
#define IDMX_NEXTECOLOR             0x8691
#define IDMX_TEXTTOOL               0x8692
#define IDMB_CURVETOOL              0x8693
#define IDMB_ORECTTOOL              0x8694
#define IDMB_ORNDRECTTOOL           0x8695
#define IDMB_OELLIPSETOOL           0x8696
#define IDMB_POLYGONTOOL            0x8697
#define IDMB_FPOLYGONTOOL           0x8698
#define IDMB_OPOLYGONTOOL           0x8699
#define IDMB_PICKRGNTOOL            0x869A
#define IDMX_OUTLINEBSH             0x869B
#define IDMB_AIRBSHTOOL             0x869C
#define IDMX_SMALLBRUSH             0x869D
#define IDMX_SEL2BSH                0x869E
#define IDMB_RNDRECTTOOL            0x869F
#define IDMB_FRNDRECTTOOL           0x86A1
#define IDMB_PENCILTOOL             0x86A2
#define IDMB_ERASERTOOL             0x86A3
#define IDMB_ZOOMTOOL               0x86A4
#define IDMB_CBRUSHTOOL             0x86A5
#define IDM_GRIDOPT                 0x86A6
#define IDMX_SHOWTILEGRID           0x86A7
#define IDM_ROT90                   0x86A8
#define IDMX_OTHERPANE              0x86A9
#define IDM_SHOWIMAGEGRID           0x86AA
#define IDMB_SHOWRULER              0x86AB
#define IDMB_TOGGLEGUIDES           0x86AC
#define IDM_GUIDE_ATTACH_LEFT       0x86AD
#define IDM_GUIDE_ATTACH_RIGHT      0x86AE
#define IDM_GUIDE_ATTACH_PROPORTIONAL 0x86AF
#define IDM_GUIDE_CLEAR             0x86B0
#define IDM_GUIDE_CLEAR_ALL         0x86B1
#define IDM_WINDOWRECTTOOL          0x86B3
#define IDMY_FLIPDIALOG             0x86B4

#define IDM_CTL_POSFIRST                        0x86B5
	#define IDM_CTL_MOVELEFT                        0x86B5
	#define IDM_CTL_MOVEUP                          0x86B6
	#define IDM_CTL_MOVERIGHT                       0x86B7
	#define IDM_CTL_MOVEDOWN                        0x86B8
#define IDM_CTL_SIZEFIRST                       0x86B9
	#define IDM_CTL_SIZELEFT                        0x86B9
	#define IDM_CTL_SIZEUP                          0x86BA
	#define IDM_CTL_SIZERIGHT                       0x86BB
	#define IDM_CTL_SIZEDOWN                        0x86BC
#define IDM_CTL_POSLAST                         0x86BC

#define IDM_CTL_HOTSPOT_LOC         0x86F0
#define IDM_CTL_INSERT                          0x86F1


/////////////////////////////////////////////////////////////////////
// VCPP Commands
//
// Many of the standard ones (FileOpen, etc) now use the MFC
// predefined IDs.
//
#define IDM_FILE_SAVE_ALL               0x8700
#define ID_EDIT_FIND_DLG        0x8701 // invoke find dialog
#define IDM_EDIT_GREP                   0x8702
#define IDM_EDIT_FINDMATCH              0x8703
#define IDM_EDIT_READONLY               0x8704
#define ID_CUT_LINE             0x8705
#define ID_EDIT_FIND_NEXT       0x8706
#define ID_EDIT_FIND_PREV       0x8707
#define ID_EDIT_FIND_COMBO      0x8708 // activate find combo
#define IDM_ADDTOPROJECT                0x8709
#define IDM_EDIT_CLEAR_OUTPUT   0x870A
#define IDM_FILE_SEND                   0x870B
#define IDM_FILE_REVERT                 0x870C
#define IDM_FILE_LIST_DOCS              0x870D

#define IDM_GOTO_LINE                   0x8711
#define IDM_VIEW_MIXEDMODE              0x8712
#define IDM_GOTO_DISASSY                0x8713
#define IDM_GOTO_SOURCE                 0x8714
#define IDM_GOTO_CURRENTERROR			0x8715
#define IDM_VIEW_NEXTERROR              0x8716
#define IDM_VIEW_PREVIOUSERROR			0x8717
#define IDM_VIEW_TOGGLETAG              0x8718
#define IDM_VIEW_NEXTTAG                0x8719
#define IDM_VIEW_PREVIOUSTAG			0x871A
#define IDM_VIEW_CLEARALLTAGS			0x871B
#define IDM_GOTO_FILE                   0x871C
#define IDM_VIEW_OUTTOOLS               0x871D
#define IDM_BOOKMARKS                   0x871E
#define IDM_GOTO_CODE                   0x871F
#define IDM_BROWSE_GOTOREF              0x8720
#define IDM_BROWSE_GOTODEF              0x8721
#define IDM_BROWSE_NEXT                 0x8722
#define IDM_BROWSE_PREVIOUS             0x8723
#define IDM_BROWSE_OPEN                 0x8724
#define IDM_BROWSE_POP                  0x8725
#define IDM_BROWSE_SYM                  0x8726
#define IDM_BROWSE_OUTLINE              0x8727
#define IDM_BROWSE_FGRAPH               0x8728
#define IDM_BROWSE_CGRAPH               0x8729
#define IDM_BROWSE_FGRAPH_R             0x872A
#define IDM_BROWSE_CGRAPH_R             0x872B
#define IDM_BROWSE_QUERY                0x872C
#define IDM_BROWSE_FILES                0x872D

#define IDM_RUN_GO                      0x8730
#define IDM_RUN_RESTART                 0x8731
#define IDM_RUN_STOPDEBUGGING           0x8732
#define IDM_RUN_BREAK                   0x8733
#define IDM_RUN_TOCURSOR                0x8734
#define IDM_RUN_TRACEINTO               0x8735
#define IDM_RUN_STEPOVER                0x8736
#define IDM_RUN_STOPAFTERRETURN         0x8737
#define IDM_RUN_CALLS                   0x8738
#define IDM_RUN_SETBREAK                0x8739
#define IDM_RUN_QUICKWATCH              0x873A
#define IDM_RUN_EXCEPTIONS              0x873B
#define IDM_RUN_THREADS                 0x873C
#define IDM_RUN_PROFILE                 0x873D
#define IDM_RUN_COVER_TOGGLE            0x873E
#define IDM_RUN_COVER_CLEAR             0x873F
#define IDM_RUN_SETNEXTSTMT             0x8740
#define IDM_RUN_CLEARALLBREAK           0x8741
#define IDM_RUN_SRC_TRACEINTO           0x8742
#define IDM_RUN_SRC_STEPOVER            0x8743
#define IDM_RUN_FLIPHACK                0x8744 // not used in release build
#define IDM_RUN_REMOTEDEBUGGER          0x8745
#define IDM_RUN_JIT_GO                  0x8746
#define IDM_DEBUG_ACTIVE                0x8747
#define IDM_RUN_TRACEFUNCTION           0x8749
#define IDM_RUN_SRC_TRACEFUNCTION       0x874A
#define IDM_RUN_ENABLEBREAK             0x874B
#define IDM_RUN_TOGGLEBREAK             0x874C

#define IDM_OUTPUT_EXECTOOL             0x8750
#define IDM_OUTPUT_CLEAR                0x8751
#define IDM_OUTPUT_GOTO_TAG             0x8752
#define IDM_DEBUG_DEREF                 0x8753
#define IDM_DEBUG_GOTOSOURCE			0x8754
#define IDM_DBGSHOW_FLOAT               0x8755
#define IDM_DBGSHOW_STACKTYPES			0x8756
#define IDM_DBGSHOW_STACKVALUES			0x8757
#define IDM_DBGSHOW_SOURCE              0x8758
#define IDM_DBGSHOW_MEMBYTE             0x8759
#define IDM_DBGSHOW_MEMSHORT			0x875A
#define IDM_DBGSHOW_MEMLONG             0x875B
#define IDM_DBGSHOW_LOCALS_BAR			0x875C
#define IDM_DBGSHOW_WATCH_BAR			0x875D
#define IDM_DBGSHOW_MEMORY_BAR			0x875E
#define IDM_DBGSHOW_TYPE_COLUMN			0x875F
#define IDM_INDENT						0x8760
#define IDM_UNINDENT					0x8761
#define IDM_DBGSHOW_VARS_BAR			0x8762
#define IDM_KEYBOARDINDENT				0x8763
#define IDM_KEYBOARDUNINDENT			0x8764
#define IDM_TOOL_STOP					0x8765
#define IDM_FIRST_TOOL					0x8766
#define IDM_MACROS						0x8767
#define IDM_MACRO_PLAY					0x8768
#define IDM_MACRO_PAUSE					0x8769
#define IDM_MACRO_STOP					0x876A
#define IDM_WINDOW_ERRORS				0x876C
#define IDM_WINDOWCHILD					0x876D

#define IDM_HELP_FULLTEXT				0x8770
#define IDM_HELP_LANGUAGE				0x8771
#define IDM_HELP_MFC					0x8772
#define IDM_HELP_WIN32					0x8773
#define IDM_HELP_MSPROD					0x8774
#define IDM_HELP_RETRY					0x8775
#define IDM_HELP_CONTENTS               0x8776
#define IDM_HELP_INDEX                  0x8777
#define IDM_HELP_SEARCH					0x8778
#define IDM_HELP_EXTHELP				0x8779
//#define IDM_HELP_BOOKS          0x8779
//#define IDM_HELP_CRT            0x877A
#define IDM_HELP_TIPOFTHEDAY			0x877b

#define IDM_REFORMAT                    0x877f
#define IDM_UPPERCASE                   0x8780
#define IDM_NEWLINE                     0x8781
#define IDM_NEXTLINE                    0x8782
#define IDM_NEWLINEABOVE                0x8783
#define IDM_LOWERCASE                   0x8784
#define IDM_CLEARSELECTION              0x8785
#define IDM_SCROLL_LINEUP               0x8786
#define IDM_SCROLL_LINEDOWN             0x8787
#define ID_EDIT_FIND_NEXTCURRENTWORD    0x8788
#define ID_EDIT_FIND_PREVCURRENTWORD    0x8789
#define IDM_EDIT_DELETEWORDRIGHT        0x878a
#define IDM_EDIT_DELETELEFT             0x878b
#define IDM_FIND_IFDEF_UP               0x878c
#define IDM_FIND_IFDEF_DOWN             0x878d
#define IDM_EXTEND_FIND_IFDEF_UP        0x878e
#define IDM_EXTEND_FIND_IFDEF_DOWN      0x878f
#define IDM_CURSOR_STARTNEXTLINE        0x8790
#define ID_COL_SELECT                   0x8791
#define IDM_INSERT                      0x8792

#define ID_RUN_GO                               0x8793
#define ID_RUN_RESTART                          0x8794
#define ID_RUN_TOCURSOR                         0x8795
#define ID_RUN_TRACEINTO                        0x8796
#define ID_RUN_STEPOVER                         0x8797
#define ID_RUN_STOPAFTERRETURN                  0x8798
#define ID_RUN_SRC_TRACEINTO                    0x8799
#define ID_RUN_SRC_STEPOVER                     0x879a
#define ID_RUN_STOPDEBUGGING                    0x879b
#define IDM_VIEW_EPSDROPBOOKMARK                0x879c
#define IDM_VIEW_EPSNEXTBOOKMARK                0x879d
#define IDM_VIEW_BRF_DROPBOOKMARK1              0x879e
#define IDM_VIEW_BRF_DROPBOOKMARK2              0x879f
#define IDM_VIEW_BRF_DROPBOOKMARK3              0x87a0
#define IDM_VIEW_BRF_DROPBOOKMARK4              0x87a1
#define IDM_VIEW_BRF_DROPBOOKMARK5              0x87a2
#define IDM_VIEW_BRF_DROPBOOKMARK6              0x87a3
#define IDM_VIEW_BRF_DROPBOOKMARK7              0x87a4
#define IDM_VIEW_BRF_DROPBOOKMARK8              0x87a5
#define IDM_VIEW_BRF_DROPBOOKMARK9              0x87a6
#define IDM_VIEW_BRF_DROPBOOKMARK10             0x87a7
#define ID_RUN_TRACEFUNCTION                    0x87a8
#define ID_RUN_SRC_TRACEFUNCTION                0x87a9

#define IDM_ACTIVATE_SRCFIRST                   0x87B0
#define IDM_ACTIVATE_OUTPUT                     0x87B0
#define IDM_ACTIVATE_WATCH                      0x87B1
#define IDM_ACTIVATE_VARIABLES                  0x87B2
#define IDM_ACTIVATE_REGISTERS                  0x87B3
#define IDM_ACTIVATE_MEMORY                     0x87B4
#define IDM_ACTIVATE_CALLSTACK                  0x87B5
#define IDM_ACTIVATE_DISASSEMBLY                0x87B6
// ...
#define IDM_ACTIVATE_SRCLAST                                    0x87BF

#define IDM_TOGGLE_SRCFIRST                                             0x87B0
#define IDM_TOGGLE_OUTPUT                       0x87C0
#define IDM_TOGGLE_WATCH                        0x87C1
#define IDM_TOGGLE_VARIABLES                    0x87C2
#define IDM_TOGGLE_REGISTERS                    0x87C3
#define IDM_TOGGLE_MEMORY                       0x87C4
#define IDM_TOGGLE_CALLSTACK                    0x87C5
#define IDM_TOGGLE_DISASSEMBLY                  0x87C6
// ...
#define IDM_TOGGLE_SRCLAST                      0x87CF

#define IDM_VIEW_TOPIC							0x87D0	// Added by DougMan for InfoViewer Mnemonics
#define IDM_VIEW_RESULTS						0x87D1	// Added by DougMan for InfoViewer Mnemonics
#define IDM_HELP_IVBOOKMARKS					0x87D2	// Added by DougMan for InfoViewer Mnemonics
#define IDM_HELP_DEFINESUBSETS					0x87D3	// Added by DougMan for InfoViewer Mnemonics
#define IDM_HELP_SELECTSUBSETS					0x87D4	// Added by DougMan for InfoViewer Mnemonics


#define IDM_CTL_LOCAL_FUNCS                     0x87E0
#define IDM_CTL_WATCH_EDIT                      0x87E1
#define IDM_CTL_WATCH_DELETE                    0x87E2
#define IDM_CTL_BROWSE_CLASS                    0x6403
#define IDM_CTL_BROWSE_FUNCMEM                  0x6404
#define IDM_CTL_BROWSE_DATAMEM                  0x6405
#define IDM_CTL_BROWSE_MACRO                    0x6406
#define IDM_CTL_BROWSE_TYPE                     0x6407
#define IDM_CTL_BROWSE_FUNCS                    0x6408
#define IDM_CTL_BROWSE_DATA                     0x6409
#define IDM_CTL_MEMORY_EDIT                     0x87EA
#define IDM_CTL_VARS_FUNCS                      0x87EB

#define IDM_PARSER_PARSE                0x87F0
#define IDM_PARSER_INFO                 0x87F1
#define IDM_PARSER_PARSEPROJ    0x87F2
#define IDM_PARSER_OPENDBASE    0x87F3
#define IDM_PARSER_CLOSEDBASE   0x87F4

#define ID_DELETE_LINE                  0x8800
#define ID_EDIT_REPLACE_DLG             0x8801
#define ID_NEXT_WINDOW          0x8802
#define ID_PREV_WINDOW          0x8803
#define ID_EDIT_BRF_FIND_FWD    0x8804
#define ID_EDIT_BRF_FIND_BACK   0x8805
#define ID_TOGGLE_RESTORE               0x8806
#define ID_SEARCH_AGAIN                 0x8807

#define IDM_CURSOR_CHARLEFT     0x8808
#define IDM_CURSOR_CHARRIGHT    0x8809
#define IDM_CURSOR_LINEUP               0x880A
#define IDM_CURSOR_LINEDOWN             0x880B
#define IDM_CURSOR_WORDLEFT             0x880C
#define IDM_CURSOR_WORDRIGHT    0x880D
#define IDM_CURSOR_SCREENUP             0x880E
#define IDM_CURSOR_SCREENDOWN   0x880F
#define IDM_EPSILON_ESCAPE              0x8810
#define IDM_CURSOR_LINESTART    0x8811
#define IDM_CURSOR_LINEEND              0x8812
#define IDM_CURSOR_FILESTART    0x8813
#define IDM_CURSOR_FILEEND              0x8814
#define IDM_CURSOR_BEGINPANE    0x8815
#define IDM_CURSOR_ENDPANE              0x8816
#define IDM_CURSOR_LEFTPANE             0x8817
#define IDM_CURSOR_RIGHTPANE    0x8818
#define IDM_CURSOR_BEGINLINE    0x8819
#define IDM_CURSOR_BRIEFHOME    0x881A
#define IDM_CURSOR_BRIEFEND             0x881B
#define IDM_TABIFYREGION        0x881C
#define IDM_UNTABIFYREGION      0x881D
#define IDM_HIGHLIGHTREGION             0x881E
#define IDM_EPSILON_INDENT_REGION       0x881F
#define IDM_EPSILON_KILLREGION  0x8820
#define IDM_EDIT_DELETEBRIEFEOLN        0x8821
#define IDM_EPSILON_FWDPARAGRAPH        0x8822
#define IDM_EPSILON_BCKPARAGRAPH        0x8823
#define IDM_EPSILON_FWDSENTENCE 0x8824
#define IDM_EPSILON_BCKSENTENCE 0x8825
#define IDM_EPSILON_DELSENTENCE 0x8826
#define IDM_EDIT_DELETEBRIEFBOLN        0x8827
#define IDM_EPSILON_TRANSPOSECH 0x8828
#define IDM_EPSILON_TRANSPOSEWD 0x8829
#define IDM_EPSILON_TRANSPOSELN 0x882A
#define IDM_EPSILON_CAPWORD             0x882B
#define IDM_EPSILON_LOWERWORD   0x882C
#define IDM_EPSILON_UPPERWORD   0x882D
#define ID_CMD_INVALID                  0x882E
#define IDM_NEWLINEBELOW                0x882F
#define ID_EDIT_REDOCHANGES             0x8830
#define ID_EDIT_UNDOCHANGES             0x8831
#define IDM_EPSILON_MARKPARAGRAPH       0x8832
#define IDM_EPSILON_CENTERWINDOW        0x8833
#define IDM_EPSILON_DELHORZSPACE        0x8834
#define IDM_EPSILON_DELBLANKLINES       0x8835
#define IDM_EDIT_TOGGLE_CASE    0x8836
#define IDM_EDIT_TOGGLE_RE              0x8837
#define IDM_EDIT_TOGGLE_WORD    0x8838
#define IDM_EPSILON_APPENDNEXTKILL      0x8839
#define IDM_EPSILON_TOINDENT    0x883A
#define IDM_FORMAT_REGION               0x883B
#define IDM_INDENT_UNDER                0x883C
#define IDM_INSERT_FROM_FILE    0x883D
#define IDM_REGEX_SRCH_FWD              0x883E
#define IDM_REGEX_SRCH_BCK              0x883F
#define IDM_BP_CURRENT_ADDR                             0x8840
#define IDM_BP_CURRENT_LINE                             0x8841
#define IDM_BP_CURRENT_FUNC                             0x8842
#define IDM_BP_ADVANCED                                 0x8843
#define ID_EDIT_RE_REPLACE                              0x8844
#define IDM_SEARCH_ISEARCH_FWD                  0x8845
#define IDM_SEARCH_ISEARCH_BACK                 0x8846
#define IDM_SEARCH_ISEARCH_REFWD        0x8847
#define IDM_SEARCH_ISEARCH_REBACK       0x8848
#define IDM_ISEARCH_ISEARCH_FWD         0x8849
#define IDM_ISEARCH_ISEARCH_BACK        0x884A
#define IDM_ISEARCH_TOGGLE_CASE         0x884B
#define IDM_ISEARCH_TOGGLE_WORD         0x884C
#define IDM_ISEARCH_TOGGLE_REGEX        0x884D
#define ID_QUOTED_INSERT                                0x884E
#define ID_CHANGE_PANE_UP                               0x884F
#define ID_CHANGE_PANE_DOWN                             0x8850
#define ID_CHANGE_PANE_LEFT                             0x8851
#define ID_CHANGE_PANE_RIGHT                    0x8852
#define ID_DELETE_ROW_UP                                0x8853
#define ID_DELETE_ROW_DOWN                              0x8854
#define ID_DELETE_COL_LEFT                              0x8855
#define ID_DELETE_COL_RIGHT                             0x8856
#define ID_LINE_TO_TOP                                  0x8857
#define ID_LINE_TO_BOTTOM                               0x8858

#define ID_FILESET_FIRST                                                                0x8859
#define ID_FILESET0                                                                             ID_FILESET_FIRST + 0
#define ID_FILESET1                                                                             ID_FILESET_FIRST + 1
#define ID_FILESET2                                                                             ID_FILESET_FIRST + 2
#define ID_FILESET3                                                                             ID_FILESET_FIRST + 3
#define ID_FILESET4                                                                             ID_FILESET_FIRST + 4
#define ID_FILESET5                                                                             ID_FILESET_FIRST + 5
#define ID_FILESET6                                                                             ID_FILESET_FIRST + 6
#define ID_FILESET7                                                                             ID_FILESET_FIRST + 7
#define ID_FILESET8                                                                             ID_FILESET_FIRST + 8
#define ID_FILESET9                                                                             ID_FILESET_FIRST + 9
#define ID_FILESET10                                                                    ID_FILESET_FIRST + 10
#define ID_FILESET11                                                                    ID_FILESET_FIRST + 11
#define ID_FILESET12                                                                    ID_FILESET_FIRST + 12
#define ID_FILESET13                                                                    ID_FILESET_FIRST + 13
#define ID_FILESET14                                                                    ID_FILESET_FIRST + 14
#define ID_FILESET15                                                                    ID_FILESET_FIRST + 15
#define ID_FILESET_LAST                                                                 0x8868
#define ID_EDIT_BACKWARD_LEVEL                  0x8869
#define ID_EDIT_FORWARD_LEVEL                   0x8870
#define ID_EDIT_KILL_LEVEL                              0x8871
#define ID_EDIT_BACKWARD_KILL_LEVEL             0x8872

#define IDM_STRING_TO_RESOURCE          0x8873

/////////////////////////////////////////////////////////////////////
// MSIN Commands
//
#define IDM_MSIN                        0x8a00
#define ID_SEARCH                       0x8a01
#define ID_QUERY_RESULTS                0x8a02
#define ID_SELECT_TITLE                 0x8a10
#define ID_SELECT_SUBSET                0x8a11
#define ID_FIND_FIND                    0x8a12
#define ID_FIND_KEYWORD                 0x8a13
#define ID_RESULTS_FIND                 0x8a14
#define ID_RESULTS_HISTORY              0x8a15
#define ID_BACK                         0x8a16
#define ID_NEXT                         0x8a17
#define ID_PREV                         0x8a18
#define ID_FIND_NEXT                    0x8a19
#define ID_BOOK_NEXT                    0x8a1a
#define ID_FIND_PREV                    0x8a1b
#define ID_BOOK_PREV                    0x8a1c
#define ID_PRINT_TOPIC                  0x8a1d
#define ID_PRINT_TOC                    0x8a1e
#define ID_SYNC                         0x8a1f
#define ID_TOGGLE_HIGHLIGHTS            0x8a20
#define ID_ADD_BOOKMARK                 0x8a21
#define ID_EDIT_BOOKMARK                0x8a22
#define ID_WHATSNEW                     0x8a24
#define ID_COPYTOPIC                    0x8a25
#define ID_SELECTION_RIGHT              0x8a26
#define ID_SELECTION_LEFT               0x8a27
#define ID_SELECTION_UP                 0x8a28
#define ID_SELECTION_DOWN               0x8a29
#define ID_SELECTION_ENTIRE             0x8a2a
#define ID_SEEALSO                      0x8a2b
#define ID_FIRST_TOPIC                  0x8a2c
#define ID_HOME_SCREEN                  0x8a2d
#define ID_EDIT_HELPBOOKMARKS           0x8a2e
#define ID_EDIT_ANNOTATION              0x8a30
#define ID_ADD_ANNOTATION               0x8a31
#define ID_NEXT_BOOKMARK                0x8a32
#define ID_PREV_BOOKMARK                0x8a33
#define ID_NEXT_LIST                    0x8a34
#define ID_PREV_LIST                    0x8a35
#define ID_SHOWBAR                      0x8a37
#define ID_SHOWNSR                      0x8a38
#define ID_DISPLAY_OPTIONS              0x8a39
#define ID_FIND_OPTIONS                 0x8a3a
#define ID_DEFINE_SUBSETS               0x8a3b
#define ID_SWITCHMVB                    0x8a3c
#define ID_NEXT_BOOKMARK_LIST           0x8a3d
#define ID_PREV_BOOKMARK_LIST           0x8a3e
#define ID_HELP_QUERY_RESULTS           0x8a3f
#define ID_HELP_BOOKMARK_LIST           0x8a40
#define ID_NEXT_JUMP                    0x8a41
#define ID_SEEALSOP                     0x8a42
#define ID_BACKP                        0x8a43
#define ID_HELPP                        0x8a44
#define ID_TOPIC                        0x8a45
#define ID_MSIN_OPEN                    0x8a46
#define ID_MSIN_ABOUT                   0x8a47
#define ID_NEXT_HIGHLIGHT               0x8b48
#define ID_PREV_HIGHLIGHT               0x8b49
#define ID_MSIN_PRINT_SETUP             0x8b4a
#define ID_HELP_KEYBOARD				0x8a4b

// The following commands are handled by the MSIN macro system
//
#define IDM_MSIN_MACRO_FIRST    0x8b00  //ID of first one
#define IDM_NEXT_HIGHLIGHT              0x8b00
#define IDM_PREV_HIGHLIGHT              0x8b01
#define IDM_ANNOTATION                  0x8b02
#define IDM_MSIN_MACRO_LAST             0x8b02  //ID of last one


//////////////////////////////////////////////////////////
//
// New IV command defines here.
//
#define ID_IV_TOPIC                        MIN_IV_COMMAND_ID + 1
#define ID_IV_URL_EC                       MIN_IV_COMMAND_ID + 2
#define ID_IV_OPEN_URL                     MIN_IV_COMMAND_ID + 3 
#define ID_IV_SYNC                         MIN_IV_COMMAND_ID + 4 
#define ID_IV_BACK                         MIN_IV_COMMAND_ID + 5 
#define ID_IV_NEXT                         MIN_IV_COMMAND_ID + 6 
#define ID_IV_PREV                         MIN_IV_COMMAND_ID + 7
#define ID_IV_FORWARD                      MIN_IV_COMMAND_ID + 8
#define ID_IV_REFRESH                      MIN_IV_COMMAND_ID + 9
#define ID_IV_STOP_DOWNLOAD                MIN_IV_COMMAND_ID + 10
#define ID_IV_LIST_BOOKMARKS               MIN_IV_COMMAND_ID + 11
#define ID_IV_ADD_BOOKMARK                 MIN_IV_COMMAND_ID + 12
#define ID_IV_PREV_BOOKMARK                MIN_IV_COMMAND_ID + 13
#define ID_IV_NEXT_BOOKMARK                MIN_IV_COMMAND_ID + 14
#define ID_IV_DEF_SUBSET                   MIN_IV_COMMAND_ID + 15


/////////////////////////////////////////////////////////////////////
// Wizard Commands
//
#define IDM_WIZARD_FIRST                0x8900
#define IDM_WIZARDBAR_VISIBLE			(IDM_WIZARD_FIRST+0)
#define IDM_ADDCLASS                    (IDM_WIZARD_FIRST+1)
#define IDM_GALLERY						(IDM_WIZARD_FIRST+2)
#define ID_WIZBAR_HEADERFILE            (IDM_WIZARD_FIRST+3)
// michma 2/20/98 - this isn't inherited from the ide but just a command we needed.
#define IDM_NEW_ATL_OBJECT				(IDM_WIZARD_FIRST+4)
#define IDM_WIZARD_LAST                 0x89ff

// Commands which are dynamically bound to actions via the CDynCT instance in CTheApp.
#define IDM_DYNAMIC_BASE                0x8d00
#define IDM_DYNAMIC_LAST                0x8eff


/////////////////////////////////////////////////////////////////////
// Project Window Commands
//
#define IDM_PRJWIN_FIRST                        0x8f00
#define IDM_ADDFOLDER               (IDM_PRJWIN_FIRST+0)
#define IDM_ADDSUBFOLDER            (IDM_PRJWIN_FIRST+1)
#define IDM_ADDMENU                 (IDM_PRJWIN_FIRST+2)
#define IDM_ADDSELECTEDNODE         (IDM_PRJWIN_FIRST+3)
#define IDM_ADDPANE                 (IDM_PRJWIN_FIRST+4)
#define IDM_DELETEPANE              (IDM_PRJWIN_FIRST+5)
#define IDM_ADDPROVIDEDNODE1        (IDM_PRJWIN_FIRST+6)
#define IDM_ADDPROVIDEDNODE2        (IDM_PRJWIN_FIRST+7)
#define IDM_ADDPROVIDEDNODE3        (IDM_PRJWIN_FIRST+8)
#define IDM_ADDPROVIDEDNODE4        (IDM_PRJWIN_FIRST+9)
#define IDM_ADDPROVIDEDNODE5        (IDM_PRJWIN_FIRST+10)
#define IDM_ADDPROVIDEDNODE6        (IDM_PRJWIN_FIRST+11)
#define IDM_ADDPROVIDEDNODE7        (IDM_PRJWIN_FIRST+12)
#define IDM_ADDPROVIDEDNODE8        (IDM_PRJWIN_FIRST+13)
#define IDM_ADDPROVIDEDNODE9        (IDM_PRJWIN_FIRST+14)
#define IDM_ADDPROVIDEDNODE10       (IDM_PRJWIN_FIRST+15)
#define IDM_ADDPROVIDEDNODE11       (IDM_PRJWIN_FIRST+16)
#define IDM_ADDPROVIDEDNODE12       (IDM_PRJWIN_FIRST+17)
#define IDM_ADDPROVIDEDNODE13       (IDM_PRJWIN_FIRST+18)
#define IDM_ADDPROVIDEDNODE14       (IDM_PRJWIN_FIRST+19)
#define IDM_ADDPROVIDEDNODE15       (IDM_PRJWIN_FIRST+20)
#define IDM_ADDPROVIDEDNODE16       (IDM_PRJWIN_FIRST+21)
#define IDM_SLOB_DEFAULTCMD         (IDM_PRJWIN_FIRST+22)
#define IDM_TOGGLE_TITLETIPS        (IDM_PRJWIN_FIRST+23)
#define IDM_OPEN_WORKSPACE          (IDM_PRJWIN_FIRST+24)
#define IDM_SAVE_WORKSPACE			(IDM_PRJWIN_FIRST+25)
#define IDM_CLOSE_WORKSPACE         (IDM_PRJWIN_FIRST+26)
// Begin Class View specific
#define IDM_CLSVIEW_GOTO_DECL       (IDM_PRJWIN_FIRST+50)
#define IDM_CLSVIEW_DERIVED_CLASSES (IDM_PRJWIN_FIRST+51)
#define IDM_CLSVIEW_BASE_CLASSES    (IDM_PRJWIN_FIRST+52)
#define IDM_CLSVIEW_REFS                    (IDM_PRJWIN_FIRST+53)
#define IDM_CLSVIEW_CALLED_FUNCS    (IDM_PRJWIN_FIRST+54)
#define IDM_CLSVIEW_CALLING_FUNCS   (IDM_PRJWIN_FIRST+55)
#define IDM_CLSVIEW_ADD_MF          (IDM_PRJWIN_FIRST+56)
#define IDM_CLSVIEW_ADD_MV          (IDM_PRJWIN_FIRST+57)
#define IDM_GROUPBYACCESS			(IDM_PRJWIN_FIRST+58)
// End Class View specific
// Begin Source Control specific
#define IDM_SCC_FIRST               (IDM_PRJWIN_FIRST+80)
#define IDM_SCC_POP                 (IDM_SCC_FIRST+0)
#define IDM_SCC_GET                 (IDM_SCC_FIRST+1)
#define IDM_SCC_OUT                 (IDM_SCC_FIRST+2)
#define IDM_SCC_IN                  (IDM_SCC_FIRST+3)
#define IDM_SCC_UNOUT               (IDM_SCC_FIRST+4)
#define IDM_SCC_ADD                 (IDM_SCC_FIRST+5)
#define IDM_SCC_REMOVE              (IDM_SCC_FIRST+6)
#define IDM_SCC_HISTORY             (IDM_SCC_FIRST+7)
#define IDM_SCC_DIFF                (IDM_SCC_FIRST+8)
#define IDM_SCC_PROPS               (IDM_SCC_FIRST+9)
#define IDM_SCC_SHARE    			(IDM_SCC_FIRST+10)
#define IDM_SCC_ADMIN               (IDM_SCC_FIRST+11)
#define IDM_SCC_LAST                (IDM_SCC_FIRST+19) // (IDM_PRJWIN_FIRST+99)
// End Source Control specific
#define IDM_PRJWIN_LAST                 0x8fff  // (IDM_PRJWIN_FIRST + 255)

