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
//
//   Contains ids used for commands used in StandardCommandSet2K.
//   StandardCommandSet2K is defined by the following guid:
//
//   {1496A755-94DE-11D0-8C3F-00C04FC2AAE2}
//  DEFINE_GUID(CMDSETID_StandardCommandSet2K,
//    0x1496A755, 0x94DE, 0x11D0, 0x8C, 0x3F, 0x00, 0xC0, 0x4F, 0xC2, 0xAA, 0xE2);
//
//  NOTE that new commands should be added to the end of StandardCommandSet2K
//  and that CLSID_StandardCommandSet97 should not be further added to.
//  NOTE also that in StandardCommandSet2K all commands up to ECMD_FINAL are
//  standard editor commands and have been moved here from editcmd.h.
//-----------------------------------------------------------------------------

#ifndef _STDIDCMD_H_                  
#define _STDIDCMD_H_                  

#ifndef __CTC__
#ifdef __cplusplus

// for specialized contracts
enum
  {
  CMD_ZOOM_PAGEWIDTH	   = -1,
  CMD_ZOOM_ONEPAGE	   = -2,
  CMD_ZOOM_TWOPAGES	   = -3,
  CMD_ZOOM_SELECTION	   = -4,
  CMD_ZOOM_FIT		   = -5
  };

#endif //__cplusplus
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
#define cmdidFontNameGetList			500                                                                     
#define cmdidFontSize			19                                                                     
#define cmdidFontSizeGetList			501                                                                     
#define cmdidGroup			20                                                                        
#define cmdidHorizSpaceConcatenate      21                                                        
#define cmdidHorizSpaceDecrease         22                                                           
#define cmdidHorizSpaceIncrease         23                                                           
#define cmdidHorizSpaceMakeEqual        24                                                          
#define cmdidLockControls               369
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

// ids on the ole verbs menu - these must be sequential ie verblist0-verblist9
#define cmdidObjectVerbList0		137
#define cmdidObjectVerbList1		138
#define cmdidObjectVerbList2		139
#define cmdidObjectVerbList3		140
#define cmdidObjectVerbList4		141
#define cmdidObjectVerbList5		142
#define cmdidObjectVerbList6		143
#define cmdidObjectVerbList7		144
#define cmdidObjectVerbList8		145
#define cmdidObjectVerbList9		146 // Unused on purpose!

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
#define cmdidNewProject			216
#define cmdidOpenProject		217
#define cmdidOpenProjectFromWeb		450
#define cmdidOpenSolution		218
#define cmdidCloseSolution		219
#define cmdidFileNew			221
#define cmdidFileOpen			222
#define cmdidFileOpenFromWeb		451
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
#define cmdidImmediateWindow	240
#define cmdidWatchWindow		241
#define cmdidLocalsWindow		242
#define cmdidCallStack			243
#define cmdidAutosWindow		cmdidDebugReserved1
#define cmdidThisWindow			cmdidDebugReserved2

// ids on project menu
#define cmdidAddNewItem			220
#define cmdidAddExistingItem		244
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
#define cmdidQuickWatch                 254

#define cmdidToggleBreakpoint		255
#define cmdidClearBreakpoints		256
#define cmdidShowBreakpoints		257
#define cmdidSetNextStatement		258
#define cmdidShowNextStatement		259
#define cmdidEditBreakpoint             260
#define cmdidDetachDebugger             262

// ids on the tools menu
#define cmdidCustomizeKeyboard		263
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
// ids 276, 277, 278, 279, 280 are in use
// below 
// ids on the property browser context menu
#define cmdidPbrsToggleStatus		282
#define cmdidPropbrsHide		283

// ids on the docking context menu
#define cmdidDockingView		284
#define cmdidHideActivePane		285
// ids for window selection via keyboard
//#define cmdidPaneNextPane		316 (listed below in order)
//#define cmdidPanePrevPane		317 (listed below in order)
#define cmdidPaneNextTab		286
#define cmdidPanePrevTab		287
#define cmdidPaneCloseToolWindow	288
#define cmdidPaneActivateDocWindow	289
#define cmdidDockingViewMDI		290
#define cmdidDockingViewFloater		291
#define cmdidAutoHideWindow		292
#define cmdidMoveToDropdownBar          293
#define cmdidFindCmd                    294 // internal Find commands
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
#define cmdidToolboxRenameItem		305
#define cmdidToolboxListView		306
//(below) cmdidSearchSetCombo		307

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
#define cmdidTaskListSortByDefaultSort  323
#define cmdidTaskListShowTooltip        324
#define cmdidTaskListFilterByNothing	325
#define cmdidCancelEZDrag               326
#define cmdidTaskListFilterByCategoryCompiler	327
#define cmdidTaskListFilterByCategoryComment	328

#define cmdidToolboxAddItem		329
#define cmdidToolboxReset		330

#define cmdidSaveProjectItem		331
#define cmdidSaveOptions                959
#define cmdidViewForm			332
#define cmdidViewCode			333
#define cmdidPreviewInBrowser		334
#define cmdidBrowseWith			336
#define cmdidSearchSetCombo		307
#define cmdidSearchCombo		337
#define cmdidEditLabel	  		338
#define cmdidExceptions         339
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
#define cmdidFindHiddenText             349
#define cmdidFindMatchCase              350
#define cmdidFindWholeWord              351
#define cmdidFindSimplePattern          276
#define cmdidFindRegularExpression      352
#define cmdidFindBackwards              353
#define cmdidFindInSelection            354
#define cmdidFindStop                   355
// UNUSED                               356
#define cmdidFindInFiles                277
#define cmdidReplaceInFiles             278
#define cmdidNextLocation               279 // next item in task list, find in files results, etc.
#define cmdidPreviousLocation           280 // prev item "

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
// 369 is used above in cmdidLockControls

#define cmdidFindNext                   370
#define cmdidFindPrev                   371
#define cmdidFindSelectedNext           372
#define cmdidFindSelectedPrev           373
#define cmdidSearchGetList              374
#define cmdidInsertBreakpoint		375
#define cmdidEnableBreakpoint		376
#define cmdidF1Help			377

//UNUSED 378-396

#define cmdidMoveToNextEZCntr           384
#define cmdidMoveToPreviousEZCntr       393

#define cmdidProjectProperties          396
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
#define cmdidMarkerEnd                  410 // list terminator reserved

// user-invoked project reload and unload
#define cmdidReloadProject              412
#define cmdidUnloadProject              413

#define cmdidNewBlankSolution           414
#define cmdidSelectProjectTemplate      415

// document outline commands
#define cmdidDetachAttachOutline		420
#define cmdidShowHideOutline			421
#define cmdidSyncOutline				422

#define cmdidRunToCallstCursor          423
#define cmdidNoCmdsAvailable		424

#define cmdidContextWindow              427
#define cmdidAlias			428
#define cmdidGotoCommandLine		429
#define cmdidEvaluateExpression		430
#define cmdidImmediateMode		431
#define cmdidEvaluateStatement		432

#define cmdidFindResultWindow1  433
#define cmdidFindResultWindow2  434

// 500 is used above in cmdidFontNameGetList
// 501 is used above in cmdidFontSizeGetList

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

#define cmdidAutoHideAllWindows		597  
#define cmdidTaskListTaskHelp		598

#define cmdidClassView			599

#define cmdidMRUProj1			600
#define cmdidMRUProj2			601
#define cmdidMRUProj3			602
#define cmdidMRUProj4			603
#define cmdidMRUProj5			604
#define cmdidMRUProj6			605
#define cmdidMRUProj7			606
#define cmdidMRUProj8			607
#define cmdidMRUProj9			608
#define cmdidMRUProj10			609
#define cmdidMRUProj11			610
#define cmdidMRUProj12			611
#define cmdidMRUProj13			612
#define cmdidMRUProj14			613
#define cmdidMRUProj15			614
#define cmdidMRUProj16			615
#define cmdidMRUProj17			616
#define cmdidMRUProj18			617
#define cmdidMRUProj19			618
#define cmdidMRUProj20			619
#define cmdidMRUProj21			620
#define cmdidMRUProj22			621
#define cmdidMRUProj23			622
#define cmdidMRUProj24			623
#define cmdidMRUProj25			624  // note cmdidMRUProj25 is unused on purpose!

#define cmdidSplitNext			625
#define cmdidSplitPrev			626

#define cmdidCloseAllDocuments		627
#define cmdidNextDocument		628
#define cmdidPrevDocument		629

#define cmdidTool1			630  // note cmdidTool1 - cmdidTool24 must be
#define cmdidTool2			631  // consecutive
#define cmdidTool3			632
#define cmdidTool4			633
#define cmdidTool5			634
#define cmdidTool6			635
#define cmdidTool7			636
#define cmdidTool8			637
#define cmdidTool9			638
#define cmdidTool10			639
#define cmdidTool11			640
#define cmdidTool12			641
#define cmdidTool13			642
#define cmdidTool14			643
#define cmdidTool15			644
#define cmdidTool16			645
#define cmdidTool17			646
#define cmdidTool18			647
#define cmdidTool19			648
#define cmdidTool20			649
#define cmdidTool21			650
#define cmdidTool22			651
#define cmdidTool23			652
#define cmdidTool24			653
#define cmdidExternalCommands		654

#define cmdidPasteNextTBXCBItem		655
#define cmdidToolboxShowAllTabs		656
#define cmdidProjectDependencies	657
#define cmdidCloseDocument		658
#define cmdidToolboxSortItems		659

#define cmdidViewBarView1		660   //UNUSED
#define cmdidViewBarView2		661   //UNUSED
#define cmdidViewBarView3		662   //UNUSED
#define cmdidViewBarView4		663   //UNUSED
#define cmdidViewBarView5		664   //UNUSED
#define cmdidViewBarView6		665   //UNUSED
#define cmdidViewBarView7		666   //UNUSED
#define cmdidViewBarView8		667   //UNUSED
#define cmdidViewBarView9		668   //UNUSED
#define cmdidViewBarView10		669   //UNUSED
#define cmdidViewBarView11		670   //UNUSED
#define cmdidViewBarView12		671   //UNUSED
#define cmdidViewBarView13		672   //UNUSED
#define cmdidViewBarView14		673   //UNUSED
#define cmdidViewBarView15		674   //UNUSED
#define cmdidViewBarView16		675   //UNUSED
#define cmdidViewBarView17		676   //UNUSED
#define cmdidViewBarView18		677   //UNUSED
#define cmdidViewBarView19		678   //UNUSED
#define cmdidViewBarView20		679   //UNUSED
#define cmdidViewBarView21		680   //UNUSED
#define cmdidViewBarView22		681   //UNUSED
#define cmdidViewBarView23		682   //UNUSED
#define cmdidViewBarView24		683   //UNUSED

#define cmdidSolutionCfg		684
#define cmdidSolutionCfgGetList		685

//
// Schema table commands:
// All invoke table property dialog and select appropriate page.
//
#define cmdidManageIndexes			675
#define cmdidManageRelationships	676
#define cmdidManageConstraints		677

#define cmdidTaskListCustomView1        678
#define cmdidTaskListCustomView2        679
#define cmdidTaskListCustomView3        680
#define cmdidTaskListCustomView4        681
#define cmdidTaskListCustomView5        682
#define cmdidTaskListCustomView6        683
#define cmdidTaskListCustomView7        684
#define cmdidTaskListCustomView8        685
#define cmdidTaskListCustomView9        686
#define cmdidTaskListCustomView10       687
#define cmdidTaskListCustomView11       688
#define cmdidTaskListCustomView12       689
#define cmdidTaskListCustomView13       690
#define cmdidTaskListCustomView14       691
#define cmdidTaskListCustomView15       692
#define cmdidTaskListCustomView16       693  
#define cmdidTaskListCustomView17       694
#define cmdidTaskListCustomView18       695
#define cmdidTaskListCustomView19       696
#define cmdidTaskListCustomView20       697
#define cmdidTaskListCustomView21       698
#define cmdidTaskListCustomView22       699
#define cmdidTaskListCustomView23       700
#define cmdidTaskListCustomView24       701
#define cmdidTaskListCustomView25       702
#define cmdidTaskListCustomView26       703 
#define cmdidTaskListCustomView27       704
#define cmdidTaskListCustomView28       705
#define cmdidTaskListCustomView29       706
#define cmdidTaskListCustomView30       707
#define cmdidTaskListCustomView31       708
#define cmdidTaskListCustomView32       709
#define cmdidTaskListCustomView33       710
#define cmdidTaskListCustomView34       711
#define cmdidTaskListCustomView35       712
#define cmdidTaskListCustomView36       713
#define cmdidTaskListCustomView37       714
#define cmdidTaskListCustomView38       715
#define cmdidTaskListCustomView39       716
#define cmdidTaskListCustomView40       717
#define cmdidTaskListCustomView41       718
#define cmdidTaskListCustomView42       719
#define cmdidTaskListCustomView43       720
#define cmdidTaskListCustomView44       721
#define cmdidTaskListCustomView45       722
#define cmdidTaskListCustomView46       723
#define cmdidTaskListCustomView47       724
#define cmdidTaskListCustomView48       725
#define cmdidTaskListCustomView49       726
#define cmdidTaskListCustomView50       727 //not used on purpose, ends the list

#define cmdidWhiteSpace                 728

#define cmdidCommandWindow		729
#define cmdidCommandWindowMarkMode	730
#define cmdidLogCommandWindow		731

#define cmdidShell			732

#define cmdidSingleChar           733
#define cmdidZeroOrMore           734
#define cmdidOneOrMore            735
#define cmdidBeginLine            736
#define cmdidEndLine              737
#define cmdidBeginWord            738
#define cmdidEndWord              739
#define cmdidCharInSet            740
#define cmdidCharNotInSet         741
#define cmdidOr                   742
#define cmdidEscape               743
#define cmdidTagExp               744

// Regex builder context help menu commands
#define cmdidPatternMatchHelp       745
#define cmdidRegExList              746

#define cmdidDebugReserved1         747
#define cmdidDebugReserved2         748
#define cmdidDebugReserved3         749
//USED ABOVE                        750
//USED ABOVE                        751
//USED ABOVE                        752
//USED ABOVE                        753

//Regex builder wildcard menu commands
#define cmdidWildZeroOrMore         754
#define cmdidWildSingleChar         755
#define cmdidWildSingleDigit        756
#define cmdidWildCharInSet          757
#define cmdidWildCharNotInSet       758

#define cmdidFindWhatText           759
#define cmdidTaggedExp1             760
#define cmdidTaggedExp2             761
#define cmdidTaggedExp3             762
#define cmdidTaggedExp4             763
#define cmdidTaggedExp5             764
#define cmdidTaggedExp6             765
#define cmdidTaggedExp7             766
#define cmdidTaggedExp8             767
#define cmdidTaggedExp9             768		                    

#define cmdidEditorWidgetClick      769 // param 0 is the moniker as VT_BSTR, param 1 is the buffer line as VT_I4, and param 2 is the buffer index as VT_I4
#define cmdidCmdWinUpdateAC         770

#define cmdidSlnCfgMgr                  771

#define cmdidAddNewProject		772
#define cmdidAddExistingProject		773
#define cmdidAddExistingProjFromWeb	774

#define cmdidAutoHideContext1		776
#define cmdidAutoHideContext2		777
#define cmdidAutoHideContext3		778
#define cmdidAutoHideContext4		779
#define cmdidAutoHideContext5		780
#define cmdidAutoHideContext6		781
#define cmdidAutoHideContext7		782
#define cmdidAutoHideContext8		783
#define cmdidAutoHideContext9		784
#define cmdidAutoHideContext10		785
#define cmdidAutoHideContext11		786
#define cmdidAutoHideContext12		787
#define cmdidAutoHideContext13		788
#define cmdidAutoHideContext14		789
#define cmdidAutoHideContext15		790
#define cmdidAutoHideContext16		791
#define cmdidAutoHideContext17		792
#define cmdidAutoHideContext18		793
#define cmdidAutoHideContext19		794
#define cmdidAutoHideContext20		795
#define cmdidAutoHideContext21		796
#define cmdidAutoHideContext22		797
#define cmdidAutoHideContext23		798
#define cmdidAutoHideContext24		799
#define cmdidAutoHideContext25		800
#define cmdidAutoHideContext26		801
#define cmdidAutoHideContext27		802
#define cmdidAutoHideContext28		803
#define cmdidAutoHideContext29		804
#define cmdidAutoHideContext30		805
#define cmdidAutoHideContext31		806
#define cmdidAutoHideContext32		807
#define cmdidAutoHideContext33		808  // must remain unused

#define cmdidShellNavBackward           809
#define cmdidShellNavForward            810

#define cmdidShellNavigate1		811
#define cmdidShellNavigate2		812
#define cmdidShellNavigate3		813
#define cmdidShellNavigate4		814
#define cmdidShellNavigate5		815
#define cmdidShellNavigate6		816
#define cmdidShellNavigate7		817
#define cmdidShellNavigate8		818
#define cmdidShellNavigate9		819
#define cmdidShellNavigate10		820
#define cmdidShellNavigate11		821
#define cmdidShellNavigate12		822
#define cmdidShellNavigate13		823
#define cmdidShellNavigate14		824
#define cmdidShellNavigate15		825
#define cmdidShellNavigate16		826
#define cmdidShellNavigate17		827
#define cmdidShellNavigate18		828
#define cmdidShellNavigate19		829
#define cmdidShellNavigate20		830
#define cmdidShellNavigate21		831
#define cmdidShellNavigate22		832
#define cmdidShellNavigate23		833
#define cmdidShellNavigate24		834
#define cmdidShellNavigate25		835
#define cmdidShellNavigate26		836
#define cmdidShellNavigate27		837
#define cmdidShellNavigate28		838
#define cmdidShellNavigate29		839
#define cmdidShellNavigate30		840
#define cmdidShellNavigate31		841
#define cmdidShellNavigate32		842
#define cmdidShellNavigate33		843  // must remain unused

#define cmdidShellWindowNavigate1	844
#define cmdidShellWindowNavigate2	845
#define cmdidShellWindowNavigate3	846
#define cmdidShellWindowNavigate4	847
#define cmdidShellWindowNavigate5	848
#define cmdidShellWindowNavigate6	849
#define cmdidShellWindowNavigate7	850
#define cmdidShellWindowNavigate8	851
#define cmdidShellWindowNavigate9	852
#define cmdidShellWindowNavigate10	853
#define cmdidShellWindowNavigate11	854
#define cmdidShellWindowNavigate12	855
#define cmdidShellWindowNavigate13	856
#define cmdidShellWindowNavigate14	857
#define cmdidShellWindowNavigate15	858
#define cmdidShellWindowNavigate16	859
#define cmdidShellWindowNavigate17	860
#define cmdidShellWindowNavigate18	861
#define cmdidShellWindowNavigate19	862
#define cmdidShellWindowNavigate20	863
#define cmdidShellWindowNavigate21	864
#define cmdidShellWindowNavigate22	865
#define cmdidShellWindowNavigate23	866
#define cmdidShellWindowNavigate24	867
#define cmdidShellWindowNavigate25	868
#define cmdidShellWindowNavigate26	869
#define cmdidShellWindowNavigate27	870
#define cmdidShellWindowNavigate28	871
#define cmdidShellWindowNavigate29	872
#define cmdidShellWindowNavigate30	873
#define cmdidShellWindowNavigate31	874
#define cmdidShellWindowNavigate32	875
#define cmdidShellWindowNavigate33	876  // must remain unused

// ObjectSearch cmds
#define cmdidOBSDoFind                  877
#define cmdidOBSMatchCase               878
#define cmdidOBSMatchSubString          879
#define cmdidOBSMatchWholeWord          880
#define cmdidOBSMatchPrefix             881

// build cmds
#define cmdidBuildSln                   882
#define cmdidRebuildSln                 883
#define cmdidDeploySln                  884
#define cmdidCleanSln                   885

#define cmdidBuildSel                   886
#define cmdidRebuildSel                 887
#define cmdidDeploySel                  888
#define cmdidCleanSel                   889

#define cmdidCancelBuild                890
#define cmdidBatchBuildDlg              891

#define cmdidBuildCtx                   892
#define cmdidRebuildCtx                 893
#define cmdidDeployCtx                  894
#define cmdidCleanCtx                   895

#define cmdidQryManageIndexes		896
#define cmdidPrintDefault               897		// quick print
#define cmdidBrowseDoc                  898
#define cmdidShowStartPage	        899

#define cmdidMRUFile1			900
#define cmdidMRUFile2			901
#define cmdidMRUFile3			902
#define cmdidMRUFile4			903
#define cmdidMRUFile5			904
#define cmdidMRUFile6			905
#define cmdidMRUFile7			906
#define cmdidMRUFile8			907
#define cmdidMRUFile9			908
#define cmdidMRUFile10			909
#define cmdidMRUFile11			910
#define cmdidMRUFile12			911
#define cmdidMRUFile13			912
#define cmdidMRUFile14			913
#define cmdidMRUFile15			914
#define cmdidMRUFile16			915
#define cmdidMRUFile17			916
#define cmdidMRUFile18			917
#define cmdidMRUFile19			918
#define cmdidMRUFile20			919
#define cmdidMRUFile21			920
#define cmdidMRUFile22			921
#define cmdidMRUFile23			922
#define cmdidMRUFile24			923
#define cmdidMRUFile25			924  // note cmdidMRUFile25 is unused on purpose!

//External Tools Context Menu Commands
// continued at 1109
#define cmdidExtToolsCurPath            925
#define cmdidExtToolsCurDir             926
#define cmdidExtToolsCurFileName        927
#define cmdidExtToolsCurExtension       928
#define cmdidExtToolsProjDir            929
#define cmdidExtToolsProjFileName       930
#define cmdidExtToolsSlnDir             931
#define cmdidExtToolsSlnFileName        932


// Object Browsing & ClassView cmds
// Shared shell cmds (for accessing Object Browsing functionality)
#define cmdidGotoDefn			935
#define cmdidGotoDecl			936
#define cmdidBrowseDefn			937
#define cmdidSyncClassView              938
#define cmdidShowMembers		939
#define cmdidShowBases			940
#define cmdidShowDerived		941
#define cmdidShowDefns			942
#define cmdidShowRefs			943
#define cmdidShowCallers		944
#define cmdidShowCallees		945

#define cmdidAddClass                   946
#define cmdidAddNestedClass             947
#define cmdidAddInterface               948
#define cmdidAddMethod                  949
#define cmdidAddProperty                950
#define cmdidAddEvent                   951
#define cmdidAddVariable                952
#define cmdidImplementInterface         953
#define cmdidOverride                   954
#define cmdidAddFunction                955
#define cmdidAddConnectionPoint         956
#define cmdidAddIndexer                 957

#define cmdidBuildOrder                 958
//959 used above for cmdidSaveOptions

// Object Browser Tool Specific cmds
#define cmdidOBShowHidden               960
#define cmdidOBEnableGrouping           961
#define cmdidOBSetGroupingCriteria      962
#define cmdidOBBack                     963
#define cmdidOBForward                  964
#define cmdidOBShowPackages             965
#define cmdidOBSearchCombo              966
#define cmdidOBSearchOptWholeWord       967 
#define cmdidOBSearchOptSubstring       968 
#define cmdidOBSearchOptPrefix          969 
#define cmdidOBSearchOptCaseSensitive   970

// ClassView Tool Specific cmds
#define cmdidCVGroupingNone             971
#define cmdidCVGroupingSortOnly		972
#define cmdidCVGroupingGrouped		973
#define cmdidCVShowPackages		974
#define cmdidCVNewFolder                975
#define cmdidCVGroupingSortAccess       976

#define cmdidObjectSearch               977
#define cmdidObjectSearchResults        978

// Further Obj Browsing cmds at 1095

// build cascade menus
#define cmdidBuild1                     979
#define cmdidBuild2                     980
#define cmdidBuild3                     981
#define cmdidBuild4                     982
#define cmdidBuild5                     983
#define cmdidBuild6                     984
#define cmdidBuild7                     985
#define cmdidBuild8                     986
#define cmdidBuild9                     987
#define cmdidBuildLast                  988

#define cmdidRebuild1                   989
#define cmdidRebuild2                   990
#define cmdidRebuild3                   991
#define cmdidRebuild4                   992
#define cmdidRebuild5                   993
#define cmdidRebuild6                   994
#define cmdidRebuild7                   995
#define cmdidRebuild8                   996
#define cmdidRebuild9                   997
#define cmdidRebuildLast                998

#define cmdidClean1                     999
#define cmdidClean2                     1000
#define cmdidClean3                     1001
#define cmdidClean4                     1002
#define cmdidClean5                     1003
#define cmdidClean6                     1004
#define cmdidClean7                     1005
#define cmdidClean8                     1006
#define cmdidClean9                     1007
#define cmdidCleanLast                  1008

#define cmdidDeploy1                    1009
#define cmdidDeploy2                    1010
#define cmdidDeploy3                    1011
#define cmdidDeploy4                    1012
#define cmdidDeploy5                    1013
#define cmdidDeploy6                    1014
#define cmdidDeploy7                    1015
#define cmdidDeploy8                    1016
#define cmdidDeploy9                    1017
#define cmdidDeployLast                 1018

#define cmdidBuildProjPicker            1019
#define cmdidRebuildProjPicker          1020
#define cmdidCleanProjPicker            1021
#define cmdidDeployProjPicker           1022
#define cmdidResourceView               1023

#define cmdidShowHomePage               1024
#define cmdidEditMenuIDs                1025

#define cmdidLineBreak                  1026
#define cmdidCPPIdentifier              1027
#define cmdidQuotedString               1028
#define cmdidSpaceOrTab                 1029
#define cmdidInteger                    1030
//unused 1031-1035

#define cmdidCustomizeToolbars          1036
#define cmdidMoveToTop                  1037
#define cmdidWindowHelp	                1038

#define cmdidViewPopup                  1039
#define cmdidCheckMnemonics             1040

#define cmdidPRSortAlphabeticaly        1041
#define cmdidPRSortByCategory           1042

#define cmdidViewNextTab                1043

#define cmdidCheckForUpdates            1044

#define cmdidBrowser1			1045
#define cmdidBrowser2			1046
#define cmdidBrowser3			1047
#define cmdidBrowser4			1048
#define cmdidBrowser5			1049
#define cmdidBrowser6			1050
#define cmdidBrowser7			1051
#define cmdidBrowser8			1052
#define cmdidBrowser9			1053
#define cmdidBrowser10			1054
#define cmdidBrowser11			1055 //note unused on purpose to end list

#define cmdidOpenDropDownOpen           1058
#define cmdidOpenDropDownOpenWith       1059

#define cmdidToolsDebugProcesses        1060

#define cmdidPaneNextSubPane            1062
#define cmdidPanePrevSubPane            1063

#define cmdidMoveFileToProject1         1070
#define cmdidMoveFileToProject2         1071
#define cmdidMoveFileToProject3         1072
#define cmdidMoveFileToProject4         1073
#define cmdidMoveFileToProject5         1074
#define cmdidMoveFileToProject6         1075
#define cmdidMoveFileToProject7         1076
#define cmdidMoveFileToProject8         1077
#define cmdidMoveFileToProject9         1078
#define cmdidMoveFileToProjectLast      1079 // unused in order to end list
#define cmdidMoveFileToProjectPick      1081

#define cmdidDefineSubset               1095
#define cmdidSubsetCombo                1096
#define cmdidSubsetGetList              1097
#define cmdidOBSortObjectsAlpha         1098
#define cmdidOBSortObjectsType          1099
#define cmdidOBSortObjectsAccess        1100
#define cmdidOBGroupObjectsType         1101
#define cmdidOBGroupObjectsAccess       1102
#define cmdidOBSortMembersAlpha         1103
#define cmdidOBSortMembersType          1104
#define cmdidOBSortMembersAccess        1105

#define cmdidPopBrowseContext           1106
#define cmdidGotoRef			1107
#define cmdidOBSLookInReferences        1108

#define cmdidExtToolsTargetPath         1109
#define cmdidExtToolsTargetDir          1110
#define cmdidExtToolsTargetFileName     1111
#define cmdidExtToolsTargetExtension    1112
#define cmdidExtToolsCurLine            1113
#define cmdidExtToolsCurCol             1114
#define cmdidExtToolsCurText            1115

#define cmdidBrowseNext                 1116
#define cmdidBrowsePrev                 1117
#define cmdidBrowseUnload		1118
#define cmdidQuickObjectSearch          1119
#define cmdidExpandAll                  1120

#define cmdidStandardMax                1500

///////////////////////////////////////////
//
// cmdidStandardMax is now thought to be
// obsolete. Any new shell commands should
// be added to the end of StandardCommandSet2K
// which appears below.
//
// If you are not adding shell commands,
// you shouldn't be doing it in this file! 
//
///////////////////////////////////////////


#define cmdidFormsFirst           0x00006000

#define cmdidFormsLast           0x00006FFF

#define cmdidVBEFirst           0x00008000   


#define cmdidZoom200		    0x00008002
#define cmdidZoom150		    0x00008003
#define cmdidZoom100		    0x00008004
#define cmdidZoom75		    0x00008005
#define cmdidZoom50		    0x00008006
#define cmdidZoom25		    0x00008007
#define cmdidZoom10		    0x00008010


#define cmdidVBELast           0x00009FFF 
                                         
#define cmdidSterlingFirst           0x0000A000                                    
#define cmdidSterlingLast           0x0000BFFF                                     

#define uieventidFirst                   0xC000
#define uieventidSelectRegion      0xC001
#define uieventidDrop                  0xC002
#define uieventidLast                   0xDFFF




//////////////////////////////////////////////////////////////////
//
// The following commands form CMDSETID_StandardCommandSet2k.
// Note that commands up to ECMD_FINAL are standard editor
// commands and have been moved from editcmd.h.
// NOTE that all these commands are shareable and may be used
// in any appropriate menu.
//
//////////////////////////////////////////////////////////////////
//
// Shareable standard editor commands
//
#define ECMD_TYPECHAR                1
#define ECMD_BACKSPACE               2
#define ECMD_RETURN                  3
#define ECMD_TAB                     4
#define ECMD_BACKTAB                 5
#define ECMD_DELETE                  6
#define ECMD_LEFT                    7
#define ECMD_LEFT_EXT                8
#define ECMD_RIGHT                   9
#define ECMD_RIGHT_EXT              10
#define ECMD_UP                     11
#define ECMD_UP_EXT                 12
#define ECMD_DOWN                   13
#define ECMD_DOWN_EXT               14
#define ECMD_HOME                   15
#define ECMD_HOME_EXT               16
#define ECMD_END                    17
#define ECMD_END_EXT                18
#define ECMD_BOL                    19
#define ECMD_BOL_EXT                20
#define ECMD_FIRSTCHAR              21
#define ECMD_FIRSTCHAR_EXT          22
#define ECMD_EOL                    23
#define ECMD_EOL_EXT                24
#define ECMD_LASTCHAR               25
#define ECMD_LASTCHAR_EXT           26
#define ECMD_PAGEUP                 27
#define ECMD_PAGEUP_EXT             28
#define ECMD_PAGEDN                 29
#define ECMD_PAGEDN_EXT             30
#define ECMD_TOPLINE                31
#define ECMD_TOPLINE_EXT            32
#define ECMD_BOTTOMLINE             33
#define ECMD_BOTTOMLINE_EXT         34
#define ECMD_SCROLLUP               35
#define ECMD_SCROLLDN               36
#define ECMD_SCROLLPAGEUP           37
#define ECMD_SCROLLPAGEDN           38
#define ECMD_SCROLLLEFT             39
#define ECMD_SCROLLRIGHT            40
#define ECMD_SCROLLBOTTOM           41
#define ECMD_SCROLLCENTER           42
#define ECMD_SCROLLTOP              43
#define ECMD_SELECTALL              44
#define ECMD_SELTABIFY              45
#define ECMD_SELUNTABIFY            46
#define ECMD_SELLOWCASE             47
#define ECMD_SELUPCASE              48
#define ECMD_SELTOGGLECASE          49
#define ECMD_SELTITLECASE           50
#define ECMD_SELSWAPANCHOR          51
#define ECMD_GOTOLINE               52
#define ECMD_GOTOBRACE              53
#define ECMD_GOTOBRACE_EXT          54
#define ECMD_GOBACK                 55
#define ECMD_SELECTMODE             56
#define ECMD_TOGGLE_OVERTYPE_MODE   57
#define ECMD_CUT                    58
#define ECMD_COPY                   59
#define ECMD_PASTE                  60
#define ECMD_CUTLINE                61
#define ECMD_DELETELINE             62
#define ECMD_DELETEBLANKLINES       63
#define ECMD_DELETEWHITESPACE       64
#define ECMD_DELETETOEOL            65
#define ECMD_DELETETOBOL            66
#define ECMD_OPENLINEABOVE          67
#define ECMD_OPENLINEBELOW          68
#define ECMD_INDENT                 69
#define ECMD_UNINDENT               70
#define ECMD_UNDO                   71
#define ECMD_UNDONOMOVE             72
#define ECMD_REDO                   73
#define ECMD_REDONOMOVE             74
#define ECMD_DELETEALLTEMPBOOKMARKS 75
#define ECMD_TOGGLETEMPBOOKMARK     76
#define ECMD_GOTONEXTBOOKMARK       77
#define ECMD_GOTOPREVBOOKMARK       78
#define ECMD_FIND                   79
#define ECMD_REPLACE                80
#define ECMD_REPLACE_ALL            81
#define ECMD_FINDNEXT               82
#define ECMD_FINDNEXTWORD           83
#define ECMD_FINDPREV               84
#define ECMD_FINDPREVWORD           85
#define ECMD_FINDAGAIN              86
#define ECMD_TRANSPOSECHAR          87
#define ECMD_TRANSPOSEWORD          88
#define ECMD_TRANSPOSELINE          89
#define ECMD_SELECTCURRENTWORD      90
#define ECMD_DELETEWORDRIGHT        91
#define ECMD_DELETEWORDLEFT         92
#define ECMD_WORDPREV               93
#define ECMD_WORDPREV_EXT           94
#define ECMD_WORDNEXT               96
#define ECMD_WORDNEXT_EXT           97
#define ECMD_COMMENTBLOCK           98
#define ECMD_UNCOMMENTBLOCK         99
#define ECMD_SETREPEATCOUNT         100
#define ECMD_WIDGETMARGIN_LBTNDOWN  101
#define ECMD_SHOWCONTEXTMENU        102
#define ECMD_CANCEL                 103
#define ECMD_PARAMINFO              104
#define ECMD_TOGGLEVISSPACE         105
#define ECMD_TOGGLECARETPASTEPOS    106
#define ECMD_COMPLETEWORD           107
#define ECMD_SHOWMEMBERLIST         108
#define ECMD_FIRSTNONWHITEPREV      109
#define ECMD_FIRSTNONWHITENEXT      110
#define ECMD_HELPKEYWORD            111
#define ECMD_FORMATSELECTION	    112
#define ECMD_OPENURL    	    113     
#define ECMD_INSERTFILE 	    114
#define ECMD_TOGGLESHORTCUT	    115
#define ECMD_QUICKINFO              116
#define ECMD_LEFT_EXT_COL           117
#define ECMD_RIGHT_EXT_COL          118
#define ECMD_UP_EXT_COL             119
#define ECMD_DOWN_EXT_COL           120
#define ECMD_TOGGLEWORDWRAP         121
#define ECMD_ISEARCH                122
#define ECMD_ISEARCHBACK            123
#define ECMD_BOL_EXT_COL            124
#define ECMD_EOL_EXT_COL            125
#define ECMD_WORDPREV_EXT_COL       126
#define ECMD_WORDNEXT_EXT_COL       127
#define ECMD_OUTLN_HIDE_SELECTION   128
#define ECMD_OUTLN_TOGGLE_CURRENT   129
#define ECMD_OUTLN_TOGGLE_ALL       130
#define ECMD_OUTLN_STOP_HIDING_ALL  131
#define ECMD_OUTLN_STOP_HIDING_CURRENT 132
#define ECMD_OUTLN_COLLAPSE_TO_DEF  133
#define ECMD_DOUBLECLICK            134
#define ECMD_EXTERNALLY_HANDLED_WIDGET_CLICK 135
#define ECMD_COMMENT_BLOCK          136
#define ECMD_UNCOMMENT_BLOCK        137
#define ECMD_OPENFILE               138
#define ECMD_NAVIGATETOURL          139

// For editor internal use only
#define ECMD_HANDLEIMEMESSAGE       140

#define ECMD_SELTOGOBACK            141
#define ECMD_COMPLETION_HIDE_ADVANCED 142

#define ECMD_FORMATDOCUMENT         143
#define ECMD_OUTLN_START_AUTOHIDING 144

// Last Standard Editor Command (+1)
#define ECMD_FINAL                  145

///////////////////////////////////////////////////////////////
// Some new commands created during CTC file rationalisation
///////////////////////////////////////////////////////////////
#define ECMD_STOP                   220
#define ECMD_REVERSECANCEL          221
#define ECMD_SLNREFRESH             222
#define ECMD_SAVECOPYOFITEMAS       223
//
// Shareable commands originating in the HTML editor
//
#define ECMD_NEWELEMENT             224
#define ECMD_NEWATTRIBUTE           225
#define ECMD_NEWCOMPLEXTYPE         226
#define ECMD_NEWSIMPLETYPE          227
#define ECMD_NEWGROUP               228
#define ECMD_NEWATTRIBUTEGROUP      229
#define ECMD_NEWKEY                 230
#define ECMD_NEWRELATION            231
#define ECMD_EDITKEY                232
#define ECMD_EDITRELATION           233
#define ECMD_MAKETYPEGLOBAL         234
#define ECMD_PREVIEWDATASET         235
#define ECMD_GENERATEDATASET        236
#define ECMD_CREATESCHEMA           237
#define ECMD_LAYOUTINDENT           238
#define ECMD_LAYOUTUNINDENT         239
#define ECMD_REMOVEHANDLER			240
#define ECMD_EDITHANDLER			241
#define ECMD_ADDHANDLER				242
#define ECMD_STYLE                  243
#define ECMD_STYLEGETLIST           244
#define ECMD_FONTSTYLE              245
#define ECMD_FONTSTYLEGETLIST       246
#define ECMD_PASTEASHTML			247
#define ECMD_VIEWBORDERS			248
#define ECMD_VIEWDETAILS			249
#define ECMD_EXPANDCONTROLS			250
#define ECMD_COLLAPSECONTROLS		251
#define ECMD_SHOWSCRIPTONLY			252
#define ECMD_INSERTTABLE			253
#define ECMD_INSERTCOLLEFT			254
#define ECMD_INSERTCOLRIGHT			255
#define ECMD_INSERTROWABOVE			256
#define ECMD_INSERTROWBELOW			257
#define ECMD_DELETETABLE			258
#define ECMD_DELETECOLS				259
#define ECMD_DELETEROWS				260
#define ECMD_SELECTTABLE			261
#define ECMD_SELECTTABLECOL			262
#define ECMD_SELECTTABLEROW			263
#define ECMD_SELECTTABLECELL		264
#define ECMD_MERGECELLS				265
#define ECMD_SPLITCELL				266
#define ECMD_INSERTCELL				267
#define ECMD_DELETECELLS			268
#define ECMD_SEAMLESSFRAME			269
#define ECMD_VIEWFRAME				270
#define ECMD_DELETEFRAME			271
#define ECMD_SETFRAMESOURCE			272
#define ECMD_NEWLEFTFRAME			273
#define ECMD_NEWRIGHTFRAME			274
#define ECMD_NEWTOPFRAME			275
#define ECMD_NEWBOTTOMFRAME			276
#define ECMD_SHOWGRID				277
#define ECMD_SNAPTOGRID				278
#define ECMD_BOOKMARK				279
#define ECMD_HYPERLINK				280
#define ECMD_IMAGE					281
#define ECMD_INSERTFORM				282
#define ECMD_INSERTSPAN				283
#define ECMD_DIV					284
#define ECMD_HTMLCLIENTSCRIPTBLOCK	285
#define ECMD_HTMLSERVERSCRIPTBLOCK	286
#define ECMD_BULLETEDLIST			287
#define ECMD_NUMBEREDLIST			288
#define ECMD_EDITSCRIPT				289
#define ECMD_EDITCODEBEHIND			290
#define ECMD_DOCOUTLINEHTML			291
#define ECMD_DOCOUTLINESCRIPT		292
#define ECMD_RUNATSERVER			293
#define ECMD_WEBFORMSVERBS			294
#define ECMD_WEBFORMSTEMPLATES		295
#define ECMD_ENDTEMPLATE			296
#define ECMD_EDITDEFAULTEVENT		297
#define ECMD_SUPERSCRIPT            298
#define ECMD_SUBSCRIPT              299
#define ECMD_EDITSTYLE              300
#define ECMD_ADDIMAGEHEIGHTWIDTH    301
#define ECMD_REMOVEIMAGEHEIGHTWIDTH 302
#define ECMD_LOCKELEMENT            303
#define ECMD_VIEWSTYLEORGANIZER     304
#define	ECMD_AUTOCLOSEOVERRIDE		305
#define ECMD_NEWANY                 306
#define ECMD_NEWANYATTRIBUTE        307
#define ECMD_DELETEKEY              308
#define ECMD_AUTOARRANGE            309
#define ECMD_VALIDATESCHEMA         310
#define ECMD_NEWFACET               311
#define ECMD_VALIDATEXMLDATA        312
#define ECMD_DOCOUTLINETOGGLE       313
#define ECMD_VALIDATEHTMLDATA       314
#define ECMD_VIEWXMLSCHEMAOVERVIEW  315
//
// Shareable commands originating in the VC project
//
#define ECMD_COMPILE                350
//
#define ECMD_PROJSETTINGS           352
#define ECMD_LINKONLY               353
//
#define ECMD_REMOVE					355
#define ECMD_PROJSTARTDEBUG         356
#define ECMD_PROJSTEPINTO           357
//
//
#define ECMD_UPDATEWEBREF			360
//
#define ECMD_ADDRESOURCE			362
#define ECMD_WEBDEPLOY			    363

#define ECMD_XBOXIMAGE				370
#define ECMD_XBOXDEPLOY				371
//
// Shareable commands originating in the VB and VBA projects
// Note that there are two versions of each command. One
// version is originally from the main (project) menu and the
// other version from a cascading "Add" context menu. The main
// difference between the two commands is that the main menu
// version starts with the text "Add" whereas this is not
// present on the context menu version.
//
#define ECMD_ADDHTMLPAGE            400
#define ECMD_ADDHTMLPAGECTX         401
#define ECMD_ADDMODULE              402
#define ECMD_ADDMODULECTX           403
// unused 404
// unused 405
#define ECMD_ADDWFCFORM             406
// unused 407
// unused 408
// unused 409
#define ECMD_ADDWEBFORM             410
// unused 411
#define ECMD_ADDUSERCONTROL         412
// unused 413 to 425
#define ECMD_ADDDHTMLPAGE           426
// unused 427 to 431
#define ECMD_ADDIMAGEGENERATOR      432
// unused 433
#define ECMD_ADDINHERWFCFORM        434
// unused 435
#define ECMD_ADDINHERCONTROL        436
// unused 437
#define ECMD_ADDWEBUSERCONTROL      438
#define ECMD_BUILDANDBROWSE         439
// unused 440
// unused 441
#define ECMD_ADDTBXCOMPONENT        442
// unused 443
#define ECMD_ADDWEBSERVICE          444
// unused 445
//
// Shareable commands originating in the VFP project
//
#define ECMD_ADDVFPPAGE             500
#define ECMD_SETBREAKPOINT          501
//
// Shareable commands originating in the HELP WORKSHOP project
//
#define ECMD_SHOWALLFILES           600
#define ECMD_ADDTOPROJECT           601
#define ECMD_ADDBLANKNODE           602
#define ECMD_ADDNODEFROMFILE        603
#define ECMD_CHANGEURLFROMFILE      604
#define ECMD_EDITTOPIC              605
#define ECMD_EDITTITLE              606
#define ECMD_MOVENODEUP             607
#define ECMD_MOVENODEDOWN           608
#define ECMD_MOVENODELEFT           609
#define ECMD_MOVENODERIGHT          610
//
// Shareable commands originating in the Deploy project
//
// Note there are two groups of deploy project commands.
// The first group of deploy commands.
#define ECMD_ADDOUTPUT              700
#define ECMD_ADDFILE                701
#define ECMD_MERGEMODULE            702
#define ECMD_ADDCOMPONENTS          703
#define ECMD_LAUNCHINSTALLER        704
#define ECMD_LAUNCHUNINSTALL        705
#define ECMD_LAUNCHORCA             706
#define ECMD_FILESYSTEMEDITOR       707
#define ECMD_REGISTRYEDITOR         708
#define ECMD_FILETYPESEDITOR        709
#define ECMD_USERINTERFACEEDITOR    710
#define ECMD_CUSTOMACTIONSEDITOR    711
#define ECMD_LAUNCHCONDITIONSEDITOR 712
#define ECMD_EDITOR                 713
#define ECMD_EXCLUDE                714
#define ECMD_REFRESHDEPENDENCIES    715
#define ECMD_VIEWOUTPUTS            716
#define ECMD_VIEWDEPENDENCIES       717
#define ECMD_VIEWFILTER             718

//
// The Second group of deploy commands.
// Note that there is a special sub-group in which the relative 
// positions are important (see below)
//
#define ECMD_KEY                    750
#define ECMD_STRING                 751
#define ECMD_BINARY                 752
#define ECMD_DWORD                  753
#define ECMD_KEYSOLO                754
#define ECMD_IMPORT                 755
#define ECMD_FOLDER                 756
#define ECMD_PROJECTOUTPUT          757
#define ECMD_FILE                   758
#define ECMD_ADDMERGEMODULES        759
#define ECMD_CREATESHORTCUT         760
#define ECMD_LARGEICONS             761
#define ECMD_SMALLICONS             762
#define ECMD_LIST                   763
#define ECMD_DETAILS                764
#define ECMD_ADDFILETYPE            765
#define ECMD_ADDACTION              766
#define ECMD_SETASDEFAULT           767
#define ECMD_MOVEUP                 768
#define ECMD_MOVEDOWN               769
#define ECMD_ADDDIALOG              770
#define ECMD_IMPORTDIALOG           771
#define ECMD_ADDFILESEARCH          772
#define ECMD_ADDREGISTRYSEARCH      773
#define ECMD_ADDCOMPONENTSEARCH     774
#define ECMD_ADDLAUNCHCONDITION     775
#define ECMD_ADDCUSTOMACTION        776
#define ECMD_OUTPUTS                777
#define ECMD_DEPENDENCIES           778
#define ECMD_FILTER                 779
#define ECMD_COMPONENTS             780
#define ECMD_ENVSTRING		        781
#define ECMD_CREATEEMPTYSHORTCUT    782
#define ECMD_ADDFILECONDITION       783
#define ECMD_ADDREGISTRYCONDITION   784
#define ECMD_ADDCOMPONENTCONDITION  785
#define ECMD_ADDURTCONDITION        786
#define ECMD_ADDIISCONDITION        787

//
// The relative positions of the commands within the following deploy
// subgroup must remain unaltered, although the group as a whole may
// be repositioned. Note that the first and last elements are special
// boundary elements.
#define ECMD_SPECIALFOLDERBASE      800
#define ECMD_USERSAPPLICATIONDATAFOLDER 800
#define ECMD_COMMONFILESFOLDER      801
#define ECMD_CUSTOMFOLDER           802
#define ECMD_USERSDESKTOP           803
#define ECMD_USERSFAVORITESFOLDER   804
#define ECMD_FONTSFOLDER            805
#define ECMD_GLOBALASSEMBLYCACHEFOLDER  806
#define ECMD_MODULERETARGETABLEFOLDER   807
#define ECMD_USERSPERSONALDATAFOLDER    808
#define ECMD_PROGRAMFILESFOLDER     809
#define ECMD_USERSPROGRAMSMENU      810
#define ECMD_USERSSENDTOMENU        811
#define ECMD_SHAREDCOMPONENTSFOLDER 812
#define ECMD_USERSSTARTMENU         813
#define ECMD_USERSSTARTUPFOLDER     814
#define ECMD_SYSTEMFOLDER           815
#define ECMD_APPLICATIONFOLDER      816
#define ECMD_USERSTEMPLATEFOLDER    817
#define ECMD_WEBCUSTOMFOLDER        818
#define ECMD_WINDOWSFOLDER          819
#define ECMD_SPECIALFOLDERLAST      819
// End of deploy sub-group
//
// Shareable commands originating in the Visual Studio Analyzer project
//
#define ECMD_EXPORTEVENTS           900
#define ECMD_IMPORTEVENTS           901
#define ECMD_VIEWEVENT              902
#define ECMD_VIEWEVENTLIST          903
#define ECMD_VIEWCHART              904
#define ECMD_VIEWMACHINEDIAGRAM     905
#define ECMD_VIEWPROCESSDIAGRAM     906
#define ECMD_VIEWSOURCEDIAGRAM      907
#define ECMD_VIEWSTRUCTUREDIAGRAM   908
#define ECMD_VIEWTIMELINE           909
#define ECMD_VIEWSUMMARY            910
#define ECMD_APPLYFILTER            911
#define ECMD_CLEARFILTER            912
#define ECMD_STARTRECORDING         913
#define ECMD_STOPRECORDING          914
#define ECMD_PAUSERECORDING         915
#define ECMD_ACTIVATEFILTER         916
#define ECMD_SHOWFIRSTEVENT         917
#define ECMD_SHOWPREVIOUSEVENT      918
#define ECMD_SHOWNEXTEVENT          919
#define ECMD_SHOWLASTEVENT          920
#define ECMD_REPLAYEVENTS           921
#define ECMD_STOPREPLAY             922
#define ECMD_INCREASEPLAYBACKSPEED  923
#define ECMD_DECREASEPLAYBACKSPEED  924
#define ECMD_ADDMACHINE             925
#define ECMD_ADDREMOVECOLUMNS       926
#define ECMD_SORTCOLUMNS            927
#define ECMD_SAVECOLUMNSETTINGS     928
#define ECMD_RESETCOLUMNSETTINGS    929
#define ECMD_SIZECOLUMNSTOFIT       930
#define ECMD_AUTOSELECT             931
#define ECMD_AUTOFILTER             932
#define ECMD_AUTOPLAYTRACK          933
#define ECMD_GOTOEVENT              934
#define ECMD_ZOOMTOFIT              935
#define ECMD_ADDGRAPH               936
#define ECMD_REMOVEGRAPH            937
#define ECMD_CONNECTMACHINE         938
#define ECMD_DISCONNECTMACHINE      939
#define ECMD_EXPANDSELECTION        940
#define ECMD_COLLAPSESELECTION      941
#define ECMD_ADDFILTER              942
#define ECMD_ADDPREDEFINED0         943
#define ECMD_ADDPREDEFINED1         944
#define ECMD_ADDPREDEFINED2         945
#define ECMD_ADDPREDEFINED3         946
#define ECMD_ADDPREDEFINED4         947
#define ECMD_ADDPREDEFINED5         948
#define ECMD_ADDPREDEFINED6         949
#define ECMD_ADDPREDEFINED7         950
#define ECMD_ADDPREDEFINED8         951
#define ECMD_TIMELINESIZETOFIT      952

//
// Shareable commands originating with Crystal Reports
//
#define ECMD_FIELDVIEW             1000
#define ECMD_SELECTEXPERT          1001
#define ECMD_TOPNEXPERT            1002
#define ECMD_SORTORDER             1003
#define ECMD_PROPPAGE              1004
#define ECMD_HELP                  1005
#define ECMD_SAVEREPORT            1006
#define ECMD_INSERTSUMMARY         1007
#define ECMD_INSERTGROUP           1008
#define ECMD_INSERTSUBREPORT       1009
#define ECMD_INSERTCHART           1010
#define ECMD_INSERTPICTURE         1011
//
// Shareable commands from the common project area (DirPrj)
//
#define ECMD_SETASSTARTPAGE        1100
#define ECMD_RECALCULATELINKS      1101
#define ECMD_WEBPERMISSIONS        1102
#define ECMD_COMPARETOMASTER       1103
#define ECMD_WORKOFFLINE           1104
#define ECMD_SYNCHRONIZEFOLDER     1105
#define ECMD_SYNCHRONIZEALLFOLDERS 1106
#define ECMD_COPYPROJECT           1107
#define ECMD_IMPORTFILEFROMWEB     1108
#define ECMD_INCLUDEINPROJECT      1109
#define ECMD_EXCLUDEFROMPROJECT    1110
#define ECMD_BROKENLINKSREPORT     1111
#define ECMD_ADDPROJECTOUTPUTS     1112
#define ECMD_ADDREFERENCE          1113
#define ECMD_ADDWEBREFERENCE       1114
#define ECMD_ADDWEBREFERENCECTX    1115
#define ECMD_UPDATEWEBREFERENCE    1116
#define ECMD_RUNCUSTOMTOOL         1117 
//
// Shareable commands for right drag operations
//
#define ECMD_DRAG_MOVE             1140
#define ECMD_DRAG_COPY             1141
#define ECMD_DRAG_CANCEL           1142

//
// Shareable commands from the VC resource editor
//
#define ECMD_TESTDIALOG            1200
#define ECMD_SPACEACROSS           1201
#define ECMD_SPACEDOWN             1202
#define ECMD_TOGGLEGRID            1203
#define ECMD_TOGGLEGUIDES          1204
#define ECMD_SIZETOTEXT            1205
#define ECMD_CENTERVERT            1206
#define ECMD_CENTERHORZ            1207
#define ECMD_FLIPDIALOG            1208
#define ECMD_SETTABORDER           1209
#define ECMD_BUTTONRIGHT           1210
#define ECMD_BUTTONBOTTOM          1211
#define ECMD_AUTOLAYOUTGROW        1212
#define ECMD_AUTOLAYOUTNORESIZE    1213
#define ECMD_AUTOLAYOUTOPTIMIZE    1214
#define ECMD_GUIDESETTINGS         1215
#define ECMD_RESOURCEINCLUDES      1216
#define ECMD_RESOURCESYMBOLS       1217
#define ECMD_OPENBINARY            1218
#define ECMD_RESOURCEOPEN          1219
#define ECMD_RESOURCENEW           1220
#define ECMD_RESOURCENEWCOPY       1221
#define ECMD_INSERT                1222
#define ECMD_EXPORT                1223
#define ECMD_CTLMOVELEFT           1224
#define ECMD_CTLMOVEDOWN           1225
#define ECMD_CTLMOVERIGHT          1226
#define ECMD_CTLMOVEUP             1227
#define ECMD_CTLSIZEDOWN           1228
#define ECMD_CTLSIZEUP             1229
#define ECMD_CTLSIZELEFT           1230
#define ECMD_CTLSIZERIGHT          1231
#define ECMD_NEWACCELERATOR        1232
#define ECMD_CAPTUREKEYSTROKE      1233
#define ECMD_INSERTACTIVEXCTL      1234
#define ECMD_INVERTCOLORS          1235
#define ECMD_FLIPHORIZONTAL        1236
#define ECMD_FLIPVERTICAL          1237
#define ECMD_ROTATE90              1238
#define ECMD_SHOWCOLORSWINDOW      1239
#define ECMD_NEWSTRING             1240
#define ECMD_NEWINFOBLOCK          1241
#define ECMD_DELETEINFOBLOCK       1242
#define ECMD_ADJUSTCOLORS          1243
#define ECMD_LOADPALETTE           1244
#define ECMD_SAVEPALETTE           1245
#define ECMD_CHECKMNEMONICS        1246
#define ECMD_DRAWOPAQUE            1247
#define ECMD_TOOLBAREDITOR         1248
#define ECMD_GRIDSETTINGS          1249
#define ECMD_NEWDEVICEIMAGE        1250
#define ECMD_OPENDEVICEIMAGE       1251
#define ECMD_DELETEDEVICEIMAGE     1252
#define ECMD_VIEWASPOPUP           1253
#define ECMD_CHECKMENUMNEMONICS    1254
#define ECMD_SHOWIMAGEGRID         1255
#define ECMD_SHOWTILEGRID          1256
#define ECMD_MAGNIFY               1257
#define cmdidResProps              1258
//
// Shareable commands from the VC resource editor (Image editor toolbar)
//
#define ECMD_PICKRECTANGLE         1300
#define ECMD_PICKREGION            1301
#define ECMD_PICKCOLOR             1302
#define ECMD_ERASERTOOL            1303
#define ECMD_FILLTOOL              1304
#define ECMD_PENCILTOOL            1305
#define ECMD_BRUSHTOOL             1306
#define ECMD_AIRBRUSHTOOL          1307
#define ECMD_LINETOOL              1308
#define ECMD_CURVETOOL             1309
#define ECMD_TEXTTOOL              1310
#define ECMD_RECTTOOL              1311
#define ECMD_OUTLINERECTTOOL       1312
#define ECMD_FILLEDRECTTOOL        1313
#define ECMD_ROUNDRECTTOOL         1314
#define ECMD_OUTLINEROUNDRECTTOOL  1315
#define ECMD_FILLEDROUNDRECTTOOL   1316
#define ECMD_ELLIPSETOOL           1317
#define ECMD_OUTLINEELLIPSETOOL    1318
#define ECMD_FILLEDELLIPSETOOL     1319
#define ECMD_SETHOTSPOT            1320
#define ECMD_ZOOMTOOL              1321
#define ECMD_ZOOM1X                1322
#define ECMD_ZOOM2X                1323
#define ECMD_ZOOM6X                1324
#define ECMD_ZOOM8X                1325
#define ECMD_TRANSPARENTBCKGRND    1326
#define ECMD_OPAQUEBCKGRND         1327
//---------------------------------------------------
// The commands ECMD_ERASERSMALL thru ECMD_LINELARGER
// must be left in the same order for the use of the
// Resource Editor - They may however be relocated as
// a complete block
//---------------------------------------------------
#define ECMD_ERASERSMALL           1328
#define ECMD_ERASERMEDIUM          1329
#define ECMD_ERASERLARGE           1330
#define ECMD_ERASERLARGER          1331
#define ECMD_CIRCLELARGE           1332
#define ECMD_CIRCLEMEDIUM          1333
#define ECMD_CIRCLESMALL           1334
#define ECMD_SQUARELARGE           1335
#define ECMD_SQUAREMEDIUM          1336
#define ECMD_SQUARESMALL           1337
#define ECMD_LEFTDIAGLARGE         1338
#define ECMD_LEFTDIAGMEDIUM        1339
#define ECMD_LEFTDIAGSMALL         1340
#define ECMD_RIGHTDIAGLARGE        1341
#define ECMD_RIGHTDIAGMEDIUM       1342
#define ECMD_RIGHTDIAGSMALL        1343
#define ECMD_SPLASHSMALL           1344
#define ECMD_SPLASHMEDIUM          1345
#define ECMD_SPLASHLARGE           1346
#define ECMD_LINESMALLER           1347
#define ECMD_LINESMALL             1348
#define ECMD_LINEMEDIUM            1349
#define ECMD_LINELARGE             1350
#define ECMD_LINELARGER            1351
#define ECMD_LARGERBRUSH           1352
#define ECMD_LARGEBRUSH            1353
#define ECMD_STDBRUSH              1354
#define ECMD_SMALLBRUSH            1355
#define ECMD_SMALLERBRUSH          1356
#define ECMD_ZOOMIN                1357
#define ECMD_ZOOMOUT               1358
#define ECMD_PREVCOLOR             1359
#define ECMD_PREVECOLOR            1360
#define ECMD_NEXTCOLOR             1361
#define ECMD_NEXTECOLOR            1362
#define ECMD_IMG_OPTIONS           1363

//---------------------------------------------------

//
// Shareable commands from WINFORMS
//
#define ECMD_CANCELDRAG            1500
#define ECMD_DEFAULTACTION         1501
#define ECMD_CTLMOVEUPGRID         1502
#define ECMD_CTLMOVEDOWNGRID       1503
#define ECMD_CTLMOVELEFTGRID       1504
#define ECMD_CTLMOVERIGHTGRID      1505
#define ECMD_CTLSIZERIGHTGRID      1506
#define ECMD_CTLSIZEUPGRID         1507
#define ECMD_CTLSIZELEFTGRID       1508
#define ECMD_CTLSIZEDOWNGRID       1509
#define ECMD_NEXTCTL               1510
#define ECMD_PREVCTL               1511

//////////////////////////////////////////////////////////////////
//
// The following commands form guidDataCmdId.
// NOTE that all these commands are shareable and may be used
// in any appropriate menu.
//
//////////////////////////////////////////////////////////////////
#define icmdDesign					0x3000		// design command for project items
#define icmdDesignOn				0x3001		// design on... command for project items

#define icmdSEDesign				0x3003		// design command for the SE side
#define icmdNewDiagram				0x3004
#define icmdNewTable				0x3006

#define icmdNewDBItem				0x300E
#define icmdNewTrigger				0x3010

#define icmdDebug					0x3012
#define icmdNewProcedure			0x3013
#define icmdNewQuery				0x3014
#define icmdRefreshLocal			0x3015

#define icmdDbAddDataConnection			0x3017
#define icmdDBDefDBRef				0x3018
#define icmdRunCmd					0x3019
#define icmdRunOn					0x301A
#define icmdidNewDBRef				0x301B
#define icmdidSetAsDef				0x301C
#define icmdidCreateCmdFile			0x301D

#define icmdNewDatabase				0x3020
#define icmdNewUser					0x3021
#define icmdNewRole					0x3022
#define icmdChangeLogin				0x3023
#define icmdNewView					0x3024
#define icmdModifyConnection		0x3025
#define icmdDisconnect				0x3026
#define icmdCopyScript				0x3027
#define icmdAddSCC					0x3028
#define icmdRemoveSCC				0x3029
#define icmdGetLatest				0x3030
#define icmdCheckOut				0x3031
#define icmdCheckIn					0x3032
#define icmdUndoCheckOut			0x3033
#define icmdAddItemSCC				0x3034
#define icmdNewPackageSpec			0x3035
#define icmdNewPackageBody			0x3036
#define icmdInsertSQL				0x3037
#define icmdRunSelection			0x3038
#define icmdUpdateScript			0x3039
#define icmdCreateScript			0x303A	// to be used by db project side
#define icmdSECreateScript			0x303B	// to be used by SE side as opposed to db project side
#define icmdNewScript				0x303C
#define icmdNewFunction				0x303D
#define icmdNewTableFunction		0x303E
#define icmdNewInlineFunction		0x303F

#define icmdAddDiagram				0x3040
#define icmdAddTable				0x3041			
#define icmdAddSynonym				0x3042			
#define icmdAddView					0x3043
#define icmdAddProcedure			0x3044
#define icmdAddFunction				0x3045
#define icmdAddTableFunction		0x3046
#define icmdAddInlineFunction		0x3047
#define icmdAddPkgSpec				0x3048
#define icmdAddPkgBody				0x3049
#define icmdAddTrigger				0x304A
#define icmdExportData				0x304B

#define icmdDbnsVcsAdd				0x304C
#define icmdDbnsVcsRemove			0x304D
#define icmdDbnsVcsCheckout			0x304E
#define icmdDbnsVcsUndoCheckout		0x304F
#define icmdDbnsVcsCheckin			0x3050

#define icmdSERetrieveData			0x3060
#define icmdSEEditTextObject		0x3061
#define icmdSERun					0x3062	// to be used by SE side as opposed to db project side
#define icmdSERunSelection			0x3063	// to be used by SE side as opposed to db project side
#define icmdDesignSQLBlock			0x3064	

#define icmdRegisterSQLInstance		0x3065
#define icmdUnregisterSQLInstance	0x3066


//////////////////////////////////////////////////////////////////
//
// The following commands form guidDavDataCmdId.
// NOTE that all these commands are shareable and may be used
// in any appropriate menu.
//
//////////////////////////////////////////////////////////////////
#define	cmdidAddRelatedTables			0x0001
#define cmdidLayoutDiagram				0x0002
#define cmdidLayoutSelection			0x0003
#define cmdidInsertColumn				0x0004
#define cmdidDeleteColumn				0x0005
#define cmdidNewTextAnnotation			0x0006
#define cmdidShowRelLabels				0x0007
#define cmdidViewPageBreaks				0x0008
#define cmdidRecalcPageBreaks			0x0009
#define cmdidViewUserDefined			0x000a
#define cmdidGenerateQuery				0x000b
#define cmdidDeleteFromDB				0x000c
#define cmdidAutoSize					0x000d
#define cmdidEditViewUserDefined		0x000e
#define cmdidSetAnnotationFont			0x000f
#define cmdidZoomPercent200				0x0010
#define cmdidZoomPercent150				0x0011
#define cmdidZoomPercent100				0x0012
#define cmdidZoomPercent75				0x0013
#define cmdidZoomPercent50				0x0014
#define cmdidZoomPercent25				0x0015
#define cmdidZoomPercent10				0x0016
#define cmdidZoomPercentSelection		0x0017
#define cmdidZoomPercentFit				0x0018
#define cmdidInsertQBERow				0x0019
#define cmdidInsertCriteria				0x0020
#define cmdidAddTableView				0x0021
#define cmdidManageTriggers				0x0022
#define cmdidManagePermissions			0x0023
#define cmdidViewDependencies			0x0024
#define cmdidGenerateSQLScript			0x0025		
#define cmdidVerifySQLSilent			0x0026
#define cmdidAddTableViewForQRY			0x0027
#define cmdidManageIndexesForQRY		0x0028
#define cmdidViewFieldListQry			0x0029
#define cmdidViewCollapsedQry			0x002a
#define cmdidCopyDiagram				0x002b


#endif //_STDIDCMD_H_
