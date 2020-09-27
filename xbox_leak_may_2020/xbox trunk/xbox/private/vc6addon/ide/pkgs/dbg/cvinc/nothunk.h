/*** nothunk.h -- gets rid of thunks
*
*   Copyright <C> 1989, Microsoft Corporation
*
*   Purpose:
*
*   Revision History:
*
*   [xx]    30-Sep-1989 Dans	Created
*
*************************************************************************/
#if !(defined (_NOTHUNKH_))
#define _NOTHUNKH_

#define RESassemble_context		assemble_context
#define RESenter_context		enter_context
#define RESregister_context		register_context
#define ResBPDlgList			BPDlgList
#define ResBlankTextWndProc		BlankTextWndProc
#define ResBorderWndProc		BorderWndProc
#define ResButtonWndProc		ButtonWndProc
#define ResCMDFReadOnlyBuf		CWndFReadOnlyBuf
#define ResCMDGetLineAttrs		CWndGetLineAttrs
#define ResCMDInsertLineBuf		CWndInsertLineBuf
#define ResCMDLinesInBuf		CWndLinesInBuf
#define ResCMDReplaceLineBuf		CWndReplaceLineBuf
#define ResCMDcbGetLineBuf		CWndcbGetLineBuf
#define ResDialogFilterProc		DialogFilterProc
#define ResDialogWndProc		DialogWndProc
#define ResDlgListDelWatch		DlgListDelWatch
#define ResFDlgOpen				FDlgOpen
#define ResDummyFilter			DummyFilter
#define ResFDlgBPAdd			FDlgBPAdd
#define ResFDlgBPList			FDlgBPList
#define ResBPDlgRList			BPDlgRList
#define ResFDlgBPResolve		FDlgBPResolve
#define ResFDlgDelWatch 		FDlgDelWatch
#define ResFDlgFind			FDlgFind
#define ResFDlgHOvrW			FDlgHOvrW
#define ResFDlgHelp			FDlgHelp
#define ResFDlgLang			FDlgLang
#define ResFDlgModules			FDlgModules
#define ResFDlgPrint			FDlgPrint
#define ResFDlgQckWatch 		FDlgQckWatch
#define ResFDlgDebuggee		FDlgDebuggee
#define ResFDlgRun              FDlgRun
#define ResFDlgSource			FDlgSource
#define ResFDlgSysIsa		FDlgSysIsa
#define ResFDlgTrunc			FDlgTrunc
#define ResFDlghistopt			FDlghistopt
#define ResGeneralWndProc		GeneralWndProc
#define ResHLPGetLineAttrs		hlpGetLineAttrs
#define ResHLPLinesInBuf		hlpLinesInBuf
#define ResHLPcbGetLineBuf		hlpcbGetLineBuf
#define ResHelpTextWndProc		HelpTextWndProc
#define ResMWTypeList			MWTypeList
#define ResLCLFDlgLocalOptions  LCLFDlgLocalOptions
#define ResWATGetLineAttrs		WATGetLineAttrs
#define ResLOCFReadOnlyBuf		LTFReadOnlyBuf
#define ResLOCFReplaceLineBuf		LTReplaceLineBuf
#define ResLOCGetLineAttrs		LTGetLineAttrs
#define ResLOCLinesInBuf		LTLinesInBuf
#define ResLOCcbGetLineBuf		LTcbGetLineBuf
#define ResListBoxWndProc		ListBoxWndProc
#define ResMboxWndProc			MboxWndProc
#define ResMenuFilterProc		MenuFilterProc
#define ResModulesDlgList		ModulesDlgList
#define ResSourcesDlgList       SourcesDlgList
#define ResMWCbGetLineBuf		MWCbGetLineBuf
#define ResMWCLinesInBuf		MWCLinesInBuf
#define ResMWGetLineAttrs		MWGetLineAttrs
#define ResMWFDlgMwEdit			MWFDlgMwEdit
#define ResMWFDlgMwOption       MWFDlgMwOption
#define ResMWTypeListProc		MWTypeListProc
#define ResParentKeyResizeWndProc	ParentKeyResizeWndProc
#define ResParentMouResizeWndProc	ParentMouResizeWndProc
#define ResParentWndProc		ParentWndProc
#define ResQckWatchListProc		QckWatchListProc
#define ResR87GetLineAttrs		reg87GetLineAttrs
#define ResR87LinesInBuf		reg87LinesInBuf
#define ResR87cbGetLineBuf		reg87cbGetLineBuf
#define ResREGGetLineAttrs		regGetLineAttrs
#define ResREGLinesInBuf		regLinesInBuf
#define ResREGcbGetLineBuf		regcbGetLineBuf
#define ResResizeTextWndProc		ResizeTextWndProc
#define ResSRCGetLineAttrs		SWGetLineAttrs
#define ResSRCLinesInBuf		SWLinesInBuf
#define ResSRCcbGetLineBuf		SWcbGetLineBuf
#define ResScrollBarWndProc		ScrollBarWndProc
#define ResSdmDialogProc		SdmDialogProc
#define ResStaticWndProc		StaticWndProc
#define ResStatusWndProc		StatusWndProc
#define ResTextWndProc			TextWndProc

#ifdef OSDEBUG
#define ResCNFKillNode          CNFKillNode
#define ResCNFWMultiListProc	  CNFWMultiListProc
#define ResCNFWSingleListProc	  CNFWSingleListProc
#define ResCNFWMultiDllListProc CNFWMultiDllListProc
#define ResCNFFDlgConfig        CNFFDlgConfig
#define ResCNFFDlgSingle        CNFFDlgSingle
#define ResCNFFDlgMulti         CNFFDlgMulti
#endif	// OSDEBUG

#endif	// _NOTHUNKH_
