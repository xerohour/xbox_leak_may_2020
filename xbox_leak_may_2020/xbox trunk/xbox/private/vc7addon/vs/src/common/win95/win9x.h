//---------------------------------------------------------------------------
//  Microsoft Confidential
//  Copyright (C) Microsoft Corporation, 1993 - 1999.  All Rights Reserved.
//
//  File:       win9x.h
//
//  Contents:   Header file to be included by clients of the static lib UNICODE
//              wrapper win9x(d).lib
//
//  Owner: Microsoft Visual Studio
//
//  Contacts: RahulK, KSrini, larsberg
//
//  All changes and additions must be approved by one of the contacts above.
//----------------------------------------------------------------------------

#include <mbstring.h>
#include <prsht.h>
#include <commctrl.h>
#include <shlobj.h>
#include <nspapi.h>
#include <oledlg.h>
#include <ras.h>
#include <vfw.h>

// Static Win9x Wrapper Initializer
BOOL WINAPI Win9xInit (HANDLE    hDLL,DWORD     dwReason,LPVOID    lpReserved);

#ifdef MAPWAPIS

//
//  The "GOOD" APIs i.e. the ones that are wrapped
//

#define AppendMenuW OAppendMenuW
#define CallWindowProcW OCallWindowProcW
#define CharLowerBuffW OCharLowerBuffW
#define CharLowerW OCharLowerW
#define CharNextW OCharNextW
#define CharPrevW OCharPrevW
#define CharToOemW OCharToOemW
#define CharUpperBuffW OCharUpperBuffW
#define CharUpperW OCharUpperW
#define ChooseColorW OChooseColorW
#define ChooseFontW OChooseFontW
#define CompareStringW OCompareStringW
#define CopyAcceleratorTableW OCopyAcceleratorTableW
#define CopyFileW OCopyFileW
#define CopyMetaFileW OCopyMetaFileW
#define CreateMutexW OCreateMutexW
#define CreateAcceleratorTableW OCreateAcceleratorTableW
#define CreateDCW OCreateDCW
#define CreateDialogIndirectParamW OCreateDialogIndirectParamW
#define CreateDialogParamW OCreateDialogParamW
#define CreateDirectoryExW OCreateDirectoryExW
#define CreateDirectoryW OCreateDirectoryW
#define CreateEnhMetaFileW OCreateEnhMetaFileW
#define CreateEventW OCreateEventW
#define CreateFileMappingW OCreateFileMappingW
#define CreateFileW OCreateFileW
#define CreateFontIndirectW OCreateFontIndirectW
#define CreateFontW OCreateFontW
#define CreateICW OCreateICW
#define CreateMDIWindowW OCreateMDIWindowW
#define CreateMetaFileW OCreateMetaFileW
#define CreateProcessW OCreateProcessW
#define CreatePropertySheetPageW OCreatePropertySheetPageW
#define CreateSemaphoreW OCreateSemaphoreW
#define CreateWindowExW OCreateWindowExW
#define DdeCreateStringHandleW ODdeCreateStringHandleW
#define DdeInitializeW ODdeInitializeW
#define DefDlgProcW ODefDlgProcW
#define DefFrameProcW ODefFrameProcW
#define DefMDIChildProcW ODefMDIChildProcW
#define DefWindowProcW ODefWindowProcW
#define DeleteFileW ODeleteFileW
#define DialogBoxIndirectParamW ODialogBoxIndirectParamW
#define DialogBoxParamW ODialogBoxParamW
#define DispatchMessageW ODispatchMessageW
#define DocumentPropertiesW ODocumentPropertiesW
#define DragQueryFileW ODragQueryFileW
#define DrawTextExW ODrawTextExW
#define DrawTextW ODrawTextW
#define EnableWindow OEnableWindow
#define ExtractAssociatedIconW OExtractAssociatedIconW
#define ExtractIconW OExtractIconW
#define ExtractIconExW OExtractIconExW
#define ExtTextOutW OExtTextOutW
#define ExpandEnvironmentStringsW OExpandEnvironmentStringsW
#define FatalAppExitW OFatalAppExitW
#define FindFirstChangeNotificationW OFindFirstChangeNotificationW
#define FindExecutableW OFindExecutableW
#define FindFirstFileW OFindFirstFileW
#define FindNextFileW OFindNextFileW
#define FindResourceW OFindResourceW
#define FindWindowW OFindWindowW
#define FormatMessageW OFormatMessageW
#define FreeEnvironmentStringsW OFreeEnvironmentStringsW
#define GetCharABCWidthsFloatW OGetCharABCWidthsFloatW
#define GetCharABCWidthsW OGetCharABCWidthsW
#define GetCharWidthFloatW OGetCharWidthFloatW
#define GetCharWidthW OGetCharWidthW
#define GetClassInfoW OGetClassInfoW
#define GetClassInfoExW OGetClassInfoExW
#define GetClassLongW OGetClassLongW
#define GetClassNameW OGetClassNameW
#define GetClipboardFormatNameW OGetClipboardFormatNameW
#define GetComputerNameW OGetComputerNameW
#define GetCurrentDirectoryW OGetCurrentDirectoryW
#define GetCurrencyFormatW OGetCurrencyFormatW
#define GetDateFormatW OGetDateFormatW
#define GetDlgItemTextW OGetDlgItemTextW
#define GetDriveTypeW OGetDriveTypeW
#define GetEnvironmentVariableW OGetEnvironmentVariableW
#define GetEnvironmentStringsW OGetEnvironmentStringsW
#define GetFileAttributesW OGetFileAttributesW
#define GetFileAttributesExW OGetFileAttributesExW
#define GetFileTitleW OGetFileTitleW
#define GetFileVersionInfoSizeW OGetFileVersionInfoSizeW
#define GetFileVersionInfoW OGetFileVersionInfoW
#define GetFullPathNameW OGetFullPathNameW
#define GetGlyphOutlineW OGetGlyphOutlineW
#define GetKerningPairsW OGetKerningPairsW
#define GetLocaleInfoW OGetLocaleInfoW
#define GetMessageW OGetMessageW
#define GetModuleFileNameW OGetModuleFileNameW
#define GetModuleHandleW OGetModuleHandleW
#define GetNumberFormatW OGetNumberFormatW
#define GetObjectW OGetObjectW
#define GetOpenFileNameW OGetOpenFileNameW
#define GetOutlineTextMetricsW OGetOutlineTextMetricsW
#define GetPrivateProfileIntW OGetPrivateProfileIntW
#define GetPrivateProfileStringW OGetPrivateProfileStringW
#define GetProfileIntW OGetProfileIntW
#define GetPropW OGetPropW
#define GetSaveFileNameW OGetSaveFileNameW
#define GetShortPathNameW OGetShortPathNameW
#define GetStartupInfoW OGetStartupInfoW
#define GetStringTypeExW OGetStringTypeExW
#define GetSystemDirectoryW OGetSystemDirectoryW
#define GetTabbedTextExtentW OGetTabbedTextExtentW
#define GetTempFileNameW OGetTempFileNameW
#define GetTempPathW OGetTempPathW
#define GetTextExtentPoint32W OGetTextExtentPoint32W
#define GetTextExtentPointW OGetTextExtentPointW
#define GetTextExtentExPointW OGetTextExtentExPointW
#define GetTextFaceW OGetTextFaceW
#define GetTextMetricsW OGetTextMetricsW
#define GetTimeFormatW OGetTimeFormatW
#define GetUserNameW OGetUserNameW
#define GetVersionExW OGetVersionExW
#define GetVolumeInformationW OGetVolumeInformationW
#define GetWindowLongW OGetWindowLongW
#define GetWindowTextLengthW OGetWindowTextLengthW
#define GetWindowTextW OGetWindowTextW
#define GlobalAddAtomW OGlobalAddAtomW
#define GlobalFindAtomW OGlobalFindAtomW
#define GlobalGetAtomNameW OGlobalGetAtomNameW
#define GrayStringW OGrayStringW
#define ImageList_LoadImageW OImageList_LoadImageW
#define ImmGetCompositionStringW OImmGetCompositionStringW
#define InsertMenuW OInsertMenuW
#define IsBadStringPtrW OIsBadStringPtrW
#define IsCharAlphaNumericW OIsCharAlphaNumericW
#define IsCharAlphaW OIsCharAlphaW
#define IsDialogMessageW OIsDialogMessageW
#define LCMapStringW OLCMapStringW
#define LoadAcceleratorsW OLoadAcceleratorsW
#define LoadBitmapW OLoadBitmapW
#define LoadCursorW OLoadCursorW
#define LoadIconW OLoadIconW
#define LoadImageW OLoadImageW
#define LoadLibraryExW OLoadLibraryExW
#define LoadLibraryW OLoadLibraryW
#define LoadMenuIndirectW OLoadMenuIndirectW
#define LoadMenuW OLoadMenuW
#define LoadStringW OLoadStringW
#define lstrcatW OlstrcatW
#define lstrcmpiW OlstrcmpiW
#define lstrcmpW OlstrcmpW
#define lstrcpynW OlstrcpynW
#define lstrcpyW OlstrcpyW
#define lstrlenW OlstrlenW
#define MapVirtualKeyW OMapVirtualKeyW
#define MessageBoxW OMessageBoxW
#define MessageBoxIndirectW OMessageBoxIndirectW
#define ModifyMenuW OModifyMenuW
#define MoveFileExW OMoveFileExW
#define MoveFileW OMoveFileW
#define OemToCharW OOemToCharW
#define OpenEventW OOpenEventW
#define OpenMutexW OOpenMutexW
#define OpenPrinterW OOpenPrinterW
#define OutputDebugStringW OOutputDebugStringW
#define PeekMessageW OPeekMessageW
#define PostMessageW OPostMessageW
#define PostThreadMessageW OPostThreadMessageW
#define PrintDlgW OPrintDlgW
#define PropertySheetW OPropertySheetW
#define RegCreateKeyExW ORegCreateKeyExW
#define RegCreateKeyW ORegCreateKeyW
#define RegDeleteKeyW ORegDeleteKeyW
#define RegDeleteValueW ORegDeleteValueW
#define RegEnumKeyW ORegEnumKeyW
#define RegEnumValueW ORegEnumValueW
#define RegEnumKeyExW ORegEnumKeyExW
#define RegisterClassW ORegisterClassW
#define RegisterClassExW ORegisterClassExW
#define RegisterClipboardFormatW ORegisterClipboardFormatW
#define RegisterWindowMessageW ORegisterWindowMessageW
#define RegOpenKeyExW ORegOpenKeyExW
#define RegOpenKeyW ORegOpenKeyW
#define RegQueryInfoKeyW ORegQueryInfoKeyW
#define RegQueryValueExW ORegQueryValueExW
#define RegQueryValueW ORegQueryValueW
#define RegSetValueExW ORegSetValueExW
#define RegSetValueW ORegSetValueW
#define RemovePropW ORemovePropW
#define ResetDCW OResetDCW
#define SendDlgItemMessageW OSendDlgItemMessageW
#define SendMessageW OSendMessageW
#define SendMessageTimeoutW OSendMessageTimeoutW
#define SendNotifyMessageW OSendNotifyMessageW
#define SetClassLongW OSetClassLongW
#define SetCurrentDirectoryW OSetCurrentDirectoryW
#define SetDlgItemTextW OSetDlgItemTextW
#define SetEnvironmentVariableW OSetEnvironmentVariableW
#define SetFileAttributesW OSetFileAttributesW
#define SetLocaleInfoW OSetLocaleInfoW
#define SetMenuItemInfoW OSetMenuItemInfoW
#define SetPropW OSetPropW
#define SetWindowLongW OSetWindowLongW
#define SetWindowsHookExW OSetWindowsHookExW
#define SetWindowTextW OSetWindowTextW
#define SHBrowseForFolderW OSHBrowseForFolderW
#define Shell_NotifyIconW OShell_NotifyIconW
#define ShellExecuteExW OShellExecuteExW
#define SHGetFileInfoW OSHGetFileInfoW
#define SHGetPathFromIDListW OSHGetPathFromIDListW
#define StartDocW OStartDocW
#define StgCreateDocfile OStgCreateDocfile
#define SystemParametersInfoW OSystemParametersInfoW
#define TabbedTextOutW OTabbedTextOutW
#define TranslateAcceleratorW OTranslateAcceleratorW
#define UnregisterClassW OUnregisterClassW
#define VerLanguageNameW OVerLanguageNameW
#define VerQueryValueW OVerQueryValueW
#define VkKeyScanW OVkKeyScanW
#define WinHelpW OWinHelpW
#define WNetGetConnectionW OWNetGetConnectionW
#define WritePrivateProfileStringW OWritePrivateProfileStringW
#define wvsprintfW OwvsprintfW
#define wsprintfW OwsprintfW


//
//  The "BAD" APIs i.e. the ones that are NOT wrapped
//

#ifdef DEBUG

#define AbortSystemShutdownW OAbortSystemShutdownW
#define AccessCheckAndAuditAlarmW OAccessCheckAndAuditAlarmW
#define AddFontResourceW OAddFontResourceW
#define AddFormW OAddFormW
#define AddJobW OAddJobW
#define AddMonitorW OAddMonitorW
#define AddPortW OAddPortW
#define AddPrinterConnectionW OAddPrinterConnectionW
#define AddPrinterDriverW OAddPrinterDriverW
#define AddPrinterW OAddPrinterW
#define AddPrintProcessorW OAddPrintProcessorW
#define AddPrintProvidorW OAddPrintProvidorW
#define AdvancedDocumentPropertiesW OAdvancedDocumentPropertiesW
#define auxGetDevCapsW OauxGetDevCapsW
#define AVIBuildFilterW OAVIBuildFilterW
#define AVIFileCreateStreamW OAVIFileCreateStreamW
#define AVIFileInfoW OAVIFileInfoW
#define AVISaveW OAVISaveW
#define AVIStreamInfoW OAVIStreamInfoW
#define AVIStreamOpenFromFileW OAVIStreamOpenFromFileW
#define BackupEventLogW OBackupEventLogW
#define BeginUpdateResourceW OBeginUpdateResourceW
#define BuildCommDCBAndTimeoutsW OBuildCommDCBAndTimeoutsW
#define BuildCommDCBW OBuildCommDCBW
#define CallMsgFilterW OCallMsgFilterW
#define CallNamedPipeW OCallNamedPipeW
#define capCreateCaptureWindowW OcapCreateCaptureWindowW
#define ChangeDisplaySettingsW OChangeDisplaySettingsW
#define ChangeMenuW OChangeMenuW
#define CharToOemBuffW OCharToOemBuffW
#define ClearEventLogW OClearEventLogW
#define CommConfigDialogW OCommConfigDialogW
#define ConfigurePortW OConfigurePortW
#define CopyEnhMetaFileW OCopyEnhMetaFileW
#define CreateColorSpaceW OCreateColorSpaceW
#define CreateDesktopW OCreateDesktopW
#define CreateMailslotW OCreateMailslotW
#define CreateNamedPipeW OCreateNamedPipeW
#define CreateProcessAsUserW OCreateProcessAsUserW
#define CreateScalableFontResourceW OCreateScalableFontResourceW
#define CreateStatusWindowW OCreateStatusWindowW
#define CreateWindowStationW OCreateWindowStationW
#define DceErrorInqTextW ODceErrorInqTextW
#define DdeQueryStringW   ODdeQueryStringW
#define DefineDosDeviceW ODefineDosDeviceW
#define DeleteFormW ODeleteFormW
#define DeleteMonitorW ODeleteMonitorW
#define DeletePortW ODeletePortW
#define DeletePrinterConnectionW ODeletePrinterConnectionW
#define DeletePrinterDriverW ODeletePrinterDriverW
#define DeletePrintProcessorW ODeletePrintProcessorW
#define DeletePrintProvidorW ODeletePrintProvidorW
#define DeviceCapabilitiesW ODeviceCapabilitiesW
#define DlgDirListComboBoxW ODlgDirListComboBoxW
#define DlgDirListW ODlgDirListW
#define DlgDirSelectComboBoxExW ODlgDirSelectComboBoxExW
#define DlgDirSelectExW ODlgDirSelectExW
#define DoEnvironmentSubstW ODoEnvironmentSubstW
#define DrawStateW ODrawStateW
#define EditStreamSetInfoW OEditStreamSetInfoW
#define EndUpdateResourceW OEndUpdateResourceW
#define EnumCalendarInfoW OEnumCalendarInfoW
#define EnumDateFormatsW OEnumDateFormatsW
#define EnumDesktopsW OEnumDesktopsW
#define EnumDisplaySettingsW OEnumDisplaySettingsW
#define EnumFontFamiliesExW OEnumFontFamiliesExW
#define EnumFontFamiliesW OEnumFontFamiliesW
#define EnumFontsW OEnumFontsW
#define EnumFormsW OEnumFormsW
#define EnumICMProfilesW OEnumICMProfilesW
#define EnumJobsW OEnumJobsW
#define EnumMonitorsW OEnumMonitorsW
#define EnumPortsW OEnumPortsW
#define EnumPrinterDriversW OEnumPrinterDriversW
#define EnumPrintersW OEnumPrintersW
#define EnumPrintProcessorDatatypesW OEnumPrintProcessorDatatypesW
#define EnumPrintProcessorsW OEnumPrintProcessorsW
#define EnumPropsExW OEnumPropsExW
#define EnumPropsW OEnumPropsW
#define EnumProtocolsW OEnumProtocolsW
#define EnumResourceLanguagesW OEnumResourceLanguagesW
#define EnumResourceNamesW OEnumResourceNamesW
#define EnumResourceTypesW OEnumResourceTypesW
#define EnumSystemCodePagesW OEnumSystemCodePagesW
#define EnumSystemLocalesW OEnumSystemLocalesW
#define EnumTimeFormatsW OEnumTimeFormatsW
#define EnumWindowStationsW OEnumWindowStationsW
#define FillConsoleOutputCharacterW OFillConsoleOutputCharacterW
#define FindEnvironmentStringW OFindEnvironmentStringW
#define FindResourceExW OFindResourceExW
#define FindTextW OFindTextW
#define FindWindowExW OFindWindowExW
#define FoldStringW OFoldStringW
#define GetAddressByNameW OGetAddressByNameW
#define GetBinaryTypeW OGetBinaryTypeW
#define GetCharacterPlacementW OGetCharacterPlacementW
#define GetCharWidth32W OGetCharWidth32W
#define GetCommandLineW OGetCommandLineW
#define GetCompressedFileSizeW OGetCompressedFileSizeW
#define GetConsoleTitleW OGetConsoleTitleW
#define GetDefaultCommConfigW OGetDefaultCommConfigW
#define GetDiskFreeSpaceW OGetDiskFreeSpaceW
#define GetEnhMetaFileDescriptionW OGetEnhMetaFileDescriptionW
#define GetEnhMetaFileW OGetEnhMetaFileW
#define GetExpandedNameW OGetExpandedNameW
#define GetFileSecurityW OGetFileSecurityW
#define GetFormW OGetFormW
#define GetICMProfileW OGetICMProfileW
#define GetJobW OGetJobW
#define GetKeyboardLayoutNameW OGetKeyboardLayoutNameW
#define GetKeyNameTextW OGetKeyNameTextW
#define GetLogColorSpaceW OGetLogColorSpaceW
#define GetLogicalDriveStringsW OGetLogicalDriveStringsW
#define GetMenuItemInfoW OGetMenuItemInfoW
#define GetMenuStringW OGetMenuStringW
#define GetMetaFileW OGetMetaFileW
#define GetNameByTypeW OGetNameByTypeW
#define GetNamedPipeHandleStateW OGetNamedPipeHandleStateW
#define GetPrinterDataW OGetPrinterDataW
#define GetPrinterDriverDirectoryW OGetPrinterDriverDirectoryW
#define GetPrinterDriverW OGetPrinterDriverW
#define GetPrinterW OGetPrinterW
#define GetPrintProcessorDirectoryW OGetPrintProcessorDirectoryW
#define GetPrivateProfileSectionNamesW OGetPrivateProfileSectionNamesW
#define GetPrivateProfileSectionW OGetPrivateProfileSectionW
#define GetPrivateProfileStructW OGetPrivateProfileStructW
#define GetProfileSectionW OGetProfileSectionW
#define GetProfileStringW OGetProfileStringW
#define GetServiceW OGetServiceW
#define GetTypeByNameW OGetTypeByNameW
#define GetUserObjectInformationW OGetUserObjectInformationW
#define GetWindowsDirectoryW OGetWindowsDirectoryW
#define ImmConfigureIMEW OImmConfigureIMEW
#define ImmEnumRegisterWordW OImmEnumRegisterWordW
#define ImmEscapeW OImmEscapeW
#define ImmGetCandidateListCountW OImmGetCandidateListCountW
#define ImmGetCandidateListW OImmGetCandidateListW
#define ImmGetCompositionFontW OImmGetCompositionFontW
#define ImmGetConversionListW OImmGetConversionListW
#define ImmGetDescriptionW OImmGetDescriptionW
#define ImmGetGuideLineW OImmGetGuideLineW
#define ImmGetIMEFileNameW OImmGetIMEFileNameW
#define ImmGetRegisterWordStyleW OImmGetRegisterWordStyleW
#define ImmInstallIMEW OImmInstallIMEW
#define ImmIsUIMessageW OImmIsUIMessageW
#define ImmRegisterWordW OImmRegisterWordW
#define ImmSetCompositionFontW OImmSetCompositionFontW
#define ImmSetCompositionStringW OImmSetCompositionStringW
#define ImmUnregisterWordW OImmUnregisterWordW
#define InitiateSystemShutdownW OInitiateSystemShutdownW
#define InsertMenuItemW OInsertMenuItemW
#define IsCharLowerW OIsCharLowerW
#define IsCharUpperW OIsCharUpperW
#define I_RpcServerUnregisterEndpointW OI_RpcServerUnregisterEndpointW
#define joyGetDevCapsW OjoyGetDevCapsW
#define LoadCursorFromFileW OLoadCursorFromFileW
#define LoadKeyboardLayoutW OLoadKeyboardLayoutW
#define LogonUserW OLogonUserW
#define LZOpenFileW OLZOpenFileW
#define MapVirtualKeyExW OMapVirtualKeyExW
#define mciGetDeviceIDFromElementIDW OmciGetDeviceIDFromElementIDW
#define mciGetDeviceIDW OmciGetDeviceIDW
#define mciGetErrorStringW OmciGetErrorStringW
#define mciSendCommandW OmciSendCommandW
#define mciSendStringW OmciSendStringW
#define MCIWndCreateW OMCIWndCreateW
#define midiInGetDevCapsW OmidiInGetDevCapsW
#define midiInGetErrorTextW OmidiInGetErrorTextW
#define midiOutGetDevCapsW OmidiOutGetDevCapsW
#define midiOutGetErrorTextW OmidiOutGetErrorTextW
#define MIMEAssociationDialogW OMIMEAssociationDialogW
#define mixerGetControlDetailsW OmixerGetControlDetailsW
#define mixerGetDevCapsW OmixerGetDevCapsW
#define mixerGetLineControlsW OmixerGetLineControlsW
#define mixerGetLineInfoW OmixerGetLineInfoW
#define mmioInstallIOProcW OmmioInstallIOProcW
#define mmioOpenW OmmioOpenW
#define mmioRenameW OmmioRenameW
#define mmioStringToFOURCCW OmmioStringToFOURCCW
#define MultinetGetConnectionPerformanceW OMultinetGetConnectionPerformanceW
#define ObjectCloseAuditAlarmW OObjectCloseAuditAlarmW
#define ObjectOpenAuditAlarmW OObjectOpenAuditAlarmW
#define ObjectPrivilegeAuditAlarmW OObjectPrivilegeAuditAlarmW
#define OemToCharBuffW OOemToCharBuffW
#define OleUIAddVerbMenuW OOleUIAddVerbMenuW
#define OleUIEditLinksW OOleUIEditLinksW
#define OleUIPasteSpecialW OOleUIPasteSpecialW
#define OleUIPromptUserW OOleUIPromptUserW
#define OleUIUpdateLinksW OOleUIUpdateLinksW
#define OpenBackupEventLogW OOpenBackupEventLogW
#define OpenDesktopW OOpenDesktopW
#define OpenEventLogW OOpenEventLogW
#define OpenFileMappingW OOpenFileMappingW
#define OpenSemaphoreW OOpenSemaphoreW
#define OpenWindowStationW OOpenWindowStationW
#define PageSetupDlgW OPageSetupDlgW
#define PeekConsoleInputW OPeekConsoleInputW
#define PolyTextOutW OPolyTextOutW
#define PrinterMessageBoxW OPrinterMessageBoxW
#define PrivilegedServiceAuditAlarmW OPrivilegedServiceAuditAlarmW
#define QueryDosDeviceW OQueryDosDeviceW
#define RasDialW ORasDialW
#define ReadConsoleInputW OReadConsoleInputW
#define ReadConsoleOutputCharacterW OReadConsoleOutputCharacterW
#define ReadConsoleOutputW OReadConsoleOutputW
#define ReadConsoleW OReadConsoleW
#define ReadEventLogW OReadEventLogW
#define RegConnectRegistryW ORegConnectRegistryW
#define RegisterEventSourceW ORegisterEventSourceW
#define RegLoadKeyW ORegLoadKeyW
#define RegQueryMultipleValuesW ORegQueryMultipleValuesW
#define RegReplaceKeyW ORegReplaceKeyW
#define RegRestoreKeyW ORegRestoreKeyW
#define RegSaveKeyW ORegSaveKeyW
#define RegUnLoadKeyW ORegUnLoadKeyW
#define RemoveDirectoryW ORemoveDirectoryW
#define RemoveFontResourceW ORemoveFontResourceW
#define ReplaceTextW OReplaceTextW
#define ReportEventW OReportEventW
#define ResetPrinterW OResetPrinterW
#define RpcBindingFromStringBindingW ORpcBindingFromStringBindingW
#define RpcBindingInqAuthClientW ORpcBindingInqAuthClientW
#define RpcBindingToStringBindingW ORpcBindingToStringBindingW
#define RpcEpRegisterNoReplaceW ORpcEpRegisterNoReplaceW
#define RpcMgmtEpEltInqNextW ORpcMgmtEpEltInqNextW
#define RpcMgmtInqServerPrincNameW ORpcMgmtInqServerPrincNameW
#define RpcNetworkInqProtseqsW ORpcNetworkInqProtseqsW
#define RpcNetworkIsProtseqValidW ORpcNetworkIsProtseqValidW
#define RpcNsBindingInqEntryNameW ORpcNsBindingInqEntryNameW
#define RpcProtseqVectorFreeW ORpcProtseqVectorFreeW
#define RpcServerInqDefaultPrincNameW ORpcServerInqDefaultPrincNameW
#define RpcServerUseProtseqEpW ORpcServerUseProtseqEpW
#define RpcServerUseProtseqIfW ORpcServerUseProtseqIfW
#define RpcServerUseProtseqW ORpcServerUseProtseqW
#define RpcStringBindingComposeW ORpcStringBindingComposeW
#define RpcStringBindingParseW ORpcStringBindingParseW
#define RpcStringFreeW ORpcStringFreeW
#define ScrollConsoleScreenBufferW OScrollConsoleScreenBufferW
#define SearchPathW OSearchPathW
#define SendMessageCallbackW OSendMessageCallbackW
#define SetComputerNameW OSetComputerNameW
#define SetConsoleTitleW OSetConsoleTitleW
#define SetDefaultCommConfigW OSetDefaultCommConfigW
#define SetFileSecurityW OSetFileSecurityW
#define SetFormW OSetFormW
#define SetICMProfileW OSetICMProfileW
#define SetJobW OSetJobW
#define SetPrinterDataW OSetPrinterDataW
#define SetPrinterW OSetPrinterW
#define SetServiceW OSetServiceW
#define SetUserObjectInformationW OSetUserObjectInformationW
#define SetVolumeLabelW OSetVolumeLabelW
#define SetWindowsHookW OSetWindowsHookW
#define ShellAboutW OShellAboutW
#define ShellExecuteW OShellExecuteW
#define SHFileOperationW OSHFileOperationW
#define SHGetNewLinkInfoW OSHGetNewLinkInfoW
#define sndPlaySoundW OsndPlaySoundW
#define StartDocPrinterW OStartDocPrinterW
#define TranslateURLW OTranslateURLW
#define UpdateICMRegKeyW OUpdateICMRegKeyW
#define URLAssociationDialogW OURLAssociationDialogW
#define UuidFromStringW OUuidFromStringW
#define VerFindFileW OVerFindFileW
#define VerInstallFileW OVerInstallFileW
#define VkKeyScanExW OVkKeyScanExW
#define WaitNamedPipeW OWaitNamedPipeW
#define waveInGetDevCapsW OwaveInGetDevCapsW
#define waveInGetErrorTextW OwaveInGetErrorTextW
#define waveOutGetDevCapsW OwaveOutGetDevCapsW
#define waveOutGetErrorTextW OwaveOutGetErrorTextW
#define wglUseFontBitmapsW OwglUseFontBitmapsW
#define wglUseFontOutlinesW OwglUseFontOutlinesW
#define WinExecErrorW OWinExecErrorW
#define WNetAddConnection2W OWNetAddConnection2W
#define WNetAddConnection3W OWNetAddConnection3W
#define WNetAddConnectionW OWNetAddConnectionW
#define WNetCancelConnection2W OWNetCancelConnection2W
#define WNetCancelConnectionW OWNetCancelConnectionW
#define WNetConnectionDialog1W OWNetConnectionDialog1W
#define WNetDisconnectDialog1W OWNetDisconnectDialog1W
#define WNetEnumResourceW OWNetEnumResourceW
#define WNetGetLastErrorW OWNetGetLastErrorW
#define WNetGetNetworkInformationW OWNetGetNetworkInformationW
#define WNetGetProviderNameW OWNetGetProviderNameW
#define WNetGetUniversalNameW OWNetGetUniversalNameW
#define WNetGetUserW OWNetGetUserW
#define WNetOpenEnumW OWNetOpenEnumW
#define WNetSetConnectionW OWNetSetConnectionW
#define WNetUseConnectionW OWNetUseConnectionW
#define WriteConsoleInputW OWriteConsoleInputW
#define WriteConsoleOutputCharacterW OWriteConsoleOutputCharacterW
#define WriteConsoleOutputW OWriteConsoleOutputW
#define WriteConsoleW OWriteConsoleW
#define WritePrivateProfileSectionW OWritePrivateProfileSectionW
#define WritePrivateProfileStructW OWritePrivateProfileStructW
#define WriteProfileSectionW OWriteProfileSectionW
#define WriteProfileStringW OWriteProfileStringW

#endif // DEBUG

#endif // MAPWAPIS



//----------------------------------------------------------------------------
// Wrappers
//----------------------------------------------------------------------------
extern "C" {


BOOL
WINAPI
OAppendMenuW(
  HMENU hMenu,
  UINT uFlags,
  UINT uIDnewItem,
  LPCWSTR lpnewItem
  );



LRESULT
WINAPI
OCallWindowProcW(
  WNDPROC lpPrevWndFunc,
  HWND hWnd,
  UINT Msg,
  WPARAM wParam,
  LPARAM lParam);



DWORD
WINAPI
OCharLowerBuffW(
  LPWSTR lpsz,
  DWORD cchLength);



LPWSTR
WINAPI
OCharLowerW (LPWSTR lpsz);



LPWSTR
WINAPI
OCharPrevW(
  LPCWSTR lpszStart,
  LPCWSTR lpszCurrent);


LPWSTR
WINAPI
OCharNextW (LPCWSTR lpsz);



BOOL
WINAPI
OCharToOemW(
  LPCWSTR lpszSrc,
  LPSTR lpszDst);


DWORD
WINAPI
OCharUpperBuffW(
  LPWSTR lpsz,
  DWORD cchLength);


LPWSTR
WINAPI
OCharUpperW(
  LPWSTR lpsz);

// From: Sridhar Chandrashekar on 7/21/98
//       Ted Smith: Returned result,
//                  Only convert lpTemplateName as nessacary
//                  Added paranioa re. CC_ENABLEHOOK

BOOL
WINAPI
OChooseColorW (LPCHOOSECOLORW lpcc);




BOOL
APIENTRY OChooseFontW(LPCHOOSEFONTW pchfw);

//  From EricKn 3/18/99
// CONSIDER: String should be converted w/ codepage that matches locale?

int
WINAPI
OCompareStringW(
  LCID     Locale,
  DWORD    dwCmpFlags,
  LPCWSTR lpString1,
  int      cchCount1,
  LPCWSTR lpString2,
  int      cchCount2);

// from johnchen 6/29/99
//UNDONE
// : Both of the CopyAcceleratorTable and CreateAcceleratorTable 
// : wrappers assume that the chars in the accelerator table are invariant 
// : between ANSI and Unicode. This is probably ok for VS tables, but it's 
// : not a complete solution if we deal with user-created tables. 

int
WINAPI
OCopyAcceleratorTableW(
  HACCEL hAccelSrc,
  LPACCEL lpAccelDst,
  int cAccelEntries);

// From: Mark Ashton on 5/8/97

BOOL
WINAPI
OCopyFileW(
  LPCWSTR lpExistingFileName,
  LPCWSTR lpNewFileName,
  BOOL bFailIfExists
  );

//  From EricKn 3/18/99

HMETAFILE 
WINAPI 
OCopyMetaFileW(
    HMETAFILE hmfSrc, 
    LPCWSTR lpszFile
    );

// from johnchen 7/19/99 
//UNDONE
// : Both of the CopyAcceleratorTable and CreateAcceleratorTable 
// : wrappers assume that the chars in the accelerator table are invariant 
// : between ANSI and Unicode. This is probably ok for VS tables, but it's 
// : not a complete solution if we deal with user-created tables. 
//

HACCEL
WINAPI
OCreateAcceleratorTableW(
  LPACCEL lpAccel, int cEntries);




HWND
WINAPI
OCreateDialogIndirectParamW(
  HINSTANCE hInstance,
  LPCDLGTEMPLATEW lpTemplate,
  HWND hWndParent,
  DLGPROC lpDialogFunc,
  LPARAM dwInitParam);



HWND
WINAPI
OCreateDialogParamW(
  HINSTANCE hInstance,
  LPCWSTR lpTemplateName,
  HWND hWndParent ,
  DLGPROC lpDialogFunc,
  LPARAM dwInitParam);


HDC
WINAPI
OCreateDCW(
  LPCWSTR lpszDriver,
  LPCWSTR lpszDevice,
  LPCWSTR lpszOutput,
  CONST DEVMODEW *lpInitData);

// From: Mark Ashton on 5/8/97

BOOL
WINAPI
OCreateDirectoryW(
  LPCWSTR lpPathName,
  LPSECURITY_ATTRIBUTES lpSecurityAttributes
  );

// From: Mark Ashton on 5/8/97
//       Ted Smith: simpified on 6/25
// Smoke tested by Mark Ashton on 6/25

BOOL
WINAPI
OCreateDirectoryExW(
  LPCWSTR lpTemplateDirectory,
  LPCWSTR lpNewDirectory,
  LPSECURITY_ATTRIBUTES lpSecurityAttributes
  );


HDC
WINAPI
OCreateEnhMetaFileW(
  HDC hdc,
  LPCWSTR lpFileName,
  CONST RECT *lpRect,
  LPCWSTR lpDescription);



HANDLE
WINAPI
OCreateEventW(
  LPSECURITY_ATTRIBUTES lpEventAttributes,
  BOOL bManualReset,
  BOOL bInitialState,
  LPCWSTR lpName
  );


HANDLE
WINAPI
OCreateFileW(
  LPCWSTR lpFileName,
  DWORD dwDesiredAccess,
  DWORD dwShareMode,
  LPSECURITY_ATTRIBUTES lpSecurityAttributes,
  DWORD dwCreationDisposition,
  DWORD dwFlagsAndAttributes,
  HANDLE hTemplateFile
  );


HFONT
WINAPI
OCreateFontIndirectW(CONST LOGFONTW * plfw);

// From: Mark Ashton on 5/29/97

HFONT
OCreateFontW(
  int nHeight, // logical height of font
  int nWidth, // logical average character width
  int nEscapement, // angle of escapement
  int nOrientation, // base-line orientation angle
  int fnWeight, // font weight
  DWORD fdwItalic, // italic attribute flag
  DWORD fdwUnderline, // underline attribute flag
  DWORD fdwStrikeOut, // strikeout attribute flag
  DWORD fdwCharSet, // character set identifier
  DWORD fdwOutputPrecision, // output precision
  DWORD fdwClipPrecision, // clipping precision
  DWORD fdwQuality, // output quality
  DWORD fdwPitchAndFamily, // pitch and family
  LPCWSTR lpszFace); // pointer to typeface name string


HDC
WINAPI
OCreateICW(
  LPCWSTR lpszDriver,
  LPCWSTR lpszDevice,
  LPCWSTR lpszOutput,
  CONST DEVMODEW *lpdvmInit);



HWND
WINAPI
OCreateMDIWindowW(
  LPWSTR lpClassName,
  LPWSTR lpWindowName,
  DWORD dwStyle,
  int X,
  int Y,
  int nWidth,
  int nHeight,
  HWND hWndParent,
  HINSTANCE hInstance,
  LPARAM lParam
  );



HDC     WINAPI
OCreateMetaFileW(LPCWSTR lpstr);



BOOL
WINAPI
OCreateProcessW(
  LPCWSTR lpApplicationName,
  LPWSTR lpCommandLine,
  LPSECURITY_ATTRIBUTES lpProcessAttributes,
  LPSECURITY_ATTRIBUTES lpThreadAttributes,
  BOOL bInheritHandles,
  DWORD dwCreationFlags,
  LPVOID lpEnvironment,
  LPCWSTR lpCurrentDirectory,
  LPSTARTUPINFOW lpStartupInfo,
  LPPROCESS_INFORMATION lpProcessInformation
  );


HPROPSHEETPAGE
WINAPI
OCreatePropertySheetPageW(
  LPCPROPSHEETPAGEW lpcpsp
  );



HANDLE
WINAPI
OCreateSemaphoreW(
  LPSECURITY_ATTRIBUTES lpSemaphoreAttributes,
  LONG lInitialCount,
  LONG lMaximumCount,
  LPCWSTR lpName
  );


HWND  
WINAPI
OCreateWindowExW( DWORD dwExStyle,
        LPCWSTR lpClassName,
        LPCWSTR lpWindowName,
        DWORD dwStyle,
        int X,
        int Y,
        int nWidth,
        int nHeight,
        HWND hWndParent ,
        HMENU hMenu,
        HINSTANCE hInstance,
        LPVOID lpParam );


HSZ
WINAPI
ODdeCreateStringHandleW(
  DWORD idInst,
  LPCWSTR psz,
  int iCodePage);


UINT
WINAPI
ODdeInitializeW(
  LPDWORD pidInst,
  PFNCALLBACK pfnCallback,
  DWORD afCmd,
  DWORD ulRes);


LRESULT
WINAPI
ODefFrameProcW(
  HWND hWnd,
  HWND hWndMDIClient ,
  UINT uMsg,
  WPARAM wParam,
  LPARAM lParam);


LRESULT
WINAPI
ODefMDIChildProcW(
  HWND hWnd,
  UINT uMsg,
  WPARAM wParam,
  LPARAM lParam);


LRESULT
WINAPI
ODefWindowProcW(
  HWND hWnd,
  UINT Msg,
  WPARAM wParam,
  LPARAM lParam);



BOOL
WINAPI
ODeleteFileW(
  LPCWSTR pwsz);


int
WINAPI
ODialogBoxIndirectParamW(
  HINSTANCE hInstance,
  LPCDLGTEMPLATEW hDialogTemplate,
  HWND hWndParent ,
  DLGPROC lpDialogFunc,
  LPARAM dwInitParam);


int
WINAPI
ODialogBoxParamW(
  HINSTANCE hInstance,
  LPCWSTR lpTemplateName,
  HWND hWndParent ,
  DLGPROC lpDialogFunc,
  LPARAM dwInitParam);


LONG
WINAPI
ODispatchMessageW(
  CONST MSG *lpMsg);


DWORD
WINAPI
ODocumentPropertiesW(
  HWND      hWnd,
  HANDLE    hPrinter,
  LPWSTR   pDeviceName,
  PDEVMODEW pDevModeOutput,
  PDEVMODEW pDevModeInput,
  DWORD     fMode
  );

// From: Mark Ashton on 5/29/97

UINT APIENTRY ODragQueryFileW(HDROP hDrop, UINT iFile, LPWSTR lpszFile, UINT cch);


int
WINAPI
ODrawTextW(
  HDC hDC,
  LPCWSTR lpString,
  int nCount,
  LPRECT lpRect,
  UINT uFormat);

// Written by Bill Hiebert on 9/4/97
// Smoke tested by Bill Hiebert 9/4/97

int
WINAPI
ODrawTextExW(HDC hdc, LPWSTR pwsz, int cb, LPRECT lprect, UINT dwDTFormat, LPDRAWTEXTPARAMS lpDTParams);


BOOL
WINAPI
OEnableWindow(
  HWND hWnd,
  BOOL bEnable);


// Written for Carlos Gomes on 6/26/97 by Ted Smith
// Smoke tested by Carlos Gomes on 6/26

DWORD
WINAPI
OExpandEnvironmentStringsW(
  LPCWSTR lpSrc,
  LPWSTR lpDst,
  DWORD nSize
  );

HICON 
APIENTRY 
OExtractAssociatedIconW(
    HINSTANCE hInst, 
    LPWSTR lpIconPath, 
    LPWORD lpiIcon
    );

HICON 
APIENTRY 
OExtractIconW(
    HINSTANCE hInst, 
    LPCWSTR lpszExeFileName, 
    UINT nIconIndex
    );

UINT 
WINAPI 
OExtractIconExW(
    LPCWSTR lpszFile, 
    int nIconIndex, 
    HICON FAR *phiconLarge, 
    HICON FAR *phiconSmall, 
    UINT nIcons
    );

BOOL  APIENTRY OExtTextOutW(
    HDC             hdc, 
    int             X, 
    int             Y, 
    UINT            fuOptions, 
    CONST RECT *    lprc, 
    LPCWSTR         lpString, 
    UINT            cch, 
    CONST INT *     lpDx
    );


VOID
WINAPI
OFatalAppExitW(
  UINT uAction,
  LPCWSTR lpMessageText
  );

HINSTANCE
APIENTRY
OFindExecutableW(
  LPCWSTR lpFile,
  LPCWSTR lpDirectory,
  LPWSTR lpResult
  );

// From: Mark Ashton on 5/8/97

HANDLE
WINAPI
OFindFirstChangeNotificationW(
  LPCWSTR lpPathName,
  BOOL bWatchSubtree,
  DWORD dwNotifyFilter
  );

// From: Mark Ashton on 5/8/97

HANDLE
WINAPI
OFindFirstFileW(
  LPCWSTR lpFileName,
  LPWIN32_FIND_DATAW lpFindFileData
  );

// From: Mark Ashton on 5/8/97

BOOL
WINAPI
OFindNextFileW(
  HANDLE hFindFile,
  LPWIN32_FIND_DATAW lpFindFileData
  );

// This is wrapped even though FindResourceW is implemented on Win95,
// because Win95 has a bug that corrupts the heap.

HRSRC
WINAPI
OFindResourceW(
  HINSTANCE hModule,
  LPCWSTR lpName,
  LPCWSTR lpType
  );


HWND
WINAPI
OFindWindowW(
  LPCWSTR lpClassName ,
  LPCWSTR lpWindowName);

// Bill Hiebert of IStudio on 6/13/97 added support for the
//   FORMAT_MESSAGE_ALLOCATE_BUFFER flag
// Bill donated a bugfix for 1819 on 8/1/97

DWORD
WINAPI
OFormatMessageW(
  DWORD dwFlags,
  LPCVOID lpSource,
  DWORD dwMessageId,
  DWORD dwLanguageId,
  LPWSTR lpBuffer,
  DWORD nSize,
  va_list *Arguments);

#ifdef _DEBUG
// These dumb wrapper implementations assume that the code points
// are the same in A/W
void VERIFYGETCHARWIDTHARGS (UINT uFirstChar, UINT uLastChar);
#else
#define VERIFYGETCHARWIDTHARGS(uFirstChar, uLastChar) (0);
#endif

 BOOL  APIENTRY
OGetCharABCWidthsFloatW(
  HDC     hdc,
  UINT    uFirstChar,
  UINT    uLastChar,
  LPABCFLOAT      lpABC);

 BOOL  APIENTRY
OGetCharABCWidthsW(
  HDC hdc,
  UINT uFirstChar,
  UINT uLastChar,
  LPABC lpABC);

 BOOL  APIENTRY
OGetCharWidthFloatW(
  HDC     hdc,
  UINT    uFirstChar,
  UINT    uLastChar,
  PFLOAT  pBuffer);


 BOOL  WINAPI
OGetCharWidthW(
  HDC hdc,
  UINT uFirstChar,
  UINT uLastChar,
  LPINT lpBuffer);

// Static buffers for GetClassInfo[Ex] to return the classname
// and menuname in Unicode, when running on an Ansi system.
// The contract of GetClassInfo is that it returns const ptrs
// back to the class name and menu name.  Unfortuntely, this
// prevents us from translating these back from Ansi to Unicode,
// without having some static buffers to use.  Since we strongly
// believe that the only people calling this are doing it just to
// see if it succeeds or not, so they know whether the class is
// already registered, we've willing to just have one set of
// static buffers to use.
// CAUTION: this will work as long as two threads don't call
// GetClassInfo[Ex] at the same time!
static WCHAR g_szClassName[256];
static WCHAR g_szMenuName[256];

#ifdef _DEBUG
static DWORD g_dwCallingThread = 0;    // debug global for ensuring one thread.
#endif // _DEBUG


BOOL
WINAPI
OGetClassInfoW
(
HINSTANCE hInstance,
LPCWSTR lpClassName,
LPWNDCLASSW lpWndClass
);


BOOL
WINAPI
OGetClassInfoExW
(
HINSTANCE hInstance,
LPCWSTR lpClassName,
LPWNDCLASSEXW lpWndClass
);


DWORD
WINAPI
OGetClassLongW(
  HWND hWnd,
  int nIndex);



DWORD
WINAPI
OSetClassLongW(
  HWND hWnd,
  int nIndex,
  LONG dwNewLong);


int
WINAPI
OGetClassNameW(
  HWND hWnd,
  LPWSTR lpClassName,
  int nMaxCount);


DWORD WINAPI
OGetCurrentDirectoryW(
  DWORD nBufferLength,
  LPWSTR lpBuffer);


int
WINAPI
OGetCurrencyFormatW(
  LCID     Locale,
  DWORD    dwFlags,
  LPCWSTR lpValue,
  CONST CURRENCYFMTW *lpFormat,
  LPWSTR  lpCurrencyStr,
  int      cchCurrency);


int
WINAPI
OGetDateFormatW(
  LCID     Locale,
  DWORD    dwFlags,
  CONST SYSTEMTIME *lpDate,
  LPCWSTR lpFormat,
  LPWSTR  lpDateStr,
  int      cchDate);


UINT
WINAPI
OGetDlgItemTextW(
  HWND hDlg,
  int nIDDlgItem,
  LPWSTR lpString,
  int nMaxCount);


UINT
WINAPI
OGetDriveTypeW(
  LPCWSTR lpRootPathName
  );

// Added by BillHie 4/5/99


DWORD
WINAPI
OGetEnvironmentVariableW(
  LPCWSTR lpName,
  LPWSTR lpBuffer,
  DWORD nSize
  );


LPWSTR
WINAPI
OGetEnvironmentStringsW(VOID);


int
WINAPI
OGetNumberFormatW(
  LCID     Locale,
  DWORD    dwFlags,
  LPCWSTR lpValue,
  CONST NUMBERFMTW *lpFormat,
  LPWSTR  lpNumberStr,
  int      cchNumber);


BOOL
WINAPI
OFreeEnvironmentStringsW(
    LPWSTR lpwstr
    );


BOOL
WINAPI
OSetEnvironmentVariableW(
  LPCWSTR lpName,
  LPCWSTR lpValue
  );


DWORD
WINAPI
OGetFileAttributesW(
  LPCWSTR lpFileName
  );

DWORD
WINAPI
OGetFileAttributesExW(
  LPCWSTR lpFileName,
  GET_FILEEX_INFO_LEVELS fInfoLevelId,
  LPVOID lpFileInformation	
  );

BOOL
WINAPI
OGetFileVersionInfoW(
  LPWSTR lpszFile,
  DWORD dwHandle,
  DWORD cbBuf,
  LPVOID lpvData);


short
WINAPI
OGetFileTitleW
(
LPCWSTR pwszFile,
LPWSTR pwszOut,
WORD w
);


DWORD
WINAPI
OGetFileVersionInfoSizeW(
  LPWSTR lpszFile,
  LPDWORD lpdwHandle);


DWORD
WINAPI
OGetFullPathNameW(
  LPCWSTR lpFileName,
  DWORD nBufferLength,
  LPWSTR lpBuffer,
  LPWSTR *lpFilePart
  );


//BUGBUG: wide character code points generally aren't the same for W/A,
// but this code passes the same value to both APIS

DWORD
WINAPI
OGetGlyphOutlineW(
  HDC     hdc,
  UINT    uChar,
  UINT    uFormat,
  LPGLYPHMETRICS      lpgm,
  DWORD       cbBuffer,
  LPVOID      lpvBuffer,
  CONST MAT2 *    lpmat2);


DWORD
WINAPI
OGetKerningPairsW(
  HDC         hdc,
  DWORD       nNumPairs,
  LPKERNINGPAIR       lpkrnpair);


BOOL
WINAPI
OGetMessageW(
  LPMSG lpMsg,
  HWND hWnd ,
  UINT wMsgFilterMin,
  UINT wMsgFilterMax);

 DWORD
WINAPI
OGetModuleFileNameW(
  HINSTANCE hModule,
  LPWSTR pwszFilename,
  DWORD nSize
  );


HMODULE
WINAPI
OGetModuleHandleW(
  LPCWSTR lpModuleName
  );

 UINT APIENTRY
OGetOutlineTextMetricsW(
  HDC hdc,
  UINT cbData,
  LPOUTLINETEXTMETRICW lpOTM);


UINT
WINAPI
OGetPrivateProfileIntW(
  LPCWSTR lpAppName,
  LPCWSTR lpKeyName,
  INT nDefault,
  LPCWSTR lpFileName);



DWORD
WINAPI
OGetPrivateProfileStringW(
  LPCWSTR lpAppName,
  LPCWSTR lpKeyName,
  LPCWSTR lpDefault,
  LPWSTR lpReturnedString,
  DWORD nSize,
  LPCWSTR lpFileName);


 int   WINAPI
OGetObjectW(
  HGDIOBJ hgdiobj,
  int cbBuffer,
  LPVOID lpvObject);

// From: Mark Ashton on 5/8/97
//       Ted Smith: Re-wrote fixing handling of non-trivial parameters on 6/25
// Smoke tested by Mark Ashton on 6/25

APIENTRY
OGetOpenFileNameW
(
LPOPENFILENAMEW lpofn
);


UINT
WINAPI
OGetProfileIntW(
  LPCWSTR lpAppName,
  LPCWSTR lpKeyName,
  INT nDefault
  );


HANDLE
WINAPI
OGetPropW(
  HWND hWnd,
  LPCWSTR lpString);

// From: Mark Ashton on 5/29/97
//       Ted Smith: Re-wrote fixing handling of non-trivial parameters on 6/25
// Smoke tested by Mark Ashton on 6/25

APIENTRY
OGetSaveFileNameW(LPOPENFILENAMEW lpofn);

/* We had to add this wrapper because components in daVinci that use ATL link statically. When statically
  the ATL makes call to this api and we need to provide the wrapper for that call. 
*/

// UNDONE  OGetShortPathNameW needs a thorough review for ANSI/Unicode issues


DWORD
WINAPI
OGetShortPathNameW
(
LPCTSTR lpszLongPath,  // pointer to a null-terminated path string
LPTSTR lpszShortPath,  // pointer to a buffer to receive the null-terminated short form of the path
DWORD cchBuffer        // specifies the size of the buffer pointed  to by lpszShortPath
); 


BOOL
WINAPI
OGetStringTypeExW(
  LCID     Locale,
  DWORD    dwInfoType,
  LPCWSTR  lpSrcStr,
  int      cchSrc,
  LPWORD   lpCharType);


DWORD
WINAPI
OGetTabbedTextExtentW(
  HDC hDC,
  LPCWSTR lpString,
  int nCount,
  int nTabPositions,
  LPINT lpnTabStopPositions);

// From: Mark Ashton on 5/8/97

UINT
WINAPI
OGetTempFileNameW(
  LPCWSTR lpPathName,
  LPCWSTR lpPrefixString,
  UINT uUnique,
  LPWSTR lpTempFileName
  );

// From: Mark Ashton on 5/8/97

DWORD
WINAPI
OGetTempPathW(
  DWORD nBufferLength,
  LPWSTR lpBuffer
  );

//wrapper when this is implemented on Win95 because of Win95 crashing bugs

BOOL  APIENTRY
OGetTextExtentPoint32W(
          HDC hdc,
          LPCWSTR pwsz,
          int cb,
          LPSIZE pSize
          );


BOOL APIENTRY
OGetTextExtentPointW(
          HDC hdc,
          LPCWSTR pwsz,
          int cch,
          LPSIZE pSize
          );

// helper for OGetTextExtentExPointW
static BOOL SimulateGetTextExtentExPointW (
    HDC     hdc,
    LPCWSTR lpString,
    int     cchString,
    int     nMaxExtent,
    LPINT   lpnFit,
    LPINT   alpDx,
    LPSIZE  lpSize
    );


BOOL  APIENTRY OGetTextExtentExPointW(
    HDC     hdc,
    LPCWSTR lpString,
    int     cchString,
    int     nMaxExtent,
    LPINT   lpnFit,
    LPINT   alpDx,
    LPSIZE  lpSize
    );


LONG
WINAPI
OGetWindowLongW(
  HWND hWnd,
  int nIndex);


BOOL
WINAPI
OGetTextMetricsW(
  HDC hdc,
  LPTEXTMETRICW lptm);


int
WINAPI
OGetTimeFormatW(
  LCID     Locale,
  DWORD    dwFlags,
  CONST SYSTEMTIME *lpTime,
  LPCWSTR lpFormat,
  LPWSTR  lpTimeStr,
  int      cchTime);

// From: Mark Ashton on 5/8/97

BOOL
WINAPI
OGetUserNameW (
  LPWSTR lpBuffer,
  LPDWORD pnSize
  );


BOOL
WINAPI
OGetVolumeInformationW(
  LPCWSTR lpRootPathName,
  LPWSTR lpVolumeNameBuffer,
  DWORD nVolumeNameSize,
  LPDWORD lpVolumeSerialNumber,
  LPDWORD lpMaximumComponentLength,
  LPDWORD lpFileSystemFlags,
  LPWSTR lpFileSystemNameBuffer,
  DWORD nFileSystemNameSize
  );


int
WINAPI
OGetWindowTextLengthW(
  HWND hWnd);


int
WINAPI
OGetWindowTextW(
  HWND hWnd,
  LPWSTR lpString,
  int nMaxCount);


ATOM
WINAPI
OGlobalAddAtomW(
  LPCWSTR lpString
  );

ATOM
WINAPI
OGlobalFindAtomW(
  LPCWSTR lpString
  );

// From: Josh Kaplan on 8/12/97

UINT
WINAPI
OGlobalGetAtomNameW(
  ATOM nAtom,
  LPWSTR lpBuffer,
  int nSize
  );


BOOL
WINAPI
OGrayStringW(
  HDC hDC,
  HBRUSH hBrush,
  GRAYSTRINGPROC lpOutputFunc,
  LPARAM lpData,
  int nCount,
  int X,
  int Y,
  int nWidth,
  int nHeight);

// From: Mark Ashton on 5/8/97

HIMAGELIST
WINAPI
OImageList_LoadImageW(HINSTANCE hi, LPCWSTR lpbmp, int cx, int cGrow, COLORREF crMask, UINT uType, UINT uFlags);


LONG
WINAPI 
OImmGetCompositionStringW(HIMC, DWORD, LPVOID, DWORD);


BOOL
WINAPI
OInsertMenuW(
  HMENU hMenu,
  UINT uPosition,
  UINT uFlags,
  UINT uIDNewItem,
  LPCWSTR lpNewItem
  );


BOOL
WINAPI
OIsBadStringPtrW(
  LPCWSTR lpsz,
  UINT ucchMax
  );


BOOL
WINAPI
OIsCharAlphaNumericW(
  WCHAR wch);


BOOL
WINAPI
OIsCharAlphaW(
  WCHAR wch);


BOOL
WINAPI
OIsDialogMessageW(
  HWND hDlg,
  LPMSG lpMsg);

// From: Mark Ashton on 5/8/97
//     Bill Hieber - 2/5/98 fixed buffer size problem.
// CONSIDER: map only those characters that convert successfully (if not generating a sort key);$
// CONSIDER: use the codepage for locale in conversions?

int
WINAPI
OLCMapStringW(
  LCID     Locale,
  DWORD    dwMapFlags,
  LPCWSTR lpSrcStr,
  int      cchSrc,
  LPWSTR  lpDestStr,
  int      cchDest);

 HACCEL
WINAPI
OLoadAcceleratorsW(
  HINSTANCE hInst,
  LPCWSTR   lpTableName);


HBITMAP
WINAPI
OLoadBitmapW(
  HINSTANCE hInstance,
  LPCWSTR lpBitmapName);

 HCURSOR
WINAPI
OLoadCursorW(
  HINSTANCE hInstance,
  LPCWSTR lpCursorName);


HICON
WINAPI
OLoadIconW(
  HINSTANCE hInstance,
  LPCWSTR lpIconName);



HINSTANCE
WINAPI
OLoadLibraryW(
  LPCWSTR pwszFileName
  );


HMODULE
WINAPI
OLoadLibraryExW(
  LPCWSTR lpLibFileName,
  HANDLE hFile,
  DWORD dwFlags
  );


HMENU
WINAPI
OLoadMenuIndirectW(
  CONST MENUTEMPLATEW *lpMenuTemplate);


HMENU
WINAPI
OLoadMenuW(
  HINSTANCE hInstance,
  LPCWSTR lpMenuName);


int
WINAPI
OLoadStringW(
  HINSTANCE hInstance,
  UINT uID,
  LPWSTR lpBuffer,
  int nBufferMax);


LPWSTR
WINAPI
OlstrcatW(
  LPWSTR lpString1,
  LPCWSTR lpString2
  );



int
WINAPI
OlstrcmpiW(
  LPCWSTR lpString1,
  LPCWSTR lpString2
  );



int
WINAPI
OlstrcmpW(
  LPCWSTR lpString1,
  LPCWSTR lpString2
  );


LPWSTR
WINAPI
OlstrcpyW(
  LPWSTR lpString1,
  LPCWSTR lpString2
  );

// From: Mark Ashton on 5/8/97
//       Ted Smith added null string pointer handling
// [paulde] Just implement w/o using the API and save a little code

LPWSTR
WINAPI
OlstrcpynW(
  LPWSTR lpString1,
  LPCWSTR lpString2,
  int iMaxLength
  );


int
WINAPI
OlstrlenW(
  LPCWSTR lpString
  );


UINT
WINAPI
OMapVirtualKeyW(
  UINT uCode,
  UINT uMapType);


//BUGBUG: why the wrapper when it's implemented on Win95?

int
WINAPI
OMessageBoxW(
  HWND hWnd ,
  LPCWSTR lpText,
  LPCWSTR lpCaption,
  UINT uType);



int
WINAPI
OMessageBoxIndirectW(
  LPMSGBOXPARAMSW lpmbp);



BOOL
WINAPI
OModifyMenuW(
  HMENU hMnu,
  UINT uPosition,
  UINT uFlags,
  UINT uIDNewItem,
  LPCWSTR lpNewItem
  );


// From: Mark Ashton on 5/29/97

BOOL
WINAPI
OMoveFileExW(
  LPCWSTR lpExistingFileName,
  LPCWSTR lpNewFileName,
  DWORD dwFlags
  );



BOOL
WINAPI
OMoveFileW(
  LPCWSTR lpExistingFileName,
  LPCWSTR lpNewFileName);



HANDLE
WINAPI
OLoadImageW(
  HINSTANCE hinst,
  LPCWSTR lpszName,
  UINT uType,
  int cxDesired,
  int cyDesired,
  UINT fuLoad);



BOOL
WINAPI
OOemToCharW(
  LPCSTR lpszSrc,
  LPWSTR lpszDst);


// CONSIDER why is this here

BOOL
WINAPI
OOpenPrinterW(
  LPWSTR    pPrinterName,
  LPHANDLE phPrinter,
  LPPRINTER_DEFAULTSW pDefault
  );



VOID
WINAPI
OOutputDebugStringW(
  LPCWSTR lpOutputString
  );



BOOL
WINAPI
OPeekMessageW(
  LPMSG lpMsg,
  HWND hWnd ,
  UINT wMsgFilterMin,
  UINT wMsgFilterMax,
  UINT wRemoveMsg);


// Patch for converting embedded sheets from Brian Marshall 2/2/98
// TedSmith: Rewritten to survive either set of header for size of PROPSHEETPAGE

int
WINAPI
OPropertySheetW(
  LPCPROPSHEETHEADERW lpcpsh
  );




BOOL
WINAPI
OPostMessageW(
  HWND hWnd,
  UINT Msg,
  WPARAM wParam,
  LPARAM lParam);


BOOL
WINAPI
OPostThreadMessageW(
  DWORD idThread,
  UINT Msg,
  WPARAM wParam,
  LPARAM lParam);


// From: Mark Ashton on 5/8/97

LONG
APIENTRY
ORegCreateKeyExW(
  HKEY hKey,
  LPCWSTR lpSubKey,
  DWORD Reserved,
  LPWSTR lpClass,
  DWORD dwOptions,
  REGSAM samDesired,
  LPSECURITY_ATTRIBUTES lpSecurityAttributes,
  PHKEY phkResult,
  LPDWORD lpdwDisposition
  );

// From: Mark Ashton on 5/8/97

LONG
APIENTRY
ORegCreateKeyW (
  HKEY hKey,
  LPCWSTR lpSubKey,
  PHKEY phkResult
  );

// From: Mark Ashton on 5/8/97

LONG
APIENTRY
ORegEnumKeyW (
  HKEY hKey,
  DWORD dwIndex,
  LPWSTR lpName,
  DWORD cbName
  );

//  Van Kichline
//  IHammer group
//  Not supported: REG_MULTI_SZ
//

LONG
APIENTRY
ORegEnumValueW (
  HKEY hKey,
  DWORD dwIndex,
  LPWSTR lpValueName,
  LPDWORD lpcbValueName,  // Documentation indicates this is a count of characters, despite the Hungarian.
  LPDWORD lpReserved,
  LPDWORD lpType,         // May be NULL, but we need to know it on return if lpData is not NULL.
  LPBYTE lpData,          // May be NULL
  LPDWORD lpcbData        // May be NULL is lpData is NULL
  );

 LONG APIENTRY ORegOpenKeyW(HKEY hKey, LPCWSTR pwszSubKey, PHKEY phkResult);


LONG
APIENTRY
ORegDeleteKeyW(
  HKEY hKey,
  LPCWSTR pwszSubKey
  );


LONG
APIENTRY
ORegDeleteValueW(
  HKEY hKey,
  LPWSTR lpValueName
  );


ATOM
WINAPI
ORegisterClassW(
  CONST WNDCLASSW *lpWndClass);


ATOM
WINAPI
ORegisterClassExW (CONST WNDCLASSEXW * lpWndClass);



BOOL
WINAPI
OUnregisterClassW (
    LPCTSTR  lpClassName,   // address of class name string
    HINSTANCE  hInstance    // handle of application instance
    );


UINT
WINAPI
ORegisterClipboardFormatW (LPCWSTR lpszFormat);

 UINT WINAPI
ORegisterWindowMessageW (LPCWSTR lpString);

 LONG
APIENTRY
ORegOpenKeyExW (
  HKEY hKey,
  LPCTSTR lpSubKey,
  DWORD ulOptions,
  REGSAM samDesired,
  PHKEY phkResult
  );


LONG
APIENTRY
ORegQueryInfoKeyW (
  HKEY hKey,
  LPWSTR lpClass,
  LPDWORD lpcbClass,
  LPDWORD lpReserved,
  LPDWORD lpcSubKeys,
  LPDWORD lpcbMaxSubKeyLen,
  LPDWORD lpcbMaxClassLen,
  LPDWORD lpcValues,
  LPDWORD lpcbMaxValueNameLen,
  LPDWORD lpcbMaxValueLen,
  LPDWORD lpcbSecurityDescriptor,
  PFILETIME lpftLastWriteTime
  );

 LONG APIENTRY ORegQueryValueW (
    HKEY    hKey, 
    LPCWSTR pwszSubKey, 
    LPWSTR  pwszValue,
    PLONG   lpcbValue
    );


LONG
APIENTRY
ORegSetValueExW (
  HKEY            hKey,
  LPCWSTR         lpValueName,
  DWORD           Reserved,
  DWORD           dwType,
  CONST BYTE *    lpData,
  DWORD           cbData
  );

 LONG APIENTRY 
ORegSetValueW (
    HKEY    hKey, 
    LPCWSTR lpSubKey, 
    DWORD   dwType,
    LPCWSTR lpData, 
    DWORD   cbData
    );

 LONG
APIENTRY
ORegQueryValueExW (
  HKEY    hKey,
  LPWSTR  lpValueName,
  LPDWORD lpReserved,
  LPDWORD lpType,
  LPBYTE  lpData,
  LPDWORD lpcbData
  );



HANDLE
WINAPI
ORemovePropW(
  HWND hWnd,
  LPCWSTR lpString);

 HDC  WINAPI
OResetDCW(
  HDC hdc,
  CONST DEVMODEW *lpInitData);

// BUGBUG: OSendDlgItemMessageW converts lParam, OSendMessageW does not!!

LONG
WINAPI
OSendDlgItemMessageW(
  HWND hDlg,
  int nIDDlgItem,
  UINT Msg,
  WPARAM wParam,
  LPARAM lParam);

// BUGBUG: OSendDlgItemMessageW converts lParam, OSendMessageW does not!!

LRESULT
WINAPI
OSendMessageW(
  HWND hWnd,
  UINT Msg,
  WPARAM wParam,
  LPARAM lParam);


BOOL
WINAPI
OSendNotifyMessageW(
  HWND hWnd,
  UINT Msg,
  WPARAM wParam,
  LPARAM lParam);

//  From EricKn 3/18/99

BOOL
WINAPI
OSetCurrentDirectoryW(
  LPCWSTR lpPathName
  );


BOOL
WINAPI
OSetDlgItemTextW(
  HWND hDlg,
  int nIDDlgItem,
  LPCWSTR lpString);


BOOL
WINAPI
OSetFileAttributesW(
  LPCWSTR lpFileName,
  DWORD dwFileAttributes
  );


BOOL
WINAPI
OSetPropW(
  HWND hWnd,
  LPCWSTR lpString,
  HANDLE hData);


BOOL
WINAPI
OSetMenuItemInfoW(
  HMENU hMenu,
  UINT uItem,
  BOOL fByPosition,
  LPCMENUITEMINFOW lpcmii
  );



LONG
WINAPI
OSetWindowLongW(
  HWND hWnd,
  int nIndex,
  LONG dwNewLong);


HHOOK
WINAPI
OSetWindowsHookExW(
  int idHook,
  HOOKPROC lpfn,
  HINSTANCE hmod,
  DWORD dwThreadId);


BOOL
WINAPI
OSetWindowTextW(
  HWND hWnd,
  LPCWSTR lpString);

// From: Bill Hiebert of IStudio on 6/13/97
//       Ted Smith: somplified and returned results 6/13
//       Ted Smith: added LoadProcAddress ... on 6/16
// Smoke tested by Bill on 6/16
//       Ted Smith: Fixed handling of lpFile on 6/25
// Smoke tested by Mark Ashton on 6/25

typedef BOOL (WINAPI *PFShellExecuteExW)(LPSHELLEXECUTEINFOW);
static PFShellExecuteExW s_PFShellExecuteExW = 0;


BOOL
WINAPI
OShellExecuteExW(
  LPSHELLEXECUTEINFOW lpExecInfo);


LONG
WINAPI
OTabbedTextOutW(
  HDC hDC,
  int X,
  int Y,
  LPCWSTR lpString,
  int nCount,
  int nTabPositions,
  LPINT lpnTabStopPositions,
  int nTabOrigin);


int
WINAPI
OTranslateAcceleratorW(
  HWND hWnd,
  HACCEL hAccTable,
  LPMSG lpMsg);



DWORD
APIENTRY
OVerLanguageNameW(
    DWORD wLang,
    LPWSTR szLang,
    DWORD nSize
    );


BOOL
WINAPI
OVerQueryValueW(
  const LPVOID pBlock,
  LPWSTR lpSubBlock,
  LPVOID *lplpBuffer,
  PUINT puLerr);


SHORT
WINAPI
OVkKeyScanW(
  WCHAR ch);


BOOL
WINAPI
OWinHelpW(
  HWND hWndMain,
  LPCWSTR lpszHelp,
  UINT uCommand,
  DWORD dwData
  );


DWORD APIENTRY
OWNetGetConnectionW (
    LPCWSTR lpLocalName,
    LPWSTR  lpRemoteName,
    LPDWORD  lpnLength
    );



BOOL
WINAPI
OWritePrivateProfileStringW(
  LPCWSTR lpAppName,
  LPCWSTR lpKeyName,
  LPCWSTR lpString,
  LPCWSTR lpFileName);

 int WINAPIV
OwsprintfW (
    LPWSTR pwszOut, 
    LPCWSTR pwszFormat, 
    ...);



BOOL
WINAPI
OGetVersionExW(
  LPOSVERSIONINFOW lpVersionInformation
  );



LONG
APIENTRY
ORegEnumKeyExW (
  HKEY hKey,
  DWORD dwIndex,
  LPWSTR lpName,
  LPDWORD lpcbName,
  LPDWORD lpReserved,
  LPWSTR lpClass,
  LPDWORD lpcbClass,
  PFILETIME lpftLastWriteTime
  );



HANDLE
WINAPI
OCreateFileMappingW(
  HANDLE hFile,
  LPSECURITY_ATTRIBUTES lpFileMappingAttributes,
  DWORD flProtect,
  DWORD dwMaximumSizeHigh,
  DWORD dwMaximumSizeLow,
  LPCWSTR lpName
  );



LRESULT
WINAPI
ODefDlgProcW(
  HWND hDlg,
  UINT Msg,
  WPARAM wParam,
  LPARAM lParam);



int
WINAPI
OGetLocaleInfoW(
  LCID     Locale,
  LCTYPE   LCType,
  LPWSTR  lpLCData,
  int      cchData);


BOOL
WINAPI
OSetLocaleInfoW(
  LCID     Locale,
  LCTYPE   LCType,
  LPCWSTR lpLCData);

//-----------------------------------------------------------------------------
// Name: OStgCreateDocfile
//
// Description:
// Wrapper for StgCreateDocfile to protect against reentrancy bug in OLE.
//
// Thread-Safety: Bullet-proof
//
// Return Values: same HRESULT as StgCreateDocfile
//-----------------------------------------------------------------------------


HRESULT
WINAPI
OStgCreateDocfile
(
const WCHAR * pwcsName, 
DWORD grfMode,
DWORD reserved,
IStorage ** ppstgOpen
);



APIENTRY
OPrintDlgW
(
LPPRINTDLGW lppd
);




int
WINAPI
OStartDocW
(
HDC hDC,
CONST DOCINFOW * pdiDocW
);


LPITEMIDLIST
WINAPI
OSHBrowseForFolderW(
  LPBROWSEINFO lpbi);


BOOL
WINAPI
OSHGetPathFromIDListW(
  LPCITEMIDLIST pidl,
  LPTSTR pszPath);


BOOL
WINAPI
OSystemParametersInfoW(
  UINT uiAction,
  UINT uiParam,
  PVOID pvParam,
  UINT fWinIni);


BOOL WINAPI OShell_NotifyIconW(DWORD dwMessage, PNOTIFYICONDATAW lpData);



int
WINAPI
OGetTextFaceW(
    HDC    hdc,
    int    cwch,
    LPWSTR lpFaceName);

//  From VanK 2/23/99

DWORD 
WINAPI 
OSHGetFileInfoW(LPCWSTR pszPath, DWORD dwFileAttributes, SHFILEINFOW FAR *psfi, UINT cbFileInfo, UINT uFlags);



int
WINAPI
OwvsprintfW(
  LPWSTR pwszOut,
  LPCWSTR pwszFormat,
  va_list arglist);

//================================================================
//================================================================
//=====================  Unwrapped APIs  =========================
//================================================================
//================================================================


#ifdef _DEBUG


BOOL
APIENTRY
OAbortSystemShutdownW(
  LPWSTR lpMachineName
  );


BOOL
WINAPI
OAccessCheckAndAuditAlarmW (
  LPCWSTR SubsystemName,
  LPVOID HandleId,
  LPWSTR ObjectTypeName,
  LPWSTR ObjectName,
  PSECURITY_DESCRIPTOR SecurityDescriptor,
  DWORD DesiredAccess,
  PGENERIC_MAPPING GenericMapping,
  BOOL ObjectCreation,
  LPDWORD GrantedAccess,
  LPBOOL AccessStatus,
  LPBOOL pfGenerateOnClose
  );


int WINAPI OAddFontResourceW(LPCWSTR);


BOOL
WINAPI
OAddFormW(
  HANDLE  hPrinter,
  DWORD   Level,
  LPBYTE  pForm
);


BOOL
WINAPI
OAddJobW(
  HANDLE  hPrinter,
  DWORD   Level,
  LPBYTE  pData,
  DWORD   cbBuf,
  LPDWORD pcbNeeded
);


BOOL
WINAPI
OAddMonitorW(
  LPWSTR   pName,
  DWORD   Level,
  LPBYTE  pMonitors
);


BOOL
WINAPI
OAddPortW(
  LPWSTR   pName,
  HWND    hWnd,
  LPWSTR   pMonitorName
);


HANDLE
WINAPI
OAddPrinterW(
  LPWSTR   pName,
  DWORD   Level,
  LPBYTE  pPrinter
);


BOOL
WINAPI
OAddPrinterConnectionW(
  LPWSTR   pName
);


BOOL
WINAPI
OAddPrinterDriverW(
  LPWSTR   pName,
  DWORD   Level,
  LPBYTE  pDriverInfo
);


BOOL
WINAPI
OAddPrintProcessorW(
  LPWSTR   pName,
  LPWSTR   pEnvironment,
  LPWSTR   pPathName,
  LPWSTR   pPrintProcessorName
);


BOOL
WINAPI
OAddPrintProvidorW(
  LPWSTR  pName,
  DWORD    level,
  LPBYTE   pProvidorInfo
);


LONG
WINAPI
OAdvancedDocumentPropertiesW(
  HWND    hWnd,
  HANDLE  hPrinter,
  LPWSTR   pDeviceName,
  PDEVMODEW pDevModeOutput,
  PDEVMODEW pDevModeInput
);


MMRESULT WINAPI OauxGetDevCapsW(UINT uDeviceID, LPAUXCAPSW pac, UINT cbac);


WINAPI OAVIBuildFilterW(LPWSTR lpszFilter, LONG cbFilter, BOOL fSaving);


WINAPI OAVIFileCreateStreamW (PAVIFILE pfile, PAVISTREAM FAR *ppavi, AVISTREAMINFOW FAR * psi);


WINAPI OAVIFileInfoW (PAVIFILE pfile, LPAVIFILEINFOW pfi, LONG lSize);


HRESULT CDECL OAVISaveW (LPCWSTR               szFile,
    CLSID FAR *pclsidHandler,
    AVISAVECALLBACK     lpfnCallback,
    int                 nStreams,
    PAVISTREAM      pfile,
    LPAVICOMPRESSOPTIONS lpOptions,
    ...);


WINAPI OAVIStreamInfoW (PAVISTREAM pavi, LPAVISTREAMINFOW psi, LONG lSize);


WINAPI OAVIStreamOpenFromFileW(PAVISTREAM FAR *ppavi, LPCWSTR szFile,
         DWORD fccType, LONG lParam,
         UINT mode, CLSID FAR *pclsidHandler);


BOOL
WINAPI
OBackupEventLogW (
  HANDLE hEventLog,
  LPCWSTR lpBackupFileName
  );


HANDLE
WINAPI
OBeginUpdateResourceW(
  LPCWSTR pFileName,
  BOOL bDeleteExistingResources
  );


BOOL
WINAPI
OBuildCommDCBW(
  LPCWSTR lpDef,
  LPDCB lpDCB
  );


BOOL
WINAPI
OBuildCommDCBAndTimeoutsW(
  LPCWSTR lpDef,
  LPDCB lpDCB,
  LPCOMMTIMEOUTS lpCommTimeouts
  );


BOOL
WINAPI
OCallMsgFilterW(
  LPMSG lpMsg,
  int nCode);


BOOL
WINAPI
OCallNamedPipeW(
  LPCWSTR lpNamedPipeName,
  LPVOID lpInBuffer,
  DWORD nInBufferSize,
  LPVOID lpOutBuffer,
  DWORD nOutBufferSize,
  LPDWORD lpBytesRead,
  DWORD nTimeOut
  );


VFWAPI OcapCreateCaptureWindowW (
    LPCWSTR lpszWindowName,
    DWORD dwStyle,
    int x, int y, int nWidth, int nHeight,
    HWND hwndParent, int nID);


LONG
WINAPI
OChangeDisplaySettingsW(
  LPDEVMODEW lpDevMode,
  DWORD dwFlags);


BOOL
WINAPI
OChangeMenuW(
  HMENU hMenu,
  UINT cmd,
  LPCWSTR lpszNewItem,
  UINT cmdInsert,
  UINT flags);

#if 0 //UNDONE_POST_98 - We should wrap these as being NT only...

BOOL
WINAPI
OChangeServiceConfigW(
  SC_HANDLE    hService,
  DWORD        dwServiceType,
  DWORD        dwStartType,
  DWORD        dwErrorControl,
  LPCWSTR     lpBinaryPathName,
  LPCWSTR     lpLoadOrderGroup,
  LPDWORD      lpdwTagId,
  LPCWSTR     lpDependencies,
  LPCWSTR     lpServiceStartName,
  LPCWSTR     lpPassword,
  LPCWSTR     lpDisplayName
  );
#endif


BOOL
WINAPI
OCharToOemBuffW(
  LPCWSTR lpszSrc,
  LPSTR lpszDst,
  DWORD cchDstLength);


BOOL
WINAPI
OClearEventLogW (
  HANDLE hEventLog,
  LPCWSTR lpBackupFileName
  );


BOOL
WINAPI
OCommConfigDialogW(
  LPCWSTR lpszName,
  HWND hWnd,
  LPCOMMCONFIG lpCC
  );


BOOL
WINAPI
OConfigurePortW(
  LPWSTR   pName,
  HWND    hWnd,
  LPWSTR   pPortName
);



HENHMETAFILE WINAPI OCopyEnhMetaFileW(HENHMETAFILE, LPCWSTR);


WINAPI OCreateColorSpaceW(LPLOGCOLORSPACEW);


HDESK
WINAPI
OCreateDesktopW(
  LPWSTR lpszDesktop,
  LPWSTR lpszDevice,
  LPDEVMODEW pDevmode,
  DWORD dwFlags,
  DWORD dwDesiredAccess,
  LPSECURITY_ATTRIBUTES lpsa);


HANDLE
WINAPI
OCreateMailslotW(
  LPCWSTR lpName,
  DWORD nMaxMessageSize,
  DWORD lReadTimeout,
  LPSECURITY_ATTRIBUTES lpSecurityAttributes
  );


HANDLE
WINAPI
OCreateMutexW(
  LPSECURITY_ATTRIBUTES lpMutexAttributes,
  BOOL bInitialOwner,
  LPCWSTR lpName
  );


HANDLE
WINAPI
OCreateNamedPipeW(
  LPCWSTR lpName,
  DWORD dwOpenMode,
  DWORD dwPipeMode,
  DWORD nMaxInstances,
  DWORD nOutBufferSize,
  DWORD nInBufferSize,
  DWORD nDefaultTimeOut,
  LPSECURITY_ATTRIBUTES lpSecurityAttributes
  );


BOOL
WINAPI
OCreateProcessAsUserW (
  HANDLE hToken,
  LPCWSTR lpApplicationName,
  LPWSTR lpCommandLine,
  LPSECURITY_ATTRIBUTES lpProcessAttributes,
  LPSECURITY_ATTRIBUTES lpThreadAttributes,
  BOOL bInheritHandles,
  DWORD dwCreationFlags,
  LPVOID lpEnvironment,
  LPCWSTR lpCurrentDirectory,
  LPSTARTUPINFOW lpStartupInfo,
  LPPROCESS_INFORMATION lpProcessInformation
  );


BOOL    WINAPI OCreateScalableFontResourceW(DWORD, LPCWSTR, LPCWSTR, LPCWSTR);

#if 0 //UNDONE_POST_98 - We should wrap these as being NT only...

SC_HANDLE
WINAPI
OCreateServiceW(
  SC_HANDLE    hSCManager,
  LPCWSTR     lpServiceName,
  LPCWSTR     lpDisplayName,
  DWORD        dwDesiredAccess,
  DWORD        dwServiceType,
  DWORD        dwStartType,
  DWORD        dwErrorControl,
  LPCWSTR     lpBinaryPathName,
  LPCWSTR     lpLoadOrderGroup,
  LPDWORD      lpdwTagId,
  LPCWSTR     lpDependencies,
  LPCWSTR     lpServiceStartName,
  LPCWSTR     lpPassword
  );
#endif


HWND WINAPI OCreateStatusWindowW(LONG style, LPCWSTR lpszText, HWND hwndParent, UINT wID);


HWINSTA
WINAPI
OCreateWindowStationW(
  LPWSTR lpwinsta,
  DWORD dwReserved,
  DWORD dwDesiredAccess,
  LPSECURITY_ATTRIBUTES lpsa);


RPC_STATUS RPC_ENTRY
ODceErrorInqTextW (
  IN RPC_STATUS RpcStatus,
  OUT unsigned short __RPC_FAR * ErrorText
  );


BOOL
WINAPI
ODefineDosDeviceW(
  DWORD dwFlags,
  LPCWSTR lpDeviceName,
  LPCWSTR lpTargetPath
  );


BOOL
WINAPI
ODeleteFormW(
  HANDLE  hPrinter,
  LPWSTR   pFormName
);


BOOL
WINAPI
ODeleteMonitorW(
  LPWSTR   pName,
  LPWSTR   pEnvironment,
  LPWSTR   pMonitorName
);


BOOL
WINAPI
ODeletePortW(
  LPWSTR   pName,
  HWND    hWnd,
  LPWSTR   pPortName
);


BOOL
WINAPI
ODeletePrinterConnectionW(
  LPWSTR   pName
);


BOOL
WINAPI
ODeletePrinterDriverW(
   LPWSTR    pName,
   LPWSTR    pEnvironment,
   LPWSTR    pDriverName
);


BOOL
WINAPI
ODeletePrintProcessorW(
  LPWSTR   pName,
  LPWSTR   pEnvironment,
  LPWSTR   pPrintProcessorName
);


BOOL
WINAPI
ODeletePrintProvidorW(
  LPWSTR   pName,
  LPWSTR   pEnvironment,
  LPWSTR   pPrintProvidorName
);


int  WINAPI ODeviceCapabilitiesW(LPCWSTR, LPCWSTR, WORD,
                LPWSTR, CONST DEVMODEW *);


int
WINAPI
ODlgDirListW(
  HWND hDlg,
  LPWSTR lpPathSpec,
  int nIDListBox,
  int nIDStaticPath,
  UINT uFileType);


int
WINAPI
ODlgDirListComboBoxW(
  HWND hDlg,
  LPWSTR lpPathSpec,
  int nIDComboBox,
  int nIDStaticPath,
  UINT uFiletype);


BOOL
WINAPI
ODlgDirSelectComboBoxExW(
  HWND hDlg,
  LPWSTR lpString,
  int nCount,
  int nIDComboBox);


BOOL
WINAPI
ODlgDirSelectExW(
  HWND hDlg,
  LPWSTR lpString,
  int nCount,
  int nIDListBox);


DWORD   APIENTRY ODoEnvironmentSubstW(LPWSTR szString, UINT cbString);


BOOL WINAPI ODrawStateW(HDC, HBRUSH, DRAWSTATEPROC, LPARAM, WPARAM, int, int, int, int, UINT);


WINAPI OEditStreamSetInfoW(PAVISTREAM pavi, LPAVISTREAMINFOW lpInfo, LONG cbInfo);


BOOL
WINAPI
OEndUpdateResourceW(
  HANDLE      hUpdate,
  BOOL        fDiscard
  );


BOOL
WINAPI
OEnumCalendarInfoW(
  CALINFO_ENUMPROCW lpCalInfoEnumProc,
  LCID              Locale,
  CALID             Calendar,
  CALTYPE           CalType);


BOOL
WINAPI
OEnumDateFormatsW(
  DATEFMT_ENUMPROCW lpDateFmtEnumProc,
  LCID              Locale,
  DWORD             dwFlags);

#if 0 //UNDONE_POST_98 - We should wrap these as being NT only...

BOOL
WINAPI
OEnumDependentServicesW(
  SC_HANDLE               hService,
  DWORD                   dwServiceState,
  LPENUM_SERVICE_STATUSW  lpServices,
  DWORD                   cbBufSize,
  LPDWORD                 pcbBytesNeeded,
  LPDWORD                 lpServicesReturned
  );
#endif


BOOL
WINAPI
OEnumDesktopsW(
  HWINSTA hwinsta,
  DESKTOPENUMPROCW lpEnumFunc,
  LPARAM lParam);


BOOL
WINAPI
OEnumDisplaySettingsW(
  LPCWSTR lpszDeviceName,
  DWORD iModeNum,
  LPDEVMODEW lpDevMode);


int  WINAPI OEnumFontFamiliesW(HDC, LPCWSTR, FONTENUMPROCW, LPARAM);


int  WINAPI OEnumFontFamiliesExW(HDC, LPLOGFONTW,FONTENUMPROCW, LPARAM,DWORD);


int  WINAPI OEnumFontsW(HDC, LPCWSTR,  FONTENUMPROCW, LPARAM);


BOOL
WINAPI
OEnumFormsW(
  HANDLE  hPrinter,
  DWORD   Level,
  LPBYTE  pForm,
  DWORD   cbBuf,
  LPDWORD pcbNeeded,
  LPDWORD pcReturned
);


WINAPI OEnumICMProfilesW(HDC,ICMENUMPROCW,LPARAM);


BOOL
WINAPI
OEnumJobsW(
  HANDLE  hPrinter,
  DWORD   FirstJob,
  DWORD   NoJobs,
  DWORD   Level,
  LPBYTE  pJob,
  DWORD   cbBuf,
  LPDWORD pcbNeeded,
  LPDWORD pcReturned
);


BOOL
WINAPI
OEnumMonitorsW(
  LPWSTR   pName,
  DWORD   Level,
  LPBYTE  pMonitors,
  DWORD   cbBuf,
  LPDWORD pcbNeeded,
  LPDWORD pcReturned
);


BOOL
WINAPI
OEnumPortsW(
  LPWSTR   pName,
  DWORD   Level,
  LPBYTE  pPorts,
  DWORD   cbBuf,
  LPDWORD pcbNeeded,
  LPDWORD pcReturned
);


BOOL
WINAPI
OEnumPrinterDriversW(
  LPWSTR   pName,
  LPWSTR   pEnvironment,
  DWORD   Level,
  LPBYTE  pDriverInfo,
  DWORD   cbBuf,
  LPDWORD pcbNeeded,
  LPDWORD pcReturned
);


BOOL
WINAPI
OEnumPrintersW(
  DWORD   Flags,
  LPWSTR   Name,
  DWORD   Level,
  LPBYTE  pPrinterEnum,
  DWORD   cbBuf,
  LPDWORD pcbNeeded,
  LPDWORD pcReturned
);


BOOL
WINAPI
OEnumPrintProcessorDatatypesW(
  LPWSTR   pName,
  LPWSTR   pPrintProcessorName,
  DWORD   Level,
  LPBYTE  pDatatypes,
  DWORD   cbBuf,
  LPDWORD pcbNeeded,
  LPDWORD pcReturned
);


BOOL
WINAPI
OEnumPrintProcessorsW(
  LPWSTR   pName,
  LPWSTR   pEnvironment,
  DWORD   Level,
  LPBYTE  pPrintProcessorInfo,
  DWORD   cbBuf,
  LPDWORD pcbNeeded,
  LPDWORD pcReturned
);


int
WINAPI
OEnumPropsW(
  HWND hWnd,
  PROPENUMPROCW lpEnumFunc);


int
WINAPI
OEnumPropsExW(
  HWND hWnd,
  PROPENUMPROCEXW lpEnumFunc,
  LPARAM lParam);


INT
APIENTRY
OEnumProtocolsW (
  IN     LPINT           lpiProtocols,
  IN OUT LPVOID          lpProtocolBuffer,
  IN OUT LPDWORD         lpdwBufferLength
  );


BOOL
WINAPI
OEnumResourceLanguagesW(
  HMODULE hModule,
  LPCWSTR lpType,
  LPCWSTR lpName,
  ENUMRESLANGPROC lpEnumFunc,
  LONG lParam
  );


BOOL
WINAPI
OEnumResourceNamesW(
  HMODULE hModule,
  LPCWSTR lpType,
  ENUMRESNAMEPROC lpEnumFunc,
  LONG lParam
  );


BOOL
WINAPI
OEnumResourceTypesW(
  HMODULE hModule,
  ENUMRESTYPEPROC lpEnumFunc,
  LONG lParam
  );

#if 0 //UNDONE_POST_98 - We should wrap these as being NT only...

BOOL
WINAPI
OEnumServicesStatusW(
  SC_HANDLE               hSCManager,
  DWORD                   dwServiceType,
  DWORD                   dwServiceState,
  LPENUM_SERVICE_STATUSW  lpServices,
  DWORD                   cbBufSize,
  LPDWORD                 pcbBytesNeeded,
  LPDWORD                 lpServicesReturned,
  LPDWORD                 lpResumeHandle
  );
#endif


BOOL
WINAPI
OEnumSystemCodePagesW(
  CODEPAGE_ENUMPROCW lpCodePageEnumProc,
  DWORD              dwFlags);


BOOL
WINAPI
OEnumSystemLocalesW(
  LOCALE_ENUMPROCW lpLocaleEnumProc,
  DWORD            dwFlags);


BOOL
WINAPI
OEnumTimeFormatsW(
  TIMEFMT_ENUMPROCW lpTimeFmtEnumProc,
  LCID              Locale,
  DWORD             dwFlags);


BOOL
WINAPI
OEnumWindowStationsW(
  WINSTAENUMPROCW lpEnumFunc,
  LPARAM lParam);




BOOL
WINAPI
OFillConsoleOutputCharacterW(
  HANDLE hConsoleOutput,
  WCHAR  cCharacter,
  DWORD  nLength,
  COORD  dwWriteCoord,
  LPDWORD lpNumberOfCharsWritten
  );


LPWSTR APIENTRY OFindEnvironmentStringW(LPWSTR szEnvVar);


// FindResourceExW is implemented on Win95, but can corrupt heap on Win95

HRSRC
WINAPI
OFindResourceExW(
  HMODULE hModule,
  LPCWSTR lpType,
  LPCWSTR lpName,
  WORD    wLanguage
  );


 APIENTRY    OFindTextW(LPFINDREPLACEW);


HWND    WINAPI OFindWindowExW(HWND, HWND, LPCWSTR, LPCWSTR);


int
WINAPI
OFoldStringW(
  DWORD    dwMapFlags,
  LPCWSTR lpSrcStr,
  int      cchSrc,
  LPWSTR  lpDestStr,
  int      cchDest);


INT
APIENTRY
OGetAddressByNameW (
  IN     DWORD                dwNameSpace,
  IN     LPGUID               lpServiceType,
  IN     LPWSTR              lpServiceName OPTIONAL,
  IN     LPINT                lpiProtocols OPTIONAL,
  IN     DWORD                dwResolution,
  IN     LPSERVICE_ASYNC_INFO lpServiceAsyncInfo OPTIONAL,
  IN OUT LPVOID               lpCsaddrBuffer,
  IN OUT LPDWORD              lpdwBufferLength,
  IN OUT LPWSTR              lpAliasBuffer OPTIONAL,
  IN OUT LPDWORD              lpdwAliasBufferLength OPTIONAL
  );


BOOL
WINAPI
OGetBinaryTypeW(
  LPCWSTR lpApplicationName,
  LPDWORD lpBinaryType
  );


DWORD WINAPI OGetCharacterPlacementW(HDC, LPCWSTR, int, int, LPGCP_RESULTSW, DWORD);



BOOL  WINAPI OGetCharWidth32W(HDC, UINT, UINT, LPINT);



LPWSTR
WINAPI
OGetCommandLineW(
  VOID
  );


DWORD
WINAPI
OGetCompressedFileSizeW(
  LPCWSTR lpFileName,
  LPDWORD lpFileSizeHigh
  );


BOOL
WINAPI
OGetComputerNameW (
  LPWSTR lpBuffer,
  LPDWORD lpnSize
  );


DWORD
WINAPI
OGetConsoleTitleW(
  LPWSTR lpConsoleTitle,
  DWORD nSize
  );
 


BOOL
WINAPI
OGetDefaultCommConfigW(
  LPCWSTR lpszName,
  LPCOMMCONFIG lpCC,
  LPDWORD lpdwSize
  );


BOOL
WINAPI
OGetDiskFreeSpaceW(
  LPCWSTR lpRootPathName,
  LPDWORD lpSectorsPerCluster,
  LPDWORD lpBytesPerSector,
  LPDWORD lpNumberOfFreeClusters,
  LPDWORD lpTotalNumberOfClusters
  );

BOOL
WINAPI
OGetDiskFreeSpaceExW(
  LPCTSTR lpDirectoryName,
  PULARGE_INTEGER lpFreeBytesAvailableToCaller,
  PULARGE_INTEGER lpTotalNumberOfBytes,
  PULARGE_INTEGER lpTotalNumberOfFreeBytes);


HENHMETAFILE  WINAPI OGetEnhMetaFileW(LPCWSTR);


UINT  WINAPI OGetEnhMetaFileDescriptionW(HENHMETAFILE, UINT, LPWSTR );


INT
APIENTRY
OGetExpandedNameW(
  LPWSTR,
  LPWSTR
  );


BOOL
WINAPI
OGetFileSecurityW (
  LPCWSTR lpFileName,
  SECURITY_INFORMATION RequestedInformation,
  PSECURITY_DESCRIPTOR pSecurityDescriptor,
  DWORD nLength,
  LPDWORD lpnLengthNeeded
  );


BOOL
WINAPI
OGetFormW(
  HANDLE  hPrinter,
  LPWSTR   pFormName,
  DWORD   Level,
  LPBYTE  pForm,
  DWORD   cbBuf,
  LPDWORD pcbNeeded
);


WINAPI OGetICMProfileW(HDC,LPDWORD,LPWSTR);


BOOL
WINAPI
OGetJobW(
   HANDLE   hPrinter,
   DWORD    JobId,
   DWORD    Level,
   LPBYTE   pJob,
   DWORD    cbBuf,
   LPDWORD  pcbNeeded
);


BOOL
WINAPI
OGetKeyboardLayoutNameW(
  LPWSTR pwszKLID);



WINAPI OGetLogColorSpaceW(HCOLORSPACE,LPLOGCOLORSPACEW,DWORD);


DWORD
WINAPI
OGetLogicalDriveStringsW(
  DWORD nBufferLength,
  LPWSTR lpBuffer
  );


BOOL
WINAPI
OGetMenuItemInfoW(
  HMENU,
  UINT,
  BOOL,
  LPMENUITEMINFOW
  );


HMETAFILE   WINAPI OGetMetaFileW(LPCWSTR);


INT
APIENTRY
OGetNameByTypeW (
  IN     LPGUID          lpServiceType,
  IN OUT LPWSTR         lpServiceName,
  IN     DWORD           dwNameLength
  );


BOOL
WINAPI
OGetNamedPipeHandleStateW(
  HANDLE hNamedPipe,
  LPDWORD lpState,
  LPDWORD lpCurInstances,
  LPDWORD lpMaxCollectionCount,
  LPDWORD lpCollectDataTimeout,
  LPWSTR lpUserName,
  DWORD nMaxUserNameSize
  );


BOOL
WINAPI
OGetPrinterW(
  HANDLE  hPrinter,
  DWORD   Level,
  LPBYTE  pPrinter,
  DWORD   cbBuf,
  LPDWORD pcbNeeded
);


DWORD
WINAPI
OGetPrinterDataW(
  HANDLE   hPrinter,
  LPWSTR    pValueName,
  LPDWORD  pType,
  LPBYTE   pData,
  DWORD    nSize,
  LPDWORD  pcbNeeded
);


BOOL
WINAPI
OGetPrinterDriverW(
  HANDLE  hPrinter,
  LPWSTR   pEnvironment,
  DWORD   Level,
  LPBYTE  pDriverInfo,
  DWORD   cbBuf,
  LPDWORD pcbNeeded
);


BOOL
WINAPI
OGetPrinterDriverDirectoryW(
  LPWSTR   pName,
  LPWSTR   pEnvironment,
  DWORD   Level,
  LPBYTE  pDriverDirectory,
  DWORD   cbBuf,
  LPDWORD pcbNeeded
);


BOOL
WINAPI
OGetPrintProcessorDirectoryW(
  LPWSTR   pName,
  LPWSTR   pEnvironment,
  DWORD   Level,
  LPBYTE  pPrintProcessorInfo,
  DWORD   cbBuf,
  LPDWORD pcbNeeded
);


DWORD
WINAPI
OGetPrivateProfileSectionW(
  LPCWSTR lpAppName,
  LPWSTR lpReturnedString,
  DWORD nSize,
  LPCWSTR lpFileName
  );


DWORD
WINAPI
OGetPrivateProfileSectionNamesW(
  LPWSTR lpszReturnBuffer,
  DWORD nSize,
  LPCWSTR lpFileName
  );


BOOL
WINAPI
OGetPrivateProfileStructW(
  LPCWSTR lpszSection,
  LPCWSTR lpszKey,
  LPVOID   lpStruct,
  UINT     uSizeStruct,
  LPCWSTR szFile
  );


DWORD
WINAPI
OGetProfileSectionW(
  LPCWSTR lpAppName,
  LPWSTR lpReturnedString,
  DWORD nSize
  );


DWORD
WINAPI
OGetProfileStringW(
  LPCWSTR lpAppName,
  LPCWSTR lpKeyName,
  LPCWSTR lpDefault,
  LPWSTR lpReturnedString,
  DWORD nSize
  );


INT
APIENTRY
OGetServiceW (
  IN     DWORD                dwNameSpace,
  IN     LPGUID               lpGuid,
  IN     LPWSTR              lpServiceName,
  IN     DWORD                dwProperties,
  IN OUT LPVOID               lpBuffer,
  IN OUT LPDWORD              lpdwBufferSize,
  IN     LPSERVICE_ASYNC_INFO lpServiceAsyncInfo
  );

#if 0 //UNDONE_POST_98 - We should wrap these as being NT only...

BOOL
WINAPI
OGetServiceDisplayNameW(
  SC_HANDLE               hSCManager,
  LPCWSTR                lpServiceName,
  LPWSTR                 lpDisplayName,
  LPDWORD                 lpcchBuffer
  );


BOOL
WINAPI
OGetServiceKeyNameW(
  SC_HANDLE               hSCManager,
  LPCWSTR                lpDisplayName,
  LPWSTR                 lpServiceName,
  LPDWORD                 lpcchBuffer
  );
#endif


VOID
WINAPI
OGetStartupInfoW(
  LPSTARTUPINFOW lpStartupInfo
  );



UINT
WINAPI
OGetSystemDirectoryW(
  LPWSTR lpBuffer,
  UINT uSize
  );




INT
APIENTRY
OGetTypeByNameW (
  IN     LPWSTR         lpServiceName,
  IN OUT LPGUID          lpServiceType
  );



BOOL
WINAPI
OGetUserObjectInformationW(
  HANDLE hObj,
  int nIndex,
  PVOID pvInfo,
  DWORD nLength,
  LPDWORD lpnLengthNeeded);



UINT
WINAPI
OGetWindowsDirectoryW(
  LPWSTR lpBuffer,
  UINT uSize
  );




RPC_STATUS RPC_ENTRY
OI_RpcServerUnregisterEndpointW (
  IN unsigned short * Protseq,
  IN unsigned short * Endpoint
  );



   WINAPI OImmConfigureIMEW(HKL, HWND, DWORD, LPVOID);



WINAPI OImmEnumRegisterWordW(HKL, REGISTERWORDENUMPROCW, LPCWSTR lpszReading, DWORD, LPCWSTR lpszRegister, LPVOID);



WINAPI OImmEscapeW(HKL, HIMC, UINT, LPVOID);



WINAPI OImmGetCandidateListW(HIMC, DWORD deIndex, LPCANDIDATELIST, DWORD dwBufLen);



WINAPI OImmGetCandidateListCountW(HIMC, LPDWORD lpdwListCount);



WINAPI OImmGetCompositionFontW(HIMC, LPLOGFONTW);



WINAPI OImmGetConversionListW(HKL, HIMC, LPCWSTR, LPCANDIDATELIST, DWORD dwBufLen, UINT uFlag);



WINAPI OImmGetDescriptionW(HKL, LPWSTR, UINT uBufLen);



WINAPI OImmGetGuideLineW(HIMC, DWORD dwIndex, LPWSTR, DWORD dwBufLen);



WINAPI OImmGetIMEFileNameW(HKL, LPWSTR, UINT uBufLen);



WINAPI OImmGetRegisterWordStyleW(HKL, UINT nItem, LPSTYLEBUFW);



 WINAPI OImmInstallIMEW(LPCWSTR lpszIMEFileName, LPCWSTR lpszLayoutText);



WINAPI OImmIsUIMessageW(HWND, UINT, WPARAM, LPARAM);



WINAPI OImmRegisterWordW(HKL, LPCWSTR lpszReading, DWORD, LPCWSTR lpszRegister);



WINAPI OImmSetCompositionFontW(HIMC, LPLOGFONTW);



 WINAPI OImmSetCompositionStringW(HIMC, DWORD dwIndex, LPCVOID lpComp, DWORD, LPCVOID lpRead, DWORD);



WINAPI OImmUnregisterWordW(HKL, LPCWSTR lpszReading, DWORD, LPCWSTR lpszUnregister);



BOOL
APIENTRY
OInitiateSystemShutdownW(
  LPWSTR lpMachineName,
  LPWSTR lpMessage,
  DWORD dwTimeout,
  BOOL bForceAppsClosed,
  BOOL bRebootAfterShutdown
  );



BOOL
WINAPI
OInsertMenuItemW(
  HMENU,
  UINT,
  BOOL,
  LPCMENUITEMINFOW
  );



BOOL
WINAPI
OIsCharLowerW(
  WCHAR ch);



BOOL
WINAPI
OIsCharUpperW(
  WCHAR ch);



MMRESULT WINAPI OjoyGetDevCapsW(UINT uJoyID, LPJOYCAPSW pjc, UINT cbjc);



HCURSOR
WINAPI
OLoadCursorFromFileW(
  LPCWSTR    lpFileName);



HKL
WINAPI
OLoadKeyboardLayoutW(
  LPCWSTR pwszKLID,
  UINT Flags);



BOOL
WINAPI
OLogonUserW (
  LPWSTR lpszUsername,
  LPWSTR lpszDomain,
  LPWSTR lpszPassword,
  DWORD dwLogonType,
  DWORD dwLogonProvider,
  PHANDLE phToken
  );



INT
APIENTRY
OLZOpenFileW(
  LPWSTR,
  LPOFSTRUCT,
  WORD
  );



UINT
WINAPI
OMapVirtualKeyExW(
  UINT uCode,
  UINT uMapType,
  HKL dwhkl);



MCIDEVICEID WINAPI OmciGetDeviceIDW(LPCWSTR pszDevice);



MCIDEVICEID WINAPI OmciGetDeviceIDFromElementIDW(DWORD dwElementID, LPCWSTR lpstrType );



BOOL WINAPI OmciGetErrorStringW(MCIERROR mcierr, LPWSTR pszText, UINT cchText);



MCIERROR WINAPI OmciSendCommandW(MCIDEVICEID mciId, UINT uMsg, DWORD dwParam1, DWORD dwParam2);



MCIERROR  WINAPI OmciSendStringW(LPCWSTR lpstrCommand, LPWSTR lpstrReturnString, UINT uReturnLength, HWND hwndCallback);



VFWAPIV OMCIWndCreateW(HWND hwndParent, HINSTANCE hInstance,
        DWORD dwStyle,LPCWSTR szFile);



MMRESULT WINAPI OmidiInGetDevCapsW(UINT uDeviceID, LPMIDIINCAPSW pmic, UINT cbmic);



MMRESULT WINAPI OmidiInGetErrorTextW(MMRESULT mmrError, LPWSTR pszText, UINT cchText);



MMRESULT WINAPI OmidiOutGetDevCapsW(UINT uDeviceID, LPMIDIOUTCAPSW pmoc, UINT cbmoc);



MMRESULT WINAPI OmidiOutGetErrorTextW(MMRESULT mmrError, LPWSTR pszText, UINT cchText);



HRESULT WINAPI OMIMEAssociationDialogW(HWND hwndParent,
                     DWORD dwInFlags,
                     PCWSTR pcszFile,
                     PCWSTR pcszMIMEContentType,
                     PWSTR pszAppBuf,
                     UINT ucAppBufLen);



MMRESULT WINAPI OmixerGetControlDetailsW(HMIXEROBJ hmxobj, LPMIXERCONTROLDETAILS pmxcd, DWORD fdwDetails);



MMRESULT WINAPI OmixerGetDevCapsW(UINT uMxId, LPMIXERCAPSW pmxcaps, UINT cbmxcaps);



MMRESULT WINAPI OmixerGetLineControlsW(HMIXEROBJ hmxobj, LPMIXERLINECONTROLSW pmxlc, DWORD fdwControls);



MMRESULT WINAPI OmixerGetLineInfoW(HMIXEROBJ hmxobj, LPMIXERLINEW pmxl, DWORD fdwInfo);



LPMMIOPROC WINAPI OmmioInstallIOProcW(FOURCC fccIOProc, LPMMIOPROC pIOProc, DWORD dwFlags);



HMMIO WINAPI OmmioOpenW(LPWSTR pszFileName, LPMMIOINFO pmmioinfo, DWORD fdwOpen);



MMRESULT WINAPI OmmioRenameW(LPCWSTR pszFileName, LPCWSTR pszNewFileName, LPCMMIOINFO pmmioinfo, DWORD fdwRename);



FOURCC WINAPI OmmioStringToFOURCCW(LPCWSTR sz, UINT uFlags);



DWORD APIENTRY
OMultinetGetConnectionPerformanceW(
    LPNETRESOURCEW lpNetResource,
    LPNETCONNECTINFOSTRUCT lpNetConnectInfoStruct
    );



BOOL
WINAPI
OObjectCloseAuditAlarmW (
  LPCWSTR SubsystemName,
  LPVOID HandleId,
  BOOL GenerateOnClose
  );



BOOL
WINAPI
OObjectOpenAuditAlarmW (
  LPCWSTR SubsystemName,
  LPVOID HandleId,
  LPWSTR ObjectTypeName,
  LPWSTR ObjectName,
  PSECURITY_DESCRIPTOR pSecurityDescriptor,
  HANDLE ClientToken,
  DWORD DesiredAccess,
  DWORD GrantedAccess,
  PPRIVILEGE_SET Privileges,
  BOOL ObjectCreation,
  BOOL AccessGranted,
  LPBOOL GenerateOnClose
  );



BOOL
WINAPI
OObjectPrivilegeAuditAlarmW (
  LPCWSTR SubsystemName,
  LPVOID HandleId,
  HANDLE ClientToken,
  DWORD DesiredAccess,
  PPRIVILEGE_SET Privileges,
  BOOL AccessGranted
  );



BOOL
WINAPI
OOemToCharBuffW(
  LPCSTR lpszSrc,
  LPWSTR lpszDst,
  DWORD cchDstLength);



BOOL
WINAPI OOleUIAddVerbMenuW(LPOLEOBJECT lpOleObj, LPCWSTR lpszShortType,
    HMENU hMenu, UINT uPos, UINT uIDVerbMin, UINT uIDVerbMax,
    BOOL bAddConvert, UINT idConvert, HMENU FAR *lphMenu);



UINT
WINAPI OOleUIEditLinksW(LPOLEUIEDITLINKSW);



UINT
WINAPI OOleUIPasteSpecialW(LPOLEUIPASTESPECIALW);



FAR CDECL OOleUIPromptUserW(int nTemplate, HWND hwndParent, ...);



BOOL
WINAPI OOleUIUpdateLinksW(LPOLEUILINKCONTAINERW lpOleUILinkCntr,
    HWND hwndParent, LPWSTR lpszTitle, int cLinks);



HANDLE
WINAPI
OOpenBackupEventLogW (
  LPCWSTR lpUNCServerName,
  LPCWSTR lpFileName
  );



HDESK
WINAPI
OOpenDesktopW(
  LPWSTR lpszDesktop,
  DWORD dwFlags,
  BOOL fInherit,
  DWORD dwDesiredAccess);



HANDLE
WINAPI
OOpenEventW(
  DWORD dwDesiredAccess,
  BOOL bInheritHandle,
  LPCWSTR lpName
  );



HANDLE
WINAPI
OOpenEventLogW (
  LPCWSTR lpUNCServerName,
  LPCWSTR lpSourceName
  );



HANDLE
WINAPI
OOpenFileMappingW(
  DWORD dwDesiredAccess,
  BOOL bInheritHandle,
  LPCWSTR lpName
  );



HANDLE
WINAPI
OOpenMutexW(
  DWORD dwDesiredAccess,
  BOOL bInheritHandle,
  LPCWSTR lpName
  );


#if 0 //UNDONE_POST_98 - We should wrap these as being NT only...

SC_HANDLE
WINAPI
OOpenSCManagerW(
  LPCWSTR lpMachineName,
  LPCWSTR lpDatabaseName,
  DWORD   dwDesiredAccess
  );
#endif



HANDLE
WINAPI
OOpenSemaphoreW(
  DWORD dwDesiredAccess,
  BOOL bInheritHandle,
  LPCWSTR lpName
  );


#if 0 //UNDONE_POST_98 - We should wrap these as being NT only...

SC_HANDLE
WINAPI
OOpenServiceW(
  SC_HANDLE   hSCManager,
  LPCWSTR    lpServiceName,
  DWORD       dwDesiredAccess
  );
#endif



HWINSTA
WINAPI
OOpenWindowStationW(
  LPWSTR lpszWinSta,
  BOOL fInherit,
  DWORD dwDesiredAccess);



APIENTRY OPageSetupDlgW( LPPAGESETUPDLGW );



BOOL
WINAPI
OPeekConsoleInputW(
  HANDLE hConsoleInput,
  PINPUT_RECORD lpBuffer,
  DWORD nLength,
  LPDWORD lpNumberOfEventsRead
  );



BOOL  WINAPI OPolyTextOutW(HDC, CONST POLYTEXTW *, int);



DWORD
WINAPI
OPrinterMessageBoxW(
  HANDLE  hPrinter,
  DWORD   Error,
  HWND    hWnd,
  LPWSTR   pText,
  LPWSTR   pCaption,
  DWORD   dwType
);



BOOL
WINAPI
OPrivilegedServiceAuditAlarmW (
  LPCWSTR SubsystemName,
  LPCWSTR ServiceName,
  HANDLE ClientToken,
  PPRIVILEGE_SET Privileges,
  BOOL AccessGranted
  );



DWORD
WINAPI
OQueryDosDeviceW(
  LPCWSTR lpDeviceName,
  LPWSTR lpTargetPath,
  DWORD ucchMax
  );


#if 0 //UNDONE_POST_98 - We should wrap these as being NT only...

BOOL
WINAPI
OQueryServiceConfigW(
  SC_HANDLE               hService,
  LPQUERY_SERVICE_CONFIGW lpServiceConfig,
  DWORD                   cbBufSize,
  LPDWORD                 pcbBytesNeeded
  );



BOOL
WINAPI
OQueryServiceLockStatusW(
  SC_HANDLE                       hSCManager,
  LPQUERY_SERVICE_LOCK_STATUSW    lpLockStatus,
  DWORD                           cbBufSize,
  LPDWORD                         pcbBytesNeeded
  );
#endif



APIENTRY ORasDialW( LPRASDIALEXTENSIONS, LPWSTR, LPRASDIALPARAMSW, DWORD,
           LPVOID, LPHRASCONN );



BOOL
WINAPI
OReadConsoleW(
  HANDLE hConsoleInput,
  LPVOID lpBuffer,
  DWORD nNumberOfCharsToRead,
  LPDWORD lpNumberOfCharsRead,
  LPVOID lpReserved
  );



BOOL
WINAPI
OReadConsoleInputW(
  HANDLE hConsoleInput,
  PINPUT_RECORD lpBuffer,
  DWORD nLength,
  LPDWORD lpNumberOfEventsRead
  );



BOOL
WINAPI
OReadConsoleOutputW(
  HANDLE hConsoleOutput,
  PCHAR_INFO lpBuffer,
  COORD dwBufferSize,
  COORD dwBufferCoord,
  PSMALL_RECT lpReadRegion
  );



BOOL
WINAPI
OReadConsoleOutputCharacterW(
  HANDLE hConsoleOutput,
  LPWSTR lpCharacter,
  DWORD nLength,
  COORD dwReadCoord,
  LPDWORD lpNumberOfCharsRead
  );



BOOL
WINAPI
OReadEventLogW (
   HANDLE     hEventLog,
   DWORD      dwReadFlags,
   DWORD      dwRecordOffset,
   LPVOID     lpBuffer,
   DWORD      nNumberOfBytesToRead,
   DWORD      *pnBytesRead,
   DWORD      *pnMinNumberOfBytesNeeded
  );



LONG
APIENTRY
ORegConnectRegistryW (
  LPWSTR lpMachineName,
  HKEY hKey,
  PHKEY phkResult
  );



HANDLE
WINAPI
ORegisterEventSourceW (
  LPCWSTR lpUNCServerName,
  LPCWSTR lpSourceName
  );


#if 0 //UNDONE_POST_98 - We should wrap these as being NT only...

SERVICE_STATUS_HANDLE
WINAPI
ORegisterServiceCtrlHandlerW(
  LPCWSTR             lpServiceName,
  LPHANDLER_FUNCTION   lpHandlerProc
  );
#endif



LONG
APIENTRY
ORegLoadKeyW (
  HKEY    hKey,
  LPCWSTR  lpSubKey,
  LPCWSTR  lpFile
  );



LONG
APIENTRY
ORegQueryMultipleValuesW (
  HKEY hKey,
  PVALENTW val_list,
  DWORD num_vals,
  LPWSTR lpValueBuf,
  LPDWORD ldwTotsize
  );



LONG
APIENTRY
ORegReplaceKeyW (
  HKEY     hKey,
  LPCWSTR  lpSubKey,
  LPCWSTR  lpNewFile,
  LPCWSTR  lpOldFile
  );



LONG
APIENTRY
ORegRestoreKeyW (
  HKEY hKey,
  LPCWSTR lpFile,
  DWORD   dwFlags
  );



LONG
APIENTRY
ORegSaveKeyW (
  HKEY hKey,
  LPCWSTR lpFile,
  LPSECURITY_ATTRIBUTES lpSecurityAttributes
  );



LONG
APIENTRY
ORegUnLoadKeyW (
  HKEY    hKey,
  LPCWSTR lpSubKey
  );



BOOL
WINAPI
ORemoveDirectoryW(
  LPCWSTR lpPathName
  );



BOOL WINAPI ORemoveFontResourceW(LPCWSTR);



 APIENTRY    OReplaceTextW(LPFINDREPLACEW);



BOOL
WINAPI
OReportEventW (
   HANDLE     hEventLog,
   WORD       wType,
   WORD       wCategory,
   DWORD      dwEventID,
   PSID       lpUserSid,
   WORD       wNumStrings,
   DWORD      dwDataSize,
   LPCWSTR   *lpStrings,
   LPVOID     lpRawData
  );



BOOL
WINAPI
OResetPrinterW(
   HANDLE   hPrinter,
   LPPRINTER_DEFAULTSW pDefault
);



RPC_STATUS RPC_ENTRY
ORpcBindingFromStringBindingW (
  IN unsigned short __RPC_FAR * StringBinding,
  OUT RPC_BINDING_HANDLE __RPC_FAR * Binding
  );



RPC_STATUS RPC_ENTRY
ORpcBindingInqAuthClientW (
  IN RPC_BINDING_HANDLE ClientBinding, OPTIONAL
  OUT RPC_AUTHZ_HANDLE __RPC_FAR * Privs,
  OUT unsigned short __RPC_FAR * __RPC_FAR * ServerPrincName, OPTIONAL
  OUT unsigned long __RPC_FAR * AuthnLevel, OPTIONAL
  OUT unsigned long __RPC_FAR * AuthnSvc, OPTIONAL
  OUT unsigned long __RPC_FAR * AuthzSvc OPTIONAL
  );



RPC_STATUS RPC_ENTRY
ORpcBindingToStringBindingW (
  IN RPC_BINDING_HANDLE Binding,
  OUT unsigned short __RPC_FAR * __RPC_FAR * StringBinding
  );



RPC_STATUS RPC_ENTRY
ORpcEpRegisterNoReplaceW (
  IN RPC_IF_HANDLE IfSpec,
  IN RPC_BINDING_VECTOR * BindingVector,
  IN UUID_VECTOR * UuidVector OPTIONAL,
  IN unsigned short  * Annotation
  );



RPC_STATUS RPC_ENTRY
ORpcMgmtEpEltInqNextW (
  IN RPC_EP_INQ_HANDLE InquiryContext,
  OUT RPC_IF_ID __RPC_FAR * IfId,
  OUT RPC_BINDING_HANDLE __RPC_FAR * Binding OPTIONAL,
  OUT UUID __RPC_FAR * ObjectUuid OPTIONAL,
  OUT unsigned short __RPC_FAR * __RPC_FAR * Annotation OPTIONAL
  );



RPC_STATUS RPC_ENTRY
ORpcMgmtInqServerPrincNameW (
  IN RPC_BINDING_HANDLE Binding,
  IN unsigned long AuthnSvc,
  OUT unsigned short __RPC_FAR * __RPC_FAR * ServerPrincName
  );



RPC_STATUS RPC_ENTRY
ORpcNetworkInqProtseqsW (
  OUT RPC_PROTSEQ_VECTORW __RPC_FAR * __RPC_FAR * ProtseqVector
  );



RPC_STATUS RPC_ENTRY
ORpcNetworkIsProtseqValidW (
  IN unsigned short __RPC_FAR * Protseq
  );



RPC_STATUS RPC_ENTRY
ORpcNsBindingInqEntryNameW (
  IN RPC_BINDING_HANDLE Binding,
  IN unsigned long EntryNameSyntax,
  OUT unsigned short __RPC_FAR * __RPC_FAR * EntryName
  );



RPC_STATUS RPC_ENTRY
ORpcProtseqVectorFreeW (
  IN OUT RPC_PROTSEQ_VECTORW __RPC_FAR * __RPC_FAR * ProtseqVector
  );



RPC_STATUS RPC_ENTRY
ORpcServerInqDefaultPrincNameW (
  IN unsigned long AuthnSvc,
  OUT unsigned short __RPC_FAR * __RPC_FAR * PrincName
  );



RPC_STATUS RPC_ENTRY
ORpcServerUseProtseqW (
  IN unsigned short __RPC_FAR * Protseq,
  IN unsigned int MaxCalls,
  IN void __RPC_FAR * SecurityDescriptor OPTIONAL
  );



RPC_STATUS RPC_ENTRY
ORpcServerUseProtseqEpW (
  IN unsigned short __RPC_FAR * Protseq,
  IN unsigned int MaxCalls,
  IN unsigned short __RPC_FAR * Endpoint,
  IN void __RPC_FAR * SecurityDescriptor OPTIONAL
  );



RPC_STATUS RPC_ENTRY
ORpcServerUseProtseqIfW (
  IN unsigned short __RPC_FAR * Protseq,
  IN unsigned int MaxCalls,
  IN RPC_IF_HANDLE IfSpec,
  IN void __RPC_FAR * SecurityDescriptor OPTIONAL
  );



RPC_STATUS RPC_ENTRY
ORpcStringBindingComposeW (
  IN unsigned short __RPC_FAR * ObjUuid OPTIONAL,
  IN unsigned short __RPC_FAR * Protseq OPTIONAL,
  IN unsigned short __RPC_FAR * NetworkAddr OPTIONAL,
  IN unsigned short __RPC_FAR * Endpoint OPTIONAL,
  IN unsigned short __RPC_FAR * Options OPTIONAL,
  OUT unsigned short __RPC_FAR * __RPC_FAR * StringBinding OPTIONAL
  );



RPC_STATUS RPC_ENTRY
ORpcStringBindingParseW (
  IN unsigned short __RPC_FAR * StringBinding,
  OUT unsigned short __RPC_FAR * __RPC_FAR * ObjUuid OPTIONAL,
  OUT unsigned short __RPC_FAR * __RPC_FAR * Protseq OPTIONAL,
  OUT unsigned short __RPC_FAR * __RPC_FAR * NetworkAddr OPTIONAL,
  OUT unsigned short __RPC_FAR * __RPC_FAR * Endpoint OPTIONAL,
  OUT unsigned short __RPC_FAR * __RPC_FAR * NetworkOptions OPTIONAL
  );



RPC_STATUS RPC_ENTRY
ORpcStringFreeW (
  IN OUT unsigned short __RPC_FAR * __RPC_FAR * String
  );



BOOL
WINAPI
OScrollConsoleScreenBufferW(
  HANDLE hConsoleOutput,
  CONST SMALL_RECT *lpScrollRectangle,
  CONST SMALL_RECT *lpClipRectangle,
  COORD dwDestinationOrigin,
  CONST CHAR_INFO *lpFill
  );



DWORD
WINAPI
OSearchPathW(
  LPCWSTR lpPath,
  LPCWSTR lpFileName,
  LPCWSTR lpExtension,
  DWORD nBufferLength,
  LPWSTR lpBuffer,
  LPWSTR *lpFilePart
  );



BOOL
WINAPI
OSendMessageCallbackW(
  HWND hWnd,
  UINT Msg,
  WPARAM wParam,
  LPARAM lParam,
  SENDASYNCPROC lpResultCallBack,
  DWORD dwData);



LRESULT
WINAPI
OSendMessageTimeoutW(
  HWND hWnd,
  UINT Msg,
  WPARAM wParam,
  LPARAM lParam,
  UINT fuFlags,
  UINT uTimeout,
  LPDWORD lpdwResult);



BOOL
WINAPI
OSetComputerNameW (
  LPCWSTR lpComputerName
  );



BOOL
WINAPI
OSetConsoleTitleW(
  LPCWSTR lpConsoleTitle
  );



BOOL
WINAPI
OSetDefaultCommConfigW(
  LPCWSTR lpszName,
  LPCOMMCONFIG lpCC,
  DWORD dwSize
  );



BOOL
WINAPI
OSetFileSecurityW (
  LPCWSTR lpFileName,
  SECURITY_INFORMATION SecurityInformation,
  PSECURITY_DESCRIPTOR pSecurityDescriptor
  );



BOOL
WINAPI
OSetFormW(
  HANDLE  hPrinter,
  LPWSTR   pFormName,
  DWORD   Level,
  LPBYTE  pForm
);



WINAPI OSetICMProfileW(HDC,LPWSTR);



BOOL
WINAPI
OSetJobW(
  HANDLE  hPrinter,
  DWORD   JobId,
  DWORD   Level,
  LPBYTE  pJob,
  DWORD   Command
);



BOOL
WINAPI
OSetPrinterW(
  HANDLE  hPrinter,
  DWORD   Level,
  LPBYTE  pPrinter,
  DWORD   Command
);



DWORD
WINAPI
OSetPrinterDataW(
  HANDLE  hPrinter,
  LPWSTR   pValueName,
  DWORD   Type,
  LPBYTE  pData,
  DWORD   cbData
);



INT
APIENTRY
OSetServiceW (
  IN     DWORD                dwNameSpace,
  IN     DWORD                dwOperation,
  IN     DWORD                dwFlags,
  IN     LPSERVICE_INFOW      lpServiceInfo,
  IN     LPSERVICE_ASYNC_INFO lpServiceAsyncInfo,
  IN OUT LPDWORD              lpdwStatusFlags
  );



BOOL
WINAPI
OSetUserObjectInformationW(
  HANDLE hObj,
  int nIndex,
  PVOID pvInfo,
  DWORD nLength);



BOOL
WINAPI
OSetVolumeLabelW(
  LPCWSTR lpRootPathName,
  LPCWSTR lpVolumeName
  );



HHOOK
WINAPI
OSetWindowsHookW(
  int nFilterType,
  HOOKPROC pfnFilterProc);



INT       APIENTRY OShellAboutW(HWND hWnd, LPCWSTR szApp, LPCWSTR szOtherStuff, HICON hIcon);



HINSTANCE APIENTRY OShellExecuteW(HWND hwnd, LPCWSTR lpOperation, LPCWSTR lpFile, LPCWSTR lpParameters, LPCWSTR lpDirectory, INT nShowCmd);



int WINAPI OSHFileOperationW(LPSHFILEOPSTRUCTW lpFileOp);



BOOL WINAPI OSHGetNewLinkInfoW(LPCWSTR pszLinkTo, LPCWSTR pszDir, LPWSTR pszName,
               BOOL FAR * pfMustCopy, UINT uFlags);



BOOL WINAPI OsndPlaySoundW(LPCWSTR pszSound, UINT fuSound);



DWORD
WINAPI
OStartDocPrinterW(
  HANDLE  hPrinter,
  DWORD   Level,
  LPBYTE  pDocInfo
);


#if 0 //UNDONE_POST_98 - We should wrap these as being NT only...

BOOL
WINAPI
OStartServiceW(
  SC_HANDLE            hService,
  DWORD                dwNumServiceArgs,
  LPCWSTR             *lpServiceArgVectors
  );



BOOL
WINAPI
OStartServiceCtrlDispatcherW(
  LPSERVICE_TABLE_ENTRYW    lpServiceStartTable
  );
#endif


// Commented since gdi32.dll on Win95 provides the wrapper for this function.
/*

BOOL  WINAPI OTextOutW(HDC, int, int, LPCWSTR, int);
*/



HRESULT WINAPI OTranslateURLW(PCWSTR pcszURL,
                     DWORD dwInFlags,
                     PWSTR *ppszTranslatedURL);



WINAPI OUpdateICMRegKeyW(DWORD, DWORD, LPWSTR, UINT);



HRESULT WINAPI OURLAssociationDialogW(HWND hwndParent,
                         DWORD dwInFlags,
                         PCWSTR pcszFile,
                         PCWSTR pcszURL,
                         PWSTR pszAppBuf,
                         UINT ucAppBufLen);



/* client/server */
RPC_STATUS RPC_ENTRY
OUuidFromStringW (
  IN unsigned short __RPC_FAR * StringUuid,
  OUT UUID __RPC_FAR * Uuid
  );



DWORD
APIENTRY
OVerFindFileW(
    DWORD uFlags,
    LPWSTR szFileName,
    LPWSTR szWinDir,
    LPWSTR szAppDir,
    LPWSTR szCurDir,
    PUINT lpuCurDirLen,
    LPWSTR szDestDir,
    PUINT lpuDestDirLen
    );



DWORD
APIENTRY
OVerInstallFileW(
    DWORD uFlags,
    LPWSTR szSrcFileName,
    LPWSTR szDestFileName,
    LPWSTR szSrcDir,
    LPWSTR szDestDir,
    LPWSTR szCurDir,
    LPWSTR szTmpFile,
    PUINT lpuTmpFileLen
    );



WINAPI OVkKeyScanExW(
  WCHAR  ch,
  HKL   dwhkl);



BOOL
WINAPI
OWaitNamedPipeW(
  LPCWSTR lpNamedPipeName,
  DWORD nTimeOut
  );



MMRESULT WINAPI OwaveInGetDevCapsW(UINT uDeviceID, LPWAVEINCAPSW pwic, UINT cbwic);



MMRESULT WINAPI OwaveInGetErrorTextW(MMRESULT mmrError, LPWSTR pszText, UINT cchText);



MMRESULT WINAPI OwaveOutGetDevCapsW(UINT uDeviceID, LPWAVEOUTCAPSW pwoc, UINT cbwoc);



MMRESULT WINAPI OwaveOutGetErrorTextW(MMRESULT mmrError, LPWSTR pszText, UINT cchText);



BOOL  WINAPI OwglUseFontBitmapsW(HDC, DWORD, DWORD, DWORD);



BOOL  WINAPI OwglUseFontOutlinesW(HDC, DWORD, DWORD, DWORD, FLOAT,
                       FLOAT, int, LPGLYPHMETRICSFLOAT);



void WINAPI OWinExecErrorW(HWND hwnd, int error, LPCWSTR lpstrFileName, LPCWSTR lpstrTitle);



DWORD APIENTRY
OWNetAddConnectionW(
   LPCWSTR   lpRemoteName,
   LPCWSTR   lpPassword,
   LPCWSTR   lpLocalName
  );



DWORD APIENTRY
OWNetAddConnection2W(
   LPNETRESOURCEW lpNetResource,
   LPCWSTR       lpPassword,
   LPCWSTR       lpUserName,
   DWORD          dwFlags
  );



DWORD APIENTRY
OWNetAddConnection3W(
   HWND           hwndOwner,
   LPNETRESOURCEW lpNetResource,
   LPCWSTR       lpPassword,
   LPCWSTR       lpUserName,
   DWORD          dwFlags
  );



DWORD APIENTRY
OWNetCancelConnectionW(
   LPCWSTR lpName,
   BOOL     fForce
  );



DWORD APIENTRY
OWNetCancelConnection2W(
   LPCWSTR lpName,
   DWORD    dwFlags,
   BOOL     fForce
  );



DWORD APIENTRY
OWNetConnectionDialog1W(
  LPCONNECTDLGSTRUCTW lpConnDlgStruct
  );



DWORD APIENTRY
OWNetDisconnectDialog1W(
  LPDISCDLGSTRUCTW lpConnDlgStruct
  );



DWORD APIENTRY
OWNetEnumResourceW(
   HANDLE  hEnum,
   LPDWORD lpcCount,
   LPVOID  lpBuffer,
   LPDWORD lpBufferSize
  );




DWORD APIENTRY
OWNetGetLastErrorW(
   LPDWORD    lpError,
   LPWSTR    lpErrorBuf,
   DWORD      nErrorBufSize,
   LPWSTR    lpNameBuf,
   DWORD      nNameBufSize
  );



DWORD APIENTRY
OWNetGetNetworkInformationW(
  LPCWSTR          lpProvider,
  LPNETINFOSTRUCT   lpNetInfoStruct
  );



DWORD APIENTRY
OWNetGetProviderNameW(
  DWORD   dwNetType,
  LPWSTR lpProviderName,
  LPDWORD lpBufferSize
  );



DWORD APIENTRY
OWNetGetUniversalNameW(
   LPCWSTR lpLocalPath,
   DWORD    dwInfoLevel,
   LPVOID   lpBuffer,
   LPDWORD  lpBufferSize
   );



DWORD APIENTRY
OWNetGetUserW(
   LPCWSTR  lpName,
   LPWSTR   lpUserName,
   LPDWORD   lpnLength
  );



DWORD APIENTRY
OWNetOpenEnumW(
   DWORD          dwScope,
   DWORD          dwType,
   DWORD          dwUsage,
   LPNETRESOURCEW lpNetResource,
   LPHANDLE       lphEnum
  );



DWORD APIENTRY
OWNetSetConnectionW(
  LPCWSTR    lpName,
  DWORD       dwProperties,
  LPVOID      pvValues
  );



DWORD APIENTRY
OWNetUseConnectionW(
  HWND            hwndOwner,
  LPNETRESOURCEW  lpNetResource,
  LPCWSTR        lpUserID,
  LPCWSTR        lpPassword,
  DWORD           dwFlags,
  LPWSTR         lpAccessName,
  LPDWORD         lpBufferSize,
  LPDWORD         lpResult
  );



BOOL
WINAPI
OWriteConsoleW(
  HANDLE hConsoleOutput,
  CONST VOID *lpBuffer,
  DWORD nNumberOfCharsToWrite,
  LPDWORD lpNumberOfCharsWritten,
  LPVOID lpReserved
  );



BOOL
WINAPI
OWriteConsoleInputW(
  HANDLE hConsoleInput,
  CONST INPUT_RECORD *lpBuffer,
  DWORD nLength,
  LPDWORD lpNumberOfEventsWritten
  );



BOOL
WINAPI
OWriteConsoleOutputW(
  HANDLE hConsoleOutput,
  CONST CHAR_INFO *lpBuffer,
  COORD dwBufferSize,
  COORD dwBufferCoord,
  PSMALL_RECT lpWriteRegion
  );



BOOL
WINAPI
OWriteConsoleOutputCharacterW(
  HANDLE hConsoleOutput,
  LPCWSTR lpCharacter,
  DWORD nLength,
  COORD dwWriteCoord,
  LPDWORD lpNumberOfCharsWritten
  );



BOOL
WINAPI
OWritePrivateProfileSectionW(
  LPCWSTR lpAppName,
  LPCWSTR lpString,
  LPCWSTR lpFileName
  );



BOOL
WINAPI
OWritePrivateProfileStructW(
  LPCWSTR lpszSection,
  LPCWSTR lpszKey,
  LPVOID   lpStruct,
  UINT     uSizeStruct,
  LPCWSTR szFile
  );



BOOL
WINAPI
OWriteProfileSectionW(
  LPCWSTR lpAppName,
  LPCWSTR lpString
  );



BOOL
WINAPI
OWriteProfileStringW(
  LPCWSTR lpAppName,
  LPCWSTR lpKeyName,
  LPCWSTR lpString
  );



DWORD
WINAPI
ODdeQueryStringW(
  DWORD idInst,
  HSZ hsz,
  LPWSTR psz,
  DWORD cchMax,
  int iCodePage);



int WINAPI
OGetClipboardFormatNameW(
  UINT format,
  LPWSTR lpszFormatName,
  int cchMaxCount);



int
WINAPI
OGetKeyNameTextW(
  LONG lParam,
  LPWSTR lpString,
  int nSize);



int
WINAPI
OGetMenuStringW(
  HMENU hMenu,
  UINT uIDItem,
  LPWSTR lpString,
  int nMaxCount,
  UINT uFlag);



//CONSIDER 5525 (daVinci97) - see comment at beginning of file
#endif    //ifdef _DEBUG

} // extern "C"
