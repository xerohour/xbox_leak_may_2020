//-----------------------------------------------------------------------------
// Microsoft Visual Studio
//
// Microsoft Confidential
// Copyright 1995-1997 Microsoft Corporation.  All Rights Reserved.
//
// File: stdidcmd.h
// Area: IOleCommandTarget and IOleComponentUIManager
//
// Contents:
//   Contains ids used for commands used in StandardCommandSet97.
//   StandardCommandSet97 is defined by the following guid:
//
//   {5efc7975-14bc-11cf-9b2b-00aa00573819}
//   DEFINE_GUID(CLSID_StandardCommandSet97,
//               0x5efc7975, 0x14bc, 0x11cf, 0x9b, 0x2b, 0x00, 0xaa, 0x00, 
//               0x57, 0x38, 0x19);
//-----------------------------------------------------------------------------

#ifndef _STDIDCMD_H_                  
#define _STDIDCMD_H_                  

#ifndef __CTC__

// for specialized contracts
enum
  {
  CMD_ZOOM_PAGEWIDTH	   = -1,
  CMD_ZOOM_ONEPAGE	   = -2,
  CMD_ZOOM_TWOPAGES	   = -3,
  CMD_ZOOM_SELECTION	   = -4,
  CMD_ZOOM_FIT		   = -5
  };

#endif //__CTC__

#define cmdidAlignBottom		1                                                                   
#define cmdidAlignHorizontalCenters     2                                                        
#define cmdidAlignLeft			3                                                                     
#define cmdidAlignRight			4                                                                    
#define cmdidAlignToGrid		5                                                                   
#define cmdidAlignTop			6                                                                      
#define cmdidAlignVerticalCenters       7                                                          
#define cmdidArrangeBottom		8                                                                 
#define cmdidArrangeRight		9                                                                  
#define cmdidBringForward		10                                                                 
#define cmdidBringToFront		11                                                                 
#define cmdidCenterHorizontally         12                                                           
#define cmdidCenterVertically           13                                                             
#define cmdidCode			14                                                                         
#define cmdidCopy			15                                                                         
#define cmdidCut			16                                                                          
#define cmdidDelete			17                                                                       
#define cmdidFontName			18                                                                     
#define cmdidFontSize			19                                                                     
#define cmdidGroup			20                                                                        
#define cmdidHorizSpaceConcatenate      21                                                        
#define cmdidHorizSpaceDecrease         22                                                           
#define cmdidHorizSpaceIncrease         23                                                           
#define cmdidHorizSpaceMakeEqual        24                                                          
#define cmdidInsertObject		25                                                                 
#define cmdidPaste			26                                                                        
#define cmdidPrint			27                                                                        
#define cmdidProperties			28                                                                   
#define cmdidRedo			29                                                                         
#define cmdidMultiLevelRedo		30                                                               
#define cmdidSelectAll			31                                                                    
#define cmdidSendBackward		32                                                                 
#define cmdidSendToBack			33                                                                   
#define cmdidShowTable			34                                                                    
#define cmdidSizeToControl		35                                                                
#define cmdidSizeToControlHeight        36                                                          
#define cmdidSizeToControlWidth         37                                                           
#define cmdidSizeToFit			38                                                                    
#define cmdidSizeToGrid			39                                                                   
#define cmdidSnapToGrid			40                                                                   
#define cmdidTabOrder			41                                                                     
#define cmdidToolbox			42                                                                      
#define cmdidUndo			43                                                                         
#define cmdidMultiLevelUndo		44                                                               
#define cmdidUngroup			45                                                                      
#define cmdidVertSpaceConcatenate       46                                                         
#define cmdidVertSpaceDecrease          47                                                            
#define cmdidVertSpaceIncrease          48                                                            
#define cmdidVertSpaceMakeEqual         49                                                           
#define cmdidZoomPercent		50                                                                  
#define cmdidBackColor			51                                                                    
#define cmdidBold			52                                                                         
#define cmdidBorderColor		53                                                                  
#define cmdidBorderDashDot		54                                                                
#define cmdidBorderDashDotDot           55                                                             
#define cmdidBorderDashes		56                                                                 
#define cmdidBorderDots			57                                                                   
#define cmdidBorderShortDashes          58                                                            
#define cmdidBorderSolid		59                                                                  
#define cmdidBorderSparseDots           60                                                             
#define cmdidBorderWidth1		61                                                                 
#define cmdidBorderWidth2		62                                                                 
#define cmdidBorderWidth3		63                                                                 
#define cmdidBorderWidth4		64                                                                 
#define cmdidBorderWidth5		65                                                                 
#define cmdidBorderWidth6		66                                                                 
#define cmdidBorderWidthHairline        67                                                          
#define cmdidFlat			68                                                                         
#define cmdidForeColor			69                                                                    
#define cmdidItalic			70                                                                       
#define cmdidJustifyCenter		71                                                                
#define cmdidJustifyGeneral		72                                                               
#define cmdidJustifyLeft		73                                                                  
#define cmdidJustifyRight		74                                                                 
#define cmdidRaised			75                                                                       
#define cmdidSunken			76                                                                       
#define cmdidUnderline			77                                                                    
#define cmdidChiseled			78                                                                     
#define cmdidEtched			79                                                                       
#define cmdidShadowed			80                                                                     
#define cmdidCompDebug1			81                                                                   
#define cmdidCompDebug2			82                                                                   
#define cmdidCompDebug3			83                                                                   
#define cmdidCompDebug4			84                                                                   
#define cmdidCompDebug5			85                                                                   
#define cmdidCompDebug6			86                                                                   
#define cmdidCompDebug7			87                                                                   
#define cmdidCompDebug8			88                                                                   
#define cmdidCompDebug9			89                                                                   
#define cmdidCompDebug10		90                                                                  
#define cmdidCompDebug11		91                                                                  
#define cmdidCompDebug12		92                                                                  
#define cmdidCompDebug13		93                                                                  
#define cmdidCompDebug14		94                                                                  
#define cmdidCompDebug15		95                                                                  
#define cmdidExistingSchemaEdit         96                                                           
#define cmdidFind			97                                                                         
#define cmdidGetZoom			98                                                                      
#define cmdidQueryOpenDesign		99                                                              
#define cmdidQueryOpenNew		100                                                                
#define cmdidSingleTableDesign          101                                                           
#define cmdidSingleTableNew		102                                                              
#define cmdidShowGrid			103
#define cmdidNewTable			104
#define cmdidCollapsedView		105
#define cmdidFieldView			106
#define cmdidVerifySQL			107
#define cmdidHideTable			108

#define cmdidPrimaryKey			109
#define cmdidSave			110
#define cmdidSaveAs			111
#define cmdidSortAscending		112

#define cmdidSortDescending		113
#define cmdidAppendQuery		114
#define cmdidCrosstabQuery		115
#define cmdidDeleteQuery		116
#define cmdidMakeTableQuery		117

#define cmdidSelectQuery		118
#define cmdidUpdateQuery		119
#define cmdidParameters			120
#define cmdidTotals			121
#define cmdidViewCollapsed		122

#define cmdidViewFieldList		123


#define cmdidViewKeys			124
#define cmdidViewGrid			125
#define cmdidInnerJoin			126

#define cmdidRightOuterJoin		127
#define cmdidLeftOuterJoin		128
#define cmdidFullOuterJoin		129
#define cmdidUnionJoin			130
#define cmdidShowSQLPane		131

#define cmdidShowGraphicalPane          132
#define cmdidShowDataPane		133
#define cmdidShowQBEPane		134
#define cmdidSelectAllFields		135

#define cmdidOLEObjectMenuButton        136
#define cmdidObjectVerbList0		137
#define cmdidObjectVerbList1		138
#define cmdidObjectVerbList2		139
#define cmdidObjectVerbList3		140
					
#define cmdidObjectVerbList4		141
#define cmdidObjectVerbList5		142
#define cmdidObjectVerbList6		143
#define cmdidObjectVerbList7		144
#define cmdidObjectVerbList8		145

#define cmdidObjectVerbList9		146
#define cmdidConvertObject		147
#define cmdidCustomControl		148
#define cmdidCustomizeItem		149
#define cmdidRename			150

#define cmdidImport			151
#define cmdidNewPage			152
#define cmdidMove			153
#define cmdidCancel			154

#define cmdidFont			155

#define cmdidExpandLinks		156
#define cmdidExpandImages		157
#define cmdidExpandPages		158
#define cmdidRefocusDiagram 		159
#define cmdidTransitiveClosure		160
#define cmdidCenterDiagram		161
#define cmdidZoomIn 			162
#define cmdidZoomOut			163
#define cmdidRemoveFilter		164
#define cmdidHidePane			165
#define cmdidDeleteTable		166
#define cmdidDeleteRelationship		167
#define cmdidRemove 			168
#define cmdidJoinLeftAll		169
#define cmdidJoinRightAll		170
#define cmdidAddToOutput		171 	// Add selected fields to query output
#define cmdidOtherQuery 		172 	// change query type to 'other'
#define cmdidGenerateChangeScript	173
#define cmdidSaveSelection		174	// Save current selection
#define cmdidAutojoinCurrent		175	// Autojoin current tables
#define cmdidAutojoinAlways 		176	// Toggle Autojoin state
#define cmdidEditPage			177	// Launch editor for url
#define cmdidViewLinks			178	// Launch new webscope for url
#define cmdidStop			179	// Stope webscope rendering
#define cmdidPause			180	// Pause webscope rendering
#define cmdidResume			181	// Resume webscope rendering
#define cmdidFilterDiagram		182	// Filter webscope diagram
#define cmdidShowAllObjects		183	// Show All objects in webscope diagram
#define cmdidShowApplications		184	// Show Application objects in webscope diagram
#define cmdidShowOtherObjects		185	// Show other objects in webscope diagram
#define cmdidShowPrimRelationships	186	// Show primary relationships
#define cmdidExpand			187	// Expand links
#define cmdidCollapse			188	// Collapse links
#define cmdidRefresh			189	// Refresh Webscope diagram
#define cmdidLayout			190	// Layout websope diagram
#define cmdidShowResources		191	// Show resouce objects in webscope diagram
#define cmdidInsertHTMLWizard		192	// Insert HTML using a Wizard
#define cmdidShowDownloads		193	// Show download objects in webscope diagram
#define cmdidShowExternals		194	// Show external objects in webscope diagram
#define cmdidShowInBoundLinks		195	// Show inbound links in webscope diagram
#define cmdidShowOutBoundLinks		196	// Show out bound links in webscope diagram
#define cmdidShowInAndOutBoundLinks	197	// Show in and out bound links in webscope diagram
#define cmdidPreview			198	// Preview page
#define cmdidOpen			261	// Open
#define cmdidOpenWith			199	// Open with
#define cmdidShowPages			200	// Show HTML pages
#define cmdidRunQuery			201 	// Runs a query
#define cmdidClearQuery 		202 	// Clears the query's associated cursor
#define cmdidRecordFirst		203 	// Go to first record in set
#define cmdidRecordLast 		204 	// Go to last record in set
#define cmdidRecordNext 		205 	// Go to next record in set
#define cmdidRecordPrevious 		206 	// Go to previous record in set
#define cmdidRecordGoto 		207 	// Go to record via dialog
#define cmdidRecordNew			208 	// Add a record to set

#define cmdidInsertNewMenu		209	// menu designer
#define cmdidInsertSeparator		210	// menu designer
#define cmdidEditMenuNames		211	// menu designer

#define cmdidDebugExplorer		212 
#define cmdidDebugProcesses		213
#define cmdidViewThreadsWindow		214
#define cmdidWindowUIList		215

// ids on the file menu
#define cmdidNewSolution		216
#define cmdidOpenSolution		217
#define cmdidCloseSolution		218
#define cmdidAddProject			219
#define cmdidRemoveProject		220
#define cmdidFileNew			221
#define cmdidFileOpen			222
#define cmdidFileClose			223
#define cmdidSaveSolution		224
#define cmdidSaveSolutionAs		225
#define cmdidSaveProjectItemAs		226
#define cmdidPageSetup			227
#define cmdidPrintPreview		228
#define cmdidExit			229

// ids on the edit menu
#define cmdidReplace			230
#define cmdidGoto			231

// ids on the view menu
#define cmdidPropertyPages		232
#define cmdidFullScreen			233
#define cmdidProjectExplorer		234
#define cmdidPropertiesWindow		235
#define cmdidTaskListWindow		236
#define cmdidOutputWindow		237
#define cmdidObjectBrowser		238
#define cmdidDocOutlineWindow	239
#define cmdidImmediateWindow		240
#define cmdidWatchWindow		241
#define cmdidLocalsWindow		242
#define cmdidCallStack			243
#define cmdidAutosWindow		596

// ids on project menu
#define cmdidAddItem			244
#define cmdidNewFolder			245
#define cmdidSetStartupProject		246
#define cmdidProjectSettings		247
#define cmdidProjectReferences          367

// ids on the debug menu
#define cmdidStepInto			248
#define cmdidStepOver			249
#define cmdidStepOut			250
#define cmdidRunToCursor		251
#define cmdidAddWatch			252
#define cmdidEditWatch			253
#define cmdidQuickWatch			254
#define cmdidToggleBreakpoint		255
#define cmdidClearBreakpoints		256
#define cmdidShowBreakpoints		257
#define cmdidSetNextStatement		258
#define cmdidShowNextStatement		259
#define cmdidEditBreakpoint             260
#define cmdidDetachDebugger             262

// ids on the tools menu
#define cmdidToolsOptions		264

// ids on the windows menu
#define cmdidNewWindow			265
#define cmdidSplit			266
#define cmdidCascade			267
#define cmdidTileHorz			268
#define cmdidTileVert			269

// ids on the help menu
#define cmdidTechSupport		270

// NOTE cmdidAbout and cmdidDebugOptions must be consecutive
//      cmd after cmdidDebugOptions (ie 273) must not be used
#define cmdidAbout			271
#define cmdidDebugOptions		272

// ids on the watch context menu
// CollapseWatch appears as 'Collapse Parent', on any
// non-top-level item
#define cmdidDeleteWatch		274
#define cmdidCollapseWatch		275

// ids on the property browser context menu
#define cmdidPbrsToggleStatus		282
#define cmdidPropbrsHide		283

// ids on the docking context menu
#define cmdidDockingView		284
#if DCR27419
#define cmdidDockingViewMDI		290
#define cmdidDockingViewFloater		291
#endif
#define cmdidHideActivePane		285

// ids for window selection via keyboard
//#define cmdidPaneNextPane		316 (listed below in order)
//#define cmdidPanePrevPane		317 (listed below in order)
#define cmdidPaneNextTab		286
#define cmdidPanePrevTab		287
#define cmdidPaneCloseToolWindow	288
#define cmdidPaneActivateDocWindow	289

#define cmdidStart			295
#define cmdidRestart			296

#define cmdidAddinManager		297

#define cmdidMultiLevelUndoList		298
#define cmdidMultiLevelRedoList		299

#define cmdidToolboxAddTab		300
#define cmdidToolboxDeleteTab		301
#define cmdidToolboxRenameTab		302  
#define cmdidToolboxTabMoveUp		303
#define cmdidToolboxTabMoveDown		304
#define cmdidToolboxDeleteItem		305
#define cmdidToolboxRenameItem		306
#define cmdidToolboxListView		307

#define cmdidWindowUIGetList		308
#define cmdidInsertValuesQuery		309

#define cmdidShowProperties		310

#define cmdidThreadSuspend		311
#define cmdidThreadResume		312
#define cmdidThreadSetFocus		313
#define cmdidDisplayRadix		314

#define cmdidOpenProjectItem		315

#define cmdidPaneNextPane		316
#define cmdidPanePrevPane		317

#define cmdidClearPane			318
#define cmdidGotoErrorTag		319

#define cmdidTaskListSortByCategory	320
#define cmdidTaskListSortByFileLine	321
#define cmdidTaskListSortByPriority	322

#define cmdidTaskListFilterByNothing	325
#define cmdidTaskListFilterByCategoryCodeSense	326
#define cmdidTaskListFilterByCategoryCompiler	327
#define cmdidTaskListFilterByCategoryComment	328

#define cmdidToolboxAddItem		329
#define cmdidToolboxReset		330

#define cmdidSaveProjectItem		331
#define cmdidViewForm			332
#define cmdidViewCode			333
#define cmdidPreviewInBrowser		334
#define cmdidBrowseWith			336
#define cmdidSearchCombo		337
#define cmdidEditLabel	  		338

#define cmdidJavaExceptions     339
#define cmdidDefineViews		340

#define cmdidToggleSelMode		341
#define cmdidToggleInsMode		342

#define cmdidLoadUnloadedProject	343
#define cmdidUnloadLoadedProject	344

// ids on the treegrids (watch/local/threads/stack)
#define cmdidElasticColumn	345
#define cmdidHideColumn			346

#define cmdidTaskListPreviousView	347
#define cmdidZoomDialog			348

// find/replace options
#define cmdidFindNew                    349
#define cmdidFindMatchCase              350
#define cmdidFindWholeWord              351
#define cmdidFindRegularExpression      352
#define cmdidFindBackwards              353
#define cmdidFindInSelection            354
#define cmdidFindStop                   355
#define cmdidFindHelp                   356

#define cmdidTaskListNextError				357
#define cmdidTaskListPrevError				358
#define cmdidTaskListFilterByCategoryUser		359
#define cmdidTaskListFilterByCategoryShortcut		360
#define cmdidTaskListFilterByCategoryHTML		361
#define cmdidTaskListFilterByCurrentFile		362
#define cmdidTaskListFilterByChecked			363
#define cmdidTaskListFilterByUnchecked			364
#define cmdidTaskListSortByDescription			365
#define cmdidTaskListSortByChecked			366

// 367 is used above in cmdidProjectReferences
#define cmdidStartNoDebug		368

#define cmdidFindNext                   370
#define cmdidFindPrev                   371
#define cmdidFindSelectedNext           372
#define cmdidFindSelectedPrev           373
#define cmdidSearchGetList              374
#define cmdidInsertBreakpoint		375
#define cmdidEnableBreakpoint		376
#define cmdidF1Help			377

// Object Browser commands
#define cmdidOBF1Help			378
#define cmdidObjectBrowserHelp		379
#define cmdidOBShowClasses		380
#define cmdidOBShowMembers		381
#define cmdidOBPackageView		382
#define cmdidOBCopy     		383
#define cmdidOBSearchCombo		384
#define cmdidOBShowAll			385
#define cmdidOBSearchBeginningOfWord    386
#define cmdidOBGroupingDialog		387
#define cmdidOBGroupClasses		388
#define cmdidOBGroupMembers		389
#define cmdidOBGroupInTreeLeft		390
#define cmdidOBGroupInTreeRight		391
#define cmdidOBDefinition		392
#define cmdidOBShowHidden		393
#define cmdidOBProperties		394
#define cmdidOBSubsetsDialog		395
#define cmdidOBFilterFocus		396

#define cmdidPropSheetOrProperties	397

// NOTE - the next items are debug only !!
#define cmdidTshellStep                 398
#define cmdidTshellRun                  399

// marker commands on the codewin menu
#define cmdidMarkerCmd0                 400
#define cmdidMarkerCmd1                 401
#define cmdidMarkerCmd2                 402
#define cmdidMarkerCmd3                 403
#define cmdidMarkerCmd4                 404
#define cmdidMarkerCmd5                 405
#define cmdidMarkerCmd6                 406
#define cmdidMarkerCmd7                 407
#define cmdidMarkerCmd8                 408
#define cmdidMarkerCmd9                 409
#define cmdidMarkerLast                 409

// document outline commands
#define cmdidDetachAttachOutline		420
#define cmdidShowHideOutline			421
#define cmdidSyncOutline				422

#define cmdidRunToCallstCursor          423

// ids on the window menu - these must be sequential ie window1-morewind
#define cmdidWindow1			570
#define cmdidWindow2			571
#define cmdidWindow3			572
#define cmdidWindow4			573
#define cmdidWindow5			574
#define cmdidWindow6			575
#define cmdidWindow7			576
#define cmdidWindow8			577
#define cmdidWindow9			578
#define cmdidWindow10			579
#define cmdidWindow11			580
#define cmdidWindow12			581
#define cmdidWindow13			582
#define cmdidWindow14			583
#define cmdidWindow15			584
#define cmdidWindow16			585
#define cmdidWindow17			586
#define cmdidWindow18			587
#define cmdidWindow19			588
#define cmdidWindow20			589
#define cmdidWindow21			590
#define cmdidWindow22			591
#define cmdidWindow23			592
#define cmdidWindow24			593
#define cmdidWindow25			594   // note cmdidWindow25 is unused on purpose!
#define cmdidMoreWindows		595

//#define cmdidAutosWindow		596
#define cmdidTaskListDeleteTask		597
#define cmdidTaskListTaskHelp		598

#define cmdidMRUList1			600
#define cmdidMRUList2			601
#define cmdidMRUList3			602
#define cmdidMRUList4			603
#define cmdidMRUList5			604
#define cmdidMRUList6			605
#define cmdidMRUList7			606
#define cmdidMRUList8			607
#define cmdidMRUList9			608
#define cmdidMRUList10			609
#define cmdidMRUList11			610
#define cmdidMRUList12			611
#define cmdidMRUList13			612
#define cmdidMRUList14			613
#define cmdidMRUList15			614
#define cmdidMRUList16			615
#define cmdidMRUList17			616
#define cmdidMRUList18			617
#define cmdidMRUList19			618
#define cmdidMRUList20			619
#define cmdidMRUList21			620
#define cmdidMRUList22			621
#define cmdidMRUList23			622
#define cmdidMRUList24			623
#define cmdidMRUList25			624  // note cmdidMRUList25 is unused on purpose!

#define cmdidSplitNext			625
#define cmdidSplitPrev			626

#define cmdidStandardMax		700

///////////////////////////////////////////
//
// DON'T go beyond the cmdidStandardMax
// if you are adding shell commands.
//
// If you are not adding shell commands,
// you shouldn't be doing it in this file! 
//
///////////////////////////////////////////


#define cmdidFormsFirst           0x00006000

#define cmdidFormsLast           0x00006FFF

#define cmdidVBEFirst           0x00008000   

#define msotcidBookmarkWellMenu	    0x00008001

#define cmdidZoom200		    0x00008002
#define cmdidZoom150		    0x00008003
#define cmdidZoom100		    0x00008004
#define cmdidZoom75		    0x00008005
#define cmdidZoom50		    0x00008006
#define cmdidZoom25		    0x00008007
#define cmdidZoom10		    0x00008010

#define msotcidZoomWellMenu	    0x00008011
#define msotcidDebugPopWellMenu	    0x00008012
#define msotcidAlignWellMenu	    0x00008013
#define msotcidArrangeWellMenu	    0x00008014
#define msotcidCenterWellMenu	    0x00008015
#define msotcidSizeWellMenu	    0x00008016
#define msotcidHorizontalSpaceWellMenu 0x00008017
#define msotcidVerticalSpaceWellMenu 0x00008020

#define msotcidDebugWellMenu	    0x00008021
#define msotcidDebugMenuVB	    0x00008022

#define msotcidStatementBuilderWellMenu 0x00008023
#define msotcidProjWinInsertMenu    0x00008024
#define msotcidToggleMenu	    0x00008025
#define msotcidNewObjInsertWellMenu 0x00008026
#define msotcidSizeToWellMenu	    0x00008027
#define msotcidCommandBars	    0x00008028
#define msotcidVBOrderMenu	    0x00008029
#define msotcidMSOnTheWeb	    0x0000802A
#define msotcidVBDesignerMenu       0x00008030
#define msotcidNewProjectWellMenu   0x00008031
#define msotcidProjectWellMenu      0x00008032

#define msotcidVBCode1ContextMenu	  0x00008033
#define msotcidVBCode2ContextMenu	  0x00008034
#define msotcidVBWatchContextMenu	  0x00008035
#define msotcidVBImmediateContextMenu	  0x00008036
#define msotcidVBLocalsContextMenu	  0x00008037
#define msotcidVBFormContextMenu	  0x00008038  
#define msotcidVBControlContextMenu	  0x00008039  
#define msotcidVBProjWinContextMenu	  0x0000803A
#define msotcidVBProjWinContextBreakMenu  0x0000803B
#define msotcidVBPreviewWinContextMenu	  0x0000803C
#define msotcidVBOBContextMenu		  0x0000803D
#define msotcidVBForms3ContextMenu	  0x0000803E
#define msotcidVBForms3ControlCMenu	  0x0000803F
#define msotcidVBForms3ControlCMenuGroup  0x00008040
#define msotcidVBForms3ControlPalette	  0x00008041
#define msotcidVBForms3ToolboxCMenu	  0x00008042
#define msotcidVBForms3MPCCMenu		  0x00008043
#define msotcidVBForms3DragDropCMenu	  0x00008044
#define msotcidVBToolBoxContextMenu	  0x00008045
#define msotcidVBToolBoxGroupContextMenu  0x00008046
#define msotcidVBPropBrsHostContextMenu	  0x00008047
#define msotcidVBPropBrsContextMenu	  0x00008048
#define msotcidVBPalContextMenu		  0x00008049
#define msotcidVBProjWinProjectContextMenu  0x0000804A
#define msotcidVBProjWinFormContextMenu	    0x0000804B
#define msotcidVBProjWinModClassContextMenu 0x0000804C
#define msotcidVBProjWinRelDocContextMenu   0x0000804D
#define msotcidVBDockedWindowContextMenu    0x0000804E

#define msotcidVBShortCutForms		  0x0000804F
#define msotcidVBShortCutCodeWindows	  0x00008050
#define msotcidVBShortCutMisc		  0x00008051
#define msotcidVBBuiltInMenus		  0x00008052
#define msotcidPreviewWinFormPos          0x00008053

#define msotcidVBAddinFirst         0x00008200

#define cmdidVBELast           0x00009FFF 
                                         
#define cmdidSterlingFirst           0x0000A000                                    
#define cmdidSterlingLast           0x0000BFFF                                     

#define uieventidFirst                   0xC000
#define uieventidSelectRegion      0xC001
#define uieventidDrop                  0xC002
#define uieventidLast                   0xDFFF

#endif //_STDIDCMD_H_

