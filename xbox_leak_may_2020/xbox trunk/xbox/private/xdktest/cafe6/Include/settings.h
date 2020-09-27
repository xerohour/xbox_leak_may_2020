/////////////////////////////////////////////////////////////////////////////
// settings.h
//
// email	date		change
// briancr	11/02/94	created
//
// copyright 1994 Microsoft

// Interface of the CSettings class

#ifndef __SETTINGS_H__
#define __SETTINGS_H__

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "targxprt.h"

#pragma warning(disable: 4251)

/////////////////////////////////////////////////////////////////////////////
// setting constants

// CAFE
const CString settingCAFEKey = "CAFEKey";
//const CString settingToolset = "Toolset";
const CString settingPlatform = "target_platforms";
const CString settingDoSingleStep = "SingleStep";
const CString settingNewKey = "NewKey";
// UGLY_HACK(briancr): this code is here to log info to a server about who runs CAFE
// remove it as soon as possible
const CString settingBigBro = "BigBro";

// command line
const CString settingRun = "run";
const CString settingSTFFile = "stf";
const CString settingInfiniteIdleWaits = "inf_waits";
const CString settingSleep = "sleep";

// windows (including toolbars)
const CString settingCAFEWndX = "WndX";
const CString settingCAFEWndY = "WndY";
const CString settingCAFEWndWidth = "WndWidth";
const CString settingCAFEWndHeight = "WndHeight";
const CString settingStdToolbar = "CAFETB";
const CString settingCAFETBDock = "CAFETBDock";
const CString settingCAFETBX = "CAFETBX";
const CString settingCAFETBY = "CAFETBY";
const CString settingSubSuiteToolbar = "SSTB";
const CString settingSSTBDock = "SSTBDock";
const CString settingSSTBX = "SSTBX";
const CString settingSSTBY = "SSTBY";
const CString settingViewportToolbar = "PortTB";
const CString settingPortTBDock = "PortTBDock";
const CString settingPortTBX = "PortTBX";
const CString settingPortTBY = "PortTBY";
const CString settingStatusBar = "StatusBar";
const CString settingWaitStepWndX = "WaitStepX";
const CString settingWaitStepWndY = "WaitStepY";
const CString settingWaitStepWndWidth = "WaitStepWidth";
const CString settingWaitStepWndHeight = "WaitStepHeight";


// logging
const CString settingLogResultsFile = "ResultsFile";
const CString settingLogSummaryFile = "SummaryFile";
const CString settingLogWorkDir = "WorkDir";
const CString settingLogMaxFail = "MaxFail";
const CString settingLogUseCom = "UseCom";
const CString settingLogCom = "Com";
const CString settingLogBaudRate = "BaudRate";
const CString settingLogPost = "Post";
const CString settingLogComments = "Comments";

// viewport
const CString settingHeaderFilter = "HeaderFilter";
const CString settingSummaryFilter = "SummaryFilter";
const CString settingCommentsFilter = "CommentsFilter";
const CString settingResultsFilter = "ResultsFilter";
const CString settingFailureFilter = "FailureFilter";
const CString settingInfoFilter = "InfoFilter";
const CString settingSeparatorFilter = "SeparatorFilter";
const CString settingSuccessFilter = "SuccessFilter";
const CString settingStepFilter = "StepFilter";
const CString settingSelectedFilter = "SelectedFilter";

// toolset settings
const CString settingLanguage = "Lang";
const CString settingDebugBuild = "Debug";
const CString settingLanguageKey = "LangKey";
const CString settingDebugBuildKey = "DebugKey";

// toolset environment settings
const CString settingPathEnv = "PATH";
const CString settingLibEnv	= "LIB";
const CString settingIncludeEnv	= "INCLUDE";

// general tool settings
const CString settingDebugProcess = "Debug_Thread";
const CString settingFilename = "Filename";
const CString settingDebugFilename = "DebugFilename";
const CString settingCmdLine = "CmdLine";
const CString settingWorkingDir = "WorkDir";

// GUI tool settings
const CString settingWndTitle = "WndTitle";
const CString settingWndClass = "WndClass";
const CString settingIgnoreASSERT = "IgnoreASSERT";
const CString settingBreakASSERT = "BreakASSERT";
const CString settingNewApp = "NewApp";
const CString settingClose = "Close";
const CString settingPersist = "Persist";

// IDE settings
const CString settingIDEKeyExt = "KeyExt";
const CString settingIDEUseKey = "UseKey";
const CString settingIDEKey = "Key";
const CString settingConnection = "connections";
const CString settingRemoteLocalPath = "remote_local_path";
const CString settingRemoteSharePath = "remote_share_path";

const CString cstrKeyRoot = "Software\\Microsoft\\";

// Serial connection settings
const CString settingConnector = "connector";
const CString settingBaudRate = "baud_rate";
const CString settingFlowControl = "flow_control";
const CString settingDataBits = "data_bits";
const CString settingStopBits = "stop_bits";
const CString settingParity = "parity";

// TCP/IP connection settings
const CString settingRemoteIPAddress = "remote_ip_address";
const CString settingRemoteIPPassword = "remote_ip_password";

// Appletalk connection settings
const CString settingRemoteZone = "remote_zone";
const CString settingRemoteMachName = "remote_machine_name";
const CString settingRemoteATPassword = "remote_at_password";


/////////////////////////////////////////////////////////////////////////////
// CSettings

class TARGET_CLASS CSettings
{
// constructor/destructor
public:
	CSettings(CString strBaseSubKey = "") : m_bReadOnly(FALSE), m_strKey(strBaseSubKey) { }
	virtual ~CSettings();

// operations
public:
	// check whether a setting is defined or not
	virtual BOOL IsDefined(CString strSetting);

	// set the key to write settings to
	BOOL SetKey(CString strKey);

	// set/get text values
	virtual BOOL SetTextValue(CString strSetting, CString strValue, BOOL bOverride = TRUE);
	virtual CString GetTextValue(CString strSetting, CString strDefault = "");

	// set/get boolean values
	virtual BOOL SetBooleanValue(CString strSetting, BOOL bValue, BOOL bOverride = TRUE);
	virtual BOOL GetBooleanValue(CString strSetting, BOOL bDefault = FALSE);
	
	// set/get int values
	virtual BOOL SetIntValue(CString strSetting, int nValue, BOOL bOverride = TRUE);
	virtual int GetIntValue(CString strSetting, int nDefault = 0);

	// delete a setting
	virtual BOOL Delete(CString strSetting);

	// read/write settings from/to registry
	virtual BOOL ReadRegistry(void);
	virtual BOOL WriteRegistry(void);

	// dump settings
	#ifdef _DEBUG
		void DumpSettings(CString strDesc);
	#endif // _DEBUG

// operations (internal)
protected:
	BOOL InterpretBooleanValue(CString &strValue, BOOL bDefault = FALSE);
	BOOL WriteRegistryValue(CString strSetting, CString strValue);
	CString ReadRegistryValue(CString strSetting);
	BOOL DeleteRegistryValue(CString strSetting);

// data
protected:
	CString m_strKey;
	BOOL m_bReadOnly;

	static CString strKeyClass;

	CMapStringToString m_mapSettings;
};

extern TARGET_DATA  BOOL gbLastSubSuite;
extern TARGET_DATA  BOOL gbPersistTarget;

#endif // __SETTINGS_H__
